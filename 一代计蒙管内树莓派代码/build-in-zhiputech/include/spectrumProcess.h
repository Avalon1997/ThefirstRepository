/*! \file spectrumprocess.h
 *  \brief \b 光谱仪数据处理
 *  \author wangdh
 *  \version 1.0
 *  \copyright Copyright (c) 2018-2021 深圳谱研互联科技有限公司
 *  \date 2021-05-29
 *  \details \b 描述
 *  -# 操作Avantes光谱仪操作
 *	\details \b 修改日志
 *  <table>
 *  <tr><th>Date        <th>Version  <th>Author    <th>Description </tr>
 *  <tr><td>2021-05-29  <td>1.0      <td>wangdh    <td>创建初始版本 </tr>
 *  </table>
 */
#ifndef _SPECTRUM_PROCESS_H
#define _SPECTRUM_PROCESS_H


/**
 * \brief 处理光谱仪数据数据
 * \param spectrumBuffer 光谱仪模块需要处理的数据
 * \param comOutputBuffer 需要串口发送的数据
 * \param otherBuffer 触发指令
 * \param status 模块状态
 * \param busing 忙状态
 * \param deviceID 设备地址
 */
template <typename T1,template <typename T> class C>
void spectrumProcess(C<T1>* spectrumBuffer, C<T1>* comOutputBuffer, C<T1>* otherBuffer, int* status, int* busing, long* deviceID);




#endif//_SPECTRUM_PROCESS_H