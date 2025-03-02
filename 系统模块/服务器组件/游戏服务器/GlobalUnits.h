#ifndef GLOBAL_UNITES_HEAD_FILE
#define GLOBAL_UNITES_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//全局单元
class CGlobalUnits
{
	//服务参数
public:
	bool							m_bAutoStart;						//自动启动
	bool							m_bFailureClose;					//错误关闭
	bool							m_bValidateSuccess;					//验证标志

	//组件变量
public:	
	CDatabaseListManager			m_DBLisMgr;							//数据库列表
	CModuleDBParameter				m_ModuleDBParameter;				//模块参数

	//函数定义
public:
	//构造函数
	CGlobalUnits();
	//析构函数
	virtual ~CGlobalUnits();

	//参数函数
public:
	//服务参数
	bool LoadServiceParameter();
};

//////////////////////////////////////////////////////////////////////////

//对象说明
extern CGlobalUnits					g_GlobalUnits;						//全局单元

//////////////////////////////////////////////////////////////////////////

#endif