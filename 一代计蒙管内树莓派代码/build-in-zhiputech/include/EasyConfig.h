/*! @file EasyConfig.h
 *  @brief \b ini��conf�����ļ��ӿ�
 *  @author wangdh
 *  @version 1.0
 *  @copyright Copyright (c) 2018-2021 �Ϻ������Զ����о������޹�˾
 *  @date 2021-07-02
 *  @details \b ����
 *  -# ��д�����ļ�
 *  -# (1)
 *	@details \b �޸���־
 *  <table>
 *  <tr><th>Date        <th>Version  <th>Author    <th>Description </tr>
 *  <tr><td>2021-07-02  <td>1.0      <td>wangdh    <td>������ʼ�汾 </tr>
 *  </table>
 */

#ifndef EASY_CONFIG_H
#define EASY_CONFIG_H
#include <string>
#include "SimpleIni.h"

class EasyConf
{
public:
	/**
	 * \brief ���������ļ�����·��
	 * \param[in] filePath �����ļ�����·��
	 * \return true:��ʼ�����óɹ� false:��ʼ�������ļ�ʧ��
	 */
	static bool setPath(const std::string& filePath);


	/**
	 * \brief ��ȡ�����ļ���ֵ
	 * \param section ģ������
	 * \param key �ؼ���
	 * \param value ֵ
	 * \return true:��ȡ�ɹ� false:��ȡʧ��
	 * \note ���Ի�ȡ����ֵ������֤
	 */
	static bool getLongValue(const std::string& section, const std::string& key, long& value);

	/**
	 * \brief ��ȡ�����ļ���ֵ
	 * \param section ģ������
	 * \param key �ؼ���
	 * \param value ֵ
	 * \return true:��ȡ�ɹ� false:��ȡʧ��
	 * \note ���Ի�ȡ����ֵ������֤
	 */
	static bool getBoolValue(const std::string& section, const std::string& key, bool& value);

	/**
	 * \brief ��ȡ�����ļ���ֵ
	 * \param section ģ������
	 * \param key �ؼ���
	 * \param value ֵ
	 * \return true:��ȡ�ɹ� false:��ȡʧ��
	 * \note ���Ի�ȡ����ֵ������֤
	 */
	static bool getDoubleValue(const std::string& section, const std::string& key, double& value);

	/**
	 * \brief ��ȡ�����ļ���ֵ
	 * \param section ģ������
	 * \param key �ؼ���
	 * \param value ֵ
	 * \return true:��ȡ�ɹ� false:��ȡʧ��
	 * \note ���Ի�ȡ����ֵ������֤
	 */
	static bool getStringValue(const std::string& section, const std::string& key, std::string& value);

private:
	static CSimpleIniA m_conf;
};



#endif // EASY_CONFIG_H

