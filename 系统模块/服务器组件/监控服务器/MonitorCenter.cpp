#include "StdAfx.h"
#include "MonitorCenter.h"
#include "ControlPacket.h"
#include "ProcessHelper.h"

//////////////////////////////////////////////////////////////////////////
//时钟定义
#define					IDI_PROCESS_DETECT		1					//进程检测
#define					TIME_PROCESS_DETECT		2000				//检测时间

//常量定义
#define					CPU_TEST_TIME_COUNT		20					//测试时长	
#define					MEM_TEST_INC_MULTIPLE	10					//增长倍数		

//////////////////////////////////////////////////////////////////////////

//静态变量
CMonitorCenter *	CMonitorCenter::g_pMonitorCenter=NULL;			//对象指针

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMonitorCenter, CWnd)
	ON_WM_TIMER()
	ON_MESSAGE(WM_UICONTROL_REQUEST,OnUIControlRequest)	
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CMonitorCenter::CMonitorCenter()
{
	//设置变量
	m_bServiceStatus = false;
	m_pServiceOption = NULL;
	m_pIAttemperEngine = NULL;	

	//初始化
	m_StockCrashInfo.InitHashTable(13);
	m_StockProcessItem.InitHashTable(13);

	//系统信息
	GetSystemInfo(&m_SystemInfo);

	//设置对象
	ASSERT(g_pMonitorCenter ==NULL);
	if (g_pMonitorCenter ==NULL) g_pMonitorCenter =this;

	return;
}

//析构函数
CMonitorCenter::~CMonitorCenter()
{
	//删除子项
	m_StockProcessItem.DeleteAllItem();
}

//接口查询
VOID* CMonitorCenter::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IProcessTradeSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IProcessTradeSink, Guid, dwQueryVer);
	return NULL;
}

//启动服务
bool CMonitorCenter::StartService()
{
	//校验状态
	ASSERT (m_bServiceStatus ==false);
	if (m_bServiceStatus == true) return true;

	//创建窗口
	if (m_hWnd==NULL)
	{
		//创建窗口
		CRect rcCreate(0,0,0,0);
		Create(NULL,NULL,WS_CHILD,rcCreate,AfxGetMainWnd(),200);

		//设置接口
		m_UIControlSink.SetHookWindow(m_hWnd);
	}
	
	//创建接口
	if (!m_ProcessTrade.GetInterface() && !m_ProcessTrade.CreateInstance())
	{
		return false;
	}

	//设置回调
	if (!m_ProcessTrade->SetProcessTradeSink(QUERY_ME_INTERFACE(IProcessTradeSink)))
	{
		return false;
	}

	//创建对象	
	if (!m_ProcessTrade->CreateProcessTrade(MONITOR_CLASS_NAME))
	{
		return false;
	}

	//设置标志
	m_bServiceStatus = true;

	//启动定时器
	SetTimer(IDI_PROCESS_DETECT, TIME_PROCESS_DETECT, NULL);

	return true;
}

//停止服务
bool CMonitorCenter::ConcludeService()
{
	//校验状态
	if (m_bServiceStatus == true)
	{
		//设置状态
		m_bServiceStatus = false;

		//关闭定时器
		KillTimer(IDI_PROCESS_DETECT);

		//移除子项
		m_StockCrashInfo.RemoveAllItem();
		m_StockProcessItem.RemoveAllItem();

		//关闭进程
		if (m_ProcessTrade.GetInterface() != NULL) m_ProcessTrade.CloseInstance();

		//销毁窗口
		if (m_hWnd) DestroyWindow();
	}	
	
	return true;
}

//进程数据
bool CMonitorCenter::OnProcessTradeData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize, HWND hWndSend)
{
	CTraceService::TraceString(TEXT("MainCmdID=%d,SubCmdID=%d"), TraceLevel_Debug, wMainCmdID, wSubCmdID);

	switch (wMainCmdID)
	{
	case IPC_CMD_MS_MONITOR:			//监控命令
	{
		return OnEventProcessMonitor(wSubCmdID,pData,wDataSize,hWndSend);
	}
	}

	return true;
}

//服务名称
VOID CMonitorCenter::SetServerName(LPCTSTR pszServerName)
{
	StringCchCopy(m_szServerName,CountArray(m_szServerName),pszServerName);
}

//服务参数
VOID CMonitorCenter::SetServiceOption(tagServiceOption* pServiceOption)
{
	m_pServiceOption = pServiceOption;
}

//设置接口
VOID CMonitorCenter::SetAttemperEngine(IAttemperEngine* pIAttemperEngine)
{
	m_pIAttemperEngine = pIAttemperEngine;
}

//进程监控
bool CMonitorCenter::OnEventProcessMonitor(WORD wSubCmdID, VOID* pData, WORD wDataSize, HWND hWndSend)
{
	switch (wSubCmdID)
	{
	case IPC_SUB_MS_PROCESS_START:	//进程启动
	{
		//数据校验
		ASSERT(sizeof(CMD_MS_ProcessStart) == wDataSize);
		if (sizeof(CMD_MS_ProcessStart) != wDataSize) return false;

		//提取数据
		auto pProcessStart = (CMD_MS_ProcessStart*)pData;

		//查找进程
		if (!CProcessHelper::IsProcessExists(pProcessStart->dwProcessID))
		{
			NotifyCloseProcess(hWndSend);

			return true;
		}

		//查找子项
		if (m_StockProcessItem.SearchStockItem(pProcessStart->dwProcessID) != NULL)
		{
			return true;
		}

		//创建子项
		auto pProcessItem = m_StockProcessItem.CreateStockItem(pProcessStart->dwProcessID);
		if (pProcessItem != NULL)
		{
			//初始化
			ZeroMemory(pProcessItem,sizeof(tagServiceProcessItem));

			//进程信息
			pProcessItem->hWndRecv = hWndSend;
			pProcessItem->dwProcessID = pProcessStart->dwProcessID;			
			CProcessHelper::GetProcessPathByPId(pProcessItem->dwProcessID, pProcessItem->szProcessPath);
		}

		return true;
	}
	case IPC_SUB_MS_PROCESS_CLOSE:	//进程关闭
	{
		//数据校验
		ASSERT(sizeof(CMD_MS_ProcessClose) == wDataSize);
		if (sizeof(CMD_MS_ProcessClose) != wDataSize) return false;

		//提取数据
		auto pProcessClose = (CMD_MS_ProcessClose*)pData;

		//查找子项
		if (m_StockProcessItem.SearchStockItem(pProcessClose->dwProcessID) == NULL)
		{
			return true;
		}

		//移除子项
		m_StockProcessItem.RemoveStockItem(pProcessClose->dwProcessID);

		return true;
	}
	case IPC_SUB_MS_SERVICE_READY:	//服务准备
	{
		//数据校验
		ASSERT (sizeof(CMD_MS_ServiceReady) == wDataSize);
		if (sizeof(CMD_MS_ServiceReady) != wDataSize) return false;

		//提取数据
		auto pServiceReady = (CMD_MS_ServiceReady*)pData;

		//查找子项
		auto pProcessItem = m_StockProcessItem.SearchStockItem(pServiceReady->dwProcessID);
		if (pProcessItem == NULL)
		{
			pProcessItem = m_StockProcessItem.CreateStockItem(pServiceReady->dwProcessID);	

			//进程路径
			CProcessHelper::GetProcessPathByPId(pProcessItem->dwProcessID, pProcessItem->szProcessPath);
		}

		//设置服务信息
		if (pProcessItem != NULL)
		{
			//进程信息
			pProcessItem->hWndRecv = hWndSend;
			pProcessItem->dwProcessID = pServiceReady->dwProcessID;

			//服务信息
			pProcessItem->wServiceID = pServiceReady->wServiceID;
			pProcessItem->wServiceModule = pServiceReady->wServiceModule;
			pProcessItem->wServiceStatus = pServiceReady->wServiceStatus;
			StringCchCopy(pProcessItem->szServiceName,CountArray(pProcessItem->szServiceName), pServiceReady->szServiceName);				

			//统计信息
			ZeroMemory(&pProcessItem->ProcessStatInfo,sizeof(pProcessItem->ProcessStatInfo));

			//构造标识
			DWORD dwCrashID = MAKELONG(pProcessItem->wServiceModule, pProcessItem->wServiceID);
			auto pServiceCrashInfo = m_StockCrashInfo.SearchStockItem(dwCrashID);
			if (pServiceCrashInfo && pServiceCrashInfo->wServiceModule!=SERVICE_MODULE_GAME)
			{
				//启动服务				
				StartService(pProcessItem);
			}
		}
	
		return true;
	}	
	case IPC_SUB_MS_SERVICE_STATUS:		//服务状态
	{
		//数据校验
		ASSERT(sizeof(CMD_MS_ServiceStatus) == wDataSize);
		if (sizeof(CMD_MS_ServiceStatus) != wDataSize) return false;

		//提取数据
		auto pServiceStatus = (CMD_MS_ServiceStatus*)pData;

		//查找子项
		auto pProcessItem = m_StockProcessItem.SearchStockItem(pServiceStatus->dwProcessID);
		if (pProcessItem != NULL)
		{
			pProcessItem->wServiceStatus = pServiceStatus->wServiceStatus;			

			//服务状态
			if (pProcessItem->wServiceStatus == SERVICE_STATUS_SERVICE)
			{
				//构造标识
				DWORD dwCrashID = MAKELONG(pProcessItem->wServiceModule, pProcessItem->wServiceID);

				//查找信息
				if (m_StockCrashInfo.SearchStockItem(dwCrashID))
				{
					//构造对象
					CMD_CS_SendExceptionAlarm SendExceptionAlarm;
					ZeroMemory(&SendExceptionAlarm, sizeof(SendExceptionAlarm));

					//状态信息
					StringCchCopyA(SendExceptionAlarm.szAlarmTitle, CountArray(SendExceptionAlarm.szAlarmTitle), "Service Restart Notification");
					StringCchCopy(SendExceptionAlarm.szServiceName, CountArray(SendExceptionAlarm.szServiceName), pProcessItem->szServiceName);
					_sntprintf_s(SendExceptionAlarm.szExceptionInfo, CountArray(SendExceptionAlarm.szExceptionInfo), TEXT("自动重启成功, 服务器IP: %s,平台名称: %s"), m_pServiceOption->szServiceDomain, m_szServerName);

					//发送预警
					SendExceptionAlarmNotice(pProcessItem->dwProcessID, &SendExceptionAlarm);

					//移除崩溃信息
					m_StockCrashInfo.RemoveStockItem(dwCrashID);
				}
			}
		}				

		return true;
	}
	case IPC_SUB_MS_DATABASE_EXCEPTION:	//数据库异常
	{
		//数据校验
		ASSERT(sizeof(CMD_MS_DataBaseException) >= wDataSize);
		if (sizeof(CMD_MS_DataBaseException) < wDataSize) return false;

		//提取数据
		auto pDataBaseException = (CMD_MS_DataBaseException*)pData;

		//查找子项
		auto pProcessItem = m_StockProcessItem.SearchStockItem(pDataBaseException->dwProcessID);
		if (pProcessItem == NULL) return false;
		
		//构造对象
		CMD_CS_SendExceptionAlarm SendExceptionAlarm;
		ZeroMemory(&SendExceptionAlarm, sizeof(SendExceptionAlarm));

		//状态信息
		StringCchCopyA(SendExceptionAlarm.szAlarmTitle, CountArray(SendExceptionAlarm.szAlarmTitle), "DataBase Exception Notification");
		StringCchCopy(SendExceptionAlarm.szServiceName, CountArray(SendExceptionAlarm.szServiceName), pProcessItem->szServiceName);
		_sntprintf_s(SendExceptionAlarm.szExceptionInfo, CountArray(SendExceptionAlarm.szExceptionInfo), TEXT("%s, 服务器IP：%s, 平台名称：%s"), pDataBaseException->szExceptionInfo, m_pServiceOption->szServiceDomain, m_szServerName);

		//发送预警
		SendExceptionAlarmNotice(pProcessItem->dwProcessID, &SendExceptionAlarm);

		return true;
	}	
	}

	return true;
}

//启动服务
bool CMonitorCenter::StartService(tagServiceProcessItem* pServiceProcessItem)
{
	//发送消息
	if (m_ProcessTrade.GetInterface() && pServiceProcessItem)
	{
		m_ProcessTrade->SendData(pServiceProcessItem->hWndRecv, IPC_CMD_MS_MONITOR, IPC_SUB_MS_START_SERVICE);

		return true;
	}

	return false;
}

//停止服务
bool CMonitorCenter::ConcludeService(tagServiceProcessItem* pServiceProcessItem)
{
	//发送消息
	if (m_ProcessTrade.GetInterface() && pServiceProcessItem)
	{
		m_ProcessTrade->SendData(pServiceProcessItem->hWndRecv, IPC_CMD_MS_MONITOR, IPC_SUB_MS_STOP_SERVICE);

		return true;
	}

	return false;
}

//通知关闭
VOID CMonitorCenter::NotifyCloseProcess(HWND hWndClient)
{
	if (m_ProcessTrade.GetInterface())
	{
		m_ProcessTrade->SendData(hWndClient, IPC_CMD_MS_MONITOR, IPC_SUB_MS_CLOSE_PROCESS);
	}
}

//发送控制
bool CMonitorCenter::SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize)
{
	//状态效验
	ASSERT(m_pIAttemperEngine !=NULL);
	if (m_pIAttemperEngine ==NULL) return false;

	//发送控制
	m_pIAttemperEngine->OnEventControl(wControlID,pData,wDataSize);

	return true;
}

//控制消息
LRESULT CMonitorCenter::OnUIControlRequest(WPARAM wParam, LPARAM lParam)
{
	//获取数据
	tagDataHead DataHead;
	VOID * pData=m_UIControlSink.GetControlRequest(DataHead);

	//数据处理
	/*switch (DataHead.dwIdentifier)
	{	

	}*/

	return 0;
}

//分析CPU占用
VOID CMonitorCenter::AnalyseCpuUsage(tagServiceProcessItem* pServiceProcessItem)
{
	//当前时间
	DWORD dwCurrentTime = time(NULL);

	//进程统计信息
	auto pProcessStatInfo = &pServiceProcessItem->ProcessStatInfo;

	//校验CPU
	if (pProcessStatInfo->nCPUUsage >= 100 / m_SystemInfo.dwNumberOfProcessors)
	{
		//开始测试
		if (pProcessStatInfo->dwCpuTestTime == 0)
		{
			pProcessStatInfo->dwCpuTestTime = dwCurrentTime;
		}
		
		//校验时长
		if (pProcessStatInfo->dwCpuTestTime + CPU_TEST_TIME_COUNT < dwCurrentTime && pProcessStatInfo->dwCpuAlarmTime==0)
		{
			CTraceService::TraceString(TEXT("检测到服务进程CPU占用过高(%d%%)，ServiceID=%d,ServiceModule=%d,ServiceName=%s"), TraceLevel_Exception, pProcessStatInfo->nCPUUsage,pServiceProcessItem->wServiceID, pServiceProcessItem->wServiceModule, pServiceProcessItem->szServiceName);

			//记录时间
			pProcessStatInfo->dwCpuAlarmTime = dwCurrentTime;

			//构造对象
			CMD_CS_SendExceptionAlarm SendExceptionAlarm;
			ZeroMemory(&SendExceptionAlarm, sizeof(SendExceptionAlarm));

			//状态信息
			StringCchCopyA(SendExceptionAlarm.szAlarmTitle, CountArray(SendExceptionAlarm.szAlarmTitle), "CPU Overload Notification");
			StringCchCopy(SendExceptionAlarm.szServiceName, CountArray(SendExceptionAlarm.szServiceName), pServiceProcessItem->szServiceName);
			_sntprintf_s(SendExceptionAlarm.szExceptionInfo, CountArray(SendExceptionAlarm.szExceptionInfo),TEXT("进程CPU占用过高(%d%%), 服务器IP: %s,平台名称: %s"), pProcessStatInfo->nCPUUsage, m_pServiceOption->szServiceDomain,m_szServerName);

			//发送预警
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

//分析内存占用
VOID CMonitorCenter::AnalyseMemoryUsage(tagServiceProcessItem* pServiceProcessItem)
{
	//进程统计信息
	auto pProcessStatInfo = &pServiceProcessItem->ProcessStatInfo;

	//物理内存
	if ((pProcessStatInfo->uMemoryUsage-pProcessStatInfo ->uAlarmMemoryUsage)/pProcessStatInfo->uInitMemoryUsage >= MEM_TEST_INC_MULTIPLE)
	{
		CTraceService::TraceString(TEXT("检测到服务进程物理内存占用过高(%I64dMB)，ServiceID=%d,ServiceModule=%d,ServiceName=%s"), TraceLevel_Exception, pProcessStatInfo->uMemoryUsage / (1024 * 1024), pServiceProcessItem->wServiceID, pServiceProcessItem->wServiceModule, pServiceProcessItem->szServiceName, pProcessStatInfo->nCPUUsage);

		//记录内存
		pProcessStatInfo->uAlarmMemoryUsage = pProcessStatInfo->uMemoryUsage;

		//构造对象
		CMD_CS_SendExceptionAlarm SendExceptionAlarm;
		ZeroMemory(&SendExceptionAlarm, sizeof(SendExceptionAlarm));

		//状态信息
		StringCchCopyA(SendExceptionAlarm.szAlarmTitle, CountArray(SendExceptionAlarm.szAlarmTitle), "Memory Usage Overload Notification");
		StringCchCopy(SendExceptionAlarm.szServiceName, CountArray(SendExceptionAlarm.szServiceName), pServiceProcessItem->szServiceName);
		_sntprintf_s(SendExceptionAlarm.szExceptionInfo, CountArray(SendExceptionAlarm.szExceptionInfo), TEXT("进程物理内存占用过高(%I64dMB), 服务器IP: %s, 平台名称: %s"), pProcessStatInfo->uMemoryUsage/(1024*1024), m_pServiceOption->szServiceDomain,m_szServerName);

		//发送预警
		SendExceptionAlarmNotice(pServiceProcessItem->dwProcessID, &SendExceptionAlarm);		
	}

	//虚拟内存
	if ((pProcessStatInfo->uVMemoryUsage - pProcessStatInfo->uAlarmVMemoryUsage) / pProcessStatInfo->uInitVMemoryUsage >= MEM_TEST_INC_MULTIPLE*2)
	{
		CTraceService::TraceString(TEXT("检测到服务进程虚拟内存占用过高(%I64dMB)，ServiceID=%d,ServiceModule=%d,ServiceName=%s"), TraceLevel_Exception, pProcessStatInfo->uVMemoryUsage / (1024 * 1024), pServiceProcessItem->wServiceID, pServiceProcessItem->wServiceModule, pServiceProcessItem->szServiceName, pProcessStatInfo->nCPUUsage);

		//记录内存
		pProcessStatInfo->uAlarmVMemoryUsage = pProcessStatInfo->uVMemoryUsage;

		//构造对象
		CMD_CS_SendExceptionAlarm SendExceptionAlarm;
		ZeroMemory(&SendExceptionAlarm, sizeof(SendExceptionAlarm));

		//状态信息
		StringCchCopyA(SendExceptionAlarm.szAlarmTitle, CountArray(SendExceptionAlarm.szAlarmTitle), "Vritual Memory Overload Notification");
		StringCchCopy(SendExceptionAlarm.szServiceName, CountArray(SendExceptionAlarm.szServiceName), pServiceProcessItem->szServiceName);
		_sntprintf_s(SendExceptionAlarm.szExceptionInfo, CountArray(SendExceptionAlarm.szExceptionInfo), TEXT("进程虚拟内存占用过高(%I64dMB), 服务器IP: %s, 平台名称: %s"), pProcessStatInfo->uVMemoryUsage / (1024 * 1024), m_pServiceOption->szServiceDomain, m_szServerName);

		//发送预警
		SendExceptionAlarmNotice(pServiceProcessItem->dwProcessID, &SendExceptionAlarm);
	}
}


//发送预警
VOID CMonitorCenter::SendExceptionAlarmNotice(DWORD dwProcessID,CMD_CS_SendExceptionAlarm* pSendExceptionAlarm)
{
	//变量定义
	FP_ForwardToCenter ForwardToCenter;
	ZeroMemory(&ForwardToCenter, sizeof(ForwardToCenter));

	//获取对象
	auto pForwardHead = (tagForwardHead*)ForwardToCenter.cbDataBuffer;
	ZeroMemory(pForwardHead, sizeof(tagForwardHead));

	//转发对象
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;

	//转发子项
	pForwardHead->wItemCount = 1;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;
	pForwardHead->ForwardItem[0].dwHashKeyID = dwProcessID;

	//构造对象
	auto pFSendExceptionAlarm = (CMD_CS_SendExceptionAlarm*)(pForwardHead + 1);
	CopyMemory(pFSendExceptionAlarm, pSendExceptionAlarm, sizeof(CMD_CS_SendExceptionAlarm));

	//构造数据包
	ForwardToCenter.wMainCmdID = MDM_CS_FORWARD_SERVICE;
	ForwardToCenter.wSubCmdID = SUB_CS_SEND_EXCEPTION_ALARM;
	ForwardToCenter.wDataSize = sizeof(tagForwardHead) + sizeof(CMD_CS_SendExceptionAlarm);

	//发送数据				
	WORD wPacketSize = sizeof(ForwardToCenter) - sizeof(ForwardToCenter.cbDataBuffer) + ForwardToCenter.wDataSize;
	SendControlPacket(FT_FORWARD_TO_CENTER, &ForwardToCenter, wPacketSize);
}

//定时器消息
void CMonitorCenter::OnTimer(UINT_PTR nIDEvent)
{
	//检测进程
	if (IDI_PROCESS_DETECT == nIDEvent)
	{
		//遍历服务
		auto pProcessItemCurrent = m_StockProcessItem.GetHeadStockItem();
		while (pProcessItemCurrent)
		{
			//移除标志
			bool bRemoveItem = false;

			//崩溃检测
			if (!CProcessHelper::IsProcessExists(pProcessItemCurrent->dwProcessID))
			{
				bRemoveItem = true;

				CTraceService::TraceString(TEXT("检测到服务崩溃，ServiceID=%d,ServiceModule=%d,ServiceName=%s"), TraceLevel_Exception, pProcessItemCurrent->wServiceID, pProcessItemCurrent->wServiceModule, pProcessItemCurrent->szServiceName);

				//校验状态
				if (pProcessItemCurrent->wServiceStatus == SERVICE_STATUS_SERVICE)
				{
					//构造对象
					CMD_CS_SendExceptionAlarm SendExceptionAlarm;
					ZeroMemory(&SendExceptionAlarm, sizeof(SendExceptionAlarm));

					//状态信息
					StringCchCopyA(SendExceptionAlarm.szAlarmTitle,CountArray(SendExceptionAlarm.szAlarmTitle),"Service Crash  Notification");
					StringCchCopy(SendExceptionAlarm.szServiceName, CountArray(SendExceptionAlarm.szServiceName), pProcessItemCurrent->szServiceName);
					_sntprintf_s(SendExceptionAlarm.szExceptionInfo, CountArray(SendExceptionAlarm.szExceptionInfo), CountArray(SendExceptionAlarm.szExceptionInfo), TEXT("进程崩溃，请相关运维人员及时响应处理！ 服务器IP: %s, 平台名称：%s"), m_pServiceOption->szServiceDomain, m_szServerName);

					//发送预警
					SendExceptionAlarmNotice(pProcessItemCurrent->dwProcessID, &SendExceptionAlarm);

					//启动参数
					TCHAR szLaunchParam[64] = {0};

					//游戏服务
					if (pProcessItemCurrent->wServiceModule == SERVICE_MODULE_GAME)
					{
						_sntprintf_s(szLaunchParam, CountArray(szLaunchParam), TEXT("/ServerID:%d"), pProcessItemCurrent->wServiceID);
					}
					else
					{
						_sntprintf_s(szLaunchParam, CountArray(szLaunchParam), TEXT("/ServiceID:%d"), pProcessItemCurrent->wServiceID);
					}

					//启动进程
					auto dwProcessID = CProcessHelper::LaunchProcess(pProcessItemCurrent->szProcessPath, szLaunchParam, SW_SHOW);
					if (dwProcessID!=0)
					{
						//构造标识
						DWORD dwCrashID = MAKELONG(pProcessItemCurrent->wServiceModule, pProcessItemCurrent->wServiceID);

						//获取对象
						auto pServiceCrashInfo = m_StockCrashInfo.SearchStockItem(dwCrashID);
						if (pServiceCrashInfo == NULL)
						{
							pServiceCrashInfo = m_StockCrashInfo.CreateStockItem(dwCrashID);
						}

						//崩溃信息
						pServiceCrashInfo->dwCrashTime = time(NULL);
						pServiceCrashInfo->wServiceID = pProcessItemCurrent->wServiceID;
						pServiceCrashInfo->wServiceModule = pProcessItemCurrent->wServiceModule;
					}
				}
			}
			else
			{
				//统计信息
				auto pProcessStatInfo = &pProcessItemCurrent->ProcessStatInfo;

				//获取信息
				pProcessStatInfo->dwLastTimeStamp = time(NULL);
				CProcessHelper::GetIOBytes(pProcessItemCurrent->dwProcessID, &pProcessStatInfo->uReadBytes, &pProcessStatInfo->uWriteBytes);
				CProcessHelper::GetMemoryUsage(pProcessItemCurrent->dwProcessID, &pProcessStatInfo->uMemoryUsage, &pProcessStatInfo->uVMemoryUsage);
				pProcessStatInfo->nCPUUsage = CProcessHelper::GetCPUUsage(pProcessItemCurrent->dwProcessID, pProcessStatInfo->CpuLastTime, pProcessStatInfo->CpuLastSystemTime);

				//记录初始值
				if (pProcessStatInfo->uInitMemoryUsage == 0) pProcessStatInfo->uInitMemoryUsage = pProcessStatInfo->uMemoryUsage;
				if (pProcessStatInfo->uInitVMemoryUsage == 0) pProcessStatInfo->uInitVMemoryUsage = pProcessStatInfo->uVMemoryUsage;

				//CPU占用分析
				if (pProcessStatInfo->nCPUUsage>=0) AnalyseCpuUsage(pProcessItemCurrent);

				//内存占用分析
				if (pProcessStatInfo->uMemoryUsage>=0) AnalyseMemoryUsage(pProcessItemCurrent);
			}

			//获取下一个
			auto pProcessItemLast = pProcessItemCurrent;
			pProcessItemCurrent = m_StockProcessItem.GetNextStockItem(pProcessItemLast);

			//移除上一个
			if (bRemoveItem)
			{
				m_StockProcessItem.RemoveStockItem(pProcessItemLast->dwProcessID);
			}
		};		
	}

	__super::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////
