#include "StdAfx.h"
#include "AttemperEngine.h"
#include "TraceServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngine::CAttemperEngine()
{
	//�ӿڱ���
	m_pITCPNetworkEngine=NULL;
	m_pIAttemperEngineSink=NULL;

	//��������
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));

	return;
}

//��������
CAttemperEngine::~CAttemperEngine()
{
}

//�ӿڲ�ѯ
VOID * CAttemperEngine::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngine,Guid,dwQueryVer);
	QUERYINTERFACE(ITCPSocketEvent,Guid,dwQueryVer);
	QUERYINTERFACE(ITimerEngineEvent,Guid,dwQueryVer);
	QUERYINTERFACE(IDataBaseEngineEvent,Guid,dwQueryVer);
	QUERYINTERFACE(ITCPNetworkEngineEvent,Guid,dwQueryVer);
	QUERYINTERFACE(IAsyncEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngine,Guid,dwQueryVer);
	return NULL;
}

//��������
bool CAttemperEngine::StartService()
{
	//Ч�����
	ASSERT((m_pITCPNetworkEngine!=NULL)&&(m_pIAttemperEngineSink!=NULL));
	if ((m_pITCPNetworkEngine != NULL) && (m_pIAttemperEngineSink != NULL))
	{

		//ע�����
		IUnknownEx * pIAsynchronismEngineSink = QUERY_ME_INTERFACE(IUnknownEx);

		if (m_AsynchronismEngine.SetAsynchronismSink(pIAsynchronismEngineSink) == false)
		{
			ASSERT(FALSE);
			return false;
		}

		//�첽����
		if (m_AsynchronismEngine.StartService() == false) return false;

		return true;
	}

	return false;
}

//ֹͣ����
bool CAttemperEngine::ConcludeService()
{
	//�첽����
	m_AsynchronismEngine.ConcludeService();

	return true;
}

//����ӿ�
bool CAttemperEngine::SetNetworkEngine(IUnknownEx * pIUnknownEx)
{
	//���ýӿ�
	if (pIUnknownEx!=NULL)
	{
		//��ѯ�ӿ�
		m_pITCPNetworkEngine=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITCPNetworkEngine);

		//�ɹ��ж�
		return  (m_pITCPNetworkEngine != NULL);
	}
	else 
	{
		m_pITCPNetworkEngine = NULL;
	}

	return true;
}

//�ص��ӿ�
bool CAttemperEngine::SetAttemperEngineSink(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	if (pIUnknownEx != NULL)
	{
		m_pIAttemperEngineSink = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IAttemperEngineSink);

		//����ж�
		return (m_pIAttemperEngineSink != NULL);
	}
	else
	{
		m_pIAttemperEngineSink = NULL;
	}

	return true;
}

//�Զ��¼�
bool CAttemperEngine::OnEventCustom(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT((wRequestID&EVENT_MASK_CUSTOM)==wRequestID);
	if ((wRequestID&EVENT_MASK_CUSTOM)!=wRequestID) return false;

	//Ͷ������
	return m_AsynchronismEngine.PostAsynchronismData(wRequestID,pData,wDataSize);
}

//������Ϣ
bool CAttemperEngine::OnEventControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT((wDataSize+sizeof(NTY_ControlEvent))<= MAX_ASYNC_DATA);
	if ((wDataSize+sizeof(NTY_ControlEvent))> MAX_ASYNC_DATA) return false;

	//��������
	CThreadLock ThreadLock(m_CriticalLocker);
	NTY_ControlEvent pControlEvent;
	
	//��������
	pControlEvent.wControlID=wControlID;
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_CONTROL, &pControlEvent, sizeof(NTY_ControlEvent));
	//��������
	/*if (wDataSize>0)
	{
		tagDataBuffer DataBuffer[2];
		DataBuffer[0].pDataBuffer = &pControlEvent;
		DataBuffer[0].wDataSize = sizeof(NTY_ControlEvent);

		DataBuffer[1].pDataBuffer = pData;
		DataBuffer[1].wDataSize = wDataSize;

		return m_AsynchronismEngine.PostAsynchronismData(EVENT_CONTROL, DataBuffer, 2);
	}
	else
	{
		//Ͷ������
		return m_AsynchronismEngine.PostAsynchronismData(EVENT_CONTROL, &pControlEvent, sizeof(NTY_ControlEvent));
	}*/

}

//��ʱ���¼�
bool CAttemperEngine::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//��������
	CThreadLock ThreadLock(m_CriticalLocker);
	NTY_TimerEvent pTimerEvent;
	
	//��������
	pTimerEvent.dwTimerID=dwTimerID;
	pTimerEvent.dwBindParameter=dwBindParameter;

	//Ͷ������
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TIMER,&pTimerEvent,sizeof(NTY_TimerEvent));
}

//���ݿ���
bool CAttemperEngine::OnEventDataBaseResult(WORD wRequestID, DWORD dwContextID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT((wDataSize+sizeof(NTY_DataBaseEvent))<= MAX_ASYNC_DATA);
	if ((wDataSize+sizeof(NTY_DataBaseEvent))> MAX_ASYNC_DATA) return false;

	//��������
	CThreadLock ThreadLock(m_CriticalLocker);
	NTY_DataBaseEvent pDataBaseEvent;
	
	//��������
	pDataBaseEvent.wRequestID=wRequestID;
	pDataBaseEvent.dwContextID=dwContextID;

	//��������
	if (wDataSize>0)
	{
		tagDataBuffer DataBuffer[2];
		DataBuffer[0].pDataBuffer = &pDataBaseEvent;
		DataBuffer[0].wDataSize = sizeof(NTY_DataBaseEvent);

		DataBuffer[1].pDataBuffer = pData;
		DataBuffer[1].wDataSize = wDataSize;

		return m_AsynchronismEngine.PostAsynchronismData(EVENT_DATABASE, DataBuffer, 2);
	}
	else
	{

		//Ͷ������
		return m_AsynchronismEngine.PostAsynchronismData(EVENT_DATABASE, &pDataBaseEvent, sizeof(NTY_DataBaseEvent));
	}
}

//�����¼�
bool CAttemperEngine::OnEventTCPSocketLink(WORD wServiceID, INT nErrorCode)
{
	//��������
	CThreadLock ThreadLock(m_CriticalLocker);
	NTY_TCPSocketConnectEvent pConnectEvent;
	
	//��������
	pConnectEvent.dwServiceID=wServiceID;
	pConnectEvent.nErrorCode=nErrorCode;

	//Ͷ������
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_SOCKET_CONNECT,&pConnectEvent,sizeof(NTY_TCPSocketConnectEvent));
}

//�ر��¼�
bool CAttemperEngine::OnEventTCPSocketShut(WORD wServiceID, BYTE cbShutReason)
{
	//��������
	CThreadLock ThreadLock(m_CriticalLocker);
	NTY_TCPSocketCloseEvent pCloseEvent;
	
	//��������
	pCloseEvent.dwServiceID=wServiceID;
	pCloseEvent.cbShutReason=cbShutReason;

	//Ͷ������
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_SOCKET_CONNECT,&pCloseEvent,sizeof(NTY_TCPSocketCloseEvent));
}

//��ȡ�¼�
bool CAttemperEngine::OnEventTCPSocketRead(WORD wServiceID, CMD_Command Command, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT((wDataSize+sizeof(NTY_TCPSocketReadEvent))<= MAX_ASYNC_DATA);
	if ((wDataSize+sizeof(NTY_TCPSocketReadEvent))> MAX_ASYNC_DATA) return false;

	//��������
	CThreadLock ThreadLock(m_CriticalLocker);
	NTY_TCPSocketReadEvent ReadEvent;
	
	//��������
	ReadEvent.Command=Command;
	ReadEvent.wDataSize=wDataSize;
	ReadEvent.dwServiceID =wServiceID;

	//��������
	if (wDataSize>0)
	{
		tagDataBuffer DataBuffer[2];
		DataBuffer[0].pDataBuffer = &ReadEvent;
		DataBuffer[0].wDataSize = sizeof(NTY_TCPSocketReadEvent);

		DataBuffer[1].pDataBuffer = pData;
		DataBuffer[1].wDataSize = wDataSize;

		return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_SOCKET_READ, DataBuffer, 2);
	}
	else
	{
		//Ͷ������
		return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_SOCKET_READ, &ReadEvent, sizeof(NTY_TCPSocketReadEvent));
	}
}

//Ӧ���¼�
bool CAttemperEngine::OnEventTCPNetworkBind(DWORD dwSocketID, DWORD dwClientAddr)
{
	//��������
	CThreadLock ThreadLock(m_CriticalLocker);
	NTY_TCPNetworkAcceptEvent pAcceptEvent;
	
	//��������
	pAcceptEvent.dwSocketID=dwSocketID;
	pAcceptEvent.dwClientIP=dwClientAddr;

	//Ͷ������
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_NETWORK_ACCEPT,&pAcceptEvent,sizeof(NTY_TCPNetworkAcceptEvent));
}

//�ر��¼�
bool CAttemperEngine::OnEventTCPNetworkShut(DWORD dwSocketID, DWORD dwClientAddr, DWORD dwActiveTime)
{
	//��������
	CThreadLock ThreadLock(m_CriticalLocker);
	NTY_TCPNetworkCloseEvent pCloseEvent;
	
	//��������
	pCloseEvent.dwSocketID=dwSocketID;
	pCloseEvent.dwClientIP=dwClientAddr;
	pCloseEvent.dwActiveTime=dwActiveTime;

	//Ͷ������
	return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_SOCKET_CONNECT,&pCloseEvent,sizeof(NTY_TCPNetworkCloseEvent));
}

//��ȡ�¼�
bool CAttemperEngine::OnEventTCPNetworkRead(DWORD dwSocketID, CMD_Command Command, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT((wDataSize+sizeof(NTY_TCPNetworkReadEvent))<= MAX_ASYNC_DATA);
	if ((wDataSize+sizeof(NTY_TCPNetworkReadEvent))> MAX_ASYNC_DATA) return false;

	//��������
	CThreadLock ThreadLock(m_CriticalLocker);
	NTY_TCPNetworkReadEvent pReadEvent;
	
	//��������
	pReadEvent.Command=Command;
	pReadEvent.wDataSize=wDataSize;
	pReadEvent.dwSocketID=dwSocketID;

	//��������
	if (wDataSize>0)
	{
		tagDataBuffer DataBuffer[2];
		DataBuffer[0].pDataBuffer = &pReadEvent;
		DataBuffer[0].wDataSize = sizeof(NTY_TCPNetworkReadEvent);

		DataBuffer[1].pDataBuffer = pData;
		DataBuffer[1].wDataSize = wDataSize;

		return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_NETWORK_READ, DataBuffer, 2);
	}
	else
	{
		//Ͷ������
		return m_AsynchronismEngine.PostAsynchronismData(EVENT_TCP_NETWORK_READ, &pReadEvent, sizeof(NTY_TCPNetworkReadEvent));
	}
}

//�����¼�
bool CAttemperEngine::OnAsynchronismEngineStart()
{
	//Ч�����
	ASSERT(m_pIAttemperEngineSink!=NULL);
	if (m_pIAttemperEngineSink==NULL) return false;

	//����֪ͨ
	if (m_pIAttemperEngineSink->OnAttemperEngineStart(QUERY_ME_INTERFACE(IUnknownEx))==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//ֹͣ�¼�
bool CAttemperEngine::OnAsynchronismEngineConclude()
{
	//Ч�����
	ASSERT(m_pIAttemperEngineSink!=NULL);
	if (m_pIAttemperEngineSink==NULL) return false;

	//ֹ֪ͣͨ
	if (m_pIAttemperEngineSink->OnAsyncEngineConclude(QUERY_ME_INTERFACE(IUnknownEx))==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//�첽����
bool CAttemperEngine::OnAsynchronismEngineData(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(m_pITCPNetworkEngine!=NULL);
	ASSERT(m_pIAttemperEngineSink!=NULL);

	//�ں��¼�
	switch (wIdentifier)
	{
	case EVENT_TIMER:				//ʱ���¼�
		{
			//��С����
			ASSERT(wDataSize==sizeof(NTY_TimerEvent));
			if (wDataSize!=sizeof(NTY_TimerEvent)) return false;

			//������Ϣ
			NTY_TimerEvent * pTimerEvent=(NTY_TimerEvent *)pData;
			m_pIAttemperEngineSink->OnEventTimer(pTimerEvent->dwTimerID,pTimerEvent->dwBindParameter);

			return true;
		}
	case EVENT_CONTROL:				//������Ϣ
		{
			//��С����
			ASSERT(wDataSize>=sizeof(NTY_ControlEvent));
			if (wDataSize<sizeof(NTY_ControlEvent)) return false;

			//������Ϣ
			NTY_ControlEvent * pControlEvent=(NTY_ControlEvent *)pData;
			m_pIAttemperEngineSink->OnEventControl(pControlEvent->wControlID, ((BYTE*)pData)+sizeof(NTY_ControlEvent),wDataSize-sizeof(NTY_ControlEvent));

			return true;
		}
	case EVENT_DATABASE:			//���ݿ��¼�
		{
			//��С����
			ASSERT(wDataSize>=sizeof(NTY_DataBaseEvent));
			if (wDataSize<sizeof(NTY_DataBaseEvent)) return false;

			//������Ϣ
			NTY_DataBaseEvent * pDataBaseEvent=(NTY_DataBaseEvent *)pData;
			m_pIAttemperEngineSink->OnEventDataBase(pDataBaseEvent->wRequestID,pDataBaseEvent->dwContextID, ((BYTE*)pData)+sizeof(NTY_DataBaseEvent),wDataSize-sizeof(NTY_DataBaseEvent));

			return true;
		}
	case EVENT_TCP_SOCKET_READ:		//��ȡ�¼�
		{
			//��������
			NTY_TCPSocketReadEvent * pReadEvent=(NTY_TCPSocketReadEvent *)pData;

			//��С����
			ASSERT(wDataSize>=sizeof(NTY_TCPSocketReadEvent));
			ASSERT(wDataSize==(sizeof(NTY_TCPSocketReadEvent)+pReadEvent->wDataSize));

			//��СЧ��
			if (wDataSize<sizeof(NTY_TCPSocketReadEvent)) return false;
			if (wDataSize!=(sizeof(NTY_TCPSocketReadEvent)+pReadEvent->wDataSize)) return false;

			//������Ϣ
			m_pIAttemperEngineSink->OnEventTCPSocketRead(pReadEvent->dwServiceID,pReadEvent->Command, ((BYTE*)pData)+sizeof(NTY_TCPSocketReadEvent),pReadEvent->wDataSize);

			return true;
		}
	case EVENT_TCP_SOCKET_CLOSE:	//�ر��¼�
		{
			//��С����
			ASSERT(wDataSize==sizeof(NTY_TCPSocketShutEvent));
			if (wDataSize!=sizeof(NTY_TCPSocketCloseEvent)) return false;

			//������Ϣ
			NTY_TCPSocketCloseEvent* pCloseEvent=(NTY_TCPSocketCloseEvent*)pData;
			m_pIAttemperEngineSink->OnEventTCPSocketShut(pCloseEvent->dwServiceID,pCloseEvent->cbShutReason);

			return true;
		}
	case EVENT_TCP_SOCKET_CONNECT:	//�����¼�
		{
			//��С����
			ASSERT(wDataSize==sizeof(NTY_TCPSocketLinkEvent));
			if (wDataSize!=sizeof(NTY_TCPSocketConnectEvent)) return false;

			//������Ϣ
			NTY_TCPSocketConnectEvent * pConnectEvent=(NTY_TCPSocketConnectEvent*)pData;
			m_pIAttemperEngineSink->OnEventTCPSocketLink(pConnectEvent->dwServiceID,pConnectEvent->nErrorCode);

			return true;
		}
	case EVENT_TCP_NETWORK_ACCEPT:	//Ӧ���¼�
		{
			//��С����
			ASSERT(wDataSize==sizeof(NTY_TCPNetworkAcceptEvent));
			if (wDataSize!=sizeof(NTY_TCPNetworkAcceptEvent)) return false;

			//��������
			bool bSuccess=false;
			NTY_TCPNetworkAcceptEvent * pAcceptEvent=(NTY_TCPNetworkAcceptEvent *)pData;

			//������Ϣ
			try
			{ 
				bSuccess=m_pIAttemperEngineSink->OnEventTCPNetworkBind(pAcceptEvent->dwClientIP,pAcceptEvent->dwSocketID);
			}
			catch (...)	{ }

			//ʧ�ܴ���
			if (bSuccess==false) m_pITCPNetworkEngine->CloseSocket(pAcceptEvent->dwSocketID);

			return true;
		}
	case EVENT_TCP_NETWORK_READ:	//��ȡ�¼�
		{
			//Ч���С
			NTY_TCPNetworkReadEvent * pReadEvent=(NTY_TCPNetworkReadEvent *)pData;

			//��С����
			ASSERT(wDataSize>=sizeof(NTY_TCPNetworkReadEvent));
			ASSERT(wDataSize==(sizeof(NTY_TCPNetworkReadEvent)+pReadEvent->wDataSize));

			//��СЧ��
			if (wDataSize<sizeof(NTY_TCPNetworkReadEvent))
			{
				m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);
				return false;
			}

			//��СЧ��
			if (wDataSize!=(sizeof(NTY_TCPNetworkReadEvent)+pReadEvent->wDataSize))
			{
				m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);
				return false;
			}

			//������Ϣ
			bool bSuccess=false;
			try
			{ 
				bSuccess=m_pIAttemperEngineSink->OnEventTCPNetworkRead(pReadEvent->Command, ((BYTE*)pData)+sizeof(NTY_TCPNetworkReadEvent),pReadEvent->wDataSize,pReadEvent->dwSocketID);
			}
			catch (...)	{ }

			//ʧ�ܴ���
			if (bSuccess == false)
			{
				m_pITCPNetworkEngine->CloseSocket(pReadEvent->dwSocketID);
				TCHAR kTemp[256];
				_sntprintf(kTemp, CountArray(kTemp), TEXT("if (bSuccess == false) OnEventTCPNetworkRead %d %d"), pReadEvent->Command.wMainCmdID, pReadEvent->Command.wSubCmdID);
				g_TraceServiceManager.TraceString(kTemp, TraceLevel_Exception);
			}

			return true;
		}
	case EVENT_TCP_NETWORK_CLOSE:	//�ر��¼�
		{
			//��С����
			ASSERT(wDataSize==sizeof(NTY_TCPNetworkCloseEvent));
			if (wDataSize!=sizeof(NTY_TCPNetworkCloseEvent)) return false;

			//������Ϣ
			NTY_TCPNetworkCloseEvent * pCloseEvent=(NTY_TCPNetworkCloseEvent*)pData;
			m_pIAttemperEngineSink->OnEventTCPNetworkShut(pCloseEvent->dwClientIP,pCloseEvent->dwActiveTime,pCloseEvent->dwSocketID);

			return true;
		}
	}

	//�����¼�
	return m_pIAttemperEngineSink->OnEventAttemperData(wIdentifier,pData,wDataSize); 
}

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(AttemperEngine);

//////////////////////////////////////////////////////////////////////////
