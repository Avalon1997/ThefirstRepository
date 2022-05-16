/*! @file EasyCommon.h
 *  @brief \b �������ܽӿ�
 *  @author wangdh
 *  @version 1.0
 *  @copyright Copyright (c) 2018-2021 �Ϻ������Զ����о������޹�˾
 *  @date 2021-07-02
 *  @details \b ����
 *  -# �������ܽӿ�
 *  -# (1)
 *	@details \b �޸���־
 *  <table>
 *  <tr><th>Date        <th>Version  <th>Author    <th>Description </tr>
 *  <tr><td>2021-07-02  <td>1.0      <td>wangdh    <td>������ʼ�汾 </tr>
 *  </table>
 */
#ifndef EASY_COMMON_H
#define EASY_COMMON_H
#include <string>
#include <vector>

 /* ֻ����_FILENAME_������ */
#ifdef WIN32
#define FILE_NAME(x) strrchr(x, '\\') ? strrchr(x, '\\') + 1 : x
#else
#define FILE_NAME(x) strrchr(x, '/') ? strrchr(x, '/') + 1 : x
#endif

/* ��ȡ����ֵ�е���Сֵ */
#define EASY_MAX(x , y)  (((x) > (y)) ? (x) : (y))

/* ��ȡ����ֵ�е����ֵ */
#define EASY_MIN(x , y)  (((x) < (y)) ? (x) : (y))

/* ��ȡ���鳤��(x����Ϊָ��) */
#define EASY_ARRAY_SIZE(x) (sizeof(x)/sizeof((x)[0]))

/* �ֽ�ת��Ϊ32ֵ */
#define EASY_MAKE_U32(a,b,c,d) (((u32_t)((a) & 0xff) << 24) | \
                                ((u32_t)((b) & 0xff) << 16) | \
                                ((u32_t)((c) & 0xff) << 8)  | \
                                (u32_t)((d) & 0xff))

/* ָ����ĳһλ����1 */
#define SET_BIT(x,y)  x |= (1<<y)

/* ָ����ĳһλ����0 */
#define CLR_BIT(x,y)  x &= ~(1<<y)

/* ָ����ĳһλ��ȡ�� */
#define REVERSE_BIT(x,y)  x ^= (1<<y)

/* ��ȡ��ĳһλ��ֵ */
#define GET_BIT(x,y)   ((x) >> (y)&1)

/**
 * \brief �ж��ļ��Ƿ����
 * \param name �ļ����� 
 * \return true:���� false:������
 */
bool isFileExists(std::string& name);


/**
 * \brief ��ȡϵͳʱ��
 * \return ϵͳʱ��[yyyy-YY-dd hh:mm:ss.zzz]
 */
char* getSystemTime();

/**
 * \brief ��ȡϵͳʱ��
 * \return ϵͳʱ��[yyyy-YY-dd-hh-mm-ss-zzz]
 * \note �ļ���ʱ�����������������: �������ַ�
 */
char* getSystemTimeFile();

/**
 * \brief �ָ��ַ���
 * \param str ���ָ���ַ���
 * \param delim �ָ��ʶ��
 * \param trim_empty �Ƿ�ȥ�����ָ��ַ�����β�ո�
 * \return �ָ����ַ���
 */
std::vector<std::string> easySplit(const std::string &str, const std::string &delim, const bool trim_empty = false);

/**
 * \brief �޷����ֽ�����תʮ�������ַ���
 * \param bytes �޷����ֽ�����
 * \param length �޷����ֽ����鳤��
 * \return 16�����ַ���
 * \note ת������ַ�����ӿո�
 */
std::string easyBytesToHexString(const unsigned char* bytes, const int length);


/**
 * \brief ʮ�������ַ���ת�޷����ֽ�����
 * \param hex ʮ�������ַ���
 * \param bytes �޷����ֽ�����
 * \return ת�����޷����ֽڵĸ���
 * \note �ַ����ָ�ο�EasySplit����˵��
 */
int easyHexToBytes(const std::string& hex, unsigned char* bytes);

/**
 * \brief ���ַ��������ʮ����������,�����������ַ����������ģ�
 * \param str ��ת�����ַ���
 * \return ת�����ʮ�������ַ���
 * \note ת������ַ�����ӿո�
 */
std::string easyEncodeHexString(const std::string& str);

/**
 * \brief ʮ�������ַ���ת�ַ���
 * \param hexStr 16�����ַ���
 * \return ת������ַ���
 * \note �Ƚ�Сдת�ɴ�д,�ַ����ָ�ο�EasySplit����˵��
 */
std::string easyHexStringToString(const std::string& hexStr);

#endif //!EASY_COMMON_H
