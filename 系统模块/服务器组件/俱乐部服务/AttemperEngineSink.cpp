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
#define IDI_CONNECT_CENTER			1									//重连标识
#define IDI_CONNECT_BATTLE			2									//连接时间
#define IDI_SERVICE_PULSE			3									//服务脉冲
#define IDI_DETECTION_TABLE			4									//检测桌子

//////////////////////////////////////////////////////////////////////////
//位置掩码
#define LOCATION_MASK_INCLUB		0x01								//馆内
#define LOCATION_MASK_OUTCLUB		0x02								//馆外

//常量定义
#define FREE_TABLE_COUNT			1									//桌子数量

//////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//设置变量		
	m_bClubSevice=false;
	m_bNeedConnStatus=true;
	m_pClubOption=NULL;
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;		
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));	
	ZeroMemory(&m_BattleItem,sizeof(m_BattleItem));

	//组件变量
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;
	m_pITCPSocketBattle=NULL;

	//调试变量
	m_MapInvokeCount1.InitHashTable(33);
	m_MapInvokeCount2.InitHashTable(33);

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
	m_pBindParameter=new tagBindParameter[m_pClubOption->wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pClubOption->wMaxConnect);

	//设置状态
	m_bClubSevice=true;

	//调试变量
	m_MapInvokeCount1.RemoveAll();
	m_MapInvokeCount2.RemoveAll();

	//设置接口
	m_GlobalServiceManager.SetIAttemperEngine(m_pIAttemperEngine);

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
	m_pITCPSocketBattle = NULL;
	ZeroMemory(&m_BattleItem, sizeof(m_BattleItem));

	//调试变量
	m_MapInvokeCount1.RemoveAll();
	m_MapInvokeCount2.RemoveAll();

	//设置状态
	m_bClubSevice=false;
	m_bNeedConnStatus=true;

	//删除数据
	SafeDeleteArray(m_pBindParameter);

	//设置组件	
	m_ClubUserManager.DeleteUserItem();
	m_ClubInfoManager.RemoveClubItem();

	//重置数据
	m_GlobalItemManager.ResetData();
	m_GlobalServiceManager.ResetData();

	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case CT_CONNECT_BATTLE:			//连接约战
	{
		//发起连接
		m_pITCPSocketBattle->Connect(m_BattleItem.szServiceDomain, m_BattleItem.wServicePort);

		//提示消息
		CTraceService::TraceString(TEXT("正在尝试连接约战服务器..."), TraceLevel_Normal);

		return true;
	}
	case CT_CONNECT_CENTER:		//连接中心
	{
		//关闭时间
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->KillTimer(IDI_CONNECT_CENTER);

		//获取地址					
		tagAddressInfo * pAddressInfo=&m_pInitParameter->m_CenterAddress;

		//发起连接
		m_pITCPSocketCenter->Connect(htonl(pAddressInfo->dwServiceAddr), m_pInitParameter->m_wCenterPort);

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
	case IDI_CONNECT_BATTLE:	//连接约战
	{
		//发起连接
		m_pITCPSocketBattle->Connect(m_BattleItem.szServiceDomain, m_BattleItem.wServicePort);

		//提示消息
		CTraceService::TraceString(TEXT("正在尝试连接%s..."), TraceLevel_Normal, m_BattleItem.szServiceName);

		return true;
	}
	case IDI_CONNECT_CENTER:	//连接中心
	{
		//获取地址					
		auto pAddressInfo=&m_pInitParameter->m_CenterAddress;

		//发起连接
		m_pITCPSocketCenter->Connect(htonl(pAddressInfo->dwServiceAddr), m_pInitParameter->m_wCenterPort);

		//提示消息
		CTraceService::TraceString(TEXT("正在尝试连接中心服务器..."),TraceLevel_Normal);

		return true;
	}
	case IDI_SERVICE_PULSE:		//服务脉冲
	{
		//获取时间
		SYSTEMTIME SystemTime;
		GetSystemTime(&SystemTime);

		//当前时间
		DWORD dwCurrentTime = (DWORD)time(NULL);

		//凌晨3点-5点检查
		if (SystemTime.wHour >= 3 && SystemTime.wHour <= 5)
		{
			//变量定义			
			CClubInfoItem* pClubInfoItem = NULL;

			do
			{
				//枚举茶社
				auto pLastClubInfoItem = pClubInfoItem;
				pClubInfoItem = m_ClubInfoManager.EnumClubItem(pLastClubInfoItem);

				//校验茶社
				if (pClubInfoItem != NULL)
				{
					//卸载处理
					if (pClubInfoItem->GetClubState() == CLUB_STATE_ONLINE && pClubInfoItem->GetOnlineCount() == 0 && (dwCurrentTime - pClubInfoItem->GetActiveTime() >= ONE_WEEK || pClubInfoItem->GetUnloadFlag()))
					{
						//构造结构
						CMD_CS_C_UpdateRouteItem UpdateRouteItem = {};

						//设置变量
						UpdateRouteItem.dwRouteID = pClubInfoItem->GetClubID();
						UpdateRouteItem.cbRouteState = ROUTE_STATE_NULL;

						//发送消息
						m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_UPDATE_ROUTE_ITEM, &UpdateRouteItem, sizeof(UpdateRouteItem), 0);

						//移除群组
						m_ClubInfoManager.RemoveClubItem(pClubInfoItem->GetClubID());
					}
				}

			} while (pClubInfoItem != NULL);
		}
	}
	case IDI_DETECTION_TABLE:	//检测桌子
	{
		//变量定义			
		CClubInfoItem* pClubInfoItem = NULL;

		do
		{
			//枚举茶社
			auto pLastClubInfoItem = pClubInfoItem;
			pClubInfoItem = m_ClubInfoManager.EnumClubItem(pLastClubInfoItem);

			//校验茶社
			if (pClubInfoItem != NULL)
			{
				//卸载处理
				if (pClubInfoItem->GetClubState() == CLUB_STATE_ONLINE)
				{
					//定义索引
					INT nIndex = 0;

					do
					{
						auto pBattleConfig = pClubInfoItem->EnumConfig(nIndex++);
						if (pBattleConfig == NULL) break;

						//校验桌子
						while (pBattleConfig->wTableCount < FREE_TABLE_COUNT)
						{
							//更新桌数
							pBattleConfig->wTableCount++;

							CTraceService::TraceString(TEXT("创建桌子1"),TraceLevel_Debug);

							//创建桌子
							CreateBattleTable(pClubInfoItem, pBattleConfig);
						}

					} while (true);
				}
			}

		} while (pClubInfoItem != NULL);		

		return true;
	}	
	}

	//服务信息
	WORD wServiceID = LOWORD(dwTimerID);
	WORD wServiceModule = HIWORD(dwTimerID);

	//约战服务
	if (wServiceModule == SERVICE_MODULE_BATTLE)
	{
		auto pGlobalServiceItem = m_GlobalServiceManager.SearchBattleService(wServiceID);
		if (pGlobalServiceItem != NULL && pGlobalServiceItem->m_pITCPSocketService != NULL)
		{
			//连接服务
			pGlobalServiceItem->m_pITCPSocketService->Connect(pGlobalServiceItem->m_ServiceItem.szServiceDomain, pGlobalServiceItem->m_ServiceItem.wServicePort);

			//提示消息
			CTraceService::TraceString(TEXT("正在尝试连接[%d]号约战服务器..."), TraceLevel_Normal, wServiceID);
		}

		return true;
	}	

	return false;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//获取索引
	ASSERT(LOWORD(dwSocketID)<m_pClubOption->wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pClubOption->wMaxConnect) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	auto pBindParameter=(m_pBindParameter+wBindIndex);

	//设置变量
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientIP=dwClientIP;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);	

	//服务信息
	pBindParameter->wServiceID = 0;
	pBindParameter->wServiceModule = 0;
	pBindParameter->pBindServiceItem = NULL;

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	DWORD dwCmdKey=MAKELONG(Command.wMainCmdID,Command.wSubCmdID);
	m_MapInvokeCount1[dwCmdKey]++;

	//提取命令码
	WORD wMainCmdID = LOCMMD(Command.wMainCmdID);

	//注册命令
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
	case MDM_CLUB_SERVICE:		//茶社服务
		{
			OnTCPNetworkMainClubService(Command.wSubCmdID,pData,wDataSize,dwSocketID,dwTokenID);

			return true;
		}
	}

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	auto pBindParameter=(m_pBindParameter+wBindIndex);

	//网关服务
	if (pBindParameter->wServiceModule==SERVICE_MODULE_GATE)
	{
		m_GlobalItemManager.DeleteGateItem(pBindParameter->wServiceID);							
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
	case DBO_GR_CLUB_LIST:			//茶社列表
	{
		return OnDBClubList(dwContextID, dwTokenID,pData,wDataSize);
	}
	case DBO_GR_CLUB_LIST_FINISH:	//列表完成
	{
		return OnDBClubListFinish(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBO_GR_CLUB_ITEM:			//茶社子项
	{
		return OnDBClubItem(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBO_GR_CLUB_ITEM_FINISH:	//子项完成
	{
		return OnDBClubItemFinish(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBO_GR_CLUB_MEMBER:		//茶社成员
	{
		return OnDBClubMember(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBO_GR_BATTLE_CONFIG:		//约战玩法
	{
		return OnDBBattleConfig(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBO_GR_KIND_SETTING:		//类型设置
	{
		return OnDBKindSetting(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_MODIFY_SETTING:		//修改设置
	{
		return OnDBKindSettingModify(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_APPLY_MESSAGE:		//申请消息
	{
		return OnDBApplyMessage(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_UPDATE_MEMBER:		//更新成员
	{
		return OnDBUpdateMember(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBO_GR_APPEND_CONFIG:		//添加玩法
	{
		return OnDBAppendConfig(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_MODIFY_CONFIG:		//修改玩法
	{
		return OnDBModifyConfig(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_DELETE_CONFIG:		//删除玩法
	{
		return OnDBDeleteConfig(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_OPERATE_FAILURE:	//操作失败
	{
		return OnDBOperateFailure(dwContextID, dwTokenID, pData, wDataSize);
	}
	}

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason)
{
	if (dwServiceID==NETWORK_CENTER)
	{
		//构造提示
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("与中心服务器的连接关闭了，%ld 秒后将重新连接"),m_pInitParameter->m_wConnectCenter);

		//提示消息
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);

		//设置时间
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectCenter*1000L,1,0);

		return true;
	}

	//约战服务
	if (dwServiceID == NETWORK_BATTLE)
	{
		//构造提示
		TCHAR szDescribe[128] = TEXT("");
		_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("与约战服务器的连接关闭了，%ld 秒后将重新连接"), m_pInitParameter->m_wConnectBattle);

		//提示消息
		CTraceService::TraceString(szDescribe, TraceLevel_Warning);

		//关闭时钟
		ASSERT(m_pITimerEngine != NULL);
		m_pITimerEngine->KillTimer(IDI_DETECTION_TABLE);

		//设置时间
		ASSERT(m_pITimerEngine != NULL);
		m_pITimerEngine->SetTimer(IDI_CONNECT_BATTLE, m_pInitParameter->m_wConnectBattle * 1000L, 1, 0);		

		return true;
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
			//构造提示
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("中心服务器连接失败 [ %ld ]，%ld 秒后将重新连接"),
				nErrorCode,m_pInitParameter->m_wConnectCenter);

			//提示消息
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectCenter*1000L,1,0);

			return false;
		}

		//变量定义
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService,sizeof(RegisterService));

		//构造结构
		RegisterService.wServiceID=m_pClubOption->wServiceID;
		RegisterService.wServiceModule=SERVICE_MODULE_CLUB;
		RegisterService.wServiceMask=SERVICE_MASK_BATTLE;
		RegisterService.wServicePort=m_pClubOption->wServicePort;
		StringCchCopy(RegisterService.szServiceName, CountArray(RegisterService.szServiceName), TEXT("俱乐部服务"));
		StringCchCopy(RegisterService.szServiceDomain, CountArray(RegisterService.szServiceDomain),m_pClubOption->szServiceDomain);

		//发送数据
		m_pITCPSocketCenter->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_SERVICE,&RegisterService,sizeof(RegisterService),0);

		return true;
	}

	//约战连接
	if (dwServiceID == NETWORK_BATTLE)
	{
		//错误判断
		if (nErrorCode != 0)
		{
			//提示消息
			CTraceService::TraceString(TEXT("约战服务器连接失败 [ %ld ]，%ld 秒后将重新连接"), TraceLevel_Warning, nErrorCode, m_pInitParameter->m_wConnectBattle);

			//设置时间
			ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_BATTLE, m_pInitParameter->m_wConnectBattle * 1000L, 1, 0);

			return false;
		}

		//变量定义
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService, sizeof(RegisterService));

		//设置变量	
		RegisterService.wServiceID = m_pClubOption->wServiceID;
		RegisterService.wServiceModule = SERVICE_MODULE_CLUB;
		RegisterService.wServicePort = m_pClubOption->wServicePort;
		RegisterService.wMaxConnect = m_pClubOption->wMaxConnect;
		RegisterService.wServiceKindID = m_pClubOption->wServiceKindID;
		StringCchCopy(RegisterService.szServiceDomain, CountArray(RegisterService.szServiceDomain), m_pClubOption->szServiceDomain);

		//发送数据
		m_pITCPSocketBattle->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_SERVICE, &RegisterService, sizeof(RegisterService), 0);
	}

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD *pdwTokenID, WORD wTokenCount)
{
	//中心连接
	if (dwServiceID == NETWORK_CENTER)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:			//注册服务
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
		}
		case MDM_CS_USER_COLLECT:		//用户汇总
		{
			return OnTCPSocketMainUserCollect(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_CS_COMMON_SERVICE:		//服务信息
		{
			return OnTCPSocketMainComonService(Command.wSubCmdID,pData,wDataSize);
		}
		case MDM_CS_MANAGER_SERVICE:	//管理服务
		{
			return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
		}
		case MDM_CS_FORWARD_SERVICE:	//转发服务
		{
			return OnTCPSocketMainForwardService(Command.wSubCmdID, pData, wDataSize);
		}
		}

		return true;
	}
	
	//约战服务
	if (dwServiceID == NETWORK_BATTLE)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:		//注册服务
		{
			return OnTCPSocketBattleMainRegister(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_CS_COMMON_SERVICE:	//通用服务
		{
			return OnTCPSocketBattleMainCommonService(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_BC_BATTLE_SERVICE:	//服务命令	
		{
			return OnTCPSocketBattleMainBattleService(Command.wSubCmdID, pData, wDataSize);
		}
		}

		return true;
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

			//汇总用户
			m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_COLLECT_USER, NULL, 0, 0);

			//设置时间
			ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_SERVICE_PULSE, 60*1000L, -1, 0);						

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

//用户汇总
bool CAttemperEngineSink::OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_USER_LIST:		//用户列表
	{
		//校验参数
		ASSERT(wDataSize % sizeof(tagGlobalUserInfo) == 0);
		if (wDataSize % sizeof(tagGlobalUserInfo) != 0) return true;

		//提取数据
		WORD wUserCount = wDataSize / sizeof(tagGlobalUserInfo);
		tagGlobalUserInfo* pUserItem = (tagGlobalUserInfo*)pData;

		//变量定义
		tagClubUserInfo UserInfo;
		ZeroMemory(&UserInfo, sizeof(UserInfo));

		//变量定义
		CClubUserItem* pClubUserItem = NULL;

		//插入用户
		for (WORD i = 0; i < wUserCount; i++)
		{
			//校验状态
			if (pUserItem->cbCBUserStatus == US_CB_OFFLINE) continue;

			//查找用户
			pClubUserItem = m_ClubUserManager.SearchUserItem(pUserItem->dwUserID);
			if (pClubUserItem != NULL)
			{
				pClubUserItem->SetSocketInfo(pUserItem->SocketInfo);
				pClubUserItem->SetUserStatus(US_CB_ONLINE);
			}
			else
			{
				//构造用户
				UserInfo.dwUserID = pUserItem->dwUserID;
				UserInfo.cbUserStatus = US_CB_ONLINE;
				UserInfo.dwGameID = pUserItem->dwGameID;
				UserInfo.cbGender = pUserItem->cbGender;
				StringCchCopy(UserInfo.szNickName, CountArray(UserInfo.szNickName), pUserItem->szNickName);
				StringCchCopy(UserInfo.szUserHeadUrl, CountArray(UserInfo.szUserHeadUrl), pUserItem->szUserHeadUrl);//kk jia

				//头像信息
				UserInfo.wFaceID = pUserItem->wFaceID;
				UserInfo.dwCustomID = pUserItem->dwCustomID;

				//插入用户
				tagSocketInfo SocketInfo = {};
				/*CString s;
				s.Format(_T("插入用户1 szNickName:%s,szUserHeadUrl:%s"), pUserItem->szNickName, pUserItem->szUserHeadUrl);
				CTraceService::TraceString(s, TraceLevel_Exception);*/

				m_ClubUserManager.InsertUserItem(UserInfo, pUserItem->SocketInfo);
			}

			pUserItem++;
		}

		return true;
	}
	case SUB_CS_S_COLLECT_FINISH:	//列表完成
	{
		//缓冲定义
		BYTE cbBuffer[SOCKET_BUFFER];
		CClubInfoItem* pClubInfoItem = NULL;

		//变量定义
		tagRouteItem* pRouteItem = NULL;
		auto wPacketSize = sizeof(CMD_CS_C_AppendRouteItem);
		auto pAppendRouteItem = (CMD_CS_C_AppendRouteItem*)cbBuffer;

		//设置变量			
		pAppendRouteItem->bTheLast = false;
		pAppendRouteItem->wServiceKind = m_pClubOption->wServiceKindID;
		pAppendRouteItem->wServiceModule = SERVICE_MODULE_CLUB;

		do
		{
			//枚举茶社
			auto pLastClubInfoItem = pClubInfoItem;
			pClubInfoItem = m_ClubInfoManager.EnumClubItem(pLastClubInfoItem);

			//校验茶社
			if (pClubInfoItem != NULL && pClubInfoItem->GetClubState()!=CLUB_STATE_OFFLINE)
			{
				if (wPacketSize + sizeof(tagRouteItem) > sizeof(cbBuffer))
				{
					//发送消息					
					m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_APPEND_ROUTE_ITEM, cbBuffer, wPacketSize, 0);

					//重置大小		
					wPacketSize = sizeof(CMD_CS_C_AppendRouteItem);
				}

				//获取对象
				pRouteItem = (tagRouteItem*)&cbBuffer[wPacketSize];

				//路由信息
				pRouteItem->dwRouteID = pClubInfoItem->GetClubID();
				pRouteItem->cbRouteState = pClubInfoItem->GetClubState()==CLUB_STATE_ONLINE?ROUTE_STATE_SERVICE:ROUTE_STATE_ACTIVE;

				//增加大小
				wPacketSize += sizeof(tagRouteItem);
			}

		} while (pClubInfoItem != NULL);

		//剩余发送
		if (wPacketSize > sizeof(CMD_CS_C_AppendRouteItem))
		{
			//设置变量
			pAppendRouteItem->bTheLast = true;	

			//发送消息					
			m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_APPEND_ROUTE_ITEM, cbBuffer, wPacketSize, 0);
		}

		return true;
	}
	}

	return true;
}

//列表事件
bool CAttemperEngineSink::OnTCPSocketMainComonService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{	
	case SUB_CS_S_SERVICE_INSERT:	//服务插入
	{
		//效验参数
		ASSERT(wDataSize % sizeof(tagServiceItem) == 0);
		if (wDataSize % sizeof(tagServiceItem) != 0) return false;

		//变量定义
		WORD wItemCount = wDataSize / sizeof(tagServiceItem);
		tagServiceItem* pServiceItem = (tagServiceItem*)pData;

		//约战服务
		if (pServiceItem->wServiceModule == SERVICE_MODULE_BATTLE)
		{
			CopyMemory(&m_BattleItem, pServiceItem, sizeof(m_BattleItem));			
		}				

		return true;
	}
	case SUB_CS_S_SERVICE_REMOVE:	//状态移除
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_CS_S_ServiceRemove));
		if (wDataSize != sizeof(CMD_CS_S_ServiceRemove)) return false;

		//提取数据
		auto pServiceRemove = (CMD_CS_S_ServiceRemove*)pData;

		//约战移除
		if (pServiceRemove->wServiceModule == SERVICE_MODULE_BATTLE)
		{
			ZeroMemory(&m_BattleItem, sizeof(m_BattleItem));
		}		

		return true;
	}
	case SUB_CS_S_ACTIVE_ROUTE_ITEM: //激活路由
	{
		//效验参数
		ASSERT (wDataSize == sizeof(CMD_CS_S_ActiveRouteItem));
		if (wDataSize != sizeof(CMD_CS_S_ActiveRouteItem)) return false;

		//提取数据
		auto pActiveRouteItem = (CMD_CS_S_ActiveRouteItem*)pData;

		//查找茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pActiveRouteItem->dwRouteID);
		if (pClubInfoItem != NULL)
		{
			//在线状态或即将下线
			if (pClubInfoItem->GetClubState() >= CLUB_STATE_ONLINE)
			{
				//构造结构
				CMD_CS_C_UpdateRouteItem UpdateRouteItem = {};

				//设置变量
				UpdateRouteItem.cbRouteState = ROUTE_STATE_SERVICE;
				UpdateRouteItem.dwRouteID = pClubInfoItem->GetClubID();

				//发送消息
				m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_UPDATE_ROUTE_ITEM, &UpdateRouteItem, sizeof(UpdateRouteItem), 0);				
			}
		}
		else
		{
			//构造对象
			tagClubItem ClubItem = {};
			ClubItem.ClubProperty.dwClubID = pActiveRouteItem->dwRouteID;

			//创建茶社
			pClubInfoItem = m_ClubInfoManager.ActiveClubItem(ClubItem);
			if (pClubInfoItem != NULL)
			{
				//设置状态
				pClubInfoItem->SetClubState(CLUB_STATE_LOADING);

				//加载茶社		
				DBR_GR_LoadClubItem LoadClubItem;
				ZeroMemory(&LoadClubItem, sizeof(LoadClubItem));

				//构造结构
				LoadClubItem.bActiveRoute = true;
				LoadClubItem.dwClubID = pActiveRouteItem->dwRouteID;				

				//加载成员
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_CLUB_ITEM, LoadClubItem.dwClubID, 0, 0, &LoadClubItem, sizeof(LoadClubItem));
			}
		}		

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

			//停止服务
			if (pCommandCode->dwCommandCode&CMD_STOP_CLUB_SERVICE)
			{
				//设置标识
				m_bClubSevice=false;
			}

			//启动服务
			if (pCommandCode->dwCommandCode&CMD_START_CLUB_SERVICE)
			{
				//设置标识
				m_bClubSevice=true;
			}

			//重载茶社
			if (pCommandCode->dwCommandCode & CMD_RELOAD_CLUB_DATA)
			{
				//变量定义
				CClubInfoItem* pClubInfoItem = NULL;

				do
				{
					//枚举茶社
					auto pLastClubInfoItem = pClubInfoItem;
					pClubInfoItem = m_ClubInfoManager.EnumClubItem(pLastClubInfoItem);

					//校验状态
					if (pClubInfoItem != NULL && pClubInfoItem->GetClubState() == CLUB_STATE_ONLINE)
					{
						//设置标志
						pClubInfoItem->SetUnloadFlag(true);						
					}

				} while (pClubInfoItem != NULL);
			}

			//调试命令
			if (pCommandCode->dwCommandCode&CMD_PRINT_CLUB_INFO)
			{
				//变量定义
				DWORD dwOnlineCount=0;
				DWORD dwTotalCount=m_ClubUserManager.GetUserItemCount();

				//变量定义
				DWORD dwEnumIndex=0;
				CClubUserItem * pClubUserItem=NULL;
				
				do
				{
					pClubUserItem=m_ClubUserManager.EnumUserItem(pClubUserItem);
					if (pClubUserItem==NULL) break;
					if (pClubUserItem->GetUserStatus()==US_CB_ONLINE) dwOnlineCount++;
				} while (true);

				CString strMessage;
				strMessage.Format(TEXT("总人数：[%d] 当前在线人数：[%d]"),dwTotalCount,dwOnlineCount);
				CTraceService::TraceString(strMessage,TraceLevel_Warning);

				//删除用户
				DWORD dwCmdKey=0L;
				ULONGLONG ulInvokeCount=0;
				POSITION Position=m_MapInvokeCount1.GetStartPosition();
				while (Position!=NULL)
				{
					m_MapInvokeCount1.GetNextAssoc(Position,dwCmdKey,ulInvokeCount);					
					strMessage.Format(TEXT("Network: MCmdID[%d] SCmdID[%d] InvokeCount[%ld]"),LOWORD(dwCmdKey),HIWORD(dwCmdKey),ulInvokeCount);
					CTraceService::TraceString(strMessage,TraceLevel_Warning);
				}

				Position=m_MapInvokeCount2.GetStartPosition();
				while (Position!=NULL)
				{
					m_MapInvokeCount2.GetNextAssoc(Position,dwCmdKey,ulInvokeCount);					
					strMessage.Format(TEXT("Socket: MCmdID[%d] SCmdID[%d] InvokeCount[%ld]"),LOWORD(dwCmdKey),HIWORD(dwCmdKey),ulInvokeCount);
					CTraceService::TraceString(strMessage,TraceLevel_Warning);
				}
			}

			return true;
		}
	}

	return true;
}

//转发消息
bool CAttemperEngineSink::OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_CB_USER_ONLINE:			//用户上线
	{
		//参数校验
		ASSERT(wDataSize == sizeof(tagGlobalUserInfo));
		if (wDataSize != sizeof(tagGlobalUserInfo)) return false;

		//提取数据
		auto pGlobalUserInfo = (tagGlobalUserInfo*)pData;

		//查找对象
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pGlobalUserInfo->dwUserID);
		if (pClubUserItem != NULL)
		{
			//用户信息
			auto pClubUserInfo = pClubUserItem->GetUserInfo();

			//设置变量
			pClubUserInfo->wFaceID = pGlobalUserInfo->wFaceID;
			pClubUserInfo->dwCustomID = pGlobalUserInfo->dwCustomID;
			pClubUserInfo->cbUserStatus = pGlobalUserInfo->cbCBUserStatus;
			StringCchCopy(pClubUserInfo->szNickName, CountArray(pClubUserInfo->szNickName), pGlobalUserInfo->szNickName);

			//设置网络				
			pClubUserItem->SetSocketInfo(pGlobalUserInfo->SocketInfo);
		}
		else
		{
			//构造结构
			tagClubUserInfo ClubUserInfo = {};

			//设置变量
			ClubUserInfo.dwUserID = pGlobalUserInfo->dwUserID;
			ClubUserInfo.dwGameID = pGlobalUserInfo->dwGameID;
			ClubUserInfo.cbGender = pGlobalUserInfo->cbGender;
			ClubUserInfo.wFaceID = pGlobalUserInfo->wFaceID;
			ClubUserInfo.dwCustomID = pGlobalUserInfo->dwCustomID;
			ClubUserInfo.cbUserStatus = pGlobalUserInfo->cbCBUserStatus;
			StringCchCopy(ClubUserInfo.szNickName, CountArray(ClubUserInfo.szNickName), pGlobalUserInfo->szNickName);
			StringCchCopy(ClubUserInfo.szUserHeadUrl, CountArray(ClubUserInfo.szUserHeadUrl), pGlobalUserInfo->szUserHeadUrl);//kk jia

			CString s;
			s.Format(_T("插入用户2 szNickName:%s,szUserHeadUrl:%s"), pGlobalUserInfo->szNickName, pGlobalUserInfo->szUserHeadUrl);
			CTraceService::TraceString(s, TraceLevel_Exception);
			//插入用户
			m_ClubUserManager.InsertUserItem(ClubUserInfo, pGlobalUserInfo->SocketInfo);
		}

		return true;
	}
	case SUB_CS_CB_USER_OFFLINE:		//用户下线
	{
		//参数校验
		ASSERT(wDataSize == sizeof(CMD_CS_CB_UserOffline));
		if (wDataSize != sizeof(CMD_CS_CB_UserOffline)) return false;

		//提取数据
		auto pUserOffline = (CMD_CS_CB_UserOffline*)pData;

		//查找对象
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pUserOffline->dwUserID);
		if (pClubUserItem != NULL)
		{
			//设置状态
			pClubUserItem->SetUserStatus(US_CB_OFFLINE);

			//重置网络
			ZeroMemory(pClubUserItem->GetSocketInfo(),sizeof(tagSocketInfo));
		}	

		return true;
	}	
	case SUB_CS_CB_USER_UPDATE:			//用户更新
	{
		//参数校验
		ASSERT(wDataSize == sizeof(tagGlobalUserInfo));
		if (wDataSize != sizeof(tagGlobalUserInfo)) return false;

		//提取数据
		auto pGlobalUserInfo = (tagGlobalUserInfo*)pData;

		//查找对象
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pGlobalUserInfo->dwUserID);
		if (pClubUserItem != NULL)
		{
			//用户信息
			auto pClubUserInfo = pClubUserItem->GetUserInfo();

			//更新信息
			pClubUserInfo->wFaceID = pGlobalUserInfo->wFaceID;
			pClubUserInfo->dwCustomID = pGlobalUserInfo->dwCustomID;						
			StringCchCopy(pClubUserInfo->szNickName, CountArray(pClubUserInfo->szNickName), pGlobalUserInfo->szNickName);			
		}

		return true;
	}
	case SUB_CS_CB_UPDATE_CLUB:			//修改茶社
	{
		//提取数据
		ASSERT(wDataSize >= sizeof(CMD_CS_CB_UpdateClub));
		if (wDataSize < sizeof(CMD_CS_CB_UpdateClub)) return false;

		//提取数据
		auto pUpdateClub = (CMD_CS_CB_UpdateClub*)pData;

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pUpdateClub->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//构造结构
			CMD_CB_S_ClubUpdate* pClubUpdate = (CMD_CB_S_ClubUpdate*)m_cbBuffer;

			//设置变量
			pClubUpdate->dwClubID = pUpdateClub->dwClubID;
			pClubUpdate->cbUpdateKind = pUpdateClub->cbUpdateKind;

			//更新规则
			if (pUpdateClub->cbUpdateKind == UPCLUB_KIND_RULES)
			{
				//保存规则
				tagClubRules ClubRules;
				CopyMemory(&ClubRules, pClubInfoItem->GetClubRules(), sizeof(ClubRules));

				//拷贝数据
				CopyMemory(pClubUpdate + 1, pUpdateClub + 1, sizeof(tagClubRules));

				//更新数据
				CopyMemory(pClubInfoItem->GetClubRules(), pUpdateClub + 1, sizeof(tagClubRules));

				//发送数据
				WORD wPacketSize = sizeof(CMD_CB_S_ClubUpdate) + sizeof(tagClubRules);
				SendDataToMember(pUpdateClub->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_UPDATE, pClubUpdate, wPacketSize, LOCATION_MASK_INCLUB);

				//校验隐私开关
				if ((ClubRules.wSwitchOption & (SWITCH_OPTION_PRIVATE | SWITCH_OPTION_SHUTUP)) != (pClubInfoItem->GetClubRules()->wSwitchOption & (SWITCH_OPTION_PRIVATE | SWITCH_OPTION_SHUTUP)))
				{
					//获取对象
					CMD_BC_C_ClubUpdate* pFWClubUpdate=(CMD_BC_C_ClubUpdate*)m_cbBuffer;

					//设置变量
					pFWClubUpdate->dwClubID = pUpdateClub->dwClubID;
					pFWClubUpdate->cbUpdateKind = pUpdateClub->cbUpdateKind;
					CopyMemory(pFWClubUpdate + 1, pClubInfoItem->GetClubRules(), sizeof(tagClubRules));

					//删除通知
					WORD wPacketSize = sizeof(CMD_BC_C_ClubUpdate) + sizeof(tagClubRules);
					m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_CLUB_UPDATE, pFWClubUpdate, wPacketSize,0);
				}
			}

			//更新公告
			if (pUpdateClub->cbUpdateKind == UPCLUB_KIND_NOTICE)
			{
				//保存规则
				tagClubNotice ClubNotice;
				CopyMemory(&ClubNotice, pClubInfoItem->GetClubNotice(), sizeof(ClubNotice));

				//拷贝数据
				CopyMemory(pClubUpdate + 1, pUpdateClub + 1, sizeof(tagClubNotice));

				//更新数据
				CopyMemory(pClubInfoItem->GetClubNotice(), pUpdateClub + 1, sizeof(tagClubNotice));

				//发送数据
				WORD wPacketSize = sizeof(CMD_CB_S_ClubUpdate) + sizeof(tagClubNotice);
				SendDataToMember(pUpdateClub->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_UPDATE, pClubUpdate, wPacketSize, LOCATION_MASK_INCLUB);				
			}

			//更新属性
			if (pUpdateClub->cbUpdateKind == UPCLUB_KIND_PROPERTY)
			{
				//拷贝数据
				CopyMemory(pClubUpdate + 1, pUpdateClub + 1, sizeof(tagClubProperty));

				//更新数据
				CopyMemory(pClubInfoItem->GetClubProperty(), pUpdateClub + 1, sizeof(tagClubProperty));

				//发送数据
				WORD wPacketSize = sizeof(CMD_CB_S_ClubUpdate) + sizeof(tagClubProperty);
				SendDataToMember(pUpdateClub->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_UPDATE, pClubUpdate, wPacketSize, LOCATION_MASK_INCLUB);

				//构造结构
				tagClubNameCard ClubNameCard;
				ZeroMemory(&ClubNameCard, sizeof(ClubNameCard));

				//构造群名片
				ConstructClubNameCard(pClubInfoItem->GetClubProperty(), &ClubNameCard);

				//更新名片
				SendDataToMember(pUpdateClub->dwClubID, SERVICE_MODULE_CLUBPLAZA, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_NAMECARD, &ClubNameCard, sizeof(ClubNameCard), LOCATION_MASK_INCLUB | LOCATION_MASK_OUTCLUB);
			}
		}

		return true;
	}
	case SUB_CS_CB_DELETE_CLUB:			//删除茶社
	{
		//提取数据
		ASSERT (wDataSize == sizeof(CMD_CS_CB_DeleteClub));
		if (wDataSize != sizeof(CMD_CS_CB_DeleteClub)) return false;

		//提取数据
		auto pDeleteClub = (CMD_CS_CB_DeleteClub*)pData;

		//删除茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pDeleteClub->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//茶馆属性
			tagClubProperty* pClubProperty = pClubInfoItem->GetClubProperty();

			//构造结构
			CMD_CS_C_UpdateRouteItem UpdateRouteItem = {};

			//设置变量
			UpdateRouteItem.dwRouteID = pClubInfoItem->GetClubID();
			UpdateRouteItem.cbRouteState = ROUTE_STATE_NULL;

			//发送消息
			m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_UPDATE_ROUTE_ITEM, &UpdateRouteItem, sizeof(UpdateRouteItem), 0);

			//构造结构
			CMD_CP_S_ClubDelete ClubDelete;
			ZeroMemory(&ClubDelete, sizeof(ClubDelete));

			//拷贝数据
			ClubDelete.dwClubID = pDeleteClub->dwClubID;
			StringCchCopy(ClubDelete.szNotifyContent, CountArray(ClubDelete.szNotifyContent), CWHService::FormatMessageV(MIM_CLUB_0012, TEXT("您已解散茶社[%s]！"), pClubProperty->szClubName));

			//查找馆主
			CClubUserItem* pClubUserItem = m_ClubUserManager.SearchUserItem(pClubProperty->dwCreaterID);
			if (pClubUserItem != NULL && pClubUserItem->GetUserStatus() == US_CB_ONLINE)
			{
				//发送数据
				SendDataToUser(SERVICE_MODULE_CLUBPLAZA, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_DELETE, &ClubDelete, sizeof(ClubDelete), pClubUserItem->GetSocketInfo());
			}

			//移除馆主
			pClubInfoItem->RemoveMember(pClubProperty->dwCreaterID);

			//构造提示
			StringCchCopy(ClubDelete.szNotifyContent, CountArray(ClubDelete.szNotifyContent), CWHService::FormatMessageV(MIM_CLUB_0013, TEXT("您加入的茶社[%s]已被老板解散！"), pClubProperty->szClubName));

			//发送数据
			SendDataToMember(ClubDelete.dwClubID, SERVICE_MODULE_CLUBPLAZA, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_DELETE, &ClubDelete, sizeof(ClubDelete), LOCATION_MASK_INCLUB | LOCATION_MASK_OUTCLUB);

			//删除茶社
			m_ClubInfoManager.RemoveClubItem(pDeleteClub->dwClubID);

			//获取对象
			CMD_BC_C_ClubDelete BCClubDelete;
			ZeroMemory(&BCClubDelete, sizeof(BCClubDelete));

			//设置变量
			BCClubDelete.dwClubID = pDeleteClub->dwClubID;

			//删除通知
			m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_CLUB_DELETE, &BCClubDelete, sizeof(BCClubDelete),0);
		}

		return true;
	}		
	case SUB_CS_CB_INSERT_MEMBER:		//添加成员
	{
		//提取数据
		ASSERT (wDataSize == sizeof(CMD_CS_CB_InsertMember));
		if (wDataSize != sizeof(CMD_CS_CB_InsertMember)) return false;

		//提取数据
		auto pInsertMember = (CMD_CS_CB_InsertMember*)pData;

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pInsertMember->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//构造结构
			CMD_CB_S_MemberInsert MemberInsert;
			ZeroMemory(&MemberInsert, sizeof(MemberInsert));

			//设置变量
			MemberInsert.dwClubID = pInsertMember->dwClubID;
			MemberInsert.wMemberCount = pInsertMember->wMemberCount;
			CopyMemory(&MemberInsert.MemberUser, &pInsertMember->MemberUser, sizeof(MemberInsert.MemberUser));

			//发送数据
			SendDataToMember(MemberInsert.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_INSERT, &MemberInsert, sizeof(MemberInsert), LOCATION_MASK_INCLUB);

			//添加成员
			pClubInfoItem->GetClubProperty()->wMemberCount=pInsertMember->wMemberCount;
			pClubInfoItem->AppendMember(pInsertMember->MemberUser.MemberInfo);

			//添加用户
			auto pClubUserItem = m_ClubUserManager.SearchUserItem(pInsertMember->MemberUser.UserInfo.dwUserID);
			if (pClubUserItem != NULL)
			{
				//获取对象
				auto pClubUserInfo = pClubUserItem->GetUserInfo();
				auto pMemberUserInfo = &pInsertMember->MemberUser.UserInfo;

				//设置变量
				pClubUserInfo->wFaceID = pMemberUserInfo->wFaceID;
				pClubUserInfo->cbGender = pMemberUserInfo->cbGender;
				pClubUserInfo->dwCustomID = pMemberUserInfo->dwCustomID;
				StringCchCopy(pClubUserInfo->szNickName,CountArray(pClubUserInfo->szNickName), pMemberUserInfo->szNickName);
				StringCchCopy(pClubUserInfo->szUserHeadUrl, CountArray(pClubUserInfo->szUserHeadUrl), pMemberUserInfo->szUserHeadUrl);//kk jia
			}
			else
			{
				tagSocketInfo SocketInfo = {};
				m_ClubUserManager.InsertUserItem(pInsertMember->MemberUser.UserInfo, SocketInfo);

				

				CString s;
				s.Format(_T("插入用户3 szNickName:%s,szUserHeadUrl:%s"), pInsertMember->MemberUser.UserInfo.szNickName, pInsertMember->MemberUser.UserInfo.szUserHeadUrl);
				CTraceService::TraceString(s, TraceLevel_Exception);
			}

			//构造结构
			tagClubNameCard ClubNameCard;
			ZeroMemory(&ClubNameCard, sizeof(ClubNameCard));

			//构造群名片
			ConstructClubNameCard(pClubInfoItem->GetClubProperty(), &ClubNameCard);

			//更新名片
			SendDataToMember(pInsertMember->dwClubID, SERVICE_MODULE_CLUBPLAZA, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_NAMECARD, &ClubNameCard, sizeof(ClubNameCard), LOCATION_MASK_OUTCLUB);
		}

		return true;
	}
	case SUB_CS_CB_UPDATE_MEMBER:		//更新成员
	{
		//提取数据
		ASSERT(wDataSize == sizeof(CMD_CS_CB_UpdateMember));
		if (wDataSize != sizeof(CMD_CS_CB_UpdateMember)) return false;

		//提取数据
		auto pUpdateMember = (CMD_CS_CB_UpdateMember*)pData;

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pUpdateMember->dwClubID);
		if (pClubInfoItem == NULL) return true;

		//查询成员
		auto pIMClubProperty = pClubInfoItem->GetClubProperty();

		//查询成员
		auto pClubMember = pClubInfoItem->SearchMember(pUpdateMember->ClubMember.dwMemberID);
		if (pClubMember == NULL) return true;

		//变量定义
		BYTE cbLastMemberType = pClubMember->cbMemberType;

		//更新成员
		CopyMemory(pClubMember, &pUpdateMember->ClubMember, sizeof(tagClubMemberInfo));

		//更新成员类型
		if (pUpdateMember->wUpdateKind == UPMEMBER_KIND_TYPE)
		{
			//查找用户
			auto pClubUserItem = m_ClubUserManager.SearchUserItem(pUpdateMember->ClubMember.dwMemberID);
			if (pClubUserItem != NULL && pClubUserItem->GetUserStatus() == US_CB_ONLINE)
			{
				//设置管理员
				if (cbLastMemberType != MEMBER_TYPE_MANAGER && pUpdateMember->ClubMember.cbMemberType == MEMBER_TYPE_MANAGER && (pUpdateMember->ClubMember.dwManageRight & MANAGE_RIGHT_AUDIT_APPLY) != 0)
				{
					//查找用户
					SendApplyMessageToUser(pIMClubProperty->dwClubID, pClubUserItem->GetSocketInfo());
				}
			}	
		}

		//构造结构
		CMD_CB_S_MemberUpdate MemberUpdate;
		ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));

		//设置变量
		MemberUpdate.dwClubID = pUpdateMember->dwClubID;
		CopyMemory(&MemberUpdate.ClubMember, &pUpdateMember->ClubMember, sizeof(MemberUpdate.ClubMember));

		//发送数据
		SendDataToMember(MemberUpdate.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), LOCATION_MASK_INCLUB);

		return true;
	}
	case SUB_CS_CB_DELETE_MEMBER:		//删除成员
	{
		//提取数据
		ASSERT(wDataSize == sizeof(CMD_CS_CB_DeleteMember));
		if (wDataSize != sizeof(CMD_CS_CB_DeleteMember)) return false;

		//提取数据
		auto pDeleteMember = (CMD_CS_CB_DeleteMember*)pData;

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pDeleteMember->dwClubID);
		if (pClubInfoItem != NULL && pClubInfoItem->SearchMember(pDeleteMember->dwMemberID))
		{
			//茶社属性
			tagClubProperty* pClubProperty = pClubInfoItem->GetClubProperty();

			//删除成员
			pClubProperty->wMemberCount= pDeleteMember->wMemberCount;
			pClubInfoItem->RemoveMember(pDeleteMember->dwMemberID);

			//构造结构
			CMD_CB_S_MemberDelete MemberDelete;
			ZeroMemory(&MemberDelete, sizeof(MemberDelete));

			//设置变量
			MemberDelete.dwClubID = pDeleteMember->dwClubID;
			MemberDelete.dwMemberID = pDeleteMember->dwMemberID;
			MemberDelete.wMemberCount = pDeleteMember->wMemberCount;

			//发送数据
			SendDataToMember(MemberDelete.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_DELETE, &MemberDelete, sizeof(MemberDelete), LOCATION_MASK_INCLUB);

			//更新代付
			if (pDeleteMember->dwMemberID == pClubProperty->dwPayrollerID)
			{
				//构造结构
				CMD_CB_S_ClubUpdate* pClubUpdate = (CMD_CB_S_ClubUpdate*)m_cbBuffer;

				//设置变量
				pClubUpdate->cbUpdateKind = UPCLUB_KIND_PROPERTY;
				pClubUpdate->dwClubID = pDeleteMember->dwClubID;

				//拷贝数据
				CopyMemory(pClubUpdate + 1, pClubProperty, sizeof(tagClubProperty));

				//发送数据
				WORD wPacketSize = sizeof(CMD_CB_S_ClubUpdate) + sizeof(tagClubProperty);
				SendDataToMember(pDeleteMember->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_UPDATE, pClubUpdate, wPacketSize, LOCATION_MASK_INCLUB);
			}

			//构造结构
			tagClubNameCard ClubNameCard;
			ZeroMemory(&ClubNameCard, sizeof(ClubNameCard));

			//构造群名片
			ConstructClubNameCard(pClubInfoItem->GetClubProperty(), &ClubNameCard);

			//更新名片
			SendDataToMember(pDeleteMember->dwClubID, SERVICE_MODULE_CLUBPLAZA, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_NAMECARD, &ClubNameCard, sizeof(ClubNameCard), LOCATION_MASK_OUTCLUB);
		}

		return true;
	}
	case SUB_CS_CB_APPLY_MESSAGE:		//申请消息
	{
		//提取数据
		ASSERT(wDataSize >= sizeof(CMD_CS_CB_ApplyMessage));
		if (wDataSize < sizeof(CMD_CS_CB_ApplyMessage)) return false;

		//提取数据
		auto pApplyMessage = (CMD_CS_CB_ApplyMessage*)pData;

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pApplyMessage->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//变量定义
			auto pClubApplyMessage = (tagClubApplyMessage*)(pApplyMessage + 1);

			//更新消息
			for (WORD i = 0; i < pApplyMessage->wMessageCount; i++)
			{
				auto pMessageItem = pClubInfoItem->SearchMessage(pClubApplyMessage->dwMessageID);
				if (pMessageItem == NULL)
				{
					pClubInfoItem->AppendMessage(*pClubApplyMessage);
				}
				else
				{
					CopyMemory(pMessageItem, pClubApplyMessage, sizeof(tagClubApplyMessage));
				}				

				pClubApplyMessage++;
			}

			//构造结构
			auto pClubMessage = (CMD_CB_S_ApplyMessage*)m_cbBuffer;
			ZeroMemory(pClubMessage, sizeof(CMD_CB_S_ApplyMessage));

			//设置变量
			pClubMessage->wMessageCount = pApplyMessage->wMessageCount;

			//拷贝数据
			CopyMemory(pClubMessage + 1, pApplyMessage + 1, wDataSize - sizeof(CMD_CS_CB_ApplyMessage));

			//计算大小
			WORD wPacketSize = sizeof(CMD_CB_S_ApplyMessage) + wDataSize - sizeof(CMD_CS_CB_ApplyMessage);

			//变量定义
			POSITION Position = NULL;
			tagClubMemberInfo* pClubMember = NULL;

			do
			{
				//枚举成员
				pClubMember = pClubInfoItem->EnumMember(Position);
				if (pClubMember == NULL) break;				

				//馆主或管理员
				if (pClubMember->cbMemberType == MEMBER_TYPE_EMPLOYER || (pClubMember->cbMemberType == MEMBER_TYPE_MANAGER && (pClubMember->dwManageRight & MANAGE_RIGHT_AUDIT_APPLY) != 0))
				{
					//查找用户
					CClubUserItem* pClubUserItem = m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
					if (pClubUserItem == NULL || pClubUserItem->GetActiveClubID() != pApplyMessage->dwClubID) continue;

					//发送数据
					SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_APPLY_MESSAGE, m_cbBuffer, wPacketSize, pClubUserItem->GetSocketInfo());
				}

			} while (Position != NULL);
		}

		return true;
	}
	case SUB_CS_CB_APPLY_DELETE:		//申请删除
	{
		//提取数据
		ASSERT(wDataSize >= sizeof(CMD_CS_CB_ApplyDelete));
		if (wDataSize < sizeof(CMD_CS_CB_ApplyDelete)) return false;

		//提取数据
		auto pApplyDelete = (CMD_CS_CB_ApplyDelete*)pData;

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pApplyDelete->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//消息标识
			auto pdwMessageID = (DWORD*)(pApplyDelete + 1);

			//更新消息
			for (WORD i = 0; i < pApplyDelete->wMessageCount; i++)
			{
				//移除消息
				pClubInfoItem->RemoveMessage(*pdwMessageID++);
			}

			//构造结构
			auto pCBApplyDelete = (CMD_CB_S_ApplyDelete*)m_cbBuffer;
			ZeroMemory(pCBApplyDelete, sizeof(CMD_CB_S_ApplyDelete));

			//设置变量
			pCBApplyDelete->wMessageCount = pApplyDelete->wMessageCount;

			//拷贝数据
			CopyMemory(pCBApplyDelete + 1, pApplyDelete + 1, wDataSize - sizeof(CMD_CS_CB_ApplyDelete));

			//计算大小
			WORD wPacketSize = sizeof(CMD_CB_S_ApplyDelete) + wDataSize - sizeof(CMD_CS_CB_ApplyDelete);

			//变量定义
			POSITION Position = NULL;
			tagClubMemberInfo* pClubMember = NULL;

			do
			{
				//枚举成员
				pClubMember = pClubInfoItem->EnumMember(Position);
				if (pClubMember == NULL) break;

				//馆主或管理员
				if (pClubMember->dwMemberID!=pApplyDelete->dwUserID && (pClubMember->cbMemberType == MEMBER_TYPE_EMPLOYER || (pClubMember->cbMemberType == MEMBER_TYPE_MANAGER && (pClubMember->dwManageRight & MANAGE_RIGHT_AUDIT_APPLY) != 0)))
				{
					//查找用户
					auto pClubUserItem = m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
					if (pClubUserItem == NULL || pClubUserItem->GetActiveClubID() != pApplyDelete->dwClubID) continue;

					//发送数据
					SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_APPLY_DELETE, m_cbBuffer, wPacketSize, pClubUserItem->GetSocketInfo());
				}

			} while (Position != NULL);
		}

		return true;
	}
	case SUB_CS_CB_MODIFY_SCORE:		//修改积分
	{
		//提取数据
		ASSERT(wDataSize == sizeof(CMD_CS_CB_ModifyScore));
		if (wDataSize != sizeof(CMD_CS_CB_ModifyScore)) return false;

		//提取数据
		auto pModifyScore = (CMD_CS_CB_ModifyScore*)pData;

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pModifyScore->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//查询成员
			tagClubMemberInfo* pClubMember = pClubInfoItem->SearchMember(pModifyScore->dwMemberID);
			if (pClubMember != NULL)
			{
				//更新分数
				pClubMember->lClubScore = pModifyScore->lMemberScore;

				CString strMessage;
				strMessage.Format(TEXT("MemberID=%d,MemberScore=%I64d"), pModifyScore->dwMemberID, pModifyScore->lMemberScore);
				CTraceService::TraceString(strMessage, TraceLevel_Exception);

				//构造结构
				CMD_CB_S_MemberUpdate MemberUpdate;
				ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));

				//设置变量
				MemberUpdate.dwClubID = pModifyScore->dwClubID;
				CopyMemory(&MemberUpdate.ClubMember, pClubMember, sizeof(MemberUpdate.ClubMember));

				//发送数据
				SendDataToMember(MemberUpdate.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), LOCATION_MASK_INCLUB);

				//成员提示
				CClubUserItem* pClubUserItem = m_ClubUserManager.SearchUserItem(pModifyScore->dwMemberID);
				if (pClubUserItem != NULL && pClubUserItem->GetActiveClubID() == MemberUpdate.dwClubID)
				{
					//获取对象		
					tagSocketInfo* pIMSocketInfo = pClubUserItem->GetSocketInfo();

					//构造消息
					CMD_CM_SystemMessage SystemMessage;
					ZeroMemory(&SystemMessage, sizeof(SystemMessage));

					//变量定义						
					SystemMessage.wType = SMT_EJECT;
					StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), CWHService::FormatMessageV(MIM_CLUB_0014, TEXT("茶社老板,给您%s%lld分"), pModifyScore->lModifyScore >= 0 ? TEXT("增加") : TEXT("减去"), abs(pModifyScore->lModifyScore / SCORE_MULTIPLE)));
					

					//发送消息
					SystemMessage.wLength = CountStringBuffer(SystemMessage.szString);
					WORD wPacketSize = sizeof(SystemMessage) - sizeof(SystemMessage.szString) + SystemMessage.wLength;
					SendDataToGate(pIMSocketInfo->wGateID, SERVICE_MODULE_CLUBPLAZA, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wPacketSize, &pIMSocketInfo->dwTokenID, 1);
				}
			}

			//查询分数
			if (pModifyScore->wKindID != 0 && pModifyScore->wServerID != 0)
			{
				//获取对象
				CMD_BC_C_ModifyScore ModifyScore;
				ZeroMemory(&ModifyScore, sizeof(ModifyScore));

				//成员信息
				ModifyScore.wKindID = pModifyScore->wKindID;
				ModifyScore.wServerID = pModifyScore->wServerID;
				ModifyScore.dwUserID = pModifyScore->dwMemberID;

				//变更信息
				ModifyScore.cbChangeType = SCORE_CHANGE_TYPE_MODIFY;
				ModifyScore.lChangeScore = pModifyScore->lModifyScore;

				//茶馆信息
				ModifyScore.dwClubID = pModifyScore->dwClubID;
				StringCchCopy(ModifyScore.szClubName, CountArray(ModifyScore.szClubName), pClubInfoItem->GetClubProperty()->szClubName);

				//广播消息
				m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_MODIFY_SCORE, &ModifyScore, sizeof(ModifyScore),0);
			}
		}		

		return true;
	}		
	case SUB_CS_CB_UPDATE_PAYROLL:		//更新代付
	{
		//提取数据
		ASSERT(wDataSize == sizeof(CMD_CS_CB_UpdatePayroll));
		if (wDataSize != sizeof(CMD_CS_CB_UpdatePayroll)) return false;

		//提取数据
		auto pUpdatePayroll = (CMD_CS_CB_UpdatePayroll*)pData;

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pUpdatePayroll->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//茶社属性
			tagClubProperty* pClubProperty = pClubInfoItem->GetClubProperty();

			//设置代付
			pClubProperty->dwPayrollerID = pUpdatePayroll->dwPayrollerID;

			//构造结构
			auto pClubUpdate = (CMD_CB_S_ClubUpdate*)m_cbBuffer;

			//设置变量
			pClubUpdate->cbUpdateKind = UPCLUB_KIND_PROPERTY;
			pClubUpdate->dwClubID = pUpdatePayroll->dwClubID;

			//拷贝数据
			CopyMemory(pClubUpdate + 1, pClubProperty, sizeof(tagClubProperty));

			//发送数据
			WORD wPacketSize = sizeof(CMD_CB_S_ClubUpdate) + sizeof(tagClubProperty);
			SendDataToMember(pUpdatePayroll->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_UPDATE, pClubUpdate, wPacketSize, LOCATION_MASK_INCLUB);
		}

		return true;
	}
	case SUB_CS_CB_CLEANUP_SCORE:		//清理积分
	{
		//提取数据
		ASSERT(wDataSize == sizeof(CMD_CS_CB_CleanupScore));
		if (wDataSize != sizeof(CMD_CS_CB_CleanupScore)) return false;

		//提取数据
		auto pCleanupScore = (CMD_CS_CB_CleanupScore*)pData;

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pCleanupScore->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//一个成员
			if (pCleanupScore->cbCleanupObject == CLEANUP_OBJECT_ONE_MEMBER)
			{
				//查询成员
				tagClubMemberInfo* pClubMember = pClubInfoItem->SearchMember(pCleanupScore->dwMemberID);
				if (pClubMember != NULL)
				{
					//更新分数
					pClubMember->lClubScore = 0;

					//构造结构
					CMD_CB_S_MemberUpdate MemberUpdate;
					ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));

					//设置变量
					MemberUpdate.dwClubID = pCleanupScore->dwClubID;
					CopyMemory(&MemberUpdate.ClubMember, pClubMember, sizeof(MemberUpdate.ClubMember));

					//发送数据
					SendDataToMember(MemberUpdate.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), LOCATION_MASK_INCLUB);
				}

				//成员提示
				CClubUserItem* pClubUserItem = m_ClubUserManager.SearchUserItem(pCleanupScore->dwMemberID);
				if (pClubUserItem != NULL && pClubUserItem->GetActiveClubID() == pCleanupScore->dwClubID)
				{
					//获取对象		
					tagSocketInfo* pIMSocketInfo = pClubUserItem->GetSocketInfo();

					//构造消息
					CMD_CM_SystemMessage SystemMessage;
					ZeroMemory(&SystemMessage, sizeof(SystemMessage));

					//变量定义						
					SystemMessage.wType = SMT_EJECT;
					StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), CWHService::FormatMessage(MIM_CLUB_0015, TEXT("您的积分已被清零")));

					//发送消息
					SystemMessage.wLength = CountStringBuffer(SystemMessage.szString);
					WORD wPacketSize = sizeof(SystemMessage) - sizeof(SystemMessage.szString) + SystemMessage.wLength;
					SendDataToGate(pIMSocketInfo->wGateID, SERVICE_MODULE_CLUBPLAZA, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wPacketSize, &pIMSocketInfo->dwTokenID, 1);
				}
			}

			//所有成员
			if (pCleanupScore->cbCleanupObject == CLEANUP_OBJECT_ALL_MEMBER)
			{
				//变量定义
				POSITION Position = NULL;
				tagClubMemberInfo* pClubMember = NULL;

				do
				{
					//枚举成员
					pClubMember = pClubInfoItem->EnumMember(Position);
					if (pClubMember == NULL) break;

					//修改积分
					pClubMember->lClubScore = 0;

				} while (Position != NULL);

				//更新成员
				SendClubMemberToMember(pCleanupScore->dwClubID);

				//构造消息
				CMD_CM_SystemMessage SystemMessage;
				ZeroMemory(&SystemMessage, sizeof(SystemMessage));

				//变量定义						
				SystemMessage.wType = SMT_EJECT;
				StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), CWHService::FormatMessage(MIM_CLUB_0015, TEXT("您的积分已被清零")));

				//发送消息
				SystemMessage.wLength = CountStringBuffer(SystemMessage.szString);
				WORD wPacketSize = sizeof(SystemMessage) - sizeof(SystemMessage.szString) + SystemMessage.wLength;
				SendDataToMember(pCleanupScore->dwClubID, SERVICE_MODULE_CLUBPLAZA, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wPacketSize, LOCATION_MASK_INCLUB);
			}
		}

		return true;
	}
	case SUB_CS_CB_CLEANUP_REVENUE:		//清理税收
	{
		//提取数据
		ASSERT(wDataSize == sizeof(CMD_CS_CB_CleanupRevenue));
		if (wDataSize != sizeof(CMD_CS_CB_CleanupRevenue)) return false;

		//提取数据
		auto pCleanupRevenue = (CMD_CS_CB_CleanupRevenue*)pData;

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pCleanupRevenue->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//一个成员
			if (pCleanupRevenue->cbCleanupObject == CLEANUP_OBJECT_ONE_MEMBER)
			{
				//查询成员
				tagClubMemberInfo* pClubMember = pClubInfoItem->SearchMember(pCleanupRevenue->dwMemberID);
				if (pClubMember != NULL)
				{
					//更新分数
					pClubMember->lRevenueCount = 0;

					//构造结构
					CMD_CB_S_MemberUpdate MemberUpdate;
					ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));

					//设置变量
					MemberUpdate.dwClubID = pCleanupRevenue->dwClubID;
					CopyMemory(&MemberUpdate.ClubMember, pClubMember, sizeof(MemberUpdate.ClubMember));

					//发送数据
					SendDataToMember(MemberUpdate.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), LOCATION_MASK_INCLUB);
				}
			}

			//所有成员
			if (pCleanupRevenue->cbCleanupObject == CLEANUP_OBJECT_ALL_MEMBER)
			{
				//变量定义
				POSITION Position = NULL;
				tagClubMemberInfo* pClubMember = NULL;

				do
				{
					//枚举成员
					pClubMember = pClubInfoItem->EnumMember(Position);
					if (pClubMember == NULL) break;

					//修改积分
					pClubMember->lRevenueCount = 0;

				} while (Position != NULL);

				//更新成员
				SendClubMemberToMember(pCleanupRevenue->dwClubID);
			}
		}

		return true;
	}
	}

	return true;
}


//注册服务
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{		
	case SUB_CS_C_REGISTER_SERVICE:	//注册网关
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterService));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterService)) return false;

			//消息定义
			auto pRegisterAccess=(CMD_CS_C_RegisterService *)pData;

			//校验模块
			if (pRegisterAccess->wServiceModule != SERVICE_MODULE_GATE) return false;

			//查找网关
			if (m_GlobalItemManager.SearchGateItem(pRegisterAccess->wServiceID)!=NULL)
			{
				//变量定义
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//设置变量
				RegisterFailure.lErrorCode=0;
				StringCchCopy(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), TEXT("已经存在相同标识的网关服务，网关服务注册失败"));

				//发送消息
				WORD wStringSize=CountStringBufferW(RegisterFailure.szDescribeString);
				WORD wSendSize=sizeof(RegisterFailure)-sizeof(RegisterFailure.szDescribeString)+wStringSize;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_FAILURE,&RegisterFailure,wSendSize,NULL,0);

				//中断网络
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//设置绑定
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->wServiceModule= SERVICE_MODULE_GATE;
			(m_pBindParameter+wBindIndex)->wServiceID=pRegisterAccess->wServiceID;

			//变量定义
			tagServiceItem ServiceItem;
			ZeroMemory(&ServiceItem,sizeof(ServiceItem));

			//构造数据
			ServiceItem.wServiceID=pRegisterAccess->wServiceID;
			ServiceItem.wServiceModule= SERVICE_MODULE_GATE;
			ServiceItem.wServicePort = pRegisterAccess->wServicePort;
			ServiceItem.wMaxConnect= pRegisterAccess->wMaxConnect;
			ServiceItem.dwServiceAddr = pRegisterAccess->dwServiceAddr;
			StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), pRegisterAccess->szServiceName);
			StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain),pRegisterAccess->szServiceDomain);

			//注册网关
			(m_pBindParameter + wBindIndex)->pBindServiceItem = m_GlobalItemManager.ActiveGateItem(dwSocketID, ServiceItem);

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
	case SUB_CS_C_LOGOUT_TOKEN:
	{
		//设置绑定
		WORD wBindIndex = LOWORD(dwSocketID);
		auto pBindParameter = (m_pBindParameter + wBindIndex);

		//校验服务
		if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return true;

		//提取数据
		auto pLogoutToken = (CMD_CS_C_LogoutToken*)pData;

		//查找网关
		auto pGlobalGateItem = (CGlobalGateItem*)pBindParameter->pBindServiceItem;
		if (pGlobalGateItem != NULL)
		{
			//获取令牌
			auto pTokenParameter = pGlobalGateItem->SearchTokenID(pLogoutToken->dwTokenID);
			if (pTokenParameter!=NULL)
			{
				//查找用户
				auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
				if (pClubUserItem != NULL && pClubUserItem->GetActiveClubID()>0)
				{
					//更新在线
					auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pClubUserItem->GetActiveClubID());
					if (pClubInfoItem != NULL)
					{
						//在线状态
						if (pClubInfoItem->GetClubState() == CLUB_STATE_ONLINE)
						{
							pClubInfoItem->DecreaseOnline();
						}						

						//加载状态
						if (pClubInfoItem->GetClubState() == CLUB_STATE_LOADING)
						{
							//构造对象
							tagSocketInfo SocketInfo = {};
							SocketInfo.wGateID = pBindParameter->wServiceID;
							SocketInfo.dwTokenID = pLogoutToken->dwTokenID;

							//移除请求
							pClubInfoItem->RemoveEnterRequest(SocketInfo);
						}
					}

					//设置变量
					pClubUserItem->SetActiveClubID(0);
				}

				//移除令牌
				pGlobalGateItem->RemoveTokenID(pLogoutToken->dwTokenID);
			}
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
		auto pRegisterToken = (CMD_CS_C_RegisterToken*)pData;

		//构造结构
		CMD_CS_S_RegisterToken RegisterToken;
		ZeroMemory(&RegisterToken, sizeof(RegisterToken));

		//默认成功
		RegisterToken.lResultCode = REGISTER_RESULT_FAILURE;

		//查找网关
		auto pGlobalGateItem = (CGlobalGateItem*)pBindParameter->pBindServiceItem;
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

	return true;
}

//服务状态
bool CAttemperEngineSink::OnTCPSocketBattleMainRegister(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_SUCCESS:		//注册成功
	{
		//事件通知
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess = ER_SUCCESS;
		SendUIControlPacket(UI_BATTLE_RESULT, &ControlResult, sizeof(ControlResult));

		CTraceService::TraceString(TEXT("%s连接成功"), TraceLevel_Normal, TEXT("约战服务器"));

		//断线重连特殊处理
		if (m_ClubInfoManager.GetClubItemCount()>0)
		{
			//变量定义
			CClubInfoItem* pClubInfoItem = NULL;

			do
			{
				//枚举茶社
				auto pLastClubInfoItem = pClubInfoItem;
				pClubInfoItem = m_ClubInfoManager.EnumClubItem(pLastClubInfoItem);

				//校验茶社
				if (pClubInfoItem != NULL)
				{		
					QueryBattleTable(pClubInfoItem);
				}

			} while (pClubInfoItem != NULL);
		}


		//设置时钟
		m_pITimerEngine->SetTimer(IDI_DETECTION_TABLE, 5 * 1000L, -1, 0);

		return true;
	}
	case SUB_CS_S_REGISTER_FAILURE:		//注册失败
	{
		//变量定义
		auto pRegisterFailure = (CMD_CS_S_RegisterFailure*)pData;

		//效验参数
		ASSERT(wDataSize >= (sizeof(CMD_CS_S_RegisterFailure) - sizeof(pRegisterFailure->szDescribeString)));
		if (wDataSize < (sizeof(CMD_CS_S_RegisterFailure) - sizeof(pRegisterFailure->szDescribeString))) return false;

		//显示消息
		CW2CT strDescribeString(pRegisterFailure->szDescribeString);
		if (lstrlen(strDescribeString) > 0) CTraceService::TraceString(strDescribeString, TraceLevel_Exception);

		return true;
	}
	}

	return true;
}

//通用消息
bool CAttemperEngineSink::OnTCPSocketBattleMainCommonService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	return true;
}

//约战服务
bool CAttemperEngineSink::OnTCPSocketBattleMainBattleService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_BC_S_TABLE_UPDATE:			//桌子更新
	{
		//校验数据
		ASSERT (wDataSize == sizeof(tagBattleTableItem));
		if (wDataSize != sizeof(tagBattleTableItem)) return false;

		//提取数据
		auto pBattleTableItem = (tagBattleTableItem*)pData;

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pBattleTableItem->dwClubID);
		if (pClubInfoItem == NULL)
		{
			return true;
		}

		//更新桌子
		auto pClubTableItem = pClubInfoItem->SearchClubTable(pBattleTableItem->dwBattleNum);
		if (pClubTableItem != NULL)
		{
			CopyMemory(&pClubTableItem->TableItem, pBattleTableItem,sizeof(tagBattleTableItem));
		}

		//发送数据
		SendDataToMember(pBattleTableItem->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_TABLE_UPDATE, pBattleTableItem, sizeof(tagBattleTableItem), LOCATION_MASK_INCLUB);

		return true;
	}
	case SUB_BC_S_TABLE_DISMISS:		//桌子解散
	{
		//校验数据
		ASSERT(wDataSize >= sizeof(CMD_BC_S_TableDismiss));
		if (wDataSize < sizeof(CMD_BC_S_TableDismiss)) return false;

		//提取数据
		auto pTableDismiss = (CMD_BC_S_TableDismiss*)pData;

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pTableDismiss->dwClubID);
		if (pClubInfoItem == NULL) return false;

		//游戏开始
		auto pClubTableItem = pClubInfoItem->SearchClubTable(pTableDismiss->dwBattleNum);
		if (pClubTableItem->TableParam.wFinishCount==0 && !pClubTableItem->TableParam.cbGameStatus)
		{
			//查找玩法
			auto pBattleConfig = pClubInfoItem->SearchConfig(pClubTableItem->TableItem.dwConfigID);
			if (pBattleConfig != NULL)
			{
				if (pBattleConfig->wTableCount > 0) pBattleConfig->wTableCount--;

				//创建桌子
				while (pBattleConfig->wTableCount < FREE_TABLE_COUNT)
				{
					//更新桌数
					pBattleConfig->wTableCount++;

					CTraceService::TraceString(TEXT("创建桌子2"), TraceLevel_Debug);

					//创建桌子
					CreateBattleTable(pClubInfoItem, pBattleConfig);
				}
			}
		}

		//移除桌子
		pClubInfoItem->RemoveClubTable(pTableDismiss->dwBattleNum);

		//构造结构
		CMD_CB_S_TableDismiss TableDismiss;
		ZeroMemory(&TableDismiss, sizeof(TableDismiss));

		//设置变量
		TableDismiss.dwClubID = pTableDismiss->dwClubID;
		TableDismiss.dwBattleNum = pTableDismiss->dwBattleNum;

		//发送数据
		SendDataToMember(TableDismiss.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_TABLE_DISMISS, &TableDismiss, sizeof(TableDismiss), LOCATION_MASK_INCLUB);

		//变量定义
		DWORD* pdwUserID = (DWORD*)(pTableDismiss + 1);

		//统计约战次数
		for (WORD i = 0; i < pTableDismiss->wUserCount; i++)
		{
			//更新成员
			DBR_GR_UpdateMember UpdateMember;
			ZeroMemory(&UpdateMember, sizeof(UpdateMember));

			//设置变量
			UpdateMember.dwMemberID = *pdwUserID++;
			UpdateMember.dwClubID = pTableDismiss->dwClubID;

			//更新信息
			UpdateMember.wIncBattleCount = 1;
			UpdateMember.cbUpdateKind = UPMEMBER_KIND_BATTLE;

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_UPDATE_MEMBER, 0, 0, 0, &UpdateMember, sizeof(UpdateMember));
		}

		return true;
	}
	case SUB_BC_S_TABLE_ITEM:			//桌子子项
	{
		//校验数据
		ASSERT (wDataSize>=sizeof(tagBattleTableItem));
		if ((wDataSize<sizeof(tagBattleTableItem))) return false;

		//提取数据
		int nResidueSize = wDataSize;
		auto pcbByteData = (LPBYTE)pData;				

		//变量定义
		CClubInfoItem* pClubInfoItem = NULL;
		tagClubTableItem ClubTableItem = {};			
		
		//校验数量
		while (nResidueSize>0)
		{
			//重置桌子
			ZeroMemory(&ClubTableItem,sizeof(ClubTableItem));

			//桌子子项
			ClubTableItem.TableItem = *(tagBattleTableItem*)(pcbByteData+(wDataSize-nResidueSize));
			nResidueSize -= sizeof(tagBattleTableItem);

			//桌子参数
			auto pTableParam = (tagBattleTableParam*)(pcbByteData + (wDataSize-nResidueSize));
			nResidueSize -= sizeof(tagBattleTableParam)+pTableParam->wUserCount*sizeof(tagBattleTableUser);

			//设置参数
			ClubTableItem.TableParam.dwStartTime = pTableParam->dwStartTime;
			ClubTableItem.TableParam.wUserCount = pTableParam->wUserCount;
			ClubTableItem.TableParam.wFinishCount = pTableParam->wFinishCount;
			ClubTableItem.TableParam.cbGameStatus = pTableParam->cbGameStatus;

			//拷贝用户
			for (int i = 0; i < pTableParam->wUserCount; i++)
			{		
				auto pTableUser = &pTableParam->TableUserItem[i];
				if (pTableUser->wChairID < CountArray(ClubTableItem.TableParam.TableUserItem))
				{
					CopyMemory(&ClubTableItem.TableParam.TableUserItem[pTableUser->wChairID], pTableUser, sizeof(tagBattleTableUser));
				}
			}

			//查找茶社
			if (pClubInfoItem == NULL)
			{
				pClubInfoItem = m_ClubInfoManager.SearchClubItem(ClubTableItem.TableItem.dwClubID);
			}

			//添加桌子
			if (pClubInfoItem != NULL)
			{
				if (pClubInfoItem->AppendClubTable(ClubTableItem))
				{
					//校验状态
					if (ClubTableItem.TableItem.dwConfigID != 0 && ClubTableItem.TableParam.wFinishCount==0 && !ClubTableItem.TableParam.cbGameStatus)
					{
						auto pBattleConfig = pClubInfoItem->SearchConfig(ClubTableItem.TableItem.dwConfigID);
						if (pBattleConfig != NULL) pBattleConfig->wTableCount++;

						CTraceService::TraceString(TEXT("玩法[%d]增加桌子)！"), TraceLevel_Exception, ClubTableItem.TableItem.dwConfigID);
					}
				}
			}			
		}

		//房间创建失败
		CTraceService::TraceString(TEXT("茶社[%d]广播桌子给玩家)！"), TraceLevel_Exception, ClubTableItem.TableItem.dwClubID);

		//发送给成员
		SendDataToMember(ClubTableItem.TableItem.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_TABLE_ITEM, pData, wDataSize, LOCATION_MASK_INCLUB);

		return true;
	}
	case SUB_BC_S_TABLE_PARAM:			//房间参数
	{
		//校验数据
		ASSERT (wDataSize == sizeof(CMD_BC_S_TableParam));
		if (wDataSize != sizeof(CMD_BC_S_TableParam)) return false;

		//提取数据
		auto pTableParam = (CMD_BC_S_TableParam*)pData;

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pTableParam->dwClubID);
		if (pClubInfoItem == NULL) return false;
		
		//更新参数
		auto pClubTableItem = pClubInfoItem->SearchClubTable(pTableParam->dwBattleNum);
		if (pClubTableItem != NULL)
		{
			//约战开始
			if (pTableParam->wFinishCount == 1 && pTableParam->cbGameStatus==TRUE)
			{
				//查找玩法
				auto pBattleConfig = pClubInfoItem->SearchConfig(pClubTableItem->TableItem.dwConfigID);
				if (pBattleConfig != NULL)
				{
					if (pBattleConfig->wTableCount > 0) pBattleConfig->wTableCount--;

					//创建桌子
					while (pBattleConfig->wTableCount < FREE_TABLE_COUNT)
					{
						//更新桌数
						pBattleConfig->wTableCount++;

						CTraceService::TraceString(TEXT("创建桌子3"), TraceLevel_Debug);

						//创建桌子
						CreateBattleTable(pClubInfoItem, pBattleConfig);						
					}
				}
			}						

			pClubTableItem->TableParam.dwStartTime = pTableParam->dwStartTime;
			pClubTableItem->TableParam.wFinishCount = pTableParam->wFinishCount;
			pClubTableItem->TableParam.cbGameStatus = pTableParam->cbGameStatus;
		}

		//构造结构
		CMD_CB_S_TableParam TableParam;
		ZeroMemory(&TableParam, sizeof(TableParam));

		//设置变量
		TableParam.dwBattleNum = pTableParam->dwBattleNum;
		TableParam.dwStartTime = pTableParam->dwStartTime;
		TableParam.wFinishCount = pTableParam->wFinishCount;
		TableParam.cbGameStatus = pTableParam->cbGameStatus;

		//发送数据
		SendDataToMember(pTableParam->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_TABLE_PARAM, &TableParam, sizeof(TableParam), LOCATION_MASK_INCLUB);

		return true;
	}	
	case SUB_BC_S_TABLE_CREATE:			//桌子创建
	{
		//校验数据
		ASSERT (wDataSize >= sizeof(tagBattleTableItem)+sizeof(tagBattleTableParam));
		if (wDataSize < sizeof(tagBattleTableItem) + sizeof(tagBattleTableParam)) return false;

		//提取数据
		auto wResidueSize = wDataSize;
		auto pcbByteData = (LPBYTE)pData;

		//变量定义
		tagClubTableItem ClubTableItem = {};		

		//桌子子项
		ClubTableItem.TableItem = *(tagBattleTableItem*)(pcbByteData + (wDataSize - wResidueSize));
		wResidueSize -= sizeof(tagBattleTableItem);

		//桌子参数
		auto pTableParam = (tagBattleTableParam*)(pcbByteData + (wDataSize - wResidueSize));
		wResidueSize -= sizeof(tagBattleTableParam) + pTableParam->wUserCount * sizeof(tagBattleTableUser);

		//设置参数
		ClubTableItem.TableParam.dwStartTime = pTableParam->dwStartTime;
		ClubTableItem.TableParam.wUserCount = pTableParam->wUserCount;
		ClubTableItem.TableParam.wFinishCount = pTableParam->wFinishCount;
		ClubTableItem.TableParam.cbGameStatus = pTableParam->cbGameStatus;

		//拷贝用户
		for (int i = 0; i < pTableParam->wUserCount; i++)
		{
			auto pTableUser = &pTableParam->TableUserItem[i];
			if (pTableUser->wChairID < CountArray(ClubTableItem.TableParam.TableUserItem))
			{
				CopyMemory(&ClubTableItem.TableParam.TableUserItem[pTableUser->wChairID], pTableUser, sizeof(tagBattleTableUser));
			}
		}

		//添加桌子
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(ClubTableItem.TableItem.dwClubID);
		if (pClubInfoItem != NULL)
		{
			pClubInfoItem->AppendClubTable(ClubTableItem);
		}		

		//发送给成员
		SendDataToMember(ClubTableItem.TableItem.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_TABLE_CREATE, pData, wDataSize, LOCATION_MASK_INCLUB);

		return true;
	}
	case SUB_BC_S_BATTLE_FINISH:		//约战完成
	{
		//校验数据
		ASSERT(wDataSize >= sizeof(CMD_BC_S_BattleFinish));
		if (wDataSize < sizeof(CMD_BC_S_BattleFinish)) return false;

		//提取数据
		auto pBattleFinish = (CMD_BC_S_BattleFinish*)pData;

		//变量定义
		DWORD* pdwUserID = (DWORD*)(pBattleFinish + 1);

		//统计约战次数
		for (WORD i = 0; i < pBattleFinish->wUserCount; i++)
		{
			//更新成员
			DBR_GR_UpdateMember UpdateMember;
			ZeroMemory(&UpdateMember, sizeof(UpdateMember));

			//设置变量
			UpdateMember.dwMemberID = *pdwUserID++;
			UpdateMember.dwClubID = pBattleFinish->dwClubID;

			//更新信息
			UpdateMember.wIncBattleCount = 1;
			UpdateMember.cbUpdateKind = UPMEMBER_KIND_BATTLE;

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_UPDATE_MEMBER, 0, 0, 0, &UpdateMember, sizeof(UpdateMember));
		}

		return true;
	}
	case SUB_BC_S_USER_SITDOWN:			//用户坐下
	{
		//校验数据
		ASSERT(wDataSize == sizeof(CMD_BC_S_UserSitdown));
		if (wDataSize != sizeof(CMD_BC_S_UserSitdown)) return false;

		//提取数据
		auto pUserSitdown = (CMD_BC_S_UserSitdown*)pData;

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pUserSitdown->dwClubID);
		if (pClubInfoItem == NULL) return false;

		//查找桌子
		auto pClubTableItem = pClubInfoItem->SearchClubTable(pUserSitdown->dwBattleNum);
		if (pClubTableItem != NULL)
		{
			auto pTableUser = &pUserSitdown->TableUserItem;
			if (pTableUser->wChairID < CountArray(pClubTableItem->TableParam.TableUserItem))
			{
				CopyMemory(&pClubTableItem->TableParam.TableUserItem[pTableUser->wChairID], pTableUser, sizeof(tagBattleTableUser));
			}
		}

		//构造结构
		CMD_CB_S_UserSitdown UserSitdown;
		ZeroMemory(&UserSitdown, sizeof(UserSitdown));

		//设置变量
		UserSitdown.dwUserID = pUserSitdown->dwUserID;
		UserSitdown.dwBattleNum = pUserSitdown->dwBattleNum;
		CopyMemory(&UserSitdown.TableUserItem, &pUserSitdown->TableUserItem, sizeof(UserSitdown.TableUserItem));

		//发送数据
		SendDataToMember(pUserSitdown->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_USER_SITDOWN, &UserSitdown, sizeof(UserSitdown), LOCATION_MASK_INCLUB);

		return true;
	}
	case SUB_BC_S_USER_STANDUP:			//用户起立
	{
		//校验数据
		ASSERT(wDataSize == sizeof(CMD_BC_S_UserStandup));
		if (wDataSize != sizeof(CMD_BC_S_UserStandup)) return false;

		//提取数据
		auto pUserStandup = (CMD_BC_S_UserStandup*)pData;

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pUserStandup->dwClubID);
		if (pClubInfoItem == NULL) return false;

		//查找桌子
		auto pClubTableItem = pClubInfoItem->SearchClubTable(pUserStandup->dwBattleNum);
		if (pClubTableItem != NULL)
		{
			if (pUserStandup->wChairID < CountArray(pClubTableItem->TableParam.TableUserItem))
			{
				ZeroMemory(&pClubTableItem->TableParam.TableUserItem[pUserStandup->wChairID], sizeof(tagBattleTableUser));
			}
		}

		//构造结构
		CMD_CB_S_UserStandup UserStandup;
		ZeroMemory(&UserStandup, sizeof(UserStandup));

		//设置变量
		UserStandup.wChairID = pUserStandup->wChairID;
		UserStandup.dwUserID = pUserStandup->dwUserID;
		UserStandup.dwBattleNum = pUserStandup->dwBattleNum;

		//发送数据
		SendDataToMember(pUserStandup->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_USER_STANDUP, &UserStandup, sizeof(UserStandup), LOCATION_MASK_INCLUB);

		return true;
	}
	case SUB_BC_S_USER_EFFICACY:		//用户校验
	{
		//校验数据
		ASSERT(wDataSize == sizeof(CMD_BC_S_UserEfficacy));
		if (wDataSize != sizeof(CMD_BC_S_UserEfficacy)) return false;

		//提取数据
		auto pUserEfficacy = (CMD_BC_S_UserEfficacy*)pData;

		//构造结构
		CMD_BC_C_EfficacyResult EfficacyResult;
		ZeroMemory(&EfficacyResult, sizeof(CMD_BC_C_EfficacyResult));

		//设置变量
		EfficacyResult.dwUserID = pUserEfficacy->dwUserID;
		EfficacyResult.dwBattleNum = pUserEfficacy->dwBattleNum;
		CopyMemory(&EfficacyResult.UserSocketInfo, &pUserEfficacy->UserSocketInfo, sizeof(EfficacyResult.UserSocketInfo));

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pUserEfficacy->dwClubID);
		if (pClubInfoItem == NULL )
		{
			//设置结果
			EfficacyResult.bResult = false;
			StringCchCopy(EfficacyResult.szErrorMessage, CountArray(EfficacyResult.szErrorMessage), CWHService::FormatMessage(MIM_CLUB_0016, TEXT("您不是该茶社的成员不能加入房间！")));			

			//发送数据
			m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_EFFICACY_RESULT,&EfficacyResult,sizeof(EfficacyResult),0);

			return true;
		}

		auto pClubMember = pClubInfoItem->SearchMember(pUserEfficacy->dwUserID);
		if (pClubMember == NULL)
		{
			//设置结果
			EfficacyResult.bResult = false;
			StringCchCopy(EfficacyResult.szErrorMessage, CountArray(EfficacyResult.szErrorMessage), CWHService::FormatMessage(MIM_CLUB_0016, TEXT("您不是该茶社的成员不能加入房间！")));

			//发送数据
			m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_EFFICACY_RESULT, &EfficacyResult, sizeof(EfficacyResult),0);

			return true;
		}

		//被冻结
		if (pClubMember->cbMemberStatus == MEMBER_STATUS_FREEZE)
		{
			//设置结果
			EfficacyResult.bResult = false;
			StringCchCopy(EfficacyResult.szErrorMessage, CountArray(EfficacyResult.szErrorMessage), CWHService::FormatMessage(MIM_CLUB_0016, TEXT("您已被茶社老板冻结游戏权限，请联系老板解除！")));			

			//发送数据
			m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_EFFICACY_RESULT, &EfficacyResult, sizeof(EfficacyResult),0);

			return true;
		}

		//设置结果
		EfficacyResult.bResult = true;

		//发送数据
		m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_EFFICACY_RESULT, &EfficacyResult, sizeof(EfficacyResult),0);

		return true;
	}
	case SUB_BC_S_CLUB_SCORE:			//茶社积分
	{
		//提取数据
		ASSERT(wDataSize == sizeof(CMD_BC_S_ClubScore));
		if (wDataSize != sizeof(CMD_BC_S_ClubScore)) return false;

		//提取数据
		auto pClubScore = (CMD_BC_S_ClubScore*)pData;		

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pClubScore->dwClubID);
		if (pClubInfoItem != NULL)
		{
			CString strMessage;
			strMessage.Format(TEXT("ClubID=%d,UserID=%d,ClubScore=%64d"), pClubScore->dwClubID, pClubScore->dwUserID, pClubScore->lClubScore);
			CTraceService::TraceString(strMessage, TraceLevel_Normal);

			//查询成员
			auto pClubMember = pClubInfoItem->SearchMember(pClubScore->dwUserID);
			if (pClubMember != NULL && (pClubScore->lClubScore!=pClubMember->lClubScore || pClubScore->lRevenue!=0))
			{
				//更新分数
				pClubMember->lClubScore = pClubScore->lClubScore;
				pClubMember->lRevenueCount += pClubScore->lRevenue;

				//构造结构
				CMD_CB_S_MemberUpdate MemberUpdate;
				ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));

				//设置变量
				MemberUpdate.dwClubID = pClubScore->dwClubID;
				CopyMemory(&MemberUpdate.ClubMember, pClubMember, sizeof(MemberUpdate.ClubMember));

				//发给自己			
				auto pClubUserItem = m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
				if (pClubUserItem->GetActiveClubID() == pClubScore->dwClubID)
				{
					CTraceService::TraceString(TEXT("发送给玩家%d"), TraceLevel_Normal, pClubMember->dwMemberID);

					//发送数据
					SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), pClubUserItem->GetSocketInfo());
				}

				//变量定义
				POSITION Position = NULL;
				tagClubMemberInfo* pClubMember = NULL;

				do
				{
					//枚举成员
					pClubMember = pClubInfoItem->EnumMember(Position);
					if (pClubMember == NULL) break;

					//馆主或管理员
					if (pClubMember->dwMemberID != MemberUpdate.ClubMember.dwMemberID && (pClubMember->cbMemberType == MEMBER_TYPE_EMPLOYER || pClubMember->cbMemberType == MEMBER_TYPE_MANAGER))
					{
						//查找用户
						auto pClubUserItem = m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
						if (pClubUserItem == NULL || pClubUserItem->GetActiveClubID() != pClubScore->dwClubID) continue;


						CTraceService::TraceString(TEXT("发送给管理员%d"), TraceLevel_Normal, pClubMember->dwMemberID);

						//发送数据
						SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), pClubUserItem->GetSocketInfo());
					}

				} while (Position != NULL);
			}
		}

		return true;
	}
	case SUB_BC_S_CLUB_WEALTH:			//茶社财富
	{
		//提取数据
		ASSERT(wDataSize == sizeof(CMD_BC_S_ClubWealth));
		if (wDataSize != sizeof(CMD_BC_S_ClubWealth)) return false;

		//提取数据
		auto pClubWealth = (CMD_BC_S_ClubWealth*)pData;

		//查询茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pClubWealth->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//查找用户
			auto pClubUserItem = m_ClubUserManager.SearchUserItem(pClubWealth->dwPayUserID);
			if (pClubUserItem != NULL && pClubUserItem->GetActiveClubID() == pClubWealth->dwClubID)
			{
				//定义变量
				auto pWealthUpdate = (CMD_CM_WealthUpdate*)m_cbBuffer;
				ZeroMemory(pWealthUpdate, sizeof(CMD_CM_WealthUpdate));

				//设置变量
				pWealthUpdate->wItemCount = 1;
				pWealthUpdate->WealthItem[0].wMoneyID = GOODS_ID_CARD;
				pWealthUpdate->WealthItem[0].lBalance = pClubWealth->lUserCard;

				//发送数据
				WORD wPacketSize = sizeof(CMD_CM_WealthUpdate) + pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]);
				SendDataToUser(SERVICE_MODULE_CLUBPLAZA, MDM_CM_SYSTEM, SUB_CM_WEALTH_UPDATE, pWealthUpdate, wPacketSize, pClubUserItem->GetSocketInfo());
			}
		}

		return true;
	}
	case SUB_BC_S_BATTLE_BASE_OPTION:	//基础配置
	{
		//提取网络信息
		auto pSocketInfo = (tagSocketInfo*)pData;

		//发送数据
		SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_BATTLE_BASE_OPTION, pSocketInfo+1, wDataSize-sizeof(tagSocketInfo), pSocketInfo);

		return true;
	}
	case SUB_BC_S_BATTLE_GAME_OPTION:	//游戏配置
	{
		//提取网络信息
		auto pSocketInfo = (tagSocketInfo*)pData;

		//发送数据
		SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_BATTLE_GAME_OPTION, pSocketInfo + 1, wDataSize - sizeof(tagSocketInfo), pSocketInfo);

		return true;
	}
	case SUB_BC_S_BATTLE_OPTION_FINISH:	//配置完成
	{
		//提取网络信息
		auto pSocketInfo = (tagSocketInfo*)pData;

		//发送数据
		SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_BATTLE_OPTION_FINISH, NULL, 0, pSocketInfo);

		return true;
	}
	case SUB_BC_S_OPERATE_SUCCESS:		//操作成功
	{
		//提取数据
		auto pOperateSuccess = (CMD_BC_S_OperateSuccess*)pData;

		//网络信息
		auto pSocketInfo = &pOperateSuccess->SocketInfo;
		if (pSocketInfo->wGateID != 0)
		{
			//操作成功
			SendOperateSuccess(pSocketInfo->wGateID, pSocketInfo->dwTokenID, 0, pOperateSuccess->lOperateCode, pOperateSuccess->szDescribeString);
		}

		return true;
	}
	case SUB_BC_S_OPERATE_FAILURE:		//操作失败
	{
		//提取数据
		auto pOperateFailure = (CMD_BC_S_OperateFailure*)pData;

		//网络信息
		auto pSocketInfo = &pOperateFailure->SocketInfo;
		if (pSocketInfo->wGateID != 0)
		{
			//操作失败
			SendOperateFailure(pSocketInfo->wGateID, pSocketInfo->dwTokenID, 0, pOperateFailure->lErrorCode, pOperateFailure->szErrorMessage);
		}
		else
		{
			//房间创建失败
			CTraceService::TraceString(TEXT("茶社[%d]约战房间创建失败(%d)(%s)！"),TraceLevel_Exception, pOperateFailure->dwClubID, pOperateFailure->lErrorCode, pOperateFailure->szErrorMessage);

			//创建房间失败
			if (pOperateFailure->lErrorCode == BATTLE_CODE_CREATE_FAILED)
			{
				auto dwConfigID = pSocketInfo->dwTokenID;

				//查找俱乐部
				auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pOperateFailure->dwClubID);
				if (pClubInfoItem != NULL)
				{
					auto pBattleConfig = pClubInfoItem->SearchConfig(dwConfigID);
					if (pBattleConfig && pBattleConfig->wTableCount > 0)
					{
						pBattleConfig->wTableCount--;
					}					
				}
			}
		}

		return true;
	}
	}

	return true;
}

//茶社服务
bool CAttemperEngineSink::OnTCPNetworkMainClubService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//获取信息
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter * pBindParameter = (m_pBindParameter + wBindIndex);

	//令牌参数
	auto pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	switch (wSubCmdID)
	{
	case SUB_CB_C_ENTER_CLUB:		//进入服务
	{
		//参数校验
		ASSERT (wDataSize==sizeof(CMD_CB_C_EnterClub));
		if (wDataSize!=sizeof(CMD_CB_C_EnterClub)) return false;

		//提取数据
		auto pEnterClub=(CMD_CB_C_EnterClub *)pData;			

		//构造结构
		tagSocketInfo SocketInfo;
		ZeroMemory(&SocketInfo,sizeof(SocketInfo));

		//设置变量
		SocketInfo.dwTokenID=dwTokenID;
		SocketInfo.wGateID=pBindParameter->wServiceID;

		//停止服务
		if (m_bClubSevice==false)
		{
			//构造结构
			CMD_CB_S_EnterFailure EnterFailure;
			ZeroMemory(&EnterFailure,sizeof(EnterFailure));

			//设置变量
			EnterFailure.lErrorCode=LOGON_ECODE_FAILED;
			StringCchCopy(EnterFailure.szErrorDescribe,CountArray(EnterFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUB_0001, TEXT("系统维护中，茶社暂时停止服务。请稍后再次重试！")));

			//发送数据
			WORD wPacketSize=CountStringBuffer(EnterFailure.szErrorDescribe);
			wPacketSize+=sizeof(EnterFailure)-sizeof(EnterFailure.szErrorDescribe);
			SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE,SUB_CB_S_ENTER_FAILURE,&EnterFailure,wPacketSize,&SocketInfo);

			return true;
		}			

		//查找茶社
		auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(pEnterClub->dwClubID);
		if (pClubInfoItem==NULL) 
		{
			//构造结构
			CMD_CB_S_EnterFailure EnterFailure;
			ZeroMemory(&EnterFailure,sizeof(EnterFailure));

			//设置变量
			EnterFailure.lErrorCode=LOGON_ECODE_FAILED;
			StringCchCopy(EnterFailure.szErrorDescribe,CountArray(EnterFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUB_0002, TEXT("系统未查询到该茶社的信息，进入失败！")));			

			//发送数据
			WORD wPacketSize=CountStringBuffer(EnterFailure.szErrorDescribe);
			wPacketSize+=sizeof(EnterFailure)-sizeof(EnterFailure.szErrorDescribe);
			SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE,SUB_CB_S_ENTER_FAILURE,&EnterFailure,wPacketSize,&SocketInfo);

			return true;
		}

		//查找用户
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL || pClubUserItem->GetUserStatus()!=US_CB_ONLINE)
		{
			//构造结构
			CMD_CB_S_EnterFailure EnterFailure;
			ZeroMemory(&EnterFailure, sizeof(EnterFailure));

			//设置变量
			EnterFailure.lErrorCode = LOGON_ECODE_FAILED;
			StringCchCopy(EnterFailure.szErrorDescribe, CountArray(EnterFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUB_0003, TEXT("系统未查询到您的用户信息，进入失败！")));			

			//发送数据
			WORD wPacketSize = CountStringBuffer(EnterFailure.szErrorDescribe);
			wPacketSize += sizeof(EnterFailure) - sizeof(EnterFailure.szErrorDescribe);
			SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_ENTER_FAILURE, &EnterFailure, wPacketSize, &SocketInfo);

			return true;
		}

		//缓存请求
		if (pClubInfoItem->GetClubState() == CLUB_STATE_LOADING)
		{
			//缓存请求
			pClubInfoItem->CacheEnterRequest(SocketInfo);

			//设置茶社
			pClubUserItem->SetActiveClubID(pEnterClub->dwClubID);

			return true;
		}

		//查找成员
		auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL)
		{
			//构造结构
			CMD_CB_S_EnterFailure EnterFailure;
			ZeroMemory(&EnterFailure,sizeof(EnterFailure));

			//设置变量
			EnterFailure.lErrorCode=LOGON_ECODE_FAILED;
			StringCchCopy(EnterFailure.szErrorDescribe,CountArray(EnterFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUB_0004, TEXT("抱歉，你不是该茶社的成员，进入失败！")));			

			//发送数据
			WORD wPacketSize=CountStringBuffer(EnterFailure.szErrorDescribe);
			wPacketSize+=sizeof(EnterFailure)-sizeof(EnterFailure.szErrorDescribe);
			SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE,SUB_CB_S_ENTER_FAILURE,&EnterFailure,wPacketSize,&SocketInfo);

			return true;
		}			

		//发送数据
		SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_ITEM, pClubInfoItem->GetClubItem(), sizeof(tagClubItem), &SocketInfo);

		//发送成员
		SendClubMemberToUser(pEnterClub->dwClubID, &SocketInfo);

		//发送玩法
		SendBattleConfigToUser(pEnterClub->dwClubID, &SocketInfo);

		//发送桌子
		SendBattleTableToUser(pEnterClub->dwClubID, &SocketInfo);

		//发送设置
		SendKindSettingToUser(pEnterClub->dwClubID, &SocketInfo);

		//申请消息
		if (pClubMember->cbMemberType == MEMBER_TYPE_EMPLOYER || (pClubMember->cbMemberType == MEMBER_TYPE_MANAGER && (pClubMember->dwManageRight & MANAGE_RIGHT_AUDIT_APPLY) != 0))
		{
			SendApplyMessageToUser(pEnterClub->dwClubID, &SocketInfo);
		}

		//构造结构
		CMD_CB_S_EnterSuccess EnterSuccess;
		ZeroMemory(&EnterSuccess,sizeof(EnterSuccess));

		//设置变量
		EnterSuccess.dwClubID=pEnterClub->dwClubID;

		//进入成功
		SendDataToUser(SERVICE_MODULE_CLUB,MDM_CLUB_SERVICE,SUB_CB_S_ENTER_SUCCESS,&EnterSuccess,sizeof(EnterSuccess),&SocketInfo);

		//设置状态
		pClubInfoItem->IncreaseOnline();
		pClubInfoItem->SetActiveTime((DWORD)time(NULL));

		//设置茶社
		pClubUserItem->SetActiveClubID(pEnterClub->dwClubID);			

		return true;
	}
	case SUB_CB_C_QUERY_OPTION:		//查询配置
	{
		//参数校验
		ASSERT (wDataSize == sizeof(CMD_CB_C_QueryOption));
		if (wDataSize > sizeof(CMD_CB_C_QueryOption)) return false;

		//提取数据
		auto pQueryOption = (CMD_CB_C_QueryOption*)pData;

		//查找用户
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_CLUB_0003, TEXT("系统未查询到您的用户信息！")));

			return true;
		}

		//查找茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pClubUserItem->GetActiveClubID());
		if (pClubInfoItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_CLUB_0002, TEXT("系统未查询到该茶社的信息！")));

			return true;
		}

		//查找成员
		/*auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL || pClubMember->cbMemberType != MEMBER_TYPE_EMPLOYER)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_CLUB_0006, TEXT("抱歉，您没有权限创建桌子！")));

			return true;
		}*/

		//构造结构
		CMD_BC_C_QueryOption QueryOption = {};
		
		//设置变量
		QueryOption.wKindID = pQueryOption->wKindID;		
		QueryOption.cbEmployScene = pClubInfoItem->GetClubProperty()->wClubKind;
		CopyMemory(&QueryOption.SocketInfo, pClubUserItem->GetSocketInfo(), sizeof(QueryOption.SocketInfo));

		//发送数据
		m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_QUERY_OPTION, &QueryOption, sizeof(QueryOption),0);

		return true;
	}
	case SUB_CB_C_CREATE_TABLE:		//创建房间
	{
		//参数校验
		ASSERT (wDataSize <= sizeof(CMD_CB_C_CreateTable));
		if (wDataSize > sizeof(CMD_CB_C_CreateTable)) return false;

		//提取数据
		auto pCreateTable = (CMD_CB_C_CreateTable *)pData;

		//查找用户
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID,dwTokenID, SUB_CB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_CLUB_0003, TEXT("系统未查询到您的用户信息！")));

			return true;
		}

		//查找茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pCreateTable->dwClubID);
		if (pClubInfoItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_CLUB_0002, TEXT("系统未查询到该茶社的信息！")));
								
			return true;
		}

		//查找成员
		auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL || pClubMember->cbMemberType != MEMBER_TYPE_EMPLOYER)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_CLUB_0006, TEXT("抱歉，您没有权限创建桌子！")));

			return true;
		}		

		//构造结构
		CMD_BC_C_CreateTable CreateTable;
		ZeroMemory(&CreateTable, sizeof(CMD_BC_C_CreateTable));

		//用户信息
		CreateTable.dwUserID= pTokenParameter->dwUserID;		
		
		//茶社信息
		CreateTable.dwClubID = pCreateTable->dwClubID;
		CreateTable.dwClubCreaterID = pClubInfoItem->GetClubItem()->ClubProperty.dwCreaterID;
		CreateTable.wSwitchOption = pClubInfoItem->GetClubRules()->wSwitchOption;

		//约战配置
		CreateTable.dwConfigID = pCreateTable->dwConfigID;
		CopyMemory(&CreateTable.BattleConfig, &pCreateTable->BattleConfig, sizeof(CreateTable.BattleConfig));

		//网络信息
		CopyMemory(&CreateTable.SocketInfo, pClubUserItem->GetSocketInfo(), sizeof(CreateTable.SocketInfo));

		//发送数据
		m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_CREATE_TABLE, &CreateTable, sizeof(CreateTable),0);

		return true;
	}
	case SUB_CB_C_MODIFY_TABLE:		//修改桌子
	{
		//参数校验
		ASSERT (wDataSize <= sizeof(CMD_CB_C_ModifyTable));
		if (wDataSize > sizeof(CMD_CB_C_ModifyTable)) return false;

		//提取数据
		auto pModifyTable = (CMD_CB_C_ModifyTable*)pData;

		//查找用户
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_MODIFY_TABLE, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_CLUB_0003, TEXT("系统未查询到您的用户信息！")));

			return true;
		}

		//查找茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pModifyTable->dwClubID);
		if (pClubInfoItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_MODIFY_TABLE, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_CLUB_0002, TEXT("系统未查询到该茶社的信息！")));

			return true;
		}

		//查找成员
		auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL || pClubMember->cbMemberType != MEMBER_TYPE_EMPLOYER)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_MODIFY_TABLE, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_CLUB_0007, TEXT("抱歉，您没有权限修改房间！")));

			return true;
		}

		//构造结构
		CMD_BC_C_ModifyTable ModifyTable;
		ZeroMemory(&ModifyTable, sizeof(ModifyTable));

		//用户信息
		ModifyTable.dwUserID = pTokenParameter->dwUserID;
		ModifyTable.dwClubID = pModifyTable->dwClubID;
		ModifyTable.dwBattleNum = pModifyTable->dwBattleNum;

		//约战配置
		CopyMemory(&ModifyTable.BattleConfig, &pModifyTable->BattleConfig, sizeof(ModifyTable.BattleConfig));

		//网络信息
		CopyMemory(&ModifyTable.SocketInfo, pClubUserItem->GetSocketInfo(), sizeof(ModifyTable.SocketInfo));

		//发送数据			
		m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_MODIFY_TABLE, &ModifyTable, sizeof(ModifyTable),0);

		return true;
	}
	case SUB_CB_C_DISMISS_TABLE:	//解散桌子
	{
		//参数校验
		ASSERT (wDataSize == sizeof(CMD_CB_C_DismissTable));
		if (wDataSize != sizeof(CMD_CB_C_DismissTable)) return false;

		//提取数据
		auto pDismissTable = (CMD_CB_C_DismissTable*)pData;

		//查找用户
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DISMISS_TABLE, BATTLE_CODE_DISMISS_FAILED, CWHService::FormatMessage(MIM_CLUB_0003, TEXT("系统未查询到您的用户信息！")));

			return true;
		}

		//查找茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pDismissTable->dwClubID);
		if (pClubInfoItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DISMISS_TABLE, BATTLE_CODE_DISMISS_FAILED, CWHService::FormatMessage(MIM_CLUB_0002, TEXT("系统未查询到该茶社的信息！")));

			return true;
		}

		//查找成员
		auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL || (pClubMember->cbMemberType!=MEMBER_TYPE_EMPLOYER && pClubMember->cbMemberType != MEMBER_TYPE_MANAGER))
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DISMISS_TABLE, BATTLE_CODE_DISMISS_FAILED, CWHService::FormatMessage(MIM_CLUB_0008, TEXT("抱歉，您没有权限解散房间！")));

			return true;
		}

		//构造结构
		CMD_BC_C_DismissTable DismissTable;
		ZeroMemory(&DismissTable, sizeof(DismissTable));

		//用户信息
		DismissTable.dwUserID = pTokenParameter->dwUserID;
		DismissTable.dwBattleNum = pDismissTable->dwBattleNum;
		
		//网络信息
		CopyMemory(&DismissTable.SocketInfo, pClubUserItem->GetSocketInfo(), sizeof(DismissTable.SocketInfo));

		//发送数据			
		m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_DISMISS_TABLE, &DismissTable, sizeof(DismissTable),0);

		return true;
	}
	case SUB_CB_C_APPEND_CONFIG:	//添加玩法
	{
		//参数校验
		ASSERT (wDataSize == sizeof(CMD_CB_C_AppendConfig));
		if (wDataSize != sizeof(CMD_CB_C_AppendConfig)) return false;

		//提取数据
		auto pAppendConfig = (CMD_CB_C_AppendConfig*)pData;

		//查找用户
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_APPEND_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0003, TEXT("系统未查询到您的用户信息！")));

			return true;
		}

		//查找茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pAppendConfig->dwClubID);
		if (pClubInfoItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_APPEND_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0002, TEXT("系统未查询到该茶社的信息！")));

			return true;
		}

		//查找成员
		auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL || (pClubMember->cbMemberType != MEMBER_TYPE_EMPLOYER && (pClubMember->dwManageRight & MANAGE_RIGHT_MODIFY_CONFIG) == 0))
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_APPEND_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0009, TEXT("抱歉，您没有权限添加约战玩法！")));

			return true;
		}

		//构造结构
		DBR_GR_AppendConfig AppendConfig;
		ZeroMemory(&AppendConfig, sizeof(AppendConfig));

		//用户信息
		AppendConfig.dwClubID = pAppendConfig->dwClubID;
		AppendConfig.dwUserID = pTokenParameter->dwUserID;		
		
		//玩法转换
		ConvertBattleConfig(&pAppendConfig->BattleConfig, AppendConfig.BattleConfig);

		//投递数据
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_APPEND_CONFIG, AppendConfig.dwClubID, dwSocketID, dwTokenID, &AppendConfig, sizeof(AppendConfig));

		return true;
	}
	case SUB_CB_C_MODIFY_CONFIG:	//修改玩法
	{
		//参数校验
		ASSERT(wDataSize > sizeof(CMD_CB_C_ModifyConfig));
		if (wDataSize <= sizeof(CMD_CB_C_ModifyConfig)) return false;

		//提取数据
		auto pModifyConfig = (CMD_CB_C_ModifyConfig*)pData;

		//查找用户
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_MODIFY_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0003, TEXT("系统未查询到您的用户信息！")));

			return true;
		}

		//查找茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pModifyConfig->dwClubID);
		if (pClubInfoItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_MODIFY_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0002, TEXT("系统未查询到该茶社的信息！")));

			return true;
		}

		//查找成员
		auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL || (pClubMember->cbMemberType != MEMBER_TYPE_EMPLOYER && (pClubMember->dwManageRight & MANAGE_RIGHT_MODIFY_CONFIG) == 0))
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_MODIFY_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0010, TEXT("抱歉，您没有权限修改约战玩法！")));

			return true;
		}

		//构造结构
		DBR_GR_ModifyConfig ModifyConfig;
		ZeroMemory(&ModifyConfig, sizeof(ModifyConfig));

		//用户信息
		ModifyConfig.dwClubID = pModifyConfig->dwClubID;
		ModifyConfig.dwUserID = pTokenParameter->dwUserID;		
		ModifyConfig.cbModifyKind = pModifyConfig->cbModifyKind;
		ModifyConfig.BattleConfig.dwConfigID = pModifyConfig->dwConfigID;

		//变量定义
		auto pModifyData = (PBYTE)(pModifyConfig + 1);
		
		//修改玩法
		if (ModifyConfig.cbModifyKind == CLUB_BC_MK_BATTLE_CONFIG)
		{
			auto pBattleConfig = (tagBattleConfig*)pModifyData;
			ConvertBattleConfig(pBattleConfig, ModifyConfig.BattleConfig);
		}		

		//投递数据
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MODIFY_CONFIG, ModifyConfig.dwClubID, dwSocketID, dwTokenID, &ModifyConfig, sizeof(ModifyConfig));

		return true;
	}
	case SUB_CB_C_DELETE_CONFIG:	//删除玩法
	{
		//参数校验
		ASSERT (wDataSize == sizeof(CMD_CB_C_DeleteConfig));
		if (wDataSize != sizeof(CMD_CB_C_DeleteConfig)) return false;

		//提取数据
		auto pDeleteConfig = (CMD_CB_C_DeleteConfig*)pData;

		//查找用户
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DELETE_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0003, TEXT("系统未查询到您的用户信息！")));

			return true;
		}

		//查找茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pDeleteConfig->dwClubID);
		if (pClubInfoItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DELETE_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0002, TEXT("系统未查询到该茶社的信息！")));

			return true;
		}

		//查找成员
		auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL || (pClubMember->cbMemberType != MEMBER_TYPE_EMPLOYER && (pClubMember->dwManageRight & MANAGE_RIGHT_MODIFY_CONFIG) == 0))
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DELETE_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0011, TEXT("抱歉，您没有权限删除约战玩法！")));

			return true;
		}

		//构造结构
		DBR_GR_DeleteConfig DeleteConfig;
		ZeroMemory(&DeleteConfig, sizeof(DeleteConfig));

		//用户信息
		DeleteConfig.wKindID = pDeleteConfig->wKindID;
		DeleteConfig.dwClubID = pDeleteConfig->dwClubID;
		DeleteConfig.dwUserID = pTokenParameter->dwUserID;		
		DeleteConfig.dwConfigID= pDeleteConfig->dwConfigID;	

		//投递数据
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_CONFIG, DeleteConfig.dwClubID, dwSocketID, dwTokenID, &DeleteConfig, sizeof(DeleteConfig));

		return true;
	}
	case SUB_CB_C_SET_WINNER_SCORE:	//设置赢分
	{
		//参数校验
		ASSERT(wDataSize == sizeof(CMD_CB_C_SetWinnerScore));
		if (wDataSize != sizeof(CMD_CB_C_SetWinnerScore)) return false;

		//提取数据
		auto pSetWinnerScore = (CMD_CB_C_SetWinnerScore*)pData;

		//查找用户
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DELETE_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0003, TEXT("系统未查询到您的用户信息！")));

			return true;
		}

		//查找茶社
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pSetWinnerScore->dwClubID);
		if (pClubInfoItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DELETE_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0002, TEXT("系统未查询到该茶社的信息！")));

			return true;
		}

		//查找成员
	/*	auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL || pClubMember->cbMemberType != MEMBER_TYPE_EMPLOYER)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DELETE_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0018, TEXT("抱歉，您没有权限设置大赢家最低赢分！")));

			return true;
		}*/

		//构造结构
		DBR_GR_SetWinnerScore SetWinnerScore;
		ZeroMemory(&SetWinnerScore, sizeof(SetWinnerScore));

		//用户信息
		SetWinnerScore.wKindID = pSetWinnerScore->wKindID;
		SetWinnerScore.dwClubID = pSetWinnerScore->dwClubID;
		SetWinnerScore.dwUserID = pTokenParameter->dwUserID;
		SetWinnerScore.lMinWinnerScore = pSetWinnerScore->lMinWinnerScore;

		//投递数据
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SET_WINNER_SCORE, SetWinnerScore.dwClubID, dwSocketID, dwTokenID, &SetWinnerScore, sizeof(SetWinnerScore));

		return true;
	}
	}

	return true;
}

//查询桌子
bool CAttemperEngineSink::QueryBattleTable(CClubInfoItem* pClubInfoItem)
{
	//构造对象
	CMD_BC_C_QueryTable QueryTable = {};	
	QueryTable.dwClubID = pClubInfoItem->GetClubID();

	//发送消息
	m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_QUERY_TABLE, &QueryTable, sizeof(QueryTable),0);

	//清除桌数
	INT nIndex = 0;
	do
	{
		auto pBattleConfig = pClubInfoItem->EnumConfig(nIndex++);
		if (pBattleConfig == NULL) break;

		pBattleConfig->wTableCount = 0;

	} while (true);

	//移除桌子
	pClubInfoItem->RemoveClubTable();

	//广播消息
	CMD_CB_S_TableCleanup TableCleanup = {};
	TableCleanup.dwClubID = pClubInfoItem->GetClubID();

	//广播消息
	SendDataToMember(TableCleanup.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_TABLE_CLEANUP, &TableCleanup, sizeof(TableCleanup), LOCATION_MASK_INCLUB);

	return true;
}

//创建桌子
bool CAttemperEngineSink::CreateBattleTable(CClubInfoItem* pClubInfoItem, tagClubBattleConfig* pClubBattleConfig)
{
	//构造结构
	CMD_BC_C_CreateTable CreateTable;
	ZeroMemory(&CreateTable, sizeof(CMD_BC_C_CreateTable));

	//茶社信息
	CreateTable.dwClubID = pClubInfoItem->GetClubID();
	CreateTable.dwClubCreaterID = pClubInfoItem->GetClubItem()->ClubProperty.dwCreaterID;
	CreateTable.wSwitchOption = pClubInfoItem->GetClubRules()->wSwitchOption;

	//约战配置
	CreateTable.dwConfigID = pClubBattleConfig->dwConfigID;
	ConvertBattleConfig(pClubBattleConfig, CreateTable.BattleConfig);

	//特殊赋值
	CreateTable.dwUserID = CreateTable.dwClubCreaterID;
	CreateTable.SocketInfo.dwTokenID = CreateTable.dwConfigID;

	//发送数据
	m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_CREATE_TABLE, &CreateTable, sizeof(CreateTable),0);

	return true;
}

//令牌参数
tagTokenParameter* CAttemperEngineSink::GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID)
{
	//获取信息
	WORD wBindIndex = LOWORD(dwSocketID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);	

	//转发模式
	if (pBindParameter->wServiceModule == SERVICE_MODULE_GATE)
	{
		//查询网关
		auto pGlobalGateItem = (CGlobalGateItem*)pBindParameter->pBindServiceItem;
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

//构造群名片
bool CAttemperEngineSink::ConstructClubNameCard(const tagClubProperty * pClubProperty, tagClubNameCard * pClubNameCard)
{
	//参数校验
	ASSERT (pClubProperty != NULL && pClubNameCard != NULL);
	if (pClubProperty == NULL || pClubNameCard == NULL) return false;

	//茶社属性
	pClubNameCard->dwClubID = pClubProperty->dwClubID;
	pClubNameCard->wClubKind = pClubProperty->wClubKind;
	pClubNameCard->wClubIconID = pClubProperty->wClubIconID;
	pClubNameCard->wMemberCount= pClubProperty->wMemberCount;
	pClubNameCard->wMaxMemberCount = pClubProperty->wMaxMemberCount;
	StringCchCopy(pClubNameCard->szClubName, CountArray(pClubNameCard->szClubName), pClubProperty->szClubName);

	//茶社信息
	pClubNameCard->dwCreaterID = pClubProperty->dwCreaterID;
	pClubNameCard->dwCreaterGameID = pClubProperty->dwCreaterGameID;

	//查询用户
	auto pClubUserItem = m_ClubUserManager.SearchUserItem(pClubNameCard->dwCreaterID);
	if (pClubUserItem != NULL)
	{
		pClubNameCard->wCreaterFaceID = pClubUserItem->GetUserInfo()->wFaceID;
		pClubNameCard->dwCreaterCustomID = pClubUserItem->GetUserInfo()->dwCustomID;
		StringCchCopy(pClubNameCard->szCreaterNickName, CountArray(pClubNameCard->szCreaterNickName), pClubUserItem->GetNickName());
	}

	return true;
}

//茶社成员
bool CAttemperEngineSink::OnDBClubMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验数据
	ASSERT ((wDataSize-sizeof(DBO_GR_ClubMember))%sizeof(tagClubMemberUser)==0);
	if ((wDataSize-sizeof(DBO_GR_ClubMember))%sizeof(tagClubMemberUser)!=0) return false;

	//提取数据
	auto pClubMember=(DBO_GR_ClubMember *)pData;

	//变量定义
	auto pClubMemberUser = (tagClubMemberUser*)(pClubMember + 1);
	auto wMemberCount=(wDataSize-sizeof(DBO_GR_ClubMember))/sizeof(tagClubMemberUser);	

	//查询茶社
	auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(pClubMember->dwClubID);
	if (pClubInfoItem==NULL) return true;

	//变量定义
	for (int i=0; i<wMemberCount; i++)
	{
		//添加成员
		pClubInfoItem->AppendMember(pClubMemberUser->MemberInfo);

		//添加用户
		auto pClubUserItem=m_ClubUserManager.SearchUserItem(pClubMemberUser->UserInfo.dwUserID);
		if (pClubUserItem!=NULL)
		{		
			//用户信息
			auto pIMUserInfo = pClubUserItem->GetUserInfo();

			//更新信息
			pIMUserInfo->wFaceID = pClubMemberUser->UserInfo.wFaceID;
			pIMUserInfo->cbGender = pClubMemberUser->UserInfo.cbGender;
			pIMUserInfo->dwCustomID = pClubMemberUser->UserInfo.dwCustomID;						
			StringCchCopy(pIMUserInfo->szNickName, CountArray(pIMUserInfo->szNickName), pClubMemberUser->UserInfo.szNickName);
			StringCchCopy(pIMUserInfo->szUserHeadUrl, CountArray(pIMUserInfo->szUserHeadUrl), pClubMemberUser->UserInfo.szUserHeadUrl);//kk jia
		}
		else
		{
			tagSocketInfo SocketInfo = {};
			m_ClubUserManager.InsertUserItem(pClubMemberUser->UserInfo, SocketInfo);
			/*CString s;
			s.Format(_T("插入用户4 szNickName:%s,szUserHeadUrl:%s"), pClubMemberUser->UserInfo.szNickName, pClubMemberUser->UserInfo.szUserHeadUrl);
			CTraceService::TraceString(s, TraceLevel_Exception);*/
		}

		//指针递增
		pClubMemberUser++;
	}

	return true;
}

//类型设置
bool CAttemperEngineSink::OnDBKindSetting(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//校验数据
	ASSERT(wDataSize>=sizeof(DBO_GR_ClubKindSetting));
	if (wDataSize<sizeof(DBO_GR_ClubKindSetting)) return false;

	//提取数据
	auto pClubKindSetting = (DBO_GR_ClubKindSetting*)pData;

	//查询茶社
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pClubKindSetting->dwClubID);
	if (pClubInfoItem == NULL) return true;

	//变量定义
	for (int i = 0; i < pClubKindSetting->wItemCount; i++)
	{
		//添加设置
		pClubInfoItem->AppendKindSetting(pClubKindSetting->KindSettingItem[i]);		
	}

	return true;
}

//约战玩法
bool CAttemperEngineSink::OnDBBattleConfig(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验数据
	ASSERT ((wDataSize-sizeof(DBO_GR_BattleConfig))%sizeof(tagClubBattleConfig)==0);
	if ((wDataSize-sizeof(DBO_GR_BattleConfig))%sizeof(tagClubBattleConfig)!=0) return false;

	//提取数据
	auto pBattleConfig=(DBO_GR_BattleConfig *)pData;

	//变量定义
	auto pIMBattleConfig = (tagClubBattleConfig*)(pBattleConfig + 1);
	auto wConfigCount=(wDataSize-sizeof(DBO_GR_BattleConfig))/sizeof(tagClubBattleConfig);	

	//查询茶社
	auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(pBattleConfig->dwClubID);
	if (pClubInfoItem==NULL) return false;

	//添加玩法
	for (int i=0; i<wConfigCount; i++)
	{		
		pClubInfoItem->AppendConfig(*pIMBattleConfig++);		
	}

	return true;
}

//申请消息
bool CAttemperEngineSink::OnDBApplyMessage(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验数据
	ASSERT ((wDataSize - sizeof(DBO_GR_ApplyMessage)) % sizeof(tagClubApplyMessage) == 0);
	if ((wDataSize - sizeof(DBO_GR_ApplyMessage)) % sizeof(tagClubApplyMessage) != 0) return false;

	//提取数据
	auto pApplyMessage = (DBO_GR_ApplyMessage *)pData;

	//变量定义
	auto pIMApplyMessage = (tagClubApplyMessage*)(pApplyMessage + 1);
	auto wMessageCount = (wDataSize - sizeof(DBO_GR_ApplyMessage)) / sizeof(tagClubApplyMessage);	

	//查询茶社
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pApplyMessage->dwClubID);
	if (pClubInfoItem == NULL) return false;

	//添加消息
	for (int i=0; i<wMessageCount; i++)
	{		
		pClubInfoItem->AppendMessage(*pIMApplyMessage++);
	}

	return true;
}

//更新成员
bool CAttemperEngineSink::OnDBUpdateMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT (wDataSize==sizeof(DBO_GR_UpdateMember));
	if (wDataSize!=sizeof(DBO_GR_UpdateMember)) return false;

	//提取数据
	auto pUpdateMember=(DBO_GR_UpdateMember *)pData;

	//查找茶社
	auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(pUpdateMember->dwClubID);
	if (pClubInfoItem!=NULL)
	{
		//茶馆成员
		auto pClubMember=pClubInfoItem->SearchMember(pUpdateMember->ClubMember.dwMemberID);
		if (pClubMember!=NULL)
		{
			//构造结构
			CMD_CB_S_MemberUpdate MemberUpdate;
			ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));

			//设置变量
			MemberUpdate.dwClubID = pUpdateMember->dwClubID;

			//更新信息
			if (pUpdateMember->cbUpdateKind == UPMEMBER_KIND_BATTLE)
			{
				//pClubMember->lClubScore = pUpdateMember->ClubMember.lClubScore;
				//pClubMember->lRevenueCount = pUpdateMember->ClubMember.lRevenueCount;
				pClubMember->dwBattleCount = pUpdateMember->ClubMember.dwBattleCount;
				pClubMember->dwCreateCount = pUpdateMember->ClubMember.dwCreateCount;
				pClubMember->LastBattleTime = pUpdateMember->ClubMember.LastBattleTime;				

				//设置变量
				CopyMemory(&MemberUpdate.ClubMember, pClubMember, sizeof(MemberUpdate.ClubMember));

				//发送数据
				SendDataToMember(MemberUpdate.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), LOCATION_MASK_INCLUB);
			}
			else
			{
				pClubMember->lClubScore = pUpdateMember->ClubMember.lClubScore;
				pClubMember->lRevenueCount = pUpdateMember->ClubMember.lRevenueCount;
				pClubMember->cbMemberType = pUpdateMember->ClubMember.cbMemberType;
				pClubMember->dwManageRight = pUpdateMember->ClubMember.dwManageRight;
				pClubMember->cbMemberStatus = pUpdateMember->ClubMember.cbMemberStatus;				
				pClubMember->dwBattleCount = pUpdateMember->ClubMember.dwBattleCount;
				pClubMember->dwCreateCount = pUpdateMember->ClubMember.dwCreateCount;
				pClubMember->LastBattleTime = pUpdateMember->ClubMember.LastBattleTime;

				//设置变量
				CopyMemory(&MemberUpdate.ClubMember, pClubMember, sizeof(MemberUpdate.ClubMember));

				//发给自己			
				auto pClubUserItem = m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
				if (pClubUserItem->GetActiveClubID() == pUpdateMember->dwClubID)
				{
					//发送数据
					SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), pClubUserItem->GetSocketInfo());
				}

				//变量定义
				POSITION Position = NULL;
				tagClubMemberInfo* pClubMember = NULL;

				do
				{
					//枚举成员
					pClubMember = pClubInfoItem->EnumMember(Position);
					if (pClubMember == NULL) break;

					//馆主或管理员
					if (pClubMember->dwMemberID != pUpdateMember->ClubMember.dwMemberID && (pClubMember->cbMemberType == MEMBER_TYPE_EMPLOYER || pClubMember->cbMemberType == MEMBER_TYPE_MANAGER))
					{
						//查找用户
						auto pClubUserItem = m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
						if (pClubUserItem == NULL || pClubUserItem->GetActiveClubID() != pUpdateMember->dwClubID) continue;

						//发送数据
						SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), pClubUserItem->GetSocketInfo());
					}

				} while (Position != NULL);
			}			
		}
	}

	return true;
}

//添加玩法
bool CAttemperEngineSink::OnDBAppendConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//校验参数
	ASSERT(wDataSize == sizeof(DBO_GR_AppendConfig));
	if (wDataSize != sizeof(DBO_GR_AppendConfig)) return false;

	//提取数据
	auto pAppendConfig = (DBO_GR_AppendConfig*)pData;

	//查询茶社
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pAppendConfig->dwClubID);
	if (pClubInfoItem != NULL)
	{
		//添加玩法
		pClubInfoItem->AppendConfig(pAppendConfig->BattleConfig);

		//构造结构
		CMD_CB_S_ConfigAppend ConfigAppend;
		ZeroMemory(&ConfigAppend, sizeof(ConfigAppend));

		//设置变量
		ConfigAppend.dwClubID = pAppendConfig->dwClubID;
		CopyMemory(&ConfigAppend.BattleConfig, &pAppendConfig->BattleConfig, sizeof(ConfigAppend.BattleConfig));

		//发送数据
		SendDataToMember(pAppendConfig->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CONFIG_APPEND, &ConfigAppend, sizeof(ConfigAppend), LOCATION_MASK_INCLUB);
	}

	return true;
}

//修改玩法
bool CAttemperEngineSink::OnDBModifyConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//校验参数
	ASSERT(wDataSize == sizeof(DBO_GR_ModifyConfig));
	if (wDataSize != sizeof(DBO_GR_ModifyConfig)) return false;

	//提取数据
	auto pModifyConfig = (DBO_GR_ModifyConfig*)pData;

	//查询茶社
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pModifyConfig->dwClubID);
	if (pClubInfoItem != NULL)
	{
		//修改玩法
		auto pIMBattleConfig = pClubInfoItem->SearchConfig(pModifyConfig->BattleConfig.dwConfigID);
		if (pIMBattleConfig != NULL)
		{
			CopyMemory(pIMBattleConfig, &pModifyConfig->BattleConfig, sizeof(tagClubBattleConfig));
		}

		//构造结构
		CMD_CB_S_ConfigModify ConfigModify;
		ZeroMemory(&ConfigModify, sizeof(ConfigModify));

		//设置变量
		ConfigModify.dwClubID = pModifyConfig->dwClubID;
		ConfigModify.cbModifyKind = pModifyConfig->cbModifyKind;
		CopyMemory(&ConfigModify.BattleConfig, &pModifyConfig->BattleConfig, sizeof(ConfigModify.BattleConfig));

		//发送数据
		SendDataToMember(ConfigModify.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CONFIG_MODIFY, &ConfigModify, sizeof(ConfigModify), LOCATION_MASK_INCLUB);
	}
	

	return true;
}

//删除玩法
bool CAttemperEngineSink::OnDBDeleteConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//校验参数
	ASSERT(wDataSize == sizeof(DBO_GR_DeleteConfig));
	if (wDataSize != sizeof(DBO_GR_DeleteConfig)) return false;

	//提取数据
	auto pDeleteConfig = (DBO_GR_DeleteConfig*)pData;

	//查询茶社
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pDeleteConfig->dwClubID);
	if (pClubInfoItem != NULL)
	{
		//移除玩法
		pClubInfoItem->RemoveConfig(pDeleteConfig->dwConfigID);

		//构造结构
		CMD_CB_S_ConfigDelete ConfigDelete;
		ZeroMemory(&ConfigDelete, sizeof(ConfigDelete));

		//设置变量
		ConfigDelete.wKindID = pDeleteConfig->wKindID;
		ConfigDelete.dwClubID = pDeleteConfig->dwClubID;
		ConfigDelete.dwConfigID = pDeleteConfig->dwConfigID;

		//发送数据
		SendDataToMember(ConfigDelete.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CONFIG_DELETE, &ConfigDelete, sizeof(ConfigDelete), LOCATION_MASK_INCLUB);
	}	

	return true;
}

//操作失败
bool CAttemperEngineSink::OnDBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//校验参数
	ASSERT(wDataSize <= sizeof(DBO_GR_OperateFailure));
	if (wDataSize > sizeof(DBO_GR_OperateFailure)) return false;

	//获取信息
	auto wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);

	//提取数据
	auto pOperateFailure = (DBO_GR_OperateFailure*)pData;

	//构造消息
	//CMD_CM_SystemMessage SystemMessage;
	//ZeroMemory(&SystemMessage, sizeof(SystemMessage));

	////变量定义						
	//SystemMessage.wType = SMT_EJECT;
	//StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), pOperateFailure->szErrorMessage);

	////发送消息
	//SystemMessage.wLength = CountStringBuffer(SystemMessage.szString);
	//WORD wPacketSize = sizeof(SystemMessage) - sizeof(SystemMessage.szString) + SystemMessage.wLength;
	//SendDataToGate(pBindParameter->wServiceID, SERVICE_MODULE_CLUB, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wPacketSize, &dwTokenID, 1);

	SendOperateFailure(pBindParameter->wServiceID, dwTokenID,0, pOperateFailure->lErrorCode, pOperateFailure->szErrorMessage);

	return true;
}

//设置修改
bool CAttemperEngineSink::OnDBKindSettingModify(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//校验参数
	ASSERT(wDataSize == sizeof(DBO_GR_KindSettingModify));
	if (wDataSize != sizeof(DBO_GR_KindSettingModify)) return false;

	//提取数据
	auto pKindSettingModify = (DBO_GR_KindSettingModify*)pData;

	//查询茶社
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pKindSettingModify->dwClubID);
	if (pClubInfoItem != NULL)
	{
		//修改设置
		pClubInfoItem->AppendKindSetting(pKindSettingModify->KindSettingItem);

		//发送数据
		SendDataToMember(pKindSettingModify->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_KIND_SETTING_MODIFY, &pKindSettingModify->KindSettingItem, sizeof(tagClubKindSetting), LOCATION_MASK_INCLUB);
	}

	return true;
}

//茶社属性
bool CAttemperEngineSink::OnDBClubList(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验数据
	ASSERT (wDataSize%sizeof(tagClubItem)==0);
	if (wDataSize%sizeof(tagClubItem)!=0) return false;

	//变量定义
	auto pIMClubItem = (tagClubItem*)pData;
	auto wClubCount=wDataSize/sizeof(tagClubItem);	

	//变量定义
	for (int i=0; i<wClubCount; i++)
	{
		auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(pIMClubItem->ClubProperty.dwClubID);
		if (pClubInfoItem!=NULL)
		{
			CopyMemory(pClubInfoItem->GetClubItem(),pIMClubItem,sizeof(tagClubItem));
		}
		else
		{
			m_ClubInfoManager.ActiveClubItem(*pIMClubItem);
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//加载成员

		//构造结构
		DBR_GR_LoadClubMember LoadClubMember;
		LoadClubMember.dwClubID=pIMClubItem->ClubProperty.dwClubID;

		//加载成员
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_CLUB_MEMBER, 0, 0, 0, &LoadClubMember, sizeof(LoadClubMember));

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//加载设置

		//构造结构
		DBR_GR_LoadKindSetting LoadKindSetting;
		LoadKindSetting.dwClubID = pIMClubItem->ClubProperty.dwClubID;

		//加载成员
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_KIND_SETTING, 0, 0, 0, &LoadKindSetting, sizeof(LoadKindSetting));

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//加载玩法

		//构造结构
		DBR_GR_LoadBattleConfig LoadBattleConfig;
		LoadBattleConfig.dwClubID=pIMClubItem->ClubProperty.dwClubID;

		//加载成员
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_BATTLE_CONFIG,0,0,0,&LoadBattleConfig,sizeof(LoadBattleConfig));		

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//加载消息

		//构造结构
		DBR_GR_LoadApplyMessage LoadApplyMessage;
		LoadApplyMessage.dwClubID = pIMClubItem->ClubProperty.dwClubID;

		//加载成员
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_APPLY_MESSAGE, 0, 0, 0, &LoadApplyMessage, sizeof(LoadApplyMessage));

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//指针递增
		pIMClubItem++;
	}

	return true;
}

//列表完成
bool CAttemperEngineSink::OnDBClubListFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	return true;
}

//茶社子项
bool CAttemperEngineSink::OnDBClubItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验数据
	ASSERT (wDataSize==sizeof(tagClubItem));
	if (wDataSize!=sizeof(tagClubItem)) return false;

	//变量定义
	auto pIMClubItem=(tagClubItem *)pData;

	//查询茶馆
	auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(pIMClubItem->ClubProperty.dwClubID);
	if (pClubInfoItem!=NULL)
	{
		CopyMemory(pClubInfoItem->GetClubItem(),pIMClubItem,sizeof(tagClubItem));
	}
	else
	{
		m_ClubInfoManager.ActiveClubItem(*pIMClubItem);
	}

	return true;
}

//子项完成
bool CAttemperEngineSink::OnDBClubItemFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT (wDataSize==sizeof(DBO_GR_ClubItemFinish));
	if (wDataSize != sizeof(DBO_GR_ClubItemFinish)) return false;

	//提取数据
	auto pClubItemFinish  = (DBO_GR_ClubItemFinish*)pData;

	//查询茶馆
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pClubItemFinish->dwClubID);
	if (pClubInfoItem != NULL)
	{
		//设置时间
		pClubInfoItem->SetActiveTime((DWORD)time(NULL));

		//设置状态
		pClubInfoItem->SetClubState(CLUB_STATE_ONLINE);		

		//遍历定义
		WORD wIndex = 0;		
		tagSocketInfo* pSocketInfo = NULL;

		do
		{
			//枚举请求
			pSocketInfo = pClubInfoItem->EnumEnterRequest(wIndex++);
			if (pSocketInfo == NULL) break;

			//查询网关
			auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pSocketInfo->wGateID);
			if (pGlobalGateItem == NULL) continue;

			//令牌参数
			auto pTokenParameter = GetTokenParameter(pGlobalGateItem->GetSocketID(), pSocketInfo->dwTokenID);
			if (pTokenParameter != NULL)
			{
				//成员信息
				auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
				if (pClubMember == NULL)
				{
					//构造结构
					CMD_CB_S_EnterFailure EnterFailure;
					ZeroMemory(&EnterFailure, sizeof(EnterFailure));

					//设置变量
					EnterFailure.lErrorCode = LOGON_ECODE_FAILED;
					StringCchCopy(EnterFailure.szErrorDescribe, CountArray(EnterFailure.szErrorDescribe), TEXT("抱歉，你不是该茶社的成员，进入失败！"));

					//发送数据
					WORD wPacketSize = CountStringBuffer(EnterFailure.szErrorDescribe);
					wPacketSize += sizeof(EnterFailure) - sizeof(EnterFailure.szErrorDescribe);
					SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_ENTER_FAILURE, &EnterFailure, wPacketSize, pSocketInfo);
				}
				else
				{
					//用户信息
					auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
					if (pClubUserItem != NULL && pClubUserItem->GetActiveClubID() == pClubItemFinish->dwClubID)
					{
						//发送数据
						SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_ITEM, pClubInfoItem->GetClubItem(), sizeof(tagClubItem), pSocketInfo);

						//发送成员
						SendClubMemberToUser(pClubInfoItem->GetClubID(), pSocketInfo);

						//发送玩法
						SendBattleConfigToUser(pClubInfoItem->GetClubID(), pSocketInfo);

						//发送设置
						SendKindSettingToUser(pClubInfoItem->GetClubID(), pSocketInfo);

						//申请消息
						if (pClubMember->cbMemberType == MEMBER_TYPE_EMPLOYER || (pClubMember->cbMemberType == MEMBER_TYPE_MANAGER && (pClubMember->dwManageRight & MANAGE_RIGHT_AUDIT_APPLY) != 0))
						{
							SendApplyMessageToUser(pClubInfoItem->GetClubID(), pSocketInfo);
						}

						//构造结构
						CMD_CB_S_EnterSuccess EnterSuccess = {};
						
						//设置变量
						EnterSuccess.dwClubID = pClubInfoItem->GetClubID();

						//发送消息
						SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_ENTER_SUCCESS, &EnterSuccess, sizeof(EnterSuccess), pSocketInfo);
					}					
				}
			}			

		} while (true);		
	}	

	//激活路由
	if (pClubItemFinish->bActiveRoute == true)
	{
		//构造结构
		CMD_CS_C_UpdateRouteItem UpdateRouteItem = {};

		//设置变量
		UpdateRouteItem.dwRouteID = pClubItemFinish->dwClubID;
		UpdateRouteItem.cbRouteState = pClubInfoItem ? ROUTE_STATE_SERVICE : ROUTE_STATE_NULL;

		//发送消息
		m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_UPDATE_ROUTE_ITEM, &UpdateRouteItem, sizeof(UpdateRouteItem), 0);

		//查询桌子
		QueryBattleTable(pClubInfoItem);
	}

	return true;
}

//转换配置
bool CAttemperEngineSink::ConvertBattleConfig(tagClubBattleConfig* pBattleConfig, tagBattleConfig& BattleConfig)
{
	//校验参数
	ASSERT (pBattleConfig != NULL);
	if (pBattleConfig == NULL) return false;

	//获取对象
	auto JsonValuePtr = CWHJson::getValue(string(CT2CA(pBattleConfig->szBattleConfig)));
	if (JsonValuePtr->getType() != eJsonTypeObj) return false;

	//转换对象
	auto JsonValueObjPtr = CJsonValueObjPtr::dynamicCast(JsonValuePtr);

	//类型标识
	if (JsonValueObjPtr->value["KindID"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.wKindID, JsonValueObjPtr->value["KindID"], false);
	}

	//游戏人数
	if (JsonValueObjPtr->value["PlayerCount"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.wPlayerCount, JsonValueObjPtr->value["PlayerCount"], false);
	}

	//使用场景
	if (JsonValueObjPtr->value["EmployScene"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.cbEmployScene, JsonValueObjPtr->value["EmployScene"], false);
	}

	//分数类型
	if (JsonValueObjPtr->value["ScoreKind"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.cbScoreKind, JsonValueObjPtr->value["ScoreKind"], false);
	}

	//入场费
	if (JsonValueObjPtr->value["EntryCost"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.lEntryCost, JsonValueObjPtr->value["EntryCost"], false);
	}

	//结算方式
	if (JsonValueObjPtr->value["SettleKind"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.cbSettleKind, JsonValueObjPtr->value["SettleKind"], false);
	}

	//游戏局数
	if (JsonValueObjPtr->value["PlayCount"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.wPlayCount, JsonValueObjPtr->value["PlayCount"], false);
	}

	//游戏底分
	if (JsonValueObjPtr->value["BaseScore"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.lBaseScore, JsonValueObjPtr->value["BaseScore"], false);
	}

	//kk jia start
	//最低
	if (JsonValueObjPtr->value["lMinWin"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.lMinWin, JsonValueObjPtr->value["lMinWin"], false);
	}

	//2
	if (JsonValueObjPtr->value["lWin2"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.lWin2, JsonValueObjPtr->value["lWin2"], false);
	}
	//"AllowPublic" : 0, "lMinWin" : 2, "lWin2" : 3, "lWin3" : 4, "lRevenueRate1" : 5, "lRevenueRate2" : 6, "lRevenueRate3" : 7,
  //3
	if (JsonValueObjPtr->value["lWin3"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.lWin3, JsonValueObjPtr->value["lWin3"], false);
	}

	//shui1
	if (JsonValueObjPtr->value["lRevenueRate1"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.lRevenueRate1, JsonValueObjPtr->value["lRevenueRate1"], false);
	}

	//shui2
	if (JsonValueObjPtr->value["lRevenueRate2"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.lRevenueRate2, JsonValueObjPtr->value["lRevenueRate2"], false);
	}

	//shui3
	if (JsonValueObjPtr->value["lRevenueRate3"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.lRevenueRate3, JsonValueObjPtr->value["lRevenueRate3"], false);
	}

	//kk jia end

	//游戏底分
	if (JsonValueObjPtr->value["PayType"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.wPayType, JsonValueObjPtr->value["PayType"], false);
	}

	//货币类型
	if (JsonValueObjPtr->value["CurrencyKind"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.cbCurrencyKind, JsonValueObjPtr->value["CurrencyKind"], false);
	}

	//允许语音
	if (JsonValueObjPtr->value["AllowVoice"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.cbAllowVoice, JsonValueObjPtr->value["AllowVoice"], false);
	}

	//允许同IP
	if (JsonValueObjPtr->value["AllowSameIP"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.cbAllowSameIP, JsonValueObjPtr->value["AllowSameIP"], false);
	}

	//允许公开显示
	if (JsonValueObjPtr->value["AllowPublic"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.cbAllowPublic, JsonValueObjPtr->value["AllowPublic"], false);
	}

	//旁观模式
	if (JsonValueObjPtr->value["LookonMode"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.cbLookonMode, JsonValueObjPtr->value["LookonMode"], false);
	}

	//游戏配置
	if (JsonValueObjPtr->value["GameOption"].get() != NULL)
	{
		string sGameOption;
		CHAR szHexValue[3] = {};
		CWHJsonInput::readJson(sGameOption, JsonValueObjPtr->value["GameOption"], false);

		for (int i = 0; i < sGameOption.size(); i += 2)
		{
			lstrcpynA(szHexValue, &sGameOption[i], CountArray(szHexValue));
			BattleConfig.cbGameOption[BattleConfig.cbOptionSize++] = (BYTE)strtol(szHexValue, NULL, 16);
		}
	}

	//高级配置
	if (JsonValueObjPtr->value["AdvancedOption"].get() != NULL)
	{
		string sAdvancedOption;
		CHAR szHexValue[3] = {};
		CWHJsonInput::readJson(sAdvancedOption, JsonValueObjPtr->value["AdvancedOption"], false);

		for (int i = 0; i < sAdvancedOption.size(); i += 2)
		{
			lstrcpynA(szHexValue, &sAdvancedOption[i], CountArray(szHexValue));
			BattleConfig.cbAdvancedOption[BattleConfig.cbAdvancedSize++] = (BYTE)strtol(szHexValue, NULL, 16);
		}
	}

	return true;
}

//转换配置
bool CAttemperEngineSink::ConvertBattleConfig(tagBattleConfig* pBattleConfig, tagClubBattleConfig& BattleConfig)
{
	//变量定义
	CString strOptionItem;
	CString strGameOption = TEXT("");
	CString strAdvancedOption = TEXT("");

	//游戏配置
	for (BYTE i = 0; i < pBattleConfig->cbOptionSize; i++)
	{
		strOptionItem.Format(TEXT("%02X"), pBattleConfig->cbGameOption[i]);
		strGameOption.Append(strOptionItem);
	}

	//特殊处理
	if (pBattleConfig->cbAdvancedSize == 0)
	{
		ZeroMemory(pBattleConfig->cbAdvancedOption, sizeof(pBattleConfig->cbAdvancedOption));		
	}	
	else
	{
		//调整配置
		auto pAdvancedOption = (tagAdvancedOption*)pBattleConfig->cbAdvancedOption;
		if (pBattleConfig->cbEmployScene == CLUB_KIND_NORMAL)
		{
			if (pAdvancedOption->lRevenueRate != 0) pAdvancedOption->lRevenueRate = 0;
			if (pAdvancedOption->lMinWinScore != 0) pAdvancedOption->lMinWinScore = 0;
		}
		else
		{
			if (pAdvancedOption->lMinWinScore < 0) pAdvancedOption->lMinWinScore = 0;
			if (pAdvancedOption->lRevenueRate > 1000) pAdvancedOption->lRevenueRate = 1000;
		}

		//高级配置
		for (BYTE i = 0; i < pBattleConfig->cbAdvancedSize; i++)
		{
			strOptionItem.Format(TEXT("%02X"), pBattleConfig->cbAdvancedOption[i]);
			strAdvancedOption.Append(strOptionItem);
		}
	}

	//设置变量
	BattleConfig.wKindID = pBattleConfig->wKindID;		

	//变量定义	
	//LPCTSTR pszConfigFormat = TEXT("{\"KindID\":%d,\"ScoreKind\":%d,\"PlayerCount\":%d,\"EmployScene\":%d,\"EntryCost\":%I64d,\"SettleKind\":%d,\"PlayCount\":%d, \
	//	\"BaseScore\":%I64d,\"PayType\":%d,\"CurrencyKind\":%d,\"AllowVoice\":%d,\"AllowSameIP\":%d,\"LookonMode\":%d,\"AllowPublic\":%d,\"GameOption\":\"%s\",\"AdvancedOption\":\"%s\"}");

	////格式配置
	//_sntprintf_s(BattleConfig.szBattleConfig, CountArray(BattleConfig.szBattleConfig), pszConfigFormat, pBattleConfig->wKindID, pBattleConfig->cbScoreKind, pBattleConfig->wPlayerCount, pBattleConfig->cbEmployScene,
	//	pBattleConfig->lEntryCost, pBattleConfig->cbSettleKind, pBattleConfig->wPlayCount, pBattleConfig->lBaseScore, pBattleConfig->wPayType, pBattleConfig->cbCurrencyKind, pBattleConfig->cbAllowVoice, pBattleConfig->cbAllowSameIP,
	//	pBattleConfig->cbLookonMode, pBattleConfig->cbAllowPublic, strGameOption,strAdvancedOption);	


	//变量定义	kk gai
	LPCTSTR pszConfigFormat = TEXT("{\"KindID\":%d,\"ScoreKind\":%d,\"PlayerCount\":%d,\"EmployScene\":%d,\"EntryCost\":%I64d,\"SettleKind\":%d,\"PlayCount\":%d, \
		\"BaseScore\":%I64d,\"PayType\":%d,\"CurrencyKind\":%d,\"AllowVoice\":%d,\"AllowSameIP\":%d,\"LookonMode\":%d,\"AllowPublic\":%d,\"lMinWin\":%I64d,\"lWin2\":%I64d,\"lWin3\":%I64d,\"lRevenueRate1\":%I64d,\"lRevenueRate2\":%I64d,\"lRevenueRate3\":%I64d,\"GameOption\":\"%s\",\"AdvancedOption\":\"%s\"}");

	//格式配置 kk gai
	_sntprintf_s(BattleConfig.szBattleConfig, CountArray(BattleConfig.szBattleConfig), pszConfigFormat, pBattleConfig->wKindID, pBattleConfig->cbScoreKind, pBattleConfig->wPlayerCount, pBattleConfig->cbEmployScene,
		pBattleConfig->lEntryCost, pBattleConfig->cbSettleKind, pBattleConfig->wPlayCount, pBattleConfig->lBaseScore, pBattleConfig->wPayType, pBattleConfig->cbCurrencyKind, pBattleConfig->cbAllowVoice, pBattleConfig->cbAllowSameIP,
		pBattleConfig->cbLookonMode, pBattleConfig->cbAllowPublic, pBattleConfig->lMinWin, pBattleConfig->lWin2, pBattleConfig->lWin3, pBattleConfig->lRevenueRate1, pBattleConfig->lRevenueRate2, pBattleConfig->lRevenueRate3, strGameOption, strAdvancedOption);


	return true;
}

//发送成员
bool CAttemperEngineSink::SendClubMemberToMember(DWORD dwClubID)
{
	//查找网关
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(dwClubID);
	if (pClubInfoItem == NULL) return false;

	//变量定义
	tagClubMemberInfo * pClubMember = NULL;
	CClubUserItem * pClubUserItem = NULL;
	tagClubMemberUser * pClubMemberUser = NULL;

	/*CString s;
	s.Format(_T("1 sizeof(tagClubMemberUser)%d"), sizeof(tagClubMemberUser));
	CTraceService::TraceString(s, TraceLevel_Exception, dwClubID);*/


	//变量定义
	POSITION Position = NULL;
	WORD wPacketSize = sizeof(CMD_CB_S_ClubMember);

	//成员列表
	auto pClubMemberList = (CMD_CB_S_ClubMember*)m_cbDataBuffer;
	pClubMemberList->dwClubID = dwClubID;

	do
	{
		//枚举成员
		pClubMember = pClubInfoItem->EnumMember(Position);
		if (pClubMember == NULL) break;

		//查找用户
		pClubUserItem = m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
		if (pClubUserItem == NULL) continue;

		//发送判断
		if (wPacketSize + sizeof(tagClubMemberUser) > sizeof(m_cbDataBuffer))
		{
			SendDataToMember(dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_MEMBER, m_cbDataBuffer, wPacketSize, LOCATION_MASK_INCLUB);
			wPacketSize = sizeof(CMD_CB_S_ClubMember);
		}

		//获取对象
		pClubMemberUser = (tagClubMemberUser *)&m_cbDataBuffer[wPacketSize];

		//拷贝数据		
		CopyMemory(&pClubMemberUser->MemberInfo, pClubMember, sizeof(pClubMemberUser->MemberInfo));
		CopyMemory(&pClubMemberUser->UserInfo, pClubUserItem->GetUserInfo(), sizeof(pClubMemberUser->UserInfo));

		//设置变量
		wPacketSize += sizeof(tagClubMemberUser);

	} while (Position!=NULL);
	//CString s;
	/*s.Format(_T("2 sizeof(tagClubMemberUser)%d"), sizeof(tagClubMemberUser));
	CTraceService::TraceString(s, TraceLevel_Exception, dwClubID);*/
	//发送数据
	if (wPacketSize > 0)
	{
		SendDataToMember(dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_MEMBER, m_cbDataBuffer, wPacketSize, LOCATION_MASK_INCLUB);
	}

	return true;
}

//发送玩法
bool CAttemperEngineSink::SendClubMemberToUser(DWORD dwClubID,tagSocketInfo *pUserSocketInfo)
{
	//查找网关
	auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(dwClubID);
	if (pClubInfoItem==NULL) return false; 

	//变量定义
	tagClubMemberInfo * pClubMember=NULL;
	CClubUserItem * pClubUserItem=NULL;
	tagClubMemberUser * pClubMemberUser=NULL;

	//变量定义
	POSITION Position = NULL;
	WORD wPacketSize=sizeof(CMD_CB_S_ClubMember);	

	//成员列表
	auto pClubMemberList = (CMD_CB_S_ClubMember*)m_cbDataBuffer;
	pClubMemberList->dwClubID = dwClubID;

	do
	{
		//枚举成员
		pClubMember=pClubInfoItem->EnumMember(Position);
		if (pClubMember==NULL) break;

		//查找用户
		pClubUserItem=m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
		if (pClubUserItem==NULL) continue;		

		//发送判断
		if (wPacketSize+sizeof(tagClubMemberUser)>sizeof(m_cbDataBuffer))
		{
			SendDataToGate(pUserSocketInfo->wGateID,SERVICE_MODULE_CLUB,MDM_CLUB_SERVICE,SUB_CB_S_CLUB_MEMBER,m_cbDataBuffer,wPacketSize,&pUserSocketInfo->dwTokenID,1);
			wPacketSize=sizeof(CMD_CB_S_ClubMember);
		}
		
		//获取对象
		pClubMemberUser=(tagClubMemberUser *)&m_cbDataBuffer[wPacketSize];

		//拷贝数据		
		CopyMemory(&pClubMemberUser->MemberInfo,pClubMember,sizeof(pClubMemberUser->MemberInfo));
		CopyMemory(&pClubMemberUser->UserInfo,pClubUserItem->GetUserInfo(),sizeof(pClubMemberUser->UserInfo));

		/*CString s;
		s.Format(_T("3 szNickName:%s,szUserHeadUrl:%s"), pClubMemberUser->UserInfo.szNickName, pClubMemberUser->UserInfo.szUserHeadUrl);
		CTraceService::TraceString(s, TraceLevel_Exception, dwClubID);*/

		//设置变量
		wPacketSize+=sizeof(tagClubMemberUser);

	} while (Position!=NULL);

	//发送数据
	if (wPacketSize>0)
	{
		SendDataToGate(pUserSocketInfo->wGateID,SERVICE_MODULE_CLUB,MDM_CLUB_SERVICE,SUB_CB_S_CLUB_MEMBER,m_cbDataBuffer,wPacketSize, &pUserSocketInfo->dwTokenID, 1);
	}
	/*CString s;
	s.Format(_T("4 sizeof(tagClubMemberUser)%d"), sizeof(tagClubMemberUser));
	CTraceService::TraceString(s, TraceLevel_Exception, dwClubID);*/
	return true;
}

//发送消息
bool CAttemperEngineSink::SendApplyMessageToUser(DWORD dwClubID, tagSocketInfo *pUserSocketInfo)
{
	//查找网关
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(dwClubID);
	if (pClubInfoItem == NULL) return false;

	//变量定义	
	CClubUserItem * pClubUserItem = NULL;
	tagClubApplyMessage * pIMApplyMessage1 = NULL;
	tagClubApplyMessage * pIMApplyMessage2 = NULL;

	//变量定义
	auto pClubMessage = (CMD_CB_S_ApplyMessage *)&m_cbDataBuffer;

	//设置变量
	pClubMessage->wMessageCount = 0;	

	//变量定义
	POSITION Position = NULL;
	WORD wPacketSize = sizeof(CMD_CB_S_ApplyMessage);

	do
	{
		//枚举成员
		pIMApplyMessage1 = pClubInfoItem->EnumMessage(Position);
		if (pIMApplyMessage1 == NULL) break;

		//发送判断
		if (wPacketSize + sizeof(tagClubApplyMessage) > sizeof(m_cbDataBuffer))
		{
			//发送数据
			SendDataToGate(pUserSocketInfo->wGateID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_APPLY_MESSAGE, m_cbDataBuffer, wPacketSize, &pUserSocketInfo->dwTokenID, 1);
			
			//设置变量
			pClubMessage->wMessageCount=0;
			wPacketSize = sizeof(CMD_CB_S_ApplyMessage);
		}

		//获取对象
		pIMApplyMessage2 = (tagClubApplyMessage *)&m_cbDataBuffer[wPacketSize];

		//拷贝数据		
		CopyMemory(pIMApplyMessage2, pIMApplyMessage1, sizeof(tagClubApplyMessage));

		//设置变量
		pClubMessage->wMessageCount++;
		wPacketSize += sizeof(tagClubApplyMessage);

	} while (Position != NULL);

	//发送数据
	if (wPacketSize > sizeof(CMD_CB_S_ApplyMessage))
	{
		SendDataToGate(pUserSocketInfo->wGateID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_APPLY_MESSAGE, m_cbDataBuffer, wPacketSize, &pUserSocketInfo->dwTokenID, 1);
	}

	return true;
}

//发送成员
bool CAttemperEngineSink::SendBattleConfigToUser(DWORD dwClubID,tagSocketInfo *pUserSocketInfo)
{
	//查找茶社
	auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(dwClubID);
	if (pClubInfoItem==NULL) return false; 

	//变量定义
	tagClubBattleConfig * pBattleConfig1=NULL;
	tagClubBattleConfig * pBattleConfig2=NULL;

	//变量定义
	auto pDBOBattleConfig=(CMD_CB_S_BattleConfig *)&m_cbDataBuffer;
	
	//设置变量
	pDBOBattleConfig->dwClubID=dwClubID;	

	//变量定义
	INT nIndex=0;
	WORD wPacketSize=sizeof(CMD_CB_S_BattleConfig);

	do
	{
		//枚举玩法
		pBattleConfig1=pClubInfoItem->EnumConfig(nIndex++);		
		if (pBattleConfig1==NULL) break;

		//发送判断
		if (wPacketSize+sizeof(tagClubBattleConfig)>sizeof(m_cbDataBuffer))
		{
			SendDataToGate(pUserSocketInfo->wGateID,SERVICE_MODULE_CLUB,MDM_CLUB_SERVICE,SUB_CB_S_BATTLE_CONFIG,m_cbDataBuffer,wPacketSize,&pUserSocketInfo->dwTokenID,1);
			wPacketSize=sizeof(CMD_CB_S_BattleConfig);
		}
		
		//获取对象
		pBattleConfig2=(tagClubBattleConfig *)&m_cbDataBuffer[wPacketSize];

		//拷贝数据		
		CopyMemory(pBattleConfig2,pBattleConfig1,sizeof(tagClubBattleConfig));		

		//设置变量
		wPacketSize+=sizeof(tagClubBattleConfig);

	} while (true);	

	//发送数据
	if (wPacketSize>sizeof(CMD_CB_S_BattleConfig))
	{
		SendDataToGate(pUserSocketInfo->wGateID,SERVICE_MODULE_CLUB,MDM_CLUB_SERVICE,SUB_CB_S_BATTLE_CONFIG,m_cbDataBuffer,wPacketSize,&pUserSocketInfo->dwTokenID,1);
	}

	return true;
}

//发送设置
bool CAttemperEngineSink::SendKindSettingToUser(DWORD dwClubID, tagSocketInfo* pUserSocketInfo)
{
	//查找茶社
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(dwClubID);
	if (pClubInfoItem == NULL) return false;

	//变量定义
	INT nIndex = 0;
	WORD wPacketSize = 0;

	do
	{
		//枚举玩法
		auto pKindSettingItem1 = pClubInfoItem->EnumKindSetting(nIndex++);
		if (pKindSettingItem1 == NULL) break;

		//发送判断
		if (wPacketSize + sizeof(tagClubKindSetting) > sizeof(m_cbDataBuffer))
		{
			SendDataToGate(pUserSocketInfo->wGateID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_KIND_SETTING_ITEM, m_cbDataBuffer, wPacketSize, &pUserSocketInfo->dwTokenID, 1);
			wPacketSize = 0;
		}

		//获取对象
		auto pKindSettingItem2 = (tagClubKindSetting*)&m_cbDataBuffer[wPacketSize];

		//拷贝数据		
		CopyMemory(pKindSettingItem2, pKindSettingItem1, sizeof(tagClubKindSetting));

		//设置变量
		wPacketSize += sizeof(tagClubKindSetting);

	} while (true);

	//发送数据
	if (wPacketSize > 0)
	{
		SendDataToGate(pUserSocketInfo->wGateID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_KIND_SETTING_ITEM, m_cbDataBuffer, wPacketSize, &pUserSocketInfo->dwTokenID, 1);
	}

	return true;
}

//发送桌子
bool CAttemperEngineSink::SendBattleTableToUser(DWORD dwClubID, tagSocketInfo* pUserSocketInfo)
{
	//变量定义	
	POSITION Position = NULL;
	tagClubTableItem* pClubTableItem = NULL;

	//变量定义
	WORD wPacketSize = 0;
	tagBattleTableItem* pTableItem = NULL;
	tagBattleTableItem* pTableItemEx = NULL;
	tagBattleTableParam* pTableParam = NULL;
	tagBattleTableParamEx* pTableParamEx = NULL;

	//查找茶社
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(dwClubID);
	if (pClubInfoItem != NULL)
	{
		do
		{
			pClubTableItem = pClubInfoItem->EnumClubTable(Position);
			if (pClubTableItem == NULL) break;

			//发送判断
			if (wPacketSize + sizeof(tagBattleTableItem) + sizeof(tagBattleTableParam) + TABLE_USER_COUNT * sizeof(tagBattleTableUser) > sizeof(m_cbDataBuffer))
			{
				SendDataToGate(pUserSocketInfo->wGateID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_TABLE_ITEM, m_cbDataBuffer, wPacketSize, &pUserSocketInfo->dwTokenID, 1);
				wPacketSize = 0;
			}

			//获取数据
			pTableItemEx = &pClubTableItem->TableItem;
			pTableParamEx = &pClubTableItem->TableParam;

			//桌子子项
			pTableItem = (tagBattleTableItem*)&m_cbDataBuffer[wPacketSize];
			CopyMemory(pTableItem, pTableItemEx, sizeof(tagBattleTableItem));

			//拷贝数据			
			wPacketSize += sizeof(tagBattleTableItem);

			//桌子参数
			pTableParam = (tagBattleTableParam*)&m_cbDataBuffer[wPacketSize];
			ZeroMemory(pTableParam, sizeof(tagBattleTableParam));

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

		} while (Position!=NULL);
	}

	//发送判断
	if (wPacketSize>0)
	{
		CTraceService::TraceString(TEXT("茶社[%d]发送桌子给玩家！"), TraceLevel_Exception, dwClubID);

		SendDataToGate(pUserSocketInfo->wGateID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_TABLE_ITEM, m_cbDataBuffer, wPacketSize, &pUserSocketInfo->dwTokenID, 1);		
	}	

	return true;
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
	SendDataToGate(wServiceID, SERVICE_MODULE_CLUBPLAZA, MDM_CM_SYSTEM, SUB_CM_OPERATE_SUCCESS, pRequestSuccess, wDataSize, &dwTokenID, 1);

	return;
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
	SendDataToGate(wServiceID, SERVICE_MODULE_CLUBPLAZA, MDM_CM_SYSTEM, SUB_CM_OPERATE_FAILURE, pRequestFailure, wDataSize, &dwTokenID, 1);

	return;
}

//发送数据
bool CAttemperEngineSink::SendDataToUser(WORD wServiceModule, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize,tagSocketInfo *pUserSocketInfo)
{
	//发送数据
	return SendDataToGate(pUserSocketInfo->wGateID,wServiceModule,wMainCmdID,wSubCmdID, pData,wDataSize,&pUserSocketInfo->dwTokenID,1);
}

//网关发送
bool CAttemperEngineSink::SendDataToGate(WORD wGateID, WORD wServiceModule, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD *pdwTokenID, WORD wTokenCount)
{
	//查找网关
	auto  pGlobalGateItem=m_GlobalItemManager.SearchGateItem(wGateID);
	if (pGlobalGateItem ==NULL) return false;

	//发送数据
	m_pITCPNetworkEngine->SendData(pGlobalGateItem->GetSocketID(), MAKECMMD(wMainCmdID, wServiceModule),wSubCmdID,pData,wDataSize,pdwTokenID,wTokenCount);

	return true;
}


//茶社发送
bool CAttemperEngineSink::SendDataToMember(DWORD dwClubID, WORD wServiceModule, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, BYTE cbLocationMask)
{
	//查找网关
	auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(dwClubID);
	if (pClubInfoItem==NULL) return false;

	//变量定义
	INT_PTR nIndex=0;		
	tagClubMemberInfo * pClubMember=NULL;
	CClubUserItem * pClubUserItem=NULL;

	//变量定义
	POSITION Position=NULL;
	CGlobalGateItem * pGlobalGateItem=NULL;

	do
	{	
		//枚举成员
		pClubMember=pClubInfoItem->EnumMember(Position);
		if (pClubMember==NULL) break;

		//查找用户
		pClubUserItem=m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
		if (pClubUserItem==NULL) continue;
		if (pClubUserItem->GetUserStatus()!=US_CB_ONLINE) continue;

		//位置掩码
		if (cbLocationMask != (LOCATION_MASK_INCLUB | LOCATION_MASK_OUTCLUB))
		{
			if ((cbLocationMask&LOCATION_MASK_INCLUB) != 0 && pClubUserItem->GetActiveClubID() != dwClubID) continue;
			if ((cbLocationMask&LOCATION_MASK_OUTCLUB) != 0 && pClubUserItem->GetActiveClubID() == dwClubID) continue;
		}

		//查找网关
		pGlobalGateItem=m_GlobalItemManager.SearchGateItem(pClubUserItem->GetGateID());
		if (pGlobalGateItem!=NULL)
		{
			pGlobalGateItem->m_TokenIDArray.Add(pClubUserItem->GetTokenID());
		}		

	} while (Position!=NULL);

	//////////////////////////////////////////////////////////////////////////////////////////////

	//变量定义		
	DWORD * pdwTokenIDArray=NULL; 
	INT_PTR nCellCount = 0,nSendCount=0,nTotalCount=0;
	
	//重置变量
	pGlobalGateItem = NULL;

	do
	{	
		//枚举网关
		pGlobalGateItem=m_GlobalItemManager.EnumGateItem(pGlobalGateItem);
		if (pGlobalGateItem==NULL) break;

		//设置变量		
		nSendCount=0;
		nCellCount =pGlobalGateItem->m_TokenIDArray.GetCount();
		nTotalCount=pGlobalGateItem->m_TokenIDArray.GetCount();		
		pdwTokenIDArray=pGlobalGateItem->m_TokenIDArray.GetData();		

		//切分数量
		while ((sizeof(WORD)+ nCellCount *sizeof(DWORD)+wDataSize)>SOCKET_PACKET)
		{
			nCellCount = nCellCount /2;
		}	

		//循环发送
		while (nSendCount < nTotalCount)
		{		
			//调整变量
			if (nTotalCount-nSendCount< nCellCount)
			{
				nCellCount =nTotalCount-nSendCount;
			}

			//调整数量
			if (nCellCount > TOKEN_COUNT) nCellCount = TOKEN_COUNT;

			//发送数据
			SendDataToGate(pGlobalGateItem->GetServiceID(),wServiceModule,wMainCmdID,wSubCmdID,pData,wDataSize,pdwTokenIDArray+nSendCount,nCellCount);

			//设置数据			
			nSendCount += nCellCount;
		}

		//清除令牌
		pGlobalGateItem->m_TokenIDArray.RemoveAll();

	} while (Position!=NULL);

	return true;
}
//////////////////////////////////////////////////////////////////////////
