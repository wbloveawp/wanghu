#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_COLLECT_ONLINE_INFO		1									//统计在线
#define IDI_UPLOAD_GATE_LIST		2									//上传网关	
#define IDI_UPDATE_STOCK_INFO		3									//更新库存	
#define IDI_UPDATE_SERVER_LIST		4									//房间列表
#define IDI_UPDATE_RANKING_LIST		5									//更新排行


//常量定义
#define MAX_TABLE_COUNT				32									//最大桌数	

//////////////////////////////////////////////////////////////////////////
//平台人数
struct tagPlatformUserCount
{
	DWORD							dwGlobalCount;						//用户总数
	DWORD							dwPlayerCount;						//玩家总数
	DWORD							dwAndroidCount;						//机器总数
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//状态变量
	m_bWillConcludeService=false;
	m_wCollectItem=INVALID_WORD;

	//设置变量
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	return;
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{
}

//接口查询
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//绑定参数
	m_pBindParameter=new tagBindParameter[m_pInitParameter->m_wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pInitParameter->m_wMaxConnect);

	//设置时间
	ASSERT(m_pITimerEngine != NULL);
	m_pITimerEngine->SetTimer(IDI_UPDATE_SERVER_LIST, 10 * 1000L, TIMES_INFINITY, 0);

	//设置时间
	ASSERT(m_pITimerEngine != NULL);
	m_pITimerEngine->SetTimer(IDI_UPDATE_RANKING_LIST, 60 * 1000L, TIMES_INFINITY, 0);		

	//设置时间
	ASSERT(m_pITimerEngine != NULL);
	m_pITimerEngine->SetTimer(IDI_UPLOAD_GATE_LIST, m_pInitParameter->m_wUploadTime * 1000L, TIMES_INFINITY, 0);

	//设置时间
	ASSERT(m_pITimerEngine!=NULL);
	m_pITimerEngine->SetTimer(IDI_COLLECT_ONLINE_INFO,m_pInitParameter->m_wCollectTime * 1000L,TIMES_INFINITY,0);

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//状态变量
	m_bWillConcludeService=false;
	m_wCollectItem=INVALID_WORD;
	m_WaitCollectItemArray.RemoveAll();

	//设置变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	//删除数据
	SafeDeleteArray(m_pBindParameter);

	//设置组件	
	m_GlobalInfoManager.ResetData();

	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	switch (dwIdentifier)
	{
	case CT_LOAD_GLOBAL_CONFIG:		//加载配置
	{
		//加载配置
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_CONFIG, 0, 0, 0, NULL, 0);

		return true;
	}
	case CT_SERVICE_WILLSTOP:		//停止服务
	{
		//状态判断
		if (m_bWillConcludeService) return true;

		//设置状态
		m_bWillConcludeService = true;

		//事件通知
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess = ER_SUCCESS;
		SendUIControlPacket(UI_WILLSTOP_RESULT, &ControlResult, sizeof(ControlResult));			

		return true;
	}
	case CT_DEBUG_SERVICE:			//调试服务
	{
		CMD_CS_C_ModifyStockItem ModifyStockItem;
		ModifyStockItem.wStockID = 1;
		
		OnTCPNetworkMasterService(SUB_CS_C_MODIFY_STOCK_ITEM, &ModifyStockItem, sizeof(ModifyStockItem), 0);


		CMD_CS_C_AdjustStockScore AdjustStockScore;
		AdjustStockScore.wStockID = 4;
		AdjustStockScore.lChangeScore = 1000;		

		OnTCPNetworkMasterService(SUB_CS_C_ADJUST_STOCK_SCORE,&AdjustStockScore,sizeof(AdjustStockScore),0);


		CMD_CS_C_UpdateSPlayerControl UpdateSPlayerControl = {};		

		OnTCPNetworkMasterService(SUB_CS_C_UPDATE_SPLAYER_CONTROL, &UpdateSPlayerControl, sizeof(UpdateSPlayerControl), 0);

		return true;
	}
	}

	return false;
}

//调度事件
bool CAttemperEngineSink::OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//时间事件
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	switch (dwTimerID)
	{
	case IDI_COLLECT_ONLINE_INFO:	//统计在线
	{
		//发送请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_ONLINE_COUNT_INFO, 0, 0, 0, NULL, 0);

		return true;
	}	
	case IDI_UPLOAD_GATE_LIST:		//上传网关
	{
		//网关列表
		if (m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_GATE) > 0)
		{
			UpdateGateList();

			return true;
		}

		return true;
	}
	case IDI_UPDATE_SERVER_LIST:		//上报房间列表
	{
		//构造结构
		DBR_GP_UpdateServerList UpdateServerList;
		ZeroMemory(&UpdateServerList, sizeof(UpdateServerList));

		//更新类型
		UpdateServerList.cbUpdateKind = UPDATE_KIND_MODIFY;

		//变量定义
		CStringW strServerList, strServerItem;

		//枚举定义
		POSITION Position = NULL;
		CGlobalServerInfo * pGlobalServerInfo = NULL;

		//变量定义	
		do
		{
			pGlobalServerInfo = (CGlobalServerInfo*)m_GlobalInfoManager.EnumServiceInfo(SERVICE_MODULE_GAME, Position);
			if (pGlobalServerInfo == NULL) break;

			//格式化
			FormatServerItem(pGlobalServerInfo, strServerItem);
			
			//发送判断
			if (strServerList.GetLength() + strServerItem.GetLength() >= CountArray(UpdateServerList.szServerList))
			{
				strServerList.Append(L"]");

				//拷贝信息
				StringCchCopy(UpdateServerList.szServerList, CountArray(UpdateServerList.szServerList), strServerList.GetString());

				//发送请求
				WORD wDataSize = CountStringBufferW(UpdateServerList.szServerList);
				WORD wHeadSize = sizeof(UpdateServerList) - sizeof(UpdateServerList.szServerList);
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_SERVER_LIST, 0L, 0L, 0L, &UpdateServerList, wHeadSize + wDataSize);

				//清空数据
				strServerList.Empty();
			}

			//拼接子项
			strServerList.Append(strServerList.IsEmpty() ? L"[" : L",");
			strServerList.Append(strServerItem.GetString());					

		} while (Position!=NULL);

		//发送列表
		if (strServerList.IsEmpty() == false)
		{
			strServerList.Append(L"]");

			//拷贝信息
			StringCchCopy(UpdateServerList.szServerList, CountArray(UpdateServerList.szServerList), strServerList.GetString());

			//发送请求
			WORD wDataSize = CountStringBufferW(UpdateServerList.szServerList);
			WORD wHeadSize = sizeof(UpdateServerList) - sizeof(UpdateServerList.szServerList);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_SERVER_LIST, 0L, 0L, 0L, &UpdateServerList, wHeadSize + wDataSize);
		}

		return true;
	}	
	case IDI_UPDATE_RANKING_LIST:		//更新榜单
	{
		//发送请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_RANKING_LIST, 0L, 0L, 0L, NULL, 0);

		return true;
	}
	}

	return false;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//获取索引
	ASSERT(LOWORD(dwSocketID)<m_pInitParameter->m_wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pInitParameter->m_wMaxConnect) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//设置变量
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientIP=dwClientIP;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (Command.wMainCmdID)
	{
	case MDM_CS_REGISTER:			//服务注册
	{
		return OnTCPNetworkMainRegister(Command.wSubCmdID,pData,wDataSize,dwSocketID);
	}
	case MDM_CS_COMMON_SERVICE:		//服务信息
	{
		return OnTCPNetworkMainCommonService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
	}
	case MDM_CS_USER_COLLECT:		//用户命令
	{
		return OnTCPNetworkMainUserCollect(Command.wSubCmdID,pData,wDataSize,dwSocketID);
	}
	case MDM_CS_QUERY_SERVICE:		//信息查询
	{
		return OnTCPNetworkMainQueryService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
	}
	case MDM_CS_BROADCAST:			//广播消息
	{
		return OnTCPNetworkMainBroadcast(Command.wSubCmdID,pData,wDataSize,dwSocketID);
	}
	case MDM_CS_MASTER_SERVICE:		//后台服务
	{
		return OnTCPNetworkMasterService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
	}
	case MDM_CS_MANAGER_SERVICE:	//管理服务
	{
		return OnTCPNetWorkMainManagerService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
	}
	case MDM_CS_FORWARD_SERVICE:	//转发服务
	{
		return OnTCPNetworkMainForwardService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
	}	
	}

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//游戏服务
	if (pBindParameter->wServiceModule == SERVICE_MODULE_GAME)	
	{
		//汇总处理
		if (wBindIndex==m_wCollectItem)
		{
			//设置变量
			m_wCollectItem=INVALID_WORD;

			//汇总切换
			if (m_WaitCollectItemArray.GetCount()>0)
			{
				//提取变量
				INT_PTR nWaitCount=m_WaitCollectItemArray.GetCount();
				m_wCollectItem=m_WaitCollectItemArray[nWaitCount-1];

				//删除数组
				m_WaitCollectItemArray.RemoveAt(nWaitCount-1);

				//发送消息
				DWORD dwSocketID=(m_pBindParameter+m_wCollectItem)->dwSocketID;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_USER,NULL,0);
			}
		}
		else
		{
			//删除等待
			for (INT_PTR i=0;i<m_WaitCollectItemArray.GetCount();i++)
			{
				if (wBindIndex==m_WaitCollectItemArray[i])
				{
					m_WaitCollectItemArray.RemoveAt(i);
					break;
				}
			}
		}
	}
	
	//注销服务
	if (pBindParameter->wServiceModule != SERVICE_MODULE_NONE)
	{
		//查找服务
		CGlobalServiceInfo * pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(pBindParameter->wServiceModule, pBindParameter->wServiceID);		
		if (pGlobalServiceInfo != NULL)
		{
			//变量定义
			CMD_CS_S_ServiceRemove ServiceRemove;
			ZeroMemory(&ServiceRemove, sizeof(ServiceRemove));

			//构造结构
			ServiceRemove.wServiceID = pBindParameter->wServiceID;			
			ServiceRemove.wServiceModule = pBindParameter->wServiceModule;
			ServiceRemove.wServicePort = pGlobalServiceInfo->m_ServiceItem.wServicePort;

			//发送命令
			if (pBindParameter->wServiceModule == SERVICE_MODULE_GAME)
			{
				m_pITCPNetworkEngine->SendDataBatch(MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVER_REMOVE, &ServiceRemove, sizeof(ServiceRemove), 0L);
			}
			else
			{
				m_pITCPNetworkEngine->SendDataBatch(MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVICE_REMOVE, &ServiceRemove, sizeof(ServiceRemove), 0L);
			}			
		}

		//游戏服务
		if (pBindParameter->wServiceModule == SERVICE_MODULE_GAME)
		{
			//删除缓存
			auto pGlobalServerInfo = (CGlobalServerInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GAME, pBindParameter->wServiceID);
			if (pGlobalServerInfo != NULL)
			{
				UpdateServerItem(pGlobalServerInfo, UPDATE_KIND_DELETE);
			}

			//删除房间
			m_GlobalInfoManager.DeleteServerInfo(pBindParameter->wServiceID);
		}

		//登录服务
		else if (pBindParameter->wServiceModule == SERVICE_MODULE_LOGON)
		{
			//删除登录
			m_GlobalInfoManager.DeleteLogonInfo(pBindParameter->wServiceID);
		}
		else
		{
			m_GlobalInfoManager.DeleteServiceInfo(pBindParameter->wServiceModule, pBindParameter->wServiceID);
		}
	}
		
	//清除信息
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return true;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GP_UNLOCK_PLAYER:		//解锁玩家
	{
		return OnDBUnlockPlayer(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_CONFIG_FINISH:		//配置完成
	{
		return OnDBConfigFinish(dwContextID, dwTokenID, pData, wDataSize);
	}	
	case DBO_GP_UPDATA_RANKING_LIST: //更新榜单
	{
		return OnDBUpdateRankingList(dwContextID, dwTokenID, pData, wDataSize);
	}
	}

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason)
{
	return false;
}

//连接事件
bool CAttemperEngineSink::OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode)
{
	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	//错误断言
	ASSERT(FALSE);

	return true;
}

//注册服务
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_REGISTER_SERVICE:	//注册服务
	{
		//校验数据
		ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterService));
		if (wDataSize != sizeof(CMD_CS_C_RegisterService)) return false;

		//提取数据
		CMD_CS_C_RegisterService * pRegisterService = (CMD_CS_C_RegisterService *)pData;

		//变量定义
		CMD_CS_S_RegisterFailure RegisterFailure;
		ZeroMemory(&RegisterFailure, sizeof(RegisterFailure));

		//查询服务
		auto pServiceAttrib = CServiceAttribManager::SearchServiceAttrib(pRegisterService->wServiceModule);
		if (pServiceAttrib == NULL)
		{
			//设置变量
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), L"系统未查询到模块标识为[%d]的服务信息，注册失败", pRegisterService->wServiceModule);
		}

		//校验数目
		if (RegisterFailure.lErrorCode == 0 && m_GlobalInfoManager.GetServiceItemCount(pRegisterService->wServiceModule) >= pServiceAttrib->dwMaxInstanceNum)
		{
			//设置变量
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), L"系统中已注册的[%s]实例数目已达到%d个，注册失败！", pServiceAttrib->szServiceName, pServiceAttrib->dwMaxInstanceNum);
		}

		//查找房间
		if (RegisterFailure.lErrorCode == 0 && m_GlobalInfoManager.SearchServiceInfo(pRegisterService->wServiceModule, pRegisterService->wServiceID) != NULL)
		{
			//设置变量
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), TEXT("系统中已存在相同标识的[%s]，注册失败"), pServiceAttrib->szServiceName);
		}

		//注册失败
		if (RegisterFailure.lErrorCode != 0)
		{
			//发送消息
			WORD wStringSize = CountStringBuffer(RegisterFailure.szDescribeString);
			WORD wSendSize = sizeof(RegisterFailure) - sizeof(RegisterFailure.szDescribeString) + wStringSize;
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_FAILURE, &RegisterFailure, wSendSize, NULL, 0);

			//中断网络
			m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

			return true;
		}

		//设置绑定
		WORD wBindIndex = LOWORD(dwSocketID);
		(m_pBindParameter + wBindIndex)->wServiceID = pRegisterService->wServiceID;
		(m_pBindParameter + wBindIndex)->wServiceModule = pRegisterService->wServiceModule;

		//变量定义
		tagServiceItem ServiceItem;
		ZeroMemory(&ServiceItem, sizeof(ServiceItem));

		//构造数据
		ServiceItem.wServiceID = pRegisterService->wServiceID;
		ServiceItem.wServiceModule = pRegisterService->wServiceModule;
		ServiceItem.wServicePort = pRegisterService->wServicePort;
		ServiceItem.wMaxConnect= pRegisterService->wMaxConnect;
		ServiceItem.dwServiceAddr = pRegisterService->dwServiceAddr;			
		ServiceItem.wServiceKindID = pRegisterService->wServiceKindID;
		StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), pServiceAttrib->szServiceName);
		StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain), pRegisterService->szServiceDomain);

		//注册服务
		m_GlobalInfoManager.ActiveServiceInfo(dwSocketID, ServiceItem);

		//群发服务
		m_pITCPNetworkEngine->SendDataBatch(MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVICE_INSERT, &ServiceItem, sizeof(ServiceItem), 0L);

		//网关服务
		if (pRegisterService->wServiceModule == SERVICE_MODULE_GATE)
		{
			//发送服务属性
			SendServiceAttrib(dwSocketID);

			//查找代理
			CGlobalAgentInfo* pGlobalAgentItem = (CGlobalAgentInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_AGENT, ServiceItem.wServiceID);
			if (pGlobalAgentItem != NULL && pGlobalAgentItem->m_AgentItem.wAgentKind == AGENT_KIND_GATE)
			{
				//构造结构
				if (pGlobalAgentItem->GetServicePortCount() == 0)
				{
					tagPortPair PortPair;
					ZeroMemory(&PortPair, sizeof(PortPair));

					//端口映射
					PortPair.wForwardPort = ServiceItem.wServicePort;
					PortPair.wServicePort = ServiceItem.wServicePort;

					//添加映射
					pGlobalAgentItem->InsertServicePort(&PortPair);
				}

				//转发子项
				tagForwardItem ForwardItem;
				ZeroMemory(&ForwardItem, sizeof(ForwardItem));

				//设置标识
				ForwardItem.wServiceID = pGlobalAgentItem->GetServiceID();

				//发送消息
				SendDataToService(SERVICE_MODULE_AGENT, ForwardItem, MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVICE_INSERT, &ServiceItem, sizeof(ServiceItem));
			}
		}	

		//推送服务
		PushServiceListItem(pRegisterService->wServiceMask, dwSocketID, pRegisterService->wServiceModule == SERVICE_MODULE_GATE);

		//发送日志
		if(pRegisterService->wServiceModule != SERVICE_MODULE_LOGGER)
			PushServiceListItem(SERVICE_MASK_LOGGER, dwSocketID, false);

		//登录服务
		if (pRegisterService->wServiceModule == SERVICE_MODULE_LOGON)
		{
			SendGameJackpot(dwSocketID);
		}

		//注册成功
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_SUCCESS, NULL, 0);

		//群发设置
		m_pITCPNetworkEngine->AllowBatchSend(dwSocketID, true, 0L);

		//汇总通知
		if (pRegisterService->wServiceModule == SERVICE_MODULE_GAME || pRegisterService->wServiceModule == SERVICE_MODULE_LOGON || pRegisterService->wServiceModule == SERVICE_MODULE_MESSAGE)
		{
			if (m_wCollectItem == INVALID_WORD)
			{
				m_wCollectItem = wBindIndex;
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_USER_COLLECT, SUB_CS_S_COLLECT_USER, NULL, 0);
			}
			else m_WaitCollectItemArray.Add(wBindIndex);
		}	

		return true;
	}	
	case SUB_CS_C_REGISTER_AGENT:	//注册代理
	{
		//效验数据
		ASSERT (wDataSize >= sizeof(CMD_CS_C_RegisterAgent));
		if (wDataSize < sizeof(CMD_CS_C_RegisterAgent)) return false;

		//消息定义
		CMD_CS_C_RegisterAgent * pRegisterAgent = (CMD_CS_C_RegisterAgent *)pData;

		//变量定义
		CMD_CS_S_RegisterFailure RegisterFailure;
		ZeroMemory(&RegisterFailure, sizeof(RegisterFailure));

		//查询服务
		auto pServiceAttrib = CServiceAttribManager::SearchServiceAttrib(SERVICE_MODULE_AGENT);
		if (pServiceAttrib == NULL)
		{
			//设置变量
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), L"系统未查询到模块标识为[%d]的服务信息，注册失败", SERVICE_MODULE_AGENT);
		}

		//校验数目
		if (RegisterFailure.lErrorCode == 0 && m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_AGENT) >= pServiceAttrib->dwMaxInstanceNum)
		{
			//设置变量
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), L"系统中[%s]注册的实例数目已达到%d个，注册失败！", pServiceAttrib->szServiceName, pServiceAttrib->dwMaxInstanceNum);
		}

		//查找房间
		if (RegisterFailure.lErrorCode == 0 && m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_AGENT, pRegisterAgent->wAgentID) != NULL)
		{
			//设置变量
			RegisterFailure.lErrorCode = -1;
			StringCchCopy(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), TEXT("系统中已存在相同标识的代理服务，游戏服务注册失败"));
		}

		//注册失败
		if (RegisterFailure.lErrorCode != 0)
		{
			//发送消息
			WORD wStringSize = CountStringBuffer(RegisterFailure.szDescribeString);
			WORD wSendSize = sizeof(RegisterFailure) - sizeof(RegisterFailure.szDescribeString) + wStringSize;
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_FAILURE, &RegisterFailure, wSendSize, NULL, 0);

			//中断网络
			m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

			return true;
		}				

		//设置绑定
		WORD wBindIndex = LOWORD(dwSocketID);
		(m_pBindParameter + wBindIndex)->wServiceModule = SERVICE_MODULE_AGENT;
		(m_pBindParameter + wBindIndex)->wServiceID = pRegisterAgent->wAgentID;

		//变量定义
		tagServiceItem ServiceItem;
		ZeroMemory(&ServiceItem, sizeof(ServiceItem));

		//构造数据
		ServiceItem.wServiceID = pRegisterAgent->wAgentID;
		ServiceItem.wServiceModule = SERVICE_MODULE_AGENT;
		ServiceItem.wMaxConnect = pRegisterAgent->wMaxConnect;
		ServiceItem.dwServiceAddr = pRegisterAgent->dwServiceAddr;			
		StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), TEXT("代理服务器"));
		StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain), pRegisterAgent->szServiceDomain);

		//变量定义
		tagAgentItem AgentItem;
		ZeroMemory(&AgentItem, sizeof(AgentItem));

		//构造数据
		AgentItem.wAgentKind= pRegisterAgent->wAgentKind;

		//注册代理
		if (m_GlobalInfoManager.ActiveAgentInfo(dwSocketID, ServiceItem, AgentItem) == false)
		{
			return false;
		}

		//查询代理
		CGlobalAgentInfo *pGlobalAgentItem = (CGlobalAgentInfo *)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_AGENT,ServiceItem.wServiceID);
		if (pGlobalAgentItem != NULL)
		{
			//获取端口
			tagPortPair * pPortPair=(tagPortPair *)(pRegisterAgent + 1);
			WORD wPortCount=(wDataSize-sizeof(CMD_CS_C_RegisterAgent))/sizeof(tagPortPair);

			//插入端口
			for (int i=0; i<wPortCount; i++)
			{
				pGlobalAgentItem->InsertServicePort(pPortPair++);
			}
		}

		//网关代理
		if (AgentItem.wAgentKind==AGENT_KIND_GATE)
		{
			//发送网关
			SendServiceListItem(SERVICE_MODULE_GATE, dwSocketID, false);

			//添加端口
			if (pGlobalAgentItem->GetServicePortCount()==0)
			{
				//查找网关
				CGlobalServiceInfo * pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GATE, ServiceItem.wServiceID);
				if (pGlobalServiceInfo != NULL)
				{
					//服务信息
					tagServiceItem * pServiceItem = &pGlobalServiceInfo->m_ServiceItem;

					//构造结构
					tagPortPair PortPair;
					ZeroMemory(&PortPair, sizeof(PortPair));

					//端口映射
					PortPair.wForwardPort = pServiceItem->wServicePort;
					PortPair.wServicePort = pServiceItem->wServicePort;

					//添加映射
					pGlobalAgentItem->InsertServicePort(&PortPair);						
				}
			}
		}

		//注册成功
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_SUCCESS,NULL,0);

		//群发设置
		m_pITCPNetworkEngine->AllowBatchSend(dwSocketID, true, 0L);				

		return true;
	}
	case SUB_CS_C_REGISTER_SERVER:	//注册房间
	{
		//效验数据
		ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterServer));
		if (wDataSize!=sizeof(CMD_CS_C_RegisterServer)) return false;

		//消息定义
		CMD_CS_C_RegisterServer * pRegisterServer=(CMD_CS_C_RegisterServer *)pData;

		//变量定义
		CMD_CS_S_RegisterFailure RegisterFailure;
		ZeroMemory(&RegisterFailure, sizeof(RegisterFailure));

		//查询服务
		auto pServiceAttrib = CServiceAttribManager::SearchServiceAttrib(SERVICE_MODULE_GAME);
		if (pServiceAttrib == NULL)
		{
			//设置变量
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), L"系统未查询到模块标识为[%d]的服务信息，注册失败", SERVICE_MODULE_GAME);
		}

		//校验数目
		if (RegisterFailure.lErrorCode==0 && m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_GAME) >= pServiceAttrib->dwMaxInstanceNum)
		{
			//设置变量
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), L"系统中[%s]注册的实例数目已达到%d个，注册失败！", pServiceAttrib->szServiceName, pServiceAttrib->dwMaxInstanceNum);
		}

		//查找房间
		if (RegisterFailure.lErrorCode == 0 && m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GAME,pRegisterServer->wServerID)!=NULL)
		{
			//设置变量
			RegisterFailure.lErrorCode=-1;
			StringCchCopy(RegisterFailure.szDescribeString,CountArray(RegisterFailure.szDescribeString), TEXT("已经存在相同标识的游戏服务，游戏服务注册失败"));			
		}

		//注册失败
		if (RegisterFailure.lErrorCode!=0)
		{
			//发送消息
			WORD wStringSize = CountStringBuffer(RegisterFailure.szDescribeString);
			WORD wSendSize = sizeof(RegisterFailure) - sizeof(RegisterFailure.szDescribeString) + wStringSize;
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_FAILURE, &RegisterFailure, wSendSize, NULL, 0);

			//中断网络
			m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

			return true;
		}

		//设置绑定
		WORD wBindIndex=LOWORD(dwSocketID);
		(m_pBindParameter+wBindIndex)->wServiceModule=SERVICE_MODULE_GAME;
		(m_pBindParameter+wBindIndex)->wServiceID=pRegisterServer->wServerID;

		//变量定义
		tagServiceItem ServiceItem;
		ZeroMemory(&ServiceItem, sizeof(ServiceItem));

		//构造数据
		ServiceItem.wServiceID = pRegisterServer->wServerID;
		ServiceItem.wServiceModule = SERVICE_MODULE_GAME;
		ServiceItem.wServicePort = pRegisterServer->wServerPort;			
		ServiceItem.dwServiceAddr = pRegisterServer->dwServerAddr;			
		StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), TEXT("游戏服务器"));
		StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain), pRegisterServer->szServerDomain);

		//变量定义
		tagGameServer GameServer;
		ZeroMemory(&GameServer,sizeof(GameServer));

		//构造数据
		GameServer.wModuleID = pRegisterServer->wModuleID;
		GameServer.wKindID=pRegisterServer->wKindID;
		GameServer.wNodeID=pRegisterServer->wNodeID;
		GameServer.wSortID=pRegisterServer->wSortID;
		GameServer.wVisibleMask=pRegisterServer->wVisibleMask;
		GameServer.wServerID=pRegisterServer->wServerID;
		GameServer.wServerPort=pRegisterServer->wServerPort;
		GameServer.wServerType=pRegisterServer->wServerType;			
		GameServer.wServerLevel = pRegisterServer->wServerLevel;
		GameServer.wChairCount = pRegisterServer->wChairCount;
		GameServer.dwMaxPlayer=pRegisterServer->dwMaxPlayer;
		GameServer.dwOnLineCount=pRegisterServer->dwOnLineCount;
		GameServer.dwServerRule=pRegisterServer->dwServerRule;
		GameServer.dwServerAddr=pRegisterServer->dwServerAddr;	
		StringCchCopy(GameServer.szServerName,CountArray(GameServer.szServerName), pRegisterServer->szServerName);
		StringCchCopy(GameServer.szServerDomain,CountArray(GameServer.szServerDomain), pRegisterServer->szServerDomain);

		//分数配置
		GameServer.lCellScore=pRegisterServer->lCellScore;
		GameServer.lMinEnterScore=pRegisterServer->lMinEnterScore;
		GameServer.lMaxEnterScore=pRegisterServer->lMaxEnterScore;

		//注册房间
		m_GlobalInfoManager.ActiveServerInfo(dwSocketID, ServiceItem, GameServer);

		//群发房间
		//m_pITCPNetworkEngine->SendDataBatch(MDM_CS_COMMON_SERVICE,SUB_CS_S_SERVICE_INSERT,&ServiceItem,sizeof(ServiceItem),0L);

		//群发房间
		m_pITCPNetworkEngine->SendDataBatch(MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVER_INSERT, &GameServer, sizeof(GameServer), 0L);

		//发送监控
		PushServiceListItem(SERVICE_MASK_MONITOR,dwSocketID,false);

		//发送日志
		PushServiceListItem(SERVICE_MASK_LOGGER, dwSocketID,false);

		//发送存储
		PushServiceListItem(SERVICE_MASK_STORE, dwSocketID, false);

		//发送比赛
		if ((pRegisterServer->wServerType&GAME_GENRE_MATCH)!=0)
		{
			//发送比赛
			PushServiceListItem(SERVICE_MASK_MATCH, dwSocketID,false);				
		}

		//发送约战
		if ((pRegisterServer->wServerType&GAME_GENRE_BATTLE)!=0)
		{
			//发送约战
			PushServiceListItem(SERVICE_MASK_BATTLE, dwSocketID,false);
		}

		//发送排位
		if ((pRegisterServer->wServerType & GAME_GENRE_RANKING) != 0)
		{
			//发送排位
			PushServiceListItem(SERVICE_MASK_RANKING, dwSocketID,false);
		}		

		//发送房间
		SendServerListItem(dwSocketID);			

		//注册成功
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_SUCCESS,NULL,0);

		//群发设置
		m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);			

		//汇总通知
		if (m_wCollectItem==INVALID_WORD)
		{
			m_wCollectItem=wBindIndex;
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_USER,NULL,0);
		}
		else m_WaitCollectItemArray.Add(wBindIndex);

		//更新缓存
		auto pGlobalServerInfo = (CGlobalServerInfo *)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GAME, ServiceItem.wServiceID);
		if (pGlobalServerInfo != NULL)
		{
			UpdateServerItem(pGlobalServerInfo,UPDATE_KIND_MODIFY);
		}

		return true;
	}
	case SUB_CS_C_REGISTER_MASTER:  //注册后台
	{
		//效验数据
		ASSERT(wDataSize >= sizeof(CMD_CS_C_RegisterMaster));
		if (wDataSize < sizeof(CMD_CS_C_RegisterMaster)) return false;

		//消息定义
		CMD_CS_C_RegisterMaster* pRegisterMaster = (CMD_CS_C_RegisterMaster*)pData;

		//服务模块
		WORD wServiceModule = SERVICE_MODULE_MASTER;

		//变量定义
		CMD_CS_S_RegisterFailure RegisterFailure;
		ZeroMemory(&RegisterFailure, sizeof(RegisterFailure));

		//查询服务
		auto pServiceAttrib = CServiceAttribManager::SearchServiceAttrib(wServiceModule);
		if (pServiceAttrib == NULL)
		{
			//设置变量
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), L"系统未查询到模块标识为[%d]的服务信息，注册失败", wServiceModule);
		}

		//校验数目
		if (RegisterFailure.lErrorCode == 0 && m_GlobalInfoManager.GetServiceItemCount(wServiceModule) >= pServiceAttrib->dwMaxInstanceNum)
		{
			//设置变量
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), L"系统中已注册的[%s]实例数目已达到%d个，注册失败！", pServiceAttrib->szServiceName, pServiceAttrib->dwMaxInstanceNum);
		}

		//查找房间
		if (RegisterFailure.lErrorCode == 0 && m_GlobalInfoManager.SearchServiceInfo(wServiceModule, pRegisterMaster->wServiceID) != NULL)
		{
			//设置变量
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), TEXT("系统中已存在相同标识的[%s]，注册失败"), pServiceAttrib->szServiceName);
		}

		//注册失败
		if (RegisterFailure.lErrorCode != 0)
		{
			//发送消息
			WORD wStringSize = CountStringBuffer(RegisterFailure.szDescribeString);
			WORD wSendSize = sizeof(RegisterFailure) - sizeof(RegisterFailure.szDescribeString) + wStringSize;
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_FAILURE, &RegisterFailure, wSendSize, NULL, 0);

			//中断网络
			m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

			return true;
		}

		//设置绑定
		WORD wBindIndex = LOWORD(dwSocketID);
		(m_pBindParameter + wBindIndex)->wServiceModule = SERVICE_MODULE_MASTER;
		(m_pBindParameter + wBindIndex)->wServiceID = pRegisterMaster->wServiceID;

		//变量定义
		tagServiceItem ServiceItem;
		ZeroMemory(&ServiceItem, sizeof(ServiceItem));

		//构造数据
		ServiceItem.wServiceID = pRegisterMaster->wServiceID;
		ServiceItem.wServiceModule = SERVICE_MODULE_MASTER;
		StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), TEXT("后台服务"));		

		//注册后台
		if (m_GlobalInfoManager.ActiveServiceInfo(dwSocketID, ServiceItem) == false)
		{
			return false;
		}	

		//注册成功
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_SUCCESS, NULL, 0);

		return true;
	}
	}

	return false;
}

//服务状态
bool CAttemperEngineSink::OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_SERVICE_ONLINE:	 //服务人数
	{
		//效验数据
		ASSERT (wDataSize==sizeof(CMD_CS_C_ServiceOnline));
		if (wDataSize!=sizeof(CMD_CS_C_ServiceOnline)) return false;

		//消息定义
		CMD_CS_C_ServiceOnline * pServiceOnline =(CMD_CS_C_ServiceOnline *)pData;

		//获取参数
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//游戏模块
		if (pBindParameter->wServiceModule == SERVICE_MODULE_GAME)
		{
			//查找房间
			WORD wServerID = pBindParameter->wServiceID;
			CGlobalServerInfo * pGlobalServerInfo = (CGlobalServerInfo *)m_GlobalInfoManager.SearchServiceInfo(pBindParameter->wServiceModule, wServerID);

			//设置人数
			if (pGlobalServerInfo != NULL)
			{
				//变量定义
				CMD_CS_S_ServiceOnline ServiceOnline;
				ZeroMemory(&ServiceOnline, sizeof(ServiceOnline));

				//设置变量
				pGlobalServerInfo->m_GameServer.dwOnLineCount = pServiceOnline->wOnlineCount;

				//设置变量
				ServiceOnline.wServiceID = wServerID;
				ServiceOnline.wServiceModule = SERVICE_MODULE_GAME;
				ServiceOnline.wOnlineCount = pServiceOnline->wOnlineCount;

				//发送通知
				SendDataToAllService(SERVICE_MODULE_LOGON,0, MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVICE_ONLINE, &ServiceOnline, sizeof(ServiceOnline));

				//发送通知
				SendDataToAllService(SERVICE_MODULE_GATE,0, MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVICE_ONLINE, &ServiceOnline, sizeof(ServiceOnline));
			}
		}
		else
		{
			//查找网关
			CGlobalServiceInfo* pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(pBindParameter->wServiceModule, pBindParameter->wServiceID);
			if (pGlobalServiceInfo != NULL)
			{
				pGlobalServiceInfo->m_wOnlineCount = pServiceOnline->wOnlineCount;
			}
		}

		return true;
	}
	case SUB_CS_C_REPORT_JACKPOT:	 //上报奖池
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_CS_C_ReportJackpot));
		if (wDataSize != sizeof(CMD_CS_C_ReportJackpot)) return false;

		//消息定义
		auto pReportJackpot = (CMD_CS_C_ReportJackpot*)pData;

		//获取参数
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//连接效验
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_MONITOR);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_MONITOR) return false;

		//查找房间
		auto pGlobalMonitorInfo = (CGlobalMonitorInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_MONITOR, pBindParameter->wServiceID);

		//校验监控
		if (pGlobalMonitorInfo != NULL)
		{
			//查找奖池
			auto pGlobalGameStock = pGlobalMonitorInfo->m_StockGameStock.SearchStockItem(pReportJackpot->wKindID);
			if (pGlobalGameStock == NULL)
			{
				pGlobalGameStock = pGlobalMonitorInfo->m_StockGameStock.CreateStockItem(pReportJackpot->wKindID);
			}

			if (pGlobalGameStock != NULL)
			{
				//更新彩金	
				pGlobalGameStock->m_wKindID = pReportJackpot->wKindID;
				pGlobalGameStock->m_lJackpotScore = pReportJackpot->lJackpotScore;

				//广播消息
				CMD_CS_S_GameJackpot GameJackpot;
				ZeroMemory(&GameJackpot, sizeof(GameJackpot));

				//设置奖池
				GameJackpot.wKindID = pReportJackpot->wKindID;
				GameJackpot.lJackpotScore = pReportJackpot->lJackpotScore;

				//发送通知							
				m_pITCPNetworkEngine->SendDataBatch(MDM_CS_COMMON_SERVICE, SUB_CS_S_GAME_JACKPOT, &GameJackpot, sizeof(GameJackpot), 0L);
			}				
		}

		return true;
	}
	case SUB_CS_C_SERVER_MODIFY:	 //房间修改
	{
		//效验数据
		ASSERT(wDataSize==sizeof(CMD_CS_C_ServerModify));
		if (wDataSize!=sizeof(CMD_CS_C_ServerModify)) return false;

		//消息定义
		CMD_CS_C_ServerModify * pServerModify=(CMD_CS_C_ServerModify *)pData;

		//获取参数
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//连接效验
		ASSERT(pBindParameter->wServiceModule== SERVICE_MODULE_GAME);
		if (pBindParameter->wServiceModule!= SERVICE_MODULE_GAME) return false;

		//查找房间
		WORD wServerID=pBindParameter->wServiceID;
		CGlobalServerInfo * pGlobalServerInfo=(CGlobalServerInfo *)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GAME,wServerID);

		//房间修改
		if (pGlobalServerInfo!=NULL)
		{
			//变量定义
			CMD_CS_S_ServerModify ServerModify;
			ZeroMemory(&ServerModify,sizeof(ServerModify));

			//设置变量
			pGlobalServerInfo->m_ServiceItem.wServicePort = pServerModify->wServerPort;
			pGlobalServerInfo->m_ServiceItem.dwServiceAddr = pServerModify->dwServerAddr;
			StringCchCopy(pGlobalServerInfo->m_ServiceItem.szServiceDomain, CountArray(pGlobalServerInfo->m_ServiceItem.szServiceDomain), pServerModify->szServerDomain);

			//设置变量
			pGlobalServerInfo->m_GameServer.wSortID=pServerModify->wSortID;
			pGlobalServerInfo->m_GameServer.wKindID=pServerModify->wKindID;
			pGlobalServerInfo->m_GameServer.wNodeID=pServerModify->wNodeID;
			pGlobalServerInfo->m_GameServer.wServerPort=pServerModify->wServerPort;		
			pGlobalServerInfo->m_GameServer.wServerLevel =pServerModify->wServerLevel;
			pGlobalServerInfo->m_GameServer.dwServerRule=pServerModify->dwServerRule;
			pGlobalServerInfo->m_GameServer.dwMaxPlayer=pServerModify->dwMaxPlayer;
			pGlobalServerInfo->m_GameServer.dwOnLineCount=pServerModify->dwOnLineCount;
			pGlobalServerInfo->m_GameServer.dwServerAddr = pServerModify->dwServerAddr;
			StringCchCopy(pGlobalServerInfo->m_GameServer.szServerName,LEN_SERVER, pServerModify->szServerName);
			StringCchCopy(pGlobalServerInfo->m_GameServer.szServerDomain,CountArray(pGlobalServerInfo->m_GameServer.szServerDomain), pServerModify->szServerDomain);

			//积分信息
			pGlobalServerInfo->m_GameServer.lCellScore = pServerModify->lCellScore;
			pGlobalServerInfo->m_GameServer.lMinEnterScore = pServerModify->lMinEnterScore;
			pGlobalServerInfo->m_GameServer.lMaxEnterScore = pServerModify->lMaxEnterScore;

			//设置变量
			ServerModify.wServerID=wServerID;
			ServerModify.wSortID=pServerModify->wSortID;
			ServerModify.wKindID=pServerModify->wKindID;
			ServerModify.wNodeID=pServerModify->wNodeID;
			ServerModify.wServerPort=pServerModify->wServerPort;
			ServerModify.wServerType=pServerModify->wServerType;
			ServerModify.wServerLevel =pServerModify->wServerLevel;
			ServerModify.dwMaxPlayer=pServerModify->dwMaxPlayer;
			ServerModify.dwOnLineCount=pServerModify->dwOnLineCount;
			ServerModify.dwServerAddr=pServerModify->dwServerAddr;
			ServerModify.dwServerRule=pServerModify->dwServerRule;
			StringCchCopy(ServerModify.szServerName,CountArray(ServerModify.szServerName), pServerModify->szServerName);
			StringCchCopy(ServerModify.szServerDomain,CountArray(ServerModify.szServerDomain), pServerModify->szServerDomain);

			//积分信息
			ServerModify.lCellScore = pServerModify->lCellScore;
			ServerModify.lMinEnterScore = pServerModify->lMinEnterScore;
			ServerModify.lMaxEnterScore = pServerModify->lMaxEnterScore;

			//发送通知							
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_COMMON_SERVICE,SUB_CS_S_SERVER_MODIFY,&ServerModify,sizeof(ServerModify),0L);
		}

		return true;
	}		
	case SUB_CS_C_QUERY_ROUTE_ITEM:	 //查询路由
	{
		//效验数据
		ASSERT (wDataSize == sizeof(CMD_CS_C_QueryRouteItem));
		if (wDataSize != sizeof(CMD_CS_C_QueryRouteItem)) return false;

		//消息定义
		auto pQueryRouteItem = (CMD_CS_C_QueryRouteItem*)pData;

		//查询结果
		CMD_CS_S_QueryRouteResult QueryRouteResult = {};
		QueryRouteResult.dwRouteID = pQueryRouteItem->dwRouteID;
		QueryRouteResult.dwTokenID = pQueryRouteItem->dwTokenID;
		QueryRouteResult.wServiceModule = pQueryRouteItem->wServiceModule;

		//查找服务
		auto pGlobalServiceInfo = m_GlobalInfoManager.LookupService(pQueryRouteItem->wServiceModule, pQueryRouteItem->wServiceKind, pQueryRouteItem->dwRouteID);
		if (pGlobalServiceInfo == NULL)
		{
			//分配服务
			pGlobalServiceInfo = m_GlobalInfoManager.AssignService(pQueryRouteItem->wServiceModule, pQueryRouteItem->wServiceKind, pQueryRouteItem->dwRouteID);
		}

		if (pGlobalServiceInfo != NULL)
		{
			//查询路由
			auto pRouteItem = pGlobalServiceInfo->QueryRouteItem(pQueryRouteItem->dwRouteID);
			if (pRouteItem != NULL)
			{
				QueryRouteResult.wServiceID = pGlobalServiceInfo->GetServiceID();

				//激活路由
				if (pRouteItem->cbRouteState == ROUTE_STATE_NULL)
				{
					//构造结构
					CMD_CS_S_ActiveRouteItem ActiveRouteItem = {};
					ActiveRouteItem.dwRouteID = pQueryRouteItem->dwRouteID;

					//发送消息
					if (SendDataToService(pQueryRouteItem->wServiceModule, QueryRouteResult.wServiceID, MDM_CS_COMMON_SERVICE, SUB_CS_S_ACTIVE_ROUTE_ITEM, &ActiveRouteItem, sizeof(ActiveRouteItem)))
					{
						//更新状态
						pRouteItem->cbRouteState = ROUTE_STATE_ACTIVE;
					}
				}
			}			
		}

		//回复消息
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_QUERY_ROUTE_RESULT, &QueryRouteResult, sizeof(QueryRouteResult), &pQueryRouteItem->dwTokenID, 1);

		return true;
	}
	case SUB_CS_C_ACTIVE_ROUTE_ITEM: //激活路由
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_CS_C_ActiveRouteItem));
		if (wDataSize != sizeof(CMD_CS_C_ActiveRouteItem)) return false;

		//消息定义
		auto pActiveRouteItem = (CMD_CS_C_ActiveRouteItem*)pData;

		//映射服务
		auto pGlobalServiceInfo = m_GlobalInfoManager.LookupService(pActiveRouteItem->wServiceModule, pActiveRouteItem->wServiceKind, pActiveRouteItem->dwRouteID);
		if (pGlobalServiceInfo == NULL)
		{
			//分配服务
			pGlobalServiceInfo = m_GlobalInfoManager.AssignService(pActiveRouteItem->wServiceModule, pActiveRouteItem->wServiceKind, pActiveRouteItem->dwRouteID);
		}

		if (pGlobalServiceInfo!=NULL)
		{
			//查询路由
			auto pRouteItem = pGlobalServiceInfo->QueryRouteItem(pActiveRouteItem->dwRouteID);
			if (pRouteItem != NULL)
			{
				//激活路由
				if (pRouteItem->cbRouteState == ROUTE_STATE_NULL)
				{
					//构造结构
					CMD_CS_S_ActiveRouteItem ActiveRouteItem = {};
					ActiveRouteItem.dwRouteID = pActiveRouteItem->dwRouteID;

					//发送消息
					if (SendDataToService(pActiveRouteItem->wServiceModule, pGlobalServiceInfo->GetServiceID(), MDM_CS_COMMON_SERVICE, SUB_CS_S_ACTIVE_ROUTE_ITEM, &ActiveRouteItem, sizeof(ActiveRouteItem)))
					{
						//更新状态
						pRouteItem->cbRouteState = ROUTE_STATE_ACTIVE;
					}
				}
			}
		}
		else
		{
			CTraceService::TraceString(TEXT("服务映射失败：ServiceModule=%d,ServiceKind=%d,RouteID=%d"),TraceLevel_Exception, pActiveRouteItem->wServiceModule, pActiveRouteItem->wServiceKind, pActiveRouteItem->dwRouteID);
		}

		return true;
	}
	case SUB_CS_C_APPEND_ROUTE_ITEM: //添加路由
	{
		//效验数据
		ASSERT((wDataSize - sizeof(CMD_CS_C_AppendRouteItem)) % sizeof(tagRouteItem) == 0);
		if ((wDataSize - sizeof(CMD_CS_C_AppendRouteItem)) % sizeof(tagRouteItem) != 0) return false;

		//获取参数
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//提取数据
		auto pAppendRouteItem = (CMD_CS_C_AppendRouteItem*)pData;

		//路由子项
		auto pRouteItem = (tagRouteItem*)(pAppendRouteItem + 1);
		auto wItemCount = (wDataSize - sizeof(CMD_CS_C_AppendRouteItem)) / sizeof(tagRouteItem);

		for (int i = 0; i < wItemCount; i++)
		{
			//查找服务
			auto pGlobalServiceInfo = m_GlobalInfoManager.LookupService(pAppendRouteItem->wServiceModule, pAppendRouteItem->wServiceKind, pRouteItem->dwRouteID);
			if (pGlobalServiceInfo == NULL)
			{
				//分配服务
				pGlobalServiceInfo = m_GlobalInfoManager.AssignService(pAppendRouteItem->wServiceModule, pAppendRouteItem->wServiceKind, pRouteItem->dwRouteID, pBindParameter->wServiceID);
			}

			if (pGlobalServiceInfo != NULL)
			{
				//查询路由
				auto pQueryRouteItem = pGlobalServiceInfo->QueryRouteItem(pRouteItem->dwRouteID);
				if (pQueryRouteItem != NULL)
				{
					pQueryRouteItem->cbRouteState = pRouteItem->cbRouteState;
				}
			}

			pRouteItem++;
		}		

		//广播网关
		if (pAppendRouteItem->bTheLast)
		{
			//缓冲定义
			BYTE cbBuffer[SOCKET_PACKET];

			//枚举变量
			tagRouteItem* pRouteItem = NULL;
			auto wPacketSize = sizeof(CMD_CS_S_AppendRouteItem);
			auto pAppendRouteItem1 = (CMD_CS_S_AppendRouteItem*)cbBuffer;			

			//设置变量			
			pAppendRouteItem1->bTheFirst = true;
			pAppendRouteItem1->wServiceID = pBindParameter->wServiceID;
			pAppendRouteItem1->wServiceModule = pAppendRouteItem->wServiceModule;

			//查找服务
			auto pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(pAppendRouteItem->wServiceModule, pBindParameter->wServiceID);
			if (pGlobalServiceInfo != NULL)
			{
				//变量定义
				POSITION Position = NULL;

				do
				{
					pRouteItem = pGlobalServiceInfo->EnumRouteItem(Position);
					if (pRouteItem == NULL) break;

					//拷贝数据
					if (pRouteItem->cbRouteState == ROUTE_STATE_SERVICE)
					{
						CopyMemory(&cbBuffer[wPacketSize], pRouteItem, sizeof(tagRouteItem));
						wPacketSize += sizeof(tagRouteItem);
					}

					//移除判断
					if (wPacketSize + sizeof(tagRouteItem) > CountArray(cbBuffer))
					{
						m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_APPEND_ROUTE_ITEM, cbBuffer, wPacketSize, NULL, 0);

						//设置变量
						pAppendRouteItem1->bTheFirst = false;
						wPacketSize = sizeof(CMD_CS_S_AppendRouteItem);
					}

				} while (Position != NULL);

				//发送判断
				if (wPacketSize > sizeof(CMD_CS_S_AppendRouteItem))
				{
					m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_APPEND_ROUTE_ITEM, cbBuffer, wPacketSize, NULL, 0);
				}
			}
		}

		return true;
	}
	case SUB_CS_C_UPDATE_ROUTE_ITEM: //更新路由
	{
		//效验数据
		ASSERT (wDataSize == sizeof(CMD_CS_C_UpdateRouteItem));
		if (wDataSize != sizeof(CMD_CS_C_UpdateRouteItem)) return false;

		//消息定义
		auto pUpdateRouteItem = (CMD_CS_C_UpdateRouteItem*)pData;

		//获取参数
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//查询服务
		auto pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(pBindParameter->wServiceModule, pBindParameter->wServiceID);
		if (pGlobalServiceInfo != NULL)
		{
			//查询路由
			auto pRouteItem = pGlobalServiceInfo->QueryRouteItem(pUpdateRouteItem->dwRouteID);
			if (pRouteItem != NULL)
			{
				pRouteItem->cbRouteState = pUpdateRouteItem->cbRouteState;
				if (pRouteItem->cbRouteState != ROUTE_STATE_SERVICE)
				{
					pGlobalServiceInfo->RemoveRouteItem(pUpdateRouteItem->dwRouteID);

					//构造结构
					CMD_CS_S_RemoveRouteItem RemoveRouteItem = {};

					//设置变量
					RemoveRouteItem.dwRouteID = pUpdateRouteItem->dwRouteID;
					RemoveRouteItem.wServiceID = pGlobalServiceInfo->GetServiceID();
					RemoveRouteItem.wServiceModule = pGlobalServiceInfo->GetServiceModule();

					//发送数据
					SendDataToAllService(SERVICE_MODULE_GATE, 0, MDM_CS_COMMON_SERVICE, SUB_CS_S_REMOVE_ROUTE_ITEM, &RemoveRouteItem, sizeof(RemoveRouteItem));
				}
				else
				{
					//缓存定义
					BYTE cbBuffer[1024] = {};
					auto pAppendRouteItem = (CMD_CS_S_AppendRouteItem*)cbBuffer;

					//设置变量
					pAppendRouteItem->wServiceID = pBindParameter->wServiceID;
					pAppendRouteItem->wServiceModule = pBindParameter->wServiceModule;

					//拷贝数据
					CopyMemory(pAppendRouteItem+1, pRouteItem, sizeof(tagRouteItem));

					//发送数据
					SendDataToAllService(SERVICE_MODULE_GATE,0,MDM_CS_COMMON_SERVICE, SUB_CS_S_APPEND_ROUTE_ITEM, cbBuffer,sizeof(CMD_CS_S_AppendRouteItem)+sizeof(tagRouteItem));
				}
			}
		}		

		return true;
	}
	case SUB_CS_C_REMOVE_ROUTE_ITEM: //移除路由
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_CS_C_RemoveRouteItem));
		if (wDataSize != sizeof(CMD_CS_C_RemoveRouteItem)) return false;

		//消息定义
		auto pRemoveRouteItem = (CMD_CS_C_RemoveRouteItem*)pData;

		//获取参数
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//查询服务
		auto pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(pBindParameter->wServiceModule, pBindParameter->wServiceID);
		if (pGlobalServiceInfo != NULL)
		{
			//查询路由
			pGlobalServiceInfo->RemoveRouteItem(pRemoveRouteItem->dwRouteID);

			//构造结构
			CMD_CS_S_RemoveRouteItem RemoveRouteItem = {};

			//设置变量
			RemoveRouteItem.dwRouteID = pRemoveRouteItem->dwRouteID;
			RemoveRouteItem.wServiceID = pBindParameter->wServiceID;
			RemoveRouteItem.wServiceModule = pBindParameter->wServiceModule;

			//发送消息
			SendDataToAllService(SERVICE_MODULE_GATE, 0, MDM_CS_COMMON_SERVICE, SUB_CS_S_REMOVE_ROUTE_ITEM, &RemoveRouteItem, sizeof(RemoveRouteItem));
		}

		return true;
	}
	}

	return false;
}

//用户处理
bool CAttemperEngineSink::OnTCPNetworkMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_ENTER_SERVER:		//用户进入
	{
		//效验数据
		ASSERT(wDataSize==sizeof(CMD_CS_C_EnterServer));
		if (wDataSize!=sizeof(CMD_CS_C_EnterServer)) return false;

		//消息处理
		CMD_CS_C_EnterServer * pEnterServer=(CMD_CS_C_EnterServer *)pData;
			
		//获取参数
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//连接效验
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_GAME);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_GAME) return false;

		//查找服务
		auto pGlobalServerInfo = (CGlobalServerInfo *)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GAME, pBindParameter->wServiceID);
		if (pGlobalServerInfo == NULL) return false;

		//查找用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pEnterServer->dwUserID);
		if (pGlobalUserInfo == NULL)
		{
			//变量定义
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo, sizeof(GlobalUserInfo));

			//设置变量
			GlobalUserInfo.dwUserID = pEnterServer->dwUserID;
			GlobalUserInfo.dwGameID = pEnterServer->dwGameID;
			GlobalUserInfo.cbGender = pEnterServer->cbGender;
			GlobalUserInfo.wFaceID = pEnterServer->wFaceID;
			GlobalUserInfo.dwCustomID = pEnterServer->dwCustomID;
			StringCchCopy(GlobalUserInfo.szNickName, CountArray(GlobalUserInfo.szNickName), pEnterServer->szNickName);

			//激活用户
			pGlobalUserInfo = m_GlobalInfoManager.ActiveUserInfo(GlobalUserInfo);
			if (pGlobalUserInfo == NULL) return false;
		}

		//绑定服务		
		pGlobalUserInfo->BindServerInfo(pGlobalServerInfo);				

		return true;
	}
	case SUB_CS_C_LEAVE_SERVER:		//用户离开
	{
		//效验数据
		ASSERT(wDataSize==sizeof(CMD_CS_C_LeaveServer));
		if (wDataSize!=sizeof(CMD_CS_C_LeaveServer)) return false;

		//消息处理
		CMD_CS_C_LeaveServer * pLeaveServer=(CMD_CS_C_LeaveServer *)pData;

		//获取参数
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//连接效验
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_GAME);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_GAME) return false;

		//查找服务
		auto pGlobalServerInfo = (CGlobalServerInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GAME, pBindParameter->wServiceID);
		if (pGlobalServerInfo == NULL) return false;

		//查找用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pLeaveServer->dwUserID);
		if (pGlobalUserInfo == NULL) return true;

		//解绑服务
		pGlobalUserInfo->UnBindServerInfo(pGlobalServerInfo);

		//删除用户
		if (pGlobalUserInfo->BindServiceCount() == 0)
		{
			m_GlobalInfoManager.DeleteUserInfo(pLeaveServer->dwUserID);
		}

		return true;
	}
	case SUB_CS_C_ENTER_MESSAGE:	//用户进入
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_CS_C_EnterMessage));
		if (wDataSize != sizeof(CMD_CS_C_EnterMessage)) return false;

		//消息处理
		CMD_CS_C_EnterMessage* pEnterMessage = (CMD_CS_C_EnterMessage*)pData;

		//获取参数
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//连接效验
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_MESSAGE);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_MESSAGE) return false;

		//查找服务
		auto pGlobalMessageInfo = m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_MESSAGE, pBindParameter->wServiceID);
		if (pGlobalMessageInfo == NULL) return false;

		//查找用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pEnterMessage->dwUserID);
		if (pGlobalUserInfo == NULL)
		{
			//变量定义
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo, sizeof(GlobalUserInfo));

			//设置变量
			GlobalUserInfo.dwUserID = pEnterMessage->dwUserID;						

			//激活用户
			pGlobalUserInfo = m_GlobalInfoManager.ActiveUserInfo(GlobalUserInfo);
			if (pGlobalUserInfo == NULL) return false;
		}

		//绑定服务		
		pGlobalUserInfo->BindMessageInfo(pGlobalMessageInfo);

		return true;
	}
	case SUB_CS_C_LEAVE_MESSAGE:	//用户离开
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_CS_C_LeaveMessage));
		if (wDataSize != sizeof(CMD_CS_C_LeaveMessage)) return false;

		//消息处理
		CMD_CS_C_LeaveMessage* pLeaveMessage = (CMD_CS_C_LeaveMessage*)pData;

		//获取参数
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//连接效验
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_MESSAGE);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_MESSAGE) return false;

		//查找服务
		auto pGlobalMessageInfo = m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_MESSAGE, pBindParameter->wServiceID);
		if (pGlobalMessageInfo == NULL) return false;

		//查找用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pLeaveMessage->dwUserID);
		if (pGlobalUserInfo == NULL) return true;

		//解绑服务
		pGlobalUserInfo->UnBindMessageInfo(pGlobalMessageInfo);

		//删除用户
		if (pGlobalUserInfo->BindServiceCount() == 0)
		{
			m_GlobalInfoManager.DeleteUserInfo(pLeaveMessage->dwUserID);
		}

		return true;
	}
	case SUB_CS_C_ENTER_CLUBPLAZA:	//用户进入
	{
		//效验数据
		ASSERT (wDataSize == sizeof(CMD_CS_C_EnterClubPlaza));
		if (wDataSize != sizeof(CMD_CS_C_EnterClubPlaza)) return false;

		//消息处理
		auto pEnterClubPlaza = (CMD_CS_C_EnterClubPlaza*)pData;		

		//获取参数
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//连接效验
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_CLUBPLAZA);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_CLUBPLAZA) return false;

		//查找服务
		auto pGlobalLogonInfo = (CGlobalLogonInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_CLUBPLAZA, pBindParameter->wServiceID);
		if (pGlobalLogonInfo == NULL) return false;

		//查找用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pEnterClubPlaza->dwUserID);
		if (pGlobalUserInfo != NULL)
		{
			//更新状态
			pGlobalUserInfo->m_cbCBUserStatus = US_CB_ONLINE;

			//变量定义
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo, sizeof(GlobalUserInfo));

			//设置变量
			GlobalUserInfo.dwUserID = pGlobalUserInfo->m_dwUserID;
			GlobalUserInfo.dwGameID = pGlobalUserInfo->m_dwGameID;
			GlobalUserInfo.cbGender = pGlobalUserInfo->m_cbGender;			
			StringCchCopy(GlobalUserInfo.szNickName, CountArray(GlobalUserInfo.szNickName), pGlobalUserInfo->m_szNickName);
			StringCchCopy(GlobalUserInfo.szUserHeadUrl, CountArray(GlobalUserInfo.szNickName), pGlobalUserInfo->m_szUserHeadUrl);//kk jia

			//状态信息
			GlobalUserInfo.cbIMUserStatus = pGlobalUserInfo->m_cbIMUserStatus;
			GlobalUserInfo.cbCBUserStatus = pGlobalUserInfo->m_cbCBUserStatus;

			//头像标识
			GlobalUserInfo.wFaceID = pGlobalUserInfo->m_wFaceID;
			GlobalUserInfo.dwCustomID = pGlobalUserInfo->m_dwCustomID;

			//状态信息
			GlobalUserInfo.cbShutUpStatus = pGlobalUserInfo->m_cbShutUpStatus;
			GlobalUserInfo.dwShutUpOverTime = pGlobalUserInfo->m_dwShutUpOverTime;

			//网络信息
			GlobalUserInfo.SocketInfo.wGateID = pGlobalUserInfo->m_wGateID;
			GlobalUserInfo.SocketInfo.dwTokenID = pGlobalUserInfo->m_dwTokenID;

			//广播消息
			SendDataToAllService(SERVICE_MODULE_CLUB, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_CB_USER_ONLINE, &GlobalUserInfo, sizeof(tagGlobalUserInfo));

			//广播消息
			SendDataToAllService(SERVICE_MODULE_CLUBPLAZA, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_CB_USER_ONLINE, &GlobalUserInfo, sizeof(tagGlobalUserInfo));
		}

		return true;
	}
	case SUB_CS_C_LEAVE_CLUBPLAZA:	//用户离开
	{
		//效验数据
		ASSERT (wDataSize == sizeof(CMD_CS_C_LeaveClubPlaza));
		if (wDataSize != sizeof(CMD_CS_C_LeaveClubPlaza)) return false;

		//消息处理
		auto pLeaveClubPlaza = (CMD_CS_C_LeaveClubPlaza*)pData;

		//获取参数
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//连接效验
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_CLUBPLAZA);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_CLUBPLAZA) return false;

		//查找服务
		auto pGlobalLogonInfo = (CGlobalLogonInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_CLUBPLAZA, pBindParameter->wServiceID);
		if (pGlobalLogonInfo == NULL) return false;

		//查找用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pLeaveClubPlaza->dwUserID);
		if (pGlobalUserInfo != NULL)
		{
			//更新状态
			pGlobalUserInfo->m_cbCBUserStatus = US_CB_OFFLINE;

			//变量定义
			CMD_CS_CB_UserOffline UserOffline = {};			
			UserOffline.dwUserID = pLeaveClubPlaza->dwUserID;

			//广播消息
			SendDataToAllService(SERVICE_MODULE_CLUB, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_CB_USER_OFFLINE, &UserOffline, sizeof(UserOffline));

			//广播消息
			SendDataToAllService(SERVICE_MODULE_CLUBPLAZA, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_CB_USER_OFFLINE, &UserOffline, sizeof(UserOffline));
		}

		return true;
	}
	case SUB_CS_C_USER_ONLINE:		//用户上线
	{
		//效验数据
		ASSERT (wDataSize == sizeof(CMD_CS_C_UserOnline));
		if (wDataSize != sizeof(CMD_CS_C_UserOnline)) return false;

		//消息处理
		CMD_CS_C_UserOnline* pUserOnline = (CMD_CS_C_UserOnline*)pData;
		pUserOnline->szNickName[CountArray(pUserOnline->szNickName) - 1] = 0;

		//获取参数
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//连接效验
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_LOGON);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_LOGON) return false;

		//查找服务
		auto pGlobalLogonInfo = (CGlobalLogonInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_LOGON, pBindParameter->wServiceID);
		if (pGlobalLogonInfo == NULL) return false;

		//查找用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pUserOnline->dwUserID);
		if (pGlobalUserInfo == NULL)
		{
			//变量定义
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo, sizeof(GlobalUserInfo));

			//设置变量
			GlobalUserInfo.dwUserID = pUserOnline->dwUserID;
			GlobalUserInfo.dwGameID = pUserOnline->dwGameID;
			GlobalUserInfo.cbGender = pUserOnline->cbGender;
			StringCchCopy(GlobalUserInfo.szNickName, CountArray(GlobalUserInfo.szNickName), pUserOnline->szNickName);

			//头像标识
			GlobalUserInfo.wFaceID = pUserOnline->wFaceID;
			GlobalUserInfo.dwCustomID = pUserOnline->dwCustomID;

			//状态信息
			GlobalUserInfo.cbShutUpStatus = pUserOnline->cbShutUpStatus;
			GlobalUserInfo.dwShutUpOverTime = pUserOnline->dwShutUpOverTime;

			//网络信息
			GlobalUserInfo.SocketInfo.wGateID = pUserOnline->wGateID;
			GlobalUserInfo.SocketInfo.dwTokenID = pUserOnline->dwTokenID;

			//激活用户
			pGlobalUserInfo = m_GlobalInfoManager.ActiveUserInfo(GlobalUserInfo);
			if (pGlobalUserInfo == NULL) return false;	

			//广播消息
			SendDataToAllService(SERVICE_MODULE_CHAT,0, MDM_CS_FORWARD_SERVICE, SUB_CS_IM_USER_ONLINE, &GlobalUserInfo, sizeof(GlobalUserInfo));
		}
		else
		{
			//用户信息
			pGlobalUserInfo->m_dwGameID = pUserOnline->dwGameID;
			pGlobalUserInfo->m_cbGender = pUserOnline->cbGender;
			pGlobalUserInfo->m_wFaceID = pUserOnline->wFaceID;
			pGlobalUserInfo->m_dwCustomID = pUserOnline->dwCustomID;
			StringCchCopy(pGlobalUserInfo->m_szNickName, CountArray(pGlobalUserInfo->m_szNickName), pUserOnline->szNickName);
			StringCchCopy(pGlobalUserInfo->m_szUserHeadUrl, CountArray(pGlobalUserInfo->m_szUserHeadUrl), pUserOnline->szUserHeadUrl
			);//kk jia

			//状态信息
			pGlobalUserInfo->m_cbShutUpStatus = pUserOnline->cbShutUpStatus;
			pGlobalUserInfo->m_dwShutUpOverTime = pUserOnline->dwShutUpOverTime;

			//网络信息
			pGlobalUserInfo->m_wGateID = pUserOnline->wGateID;
			pGlobalUserInfo->m_dwTokenID = pUserOnline->dwTokenID;
		}			

		//绑定服务
		pGlobalUserInfo->BindLogonInfo(pGlobalLogonInfo);

		return true;
	}
	case SUB_CS_C_USER_OFFLINE:		//用户下线
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_CS_C_UserOffline));
		if (wDataSize != sizeof(CMD_CS_C_UserOffline)) return false;

		//消息处理
		CMD_CS_C_UserOffline* pUserOffline = (CMD_CS_C_UserOffline*)pData;
			
		//获取参数
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//连接效验
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_LOGON);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_LOGON) return false;

		//查找服务
		auto pGlobalLogonInfo = (CGlobalLogonInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_LOGON, pBindParameter->wServiceID);
		if (pGlobalLogonInfo == NULL) return false;

		//查找用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pUserOffline->dwUserID);
		if (pGlobalUserInfo != NULL)
		{
			//聊天在线
			if (pGlobalUserInfo->m_cbIMUserStatus == US_IM_ONLINE)
			{
				//构造结构
				CMD_CS_IM_UserOffline UserOffline = {};
				UserOffline.dwUserID = pUserOffline->dwUserID;

				//广播消息
				SendDataToAllService(SERVICE_MODULE_CHAT, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_IM_USER_OFFLINE, &UserOffline, sizeof(UserOffline));
			}			

			//茶社在线
			if (pGlobalUserInfo->m_cbCBUserStatus == US_CB_ONLINE)
			{
				//变量定义
				CMD_CS_CB_UserOffline UserOffline = {};				
				UserOffline.dwUserID = pUserOffline->dwUserID;

				//广播消息
				SendDataToAllService(SERVICE_MODULE_CLUB, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_CB_USER_OFFLINE, &UserOffline, sizeof(UserOffline));

				//广播消息
				SendDataToAllService(SERVICE_MODULE_CLUBPLAZA, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_CB_USER_OFFLINE, &UserOffline, sizeof(UserOffline));
			}

			//更新信息
			pGlobalUserInfo->m_wGateID = 0;
			pGlobalUserInfo->m_dwTokenID = 0;

			//绑定服务
			pGlobalUserInfo->UnBindLogonInfo(pGlobalLogonInfo);

			//删除用户
			if (pGlobalUserInfo->BindServiceCount() == 0)
			{
				m_GlobalInfoManager.DeleteUserInfo(pUserOffline->dwUserID);
			}
		}					

		return true;
	}	
	case SUB_CS_C_USER_UPDATE:		//用户更新
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_CS_C_UserUpdate));
		if (wDataSize != sizeof(CMD_CS_C_UserUpdate)) return false;

		//消息处理
		auto pUserDataUpdate = (CMD_CS_C_UserUpdate*)pData;
		
		//获取参数
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//连接效验
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_LOGON);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_LOGON) return false;

		//查找服务
		auto pGlobalLogonInfo = (CGlobalLogonInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_LOGON, pBindParameter->wServiceID);
		if (pGlobalLogonInfo == NULL) return false;

		//查找用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pUserDataUpdate->dwUserID);
		if (pGlobalUserInfo != NULL)
		{
			//用户昵称
			if (pUserDataUpdate->dwInfoMask & INFO_MASK_NICKNAME)
			{
				StringCchCopy(pGlobalUserInfo->m_szNickName, CountArray(pGlobalUserInfo->m_szNickName), pUserDataUpdate->szNickName);
				CTraceService::TraceString(TEXT("玩家%d更新昵称：%s"), TraceLevel_Debug,pUserDataUpdate->dwUserID, pUserDataUpdate->szNickName);
			}
			StringCchCopy(pGlobalUserInfo->m_szUserHeadUrl, CountArray(pGlobalUserInfo->m_szUserHeadUrl), pUserDataUpdate->szUserHeadUrl);//kk jia
			//用户头像
			if (pUserDataUpdate->dwInfoMask & INFO_MASK_FACEID)
			{
				pGlobalUserInfo->m_wFaceID = pUserDataUpdate->wFaceID;
				pGlobalUserInfo->m_dwCustomID = pUserDataUpdate->dwCustomID;
				CTraceService::TraceString(TEXT("玩家%d更新头像：%d|%d"), TraceLevel_Debug, pUserDataUpdate->dwUserID,pUserDataUpdate->wFaceID, pUserDataUpdate->dwCustomID);
			}	

			//变量定义
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo, sizeof(GlobalUserInfo));

			//设置变量
			GlobalUserInfo.dwUserID = pGlobalUserInfo->m_dwUserID;
			GlobalUserInfo.dwGameID = pGlobalUserInfo->m_dwGameID;
			GlobalUserInfo.cbGender = pGlobalUserInfo->m_cbGender;
			StringCchCopy(GlobalUserInfo.szNickName, CountArray(GlobalUserInfo.szNickName), pGlobalUserInfo->m_szNickName);
			StringCchCopy(GlobalUserInfo.szUserHeadUrl, CountArray(GlobalUserInfo.szUserHeadUrl), pGlobalUserInfo->m_szUserHeadUrl);//kk jia

			//状态信息
			GlobalUserInfo.cbIMUserStatus = pGlobalUserInfo->m_cbIMUserStatus;
			GlobalUserInfo.cbCBUserStatus = pGlobalUserInfo->m_cbCBUserStatus;

			//头像标识
			GlobalUserInfo.wFaceID = pGlobalUserInfo->m_wFaceID;
			GlobalUserInfo.dwCustomID = pGlobalUserInfo->m_dwCustomID;

			//状态信息
			GlobalUserInfo.cbShutUpStatus = pGlobalUserInfo->m_cbShutUpStatus;
			GlobalUserInfo.dwShutUpOverTime = pGlobalUserInfo->m_dwShutUpOverTime;

			//网络信息
			GlobalUserInfo.SocketInfo.wGateID = pGlobalUserInfo->m_wGateID;
			GlobalUserInfo.SocketInfo.dwTokenID = pGlobalUserInfo->m_dwTokenID;

			//广播消息
			SendDataToAllService(SERVICE_MODULE_CLUB, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_CB_USER_UPDATE, &GlobalUserInfo, sizeof(tagGlobalUserInfo));

			//广播消息
			SendDataToAllService(SERVICE_MODULE_CLUBPLAZA, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_CB_USER_UPDATE, &GlobalUserInfo, sizeof(tagGlobalUserInfo));
		}

		return true;
	}
	case SUB_CS_C_COLLECT_USER:		//汇总用户	
	{
		//获取参数
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//变量定义
		WORD wBuffSize = 0;
		BYTE cbBuffer[SOCKET_PACKET];

		//用户信息		
		auto pUserInfoItem = (tagGlobalUserInfo*)cbBuffer;
		
		//变量定义
		POSITION Position = NULL;
		CGlobalUserInfo* pGlobalUserInfo= NULL;

		do
		{
			//枚举用户
			pGlobalUserInfo = m_GlobalInfoManager.EnumUserInfo(Position);
			if (pGlobalUserInfo == NULL) break;

			//发送判断
			if (wBuffSize + sizeof(tagGlobalUserInfo) >= sizeof(cbBuffer))
			{
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_USER_COLLECT, SUB_CS_S_USER_LIST, cbBuffer, wBuffSize, NULL, 0);

				//重置数据
				wBuffSize = 0;
				pUserInfoItem = (tagGlobalUserInfo*)cbBuffer;
			}

			//基础信息
			pUserInfoItem->dwUserID = pGlobalUserInfo->GetUserID();
			pUserInfoItem->dwGameID = pGlobalUserInfo->GetGameID();
			pUserInfoItem->cbGender = pGlobalUserInfo->GetGender();
			StringCchCopy(pUserInfoItem->szNickName,CountArray(pUserInfoItem->szNickName), pGlobalUserInfo->m_szNickName);	
			StringCchCopy(pUserInfoItem->szUserHeadUrl, CountArray(pUserInfoItem->szUserHeadUrl), pGlobalUserInfo->m_szUserHeadUrl);//kk jia

			//头像信息
			pUserInfoItem->wFaceID = pGlobalUserInfo->GetFaceID();
			pUserInfoItem->dwCustomID = pGlobalUserInfo->GetCustomID();

			//用户状态
			pUserInfoItem->cbIMUserStatus = pGlobalUserInfo->m_cbIMUserStatus;
			pUserInfoItem->cbCBUserStatus = pGlobalUserInfo->m_cbCBUserStatus;

			//网络信息
			pUserInfoItem->SocketInfo.wGateID = pGlobalUserInfo->GetGateID();
			pUserInfoItem->SocketInfo.dwTokenID = pGlobalUserInfo->GetTokenID();

			//状态信息
			pUserInfoItem->cbShutUpStatus = pGlobalUserInfo->m_cbShutUpStatus;
			pUserInfoItem->dwShutUpOverTime = pGlobalUserInfo->m_dwShutUpOverTime;

			//设置变量
			pUserInfoItem++;

			wBuffSize += sizeof(tagGlobalUserInfo);

		} while (Position != NULL);

		//发送判断
		if (wBuffSize > 0)
		{
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_USER_COLLECT, SUB_CS_S_USER_LIST, cbBuffer, wBuffSize, NULL, 0);
		}

		//汇总完成
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_USER_COLLECT, SUB_CS_S_COLLECT_FINISH, cbBuffer, wBuffSize, NULL, 0);

		return true;
	}
	case SUB_CS_C_COLLECT_FINISH:	//汇总完成
	{
		//获取参数
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//设置变量
		m_wCollectItem=INVALID_WORD;

		//汇总切换
		if (m_WaitCollectItemArray.GetCount()>0)
		{
			//切换汇总
			INT_PTR nWaitCount=m_WaitCollectItemArray.GetCount();
			m_wCollectItem=m_WaitCollectItemArray[nWaitCount-1];

			//删除数组
			m_WaitCollectItemArray.RemoveAt(nWaitCount-1);

			//发送消息
			DWORD dwSocketID=(m_pBindParameter+m_wCollectItem)->dwSocketID;
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_USER,NULL,0);
		}

		return true;
	}
	}

	return false;
}

//信息查询
bool CAttemperEngineSink::OnTCPNetworkMainQueryService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_CS_C_QUERY_BY_GAMEID:		//根据游戏ID查询
		{
			return OnTCPNetwordSubQueryByGameID(pData,wDataSize,dwSocketID);
		}
	case SUB_CS_C_QUERY_BY_ACCOUNTS:	//根据游戏帐户查询
		{
			return OnTCPNetwordSubQueryByAccounts(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//查询用户
bool CAttemperEngineSink::OnTCPNetwordSubQueryByGameID(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	//效验
	ASSERT(wDataSize==sizeof(CMD_CS_C_QueryByGameID));
	if( wDataSize != sizeof(CMD_CS_C_QueryByGameID) ) return false;

	//变量定义
	CMD_CS_C_QueryByGameID *pQueryByGameID = (CMD_CS_C_QueryByGameID *)pData;

	//遍历用户
	POSITION pos = NULL;
	CGlobalUserInfo *pGlobalUserInfo = m_GlobalInfoManager.EnumUserInfo(pos);
	while(pGlobalUserInfo)
	{
		//找到用户
		if( pGlobalUserInfo->GetGameID() == pQueryByGameID->dwQueryGameID )
		{
			//变量定义
			BYTE cbBuffer[SOCKET_PACKET+sizeof(CMD_CS_S_QueryUserResult)+sizeof(tagGameServerItem)] = {0};

			//构造信息
			CMD_CS_S_QueryUserResult *pQueryResult = (CMD_CS_S_QueryUserResult *)cbBuffer;
			pQueryResult->dwSendUserID = pQueryByGameID->dwUserID;
			pQueryResult->dwUserID = pGlobalUserInfo->GetUserID();
			pQueryResult->dwGameID = pGlobalUserInfo->GetGameID();
			StringCchCopy( pQueryResult->szAccounts,CountArray(pQueryResult->szAccounts), pGlobalUserInfo->GetNickName());

			//附加信息
			WORD wBaseSize = sizeof(CMD_CS_S_QueryUserResult)-sizeof(pQueryResult->szAccounts)+CountStringBuffer(pQueryResult->szAccounts);
			CSendPacketHelper SendPacket(cbBuffer+wBaseSize,sizeof(cbBuffer)-wBaseSize);			

			//房间子项
			CGlobalServerInfo* pGlobalServerInfo = pGlobalUserInfo->GetBindServerInfo();
			if (pGlobalServerInfo != NULL)
			{
				tagGameServerItem ServerItem={0};
				ServerItem.wServerID = pGlobalServerInfo->m_GameServer.wServerID;
				ServerItem.wServerPort = pGlobalServerInfo->m_GameServer.wServerPort;
				ServerItem.dwServerAddr = pGlobalServerInfo->m_GameServer.dwServerAddr;
				StringCchCopy(ServerItem.szServerName, CountArray(ServerItem.szServerName), pGlobalServerInfo->m_GameServer.szServerName);

				WORD wSize = sizeof(ServerItem) - sizeof(ServerItem.szServerName) + CountStringBuffer(ServerItem.szServerName);
				if (!SendPacket.AddPacket(&ServerItem, wSize, DTP_GR_SERVER_INFO)) break;
			}			

			WORD wSendSize = wBaseSize+SendPacket.GetDataSize();
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_QUERY_SERVICE,SUB_CS_S_QUERY_USER_RESULT,cbBuffer,wSendSize,NULL,0);

			return true;
		}

		if( !pos ) break;
		pGlobalUserInfo = m_GlobalInfoManager.EnumUserInfo(pos);
	}

	//找不到用户,发送通知
	CMD_CS_S_QueryNotFound NotFound = {0};
	NotFound.dwSendUserID = pQueryByGameID->dwUserID;
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_QUERY_SERVICE,SUB_CS_S_QUERY_NOT_FOUND,&NotFound,sizeof(NotFound),NULL,0);

	return true;
}

//查询用户
bool CAttemperEngineSink::OnTCPNetwordSubQueryByAccounts(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	//变量定义
	CMD_CS_C_QueryByAccounts *pQueryByAccounts = (CMD_CS_C_QueryByAccounts *)pData;

	//效验
	ASSERT(wDataSize == sizeof(CMD_CS_C_QueryByAccounts)-sizeof(pQueryByAccounts->szQueryAccounts)+CountStringBuffer(pQueryByAccounts->szQueryAccounts));
	if( wDataSize != sizeof(CMD_CS_C_QueryByAccounts)-sizeof(pQueryByAccounts->szQueryAccounts)+CountStringBuffer(pQueryByAccounts->szQueryAccounts) ) 
		return false;

	//遍历用户
	POSITION pos = NULL;
	CGlobalUserInfo *pGlobalUserInfo = m_GlobalInfoManager.EnumUserInfo(pos);
	while( pGlobalUserInfo )
	{
		//找到用户
		CW2W strAccount(pGlobalUserInfo->GetNickName());
		_wcslwr_s(strAccount,sizeof(strAccount));
		if( lstrcmp((LPCTSTR)strAccount,pQueryByAccounts->szQueryAccounts) == 0 )
		{
			//变量定义
			BYTE cbBuffer[SOCKET_PACKET] = {0};

			//构造信息
			CMD_CS_S_QueryUserResult *pQueryResult = (CMD_CS_S_QueryUserResult *)cbBuffer;
			pQueryResult->dwSendUserID = pQueryByAccounts->dwUserID;
			pQueryResult->dwUserID = pGlobalUserInfo->GetUserID();
			pQueryResult->dwGameID = pGlobalUserInfo->GetGameID();
			StringCchCopy( pQueryResult->szAccounts,CountArray(pQueryResult->szAccounts), pGlobalUserInfo->GetNickName());

			//附加信息
			WORD wBaseSize = sizeof(CMD_CS_S_QueryUserResult)-sizeof(pQueryResult->szAccounts)+CountStringBuffer(pQueryResult->szAccounts);
			CSendPacketHelper SendPacket(cbBuffer+wBaseSize,sizeof(cbBuffer)-wBaseSize);
			
			//房间子项
			CGlobalServerInfo* pGlobalServerInfo = pGlobalUserInfo->GetBindServerInfo();
			if (pGlobalServerInfo != NULL)
			{
				tagGameServerItem ServerItem = { 0 };
				ServerItem.wServerID = pGlobalServerInfo->m_GameServer.wServerID;
				ServerItem.wServerPort = pGlobalServerInfo->m_GameServer.wServerPort;
				ServerItem.dwServerAddr = pGlobalServerInfo->m_GameServer.dwServerAddr;
				StringCchCopy(ServerItem.szServerName, CountArray(ServerItem.szServerName), pGlobalServerInfo->m_GameServer.szServerName);

				WORD wSize = sizeof(ServerItem) - sizeof(ServerItem.szServerName) + CountStringBuffer(ServerItem.szServerName);
				if (!SendPacket.AddPacket(&ServerItem, wSize, DTP_GR_SERVER_INFO)) break;
			}

			WORD wSendSize = wBaseSize + SendPacket.GetDataSize();
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_QUERY_SERVICE,SUB_CS_S_QUERY_USER_RESULT,cbBuffer,wSendSize,NULL,0);

			return true;
		}

		if( !pos ) break;
		pGlobalUserInfo = m_GlobalInfoManager.EnumUserInfo(pos);
	}

	//找不到用户,发送通知
	CMD_CS_S_QueryNotFound NotFound = {0};
	NotFound.dwSendUserID = pQueryByAccounts->dwUserID;
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_QUERY_SERVICE,SUB_CS_S_QUERY_NOT_FOUND,&NotFound,sizeof(NotFound),NULL,0);

	return true;
}

//配置完成
bool CAttemperEngineSink::OnDBConfigFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//事件通知
	CP_ControlResult ControlResult;
	ControlResult.cbSuccess = ER_SUCCESS;
	SendUIControlPacket(UI_CONFIG_RESULT, &ControlResult, sizeof(ControlResult));

	return true;
}

//解锁玩家
bool CAttemperEngineSink::OnDBUnlockPlayer(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//变量定义
	auto pUnlockPlayer = (DBO_GP_UnlockPlayer*)pData;

	//操作结果
	CMD_CS_S_OperateResult OperateResult;
	ZeroMemory(&OperateResult, sizeof(OperateResult));

	//设置变量
	OperateResult.wRequestCmdID = SUB_CS_C_UNLOCK_PLAYER;

	//发送结果
	WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
	WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);
	
	//发送数据
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize+wMsgSize, NULL, 0);

	return true;
}

//更新榜单
bool CAttemperEngineSink::OnDBUpdateRankingList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//发送消息
	SendDataToAllService(SERVICE_MODULE_LOGON,0, MDM_CS_FORWARD_SERVICE, SUB_CS_RANKING_LIST_UPDATE, pData, wDataSize);

	return true;
}

//发送服务
bool CAttemperEngineSink::SendServiceListItem(WORD wServiceModule, DWORD dwSocketID, bool bPushRouteTable)
{
	//变量定义
	WORD wPacketSize = 0L;
	POSITION Position = NULL;
	BYTE cbBuffer[SOCKET_PACKET];

	//收集数据
	do
	{
		//获取对象
		tagServiceItem * pServiceItem = (tagServiceItem *)(cbBuffer + wPacketSize);
		CGlobalServiceInfo * pGlobalServiceInfo = m_GlobalInfoManager.EnumServiceInfo(wServiceModule,Position);

		//设置数据
		if (pGlobalServiceInfo != NULL)
		{
			wPacketSize += sizeof(tagServiceItem);
			CopyMemory(pServiceItem, &pGlobalServiceInfo->m_ServiceItem, sizeof(tagServiceItem));
		}

	} while (Position != NULL);

	//发送数据
	if (wPacketSize > 0)
	{
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVICE_INSERT, cbBuffer, wPacketSize,NULL,0);
	}

	//推送路由表
	if (bPushRouteTable == true)
	{
		//重置变量
		Position = NULL;

		//枚举变量
		POSITION Position1 = NULL;
		tagRouteItem* pRouteItem = NULL;		
		auto pAppendRouteItem=(CMD_CS_S_AppendRouteItem*)cbBuffer;

		//收集数据
		do
		{
			//获取对象
			auto pGlobalServiceInfo = m_GlobalInfoManager.EnumServiceInfo(wServiceModule, Position);
			if (pGlobalServiceInfo != NULL)
			{	
				//设置变量
				Position1 = NULL;
				wPacketSize = sizeof(CMD_CS_S_AppendRouteItem);
				pAppendRouteItem->bTheFirst = true;
				pAppendRouteItem->wServiceID = pGlobalServiceInfo->GetServiceID();
				pAppendRouteItem->wServiceModule = pGlobalServiceInfo->GetServiceModule();

				do
				{
					pRouteItem=pGlobalServiceInfo->EnumRouteItem(Position1);
					if (pRouteItem == NULL) break;

					//拷贝数据
					if (pRouteItem->cbRouteState == ROUTE_STATE_SERVICE)
					{
						CopyMemory(&cbBuffer[wPacketSize], pRouteItem, sizeof(tagRouteItem));
						wPacketSize += sizeof(tagRouteItem);
					}

					//移除判断
					if (wPacketSize + sizeof(tagRouteItem) > CountArray(cbBuffer))
					{
						m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_APPEND_ROUTE_ITEM, cbBuffer, wPacketSize, NULL, 0);

						//设置变量
						pAppendRouteItem->bTheFirst = false;
						wPacketSize = sizeof(CMD_CS_S_AppendRouteItem);						
					}

				} while (Position1!=NULL);

				//发送判断
				if (wPacketSize > sizeof(CMD_CS_S_AppendRouteItem))
				{
					m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_APPEND_ROUTE_ITEM, cbBuffer, wPacketSize, NULL, 0);					
				}
			}

		} while (Position != NULL);
	}

	return true;
}

//推送服务
bool CAttemperEngineSink::PushServiceListItem(WORD wServiceMask, DWORD dwSocketID, bool bPushRouteTable)
{
	//发送服务列表
	if (wServiceMask & SERVICE_MASK_GAME) SendServerListItem(dwSocketID);
	if (wServiceMask & SERVICE_MASK_CHAT) SendServiceListItem(SERVICE_MODULE_CHAT, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_SYNC) SendServiceListItem(SERVICE_MODULE_SYNC, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_CLUB) SendServiceListItem(SERVICE_MODULE_CLUB, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_QUERY) SendServiceListItem(SERVICE_MODULE_QUERY, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_LOGON) SendServiceListItem(SERVICE_MODULE_LOGON, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_BATTLE) SendServiceListItem(SERVICE_MODULE_BATTLE, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_MATCH) SendServiceListItem(SERVICE_MODULE_MATCH, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_STORE) SendServiceListItem(SERVICE_MODULE_STORE, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_MESSAGE) SendServiceListItem(SERVICE_MODULE_MESSAGE, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_RANKING) SendServiceListItem(SERVICE_MODULE_RANKING, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_LOGGER) SendServiceListItem(SERVICE_MODULE_LOGGER, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_MONITOR) SendServiceListItem(SERVICE_MODULE_MONITOR, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_CLUBPLAZA) SendServiceListItem(SERVICE_MODULE_CLUBPLAZA, dwSocketID, bPushRouteTable);

	return true;
}

//发送列表
bool CAttemperEngineSink::SendServerListItem(DWORD dwSocketID)
{
	//变量定义
	WORD wPacketSize=0L;
	POSITION Position=NULL;
	BYTE cbBuffer[SOCKET_PACKET];

	//发送信息
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_COMMON_SERVICE,SUB_CS_S_SERVER_INFO,NULL,0);

	//收集数据
	do
	{
		//发送数据
		if ((wPacketSize+sizeof(tagGameServer))>sizeof(cbBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_COMMON_SERVICE,SUB_CS_S_SERVER_INSERT,cbBuffer,wPacketSize,NULL,0);
			wPacketSize=0;
		}

		//获取对象
		auto pGameServer=(tagGameServer *)(cbBuffer+wPacketSize);
		auto pGlobalServerInfo=(CGlobalServerInfo *)m_GlobalInfoManager.EnumServiceInfo(SERVICE_MODULE_GAME,Position);

		//设置数据
		if (pGlobalServerInfo!=NULL)
		{
			wPacketSize+=sizeof(tagGameServer);
			CopyMemory(pGameServer,&pGlobalServerInfo->m_GameServer,sizeof(tagGameServer));
		}

	} while (Position!=NULL);

	//发送数据
	if (wPacketSize>0) 
	{
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_COMMON_SERVICE,SUB_CS_S_SERVER_INSERT,cbBuffer,wPacketSize,NULL,0);
	}

	//发送完成
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_COMMON_SERVICE,SUB_CS_S_SERVER_FINISH,NULL,0);

	return true;
}

//发送服务
bool CAttemperEngineSink::SendServiceAttrib(DWORD dwSocketID)
{
	//变量定义
	WORD wIndex = 0;
	WORD wBufferSize = 0L;
	BYTE cbBuffer[SOCKET_PACKET] = {0};

	do
	{
		//枚举服务
		auto pServiceAttrib = CServiceAttribManager::EnumServiceAttrib(wIndex++);
		if (pServiceAttrib == NULL) break;		

		//拷贝数据
		CopyMemory(&cbBuffer[wBufferSize], pServiceAttrib, sizeof(tagServiceAttrib));
		wBufferSize += sizeof(tagServiceAttrib);

	} while(true);	

	//发送数据
	m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVICE_ATTRIB, cbBuffer, wBufferSize, NULL, 0);

	return true;
}

//发送彩池
bool CAttemperEngineSink::SendGameJackpot(DWORD dwSocketID)
{
	//变量定义
	POSITION Position = NULL;
	CGlobalMonitorInfo* pGlobalServiceInfo = NULL;

	//缓冲定义
	WORD wBuffSize = 0;
	BYTE cbBuffer[SOCKET_PACKET];

	do
	{
		pGlobalServiceInfo = (CGlobalMonitorInfo*)m_GlobalInfoManager.EnumServiceInfo(SERVICE_MODULE_MONITOR, Position);
		if (pGlobalServiceInfo == NULL) break;

		//遍历奖池
		auto pGlobalGameStock = pGlobalServiceInfo->m_StockGameStock.GetHeadStockItem();
		while (pGlobalGameStock)
		{
			auto pCurrentGameStock = pGlobalGameStock;
			pGlobalGameStock = pGlobalServiceInfo->m_StockGameStock.GetNextStockItem(pCurrentGameStock);

			//获取对象
			auto pGameJackpot = (CMD_CS_S_GameJackpot*)&cbBuffer[wBuffSize];

			//设置变量
			pGameJackpot->wKindID = pCurrentGameStock->m_wKindID;
			pGameJackpot->lJackpotScore = pCurrentGameStock->m_lJackpotScore;

			//缓冲大小
			wBuffSize += sizeof(CMD_CS_S_GameJackpot);
		}		

	} while (Position != NULL);

	//发送数据
	if (wBuffSize > 0)
	{
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_GAME_JACKPOT, cbBuffer, wBuffSize, NULL, 0);
	}

	return true;
}

//发送请求
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//发送数据
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->m_UIControlSink.PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//发送数据
bool CAttemperEngineSink::SendDataToAllService(WORD wServiceModule, WORD wExcludeID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//变量定义
	POSITION Position = NULL;
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;

	do
	{
		pGlobalServiceInfo= m_GlobalInfoManager.EnumServiceInfo(wServiceModule, Position);
		if (pGlobalServiceInfo==NULL) break;

		//校验标识
		if (pGlobalServiceInfo->GetServiceID() != wExcludeID)
		{	
			DWORD dwSocketID = pGlobalServiceInfo->GetSocketID();
			m_pITCPNetworkEngine->SendData(dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize, NULL, 0);
		}

	} while (Position != NULL);

	return true;
}

//服务数据
bool CAttemperEngineSink::SendDataToService(WORD wServiceModule, WORD wServiceID, WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	//查找房间
	CGlobalServiceInfo* pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(wServiceModule, wServiceID);
	if (pGlobalServiceInfo == NULL) return false;

	//发送数据
	DWORD dwSocketID = pGlobalServiceInfo->GetSocketID();
	m_pITCPNetworkEngine->SendData(dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize, NULL, 0);

	return true;
}

//消息发送
bool CAttemperEngineSink::SendDataToService(WORD wServiceModule, tagForwardItem& ForwardItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//映射服务
	if (ForwardItem.wServiceID == INVALID_WORD)
	{
		//映射服务
		auto pGlobalServiceInfo = m_GlobalInfoManager.LookupService(wServiceModule, ForwardItem.wServiceKind, ForwardItem.dwHashKeyID);
		if (pGlobalServiceInfo == NULL)
		{
			CTraceService::TraceString(TEXT("MapService：ServiceModule:%d,ServiceKind=%d,HashKeyID=%d"),TraceLevel_Exception, wServiceModule, ForwardItem.wServiceKind, ForwardItem.dwHashKeyID);
			return false;
		}

		SendDataToService(wServiceModule, pGlobalServiceInfo->GetServiceID() , wMainCmdID, wSubCmdID, pData, wDataSize);
	}	
	else if (ForwardItem.wServiceID == 0)
	{
		SendDataToAllService(wServiceModule, ForwardItem.wExcludeID, wMainCmdID, wSubCmdID, pData, wDataSize);
	}
	else
	{
		SendDataToService(wServiceModule, ForwardItem.wServiceID, wMainCmdID, wSubCmdID, pData, wDataSize);
	}	

	return true;
}

//用户发送
bool CAttemperEngineSink::SendDataToUser(WORD wServerID, DWORD dwUserID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//更新房间
VOID CAttemperEngineSink::UpdateServerItem(CGlobalServerInfo* pGlobalServerInfo, BYTE cbUpdateKind)
{
	//构造结构
	DBR_GP_UpdateServerList  UpdateServerList;
	ZeroMemory(&UpdateServerList, sizeof(UpdateServerList));

	//设置变量
	UpdateServerList.cbUpdateKind = cbUpdateKind;

	//格式化房间
	CStringW strServerItem;
	FormatServerItem(pGlobalServerInfo, strServerItem);
	_snwprintf_s(UpdateServerList.szServerList, CountArray(UpdateServerList.szServerList), L"[%s]", strServerItem.GetString());	

	//发送请求
	WORD wDataSize = CountStringBufferW(UpdateServerList.szServerList);
	WORD wHeadSize = sizeof(UpdateServerList) - sizeof(UpdateServerList.szServerList);
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_SERVER_LIST, 0L, 0L, 0L, &UpdateServerList, wHeadSize + wDataSize);
}

//格式化房间
VOID CAttemperEngineSink::FormatServerItem(CGlobalServerInfo* pGlobalServerInfo, CStringW& strServerItem)
{
	//变量定义
	LPCWSTR pszServerFormat = L"{\"ServerID\":%d,\"ServerName\":\"%s\",\"ServerRule\":%d,\"OnlineCount\":%d,\"MaxConnect\":%d}";

	//格式化
	auto pGameServer = &pGlobalServerInfo->m_GameServer;
	strServerItem.Format(pszServerFormat, pGameServer->wServerID, CT2CW(pGameServer->szServerName).m_psz, pGameServer->dwMasterRule, pGameServer->dwOnLineCount, pGameServer->dwMaxPlayer);	
}

//更新代理
VOID CAttemperEngineSink::UploadAgentList()
{
	//变量定义
	POSITION Position = NULL;
	CMapPortOnline MapPortOnline;	
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;
	
	do
	{
		//枚举网关
		CGlobalServiceInfo * pGlobalServiceInfo = m_GlobalInfoManager.EnumServiceInfo(SERVICE_MODULE_GATE, Position);
		if (pGlobalServiceInfo != NULL)
		{
			MapPortOnline[pGlobalServiceInfo->m_ServiceItem.wServicePort] = pGlobalServiceInfo->m_wOnlineCount;
		}

	} while (Position != NULL);

	//变量定义
	Position = NULL;
	WORD wOnlineCount = 0;
	tagPortPair * pPortPair = NULL;
	tagServiceItem * pServiceItem = NULL;
	CGlobalAgentInfo * pGlobalAgentItem = NULL;

	//变量定义
	CStringA strAgentList, strMappedList;
	CStringA strAgentItem, strMappedItem;

	//常量定义
	LPCSTR pszAgentFormat = "{\"GateID\":%d,\"ServiceDomain\":\"%ls\",\"MappedList\":[%s],\"OnlineCount\":%d,\"MaxConnect\":%d}";
	LPCSTR pszMappedFormat = "{\"ServicePort\":%d,\"ForwardPort\":%d,\"OnlineCount\":%d}";

	//收集数据
	do
	{
		//枚举网关
		pGlobalAgentItem = (CGlobalAgentInfo *)m_GlobalInfoManager.EnumServiceInfo(SERVICE_MODULE_AGENT, Position);
		if (pGlobalAgentItem == NULL) break;
		if (pGlobalAgentItem->GetServicePortCount() == 0) continue;
		if (pGlobalAgentItem->m_AgentItem.wAgentKind != AGENT_KIND_GATE) continue;
		
		//清空数据
		strMappedList.Empty();

		//端口映射列表
		for (int i = 0; i < pGlobalAgentItem->GetServicePortCount(); i++)
		{
			//端口对
			pPortPair=&pGlobalAgentItem->m_PortPairArray[i];

			//在线人数
			wOnlineCount=0;
			MapPortOnline.Lookup(pPortPair->wForwardPort,wOnlineCount);

			//添加分隔符
			if (strMappedList.IsEmpty()==false) strMappedList.Append(",");

			//格式字符串
			strMappedItem.Format(pszMappedFormat,pPortPair->wServicePort,pPortPair->wForwardPort,wOnlineCount);
			strMappedList.Append(strMappedItem);			
		}

		//添加分隔符
		if (strAgentList.IsEmpty()==false) strAgentList.Append(",");

		//代理信息
		pServiceItem=&pGlobalAgentItem->m_ServiceItem;
		strAgentItem.Format(pszAgentFormat, pServiceItem->wServiceID, pServiceItem->szServiceDomain, strMappedList, pGlobalAgentItem->m_wOnlineCount, pGlobalAgentItem->m_ServiceItem.wMaxConnect);
		strAgentList.Append(strAgentItem);

		//添加分隔符
	} while (Position != NULL);

	//添加括号
	strAgentList.Insert(0,"[");
	strAgentList.Append("]");

	//构造结构
	DBR_UpdateGateList UpdateGateList;
	ZeroMemory(&UpdateGateList, sizeof(UpdateGateList));

	//设置变量
	StringCchCopyA(UpdateGateList.szGateList, CountArray(UpdateGateList.szGateList), strAgentList);

	//发送请求
	WORD wDataSize = CountStringBufferA(UpdateGateList.szGateList);
	WORD wHeadSize = sizeof(UpdateGateList) - sizeof(UpdateGateList.szGateList);
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_GATE_LIST, 0L, 0L, 0L, &UpdateGateList, wHeadSize + wDataSize);
}

//上传网关
VOID CAttemperEngineSink::UpdateGateList()
{
	//变量定义
	POSITION Position = NULL;	
	tagServiceItem * pServiceItem = NULL;
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;

	//变量定义
	CStringA strAgentList;
	CStringA strAgentItem, strMappedItem;

	//常量定义
	LPCSTR pszAgentFormat = "{\"GateID\":%d,\"ServiceDomain\":\"%ls\",\"ServicePort\":%d,\"OnlineCount\":%d,\"MaxConnect\":%d}";	

	//收集数据
	do
	{
		//枚举网关
		pGlobalServiceInfo = m_GlobalInfoManager.EnumServiceInfo(SERVICE_MODULE_GATE, Position);
		if (pGlobalServiceInfo == NULL) break;

		//服务信息
		pServiceItem = &pGlobalServiceInfo->m_ServiceItem;

		//添加分隔符
		if (strAgentList.IsEmpty()==false) strAgentList.Append(",");

		//代理信息		
		strAgentItem.Format(pszAgentFormat, pServiceItem->wServiceID, pServiceItem->szServiceDomain, pServiceItem->wServicePort, pGlobalServiceInfo->m_wOnlineCount, pGlobalServiceInfo->m_ServiceItem.wMaxConnect);
		strAgentList.Append(strAgentItem);

		//添加分隔符
	} while (Position != NULL);

	//添加括号
	strAgentList.Insert(0, "[");
	strAgentList.Append("]");	

	//构造结构
	DBR_UpdateGateList UpdateGateList;
	ZeroMemory(&UpdateGateList, sizeof(UpdateGateList));

	//设置变量
	StringCchCopyA(UpdateGateList.szGateList, CountArray(UpdateGateList.szGateList), strAgentList);

	//发送请求
	WORD wDataSize = CountStringBufferA(UpdateGateList.szGateList);
	WORD wHeadSize = sizeof(UpdateGateList) - sizeof(UpdateGateList.szGateList);
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_GATE_LIST, 0L,0L, 0L, &UpdateGateList, wHeadSize + wDataSize);
}

//广播消息
bool CAttemperEngineSink::OnTCPNetworkMainBroadcast(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{	
	switch (wSubCmdID)
	{
	case SUB_CS_C_GAME_BROADCAST:
		{
			//消息定义
			CMD_Buffer * pBuffer=(CMD_Buffer *)pData;

			//效验数据
			ASSERT(wDataSize>=(sizeof(pBuffer->Head)));
			ASSERT(wDataSize==(sizeof(CMD_Buffer)-sizeof(pBuffer->cbBuffer)+pBuffer->Head.CmdInfo.wPacketSize));

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			ASSERT(pBindParameter->wServiceModule==SERVICE_MODULE_GAME);
			if (pBindParameter->wServiceModule != SERVICE_MODULE_GAME) return false;

			//遍历连接
			for (int i=0; i<m_pInitParameter->m_wMaxConnect; i++)
			{
				//获取对象
				pBindParameter=(m_pBindParameter+i);
				if(pBindParameter!=NULL && pBindParameter->wServiceModule != SERVICE_MODULE_GAME)
					continue;

				//发送数据
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_CS_BROADCAST,SUB_CS_C_GAME_BROADCAST,pData,wDataSize,NULL,0);

			}
			break;
		}
	}
	
	return true;
}

//网站通知
bool CAttemperEngineSink::OnTCPNetworkMasterService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	CString str;
	str.Format(L"[WEB] CMD:%d Size:%d",wSubCmdID,wDataSize);
	CTraceService::TraceString(str,enTraceLevel::TraceLevel_Debug);

	switch (wSubCmdID)
	{
	case SUB_CS_C_MODIFY_SERVER:		//修改房间
	{	
		//校验数据
		ASSERT(wDataSize == sizeof(CMD_CS_C_ModifyServer));
		if (wDataSize != sizeof(CMD_CS_C_ModifyServer)) return true;

		//提取数据
		auto pModifyServer = (CMD_CS_C_ModifyServer*)pData;
		pModifyServer->szOperateID[CountArray(pModifyServer->szOperateID) - 1] = 0;

		//权限过滤
		pModifyServer->dwAppendRule &= (MR_FORBID_ENTER_ROOM);
		pModifyServer->dwRemoveRule &= (MR_FORBID_ENTER_ROOM);

		//变量定义	
		auto pGlobalServerInfo = (CGlobalServerInfo *)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GAME, pModifyServer->wServerID);
		if (pGlobalServerInfo != NULL)
		{
			//更新权限
			if (pModifyServer->dwAppendRule != 0)
			{
				pGlobalServerInfo->m_GameServer.dwMasterRule|= pModifyServer->dwAppendRule;
			}

			if (pModifyServer->dwRemoveRule != 0)
			{
				pGlobalServerInfo->m_GameServer.dwMasterRule &= ~pModifyServer->dwRemoveRule;
			}

			//校验权限
			if (pModifyServer->dwAppendRule != 0 || pModifyServer->dwRemoveRule != 0)
			{
				//构造结构
				CMD_CS_S_ServerModify ServerModify;
				ZeroMemory(&ServerModify, sizeof(ServerModify));

				//设置变量
				ServerModify.wKindID = pGlobalServerInfo->m_GameServer.wKindID;
				ServerModify.wSortID = pGlobalServerInfo->m_GameServer.wSortID;
				ServerModify.wNodeID = pGlobalServerInfo->m_GameServer.wNodeID;
				ServerModify.wServerID = pGlobalServerInfo->m_GameServer.wServerID;
				ServerModify.wServerPort = pGlobalServerInfo->m_GameServer.wServerPort;
				ServerModify.dwServerRule = pGlobalServerInfo->m_GameServer.dwServerRule;
				ServerModify.dwMasterRule = pGlobalServerInfo->m_GameServer.dwMasterRule;
				ServerModify.dwMaxPlayer = pGlobalServerInfo->m_GameServer.dwMasterRule;
				ServerModify.dwMaxPlayer = pGlobalServerInfo->m_GameServer.dwMaxPlayer;
				ServerModify.wServerLevel = pGlobalServerInfo->m_GameServer.wServerLevel;
				ServerModify.dwOnLineCount = pGlobalServerInfo->m_GameServer.dwOnLineCount;
				StringCchCopy(ServerModify.szServerName, CountArray(ServerModify.szServerName), pGlobalServerInfo->m_GameServer.szServerName);
				StringCchCopy(ServerModify.szServerDomain, CountArray(ServerModify.szServerDomain), pGlobalServerInfo->m_GameServer.szServerDomain);	

				//发送通知
				SendDataToAllService(SERVICE_MODULE_GATE,0, MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVER_MODIFY, &ServerModify, sizeof(ServerModify));
			}

			//更新房间
			UpdateServerItem(pGlobalServerInfo, UPDATE_KIND_MODIFY);

			//操作结果
			CMD_CS_S_OperateResult OperateResult;
			ZeroMemory(&OperateResult,sizeof(OperateResult));

			//设置变量
			OperateResult.wRequestCmdID = SUB_CS_C_MODIFY_SERVER;
			StringCchCopy(OperateResult.szOperateID,CountArray(OperateResult.szOperateID), pModifyServer->szOperateID);

			//发送消息
			WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
			WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);
		}		
		else
		{
			//操作结果
			CMD_CS_S_OperateResult OperateResult;
			ZeroMemory(&OperateResult, sizeof(OperateResult));

			//设置变量
			OperateResult.wRequestCmdID = SUB_CS_C_MODIFY_SERVER;
			StringCchCopy(OperateResult.szOperateID, CountArray(OperateResult.szOperateID), pModifyServer->szOperateID);

			//结果信息
			OperateResult.lResultCode = -1;
			StringCchCopy(OperateResult.szErrorDescribe,CountArray(OperateResult.szErrorDescribe),TEXT("The room does not exist."));

			//发送消息
			WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
			WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);
		}

		return true;
	}
	case SUB_CS_C_COLLECT_SERVER:		//收集房间
	{
		return true;
	}
	case SUB_CS_C_UNLOCK_PLAYER:		//解锁玩家
	{
		//校验数据
		ASSERT(wDataSize == sizeof(CMD_CS_C_UnlockPlayer));
		if (wDataSize != sizeof(CMD_CS_C_UnlockPlayer)) return true;

		//提取数据
		auto pUnlockPlayer = (CMD_CS_C_UnlockPlayer*)pData;
		
		//查询用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pUnlockPlayer->dwUserID);
		if (pGlobalUserInfo != NULL && pGlobalUserInfo->GetBindServerInfo()!=NULL)
		{
			//操作结果
			CMD_CS_S_OperateResult OperateResult;
			ZeroMemory(&OperateResult, sizeof(OperateResult));

			//设置变量
			OperateResult.wRequestCmdID = SUB_CS_C_UNLOCK_PLAYER;

			//操作结果
			OperateResult.lResultCode = 1;
			StringCchCopy(OperateResult.szErrorDescribe, CountArray(OperateResult.szErrorDescribe), TEXT("The player is currently in the game and is not allowed to unlock."));

			//发送数据			
			WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
			WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize+wMsgSize, NULL, 0);
		}
		else
		{
			//构造结构
			DBR_GP_UnlockPlayer UnlockPlayer;
			ZeroMemory(&UnlockPlayer,sizeof(UnlockPlayer));

			//设置变量
			UnlockPlayer.dwUserID = pUnlockPlayer->dwUserID;

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UNLOCK_PLAYER, UnlockPlayer.dwUserID, dwSocketID,0, &UnlockPlayer, sizeof(UnlockPlayer));
		}		

		return true;
	}
	case SUB_CS_C_KICKOUT_PLAYER:		//踢出玩家
	{
		//校验数据
		ASSERT(wDataSize <= sizeof(CMD_CS_C_KickoutPlayer));
		if (wDataSize > sizeof(CMD_CS_C_KickoutPlayer)) return true;

		//提取数据
		auto pKickoutPlayer = (CMD_CS_C_KickoutPlayer*)pData;

		//查询用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pKickoutPlayer->dwUserID);
		if (pGlobalUserInfo != NULL && pGlobalUserInfo->GetBindLogonInfo() != NULL)
		{
			//登录服务
			auto pGlobalLogonInfo = pGlobalUserInfo->GetBindLogonInfo();

			//构造结构
			CMD_CS_LogoutNotify LogoutNotify;

			//设置变量
			LogoutNotify.dwUserID = pKickoutPlayer->dwUserID;			
			StringCchCopy(LogoutNotify.szNotifyContent, CountArray(LogoutNotify.szNotifyContent), CWHService::FormatMessage(MIM_LOGON_0012, TEXT("抱歉，您已被迫下线！")));

			//发送消息
			SendDataToService(SERVICE_MODULE_LOGON, pGlobalLogonInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_LOGOUT_NOTIFY, &LogoutNotify, sizeof(LogoutNotify));
		}
		
		//操作结果
		CMD_CS_S_OperateResult OperateResult;
		ZeroMemory(&OperateResult, sizeof(OperateResult));

		//设置变量
		OperateResult.wRequestCmdID = SUB_CS_C_KICKOUT_PLAYER;

		//操作结果
		OperateResult.lResultCode = 0;
		StringCchCopy(OperateResult.szErrorDescribe, CountArray(OperateResult.szErrorDescribe), TEXT("Kicked out successful."));

		//发送数据			
		WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
		WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);

		return true;
	}
	case SUB_CS_C_USER_MAIL_NOTIFY:		//邮件通知
	{
		//校验数据
		ASSERT (wDataSize%sizeof(CMD_CS_MailNotify)==0);
		if (wDataSize % sizeof(CMD_CS_MailNotify) != 0) return false;

		//提取数据
		auto pMailNotify = (CMD_CS_MailNotify*)pData;
		WORD wItemCount = wDataSize / sizeof(CMD_CS_MailNotify);
		
		//转发消息
		SendDataToAllService(SERVICE_MODULE_MESSAGE,0,MDM_CS_FORWARD_SERVICE, SUB_CS_MAIL_NOTIFY, pData, wDataSize);

		return true;
	}
	case SUB_CS_C_USER_MAIL_INSERT:		//邮件插入
	{
		//校验数据
		ASSERT(wDataSize % sizeof(CMD_CS_MailInsert) == 0);
		if (wDataSize % sizeof(CMD_CS_MailInsert) != 0) return false;

		//操作结果
		CMD_CS_S_OperateResult OperateResult;
		ZeroMemory(&OperateResult, sizeof(OperateResult));

		//设置变量
		OperateResult.wRequestCmdID = SUB_CS_C_USER_MAIL_INSERT;

		//校验数量
		if (m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_MESSAGE)>0)
		{
			//构造结构
			tagForwardItem ForwardItem;
			ZeroMemory(&ForwardItem, sizeof(ForwardItem));

			//设置变量
			ForwardItem.wServiceID = INVALID_WORD;
			ForwardItem.dwHashKeyID = time(NULL);

			//转发消息
			SendDataToService(SERVICE_MODULE_MESSAGE, ForwardItem, MDM_CS_FORWARD_SERVICE, SUB_CS_MAIL_INSERT, pData, wDataSize);

			//操作结果
			OperateResult.lResultCode = 0;
			StringCchCopy(OperateResult.szErrorDescribe, CountArray(OperateResult.szErrorDescribe), TEXT("The server has received the request and the email is being sent."));
		}
		else
		{
			//操作结果
			OperateResult.lResultCode = 1;
			StringCchCopy(OperateResult.szErrorDescribe, CountArray(OperateResult.szErrorDescribe), TEXT("The system is not registered to process emails."));			
		}		

		//发送结果
		WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
		WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);

		return true;
	}
	case SUB_CS_C_USER_INFO_UPDATE:		//信息更新
	{
		//校验数据
		ASSERT(wDataSize == sizeof(CMD_CS_C_UserInfoUpdate));
		if (wDataSize != sizeof(CMD_CS_C_UserInfoUpdate)) return false;

		//提取数据
		auto pUserInfoUpdate = (CMD_CS_C_UserInfoUpdate*)pData;		
		
		return true;
	}
	case SUB_CS_C_SHUTDOWN_ACCOUNT:		//关闭账号
	{
		//校验数据
		ASSERT(wDataSize <= sizeof(CMD_CS_C_ShutDownAccount));
		if (wDataSize > sizeof(CMD_CS_C_ShutDownAccount)) return false;

		//提取数据
		auto pShutDownAccount = (CMD_CS_C_ShutDownAccount*)pData;		

		//查找用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pShutDownAccount->dwUserID);
		if (pGlobalUserInfo != NULL)
		{
			//构造结构
			CMD_CS_LogoutNotify LogoutNotify;

			//设置变量
			LogoutNotify.dwUserID = pShutDownAccount->dwUserID;
			StringCchCopy(LogoutNotify.szNotifyContent, CountArray(LogoutNotify.szNotifyContent), CWHService::FormatMessage(MIM_LOGON_0011, TEXT("抱歉，您的账号已被冻结！")));

			//在游戏
			if (pGlobalUserInfo->GetBindServerInfo() != NULL)
			{
				//登录服务
				auto pGlobalServerInfo = pGlobalUserInfo->GetBindServerInfo();

				//发送消息
				SendDataToService(SERVICE_MODULE_GAME, pGlobalServerInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_LOGOUT_NOTIFY, &LogoutNotify, sizeof(LogoutNotify));
			}

			//在大厅
			if (pGlobalUserInfo->GetBindLogonInfo() != NULL)
			{
				//登录服务
				auto pGlobalLogonInfo = pGlobalUserInfo->GetBindLogonInfo();

				//发送消息
				SendDataToService(SERVICE_MODULE_LOGON, pGlobalLogonInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_LOGOUT_NOTIFY, &LogoutNotify, sizeof(LogoutNotify));
			}

		}

		return true;
	}
	case SUB_CS_C_WEALTH_UPDATE:		//财富更新
	{
		//校验数据
		ASSERT(wDataSize == sizeof(CMD_CS_C_WealthUpdate));
		if (wDataSize != sizeof(CMD_CS_C_WealthUpdate)) return false;

		//提取数据
		auto pWealthUpdate = (CMD_CS_C_WealthUpdate*)pData;

		//查找用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pWealthUpdate->dwUserID);
		if (pGlobalUserInfo != NULL && pGlobalUserInfo->GetBindLogonInfo() != NULL)
		{
			//登录服务
			auto pGlobalLogonInfo = pGlobalUserInfo->GetBindLogonInfo();

			//构造结构
			CMD_CS_UserWealthUpdate UserWealthUpdate;

			//设置变量
			UserWealthUpdate.dwUserID = pWealthUpdate->dwUserID;

			//发送消息
			SendDataToService(SERVICE_MODULE_LOGON, pGlobalLogonInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_USER_WEALTH_UPDATE, &UserWealthUpdate, sizeof(UserWealthUpdate));
		}

		return true;
	}	
	case SUB_CS_C_RECHARGE_NOTICE:		//充值通知
	{
		//校验数据
		ASSERT(wDataSize <= sizeof(CMD_CS_C_RechargeNotice));
		if (wDataSize > sizeof(CMD_CS_C_RechargeNotice)) return false;

		//提取数据
		auto pRechargeNotice = (CMD_CS_C_RechargeNotice*)pData;

		//查找用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pRechargeNotice->dwUserID);
		if (pGlobalUserInfo != NULL)
		{
			//构造结构
			CMD_CS_UserRechargeNotice UserRechargeNotice;

			//设置变量
			UserRechargeNotice.dwUserID = pRechargeNotice->dwUserID;
			StringCchCopy(UserRechargeNotice.szGoodsList, CountArray(UserRechargeNotice.szGoodsList), pRechargeNotice->szGoodsList);

			//在游戏
			if (pGlobalUserInfo->GetBindServerInfo() != NULL)
			{
				//登录服务
				auto pGlobalServerInfo = pGlobalUserInfo->GetBindServerInfo();		

				//发送消息
				SendDataToService(SERVICE_MODULE_GAME, pGlobalServerInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_USER_RECHARGE_NOTICE, &UserRechargeNotice, sizeof(UserRechargeNotice));
			}
			//在大厅
			if (pGlobalUserInfo->GetBindLogonInfo() != NULL)
			{
				//登录服务
				auto pGlobalLogonInfo = pGlobalUserInfo->GetBindLogonInfo();

				//更新掩码
				if (pRechargeNotice->bUpdateMask==true)
				{
					//变量定义
					CMD_CS_UpdateActionMask UpdateActionMask;
					ZeroMemory(&UpdateActionMask, sizeof(UpdateActionMask));

					//设置变量
					UpdateActionMask.dwUserID = pRechargeNotice->dwUserID;
					UpdateActionMask.dwActionMaskEver = pRechargeNotice->dwActionMaskEver;
					UpdateActionMask.dwActionMaskPerDay = pRechargeNotice->dwActionMaskPerDay;
					UpdateActionMask.dwActionMaskPerWeek = pRechargeNotice->dwActionMaskPerWeek;

					//发送消息
					SendDataToService(SERVICE_MODULE_LOGON, pGlobalLogonInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_UPDATE_ACTION_MASK, &UpdateActionMask, sizeof(UpdateActionMask));
				}

				//充值通知
				if (pGlobalUserInfo->GetBindServerInfo() == NULL)
				{
					//发送消息
					SendDataToService(SERVICE_MODULE_LOGON, pGlobalLogonInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_USER_RECHARGE_NOTICE, &UserRechargeNotice, sizeof(UserRechargeNotice));
				}
			}
		}

		return true;
	}	
	case SUB_CS_C_MODIFY_SHUTUP_STATUS:	//修改状态
	{
		//校验数据
		ASSERT(wDataSize == sizeof(CMD_CS_C_ModifyShutUpStatus));
		if (wDataSize != sizeof(CMD_CS_C_ModifyShutUpStatus)) return false;

		//提取数据
		auto pModifyShutUpStatus = (CMD_CS_C_ModifyShutUpStatus*)pData;

		//构造结构
		CMD_CS_UserShutUpStatus UserShutUpStatus;
		ZeroMemory(&UserShutUpStatus,sizeof(UserShutUpStatus));

		//设置状态
		UserShutUpStatus.dwUserID = pModifyShutUpStatus->dwUserID;
		UserShutUpStatus.cbShutUpStatus = pModifyShutUpStatus->cbShutUpStatus;
		UserShutUpStatus.dwShutUpOverTime = pModifyShutUpStatus->dwShutUpOverTime;

		//查找用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pModifyShutUpStatus->dwUserID);
		if (pGlobalUserInfo != NULL)
		{
			//更新状态
			pGlobalUserInfo->m_cbShutUpStatus = pModifyShutUpStatus->cbShutUpStatus;
			pGlobalUserInfo->m_dwShutUpOverTime = pModifyShutUpStatus->dwShutUpOverTime;

			//通知登录
			if (pGlobalUserInfo->GetBindLogonInfo() != NULL)
			{
				//登录服务
				auto pGlobalLogonInfo = pGlobalUserInfo->GetBindLogonInfo();

				//发送消息
				SendDataToService(SERVICE_MODULE_LOGON, pGlobalLogonInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_USER_SHUTUP_STATUS, pData, wDataSize);
			}

			//通知房间
			if (pGlobalUserInfo->GetBindServerInfo() != NULL)
			{
				//登录服务
				auto pGlobalServerInfo = pGlobalUserInfo->GetBindServerInfo();

				//发送消息
				SendDataToService(SERVICE_MODULE_GAME, pGlobalServerInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_USER_SHUTUP_STATUS, pData, wDataSize);
			}
		}

		//广播消息		
		SendDataToAllService(SERVICE_MODULE_CHAT,0, MDM_CS_FORWARD_SERVICE, SUB_CS_USER_SHUTUP_STATUS, pData, wDataSize);

		return true;
	}
	case SUB_CS_C_RELOAD_GAME_CONFIG:	//重载配置
	{
		//提取数据
		auto pReloadGameConfig = (CMD_CS_C_ReloadGameConfig*)pData;

		//构造结构
		tagExecuteCommand ExecuteCommand = {};
		
		//设置变量
		ExecuteCommand.dwCommandCode = CMD_LOAD_CONFIG_DATA;
		ExecuteCommand.dwCommandParam = pReloadGameConfig->dwConfigMask;

		CTraceService::TraceString(TEXT("ServiceMask=%d,ConfigMask=%d"), TraceLevel_Exception, pReloadGameConfig->dwServiceMask, pReloadGameConfig->dwConfigMask);

		//转发游戏
		if (pReloadGameConfig->dwServiceMask & SERVICE_MASK_GAME)
		{
			SendDataToAllService(SERVICE_MODULE_GAME,0, MDM_CS_MANAGER_SERVICE, SUB_CS_S_EXECUTE_COMMAND, &ExecuteCommand, sizeof(ExecuteCommand));
		}

		//转发登录
		if (pReloadGameConfig->dwServiceMask & SERVICE_MASK_LOGON)
		{
			SendDataToAllService(SERVICE_MODULE_LOGON,0, MDM_CS_MANAGER_SERVICE, SUB_CS_S_EXECUTE_COMMAND, &ExecuteCommand, sizeof(ExecuteCommand));
		}

		//转发约战
		if (pReloadGameConfig->dwServiceMask & SERVICE_MASK_BATTLE)
		{
			SendDataToAllService(SERVICE_MODULE_BATTLE,0, MDM_CS_MANAGER_SERVICE, SUB_CS_S_EXECUTE_COMMAND, &ExecuteCommand, sizeof(ExecuteCommand));
		}

		//转发排位
		if (pReloadGameConfig->dwServiceMask & SERVICE_MASK_RANKING)
		{
			SendDataToAllService(SERVICE_MODULE_RANKING,0, MDM_CS_MANAGER_SERVICE, SUB_CS_S_EXECUTE_COMMAND, &ExecuteCommand, sizeof(ExecuteCommand));
		}

		//转发茶社服务
		if (pReloadGameConfig->dwServiceMask & SERVICE_MASK_CLUB)
		{
			SendDataToAllService(SERVICE_MODULE_CLUB, 0, MDM_CS_MANAGER_SERVICE, SUB_CS_S_EXECUTE_COMMAND, &ExecuteCommand, sizeof(ExecuteCommand));
		}

		//转发茶社广场
		if (pReloadGameConfig->dwServiceMask & SERVICE_MASK_CLUBPLAZA)
		{
			SendDataToAllService(SERVICE_MODULE_CLUBPLAZA, 0, MDM_CS_MANAGER_SERVICE, SUB_CS_S_EXECUTE_COMMAND, &ExecuteCommand, sizeof(ExecuteCommand));
		}

		return true;
	}
	case SUB_CS_C_APPEND_FILTER_WORDS:	//添加词汇
	{
		//提取数据
		auto pAppendFilterWords = (CMD_CS_C_AppendFilterWords*)pData;

		//构造结构
		CMD_CS_FilterWordsUpdate FilterWordsUpdate;
		ZeroMemory(&FilterWordsUpdate,sizeof(FilterWordsUpdate));

		//设置变量
		FilterWordsUpdate.bDeleted = false;
		StringCchCopy(FilterWordsUpdate.szFilterWords,CountArray(FilterWordsUpdate.szFilterWords), pAppendFilterWords->szFilterWords);

		//转发聊天
		SendDataToAllService(SERVICE_MODULE_CHAT,0, MDM_CS_FORWARD_SERVICE, SUB_CS_FILTER_WORDS_UPDATE, &FilterWordsUpdate, sizeof(FilterWordsUpdate));

		//转发登录
		SendDataToAllService(SERVICE_MODULE_LOGON,0, MDM_CS_FORWARD_SERVICE, SUB_CS_FILTER_WORDS_UPDATE, &FilterWordsUpdate, sizeof(FilterWordsUpdate));

		return true;
	}
	case SUB_CS_C_DELETE_FILTER_WORDS:	//删除词汇
	{
		//提取数据
		auto pDeleteFilterWords = (CMD_CS_C_DeleteFilterWords*)pData;

		//构造结构
		CMD_CS_FilterWordsUpdate FilterWordsUpdate;
		ZeroMemory(&FilterWordsUpdate, sizeof(FilterWordsUpdate));

		//设置变量
		FilterWordsUpdate.bDeleted = true;
		StringCchCopy(FilterWordsUpdate.szFilterWords, CountArray(FilterWordsUpdate.szFilterWords), pDeleteFilterWords->szFilterWords);

		//转发聊天
		SendDataToAllService(SERVICE_MODULE_CHAT,0,MDM_CS_FORWARD_SERVICE, SUB_CS_FILTER_WORDS_UPDATE, &FilterWordsUpdate, sizeof(FilterWordsUpdate));

		//转发登录
		SendDataToAllService(SERVICE_MODULE_LOGON,0,MDM_CS_FORWARD_SERVICE, SUB_CS_FILTER_WORDS_UPDATE, &FilterWordsUpdate, sizeof(FilterWordsUpdate));

		return true;
	}
	case SUB_CS_C_APPEND_USER_CONTROL:  //添加控制
	{
		//提取数据
		auto pAppendUserControl = (CMD_CS_C_AppendUserControl*)pData;
		
		//查找用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pAppendUserControl->dwUserID);
		if (pGlobalUserInfo)
		{
			auto pBindServerInfo = pGlobalUserInfo->GetBindServerInfo();
			if (pBindServerInfo && pBindServerInfo->m_GameServer.wServerType&GAME_GENRE_GOLD)
			{
				//构造结构
				CMD_CS_UserControlAppend UserControlAppend;
				ZeroMemory(&UserControlAppend,sizeof(UserControlAppend));

				//设置标识
				UserControlAppend.dwUserID = pAppendUserControl->dwUserID;

				//转发服务
				SendDataToService(SERVICE_MODULE_GAME, pBindServerInfo->m_GameServer.wServerID, MDM_CS_FORWARD_SERVICE, SUB_CS_USER_CONTROL_APPEND, &UserControlAppend, sizeof(UserControlAppend));
			}
		}		

		return true;
	}
	case SUB_CS_C_REMOVE_USER_CONTROL:  //移除控制
	{
		//提取数据
		auto pRemoveUserControl = (CMD_CS_C_RemoveUserControl*)pData;

		//查找用户
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pRemoveUserControl->dwUserID);
		if (pGlobalUserInfo)
		{
			auto pBindServerInfo = pGlobalUserInfo->GetBindServerInfo();
			if (pBindServerInfo && pBindServerInfo->m_GameServer.wServerType & GAME_GENRE_GOLD)
			{
				//构造结构
				CMD_CS_UserControlRemove RemoveUserControl;
				ZeroMemory(&RemoveUserControl, sizeof(RemoveUserControl));

				//设置标识
				RemoveUserControl.dwUserID = pRemoveUserControl->dwUserID;

				//转发服务
				SendDataToService(SERVICE_MODULE_GAME, pBindServerInfo->m_GameServer.wServerID, MDM_CS_FORWARD_SERVICE, SUB_CS_USER_CONTROL_REMOVE, &RemoveUserControl, sizeof(RemoveUserControl));
			}
		}

		return true;
	}
	case SUB_CS_C_APPEND_STOCK_ITEM:	//添加库存
	{
		//提取数据
		auto pAppendStockItem = (CMD_CS_C_AppendStockItem*)pData;

		//构造结构
		CMD_CS_AppendStockItem  AppendStockItem = {};
		AppendStockItem.wStockID = pAppendStockItem->wStockID;
		StringCchCopy(AppendStockItem.szOperateID,CountArray(AppendStockItem.szOperateID), pAppendStockItem->szOperateID);

		//发送消息
		SendDataToAllService(SERVICE_MODULE_MONITOR,0, MDM_CS_FORWARD_SERVICE, SUB_CS_APPEND_STOCK_ITEM,&AppendStockItem,sizeof(AppendStockItem));

		//操作结果
		CMD_CS_S_OperateResult OperateResult = {};

		//设置变量
		OperateResult.wRequestCmdID = SUB_CS_C_APPEND_STOCK_ITEM;
		OperateResult.lResultCode = m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_MONITOR) ? 0 : -1;
		StringCchCopy(OperateResult.szOperateID, CountArray(OperateResult.szOperateID), pAppendStockItem->szOperateID);

		//发送结果
		WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
		WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);

		//发送数据
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);
		
		return true;
	}
	case SUB_CS_C_DELETE_STOCK_ITEM:	//删除库存
	{
		//提取数据
		auto pDeleteStockItem = (CMD_CS_C_DeleteStockItem*)pData;

		//构造结构
		CMD_CS_DeleteStockItem  DeleteStockItem = {};
		DeleteStockItem.wStockID = pDeleteStockItem->wStockID;
		StringCchCopy(DeleteStockItem.szOperateID, CountArray(DeleteStockItem.szOperateID), pDeleteStockItem->szOperateID);

		//发送消息
		SendDataToAllService(SERVICE_MODULE_MONITOR,0, MDM_CS_FORWARD_SERVICE, SUB_CS_DELETE_STOCK_ITEM, &DeleteStockItem, sizeof(DeleteStockItem));

		//操作结果
		CMD_CS_S_OperateResult OperateResult = {};

		//设置变量
		OperateResult.wRequestCmdID = SUB_CS_C_DELETE_STOCK_ITEM;
		OperateResult.lResultCode = m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_MONITOR) ? 0 : -1;
		StringCchCopy(OperateResult.szOperateID, CountArray(OperateResult.szOperateID), pDeleteStockItem->szOperateID);

		//发送结果
		WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
		WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);

		//发送数据
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);
		
		return true;
	}
	case SUB_CS_C_MODIFY_STOCK_ITEM:	//修改库存
	{
		//提取数据
		auto pModifyStockItem = (CMD_CS_C_ModifyStockItem*)pData;

		//构造结构
		CMD_CS_ModifyStockItem  ModifyStockItem = {};
		ModifyStockItem.wStockID = pModifyStockItem->wStockID;
		StringCchCopy(ModifyStockItem.szOperateID, CountArray(ModifyStockItem.szOperateID), pModifyStockItem->szOperateID);

		//发送消息
		SendDataToAllService(SERVICE_MODULE_MONITOR,0, MDM_CS_FORWARD_SERVICE, SUB_CS_MODIFY_STOCK_ITEM, &ModifyStockItem, sizeof(ModifyStockItem));

		//操作结果
		CMD_CS_S_OperateResult OperateResult = {};

		//设置变量
		OperateResult.wRequestCmdID = SUB_CS_C_MODIFY_STOCK_ITEM;
		OperateResult.lResultCode = m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_MONITOR) ? 0 : -1;
		StringCchCopy(OperateResult.szOperateID, CountArray(OperateResult.szOperateID), pModifyStockItem->szOperateID);

		//发送结果
		WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
		WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);

		//发送数据
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);		

		return true;
	}
	case SUB_CS_C_ADJUST_STOCK_SCORE:	//调整库存值
	{
		//提取数据
		auto pAdjustStockScore = (CMD_CS_C_AdjustStockScore*)pData;

		//构造结构
		CMD_CS_AdjustStockScore  AdjustStockScore = {};
		AdjustStockScore.wStockID = pAdjustStockScore->wStockID;
		AdjustStockScore.lChangeScore = pAdjustStockScore->lChangeScore;
		StringCchCopy(AdjustStockScore.szOperateID, CountArray(AdjustStockScore.szOperateID), pAdjustStockScore->szOperateID);

		//发送消息
		SendDataToAllService(SERVICE_MODULE_MONITOR,0, MDM_CS_FORWARD_SERVICE, SUB_CS_ADJUST_STOCK_SCORE, &AdjustStockScore, sizeof(AdjustStockScore));

		//操作结果
		CMD_CS_S_OperateResult OperateResult = {};

		//设置变量
		OperateResult.wRequestCmdID = SUB_CS_C_ADJUST_STOCK_SCORE;
		OperateResult.lResultCode = m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_MONITOR) ? 0 : -1;
		StringCchCopy(OperateResult.szOperateID, CountArray(OperateResult.szOperateID), pAdjustStockScore->szOperateID);

		//发送结果
		WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
		WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);

		//发送数据
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);

		return true;
	}
	case SUB_CS_C_ADJUST_JACKPOT_SCORE: //调整彩金
	{
		//提取数据
		auto pAdjustJackpotScore = (CMD_CS_C_AdjustJackpotScore*)pData;

		//构造结构
		CMD_CS_AdjustJackpotScore  AdjustJackpotScore = {};
		AdjustJackpotScore.wStockID = pAdjustJackpotScore->wStockID;
		AdjustJackpotScore.wLevelID = pAdjustJackpotScore->wLevelID;
		AdjustJackpotScore.lChangeScore = pAdjustJackpotScore->lChangeScore;
		StringCchCopy(AdjustJackpotScore.szOperateID, CountArray(AdjustJackpotScore.szOperateID), pAdjustJackpotScore->szOperateID);

		//发送消息
		SendDataToAllService(SERVICE_MODULE_MONITOR,0, MDM_CS_FORWARD_SERVICE, SUB_CS_ADJUST_JACKPOT_SCORE, &AdjustJackpotScore, sizeof(AdjustJackpotScore));

		//操作结果
		CMD_CS_S_OperateResult OperateResult = {};

		//设置变量
		OperateResult.wRequestCmdID = SUB_CS_C_ADJUST_JACKPOT_SCORE;
		OperateResult.lResultCode = m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_MONITOR) ? 0 : -1;
		StringCchCopy(OperateResult.szOperateID, CountArray(OperateResult.szOperateID), pAdjustJackpotScore->szOperateID);

		//发送结果
		WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
		WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);

		//发送数据
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);
		
		return true;
	}
	case SUB_CS_C_UPDATE_SPLAYER_CONTROL://更新大R控制
	{
		//提取数据
		auto pUpdateSPlayerControl = (CMD_CS_C_UpdateSPlayerControl*)pData;

		//发送消息
		SendDataToAllService(SERVICE_MODULE_MONITOR,0, MDM_CS_FORWARD_SERVICE, SUB_CS_UPDATE_SPLAYER_CONTROL, NULL, 0);

		//操作结果
		CMD_CS_S_OperateResult OperateResult = {};

		//设置变量
		OperateResult.wRequestCmdID = SUB_CS_C_UPDATE_SPLAYER_CONTROL;
		OperateResult.lResultCode = m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_MONITOR) ? 0 : -1;
		StringCchCopy(OperateResult.szOperateID, CountArray(OperateResult.szOperateID), pUpdateSPlayerControl->szOperateID);

		//发送结果
		WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
		WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);

		//发送数据
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);

		return true;
	}
	case SUB_CS_C_PUBLISH_MARQUEE_NOTICE:  //发布公告
	{
		//提取数据
		auto pPublishMarqueeNotice = (CMD_CS_C_PublishMarqueeNotice*)pData;

		//构造结构
		CMD_CS_MarqueeNotice MarqueeNotice;
		ZeroMemory(&MarqueeNotice,sizeof(MarqueeNotice));

		//公告配置
		MarqueeNotice.wRollTimes = 0;
		MarqueeNotice.cbPriority = 0;
		MarqueeNotice.cbNoticeKind = NOTICE_KIND_SYSTEM;

		//设置数据
		MarqueeNotice.dwNoticeID = pPublishMarqueeNotice->dwNoticeID;
		MarqueeNotice.bExclusive = pPublishMarqueeNotice->bExclusive;
		MarqueeNotice.wViewPlace = pPublishMarqueeNotice->wViewPlace;
		MarqueeNotice.dwEndRollTime = pPublishMarqueeNotice->dwEndRollTime;
		MarqueeNotice.dwStartRollTime = pPublishMarqueeNotice->dwStartRollTime;		

		//构造结构
		auto pNoticeSystem= (tagNoticeKind_System*)MarqueeNotice.cbNoticeContent;
		StringCchCopy(pNoticeSystem->szNoticeContent, CountArray(pNoticeSystem->szNoticeContent), pPublishMarqueeNotice->szNoticeContent);

		//公告内容
		MarqueeNotice.wNoticeSize = CountStringBuffer(pNoticeSystem->szNoticeContent);
		MarqueeNotice.wNoticeSize += sizeof(tagNoticeKind_System)-sizeof(pNoticeSystem->szNoticeContent);
		CopyMemory(MarqueeNotice.cbNoticeContent, pNoticeSystem, MarqueeNotice.wNoticeSize);

		//发送数据
		WORD wPacketSize = sizeof(CMD_CS_MarqueeNotice) - sizeof(MarqueeNotice.cbNoticeContent) + MarqueeNotice.wNoticeSize;
		SendDataToAllService(SERVICE_MODULE_MESSAGE,0,MDM_CS_FORWARD_SERVICE, SUB_CS_MARQUEE_NOTICE, &MarqueeNotice, wPacketSize);

		return true;
	}
	}

	return true;
}

//管理服务
bool CAttemperEngineSink::OnTCPNetWorkMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_MODIFY_OPTION:		//修改选项
	{
		auto pModifyOption = (CMD_CS_C_ModifyOption*)pData;

		//循环发送
		for (int i = 0; i < pModifyOption->wServerCount; i++)
		{
			//转发子项
			tagForwardItem ForwardItem;
			ZeroMemory(&ForwardItem, sizeof(ForwardItem));

			//设置标识
			ForwardItem.wServiceID = pModifyOption->wServerIDList[i];

			//发送消息
			SendDataToService(SERVICE_MODULE_GAME, ForwardItem, MDM_CS_MANAGER_SERVICE, SUB_CS_S_MODIFY_OPTION, &pModifyOption->ModifyOption, sizeof(pModifyOption->ModifyOption));
		}

		return true;
	}
	case SUB_CS_C_EXECUTE_COMMAND:		//执行命令
	{
		m_pITCPNetworkEngine->SendDataBatch(MDM_CS_MANAGER_SERVICE, SUB_CS_S_EXECUTE_COMMAND, pData, wDataSize, 0L);

		return true;
	}
	}
	
	return true;
}

//转发服务
bool CAttemperEngineSink::OnTCPNetworkMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID)
{
	//提取头部
	WORD wHeadSize = sizeof(tagForwardHead);
	auto pForwardHead = (tagForwardHead*)pData;

	//转发给用户
	if (pForwardHead->wTarget == FORWARD_TARGET_USER)
	{
		//用户信息
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pForwardHead->dwUserID);
		if (pGlobalUserInfo != NULL)
		{
			//转发顺序
			for (auto i = 0; i < pForwardHead->wItemCount; i++)
			{
				//变量定义
				CGlobalServiceInfo* pGlobalServiceInfo = NULL;				
				tagForwardItem* pForwardItem = &pForwardHead->ForwardItem[i];	

				//登录服务
				if (pForwardItem->wServiceModule == SERVICE_MODULE_LOGON && pGlobalUserInfo->GetBindLogonInfo() != NULL)
				{
					pGlobalServiceInfo = pGlobalUserInfo->GetBindLogonInfo();
				}

				//房间服务
				if (pForwardItem->wServiceModule == SERVICE_MODULE_GAME && pGlobalUserInfo->GetBindServerInfo() != NULL)
				{
					pGlobalServiceInfo = pGlobalUserInfo->GetBindServerInfo();
				}

				//消息服务
				if (pForwardItem->wServiceModule == SERVICE_MODULE_MESSAGE && pGlobalUserInfo->GetBindMessageInfo() != NULL)
				{
					pGlobalServiceInfo = pGlobalUserInfo->GetBindMessageInfo();
				}				

				//发给所在服务
				if (pGlobalServiceInfo != NULL)
				{
					SendDataToService(pForwardItem->wServiceModule, pGlobalServiceInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, wSubCmdID, pForwardHead + 1, wDataSize - wHeadSize);
				}
			}
		}	
	}

	//转发给服务
	if (pForwardHead->wTarget == FORWARD_TARGET_SERVICE)
	{
		//用户信息
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pForwardHead->dwUserID);		

		//顺序转发
		for (auto i = 0; i < pForwardHead->wItemCount; i++)
		{	
			//变量定义
			CGlobalServiceInfo* pGlobalServiceInfo = NULL;
			tagForwardItem* pForwardItem = &pForwardHead->ForwardItem[i];			

			//校验用户
			if (pGlobalUserInfo != NULL)
			{
				//登录服务
				if (pForwardItem->wServiceModule == SERVICE_MODULE_LOGON && pGlobalUserInfo->GetBindLogonInfo() != NULL)
				{
					pGlobalServiceInfo = pGlobalUserInfo->GetBindLogonInfo();
				}

				//房间服务
				if (pForwardItem->wServiceModule == SERVICE_MODULE_GAME && pGlobalUserInfo->GetBindServerInfo() != NULL)
				{
					pGlobalServiceInfo = pGlobalUserInfo->GetBindServerInfo();
				}

				//消息服务
				if (pForwardItem->wServiceModule == SERVICE_MODULE_MESSAGE && pGlobalUserInfo->GetBindMessageInfo() != NULL)
				{
					pGlobalServiceInfo = pGlobalUserInfo->GetBindMessageInfo();
				}		

				//发给所在服务
				if (pGlobalServiceInfo != NULL)
				{
					SendDataToService(pForwardItem->wServiceModule, pGlobalServiceInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, wSubCmdID, pForwardHead + 1, wDataSize - wHeadSize);
					continue;
				}				
			}

			//映射服务			
			SendDataToService(pForwardItem->wServiceModule, *pForwardItem, MDM_CS_FORWARD_SERVICE, wSubCmdID, pForwardHead + 1, wDataSize - wHeadSize);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
