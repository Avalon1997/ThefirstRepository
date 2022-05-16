/*! \file pingProcess.h
 *  \brief \b 通讯握手.
 *  \author wangdh
 *  \version 1.0
 *  \copyright Copyright (c) 2018-2021 深圳谱研互联科技有限公司
 *  \date 2021-05-28
 *  \details \b 描述
 *  -# 通讯握手
 *	\details \b 修改日志
 *  <table>
 *  <tr><th>Date        <th>Version  <th>Author    <th>Description </tr>
 *  <tr><td>2021-05-28  <td>1.0      <td>wangdh    <td>创建初始版本 </tr>
 *  </table>
 */

#ifndef _PING_PROCESS_H
#define _PING_PROCESS_H



/**
 * \brief 处理舵机数据数据
 * \param pingBuffer 通讯握手模块需要处理的数据
 * \param comOutputBuffer 需要串口发送的数据
 * \param deviceID 设备地址
 */
template <typename T1,template <typename T> class C>
void pingProcess(C<T1>* pingBuffer, C<T1>* comOutputBuffer, long* deviceID);




#endif // !_PING_PROCESS_H
