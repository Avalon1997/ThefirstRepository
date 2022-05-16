/*! \file serialCtrl.h
 *  \brief \b 串口通信
 *  \author wangdh
 *  \version 1.0
 *  \copyright Copyright (c) 2018-2021 深圳谱研互联科技有限公司
 *  \date 2021-05-28
 *  \details \b 描述
 *  -# 串口（232）驱动
 *	\details \b 修改日志
 *  <table>
 *  <tr><th>Date        <th>Version  <th>Author    <th>Description </tr>
 *  <tr><td>2021-05-28  <td>1.0      <td>wangdh    <td>创建初始版本 </tr>
 *  </table>
 */
#ifndef _SERIAL_CTRL_H
#define _SERIAL_CTRL_H
#include "serial/serial.h"

/**
 * \brief 串口接收数据
 * \param fd 文件描述符
 * \param buffer 共享内存指针
 */
template <typename T1, template <typename T> class T2>
void serialRead(serial::Serial *com, T2<T1>* buffer);

/**
 * \brief 串口发送数据
 * \param fd 文件描述符
 * \param buffer 共享内存指针
 */
template <typename T1, template <typename T> class T2>
void serialWrite(serial::Serial *com, T2<T1>* buffer);

/**
 * \brief 串口接收数据处理（生成熟数据）
 * \param raw_buffer 生数据内存指针
 * \param processed_buffer 熟数据内存指针
 */
template <typename T1, typename T2, template <typename T> class C>
void serialProcess(serial::Serial *com, C<T1>* raw_buffer, C<T2>* comOutputBuffer, C<T2>* processed_buffer, long* deviceID);


#endif//_SERIAL_CTRL_H