#ifndef SYNCHRONISE_ENGINE_HEAD_FILE
#define SYNCHRONISE_ENGINE_HEAD_FILE

#pragma once

#include "GameServiceHead.h"
#include <afxtempl.h>

//////////////////////////////////////////////////////////////////////////
//数据定义

//请求结构
struct tagSyncRequest
{
	DWORD							dwSyncID;							//同步ID
	DWORD							dwRequestCount;						//请求次数

	//构造函数
	tagSyncRequest()
{
		dwSyncID = 0xffffffff;
		dwRequestCount = 0;
	}
};

//////////////////////////////////////////////////////////////////////////
//宏定义

typedef CWHArray<IDataBaseEngine *> CDataBaseEngineArray;			//数据引擎 
typedef CWHArray<IDataBaseEngineSink *> CDataBaseEngineSinkArray;	//调度勾子
typedef CWHArray<tagSyncRequest> CSyncReqeust;					//请求数组
typedef CMap<DWORD,DWORD,INT_PTR,INT_PTR> CMapSyncID;					//ID映射

//////////////////////////////////////////////////////////////////////////

//数据库引擎类
class CDataBaseSyncEngine: public IDataBaseSyncEngine
{
	//友元定义
	friend class CServiceUnits;

	//变量定义
protected:
	bool							m_bService;							//服务状态
	CMapSyncID 						m_SyncIDMap;						//同步ID

	//组件变量
protected:
	CDataBaseEngineArray			m_DataBaseEngineArry;				//引擎数组
	CDataBaseEngineSinkArray		m_DataBaseEngineSinkArry;			//调度勾子
	CCriticalSection				m_AsyncCritical;					//锁定对象
	CSyncReqeust					m_SyncRequestArry;					//请求结构

	//组件变量
protected:
	ITimerEngine*					m_pITimerEngine;					//时间引擎

	//缓冲变量
protected:
	CBlockDataQueue					m_DataQueue;						//数据队列
	
	//函数定义
public:
	//构造函数
	CDataBaseSyncEngine();
	//析构函数
	virtual ~CDataBaseSyncEngine();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//服务接口
public:
	//启动服务
	virtual bool StartService();
	//停止服务
	virtual bool ConcludeService();

	//配置接口
public:
	//设置时间引擎
	virtual VOID SetTimerEngine(ITimerEngine *pITimerEngine) { m_pITimerEngine = pITimerEngine; }
	//配置模块
	virtual bool InitDataBaseSyncEngine(IDataBaseEngine * pIDataBaseEngine[], IDataBaseEngineSink *pIDataBaseEngineSink[], WORD wSinkCount);	

	//查询接口
public:
	//查询索引
	virtual INT_PTR GetDataBaseEngineSinkIndex(IDataBaseEngineSink *pIDataBaseEngineSink);

	//功能函数
public:
	//设置时间
	virtual bool SetTimer(INT_PTR nSyncIndex, DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);
	//删除时间
	virtual bool KillTimer(INT_PTR nSyncIndex, DWORD dwTimerID);

	//控制事件
public:
	//时间事件
	virtual bool PostDataBaseTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//控制事件
	virtual bool PostDataBaseControl(DWORD dwSyncID, WORD wControlID, VOID * pData, WORD wDataSize);
	//请求事件
	virtual bool PostDataBaseRequest(DWORD dwSyncID, WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);

	//同步事件
public:
	//请求完成
	virtual bool OnPostRequestComplete(INT_PTR nSyncIndex);

	//辅助函数
protected:
	//查找异步引擎
	INT_PTR SearchValidEngine(DWORD dwSyncID);
	//缓冲判断
	bool AmortizeEstimate();
	//缓冲请求
	bool AmortizeSyncData(WORD wIdentifier, DWORD dwSyncID, VOID * pData, WORD wDataSize);
	//执行缓冲
	VOID PerformAmortisation();
};

//////////////////////////////////////////////////////////////////////////
//辅助类

DECLARE_MODULE_HELPER(DataBaseSyncEngine,GAME_SERVICE_DLL_NAME,"CreateDataBaseSyncEngine")

//////////////////////////////////////////////////////////////////////////

#endif
