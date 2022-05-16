/*! \file easyerror.h
 *  \brief \b ���������
 *  \author wangdh
 *  \version 1.0
 *  \copyright Copyright (c) 2018-2021 �Ϻ������Զ����о������޹�˾
 *  \date 2021-07-02
 *  \details \b ����
 *  -# ���������
 *	\details \b �޸���־
 *  <table>
 *  <tr><th>Date        <th>Version  <th>Author    <th>Description </tr>
 *  <tr><td>2021-07-02  <td>1.0      <td>wangdh    <td>������ʼ�汾 </tr>
 *  </table>
 */
#ifndef _EASY_ERROR_H
#define _EASY_ERROR_H
 /*
  * �������ʽ
  * +-------------------------------+-------------------------------------+---------------------------------------+---------------------------------------+--------------+----------------+-------------------+
  * |             ����(A)           |               ģ��(B)               |                ���(C)                |                 ����(D)               |	 ʮ������   |     ʮ����     |	      ˵��		 |
  * +-------------------------------+-------------------------------------+---------------------------------------+---------------------------------------+--------------+----------------+-------------------+
  * | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 | 11 | 12 | 13 | 14 | 15 | 16 | 17 | 18 | 19 | 20 | 21 | 22 | 23 | 24 | 25 | 26 | 27 | 28 | 29 | 30 | 31 |	    xxx     |       xxx	     |	    			 |
  * +-------------------------------+-------------------------------------+-------------------------------------------------------------------------------+--------------+----------------+-------------------+
  * | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |  0 |	0x00000000	|   0000000000   |  �޴���			 |
  * +-------------------------------+-------------------------------------+---------------------------------------+---------------------------------------+--------------+----------------+-------------------+
  * �������ʽ˵��
  * ��ʽ��A-BB-CCC
  * A:���󼶱�8bits��ʾ����Χ0~277
  * 1.ϵͳ������2.���񼶴���3.�ͻ��˴���4.������ҵ��ϵͳ����5.�����м������6.���ݿ����7.����δ֪����
  * B:��Ŀ��ģ��(��)���ƣ�8bits��ʾ����Χ0~277��
  * 1.TCP��2.Com��3.Http
  * C:��������ţ���������
  * D:����
  */

  /** \brief �������ʽ */
typedef struct _errorMsg
{
	unsigned int _reserved : 8;     ///< ����[0-7]
	unsigned int _code : 8;         ///< ���к�[8-15]
	unsigned int _group : 8;        ///< ģ��[16-23]
	unsigned int _class : 8;        ///< ����[24-31]
} ErrorMsg;

/** \brief ���������� */
typedef enum
{
	ERR_MSG_CLASS = 0xFF000000,     ///< ��������
	ERR_MSG_GROUP = 0x00FF0000,     ///< ģ������
	ERR_MSG_CODE  = 0x0000FF00       ///< �������
} ErrorMsgMask;

/** \brief ���� */
typedef enum
{
	ERR_MSG_CLASS_IDLE,             ///< �����壬����ʼ��ʹ��
	ERR_MSG_CLASS_SYSTEM,           ///< ϵͳ
	ERR_MSG_CLASS_SERVICE,          ///< �����
	ERR_MSG_CLASS_CLIENT,           ///< �ͻ���
	ERR_MSG_CLASS_3RDPARTY,         ///< ������
	ERR_MSG_CLASS_MIDDLEWARE,       ///< �м��
	ERR_MSG_CLASS_DATABASE,         ///< ���ݿ�
	ERR_MSG_CLASS_DEVICE          	///< �ն��豸
} ErrorMsgClass;

/** \brief ģ�� */
typedef enum
{
	ERR_MSG_GROUP_IDLE,             ///< �����壬����ʼ��ʹ��
	ERR_MSG_GROUP_TCP,              ///< TCP
	ERR_MSG_GROUP_UDP,              ///< UDP
	ERR_MSG_GROUP_COM,          	///< ����
	ERR_MSG_GROUP_COM_232,          ///< RS232
	ERR_MSG_GROUP_COM_485,          ///< RS485
	ERR_MSG_GROUP_COM_422,          ///< RS422
	ERR_MSG_GROUP_USB,              ///< USB
	ERR_MSG_GROUP_HTTP,             ///< Http
	ERR_MSG_GROUP_SPEC,             ///< ������
} ErrorMsgGroup;


/** \brief ���� */
typedef enum
{
	ERR_MSG_CODE_NO,                				///< �޴���
	ERR_MSG_CODE_INTR,              				///< ϵͳ�����ж�
	ERR_MSG_CODE_AGAIN,            	 				///< ��Դ��ʱ������
	ERR_MSG_CODE_BUSY,              				///< ��Դ��æ
	ERR_MSG_CODE_ADDRINUSE,         				///< �Ѿ���ʹ��ָ���ĵ�ַ
	ERR_MSG_CODE_NETDOWN,           				///< ������ֹͣ
	ERR_MSG_CODE_NOBUFS,            				///< ϵͳ��û���㹻�Ļ������ռ���Դ��������ɵ���
	ERR_MSG_CODE_NOTCONN,           				///< δ����
	ERR_MSG_CODE_TIMEDOUT,          				///< ���ӳ�ʱ
	ERR_MSG_CODE_TIMEOUT,           				///< ��ʱ
	ERR_MSG_CODE_ACCESS_DENIED,     				///< �ܾ�����
	ERR_MSG_CODE_INVALID_HANDLE,    				///< �����Ч
	ERR_MSG_CODE_NOT_ENOUGH_MEMORY, 				///< �洢�ռ䲻�㣬�޷����������
	ERR_MSG_CODE_INVALID_DATA,      				///< ������Ч
	ERR_MSG_CODE_CRC,               				///< ���ݴ��� (ѭ��������)
	ERR_MSG_CODE_BAD_LENGTH,        				///< ���򷢳����������Ȳ���ȷ
	ERR_MSG_CODE_INVALID_PARAMETER, 				///< ��������
	ERR_MSG_CODE_DEVICE_NOT_FOUND,  				///< δ�����豸
	ERR_MSG_CODE_OPERATION_PENDING,   				///< ��������
	ERR_MSG_CODE_INVALID_DEVICE_ID,   				///< ��Ч���豸ID
	ERR_MSG_CODE_INVALID_PIXEL_RANGE,   			///< ��Ч�����ط�Χ
	ERR_MSG_CODE_INVALID_CONFIGURATION,	   			///< ��Ч������
	ERR_MSG_CODE_INVALID_MEASPARAM_DYNDARK,	   		///< ��֧�ֶ�̬��У��
	ERR_MSG_CODE_OPERATION_NOT_SUPPORTED,	   		///< ��֧�ִ˲���
	ERR_MSG_CODE_INVALID_STATE,						///< ״̬����
	ERR_MSG_CODE_REBOOT,							///< ����
	ERR_MSG_CODE_COMMUNICATION,						///< ͨ�Ŵ���
	ERR_MSG_CODE_UNKNOW	   							///< δ֪����
} ErrorMsgCode;




typedef struct _errorMsgString
{
	int value;                      ///< ֵ
	const char *desc;               ///< ����
} ErrorMsgString;

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * \brief ��ȡ����������
	 * \param errorCode ������
	 * \return ����������
	 */
	char *getErrorMsg(int errorCode);


	/**
	 * \brief ��ȡ�����룬���ݹ��򣬽�����ģ�顢������Ϊint���͵Ĵ�����
	 * \param classLevel ����
	 * \param groupLevel ģ��
	 * \param codeLevel ���
	 * \return ������
	 */
	int getErrorCode(ErrorMsgClass classLevel, ErrorMsgGroup groupLevel, ErrorMsgCode codeLevel);


#ifdef __cplusplus
}
#endif


#endif //!_EASY_ERROR_H

