#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////

//时间定义
#define TIME_UPDATE					10									//更新时间
#define TIME_UPLOAD					15									//上传时间
#define TIME_CONNECT				30									//重连时间
#define TIME_COLLECT				60									//统计时间
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
	m_wUpdateTime=TIME_UPDATE;
	m_wUploadTime=TIME_UPLOAD;
	m_wMaxConnect=MAX_CONTENT;
	m_wServicePort=PORT_CENTER;
	m_wConnectTime=TIME_CONNECT;
	m_wCollectTime=TIME_COLLECT;	

	//配置信息
	ZeroMemory(m_szUploadDomain,sizeof(m_szUploadDomain));

	//连接信息
	ZeroMemory(&m_PlatformDBParameter,sizeof(m_PlatformDBParameter));
	ZeroMemory(&m_AccountsDBParameter,sizeof(m_AccountsDBParameter));

	//连接信息
	ZeroMemory(&m_RedisConnOption,sizeof(m_RedisConnOption));

	return;
}

//加载配置
VOID CInitParameter::LoadInitParameter()
{
	//重置参数
	InitParameter();

	//服务信息
	CServiceParameter ServiceParameter;
	m_wServicePort =ServiceParameter.GetCenterPort();

	//配置信息
	ServiceParameter.GetPlatformDBInfo(m_PlatformDBParameter);
	ServiceParameter.GetAccountsDBInfo(m_AccountsDBParameter);

	//缓存配置
	LoadRedisParameter();

	//工作目录
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//文件目录
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\ServerInfo.ini"), szDirectory);

	//系统配置
	GetPrivateProfileString(TEXT("Information"), TEXT("UploadDomain"), TEXT(""), m_szUploadDomain, CountArray(m_szUploadDomain), szFileName);

	return;
}


//登录配置
VOID CInitParameter::LoadRedisParameter()
{
	//工作目录
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//文件目录
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\RedisParameter.ini"), szDirectory);

	//缓存配置
	m_RedisConnOption.wDB = GetPrivateProfileInt(TEXT("Master_Write"), TEXT("DB"), RKEY_DDI_MASTER, szFileName);
	m_RedisConnOption.wPort = GetPrivateProfileInt(TEXT("Master_Write"), TEXT("Port"), 0, szFileName);
	GetPrivateProfileString(TEXT("Master_Write"), TEXT("Host"), 0, m_RedisConnOption.szHost, CountArray(m_RedisConnOption.szHost), szFileName);
	GetPrivateProfileString(TEXT("Master_Write"), TEXT("User"), 0, m_RedisConnOption.szUser, CountArray(m_RedisConnOption.szUser), szFileName);
	GetPrivateProfileString(TEXT("Master_Write"), TEXT("Password"), 0, m_RedisConnOption.szPassword, CountArray(m_RedisConnOption.szPassword), szFileName);

	return;
}

//////////////////////////////////////////////////////////////////////////
