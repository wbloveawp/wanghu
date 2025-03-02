#include "StdAfx.h"
#include "MonitorClient.h"

//////////////////////////////////////////////////////////////////////////
// 常量定义
#define UI_DATABASE_EXCEPTION			1					//数据库异常	
 
#define	IDI_WINDOW_DETECTION			1					//窗口检测

//////////////////////////////////////////////////////////////////////////
//静态变量
CMonitorClient* CMonitorClient::g_pMonitorClient = NULL;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMonitorClient, CWnd)
	ON_MESSAGE(WM_UICONTROL_REQUEST, OnUIControlRequest)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CMonitorClient::CMonitorClient()
{
	//状态变量
	m_bServiceStatus = false;

	//设置变量
	m_hWndServer = NULL;
	m_pIMonitorClientSink = NULL;

	//服务参数
	m_wServiceID = NONE_SERVICE_ID;
	m_wServiceModule = 0;
	m_wServiceStatus = 0;
	m_szServiceName[0] = 0;

	//初始化
	m_MapExceptionDigest.InitHashTable(13);

	//设置指针
	g_pMonitorClient = this;

	return;
}

//析构函数
CMonitorClient::~CMonitorClient()
{
	if (g_pMonitorClient == this)
	{
		g_pMonitorClient = NULL;
	}

	//移除元素
	m_MapExceptionDigest.RemoveAll();

	//关闭通知
	if (IsWindow(m_hWndServer))
	{
		//变量定义
		CMD_MS_ProcessClose ProcessClose;
		ZeroMemory(&ProcessClose, sizeof(ProcessClose));

		//构造结构
		ProcessClose.dwProcessID = GetCurrentProcessId();

		//发送消息
		m_ProcessTrade->SendData(m_hWndServer, IPC_CMD_MS_MONITOR, IPC_SUB_MS_PROCESS_CLOSE, &ProcessClose, sizeof(ProcessClose));
	}

	//关闭接口
	if (m_ProcessTrade.GetInterface() != NULL) m_ProcessTrade.CloseInstance();
}

//接口查询
VOID* CMonitorClient::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IProcessTradeSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IProcessTradeSink, Guid, dwQueryVer);
	return NULL;
}

//启动服务
bool CMonitorClient::StartService()
{
	//校验状态
	ASSERT (m_bServiceStatus == false);
	if (m_bServiceStatus == true) return false;

	//校验接口
	if (m_pIMonitorClientSink == NULL)
	{
		return false;
	}

	//校验参数
	if (m_wServiceID == NONE_SERVICE_ID)
	{
		return false;
	}

	//设置状态
	m_bServiceStatus = true;

	//启动定时器
	SetTimer(IDI_WINDOW_DETECTION, 2000, NULL);	

	return true;
}

//停止服务
bool CMonitorClient::ConcludeService()
{	
	//校验状态
	if (m_bServiceStatus == true)
	{
		//设置状态
		m_bServiceStatus = false;

		//移除元素
		m_MapExceptionDigest.RemoveAll();

		//关闭定时器
		KillTimer(IDI_WINDOW_DETECTION);
	}

	return true;
}

//进程数据
bool CMonitorClient::OnProcessTradeData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize, HWND hWndSend)
{
	switch (wMainCmdID)
	{
	case IPC_CMD_MS_MONITOR:			//监控命令
	{
		return OnEventProcessMonitor(wSubCmdID,pData,wDataSize,hWndSend);
	}
	}

	return true;
}

//更新状态
VOID CMonitorClient::UpdateServiceStatus(WORD wServiceStatus)
{
	if (wServiceStatus != m_wServiceStatus)
	{
		//设置状态
		m_wServiceStatus = wServiceStatus;

		//校验窗口
		if (!IsWindow(m_hWndServer))
		{
			//初始化
			InitializeClient();
		}
		else
		{
			//变量定义
			CMD_MS_ServiceStatus ServiceStatus;
			ZeroMemory(&ServiceStatus, sizeof(ServiceStatus));

			//构造结构
			ServiceStatus.dwProcessID = GetCurrentProcessId();
			ServiceStatus.wServiceStatus = m_wServiceStatus;			

			//发送消息
			m_ProcessTrade->SendData(m_hWndServer, IPC_CMD_MS_MONITOR, IPC_SUB_MS_SERVICE_STATUS, &ServiceStatus, sizeof(ServiceStatus));
		}
	}	
}

//设置接口
VOID CMonitorClient::SetMonitorClientSink(IMonitorClientSink* pIMonitorClientSink)
{
	m_pIMonitorClientSink = pIMonitorClientSink;
}

//设置参数
VOID CMonitorClient::SetServiceParameter(WORD wServiceModule, WORD wServiceID, LPCTSTR pszServiceName)
{
	//设置参数
	m_wServiceID = wServiceID;
	m_wServiceModule = wServiceModule;
	StringCchCopy(m_szServiceName,CountArray(m_szServiceName), pszServiceName);

	//校验结果
	if (!IsWindow(m_hWndServer))
	{
		//初始化
		InitializeClient();
	}	
	else
	{
		//变量定义
		CMD_MS_ServiceReady ServiceReady;
		ZeroMemory(&ServiceReady, sizeof(ServiceReady));

		//构造结构
		ServiceReady.wServiceID = m_wServiceID;
		ServiceReady.dwProcessID = GetCurrentProcessId();
		ServiceReady.wServiceModule = m_wServiceModule;
		ServiceReady.wServiceStatus = m_wServiceStatus;
		StringCchCopy(ServiceReady.szServiceName, CountArray(ServiceReady.szServiceName), m_szServiceName);

		//发送消息
		m_ProcessTrade->SendData(m_hWndServer, IPC_CMD_MS_MONITOR, IPC_SUB_MS_SERVICE_READY, &ServiceReady, sizeof(ServiceReady));
	}
}

//进程监控
bool CMonitorClient::OnEventProcessMonitor(WORD wSubCmdID, VOID* pData, WORD wDataSize, HWND hWndSend)
{
	switch (wSubCmdID)
	{
	case IPC_SUB_MS_STOP_SERVICE:	//停止服务
	{
		//停止服务		
		if (m_pIMonitorClientSink != NULL)
		{
			m_pIMonitorClientSink->ConcludeService();
		}		

		return true;
	}
	case IPC_SUB_MS_START_SERVICE:	//启动服务
	{
		//启动服务
		if (m_pIMonitorClientSink != NULL)
		{
			m_pIMonitorClientSink->StartService();
		}

		return true;
	}
	case IPC_SUB_MS_CLOSE_PROCESS:	//关闭进程
	{
		//停止服务
		if (m_pIMonitorClientSink != NULL)
		{
			m_pIMonitorClientSink->ConcludeService();
		}

		//退出进程
		exit(0);

		return true;
	}
	}

	return true;
}

//初始化终端
bool CMonitorClient::InitializeClient()
{
	//创建窗口
	if (m_hWnd == NULL)
	{
		//创建窗口
		CRect rcCreate(0, 0, 0, 0);
		Create(NULL, NULL, WS_CHILD, rcCreate, AfxGetMainWnd(), 100);

		//设置句柄
		m_UIControlSink.SetHookWindow(m_hWnd);
	}

	//创建接口
	if ((m_ProcessTrade.GetInterface() == NULL) && (m_ProcessTrade.CreateInstance() == false))
	{
		return false;
	}

	//设置回调
	if (m_ProcessTrade->SetProcessTradeSink(QUERY_ME_INTERFACE(IProcessTradeSink)) == false)
	{
		return false;
	}

	//创建对象	
	if (m_ProcessTrade->GetTradeKernelWnd()==NULL && m_ProcessTrade->CreateProcessTrade(NULL) == false)
	{
		return false;
	}

	//查找窗口
	m_hWndServer = ::FindWindow(MONITOR_CLASS_NAME, NULL);
	if (m_hWndServer == NULL)
	{
		return false;
	}

	//停止状态
	if (m_wServiceStatus == SERVICE_STATUS_STOP && m_wServiceID== NONE_SERVICE_ID)
	{
		//变量定义
		CMD_MS_ProcessStart ProcessStart;
		ZeroMemory(&ProcessStart, sizeof(ProcessStart));

		//构造结构
		ProcessStart.dwProcessID = GetCurrentProcessId();		

		//发送消息
		m_ProcessTrade->SendData(m_hWndServer, IPC_CMD_MS_MONITOR, IPC_SUB_MS_PROCESS_START, &ProcessStart, sizeof(ProcessStart));
	}
	else
	{
		//变量定义
		CMD_MS_ServiceReady ServiceReady;
		ZeroMemory(&ServiceReady, sizeof(ServiceReady));

		//构造结构
		ServiceReady.wServiceID = m_wServiceID;
		ServiceReady.dwProcessID = GetCurrentProcessId();
		ServiceReady.wServiceModule = m_wServiceModule;
		ServiceReady.wServiceStatus = m_wServiceStatus;
		StringCchCopy(ServiceReady.szServiceName, CountArray(ServiceReady.szServiceName), m_szServiceName);

		//发送消息
		m_ProcessTrade->SendData(m_hWndServer, IPC_CMD_MS_MONITOR, IPC_SUB_MS_SERVICE_READY, &ServiceReady, sizeof(ServiceReady));
	}

	return true;
}

//控制消息
LRESULT CMonitorClient::OnUIControlRequest(WPARAM wParam, LPARAM lParam)
{
	//获取数据
	tagDataHead DataHead;
	VOID* pData = m_UIControlSink.GetControlRequest(DataHead);

	//数据库异常
	if (DataHead.dwIdentifier == UI_DATABASE_EXCEPTION)
	{
		//构造结构
		CMD_MS_DataBaseException DataBaseException;
		ZeroMemory(&DataBaseException, sizeof(DataBaseException));

		//设置变量
		DataBaseException.dwProcessID = GetCurrentProcessId();
		StringCchCopy(DataBaseException.szExceptionInfo,CountArray(DataBaseException.szExceptionInfo), (LPCTSTR)pData);

		//计算摘要
		TCHAR szExceptionDigest[LEN_MD5];
		CMD5Encrypt::EncryptData(DataBaseException.szExceptionInfo, szExceptionDigest);

		//查找摘要
		bool bExists = false;
		if (!m_MapExceptionDigest.Lookup(szExceptionDigest, bExists))
		{
			//添加摘要
			m_MapExceptionDigest[szExceptionDigest] = true;

			//发送消息
			WORD wStringSize = CountStringBuffer(DataBaseException.szExceptionInfo);
			WORD wHeadSize = sizeof(DataBaseException) - sizeof(DataBaseException.szExceptionInfo);
			m_ProcessTrade->SendData(m_hWndServer, IPC_CMD_MS_MONITOR, IPC_SUB_MS_DATABASE_EXCEPTION, &DataBaseException, wHeadSize + wStringSize);
		}		
	}

	return TRUE;
}

//异常钩子
VOID CMonitorClient::DBExceptionHookFun(enSQLException SQLException, CComError* pComError)
{
	//获取对象
	auto pMonitorClient = CMonitorClient::g_pMonitorClient;	

	//异常信息
	TCHAR szExceptionInfo[256];	
	_sntprintf_s(szExceptionInfo, CountArray(szExceptionInfo), TEXT("数据库%s:%s [ 0x%8x ]"), SQLException == SQLException_Connect ? TEXT("连接异常") : TEXT("语法异常"), (LPCTSTR)(pComError->Description()), pComError->Error());

	//投递请求
	pMonitorClient->m_UIControlSink.PostControlRequest(UI_DATABASE_EXCEPTION,szExceptionInfo, CountStringBuffer(szExceptionInfo));
}

//定时器消息
void CMonitorClient::OnTimer(UINT_PTR nIDEvent)
{
	//窗口检测
	if (nIDEvent == IDI_WINDOW_DETECTION)
	{
		//校验窗口
		if (!IsWindow(m_hWndServer))
		{
			//初始化
			InitializeClient();
		}
	}
	__super::OnTimer(nIDEvent);
}


//////////////////////////////////////////////////////////////////////////
