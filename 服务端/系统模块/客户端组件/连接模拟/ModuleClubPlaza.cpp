#include "Stdafx.h"
#include "ModuleClub.h"
#include "ModuleClubPlaza.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//ʱ�Ӷ���

#define	IDI_TIME_PULSE				1								//ʱ������

//////////////////////////////////////////////////////////////////////////////////////////////

//���캯��
CModuleClubPlaza::CModuleClubPlaza(IModuleItemSink* pIModuleItemSink):
	m_pIModuleItemSink(pIModuleItemSink)
{
	//����ģ��
	SetServiceModule(SERVICE_MODULE_CLUBPLAZA);

	//���ñ���
	ZeroMemory(&m_ClubOption, sizeof(m_ClubOption));
	ZeroMemory(&m_ClubNameCard, sizeof(m_ClubNameCard));
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));
}

//��������
CModuleClubPlaza::~CModuleClubPlaza()
{
}

//�ر��¼�
bool CModuleClubPlaza::OnEventVirtualShut()
{
	//���ñ���
	ZeroMemory(&m_ClubOption, sizeof(m_ClubOption));
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));

	//Ͷ����Ϣ	
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_CLUBPLAZA_LINK_SHUT, 0, 0);

	//������Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: �����㳡���������������ӶϿ���"), m_pIModuleItemSink->GetGlobalUserData()->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(255, 0, 0));

	return true;
}

//�����¼�
bool CModuleClubPlaza::OnEventVirtualLink(LONG lErrorCode)
{
	//����ʧ��
	if (lErrorCode != 0)
	{
		//Ͷ����Ϣ	
		ASSERT(m_pIModuleItemSink != NULL);
		m_pIModuleItemSink->PostWindowMessage(WM_CLUBPLAZA_LINK_FAILURE, 0, 0);

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
bool CModuleClubPlaza::KillModuleTimer(UINT nTimerID)
{
	//Ͷ����Ϣ
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->KillGameTimer(GetTimerStart() + nTimerID);

	return true;
}

//����ʱ��
bool CModuleClubPlaza::SetModuleTimer(UINT nTimerID, UINT nElapse)
{
	//Ͷ����Ϣ
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->SetGameTimer(GetTimerStart() + nTimerID, nElapse);

	return true;
}

//ʱ���¼�
bool CModuleClubPlaza::OnEventTimer(UINT uTimerID, WPARAM wBindParam)
{
	//ʱ������
	if (uTimerID == IDI_TIME_PULSE)
	{
		//��������
		DWORD dwTimeStamp = (DWORD)time(NULL);

		if (dwTimeStamp % 5 == 0 && !m_bTestFlag)
		{
			//������Ϣ
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

			//��ѯ����
			if (m_ClubNameCard.dwClubID % 3 == 0)
			{
				m_bTestFlag = true;
				SearchClubItem(m_ClubNameCard.dwClubID+1);
			}

			//���²���
			if (m_ClubNameCard.dwClubID % 3 == 1)
			{
				m_bTestFlag = true;
				UpdateClubItem(m_ClubNameCard.dwClubID);
			}

			//ɾ������
			/*if (m_ClubNameCard.dwClubID % 3 == 2)
			{
				m_bTestFlag = true;
				DeleteClubItem(m_ClubNameCard.dwClubID);
			}*/
		}		
	}

	return true;
}

//��ȡ�¼�
bool CModuleClubPlaza::OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_CLUBPLAZA_SERVICE:			//�������
	{
		return OnSocketMainClubPlazaService(Command.wSubCmdID, pData, wDataSize);
	}		
	}

	return true;
}

//��¼����
bool CModuleClubPlaza::OnSocketMainClubPlazaService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CP_S_LOGON_FINISH:		//��¼���
	{
		return OnSocketSubLogonFinish(pData, wDataSize);
	}
	case SUB_CP_S_LOGON_SUCCESS:	//��¼�ɹ�
	{
		return OnSocketSubLogonSuccess(pData,wDataSize);
	}
	case SUB_CP_S_LOGON_FAILURE:	//��¼ʧ��
	{
		return OnSocketSubLogonFailure(pData, wDataSize);
	}
	case SUB_CP_S_CLUB_OPTION:		//��������
	{
		return OnSocketSubClubOption(pData, wDataSize);
	}
	case SUB_CP_S_CLUB_NAMECARD:	//������Ƭ
	{
		return OnSocketSubClubNameCard(pData, wDataSize);
	}
	case SUB_CP_S_APPLY_MESSAGE:	//������Ϣ
	{
		return OnSocketSubClubMessage(pData, wDataSize);
	}
	case SUB_CP_S_SEARCH_RESULT:	//��ѯ���
	{
		return OnSocketSubSearchResult(pData, wDataSize);
	}
	case SUB_CP_S_OPERATE_SUCCESS:	//�����ɹ�
	{
		return OnSocketSubOperateSuccess(pData, wDataSize);
	}
	case SUB_CP_S_OPERATE_FAILURE:	//����ʧ��
	{
		return OnSocketSubOperateFailure(pData, wDataSize);
	}
	}

	return true;
}

//����ʧ��
bool CModuleClubPlaza::OnSocketSubLogonFailure(VOID* pData, WORD wDataSize)
{
	auto pLogonFailure = (CMD_CP_S_LogonFailure*)pData;

	//��ӡ��Ϣ
	CString strMessage;
	m_pIModuleItemSink->InsertString(pLogonFailure->szErrorDescribe, RGB(128, 128, 128));

	//Ͷ����Ϣ		
	m_pIModuleItemSink->PostWindowMessage(WM_CLUBPLAZA_LOGON_FAILURE, 0, 0);

	return true;
}

//��ѯ���
bool CModuleClubPlaza::OnSocketSubSearchResult(VOID* pData, WORD wDataSize)
{
	auto pSearchResult = (CMD_CP_S_SearchResult*)pData;

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("��ѯ�����ֲ�[%d][%s]"), pSearchResult->ClubNameCard.dwClubID, pSearchResult->ClubNameCard.szClubName);
	m_pIModuleItemSink->InsertString(strMessage, RGB(128, 128, 128));

	//�������
	RequestJoinClub(pSearchResult->ClubNameCard.dwClubID);

	return true;
}

//�����ɹ�
bool CModuleClubPlaza::OnSocketSubOperateSuccess(VOID* pData, WORD wDataSize)
{
	auto pOperateSuccess = (CMD_CP_S_OperateSuccess*)pData;

	//��ӡ��Ϣ
	if (pOperateSuccess->szOperateNotice[0] != 0)
	{
		CString strMessage;
		m_pIModuleItemSink->InsertString(pOperateSuccess->szOperateNotice, RGB(128, 128, 128));
	}

	return true;
}

//����ʧ��
bool CModuleClubPlaza::OnSocketSubOperateFailure(VOID* pData, WORD wDataSize)
{
	auto pOperateFailure = (CMD_CP_S_OperateFailure*)pData;

	//��ӡ��Ϣ
	CString strMessage;
	m_pIModuleItemSink->InsertString(pOperateFailure->szErrorDescribe, RGB(128, 128, 128));

	return true;
}

//����ɹ�
bool CModuleClubPlaza::OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize)
{
	return true;
}

//��������
bool CModuleClubPlaza::OnSocketSubClubOption(VOID* pData, WORD wDataSize)
{
	m_ClubOption = *(tagClubOption*)pData;

	return true;
}

//������Ƭ
bool CModuleClubPlaza::OnSocketSubClubNameCard(VOID* pData, WORD wDataSize)
{
	//У�����
	ASSERT(wDataSize%sizeof(tagClubNameCard)==0);
	if (wDataSize % sizeof(tagClubNameCard) != 0) return false;

	//������Ƭ
	auto pClubNameCard = (tagClubNameCard*)pData;

	//������Ƭ
	CopyMemory(&m_ClubNameCard, pClubNameCard, sizeof(m_ClubNameCard));	

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("�յ�����[%d]��Ƭ��Ϣ"),m_ClubNameCard.dwClubID);
	m_pIModuleItemSink->InsertString(strMessage, RGB(128, 128, 128));

	return true;
}

//������Ϣ
bool CModuleClubPlaza::OnSocketSubClubMessage(VOID* pData, WORD wDataSize)
{
	auto pClubMessage = (CMD_CP_S_ApplyMessage*)pData;

	//��������
	auto pMsgItem = (tagClubApplyMessage*)(pClubMessage + 1);
	auto nMsgCount = (wDataSize - sizeof(CMD_CP_S_ApplyMessage)) / sizeof(tagClubApplyMessage);

	for (int i = 0; i < nMsgCount; i++)
	{
		m_ArrayApplyMessage.Add(*pMsgItem++);
	}

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("�յ�����[%d]������Ϣ"), m_ClubNameCard.dwClubID);
	m_pIModuleItemSink->InsertString(strMessage, RGB(128, 128, 128));

	return true;
}

//�������
bool CModuleClubPlaza::OnSocketSubLogonFinish(VOID* pData, WORD wDataSize)
{
	//���ö�ʱ��
	SetModuleTimer(IDI_TIME_PULSE, 1000);

	//�û��ӿ�
	ASSERT(m_pIModuleItemSink != NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ����㳡��¼���..."), pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	//���ñ�־
	m_bTestFlag = false;	

	//У�����
	if (m_ClubNameCard.dwClubID != 0)
	{
		//����ģ��
		auto pModuleClub = (CModuleClub*)m_pIModuleItemSink->GetModuleClub();

		//�������
		pModuleClub->EnterService(m_ClubNameCard.dwClubID);				
	}	
	else
	{
		//��������
		CreateClubItem();
	}

	return true;
}

//�������
bool CModuleClubPlaza::EnterService()
{
	//�û��ӿ�
	ASSERT(m_pIModuleItemSink != NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//У��״̬
	if (GetLinkStatus() == true)
	{
		SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_ENTER_SERVICE, NULL, 0);
	}
	else
	{
		//��������
		m_BufferPacket.wDataSize = 0;
		m_BufferPacket.wMainCmdID = MDM_CLUBPLAZA_SERVICE;
		m_BufferPacket.wSubCmdID = SUB_CP_C_ENTER_SERVICE;		

		//��������
		CreateVirtualLink(INVALID_WORD, pGlobalUserData->dwUserID, pGlobalUserData->szLogonToken, NULL, 0);
	}

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ���ڵ�¼����㳡..."), pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

//��������
bool CModuleClubPlaza::CreateClubItem()
{
	//�������
	CMD_CP_C_CreateClub  CreateClub = {};

	//��������
	CreateClub.wClubKind = rand() % CLUB_KIND_COUNT;
	CreateClub.cbPayType = m_ClubOption.ClubKindOption[CreateClub.wClubKind].cbPayTypeMask;
	CreateClub.cbCurrencyKind = m_ClubOption.ClubKindOption[CreateClub.wClubKind].cbCurrencyKindMask;
	
	//���ֲ�
	_sntprintf_s(CreateClub.szClubName,CountArray(CreateClub.szClubName),TEXT("%s�Ĳ���-%d-%04d"), m_pIModuleItemSink->GetGlobalUserData()->szAccounts, CreateClub.wClubKind,rand()%1000);
	_sntprintf_s(CreateClub.szClubIntroduce, CountArray(CreateClub.szClubIntroduce), TEXT("�������"));

	//������Ϣ
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_CREATE_CLUB, &CreateClub, sizeof(CreateClub));

	return true;
}

//�������
VOID CModuleClubPlaza::RequestJoinClub(DWORD dwClubID)
{
	CMD_CP_C_ApplyRequest ApplyRequest = {};

	ApplyRequest.dwClubID = dwClubID;

	//������Ϣ
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_APPLY_REQUEST, &ApplyRequest, sizeof(ApplyRequest));

	m_pIModuleItemSink->InsertString(TEXT("����������"), RGB(128, 128, 128));
}

//ɾ����Ϣ
VOID CModuleClubPlaza::DeleteClubMessage(DWORD dwMessageID, BYTE cbMessageType)
{
	CMD_CP_C_ApplyDelete ApplyDelete = {};

	ApplyDelete.dwMessageID = dwMessageID;
	ApplyDelete.cbMessageType = cbMessageType;

	//������Ϣ
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_APPLY_DELETE, &ApplyDelete, sizeof(ApplyDelete));

	m_pIModuleItemSink->InsertString(TEXT("ɾ����������"), RGB(128, 128, 128));
}

//ɾ����Ա
VOID CModuleClubPlaza::DeleteClubMember(DWORD dwClubID, DWORD dwMemberID, BYTE cbDeleteKind)
{
	CMD_CP_C_DeleteMember DeleteMember = {};

	DeleteMember.dwClubID = dwClubID;
	DeleteMember.dwMemberID = dwMemberID;
	DeleteMember.cbDeleteKind = cbDeleteKind;

	//������Ϣ
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_DELETE_MEMBER, &DeleteMember, sizeof(DeleteMember));

	CString strMessage;
	strMessage.Format(TEXT("ɾ�������Ա[%d]"), dwMemberID);
	m_pIModuleItemSink->InsertString(strMessage, RGB(128, 128, 128));
}

//���³�Ա
VOID CModuleClubPlaza::UpdateClubMemberType(DWORD dwClubID, DWORD dwMemberID, BYTE cbMemberType)
{
	BYTE cbBuffer[1024];
	auto pUpdateMember = (CMD_CP_C_UpdateMember*)cbBuffer;

	pUpdateMember->dwClubID = dwClubID;
	pUpdateMember->dwMemberID = dwMemberID;
	pUpdateMember->wUpdateKind = UPMEMBER_KIND_TYPE;

	//��Ա����
	*(BYTE*)(pUpdateMember + 1) = cbMemberType;
	*(DWORD*)((BYTE*)(pUpdateMember + 1) + 1) = 0xFF;

	//������Ϣ
	WORD wPacketSize = sizeof(CMD_CP_C_UpdateMember) + sizeof(BYTE)+sizeof(DWORD);
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_UPDATE_MEMBER, cbBuffer, wPacketSize);

	CString strMessage;
	strMessage.Format(TEXT("���³�Ա����%d-%d"), dwMemberID, cbMemberType);
	m_pIModuleItemSink->InsertString(strMessage, RGB(128, 128, 128));
}

//���³�Ա
VOID CModuleClubPlaza::UpdateClubMemberStatus(DWORD dwClubID, DWORD dwMemberID, BYTE cbMemberStatus)
{
	BYTE cbBuffer[1024];
	auto pUpdateMember = (CMD_CP_C_UpdateMember*)cbBuffer;

	pUpdateMember->dwClubID = dwClubID;
	pUpdateMember->dwMemberID = dwMemberID;
	pUpdateMember->wUpdateKind = UPMEMBER_KIND_STATUS;

	//��Ա����
	*(BYTE*)(pUpdateMember + 1) = cbMemberStatus;

	//������Ϣ
	WORD wPacketSize = sizeof(CMD_CP_C_UpdateMember) + sizeof(BYTE);
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_UPDATE_MEMBER, cbBuffer, wPacketSize);

	CString strMessage;
	strMessage.Format(TEXT("���³�Ա״̬%d-%d"), dwMemberID, cbMemberStatus);
	m_pIModuleItemSink->InsertString(strMessage, RGB(128, 128, 128));
}

//��Ӧ����
VOID CModuleClubPlaza::RespondJoinClub(tagClubApplyMessage* pApplyMessage, BYTE cbApplyStatus)
{
	CMD_CP_C_ApplyRespond ApplyRespond = {};

	//���ñ���
	ApplyRespond.cbApplyStatus = cbApplyStatus;
	ApplyRespond.dwClubID = pApplyMessage->dwClubID;
	ApplyRespond.dwApplyerID= pApplyMessage->dwApplyerID;
	ApplyRespond.dwMessageID = pApplyMessage->dwMessageID;	

	//������Ϣ
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_APPLY_RESPOND, &ApplyRespond, sizeof(ApplyRespond));

	m_pIModuleItemSink->InsertString(TEXT("��Ӧ�������"), RGB(128, 128, 128));
}

//��ѯ����
VOID CModuleClubPlaza::SearchClubItem(DWORD dwClubID)
{
	//�������
	CMD_CP_C_SearchClub  SearchClub = {};

	//���ñ���
	SearchClub.dwClubID = dwClubID;

	//������Ϣ
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_SEARCH_CLUB, &SearchClub, sizeof(SearchClub));

	m_pIModuleItemSink->InsertString(TEXT("��ѯ����"), RGB(128, 128, 128));
}

//ɾ������
VOID CModuleClubPlaza::DeleteClubItem(DWORD dwClubID)
{
	//�������
	CMD_CP_C_DeleteClub  DeleteClub = {};

	//���ñ���
	DeleteClub.dwClubID = dwClubID;
	
	//������Ϣ
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_DELETE_CLUB, &DeleteClub, sizeof(DeleteClub));

	m_pIModuleItemSink->InsertString(TEXT("ɾ������"), RGB(128, 128, 128));
}

//���²���
VOID CModuleClubPlaza::UpdateClubItem(DWORD dwClubID)
{
	//���涨��
	BYTE cbBuffer[512];	

	//���²���
	auto pUpdateClub = (CMD_CP_C_UpdateClub*)cbBuffer;

	//�޸�����
	pUpdateClub->dwClubID = dwClubID;
	pUpdateClub->cbUpdateKind = UPCLUB_KIND_PROPERTY;

	//��������
	auto pClubProperty = (tagClubProperty*)(pUpdateClub+1);

	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();
	
	_sntprintf_s(pClubProperty->szClubName,CountArray(pClubProperty->szClubName),TEXT("%s�Ĳ���-%d"), pGlobalUserData->szAccounts,(DWORD)time(NULL));
	_sntprintf_s(pClubProperty->szClubIntroduce, CountArray(pClubProperty->szClubIntroduce), TEXT("�������-%d"), (DWORD)time(NULL));

	//������Ϣ
	WORD wPacketSize = sizeof(CMD_CP_C_UpdateClub)+sizeof(tagClubProperty);
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_UPDATE_CLUB, pUpdateClub, wPacketSize);

	//////////////////////////////////////////////////////////////////////////////////////////////////


	//�޸�����
	pUpdateClub->dwClubID = dwClubID;
	pUpdateClub->cbUpdateKind = UPCLUB_KIND_RULES;

	//�������
	auto pClubRules = (tagClubRules*)(pUpdateClub + 1);

	//���¹���
	pClubRules->wSwitchOption = 0xFF;
	pClubRules->cbBattleMask = 0xFF;

	//������Ϣ
	wPacketSize = sizeof(CMD_CP_C_UpdateClub) + sizeof(tagClubRules);
	SendVirtualData(MDM_CLUBPLAZA_SERVICE, SUB_CP_C_UPDATE_CLUB, pUpdateClub, wPacketSize);

	//////////////////////////////////////////////////////////////////////////////////////////////////

	m_pIModuleItemSink->InsertString(TEXT("�޸Ĳ���"), RGB(128, 128, 128));
}

//����ģ��
VOID CModuleClubPlaza::ResetModule()
{
	//���ñ���
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));	
}
//////////////////////////////////////////////////////////////////////////////////////////////
