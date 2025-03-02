#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_SERVICE_PULSE			1									//服务脉冲
#define IDI_LOAD_GAME_LIST			2									//加载列表
#define IDI_CONNECT_CENTER			3									//重连标识
#define IDI_COLLECT_ONLINE_INFO		4									//统计在线
#define IDI_COLLECT_JACKPOT_INFO	5									//统计奖池

//常量定义
#define INVALIDATE_CODE_TIME		60									//有效时间	

//////////////////////////////////////////////////////////////////////////
//掩码定义
#define SUPPORT_MASK_PC				0x0001								//电脑支持
#define SUPPORT_MASK_MOBILE			0x0002								//手机支持

//////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//状态变量
	m_dwPCFuncption=0;
	m_dwMBFuncption=0;
	m_pServiceOption=NULL;
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;	
	m_dwShowOnLineInfo=SHOW_SERVER_USER_COUNT;

	//辅助变量
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));

	//组件变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;

	//版本配置
	m_bUpdateSilent=false;

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

	//设置时间
	ASSERT(m_pITimerEngine != NULL);
	m_pITimerEngine->SetTimer(IDI_SERVICE_PULSE, 1 * 1000L, TIMES_INFINITY, 0);

	//设置时间
	ASSERT(m_pITimerEngine!=NULL);
	m_pITimerEngine->SetTimer(IDI_COLLECT_ONLINE_INFO,m_pInitParameter->m_wCollectTime*1000L,TIMES_INFINITY,0);		
	

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//重置数据
	m_WordsFilter.ResetFilter();
	m_GlobalItemManager.ResetData();
	m_ServerListManager.ResetServerList();

	//组件变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;

	//清理数据
	m_pServiceOption=NULL;
	SafeDeleteArray(m_pBindParameter);

	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	switch (dwIdentifier)
	{
	case CT_LOAD_CONFIG_DATA:	//加载配置
	{
		//加载列表
		m_ServerListManager.DisuseKernelItem();

		//配置掩码
		DBR_GP_LoadConfig LoadConfig;
		ZeroMemory(&LoadConfig, sizeof(LoadConfig));

		//设置变量
		LoadConfig.bInitService = true;
		LoadConfig.dwConfigMask = 0xFFFFFFFF;		

		//加载配置
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_CONFIG, 0, 0, 0, &LoadConfig, sizeof(LoadConfig));

		//加载排行榜
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_RANKING_LIST, 0, 0, 0, NULL, 0);		

		return true;
	}
	case CT_CONNECT_CENTER:		//连接中心
	{
		//变量定义
		DWORD dwCenterServer=0L;

		//设置地址
		dwCenterServer=m_pInitParameter->m_CenterAddress.dwServiceAddr;

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
	case IDI_LOAD_GAME_LIST:		//加载列表
	{
		//加载列表
		m_ServerListManager.DisuseKernelItem();

		//配置掩码
		DBR_GP_LoadConfig LoadConfig;
		ZeroMemory(&LoadConfig, sizeof(LoadConfig));

		//设置变量
		LoadConfig.dwConfigMask = CONFIG_MASK_GAME_LIST;

		//加载配置
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_CONFIG, 0, 0, 0, &LoadConfig, sizeof(LoadConfig));				

		return true;
	}
	case IDI_CONNECT_CENTER:		//连接中心
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
	case IDI_COLLECT_ONLINE_INFO:	//统计在线
	{
		//变量定义
		DBR_GP_OnLineCountInfo OnLineCountInfo;
		ZeroMemory(&OnLineCountInfo,sizeof(OnLineCountInfo));

		//获取总数
		OnLineCountInfo.dwOnLineCountSum=m_ServerListManager.CollectOnlineInfo();

		//获取类型
		POSITION KindPosition=NULL;
		do
		{
			//获取类型
			CGameKindItem * pGameKindItem=m_ServerListManager.EmunGameKindItem(KindPosition);

			//设置变量
			if (pGameKindItem!=NULL)
			{
				//溢出判断
				if (OnLineCountInfo.wKindCount>=CountArray(OnLineCountInfo.OnLineCountKind))
				{
					ASSERT(FALSE);
					break;
				}

				WORD wKindIndex=OnLineCountInfo.wKindCount++;
				OnLineCountInfo.OnLineCountKind[wKindIndex].wKindID=pGameKindItem->GetKindID();
				OnLineCountInfo.OnLineCountKind[wKindIndex].dwOnLineCount=pGameKindItem->m_dwOnLineCount;
			}

		} while (KindPosition!=NULL);
		
		//发送请求
		WORD wHeadSize=sizeof(OnLineCountInfo)-sizeof(OnLineCountInfo.OnLineCountKind);
		WORD wSendSize=wHeadSize+OnLineCountInfo.wKindCount*sizeof(OnLineCountInfo.OnLineCountKind[0]);
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_ONLINE_COUNT_INFO,0,0,0,&OnLineCountInfo,wSendSize);

		return true;
	}
	case IDI_SERVICE_PULSE:			//服务脉冲
	{
		//静态变量
		static bool bTimeInit = false;
		static SYSTEMTIME LastSystemTime;

		//初始化时间
		if (bTimeInit == false)
		{
			bTimeInit = true;
			GetLocalTime(&LastSystemTime);
		}

		//变量定义
		SYSTEMTIME CurrSysteTime;
		GetLocalTime(&CurrSysteTime);

		//日期切换
		if (CurrSysteTime.wDay != LastSystemTime.wDay)
		{
			OnEventDaySwitch();
		}

		//更新时间
		LastSystemTime = CurrSysteTime;

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
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	//设置变量
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//提取命令码	
	bool bResult = false;
	WORD wMainCmdID = LOCMMD(Command.wMainCmdID);

	//注册服务
	if (wMainCmdID==MDM_CS_REGISTER) 
	{
		return OnTCPNetworkMainRegister(Command.wSubCmdID, pData, wDataSize, dwSocketID);
	}

	//通用服务
	if (wMainCmdID==MDM_CS_COMMON_SERVICE)	
	{
		return OnTCPNetworkMainCommonService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
	}	

	switch (wMainCmdID)
	{	
	case MDM_GP_LOGON:			 //登录命令
	{
		bResult=OnTCPNetworkMainPCLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
		break;
	}
	case MDM_GP_SERVER_LIST:	 //列表命令
	{
		bResult=OnTCPNetworkMainPCServerList(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
		break;
	}
	case MDM_GP_USER_SERVICE:	 //服务命令
	{
		bResult=OnTCPNetworkMainPCUserService(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
		break;
	}
	case MDM_GP_CONFIG_SERVICE:	 //配置服务
	{
		bResult=OnTCPNetworkMainPCConfigService(Command.wSubCmdID, pData, wDataSize, dwSocketID, dwTokenID);
		break;
	}
	case MDM_GP_ROBOT_SERVICE:	 //机器服务
	{
		bResult=OnTCPNetworkMainPCRobotService(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
		break;
	}
	case MDM_GP_STOCK_SERVICE:	 //库存服务
	{
		bResult = OnTCPNetworkMainPCStockService(Command.wSubCmdID, pData, wDataSize, dwSocketID, dwTokenID);
		break;
	}	
	case MDM_GP_MANAGER_SERVICE: //管理服务	
	{
		bResult=OnTCPNetworkMainPCManagerService(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
		break;
	}
	case MDM_MB_LOGON:			 //登录命令
	{
		bResult=OnTCPNetworkMainMBLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
		break;
	}
	case MDM_MB_SERVER_LIST:	 //列表命令
	{
		bResult=OnTCPNetworkMainMBServerList(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
		break;
	}	
	case MDM_MB_USER_SERVICE:	 //用户服务
	{
		bResult=OnTCPNetworkMainMBUserService(Command.wSubCmdID, pData, wDataSize, dwSocketID, dwTokenID);
		break;
	}
	}

	//关闭链接
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
	//获取参数
	auto pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return false;

	//中转模式
	if (pBindParameter->wServiceModule == SERVICE_MODULE_GATE)
	{
		//移除服务
		m_GlobalItemManager.DeleteGateItem(pBindParameter->wServiceID);
	}

	//清除信息
	ZeroMemory(pBindParameter, sizeof(tagBindParameter));

	return true;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GP_GAME_TYPE_ITEM:			//类型子项
	{
		return OnDBGameTypeItem(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_GAME_KIND_ITEM:			//类型子项
	{
		return OnDBGameKindItem(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_GAME_NODE_ITEM:			//节点子项
	{
		return OnDBGameNodeItem(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_GAME_PAGE_ITEM:			//定制子项
	{
		return OnDBGamePageItem(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_GAME_FILTER_ITEM:		//过滤子项
	{
		return OnDBGameFilterItem(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_CONFIG_RESULT:			//配置结果
	{
		return OnDBConfigResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_FILTER_WORDS:			//敏感词信息
	{
		return OnDBFilterWordsInfo(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_RANKING_LIST_DATA:		//榜单数据
	{
		return OnDBRankingListData(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_VERSION_INFO:			//版本信息
	{
		return OnDBModuleVersionInfo(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_TRANSFER_PARAMETER:		//转账参数
	{
		return OnDBTransferParameter(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_LOGON_SUCCESS:			//登录成功
	{
		return OnDBPCLogonSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_LOGON_FAILURE:			//登录失败
	{
		return OnDBPCLogonFailure(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_USER_INDIVIDUAL:		//用户信息
	{
		return OnDBPCUserIndividual(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_OPERATE_SUCCESS:		//操作成功
	{
		return OnDBPCOperateSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_OPERATE_FAILURE:		//操作失败
	{
		return OnDBPCOperateFailure(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_GAME_BASIC_CONFIG:		//基础配置
	{
		return OnDBPCGameBasicConfig(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_GAME_WEIGHT_CONFIG:		//权重配置
	{
		return OnDBPCGameWeightConfig(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_GAME_CONFIG_FINISH:		//配置完成
	{
		return OnDBPCGameConfigFinish(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_ROBOT_OPERATE_RESULT:	//操作结果
	{
		return OnDBPCRobotOperateResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_STOCK_OPERATE_RESULT:	//操作结果
	{
		return OnDBPCStockOperateResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_FACE_INFO:			//用户头像
	{
		return OnDBMBUserFaceInfo(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_CACHE_INFO:		//用户缓存
	{
		return OnDBMBUserCacheInfo(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_ACTION_MASK:		//行为掩码
	{
		return OnDBMBUserActionMask(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_RANKING_INFO:		//排位信息
	{
		return OnDBMBUserRankingInfo(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_EXCHANGE_MALLGOODS:		//兑换商品 
	{
		return OnDBMBExchangeMallGoods(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_EXCHANGE_GIFTBAGCODE:	//兑换礼包码
	{
		return OnDBMBExchangeGiftbagCode(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_BINDMP_SUCCESS:			//绑定成功
	{
		return OnDBMBBindMPSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_UNBINDMP_SUCCESS:		//解绑成功
	{
		return OnDBMBUnbindMPSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_BIND_EMAIL_SUCCESS:		//绑定成功
	{
		return OnDBMBBindEMailSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_UNBIND_EMAIL_SUCCESS:	//解绑成功
	{
		return OnDBMBUnbindEMailSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_ACQUIREAC_RESULT:		//获取结果
	{
		return OnDBMBAcquireACResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_PACK_GOODS:		//背包物品
	{
		return OnDBMBUserPackGoods(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_GOODS_UPDATE:		//物品更新
	{
		return OnDBMBUserGoodsUpdate(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_WEALTH_UPDATE:		//财富更新
	{
		return OnDBMBUserWealthUpdate(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_LOVELINESS_UPDATE: //魅力更新
	{
		return OnDBMBUserLoveLinessUpdate(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_LOGON_SUCCESS:			//登录成功
	{
		return OnDBMBLogonSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_LOGON_FAILURE:			//登录失败
	{
		return OnDBMBLogonFailure(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_LOGOUT_RESULT:			//登出结果
	{
		return OnDBMBLogoutResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_POST_MESSAGE:			//投递消息
	{
		return OnDBMBPostMessage(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_CHECKIN_RESULT:			//签到结果
	{
		return OnDBMBCheckInResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_BIND_INVITE_CODE:		//绑定邀请码
	{
		return OnDBMBBindInviteCode(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_BIND_EXTERNAL_ACCOUNT:	//绑定账号
	{
		return OnDBMBBindExternalAccount(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_SEND_GIFT_SUCCESS:		//赠送礼物成功
	{
		return OnDBMBSendGiftSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_GIFT_INFO:			//礼物信息
	{
		return OnDBMBUserGiftInfo(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_GIFT_INFO_FINISH:	//礼物信息完成
	{
		return OnDBMBUserGiftInfoFinish(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_WEALTH_TRANSFER_SUCCESS: //转账成功
	{
		return OnDBMBWealthTransferSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_TURNTABLE_LOTTERY_DATA:	//转盘数据
	{
		return OnDBMBTurntableLotteryData(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_TURNTABLE_LOTTERY_RESULT: //抽奖结果
	{
		return OnDBMBTurntableLotteryResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_MB_INSURE_QUERY_RESULT:	//银行查询结果
	{
		return OnDBMBInsureQueryResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_MB_INSURE_SUCCESS:		//操作成功
	{
		return OnDBMBInsureSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_MB_INSURE_MAIL_NOTIFY:	//银行邮件通知
	{
		return OnDBMBInsureMailNotify(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_MB_INSURE_FAILURE:		//操作失败
	{
		return OnDBMBInsureFailure(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_MB_RESULT_SUBSIDY:	//低保查询
	{
		return OnDBMBSubsidyQuery(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_OPERATE_SUCCESS:		//操作成功
	{
		return OnDBMBOperateSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_OPERATE_FAILURE:		//操作失败
	{
		return OnDBMBOperateFailure(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_MB_NICKNAME_OPTION:	//昵称配置
	{
		return OnDBMBNickNameOption(dwContextID, dwTokenID, pData, wDataSize);
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
	if (dwServiceID ==NETWORK_CENTER)
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

		//提示消息
		CTraceService::TraceString(TEXT("正在获取广场游戏列表..."),TraceLevel_Normal);

		//变量定义
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService,sizeof(RegisterService));

		//设置变量
		RegisterService.wServiceID = m_pServiceOption->wServiceID;
		RegisterService.wServiceModule = SERVICE_MODULE_LOGON;
		RegisterService.wServicePort= m_pServiceOption->wServicePort;
		RegisterService.wServiceKindID = m_pServiceOption->wServiceKindID;
		RegisterService.wServiceMask = SERVICE_MASK_GAME | SERVICE_MASK_LOGON;		
		StringCchCopy(RegisterService.szServiceName,CountArray(RegisterService.szServiceName), L"登录服务器");
		StringCchCopy(RegisterService.szServiceDomain,CountArray(RegisterService.szServiceDomain), m_pServiceOption->szServiceDomain);

		//发送数据
		m_pITCPSocketCenter->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_SERVICE,&RegisterService,sizeof(RegisterService),0);

		return true;
	}

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	//中心连接
	if (dwServiceID==NETWORK_CENTER)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:		//注册服务
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
		}
		case MDM_CS_COMMON_SERVICE:	//服务信息
		{
			return OnTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
		}	
		case MDM_CS_USER_COLLECT:	//用户收集
		{
			return OnTCPSocketMainUserCollect(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_CS_FORWARD_SERVICE://转发服务
		{
			return OnTCPSocketMainForwardService(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_CS_MANAGER_SERVICE://管理服务
		{
			return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
		}
		}
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
	case SUB_CS_S_REGISTER_FAILURE:		//注册失败
		{
			//变量定义
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;

			//效验参数
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//关闭处理
			m_pITCPSocketCenter->CloseSocket();

			//显示消息
			if (lstrlen(pRegisterFailure->szDescribeString) > 0)
			{
				CTraceService::TraceString(pRegisterFailure->szDescribeString, TraceLevel_Exception);
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
	case SUB_CS_C_REGISTER_SERVICE:		//注册服务
		{
			//效验数据
			ASSERT(wDataSize == sizeof (CMD_CS_C_RegisterService));
			if (wDataSize != sizeof (CMD_CS_C_RegisterService)) return false;

			//消息定义
			CMD_CS_C_RegisterService * pRegisterService = (CMD_CS_C_RegisterService *)pData;

			//校验服务
			if (pRegisterService->wServiceModule != SERVICE_MODULE_GATE) return false;

			//查找路由
			if (m_GlobalItemManager.SearchGateItem(pRegisterService->wServiceID) != NULL)
			{
				//变量定义
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure, sizeof(RegisterFailure));

				//设置变量
				RegisterFailure.lErrorCode = 0;
				_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString),L"已经存在相同游戏标识的%s，服务注册失败", pRegisterService->szServiceName);

				//发送消息
				WORD wStringSize = CountStringBuffer(RegisterFailure.szDescribeString);
				WORD wSendSize = sizeof(RegisterFailure) - sizeof(RegisterFailure.szDescribeString) + wStringSize;
				SendDataToGate(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_FAILURE, &RegisterFailure, wSendSize, NULL, 0);

				//中断网络
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//设置绑定
			tagBindParameter * pBindParameter = GetBindParameter(dwSocketID);
			if (pBindParameter == NULL) return false;

			//设置绑定
			pBindParameter->wServiceID = pRegisterService->wServiceID;
			pBindParameter->wServiceModule = pRegisterService->wServiceModule;			
			
			//变量定义
			tagServiceItem ServiceItem;
			ZeroMemory(&ServiceItem, sizeof(ServiceItem));

			//构造结构
			ServiceItem.wServiceID = pRegisterService->wServiceID;
			ServiceItem.wMaxConnect = pRegisterService->wMaxConnect;
			ServiceItem.wServicePort = pRegisterService->wServicePort;
			ServiceItem.wServiceModule = pRegisterService->wServiceModule;
			ServiceItem.dwServiceAddr = pRegisterService->dwServiceAddr;			
			StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), pRegisterService->szServiceName);
			StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain), pRegisterService->szServiceDomain);

			//注册网关
			pBindParameter->pGlobalGateItem = m_GlobalItemManager.ActiveGateItem(dwSocketID, ServiceItem);

			//发送数据
			SendDataToGate(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_SUCCESS, NULL, 0);			

			return true;
		}
	}

	return true;
}

//服务信息
bool CAttemperEngineSink::OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_LOGOUT_TOKEN:	//注销令牌
		{
			//绑定参数
			tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
			if (pBindParameter == NULL) return false;

			//提取数据
			CMD_CS_C_LogoutToken * pLogoutToken = (CMD_CS_C_LogoutToken *)pData;

			//获取对象
			CGlobalGateItem * pGlobalGateItem = (CGlobalGateItem *)pBindParameter->pGlobalGateItem;
			if (pGlobalGateItem == NULL) return true;

			//账号登出
			auto pTokenParameter = pGlobalGateItem->SearchTokenID(pLogoutToken->dwTokenID);
			if (pTokenParameter != NULL && pTokenParameter->pBindUserItem!=NULL)
			{
				//用户信息
				auto pGlobalUserItem = (CGlobalUserItem *)pTokenParameter->pBindUserItem;

				//构造结构
				DBR_MB_LogoutAccounts LogoutAccounts;
				ZeroMemory(&LogoutAccounts,sizeof(LogoutAccounts));

				//设置变量
				LogoutAccounts.wGateID = pGlobalGateItem->GetServiceID();
				LogoutAccounts.wLogonID = m_pServiceOption->wServiceID;
				LogoutAccounts.dwSocketID = pLogoutToken->dwTokenID;
				LogoutAccounts.dwUserID = pGlobalUserItem->GetUserID();

				//投递请求
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGOUT_ACCOUNTS, LogoutAccounts.dwUserID, dwSocketID, pLogoutToken->dwTokenID, &LogoutAccounts, sizeof(LogoutAccounts));				

				//移除用户
				m_GlobalItemManager.DeleteUserItem(pGlobalUserItem->GetUserID());
			}			

			//注销令牌
			pGlobalGateItem->RemoveTokenID(pLogoutToken->dwTokenID);
			
			//登出处理
			return true;
		}
	case SUB_CS_C_REGISTER_TOKEN: //注册令牌
		{
			//绑定参数
			tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
			if (pBindParameter == NULL) return false;

			//获取对象
			CGlobalGateItem * pGlobalGateItem = (CGlobalGateItem *)pBindParameter->pGlobalGateItem;
			if (pGlobalGateItem == NULL) return true;

			//提取数据
			CMD_CS_C_RegisterToken * pRegisterToken = (CMD_CS_C_RegisterToken *)pData;

			//构造结构
			CMD_CS_S_RegisterToken RegisterToken;
			ZeroMemory(&RegisterToken, sizeof(RegisterToken));

			//默认失败
			RegisterToken.lResultCode = REGISTER_RESULT_FAILURE;

			//获取参数
			tagTokenParameter * pTokenParameter = pGlobalGateItem->ActiveTokenID(pRegisterToken->dwTokenID);
			if (pTokenParameter != NULL)
			{
				//注册结果
				RegisterToken.lResultCode = REGISTER_RESULT_SUCCESS;

				//设置地址		
				pTokenParameter->dwUserID = pRegisterToken->dwUserID;
				pTokenParameter->dwClientIP = pRegisterToken->dwClientIP;
			}

			//发送数据
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_REGISTER_TOKEN, &RegisterToken, sizeof(RegisterToken), &pRegisterToken->dwTokenID, 1);

			return true;
		}
	}
	return true;
}

//登录处理
bool CAttemperEngineSink::OnTCPNetworkMainPCLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_LOGON_GAMEID:		//游客登录
		{
			return OnTCPNetworkSubPCLogonByGameID(pData,wDataSize,dwSocketID, dwTokenID);
		}
	case SUB_GP_LOGON_ACCOUNTS:		//帐号登录
		{
			return OnTCPNetworkSubPCLogonByAccounts(pData,wDataSize,dwSocketID, dwTokenID);
		}
	case SUB_GP_LOGON_MANAGETOOL:	//管理工具
		{
			return OnTCPNetworkSubPCLogonManageTool(pData,wDataSize,dwSocketID, dwTokenID);
		}
	case SUB_GP_REGISTER_ACCOUNTS:	//帐号注册
		{
			return OnTCPNetworkSubPCRegisterAccounts(pData,wDataSize,dwSocketID, dwTokenID);
		}
	}

	return false;
}

//列表处理
bool CAttemperEngineSink::OnTCPNetworkMainPCServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_GET_LIST:			//获取列表
		{
			ASSERT (wDataSize==sizeof(DWORD));
			if (wDataSize!=sizeof(DWORD)) return true;

			//提取站点
      DWORD dwStationID=*(DWORD*)pData;

			//发送类型
			SendGameTypeInfo(dwSocketID, dwTokenID);

			//发送游戏
			SendGameKindInfo(dwSocketID, dwTokenID, SUPPORT_MASK_PC);

			//发送页面
			SendGamePageInfo(dwSocketID, dwTokenID, 0);

			//发送完成
			SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_FINISH,&dwTokenID,1);

			return true;
		}
	case SUB_GP_GET_SERVER:			//获取房间
		{
			//效验数据
			ASSERT(wDataSize%sizeof(WORD)==0);
			if (wDataSize%sizeof(WORD)!=0) return false;

			//发送列表
			UINT nKindCount=wDataSize/sizeof(WORD);
			for (UINT i=1;i<nKindCount;i++)
			{
				SendGameNodeInfo(dwSocketID, dwTokenID, ((WORD *)pData)[i]);
				SendGamePageInfo(dwSocketID, dwTokenID, ((WORD *)pData)[i]);
				SendGameServerInfo(dwSocketID, dwTokenID,((WORD *)pData)[i],((WORD *)pData)[0]);
			}

			//发送完成
			SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_SERVER_FINISH,pData,wDataSize,&dwTokenID,1);

			return true;
		}
	case SUB_GP_GET_ONLINE:			//获取在线
		{
			//变量定义
			CMD_GP_GetOnline * pGetOnline=(CMD_GP_GetOnline *)pData;
			WORD wHeadSize=(sizeof(CMD_GP_GetOnline)-sizeof(pGetOnline->wOnLineServerID));

			//效验数据
			ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGetOnline->wServerCount*sizeof(WORD))));
			if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGetOnline->wServerCount*sizeof(WORD)))) return false;

			//变量定义
			CMD_GP_KindOnline KindOnline;
			CMD_GP_ServerOnline ServerOnline;
			ZeroMemory(&KindOnline,sizeof(KindOnline));
			ZeroMemory(&ServerOnline,sizeof(ServerOnline));

			//获取类型
			POSITION KindPosition=NULL;
			do
			{
				//获取类型
				CGameKindItem * pGameKindItem=m_ServerListManager.EmunGameKindItem(KindPosition);

				//设置变量
				if (pGameKindItem!=NULL)
				{
					WORD wKindIndex=KindOnline.wKindCount++;
					KindOnline.OnLineInfoKind[wKindIndex].wKindID= pGameKindItem->GetKindID();
					KindOnline.OnLineInfoKind[wKindIndex].dwOnLineCount=pGameKindItem->m_dwOnLineCount;
				}

				//溢出判断
				if (KindOnline.wKindCount>=CountArray(KindOnline.OnLineInfoKind))
				{
					ASSERT(FALSE);
					break;
				}

			} while (KindPosition!=NULL);

			//获取房间
			for (WORD i=0;i<pGetOnline->wServerCount;i++)
			{
				//获取房间
				WORD wServerID=pGetOnline->wOnLineServerID[i];
				CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(wServerID);

				//设置变量
				if (pGameServerItem!=NULL)
				{
					WORD wServerIndex=ServerOnline.wServerCount++;
					ServerOnline.OnLineInfoServer[wServerIndex].wServerID=wServerID;
					ServerOnline.OnLineInfoServer[wServerIndex].dwOnLineCount=pGameServerItem->m_GameServer.dwOnLineCount;
				}

				//溢出判断
				if (ServerOnline.wServerCount>=CountArray(ServerOnline.OnLineInfoServer))
				{
					ASSERT(FALSE);
					break;
				}
			}

			//类型在线
			if (KindOnline.wKindCount>0)
			{
				WORD wHeadSize=sizeof(KindOnline)-sizeof(KindOnline.OnLineInfoKind);
				WORD wSendSize=wHeadSize+KindOnline.wKindCount*sizeof(KindOnline.OnLineInfoKind[0]);
				SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GR_KINE_ONLINE,&KindOnline,wSendSize,&dwTokenID,1);
			}

			//房间在线
			if (ServerOnline.wServerCount>0)
			{
				WORD wHeadSize=sizeof(ServerOnline)-sizeof(ServerOnline.OnLineInfoServer);
				WORD wSendSize=wHeadSize+ServerOnline.wServerCount*sizeof(ServerOnline.OnLineInfoServer[0]);
				SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GR_SERVER_ONLINE,&ServerOnline,wSendSize,&dwTokenID,1);
			}

			return true;
		}
	case SUB_GP_GET_COLLECTION:		//获取收藏
		{
			return true;
		}
	}

	return false;
}

//服务处理
bool CAttemperEngineSink::OnTCPNetworkMainPCUserService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_MODIFY_ACCOUNTS:	//修改帐号	
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_GP_ModifyAccounts));
			if (wDataSize!=sizeof(CMD_GP_ModifyAccounts)) return false;

			//变量定义
			CMD_GP_ModifyAccounts * pModifyAccounts=(CMD_GP_ModifyAccounts *)pData;

			//字符截断
			pModifyAccounts->szArea[CountArray(pModifyAccounts->szArea)-1]=0;
			pModifyAccounts->szCity[CountArray(pModifyAccounts->szCity)-1]=0;
			pModifyAccounts->szProvince[CountArray(pModifyAccounts->szProvince)-1]=0;
			pModifyAccounts->szAccounts[CountArray(pModifyAccounts->szAccounts)-1]=0;
			pModifyAccounts->szPassword[CountArray(pModifyAccounts->szPassword)-1]=0;
			pModifyAccounts->szUnderWrite[CountArray(pModifyAccounts->szUnderWrite)-1]=0;

			//令牌参数
			tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParameter == NULL) return false;

			//变量定义
			DBR_GP_ModifyAccounts ModifyAccounts;
			ZeroMemory(&ModifyAccounts,sizeof(ModifyAccounts));

			//年龄性别
			ModifyAccounts.cbAge=pModifyAccounts->cbAge;
			ModifyAccounts.cbGender=pModifyAccounts->cbGender;

			//验证资料
			ModifyAccounts.dwUserID=pModifyAccounts->dwUserID;
			ModifyAccounts.dwClientIP= pTokenParameter->dwClientIP;
			StringCchCopy(ModifyAccounts.szPassword, CountArray(ModifyAccounts.szPassword), pModifyAccounts->szPassword);

			//帐号资料
			ModifyAccounts.wFaceID=pModifyAccounts->wFaceID;
			StringCchCopy(ModifyAccounts.szAccounts,CountArray(ModifyAccounts.szAccounts), pModifyAccounts->szAccounts);
			StringCchCopy(ModifyAccounts.szPassword,CountArray(ModifyAccounts.szPassword), pModifyAccounts->szPassword);
			StringCchCopy(ModifyAccounts.szUnderWrite,CountArray(ModifyAccounts.szUnderWrite), pModifyAccounts->szUnderWrite);

			//地区资料
			StringCchCopy(ModifyAccounts.szArea,CountArray(ModifyAccounts.szArea), pModifyAccounts->szArea);
			StringCchCopy(ModifyAccounts.szCity,CountArray(ModifyAccounts.szCity), pModifyAccounts->szCity);
			StringCchCopy(ModifyAccounts.szProvince,CountArray(ModifyAccounts.szProvince), pModifyAccounts->szProvince);
			ModifyAccounts.bOldVersion=(wSubCmdID==SUB_GP_MODIFY_ACCOUNTS);

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_ACCOUNTS, ModifyAccounts.dwUserID, dwSocketID,dwTokenID,&ModifyAccounts,sizeof(ModifyAccounts));

			return true;
		}
	case SUB_GP_MODIFY_LOGON_PASS:	//修改密码		
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_GP_ModifyLogonPass));
			if (wDataSize!=sizeof(CMD_GP_ModifyLogonPass)) return false;

			//处理消息
			CMD_GP_ModifyLogonPass * pModifyLogonPass=(CMD_GP_ModifyLogonPass *)pData;
			pModifyLogonPass->szDesPassword[CountArray(pModifyLogonPass->szDesPassword)-1]=0;
			pModifyLogonPass->szScrPassword[CountArray(pModifyLogonPass->szScrPassword)-1]=0;

			//令牌参数
			tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParameter == NULL) return false;

			//变量定义
			DBR_GP_ModifyLogonPass ModifyLogonPass;
			ZeroMemory(&ModifyLogonPass,sizeof(ModifyLogonPass));

			//构造数据
			ModifyLogonPass.dwUserID=pModifyLogonPass->dwUserID;
			ModifyLogonPass.dwClientIP= pTokenParameter->dwClientIP;
			StringCchCopy(ModifyLogonPass.szDesPassword,CountArray(ModifyLogonPass.szDesPassword), pModifyLogonPass->szDesPassword);
			StringCchCopy(ModifyLogonPass.szScrPassword,CountArray(ModifyLogonPass.szScrPassword), pModifyLogonPass->szScrPassword);

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_LOGON_PASS, ModifyLogonPass.dwUserID,dwSocketID,dwTokenID,&ModifyLogonPass,sizeof(ModifyLogonPass));

			return true;
		}		
	case SUB_GP_MODIFY_INSURE_PASS:	//修改密码
		{
			//效验参数
			ASSERT (wDataSize==sizeof(CMD_GP_ModifyInsurePass));
			if (wDataSize!=sizeof(CMD_GP_ModifyInsurePass)) return false;

			//处理消息
			CMD_GP_ModifyInsurePass * pModifyInsurePass=(CMD_GP_ModifyInsurePass *)pData;
			pModifyInsurePass->szDesPassword[CountArray(pModifyInsurePass->szDesPassword)-1]=0;
			pModifyInsurePass->szScrPassword[CountArray(pModifyInsurePass->szScrPassword)-1]=0;

			//令牌参数
			tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParameter == NULL) return false;

			//变量定义
			DBR_GP_ModifyInsurePass ModifyInsurePass;
			ZeroMemory(&ModifyInsurePass,sizeof(ModifyInsurePass));

			//构造数据
			ModifyInsurePass.dwUserID=pModifyInsurePass->dwUserID;
			ModifyInsurePass.dwClientIP= pTokenParameter->dwClientIP;
			StringCchCopy(ModifyInsurePass.szDesPassword,CountArray(ModifyInsurePass.szDesPassword), pModifyInsurePass->szDesPassword);
			StringCchCopy(ModifyInsurePass.szScrPassword,CountArray(ModifyInsurePass.szScrPassword), pModifyInsurePass->szScrPassword);

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_INSURE_PASS, ModifyInsurePass.dwUserID, dwSocketID,dwTokenID,&ModifyInsurePass,sizeof(ModifyInsurePass));

			return true;
		}
	case SUB_GP_MODIFY_INDIVIDUAL:	//修改资料
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(CMD_GP_ModifyIndividual));
			if (wDataSize<sizeof(CMD_GP_ModifyIndividual)) return false;

			//处理消息
			CMD_GP_ModifyIndividual * pModifyIndividual=(CMD_GP_ModifyIndividual *)pData;
			pModifyIndividual->szPassword[CountArray(pModifyIndividual->szPassword)-1]=0;

			//令牌参数
			tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParameter == NULL) return false;

			//变量定义
			DBR_GP_ModifyIndividual ModifyIndividual;
			ZeroMemory(&ModifyIndividual,sizeof(ModifyIndividual));

			//设置变量
			ModifyIndividual.dwUserID=pModifyIndividual->dwUserID;
			ModifyIndividual.dwClientIP= pTokenParameter->dwClientIP;
			StringCchCopy(ModifyIndividual.szPassword,CountArray(ModifyIndividual.szPassword),pModifyIndividual->szPassword);

			//变量定义
			VOID * pDataBuffer=NULL;
			tagDataDescribe DataDescribe;
			CRecvPacketHelper RecvPacket(pModifyIndividual+1,wDataSize-sizeof(CMD_GP_ModifyIndividual));

			//扩展信息
			while (true)
			{
				pDataBuffer=RecvPacket.GetData(DataDescribe);
				if (DataDescribe.wDataDescribe==DTP_NULL) break;
				switch (DataDescribe.wDataDescribe)
				{
				case DTP_GP_UI_USER_NOTE:		//用户备注
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szUserNote));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szUserNote))
						{
							CopyMemory(&ModifyIndividual.szUserNote,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szUserNote[CountArray(ModifyIndividual.szUserNote)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_COMPELLATION:	//真实名字
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szCompellation));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szCompellation))
						{
							CopyMemory(&ModifyIndividual.szCompellation,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szCompellation[CountArray(ModifyIndividual.szCompellation)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_SEAT_PHONE:		//固定电话
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szSeatPhone));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szSeatPhone))
						{
							CopyMemory(ModifyIndividual.szSeatPhone,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szSeatPhone[CountArray(ModifyIndividual.szSeatPhone)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_MOBILE_PHONE:	//移动电话
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szMobilePhone));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szMobilePhone))
						{
							CopyMemory(ModifyIndividual.szMobilePhone,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szMobilePhone[CountArray(ModifyIndividual.szMobilePhone)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_QQ:				//Q Q 号码
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szQQ));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szQQ))
						{
							CopyMemory(ModifyIndividual.szQQ,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szQQ[CountArray(ModifyIndividual.szQQ)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_EMAIL:			//电子邮件
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szEMail));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szEMail))
						{
							CopyMemory(ModifyIndividual.szEMail,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szEMail[CountArray(ModifyIndividual.szEMail)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_DWELLING_PLACE:	//联系地址
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szDwellingPlace));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szDwellingPlace))
						{
							CopyMemory(ModifyIndividual.szDwellingPlace,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szDwellingPlace[CountArray(ModifyIndividual.szDwellingPlace)-1]=0;
						}
						break;
					}
				}
			}

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_INDIVIDUAL, ModifyIndividual.dwUserID, dwSocketID,dwTokenID,&ModifyIndividual,sizeof(ModifyIndividual));

			return true;
		}	
	case SUB_GP_QUERY_INDIVIDUAL:	//查询信息
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_GP_QueryIndividual));
			if (wDataSize!=sizeof(CMD_GP_QueryIndividual)) return false;

			//处理消息
			CMD_GP_QueryIndividual * pQueryIndividual=(CMD_GP_QueryIndividual *)pData;

			//令牌参数
			tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParameter == NULL) return false;

			//变量定义
			DBR_GP_QueryIndividual QueryIndividual;
			ZeroMemory(&QueryIndividual,sizeof(QueryIndividual));

			//构造数据
			QueryIndividual.dwUserID=pQueryIndividual->dwUserID;
			QueryIndividual.dwClientIP= pTokenParameter->dwClientIP;

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_INDIVIDUAL, QueryIndividual.dwUserID,dwSocketID,dwTokenID,&QueryIndividual,sizeof(QueryIndividual));

			return true;
		}
	case SUB_GP_BIND_MACHINE:		//锁定机器
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_GP_BindMachine));
			if (wDataSize!=sizeof(CMD_GP_BindMachine)) return false;

			//处理消息
			CMD_GP_BindMachine * pBindMachine=(CMD_GP_BindMachine *)pData;
			pBindMachine->szPassword[CountArray(pBindMachine->szPassword)-1] = 0;
			pBindMachine->szMachineID[CountArray(pBindMachine->szMachineID)-1] = 0;

			//变量定义
			DBR_GP_BindMachine BindMachine = {0};

			//构造数据
			BindMachine.dwUserID=pBindMachine->dwUserID;
			StringCchCopy( BindMachine.szPassword,CountArray(BindMachine.szPassword), pBindMachine->szPassword);
			StringCchCopy( BindMachine.szMachineID,CountArray(BindMachine.szMachineID), pBindMachine->szMachineID);

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_BIND_MACHINE, BindMachine.dwUserID,dwSocketID,dwTokenID,&BindMachine,sizeof(BindMachine));

			return true;
		}
	case SUB_GP_UN_BIND_MACHINE:	//解锁机器
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_GP_UnbindMachine));
			if (wDataSize!=sizeof(CMD_GP_UnbindMachine)) return false;

			//处理消息
			CMD_GP_UnbindMachine * pUnbindMachine=(CMD_GP_UnbindMachine *)pData;
			pUnbindMachine->szPassword[CountArray(pUnbindMachine->szPassword)-1] = 0;
			pUnbindMachine->szMachineID[CountArray(pUnbindMachine->szMachineID)-1] = 0;

			//变量定义
			DBR_GP_UnbindMachine UnbindMachine = {0};

			//构造数据
			UnbindMachine.dwUserID=pUnbindMachine->dwUserID;
			StringCchCopy( UnbindMachine.szPassword,CountArray(UnbindMachine.szPassword), pUnbindMachine->szPassword);
			StringCchCopy( UnbindMachine.szMachineID,CountArray(UnbindMachine.szMachineID), pUnbindMachine->szMachineID);

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UN_BIND_MACHINE, UnbindMachine.dwUserID,dwSocketID,dwTokenID,&UnbindMachine,sizeof(UnbindMachine));

			return true;
		}	
	}

	return false;
}

//登录处理
bool CAttemperEngineSink::OnTCPNetworkMainMBLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_MB_LOGON_EMAIL:		//邮箱登陆
	{
		return OnTCPNetworkSubMBLogonEMail(pData, wDataSize, dwSocketID, dwTokenID);
	}
	case SUB_MB_LOGON_EMAILCODE:   //邮箱验证码
	{
		return OnTCPNetworkSubMBLogonEMailCode(pData, wDataSize, dwSocketID, dwTokenID);
	}
	case SUB_MB_LOGON_VISITOR:		//游客登录
	{
		return OnTCPNetworkSubMBLogonVisitor(pData,wDataSize,dwSocketID,dwTokenID);
	}
	case SUB_MB_LOGON_SECRETKEY:	//秘钥登录
	{
		return OnTCPNetworkSubMBLogonSecretKey(pData,wDataSize,dwSocketID,dwTokenID);
	}
	case SUB_MB_LOGON_ACCOUNTS:		//帐号登录
	{
		return OnTCPNetworkSubMBLogonAccounts(pData,wDataSize,dwSocketID,dwTokenID);			
	}	
	case SUB_MB_LOGON_MOBILECODE:	//手机验证码
	{
		return OnTCPNetworkSubMBLogonMobileCode(pData, wDataSize, dwSocketID, dwTokenID);
	}
	case SUB_MB_LOGON_THIRDPARTY:	//第三方登陆
	{
		return OnTCPNetworkSubMBLogonThirdParty(pData,wDataSize,dwSocketID,dwTokenID);			
	}
	case SUB_MB_LOGON_MOBILEPHONE:	//手机登陆
	{
		return OnTCPNetworkSubMBLogonMobilePhone(pData, wDataSize, dwSocketID, dwTokenID);
	}
	case SUB_MB_REGISTER_EMAIL:		//邮箱注册
	{
		return OnTCPNetworkSubMBRegisterEMail(pData, wDataSize, dwSocketID, dwTokenID);
	}	
	case SUB_MB_REGISTER_ACCOUNTS:	//帐号注册
	{
		return OnTCPNetworkSubMBRegisterAccounts(pData, wDataSize, dwSocketID, dwTokenID);
	}
	case SUB_MB_REGISTER_MOBILEPHONE://手机注册
	{
		return OnTCPNetworkSubMBRegisterMobilePhone(pData, wDataSize, dwSocketID, dwTokenID);
	}
	}

	return false;
}

//列表处理
bool CAttemperEngineSink::OnTCPNetworkMainMBServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_MB_GET_LIST:		//获取类型
		{			
			//效验数据
			ASSERT (wDataSize==sizeof(DWORD));
			if (wDataSize!=sizeof(DWORD)) return false;

			//获取参数
			auto pTokenParemeter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParemeter == NULL) return false;

			//发送游戏
			SendMobileKindInfo(dwSocketID,dwTokenID, (tagExtendParemter*)pTokenParemeter->cbExtendData);

			//发送完成
			SendDataToGate(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_FINISH,&dwTokenID,1);

			return true;
		}
	case SUB_MB_GET_SERVER:		//获取房间
		{
			//效验数据
			ASSERT (wDataSize==sizeof(tagGetServer));
			if (wDataSize!=sizeof(tagGetServer)) return false;

			//提取数据
			tagGetServer * pGetServer=(tagGetServer *)pData;

			//获取参数
			auto pTokenParemeter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParemeter == NULL) return false;

			//发送列表
			SendMobileServerInfo(dwSocketID, dwTokenID, pGetServer->wKindID, (tagExtendParemter*)pTokenParemeter->cbExtendData);

			//发送完成
			SendDataToGate(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_SERVER_FINISH,pData,wDataSize,&dwTokenID,1);

			return true;
		}	
	case SUB_MB_GET_ONLINE:		//获取在线
		{
			//效验数据
			ASSERT (wDataSize == sizeof(tagGetOnline));
			if (wDataSize != sizeof(tagGetOnline)) return false;

			//提取数据
			tagGetOnline* pGetOnline = (tagGetOnline*)pData;

			//获取参数
			auto pTokenParemeter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParemeter == NULL) return false;

			//发送在线信息
			SendMobileOnlineInfo(dwSocketID, dwTokenID, pGetOnline->wKindID, (tagExtendParemter*)pTokenParemeter->cbExtendData);

			return true;
		}
	case SUB_MB_GET_JACKPOT:	//获取奖池
		{
			//获取参数
			auto pTokenParemeter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParemeter == NULL) return false;

			//发送奖池信息
			SendMobileJackpotInfo(dwSocketID, dwTokenID, (tagExtendParemter*)pTokenParemeter->cbExtendData);

			return true;
		}
	}

	return false;
}

//用户服务
bool CAttemperEngineSink::OnTCPNetworkMainMBUserService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{	
	case SUB_MB_LIKE_PLAYER:		//点赞玩家
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_LikePlayer));
		if (wDataSize != sizeof(CMD_MB_LikePlayer)) return false;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//提取数据
		CMD_MB_LikePlayer* pLikePlayer = (CMD_MB_LikePlayer*)pData;

		//变量定义
		DBR_MB_LikePlayer LikePlayer;
		ZeroMemory(&LikePlayer, sizeof(LikePlayer));

		//构造数据
		LikePlayer.dwUserID = pTokenParameter->dwUserID;
		LikePlayer.dwDstUserID = pLikePlayer->dwDstUserID;
		LikePlayer.dwClientIP = pTokenParameter->dwClientIP;				

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LIKE_PLAYER, LikePlayer.dwUserID, dwSocketID, dwTokenID, &LikePlayer, sizeof(LikePlayer));

		return true;
	}
	case SUB_MB_MODIFY_LOGON_PASS:	//修改密码
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_ModifyLogonPass));
		if (wDataSize != sizeof(CMD_MB_ModifyLogonPass)) return false;

		//处理消息
		CMD_MB_ModifyLogonPass* pModifyLogonPass = (CMD_MB_ModifyLogonPass*)pData;
		pModifyLogonPass->szDesPassword[CountArray(pModifyLogonPass->szDesPassword) - 1] = 0;
		pModifyLogonPass->szScrPassword[CountArray(pModifyLogonPass->szScrPassword) - 1] = 0;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_ModifyLogonPass ModifyLogonPass;
		ZeroMemory(&ModifyLogonPass, sizeof(ModifyLogonPass));

		//构造数据
		ModifyLogonPass.dwUserID = pTokenParameter->dwUserID;
		ModifyLogonPass.dwClientIP = pTokenParameter->dwClientIP;		
		StringCchCopy(ModifyLogonPass.szDesPassword, CountArray(ModifyLogonPass.szDesPassword), pModifyLogonPass->szDesPassword);
		StringCchCopy(ModifyLogonPass.szScrPassword, CountArray(ModifyLogonPass.szScrPassword), pModifyLogonPass->szScrPassword);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_MODIFY_LOGON_PASS, ModifyLogonPass.dwUserID, dwSocketID, dwTokenID, &ModifyLogonPass, sizeof(ModifyLogonPass));

		return true;
	}	
	case SUB_MB_MODIFY_INDIVIDUAL:	//修改资料
	{
		//效验参数
		ASSERT(wDataSize >= sizeof(CMD_MB_ModifyIndividual));
		if (wDataSize < sizeof(CMD_MB_ModifyIndividual)) return false;

		//处理消息
		CMD_MB_ModifyIndividual* pModifyIndividual = (CMD_MB_ModifyIndividual*)pData;
		
		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_ModifyIndividual ModifyIndividual;
		ZeroMemory(&ModifyIndividual, sizeof(ModifyIndividual));

		//设置变量
		ModifyIndividual.dwUserID = pTokenParameter->dwUserID;
		ModifyIndividual.cbGender = pModifyIndividual->cbGender;
		ModifyIndividual.dwClientIP = pTokenParameter->dwClientIP;		
		
		//变量定义
		VOID* pDataBuffer = NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pModifyIndividual + 1, wDataSize - sizeof(CMD_MB_ModifyIndividual));

		//扩展信息
		while (true)
		{
			pDataBuffer = RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe == DTP_NULL) break;
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_MB_NICKNAME:			//用户昵称
			{
				ASSERT(pDataBuffer != NULL);
				ASSERT(DataDescribe.wDataSize <= sizeof(ModifyIndividual.szNickName));
				if (DataDescribe.wDataSize <= sizeof(ModifyIndividual.szNickName))
				{
					ModifyIndividual.wInfoMask |= DTP_MB_NICKNAME;
					CopyMemory(&ModifyIndividual.szNickName, pDataBuffer, DataDescribe.wDataSize);
					ModifyIndividual.szNickName[CountArray(ModifyIndividual.szNickName) - 1] = 0;
				}
				break;
			}
			case DTP_MB_UNDER_WRITE:		//个性签名
			{
				ASSERT(pDataBuffer != NULL);
				ASSERT(DataDescribe.wDataSize <= sizeof(ModifyIndividual.szUnderWrite));
				if (DataDescribe.wDataSize <= sizeof(ModifyIndividual.szUnderWrite))
				{
					ModifyIndividual.wInfoMask |= DTP_MB_UNDER_WRITE;
					CopyMemory(&ModifyIndividual.szUnderWrite, pDataBuffer, DataDescribe.wDataSize);
					ModifyIndividual.szUnderWrite[CountArray(ModifyIndividual.szUnderWrite) - 1] = 0;
				}
				break;
			}
			}
		}

		//校验昵称
		if ((ModifyIndividual.wInfoMask&DTP_MB_NICKNAME)!=0 && m_WordsFilter.FilterWords(ModifyIndividual.szNickName))
		{
			//操作失败
			SendOperateFailure(dwSocketID, dwTokenID, SUB_MB_MODIFY_INDIVIDUAL, -1, CWHService::FormatMessage(MIM_LOGON_0009, TEXT("抱歉，您所输入的昵称含有限制字符串，请更换昵称后尝试再次修改！")));

			return true;
		}

		//校验签名
		if ((ModifyIndividual.wInfoMask&DTP_MB_UNDER_WRITE)!=0 && m_WordsFilter.FilterWords(ModifyIndividual.szUnderWrite))
		{
			//操作失败
			SendOperateFailure(dwSocketID, dwTokenID, SUB_MB_MODIFY_INDIVIDUAL, -1, CWHService::FormatMessage(MIM_LOGON_0010, TEXT("抱歉，您所输入的个性签名含有限制字符串，请更换后尝试再次修改！")));

			return true;
		}

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_MODIFY_INDIVIDUAL, ModifyIndividual.dwUserID, dwSocketID, dwTokenID, &ModifyIndividual, sizeof(ModifyIndividual));

		return true;
	}
	case SUB_MB_QUERY_WEALTH:		//查询财富
	{
		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_QueryWealth QueryWealth;
		ZeroMemory(&QueryWealth, sizeof(QueryWealth));

		//构造数据
		QueryWealth.dwUserID = pTokenParameter->dwUserID;
		QueryWealth.dwClientIP = pTokenParameter->dwClientIP;		

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_QUERY_WEALTH, QueryWealth.dwUserID, dwSocketID, dwTokenID, &QueryWealth, sizeof(QueryWealth));

		return true;
	}
	case SUB_MB_QUERY_RANKINGINFO:	//查询排位
	{
		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_QueryRankingInfo QueryRankingInfo;
		ZeroMemory(&QueryRankingInfo, sizeof(QueryRankingInfo));

		//构造数据
		QueryRankingInfo.dwUserID = pTokenParameter->dwUserID;
		QueryRankingInfo.dwClientIP = pTokenParameter->dwClientIP;		

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_QUERY_RANKINGINFO, QueryRankingInfo.dwUserID, dwSocketID, dwTokenID, &QueryRankingInfo, sizeof(QueryRankingInfo));

		return true;
	}	
	case SUB_MB_MODIFY_SYSTEM_FACE:	//修改头像
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_ModifySystemFace));
		if (wDataSize != sizeof(CMD_MB_ModifySystemFace)) return false;

		//处理消息
		CMD_MB_ModifySystemFace* pModifySystemFace = (CMD_MB_ModifySystemFace*)pData;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_ModifySystemFace ModifySystemFace;
		ZeroMemory(&ModifySystemFace, sizeof(ModifySystemFace));

		//构造数据
		ModifySystemFace.dwUserID = pTokenParameter->dwUserID;
		ModifySystemFace.wFaceID = pModifySystemFace->wFaceID;
		ModifySystemFace.dwClientIP = pTokenParameter->dwClientIP;		
		StringCchCopy(ModifySystemFace.szMachineID, CountArray(ModifySystemFace.szMachineID), pModifySystemFace->szMachineID);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_MODIFY_SYSTEM_FACE, ModifySystemFace.dwUserID, dwSocketID, dwTokenID, &ModifySystemFace, sizeof(ModifySystemFace));

		return true;
	}
	case SUB_MB_MODIFY_CUSTOM_FACE:	//修改头像
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_ModifyCustomFace));
		if (wDataSize != sizeof(CMD_MB_ModifyCustomFace)) return false;

		//处理消息
		CMD_MB_ModifyCustomFace* pModifyCustomFace = (CMD_MB_ModifyCustomFace*)pData;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_ModifyCustomFace ModifyCustomFace;
		ZeroMemory(&ModifyCustomFace, sizeof(ModifyCustomFace));

		//构造数据
		ModifyCustomFace.dwUserID = pTokenParameter->dwUserID;
		ModifyCustomFace.dwCustomID = pModifyCustomFace->dwCustomID;				
		StringCchCopy(ModifyCustomFace.szFacePath,CountArray(ModifyCustomFace.szFacePath), pModifyCustomFace->szFacePath);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_MODIFY_CUSTOM_FACE, ModifyCustomFace.dwUserID, dwSocketID, dwTokenID, &ModifyCustomFace, sizeof(ModifyCustomFace));

		return true;
	}	
	case SUB_MB_EXCHANGE_MALLGOODS: //兑换商品
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_ExchangeMallGoods));
		if (wDataSize != sizeof(CMD_MB_ExchangeMallGoods)) return false;

		//处理消息
		CMD_MB_ExchangeMallGoods* pExchangeMallGoods = (CMD_MB_ExchangeMallGoods*)pData;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_ExchangeMallGoods ExchangeMallGoods;
		ZeroMemory(&ExchangeMallGoods, sizeof(ExchangeMallGoods));

		//构造数据
		ExchangeMallGoods.dwUserID = pTokenParameter->dwUserID;
		ExchangeMallGoods.dwMallGoodsID = pExchangeMallGoods->dwMallGoodsID;		

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_EXCHANGE_MALLGOODS, ExchangeMallGoods.dwUserID, dwSocketID, dwTokenID, &ExchangeMallGoods, sizeof(ExchangeMallGoods));

		return true;
	}
	case SUB_MB_EXCHANGE_GIFTBAGCODE: //兑换礼包码
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_ExchangeGiftbagCode));
		if (wDataSize != sizeof(CMD_MB_ExchangeGiftbagCode)) return false;

		//处理消息
		CMD_MB_ExchangeGiftbagCode* pExchangeGiftbagCode = (CMD_MB_ExchangeGiftbagCode*)pData;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_ExchangeGiftbagCode ExchangeGiftbagCode;
		ZeroMemory(&ExchangeGiftbagCode, sizeof(ExchangeGiftbagCode));

		//构造数据
		ExchangeGiftbagCode.dwUserID = pTokenParameter->dwUserID;
		ExchangeGiftbagCode.dwClientIP = pTokenParameter->dwClientIP;		
		StringCchCopy(ExchangeGiftbagCode.szGiftbagCode,CountArray(ExchangeGiftbagCode.szGiftbagCode), pExchangeGiftbagCode->szGiftbagCode);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_EXCHANGE_GIFTBAGCODE, ExchangeGiftbagCode.dwUserID, dwSocketID, dwTokenID, &ExchangeGiftbagCode, sizeof(ExchangeGiftbagCode));

		return true;
	}
	case SUB_MB_ACQUIRE_AUTHCODE:	//获取验证码
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_AcquireAuthCode));
		if (wDataSize != sizeof(CMD_MB_AcquireAuthCode)) return false;

		//处理消息
		CMD_MB_AcquireAuthCode* pAcquireAuthCode = (CMD_MB_AcquireAuthCode*)pData;
		pAcquireAuthCode->szPassword[CountArray(pAcquireAuthCode->szPassword) - 1] = 0;
		pAcquireAuthCode->szMachineID[CountArray(pAcquireAuthCode->szMachineID) - 1] = 0;
		pAcquireAuthCode->szMobileEmail[CountArray(pAcquireAuthCode->szMobileEmail) - 1] = 0;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		
		//变量定义
		DBR_MB_AcquireAuthCode AcquireAuthCode;
		ZeroMemory(&AcquireAuthCode, sizeof(AcquireAuthCode));

		//用户信息
		AcquireAuthCode.dwUserID = pAcquireAuthCode->dwUserID;
		StringCchCopy(AcquireAuthCode.szPassword, CountArray(AcquireAuthCode.szPassword), pAcquireAuthCode->szPassword);

		//验证信息
		AcquireAuthCode.cbCodeKind = pAcquireAuthCode->cbCodeKind;
		AcquireAuthCode.cbAuthKind = pAcquireAuthCode->cbAuthKind;
		AcquireAuthCode.dwBindUserID = pAcquireAuthCode->dwBindUserID;
		StringCchCopy(AcquireAuthCode.szMobileEmail, CountArray(AcquireAuthCode.szMobileEmail), pAcquireAuthCode->szMobileEmail);

		//生成验证码
		MakeAuthCode(AcquireAuthCode.szAuthCode, CountArray(AcquireAuthCode.szAuthCode));

		//加密验证码			
		WCHAR szValidateCode[64];
		_snwprintf_s(szValidateCode, CountArray(szValidateCode), L"AutchCode:%c-%c-%c-%c-%c-%c", AcquireAuthCode.szAuthCode[0], AcquireAuthCode.szAuthCode[1], AcquireAuthCode.szAuthCode[2], AcquireAuthCode.szAuthCode[3], AcquireAuthCode.szAuthCode[4], AcquireAuthCode.szAuthCode[5]);

		//加密验证码
		CMD5Encrypt::EncryptData(szValidateCode, AcquireAuthCode.szAuthCodeMD5);

		//连接信息
		AcquireAuthCode.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(AcquireAuthCode.szMachineID, CountArray(AcquireAuthCode.szMachineID), pAcquireAuthCode->szMachineID);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_ACQUIRE_AUTHCODE, AcquireAuthCode.dwUserID, dwSocketID, dwTokenID, &AcquireAuthCode, sizeof(AcquireAuthCode));

		return true;
	}
	case SUB_MB_BIND_MOBILEPHONE:	//绑定手机号
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_BindMobilePhone));
		if (wDataSize != sizeof(CMD_MB_BindMobilePhone)) return false;

		//处理消息
		CMD_MB_BindMobilePhone* pBindMobilePhone = (CMD_MB_BindMobilePhone*)pData;
		pBindMobilePhone->szPassword[CountArray(pBindMobilePhone->szPassword) - 1] = 0;
		pBindMobilePhone->szNewPassword[CountArray(pBindMobilePhone->szNewPassword) - 1] = 0;
		pBindMobilePhone->szMachineID[CountArray(pBindMobilePhone->szMachineID) - 1] = 0;
		pBindMobilePhone->szAuthCode[CountArray(pBindMobilePhone->szAuthCode) - 1] = 0;
		pBindMobilePhone->szMobilePhone[CountArray(pBindMobilePhone->szMobilePhone) - 1] = 0;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;

		//变量定义
		DBR_MB_BindMobilePhone BindMobilePhone;
		ZeroMemory(&BindMobilePhone, sizeof(BindMobilePhone));

		//用户信息
		BindMobilePhone.dwUserID = pBindMobilePhone->dwUserID;
		StringCchCopy(BindMobilePhone.szPassword, CountArray(BindMobilePhone.szPassword), pBindMobilePhone->szPassword);
		StringCchCopy(BindMobilePhone.szNewPassword, CountArray(BindMobilePhone.szNewPassword), pBindMobilePhone->szNewPassword);

		//验证信息
		BindMobilePhone.dwUnbindUserID = pBindMobilePhone->dwUnbindUserID;
		StringCchCopy(BindMobilePhone.szAuthCode, CountArray(BindMobilePhone.szAuthCode), pBindMobilePhone->szAuthCode);
		StringCchCopy(BindMobilePhone.szMobilePhone, CountArray(BindMobilePhone.szMobilePhone), pBindMobilePhone->szMobilePhone);

		//连接信息			
		BindMobilePhone.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(BindMobilePhone.szMachineID, CountArray(BindMobilePhone.szMachineID), pBindMobilePhone->szMachineID);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_BIND_MOBILEPHONE, BindMobilePhone.dwUserID, dwSocketID, dwTokenID, &BindMobilePhone, sizeof(BindMobilePhone));

		return true;
	}
	case SUB_MB_UNBIND_MOBILEPHONE:	//解绑手机号
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_UnbindMobilePhone));
		if (wDataSize != sizeof(CMD_MB_UnbindMobilePhone)) return false;

		//处理消息
		CMD_MB_UnbindMobilePhone* pUnbindMobilePhone = (CMD_MB_UnbindMobilePhone*)pData;
		pUnbindMobilePhone->szPassword[CountArray(pUnbindMobilePhone->szPassword) - 1] = 0;
		pUnbindMobilePhone->szMachineID[CountArray(pUnbindMobilePhone->szMachineID) - 1] = 0;
		pUnbindMobilePhone->szAuthCode[CountArray(pUnbindMobilePhone->szAuthCode) - 1] = 0;
		pUnbindMobilePhone->szMobilePhone[CountArray(pUnbindMobilePhone->szMobilePhone) - 1] = 0;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;

		//变量定义
		DBR_MB_UnbindMobilePhone UnbindMobilePhone;
		ZeroMemory(&UnbindMobilePhone, sizeof(UnbindMobilePhone));

		//用户信息
		UnbindMobilePhone.dwUserID = pUnbindMobilePhone->dwUserID;
		StringCchCopy(UnbindMobilePhone.szPassword, CountArray(UnbindMobilePhone.szPassword), pUnbindMobilePhone->szPassword);

		//验证信息
		StringCchCopy(UnbindMobilePhone.szAuthCode, CountArray(UnbindMobilePhone.szAuthCode), pUnbindMobilePhone->szAuthCode);
		StringCchCopy(UnbindMobilePhone.szMobilePhone, CountArray(UnbindMobilePhone.szMobilePhone), pUnbindMobilePhone->szMobilePhone);

		//连接信息		
		UnbindMobilePhone.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(UnbindMobilePhone.szMachineID, CountArray(UnbindMobilePhone.szMachineID),pUnbindMobilePhone->szMachineID);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_UNBIND_MOBILEPHONE, UnbindMobilePhone.dwUserID, dwSocketID, dwTokenID, &UnbindMobilePhone, sizeof(UnbindMobilePhone));

		return true;
	}
	case SUB_MB_BIND_EMAIL:			//绑定邮箱
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_BindEMail));
		if (wDataSize != sizeof(CMD_MB_BindEMail)) return false;

		//处理消息
		CMD_MB_BindEMail* pBindEMail = (CMD_MB_BindEMail*)pData;
		pBindEMail->szPassword[CountArray(pBindEMail->szPassword) - 1] = 0;
		pBindEMail->szNewPassword[CountArray(pBindEMail->szNewPassword) - 1] = 0;
		pBindEMail->szMachineID[CountArray(pBindEMail->szMachineID) - 1] = 0;
		pBindEMail->szAuthCode[CountArray(pBindEMail->szAuthCode) - 1] = 0;
		pBindEMail->szEMail[CountArray(pBindEMail->szEMail) - 1] = 0;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;

		//变量定义
		DBR_MB_BindEMail BindEMail;
		ZeroMemory(&BindEMail, sizeof(BindEMail));

		//用户信息
		BindEMail.dwUserID = pBindEMail->dwUserID;
		StringCchCopy(BindEMail.szPassword, CountArray(BindEMail.szPassword), pBindEMail->szPassword);
		StringCchCopy(BindEMail.szNewPassword, CountArray(BindEMail.szNewPassword), pBindEMail->szNewPassword);

		//验证信息
		BindEMail.dwUnbindUserID = pBindEMail->dwUnbindUserID;
		StringCchCopy(BindEMail.szAuthCode, CountArray(BindEMail.szAuthCode), pBindEMail->szAuthCode);
		StringCchCopy(BindEMail.szEMail, CountArray(BindEMail.szEMail), pBindEMail->szEMail);

		//连接信息			
		BindEMail.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(BindEMail.szMachineID, CountArray(BindEMail.szMachineID), pBindEMail->szMachineID);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_BIND_EMAIL, BindEMail.dwUserID, dwSocketID, dwTokenID, &BindEMail, sizeof(BindEMail));

		return true;
	}
	case SUB_MB_UNBIND_EMAIL:		//解绑邮箱
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_UnbindEMail));
		if (wDataSize != sizeof(CMD_MB_UnbindEMail)) return false;

		//处理消息
		CMD_MB_UnbindEMail* pUnbindEMail = (CMD_MB_UnbindEMail*)pData;
		pUnbindEMail->szPassword[CountArray(pUnbindEMail->szPassword) - 1] = 0;
		pUnbindEMail->szMachineID[CountArray(pUnbindEMail->szMachineID) - 1] = 0;
		pUnbindEMail->szAuthCode[CountArray(pUnbindEMail->szAuthCode) - 1] = 0;
		pUnbindEMail->szEMail[CountArray(pUnbindEMail->szEMail) - 1] = 0;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;

		//变量定义
		DBR_MB_UnbindEMail UnbindEMail;
		ZeroMemory(&UnbindEMail, sizeof(UnbindEMail));

		//用户信息
		UnbindEMail.dwUserID = pUnbindEMail->dwUserID;
		StringCchCopy(UnbindEMail.szPassword, CountArray(UnbindEMail.szPassword), pUnbindEMail->szPassword);

		//验证信息
		StringCchCopy(UnbindEMail.szAuthCode, CountArray(UnbindEMail.szAuthCode), pUnbindEMail->szAuthCode);
		StringCchCopy(UnbindEMail.szEMail, CountArray(UnbindEMail.szEMail), pUnbindEMail->szEMail);

		//连接信息		
		UnbindEMail.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(UnbindEMail.szMachineID, CountArray(UnbindEMail.szMachineID), pUnbindEMail->szMachineID);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_UNBIND_EMAIL, UnbindEMail.dwUserID, dwSocketID, dwTokenID, &UnbindEMail, sizeof(UnbindEMail));

		return true;
	}
	case SUB_MB_RESET_LOGON_PASS:	//重置密码
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_ResetLogonPasswd));
		if (wDataSize != sizeof(CMD_MB_ResetLogonPasswd)) return false;

		//处理消息
		CMD_MB_ResetLogonPasswd* pResetLogonPasswd = (CMD_MB_ResetLogonPasswd*)pData;
		pResetLogonPasswd->szPassword[CountArray(pResetLogonPasswd->szPassword) - 1] = 0;
		pResetLogonPasswd->szMachineID[CountArray(pResetLogonPasswd->szMachineID) - 1] = 0;
		pResetLogonPasswd->szAuthCode[CountArray(pResetLogonPasswd->szAuthCode) - 1] = 0;
		pResetLogonPasswd->szMobileEmail[CountArray(pResetLogonPasswd->szMobileEmail) - 1] = 0;

		//变量定义
		DBR_MB_ResetLogonPasswd ResetLogonPasswd;
		ZeroMemory(&ResetLogonPasswd, sizeof(ResetLogonPasswd));

		//验证信息
		ResetLogonPasswd.cbCodeKind = pResetLogonPasswd->cbCodeKind;
		ResetLogonPasswd.dwBindUserID = pResetLogonPasswd->dwBindUserID;
		StringCchCopy(ResetLogonPasswd.szPassword, CountArray(ResetLogonPasswd.szPassword), pResetLogonPasswd->szPassword);
		StringCchCopy(ResetLogonPasswd.szAuthCode, CountArray(ResetLogonPasswd.szAuthCode), pResetLogonPasswd->szAuthCode);
		StringCchCopy(ResetLogonPasswd.szMobileEmail, CountArray(ResetLogonPasswd.szMobileEmail), pResetLogonPasswd->szMobileEmail);

		//连接信息			
		StringCchCopy(ResetLogonPasswd.szMachineID, CountArray(ResetLogonPasswd.szMachineID), pResetLogonPasswd->szMachineID);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_RESET_LOGON_PASS, ResetLogonPasswd.dwBindUserID, dwSocketID, dwTokenID, &ResetLogonPasswd, sizeof(ResetLogonPasswd));

		return true;
	}	
	case SUB_MB_REQUEST_CHECKIN:	 //请求签到
	{
		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//提取数据
		auto pRequestCheckIn = (CMD_MB_RequestCheckIn*)pData;

		//签到配置
		BOOL bQuery = false;
		auto CheckInOptionPtr = g_GlobalOptionManager->GetOptionItem<CCheckInOptionPtr>(OPTION_MASK_CHECKIN);
		if (CheckInOptionPtr && lstrcmp(pRequestCheckIn->szOptionDigest, CheckInOptionPtr->GetOptionDigest()) != 0)
		{
			bQuery = true;

			//签到配置
			CMD_MB_CheckInOption CheckInOption = {};

			//设置变量
			CheckInOption.bCheckInEnabled = CheckInOptionPtr->IsCheckInEnabled();
			StringCchCopy(CheckInOption.szOptionDigest, CountArray(CheckInOption.szOptionDigest), CheckInOptionPtr->GetOptionDigest());
			StringCchCopy(CheckInOption.szCheckInRewards, CountArray(CheckInOption.szCheckInRewards), CheckInOptionPtr->GetCheckInRewards());

			//签到配置
			WORD wStringSize = CountStringBuffer(CheckInOption.szCheckInRewards);
			WORD wPacketSize = sizeof(CheckInOption) - sizeof(CheckInOption.szCheckInRewards) + wStringSize;
			SendDataToGate(dwSocketID, MDM_MB_USER_SERVICE, SUB_MB_CHECKIN_OPTION, &CheckInOption, wPacketSize, &dwTokenID, 1);
		}

		//变量定义
		DBR_MB_RequestCheckIn RequestCheckIn;
		ZeroMemory(&RequestCheckIn, sizeof(RequestCheckIn));

		//用户信息
		RequestCheckIn.dwUserID = pTokenParameter->dwUserID;
		RequestCheckIn.bQuery = bQuery;
		RequestCheckIn.dwClientIP = pTokenParameter->dwClientIP;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_REQUEST_CHECKIN, RequestCheckIn.dwUserID, dwSocketID, dwTokenID, &RequestCheckIn, sizeof(RequestCheckIn));

		return true;
	}
	case SUB_MB_BIND_INVITE_CODE:	//绑定邀请码
	{
        //效验参数
        ASSERT(wDataSize == sizeof(CMD_MB_BindInviteCode));
        if (wDataSize != sizeof(CMD_MB_BindInviteCode)) return false;

        //处理消息
		CMD_MB_BindInviteCode* pBindInviteCode = (CMD_MB_BindInviteCode*)pData;
		pBindInviteCode->szMachineID[CountArray(pBindInviteCode->szMachineID) - 1] = 0;

        //令牌参数
        tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
        if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

        //变量定义
		DBR_MB_BindInviteCode BindInviteCode;
        ZeroMemory(&BindInviteCode, sizeof(BindInviteCode));

        //用户信息
		BindInviteCode.dwUserID = pTokenParameter->dwUserID;
        BindInviteCode.dwInviteCode = pBindInviteCode->dwInviteCode;		
        StringCchCopy(BindInviteCode.szPassword, CountArray(BindInviteCode.szPassword), pBindInviteCode->szPassword);

        //连接信息			
		BindInviteCode.dwClientIP = pTokenParameter->dwClientIP;
        StringCchCopy(BindInviteCode.szMachineID, CountArray(BindInviteCode.szMachineID), pBindInviteCode->szMachineID);

        //投递请求
        m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_BIND_INVITE_CODE, BindInviteCode.dwUserID, dwSocketID, dwTokenID, &BindInviteCode, sizeof(BindInviteCode));

        return true;
	}
	case SUB_MB_SUBMIT_INFORM_INFO:	//提交投诉
	{
		//效验参数
		ASSERT (wDataSize >= sizeof(CMD_MB_SubmitInformInfo));
		if (wDataSize < sizeof(CMD_MB_SubmitInformInfo)) return false;

		//提取数据
		auto pSubmitInformInfo = (CMD_MB_SubmitInformInfo*)pData;
		
		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//构造结构
		DBR_MB_SubmitInformInfo SubmitInformInfo;
		ZeroMemory(&SubmitInformInfo, sizeof(SubmitInformInfo));

		//设置变量
		SubmitInformInfo.dwInformUserID = pTokenParameter->dwUserID;
        SubmitInformInfo.cbInformKind = pSubmitInformInfo->cbInformKind;
        SubmitInformInfo.cbInformSource = pSubmitInformInfo->cbInformSource;
		SubmitInformInfo.dwBeInformUserID = pSubmitInformInfo->dwBeInformUserID;		

        //变量定义
        VOID* pDataBuffer = NULL;
        tagDataDescribe DataDescribe;
        CRecvPacketHelper RecvPacket(pSubmitInformInfo + 1, wDataSize - sizeof(CMD_MB_SubmitInformInfo));

        //扩展信息
        while (true)
        {
            pDataBuffer = RecvPacket.GetData(DataDescribe);
            if (DataDescribe.wDataDescribe == DTP_NULL) break;
            switch (DataDescribe.wDataDescribe)
            {
            case DTP_MB_INFORM_ATTACHMENTURL:			//附件地址
            {
                ASSERT(pDataBuffer != NULL);
                ASSERT(DataDescribe.wDataSize <= sizeof(SubmitInformInfo.szAttachmentUrl));
                if (DataDescribe.wDataSize <= sizeof(SubmitInformInfo.szAttachmentUrl))
                {
                    CopyMemory(&SubmitInformInfo.szAttachmentUrl, pDataBuffer, DataDescribe.wDataSize);
					SubmitInformInfo.szAttachmentUrl[CountArray(SubmitInformInfo.szAttachmentUrl) - 1] = 0;
                }
                break;
            }
            case DTP_MB_INFORM_MESSAGE:					//投诉消息
            {
                ASSERT(pDataBuffer != NULL);
                ASSERT(DataDescribe.wDataSize <= sizeof(SubmitInformInfo.szInformMessage));
                if (DataDescribe.wDataSize <= sizeof(SubmitInformInfo.szInformMessage))
                {
                    CopyMemory(&SubmitInformInfo.szInformMessage, pDataBuffer, DataDescribe.wDataSize);
                    SubmitInformInfo.szInformMessage[CountArray(SubmitInformInfo.szInformMessage) - 1] = 0;
                }
                break;
            }
            case DTP_MB_INFORM_CONTENT:					//投诉内容
            {
                ASSERT(pDataBuffer != NULL);
                ASSERT(DataDescribe.wDataSize <= sizeof(SubmitInformInfo.szInformContent));
                if (DataDescribe.wDataSize <= sizeof(SubmitInformInfo.szInformContent))
                {
                    CopyMemory(&SubmitInformInfo.szInformContent, pDataBuffer, DataDescribe.wDataSize);
                    SubmitInformInfo.szInformContent[CountArray(SubmitInformInfo.szInformContent) - 1] = 0;
                }
                break;
            }
            }
        }

		//连接信息			
		SubmitInformInfo.dwClientIP = pTokenParameter->dwClientIP;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_SUBMIT_INFORM_INFO, SubmitInformInfo.dwInformUserID, dwSocketID, dwTokenID, &SubmitInformInfo, sizeof(SubmitInformInfo));

		return true;
	}
	case SUB_MB_BIND_EXTERNAL_ACCOUNT://绑定账号
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_BindExternalAccount));
		if (wDataSize != sizeof(CMD_MB_BindExternalAccount)) return false;

		//处理消息
		auto pBindExternalAccount = (CMD_MB_BindExternalAccount*)pData;
		
		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_BindExternalAccount BindExternalAccount;
		ZeroMemory(&BindExternalAccount, sizeof(BindExternalAccount));

		//外部信息
		BindExternalAccount.dwUserID = pTokenParameter->dwUserID;
		BindExternalAccount.dwExternalID = pBindExternalAccount->dwExternalID;		
		StringCchCopy(BindExternalAccount.szUniqueID, CountArray(BindExternalAccount.szUniqueID), pBindExternalAccount->szUniqueID);

		//连接信息			
		BindExternalAccount.dwClientIP = pTokenParameter->dwClientIP;

		//扩展信息
		if (wDataSize > sizeof(CMD_MB_BindExternalAccount))
		{
			//变量定义
			VOID* pDataBuffer = NULL;
			tagDataDescribe DataDescribe;
			CRecvPacketHelper RecvPacket(pBindExternalAccount + 1, wDataSize - sizeof(CMD_MB_BindExternalAccount));

			//读取信息
			while (true)
			{
				//提取数据
				pDataBuffer = RecvPacket.GetData(DataDescribe);
				if (DataDescribe.wDataDescribe == DTP_NULL) break;

				//提取数据
				switch (DataDescribe.wDataDescribe)
				{
				case DTP_MB_USERFACE_URL:		//头像地址
				{
					ASSERT(DataDescribe.wDataSize <= sizeof(BindExternalAccount.szUserFaceUrl));
					if (DataDescribe.wDataSize <= sizeof(BindExternalAccount.szUserFaceUrl))
					{
						BindExternalAccount.szUserFaceUrl[CountArray(BindExternalAccount.szUserFaceUrl) - 1L] = 0;
						StringCchCopy(BindExternalAccount.szUserFaceUrl, CountArray(BindExternalAccount.szUserFaceUrl), (LPCTSTR)pDataBuffer);
					}

					break;
				}
				case DTP_MB_EXTERNAL_DATUM:		//外部资料
				{
					ASSERT(DataDescribe.wDataSize <= sizeof(BindExternalAccount.szUserDatum));
					if (DataDescribe.wDataSize <= sizeof(BindExternalAccount.szUserDatum))
					{
						BindExternalAccount.szUserDatum[CountArray(BindExternalAccount.szUserDatum) - 1L] = 0;
						StringCchCopy(BindExternalAccount.szUserDatum, CountArray(BindExternalAccount.szUserDatum), (LPCTSTR)pDataBuffer);
					}

					break;
				}
				}
			}
		}		

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_BIND_EXTERNAL_ACCOUNT, BindExternalAccount.dwUserID, dwSocketID, dwTokenID, &BindExternalAccount, sizeof(BindExternalAccount));

		return true;
	}
	case SUB_MB_SEND_GIFT:			 //赠送礼物
	{
        //效验参数
        ASSERT(wDataSize == sizeof(CMD_MB_SendGift));
        if (wDataSize != sizeof(CMD_MB_SendGift)) return false;

        //处理消息
        auto pSendGift = (CMD_MB_SendGift*)pData;

        //令牌参数
        tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
        if (pTokenParameter == NULL) return false;
        if (pTokenParameter->pBindUserItem == NULL) return false;

        //变量定义
		DBR_MB_SendGift SendGift;
        ZeroMemory(&SendGift, sizeof(SendGift));

		//设置变量
		SendGift.dwUserID = pTokenParameter->dwUserID;
		SendGift.dwTargetUserID = pSendGift->dwUserID;
		SendGift.dwGoodsID = pSendGift->dwGoodsID;
		SendGift.dwGoodsCount = pSendGift->dwGoodsCount;

        //连接信息			
		SendGift.dwClientIP = pTokenParameter->dwClientIP;

        //投递请求
        m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_SEND_GIFT, SendGift.dwUserID, dwSocketID, dwTokenID, &SendGift, sizeof(SendGift));

		return true;
	}
	case SUB_MB_LOAD_GIFT:			 //加载礼物
	{
        //效验参数
        ASSERT(wDataSize == sizeof(CMD_MB_LoadGift));
        if (wDataSize != sizeof(CMD_MB_LoadGift)) return false;

        //处理消息
        auto pLoadGift = (CMD_MB_LoadGift*)pData;

        //令牌参数
        tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
        if (pTokenParameter == NULL) return false;
        if (pTokenParameter->pBindUserItem == NULL) return false;

        //变量定义
		DBR_MB_LoadGift LoadGift;
        ZeroMemory(&LoadGift, sizeof(LoadGift));

        //设置变量
		LoadGift.dwUserID = pTokenParameter->dwUserID;
        //LoadGift.dwRecordID = pLoadGift->dwRecordID;

        //投递请求
        m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOAD_GIFT, LoadGift.dwUserID, dwSocketID, dwTokenID, &LoadGift, sizeof(LoadGift));

        return true;
	}
	case SUB_MB_GIFT_STATUS_UPDATE:		//礼物状态更新
	{
        //效验参数
        ASSERT(wDataSize == sizeof(CMD_MB_GiftStatusUpdate));
        if (wDataSize != sizeof(CMD_MB_GiftStatusUpdate)) return false;

        //处理消息
        auto pGiftStatusUpdate = (CMD_MB_GiftStatusUpdate*)pData;

        //令牌参数
        tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
        if (pTokenParameter == NULL) return false;
        if (pTokenParameter->pBindUserItem == NULL) return false;

        //变量定义
		DBR_MB_GiftStatusUpdata GiftStatusUpdata;
        ZeroMemory(&GiftStatusUpdata, sizeof(GiftStatusUpdata));

        //设置变量
		GiftStatusUpdata.dwUserID = pTokenParameter->dwUserID;
		GiftStatusUpdata.dwRecordID = pGiftStatusUpdate->dwRecordID;

        //投递请求
        m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_GIFT_STATUS_UPDATE, GiftStatusUpdata.dwUserID, dwSocketID, dwTokenID, &GiftStatusUpdata, sizeof(GiftStatusUpdata));

        return true;
	}
	case SUB_MB_QUERY_TURNTABLE_LOTTERY://查询转盘抽奖数据
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_QueryTurntableLottery));
		if (wDataSize != sizeof(CMD_MB_QueryTurntableLottery)) return false;

		//提取数据
		auto pQueryTurntableLottery = (CMD_MB_QueryTurntableLottery*)pData;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//查询配置
		auto TurntableOptionPtr = g_GlobalOptionManager->GetOptionItem<CTurntableOptionPtr>(OPTION_MASK_TURNTABLE);
		if (!TurntableOptionPtr)
		{
			return false;
		}

		//校验摘要
		if (lstrcmp(pQueryTurntableLottery->szOptionDigest, TurntableOptionPtr->GetOptionDigest()) != 0)
		{
			//构造对象
			CMD_MB_TurntableOption TurntableOption;
			ZeroMemory(&TurntableOption,sizeof(TurntableOption));

			//设置变量
			StringCchCopy(TurntableOption.szOptionDigest, CountArray(TurntableOption.szOptionDigest) ,TurntableOptionPtr->GetOptionDigest());
			StringCchCopy(TurntableOption.szTurantableAward, CountArray(TurntableOption.szTurantableAward), TurntableOptionPtr->GetTurntableAwards());

			//发送配置		
			WORD wAwardSize = CountStringBuffer(TurntableOption.szTurantableAward);
			WORD wPacketSize = sizeof(TurntableOption) - sizeof(TurntableOption.szTurantableAward) + wAwardSize;
			SendDataToGate(dwSocketID, MDM_MB_USER_SERVICE, SUB_MB_TURNTABLE_OPTION, &TurntableOption, wPacketSize, &dwTokenID, 1);
		}

		//变量定义
		DBR_MB_QueryTurntableLottery QueryTurntableLottery;
		ZeroMemory(&QueryTurntableLottery, sizeof(QueryTurntableLottery));

		//设置变量
		QueryTurntableLottery.dwUserID = pTokenParameter->dwUserID;
		QueryTurntableLottery.dwClientIP = pTokenParameter->dwClientIP;
		QueryTurntableLottery.dwRecordID = pQueryTurntableLottery->dwRecordID;
		StringCchCopy(QueryTurntableLottery.szMachineID, CountArray(QueryTurntableLottery.szMachineID), pQueryTurntableLottery->szMachineID);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_QUERY_TURNTABLE_LOTTERY, QueryTurntableLottery.dwUserID, dwSocketID, dwTokenID, &QueryTurntableLottery, sizeof(QueryTurntableLottery));

		return true;
	}
	case SUB_MB_REQUEST_TURNTABLE_LOTTERY:	//请求转盘抽奖
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_RequestTurntableLottery));
		if (wDataSize != sizeof(CMD_MB_RequestTurntableLottery)) return false;

		//提取数据
		auto pRequestTurntableLottery = (CMD_MB_RequestTurntableLottery*)pData;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//查询配置
		auto TurntableOptionPtr = g_GlobalOptionManager->GetOptionItem<CTurntableOptionPtr>(OPTION_MASK_TURNTABLE);
		if (!TurntableOptionPtr)
		{
			return false;
		}

		//校验摘要
		if (lstrcmp(pRequestTurntableLottery->szOptionDigest, TurntableOptionPtr->GetOptionDigest()) != 0)
		{
			//构造对象
			CMD_MB_TurntableOption TurntableOption;
			ZeroMemory(&TurntableOption, sizeof(TurntableOption));

			//设置变量
			StringCchCopy(TurntableOption.szOptionDigest, CountArray(TurntableOption.szOptionDigest), TurntableOptionPtr->GetOptionDigest());
			StringCchCopy(TurntableOption.szTurantableAward, CountArray(TurntableOption.szTurantableAward), TurntableOptionPtr->GetTurntableAwards());

			//发送配置		
			WORD wAwardSize = CountStringBuffer(TurntableOption.szTurantableAward);
			WORD wPacketSize = sizeof(TurntableOption) - sizeof(TurntableOption.szTurantableAward);
			SendDataToGate(dwSocketID, MDM_MB_USER_SERVICE, SUB_MB_TURNTABLE_OPTION, &TurntableOption, wPacketSize, &dwTokenID, 1);
		}

		//变量定义
		DBR_MB_RequestTurntableLottery RequestTurntableLottery;
		ZeroMemory(&RequestTurntableLottery, sizeof(RequestTurntableLottery));

		//设置变量
		RequestTurntableLottery.dwUserID = pTokenParameter->dwUserID;
		RequestTurntableLottery.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(RequestTurntableLottery.szMachineID, CountArray(RequestTurntableLottery.szMachineID), pRequestTurntableLottery->szMachineID);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_REQUEST_TURNTABLE_LOTTERY, RequestTurntableLottery.dwUserID, dwSocketID, dwTokenID, &RequestTurntableLottery, sizeof(RequestTurntableLottery));

		return true;
	}
	case SUB_MB_QUERY_RANKING_LIST:	//查询榜单
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_QueryRankingList));
		if (wDataSize != sizeof(CMD_MB_QueryRankingList)) return false;

		//提取数据
		auto pQueryRankingList = (CMD_MB_QueryRankingList*)pData;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//榜单数据
		CMD_MB_RankingListData RankingListData;
		ZeroMemory(&RankingListData,sizeof(RankingListData));

		//榜单类型
		RankingListData.cbListType = pQueryRankingList->cbListType;

		//查询榜单
		auto pRankingListItem = m_RankingListManager.SearchRankingList(RankingListData.cbListType);
		if (pRankingListItem)
		{
			StringCchCopy(RankingListData.szRankingList,CountArray(RankingListData.szRankingList), pRankingListItem->GetRankingList());
		}

		//发送配置		
		WORD wListSize = CountStringBuffer(RankingListData.szRankingList);
		WORD wPacketSize = sizeof(RankingListData) - sizeof(RankingListData.szRankingList) + wListSize;
		SendDataToGate(dwSocketID, MDM_MB_USER_SERVICE, SUB_MB_RANKING_LIST_DATA, &RankingListData, wPacketSize, &dwTokenID, 1);

		return true;
	}
	case SUB_MB_WEALTH_TRANSFER:	//货币转账
	{
		//效验参数
		ASSERT (wDataSize == sizeof(CMD_MB_WealthTransfer));
		if (wDataSize != sizeof(CMD_MB_WealthTransfer)) return false;

		//处理消息
		auto pWealthTransfer = (CMD_MB_WealthTransfer*)pData;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_WealthTransfer WealthTransfer;
		ZeroMemory(&WealthTransfer, sizeof(WealthTransfer));

		//设置变量
		WealthTransfer.dwUserID = pTokenParameter->dwUserID;
		WealthTransfer.cbCodeKind = pWealthTransfer->cbCodeKind;
		StringCchCopy(WealthTransfer.szAuthCode,CountArray(WealthTransfer.szAuthCode), pWealthTransfer->szAuthCode);

		//转账信息
		WealthTransfer.dwTargetGameID = pWealthTransfer->dwTargetGameID;
		WealthTransfer.lTransferAmount = pWealthTransfer->lTransferAmount;

		//连接信息
		WealthTransfer.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(WealthTransfer.szMachineID,CountArray(WealthTransfer.szMachineID), pWealthTransfer->szMachineID);
	
		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_WEALTH_TRANSFER, WealthTransfer.dwUserID, dwSocketID, dwTokenID, &WealthTransfer, sizeof(WealthTransfer));

		return true;
	}
	case SUB_MB_DRAW_COMMISSION:	//取出佣金
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_DrawCommission));
		if (wDataSize != sizeof(CMD_MB_DrawCommission)) return false;

		//处理消息
		auto pDrawCommission = (CMD_MB_DrawCommission*)pData;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_DrawCommission DrawCommission;
		ZeroMemory(&DrawCommission, sizeof(DrawCommission));

		//设置变量
		DrawCommission.dwUserID = pTokenParameter->dwUserID;
		DrawCommission.lDrawAmount = pDrawCommission->lDarwinAmount;

		//连接信息
		DrawCommission.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(DrawCommission.szMachineID, CountArray(DrawCommission.szMachineID), pDrawCommission->szMachineID);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_DRAW_COMMISSION, DrawCommission.dwUserID, dwSocketID, dwTokenID, &DrawCommission, sizeof(DrawCommission));

		return true;
	}
	case SUB_MB_INSURE_QUERY:		//银行查询
	{
		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_InsureQuery InsureQuery;
		ZeroMemory(&InsureQuery, sizeof(InsureQuery));

		//设置变量
		InsureQuery.dwUserID = pTokenParameter->dwUserID;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_INSURE_QUERY, InsureQuery.dwUserID, dwSocketID, dwTokenID, &InsureQuery, sizeof(InsureQuery));
		return true;
	}
	case SUB_MB_INSURE_ENABLE:		//银行开通
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_InsureEnable));
		if (wDataSize != sizeof(CMD_MB_InsureEnable)) return false;

		//处理消息
		auto pInsureEnable = (CMD_MB_InsureEnable*)pData;
		pInsureEnable->szPassword[LEN_PASSWORD - 1] = 0;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_InsureEnable InsureEnable;
		ZeroMemory(&InsureEnable, sizeof(InsureEnable));

		//设置变量
		InsureEnable.dwUserID = pTokenParameter->dwUserID;
		StringCchCopy(InsureEnable.szPassword, CountArray(InsureEnable.szPassword), pInsureEnable->szPassword);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_INSURE_ENABLE, InsureEnable.dwUserID, dwSocketID, dwTokenID, &InsureEnable, sizeof(InsureEnable));
		return true;
	}
	case SUB_MB_INSURE_SAVE:		//银行存储
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_InsureSave));
		if (wDataSize != sizeof(CMD_MB_InsureSave)) return false;

		//处理消息
		auto pInsureSave = (CMD_MB_InsureSave*)pData;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_InsureSave InsureSave;
		ZeroMemory(&InsureSave, sizeof(InsureSave));

		//设置变量
		InsureSave.dwUserID = pTokenParameter->dwUserID;
		InsureSave.lSaveScore = pInsureSave->lSaveScore;

		//连接信息
		InsureSave.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(InsureSave.szMachineID, CountArray(InsureSave.szMachineID), pInsureSave->szMachineID);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_INSURE_SAVE, InsureSave.dwUserID, dwSocketID, dwTokenID, &InsureSave, sizeof(InsureSave));
		return true;
	}
	case SUB_MB_INSURE_TAKE:		//银行提取
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_InsureTake));
		if (wDataSize != sizeof(CMD_MB_InsureTake)) return false;

		//处理消息
		auto pInsureTake = (CMD_MB_InsureTake*)pData;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_InsureTake InsureTake;
		ZeroMemory(&InsureTake, sizeof(InsureTake));

		//设置变量
		InsureTake.dwUserID = pTokenParameter->dwUserID;
		InsureTake.lTakeScore = pInsureTake->lTakeScore;
		StringCchCopy(InsureTake.szPassword, CountArray(InsureTake.szPassword), pInsureTake->szPassword);

		//连接信息
		InsureTake.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(InsureTake.szMachineID, CountArray(InsureTake.szMachineID), pInsureTake->szMachineID);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_INSURE_TAKE, InsureTake.dwUserID, dwSocketID, dwTokenID, &InsureTake, sizeof(InsureTake));
		return true;
	}
	case SUB_MB_INSURE_TRANSFER:		//银行转账
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_InsureTransfer));
		if (wDataSize != sizeof(CMD_MB_InsureTransfer)) return false;

		//处理消息
		auto pInsureTransfer = (CMD_MB_InsureTransfer*)pData;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_InsureTransfer InsureTransfer;
		ZeroMemory(&InsureTransfer, sizeof(InsureTransfer));

		//设置变量
		InsureTransfer.dwUserID = pTokenParameter->dwUserID;
		InsureTransfer.lTransferScore = pInsureTransfer->lTransferScore;
		InsureTransfer.dwGameID = pInsureTransfer->dwGameID;
		StringCchCopy(InsureTransfer.szPassword, CountArray(InsureTransfer.szPassword), pInsureTransfer->szPassword);

		//连接信息
		InsureTransfer.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(InsureTransfer.szMachineID, CountArray(InsureTransfer.szMachineID), pInsureTransfer->szMachineID);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_INSURE_TRANSFER, InsureTransfer.dwUserID, dwSocketID, dwTokenID, &InsureTransfer, sizeof(InsureTransfer));
		return true;
	}
	case SUB_MB_INSURE_RESET_PASSWORD:		//银行密码重置
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_InsureResetPassword));
		if (wDataSize != sizeof(CMD_MB_InsureResetPassword)) return false;

		//处理消息
		auto pInsureReset = (CMD_MB_InsureResetPassword*)pData;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_InsureResetPassword InsureReset;
		ZeroMemory(&InsureReset, sizeof(InsureReset));

		//设置变量
		InsureReset.dwUserID = pTokenParameter->dwUserID;
		StringCchCopy(InsureReset.szSrcPassword, CountArray(InsureReset.szSrcPassword), pInsureReset->szSrcPassword);
		StringCchCopy(InsureReset.szDstPassword, CountArray(InsureReset.szDstPassword), pInsureReset->szDstPassword);

		//连接信息
		InsureReset.dwClientIP = pTokenParameter->dwClientIP;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_INSURE_RESET_PASSWORD, InsureReset.dwUserID, dwSocketID, dwTokenID, &InsureReset, sizeof(InsureReset));
		return true;
	}
	case SUB_MB_REAL_AUTH:		//实名认证
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_RealAuth));
		if (wDataSize != sizeof(CMD_MB_RealAuth)) return false;

		//处理消息
		CMD_MB_RealAuth* pRealAuth = (CMD_MB_RealAuth*)pData;
		pRealAuth->szPassword[CountArray(pRealAuth->szPassword) - 1] = 0;
		pRealAuth->szCompellation[CountArray(pRealAuth->szCompellation) - 1] = 0;
		pRealAuth->szPassPortID[CountArray(pRealAuth->szPassPortID) - 1] = 0;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_RealAuth RealAuthent;

		//构造数据
		RealAuthent.dwUserID = pTokenParameter->dwUserID;
		StringCchCopy(RealAuthent.szPassword, CountArray(RealAuthent.szPassword), pRealAuth->szPassword);
		StringCchCopy(RealAuthent.szCompellation, CountArray(RealAuthent.szCompellation), pRealAuth->szCompellation);
		StringCchCopy(RealAuthent.szPassPortID, CountArray(RealAuthent.szPassPortID), pRealAuth->szPassPortID);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_REAL_AUTH, RealAuthent.dwUserID, dwSocketID, dwTokenID, &RealAuthent, sizeof(RealAuthent));
		return true;
	}
	case SUB_MB_QUERY_SUBSIDY:	//查询低保
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_MB_SubsidyQuery));
		if (wDataSize != sizeof(CMD_MB_SubsidyQuery)) return false;

		//处理消息
		auto pSubsidy = (CMD_MB_SubsidyQuery*)pData;

		//令牌参数
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//变量定义
		DBR_MB_SubsidyQuery Subsidy;
		ZeroMemory(&Subsidy, sizeof(Subsidy));

		//设置变量
		Subsidy.dwUserID = pTokenParameter->dwUserID;
		Subsidy.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(Subsidy.szMachineID, CountArray(Subsidy.szMachineID), pSubsidy->szMachineID);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_QUERY_SUBSIDY, Subsidy.dwUserID, dwSocketID, dwTokenID, &Subsidy, sizeof(Subsidy));

		return true;
	}
	}

	return false;
}

//管理服务
bool CAttemperEngineSink::OnTCPNetworkMainPCManagerService(WORD wSubCmdID, VOID *pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_MODIFY_OPTION:			//修改选项
	{
		auto pModifyOption = (CMD_GP_ModifyOption*)pData;

		//构造结构
		CMD_CS_C_ModifyOption ModifyOption = {};
		
		//设置变量
		ModifyOption.ModifyOption = pModifyOption->ModifyOption;
		ModifyOption.wServerCount = pModifyOption->wServerCount;
		CopyMemory(ModifyOption.wServerIDList, pModifyOption->wServerIDList, ModifyOption.wServerCount*sizeof(ModifyOption.wServerIDList[0]));

		//发送数据
		WORD wSendSize = sizeof(ModifyOption) - sizeof(ModifyOption.wServerIDList);
		wSendSize += ModifyOption.wServerCount * sizeof(ModifyOption.wServerIDList[0]);
		m_pITCPSocketCenter->SendData(MDM_CS_MANAGER_SERVICE, SUB_CS_C_MODIFY_OPTION, pData, wDataSize, dwTokenID);
		break;
	}
	case SUB_GP_EXECUTE_COMMAND:		//执行命令
	{
		//发送数据
		m_pITCPSocketCenter->SendData(MDM_CS_MANAGER_SERVICE, SUB_CS_C_EXECUTE_COMMAND, pData, wDataSize, dwTokenID);
		break;
	}
	}		

	//操作成功
	SendDataToGate(dwSocketID, MDM_GP_MANAGER_SERVICE, SUB_GP_MANAGE_SUCCESS,&dwTokenID,1);

	return true;
}

//配置服务
bool CAttemperEngineSink::OnTCPNetworkMainPCConfigService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_QUERY_GAME_CONFIG:		//查询配置
	{
		//参数校验
		ASSERT(wDataSize == sizeof(CMD_GP_QueryGameConfig));
		if (wDataSize != sizeof(CMD_GP_QueryGameConfig)) return false;

		//提取数据
		auto pQueryGameConfig = (CMD_GP_QueryGameConfig*)pData;
		ASSERT(pQueryGameConfig != NULL);

		//构造结构
		DBR_GP_QueryGameConfig QueryGameConfig = {};
		QueryGameConfig.wKindID = pQueryGameConfig->wKindID;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_GAME_CONFIG, 0, dwSocketID, dwTokenID, &QueryGameConfig, sizeof(QueryGameConfig));

		return true;
	}
	case SUB_GP_UPDATE_BASIC_CONFIG:		//更新配置
	{
		//参数校验
		ASSERT (wDataSize <= sizeof(CMD_GP_UpdateBasicConfig));
		if (wDataSize > sizeof(CMD_GP_UpdateBasicConfig)) return false;

		//提取数据
		auto pUpdateBasicConfig = (CMD_GP_UpdateBasicConfig*)pData;
		ASSERT(pUpdateBasicConfig != NULL);

		//构造结构
		DBR_GP_UpdateBasicConfig UpdateBasicConfig = {};
		UpdateBasicConfig.wKindID = pUpdateBasicConfig->wKindID;
		StringCchCopy(UpdateBasicConfig.szBasicConfig,CountArray(UpdateBasicConfig.szBasicConfig), pUpdateBasicConfig->szBasicConfig);
		
		//投递请求
		WORD wSendSize = CountStringBuffer(UpdateBasicConfig.szBasicConfig);
		wSendSize += sizeof(UpdateBasicConfig) - sizeof(UpdateBasicConfig.szBasicConfig);
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_BASIC_CONFIG, 0, dwSocketID, dwTokenID, &UpdateBasicConfig, wSendSize);

		return true;
	}
	case SUB_GP_UPDATE_WEIGHT_CONFIG:		//更新配置
	{
		//参数校验
		ASSERT(wDataSize <= sizeof(CMD_GP_UpdateWeightConfig));
		if (wDataSize > sizeof(CMD_GP_UpdateWeightConfig)) return false;

		//提取数据
		auto pUpdateWeightConfig = (CMD_GP_UpdateWeightConfig*)pData;
		ASSERT(pUpdateWeightConfig != NULL);

		//构造结构
		DBR_GP_UpdateWeightConfig UpdateWeightConfig;
		UpdateWeightConfig.wKindID = pUpdateWeightConfig->wKindID;
		UpdateWeightConfig.cbTypeID = pUpdateWeightConfig->cbTypeID;
		UpdateWeightConfig.wItemCount = pUpdateWeightConfig->wItemCount;
		CopyMemory(&UpdateWeightConfig.TimesWeightItem, &pUpdateWeightConfig->TimesWeightItem, sizeof(UpdateWeightConfig.TimesWeightItem));

		//投递请求
		WORD wSendSize = sizeof(UpdateWeightConfig) - sizeof(UpdateWeightConfig.TimesWeightItem);
		wSendSize += UpdateWeightConfig.wItemCount * sizeof(UpdateWeightConfig.TimesWeightItem[0]);
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_WEIGHT_CONFIG, 0, dwSocketID, dwTokenID, &UpdateWeightConfig, wSendSize);

		return true;
	}	
	}

	return false;
}

//机器服务
bool CAttemperEngineSink::OnTCPNetworkMainPCRobotService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_QUERY_PARAMETER:		//获取参数
		{
			//参数校验
			ASSERT(wDataSize==sizeof(CMD_GP_QueryParameter));
			if(wDataSize!=sizeof(CMD_GP_QueryParameter)) return false;

			//提取数据
			CMD_GP_QueryParameter * pQueryParameter = (CMD_GP_QueryParameter *)pData;
			ASSERT(pQueryParameter!=NULL);

			//构造结构
			DBR_GP_QueryParameter QueryParameter;
			QueryParameter.wServerID = pQueryParameter->wServerID;

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_PARAMETER,0,dwSocketID,dwTokenID,&QueryParameter,sizeof(QueryParameter));

			return true;
		}
	case SUB_GP_APPEND_PARAMETER:		//添加参数
		{
			//参数校验
			ASSERT(wDataSize==sizeof(CMD_GP_AppendParameter));
			if(wDataSize!=sizeof(CMD_GP_AppendParameter)) return false;

			//提取数据
			CMD_GP_AppendParameter * pAppendParameter = (CMD_GP_AppendParameter *)pData;
			ASSERT(pAppendParameter!=NULL);

			//构造结构
			DBR_GP_AppendParameter AppendParameter;
			AppendParameter.wServerID = pAppendParameter->wServerID;
			CopyMemory(&AppendParameter.RobotParameter,&pAppendParameter->RobotParameter,sizeof(tagRobotParameter));

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_APPEND_PARAMETER,0,dwSocketID,dwTokenID,&AppendParameter,sizeof(AppendParameter));

			return true;
		}
	case SUB_GP_MODIFY_PARAMETER:	//修改参数
		{
			//参数校验
			ASSERT(wDataSize==sizeof(CMD_GP_ModifyParameter));
			if(wDataSize!=sizeof(CMD_GP_ModifyParameter)) return false;

			//提取数据
			CMD_GP_ModifyParameter * pModifyParameter = (CMD_GP_ModifyParameter *)pData;
			ASSERT(pModifyParameter!=NULL);

			//构造结构
			DBR_GP_ModifyParameter ModifyParameter;
			ModifyParameter.wServerID=pModifyParameter->wServerID;
			CopyMemory(&ModifyParameter.RobotParameter,&pModifyParameter->RobotParameter,sizeof(tagRobotParameter));

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_PARAMETER,0,dwSocketID,dwTokenID,&ModifyParameter,sizeof(ModifyParameter));
			
			return true;
		}
	case SUB_GP_DELETE_PARAMETER:	//删除参数
		{
			//参数校验
			ASSERT(wDataSize==sizeof(CMD_GP_DeleteParameter));
			if(wDataSize!=sizeof(CMD_GP_DeleteParameter)) return false;

			//提取数据
			CMD_GP_DeleteParameter * pDeleteParameter = (CMD_GP_DeleteParameter *)pData;
			ASSERT(pDeleteParameter!=NULL);

			//构造结构
			DBR_GP_DeleteParameter DeleteParameter;
			DeleteParameter.wServerID = pDeleteParameter->wServerID;
			DeleteParameter.dwBatchID = pDeleteParameter->dwBatchID;

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_DELETE_PARAMETER,0,dwSocketID,dwTokenID,&DeleteParameter,sizeof(DeleteParameter));
			
			return true;
		}
	}

	return false;
}

//库存服务
bool CAttemperEngineSink::OnTCPNetworkMainPCStockService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_QUERY_STOCK_INFO:		//查询库存
	{
		//参数校验
		ASSERT(wDataSize == sizeof(CMD_GP_QueryStockInfo));
		if (wDataSize != sizeof(CMD_GP_QueryStockInfo)) return false;

		//提取数据
		auto pQueryStockInfo = (CMD_GP_QueryStockInfo*)pData;
		
		//构造结构
		DBR_GP_QueryStockInfo QueryStockInfo = {};
		StringCchCopy(QueryStockInfo.szKindID,CountArray(QueryStockInfo.szKindID), pQueryStockInfo->szKindID);
		
		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_STOCK_INFO, 0, dwSocketID, dwTokenID, &QueryStockInfo, sizeof(QueryStockInfo));

		return true;
	}
	case SUB_GP_APPEND_STOCK_INFO:		//添加库存
	{
		//参数校验
		ASSERT(wDataSize == sizeof(CMD_GP_AppendStockInfo));
		if (wDataSize != sizeof(CMD_GP_AppendStockInfo)) return false;

		//提取数据
		auto pAppendStockInfo = (CMD_GP_AppendStockInfo*)pData;

		//构造结构
		DBR_GP_AppendStockInfo AppendStockInfo = {};
		CopyMemory(&AppendStockInfo.StockInfo, &pAppendStockInfo->StockInfo, sizeof(AppendStockInfo.StockInfo));

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_APPEND_STOCK_INFO, 0, dwSocketID, dwTokenID, &AppendStockInfo, sizeof(AppendStockInfo));

		return true;
	}
	case SUB_GP_MODIFY_STOCK_INFO:		//修改库存
	{
		//参数校验
		ASSERT(wDataSize == sizeof(CMD_GP_ModifyStockInfo));
		if (wDataSize != sizeof(CMD_GP_ModifyStockInfo)) return false;

		//提取数据
		auto pModifyStockInfo = (CMD_GP_ModifyStockInfo*)pData;

		//构造结构
		DBR_GP_ModifyStockInfo ModifyStockInfo = {};
		CopyMemory(&ModifyStockInfo.StockInfo, &pModifyStockInfo->StockInfo, sizeof(ModifyStockInfo.StockInfo));

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_STOCK_INFO, 0, dwSocketID, dwTokenID, &ModifyStockInfo, sizeof(ModifyStockInfo));

		return true;
	}
	case SUB_GP_DELETE_STOCK_INFO:		//删除库存
	{
		//参数校验
		ASSERT(wDataSize == sizeof(CMD_GP_DeleteStockInfo));
		if (wDataSize != sizeof(CMD_GP_DeleteStockInfo)) return false;

		//提取数据
		auto pDeleteStockInfo = (CMD_GP_DeleteStockInfo*)pData;

		//构造结构
		DBR_GP_DeleteStockInfo DeleteStockInfo = {};
		DeleteStockInfo.wStockID = pDeleteStockInfo->wStockID;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_DELETE_STOCK_INFO, 0, dwSocketID, dwTokenID, &DeleteStockInfo, sizeof(DeleteStockInfo));

		return true;
	}
	case SUB_GP_ADJUST_STOCK_SCORE:		//调整库存
	{
		//参数校验
		ASSERT(wDataSize == sizeof(CMD_GP_AdjustStockScore));
		if (wDataSize != sizeof(CMD_GP_AdjustStockScore)) return false;

		//提取数据
		auto pAdjustStockScore = (CMD_GP_AdjustStockScore*)pData;

		//缓冲定义
		WORD wBufferSize = 0;
		BYTE cbBuffer[512] = { 0 };

		//构造结构
		tagForwardHead* pForwadHead = (tagForwardHead*)cbBuffer;

		//转发设置
		pForwadHead->wItemCount = 1;
		pForwadHead->wTarget = FORWARD_TARGET_SERVICE;
		pForwadHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MONITOR;

		//构造结构
		auto pFWAdjustStockScore = (CMD_CS_AdjustStockScore*)(pForwadHead + 1);

		//设置变量
		pFWAdjustStockScore->wStockID = pAdjustStockScore->wStockID;
		pFWAdjustStockScore->lChangeScore = pAdjustStockScore->lChangeScore;

		//发送数据
		wBufferSize = sizeof(tagForwardHead) + sizeof(CMD_CS_AdjustStockScore);
		m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_ADJUST_STOCK_SCORE, cbBuffer, wBufferSize, dwTokenID);		

		//变量定义
		CMD_GP_StockOperateResult StockOperateResult;
		ZeroMemory(&StockOperateResult, sizeof(StockOperateResult));

		//构造变量
		StockOperateResult.wSubCommdID = SUB_GP_ADJUST_STOCK_SCORE;		

		//计算大小
		WORD wPacketSize = sizeof(StockOperateResult) - sizeof(StockOperateResult.StockList);
		wPacketSize += sizeof(StockOperateResult.StockList[0]) * StockOperateResult.wStockCount;

		//发送数据
		SendDataToGate(dwSocketID, MDM_GP_STOCK_SERVICE, SUB_GP_STOCK_OPERATE_RESULT, &StockOperateResult, wPacketSize, &dwTokenID, 1);

		return true;
	}
	case SUB_GP_ADJUST_JACKPOT_SCORE:	//调整彩金
	{
		//参数校验
		ASSERT(wDataSize == sizeof(CMD_GP_AdjustJackpotScore));
		if (wDataSize != sizeof(CMD_GP_AdjustJackpotScore)) return false;

		//提取数据
		auto pAdjustJackpotScore = (CMD_GP_AdjustJackpotScore*)pData;

		//缓冲定义
		WORD wBufferSize = 0;
		BYTE cbBuffer[512] = { 0 };

		//构造结构
		tagForwardHead* pForwadHead = (tagForwardHead*)cbBuffer;

		//转发设置
		pForwadHead->wItemCount = 1;
		pForwadHead->wTarget = FORWARD_TARGET_SERVICE;
		pForwadHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MONITOR;

		//构造结构
		auto pFWAdjustJackpotScore = (CMD_CS_AdjustJackpotScore*)(pForwadHead + 1);

		//设置变量
		pFWAdjustJackpotScore->wStockID = pAdjustJackpotScore->wStockID;
		pFWAdjustJackpotScore->wLevelID = pAdjustJackpotScore->wLevelID;
		pFWAdjustJackpotScore->lChangeScore = pAdjustJackpotScore->lChangeScore;

		//发送数据
		wBufferSize = sizeof(tagForwardHead) + sizeof(CMD_CS_AdjustJackpotScore);
		m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_ADJUST_JACKPOT_SCORE, cbBuffer, wBufferSize, dwTokenID);

		//变量定义
		CMD_GP_StockOperateResult StockOperateResult;
		ZeroMemory(&StockOperateResult, sizeof(StockOperateResult));

		//构造变量
		StockOperateResult.wSubCommdID = SUB_GP_ADJUST_JACKPOT_SCORE;

		//计算大小
		WORD wPacketSize = sizeof(StockOperateResult) - sizeof(StockOperateResult.StockList);
		wPacketSize += sizeof(StockOperateResult.StockList[0]) * StockOperateResult.wStockCount;

		//发送数据
		SendDataToGate(dwSocketID, MDM_GP_STOCK_SERVICE, SUB_GP_STOCK_OPERATE_RESULT, &StockOperateResult, wPacketSize, &dwTokenID, 1);

		return true;
	}
	}

	return false;
}

//游客登录
bool CAttemperEngineSink::OnTCPNetworkSubPCLogonByGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID,DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GP_LogonByGameID));
	if (wDataSize<sizeof(CMD_GP_LogonByGameID)) return false;

	//处理消息
	CMD_GP_LogonByGameID * pLogonByGameID=(CMD_GP_LogonByGameID *)pData;
	pLogonByGameID->szPassword[CountArray(pLogonByGameID->szPassword)-1]=0;

	//令牌参数
	tagTokenParameter * pTokenParameter=GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//设置连接
	pExtendParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pExtendParameter->dwPlazaVersion=pLogonByGameID->dwPlazaVersion;

	//效验版本
	if (CheckPlazaVersion(DEVICE_KIND_PC,pLogonByGameID->dwPlazaVersion,dwSocketID,dwTokenID)==false) return true;

	//变量定义
	DBR_GP_LogonByGameID LogonByGameID;
	ZeroMemory(&LogonByGameID,sizeof(LogonByGameID));

	//附加信息
	LogonByGameID.dwPostTime=GetTickCount();
	LogonByGameID.pTokenParameter= pTokenParameter;

	//构造数据
	LogonByGameID.dwGameID=pLogonByGameID->dwGameID;
	LogonByGameID.dwClientIP=pTokenParameter->dwClientIP;
	StringCchCopy(LogonByGameID.szPassword, CountArray(LogonByGameID.szPassword), pLogonByGameID->szPassword);

	//变量定义
	VOID * pDataBuffer=NULL;
	tagDataDescribe DataDescribe;
	CRecvPacketHelper RecvPacket(pLogonByGameID+1,wDataSize-sizeof(CMD_GP_LogonByGameID));

	//扩展信息
	while (true)
	{
		//提取数据
		pDataBuffer=RecvPacket.GetData(DataDescribe);
		if (DataDescribe.wDataDescribe==DTP_NULL) break;

		//数据分析
		switch (DataDescribe.wDataDescribe)
		{
		case DTP_GP_MACHINE_ID:	//机器标识
			{
				ASSERT(pDataBuffer!=NULL);
				ASSERT(DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR));
				if (DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR))
				{
					StringCchCopy( LogonByGameID.szMachineID,CountArray(LogonByGameID.szMachineID), (WCHAR*)pDataBuffer);
				}

				break;
			}
		}
	}

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_GAMEID, LogonByGameID.dwGameID,dwSocketID,dwTokenID,&LogonByGameID,sizeof(LogonByGameID));

	return true;
}

//帐号登录
bool CAttemperEngineSink::OnTCPNetworkSubPCLogonByAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID,DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GP_LogonByAccounts));
	if (wDataSize<sizeof(CMD_GP_LogonByAccounts)) return false;

	//处理消息
	CMD_GP_LogonByAccounts * pLogonByAccounts=(CMD_GP_LogonByAccounts *)pData;
	pLogonByAccounts->szAccounts[CountArray(pLogonByAccounts->szAccounts)-1]=0;
	pLogonByAccounts->szPassword[CountArray(pLogonByAccounts->szPassword)-1]=0;

	//设置绑定
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//获取参数
	tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//设置连接
	pExtendParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pExtendParameter->dwPlazaVersion=pLogonByAccounts->dwPlazaVersion;
	
	//版本判断
	if (CheckPlazaVersion(DEVICE_KIND_PC,pLogonByAccounts->dwPlazaVersion,dwSocketID,dwTokenID)==false) return true;

	//变量定义
	DBR_GP_LogonByAccounts LogonByAccounts;
	ZeroMemory(&LogonByAccounts,sizeof(LogonByAccounts));

	//附加信息
	LogonByAccounts.dwPostTime=GetTickCount();
	LogonByAccounts.pTokenParameter = pTokenParameter;
	LogonByAccounts.wGateID = pBindParameter->wServiceID;
	LogonByAccounts.wLogonID = m_pServiceOption->wServiceID;

	//构造数据
	LogonByAccounts.dwCheckUserRight=0;
	LogonByAccounts.szNewComputerID[0]=0;			
	LogonByAccounts.dwLogonServerID = m_pServiceOption->wServiceID;
	LogonByAccounts.dwPlazaVersion=pLogonByAccounts->dwPlazaVersion;	
	LogonByAccounts.dwClientIP= pTokenParameter->dwClientIP;
	StringCchCopy(LogonByAccounts.szAccounts,CountArray(LogonByAccounts.szAccounts), pLogonByAccounts->szAccounts);
	StringCchCopy(LogonByAccounts.szPassword,CountArray(LogonByAccounts.szPassword), pLogonByAccounts->szPassword);

	//扩展信息
	VOID * pDataBuffer=NULL;
	tagDataDescribe DataDescribe;
	CRecvPacketHelper RecvPacket(pLogonByAccounts+1,wDataSize-sizeof(CMD_GP_LogonByAccounts));
	while (true)
	{
		//提取数据
		pDataBuffer=RecvPacket.GetData(DataDescribe);
		if (DataDescribe.wDataDescribe==DTP_NULL) break;

		//数据分析
		switch (DataDescribe.wDataDescribe)
		{
		case DTP_GP_MACHINE_ID:	//机器标识
			{
				ASSERT(pDataBuffer!=NULL);
				ASSERT(DataDescribe.wDataSize==LEN_MACHINE_ID*sizeof(WCHAR));
				if (DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR))
				{
					StringCchCopy( LogonByAccounts.szMachineID, CountArray(LogonByAccounts.szMachineID), (WCHAR*)pDataBuffer);
				}

				break;
			}
		}
	}

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_ACCOUNTS,0,dwSocketID,dwTokenID,&LogonByAccounts,sizeof(LogonByAccounts));

	return true;
}

//工具登录
bool CAttemperEngineSink::OnTCPNetworkSubPCLogonManageTool(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GP_LogonByAccounts));
	if (wDataSize<sizeof(CMD_GP_LogonByAccounts)) return false;

	//处理消息
	CMD_GP_LogonByAccounts * pLogonByAccounts=(CMD_GP_LogonByAccounts *)pData;
	pLogonByAccounts->szAccounts[CountArray(pLogonByAccounts->szAccounts)-1]=0;
	pLogonByAccounts->szPassword[CountArray(pLogonByAccounts->szPassword)-1]=0;

	//设置绑定
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//获取参数
	tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//设置连接
	pExtendParameter->cbClientKind = CLIENT_KIND_COMPUTER;
	pExtendParameter->dwPlazaVersion = pLogonByAccounts->dwPlazaVersion;

	//版本判断
	if (CheckPlazaVersion(DEVICE_KIND_PC,pLogonByAccounts->dwPlazaVersion,dwSocketID,dwTokenID)==false) return true;

	//变量定义
	DBR_GP_LogonByAccounts LogonByAccounts;
	ZeroMemory(&LogonByAccounts,sizeof(LogonByAccounts));

	//附加信息
	LogonByAccounts.dwPostTime=GetTickCount();
	LogonByAccounts.pTokenParameter = pTokenParameter;
	LogonByAccounts.wGateID = pBindParameter->wServiceID;
	LogonByAccounts.wLogonID = m_pServiceOption->wServiceID;

	//构造数据
	LogonByAccounts.szNewComputerID[0]=0;	
	LogonByAccounts.dwLogonServerID = m_pServiceOption->wServiceID;
	LogonByAccounts.dwPlazaVersion=pLogonByAccounts->dwPlazaVersion;
	LogonByAccounts.dwCheckUserRight=UR_CAN_MANAGER_ANDROID;
	LogonByAccounts.dwClientIP= pTokenParameter->dwClientIP;
	StringCchCopy(LogonByAccounts.szAccounts, CountArray(LogonByAccounts.szAccounts), pLogonByAccounts->szAccounts);
	StringCchCopy(LogonByAccounts.szPassword, CountArray(LogonByAccounts.szPassword), pLogonByAccounts->szPassword);

	//扩展信息
	VOID * pDataBuffer=NULL;
	tagDataDescribe DataDescribe;
	CRecvPacketHelper RecvPacket(pLogonByAccounts+1,wDataSize-sizeof(CMD_GP_LogonByAccounts));
	while (true)
	{
		//提取数据
		pDataBuffer=RecvPacket.GetData(DataDescribe);
		if (DataDescribe.wDataDescribe==DTP_NULL) break;

		//数据分析
		switch (DataDescribe.wDataDescribe)
		{
		case DTP_GP_MACHINE_ID:	//机器标识
			{
				ASSERT(pDataBuffer!=NULL);
				ASSERT(DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR));
				if (DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR))
				{
					StringCchCopy( LogonByAccounts.szMachineID,CountArray(LogonByAccounts.szMachineID), (WCHAR*)pDataBuffer);
				}

				break;
			}
		}
	}

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_ACCOUNTS,0,dwSocketID,dwTokenID,&LogonByAccounts,sizeof(LogonByAccounts));

	return true;
}

//帐号注册
bool CAttemperEngineSink::OnTCPNetworkSubPCRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GP_RegisterAccounts));
	if (wDataSize<sizeof(CMD_GP_RegisterAccounts)) return false;

	//处理消息
	CMD_GP_RegisterAccounts * pRegisterAccounts=(CMD_GP_RegisterAccounts *)pData;
	pRegisterAccounts->szAccounts[CountArray(pRegisterAccounts->szAccounts)-1]=0;
	pRegisterAccounts->szPassword[CountArray(pRegisterAccounts->szPassword)-1]=0;
	pRegisterAccounts->szBankPass[CountArray(pRegisterAccounts->szBankPass)-1]=0;

	//获取参数
	tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//设置连接
	pExtendParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pExtendParameter->dwPlazaVersion=pRegisterAccounts->dwPlazaVersion;

	//效验版本
	if (CheckPlazaVersion(DEVICE_KIND_PC,pRegisterAccounts->dwPlazaVersion,dwSocketID,dwTokenID)==false) return true;

	//变量定义
	DBR_GP_RegisterAccounts RegisterAccounts;
	ZeroMemory(&RegisterAccounts,sizeof(RegisterAccounts));

	//附加信息
	RegisterAccounts.dwPostTime=GetTickCount();
	RegisterAccounts.pTokenParameter = pTokenParameter;

	//构造数据
	RegisterAccounts.wFaceID=pRegisterAccounts->wFaceID;
	RegisterAccounts.dwClientIP = pTokenParameter->dwClientIP;
	RegisterAccounts.dwPlazaVersion=pRegisterAccounts->dwPlazaVersion;
	StringCchCopy(RegisterAccounts.szAccounts,CountArray(RegisterAccounts.szAccounts), pRegisterAccounts->szAccounts);
	StringCchCopy(RegisterAccounts.szNickName,CountArray(RegisterAccounts.szAccounts), pRegisterAccounts->szAccounts);
	StringCchCopy(RegisterAccounts.szPassword,CountArray(RegisterAccounts.szPassword), pRegisterAccounts->szPassword);
	StringCchCopy(RegisterAccounts.szBankPass,CountArray(RegisterAccounts.szBankPass), pRegisterAccounts->szBankPass);
	RegisterAccounts.dwLogonServerID=m_pServiceOption->wServiceID;
	RegisterAccounts.szValidComputerID[0]=0;

	//扩展信息
	VOID * pDataBuffer=NULL;
	tagDataDescribe DataDescribe;
	CRecvPacketHelper RecvPacket(pRegisterAccounts+1,wDataSize-sizeof(CMD_GP_RegisterAccounts));
	while (true)
	{
		//提取数据
		pDataBuffer=RecvPacket.GetData(DataDescribe);
		if (DataDescribe.wDataDescribe==DTP_NULL) break;

		//数据分析
		switch (DataDescribe.wDataDescribe)
		{
		case DTP_GP_USER_INFO:		//用户信息
			{
				ASSERT(DataDescribe.wDataSize==sizeof(DTP_GP_UserInfo));
				if (DataDescribe.wDataSize==sizeof(DTP_GP_UserInfo))
				{
					//变量定义
					DTP_GP_UserInfo * pUserInfo=(DTP_GP_UserInfo *)pDataBuffer;
					pUserInfo->szArea[CountArray(pUserInfo->szArea)-1]=0;
					pUserInfo->szCity[CountArray(pUserInfo->szCity)-1]=0;
					pUserInfo->szProvince[CountArray(pUserInfo->szProvince)-1]=0;

					//保存数据
					RegisterAccounts.cbAge=pUserInfo->cbAge;
					RegisterAccounts.cbGender=pUserInfo->cbGender;
					StringCchCopy(RegisterAccounts.szArea,CountArray(RegisterAccounts.szArea), pUserInfo->szArea);
					StringCchCopy(RegisterAccounts.szCity,CountArray(RegisterAccounts.szCity), pUserInfo->szCity);
					StringCchCopy(RegisterAccounts.szProvince,CountArray(RegisterAccounts.szProvince), pUserInfo->szProvince);
				}
				break;
			}
		case DTP_GP_MACHINE_ID:	//机器标识
			{
				ASSERT(pDataBuffer!=NULL);
				ASSERT(DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR));
				if (DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR))
				{
					StringCchCopy( RegisterAccounts.szMachineID,CountArray(RegisterAccounts.szMachineID), (WCHAR*)pDataBuffer);
				}

				break;
			}
		case DTP_GP_PASSPORT:		//身份信息
			{
				ASSERT(pDataBuffer);

				//变量定义
				DTP_GP_Passport *pPassport = (DTP_GP_Passport *)pDataBuffer;

				ASSERT(DataDescribe.wDataSize == sizeof(DTP_GP_Passport)-sizeof(pPassport->szPassportID)+
					CountStringBuffer(pPassport->szPassportID));
				if( DataDescribe.wDataSize == sizeof(DTP_GP_Passport)-sizeof(pPassport->szPassportID)+
					CountStringBuffer(pPassport->szPassportID) )
				{
					RegisterAccounts.cbPassportType = pPassport->cbPassportType;
					StringCchCopy( RegisterAccounts.szPassportID,CountArray(RegisterAccounts.szPassportID), pPassport->szPassportID);
				}

				break;
			}
		case DTP_GP_SPREADER:		//推荐玩家
			{
				ASSERT(pDataBuffer);

				StringCchCopy( RegisterAccounts.szSpreader,CountArray(RegisterAccounts.szSpreader), (WCHAR*)pDataBuffer);

				break;
			}
		case DTP_GP_NICKNAME:		//用户昵称
			{
				ASSERT(pDataBuffer);

				StringCchCopy(RegisterAccounts.szNickName,CountArray(RegisterAccounts.szNickName), (WCHAR*)pDataBuffer);
				break;
			}
		case DTP_GP_VERIFY_COMPUTER_ID:	//校验机器
			{
				ASSERT(pDataBuffer!=NULL);
				ASSERT(DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR));
				if (DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR))
				{
					StringCchCopy(RegisterAccounts.szValidComputerID,CountArray(RegisterAccounts.szValidComputerID), (WCHAR*)pDataBuffer);
				}
			}
		}
	}

	//机器补全
	if(RegisterAccounts.szValidComputerID[0]==0)
	{
		StringCchCopy(RegisterAccounts.szValidComputerID,CountArray(RegisterAccounts.szValidComputerID), (WCHAR*)RegisterAccounts.szMachineID);
	}

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_REGISTER_ACCOUNTS,0,dwSocketID,dwTokenID,&RegisterAccounts,sizeof(RegisterAccounts));

	return true;
}

//游客登录
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonVisitor(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT (wDataSize>=sizeof(CMD_MB_LogonVisitor));
	if (wDataSize<sizeof(CMD_MB_LogonVisitor)) return false;

	//设置绑定
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//获取参数
	tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//处理消息
	CMD_MB_LogonVisitor* pLogonVisitor =(CMD_MB_LogonVisitor*)pData;
	pLogonVisitor->szMachineID[CountArray(pLogonVisitor->szMachineID)-1]=0;
	pLogonVisitor->szMachineName[CountArray(pLogonVisitor->szMachineName)-1]=0;	
	
	//设置连接
	pExtendParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID= pLogonVisitor->wModuleID;
	pExtendParameter->dwDeviceKind= pLogonVisitor->dwDeviceKind;
	pExtendParameter->dwAppVersion= pLogonVisitor->dwAppVersion;
	pExtendParameter->dwPlazaVersion= pLogonVisitor->dwPlazaVersion;

	//效验版本
	if (CheckPlazaVersion(pLogonVisitor->dwDeviceKind&DEVICE_MASK_MB, pLogonVisitor->dwPlazaVersion,dwSocketID,dwTokenID)==false) return true;

	//变量定义
	DBR_MB_LogonVisitor LogonVisitor;
	ZeroMemory(&LogonVisitor,sizeof(LogonVisitor));

	//连接信息
	LogonVisitor.pTokenParameter = pTokenParameter;
	LogonVisitor.wGateID = pBindParameter->wServiceID;
	LogonVisitor.wLogonID = m_pServiceOption->wServiceID;	

	//构造数据	
	LogonVisitor.wMarketID= pLogonVisitor->wMarketID;
	LogonVisitor.dwClientIP=pTokenParameter->dwClientIP;
	LogonVisitor.dwPlazaVersion= pLogonVisitor->dwAppVersion;
    LogonVisitor.dwDeviceKind = pLogonVisitor->dwDeviceKind;
    LogonVisitor.dwSpreaderID = pLogonVisitor->dwSpreaderID;
	StringCchCopy(LogonVisitor.szMachineID, CountArray(LogonVisitor.szMachineID), pLogonVisitor->szMachineID);
	StringCchCopy(LogonVisitor.szMachineName, CountArray(LogonVisitor.szMachineName), pLogonVisitor->szMachineName);
	StringCchCopy(LogonVisitor.szMachineOSID, CountArray(LogonVisitor.szMachineOSID), pLogonVisitor->szMachineOSID);

	//扩展信息
	if (wDataSize > sizeof(CMD_MB_LogonVisitor))
	{
		//变量定义
		VOID* pDataBuffer = NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pLogonVisitor + 1, wDataSize - sizeof(CMD_MB_LogonVisitor));

		//读取信息
		while (true)
		{
			//提取数据
			pDataBuffer = RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe == DTP_NULL) break;

			//提取数据
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_MB_PACKAGE_NAME:		//应用包名
			{
				ASSERT(DataDescribe.wDataSize <= sizeof(LogonVisitor.szPackageName));
				if (DataDescribe.wDataSize <= sizeof(LogonVisitor.szPackageName))
				{
					LogonVisitor.szPackageName[CountArray(LogonVisitor.szPackageName) - 1L] = 0;
					StringCchCopy(LogonVisitor.szPackageName, CountArray(LogonVisitor.szPackageName), (LPCTSTR)pDataBuffer);
				}

				break;
			}
			}
		}
	}

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_VISITOR, dwTokenID,dwSocketID,dwTokenID,&LogonVisitor,sizeof(LogonVisitor));

	return true;
}

//秘钥登录
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonSecretKey(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize >= sizeof(CMD_MB_LogonSecretKey));
	if (wDataSize < sizeof(CMD_MB_LogonSecretKey)) return false;

	//绑定参数
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//获取参数
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//处理消息
	CMD_MB_LogonSecretKey* pLogonSecretKey = (CMD_MB_LogonSecretKey*)pData;
	pLogonSecretKey->szSecretKey[CountArray(pLogonSecretKey->szSecretKey) - 1] = 0;
	pLogonSecretKey->szMachineID[CountArray(pLogonSecretKey->szMachineID) - 1] = 0;
	pLogonSecretKey->szMobilePhone[CountArray(pLogonSecretKey->szMobilePhone) - 1] = 0;

	//设置连接
	pExtendParameter->cbClientKind = CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID = pLogonSecretKey->wModuleID;
	pExtendParameter->dwDeviceKind = pLogonSecretKey->dwDeviceKind;
	pExtendParameter->dwAppVersion = pLogonSecretKey->dwAppVersion;
	pExtendParameter->dwPlazaVersion = pLogonSecretKey->dwPlazaVersion;

	//版本判断
	if (CheckPlazaVersion(pLogonSecretKey->dwDeviceKind & DEVICE_MASK_MB, pLogonSecretKey->dwPlazaVersion, dwSocketID, dwTokenID) == false) return true;

	//变量定义
	DBR_MB_LogonSecretKey LogonSecretKey;
	ZeroMemory(&LogonSecretKey, sizeof(LogonSecretKey));

	//附加信息
	LogonSecretKey.pTokenParameter = pTokenParameter;
	LogonSecretKey.wGateID = pBindParameter->wServiceID;
	LogonSecretKey.wLogonID = m_pServiceOption->wServiceID;

	//构造数据	
	LogonSecretKey.dwUserID = pLogonSecretKey->dwUserID;
	LogonSecretKey.wMarketID = pLogonSecretKey->wMarketID;
	LogonSecretKey.dwClientIP = pTokenParameter->dwClientIP;
	LogonSecretKey.dwPlazaVersion = pLogonSecretKey->dwAppVersion;
	LogonSecretKey.dwDeviceKind = pLogonSecretKey->dwDeviceKind;
	StringCchCopy(LogonSecretKey.szSecretKey, CountArray(LogonSecretKey.szSecretKey), pLogonSecretKey->szSecretKey);
	StringCchCopy(LogonSecretKey.szMachineID, CountArray(LogonSecretKey.szMachineID), pLogonSecretKey->szMachineID);	
	StringCchCopy(LogonSecretKey.szMobilePhone, CountArray(LogonSecretKey.szMobilePhone), pLogonSecretKey->szMobilePhone);
	StringCchCopy(LogonSecretKey.szMachineOSID, CountArray(LogonSecretKey.szMachineOSID), pLogonSecretKey->szMachineOSID);

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_SECRETKEY, LogonSecretKey.dwUserID, dwSocketID, dwTokenID, &LogonSecretKey, sizeof(LogonSecretKey));

	return true;
}

//帐号登录
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	CTraceService::TraceString(_T("s1"), TraceLevel_Normal);
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_MB_LogonAccounts));
	if (wDataSize<sizeof(CMD_MB_LogonAccounts)) return false;
	CTraceService::TraceString(_T("s2"), TraceLevel_Normal);
	//绑定参数
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;
	CTraceService::TraceString(_T("s3"), TraceLevel_Normal);
	//获取参数
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;
	CTraceService::TraceString(_T("s4"), TraceLevel_Normal);
	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//处理消息
	CMD_MB_LogonAccounts * pLogonAccounts=(CMD_MB_LogonAccounts *)pData;
	pLogonAccounts->szAccounts[CountArray(pLogonAccounts->szAccounts)-1]=0;
	pLogonAccounts->szPassword[CountArray(pLogonAccounts->szPassword)-1]=0;
	pLogonAccounts->szMachineID[CountArray(pLogonAccounts->szMachineID)-1]=0;
	pLogonAccounts->szMobilePhone[CountArray(pLogonAccounts->szMobilePhone)-1]=0;

	//设置连接
	pExtendParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID=pLogonAccounts->wModuleID;
	pExtendParameter->dwDeviceKind=pLogonAccounts->dwDeviceKind;
	pExtendParameter->dwAppVersion=pLogonAccounts->dwAppVersion;
	pExtendParameter->dwPlazaVersion=pLogonAccounts->dwPlazaVersion;
	
	CString s;
	s.Format(_T("1 szAccounts:%s"), pLogonAccounts->szAccounts);
	CTraceService::TraceString(s, TraceLevel_Normal);
	
	//版本判断
	if (CheckPlazaVersion(pLogonAccounts->dwDeviceKind & DEVICE_MASK_MB, pLogonAccounts->dwPlazaVersion, dwSocketID, dwTokenID) == false)
	{
		CString s;
		s.Format(_T("2 szAccounts:%s"), pLogonAccounts->szAccounts);
		CTraceService::TraceString(s, TraceLevel_Normal);

		return true;
	}
	s.Format(_T("3 szAccounts:%s"), pLogonAccounts->szAccounts);
	CTraceService::TraceString(s, TraceLevel_Normal);

	//变量定义
	DBR_MB_LogonAccounts LogonAccounts;
	ZeroMemory(&LogonAccounts,sizeof(LogonAccounts));

	//附加信息
	LogonAccounts.pTokenParameter = pTokenParameter;
	LogonAccounts.wGateID = pBindParameter->wServiceID;
	LogonAccounts.wLogonID = m_pServiceOption->wServiceID;

	//构造数据	
	LogonAccounts.wMarketID=pLogonAccounts->wMarketID;
	LogonAccounts.dwClientIP = pTokenParameter->dwClientIP;
	LogonAccounts.dwPlazaVersion= pLogonAccounts->dwAppVersion;
	LogonAccounts.dwDeviceKind = pLogonAccounts->dwDeviceKind;
	StringCchCopy(LogonAccounts.szAccounts,CountArray(LogonAccounts.szAccounts), pLogonAccounts->szAccounts);
	StringCchCopy(LogonAccounts.szPassword,CountArray(LogonAccounts.szPassword), pLogonAccounts->szPassword);
	StringCchCopy(LogonAccounts.szMachineID,CountArray(LogonAccounts.szMachineID), pLogonAccounts->szMachineID);
	StringCchCopy(LogonAccounts.szMobilePhone,CountArray(LogonAccounts.szMobilePhone), pLogonAccounts->szMobilePhone);
	StringCchCopy(LogonAccounts.szMachineOSID, CountArray(LogonAccounts.szMachineOSID), pLogonAccounts->szMachineOSID);

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_ACCOUNTS,dwTokenID,dwSocketID,dwTokenID,&LogonAccounts,sizeof(LogonAccounts));

	return true;
}


//邮箱注册
bool CAttemperEngineSink::OnTCPNetworkSubMBRegisterEMail(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize >= sizeof(CMD_MB_RegisterEMail));
	if (wDataSize < sizeof(CMD_MB_RegisterEMail)) return false;

	//绑定参数
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//获取参数
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//处理消息
	CMD_MB_RegisterEMail* pRegisterAccounts = (CMD_MB_RegisterEMail*)pData;
	pRegisterAccounts->szAuthCode[CountArray(pRegisterAccounts->szAuthCode) - 1] = 0;
	pRegisterAccounts->szNickName[CountArray(pRegisterAccounts->szNickName) - 1] = 0;
	pRegisterAccounts->szMachineID[CountArray(pRegisterAccounts->szMachineID) - 1] = 0;
	pRegisterAccounts->szLogonPass[CountArray(pRegisterAccounts->szLogonPass) - 1] = 0;
	pRegisterAccounts->szEMail[CountArray(pRegisterAccounts->szEMail) - 1] = 0;

	//设置连接
	pExtendParameter->cbClientKind = CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID = pRegisterAccounts->wModuleID;
	pExtendParameter->dwDeviceKind = pRegisterAccounts->dwDeviceKind;
	pExtendParameter->dwAppVersion = pRegisterAccounts->dwAppVersion;
	pExtendParameter->dwPlazaVersion = pRegisterAccounts->dwPlazaVersion;

	//效验版本
	if (CheckPlazaVersion(pRegisterAccounts->dwDeviceKind & DEVICE_MASK_MB, pRegisterAccounts->dwPlazaVersion, dwSocketID, dwTokenID) == false) return true;

	//变量定义
	DBR_MB_RegisterEMail RegisterAccounts;
	ZeroMemory(&RegisterAccounts, sizeof(RegisterAccounts));

	//附加信息
	RegisterAccounts.pTokenParameter = pTokenParameter;
	RegisterAccounts.wGateID = pBindParameter->wServiceID;
	RegisterAccounts.wLogonID = m_pServiceOption->wServiceID;

	//构造数据
	RegisterAccounts.wFaceID = pRegisterAccounts->wFaceID;
	RegisterAccounts.cbGender = pRegisterAccounts->cbGender;
	RegisterAccounts.wMarketID = pRegisterAccounts->wMarketID;
	RegisterAccounts.dwClientIP = pTokenParameter->dwClientIP;
	RegisterAccounts.dwPlazaVersion = pRegisterAccounts->dwAppVersion;
	RegisterAccounts.dwDeviceKind = pRegisterAccounts->dwDeviceKind;
	RegisterAccounts.dwSpreaderID = pRegisterAccounts->dwSpreaderID;
	StringCchCopy(RegisterAccounts.szAuthCode, CountArray(RegisterAccounts.szAuthCode), pRegisterAccounts->szAuthCode);
	StringCchCopy(RegisterAccounts.szNickName, CountArray(RegisterAccounts.szNickName), pRegisterAccounts->szNickName);
	StringCchCopy(RegisterAccounts.szMachineID, CountArray(RegisterAccounts.szMachineID), pRegisterAccounts->szMachineID);
	StringCchCopy(RegisterAccounts.szLogonPass, CountArray(RegisterAccounts.szLogonPass), pRegisterAccounts->szLogonPass);
	StringCchCopy(RegisterAccounts.szEMail, CountArray(RegisterAccounts.szEMail), pRegisterAccounts->szEMail);
	StringCchCopy(RegisterAccounts.szMachineOSID, CountArray(RegisterAccounts.szMachineOSID), pRegisterAccounts->szMachineOSID);

	//扩展信息
	if (wDataSize > sizeof(CMD_MB_RegisterEMail))
	{
		//变量定义
		VOID* pDataBuffer = NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pRegisterAccounts + 1, wDataSize - sizeof(CMD_MB_RegisterEMail));

		//读取信息
		while (true)
		{
			//提取数据
			pDataBuffer = RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe == DTP_NULL) break;

			//提取数据
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_MB_PACKAGE_NAME:		//应用包名
			{
				ASSERT(DataDescribe.wDataSize <= sizeof(RegisterAccounts.szPackageName));
				if (DataDescribe.wDataSize <= sizeof(RegisterAccounts.szPackageName))
				{
					RegisterAccounts.szPackageName[CountArray(RegisterAccounts.szPackageName) - 1L] = 0;
					StringCchCopy(RegisterAccounts.szPackageName, CountArray(RegisterAccounts.szPackageName), (LPCTSTR)pDataBuffer);
				}

				break;
			}
			}
		}
	}

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_REGISTER_EMAIL, dwTokenID, dwSocketID, dwTokenID, &RegisterAccounts, sizeof(RegisterAccounts));

	return true;
}

//帐号注册
bool CAttemperEngineSink::OnTCPNetworkSubMBRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT (wDataSize>=sizeof(CMD_MB_RegisterAccounts));
	if (wDataSize<sizeof(CMD_MB_RegisterAccounts)) return false;

	//绑定参数
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//获取参数
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//处理消息
	CMD_MB_RegisterAccounts * pRegisterAccounts=(CMD_MB_RegisterAccounts *)pData;
	pRegisterAccounts->szAccounts[CountArray(pRegisterAccounts->szAccounts) - 1] = 0;
	pRegisterAccounts->szNickName[CountArray(pRegisterAccounts->szNickName)-1]=0;
	pRegisterAccounts->szMachineID[CountArray(pRegisterAccounts->szMachineID)-1]=0;
	pRegisterAccounts->szLogonPass[CountArray(pRegisterAccounts->szLogonPass)-1]=0;

	//设置连接
	pExtendParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID=pRegisterAccounts->wModuleID;
	pExtendParameter->dwDeviceKind=pRegisterAccounts->dwDeviceKind;
	pExtendParameter->dwAppVersion=pRegisterAccounts->dwAppVersion;
	pExtendParameter->dwPlazaVersion=pRegisterAccounts->dwPlazaVersion;

	//效验版本
	if (CheckPlazaVersion(pRegisterAccounts->dwDeviceKind & DEVICE_MASK_MB,pRegisterAccounts->dwPlazaVersion,dwSocketID,dwTokenID)==false) return true;

	//变量定义
	DBR_MB_RegisterAccounts RegisterAccounts;
	ZeroMemory(&RegisterAccounts,sizeof(RegisterAccounts));

	//附加信息
	RegisterAccounts.pTokenParameter = pTokenParameter;
	RegisterAccounts.wGateID = pBindParameter->wServiceID;
	RegisterAccounts.wLogonID = m_pServiceOption->wServiceID;

	//构造数据
	RegisterAccounts.wFaceID=pRegisterAccounts->wFaceID;
	RegisterAccounts.cbGender=pRegisterAccounts->cbGender;	
	RegisterAccounts.wMarketID=pRegisterAccounts->wMarketID;
	RegisterAccounts.dwClientIP=pTokenParameter->dwClientIP;
	RegisterAccounts.dwPlazaVersion=pRegisterAccounts->dwAppVersion;
    RegisterAccounts.dwDeviceKind = pRegisterAccounts->dwDeviceKind;
    RegisterAccounts.dwSpreaderID = pRegisterAccounts->dwSpreaderID;
	StringCchCopy(RegisterAccounts.szAccounts, CountArray(RegisterAccounts.szAccounts), pRegisterAccounts->szAccounts);
	StringCchCopy(RegisterAccounts.szNickName,CountArray(RegisterAccounts.szNickName), pRegisterAccounts->szNickName);
	StringCchCopy(RegisterAccounts.szMachineID,CountArray(RegisterAccounts.szMachineID), pRegisterAccounts->szMachineID);
	StringCchCopy(RegisterAccounts.szLogonPass,CountArray(RegisterAccounts.szLogonPass), pRegisterAccounts->szLogonPass);
	StringCchCopy(RegisterAccounts.szMachineOSID, CountArray(RegisterAccounts.szMachineOSID), pRegisterAccounts->szMachineOSID);

	//扩展信息
	if (wDataSize > sizeof(CMD_MB_RegisterAccounts))
	{
		//变量定义
		VOID* pDataBuffer = NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pRegisterAccounts + 1, wDataSize - sizeof(CMD_MB_RegisterAccounts));

		//读取信息
		while (true)
		{
			//提取数据
			pDataBuffer = RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe == DTP_NULL) break;

			//提取数据
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_MB_PACKAGE_NAME:		//应用包名
			{
				ASSERT(DataDescribe.wDataSize <= sizeof(RegisterAccounts.szPackageName));
				if (DataDescribe.wDataSize <= sizeof(RegisterAccounts.szPackageName))
				{
					RegisterAccounts.szPackageName[CountArray(RegisterAccounts.szPackageName) - 1L] = 0;
					StringCchCopy(RegisterAccounts.szPackageName, CountArray(RegisterAccounts.szPackageName), (LPCTSTR)pDataBuffer);
				}

				break;
			}
			}
		}
	}

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_REGISTER_ACCOUNTS, dwTokenID, dwSocketID,dwTokenID,&RegisterAccounts,sizeof(RegisterAccounts));

	return true;
}

//手机注册
bool CAttemperEngineSink::OnTCPNetworkSubMBRegisterMobilePhone(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize >= sizeof(CMD_MB_RegisterMobilePhone));
	if (wDataSize < sizeof(CMD_MB_RegisterMobilePhone)) return false;

	//绑定参数
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//获取参数
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//处理消息
	auto pRegisterAccounts = (CMD_MB_RegisterMobilePhone*)pData;
	pRegisterAccounts->szAuthCode[CountArray(pRegisterAccounts->szAuthCode) - 1] = 0;
	pRegisterAccounts->szNickName[CountArray(pRegisterAccounts->szNickName) - 1] = 0;
	pRegisterAccounts->szMachineID[CountArray(pRegisterAccounts->szMachineID) - 1] = 0;
	pRegisterAccounts->szLogonPass[CountArray(pRegisterAccounts->szLogonPass) - 1] = 0;
	pRegisterAccounts->szMobilePhone[CountArray(pRegisterAccounts->szMobilePhone) - 1] = 0;

	//设置连接
	pExtendParameter->cbClientKind = CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID = pRegisterAccounts->wModuleID;
	pExtendParameter->dwDeviceKind = pRegisterAccounts->dwDeviceKind;
	pExtendParameter->dwAppVersion = pRegisterAccounts->dwAppVersion;
	pExtendParameter->dwPlazaVersion = pRegisterAccounts->dwPlazaVersion;

	//效验版本
	if (CheckPlazaVersion(pRegisterAccounts->dwDeviceKind & DEVICE_MASK_MB, pRegisterAccounts->dwPlazaVersion, dwSocketID, dwTokenID) == false) return true;

	//变量定义
	DBR_MB_RegisterMobilePhone RegisterMobilePhone;
	ZeroMemory(&RegisterMobilePhone, sizeof(RegisterMobilePhone));

	//附加信息
	RegisterMobilePhone.pTokenParameter = pTokenParameter;
	RegisterMobilePhone.wGateID = pBindParameter->wServiceID;
	RegisterMobilePhone.wLogonID = m_pServiceOption->wServiceID;

	//构造数据
	RegisterMobilePhone.wFaceID = pRegisterAccounts->wFaceID;
	RegisterMobilePhone.cbGender = pRegisterAccounts->cbGender;
	RegisterMobilePhone.wMarketID = pRegisterAccounts->wMarketID;
	RegisterMobilePhone.dwClientIP = pTokenParameter->dwClientIP;
	RegisterMobilePhone.dwPlazaVersion = pRegisterAccounts->dwAppVersion;
	RegisterMobilePhone.dwDeviceKind = pRegisterAccounts->dwDeviceKind;
	RegisterMobilePhone.dwSpreaderID = pRegisterAccounts->dwSpreaderID;
	StringCchCopy(RegisterMobilePhone.szAuthCode, CountArray(RegisterMobilePhone.szAuthCode), pRegisterAccounts->szAuthCode);
	StringCchCopy(RegisterMobilePhone.szNickName, CountArray(RegisterMobilePhone.szNickName), pRegisterAccounts->szNickName);
	StringCchCopy(RegisterMobilePhone.szMachineID, CountArray(RegisterMobilePhone.szMachineID), pRegisterAccounts->szMachineID);
	StringCchCopy(RegisterMobilePhone.szLogonPass, CountArray(RegisterMobilePhone.szLogonPass), pRegisterAccounts->szLogonPass);
	StringCchCopy(RegisterMobilePhone.szMobilePhone, CountArray(RegisterMobilePhone.szMobilePhone), pRegisterAccounts->szMobilePhone);
	StringCchCopy(RegisterMobilePhone.szMachineOSID, CountArray(RegisterMobilePhone.szMachineOSID), pRegisterAccounts->szMachineOSID);

	//扩展信息
	if (wDataSize > sizeof(CMD_MB_RegisterMobilePhone))
	{
		//变量定义
		VOID* pDataBuffer = NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pRegisterAccounts + 1, wDataSize - sizeof(CMD_MB_RegisterMobilePhone));

		//读取信息
		while (true)
		{
			//提取数据
			pDataBuffer = RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe == DTP_NULL) break;

			//提取数据
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_MB_PACKAGE_NAME:		//应用包名
			{
				ASSERT(DataDescribe.wDataSize <= sizeof(RegisterMobilePhone.szPackageName));
				if (DataDescribe.wDataSize <= sizeof(RegisterMobilePhone.szPackageName))
				{
					RegisterMobilePhone.szPackageName[CountArray(RegisterMobilePhone.szPackageName) - 1L] = 0;
					StringCchCopy(RegisterMobilePhone.szPackageName, CountArray(RegisterMobilePhone.szPackageName), (LPCTSTR)pDataBuffer);
				}

				break;
			}
			}
		}
	}

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_REGISTER_MOBILEPHONE, dwTokenID, dwSocketID, dwTokenID, &RegisterMobilePhone, sizeof(RegisterMobilePhone));

	return true;
}


//邮箱登录
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonEMail(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize >= sizeof(CMD_MB_LogonEMail));
	if (wDataSize < sizeof(CMD_MB_LogonEMail)) return false;

	//绑定参数
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//获取参数
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//处理消息
	CMD_MB_LogonEMail* pLogonEMail = (CMD_MB_LogonEMail*)pData;
	pLogonEMail->szPassword[CountArray(pLogonEMail->szPassword) - 1] = 0;
	pLogonEMail->szMachineID[CountArray(pLogonEMail->szMachineID) - 1] = 0;
	pLogonEMail->szEMail[CountArray(pLogonEMail->szEMail) - 1] = 0;

	//设置连接
	pExtendParameter->cbClientKind = CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID = pLogonEMail->wModuleID;
	pExtendParameter->dwDeviceKind = pLogonEMail->dwDeviceKind;
	pExtendParameter->dwAppVersion = pLogonEMail->dwAppVersion;
	pExtendParameter->dwPlazaVersion = pLogonEMail->dwPlazaVersion;

	//版本判断
	if (CheckPlazaVersion(pLogonEMail->dwDeviceKind & DEVICE_MASK_MB, pLogonEMail->dwPlazaVersion, dwSocketID, dwTokenID) == false) return true;

	//变量定义
	DBR_MB_LogonEMail LogonEMail;
	ZeroMemory(&LogonEMail, sizeof(LogonEMail));

	//附加信息
	LogonEMail.pTokenParameter = pTokenParameter;
	LogonEMail.wGateID = pBindParameter->wServiceID;
	LogonEMail.wLogonID = m_pServiceOption->wServiceID;

	//构造数据	
	LogonEMail.wMarketID = pLogonEMail->wMarketID;
	LogonEMail.dwClientIP = pTokenParameter->dwClientIP;
	LogonEMail.dwPlazaVersion = pLogonEMail->dwAppVersion;
	LogonEMail.dwDeviceKind = pLogonEMail->dwDeviceKind;
	StringCchCopy(LogonEMail.szPassword, CountArray(LogonEMail.szPassword), pLogonEMail->szPassword);
	StringCchCopy(LogonEMail.szMachineID, CountArray(LogonEMail.szMachineID), pLogonEMail->szMachineID);
	StringCchCopy(LogonEMail.szEMail, CountArray(LogonEMail.szEMail), pLogonEMail->szEMail);
	StringCchCopy(LogonEMail.szMachineOSID, CountArray(LogonEMail.szMachineOSID), pLogonEMail->szMachineOSID);

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_EMAIL, dwTokenID, dwSocketID, dwTokenID, &LogonEMail, sizeof(LogonEMail));

	return true;
}

//邮箱验证码登录
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonEMailCode(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize >= sizeof(CMD_MB_LogonEMailCode));
	if (wDataSize < sizeof(CMD_MB_LogonEMailCode)) return false;

	//绑定参数
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//获取参数
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//处理消息
	CMD_MB_LogonEMailCode* pLogonMobileCode = (CMD_MB_LogonEMailCode*)pData;
	pLogonMobileCode->szAuthCode[CountArray(pLogonMobileCode->szAuthCode) - 1] = 0;
	pLogonMobileCode->szMachineID[CountArray(pLogonMobileCode->szMachineID) - 1] = 0;
	pLogonMobileCode->szEMail[CountArray(pLogonMobileCode->szEMail) - 1] = 0;

	//设置连接
	pExtendParameter->cbClientKind = CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID = pLogonMobileCode->wModuleID;
	pExtendParameter->dwDeviceKind = pLogonMobileCode->dwDeviceKind;
	pExtendParameter->dwAppVersion = pLogonMobileCode->dwAppVersion;
	pExtendParameter->dwPlazaVersion = pLogonMobileCode->dwPlazaVersion;

	//版本判断
	if (CheckPlazaVersion(pLogonMobileCode->dwDeviceKind & DEVICE_MASK_MB, pLogonMobileCode->dwPlazaVersion, dwSocketID, dwTokenID) == false) return true;

	//变量定义
	DBR_MB_LogonEMailCode LogonMobileCode;
	ZeroMemory(&LogonMobileCode, sizeof(LogonMobileCode));

	//连接信息
	LogonMobileCode.pTokenParameter = pTokenParameter;
	LogonMobileCode.wGateID = pBindParameter->wServiceID;
	LogonMobileCode.wLogonID = m_pServiceOption->wServiceID;

	//构造数据	
	LogonMobileCode.wMarketID = pLogonMobileCode->wMarketID;
	LogonMobileCode.dwClientIP = pTokenParameter->dwClientIP;
	LogonMobileCode.dwPlazaVersion = pLogonMobileCode->dwAppVersion;
	LogonMobileCode.dwDeviceKind = pLogonMobileCode->dwDeviceKind;
	LogonMobileCode.dwSpreaderID = pLogonMobileCode->dwSpreaderID;
	StringCchCopy(LogonMobileCode.szAuthCode, CountArray(LogonMobileCode.szAuthCode), pLogonMobileCode->szAuthCode);
	StringCchCopy(LogonMobileCode.szMachineID, CountArray(LogonMobileCode.szMachineID), pLogonMobileCode->szMachineID);
	StringCchCopy(LogonMobileCode.szEMail, CountArray(LogonMobileCode.szEMail), pLogonMobileCode->szEMail);
	StringCchCopy(LogonMobileCode.szMachineOSID, CountArray(LogonMobileCode.szMachineOSID), pLogonMobileCode->szMachineOSID);

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_EMAILCODE, dwTokenID, dwSocketID, dwTokenID, &LogonMobileCode, sizeof(LogonMobileCode));

	return true;
}

//验证码登录
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonMobileCode(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize >= sizeof(CMD_MB_LogonMobileCode));
	if (wDataSize < sizeof(CMD_MB_LogonMobileCode)) return false;

	//绑定参数
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//获取参数
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//处理消息
	CMD_MB_LogonMobileCode * pLogonMobileCode = (CMD_MB_LogonMobileCode *)pData;
	pLogonMobileCode->szAuthCode[CountArray(pLogonMobileCode->szAuthCode) - 1] = 0;
	pLogonMobileCode->szMachineID[CountArray(pLogonMobileCode->szMachineID) - 1] = 0;
	pLogonMobileCode->szMobilePhone[CountArray(pLogonMobileCode->szMobilePhone) - 1] = 0;

	//设置连接
	pExtendParameter->cbClientKind = CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID = pLogonMobileCode->wModuleID;
	pExtendParameter->dwDeviceKind = pLogonMobileCode->dwDeviceKind;
	pExtendParameter->dwAppVersion = pLogonMobileCode->dwAppVersion;
	pExtendParameter->dwPlazaVersion = pLogonMobileCode->dwPlazaVersion;

	//版本判断
	if (CheckPlazaVersion(pLogonMobileCode->dwDeviceKind & DEVICE_MASK_MB, pLogonMobileCode->dwPlazaVersion, dwSocketID, dwTokenID) == false) return true;

	//变量定义
	DBR_MB_LogonMobileCode LogonMobileCode;
	ZeroMemory(&LogonMobileCode, sizeof(LogonMobileCode));

	//连接信息
	LogonMobileCode.pTokenParameter = pTokenParameter;
	LogonMobileCode.wGateID = pBindParameter->wServiceID;
	LogonMobileCode.wLogonID = m_pServiceOption->wServiceID;

	//构造数据	
	LogonMobileCode.wMarketID = pLogonMobileCode->wMarketID;
	LogonMobileCode.dwClientIP = pTokenParameter->dwClientIP;
	LogonMobileCode.dwPlazaVersion = pLogonMobileCode->dwAppVersion;
	LogonMobileCode.dwDeviceKind = pLogonMobileCode->dwDeviceKind;
	LogonMobileCode.dwSpreaderID = pLogonMobileCode->dwSpreaderID;
	StringCchCopy(LogonMobileCode.szAuthCode, CountArray(LogonMobileCode.szAuthCode), pLogonMobileCode->szAuthCode);
	StringCchCopy(LogonMobileCode.szMachineID, CountArray(LogonMobileCode.szMachineID), pLogonMobileCode->szMachineID);
	StringCchCopy(LogonMobileCode.szMobilePhone, CountArray(LogonMobileCode.szMobilePhone), pLogonMobileCode->szMobilePhone);
	StringCchCopy(LogonMobileCode.szMachineOSID, CountArray(LogonMobileCode.szMachineOSID), pLogonMobileCode->szMachineOSID);

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_MOBILECODE, dwTokenID, dwSocketID, dwTokenID, &LogonMobileCode, sizeof(LogonMobileCode));

	return true;
}

//其他登录
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonThirdParty(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT (wDataSize>=sizeof(CMD_MB_LogonThirdParty));
	if (wDataSize<sizeof(CMD_MB_LogonThirdParty)) return false;

	//绑定参数
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//获取参数
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//处理消息
	CMD_MB_LogonThirdParty * pLogonThirdParty=(CMD_MB_LogonThirdParty *)pData;
	pLogonThirdParty->szUniqueID[CountArray(pLogonThirdParty->szUniqueID)-1]=0;
	pLogonThirdParty->szNickName[CountArray(pLogonThirdParty->szNickName)-1]=0;
	pLogonThirdParty->szMachineID[CountArray(pLogonThirdParty->szMachineID)-1]=0;
	pLogonThirdParty->szMobilePhone[CountArray(pLogonThirdParty->szMobilePhone)-1]=0;
	pLogonThirdParty->szCompellation[CountArray(pLogonThirdParty->szCompellation)-1]=0;

	//设置连接
	pExtendParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID=pLogonThirdParty->wModuleID;
	pExtendParameter->dwDeviceKind=pLogonThirdParty->dwDeviceKind;
	pExtendParameter->dwAppVersion=pLogonThirdParty->dwAppVersion;
	pExtendParameter->dwPlazaVersion=pLogonThirdParty->dwPlazaVersion;

	//版本判断
	if (CheckPlazaVersion(pLogonThirdParty->dwDeviceKind & DEVICE_MASK_MB,pLogonThirdParty->dwPlazaVersion,dwSocketID,dwTokenID)==false) return true;

	//变量定义
	DBR_MB_LogonThirdParty LogonThirdParty;
	ZeroMemory(&LogonThirdParty,sizeof(LogonThirdParty));

	//连接信息
	LogonThirdParty.pTokenParameter = pTokenParameter;
	LogonThirdParty.wGateID = pBindParameter->wServiceID;
	LogonThirdParty.wLogonID = m_pServiceOption->wServiceID;

	//构造数据		
	LogonThirdParty.cbGender=pLogonThirdParty->cbGender;	
	LogonThirdParty.wMarketID=pLogonThirdParty->wMarketID;
	LogonThirdParty.dwClientIP = pTokenParameter->dwClientIP;
	LogonThirdParty.dwExternalID=pLogonThirdParty->dwExternalID;	
	LogonThirdParty.dwPlazaVersion=pLogonThirdParty->dwAppVersion;
    LogonThirdParty.dwDeviceKind = pLogonThirdParty->dwDeviceKind;
    LogonThirdParty.dwSpreaderID = pLogonThirdParty->dwSpreaderID;
	StringCchCopy(LogonThirdParty.szUniqueID,CountArray(LogonThirdParty.szUniqueID), pLogonThirdParty->szUniqueID);
	StringCchCopy(LogonThirdParty.szNickName,CountArray(LogonThirdParty.szNickName), pLogonThirdParty->szNickName);
	StringCchCopy(LogonThirdParty.szMachineID,CountArray(LogonThirdParty.szMachineID), pLogonThirdParty->szMachineID);
	StringCchCopy(LogonThirdParty.szMachineOSID, CountArray(LogonThirdParty.szMachineOSID), pLogonThirdParty->szMachineOSID);
	StringCchCopy(LogonThirdParty.szMobilePhone,CountArray(LogonThirdParty.szMobilePhone), pLogonThirdParty->szMobilePhone);	
	StringCchCopy(LogonThirdParty.szCompellation,CountArray(LogonThirdParty.szCompellation), pLogonThirdParty->szCompellation);
	
	//扩展信息
	if (wDataSize > sizeof(CMD_MB_LogonThirdParty))
	{
		//变量定义
		VOID* pDataBuffer = NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pLogonThirdParty + 1, wDataSize - sizeof(CMD_MB_LogonThirdParty));

		//读取信息
		while (true)
		{
			//提取数据
			pDataBuffer = RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe == DTP_NULL) break;

			//提取数据
			switch (DataDescribe.wDataDescribe)
			{	
			case DTP_MB_USERFACE_URL:		//头像地址
			{
				ASSERT(DataDescribe.wDataSize <= sizeof(LogonThirdParty.szUserFaceUrl));
				if (DataDescribe.wDataSize <= sizeof(LogonThirdParty.szUserFaceUrl))
				{
					LogonThirdParty.szUserFaceUrl[CountArray(LogonThirdParty.szUserFaceUrl) - 1L] = 0;
					StringCchCopy(LogonThirdParty.szUserFaceUrl, CountArray(LogonThirdParty.szUserFaceUrl), (LPCTSTR)pDataBuffer);
				}

				break;
			}
			case DTP_MB_EXTERNAL_DATUM:		//外部资料
			{
				ASSERT (DataDescribe.wDataSize <= sizeof(LogonThirdParty.szUserDatum));
				if (DataDescribe.wDataSize <= sizeof(LogonThirdParty.szUserDatum))
				{
					LogonThirdParty.szUserDatum[CountArray(LogonThirdParty.szUserDatum) - 1L] = 0;
					StringCchCopy(LogonThirdParty.szUserDatum, CountArray(LogonThirdParty.szUserDatum), (LPCTSTR)pDataBuffer);
				}

				break;
			}
			case DTP_MB_PACKAGE_NAME:		//应用包名
			{
				ASSERT(DataDescribe.wDataSize <= sizeof(LogonThirdParty.szPackageName));
				if (DataDescribe.wDataSize <= sizeof(LogonThirdParty.szPackageName))
				{
					LogonThirdParty.szPackageName[CountArray(LogonThirdParty.szPackageName) - 1L] = 0;
					StringCchCopy(LogonThirdParty.szPackageName, CountArray(LogonThirdParty.szPackageName), (LPCTSTR)pDataBuffer);
				}
				break;
			}
			}
		}
	}
	////CString s;
	//s.Format(_T(""))/*;*/
	//CTraceService::TraceString(LogonThirdParty.szUserFaceUrl, TraceLevel_Normal);
	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_THIRDPARTY, dwTokenID, dwSocketID,dwTokenID,&LogonThirdParty,sizeof(LogonThirdParty));

	return true;
}

//手机登录
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonMobilePhone(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize >= sizeof(CMD_MB_LogonMobilePhone));
	if (wDataSize < sizeof(CMD_MB_LogonMobilePhone)) return false;

	//绑定参数
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//获取参数
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//处理消息
	CMD_MB_LogonMobilePhone * pLogonMobilePhone = (CMD_MB_LogonMobilePhone *)pData;
	pLogonMobilePhone->szPassword[CountArray(pLogonMobilePhone->szPassword) - 1] = 0;
	pLogonMobilePhone->szMachineID[CountArray(pLogonMobilePhone->szMachineID) - 1] = 0;
	pLogonMobilePhone->szMobilePhone[CountArray(pLogonMobilePhone->szMobilePhone) - 1] = 0;

	//设置连接
	pExtendParameter->cbClientKind = CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID = pLogonMobilePhone->wModuleID;
	pExtendParameter->dwDeviceKind = pLogonMobilePhone->dwDeviceKind;
	pExtendParameter->dwAppVersion = pLogonMobilePhone->dwAppVersion;
	pExtendParameter->dwPlazaVersion = pLogonMobilePhone->dwPlazaVersion;

	//版本判断
	if (CheckPlazaVersion(pLogonMobilePhone->dwDeviceKind & DEVICE_MASK_MB, pLogonMobilePhone->dwPlazaVersion, dwSocketID, dwTokenID) == false) return true;

	//变量定义
	DBR_MB_LogonMobilePhone LogonMobilePhone;
	ZeroMemory(&LogonMobilePhone, sizeof(LogonMobilePhone));

	//附加信息
	LogonMobilePhone.pTokenParameter = pTokenParameter;
	LogonMobilePhone.wGateID = pBindParameter->wServiceID;
	LogonMobilePhone.wLogonID = m_pServiceOption->wServiceID;

	//构造数据	
	LogonMobilePhone.wMarketID = pLogonMobilePhone->wMarketID;
	LogonMobilePhone.dwClientIP = pTokenParameter->dwClientIP;
	LogonMobilePhone.dwPlazaVersion = pLogonMobilePhone->dwAppVersion;
	LogonMobilePhone.dwDeviceKind = pLogonMobilePhone->dwDeviceKind;
	StringCchCopy(LogonMobilePhone.szPassword, CountArray(LogonMobilePhone.szPassword), pLogonMobilePhone->szPassword);
	StringCchCopy(LogonMobilePhone.szMachineID, CountArray(LogonMobilePhone.szMachineID), pLogonMobilePhone->szMachineID);
	StringCchCopy(LogonMobilePhone.szMobilePhone, CountArray(LogonMobilePhone.szMobilePhone), pLogonMobilePhone->szMobilePhone);
	StringCchCopy(LogonMobilePhone.szMachineOSID, CountArray(LogonMobilePhone.szMachineOSID), pLogonMobilePhone->szMachineOSID);

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_MOBILEPHONE, dwTokenID, dwSocketID, dwTokenID, &LogonMobilePhone, sizeof(LogonMobilePhone));

	return true;
}

//列表事件
bool CAttemperEngineSink::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{	
	case SUB_CS_S_SERVICE_INSERT:
		{
			//效验参数
			ASSERT(wDataSize % sizeof(tagServiceItem) == 0);
			if (wDataSize % sizeof(tagServiceItem) != 0) return false;

			//变量定义
			WORD wItemCount = wDataSize / sizeof(tagServiceItem);
			tagServiceItem * pServiceItem = (tagServiceItem *)pData;

			return true;
		}
	case SUB_CS_S_SERVICE_REMOVE:
		{
			//效验参数
			ASSERT (wDataSize == sizeof(CMD_CS_S_ServiceRemove));
			if (wDataSize != sizeof(CMD_CS_S_ServiceRemove)) return false;

			//变量定义
			CMD_CS_S_ServiceRemove * pServiceRemove = (CMD_CS_S_ServiceRemove *)pData;

			return true;
		}
	case SUB_CS_S_SERVER_INFO:		//房间信息
		{
			//废弃列表
			m_ServerListManager.DisuseServerItem();

			return true;
		}
	case SUB_CS_S_SERVICE_ONLINE:	//服务人数
		{
			//效验参数
			ASSERT (wDataSize==sizeof(CMD_CS_S_ServiceOnline));
			if (wDataSize!=sizeof(CMD_CS_S_ServiceOnline)) return false;

			//变量定义
			CMD_CS_S_ServiceOnline * pServiceOnline =(CMD_CS_S_ServiceOnline *)pData;

			//校验模块
			if (pServiceOnline->wServiceModule != SERVICE_MODULE_GAME) return true;

			//查找房间
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServiceOnline->wServiceID);

			//设置人数
			if (pGameServerItem!=NULL)
			{
				pGameServerItem->m_GameServer.dwOnLineCount= pServiceOnline->wOnlineCount;
			}

			return true;
		}
	case SUB_CS_S_SERVER_INSERT:	//房间插入
		{
			//效验参数
			ASSERT(wDataSize%sizeof(tagGameServer)==0);
			if (wDataSize%sizeof(tagGameServer)!=0) return false;

			//变量定义
			WORD wItemCount=wDataSize/sizeof(tagGameServer);
			tagGameServer * pGameServer=(tagGameServer *)pData;

			//更新数据
			for (WORD i=0;i<wItemCount;i++)
			{
				m_ServerListManager.InsertGameServer(pGameServer++);			
			}

			return true;
		}
	case SUB_CS_S_SERVER_REMOVE:	//房间移除
		{
			//效验参数
			ASSERT(wDataSize == sizeof(CMD_CS_S_ServiceRemove));
			if (wDataSize != sizeof(CMD_CS_S_ServiceRemove)) return false;

			//变量定义
			CMD_CS_S_ServiceRemove* pServiceRemove = (CMD_CS_S_ServiceRemove*)pData;

			//查找房间
			auto pGameServerItem = m_ServerListManager.SearchGameServer(pServiceRemove->wServiceID);
			if (pGameServerItem != NULL)
			{
				//类型标识
				WORD wKindID = pGameServerItem->m_GameServer.wKindID;

				//变量定义
				m_ServerListManager.DeleteGameServer(pServiceRemove->wServiceID);
			}			

			return true;
		}
	case SUB_CS_S_GAME_JACKPOT:		//游戏奖池
		{
			//效验参数
			ASSERT (wDataSize%sizeof(CMD_CS_S_GameJackpot)==0);
			if ((wDataSize%sizeof(CMD_CS_S_GameJackpot))!=0) return false;

			//变量定义
			auto pGameJackpot = (CMD_CS_S_GameJackpot*)pData;
			WORD wJackpotCount = wDataSize / sizeof(CMD_CS_S_GameJackpot);

			for (int i = 0; i < wJackpotCount; i++)
			{
				//查找类型
				auto pGameKindItem = m_ServerListManager.SearchGameKind(pGameJackpot->wKindID);
				if (pGameKindItem != NULL)
				{
					pGameKindItem->m_ulLastTickCount = GetTickCount64();
					pGameKindItem->m_lJackpotScore = pGameJackpot->lJackpotScore;
				}

				pGameJackpot++;
			}			

			return true;
		}
	case SUB_CS_S_SERVER_MODIFY:	//房间修改
		{
			//效验参数
			ASSERT(wDataSize%sizeof(CMD_CS_S_ServerModify)==0);
			if (wDataSize%sizeof(CMD_CS_S_ServerModify)!=0) return false;

			//变量定义
			CMD_CS_S_ServerModify * pServerModify=(CMD_CS_S_ServerModify *)pData;

			//查找房间
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerModify->wServerID);
			CGameFilterItem * pGameFilterItem=m_ServerListManager.SearchGameFilter(pServerModify->wServerID);

			//设置房间
			if (pGameServerItem!=NULL)
			{
				//过滤信息
				if (pGameFilterItem!=NULL)
				{
					//设置变量
					pServerModify->wSortID=pGameFilterItem->FilterSortID(pServerModify->wSortID);
					pServerModify->wKindID=pGameFilterItem->FilterKindID(pServerModify->wKindID);
					pServerModify->wNodeID=pGameFilterItem->FilterNodeID(pServerModify->wNodeID);

					//房间名字
					LPCTSTR pszServerName=pGameFilterItem->FilterServerName(pServerModify->szServerName);
					StringCchCopy(pServerModify->szServerName,CountArray(pServerModify->szServerName), pszServerName);
				}

				//设置房间
				pGameServerItem->m_GameServer.wNodeID=pServerModify->wNodeID;
				pGameServerItem->m_GameServer.wSortID=pServerModify->wSortID;
				pGameServerItem->m_GameServer.wKindID=pServerModify->wKindID;
				pGameServerItem->m_GameServer.wVisibleMask=pServerModify->wVisibleMask;
				pGameServerItem->m_GameServer.wServerPort=pServerModify->wServerPort;
				pGameServerItem->m_GameServer.wServerType=pServerModify->wServerType;
				pGameServerItem->m_GameServer.dwMaxPlayer=pServerModify->dwMaxPlayer;
				pGameServerItem->m_GameServer.dwOnLineCount=pServerModify->dwOnLineCount;
				pGameServerItem->m_GameServer.dwServerAddr=pServerModify->dwServerAddr;
				StringCchCopy(pGameServerItem->m_GameServer.szServerName,LEN_SERVER, pServerModify->szServerName);
				StringCchCopy(pGameServerItem->m_GameServer.szServerDomain,LEN_DOMAIN, pServerModify->szServerDomain);

				//分数配置
				pGameServerItem->m_GameServer.lCellScore=pServerModify->lCellScore;
				pGameServerItem->m_GameServer.lMinEnterScore=pServerModify->lMinEnterScore;
				pGameServerItem->m_GameServer.lMaxEnterScore=pServerModify->lMaxEnterScore;
			}

			return true;
		}	
	case SUB_CS_S_SERVER_FINISH:	//房间完成
		{
			//设置列表
			m_ServerListManager.CleanServerItem();
			m_ServerListManager.CollectOnlineInfo();

			//事件处理
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CENTER_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}			
	}

	return true;
}

//用户收集
bool CAttemperEngineSink::OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_COLLECT_USER:		//收集用户
	{
		//变量定义
		CGlobalUserItem* pGlobalUserItem = NULL;

		do
		{
			pGlobalUserItem = m_GlobalItemManager.EnumUserItem(pGlobalUserItem);
			if (pGlobalUserItem != NULL)
			{
				//构造结构
				CMD_CS_C_UserOnline UserOnline;
				ZeroMemory(&UserOnline, sizeof(UserOnline));

				//设置变量
				UserOnline.dwUserID = pGlobalUserItem->m_dwUserID;
				UserOnline.dwGameID = pGlobalUserItem->m_dwGameID;
				StringCchCopy(UserOnline.szNickName, CountArray(UserOnline.szNickName), pGlobalUserItem->m_szNickName);
				StringCchCopy(UserOnline.szUserHeadUrl, CountArray(UserOnline.szUserHeadUrl), pGlobalUserItem->m_szUserHeadUrl);//kk jia

				//用户资料
				UserOnline.cbGender = pGlobalUserItem->m_cbGender;
				UserOnline.wFaceID = pGlobalUserItem->m_wFaceID;
				UserOnline.dwCustomID = pGlobalUserItem->m_dwCustomID;

				//状态信息
				UserOnline.cbShutUpStatus = pGlobalUserItem->m_cbShutUpStatus;
				UserOnline.dwShutUpOverTime = pGlobalUserItem->m_dwShutUpOverTime;

				//网络信息
				UserOnline.wGateID = pGlobalUserItem->GetGateID();
				UserOnline.dwTokenID = pGlobalUserItem->GetTokenID();

				//用户上线
				m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_USER_ONLINE, &UserOnline, sizeof(UserOnline), 0);
			}

		} while (pGlobalUserItem!=NULL);		

		//收集完成
		m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_COLLECT_FINISH, NULL, 0, 0);

		return true;
	}
	}

	return true;
}

//转发服务
bool CAttemperEngineSink::OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_USER_SHUTUP_STATUS:	//禁言状态
	{
		//校验数据
		ASSERT(wDataSize == sizeof(CMD_CS_UserShutUpStatus));
		if (wDataSize != sizeof(CMD_CS_UserShutUpStatus)) return true;

		//提取数据
		auto pUserShutUpStatus = (CMD_CS_UserShutUpStatus*)pData;

		//查找用户
		auto pGlobalUserItem = m_GlobalItemManager.SearchUserItem(pUserShutUpStatus->dwUserID);
		if (pGlobalUserItem != NULL)
		{
			pGlobalUserItem->m_cbShutUpStatus = pUserShutUpStatus->cbShutUpStatus;
			pGlobalUserItem->m_dwShutUpOverTime = pUserShutUpStatus->dwShutUpOverTime;

			//查找网关
			auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGlobalUserItem->GetGateID());
			if (pGlobalGateItem != NULL)
			{
				//构造结构
				CMD_MB_ShutUpStatusUpdate ShutUpStatusUpdate;
				ZeroMemory(&ShutUpStatusUpdate,sizeof(ShutUpStatusUpdate));

				//设置变量
				ShutUpStatusUpdate.cbShutUpStatus = pUserShutUpStatus->cbShutUpStatus;
				ShutUpStatusUpdate.dwShutUpOverTime = pUserShutUpStatus->dwShutUpOverTime;

				//发送消息	
				DWORD dwTokenID = pGlobalUserItem->GetTokenID();
				SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_MB_USER_SERVICE, SUB_MB_SHUTUP_STATUS_UPDATE, &ShutUpStatusUpdate, sizeof(ShutUpStatusUpdate),&dwTokenID,1);
			}
		}	

		return true;
	}	
	case SUB_CS_UPDATE_ACTION_MASK:	//更新掩码
	{
		//提取数据
		auto pUpdateActionMask= (CMD_CS_UpdateActionMask*)pData;

		//查找用户
		auto pGlobalUserItem = m_GlobalItemManager.SearchUserItem(pUpdateActionMask->dwUserID);
		if (pGlobalUserItem != NULL)
		{
			//查询网关
			auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGlobalUserItem->GetGateID());
			if (pGlobalGateItem != NULL)
			{
				//变量定义
				CMD_MB_UserActionMask UserActionMask = {};

				//设置变量
				UserActionMask.dwActionMaskEver = pUpdateActionMask->dwActionMaskEver;
				UserActionMask.dwActionMaskPerDay = pUpdateActionMask->dwActionMaskPerDay;
				UserActionMask.dwActionMaskPerWeek = pUpdateActionMask->dwActionMaskPerWeek;

				//网络信息
				auto dwTokenID = pGlobalUserItem->GetTokenID();
				auto dwSocketID = pGlobalGateItem->GetSocketID();				

				//发送消息
				SendDataToGate(dwSocketID, MDM_MB_USER_SERVICE, SUB_MB_USER_ACTION_MASK, &UserActionMask, sizeof(UserActionMask),&dwTokenID,1);
			}			
		}		

		return true;
	}
	case SUB_CS_LOGOUT_NOTIFY:		//登出提醒
	{
		//提取数据
		auto pLogoutNotify = (CMD_CS_LogoutNotify*)pData;

		//查找用户
		auto pGlobalUserItem = m_GlobalItemManager.SearchUserItem(pLogoutNotify->dwUserID);
		if (pGlobalUserItem == NULL) return true;


		//查找网关
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGlobalUserItem->GetGateID());
		if (pGlobalGateItem == NULL) return true;

		//令牌参数
		auto pTokenParameter = pGlobalGateItem->SearchTokenID(pGlobalUserItem->GetTokenID());
		if (pTokenParameter == NULL) return true;

		//扩展参数
		auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

		//电脑端
		if (pExtendParameter->cbClientKind == CLIENT_KIND_COMPUTER)
		{
			//构造结构
			CMD_GP_LogoutNotify LogoutNotify;
			ZeroMemory(&LogoutNotify, sizeof(LogoutNotify));

			//设置变量
			StringCchCopy(LogoutNotify.szDescribeString, CountArray(LogoutNotify.szDescribeString), pLogoutNotify->szNotifyContent);

			//发送数据		
			WORD wMsgSize = CountStringBuffer(LogoutNotify.szDescribeString);
			WORD wHeadSize = sizeof(LogoutNotify) - sizeof(LogoutNotify.szDescribeString);
			SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_GP_LOGON, SUB_GP_LOGOUT_NOTIFY, &LogoutNotify, wHeadSize + wMsgSize, &pGlobalUserItem->m_dwTokenID, 1);
		}		
		else
		{
			//构造结构
			CMD_MB_LogoutNotify LogoutNotify;
			ZeroMemory(&LogoutNotify, sizeof(LogoutNotify));

			//设置变量
			StringCchCopy(LogoutNotify.szDescribeString, CountArray(LogoutNotify.szDescribeString), pLogoutNotify->szNotifyContent);

			//发送数据		
			WORD wMsgSize = CountStringBuffer(LogoutNotify.szDescribeString);
			WORD wHeadSize = sizeof(LogoutNotify) - sizeof(LogoutNotify.szDescribeString);
			SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_MB_LOGON, SUB_MB_LOGOUT_NOTIFY, &LogoutNotify, wHeadSize + wMsgSize, &pGlobalUserItem->m_dwTokenID, 1);
		}

		//构造结构
		CMD_CS_C_ShutdownSocket ShutdownSocket = {true};		

		//关闭网络
		SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket),&pGlobalUserItem->m_dwTokenID,1);

		return true;
	}
	case SUB_CS_FILTER_WORDS_UPDATE:	//词汇更新
	{
		//提取数据
		auto pFilterWordsUpdate = (CMD_CS_FilterWordsUpdate*)pData;

		//删除判断
		if (pFilterWordsUpdate->bDeleted == false)
		{
			m_WordsFilter.InsertKeyword(pFilterWordsUpdate->szFilterWords);
		}
		else
		{
			m_WordsFilter.DeleteKeyword(pFilterWordsUpdate->szFilterWords);
		}

		return true;
	}	
	case SUB_CS_RANKING_LIST_UPDATE:	//榜单更新
	{
		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_RANKING_LIST, 0, 0, 0, NULL, 0);

		return true;
	}
	case SUB_CS_USER_WEALTH_UPDATE:		//财富更新
	{
		//校验数据
		ASSERT(wDataSize == sizeof(CMD_CS_UserWealthUpdate));
		if (wDataSize != sizeof(CMD_CS_UserWealthUpdate)) return true;

		//提取数据
		auto pUserWealthUpdate = (CMD_CS_UserWealthUpdate*)pData;

		//查找用户
		auto pGlobalUserItem = m_GlobalItemManager.SearchUserItem(pUserWealthUpdate->dwUserID);
		if (pGlobalUserItem == NULL) return true;

		//查找网关
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGlobalUserItem->GetGateID());
		if (pGlobalGateItem == NULL) return true;

		//构造结构
		DBR_MB_QueryWealth QueryWealth;
		ZeroMemory(&QueryWealth, sizeof(QueryWealth));

		//配置掩码
		QueryWealth.dwUserID = pUserWealthUpdate->dwUserID;

		//投递请求		
		DWORD dwTokenID = pGlobalUserItem->GetTokenID();
		DWORD dwSocketID = pGlobalGateItem->GetSocketID();
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_QUERY_WEALTH, 0, dwSocketID, dwTokenID, &QueryWealth, sizeof(QueryWealth));

		return true;
	}
	case SUB_CS_USER_RECHARGE_NOTICE:	//充值通知
	{
		//校验数据
		ASSERT(wDataSize == sizeof(CMD_CS_UserRechargeNotice));
		if (wDataSize != sizeof(CMD_CS_UserRechargeNotice)) return true;

		//提取数据
		auto pUserRechargeNotice = (CMD_CS_UserRechargeNotice*)pData;

		//查找用户
		auto pGlobalUserItem = m_GlobalItemManager.SearchUserItem(pUserRechargeNotice->dwUserID);
		if (pGlobalUserItem == NULL) return true;

		//查找网关
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGlobalUserItem->GetGateID());
		if (pGlobalGateItem == NULL) return true;

		//构造结构
		DBR_MB_QueryWealth QueryWealth;
		ZeroMemory(&QueryWealth, sizeof(QueryWealth));

		//配置掩码
		QueryWealth.dwUserID = pUserRechargeNotice->dwUserID;

		//网络信息
		DWORD dwTokenID = pGlobalUserItem->GetTokenID();
		DWORD dwSocketID = pGlobalGateItem->GetSocketID();		

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_QUERY_WEALTH, 0, dwSocketID, dwTokenID, &QueryWealth, sizeof(QueryWealth));

		//操作成功
		SendOperateSuccess(dwSocketID, dwTokenID,0,0,NULL, pUserRechargeNotice->szGoodsList);

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
	case SUB_CS_S_EXECUTE_COMMAND:		//执行命令
		{
			//参数校验
			ASSERT (sizeof(tagExecuteCommand)==wDataSize);
			if (sizeof(tagExecuteCommand)!=wDataSize) return false;

			//提取数据
			auto pCommandCode=(tagExecuteCommand*)pData;

			//加载配置
			if (pCommandCode->dwCommandCode== CMD_LOAD_CONFIG_DATA && pCommandCode->dwCommandParam!=0)
			{
				//配置掩码
				DBR_GP_LoadConfig LoadConfig;
				ZeroMemory(&LoadConfig, sizeof(LoadConfig));

				//设置变量
				LoadConfig.dwConfigMask = pCommandCode->dwCommandParam;

				//加载配置
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_CONFIG, 0, 0, 0, &LoadConfig, sizeof(LoadConfig));
			}			

			return true;
		}
	}

	return true;
}

//登录成功
bool CAttemperEngineSink::OnDBPCLogonSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//判断在线
	auto pBindParameter = GetBindParameter(dwContextID);
	if (pBindParameter == NULL) return true;

	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID,dwTokenID);
	if (pTokenParameter == NULL) return true;

	//变量定义
	DBO_GP_LogonSuccess * pDBOLogonSuccess=(DBO_GP_LogonSuccess *)pData;
	CMD_GP_LogonSuccess * pCMDLogonSuccess=(CMD_GP_LogonSuccess *)m_cbBuffer;

	//发送定义
	WORD wHeadSize=sizeof(CMD_GP_LogonSuccess);
	CSendPacketHelper SendPacket(m_cbBuffer+wHeadSize,sizeof(m_cbBuffer)-wHeadSize);

	//设置变量
	ZeroMemory(pCMDLogonSuccess,sizeof(CMD_GP_LogonSuccess));

	//构造数据
	pCMDLogonSuccess->cbAge=pDBOLogonSuccess->cbAge;
	pCMDLogonSuccess->wFaceID=pDBOLogonSuccess->wFaceID;
	pCMDLogonSuccess->cbGender=pDBOLogonSuccess->cbGender;
	pCMDLogonSuccess->dwGameID=pDBOLogonSuccess->dwGameID;
	pCMDLogonSuccess->dwUserID=pDBOLogonSuccess->dwUserID;
	pCMDLogonSuccess->dwCustomID=pDBOLogonSuccess->dwCustomID;
	pCMDLogonSuccess->dwProtectID = pDBOLogonSuccess->dwProtectID;
	pCMDLogonSuccess->cbMoorMachine = pDBOLogonSuccess->cbMoorMachine;
	pCMDLogonSuccess->cbMasterOrder = pDBOLogonSuccess->cbMasterOrder;
	StringCchCopy(pCMDLogonSuccess->szAccounts,CountArray(pCMDLogonSuccess->szAccounts), pDBOLogonSuccess->szAccounts);
	StringCchCopy(pCMDLogonSuccess->szPassword,CountArray(pCMDLogonSuccess->szPassword), pDBOLogonSuccess->szPassword);

	//经验等级
	pCMDLogonSuccess->dwExperience = pDBOLogonSuccess->dwExperience;

	//财富信息
	pCMDLogonSuccess->lUserGold = pDBOLogonSuccess->lUserGold;
	pCMDLogonSuccess->lInsureGold = pDBOLogonSuccess->lInsureGold;

	//会员等级
	pCMDLogonSuccess->dwMemberPoint = pDBOLogonSuccess->dwMemberPoint;
	pCMDLogonSuccess->dwMemberOverTime = pDBOLogonSuccess->dwMemberOverTime;

	//区域信息
	if ((pDBOLogonSuccess->szArea[0]!=0)||(pDBOLogonSuccess->szCity[0]!=0)||(pDBOLogonSuccess->szProvince[0]!=0))
	{
		DTP_GP_AreaInfo AreaInfo;
		ZeroMemory(&AreaInfo,sizeof(AreaInfo));
		StringCchCopy(AreaInfo.szArea,CountArray(AreaInfo.szArea), pDBOLogonSuccess->szArea);
		StringCchCopy(AreaInfo.szCity,CountArray(AreaInfo.szCity), pDBOLogonSuccess->szCity);
		StringCchCopy(AreaInfo.szProvince,CountArray(AreaInfo.szProvince), pDBOLogonSuccess->szProvince);
		SendPacket.AddPacket(&AreaInfo,sizeof(AreaInfo),DTP_GP_AREA_INFO);
	}	

	//个性签名
	if (pDBOLogonSuccess->szUnderWrite[0]!=0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szUnderWrite,CountStringBuffer(pDBOLogonSuccess->szUnderWrite),DTP_GP_UNDER_WRITE);
	}

	//用户昵称
	if(pDBOLogonSuccess->szNickName[0]!=0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szNickName,CountStringBuffer(pDBOLogonSuccess->szNickName),DTP_GP_NICKNAME);
	}

	//变量定义
	DWORD dwOnlineInfoMask=m_dwShowOnLineInfo;
	if (pCMDLogonSuccess->cbMasterOrder>0) dwOnlineInfoMask=SHOW_TOTAL_USER_COUNT|SHOW_KIND_USER_COUNT|SHOW_SERVER_USER_COUNT;

	//发送显示在线信息配置
	SendDataToGate(dwContextID,MDM_GP_LOGON,SUB_GP_SHOW_ONLINE,&dwOnlineInfoMask,sizeof(dwOnlineInfoMask),&dwTokenID,1);

	//登录成功
	WORD wSendSize=SendPacket.GetDataSize()+sizeof(CMD_GP_LogonSuccess);
	SendDataToGate(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_SUCCESS,m_cbBuffer,wSendSize,&dwTokenID,1);

	//构造结构
	CMD_CS_S_TokenParameter TokenParameter;
	ZeroMemory(&TokenParameter, sizeof(TokenParameter));

	//令牌参数
	TokenParameter.dwUserID = pDBOLogonSuccess->dwUserID;
	StringCchCopy(TokenParameter.szLogonToken, CountArray(TokenParameter.szLogonToken), pDBOLogonSuccess->szPassword);

	//发送数据
	WORD wPacketSize = CountStringBuffer(TokenParameter.szLogonToken);
	wPacketSize += sizeof(TokenParameter) - sizeof(TokenParameter.szLogonToken);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CS_COMMON_SERVICE, SUB_CS_S_TOKEN_PARAMETER, &TokenParameter, wPacketSize, &dwTokenID, 1);

	//变量定义
	bool bSendAll=(pDBOLogonSuccess->cbMasterOrder>0)||(pDBOLogonSuccess->dwCheckUserRight==UR_CAN_MANAGER_ANDROID);

	//发送列表
	SendGameTypeInfo(dwContextID,dwTokenID);

	//发送类型
	SendGameKindInfo(dwContextID,dwTokenID,bSendAll?(SUPPORT_MASK_PC|SUPPORT_MASK_MOBILE):SUPPORT_MASK_PC);

	//发送页面
	SendGamePageInfo(dwContextID,dwTokenID,0);

	//发送房间
	if (bSendAll==true) SendGameServerInfo(dwContextID,dwTokenID,0,0);

	//列表完成
	SendDataToGate(dwContextID,MDM_GP_SERVER_LIST,SUB_GP_LIST_FINISH,&dwTokenID,1);

	//登录完成
	CMD_GP_LogonFinish LogonFinish;
	ZeroMemory(&LogonFinish,sizeof(LogonFinish));
	LogonFinish.wIntermitTime=m_pInitParameter->m_wIntermitTime;
	LogonFinish.wOnLineCountTime=m_pInitParameter->m_wOnLineCountTime;
	LogonFinish.dwFunctionOption=m_dwPCFuncption;	
	SendDataToGate(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_FINISH,&LogonFinish,sizeof(LogonFinish),&dwTokenID,1);

	//////////////////////////////////////////////////////////////////////////////////////////////////

	//构造结构
	tagGlobalUserInfo GlobalUserInfo;
	ZeroMemory(&GlobalUserInfo, sizeof(GlobalUserInfo));

	//用户信息
	GlobalUserInfo.dwUserID = pDBOLogonSuccess->dwUserID;
	GlobalUserInfo.dwGameID = pDBOLogonSuccess->dwGameID;
	GlobalUserInfo.cbGender = pDBOLogonSuccess->cbGender;
	GlobalUserInfo.wFaceID = pDBOLogonSuccess->wFaceID;
	GlobalUserInfo.dwCustomID = pDBOLogonSuccess->dwCustomID;	
	StringCchCopy(GlobalUserInfo.szNickName, CountArray(GlobalUserInfo.szNickName), pDBOLogonSuccess->szNickName);

	//网络信息
	GlobalUserInfo.SocketInfo.wGateID = pBindParameter->wServiceID;
	GlobalUserInfo.SocketInfo.dwTokenID = pTokenParameter->dwTokenID;

	//激活用户
	auto pGlobalUserItem = m_GlobalItemManager.ActiveUserItem(GlobalUserInfo);
	if (pGlobalUserItem != NULL)
	{
		//收集用户
		CMD_CS_C_UserOnline  UserOnline;
		ZeroMemory(&UserOnline, sizeof(UserOnline));

		//设置变量
		UserOnline.dwUserID = pDBOLogonSuccess->dwUserID;
		UserOnline.dwGameID = pDBOLogonSuccess->dwGameID;
		StringCchCopy(UserOnline.szNickName, CountArray(UserOnline.szNickName), pDBOLogonSuccess->szNickName);
		//StringCchCopy(UserOnline.szUserHeadUrl, CountArray(UserOnline.szUserHeadUrl), pDBOLogonSuccess->szUserFaceUrl);//kk jia

		//用户资料
		UserOnline.cbGender = pDBOLogonSuccess->cbGender;
		UserOnline.wFaceID = pDBOLogonSuccess->wFaceID;
		UserOnline.dwCustomID = pDBOLogonSuccess->dwCustomID;

		//网络信息
		UserOnline.wGateID = pBindParameter->wServiceID;
		UserOnline.dwTokenID = pTokenParameter->dwTokenID;		

		//用户上线
		m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_USER_ONLINE, &UserOnline, sizeof(UserOnline), 0);
	}

	//绑定用户
	pTokenParameter->pBindUserItem = pGlobalUserItem;
	pTokenParameter->dwUserID = pGlobalUserItem->GetUserID();

	//重入处理
	if (pDBOLogonSuccess->wLockGateID != 0 && pDBOLogonSuccess->wLockLogonID != 0 && pDBOLogonSuccess->dwLockSocketID != 0)
	{
		//查找网关
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pDBOLogonSuccess->wLockGateID);
		if (pGlobalGateItem != NULL)
		{
			//构造结构
			CMD_GP_LogoutNotify LogoutNotify;
			ZeroMemory(&LogoutNotify, sizeof(LogoutNotify));

			//设置变量
			StringCchCopy(LogoutNotify.szDescribeString, CountArray(LogoutNotify.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0007, TEXT("由于您的账号在别处登录，您被迫下线！")));

			//发送数据
			WORD wMsgSize = CountStringBuffer(LogoutNotify.szDescribeString);
			WORD wHeadSize = sizeof(LogoutNotify) - sizeof(LogoutNotify.szDescribeString);
			SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_GP_LOGON, SUB_GP_LOGOUT_NOTIFY, &LogoutNotify, wHeadSize + wMsgSize, &pDBOLogonSuccess->dwLockSocketID, 1);

			//移除令牌
			if (pDBOLogonSuccess->wLockLogonID == m_pServiceOption->wServiceID)
			{
				pGlobalGateItem->RemoveTokenID(pDBOLogonSuccess->dwLockSocketID);
			}

			//构造结构
			CMD_CS_C_ShutdownSocket ShutdownSocket = { true };

			//关闭网络
			SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket), &pDBOLogonSuccess->dwLockSocketID, 1);
		}
	}

	return true;
}

//登录失败
bool CAttemperEngineSink::OnDBPCLogonFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//变量定义
	CMD_GP_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));
	DBO_GP_LogonFailure * pLogonFailure=(DBO_GP_LogonFailure *)pData;

	//构造数据
	LogonFailure.lErrorCode=pLogonFailure->lErrorCode;
	StringCchCopy(LogonFailure.szDescribeString,CountArray(LogonFailure.szDescribeString), pLogonFailure->szDescribeString);

	//发送数据
	WORD wStringSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wSendSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString)+wStringSize;
	SendDataToGate(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_FAILURE,&LogonFailure,wSendSize,&dwTokenID,1);

	//构造结构
	CMD_CS_C_ShutdownSocket ShutdownSocket;
	ShutdownSocket.bForceClose = false;
	
	//关闭连接
	SendDataToGate(dwContextID,MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket),&dwTokenID,1);

	return true;
}

//用户信息
bool CAttemperEngineSink::OnDBPCUserIndividual(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//变量定义
	DBO_GP_UserIndividual * pDBOUserIndividual=(DBO_GP_UserIndividual *)pData;
	CMD_GP_UserIndividual * pCMDUserIndividual=(CMD_GP_UserIndividual *)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer+sizeof(CMD_GP_UserIndividual),sizeof(m_cbBuffer)-sizeof(CMD_GP_UserIndividual));

	//设置变量
	ZeroMemory(pCMDUserIndividual,sizeof(CMD_GP_UserIndividual));

	//构造数据
	pCMDUserIndividual->dwUserID=pDBOUserIndividual->dwUserID;

	//用户信息
	if (pDBOUserIndividual->szUserNote[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szUserNote);
		SendPacket.AddPacket(pDBOUserIndividual->szUserNote,wBufferSize,DTP_GP_UI_USER_NOTE);
	}

	//真实姓名
	if (pDBOUserIndividual->szCompellation[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szCompellation);
		SendPacket.AddPacket(pDBOUserIndividual->szCompellation,wBufferSize,DTP_GP_UI_COMPELLATION);
	}

	//电话号码
	if (pDBOUserIndividual->szSeatPhone[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szSeatPhone);
		SendPacket.AddPacket(pDBOUserIndividual->szSeatPhone,wBufferSize,DTP_GP_UI_SEAT_PHONE);
	}

	//移动电话
	if (pDBOUserIndividual->szMobilePhone[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szMobilePhone);
		SendPacket.AddPacket(pDBOUserIndividual->szMobilePhone,wBufferSize,DTP_GP_UI_MOBILE_PHONE);
	}

	//联系资料
	if (pDBOUserIndividual->szQQ[0]!=0) 
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szQQ);
		SendPacket.AddPacket(pDBOUserIndividual->szQQ,wBufferSize,DTP_GP_UI_QQ);
	}

	//电子邮件
	if (pDBOUserIndividual->szEMail[0]!=0) 
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szEMail);
		SendPacket.AddPacket(pDBOUserIndividual->szEMail,wBufferSize,DTP_GP_UI_EMAIL);
	}

	//联系地址
	if (pDBOUserIndividual->szDwellingPlace[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szDwellingPlace);
		SendPacket.AddPacket(pDBOUserIndividual->szDwellingPlace,wBufferSize,DTP_GP_UI_DWELLING_PLACE);
	}

	//发送消息
	WORD wSendSize=sizeof(CMD_GP_UserIndividual)+SendPacket.GetDataSize();
	SendDataToGate(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_USER_INDIVIDUAL,m_cbBuffer,wSendSize,&dwTokenID,1);

	return true;
}

//操作成功
bool CAttemperEngineSink::OnDBPCOperateSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//电脑类型
	if (pExtendParameter->cbClientKind == CLIENT_KIND_COMPUTER)
	{
		//提取数据
		DBO_GP_OperateSuccess* pOperateSuccess = (DBO_GP_OperateSuccess*)pData;

		//变量定义
		CMD_GP_OperateSuccess OperateSuccess;
		ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));		

		//构造数据
		OperateSuccess.lResultCode = pOperateSuccess->lResultCode;
		StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString),pOperateSuccess->szDescribeString);

		//发送数据
		WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
		WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
		SendDataToGate(dwContextID, MDM_GP_USER_SERVICE, SUB_GP_OPERATE_SUCCESS, &OperateSuccess, wSendSize,&dwTokenID,1);
	}

	return true;
}

//操作失败
bool CAttemperEngineSink::OnDBPCOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//电脑类型
	if (pExtendParameter->cbClientKind == CLIENT_KIND_COMPUTER)
	{
		//变量定义
		CMD_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));
		DBO_GP_OperateFailure* pOperateFailure = (DBO_GP_OperateFailure*)pData;

		//构造数据
		OperateFailure.lErrorCode = pOperateFailure->lErrorCode;
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pOperateFailure->szDescribeString);

		//发送数据
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		SendDataToGate(dwContextID, MDM_GP_USER_SERVICE, SUB_GP_OPERATE_FAILURE, &OperateFailure, wSendSize,&dwTokenID,1);
	}

	return true;
}

//基础配置
bool CAttemperEngineSink::OnDBPCGameBasicConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//提取数据
	auto pBasicConfig = (DBO_GP_GameBasicConfig*)pData;

	//构造结构
	CMD_GP_GameBasicConfig BasicConfig = {};

	//拷贝数据
	StringCchCopy(BasicConfig.szBasicConfig,CountArray(BasicConfig.szBasicConfig), pBasicConfig->szBasicConfig);

	//发送数据
	WORD wSendSize = CountStringBuffer(BasicConfig.szBasicConfig);
	wSendSize += sizeof(BasicConfig) - sizeof(BasicConfig.szBasicConfig);
	SendDataToGate(dwContextID, MDM_GP_CONFIG_SERVICE, SUB_GP_GAME_BASIC_CONFIG, &BasicConfig, wSendSize, &dwTokenID, 1);

	return true;
}

//权重配置
bool CAttemperEngineSink::OnDBPCGameWeightConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//提取数据
	auto pWeightConfig = (DBO_GP_GameWeightConfig*)pData;

	//构造结构
	CMD_GP_GameWeightConfig WeightConfig = {};

	//拷贝数据
	WeightConfig.wItemCount = pWeightConfig->wItemCount;
	CopyMemory(WeightConfig.TimesWeightItem, pWeightConfig->TimesWeightItem,sizeof(WeightConfig.TimesWeightItem));

	//发送数据
	WORD wSendSize = sizeof(WeightConfig) - sizeof(WeightConfig.TimesWeightItem);
	wSendSize += WeightConfig.wItemCount * sizeof(WeightConfig.TimesWeightItem[0]);
	SendDataToGate(dwContextID, MDM_GP_CONFIG_SERVICE, SUB_GP_GAME_WEIGHT_CONFIG, &WeightConfig, wSendSize, &dwTokenID, 1);

	return true;
}

//配置完成
bool CAttemperEngineSink::OnDBPCGameConfigFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//发送数据
	SendDataToGate(dwContextID, MDM_GP_CONFIG_SERVICE, SUB_GP_GAME_CONFIG_FINISH, NULL, 0, &dwTokenID, 1);

	return true;
}

//机器操作
bool CAttemperEngineSink::OnDBPCRobotOperateResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//变量定义
	auto pRobotOperateResult = (DBO_GP_RobotOperateResult*)pData;

	//变量定义
	CMD_GP_RobotOperateResult RobotOperateResult;
	ZeroMemory(&RobotOperateResult, sizeof(RobotOperateResult));

	//构造变量
	RobotOperateResult.wSubCommdID = pRobotOperateResult->wSubCommdID;
	RobotOperateResult.wParameterCount = pRobotOperateResult->wParameterCount;
	CopyMemory(RobotOperateResult.RobotParameter, pRobotOperateResult->RobotParameter, sizeof(tagRobotParameter) * RobotOperateResult.wParameterCount);

	//计算大小
	WORD wPacketSize= sizeof(RobotOperateResult) - sizeof(RobotOperateResult.RobotParameter);
	wPacketSize += sizeof(tagRobotParameter) * RobotOperateResult.wParameterCount;

	//发送数据
	SendDataToGate(dwContextID, MDM_GP_ROBOT_SERVICE, SUB_GP_ROBOT_OPERATE_RESULT, &RobotOperateResult, wPacketSize,&dwTokenID,1);

	//数量判断
	if (RobotOperateResult.wParameterCount == 1)
	{
		//缓冲定义
		WORD wBufferSize = 0;
		BYTE cbBuffer[1024] = {0};		

		//构造结构
		tagForwardHead* pForwadHead = (tagForwardHead*)cbBuffer;
		
		//转发设置
		pForwadHead->wItemCount = 1;
		pForwadHead->wTarget = FORWARD_TARGET_SERVICE;
		pForwadHead->ForwardItem[0].wServiceID = pRobotOperateResult->wServerID;
		pForwadHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_GAME;

		//添加参数
		if (RobotOperateResult.wSubCommdID == SUB_GP_APPEND_PARAMETER)
		{
			//构造结构
			auto pAppendParameter= (CMD_CS_AppendRobotParameter*)(pForwadHead+1);

			//设置变量
			CopyMemory(&pAppendParameter->RobotParameter, &RobotOperateResult.RobotParameter[0], sizeof(tagRobotParameter));

			//发送数据
			wBufferSize = sizeof(tagForwardHead) + sizeof(CMD_CS_AppendRobotParameter);
			m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_APPEND_ROBOT_PARAMETER, cbBuffer, wBufferSize, dwTokenID);
		}

		//修改参数
		if (RobotOperateResult.wSubCommdID == SUB_GP_MODIFY_PARAMETER)
		{
			//构造结构
			auto pModifyParameter = (CMD_CS_ModifyRobotParameter*)(pForwadHead + 1);

			//拷贝参数
			CopyMemory(&pModifyParameter->RobotParameter, &RobotOperateResult.RobotParameter[0], sizeof(tagRobotParameter));

			//发送数据
			wBufferSize = sizeof(tagForwardHead) + sizeof(CMD_CS_ModifyRobotParameter);
			m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_MODIFY_ROBOT_PARAMETER, cbBuffer, wBufferSize, dwTokenID);
		}

		//删除参数
		if (RobotOperateResult.wSubCommdID == SUB_GP_DELETE_PARAMETER)
		{
			//构造结构
			auto pDeleteParameter = (CMD_CS_DeleteRobotParameter*)(pForwadHead + 1);

			//构造结构
			pDeleteParameter->dwBatchID = RobotOperateResult.RobotParameter[0].dwBatchID;

			//发送数据
			wBufferSize = sizeof(tagForwardHead) + sizeof(CMD_CS_DeleteRobotParameter);
			m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_DELETE_ROBOT_PARAMETER, cbBuffer, wBufferSize, dwTokenID);
		}
	}

	return true;
}

//操作结果
bool CAttemperEngineSink::OnDBPCStockOperateResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//变量定义
	auto pStockOperateResult = (DBO_GP_StockOperateResult*)pData;

	//变量定义
	CMD_GP_StockOperateResult StockOperateResult;
	ZeroMemory(&StockOperateResult, sizeof(StockOperateResult));

	//构造变量
	StockOperateResult.wPacketIdx = pStockOperateResult->wPacketIdx;
	StockOperateResult.wSubCommdID = pStockOperateResult->wSubCommdID;
	StockOperateResult.wStockCount = pStockOperateResult->wStockCount;
	CopyMemory(StockOperateResult.StockList, pStockOperateResult->StockList, sizeof(StockOperateResult.StockList[0]) * StockOperateResult.wStockCount);

	//计算大小
	WORD wPacketSize = sizeof(StockOperateResult) - sizeof(StockOperateResult.StockList);
	wPacketSize += sizeof(StockOperateResult.StockList[0]) * StockOperateResult.wStockCount;

	//发送数据
	SendDataToGate(dwContextID, MDM_GP_STOCK_SERVICE, SUB_GP_STOCK_OPERATE_RESULT, &StockOperateResult, wPacketSize, &dwTokenID, 1);

	//数量判断
	if (StockOperateResult.wStockCount == 1)
	{
		//缓冲定义
		WORD wBufferSize = 0;
		BYTE cbBuffer[1024] = { 0 };

		//构造结构
		tagForwardHead* pForwadHead = (tagForwardHead*)cbBuffer;

		//转发设置
		pForwadHead->wItemCount = 1;
		pForwadHead->wTarget = FORWARD_TARGET_SERVICE;
		pForwadHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MONITOR;

		//添加库存
		if (StockOperateResult.wSubCommdID == SUB_GP_APPEND_STOCK_INFO)
		{
			//构造结构
			auto pAppendStockItem = (CMD_CS_AppendStockItem*)(pForwadHead + 1);

			//设置变量
			pAppendStockItem->szOperateID[0] = 0;
			pAppendStockItem->wStockID = StockOperateResult.StockList[0].wStockID;			

			//发送数据
			wBufferSize = sizeof(tagForwardHead) + sizeof(CMD_CS_AppendStockItem);
			m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_APPEND_STOCK_ITEM, cbBuffer, wBufferSize, dwTokenID);
		}

		//修改库存
		if (StockOperateResult.wSubCommdID == SUB_GP_MODIFY_STOCK_INFO)
		{
			//构造结构
			auto pModifyStockItem = (CMD_CS_ModifyStockItem*)(pForwadHead + 1);

			//设置变量
			pModifyStockItem->szOperateID[0] = 0;
			pModifyStockItem->wStockID = StockOperateResult.StockList[0].wStockID;

			//发送数据
			wBufferSize = sizeof(tagForwardHead) + sizeof(CMD_CS_ModifyStockItem);
			m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_MODIFY_STOCK_ITEM, cbBuffer, wBufferSize, dwTokenID);
		}

		//删除库存
		if (StockOperateResult.wSubCommdID == SUB_GP_DELETE_STOCK_INFO)
		{
			//构造结构
			auto pDeleteStockItem = (CMD_CS_DeleteStockItem*)(pForwadHead + 1);

			//构造结构
			pDeleteStockItem->wStockID = StockOperateResult.StockList[0].wStockID;

			//发送数据
			wBufferSize = sizeof(tagForwardHead) + sizeof(CMD_CS_DeleteStockItem);
			m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_DELETE_STOCK_ITEM, cbBuffer, wBufferSize, dwTokenID);
		}
	}

	return true;
}

//游戏种类
bool CAttemperEngineSink::OnDBGameTypeItem(DWORD dwContextID,DWORD dwTokenID,VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(DBO_GP_GameType)==0);
	if (wDataSize%sizeof(DBO_GP_GameType)!=0) return false;

	//变量定义
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GameType);
	DBO_GP_GameType * pGameType=(DBO_GP_GameType *)pData;

	//更新数据
	for (WORD i=0;i<wItemCount;i++)
	{
		//变量定义
		tagGameType GameType;
		ZeroMemory(&GameType,sizeof(GameType));

		//构造数据
		GameType.wTypeID=(pGameType+i)->wTypeID;
		GameType.wJoinID=(pGameType+i)->wJoinID;
		GameType.wSortID=(pGameType+i)->wSortID;
		StringCchCopy(GameType.szTypeName,CountArray(GameType.szTypeName), (pGameType + i)->szTypeName);

		//插入列表
		m_ServerListManager.InsertGameType(&GameType);
	}

	return true;
}

//游戏类型
bool CAttemperEngineSink::OnDBGameKindItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(DBO_GP_GameKind)==0);
	if (wDataSize%sizeof(DBO_GP_GameKind)!=0) return false;

	//变量定义
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GameKind);
	DBO_GP_GameKind * pGameKind=(DBO_GP_GameKind *)pData;

	//更新数据
	for (WORD i=0;i<wItemCount;i++)
	{
		//变量定义
		tagGameKindEx GameKind;
		ZeroMemory(&GameKind,sizeof(GameKind));

		//构造数据
		GameKind.wModuleID = (pGameKind+i)->wModuleID;
		GameKind.wTypeID=(pGameKind+i)->wTypeID;
		GameKind.wJoinID=(pGameKind+i)->wJoinID;
		GameKind.wSortID=(pGameKind+i)->wSortID;
		GameKind.wKindID=(pGameKind+i)->wKindID;
		GameKind.dwOnLineCount=m_ServerListManager.CollectOnlineInfo((pGameKind+i)->wKindID);
		StringCchCopy(GameKind.szKindName,CountArray(GameKind.szKindName), (pGameKind + i)->szKindName);
		StringCchCopy(GameKind.szProcessName,CountArray(GameKind.szProcessName), (pGameKind + i)->szProcessName);

		//支持掩码
		GameKind.wSupportMask=(pGameKind+i)->wSupportMask;

		//插入列表
		if ((pGameKind + i)->bNullity == false)
		{
			m_ServerListManager.InsertGameKind(&GameKind);
		}		
		else
		{
			m_ServerListManager.DeleteGameKind(GameKind.wKindID);
		}
	}

	return true;
}

//游戏节点
bool CAttemperEngineSink::OnDBGameNodeItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(DBO_GP_GameNode)==0);
	if (wDataSize%sizeof(DBO_GP_GameNode)!=0) return false;

	//变量定义
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GameNode);
	DBO_GP_GameNode * pGameNode=(DBO_GP_GameNode *)pData;

	//更新数据
	for (WORD i=0;i<wItemCount;i++)
	{
		//变量定义
		tagGameNode GameNode;
		ZeroMemory(&GameNode,sizeof(GameNode));

		//构造数据
		GameNode.wKindID=(pGameNode+i)->wKindID;
		GameNode.wJoinID=(pGameNode+i)->wJoinID;
		GameNode.wSortID=(pGameNode+i)->wSortID;
		GameNode.wNodeID=(pGameNode+i)->wNodeID;
		StringCchCopy(GameNode.szNodeName,CountArray(GameNode.szNodeName), (pGameNode + i)->szNodeName);

		//插入列表
		m_ServerListManager.InsertGameNode(&GameNode);
	}

	return true;
}

//游戏定制
bool CAttemperEngineSink::OnDBGamePageItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(DBO_GP_GameCustom)==0);
	if (wDataSize%sizeof(DBO_GP_GameCustom)!=0) return false;

	//变量定义
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GameCustom);
	DBO_GP_GameCustom * pGameCustom=(DBO_GP_GameCustom *)pData;

	//更新数据
	for (WORD i=0;i<wItemCount;i++)
	{
		//变量定义
		tagGameCustom GameCustom;
		ZeroMemory(&GameCustom,sizeof(GameCustom));

		//构造数据
		GameCustom.wKindID=(pGameCustom+i)->wKindID;
		GameCustom.wNodeID=(pGameCustom+i)->wNodeID;
		GameCustom.wSortID=(pGameCustom+i)->wSortID;
		GameCustom.wPageID=(pGameCustom+i)->wPageID;
		GameCustom.wOperateType=(pGameCustom+i)->wOperateType;
		StringCchCopy(GameCustom.szDisplayName,CountArray(GameCustom.szDisplayName), (pGameCustom + i)->szDisplayName);
		StringCchCopy(GameCustom.szReponseUrl,CountArray(GameCustom.szReponseUrl), (pGameCustom + i)->szReponseUrl);

		//插入列表
		m_ServerListManager.InsertGameCustom(&GameCustom);
	}

	return true;
}

//游戏过滤
bool CAttemperEngineSink::OnDBGameFilterItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(DBO_GP_GameFilter)==0);
	if (wDataSize%sizeof(DBO_GP_GameFilter)!=0) return false;

	//变量定义
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GameFilter);
	DBO_GP_GameFilter * pGameFilter=(DBO_GP_GameFilter *)pData;

	//更新数据
	for (WORD i=0;i<wItemCount;i++)
	{
		//变量定义
		tagGameFilter GameFilter;
		ZeroMemory(&GameFilter,sizeof(GameFilter));

		//构造数据
		GameFilter.wSortID=(pGameFilter+i)->wSortID;
		GameFilter.wNodeID=(pGameFilter+i)->wNodeID;
		GameFilter.wKindID=(pGameFilter+i)->wKindID;
		GameFilter.wServerID=(pGameFilter+i)->wServerID;
		GameFilter.wFilterMask=(pGameFilter+i)->wFilterMask;
		StringCchCopy(GameFilter.szServerName,CountArray(GameFilter.szServerName), (pGameFilter + i)->szServerName);

		//插入列表
		m_ServerListManager.InsertGameFilter(&GameFilter);
	}

	return true;
}

//配置结果
bool CAttemperEngineSink::OnDBConfigResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(DBO_GP_ConfigResult));
	if (wDataSize != sizeof(DBO_GP_ConfigResult)) return false;

	//提取数据
	DBO_GP_ConfigResult* pConfigResult = (DBO_GP_ConfigResult*)pData;

	//构造结构
	CP_ControlResult ControlResult;
	ZeroMemory(&ControlResult, sizeof(ControlResult));

	//数据处理
	if (pConfigResult->bResultCode == true)
	{
		//事件通知
		ControlResult.cbSuccess = ER_SUCCESS;

		//切换配置
		if (pConfigResult->dwConfigMask & GLOBAL_OPTION_MASK)
		{
			g_GlobalOptionManager->SwitchOptionItem(pConfigResult->dwConfigMask);
		}
	}
	else
	{
		ControlResult.cbSuccess = ER_FAILURE;
	}

	//事件通知
	if (pConfigResult->bInitService == true)
	{
		SendUIControlPacket(UI_LOAD_CONFIG_RESULT, &ControlResult, sizeof(ControlResult));
	}

	return true;
}

//全局配置
bool CAttemperEngineSink::OnDBGlobalOption(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	return true;
}

//敏感词信息
bool CAttemperEngineSink::OnDBFilterWordsInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	if (dwContextID == 0xffff)			//加载完成
	{
		return true;
	}
	else if (dwContextID == 0xfffe)		//开始加载
	{
		m_WordsFilter.ResetFilter();
		return true;
	}

	const TCHAR* pWords = (const TCHAR*)pData;
	m_WordsFilter.InsertKeyword(pWords);

	return true;
}

//榜单数据
bool CAttemperEngineSink::OnDBRankingListData(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pRankingListData = (DBO_GP_RankingListData*)pData;

	//添加榜单
	m_RankingListManager.AppendRankingList(pRankingListData->cbListType, CA2CT(pRankingListData->szRankingList).m_psz);

	return true;
}

//转账参数
bool CAttemperEngineSink::OnDBTransferParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize==sizeof(tagTransferParameter));
	if (wDataSize != sizeof(tagTransferParameter)) return true;

	//拷贝配置
	CopyMemory(&m_TransferParameter, pData, wDataSize);

	return true;
}

//版本信息
bool CAttemperEngineSink::OnDBModuleVersionInfo(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//提取数据
	ASSERT (wDataSize%sizeof(tagModuleVersion)==0);
	if (wDataSize%sizeof(tagModuleVersion)!=0) return false;

	//变量定义
	WORD wItemCount=wDataSize/sizeof(tagModuleVersion);
	tagModuleVersion * pModuleVersion=(tagModuleVersion *)pData;

	//移除版本
	m_ModuleVersionActive.RemoveAll();

	//变量定义
	for (int i=0; i<wItemCount; i++)
	{
		m_ModuleVersionActive.Add(*pModuleVersion++);
	}

	return true;
}

//绑定机器
bool CAttemperEngineSink::OnDBBindMachineResult(DWORD dwContextID,DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//绑定参数
	tagBindParameter* pBindParameter = GetBindParameter(dwContextID);
	if (pBindParameter == NULL) return true;

	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//变量定义
	DBO_GP_BindMachineResult * pBindMachineResult=(DBO_GP_BindMachineResult *)pData;

	//效验参数
	ASSERT(wDataSize==sizeof(DBO_GP_BindMachineResult)-sizeof(pBindMachineResult->szDescribeString)+CountStringBuffer(pBindMachineResult->szDescribeString));
	if( wDataSize != sizeof(DBO_GP_BindMachineResult)-sizeof(pBindMachineResult->szDescribeString)+CountStringBuffer(pBindMachineResult->szDescribeString) )
		return false;

	//判断在线
	ASSERT(LOWORD(dwContextID)< m_pServiceOption->wMaxConnect);
	if (pBindParameter->dwSocketID!=dwContextID) return true;

	if( pBindMachineResult->lResultCode == DB_SUCCESS )
	{
		//变量定义
		CMD_GP_OperateSuccess OperateSuccess;
		ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

		//构造数据
		OperateSuccess.lResultCode=pBindMachineResult->lResultCode;
		StringCchCopy(OperateSuccess.szDescribeString,CountArray(OperateSuccess.szDescribeString), pBindMachineResult->szDescribeString);

		//发送数据
		WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
		WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
		SendDataToGate(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_OPERATE_SUCCESS,&OperateSuccess,wSendSize,&dwTokenID,1);
	}
	else
	{
		//变量定义
		CMD_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//构造数据
		OperateFailure.lErrorCode=pBindMachineResult->lResultCode;
		StringCchCopy(OperateFailure.szDescribeString,CountArray(OperateFailure.szDescribeString), pBindMachineResult->szDescribeString);

		//发送数据
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		SendDataToGate(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_OPERATE_FAILURE,&OperateFailure,wSendSize,&dwTokenID,1);
	}

	return true;
}

//跑马灯公告
bool CAttemperEngineSink::SendMarqeeNotice(BYTE cbNoticeKind, VOID* pNoticeContent, WORD wNoticeSize)
{
	//查找配置
	auto MarqueeNoticeOptionPtr = g_GlobalOptionManager->GetOptionItem<CMarqueeNoticeOptionPtr>(OPTION_MASK_MARQUEE_NOTICE);
	if (!MarqueeNoticeOptionPtr) return false;

	//滚动配置
	auto pNoticeRollOption = MarqueeNoticeOptionPtr->GetNoticeRollOption(cbNoticeKind);
	if (!pNoticeRollOption) return false;

	//缓冲定义
	BYTE cbBuffer[1024];

	//构造结构
	tagForwardHead* pForwadHead = (tagForwardHead*)cbBuffer;
	auto pMarqueeNotice = (CMD_CS_MarqueeNotice*)(pForwadHead + 1);

	//转发设置
	pForwadHead->wItemCount = 1;
	pForwadHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwadHead->ForwardItem[0].wServiceID = 0;
	pForwadHead->ForwardItem[0].wServiceKind = 0;
	pForwadHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;

	//设置数据
	pMarqueeNotice->dwNoticeID = GetTickCount();
	pMarqueeNotice->bExclusive = pNoticeRollOption->bExclusive;
	pMarqueeNotice->cbPriority = pNoticeRollOption->cbPriority;
	pMarqueeNotice->wRollTimes = pNoticeRollOption->wRollTimes;
	pMarqueeNotice->wViewPlace = pNoticeRollOption->wViewPlace;
	pMarqueeNotice->cbNoticeKind = pNoticeRollOption->cbNoticeKind;
	pMarqueeNotice->dwStartRollTime = (DWORD)time(NULL);
	pMarqueeNotice->dwEndRollTime = pMarqueeNotice->dwStartRollTime + MarqueeNoticeOptionPtr->GetDurationTime();

	//公告内容
	pMarqueeNotice->wNoticeSize = wNoticeSize;
	CopyMemory(pMarqueeNotice->cbNoticeContent, pNoticeContent, pMarqueeNotice->wNoticeSize);

	//发送数据
	WORD wPacketSize = sizeof(CMD_CS_MarqueeNotice) - sizeof(pMarqueeNotice->cbNoticeContent) + pMarqueeNotice->wNoticeSize;
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_MARQUEE_NOTICE, cbBuffer, sizeof(tagForwardHead) + wPacketSize,0);

	return true;
}

//版本检测
bool CAttemperEngineSink::CheckPlazaVersion(DWORD dwDeviceKind, DWORD dwPlazaVersion, DWORD dwSocketID, DWORD dwTokenID)
{
	//版本判断
	bool bMustUpdate=false,bAdviceUpdate=false,bSpecialVersion=false;
	
	//变量定义
	DWORD dwClientVersion=0;
	tagModuleVersion * pModuleVersion=NULL;		

	//特殊版本
	pModuleVersion=GetSpecialVersion(0,dwDeviceKind,dwPlazaVersion);
	if (pModuleVersion!=NULL) bSpecialVersion=true;

	//当前版本
	pModuleVersion=GetCurrentVersion(0,dwDeviceKind);
	if (pModuleVersion!=NULL) dwClientVersion=pModuleVersion->dwVersion;

	//校验版本
	if (!bSpecialVersion) 
	{
		//判断版本
		if (GetSubVer(dwPlazaVersion)<GetSubVer(dwClientVersion)) bAdviceUpdate=true;
		if (GetMainVer(dwPlazaVersion)!=GetMainVer(dwClientVersion)) bMustUpdate=true;
		if (GetProductVer(dwPlazaVersion)!=GetProductVer(dwClientVersion)) bMustUpdate=true;
	}

	//升级提示
	if ((bMustUpdate==true)||(bAdviceUpdate==true))
	{
		//变量定义
		CMD_GP_UpdateNotify UpdateNotify;
		ZeroMemory(&UpdateNotify,sizeof(UpdateNotify));

		//变量定义
		UpdateNotify.cbMustUpdate=bMustUpdate;
		UpdateNotify.cbAdviceUpdate=bAdviceUpdate;
		UpdateNotify.dwCurrentVersion=dwClientVersion;

		//发送消息
		if (bMustUpdate || (bAdviceUpdate && !m_bUpdateSilent))
		{
			SendDataToGate(dwSocketID,MDM_GP_LOGON,SUB_GP_UPDATE_NOTIFY,&UpdateNotify,sizeof(UpdateNotify),&dwTokenID,1);
		}
		else
		{
			SendDataToGate(dwSocketID,MDM_GP_LOGON,SUB_GP_UPDATE_SILENT,&UpdateNotify,sizeof(UpdateNotify),&dwTokenID,1);
		}

		//中断判断
		if (bMustUpdate==true) 
		{
			//构造结构
			CMD_CS_C_ShutdownSocket ShutdownSocket;
			ShutdownSocket.bForceClose = false;

			//关闭连接
			SendDataToGate(dwSocketID,MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket),&dwTokenID,1);

			return false;
		}
	}

	return true;
}

//操作失败
VOID CAttemperEngineSink::SendOperateFailure(DWORD dwSocketID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe)
{
	//变量定义
	CMD_CM_OperateFailure * pRequestFailure = (CMD_CM_OperateFailure*)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer + sizeof(CMD_CM_OperateFailure), sizeof(m_cbBuffer) - sizeof(CMD_CM_OperateFailure));

	//设置变量
	pRequestFailure->lErrorCode = lErrorCode;
	pRequestFailure->wRequestCmdID = wRequestCmdID;	

	//叠加信息
	if (pszDescribe != NULL)  SendPacket.AddPacket(pszDescribe, DTP_CM_DESCRIBE);

	//发送完成
	WORD wDataSize = sizeof(CMD_CM_OperateFailure) + SendPacket.GetDataSize();
	SendDataToGate(dwSocketID, MDM_CM_SYSTEM, SUB_CM_OPERATE_FAILURE, pRequestFailure, wDataSize,&dwTokenID,1);

	return;
}

//操作成功
VOID CAttemperEngineSink::SendOperateSuccess(DWORD dwSocketID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe, LPCTSTR pszGoodsList)
{
	//变量定义
	CMD_CM_OperateSuccess* pRequestSuccess = (CMD_CM_OperateSuccess*)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer + sizeof(CMD_CM_OperateSuccess), sizeof(m_cbBuffer) - sizeof(CMD_CM_OperateSuccess));
	
	//设置变量
	pRequestSuccess->lErrorCode = lErrorCode;
	pRequestSuccess->wRequestCmdID = wRequestCmdID;
	
	//叠加信息
	if (pszDescribe != NULL)  SendPacket.AddPacket(pszDescribe, DTP_CM_DESCRIBE);
	if (pszGoodsList != NULL)  SendPacket.AddPacket(pszGoodsList, DTP_CM_GOODSLIST);

	//发送完成
	WORD wDataSize = sizeof(CMD_CM_OperateSuccess) + SendPacket.GetDataSize();
	SendDataToGate(dwSocketID, MDM_CM_SYSTEM, SUB_CM_OPERATE_SUCCESS, pRequestSuccess, wDataSize,&dwTokenID,1);

	return;
}

//发送请求
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//发送数据
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->m_UIControlSink.PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}


//日期切换
VOID CAttemperEngineSink::OnEventDaySwitch()
{
	//获取时间
	SYSTEMTIME CurrSystemTime;
	GetLocalTime(&CurrSystemTime);

	//变量定义
	CDWordArray UserIDList;
	CString strUserIDList = TEXT("[");

	//变量定义		
	CGlobalUserItem* pGlobalUserItem = NULL;
	pGlobalUserItem = m_GlobalItemManager.EnumUserItem(NULL);

	//变量定义
	DBR_GP_ResetUserData ResetUserData;
	ZeroMemory(&ResetUserData, sizeof(ResetUserData));

	//发送数据
	while (pGlobalUserItem != NULL)
	{
		//添加用户		
		UserIDList.Add(pGlobalUserItem->GetUserID());

		if (strUserIDList != TEXT("[")) strUserIDList.AppendChar(TEXT(','));
		strUserIDList.AppendFormat(TEXT("%d"), pGlobalUserItem->GetUserID());

		//获取对象
		CGlobalUserItem* pGlobalUserCurrent = pGlobalUserItem;
		pGlobalUserItem = m_GlobalItemManager.EnumUserItem(pGlobalUserCurrent);		

		//更新任务
		if (strUserIDList.GetLength()+30 >= CountArray(ResetUserData.szUserIDList))
		{
			//添加回括号
			strUserIDList.Append(TEXT("]"));

			//拷贝数据
			ResetUserData.wDayOfWeek = CurrSystemTime.wDayOfWeek;
			StringCchCopy(ResetUserData.szUserIDList, CountArray(ResetUserData.szUserIDList), strUserIDList);

			//投递数据
			WORD wDataSize = CountStringBuffer(ResetUserData.szUserIDList);
			WORD wHeadSize = sizeof(ResetUserData) - sizeof(ResetUserData.szUserIDList);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_RESET_USER_DATA, 0L, 0L, 0L, &ResetUserData, wHeadSize + wDataSize);

			//设置变量
			strUserIDList = TEXT("[");
		}
	}

	//发送剩余
	if (strUserIDList!=TEXT("["))
	{
		//添加回括号
		strUserIDList.Append(TEXT("]"));
		
		//变量定义
		DBR_GP_ResetUserData ResetUserData;
		ZeroMemory(&ResetUserData, sizeof(ResetUserData));

		//拷贝数据
		ResetUserData.wDayOfWeek = CurrSystemTime.wDayOfWeek;
		StringCchCopy(ResetUserData.szUserIDList, CountArray(ResetUserData.szUserIDList), strUserIDList);

		//投递数据
		WORD wDataSize = CountStringBuffer(ResetUserData.szUserIDList);
		WORD wHeadSize = sizeof(ResetUserData) - sizeof(ResetUserData.szUserIDList);
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_RESET_USER_DATA, 0L, 0L, 0L, &ResetUserData, wHeadSize + wDataSize);
	}

	//消息通知
	if (UserIDList.GetCount() > 0)
	{
		//构造结构
		CMD_MB_ResetUserData ResetUserData;
		ZeroMemory(&ResetUserData, sizeof(ResetUserData));

		//设置变量
		ResetUserData.wDayID = CurrSystemTime.wDay;
		ResetUserData.wDayOfWeek = CurrSystemTime.wDayOfWeek;

		//更新通知
		for (auto i = 0; i < UserIDList.GetCount(); i++)
		{
			auto pGlobalUserItem = m_GlobalItemManager.SearchUserItem(UserIDList[i]);
			if (pGlobalUserItem != NULL)
			{
				//查找网关				
				auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGlobalUserItem->GetGateID());
				if (pGlobalGateItem != NULL)
				{
					DWORD dwTokenID = pGlobalUserItem->GetTokenID();
					SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_MB_USER_SERVICE, SUB_MB_RESET_USERDATA, &ResetUserData, sizeof(ResetUserData),&dwTokenID,1);
				}				
			}
		}
	}	
}

//发送类型
VOID CAttemperEngineSink::SendGameTypeInfo(DWORD dwSocketID,DWORD dwTokenID)
{
	//变量定义
	WORD wSendSize=0;
	POSITION Position=NULL;
	CGameTypeItem * pGameTypeItem=NULL;

	//枚举数据
	for (DWORD i=0;i<m_ServerListManager.GetGameTypeCount();i++)
	{
		//发送数据
		if ((wSendSize+sizeof(tagGameType))>sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_TYPE,m_cbBuffer,wSendSize,&dwTokenID,1);
			wSendSize=0;
		}

		//获取数据
		pGameTypeItem=m_ServerListManager.EmunGameTypeItem(Position);
		if (pGameTypeItem==NULL) break;

		//拷贝数据
		CopyMemory(m_cbBuffer+wSendSize,&pGameTypeItem->m_GameType,sizeof(tagGameType));
		wSendSize+=sizeof(tagGameType);
	}

	//发送剩余
	if (wSendSize>0) SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_TYPE,m_cbBuffer,wSendSize,&dwTokenID,1);

	return;
}

//发送种类
VOID CAttemperEngineSink::SendGameKindInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wSupportMask)
{
	//变量定义
	WORD wSendSize=0;
	POSITION Position=NULL;
	tagGameKind * pGameKind=NULL;
	CGameKindItem * pGameKindItem=NULL;

	//枚举数据
	for (DWORD i=0;i<m_ServerListManager.GetGameKindCount();i++)
	{
		//发送数据
		if ((wSendSize+sizeof(tagGameKind))>sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_KIND,m_cbBuffer,wSendSize, &dwTokenID, 1);
			wSendSize=0;
		}

		//获取数据
		pGameKindItem=m_ServerListManager.EmunGameKindItem(Position);
		if (pGameKindItem==NULL) break;

		//查找类型
		if ((pGameKindItem->m_GameKind.wSupportMask&wSupportMask)==0) continue;

		//获取对象
		pGameKind=(tagGameKind *)(m_cbBuffer+wSendSize);

		//拷贝数据
		CopyMemory(pGameKind, &pGameKindItem->m_GameKind, sizeof(tagGameKind));

		//设置人数
		pGameKind->dwOnLineCount=pGameKindItem->m_dwOnLineCount;
		wSendSize+=sizeof(tagGameKind);
	}

	//发送剩余
	if (wSendSize>0) SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_KIND,m_cbBuffer,wSendSize,&dwTokenID,1);

	return;
}

//发送节点
VOID CAttemperEngineSink::SendGameNodeInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wKindID)
{
	//变量定义
	WORD wSendSize=0;
	POSITION Position=NULL;
	CGameNodeItem * pGameNodeItem=NULL;

	//枚举数据
	for (DWORD i=0;i<m_ServerListManager.GetGameNodeCount();i++)
	{
		//发送数据
		if ((wSendSize+sizeof(tagGameNode))>sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_NODE,m_cbBuffer,wSendSize,&dwTokenID,1);
			wSendSize=0;
		}

		//获取数据
		pGameNodeItem=m_ServerListManager.EmunGameNodeItem(Position);
		if (pGameNodeItem==NULL) break;

		//拷贝数据
		if (pGameNodeItem->m_GameNode.wKindID==wKindID)
		{
			CopyMemory(m_cbBuffer+wSendSize,&pGameNodeItem->m_GameNode,sizeof(tagGameNode));
			wSendSize+=sizeof(tagGameNode);
		}
	}

	//发送剩余
	if (wSendSize>0) SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_NODE,m_cbBuffer,wSendSize,&dwTokenID,1);

	return;
}

//发送房间
VOID CAttemperEngineSink::SendGameServerInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wKindID, WORD wClientMask)
{
	//变量定义
	WORD wSendSize=0;
	POSITION Position=NULL;
	tagPCGameServer PCGameServer={0};
	CGameServerItem * pGameServerItem=NULL;

	

	//枚举数据
	for (DWORD i=0;i<m_ServerListManager.GetGameServerCount();i++)
	{
		//发送数据
		if ((wSendSize+sizeof(tagGameServer))>sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_SERVER,m_cbBuffer,wSendSize,&dwTokenID,1);
			wSendSize=0;
		}

		//获取数据
		pGameServerItem=m_ServerListManager.EmunGameServerItem(Position);
		if (pGameServerItem==NULL) break;

		//拷贝数据
		if ((pGameServerItem->m_GameServer.wKindID==wKindID && pGameServerItem->m_GameServer.wVisibleMask&wClientMask) 
			|| wKindID==0 )
		{
			//构造结构
			PCGameServer.wKindID=pGameServerItem->m_GameServer.wKindID;	
			PCGameServer.wNodeID=pGameServerItem->m_GameServer.wNodeID;	
			PCGameServer.wSortID=pGameServerItem->m_GameServer.wSortID;	
			PCGameServer.wServerID=pGameServerItem->m_GameServer.wServerID;
			PCGameServer.wServerPort=pGameServerItem->m_GameServer.wServerPort;
			PCGameServer.wServerType=pGameServerItem->m_GameServer.wServerType;
			PCGameServer.dwOnLineCount=pGameServerItem->m_GameServer.dwOnLineCount;
			PCGameServer.dwServerAddr=pGameServerItem->m_GameServer.dwServerAddr;
			StringCchCopy(PCGameServer.szServerName,CountArray(PCGameServer.szServerName), pGameServerItem->m_GameServer.szServerName);

			//分数信息
			PCGameServer.lCellScore=pGameServerItem->m_GameServer.lCellScore;
			PCGameServer.lMinEnterScore=pGameServerItem->m_GameServer.lMinEnterScore;
			PCGameServer.lMaxEnterScore=pGameServerItem->m_GameServer.lMaxEnterScore;

			//拷贝数据
			CopyMemory(m_cbBuffer+wSendSize,&PCGameServer,sizeof(tagPCGameServer));
			wSendSize+=sizeof(tagPCGameServer);
		}
	}

	//发送剩余
	if (wSendSize>0) SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_SERVER,m_cbBuffer,wSendSize,&dwTokenID,1);

	return;
}

//发送定制
VOID CAttemperEngineSink::SendGamePageInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wKindID)
{
	//变量定义
	WORD wSendSize=0;
	POSITION Position=NULL;
	CGameCustomItem * pGameCustomItem=NULL;

	//枚举数据
	for (DWORD i=0;i<m_ServerListManager.GetGameCustomCount();i++)
	{
		//发送数据
		if ((wSendSize+sizeof(tagGameCustom))>sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_CUSTOM,m_cbBuffer,wSendSize,&dwTokenID,1);
			wSendSize=0;
		}

		//获取数据
		pGameCustomItem=m_ServerListManager.EmunGameCustomItem(Position);
		if (pGameCustomItem==NULL) break;

		//拷贝数据
		if (pGameCustomItem->m_GameCustom.wKindID==wKindID)
		{
			CopyMemory(m_cbBuffer+wSendSize,&pGameCustomItem->m_GameCustom,sizeof(tagGameCustom));
			wSendSize+=sizeof(tagGameCustom);
		}
	}

	//发送剩余
	if (wSendSize>0) SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_CUSTOM,m_cbBuffer,wSendSize,&dwTokenID,1);

	return;
}

//发送类型
VOID CAttemperEngineSink::SendMobileKindInfo(DWORD dwSocketID, DWORD dwTokenID, tagExtendParemter* pExtendParemter)
{
	//变量定义
	WORD wSendSize=0;
	bool bShielded = false;
	POSITION Position=NULL;
	CGameKindItem * pGameKindItem=NULL;

	do
	{
		//获取数据
		pGameKindItem = m_ServerListManager.EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//发送数据
		if ((wSendSize + sizeof(tagGameKind)) > sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_KIND, m_cbBuffer, wSendSize,&dwTokenID,1);
			wSendSize = 0;
		}
			
		//支持过滤
		if ((pGameKindItem->m_GameKind.wSupportMask & SUPPORT_MASK_MOBILE) == 0) continue;
		
		//设置标识
		bShielded = false;

		//查找类型 
		for (int i = 0; i < pExtendParemter->wKindCount; i++)
		{
			if (pGameKindItem->m_GameKind.wKindID == pExtendParemter->wShieldKindList[i])
			{
				bShielded = true;
				break;
			}
		}
		CString s;
		s.Format(_T("wKindID:%d,wModuleID:%d,bShielded:%d"), pGameKindItem->m_GameKind.wKindID, pGameKindItem->m_GameKind.wModuleID, bShielded);
		CTraceService::TraceString(s, TraceLevel_Exception);

		//屏蔽判断
		if (bShielded == false)
		{
			//拷贝数据
			CopyMemory(m_cbBuffer + wSendSize, &pGameKindItem->m_GameKind, sizeof(tagGameKind));
			wSendSize += sizeof(tagGameKind);
		}

	} while (Position!=NULL);

	//发送剩余
	if (wSendSize>0) SendDataToGate(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_KIND,m_cbBuffer,wSendSize,&dwTokenID,1);

	return;
}

//发送在线
VOID CAttemperEngineSink::SendMobileOnlineInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wModuleID, tagExtendParemter* pExtendParemter)
{
	//变量定义
	WORD wSendSize = 0;
	bool bShielded = false;
	POSITION Position = NULL;
	CGameServerItem* pGameServerItem = NULL;

	//枚举房间
	do
	{
		pGameServerItem = m_ServerListManager.EmunGameServerItem(Position);
		if (pGameServerItem == NULL) break;

		//发送数据
		if ((wSendSize + sizeof(tagOnlineInfo)) > sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_ONLINE, m_cbBuffer, wSendSize,&dwTokenID,1);
			wSendSize = 0;
		}	

		//设置标识
		bShielded = false;

		//查找类型 
		for (int i = 0; i < pExtendParemter->wKindCount; i++)
		{
			if (pGameServerItem->m_GameServer.wKindID == pExtendParemter->wShieldKindList[i])
			{
				bShielded = true;
				break;
			}
		}

		//拷贝数据
		if (bShielded==false && pGameServerItem->m_GameServer.wVisibleMask & MASK_VISIBLE_MOBILE &&
			(pGameServerItem->m_GameServer.wKindID == wModuleID || wModuleID == INVALID_WORD))
		{
			//获取对象
			auto pOnlineInfo = (tagOnlineInfo*)(m_cbBuffer+wSendSize);

			//在线信息
			pOnlineInfo->wServerID = pGameServerItem->m_GameServer.wServerID;
			pOnlineInfo->dwOnlineCount = pGameServerItem->m_GameServer.dwOnLineCount;

			//拷贝数据			
			wSendSize += sizeof(tagOnlineInfo);
		}

	} while (Position != NULL);

	//发送剩余
	if (wSendSize > 0) SendDataToGate(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_ONLINE, m_cbBuffer, wSendSize,&dwTokenID,1);

	return;
}

//发送奖金
VOID CAttemperEngineSink::SendMobileJackpotInfo(DWORD dwSocketID, DWORD dwTokenID, tagExtendParemter* pExtendParemter)
{
	//网络数据
	WORD wSendSize = 0;
	SCORE lJacketScore = 0;
	POSITION Position = NULL;
	CGameKindItem* pGameKindItem = NULL;

	do
	{
		//获取数据
		pGameKindItem = m_ServerListManager.EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//发送数据
		if ((wSendSize + sizeof(tagJackpotInfo)) > sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_JACKPOT, m_cbBuffer, wSendSize, &dwTokenID, 1);
			wSendSize = 0;
		}

		if (pGameKindItem->m_ulLastTickCount > 0)
		{
			//获取对象
			auto pJackpotInfo = (tagJackpotInfo*)(m_cbBuffer + wSendSize);

			//在线信息
			pJackpotInfo->wKindID = pGameKindItem->m_GameKind.wKindID;
			pJackpotInfo->lJackpotScore = pGameKindItem->m_lJackpotScore;

			//拷贝数据			
			wSendSize += sizeof(tagJackpotInfo);
		}

	} while (Position != NULL);

	//发送数据
	if (wSendSize > 0) SendDataToGate(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_JACKPOT, m_cbBuffer, wSendSize, &dwTokenID, 1);

	return;
}

//发送房间
VOID CAttemperEngineSink::SendMobileServerInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wModuleID, tagExtendParemter* pExtendParemter)
{
	//网络数据
	WORD wSendSize=0;
	bool bShielded = false;
	POSITION Position=NULL;
	CGameServerItem * pGameServerItem=NULL;
	tagMBGameServer MBGameServer={0};	

	do
	{
		//获取数据
		pGameServerItem = m_ServerListManager.EmunGameServerItem(Position);
		if (pGameServerItem == NULL) break;

		//发送数据
		if ((wSendSize + sizeof(tagMBGameServer)) > sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_SERVER, m_cbBuffer, wSendSize,&dwTokenID,1);
			wSendSize = 0;
		}

		//设置标识
		bShielded = false;

		//查找类型 
		for (int i = 0; i < pExtendParemter->wKindCount; i++)
		{
			if (pGameServerItem->m_GameServer.wKindID == pExtendParemter->wShieldKindList[i])
			{
				bShielded = true;
				break;
			}
		}

		//拷贝数据
		if (bShielded==false && pGameServerItem->m_GameServer.wVisibleMask & MASK_VISIBLE_MOBILE &&
			(pGameServerItem->m_GameServer.wKindID == wModuleID || wModuleID == INVALID_WORD))
		{
			//构造结构
			MBGameServer.wKindID = pGameServerItem->m_GameServer.wKindID;
			MBGameServer.wSortID = pGameServerItem->m_GameServer.wSortID;
			MBGameServer.wServerID = pGameServerItem->m_GameServer.wServerID;
			MBGameServer.wServerPort = pGameServerItem->m_GameServer.wServerPort;
			MBGameServer.wServerType = pGameServerItem->m_GameServer.wServerType;
			MBGameServer.wServerLevel = pGameServerItem->m_GameServer.wServerLevel;
			MBGameServer.wChairCount = pGameServerItem->m_GameServer.wChairCount;
			MBGameServer.dwMaxPlayer = pGameServerItem->m_GameServer.dwMaxPlayer;
			MBGameServer.dwOnLineCount = pGameServerItem->m_GameServer.dwOnLineCount;
			MBGameServer.dwServerAddr = pGameServerItem->m_GameServer.dwServerAddr;
			StringCchCopy(MBGameServer.szServerName, CountArray(MBGameServer.szServerName), pGameServerItem->m_GameServer.szServerName);

			//服务域名
			StringCchCopy(MBGameServer.szServerDomain, CountArray(MBGameServer.szServerDomain), pGameServerItem->m_GameServer.szServerDomain);

			//分数信息
			MBGameServer.lCellScore = pGameServerItem->m_GameServer.lCellScore;
			MBGameServer.lMinEnterScore = pGameServerItem->m_GameServer.lMinEnterScore;
			MBGameServer.lMaxEnterScore = pGameServerItem->m_GameServer.lMaxEnterScore;

			//拷贝数据
			CopyMemory(m_cbBuffer + wSendSize, &MBGameServer, sizeof(tagMBGameServer));
			wSendSize += sizeof(tagMBGameServer);
		}

	} while (Position!=NULL);

	//发送剩余
	if (wSendSize>0) SendDataToGate(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_SERVER, m_cbBuffer,wSendSize,&dwTokenID,1);

	return;
}

//发送背包
VOID CAttemperEngineSink::SendUserPackGoods(DWORD dwSocketID, DWORD dwTokenID, tagGoodsHoldInfo GoodsHoldInfo[], WORD wGoodsCount)
{
	//定义变量
	auto pPackGoods = (CMD_CM_PackGoods*)m_cbBuffer;
	ZeroMemory(pPackGoods, sizeof(CMD_CM_PackGoods));

	//设置变量
	pPackGoods->wGoodsCount = wGoodsCount;
	CopyMemory(pPackGoods->GoodsHoldInfo, GoodsHoldInfo, pPackGoods->wGoodsCount * sizeof(pPackGoods->GoodsHoldInfo[0]));

	//发送数据
	WORD wPacketSize = sizeof(CMD_CM_GoodsUpdate) + pPackGoods->wGoodsCount * sizeof(pPackGoods->GoodsHoldInfo[0]);
	SendDataToGate(dwSocketID, MDM_CM_SYSTEM, SUB_CM_PACK_GOODS, pPackGoods, wPacketSize,&dwTokenID,1);
}
//发送昵称配置
VOID CAttemperEngineSink::SendNickNameOption(DWORD dwSocketID, DWORD dwTokenID, WORD wModifyTime, WORD wDailyModifyTimes)
{
	//定义变量
	auto pHead = (CMD_CM_ConfigUpdate*)m_cbBuffer;
	ZeroMemory(pHead, sizeof(CMD_CM_ConfigUpdate));

	//设置变量
	pHead->dwType = CM_CONFIG_TYPE_NICKNAME;

	CMD_CM_Config_NickName* pData = (CMD_CM_Config_NickName*)(pHead + 1);
	pData->wFreeModifyTimes = m_NickNameOption.wFreeModifyTimes;
	pData->wDailyModifyTimes = m_NickNameOption.wDailyModifyTimes;
	pData->dwGoodsID = m_NickNameOption.dwGoodsID;
	pData->dwGoodsCount = m_NickNameOption.dwGoodsCount;
	//
	if (wModifyTime >= pData->wFreeModifyTimes)
		pData->wFreeModifyTimes = 0;
	else
		pData->wFreeModifyTimes -= wModifyTime;


	//发送数据
	WORD wPacketSize = sizeof(CMD_CM_ConfigUpdate) + sizeof(CMD_CM_Config_NickName);
	SendDataToGate(dwSocketID, MDM_CM_SYSTEM, SUB_CM_CONFIG_UPDATE, pHead, wPacketSize, &dwTokenID, 1);
};
//用户头像
bool CAttemperEngineSink::OnDBMBUserFaceInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//提取数据
	auto pUserFaceInfo = (DBO_MB_UserFaceInfo*)pData;

	//变量定义
	CMD_MB_UserFaceInfo UserFaceInfo;
	ZeroMemory(&UserFaceInfo, sizeof(UserFaceInfo));	

	//设置变量
	UserFaceInfo.wFaceID = pUserFaceInfo->wFaceID;
	UserFaceInfo.dwCustomID = pUserFaceInfo->dwCustomID;

	//发送消息
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_USER_FACE_INFO, &UserFaceInfo, sizeof(UserFaceInfo),&dwTokenID,1);

	return true;
}

//用户缓冲
bool CAttemperEngineSink::OnDBMBUserCacheInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//解析数据
	UserPublicInfo CachePublicInfo;
	if (CachePublicInfo.ParseFromArray((char*)pData, wDataSize))
	{	
		//构造结构
		CMD_CS_C_UserUpdate UserUpdate = {};

		//设置变量
		UserUpdate.dwUserID=CachePublicInfo.uid();

		//用户昵称
		if (CachePublicInfo.has_nickname())
		{
			UserUpdate.dwInfoMask |= INFO_MASK_NICKNAME;

			//转换昵称
			TCHAR szNickName[LEN_NICKNAME] = {0};
			CWHService::Utf82Unicode(CachePublicInfo.nickname().c_str(), szNickName,CountArray(szNickName));

			//拷贝昵称
			StringCchCopy(UserUpdate.szNickName,CountArray(UserUpdate.szNickName), szNickName);
		}

		//头像标识
		if ((CachePublicInfo.has_faceid() && CachePublicInfo.has_customid()))
		{
			UserUpdate.dwInfoMask |= INFO_MASK_FACEID;
			UserUpdate.wFaceID= CachePublicInfo.faceid();
			UserUpdate.dwCustomID=CachePublicInfo.customid();
		}

		//数据更新
		m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_USER_UPDATE, &UserUpdate, sizeof(UserUpdate), 0);
	}

	//发送消息
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_USER_CACHE_INFO, pData, wDataSize,&dwTokenID,1);

	return true;
}

//兑换商品
bool CAttemperEngineSink::OnDBMBExchangeMallGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//提取数据
	auto pExchangeMallGoods = (DBO_MB_ExchangeMallGoods*)pData;

	//构造物品
	tagGoodsItem GoodsItem = {};	
	GoodsItem.wGoodsID = pExchangeMallGoods->wIncGoodsID;
	GoodsItem.dwGoodsCount = pExchangeMallGoods->dwIncGoodsCount;
	GoodsItem.dwGoodsIndate = pExchangeMallGoods->dwIncGoodsIndate;

	//构造列表
	TCHAR szGoodsList[128];
	g_GameGoodsManager->SerializeToJson(&GoodsItem,1, szGoodsList, CountArray(szGoodsList));

	//操作成功
	SendOperateSuccess(dwContextID, dwTokenID, SUB_MB_EXCHANGE_MALLGOODS, pExchangeMallGoods->lResultCode, pExchangeMallGoods->szDescribeString, szGoodsList);

	return true;
}

//兑换礼包码
bool CAttemperEngineSink::OnDBMBExchangeGiftbagCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//提取数据
	auto pExchangeGiftbagCode = (DBO_MB_ExchangeGiftbagCode*)pData;

	//操作成功
	SendOperateSuccess(dwContextID, dwTokenID, SUB_MB_EXCHANGE_GIFTBAGCODE, pExchangeGiftbagCode->lResultCode, pExchangeGiftbagCode->szDescribeString, pExchangeGiftbagCode->szGiftBagGoodsList);

	return true;
}


//签到结果
bool CAttemperEngineSink::OnDBMBCheckInResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//提取数据
	auto pCheckInResult = (DBO_MB_CheckInResult*)pData;

	//构造结果
	CMD_MB_CheckInResult CheckInResult = {};

	//设置变量
	CheckInResult.wCheckInDays = pCheckInResult->wCheckInDays;
	CheckInResult.cbCheckInState = pCheckInResult->cbCheckInState;

	//发送结果
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_CHECKIN_RESULT, &CheckInResult, sizeof(CheckInResult), &dwTokenID, 1);

	//操作成功
	SendOperateSuccess(dwContextID, dwTokenID, SUB_MB_REQUEST_CHECKIN, 0, TEXT(""), pCheckInResult->szRewardGoodsList);

	return true;
}

//绑定邀请码成功
bool CAttemperEngineSink::OnDBMBBindInviteCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
    //判断在线
    auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
    if (pTokenParameter == NULL) return true;

    //效验参数
    ASSERT(wDataSize <= sizeof(DBO_MB_BindInviteCode));
    if (wDataSize > sizeof(DBO_MB_BindInviteCode)) return false;

    //提取数据
	auto  pBindInviteCode = (DBO_MB_BindInviteCode*)pData;

    //变量定义
    CMD_MB_BindInviteCodeSuccess BindInviteCodeSuccess;
    ZeroMemory(&BindInviteCodeSuccess, sizeof(BindInviteCodeSuccess));

	//构造数据
	BindInviteCodeSuccess.dwParentGameID = pBindInviteCode->dwParentGameID;
	BindInviteCodeSuccess.wParentFaceID = pBindInviteCode->wParentFaceID;
    StringCchCopy(BindInviteCodeSuccess.szParentNickName, CountArray(BindInviteCodeSuccess.szParentNickName), pBindInviteCode->szParentNickName);
    StringCchCopy(BindInviteCodeSuccess.szNoticeString, CountArray(BindInviteCodeSuccess.szNoticeString), pBindInviteCode->szNoticeString);

    //发送数据
    WORD wStringSize = CountStringBuffer(BindInviteCodeSuccess.szNoticeString);
    WORD wPacketSize = sizeof(BindInviteCodeSuccess) - sizeof(BindInviteCodeSuccess.szNoticeString) + wStringSize;
    SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_BIND_INVITE_CODE_SUCCESS, &BindInviteCodeSuccess, wPacketSize,&dwTokenID,1);

	//定义变量
	BYTE cbBuffer[256];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//获取对象
	tagForwardHead* pForwardHead = (tagForwardHead*)cbBuffer;

	//设置转发头
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwardHead->dwUserID = pBindInviteCode->dwUserID;

	//转发信息
	pForwardHead->wItemCount = 1;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;

	//转换对象				
	CMD_CS_MailNotify* pMailNotify = (CMD_CS_MailNotify*)(pForwardHead + 1);

	//设置邮件
	pMailNotify->dwUserID = pBindInviteCode->dwUserID;
	pMailNotify->dwMailID = pBindInviteCode->dwMailID;

	//发送数据
	wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_MailNotify);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_MAIL_NOTIFY, cbBuffer, wPacketSize, 0);

    return true;
}

//绑定账号
bool CAttemperEngineSink::OnDBMBBindExternalAccount(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//效验参数
	ASSERT(wDataSize <= sizeof(DBO_MB_BindExternalAccount));
	if (wDataSize > sizeof(DBO_MB_BindExternalAccount)) return false;

	//提取数据
	auto pBindExternalAccount = (DBO_MB_BindExternalAccount*)pData;

	//变量定义
	CMD_MB_BindExternalAccountSuccess BindExternalAccountSuccess;
	ZeroMemory(&BindExternalAccountSuccess, sizeof(BindExternalAccountSuccess));

	//构造数据
	BindExternalAccountSuccess.dwExternalID = pBindExternalAccount->dwExternalID;
	StringCchCopy(BindExternalAccountSuccess.szUniqueID, CountArray(BindExternalAccountSuccess.szUniqueID), pBindExternalAccount->szUniqueID);
	StringCchCopy(BindExternalAccountSuccess.szNoticeString, CountArray(BindExternalAccountSuccess.szNoticeString), pBindExternalAccount->szNoticeString);

	//发送数据
	WORD wStringSize = CountStringBuffer(BindExternalAccountSuccess.szNoticeString);
	WORD wPacketSize = sizeof(BindExternalAccountSuccess) - sizeof(BindExternalAccountSuccess.szNoticeString) + wStringSize;
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_BIND_EXTERNAL_ACCOUNT_SUCCESS, &BindExternalAccountSuccess, wPacketSize, &dwTokenID, 1);

	return true;
}

//赠送礼物成功
bool CAttemperEngineSink::OnDBMBSendGiftSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
    //判断在线
    auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
    if (pTokenParameter == NULL) return true;

    //效验参数
    ASSERT(wDataSize <= sizeof(DBO_MB_SendGiftSuccess));
    if (wDataSize > sizeof(DBO_MB_SendGiftSuccess)) return false;

    //提取数据
    auto  pSendGiftSuccess = (DBO_MB_SendGiftSuccess*)pData;

	//赠送成功
	CMD_MB_SendGiftSuccess SendGiftSuccess;
	ZeroMemory(&SendGiftSuccess, sizeof(SendGiftSuccess));
    SendGiftSuccess.dwTargetUserID = pSendGiftSuccess->dwTargetUserID;
    SendGiftSuccess.dwGoodsID = pSendGiftSuccess->dwGoodsID;
    SendGiftSuccess.dwGoodsCount = pSendGiftSuccess->dwGoodsCount;
    SendGiftSuccess.dwLoveLiness = pSendGiftSuccess->dwLoveLiness;
    SendGiftSuccess.cbSendType = pSendGiftSuccess->cbSendType;

	//发送消息
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_SEND_GIFT_SUCCESS, &SendGiftSuccess, sizeof(SendGiftSuccess), &dwTokenID, 1);

    //魅力值变更
    CMD_CM_LoveLinessUpdate LoveLinessUpdate;
    ZeroMemory(&LoveLinessUpdate, sizeof(LoveLinessUpdate));
    LoveLinessUpdate.dwLoveLiness = pSendGiftSuccess->dwMyLoveLiness;

    //发送数据
    SendDataToGate(dwContextID, MDM_CM_SYSTEM, SUB_CM_LOVELINESS_UPDATE, &LoveLinessUpdate, sizeof(LoveLinessUpdate), &dwTokenID, 1);

	//群发所有人
	if (pSendGiftSuccess->cbSendType == 0)
	{
		//构造消息
		CMD_MB_GlobalGiftNotify GlobalGiftNotify;
        ZeroMemory(&GlobalGiftNotify, sizeof(GlobalGiftNotify));
        GlobalGiftNotify.dwUserID = pSendGiftSuccess->dwUserID;
        GlobalGiftNotify.dwTargetUserID = pSendGiftSuccess->dwTargetUserID;
        GlobalGiftNotify.dwGoodsID = pSendGiftSuccess->dwGoodsID;
		GlobalGiftNotify.dwGoodsCount = pSendGiftSuccess->dwGoodsCount;
		GlobalGiftNotify.dwLoveLiness = pSendGiftSuccess->dwLoveLiness;
		GlobalGiftNotify.cbSendType = pSendGiftSuccess->cbSendType;

		//群发数据
		SendBatchData(MDM_MB_USER_SERVICE, SUB_MB_GLOBAL_GIFT_NOTIFY, &GlobalGiftNotify, sizeof(GlobalGiftNotify));
	}

    //查找目标用户
    auto pGlobalUserItem = m_GlobalItemManager.SearchUserItem(pSendGiftSuccess->dwTargetUserID);
	if (pGlobalUserItem != NULL)
	{
		//查找网关
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGlobalUserItem->GetGateID());
		if (pGlobalGateItem != NULL)
		{
			//通知对方
			SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_MB_USER_SERVICE, SUB_MB_RECV_GIFT_NOTIFY, NULL, 0,&pGlobalUserItem->m_dwTokenID,1);

			//对方魅力值
			ZeroMemory(&LoveLinessUpdate, sizeof(LoveLinessUpdate));
			LoveLinessUpdate.dwLoveLiness = pSendGiftSuccess->dwTargetLoveLiness;

			//发送数据
			SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_CM_SYSTEM, SUB_CM_LOVELINESS_UPDATE, &LoveLinessUpdate, sizeof(LoveLinessUpdate),&pGlobalUserItem->m_dwTokenID,1);
		}		
	}	
	
	return true;
}

//礼物信息
bool CAttemperEngineSink::OnDBMBUserGiftInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
    //提取数据
    auto pDBUserGiftInfo = (DBO_MB_UserGiftInfo*)pData;

    //获取参数
    auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
    if (pTokenParameter == NULL) return true;

    //定义变量
	CMD_MB_UserGiftInfo* pUserGiftInfo = (CMD_MB_UserGiftInfo*)m_cbBuffer;
    ZeroMemory(pUserGiftInfo, sizeof(CMD_MB_UserGiftInfo));

    //构造数据		
    pUserGiftInfo->wItemCount = pDBUserGiftInfo->wItemCount;
    if (pUserGiftInfo->wItemCount > 0)
    {
        CopyMemory(pUserGiftInfo->GiftInfo, pDBUserGiftInfo->GiftInfo, pUserGiftInfo->wItemCount * sizeof(pUserGiftInfo->GiftInfo[0]));
    }

    //发送数据
    WORD wPacketSize = sizeof(CMD_MB_UserGiftInfo) + pUserGiftInfo->wItemCount * sizeof(pUserGiftInfo->GiftInfo[0]);
    SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_GIFT_INFO, pUserGiftInfo, wPacketSize,&dwTokenID,1);

    return true;
}

//礼物信息完成
bool CAttemperEngineSink::OnDBMBUserGiftInfoFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
    //获取参数
    auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
    if (pTokenParameter == NULL) return true;

    //发送数据
    SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_GIFT_INFO_FINISH, NULL, 0, &dwTokenID, 1);

	return true;
}

//转账成功
bool CAttemperEngineSink::OnDBMBWealthTransferSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//效验参数
	ASSERT (wDataSize == sizeof(DBO_MB_WealthTransferSuccess));
	if (wDataSize > sizeof(DBO_MB_WealthTransferSuccess)) return false;

	//提取数据
	auto  pWealthTransferSuccess = (DBO_MB_WealthTransferSuccess*)pData;

	//定义变量
	BYTE cbBuffer[256];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//获取对象
	tagForwardHead* pForwardHead = (tagForwardHead*)cbBuffer;

	//设置转发头
	pForwardHead->wTarget = FORWARD_TARGET_USER;
	pForwardHead->dwUserID = pWealthTransferSuccess->dwTargetUserID;

	//转发信息
	pForwardHead->wItemCount = 1;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;

	//转换对象				
	CMD_CS_MailNotify* pMailNotify = (CMD_CS_MailNotify*)(pForwardHead + 1);

	//设置邮件
	pMailNotify->dwMailID = pWealthTransferSuccess->dwMailID;
	pMailNotify->dwUserID = pWealthTransferSuccess->dwTargetUserID;	

	//发送数据
	WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_MailNotify);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_MAIL_NOTIFY, cbBuffer, wPacketSize, 0);

	//操作成功
	SendOperateSuccess(dwContextID, dwTokenID, SUB_MB_WEALTH_TRANSFER, DB_SUCCESS, TEXT(""), NULL);

	return true;
}

//转盘抽奖数据
bool CAttemperEngineSink::OnDBMBTurntableLotteryData(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//获取参数
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//提取数据
	auto pTurntableLotteryData = (DBO_MB_TurntableLotteryData*)pData;

	//构造对象
	CMD_MB_TurntableLotteryData TurntableLotteryData;
	ZeroMemory(&TurntableLotteryData,sizeof(TurntableLotteryData));

	//抽奖次数
	TurntableLotteryData.wPayLotteryTimes = pTurntableLotteryData->wPayLotteryTimes;
	TurntableLotteryData.wUseLotteryTimes = pTurntableLotteryData->wUseLotteryTimes;
	TurntableLotteryData.wFreeLotteryTimes = pTurntableLotteryData->wFreeLotteryTimes;	
	TurntableLotteryData.wResidueLotteryTimes = pTurntableLotteryData->wResidueLotteryTimes;
	TurntableLotteryData.wDailyMayLotteryTimes = pTurntableLotteryData->wDailyMayLotteryTimes;	
	TurntableLotteryData.wGameTimeLotteryTimes = pTurntableLotteryData->wGameTimeLotteryTimes;
	TurntableLotteryData.wGameCountLotteryTimes = pTurntableLotteryData->wGameCountLotteryTimes;
	TurntableLotteryData.wGameExpendLotteryTimes = pTurntableLotteryData->wGameExpendLotteryTimes;

	//进度数据
	CopyMemory(TurntableLotteryData.dwGameTimeProgress, pTurntableLotteryData->dwGameTimeProgress,sizeof(TurntableLotteryData.dwGameTimeProgress));
	CopyMemory(TurntableLotteryData.dwGameCountProgress, pTurntableLotteryData->dwGameCountProgress, sizeof(TurntableLotteryData.dwGameCountProgress));
	CopyMemory(TurntableLotteryData.dwGameExpendProgress, pTurntableLotteryData->dwGameExpendProgress, sizeof(TurntableLotteryData.dwGameExpendProgress));

	//转盘记录
	StringCchCopy(TurntableLotteryData.szTurantableRecord,CountArray(TurntableLotteryData.szTurantableRecord), pTurntableLotteryData->szTurntableRecord);

	//计算大小
	WORD wRecordSize = CountStringBuffer(TurntableLotteryData.szTurantableRecord);
	WORD wPacketSize = sizeof(TurntableLotteryData) - sizeof(TurntableLotteryData.szTurantableRecord) + wRecordSize;

	//发送数据
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_TURNTABLE_LOTTERY_DATA, &TurntableLotteryData, wPacketSize, &dwTokenID, 1);

	return true;	
}

//转盘抽奖结果
bool CAttemperEngineSink::OnDBMBTurntableLotteryResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//获取参数
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//提取数据
	auto pTurntableLotteryResult = (DBO_MB_TurntableLotteryResult*)pData;

	//构造对象
	CMD_MB_TurntableLotteryResult TurntableLotteryResult;
	ZeroMemory(&TurntableLotteryResult, sizeof(TurntableLotteryResult));

	//设置变量
	TurntableLotteryResult.wHitIndex = pTurntableLotteryResult->wHitIndex;
	TurntableLotteryResult.wUseLotteryTimes = pTurntableLotteryResult->wUseLotteryTimes;
	TurntableLotteryResult.wResidueLotteryTimes = pTurntableLotteryResult->wResidueLotteryTimes;

	//发送数据
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_TURNTABLE_LOTTERY_RESULT, &TurntableLotteryResult, sizeof(TurntableLotteryResult), &dwTokenID, 1);

	return true;
}

//行为掩码
bool CAttemperEngineSink::OnDBMBUserActionMask(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//提取数据
	auto pUserActionMask= (DBO_MB_UserActionMask*)pData;

	//变量定义
	CMD_MB_UserActionMask UserActionMask;
	ZeroMemory(&UserActionMask, sizeof(UserActionMask));

	//设置变量
	UserActionMask.dwActionMaskEver = pUserActionMask->dwActionMaskEver;
	UserActionMask.dwActionMaskPerDay = pUserActionMask->dwActionMaskPerDay;
	UserActionMask.dwActionMaskPerWeek = pUserActionMask->dwActionMaskPerWeek;

	//发送消息
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_USER_ACTION_MASK, &UserActionMask, sizeof(UserActionMask), &dwTokenID, 1);

	return true;
}

//排位信息
bool CAttemperEngineSink::OnDBMBUserRankingInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//参数校验
	ASSERT(wDataSize == sizeof(tagUserRankingInfo));
	if (wDataSize != sizeof(tagUserRankingInfo)) return false;

	//提取数据
	auto pUserRankingInfo = (tagUserRankingInfo*)pData;
	
	//变量定义
	CMD_MB_UserRankingInfo RankingResult;
	ZeroMemory(&RankingResult, sizeof(RankingResult));
	CopyMemory(&RankingResult.UserRankingInfo, pUserRankingInfo, sizeof(RankingResult.UserRankingInfo));

	//发送数据
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_USER_RANKING_INFO, &RankingResult, sizeof(RankingResult), &dwTokenID, 1);

	return true;
}

//登录成功
bool CAttemperEngineSink::OnDBMBLogonSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//绑定参数
	auto pBindParameter = GetBindParameter(dwContextID);
	if (pBindParameter == NULL) return NULL;

	//判断在线
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;	

	//变量定义
	DBO_MB_LogonSuccess* pDBOLogonSuccess = (DBO_MB_LogonSuccess*)pData;

	//变量定义
	CMD_MB_LogonSuccess* pLogonSuccess = (CMD_MB_LogonSuccess*)m_cbBuffer;
	ZeroMemory(pLogonSuccess, sizeof(CMD_MB_LogonSuccess));

	//构造数据
	pLogonSuccess->wFaceID = pDBOLogonSuccess->wFaceID;
	pLogonSuccess->cbGender = pDBOLogonSuccess->cbGender;
	pLogonSuccess->dwCustomID = pDBOLogonSuccess->dwCustomID;
	pLogonSuccess->dwGameID = pDBOLogonSuccess->dwGameID;
	pLogonSuccess->dwUserID = pDBOLogonSuccess->dwUserID;
	StringCchCopy(pLogonSuccess->szNickName, CountArray(pLogonSuccess->szNickName), pDBOLogonSuccess->szNickName);
	StringCchCopy(pLogonSuccess->szAccounts, CountArray(pLogonSuccess->szAccounts), pDBOLogonSuccess->szAccounts);
	StringCchCopy(pLogonSuccess->szLogonPass, CountArray(pLogonSuccess->szLogonPass), pDBOLogonSuccess->szLogonPass);
	StringCchCopy(pLogonSuccess->szInsurePass, CountArray(pLogonSuccess->szInsurePass), pDBOLogonSuccess->szInsurePass);

	//经验等级
	pLogonSuccess->dwExperience = pDBOLogonSuccess->dwExperience;

	//魅力值
	pLogonSuccess->dwLoveLiness = pDBOLogonSuccess->dwLoveLiness;

	//用户权限
	pLogonSuccess->dwUserRight = pDBOLogonSuccess->dwUserRight;

	//会员资料
	pLogonSuccess->wMasterOrder = pDBOLogonSuccess->wMasterOrder;
	pLogonSuccess->dwMemberPoint = pDBOLogonSuccess->dwMemberPoint;
	pLogonSuccess->dwMemberOverTime = pDBOLogonSuccess->dwMemberOverTime;

	//用户财富
	pLogonSuccess->lUserGold = pDBOLogonSuccess->lUserGold;
	pLogonSuccess->lUserCard = pDBOLogonSuccess->lUserCard;
	pLogonSuccess->lUserInsure = pDBOLogonSuccess->lUserInsure;	
	pLogonSuccess->lCommission = pDBOLogonSuccess->lCommission;

	//行为掩码
	pLogonSuccess->dwActionMaskEver = pDBOLogonSuccess->dwActionMaskEver;
	pLogonSuccess->dwActionMaskPerDay = pDBOLogonSuccess->dwActionMaskPerDay;
	pLogonSuccess->dwActionMaskPerWeek = pDBOLogonSuccess->dwActionMaskPerWeek;

	//代理信息
	pLogonSuccess->dwParentGameID = pDBOLogonSuccess->dwParentGameID;
	pLogonSuccess->wParentFaceID = pDBOLogonSuccess->wParentFaceID;
	StringCchCopy(pLogonSuccess->szParentNickName, CountArray(pLogonSuccess->szParentNickName), pDBOLogonSuccess->szParentNickName);

	//锁定信息
	pLogonSuccess->wLockKindID = pDBOLogonSuccess->wLockKindID;
	pLogonSuccess->wLockServerID = pDBOLogonSuccess->wLockServerID;
	pLogonSuccess->wLockServerKind = pDBOLogonSuccess->wLockServerKind;	

	//状态信息
	pLogonSuccess->cbShutUpStatus = pDBOLogonSuccess->cbShutUpStatus;
	pLogonSuccess->dwShutUpOverTime = pDBOLogonSuccess->dwShutUpOverTime;

	//调整状态
	if (pLogonSuccess->cbShutUpStatus > 0 && pLogonSuccess->dwShutUpOverTime <= time(NULL))
	{
		pLogonSuccess->cbShutUpStatus = 0;
		pLogonSuccess->dwShutUpOverTime = 0;
	}

	//附加信息
	pLogonSuccess->dwServerTime = time(NULL);
	pLogonSuccess->dwLikedNumber = pDBOLogonSuccess->dwLikedNumber;		
	pLogonSuccess->cbRegisterMode = pDBOLogonSuccess->cbRegisterMode;
	pLogonSuccess->dwRegisterTime = pDBOLogonSuccess->dwRegisterTime;
	pLogonSuccess->dwUIShieldMask = pDBOLogonSuccess->dwUIShieldMask;	

	//发送定义
	WORD wHeadSize = sizeof(CMD_MB_LogonSuccess);
	CSendPacketHelper SendPacket(m_cbBuffer + wHeadSize, sizeof(m_cbBuffer) - wHeadSize);

	//转账配置
	if (m_TransferParameter.bTransferEnabled)
	{
		//转账配置
		DTP_GP_TransferParam TransferParam;
		ZeroMemory(&TransferParam, sizeof(TransferParam));

		//设置变量
		TransferParam.bTransferEnabled = m_TransferParameter.bTransferEnabled;
		TransferParam.wServiceFeeRate = m_TransferParameter.wServiceFeeRate;
		TransferParam.lReservedAmount = m_TransferParameter.lReservedAmount;
		TransferParam.lMinTransferAmount = m_TransferParameter.lMinTransferAmount;
		TransferParam.lMaxTransferAmount = m_TransferParameter.lMaxTransferAmount;

		//转账配置
		SendPacket.AddPacket(&TransferParam, sizeof(TransferParam), DTP_MB_TRANSFER_PARAM);
	}	

	//电子邮箱
	if (pDBOLogonSuccess->szEmail[0] != 0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szEmail, CountStringBuffer(pDBOLogonSuccess->szEmail), DTP_MB_USER_EMAIL);
	}

	//用户UID
	if (pDBOLogonSuccess->szUniqueID[0] != 0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szUniqueID, CountStringBuffer(pDBOLogonSuccess->szUniqueID), DTP_MB_UNIQUEID);
	}

	//个性签名
	if (pDBOLogonSuccess->szUnderWrite[0] != 0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szUnderWrite, CountStringBuffer(pDBOLogonSuccess->szUnderWrite), DTP_MB_UNDER_WRITE);
	}	

	//手机号码
	if (pDBOLogonSuccess->szMobilePhone[0] != 0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szMobilePhone, CountStringBuffer(pDBOLogonSuccess->szMobilePhone), DTP_MB_MOBILE_PHONE);
	}

	//登录令牌
	if (pDBOLogonSuccess->szLogonToken[0] != 0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szLogonToken, CountStringBuffer(pDBOLogonSuccess->szLogonToken), DTP_MB_LOGON_TOKEN);
	}

	
	//注册赠送
	if (pDBOLogonSuccess->szPresentGoodsList[0] != 0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szPresentGoodsList, CountStringBuffer(pDBOLogonSuccess->szPresentGoodsList), DTP_MB_REGISTER_PRESENT);
	}

	//实名信息
	if (pDBOLogonSuccess->RealAuth.szCompellation[0] != 0)
	{
		//构造结构
		DTP_MB_RealAuth RealAuth;
		ZeroMemory(&RealAuth, sizeof(RealAuth));

		//设置变量
		CopyMemory(RealAuth.szCompellation,pDBOLogonSuccess->RealAuth.szCompellation,sizeof(RealAuth.szCompellation));
		CopyMemory(RealAuth.szPassPortID,pDBOLogonSuccess->RealAuth.szPassPortID,sizeof(RealAuth.szPassPortID));
		//
		SendPacket.AddPacket(&RealAuth, sizeof(RealAuth), DTP_MB_REAL_AUTH);
	}

	//CString s;
	//s.Format(_T("2 szNickName:%s,pDBOLogonSuccess->szUserFaceUrl:%s"), pDBOLogonSuccess->szNickName,pDBOLogonSuccess->szUserFaceUrl);
	//CTraceService::TraceString(s, TraceLevel_Debug); 

	//登录令牌 kk jia
	if (pDBOLogonSuccess->szUserFaceUrl[0] != 0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szUserFaceUrl, CountStringBuffer(pDBOLogonSuccess->szUserFaceUrl), DTP_MB_USER_INFO);
	}
	

	//构造结构
	CMD_CS_S_TokenParameter TokenParameter;
	ZeroMemory(&TokenParameter,sizeof(TokenParameter));

	//令牌参数
	TokenParameter.dwUserID = pDBOLogonSuccess->dwUserID;
	StringCchCopy(TokenParameter.szLogonToken,CountArray(TokenParameter.szLogonToken), pDBOLogonSuccess->szLogonToken);

	//发送数据
	WORD wPacketSize = CountStringBuffer(TokenParameter.szLogonToken);
	wPacketSize += sizeof(TokenParameter) - sizeof(TokenParameter.szLogonToken);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CS_COMMON_SERVICE, SUB_CS_S_TOKEN_PARAMETER, &TokenParameter, wPacketSize, &dwTokenID, 1);

	//登录成功
	wPacketSize = SendPacket.GetDataSize() + sizeof(CMD_MB_LogonSuccess);
	SendDataToGate(dwContextID, MDM_MB_LOGON, SUB_MB_LOGON_SUCCESS, pLogonSuccess, wPacketSize, &dwTokenID, 1);

	//扩展参数
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//解析类型列表 
	auto JsonValuePtr = CWHJson::getValue(string(CT2CA(pDBOLogonSuccess->szShieldKindList)));
	if (JsonValuePtr->getType() == eJsonTypeArray)
	{
		//转换对象
		auto KindObjArrayPtr = CJsonValueArrayPtr::dynamicCast(JsonValuePtr);

		for (auto KindObjPtr : KindObjArrayPtr->value)
		{
			if (KindObjPtr->getType() == eJsonTypeNum)
			{
				auto KindIDPtr = CJsonValueNumPtr::dynamicCast(KindObjPtr);
				if (pExtendParameter->wKindCount < CountArray(pExtendParameter->wShieldKindList))
				{
					pExtendParameter->wShieldKindList[pExtendParameter->wKindCount++] = (WORD)KindIDPtr->value;
				}				
			}
		}
	}

	//获取标识
	WORD wIndex = LOWORD(dwContextID);
	WORD wModuleID = pExtendParameter->wModuleID;

	//类型信息
	SendMobileKindInfo(dwContextID, dwTokenID, pExtendParameter);

	//奖池信息
	SendMobileJackpotInfo(dwContextID, dwTokenID, pExtendParameter);

	//房间信息
	SendMobileServerInfo(dwContextID, dwTokenID, wModuleID, pExtendParameter);	

	//背包物品
	if (pDBOLogonSuccess->wGoodsCount>0)
	{
		SendUserPackGoods(dwContextID, dwTokenID, pDBOLogonSuccess->GoodsHoldInfo, pDBOLogonSuccess->wGoodsCount);
	}
	//昵称配置
	SendNickNameOption(dwContextID, dwTokenID,pDBOLogonSuccess->wNickNameModifyTimes, pDBOLogonSuccess->wNickNameDailyModifyTimes);

	//完成通知
	SendDataToGate(dwContextID, MDM_MB_SERVER_LIST, SUB_MB_LIST_FINISH, &dwTokenID, 1);

	//变量定义
	CMD_MB_LogonFinish LogonFinish;
	ZeroMemory(&LogonFinish, sizeof(LogonFinish));

	//登录完成
	LogonFinish.dwFuncOption = m_dwMBFuncption;

	//发送数据
	SendDataToGate(dwContextID, MDM_MB_LOGON, SUB_MB_LOGON_FINISH, &LogonFinish, sizeof(LogonFinish), &dwTokenID, 1);

	//构造结构
	tagGlobalUserInfo GlobalUserInfo;
	ZeroMemory(&GlobalUserInfo,sizeof(GlobalUserInfo));

	//用户信息
	GlobalUserInfo.dwUserID = pDBOLogonSuccess->dwUserID;
	GlobalUserInfo.dwGameID = pDBOLogonSuccess->dwGameID;
	GlobalUserInfo.wFaceID= pDBOLogonSuccess->wFaceID;
	GlobalUserInfo.dwCustomID = pDBOLogonSuccess->dwCustomID;
	StringCchCopy(GlobalUserInfo.szNickName, CountArray(GlobalUserInfo.szNickName), pDBOLogonSuccess->szNickName);

	//网络信息
	GlobalUserInfo.SocketInfo.wGateID = pBindParameter->wServiceID;
	GlobalUserInfo.SocketInfo.dwTokenID = pTokenParameter->dwTokenID;

	//状态信息
	GlobalUserInfo.cbShutUpStatus = pDBOLogonSuccess->cbShutUpStatus;
	GlobalUserInfo.dwShutUpOverTime = pDBOLogonSuccess->dwShutUpOverTime;	

	//激活用户
	auto pGlobalUserItem = m_GlobalItemManager.ActiveUserItem(GlobalUserInfo);
	if (pGlobalUserItem != NULL)
	{
		//收集用户
		CMD_CS_C_UserOnline  UserOnline;
		ZeroMemory(&UserOnline, sizeof(UserOnline));

		//设置变量
		UserOnline.dwUserID = pDBOLogonSuccess->dwUserID;
		UserOnline.dwGameID = pDBOLogonSuccess->dwGameID;
		StringCchCopy(UserOnline.szNickName, CountArray(UserOnline.szNickName), pDBOLogonSuccess->szNickName);
		StringCchCopy(UserOnline.szUserHeadUrl, CountArray(UserOnline.szUserHeadUrl), pDBOLogonSuccess->szUserFaceUrl);//kk jia
		//用户资料
		UserOnline.wFaceID = pDBOLogonSuccess->wFaceID;
		UserOnline.cbGender = pDBOLogonSuccess->cbGender;
		UserOnline.dwCustomID = pDBOLogonSuccess->dwCustomID;

		//网络信息
		UserOnline.wGateID = pBindParameter->wServiceID;
		UserOnline.dwTokenID = pTokenParameter->dwTokenID;

		//状态信息
		UserOnline.cbShutUpStatus = pDBOLogonSuccess->cbShutUpStatus;
		UserOnline.dwShutUpOverTime = pDBOLogonSuccess->dwShutUpOverTime;		

		//用户上线
		m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_USER_ONLINE, &UserOnline, sizeof(UserOnline), 0);
	}	

	//榜单大哥上线播报
	auto MarqueeNoticeOptionPtr = g_GlobalOptionManager->GetOptionItem<CMarqueeNoticeOptionPtr>(OPTION_MASK_MARQUEE_NOTICE);
	if (MarqueeNoticeOptionPtr)
	{
		//榜单类型
		BYTE cbListType[10] = { 0 };
		auto nTypeCount = MarqueeNoticeOptionPtr->GetListTypeArray(cbListType, CountArray(cbListType));

		for (int i = 0; i < nTypeCount; i++)
		{
			auto pRankingListItem = m_RankingListManager.SearchRankingList(cbListType[i]);
			if (pRankingListItem != NULL)
			{
				auto pListUserItem = pRankingListItem->SearchListUser(pDBOLogonSuccess->dwUserID);
				if (pListUserItem != NULL && pListUserItem->wRankID<=MarqueeNoticeOptionPtr->GetReachRankID())
				{
					//构造对象
					tagNoticeKind_UserLogon NoticeUserLogon;
					ZeroMemory(&NoticeUserLogon, sizeof(NoticeUserLogon));

					//设置变量
					NoticeUserLogon.cbListType = cbListType[i];
					NoticeUserLogon.wRankID = pListUserItem->wRankID;
					StringCchCopy(NoticeUserLogon.szNickName, CountArray(NoticeUserLogon.szNickName), pDBOLogonSuccess->szNickName);

					//发送公告
					if (SendMarqeeNotice(NOTICE_KIND_LOGON, &NoticeUserLogon, sizeof(NoticeUserLogon)))
					{
						break;
					}					
				}
			}
		}
	}

	//绑定用户
	pTokenParameter->pBindUserItem = pGlobalUserItem;
	pTokenParameter->dwUserID = pGlobalUserItem->GetUserID();

	//重入处理
	if (pDBOLogonSuccess->wLockGateID != 0 && pDBOLogonSuccess->wLockLogonID != 0 && pDBOLogonSuccess->dwLockSocketID != 0)
	{
		//查找网关
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pDBOLogonSuccess->wLockGateID);
		if (pGlobalGateItem != NULL)
		{
			//构造结构
			CMD_MB_LogoutNotify LogoutNotify;
			ZeroMemory(&LogoutNotify, sizeof(LogoutNotify));

			//设置变量
			StringCchCopy(LogoutNotify.szDescribeString, CountArray(LogoutNotify.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0007, TEXT("由于您的账号在别处登录，您被迫下线！")));

			//发送数据
			WORD wMsgSize = CountStringBuffer(LogoutNotify.szDescribeString);
			WORD wHeadSize = sizeof(LogoutNotify) - sizeof(LogoutNotify.szDescribeString);			
			SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_MB_LOGON, SUB_MB_LOGOUT_NOTIFY,&LogoutNotify, wHeadSize + wMsgSize,&pDBOLogonSuccess->dwLockSocketID,1);

			//移除令牌
			if (pDBOLogonSuccess->wLockLogonID == m_pServiceOption->wServiceID)
			{				
				pGlobalGateItem->RemoveTokenID(pDBOLogonSuccess->dwLockSocketID);
			}

			//构造结构
			CMD_CS_C_ShutdownSocket ShutdownSocket = {true};			

			//关闭网络
			SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket),&pDBOLogonSuccess->dwLockSocketID,1);
		}		
	}	

	return true;
}

//登录失败
bool CAttemperEngineSink::OnDBMBLogonFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//变量定义
	CMD_MB_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure, sizeof(LogonFailure));
	DBO_MB_LogonFailure* pLogonFailure = (DBO_MB_LogonFailure*)pData;

	//构造数据
	LogonFailure.lResultCode = pLogonFailure->lResultCode;
	StringCchCopy(LogonFailure.szDescribeString, CountArray(LogonFailure.szDescribeString), pLogonFailure->szDescribeString);

	//发送数据
	WORD wStringSize = CountStringBuffer(LogonFailure.szDescribeString);
	WORD wSendSize = sizeof(LogonFailure) - sizeof(LogonFailure.szDescribeString) + wStringSize;
	SendDataToGate(dwContextID, MDM_MB_LOGON, SUB_MB_LOGON_FAILURE, &LogonFailure, wSendSize,&dwTokenID,1);

	//构造结构
	CMD_CS_C_ShutdownSocket ShutdownSocket;
	ShutdownSocket.bForceClose = true;

	//关闭连接
	SendDataToGate(dwContextID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket),&dwTokenID,1);

	return true;
}

//登出结果
bool CAttemperEngineSink::OnDBMBLogoutResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(DBO_MB_LogoutResult));
	if (wDataSize > sizeof(DBO_MB_LogoutResult)) return false;

	//提取数据
	DBO_MB_LogoutResult* pLogoutResult = (DBO_MB_LogoutResult*)pData;

	//广播状态
	if (pLogoutResult->lResultCode == DB_SUCCESS)
	{
		//收集用户
		CMD_CS_C_UserOffline  UserOffline;
		ZeroMemory(&UserOffline, sizeof(UserOffline));

		//设置变量
		UserOffline.dwUserID = pLogoutResult->dwUserID;
		
		//发送消息
		m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_USER_OFFLINE, &UserOffline, sizeof(UserOffline), 0);
	}

	return true;
}

//绑定成功
bool CAttemperEngineSink::OnDBMBBindMPSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//效验参数
	ASSERT(wDataSize <= sizeof(DBO_MB_BindMPSuccess));
	if (wDataSize > sizeof(DBO_MB_BindMPSuccess)) return false;

	//提取数据
	DBO_MB_BindMPSuccess* pBindMPSuccess = (DBO_MB_BindMPSuccess*)pData;

	//变量定义
	CMD_MB_BindMPSuccess BindMPSuccess;
	ZeroMemory(&BindMPSuccess, sizeof(BindMPSuccess));

	//构造数据
	StringCchCopy(BindMPSuccess.szMobilePhone, CountArray(BindMPSuccess.szMobilePhone), pBindMPSuccess->szMobilePhone);
	StringCchCopy(BindMPSuccess.szNoticeString, CountArray(BindMPSuccess.szNoticeString), pBindMPSuccess->szNoticeString);

	//发送数据
	WORD wStringSize = CountStringBuffer(BindMPSuccess.szNoticeString);
	WORD wPacketSize = sizeof(BindMPSuccess) - sizeof(BindMPSuccess.szNoticeString) + wStringSize;
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_BINDMP_SUCCESS, &BindMPSuccess, wPacketSize,&dwTokenID,1);

	return true;
}

//解绑成功
bool CAttemperEngineSink::OnDBMBUnbindMPSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//效验参数
	ASSERT(wDataSize <= sizeof(DBO_MB_UnbindMPSuccess));
	if (wDataSize > sizeof(DBO_MB_UnbindMPSuccess)) return false;

	//提取数据
	DBO_MB_UnbindMPSuccess* pUnbindMPSuccess = (DBO_MB_UnbindMPSuccess*)pData;

	//变量定义
	CMD_MB_UnbindMPSuccess UnbindMPSuccess;
	ZeroMemory(&UnbindMPSuccess, sizeof(UnbindMPSuccess));

	//构造数据
	StringCchCopy(UnbindMPSuccess.szNoticeString, CountArray(UnbindMPSuccess.szNoticeString), pUnbindMPSuccess->szNoticeString);

	//发送数据
	WORD wStringSize = CountStringBuffer(UnbindMPSuccess.szNoticeString);
	WORD wPacketSize = sizeof(UnbindMPSuccess) - sizeof(UnbindMPSuccess.szNoticeString) + wStringSize;
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_UNBINDMP_SUCCESS, &UnbindMPSuccess, wPacketSize,&dwTokenID,1);

	return true;
}

//获取结果
bool CAttemperEngineSink::OnDBMBAcquireACResult(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//效验参数
	ASSERT(wDataSize <= sizeof(DBO_MB_AcquireACResult));
	if (wDataSize > sizeof(DBO_MB_AcquireACResult)) return false;

	//提取数据
	DBO_MB_AcquireACResult* pAcquireACResult = (DBO_MB_AcquireACResult*)pData;

	//获取成功
	if (pAcquireACResult->lResultCode == 0)
	{
		BYTE cbBuffer[1024] = { 0 };
		tagForwardHead* pForwardHead = (tagForwardHead*)cbBuffer;
		ZeroMemory(pForwardHead,sizeof(tagForwardHead));

		//转发头部
		pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
		pForwardHead->wItemCount = 1;
		pForwardHead->ForwardItem[0].wServiceID = 0;
		pForwardHead->ForwardItem[0].wExcludeID = 0;
		pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;

		//构造消息
		CMD_CS_SendAuthCode * pSendAuthCode = (CMD_CS_SendAuthCode *)(pForwardHead + 1);

		//验证信息
		pSendAuthCode->cbCodeKind = pAcquireACResult->cbCodeKind;
		pSendAuthCode->cbAuthKind = pAcquireACResult->cbAuthKind;
		StringCchCopy(pSendAuthCode->szAuthCode, CountArray(pSendAuthCode->szAuthCode), pAcquireACResult->szAuthCode);
		StringCchCopy(pSendAuthCode->szMobileEmail, CountArray(pSendAuthCode->szMobileEmail), pAcquireACResult->szMobileEmail);		

		//发送数据
		WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_SendAuthCode);
		m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_SEND_AUTHCODE, pForwardHead, wPacketSize,0);
	}

	//变量定义
	CMD_MB_AcquireACResult AcquireACResult;
	ZeroMemory(&AcquireACResult, sizeof(AcquireACResult));

	//验证信息
	AcquireACResult.cbAuthKind = pAcquireACResult->cbAuthKind;
	AcquireACResult.dwBindUserID = pAcquireACResult->dwBindUserID;

	//结果信息
	AcquireACResult.lResultCode = pAcquireACResult->lResultCode;
	StringCchCopy(AcquireACResult.szNoticeString, CountArray(AcquireACResult.szNoticeString), pAcquireACResult->szNoticeString);

	//发送数据
	WORD wStringSize = CountStringBuffer(AcquireACResult.szNoticeString);
	WORD wPacketSize = sizeof(AcquireACResult) - sizeof(AcquireACResult.szNoticeString) + wStringSize;
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_ACQUIREAC_RESULT, &AcquireACResult, wPacketSize,&dwTokenID,1);

	return true;
}

//绑定成功
bool CAttemperEngineSink::OnDBMBBindEMailSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//效验参数
	ASSERT(wDataSize <= sizeof(DBO_MB_BindEMailSuccess));
	if (wDataSize > sizeof(DBO_MB_BindEMailSuccess)) return false;

	//提取数据
	DBO_MB_BindEMailSuccess* pBindEMailSuccess = (DBO_MB_BindEMailSuccess*)pData;

	//变量定义
	CMD_MB_BindEMailSuccess BindEMailSuccess;
	ZeroMemory(&BindEMailSuccess, sizeof(BindEMailSuccess));

	//构造数据
	StringCchCopy(BindEMailSuccess.szEMail, CountArray(BindEMailSuccess.szEMail), pBindEMailSuccess->szEMail);
	StringCchCopy(BindEMailSuccess.szNoticeString, CountArray(BindEMailSuccess.szNoticeString), pBindEMailSuccess->szNoticeString);

	//发送数据
	WORD wStringSize = CountStringBuffer(BindEMailSuccess.szNoticeString);
	WORD wPacketSize = sizeof(BindEMailSuccess) - sizeof(BindEMailSuccess.szNoticeString) + wStringSize;
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_BIND_EMAIL_SUCCESS, &BindEMailSuccess, wPacketSize, &dwTokenID, 1);

	return true;
}

//解绑成功
bool CAttemperEngineSink::OnDBMBUnbindEMailSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//效验参数
	ASSERT(wDataSize <= sizeof(DBO_MB_UnbindEMailSuccess));
	if (wDataSize > sizeof(DBO_MB_UnbindEMailSuccess)) return false;

	//提取数据
	DBO_MB_UnbindEMailSuccess* pUnBindEMailSuccess = (DBO_MB_UnbindEMailSuccess*)pData;

	//变量定义
	CMD_MB_UnbindEMailSuccess UnBindEMailSuccess;
	ZeroMemory(&UnBindEMailSuccess, sizeof(UnBindEMailSuccess));

	//构造数据
	StringCchCopy(UnBindEMailSuccess.szNoticeString, CountArray(UnBindEMailSuccess.szNoticeString), pUnBindEMailSuccess->szNoticeString);

	//发送数据
	WORD wStringSize = CountStringBuffer(UnBindEMailSuccess.szNoticeString);
	WORD wPacketSize = sizeof(UnBindEMailSuccess) - sizeof(UnBindEMailSuccess.szNoticeString) + wStringSize;
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_UNBIND_EMAIL_SUCCESS, &UnBindEMailSuccess, wPacketSize, &dwTokenID, 1);

	return true;
}

//背包物品
bool CAttemperEngineSink::OnDBMBUserPackGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pUserPackGoods = (DBO_MB_PackGoods *)pData;

	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//定义变量
	auto pPackGoods= (CMD_CM_PackGoods*)m_cbBuffer;
	ZeroMemory(pPackGoods, sizeof(CMD_CM_PackGoods));

	//设置变量
	pPackGoods->wGoodsCount = pUserPackGoods->wGoodsCount;
	CopyMemory(pPackGoods->GoodsHoldInfo, pUserPackGoods->GoodsHoldInfo, pPackGoods->wGoodsCount * sizeof(pPackGoods->GoodsHoldInfo[0]));

	//发送数据
	WORD wPacketSize = sizeof(CMD_CM_GoodsUpdate) + pPackGoods->wGoodsCount * sizeof(pPackGoods->GoodsHoldInfo[0]);
	SendDataToGate(dwContextID, MDM_CM_SYSTEM, SUB_CM_PACK_GOODS, pPackGoods, wPacketSize,&dwTokenID,1);

	return true;
}

//物品更新
bool CAttemperEngineSink::OnDBMBUserGoodsUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pUserGoodsUpdate = (DBO_MB_GoodsUpdate*)pData;

	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//定义变量
	auto pGoodsUpdate = (CMD_CM_GoodsUpdate*)m_cbBuffer;
	ZeroMemory(pGoodsUpdate, sizeof(CMD_CM_GoodsUpdate));

	//设置变量
	pGoodsUpdate->wGoodsCount = pUserGoodsUpdate->wGoodsCount;
	CopyMemory(pGoodsUpdate->GoodsHoldInfo, pUserGoodsUpdate->GoodsHoldInfo, pGoodsUpdate->wGoodsCount * sizeof(pGoodsUpdate->GoodsHoldInfo[0]));

	//发送数据
	WORD wPacketSize = sizeof(CMD_CM_GoodsUpdate) + pGoodsUpdate->wGoodsCount * sizeof(pGoodsUpdate->GoodsHoldInfo[0]);
	SendDataToGate(dwContextID, MDM_CM_SYSTEM, SUB_CM_GOODS_UPDATE, pGoodsUpdate, wPacketSize,&dwTokenID,1);

	return true;
}

//财富更新
bool CAttemperEngineSink::OnDBMBUserWealthUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pUserWealthUpdate = (DBO_MB_WealthUpdate*)pData;

	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//定义变量
	auto pWealthUpdate = (CMD_CM_WealthUpdate*)m_cbBuffer;
	ZeroMemory(pWealthUpdate, sizeof(CMD_CM_WealthUpdate));

	//设置变量
	pWealthUpdate->wItemCount = pUserWealthUpdate->wItemCount;
	CopyMemory(pWealthUpdate->WealthItem, pUserWealthUpdate->WealthItem, pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]));

	//发送数据
	WORD wPacketSize = sizeof(CMD_CM_WealthUpdate) + pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]);
	SendDataToGate(dwContextID, MDM_CM_SYSTEM, SUB_CM_WEALTH_UPDATE, pWealthUpdate, wPacketSize,&dwTokenID,1);

	return true;
}

//魅力更新
bool CAttemperEngineSink::OnDBMBUserLoveLinessUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
    //效验参数
    ASSERT(wDataSize <= sizeof(DBO_MB_LoveLinessUpdate));
    if (wDataSize > sizeof(DBO_MB_LoveLinessUpdate)) return false;

    //提取数据
    auto pDBOLoveLinessUpdate = (DBO_MB_LoveLinessUpdate*)pData;

    //判断在线
    auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
    if (pTokenParameter == NULL) return true;

    //定义变量
	CMD_CM_LoveLinessUpdate LoveLinessUpdate;
    ZeroMemory(&LoveLinessUpdate, sizeof(LoveLinessUpdate));

    //设置变量
	LoveLinessUpdate.dwLoveLiness = pDBOLoveLinessUpdate->dwLoveLiness;

    //发送数据
    SendDataToGate(dwContextID, MDM_CM_SYSTEM, SUB_CM_LOVELINESS_UPDATE, &LoveLinessUpdate, sizeof(LoveLinessUpdate),&dwTokenID,1);

    return true;
}

//投递消息
bool CAttemperEngineSink::OnDBMBPostMessage(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//提取数据
	DBO_MB_PostMessage* pDBPostMessage = (DBO_MB_PostMessage*)pData;

	//广播状态
	auto pForwardHead = (tagForwardHead*)m_cbBuffer;
	ZeroMemory(pForwardHead, sizeof(tagForwardHead));

	//转发对象
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;

	//转发子项
	pForwardHead->wItemCount = 1;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CHAT;
	pForwardHead->ForwardItem[0].dwHashKeyID = pDBPostMessage->dwHashKeyID;
	pForwardHead->ForwardItem[0].wServiceKind = pDBPostMessage->wServiceKind;	

	//投递消息
	auto pPostMessage = (CMD_CS_PostMessage*)(pForwardHead + 1);
	ZeroMemory(pPostMessage, sizeof(CMD_CS_PostMessage));

	//散列信息
	pPostMessage->dwHashKeyID = pDBPostMessage->dwHashKeyID;

	//拷贝数据
	CopyMemory(pPostMessage+1, pDBPostMessage->cbMessageData, pDBPostMessage->wMessageSize);

	//发送数据
	WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_PostMessage) + pDBPostMessage->wMessageSize;
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_POST_MESSAGE, pForwardHead, wPacketSize, 0);

	return true;
}
//银行查询结果
bool CAttemperEngineSink::OnDBMBInsureQueryResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//
	ASSERT(wDataSize == sizeof(DBR_MB_InsureQueryResult));
	DBR_MB_InsureQueryResult* pInsureResult = (DBR_MB_InsureQueryResult*)pData;

	//
	CMD_MB_InsureQueryResult InsureResult;
	ZeroMemory(&InsureResult, sizeof(CMD_MB_InsureQueryResult));
	InsureResult.cbEnable = pInsureResult->cbEnable;
	CopyMemory(&InsureResult.SaveAndTake, &pInsureResult->SaveAndTake, sizeof(InsureResult.SaveAndTake));
	CopyMemory(&InsureResult.Transfer, &pInsureResult->Transfer, sizeof(InsureResult.Transfer));
	InsureResult.lScore = pInsureResult->lScore;
	InsureResult.lInsureScore = pInsureResult->lInsureScore;

	//发送数据
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_INSURE_QUERY_RESULT, &InsureResult, sizeof(InsureResult), &dwTokenID, 1);
	return true;
};
//银行成功
bool CAttemperEngineSink::OnDBMBInsureSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//
	ASSERT(wDataSize == sizeof(DBR_MB_InsureSuccess));
	DBR_MB_InsureSuccess* pInsure = (DBR_MB_InsureSuccess*)pData;

	//
	CMD_MB_InsureSuccess Insure;
	ZeroMemory(&Insure, sizeof(CMD_MB_InsureSuccess));
	Insure.lScore = pInsure->lScore;
	Insure.lInsureScore = pInsure->lInsureScore;

	//发送数据
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_INSURE_SUCCESS, &Insure, sizeof(Insure), &dwTokenID, 1);
	return true;
}
//银行失败
bool CAttemperEngineSink::OnDBMBInsureFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//
	ASSERT(wDataSize == sizeof(DBR_MB_InsureFailure));
	DBR_MB_InsureFailure* pInsure = (DBR_MB_InsureFailure*)pData;

	//
	CMD_MB_InsureFailure Insure;
	ZeroMemory(&Insure, sizeof(CMD_MB_InsureFailure));

	Insure.dwResultCode = pInsure->dwResultCode;
	CopyMemory(Insure.szDescribeString, pInsure->szDescribeString, sizeof(Insure.szDescribeString));

	//发送数据
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_INSURE_FAILURE, &Insure, sizeof(Insure), &dwTokenID, 1);

	return true;
}
//银行邮件通知
bool CAttemperEngineSink::OnDBMBInsureMailNotify(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//
	ASSERT(wDataSize == sizeof(DBR_MB_InsureMailNotify));
	DBR_MB_InsureMailNotify* pInsureMail = (DBR_MB_InsureMailNotify*)pData;

	//定义变量
	BYTE cbBuffer[256];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//获取对象
	tagForwardHead* pForwardHead = (tagForwardHead*)cbBuffer;

	//设置转发头
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwardHead->dwUserID = pInsureMail->dwUserID;

	//转发信息
	pForwardHead->wItemCount = 1;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;

	//转换对象				
	CMD_CS_MailNotify* pMailNotify = (CMD_CS_MailNotify*)(pForwardHead + 1);

	//设置邮件
	pMailNotify->dwUserID = pInsureMail->dwUserID;
	pMailNotify->dwMailID = pInsureMail->dwMailID;

	//发送数据
	WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_MailNotify);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_MAIL_NOTIFY, cbBuffer, wPacketSize, 0);
	
	return true;
}
//低保查询
bool CAttemperEngineSink::OnDBMBSubsidyQuery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//
	ASSERT(wDataSize == sizeof(DBR_MB_Subsidy));
	DBR_MB_Subsidy* pSubsidy = (DBR_MB_Subsidy*)pData;

	//
	CMD_MB_Subsidy Subsidy;
	ZeroMemory(&Subsidy, sizeof(CMD_MB_Subsidy));

	Subsidy.cbDailyTimes = pSubsidy->cbDailyTimes;
	Subsidy.cbResidualTimes = pSubsidy->cbResidualTimes;
	Subsidy.lConsumeScore = pSubsidy->lConsumeScore;

	//发送数据
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_USER_SUBSIDY, &Subsidy, sizeof(Subsidy), &dwTokenID, 1);

	return true;
};
//操作成功
bool CAttemperEngineSink::OnDBMBOperateSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//提取数据
	DBO_MB_OperateSuccess* pOperateSuccess = (DBO_MB_OperateSuccess*)pData;

	//操作成功
	SendOperateSuccess(dwContextID, dwTokenID, pOperateSuccess->wRequestCmdID, pOperateSuccess->lResultCode, pOperateSuccess->szDescribeString);

	return true;
}

//操作失败
bool CAttemperEngineSink::OnDBMBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//判断在线
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//提取数据
	DBO_MB_OperateFailure* pOperateFailure = (DBO_MB_OperateFailure*)pData;

	//操作失败
	SendOperateFailure(dwContextID,dwTokenID, pOperateFailure->wRequestCmdID, pOperateFailure->lErrorCode, pOperateFailure->szDescribeString);

	return true;	
}
//昵称配置
bool CAttemperEngineSink::OnDBMBNickNameOption(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	CopyMemory(&m_NickNameOption, pData, sizeof(tagNickNameOption));
	return true;
}

//生成验证码
VOID CAttemperEngineSink::MakeAuthCode(WCHAR szAutoCode[], BYTE cbBufferSize)
{
	//校验参数
	ASSERT(cbBufferSize > 0);
	if (cbBufferSize <= 0) return;

	//字符集定义
	LPCTSTR szCharacterSet = L"0123456789";

	//变量定义
	LPCTSTR pszCharacter = NULL;
	WORD wStringLen = lstrlen(szCharacterSet);

	//生成验证码
	for (int i = 0; i < cbBufferSize - 1; i++)
	{
		szAutoCode[i] = szCharacterSet[rand() % wStringLen];
	}

	//StringCchCopy(szAutoCode, cbBufferSize, TEXT("123456"));

	//添加结束符
	szAutoCode[cbBufferSize - 1] = 0;
}

//绑定参数
tagBindParameter* CAttemperEngineSink::GetBindParameter(DWORD dwSocketID)
{
	//无效连接
	WORD wBindIndex = LOWORD(dwSocketID);	
	if (wBindIndex == INVALID_WORD_INDEX) return NULL;

	//常规连接
	if (wBindIndex < m_pServiceOption->wMaxConnect)
	{
		auto pBindParameter = m_pBindParameter + wBindIndex;

		//校验网络
		if (pBindParameter->dwSocketID != dwSocketID)
		{
			return NULL;
		}

		return pBindParameter;
	}	

	//错误断言
	ASSERT(FALSE);

	return NULL;
}

//令牌参数
tagTokenParameter* CAttemperEngineSink::GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID)
{
	//绑定参数
	auto pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return NULL;

	//转发模式
	if (pBindParameter->wServiceModule == SERVICE_MODULE_GATE)
	{
		//查询网关
		CGlobalGateItem* pGlobalGateItem = pBindParameter->pGlobalGateItem;
		if (pGlobalGateItem == NULL) return NULL;

		//查询用户
		tagTokenParameter* pTokenParameter = pGlobalGateItem->SearchTokenID(dwTokenID);
		if (pTokenParameter == NULL || pTokenParameter->dwTokenID != dwTokenID) return NULL;

		return pTokenParameter;
	}

	return NULL;
}

//发送数据
bool CAttemperEngineSink::SendDataToGate(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, DWORD* pdwTokenID, WORD wTokenCount)
{
	//发送数据
	return m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(wMainCmdID, SERVICE_MODULE_LOGON), wSubCmdID, pdwTokenID, wTokenCount);
}

//发送数据
bool CAttemperEngineSink::SendDataToGate(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD* pdwTokenID, WORD wTokenCount)
{
	//发送数据
	return m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(wMainCmdID, SERVICE_MODULE_LOGON), wSubCmdID, pData, wDataSize, pdwTokenID, wTokenCount);
}

//群发数据
bool CAttemperEngineSink::SendBatchData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	//变量定义
	CGlobalGateItem* pGlobalGateItem = NULL;

	//变量定义
	DWORD* pdwTokenIDArray = NULL;
	WORD wCellCount = 0, wSendCount = 0, wTotalCount = 0;

	//查找房间
	do
	{
		//枚举网关 
		pGlobalGateItem = m_GlobalItemManager.EnumGateItem(pGlobalGateItem);
		if (pGlobalGateItem == NULL) break;

		//查找容器
		if (pGlobalGateItem->GetTokenCount() == 0) continue;

		//设置变量		
		wSendCount = 0;
		wCellCount = pGlobalGateItem->GetTokenCount();
		wTotalCount = pGlobalGateItem->GetTokenCount();
		pdwTokenIDArray = pGlobalGateItem->m_TokenIDArray.GetData();

		//切分数量
		while ((sizeof(WORD) + wCellCount * sizeof(DWORD) + wDataSize) > SOCKET_PACKET)
		{
			wCellCount = wCellCount / 2;
		}

		//调整数量
		if (wCellCount > TOKEN_COUNT) wCellCount = TOKEN_COUNT;

		//循环发送
		while (wSendCount < wTotalCount)
		{
			//调整变量
			if (wTotalCount - wSendCount < wCellCount)
			{
				wCellCount = wTotalCount - wSendCount;
			}

			//发送数据
			SendDataToGate(pGlobalGateItem->GetSocketID(), wMainCmdID, wSubCmdID, pData, wDataSize, pdwTokenIDArray + wSendCount, wCellCount);

			//设置数据			
			wSendCount += wCellCount;
		}

	} while (true);

    return true;
}

//当前版本
tagModuleVersion* CAttemperEngineSink::GetCurrentVersion(WORD wModuleID, DWORD dwDeviceKind)
{
	//特殊版本	
	for (INT_PTR i = 0; i < m_ModuleVersionActive.GetCount(); i++)
	{
		//判断类型
		if (m_ModuleVersionActive[i].IsCurrent == false) continue;
		if (m_ModuleVersionActive[i].wModuleID != wModuleID) continue;
		if (m_ModuleVersionActive[i].dwDeviceKind != dwDeviceKind) continue;

		return &m_ModuleVersionActive[i];
	}

	return NULL;
}

//特殊版本
tagModuleVersion* CAttemperEngineSink::GetSpecialVersion(WORD wModuleID, DWORD dwDeviceKind, DWORD dwVersion)
{
	//特殊版本	
	for (INT_PTR i = 0; i < m_ModuleVersionActive.GetCount(); i++)
	{
		//判断类型
		if (m_ModuleVersionActive[i].IsCurrent == true) continue;
		if (m_ModuleVersionActive[i].wModuleID != wModuleID) continue;
		if (m_ModuleVersionActive[i].dwVersion != dwVersion) continue;
		if (m_ModuleVersionActive[i].dwDeviceKind != dwDeviceKind) continue;

		return &m_ModuleVersionActive[i];
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////


