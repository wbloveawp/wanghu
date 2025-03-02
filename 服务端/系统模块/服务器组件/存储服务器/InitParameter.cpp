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
	ZeroMemory(m_szVideoPath,sizeof(m_szVideoPath));
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
	ServiceParameter.GetCenterAddress(m_CenterAddress);

	//中心端口
	m_wCenterPort = ServiceParameter.GetCenterPort();

	//工作目录
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//文件目录
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\StoreServer.ini"), szDirectory);

	//路径配置
	GetPrivateProfileString(TEXT("PathConfig"), TEXT("VideoPath"), TEXT("D:"), m_szVideoPath, CountArray(m_szVideoPath), szFileName);

	return;
}

//////////////////////////////////////////////////////////////////////////
