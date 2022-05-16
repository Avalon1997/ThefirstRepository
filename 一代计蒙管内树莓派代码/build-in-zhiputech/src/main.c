#include <stdio.h>
#include <thread>
#include <string.h>
#include <wiringPi.h>
#include "EasyCommon.h"
#include "uartdefs.h"
#include "ringbuffer.h"
#include "serialCtrl.h"
#include "dispatchProcess.h"
#include "spectrumProcess.h"
#include "servoCtrl.h"
#include "temperatureCtrl.h"
#include "pingProcess.h"
#include "otherProcess.h"
#include "systemProcess.h"
#include "Easylog.h"
#include "EasyError.h"
#include "EasyConfig.h"
#include "serial/serial.h"


#define MAX_BUFFER_SIZE 2048*10
#define MAX_DATA_SIZE 100

//#define DEVICE_ID 0x01

int main()
{
	/*
	 * 状态说明
	 * +------------------------+-----------+-----------+-----------+
	 * |   位号   |    31~3     |	  2		|	  1		|	  0		|
	 * +------------------------+-----------+-----------+-----------+
	 * |   名称   |  reserved   |   温度 	|   舵机	|	光谱仪	|
	 * +------------------------+-----------+-----------+-----------+
	 * |   说明   |    保留     |  0:正常   |  0:正常   |	0:正常	|
	 * | 		  |  			|  1:异常   |  1:异常	|	1:异常	|
	 * +------------------------+-----------+-----------+-----------+
	 */
	int spectrumStatus = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_DEVICE_NOT_FOUND);
	int servoStatus = 0;
	int temperatureStatus = 0;
	
	int spectrumBusing = 0; // 0:no busing 1:busing 
	
	long deviceID = 0;
	

	// 初始化设备ID
	std::string sysFilePath = "./cfg/sys.conf";
	if (!EasyConf::setPath(sysFilePath))
	{
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Init config error");
		return 1;
	}

	if(!EasyConf::getLongValue("SYSTEM", "DeviceID", deviceID))
	{
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Get device ID error");
		return 1;
	}

	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Get device ID:%d",deviceID);

	// [1-1]初始化树莓派	
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Init respberry");

	if(wiringPiSetup() < 0) {
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Wiring Pi setup error");
		return 1;
	}

	// [1-2]初始化串口
	serial::Serial *m_uartDevice = new serial::Serial("/dev/ttyAMA0",115200, serial::Timeout::simpleTimeout(10));

	if(!m_uartDevice->isOpen()) {
		printf("init serial fail\n");
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to open serial port");
		return 1;
	}

	
	/**
	 * 为了减少模块间的耦合，把每个模块独立到线程中，分配内存如下：
	 * 串口接收：comInputBuffer
	 * 串口发送：comOutputBuffer
	 * 总调度处理后的数据：dispatchBuffer，总调度线程将串口处理后的数据，根据所属的不同线程，分配到不同的线程进行处理
	 * 光谱仪模块需要处理的数据：spectrumBuffer
	 * 舵机模块需要处理的数据：servoBuffer
	 * 温度模块需要处理的数据：temperatureBuffer
	 * 通讯握手：pingBuffer
	 * 其他：otherBuffer
	 */

	// [1-5]初始化内存
	RingBuffer<unsigned char> comInputBuffer(MAX_BUFFER_SIZE);
	//printf("comInputBuffer: size:%d count:%d\n", comInputBuffer.capacity(), comInputBuffer.count());

	RingBuffer<MessageStruct> comOutputBuffer(MAX_DATA_SIZE);
	//printf("comOutputBuffer: size:%d count:%d\n", comOutputBuffer.capacity(), comOutputBuffer.count());

	RingBuffer<MessageStruct> dispatchBuffer(MAX_DATA_SIZE);
	//printf("dispatchBuffer: size:%d count:%d\n", dispatchBuffer.capacity(), dispatchBuffer.count());

	RingBuffer<MessageStruct> spectrumBuffer(MAX_DATA_SIZE);
	//printf("spectrumBuffer: size:%d count:%d\n", spectrumBuffer.capacity(), spectrumBuffer.count());

	RingBuffer<MessageStruct> servoBuffer(MAX_DATA_SIZE);
	//printf("servoBuffer: size:%d count:%d\n", servoBuffer.capacity(), servoBuffer.count());

	RingBuffer<MessageStruct> temperatureBuffer(MAX_DATA_SIZE);
	//printf("temperatureBuffer: size:%d count:%d\n", temperatureBuffer.capacity(), temperatureBuffer.count());

	RingBuffer<MessageStruct> pingBuffer(MAX_DATA_SIZE);
	//printf("pingBuffer: size:%d count:%d\n", pingBuffer.capacity(), pingBuffer.count());

	RingBuffer<MessageStruct> otherBuffer(MAX_DATA_SIZE);
	//printf("otherBuffer: size:%d count:%d\n", otherBuffer.capacity(), otherBuffer.count());

	RingBuffer<MessageStruct> systemBuffer(MAX_DATA_SIZE);
	//printf("systemBuffer: size:%d count:%d\n", systemBuffer.capacity(), systemBuffer.count());


	// [2]启动串口数据采集线程
	std::thread comInputThread(serialRead<unsigned char, RingBuffer>, m_uartDevice, &comInputBuffer);

	// [3]启动采集数据处理线程
	std::thread comInputProcessThread(serialProcess<unsigned char, MessageStruct, RingBuffer>, m_uartDevice, &comInputBuffer, &comOutputBuffer, &dispatchBuffer, &deviceID);

	// [4]启动总调度处理数据执行线程
	std::thread dispatchProcessThread(dispatchProcess<MessageStruct, RingBuffer>, &dispatchBuffer, &spectrumBuffer, &servoBuffer, &temperatureBuffer, &pingBuffer, &otherBuffer, &comOutputBuffer, &systemBuffer, &spectrumStatus, &servoStatus, &temperatureStatus, &spectrumBusing, &deviceID);

	// [5]启动光谱仪处理数据执行线程
	std::thread spectrumProcessThread(spectrumProcess<MessageStruct, RingBuffer>, &spectrumBuffer, &comOutputBuffer, &otherBuffer, &spectrumStatus, &spectrumBusing, &deviceID);

	// [6]启动舵机处理数据执行线程
	std::thread servoProcessThread(servoProcess<MessageStruct, RingBuffer>, &servoBuffer, &comOutputBuffer, &deviceID);

	// [7]启动温度处理数据执行线程
	std::thread temperatureProcessThread(temperatureProcess<MessageStruct, RingBuffer>, &temperatureBuffer, &comOutputBuffer, &deviceID);

	// [8]启动通讯握手处理数据执行线程
	std::thread pingProcessThread(pingProcess<MessageStruct, RingBuffer>, &pingBuffer, &comOutputBuffer, &deviceID);

	// [9]启动其他处理数据执行线程
	std::thread otherProcessThread(otherProcess<MessageStruct, RingBuffer>, &otherBuffer, &comOutputBuffer, &deviceID);

	// [10] 启动串口数据发送线程
	std::thread comOutputThread(serialWrite<MessageStruct,  RingBuffer>, m_uartDevice, &comOutputBuffer);

	// [11] 启动系统处理数据线程
	std::thread systemProcessThread(systemProcess<MessageStruct,  RingBuffer>, &systemBuffer, &comOutputBuffer, &spectrumBuffer, &spectrumStatus, &servoStatus, &temperatureStatus, &deviceID);

	/// 等待其它线程启动，由于线程没有像光谱仪这种容易卡住的设备，所以不传递status，如后期有需要，可进行传递
	std::this_thread::sleep_for(std::chrono::milliseconds(5000)); //单位是毫秒

	MessageStruct msg;
	memset(&msg,0,sizeof(msg));
	msg.head.flags.rw = 1;///* Write = 0; Read = 1 */
	msg.head.flags.reply = 1;
	msg.head.flags.resp = 0;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
	msg.head.seq = 0x0E;
	msg.head.addr = (deviceID&0xFFFF);
	msg.head.length = 0x0E;
	msg.payload.cmd = CMD_SYS_STATUS;


	memcpy(msg.payload.data + 2,(char*)(&spectrumStatus),4);
	memcpy(msg.payload.data + 2 + 4,(char*)(&servoStatus),4);
	memcpy(msg.payload.data + 2 + 4 + 4,(char*)(&temperatureStatus),4);


	if(!comOutputBuffer.full()) {
		comOutputBuffer.enqueue(msg);
	}

	comInputThread.join();
	comInputProcessThread.join();
	dispatchProcessThread.join();
	spectrumProcessThread.join();
	servoProcessThread.join();
	temperatureProcessThread.join();
	pingProcessThread.join();
	otherProcessThread.join();
	comOutputThread.join();
	systemProcessThread.join();

	while(1) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1)); //单位是毫秒
	}
	return 0;
}