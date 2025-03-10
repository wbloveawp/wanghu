#include "StdAfx.h"
#include "AsynchronismEngine.h"
#include "TraceServiceManager.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CAsynchronismThread::CAsynchronismThread()
{
	//运行变量
	m_hCompletionPort=NULL;
	m_pIAsyncEngineSink = NULL;
	//辅助变量
	ZeroMemory(m_cbBuffer, sizeof(m_cbBuffer));
}

//析构函数
CAsynchronismThread::~CAsynchronismThread()
{
}

void CAsynchronismThread::SetAsyncEngineSink(IAsyncEngineSink *pIAsynchronismEngineSink)
{
	m_pIAsyncEngineSink = pIAsynchronismEngineSink;
}

bool CAsynchronismThread::OnEventThreadStrat()
{
	//创建完成端口
	m_hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	
	return m_pIAsyncEngineSink->OnAsyncEngineStart();
 }

bool CAsynchronismThread::OnEventThreadConclude()
{
	//清理完成端口及关闭服务通知
	if (m_hCompletionPort != NULL)
	{
		CloseHandle(m_hCompletionPort);
		m_hCompletionPort = NULL;
	}

	return m_pIAsyncEngineSink->OnAsyncEngineConclude();
 }

//运行函数   --经IDA反编译对比  当前函数无任何问题 函数对比调用关系正确
bool CAsynchronismThread::OnEventThreadRun()
{
	//效验参数
	ASSERT(m_hCompletionPort!=NULL);
	if (m_hCompletionPort == NULL) return false;

	//变量定义
	DWORD dwThancferred=0;
	OVERLAPPED * pOverLapped=NULL;
	IAsyncEngine * pAsyncEngine=NULL;

	//完成端口
	if (GetQueuedCompletionStatus(m_hCompletionPort,&dwThancferred,(PULONG_PTR)&pAsyncEngine,&pOverLapped,INFINITE) 
		&& pAsyncEngine)
	{
		//队列锁定
		CThreadLock ThreadLock(pAsyncEngine->m_CriticalSection);
		//提取数据
		tagDataHead DataHead;
		pAsyncEngine->m_DataQueue.DistillData(DataHead, m_cbBuffer, sizeof(m_cbBuffer));
		//队列解锁
		ThreadLock.UnLock();
		//数据处理
		m_pIAsyncEngineSink->OnAsyncEngineData(DataHead.dwIdentifier, m_cbBuffer, DataHead.wDataSize);

		return true;
	}

	return false;
}


//////////////////////////////////////////////////////////////////////////

//构造函数
IAsyncEngine::IAsyncEngine()
{
}

//析构函数
IAsyncEngine::~IAsyncEngine()
{
	//终止服务
	ConcludeService();
}

//接口查询
VOID * IAsyncEngine::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServiceModule,Guid,dwQueryVer);
	QUERYINTERFACE(IAsyncEngine,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAsyncEngine,Guid,dwQueryVer);
	return NULL;
}

//队列负荷
bool IAsyncEngine::GetBurthenInfo(tagBurthenInfo & BurthenInfo)
{
	CThreadLock lock(m_CriticalSection);
	m_DataQueue.GetBurthenInfo(BurthenInfo);

	return true;
}

//设置模块
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

//异步数据
bool IAsyncEngine::PostAsynchronismData(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	//检测线程是否开启
	if (!m_AsynchronismThread.IsRuning()) return false;

	//向队列中添加数据
	CThreadLock ThreadLock(m_CriticalSection);
	if (!m_DataQueue.InsertData(wIdentifier,pData,wDataSize)) return false;
	PostQueuedCompletionStatus(m_AsynchronismThread.GetCompletionPort(), 0, (ULONG_PTR)this, NULL);

	return true;
}

//投递数据
bool IAsyncEngine::PostAsynchronismData(WORD wIdentifier, tagDataBuffer DataBuffer[], WORD wDataCount)
{
	//插入数据
	CThreadLock ThreadLock(m_CriticalSection);
	if (!m_DataQueue.InsertData(wIdentifier, DataBuffer, wDataCount)) return false;
	PostQueuedCompletionStatus(m_AsynchronismThread.GetCompletionPort(),0,(ULONG_PTR)this, NULL);
	return true;
}

//启动服务
bool IAsyncEngine::StartService()
{
	//线程是否在运行  如果没有在运行
	if (m_AsynchronismThread.IsRuning())
		return false;

	//清理队列
	m_DataQueue.RemoveData(false);
	//开始线程
	return m_AsynchronismThread.StartThread();

}

//停止服务
bool IAsyncEngine::ConcludeService()
{
	HANDLE hCompletionPort = m_AsynchronismThread.GetCompletionPort();
	// 结束线程
	if (hCompletionPort != NULL)
	{
		PostQueuedCompletionStatus(hCompletionPort, NULL, NULL, NULL);
	}

	//关闭线程
	if (m_AsynchronismThread.IsRuning())
	{
		m_AsynchronismThread.ConcludeThread(INFINITE);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(AsyncEngine);

//////////////////////////////////////////////////////////////////////////
