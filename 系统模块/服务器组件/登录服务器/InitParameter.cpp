#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////

//时间定义
#define TIME_CONNECT				1									//重连时间
#define TIME_COLLECT				300									//统计时间
#define TIME_LOAD_LIST				300									//加载列表
#define TIME_RELOAD_LIST			600									//加载列表

//客户时间
#define TIME_INTERMIT				0									//中断时间
#define TIME_ONLINE_COUNT			600									//人数时间

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
	//时间定义
	m_wCenterPort=PORT_CENTER;
	m_wConnectTime=TIME_CONNECT;
	m_wCollectTime=TIME_COLLECT;
	m_wLoadListTime=TIME_LOAD_LIST;
	m_wReLoadListTime=TIME_RELOAD_LIST;

	//客户时间
	m_wIntermitTime=TIME_INTERMIT;
	m_wOnLineCountTime=TIME_ONLINE_COUNT;

	//配置信息
	ZeroMemory(m_szServerName,sizeof(m_szServerName));

	//连接信息
	ZeroMemory(&m_UserConnOption,sizeof(m_UserConnOption));
	ZeroMemory(&m_TokenConnOption, sizeof(m_TokenConnOption));
	ZeroMemory(&m_FriendConnOption, sizeof(m_FriendConnOption));	
	ZeroMemory(&m_MasterConnOption, sizeof(m_MasterConnOption));
	

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

	//读取配置
	CServiceParameter ServiceParameter;
	ServiceParameter.GetServerName(m_szServerName);
	ServiceParameter.GetServiceAddress(m_ServiceAddress);

	//中心端口
	m_wCenterPort = ServiceParameter.GetCenterPort();

	//配置信息
	ServiceParameter.GetCenterAddress(m_CenterAddress);
	ServiceParameter.GetAccountsDBInfo(m_AccountsDBParameter);
	ServiceParameter.GetPlatformDBInfo(m_PlatformDBParameter);

	//登录配置
	LoadRedisParameter();

	return;
}

//登录配置
VOID CInitParameter::LoadRedisParameter()
{
	//工作目录
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//文件目录
	TCHAR szFileName[MAX_PATH]=TEXT("");
	_sntprintf_s(szFileName,CountArray(szFileName),TEXT("%s\\RedisParameter.ini"),szDirectory);

	//用户配置
	m_UserConnOption.wDB = GetPrivateProfileInt(TEXT("User_Write"), TEXT("DB"), RKEY_DDI_USER, szFileName);
	m_UserConnOption.wPort = GetPrivateProfileInt(TEXT("User_Write"), TEXT("Port"), 0, szFileName);
	GetPrivateProfileString(TEXT("User_Write"), TEXT("Host"), 0, m_UserConnOption.szHost, CountArray(m_UserConnOption.szHost), szFileName);
	GetPrivateProfileString(TEXT("User_Write"), TEXT("User"), 0, m_UserConnOption.szUser, CountArray(m_UserConnOption.szUser), szFileName);
	GetPrivateProfileString(TEXT("User_Write"), TEXT("Password"), 0, m_UserConnOption.szPassword, CountArray(m_UserConnOption.szPassword), szFileName);

	//令牌配置
	m_TokenConnOption.wDB=GetPrivateProfileInt(TEXT("Token_Write"), TEXT("DB"), RKEY_DDI_TOKEN, szFileName);
	m_TokenConnOption.wPort = GetPrivateProfileInt(TEXT("Token_Write"), TEXT("Port"), 0, szFileName);
	GetPrivateProfileString(TEXT("Token_Write"), TEXT("Host"), 0, m_TokenConnOption.szHost,CountArray(m_TokenConnOption.szHost), szFileName);
	GetPrivateProfileString(TEXT("Token_Write"), TEXT("User"), 0, m_TokenConnOption.szUser, CountArray(m_TokenConnOption.szUser), szFileName);
	GetPrivateProfileString(TEXT("Token_Write"), TEXT("Password"), 0, m_TokenConnOption.szPassword, CountArray(m_TokenConnOption.szPassword), szFileName);	

	//好友配置
	m_FriendConnOption.wDB = GetPrivateProfileInt(TEXT("UFriend_Write"), TEXT("DB"), RKEY_DDI_UFRIEND, szFileName);
	m_FriendConnOption.wPort = GetPrivateProfileInt(TEXT("UFriend_Write"), TEXT("Port"), 0, szFileName);
	GetPrivateProfileString(TEXT("UFriend_Write"), TEXT("Host"), 0, m_FriendConnOption.szHost, CountArray(m_FriendConnOption.szHost), szFileName);
	GetPrivateProfileString(TEXT("UFriend_Write"), TEXT("User"), 0, m_FriendConnOption.szUser, CountArray(m_FriendConnOption.szUser), szFileName);
	GetPrivateProfileString(TEXT("UFriend_Write"), TEXT("Password"), 0, m_FriendConnOption.szPassword, CountArray(m_FriendConnOption.szPassword), szFileName);

	//管理配置
	m_MasterConnOption.wDB = GetPrivateProfileInt(TEXT("Master_Write"), TEXT("DB"), RKEY_DDI_MASTER, szFileName);
	m_MasterConnOption.wPort = GetPrivateProfileInt(TEXT("Master_Write"), TEXT("Port"), 0, szFileName);
	GetPrivateProfileString(TEXT("Master_Write"), TEXT("Host"), 0, m_MasterConnOption.szHost, CountArray(m_MasterConnOption.szHost), szFileName);
	GetPrivateProfileString(TEXT("Master_Write"), TEXT("User"), 0, m_MasterConnOption.szUser, CountArray(m_MasterConnOption.szUser), szFileName);
	GetPrivateProfileString(TEXT("Master_Write"), TEXT("Password"), 0, m_MasterConnOption.szPassword, CountArray(m_MasterConnOption.szPassword), szFileName);
	
	return;
}

//////////////////////////////////////////////////////////////////////////
