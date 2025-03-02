#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_CONNECT_CENTER			1								//重连标识

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

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//设置变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
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
	case IDI_CONNECT_CENTER:	//连接中心
		{
			//变量定义
			tagAddressInfo * pAddressInfo=&m_pInitParameter->m_CenterAddress;

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
	//提取命令码
	WORD wMainCmdID = LOWORD(Command.wMainCmdID);

	//注册服务
	if (wMainCmdID == MDM_CS_REGISTER)
	{
		return OnTCPNetworkMainRegister(Command.wSubCmdID, pData, wDataSize, dwSocketID);
	}

	switch (wMainCmdID)
	{	
	case MDM_SS_STORE_SERVICE:		//存储服务
		{
			return OnTCPNetWorkMainStoreService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
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
	if (pBindParameter->wServiceModule==SERVICE_MODULE_GAME)
	{
		//查找房间
		auto pGlobalServerItem=m_GlobalItemManager.SearchServerItem(pBindParameter->wServiceID);
		if (pGlobalServerItem!=NULL)
		{
			//注销房间
			m_GlobalItemManager.DeleteServerItem(pBindParameter->wServiceID);					
		}
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
	if (dwServiceID ==NETWORK_CENTER)
	{
		//错误判断
		if (nErrorCode!=0)
		{
			//构造提示
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("中心服务器连接失败 [ %ld ]，%ld 秒后将重新连接"),
				nErrorCode,m_pInitParameter->m_wConnectTime);

			//提示消息
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}

		//变量定义
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService,sizeof(RegisterService));

		//构造结构
		RegisterService.wServiceID=m_pServiceOption->wServiceID;
		RegisterService.wServiceModule = SERVICE_MODULE_STORE;
		RegisterService.wServicePort=m_pServiceOption->wServicePort;
		RegisterService.wServiceKindID = m_pServiceOption->wServiceKindID;
		StringCchCopy(RegisterService.szServiceName, CountArray(RegisterService.szServiceName), TEXT("存储服务器"));
		StringCchCopy(RegisterService.szServiceDomain, CountArray(RegisterService.szServiceDomain), m_pServiceOption->szServiceDomain);

		//发送数据
		m_pITCPSocketCenter->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_SERVICE,&RegisterService,sizeof(RegisterService),0);

		return true;
	}

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenID)
{
	//中心连接
	if (dwServiceID ==NETWORK_CENTER)
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
	return true;
}

//管理服务
bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
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

//注册服务
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
  switch (wSubCmdID)
	{	
	case SUB_CS_C_REGISTER_SERVER:	//注册房间
		{
			//效验数据
			ASSERT (wDataSize==sizeof(CMD_CS_C_RegisterServer));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterServer)) return false;

			//消息定义
			CMD_CS_C_RegisterServer * pRegisterServer=(CMD_CS_C_RegisterServer *)pData;

			//查找房间
			if (m_GlobalItemManager.SearchServerItem(pRegisterServer->wServerID)!=NULL)
			{
				//变量定义
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//设置变量
				RegisterFailure.lErrorCode=0;
				StringCchCopy(RegisterFailure.szDescribeString,CountArray(RegisterFailure.szDescribeString), TEXT("已经存在相同标识的游戏房间服务，房间服务注册失败"));

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
			StringCchCopy(GameServer.szServerDomain, CountArray(GameServer.szServerDomain), pRegisterServer->szServerDomain);

			//注册房间
			m_GlobalItemManager.ActiveServerItem(dwSocketID,GameServer);

			//群发设置
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);

			//注册成功
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_SUCCESS,NULL,0);			

			return true;
		}	
	}

	return false;
}

//存储服务
bool CAttemperEngineSink::OnTCPNetWorkMainStoreService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_SS_C_STORE_VIDEO:	//存储录像
		{
			//校验参数
			ASSERT (wDataSize>sizeof(CMD_SS_C_StoreVideo));
			if (wDataSize<=sizeof(CMD_SS_C_StoreVideo)) return false;

			//投递请求			
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_STORE_VIDEO,0,dwSocketID,0,pData,wDataSize);

			return true;
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////
