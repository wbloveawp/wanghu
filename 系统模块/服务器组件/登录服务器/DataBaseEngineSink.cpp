#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBaseEngineSink.h"
#include <afxinet.h>

//////////////////////////////////////////////////////////////////////////
//添加好友
const string script_append_friend =
"local key1=KEYS[1];"
"local key2=KEYS[2];"
"local field1=ARGV[1];"
"local field2=ARGV[2];"
"local extras1=ARGV[3];"
"local extras2=ARGV[4];"
"redis.call('HSET',key1, field1, extras1);"
"redis.call('HSET',key2, field2, extras2);"
"return 1";

//删除好友
const string script_delete_friend =
"local key1=KEYS[1];"
"local key2=KEYS[2];"
"local field1=ARGV[1];"
"local field2=ARGV[2];"
"redis.call('HDEL',key1, field1);"
"redis.call('HDEL',key2, field2);"
"return 1";

//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//组件变量
	m_pServiceOption=NULL;
	m_pInitParameter=NULL;	
	m_pIDataBaseEngineEvent=NULL;

	//缓存变量
	m_pUserClient = NULL;
	m_pTokenClient = NULL;
	m_pFriendClient = NULL;
	m_pMasterClient = NULL;

	//辅助变量
	ZeroMemory(&m_GPLogonFailure,sizeof(m_GPLogonFailure));
	ZeroMemory(&m_GPLogonSuccess,sizeof(m_GPLogonSuccess));
	ZeroMemory(&m_UserIndividual,sizeof(m_UserIndividual));

	return;
}

//析构函数
CDataBaseEngineSink::~CDataBaseEngineSink()
{
	//释放对象
	if (m_pUserClient) SafeDelete(m_pUserClient);
	if (m_pTokenClient) SafeDelete(m_pTokenClient);
	if (m_pFriendClient) SafeDelete(m_pFriendClient);
	if (m_pMasterClient) SafeDelete(m_pMasterClient);
}

//接口查询
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//创建对象
	if ((m_AccountsDBModule.GetInterface()==NULL)&&(m_AccountsDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_PlatformDBModule.GetInterface()==NULL)&&(m_PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}	

	try
	{
		//变量定义
		tagDataBaseParameter * pAccountsDBParameter=&m_pInitParameter->m_AccountsDBParameter;
		tagDataBaseParameter * pPlatformDBParameter=&m_pInitParameter->m_PlatformDBParameter;	 

		//设置连接
		m_AccountsDBModule->SetConnectionInfo(pAccountsDBParameter->dwDataBaseAddr,pAccountsDBParameter->wDataBasePort,
		pAccountsDBParameter->szDataBaseName,pAccountsDBParameter->szDataBaseUser,pAccountsDBParameter->szDataBasePass);

		//设置连接
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->dwDataBaseAddr, pPlatformDBParameter->wDataBasePort,
		pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);
		
		//发起连接
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//发起连接
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());	

		//连接选项
		auto pUserConnOption = &m_pInitParameter->m_UserConnOption;
		auto pTokenConnOption = &m_pInitParameter->m_TokenConnOption;
		auto pFriendConnOption = &m_pInitParameter->m_FriendConnOption;
		auto pMasterConnOption = &m_pInitParameter->m_MasterConnOption;

		//变量定义
		ConnectionOptions ConnectionOpts;
		ConnectionPoolOptions ConnectionPoolopts;

		//超时设置
		ConnectionOpts.socket_timeout = std::chrono::milliseconds(1000);
		ConnectionOpts.connect_timeout = std::chrono::milliseconds(1000);

		//用户连接
		ConnectionOpts.db = pUserConnOption->wDB;
		ConnectionOpts.port = pUserConnOption->wPort;
		ConnectionOpts.host = string(CT2CA(pUserConnOption->szHost).m_psz);
		ConnectionOpts.user = string(CT2CA(pUserConnOption->szUser).m_psz);
		ConnectionOpts.password = string(CT2CA(pUserConnOption->szPassword).m_psz);		

		//创建对象
		m_pUserClient = new Redis(ConnectionOpts);
		if (m_pUserClient == NULL) throw TEXT("UserClient创建失败");

		//令牌连接
		ConnectionOpts.db = pTokenConnOption->wDB;
		ConnectionOpts.port = pTokenConnOption->wPort;
		ConnectionOpts.host = string(CT2CA(pTokenConnOption->szHost).m_psz);
		ConnectionOpts.user = string(CT2CA(pTokenConnOption->szUser).m_psz);
		ConnectionOpts.password = string(CT2CA(pTokenConnOption->szPassword).m_psz);

		//创建对象		
		m_pTokenClient = new Redis(ConnectionOpts);
		if (m_pTokenClient == NULL) throw TEXT("TokenClient创建失败");

		////好友连接
		//ConnectionOpts.db = pFriendConnOption->wDB;
		//ConnectionOpts.port = pFriendConnOption->wPort;
		//ConnectionOpts.host = string(CT2CA(pFriendConnOption->szHost).m_psz);
		//ConnectionOpts.user = string(CT2CA(pFriendConnOption->szUser).m_psz);
		//ConnectionOpts.password = string(CT2CA(pFriendConnOption->szPassword).m_psz);

		////创建对象		
		//m_pFriendClient = new Redis(ConnectionOpts);
		//if (m_pFriendClient == NULL) throw TEXT("FriendClient创建失败");	

		//管理连接
		ConnectionOpts.db = pMasterConnOption->wDB;
		ConnectionOpts.port = pMasterConnOption->wPort;
		ConnectionOpts.host = string(CT2CA(pMasterConnOption->szHost).m_psz);
		ConnectionOpts.user = string(CT2CA(pMasterConnOption->szUser).m_psz);
		ConnectionOpts.password = string(CT2CA(pMasterConnOption->szPassword).m_psz);

		//创建对象		
		m_pMasterClient = new Redis(ConnectionOpts);
		if (m_pMasterClient == NULL) throw TEXT("MasterClient创建失败");

		//加载脚本
		if (m_pFriendClient != NULL)
		{
			m_sScriptDeleteFriend = m_pFriendClient->script_load(script_delete_friend);
			m_sScriptAppendFriend = m_pFriendClient->script_load(script_append_friend);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}
	catch (std::exception ex)
	{
		CTraceService::TraceString(CA2CT(ex.what()).m_psz, TraceLevel_Exception);

		return false;
	}

	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	//服务单元
	ASSERT (CServiceUnits::g_pServiceUnits!=NULL);
	auto pServiceUnits = CServiceUnits::g_pServiceUnits;

	//清理在线信息
	if (&pServiceUnits->m_DataBaseEngineSink[0] == this)
	{
		CleanOnlineInfo(pServiceUnits->m_ServiceOption.wServiceID);
	}

	//设置对象
	m_AccountsDBAide.SetDataBase(NULL);
	m_PlatformDBAide.SetDataBase(NULL);	

	//关闭连接
	if (m_AccountsDBModule.GetInterface()!=NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
	}

	//关闭连接
	if (m_PlatformDBModule.GetInterface()!=NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}	

	//删除对象
	SafeDelete(m_pUserClient);
	SafeDelete(m_pTokenClient);
	SafeDelete(m_pFriendClient);
	SafeDelete(m_pMasterClient);

	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	//辅助变量
	ZeroMemory(&m_GPLogonFailure,sizeof(m_GPLogonFailure));
	ZeroMemory(&m_GPLogonSuccess,sizeof(m_GPLogonSuccess));
	ZeroMemory(&m_UserIndividual,sizeof(m_UserIndividual));

	return true;
}

//时间事件
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//控制事件
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//请求事件
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	CString strMessage;
	strMessage.Format(TEXT("ThreadID=%d,RequestID=%d"), ::GetCurrentThreadId(),wRequestID);
	CTraceService::TraceString(strMessage, TraceLevel_Debug);

	switch (wRequestID)
	{
	case DBR_GP_LOAD_CONFIG:			//加载配置
	{
		return OnRequestLoadConfig(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_GP_LOAD_RANKING_LIST:		//加载榜单
	{
		return OnRequestLoadRankingList(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_RESET_USER_DATA:		//重置数据
	{
		return OnRequestResetUserData(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_ONLINE_COUNT_INFO:		//在线信息
	{
		return OnRequestOnLineCountInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_GP_LOGON_ACCOUNTS:			//帐号登录
	{
		return OnRequestPCLogonAccounts(dwContextID,dwTokenID,pData,wDataSize, pdwControlCode);
	}
	case DBR_GP_BIND_MACHINE:			//锁定机器
	{
		return OnRequestPCBindMachine(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_UN_BIND_MACHINE:		//解锁机器
	{
		return OnRequestPCUnbindMachine(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_REGISTER_ACCOUNTS:		//注册帐号
	{
		return OnRequestPCRegisterAccounts(dwContextID,dwTokenID,pData,wDataSize, pdwControlCode);
	}
	case DBR_GP_QUERY_INDIVIDUAL:		//查询资料
	{
		return OnRequestPCQueryIndividual(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_MODIFY_ACCOUNTS:		//修改帐号
	{
		return OnRequestPCModifyAccounts(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_MODIFY_LOGON_PASS:		//修改密码
	{
		return OnRequestPCModifyLogonPass(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_MODIFY_INSURE_PASS:		//修改密码
	{
		return OnRequestPCModifyInsurePass(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_MODIFY_INDIVIDUAL:		//修改资料
	{
		return OnRequestPCModifyIndividual(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_GP_QUERY_GAME_CONFIG:		//查询配置
	{
		return OnRequestPCQueryGameConfig(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_UPDATE_BASIC_CONFIG:	//更新配置
	{
		return OnRequestPCUpdateBasicConfig(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_UPDATE_WEIGHT_CONFIG:	//更新配置
	{
		return OnRequestPCUpdateWeightConfig(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_QUERY_PARAMETER:		//获取参数
	{
		return OnRequestPCQueryParameter(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_APPEND_PARAMETER:		//添加参数
	{
		return OnRequestPCAppendParameter(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_MODIFY_PARAMETER:		//修改参数
	{
		return OnRequestPCModifyParameter(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_DELETE_PARAMETER:		//删除参数
	{
		return OnRequestPCDeleteParameter(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_QUERY_STOCK_INFO:		//查询库存
	{
		return OnRequestPCQueryStockInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_APPEND_STOCK_INFO:		//添加库存
	{
		return OnRequestPCAppendStockInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_MODIFY_STOCK_INFO:		//修改库存
	{
		return OnRequestPCModifyStockInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_DELETE_STOCK_INFO:		//删除库存
	{
		return OnRequestPCDeleteStockInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGON_VISITOR:			//游客登录
	{
		return OnRequestMBLogonVisitor(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGON_ACCOUNTS:			//帐号登录
	{
		return OnRequestMBLogonAccounts(dwContextID,dwTokenID,pData,wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGON_SECRETKEY:		//秘钥登录
	{
		return OnRequestMBLogonSecretKey(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGON_MOBILECODE:		//手机验证码
	{
		return OnRequestMBLogonMobileCode(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGON_THIRDPARTY:		//第三方登录
	{
		return OnRequestMBLogonThirdParty(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGON_MOBILEPHONE:		//手机登录
	{
		return OnRequestMBLogonMobilePhone(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_REGISTER_EMAIL:			//邮箱注册
	{
		return OnRequestMBRegisterEMail(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_REGISTER_ACCOUNTS:		//注册帐号
	{
		return OnRequestMBRegisterAccounts(dwContextID,dwTokenID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_MB_REGISTER_MOBILEPHONE:	//手机注册
	{
		return OnRequestMBRegisterMobilePhone(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGON_EMAIL:			//邮箱登录
	{
		return OnRequestMBLogonEMail(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGON_EMAILCODE:		//邮箱验证码登录
	{
		return OnRequestMBLogonEMailCode(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGOUT_ACCOUNTS:		//登出账号
	{
		return OnRequestMBLogoutAccounts(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_QUERY_WEALTH:			//查询财富
	{
		return OnRequestMBQueryWealth(dwContextID,dwTokenID,pData,wDataSize, pdwControlCode);
	}
	case DBR_MB_QUERY_RANKINGINFO:		//查询排位
	{
		return OnRequestMBQueryRankingInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_QUERY_LOVELINESS:		//查询魅力
	{
		return OnRequestMBQueryLoveLiness(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LIKE_PLAYER:			//点赞玩家
	{
		return OnRequestMBLikePlayer(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_MODIFY_LOGON_PASS:		//修改密码
	{
		return OnRequestMBModifyLogonPass(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_MODIFY_INDIVIDUAL:		//修改资料
	{
		return OnRequestMBModifyIndividual(dwContextID,dwTokenID,pData,wDataSize, pdwControlCode);
	}	
	case DBR_MB_MODIFY_SYSTEM_FACE:		//修改头像
	{
		return OnRequestMBModifySystemFace(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_MODIFY_CUSTOM_FACE:		//修改头像
	{
		return OnRequestMBModifyCustomFace(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_MB_EXCHANGE_MALLGOODS:		//兑换商品
	{
		return OnRequestMBExchangeMallGoods(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_EXCHANGE_GIFTBAGCODE:	//兑换礼包码	
	{
		return OnRequestMBExchangeGiftbagCode(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_ACQUIRE_AUTHCODE:		//获取验证码
	{
		return OnRequestMBAcquireAuthCode(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_MB_BIND_MOBILEPHONE:		//绑定手机号
	{
		return OnRequestMBBindMobilePhone(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_UNBIND_MOBILEPHONE:		//解绑手机号
	{
		return OnRequestMBUnbindMobilePhone(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_RESET_LOGON_PASS:		//重置密码
	{
		return OnRequestMBResetLogonPasswd(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_MB_BIND_EMAIL:				//绑定邮箱
	{
		return OnRequestMBBindEMail(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_UNBIND_EMAIL:			//解绑邮箱
	{
		return OnRequestMBUnbindEMail(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_MB_REQUEST_CHECKIN:		//请求签到
	{
		return OnRequestMBRequestCheckIn(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_BIND_INVITE_CODE:		//绑定邀请码
	{
        return OnRequestMBBindInviteCode(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
    }
	case DBR_MB_SUBMIT_INFORM_INFO:		//提交投诉
	{
		return OnRequestMBSubmitInformInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_BIND_EXTERNAL_ACCOUNT:	//绑定账号
	{
		return OnRequestMBBindExternalAccount(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_SEND_GIFT:				//赠送礼物
	{
		return OnRequestMBSendGift(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
    case DBR_MB_LOAD_GIFT:				//加载礼物
    {
        return OnRequestMBLoadGift(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
    }
	case DBR_MB_GIFT_STATUS_UPDATE:		//状态更新
	{
        return OnRequestMBGiftStatusUpdate(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_WEALTH_TRANSFER:		//货币转账
	{
		return OnRequestMBWealthTranser(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_DRAW_COMMISSION:		//提取佣金
	{
		return OnRequestMBDrawCommission(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_QUERY_TURNTABLE_LOTTERY: //查询转盘
	{
		return OnRequestMBQueryTurntableLottery(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_REQUEST_TURNTABLE_LOTTERY: //请求抽奖
	{
		return OnRequestMBRequestTurntableLottery(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_REAL_AUTH: //实名认证
	{
		return OnRequestMBRequestRealAuthent(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_INSURE_QUERY: //银行查询
	{
		return OnRequestMBRequestInsureQuery(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_INSURE_ENABLE://银行开通
	{
		return OnRequestMBRequestInsureEnable(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_INSURE_SAVE: //银行存储
	{
		return OnRequestMBRequestInsureSave(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_INSURE_TAKE: //银行提取
	{
		return OnRequestMBRequestInsureTake(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_INSURE_TRANSFER: //银行转账
	{
		return OnRequestMBRequestInsureTransfer(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_INSURE_RESET_PASSWORD://银行密码重置
	{
		return OnRequestMBInsureResetPassword(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_QUERY_SUBSIDY://低保查询
	{
		return OnRequestMBQuerySubsidy(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	}

	return false;
}

//帐号登录
bool CDataBaseEngineSink::OnRequestPCLogonAccounts(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_LogonByAccounts));
		if (wDataSize!=sizeof(DBR_GP_LogonByAccounts)) return false;

		//请求处理
		DBR_GP_LogonByAccounts * pLogonByAccounts=(DBR_GP_LogonByAccounts *)pData;

		//执行判断
		tagTokenParameter * pTokenParameter =(tagTokenParameter *)pLogonByAccounts->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID)
		{
			TCHAR szBuffer[128]=TEXT("");
			_sntprintf_s(szBuffer,CountArray(szBuffer),TEXT("OnRequestPCLogonAccounts 投递用时：%ld 毫秒，连接用时：%lld 秒 放弃执行"),
				GetTickCount()-pLogonByAccounts->dwPostTime,time(NULL)- pTokenParameter->dwActiveTime);
			CTraceService::TraceString(szBuffer,TraceLevel_Exception);
			return true;
		}

		//变量定义
		LONG lReturnValue=DB_SUCCESS;
		DWORD dwNowTick=GetTickCount();

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pLogonByAccounts->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//检查权限
		if(pLogonByAccounts->dwCheckUserRight!=0)
		{
			//构造参数
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pLogonByAccounts->szAccounts);
			m_AccountsDBAide.AddParameter(TEXT("@dwcheckRight"),pLogonByAccounts->dwCheckUserRight);
			m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_GPLogonSuccess.szDescribeString,sizeof(m_GPLogonSuccess.szDescribeString),adParamOutput);

			//执行过程
			lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CheckUserRight"),false);
		}

		//验证帐号
		if(lReturnValue==DB_SUCCESS)
		{
			//构造参数
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonByAccounts->wGateID);
			m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonByAccounts->wLogonID);
			m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
			m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pLogonByAccounts->szAccounts);
			m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonByAccounts->szPassword);			
			m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
			m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"),pLogonByAccounts->dwPlazaVersion);
			m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonByAccounts->szMachineID);				
			m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_GPLogonSuccess.szDescribeString,sizeof(m_GPLogonSuccess.szDescribeString),adParamOutput);
			//m_AccountsDBAide.AddParameter(TEXT("@dwLogonServerID"),pLogonByAccounts->dwLogonServerID);

			//执行查询
			lReturnValue=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyAccounts"),true);
		}
	
		//设置数据
		m_GPLogonSuccess.dwCheckUserRight=pLogonByAccounts->dwCheckUserRight;
		StringCchCopy(m_GPLogonSuccess.szPassword,CountArray(m_GPLogonSuccess.szPassword), pLogonByAccounts->szPassword);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,dwTokenID,lReturnValue,CW2CT(DBVarValue.bstrVal).m_psz,false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误处理
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//错误处理
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004,L"由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),true);

		return false;
	}

	return true;
}

//注册处理
bool CDataBaseEngineSink::OnRequestPCRegisterAccounts(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_RegisterAccounts));
		if (wDataSize!=sizeof(DBR_GP_RegisterAccounts)) return false;

		//请求处理
		DBR_GP_RegisterAccounts * pRegisterAccounts=(DBR_GP_RegisterAccounts *)pData;

		//执行判断
		tagTokenParameter * pTokenParameter = (tagTokenParameter *)pRegisterAccounts->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID)
		{
			CTraceService::TraceString(TEXT("OnRequestPCRegisterAccounts 放弃执行"),TraceLevel_Exception);
			return true;
		}

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pRegisterAccounts->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pRegisterAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pRegisterAccounts->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pRegisterAccounts->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strBankPass"),pRegisterAccounts->szBankPass);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pRegisterAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbAge"),pRegisterAccounts->cbAge);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pRegisterAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strProvince"),pRegisterAccounts->szProvince);
		m_AccountsDBAide.AddParameter(TEXT("@strCity"),pRegisterAccounts->szCity);
		m_AccountsDBAide.AddParameter(TEXT("@strArea"),pRegisterAccounts->szArea);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"),pRegisterAccounts->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pRegisterAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strValidMachineSerial"),pRegisterAccounts->szValidComputerID);
		m_AccountsDBAide.AddParameter(TEXT("@cbPassportType"),pRegisterAccounts->cbPassportType);
		m_AccountsDBAide.AddParameter(TEXT("@strPassportID"),pRegisterAccounts->szPassportID);
		m_AccountsDBAide.AddParameter(TEXT("@strSpreader"),pRegisterAccounts->szSpreader);		
		m_AccountsDBAide.AddParameter(TEXT("@dwLogonServerID"),pRegisterAccounts->dwLogonServerID);
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_GPLogonSuccess.szDescribeString,sizeof(m_GPLogonSuccess.szDescribeString),adParamOutput);

		//执行查询
		LONG lReturnValue=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RegisterAccounts"),true);

		//用户密码
		StringCchCopy(m_GPLogonSuccess.szPassword,CountArray(m_GPLogonSuccess.szPassword), pRegisterAccounts->szPassword);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lReturnValue,CW2CT(DBVarValue.bstrVal).m_psz,false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误处理
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//错误处理
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004,L"由于数据库操作异常，请您稍后重试或选择另一服务器登录！"),true);

		//请求处理
		DBR_GP_RegisterAccounts * pRegisterAccounts=(DBR_GP_RegisterAccounts *)pData;

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pRegisterAccounts->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//注册信息
		CString strTrace;
		strTrace.Format(TEXT("Accounts=%s, NickName=%s, Password=%s, BankPass=%s, FaceID=%d, Age=%d, Gender=%d, Province=%s, City=%s, Area=%s, ")
			TEXT("ClientIP=%s, PlazaVersion=%d, ComputerID=%s, @PassportType=%d, PassportID=%s, Spreader=%s"), 
			pRegisterAccounts->szAccounts,pRegisterAccounts->szNickName,
			pRegisterAccounts->szPassword,pRegisterAccounts->szBankPass,
			pRegisterAccounts->wFaceID,pRegisterAccounts->cbAge, pRegisterAccounts->cbGender,pRegisterAccounts->szProvince,
			pRegisterAccounts->szCity,pRegisterAccounts->szArea, szClientIP,
			pRegisterAccounts->dwPlazaVersion,pRegisterAccounts->szMachineID,
			pRegisterAccounts->cbPassportType,pRegisterAccounts->szPassportID,pRegisterAccounts->szSpreader);

		//输出错误
		CTraceService::TraceString(strTrace,TraceLevel_Exception);

		return false;
	}

	return true;
}

//修改帐号
bool CDataBaseEngineSink::OnRequestPCModifyAccounts(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyAccounts));
		if (wDataSize!=sizeof(DBR_GP_ModifyAccounts)) return false;

		//请求处理
		DBR_GP_ModifyAccounts * pModifyAccounts=(DBR_GP_ModifyAccounts *)pData;

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pModifyAccounts->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyAccounts->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyAccounts->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pModifyAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strUnderWrite"),pModifyAccounts->szUnderWrite);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pModifyAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbAge"),pModifyAccounts->cbAge);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pModifyAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strProvince"),pModifyAccounts->szProvince);
		m_AccountsDBAide.AddParameter(TEXT("@strCity"),pModifyAccounts->szCity);
		m_AccountsDBAide.AddParameter(TEXT("@strArea"),pModifyAccounts->szArea);
		m_AccountsDBAide.AddParameter(TEXT("@bOldVersion"),pModifyAccounts->bOldVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyAccountsInfo"),false)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString,CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,dwTokenID,&OperateSuccess,wSendSize);
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateFailure.lErrorCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString,CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"由于系统繁忙，请稍后再试！"));

		//发送结果
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wSendSize);

		return false;
	}

	return true;
}

//修改密码
bool CDataBaseEngineSink::OnRequestPCModifyLogonPass(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyLogonPass));
		if (wDataSize!=sizeof(DBR_GP_ModifyLogonPass)) return false;

		//请求处理
		DBR_GP_ModifyLogonPass * pModifyLogonPass=(DBR_GP_ModifyLogonPass *)pData;

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pModifyLogonPass->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyLogonPass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"),pModifyLogonPass->szScrPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"),pModifyLogonPass->szDesPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyLogonPassword"),false)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString,CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,dwTokenID,&OperateSuccess,wSendSize);
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateFailure.lErrorCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString,CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode=pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"由于系统繁忙，请稍后再试！"));

		//发送结果
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wSendSize);

		return false;
	}

	return true;
}

//修改密码
bool CDataBaseEngineSink::OnRequestPCModifyInsurePass(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyInsurePass));
		if (wDataSize!=sizeof(DBR_GP_ModifyInsurePass)) return false;

		//请求处理
		DBR_GP_ModifyInsurePass * pModifyInsurePass=(DBR_GP_ModifyInsurePass *)pData;

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pModifyInsurePass->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyInsurePass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"),pModifyInsurePass->szScrPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"),pModifyInsurePass->szDesPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyInsurePassword"),false)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString,CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,dwTokenID,&OperateSuccess,wSendSize);
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateFailure.lErrorCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString,CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode=pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"由于系统繁忙，请稍后再试！"));

		//发送结果
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID, dwTokenID, &OperateFailure,wSendSize);

		return false;
	}

	return true;
}

//修改资料
bool CDataBaseEngineSink::OnRequestPCModifyIndividual(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyIndividual));
		if (wDataSize!=sizeof(DBR_GP_ModifyIndividual)) return false;

		//请求处理
		DBR_GP_ModifyIndividual * pModifyIndividual=(DBR_GP_ModifyIndividual *)pData;

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pModifyIndividual->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyIndividual->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyIndividual->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"),pModifyIndividual->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@strQQ"),pModifyIndividual->szQQ);
		m_AccountsDBAide.AddParameter(TEXT("@strEMail"),pModifyIndividual->szEMail);
		m_AccountsDBAide.AddParameter(TEXT("@strSeatPhone"),pModifyIndividual->szSeatPhone);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pModifyIndividual->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strDwellingPlace"),pModifyIndividual->szDwellingPlace);
		m_AccountsDBAide.AddParameter(TEXT("@strUserNote"),pModifyIndividual->szUserNote);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyUserIndividual"),false)==DB_SUCCESS)
		{
			//变量定义
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString,CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID, dwTokenID, &OperateSuccess,wSendSize);
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateFailure.lErrorCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString,CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID, dwTokenID, &OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode=pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString,CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"由于系统繁忙，请稍后再试！"));

		//发送结果
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID, dwTokenID, &OperateFailure,wSendSize);

		return false;
	}

	return true;
}

//查询资料
bool CDataBaseEngineSink::OnRequestPCQueryIndividual(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GP_QueryIndividual));
		if (wDataSize != sizeof(DBR_GP_QueryIndividual)) return false;

		//请求处理
		DBR_GP_QueryIndividual* pQueryIndividual = (DBR_GP_QueryIndividual*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pQueryIndividual->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pQueryIndividual->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserIndividual"), true) == DB_SUCCESS)
		{
			//用户信息
			m_UserIndividual.dwUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_AccountsDBAide.GetValue_String(TEXT("UserNote"), m_UserIndividual.szUserNote, CountArray(m_UserIndividual.szUserNote));
			m_AccountsDBAide.GetValue_String(TEXT("Compellation"), m_UserIndividual.szCompellation, CountArray(m_UserIndividual.szCompellation));

			//电话号码
			m_AccountsDBAide.GetValue_String(TEXT("SeatPhone"), m_UserIndividual.szSeatPhone, CountArray(m_UserIndividual.szSeatPhone));
			m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"), m_UserIndividual.szMobilePhone, CountArray(m_UserIndividual.szMobilePhone));

			//联系资料
			m_AccountsDBAide.GetValue_String(TEXT("QQ"), m_UserIndividual.szQQ, CountArray(m_UserIndividual.szQQ));
			m_AccountsDBAide.GetValue_String(TEXT("EMail"), m_UserIndividual.szEMail, CountArray(m_UserIndividual.szEMail));
			m_AccountsDBAide.GetValue_String(TEXT("DwellingPlace"), m_UserIndividual.szDwellingPlace, CountArray(m_UserIndividual.szDwellingPlace));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INDIVIDUAL, dwContextID, dwTokenID, &m_UserIndividual, sizeof(m_UserIndividual));
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"由于系统繁忙，请稍后再试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//锁定机器
bool CDataBaseEngineSink::OnRequestPCBindMachine(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GP_BindMachine));
		if (wDataSize != sizeof(DBR_GP_BindMachine)) return false;

		//请求处理
		DBR_GP_BindMachine* pBindMachine = (DBR_GP_BindMachine*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pBindMachine->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pBindMachine->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pBindMachine->szMachineID);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_BindMachine"), false);

		//变量定义
		DBO_GP_BindMachineResult BindMachineResult = { 0 };
		BindMachineResult.lResultCode = lReturnValue;

		//获取信息
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		StringCchCopy(BindMachineResult.szDescribeString, CountArray(BindMachineResult.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

		//发送结果
		WORD wStringSize = CountStringBuffer(BindMachineResult.szDescribeString);
		WORD wSendSize = sizeof(BindMachineResult) - sizeof(BindMachineResult.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BIND_MANCHINE_RESULT, dwContextID, dwTokenID, &BindMachineResult, wSendSize);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GP_BindMachineResult BindMachineResult = { 0 };

		//设置变量
		BindMachineResult.lResultCode = pIException->GetExceptionResult();

		//获取信息
		StringCchCopy(BindMachineResult.szDescribeString, CountArray(BindMachineResult.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(BindMachineResult.szDescribeString);
		WORD wSendSize = sizeof(BindMachineResult) - sizeof(BindMachineResult.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BIND_MANCHINE_RESULT, dwContextID, dwTokenID, &BindMachineResult, wSendSize);

		return false;
	}

	return true;
}

//解锁机器
bool CDataBaseEngineSink::OnRequestPCUnbindMachine(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GP_BindMachine));
		if (wDataSize != sizeof(DBR_GP_BindMachine)) return false;

		//请求处理
		DBR_GP_UnbindMachine* pUnBindMachine = (DBR_GP_UnbindMachine*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUnBindMachine->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pUnBindMachine->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pUnBindMachine->szMachineID);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UnBindMachine"), false);

		//变量定义
		DBO_GP_BindMachineResult BindMachineResult = { 0 };
		BindMachineResult.lResultCode = lReturnValue;

		if (lReturnValue != DB_SUCCESS)
		{
			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
			StringCchCopy(BindMachineResult.szDescribeString, CountArray(BindMachineResult.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);
		}

		//发送结果
		WORD wStringSize = CountStringBuffer(BindMachineResult.szDescribeString);
		WORD wSendSize = sizeof(BindMachineResult) - sizeof(BindMachineResult.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BIND_MANCHINE_RESULT, dwContextID, dwTokenID, &BindMachineResult, wSendSize);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GP_BindMachineResult BindMachineResult = { 0 };

		//设置变量
		BindMachineResult.lResultCode = pIException->GetExceptionResult();

		//获取信息
		StringCchCopy(BindMachineResult.szDescribeString, CountArray(BindMachineResult.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(BindMachineResult.szDescribeString);
		WORD wSendSize = sizeof(BindMachineResult) - sizeof(BindMachineResult.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BIND_MANCHINE_RESULT, dwContextID, dwTokenID, &BindMachineResult, wSendSize);

		return false;
	}

	return true;
}

//查询配置
bool CDataBaseEngineSink::OnRequestPCQueryGameConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT (wDataSize == sizeof(DBR_GP_QueryGameConfig));
		if (wDataSize != sizeof(DBR_GP_QueryGameConfig)) return false;

		//请求处理
		auto pQueryGameConfig = (DBR_GP_QueryGameConfig*)pData;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"), pQueryGameConfig->wKindID);
		
		//执行查询
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameBasicConfig"), true);
		if (lResultCode == DB_SUCCESS && m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//构造对象
			DBO_GP_GameBasicConfig BasicConfig = { 0 };

			//读取配置
			m_PlatformDBAide.GetValue_String(TEXT("ConfigValue"), BasicConfig.szBasicConfig,CountArray(BasicConfig.szBasicConfig));			

			//发送消息
			WORD wSendSize = CountStringBuffer(BasicConfig.szBasicConfig);
			wSendSize += sizeof(BasicConfig) - sizeof(BasicConfig.szBasicConfig);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_BASIC_CONFIG, dwContextID, dwTokenID, &BasicConfig, wSendSize);
		}		

		//加载配置
		for (BYTE cbTypeID = 0; cbTypeID < 2; cbTypeID++)
		{
			//构造参数
			m_PlatformDBAide.ResetParameter();
			m_PlatformDBAide.AddParameter(TEXT("@wKindID"), pQueryGameConfig->wKindID);
			m_PlatformDBAide.AddParameter(TEXT("@cbTypeID"), cbTypeID);

			//执行查询
			LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameWeightConfig"), true);
			if (lResultCode == DB_SUCCESS)
			{
				//构造对象
				DBO_GP_GameWeightConfig WeightConfig = {0};

				//读取信息
				while (m_PlatformDBModule->IsRecordsetEnd() == false)
				{
					auto pWeightItem = &WeightConfig.TimesWeightItem[WeightConfig.wItemCount++];

					//读取配置
					pWeightItem->cbWeightType = cbTypeID;
					pWeightItem->dwTimesRange[0] = m_PlatformDBAide.GetValue_WORD(TEXT("MinTimes"));
					pWeightItem->dwTimesRange[1] = m_PlatformDBAide.GetValue_WORD(TEXT("MaxTimes"));
					pWeightItem->cbTimesKind = m_PlatformDBAide.GetValue_BYTE(TEXT("TimesKind"));
					pWeightItem->wTimesWeight = m_PlatformDBAide.GetValue_WORD(TEXT("WeightValue"));
					pWeightItem->fWightRatio = m_PlatformDBAide.GetValue_FLOAT(TEXT("WeightRatio"));					

					//移动记录
					m_PlatformDBModule->MoveToNext();
				}

				//发送结果
				WORD wSendSize = sizeof(WeightConfig) - sizeof(WeightConfig.TimesWeightItem);
				wSendSize += WeightConfig.wItemCount * sizeof(WeightConfig.TimesWeightItem[0]);				
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_WEIGHT_CONFIG, dwContextID, dwTokenID, &WeightConfig, wSendSize);
			}			
		}

		//执行结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_CONFIG_FINISH, dwContextID, dwTokenID, NULL, 0);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("数据库操作异常，请稍后再次尝试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//保存配置
bool CDataBaseEngineSink::OnRequestPCUpdateBasicConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize <= sizeof(DBR_GP_UpdateBasicConfig));
		if (wDataSize > sizeof(DBR_GP_UpdateBasicConfig)) return false;

		//请求处理
		auto pUpdateBasicConfig = (DBR_GP_UpdateBasicConfig*)pData;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"), pUpdateBasicConfig->wKindID);
		m_PlatformDBAide.AddParameter(TEXT("@strBasicConfig"), pUpdateBasicConfig->szBasicConfig);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_UpdateGameBasicConfig"), false);
		if (lResultCode == DB_SUCCESS)
		{
			//变量定义
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//构造变量
			StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString),TEXT("基础配置保存成功！"));

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wSendSize);
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("数据库操作异常，请稍后再次尝试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//保存配置
bool CDataBaseEngineSink::OnRequestPCUpdateWeightConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize <= sizeof(DBR_GP_UpdateWeightConfig));
		if (wDataSize > sizeof(DBR_GP_UpdateWeightConfig)) return false;

		//请求处理
		auto pUpdateWeightConfig = (DBR_GP_UpdateWeightConfig*)pData;

		//序列化权重
		TCHAR szWeightConfig[4000] = { 0 };
		SerializeWeightConfig(pUpdateWeightConfig->TimesWeightItem, pUpdateWeightConfig->wItemCount, szWeightConfig,CountArray(szWeightConfig));

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"), pUpdateWeightConfig->wKindID);
		m_PlatformDBAide.AddParameter(TEXT("@cbWeightType"), pUpdateWeightConfig->cbTypeID);
		m_PlatformDBAide.AddParameter(TEXT("@strWeightConfig"), szWeightConfig);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_UpdateGameWeightConfig"), false);
		if (lResultCode == DB_SUCCESS)
		{
			//变量定义
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//构造变量
			StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString), TEXT("权重配置保存成功！"));

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wSendSize);
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("数据库操作异常，请稍后再次尝试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//获取参数
bool CDataBaseEngineSink::OnRequestPCQueryParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GP_QueryParameter));
		if (wDataSize != sizeof(DBR_GP_QueryParameter)) return false;

		//请求处理
		DBR_GP_QueryParameter* pQueryParameter = (DBR_GP_QueryParameter*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wServerID"), pQueryParameter->wServerID);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RobotQueryParameter"), true);

		//执行结果
		OnRobotDisposeResult(dwContextID, dwTokenID, lResultCode, SUB_GP_QUERY_PARAMETER, pQueryParameter->wServerID);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("数据库操作异常，请稍后再次尝试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//添加参数
bool CDataBaseEngineSink::OnRequestPCAppendParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GP_AppendParameter));
		if (wDataSize != sizeof(DBR_GP_AppendParameter)) return false;

		//请求处理
		DBR_GP_AppendParameter* pAppendParameter = (DBR_GP_AppendParameter*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wServerID"), pAppendParameter->wServerID);
		m_AccountsDBAide.AddParameter(TEXT("@dwServiceMode"), pAppendParameter->RobotParameter.dwServiceMode);
		m_AccountsDBAide.AddParameter(TEXT("@dwRobotCount"), pAppendParameter->RobotParameter.dwRobotCount);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterTime"), pAppendParameter->RobotParameter.dwEnterTime);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveTime"), pAppendParameter->RobotParameter.dwLeaveTime);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterMinInterval"), pAppendParameter->RobotParameter.dwEnterMinInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterMaxInterval"), pAppendParameter->RobotParameter.dwEnterMaxInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveMinInterval"), pAppendParameter->RobotParameter.dwLeaveMinInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveMaxInterval"), pAppendParameter->RobotParameter.dwLeaveMaxInterval);
		m_AccountsDBAide.AddParameter(TEXT("@lTakeMinScore"), pAppendParameter->RobotParameter.lTakeMinScore);
		m_AccountsDBAide.AddParameter(TEXT("@lTakeMaxScore"), pAppendParameter->RobotParameter.lTakeMaxScore);
		m_AccountsDBAide.AddParameter(TEXT("@dwSwitchMinInnings"), pAppendParameter->RobotParameter.dwSwitchMinInnings);
		m_AccountsDBAide.AddParameter(TEXT("@dwSwitchMaxInnings"), pAppendParameter->RobotParameter.dwSwitchMaxInnings);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RobotAppendParameter"), true);

		//执行结果
		OnRobotDisposeResult(dwContextID, dwTokenID, lResultCode, SUB_GP_APPEND_PARAMETER, pAppendParameter->wServerID);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("数据库操作异常，请稍后再次尝试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//修改参数
bool CDataBaseEngineSink::OnRequestPCModifyParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GP_ModifyParameter));
		if (wDataSize != sizeof(DBR_GP_ModifyParameter)) return false;

		//请求处理
		DBR_GP_ModifyParameter* pModifyParameter = (DBR_GP_ModifyParameter*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwDatchID"), pModifyParameter->RobotParameter.dwBatchID);
		m_AccountsDBAide.AddParameter(TEXT("@dwServiceMode"), pModifyParameter->RobotParameter.dwServiceMode);
		m_AccountsDBAide.AddParameter(TEXT("@dwRobotCount"), pModifyParameter->RobotParameter.dwRobotCount);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterTime"), pModifyParameter->RobotParameter.dwEnterTime);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveTime"), pModifyParameter->RobotParameter.dwLeaveTime);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterMinInterval"), pModifyParameter->RobotParameter.dwEnterMinInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterMaxInterval"), pModifyParameter->RobotParameter.dwEnterMaxInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveMinInterval"), pModifyParameter->RobotParameter.dwLeaveMinInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveMaxInterval"), pModifyParameter->RobotParameter.dwLeaveMaxInterval);
		m_AccountsDBAide.AddParameter(TEXT("@lTakeMinScore"), pModifyParameter->RobotParameter.lTakeMinScore);
		m_AccountsDBAide.AddParameter(TEXT("@lTakeMaxScore"), pModifyParameter->RobotParameter.lTakeMaxScore);
		m_AccountsDBAide.AddParameter(TEXT("@dwSwitchMinInnings"), pModifyParameter->RobotParameter.dwSwitchMinInnings);
		m_AccountsDBAide.AddParameter(TEXT("@dwSwitchMaxInnings"), pModifyParameter->RobotParameter.dwSwitchMaxInnings);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RobotModifyParameter"), true);

		//执行结果
		OnRobotDisposeResult(dwContextID, dwTokenID, lResultCode, SUB_GP_MODIFY_PARAMETER, pModifyParameter->wServerID);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("数据库操作异常，请稍后再次尝试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//删除参数
bool CDataBaseEngineSink::OnRequestPCDeleteParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GP_DeleteParameter));
		if (wDataSize != sizeof(DBR_GP_DeleteParameter)) return false;

		//请求处理
		DBR_GP_DeleteParameter* pDeleteParameter = (DBR_GP_DeleteParameter*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwBatchID"), pDeleteParameter->dwBatchID);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RobotDeleteParameter"), true);

		//执行结果
		OnRobotDisposeResult(dwContextID, dwTokenID, lResultCode, SUB_GP_DELETE_PARAMETER, pDeleteParameter->wServerID);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("数据库操作异常，请稍后再次尝试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//查询库存
bool CDataBaseEngineSink::OnRequestPCQueryStockInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GP_QueryStockInfo));
		if (wDataSize != sizeof(DBR_GP_QueryStockInfo)) return false;

		//请求处理
		auto pQueryStockInfo = (DBR_GP_QueryStockInfo*)pData;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@strKindIDList"), pQueryStockInfo->szKindID);

		//执行查询
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryStockList"), true);

		if (lResultCode == DB_SUCCESS)
		{
			OnStockDisposeResult(dwContextID,dwTokenID,0,SUB_GP_QUERY_STOCK_INFO);			
		}		

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("数据库操作异常，请稍后再次尝试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//添加库存
bool CDataBaseEngineSink::OnRequestPCAppendStockInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GP_AppendStockInfo));
		if (wDataSize != sizeof(DBR_GP_AppendStockInfo)) return false;

		//提取数据
		auto pAppendStockInfo = (DBR_GP_AppendStockInfo*)pData;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"), pAppendStockInfo->StockInfo.wKindID);
		m_PlatformDBAide.AddParameter(TEXT("@lBetAmount"), pAppendStockInfo->StockInfo.lBetAmount);
		m_PlatformDBAide.AddParameter(TEXT("@wStockKind"), pAppendStockInfo->StockInfo.wStockKind);
		m_PlatformDBAide.AddParameter(TEXT("@wStockLevel"), pAppendStockInfo->StockInfo.wStockLevel);
		m_PlatformDBAide.AddParameter(TEXT("@strStockName"), pAppendStockInfo->StockInfo.szStockName);
		m_PlatformDBAide.AddParameter(TEXT("@lExtraScore"), pAppendStockInfo->StockInfo.lExtraScore);
		m_PlatformDBAide.AddParameter(TEXT("@lStockScore"), pAppendStockInfo->StockInfo.lStockScore);
		m_PlatformDBAide.AddParameter(TEXT("@cbEnableMode"), pAppendStockInfo->StockInfo.cbEnableMode);
		m_PlatformDBAide.AddParameter(TEXT("@dwEnableEndTime"), pAppendStockInfo->StockInfo.dwEnableEndTime);
		m_PlatformDBAide.AddParameter(TEXT("@dwEnableStartTime"), pAppendStockInfo->StockInfo.dwEnableStartTime);
		m_PlatformDBAide.AddParameter(TEXT("@strShrinkOption"), pAppendStockInfo->StockInfo.szShrinkOption);
		m_PlatformDBAide.AddParameter(TEXT("@strRelationIDList"), pAppendStockInfo->StockInfo.szRelationStockID);

		//序列化
		TCHAR szJackpotOption[2048] = { 0 };
		SerializeJackpotOption(pAppendStockInfo->StockInfo.GameJackpotItem, pAppendStockInfo->StockInfo.wJackpotCount, szJackpotOption, CountArray(szJackpotOption));

		//彩池信息
		m_PlatformDBAide.AddParameter(TEXT("@strJackpotInfo"), szJackpotOption);
		
		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_AppendStockInfo"), true);
		if (lResultCode == DB_SUCCESS)
		{
			OnStockDisposeResult(dwContextID, dwTokenID, 0, SUB_GP_APPEND_STOCK_INFO);					
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("数据库操作异常，请稍后再次尝试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//修改库存
bool CDataBaseEngineSink::OnRequestPCModifyStockInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GP_ModifyStockInfo));
		if (wDataSize != sizeof(DBR_GP_ModifyStockInfo)) return false;

		//提取数据
		auto pModifyStockInfo = (DBR_GP_ModifyStockInfo*)pData;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wStockID"), pModifyStockInfo->StockInfo.wStockID);
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"), pModifyStockInfo->StockInfo.wKindID);
		m_PlatformDBAide.AddParameter(TEXT("@lBetAmount"), pModifyStockInfo->StockInfo.lBetAmount);
		m_PlatformDBAide.AddParameter(TEXT("@wStockKind"), pModifyStockInfo->StockInfo.wStockKind);
		m_PlatformDBAide.AddParameter(TEXT("@wStockLevel"), pModifyStockInfo->StockInfo.wStockLevel);
		m_PlatformDBAide.AddParameter(TEXT("@strStockName"), pModifyStockInfo->StockInfo.szStockName);
		m_PlatformDBAide.AddParameter(TEXT("@lExtraScore"), pModifyStockInfo->StockInfo.lExtraScore);
		m_PlatformDBAide.AddParameter(TEXT("@cbEnableMode"), pModifyStockInfo->StockInfo.cbEnableMode);
		m_PlatformDBAide.AddParameter(TEXT("@dwEnableEndTime"), pModifyStockInfo->StockInfo.dwEnableEndTime);
		m_PlatformDBAide.AddParameter(TEXT("@dwEnableStartTime"), pModifyStockInfo->StockInfo.dwEnableStartTime);
		m_PlatformDBAide.AddParameter(TEXT("@strShrinkOption"), pModifyStockInfo->StockInfo.szShrinkOption);
		m_PlatformDBAide.AddParameter(TEXT("@strRelationIDList"), pModifyStockInfo->StockInfo.szRelationStockID);
		
		//序列化
		TCHAR szJackpotOption[2048] = { 0 };
		SerializeJackpotOption(pModifyStockInfo->StockInfo.GameJackpotItem, pModifyStockInfo->StockInfo.wJackpotCount, szJackpotOption, CountArray(szJackpotOption));

		//彩池信息
		m_PlatformDBAide.AddParameter(TEXT("@strJackpotInfo"), szJackpotOption);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyStockInfo"), true);
		if (lResultCode == DB_SUCCESS)
		{
			OnStockDisposeResult(dwContextID, dwTokenID, 0, SUB_GP_MODIFY_STOCK_INFO);			
		}
		else
		{
			//变量定义
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("数据库操作异常，请稍后再次尝试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//删除库存
bool CDataBaseEngineSink::OnRequestPCDeleteStockInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GP_DeleteStockInfo));
		if (wDataSize != sizeof(DBR_GP_DeleteStockInfo)) return false;

		//提取数据
		auto pDeleteStockInfo = (DBR_GP_DeleteStockInfo*)pData;

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wStockID"), pDeleteStockInfo->wStockID);

		//执行查询
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_DeleteStockInfo"), true);
		if (lResultCode == DB_SUCCESS)
		{
			OnStockDisposeResult(dwContextID, dwTokenID, 0, SUB_GP_DELETE_STOCK_INFO);			
		}	

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("数据库操作异常，请稍后再次尝试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//游客登录
bool CDataBaseEngineSink::OnRequestMBLogonVisitor(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_LogonVisitor));
		if (wDataSize != sizeof(DBR_MB_LogonVisitor)) return false;

		//请求处理
		DBR_MB_LogonVisitor* pLogonVisitor = (DBR_MB_LogonVisitor*)pData;

		//执行判断
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pLogonVisitor->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pLogonVisitor->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数		
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonVisitor->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonVisitor->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pLogonVisitor->wMarketID);
        m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"), pLogonVisitor->dwSpreaderID);
        m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonVisitor->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pLogonVisitor->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pLogonVisitor->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pLogonVisitor->szMachineOSID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineName"), pLogonVisitor->szMachineName);
		m_AccountsDBAide.AddParameter(TEXT("@strPackageName"), pLogonVisitor->szPackageName);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyVisitor"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//错误处理
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"由于数据库操作异常，请您稍后重试或选择另一服务器登录！"), true);

		return false;
	}

	return true;
}

//帐号登录
bool CDataBaseEngineSink::OnRequestMBLogonAccounts(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_LogonAccounts));
		if (wDataSize != sizeof(DBR_MB_LogonAccounts)) return false;

		//请求处理
		DBR_MB_LogonAccounts* pLogonAccounts = (DBR_MB_LogonAccounts*)pData;

		//执行判断
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pLogonAccounts->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pLogonAccounts->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonAccounts->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonAccounts->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"), pLogonAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pLogonAccounts->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pLogonAccounts->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonAccounts->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pLogonAccounts->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pLogonAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pLogonAccounts->szMachineOSID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pLogonAccounts->szMobilePhone);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyAccounts"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//错误处理
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"由于数据库操作异常，请您稍后重试或选择另一服务器登录！"), true);

		return false;
	}

	return true;
}

//秘钥登录
bool CDataBaseEngineSink::OnRequestMBLogonSecretKey(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_LogonSecretKey));
		if (wDataSize != sizeof(DBR_MB_LogonSecretKey)) return false;

		//请求处理
		DBR_MB_LogonSecretKey* pLogonSecretKey = (DBR_MB_LogonSecretKey*)pData;

		//执行判断
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pLogonSecretKey->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//构造令牌
		string sLogonToken(CT2CA(pLogonSecretKey->szSecretKey).m_psz);

		//解析JWT
		auto jwtPtr = CWHJwt::Parse(sLogonToken);
		if (jwtPtr->Verify() == false)
		{
			OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0008, L"您的登录令牌已过期！"), true);

			return true;
		}

		//获取秘钥
		auto sSecretKey = jwtPtr->Claims().Get<string>("SecretKey");
		if (sSecretKey.empty())
		{
			OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0008, L"您的登录令牌已过期！"), true);

			return true;
		}		

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pLogonSecretKey->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonSecretKey->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonSecretKey->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pLogonSecretKey->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strLogonToken"), CA2CT(sSecretKey.c_str()).m_psz);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pLogonSecretKey->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonSecretKey->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pLogonSecretKey->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pLogonSecretKey->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pLogonSecretKey->szMachineOSID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pLogonSecretKey->szMobilePhone);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacySecretKey"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//错误处理
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"由于数据库操作异常，请您稍后重试或选择另一服务器登录！"), true);

		return false;
	}

	return true;
}

//验证码登录
bool CDataBaseEngineSink::OnRequestMBLogonMobileCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_LogonMobileCode));
		if (wDataSize != sizeof(DBR_MB_LogonMobileCode)) return false;

		//请求处理
		auto pLogonMobileCode = (DBR_MB_LogonMobileCode*)pData;

		//执行判断
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pLogonMobileCode->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pLogonMobileCode->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonMobileCode->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonMobileCode->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCode"), pLogonMobileCode->szAuthCode);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pLogonMobileCode->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pLogonMobileCode->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"), pLogonMobileCode->dwSpreaderID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonMobileCode->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pLogonMobileCode->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pLogonMobileCode->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pLogonMobileCode->szMachineOSID);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyMobileCode"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//错误处理
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"由于数据库操作异常，请您稍后重试或选择另一服务器登录！"), true);

		return false;
	}

	return true;
}

//其他登录
bool CDataBaseEngineSink::OnRequestMBLogonThirdParty(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_LogonThirdParty));
		if (wDataSize != sizeof(DBR_MB_LogonThirdParty)) return false;

		//请求处理
		DBR_MB_LogonThirdParty* pLogonThirdParty = (DBR_MB_LogonThirdParty*)pData;

		//执行判断
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pLogonThirdParty->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pLogonThirdParty->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonThirdParty->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonThirdParty->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
        m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"), pLogonThirdParty->dwSpreaderID);
        m_AccountsDBAide.AddParameter(TEXT("@dwExternalID"), pLogonThirdParty->dwExternalID);
		m_AccountsDBAide.AddParameter(TEXT("@strUniqueID"), pLogonThirdParty->szUniqueID);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"), pLogonThirdParty->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strUserDatum"), pLogonThirdParty->szUserDatum);
		m_AccountsDBAide.AddParameter(TEXT("@strUserFaceUrl"), pLogonThirdParty->szUserFaceUrl);//kk jia
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"), pLogonThirdParty->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"), pLogonThirdParty->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pLogonThirdParty->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonThirdParty->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pLogonThirdParty->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pLogonThirdParty->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pLogonThirdParty->szMachineOSID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pLogonThirdParty->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strPackageName"), pLogonThirdParty->szPackageName);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyThirdParty"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true,true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//错误处理
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"由于数据库操作异常，请您稍后重试或选择另一服务器登录！"), true);

		return false;
	}

	return true;
}

//帐号登录
bool CDataBaseEngineSink::OnRequestMBLogonMobilePhone(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_LogonMobilePhone));
		if (wDataSize != sizeof(DBR_MB_LogonMobilePhone)) return false;

		//请求处理
		DBR_MB_LogonMobilePhone* pLogonMobilePhone = (DBR_MB_LogonMobilePhone*)pData;

		//执行判断
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pLogonMobilePhone->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pLogonMobilePhone->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonMobilePhone->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonMobilePhone->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pLogonMobilePhone->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pLogonMobilePhone->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pLogonMobilePhone->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonMobilePhone->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pLogonMobilePhone->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pLogonMobilePhone->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pLogonMobilePhone->szMachineOSID);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyMobilePhone"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//错误处理
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"由于数据库操作异常，请您稍后重试或选择另一服务器登录！"), true);

		return false;
	}

	return true;
}

//帐号注册
bool CDataBaseEngineSink::OnRequestMBRegisterAccounts(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_RegisterAccounts));
		if (wDataSize != sizeof(DBR_MB_RegisterAccounts)) return false;

		//请求处理
		DBR_MB_RegisterAccounts* pRegisterAccounts = (DBR_MB_RegisterAccounts*)pData;

		//执行判断
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pRegisterAccounts->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pRegisterAccounts->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pRegisterAccounts->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pRegisterAccounts->wLogonID);
        m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
        m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"), pRegisterAccounts->dwSpreaderID);
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"), pRegisterAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"), pRegisterAccounts->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pRegisterAccounts->szLogonPass);		
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"), pRegisterAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"), pRegisterAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pRegisterAccounts->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pRegisterAccounts->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pRegisterAccounts->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pRegisterAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pRegisterAccounts->szMachineOSID);
		m_AccountsDBAide.AddParameter(TEXT("@strPackageName"), pRegisterAccounts->szPackageName);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_RegisterAccounts"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//错误处理
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"由于数据库操作异常，请您稍后重试或选择另一服务器登录！"), true);

		return false;
	}

	return true;
}

//手机注册 
bool CDataBaseEngineSink::OnRequestMBRegisterMobilePhone(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_RegisterMobilePhone));
		if (wDataSize != sizeof(DBR_MB_RegisterMobilePhone)) return false;

		//请求处理
		auto pRegisterMobilePhone = (DBR_MB_RegisterMobilePhone*)pData;

		//执行判断
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pRegisterMobilePhone->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pRegisterMobilePhone->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pRegisterMobilePhone->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pRegisterMobilePhone->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"), pRegisterMobilePhone->dwSpreaderID);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCode"), pRegisterMobilePhone->szAuthCode);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pRegisterMobilePhone->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"), pRegisterMobilePhone->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pRegisterMobilePhone->szLogonPass);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"), pRegisterMobilePhone->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"), pRegisterMobilePhone->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pRegisterMobilePhone->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pRegisterMobilePhone->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pRegisterMobilePhone->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pRegisterMobilePhone->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pRegisterMobilePhone->szMachineOSID);
		m_AccountsDBAide.AddParameter(TEXT("@strPackageName"), pRegisterMobilePhone->szPackageName);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_RegisterMobilePhone"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//错误处理
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"由于数据库操作异常，请您稍后重试或选择另一服务器登录！"), true);

		return false;
	}

	return true;
}

//邮箱注册 
bool CDataBaseEngineSink::OnRequestMBRegisterEMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_RegisterEMail));
		if (wDataSize != sizeof(DBR_MB_RegisterEMail)) return false;

		//请求处理
		DBR_MB_RegisterEMail* pRegisterAccounts = (DBR_MB_RegisterEMail*)pData;

		//执行判断
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pRegisterAccounts->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pRegisterAccounts->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pRegisterAccounts->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pRegisterAccounts->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"), pRegisterAccounts->dwSpreaderID);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCode"), pRegisterAccounts->szAuthCode);
		m_AccountsDBAide.AddParameter(TEXT("@strEMail"), pRegisterAccounts->szEMail);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"), pRegisterAccounts->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pRegisterAccounts->szLogonPass);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"), pRegisterAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"), pRegisterAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pRegisterAccounts->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pRegisterAccounts->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pRegisterAccounts->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pRegisterAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pRegisterAccounts->szMachineOSID);
		m_AccountsDBAide.AddParameter(TEXT("@strPackageName"), pRegisterAccounts->szPackageName);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_RegisterEMail"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//错误处理
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"由于数据库操作异常，请您稍后重试或选择另一服务器登录！"), true);

		return false;
	}

	return true;
}

//邮箱登陆
bool CDataBaseEngineSink::OnRequestMBLogonEMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_LogonEMail));
		if (wDataSize != sizeof(DBR_MB_LogonEMail)) return false;

		//请求处理
		DBR_MB_LogonEMail* pLogonEMail = (DBR_MB_LogonEMail*)pData;

		//执行判断
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pLogonEMail->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pLogonEMail->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonEMail->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonEMail->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@strEMail"), pLogonEMail->szEMail);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pLogonEMail->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pLogonEMail->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonEMail->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pLogonEMail->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pLogonEMail->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pLogonEMail->szMachineOSID);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyEMail"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//错误处理
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"由于数据库操作异常，请您稍后重试或选择另一服务器登录！"), true);

		return false;
	}

	return true;
}

//验证码登录
bool CDataBaseEngineSink::OnRequestMBLogonEMailCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_LogonEMailCode));
		if (wDataSize != sizeof(DBR_MB_LogonEMailCode)) return false;

		//请求处理
		DBR_MB_LogonEMailCode* pLogonMobileCode = (DBR_MB_LogonEMailCode*)pData;

		//执行判断
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pLogonMobileCode->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pLogonMobileCode->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonMobileCode->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonMobileCode->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCode"), pLogonMobileCode->szAuthCode);
		m_AccountsDBAide.AddParameter(TEXT("@strEMail"), pLogonMobileCode->szEMail);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pLogonMobileCode->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"), pLogonMobileCode->dwSpreaderID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonMobileCode->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pLogonMobileCode->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pLogonMobileCode->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pLogonMobileCode->szMachineOSID);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyEMailAuthCode"), true);

		//结果处理
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//错误处理
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"由于数据库操作异常，请您稍后重试或选择另一服务器登录！"), true);

		return false;
	}

	return true;
}

//帐号登出
bool CDataBaseEngineSink::OnRequestMBLogoutAccounts(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT (wDataSize == sizeof(DBR_MB_LogoutAccounts));
		if (wDataSize != sizeof(DBR_MB_LogoutAccounts)) return false;

		//请求处理
		DBR_MB_LogoutAccounts* pLogoutAccounts = (DBR_MB_LogoutAccounts*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogoutAccounts->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogoutAccounts->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pLogoutAccounts->dwSocketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pLogoutAccounts->dwUserID);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_LogoutAccounts"), false);

		//登出结果
		DBO_MB_LogoutResult LogoutResult;
		ZeroMemory(&LogoutResult,sizeof(LogoutResult));

		//设置变量
		LogoutResult.lResultCode = lResultCode;
		LogoutResult.dwUserID = pLogoutAccounts->dwUserID;		

		//投递消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_LOGOUT_RESULT, dwContextID, dwTokenID, &LogoutResult, sizeof(LogoutResult));

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//点赞玩家
bool CDataBaseEngineSink::OnRequestMBLikePlayer(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_LikePlayer));
		if (wDataSize != sizeof(DBR_MB_LikePlayer)) return false;

		//请求处理
		DBR_MB_LikePlayer* pLikePlayer = (DBR_MB_LikePlayer*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pLikePlayer->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pLikePlayer->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDstUserID"), pLikePlayer->dwDstUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_LikePlayer"), true) == DB_SUCCESS)
		{
			//点赞数量
			DWORD dwLinkedNumber = m_AccountsDBAide.GetValue_DWORD(TEXT("LikedNumber"));

			//构造结构
			UserPublicInfo PublicInfo;
			PublicInfo.set_uid(pLikePlayer->dwDstUserID);
			//PublicInfo.set_likednumber(dwLinkedNumber);

			//更新信息
			UpdateUserCache(dwContextID, dwTokenID, PublicInfo);

			//变量定义
			DBO_MB_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateSuccess.wRequestCmdID = SUB_MB_LIKE_PLAYER;
			OperateSuccess.lResultCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wSendSize);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_LIKE_PLAYER;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.wRequestCmdID = SUB_MB_LIKE_PLAYER;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"由于系统繁忙，请稍后再试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//修改密码
bool CDataBaseEngineSink::OnRequestMBModifyLogonPass(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT (wDataSize == sizeof(DBR_MB_ModifyLogonPass));
		if (wDataSize != sizeof(DBR_MB_ModifyLogonPass)) return false;

		//请求处理
		DBR_MB_ModifyLogonPass* pModifyLogonPass = (DBR_MB_ModifyLogonPass*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pModifyLogonPass->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pModifyLogonPass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"), pModifyLogonPass->szScrPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"), pModifyLogonPass->szDesPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_ModifyLogonPassword"), false) == DB_SUCCESS)
		{
			//变量定义
			DBO_MB_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateSuccess.wRequestCmdID = SUB_MB_MODIFY_LOGON_PASS;
			OperateSuccess.lResultCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString),CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wSendSize);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_MODIFY_LOGON_PASS;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString),CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.wRequestCmdID = SUB_MB_MODIFY_LOGON_PASS;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"由于系统繁忙，请稍后再试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//修改资料
bool CDataBaseEngineSink::OnRequestMBModifyIndividual(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_MB_ModifyIndividual));
		if (wDataSize!=sizeof(DBR_MB_ModifyIndividual)) return false;

		//请求处理
		DBR_MB_ModifyIndividual * pModifyIndividual=(DBR_MB_ModifyIndividual *)pData;

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pModifyIndividual->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyIndividual->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"), pModifyIndividual->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@wInfoMask"), pModifyIndividual->wInfoMask);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pModifyIndividual->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strUnderWrite"), pModifyIndividual->szUnderWrite);		
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_ModifyUserIndividual"),true)==DB_SUCCESS)
		{
			////////////////////////////////////////////////////////////////////////////////////////////////////
			//更新财富
			if (pModifyIndividual->wInfoMask & DTP_MB_NICKNAME)
			{
				//更新财富
				DBO_MB_WealthUpdate WealthUpdate;
				ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

				//设置变量
				WealthUpdate.wItemCount = 1;
				WealthUpdate.WealthItem[0].wMoneyID = GOODS_ID_GOLD;
				WealthUpdate.WealthItem[0].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));

				//发送结果			
				WORD wHeadSize = sizeof(WealthUpdate) - sizeof(WealthUpdate.WealthItem);
				WORD wPacketSize = WealthUpdate.wItemCount * sizeof(WealthUpdate.WealthItem[0]) + wHeadSize;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_WEALTH_UPDATE, dwContextID, dwTokenID, &WealthUpdate, wPacketSize);
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			//更新行为掩码
			DBO_MB_UserActionMask UserActionMask;
			ZeroMemory(&UserActionMask, sizeof(UserActionMask));

			//读取数据
			UserActionMask.dwActionMaskEver = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskEver"));
			UserActionMask.dwActionMaskPerDay = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskPerDay"));
			UserActionMask.dwActionMaskPerWeek = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskPerWeek"));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_ACTION_MASK, dwContextID, dwTokenID, &UserActionMask, sizeof(UserActionMask));

			////////////////////////////////////////////////////////////////////////////////////////////////////
			//操作结果 

			//变量定义
			DBO_MB_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateSuccess.wRequestCmdID = SUB_MB_MODIFY_INDIVIDUAL;
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString,CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_SUCCESS,dwContextID, dwTokenID, &OperateSuccess,wSendSize);

			//构造结构
			UserPublicInfo PublicInfo;
			PublicInfo.set_uid(pModifyIndividual->dwUserID);
			PublicInfo.set_gender(pModifyIndividual->cbGender);

			//用户昵称
			if (pModifyIndividual->wInfoMask & DTP_MB_NICKNAME)
			{
				//转换编码
				CHAR cbBuffer[128];
				int nStringLen = CWHService::Unicode2Utf8(pModifyIndividual->szNickName, cbBuffer, CountArray(cbBuffer));

				//设置昵称
				PublicInfo.set_nickname(cbBuffer, nStringLen);
			}

			//个性签名
			if (pModifyIndividual->wInfoMask & DTP_MB_UNDER_WRITE)
			{
				//转换编码
				CHAR cbBuffer[256];
				int nStringLen = CWHService::Unicode2Utf8(pModifyIndividual->szUnderWrite, cbBuffer, CountArray(cbBuffer));

				//个性签名
				PublicInfo.set_underwrite(cbBuffer, nStringLen);
			}

			//更新信息
			UpdateUserCache(dwContextID, dwTokenID, PublicInfo);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_MODIFY_INDIVIDUAL;
			OperateFailure.lErrorCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString,CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE,dwContextID, dwTokenID, &OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//构造变量
		OperateFailure.wRequestCmdID = SUB_MB_MODIFY_INDIVIDUAL;
		OperateFailure.lErrorCode=pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"由于系统繁忙，请稍后再试！"));

		//发送结果
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE,dwContextID, dwTokenID, &OperateFailure,wSendSize);

		return false;
	}

	return true;
}

//修改头像
bool CDataBaseEngineSink::OnRequestMBModifySystemFace(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_ModifySystemFace));
		if (wDataSize != sizeof(DBR_MB_ModifySystemFace)) return false;

		//请求处理
		DBR_MB_ModifySystemFace* pModifySystemFace = (DBR_MB_ModifySystemFace*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pModifySystemFace->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"), pModifySystemFace->wFaceID);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_ModifySystemFace"), true) == DB_SUCCESS)
		{
			////////////////////////////////////////////////////////////////////////////////////////////////////
			//更新财富
			DBO_MB_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

			//设置变量
			WealthUpdate.wItemCount = 1;
			WealthUpdate.WealthItem[0].wMoneyID = GOODS_ID_GOLD;
			WealthUpdate.WealthItem[0].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));

			//发送结果			
			WORD wHeadSize = sizeof(WealthUpdate) - sizeof(WealthUpdate.WealthItem);
			WORD wPacketSize = WealthUpdate.wItemCount * sizeof(WealthUpdate.WealthItem[0]) + wHeadSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_WEALTH_UPDATE, dwContextID, dwTokenID, &WealthUpdate, wPacketSize);

			////////////////////////////////////////////////////////////////////////////////////////////////////
			//更新行为掩码
			DBO_MB_UserActionMask UserActionMask;
			ZeroMemory(&UserActionMask, sizeof(UserActionMask));

			//读取数据
			UserActionMask.dwActionMaskEver = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskEver"));
			UserActionMask.dwActionMaskPerDay = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskPerDay"));
			UserActionMask.dwActionMaskPerWeek = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskPerWeek"));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_ACTION_MASK, dwContextID, dwTokenID, &UserActionMask, sizeof(UserActionMask));

			////////////////////////////////////////////////////////////////////////////////////////////////////
			//更新头像
			DBO_MB_UserFaceInfo UserFaceInfo;
			ZeroMemory(&UserFaceInfo, sizeof(UserFaceInfo));

			//读取信息
			UserFaceInfo.wFaceID = m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			UserFaceInfo.dwCustomID = m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_FACE_INFO, dwContextID, dwTokenID, &UserFaceInfo, sizeof(UserFaceInfo));

			//构造结构
			UserPublicInfo PublicInfo;
			PublicInfo.set_uid(pModifySystemFace->dwUserID);
			PublicInfo.set_faceid(UserFaceInfo.wFaceID);
			PublicInfo.set_customid(UserFaceInfo.dwCustomID);

			//更新信息
			UpdateUserCache(dwContextID, dwTokenID, PublicInfo);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_MODIFY_SYSTEM_FACE;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);


			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.wRequestCmdID = SUB_MB_MODIFY_SYSTEM_FACE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"由于系统繁忙，请稍后再试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//修改头像
bool CDataBaseEngineSink::OnRequestMBModifyCustomFace(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_ModifyCustomFace));
		if (wDataSize != sizeof(DBR_MB_ModifyCustomFace)) return false;

		//请求处理
		DBR_MB_ModifyCustomFace* pModifyCustomFace = (DBR_MB_ModifyCustomFace*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pModifyCustomFace->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwCustomID"), pModifyCustomFace->dwCustomID);
		m_AccountsDBAide.AddParameter(TEXT("@strFacePath"), pModifyCustomFace->szFacePath);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_ModifyCustomFace"), true) == DB_SUCCESS)
		{
			////////////////////////////////////////////////////////////////////////////////////////////////////
			//更新财富
			DBO_MB_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

			//设置变量
			WealthUpdate.wItemCount = 1;
			WealthUpdate.WealthItem[0].wMoneyID = GOODS_ID_GOLD;
			WealthUpdate.WealthItem[0].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));

			//发送结果			
			WORD wHeadSize = sizeof(WealthUpdate) - sizeof(WealthUpdate.WealthItem);
			WORD wPacketSize = WealthUpdate.wItemCount * sizeof(WealthUpdate.WealthItem[0]) + wHeadSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_WEALTH_UPDATE, dwContextID, dwTokenID, &WealthUpdate, wPacketSize);

			////////////////////////////////////////////////////////////////////////////////////////////////////
			//更新行为掩码
			DBO_MB_UserActionMask UserActionMask;
			ZeroMemory(&UserActionMask, sizeof(UserActionMask));

			//读取数据
			UserActionMask.dwActionMaskEver = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskEver"));
			UserActionMask.dwActionMaskPerDay = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskPerDay"));
			UserActionMask.dwActionMaskPerWeek = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskPerWeek"));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_ACTION_MASK, dwContextID, dwTokenID, &UserActionMask, sizeof(UserActionMask));
			
			////////////////////////////////////////////////////////////////////////////////////////////////////
			//更新头像
			DBO_MB_UserFaceInfo UserFaceInfo;
			ZeroMemory(&UserFaceInfo, sizeof(UserFaceInfo));

			//读取信息
			UserFaceInfo.wFaceID = m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			UserFaceInfo.dwCustomID = m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_FACE_INFO, dwContextID, dwTokenID, &UserFaceInfo, sizeof(UserFaceInfo));

			//构造结构
			UserPublicInfo PublicInfo;
			PublicInfo.set_uid(pModifyCustomFace->dwUserID);
			PublicInfo.set_faceid(UserFaceInfo.wFaceID);
			PublicInfo.set_customid(UserFaceInfo.dwCustomID);

			//更新信息
			UpdateUserCache(dwContextID, dwTokenID, PublicInfo);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_MODIFY_CUSTOM_FACE;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.wRequestCmdID = SUB_MB_MODIFY_CUSTOM_FACE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"由于系统繁忙，请稍后再试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//兑换商品
bool CDataBaseEngineSink::OnRequestMBExchangeMallGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_ExchangeMallGoods));
		if (wDataSize != sizeof(DBR_MB_ExchangeMallGoods)) return false;

		//请求处理
		DBR_MB_ExchangeMallGoods* pExchangeMallGoods = (DBR_MB_ExchangeMallGoods*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pExchangeMallGoods->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwMallGoodsID"), pExchangeMallGoods->dwMallGoodsID);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_EXCHANGE_MALLGOODS);
		m_AccountsDBAide.AddParameter(TEXT("@dwTimeStamp"), time(NULL));

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ExchangeMallGoods"), true) == DB_SUCCESS)
		{
			//构造结果
			DBO_MB_ExchangeMallGoods ExchangeMallGoods;
			ZeroMemory(&ExchangeMallGoods, sizeof(ExchangeMallGoods));

			//构造结构
			tagGoodsItem BuyGoodsItem;
			ZeroMemory(&BuyGoodsItem, sizeof(BuyGoodsItem));

			//物品信息
			BuyGoodsItem.wGoodsID = m_AccountsDBAide.GetValue_WORD(TEXT("IncGoodsID"));
			BuyGoodsItem.dwGoodsCount = m_AccountsDBAide.GetValue_DWORD(TEXT("IncGoodsCount"));
			BuyGoodsItem.dwGoodsIndate = m_AccountsDBAide.GetValue_DWORD(TEXT("IncGoodsIndate"));

			//物品信息
			ExchangeMallGoods.wIncGoodsID = BuyGoodsItem.wGoodsID;
			ExchangeMallGoods.dwIncGoodsCount = BuyGoodsItem.dwGoodsCount;
			ExchangeMallGoods.dwIncGoodsIndate = BuyGoodsItem.dwGoodsIndate;

			//钻石信息
			ExchangeMallGoods.lResultCode = DB_SUCCESS;			
			ExchangeMallGoods.wExpendMoneyID = m_AccountsDBAide.GetValue_WORD(TEXT("ExpendMoneyID"));
			ExchangeMallGoods.lMoneyBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("MoneyBalance"));			

			//更新财富
			UpdateUserWealth(pExchangeMallGoods->dwUserID, dwContextID, dwTokenID);

			//更新物品
			if (!g_GameGoodsManager->IsMoneyGoods(BuyGoodsItem.wGoodsID))
			{				
				UpdateUserGoods(pExchangeMallGoods->dwUserID, dwContextID, dwTokenID, &BuyGoodsItem, 1);
			}

			//投递消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_EXCHANGE_MALLGOODS, dwContextID, dwTokenID, &ExchangeMallGoods, sizeof(ExchangeMallGoods));
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量			
			OperateFailure.wRequestCmdID = SUB_MB_EXCHANGE_MALLGOODS;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.wRequestCmdID = SUB_MB_EXCHANGE_MALLGOODS;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"由于系统繁忙，请稍后再试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//兑换礼包码
bool CDataBaseEngineSink::OnRequestMBExchangeGiftbagCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_ExchangeGiftbagCode));
		if (wDataSize != sizeof(DBR_MB_ExchangeGiftbagCode)) return false;

		//请求处理
		DBR_MB_ExchangeGiftbagCode* pExchangeGiftbagCode = (DBR_MB_ExchangeGiftbagCode*)pData;

		//转化地址
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pExchangeGiftbagCode->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pExchangeGiftbagCode->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_EXCHANGE_GIFTBAGCODE);
		m_AccountsDBAide.AddParameter(TEXT("@strGiftbagCode"), pExchangeGiftbagCode->szGiftbagCode);
		m_AccountsDBAide.AddParameter(TEXT("@strClientAddr"), szClientAddr);
		
		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//结果处理
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ExchangeGiftbagCode"), true) == DB_SUCCESS)
		{
			//构造结果
			DBO_MB_ExchangeGiftbagCode ExchangeGiftbagCode;
			ZeroMemory(&ExchangeGiftbagCode, sizeof(ExchangeGiftbagCode));

			//钻石信息
			ExchangeGiftbagCode.lResultCode = DB_SUCCESS;
			m_AccountsDBAide.GetValue_String(TEXT("GiftBagGoodsList"), ExchangeGiftbagCode.szGiftBagGoodsList, CountArray(ExchangeGiftbagCode.szGiftBagGoodsList));

			//解析物品
			CGoodsItemArray GoodsArray, MoneyArray;
			g_GameGoodsManager->ParseFromJson(ExchangeGiftbagCode.szGiftBagGoodsList, MoneyArray, GoodsArray);

			//更新财富
			if (MoneyArray.GetCount() > 0)
			{
				UpdateUserWealth(pExchangeGiftbagCode->dwUserID, dwContextID, dwTokenID);
			}			

			//更新物品
			if (GoodsArray.GetCount())
			{
				UpdateUserGoods(pExchangeGiftbagCode->dwUserID, dwContextID, dwTokenID, GoodsArray.GetData(), GoodsArray.GetCount());
			}

			//投递消息
			WORD wStringSize = CountStringBuffer(ExchangeGiftbagCode.szDescribeString);
			WORD wSendSize = sizeof(ExchangeGiftbagCode) - sizeof(ExchangeGiftbagCode.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_EXCHANGE_GIFTBAGCODE, dwContextID, dwTokenID, &ExchangeGiftbagCode, wSendSize);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量			
			OperateFailure.wRequestCmdID = SUB_MB_EXCHANGE_GIFTBAGCODE;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.wRequestCmdID = SUB_MB_EXCHANGE_GIFTBAGCODE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"由于系统繁忙，请稍后再试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//绑定邮箱
bool CDataBaseEngineSink::OnRequestMBBindEMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_BindEMail));
		if (wDataSize != sizeof(DBR_MB_BindEMail)) return false;

		//请求处理
		DBR_MB_BindEMail* pBindEMail = (DBR_MB_BindEMail*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pBindEMail->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pBindEMail->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pBindEMail->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strNewPassword"), pBindEMail->szNewPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strMachine"), pBindEMail->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strEMail"), pBindEMail->szEMail);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCodeMD5"), pBindEMail->szAuthCode);
		m_AccountsDBAide.AddParameter(TEXT("@dwUnbindUserID"), pBindEMail->dwUnbindUserID);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_BindEMail"), true);

		//获取信息
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			DBO_MB_BindEMailSuccess BindSuccess;
			ZeroMemory(&BindSuccess, sizeof(BindSuccess));

			//绑定号码
			StringCchCopy(BindSuccess.szEMail, CountArray(BindSuccess.szEMail), pBindEMail->szEMail);

			//财富信息
			BindSuccess.lUserCard = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserCard"));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量			
			StringCchCopy(BindSuccess.szNoticeString, CountArray(BindSuccess.szNoticeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(BindSuccess.szNoticeString);
			WORD wPacketSize = sizeof(BindSuccess) - sizeof(BindSuccess.szNoticeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_BIND_EMAIL_SUCCESS, dwContextID, dwTokenID, &BindSuccess, wPacketSize);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_BIND_EMAIL;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = SUB_MB_BIND_EMAIL;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//解绑邮箱
bool CDataBaseEngineSink::OnRequestMBUnbindEMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_UnbindEMail));
		if (wDataSize != sizeof(DBR_MB_UnbindEMail)) return false;

		//请求处理
		DBR_MB_UnbindEMail* pUnBindEMail = (DBR_MB_UnbindEMail*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pUnBindEMail->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUnBindEMail->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pUnBindEMail->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strMachine"), pUnBindEMail->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strEMail"), pUnBindEMail->szEMail);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCodeMD5"), pUnBindEMail->szAuthCode);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_UnBindEMail"), true);

		//获取信息
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			DBO_MB_UnbindEMailSuccess UnBindMPSuccess;
			ZeroMemory(&UnBindMPSuccess, sizeof(UnBindMPSuccess));

			//财富信息
			UnBindMPSuccess.lUserCard = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserCard"));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量			
			StringCchCopy(UnBindMPSuccess.szNoticeString, CountArray(UnBindMPSuccess.szNoticeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(UnBindMPSuccess.szNoticeString);
			WORD wPacketSize = sizeof(UnBindMPSuccess) - sizeof(UnBindMPSuccess.szNoticeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_UNBIND_EMAIL_SUCCESS, dwContextID, dwTokenID, &UnBindMPSuccess, wPacketSize);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_UNBIND_EMAIL;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = SUB_MB_UNBIND_EMAIL;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//获取验证码
bool CDataBaseEngineSink::OnRequestMBAcquireAuthCode(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//效验参数
	ASSERT(wDataSize == sizeof(DBR_MB_AcquireAuthCode));
	if (wDataSize != sizeof(DBR_MB_AcquireAuthCode)) return false;

	//请求处理
	DBR_MB_AcquireAuthCode* pAcquireAuthCode = (DBR_MB_AcquireAuthCode*)pData;

	try
	{
		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE * pClientIP = (BYTE *)&pAcquireAuthCode->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pAcquireAuthCode->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pAcquireAuthCode->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strMachine"), pAcquireAuthCode->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@cbCodeKind"), pAcquireAuthCode->cbCodeKind);
		m_AccountsDBAide.AddParameter(TEXT("@cbAuthKind"), pAcquireAuthCode->cbAuthKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwBindUserID"), pAcquireAuthCode->dwBindUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobileEmail"), pAcquireAuthCode->szMobileEmail);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCode"), pAcquireAuthCode->szAuthCode);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCodeMD5"), pAcquireAuthCode->szAuthCodeMD5);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_AcquireAuthCode"), true);

		//构造结构
		DBO_MB_AcquireACResult AcquireACResult;
		ZeroMemory(&AcquireACResult, sizeof(AcquireACResult));

		//验证类型
		AcquireACResult.cbCodeKind = pAcquireAuthCode->cbCodeKind;
		AcquireACResult.cbAuthKind = pAcquireAuthCode->cbAuthKind;
		StringCchCopy(AcquireACResult.szAuthCode, CountArray(AcquireACResult.szAuthCode), pAcquireAuthCode->szAuthCode);
		StringCchCopy(AcquireACResult.szMobileEmail, CountArray(AcquireACResult.szMobileEmail), pAcquireAuthCode->szMobileEmail);

		//获取信息
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

		//结果信息
		AcquireACResult.lResultCode = lReturnValue;
		if (DBVarValue.bstrVal)
		{
			StringCchCopy(AcquireACResult.szNoticeString, CountArray(AcquireACResult.szNoticeString), CW2CT(DBVarValue.bstrVal).m_psz);
		}

		//手机号已绑定
		if (AcquireACResult.lResultCode == AUTHECODE_MP_BINDED)
		{
			AcquireACResult.dwBindUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("BindUserID"));
		}

		//验证码已发送
		if (AcquireACResult.lResultCode == AUTHECODE_AC_HAVESENT)
		{
			AcquireACResult.dwBindUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("BindUserID"));
		}

		//重置密码
		if (AcquireACResult.lResultCode == DB_SUCCESS && AcquireACResult.cbAuthKind == AUTH_KIND_RESETPASSWD)
		{
			AcquireACResult.dwBindUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("BindUserID"));
		}

		//发送消息
		WORD wStringSize = CountStringBuffer(AcquireACResult.szNoticeString);
		WORD wPacketSize = sizeof(AcquireACResult) - sizeof(AcquireACResult.szNoticeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_ACQUIREAC_RESULT, dwContextID, dwTokenID, &AcquireACResult, wPacketSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_MB_AcquireACResult AcquireACResult;
		ZeroMemory(&AcquireACResult, sizeof(AcquireACResult));

		//验证类型
		AcquireACResult.cbAuthKind = pAcquireAuthCode->cbAuthKind;

		//结果信息
		AcquireACResult.lResultCode = AUTHECODE_DB_EXCEPTION;
		StringCchCopy(AcquireACResult.szNoticeString, CountArray(AcquireACResult.szNoticeString), CWHService::FormatMessage(MIM_LOGON_0006, TEXT("由于数据库异常，请稍后再次尝试！")));

		//发送消息
		WORD wStringSize = CountStringBuffer(AcquireACResult.szNoticeString);
		WORD wPacketSize = sizeof(AcquireACResult) - sizeof(AcquireACResult.szNoticeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_ACQUIREAC_RESULT, dwContextID, dwTokenID, &AcquireACResult, wPacketSize);

		return false;
	}

	return true;
}

//绑定手机号
bool CDataBaseEngineSink::OnRequestMBBindMobilePhone(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_BindMobilePhone));
		if (wDataSize != sizeof(DBR_MB_BindMobilePhone)) return false;

		//请求处理
		DBR_MB_BindMobilePhone* pBindMobilePhone = (DBR_MB_BindMobilePhone*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE * pClientIP = (BYTE *)&pBindMobilePhone->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pBindMobilePhone->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pBindMobilePhone->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strNewPassword"), pBindMobilePhone->szNewPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strMachine"), pBindMobilePhone->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pBindMobilePhone->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCodeMD5"), pBindMobilePhone->szAuthCode);
		m_AccountsDBAide.AddParameter(TEXT("@dwUnbindUserID"), pBindMobilePhone->dwUnbindUserID);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_BindMobilePhone"), true);

		//获取信息
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			DBO_MB_BindMPSuccess BindMPSuccess;
			ZeroMemory(&BindMPSuccess, sizeof(BindMPSuccess));

			//绑定号码
			StringCchCopy(BindMPSuccess.szMobilePhone, CountArray(BindMPSuccess.szMobilePhone), pBindMobilePhone->szMobilePhone);

			//财富信息
			BindMPSuccess.lUserCard = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserCard"));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量			
			StringCchCopy(BindMPSuccess.szNoticeString, CountArray(BindMPSuccess.szNoticeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(BindMPSuccess.szNoticeString);
			WORD wPacketSize = sizeof(BindMPSuccess) - sizeof(BindMPSuccess.szNoticeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_BINDMP_SUCCESS, dwContextID, dwTokenID, &BindMPSuccess, wPacketSize);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_BIND_MOBILEPHONE;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = SUB_MB_BIND_MOBILEPHONE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//解绑手机号
bool CDataBaseEngineSink::OnRequestMBUnbindMobilePhone(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT (wDataSize == sizeof(DBR_MB_UnbindMobilePhone));
		if (wDataSize != sizeof(DBR_MB_UnbindMobilePhone)) return false;

		//请求处理
		DBR_MB_UnbindMobilePhone* pUnBindMobilePhone = (DBR_MB_UnbindMobilePhone*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE * pClientIP = (BYTE *)&pUnBindMobilePhone->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUnBindMobilePhone->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pUnBindMobilePhone->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strMachine"), pUnBindMobilePhone->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pUnBindMobilePhone->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCodeMD5"), pUnBindMobilePhone->szAuthCode);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_UnBindMobilePhone"), true);

		//获取信息
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			DBO_MB_UnbindMPSuccess UnBindMPSuccess;
			ZeroMemory(&UnBindMPSuccess, sizeof(UnBindMPSuccess));

			//财富信息
			UnBindMPSuccess.lUserCard = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserCard"));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量			
			StringCchCopy(UnBindMPSuccess.szNoticeString, CountArray(UnBindMPSuccess.szNoticeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(UnBindMPSuccess.szNoticeString);
			WORD wPacketSize = sizeof(UnBindMPSuccess) - sizeof(UnBindMPSuccess.szNoticeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_UNBINDMP_SUCCESS, dwContextID, dwTokenID, &UnBindMPSuccess, wPacketSize);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_UNBIND_MOBILEPHONE;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = SUB_MB_UNBIND_MOBILEPHONE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//重置密码
bool CDataBaseEngineSink::OnRequestMBResetLogonPasswd(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_ResetLogonPasswd));
		if (wDataSize != sizeof(DBR_MB_ResetLogonPasswd)) return false;

		//请求处理
		DBR_MB_ResetLogonPasswd* pResetLogonPasswd = (DBR_MB_ResetLogonPasswd*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwBindUserID"), pResetLogonPasswd->dwBindUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pResetLogonPasswd->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strMachine"), pResetLogonPasswd->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@cbCodeKind"), pResetLogonPasswd->cbCodeKind);
		m_AccountsDBAide.AddParameter(TEXT("@strMobileEmail"), pResetLogonPasswd->szMobileEmail);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCodeMD5"), pResetLogonPasswd->szAuthCode);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_ResetLogonPasswd"), false);

		//获取信息
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			DBO_MB_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateSuccess.lResultCode = 0;
			OperateSuccess.wRequestCmdID = SUB_MB_RESET_LOGON_PASS;
			StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wPacketSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wPacketSize);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_RESET_LOGON_PASS;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wPacketSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = SUB_MB_RESET_LOGON_PASS;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wPacketSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);

		return false;
	}

	return true;
}


//请求签到
bool CDataBaseEngineSink::OnRequestMBRequestCheckIn(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_RequestCheckIn));
		if (wDataSize != sizeof(DBR_MB_RequestCheckIn)) return false;

		//提取数据
		auto pRequestCheckIn = (DBR_MB_RequestCheckIn*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pRequestCheckIn->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pRequestCheckIn->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@cbQuery"), pRequestCheckIn->bQuery);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyCheckIn"), true);

		//获取信息
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			DBO_MB_CheckInResult CheckInResult;
			ZeroMemory(&CheckInResult, sizeof(CheckInResult));

			//签到信息
			CheckInResult.dwUserID = pRequestCheckIn->dwUserID;
			CheckInResult.wCheckInDays = m_AccountsDBAide.GetValue_WORD(TEXT("CheckInDays"));
			CheckInResult.cbCheckInState = m_AccountsDBAide.GetValue_BYTE(TEXT("CheckInState"));

			//奖励金额
			tagGoodsItem GoodsItem;
			ZeroMemory(&GoodsItem, sizeof(GoodsItem));
			GoodsItem.wGoodsID = m_AccountsDBAide.GetValue_LONG(TEXT("RewardGoodsID"));
			GoodsItem.dwGoodsCount = m_AccountsDBAide.GetValue_LONG(TEXT("RewardAmount"));

			//序列化
			g_GameGoodsManager->SerializeToJson(&GoodsItem, 1, CheckInResult.szRewardGoodsList, CountArray(CheckInResult.szRewardGoodsList));

			//发送结果
			WORD wStringSize = CountStringBuffer(CheckInResult.szRewardGoodsList);
			WORD wPacketSize = sizeof(CheckInResult) - sizeof(CheckInResult.szRewardGoodsList) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_CHECKIN_RESULT, dwContextID, dwTokenID, &CheckInResult, wPacketSize);

			//更新财富
			UpdateUserWealth(CheckInResult.dwUserID, dwContextID, dwTokenID);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_BIND_INVITE_CODE;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = SUB_MB_BIND_INVITE_CODE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//绑定邀请码
bool CDataBaseEngineSink::OnRequestMBBindInviteCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
    try
    {
        //效验参数
        ASSERT(wDataSize == sizeof(DBR_MB_BindInviteCode));
        if (wDataSize != sizeof(DBR_MB_BindInviteCode)) return false;

        //请求处理
		DBR_MB_BindInviteCode* pBindInviteCode = (DBR_MB_BindInviteCode*)pData;

        //转化地址
        TCHAR szClientIP[16] = TEXT("");
        BYTE* pClientIP = (BYTE*)&pBindInviteCode->dwClientIP;
        _sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

        //构造参数
        m_AccountsDBAide.ResetParameter();
        m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pBindInviteCode->dwUserID);
        m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pBindInviteCode->szPassword);
        m_AccountsDBAide.AddParameter(TEXT("@dwInviteCode"), pBindInviteCode->dwInviteCode);
		m_AccountsDBAide.AddParameter(TEXT("@strMachine"), pBindInviteCode->szMachineID);
        m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);

        //输出变量
        TCHAR szDescribe[128] = TEXT("");
        m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

        //执行查询
        LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_BindInviteCode"), true);

        //获取信息
        if (lReturnValue == DB_SUCCESS)
        {
            //变量定义
			DBO_MB_BindInviteCode BindInviteCode;
            ZeroMemory(&BindInviteCode, sizeof(BindInviteCode));

			BindInviteCode.dwUserID = pBindInviteCode->dwUserID;
			BindInviteCode.dwParentGameID = m_AccountsDBAide.GetValue_DWORD(TEXT("ParentGameID"));
			BindInviteCode.wParentFaceID = m_AccountsDBAide.GetValue_WORD(TEXT("ParentFaceID"));
			BindInviteCode.dwMailID = m_AccountsDBAide.GetValue_DWORD(TEXT("MailID"));
			m_AccountsDBAide.GetValue_String(TEXT("ParentNickName"), BindInviteCode.szParentNickName, CountArray(BindInviteCode.szParentNickName));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量			
			StringCchCopy(BindInviteCode.szNoticeString, CountArray(BindInviteCode.szNoticeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(BindInviteCode.szNoticeString);
			WORD wPacketSize = sizeof(BindInviteCode) - sizeof(BindInviteCode.szNoticeString) + wStringSize;
            m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_BIND_INVITE_CODE, dwContextID, dwTokenID, &BindInviteCode, wPacketSize);
        }
        else
        {
            //变量定义
            DBO_MB_OperateFailure OperateFailure;
            ZeroMemory(&OperateFailure, sizeof(OperateFailure));

            //获取信息
            CDBVarValue DBVarValue;
            m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

            //构造变量
            OperateFailure.wRequestCmdID = SUB_MB_BIND_INVITE_CODE;
            OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
            StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

            //发送结果
            WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
            WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
            m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
        }

        return true;
    }
    catch (IDataBaseException* pIException)
    {
        //输出错误
        LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
        TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

        //变量定义
        DBO_MB_OperateFailure OperateFailure;
        ZeroMemory(&OperateFailure, sizeof(OperateFailure));

        //设置变量
        OperateFailure.wRequestCmdID = SUB_MB_BIND_INVITE_CODE;
        OperateFailure.lErrorCode = pIException->GetExceptionResult();
        StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

        //发送结果
        WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
        WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
        m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

        return false;
    }

    return true;
}

//提交反馈
bool CDataBaseEngineSink::OnRequestMBSubmitInformInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize <= sizeof(DBR_MB_SubmitInformInfo));
		if (wDataSize > sizeof(DBR_MB_SubmitInformInfo)) return false;

		//请求处理
		auto pSubmitInformInfo = (DBR_MB_SubmitInformInfo*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pSubmitInformInfo->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
        m_AccountsDBAide.AddParameter(TEXT("@cbInformKind"), pSubmitInformInfo->cbInformKind);
        m_AccountsDBAide.AddParameter(TEXT("@cbInformSource"), pSubmitInformInfo->cbInformSource);
		m_AccountsDBAide.AddParameter(TEXT("@dwInformUserID"), pSubmitInformInfo->dwInformUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwBeInformUserID"), pSubmitInformInfo->dwBeInformUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strAttachmentUrl"), pSubmitInformInfo->szAttachmentUrl);
        m_AccountsDBAide.AddParameter(TEXT("@strInformMessage"), pSubmitInformInfo->szInformMessage);
        m_AccountsDBAide.AddParameter(TEXT("@strInformContent"), pSubmitInformInfo->szInformContent);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		
		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_SubmitInformInfo"), false);

		//获取信息
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			DBO_MB_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//构造变量
			OperateSuccess.lResultCode = lReturnValue;
			OperateSuccess.wRequestCmdID = SUB_MB_SUBMIT_INFORM_INFO;						

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wSendSize);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.wRequestCmdID = SUB_MB_SUBMIT_INFORM_INFO;			
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = SUB_MB_SUBMIT_INFORM_INFO;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//绑定账号
bool CDataBaseEngineSink::OnRequestMBBindExternalAccount(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize >= sizeof(DBR_MB_BindExternalAccount));
		if (wDataSize < sizeof(DBR_MB_BindExternalAccount)) return false;

		//请求处理
		auto pBindExternalAccount = (DBR_MB_BindExternalAccount*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pBindExternalAccount->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pBindExternalAccount->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwExternalID"), pBindExternalAccount->dwExternalID);
		m_AccountsDBAide.AddParameter(TEXT("@strUniqueID"), pBindExternalAccount->szUniqueID);
		m_AccountsDBAide.AddParameter(TEXT("@strFacePath"), pBindExternalAccount->szUserFaceUrl);
		m_AccountsDBAide.AddParameter(TEXT("@strUserDatum"), pBindExternalAccount->szUserDatum);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_BindExternalAccounts"), true);

		//获取信息
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			DBO_MB_BindExternalAccount BindExternalAccount;
			ZeroMemory(&BindExternalAccount, sizeof(BindExternalAccount));

			//设置变量
			BindExternalAccount.dwExternalID = pBindExternalAccount->dwExternalID;
			StringCchCopy(BindExternalAccount.szUniqueID,CountArray(BindExternalAccount.szUniqueID), pBindExternalAccount->szUniqueID);

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量			
			StringCchCopy(BindExternalAccount.szNoticeString, CountArray(BindExternalAccount.szNoticeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(BindExternalAccount.szNoticeString);
			WORD wPacketSize = sizeof(BindExternalAccount) - sizeof(BindExternalAccount.szNoticeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_BIND_EXTERNAL_ACCOUNT, dwContextID, dwTokenID, &BindExternalAccount, wPacketSize);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_BIND_EXTERNAL_ACCOUNT;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = SUB_MB_BIND_EXTERNAL_ACCOUNT;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//货币转账
bool CDataBaseEngineSink::OnRequestMBWealthTranser(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_WealthTransfer));
		if (wDataSize != sizeof(DBR_MB_WealthTransfer)) return false;

		//请求处理
		auto pWealthTransfer = (DBR_MB_WealthTransfer*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pWealthTransfer->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pWealthTransfer->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwTargetGameID"), pWealthTransfer->dwTargetGameID);
		m_AccountsDBAide.AddParameter(TEXT("@cbCodeKind"), pWealthTransfer->cbCodeKind);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCode"), pWealthTransfer->szAuthCode);
		m_AccountsDBAide.AddParameter(TEXT("@lTransferAmount"), pWealthTransfer->lTransferAmount);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_WEALTH_TRANSFER);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pWealthTransfer->szMachineID);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_WealthTransfer"), true);

		//获取信息
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_MB_WealthTransferSuccess WealthTransferSuccess;
			ZeroMemory(&WealthTransferSuccess,sizeof(WealthTransferSuccess));

			//设置变量
			WealthTransferSuccess.dwUserID = pWealthTransfer->dwUserID;
			WealthTransferSuccess.dwMailID = m_AccountsDBAide.GetValue_DWORD(TEXT("MailID"));
			WealthTransferSuccess.dwTargetUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("TargetUserID"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_WEALTH_TRANSFER_SUCCESS, dwContextID, dwTokenID, &WealthTransferSuccess, sizeof(WealthTransferSuccess));

			//更新财富
			UpdateUserWealth(pWealthTransfer->dwUserID, dwContextID, dwTokenID);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_WEALTH_TRANSFER;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = SUB_MB_WEALTH_TRANSFER;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//取出佣金
bool CDataBaseEngineSink::OnRequestMBDrawCommission(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_DrawCommission));
		if (wDataSize != sizeof(DBR_MB_DrawCommission)) return false;

		//请求处理
		auto pDrawCommission = (DBR_MB_DrawCommission*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pDrawCommission->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pDrawCommission->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@lDrawAmount"), pDrawCommission->lDrawAmount);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_DRAW_COMMISSION);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pDrawCommission->szMachineID);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_DrawCommission"), true);

		//获取信息
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			DBO_MB_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateSuccess.wRequestCmdID = SUB_MB_DRAW_COMMISSION;
			OperateSuccess.lResultCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wSendSize);

			//更新财富
			UpdateUserWealth(pDrawCommission->dwUserID, dwContextID, dwTokenID);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_DRAW_COMMISSION;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = SUB_MB_DRAW_COMMISSION;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//赠送礼物
bool CDataBaseEngineSink::OnRequestMBSendGift(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
    try
    {
        //效验参数
        ASSERT(wDataSize >= sizeof(DBR_MB_SendGift));
        if (wDataSize < sizeof(DBR_MB_SendGift)) return false;

        //请求处理
        auto pSendGift = (DBR_MB_SendGift*)pData;

        //转化地址
        TCHAR szClientIP[16] = TEXT("");
        BYTE* pClientIP = (BYTE*)&pSendGift->dwClientIP;
        _sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

        //构造参数
        m_AccountsDBAide.ResetParameter();
        m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pSendGift->dwUserID);
        m_AccountsDBAide.AddParameter(TEXT("@dwTargetUserID"), pSendGift->dwTargetUserID);
        m_AccountsDBAide.AddParameter(TEXT("@dwGoodsID"), pSendGift->dwGoodsID);
        m_AccountsDBAide.AddParameter(TEXT("@dwGoodsCount"), pSendGift->dwGoodsCount);
        m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);

        //输出变量
        TCHAR szDescribe[128] = TEXT("");
        m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

        //执行查询
        LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_SendGift"), true);

        //获取信息
        if (lReturnValue == DB_SUCCESS)
        {
            //变量定义
			DBO_MB_SendGiftSuccess OperateSuccess;
            ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

            //构造变量
            OperateSuccess.dwUserID = pSendGift->dwUserID;
            OperateSuccess.dwTargetUserID = pSendGift->dwTargetUserID;
            OperateSuccess.dwGoodsID = pSendGift->dwGoodsID;
            OperateSuccess.dwGoodsCount = pSendGift->dwGoodsCount;
            OperateSuccess.dwRecordID = m_AccountsDBAide.GetValue_DWORD(TEXT("RecordID"));
            OperateSuccess.dwLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));
            OperateSuccess.dwMyLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("MyLoveLiness"));
            OperateSuccess.dwTargetLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("TargetLoveLiness"));
            OperateSuccess.cbSendType = m_AccountsDBAide.GetValue_BYTE(TEXT("SendType"));

            //更新财富
            UpdateUserWealth(pSendGift->dwUserID, dwContextID, dwTokenID);

			//构造结构
			UserPublicInfo PublicInfo;
			PublicInfo.set_uid(pSendGift->dwUserID);
			PublicInfo.set_loveliness(OperateSuccess.dwMyLoveLiness);

			//更新信息
			UpdateUserCache(dwContextID, dwTokenID, PublicInfo);

            //发送结果
            m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_SEND_GIFT_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, sizeof(OperateSuccess));
        }
        else
        {
            //变量定义
            DBO_MB_OperateFailure OperateFailure;
            ZeroMemory(&OperateFailure, sizeof(OperateFailure));

            //获取信息
            CDBVarValue DBVarValue;
            m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

            //构造变量
            OperateFailure.wRequestCmdID = SUB_MB_SEND_GIFT;
            OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
            StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

            //发送结果
            WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
            WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
            m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
        }

        return true;
    }
    catch (IDataBaseException* pIException)
    {
        //输出错误
        LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
        TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

        //变量定义
        DBO_MB_OperateFailure OperateFailure;
        ZeroMemory(&OperateFailure, sizeof(OperateFailure));

        //设置变量
        OperateFailure.wRequestCmdID = SUB_MB_SEND_GIFT;
        OperateFailure.lErrorCode = pIException->GetExceptionResult();
        StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

        //发送结果
        WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
        WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
        m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

        return false;
    }

    return true;
}

//加载礼物
bool CDataBaseEngineSink::OnRequestMBLoadGift(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
    try
    {
        //效验参数
        ASSERT(wDataSize >= sizeof(DBR_MB_LoadGift));
        if (wDataSize < sizeof(DBR_MB_LoadGift)) return false;

        //请求处理
        auto pLoadGift = (DBR_MB_LoadGift*)pData;

        //构造参数
        m_AccountsDBAide.ResetParameter();
        m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pLoadGift->dwUserID);
        //m_AccountsDBAide.AddParameter(TEXT("@dwRecordID"), pLoadGift->dwRecordID);

        //输出变量
        TCHAR szDescribe[128] = TEXT("");
        m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

        //执行查询
        LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGift"), true);

        //获取信息
        if (lReturnValue == DB_SUCCESS)
        {
            //定义变量
			DBO_MB_UserGiftInfo UserGiftInfo;
            ZeroMemory(&UserGiftInfo, sizeof(UserGiftInfo));

            //遍历数据
            while (m_AccountsDBModule->IsRecordsetEnd() == false)
            {
                if (UserGiftInfo.wItemCount >= CountArray(UserGiftInfo.GiftInfo))
                {
                    //发送数据
                    WORD wHeadSize = sizeof(UserGiftInfo) - sizeof(UserGiftInfo.GiftInfo);
                    WORD wDataSize = UserGiftInfo.wItemCount * sizeof(UserGiftInfo.GiftInfo[0]);
                    m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_GIFT_INFO, dwContextID, dwTokenID, &UserGiftInfo, wHeadSize + wDataSize);

                    //重置数量
					UserGiftInfo.wItemCount = 0;
                }

                //礼物信息
                tagGiftInfo* pGiftInfo = &UserGiftInfo.GiftInfo[UserGiftInfo.wItemCount++];

                //读取数据
                pGiftInfo->dwRecordID = m_AccountsDBAide.GetValue_DWORD(TEXT("RecordID"));
                pGiftInfo->dwUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
                pGiftInfo->dwGoodsID = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsID"));
                pGiftInfo->dwGoodsCount = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsCount"));
                pGiftInfo->dwLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));
                m_AccountsDBAide.GetValue_String(TEXT("NickName"), pGiftInfo->szNickName, CountArray(pGiftInfo->szNickName));
                m_AccountsDBAide.GetValue_SystemTime(TEXT("CollectDateTime"), pGiftInfo->SendDate);

                //移动记录
                m_AccountsDBModule->MoveToNext();
            }

            //发送数据
            WORD wHeadSize = sizeof(UserGiftInfo) - sizeof(UserGiftInfo.GiftInfo);
            WORD wDataSize = UserGiftInfo.wItemCount * sizeof(UserGiftInfo.GiftInfo[0]);
            m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_GIFT_INFO, dwContextID, dwTokenID, &UserGiftInfo, wHeadSize + wDataSize);
        }
		
        //礼物信息完成
        m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_GIFT_INFO_FINISH, dwContextID, dwTokenID, NULL, NULL);

        return true;
    }
    catch (IDataBaseException* pIException)
    {
        //输出错误
        LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
        TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

        //变量定义
        DBO_MB_OperateFailure OperateFailure;
        ZeroMemory(&OperateFailure, sizeof(OperateFailure));

        //设置变量
        OperateFailure.wRequestCmdID = SUB_MB_LOAD_GIFT;
        OperateFailure.lErrorCode = pIException->GetExceptionResult();
        StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

        //发送结果
        WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
        WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
        m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

        return false;
    }

    return true;
}

//礼物状态更新
bool CDataBaseEngineSink::OnRequestMBGiftStatusUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
    try
    {
        //效验参数
        ASSERT(wDataSize >= sizeof(DBR_MB_GiftStatusUpdata));
        if (wDataSize < sizeof(DBR_MB_GiftStatusUpdata)) return false;

        //请求处理
        auto pGiftStatusUpdata = (DBR_MB_GiftStatusUpdata*)pData;

        //构造参数
        m_AccountsDBAide.ResetParameter();
        m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pGiftStatusUpdata->dwUserID);
        m_AccountsDBAide.AddParameter(TEXT("@dwRecordID"), pGiftStatusUpdata->dwRecordID);

        //输出变量
        TCHAR szDescribe[128] = TEXT("");
        m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

        //执行查询
        LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GiftStatusUpdate"), true);

        return true;
    }
    catch (IDataBaseException* pIException)
    {
        //输出错误
        LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
        TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

        //变量定义
        DBO_MB_OperateFailure OperateFailure;
        ZeroMemory(&OperateFailure, sizeof(OperateFailure));

        //设置变量
        OperateFailure.wRequestCmdID = SUB_MB_GIFT_STATUS_UPDATE;
        OperateFailure.lErrorCode = pIException->GetExceptionResult();
        StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

        //发送结果
        WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
        WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
        m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

        return false;
    }

    return true;
}

//查询财富
bool CDataBaseEngineSink::OnRequestMBQueryWealth(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_MB_QueryWealth));
		if (wDataSize!=sizeof(DBR_MB_QueryWealth)) return false;

		//请求处理
		DBR_MB_QueryWealth* pQueryWealth=(DBR_MB_QueryWealth*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pQueryWealth->dwUserID);

		//执行查询
		LONG lReturnValue=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryGameWealth"),true);

		//获取信息
		if (lReturnValue==DB_SUCCESS)
		{
			//更新财富
			DBO_MB_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

			//设置变量
			WealthUpdate.wItemCount = 2;
			WealthUpdate.WealthItem[0].wMoneyID = GOODS_ID_CARD;
			WealthUpdate.WealthItem[0].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserCard"));
			WealthUpdate.WealthItem[1].wMoneyID = GOODS_ID_GOLD;
			WealthUpdate.WealthItem[1].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));

			//发送结果			
			WORD wHeadSize = sizeof(WealthUpdate) - sizeof(WealthUpdate.WealthItem);
			WORD wPacketSize = WealthUpdate.wItemCount * sizeof(WealthUpdate.WealthItem[0]) + wHeadSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_WEALTH_UPDATE, dwContextID, dwTokenID, &WealthUpdate, wPacketSize);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_QUERY_WEALTH;
			OperateFailure.lErrorCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE,dwContextID, dwTokenID, &OperateFailure,wSendSize);
		}
		
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = SUB_MB_QUERY_WEALTH;
		OperateFailure.lErrorCode=pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE,dwContextID, dwTokenID, &OperateFailure,wSendSize);

		return false;
	}

	return true;
}

//查询排位
bool CDataBaseEngineSink::OnRequestMBQueryRankingInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_QueryRankingInfo));
		if (wDataSize != sizeof(DBR_MB_QueryRankingInfo)) return false;

		//请求处理
		DBR_MB_QueryRankingInfo* pQueryRankingInfo = (DBR_MB_QueryRankingInfo*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pQueryRankingInfo->dwUserID);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadUserRankingInfo"), true);

		//获取信息
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			tagUserRankingInfo UserRankingInfo;
			ZeroMemory(&UserRankingInfo,sizeof(UserRankingInfo));

			//构造结构
			UserRankingInfo.dwRankID = m_AccountsDBAide.GetValue_DWORD(TEXT("RankID"));
			UserRankingInfo.wSeasonID = m_AccountsDBAide.GetValue_WORD(TEXT("SeasonID"));
			UserRankingInfo.lWinCount = m_AccountsDBAide.GetValue_LONG(TEXT("WinCount"));
			UserRankingInfo.lLostCount = m_AccountsDBAide.GetValue_LONG(TEXT("LostCount"));
			UserRankingInfo.lLoveHeart = m_AccountsDBAide.GetValue_LONG(TEXT("LoveHeart"));
			UserRankingInfo.lRankScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("RankScore"));
			UserRankingInfo.lWinStreakCount = m_AccountsDBAide.GetValue_LONG(TEXT("WinStreakCount"));			

			//发送结果			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_RANKING_INFO, dwContextID, dwTokenID, &UserRankingInfo, sizeof(UserRankingInfo));

		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_QUERY_RANKINGINFO;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = SUB_MB_QUERY_RANKINGINFO;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//查询魅力
bool CDataBaseEngineSink::OnRequestMBQueryLoveLiness(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
    //效验参数
    ASSERT(wDataSize == sizeof(DBR_MB_QueryLoveLiness));
    if (wDataSize != sizeof(DBR_MB_QueryLoveLiness)) return false;

    //请求处理
	DBR_MB_QueryLoveLiness* pQueryLoveLiness = (DBR_MB_QueryLoveLiness*)pData;

	//更新魅力
	UpdateUserLoveLiness(pQueryLoveLiness->dwUserID, dwContextID, dwTokenID);

	return true;
}
//查询低保
bool CDataBaseEngineSink::OnRequestMBQuerySubsidy(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_SubsidyQuery));
		if (wDataSize != sizeof(DBR_MB_SubsidyQuery)) return false;

		//请求处理
		DBR_MB_SubsidyQuery* pQuerySubsidy = (DBR_MB_SubsidyQuery*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pQuerySubsidy->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@szMachine"), pQuerySubsidy->szMachineID);

		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QuerySubsidy"), true);

		//获取信息
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBR_MB_Subsidy Subsidy;
			ZeroMemory(&Subsidy, sizeof(Subsidy));

			//构造结构
			Subsidy.cbDailyTimes = m_AccountsDBAide.GetValue_BYTE(TEXT("ClaimTimesPerAccounts"));
			Subsidy.cbResidualTimes = m_AccountsDBAide.GetValue_BYTE(TEXT("ClaimTimes"));
			Subsidy.lConsumeScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("ClaimCondition"));
			Subsidy.cbResidualTimes = Subsidy.cbDailyTimes <= Subsidy.cbResidualTimes ? 0 : Subsidy.cbDailyTimes - Subsidy.cbResidualTimes;
			//发送结果			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_RESULT_SUBSIDY, dwContextID, dwTokenID, &Subsidy, sizeof(DBR_MB_Subsidy));

		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_USER_SUBSIDY;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = SUB_MB_USER_SUBSIDY;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
};
//加载配置
bool CDataBaseEngineSink::OnRequestLoadConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//提取数据
	auto pLoadConfig = (DBR_GP_LoadConfig*)pData;

	//构造结果
	DBO_GP_ConfigResult ConfigResult;
	ZeroMemory(&ConfigResult, sizeof(ConfigResult));

	//设置掩码
	ConfigResult.bInitService = pLoadConfig->bInitService;
	ConfigResult.dwConfigMask = pLoadConfig->dwConfigMask;

	do
	{
		//游戏列表
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_GAME_LIST)
		{
			ConfigResult.bResultCode = LoadGameList(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//版本信息
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_PLAZA_VERSION)
		{
			ConfigResult.bResultCode = LoadPlazaVersion(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//敏感词汇
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_FILTER_WORDS)
		{
			ConfigResult.bResultCode = LoadFilterWords(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//转账参数
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_TRANSFER_PARAM)
		{
			ConfigResult.bResultCode = LoadTransferParemter(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;			
		}

		//全局配置
		if (pLoadConfig->dwConfigMask & GLOBAL_OPTION_MASK)
		{
			ConfigResult.bResultCode = LoadGlobalOption(dwContextID, dwTokenID, pLoadConfig->dwConfigMask);
			if (!ConfigResult.bResultCode) break;
		}

		//昵称配置
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_NICKANME_OPTION)
		{
			ConfigResult.bResultCode = LoadNickNameOption(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

	} while (false);

	//发送结果
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CONFIG_RESULT, dwContextID, dwTokenID, &ConfigResult, sizeof(ConfigResult));

	return true;
}

//加载榜单
bool CDataBaseEngineSink::OnRequestLoadRankingList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//变量定义
		DBO_GP_RankingListData RankingListData;
		ZeroMemory(&RankingListData,sizeof(RankingListData));

		//类型定义
		BYTE cbListType[] = { LIST_TYPE_PAY,LIST_TYPE_WEALTH,LIST_TYPE_BIGWINNER,LIST_TYPE_SPREAD,LIST_TYPE_COMMISSION };

		for (int i = 0; i < CountArray(cbListType); i++)
		{
			//构造Key
			string sKey = RKEY_PREFIX_RANKINGLIST + to_string(cbListType[i]);

			//获取数据
			auto sRankingList = m_pMasterClient->get(sKey);
			if (sRankingList)
			{
				//拷贝数据
				RankingListData.cbListType = cbListType[i];
				StringCchCopyA(RankingListData.szRankingList,CountArray(RankingListData.szRankingList),sRankingList->c_str());

				//发送结果
				WORD wListSize = CountStringBufferA(RankingListData.szRankingList);
				WORD wPacketSize = sizeof(RankingListData) - sizeof(RankingListData.szRankingList) + wListSize;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_RANKING_LIST_DATA, dwContextID, dwTokenID, &RankingListData, wPacketSize);
			}
		}

		return true;
	}
	catch (exception& ex)
	{
		CTraceService::TraceString(TEXT("%hs"), TraceLevel_Exception, ex.what());

		//延迟处理
		*pdwControlCode |= DB_RESULT_DEFER_DATA;
	}

	return false;
}

//重置数据
bool CDataBaseEngineSink::OnRequestResetUserData(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//提取变量
		auto pResetUserData = (DBR_GP_ResetUserData*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wDayOfWeek"), pResetUserData->wDayOfWeek);
		m_AccountsDBAide.AddParameter(TEXT("@strUserIDList"), pResetUserData->szUserIDList);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ResetUserData"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//在线信息
bool CDataBaseEngineSink::OnRequestOnLineCountInfo(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//变量定义
		DBR_GP_OnLineCountInfo * pOnLineCountInfo=(DBR_GP_OnLineCountInfo *)pData;
		WORD wHeadSize=(sizeof(DBR_GP_OnLineCountInfo)-sizeof(pOnLineCountInfo->OnLineCountKind));

		//效验数据
		ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOnLineCountInfo->wKindCount*sizeof(tagOnLineInfoKind))));
		if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pOnLineCountInfo->wKindCount*sizeof(tagOnLineInfoKind)))) return false;

		//机器标识
		WCHAR szMachineID[LEN_MACHINE_ID];
		CWHService::GetMachineID(szMachineID);

		//构造信息
		TCHAR szOnLineCountKind[2048]=TEXT("");
		for (WORD i=0;i<pOnLineCountInfo->wKindCount;i++)
		{
			INT nLength=lstrlen(szOnLineCountKind);
			INT nBufferCount = CountArray(szOnLineCountKind)-nLength;
			_sntprintf_s(&szOnLineCountKind[nLength], nBufferCount, nBufferCount,TEXT("%d:%ld;"),pOnLineCountInfo->OnLineCountKind[i].wKindID,
				pOnLineCountInfo->OnLineCountKind[i].dwOnLineCount);
		}

		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),szMachineID);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineServer"),m_pInitParameter->m_szServerName);
		m_PlatformDBAide.AddParameter(TEXT("@dwOnLineCountSum"),pOnLineCountInfo->dwOnLineCountSum);
		m_PlatformDBAide.AddParameter(TEXT("@strOnLineCountKind"),szOnLineCountKind);

		//执行命令
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_OnLineCountInfo"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//库存结果
VOID CDataBaseEngineSink::OnStockDisposeResult(DWORD dwContextID, DWORD dwTokenID, DWORD dwErrorCode, WORD wSubCommdID)
{
	//缓冲定义
	DBO_GP_StockOperateResult StockOperateResult = { 0 };

	//设置命令
	StockOperateResult.wPacketIdx = 0;
	StockOperateResult.wSubCommdID = wSubCommdID;

	//读取信息
	while (m_PlatformDBModule->IsRecordsetEnd() == false)
	{
		//获取对象
		auto pStockInfo = &StockOperateResult.StockList[StockOperateResult.wStockCount++];

		//读取信息
		pStockInfo->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
		pStockInfo->wStockID = m_PlatformDBAide.GetValue_WORD(TEXT("StockID"));
		pStockInfo->wStockKind = m_PlatformDBAide.GetValue_WORD(TEXT("StockKind"));
		pStockInfo->wStockLevel = m_PlatformDBAide.GetValue_WORD(TEXT("StockLevel"));
		pStockInfo->cbStockState = m_PlatformDBAide.GetValue_BYTE(TEXT("StockState"));
		pStockInfo->lBetAmount = m_PlatformDBAide.GetValue_LONG(TEXT("BetAmount"));

		//启用配置
		pStockInfo->cbEnableMode = m_PlatformDBAide.GetValue_BYTE(TEXT("EnableMode"));
		pStockInfo->dwEnableEndTime = m_PlatformDBAide.GetValue_DWORD(TEXT("EnableEndTime"));
		pStockInfo->dwEnableStartTime = m_PlatformDBAide.GetValue_DWORD(TEXT("EnableStartTime"));

		//积分配置
		pStockInfo->lStockScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("StockScore"));
		pStockInfo->lExtraScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("ExtraScore"));

		//抽水利润
		pStockInfo->lShrinkProfit = m_PlatformDBAide.GetValue_LONGLONG(TEXT("ShrinkProfit"));

		//库存名称
		m_PlatformDBAide.GetValue_String(TEXT("StockName"), pStockInfo->szStockName, CountArray(pStockInfo->szStockName));

		//关联标识
		m_PlatformDBAide.GetValue_String(TEXT("RelationIDList"), pStockInfo->szRelationStockID, CountArray(pStockInfo->szRelationStockID));

		//抽水配置
		m_PlatformDBAide.GetValue_String(TEXT("ShrinkOption"), pStockInfo->szShrinkOption, CountArray(pStockInfo->szShrinkOption));

		//奖池配置
		CHAR szJackpotOption[2048];
		m_PlatformDBAide.GetValue_String(TEXT("JackpotOption"), szJackpotOption, CountArray(szJackpotOption));

		//解析配置
		if (szJackpotOption[0] != 0)
		{
			pStockInfo->wJackpotCount = ParseJackpotOption(szJackpotOption, pStockInfo->GameJackpotItem,CountArray(pStockInfo->GameJackpotItem));
		}

		//移动记录
		m_PlatformDBModule->MoveToNext();

		//发送判断
		if (StockOperateResult.wStockCount >= CountArray(StockOperateResult.StockList))
		{
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_STOCK_OPERATE_RESULT, dwContextID, dwTokenID, &StockOperateResult, sizeof(StockOperateResult));

			StockOperateResult.wPacketIdx++;
			StockOperateResult.wStockCount = 0;			
		}
	}

	//发送判断
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_STOCK_OPERATE_RESULT, dwContextID, dwTokenID, &StockOperateResult, sizeof(StockOperateResult));
}

//机器结果
VOID CDataBaseEngineSink::OnRobotDisposeResult(DWORD dwContextID, DWORD dwTokenID, DWORD dwErrorCode,WORD wSubCommdID,WORD wServerID)
{
	//构造结构
	DBO_GP_RobotOperateResult RobotOperateResult;
	ZeroMemory(&RobotOperateResult,sizeof(RobotOperateResult));

	//设置变量
	RobotOperateResult.wSubCommdID=wSubCommdID;
	RobotOperateResult.wServerID=wServerID;

	if(dwErrorCode==DB_SUCCESS)
	{
		//变量定义
		WORD wParameterCount=0;

		while(m_AccountsDBModule->IsRecordsetEnd()==false)		
		{
			wParameterCount = RobotOperateResult.wParameterCount;
			RobotOperateResult.RobotParameter[wParameterCount].dwBatchID = m_AccountsDBAide.GetValue_DWORD(TEXT("BatchID"));
			RobotOperateResult.RobotParameter[wParameterCount].dwRobotCount = m_AccountsDBAide.GetValue_DWORD(TEXT("RobotCount"));
			RobotOperateResult.RobotParameter[wParameterCount].dwServiceMode = m_AccountsDBAide.GetValue_DWORD(TEXT("ServiceMode"));
			RobotOperateResult.RobotParameter[wParameterCount].dwEnterTime = m_AccountsDBAide.GetValue_DWORD(TEXT("EnterTime"));
			RobotOperateResult.RobotParameter[wParameterCount].dwLeaveTime = m_AccountsDBAide.GetValue_DWORD(TEXT("LeaveTime"));
			RobotOperateResult.RobotParameter[wParameterCount].dwEnterMinInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("EnterMinInterval"));
			RobotOperateResult.RobotParameter[wParameterCount].dwEnterMaxInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("EnterMaxInterval"));
			RobotOperateResult.RobotParameter[wParameterCount].dwLeaveMinInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("LeaveMinInterval"));
			RobotOperateResult.RobotParameter[wParameterCount].dwLeaveMaxInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("LeaveMaxInterval"));
			RobotOperateResult.RobotParameter[wParameterCount].dwSwitchMinInnings = m_AccountsDBAide.GetValue_DWORD(TEXT("SwitchMinInnings"));
			RobotOperateResult.RobotParameter[wParameterCount].dwSwitchMaxInnings = m_AccountsDBAide.GetValue_DWORD(TEXT("SwitchMaxInnings"));
			RobotOperateResult.RobotParameter[wParameterCount].lTakeMinScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("TakeMinScore"));
			RobotOperateResult.RobotParameter[wParameterCount].lTakeMaxScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("TakeMaxScore"));

			//设置变量
			RobotOperateResult.wParameterCount++;

			//溢出判断
			if(RobotOperateResult.wParameterCount>=CountArray(RobotOperateResult.RobotParameter))
			{
				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_ROBOT_OPERATE_RESULT,dwContextID, dwTokenID, &RobotOperateResult,sizeof(RobotOperateResult));

				//设置变量
				ZeroMemory(&RobotOperateResult,sizeof(RobotOperateResult));
			}

			//移动游标
			m_AccountsDBModule->MoveToNext();
		}	
	}
	
	//计算大小
	WORD wSendDataSize = sizeof(RobotOperateResult)-sizeof(RobotOperateResult.RobotParameter);
	wSendDataSize += RobotOperateResult.wParameterCount*sizeof(tagRobotParameter);

	//发送数据
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_ROBOT_OPERATE_RESULT,dwContextID, dwTokenID, &RobotOperateResult,wSendDataSize);
}


//解析彩金配置
WORD CDataBaseEngineSink::ParseJackpotOption(LPCSTR szJackpotOption, tagJackpotItem JackPotItem[], WORD wBufferCount)
{
	//获取对象
	auto JsonValuePtr = CWHJson::getValue(szJackpotOption);
	if (JsonValuePtr->getType() != eJsonTypeArray)
	{
		return 0;
	}

	//变量定义
	WORD wItemCount = 0;

	//获取对象
	auto JackpotArrayPtr = CJsonValueArrayPtr::dynamicCast(JsonValuePtr);

	for (auto JackpotItemPtr : JackpotArrayPtr->value)
	{
		if (JackpotItemPtr->getType() == eJsonTypeObj)
		{
			//转换对象
			auto JackpotItemObjPtr = CJsonValueObjPtr::dynamicCast(JackpotItemPtr);

			//获取对象
			auto pJackpotItem = &JackPotItem[wItemCount++];

			//读取配置
			CWHJsonInput::readJson(pJackpotItem->wLevelID, JackpotItemObjPtr->value["LevelID"], true);
			CWHJsonInput::readJson(pJackpotItem->lStockScore, JackpotItemObjPtr->value["StockScore"], true);
			CWHJsonInput::readJson(pJackpotItem->lExtraScore, JackpotItemObjPtr->value["ExtraScore"], true);
			CWHJsonInput::readJson(pJackpotItem->lVirtualScore, JackpotItemObjPtr->value["VirtualScore"], true);
			CWHJsonInput::readJson(pJackpotItem->wShrinkRatio, JackpotItemObjPtr->value["ShrinkRatio"], true);
			CWHJsonInput::readJson(pJackpotItem->wPayoutProb, JackpotItemObjPtr->value["PayoutProb"], true);
			CWHJsonInput::readJson(pJackpotItem->lPayoutLimit, JackpotItemObjPtr->value["PayoutLimit"], true);
			CWHJsonInput::readJson(pJackpotItem->lPayoutOrigin, JackpotItemObjPtr->value["PayoutOrigin"], true);
			CWHJsonInput::readJson(pJackpotItem->lInitialScore, JackpotItemObjPtr->value["FirstGold"], true);
			
			//退出判断
			if (wItemCount == wBufferCount) break;
		}
	}

	return wItemCount;
}

//序列化彩池
bool CDataBaseEngineSink::SerializeJackpotOption(tagJackpotItem JackpotItem[], WORD wItemCount, TCHAR szJackpotOption[], WORD wBufferSize)
{
	//构造Json
	CString strJackpotList, strJackpotItem;
	LPCTSTR pszJackpotFmt = L"{\"LevelID\":%d,\"StockScore\":%I64d,\"ExtraScore\":%I64d,\"VirtualScore\":%I64d,\"ShrinkRatio\":%d,\"PayoutProb\":%d,\"PayoutLimit\":%I64d,\"PayoutOrigin\":%I64d}";

	//格式化
	for (int i = 0; i < wItemCount; i++)
	{
		strJackpotItem.Format(pszJackpotFmt, JackpotItem[i].wLevelID, JackpotItem[i].lStockScore, JackpotItem[i].lExtraScore, JackpotItem[i].lVirtualScore, 
			JackpotItem[i].wShrinkRatio, JackpotItem[i].wPayoutProb, JackpotItem[i].lPayoutLimit, JackpotItem[i].lPayoutOrigin);

		//添加分隔符
		if (!strJackpotList.IsEmpty()) strJackpotList.AppendChar(L',');

		//添加子项
		strJackpotList.Append(strJackpotItem);
	}

	//添加括号
	strJackpotList.Insert(0, L'[');
	strJackpotList.AppendChar(L']');

	//校验缓冲大小
	if (strJackpotList.GetLength() > wBufferSize)
	{
		return false;
	}

	//拷贝字符串
	StringCchCopy(szJackpotOption,wBufferSize,strJackpotList.GetString());

	return true;
}

//序列化权重
bool CDataBaseEngineSink::SerializeWeightConfig(tagTimesWeightItem WeightItem[], WORD wItemCount, TCHAR szWeightConfig[], WORD wBufferSize)
{
	//构造Json
	CString strWeightList, strWeightItem;
	LPCTSTR pszWeightFmt = L"{\"MinTimes\":%d,\"MaxTimes\":%d,\"TimesKind\":%d,\"WeightValue\":%d}";

	//格式化
	for (int i = 0; i < wItemCount; i++)
	{
		strWeightItem.Format(pszWeightFmt, WeightItem[i].dwTimesRange[0], WeightItem[i].dwTimesRange[1], WeightItem[i].cbTimesKind, WeightItem[i].wTimesWeight);

		//添加分隔符
		if (!strWeightList.IsEmpty()) strWeightList.AppendChar(L',');

		//添加子项
		strWeightList.Append(strWeightItem);
	}

	//添加括号
	strWeightList.Insert(0, L'[');
	strWeightList.AppendChar(L']');

	//校验缓冲大小
	if (strWeightList.GetLength() > wBufferSize)
	{
		return false;
	}

	//拷贝字符串
	StringCchCopy(szWeightConfig, wBufferSize, strWeightList.GetString());

	return true;
}

//输出信息
void CDataBaseEngineSink::TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException)
{
	CString strMessage;
	strMessage.Format(TEXT("%s(%d):%hs(%s)"), CA2CT(pFile).m_psz, nLine, pFunction, pIException ? pIException->GetExceptionDescribe() : TEXT(""));
	CTraceService::TraceString(strMessage, TraceLevel_Exception);
}

//查询抽奖
bool CDataBaseEngineSink::OnRequestMBQueryTurntableLottery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_QueryTurntableLottery));
		if (wDataSize != sizeof(DBR_MB_QueryTurntableLottery)) return false;

		//请求处理
		auto pQueryTurntableLottery = (DBR_MB_QueryTurntableLottery*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pQueryTurntableLottery->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pQueryTurntableLottery->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwRecordID"), pQueryTurntableLottery->dwRecordID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pQueryTurntableLottery->szMachineID);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryTurntableLottery"), true);		

		//获取信息
		if (lReturnValue == DB_SUCCESS)
		{
			//抽奖数据
			DBO_MB_TurntableLotteryData TurntableLotteryData;
			ZeroMemory(&TurntableLotteryData, sizeof(TurntableLotteryData));

			//抽奖次数
			TurntableLotteryData.wPayLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("PayLotteryTimes"));
			TurntableLotteryData.wUseLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("UseLotteryTimes"));
			TurntableLotteryData.wFreeLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("FreeLotteryTimes"));						
			TurntableLotteryData.wResidueLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("ResidueLotteryTimes"));
			TurntableLotteryData.wDailyMayLotteryTimes= m_AccountsDBAide.GetValue_WORD(TEXT("DailyMayLotteryTimes"));
			TurntableLotteryData.wGameTimeLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("GameTimeLotteryTimes"));
			TurntableLotteryData.wGameCountLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("GameCountLotteryTimes"));
			TurntableLotteryData.wGameExpendLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("GameExpendLotteryTimes"));

			//进度数据
			TurntableLotteryData.dwGameTimeProgress[0] = m_AccountsDBAide.GetValue_DWORD(TEXT("CurrGameTime"));
			TurntableLotteryData.dwGameTimeProgress[1] = m_AccountsDBAide.GetValue_DWORD(TEXT("SliceGameTime"));
			TurntableLotteryData.dwGameCountProgress[0] = m_AccountsDBAide.GetValue_DWORD(TEXT("CurrGameCount"));
			TurntableLotteryData.dwGameCountProgress[1] = m_AccountsDBAide.GetValue_DWORD(TEXT("SliceGameCount"));
			TurntableLotteryData.dwGameExpendProgress[0] = m_AccountsDBAide.GetValue_DWORD(TEXT("CurrGameExpend"));
			TurntableLotteryData.dwGameExpendProgress[1] = m_AccountsDBAide.GetValue_DWORD(TEXT("SliceGameExpend"));

			//转盘记录
			m_AccountsDBAide.GetValue_String(TEXT("TurntableRecord"), TurntableLotteryData.szTurntableRecord,CountArray(TurntableLotteryData.szTurntableRecord));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_TURNTABLE_LOTTERY_DATA, dwContextID, dwTokenID, &TurntableLotteryData, sizeof(TurntableLotteryData));
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.wRequestCmdID = SUB_MB_QUERY_TURNTABLE_LOTTERY;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"由于系统繁忙，请稍后再试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//请求抽奖
bool CDataBaseEngineSink::OnRequestMBRequestTurntableLottery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_RequestTurntableLottery));
		if (wDataSize != sizeof(DBR_MB_RequestTurntableLottery)) return false;

		//请求处理
		auto pRequestTurntableLottery = (DBR_MB_RequestTurntableLottery*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pRequestTurntableLottery->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_TURNTABLE_LOTTERY);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RequestTurntableLottery"), true);

		//跨天处理
		if (lReturnValue == 100)
		{
			//构造结构
			DBR_MB_QueryTurntableLottery QueryTurntableLottery;
			ZeroMemory(&QueryTurntableLottery,sizeof(QueryTurntableLottery));

			//设置变量
			QueryTurntableLottery.dwUserID = pRequestTurntableLottery->dwUserID;
			QueryTurntableLottery.dwClientIP = pRequestTurntableLottery->dwClientIP;
			StringCchCopy(QueryTurntableLottery.szMachineID,CountArray(QueryTurntableLottery.szMachineID), pRequestTurntableLottery->szMachineID);

			//查询抽奖数据
			if (OnRequestMBQueryTurntableLottery(dwContextID, dwTokenID, &QueryTurntableLottery, sizeof(QueryTurntableLottery), pdwControlCode))
			{
				//构造参数
				m_AccountsDBAide.ResetParameter();
				m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pRequestTurntableLottery->dwUserID);
				m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_TURNTABLE_LOTTERY);

				//输出变量
				m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

				//执行查询
				lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RequestTurntableLottery"), true);
			}			
		}

		//抽奖结果
		DBO_MB_TurntableLotteryResult TurntableLotteryResult;
		ZeroMemory(&TurntableLotteryResult,sizeof(TurntableLotteryResult));		

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//读取数据
			TurntableLotteryResult.wHitIndex = m_AccountsDBAide.GetValue_WORD(TEXT("HitIndex"));
			TurntableLotteryResult.wUseLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("UseLotteryTimes"));
			TurntableLotteryResult.wResidueLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("ResidueLotteryTimes"));

			//发送结果
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_TURNTABLE_LOTTERY_RESULT, dwContextID, dwTokenID, &TurntableLotteryResult, sizeof(TurntableLotteryResult));
		}
		else
		{
			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_REQUEST_TURNTABLE_LOTTERY;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.wRequestCmdID = SUB_MB_REQUEST_TURNTABLE_LOTTERY;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"由于系统繁忙，请稍后再试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//加载背包
bool CDataBaseEngineSink::LoadUserPackGoods()
{
	try
	{
		//设置变量
		m_MBLogonSuccess.wGoodsCount = 0;

		//当前时间
		DWORD dwCurrentTime = (DWORD)time(NULL)+10;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), m_MBLogonSuccess.dwUserID);		

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserQueryGoods"), true);

		//执行成功
		if (lResultCode == DB_SUCCESS)
		{
			//变量定义
			WORD wExpireCount = 0;
			WORD wAccessorMask = 0;

			//变量定义
			CString strAccessorKind=TEXT("");

			//读取信息
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				//获取对象
				auto pGoodsHoldInfo = &m_MBLogonSuccess.GoodsHoldInfo[m_MBLogonSuccess.wGoodsCount++];

				//读取数据
				pGoodsHoldInfo->wGoodsID = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsID"));
				pGoodsHoldInfo->wGoodsKind = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsKind"));
				pGoodsHoldInfo->wGoodsLevel = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsLevel"));
				pGoodsHoldInfo->dwGoodsCount = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsCount"));
				pGoodsHoldInfo->dwGoodsIndate = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsIndate"));
				pGoodsHoldInfo->dwExpireTime = m_AccountsDBAide.GetValue_DWORD(TEXT("ExpireTime"));

				//过期处理
				if (pGoodsHoldInfo->dwGoodsIndate > 0 && dwCurrentTime >= pGoodsHoldInfo->dwExpireTime)
				{
					//过期数量
					wExpireCount++;

					//移除物品
					m_MBLogonSuccess.wGoodsCount--;								
				}

				//校验数量
				if (m_MBLogonSuccess.wGoodsCount >= CountArray(m_MBLogonSuccess.GoodsHoldInfo))
				{
					break;
				}				

				//移动记录
				m_AccountsDBModule->MoveToNext();
			}	

			//添加括号
			if (!strAccessorKind.IsEmpty())
			{
				strAccessorKind.Append(TEXT("]"));
			}
			else
			{
				strAccessorKind.Append(TEXT("[]"));
			}			
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

//排位信息
bool CDataBaseEngineSink::LoadUserRankingInfo()
{
	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), m_MBLogonSuccess.dwUserID);		

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadUserRankingInfo"), true);

		//获取信息
		if (lReturnValue == DB_SUCCESS)
		{
			auto pUserRankingInfo = &m_MBLogonSuccess.UserRankingInfo;
			pUserRankingInfo->dwRankID = m_AccountsDBAide.GetValue_DWORD(TEXT("RankID"));
			pUserRankingInfo->wSeasonID = m_AccountsDBAide.GetValue_WORD(TEXT("SeasonID"));
			pUserRankingInfo->lWinCount = m_AccountsDBAide.GetValue_LONG(TEXT("WinCount"));
			pUserRankingInfo->lLostCount = m_AccountsDBAide.GetValue_LONG(TEXT("LostCount"));
			pUserRankingInfo->lLoveHeart = m_AccountsDBAide.GetValue_LONG(TEXT("LoveHeart"));
			pUserRankingInfo->lRankScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("RankScore"));
			pUserRankingInfo->lWinStreakCount = m_AccountsDBAide.GetValue_LONG(TEXT("WinStreakCount"));			
		}
		
		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);		

		return false;
	}

	return true;
}

//游戏列表
bool CDataBaseEngineSink::LoadGameList(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//变量定义
		WORD wPacketSize = 0;
		BYTE cbBuffer[SOCKET_PACKET];

		//加载类型
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameTypeItem"), true);

		//发送种类
		wPacketSize = 0;
		DBO_GP_GameType* pGameType = NULL;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//发送信息
			if ((wPacketSize + sizeof(DBO_GP_GameType)) > sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_TYPE_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);
				wPacketSize = 0;
			}

			//读取信息
			pGameType = (DBO_GP_GameType*)(cbBuffer + wPacketSize);
			pGameType->wSortID = m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameType->wJoinID = m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameType->wTypeID = m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
			m_PlatformDBAide.GetValue_String(TEXT("TypeName"), pGameType->szTypeName, CountArray(pGameType->szTypeName));

			//设置位移
			wPacketSize += sizeof(DBO_GP_GameType);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize > 0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_TYPE_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);

		//读取类型
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameKindItem"), true);

		//发送类型
		wPacketSize = 0;
		DBO_GP_GameKind* pGameKind = NULL;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//发送信息
			if ((wPacketSize + sizeof(DBO_GP_GameKind)) > sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);
				wPacketSize = 0;
			}

			//读取信息
			pGameKind = (DBO_GP_GameKind*)(cbBuffer + wPacketSize);
			pGameKind->wModuleID = m_PlatformDBAide.GetValue_WORD(TEXT("ModuleID"));
			pGameKind->bNullity = m_PlatformDBAide.GetValue_BYTE(TEXT("Nullity"));
			pGameKind->wSortID = m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameKind->wTypeID = m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
			pGameKind->wJoinID = m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameKind->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameKind->wSupportMask = m_PlatformDBAide.GetValue_WORD(TEXT("SupportMask"));
			m_PlatformDBAide.GetValue_String(TEXT("KindName"), pGameKind->szKindName, CountArray(pGameKind->szKindName));
			m_PlatformDBAide.GetValue_String(TEXT("ProcessName"), pGameKind->szProcessName, CountArray(pGameKind->szProcessName));

			//设置位移
			wPacketSize += sizeof(DBO_GP_GameKind);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize > 0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);

		//读取节点
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameNodeItem"), true);

		//发送节点
		wPacketSize = 0;
		DBO_GP_GameNode* pGameNode = NULL;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//发送信息
			if ((wPacketSize + sizeof(DBO_GP_GameNode)) > sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_NODE_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);
				wPacketSize = 0;
			}

			//读取信息
			pGameNode = (DBO_GP_GameNode*)(cbBuffer + wPacketSize);
			pGameNode->wSortID = m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameNode->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameNode->wJoinID = m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameNode->wNodeID = m_PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
			m_PlatformDBAide.GetValue_String(TEXT("NodeName"), pGameNode->szNodeName, CountArray(pGameNode->szNodeName));

			//设置位移
			wPacketSize += sizeof(DBO_GP_GameNode);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize > 0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_NODE_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);

		//读取定制
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameCustomItem"), true);

		//发送定制
		wPacketSize = 0;
		DBO_GP_GameCustom* pGameCustom = NULL;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//发送信息
			if ((wPacketSize + sizeof(DBO_GP_GameCustom)) > sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PAGE_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);
				wPacketSize = 0;
			}

			//读取信息
			pGameCustom = (DBO_GP_GameCustom*)(cbBuffer + wPacketSize);
			pGameCustom->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameCustom->wNodeID = m_PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
			pGameCustom->wSortID = m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameCustom->wPageID = m_PlatformDBAide.GetValue_WORD(TEXT("PageID"));
			pGameCustom->wOperateType = m_PlatformDBAide.GetValue_WORD(TEXT("OperateType"));
			m_PlatformDBAide.GetValue_String(TEXT("DisplayName"), pGameCustom->szDisplayName, CountArray(pGameCustom->szDisplayName));
			m_PlatformDBAide.GetValue_String(TEXT("ResponseUrl"), pGameCustom->szReponseUrl, CountArray(pGameCustom->szReponseUrl));

			//设置位移
			wPacketSize += sizeof(DBO_GP_GameCustom);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize > 0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PAGE_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);

		//读取过滤
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameFilterItem"), true);

		//发送过滤
		wPacketSize = 0;
		DBO_GP_GameFilter* pGameFilter = NULL;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//发送信息
			if ((wPacketSize + sizeof(DBO_GP_GameFilter)) > sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_FILTER_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);
				wPacketSize = 0;
			}

			//读取信息
			pGameFilter = (DBO_GP_GameFilter*)(cbBuffer + wPacketSize);
			pGameFilter->wSortID = m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameFilter->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameFilter->wNodeID = m_PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
			pGameFilter->wServerID = m_PlatformDBAide.GetValue_WORD(TEXT("ServerID"));
			pGameFilter->wFilterMask = m_PlatformDBAide.GetValue_WORD(TEXT("SiftMask"));
			m_PlatformDBAide.GetValue_String(TEXT("ServerName"), pGameFilter->szServerName, CountArray(pGameFilter->szServerName));

			//设置位移
			wPacketSize += sizeof(DBO_GP_GameFilter);

			//移动记录
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize > 0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_FILTER_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//敏感词汇
bool CDataBaseEngineSink::LoadFilterWords(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadConfineContent"), true);

		//读取信息
		if (lReturnValue == DB_SUCCESS)
		{
			//起始消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_FILTER_WORDS, 0xfffe, dwTokenID, NULL, 0);

			//读取消息
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				TCHAR szFribidWords[32] = TEXT("");

				//读取消息
				m_AccountsDBAide.GetValue_String(TEXT("ForbidWords"), szFribidWords, 32);

				//发送消息
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_FILTER_WORDS, 0, dwTokenID, szFribidWords, 32);

				//移动记录
				m_AccountsDBModule->MoveToNext();
			};

			//结束消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_FILTER_WORDS, 0xffff, 0, NULL, 0);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//全局配置
bool CDataBaseEngineSink::LoadGlobalOption(DWORD dwContextID, DWORD dwTokenID, DWORD dwOptionMask)
{
	//变量定义
	TCHAR szOptionData[2048] = {0};

	try
	{
		//签到配置
		if (dwOptionMask & OPTION_MASK_CHECKIN)
		{
			//构造参数
			m_PlatformDBAide.ResetParameter();
			LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadCheckInParameter"), true);

			//加载成功
			if (lReturnValue == DB_SUCCESS)
			{
				//读取配置
				m_PlatformDBAide.GetValue_String(TEXT("CheckInOption"), szOptionData, CountArray(szOptionData));

				//添加配置
				if (!g_GlobalOptionManager->AppendOptionItem(OPTION_MASK_CHECKIN, szOptionData))
				{
					return false;
				}
			}
		}

		//转盘配置
		if (dwOptionMask & OPTION_MASK_TURNTABLE)
		{		
			//构造参数
			m_PlatformDBAide.ResetParameter();
			LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadTurntableParameter"), true);

			//加载成功
			if (lReturnValue == DB_SUCCESS)
			{
				//读取配置
				m_PlatformDBAide.GetValue_String(TEXT("TurntableOption"), szOptionData, CountArray(szOptionData));

				//添加配置
				if (!g_GlobalOptionManager->AppendOptionItem(OPTION_MASK_TURNTABLE, szOptionData))
				{
					return false;
				}
			}			
		}	

		//跑马灯配置
		if (dwOptionMask & OPTION_MASK_MARQUEE_NOTICE)
		{
			//构造参数
			m_PlatformDBAide.ResetParameter();
			LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadMarqueeNoticeOption"), true);

			//加载成功
			if (lReturnValue == DB_SUCCESS)
			{
				//读取配置
				m_PlatformDBAide.GetValue_String(TEXT("MarqueeNoticeOption"), szOptionData, CountArray(szOptionData));

				//添加配置
				if (!g_GlobalOptionManager->AppendOptionItem(OPTION_MASK_MARQUEE_NOTICE, szOptionData))
				{
					return false;
				}
			}
		}
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}
//昵称配置
bool CDataBaseEngineSink::LoadNickNameOption(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//变量定义
		tagNickNameOption NickNameOption;
		ZeroMemory(&NickNameOption, sizeof(NickNameOption));

		//构造参数
		m_PlatformDBAide.ResetParameter();

		//执行查询
		LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadNickNameOption"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//配置参数
			NickNameOption.wFreeModifyTimes = m_PlatformDBAide.GetValue_WORD(TEXT("FreeModifyTimes"));
			NickNameOption.wDailyModifyTimes = m_PlatformDBAide.GetValue_WORD(TEXT("DailyModifyTimes"));
			NickNameOption.dwGoodsID = m_PlatformDBAide.GetValue_DWORD(TEXT("GoodsID"));
			NickNameOption.dwGoodsCount = m_PlatformDBAide.GetValue_DWORD(TEXT("GoodsCount"));
		}

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_NICKNAME_OPTION, 0, 0, &NickNameOption, sizeof(tagNickNameOption));

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
};
//版本信息
bool CDataBaseEngineSink::LoadPlazaVersion(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wModuleID"), 0);
		LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameVersionInfo"), true);

		//加载成功
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			WORD wPacketSize = 0;
			BYTE cbBuffer[SOCKET_PACKET];

			//变量定义
			tagModuleVersion* pModuleVersion = NULL;

			//开桌配置
			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				//发送信息
				if ((wPacketSize + sizeof(tagModuleVersion)) > sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_VERSION_INFO, dwContextID, dwTokenID, cbBuffer, wPacketSize);
					wPacketSize = 0;
				}

				//读取信息
				pModuleVersion = (tagModuleVersion*)(cbBuffer + wPacketSize);

				//读取数据				
				pModuleVersion->IsCurrent = m_PlatformDBAide.GetValue_BYTE(TEXT("IsCurrent"));
				pModuleVersion->wModuleID = m_PlatformDBAide.GetValue_WORD(TEXT("ModuleID"));
				pModuleVersion->dwDeviceKind = m_PlatformDBAide.GetValue_DWORD(TEXT("DeviceKind"));
				pModuleVersion->dwVersion = m_PlatformDBAide.GetValue_DWORD(TEXT("Version"));

				//设置位移
				wPacketSize += sizeof(tagModuleVersion);

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}

			//发送剩余
			if (wPacketSize > 0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_VERSION_INFO, dwContextID, dwTokenID, cbBuffer, wPacketSize);
			}
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//转账配置
bool CDataBaseEngineSink::LoadTransferParemter(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//变量定义
		tagTransferParameter TransferParemter;
		ZeroMemory(&TransferParemter, sizeof(TransferParemter));

		//构造参数
		m_PlatformDBAide.ResetParameter();

		//执行查询
		LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadTransferParameter"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//控制参数
			TransferParemter.bTransferEnabled = m_PlatformDBAide.GetValue_BYTE(TEXT("TransferEnabled"));
			TransferParemter.wServiceFeeRate = m_PlatformDBAide.GetValue_WORD(TEXT("ServiceFeeRate"));
			TransferParemter.lReservedAmount = m_PlatformDBAide.GetValue_LONGLONG(TEXT("ReservedAmount"));
			TransferParemter.lMinTransferAmount = m_PlatformDBAide.GetValue_LONGLONG(TEXT("MinTransferAmount"));
			TransferParemter.lMaxTransferAmount = m_PlatformDBAide.GetValue_LONGLONG(TEXT("MaxTransferAmount"));
		}

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TRANSFER_PARAMETER, 0, 0, &TransferParemter, sizeof(TransferParemter));

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//清理信息
bool CDataBaseEngineSink::CleanOnlineInfo(WORD wLogonID)
{
	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), wLogonID);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CleanOnlineLogonInfo"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return false;
}

//缓存信息
bool CDataBaseEngineSink::CacheUserInfo(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//变量定义
		int nBuffLen = 0;
		CHAR szBuffer[512];

		//构造结构
		UserPublicInfo PublicInfo;				

		//设置信息
		PublicInfo.set_uid(m_MBLogonSuccess.dwUserID);
		PublicInfo.set_gameid(m_MBLogonSuccess.dwGameID);
		PublicInfo.set_gender(m_MBLogonSuccess.cbGender);
		PublicInfo.set_faceid(m_MBLogonSuccess.wFaceID);
		PublicInfo.set_customid(m_MBLogonSuccess.dwCustomID);
		PublicInfo.set_infopublic(m_MBLogonSuccess.cbInfoPublic);
		PublicInfo.set_experience(m_MBLogonSuccess.dwExperience);
		PublicInfo.set_memberpoint(m_MBLogonSuccess.dwMemberPoint);				
		PublicInfo.set_masterorder(m_MBLogonSuccess.wMasterOrder);
		PublicInfo.set_rankscore(m_MBLogonSuccess.UserRankingInfo.lRankScore);
		PublicInfo.set_loveliness(m_MBLogonSuccess.dwLoveLiness);
		
		//设置昵称
		nBuffLen = CWHService::Unicode2Utf8(m_MBLogonSuccess.szNickName, szBuffer,CountArray(szBuffer));		
		PublicInfo.set_nickname(szBuffer, nBuffLen);

		//设置昵称
		nBuffLen = CWHService::Unicode2Utf8(m_MBLogonSuccess.szUnderWrite, szBuffer, CountArray(szBuffer));
		PublicInfo.set_underwrite(szBuffer, nBuffLen);

		//版本信息
		lhchat::InfoVersion Version;

		//初始化版本		
		for (int i=0; i< UserInfoIndex_ARRAYSIZE; i++) Version.add_infoversion(1);		

		//序列化
		string sVersion;
		Version.SerializeToString(&sVersion);

		//设置版本
		PublicInfo.set_infoversion(sVersion);

		//序列化
		string sPublicInfo;
		PublicInfo.SerializeToString(&sPublicInfo);

		//构造变量
		string sUserKey = RKEY_PREFIX_USER + to_string(PublicInfo.uid());		

		//设置key
		m_pUserClient->set(sUserKey, sPublicInfo);		

		return true;
	}
	catch (exception& ex)
	{
		TCHAR szDescribe[512] = { 0 };
		_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("%hs"), ex.what());
		CTraceService::TraceString(szDescribe, TraceLevel_Exception);
	}

	return false;
}
//更新信息
bool CDataBaseEngineSink::UpdateUserCache(DWORD dwContextID, DWORD dwTokenID, UserPublicInfo& PublicInfo)
{
	try
	{
		//构造变量
		string sUserKey = RKEY_PREFIX_USER + to_string(PublicInfo.uid());

		//读取信息
		auto sCacheUserInfo = m_pUserClient->get(sUserKey);
		if (!sCacheUserInfo) return false;

		//解析数据
		UserPublicInfo CachePublicInfo;
		if (!CachePublicInfo.ParseFromString(sCacheUserInfo.value()))
		{
			return false;
		}

		//基础信息
		//if (PublicInfo.has_faceid()) CachePublicInfo.set_faceid(PublicInfo.faceid());
		//if (PublicInfo.has_customid()) CachePublicInfo.set_customid(PublicInfo.customid());
		if (PublicInfo.has_infopublic()) CachePublicInfo.set_infopublic(PublicInfo.infopublic());
		//if (PublicInfo.has_likednumber()) CachePublicInfo.set_infopublic(PublicInfo.likednumber());
		//if (PublicInfo.has_faceframeid()) CachePublicInfo.set_faceframeid(PublicInfo.faceframeid());

		//版本信息
		InfoVersion Version;
		Version.ParseFromString(CachePublicInfo.infoversion());

		//更新信息
		for (auto i = 0; i < Version.infoversion_size(); i++)
		{
			//更新标志
			bool bUpdateVersion = false;

			do
			{
				//用户性别
				if (i == UII_GENDER && PublicInfo.has_gender())
				{
					if (PublicInfo.gender() != CachePublicInfo.gender())
					{
						bUpdateVersion = true;
						CachePublicInfo.set_gender(PublicInfo.gender());
						break;
					}
				}

				//用户昵称
				if (i == UII_NICKNAME && PublicInfo.has_nickname())
				{
					bUpdateVersion = true;
					CachePublicInfo.set_nickname(PublicInfo.nickname());
					break;
				}

				//个性签名
				if (i == UII_UNDERWRITE && PublicInfo.has_underwrite())
				{
					bUpdateVersion = true;
					CachePublicInfo.set_underwrite(PublicInfo.underwrite());
					break;
				}

				//排位分数
				if (i == UII_RANKSCORE && PublicInfo.has_rankscore())
				{
					bUpdateVersion = true;
					CachePublicInfo.set_rankscore(PublicInfo.rankscore());
					break;
				}

				//游戏经验
				if (i == UII_EXPERIENCE && PublicInfo.has_experience())
				{
					bUpdateVersion = true;
					CachePublicInfo.set_experience(PublicInfo.experience());
					break;
				}

				//会员点数
				if (i == UII_MEMBERPOINT && PublicInfo.has_memberpoint())
				{
					bUpdateVersion = true;
					CachePublicInfo.set_memberpoint(PublicInfo.memberpoint());
					break;
				}

				//管理等级
				if (i == UII_MASTERORDER && PublicInfo.has_masterorder())
				{
					bUpdateVersion = true;
					CachePublicInfo.set_masterorder(PublicInfo.masterorder());
					break;
				}

				//头像标识
				if (i == UII_FACEID && (PublicInfo.has_faceid() || PublicInfo.has_customid()))
				{
					bUpdateVersion = true;
					if (PublicInfo.has_faceid()) CachePublicInfo.set_faceid(PublicInfo.faceid());
					if (PublicInfo.has_customid()) CachePublicInfo.set_customid(PublicInfo.customid());
					break;
				}

				//像框标识
				//if (i == UII_FACEFRAMEID && PublicInfo.has_faceframeid())
				//{
				//	bUpdateVersion = true;
				//	CachePublicInfo.set_faceframeid(PublicInfo.faceframeid());
				//	break;
				//}

				//魅力值
				if (i == UII_LOVELINESS && PublicInfo.has_loveliness())
				{
					bUpdateVersion = true;
					CachePublicInfo.set_loveliness(PublicInfo.loveliness());
					break;
				}

				//聊天标识
				//if (i == UII_CHATFRAMEID && PublicInfo.has_chatframeid())
				//{
				//	bUpdateVersion = true;
				//	CachePublicInfo.set_chatframeid(PublicInfo.chatframeid());
				//	break;
				//}
			} while (false);

			//更新版本
			if (bUpdateVersion) Version.set_infoversion(i, Version.infoversion(i) + 1);
		}

		//序列化
		string sVersion;
		Version.SerializeToString(&sVersion);

		//设置版本
		CachePublicInfo.set_infoversion(sVersion);

		//序列化
		string sCachePublicInfo;
		CachePublicInfo.SerializeToString(&sCachePublicInfo);

		//设置key
		m_pUserClient->set(sUserKey, sCachePublicInfo);

		//更新缓存
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_CACHE_INFO, dwContextID, dwTokenID, (char*)sCachePublicInfo.data(), sCachePublicInfo.size());

		return true;
	}
	catch (exception& ex)
	{
		CTraceService::TraceString(CA2CT(ex.what()).m_psz, TraceLevel_Exception);
	}

	return false;
}

//生成TOKEN
bool CDataBaseEngineSink::BuildLogonToken(DWORD dwUserID, LPCTSTR pszAccounts, LPCTSTR pszSecretKey, TCHAR szUserToken[LEN_LOGON_TOKEN])
{
	//参数校验
	ASSERT(pszAccounts != NULL);
	if (pszAccounts == NULL) return false;

	try
	{
		//变量定义
		DWORD dwCurrentTime = time(NULL);

		//构造信息
		CWHClaims Claims;
		Claims.Set("UserID", dwUserID);
		Claims.Set("Accounts", string(CT2A(pszAccounts).m_psz));
		Claims.Set("SecretKey", string(CT2A(pszSecretKey).m_psz));
		Claims.Set("Time", dwCurrentTime);

		//构造JWT
		string sToken = CWHJwt::Sign(Claims);

		//拷贝数据
		StringCchCopy(szUserToken, LEN_LOGON_TOKEN, CA2CT(sToken.c_str()));

		//构造key
		CHAR szTokenKey[64];
		_snprintf_s(szTokenKey, CountArray(szTokenKey), RKEY_PREFIX_TOKEN "%d", dwUserID);

		//构造令牌
		CHAR szTokenValue[512];
		_snprintf_s(szTokenValue, CountArray(szTokenValue), "{\"Token\":\"%s\",\"Expire\":%d}", sToken.c_str(), dwCurrentTime + ONE_WEEK);

		//构造变量
		StringView sTokenKey(szTokenKey);
		StringView sUserToken(szTokenValue);

		//设置key
		m_pTokenClient->set(sTokenKey, szTokenValue);

		return true;
	}
	catch (exception& ex)
	{
		TCHAR szDescribe[512] = { 0 };
		_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("%hs"), ex.what());
		CTraceService::TraceString(szDescribe, TraceLevel_Exception);
	}

	return false;
}

//更新财富
bool CDataBaseEngineSink::UpdateUserWealth(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryGameWealth"), true);

		//加载成功
		if (lResultCode == DB_SUCCESS)
		{
			//更新财富
			DBO_MB_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

			//设置变量
			WealthUpdate.wItemCount = 2;
			WealthUpdate.WealthItem[0].wMoneyID = GOODS_ID_CARD;
			WealthUpdate.WealthItem[0].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserCard"));
			WealthUpdate.WealthItem[1].wMoneyID = GOODS_ID_GOLD;
			WealthUpdate.WealthItem[1].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));
			
			//发送结果			
			WORD wHeadSize = sizeof(WealthUpdate) - sizeof(WealthUpdate.WealthItem);
			WORD wPacketSize = WealthUpdate.wItemCount * sizeof(WealthUpdate.WealthItem[0]) + wHeadSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_WEALTH_UPDATE, dwContextID, dwTokenID, &WealthUpdate, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

//更新物品
bool CDataBaseEngineSink::UpdateUserGoods(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID, tagGoodsItem GoodsItemList[], WORD wGoodsCount)
{
	try
	{
		//变量定义
		CString strGoodsID;
		CString strGoodsIDList;

		//构造物品列表
		for (auto i = 0; i < wGoodsCount; i++)
		{
			//添加括号
			if (i == 0) strGoodsIDList.Append(TEXT("["));

			//格式化
			strGoodsID.Format(TEXT("%d"), GoodsItemList[i].wGoodsID);

			//追加数据
			strGoodsIDList.Append(strGoodsID);
			strGoodsIDList.Append((i < wGoodsCount - 1) ? TEXT(",") : TEXT("]"));
		}

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@szGoodsIDList"), strGoodsIDList);
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserQueryGoods"), true);

		//加载成功
		if (lResultCode == DB_SUCCESS)
		{
			//变量定义
			DBO_MB_GoodsUpdate UserGoodsUpdate;
			ZeroMemory(&UserGoodsUpdate, sizeof(UserGoodsUpdate));

			//读取信息
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				//获取对象
				auto pGoodsHoldInfo = &UserGoodsUpdate.GoodsHoldInfo[UserGoodsUpdate.wGoodsCount++];

				//物品信息
				pGoodsHoldInfo->wGoodsID = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsID"));
				pGoodsHoldInfo->wGoodsKind = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsKind"));
				pGoodsHoldInfo->wGoodsLevel = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsLevel"));
				pGoodsHoldInfo->dwGoodsCount = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsCount"));
				pGoodsHoldInfo->dwGoodsIndate = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsIndate"));
				pGoodsHoldInfo->dwExpireTime = m_AccountsDBAide.GetValue_DWORD(TEXT("ExpireTime"));

				//移动记录
				m_AccountsDBModule->MoveToNext();
			}

			//发送数据
			WORD wHeadSize = sizeof(UserGoodsUpdate) - sizeof(UserGoodsUpdate.GoodsHoldInfo);
			WORD wPacketSize = UserGoodsUpdate.wGoodsCount * sizeof(UserGoodsUpdate.GoodsHoldInfo[0]) + wHeadSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_GOODS_UPDATE, dwContextID, dwTokenID, &UserGoodsUpdate, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

//更新魅力
bool CDataBaseEngineSink::UpdateUserLoveLiness(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID)
{
    try
    {
        //构造参数
        m_AccountsDBAide.ResetParameter();
        m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
        LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserLoveLiness"), true);

        //加载成功
        if (lResultCode == DB_SUCCESS)
        {
            //更新财富
			DBO_MB_LoveLinessUpdate LoveLinessUpdate;
            ZeroMemory(&LoveLinessUpdate, sizeof(LoveLinessUpdate));

            //设置变量
			LoveLinessUpdate.dwLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));

            //发送结果			
            m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_LOVELINESS_UPDATE, dwContextID, dwTokenID, &LoveLinessUpdate, sizeof(LoveLinessUpdate));
        }

        return true;
    }
    catch (IDataBaseException* pIException)
    {
        TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
    }

    return false;
}

//登录成功
VOID CDataBaseEngineSink::OnLogonDisposeResult(DWORD dwContextID, DWORD dwTokenID, LONG lResultCode, LPCTSTR pszErrorString, bool bMobileClient, bool isThirdLogon )
{
	if (bMobileClient==false)
	{
		if (lResultCode==DB_SUCCESS)
		{
			//属性资料
			m_GPLogonSuccess.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			m_GPLogonSuccess.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_GPLogonSuccess.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			m_GPLogonSuccess.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
			m_AccountsDBAide.GetValue_String(TEXT("Accounts"),m_GPLogonSuccess.szAccounts,CountArray(m_GPLogonSuccess.szAccounts));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),m_GPLogonSuccess.szNickName,CountArray(m_GPLogonSuccess.szNickName));
			m_AccountsDBAide.GetValue_String(TEXT("Password"),m_GPLogonSuccess.szPassword,CountArray(m_GPLogonSuccess.szPassword));
			m_GPLogonSuccess.cbMasterOrder = m_AccountsDBAide.GetValue_BYTE(TEXT("MasterOrder"));
			m_GPLogonSuccess.dwProtectID = m_AccountsDBAide.GetValue_DWORD(TEXT("ProtectID"));
			m_GPLogonSuccess.cbMoorMachine = m_AccountsDBAide.GetValue_BYTE(TEXT("MoorMachine"));

			//用户资料
			m_GPLogonSuccess.cbAge=m_AccountsDBAide.GetValue_BYTE(TEXT("Age"));
			m_GPLogonSuccess.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
			m_AccountsDBAide.GetValue_String(TEXT("Area"),m_GPLogonSuccess.szArea,CountArray(m_GPLogonSuccess.szArea));
			m_AccountsDBAide.GetValue_String(TEXT("City"),m_GPLogonSuccess.szCity,CountArray(m_GPLogonSuccess.szCity));
			m_AccountsDBAide.GetValue_String(TEXT("Province"),m_GPLogonSuccess.szProvince,CountArray(m_GPLogonSuccess.szProvince));
			m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),m_GPLogonSuccess.szUnderWrite,CountArray(m_GPLogonSuccess.szUnderWrite));

			//经验等级
			m_GPLogonSuccess.dwExperience = m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));

			//会员资料
			m_GPLogonSuccess.dwMemberOverTime= m_AccountsDBAide.GetValue_DWORD(TEXT("MemberOverTime"));			

			//财富信息
			m_GPLogonSuccess.lUserGold = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Gold"));
			m_GPLogonSuccess.lInsureGold = m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureGold"));

			//锁定信息
			m_GPLogonSuccess.wLockGateID = m_AccountsDBAide.GetValue_WORD(TEXT("LockGateID"));
			m_GPLogonSuccess.wLockLogonID = m_AccountsDBAide.GetValue_WORD(TEXT("LockLogonID"));
			m_GPLogonSuccess.dwLockSocketID = m_AccountsDBAide.GetValue_DWORD(TEXT("LockSocketID"));

			//附加信息
			m_GPLogonSuccess.dwPostTime=(DWORD)time(NULL);

			//发送结果
			WORD wStringSize=CountStringBuffer(m_GPLogonSuccess.szDescribeString);
			WORD wSendSize=sizeof(m_GPLogonSuccess)-sizeof(m_GPLogonSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOGON_SUCCESS,dwContextID,dwTokenID,&m_GPLogonSuccess,wSendSize);
		}
		else
		{
			//获取信息
			StringCchCopy(m_GPLogonFailure.szDescribeString, CountArray(m_GPLogonFailure.szDescribeString), pszErrorString);

			//发送结果
			WORD wStringSize=CountStringBuffer(pszErrorString);
			WORD wSendSize=sizeof(m_GPLogonFailure)-sizeof(m_GPLogonFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOGON_FAILURE,dwContextID,dwTokenID,&m_GPLogonFailure,wSendSize);
		}
	}
	else
	{
		if (lResultCode==DB_SUCCESS)
		{
			//属性资料
			m_MBLogonSuccess.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			m_MBLogonSuccess.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
			m_MBLogonSuccess.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
			m_MBLogonSuccess.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_MBLogonSuccess.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			m_MBLogonSuccess.cbInfoPublic=m_AccountsDBAide.GetValue_BYTE(TEXT("InfoPublic"));
			m_MBLogonSuccess.dwUserRight=m_AccountsDBAide.GetValue_DWORD(TEXT("UserRight"));
			m_AccountsDBAide.GetValue_String(TEXT("UniqueID"), m_MBLogonSuccess.szUniqueID, CountArray(m_MBLogonSuccess.szUniqueID));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"), m_MBLogonSuccess.szNickName,CountArray(m_MBLogonSuccess.szNickName));
			m_AccountsDBAide.GetValue_String(TEXT("Accounts"), m_MBLogonSuccess.szAccounts,CountArray(m_MBLogonSuccess.szAccounts));
			m_AccountsDBAide.GetValue_String(TEXT("Password"), m_MBLogonSuccess.szLogonPass,CountArray(m_MBLogonSuccess.szLogonPass));
			m_AccountsDBAide.GetValue_String(TEXT("InsurePass"), m_MBLogonSuccess.szInsurePass,CountArray(m_MBLogonSuccess.szInsurePass));

			//用户秘钥
			TCHAR szSecretKey[33] = {0};
			m_AccountsDBAide.GetValue_String(TEXT("SecretKey"), szSecretKey, CountArray(szSecretKey));

			//用户资料
			m_AccountsDBAide.GetValue_String(TEXT("Email"), m_MBLogonSuccess.szEmail, CountArray(m_MBLogonSuccess.szEmail));
			m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"), m_MBLogonSuccess.szUnderWrite, CountArray(m_MBLogonSuccess.szUnderWrite));
			m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"), m_MBLogonSuccess.szMobilePhone, CountArray(m_MBLogonSuccess.szMobilePhone));

			//经验等级
			m_MBLogonSuccess.dwExperience = m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));

            //魅力值
            m_MBLogonSuccess.dwLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));

			//会员资料
			m_MBLogonSuccess.wMasterOrder = m_AccountsDBAide.GetValue_WORD(TEXT("MasterOrder"));
			m_MBLogonSuccess.dwMemberPoint = m_AccountsDBAide.GetValue_DWORD(TEXT("MemberPoint"));
			m_MBLogonSuccess.dwMemberOverTime = m_AccountsDBAide.GetValue_DWORD(TEXT("MemberOverTime"));			

			//用户财富
			m_MBLogonSuccess.lUserGold=m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));
			m_MBLogonSuccess.lUserCard=m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserCard"));
			m_MBLogonSuccess.lUserInsure=m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureGold"));		
			m_MBLogonSuccess.lCommission=m_AccountsDBAide.GetValue_LONGLONG(TEXT("Commission"));

			//行为掩码
			m_MBLogonSuccess.dwActionMaskEver = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskEver"));
			m_MBLogonSuccess.dwActionMaskPerDay = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskPerDay"));
			m_MBLogonSuccess.dwActionMaskPerWeek = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskPerWeek"));

			//锁定信息
			m_MBLogonSuccess.wLockGateID = m_AccountsDBAide.GetValue_WORD(TEXT("LockGateID"));
			m_MBLogonSuccess.wLockLogonID = m_AccountsDBAide.GetValue_WORD(TEXT("LockLogonID"));
			m_MBLogonSuccess.dwLockSocketID = m_AccountsDBAide.GetValue_DWORD(TEXT("LockSocketID"));
			m_MBLogonSuccess.wLockKindID = m_AccountsDBAide.GetValue_WORD(TEXT("LockKindID"));
			m_MBLogonSuccess.wLockServerID = m_AccountsDBAide.GetValue_WORD(TEXT("LockServerID"));
			m_MBLogonSuccess.wLockServerKind = m_AccountsDBAide.GetValue_WORD(TEXT("LockServerKind"));				

			//代理信息
			m_MBLogonSuccess.dwParentGameID = m_AccountsDBAide.GetValue_DWORD(TEXT("ParentGameID"));
			m_MBLogonSuccess.wParentFaceID = m_AccountsDBAide.GetValue_WORD(TEXT("ParentFaceID"));
			m_AccountsDBAide.GetValue_String(TEXT("ParentNickName"), m_MBLogonSuccess.szParentNickName, CountArray(m_MBLogonSuccess.szParentNickName));

			//状态信息
			m_MBLogonSuccess.cbShutUpStatus = m_AccountsDBAide.GetValue_BYTE(TEXT("ShutUpStatus"));
			m_MBLogonSuccess.dwShutUpOverTime = m_AccountsDBAide.GetValue_DWORD(TEXT("ShutUpOverTime"));

			//签到信息
			m_MBLogonSuccess.wCheckInDays = m_AccountsDBAide.GetValue_WORD(TEXT("CheckInDays"));
			m_MBLogonSuccess.cbCheckInState = m_AccountsDBAide.GetValue_BYTE(TEXT("CheckInState"));

			//昵称修改信息
			m_MBLogonSuccess.wNickNameModifyTimes = m_AccountsDBAide.GetValue_WORD(TEXT("NickNameModifyTimes"));
			//头像地址 kk jia
			m_MBLogonSuccess.szUserFaceUrl[0] = 0;
			if (isThirdLogon)
				m_AccountsDBAide.GetValue_String(TEXT("UserFaceUrl"), m_MBLogonSuccess.szUserFaceUrl, CountArray(m_MBLogonSuccess.szUserFaceUrl));

			CString s;
			s.Format(_T("1 szNickName:%s,szUserFaceUrl:%s,isThirdLogon:%d"), m_MBLogonSuccess.szNickName,m_MBLogonSuccess.szUserFaceUrl, isThirdLogon);
			CTraceService::TraceString(s, TraceLevel_Exception);

			m_MBLogonSuccess.wNickNameDailyModifyTimes = m_AccountsDBAide.GetValue_WORD(TEXT("NickNameDailyModifyTimes"));

			//附加信息
			m_MBLogonSuccess.dwLikedNumber = m_AccountsDBAide.GetValue_DWORD(TEXT("LikedNumber"));
			m_MBLogonSuccess.dwRegisterTime = m_AccountsDBAide.GetValue_DWORD(TEXT("RegisterTime"));			
			m_MBLogonSuccess.cbRegisterMode  = m_AccountsDBAide.GetValue_BYTE(TEXT("RegisterMode"));
			m_MBLogonSuccess.dwUIShieldMask = m_AccountsDBAide.GetValue_DWORD(TEXT("UIShieldMask"));		
			m_AccountsDBAide.GetValue_String(TEXT("ShieldKindList"), m_MBLogonSuccess.szShieldKindList, CountArray(m_MBLogonSuccess.szShieldKindList));

			//实名信息
			m_AccountsDBAide.GetValue_String(TEXT("Compellation"), m_MBLogonSuccess.RealAuth.szCompellation, CountArray(m_MBLogonSuccess.RealAuth.szCompellation));
			m_AccountsDBAide.GetValue_String(TEXT("PassPortID"), m_MBLogonSuccess.RealAuth.szPassPortID, CountArray(m_MBLogonSuccess.RealAuth.szPassPortID));

			//登录次数
			DWORD dwGameLogonTimes = m_AccountsDBAide.GetValue_DWORD(TEXT("GameLogonTimes"));

			//清空赠送
			m_MBLogonSuccess.szPresentGoodsList[0] = 0;

			//注册赠送
			if (dwGameLogonTimes == 1)
			{
				m_AccountsDBAide.GetValue_String(TEXT("PresentGoodsList"), m_MBLogonSuccess.szPresentGoodsList, CountArray(m_MBLogonSuccess.szPresentGoodsList));
			}

			//获取信息
			if (pszErrorString != NULL)
			{
				StringCchCopy(m_MBLogonSuccess.szDescribeString, CountArray(m_MBLogonSuccess.szDescribeString), pszErrorString);
			}		

			//缓存信息
			if (dwGameLogonTimes == 1) CacheUserInfo(dwContextID, dwTokenID);

			//背包信息
			LoadUserPackGoods();

			//构造令牌
			BuildLogonToken(m_MBLogonSuccess.dwUserID, m_MBLogonSuccess.szAccounts, szSecretKey, m_MBLogonSuccess.szLogonToken);	
			
			
			//发送结果
			WORD wDataSize=CountStringBuffer(m_MBLogonSuccess.szDescribeString);
			WORD wHeadSize=sizeof(m_MBLogonSuccess)-sizeof(m_MBLogonSuccess.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_LOGON_SUCCESS,dwContextID,dwTokenID,&m_MBLogonSuccess,wHeadSize+wDataSize);
		}
		else
		{
			//构造数据
			m_MBLogonFailure.lResultCode=lResultCode;

			//错误信息
			if (pszErrorString != NULL)
			{
				StringCchCopy(m_MBLogonFailure.szDescribeString, CountArray(m_MBLogonFailure.szDescribeString), pszErrorString);
			}			

			//发送结果
			WORD wDataSize=CountStringBuffer(m_MBLogonFailure.szDescribeString);
			WORD wHeadSize=sizeof(m_MBLogonFailure)-sizeof(m_MBLogonFailure.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_LOGON_FAILURE,dwContextID,dwTokenID,&m_MBLogonFailure,wHeadSize+wDataSize);
		}
	}

	return;
}
//实名认证
bool CDataBaseEngineSink::OnRequestMBRequestRealAuthent(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize >= sizeof(DBR_MB_RealAuth));
		if (wDataSize < sizeof(DBR_MB_RealAuth)) return false;

		//请求处理
		auto pRealAuth = (DBR_MB_RealAuth*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pRealAuth->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@szPassword"), pRealAuth->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@szCompellation"), pRealAuth->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@szPassPortID"), pRealAuth->szPassPortID);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//执行查询
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_RealAuth"), true) == DB_SUCCESS)
		{

			//变量定义
			DBO_MB_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateSuccess.wRequestCmdID = SUB_MB_REAL_AUTH;
			OperateSuccess.lResultCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wSendSize);
		}
		else
		{
			//变量定义
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.wRequestCmdID = SUB_MB_REAL_AUTH;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}
		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = SUB_MB_REAL_AUTH;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}
};
//银行查询
bool CDataBaseEngineSink::OnRequestMBRequestInsureQuery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		ASSERT(wDataSize == sizeof(DBR_MB_InsureQuery));
		DBR_MB_InsureQuery* pInfo = (DBR_MB_InsureQuery*)pData;
		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pInfo->dwUserID);

		//
		DBR_MB_InsureQueryResult InsureResult;
		ZeroMemory(&InsureResult, sizeof(InsureResult));

		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_InsureQuery"), true) == DB_SUCCESS)
		{
			InsureResult.cbEnable = m_AccountsDBAide.GetValue_BYTE(TEXT("Enable"));
			InsureResult.lScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			InsureResult.lInsureScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureScore"));

			InsureResult.SaveAndTake.lReserveAmount = m_AccountsDBAide.GetValue_LONGLONG(TEXT("SaveAndTakeReserveAmount"));
			InsureResult.SaveAndTake.lDealMin = m_AccountsDBAide.GetValue_LONGLONG(TEXT("SaveAndTakeDealMin"));
			InsureResult.SaveAndTake.lDealMax = m_AccountsDBAide.GetValue_LONGLONG(TEXT("SaveAndTakeDealMax"));
			InsureResult.SaveAndTake.lDailyMax = m_AccountsDBAide.GetValue_LONGLONG(TEXT("SaveAndTakeDailyMax"));
			InsureResult.SaveAndTake.dwFeeRate = m_AccountsDBAide.GetValue_DWORD(TEXT("SaveAndTakeFeeRate"));
			
			InsureResult.Transfer.lReserveAmount = m_AccountsDBAide.GetValue_LONGLONG(TEXT("TransferReserveAmount"));
			InsureResult.Transfer.lDealMin = m_AccountsDBAide.GetValue_LONGLONG(TEXT("TransferDealMin"));
			InsureResult.Transfer.lDealMax = m_AccountsDBAide.GetValue_LONGLONG(TEXT("TransferDealMax"));
			InsureResult.Transfer.lDailyMax = m_AccountsDBAide.GetValue_LONGLONG(TEXT("TransferDailyMax"));
			InsureResult.Transfer.dwFeeRate = m_AccountsDBAide.GetValue_DWORD(TEXT("TransferFeeRate"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_QUERY_RESULT, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = DBR_MB_INSURE_QUERY;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}
};
//银行开通
bool CDataBaseEngineSink::OnRequestMBRequestInsureEnable(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		ASSERT(wDataSize == sizeof(DBR_MB_InsureEnable));
		DBR_MB_InsureEnable* pInfo = (DBR_MB_InsureEnable*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pInfo->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pInfo->szPassword);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//
		DBR_MB_InsureSuccess InsureResult;
		ZeroMemory(&InsureResult, sizeof(InsureResult));

		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_InsureEnable"), true);
		if (lResultCode == DB_SUCCESS)
		{
			InsureResult.lScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			InsureResult.lInsureScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureScore"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_SUCCESS, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}
		else
		{
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			DBR_MB_InsureFailure InsureResult;
			ZeroMemory(&InsureResult, sizeof(InsureResult));

			InsureResult.dwResultCode = lResultCode;
			StrCpy(InsureResult.szDescribeString, szDescribe);

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_FAILURE, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = DBR_MB_INSURE_ENABLE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}
};
//银行存储
bool CDataBaseEngineSink::OnRequestMBRequestInsureSave(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		ASSERT(wDataSize == sizeof(DBR_MB_InsureSave));
		DBR_MB_InsureSave* pInfo = (DBR_MB_InsureSave*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pInfo->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pInfo->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@nScore"), pInfo->lSaveScore);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_INSURE);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pInfo->szMachineID);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//
		DBR_MB_InsureSuccess InsureResult;
		ZeroMemory(&InsureResult, sizeof(InsureResult));

		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_InsureSave"), true);
		if (lResultCode == DB_SUCCESS)
		{
			InsureResult.lScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			InsureResult.lInsureScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureScore"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_SUCCESS, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}
		else
		{
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			DBR_MB_InsureFailure InsureResult;
			ZeroMemory(&InsureResult, sizeof(InsureResult));

			InsureResult.dwResultCode = lResultCode;
			StrCpy(InsureResult.szDescribeString, CW2CT(DBVarValue.bstrVal).m_psz);

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_FAILURE, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = DBR_MB_INSURE_SAVE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}
};
//银行提取
bool CDataBaseEngineSink::OnRequestMBRequestInsureTake(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		ASSERT(wDataSize == sizeof(DBR_MB_InsureTake));
		DBR_MB_InsureTake* pInfo = (DBR_MB_InsureTake*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pInfo->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pInfo->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@nScore"), pInfo->lTakeScore);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pInfo->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_INSURE);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pInfo->szMachineID);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//
		DBR_MB_InsureSuccess InsureResult;
		ZeroMemory(&InsureResult, sizeof(InsureResult));

		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_InsureTake"), true);
		if (lResultCode == DB_SUCCESS)
		{
			InsureResult.lScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			InsureResult.lInsureScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureScore"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_SUCCESS, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}
		else
		{
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			DBR_MB_InsureFailure InsureResult;
			ZeroMemory(&InsureResult, sizeof(InsureResult));

			InsureResult.dwResultCode = lResultCode;
			StrCpy(InsureResult.szDescribeString, CW2CT(DBVarValue.bstrVal).m_psz);

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_FAILURE, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = DBR_MB_INSURE_TAKE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}
};
//银行转账
bool CDataBaseEngineSink::OnRequestMBRequestInsureTransfer(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		ASSERT(wDataSize == sizeof(DBR_MB_InsureTransfer));
		DBR_MB_InsureTransfer* pInfo = (DBR_MB_InsureTransfer*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pInfo->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pInfo->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwGameID"), pInfo->dwGameID);
		m_AccountsDBAide.AddParameter(TEXT("@nScore"), pInfo->lTransferScore);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pInfo->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_INSURE);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pInfo->szMachineID);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//
		DBR_MB_InsureSuccess InsureResult;
		ZeroMemory(&InsureResult, sizeof(InsureResult));

		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_InsureTransfer"), true);
		if (lResultCode == DB_SUCCESS)
		{
			InsureResult.lScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			InsureResult.lInsureScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureScore"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_SUCCESS, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));

			//邮件通知
			DBR_MB_InsureMailNotify InsureMail;
			
			InsureMail.dwUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("TargetUserID"));
			InsureMail.dwMailID = m_AccountsDBAide.GetValue_DWORD(TEXT("TargetMailID"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_MAIL_NOTIFY, dwContextID, dwTokenID, &InsureMail, sizeof(InsureMail));
		}
		else
		{
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			DBR_MB_InsureFailure InsureResult;
			ZeroMemory(&InsureResult, sizeof(InsureResult));

			InsureResult.dwResultCode = lResultCode;
			StrCpy(InsureResult.szDescribeString, CW2CT(DBVarValue.bstrVal).m_psz);

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_FAILURE, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}
		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.wRequestCmdID = DBR_MB_INSURE_TRANSFER;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}
};
//银行重置密码
bool CDataBaseEngineSink::OnRequestMBInsureResetPassword(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_MB_InsureResetPassword));
		if (wDataSize != sizeof(DBR_MB_InsureResetPassword)) return false;

		//请求处理
		DBR_MB_InsureResetPassword* pModifyInsurePass = (DBR_MB_InsureResetPassword*)pData;

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pModifyInsurePass->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pModifyInsurePass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"), pModifyInsurePass->szSrcPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"), pModifyInsurePass->szDstPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);

		//输出变量
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//结果处理
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_ModifyInsurePassword"), true);
		if (lResultCode == DB_SUCCESS)
		{
			DBR_MB_InsureSuccess InsureResult;
			ZeroMemory(&InsureResult, sizeof(InsureResult));

			InsureResult.lScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			InsureResult.lInsureScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureScore"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_SUCCESS, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}
		else
		{
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			DBR_MB_InsureFailure InsureResult;
			ZeroMemory(&InsureResult, sizeof(InsureResult));

			InsureResult.dwResultCode = lResultCode;
			StrCpy(InsureResult.szDescribeString, CW2CT(DBVarValue.bstrVal).m_psz);

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_FAILURE, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"由于系统繁忙，请稍后再试！"));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}
};
//////////////////////////////////////////////////////////////////////////

