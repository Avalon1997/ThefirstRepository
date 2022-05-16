#include "servoCtrl.h"
#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include <chrono>
#include <thread>
#include "dispatchProcess.h"
#include "uartdefs.h"
#include "ringbuffer.h"
#include "Easylog.h"
#include "EasyConfig.h"

#define PWM_PIN 1


// 设备地址
//#define DEVICE_ID 0x01


// 最大句柄数量
#define FUNC_HANDLERS_MAX                    ( 2 )



//使用头文件方式，需要声明，否则会报错
template void servoProcess<MessageStruct, RingBuffer>(RingBuffer<MessageStruct>*, RingBuffer<MessageStruct>*, long*);

typedef void(*pFunctionHandler) (unsigned char * pFrame, unsigned short * pLength, RingBuffer<MessageStruct>* buffer, long* deviceID);


/**
 * \brief 功能指针
 */
typedef struct {
	unsigned short functionCode;
	pFunctionHandler pHandler;
} ServoFunctionHandler;


/// 功能句柄
ServoFunctionHandler ServoFuncHandlers[FUNC_HANDLERS_MAX];



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
 * 0 度:暗背景  (5)
 * 45 度:参考	(10)
 * 90度:样品	(15)
 * 设置角度为0度，调整位置到暗背景
 * 0.5ms-------------0度: 2.5% 
 * 1.0ms------------45度: 5.0% 
 * 1.5ms------------90度: 7.5%
 * 2.0ms-----------135度: 10.0%
 * 2.5ms-----------180度: 12.5%
 *
 */
int darkPos = 75;
int refPos = 92;
int samplePos = 112;

/**
* 暗背景
*/
void goDarkPosition()
{
	//3B
	//pwmWrite(PWM_PIN, 50);
	
	//4B
	//pwmWrite(PWM_PIN, 25); //0度
	pwmWrite(PWM_PIN, darkPos);
}

/**
* 暗背景
*/
void setDarkPosition(int position)
{
	//3B
	//pwmWrite(PWM_PIN, 50);
	
	//4B
	//pwmWrite(PWM_PIN, 25); //0度
	pwmWrite(PWM_PIN, position);
}

/**
 * 参考
 */
void goReferencePosition()
{
	//3B
	//pwmWrite(PWM_PIN, 100);
	
	//4B
	//pwmWrite(PWM_PIN, 50);
	pwmWrite(PWM_PIN, refPos);

}

/**
 * 参考
 */
void setReferencePosition(int position)
{
	//3B
	//pwmWrite(PWM_PIN, 100);
	
	//4B
	//pwmWrite(PWM_PIN, 50);
	pwmWrite(PWM_PIN, position);

}

/**
 * 样品
 */
void goSamplePosition()
{
	//3B
	//pwmWrite(PWM_PIN, 150);
	
	//4B
	//pwmWrite(PWM_PIN, 75);
	pwmWrite(PWM_PIN, samplePos);
}

/**
 * 样品
 */
void setSamplePosition(int position)
{
	//3B
	//pwmWrite(PWM_PIN, 150);
	
	//4B
	//pwmWrite(PWM_PIN, 75);
	pwmWrite(PWM_PIN, position);
}

/**
 * \brief 设置氙灯闪烁次数
 * \param pData 待处理数据
 * \param pLen 待处理数据长度
 */
template <typename T1,template <typename T> class C>
void FuncSetSwitchMode(unsigned char * pData,  unsigned short * pLen, C<T1>* output_buffer, long* deviceID)
{
	//printf("FuncSetSwitchMode reply:%d\n",reply);
	
	MessageStruct msg;
	memset(&msg,0,sizeof(msg));

	// pData的前两个字节是命令、组号
	ushortUnion tmpCmd;
	memcpy(tmpCmd.buf,pData,2);

	unsigned char mode = pData[2];
	//printf("FuncSetSwitchMode mode:%d\n",mode);

	switch(mode) {
	case 0: { //dark
		//printf("FuncSetSwitchMode dark\n");
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncSetSwitchMode dark");
		goDarkPosition();
		break;
	}
	case 1: { //reference
		//printf("FuncSetSwitchMode reference\n");
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncSetSwitchMode reference");
		goReferencePosition();
		break;
	}
	case 2: { //sample
		//printf("FuncSetSwitchMode sample\n");
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncSetSwitchMode sample");
		goSamplePosition();
		break;
	}
	default: {
		//printf("FuncSetSwitchMode unknow\n");
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncSetSwitchMode unknow");
		break;
	}

	}


	msg.head.flags.rw = 1;
	msg.head.flags.resp = 0;
	msg.head.seq = 0x01;
	msg.head.addr = ((*deviceID)&0xFFFF);
	msg.head.length = 0x03;
	msg.payload.cmd = CMD_SERVO_SET_SWITCH_MODE;
	msg.payload.data[2] = mode;

	if(!output_buffer->full()) {
		//printf("FuncSetXFlash: input data\n");
		output_buffer->enqueue(msg);
		//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "FuncSetSwitchMode: input ring buffer data");
	}

}

/**
 * \brief 设置氙灯闪烁次数
 * \param pData 待处理数据
 * \param pLen 待处理数据长度
 */
template <typename T1,template <typename T> class C>
void FuncSetSwitchPosition(unsigned char * pData,  unsigned short * pLen, C<T1>* output_buffer, long* deviceID)
{
	MessageStruct msg;
	memset(&msg,0,sizeof(msg));

	unsigned char mode = pData[2];
	
	uIntUnion tmpPos;
	tmpPos.value = 0;
	memset(tmpPos.buf,0,4);
	memcpy(tmpPos.buf, &pData[3],4);
	
	//printf("FuncSetSwitchMode mode:%d\n",mode);

	switch(mode) {
	case 0: { //dark
		//printf("FuncSetSwitchMode dark\n");
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncSetSwitchPosition dark:%d",tmpPos.value);
		setDarkPosition(tmpPos.value);
		break;
	}
	case 1: { //reference
		//printf("FuncSetSwitchMode reference\n");
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncSetSwitchPosition reference:%d",tmpPos.value);
		setReferencePosition(tmpPos.value);
		break;
	}
	case 2: { //sample
		//printf("FuncSetSwitchMode sample\n");
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncSetSwitchPosition sample:%d", tmpPos.value);
		setSamplePosition(tmpPos.value);
		break;
	}
	default: {
		//printf("FuncSetSwitchMode unknow\n");
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncSetSwitchPosition unknow");
		break;
	}

	}


	msg.head.flags.rw = 1;
	msg.head.flags.resp = 0;
	msg.head.seq = 0x01;
	msg.head.addr = ((*deviceID)&0xFFFF);
	msg.head.length = 0x03;
	msg.payload.cmd = CMD_SERVO_SET_SWITCH_POSITION;
	msg.payload.data[2] = mode;

	if(!output_buffer->full()) {
		//printf("FuncSetXFlash: input data\n");
		output_buffer->enqueue(msg);
		//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "FuncSetSwitchMode: input ring buffer data");
	}
}



/**
 * 初始化
 */
void servoProcessInit()
{
	// 功能句柄
	ServoFuncHandlers[0] = { CMD_SERVO_SET_SWITCH_MODE, 		FuncSetSwitchMode };
	ServoFuncHandlers[1] = { CMD_SERVO_SET_SWITCH_POSITION, 	FuncSetSwitchPosition };
	
	// 初始化配置参数
	std::string sysFilePath = "./cfg/sys.conf";
	if (!EasyConf::setPath(sysFilePath))
	{
		printf("Init config error\n");
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Init config error");
		return;
	}
	
	long tmpDarkPos = 0;
	if(!EasyConf::getLongValue("SVO", "Dark", tmpDarkPos))
	{
		printf("get SVO-Dark failure\n");
		return;
	}
	darkPos = (int)tmpDarkPos;
	
	long tmpRefPos = 0;
	if(!EasyConf::getLongValue("SVO", "Ref", tmpRefPos))
	{
		printf("get SVO-Ref failure\n");
		return;
	}
	refPos = (int)tmpRefPos;
	
	long tmpSamplePos = 0;
	if(!EasyConf::getLongValue("SVO", "Sample", tmpSamplePos))
	{
		printf("get SVO-Sample failure\n");
		return;
	}
	samplePos = (int)tmpSamplePos;
	
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Servo position:dark:%d ref:%d sample:%d", darkPos, refPos, samplePos);
	
	

	// 初始化
	pinMode(PWM_PIN, PWM_OUTPUT);	//设置GPIO电气属性
	pwmSetMode(PWM_MODE_MS);		//设置输出模式
	//pwmFrequency in Hz = 19.2MHz / pwmClock / pwmRange.
	

	//3B
	//pwmSetClock (192);
	//pwmSetRange(2000);
	
	//4B
	pwmSetRange (1000);				//设置pwm脉宽范围(pwm脉宽范围 0~200)
	pwmSetClock (1070);			//设置pwm分频系数(250Hz，19.2MHz / 1920 / 200 = 50Hz)

	//for(size_t i=0; i<10; ++i) {
	//	pwmWrite(PWM_PIN,250);
	//	delay(1000);
	//	pwmWrite(PWM_PIN,500);
	//	delay(1000);
	//}
}


template <typename T1,template <typename T> class C>
void servoProcess(C<T1>* servoBuffer, C<T1>* comOutputBuffer, long* deviceID)
{
	//printf("init servo\n");
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Init servo");
	servoProcessInit();

	// 舵机自检
	//printf("servo go dark position\n");
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Servo check self:dark");
	goDarkPosition();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	//printf("servo go reference position\n");
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Servo check self:reference");
	goReferencePosition();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	//printf("servo go sample position\n");
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Servo check self:sample");
	goSamplePosition();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	//printf("servo go dark position\n");
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Servo check self:dark");
	goDarkPosition();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	//printf("Enter servoProcess loop\n");
	
	for(;;) {
		if(!servoBuffer->empty()) {
			const std::shared_ptr<T1> out = servoBuffer->dequeue();
			MessageStruct tmp = (*out);
			
			int i = 0;
			for (i = 0; i < FUNC_HANDLERS_MAX; i++) {
				if (ServoFuncHandlers[i].functionCode == tmp.payload.cmd) {
					//rintf("servoProcess:code:%04x cmd:%04x", ServoFuncHandlers[i].functionCode, tmp.payload.cmd);
					//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "servoProcess:code:%04x cmd:%04x", ServoFuncHandlers[i].functionCode, tmp.payload.cmd);
					ServoFuncHandlers[i].pHandler(tmp.payload.data, &tmp.head.length, comOutputBuffer, deviceID);
					break;
				}
			}
			
			if(i == FUNC_HANDLERS_MAX) {
				EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "servoProcess: no cmd!!!");
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


