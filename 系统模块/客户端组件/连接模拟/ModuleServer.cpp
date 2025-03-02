#include "Stdafx.h"
#include "GlobalUnits.h"
#include "ModuleServer.h"
#include "RobotUserManager.h"

//////////////////////////////////////////////////////////////////////////////////////////
//ʱ�Ӷ���

#define	IDI_GAME_PULSE				1								//��Ϸ����

//////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CModuleServer::CModuleServer(IModuleItemSink* pIModuleItemSink) :
	m_pIModuleItemSink(pIModuleItemSink)
{
	//����ģ��
	SetServiceModule(SERVICE_MODULE_GAME);

	//���ñ���
	m_wKindID=0;
	m_wTableCount=0;
	m_wChairCount=0;
	m_wServerType=0;
	m_dwServerRule=0;

	//�ӿڱ���
	m_pRobotUserItem = NULL;

	//�û�����
	m_pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();
	if (m_pGlobalUserData != NULL)
	{
		m_pGlobalUserData->cbUserStatus = US_NULL;
		m_pGlobalUserData->wChairID = INVALID_WORD;
		m_pGlobalUserData->wTableID = INVALID_WORD;
	}

	//���ñ���
	ZeroMemory(&m_BufferPacket,sizeof(m_BufferPacket));	
}

//��������
CModuleServer::~CModuleServer()
{	
}

//�ر��¼�
bool CModuleServer::OnEventVirtualShut()
{
	//���ñ���
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));

	//������־
	bool bStartClient = false;
	if (m_pRobotUserItem != NULL)
	{
		bStartClient = m_pRobotUserItem->IsStartClient();		
	}

	//������Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ����Ϸ������������ӶϿ���"), m_pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(),RGB(255,0,0));

	//Ͷ����Ϣ
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_SERVER_LINK_SHUT, bStartClient, m_wServiceID);

	return true;
}

//�����¼�
bool CModuleServer::OnEventVirtualLink(LONG lErrorCode)
{
	//����ʧ��
	if (lErrorCode != 0)
	{
		//Ͷ����Ϣ
		ASSERT(m_pIModuleItemSink != NULL);
		m_pIModuleItemSink->PostWindowMessage(WM_SERVER_LINK_FAILURE, 0, 0);

		return true;
	}

	//��������
	if (m_BufferPacket.wMainCmdID != 0 && m_BufferPacket.wSubCmdID != 0)
	{
		SendVirtualData(m_BufferPacket.wMainCmdID, m_BufferPacket.wSubCmdID, m_BufferPacket.cbDataBuffer, m_BufferPacket.wDataSize);
	}

	return true;
}

//��ȡ�¼�
bool CModuleServer::OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GR_LOGON:			//��¼����
	{
		return OnSocketMainLogon(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_GR_CONFIG:			//���ô���
	{
		return OnSocketMainConfig(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_GR_USER:			//�û�����
	{
		return OnSocketMainUser(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_GR_STATUS:			//״̬����
	{
		return OnSocketMainStatus(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_GR_MATCH:			//��������
	{
		return OnSocketMainMatch(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_CM_SYSTEM:			//ϵͳ����
	{
		return OnSocketMainSystem(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_GF_GAME:			//��Ϸ����
	case MDM_GF_FRAME:			//��ܴ���
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

//ɾ��ʱ��
bool CModuleServer::KillModuleTimer(UINT nTimerID)
{
	//Ͷ����Ϣ
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->KillGameTimer(GetTimerStart() + nTimerID);

	return true;
}

//����ʱ��
bool CModuleServer::SetModuleTimer(UINT nTimerID, UINT nElapse)
{
	//Ͷ����Ϣ
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->SetGameTimer(GetTimerStart() + nTimerID, nElapse);

	return true;
}

//ʱ���¼�
bool CModuleServer::OnEventTimer(UINT uTimerID, WPARAM wBindParam)
{
	//��Ϸ����
	if (uTimerID == IDI_GAME_PULSE)
	{
		if (m_pRobotUserItem != NULL)
		{
			m_pRobotUserItem->OnTimerPulse(uTimerID, wBindParam);
		}		
	}	

	return true;
}

//��¼����
bool CModuleServer::OnSocketMainLogon(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_LOGON_FAILURE:		//��¼ʧ��
	{
		return OnSocketSubLogonFailure(pData, wDataSize);
	}
	case SUB_GR_LOGON_SUCCESS:		//��¼�ɹ�
	{
		return OnSocketSubLogonSuccess(pData, wDataSize);
	}
	case SUB_GR_LOGON_FINISH:		//��¼���
	{
		return OnSocketSubLogonFinish(pData, wDataSize);
	}
	case SUB_GR_UPDATE_NOTIFY:		//������ʾ
	{
		return OnSocketSubUpdateNotify(pData, wDataSize);
	}
	}

	return true;
}

//���ô���
bool CModuleServer::OnSocketMainConfig(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_CONFIG_SERVER:		//���÷���
	{
		return OnSocketSubConfigServer(pData, wDataSize);
	}
	case SUB_GR_CONFIG_RULE:		//���ù���
	{
		return OnSocketSubConfigRule(pData, wDataSize);
	}
	}

	return true;
}

//�û�����
bool CModuleServer::OnSocketMainUser(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_USER_ENTER:			//�û�����
	{
		return OnSocketSubUserEnter(pData, wDataSize);
	}
	case SUB_GR_USER_SCORE:			//�û�����
	{
		return OnSocketSubUserScore(pData, wDataSize);
	}
	case SUB_GR_USER_STATUS:		//�û�״̬
	{
		return OnSocketSubUserStatus(pData, wDataSize);
	}
	case SUB_GR_REQUEST_FAILURE:	//����ʧ��
	{
		return OnSocketSubRequestFailure(pData, wDataSize);
	}
	}

	return true;
}

//״̬����
bool CModuleServer::OnSocketMainStatus(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	return true;
}

//ϵͳ����
bool CModuleServer::OnSocketMainSystem(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CM_SYSTEM_MESSAGE:		//ϵͳ��Ϣ
	{
		return OnSocketSubSystemMessage(pData,wDataSize);
	}	 
	}

	return true;
}

//��������
bool CModuleServer::OnSocketMainMatch(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_MATCH_STATUS:		//����״̬
	{
		return OnSocketSubMatchStatus(pData,wDataSize);
	}
	case SUB_GR_MATCH_USTATUS:		//�û�״̬
	{
		return OnSocketSubMatchUStatus(pData, wDataSize);
	}
	case SUB_GR_SIGNUP_SUCCESS:		//�����ɹ�
	{
		return OnSocketSubSignupSuccess(pData, wDataSize);
	}
	}

	return true;
}

//��¼ʧ��
bool CModuleServer::OnSocketSubLogonFailure(VOID* pData, WORD wDataSize)
{
	//��ȡ����
	CMD_GR_LogonFailure* pLogonFailure = (CMD_GR_LogonFailure*)pData;

	//������Ϣ
	CString strMessage;
	strMessage.Format(TEXT("��¼ʧ��, Accounts=%s, ErrorMsg=%s, ErrorCode=%d"), m_pGlobalUserData->szAccounts, pLogonFailure->szDescribeString, pLogonFailure->lErrorCode);

	//������Ϣ
	m_pIModuleItemSink->InsertString(strMessage.GetString(),RGB(255,0,0));

	//Ͷ����Ϣ
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_SERVER_LOGON_FAILURE, 0, 0);

	return true;
}

//��¼�ɹ�
bool CModuleServer::OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize)
{
	return true;
}

//��¼���
bool CModuleServer::OnSocketSubLogonFinish(VOID* pData, WORD wDataSize)
{
	//�������
	CMD_GR_UserRule UserRule;
	ZeroMemory(&UserRule, sizeof(UserRule));

	//���͹���
	SendVirtualData(MDM_GR_USER, SUB_GR_USER_RULE, &UserRule, sizeof(UserRule));

	//��������
	if (m_pRobotUserItem == NULL)
	{
		m_pRobotUserItem = g_RobotUserManager->ActiveRobotUserItem();
		if (m_pRobotUserItem ==NULL)
		{
			//�ر�����
			CloseVirtualLink();

			return true;
		}	

		//���ýӿ�
		m_pRobotUserItem->SetVirtualLinkItem(this);
		m_pRobotUserItem->SetGlobalUserData(m_pGlobalUserData);
	}

	//�����ж�
	if (m_pRobotUserItem->IsStartClient()==false && m_pGlobalUserData->wTableID != INVALID_TABLE)
	{
		m_pRobotUserItem->StartGameClient();

		//��ӡ��Ϣ
		CString strMessage;
		strMessage.Format(TEXT("Accounts=%s: ������Ϸ��"), m_pGlobalUserData->szAccounts);
		m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));
	}

	//Ͷ����Ϣ
	m_pIModuleItemSink->PostWindowMessage(WM_SERVER_LOGON_FINISH, 0, 0);

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: �����¼�ɹ�..."), m_pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	//������ʱ��
	SetModuleTimer(IDI_GAME_PULSE, 1000);

	return true;
}

//������ʾ
bool CModuleServer::OnSocketSubUpdateNotify(VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pUpdateNotify = (CMD_GR_UpdateNotify *) pData;

	//Ͷ����Ϣ
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_SERVER_LOGON_FAILURE, 0, 0);

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: �����¼ʧ�ܣ��汾��һ�¡�PlazaVersion=%d,ClientVersion=%d,"), m_pGlobalUserData->szAccounts, pUpdateNotify->dwCurrentPlazaVersion,
		pUpdateNotify->dwCurrentClientVersion);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

//���ù���
bool CModuleServer::OnSocketSubConfigRule(VOID* pData, WORD wDataSize)
{
	return true;
}

//���÷���
bool CModuleServer::OnSocketSubConfigServer(VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pConfigServer = (CMD_GR_ConfigServer*)pData;

	//���ñ���
	m_wKindID = pConfigServer->wKindID;
	m_wChairCount = pConfigServer->wChairCount;
	m_wTableCount = pConfigServer->wTableCount;
	m_wServerType = pConfigServer->wServerType;
	m_dwServerRule = pConfigServer->dwServerRule;

	return true;
}

//�û�����
bool CModuleServer::OnSocketSubUserEnter(VOID* pData, WORD wDataSize)
{
	//��ȡ�û�
	auto pUserInfoHead = (tagMBUserInfoHead*)pData;

	//У���û�
	if (pUserInfoHead->dwUserID == m_pGlobalUserData->dwUserID)
	{
		//����״̬
		m_pGlobalUserData->wTableID = pUserInfoHead->wTableID;
		m_pGlobalUserData->wChairID = pUserInfoHead->wChairID;
		m_pGlobalUserData->cbUserStatus = pUserInfoHead->cbUserStatus;

		//���û���
		m_pGlobalUserData->lUserGold = pUserInfoHead->lGold;
		m_pGlobalUserData->lGameScore = pUserInfoHead->lScore;
	}

	return true;
}

//�û�����
bool CModuleServer::OnSocketSubUserScore(VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pUserScore = (CMD_GR_UserScore*)pData;

	//���·���
	m_pGlobalUserData->lGameScore = pUserScore->UserScore.lScore;

	return true;
}

//�û�״̬
bool CModuleServer::OnSocketSubUserStatus(VOID* pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GR_UserStatus));
	if (wDataSize < sizeof(CMD_GR_UserStatus)) return false;

	//��������
	CMD_GR_UserStatus* pUserStatus = (CMD_GR_UserStatus*)pData;

	//����ж�
	if (pUserStatus->dwUserID == m_pGlobalUserData->dwUserID)
	{
		//�û�״̬
		tagUserStatus LastUserStatus;
		LastUserStatus.wChairID = m_pGlobalUserData->wChairID;
		LastUserStatus.wTableID = m_pGlobalUserData->wTableID;
		LastUserStatus.cbUserStatus = m_pGlobalUserData->cbUserStatus;

		//����״̬
		m_pGlobalUserData->wChairID = pUserStatus->UserStatus.wChairID;
		m_pGlobalUserData->wTableID = pUserStatus->UserStatus.wTableID;
		m_pGlobalUserData->cbUserStatus = pUserStatus->UserStatus.cbUserStatus;

		//У�����
		if (m_pRobotUserItem != NULL)
		{
			//�ر��ж�
			if ((m_pRobotUserItem->IsStartClient() == true) && (m_pGlobalUserData->wTableID == INVALID_TABLE))
			{
				//�ر���Ϸ
				m_pRobotUserItem->CloseGameClient();

				//��ӡ��Ϣ
				CString strMessage;
				strMessage.Format(TEXT("Accounts=%s: �ر���Ϸ��"), m_pGlobalUserData->szAccounts);
				m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

				return true;
			}

			//�����ж�
			if ((m_pRobotUserItem->IsStartClient() == false) && (m_pGlobalUserData->wTableID != INVALID_TABLE))
			{
				//������Ϸ
				m_pRobotUserItem->StartGameClient();

				//��ӡ��Ϣ
				CString strMessage;
				strMessage.Format(TEXT("Accounts=%s: ������Ϸ��"), m_pGlobalUserData->szAccounts);
				m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

				return true;
			}
		}		
	}

	return true;
}

//ϵͳ��Ϣ
bool CModuleServer::OnSocketSubSystemMessage(VOID* pData, WORD wDataSize)
{
	//��������
	CMD_CM_SystemMessage* pSystemMessage = (CMD_CM_SystemMessage*)pData;
	WORD wHeadSize = sizeof(CMD_CM_SystemMessage) - sizeof(pSystemMessage->szString);

	//Ч�����
	ASSERT((wDataSize > wHeadSize) && (wDataSize == (wHeadSize + pSystemMessage->wLength * sizeof(TCHAR))));
	if ((wDataSize <= wHeadSize) || (wDataSize != (wHeadSize + pSystemMessage->wLength * sizeof(TCHAR)))) return false;

	//�رմ���
	if ((pSystemMessage->wType & (SMT_CLOSE_ROOM | SMT_CLOSE_LINK)) != 0)
	{
		//�ر�����
		CloseVirtualLink();
	}

	return true;
}

//����ʧ��
bool CModuleServer::OnSocketSubRequestFailure(VOID* pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize <= sizeof(CMD_GR_RequestFailure));
	if (wDataSize > sizeof(CMD_GR_RequestFailure)) return false;

	//��������
	CMD_GR_RequestFailure* pRequestFailure = (CMD_GR_RequestFailure*)pData;

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("�û���%s��-- %s"), m_pGlobalUserData->szAccounts, pRequestFailure->szDescribeString);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	//�ر�����
	CloseVirtualLink();

	return true;
}

//����״̬
bool CModuleServer::OnSocketSubMatchStatus(VOID* pData, WORD wDataSize)
{
	return true;
}

//����״̬
bool CModuleServer::OnSocketSubMatchUStatus(VOID* pData, WORD wDataSize)
{
	auto cbMatchUserStatus = *(BYTE*)pData;

	//У��״̬
	if (cbMatchUserStatus == MUS_NULL)
	{
		//����ṹ
		CMD_GR_MatchSignup MatchSignup;
		ZeroMemory(&MatchSignup,sizeof(MatchSignup));

		//���ñ���
		MatchSignup.lMatchNo = m_lMatchNo;
		MatchSignup.dwMatchID = m_dwMatchID;

		//��������
		SendVirtualData(MDM_GR_MATCH, SUB_GR_MATCH_SIGNUP, &MatchSignup, sizeof(MatchSignup));

		//��ӡ��Ϣ
		CString strMessage;
		strMessage.Format(TEXT("Accounts=%s: ���ڱ�������..."), m_pGlobalUserData->szAccounts);
		m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));
	}

	//�������
	if (cbMatchUserStatus == MUS_COMPLETED)
	{
		//�������
		m_pIModuleItemSink->PostWindowMessage(WM_SERVER_MATCH_COMPLETED,0,0);
	}

	return true;
}

//�����ɹ�
bool CModuleServer::OnSocketSubSignupSuccess(VOID* pData, WORD wDataSize)
{
	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ���������ɹ�..."), m_pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

//���÷���
VOID CModuleServer::ResetModule()
{
	//���ñ���
	m_wKindID = 0;
	m_wTableCount = 0;
	m_wChairCount = 0;
	m_wServerType = 0;
	m_dwServerRule = 0;

	//������Ϣ
	m_lMatchNo = 0;
	m_dwMatchID = 0;
	m_cbMatchUStatus = MUS_NULL;

	//����״̬
	m_pGlobalUserData->cbUserStatus = US_NULL;
	m_pGlobalUserData->wChairID = INVALID_CHAIR;
	m_pGlobalUserData->wTableID = INVALID_TABLE;	

	//���ñ���
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));

	//�رն�ʱ��
	KillModuleTimer(IDI_GAME_PULSE);

	//�ͷŶ���
	if (m_pRobotUserItem != NULL)
	{
		g_RobotUserManager->FreeRobotUserItem(m_pRobotUserItem);		
	}

	//���ñ���
	m_pRobotUserItem = NULL;	
}

//���뷿��
bool CModuleServer::EnterServer(WORD wKindID, WORD wServerID, WORD wServerLevel)
{
	return SendLogonPacket(wKindID, wServerLevel, wServerID, NULL, 0);
}

//���뷿��
bool CModuleServer::EnterMatchServer(WORD wKindID, WORD wServerID, DWORD dwMatchID, SCORE lMatchNo)
{
	//������Ϣ
	m_lMatchNo = lMatchNo;
	m_dwMatchID = dwMatchID;

	//��������
	tagLogonMatchDetails LogonMatchDetails;
	ZeroMemory(&LogonMatchDetails, sizeof(LogonMatchDetails));

	//���ñ���
	LogonMatchDetails.lMatchNo = m_lMatchNo;
	LogonMatchDetails.dwMatchID = m_dwMatchID;

	return SendLogonPacket(wKindID, 0, wServerID, &LogonMatchDetails, sizeof(LogonMatchDetails));
}

//��¼����
bool CModuleServer::SendLogonPacket(WORD wKindID, WORD wServerLevel, WORD wServerID, VOID* pExtendData, WORD wDataSize)
{
	//����ṹ
	CMD_GR_LogonMobile * pLogonMobile = (CMD_GR_LogonMobile*)m_BufferPacket.cbDataBuffer;
	ZeroMemory(pLogonMobile,sizeof(CMD_GR_LogonMobile));

	//���ñ���
	pLogonMobile->dwPlazaVersion = 16842752;
	pLogonMobile->dwProcessVersion = g_GlobalUnits.m_dwProcessVersion;
	pLogonMobile->dwDeviceKind = DEVICE_KIND_ANDROID;	

	//������
	CWHService::GetMachineID(pLogonMobile->szMachineID);

	//��չ����
	if (wDataSize > 0)
	{
		CopyMemory(pLogonMobile+1, pExtendData, wDataSize);
	}

	//У��״̬
	if (GetLinkStatus() == true)
	{
		SendVirtualData(MDM_GR_LOGON, SUB_GR_LOGON_MOBILE, pLogonMobile, sizeof(CMD_GR_LogonMobile)+wDataSize);
	}
	else
	{
		//��������
		m_BufferPacket.wMainCmdID = MDM_GR_LOGON;
		m_BufferPacket.wSubCmdID = SUB_GR_LOGON_MOBILE;
		m_BufferPacket.wDataSize = sizeof(CMD_GR_LogonMobile)+wDataSize;		

		//��չ��Ϣ
		tagLinkExtend_Server LinkExtend;
		ZeroMemory(&LinkExtend,sizeof(LinkExtend));

		//���ñ���
		LinkExtend.wKindID = wKindID;
		LinkExtend.wServerLevel = wServerLevel;	

		//��������
		CreateVirtualLink(wServerID, m_pGlobalUserData->dwUserID, m_pGlobalUserData->szLogonToken, (LPBYTE)&LinkExtend, sizeof(LinkExtend));
	}

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ���ڵ�¼����..."), m_pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////