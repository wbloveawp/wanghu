#ifndef GLOBAL_SERVICE_MANAGER_HEAD_FILE
#define GLOBAL_SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "ClubServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//类说明
class CGlobalBattleService;

//类型定义
typedef CWHArray<CTCPSocketServiceHelper*> CSocketServiceArray;

//索引定义
typedef CWHStockData<WORD, CGlobalBattleService> CStockBattleService;

//////////////////////////////////////////////////////////////////////////

//约战服务
class CGlobalBattleService : public CGlobalServiceItem
{
	//变量定义
public:
	ITCPSocketService *				m_pITCPSocketService;				//网络接口	

	//函数定义
protected:
	//构造函数
	CGlobalBattleService();
	//析构函数
	virtual ~CGlobalBattleService();

	//重置数据
public:
	//重置数据
	virtual VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

//全局信息
class CGlobalServiceManager
{
	//配置函数
protected:
	IUnknownEx*						m_pIAttemperEngine;					//调度接口

	//索引变量
protected:	
	CStockBattleService				m_StockBattleService;				//约战服务

	//网络组件
protected:
	CSocketServiceArray				m_SocketServiceActive;				//服务存储
	CSocketServiceArray				m_SocketServiceBuffer;				//服务缓冲

	//函数定义
public:
	//构造函数
	CGlobalServiceManager();
	//析构函数
	virtual ~CGlobalServiceManager();

	//管理函数
public:
	//重置数据
	VOID ResetData();
	//服务数目
	WORD GetBattleServiceCount() { return (WORD)m_StockBattleService.GetItemCount();}

	//配置函数
public:
	//回调接口
	VOID SetIAttemperEngine(IUnknownEx* pIAttemperEngine) { m_pIAttemperEngine = pIAttemperEngine; }

	//约战服务
public:
	//删除服务
	bool DeleteBattleService(WORD wServiceID);
	//寻找服务
	CGlobalBattleService * SearchBattleService(WORD wServiceID);
	//枚举服务
	CGlobalBattleService * EnumBattleService(CGlobalBattleService* pGlobalBattleService);
	//激活服务
	CGlobalBattleService * ActiveBattleService(DWORD dwServiceID, tagServiceItem & ServiceItem);	

	//辅助函数
private:
	//激活服务
	ITCPSocketService* ActiveSocketService(DWORD dwServiceID);
	//移除服务
	bool RemoveSocketService(ITCPSocketService* pITCPSocketService);
};

//////////////////////////////////////////////////////////////////////////

#endif