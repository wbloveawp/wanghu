#include "StdAfx.h"
#include "AsynchronismEngine.h"
#include "TraceServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CAsynchronismThread::CAsynchronismThread()
{
	//���б���
	m_hCompletionPort=NULL;
	m_pIAsyncEngineSink = NULL;
	//��������
	ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));
}

//��������
CAsynchronismThread::~CAsynchronismThread()
{
}

void CAsynchronismThread::SetAsyncEngineSink(IAsyncEngineSink *pIAsynchronismEngineSink)
{
	m_pIAsyncEngineSink = pIAsynchronismEngineSink;
}

bool CAsynchronismThread::OnEventThreadStrat()
{
	//������ɶ˿�
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	
	return m_pIAsyncEngineSink->OnAsyncEngineStart();
 }

bool CAsynchronismThread::OnEventThreadConclude()
{
	//������ɶ˿ڼ��رշ���֪ͨ
	if (m_hCompletionPort != NULL)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort = NULL;
	}

	return m_pIAsyncEngineSink->OnAsyncEngineConclude();
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
	IAsyncEngine * pAsyncEngine=NULL;

	//��ɶ˿�
	if (GetQueuedCompletionStatus(m_hCompletionPort,&dwThancferred,(PULONG_PTR)&pAsyncEngine,&pOverLapped,INFINITE) 
		&& pAsyncEngine)
	{
		//��������
		CThreadLock ThreadLock(pAsyncEngine->m_CriticalSection);
		//��ȡ����
		tagDataHead DataHead;
		pAsyncEngine->m_DataQueue.DistillData(DataHead, m_cbBuffer, sizeof(m_cbBuffer));
		//���н���
		ThreadLock.UnLock();
		//���ݴ���
		m_pIAsyncEngineSink->OnAsyncEngineData(DataHead.dwIdentifier, m_cbBuffer, DataHead.wDataSize);

		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////

//���캯��
IAsyncEngine::IAsyncEngine()
{
}

//��������
IAsyncEngine::~IAsyncEngine()
{
	//��ֹ����
	ConcludeService();
}

//�ӿڲ�ѯ
VOID * IAsyncEngine::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServiceModule,Guid,dwQueryVer);
	QUERYINTERFACE(IAsyncEngine,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAsyncEngine,Guid,dwQueryVer);
	return NULL;
}

//���и���
bool IAsyncEngine::GetBurthenInfo(tagBurthenInfo & BurthenInfo)
{
	CThreadLock lock(m_CriticalSection);
	m_DataQueue.GetBurthenInfo(BurthenInfo);

	return true;
}

//����ģ��
bool IAsyncEngine::SetAsynchronismSink(IUnknownEx * pIUnknownEx)
{
	if (pIUnknownEx)
	{
		m_AsynchronismThread.SetAsyncEngineSink(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IAsyncEngineSink));
	}
	else
	{
		m_AsynchronismThread.SetAsyncEngineSink(NULL);
	}

	return true;
}

//�첽����
bool IAsyncEngine::PostAsynchronismData(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	//����߳��Ƿ���
	if (!m_AsynchronismThread.IsRuning()) return false;

	//��������������
	CThreadLock ThreadLock(m_CriticalSection);
	if (!m_DataQueue.InsertData(wIdentifier,pData,wDataSize)) return false;
	PostQueuedCompletionStatus(m_AsynchronismThread.GetCompletionPort(), 0, (ULONG_PTR)this, NULL);

	return true;
}

//Ͷ������
bool IAsyncEngine::PostAsynchronismData(WORD wIdentifier, tagDataBuffer DataBuffer[], WORD wDataCount)
{
	//��������
	CThreadLock ThreadLock(m_CriticalSection);
	if (!m_DataQueue.InsertData(wIdentifier, DataBuffer, wDataCount)) return false;
	PostQueuedCompletionStatus(m_AsynchronismThread.GetCompletionPort(),0,(ULONG_PTR)this, NULL);
	return true;
}

//��������
bool IAsyncEngine::StartService()
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
bool IAsyncEngine::ConcludeService()
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
DECLARE_CREATE_MODULE(AsyncEngine);

//////////////////////////////////////////////////////////////////////////
