#include "serialCtrl.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <thread>
#include <unistd.h>
#include <chrono>
#include "EasyCommon.h"
#include "ringbuffer.h"
#include "uartdefs.h"
#include "Easylog.h"
#include "EasyError.h"
#include "dispatchProcess.h"


//使用头文件方式，需要声明，否则会报错
template void serialRead<unsigned char,RingBuffer>(serial::Serial *, RingBuffer<unsigned char>*);
template void serialWrite<MessageStruct,RingBuffer>(serial::Serial *, RingBuffer<MessageStruct>*);
template void serialProcess<unsigned char,MessageStruct, RingBuffer>(serial::Serial *,RingBuffer<unsigned char>*,RingBuffer<MessageStruct>*,RingBuffer<MessageStruct>*, long*);

//#define DEVICE_ID 0x01

#define TIMEOUT 10 // second

union ushortUnion {
	char buf[2];
	unsigned short value;
};

union uIntUnion {
	char buf[4];
	unsigned int value;
};

union doubleUnion {
	char buf[8];
	double value;
};


template <typename T1, template <typename T> class T2>
void serialRead(serial::Serial *com, T2<T1>* buffer)
{
	unsigned char rd[1024] = {0};
	for(;;) {
		size_t avilDataSize = com->available();
		if(avilDataSize > 0) {  //如果串口缓存中有数据
			size_t rdSize = com->read(rd ,1024);

			for(int i = 0; i < rdSize; i++) {
				if (!buffer->full()) {
					//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "recv[%d]:%02x", i, rd[i]&0xFF);
					buffer->enqueue(rd[i]&0xFF);
				}
			}

		}
		//std::this_thread::sleep_for(std::chrono::milliseconds(1)); //单位是毫秒
		usleep(100);
	}
}



template <typename T1, typename T2, template <typename T> class C>
void serialProcess(serial::Serial *com, C<T1>* raw_buffer, C<T2>* comOutputBuffer, C<T2>* processed_buffer, long* deviceID)
{
	bool headerFlag = false;
	bool header0Flag = false;
	bool header1Flag = false;
	bool header2Flag = false;
	bool header3Flag = false;

	bool trailerFlag = false;
	bool trailer0Flag = false;
	bool trailer1Flag = false;
	bool trailer2Flag = false;
	bool trailer3Flag = false;

	bool timeout = true;

	char *buffer = (char*)malloc(2048 * 8 + 100);
	if(buffer == nullptr) {
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "serialProcess malloc fail");
		return;
	}

	memset(buffer, 0, 2048 * 8 + 100);
	unsigned int bufferIndex = 0;
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime;


	for (;;) {
		if (!raw_buffer->empty()) {
			startTime = std::chrono::high_resolution_clock::now();
			timeout = false;
			const std::shared_ptr<T1> tmpValuePtr = raw_buffer->dequeue();
			char tmpValue = ((*tmpValuePtr) & 0xFF);
			//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "proc:%02x", tmpValue&0xFF);
			// 查找包头
			if(!headerFlag) {
				if(!header0Flag) { // 查找包头第一个字节
					if(UART_START_IND_BYTE_0 == tmpValue) {
						header0Flag = true;
						//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "包头第一个字节");
					} else {
						headerFlag  = false;
						header0Flag = false;
						header1Flag = false;
						header2Flag = false;
						header3Flag = false;
						//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "重新查找包头");
					}
				} else {
					if(header0Flag && !header1Flag) { // 查找包头第二个字节
						if(UART_START_IND_BYTE_1 == tmpValue) {
							header1Flag = true;
							//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "包头第二个字节");
						} else {
							if(UART_START_IND_BYTE_0 == tmpValue) { // 可能存在包头的连续两个第一个字节，以第二个为起始包头的第一个字节，如0x42 0x41 0x41 0x42...
								header0Flag = true;
								//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "包头第一个字节");
							} else {
								headerFlag  = false;
								header0Flag = false;
								header1Flag = false;
								header2Flag = false;
								header3Flag = false;
								//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "重新查找包头");
							}
						}
					} else {
						if(header0Flag && header1Flag && !header2Flag) { // 查找包头第三个字节
							if(UART_START_IND_BYTE_2 == tmpValue) {
								header2Flag = true;
								//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "包头第三个字节");
							} else {
								headerFlag  = false;
								header0Flag = false;
								header1Flag = false;
								header2Flag = false;
								header3Flag = false;
								//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "重新查找包头");
							}
						} else {
							if(header0Flag && header1Flag && header2Flag && !header3Flag) { // 查找包头第四个字节
								if(UART_START_IND_BYTE_3 == tmpValue) {
									header3Flag = true;
									headerFlag = true;
									//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "发现包头");
									continue;//防止将包头的最后一个字节，在包尾进行判断，会将包头的最后一个字节当成数据
								} else {
									headerFlag  = false;
									header0Flag = false;
									header1Flag = false;
									header2Flag = false;
									header3Flag = false;
									//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "重新查找包头");
								}
							}
						}
					}
				}
			}

			// 查找包尾
			if(headerFlag && !trailerFlag) {
				if(!trailer0Flag) {
					if(UART_START_IND_BYTE_3 == tmpValue) { // 查找包尾第一个字节
						trailer0Flag = true;
						//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "包尾第一个字节");
					} else {
						trailerFlag  = false;
						trailer0Flag = false;
						trailer1Flag = false;
						trailer2Flag = false;
						trailer3Flag = false;
						buffer[bufferIndex++] = tmpValue;
						//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "重新查找包尾");
					}
				} else {
					if(trailer0Flag && !trailer1Flag) {
						if(UART_START_IND_BYTE_2 == tmpValue) { // 查找包尾第二个字节
							trailer1Flag = true;
							//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "包尾第二个字节");
						} else {
							trailerFlag  = false;
							trailer0Flag = false;
							trailer1Flag = false;
							trailer2Flag = false;
							trailer3Flag = false;
							buffer[bufferIndex++] = UART_START_IND_BYTE_3;
							buffer[bufferIndex++] = tmpValue;
							//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "重新查找包尾");
						}
					} else {
						if(trailer0Flag && trailer1Flag && !trailer2Flag) {
							if(UART_START_IND_BYTE_1 == tmpValue) { // 查找包尾第三个字节
								trailer2Flag = true;
								//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "包尾第三个字节");
							} else {
								trailerFlag  = false;
								trailer0Flag = false;
								trailer1Flag = false;
								trailer2Flag = false;
								trailer3Flag = false;
								buffer[bufferIndex++] = UART_START_IND_BYTE_3;
								buffer[bufferIndex++] = UART_START_IND_BYTE_2;
								buffer[bufferIndex++] = tmpValue;
								//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "重新查找包尾");
							}
						} else {
							if(trailer0Flag && trailer1Flag && trailer2Flag && !trailer3Flag) {
								if(UART_START_IND_BYTE_0 == tmpValue) { // 查找包尾第四个字节
									trailer3Flag = true;
									trailerFlag = true;
									//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "发现包尾");
								} else {
									trailerFlag  = false;
									trailer0Flag = false;
									trailer1Flag = false;
									trailer2Flag = false;
									trailer3Flag = false;
									buffer[bufferIndex++] = UART_START_IND_BYTE_3;
									buffer[bufferIndex++] = UART_START_IND_BYTE_2;
									buffer[bufferIndex++] = UART_START_IND_BYTE_1;
									buffer[bufferIndex++] = tmpValue;
									//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "重新查找包尾");
								}
							} else {
								buffer[bufferIndex++] = tmpValue;
								//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "数据");
							}
						}
					}
				}

			}

			if(headerFlag && trailerFlag) {
				// EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "-----------------------------------------\n");
				// for(unsigned int i = 0; i < bufferIndex; i++) {
					// EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "data[%d]:%02x",i, buffer[i]&0xFF);
				// }
				// EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "*****************************************\n");

				//处理数据
				if(bufferIndex < 12) {
					// 数据错误
					MessageStruct msg;
					memset(&msg, 0, sizeof(msg));
					msg.head.flags.reply = 1;
					msg.head.seq = 0x01;
					msg.head.addr = ((*deviceID)&0xFFFF);
					msg.head.length = 0x06;

					// pData的前两个字节是命令、组号
					ushortUnion tmpCmd;
					tmpCmd.value = CMD_SYS_CMD;
					memcpy(msg.payload.data + 0, tmpCmd.buf, 2);

					int errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_COM, ERR_MSG_CODE_INVALID_DATA);

					memcpy(msg.payload.data + 2, &errCode, 4);

					if(!comOutputBuffer->full()) {
						comOutputBuffer->enqueue(msg);
					}

					timeout = true;
					bufferIndex = 0;
					headerFlag  = false;
					header0Flag = false;
					header1Flag = false;
					header2Flag = false;
					header3Flag = false;
					trailerFlag  = false;
					trailer0Flag = false;
					trailer1Flag = false;
					trailer2Flag = false;
					trailer3Flag = false;

					return;
				}

				// 数据校验checksum
				uIntUnion checkSum;
				checkSum.buf[0] = buffer[0];
				checkSum.buf[1] = buffer[1];
				checkSum.buf[2] = buffer[2];
				checkSum.buf[3] = buffer[3];

				unsigned int tmpCheckSum = 0;

				for (unsigned int i = 4; i < bufferIndex; i++) {
					tmpCheckSum += static_cast<unsigned char>(buffer[i]);
				}

				MessageStruct msg;
				memset(&msg, 0, sizeof(msg));

				if (tmpCheckSum == checkSum.value) {
					memcpy((char*)&msg, &buffer[4], bufferIndex-4);
					if (!processed_buffer->full()) {
						processed_buffer->enqueue(msg);
					}

				} else {
					// 数据错误
					MessageStruct msg;
					memset(&msg, 0, sizeof(msg));
					msg.head.flags.reply = 1;
					msg.head.seq = 0x01;
					msg.head.addr = ((*deviceID)&0xFFFF);
					msg.head.length = 0x06;

					// pData的前两个字节是命令、组号
					ushortUnion tmpCmd;
					tmpCmd.value = CMD_SYS_CMD;
					memcpy(msg.payload.data + 0, tmpCmd.buf, 2);

					int errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_COM, ERR_MSG_CODE_INVALID_DATA);

					memcpy(msg.payload.data + 2, &errCode, 4);

					if(!comOutputBuffer->full()) {
						comOutputBuffer->enqueue(msg);
					}
				}

				timeout = true;
				bufferIndex = 0;
				headerFlag  = false;
				header0Flag = false;
				header1Flag = false;
				header2Flag = false;
				header3Flag = false;
				trailerFlag  = false;
				trailer0Flag = false;
				trailer1Flag = false;
				trailer2Flag = false;
				trailer3Flag = false;
			}
		}

		if(!timeout) {
			std::chrono::time_point<std::chrono::high_resolution_clock> nowTime = std::chrono::high_resolution_clock::now();
			if(std::chrono::duration_cast<std::chrono::seconds>(nowTime - startTime) >= std::chrono::seconds(TIMEOUT)) { // 超时
				EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "timeout...");

				MessageStruct msg;
				memset(&msg, 0, sizeof(msg));
				msg.head.flags.reply = 1;
				msg.head.seq = 0x01;
				msg.head.addr = ((*deviceID)&0xFFFF);
				msg.head.length = 0x06;

				// pData的前两个字节是命令、组号
				ushortUnion tmpCmd;
				tmpCmd.value = CMD_SYS_CMD;
				memcpy(msg.payload.data + 0, tmpCmd.buf, 2);

				int errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_COM, ERR_MSG_CODE_TIMEOUT);

				memcpy(msg.payload.data + 2, &errCode, 4);

				if(!comOutputBuffer->full()) {
					comOutputBuffer->enqueue(msg);
				}

				// 清空输入的缓存
				com->flushInput();
				timeout = true;
				bufferIndex = 0;
				headerFlag  = false;
				header0Flag = false;
				header1Flag = false;
				header2Flag = false;
				header3Flag = false;
				trailerFlag  = false;
				trailer0Flag = false;
				trailer1Flag = false;
				trailer2Flag = false;
				trailer3Flag = false;
			}
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1)); //单位是毫秒
	}
}


template <typename T1, template <typename T> class T2>
void serialWrite(serial::Serial *com, T2<T1>* buffer)
{
	for(;;) {
		if (!buffer->empty()) {
			const std::shared_ptr<T1> out = buffer->dequeue();
			MessageStruct tmp =(*out);
			//printf("[%s] output:flags:%02x addr:%04x seq:%02x length:%02x cmd:%04x\n",getSystemDate(), tmp.head.flags, tmp.head.addr, tmp.head.seq, tmp.head.length, tmp.payload.cmd);

			// 数据打包
			// 申请内存
			unsigned int sendSize = 4 + 4 + 1 + 1 + 2 + 2 + tmp.head.length + 4 + 1;
			unsigned char* send = (unsigned char*)malloc(sendSize);
			memset((char*)send,0,sendSize);
			// 字节序号
			unsigned int index = 0;
			// [1]包头4字节
			send[index++] = UART_START_IND_BYTE_0;
			send[index++] = UART_START_IND_BYTE_1;
			send[index++] = UART_START_IND_BYTE_2;
			send[index++] = UART_START_IND_BYTE_3;
			// [2]校验和4字节
			unsigned int checkSum = 0;

			checkSum += *((unsigned char *)&tmp.head.flags);
			checkSum += tmp.head.seq;
			checkSum += (tmp.head.addr&0xFF);
			checkSum += ((tmp.head.addr >> 8)&0xFF);
			checkSum += (tmp.head.length&0xFF);
			checkSum += ((tmp.head.length >> 8)&0xFF);

			for(int i = 0; i < tmp.head.length; i++) {
				checkSum += (unsigned char)tmp.payload.data[i];
			}

			send[index++] = (checkSum)&0xFF;
			send[index++] = (checkSum >> 8)&0xFF;
			send[index++] = (checkSum >> 16)&0xFF;
			send[index++] = (checkSum >> 24)&0xFF;

			//printf("checksum:%06x %06d\n",checkSum,checkSum);

			// [3]标识位1字节
			send[index++] = *((unsigned char *)&tmp.head.flags);

			// [4]包序号1字节
			send[index++] = tmp.head.seq;

			// [5]设备地址2字节
			send[index++] = tmp.head.addr&0xFF;
			send[index++] = (tmp.head.addr >> 8)&0xFF;

			// [6]数据长度2字节
			send[index++] = tmp.head.length&0xFF;
			send[index++] = (tmp.head.length >> 8)&0xFF;

			// [7]数据
			for(int i = 0; i < tmp.head.length; i++) {
				send[index++] = tmp.payload.data[i];
			}

			// [1]包尾4字节
			send[index++] = UART_END_IND_BYTE_0;
			send[index++] = UART_END_IND_BYTE_1;
			send[index++] = UART_END_IND_BYTE_2;
			send[index++] = UART_END_IND_BYTE_3;

			for(unsigned int i = 0; i < index; i++)
			{
				com->write(&send[i], 1);
				usleep(1);
			}
			
			free(send);
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1)); //单位是毫秒
	}
}










