/*! @file EasyCommon.h
 *  @brief \b 公共功能接口
 *  @author wangdh
 *  @version 1.0
 *  @copyright Copyright (c) 2018-2021 上海电气自动化研究所有限公司
 *  @date 2021-07-02
 *  @details \b 描述
 *  -# 公共功能接口
 *  -# (1)
 *	@details \b 修改日志
 *  <table>
 *  <tr><th>Date        <th>Version  <th>Author    <th>Description </tr>
 *  <tr><td>2021-07-02  <td>1.0      <td>wangdh    <td>创建初始版本 </tr>
 *  </table>
 */
#ifndef EASY_COMMON_H
#define EASY_COMMON_H
#include <string>
#include <vector>

 /* 只保留_FILENAME_的名称 */
#ifdef WIN32
#define FILE_NAME(x) strrchr(x, '\\') ? strrchr(x, '\\') + 1 : x
#else
#define FILE_NAME(x) strrchr(x, '/') ? strrchr(x, '/') + 1 : x
#endif

/* 获取两个值中的最小值 */
#define EASY_MAX(x , y)  (((x) > (y)) ? (x) : (y))

/* 获取两个值中的最大值 */
#define EASY_MIN(x , y)  (((x) < (y)) ? (x) : (y))

/* 获取数组长度(x不能为指针) */
#define EASY_ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

/* 字节转换为32值 */
#define EASY_MAKE_U32(a,b,c,d) (((u32_t)((a) & 0xff) << 24) | \
                                ((u32_t)((b) & 0xff) << 16) | \
                                ((u32_t)((c) & 0xff) << 8)  | \
                                (u32_t)((d) & 0xff))

/* 指定的某一位数置1 */
#define SET_BIT(x,y)  x |= (1<<y)

/* 指定的某一位数置0 */
#define CLR_BIT(x,y)  x &= ~(1<<y)

/* 指定的某一位数取反 */
#define REVERSE_BIT(x,y)  x ^= (1<<y)

/* 获取的某一位的值 */
#define GET_BIT(x,y)   ((x) >> (y)&1)

/**
 * \brief 判断文件是否存在
 * \param name 文件名称 
 * \return true:存在 false:不存在
 */
bool isFileExists(std::string& name);


/**
 * \brief 获取系统时间
 * \return 系统时间[yyyy-YY-dd hh:mm:ss.zzz]
 */
char* getSystemTime();

/**
 * \brief 获取系统时间
 * \return 系统时间[yyyy-YY-dd-hh-mm-ss-zzz]
 * \note 文件以时间命名，不允许存在: 等特殊字符
 */
char* getSystemTimeFile();

/**
 * \brief 分割字符串
 * \param str 待分割的字符串
 * \param delim 分割标识符
 * \param trim_empty 是否去除待分割字符串首尾空格
 * \return 分割后的字符串
 */
std::vector<std::string> easySplit(const std::string &str, const std::string &delim, const bool trim_empty = false);

/**
 * \brief 无符号字节数组转十六进制字符串
 * \param bytes 无符号字节数组
 * \param length 无符号字节数组长度
 * \return 16进制字符串
 * \note 转换后的字符间添加空格
 */
std::string easyBytesToHexString(const unsigned char* bytes, const int length);


/**
 * \brief 十六进制字符串转无符号字节数组
 * \param hex 十六进制字符串
 * \param bytes 无符号字节数组
 * \return 转换的无符号字节的个数
 * \note 字符串分割参考EasySplit函数说明
 */
int easyHexToBytes(const std::string& hex, unsigned char* bytes);

/**
 * \brief 将字符串编码成十六进制数字,适用于所有字符（包括中文）
 * \param str 待转换的字符串
 * \return 转换后的十六进制字符串
 * \note 转换后的字符间添加空格
 */
std::string easyEncodeHexString(const std::string& str);

/**
 * \brief 十六进制字符串转字符串
 * \param hexStr 16进制字符串
 * \return 转换后的字符串
 * \note 先将小写转成大写,字符串分割参考EasySplit函数说明
 */
std::string easyHexStringToString(const std::string& hexStr);

#endif //!EASY_COMMON_H
