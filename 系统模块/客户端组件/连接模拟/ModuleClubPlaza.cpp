#include "Stdafx.h"
#include "ModuleClub.h"
#include "ModuleClubPlaza.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//时钟定义

#define	IDI_TIME_PULSE				1								//时间脉冲

//////////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CModuleClubPlaza::CModuleClubPlaza(IModuleItemSink* pIModuleItemSink):
	m_pIModuleItemSink(pIModuleItemSink)
{
	//设置模块
	SetServiceModule(SERVICE_MODULE_CLUBPLAZA);

	//设置变量
	ZeroMemory(&m_ClubOption, sizeof(m_ClubOption));
	ZeroMemory(&m_ClubNameCard, sizeof(m_ClubNameCard));
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));
}

//析构函数
CModuleClubPlaza::~CModuleClubPlaza()
{
}

//关闭事件
bool CModuleClubPlaza::OnEventVirtualShut()
{
	//设置变量
	ZeroMemory(&m_ClubOption, sizeof(m_ClubOption));
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));

	//投递消息	
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_CLUBPLAZA_LINK_SHUT, 0, 0);

	//构造消息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 与茶社广场服务器的虚拟连接断开了"), m_pIModuleItemSink->GetGlobalUserData()->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(255, 0, 0));

	return true;
}

//连接事件
bool CModuleClubPlaza::OnEventVirtualLink(LONG lErrorCode)
{
	//连接失败
	if (lErrorCode != 0)
	{
		//投递消息	
		ASSERT(m_pIModuleItemSink != NULL);
		m_pIModuleItemSink->PostWindowMessage(WM_CLUBPLAZA_LINK_FAILURE, 0, 0);

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
bool CModuleClubPlaza::KillModuleTimer(UINT nTimerID)
{
	//投递消息
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->KillGameTimer(GetTimerStart() + nTimerID);

	return true;
}

//设置时间
bool CModuleClubPlaza::SetModuleTimer(UINT nTimerID, UINT nElapse)
{
	//投递消息
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->SetGameTimer(GetTimerStart() + nTimerID, nElapse);

	return true;
}

//时钟事件
bool CModuleClubPlaza::OnEventTimer(UINT uTimerID, WPARAM wBindParam)
{
	//时钟脉冲
	if (uTimerID == IDI_TIME_PULSE)
	{
		//变量定义
		DWORD dwTimeStamp = (DWORD)time(NULL);

		if (dwTimeStamp % 5 == 0 && !m_bTestFlag)
		{
			//处理消息
			if (m_ArrayApplyMessage.GetCount() > 0)
			{
				/*for (int i = 0; i < m_ArrayApplyMessage.GetCount(); i++)
				{
					if (m_ArrayApplyMessage[i].cbMessageStatus == APPLY_STATUS_NONE)
					{
						RespondJoinClub(&m_ArrayApplyMessage[i], APPLY_STATUS_AGREE);
					}
				}*/

				m_ArrayApplyMessage.RemoveAll();
			}

			//查询茶社
			if (m_ClubNameCard.dwClubID % 3 == 0)
			{
				m_bTestFlag = true;
				SearchClubItem(m_ClubNameCard.dwClubID+1);
			}

			//更新茶社
			if (m_ClubNameCard.dwClubID % 3 == 1)
			{
				m_bTestFlag = true;
				UpdateClubItem(m_ClubNameCard.dwClubID);
			}

			//删除茶社
			/*if (m_ClubNameCard.dwClubID % 3 == 2)
			{
				m_bTestFlag = true;
				DeleteClubItem(m_ClubNameCard.dwClubID);
			}*/
		}		
	}

	return true;
}

//读取事件
bool CModuleClubPlaza::OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_CLUBPLAZA_SERVICE:			//茶社服务
	{
		return OnSocketMainClubPlazaService(Command.wSubCmdID, pData, wDataSize);
	}		
	}

	return true;
}

//登录处理
bool CModuleClubPlaza::OnSocketMainClubPlazaService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CP_S_LOGON_FINISH:		//登录完成
	{
		return OnSocketSubLogonFinish(pData, wDataSize);
	}
	case SUB_CP_S_LOGON_SUCCESS:	//登录成功
	{
		return OnSocketSubLogonSuccess(pData,wDataSize);
	}
	case SUB_CP_S_LOGON_FAILURE:	//登录失败
	{
		return OnSocketSubLogonFailure(pData, wDataSize);
	}
	case SUB_CP_S_CLUB_OPTION:		//茶社配置
	{
		return OnSocketSubClubOption(pData, wDataSize);
	}
	case SUB_CP_S_CLUB_NAMECARD:	//茶社名片
	{
		return OnSocketSubClubNameCard(pData, wDataSize);
	}
	case SUB_CP_S_APPLY_MESSAGE:	//茶社消息
	{
		return OnSocketSubClubMessage(pData, wDataSize);
	}
	case SUB_CP_S_SEARCH_RESULT:	//查询结果
	{
		return OnSocketSubSearchResult(pData, wDataSize);
	}
	case SUB_CP_S_OPERATE_SUCCESS:	//操作成功
	{
		return OnSocketSubOperateSuccess(pData, wDataSize);
	}
	case SUB_CP_S_OPERATE_FAILURE:	//操作失败
	{
		return OnSocketSubOperateFailure(pData, wDataSize);
	}
	}

	return true;
}

//进入失败
bool CModuleClubPlaza::OnSocketSubLogonFailure(VOID* pData, WORD wDataSize)
{
	auto pLogonFailure = (CMD_CP_S_LogonFailure*)pData;

	//打印信息
	CString strMessage;
	m_pIModuleItemSink->InsertString(pLogonFailure->szErrorDescribe, RGB(128, 128, 128));

	//投递消息		
	m_pIModuleItemSink->PostWindowMessage(WM_CLUBPLAZA_LOGON_FAILURE, 0, 0);

	return true;
}

//查询结果
bool CModuleClubPlaza::OnSocketSubSearchResult(VOID* pData, WORD wDataSize)
{
	auto pSearchResult = (CMD_CP_S_SearchResult*)pData;

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("查询到俱乐部[%d][%s]"), pSearchResult->ClubNameCard.dwClubID, pSearchResult->ClubNameCard.szClubName);
	m_pIModuleItemSink->InsertString(strMessage, RGB(128, 128, 128));

	//申请加入
	RequestJoinClub(pSearchResult->ClubNameCard.dwClubID);

	return true;
}

//操作成功
bool CModuleClubPlaza::OnSocketSubOperateSuccess(VOID* pData, WORD wDataSize)
{
	auto pOperateSuccess = (CMD_CP_S_OperateSuccess*)pData;

	//打印信息
	if (pOperateSuccess->szOperateNotice[0] != 0)
	{
		CString strMessage;
		m_pIModuleItemSink->InsertString(pOperateSuccess->szOperateNotice, RGB(128, 128, 128));
	}

	return true;
}

//操作失败
bool CModuleClubPlaza::OnSocketSubOperateFailure(VOID* pData, WORD wDataSize)
{
	auto pOperateFailure = (CMD_CP_S_OperateFailure*)pData;

	//打印信息
	CString strMessage;
	m_pIModuleItemSink->InsertString(pOperateFailure->szErrorDescribe, RGB(128, 128, 128));

	return true;
}

//进入成功
bool CModuleClubPlaza::OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize)
{
	return true;
}

//茶社配置
bool CModuleClubPlaza::OnSocketSubClubOption(VOID* pData, WORD wDataSize)
{
	m_ClubOption = *(tagClubOption*)pData;

	return true;
}

//茶社名片
bool CModuleClubPlaza::OnSocketSubClubNameCard(VOID* pData, WORD wDataSize)
{
	//校验参数
	ASSERT(wDataSize%sizeof(tagClubNameCard)==0);
	if (wDataSize % sizeof(tagClubNameCard) != 0) return false;

	//茶社名片
	auto pClubNameCard = (tagClubNameCard*)pData;

	//拷贝名片
	CopyMemory(&m_ClubNameCard, pClubNameCard, sizeof(m_ClubNameCard));	

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("收到茶社[%d]名片消息"),m_ClubNameCard.dwClubID);
	m_pIModuleItemSink->InsertString(strMessage, RGB(128, 128, 128));

	return true;
}

//茶社消息
bool CModuleClubPlaza::OnSocketSubClubMessage(VOID* pData, WORD wDataSize)
{
	auto pClubMessage = (CMD_CP_S_ApplyMessage*)pData;

	//计算数量
	auto pMsgItem = (tagClubApplyMessage*)(pClubMessage + 1);
	auto nMsgCount = (wDataSize - sizeof(CMD_CP_S_ApplyMessage)) / sizeof(tagClubApplyMessage);

	for (int i = 0; i < nMsgCount; i++)
	{
		m_ArrayApplyMessage.Add(*pMsgItem++);
	}

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("收到茶社[%d]申请消息"), m_ClubNameCard.dwClubID);
	m_pIModuleItemSink->InsertString(strMessage, RGB(128, 128, 128));

	return true;
}

//进入完成
bool CModuleClubPlaza::OnSocketSubLogonFinish(VOID* pData, WORD wDataSize)
{
	//设置定时器
	SetModuleTimer(IDI_TIME_PULSE, 1000);

	//用户接口
	ASSERT(m_pIModuleItemSink != NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 茶社广场登录完成..."), pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	//设置标志
	m_bTestFlag = false;	

	//校验茶社
	if (m_ClubNameCard.dwClubID != 0)
	{
		//茶社模块
		auto pModuleClub = (CModuleClub*)m_pIModuleItemSink->GetModuleClub();

		//进入茶社
		pModuleClub->EnterService(m_ClubNameCard.dwClubID);				
	}	
	else
	{
		//创建茶社
		CreateClubItem();
	}

	return true;
}

//进入服务
bool CModuleClubPlaza::EnterService()
{
	//用户接口
	ASSERT(m_pIModuleItemSink != NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//校验状态
	if (GetLinkStatus() == true)
	{
		SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_ENTER_SERVICE, NULL, 0);
	}
	else
	{
		//缓冲数据
		m_BufferPacket.wDataSize = 0;
		m_BufferPacket.wMainCmdID = MDM_CLUBPLAZA_SERVICE;
		m_BufferPacket.wSubCmdID = SUB_CP_C_ENTER_SERVICE;		

		//创建连接
		CreateVirtualLink(INVALID_WORD, pGlobalUserData->dwUserID, pGlobalUserData->szLogonToken, NULL, 0);
	}

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 正在登录茶社广场..."), pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

//创建茶社
bool CModuleClubPlaza::CreateClubItem()
{
	//构造对象
	CMD_CP_C_CreateClub  CreateClub = {};

	//变量定义
	CreateClub.wClubKind = rand() % CLUB_KIND_COUNT;
	CreateClub.cbPayType = m_ClubOption.ClubKindOption[CreateClub.wClubKind].cbPayTypeMask;
	CreateClub.cbCurrencyKind = m_ClubOption.ClubKindOption[CreateClub.wClubKind].cbCurrencyKindMask;
	
	//俱乐部
	_sntprintf_s(CreateClub.szClubName,CountArray(CreateClub.szClubName),TEXT("%s的茶社-%d-%04d"), m_pIModuleItemSink->GetGlobalUserData()->szAccounts, CreateClub.wClubKind,rand()%1000);
	_sntprintf_s(CreateClub.szClubIntroduce, CountArray(CreateClub.szClubIntroduce), TEXT("茶社介绍"));

	//发送消息
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_CREATE_CLUB, &CreateClub, sizeof(CreateClub));

	return true;
}

//请求加入
VOID CModuleClubPlaza::RequestJoinClub(DWORD dwClubID)
{
	CMD_CP_C_ApplyRequest ApplyRequest = {};

	ApplyRequest.dwClubID = dwClubID;

	//发送消息
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_APPLY_REQUEST, &ApplyRequest, sizeof(ApplyRequest));

	m_pIModuleItemSink->InsertString(TEXT("申请加入茶社"), RGB(128, 128, 128));
}

//删除消息
VOID CModuleClubPlaza::DeleteClubMessage(DWORD dwMessageID, BYTE cbMessageType)
{
	CMD_CP_C_ApplyDelete ApplyDelete = {};

	ApplyDelete.dwMessageID = dwMessageID;
	ApplyDelete.cbMessageType = cbMessageType;

	//发送消息
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_APPLY_DELETE, &ApplyDelete, sizeof(ApplyDelete));

	m_pIModuleItemSink->InsertString(TEXT("删除茶社申请"), RGB(128, 128, 128));
}

//删除成员
VOID CModuleClubPlaza::DeleteClubMember(DWORD dwClubID, DWORD dwMemberID, BYTE cbDeleteKind)
{
	CMD_CP_C_DeleteMember DeleteMember = {};

	DeleteMember.dwClubID = dwClubID;
	DeleteMember.dwMemberID = dwMemberID;
	DeleteMember.cbDeleteKind = cbDeleteKind;

	//发送消息
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_DELETE_MEMBER, &DeleteMember, sizeof(DeleteMember));

	CString strMessage;
	strMessage.Format(TEXT("删除茶社成员[%d]"), dwMemberID);
	m_pIModuleItemSink->InsertString(strMessage, RGB(128, 128, 128));
}

//更新成员
VOID CModuleClubPlaza::UpdateClubMemberType(DWORD dwClubID, DWORD dwMemberID, BYTE cbMemberType)
{
	BYTE cbBuffer[1024];
	auto pUpdateMember = (CMD_CP_C_UpdateMember*)cbBuffer;

	pUpdateMember->dwClubID = dwClubID;
	pUpdateMember->dwMemberID = dwMemberID;
	pUpdateMember->wUpdateKind = UPMEMBER_KIND_TYPE;

	//成员类型
	*(BYTE*)(pUpdateMember + 1) = cbMemberType;
	*(DWORD*)((BYTE*)(pUpdateMember + 1) + 1) = 0xFF;

	//发送消息
	WORD wPacketSize = sizeof(CMD_CP_C_UpdateMember) + sizeof(BYTE)+sizeof(DWORD);
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_UPDATE_MEMBER, cbBuffer, wPacketSize);

	CString strMessage;
	strMessage.Format(TEXT("更新成员类型%d-%d"), dwMemberID, cbMemberType);
	m_pIModuleItemSink->InsertString(strMessage, RGB(128, 128, 128));
}

//更新成员
VOID CModuleClubPlaza::UpdateClubMemberStatus(DWORD dwClubID, DWORD dwMemberID, BYTE cbMemberStatus)
{
	BYTE cbBuffer[1024];
	auto pUpdateMember = (CMD_CP_C_UpdateMember*)cbBuffer;

	pUpdateMember->dwClubID = dwClubID;
	pUpdateMember->dwMemberID = dwMemberID;
	pUpdateMember->wUpdateKind = UPMEMBER_KIND_STATUS;

	//成员类型
	*(BYTE*)(pUpdateMember + 1) = cbMemberStatus;

	//发送消息
	WORD wPacketSize = sizeof(CMD_CP_C_UpdateMember) + sizeof(BYTE);
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_UPDATE_MEMBER, cbBuffer, wPacketSize);

	CString strMessage;
	strMessage.Format(TEXT("更新成员状态%d-%d"), dwMemberID, cbMemberStatus);
	m_pIModuleItemSink->InsertString(strMessage, RGB(128, 128, 128));
}

//响应加入
VOID CModuleClubPlaza::RespondJoinClub(tagClubApplyMessage* pApplyMessage, BYTE cbApplyStatus)
{
	CMD_CP_C_ApplyRespond ApplyRespond = {};

	//设置变量
	ApplyRespond.cbApplyStatus = cbApplyStatus;
	ApplyRespond.dwClubID = pApplyMessage->dwClubID;
	ApplyRespond.dwApplyerID= pApplyMessage->dwApplyerID;
	ApplyRespond.dwMessageID = pApplyMessage->dwMessageID;	

	//发送消息
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_APPLY_RESPOND, &ApplyRespond, sizeof(ApplyRespond));

	m_pIModuleItemSink->InsertString(TEXT("响应加入茶社"), RGB(128, 128, 128));
}

//查询茶社
VOID CModuleClubPlaza::SearchClubItem(DWORD dwClubID)
{
	//构造对象
	CMD_CP_C_SearchClub  SearchClub = {};

	//设置变量
	SearchClub.dwClubID = dwClubID;

	//发送消息
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_SEARCH_CLUB, &SearchClub, sizeof(SearchClub));

	m_pIModuleItemSink->InsertString(TEXT("查询茶社"), RGB(128, 128, 128));
}

//删除茶社
VOID CModuleClubPlaza::DeleteClubItem(DWORD dwClubID)
{
	//构造对象
	CMD_CP_C_DeleteClub  DeleteClub = {};

	//设置变量
	DeleteClub.dwClubID = dwClubID;
	
	//发送消息
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_DELETE_CLUB, &DeleteClub, sizeof(DeleteClub));

	m_pIModuleItemSink->InsertString(TEXT("删除茶社"), RGB(128, 128, 128));
}

//更新茶社
VOID CModuleClubPlaza::UpdateClubItem(DWORD dwClubID)
{
	//缓存定义
	BYTE cbBuffer[512];	

	//更新茶社
	auto pUpdateClub = (CMD_CP_C_UpdateClub*)cbBuffer;

	//修改属性
	pUpdateClub->dwClubID = dwClubID;
	pUpdateClub->cbUpdateKind = UPCLUB_KIND_PROPERTY;

	//茶社属性
	auto pClubProperty = (tagClubProperty*)(pUpdateClub+1);

	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();
	
	_sntprintf_s(pClubProperty->szClubName,CountArray(pClubProperty->szClubName),TEXT("%s的茶社-%d"), pGlobalUserData->szAccounts,(DWORD)time(NULL));
	_sntprintf_s(pClubProperty->szClubIntroduce, CountArray(pClubProperty->szClubIntroduce), TEXT("茶社介绍-%d"), (DWORD)time(NULL));

	//发送消息
	WORD wPacketSize = sizeof(CMD_CP_C_UpdateClub)+sizeof(tagClubProperty);
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_UPDATE_CLUB, pUpdateClub, wPacketSize);

	//////////////////////////////////////////////////////////////////////////////////////////////////


	//修改属性
	pUpdateClub->dwClubID = dwClubID;
	pUpdateClub->cbUpdateKind = UPCLUB_KIND_RULES;

	//茶社规则
	auto pClubRules = (tagClubRules*)(pUpdateClub + 1);

	//更新规则
	pClubRules->wSwitchOption = 0xFF;
	pClubRules->cbBattleMask = 0xFF;

	//发送消息
	wPacketSize = sizeof(CMD_CP_C_UpdateClub) + sizeof(tagClubRules);
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_UPDATE_CLUB, pUpdateClub, wPacketSize);

	//////////////////////////////////////////////////////////////////////////////////////////////////

	m_pIModuleItemSink->InsertString(TEXT("修改茶社"), RGB(128, 128, 128));
}

//重置模块
VOID CModuleClubPlaza::ResetModule()
{
	//设置变量
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));	
}
//////////////////////////////////////////////////////////////////////////////////////////////
