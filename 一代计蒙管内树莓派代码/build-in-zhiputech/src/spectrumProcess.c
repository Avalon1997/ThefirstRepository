#include "spectrumProcess.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <chrono>
#include <thread>
#include <unistd.h>
#include "dispatchProcess.h"
#include "uartdefs.h"
#include "ringbuffer.h"
#include "avaspec.h"
#include "type.h"
#include "EasyCommon.h"
#include "Easylog.h"
#include "EasyError.h"

#define NJ_ZP_Tech 1



//使用头文件方式，需要声明，否则会报错
template void spectrumProcess<MessageStruct, RingBuffer>(RingBuffer<MessageStruct>*, RingBuffer<MessageStruct>*, RingBuffer<MessageStruct>*, int*, int*, long*);

// 设备地址
//#define DEVICE_ID 0x01

#define DELAY_OFFSET 2000	// 开始采集与获取数据间的时间间隔
#define REPEAT_NUM_OFFSET 1 // 没有采集到数据后，重复触发次数


// 最大句柄数量
#define FUNC_HANDLERS_MAX                    ( 5 )

typedef void(*pFunctionHandler) (unsigned char * pFrame, unsigned short * pLength, RingBuffer<MessageStruct>* buffer, RingBuffer<MessageStruct>* otherBuffer, int* status, int* busing, long* deviceID);


/**
 * \brief 功能指针
 */
typedef struct {
	unsigned short functionCode;
	pFunctionHandler pHandler;
} SpectrumFunctionHandler;


/// 功能句柄
SpectrumFunctionHandler SpectrumFuncHandlers[FUNC_HANDLERS_MAX];

// 光谱仪句柄
AvsHandle _avsHandle;
avsMessageStruct avsMsg;



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
 * \brief 初始化处理函数句柄
 */
int spectrumPrepareMeasure()
{
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------spectrumPrepareMeasure");
	MeasConfigType measConfigType;
	measConfigType.m_StartPixel = 0;
	measConfigType.m_StopPixel = 2047;

	measConfigType.m_IntegrationTime = avsMsg.integrationTime;             // 积分时间
	measConfigType.m_IntegrationDelay = 0;

	measConfigType.m_NrAverages = avsMsg.averagesCount;                    // 平均次数

	measConfigType.m_CorDynDark.m_Enable = 0;
	measConfigType.m_CorDynDark.m_ForgetPercentage = 0;

	measConfigType.m_Smoothing.m_SmoothPix = avsMsg.smoothPixCount;         // 平滑次数
	measConfigType.m_Smoothing.m_SmoothModel = 0;

	measConfigType.m_SaturationDetection = 0;

	measConfigType.m_Trigger.m_Mode = 1;		// 1:硬件触发
	measConfigType.m_Trigger.m_Source = 0;
	measConfigType.m_Trigger.m_SourceType = 0;

	measConfigType.m_Control.m_StrobeControl = 0;
	measConfigType.m_Control.m_LaserDelay = 0;
	measConfigType.m_Control.m_LaserWidth = 0;
	measConfigType.m_Control.m_LaserWaveLength = 0;
	measConfigType.m_Control.m_StoreToRam = 0;


	int retSpec = AVS_PrepareMeasure(_avsHandle, &measConfigType);

	if(ERR_SUCCESS == retSpec) {
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "spectrumPrepareMeasure: Succeed to AVS_PrepareMeasure");
		return getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_NO);
	} else if(ERR_DEVICE_NOT_FOUND == retSpec) {
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "spectrumPrepareMeasure: Fail to AVS_PrepareMeasure errorCode:%d",retSpec);
		return getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_DEVICE_NOT_FOUND);
	} else if(ERR_OPERATION_PENDING == retSpec) {
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "spectrumPrepareMeasure: Fail to AVS_PrepareMeasure errorCode:%d",retSpec);
		return getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_OPERATION_PENDING);
	} else if(ERR_INVALID_DEVICE_ID == retSpec) {
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "spectrumPrepareMeasure: Fail to AVS_PrepareMeasure errorCode:%d",retSpec);
		return getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_DEVICE_ID);
	} else if(ERR_INVALID_PARAMETER == retSpec) {
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "spectrumPrepareMeasure: Fail to AVS_PrepareMeasure errorCode:%d",retSpec);
		return getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_PARAMETER);
	} else if(ERR_INVALID_PIXEL_RANGE == retSpec) {
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "spectrumPrepareMeasure: Fail to AVS_PrepareMeasure errorCode:%d",retSpec);
		return getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_PIXEL_RANGE);
	} else if(ERR_INVALID_CONFIGURATION == retSpec) {
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "spectrumPrepareMeasure: Fail to AVS_PrepareMeasure errorCode:%d",retSpec);
		return getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_CONFIGURATION);
	} else if(ERR_TIMEOUT == retSpec) {
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "spectrumPrepareMeasure: Fail to AVS_PrepareMeasure errorCode:%d",retSpec);
		return getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_TIMEDOUT);
	} else if(ERR_INVALID_MEASPARAM_DYNDARK == retSpec) {
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "spectrumPrepareMeasure: Fail to AVS_PrepareMeasure errorCode:%d",retSpec);
		return getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_MEASPARAM_DYNDARK);
	} else {
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "spectrumPrepareMeasure: Fail to AVS_PrepareMeasure errorCode:%d",retSpec);
		return getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_UNKNOW);
	}
}



/**
 * \brief 初始化处理函数句柄
 */
template <typename T1,template <typename T> class C>
int spectrumProcessReinit(C<T1>* comOutputBuffer, C<T1>* otherBuffer, int* status, int* busing, long* deviceID)
{
	// 需要等待设备连接
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "spectrumProcessReinit");

	// 触发信号
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "主动触发信号");

	MessageStruct msg;
	memset(&msg,0,sizeof(msg));
	msg.head.flags.rw = 0;
	msg.head.flags.resp = 0;
	msg.head.seq = 0x01;
	msg.head.addr = ((*deviceID)&0xFFFF);
	msg.head.length = 0x02;
	msg.payload.cmd = CMD_OTHER_LHL_PULSE;

	if(!otherBuffer->full()) {
		otherBuffer->enqueue(msg);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(5000)); //单位是毫秒


	*status = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_REBOOT);

	// 断开原先连接
	AVS_Deactivate(_avsHandle);

	// Avantes初始化
	int retAvs = AVS_Init(0); // 0:USB
	//printf("avs spectram connect ret:%d",retAvs);
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "avs spectram connect ret:%d",retAvs);

	do {
		if(retAvs > 0) {
			break;
		} else {
			std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //单位是毫秒
			int errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_DEVICE_NOT_FOUND);
			*status = errCode;
			EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_Init:%d msg:%s",errCode, getErrorMsg(errCode));
		}

	} while((retAvs = AVS_Init(0)) <= 0);


	AvsIdentityType avsIdentityType;
	unsigned int reqSize;

	retAvs =  AVS_GetList(sizeof(AvsIdentityType), &reqSize, &avsIdentityType);
	if(0 == retAvs) {
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_GetList");
		*busing = 0;
		return getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_DEVICE_NOT_FOUND);
	}

	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "avs serial number:%s",avsIdentityType.SerialNumber);

	_avsHandle = AVS_Activate(&avsIdentityType);

	if(INVALID_AVS_HANDLE_VALUE == _avsHandle) {
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_Activate");
		*busing = 0;
		return getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_HANDLE);
	}

	// 默认参数
	avsMsg.integrationTime = 10.0;
	avsMsg.averagesCount = 1;
	avsMsg.smoothPixCount = 1;

	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "default inte:%0.2f cnt:%d smooth:%d", avsMsg.integrationTime,avsMsg.averagesCount,avsMsg.smoothPixCount);

	//设置16位ADC
	retAvs = AVS_UseHighResAdc(_avsHandle, true);
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Use high res adc");

	if(ERR_OPERATION_NOT_SUPPORTED == retAvs) {
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_UseHighResAdc");
		*busing = 0;
		return getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_OPERATION_NOT_SUPPORTED);
	}

*busing = 0;
	return spectrumPrepareMeasure();
}




/**
 * \brief 设置氙灯闪烁次数
 * \param pData 待处理数据
 * \param pLen 待处理数据长度
 * \param comOutputBuffer 需要串口发送的数据
 */
template <typename T1,template <typename T> class C>
void FuncSetXFlash(unsigned char * pData,  unsigned short * pLen, C<T1>* comOutputBuffer, C<T1>* otherBuffer, int* status, int* busing, long* deviceID)
{
	//printf("--------------------FuncSetXFlash\n");
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "%s", "--------------------FuncSetXFlash");

	MessageStruct msg;
	memset(&msg,0,sizeof(msg));

	// pData的前两个字节是命令、组号
	ushortUnion tmpCmd;
	memcpy(tmpCmd.buf,pData,2);

	uIntUnion tmpPortId;
	memcpy(tmpPortId.buf,pData + 2,4);

	//printf("AVS_SetDigOutnum:%d\n",tmpPortId.value);

	if(ERR_SUCCESS != AVS_SetDigOut(_avsHandle, (unsigned char)tmpPortId.value, 0)) {
		//printf("Failed to AVS_SetDigOutnum[%d]:low\n",tmpPortId.value);
		msg.head.flags.resp = 1;
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_SetDigOutnum[%d]:low",tmpPortId.value);
		*busing = 0;
		return;
	}

	if(ERR_SUCCESS != AVS_SetDigOut(_avsHandle, (unsigned char)tmpPortId.value, 1)) {
		//printf("Failed to AVS_SetDigOutnum[%d]:high\n",tmpPortId.value);
		msg.head.flags.resp = 1;
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_SetDigOutnum[%d]:high",tmpPortId.value);
		*busing = 0;
		return;
	}

	if(ERR_SUCCESS != AVS_SetDigOut(_avsHandle, (unsigned char)tmpPortId.value, 0)) {
		//printf("Failed to AVS_SetDigOutnum[%d]:low\n",tmpPortId.value);
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_SetDigOutnum[%d]:low",tmpPortId.value);
		msg.head.flags.resp = 1;
		*busing = 0;
		return;
	}


	//printf("Sccueed to AVS_SetDigOutnum\n");
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Sccueed to AVS_SetDigOutnum");

	msg.head.flags.rw = 0;
	msg.head.flags.resp = 0;
	msg.head.seq = 0x01;
	msg.head.addr = ((*deviceID)&0xFFFF);
	msg.head.length = 0x02;
	msg.payload.cmd = CMD_SPEC_SET_X_FLASH;

	if(!comOutputBuffer->full()) {
		comOutputBuffer->enqueue(msg);
		//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "FuncSetXFlash: input ring buffer data");
	}
	
	*busing = 0;
}


/**
 * \brief 获取光谱仪测量参数
 * \param pData 待处理数据
 * \param pLen 待处理数据长度
 * \param comOutputBuffer 需要串口发送的数据
 */
template <typename T1,template <typename T> class C>
void FuncGetLambda(unsigned char * pData,  unsigned short * pLen, C<T1>* comOutputBuffer, C<T1>* otherBuffer, int* status, int* busing, long* deviceID)
{
	//printf("--------------------FuncGetLambda\n");
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncGetLambda");

	double lambdaBuffer[4096] = {0x00};

	int retAvs = AVS_GetLambda(_avsHandle, lambdaBuffer);

	int errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_NO);

	if(ERR_DEVICE_NOT_FOUND == retAvs) {
		errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_DEVICE_NOT_FOUND);
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_GetLambda:%d msg:%s",errCode, getErrorMsg(errCode));
	} else if(ERR_MSG_CODE_INVALID_DEVICE_ID == retAvs) {
		errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_DEVICE_ID);
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_GetLambda:%d msg:%s",errCode, getErrorMsg(errCode));
	}
	
	*status = errCode;
	
	if(errCode != getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_NO)) {

		MessageStruct msg;
		memset(&msg,0,sizeof(msg));
		msg.head.flags.rw = 0;
		msg.head.flags.resp = 1;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
		msg.head.seq = 0x01;
		msg.head.addr = ((*deviceID)&0xFFFF);
		msg.head.length = 0x06;
		msg.payload.cmd = CMD_SPEC_GET_LAMBDA;

		memcpy(msg.payload.data + 2,&errCode, 4);


		if(!comOutputBuffer->full()) {
			comOutputBuffer->enqueue(msg);
		}

		// 重新启动光谱仪
		//spectrumProcessReinit(comOutputBuffer, otherBuffer, status);
		
		*busing = 0;
		
		return;
	}


	std::list<double> listWaveLength;

	MessageStruct msg;
	memset(&msg,0,sizeof(msg));
	msg.head.flags.rw = 1;///* Write = 0; Read = 1 */
	msg.head.flags.reply = 1;
	msg.head.flags.resp = 0;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
	msg.head.seq = 0x01;
	msg.head.addr = ((*deviceID)&0xFFFF);
	msg.head.length = 0x02 + 2048*8;
	msg.payload.cmd = CMD_SPEC_GET_LAMBDA;


	for(int i = 0; i < 2048; i++) {
		doubleUnion tmpValue;
		memset(tmpValue.buf,0,8);
		tmpValue.value = lambdaBuffer[i];
		//printf("lambdaBuffer:%0.2f %02x-%02x-%02x-%02x-%02x-%02x-%02x-%02x\n",lambdaBuffer[i],tmpValue.buf[0]&0xFF,tmpValue.buf[1]&0xFF,tmpValue.buf[2]&0xFF,tmpValue.buf[3]&0xFF,tmpValue.buf[4]&0xFF,tmpValue.buf[5]&0xFF,tmpValue.buf[6]&0xFF,tmpValue.buf[7]&0xFF);
		memcpy(msg.payload.data + 2 + i*8,tmpValue.buf,8);
		//printf("wave[%d]:%0.2f\n",i, lambdaBuffer[i]);
		//memcpy(msg.payload.data + 2 + i*8,(char*)&lambdaBuffer[i],8);
	}

	//printf("double size:%d\n",sizeof(double));

	if(!comOutputBuffer->full()) {
		comOutputBuffer->enqueue(msg);
		//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "FuncGetLambda: input ring buffer data");
	}
	
	*busing = 0;
}


/**
 * \brief 获取光谱仪样品数据
 * \param pData 待处理数据
 * \param pLen 待处理数据长度
 * \param comOutputBuffer 需要串口发送的数据
 */
template <typename T1,template <typename T> class C>
void FuncGetSample(unsigned char * pData,  unsigned short * pLen, C<T1>* comOutputBuffer, C<T1>* otherBuffer, int* status, int* busing, long* deviceID)
{
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncGetSample");

	//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "AVS_Measure Handle:%ld", _avsHandle);
	
	int retAvs = AVS_Measure(_avsHandle, NULL, 1);

	//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "AVS_Measure: ret:%d", retAvs);

	int errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_NO);
	
	if(ERR_SUCCESS == retAvs){
#ifndef NJ_ZP_Tech
		MessageStruct msg;
		memset(&msg,0,sizeof(msg));
		msg.head.flags.rw = 0;
		msg.head.flags.resp = 0;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
		msg.head.seq = 0x01;
		msg.head.addr = ((*deviceID)&0xFFFF);
		msg.head.length = 0x02;
		msg.payload.cmd = CMD_OTHER_LHL_PULSE;
		
		if(!otherBuffer->full()) {
			//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "请求外部触发!!!");
			otherBuffer->enqueue(msg);
		}else{
			//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "请求外部触发失败!!!");
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(((int)avsMsg.integrationTime)*avsMsg.averagesCount + 350)); //单位是毫秒
#endif		
		// MessageStruct msg;
		// memset(&msg,0,sizeof(msg));
		// msg.head.flags.rw = 0;
		// msg.head.flags.resp = 0;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
		// msg.head.seq = 0x01;
		// msg.head.addr = ((*deviceID)&0xFFFF);
		// msg.head.length = 0x02;
		// msg.payload.cmd = CMD_SPEC_ACK;
		
		// //std::this_thread::sleep_for(std::chrono::milliseconds(((int)avsMsg.integrationTime)*avsMsg.averagesCount + 350)); //单位是毫秒

		// if(!comOutputBuffer->full()) {
			// EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "请求外部触发!!!");
			// comOutputBuffer->enqueue(msg);
		// }else{
			// EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "请求外部触发失败!!!");
		// }
	} else if(ERR_OPERATION_PENDING == retAvs) {
		errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_OPERATION_PENDING);
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_Measure:%d msg:%s",errCode, getErrorMsg(errCode));
	} else if(ERR_DEVICE_NOT_FOUND == retAvs) {
		errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_DEVICE_NOT_FOUND);
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_Measure:%d msg:%s",errCode, getErrorMsg(errCode));
	} else if(ERR_INVALID_DEVICE_ID == retAvs) {
		errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_DEVICE_ID);
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_Measure:%d msg:%s",errCode, getErrorMsg(errCode));
	} else if(ERR_INVALID_PARAMETER == retAvs) {
		errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_PARAMETER);
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_Measure:%d msg:%s",errCode, getErrorMsg(errCode));
	} else if(ERR_INVALID_STATE == retAvs) {
		errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_STATE);
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_Measure:%d msg:%s",errCode, getErrorMsg(errCode));
	} else if(ERR_COMMUNICATION == retAvs) {
		errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_COMMUNICATION);
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_Measure:%d msg:%s",errCode, getErrorMsg(errCode));
	}else{
		errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_UNKNOW);
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_Measure:%d msg:%s",errCode, getErrorMsg(errCode));
	}

	*status = errCode;

	if(errCode != getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_NO)) {
		*status = errCode;

		MessageStruct msg;
		memset(&msg,0,sizeof(msg));
		msg.head.flags.rw = 0;
		msg.head.flags.resp = 1;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
		msg.head.seq = 0x01;
		msg.head.addr = ((*deviceID)&0xFFFF);
		msg.head.length = 0x06;
		msg.payload.cmd = CMD_SPEC_GET_SAMPLE;

		memcpy(msg.payload.data + 2,&errCode, 4);


		if(!comOutputBuffer->full()) {
			comOutputBuffer->enqueue(msg);
		}
		
		*busing = 0;
		return;
	}

	//std::this_thread::sleep_for(std::chrono::milliseconds(((int)avsMsg.integrationTime)*avsMsg.averagesCount + 350 + DELAY_OFFSET)); //单位是毫秒

	errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_NO);

	int waitCnt = 0;
	int maxWaitCnt = 1000;
	int waitPeriod = 10;
	do {
		
		retAvs = AVS_PollScan(_avsHandle);
		
		if(1 == retAvs) { // 获取到有效数据
			break;
		} else {
			if(0 == retAvs) { // 未获取到有效数据，继续等待
				std::this_thread::sleep_for(std::chrono::milliseconds(waitPeriod)); //单位是毫秒
				retAvs = AVS_PollScan(_avsHandle);
				if(1 == retAvs){
					EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Success to AVS_PollScan");
					break;
				}else{
					//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Fail to AVS_PollScan");
					waitCnt++;
					if(waitCnt%(maxWaitCnt/waitPeriod) == 0){
						EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "AVS_PollScan: Wait to extern pulse:%ds",waitPeriod + 1 - waitCnt/(maxWaitCnt/waitPeriod));
					}
				}
				
			} 
			else if(ERR_DEVICE_NOT_FOUND == retAvs) {
				errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_DEVICE_NOT_FOUND);
				*status = errCode;
				EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_PollScan:%d msg:%s",errCode, getErrorMsg(errCode));
				break;
			} else if(ERR_INVALID_DEVICE_ID == retAvs) {
				errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_DEVICE_ID);
				*status = errCode;
				EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_PollScan:%d msg:%s",errCode, getErrorMsg(errCode));
				break;
			}
		}

	} while(retAvs == 0 && waitCnt < maxWaitCnt);

	if(errCode != getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_NO)) {
		// 停止测量
		retAvs = AVS_StopMeasure(_avsHandle);

		if(ERR_SUCCESS == retAvs) {
			EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Success to AVS_StopMeasure");
		} else if(ERR_DEVICE_NOT_FOUND == retAvs) {
			errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_DEVICE_NOT_FOUND);
			*status = errCode;
			EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_StopMeasure:%d msg:%s",errCode, getErrorMsg(errCode));
		} else if(ERR_INVALID_DEVICE_ID == retAvs) {
			errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_DEVICE_ID);
			*status = errCode;
			EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_StopMeasure:%d msg:%s",errCode, getErrorMsg(errCode));
		} else if(ERR_TIMEOUT == retAvs) {
			errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_TIMEOUT);
			*status = errCode;
			EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_StopMeasure:%d msg:%s",errCode, getErrorMsg(errCode));
		} else if(ERR_INVALID_PARAMETER == retAvs) {
			errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_PARAMETER);
			*status = errCode;
			EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_StopMeasure:%d msg:%s",errCode, getErrorMsg(errCode));
		}

		MessageStruct msg;
		memset(&msg,0,sizeof(msg));
		msg.head.flags.rw = 0;
		msg.head.flags.resp = 1;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
		msg.head.seq = 0x01;
		msg.head.addr = ((*deviceID)&0xFFFF);
		msg.head.length = 0x06;
		msg.payload.cmd = CMD_SPEC_GET_SAMPLE;

		memcpy(msg.payload.data + 2,&errCode, 4);


		if(!comOutputBuffer->full()) {
			comOutputBuffer->enqueue(msg);
		}

		*busing = 0;
		
		return;
	}


	if(maxWaitCnt == waitCnt) {
		errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_TIMEOUT);
		*status = errCode;
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_PollScan:timeout");
		//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "AVS_StopMeasure Handle:%ld", _avsHandle);
		// 停止测量
		retAvs = AVS_StopMeasure(_avsHandle);

		if(ERR_SUCCESS == retAvs) {
			EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Success to AVS_StopMeasure");
		} else if(ERR_DEVICE_NOT_FOUND == retAvs) {
			errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_DEVICE_NOT_FOUND);
			*status = errCode;
			EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_StopMeasure:%d msg:%s",errCode, getErrorMsg(errCode));
		} else if(ERR_INVALID_DEVICE_ID == retAvs) {
			errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_DEVICE_ID);
			*status = errCode;
			EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_StopMeasure:%d msg:%s",errCode, getErrorMsg(errCode));
		} else if(ERR_TIMEOUT == retAvs) {
			errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_TIMEOUT);
			*status = errCode;
			EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_StopMeasure:%d msg:%s",errCode, getErrorMsg(errCode));
		} else if(ERR_INVALID_PARAMETER == retAvs) {
			errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_PARAMETER);
			*status = errCode;
			EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_StopMeasure:%d msg:%s",errCode, getErrorMsg(errCode));
		}


		MessageStruct msg;
		memset(&msg,0,sizeof(msg));
		msg.head.flags.rw = 0;
		msg.head.flags.resp = 1;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
		msg.head.seq = 0x01;
		msg.head.addr = ((*deviceID)&0xFFFF);
		msg.head.length = 0x06;
		msg.payload.cmd = CMD_SPEC_GET_SAMPLE;

		memcpy(msg.payload.data + 2,&errCode, 4);

		if(!comOutputBuffer->full()) {
			comOutputBuffer->enqueue(msg);
		}
		
		*busing = 0;
		
		return;
	}


	double scopeData[2048] = {0x00};
	unsigned int pTimeLabel = 0;

	retAvs = AVS_GetScopeData(_avsHandle, &pTimeLabel, scopeData);

	//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "AVS_GetScopeData: ret:%d", retAvs);

	if(ERR_DEVICE_NOT_FOUND == retAvs) {
		errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_DEVICE_NOT_FOUND);
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_GetScopeData:%d msg:%s",errCode, getErrorMsg(errCode));
	} else if(ERR_INVALID_DEVICE_ID == retAvs) {
		errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_DEVICE_ID);
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_GetScopeData:%d msg:%s",errCode, getErrorMsg(errCode));
	} else if(ERR_INVALID_MEAS_DATA == retAvs) {
		errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_DATA);
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_GetScopeData:%d msg:%s",errCode, getErrorMsg(errCode));
	}
	
	*status = errCode;

	if(errCode != getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_NO)) {

		MessageStruct msg;
		memset(&msg,0,sizeof(msg));
		msg.head.flags.rw = 0;
		msg.head.flags.resp = 1;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
		msg.head.seq = 0x01;
		msg.head.addr = ((*deviceID)&0xFFFF);
		msg.head.length = 0x06;
		msg.payload.cmd = CMD_SPEC_GET_SAMPLE;

		memcpy(msg.payload.data + 2,&errCode, 4);


		if(!comOutputBuffer->full()) {
			comOutputBuffer->enqueue(msg);
		}
	
		*busing = 0;
		
		return;
	}

	MessageStruct msg;
	memset(&msg,0,sizeof(msg));
	msg.head.flags.rw = 1;///* Write = 0; Read = 1 */
	msg.head.flags.reply = 1;
	msg.head.flags.resp = 0;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
	msg.head.seq = 0x01;
	msg.head.addr = ((*deviceID)&0xFFFF);
	msg.head.length = 0x02 + 2048*8;
	msg.payload.cmd = CMD_SPEC_GET_SAMPLE;

	for(int i = 0; i < 2048; i++) {
		doubleUnion tmpValue;
		tmpValue.value = scopeData[i];
		memcpy(msg.payload.data + 2 + i*8,tmpValue.buf,8);
	}

	if(!comOutputBuffer->full()) {
		comOutputBuffer->enqueue(msg);
		//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "FuncGetSample: Success get sample");
	}

	*status = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_NO);
	
	*busing = 0;
	
	//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "FuncGetSample: busing status:%d", *busing);
}


/**
 * \brief 设置光谱仪参数
 * \param pData 待处理数据
 * \param pLen 待处理数据长度
 * \param comOutputBuffer 需要串口发送的数据
 */
template <typename T1,template <typename T> class C>
void FuncSetCfg(unsigned char * pData,  unsigned short * pLen, C<T1>* comOutputBuffer, C<T1>* otherBuffer, int* status, int* busing, long* deviceID)
{
	//printf("--------------------FuncSetCfg\n");
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncSetCfg");
	// pData的前两个字节是命令、组号
	ushortUnion tmpCmd;
	memcpy(tmpCmd.buf,pData,2);

	uIntUnion tmpInte;
	memcpy(tmpInte.buf,pData + 2,4);

	uIntUnion tmpCnt;
	memcpy(tmpCnt.buf,pData + 2 + 4,4);

	uIntUnion tmpSmooth;
	memcpy(tmpSmooth.buf,pData + 2 + 4 + 4,4);

	//printf("cmd:%02x inte:%d cnt:%d smooth:%d\n", tmpCmd.value,tmpInte.value,tmpCnt.value,tmpSmooth.value);
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "cmd:%02x inte:%d inte:%0.2f cnt:%d smooth:%d", tmpCmd.value,tmpInte.value,((float)(tmpInte.value/100*1.0) + (float)(tmpInte.value%100*0.01)),tmpCnt.value,tmpSmooth.value);
	avsMsg.integrationTime = (float)(tmpInte.value/100*1.0) + (float)(tmpInte.value%100*0.01);
	avsMsg.averagesCount = tmpCnt.value;
	avsMsg.smoothPixCount = tmpSmooth.value;


	MessageStruct msg;
	memset(&msg,0,sizeof(msg));
	msg.head.flags.rw = 1;///* Write = 0; Read = 1 */
	msg.head.flags.reply = 1;

	msg.head.seq = 0x01;
	msg.head.addr = ((*deviceID)&0xFFFF);
	msg.head.length = 0x06;
	msg.payload.cmd = CMD_SPEC_SET_CFG;

	int ret = spectrumPrepareMeasure();

	if(getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_NO) == ret) {
		msg.head.flags.resp = 0;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
	} else {
		msg.head.flags.resp = 1;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
	}

	memcpy(msg.payload.data + 2,&ret,4);

	if(!comOutputBuffer->full()) {
		//printf("FuncSetCfg: input data\n");
		comOutputBuffer->enqueue(msg);
		//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "FuncSetCfg: input ring buffer data");
	}

	*status = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_NO);
	
	*busing = 0;
}

/**
 * \brief 设置光谱仪参数
 * \param pData 待处理数据
 * \param pLen 待处理数据长度
 * \param comOutputBuffer 需要串口发送的数据
 */
template <typename T1,template <typename T> class C>
void FuncReboot(unsigned char * pData,  unsigned short * pLen, C<T1>* comOutputBuffer, C<T1>* otherBuffer, int* status, int* busing, long* deviceID)
{
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncReboot");

	MessageStruct msg;
	memset(&msg,0,sizeof(msg));
	msg.head.flags.rw = 0;///* Write = 0; Read = 1 */
	msg.head.flags.reply = 1;
	msg.head.flags.resp = 0;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
	msg.head.seq = 0x01;
	msg.head.addr = ((*deviceID)&0xFFFF);
	msg.head.length = 0x02;
	msg.payload.cmd = CMD_SYS_SPEC_REBOOT;

	if(!comOutputBuffer->full()) {
		comOutputBuffer->enqueue(msg);
	}

	// 重新启动光谱仪
	spectrumProcessReinit(comOutputBuffer, otherBuffer, status, busing, deviceID);
	//spectrumProcessInit(otherBuffer, status);
	
	*busing = 0;
}


/**
 * \brief 初始化处理函数句柄
 */
template <typename T1,template <typename T> class C>
int spectrumProcessInit(C<T1>* comOutputBuffer, int* status, long* deviceID)
{
	// 功能句柄
	SpectrumFuncHandlers[0] = { CMD_SPEC_GET_LAMBDA, 		FuncGetLambda };
	SpectrumFuncHandlers[1] = { CMD_SPEC_GET_SAMPLE, 		FuncGetSample };
	SpectrumFuncHandlers[2] = { CMD_SPEC_SET_CFG, 			FuncSetCfg };
	SpectrumFuncHandlers[3] = { CMD_SPEC_SET_X_FLASH, 		FuncSetXFlash };
	SpectrumFuncHandlers[4] = { CMD_SPEC_REBOOT, 			FuncReboot };

	// 需要等待设备连接
	// Avantes初始化
	int retAvs = AVS_Init(0);
	//printf("avs spectram connect ret:%d",retAvs);
	//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "avs spectram connect ret:%d",retAvs);

	do {
		if(retAvs > 0) {
			break;
		} else {
			//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "wait avs spectram connect ret:%d",retAvs);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //单位是毫秒

			int errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_DEVICE_NOT_FOUND);
			*status = errCode;
			EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_Init:%d msg:%s",errCode, getErrorMsg(errCode));
		}

	} while((retAvs = AVS_Init(0)) <= 0);


	AvsIdentityType avsIdentityType;
	unsigned int reqSize;

	retAvs =  AVS_GetList(sizeof(AvsIdentityType), &reqSize, &avsIdentityType);
	if(0 == retAvs) {
		int errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_DEVICE_NOT_FOUND);
		*status = errCode;
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_GetList:%d msg:%s",errCode, getErrorMsg(errCode));
		return errCode;
	}

	//printf("avs serial number:%s\n",avsIdentityType.SerialNumber);
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "avs serial number:%s",avsIdentityType.SerialNumber);

	_avsHandle = AVS_Activate(&avsIdentityType);

	if(INVALID_AVS_HANDLE_VALUE == _avsHandle) {
		int errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_INVALID_HANDLE);
		*status = errCode;
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_Activate:%d msg:%s",errCode, getErrorMsg(errCode));
		return errCode;
	}

	// 默认参数
	avsMsg.integrationTime = 10.0;
	avsMsg.averagesCount = 1;
	avsMsg.smoothPixCount = 1;

	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "default inte:%0.2f cnt:%d smooth:%d", avsMsg.integrationTime,avsMsg.averagesCount,avsMsg.smoothPixCount);

	//设置16位ADC
	retAvs = AVS_UseHighResAdc(_avsHandle, true);
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Use high res adc");

	if(ERR_OPERATION_NOT_SUPPORTED == retAvs) {
		int errCode = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_OPERATION_NOT_SUPPORTED);
		*status = errCode;
		EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "Failed to AVS_UseHighResAdc:%d msg:%s",errCode, getErrorMsg(errCode));
		return errCode;
	}

	*status = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_NO);

	return spectrumPrepareMeasure();
}



template <typename T1,template <typename T> class C>
void spectrumProcess(C<T1>* spectrumBuffer, C<T1>* comOutputBuffer, C<T1>* otherBuffer, int* status, int* busing, long* deviceID)
{
	int initRet = getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_NO);
	int reInitCnt = 0;
	do {
		initRet = spectrumProcessInit(comOutputBuffer, status, deviceID);

		if(initRet == getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_NO)) {
			break;
		} else {
			EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "spectrumProcessInit: init fail!!!");
			std::this_thread::sleep_for(std::chrono::milliseconds(3000)); //单位是毫秒
			*status = initRet;
		}
	} while(initRet != getErrorCode(ERR_MSG_CLASS_DEVICE, ERR_MSG_GROUP_SPEC, ERR_MSG_CODE_NO));


	for(;;) {
		if(!spectrumBuffer->empty()) {
			const std::shared_ptr<T1> out = spectrumBuffer->dequeue();
			MessageStruct tmp = (*out);
			int index = 0;
			for (index = 0; index < FUNC_HANDLERS_MAX; index++) {
				//printf("spectrumProcess:code:%04x cmd:%04x\n", SpectrumFuncHandlers[index].functionCode, tmp.payload.cmd);
				if (SpectrumFuncHandlers[index].functionCode == tmp.payload.cmd) {
					//printf("spectrumProcess:code:%04x cmd:%04x\n", SpectrumFuncHandlers[index].functionCode, tmp.payload.cmd);
					//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "spectrumProcess:code:%04x cmd:%04x", SpectrumFuncHandlers[index].functionCode, tmp.payload.cmd);
					*busing = 1;
					SpectrumFuncHandlers[index].pHandler(tmp.payload.data, &tmp.head.length, comOutputBuffer, otherBuffer, status, busing, deviceID);
					break;
				}
			}

			if(index == FUNC_HANDLERS_MAX) {
				EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "spectrumProcess: no cmd!!!");

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

				*busing = 0;
			}
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1)); //单位是毫秒
	}
}









