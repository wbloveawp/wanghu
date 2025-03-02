#include "StdAfx.h"
#include "Math.h"
#include "DataBaseSyncEngine.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseSyncEngine::CDataBaseSyncEngine()
{
	//变量定义
	m_bService = false;
	m_SyncIDMap.InitHashTable(PRIME_SERVER_USER);

	//组件变量
	m_pITimerEngine = NULL;
}

//析构函数
CDataBaseSyncEngine::~CDataBaseSyncEngine()
{
	//停止服务
	ConcludeService();
}

//接口查询
VOID * CDataBaseSyncEngine::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServiceModule,Guid,dwQueryVer);
	QUERYINTERFACE(IDataBaseSyncEngine,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseSyncEngine,Guid,dwQueryVer);
	return NULL;
}

//启动服务
bool CDataBaseSyncEngine::StartService()
{
	ASSERT(!m_bService);
	m_bService = true;
	return true;
}

//停止服务
bool CDataBaseSyncEngine::ConcludeService()
{
	CThreadLock ThreadLock(m_AsyncCritical);

	//设置变量
	m_bService = false;

	//同步ID
	m_SyncIDMap.RemoveAll();

	m_SyncRequestArry.RemoveAll();

	m_DataBaseEngineArry.RemoveAll();

	m_DataBaseEngineSinkArry.RemoveAll();

	//组件变量
	m_pITimerEngine = NULL;

	//设置数据
	m_DataQueue.ClearQueue(true);

	return true;
}

//配置模块
bool CDataBaseSyncEngine::InitDataBaseSyncEngine(IDataBaseEngine * pIDataBaseEngine[], IDataBaseEngineSink *pIDataBaseEngineSink[], WORD wSinkCount)
{
	ASSERT(!m_bService);
	ASSERT(wSinkCount<=64);
	if( wSinkCount > 64 ) return false;
	ASSERT(m_DataBaseEngineArry.GetCount()==0);
	if( m_DataBaseEngineArry.GetCount() > 0 ) return false;

	//设置数组
	m_DataBaseEngineArry.SetSize(wSinkCount);
	m_DataBaseEngineSinkArry.SetSize(wSinkCount);
	m_SyncRequestArry.SetSize(wSinkCount);

	//设置对象
	for (INT_PTR i=0;i<m_DataBaseEngineArry.GetCount();i++)
{
		m_DataBaseEngineArry[i] = pIDataBaseEngine[i];

		m_DataBaseEngineSinkArry[i] = pIDataBaseEngineSink[i];
	}

	return true;
}

//查询索引
INT_PTR CDataBaseSyncEngine::GetDataBaseEngineSinkIndex(IDataBaseEngineSink *pIDataBaseEngineSink)
{
	for( INT_PTR i = 0; i < m_DataBaseEngineSinkArry.GetCount(); i++ )
{
		if( m_DataBaseEngineSinkArry[i] == pIDataBaseEngineSink )
	{
			return i;
		}
	}

	return -1;
}

//设置时间
bool CDataBaseSyncEngine::SetTimer(INT_PTR nSyncIndex, DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	//效验参数
	ASSERT(dwTimerID<TIME_SYNC_DATABASE_SINK_RANGE);
	if (dwTimerID>=TIME_SYNC_DATABASE_SINK_RANGE) return false;

	//设置时间
	DWORD dwEngineTimerID=IDI_SYNC_DATABASE_SINK_START+(DWORD)nSyncIndex*TIME_SYNC_DATABASE_SINK_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->SetTimer(dwEngineTimerID+dwTimerID,dwElapse,dwRepeat,dwBindParameter);

	return true;
}

//删除时间
bool CDataBaseSyncEngine::KillTimer(INT_PTR nSyncIndex, DWORD dwTimerID)
{
	//效验参数
	ASSERT(dwTimerID<TIME_SYNC_DATABASE_SINK_RANGE);
	if (dwTimerID>=TIME_SYNC_DATABASE_SINK_RANGE) return false;

	//删除时间
	DWORD dwEngineTimerID=IDI_SYNC_DATABASE_SINK_START+(DWORD)nSyncIndex*TIME_SYNC_DATABASE_SINK_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->KillTimer(dwEngineTimerID+dwTimerID);

	return true;
}

//时间事件
bool CDataBaseSyncEngine::PostDataBaseTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//引擎索引
	DWORD dwSinkID=dwTimerID-IDI_SYNC_DATABASE_SINK_START;
	INT_PTR nSyncIndex=(INT_PTR)(dwSinkID/TIME_SYNC_DATABASE_SINK_RANGE);

	//时间效验
	if (nSyncIndex>=m_DataBaseEngineArry.GetCount()) 
{
		ASSERT(FALSE);
		return false;
	}

	//时间通知
	IDataBaseEngine * pIDataBaseEngine=m_DataBaseEngineArry[nSyncIndex];
	return pIDataBaseEngine->PostDataBaseTimer(dwSinkID%TIME_SYNC_DATABASE_SINK_RANGE,dwBindParameter);
}

//控制事件
bool CDataBaseSyncEngine::PostDataBaseControl(DWORD dwSyncID, WORD wControlID, VOID * pData, WORD wDataSize)
{
	ASSERT(m_bService);

	//设置映射
	CThreadLock ThreadLock(m_AsyncCritical);

	//查找同步ID
	INT_PTR nIndex = SearchValidEngine(dwSyncID);

	if( nIndex != -1 )
{
		m_SyncIDMap[dwSyncID] = nIndex;
		
		tagSyncRequest &SyncRequest = m_SyncRequestArry[nIndex];
		SyncRequest.dwRequestCount++;
		SyncRequest.dwSyncID = dwSyncID;
	}
	else
{
		//构造数据
		NTY_ControlEvent ControlEvent = {0};
		ControlEvent.wControlID = wControlID;

		return AmortizeSyncData(EVENT_CONTROL,dwSyncID,&ControlEvent,sizeof(ControlEvent));
	}

	//派发数据
	m_DataBaseEngineArry[nIndex]->PostDataBaseControl(wControlID,pData,wDataSize);

	return true;
}

//请求事件
bool CDataBaseSyncEngine::PostDataBaseRequest(DWORD dwSyncID, WORD wRequestID, DWORD dwContextID, DWORD dwTokenID,VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(m_bService);
	ASSERT((wDataSize+sizeof(NTY_DataBaseEvent)+sizeof(dwSyncID))<=MAX_ASYNC_DATA);
	if ((wDataSize+sizeof(NTY_DataBaseEvent))>MAX_ASYNC_DATA) 
{
		//构造信息
		TCHAR szDescribe[256]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("数据过长，调试失败：CSyncDataBaseEngine::PostDataBaseRequest [ dwSyncID=%ld wRequestID=%d wDataSize=%ld ]"),
			dwSyncID,wRequestID,wDataSize);

		//输出信息
		CTraceService::TraceString(szDescribe,TraceLevel_Exception);

		return false;
	}

	//设置映射
	CThreadLock ThreadLock(m_AsyncCritical);

	//查找同步ID
	INT_PTR nIndex = SearchValidEngine(dwSyncID);

	if( nIndex != -1 )
{
		m_SyncIDMap[dwSyncID] = nIndex;

		tagSyncRequest &SyncRequest = m_SyncRequestArry[nIndex];
		SyncRequest.dwRequestCount++;
		SyncRequest.dwSyncID = dwSyncID;
	}
	else
{
		//变量定义
		BYTE cbBuffer[MAX_ASYNC_DATA];
		NTY_DataBaseEvent * pDataBaseEvent=(NTY_DataBaseEvent *)cbBuffer;

		//构造数据		
		pDataBaseEvent->dwTokenID=dwTokenID;
		pDataBaseEvent->wRequestID=wRequestID;		
		pDataBaseEvent->dwContextID=dwContextID;

		//附加数据
		if (wDataSize>0)
	{
			ASSERT(pData!=NULL);
			CopyMemory(cbBuffer+sizeof(NTY_DataBaseEvent),pData,wDataSize);
		}

		return AmortizeSyncData(EVENT_DATABASE,dwSyncID,cbBuffer,sizeof(NTY_DataBaseEvent)+wDataSize);
	}

	//派发数据
	m_DataBaseEngineArry[nIndex]->PostDataBaseRequest(wRequestID,dwContextID,dwTokenID,pData,wDataSize);

	return true;
}

//请求完成
bool CDataBaseSyncEngine::OnPostRequestComplete(INT_PTR nSyncIndex)
{
	if( m_bService == false ) return false;

	//设置变量
	CThreadLock ThreadLock(m_AsyncCritical);

	//变量定义
	ASSERT(nSyncIndex>=0&&nSyncIndex<m_SyncRequestArry.GetCount());
	tagSyncRequest &SyncRequest = m_SyncRequestArry[nSyncIndex];

	//效验
	ASSERT(SyncRequest.dwRequestCount>0);
	INT_PTR nTmpIndex = -1;
	ASSERT( m_SyncIDMap.Lookup(SyncRequest.dwSyncID,nTmpIndex) );

	if( --SyncRequest.dwRequestCount == 0 )
{
		m_SyncIDMap.RemoveKey(SyncRequest.dwSyncID);
		
		SyncRequest.dwSyncID = 0;
	}

	//执行缓冲
	PerformAmortisation();

	return true;
}

//查找异步引擎
INT_PTR CDataBaseSyncEngine::SearchValidEngine(DWORD dwSyncID)
{
	ASSERT(dwSyncID!=0);

	//查找
	INT_PTR nIndex = -1;
	if( m_SyncIDMap.Lookup(dwSyncID,nIndex) ) return nIndex;
	
	for( INT_PTR i = 0; i < m_SyncRequestArry.GetCount(); i++ )
{
		if( m_SyncRequestArry[i].dwRequestCount == 0 )
	{
			return i;
		}
	}

	return nIndex;
}

//缓冲判断
bool CDataBaseSyncEngine::AmortizeEstimate()
{
	return m_DataQueue.GetLength()>0;
}

//缓冲请求
bool CDataBaseSyncEngine::AmortizeSyncData(WORD wIdentifier, DWORD dwSyncID, VOID * pData, WORD wDataSize)
{
	ASSERT(wDataSize+sizeof(dwSyncID)<=MAX_ASYNC_DATA);
	if( wDataSize+sizeof(dwSyncID) > MAX_ASYNC_DATA )
{
		//构造信息
		TCHAR szDescribe[256]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("数据过长，缓冲失败：CDataBaseEngine::AmortizeSyncData [ wIdentifier=%d wDataSize=%ld ]"),
			wIdentifier,wDataSize);

		//输出信息
		CTraceService::TraceString(szDescribe,TraceLevel_Exception);

		return false;
	}

	//缓冲锁定
	BYTE cbBuffer[MAX_ASYNC_DATA] = {0};
	*((DWORD *)cbBuffer) = dwSyncID;

	ASSERT(pData);
	CopyMemory(cbBuffer+sizeof(dwSyncID),pData,wDataSize);

	//插入数据
	if (m_DataQueue.EnQueue(wIdentifier,cbBuffer,sizeof(dwSyncID)+wDataSize)==false)
{
		//构造信息
		TCHAR szDescribe[256]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("插入缓冲数据失败：CDataBaseEngine::AmortizeSyncData [ wIdentifier=%d wDataSize=%ld ]"),
			wIdentifier,wDataSize);

		//输出信息
		CTraceService::TraceString(szDescribe,TraceLevel_Exception);

		return false;
	}

	return true;
}

//执行缓冲
VOID CDataBaseSyncEngine::PerformAmortisation()
{
	ASSERT(m_bService);

	//变量定义
	tagDataHead DataHead = {0};
	BYTE cbDataBuffer[MAX_ASYNC_DATA] = {0};

	//锁定对象
	if( m_DataQueue.GetLength() > 0 )
{
		do
	{
			//读取数据
			if( !m_DataQueue.GetHead(DataHead,cbDataBuffer,sizeof(cbDataBuffer)) )
		{
				return;
			}

			ASSERT(DataHead.wDataSize!=0);
			DWORD dwSyncID = *((DWORD *)cbDataBuffer);

			//查找同步
			INT_PTR nIndex = SearchValidEngine(dwSyncID);
			if( nIndex == -1 ) 
		{
				return;
			}

			//构造数据
			BYTE *pPostData = cbDataBuffer+sizeof(dwSyncID);
			
			//设置变量
			m_SyncIDMap[dwSyncID] = nIndex;
			tagSyncRequest &SyncRequest = m_SyncRequestArry[nIndex];
			SyncRequest.dwSyncID = dwSyncID;
			SyncRequest.dwRequestCount++;

			//派发消息
			switch(DataHead.wIdentifier)
		{
			case EVENT_CONTROL:
			{
					//变量定义
					NTY_ControlEvent *pControlEvent = (NTY_ControlEvent *)pPostData;

					m_DataBaseEngineArry[nIndex]->PostDataBaseControl(pControlEvent->wControlID,(VOID *)(pControlEvent+1),
						DataHead.wDataSize-sizeof(NTY_ControlEvent)-sizeof(dwSyncID));

					break;
				}
			case EVENT_DATABASE:
			{
					//变量定义
					NTY_DataBaseEvent *pDataBaseEvent = (NTY_DataBaseEvent *)pPostData;

					m_DataBaseEngineArry[nIndex]->PostDataBaseRequest(pDataBaseEvent->wRequestID,pDataBaseEvent->dwContextID,pDataBaseEvent->dwTokenID,(VOID *)(pDataBaseEvent+1),
						DataHead.wDataSize-sizeof(NTY_DataBaseEvent)-sizeof(dwSyncID) );

					break;
				}
			default:
			{
					ASSERT(FALSE);
				}
			}

			//删除队列
			m_DataQueue.DeQueue();

		}while(true);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(DataBaseSyncEngine);

//////////////////////////////////////////////////////////////////////////
