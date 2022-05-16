#include "systemProcess.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <chrono>
#include <thread>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/reboot.h>
#include "dispatchProcess.h"
#include "uartdefs.h"
#include "ringbuffer.h"
#include "EasyCommon.h"
#include "Easylog.h"




//使用头文件方式，需要声明，否则会报错
template void systemProcess<MessageStruct, RingBuffer>(RingBuffer<MessageStruct>*, RingBuffer<MessageStruct>*, RingBuffer<MessageStruct>*, int*, int*, int*, long*);

// 设备地址
//#define DEVICE_ID 0x01


// 最大句柄数量
#define FUNC_HANDLERS_MAX                    ( 5 )

typedef void(*pFunctionHandler) (unsigned char * pFrame, unsigned short * pLength, RingBuffer<MessageStruct>* buffer, RingBuffer<MessageStruct>* spectrumBuffer, int* spectrumStatus, int* servoStatus, int* temperatureStatus, long* deviceID);


/**
 * \brief 功能指针
 */
typedef struct {
	unsigned short functionCode;
	pFunctionHandler pHandler;
} SystemFunctionHandler;


/// 功能句柄
SystemFunctionHandler SystemFuncHandlers[FUNC_HANDLERS_MAX];


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

typedef struct {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
} EasyTime;



bool setSystemTime(short year, char month, char day, char hour, char minute, char second)
{
	struct tm *p = new struct tm();
	struct timeval tv;
	struct timezone tz;
	gettimeofday (&tv , &tz);//获取时区保存tz中
	p->tm_year = year - 1900;
	p->tm_mon = month - 1;
	p->tm_mday = day;
	p->tm_hour = hour;
	p->tm_min = minute;
	p->tm_sec = second;
	time_t utc_t = mktime(p);
	delete(p);
	tv.tv_sec = utc_t;
	tv.tv_usec = 0;
	if (settimeofday(&tv, &tz) < 0) {
		return false;
	}
	return true;
}

EasyTime getSystemLocalTime()
{
	EasyTime tmp;
	time_t timep;
	struct tm *p;
	time(&timep);
	p = localtime(&timep); //取得当地时间
	tmp.year = 1900 + p->tm_year;
	tmp.month = 1 + p->tm_mon;
	tmp.day = p->tm_mday;
	tmp.hour = p->tm_hour;
	tmp.minute = p->tm_min;
	tmp.second = p->tm_sec;
	return tmp;
}


/**
 * \brief 同步系统时间
 * \param pData 待处理数据
 * \param pLen 待处理数据长度
 * \param comOutputBuffer 需要串口发送的数据
 */
template <typename T1,template <typename T> class C>
void FuncSyncTime(unsigned char * pData,  unsigned short * pLen, C<T1>* comOutputBuffer, C<T1>* spectrumBuffer, int* spectrumStatus, int* servoStatus, int* temperatureStatus, long* deviceID)
{
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncSyncTime");

	ushortUnion tmpCmd;
	memcpy(tmpCmd.buf,pData,2);

	ushortUnion tmpYear;
	memcpy(tmpYear.buf,pData + 2,2);

	short year = tmpYear.value;
	char month = pData[4];
	char day = pData[5];
	char hour = pData[6];
	char minute = pData[7];
	char second = pData[8];

	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "sync time: %04d-%d-%d %d:%d:%d",year, month, day, hour, minute, second);

	MessageStruct msg;
	memset(&msg,0,sizeof(msg));


	if(setSystemTime(year, month, day, hour, minute, second)) {
		msg.head.flags.resp = 0;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
	} else {
		msg.head.flags.resp = 1;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
	}

	EasyTime tmpTime = getSystemLocalTime();

	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "local time: %04d-%d-%d %d:%d:%d",tmpTime.year, tmpTime.month, tmpTime.day, tmpTime.hour, tmpTime.minute, tmpTime.second);


	msg.head.flags.rw = 0;/* Write = 0; Read = 1 */
	msg.head.seq = 0x01;
	msg.head.addr = ((*deviceID)&0xFFFF);
	msg.head.length = 0x09;
	msg.payload.cmd = CMD_SYS_SYNC_TIME;

	memcpy(msg.payload.data + 2,&tmpTime.year, 2);
	memcpy(msg.payload.data + 4,&tmpTime.month,1);
	memcpy(msg.payload.data + 5,&tmpTime.day, 1);
	memcpy(msg.payload.data + 6,&tmpTime.hour, 1);
	memcpy(msg.payload.data + 7,&tmpTime.minute, 1);
	memcpy(msg.payload.data + 8,&tmpTime.second, 1);

	if(!comOutputBuffer->full()) {
		comOutputBuffer->enqueue(msg);
	}
}

/**
 * \brief 重启系统
 * \param pData 待处理数据
 * \param pLen 待处理数据长度
 * \param comOutputBuffer 需要串口发送的数据
 */
template <typename T1,template <typename T> class C>
void FuncSysReboot(unsigned char * pData,  unsigned short * pLen, C<T1>* comOutputBuffer, C<T1>* spectrumBuffer, int* spectrumStatus, int* servoStatus, int* temperatureStatus, long* deviceID)
{
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncReboot");


	//printf("[%s] Sucess to set LHL pulse\n",getSystemDate());
	//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Sucess to set LHL pulse");
	MessageStruct msg;
	memset(&msg,0,sizeof(msg));
	msg.head.flags.rw = 0;
	msg.head.flags.resp = 0;
	msg.head.seq = 0x01;
	msg.head.addr = ((*deviceID)&0xFFFF);
	msg.head.length = 0x02;
	msg.payload.cmd = CMD_SYS_REBOOT;

	if(!comOutputBuffer->full()) {
		comOutputBuffer->enqueue(msg);
	}

	reboot(RB_AUTOBOOT);
}

/**
 * \brief 获取系统状态
 * \param pData 待处理数据
 * \param pLen 待处理数据长度
 * \param comOutputBuffer 需要串口发送的数据
 */
template <typename T1,template <typename T> class C>
void FuncGetStatus(unsigned char * pData,  unsigned short * pLen, C<T1>* comOutputBuffer, C<T1>* spectrumBuffer, int* spectrumStatus, int* servoStatus, int* temperatureStatus, long* deviceID)
{
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncGetStatus");

	MessageStruct msg;
	memset(&msg,0,sizeof(msg));
	msg.head.flags.rw = 1;///* Write = 0; Read = 1 */
	msg.head.flags.reply = 1;
	msg.head.flags.resp = 0;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
	msg.head.seq = 0x01;
	msg.head.addr = ((*deviceID)&0xFFFF);
	msg.head.length = 0x0E;
	msg.payload.cmd = CMD_SYS_STATUS;


	memcpy(msg.payload.data + 2,(char*)(spectrumStatus),4);
	memcpy(msg.payload.data + 2 + 4,(char*)(servoStatus),4);
	memcpy(msg.payload.data + 2 + 4 + 4,(char*)(temperatureStatus),4);


	if(!comOutputBuffer->full()) {
		comOutputBuffer->enqueue(msg);
	}
}

/**
 * \brief 重启光谱仪系统
 * \param pData 待处理数据
 * \param pLen 待处理数据长度
 * \param comOutputBuffer 需要串口发送的数据
 */
template <typename T1,template <typename T> class C>
void FuncSpecReboot(unsigned char * pData,  unsigned short * pLen, C<T1>* comOutputBuffer, C<T1>* spectrumBuffer, int* spectrumStatus, int* servoStatus, int* temperatureStatus, long* deviceID)
{
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncSpecReboot");

	MessageStruct msg;
	memset(&msg,0,sizeof(msg));
	msg.head.flags.rw = 0;
	msg.head.flags.resp = 0;
	msg.head.seq = 0x01;
	msg.head.addr = ((*deviceID)&0xFFFF);
	msg.head.length = 0x02;
	msg.payload.cmd = CMD_SPEC_REBOOT;

	if(!spectrumBuffer->full()) {
		spectrumBuffer->enqueue(msg);
	}
}

/**
 * \brief 重启程序
 * \param pData 待处理数据
 * \param pLen 待处理数据长度
 * \param comOutputBuffer 需要串口发送的数据
 */
template <typename T1,template <typename T> class C>
void FuncSelfReboot(unsigned char * pData,  unsigned short * pLen, C<T1>* comOutputBuffer, C<T1>* spectrumBuffer, int* spectrumStatus, int* servoStatus, int* temperatureStatus, long* deviceID)
{
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncSelfReboot");

	MessageStruct msg;
	memset(&msg,0,sizeof(msg));
	msg.head.flags.rw = 0;
	msg.head.flags.resp = 0;
	msg.head.seq = 0x01;
	msg.head.addr = ((*deviceID)&0xFFFF);
	msg.head.length = 0x02;
	msg.payload.cmd = CMD_SYS_SELF_REBOOT;

	if(!comOutputBuffer->full()) {
		comOutputBuffer->enqueue(msg);
	}
	
	//程序启动命令
    char *args[] = {(char*)"water-monitor-daq.exe",(char*)0, 0};
    //调用系统内核
    execv("/proc/self/exe",args);
    
	exit(0);
}


/**
 * \brief 初始化处理函数句柄
 */
void SystemProcessInit()
{
	// 功能句柄
	SystemFuncHandlers[0] = { CMD_SYS_SYNC_TIME, 		FuncSyncTime };
	SystemFuncHandlers[1] = { CMD_SYS_REBOOT, 			FuncSysReboot };
	SystemFuncHandlers[2] = { CMD_SYS_STATUS, 			FuncGetStatus };
	SystemFuncHandlers[3] = { CMD_SYS_SPEC_REBOOT, 		FuncSpecReboot };
	SystemFuncHandlers[4] = { CMD_SYS_SELF_REBOOT, 		FuncSelfReboot };
}



template <typename T1,template <typename T> class C>
void systemProcess(C<T1>* systemBuffer, C<T1>* comOutputBuffer, C<T1>* spectrumBuffer, int* spectrumStatus, int* servoStatus, int* temperatureStatus, long* deviceID)
{
	SystemProcessInit();

	for(;;) {
		if(!systemBuffer->empty()) {
			const std::shared_ptr<T1> out = systemBuffer->dequeue();
			MessageStruct tmp = (*out);
			int i = 0;
			for (i = 0; i < FUNC_HANDLERS_MAX; i++) {
				if (SystemFuncHandlers[i].functionCode == tmp.payload.cmd) {
					//printf("otherProcess:code:%04x cmd:%04x\n", OtherFuncHandlers[i].functionCode, tmp.payload.cmd);
					//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "otherProcess:code:%04x cmd:%04x", OtherFuncHandlers[i].functionCode, tmp.payload.cmd);
					SystemFuncHandlers[i].pHandler(tmp.payload.data, &tmp.head.length, comOutputBuffer, spectrumBuffer, spectrumStatus, servoStatus, temperatureStatus, deviceID);
					break;
				}
			}
			
			if(i == FUNC_HANDLERS_MAX) {
				EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "systemProcess: no cmd!!!");
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









