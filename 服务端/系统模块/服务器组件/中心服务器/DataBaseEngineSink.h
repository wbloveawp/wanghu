#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "redis++.h"
#include "DataBasePacket.h"

using namespace redis;

//////////////////////////////////////////////////////////////////////////

//数据库钩子
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//组件变量
protected:
	CWininetHttp					m_WininetHttp;						//网页请求

	//组件变量
protected:
	CDataBaseAide					m_AccountsDBAide;					//用户数据库
	CDataBaseHelper					m_AccountsDBModule;					//用户数据库

	//组件变量
protected:
	CDataBaseAide					m_PlatformDBAide;					//平台数据库
	CDataBaseHelper					m_PlatformDBModule;					//平台数据库

	//组件变量
protected:
	Redis *							m_pRedisClient;						//缓冲对象
	CInitParameter *				m_pInitParameter;					//配置参数
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//数据事件

	//函数定义
public:
	//构造函数
	CDataBaseEngineSink();
	//析构函数
	virtual ~CDataBaseEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//系统事件
public:
	//启动事件
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnDataBaseEngineStop(IUnknownEx * pIUnknownEx);

	//内核事件
public:
	//时间事件
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//控制事件
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//请求事件
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//系统事件
public:	
	//加载配置
	bool OnRequestLoadConfig(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//解锁玩家
	bool OnRequestUnlockPlayer(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//请求网页
	bool OnRequestRequestWebPage(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//在线信息
	bool OnRequestOnLineCountInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//更新网关
	bool OnRequestUpdateGateList(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//上报房间
	bool OnRequestUpdateServerList(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//更新榜单
	bool OnRequestUpdateRankingList(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);	

};

//////////////////////////////////////////////////////////////////////////

#endif