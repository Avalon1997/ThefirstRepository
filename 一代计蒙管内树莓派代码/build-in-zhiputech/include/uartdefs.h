/*! \file uartDefs.h
 *  \brief \b 串口通信协议
 *  \author wangdh
 *  \version 1.0
 *  \copyright Copyright (c) 2018-2021 深圳谱研互联科技有限公司
 *  \date 2021-05-29
 *  \details \b 描述
 *  -# 串口通讯协议
 *	\details \b 修改日志
 *  <table>
 *  <tr><th>Date        <th>Version  <th>Author    <th>Description </tr>
 *  <tr><td>2021-05-29  <td>1.0      <td>wangdh    <td>创建初始版本 </tr>
 *  </table>
 */
#ifndef _UART_DEFS_H
#define _UART_DEFS_H

#define UART_START_IND_NUM_BYTES 4

#define UART_START_IND_BYTE_0 0x41
#define UART_START_IND_BYTE_1 0x42
#define UART_START_IND_BYTE_2 0x43
#define UART_START_IND_BYTE_3 0x44

#define UART_END_IND_NUM_BYTES 4

#define UART_END_IND_BYTE_0 UART_START_IND_BYTE_3
#define UART_END_IND_BYTE_1 UART_START_IND_BYTE_2
#define UART_END_IND_BYTE_2 UART_START_IND_BYTE_1
#define UART_END_IND_BYTE_3 UART_START_IND_BYTE_0

#define MSG_DATA_MAX_SIZE    2048*10

typedef struct _messageStruct
{
	struct _header
	{
		struct _packetcontrolStruct
		{
			unsigned char reserved : 4; /* Future use */
			unsigned char resp : 2; /* cmd Handler Response; 0 - Success, 1 - Error, 2 - Busy */
			unsigned char reply : 1; /* Host wants a reply from device */
			unsigned char rw : 1; /* Write = 0; Read = 1 */
		} flags;
		unsigned char seq;
		unsigned short addr;
		unsigned short length;
	} head;

	union
	{
		unsigned short cmd;
		unsigned char data[MSG_DATA_MAX_SIZE];
	} payload;
} MessageStruct;

typedef struct _uartMessageStruct
{
	unsigned char startInd[UART_START_IND_NUM_BYTES];
	unsigned int chkSum;
	MessageStruct msg;
	unsigned char endInd[UART_END_IND_NUM_BYTES];
} uartMessageStruct;

/*
 * Max packet size would be limited by size of message struct
 */
#define UART_MAX_CMD_MAX_PKT_SZ 			sizeof(uartMessageStruct)

 /*
  * Error codes
  */
#define UART_INCOMP_START_END_IND_RECD	-1
#define UART_INPUT_PKT_CHECKSUM_ERROR	-2
#define UART_WRITE_FAILED			-3

typedef struct _avsMessageStruct
{
	float integrationTime;
	unsigned int averagesCount;
	unsigned int smoothPixCount;
} avsMessageStruct;


#endif//_UART_DEFS_H