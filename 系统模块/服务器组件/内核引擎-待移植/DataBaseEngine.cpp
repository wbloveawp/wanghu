#include "StdAfx.h"
#include "Math.h"
#include "DataBaseEngine.h"
#include "TraceServiceManager.h"
#include "TraceService.h"

/////////////////////////////////////////////////////////////////////////////

CDataBaseConnection::CDataBaseConnection()
{
	m_hCompletionPort = NULL;
	m_pIDataBaseEngineSink = NULL;
	m_pDataBaseEngine = NULL;
	m_pConnectNext = NULL;
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));
}

CDataBaseConnection::~CDataBaseConnection()
{
}

//���ݴ�����
bool CDataBaseConnection::OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize)
{

	if(wIdentifier == EVENT_TIMER)		//ʱ���¼�
	{
		if (wDataSize != sizeof(NTY_TimerEvent)) return false;

		//��������
		NTY_TimerEvent * pTimerEvent = (NTY_TimerEvent *)pData;
		//����
		return m_pIDataBaseEngineSink->OnDataBaseEngineTimer(pTimerEvent->dwTimerID, pTimerEvent->dwBindParameter);
	}
	else if (wIdentifier == EVENT_CONTROL)		//�����¼�
	{
		if (wDataSize < sizeof(NTY_ControlEvent)) return false;

		//��������
		NTY_ControlEvent * pControlEvent = (NTY_ControlEvent *)pData;

		return m_pIDataBaseEngineSink->OnDataBaseEngineControl(pControlEvent->wControlID, ((BYTE*)pData)+sizeof(NTY_ControlEvent), wDataSize - sizeof(NTY_ControlEvent));

	}
	else if(wIdentifier == EVENT_DATABASE)	//�����¼�
	{
		if (wDataSize < sizeof(NTY_DataBaseEvent)) return false;
		//��������
		NTY_DataBaseEvent * pDataBaseEvent = (NTY_DataBaseEvent *)pData;

		return m_pIDataBaseEngineSink->OnDataBaseEngineRequest(pDataBaseEvent->wRequestID, 
			pDataBaseEvent->dwContextID,
			((BYTE*)pData) + sizeof(NTY_DataBaseEvent),
			wDataSize - sizeof(NTY_DataBaseEvent));
	}

	return false;
}

//�߳�ִ�к��� �����Ӷ����л�ȡ���ݲ�����
bool CDataBaseConnection::OnEventThreadRun()
{

	//��������
	DWORD dwThancferred=0;
	OVERLAPPED * pOverLapped=NULL;
	CDataBaseEngine * pDataBaseEngine=NULL;

	//��ɶ˿�
	if (GetQueuedCompletionStatus(m_hCompletionPort,&dwThancferred,(PULONG_PTR)&pDataBaseEngine,&pOverLapped,INFINITE) 
		&& pDataBaseEngine != NULL)
	{
		//��������
		CThreadLock ThreadLock(m_CriticalSection);

		//��ȡ����
		tagDataHead DataHead;
		m_DataQueue.DistillData(DataHead,m_cbBuffer,sizeof(m_cbBuffer));
		//���н���
		ThreadLock.UnLock();
		//����
		bool bSuccess = OnAsynchronismEngineData(DataHead.wIdentifier, m_cbBuffer, DataHead.wDataSize);

		//�����߳�
		pDataBaseEngine->RectifyThread(this, bSuccess);

		//������
		return true;
	}
	return false;
}

//�̺߳�������ǰ�ص�  ����IO��ɶ˿�  �����ù��ӻص�����
bool CDataBaseConnection::OnEventThreadStrat()
{
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);

	if (m_hCompletionPort == NULL)
	{
		return false;
	}

	if (m_pDataBaseEngine)
	{
		//��ѯ�ӿ�
		IUnknownEx * pIUnknownEx = QUERY_OBJECT_PTR_INTERFACE(m_pDataBaseEngine, IUnknownEx);
		//�����¼�
		return m_pIDataBaseEngineSink->OnDataBaseEngineStart(pIUnknownEx);
	}
	else
	{
		return m_pIDataBaseEngineSink->OnDataBaseEngineStart(NULL);
	}
}

//�̹߳رջص�  ������ɶ˿� �� ���ù��ӻص�
bool CDataBaseConnection::OnEventThreadConclude()
{
	//�ر���ɶ˿�
	if (m_hCompletionPort)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort = NULL;
	}

	if (m_pDataBaseEngine)
	{
		//��ѯ�ӿ�
		IUnknownEx * pIUnknownEx = QUERY_OBJECT_PTR_INTERFACE(m_pDataBaseEngine, IUnknownEx);
		//�����¼�
		return m_pIDataBaseEngineSink->OnDataBaseEngineConclude(pIUnknownEx);
	}
	else
	{
		return m_pIDataBaseEngineSink->OnDataBaseEngineConclude(NULL);
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseEngine::CDataBaseEngine()
{
	//���ñ���
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));
	m_pConnectionFree = NULL;
	m_pConnectionActive = NULL;
	m_TimerEngine.SetTimerEngineEvent(QUERY_ME_INTERFACE(IUnknownEx));
}

//��������
CDataBaseEngine::~CDataBaseEngine()
{
}

//�ӿڲ�ѯ
VOID * CDataBaseEngine::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServiceModule,Guid,dwQueryVer);
	QUERYINTERFACE(IDataBaseEngine,Guid,dwQueryVer);
	QUERYINTERFACE(ITimerEngineEvent, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngine,Guid,dwQueryVer);
	return NULL;
}

//��������
bool CDataBaseEngine::StartService()
{
	//������ʱ������

	//���������̷߳���
	if (m_pConnectionFree != NULL && m_pConnectionActive == NULL)
	{
		m_bService = true;

		CDataBaseConnection *pConnection = m_pConnectionFree;

		while ( pConnection != NULL)
		{
			if (!pConnection->StartThread()) return false;
			pConnection = pConnection->m_pConnectNext;
		}

		//������ʱ��
		if (!m_TimerEngine.StartService()) return false;

		//���ö�ʱ��
		m_TimerEngine.SetTimer(EVENT_TIMER, 60000, TIMES_INFINITY, 0);

		return true;
	}
	return false;
}

//ֹͣ����
bool CDataBaseEngine::ConcludeService()
{
	m_bService = false;

	//ֹͣ��ʱ������
	m_TimerEngine.ConcludeService();

	while (m_pConnectionFree)
	{
		CDataBaseConnection * pConnection = m_pConnectionFree;
		m_pConnectionFree = pConnection->m_pConnectNext;

		PostQueuedCompletionStatus(pConnection->m_hCompletionPort, 0, NULL, NULL);
		pConnection->ConcludeThread(INFINITE);

		SafeDelete(pConnection);

	}

	m_pConnectionActive = NULL;
	m_pConnectionFree = NULL;
	return true;
}

//���渺��
bool CDataBaseEngine::GetBurthenInfo(tagBurthenInfo & BurthenInfo)
{
	CThreadLock ThreadLock(m_CriticalSection);
	m_DataQueue.GetBurthenInfo(BurthenInfo);
	return true;
}

//����ģ��
bool CDataBaseEngine::SetDataBaseEngineSink(IUnknownEx * pIUnknownEx)
{

	IDataBaseEngineSink *pIDataBaseEngineSink = NULL;

	if (m_pConnectionFree || m_pConnectionActive) return false;
	
	if(!pIUnknownEx || (pIDataBaseEngineSink = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IDataBaseEngineSink)) == NULL) return false;

	CDataBaseConnection* pConnection = new CDataBaseConnection();
	//��ǰ����ָ��
	pConnection->m_pDataBaseEngine = this;
	//���Ӷ���
	pConnection->m_pIDataBaseEngineSink = pIDataBaseEngineSink;

	pConnection->m_pConnectNext = m_pConnectionFree;
	m_pConnectionFree = pConnection;

	return false;
}

//����ģ��
bool CDataBaseEngine::SetDataBaseEngineSink(IUnknownEx * pIUnknownEx[], WORD wSinkCount)
{
	if (m_pConnectionFree || m_pConnectionActive) return false;
	
	for (INT_PTR i = 0; i < wSinkCount; i++)
	{
		IDataBaseEngineSink *pIDataBaseEngineSink = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx[i], IDataBaseEngineSink);
		if (pIDataBaseEngineSink == NULL) return false;

		CDataBaseConnection* pConnection = new CDataBaseConnection();
		//��ǰ����ָ��
		pConnection->m_pDataBaseEngine = this;
		//���Ӷ���
		pConnection->m_pIDataBaseEngineSink = pIDataBaseEngineSink;

		pConnection->m_pConnectNext = m_pConnectionFree;
		m_pConnectionFree = pConnection;
	}

	return true;
}

bool CDataBaseEngine::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//����������
	if (!m_bService) return false;

	NTY_TimerEvent pTimerEvent;
	pTimerEvent.dwTimerID = dwTimerID;
	pTimerEvent.dwBindParameter = dwBindParameter;

	PostDataBaseRequest(EVENT_TIMER, &pTimerEvent, sizeof(NTY_TimerEvent));

	return true;
}

bool CDataBaseEngine::PostDataBaseRequest(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	CThreadLock ThreadLock(m_CriticalSection);

	//����ѡ����ʵ��߳̽���Ͷ��
	
	if (m_pConnectionFree)
	{
		CDataBaseConnection * pConnectionFree = m_pConnectionFree;
		CDataBaseConnection * pConnectionActive = m_pConnectionActive;

		m_pConnectionActive = pConnectionFree;
		pConnectionFree->m_pConnectNext = pConnectionActive;

		m_pConnectionFree = m_pConnectionFree->m_pConnectNext;
		//��������
		CThreadLock ThreadLock(pConnectionFree->m_CriticalSection);
		pConnectionFree->m_DataQueue.InsertData(wIdentifier, pData, wDataSize);
		PostQueuedCompletionStatus(pConnectionFree->m_hCompletionPort, 0, (ULONG_PTR)this, NULL);
	}
	else //���û�к��ʵ��߳̽���Ͷ�� ��ô����ͱ��浽��ǰ�Ķ����� ���к��ʵ��̺߳� �ٽ���Ͷ��
	{
		m_DataQueue.InsertData(wIdentifier, pData, wDataSize);
	}
	return true;
}

bool CDataBaseEngine::PostDataBaseRequest(WORD wIdentifier, tagDataBuffer DataBuffer[], WORD wDataCount)
{

	CThreadLock ThreadLock(m_CriticalSection);

	//����ѡ����ʵ��߳̽���Ͷ��
	
	if (m_pConnectionFree)
	{
		CDataBaseConnection * pConnectionFree = m_pConnectionFree;
		CDataBaseConnection * pConnectionActive = m_pConnectionActive;

		m_pConnectionActive = pConnectionFree;
		pConnectionFree->m_pConnectNext = pConnectionActive;

		m_pConnectionFree = m_pConnectionFree->m_pConnectNext;

		CThreadLock ThreadLock(pConnectionFree->m_CriticalSection);
		pConnectionFree->m_DataQueue.InsertData(wIdentifier, DataBuffer, wDataCount);
		PostQueuedCompletionStatus(pConnectionFree->m_hCompletionPort, 0, (ULONG_PTR)this, NULL);
	}
	else //���û�к��ʵ��߳̽���Ͷ�� ��ô����ͱ��浽��ǰ�Ķ����� ���к��ʵ��̺߳� �ٽ���Ͷ��
	{
		m_DataQueue.InsertData(wIdentifier, DataBuffer, wDataCount);
	}
	return true;
}

bool CDataBaseEngine::DeferDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	return true;
}

bool CDataBaseEngine::RectifyThread(CDataBaseConnection * pCurConnection, bool bSuccess)
{
	//������ǰ�߳�
	CThreadLock ThreadLock(m_CriticalSection);

	//�̺߳������ݴ�����ɺ�
	if (m_DataQueue.GetDataPacketCount())
	{
		//���̴߳�������� ��鵱ǰ�������Ƿ���������Ҫ���� ����� ����뵽�̶߳���
		//��������ȡ����
		tagDataHead DataHead;
		m_DataQueue.DistillData(DataHead, m_cbBuffer, sizeof(m_cbBuffer));
		//�������ݴ����߳�
		CThreadLock ThreadLock(pCurConnection->m_CriticalSection);
		//�����������̶߳���
		pCurConnection->m_DataQueue.InsertData(DataHead.wIdentifier, m_cbBuffer, DataHead.wDataSize);
		//֪ͨ�̴߳���
		PostQueuedCompletionStatus(pCurConnection->m_hCompletionPort, 0, (ULONG_PTR)this, NULL);
	}
	else
	{
		CDataBaseConnection * pConnectionPrev = NULL;
		CDataBaseConnection * pConnectionFree = m_pConnectionFree;
		CDataBaseConnection * pConnection = NULL;
		for (pConnection = m_pConnectionActive; pConnection; pConnection = pConnection->m_pConnectNext)
		{
			if (pConnection == pCurConnection)
			{
				if (pConnectionPrev)
					pConnectionPrev->m_pConnectNext = pConnection->m_pConnectNext;
				else
					m_pConnectionActive = pConnection->m_pConnectNext;

				m_pConnectionFree = pConnection;
				pConnection->m_pConnectNext = pConnectionFree;

				break;
			}
			pConnectionPrev = pConnection;
		}
		
	}
	return true;
}


//�����¼�
bool CDataBaseEngine::PostDataBaseControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	if (!m_bService) return false;
	//Ч�����

	//Ͷ����Ϣ
	NTY_ControlEvent pControlEvent;
	pControlEvent.wControlID = wControlID;
	return PostDataBaseRequest(EVENT_CONTROL, &pControlEvent, sizeof(NTY_ControlEvent));
}

//�����¼�
bool CDataBaseEngine::PostDataBaseRequest(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	if (!m_bService) return false;

	ASSERT((wDataSize + sizeof(NTY_DataBaseEvent)) <= MAX_ASYNCHRONISM_DATA);
	if ((wDataSize + sizeof(NTY_DataBaseEvent)) > MAX_ASYNCHRONISM_DATA) return false;

	NTY_DataBaseEvent pDataBaseEvent;
	pDataBaseEvent.wRequestID = wRequestID;
	pDataBaseEvent.dwContextID = dwContextID;
	
	if (wDataSize > 0)
	{
		tagDataBuffer pDataBuffer[2];

		pDataBuffer[0].pDataBuffer = &pDataBaseEvent;
		pDataBuffer[0].wDataSize = sizeof(NTY_DataBaseEvent);

		pDataBuffer[1].pDataBuffer = pData;
		pDataBuffer[1].wDataSize = wDataSize;

		PostDataBaseRequest(EVENT_DATABASE, pDataBuffer, 2);

	}
	else
	{
		PostDataBaseRequest(EVENT_DATABASE, &pDataBaseEvent, sizeof(NTY_DataBaseEvent));
	}

	return true;

}



//////////////////////////////////////////////////////////////////////////


DECLARE_CREATE_MODULE(DataBaseEngine);

//////////////////////////////////////////////////////////////////////////
