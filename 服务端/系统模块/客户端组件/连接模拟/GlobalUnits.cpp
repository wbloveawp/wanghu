#include "StdAfx.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalUnits::CGlobalUnits()
{
	//设置变量
	m_cbTestMode = 0;
	m_cbConnectMode = 0;
	m_bConnectStatus=true;
	

	//配置信息
	m_wKindID=0;
	m_wServerType=0;
	m_wServerPort=0;
	m_wServerLevel=0;
	m_szServerAddr[0]=0;
	m_dwProcessVersion=0;

	return;
}

//析构函数
CGlobalUnits::~CGlobalUnits()
{
}

//////////////////////////////////////////////////////////////////////////

//全局单元
CGlobalUnits g_GlobalUnits;

//////////////////////////////////////////////////////////////////////////
