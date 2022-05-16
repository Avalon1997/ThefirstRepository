#include "temperatureCtrl.h"
#include <wiringPi.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "dispatchProcess.h"
#include "uartdefs.h"
#include "ringbuffer.h"
#include "Easylog.h"

#define TEMP_PATH "/sys/class/thermal/thermal_zone0/temp"



//使用头文件方式，需要声明，否则会报错
template void temperatureProcess<MessageStruct, RingBuffer>(RingBuffer<MessageStruct>*, RingBuffer<MessageStruct>*, long*);

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
} TemperatureFunctionHandler;


/// 功能句柄
TemperatureFunctionHandler TemperatureFuncHandlers[FUNC_HANDLERS_MAX];



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


//复位DS18B20
void DS18B20_Rst(void)
{
	DS18B20_IO_OUT(); 	//SET GPIO7 OUTPUT
	DS18B20_DQ_OUT_LOW; 	//拉低DQ
	delayMicroseconds(750);    	//拉低750us
	DS18B20_DQ_OUT_HIGH; 	//DQ1
	delayMicroseconds(15);     	//15US
}

//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
u8 DS18B20_Check(void)
{
	u8 retry=0;
	DS18B20_IO_IN();	//SET GPIO7 INPUT
	while (DS18B20_DQ_IN&&retry<200) {
		retry++;
		delayMicroseconds(1);
	};
	if(retry>=200)return 1;
	else retry=0;
	while (!DS18B20_DQ_IN&&retry<240) {
		retry++;
		delayMicroseconds(1);
	};
	if(retry>=240)return 1;
	return 0;
}
//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在
u8 DS18B20_Init(void)
{
	wiringPiSetup() ;
	DS18B20_Rst();
	return DS18B20_Check();
}

//从DS18B20读取一个位
//返回值：1/0
u8 DS18B20_Read_Bit(void)
{
	u8 data;
	DS18B20_IO_OUT();	//SET PG11 OUTPUT
	DS18B20_DQ_OUT_LOW;
	delayMicroseconds(2);
	DS18B20_DQ_OUT_HIGH;
	DS18B20_IO_IN();	//SET PG11 INPUT
	delayMicroseconds(12);
	if(DS18B20_DQ_IN)data=1;
	else data=0;
	delayMicroseconds(50);
	return data;
}

//从DS18B20读取一个字节
//返回值：读到的数据
u8 DS18B20_Read_Byte(void)
{
	u8 i,j,dat;
	dat=0;
	for (i=1; i<=8; i++) {
		j=DS18B20_Read_Bit();
		dat=(j<<7)|(dat>>1);
	}
	return dat;
}

//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_Write_Byte(u8 dat)
{
	u8 j;
	u8 testb;
	DS18B20_IO_OUT();	//SET PG11 OUTPUT;
	for (j=1; j<=8; j++) {
		testb=dat&0x01;
		dat=dat>>1;
		if (testb) {
			DS18B20_DQ_OUT_LOW;	// Write 1
			delayMicroseconds(2);
			DS18B20_DQ_OUT_HIGH;
			delayMicroseconds(60);
		} else {
			DS18B20_DQ_OUT_LOW;	// Write 0
			delayMicroseconds(60);
			DS18B20_DQ_OUT_HIGH;
			delayMicroseconds(2);
		}
	}
}


//开始温度转换
void DS18B20_Start(void)
{
	DS18B20_Rst();
	DS18B20_Check();
	DS18B20_Write_Byte(0xcc);	// skip rom
	DS18B20_Write_Byte(0x44);	// convert
}

//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250）
short DS18B20_Get_Temp(void)
{
	u8 temp;
	u8 TL,TH;
	short tem;
	DS18B20_Start ();  			// ds1820 start convert
	DS18B20_Rst();
	DS18B20_Check();
	DS18B20_Write_Byte(0xcc);	// skip rom
	DS18B20_Write_Byte(0xbe);	// convert
	TL=DS18B20_Read_Byte(); 	// LSB
	TH=DS18B20_Read_Byte(); 	// MSB

	if(TH>7) {
		TH=~TH;
		TL=~TL;
		temp=0;					//温度为负
	} else temp=1;				//温度为正
	tem=TH; 					//获得高八位
	tem<<=8;
	tem+=TL;					//获得底八位
	tem=(float)tem*0.625;		//转换
	if(temp)return tem; 		//返回温度值
	else return -tem;
}


/**
 * 获取树莓派温度
 */

short getPiTemp()
{
	int fd;
	double temp = 0;
	char buf[32] = {0};

	// 打开/sys/class/thermal/thermal_zone0/temp
	fd = open(TEMP_PATH, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "failed to open thermal_zone0/temp\n");
		return -1;
	}

	// 读取内容
	if (read(fd, buf, 32) < 0) {
		fprintf(stderr, "failed to read temp\n");
		return -1;
	}

	// 转换为浮点数打印
	temp = atoi(buf) / 1000.0;
	//printf("pi temp: %.2f\n", temp);

	// 关闭文件
	close(fd);

	short tem = (short)(temp*10);

	return tem;

}

/**
 * \brief 设置氙灯闪烁次数
 * \param pData 待处理数据
 * \param pLen 待处理数据长度
 */
template <typename T1,template <typename T> class C>
void FuncGetTemp(unsigned char * pData,  unsigned short * pLen, C<T1>* output_buffer, long* deviceID)
{
	//printf("FuncGetTemp\n");
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "--------------------FuncGetTemp");
	MessageStruct msg;
	memset(&msg,0,sizeof(msg));
	msg.head.flags.rw = 1;///* Write = 0; Read = 1 */
	msg.head.flags.reply = 1;
	msg.head.flags.resp = 0;/* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
	msg.head.seq = 0x01;
	msg.head.addr = ((*deviceID)&0xFFFF);
	msg.head.length = 0x02 + 8 + 8;
	msg.payload.cmd = CMD_TEMP_GET_TEMP;

	doubleUnion tmpValue;
	memset(tmpValue.buf,0,8);

	doubleUnion tmpPiValue;
	memset(tmpPiValue.buf,0,8);

	short tmpTemp = DS18B20_Get_Temp();

	short piTemp = getPiTemp();

	tmpValue.value = (double)((tmpTemp)/10.0);

	tmpPiValue.value = (double)((piTemp)/10.0);

	memcpy(msg.payload.data + 2,tmpValue.buf,8);
	memcpy(msg.payload.data + 2 + 8,tmpPiValue.buf,8);


	//printf("temp:[%d]-[%0.2f] piTemp:[%d]-[%0.2f]\n",tmpTemp,tmpValue.value,piTemp,tmpPiValue.value);
	EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "temp:[%d]-[%0.2f] piTemp:[%d]-[%0.2f]",tmpTemp,tmpValue.value,piTemp,tmpPiValue.value);

	if(!output_buffer->full()) {
		//printf("FuncGetTemp: input data\n");
		output_buffer->enqueue(msg);
		//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "FuncGetTemp: input ring buffer data");
	}

}

/**
 * 初始化
 */
void temperatureProcessInit()
{
	// 功能句柄
	TemperatureFuncHandlers[0] = { CMD_TEMP_GET_TEMP, 	FuncGetTemp };

	// 初始化
	DS18B20_Init();

	DS18B20_Get_Temp();
}


template <typename T1,template <typename T> class C>
void temperatureProcess(C<T1>* temperatureBuffer, C<T1>* comOutputBuffer, long* deviceID)
{
	//printf("init temperature\n");
	temperatureProcessInit();
	//printf("Enter temperatureProcess loop\n");
	for(;;) {
		if(!temperatureBuffer->empty()) {
			const std::shared_ptr<T1> out = temperatureBuffer->dequeue();
			MessageStruct tmp = (*out);
			int i = 0;
			for (i = 0; i < FUNC_HANDLERS_MAX; i++) {
				if (TemperatureFuncHandlers[i].functionCode == tmp.payload.cmd) {
					//printf("temperatureProcess:code:%04x cmd:%04x\n", TemperatureFuncHandlers[i].functionCode, tmp.payload.cmd);
					//EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "temperatureProcess:code:%04x cmd:%04x", TemperatureFuncHandlers[i].functionCode, tmp.payload.cmd);
					TemperatureFuncHandlers[i].pHandler(tmp.payload.data, &tmp.head.length, comOutputBuffer, deviceID);
					break;
				}
				
				if(i == FUNC_HANDLERS_MAX) {
				EasyLog::GetInstance()->WriteLog(EasyLog::LOG_DEBUG, "temperatureProcess: no cmd!!!");
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
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1)); //单位是毫秒
	}
}


