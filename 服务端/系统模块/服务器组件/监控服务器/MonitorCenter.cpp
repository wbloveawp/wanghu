#include "StdAfx.h"
#include "MonitorCenter.h"
#include "ControlPacket.h"
#include "ProcessHelper.h"

//////////////////////////////////////////////////////////////////////////
//ʱ�Ӷ���
#define					IDI_PROCESS_DETECT		1					//���̼��
#define					TIME_PROCESS_DETECT		2000				//���ʱ��

//��������
#define					CPU_TEST_TIME_COUNT		20					//����ʱ��	
#define					MEM_TEST_INC_MULTIPLE	10					//��������		

//////////////////////////////////////////////////////////////////////////

//��̬����
CMonitorCenter *	CMonitorCenter::g_pMonitorCenter=NULL;			//����ָ��

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMonitorCenter, CWnd)
	ON_WM_TIMER()
	ON_MESSAGE(WM_UICONTROL_REQUEST,OnUIControlRequest)	
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CMonitorCenter::CMonitorCenter()
{
	//���ñ���
	m_bServiceStatus = false;
	m_pServiceOption = NULL;
	m_pIAttemperEngine = NULL;	

	//��ʼ��
	m_StockCrashInfo.InitHashTable(13);
	m_StockProcessItem.InitHashTable(13);

	//ϵͳ��Ϣ
	GetSystemInfo(&m_SystemInfo);

	//���ö���
	ASSERT(g_pMonitorCenter ==NULL);
	if (g_pMonitorCenter ==NULL) g_pMonitorCenter =this;

	return;
}

//��������
CMonitorCenter::~CMonitorCenter()
{
	//ɾ������
	m_StockProcessItem.DeleteAllItem();
}

//�ӿڲ�ѯ
VOID* CMonitorCenter::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IProcessTradeSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IProcessTradeSink, Guid, dwQueryVer);
	return NULL;
}

//��������
bool CMonitorCenter::StartService()
{
	//У��״̬
	ASSERT (m_bServiceStatus ==false);
	if (m_bServiceStatus == true) return true;

	//��������
	if (m_hWnd==NULL)
	{
		//��������
		CRect rcCreate(0,0,0,0);
		Create(NULL,NULL,WS_CHILD,rcCreate,AfxGetMainWnd(),200);

		//���ýӿ�
		m_UIControlSink.SetHookWindow(m_hWnd);
	}
	
	//�����ӿ�
	if (!m_ProcessTrade.GetInterface() && !m_ProcessTrade.CreateInstance())
	{
		return false;
	}

	//���ûص�
	if (!m_ProcessTrade->SetProcessTradeSink(QUERY_ME_INTERFACE(IProcessTradeSink)))
	{
		return false;
	}

	//��������	
	if (!m_ProcessTrade->CreateProcessTrade(MONITOR_CLASS_NAME))
	{
		return false;
	}

	//���ñ�־
	m_bServiceStatus = true;

	//������ʱ��
	SetTimer(IDI_PROCESS_DETECT, TIME_PROCESS_DETECT, NULL);

	return true;
}

//ֹͣ����
bool CMonitorCenter::ConcludeService()
{
	//У��״̬
	if (m_bServiceStatus == true)
	{
		//����״̬
		m_bServiceStatus = false;

		//�رն�ʱ��
		KillTimer(IDI_PROCESS_DETECT);

		//�Ƴ�����
		m_StockCrashInfo.RemoveAllItem();
		m_StockProcessItem.RemoveAllItem();

		//�رս���
		if (m_ProcessTrade.GetInterface() != NULL) m_ProcessTrade.CloseInstance();

		//���ٴ���
		if (m_hWnd) DestroyWindow();
	}	
	
	return true;
}

//��������
bool CMonitorCenter::OnProcessTradeData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize, HWND hWndSend)
{
	CTraceService::TraceString(TEXT("MainCmdID=%d,SubCmdID=%d"), TraceLevel_Debug, wMainCmdID, wSubCmdID);

	switch (wMainCmdID)
	{
	case IPC_CMD_MS_MONITOR:			//�������
	{
		return OnEventProcessMonitor(wSubCmdID,pData,wDataSize,hWndSend);
	}
	}

	return true;
}

//��������
VOID CMonitorCenter::SetServerName(LPCTSTR pszServerName)
{
	StringCchCopy(m_szServerName,CountArray(m_szServerName),pszServerName);
}

//�������
VOID CMonitorCenter::SetServiceOption(tagServiceOption* pServiceOption)
{
	m_pServiceOption = pServiceOption;
}

//���ýӿ�
VOID CMonitorCenter::SetAttemperEngine(IAttemperEngine* pIAttemperEngine)
{
	m_pIAttemperEngine = pIAttemperEngine;
}

//���̼��
bool CMonitorCenter::OnEventProcessMonitor(WORD wSubCmdID, VOID* pData, WORD wDataSize, HWND hWndSend)
{
	switch (wSubCmdID)
	{
	case IPC_SUB_MS_PROCESS_START:	//��������
	{
		//����У��
		ASSERT(sizeof(CMD_MS_ProcessStart) == wDataSize);
		if (sizeof(CMD_MS_ProcessStart) != wDataSize) return false;

		//��ȡ����
		auto pProcessStart = (CMD_MS_ProcessStart*)pData;

		//���ҽ���
		if (!CProcessHelper::IsProcessExists(pProcessStart->dwProcessID))
		{
			NotifyCloseProcess(hWndSend);

			return true;
		}

		//��������
		if (m_StockProcessItem.SearchStockItem(pProcessStart->dwProcessID) != NULL)
		{
			return true;
		}

		//��������
		auto pProcessItem = m_StockProcessItem.CreateStockItem(pProcessStart->dwProcessID);
		if (pProcessItem != NULL)
		{
			//��ʼ��
			ZeroMemory(pProcessItem,sizeof(tagServiceProcessItem));

			//������Ϣ
			pProcessItem->hWndRecv = hWndSend;
			pProcessItem->dwProcessID = pProcessStart->dwProcessID;			
			CProcessHelper::GetProcessPathByPId(pProcessItem->dwProcessID, pProcessItem->szProcessPath);
		}

		return true;
	}
	case IPC_SUB_MS_PROCESS_CLOSE:	//���̹ر�
	{
		//����У��
		ASSERT(sizeof(CMD_MS_ProcessClose) == wDataSize);
		if (sizeof(CMD_MS_ProcessClose) != wDataSize) return false;

		//��ȡ����
		auto pProcessClose = (CMD_MS_ProcessClose*)pData;

		//��������
		if (m_StockProcessItem.SearchStockItem(pProcessClose->dwProcessID) == NULL)
		{
			return true;
		}

		//�Ƴ�����
		m_StockProcessItem.RemoveStockItem(pProcessClose->dwProcessID);

		return true;
	}
	case IPC_SUB_MS_SERVICE_READY:	//����׼��
	{
		//����У��
		ASSERT (sizeof(CMD_MS_ServiceReady) == wDataSize);
		if (sizeof(CMD_MS_ServiceReady) != wDataSize) return false;

		//��ȡ����
		auto pServiceReady = (CMD_MS_ServiceReady*)pData;

		//��������
		auto pProcessItem = m_StockProcessItem.SearchStockItem(pServiceReady->dwProcessID);
		if (pProcessItem == NULL)
		{
			pProcessItem = m_StockProcessItem.CreateStockItem(pServiceReady->dwProcessID);	

			//����·��
			CProcessHelper::GetProcessPathByPId(pProcessItem->dwProcessID, pProcessItem->szProcessPath);
		}

		//���÷�����Ϣ
		if (pProcessItem != NULL)
		{
			//������Ϣ
			pProcessItem->hWndRecv = hWndSend;
			pProcessItem->dwProcessID = pServiceReady->dwProcessID;

			//������Ϣ
			pProcessItem->wServiceID = pServiceReady->wServiceID;
			pProcessItem->wServiceModule = pServiceReady->wServiceModule;
			pProcessItem->wServiceStatus = pServiceReady->wServiceStatus;
			StringCchCopy(pProcessItem->szServiceName,CountArray(pProcessItem->szServiceName), pServiceReady->szServiceName);				

			//ͳ����Ϣ
			ZeroMemory(&pProcessItem->ProcessStatInfo,sizeof(pProcessItem->ProcessStatInfo));

			//�����ʶ
			DWORD dwCrashID = MAKELONG(pProcessItem->wServiceModule, pProcessItem->wServiceID);
			auto pServiceCrashInfo = m_StockCrashInfo.SearchStockItem(dwCrashID);
			if (pServiceCrashInfo && pServiceCrashInfo->wServiceModule!=SERVICE_MODULE_GAME)
			{
				//��������				
				StartService(pProcessItem);
			}
		}
	
		return true;
	}	
	case IPC_SUB_MS_SERVICE_STATUS:		//����״̬
	{
		//����У��
		ASSERT(sizeof(CMD_MS_ServiceStatus) == wDataSize);
		if (sizeof(CMD_MS_ServiceStatus) != wDataSize) return false;

		//��ȡ����
		auto pServiceStatus = (CMD_MS_ServiceStatus*)pData;

		//��������
		auto pProcessItem = m_StockProcessItem.SearchStockItem(pServiceStatus->dwProcessID);
		if (pProcessItem != NULL)
		{
			pProcessItem->wServiceStatus = pServiceStatus->wServiceStatus;			

			//����״̬
			if (pProcessItem->wServiceStatus == SERVICE_STATUS_SERVICE)
			{
				//�����ʶ
				DWORD dwCrashID = MAKELONG(pProcessItem->wServiceModule, pProcessItem->wServiceID);

				//������Ϣ
				if (m_StockCrashInfo.SearchStockItem(dwCrashID))
				{
					//�������
					CMD_CS_SendExceptionAlarm SendExceptionAlarm;
					ZeroMemory(&SendExceptionAlarm, sizeof(SendExceptionAlarm));

					//״̬��Ϣ
					StringCchCopyA(SendExceptionAlarm.szAlarmTitle, CountArray(SendExceptionAlarm.szAlarmTitle), "Service Restart Notification");
					StringCchCopy(SendExceptionAlarm.szServiceName, CountArray(SendExceptionAlarm.szServiceName), pProcessItem->szServiceName);
					_sntprintf_s(SendExceptionAlarm.szExceptionInfo, CountArray(SendExceptionAlarm.szExceptionInfo), TEXT("�Զ������ɹ�, ������IP: %s,ƽ̨����: %s"), m_pServiceOption->szServiceDomain, m_szServerName);

					//����Ԥ��
					SendExceptionAlarmNotice(pProcessItem->dwProcessID, &SendExceptionAlarm);

					//�Ƴ�������Ϣ
					m_StockCrashInfo.RemoveStockItem(dwCrashID);
				}
			}
		}				

		return true;
	}
	case IPC_SUB_MS_DATABASE_EXCEPTION:	//���ݿ��쳣
	{
		//����У��
		ASSERT(sizeof(CMD_MS_DataBaseException) >= wDataSize);
		if (sizeof(CMD_MS_DataBaseException) < wDataSize) return false;

		//��ȡ����
		auto pDataBaseException = (CMD_MS_DataBaseException*)pData;

		//��������
		auto pProcessItem = m_StockProcessItem.SearchStockItem(pDataBaseException->dwProcessID);
		if (pProcessItem == NULL) return false;
		
		//�������
		CMD_CS_SendExceptionAlarm SendExceptionAlarm;
		ZeroMemory(&SendExceptionAlarm, sizeof(SendExceptionAlarm));

		//״̬��Ϣ
		StringCchCopyA(SendExceptionAlarm.szAlarmTitle, CountArray(SendExceptionAlarm.szAlarmTitle), "DataBase Exception Notification");
		StringCchCopy(SendExceptionAlarm.szServiceName, CountArray(SendExceptionAlarm.szServiceName), pProcessItem->szServiceName);
		_sntprintf_s(SendExceptionAlarm.szExceptionInfo, CountArray(SendExceptionAlarm.szExceptionInfo), TEXT("%s, ������IP��%s, ƽ̨���ƣ�%s"), pDataBaseException->szExceptionInfo, m_pServiceOption->szServiceDomain, m_szServerName);

		//����Ԥ��
		SendExceptionAlarmNotice(pProcessItem->dwProcessID, &SendExceptionAlarm);

		return true;
	}	
	}

	return true;
}

//��������
bool CMonitorCenter::StartService(tagServiceProcessItem* pServiceProcessItem)
{
	//������Ϣ
	if (m_ProcessTrade.GetInterface() && pServiceProcessItem)
	{
		m_ProcessTrade->SendData(pServiceProcessItem->hWndRecv, IPC_CMD_MS_MONITOR, IPC_SUB_MS_START_SERVICE);

		return true;
	}

	return false;
}

//ֹͣ����
bool CMonitorCenter::ConcludeService(tagServiceProcessItem* pServiceProcessItem)
{
	//������Ϣ
	if (m_ProcessTrade.GetInterface() && pServiceProcessItem)
	{
		m_ProcessTrade->SendData(pServiceProcessItem->hWndRecv, IPC_CMD_MS_MONITOR, IPC_SUB_MS_STOP_SERVICE);

		return true;
	}

	return false;
}

//֪ͨ�ر�
VOID CMonitorCenter::NotifyCloseProcess(HWND hWndClient)
{
	if (m_ProcessTrade.GetInterface())
	{
		m_ProcessTrade->SendData(hWndClient, IPC_CMD_MS_MONITOR, IPC_SUB_MS_CLOSE_PROCESS);
	}
}

//���Ϳ���
bool CMonitorCenter::SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize)
{
	//״̬Ч��
	ASSERT(m_pIAttemperEngine !=NULL);
	if (m_pIAttemperEngine ==NULL) return false;

	//���Ϳ���
	m_pIAttemperEngine->OnEventControl(wControlID,pData,wDataSize);

	return true;
}

//������Ϣ
LRESULT CMonitorCenter::OnUIControlRequest(WPARAM wParam, LPARAM lParam)
{
	//��ȡ����
	tagDataHead DataHead;
	VOID * pData=m_UIControlSink.GetControlRequest(DataHead);

	//���ݴ���
	/*switch (DataHead.dwIdentifier)
	{	

	}*/

	return 0;
}

//����CPUռ��
VOID CMonitorCenter::AnalyseCpuUsage(tagServiceProcessItem* pServiceProcessItem)
{
	//��ǰʱ��
	DWORD dwCurrentTime = time(NULL);

	//����ͳ����Ϣ
	auto pProcessStatInfo = &pServiceProcessItem->ProcessStatInfo;

	//У��CPU
	if (pProcessStatInfo->nCPUUsage >= 100 / m_SystemInfo.dwNumberOfProcessors)
	{
		//��ʼ����
		if (pProcessStatInfo->dwCpuTestTime == 0)
		{
			pProcessStatInfo->dwCpuTestTime = dwCurrentTime;
		}
		
		//У��ʱ��
		if (pProcessStatInfo->dwCpuTestTime + CPU_TEST_TIME_COUNT < dwCurrentTime && pProcessStatInfo->dwCpuAlarmTime==0)
		{
			CTraceService::TraceString(TEXT("��⵽�������CPUռ�ù���(%d%%)��ServiceID=%d,ServiceModule=%d,ServiceName=%s"), TraceLevel_Exception, pProcessStatInfo->nCPUUsage,pServiceProcessItem->wServiceID, pServiceProcessItem->wServiceModule, pServiceProcessItem->szServiceName);

			//��¼ʱ��
			pProcessStatInfo->dwCpuAlarmTime = dwCurrentTime;

			//�������
			CMD_CS_SendExceptionAlarm SendExceptionAlarm;
			ZeroMemory(&SendExceptionAlarm, sizeof(SendExceptionAlarm));

			//״̬��Ϣ
			StringCchCopyA(SendExceptionAlarm.szAlarmTitle, CountArray(SendExceptionAlarm.szAlarmTitle), "CPU Overload Notification");
			StringCchCopy(SendExceptionAlarm.szServiceName, CountArray(SendExceptionAlarm.szServiceName), pServiceProcessItem->szServiceName);
			_sntprintf_s(SendExceptionAlarm.szExceptionInfo, CountArray(SendExceptionAlarm.szExceptionInfo),TEXT("����CPUռ�ù���(%d%%), ������IP: %s,ƽ̨����: %s"), pProcessStatInfo->nCPUUsage, m_pServiceOption->szServiceDomain,m_szServerName);

			//����Ԥ��
			SendExceptionAlarmNotice(pServiceProcessItem->dwProcessID, &SendExceptionAlarm);
		}
	}
	else
	{
		if (pProcessStatInfo->dwCpuAlarmTime + 600 < dwCurrentTime)
		{
			pProcessStatInfo->dwCpuTestTime = 0;
			pProcessStatInfo->dwCpuAlarmTime = 0;
		}
	}
}

//�����ڴ�ռ��
VOID CMonitorCenter::AnalyseMemoryUsage(tagServiceProcessItem* pServiceProcessItem)
{
	//����ͳ����Ϣ
	auto pProcessStatInfo = &pServiceProcessItem->ProcessStatInfo;

	//�����ڴ�
	if ((pProcessStatInfo->uMemoryUsage-pProcessStatInfo ->uAlarmMemoryUsage)/pProcessStatInfo->uInitMemoryUsage >= MEM_TEST_INC_MULTIPLE)
	{
		CTraceService::TraceString(TEXT("��⵽������������ڴ�ռ�ù���(%I64dMB)��ServiceID=%d,ServiceModule=%d,ServiceName=%s"), TraceLevel_Exception, pProcessStatInfo->uMemoryUsage / (1024 * 1024), pServiceProcessItem->wServiceID, pServiceProcessItem->wServiceModule, pServiceProcessItem->szServiceName, pProcessStatInfo->nCPUUsage);

		//��¼�ڴ�
		pProcessStatInfo->uAlarmMemoryUsage = pProcessStatInfo->uMemoryUsage;

		//�������
		CMD_CS_SendExceptionAlarm SendExceptionAlarm;
		ZeroMemory(&SendExceptionAlarm, sizeof(SendExceptionAlarm));

		//״̬��Ϣ
		StringCchCopyA(SendExceptionAlarm.szAlarmTitle, CountArray(SendExceptionAlarm.szAlarmTitle), "Memory Usage Overload Notification");
		StringCchCopy(SendExceptionAlarm.szServiceName, CountArray(SendExceptionAlarm.szServiceName), pServiceProcessItem->szServiceName);
		_sntprintf_s(SendExceptionAlarm.szExceptionInfo, CountArray(SendExceptionAlarm.szExceptionInfo), TEXT("���������ڴ�ռ�ù���(%I64dMB), ������IP: %s, ƽ̨����: %s"), pProcessStatInfo->uMemoryUsage/(1024*1024), m_pServiceOption->szServiceDomain,m_szServerName);

		//����Ԥ��
		SendExceptionAlarmNotice(pServiceProcessItem->dwProcessID, &SendExceptionAlarm);		
	}

	//�����ڴ�
	if ((pProcessStatInfo->uVMemoryUsage - pProcessStatInfo->uAlarmVMemoryUsage) / pProcessStatInfo->uInitVMemoryUsage >= MEM_TEST_INC_MULTIPLE*2)
	{
		CTraceService::TraceString(TEXT("��⵽������������ڴ�ռ�ù���(%I64dMB)��ServiceID=%d,ServiceModule=%d,ServiceName=%s"), TraceLevel_Exception, pProcessStatInfo->uVMemoryUsage / (1024 * 1024), pServiceProcessItem->wServiceID, pServiceProcessItem->wServiceModule, pServiceProcessItem->szServiceName, pProcessStatInfo->nCPUUsage);

		//��¼�ڴ�
		pProcessStatInfo->uAlarmVMemoryUsage = pProcessStatInfo->uVMemoryUsage;

		//�������
		CMD_CS_SendExceptionAlarm SendExceptionAlarm;
		ZeroMemory(&SendExceptionAlarm, sizeof(SendExceptionAlarm));

		//״̬��Ϣ
		StringCchCopyA(SendExceptionAlarm.szAlarmTitle, CountArray(SendExceptionAlarm.szAlarmTitle), "Vritual Memory Overload Notification");
		StringCchCopy(SendExceptionAlarm.szServiceName, CountArray(SendExceptionAlarm.szServiceName), pServiceProcessItem->szServiceName);
		_sntprintf_s(SendExceptionAlarm.szExceptionInfo, CountArray(SendExceptionAlarm.szExceptionInfo), TEXT("���������ڴ�ռ�ù���(%I64dMB), ������IP: %s, ƽ̨����: %s"), pProcessStatInfo->uVMemoryUsage / (1024 * 1024), m_pServiceOption->szServiceDomain, m_szServerName);

		//����Ԥ��
		SendExceptionAlarmNotice(pServiceProcessItem->dwProcessID, &SendExceptionAlarm);
	}
}


//����Ԥ��
VOID CMonitorCenter::SendExceptionAlarmNotice(DWORD dwProcessID,CMD_CS_SendExceptionAlarm* pSendExceptionAlarm)
{
	//��������
	FP_ForwardToCenter ForwardToCenter;
	ZeroMemory(&ForwardToCenter, sizeof(ForwardToCenter));

	//��ȡ����
	auto pForwardHead = (tagForwardHead*)ForwardToCenter.cbDataBuffer;
	ZeroMemory(pForwardHead, sizeof(tagForwardHead));

	//ת������
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;

	//ת������
	pForwardHead->wItemCount = 1;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;
	pForwardHead->ForwardItem[0].dwHashKeyID = dwProcessID;

	//�������
	auto pFSendExceptionAlarm = (CMD_CS_SendExceptionAlarm*)(pForwardHead + 1);
	CopyMemory(pFSendExceptionAlarm, pSendExceptionAlarm, sizeof(CMD_CS_SendExceptionAlarm));

	//�������ݰ�
	ForwardToCenter.wMainCmdID = MDM_CS_FORWARD_SERVICE;
	ForwardToCenter.wSubCmdID = SUB_CS_SEND_EXCEPTION_ALARM;
	ForwardToCenter.wDataSize = sizeof(tagForwardHead) + sizeof(CMD_CS_SendExceptionAlarm);

	//��������				
	WORD wPacketSize = sizeof(ForwardToCenter) - sizeof(ForwardToCenter.cbDataBuffer) + ForwardToCenter.wDataSize;
	SendControlPacket(FT_FORWARD_TO_CENTER, &ForwardToCenter, wPacketSize);
}

//��ʱ����Ϣ
void CMonitorCenter::OnTimer(UINT_PTR nIDEvent)
{
	//������
	if (IDI_PROCESS_DETECT == nIDEvent)
	{
		//��������
		auto pProcessItemCurrent = m_StockProcessItem.GetHeadStockItem();
		while (pProcessItemCurrent)
		{
			//�Ƴ���־
			bool bRemoveItem = false;

			//�������
			if (!CProcessHelper::IsProcessExists(pProcessItemCurrent->dwProcessID))
			{
				bRemoveItem = true;

				CTraceService::TraceString(TEXT("��⵽���������ServiceID=%d,ServiceModule=%d,ServiceName=%s"), TraceLevel_Exception, pProcessItemCurrent->wServiceID, pProcessItemCurrent->wServiceModule, pProcessItemCurrent->szServiceName);

				//У��״̬
				if (pProcessItemCurrent->wServiceStatus == SERVICE_STATUS_SERVICE)
				{
					//�������
					CMD_CS_SendExceptionAlarm SendExceptionAlarm;
					ZeroMemory(&SendExceptionAlarm, sizeof(SendExceptionAlarm));

					//״̬��Ϣ
					StringCchCopyA(SendExceptionAlarm.szAlarmTitle,CountArray(SendExceptionAlarm.szAlarmTitle),"Service Crash  Notification");
					StringCchCopy(SendExceptionAlarm.szServiceName, CountArray(SendExceptionAlarm.szServiceName), pProcessItemCurrent->szServiceName);
					_sntprintf_s(SendExceptionAlarm.szExceptionInfo, CountArray(SendExceptionAlarm.szExceptionInfo), CountArray(SendExceptionAlarm.szExceptionInfo), TEXT("���̱������������ά��Ա��ʱ��Ӧ���� ������IP: %s, ƽ̨���ƣ�%s"), m_pServiceOption->szServiceDomain, m_szServerName);

					//����Ԥ��
					SendExceptionAlarmNotice(pProcessItemCurrent->dwProcessID, &SendExceptionAlarm);

					//��������
					TCHAR szLaunchParam[64] = {0};

					//��Ϸ����
					if (pProcessItemCurrent->wServiceModule == SERVICE_MODULE_GAME)
					{
						_sntprintf_s(szLaunchParam, CountArray(szLaunchParam), TEXT("/ServerID:%d"), pProcessItemCurrent->wServiceID);
					}
					else
					{
						_sntprintf_s(szLaunchParam, CountArray(szLaunchParam), TEXT("/ServiceID:%d"), pProcessItemCurrent->wServiceID);
					}

					//��������
					auto dwProcessID = CProcessHelper::LaunchProcess(pProcessItemCurrent->szProcessPath, szLaunchParam, SW_SHOW);
					if (dwProcessID!=0)
					{
						//�����ʶ
						DWORD dwCrashID = MAKELONG(pProcessItemCurrent->wServiceModule, pProcessItemCurrent->wServiceID);

						//��ȡ����
						auto pServiceCrashInfo = m_StockCrashInfo.SearchStockItem(dwCrashID);
						if (pServiceCrashInfo == NULL)
						{
							pServiceCrashInfo = m_StockCrashInfo.CreateStockItem(dwCrashID);
						}

						//������Ϣ
						pServiceCrashInfo->dwCrashTime = time(NULL);
						pServiceCrashInfo->wServiceID = pProcessItemCurrent->wServiceID;
						pServiceCrashInfo->wServiceModule = pProcessItemCurrent->wServiceModule;
					}
				}
			}
			else
			{
				//ͳ����Ϣ
				auto pProcessStatInfo = &pProcessItemCurrent->ProcessStatInfo;

				//��ȡ��Ϣ
				pProcessStatInfo->dwLastTimeStamp = time(NULL);
				CProcessHelper::GetIOBytes(pProcessItemCurrent->dwProcessID, &pProcessStatInfo->uReadBytes, &pProcessStatInfo->uWriteBytes);
				CProcessHelper::GetMemoryUsage(pProcessItemCurrent->dwProcessID, &pProcessStatInfo->uMemoryUsage, &pProcessStatInfo->uVMemoryUsage);
				pProcessStatInfo->nCPUUsage = CProcessHelper::GetCPUUsage(pProcessItemCurrent->dwProcessID, pProcessStatInfo->CpuLastTime, pProcessStatInfo->CpuLastSystemTime);

				//��¼��ʼֵ
				if (pProcessStatInfo->uInitMemoryUsage == 0) pProcessStatInfo->uInitMemoryUsage = pProcessStatInfo->uMemoryUsage;
				if (pProcessStatInfo->uInitVMemoryUsage == 0) pProcessStatInfo->uInitVMemoryUsage = pProcessStatInfo->uVMemoryUsage;

				//CPUռ�÷���
				if (pProcessStatInfo->nCPUUsage>=0) AnalyseCpuUsage(pProcessItemCurrent);

				//�ڴ�ռ�÷���
				if (pProcessStatInfo->uMemoryUsage>=0) AnalyseMemoryUsage(pProcessItemCurrent);
			}

			//��ȡ��һ��
			auto pProcessItemLast = pProcessItemCurrent;
			pProcessItemCurrent = m_StockProcessItem.GetNextStockItem(pProcessItemLast);

			//�Ƴ���һ��
			if (bRemoveItem)
			{
				m_StockProcessItem.RemoveStockItem(pProcessItemLast->dwProcessID);
			}
		};		
	}

	__super::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////
