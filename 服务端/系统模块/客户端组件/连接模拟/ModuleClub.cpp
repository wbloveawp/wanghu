#include "Stdafx.h"
#include "ModuleClub.h"
#include "ModuleClubPlaza.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//ʱ�Ӷ���

#define	IDI_TIME_PULSE				1								//ʱ������

//////////////////////////////////////////////////////////////////////////////////////////////

//���캯��
CModuleClub::CModuleClub(IModuleItemSink* pIModuleItemSink):
	m_pIModuleItemSink(pIModuleItemSink)
{
	//����ģ��
	SetServiceModule(SERVICE_MODULE_CLUB);
}

//��������
CModuleClub::~CModuleClub()
{
}

//�ر��¼�
bool CModuleClub::OnEventVirtualShut()
{
	//���ñ���
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));

	//Ͷ����Ϣ	
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_CLUB_LINK_SHUT, 0, 0);

	//������Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: �������������������ӶϿ���"), m_pIModuleItemSink->GetGlobalUserData()->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(255, 0, 0));

	return true;
}

//�����¼�
bool CModuleClub::OnEventVirtualLink(LONG lErrorCode)
{
	//����ʧ��
	if (lErrorCode != 0)
	{
		//Ͷ����Ϣ	
		ASSERT(m_pIModuleItemSink != NULL);
		m_pIModuleItemSink->PostWindowMessage(WM_CLUB_LINK_FAILURE, 0, 0);

		return true;
	}

	//��������
	if (m_BufferPacket.wMainCmdID != 0 && m_BufferPacket.wSubCmdID != 0)
	{
		SendVirtualData(m_BufferPacket.wMainCmdID, m_BufferPacket.wSubCmdID, m_BufferPacket.cbDataBuffer, m_BufferPacket.wDataSize);
	}

	return true;
}

//ɾ��ʱ��
bool CModuleClub::KillModuleTimer(UINT nTimerID)
{
	//Ͷ����Ϣ
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->KillGameTimer(GetTimerStart() + nTimerID);

	return true;
}

//����ʱ��
bool CModuleClub::SetModuleTimer(UINT nTimerID, UINT nElapse)
{
	//Ͷ����Ϣ
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->SetGameTimer(GetTimerStart() + nTimerID, nElapse);

	return true;
}

//ʱ���¼�
bool CModuleClub::OnEventTimer(UINT uTimerID, WPARAM wBindParam)
{
	//ʱ������
	if (uTimerID == IDI_TIME_PULSE)
	{
		//��������
		DWORD dwTimeStamp = (DWORD)time(NULL);
		
		if (dwTimeStamp % 5 == 0)
		{
			//������Ϣ
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

//��ȡ�¼�
bool CModuleClub::OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_CLUB_SERVICE:			//�������
	{
		return OnSocketMainClubService(Command.wSubCmdID, pData, wDataSize);
	}		
	}

	return true;
}

//��¼����
bool CModuleClub::OnSocketMainClubService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CB_S_ENTER_SUCCESS:	//����ɹ�
	{
		return OnSocketSubEnterSuccess(pData,wDataSize);
	}
	case SUB_CB_S_ENTER_FAILURE:	//����ʧ��
	{
		return OnSocketSubEnterFailure(pData, wDataSize);
	}	
	case SUB_CB_S_CLUB_ITEM:		//��������
	{
		return OnSocketSubClubItem(pData, wDataSize);
	}
	case SUB_CB_S_CLUB_MEMBER:		//�����Ա
	{
		return OnSocketSubClubMember(pData, wDataSize);
	}
	//case SUB_CB_S_CLUB_MESSAGE:		//������Ϣ
	//{
	//	return OnSocketSubClubMessage(pData, wDataSize);
	//}
	}

	return true;
}

//��������
bool CModuleClub::OnSocketSubClubItem(VOID* pData, WORD wDataSize)
{
	//��������
	m_ClubItem = *(tagClubItem*)pData;

	m_pIModuleItemSink->InsertString(TEXT("�յ�������Ϣ"), RGB(128, 128, 128));

	return true;
}

//�����Ա
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

	m_pIModuleItemSink->InsertString(TEXT("�յ������Ա"), RGB(128, 128, 128));

	return true;
}

//������Ϣ
bool CModuleClub::OnSocketSubClubMessage(VOID* pData, WORD wDataSize)
{
	//auto pClubMessage = (CMD_CB_S_ClubMessage*)pData;

	////��������
	//auto pMsgItem = (tagClubApplyMessage*)(pClubMessage + 1);
	//auto nMsgCount = (wDataSize - sizeof(CMD_CP_S_ApplyMessage)) / sizeof(tagClubApplyMessage);

	//for (int i = 0; i < nMsgCount; i++)
	//{
	//	m_ArrayApplyMessage.Add(*pMsgItem++);
	//}

	//m_pIModuleItemSink->InsertString(TEXT("�յ�������Ϣ"), RGB(128, 128, 128));

	return true;
}

//����ʧ��
bool CModuleClub::OnSocketSubEnterFailure(VOID* pData, WORD wDataSize)
{
	auto pEnterFailure = (CMD_CB_S_EnterFailure*)pData;

	//��ӡ��Ϣ	
	m_pIModuleItemSink->InsertString(pEnterFailure->szErrorDescribe, RGB(128, 128, 128));

	return true;
}

//����ɹ�
bool CModuleClub::OnSocketSubEnterSuccess(VOID* pData, WORD wDataSize)
{
	//���ö�ʱ��
	SetModuleTimer(IDI_TIME_PULSE, 1000);

	//�û��ӿ�
	ASSERT(m_pIModuleItemSink != NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ���������[%d]����ɹ�..."), pGlobalUserData->szAccounts, m_ClubItem.ClubProperty.dwClubID);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));	

	return true;
}

//�������
bool CModuleClub::EnterService(DWORD dwClubID)
{
	//�û��ӿ�
	ASSERT(m_pIModuleItemSink != NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//��������
	CMD_CB_C_EnterClub EnterClub = {};

	//�����ʶ
	EnterClub.dwClubID = dwClubID;

	//У��״̬
	if (GetLinkStatus() == true)
	{
		SendVirtualData(MDM_CLUB_SERVICE, SUB_CB_C_ENTER_CLUB, &EnterClub, sizeof(EnterClub));
	}
	else
	{
		//��������
		m_BufferPacket.wMainCmdID = MDM_CLUB_SERVICE;
		m_BufferPacket.wSubCmdID = SUB_CB_C_ENTER_CLUB;
		m_BufferPacket.wDataSize = sizeof(EnterClub);
		CopyMemory(m_BufferPacket.cbDataBuffer,&EnterClub,sizeof(EnterClub));

		//��չ��Ϣ
		tagLinkExtend_Route LinkExtend;
		ZeroMemory(&LinkExtend, sizeof(LinkExtend));

		//���ñ���
		LinkExtend.dwRouteKeyID = dwClubID;

		//��������
		CreateVirtualLink(INVALID_WORD, pGlobalUserData->dwUserID, pGlobalUserData->szLogonToken, (LPBYTE)&LinkExtend, sizeof(LinkExtend));
	}

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ���ڵ�¼���������..."), pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

//����ģ��
VOID CModuleClub::ResetModule()
{
	//���ñ���
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));	
}
//////////////////////////////////////////////////////////////////////////////////////////////
