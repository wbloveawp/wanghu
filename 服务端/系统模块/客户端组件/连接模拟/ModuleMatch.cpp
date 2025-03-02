#include "Stdafx.h"
#include "ModuleMatch.h"
#include "ModuleServer.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//时钟定义

#define	IDI_TIME_PULSE				1								//时间脉冲

//////////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CModuleMatch::CModuleMatch(IModuleItemSink* pIModuleItemSink):
	m_pIModuleItemSink(pIModuleItemSink)
{
	//设置模块
	SetServiceModule(SERVICE_MODULE_MATCH);

	//设置变量
	ZeroMemory(&m_BufferPacket,sizeof(m_BufferPacket));	
	ZeroMemory(&m_MatchGroupInfo,sizeof(m_MatchGroupInfo));
	ZeroMemory(&m_MatchViewOption, sizeof(m_MatchViewOption));
}

//析构函数
CModuleMatch::~CModuleMatch()
{
}

//关闭事件
bool CModuleMatch::OnEventVirtualShut()
{
	//设置变量
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));

	//投递消息	
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_MATCH_LINK_SHUT, 0, 0);

	//构造消息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 与比赛服务器的虚拟连接断开了"), m_pIModuleItemSink->GetGlobalUserData()->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(255, 0, 0));

	return true;
}

//连接事件
bool CModuleMatch::OnEventVirtualLink(LONG lErrorCode)
{
	//连接失败
	if (lErrorCode != 0)
	{
		//投递消息	
		ASSERT(m_pIModuleItemSink != NULL);
		m_pIModuleItemSink->PostWindowMessage(WM_MATCH_LINK_FAILURE, 0, 0);

		return true;
	}

	//发送数据
	if (m_BufferPacket.wMainCmdID != 0 && m_BufferPacket.wSubCmdID != 0)
	{
		SendVirtualData(m_BufferPacket.wMainCmdID, m_BufferPacket.wSubCmdID, m_BufferPacket.cbDataBuffer, m_BufferPacket.wDataSize);
	}	

	return true;
}

//删除时间
bool CModuleMatch::KillModuleTimer(UINT nTimerID)
{
	//投递消息
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->KillGameTimer(GetTimerStart() + nTimerID);

	return true;
}

//设置时间
bool CModuleMatch::SetModuleTimer(UINT nTimerID, UINT nElapse)
{
	//投递消息
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->SetGameTimer(GetTimerStart() + nTimerID, nElapse);

	return true;
}

//时钟事件
bool CModuleMatch::OnEventTimer(UINT uTimerID, WPARAM wBindParam)
{
	//时钟脉冲
	if (uTimerID == IDI_TIME_PULSE)
	{
		//变量定义
		DWORD dwTimeStamp = (DWORD)time(NULL);

		//房间模块
		auto pModuleServer = (CModuleServer *)m_pIModuleItemSink->GetModuleServer();

		//进入房间
		if (m_MatchGroupInfo.dwMatchID != 0 && pModuleServer->GetLinkStatus()==false)
		{
			//锦标赛
			if (m_MatchGroupInfo.cbMatchType == MATCH_TYPE_TOURNAMENT)
			{
				//构造结构
				auto pTournamentGroupDetail = (tagTournamentGroupDetail*)(m_MatchGroupInfo.cbGroupDetail);

				//获取时间
				CTime TimeCurrent = CTime::GetCurrentTime();
				CTime TimeMatchStart(pTournamentGroupDetail->MatchStartTime);

				//校验时间
				if (TimeCurrent > pTournamentGroupDetail->MatchStartTime && m_MatchGroupInfo.dwMatchTicket>0)
				{
					//构造结构
					CMD_AM_C_QueryMatchSession QueryMatchSession;
					ZeroMemory(&QueryMatchSession,sizeof(QueryMatchSession));

					//设置变量
					QueryMatchSession.dwMatchID = m_MatchGroupInfo.dwMatchID;

					//发送数据
					SendVirtualData(MDM_AM_MATCH_SERVICE, SUB_AM_C_QUERY_MATCH_SESSION, &QueryMatchSession, sizeof(QueryMatchSession));

					//关闭定时器
					KillModuleTimer(IDI_TIME_PULSE);
				}
			}
		}

		//已经进入房间
		if (pModuleServer->GetLinkStatus() == true && GetLinkStatus()==true)
		{
			//3秒查询一次用户
			if (dwTimeStamp % 1 == 0)
			{
				//构造结构
				CMD_AM_C_QuerySignupUser QuerySignupUser;
				ZeroMemory(&QuerySignupUser, sizeof(QuerySignupUser));

				//设置变量
				QuerySignupUser.dwMatchID = m_MatchGroupInfo.dwMatchID;
				QuerySignupUser.lMatchNo = m_MatchGroupInfo.lMatchNo;

				//发送数据
				SendVirtualData(MDM_AM_MATCH_SERVICE, SUB_AM_C_QUERY_SIGNUP_USER, &QuerySignupUser, sizeof(QuerySignupUser));
			}			

			//5秒查询一次桌子
			if (dwTimeStamp % 2 == 0)
			{
				//构造结构
				CMD_AM_C_QueryMatchTable QueryMatchTable;
				ZeroMemory(&QueryMatchTable, sizeof(QueryMatchTable));

				//设置变量
				QueryMatchTable.dwMatchID = m_MatchGroupInfo.dwMatchID;
				QueryMatchTable.lMatchNo = m_MatchGroupInfo.lMatchNo;

				//发送数据
				SendVirtualData(MDM_AM_MATCH_SERVICE, SUB_AM_C_QUERY_MATCH_TABLE, &QueryMatchTable, sizeof(QueryMatchTable));
			}
		}
	}

	return true;
}

//读取事件
bool CModuleMatch::OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_CM_SYSTEM:					//系统命令
	{
		return OnSocketMainSystem(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_AM_MATCH_SERVICE:			//比赛服务
	{
		return OnSocketMainMatchService(Command.wSubCmdID, pData, wDataSize);
	}		
	}

	return true;
}

//系统处理
bool CModuleMatch::OnSocketMainSystem(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CM_SYSTEM_MESSAGE:		//系统消息
	{
		return OnSocketSubSystemMessage(pData, wDataSize);
	}
	case SUB_CM_OPERATE_FAILURE:	//操作失败
	{
		return OnSocketSubOperateFailure(pData, wDataSize);
	}
	}

	return true;
}

//登录处理
bool CModuleMatch::OnSocketMainMatchService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_AM_S_MATCH_OPTION_LIST:	//配置列表
	{
		return OnSocketSubMatchOptionList(pData, wDataSize);
	}
	case SUB_AM_S_MATCH_OPTION_STATUS:	//配置状态
	{
		return OnSocketSubMatchOptionStatus(pData, wDataSize);
	}
	case SUB_AM_S_MATCH_SESSION_ITEM:		//分组子项
	{
		return OnSocketSubMatchSessionItem(pData, wDataSize);
	}
	case SUB_AM_S_MATCH_SESSION_LIST:		//分组列表
	{
		return OnSocketSubMatchSessionList(pData, wDataSize);
	}
	case SUB_AM_S_MATCH_SESSION_DETAIL:	//分组详情
	{
		return OnSocketSubMatchSessionDetail(pData, wDataSize);
	}
	case SUB_AM_S_MATCH_SESSION_STATUS:	//分组状态
	{
		return OnSocketSubMatchSessionStatus(pData, wDataSize);
	}
	case SUB_AM_S_MATCH_ENTER_FINISH:	//进入完成
	{
		return OnSocketSubEnterFinish(pData, wDataSize);
	}
	case SUB_AM_S_BUY_TCIKET_RESULT:	//购买结果
	{
		return OnSocketSubBuyTickResult(pData, wDataSize);
	}
	case SUB_AM_S_QUERY_TCIKET_RESULT:	//查询结果
	{
		return OnSocketSubQueryTickResult(pData, wDataSize);
	}
	}


	return true;
}

//进入完成
bool CModuleMatch::OnSocketSubEnterFinish(VOID* pData, WORD wDataSize)
{
	//设置定时器
	SetModuleTimer(IDI_TIME_PULSE, 1000);

	//用户接口
	ASSERT(m_pIModuleItemSink != NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 比赛服务器进入成功..."), pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

//购买结果
bool CModuleMatch::OnSocketSubBuyTickResult(VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pBuyTicketResult = (CMD_AM_S_BuyTicketResult*)pData;

	//校验场次
	if (pBuyTicketResult->dwMatchID == m_MatchGroupInfo.dwMatchID && pBuyTicketResult->lMatchNo == m_MatchGroupInfo.lMatchNo)
	{
		m_MatchGroupInfo.dwMatchTicket = pBuyTicketResult->dwTicketCount;		
	}

	return true;
}

//查询结果
bool CModuleMatch::OnSocketSubQueryTickResult(VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pQueryTicketResult = (CMD_AM_S_QueryTicketResult*)pData;

	//校验场次
	if (pQueryTicketResult->dwMatchID == m_MatchGroupInfo.dwMatchID && pQueryTicketResult->lMatchNo == m_MatchGroupInfo.lMatchNo)
	{
		m_MatchGroupInfo.dwMatchTicket = pQueryTicketResult->dwTicketCount;
	}

	//购买赛券
	if (m_MatchGroupInfo.dwMatchTicket == 0)
	{
		//构造结构
		CMD_AM_C_BuyMatchTicket BuyMatchTicket;
		ZeroMemory(&BuyMatchTicket, sizeof(BuyMatchTicket));

		//设置变量
		BuyMatchTicket.lMatchNo = m_MatchGroupInfo.lMatchNo;
		BuyMatchTicket.dwMatchID = m_MatchGroupInfo.dwMatchID;

		//查询比赛券
		SendVirtualData(MDM_AM_MATCH_SERVICE, SUB_AM_C_BUY_MATCH_TICKET, &BuyMatchTicket, sizeof(BuyMatchTicket));
	}	

	return true;
}

//分组子项
bool CModuleMatch::OnSocketSubMatchSessionItem(VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pMatchSessionItem = (CMD_AM_S_MatchSessionItem*)pData;	

	//获取模块
	ASSERT(m_pIModuleItemSink != NULL);	
	auto pModuleServer = (CModuleServer*)m_pIModuleItemSink->GetModuleServer();	

	//比赛信息
	SCORE lMatchNo = pMatchSessionItem->MatchSessionItem.lMatchNo;
	DWORD dwMatchID = pMatchSessionItem->MatchSessionItem.dwMatchID;

	//进入房间
	pModuleServer->EnterMatchServer(m_MatchViewOption.wKindID, pMatchSessionItem->MatchSessionItem.wServerID, dwMatchID, lMatchNo);

	//关闭链接
	CloseVirtualLink();

	return true;
}

//分组列表
bool CModuleMatch::OnSocketSubMatchSessionList(VOID* pData, WORD wDataSize)
{
	return true;
}

//分组详情
bool CModuleMatch::OnSocketSubMatchSessionDetail(VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pMatchSessionDetail = (CMD_AM_S_MatchSessionDetail*)pData;

	//设置变量
	m_MatchGroupInfo.dwMatchID = pMatchSessionDetail->dwMatchID;
	m_MatchGroupInfo.lMatchNo = pMatchSessionDetail->lMatchNo;
	m_MatchGroupInfo.cbMatchType = pMatchSessionDetail->cbMatchType;
	m_MatchGroupInfo.wStartCount = pMatchSessionDetail->wStartCount;
	m_MatchGroupInfo.wSignupCount = pMatchSessionDetail->wSignupCount;

	//扩展数据
	CopyMemory(m_MatchGroupInfo.cbGroupDetail, pMatchSessionDetail + 1, wDataSize-sizeof(CMD_AM_S_MatchSessionDetail));

	//构造结构
	CMD_AM_C_QueryMatchTicket QueryMatchTicket;
	ZeroMemory(&QueryMatchTicket,sizeof(QueryMatchTicket));

	//设置变量
	QueryMatchTicket.lMatchNo = m_MatchGroupInfo.lMatchNo;
	QueryMatchTicket.dwMatchID = m_MatchGroupInfo.dwMatchID;	

	//查询比赛券
	SendVirtualData(MDM_AM_MATCH_SERVICE,SUB_AM_C_QUERY_MATCH_TICKET,&QueryMatchTicket,sizeof(QueryMatchTicket));

	return true;
}

//分组状态
bool CModuleMatch::OnSocketSubMatchSessionStatus(VOID* pData, WORD wDataSize)
{
	return true;
}

//配置列表
bool CModuleMatch::OnSocketSubMatchOptionList(VOID* pData, WORD wDataSize)
{
	return true;
}

//分组状态
bool CModuleMatch::OnSocketSubMatchOptionStatus(VOID* pData, WORD wDataSize)
{
	return true;
}

//系统消息
bool CModuleMatch::OnSocketSubSystemMessage(VOID* pData, WORD wDataSize)
{
	//变量定义
	CMD_CM_SystemMessage* pSystemMessage = (CMD_CM_SystemMessage*)pData;
	WORD wHeadSize = sizeof(CMD_CM_SystemMessage) - sizeof(pSystemMessage->szString);

	//效验参数
	ASSERT((wDataSize > wHeadSize) && (wDataSize == (wHeadSize + pSystemMessage->wLength * sizeof(TCHAR))));
	if ((wDataSize <= wHeadSize) || (wDataSize != (wHeadSize + pSystemMessage->wLength * sizeof(TCHAR)))) return false;

	//关闭处理
	if ((pSystemMessage->wType & (SMT_CLOSE_LINK)) != 0)
	{
		//关闭连接
		CloseVirtualLink();
	}

	return true;
}

//操作失败
bool CModuleMatch::OnSocketSubOperateFailure(VOID* pData, WORD wDataSize)
{
	//获取数据
	auto pOperateFailure = (CMD_CM_OperateFailure*)pData;

	//用户接口
	ASSERT(m_pIModuleItemSink != NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//打印信息
	/*CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 操作失败[%s]..."), pGlobalUserData->szAccounts, pOperateFailure->szDescribeString);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));*/

	//关闭连接
	CloseVirtualLink();

	return true;
}

//进入服务
bool CModuleMatch::EnterService()
{
	//用户接口
	ASSERT(m_pIModuleItemSink != NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//校验状态
	if (GetLinkStatus() == true)
	{
		SendVirtualData(MDM_AM_MATCH_SERVICE, SUB_AM_C_ENTER_MATCH, NULL, 0);
	}
	else
	{
		//缓冲数据
		m_BufferPacket.wMainCmdID = MDM_AM_MATCH_SERVICE;
		m_BufferPacket.wSubCmdID = SUB_AM_C_ENTER_MATCH;
		m_BufferPacket.wDataSize = 0;		

		//创建连接
		CreateVirtualLink(INVALID_WORD, pGlobalUserData->dwUserID, pGlobalUserData->szLogonToken, NULL, 0);
	}

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 正在登录比赛服务器..."), pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

//重置模块
VOID CModuleMatch::ResetModule()
{
	//设置变量
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));
	ZeroMemory(&m_MatchGroupInfo, sizeof(m_MatchGroupInfo));
	ZeroMemory(&m_MatchViewOption, sizeof(m_MatchViewOption));
}
//////////////////////////////////////////////////////////////////////////////////////////////
