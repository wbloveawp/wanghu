#include "Stdafx.h"
#include "GlobalUnits.h"
#include "ModuleServer.h"
#include "RobotUserManager.h"

//////////////////////////////////////////////////////////////////////////////////////////
//时钟定义

#define	IDI_GAME_PULSE				1								//游戏脉冲

//////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CModuleServer::CModuleServer(IModuleItemSink* pIModuleItemSink) :
	m_pIModuleItemSink(pIModuleItemSink)
{
	//设置模块
	SetServiceModule(SERVICE_MODULE_GAME);

	//配置变量
	m_wKindID=0;
	m_wTableCount=0;
	m_wChairCount=0;
	m_wServerType=0;
	m_dwServerRule=0;

	//接口变量
	m_pRobotUserItem = NULL;

	//用户数据
	m_pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();
	if (m_pGlobalUserData != NULL)
	{
		m_pGlobalUserData->cbUserStatus = US_NULL;
		m_pGlobalUserData->wChairID = INVALID_WORD;
		m_pGlobalUserData->wTableID = INVALID_WORD;
	}

	//设置变量
	ZeroMemory(&m_BufferPacket,sizeof(m_BufferPacket));	
}

//析构函数
CModuleServer::~CModuleServer()
{	
}

//关闭事件
bool CModuleServer::OnEventVirtualShut()
{
	//设置变量
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));

	//启动标志
	bool bStartClient = false;
	if (m_pRobotUserItem != NULL)
	{
		bStartClient = m_pRobotUserItem->IsStartClient();		
	}

	//构造消息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 与游戏房间的虚拟连接断开了"), m_pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(),RGB(255,0,0));

	//投递消息
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_SERVER_LINK_SHUT, bStartClient, m_wServiceID);

	return true;
}

//连接事件
bool CModuleServer::OnEventVirtualLink(LONG lErrorCode)
{
	//连接失败
	if (lErrorCode != 0)
	{
		//投递消息
		ASSERT(m_pIModuleItemSink != NULL);
		m_pIModuleItemSink->PostWindowMessage(WM_SERVER_LINK_FAILURE, 0, 0);

		return true;
	}

	//发送数据
	if (m_BufferPacket.wMainCmdID != 0 && m_BufferPacket.wSubCmdID != 0)
	{
		SendVirtualData(m_BufferPacket.wMainCmdID, m_BufferPacket.wSubCmdID, m_BufferPacket.cbDataBuffer, m_BufferPacket.wDataSize);
	}

	return true;
}

//读取事件
bool CModuleServer::OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GR_LOGON:			//登录处理
	{
		return OnSocketMainLogon(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_GR_CONFIG:			//配置处理
	{
		return OnSocketMainConfig(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_GR_USER:			//用户处理
	{
		return OnSocketMainUser(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_GR_STATUS:			//状态处理
	{
		return OnSocketMainStatus(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_GR_MATCH:			//比赛处理
	{
		return OnSocketMainMatch(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_CM_SYSTEM:			//系统命令
	{
		return OnSocketMainSystem(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_GF_GAME:			//游戏处理
	case MDM_GF_FRAME:			//框架处理
	{
		if (m_pRobotUserItem != NULL)
		{
			return m_pRobotUserItem->OnSocketGameMessage(Command.wMainCmdID, Command.wSubCmdID, pData, wDataSize);
		}

		return true;
	}	
	}

	return true;
}

//删除时间
bool CModuleServer::KillModuleTimer(UINT nTimerID)
{
	//投递消息
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->KillGameTimer(GetTimerStart() + nTimerID);

	return true;
}

//设置时间
bool CModuleServer::SetModuleTimer(UINT nTimerID, UINT nElapse)
{
	//投递消息
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->SetGameTimer(GetTimerStart() + nTimerID, nElapse);

	return true;
}

//时钟事件
bool CModuleServer::OnEventTimer(UINT uTimerID, WPARAM wBindParam)
{
	//游戏脉冲
	if (uTimerID == IDI_GAME_PULSE)
	{
		if (m_pRobotUserItem != NULL)
		{
			m_pRobotUserItem->OnTimerPulse(uTimerID, wBindParam);
		}		
	}	

	return true;
}

//登录处理
bool CModuleServer::OnSocketMainLogon(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_LOGON_FAILURE:		//登录失败
	{
		return OnSocketSubLogonFailure(pData, wDataSize);
	}
	case SUB_GR_LOGON_SUCCESS:		//登录成功
	{
		return OnSocketSubLogonSuccess(pData, wDataSize);
	}
	case SUB_GR_LOGON_FINISH:		//登录完成
	{
		return OnSocketSubLogonFinish(pData, wDataSize);
	}
	case SUB_GR_UPDATE_NOTIFY:		//升级提示
	{
		return OnSocketSubUpdateNotify(pData, wDataSize);
	}
	}

	return true;
}

//配置处理
bool CModuleServer::OnSocketMainConfig(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_CONFIG_SERVER:		//配置房间
	{
		return OnSocketSubConfigServer(pData, wDataSize);
	}
	case SUB_GR_CONFIG_RULE:		//配置规则
	{
		return OnSocketSubConfigRule(pData, wDataSize);
	}
	}

	return true;
}

//用户处理
bool CModuleServer::OnSocketMainUser(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_USER_ENTER:			//用户进入
	{
		return OnSocketSubUserEnter(pData, wDataSize);
	}
	case SUB_GR_USER_SCORE:			//用户分数
	{
		return OnSocketSubUserScore(pData, wDataSize);
	}
	case SUB_GR_USER_STATUS:		//用户状态
	{
		return OnSocketSubUserStatus(pData, wDataSize);
	}
	case SUB_GR_REQUEST_FAILURE:	//请求失败
	{
		return OnSocketSubRequestFailure(pData, wDataSize);
	}
	}

	return true;
}

//状态处理
bool CModuleServer::OnSocketMainStatus(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	return true;
}

//系统处理
bool CModuleServer::OnSocketMainSystem(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CM_SYSTEM_MESSAGE:		//系统消息
	{
		return OnSocketSubSystemMessage(pData,wDataSize);
	}	 
	}

	return true;
}

//比赛处理
bool CModuleServer::OnSocketMainMatch(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_MATCH_STATUS:		//比赛状态
	{
		return OnSocketSubMatchStatus(pData,wDataSize);
	}
	case SUB_GR_MATCH_USTATUS:		//用户状态
	{
		return OnSocketSubMatchUStatus(pData, wDataSize);
	}
	case SUB_GR_SIGNUP_SUCCESS:		//报名成功
	{
		return OnSocketSubSignupSuccess(pData, wDataSize);
	}
	}

	return true;
}

//登录失败
bool CModuleServer::OnSocketSubLogonFailure(VOID* pData, WORD wDataSize)
{
	//提取数据
	CMD_GR_LogonFailure* pLogonFailure = (CMD_GR_LogonFailure*)pData;

	//构造消息
	CString strMessage;
	strMessage.Format(TEXT("登录失败, Accounts=%s, ErrorMsg=%s, ErrorCode=%d"), m_pGlobalUserData->szAccounts, pLogonFailure->szDescribeString, pLogonFailure->lErrorCode);

	//插入消息
	m_pIModuleItemSink->InsertString(strMessage.GetString(),RGB(255,0,0));

	//投递消息
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_SERVER_LOGON_FAILURE, 0, 0);

	return true;
}

//登录成功
bool CModuleServer::OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize)
{
	return true;
}

//登录完成
bool CModuleServer::OnSocketSubLogonFinish(VOID* pData, WORD wDataSize)
{
	//规则变量
	CMD_GR_UserRule UserRule;
	ZeroMemory(&UserRule, sizeof(UserRule));

	//发送规则
	SendVirtualData(MDM_GR_USER, SUB_GR_USER_RULE, &UserRule, sizeof(UserRule));

	//创建对象
	if (m_pRobotUserItem == NULL)
	{
		m_pRobotUserItem = g_RobotUserManager->ActiveRobotUserItem();
		if (m_pRobotUserItem ==NULL)
		{
			//关闭链接
			CloseVirtualLink();

			return true;
		}	

		//设置接口
		m_pRobotUserItem->SetVirtualLinkItem(this);
		m_pRobotUserItem->SetGlobalUserData(m_pGlobalUserData);
	}

	//重入判断
	if (m_pRobotUserItem->IsStartClient()==false && m_pGlobalUserData->wTableID != INVALID_TABLE)
	{
		m_pRobotUserItem->StartGameClient();

		//打印信息
		CString strMessage;
		strMessage.Format(TEXT("Accounts=%s: 启动游戏！"), m_pGlobalUserData->szAccounts);
		m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));
	}

	//投递消息
	m_pIModuleItemSink->PostWindowMessage(WM_SERVER_LOGON_FINISH, 0, 0);

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 房间登录成功..."), m_pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	//启动定时器
	SetModuleTimer(IDI_GAME_PULSE, 1000);

	return true;
}

//更新提示
bool CModuleServer::OnSocketSubUpdateNotify(VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pUpdateNotify = (CMD_GR_UpdateNotify *) pData;

	//投递消息
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_SERVER_LOGON_FAILURE, 0, 0);

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 房间登录失败，版本不一致。PlazaVersion=%d,ClientVersion=%d,"), m_pGlobalUserData->szAccounts, pUpdateNotify->dwCurrentPlazaVersion,
		pUpdateNotify->dwCurrentClientVersion);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

//配置规则
bool CModuleServer::OnSocketSubConfigRule(VOID* pData, WORD wDataSize)
{
	return true;
}

//配置房间
bool CModuleServer::OnSocketSubConfigServer(VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pConfigServer = (CMD_GR_ConfigServer*)pData;

	//设置变量
	m_wKindID = pConfigServer->wKindID;
	m_wChairCount = pConfigServer->wChairCount;
	m_wTableCount = pConfigServer->wTableCount;
	m_wServerType = pConfigServer->wServerType;
	m_dwServerRule = pConfigServer->dwServerRule;

	return true;
}

//用户进入
bool CModuleServer::OnSocketSubUserEnter(VOID* pData, WORD wDataSize)
{
	//提取用户
	auto pUserInfoHead = (tagMBUserInfoHead*)pData;

	//校验用户
	if (pUserInfoHead->dwUserID == m_pGlobalUserData->dwUserID)
	{
		//更新状态
		m_pGlobalUserData->wTableID = pUserInfoHead->wTableID;
		m_pGlobalUserData->wChairID = pUserInfoHead->wChairID;
		m_pGlobalUserData->cbUserStatus = pUserInfoHead->cbUserStatus;

		//设置积分
		m_pGlobalUserData->lUserGold = pUserInfoHead->lGold;
		m_pGlobalUserData->lGameScore = pUserInfoHead->lScore;
	}

	return true;
}

//用户积分
bool CModuleServer::OnSocketSubUserScore(VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pUserScore = (CMD_GR_UserScore*)pData;

	//更新分数
	m_pGlobalUserData->lGameScore = pUserScore->UserScore.lScore;

	return true;
}

//用户状态
bool CModuleServer::OnSocketSubUserStatus(VOID* pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize == sizeof(CMD_GR_UserStatus));
	if (wDataSize < sizeof(CMD_GR_UserStatus)) return false;

	//变量定义
	CMD_GR_UserStatus* pUserStatus = (CMD_GR_UserStatus*)pData;

	//玩家判断
	if (pUserStatus->dwUserID == m_pGlobalUserData->dwUserID)
	{
		//用户状态
		tagUserStatus LastUserStatus;
		LastUserStatus.wChairID = m_pGlobalUserData->wChairID;
		LastUserStatus.wTableID = m_pGlobalUserData->wTableID;
		LastUserStatus.cbUserStatus = m_pGlobalUserData->cbUserStatus;

		//设置状态
		m_pGlobalUserData->wChairID = pUserStatus->UserStatus.wChairID;
		m_pGlobalUserData->wTableID = pUserStatus->UserStatus.wTableID;
		m_pGlobalUserData->cbUserStatus = pUserStatus->UserStatus.cbUserStatus;

		//校验对象
		if (m_pRobotUserItem != NULL)
		{
			//关闭判断
			if ((m_pRobotUserItem->IsStartClient() == true) && (m_pGlobalUserData->wTableID == INVALID_TABLE))
			{
				//关闭游戏
				m_pRobotUserItem->CloseGameClient();

				//打印信息
				CString strMessage;
				strMessage.Format(TEXT("Accounts=%s: 关闭游戏！"), m_pGlobalUserData->szAccounts);
				m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

				return true;
			}

			//启动判断
			if ((m_pRobotUserItem->IsStartClient() == false) && (m_pGlobalUserData->wTableID != INVALID_TABLE))
			{
				//启动游戏
				m_pRobotUserItem->StartGameClient();

				//打印信息
				CString strMessage;
				strMessage.Format(TEXT("Accounts=%s: 启动游戏！"), m_pGlobalUserData->szAccounts);
				m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

				return true;
			}
		}		
	}

	return true;
}

//系统消息
bool CModuleServer::OnSocketSubSystemMessage(VOID* pData, WORD wDataSize)
{
	//变量定义
	CMD_CM_SystemMessage* pSystemMessage = (CMD_CM_SystemMessage*)pData;
	WORD wHeadSize = sizeof(CMD_CM_SystemMessage) - sizeof(pSystemMessage->szString);

	//效验参数
	ASSERT((wDataSize > wHeadSize) && (wDataSize == (wHeadSize + pSystemMessage->wLength * sizeof(TCHAR))));
	if ((wDataSize <= wHeadSize) || (wDataSize != (wHeadSize + pSystemMessage->wLength * sizeof(TCHAR)))) return false;

	//关闭处理
	if ((pSystemMessage->wType & (SMT_CLOSE_ROOM | SMT_CLOSE_LINK)) != 0)
	{
		//关闭连接
		CloseVirtualLink();
	}

	return true;
}

//请求失败
bool CModuleServer::OnSocketSubRequestFailure(VOID* pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize <= sizeof(CMD_GR_RequestFailure));
	if (wDataSize > sizeof(CMD_GR_RequestFailure)) return false;

	//变量定义
	CMD_GR_RequestFailure* pRequestFailure = (CMD_GR_RequestFailure*)pData;

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("用户【%s】-- %s"), m_pGlobalUserData->szAccounts, pRequestFailure->szDescribeString);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	//关闭连接
	CloseVirtualLink();

	return true;
}

//比赛状态
bool CModuleServer::OnSocketSubMatchStatus(VOID* pData, WORD wDataSize)
{
	return true;
}

//比赛状态
bool CModuleServer::OnSocketSubMatchUStatus(VOID* pData, WORD wDataSize)
{
	auto cbMatchUserStatus = *(BYTE*)pData;

	//校验状态
	if (cbMatchUserStatus == MUS_NULL)
	{
		//构造结构
		CMD_GR_MatchSignup MatchSignup;
		ZeroMemory(&MatchSignup,sizeof(MatchSignup));

		//设置变量
		MatchSignup.lMatchNo = m_lMatchNo;
		MatchSignup.dwMatchID = m_dwMatchID;

		//发送数据
		SendVirtualData(MDM_GR_MATCH, SUB_GR_MATCH_SIGNUP, &MatchSignup, sizeof(MatchSignup));

		//打印信息
		CString strMessage;
		strMessage.Format(TEXT("Accounts=%s: 正在报名比赛..."), m_pGlobalUserData->szAccounts);
		m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));
	}

	//比赛完成
	if (cbMatchUserStatus == MUS_COMPLETED)
	{
		//比赛完成
		m_pIModuleItemSink->PostWindowMessage(WM_SERVER_MATCH_COMPLETED,0,0);
	}

	return true;
}

//报名成功
bool CModuleServer::OnSocketSubSignupSuccess(VOID* pData, WORD wDataSize)
{
	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 比赛报名成功..."), m_pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

//重置房间
VOID CModuleServer::ResetModule()
{
	//配置变量
	m_wKindID = 0;
	m_wTableCount = 0;
	m_wChairCount = 0;
	m_wServerType = 0;
	m_dwServerRule = 0;

	//比赛信息
	m_lMatchNo = 0;
	m_dwMatchID = 0;
	m_cbMatchUStatus = MUS_NULL;

	//设置状态
	m_pGlobalUserData->cbUserStatus = US_NULL;
	m_pGlobalUserData->wChairID = INVALID_CHAIR;
	m_pGlobalUserData->wTableID = INVALID_TABLE;	

	//设置变量
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));

	//关闭定时器
	KillModuleTimer(IDI_GAME_PULSE);

	//释放对象
	if (m_pRobotUserItem != NULL)
	{
		g_RobotUserManager->FreeRobotUserItem(m_pRobotUserItem);		
	}

	//设置变量
	m_pRobotUserItem = NULL;	
}

//进入房间
bool CModuleServer::EnterServer(WORD wKindID, WORD wServerID, WORD wServerLevel)
{
	return SendLogonPacket(wKindID, wServerLevel, wServerID, NULL, 0);
}

//进入房间
bool CModuleServer::EnterMatchServer(WORD wKindID, WORD wServerID, DWORD dwMatchID, SCORE lMatchNo)
{
	//保存信息
	m_lMatchNo = lMatchNo;
	m_dwMatchID = dwMatchID;

	//附加数据
	tagLogonMatchDetails LogonMatchDetails;
	ZeroMemory(&LogonMatchDetails, sizeof(LogonMatchDetails));

	//设置变量
	LogonMatchDetails.lMatchNo = m_lMatchNo;
	LogonMatchDetails.dwMatchID = m_dwMatchID;

	return SendLogonPacket(wKindID, 0, wServerID, &LogonMatchDetails, sizeof(LogonMatchDetails));
}

//登录房间
bool CModuleServer::SendLogonPacket(WORD wKindID, WORD wServerLevel, WORD wServerID, VOID* pExtendData, WORD wDataSize)
{
	//构造结构
	CMD_GR_LogonMobile * pLogonMobile = (CMD_GR_LogonMobile*)m_BufferPacket.cbDataBuffer;
	ZeroMemory(pLogonMobile,sizeof(CMD_GR_LogonMobile));

	//设置变量
	pLogonMobile->dwPlazaVersion = 16842752;
	pLogonMobile->dwProcessVersion = g_GlobalUnits.m_dwProcessVersion;
	pLogonMobile->dwDeviceKind = DEVICE_KIND_ANDROID;	

	//机器码
	CWHService::GetMachineID(pLogonMobile->szMachineID);

	//扩展数据
	if (wDataSize > 0)
	{
		CopyMemory(pLogonMobile+1, pExtendData, wDataSize);
	}

	//校验状态
	if (GetLinkStatus() == true)
	{
		SendVirtualData(MDM_GR_LOGON, SUB_GR_LOGON_MOBILE, pLogonMobile, sizeof(CMD_GR_LogonMobile)+wDataSize);
	}
	else
	{
		//缓冲数据
		m_BufferPacket.wMainCmdID = MDM_GR_LOGON;
		m_BufferPacket.wSubCmdID = SUB_GR_LOGON_MOBILE;
		m_BufferPacket.wDataSize = sizeof(CMD_GR_LogonMobile)+wDataSize;		

		//扩展信息
		tagLinkExtend_Server LinkExtend;
		ZeroMemory(&LinkExtend,sizeof(LinkExtend));

		//设置变量
		LinkExtend.wKindID = wKindID;
		LinkExtend.wServerLevel = wServerLevel;	

		//创建连接
		CreateVirtualLink(wServerID, m_pGlobalUserData->dwUserID, m_pGlobalUserData->szLogonToken, (LPBYTE)&LinkExtend, sizeof(LinkExtend));
	}

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 正在登录房间..."), m_pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////