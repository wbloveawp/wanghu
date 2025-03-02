#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////

//时间定义
#define TIME_STORE				1									//存储时间		
#define TIME_SERVICE			1									//服务时间		
#define TIME_MONITOR			1									//监控时间		
#define TIME_CONNECT			1									//重连时间

//////////////////////////////////////////////////////////////////////////

//构造函数
CInitParameter::CInitParameter()
{ 
	InitParameter();
}

//析构函数
CInitParameter::~CInitParameter()
{
}

//初始化
VOID CInitParameter::InitParameter()
{
	//系统配置	
	m_wCenterPort=PORT_CENTER;
	m_wConnectTime=TIME_CONNECT;
	m_wConnectStore=TIME_STORE;
	m_wConnectService=TIME_SERVICE;
	m_wConnectMonitor=TIME_MONITOR;

	//配置信息
	ZeroMemory(m_szLoggerPath,sizeof(m_szLoggerPath));

	//连接信息
	ZeroMemory(&m_CenterAddress, sizeof(m_CenterAddress));
	ZeroMemory(&m_ServiceAddress,sizeof(m_ServiceAddress));	
	ZeroMemory(&m_PlatformDBParameter,sizeof(m_PlatformDBParameter));
	ZeroMemory(&m_AccountsDBParameter,sizeof(m_AccountsDBParameter));
	

	return;
}

//加载配置
VOID CInitParameter::LoadInitParameter()
{
	//重置参数
	InitParameter();

	//变量定义
	CServiceParameter ServiceParameter;

	//中心端口
	m_wCenterPort=ServiceParameter.GetCenterPort();

	//配置信息
	ServiceParameter.GetCenterAddress(m_CenterAddress);
	ServiceParameter.GetPlatformDBInfo(m_PlatformDBParameter);
	ServiceParameter.GetAccountsDBInfo(m_AccountsDBParameter);	

	//连接信息
	ServiceParameter.GetServiceAddress(m_ServiceAddress);	

	//日志配置
	LoadLoggerParameter();

	return;
}

//日志配置
VOID CInitParameter::LoadLoggerParameter()
{
	//工作目录
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//文件目录
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\ServerInfo.ini"), szDirectory);

	//读取路径
	GetPrivateProfileString(TEXT("LoggerConfig"), TEXT("LocalLogPath"), TEXT("D:\\LocalLogs"), m_szLoggerPath, CountArray(m_szLoggerPath), szFileName);
}

//////////////////////////////////////////////////////////////////////////
