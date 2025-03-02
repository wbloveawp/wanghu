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
	ZeroMemory(&m_szServerName,sizeof(m_szServerName));
	ZeroMemory(&m_CenterAddress,sizeof(m_CenterAddress));	
	ZeroMemory(&m_PlatformDBParameter,sizeof(m_PlatformDBParameter));	

	return;
}

//加载配置
VOID CInitParameter::LoadInitParameter()
{
	//服务信息
	CServiceParameter ServiceParameter;

	//中心地址
	ServiceParameter.GetServerName(m_szServerName);

	//中心地址
	m_wCenterPort = ServiceParameter.GetCenterPort();
	ServiceParameter.GetCenterAddress(m_CenterAddress);

	//数据库信息
	ServiceParameter.GetPlatformDBInfo(m_PlatformDBParameter);
	
	return;
}

//////////////////////////////////////////////////////////////////////////
