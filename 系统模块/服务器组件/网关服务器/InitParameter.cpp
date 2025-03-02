#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////

//时间定义
#define TIME_CONNECT				3									//重连时间
#define TIME_REPORT_ONLINE			600									//上报时间

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
	m_wReportTime=TIME_REPORT_ONLINE;

	//站点信息
	m_wStationID=0;
	ZeroMemory(m_szStationName,sizeof(m_szStationName));

	//中心配置
	ZeroMemory(&m_CenterAddress,sizeof(m_CenterAddress));

	return;
}

//加载配置
VOID CInitParameter::LoadInitParameter()
{
	//重置参数
	InitParameter();

	//服务信息
	CServiceParameter ServiceParameter;		
	m_wCenterPort=ServiceParameter.GetCenterPort();
	ServiceParameter.GetCenterAddress(m_CenterAddress);

	return;
}

//////////////////////////////////////////////////////////////////////////
