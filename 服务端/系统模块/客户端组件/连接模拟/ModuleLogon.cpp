#include "Stdafx.h"
#include "ModuleLogon.h"
#include "ModuleServer.h"

//////////////////////////////////////////////////////////////////////////////////////////////

//���캯��
CModuleLogon::CModuleLogon(IModuleItemSink* pIModuleItemSink):
	m_pIModuleItemSink(pIModuleItemSink)
{
	//����ģ��
	SetServiceModule(SERVICE_MODULE_LOGON);

	//���ñ���
	ZeroMemory(&m_BufferPacket,sizeof(m_BufferPacket));	
}

//��������
CModuleLogon::~CModuleLogon()
{
}

//�ر��¼�
bool CModuleLogon::OnEventVirtualShut()
{
	//���ñ���
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));

	//Ͷ����Ϣ	
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_LOGON_LINK_SHUT, 0, 0);

	//������Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ���¼���������������ӶϿ���"), m_pIModuleItemSink->GetGlobalUserData()->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(255, 0, 0));

	return true;
}

//�����¼�
bool CModuleLogon::OnEventVirtualLink(LONG lErrorCode)
{
	//����ʧ��
	if (lErrorCode != 0)
	{
		//Ͷ����Ϣ	
		ASSERT(m_pIModuleItemSink != NULL);
		m_pIModuleItemSink->PostWindowMessage(WM_LOGON_LINK_FAILURE, 0, 0);

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
bool CModuleLogon::KillModuleTimer(UINT nTimerID)
{
	//Ͷ����Ϣ
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->KillGameTimer(GetTimerStart() + nTimerID);

	return true;
}

//����ʱ��
bool CModuleLogon::SetModuleTimer(UINT nTimerID, UINT nElapse)
{
	//Ͷ����Ϣ
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->SetGameTimer(GetTimerStart() + nTimerID, nElapse);

	return true;
}

//ʱ���¼�
bool CModuleLogon::OnEventTimer(UINT uTimerID, WPARAM wBindParam)
{
	return true;
}

//��ȡ�¼�
bool CModuleLogon::OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_MB_LOGON:			//��¼����
	{
		return OnSocketMainLogon(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_MB_SERVER_LIST:	//�����б�
	{
		return OnSocketMainServerList(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_MB_USER_SERVICE:	//�û�����
	{
		return OnSocketMainUserService(Command.wSubCmdID, pData, wDataSize);
	}
	}

	return true;
}

//��¼����
bool CModuleLogon::OnSocketMainLogon(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_MB_LOGON_SUCCESS:	//��¼�ɹ�
	{
		return OnSocketSubLogonSuccess(pData, wDataSize);
	}
	case SUB_MB_LOGON_FAILURE:	//��¼ʧ��
	{
		return OnSocketSubLogonFailure(pData, wDataSize);
	}
	case SUB_MB_LOGON_FINISH:	//��¼���
	{
		return OnSocketSubLogonFinish(pData, wDataSize);
	}
	case SUB_MB_UPDATE_NOTIFY:	//������ʾ
	{
		return OnSocketSubUpdateNotify(pData, wDataSize);
	}
	}

	return true;
}

//�б���
bool CModuleLogon::OnSocketMainServerList(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_MB_LIST_SERVER:	//�����б�
	{
		//Ч�����
		ASSERT(wDataSize % sizeof(tagPCGameServer) == 0);
		if (wDataSize % sizeof(tagPCGameServer) != 0) return false;

		//��������
		WORD wItemCount = wDataSize / sizeof(tagPCGameServer);
		auto  pGameServer = (tagPCGameServer*)(pData);

		return true;
	}
	}

	return true;
}

//�û�����
bool CModuleLogon::OnSocketMainUserService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	return true;
}

//��¼ʧ��
bool CModuleLogon::OnSocketSubLogonFailure(VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pLogonFailure = (CMD_MB_LogonFailure*)pData;

	ASSERT(m_pIModuleItemSink);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ��¼ʧ��,%s!"), pGlobalUserData->szAccounts, pLogonFailure->szDescribeString);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	//Ͷ����Ϣ		
	m_pIModuleItemSink->PostWindowMessage(WM_LOGON_LOGON_FAILURE, 0, 0);

	return true;
}

//��¼�ɹ�
bool CModuleLogon::OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pLogonSuccess = (CMD_MB_LogonSuccess*)pData;

	//�û�����
	ASSERT (m_pIModuleItemSink!=NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//������Ϣ
	pGlobalUserData->dwUserID = pLogonSuccess->dwUserID;
	pGlobalUserData->dwGameID = pLogonSuccess->dwGameID;
	pGlobalUserData->wFaceID = pLogonSuccess->wFaceID;
	pGlobalUserData->cbGender = pLogonSuccess->cbGender;
	pGlobalUserData->dwCustomID = pLogonSuccess->dwCustomID;	
	StringCchCopy(pGlobalUserData->szNickName,CountArray(pGlobalUserData->szNickName), pLogonSuccess->szNickName);
	StringCchCopy(pGlobalUserData->szAccounts, CountArray(pGlobalUserData->szAccounts), pLogonSuccess->szAccounts);
	StringCchCopy(pGlobalUserData->szLogonPass, CountArray(pGlobalUserData->szLogonPass), pLogonSuccess->szLogonPass);
	StringCchCopy(pGlobalUserData->szInsurePass, CountArray(pGlobalUserData->szInsurePass), pLogonSuccess->szInsurePass);
	
	//����ȼ�
	pGlobalUserData->dwExperience = pLogonSuccess->dwExperience;

	//��Ա����
	pGlobalUserData->wMasterOrder = pLogonSuccess->wMasterOrder;
	pGlobalUserData->dwMemberPoint = pLogonSuccess->dwMemberPoint;
	pGlobalUserData->dwMemberOverTime = pLogonSuccess->dwMemberOverTime;

	//�Ƹ���Ϣ
	pGlobalUserData->lUserGold = pLogonSuccess->lUserGold;
	pGlobalUserData->lUserCard = pLogonSuccess->lUserCard;
	pGlobalUserData->lUserInsure = pLogonSuccess->lUserInsure;

	//��Ϊ����
	pGlobalUserData->dwActionMaskEver = pLogonSuccess->dwActionMaskEver;
	pGlobalUserData->dwActionMaskPerDay = pLogonSuccess->dwActionMaskPerDay;
	pGlobalUserData->dwActionMaskPerWeek = pLogonSuccess->dwActionMaskPerWeek;

	//������Ϣ
	pGlobalUserData->wLockServerID = pLogonSuccess->wLockServerID;

	//�ϼ���Ϣ
	pGlobalUserData->wParentFaceID = pLogonSuccess->wParentFaceID;
	pGlobalUserData->dwParentGameID = pLogonSuccess->dwParentGameID;
	StringCchCopy(pGlobalUserData->szParentNickName, CountArray(pGlobalUserData->szParentNickName), pLogonSuccess->szParentNickName);

	//��չ��Ϣ
	if (wDataSize > sizeof(CMD_MB_LogonSuccess))
	{
		//��������
		VOID* pDataBuffer = NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pLogonSuccess + 1, wDataSize - sizeof(CMD_MB_LogonSuccess));

		//��ȡ��Ϣ
		while (true)
		{
			//��ȡ����
			pDataBuffer = RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe == DTP_NULL) break;

			//��ȡ����
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_MB_MOBILE_PHONE:		//�ֻ�����
			{
				ASSERT(DataDescribe.wDataSize <= sizeof(pGlobalUserData->szMobilePhone));
				if (DataDescribe.wDataSize <= sizeof(pGlobalUserData->szMobilePhone))
				{
					pGlobalUserData->szMobilePhone[CountArray(pGlobalUserData->szMobilePhone) - 1L] = 0;
					StringCchCopy(pGlobalUserData->szMobilePhone, CountArray(pGlobalUserData->szMobilePhone), (LPCTSTR)pDataBuffer);
				}

				break;
			}
			case DTP_MB_LOGON_TOKEN:		//��¼����
			{
				ASSERT(DataDescribe.wDataSize <= sizeof(pGlobalUserData->szLogonToken));
				if (DataDescribe.wDataSize <= sizeof(pGlobalUserData->szLogonToken))
				{
					pGlobalUserData->szLogonToken[CountArray(pGlobalUserData->szLogonToken) - 1L] = 0;
					StringCchCopy(pGlobalUserData->szLogonToken, CountArray(pGlobalUserData->szLogonToken), (LPCTSTR)pDataBuffer);
				}

				break;
			}
			}
		}
	}	

	return true;
}

//��¼���
bool CModuleLogon::OnSocketSubLogonFinish(VOID* pData, WORD wDataSize)
{
	ASSERT(m_pIModuleItemSink);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ƽ̨��¼�ɹ�!"),pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	//Ͷ����Ϣ	
	m_pIModuleItemSink->PostWindowMessage(WM_LOGON_LOGON_FINISH, 0, 0);

	return true;
}

//������ʾ
bool CModuleLogon::OnSocketSubUpdateNotify(VOID* pData, WORD wDataSize)
{
	ASSERT(m_pIModuleItemSink);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ��¼ʧ��,�汾��һ��!"), pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	//Ͷ����Ϣ	
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_LOGON_LOGON_FAILURE, 0, 0);

	return true;
}

//�ο͵�¼
bool CModuleLogon::LogonByVisitor(LPCTSTR pszMachineID)
{
	//����ṹ
	CMD_MB_LogonVisitor LogonVisitor;
	ZeroMemory(&LogonVisitor, sizeof(LogonVisitor));

	//���ñ���
	LogonVisitor.wMarketID = 1;
	LogonVisitor.dwAppVersion = 16842752;
	LogonVisitor.dwPlazaVersion = 16842752;
	LogonVisitor.dwDeviceKind = DEVICE_KIND_ANDROID;
	StringCchCopy(LogonVisitor.szMachineID, CountArray(LogonVisitor.szMachineID) ,pszMachineID);

	//��ȡ������
	CWHService::GetMachineID(LogonVisitor.szMachineID);

	//У��״̬
	if (GetLinkStatus() == true)
	{
		SendVirtualData(MDM_MB_LOGON, SUB_MB_LOGON_VISITOR, &LogonVisitor, sizeof(LogonVisitor));
	}
	else
	{
		//��������
		m_BufferPacket.wMainCmdID = MDM_MB_LOGON;
		m_BufferPacket.wSubCmdID = SUB_MB_LOGON_VISITOR;
		m_BufferPacket.wDataSize = sizeof(LogonVisitor);
		CopyMemory(m_BufferPacket.cbDataBuffer, &LogonVisitor, sizeof(LogonVisitor));

		//��������
		CreateVirtualLink(INVALID_WORD, 0, NULL, NULL, 0);
	}

	return true;
}

//�˺ŵ�¼
bool CModuleLogon::LogonByAccounts(LPCTSTR pszAccounts, LPCTSTR pszLogonPass)
{
	//����ṹ
	CMD_MB_LogonAccounts LogonAccounts;
	ZeroMemory(&LogonAccounts,sizeof(LogonAccounts));

	//���ñ���
	LogonAccounts.wMarketID = 1;
	LogonAccounts.dwAppVersion = 16842752;
	LogonAccounts.dwPlazaVersion = 16842752;
	LogonAccounts.dwDeviceKind = DEVICE_KIND_ANDROID;	
	StringCchCopy(LogonAccounts.szAccounts,CountArray(LogonAccounts.szAccounts), pszAccounts);
	
	//MD5����
	CMD5Encrypt::EncryptData(pszLogonPass, LogonAccounts.szPassword);

	//��ȡ������
	CWHService::GetMachineID(LogonAccounts.szMachineID);

	//��ȡָ��
	ASSERT(m_pIModuleItemSink);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//�����˺�
	StringCchCopy(pGlobalUserData->szAccounts, CountArray(pGlobalUserData->szAccounts), pszAccounts);

	//У��״̬
	if (GetLinkStatus() == true)
	{
		SendVirtualData(MDM_MB_LOGON, SUB_MB_LOGON_ACCOUNTS, &LogonAccounts, sizeof(LogonAccounts));
	}
	else
	{
		//��������
		m_BufferPacket.wMainCmdID = MDM_MB_LOGON;
		m_BufferPacket.wSubCmdID = SUB_MB_LOGON_ACCOUNTS;		
		m_BufferPacket.wDataSize = sizeof(LogonAccounts);
		CopyMemory(m_BufferPacket.cbDataBuffer, &LogonAccounts, sizeof(LogonAccounts));

		//��������
		CreateVirtualLink(INVALID_WORD,0,NULL,NULL,0);
	}

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ���ڵ�¼ƽ̨..."), pszAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

//����ģ��
VOID CModuleLogon::ResetModule()
{
	//���ñ���
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));
}
//////////////////////////////////////////////////////////////////////////////////////////////
