#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_REPORT_ONLINE			1											//上报在线

//时间标识
#define IDI_CONNECT_CENTER			10											//重连标识

//////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//状态变量
	m_bNeedBattle=true;

	//状态变量
	m_pServiceOption=NULL;
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//组件变量
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_TCPSocketCenter=NULL;


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

	//设置接口
	m_GlobalInfoManager.SetIAttemperEngine(m_pIAttemperEngine);
	m_GlobalInfoManager.SetServiceAttribute(m_ServiceAttribute);

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//状态变量
	m_bNeedBattle=true;

	//组件变量
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_TCPSocketCenter=NULL;

	//重置对象
	m_GlobalInfoManager.ResetData();

	//删除数据
	SafeDeleteArray(m_pBindParameter);	

	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	switch (dwIdentifier)
	{
	case CT_CONNECT_CENTER:		//连接中心
		{
			//获取地址					
			tagAddressInfo * pAddressInfo=&m_pInitParameter->m_CenterAddress;

			//发起连接
			m_TCPSocketCenter->Connect(htonl(pAddressInfo->dwServiceAddr), m_pInitParameter->m_wCenterPort);

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
	//上报人数
	if (dwTimerID == IDI_REPORT_ONLINE)
	{
		//构造结构
		CMD_CS_C_ServiceOnline ServiceOnline;
		ZeroMemory(&ServiceOnline,sizeof(ServiceOnline));

		//在线人数
		ServiceOnline.wOnlineCount=GetOnlineCount();

		//发送消息
		m_TCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_SERVICE_ONLINE,&ServiceOnline,sizeof(ServiceOnline),0);

		return true;
	}

	//连接中心
	if (dwTimerID==IDI_CONNECT_CENTER)
	{
		//获取地址					
		tagAddressInfo * pAddressInfo=&m_pInitParameter->m_CenterAddress;

		//发起连接
		m_TCPSocketCenter->Connect(htonl(pAddressInfo->dwServiceAddr), m_pInitParameter->m_wCenterPort);

		//提示消息
		CTraceService::TraceString(TEXT("正在尝试网关中心服务器..."),TraceLevel_Normal);

		return true;
	}
	
	//服务信息
	WORD wServiceID = LOWORD(dwTimerID);
	WORD wServiceIndex = HIWORD(dwTimerID)-1;		

	//关闭处理		
	CGlobalServiceInfo * pGlobalServiceItem = m_GlobalInfoManager.SearchServiceInfo(wServiceIndex, wServiceID);
	if (pGlobalServiceItem != NULL && pGlobalServiceItem->m_pITCPSocketService != NULL)
	{
		//连接服务
		pGlobalServiceItem->m_pITCPSocketService->Connect(pGlobalServiceItem->m_ServiceItem.szServiceDomain, pGlobalServiceItem->m_ServiceItem.wServicePort);

		//提示消息
		CTraceService::TraceString(TEXT("正在尝试连接[%d]号%s..."), TraceLevel_Normal, wServiceID, m_ServiceAttribute[wServiceIndex].szServiceName);
	}

	return true;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//获取索引
	ASSERT (LOWORD(dwSocketID)<m_pServiceOption->wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pServiceOption->wMaxConnect) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//设置变量	
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientIP=dwClientIP;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);	

	//用户信息
	pBindParameter->dwUserID = 0;		
	ZeroMemory(pBindParameter->szLogonToken,sizeof(pBindParameter->szLogonToken));

	//服务信息
	ZeroMemory(pBindParameter->LinkState, sizeof(pBindParameter->LinkState));
	ZeroMemory(pBindParameter->wServiceID, sizeof(pBindParameter->wServiceID));		

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//服务模块
	WORD wServiceModule=HICMMD(Command.wMainCmdID);

	//校验模块
	if (wServiceModule > SERVICE_COUNT)
	{
		return false;
	}

	//网关服务
	if (wServiceModule == SERVICE_MODULE_GATE)
	{
		return OnTCPNetworkMainGateService(Command, pData, wDataSize, dwSocketID, dwTokenID);
	}

	return OnTCPNetworkMainInsideService(Command, pData, wDataSize, dwSocketID, dwTokenID);
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//获取对象
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//离开服务
	for (WORD i=0; i<CountArray(pBindParameter->LinkState); i++)
	{
		//校验状态
		if (pBindParameter->LinkState[i] == LS_NULL) continue;

		//注销令牌
		LogoutToken(i, pBindParameter->wServiceID[i], pBindParameter->dwSocketID);
	}

	//清除信息
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason)
{
	//中心连接
	if (dwServiceID==NETWORK_CENTER)
	{
		//提示消息
		CTraceService::TraceString(TEXT("与中心服务器的连接关闭了，%ld 秒后将重新连接"),TraceLevel_Warning, m_pInitParameter->m_wConnectTime);

		//设置时间
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L,1,0);

		return true;
	}
	//其他连接
	else
	{
		//计算索引
		WORD wServiceID = LOWORD(dwServiceID);
		WORD wServiceModule = HIWORD(dwServiceID);
		WORD wServiceIndex= wServiceModule -1;

		//提示消息
		CTraceService::TraceString(TEXT("与[%d]号%s的连接断开了"), TraceLevel_Warning, wServiceID, m_ServiceAttribute[wServiceIndex].szServiceName);

		//移除服务
		m_GlobalInfoManager.DeleteServiceItem(wServiceIndex, wServiceID);

		//设置时间
		ASSERT(m_pITimerEngine != NULL);
		m_pITimerEngine->SetTimer(dwServiceID, 2000L, 1, 0);
		
		//关闭链接
		ShutdownServiceLink(wServiceModule, wServiceID);

		return true;
	}

	return false;
}

//连接事件
bool CAttemperEngineSink::OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode)
{
	//中心连接
	if (dwServiceID == NETWORK_CENTER)
	{
		//错误判断
		if (nErrorCode!=0)
		{
			//提示消息
			CTraceService::TraceString(TEXT("中心服务器连接失败 [ %ld ]，%ld 秒后将重新连接"),TraceLevel_Warning, nErrorCode, m_pInitParameter->m_wConnectTime);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,2000L,1,0);

			return false;
		}

		//提示消息
		CTraceService::TraceString(TEXT("正在获取服务信息..."),TraceLevel_Normal);

		//变量定义
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService,sizeof(RegisterService));

		//设置变量	
		RegisterService.wServiceMask = 0xFFFF;
		RegisterService.wServiceID=m_pServiceOption->wServiceID;
		RegisterService.wServiceModule= SERVICE_MODULE_GATE;
		RegisterService.wServicePort=m_pServiceOption->wServicePort;
		RegisterService.wMaxConnect = m_pServiceOption->wMaxConnect;
		RegisterService.wServiceKindID = m_pServiceOption->wServiceKindID;		
		StringCchCopy(RegisterService.szServiceDomain, CountArray(RegisterService.szServiceDomain), m_pServiceOption->szServiceDomain);

		//发送数据
		m_TCPSocketCenter->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_SERVICE, &RegisterService, sizeof(RegisterService), 0);

		return true;
	}
	else
	{
		//计算索引
		WORD wServiceID = LOWORD(dwServiceID);
		WORD wServiceModule = HIWORD(dwServiceID);		
		WORD wServiceIndex = HIWORD(dwServiceID) - 1;

		//错误判断
		if (nErrorCode != 0)
		{
			//提示消息
			CTraceService::TraceString(TEXT("与[%d]号%s连接失败 [ %ld ]，%ld 秒后将重新连接"), TraceLevel_Warning, wServiceID, 
				m_ServiceAttribute[wServiceIndex].szServiceName, nErrorCode, m_pInitParameter->m_wConnectTime);

			//设置时间
			ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(GetTimerID(wServiceIndex, wServiceID), 2000L, 1, 0);

			return false;
		}

		//获取接口
		ITCPSocketService * pITCPSocketService = m_GlobalInfoManager.GetSocketService(wServiceIndex, wServiceID);
		if (pITCPSocketService != NULL)
		{
			//变量定义
			CMD_CS_C_RegisterService RegisterService;
			ZeroMemory(&RegisterService, sizeof(RegisterService));

			//设置变量	
			RegisterService.wServiceID = m_pServiceOption->wServiceID;
			RegisterService.wServiceModule = SERVICE_MODULE_GATE;
			RegisterService.wServicePort = m_pServiceOption->wServicePort;
			RegisterService.wMaxConnect = m_pServiceOption->wMaxConnect;
			RegisterService.wServiceKindID = m_pServiceOption->wServiceKindID;
			StringCchCopy(RegisterService.szServiceDomain, CountArray(RegisterService.szServiceDomain), m_pServiceOption->szServiceDomain);

			//发送数据
			pITCPSocketService->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_SERVICE, &RegisterService, sizeof(RegisterService), 0);
		}

		return true;
	}

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	try
	{
		//提取命令码
		WORD wMainCmdID = LOCMMD(Command.wMainCmdID);

		//中心连接
		if (dwServiceID == NETWORK_CENTER)
		{
			switch (wMainCmdID)
			{
			case MDM_CS_REGISTER:		//注册服务
				{
					return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
				}
			case MDM_CS_COMMON_SERVICE:	//服务信息
				{
					return OnTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
				}		
			}

			return true;
		}
		else
		{
			switch (wMainCmdID)
			{
			case MDM_CS_REGISTER:		//注册服务
				{
					return OnTCPSocketMainRegister(dwServiceID, Command, pData, wDataSize);
				}
			case MDM_CS_COMMON_SERVICE:	//通用服务
				{
					return OnTCPSocketMainCommonService(dwServiceID, Command, pData, wDataSize, pdwTokenID, wTokenCount);
				}
			default:					//服务命令	
				{					
					return OnTCPSocketMainInsideService(dwServiceID, Command, pData, wDataSize, pdwTokenID, wTokenCount);
				}
			}

			return true;
		}		
	} 
	catch (...)
	{
		CTraceService::TraceString(TEXT("wServiceID=%d,wMainCmdID=%d,wSubCmdID=%d"),TraceLevel_Exception, dwServiceID, Command.wMainCmdID, Command.wSubCmdID);
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//注册服务
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_SUCCESS:		//注册成功
		{
			//事件处理
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CENTER_RESULT ,&ControlResult,sizeof(ControlResult));		

			//设置定时器
			ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_REPORT_ONLINE,m_pInitParameter->m_wReportTime,-1,NULL);

			return true;
		}
	case SUB_CS_S_REGISTER_FAILURE:		//注册失败
		{
			//变量定义
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;

			//效验参数
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//关闭处理
			m_TCPSocketCenter->CloseSocket();

			//显示消息
			CW2CT strDescribeString(pRegisterFailure->szDescribeString);
			if (lstrlen(strDescribeString)>0) CTraceService::TraceString(strDescribeString,TraceLevel_Exception);

			return true;
		}
	}

	return true;
}

//网关服务
bool CAttemperEngineSink::OnTCPNetworkMainGateService(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//获取对象
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);

	//服务模块
	WORD wSubCmdID = Command.wSubCmdID;
	WORD wMainCmdID = LOCMMD(Command.wMainCmdID);

	//链接服务
	if (wMainCmdID == MDM_GG_GATE_SERVICE && wSubCmdID == SUB_GG_C_LINK_SERVICE)
	{
		//提取数据
		CMD_GG_C_LinkService * pLinkService = (CMD_GG_C_LinkService *)pData;

		//服务索引
		WORD wServiceIndex = pLinkService->wServiceModule - 1;

		//服务索引
		if (wServiceIndex > CountArray(pBindParameter->wServiceID))
		{
			return false;
		}

		//校验令牌
		if (pLinkService->wServiceModule != SERVICE_MODULE_LOGON)
		{
			//校验用户
			if (pLinkService->dwUserID != pBindParameter->dwUserID || lstrcmp(pBindParameter->szLogonToken, pLinkService->szLogonToken) != 0)
			{
				//构造结构
				CMD_GG_S_LinkFailure LinkFailure;
				ZeroMemory(&LinkFailure, sizeof(LinkFailure));

				//服务信息
				LinkFailure.wServiceID = pLinkService->wServiceID;
				LinkFailure.wServiceModule = pLinkService->wServiceModule;

				//错误信息
				LinkFailure.lErrorCode = 1;
				StringCchCopy(LinkFailure.szErrorDescribe, CountArray(LinkFailure.szErrorDescribe), CWHService::FormatMessage(MIM_GATE_0004, L"登录令牌验证失败！"));

				//发送消息
				WORD wPacketSize = sizeof(LinkFailure) - sizeof(LinkFailure.szErrorDescribe) + CountStringBuffer(LinkFailure.szErrorDescribe);
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_S_LINK_FAILURE, &LinkFailure, wPacketSize, NULL, 0);

				return false;
			}
		}

		//校验状态
		if (pBindParameter->LinkState[wServiceIndex] != LS_NULL)
		{
			//构造结构
			CMD_GG_S_LinkFailure LinkFailure;
			ZeroMemory(&LinkFailure, sizeof(LinkFailure));

			//服务信息
			LinkFailure.wServiceID= pLinkService->wServiceID;
			LinkFailure.wServiceModule = pLinkService->wServiceModule;

			//错误信息
			LinkFailure.lErrorCode = 1;
			StringCchCopy(LinkFailure.szErrorDescribe, CountArray(LinkFailure.szErrorDescribe), CWHService::FormatMessage(MIM_GATE_0001, L"当前服务模块链接状态异常，请重新登录再次进入！"));

			//发送消息
			WORD wPacketSize = sizeof(LinkFailure) - sizeof(LinkFailure.szErrorDescribe) + CountStringBuffer(LinkFailure.szErrorDescribe);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MAKECMMD(MDM_GG_GATE_SERVICE,SERVICE_MODULE_GATE), SUB_GG_S_LINK_FAILURE, &LinkFailure, wPacketSize, NULL, 0);

			return false;
		}

		//设置变量
		pBindParameter->LinkRecord[wServiceIndex].wTimes = 0;
		pBindParameter->LinkRecord[wServiceIndex].wReqServiceID = pLinkService->wServiceID;		
		CopyMemory(pBindParameter->LinkRecord[wServiceIndex].cbLinkExtend, pLinkService->cbLinkExtend, sizeof(pLinkService->cbLinkExtend));

		//分配服务
		pLinkService->wServiceID = AssignInsideService(pLinkService->wServiceModule, dwSocketID);		

		//校验标识
		if (pLinkService->wServiceID == INVALID_SERVICE_ID)
		{
			//非路由映射
			if (m_ServiceAttribute[wServiceIndex].wServiceMapKind != MAP_KIND_ROUTE)
			{
				//重置记录
				ZeroMemory(&pBindParameter->LinkRecord[wServiceIndex], sizeof(pBindParameter->LinkRecord[wServiceIndex]));

				//构造结构
				CMD_GG_S_LinkFailure LinkFailure;
				ZeroMemory(&LinkFailure, sizeof(LinkFailure));

				//服务信息
				LinkFailure.wServiceID = pLinkService->wServiceID;
				LinkFailure.wServiceModule = pLinkService->wServiceModule;

				//错误信息			
				LinkFailure.lErrorCode = 1;
				StringCchCopy(LinkFailure.szErrorDescribe, CountArray(LinkFailure.szErrorDescribe), CWHService::FormatMessage(MIM_GATE_0002, L"服务器繁忙，请您稍后再次尝试！"));

				//发送消息
				WORD wPacketSize = sizeof(LinkFailure) - sizeof(LinkFailure.szErrorDescribe) + CountStringBuffer(LinkFailure.szErrorDescribe);
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_S_LINK_FAILURE, &LinkFailure, wPacketSize, NULL, 0);
			}

			return true;
		}

		//链接校验
		if (pLinkService->wServiceID > NONE_SERVICE_ID)
		{
			//设置参数
			pBindParameter->LinkState[wServiceIndex] = LS_SHAKEHAND;
			pBindParameter->wServiceID[wServiceIndex] = pLinkService->wServiceID;

			//注册令牌
			CMD_CS_C_RegisterToken RegisterToken;
			ZeroMemory(&RegisterToken, sizeof(RegisterToken));

			//构造结构 
			RegisterToken.dwTokenID = dwSocketID;
			RegisterToken.dwUserID = pBindParameter->dwUserID;
			RegisterToken.dwClientIP = pBindParameter->dwClientIP;

			//发送消息
			SendDataToService(wServiceIndex, pLinkService->wServiceID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_C_REGISTER_TOKEN, &RegisterToken, sizeof(RegisterToken));
		}
		else
		{
			//设置参数
			pBindParameter->LinkState[wServiceIndex] = LS_LINKING;
			pBindParameter->wServiceID[wServiceIndex] = pLinkService->wServiceID;

			//构造结构
			CMD_GG_S_LinkSuccess LinkSuccess;
			ZeroMemory(&LinkSuccess, sizeof(LinkSuccess));

			//服务信息
			LinkSuccess.wServiceID = pLinkService->wServiceID;
			LinkSuccess.wServiceModule = pLinkService->wServiceModule;

			//发送消息
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_S_LINK_SUCCESS, &LinkSuccess, sizeof(LinkSuccess), NULL, 0);
		}

		return true;
	}

	//离开服务
	if (wMainCmdID == MDM_GG_GATE_SERVICE && wSubCmdID == SUB_GG_C_DISLINK_SERVICE)
	{
		//提取数据
		CMD_GG_C_DislinkService * pDislinkService = (CMD_GG_C_DislinkService *)pData;

		//校验索引
		WORD wServiceIndex = pDislinkService->wServiceModule-1;
		if (wServiceIndex >= CountArray(pBindParameter->wServiceID))
		{
			return false;
		}

		//校验状态
		if (pBindParameter->LinkState[wServiceIndex] == LS_NULL)
		{
			return true;
		}

		//注销令牌
		LogoutToken(wServiceIndex,pBindParameter->wServiceID[wServiceIndex], dwSocketID);

		//关闭链接
		ShutdownServiceLink(pDislinkService->wServiceModule, pBindParameter->wServiceID[wServiceIndex], dwSocketID);

		//转发消息
		return true;
	}	

	return false;
}

//内部服务
bool CAttemperEngineSink::OnTCPNetworkMainInsideService(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//获取参数
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//服务模块
	WORD wServiceModule = HICMMD(Command.wMainCmdID);
	WORD wServiceIndex = wServiceModule - 1;

	//校验模块
	if (wServiceIndex >= CountArray(pBindParameter->wServiceID))
	{
		return false;
	}
	
	//变量定义
	auto LinkState = pBindParameter->LinkState[wServiceIndex];
	auto wServiceID = pBindParameter->wServiceID[wServiceIndex];

	//校验状态
	if (LinkState != LS_LINKING)
	{
		//构造结构
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage, sizeof(SystemMessage));

		//变量定义						
		SystemMessage.wType = SMT_EJECT;		
		StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), CWHService::FormatMessage(MIM_GATE_0003, L"您请求的服务器繁忙！"));

		//打印消息
		CTraceService::TraceString(TEXT("ServiceModule=%d,ErrorMsg=\"%s\""), TraceLevel_Info, wServiceModule,SystemMessage.szString);

		//发送消息
		WORD wPacketSize = sizeof(SystemMessage) - sizeof(SystemMessage.szString) + CountStringBuffer(SystemMessage.szString);
		m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(MDM_CM_SYSTEM, SERVICE_MODULE_GATE), SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wPacketSize, NULL, 0);

		return true;
	}

	//校验标识
	if (wServiceID == NONE_SERVICE_ID)
	{
		//提取数据
		auto pServiceHash = (tagServiceHash*)pData;

		//映射服务
		auto pGlobalServiceItem = m_GlobalInfoManager.MapServiceInfo(wServiceIndex, (WORD)pServiceHash->dwHashParam, pServiceHash->dwHashKeyID);
		if (pGlobalServiceItem == NULL)
		{
			//构造结构
			CMD_CM_SystemMessage SystemMessage;
			ZeroMemory(&SystemMessage, sizeof(SystemMessage));

			//变量定义						
			SystemMessage.wType = SMT_EJECT;
			StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), CWHService::FormatMessage(MIM_GATE_0003, TEXT("您请求的服务器不可达！")));

			//打印消息
			CTraceService::TraceString(TEXT("[服务不可达] ServiceModule=%d,UserID=%d,HashKeyID=%d,HashParam=%d"), TraceLevel_Info, wServiceModule,pBindParameter->dwUserID, pServiceHash->dwHashKeyID, pServiceHash->dwHashParam);

			//发送消息
			WORD wPacketSize = sizeof(SystemMessage) - sizeof(SystemMessage.szString) + CountStringBuffer(SystemMessage.szString);
			m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(MDM_CM_SYSTEM, SERVICE_MODULE_GATE), SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wPacketSize, NULL, 0);

			return true;
		}

		//设置标识		
		wServiceID = pGlobalServiceItem->GetServiceID();	

		//扩展信息
		*(DWORD*)pServiceHash->cbExtendData = pBindParameter->dwUserID;
	}

	//转发消息
	return SendDataToService(wServiceIndex, wServiceID, dwSocketID, Command.wMainCmdID, Command.wSubCmdID,pData,wDataSize);
}

//列表事件
bool CAttemperEngineSink::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SERVICE_ATTRIB:	//服务属性
	{
		//效验参数
		ASSERT (wDataSize % sizeof(tagServiceAttrib) == 0);
		if (wDataSize % sizeof(tagServiceAttrib) != 0) return false;

		//服务数目
		auto pServiceAttrib = (tagServiceAttrib*)pData;
		auto wItemCount = wDataSize/sizeof(tagServiceAttrib);

		for (int i = 0; i < wItemCount; i++)
		{
			//服务索引
			WORD wServiceIndex = pServiceAttrib->wServiceModule - 1;

			//校验模块
			if (wServiceIndex < CountArray(m_ServiceAttribute))
			{
				auto pServiceParameter = &m_ServiceAttribute[wServiceIndex];

				//拷贝数据
				CopyMemory(&m_ServiceAttribute[wServiceIndex], pServiceAttrib,sizeof(m_ServiceAttribute[0]));
			}

			pServiceAttrib++;
		}

		return true;
	}
	case SUB_CS_S_SERVICE_INSERT:	//服务插入
	{
		//效验参数
		ASSERT(wDataSize % sizeof(tagServiceItem) == 0);
		if (wDataSize % sizeof(tagServiceItem) != 0) return false;

		//变量定义
		WORD wItemCount = wDataSize / sizeof(tagServiceItem);
		tagServiceItem* pServiceItem = (tagServiceItem*)pData;		

		for (WORD i = 0; i < wItemCount; i++)
		{
			//服务索引
			WORD wServiceIndex = pServiceItem->wServiceModule-1;
			if (wServiceIndex >= CountArray(m_ServiceAttribute)) continue;

			//校验属性
			if (!m_ServiceAttribute[wServiceIndex].bIsExternal) continue;

			//查找子项
			CGlobalServiceInfo* pGlobalServiceItem = m_GlobalInfoManager.SearchServiceInfo(wServiceIndex, pServiceItem->wServiceID);
			if (pGlobalServiceItem != NULL && pGlobalServiceItem->m_pITCPSocketService != NULL) continue;

			//服务标识
			DWORD dwServiceID = MAKELONG(pServiceItem->wServiceID, pServiceItem->wServiceModule);

			//激活服务
			pGlobalServiceItem = m_GlobalInfoManager.ActiveServiceInfo(wServiceIndex, dwServiceID, *pServiceItem);
			if (pGlobalServiceItem == NULL) break;

			//删除定时器
			m_pITimerEngine->KillTimer(MAKELONG(pServiceItem->wServiceID, pServiceItem->wServiceModule));

			//连接服务
			pGlobalServiceItem->m_pITCPSocketService->Connect(pGlobalServiceItem->m_ServiceItem.szServiceDomain, pGlobalServiceItem->m_ServiceItem.wServicePort);

			//提示消息
			CTraceService::TraceString(TEXT("正在尝试连接[%d]号%s..."), TraceLevel_Normal, pServiceItem->wServiceID, m_ServiceAttribute[wServiceIndex].szServiceName);

			pServiceItem++;
		}

		return true;
	}
	case SUB_CS_S_SERVER_INSERT:	//房间插入
	{
		//效验参数
		ASSERT(wDataSize % sizeof(tagGameServer) == 0);
		if (wDataSize % sizeof(tagGameServer) != 0) return false;

		//变量定义
		WORD wItemCount = wDataSize / sizeof(tagGameServer);
		tagGameServer* pGameServer = (tagGameServer*)pData;

		//获取对象
		CServiceUnits* pServiceUnits = CServiceUnits::g_pServiceUnits;

		//读取数据
		for (WORD i = 0; i < wItemCount; i++)
		{
			//服务索引
			WORD wServiceIndex = SERVICE_MODULE_GAME-1;
			if (wServiceIndex >= CountArray(m_ServiceAttribute)) continue;

			//校验属性
			if (!m_ServiceAttribute[wServiceIndex].bIsExternal) continue;

			//查找子项
			CGlobalServiceInfo* pGlobalServiceItem = m_GlobalInfoManager.SearchServiceInfo(wServiceIndex, pGameServer->wServerID);
			if (pGlobalServiceItem != NULL && pGlobalServiceItem->m_pITCPSocketService != NULL) continue;

			//变量定义
			tagServiceItem ServiceItem;
			ZeroMemory(&ServiceItem, sizeof(ServiceItem));

			//构造数据
			ServiceItem.wServiceID = pGameServer->wServerID;
			ServiceItem.wServiceModule = SERVICE_MODULE_GAME;
			ServiceItem.wServicePort = pGameServer->wServerPort;
			ServiceItem.dwServiceAddr = pGameServer->dwServerAddr;
			StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), TEXT("游戏服务器"));
			StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain), pGameServer->szServerDomain);

			//计算标识
			DWORD dwTimerID = MAKELONG(ServiceItem.wServiceID, SERVICE_MODULE_GAME);
			DWORD dwServiceID = MAKELONG(ServiceItem.wServiceID, SERVICE_MODULE_GAME);			

			//创建服务
			pGlobalServiceItem = m_GlobalInfoManager.ActiveServiceInfo(wServiceIndex, dwServiceID, ServiceItem);
			if (pGlobalServiceItem == NULL) break;

			//扩展数据
			CopyMemory(pGlobalServiceItem->GetExtendData(), pGameServer, sizeof(tagGameServer));

			//删除定时器
			m_pITimerEngine->KillTimer(dwTimerID);

			//连接服务
			pGlobalServiceItem->m_pITCPSocketService->Connect(ServiceItem.szServiceDomain, ServiceItem.wServicePort);

			//提示消息
			CTraceService::TraceString(TEXT("正在尝试连接[%d]号%s..."), TraceLevel_Normal, ServiceItem.wServiceID, ServiceItem.szServiceName);

			pGameServer++;
		}

		return true;
	}
	case SUB_CS_S_SERVER_MODIFY:	//房间修改
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_CS_S_ServerModify));
		if (wDataSize != sizeof(CMD_CS_S_ServerModify)) return false;

		//消息定义
		CMD_CS_S_ServerModify* pServerModify = (CMD_CS_S_ServerModify*)pData;

		//查找房间
		auto pGlobalServerItem = m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GAME-1, pServerModify->wServerID);
		if (pGlobalServerItem != NULL)
		{
			auto pGameServer = (tagGameServer*)pGlobalServerItem->GetExtendData();
			if (pGameServer != NULL)
			{
				pGameServer->wServerLevel = pServerModify->wServerLevel;
				pGameServer->dwMasterRule = pServerModify->dwMasterRule;
				pGameServer->dwMaxPlayer = pServerModify->dwMaxPlayer;
				pGameServer->dwOnLineCount = pServerModify->dwOnLineCount;
			}			
		}

		return true;
	}
	case SUB_CS_S_SERVICE_REMOVE:	//服务移除
	case SUB_CS_S_SERVER_REMOVE:	//房间移除
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_CS_S_ServiceRemove));
		if (wDataSize != sizeof(CMD_CS_S_ServiceRemove)) return false;

		//提取数据
		CMD_CS_S_ServiceRemove* pServiceRemove = (CMD_CS_S_ServiceRemove*)pData;

		//校验索引
		WORD wServiceIndex = pServiceRemove->wServiceModule-1;
		if (wServiceIndex >= CountArray(m_ServiceAttribute)) return false;

		//时间标识
		DWORD dwTimerID = MAKELONG(pServiceRemove->wServiceID, pServiceRemove->wServiceModule);

		//删除定时器
		m_pITimerEngine->KillTimer(dwTimerID);

		//查询子项
		CGlobalServiceInfo* pGlobalServiceItem = m_GlobalInfoManager.SearchServiceInfo(wServiceIndex, pServiceRemove->wServiceID);
		if (pGlobalServiceItem != NULL)
		{
			//移除子项
			m_GlobalInfoManager.DeleteServiceItem(wServiceIndex, pServiceRemove->wServiceID);
		}

		return true;
	}
	case SUB_CS_S_SERVICE_ONLINE:	//在线信息
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_CS_S_ServiceOnline));
		if (wDataSize != sizeof(CMD_CS_S_ServiceOnline)) return false;

		//消息定义
		CMD_CS_S_ServiceOnline* pServiceOnline = (CMD_CS_S_ServiceOnline*)pData;

		//游戏房间
		if (pServiceOnline->wServiceModule == SERVICE_MODULE_GAME)
		{
			//计算索引
			auto wServiceIndex = pServiceOnline->wServiceModule-1;

			//查找房间
			auto pGlobalServerItem = m_GlobalInfoManager.SearchServiceInfo(wServiceIndex, pServiceOnline->wServiceID);
			if (pGlobalServerItem != NULL)
			{
				auto pGameServer = (tagGameServer*)pGlobalServerItem->GetExtendData();
				if (pGameServer != NULL)
				{
					pGameServer->dwOnLineCount = pServiceOnline->wOnlineCount;
				}
			}
		}	

		return true;
	}	
	case SUB_CS_S_APPEND_ROUTE_ITEM://添加路由
	{
		//效验数据
		ASSERT((wDataSize - sizeof(CMD_CS_S_AppendRouteItem)) % sizeof(tagRouteItem) == 0);
		if ((wDataSize - sizeof(CMD_CS_S_AppendRouteItem)) % sizeof(tagRouteItem) != 0) return false;

		//提取数据
		auto pAppendRouteItem = (CMD_CS_S_AppendRouteItem*)pData;

		//路由子项
		auto pRouteItem = (tagRouteItem*)(pAppendRouteItem + 1);
		auto wItemCount = (wDataSize - sizeof(CMD_CS_S_AppendRouteItem)) / sizeof(tagRouteItem);

		//服务索引
		auto wServiceIndex = pAppendRouteItem->wServiceModule - 1;

		//查找服务
		auto pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(wServiceIndex, pAppendRouteItem->wServiceID);
		if (pGlobalServiceInfo != NULL)
		{
			//移除路由
			if (pAppendRouteItem->bTheFirst)
			{
				pGlobalServiceInfo->RemoveRouteItem();
			}

			//添加路由
			pGlobalServiceInfo->AppendRouteItem(pRouteItem, wItemCount);
		}

		return true;
	}
	case SUB_CS_S_REMOVE_ROUTE_ITEM://移除路由
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_CS_S_RemoveRouteItem));
		if (wDataSize != sizeof(CMD_CS_S_RemoveRouteItem)) return false;

		//提取数据
		auto pRemoveRouteItem = (CMD_CS_S_RemoveRouteItem*)pData;

		//服务索引
		auto wServiceIndex = pRemoveRouteItem->wServiceModule - 1;

		//查找服务
		auto pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(wServiceIndex, pRemoveRouteItem->wServiceID);
		if (pGlobalServiceInfo != NULL)
		{
			//移除路由
			pGlobalServiceInfo->RemoveRouteItem(pRemoveRouteItem->dwRouteID);
		}

		return true;
	}
	case SUB_CS_S_QUERY_ROUTE_RESULT://查询结果
	{
		//效验数据
		ASSERT(wDataSize == sizeof(CMD_CS_S_QueryRouteResult));
		if (wDataSize != sizeof(CMD_CS_S_QueryRouteResult)) return false;

		//提取数据
		auto pQueryRouteResult = (CMD_CS_S_QueryRouteResult*)pData;

		//绑定参数
		auto pBindParameter = GetBindParameter(LOWORD(pQueryRouteResult->dwTokenID));
		if (pBindParameter == NULL) return true;
		if (pBindParameter->dwSocketID != pQueryRouteResult->dwTokenID) return true;

		//未查询到服务
		if (pQueryRouteResult->wServiceID == NONE_SERVICE_ID)
		{
			//构造结构
			CMD_GG_S_LinkFailure LinkFailure;
			ZeroMemory(&LinkFailure, sizeof(LinkFailure));

			//服务信息
			LinkFailure.wServiceID = INVALID_SERVICE_ID;
			LinkFailure.wServiceModule = pQueryRouteResult->wServiceModule;

			//错误信息			
			LinkFailure.lErrorCode = 1;
			StringCchCopy(LinkFailure.szErrorDescribe, CountArray(LinkFailure.szErrorDescribe), CWHService::FormatMessage(MIM_GATE_0002, L"服务器繁忙，请您稍后再次尝试！"));

			//发送消息
			WORD wPacketSize = sizeof(LinkFailure) - sizeof(LinkFailure.szErrorDescribe) + CountStringBuffer(LinkFailure.szErrorDescribe);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_S_LINK_FAILURE, &LinkFailure, wPacketSize, NULL, 0);
		}
		else
		{
			//服务索引
			auto wServiceIndex = pQueryRouteResult->wServiceModule - 1;
			auto pLinkRecord = &pBindParameter->LinkRecord[wServiceIndex];

			//链接记录
			pLinkRecord->wServiceID[pLinkRecord->wTimes++] = pQueryRouteResult->wServiceID;

			//设置参数
			pBindParameter->LinkState[wServiceIndex] = LS_SHAKEHAND;
			pBindParameter->wServiceID[wServiceIndex] = pQueryRouteResult->wServiceID;

			//注册令牌
			CMD_CS_C_RegisterToken RegisterToken;
			ZeroMemory(&RegisterToken, sizeof(RegisterToken));

			//构造结构 
			RegisterToken.dwUserID = pBindParameter->dwUserID;
			RegisterToken.dwClientIP = pBindParameter->dwClientIP;
			RegisterToken.dwTokenID = pQueryRouteResult->dwTokenID;

			//发送消息
			SendDataToService(wServiceIndex, pQueryRouteResult->wServiceID, RegisterToken.dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_C_REGISTER_TOKEN, &RegisterToken, sizeof(RegisterToken));
		}

		return true;
	}
	}

	return true;
}

//注册服务
bool CAttemperEngineSink::OnTCPSocketMainRegister(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize)
{
	//服务信息
	WORD wServiceID = LOWORD(dwServiceID);
	WORD wServiceIndex = HIWORD(dwServiceID)-1;	

	switch (Command.wSubCmdID)
	{
	case SUB_CS_S_REGISTER_SUCCESS:		//注册成功
		{
			CTraceService::TraceString(TEXT("[%d]号%s连接成功"), TraceLevel_Normal, wServiceID, m_ServiceAttribute[wServiceIndex].szServiceName);

			return true;
		}
	case SUB_CS_S_REGISTER_FAILURE:		//注册失败
		{
			//变量定义
			CMD_CS_S_RegisterFailure * pRegisterFailure = (CMD_CS_S_RegisterFailure *)pData;

			//效验参数
			ASSERT(wDataSize >= (sizeof(CMD_CS_S_RegisterFailure) - sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize < (sizeof(CMD_CS_S_RegisterFailure) - sizeof(pRegisterFailure->szDescribeString))) return false;

			//关闭处理			
			ITCPSocketService * pITCPSocketService = m_GlobalInfoManager.GetSocketService(wServiceIndex,wServiceID);
			if (pITCPSocketService != NULL)
			{
				pITCPSocketService->CloseSocket();
			}

			//显示消息
			CW2CT strDescribeString(pRegisterFailure->szDescribeString);
			if (lstrlen(strDescribeString) > 0) CTraceService::TraceString(strDescribeString, TraceLevel_Exception);

			return true;
		}
	}	

	return true;
}

//通用服务
bool CAttemperEngineSink::OnTCPSocketMainCommonService(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	//服务信息
	WORD wServiceID = LOWORD(dwServiceID);
	WORD wServiceModule = HIWORD(dwServiceID);
	WORD wServiceIndex = HIWORD(dwServiceID)-1;	

	//校验索引
	if (wServiceIndex >= CountArray(m_ServiceAttribute)) return false;

	switch (Command.wSubCmdID)
	{
	case SUB_CS_S_REGISTER_TOKEN:		//注册令牌
	{
		//变量定义	
		CMD_CS_S_RegisterToken* pRegisterToken = (CMD_CS_S_RegisterToken*)pData;

		//获取令牌
		DWORD dwTokenID = *pdwTokenID;

		//绑定参数
		bool bLinkSuccess = false;
		auto pBindParameter = GetBindParameter(LOWORD(dwTokenID));
		if (pBindParameter == NULL) return true;

		//链接记录
		auto pLinkRecord = &pBindParameter->LinkRecord[wServiceIndex];

		//注册成功
		if (pRegisterToken->lResultCode == REGISTER_RESULT_SUCCESS)
		{
			//注销令牌
			if (pBindParameter->dwSocketID == dwTokenID && pBindParameter->LinkState[wServiceIndex] == LS_SHAKEHAND)
			{
				bLinkSuccess = true;				
			}
			else
			{
				//注销令牌
				LogoutToken(wServiceIndex, pBindParameter->wServiceID[wServiceIndex], dwTokenID);

				return true;
			}
		}	
		
		//链接已满
		if (pRegisterToken->lResultCode == REGISTER_RESULT_LINKFULL)
		{
			//未指定服务
			if (pLinkRecord->wReqServiceID == INVALID_SERVICE_ID)
			{
				//分配服务
				WORD wAssignServiceID = AssignInsideService(wServiceModule, dwTokenID);
				if (wAssignServiceID != INVALID_SERVICE_ID)
				{
					//注册令牌
					CMD_CS_C_RegisterToken RegisterToken;
					ZeroMemory(&RegisterToken, sizeof(RegisterToken));

					//构造结构 
					RegisterToken.dwTokenID = dwTokenID;
					RegisterToken.dwUserID = pBindParameter->dwUserID;
					RegisterToken.dwClientIP = pBindParameter->dwClientIP;

					//发送消息
					SendDataToService(wServiceIndex, wAssignServiceID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_C_REGISTER_TOKEN, &RegisterToken, sizeof(RegisterToken));

					return true;
				}				
			}
		}		

		//链接成功
		if (bLinkSuccess == true)
		{
			//更新状态
			pBindParameter->LinkState[wServiceIndex] = LS_LINKING;
			pBindParameter->wServiceID[wServiceIndex] = wServiceID;			

			//构造结构
			CMD_GG_S_LinkSuccess LinkSuccess;
			ZeroMemory(&LinkSuccess, sizeof(LinkSuccess));

			//服务信息
			LinkSuccess.wServiceID = pBindParameter->wServiceID[wServiceIndex];
			LinkSuccess.wServiceModule = wServiceModule;

			//发送消息
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_S_LINK_SUCCESS, &LinkSuccess, sizeof(LinkSuccess), NULL, 0);
		}
		else
		{
			//更新状态
			pBindParameter->LinkState[wServiceIndex] = LS_NULL;
			pBindParameter->wServiceID[wServiceIndex] = NONE_SERVICE_ID;			

			//构造结构
			CMD_GG_S_LinkFailure LinkFailure;
			ZeroMemory(&LinkFailure, sizeof(LinkFailure));

			//服务信息
			LinkFailure.wServiceModule = wServiceModule;
			LinkFailure.wServiceID = pBindParameter->wServiceID[wServiceIndex];

			//错误信息			
			LinkFailure.lErrorCode = 1;
			StringCchCopy(LinkFailure.szErrorDescribe, CountArray(LinkFailure.szErrorDescribe), CWHService::FormatMessage(MIM_GATE_0002, L"服务器繁忙，请您稍后再次尝试！"));

			//发送消息
			WORD wPacketSize = sizeof(LinkFailure) - sizeof(LinkFailure.szErrorDescribe) + CountStringBuffer(LinkFailure.szErrorDescribe);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_S_LINK_FAILURE, &LinkFailure, wPacketSize, NULL, 0);
		}		

		return true;
	}
	case SUB_CS_S_TOKEN_PARAMETER:		//令牌参数
	{
		//提取数据
		CMD_CS_S_TokenParameter * pTokenParameter = (CMD_CS_S_TokenParameter*)pData;
		
		//获取令牌
		DWORD dwTokenID = *pdwTokenID;

		//绑定参数
		auto pBindParameter = GetBindParameter(LOWORD(dwTokenID));
		if (pBindParameter == NULL) return true;

		//令牌参数
		pBindParameter->dwUserID = pTokenParameter->dwUserID;
		StringCchCopy(pBindParameter->szLogonToken, CountArray(pBindParameter->szLogonToken),pTokenParameter->szLogonToken);	

		return true;
	}
	case SUB_CS_S_SHUTDOWN_SOCKET:		//关闭网络
	{
		//参数校验
		ASSERT (wDataSize == sizeof(CMD_CS_C_ShutdownSocket));
		if (wDataSize != sizeof(CMD_CS_C_ShutdownSocket)) return true;

		//提取数据
		CMD_CS_C_ShutdownSocket* pShutdownSocket = (CMD_CS_C_ShutdownSocket*)pData;

		//变量定义	
		DWORD dwTokenID = 0;
		tagBindParameter * pBindParameter = NULL;

		//循环处理
		for (WORD i = 0; i < wTokenCount; i++)
		{
			//获取令牌
			dwTokenID = *pdwTokenID++;

			//校验令牌
			pBindParameter = GetBindParameter(LOWORD(dwTokenID));
			if (pBindParameter->dwSocketID != dwTokenID) continue;
			if (pBindParameter->LinkState[wServiceIndex] == LS_NULL) continue;

			//强制关闭
			if (pShutdownSocket->bForceClose == true)
			{
				//关闭连接
				m_pITCPNetworkEngine->ShutDownSocket(pBindParameter->dwSocketID);
			}
			else
			{
				//注销令牌
				LogoutToken(wServiceIndex, pBindParameter->wServiceID[wServiceIndex], dwTokenID);

				//关闭链接
				ShutdownServiceLink(m_ServiceAttribute[wServiceIndex].wServiceModule, wServiceID, dwTokenID);
			}			
		}

		return true;
	}		
	}

	return true;
}

//内部服务
bool CAttemperEngineSink::OnTCPSocketMainInsideService(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD *pdwTokenID, WORD wTokenCount)
{
	//服务信息
	WORD wServiceIndex = HICMMD(Command.wMainCmdID)-1;
	if (wServiceIndex >= CountArray(m_ServiceAttribute)) return true;
	
	//变量定义	
	DWORD dwTokenID = 0;	
	tagBindParameter * pBindParameter = NULL;

	//循环处理
	for (WORD i = 0; i < wTokenCount; i++)
	{
		//获取令牌
		dwTokenID = *pdwTokenID++;

		//校验令牌
		if (dwTokenID != 0)
		{
			pBindParameter = GetBindParameter(LOWORD(dwTokenID));
			if (pBindParameter->dwSocketID != dwTokenID) continue;
			if (pBindParameter->LinkState[wServiceIndex] == LS_NULL) continue;

			//发送信息			
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, Command.wMainCmdID, Command.wSubCmdID, pData, wDataSize, NULL, 0);
		}
		else
		{
			//变量定义
			WORD wBindIndex = 0;

			do
			{
				//获取参数
				pBindParameter = GetBindParameter(wBindIndex++);
				if (pBindParameter == NULL) break;
				if (pBindParameter->dwSocketID == 0) continue;
				if (pBindParameter->LinkState[wServiceIndex] != LS_LINKING) continue;

				//发送信息			
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, Command.wMainCmdID, Command.wSubCmdID, pData, wDataSize, NULL, 0);

			} while (true);		
		}
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

//发送消息
bool CAttemperEngineSink::SendDataToService(WORD wServiceIndex, WORD wServiceID, DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//枚举子项
	ITCPSocketService * pITCPSocketService = m_GlobalInfoManager.GetSocketService(wServiceIndex, wServiceID);
	if (pITCPSocketService == NULL)
	{
		pITCPSocketService = m_GlobalInfoManager.GetSocketService(wServiceIndex, 0);
	}

	//服务不可用
	if (pITCPSocketService == NULL)
	{
		//构造结构
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage, sizeof(SystemMessage));

		//变量定义						
		SystemMessage.wType = SMT_EJECT | SMT_CLOSE_LINK;
		_snwprintf_s(SystemMessage.szString,CountArray(SystemMessage.szString), L"%s不可用。请稍后再次重试！",m_ServiceAttribute[wServiceIndex].szServiceName);

		//打印消息
		CTraceService::TraceString(TEXT("ServiceID=%d,%s"),TraceLevel_Info, wServiceID, SystemMessage.szString);

		//发送消息
		WORD wPacketSize = sizeof(SystemMessage) - sizeof(SystemMessage.szString) + CountStringBuffer(SystemMessage.szString);
		m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(MDM_CM_SYSTEM, SERVICE_MODULE_GATE), SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wPacketSize, NULL,0);

		return false;
	}

	//发送数据
	if (pITCPSocketService != NULL)
	{
		pITCPSocketService->SendData(wMainCmdID, wSubCmdID, pData, wDataSize, dwSocketID);
	}

	return true;
}

//分配服务
WORD CAttemperEngineSink::AssignInsideService(WORD wServiceModule, DWORD dwSocketID)
{
	//获取对象
	auto wBindIndex = LOWORD(dwSocketID);
	auto pBindParameter = GetBindParameter(wBindIndex);
	if (pBindParameter == NULL) return INVALID_SERVICE_ID;

	//服务索引
	auto wServiceIndex = wServiceModule-1;
	if (wServiceIndex >=CountArray(m_ServiceAttribute)) return INVALID_SERVICE_ID;

	//校验次数
	auto pLinkRecord = &pBindParameter->LinkRecord[wServiceIndex];	
	if (pLinkRecord->wTimes>=CountArray(pLinkRecord->wServiceID)) return INVALID_SERVICE_ID;

	//指定服务
	if (pLinkRecord->wReqServiceID != INVALID_SERVICE_ID)
	{
		//查找服务
		if (m_GlobalInfoManager.SearchServiceInfo(wServiceIndex, pLinkRecord->wReqServiceID))
		{
			return pLinkRecord->wReqServiceID;
		}

		return INVALID_SERVICE_ID;
	}

	//变量定义
	WORD wAssignServiceID = INVALID_SERVICE_ID;

	switch (wServiceModule)
	{	
	case SERVICE_MODULE_GAME:		//游戏服务
	{
		//扩展信息
		auto pLinkExtend = (tagLinkExtend_Server*)pLinkRecord->cbLinkExtend;

		//变量定义
		DWORD dwMaxPlayer = 0;
		DWORD dwLeftCount = INVALID_DWORD;
		BYTE cbPlayerRatio[] = { 80,100 };
		WORD wFindServerType = pLinkExtend->wServerType & (GAME_GENRE_GOLD| GAME_GENRE_SCORE| GAME_GENRE_MATCH| GAME_GENRE_BATTLE| GAME_GENRE_RANKING);

		//分配房间
		for (int nIndex = 0; nIndex < CountArray(cbPlayerRatio); nIndex++)
		{
			//重置标识
			wAssignServiceID = INVALID_WORD;

			//变量定义			
			CGlobalServiceInfo* pGlobalServerItem = NULL;

			do
			{
				pGlobalServerItem = m_GlobalInfoManager.EnumServiceInfo(wServiceIndex, pGlobalServerItem);
				if (pGlobalServerItem == NULL) break;

				//房间配置
				auto pGameServer = (tagGameServer *)pGlobalServerItem->GetExtendData();
				WORD wServerType = pGameServer->wServerType& (GAME_GENRE_GOLD | GAME_GENRE_SCORE | GAME_GENRE_MATCH | GAME_GENRE_BATTLE | GAME_GENRE_RANKING);

				//房间判断
				if (pGameServer->wKindID == pLinkExtend->wKindID && pGameServer->wServerLevel == pLinkExtend->wServerLevel 
					&& (pGameServer->dwMasterRule&MR_FORBID_ENTER_ROOM)==0 && (wFindServerType & wServerType) != 0)
				{
					//服务过滤
					bool bServiceExits = false;
					for (auto j = 0; j < pLinkRecord->wTimes; j++)
					{
						if (pLinkRecord->wServiceID[j] == pGameServer->wServerID)
						{
							bServiceExits = true;
							break;
						}
					}

					//未曾连接
					if (bServiceExits == false)
					{
						//计算人数
						dwMaxPlayer = (WORD)(pGameServer->dwMaxPlayer / 100.f * cbPlayerRatio[nIndex]);

						//比较人数
						if ((dwMaxPlayer > pGameServer->dwOnLineCount) && (dwMaxPlayer - pGameServer->dwOnLineCount < dwLeftCount))
						{
							wAssignServiceID = pGameServer->wServerID;
							dwLeftCount = dwMaxPlayer - pGameServer->dwOnLineCount;
						}						
					}
				}

			} while (pGlobalServerItem!=NULL);

			//重置人数
			dwLeftCount = INVALID_DWORD;

			//退出判断
			if (wAssignServiceID != INVALID_WORD) break;
		}

		break;
	}
	default:
	{
		//服务数量
		INT nServiceCount = m_GlobalInfoManager.GetServiceCount(wServiceIndex);		
		if (nServiceCount == 0) break;

		//服务属性
		auto pServiceAttrib = &m_ServiceAttribute[wServiceIndex];

		//状态校验
		if (pServiceAttrib->bIsStateful==false)
		{
			wAssignServiceID = 0;
		}
		else
		{
			//计算标识
			DWORD dwObjectID = dwSocketID;

			//哈希映射
			if (pServiceAttrib->wServiceMapKind == MAP_KIND_HASH)
			{
				if (pBindParameter->dwUserID != 0) dwObjectID = pBindParameter->dwUserID;

				//映射服务
				auto  pGlobalServiceItem = m_GlobalInfoManager.MapServiceInfo(wServiceIndex, 0, dwObjectID);
				if (pGlobalServiceItem != NULL)
				{
					wAssignServiceID = pGlobalServiceItem->GetServiceID();
				}
			}

			//路由映射
			if (pServiceAttrib->wServiceMapKind == MAP_KIND_ROUTE)
			{
				auto pLinkExtend=(tagLinkExtend_Route*)pBindParameter->LinkRecord[wServiceIndex].cbLinkExtend;

				//映射服务
				auto  pGlobalServiceItem = m_GlobalInfoManager.MapServiceInfo(wServiceIndex, 0, pLinkExtend->dwRouteKeyID);
				if (pGlobalServiceItem != NULL)
				{
					wAssignServiceID = pGlobalServiceItem->GetServiceID();
				}
				else
				{
					//构造结构
					CMD_CS_C_QueryRouteItem QueryRouteItem = {};

					//设置变量
					QueryRouteItem.dwTokenID = dwSocketID;
					QueryRouteItem.dwRouteID = pLinkExtend->dwRouteKeyID;					
					QueryRouteItem.wServiceModule = pServiceAttrib->wServiceModule;

					//发送消息
					m_TCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE,SUB_CS_C_QUERY_ROUTE_ITEM,&QueryRouteItem,sizeof(QueryRouteItem),dwSocketID);
				}
			}							
		}	

		break;
	}
	}
	
	//链接记录
	if (wAssignServiceID != INVALID_SERVICE_ID)
	{
		pLinkRecord->wServiceID[pLinkRecord->wTimes++] = wAssignServiceID;
	}

	return wAssignServiceID;
}

//注销令牌
VOID CAttemperEngineSink::LogoutToken(WORD wServiceIndex, WORD wServiceID, DWORD dwTokenID)
{
	if (wServiceID != 0)
	{
		//构造结构
		CMD_CS_C_LogoutToken LogoutToken;
		ZeroMemory(&LogoutToken, sizeof(LogoutToken));

		//设置变量
		LogoutToken.dwTokenID = dwTokenID;

		//发送消息
		SendDataToService(wServiceIndex, wServiceID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_C_LOGOUT_TOKEN, &LogoutToken, sizeof(LogoutToken));
	}	
}

//关闭链接
void CAttemperEngineSink::ShutdownServiceLink(WORD wServiceModule, WORD wServiceID)
{
	//获取索引
	tagBindParameter * pBindParameter = NULL;
	WORD wServiceIndex = wServiceModule-1;

	//服务数量
	auto nServiceCount = m_GlobalInfoManager.GetServiceCount(wServiceModule);

	//枚举链接
	for (int i = 0; i < m_pServiceOption->wMaxConnect; i++)
	{
		pBindParameter = m_pBindParameter + i;
		if (pBindParameter->LinkState[wServiceIndex] == LS_NULL) continue;
		if (pBindParameter->wServiceID[wServiceIndex] != wServiceID && nServiceCount!=0) continue;

		//断开连接
		ShutdownServiceLink(wServiceModule, pBindParameter->wServiceID[wServiceIndex], pBindParameter->dwSocketID);
	}
}

//关闭链接
void CAttemperEngineSink::ShutdownServiceLink(WORD wServiceModule, WORD wServiceID, DWORD dwSoceketID)
{
	//变量定义		
	CMD_GG_S_LinkShutdown LinkShutdown;
	ZeroMemory(&LinkShutdown, sizeof(LinkShutdown));

	//构造结构
	LinkShutdown.wServiceID = wServiceID;
	LinkShutdown.wServiceModule = wServiceModule;

	//链接关闭
	m_pITCPNetworkEngine->SendData(dwSoceketID, MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_S_LINK_SHUTDOWN, &LinkShutdown, sizeof(LinkShutdown), NULL, 0);

	//获取参数
	WORD wBindIndex = LOWORD(dwSoceketID);
	tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);

	//获取索引
	WORD wServiceIndex = wServiceModule-1;

	//重置状态
	pBindParameter->LinkState[wServiceIndex] = LS_NULL;
	pBindParameter->wServiceID[wServiceIndex] = NONE_SERVICE_ID;	

	//枚举链接
	for (int i = 0; i < CountArray(pBindParameter->LinkState); i++)
	{
		if (pBindParameter->LinkState[i] != LS_NULL) return;
	}

	//关闭连接
	m_pITCPNetworkEngine->ShutDownSocket(dwSoceketID);
}

//时钟标识
DWORD CAttemperEngineSink::GetTimerID(WORD wServiceIndex, WORD wServiceID)
{
	//校验索引
	ASSERT (wServiceIndex < CountArray(m_ServiceAttribute));
	if (wServiceIndex >= CountArray(m_ServiceAttribute)) return 0;

	return MAKELONG(wServiceID, m_ServiceAttribute[wServiceIndex].wServiceModule);
}

//服务标识
DWORD CAttemperEngineSink::GetServiceID(WORD wServiceIndex, WORD wServiceID)
{
	//校验索引
	ASSERT(wServiceIndex < CountArray(m_ServiceAttribute));
	if (wServiceIndex >= CountArray(m_ServiceAttribute)) return 0;

	return MAKELONG(wServiceID, m_ServiceAttribute[wServiceIndex].wServiceModule);
}

//在线人数
WORD CAttemperEngineSink::GetOnlineCount()
{
	//变量定义
	WORD wBindIndex=0;
	WORD wOnlineCount=0;
	tagBindParameter * pBindParameter = NULL;

	do
	{
		//获取参数
		pBindParameter=GetBindParameter(wBindIndex++);
		if (pBindParameter == NULL) break;
		if (pBindParameter->dwSocketID == 0) continue;

		//统计人数
		wOnlineCount++;
	} while (true);

	return wOnlineCount;
}

//绑定参数
tagBindParameter * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//无效连接
	if (wBindIndex==INVALID_WORD) return NULL;

	//常规连接
	if (wBindIndex<m_pServiceOption->wMaxConnect)
	{
		return m_pBindParameter+wBindIndex;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

