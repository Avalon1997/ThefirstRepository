/*! \file temperatureCtrl.h
 *  \brief \b 18B20温度传感器.
 *  \author wangdh
 *  \version 1.0
 *  \copyright Copyright (c) 2018-2021 深圳谱研互联科技有限公司
 *  \date 2021-05-28
 *  \details \b 描述
 *  -# IIC协议
 *  -# 获取温度值
 *	\details \b 修改日志
 *  <table>
 *  <tr><th>Date        <th>Version  <th>Author    <th>Description </tr>
 *  <tr><td>2021-05-28  <td>1.0      <td>wangdh    <td>创建初始版本 </tr>
 *  </table>
 */

#ifndef _TEMPERATURE_CTRL_H
#define _TEMPERATURE_CTRL_H


#include <wiringPi.h>  
#include <stdio.h>
	
//IO方向设置
#define DS18B20_IO_MODE()  	pullUpDnControl(7, PUD_UP)
#define DS18B20_IO_IN()  	pinMode (7, INPUT)
#define DS18B20_IO_OUT() 	pinMode (7, OUTPUT)
	
//IO操作函数											   
#define	DS18B20_DQ_OUT_HIGH digitalWrite(7, HIGH) 		//数据端口 
#define	DS18B20_DQ_OUT_LOW digitalWrite(7, LOW) 		//数据端口 
#define	DS18B20_DQ_IN  	digitalRead(7)  		       //数据端口	
#define   u8	unsigned char 
#define   u16  	unsigned int
	
u8 DS18B20_Init(void);			//初始化DS18B20
short DS18B20_Get_Temp(void);	//获取温度
void DS18B20_Start(void);		//开始温度转换
void DS18B20_Write_Byte(u8 dat);//写入一个字节
u8 DS18B20_Read_Byte(void);		//读出一个字节
u8 DS18B20_Read_Bit(void);		//读出一个位
u8 DS18B20_Check(void);			//检测是否存在DS18B20
void DS18B20_Rst(void);			//复位DS18B20   


/**
 * \brief 处理舵机数据数据
 * \param temperatureBuffer 温度模块需要处理的数据
 * \param comOutputBuffer 需要串口发送的数据
 * \param deviceID 设备地址
 */
template <typename T1,template <typename T> class C>
void temperatureProcess(C<T1>* temperatureBuffer, C<T1>* comOutputBuffer, long* deviceID);




#endif // !_TEMPERATURE_CTRL_H
