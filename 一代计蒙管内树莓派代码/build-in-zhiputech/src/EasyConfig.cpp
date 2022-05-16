#include "EasyConfig.h"

CSimpleIniA EasyConf::m_conf;		//类的静态成员变量需要在类外分配内存空间

bool EasyConf::setPath(const std::string& filePath)
{
	const SI_Error rc = m_conf.LoadFile(filePath.c_str());

	return rc >= 0;
}


bool EasyConf::getLongValue(const std::string& section, const std::string& key, long& value)
{
	const long val = m_conf.GetLongValue(section.c_str(), key.c_str(),-1);

	value = val;
	return true;
}

bool EasyConf::getBoolValue(const std::string& section, const std::string& key, bool& value)
{
	const bool val = m_conf.GetBoolValue(section.c_str(), key.c_str(), false);

	value = val;
	return true;
}

bool EasyConf::getDoubleValue(const std::string& section, const std::string& key, double& value)
{
	const double val = m_conf.GetDoubleValue(section.c_str(), key.c_str(), 0.0);

	value = val;
	return true;
}

bool EasyConf::getStringValue(const std::string& section, const std::string& key, std::string& value)
{
	const char* pv = m_conf.GetValue(section.c_str(), key.c_str(), "");
	value = std::string(pv);
	return true;
}

