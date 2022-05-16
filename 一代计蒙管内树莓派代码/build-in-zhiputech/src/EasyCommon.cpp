#include "EasyCommon.h"
#include <fstream>
#include <sys/timeb.h>
#include <ctime>


bool isFileExists(std::string& name)
{
	const std::ifstream ifs(name.c_str());
	return ifs.good();
}

char* getSystemTime()
{
	static char acTimestamp[256] = { 0 };

	time_t t = time(nullptr);
	struct tm tm;

#if defined(_WIN32)
	localtime_s(&tm, &t);
#else
	tm = *localtime(&t);
#endif

	struct timeb msec;
	ftime(&msec);
#ifdef _WIN32
	sprintf_s(acTimestamp, 256, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
	          tm.tm_year + 1900,
	          tm.tm_mon + 1,
	          tm.tm_mday,
	          tm.tm_hour,
	          tm.tm_min,
	          tm.tm_sec,
	          static_cast<int>(msec.millitm)
	         );
#else
	snprintf(acTimestamp, 256, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
	         tm.tm_year + 1900,
	         tm.tm_mon + 1,
	         tm.tm_mday,
	         tm.tm_hour,
	         tm.tm_min,
	         tm.tm_sec,
	         static_cast<int>(msec.millitm)
	        );

#endif

	return acTimestamp;
}

char* getSystemTimeFile()
{
	static char acTimestamp[256] = { '\0' };

	time_t t = time(nullptr);
	struct tm tm;

#if defined(_WIN32)
	localtime_s(&tm, &t);
#else
	tm = *localtime(&t);
#endif

	struct timeb msec;
	ftime(&msec);

#if defined(_WIN32)
	sprintf_s(acTimestamp, 256, "%04d-%02d-%02d-%02d-%02d-%02d-%03d",
	          tm.tm_year + 1900,
	          tm.tm_mon + 1,
	          tm.tm_mday,
	          tm.tm_hour,
	          tm.tm_min,
	          tm.tm_sec,
	          static_cast<int>(msec.millitm)
	         );
#else
	snprintf(acTimestamp, 256, "%04d-%02d-%02d-%02d-%02d-%02d-%03d",
	         tm.tm_year + 1900,
	         tm.tm_mon + 1,
	         tm.tm_mday,
	         tm.tm_hour,
	         tm.tm_min,
	         tm.tm_sec,
	         static_cast<int>(msec.millitm)
	        );
#endif


	return acTimestamp;
}

std::vector<std::string> easySplit(const std::string &str, const std::string &delim, const bool trim_empty)
{
	size_t lastPos = 0;
	std::vector<std::string> tokens;

	while (true) {
		size_t pos = str.find(delim, lastPos);
		if (pos == std::string::npos) {
			pos = str.size();
		}

		const size_t len = pos - lastPos;
		if (!trim_empty || len != 0) {
			tokens.push_back(str.substr(lastPos, len));
		}

		if (pos == str.size()) {
			break;
		} else {
			lastPos = pos + delim.size();
		}
	}

	return tokens;
}


std::string easyBytesToHexString(const unsigned char* bytes, const int length)
{
	if (bytes == nullptr) {
		return "";
	}
	std::string buff;
	const int len = length;
	for (int i = 0; i < len; i++) {
		const int high = bytes[i] / 16;
		const int low = bytes[i] % 16;
		buff += (high < 10) ? ('0' + high) : ('A' + high - 10);
		buff += (low < 10) ? ('0' + low) : ('A' + low - 10);
		buff += " ";
	}
	buff.pop_back();
	return buff;
}

int easyHexToBytes(const std::string& hex, unsigned char* bytes)
{
	std::vector<std::string> vec = easySplit(hex, " ", true);

	for (size_t i = 0; i < vec.size(); i++) {
#ifdef _WIN32
	sscanf_s(vec[i].c_str(), "%x", &bytes[i]);
#else
	sscanf(vec[i].c_str(), "%x", &bytes[i]);
#endif
	}

	return vec.size();
}

std::string easyEncodeHexString(const std::string& str)
{
	// 根据默认编码获取字节数组
	std::string hexString = "0123456789ABCDEF";
	std::string buff;
	// 将字节数组中每个字节拆解成2位16进制整数
	for (int i = 0; i < str.length(); i++) {
		buff += hexString.at((str[i] & 0xF0) >> 4);
		buff += hexString.at((str[i] & 0x0F) >> 0);
		buff += " ";
	}

	buff.pop_back();
	return buff;
}

std::string easyHexStringToString(const std::string& hexStr)
{
	std::string ret;
	const std::string hexString = "0123456789ABCDEF";

	std::vector<std::string> vec = easySplit(hexStr, " ", true);

	for (size_t i = 0; i < vec.size(); i++) {
		char high = vec[i].at(0);
		char low = vec[i].at(1);
		high = (high >= 'a' && high <= 'z') ? (high -= 32) : high;
		low = (low >= 'a' && low <= 'z') ? (low -= 32) : low;

		ret += (hexString.find(high) << 4 | hexString.find(low));
	}

	return ret;
}