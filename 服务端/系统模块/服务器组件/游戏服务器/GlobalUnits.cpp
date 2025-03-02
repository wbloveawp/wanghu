#include "StdAfx.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalUnits::CGlobalUnits()
{
	//设置变量
	m_bAutoStart=false;
	m_bFailureClose=false;

	return;
}

//析构函数
CGlobalUnits::~CGlobalUnits()
{
}

//加载参数
bool CGlobalUnits::LoadServiceParameter()
{
	//变量定义
	CServiceParameter ServiceParameter;
	tagDataBaseParameter PlatformDBParameter;
	tagDataBaseParameter GameMatchDBParameter;

	//配置信息
	ServiceParameter.GetPlatformDBInfo(PlatformDBParameter);
	ServiceParameter.GetGameMatchDBInfo(GameMatchDBParameter);

	//设置参数
	m_ModuleDBParameter.SetMatchDBParameter(GameMatchDBParameter);
	m_ModuleDBParameter.SetPlatformDBParameter(PlatformDBParameter);	

	//加载数据库列表
	m_DBLisMgr.Load();

	return true;
}

//////////////////////////////////////////////////////////////////////////

//全局对象
CGlobalUnits						g_GlobalUnits;						//全局单元

//////////////////////////////////////////////////////////////////////////
