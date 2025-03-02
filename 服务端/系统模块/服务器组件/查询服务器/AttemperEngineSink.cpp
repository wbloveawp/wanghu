#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_CONNECT_CENTER			1									//重连标识


//////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//设置变量	
	m_pServiceOption=NULL;
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;		

	//组件变量
	m_pITimerEngine=NULL;
	m_pIDataCacheEngine=NULL;
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
	g_IdWorker->SetServiceID(m_pServiceOption->wServiceID);

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//设置变量
	m_pITimerEngine=NULL;
	m_pIDataCacheEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;

	//删除数据
	SafeDeleteArray(m_pBindParameter);

	//设置组件
	m_GlobalItemManager.ResetData();		

	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	switch (dwIdentifier)
	{
	case CT_CONNECT_CENTER:		//连接中心
	{
		//设置地址
		WORD wCenterPort = m_pInitParameter->m_wCenterPort;
		DWORD dwCenterServer=m_pInitParameter->m_CenterAddress.dwServiceAddr;

		//发起连接
		m_pITCPSocketCenter->Connect(htonl(dwCenterServer), wCenterPort);

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
	case IDI_CONNECT_CENTER:	//连接中心
		{
			//变量定义
			tagAddressInfo * pAddressInfo=&m_pInitParameter->m_CenterAddress;

			//设置地址
			WORD wCenterPort = m_pInitParameter->m_wCenterPort;
			DWORD dwCenterServer=pAddressInfo->dwServiceAddr; 

			//发起连接
			m_pITCPSocketCenter->Connect(htonl(dwCenterServer), wCenterPort);

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
	case MDM_AQ_QUERY_SERVICE:		//查询服务
		{
			bResult = OnTCPNetWorkMainQueryService(Command.wSubCmdID,pData,wDataSize,dwSocketID,dwTokenID);
			break;
		}
	}

	//断开链接
	if (bResult == false)
	{
		//构造结构
		CMD_CS_C_ShutdownSocket ShutdownSocket;
		ShutdownSocket.bForceClose = false;

		//发送数据
		SendDataToGate(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket), &dwTokenID, 1);
	}

	return true;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//网关服务
	if (pBindParameter->wServiceModule==SERVICE_MODULE_GATE)
	{
		//变量定义
		WORD wBindIndex=LOWORD(dwSocketID);

		//查找网关
		CGlobalGateItem * pGlobalGateItem=m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem!=NULL)
		{
			//注销网关
			m_GlobalItemManager.DeleteGateItem(pBindParameter->wServiceID);					
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
	case DBO_GR_USERINFO_LIST:				//用户列表
	{
		return OnDBUserInfoList(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_FRIENDINFO_LIST:			//好友列表
	{
		return OnDBFriendInfoList(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_FRIENDEXTRAS_LIST:			//附加列表
	{
		return OnDBFriendExtrasList(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_SYNC_USERINFO_RESULT:		//同步结果
	{
		return OnDBSyncUserInfoResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_QUERY_USERINFO_RESULT:		//查询结果
	{
		return OnDBQueryUserInfoResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_QUERY_BLACKLIST_RESULT:		//查询结果
	{
		return OnDBQueryBlackListResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_QUERY_FRIENDLIST_RESULT:	 //查询结果
	{
		return OnDBQueryFriendListResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_QUERY_FRIENDINFO_RESULT:	 //查询结果
	{
		return OnDBQueryFriendInfoResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_QUERY_FRIENDEXTRAS_RESULT:	 //查询结果
	{
		return OnDBQueryFriendExtrasResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_BUILD_ACCESSTOKEN_RESULT:	//令牌结果
	{
		return OnDBBuildAccessTokenResult(dwContextID, dwTokenID, pData, wDataSize);
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
		RegisterService.wServicePort=m_pServiceOption->wServicePort;
		RegisterService.wServiceModule= SERVICE_MODULE_QUERY;
		RegisterService.wServiceKindID = m_pServiceOption->wServiceKindID;
		StringCchCopy(RegisterService.szServiceName, CountArray(RegisterService.szServiceName), TEXT("查询服务器"));
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
	if (dwServiceID==NETWORK_CENTER)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:			//注册服务
			{
				return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_COMMON_SERVICE:		//服务信息
			{
				return OnTCPSocketMainCommonService(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_MANAGER_SERVICE:	//管理服务
			{
				return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_FORWARD_SERVICE:	//转发服务
			{
				return OnTCPSocketMainForwardService(Command.wSubCmdID,pData,wDataSize);
			}
		}
	}	

	//错误断言
	ASSERT(FALSE);

	return true;
}

//注册事件
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_SUCCESS:		//注册成功
		{
			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CENTER_RESULT,&ControlResult,sizeof(ControlResult));	

			//变量定义
			//BYTE cbBuffer[SOCKET_PACKET];

			//auto pServiceRoute = (tagServiceHash*)cbBuffer;

			//////for (int i = 0; i < 100; i++)
			//{
			//	pServiceRoute->dwHashKeyID = 15;
			//	pServiceRoute->dwHashParam = MsgQueueKind::MQK_WORLD;
			//	*(DWORD*)pServiceRoute->cbExtendData = 10;

			//	lhchat::QueryUserInfoRequest QueryRequest;
			//	QueryRequest.add_uid(135);
			//	QueryRequest.add_uid(100);
			//	QueryRequest.add_uid(101);
			//	QueryRequest.add_uid(102);
			//	QueryRequest.add_uid(136);
			//	for (int i = 0; i < 50; i++)
			//	{
			//		QueryRequest.add_uid(10+i);
			//	}

			//	//序列化
			//	string sMessageRequest;
			//	QueryRequest.SerializeToString(&sMessageRequest);

			//	//拷贝数据
			//	CopyMemory(pServiceRoute + 1, (char*)sMessageRequest.data(), sMessageRequest.size());

			//	m_pIDataCacheEngine->PostDataBaseRequest(DBR_GR_QUERY_USERINFO, pServiceRoute->dwHashKeyID, 0, 0, cbBuffer, sizeof(tagServiceHash) + sMessageRequest.size());
			//}		

			//变量定义
			//BYTE cbBuffer[SOCKET_PACKET];

			//auto pServiceRoute = (tagServiceHash*)cbBuffer;

			//////for (int i = 0; i < 100; i++)
			//{
			//	pServiceRoute->dwHashKeyID = 15;
			//	pServiceRoute->dwHashParam = MsgQueueKind::MQK_WORLD;
			//	*(DWORD*)pServiceRoute->cbExtendData = 10;

			//	lhchat::BuildAccessTokenRequest BuildRequest;
			//	BuildRequest.set_channelkind(ChannelKind::CK_SYSTEM);
			//	BuildRequest.set_channelname("132456");

			//	//序列化
			//	string sBuildRequest;
			//	BuildRequest.SerializeToString(&sBuildRequest);

			//	//拷贝数据
			//	CopyMemory(pServiceRoute + 1, (char*)sBuildRequest.data(), sBuildRequest.size());

			//	m_pIDataCacheEngine->PostDataBaseRequest(DBR_GR_BUILD_ACCESSTOKEN, pServiceRoute->dwHashKeyID, 0, 0, cbBuffer, sizeof(tagServiceHash) + sBuildRequest.size());
			//}

			return true;
		}
	case SUB_CS_S_REGISTER_FAILURE:		//注册失败
		{
			//效验参数
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//关闭处理
			m_pITCPSocketCenter->CloseSocket();

			//显示消息
			CW2CT strDescribeString(pRegisterFailure->szDescribeString);
			if (lstrlen(strDescribeString)>0) CTraceService::TraceString(strDescribeString,TraceLevel_Exception);

			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_FAILURE;
			SendUIControlPacket(UI_CENTER_RESULT,&ControlResult,sizeof(ControlResult));

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
			ASSERT(wDataSize % sizeof(tagServiceItem) == 0);
			if (wDataSize % sizeof(tagServiceItem) != 0) return false;

			//提取数据
			tagServiceItem * pServiceItem = (tagServiceItem *)pData;			

			return true;
		}
		case SUB_CS_S_SERVICE_REMOVE:	//服务移除
		{
			//参数校验
			ASSERT(wDataSize == sizeof(CMD_CS_S_ServiceRemove));
			if (wDataSize != sizeof(CMD_CS_S_ServiceRemove)) return false;

			//提取数据
			CMD_CS_S_ServiceRemove * pServiceRemove = (CMD_CS_S_ServiceRemove *)pData;

			return true;
		}
	}

	return true;
}

//管理服务
bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//转发服务
bool CAttemperEngineSink::OnTCPSocketMainForwardService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//注册服务
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{	
	case SUB_CS_C_REGISTER_SERVICE:		//注册网关
		{
			//效验数据
			ASSERT (wDataSize==sizeof(CMD_CS_C_RegisterService));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterService)) return false;

			//消息定义
			CMD_CS_C_RegisterService * pRegisterService =(CMD_CS_C_RegisterService *)pData;

			//校验模块
			if (pRegisterService->wServiceModule != SERVICE_MODULE_GATE) return false;

			//查找网关
			if (m_GlobalItemManager.SearchGateItem(pRegisterService->wServiceID)!=NULL)
			{
				//变量定义
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//设置变量
				RegisterFailure.lErrorCode=0;
				StringCchCopy(RegisterFailure.szDescribeString,CountArray(RegisterFailure.szDescribeString), TEXT("已经存在相同标识的网关服务，网关服务注册失败"));

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
			(m_pBindParameter+wBindIndex)->wServiceID= pRegisterService->wServiceID;
			(m_pBindParameter + wBindIndex)->wServiceModule = pRegisterService->wServiceModule;

			//变量定义
			tagServiceItem GateItem;
			ZeroMemory(&GateItem,sizeof(GateItem));

			//构造数据
			GateItem.wServiceID= pRegisterService->wServiceID;
			GateItem.wServicePort= pRegisterService->wServicePort;
			GateItem.wServiceModule = pRegisterService->wServiceModule;
			GateItem.dwServiceAddr= pRegisterService->dwServiceAddr;
			StringCchCopy(GateItem.szServiceName, CountArray(GateItem.szServiceName), pRegisterService->szServiceName);
			StringCchCopy(GateItem.szServiceDomain, CountArray(GateItem.szServiceDomain), pRegisterService->szServiceDomain);

			//注册房间
			CGlobalGateItem * pGlobalGateItem=m_GlobalItemManager.ActiveGateItem(dwSocketID,GateItem);

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
	return false;
}

//用户列表
bool CAttemperEngineSink::OnDBUserInfoList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//获取参数
	auto pBindParameter = GetBindParameter(dwContextID);
	if (pBindParameter == NULL) return true;

	//发送消息
	return SendDataToGate(dwContextID, MDM_AQ_QUERY_SERVICE, SUB_AQ_S_USERINFO_LIST, pData, wDataSize, &dwTokenID, 1);
}

//好友列表
bool CAttemperEngineSink::OnDBFriendInfoList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//获取参数
	auto pBindParameter = GetBindParameter(dwContextID);
	if (pBindParameter == NULL) return true;

	//发送消息
	return SendDataToGate(dwContextID, MDM_AQ_QUERY_SERVICE, SUB_AQ_S_FRIENDINFO_LIST, pData, wDataSize, &dwTokenID, 1);
}

//附加列表
bool CAttemperEngineSink::OnDBFriendExtrasList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//获取参数
	auto pBindParameter = GetBindParameter(dwContextID);
	if (pBindParameter == NULL) return true;

	//发送消息
	return SendDataToGate(dwContextID, MDM_AQ_QUERY_SERVICE, SUB_AQ_S_FRIENDEXTRAS_LIST, pData, wDataSize, &dwTokenID, 1);
}

//同步结果
bool CAttemperEngineSink::OnDBSyncUserInfoResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//获取参数
	auto pBindParameter = GetBindParameter(dwContextID);
	if (pBindParameter == NULL) return true;

	//发送消息
	return SendDataToGate(dwContextID, MDM_AQ_QUERY_SERVICE, SUB_AQ_S_SYNC_USERINFO_RESULT, pData, wDataSize, &dwTokenID, 1);
}

//查询结果
bool CAttemperEngineSink::OnDBQueryUserInfoResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//获取参数
	auto pBindParameter = GetBindParameter(dwContextID);
	if (pBindParameter == NULL) return true;

	//发送消息
	return SendDataToGate(dwContextID, MDM_AQ_QUERY_SERVICE, SUB_AQ_S_QUERY_USERINFO_RESULT, pData, wDataSize, &dwTokenID, 1);
}

//查询结果
bool CAttemperEngineSink::OnDBQueryBlackListResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//获取参数
	auto pBindParameter = GetBindParameter(dwContextID);
	if (pBindParameter == NULL) return true;

	//发送消息
	return SendDataToGate(dwContextID, MDM_AQ_QUERY_SERVICE, SUB_AQ_S_QUERY_BLACKLIST_RESULT, pData, wDataSize, &dwTokenID, 1);
}

//查询结果
bool CAttemperEngineSink::OnDBQueryFriendListResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//获取参数
	auto pBindParameter = GetBindParameter(dwContextID);
	if (pBindParameter == NULL) return true;

	//发送消息
	return SendDataToGate(dwContextID, MDM_AQ_QUERY_SERVICE, SUB_AQ_S_QUERY_FRIENDLIST_RESULT, pData, wDataSize, &dwTokenID, 1);
}

//查询结果
bool CAttemperEngineSink::OnDBQueryFriendInfoResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//获取参数
	auto pBindParameter = GetBindParameter(dwContextID);
	if (pBindParameter == NULL) return true;

	//发送消息
	return SendDataToGate(dwContextID, MDM_AQ_QUERY_SERVICE, SUB_AQ_S_QUERY_FRIENDINFO_RESULT, pData, wDataSize, &dwTokenID, 1);
}

//查询结果
bool CAttemperEngineSink::OnDBQueryFriendExtrasResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//获取参数
	auto pBindParameter = GetBindParameter(dwContextID);
	if (pBindParameter == NULL) return true;

	//发送消息
	return SendDataToGate(dwContextID, MDM_AQ_QUERY_SERVICE, SUB_AQ_S_QUERY_FRIENDEXTRAS_RESULT, pData, wDataSize, &dwTokenID, 1);
}

//令牌结果
bool CAttemperEngineSink::OnDBBuildAccessTokenResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//获取参数
	auto pBindParameter = GetBindParameter(dwContextID);
	if (pBindParameter == NULL) return true;

	//发送消息
	return SendDataToGate(dwContextID, MDM_AQ_QUERY_SERVICE, SUB_AQ_S_BUILD_ACCESSTOKEN_RESULT, pData, wDataSize, &dwTokenID, 1);
}

//发送请求
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//发送数据
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->m_UIControlSink.PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//网关发送
bool CAttemperEngineSink::SendDataToGate(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	//发送数据
	return m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(wMainCmdID,SERVICE_MODULE_QUERY),wSubCmdID,pData,wDataSize, pdwTokenID, wTokenCount);
}

//令牌参数
tagBindParameter* CAttemperEngineSink::GetBindParameter(DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex = LOWORD(dwSocketID);
	return (m_pBindParameter + wBindIndex);		
}

//查询服务
bool CAttemperEngineSink::OnTCPNetWorkMainQueryService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_AQ_C_SYNC_USERINFO:		//同步信息
	{
		//提取数据
		auto pServiceRoute = (tagServiceHash*)pData;

		//投递数据
		m_pIDataCacheEngine->PostDataBaseRequest(DBR_GR_SYNC_USERINFO, pServiceRoute->dwHashKeyID, dwSocketID, dwTokenID, pData, wDataSize);

		return true;
	}
	case SUB_AQ_C_QUERY_USERINFO:		//查询信息
	{
		//提取数据
		auto pServiceRoute = (tagServiceHash*)pData;

		//投递数据
		m_pIDataCacheEngine->PostDataBaseRequest(DBR_GR_QUERY_USERINFO, pServiceRoute->dwHashKeyID, dwSocketID, dwTokenID, pData, wDataSize);

		return true;
	}
	case SUB_AQ_C_QUERY_BLACKLIST:		//查询信息
	{
		//提取数据
		auto pServiceRoute = (tagServiceHash*)pData;

		//投递数据
		m_pIDataCacheEngine->PostDataBaseRequest(DBR_GR_QUERY_BLACKLIST, pServiceRoute->dwHashKeyID, dwSocketID, dwTokenID, pData, wDataSize);

		return true;
	}
	case SUB_AQ_C_QUERY_FRIENDLIST:		//查询信息
	{
		//提取数据
		auto pServiceRoute = (tagServiceHash*)pData;

		//投递数据
		m_pIDataCacheEngine->PostDataBaseRequest(DBR_GR_QUERY_FRIENDLIST, pServiceRoute->dwHashKeyID, dwSocketID, dwTokenID, pData, wDataSize);

		return true;
	}
	case SUB_AQ_C_QUERY_FRIENDINFO:		//查询信息
	{
		//提取数据
		auto pServiceRoute = (tagServiceHash*)pData;

		//投递数据
		m_pIDataCacheEngine->PostDataBaseRequest(DBR_GR_QUERY_FRIENDINFO, pServiceRoute->dwHashKeyID, dwSocketID, dwTokenID, pData, wDataSize);

		return true;
	}
	case SUB_AQ_C_QUERY_FRIENDEXTRAS:	//查询信息
	{
		//提取数据
		auto pServiceRoute = (tagServiceHash*)pData;

		//投递数据
		m_pIDataCacheEngine->PostDataBaseRequest(DBR_GR_QUERY_FRIENDEXTRAS, pServiceRoute->dwHashKeyID, dwSocketID, dwTokenID, pData, wDataSize);

		return true;
	}
	case SUB_AQ_C_BUILD_ACCESSTOKEN:	//生成令牌
	{
		//提取数据
		auto pServiceRoute = (tagServiceHash*)pData;

		//投递数据
		m_pIDataCacheEngine->PostDataBaseRequest(DBR_GR_BUILD_ACCESSTOKEN, pServiceRoute->dwHashKeyID, dwSocketID, dwTokenID, pData, wDataSize);

		return true;
	}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
