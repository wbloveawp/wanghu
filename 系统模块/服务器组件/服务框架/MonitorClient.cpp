#include "StdAfx.h"
#include "MonitorClient.h"

//////////////////////////////////////////////////////////////////////////
// ��������
#define UI_DATABASE_EXCEPTION			1					//���ݿ��쳣	
 
#define	IDI_WINDOW_DETECTION			1					//���ڼ��

//////////////////////////////////////////////////////////////////////////
//��̬����
CMonitorClient* CMonitorClient::g_pMonitorClient = NULL;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMonitorClient, CWnd)
	ON_MESSAGE(WM_UICONTROL_REQUEST, OnUIControlRequest)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CMonitorClient::CMonitorClient()
{
	//״̬����
	m_bServiceStatus = false;

	//���ñ���
	m_hWndServer = NULL;
	m_pIMonitorClientSink = NULL;

	//�������
	m_wServiceID = NONE_SERVICE_ID;
	m_wServiceModule = 0;
	m_wServiceStatus = 0;
	m_szServiceName[0] = 0;

	//��ʼ��
	m_MapExceptionDigest.InitHashTable(13);

	//����ָ��
	g_pMonitorClient = this;

	return;
}

//��������
CMonitorClient::~CMonitorClient()
{
	if (g_pMonitorClient == this)
	{
		g_pMonitorClient = NULL;
	}

	//�Ƴ�Ԫ��
	m_MapExceptionDigest.RemoveAll();

	//�ر�֪ͨ
	if (IsWindow(m_hWndServer))
	{
		//��������
		CMD_MS_ProcessClose ProcessClose;
		ZeroMemory(&ProcessClose, sizeof(ProcessClose));

		//����ṹ
		ProcessClose.dwProcessID = GetCurrentProcessId();

		//������Ϣ
		m_ProcessTrade->SendData(m_hWndServer, IPC_CMD_MS_MONITOR, IPC_SUB_MS_PROCESS_CLOSE, &ProcessClose, sizeof(ProcessClose));
	}

	//�رսӿ�
	if (m_ProcessTrade.GetInterface() != NULL) m_ProcessTrade.CloseInstance();
}

//�ӿڲ�ѯ
VOID* CMonitorClient::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IProcessTradeSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IProcessTradeSink, Guid, dwQueryVer);
	return NULL;
}

//��������
bool CMonitorClient::StartService()
{
	//У��״̬
	ASSERT (m_bServiceStatus == false);
	if (m_bServiceStatus == true) return false;

	//У��ӿ�
	if (m_pIMonitorClientSink == NULL)
	{
		return false;
	}

	//У�����
	if (m_wServiceID == NONE_SERVICE_ID)
	{
		return false;
	}

	//����״̬
	m_bServiceStatus = true;

	//������ʱ��
	SetTimer(IDI_WINDOW_DETECTION, 2000, NULL);	

	return true;
}

//ֹͣ����
bool CMonitorClient::ConcludeService()
{	
	//У��״̬
	if (m_bServiceStatus == true)
	{
		//����״̬
		m_bServiceStatus = false;

		//�Ƴ�Ԫ��
		m_MapExceptionDigest.RemoveAll();

		//�رն�ʱ��
		KillTimer(IDI_WINDOW_DETECTION);
	}

	return true;
}

//��������
bool CMonitorClient::OnProcessTradeData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize, HWND hWndSend)
{
	switch (wMainCmdID)
	{
	case IPC_CMD_MS_MONITOR:			//�������
	{
		return OnEventProcessMonitor(wSubCmdID,pData,wDataSize,hWndSend);
	}
	}

	return true;
}

//����״̬
VOID CMonitorClient::UpdateServiceStatus(WORD wServiceStatus)
{
	if (wServiceStatus != m_wServiceStatus)
	{
		//����״̬
		m_wServiceStatus = wServiceStatus;

		//У�鴰��
		if (!IsWindow(m_hWndServer))
		{
			//��ʼ��
			InitializeClient();
		}
		else
		{
			//��������
			CMD_MS_ServiceStatus ServiceStatus;
			ZeroMemory(&ServiceStatus, sizeof(ServiceStatus));

			//����ṹ
			ServiceStatus.dwProcessID = GetCurrentProcessId();
			ServiceStatus.wServiceStatus = m_wServiceStatus;			

			//������Ϣ
			m_ProcessTrade->SendData(m_hWndServer, IPC_CMD_MS_MONITOR, IPC_SUB_MS_SERVICE_STATUS, &ServiceStatus, sizeof(ServiceStatus));
		}
	}	
}

//���ýӿ�
VOID CMonitorClient::SetMonitorClientSink(IMonitorClientSink* pIMonitorClientSink)
{
	m_pIMonitorClientSink = pIMonitorClientSink;
}

//���ò���
VOID CMonitorClient::SetServiceParameter(WORD wServiceModule, WORD wServiceID, LPCTSTR pszServiceName)
{
	//���ò���
	m_wServiceID = wServiceID;
	m_wServiceModule = wServiceModule;
	StringCchCopy(m_szServiceName,CountArray(m_szServiceName), pszServiceName);

	//У����
	if (!IsWindow(m_hWndServer))
	{
		//��ʼ��
		InitializeClient();
	}	
	else
	{
		//��������
		CMD_MS_ServiceReady ServiceReady;
		ZeroMemory(&ServiceReady, sizeof(ServiceReady));

		//����ṹ
		ServiceReady.wServiceID = m_wServiceID;
		ServiceReady.dwProcessID = GetCurrentProcessId();
		ServiceReady.wServiceModule = m_wServiceModule;
		ServiceReady.wServiceStatus = m_wServiceStatus;
		StringCchCopy(ServiceReady.szServiceName, CountArray(ServiceReady.szServiceName), m_szServiceName);

		//������Ϣ
		m_ProcessTrade->SendData(m_hWndServer, IPC_CMD_MS_MONITOR, IPC_SUB_MS_SERVICE_READY, &ServiceReady, sizeof(ServiceReady));
	}
}

//���̼��
bool CMonitorClient::OnEventProcessMonitor(WORD wSubCmdID, VOID* pData, WORD wDataSize, HWND hWndSend)
{
	switch (wSubCmdID)
	{
	case IPC_SUB_MS_STOP_SERVICE:	//ֹͣ����
	{
		//ֹͣ����		
		if (m_pIMonitorClientSink != NULL)
		{
			m_pIMonitorClientSink->ConcludeService();
		}		

		return true;
	}
	case IPC_SUB_MS_START_SERVICE:	//��������
	{
		//��������
		if (m_pIMonitorClientSink != NULL)
		{
			m_pIMonitorClientSink->StartService();
		}

		return true;
	}
	case IPC_SUB_MS_CLOSE_PROCESS:	//�رս���
	{
		//ֹͣ����
		if (m_pIMonitorClientSink != NULL)
		{
			m_pIMonitorClientSink->ConcludeService();
		}

		//�˳�����
		exit(0);

		return true;
	}
	}

	return true;
}

//��ʼ���ն�
bool CMonitorClient::InitializeClient()
{
	//��������
	if (m_hWnd == NULL)
	{
		//��������
		CRect rcCreate(0, 0, 0, 0);
		Create(NULL, NULL, WS_CHILD, rcCreate, AfxGetMainWnd(), 100);

		//���þ��
		m_UIControlSink.SetHookWindow(m_hWnd);
	}

	//�����ӿ�
	if ((m_ProcessTrade.GetInterface() == NULL) && (m_ProcessTrade.CreateInstance() == false))
	{
		return false;
	}

	//���ûص�
	if (m_ProcessTrade->SetProcessTradeSink(QUERY_ME_INTERFACE(IProcessTradeSink)) == false)
	{
		return false;
	}

	//��������	
	if (m_ProcessTrade->GetTradeKernelWnd()==NULL && m_ProcessTrade->CreateProcessTrade(NULL) == false)
	{
		return false;
	}

	//���Ҵ���
	m_hWndServer = ::FindWindow(MONITOR_CLASS_NAME, NULL);
	if (m_hWndServer == NULL)
	{
		return false;
	}

	//ֹͣ״̬
	if (m_wServiceStatus == SERVICE_STATUS_STOP && m_wServiceID== NONE_SERVICE_ID)
	{
		//��������
		CMD_MS_ProcessStart ProcessStart;
		ZeroMemory(&ProcessStart, sizeof(ProcessStart));

		//����ṹ
		ProcessStart.dwProcessID = GetCurrentProcessId();		

		//������Ϣ
		m_ProcessTrade->SendData(m_hWndServer, IPC_CMD_MS_MONITOR, IPC_SUB_MS_PROCESS_START, &ProcessStart, sizeof(ProcessStart));
	}
	else
	{
		//��������
		CMD_MS_ServiceReady ServiceReady;
		ZeroMemory(&ServiceReady, sizeof(ServiceReady));

		//����ṹ
		ServiceReady.wServiceID = m_wServiceID;
		ServiceReady.dwProcessID = GetCurrentProcessId();
		ServiceReady.wServiceModule = m_wServiceModule;
		ServiceReady.wServiceStatus = m_wServiceStatus;
		StringCchCopy(ServiceReady.szServiceName, CountArray(ServiceReady.szServiceName), m_szServiceName);

		//������Ϣ
		m_ProcessTrade->SendData(m_hWndServer, IPC_CMD_MS_MONITOR, IPC_SUB_MS_SERVICE_READY, &ServiceReady, sizeof(ServiceReady));
	}

	return true;
}

//������Ϣ
LRESULT CMonitorClient::OnUIControlRequest(WPARAM wParam, LPARAM lParam)
{
	//��ȡ����
	tagDataHead DataHead;
	VOID* pData = m_UIControlSink.GetControlRequest(DataHead);

	//���ݿ��쳣
	if (DataHead.dwIdentifier == UI_DATABASE_EXCEPTION)
	{
		//����ṹ
		CMD_MS_DataBaseException DataBaseException;
		ZeroMemory(&DataBaseException, sizeof(DataBaseException));

		//���ñ���
		DataBaseException.dwProcessID = GetCurrentProcessId();
		StringCchCopy(DataBaseException.szExceptionInfo,CountArray(DataBaseException.szExceptionInfo), (LPCTSTR)pData);

		//����ժҪ
		TCHAR szExceptionDigest[LEN_MD5];
		CMD5Encrypt::EncryptData(DataBaseException.szExceptionInfo, szExceptionDigest);

		//����ժҪ
		bool bExists = false;
		if (!m_MapExceptionDigest.Lookup(szExceptionDigest, bExists))
		{
			//���ժҪ
			m_MapExceptionDigest[szExceptionDigest] = true;

			//������Ϣ
			WORD wStringSize = CountStringBuffer(DataBaseException.szExceptionInfo);
			WORD wHeadSize = sizeof(DataBaseException) - sizeof(DataBaseException.szExceptionInfo);
			m_ProcessTrade->SendData(m_hWndServer, IPC_CMD_MS_MONITOR, IPC_SUB_MS_DATABASE_EXCEPTION, &DataBaseException, wHeadSize + wStringSize);
		}		
	}

	return TRUE;
}

//�쳣����
VOID CMonitorClient::DBExceptionHookFun(enSQLException SQLException, CComError* pComError)
{
	//��ȡ����
	auto pMonitorClient = CMonitorClient::g_pMonitorClient;	

	//�쳣��Ϣ
	TCHAR szExceptionInfo[256];	
	_sntprintf_s(szExceptionInfo, CountArray(szExceptionInfo), TEXT("���ݿ�%s:%s [ 0x%8x ]"), SQLException == SQLException_Connect ? TEXT("�����쳣") : TEXT("�﷨�쳣"), (LPCTSTR)(pComError->Description()), pComError->Error());

	//Ͷ������
	pMonitorClient->m_UIControlSink.PostControlRequest(UI_DATABASE_EXCEPTION,szExceptionInfo, CountStringBuffer(szExceptionInfo));
}

//��ʱ����Ϣ
void CMonitorClient::OnTimer(UINT_PTR nIDEvent)
{
	//���ڼ��
	if (nIDEvent == IDI_WINDOW_DETECTION)
	{
		//У�鴰��
		if (!IsWindow(m_hWndServer))
		{
			//��ʼ��
			InitializeClient();
		}
	}
	__super::OnTimer(nIDEvent);
}


//////////////////////////////////////////////////////////////////////////
