#include "Stdafx.h"
#include "ModuleClub.h"
#include "ModuleClubPlaza.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//时钟定义

#define	IDI_TIME_PULSE				1								//时间脉冲

//////////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CModuleClub::CModuleClub(IModuleItemSink* pIModuleItemSink):
	m_pIModuleItemSink(pIModuleItemSink)
{
	//设置模块
	SetServiceModule(SERVICE_MODULE_CLUB);
}

//析构函数
CModuleClub::~CModuleClub()
{
}

//关闭事件
bool CModuleClub::OnEventVirtualShut()
{
	//设置变量
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));

	//投递消息	
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_CLUB_LINK_SHUT, 0, 0);

	//构造消息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 与茶社服务器的虚拟连接断开了"), m_pIModuleItemSink->GetGlobalUserData()->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(255, 0, 0));

	return true;
}

//连接事件
bool CModuleClub::OnEventVirtualLink(LONG lErrorCode)
{
	//连接失败
	if (lErrorCode != 0)
	{
		//投递消息	
		ASSERT(m_pIModuleItemSink != NULL);
		m_pIModuleItemSink->PostWindowMessage(WM_CLUB_LINK_FAILURE, 0, 0);

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
bool CModuleClub::KillModuleTimer(UINT nTimerID)
{
	//投递消息
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->KillGameTimer(GetTimerStart() + nTimerID);

	return true;
}

//设置时间
bool CModuleClub::SetModuleTimer(UINT nTimerID, UINT nElapse)
{
	//投递消息
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->SetGameTimer(GetTimerStart() + nTimerID, nElapse);

	return true;
}

//时钟事件
bool CModuleClub::OnEventTimer(UINT uTimerID, WPARAM wBindParam)
{
	//时钟脉冲
	if (uTimerID == IDI_TIME_PULSE)
	{
		//变量定义
		DWORD dwTimeStamp = (DWORD)time(NULL);
		
		if (dwTimeStamp % 5 == 0)
		{
			//处理消息
			if (m_ArrayApplyMessage.GetCount() > 0)
			{
				auto pModuleClubPlaza =(CModuleClubPlaza*)m_pIModuleItemSink->GetModuleClubPlaza();

				for (int i = 0; i < m_ArrayApplyMessage.GetCount(); i++)
				{
					if (m_ArrayApplyMessage[i].cbMessageStatus == APPLY_STATUS_NONE)
					{
						pModuleClubPlaza->RespondJoinClub(&m_ArrayApplyMessage[i], APPLY_STATUS_AGREE);
					}									
				}

				m_ArrayApplyMessage.RemoveAll();
			}
		}
	}

	return true;
}

//读取事件
bool CModuleClub::OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_CLUB_SERVICE:			//茶社服务
	{
		return OnSocketMainClubService(Command.wSubCmdID, pData, wDataSize);
	}		
	}

	return true;
}

//登录处理
bool CModuleClub::OnSocketMainClubService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CB_S_ENTER_SUCCESS:	//进入成功
	{
		return OnSocketSubEnterSuccess(pData,wDataSize);
	}
	case SUB_CB_S_ENTER_FAILURE:	//进入失败
	{
		return OnSocketSubEnterFailure(pData, wDataSize);
	}	
	case SUB_CB_S_CLUB_ITEM:		//茶社子项
	{
		return OnSocketSubClubItem(pData, wDataSize);
	}
	case SUB_CB_S_CLUB_MEMBER:		//茶社成员
	{
		return OnSocketSubClubMember(pData, wDataSize);
	}
	//case SUB_CB_S_CLUB_MESSAGE:		//茶社消息
	//{
	//	return OnSocketSubClubMessage(pData, wDataSize);
	//}
	}

	return true;
}

//茶社子项
bool CModuleClub::OnSocketSubClubItem(VOID* pData, WORD wDataSize)
{
	//茶社子项
	m_ClubItem = *(tagClubItem*)pData;

	m_pIModuleItemSink->InsertString(TEXT("收到茶社信息"), RGB(128, 128, 128));

	return true;
}

//茶社成员
bool CModuleClub::OnSocketSubClubMember(VOID* pData, WORD wDataSize)
{
	auto pClubMember = (CMD_CB_S_ClubMember*)pData;
	auto pMemberItem = (tagClubMemberUser*)(pClubMember + 1);
	auto nMemberCount = (wDataSize - sizeof(CMD_CB_S_ClubMember)) / sizeof(tagClubMemberUser);

	auto pModuleClubPlaza = (CModuleClubPlaza*)m_pIModuleItemSink->GetModuleClubPlaza();

	for (int i = 0; i < nMemberCount; i++)
	{
		if (pMemberItem->MemberInfo.cbMemberType == MEMBER_TYPE_MEMBER)
		{
			pModuleClubPlaza->UpdateClubMemberType(pClubMember->dwClubID, pMemberItem->MemberInfo.dwMemberID, MEMBER_TYPE_MANAGER);
			pModuleClubPlaza->UpdateClubMemberStatus(pClubMember->dwClubID, pMemberItem->MemberInfo.dwMemberID, MEMBER_STATUS_FREEZE);
		}

		if (pMemberItem->MemberInfo.cbMemberType == MEMBER_TYPE_MANAGER)
		{
			pModuleClubPlaza->UpdateClubMemberType(pClubMember->dwClubID, pMemberItem->MemberInfo.dwMemberID, MEMBER_TYPE_MEMBER);
			pModuleClubPlaza->UpdateClubMemberStatus(pClubMember->dwClubID, pMemberItem->MemberInfo.dwMemberID, MEMBER_STATUS_NORMAL);
		}

		pMemberItem++;
	}

	m_pIModuleItemSink->InsertString(TEXT("收到茶社成员"), RGB(128, 128, 128));

	return true;
}

//茶社消息
bool CModuleClub::OnSocketSubClubMessage(VOID* pData, WORD wDataSize)
{
	//auto pClubMessage = (CMD_CB_S_ClubMessage*)pData;

	////计算数量
	//auto pMsgItem = (tagClubApplyMessage*)(pClubMessage + 1);
	//auto nMsgCount = (wDataSize - sizeof(CMD_CP_S_ApplyMessage)) / sizeof(tagClubApplyMessage);

	//for (int i = 0; i < nMsgCount; i++)
	//{
	//	m_ArrayApplyMessage.Add(*pMsgItem++);
	//}

	//m_pIModuleItemSink->InsertString(TEXT("收到茶社消息"), RGB(128, 128, 128));

	return true;
}

//进入失败
bool CModuleClub::OnSocketSubEnterFailure(VOID* pData, WORD wDataSize)
{
	auto pEnterFailure = (CMD_CB_S_EnterFailure*)pData;

	//打印信息	
	m_pIModuleItemSink->InsertString(pEnterFailure->szErrorDescribe, RGB(128, 128, 128));

	return true;
}

//进入成功
bool CModuleClub::OnSocketSubEnterSuccess(VOID* pData, WORD wDataSize)
{
	//设置定时器
	SetModuleTimer(IDI_TIME_PULSE, 1000);

	//用户接口
	ASSERT(m_pIModuleItemSink != NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 茶社服务器[%d]进入成功..."), pGlobalUserData->szAccounts, m_ClubItem.ClubProperty.dwClubID);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));	

	return true;
}

//进入服务
bool CModuleClub::EnterService(DWORD dwClubID)
{
	//用户接口
	ASSERT(m_pIModuleItemSink != NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//变量定义
	CMD_CB_C_EnterClub EnterClub = {};

	//茶社标识
	EnterClub.dwClubID = dwClubID;

	//校验状态
	if (GetLinkStatus() == true)
	{
		SendVirtualData(MDM_CLUB_SERVICE, SUB_CB_C_ENTER_CLUB, &EnterClub, sizeof(EnterClub));
	}
	else
	{
		//缓冲数据
		m_BufferPacket.wMainCmdID = MDM_CLUB_SERVICE;
		m_BufferPacket.wSubCmdID = SUB_CB_C_ENTER_CLUB;
		m_BufferPacket.wDataSize = sizeof(EnterClub);
		CopyMemory(m_BufferPacket.cbDataBuffer,&EnterClub,sizeof(EnterClub));

		//扩展信息
		tagLinkExtend_Route LinkExtend;
		ZeroMemory(&LinkExtend, sizeof(LinkExtend));

		//设置变量
		LinkExtend.dwRouteKeyID = dwClubID;

		//创建连接
		CreateVirtualLink(INVALID_WORD, pGlobalUserData->dwUserID, pGlobalUserData->szLogonToken, (LPBYTE)&LinkExtend, sizeof(LinkExtend));
	}

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 正在登录茶社服务器..."), pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

//重置模块
VOID CModuleClub::ResetModule()
{
	//设置变量
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));	
}
//////////////////////////////////////////////////////////////////////////////////////////////
