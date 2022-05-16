#include "pingProcess.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <chrono>
#include <thread>
#include "dispatchProcess.h"
#include "uartdefs.h"
#include "ringbuffer.h"
#include "Easylog.h"


//使用头文件方式，需要声明，否则会报错
template void pingProcess<MessageStruct, RingBuffer>(RingBuffer<MessageStruct>*, RingBuffer<MessageStruct>*, long*);

// 设备地址
//#define DEVICE_ID 0x01


// 最大句柄数量
#define FUNC_HANDLERS_MAX                    ( 1 )

typedef void(*pFunctionHandler) (unsigned char * pFrame, unsigned short * pLength, RingBuffer<MessageStruct>* buffer, long* deviceID);


/**
 * \brief 功能指针
 */
typedef struct {
	unsigned short functionCode;
	pFunctionHandler pHandler;
} PingFunctionHandler;


/// 功能句柄
PingFunctionHandler PingFuncHandlers[FUNC_HANDLERS_MAX];




/**
 * \brief 设置氙灯闪烁次数
 * \param pData 待处理数据
 * \param pLen 待处理数据长度
 */
template <typename T1,template <typename T> class C>
void FuncPing(unsigned char * pData,  unsigned short * pLen, C<T1>* output_buffer, long* deviceID)
{
	//printf("FuncPing\n");
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncPing");
	MessageStruct msg;
	memset(&msg,0,sizeof(msg));
	msg.head.flags.rw = 0;///* Write = 0; Read = 1 */
	msg.head.flags.reply = 1;
	msg.head.flags.resp = 0;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
	msg.head.seq = 0x01;
	msg.head.addr = ((*deviceID)&0xFFFF);
	msg.head.length = 0x02;
	msg.payload.cmd = CMD_PING_PING_PING;

	if(!output_buffer->full()) {
		//printf("FuncSetCfg: input data\n");
		output_buffer->enqueue(msg);
		//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "FuncPing: input ring buffer data");
	}

}

/**
 * 初始化
 */
void pingProcessInit()
{
	// 功能句柄
	PingFuncHandlers[0] = { CMD_PING_PING_PING, 	FuncPing };

}


template <typename T1,template <typename T> class C>
void pingProcess(C<T1>* pingBuffer, C<T1>* comOutputBuffer, long* deviceID)
{
	//printf("init ping\n");
	pingProcessInit();
	//printf("Enter pingProcess loop\n");
	for(;;) {
		if(!pingBuffer->empty()) {
			const std::shared_ptr<T1> out = pingBuffer->dequeue();
			MessageStruct tmp = (*out);
			int i = 0;
			for (i = 0; i < FUNC_HANDLERS_MAX; i++) {
				if (PingFuncHandlers[i].functionCode == tmp.payload.cmd) {
					//printf("pingProcess:code:%04x cmd:%04x\n", PingFuncHandlers[i].functionCode, tmp.payload.cmd);
					EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "pingProcess:code:%04x cmd:%04x", PingFuncHandlers[i].functionCode, tmp.payload.cmd);
					PingFuncHandlers[i].pHandler(tmp.payload.data, &tmp.head.length, comOutputBuffer, deviceID);
					break;
				}
			}
			
			if(i == FUNC_HANDLERS_MAX) {
				EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "pingProcess: no cmd!!!");
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
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1)); //单位是毫秒
	}
}


