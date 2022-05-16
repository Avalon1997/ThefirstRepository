/*! \file dispatchprocess.h
 *  \brief \b 总调度数据处理
 *  \author wangdh
 *  \version 1.0
 *  \copyright Copyright (c) 2018-2021 深圳谱研互联科技有限公司
 *  \date 2021-05-29
 *  \details \b 描述
 *  -# 串口（232）驱动
 *  -# 操作Avantes光谱仪操作
 *	\details \b 修改日志
 *  <table>
 *  <tr><th>Date        <th>Version  <th>Author    <th>Description </tr>
 *  <tr><td>2021-05-29  <td>1.0      <td>wangdh    <td>创建初始版本 </tr>
 *  </table>
 */
#ifndef _DISPATCH_PROCESS_H
#define _DISPATCH_PROCESS_H



typedef enum _CMD_TYPE {
	CMD_SYS,			///< 系统
	CMD_SPEC,			///< 光谱仪
	CMD_SERVO,			///< 舵机
	CMD_TEMP,			///< 温度
	CMD_PING,			///< 通讯
	CMD_OTHER			///< 其他
} CMD_TYPE;


typedef enum _SYS_TYPE {
	SYS_SUCCESS = 0x01,		///< 正常
	SYS_ERROR,				///< 错误
	SYS_STATUS,				///< 状态
	SYS_CMD,				///< 接收命令错误
	SYS_SYNC_TIME,			///< 时间同步
	SYS_REBOOT,				///< 系统重启
	SYS_SPEC_REBOOT,		///< 光谱仪重启
	SYS_SELF_REBOOT			///< 程序重启
} SYS_TYPE;


typedef enum _SPEC_TYPE {
	SPEC_LAMBDA = 0x01,	///< 波长
	SPEC_SAMPLE,		///< 样品光谱值
	SPEC_REF,			///< 参考光谱值
	SPEC_DARK,			///< 暗背景光谱值
	SPEC_CFG,			///< 设置参数
	SPEC_FLASH,			///< 设置氙灯闪烁
	SPEC_STATUS,		///< 光谱仪状态
	SPEC_REBOOT,		///< 光谱仪重启
	SPEC_ACK			///< 响应
} SPEC_TYPE;


typedef enum _SERVO_TYPE {
	SERVO_MODE = 0x01,		///< （舵机）切换器模式
	SERVO_POSITION = 0x02	///< （舵机）切换器模式
} SERVO_TYPE;


typedef enum _TEMP_TYPE {
	TEMP_TEMP = 0x01	///< 水质模块内部温度传感器的温度值
} TEMP_TYPE;


typedef enum _PING_TYPE {
	PING_PING = 0x01	///< ping通信
} PING_TYPE;


typedef enum _OTHER_TYPE {
	OTHER_L_H_L_PULSE = 0x01	///< 0-1-0脉冲
} OTHER_TYPE;

// 命令(cmd) - 组(key)
#define CMD_KEY(key, cmd) (((key) << 8 ) | cmd )
#define CMD_GET_KEY(cmd_key) ((cmd_key >> 8) & 0xFF)
#define CMD_GET_CMD(cmd_key) ((cmd_key >> 0) & 0xFF)



// 光谱仪
#define CMD_SPEC_GET_LAMBDA			( CMD_KEY(CMD_SPEC, SPEC_LAMBDA) ) // 波长
#define CMD_SPEC_GET_SAMPLE			( CMD_KEY(CMD_SPEC, SPEC_SAMPLE) ) // 样品光谱值
#define CMD_SPEC_GET_REF			( CMD_KEY(CMD_SPEC, SPEC_REF) ) // 参考光谱值
#define CMD_SPEC_GET_DARK			( CMD_KEY(CMD_SPEC, SPEC_DARK) ) // 暗背景光谱值
#define CMD_SPEC_SET_CFG			( CMD_KEY(CMD_SPEC, SPEC_CFG) ) // 设置参数
#define CMD_SPEC_SET_X_FLASH		( CMD_KEY(CMD_SPEC, SPEC_FLASH) ) // 设置氙灯闪烁
#define CMD_SPEC_GET_STATUS			( CMD_KEY(CMD_SPEC, SPEC_STATUS) )// 光谱仪状态
#define CMD_SPEC_REBOOT				( CMD_KEY(CMD_SPEC, SPEC_REBOOT) )// 光谱仪重启
 #define CMD_SPEC_ACK			( CMD_KEY(CMD_SPEC, SPEC_ACK) )// 光谱仪重启
  
  
// 舵机
#define CMD_SERVO_SET_SWITCH_MODE	( CMD_KEY(CMD_SERVO, SERVO_MODE) ) // 设置（舵机）切换器模式
#define CMD_SERVO_SET_SWITCH_POSITION	( CMD_KEY(CMD_SERVO, SERVO_POSITION) ) // 设置（舵机）切换器模式的角度（用于调试）

// 温度
#define CMD_TEMP_GET_TEMP			( CMD_KEY(CMD_TEMP, TEMP_TEMP) ) // 读取温度值

// 通信握手
#define CMD_PING_PING_PING			( CMD_KEY(CMD_PING, PING_PING) ) // ping通信

// 其他
#define CMD_OTHER_LHL_PULSE			( CMD_KEY(CMD_OTHER, OTHER_L_H_L_PULSE) ) // 0-1-0脉冲


#define CMD_SYS_SUCCESS				( CMD_KEY(CMD_SYS, SYS_SUCCESS) ) // 正常
#define CMD_SYS_ERROR				( CMD_KEY(CMD_SYS, SYS_ERROR) )	 // 错误
#define CMD_SYS_STATUS				( CMD_KEY(CMD_SYS, SYS_STATUS) ) // 状态
#define CMD_SYS_CMD					( CMD_KEY(CMD_SYS, SYS_CMD) ) // 命令错误
#define CMD_SYS_SYNC_TIME			( CMD_KEY(CMD_SYS, SYS_SYNC_TIME) ) // 时间同步
#define CMD_SYS_REBOOT				( CMD_KEY(CMD_SYS, SYS_REBOOT) ) // 系统重启
#define CMD_SYS_SPEC_REBOOT			( CMD_KEY(CMD_SYS, SYS_SPEC_REBOOT) ) // 光谱仪重启
#define CMD_SYS_SELF_REBOOT			( CMD_KEY(CMD_SYS, SYS_SELF_REBOOT) ) // 程序重启


/**
 * \brief 处理熟数据
 * \param dispatchBuffer 总调度数据
 * \param spectrumBuffer 光谱仪模块需要处理的数据
 * \param servoBuffer 舵机模块需要处理的数据
 * \param temperatureBuffer 温度模块需要处理的数据
 * \param pingBuffer 通讯握手模块需要处理的数据
 * \param otherBuffer 其他需要处理的数据
 * \param comOutputBuffer 返回串口的数据
 * \param spectrumStatus 光谱仪状态
 * \param servoStatus 舵机状态
 * \param temperatureStatus 温度状态
 * \param spectrumBusing 光谱仪忙状态
 * \param deviceID 设备地址
 */
template <typename T1,template <typename T> class C>
void dispatchProcess(C<T1>* dispatchBuffer, C<T1>* spectrumBuffer, C<T1>* servoBuffer, C<T1>* temperatureBuffer, C<T1>* pingBuffer, C<T1>* otherBuffer, C<T1>* comOutputBuffer, C<T1>* systemBuffer, int* spectrumStatus, int* servoStatus, int* temperatureStatus, int* spectrumBusing, long* deviceID);




#endif//_DISPATCH_PROCESS_H