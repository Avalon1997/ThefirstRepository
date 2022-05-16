/*! \file otherprocess.h
 *  \brief \b 光谱仪数据处理
 *  \author wangdh
 *  \version 1.0
 *  \copyright Copyright (c) 2018-2021 深圳谱研互联科技有限公司
 *  \date 2021-05-29
 *  \details \b 描述
 *  -# 其他操作
 *	\details \b 修改日志
 *  <table>
 *  <tr><th>Date        <th>Version  <th>Author    <th>Description </tr>
 *  <tr><td>2021-05-29  <td>1.0      <td>wangdh    <td>创建初始版本 </tr>
 *  </table>
 */
#ifndef _SYSTEM_PROCESS_H
#define _SYSTEM_PROCESS_H


/**
 * \brief 处理系统数据数据
 * \param otherBuffer 其他需要处理的数据
 * \param comOutputBuffer 需要串口发送的数据
 * \param spectrumBuffer 需要光谱仪处理的数据
 * \param spectrumStatus 光谱仪状态
 * \param servoStatus 舵机状态
 * \param temperatureStatus 温度状态
 * \param deviceID 设备地址
 */
template <typename T1,template <typename T> class C>
void systemProcess(C<T1>* systemBuffer, C<T1>* comOutputBuffer, C<T1>* spectrumBuffer, int* spectrumStatus, int* servoStatus, int* temperatureStatus, long* deviceID);




#endif //!_SYSTEM_PROCESS_H