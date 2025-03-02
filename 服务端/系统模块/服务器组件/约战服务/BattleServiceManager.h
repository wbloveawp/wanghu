#ifndef BATTLE_SERVICE_MANAGER_HEAD_FILE
#define BATTLE_SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "BattleServiceHead.h"
#include "ExtendDataBaseSink.h"

////////////////////////////////////////////////////////////////////////
//类型定义
typedef CWHArray<ITableFrameItem *> CTableFrameArray;	

////////////////////////////////////////////////////////////////////////

//比赛服务管理类
class BATTLE_SERVICE_CLASS CBattleServiceManager : public IExtendServiceManager
{
	//状态变量
protected:
	bool								m_bIsService;					//服务标识
	tagExtendServiceParameter			m_ExtendServiceParameter;		//服务参数	
	
	//变量定义
protected:	
	CTableFrameArray					m_TableFrameArray;				//桌子数组

	//静态成员
protected:
	static CBattleServiceManager*		m_pBattleServiceManager;		//服务管理
	
	//函数定义
public:
	//构造函数
	CBattleServiceManager(void);
	//析构函数
	virtual ~CBattleServiceManager(void);

	//基础接口
public:
	//释放对象
	virtual VOID  Release() { delete this; }
	//接口查询
	virtual VOID *  QueryInterface(const IID & Guid, DWORD dwQueryVer);
	
	//控制接口
public:
	//启动服务
	virtual bool StartService();
	//停止服务
	virtual bool ConcludeService();	
	
	//管理接口
public:	
	//创建桌子
	virtual VOID* CreateExtendTableFrame();
	//创建子项
	virtual VOID* CreateExtendDataBaseSink(REFGUID Guid, DWORD dwQueryVer);
	//初始化接口
	virtual bool  InitServiceManager(tagExtendServiceParameter& ExtendServiceParameter);

	//系统事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, VOID * pData, WORD wDataSize,DWORD dwContextID, DWORD dwTokenID);

	//网络事件
public:
	//网络事件
	virtual bool OnEventSocketRead(WORD wMainCmdID,WORD wSubCmdID, VOID* pData, WORD wDataSize) ;
	//网络事件
	virtual bool OnEventNetworkRead(WORD wSubCmdID, VOID* pData, WORD wDataSize, IServerUserItem* pIServerUserItem, DWORD dwSocketID, DWORD dwTokenID) ;

	//用户接口
public:
	//用户登录
	virtual bool OnEventUserLogon(IServerUserItem* pIServerUserItem);
	//用户登出
	virtual bool OnEventUserLogout(IServerUserItem* pIServerUserItem);
	//用户积分
	virtual bool OnEventUserItemScore(IServerUserItem* pIServerUserItem);

	//静态函数
public:
	//设置对象
	static CBattleServiceManager * GetInstance() { return m_pBattleServiceManager; }	
};

//////////////////////////////////////////////////////////////////////////
#endif