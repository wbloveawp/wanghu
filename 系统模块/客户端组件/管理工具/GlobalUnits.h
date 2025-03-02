#ifndef GLOBAL_UNITES_HEAD_FILE
#define GLOBAL_UNITES_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "DlgStatus.h"
#include "DispatchManager.h"
#include "ManagerToolModule.h"

//////////////////////////////////////////////////////////////////////////

//模块索引
typedef CMap<WORD,WORD,IUnknownEx *,IUnknownEx * &> CGlobalModuleMap;

//////////////////////////////////////////////////////////////////////////

//全局单元
class CGlobalUnits : public IGlobalUnits
{
	//版本信息
protected:
	DWORD							m_dwPlazaVersion;					//大厅版本
	DWORD							m_dwFrameVersion;					//框架版本

	//控件变量
public:
	CDlgStatus						m_DlgStatus;						//状态窗口

	//全局组件
public:
	CGlobalStation					m_GlobalStation;					//站点信息
	CGlobalUserInfo					m_GlobalUserInfo;					//用户信息
	CParameterGlobal				m_ParameterGlobal;					//全局配置

	//组件变量
public:
	CDispatchManager				m_DispatchManager;					//事件管理

	//组件变量
public:
	CPlatformResourceHelper			m_PlatformResourceModule;			//资源组件

	//函数定义
public:
	//构造函数
	CGlobalUnits();
	//析构函数
	virtual ~CGlobalUnits();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { return; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//版本信息
public:
	//大厅版本
	virtual DWORD __cdecl GetPlazaVersion() { return m_dwPlazaVersion; }
	//框架版本
	virtual DWORD __cdecl GetFrameVersion() { return m_dwFrameVersion; }

	//配置函数
public:
	//创建组件
	bool InitGlobalUnits();
};

//////////////////////////////////////////////////////////////////////////

//对象说明
extern CGlobalUnits					g_GlobalUnits;						//全局单元

//////////////////////////////////////////////////////////////////////////

#endif
