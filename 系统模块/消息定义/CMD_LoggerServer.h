#ifndef CMD_LOGGER_SERVER_HEAD_FILE
#define CMD_LOGGER_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////

//服务命令
#define MDM_LS_LOGGER_SERVICE		10										//服务命令

//////////////////////////////////////////////////////////////////////////////////
//请求命令
#define SUB_LC_WRITE_LOGGER			100										//写入日志

//////////////////////////////////////////////////////////////////////////////////
//结构定义

//日志信息
struct tagTimeLogInfo
{
	//标志变量
	bool							bHasSufix;								//后缀标志
	bool							bHasPrefix;								//前缀标志

	//配置变量
	TCHAR							szAppName[16];							//应用名称
	TCHAR							szServiceName[16];						//服务名称
	TCHAR							szFileName[128];						//文件名称
	TCHAR							szFormat[32];							//时间格式
	TCHAR							szConcatStr[4];							//连接字符
	TCHAR							szSeparator[4];							//分割字符
	TCHAR							szLogType[16];							//日志类型
};

//写入日志
struct CMD_LC_WriteTimeLogger : public tagTimeLogInfo
{

};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif