#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////

//时间定义
#define TIME_CONNECT				30									//重连时间

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
	m_wMaxConnect=MAX_CONTENT;

	//时间定义
	m_wConnectTime=TIME_CONNECT;

	//配置信息
	m_cbNetworkKind=NETWORK_KIND_DX;
	ZeroMemory(m_szValidate,sizeof(m_szValidate));
	ZeroMemory(m_szAccreditID,sizeof(m_szAccreditID));

	//连接信息
	ZeroMemory(&m_ServiceAddress,sizeof(m_ServiceAddress));
	ZeroMemory(&m_CorrespondAddress,sizeof(m_CorrespondAddress));
	ZeroMemory(&m_PlatformDBParameter,sizeof(m_PlatformDBParameter));

	return;
}

//加载配置
VOID CInitParameter::LoadInitParameter()
{
	//重置参数
	InitParameter();

	//读取配置
	CServiceParameter ServiceParameter;
	ServiceParameter.GetAccreditID(m_szAccreditID);
	ServiceParameter.GetValidateLink(m_szValidate);
	ServiceParameter.GetServiceAddress(m_ServiceAddress);

	//网络类型
	m_cbNetworkKind=ServiceParameter.GetNetworkKind();

	return;
}

//////////////////////////////////////////////////////////////////////////
