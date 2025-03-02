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
	m_wConnectTime=TIME_CONNECT;
	m_wCenterPort = PORT_CENTER;

	//配置信息	
	ZeroMemory(&m_CenterAddress,sizeof(m_CenterAddress));
	ZeroMemory(&m_AccountsDBParameter, sizeof(m_AccountsDBParameter));
	ZeroMemory(&m_PlatformDBParameter,sizeof(m_PlatformDBParameter));

	return;
}

//加载配置
VOID CInitParameter::LoadInitParameter()
{
	//重置参数
	InitParameter();

	//服务信息
	CServiceParameter ServiceParameter;
	m_wCenterPort = ServiceParameter.GetCenterPort();

	//配置信息
	ServiceParameter.GetCenterAddress(m_CenterAddress);
	ServiceParameter.GetAccountsDBInfo(m_AccountsDBParameter);
	ServiceParameter.GetPlatformDBInfo(m_PlatformDBParameter);

	//加载参数
	LoadRedisParameter();

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

	//用户库配置
	m_UserRConnOption.wDB = GetPrivateProfileInt(TEXT("User_Read"), TEXT("DB"), RKEY_DDI_USER, szFileName);
	m_UserRConnOption.wPort = GetPrivateProfileInt(TEXT("User_Read"), TEXT("Port"), 0, szFileName);
	GetPrivateProfileString(TEXT("User_Read"), TEXT("Host"), 0, m_UserRConnOption.szHost, CountArray(m_UserRConnOption.szHost), szFileName);
	GetPrivateProfileString(TEXT("User_Read"), TEXT("User"), 0, m_UserRConnOption.szUser, CountArray(m_UserRConnOption.szUser), szFileName);
	GetPrivateProfileString(TEXT("User_Read"), TEXT("Password"), 0, m_UserRConnOption.szPassword, CountArray(m_UserRConnOption.szPassword), szFileName);

	//用户库配置
	m_UserWConnOption.wDB = GetPrivateProfileInt(TEXT("User_Write"), TEXT("DB"), RKEY_DDI_USER, szFileName);
	m_UserWConnOption.wPort = GetPrivateProfileInt(TEXT("User_Write"), TEXT("Port"), 0, szFileName);
	GetPrivateProfileString(TEXT("User_Write"), TEXT("Host"), 0, m_UserWConnOption.szHost, CountArray(m_UserWConnOption.szHost), szFileName);
	GetPrivateProfileString(TEXT("User_Write"), TEXT("User"), 0, m_UserWConnOption.szUser, CountArray(m_UserWConnOption.szUser), szFileName);
	GetPrivateProfileString(TEXT("User_Write"), TEXT("Password"), 0, m_UserWConnOption.szPassword, CountArray(m_UserWConnOption.szPassword), szFileName);

	//令牌库配置
	m_FriendConnOption.wDB = GetPrivateProfileInt(TEXT("UFriend_Read"), TEXT("DB"), RKEY_DDI_UFRIEND, szFileName);
	m_FriendConnOption.wPort = GetPrivateProfileInt(TEXT("UFriend_Read"), TEXT("Port"), 0, szFileName);
	GetPrivateProfileString(TEXT("UFriend_Read"), TEXT("Host"), 0, m_FriendConnOption.szHost, CountArray(m_FriendConnOption.szHost), szFileName);
	GetPrivateProfileString(TEXT("UFriend_Read"), TEXT("User"), 0, m_FriendConnOption.szUser, CountArray(m_FriendConnOption.szUser), szFileName);
	GetPrivateProfileString(TEXT("UFriend_Read"), TEXT("Password"), 0, m_FriendConnOption.szPassword, CountArray(m_FriendConnOption.szPassword), szFileName);

	return;
}

//////////////////////////////////////////////////////////////////////////
