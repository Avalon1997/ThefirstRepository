#include "EasyError.h"
#include <stdio.h>
#include <string.h>

static ErrorMsgString CLASS_MSG[] = {
	{ ERR_MSG_CLASS_IDLE, "Initialization only, meaningless" },
	{ ERR_MSG_CLASS_SYSTEM, "System" },
	{ ERR_MSG_CLASS_SERVICE, "Service" },
	{ ERR_MSG_CLASS_CLIENT, "Client" },
	{ ERR_MSG_CLASS_3RDPARTY, "3rdParty" },
	{ ERR_MSG_CLASS_MIDDLEWARE, "Middleware" },
	{ ERR_MSG_CLASS_DATABASE, "Database" },
	{ ERR_MSG_CLASS_DEVICE, "Device" }
};

static ErrorMsgString GROUP_MSG[] = {
	{ ERR_MSG_GROUP_IDLE, "Initialization only, meaningless" },
	{ ERR_MSG_GROUP_TCP, "TCP" },
	{ ERR_MSG_GROUP_UDP, "UDP" },
	{ ERR_MSG_GROUP_COM_232, "RS232" },
	{ ERR_MSG_GROUP_COM_485, "RS485" },
	{ ERR_MSG_GROUP_COM_422, "RS422" },
	{ ERR_MSG_GROUP_USB, "USB" },
	{ ERR_MSG_GROUP_HTTP, "Http" },
	{ ERR_MSG_GROUP_SPEC, "Spectrum" }
};

static ErrorMsgString CODE_MSG[] = {
	{ ERR_MSG_CODE_NO, "No error" },
	{ ERR_MSG_CODE_INTR, "Interrupted function" },
	{ ERR_MSG_CODE_AGAIN, "Resource temporarily unavailable" },
	{ ERR_MSG_CODE_BUSY, "Resource busy or locked" },
	{ ERR_MSG_CODE_ADDRINUSE, "Address already in use" },
	{ ERR_MSG_CODE_NETDOWN, "Network is down" },
	{ ERR_MSG_CODE_NOBUFS, "No buffer space available" },
	{ ERR_MSG_CODE_NOTCONN, "Connection timed out" },
	{ ERR_MSG_CODE_TIMEDOUT, "Connection timed out" },
	{ ERR_MSG_CODE_TIMEOUT, "Timed out" },
	{ ERR_MSG_CODE_ACCESS_DENIED, "Access is denied" },
	{ ERR_MSG_CODE_INVALID_HANDLE, "The handle is invalid" },
	{ ERR_MSG_CODE_NOT_ENOUGH_MEMORY, " Not enough storage is available to process this command" },
	{ ERR_MSG_CODE_INVALID_DATA, "The data is invalid" },
	{ ERR_MSG_CODE_CRC, "Data error (cyclic redundancy check)" },
	{ ERR_MSG_CODE_BAD_LENGTH, "The program issued a command but the command length is incorrect" },
	{ ERR_MSG_CODE_INVALID_PARAMETER, "Invalid parameter" },
	{ ERR_MSG_CODE_DEVICE_NOT_FOUND, "Device id not found" },
	{ ERR_MSG_CODE_OPERATION_PENDING, "Operation pending" },
	{ ERR_MSG_CODE_INVALID_DEVICE_ID, "Invalid device address" },
	{ ERR_MSG_CODE_INVALID_PIXEL_RANGE, "Invalid pixel range" },
	{ ERR_MSG_CODE_INVALID_CONFIGURATION, "Invalid configuration" },
	{ ERR_MSG_CODE_INVALID_MEASPARAM_DYNDARK, "Dynamic Dark Correction not supported" },
	{ ERR_MSG_CODE_OPERATION_NOT_SUPPORTED, "The operation is not supported" },
	{ ERR_MSG_CODE_REBOOT, "Reboot" },
	{ ERR_MSG_CODE_INVALID_STATE, "Wrong state" },
	{ ERR_MSG_CODE_COMMUNICATION, "Error in communication" },
	{ ERR_MSG_CODE_UNKNOW, "Unknow message" }
};


char *getErrorMsg(int errorCode)
{
	ErrorMsg *msg = reinterpret_cast<ErrorMsg *>(&errorCode);

	static char tmpUnknowError[] = "Unknow error";

	if (msg->_class == 0 || msg->_group == 0)
	{
		return tmpUnknowError;
	}

	if (msg == nullptr)
	{
		return nullptr;
	}

	const char *tmpClassMsg = nullptr;

	for (int i = 0; i < sizeof(CLASS_MSG) / sizeof(CLASS_MSG[0]); i++)
	{
		if (msg->_class == CLASS_MSG[i].value)
		{
			tmpClassMsg = CLASS_MSG[i].desc;
			break;
		}
	}

	if (tmpClassMsg == nullptr)
	{
		return nullptr;
	}

	const char *tmpGroupMsg = nullptr;

	for (int i = 0; i < sizeof(GROUP_MSG) / sizeof(GROUP_MSG[0]); i++)
	{
		if (msg->_group == GROUP_MSG[i].value)
		{
			tmpGroupMsg = GROUP_MSG[i].desc;
			break;
		}
	}

	if (tmpGroupMsg == nullptr)
	{
		return nullptr;
	}

	const char *tmpCodeMsg = nullptr;

	for (int i = 0; i < sizeof(CODE_MSG) / sizeof(CODE_MSG[0]); i++)
	{
		if (msg->_code == CODE_MSG[i].value)
		{
			tmpCodeMsg = CODE_MSG[i].desc;
			break;
		}
	}

	if (tmpCodeMsg == nullptr)
	{
		tmpCodeMsg = tmpUnknowError;
	}

	static char tmpMsg[128 * 3] = { 0 };
	memset(tmpMsg, 0, 128 * 3);
#ifdef __WIN32
	sprintf_s(tmpMsg, 128 * 3, "%s.%s.%s", tmpClassMsg, tmpGroupMsg, tmpCodeMsg);
#else
	snprintf( tmpMsg, 128 * 3, "%s.%s.%s", tmpClassMsg, tmpGroupMsg, tmpCodeMsg); 
#endif

	return tmpMsg;
}

int getErrorCode(ErrorMsgClass classLevel, ErrorMsgGroup groupLevel, ErrorMsgCode codeLevel)
{
	return ((classLevel << 24) | (groupLevel << 16) | (codeLevel << 8) | 0x00);
}
