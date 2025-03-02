#include "StdAfx.h"
#include "Math.h"
#include "DataBaseSyncEngine.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseSyncEngine::CDataBaseSyncEngine()
{
	//��������
	m_bService = false;
	m_SyncIDMap.InitHashTable(PRIME_SERVER_USER);

	//�������
	m_pITimerEngine = NULL;
}

//��������
CDataBaseSyncEngine::~CDataBaseSyncEngine()
{
	//ֹͣ����
	ConcludeService();
}

//�ӿڲ�ѯ
VOID * CDataBaseSyncEngine::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServiceModule,Guid,dwQueryVer);
	QUERYINTERFACE(IDataBaseSyncEngine,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseSyncEngine,Guid,dwQueryVer);
	return NULL;
}

//��������
bool CDataBaseSyncEngine::StartService()
{
	ASSERT(!m_bService);
	m_bService = true;
	return true;
}

//ֹͣ����
bool CDataBaseSyncEngine::ConcludeService()
{
	CThreadLock ThreadLock(m_AsyncCritical);

	//���ñ���
	m_bService = false;

	//ͬ��ID
	m_SyncIDMap.RemoveAll();

	m_SyncRequestArry.RemoveAll();

	m_DataBaseEngineArry.RemoveAll();

	m_DataBaseEngineSinkArry.RemoveAll();

	//�������
	m_pITimerEngine = NULL;

	//��������
	m_DataQueue.ClearQueue(true);

	return true;
}

//����ģ��
bool CDataBaseSyncEngine::InitDataBaseSyncEngine(IDataBaseEngine * pIDataBaseEngine[], IDataBaseEngineSink *pIDataBaseEngineSink[], WORD wSinkCount)
{
	ASSERT(!m_bService);
	ASSERT(wSinkCount<=64);
	if( wSinkCount > 64 ) return false;
	ASSERT(m_DataBaseEngineArry.GetCount()==0);
	if( m_DataBaseEngineArry.GetCount() > 0 ) return false;

	//��������
	m_DataBaseEngineArry.SetSize(wSinkCount);
	m_DataBaseEngineSinkArry.SetSize(wSinkCount);
	m_SyncRequestArry.SetSize(wSinkCount);

	//���ö���
	for (INT_PTR i=0;i<m_DataBaseEngineArry.GetCount();i++)
{
		m_DataBaseEngineArry[i] = pIDataBaseEngine[i];

		m_DataBaseEngineSinkArry[i] = pIDataBaseEngineSink[i];
	}

	return true;
}

//��ѯ����
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

//����ʱ��
bool CDataBaseSyncEngine::SetTimer(INT_PTR nSyncIndex, DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	//Ч�����
	ASSERT(dwTimerID<TIME_SYNC_DATABASE_SINK_RANGE);
	if (dwTimerID>=TIME_SYNC_DATABASE_SINK_RANGE) return false;

	//����ʱ��
	DWORD dwEngineTimerID=IDI_SYNC_DATABASE_SINK_START+(DWORD)nSyncIndex*TIME_SYNC_DATABASE_SINK_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->SetTimer(dwEngineTimerID+dwTimerID,dwElapse,dwRepeat,dwBindParameter);

	return true;
}

//ɾ��ʱ��
bool CDataBaseSyncEngine::KillTimer(INT_PTR nSyncIndex, DWORD dwTimerID)
{
	//Ч�����
	ASSERT(dwTimerID<TIME_SYNC_DATABASE_SINK_RANGE);
	if (dwTimerID>=TIME_SYNC_DATABASE_SINK_RANGE) return false;

	//ɾ��ʱ��
	DWORD dwEngineTimerID=IDI_SYNC_DATABASE_SINK_START+(DWORD)nSyncIndex*TIME_SYNC_DATABASE_SINK_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->KillTimer(dwEngineTimerID+dwTimerID);

	return true;
}

//ʱ���¼�
bool CDataBaseSyncEngine::PostDataBaseTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//��������
	DWORD dwSinkID=dwTimerID-IDI_SYNC_DATABASE_SINK_START;
	INT_PTR nSyncIndex=(INT_PTR)(dwSinkID/TIME_SYNC_DATABASE_SINK_RANGE);

	//ʱ��Ч��
	if (nSyncIndex>=m_DataBaseEngineArry.GetCount()) 
{
		ASSERT(FALSE);
		return false;
	}

	//ʱ��֪ͨ
	IDataBaseEngine * pIDataBaseEngine=m_DataBaseEngineArry[nSyncIndex];
	return pIDataBaseEngine->PostDataBaseTimer(dwSinkID%TIME_SYNC_DATABASE_SINK_RANGE,dwBindParameter);
}

//�����¼�
bool CDataBaseSyncEngine::PostDataBaseControl(DWORD dwSyncID, WORD wControlID, VOID * pData, WORD wDataSize)
{
	ASSERT(m_bService);

	//����ӳ��
	CThreadLock ThreadLock(m_AsyncCritical);

	//����ͬ��ID
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
		//��������
		NTY_ControlEvent ControlEvent = {0};
		ControlEvent.wControlID = wControlID;

		return AmortizeSyncData(EVENT_CONTROL,dwSyncID,&ControlEvent,sizeof(ControlEvent));
	}

	//�ɷ�����
	m_DataBaseEngineArry[nIndex]->PostDataBaseControl(wControlID,pData,wDataSize);

	return true;
}

//�����¼�
bool CDataBaseSyncEngine::PostDataBaseRequest(DWORD dwSyncID, WORD wRequestID, DWORD dwContextID, DWORD dwTokenID,VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(m_bService);
	ASSERT((wDataSize+sizeof(NTY_DataBaseEvent)+sizeof(dwSyncID))<=MAX_ASYNC_DATA);
	if ((wDataSize+sizeof(NTY_DataBaseEvent))>MAX_ASYNC_DATA) 
{
		//������Ϣ
		TCHAR szDescribe[256]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("���ݹ���������ʧ�ܣ�CSyncDataBaseEngine::PostDataBaseRequest [ dwSyncID=%ld wRequestID=%d wDataSize=%ld ]"),
			dwSyncID,wRequestID,wDataSize);

		//�����Ϣ
		CTraceService::TraceString(szDescribe,TraceLevel_Exception);

		return false;
	}

	//����ӳ��
	CThreadLock ThreadLock(m_AsyncCritical);

	//����ͬ��ID
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
		//��������
		BYTE cbBuffer[MAX_ASYNC_DATA];
		NTY_DataBaseEvent * pDataBaseEvent=(NTY_DataBaseEvent *)cbBuffer;

		//��������		
		pDataBaseEvent->dwTokenID=dwTokenID;
		pDataBaseEvent->wRequestID=wRequestID;		
		pDataBaseEvent->dwContextID=dwContextID;

		//��������
		if (wDataSize>0)
	{
			ASSERT(pData!=NULL);
			CopyMemory(cbBuffer+sizeof(NTY_DataBaseEvent),pData,wDataSize);
		}

		return AmortizeSyncData(EVENT_DATABASE,dwSyncID,cbBuffer,sizeof(NTY_DataBaseEvent)+wDataSize);
	}

	//�ɷ�����
	m_DataBaseEngineArry[nIndex]->PostDataBaseRequest(wRequestID,dwContextID,dwTokenID,pData,wDataSize);

	return true;
}

//�������
bool CDataBaseSyncEngine::OnPostRequestComplete(INT_PTR nSyncIndex)
{
	if( m_bService == false ) return false;

	//���ñ���
	CThreadLock ThreadLock(m_AsyncCritical);

	//��������
	ASSERT(nSyncIndex>=0&&nSyncIndex<m_SyncRequestArry.GetCount());
	tagSyncRequest &SyncRequest = m_SyncRequestArry[nSyncIndex];

	//Ч��
	ASSERT(SyncRequest.dwRequestCount>0);
	INT_PTR nTmpIndex = -1;
	ASSERT( m_SyncIDMap.Lookup(SyncRequest.dwSyncID,nTmpIndex) );

	if( --SyncRequest.dwRequestCount == 0 )
{
		m_SyncIDMap.RemoveKey(SyncRequest.dwSyncID);
		
		SyncRequest.dwSyncID = 0;
	}

	//ִ�л���
	PerformAmortisation();

	return true;
}

//�����첽����
INT_PTR CDataBaseSyncEngine::SearchValidEngine(DWORD dwSyncID)
{
	ASSERT(dwSyncID!=0);

	//����
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

//�����ж�
bool CDataBaseSyncEngine::AmortizeEstimate()
{
	return m_DataQueue.GetLength()>0;
}

//��������
bool CDataBaseSyncEngine::AmortizeSyncData(WORD wIdentifier, DWORD dwSyncID, VOID * pData, WORD wDataSize)
{
	ASSERT(wDataSize+sizeof(dwSyncID)<=MAX_ASYNC_DATA);
	if( wDataSize+sizeof(dwSyncID) > MAX_ASYNC_DATA )
{
		//������Ϣ
		TCHAR szDescribe[256]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("���ݹ���������ʧ�ܣ�CDataBaseEngine::AmortizeSyncData [ wIdentifier=%d wDataSize=%ld ]"),
			wIdentifier,wDataSize);

		//�����Ϣ
		CTraceService::TraceString(szDescribe,TraceLevel_Exception);

		return false;
	}

	//��������
	BYTE cbBuffer[MAX_ASYNC_DATA] = {0};
	*((DWORD *)cbBuffer) = dwSyncID;

	ASSERT(pData);
	CopyMemory(cbBuffer+sizeof(dwSyncID),pData,wDataSize);

	//��������
	if (m_DataQueue.EnQueue(wIdentifier,cbBuffer,sizeof(dwSyncID)+wDataSize)==false)
{
		//������Ϣ
		TCHAR szDescribe[256]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("���뻺������ʧ�ܣ�CDataBaseEngine::AmortizeSyncData [ wIdentifier=%d wDataSize=%ld ]"),
			wIdentifier,wDataSize);

		//�����Ϣ
		CTraceService::TraceString(szDescribe,TraceLevel_Exception);

		return false;
	}

	return true;
}

//ִ�л���
VOID CDataBaseSyncEngine::PerformAmortisation()
{
	ASSERT(m_bService);

	//��������
	tagDataHead DataHead = {0};
	BYTE cbDataBuffer[MAX_ASYNC_DATA] = {0};

	//��������
	if( m_DataQueue.GetLength() > 0 )
{
		do
	{
			//��ȡ����
			if( !m_DataQueue.GetHead(DataHead,cbDataBuffer,sizeof(cbDataBuffer)) )
		{
				return;
			}

			ASSERT(DataHead.wDataSize!=0);
			DWORD dwSyncID = *((DWORD *)cbDataBuffer);

			//����ͬ��
			INT_PTR nIndex = SearchValidEngine(dwSyncID);
			if( nIndex == -1 ) 
		{
				return;
			}

			//��������
			BYTE *pPostData = cbDataBuffer+sizeof(dwSyncID);
			
			//���ñ���
			m_SyncIDMap[dwSyncID] = nIndex;
			tagSyncRequest &SyncRequest = m_SyncRequestArry[nIndex];
			SyncRequest.dwSyncID = dwSyncID;
			SyncRequest.dwRequestCount++;

			//�ɷ���Ϣ
			switch(DataHead.wIdentifier)
		{
			case EVENT_CONTROL:
			{
					//��������
					NTY_ControlEvent *pControlEvent = (NTY_ControlEvent *)pPostData;

					m_DataBaseEngineArry[nIndex]->PostDataBaseControl(pControlEvent->wControlID,(VOID *)(pControlEvent+1),
						DataHead.wDataSize-sizeof(NTY_ControlEvent)-sizeof(dwSyncID));

					break;
				}
			case EVENT_DATABASE:
			{
					//��������
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

			//ɾ������
			m_DataQueue.DeQueue();

		}while(true);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(DataBaseSyncEngine);

//////////////////////////////////////////////////////////////////////////
