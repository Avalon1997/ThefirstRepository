/*! @file EasyConfig.h
 *  @brief \b ini、conf配置文件接口
 *  @author wangdh
 *  @version 1.0
 *  @copyright Copyright (c) 2018-2021 上海电气自动化研究所有限公司
 *  @date 2021-07-02
 *  @details \b 描述
 *  -# 读写配置文件
 *  -# (1)
 *	@details \b 修改日志
 *  <table>
 *  <tr><th>Date        <th>Version  <th>Author    <th>Description </tr>
 *  <tr><td>2021-07-02  <td>1.0      <td>wangdh    <td>创建初始版本 </tr>
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
	 * \brief 设置配置文件所属路径
	 * \param[in] filePath 配置文件所属路径
	 * \return true:初始化配置成功 false:初始化配置文件失败
	 */
	static bool setPath(const std::string& filePath);


	/**
	 * \brief 获取配置文件的值
	 * \param section 模块名称
	 * \param key 关键字
	 * \param value 值
	 * \return true:获取成功 false:获取失败
	 * \note 不对获取到的值进行验证
	 */
	static bool getLongValue(const std::string& section, const std::string& key, long& value);

	/**
	 * \brief 获取配置文件的值
	 * \param section 模块名称
	 * \param key 关键字
	 * \param value 值
	 * \return true:获取成功 false:获取失败
	 * \note 不对获取到的值进行验证
	 */
	static bool getBoolValue(const std::string& section, const std::string& key, bool& value);

	/**
	 * \brief 获取配置文件的值
	 * \param section 模块名称
	 * \param key 关键字
	 * \param value 值
	 * \return true:获取成功 false:获取失败
	 * \note 不对获取到的值进行验证
	 */
	static bool getDoubleValue(const std::string& section, const std::string& key, double& value);

	/**
	 * \brief 获取配置文件的值
	 * \param section 模块名称
	 * \param key 关键字
	 * \param value 值
	 * \return true:获取成功 false:获取失败
	 * \note 不对获取到的值进行验证
	 */
	static bool getStringValue(const std::string& section, const std::string& key, std::string& value);

private:
	static CSimpleIniA m_conf;
};



#endif // EASY_CONFIG_H

