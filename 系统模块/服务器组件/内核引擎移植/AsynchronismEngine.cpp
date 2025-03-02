#include "StdAfx.h"
#include "AsynchronismEngine.h"
#include "TraceServiceManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////

//���캯��
CAsynchronismThread::CAsynchronismThread()
{
	//���б���
	m_hCompletionPort=NULL;
	m_pIAsynchronismEngineSink = NULL;
	//��������
	ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));
}

//��������
CAsynchronismThread::~CAsynchronismThread()
{
}

void CAsynchronismThread::SetAsynchronismEngineSink(IAsynchronismEngineSink *pIAsynchronismEngineSink)
{
	m_pIAsynchronismEngineSink = pIAsynchronismEngineSink;
}

bool CAsynchronismThread::OnEventThreadStrat()
{
	//������ɶ˿�
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	
	return m_pIAsynchronismEngineSink->OnAsynchronismEngineStart();
 }

bool CAsynchronismThread::OnEventThreadConclude()
{
	//������ɶ˿ڼ��رշ���֪ͨ
	if (m_hCompletionPort != NULL)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort = NULL;
	}

	return m_pIAsynchronismEngineSink->OnAsynchronismEngineConclude();

 }

//���к���   --��IDA������Ա�  ��ǰ�������κ����� �����Աȵ��ù�ϵ��ȷ
bool CAsynchronismThread::OnEventThreadRun()
{
	//Ч�����
	ASSERT(m_hCompletionPort!=NULL);
	if (m_hCompletionPort == NULL) return false;

	//��������
	DWORD dwThancferred=0;
	OVERLAPPED * pOverLapped=NULL;
	CAsynchronismEngine * pAsynchronismEngine=NULL;

	//��ɶ˿�
	if (GetQueuedCompletionStatus(m_hCompletionPort,&dwThancferred,(PULONG_PTR)&pAsynchronismEngine,&pOverLapped,INFINITE) 
		&& pAsynchronismEngine)
	{
		//��������
		CWHDataLocker ThreadLock(pAsynchronismEngine->m_CriticalSection);
		//��ȡ����
		tagDataHead DataHead;
		pAsynchronismEngine->m_DataQueue.DistillData(DataHead, m_cbBuffer, sizeof(m_cbBuffer));
		//���н���
		ThreadLock.UnLock();
		//���ݴ���
		m_pIAsynchronismEngineSink->OnAsynchronismEngineData(DataHead.wIdentifier, m_cbBuffer, DataHead.wDataSize);

		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////

//���캯��
CAsynchronismEngine::CAsynchronismEngine()
{
}

//��������
CAsynchronismEngine::~CAsynchronismEngine()
{
	//��ֹ����
	ConcludeService();
}

//�ӿڲ�ѯ
VOID * CAsynchronismEngine::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServiceModule,Guid,dwQueryVer);
	QUERYINTERFACE(IAsynchronismEngine,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAsynchronismEngine,Guid,dwQueryVer);
	return NULL;
}

//���и���
bool CAsynchronismEngine::GetBurthenInfo(tagBurthenInfo & BurthenInfo)
{
	CWHDataLocker lock(m_CriticalSection);
	m_DataQueue.GetBurthenInfo(BurthenInfo);

	return true;
}
//����ģ��
bool CAsynchronismEngine::SetAsynchronismSink(IUnknownEx * pIUnknownEx)
{
	if (pIUnknownEx)
	{
		m_AsynchronismThread.SetAsynchronismEngineSink(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IAsynchronismEngineSink));
	}
	else
	{
		m_AsynchronismThread.SetAsynchronismEngineSink(NULL);
	}

	return true;
}

//�첽����
bool CAsynchronismEngine::PostAsynchronismData(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	//����߳��Ƿ���
	if (!m_AsynchronismThread.IsRuning()) return false;

	//��������������
	CWHDataLocker ThreadLock(m_CriticalSection);
	if (!m_DataQueue.InsertData(wIdentifier,pData,wDataSize)) return false;
	PostQueuedCompletionStatus(m_AsynchronismThread.GetCompletionPort(), 0, (ULONG_PTR)this, NULL);

	return true;
}

//Ͷ������
bool CAsynchronismEngine::PostAsynchronismData(WORD wIdentifier, tagDataBuffer DataBuffer[], WORD wDataCount)
{
	//��������
	CWHDataLocker ThreadLock(m_CriticalSection);
	if (!m_DataQueue.InsertData(wIdentifier, DataBuffer, wDataCount)) return false;
	PostQueuedCompletionStatus(m_AsynchronismThread.GetCompletionPort(),0,(ULONG_PTR)this, NULL);
	return true;
}

//��������
bool CAsynchronismEngine::StartService()
{
	//�߳��Ƿ�������  ���û��������
	if (m_AsynchronismThread.IsRuning())
		return false;

	//�������
	m_DataQueue.RemoveData(false);
	//��ʼ�߳�
	return m_AsynchronismThread.StartThread();

}

//ֹͣ����
bool CAsynchronismEngine::ConcludeService()
{
	HANDLE hCompletionPort = m_AsynchronismThread.GetCompletionPort();
	// �����߳�
	if (hCompletionPort != NULL)
	{
		PostQueuedCompletionStatus(hCompletionPort, NULL, NULL, NULL);
	}

	//�ر��߳�
	if (m_AsynchronismThread.IsRuning())
	{
		m_AsynchronismThread.ConcludeThread(INFINITE);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(AsynchronismEngine);

//////////////////////////////////////////////////////////////////////////
