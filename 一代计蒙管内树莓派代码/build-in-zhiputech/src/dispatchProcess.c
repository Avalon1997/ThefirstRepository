#include "dispatchProcess.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <chrono>
#include <thread>
#include "uartdefs.h"
#include "ringbuffer.h"
#include "EasyCommon.h"
#include "Easylog.h"
#include "EasyError.h"

//使用头文件方式，需要声明，否则会报错
template void dispatchProcess<MessageStruct, RingBuffer>(RingBuffer<MessageStruct>*, RingBuffer<MessageStruct>*, RingBuffer<MessageStruct>*, RingBuffer<MessageStruct>*, RingBuffer<MessageStruct>*, RingBuffer<MessageStruct>*, RingBuffer<MessageStruct>*, RingBuffer<MessageStruct>*, int*, int*, int*, int*, long*);

// 设备地址
//#define DEVICE_ID 0x01


template <typename T1,template <typename T> class C>
void dispatchProcess(C<T1>* dispatchBuffer, C<T1>* spectrumBuffer, C<T1>* servoBuffer, C<T1>* temperatureBuffer, C<T1>* pingBuffer, C<T1>* otherBuffer, C<T1>* comOutputBuffer, C<T1>* systemBuffer, int* spectrumStatus, int* servoStatus, int* temperatureStatus, int* spectrumBusing, long* deviceID)
{
	//printf("Enter dispatchProcess loop\n");
	for(;;) {
		if(!dispatchBuffer->empty()) {
			const std::shared_ptr<T1> out = dispatchBuffer->dequeue();
			MessageStruct tmp = (*out);
			//printf("addr:%d\n",tmp.head.addr);
			if(((*deviceID)&0xFFFF) == tmp.head.addr) {
				//printf("flags:%02x addr:%04x seq:%02x length:%02x cmd:%04x\n", tmp.head.flags, tmp.head.addr, tmp.head.seq, tmp.head.length, tmp.payload.cmd);
				//printf("data:%x- key:%x\n",tmp.payload.cmd, CMD_GET_KEY(tmp.payload.cmd));
				//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "data:%x- key:%x",tmp.payload.cmd, CMD_GET_KEY(tmp.payload.cmd));
				switch(CMD_GET_KEY(tmp.payload.cmd)) {
				case CMD_SPEC: { ///< 光谱仪
					//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "dispatchProcess: busing status:%d", *spectrumBusing);
					if( *spectrumBusing == 1 ) {
						EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "spectrum status busing");
						MessageStruct msg;
						memset(&msg,0,sizeof(msg));
						msg.head.flags.rw = 1;///* Write = 0; Read = 1 */
						msg.head.flags.reply = 1;
						msg.head.flags.resp = 0;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
						msg.head.seq = 0x0E;
						msg.head.addr = ((*deviceID)&0xFFFF);
						msg.head.length = 0x0E;
						msg.payload.cmd = CMD_SYS_STATUS;

						int busing = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_BUSY);
						memcpy(msg.payload.data + 2,(char*)(&busing),4);
						memcpy(msg.payload.data + 2 + 4,(char*)(servoStatus),4);
						memcpy(msg.payload.data + 2 + 4 + 4,(char*)(temperatureStatus),4);


						if(!comOutputBuffer->full()) {
							comOutputBuffer->enqueue(msg);
						}
					} else {
						if(!spectrumBuffer->full()) {
							spectrumBuffer->enqueue(tmp);
						}
					}
					break;
				}

				case CMD_SERVO: { ///< 舵机
					if(!servoBuffer->full()) {
						servoBuffer->enqueue(tmp);
					}
					break;
				}

				case CMD_TEMP: { ///< 温度
					if(!temperatureBuffer->full()) {
						temperatureBuffer->enqueue(tmp);
					}
					break;
				}

				case CMD_PING: { ///< 通信
					if(!pingBuffer->full()) {
						pingBuffer->enqueue(tmp);
					}
					break;
				}

				case CMD_OTHER: { ///< 其他
					if(!otherBuffer->full()) {
						otherBuffer->enqueue(tmp);
					}
					break;
				}
				case CMD_SYS: {
					if(!systemBuffer->full()) {
						systemBuffer->enqueue(tmp);
					}
					break;
				}
				default: {
					printf("unknow cmd!!!\n");
					EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "unknow cmd!!!");
					MessageStruct msg;
					memset(&msg,0,sizeof(msg));
					msg.head.flags.rw = 1;///* Write = 0; Read = 1 */
					msg.head.flags.reply = 1;
					msg.head.flags.resp = 1;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
					msg.head.seq = 0x01;
					msg.head.addr = ((*deviceID)&0xFFFF);
					msg.head.length = 0x02;
					msg.payload.cmd = CMD_SYS_CMD;

					if(!comOutputBuffer->full()) {
						comOutputBuffer->enqueue(msg);
					}
				}
				}
			}

		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1)); //单位是毫秒
	}
}









