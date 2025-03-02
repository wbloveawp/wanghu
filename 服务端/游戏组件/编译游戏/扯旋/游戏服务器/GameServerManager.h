#ifndef GAME_SERVER_MANAGER_HEAD_FILE
#define GAME_SERVER_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////

//游戏服务器管理类

class CGameServiceManager : public IGameServiceManager, public IGameServiceCustomRule
{
	//控件变量
protected:
	HINSTANCE							m_hDllInstance;						//DLL
	CDlgCustomRule 					m_pDlgCustomRule;				//自定规则

	//变量定义
protected:
	tagGameServiceAttrib				m_GameServiceAttrib;			//服务属性

	//函数定义
public:
	//构造函数
	CGameServiceManager(void);
	//析构函数
	virtual ~CGameServiceManager(void);

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CGameServiceManager))?true:false; }
	//接口查询
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//服务接口
public:
	//初始化
	virtual bool StartService() { return true; }
	//停止服务
	virtual bool ConcludeService() { return true; }

	//数据接口
public:
	//创建数据
	virtual VOID * CreateDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer) { return NULL; }
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, VOID * pData, WORD wDataSize, DWORD dwContextID, DWORD dwTokenID) { return true; }

	//配置接口
public:
	//获取配置
	virtual bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
	//默认配置
	virtual bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
	//创建窗口
	virtual HWND CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize);

	//创建接口
public:
	//创建桌子
	virtual VOID * CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer) ;
	//创建机器
	virtual VOID * CreateRobotUserItemSink(REFGUID Guid, DWORD dwQueryVer) ;
	//获取属性
	virtual bool __cdecl GetGameServiceAttrib(tagGameServiceAttrib & GameServiceAttrib);
	//修改参数
	virtual bool __cdecl RectifyOptionParameter(tagGameServiceOption & GameServiceOption);
};

//////////////////////////////////////////////////////////////////////////

#endif
