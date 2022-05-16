#include "otherProcess.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <chrono>
#include <thread>
#include <wiringPi.h>
#include "dispatchProcess.h"
#include "uartdefs.h"
#include "ringbuffer.h"
#include "EasyCommon.h"
#include "Easylog.h"

#define LHL_PULSE_PIN 3


//使用头文件方式，需要声明，否则会报错
template void otherProcess<MessageStruct, RingBuffer>(RingBuffer<MessageStruct>*, RingBuffer<MessageStruct>*, long*);

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
} OtherFunctionHandler;


/// 功能句柄
OtherFunctionHandler OtherFuncHandlers[FUNC_HANDLERS_MAX];


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



/**
 * \brief 设置电平变化 0-1-0
 * \param pData 待处理数据
 * \param pLen 待处理数据长度
 * \param comOutputBuffer 需要串口发送的数据
 */
template <typename T1,template <typename T> class C>
void FuncSetLHLPulse(unsigned char * pData,  unsigned short * pLen, C<T1>* comOutputBuffer, long* deviceID)
{
	//printf("--------------------FuncSetLHLPulse\n");
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncSetLHLPulse");
	digitalWrite(LHL_PULSE_PIN,LOW);//'HIGH'代表高电平，相当于1，’LOW‘则为低电平，相当于’0‘
	delayMicroseconds(100);
	digitalWrite(LHL_PULSE_PIN,HIGH);//'HIGH'代表高电平，相当于1，’LOW‘则为低电平，相当于’0‘
	delayMicroseconds(100);
	digitalWrite(LHL_PULSE_PIN,LOW);//'HIGH'代表高电平，相当于1，’LOW‘则为低电平，相当于’0‘
	
	//printf("[%s] Sucess to set LHL pulse\n",getSystemDate());
	//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Sucess to set LHL pulse");
	MessageStruct msg;
	memset(&msg,0,sizeof(msg));
	msg.head.flags.rw = 0;
	msg.head.flags.resp = 0;
	msg.head.seq = 0x01;
	msg.head.addr = ((*deviceID)&0xFFFF);
	msg.head.length = 0x02;
	msg.payload.cmd = CMD_OTHER_LHL_PULSE;

	if(!comOutputBuffer->full()) {
		//printf("FuncSetXFlash: input data\n");
		comOutputBuffer->enqueue(msg);
		//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "FuncSetLHLPulse: input ring buffer data");
	}
}


/**
 * \brief 初始化处理函数句柄
 */
void otherProcessInit()
{
	// 功能句柄
	OtherFuncHandlers[0] = { CMD_OTHER_LHL_PULSE, 		FuncSetLHLPulse };
	
	pinMode(LHL_PULSE_PIN,OUTPUT);//设置（Pin 3）为输出模式，若为’INPUT‘则为输入模式
	digitalWrite(LHL_PULSE_PIN,LOW);//'HIGH'代表高电平，相当于1，’LOW‘则为低电平，相当于’0‘
}



template <typename T1,template <typename T> class C>
void otherProcess(C<T1>* otherBuffer, C<T1>* comOutputBuffer, long* deviceID)
{
	otherProcessInit();

	//printf("Enter otherProcess loop\n");

	for(;;) {
		if(!otherBuffer->empty()) {
			const std::shared_ptr<T1> out = otherBuffer->dequeue();
			MessageStruct tmp = (*out);
			
			int i = 0;
			for (i = 0; i < FUNC_HANDLERS_MAX; i++) {
				if (OtherFuncHandlers[i].functionCode == tmp.payload.cmd) {
					//printf("otherProcess:code:%04x cmd:%04x\n", OtherFuncHandlers[i].functionCode, tmp.payload.cmd);
					//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "otherProcess:code:%04x cmd:%04x", OtherFuncHandlers[i].functionCode, tmp.payload.cmd);
					OtherFuncHandlers[i].pHandler(tmp.payload.data, &tmp.head.length, comOutputBuffer, deviceID);
					break;
				}
			}
			
			if(i == FUNC_HANDLERS_MAX) {
				EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "otherProcess: no cmd!!!");
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









