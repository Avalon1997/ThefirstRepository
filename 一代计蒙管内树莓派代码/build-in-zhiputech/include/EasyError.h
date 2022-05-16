/*! \file easyerror.h
 *  \brief \b 错误码管理
 *  \author wangdh
 *  \version 1.0
 *  \copyright Copyright (c) 2018-2021 上海电气自动化研究所有限公司
 *  \date 2021-07-02
 *  \details \b 描述
 *  -# 错误码管理
 *	\details \b 修改日志
 *  <table>
 *  <tr><th>Date        <th>Version  <th>Author    <th>Description </tr>
 *  <tr><td>2021-07-02  <td>1.0      <td>wangdh    <td>创建初始版本 </tr>
 *  </table>
 */
#ifndef _EASY_ERROR_H
#define _EASY_ERROR_H
 /*
  * 错误码格式
  * +-------------------------------+-------------------------------------+---------------------------------------+---------------------------------------+--------------+----------------+-------------------+
  * |             级别(A)           |               模块(B)               |                编号(C)                |                 保留(D)               |	 十六进制   |     十进制     |	      说明		 |
  * +-------------------------------+-------------------------------------+---------------------------------------+---------------------------------------+--------------+----------------+-------------------+
  * | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 |	    xxx     |       xxx	     |	    			 |
  * +-------------------------------+-------------------------------------+-------------------------------------------------------------------------------+--------------+----------------+-------------------+
  * | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |	0x00000000	|   0000000000   |  无错误			 |
  * +-------------------------------+-------------------------------------+---------------------------------------+---------------------------------------+--------------+----------------+-------------------+
  * 错误码格式说明
  * 格式：A-BB-CCC
  * A:错误级别，8bits表示，范围0~277
  * 1.系统级错误，2.服务级错误，3.客户端错误，4.第三方业务系统错误，5.基础中间件错误，6.数据库错误，7.其它未知错误
  * B:项目或模块(组)名称，8bits表示，范围0~277。
  * 1.TCP，2.Com，3.Http
  * C:具体错误编号，自增即可
  * D:保留
  */

  /** \brief 错误码格式 */
typedef struct _errorMsg
{
	unsigned int _reserved : 8;     ///< 保留[0-7]
	unsigned int _code : 8;         ///< 序列号[8-15]
	unsigned int _group : 8;        ///< 模块[16-23]
	unsigned int _class : 8;        ///< 类型[24-31]
} ErrorMsg;

/** \brief 错误码掩码 */
typedef enum
{
	ERR_MSG_CLASS = 0xFF000000,     ///< 级别掩码
	ERR_MSG_GROUP = 0x00FF0000,     ///< 模块掩码
	ERR_MSG_CODE  = 0x0000FF00       ///< 编号掩码
} ErrorMsgMask;

/** \brief 级别 */
typedef enum
{
	ERR_MSG_CLASS_IDLE,             ///< 无意义，仅初始化使用
	ERR_MSG_CLASS_SYSTEM,           ///< 系统
	ERR_MSG_CLASS_SERVICE,          ///< 服务端
	ERR_MSG_CLASS_CLIENT,           ///< 客户端
	ERR_MSG_CLASS_3RDPARTY,         ///< 第三方
	ERR_MSG_CLASS_MIDDLEWARE,       ///< 中间件
	ERR_MSG_CLASS_DATABASE,         ///< 数据库
	ERR_MSG_CLASS_DEVICE          	///< 终端设备
} ErrorMsgClass;

/** \brief 模块 */
typedef enum
{
	ERR_MSG_GROUP_IDLE,             ///< 无意义，仅初始化使用
	ERR_MSG_GROUP_TCP,              ///< TCP
	ERR_MSG_GROUP_UDP,              ///< UDP
	ERR_MSG_GROUP_COM,          	///< 串口
	ERR_MSG_GROUP_COM_232,          ///< RS232
	ERR_MSG_GROUP_COM_485,          ///< RS485
	ERR_MSG_GROUP_COM_422,          ///< RS422
	ERR_MSG_GROUP_USB,              ///< USB
	ERR_MSG_GROUP_HTTP,             ///< Http
	ERR_MSG_GROUP_SPEC,             ///< 光谱仪
} ErrorMsgGroup;


/** \brief 编码 */
typedef enum
{
	ERR_MSG_CODE_NO,                				///< 无错误
	ERR_MSG_CODE_INTR,              				///< 系统调用中断
	ERR_MSG_CODE_AGAIN,            	 				///< 资源临时不可用
	ERR_MSG_CODE_BUSY,              				///< 资源正忙
	ERR_MSG_CODE_ADDRINUSE,         				///< 已经在使用指定的地址
	ERR_MSG_CODE_NETDOWN,           				///< 网络已停止
	ERR_MSG_CODE_NOBUFS,            				///< 系统中没有足够的缓冲区空间资源可用来完成调用
	ERR_MSG_CODE_NOTCONN,           				///< 未连接
	ERR_MSG_CODE_TIMEDOUT,          				///< 连接超时
	ERR_MSG_CODE_TIMEOUT,           				///< 超时
	ERR_MSG_CODE_ACCESS_DENIED,     				///< 拒绝访问
	ERR_MSG_CODE_INVALID_HANDLE,    				///< 句柄无效
	ERR_MSG_CODE_NOT_ENOUGH_MEMORY, 				///< 存储空间不足，无法处理此命令
	ERR_MSG_CODE_INVALID_DATA,      				///< 数据无效
	ERR_MSG_CODE_CRC,               				///< 数据错误 (循环冗余检查)
	ERR_MSG_CODE_BAD_LENGTH,        				///< 程序发出命令，但命令长度不正确
	ERR_MSG_CODE_INVALID_PARAMETER, 				///< 参数错误
	ERR_MSG_CODE_DEVICE_NOT_FOUND,  				///< 未发现设备
	ERR_MSG_CODE_OPERATION_PENDING,   				///< 操作挂起
	ERR_MSG_CODE_INVALID_DEVICE_ID,   				///< 无效的设备ID
	ERR_MSG_CODE_INVALID_PIXEL_RANGE,   			///< 无效的像素范围
	ERR_MSG_CODE_INVALID_CONFIGURATION,	   			///< 无效的配置
	ERR_MSG_CODE_INVALID_MEASPARAM_DYNDARK,	   		///< 不支持动态暗校正
	ERR_MSG_CODE_OPERATION_NOT_SUPPORTED,	   		///< 不支持此操作
	ERR_MSG_CODE_INVALID_STATE,						///< 状态错误
	ERR_MSG_CODE_REBOOT,							///< 重启
	ERR_MSG_CODE_COMMUNICATION,						///< 通信错误
	ERR_MSG_CODE_UNKNOW	   							///< 未知错误
} ErrorMsgCode;




typedef struct _errorMsgString
{
	int value;                      ///< 值
	const char *desc;               ///< 描述
} ErrorMsgString;

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * \brief 获取错误码释义
	 * \param errorCode 错误码
	 * \return 错误码释义
	 */
	char *getErrorMsg(int errorCode);


	/**
	 * \brief 获取错误码，根据规则，将级别、模块、编号组合为int类型的错误码
	 * \param classLevel 级别
	 * \param groupLevel 模块
	 * \param codeLevel 编号
	 * \return 错误码
	 */
	int getErrorCode(ErrorMsgClass classLevel, ErrorMsgGroup groupLevel, ErrorMsgCode codeLevel);


#ifdef __cplusplus
}
#endif


#endif //!_EASY_ERROR_H

