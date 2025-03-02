#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_CONNECT_CENTER			1									//重连标识
#define IDI_CONNECT_VALIDATE		2									//重连标识
#define IDI_LOAD_BATTLE_OPTION		3									//约战配置

//时长定义
#define TIME_LOAD_BATTLE_OPTION		60									//加载配置

//常量定义
#define MAX_TABLE_COUNT_REQUEST		64									//最大桌数		

//////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//状态变量		
	m_bBattleService=false;

	//设置变量
	m_pServiceOption=NULL;
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;	

	//组件变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;

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
	m_pBindParameter=new tagBindParameter[m_pServiceOption->wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pServiceOption->wMaxConnect);

	//设置标识
	m_bBattleService=true;
	m_SynchroItemArray.RemoveAll();

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//状态变量		
	m_bBattleService=false;
	m_SynchroItemArray.RemoveAll();

	//设置变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;

	//删除数据
	SafeDeleteArray(m_pBindParameter);

	//设置组件
	m_BattleManager.ResetData();
	m_GlobalInfoManager.ResetData();	

	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	switch (dwIdentifier)
	{
	case CT_LOAD_BATTLE_OPTION:		//加载配置
	{
		//加载配置			
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_BATTLE_OPTION,0,0,0,0,0);

		//提示消息
		CTraceService::TraceString(TEXT("正在加载约战配置..."),TraceLevel_Normal);

		return true;
	}
	case CT_CONNECT_CENTER:		//连接中心
	{
		//设置地址
		DWORD dwCenterServer=m_pInitParameter->m_CenterAddress.dwServiceAddr;

		//发起连接
		m_pITCPSocketCenter->Connect(htonl(dwCenterServer), m_pInitParameter->m_wCenterPort);

		//提示消息
		CTraceService::TraceString(TEXT("正在尝试连接中心服务器..."),TraceLevel_Normal);

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
	case IDI_CONNECT_CENTER:		//连接中心
		{
			//变量定义
			auto pAddressInfo=&m_pInitParameter->m_CenterAddress;

			//设置地址
			DWORD dwCenterServer=pAddressInfo->dwServiceAddr; 

			//发起连接
			m_pITCPSocketCenter->Connect(htonl(dwCenterServer), m_pInitParameter->m_wCenterPort);

			//提示消息
			CTraceService::TraceString(TEXT("正在尝试连接中心服务器..."),TraceLevel_Normal);

			return true;
		}
	}

	return false;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//获取索引
	ASSERT(LOWORD(dwSocketID)< m_pServiceOption->wMaxConnect);
	if (LOWORD(dwSocketID)>= m_pServiceOption->wMaxConnect) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	auto pBindParameter=(m_pBindParameter+wBindIndex);

	//设置变量
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientIP=dwClientIP;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);	

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//变量定义
	bool bResult = true;
	WORD wMainCmdID = LOCMMD(Command.wMainCmdID);

	//注册服务
	if (wMainCmdID == MDM_CS_REGISTER)
	{
		return OnTCPNetworkMainRegister(Command.wSubCmdID, pData, wDataSize, dwSocketID);
	}

	//通用服务
	if (wMainCmdID == MDM_CS_COMMON_SERVICE)
	{
		return OnTCPNetworkMainCommonService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
	}

	switch (wMainCmdID)
	{
	case MDM_GB_BATTLE_SERVICE:		//约战服务
	{
		bResult = OnTCPNetWorkMainGBBattleService(Command.wSubCmdID,pData,wDataSize,dwSocketID,dwTokenID);
		break;
	}
	case MDM_BS_BATTLE_SERVICE:		//约战服务
	{
		bResult = OnTCPNetWorkMainBSBattleService(Command.wSubCmdID,pData,wDataSize,dwSocketID,dwTokenID);
		break;
	}
	case MDM_BC_BATTLE_SERVICE:		//约战服务 
	{
		bResult = OnTCPNetWorkMainBCBattleService(Command.wSubCmdID, pData, wDataSize, dwSocketID, dwTokenID);
		break;
	}
	}

	//断开链接
	if (bResult == false)
	{
		//构造结构
		CMD_CS_C_ShutdownSocket ShutdownSocket;
		ShutdownSocket.bForceClose = false;

		//获取信息
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//发送数据
		SendDataToGate(pBindParameter->wServiceID,MDM_CS_COMMON_SERVICE,SUB_CS_S_SHUTDOWN_SOCKET,&ShutdownSocket,sizeof(ShutdownSocket),&dwTokenID,1);
	}

	return true;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	auto pBindParameter=(m_pBindParameter+wBindIndex);

	//游戏服务
	if (pBindParameter->wServiceModule==SERVICE_MODULE_GAME)
	{
		//查找桌子
		auto pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);
		if (pGlobalServerItem!=NULL)
		{
			//获取参数
			tagGameServer * pGameServer=&pGlobalServerItem->m_GameServer;

			//移除桌子
			m_BattleManager.RemoveServer(pGameServer->wKindID,pGameServer->wServerID);

			//注销桌子
			m_GlobalInfoManager.DeleteServerItem(pBindParameter->wServiceID);					
		}
	}

	//网关服务
	if (pBindParameter->wServiceModule == SERVICE_MODULE_GATE)
	{
		//查找网关
		auto pGlobalGateItem=m_GlobalInfoManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem!=NULL)
		{
			//注销网关
			m_GlobalInfoManager.DeleteGateItem(pBindParameter->wServiceID);					
		}
	}

	//茶社服务
	if (pBindParameter->wServiceModule == SERVICE_MODULE_CLUB)
	{
		//查找网关
		auto pGlobalClubItem = m_GlobalInfoManager.SearchClubItem(pBindParameter->wServiceID);
		if (pGlobalClubItem != NULL)
		{
			//注销网关
			m_GlobalInfoManager.DeleteClubItem(pBindParameter->wServiceID);
		}
	}
	
	//清除信息
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GR_OPERATE_FAILURE:			//操作失败
	{
		return OnDBOperateFailure(dwContextID, dwTokenID, pData, wDataSize);
	}	
	case DBO_GR_BATTLE_GAME_OPTION:			//游戏选项
	{
		return OnDBBattleGameOption(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_BATTLE_BASE_OPTION:			//基础选项
	{
		return OnDBBattleBaseOption(dwContextID, dwTokenID, pData,wDataSize);
	}	
	case DBO_GR_BATTLE_GLOBAL_OPTION:		//全局配置
	{
		return OnDBBattleGlobalOption(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_BATTLE_OPTION_FINISH:		//选项完成	
	{
		return OnDBBattleOptionFinish(dwContextID, dwTokenID, pData,wDataSize);
	}	
	}

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason)
{
	switch (dwServiceID)
	{
	case NETWORK_CENTER:	//中心连接
		{
			//提示消息
			CTraceService::TraceString(TEXT("与中心服务器的连接关闭了，%ld 秒后将重新连接"),TraceLevel_Warning, m_pInitParameter->m_wConnectTime);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return true;
		}	
	}

	return false;
}

//连接事件
bool CAttemperEngineSink::OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode)
{
	//中心连接
	if (dwServiceID==NETWORK_CENTER)
	{
		//错误判断
		if (nErrorCode!=0)
		{
			//提示消息
			CTraceService::TraceString(TEXT("中心服务器连接失败 [ %ld ]，%ld 秒后将重新连接"),TraceLevel_Warning, nErrorCode, m_pInitParameter->m_wConnectTime);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}

		//变量定义
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService,sizeof(RegisterService));

		//构造结构
		RegisterService.wServiceID= m_pServiceOption->wServiceID;
		RegisterService.wServiceModule = SERVICE_MODULE_BATTLE;
		RegisterService.wServicePort= m_pServiceOption->wServicePort;
		RegisterService.wServiceKindID = m_pServiceOption->wServiceKindID;
		StringCchCopy(RegisterService.szServiceName, CountArray(RegisterService.szServiceName), TEXT("约战服务器"));
		StringCchCopy(RegisterService.szServiceDomain, CountArray(RegisterService.szServiceDomain), m_pServiceOption->szServiceDomain);

		//发送数据
		m_pITCPSocketCenter->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_SERVICE,&RegisterService,sizeof(RegisterService),0);

		return true;
	}	

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	//中心连接
	if (dwServiceID ==NETWORK_CENTER)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:			//注册服务
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_CS_COMMON_SERVICE:		//服务信息
		{
			return OnTCPSocketMainCommonService(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_CS_MANAGER_SERVICE:	//管理服务
		{
			return OnTCPSocketMainManagerService(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_CS_FORWARD_SERVICE:	//转发服务
		{
			return OnTCPSocketMainForwardService(Command.wSubCmdID, pData, wDataSize);
		}
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}


//注册事件
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_SUCCESS:		//注册成功
	{
		//事件通知
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess = ER_SUCCESS;
		SendUIControlPacket(UI_CENTER_RESULT, &ControlResult, sizeof(ControlResult));

		return true;
	}
	case SUB_CS_S_REGISTER_FAILURE:		//注册失败
	{
		//效验参数
		auto pRegisterFailure = (CMD_CS_S_RegisterFailure*)pData;
		ASSERT(wDataSize >= (sizeof(CMD_CS_S_RegisterFailure) - sizeof(pRegisterFailure->szDescribeString)));
		if (wDataSize < (sizeof(CMD_CS_S_RegisterFailure) - sizeof(pRegisterFailure->szDescribeString))) return false;

		//关闭处理
		m_pITCPSocketCenter->CloseSocket();

		//显示消息
		CW2CT strDescribeString(pRegisterFailure->szDescribeString);
		if (lstrlen(strDescribeString) > 0) CTraceService::TraceString(strDescribeString, TraceLevel_Exception);

		//事件通知
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess = ER_FAILURE;
		SendUIControlPacket(UI_CENTER_RESULT, &ControlResult, sizeof(ControlResult));

		return true;
	}
	}

	return true;
}

//列表事件
bool CAttemperEngineSink::OnTCPSocketMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{	
	case SUB_CS_S_SERVICE_INSERT:	//服务插入
		{
			//参数校验
			ASSERT (wDataSize%sizeof(tagServiceItem)==0);
			if (wDataSize%sizeof(tagServiceItem)!=0) return false;

			//提取数据
			auto pServiceItem =(tagServiceItem *)pData;

			return true;
		}
	case SUB_CS_S_SERVICE_REMOVE:	//服务移除
		{
			//参数校验
			ASSERT (wDataSize==sizeof(CMD_CS_S_ServiceRemove));
			if (wDataSize!=sizeof(CMD_CS_S_ServiceRemove)) return false;

			//提取数据
			auto pServiceRemove =(CMD_CS_S_ServiceRemove *)pData;


			return true;
		}
	}

	return true;
}

//管理服务
bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_EXECUTE_COMMAND:			//执行命令
		{
			//参数校验
			ASSERT(sizeof(tagExecuteCommand) == wDataSize);
			if (sizeof(tagExecuteCommand) != wDataSize) return false;

			//提取数据
			auto pCommandCode = (tagExecuteCommand*)pData;

			//服务状态
			auto bLastBattleService = m_bBattleService;

			//加载配置
			if (pCommandCode->dwCommandCode & CMD_LOAD_CONFIG_DATA)
			{
				//约战配置
				if (pCommandCode->dwCommandParam & CONFIG_MASK_BATTLE_OPTION)
				{
					//加载配置
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_BATTLE_OPTION, 0, 0, 0, 0, 0);
				}
			}

			//停止服务
			if (pCommandCode->dwCommandCode&CMD_STOP_BATTLE_SERVICE)
			{
				//设置标识
				m_bBattleService=false;
			}

			//启动服务
			if (pCommandCode->dwCommandCode&CMD_START_BATTLE_SERVICE)
			{
				//设置标识
				m_bBattleService=true;
			}			

			//状态变更
			if (bLastBattleService != m_bBattleService)
			{
				//构造选项
				CMD_BS_C_BattleOption BattleOption;
				BattleOption.bBattleService = m_bBattleService;

				//约战选项
				m_pITCPNetworkEngine->SendDataBatch(MDM_BS_BATTLE_SERVICE, SUB_BS_C_BATTLE_OPTION, &BattleOption, sizeof(BattleOption), SERVICE_MASK_GAME);
			}

			return true;
		}
	}

	return true;
}

//转发消息
bool CAttemperEngineSink::OnTCPSocketMainForwardService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//注册服务
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{	
	case SUB_CS_C_REGISTER_SERVER:	//注册桌子
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterServer));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterServer)) return false;

			//消息定义
			CMD_CS_C_RegisterServer * pRegisterServer=(CMD_CS_C_RegisterServer *)pData;

			//查找桌子
			if (m_GlobalInfoManager.SearchServerItem(pRegisterServer->wServerID)!=NULL)
			{
				//变量定义
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//设置变量
				RegisterFailure.lErrorCode=0;
				StringCchCopy(RegisterFailure.szDescribeString,CountArray(RegisterFailure.szDescribeString), TEXT("已经存在相同标识的游戏桌子服务，桌子服务注册失败"));

				//发送消息
				WORD wStringSize=CountStringBuffer(RegisterFailure.szDescribeString);
				WORD wSendSize=sizeof(RegisterFailure)-sizeof(RegisterFailure.szDescribeString)+wStringSize;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_FAILURE,&RegisterFailure,wSendSize,NULL,0);

				//中断网络
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//设置绑定
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->wServiceModule=SERVICE_MODULE_GAME;
			(m_pBindParameter+wBindIndex)->wServiceID=pRegisterServer->wServerID;

			//变量定义
			tagGameServer GameServer;
			ZeroMemory(&GameServer,sizeof(GameServer));

			//构造数据
			GameServer.wKindID=pRegisterServer->wKindID;
			GameServer.wVisibleMask=pRegisterServer->wVisibleMask;
			GameServer.wServerID=pRegisterServer->wServerID;
			GameServer.wServerPort=pRegisterServer->wServerPort;
			GameServer.wServerType=pRegisterServer->wServerType;			
			GameServer.dwServerAddr=pRegisterServer->dwServerAddr;
			StringCchCopy(GameServer.szServerDomain,CountArray(GameServer.szServerDomain), pRegisterServer->szServerDomain);

			//注册桌子
			m_GlobalInfoManager.ActiveServerItem(dwSocketID,GameServer);

			//群发设置
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true, SERVICE_MASK_GAME);

			//构造选项
			CMD_BS_C_BattleOption BattleOption;
			BattleOption.bBattleService = m_bBattleService;

			//约战选项
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_BATTLE_OPTION, &BattleOption, sizeof(BattleOption), NULL, 0);

			//构造结构
			tagServerItem ServerItem;
			ZeroMemory(&ServerItem,sizeof(ServerItem));

			//设置变量
			ServerItem.wKindID=pRegisterServer->wKindID;
			ServerItem.wServerID=pRegisterServer->wServerID;
			ServerItem.wServerType=pRegisterServer->wServerType;
			ServerItem.wTableCount=pRegisterServer->wTableCount;			

			//添加房间
			if (m_BattleManager.AppendServer(ServerItem)==false)
			{
				//变量定义
				WORD wPacketSize=0;
				BYTE cbBuffer[SOCKET_BUFFER];

				//变量定义
				CTableItemArray TableItemArray;
				tagBattleTableItemEx * pTableItemEx=NULL;							

				do
				{
					//枚举桌子
					pTableItemEx=m_BattleManager.EnumTableItem(pTableItemEx);
					if (pTableItemEx==NULL) break;

					//判断桌子
					if (pTableItemEx->wServerID==pRegisterServer->wServerID)
					{
						//判断桌号
						if (pTableItemEx->wTableID==INVALID_TABLE)
						{
							TableItemArray.Add(pTableItemEx);
							continue;
						}

						//发送判断
						if (wPacketSize+sizeof(tagBattleTableItemEx)>sizeof(cbBuffer))
						{
							//发送数据
							m_pITCPNetworkEngine->SendData(dwSocketID,MDM_BS_BATTLE_SERVICE,SUB_BS_C_SYNCHRO_TABLE,cbBuffer,wPacketSize,NULL,0);
							wPacketSize=0;
						}

						//提取数据
						tagBattleTableItemEx * pTableItemMove=(tagBattleTableItemEx *)(cbBuffer+wPacketSize);
						CopyMemory(pTableItemMove,pTableItemEx,sizeof(tagBattleTableItemEx));

						//位置偏移
						wPacketSize+=sizeof(tagBattleTableItemEx);
					}

				} while (true);

				//发送数据
				if (wPacketSize>0)
				{
					m_pITCPNetworkEngine->SendData(dwSocketID,MDM_BS_BATTLE_SERVICE, SUB_BS_C_SYNCHRO_TABLE,cbBuffer,wPacketSize,NULL,0);
				}

				//移除桌子
				for (INT_PTR i=0; i<TableItemArray.GetCount(); i++)
				{
					pTableItemEx=TableItemArray[i];
					m_BattleManager.RemoveTable(pTableItemEx->dwBattleNum);
				}
			}
			else
			{
				//添加子项
				AppendSynchroItem(pRegisterServer->wServerID);

				//发送数据
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_BS_BATTLE_SERVICE, SUB_BS_C_SYNCHRO_TABLE,NULL,0);
			}

			//注册成功
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_SUCCESS, NULL, 0);

			return true;
		}	
	case SUB_CS_C_REGISTER_SERVICE:	//注册服务
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterService));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterService)) return false;

			//消息定义
			auto pRegisterService =(CMD_CS_C_RegisterService *)pData;

			//校验服务
			if (pRegisterService->wServiceModule != SERVICE_MODULE_GATE && pRegisterService->wServiceModule != SERVICE_MODULE_CLUB)
			{
				return false;
			}

			//查找服务
			if ((pRegisterService->wServiceModule == SERVICE_MODULE_GATE && m_GlobalInfoManager.SearchGateItem(pRegisterService->wServiceID) != NULL) || 
				(pRegisterService->wServiceModule == SERVICE_MODULE_CLUB && m_GlobalInfoManager.SearchClubItem(pRegisterService->wServiceID) != NULL))
			{
				//变量定义
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//设置变量
				RegisterFailure.lErrorCode=0;
				_sntprintf_s(RegisterFailure.szDescribeString,CountArray(RegisterFailure.szDescribeString), TEXT("已经存在相同标识的%s，服务注册失败"), pRegisterService->szServiceName);

				//发送消息
				WORD wStringSize=CountStringBuffer(RegisterFailure.szDescribeString);
				WORD wSendSize=sizeof(RegisterFailure)-sizeof(RegisterFailure.szDescribeString)+wStringSize;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_FAILURE,&RegisterFailure,wSendSize,NULL,0);

				//中断网络
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}			

			//设置绑定
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter + wBindIndex)->wServiceID = pRegisterService->wServiceID;
			(m_pBindParameter+wBindIndex)->wServiceModule= pRegisterService->wServiceModule;			

			//变量定义
			tagServiceItem ServiceItem;
			ZeroMemory(&ServiceItem,sizeof(ServiceItem));

			//构造数据
			ServiceItem.wServiceID= pRegisterService->wServiceID;
			ServiceItem.wServicePort= pRegisterService->wServicePort;
			ServiceItem.wServiceModule = pRegisterService->wServiceModule;
			ServiceItem.wMaxConnect= pRegisterService->wMaxConnect;
			ServiceItem.dwServiceAddr= pRegisterService->dwServiceAddr;
			StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), pRegisterService->szServiceName);
			StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain), pRegisterService->szServiceDomain);

			//注册网关
			if (ServiceItem.wServiceModule == SERVICE_MODULE_GATE)
			{
				m_GlobalInfoManager.ActiveGateItem(dwSocketID, ServiceItem);
			}			

			//注册茶馆
			if (ServiceItem.wServiceModule == SERVICE_MODULE_CLUB)
			{
				m_GlobalInfoManager.ActiveClubItem(dwSocketID, ServiceItem);
			}

			//注册成功
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_SUCCESS,NULL,0);			

			//群发设置
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);		

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
	case SUB_CS_C_SERVER_MODIFY:	//桌子修改
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_ServerModify));
			if (wDataSize!=sizeof(CMD_CS_C_ServerModify)) return false;

			//消息定义
			auto pServerModify=(CMD_CS_C_ServerModify *)pData;

			//获取参数
			WORD wBindIndex=LOWORD(dwSocketID);
			auto pBindParameter=(m_pBindParameter+wBindIndex);

			//连接效验
			ASSERT(pBindParameter->wServiceModule==SERVICE_MODULE_GAME);
			if (pBindParameter->wServiceModule!= SERVICE_MODULE_GAME) return false;

			//查找桌子
			WORD wServerID=pBindParameter->wServiceID;
			auto pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(wServerID);

			//桌子修改
			if (pGlobalServerItem!=NULL)
			{
				//设置变量
				pGlobalServerItem->m_GameServer.wKindID=pServerModify->wKindID;
				pGlobalServerItem->m_GameServer.wServerPort=pServerModify->wServerPort;				
				pGlobalServerItem->m_GameServer.dwMaxPlayer=pServerModify->dwMaxPlayer;
				pGlobalServerItem->m_GameServer.dwServerAddr=pServerModify->dwServerAddr;
				StringCchCopy(pGlobalServerItem->m_GameServer.szServerDomain,CountArray(pGlobalServerItem->m_GameServer.szServerDomain), pServerModify->szServerDomain);
			}

			return true;
		}
	case SUB_CS_C_LOGOUT_TOKEN:
		{
			//设置绑定
			WORD wBindIndex = LOWORD(dwSocketID);
			auto pBindParameter = (m_pBindParameter + wBindIndex);

			//校验服务
			if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return true;

			//提取数据
			auto pLogoutToken = (CMD_CS_C_LogoutToken *)pData;

			//查找网关
			auto pGlobalGateItem = m_GlobalInfoManager.SearchGateItem(pBindParameter->wServiceID);
			if (pGlobalGateItem != NULL)
			{
				pGlobalGateItem->RemoveTokenID(pLogoutToken->dwTokenID);
			}

			return true;
		}
	case SUB_CS_C_REGISTER_TOKEN:
		{
			//设置绑定
			WORD wBindIndex = LOWORD(dwSocketID);
			auto pBindParameter = (m_pBindParameter + wBindIndex);

			//移除令牌
			if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return true;

			//提取数据
			auto pRegisterToken = (CMD_CS_C_RegisterToken *)pData;

			//构造结构
			CMD_CS_S_RegisterToken RegisterToken;
			ZeroMemory(&RegisterToken, sizeof(RegisterToken));

			//默认成功
			RegisterToken.lResultCode = REGISTER_RESULT_FAILURE;			

			//查找网关
			auto pGlobalGateItem = m_GlobalInfoManager.SearchGateItem(pBindParameter->wServiceID);
			if (pGlobalGateItem != NULL)
			{
				//查询用户
				auto pTokenParameter = pGlobalGateItem->ActiveTokenID(pRegisterToken->dwTokenID);
				if (pTokenParameter != NULL)
				{
					//注册成功
					RegisterToken.lResultCode = REGISTER_RESULT_SUCCESS;

					//设置令牌		
					pTokenParameter->dwUserID = pRegisterToken->dwUserID;
					pTokenParameter->dwClientIP = pRegisterToken->dwClientIP;
				}
			}	

			//发送数据
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_REGISTER_TOKEN, &RegisterToken, sizeof(RegisterToken), &pRegisterToken->dwTokenID, 1);

			return true;
		}
	}

	return false;
}

//令牌参数
tagTokenParameter* CAttemperEngineSink::GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID)
{
	//获取信息
	WORD wBindIndex = LOWORD(dwSocketID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);
	if (pBindParameter == NULL) return NULL;

	//转发模式
	if (pBindParameter->wServiceModule == SERVICE_MODULE_GATE)
	{
		//查询网关
		auto pGlobalGateItem = m_GlobalInfoManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem == NULL) return NULL;

		//查询用户
		auto pTokenParameter = pGlobalGateItem->SearchTokenID(dwTokenID);
		if (pTokenParameter == NULL || pTokenParameter->dwTokenID != dwTokenID) return NULL;

		return pTokenParameter;
	}

	return NULL;
}

//发送请求
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//发送数据
	auto pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->m_UIControlSink.PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//构造桌子
VOID CAttemperEngineSink::ConstructTableItem(tagBattleTableItemEx * pTableItemEx, tagBattleTableItem * pTableItem)
{
	//基础配置
	pTableItem->dwClubID = pTableItemEx->dwClubID;
	pTableItem->wTableID = pTableItemEx->wTableID;
	pTableItem->wServerID = pTableItemEx->wServerID;
	pTableItem->dwOwnerID = pTableItemEx->dwOwnerID;
	pTableItem->dwBattleNum = pTableItemEx->dwBattleNum;
	pTableItem->dwCreateTime = pTableItemEx->dwCreateTime;
	pTableItem->wKindID = pTableItemEx->BattleConfig.wKindID;	
	pTableItem->bLockState = pTableItemEx->szPassword[0]!=0;
	pTableItem->lWinnerScore = pTableItemEx->BattleConfig.lWinnerScore;

	//配置信息
	pTableItem->dwConfigID = pTableItemEx->dwConfigID;
	CopyMemory(&pTableItem->BattleConfig, &pTableItemEx->BattleConfig, sizeof(pTableItem->BattleConfig));	
}

//构造桌子
WORD CAttemperEngineSink::ConstructTableList(CTableItemArray& TableItemArray, CTableParamArray& TableParamArray, INT &nStartIndex, BYTE cbBuffer[], WORD wBufferSize)
{
	//变量定义
	WORD wPacketSize = 0;
	tagBattleTableItem* pTableItem = NULL;
	tagBattleTableParam* pTableParam = NULL;
	tagBattleTableItemEx* pTableItemEx = NULL;
	tagBattleTableParamEx* pTableParamEx = NULL;

	//桌子列表
	for (INT_PTR i = nStartIndex; i < TableItemArray.GetCount(); i++)
	{
		//桌子过滤
		if (TableItemArray[i]->wTableID == INVALID_TABLE) continue;

		//发送判断
		if (wPacketSize + sizeof(tagBattleTableItem) + sizeof(tagBattleTableParam) + TABLE_USER_COUNT * sizeof(tagBattleTableUser) > wBufferSize)
		{
			nStartIndex = i;
			return wPacketSize;
		}

		//获取数据
		pTableItemEx = TableItemArray[i];
		pTableParamEx = TableParamArray[i];

		//桌子子项
		pTableItem = (tagBattleTableItem*)&cbBuffer[wPacketSize];

		//构造子项
		ConstructTableItem(pTableItemEx, pTableItem);

		//拷贝数据			
		wPacketSize += sizeof(tagBattleTableItem);

		//桌子参数
		pTableParam = (tagBattleTableParam*)&cbBuffer[wPacketSize];
		ZeroMemory(pTableParam,sizeof(tagBattleTableParam));

		//设置参数			
		pTableParam->dwBattleNum = pTableItemEx->dwBattleNum;
		pTableParam->dwStartTime = pTableParamEx->dwStartTime;
		pTableParam->wFinishCount = pTableParamEx->wFinishCount;
		pTableParam->cbGameStatus = pTableParamEx->cbGameStatus;

		//拷贝用户
		for (int i = 0; i < CountArray(pTableParamEx->TableUserItem); i++)
		{
			if (pTableParamEx->TableUserItem[i].dwUserID == 0) continue;
			CopyMemory(&pTableParam->TableUserItem[pTableParam->wUserCount++], &pTableParamEx->TableUserItem[i], sizeof(tagBattleTableUser));

			//退出判断										
			if (pTableParam->wUserCount == pTableParamEx->wUserCount) break;
		}

		//设置大小			
		wPacketSize += sizeof(tagBattleTableParam) + pTableParam->wUserCount * sizeof(tagBattleTableUser);
	}

	//更新索引
	nStartIndex = (INT)TableItemArray.GetCount();

	return wPacketSize;
}

//校验玩法
bool CAttemperEngineSink::VerifyBattleConfig(tagBattleConfig * pBattleConfig)
{
	//查找配置
	tagBattleBaseOptionEx* pBattleOptionEx = NULL;
	for (INT_PTR i = 0; i < m_BattleBaseOptionActive.GetCount(); i++)
	{
		if (m_BattleBaseOptionActive[i].BattleOption.wKindID == pBattleConfig->wKindID &&
			m_BattleBaseOptionActive[i].BattleOption.cbScoreKind == pBattleConfig->cbScoreKind &&
			m_BattleBaseOptionActive[i].BattleOption.wPlayerCount == pBattleConfig->wPlayerCount &&
			m_BattleBaseOptionActive[i].BattleOption.cbEmployScene == pBattleConfig->cbEmployScene)
		{
			pBattleOptionEx = &m_BattleBaseOptionActive[i];
			break;
		}
	}

	//配置不存在
	if (pBattleOptionEx == NULL)
	{
		CTraceService::TraceString(TEXT("找不到约战配置"), TraceLevel_Exception);

		return false;
	}

	//变量定义		    			
	WORD wPayType = pBattleConfig->wPayType;
	BYTE cbScoreKind = pBattleConfig->cbScoreKind;
	BYTE cbSettleKind = pBattleConfig->cbSettleKind;
	BYTE cbCurrencyKind = pBattleConfig->cbCurrencyKind;
	tagBattleBaseOption* pBattleOption = &pBattleOptionEx->BattleOption;

	//支付方式
	if ((wPayType & pBattleOption->wSuportPayType) == 0)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE-1"), TraceLevel_Exception);

		return false;
	}

	//按局数结算
	if (cbSettleKind == SETTLE_KIND_COUNT && (pBattleOption->wOptionMask & OPTION_MASK_COUNT) == 0)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE-2"), TraceLevel_Exception);

		return false;
	}

	//自定义结算
	if (cbSettleKind == SETTLE_KIND_CUSTOM && (pBattleOption->wOptionMask & OPTION_MASK_CUSTOM) == 0)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE-3"), TraceLevel_Exception);

		return false;
	}

	//货币类型
	if (cbCurrencyKind != CURRENCY_KIND_GOLD && cbCurrencyKind != CURRENCY_KIND_CARD)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-4"), TraceLevel_Exception);

		ASSERT(FALSE);
		return false;
	}

	//金币类型
	if ((pBattleOption->wOptionMask & OPTION_MASK_GOLD) == 0 && cbCurrencyKind == CURRENCY_KIND_GOLD)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-5"), TraceLevel_Exception);
		return false;
	}

	//房卡类型
	if ((pBattleOption->wOptionMask & OPTION_MASK_CARD) == 0 && cbCurrencyKind == CURRENCY_KIND_CARD)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-6"), TraceLevel_Exception);
		return false;
	}

	//语音校验
	if ((pBattleOption->wOptionMask & OPTION_MASK_VOICE) == 0 && pBattleConfig->cbAllowVoice == TRUE)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-7"), TraceLevel_Exception);
		return false;
	}

	//共开校验
	if ((pBattleOption->wOptionMask & OPTION_MASK_PUBLIC) == 0 && pBattleConfig->cbAllowPublic == TRUE)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-8"), TraceLevel_Exception);
		return false;
	}

	//同IP校验
	if ((pBattleOption->wOptionMask & OPTION_MASK_SAMEIP) == 0 && pBattleConfig->cbAllowSameIP == TRUE)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-9"), TraceLevel_Exception);
		return false;
	}

	//旁观校验
	if ((pBattleOption->wOptionMask & OPTION_MASK_LOOKON) == 0 && pBattleConfig->cbLookonMode != LOOKON_MODE_FORBID)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-10"), TraceLevel_Exception);
		return false;
	}

	//变量定义
	bool bOptionExists = false;
	BYTE cbConsumeIndex = 0;
	LONGLONG lConsumeCount = 0;

	//常规结算
	for (int i = 0; i < pBattleOption->cbItemCount; i++)
	{
		//局数结算
		if ((pBattleConfig->cbSettleKind == SETTLE_KIND_CUSTOM) ||
			(pBattleConfig->cbSettleKind == SETTLE_KIND_COUNT && pBattleOption->OptionItem[i].wPlayCount == pBattleConfig->wPlayCount))
		{
			//设置变量
			bOptionExists = true;

			//消耗房卡
			if (cbCurrencyKind == CURRENCY_KIND_CARD)
			{
				if (wPayType == PAY_TYPE_USER) lConsumeCount = pBattleOption->OptionItem[i].lRoomCardAA;
				if (wPayType == PAY_TYPE_OWNER) lConsumeCount = pBattleOption->OptionItem[i].lRoomCard;
				if (wPayType == PAY_TYPE_WINNER) lConsumeCount = pBattleOption->OptionItem[i].lRoomCard;
			}

			//消耗金币
			if (cbCurrencyKind == CURRENCY_KIND_GOLD)
			{
				lConsumeCount = pBattleConfig->lEntryCost;
			}

			//设置索引
			if (lConsumeCount != 0) cbConsumeIndex = i + 1;

			break;
		}		
	}

	//配置判断
	if (bOptionExists == false)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-11"), TraceLevel_Exception);

		return false;
	}

	//底分判断
	if ((pBattleConfig->lBaseScore<pBattleOption->lMinBaseScore || pBattleConfig->lBaseScore>pBattleOption->lMaxBaseScore))
	{
		CString strMessage;
		strMessage.Format(TEXT("SUB_GP_CREATE_TABLE_LUA-12 KIndID=%d,BaseScore=%I64d,MinBaseSocre=%I64d,MaxBaseScore=%I64d"), pBattleConfig->wKindID, pBattleConfig->lBaseScore, pBattleOption->lMinBaseScore, pBattleOption->lMaxBaseScore);
		CTraceService::TraceString(strMessage, TraceLevel_Exception);

		return false;
	}

	//入场费判断
	if (cbCurrencyKind == CURRENCY_KIND_GOLD && (pBattleConfig->lEntryCost<pBattleOption->lMinEntryCost || pBattleConfig->lEntryCost>pBattleOption->lMaxEntryCost))
	{
		CString strMessage;
		strMessage.Format(TEXT("SUB_GP_CREATE_TABLE_LUA-13 KIndID=%d,lEntryCost=%I64d,lMinEntryCost=%I64d,lMaxEntryCost=%I64d"), pBattleConfig->wKindID, pBattleConfig->lEntryCost, pBattleOption->lMinEntryCost, pBattleOption->lMaxEntryCost);
		CTraceService::TraceString(strMessage, TraceLevel_Exception);

		return false;
	}

	return true;
}

//校验配置
bool CAttemperEngineSink::VerifyBattleConfig(tagBattleConfig* pBattleConfig, tagBattleConfigEx& BattleConfig)
{
	//查找配置
	tagBattleBaseOptionEx* pBattleOptionEx = NULL;
	for (INT_PTR i = 0; i < m_BattleBaseOptionActive.GetCount(); i++)
	{
		if (m_BattleBaseOptionActive[i].BattleOption.wKindID == pBattleConfig->wKindID &&
			m_BattleBaseOptionActive[i].BattleOption.cbScoreKind == pBattleConfig->cbScoreKind &&
			m_BattleBaseOptionActive[i].BattleOption.wPlayerCount == pBattleConfig->wPlayerCount &&
			m_BattleBaseOptionActive[i].BattleOption.cbEmployScene == pBattleConfig->cbEmployScene)
		{
			pBattleOptionEx = &m_BattleBaseOptionActive[i];
			break;
		}
	}

	//配置不存在
	if (pBattleOptionEx == NULL)
	{
		CTraceService::TraceString(TEXT("找不到约战配置"), TraceLevel_Exception);

		return false;
	}

	//变量定义		    			
	WORD wPayType = pBattleConfig->wPayType;
	BYTE cbScoreKind = pBattleConfig->cbScoreKind;
	BYTE cbSettleKind = pBattleConfig->cbSettleKind;
	BYTE cbCurrencyKind = pBattleConfig->cbCurrencyKind;
	tagBattleBaseOption* pBattleOption = &pBattleOptionEx->BattleOption;

	//支付方式
	if ((wPayType & pBattleOption->wSuportPayType) == 0)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE-1"), TraceLevel_Exception);

		return false;
	}

	//按局数结算
	if (cbSettleKind == SETTLE_KIND_COUNT && (pBattleOption->wOptionMask&OPTION_MASK_COUNT)==0)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE-2"), TraceLevel_Exception);

		return false;
	}

	//自定义结算
	if (cbSettleKind == SETTLE_KIND_CUSTOM && (pBattleOption->wOptionMask&OPTION_MASK_CUSTOM)==0)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE-3"), TraceLevel_Exception);

		return false;
	}

	//货币类型
	if (cbCurrencyKind != CURRENCY_KIND_GOLD && cbCurrencyKind != CURRENCY_KIND_CARD)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-4"), TraceLevel_Exception);

		ASSERT(FALSE);
		return false;
	}

	//金币类型
	if ((pBattleOption->wOptionMask & OPTION_MASK_GOLD) == 0 && cbCurrencyKind == CURRENCY_KIND_GOLD)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-5"), TraceLevel_Exception);
		return false;		
	}

	//房卡类型
	if ((pBattleOption->wOptionMask & OPTION_MASK_CARD) == 0 && cbCurrencyKind == CURRENCY_KIND_CARD)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-6"), TraceLevel_Exception);
		return false;		
	}

	//语音校验
	if ((pBattleOption->wOptionMask & OPTION_MASK_VOICE) == 0 && pBattleConfig->cbAllowVoice==TRUE)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-7"), TraceLevel_Exception);
		return false;		
	}	

	//共开校验
	if ((pBattleOption->wOptionMask & OPTION_MASK_PUBLIC) == 0 && pBattleConfig->cbAllowPublic == TRUE)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-8"), TraceLevel_Exception);
		return false;
	}

	//同IP校验
	if ((pBattleOption->wOptionMask & OPTION_MASK_SAMEIP) == 0 && pBattleConfig->cbAllowSameIP == TRUE)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-9"), TraceLevel_Exception);
		return false;
	}

	//旁观校验
	if ((pBattleOption->wOptionMask & OPTION_MASK_LOOKON) == 0 && pBattleConfig->cbLookonMode != LOOKON_MODE_FORBID)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-10"), TraceLevel_Exception);
		return false;
	}



	//变量定义
	bool bOptionExists = false;
	BYTE cbConsumeIndex = 0;
	LONGLONG lConsumeCount = 0;

	//常规结算
	for (int i = 0; i < pBattleOption->cbItemCount; i++)
	{
		//局数结算
		if ((pBattleConfig->cbSettleKind == SETTLE_KIND_COUNT && pBattleOption->OptionItem[i].wPlayCount == pBattleConfig->wPlayCount) || 
			(pBattleConfig->cbSettleKind == SETTLE_KIND_CUSTOM))
		{
			//设置变量
			bOptionExists = true;

			//消耗房卡
			if (cbCurrencyKind == CURRENCY_KIND_CARD)
			{
				if (wPayType == PAY_TYPE_USER) lConsumeCount = pBattleOption->OptionItem[i].lRoomCardAA;
				if (wPayType == PAY_TYPE_OWNER) lConsumeCount = pBattleOption->OptionItem[i].lRoomCard;
				if (wPayType == PAY_TYPE_WINNER) lConsumeCount = pBattleOption->OptionItem[i].lRoomCard;
			}						

			//消耗金币
			if (cbCurrencyKind == CURRENCY_KIND_GOLD)
			{
				lConsumeCount = pBattleConfig->lEntryCost;
			}

			//设置索引
			if (lConsumeCount != 0) cbConsumeIndex = i + 1;

			break;
		}
	}

	//配置判断
	if (bOptionExists == false)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-11"), TraceLevel_Exception);

		return false;
	}	

	//底分判断
	if ((pBattleConfig->lBaseScore<pBattleOption->lMinBaseScore || pBattleConfig->lBaseScore>pBattleOption->lMaxBaseScore))
	{
		CString strMessage;
		strMessage.Format(TEXT("SUB_GP_CREATE_TABLE_LUA-12 KIndID=%d,BaseScore=%I64d,MinBaseSocre=%I64d,MaxBaseScore=%I64d"), pBattleConfig->wKindID, pBattleConfig->lBaseScore, pBattleOption->lMinBaseScore, pBattleOption->lMaxBaseScore);
		CTraceService::TraceString(strMessage, TraceLevel_Exception);

		return false;
	}

	//入场费判断
	if (cbCurrencyKind == CURRENCY_KIND_GOLD && (pBattleConfig->lEntryCost<pBattleOption->lMinEntryCost || pBattleConfig->lEntryCost>pBattleOption->lMaxEntryCost))
	{
		CString strMessage;
		strMessage.Format(TEXT("SUB_GP_CREATE_TABLE_LUA-13 KIndID=%d,lEntryCost=%I64d,lMinEntryCost=%I64d,lMaxEntryCost=%I64d"), pBattleConfig->wKindID, pBattleConfig->lEntryCost, pBattleOption->lMinEntryCost, pBattleOption->lMaxEntryCost);
		CTraceService::TraceString(strMessage, TraceLevel_Exception);

		return false;
	}	

	//初始化配置
	ZeroMemory(&BattleConfig, sizeof(BattleConfig));

	//构造配置	
	BattleConfig.wKindID = pBattleConfig->wKindID;
	BattleConfig.wPayType = pBattleConfig->wPayType;
	BattleConfig.wPlayCount = pBattleConfig->wPlayCount;
	BattleConfig.cbScoreKind = pBattleConfig->cbScoreKind;
	BattleConfig.cbEmployScene = pBattleConfig->cbEmployScene;
	BattleConfig.lBaseScore = pBattleConfig->lBaseScore;
	BattleConfig.lEntryCost = pBattleConfig->lEntryCost;
	BattleConfig.wPlayerCount = pBattleConfig->wPlayerCount;
	BattleConfig.cbSettleKind = pBattleConfig->cbSettleKind;
	BattleConfig.cbCurrencyKind = pBattleConfig->cbCurrencyKind;
	
	//选择配置
	BattleConfig.cbLookonMode = pBattleConfig->cbLookonMode;
	BattleConfig.cbAllowVoice = pBattleConfig->cbAllowVoice;
	BattleConfig.cbAllowPublic = pBattleConfig->cbAllowPublic;	
	BattleConfig.cbAllowSameIP = pBattleConfig->cbAllowSameIP;


	//kk jia start
	BattleConfig.lMinWin = pBattleConfig->lMinWin;
	BattleConfig.lWin2 = pBattleConfig->lWin2;
	BattleConfig.lWin3 = pBattleConfig->lWin3;
	BattleConfig.lRevenueRate1 = pBattleConfig->lRevenueRate1;
	BattleConfig.lRevenueRate2 = pBattleConfig->lRevenueRate2;
	BattleConfig.lRevenueRate3 = pBattleConfig->lRevenueRate3;
	//kk jia end
	//CString s;
	//s.Format(_T("BattleConfig创建桌子 m_lWinnerScore1:%I64d, m_lWinnerScore2:%I64d, m_lWinnerScore3:%I64d, m_lWinnerRevenue1:%I64d, m_lWinnerRevenue2:%I64d, m_lWinnerRevenue3:%I64d"), BattleConfig.lMinWin, BattleConfig.lWin2, BattleConfig.lWin3, BattleConfig.lRevenueRate1, BattleConfig.lRevenueRate2, BattleConfig.lRevenueRate3);
	//CTraceService::TraceString(s, TraceLevel_Exception);

	//费用信息
	BattleConfig.lConsumeCount = lConsumeCount;
	BattleConfig.cbConsumeIndex = cbConsumeIndex;

	//高级配置
	BattleConfig.cbAdvancedSize = pBattleConfig->cbAdvancedSize;
	CopyMemory(BattleConfig.cbAdvancedOption, pBattleConfig->cbAdvancedOption, BattleConfig.cbAdvancedSize);

	//特殊处理
	if (BattleConfig.cbAdvancedSize == 0)
	{
		ZeroMemory(BattleConfig.cbAdvancedOption, sizeof(BattleConfig.cbAdvancedOption));
	}

	//金币类型
	if (BattleConfig.cbCurrencyKind == CURRENCY_KIND_GOLD)
	{
		auto pAdvancedOption = (tagAdvancedOption*)BattleConfig.cbAdvancedOption;
		BattleConfig.wWinnerCount = pBattleOptionEx->BattleOption.wWinnerCount;
		BattleConfig.lWinnerRevenue = pBattleOptionEx->BattleOption.lWinnerRevenue;
		BattleConfig.lWinnerScore = (lConsumeCount * pBattleConfig->wPlayerCount * (1000L - BattleConfig.lWinnerRevenue- pAdvancedOption->lRevenueRate) / 1000L)/BattleConfig.wWinnerCount;
	}	
	else
	{
		//设置税收
		BattleConfig.lGameRevenue = pBattleOptionEx->BattleOption.lWinnerRevenue;
	}
		
	//附加信息		
	BattleConfig.wBattleOptionID = pBattleOptionEx->wOptionID;
	BattleConfig.wNotifyMask = NOTIFY_MASK_TABLE_PARAM | NOTIFY_MASK_TABLE_USER;

	//游戏配置
	BattleConfig.cbOptionSize = pBattleConfig->cbOptionSize;
	CopyMemory(BattleConfig.cbGameOption, pBattleConfig->cbGameOption, BattleConfig.cbOptionSize);	

	return true;
}

//添加同步
VOID CAttemperEngineSink::AppendSynchroItem(WORD wServerID)
{
	for (int i = 0; i < m_SynchroItemArray.GetCount(); i++)
	{
		if (m_SynchroItemArray[i] == wServerID)
		{
			return;
		}
	}

	m_SynchroItemArray.Add(wServerID);
}

//移除同步
VOID CAttemperEngineSink::RemoveSynchroItem(WORD wServerID)
{
	for (int i = 0; i < m_SynchroItemArray.GetCount(); i++)
	{
		if (m_SynchroItemArray[i] == wServerID)
		{
			m_SynchroItemArray.RemoveAt(i);
			break;
		}
	}
}

// 检验桌数
bool CAttemperEngineSink::VerifyTableCount(DWORD dwClubID, WORD wTableCount)
{
	//变量定义
	WORD wMaxTableCount = 0;

	//馆外约战
	if (dwClubID == 0)
	{
		wMaxTableCount = m_GlobalOption.wNormalMaxTableCount;
	}
	else
	{
		wMaxTableCount = m_GlobalOption.wClubMaxTableCount;		
	}

	return wMaxTableCount >= wTableCount; 
}

//操作失败
VOID CAttemperEngineSink::SendOperateFailure(WORD wServiceID, DWORD dwClubID, tagSocketInfo& SocketInfo, LONG lErrorCode, LPCTSTR pszDescribe)
{
	//变量定义
	CMD_BC_S_OperateFailure OperateFailure;
	ZeroMemory(&OperateFailure, sizeof(OperateFailure));

	//构造结构
	OperateFailure.dwClubID = dwClubID;
	OperateFailure.SocketInfo = SocketInfo;
	OperateFailure.lErrorCode = lErrorCode;
	StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), pszDescribe);

	//发送消息
	WORD wStringSize = CountStringBuffer(OperateFailure.szErrorMessage);
	WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage) + wStringSize;
	SendDataToClub(wServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_OPERATE_FAILURE, &OperateFailure, wSendSize);
}

//操作失败
VOID CAttemperEngineSink::SendOperateFailure(WORD wServiceID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe)
{
	//变量定义
	auto pRequestFailure = (CMD_CM_OperateFailure*)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer + sizeof(CMD_CM_OperateFailure), sizeof(m_cbBuffer) - sizeof(CMD_CM_OperateFailure));

	//设置变量
	pRequestFailure->lErrorCode = lErrorCode;
	pRequestFailure->wRequestCmdID = wRequestCmdID;

	//叠加信息
	if (pszDescribe != NULL)  SendPacket.AddPacket(pszDescribe, DTP_CM_DESCRIBE);

	//发送完成
	WORD wDataSize = sizeof(CMD_CM_OperateFailure) + SendPacket.GetDataSize();
	SendDataToGate(wServiceID, MDM_CM_SYSTEM, SUB_CM_OPERATE_FAILURE, pRequestFailure, wDataSize, &dwTokenID, 1);

	return;
}

//操作成功
VOID CAttemperEngineSink::SendOperateSuccess(WORD wServiceID, DWORD dwClubID, tagSocketInfo& SocketInfo, LONG lErrorCode, LPCTSTR pszDescribe)
{
	//变量定义
	CMD_BC_S_OperateSuccess OperateSuccess;
	ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

	//构造结构
	OperateSuccess.dwClubID = dwClubID;
	OperateSuccess.SocketInfo = SocketInfo;
	OperateSuccess.lOperateCode = lErrorCode;
	StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString), pszDescribe);

	//发送消息
	WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
	WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
	SendDataToClub(wServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_OPERATE_SUCCESS, &OperateSuccess, wSendSize);
}

//操作成功
VOID CAttemperEngineSink::SendOperateSuccess(WORD wServiceID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe)
{
	//变量定义
	auto pRequestSuccess = (CMD_CM_OperateSuccess*)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer + sizeof(CMD_CM_OperateSuccess), sizeof(m_cbBuffer) - sizeof(CMD_CM_OperateSuccess));

	//设置变量
	pRequestSuccess->lErrorCode = lErrorCode;
	pRequestSuccess->wRequestCmdID = wRequestCmdID;

	//叠加信息
	if (pszDescribe != NULL)  SendPacket.AddPacket(pszDescribe, DTP_CM_DESCRIBE);	

	//发送完成
	WORD wDataSize = sizeof(CMD_CM_OperateSuccess) + SendPacket.GetDataSize();
	SendDataToGate(wServiceID, MDM_CM_SYSTEM, SUB_CM_OPERATE_SUCCESS, pRequestSuccess, wDataSize, &dwTokenID, 1);

	return;
}

//游戏选项
WORD CAttemperEngineSink::ConstructGameOption(INT& nStartIndex, WORD wModuleID, BYTE cbEmployScene, BYTE cbBuffer[], WORD wBufferSize)
{
	//网络数据
	WORD wPacketSize = 0;
	WORD wOptionSize = 0;
	
	//枚举数据
	for (INT_PTR i = nStartIndex; i < m_BattleGameOptionActive.GetCount(); i++)
	{
		if (m_BattleGameOptionActive[i].BattleGameOption.cbEmployScene == cbEmployScene &&
			(wModuleID == INVALID_WORD || wModuleID == m_BattleGameOptionActive[i].BattleGameOption.wKindID))
		{
			//空串过滤
			if (m_BattleGameOptionActive[i].BattleGameOption.szOptionJson[0] == 0) continue;

			//计算长度
			wOptionSize = CountStringBuffer(m_BattleGameOptionActive[i].BattleGameOption.szOptionJson);

			//溢出判断
			if (wPacketSize + sizeof(tagBattleGameOption) - sizeof(m_BattleGameOptionActive[i].BattleGameOption.szOptionJson) + wOptionSize > wBufferSize)
			{
				nStartIndex = i;
				return wPacketSize;
			}

			auto pBattleGameOptionEx = &m_BattleGameOptionActive[i];

			//设置标识
			*(WORD*)(cbBuffer + wPacketSize) = m_BattleGameOptionActive[i].BattleGameOption.wKindID;
			wPacketSize += sizeof(WORD);

			//设置标识
			*(BYTE*)(cbBuffer + wPacketSize) = m_BattleGameOptionActive[i].BattleGameOption.cbEmployScene;
			wPacketSize += sizeof(BYTE);

			//设置长度
			*(WORD*)(cbBuffer + wPacketSize) = wOptionSize;
			wPacketSize += sizeof(WORD);

			//拷贝数据
			CopyMemory(cbBuffer + wPacketSize, m_BattleGameOptionActive[i].BattleGameOption.szOptionJson, wOptionSize);
			wPacketSize += wOptionSize;
		}
	}

	//设置索引
	nStartIndex = m_BattleGameOptionActive.GetCount();

	return wPacketSize;
}

//约战选项
WORD CAttemperEngineSink::ConstructBaseOption(INT& nStartIndex, WORD wModuleID, BYTE cbEmployScene, BYTE cbBuffer[], WORD wBufferSize)
{
	//变量定义
	WORD wPacketSize = 0;
	
	//枚举数据
	for (INT_PTR i = nStartIndex; i < m_BattleBaseOptionActive.GetCount(); i++)
	{
		if (m_BattleBaseOptionActive[i].BattleOption.cbEmployScene == cbEmployScene &&
			(wModuleID == INVALID_WORD || wModuleID == m_BattleBaseOptionActive[i].BattleOption.wKindID))
		{
			//校验大小
			if (wPacketSize + sizeof(tagBattleBaseOption) > wBufferSize)
			{
				nStartIndex = i;
				return wPacketSize;
			}

			//提取数据
			auto pBattleBaseOption = (tagBattleBaseOption*)(cbBuffer + wPacketSize);
			CopyMemory(pBattleBaseOption, &m_BattleBaseOptionActive[i].BattleOption, sizeof(tagBattleBaseOption));

			//位置偏移
			wPacketSize += sizeof(tagBattleBaseOption);
		}
	}

	nStartIndex = m_BattleBaseOptionActive.GetCount();

	return wPacketSize;
}

//桌子发送
bool CAttemperEngineSink::SendDataToGame(WORD wServerID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//查找桌子
	auto pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(wServerID);
	if (pGlobalServerItem==NULL) return false;

	//发送数据
	auto dwSocketID= pGlobalServerItem->GetSocketID();
	m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize,NULL,0);

	return true;
}

//茶馆发送
bool CAttemperEngineSink::SendDataToClub(WORD wServiceID, WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	//查找茶馆
	auto pGlobalClubItem = m_GlobalInfoManager.SearchClubItem(wServiceID);
	if (pGlobalClubItem == NULL) return false;

	//发送数据
	auto dwSocketID = pGlobalClubItem->GetSocketID();
	m_pITCPNetworkEngine->SendData(dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize, NULL, 0);

	return true;
}

//网关发送
bool CAttemperEngineSink::SendDataToGate(WORD wServiceID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	//查找桌子
	auto pGlobalGateItem = m_GlobalInfoManager.SearchGateItem(wServiceID);
	if (pGlobalGateItem == NULL) return false;

	//发送数据
	DWORD dwSocketID = pGlobalGateItem->GetSocketID();
	m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(wMainCmdID, SERVICE_MODULE_BATTLE), wSubCmdID, pData, wDataSize, pdwTokenID, wTokenCount);

	return true;
}

//约战服务
bool CAttemperEngineSink::OnTCPNetWorkMainGBBattleService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//获取信息
	WORD wBindIndex = LOWORD(dwSocketID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);

	//获取参数
	auto pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	switch (wSubCmdID)
	{
	case SUB_GB_C_QUERY_OPTION:			//查询配置
	{
		//参数校验
		ASSERT (wDataSize == sizeof(CMD_GB_C_QueryOption));
		if (wDataSize != sizeof(CMD_GB_C_QueryOption)) return true;

		//提取数据
		auto pQueryOption = (CMD_GB_C_QueryOption*)pData;

		//变量定义
		INT nStartIndex = 0;

		//基础配置
		while (nStartIndex < m_BattleBaseOptionActive.GetCount())
		{
			auto wPacketSize = ConstructBaseOption(nStartIndex, pQueryOption->wKindID, EMPLOY_SCENE_CLUB_OUTSIDE, m_cbBuffer,CountArray(m_cbBuffer));
			if (wPacketSize > 0)
			{
				//发送数据
				SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_BATTLE_BASE_OPTION, m_cbBuffer, wPacketSize, &dwTokenID, 1);
			}
		}

		nStartIndex = 0;

		//游戏配置
		while (nStartIndex < m_BattleGameOptionActive.GetCount())
		{
			auto wPacketSize = ConstructGameOption(nStartIndex, pQueryOption->wKindID, EMPLOY_SCENE_CLUB_OUTSIDE, m_cbBuffer, CountArray(m_cbBuffer));
			if (wPacketSize > 0)
			{
				//发送数据
				SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_BATTLE_GAME_OPTION, m_cbBuffer, wPacketSize, &dwTokenID, 1);
			}
		}
		
		//发送数据
		SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_BATTLE_OPTION_FINISH, NULL, 0, &dwTokenID, 1);

		return true;
	}
	case SUB_GB_C_QUERY_TABLE:			//查询桌子	
	{
		//数值定义
		CTableItemArray TableItemArray;
		CTableParamArray TableParamArray;

		//获取桌子				
		m_BattleManager.GetUserTableList(pTokenParameter->dwUserID, TableItemArray, TableParamArray);

		//变量定义
		INT nStartIndex = 0;
		
		//发送列表
		while (nStartIndex < TableItemArray.GetCount())
		{
			auto wPacketSize = ConstructTableList(TableItemArray, TableParamArray, nStartIndex, m_cbBuffer, CountArray(m_cbBuffer));
			if (wPacketSize > 0)
			{
				//发送数据
				SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_ITEM, m_cbBuffer, wPacketSize, &dwTokenID, 1);
				wPacketSize = 0;
			}
		}				

		//桌子完成
		SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_FINISH, NULL, 0, &dwTokenID, 1);

		return true;
	}	
	case SUB_GB_C_SEARCH_TABLE:			//查找桌子	
	{
		//参数校验
		ASSERT (sizeof(CMD_GB_C_SearchTable) == wDataSize);
		if (sizeof(CMD_GB_C_SearchTable) != wDataSize) return false;

		//提取数据
		auto pSearchTable = (CMD_GB_C_SearchTable*)pData;

		//查询桌子
		auto pTableItemEx = m_BattleManager.GetTableItem(pSearchTable->dwBattleNum);
		if (pTableItemEx == NULL || pTableItemEx->wTableID == INVALID_TABLE)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_QUERY_TABLE, BATTLE_CODE_QUERY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0013, TEXT("抱歉，系统没有查询到您进入的桌子，请确认桌子号再次尝试！")));

			return true;
		}

		//查询参数
		auto pTableParam = m_BattleManager.GetTableParam(pSearchTable->dwBattleNum);
		if (pTableParam == NULL)
		{
			//操作失败
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_QUERY_TABLE, BATTLE_CODE_QUERY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0013, TEXT("抱歉，系统没有查询到您进入的桌子，请确认桌子号再次尝试！")));

			return true;
		}

		//校验用户
		if (pTableItemEx->dwClubID != 0)
		{
			//构造对象
			CMD_BC_S_UserEfficacy UserEfficacy = {};

			//构造结构						
			UserEfficacy.dwUserID = pTokenParameter->dwUserID;
			UserEfficacy.dwClubID = pTableItemEx->dwClubID;
			UserEfficacy.dwBattleNum = pTableItemEx->dwBattleNum;
			UserEfficacy.wBattleID = m_pServiceOption->wServiceID;

			//网络信息
			UserEfficacy.UserSocketInfo.dwTokenID = dwTokenID;
			UserEfficacy.UserSocketInfo.wGateID = pBindParameter->wServiceID;

			//发送消息
			SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_USER_EFFICACY, &UserEfficacy, sizeof(UserEfficacy));

			return true;
		}
			

		//变量定义
		auto pBattleTableItem = (tagBattleTableItem*)m_cbBuffer;
		auto pBattleTableParam = (tagBattleTableParam*)(pBattleTableItem + 1);

		//构造桌子
		ConstructTableItem(pTableItemEx, pBattleTableItem);

		//桌子参数
		pBattleTableParam->wUserCount = 0;
		pBattleTableParam->dwStartTime = pTableParam->dwStartTime;
		pBattleTableParam->wFinishCount = pTableParam->wFinishCount;
		pBattleTableParam->cbGameStatus = pTableParam->cbGameStatus;
		pBattleTableParam->dwBattleNum = pBattleTableItem->dwBattleNum;

		//发送消息
		SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_SEARCH_RESULT, m_cbBuffer, sizeof(tagBattleTableItem) + sizeof(tagBattleTableParam), &dwTokenID, 1);

		return true;
	}
	case SUB_GB_C_CREATE_TABLE:			//创建桌子	
	{
		//参数校验
		ASSERT(sizeof(CMD_GB_C_CreateTable) >= wDataSize);
		if (sizeof(CMD_GB_C_CreateTable) < wDataSize) return false;

		//提取数据
		auto pCreateTable = (CMD_GB_C_CreateTable*)pData;

		//服务状态
		if (m_bBattleService == false)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_BATTLE_0002, TEXT("系统维护中，约战桌子暂时停止服务。请在维护完成后重试！")));

			return true;
		}

		//变量定义
		tagBattleConfigEx BattleConfig;
		ZeroMemory(&BattleConfig, sizeof(BattleConfig));

		//获取配置
		auto pBattleConfig = &pCreateTable->BattleConfig;

		//校验配置
		if (VerifyBattleConfig(pBattleConfig, BattleConfig) == false)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_BATTLE_0003, TEXT("桌子创建失败！")));

			return true;
		}

		//判断桌数
		auto wTableCount = m_BattleManager.GetTableCountByKindID(pTokenParameter->dwUserID, 0, 0);
		if (VerifyTableCount(0, wTableCount + 1) == false)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_BATTLE_0004, TEXT("抱歉，您创建的桌子数目已经达到上限，如有疑问，请联系客服！")));

			return true;
		}

		//创建桌子
		auto pTableItemEx = m_BattleManager.CreateTable(pTokenParameter->dwUserID,0, BattleConfig);
		if (pTableItemEx == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessageV(MIM_BATTLE_0005, TEXT("抱歉，桌子创建失败[%d]，请稍后再次尝试！"), 2));

			return true;
		}

		//保存桌主
		auto pTableOwner = m_BattleManager.CreateTableOwner(pTableItemEx->dwBattleNum);
		if (pTableOwner == NULL)
		{
			//移除桌子
			m_BattleManager.RemoveTable(pTableItemEx->dwBattleNum);

			//操作失败
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessageV(MIM_BATTLE_0005, TEXT("抱歉，桌子创建失败[%d]，请稍后再次尝试！"), 3));

			return true;
		}

		//获取信息
		auto pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL)
		{
			//移除桌子
			m_BattleManager.RemoveTable(pTableItemEx->dwBattleNum);
			m_BattleManager.RemoveTableOwner(pTableItemEx->dwBattleNum);

			//操作失败
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessageV(MIM_BATTLE_0005, TEXT("抱歉，桌子创建失败[%d]，请稍后再次尝试！"), 4));

			return true;
		}

		//拷贝密码
		StringCchCopy(pTableItemEx->szPassword, CountArray(pTableItemEx->szPassword), pCreateTable->szTablePassword);

		//基本信息			
		pTableOwner->dwUserID = pTokenParameter->dwUserID;
		
		//网络信息
		pTableOwner->SocketInfo.dwTokenID = dwTokenID;
		pTableOwner->SocketInfo.wGateID = pBindParameter->wServiceID;

		//构造结构
		CMD_BS_C_CreateTable CreateTable;
		ZeroMemory(&CreateTable, sizeof(CreateTable));

		//用户信息			
		CreateTable.dwUserID = pTokenParameter->dwUserID;
		StringCchCopy(CreateTable.szPassword, CountArray(CreateTable.szPassword), pTableItemEx->szPassword);

		//创建信息
		CreateTable.dwConfigID = pTableItemEx->dwConfigID;
		CreateTable.dwBattleNum = pTableItemEx->dwBattleNum;		
		CopyMemory(&CreateTable.BattleConfig, &BattleConfig, sizeof(CreateTable.BattleConfig));

		//发送消息
		SendDataToGame(pTableItemEx->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_CREATE_TABLE, &CreateTable, sizeof(CreateTable));

		return true;
	}
	case SUB_GB_C_MODIFY_TABLE:			//修改桌子
	{
		//参数校验
		ASSERT (sizeof(CMD_GB_C_ModifyTable) >= wDataSize);
		if (sizeof(CMD_GB_C_ModifyTable) < wDataSize) return false;

		//提取数据
		auto pModifyTable = (CMD_GB_C_ModifyTable*)pData;

		//服务状态
		if (m_bBattleService == false)
		{
			//操作失败
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_MODIFY_TABLE, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0002, TEXT("系统维护中，约战桌子暂时停止服务。请在维护完成后重试！")));

			return true;
		}

		//查找桌子
		auto pTableItemEx = m_BattleManager.GetTableItem(pModifyTable->dwBattleNum);
		if (pTableItemEx == NULL)
		{
			//操作失败
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_MODIFY_TABLE, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0009, TEXT("抱歉，系统未查询到您要修改的桌子，请刷新后再次尝试！")));

			return true;
		}

		//校验状态
		auto pTableParamEx = m_BattleManager.GetTableParam(pModifyTable->dwBattleNum);
		if (pTableParamEx->wFinishCount > 0)
		{
			//操作失败
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_MODIFY_TABLE, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0018, TEXT("游戏已经开始，桌子配置修改失败！")));

			return true;
		}

		//校验用户			
		if (pTableItemEx->dwOwnerID != pTokenParameter->dwUserID)
		{
			//操作失败
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_MODIFY_TABLE, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0010, TEXT("抱歉，您不是该茶社的老板，没有权限修改桌子配置！")));

			return true;
		}

		//变量定义
		tagBattleConfigEx BattleConfig;
		ZeroMemory(&BattleConfig, sizeof(BattleConfig));

		//获取配置
		auto pBattleConfig = &pModifyTable->BattleConfig;

		//校验配置
		if (VerifyBattleConfig(pBattleConfig, BattleConfig) == false)
		{
			//操作失败
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_MODIFY_TABLE, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0011, TEXT("桌子修改失败！")));

			return true;
		}

		//构造结构
		CMD_BS_C_ModifyTable ModifyTable;
		ZeroMemory(&ModifyTable, sizeof(ModifyTable));

		//用户信息			
		ModifyTable.wTableID = pTableItemEx->wTableID;
		ModifyTable.dwUserID = pTokenParameter->dwUserID;
		ModifyTable.dwBattleNum = pModifyTable->dwBattleNum;

		//约战配置
		CopyMemory(&ModifyTable.BattleConfig, &BattleConfig, sizeof(BattleConfig));

		//网络信息
		ModifyTable.SocketInfo.dwTokenID = dwTokenID;
		ModifyTable.SocketInfo.wGateID = pBindParameter->wServiceID;

		//发送消息
		SendDataToGame(pTableItemEx->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_MODIFY_TABLE, &ModifyTable, sizeof(ModifyTable));

		return true;
	}
	case SUB_GB_C_DISMISS_TABLE:		//解散桌子
	{
		//参数校验
		ASSERT (sizeof(CMD_GB_C_DismissTable) == wDataSize);
		if (sizeof(CMD_GB_C_DismissTable) != wDataSize) return false;

		//提取数据
		auto pDismissTable = (CMD_GB_C_DismissTable*)pData;

		//查找桌子
		auto pTableItemEx = m_BattleManager.GetTableItem(pDismissTable->dwBattleNum);
		if (pTableItemEx == NULL)
		{
			//操作失败
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_DISMISS_TABLE, BATTLE_CODE_DISMISS_FAILED, CWHService::FormatMessage(MIM_BATTLE_0012, TEXT("抱歉，系统未查询到您要解散的桌子，请刷新后再次尝试！")));

			return true;
		}

		//非房主
		if (pTokenParameter->dwUserID != pTableItemEx->dwOwnerID)
		{
			//操作失败
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_DISMISS_TABLE, BATTLE_CODE_DISMISS_FAILED, CWHService::FormatMessage(MIM_BATTLE_0014, TEXT("抱歉，您不是该桌子的房主，没有权限解散！")));

			return true;
		}

		//构造结构
		CMD_BS_C_DismissTable DismissTable;
		ZeroMemory(&DismissTable, sizeof(DismissTable));

		//桌子信息
		DismissTable.SocketInfo.dwTokenID = dwTokenID;
		DismissTable.SocketInfo.wGateID = pBindParameter->wServiceID;

		//设置变量
		DismissTable.wTableID = pTableItemEx->wTableID;
		DismissTable.dwUserID = pTokenParameter->dwUserID;
		DismissTable.dwBattleNum = pDismissTable->dwBattleNum;

		//发送命令
		SendDataToGame(pTableItemEx->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_DISMISS_TABLE, &DismissTable, sizeof(DismissTable));

		return true;
	}	
	case SUB_GB_C_REQUEST_TABLELIST:	//桌子列表
	{
		//参数校验
		ASSERT (sizeof(CMD_GB_C_RequestTableList) == wDataSize);
		if (sizeof(CMD_GB_C_RequestTableList) != wDataSize) return false;

		//提取数据
		auto pRequestTableList = (CMD_GB_C_RequestTableList*)pData;

		//调整数量
		int nTableCount = __min(pRequestTableList->wTableCount, MAX_TABLE_COUNT_REQUEST);

		//查询桌子
		CTableItemArray TableItemArray;
		CTableParamArray TableParamArray;

		//获取桌子				
		m_BattleManager.GetRandTableList(nTableCount, TableItemArray, TableParamArray);

		//变量定义
		INT nStartIndex = 0;

		//发送列表
		while (nStartIndex < TableItemArray.GetCount())
		{
			auto wPacketSize = ConstructTableList(TableItemArray, TableParamArray, nStartIndex, m_cbBuffer, CountArray(m_cbBuffer));
			if (wPacketSize > 0)
			{
				//发送数据
				SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_ITEM, m_cbBuffer, wPacketSize, &dwTokenID, 1);
				wPacketSize = 0;
			}
		}

		//桌子完成
		SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_FINISH, NULL, 0, &dwTokenID, 1);

		return true;
	}
	case SUB_GB_C_REQUEST_TABLEPARAM:	//桌子参数
	{
		//参数校验
		ASSERT (wDataSize > sizeof(CMD_GB_C_RequestTableParam));
		if (wDataSize <= sizeof(CMD_GB_C_RequestTableParam)) return true;

		//提取数据
		auto pRequestTableParam = (CMD_GB_C_RequestTableParam*)pData;

		//校验数量
		ASSERT (pRequestTableParam->wTableCount <= MAX_TABLE_COUNT_REQUEST);
		if (pRequestTableParam->wTableCount > MAX_TABLE_COUNT_REQUEST) return true;

		//数据校验
		ASSERT (pRequestTableParam->wTableCount * sizeof(pRequestTableParam->dwBattleNum[0]) + sizeof(CMD_GB_C_RequestTableParam) == wDataSize);
		if (pRequestTableParam->wTableCount * sizeof(pRequestTableParam->dwBattleNum[0]) + sizeof(CMD_GB_C_RequestTableParam) != wDataSize) return true;

		//缓冲数据
		WORD wPacketSize = 0;

		for (int i = 0; i < pRequestTableParam->wTableCount; i++)
		{
			//桌子状态
			auto pTableParamEx = m_BattleManager.GetTableParam(pRequestTableParam->dwBattleNum[i]);
			if (pTableParamEx == NULL) continue;

			//发送判断
			if (wPacketSize + sizeof(tagBattleTableParam) + sizeof(tagBattleTableUser) * TABLE_USER_COUNT > sizeof(m_cbBuffer))
			{
				//发送数据
				SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_PARAM, m_cbBuffer, wPacketSize, &dwTokenID, 1);

				//设置变量
				wPacketSize = 0;
			}

			//获取对象			
			auto pTableParam = (tagBattleTableParam*)&m_cbBuffer[wPacketSize];

			//设置变量
			ZeroMemory(pTableParam, sizeof(tagBattleTableParam));
			pTableParam->dwBattleNum = pRequestTableParam->dwBattleNum[i];
			pTableParam->dwStartTime = pTableParamEx->dwStartTime;
			pTableParam->wFinishCount = pTableParamEx->wFinishCount;
			pTableParam->cbGameStatus = pTableParamEx->cbGameStatus;

			//拷贝用户
			for (int i = 0; i < CountArray(pTableParamEx->TableUserItem); i++)
			{
				if (pTableParamEx->TableUserItem[i].dwUserID == 0) continue;
				CopyMemory(&pTableParam->TableUserItem[pTableParam->wUserCount++], &pTableParamEx->TableUserItem[i], sizeof(tagBattleTableUser));

				//退出判断										
				if (pTableParam->wUserCount == pTableParamEx->wUserCount) break;
			}

			//设置大小			
			wPacketSize += sizeof(tagBattleTableParam)+pTableParam->wUserCount*sizeof(tagBattleTableUser);
		}

		//发送数据
		if (wPacketSize > 0)
		{
			SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_PARAM, m_cbBuffer, wPacketSize, &dwTokenID, 1);
		}

		//桌子完成
		SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_FINISH, NULL, 0, &dwTokenID, 1);

		return true;		
	} 	
	}
	return false;
}

//约战服务
bool CAttemperEngineSink::OnTCPNetWorkMainBSBattleService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{	
	case SUB_BS_S_CREATE_TABLE:		//创建桌子
	{
		//参数校验
		ASSERT(sizeof(CMD_BS_S_CreateTable) >= wDataSize);
		if (sizeof(CMD_BS_S_CreateTable) < wDataSize) return false;

		//提取数据
		auto pCreateTable = (CMD_BS_S_CreateTable *)pData;

		//查找桌子
		auto pTableItemEx = m_BattleManager.GetTableItem(pCreateTable->dwBattleNum);
		if (pTableItemEx == NULL) return true;

		//桌子状态
		auto pTableParamEx = m_BattleManager.GetTableParam(pCreateTable->dwBattleNum);
		if (pTableParamEx == NULL) return true;

		//查找桌主
		auto pTableOwner = m_BattleManager.SearchTableOwner(pCreateTable->dwBattleNum);
		if (pTableOwner == NULL) return true;

		//提取对象
		auto pSocketInfo = &pTableOwner->SocketInfo;

		//创建失败
		if (pCreateTable->lResultCode != 0)
		{
			//变量定义
			bool bCreateSuccess = false;

			//桌子未找到
			if (pCreateTable->lResultCode == BATTLE_CODE_TABLE_UNFOUND)
			{
				//创建桌子
				tagBattleTableItemEx * pNewTableItem = m_BattleManager.CreateTable(pTableItemEx->dwOwnerID, pTableItemEx->dwClubID, pTableItemEx->BattleConfig, pTableItemEx->wServerID);
				if (pNewTableItem != NULL)
				{
					//保存桌主
					tagTableOwner * pNewTableOwner = m_BattleManager.CreateTableOwner(pNewTableItem->dwBattleNum);
					if (pNewTableOwner != NULL)
					{
						//设置标识
						bCreateSuccess = true;

						//玩法信息
						pNewTableItem->dwConfigID = pTableItemEx->dwConfigID;

						//茶舍信息
						pNewTableItem->wClubServiceID = pTableItemEx->wClubServiceID;
						pNewTableItem->wSwitchOption = pTableItemEx->wSwitchOption;
						pNewTableItem->dwClubCreaterID = pTableItemEx->dwClubCreaterID;

						//桌子密码
						StringCchCopy(pNewTableItem->szPassword, CountArray(pNewTableItem->szPassword), pTableItemEx->szPassword);

						//拷贝信息
						CopyMemory(pNewTableOwner, pTableOwner, sizeof(tagTableOwner));

						//变量定义
						CMD_BS_C_CreateTable CreateTable;
						ZeroMemory(&CreateTable, sizeof(CreateTable));

						//用户信息			
						CreateTable.dwUserID = pNewTableItem->dwOwnerID;
						StringCchCopy(CreateTable.szPassword, CountArray(CreateTable.szPassword), pNewTableItem->szPassword);

						//茶舍信息			
						CreateTable.dwClubID = pNewTableItem->dwClubID;
						CreateTable.wSwitchOption = pNewTableItem->wSwitchOption;
						CreateTable.wClubServiceID = pNewTableItem->wClubServiceID;
						CreateTable.dwClubCreaterID = pNewTableItem->dwClubCreaterID;							

						//创建信息
						CreateTable.dwConfigID = pNewTableItem->dwConfigID;
						CreateTable.dwBattleNum = pNewTableItem->dwBattleNum;
						CopyMemory(&CreateTable.BattleConfig, &pNewTableItem->BattleConfig, sizeof(CreateTable.BattleConfig));

						//发送消息
						SendDataToGame(pNewTableItem->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_CREATE_TABLE, &CreateTable, sizeof(CreateTable));
					}
					else
					{
						//移除桌子							
						m_BattleManager.RemoveTable(pNewTableItem->dwBattleNum);
					}
				}
			}

			//创建失败
			if (bCreateSuccess == false)
			{
				//操作失败
				if (pTableItemEx->dwClubID != 0)
				{
					SendOperateFailure(pTableItemEx->wClubServiceID, pTableItemEx->dwClubID, *pSocketInfo, BATTLE_CODE_CREATE_FAILED, pCreateTable->szErrorMessage);
				}
				else
				{
					SendOperateFailure(pSocketInfo->wGateID, pSocketInfo->dwTokenID, SUB_GB_C_CREATE_TABLE, pCreateTable->lResultCode, pCreateTable->szErrorMessage);					
				}
			}

			//移除桌子
			m_BattleManager.RemoveTable(pCreateTable->dwBattleNum);
		}
		else
		{
			//设置变量
			pTableItemEx->wTableID = pCreateTable->wTableID;
			pTableItemEx->dwCreateTime = pCreateTable->dwCreateTime;

			//馆主支付
			if (pTableItemEx->BattleConfig.wPayType == PAY_TYPE_OWNER && pCreateTable->dwPayUserID==pTableOwner->dwUserID)
			{
				//构造财富
				CMD_CM_WealthUpdate* pWealthUpdate = (CMD_CM_WealthUpdate*)m_cbBuffer;
				ZeroMemory(pWealthUpdate, sizeof(CMD_CM_WealthUpdate));

				//金币支付
				if (pTableItemEx->BattleConfig.cbCurrencyKind == CURRENCY_KIND_GOLD)
				{
					pWealthUpdate->WealthItem[pWealthUpdate->wItemCount].wMoneyID = GOODS_ID_GOLD;
					pWealthUpdate->WealthItem[pWealthUpdate->wItemCount++].lBalance = pCreateTable->lUserGold;
				}

				//房卡支付
				if (pTableItemEx->BattleConfig.cbCurrencyKind == CURRENCY_KIND_CARD)
				{
					pWealthUpdate->WealthItem[pWealthUpdate->wItemCount].wMoneyID = GOODS_ID_CARD;
					pWealthUpdate->WealthItem[pWealthUpdate->wItemCount++].lBalance = pCreateTable->lUserCard;
				}

				//发送消息
				WORD wHeadSize = sizeof(CMD_CM_WealthUpdate);
				WORD wDataSize = pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]);
				SendDataToGate(pSocketInfo->wGateID, MDM_CM_SYSTEM, SUB_CM_WEALTH_UPDATE, pWealthUpdate, wHeadSize + wDataSize, &pSocketInfo->dwTokenID, 1);
			}
			
			//变量定义
			auto pBattleTableItem = (tagBattleTableItem*)m_cbBuffer;
			auto pBattleTableParam = (tagBattleTableParam*)(pBattleTableItem+1);
		
			//构造桌子
			ConstructTableItem(pTableItemEx, pBattleTableItem);

			//桌子参数
			pBattleTableParam->dwStartTime = 0;
			pBattleTableParam->wUserCount = 0;
			pBattleTableParam->wFinishCount = 0;
			pBattleTableParam->cbGameStatus = FALSE;
			pBattleTableParam->dwBattleNum = pBattleTableItem->dwBattleNum;

			//操作失败
			if (pTableItemEx->dwClubID != 0)
			{
				//操作成功
				SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_TABLE_CREATE, m_cbBuffer, sizeof(tagBattleTableItem)+sizeof(tagBattleTableParam));

				//操作成功
				SendOperateSuccess(pTableItemEx->wClubServiceID, pTableItemEx->dwClubID, *pSocketInfo, pCreateTable->lResultCode, pCreateTable->szErrorMessage);
			}
			else
			{
				//发送消息
				SendDataToGate(pSocketInfo->wGateID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_CREATE, m_cbBuffer, sizeof(tagBattleTableItem) + sizeof(tagBattleTableParam), &pSocketInfo->dwTokenID, 1);

				//操作成功
				SendOperateSuccess(pSocketInfo->wGateID, pSocketInfo->dwTokenID, SUB_GB_C_CREATE_TABLE, pCreateTable->lResultCode, pCreateTable->szErrorMessage);
			}
		}

		//移除桌主
		m_BattleManager.RemoveTableOwner(pCreateTable->dwBattleNum);

		return true;
	}	
	case SUB_BS_S_MODIFY_TABLE:		//修改桌子
	{
		//参数校验
		ASSERT(sizeof(CMD_BS_S_ModifyTable) >= wDataSize);
		if (sizeof(CMD_BS_S_ModifyTable) < wDataSize) return false;

		//提取数据
		auto pModifyTable = (CMD_BS_S_ModifyTable*)pData;

		//查找桌子
		auto pTableItemEx = m_BattleManager.GetTableItem(pModifyTable->dwBattleNum);
		if (pTableItemEx == NULL) return true;

		//网络信息
		auto pSocketInfo = &pModifyTable->SocketInfo;

		//修改修改
		if (pModifyTable->cbResultCode != 0)
		{
			if (pTableItemEx->dwClubID != 0)
			{
				SendOperateFailure(pTableItemEx->wClubServiceID, pTableItemEx->dwClubID, *pSocketInfo, pModifyTable->cbResultCode, pModifyTable->szErrorMessage);
			}
			else
			{
				SendOperateFailure(pSocketInfo->wGateID, pSocketInfo->dwTokenID, SUB_BS_C_MODIFY_TABLE, pModifyTable->cbResultCode, pModifyTable->szErrorMessage);
			}
		}
		else
		{
			//查找桌子
			auto pTableItemEx = m_BattleManager.GetTableItem(pModifyTable->dwBattleNum);
			if (pTableItemEx == NULL) return true;			

			//拷贝配置
			CopyMemory(&pTableItemEx->BattleConfig, &pModifyTable->BattleConfig, sizeof(pTableItemEx->BattleConfig));

			//构造对象
			tagBattleTableItem BattleTableItem = {};

			//构造桌子
			ConstructTableItem(pTableItemEx, &BattleTableItem);

			//校验茶社
			if (pTableItemEx->dwClubID != 0)
			{
				//发送消息
				SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_TABLE_UPDATE, &BattleTableItem, sizeof(BattleTableItem));

				//操作成功
				SendOperateSuccess(pTableItemEx->wClubServiceID, pTableItemEx->dwClubID, *pSocketInfo, pModifyTable->cbResultCode, pModifyTable->szErrorMessage);
			}
			else
			{
				//发送消息
				SendDataToGate(pSocketInfo->wGateID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_TABLE_UPDATE, &BattleTableItem, sizeof(BattleTableItem), &pSocketInfo->dwTokenID, 1);

				//操作成功
				SendOperateSuccess(pSocketInfo->wGateID, pSocketInfo->dwTokenID, SUB_BS_C_MODIFY_TABLE, pModifyTable->cbResultCode, pModifyTable->szErrorMessage);
			}
		}

		return true;
	}
	case SUB_BS_S_TABLE_SYNCHRO:	//桌子同步
	{
		//参数校验
		ASSERT (wDataSize % sizeof(tagBattleTableItemEx) == 0);
		if (wDataSize % sizeof(tagBattleTableItemEx) != 0) return false;

		//变量定义
		WORD wTableCount = wDataSize / sizeof(tagBattleTableItemEx);
		auto pTableItemEx = (tagBattleTableItemEx *)pData;

		//添加桌子
		for (WORD i = 0; i < wTableCount; i++)
		{
			m_BattleManager.AddTableItem(*pTableItemEx++);
		}

		return true;
	}
	case SUB_BS_S_SYNCHRO_FINISH:	//同步完成
	{
		//绑定信息
		WORD wBindIndex = LOWORD(dwSocketID);
		auto pBindParameter = (m_pBindParameter + wBindIndex);

		//移除子项
		RemoveSynchroItem(pBindParameter->wServiceID);

		//同步完成
		if (m_SynchroItemArray.GetCount() == 0)
		{
			//数组定义
			CTableItemArray TableItemArray;
			CTableParamArray TableParamArray;

			//变量定义
			DWORD dwClubID = 0;
			POSITION Position=NULL;						
			
			do
			{
				dwClubID=m_BattleManager.EnumTableClubID(Position);
				if (dwClubID == 0) break;

				//移除元素
				TableItemArray.RemoveAll();
				TableParamArray.RemoveAll();

				//获取桌子				
				m_BattleManager.GetClubTableList(dwClubID, TableItemArray, TableParamArray);

				if (TableItemArray.GetCount() > 0)
				{
					//变量定义
					INT nStartIndex = 0;
					auto pTableItemEx = TableItemArray[0];

					//发送列表
					while (nStartIndex < TableItemArray.GetCount())
					{
						auto wPacketSize = ConstructTableList(TableItemArray, TableParamArray, nStartIndex, m_cbBuffer, CountArray(m_cbBuffer));
						if (wPacketSize > 0)
						{
							//发送数据
							SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_TABLE_ITEM, m_cbBuffer, wPacketSize);
							wPacketSize = 0;
						}
					}
				}

			} while (Position!=NULL);

		}

		return true;
	}
	case SUB_BS_S_TABLE_USERLIST:	//桌子用户
	{
		//参数校验
		ASSERT(sizeof(CMD_BS_S_TableUserList) >= wDataSize);
		if (sizeof(CMD_BS_S_TableUserList) < wDataSize) return false;

		//提取数据
		auto pTableUserList = (CMD_BS_S_TableUserList *)pData;

		//查找桌子
		auto pTableParamEx = m_BattleManager.GetTableParam(pTableUserList->dwBattleNum);
		if (pTableParamEx != NULL)
		{
			//设置变量								
			pTableParamEx->wUserCount = pTableUserList->wUserCount;
			CopyMemory(pTableParamEx->TableUserItem, pTableUserList->TableUserItem, sizeof(pTableParamEx->TableUserItem));
		}

		return true;
	}
	case SUB_BS_S_TABLE_PARAM:		//桌子参数
	{
		//参数校验
		ASSERT(sizeof(CMD_BS_S_TableParam) == wDataSize);
		if (sizeof(CMD_BS_S_TableParam) != wDataSize) return false;

		//提取数据
		auto pTableParam = (CMD_BS_S_TableParam *)pData;

		//查找桌子
		auto pTableParamEx = m_BattleManager.GetTableParam(pTableParam->dwBattleNum);
		if (pTableParamEx != NULL)
		{
			//设置变量
			pTableParamEx->dwStartTime = pTableParam->dwStartTime;
			pTableParamEx->wFinishCount = pTableParam->wFinishCount;
			pTableParamEx->cbGameStatus = pTableParam->cbGameStatus;
		}

		auto pTableItemEx = m_BattleManager.GetTableItem(pTableParam->dwBattleNum);
		if (pTableItemEx != NULL && pTableItemEx->dwClubID != 0)
		{
			//构造对象
			CMD_BC_S_TableParam TableParam = {};

			//设置变量								
			TableParam.dwClubID = pTableItemEx->dwClubID;
			TableParam.dwBattleNum = pTableItemEx->dwBattleNum;
			TableParam.dwStartTime = pTableParam->dwStartTime;
			TableParam.wFinishCount = pTableParam->wFinishCount;
			TableParam.cbGameStatus = pTableParam->cbGameStatus;

			//发送消息				
			SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_TABLE_PARAM, &TableParam, sizeof(TableParam));
		}

		return true;
	}	
	case SUB_BS_S_UPDATE_CONFIG:	//更新配置
	{
		//参数校验
		ASSERT(sizeof(CMD_BS_S_UpdateConfig) == wDataSize);
		if (sizeof(CMD_BS_S_UpdateConfig) != wDataSize) return false;

		//提取数据
		auto pUpdateConfig = (CMD_BS_S_UpdateConfig *)pData;

		//广播消息
		auto pTableItemEx = m_BattleManager.GetTableItem(pUpdateConfig->dwBattleNum);
		if (pTableItemEx != NULL)
		{
			//更新配置
			pTableItemEx->BattleConfig.dwPayUserID = pUpdateConfig->dwPayUserID;
		}

		return true;
	}
	case SUB_BS_S_BATTLE_FINISH:	//约战完成
	{
		//参数校验
		ASSERT(sizeof(CMD_BS_S_BattleFinish) == wDataSize);
		if (sizeof(CMD_BS_S_BattleFinish) != wDataSize) return false;

		//提取数据
		auto pBattleFinish = (CMD_BS_S_BattleFinish*)pData;

		//查找桌子
		auto pTableItemEx = m_BattleManager.GetTableItem(pBattleFinish->dwBattleNum);
		if (pTableItemEx != NULL)
		{
			//查找桌子				
			auto pTableParamEx = m_BattleManager.GetTableParam(pBattleFinish->dwBattleNum);			
			if (pTableParamEx != NULL)
			{
				//广播消息
				if (pTableItemEx->dwClubID != 0)
				{
					//校验局数
					if (pTableParamEx && pTableParamEx->wFinishCount > 0)
					{
						//构造对象
						auto pIMBattleFinish = (CMD_BC_S_BattleFinish*)m_cbBuffer;

						//设置变量
						pIMBattleFinish->wUserCount = 0;
						pIMBattleFinish->dwClubID = pTableItemEx->dwClubID;
						pIMBattleFinish->dwBattleNum = pTableItemEx->dwBattleNum;

						//查找桌子							
						DWORD* pdwUserID = (DWORD*)(pIMBattleFinish + 1);
						for (int i = 0; i < CountArray(pTableParamEx->TableUserItem); i++)
						{
							if (pTableParamEx->TableUserItem[i].dwUserID == 0) continue;
							*pdwUserID++ = pTableParamEx->TableUserItem[i].dwUserID;
							if (++pIMBattleFinish->wUserCount == pTableParamEx->wUserCount) break;
						}

						//广播消息					
						WORD wPacketSize = sizeof(CMD_BC_S_BattleFinish) + sizeof(DWORD) * pIMBattleFinish->wUserCount;
						SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_BATTLE_FINISH, m_cbBuffer, wPacketSize);
					}
				}

				//设置状态
				if (pTableParamEx != NULL)
				{
					//设置变量							
					pTableParamEx->dwStartTime = 0;
					pTableParamEx->wFinishCount = 0;
				}				
			}
		}

		return true;
	}
	case SUB_BS_S_TABLE_DISMISS:	//桌子解散
	{
		//参数校验
		ASSERT (sizeof(CMD_BS_S_TableDismiss) == wDataSize);
		if (sizeof(CMD_BS_S_TableDismiss) != wDataSize) return false;

		//提取数据
		auto pTableDismiss = (CMD_BS_S_TableDismiss *)pData;

		//查找桌子
		auto pTableItemEx = m_BattleManager.GetTableItem(pTableDismiss->dwBattleNum);
		if (pTableItemEx != NULL)
		{
			//校验俱乐部
			if (pTableItemEx->dwClubID != 0)
			{
				//构造对象
				auto pIMTableDismiss = (CMD_BC_S_TableDismiss*)m_cbBuffer;

				//设置变量								
				pIMTableDismiss->wUserCount = 0;
				pIMTableDismiss->dwClubID = pTableItemEx->dwClubID;
				pIMTableDismiss->dwBattleNum = pTableItemEx->dwBattleNum;

				//桌子状态
				auto pTableParamEx = m_BattleManager.GetTableParam(pTableDismiss->dwBattleNum);
				if (pTableParamEx != NULL && pTableParamEx->wFinishCount >= 1)
				{
					DWORD* pdwUserID = (DWORD*)(pIMTableDismiss + 1);
					for (int i = 0; i < CountArray(pTableParamEx->TableUserItem); i++)
					{
						if (pTableParamEx->TableUserItem[i].dwUserID == 0) continue;
						*pdwUserID++ = pTableParamEx->TableUserItem[i].dwUserID;
						if (++pIMTableDismiss->wUserCount == pTableParamEx->wUserCount) break;
					}
				}

				//发送消息					
				WORD wPacketSize = sizeof(CMD_BC_S_TableDismiss) + sizeof(DWORD) * pIMTableDismiss->wUserCount;
				SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_TABLE_DISMISS, m_cbBuffer, wPacketSize);
			}

			//判断桌子
			if (pTableDismiss->wServerID == pTableItemEx->wServerID)
			{
				//移除桌子
				m_BattleManager.RemoveTable(pTableDismiss->dwBattleNum);
				m_BattleManager.RemoveTableOwner(pTableDismiss->dwBattleNum);
			}
		}			
		return true;
	}
	case SUB_BS_S_DISMISS_TABLE:	//解散结果
	{
		//参数校验
		ASSERT (sizeof(CMD_BS_S_DismissTable) >= wDataSize);
		if (sizeof(CMD_BS_S_DismissTable) < wDataSize) return false;

		//提取数据
		auto pDismissTable = (CMD_BS_S_DismissTable *)pData;

		//网络信息
		auto pSocketInfo = &pDismissTable->SocketInfo;

		//解散失败
		if (pDismissTable->cbResultCode != 0)
		{
			//操作失败
			SendOperateFailure(pSocketInfo->wGateID, pSocketInfo->dwTokenID, SUB_GB_C_DISMISS_TABLE, pDismissTable->cbResultCode, pDismissTable->szDescribeString);
		}
		else
		{
			//构造结构
			CMD_GB_S_TableDismiss TableDismiss = {};
			
			//设置变量				
			TableDismiss.dwBattleNum = pDismissTable->dwBattleNum;

			//发送数据
			SendDataToGate(pSocketInfo->wGateID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_DISMISS, &TableDismiss, sizeof(TableDismiss), &pSocketInfo->dwTokenID, 1);
		}		

		return true;
	}
	case SUB_BS_S_USER_SITDOWN:		//用户坐下
	{
		//参数校验
		ASSERT(sizeof(CMD_BS_S_UserSitdown) >= wDataSize);
		if (sizeof(CMD_BS_S_UserSitdown) < wDataSize) return false;

		//提取数据
		auto pUserSitDown = (CMD_BS_S_UserSitdown *)pData;

		//广播消息
		auto pTableItemEx = m_BattleManager.GetTableItem(pUserSitDown->dwBattleNum);
		if (pTableItemEx != NULL && pTableItemEx->dwClubID != 0)
		{
			//构造对象
			CMD_BC_S_UserSitdown UserSitdown = {};

			//设置变量								
			UserSitdown.dwClubID = pTableItemEx->dwClubID;
			UserSitdown.dwBattleNum = pTableItemEx->dwBattleNum;
			UserSitdown.dwUserID = pUserSitDown->TableUserItem.dwUserID;
			CopyMemory(&UserSitdown.TableUserItem, &pUserSitDown->TableUserItem, sizeof(UserSitdown.TableUserItem));

			//发送消息					
			SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_USER_SITDOWN, &UserSitdown, sizeof(UserSitdown));
		}

		//查找桌子
		auto pTableParamEx = m_BattleManager.GetTableParam(pUserSitDown->dwBattleNum);
		if (pTableParamEx != NULL)
		{
			//拷贝信息							
			pTableParamEx->wUserCount++;
			WORD wChairID = pUserSitDown->TableUserItem.wChairID;
			if (wChairID < CountArray(pTableParamEx->TableUserItem))
			{
				CopyMemory(&pTableParamEx->TableUserItem[wChairID], &pUserSitDown->TableUserItem, sizeof(pUserSitDown->TableUserItem));
			}
		}

		return true;
	}
	case SUB_BS_S_USER_STANDUP:		//用户起立
	{
		//参数校验
		ASSERT(sizeof(CMD_BS_S_UserStandup) >= wDataSize);
		if (sizeof(CMD_BS_S_UserStandup) < wDataSize) return false;

		//提取数据
		auto pUserStandup = (CMD_BS_S_UserStandup *)pData;

		//广播消息
		auto pTableItemEx = m_BattleManager.GetTableItem(pUserStandup->dwBattleNum);
		if (pTableItemEx != NULL && pTableItemEx->dwClubID != 0)
		{
			//构造对象
			CMD_BC_S_UserStandup UserStandup = {};

			//设置变量								
			UserStandup.dwClubID = pTableItemEx->dwClubID;
			UserStandup.dwUserID = pUserStandup->dwUserID;
			UserStandup.wChairID = pUserStandup->wChairID;
			UserStandup.dwBattleNum = pTableItemEx->dwBattleNum;

			//发送消息					
			SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_USER_STANDUP, &UserStandup, sizeof(UserStandup));
		}

		//桌子状态
		auto pTableParamEx = m_BattleManager.GetTableParam(pUserStandup->dwBattleNum);
		if (pTableParamEx != NULL)
		{
			//更新人数
			if (pUserStandup->dwUserID != 0)
			{
				pTableParamEx->wUserCount--;
				if (pUserStandup->wChairID < CountArray(pTableParamEx->TableUserItem))
				{
					ZeroMemory(&pTableParamEx->TableUserItem[pUserStandup->wChairID], sizeof(pTableParamEx->TableUserItem[0]));
				}
			}
			else
			{
				pTableParamEx->wUserCount = 0;
				ZeroMemory(&pTableParamEx->TableUserItem, sizeof(pTableParamEx->TableUserItem));
			}
		}

		return true;
	}
	case SUB_BS_S_CLUB_SCORE:		//茶舍分数
	{
		//参数校验
		ASSERT(sizeof(CMD_BS_S_ClubScore) == wDataSize);
		if (sizeof(CMD_BS_S_ClubScore) != wDataSize) return false;

		//提取数据
		auto pClubScore = (CMD_BS_S_ClubScore*)pData;

		//广播消息
		auto pTableItemEx = m_BattleManager.GetTableItem(pClubScore->dwBattleNum);
		if (pTableItemEx != NULL)
		{
			//构造对象
			CMD_BC_S_ClubScore ClubScore = {};

			//设置变量
			ClubScore.dwUserID = pClubScore->dwUserID;
			ClubScore.lRevenue = pClubScore->lRevenue;
			ClubScore.dwClubID = pClubScore->dwClubID;
			ClubScore.lClubScore = pClubScore->lClubScore;

			//发送消息					
			SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_CLUB_SCORE, &ClubScore, sizeof(ClubScore));
		}		

		return true;
	}
	case SUB_BS_S_CLUB_WEALTH:		//茶舍财富
	{
		//参数校验
		ASSERT(sizeof(CMD_BS_S_ClubWealth) == wDataSize);
		if (sizeof(CMD_BS_S_ClubWealth) != wDataSize) return false;

		//提取数据
		auto pClubWealth = (CMD_BS_S_ClubWealth*)pData;

		//广播消息
		auto pTableItemEx = m_BattleManager.GetTableItem(pClubWealth->dwBattleNum);
		if (pTableItemEx != NULL)
		{
			//构造对象
			CMD_BC_S_ClubWealth ClubWealth = {};

			//设置变量
			ClubWealth.dwClubID = pClubWealth->dwClubID;
			ClubWealth.dwPayUserID = pClubWealth->dwPayUserID;			
			ClubWealth.cbWealthMask = WEALTH_MASK_CARD;
			ClubWealth.lUserCard = pClubWealth->lUserCard;

			//发送消息					
			SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_CLUB_WEALTH, &ClubWealth, sizeof(ClubWealth));
		}

		return true;
	}
	}

	return false;
}

//约战服务
bool CAttemperEngineSink::OnTCPNetWorkMainBCBattleService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//获取信息
	WORD wBindIndex = LOWORD(dwSocketID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);

	switch (wSubCmdID)
	{
	case SUB_BC_C_QUERY_OPTION:		//查询配置
	{
		//参数校验
		ASSERT(wDataSize == sizeof(CMD_BC_C_QueryOption));
		if (wDataSize != sizeof(CMD_BC_C_QueryOption)) return true;

		//提取数据
		auto pQueryOption = (CMD_BC_C_QueryOption*)pData;

		//变量定义
		INT nStartIndex = 0;
		WORD wHeadSize = sizeof(pQueryOption->SocketInfo);

		//拷贝网络信息
		CopyMemory(m_cbBuffer,&pQueryOption->SocketInfo,sizeof(pQueryOption->SocketInfo));

		//基础配置
		while (nStartIndex < m_BattleBaseOptionActive.GetCount())
		{			
			auto wPacketSize = ConstructBaseOption(nStartIndex, pQueryOption->wKindID, pQueryOption->cbEmployScene, &m_cbBuffer[wHeadSize], CountArray(m_cbBuffer)-wHeadSize);
			if (wPacketSize > 0)
			{
				//发送数据
				SendDataToClub(pBindParameter->wServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_BATTLE_BASE_OPTION, m_cbBuffer, wPacketSize+ wHeadSize);
			}
		}

		nStartIndex = 0;

		//游戏配置
		while (nStartIndex < m_BattleGameOptionActive.GetCount())
		{
			auto wPacketSize = ConstructGameOption(nStartIndex, pQueryOption->wKindID, pQueryOption->cbEmployScene, &m_cbBuffer[wHeadSize],CountArray(m_cbBuffer)-wHeadSize);
			if (wPacketSize > 0)
			{
				//发送数据
				SendDataToClub(pBindParameter->wServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_BATTLE_GAME_OPTION, m_cbBuffer, wPacketSize+wHeadSize);
			}
		}

		//发送数据
		SendDataToClub(pBindParameter->wServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_BATTLE_OPTION_FINISH, m_cbBuffer, wHeadSize);

		return true;
	}
	case SUB_BC_C_MODIFY_SCORE:		//修改分数
	{
		//提取数据
		ASSERT(wDataSize == sizeof(CMD_BC_C_ModifyScore));
		if (wDataSize != sizeof(CMD_BC_C_ModifyScore)) return false;

		//提取数据
		auto pModifyScore = (CMD_BC_C_ModifyScore*)pData;

		//构造结构
		CMD_BS_C_ModifyClubScore ModifyClubScore;
		ZeroMemory(&ModifyClubScore,sizeof(ModifyClubScore));

		//设置变量
		ModifyClubScore.dwUserID = pModifyScore->dwUserID;
		ModifyClubScore.dwClubID = pModifyScore->dwClubID;
		ModifyClubScore.cbChangeType = pModifyScore->cbChangeType;
		ModifyClubScore.lChangeScore = pModifyScore->lChangeScore;
		StringCchCopy(ModifyClubScore.szClubName, CountArray(ModifyClubScore.szClubName), pModifyScore->szClubName);
		StringCchCopy(ModifyClubScore.szOperaterNickName, CountArray(ModifyClubScore.szOperaterNickName), pModifyScore->szOperaterNickName);

		//发送数据
		SendDataToGame(pModifyScore->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_MODIFY_CLUB_SCORE, &ModifyClubScore, sizeof(ModifyClubScore));

		return true;
	}	
	case SUB_BC_C_CLUB_DELETE:		//茶社删除
	{
		//提取数据
		ASSERT (wDataSize==sizeof(CMD_BC_C_ClubDelete));
		if (wDataSize!=sizeof(CMD_BC_C_ClubDelete)) return false;

		//提取数据
		auto pDeleteClub=(CMD_BC_C_ClubDelete*)pData;

		//查询桌子
		CTableItemArray TableItemArray;				
		CTableParamArray TableParamArray;	

		//获取桌子				
		m_BattleManager.GetClubTableList(pDeleteClub->dwClubID,TableItemArray,TableParamArray);	

		//构造结构
		CMD_BS_C_DismissTable DismissTable;
		ZeroMemory(&DismissTable,sizeof(DismissTable));

		for (INT_PTR i=0; i<TableItemArray.GetCount(); i++)
		{
			//获取对象
			tagBattleTableItemEx * pTableItemEx=TableItemArray[i];

			//设置变量
			DismissTable.wTableID=pTableItemEx->wTableID;
			DismissTable.dwBattleNum=pTableItemEx->dwBattleNum;
			DismissTable.dwUserID=pTableItemEx->dwClubCreaterID;

			//发送命令				
			SendDataToGame(pTableItemEx->wServerID,MDM_BS_BATTLE_SERVICE,SUB_BS_C_DISMISS_TABLE,&DismissTable,sizeof(DismissTable));
		}

		return true;
	}
	case SUB_BC_C_CLUB_UPDATE:		//茶社更新
	{
		//提取数据
		ASSERT(wDataSize >= sizeof(CMD_BC_C_ClubUpdate));
		if (wDataSize < sizeof(CMD_BC_C_ClubUpdate)) return false;

		//提取数据
		auto pUpdateClub = (CMD_BC_C_ClubUpdate*)pData;

		//更新规则
		if (pUpdateClub->cbUpdateKind == UPCLUB_KIND_RULES)
		{
			//拷贝数据
			auto pIMClubRules=(tagClubRules *)(pUpdateClub+1);

			//查询桌子
			CTableItemArray TableItemArray;
			CTableParamArray TableParamArray;

			//获取桌子				
			m_BattleManager.GetClubTableList(pUpdateClub->dwClubID, TableItemArray, TableParamArray);

			//构造结构
			CMD_BS_C_UpdateClubInfo UpdateClubInfo;
			ZeroMemory(&UpdateClubInfo, sizeof(UpdateClubInfo));

			for (INT_PTR i = 0; i < TableItemArray.GetCount(); i++)
			{
				//获取对象
				tagBattleTableItemEx * pTableItemEx = TableItemArray[i];

				//修改馆主
				pTableItemEx->wSwitchOption = pIMClubRules->wSwitchOption;

				//桌子信息
				UpdateClubInfo.wTableID = pTableItemEx->wTableID;
				UpdateClubInfo.dwClubID = pUpdateClub->dwClubID;
				UpdateClubInfo.dwBattleNum = pTableItemEx->dwBattleNum;

				//变量信息
				UpdateClubInfo.wSwitchOption = pTableItemEx->wSwitchOption;
				UpdateClubInfo.dwClubCreaterID = pTableItemEx->dwClubCreaterID;

				//发送命令				
				SendDataToGame(pTableItemEx->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_UPDATE_CLUB_INFO, &UpdateClubInfo, sizeof(UpdateClubInfo));
			}
		}

		return true;
	}
	case SUB_BC_C_CLUB_TRANSFER:	//茶社转让
	{
		//提取数据
		ASSERT (wDataSize == sizeof(CMD_BC_C_ClubTransfer));
		if (wDataSize != sizeof(CMD_BC_C_ClubTransfer)) return false;

		//提取数据
		auto pTransferClub = (CMD_BC_C_ClubTransfer*)pData;

		//查询桌子
		CTableItemArray TableItemArray;
		CTableParamArray TableParamArray;

		//获取桌子				
		m_BattleManager.GetClubTableList(pTransferClub->dwClubID, TableItemArray, TableParamArray);

		//构造结构
		CMD_BS_C_UpdateClubInfo UpdateClubInfo;
		ZeroMemory(&UpdateClubInfo, sizeof(UpdateClubInfo));

		for (INT_PTR i = 0; i < TableItemArray.GetCount(); i++)
		{
			//获取对象
			tagBattleTableItemEx * pTableItemEx = TableItemArray[i];
				
			//修改馆主
			pTableItemEx->dwClubCreaterID = pTransferClub->dwNewCreaterID;

			//桌子信息
			UpdateClubInfo.wTableID = pTableItemEx->wTableID;
			UpdateClubInfo.dwClubID = pTransferClub->dwClubID;
			UpdateClubInfo.dwBattleNum = pTableItemEx->dwBattleNum;

			//变量信息
			UpdateClubInfo.wSwitchOption = pTableItemEx->wSwitchOption;
			UpdateClubInfo.dwClubCreaterID = pTransferClub->dwNewCreaterID;

			//发送命令				
			SendDataToGame(pTableItemEx->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_UPDATE_CLUB_INFO, &UpdateClubInfo, sizeof(UpdateClubInfo));
		}

		return true;
	}	
	case SUB_BC_C_EFFICACY_RESULT:	//验证结果
	{
		//参数校验
		ASSERT (wDataSize==sizeof(CMD_BC_C_EfficacyResult));
		if (wDataSize!=sizeof(CMD_BC_C_EfficacyResult)) return true;

		//提取数据
		auto pEfficacyResult=(CMD_BC_C_EfficacyResult*)pData;

		//网络信息
		auto pSocketInfo=&pEfficacyResult->UserSocketInfo;

		//查询桌子
		auto pTableItemEx=m_BattleManager.GetTableItem(pEfficacyResult->dwBattleNum);
		if (pTableItemEx==NULL || pTableItemEx->wTableID==INVALID_TABLE)
		{
			//操作失败
			SendOperateFailure(pSocketInfo->wGateID, pSocketInfo->dwTokenID, SUB_GB_C_QUERY_TABLE, BATTLE_CODE_QUERY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0001, TEXT("抱歉，系统没有查询到您进入的桌子，请确认桌子号再次尝试！")));

			return true;
		}

		//验证失败
		if (pEfficacyResult->bResult==false)
		{
			//操作失败
			SendOperateFailure(pSocketInfo->wGateID, pSocketInfo->dwTokenID, SUB_GB_C_QUERY_TABLE, BATTLE_CODE_QUERY_FAILED, pEfficacyResult->szErrorMessage);

			return true;
		}

		//查询参数
		auto pTableParam = m_BattleManager.GetTableParam(pEfficacyResult->dwBattleNum);
		if (pTableParam == NULL )
		{
			//操作失败
			SendOperateFailure(pSocketInfo->wGateID, pSocketInfo->dwTokenID, SUB_GB_C_QUERY_TABLE, BATTLE_CODE_QUERY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0001, TEXT("抱歉，系统没有查询到您进入的桌子，请确认桌子号再次尝试！")));

			return true;
		}

		//变量定义
		auto pBattleTableItem = (tagBattleTableItem*)m_cbBuffer;
		auto pBattleTableParam = (tagBattleTableParam*)(pBattleTableItem + 1);

		//构造桌子
		ConstructTableItem(pTableItemEx, pBattleTableItem);

		//桌子参数
		pBattleTableParam->wUserCount = 0;
		pBattleTableParam->dwStartTime = pTableParam->dwStartTime;		
		pBattleTableParam->wFinishCount = pTableParam->wFinishCount;
		pBattleTableParam->cbGameStatus = pTableParam->cbGameStatus;
		pBattleTableParam->dwBattleNum = pBattleTableItem->dwBattleNum;

		//发送消息
		SendDataToGate(pSocketInfo->wGateID,MDM_GB_BATTLE_SERVICE, SUB_GB_S_SEARCH_RESULT,m_cbBuffer,sizeof(tagBattleTableItem)+sizeof(tagBattleTableParam),&pSocketInfo->dwTokenID,1);

		return true;
	}	
	case SUB_BC_C_QUERY_TABLE:		//查询桌子
	{
		//参数校验
		ASSERT(sizeof(CMD_BC_C_QueryTable) == wDataSize);
		if (sizeof(CMD_BC_C_QueryTable) != wDataSize) return false;

		//提取数据
		auto pQueryTable = (CMD_BC_C_QueryTable*)pData;

		//房间同步中
		if (m_SynchroItemArray.GetCount() > 0) return true;

		//数值定义
		CTableItemArray TableItemArray;
		CTableParamArray TableParamArray;

		//获取桌子				
		m_BattleManager.GetClubTableList(pQueryTable->dwClubID, TableItemArray, TableParamArray);

		//变量定义
		INT nStartIndex = 0;

		//发送列表
		while (nStartIndex < TableItemArray.GetCount())
		{
			auto wPacketSize = ConstructTableList(TableItemArray, TableParamArray, nStartIndex, m_cbBuffer, CountArray(m_cbBuffer));
			if (wPacketSize > 0)
			{
				//发送数据
				SendDataToClub(pBindParameter->wServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_TABLE_ITEM, m_cbBuffer, wPacketSize);
				wPacketSize = 0;
			}
		}
		
		return true;
	}
	case SUB_BC_C_CREATE_TABLE:		//创建桌子
	{
		//参数校验
		ASSERT (wDataSize == sizeof(CMD_BC_C_CreateTable));
		if (wDataSize != sizeof(CMD_BC_C_CreateTable)) return true;

		//提取数据
		auto pCreateTable = (CMD_BC_C_CreateTable*)pData;

		//网络信息
		auto pSocketInfo = &pCreateTable->SocketInfo;

		//服务状态
		if (m_bBattleService == false)
		{
			SendOperateFailure(pBindParameter->wServiceID, pCreateTable->dwClubID, *pSocketInfo, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_BATTLE_0002, TEXT("系统维护中，约战桌子暂时停止服务。请在维护完成后重试！")));

			return true;
		}

		//变量定义
		tagBattleConfigEx BattleConfig;
		ZeroMemory(&BattleConfig, sizeof(BattleConfig));

		//获取配置
		auto pBattleConfig = &pCreateTable->BattleConfig;

		//校验配置
		if (VerifyBattleConfig(pBattleConfig, BattleConfig) == false)
		{
			SendOperateFailure(pBindParameter->wServiceID, pCreateTable->dwClubID, *pSocketInfo, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_BATTLE_0003, TEXT("桌子创建失败！")));

			return true;
		}			

		//判断桌数
		auto wTableCount = m_BattleManager.GetTableCountByClubID(pCreateTable->dwClubID);
		if (VerifyTableCount(pCreateTable->dwClubID, wTableCount+1)==false)
		{
			SendOperateFailure(pBindParameter->wServiceID, pCreateTable->dwClubID, *pSocketInfo, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_BATTLE_0004, TEXT("抱歉，您创建的桌子数目已经达到上限，如有疑问，请联系客服！")));

			return true;
		}

		//创建桌子
		auto pTableItemEx = m_BattleManager.CreateTable(pCreateTable->dwUserID, pCreateTable->dwClubID, BattleConfig);
		if (pTableItemEx == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, pCreateTable->dwClubID, *pSocketInfo, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessageV(MIM_BATTLE_0005, TEXT("抱歉，桌子创建失败[%d]，请稍后再次尝试！"), 2));

			return true;
		}

		//保存桌主
		auto pTableOwner = m_BattleManager.CreateTableOwner(pTableItemEx->dwBattleNum);
		if (pTableOwner == NULL)
		{
			//移除桌子
			m_BattleManager.RemoveTable(pTableItemEx->dwBattleNum);

			SendOperateFailure(pBindParameter->wServiceID, pCreateTable->dwClubID, *pSocketInfo, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessageV(MIM_BATTLE_0005, TEXT("抱歉，桌子创建失败[%d]，请稍后再次尝试！"), 3));

			return true;
		}

		//茶社信息			
		pTableItemEx->wClubServiceID = pBindParameter->wServiceID;
		pTableItemEx->wSwitchOption = pCreateTable->wSwitchOption;
		pTableItemEx->dwClubCreaterID = pCreateTable->dwClubCreaterID;		

		//玩法信息
		pTableItemEx->dwConfigID = pCreateTable->dwConfigID;

		//基本信息			
		pTableOwner->dwUserID = pCreateTable->dwUserID;
			
		//网络信息
		CopyMemory(&pTableOwner->SocketInfo, pSocketInfo, sizeof(pTableOwner->SocketInfo));

		//构造结构
		CMD_BS_C_CreateTable CreateTable;
		ZeroMemory(&CreateTable, sizeof(CreateTable));

		//用户信息			
		CreateTable.dwUserID = pCreateTable->dwUserID;
		StringCchCopy(CreateTable.szPassword, CountArray(CreateTable.szPassword), pTableItemEx->szPassword);

		//茶社信息			
		CreateTable.dwClubID = pCreateTable->dwClubID;
		CreateTable.wSwitchOption = pCreateTable->wSwitchOption;
		CreateTable.wClubServiceID = pBindParameter->wServiceID;
		CreateTable.dwClubCreaterID = pCreateTable->dwClubCreaterID;			

		//创建信息
		CreateTable.dwConfigID = pTableItemEx->dwConfigID;
		CreateTable.dwBattleNum = pTableItemEx->dwBattleNum;
		CopyMemory(&CreateTable.BattleConfig, &BattleConfig, sizeof(CreateTable.BattleConfig));

		//发送消息
		SendDataToGame(pTableItemEx->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_CREATE_TABLE, &CreateTable, sizeof(CreateTable));

		return true;
	}
	case SUB_BC_C_MODIFY_TABLE:		//修改桌子
	{
		//参数校验
		ASSERT (sizeof(CMD_BC_C_ModifyTable) >= wDataSize);
		if (sizeof(CMD_BC_C_ModifyTable) < wDataSize) return false;

		//提取数据
		auto pModifyTable = (CMD_BC_C_ModifyTable*)pData;

		//网络信息
		auto pSocketInfo = &pModifyTable->SocketInfo;

		//服务状态
		if (m_bBattleService == false)
		{
			SendOperateFailure(pBindParameter->wServiceID, pModifyTable->dwClubID, *pSocketInfo, BATTLE_CODE_STOP_SERVICE, CWHService::FormatMessage(MIM_BATTLE_0002, TEXT("系统维护中，约战桌子暂时停止服务。请在维护完成后重试！")));

			return true;
		}

		//查找桌子
		auto pTableItemEx = m_BattleManager.GetTableItem(pModifyTable->dwBattleNum);
		if (pTableItemEx == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, pModifyTable->dwClubID, *pSocketInfo, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0009, TEXT("抱歉，系统未查询到您要修改的桌子，请刷新后再次尝试！")));

			return true;
		}

		//校验用户			
		if (pTableItemEx->dwClubCreaterID != pModifyTable->dwUserID)
		{
			SendOperateFailure(pBindParameter->wServiceID, pModifyTable->dwClubID, *pSocketInfo, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0010, TEXT("抱歉，您不是该茶社的老板，没有权限修改桌子配置！")));

			return true;
		}

		//变量定义
		tagBattleConfigEx BattleConfig;
		ZeroMemory(&BattleConfig, sizeof(BattleConfig));

		//获取配置
		auto pBattleConfig = &pModifyTable->BattleConfig;

		//校验配置
		if (VerifyBattleConfig(pBattleConfig, BattleConfig) == false)
		{
			SendOperateFailure(pBindParameter->wServiceID, pModifyTable->dwClubID, *pSocketInfo, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0011, TEXT("桌子修改失败！")));

			return true;
		}

		//构造结构
		CMD_BS_C_ModifyTable ModifyTable;
		ZeroMemory(&ModifyTable, sizeof(ModifyTable));

		//用户信息			
		ModifyTable.wTableID = pTableItemEx->wTableID;
		ModifyTable.dwUserID = pModifyTable->dwUserID;
		ModifyTable.dwBattleNum = pModifyTable->dwBattleNum;

		//约战配置
		CopyMemory(&ModifyTable.BattleConfig, &BattleConfig, sizeof(BattleConfig));

		//网络信息
		CopyMemory(&ModifyTable.SocketInfo, pSocketInfo,sizeof(ModifyTable.SocketInfo));

		//发送消息
		SendDataToGame(pTableItemEx->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_MODIFY_TABLE, &ModifyTable, sizeof(ModifyTable));

		return true;
	}
	case SUB_BC_C_DISMISS_TABLE:	//解散桌子
	{
		//参数校验
		ASSERT (sizeof(CMD_BC_C_DismissTable) == wDataSize);
		if (sizeof(CMD_BC_C_DismissTable) != wDataSize) return false;

		//提取数据
		auto pDismissTable = (CMD_BC_C_DismissTable*)pData;

		//查找桌子
		auto pTableItemEx = m_BattleManager.GetTableItem(pDismissTable->dwBattleNum);
		if (pTableItemEx == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, 0, pDismissTable->SocketInfo, BATTLE_CODE_DISMISS_FAILED, CWHService::FormatMessage(MIM_BATTLE_0012, TEXT("抱歉，系统未查询到您要解散的桌子，请刷新后再次尝试！")));

			return true;
		}

		//构造结构
		CMD_BS_C_DismissTable DismissTable;
		ZeroMemory(&DismissTable, sizeof(DismissTable));

		//网络信息
		CopyMemory(&DismissTable.SocketInfo, &pDismissTable->SocketInfo,sizeof(DismissTable.SocketInfo));

		//桌子信息
		DismissTable.wTableID = pTableItemEx->wTableID;
		DismissTable.dwUserID = pDismissTable->dwUserID;
		DismissTable.dwBattleNum = pDismissTable->dwBattleNum;		

		//发送命令
		SendDataToGame(pTableItemEx->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_DISMISS_TABLE, &DismissTable, sizeof(DismissTable));

		return true;
	}		
	}

	return true;
}

//操作失败
bool CAttemperEngineSink::OnDBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT(wDataSize <= sizeof(DBO_GR_OperateFailure));
	if (wDataSize > sizeof(DBO_GR_OperateFailure)) return false;

	//提取数据
	auto pOperateFailure = (DBO_GR_OperateFailure *)pData;

	//获取参数
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);

	//操作失败
	SendOperateFailure(pBindParameter->wServiceID, dwTokenID, 0, pOperateFailure->lErrorCode, pOperateFailure->szErrorMessage);

	return true;
}

//开桌选项
bool CAttemperEngineSink::OnDBBattleBaseOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//提取数据
	ASSERT (wDataSize%sizeof(tagBattleBaseOptionEx)==0);
	if (wDataSize%sizeof(tagBattleBaseOptionEx)!=0) return false;

	//变量定义
	WORD wOptionCount=wDataSize/sizeof(tagBattleBaseOptionEx);
	auto pBattleBaseOption =(tagBattleBaseOptionEx *)pData;

	//变量定义
	for (int i=0; i<wOptionCount; i++)
	{
		m_BattleBaseOptionBuffer.Add(*pBattleBaseOption++);
	}	

	return true;
}

//全局选项
bool CAttemperEngineSink::OnDBBattleGlobalOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//提取数据
	ASSERT (wDataSize == sizeof(tagBattleGlobalOption));
	if (wDataSize != sizeof(tagBattleGlobalOption)) return false;

	//变量定义
	auto pGlobalOption = (tagBattleGlobalOption *)pData;

	//拷贝配置
	CopyMemory(&m_GlobalOption, pGlobalOption, sizeof(m_GlobalOption));

	return true;
}

//自定选项
bool CAttemperEngineSink::OnDBBattleGameOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//提取数据
	ASSERT(wDataSize % sizeof(tagBattleGameOptionEx) == 0);
	if (wDataSize % sizeof(tagBattleGameOptionEx) != 0) return false;

	//变量定义
	WORD wOptionCount = wDataSize / sizeof(tagBattleGameOptionEx);
	auto pBattleGameOption = (tagBattleGameOptionEx *)pData;

	//变量定义
	for (int i = 0; i < wOptionCount; i++)
	{
		m_BattleGameOptionBuffer.Add(*pBattleGameOption++);
	}

	return true;
}

//选项完成
bool CAttemperEngineSink::OnDBBattleOptionFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//更新数据
	m_BattleBaseOptionActive.RemoveAll();
	m_BattleBaseOptionActive.Append(m_BattleBaseOptionBuffer);
	m_BattleBaseOptionBuffer.RemoveAll();

	//更新数据
	m_BattleGameOptionActive.RemoveAll();
	m_BattleGameOptionActive.Append(m_BattleGameOptionBuffer);
	m_BattleGameOptionBuffer.RemoveAll();

	//事件通知
	CP_ControlResult ControlResult;
	ControlResult.cbSuccess=ER_SUCCESS;
	SendUIControlPacket(UI_LOAD_OPTION_RESULT,&ControlResult,sizeof(ControlResult));
	
	return true;
}
//////////////////////////////////////////////////////////////////////////
