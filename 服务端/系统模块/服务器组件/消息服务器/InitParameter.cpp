#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////

//时间定义
#define TIME_CONNECT				3									//重连时间
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

	//配置信息	
	ZeroMemory(m_szPaltformDomain, sizeof(m_szPaltformDomain));
	ZeroMemory(&m_CenterAddress,sizeof(m_CenterAddress));
	ZeroMemory(&m_AccountsDBParameter,sizeof(m_AccountsDBParameter));
	ZeroMemory(&m_PlatformDBParameter, sizeof(m_PlatformDBParameter));
	ZeroMemory(m_szLoggerPath, sizeof(m_szLoggerPath));

	return;
}

//加载配置
VOID CInitParameter::LoadInitParameter()
{
	//重置参数
	InitParameter();

	//服务信息
	CServiceParameter ServiceParameter;

	//中心端口
	m_wCenterPort=ServiceParameter.GetCenterPort();

	//验证信息
	ServiceParameter.GetCenterAddress(m_CenterAddress);
	ServiceParameter.GetAccountsDBInfo(m_AccountsDBParameter);
	ServiceParameter.GetPlatformDBInfo(m_PlatformDBParameter);

	//工作目录
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//文件目录
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\ServerInfo.ini"), szDirectory);

	//系统配置
	GetPrivateProfileString(TEXT("Information"), TEXT("PlatformDomain"), TEXT(""), m_szPaltformDomain, CountArray(m_szPaltformDomain), szFileName);

	//读取路径
	GetPrivateProfileString(TEXT("LoggerConfig"), TEXT("LocalLogPath"), TEXT("D:\\LocalLogs"), m_szLoggerPath, CountArray(m_szLoggerPath), szFileName);

	return;
}

//////////////////////////////////////////////////////////////////////////
