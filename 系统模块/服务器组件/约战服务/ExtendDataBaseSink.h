#ifndef EXTEND_DATABASE_SINK_HEAD
#define EXTEND_DATABASE_SINK_HEAD
#pragma once

#include "DataBasePacket.h"
#include "BattleServiceManager.h"

///////////////////////////////////////////////////////////////////////////////////////////
//数据库钩子
class CExtendDataBaseSink: public IExtendDataBaseSink
{
	friend class CBattleServiceManager;

	//游戏数据库
protected:
	CDataBaseAide*					 m_pGameDBAide;					//游戏数据库
	IDataBase*						 m_pIGameDBModule;				//游戏数据库

	//账号数据库
protected:
	CDataBaseAide*					 m_pAccountsDBAide;				//账号数据库
	IDataBase*						 m_pIAccountsDBModule;			//账号数据库

	//平台数据库
protected:
	CDataBaseAide*					m_pPlatformDBAide;				//游戏数据库
	IDataBase*						m_pIPlatformDBModule;			//游戏数据库

	//配置变量
protected:
	tagGameServiceAttrib*			m_pGameServiceAttrib;			//桌子属性
	tagGameServiceOption*			m_pGameServiceOption;			//配置参数

	//组件变量
protected:
	ITimerEngine*					m_pITimerEngine;				//定时引擎
	IDataBaseEngine*				m_pIDataBaseEngine;				//数据引擎
	IDataBaseEngineEvent*			m_pIDataBaseEngineEvent;		//数据事件

	//函数定义
public:
	//构造函数
	CExtendDataBaseSink();
	//析构函数
	virtual ~CExtendDataBaseSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//配置接口
public:
	//初始化
	virtual bool InitExtendDataBaseSink(tagExtendDBSinkParameter& ExtendDBSinkParameter);

	//内核事件
public:
	//时间事件
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//控制事件
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID* pData, WORD wDataSize);
	//请求事件
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//约战请求
private:
	//约战锁定
	bool OnRequestBattleLock(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//约战解锁
	bool OnRequestBattleUnlock(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//战绩记录
	bool OnRequestBattleRecord(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//约战奖励
	bool OnRequestBattleReward(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//扣除费用
	bool OnRequestDeductBattleCost(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);	
	//退还费用
	bool OnRequestReturnBattleCost(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//扣除费用
	bool OnRequestDeductBattleCostAA(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);	
};


///////////////////////////////////////////////////////////////////////////////////////////

#endif