#include "StdAfx.h"
#include "Resource.h"
#include "GlobalUnits.h"
#include "SessionLogon.h"
#include "PlatformFrame.h"
#include "DispatchManager.h"
//#include "ServiceManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//��̬����

CSessionLogon * CSessionLogon::m_pMissionLogon=NULL;					//����ָ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CSessionLogon::CSessionLogon()
{
	//���ñ���
	m_cbMissionType = MT_LOGON;
	ZeroMemory(m_chBuffer,sizeof(m_chBuffer));

	//�ؼ�ָ��
	m_pDlgLogon=NULL;
	m_pDlgStatus=NULL;

	//���ö���
	ASSERT(m_pMissionLogon==NULL);
	if (m_pMissionLogon==NULL) m_pMissionLogon=this;

	return;
}

//��������
CSessionLogon::~CSessionLogon()
{
	//ɾ������
	SafeDelete(m_pDlgLogon);
	SafeDelete(m_pDlgStatus);

	//�ͷŶ���
	ASSERT(m_pMissionLogon==this);
	if (m_pMissionLogon==this) m_pMissionLogon=NULL;

	return;
}

////���󻺳�
//DWORD __cdecl CSessionLogon::OnHttpRequestBuffer(DWORD dwRequestSize, LPVOID *ppvBuffer)
//{
//	if( ppvBuffer != NULL )
//	{
//		*ppvBuffer = (LPVOID)m_chBuffer;
//		return sizeof(m_chBuffer);
//	}
//
//	return 0;
//}
//
////�������
//bool __cdecl CSessionLogon::OnHttpDataComplete(DWORD dwDataSize)
//{
//	//��������
//	BOOL bValidate = FALSE;
//	CString strTips = TEXT("");
//	CString strAdText = TEXT("");
//	CString strAdUrl = TEXT("");
//
//	ASSERT(dwDataSize>0);
//	ShowInformation(TEXT("�������ʧ�ܣ�ȱ��ϵͳ���msxml2.dll����߰汾��"), MB_ICONERROR, 30);
//	//if( dwDataSize > 0 )
//	//{
//	//	m_chBuffer[CountArray(m_chBuffer)-1] = 0;
//
//	//	//����XML
//	//	try
//	//	{
//	//		if( m_pXmlDoc == NULL )
//	//		{
//	//			HRESULT hr = m_pXmlDoc.CreateInstance(MSXML2::CLSID_DOMDocument);
//	//			if( FAILED(hr) ) _com_issue_error(hr);
//
//	//			//����ʱЧ��
//	//			m_pXmlDoc->PutvalidateOnParse(VARIANT_TRUE);
//	//		}
//
//	//		//��������
//	//		if( VARIANT_TRUE == m_pXmlDoc->loadXML((_bstr_t)m_chBuffer) )
//	//		{
//	//			//��������
//	//			MSXML2::IXMLDOMNodePtr  pNode;
//
//	//			//�ж���֤ͨ��
//	//			pNode = m_pXmlDoc->selectSingleNode(TEXT("//Validate/Flag"));
//	//			if( pNode != NULL )
//	//			{
//	//				MSXML2::IXMLDOMNodePtr pChild = pNode->firstChild;
//	//				ASSERT(pChild != NULL && MSXML2::NODE_TEXT == pChild->nodeType);
//	//				if( pChild != NULL && MSXML2::NODE_TEXT == pChild->nodeType )
//	//				{
//	//					bValidate = (_tstol(pChild->text)==0?TRUE:FALSE);
//	//				}
//
//	//				pNode.Release();
//	//			}
//
//	//			//�����֤ͨ��
//	//			if( bValidate )
//	//			{
//	//				//��ȡ��ʾ
//	//				pNode = m_pXmlDoc->selectSingleNode(TEXT("//Validate/Tips"));
//	//				if( pNode != NULL )
//	//				{
//	//					MSXML2::IXMLDOMNodePtr pChild = pNode->firstChild;
//	//					ASSERT(pChild != NULL && MSXML2::NODE_TEXT == pChild->nodeType);
//	//					if( pChild != NULL && MSXML2::NODE_TEXT == pChild->nodeType )
//	//					{
//	//						strTips = (LPCTSTR)pChild->text;
//	//					}
//
//	//					pNode.Release();
//	//				}
//	//				//��ȡ����
//	//				pNode = m_pXmlDoc->selectSingleNode(TEXT("//Validate/AdUrl"));
//	//				if( pNode != NULL )
//	//				{
//	//					MSXML2::IXMLDOMNodePtr pChild = pNode->firstChild;
//	//					if( pChild != NULL && MSXML2::NODE_TEXT == pChild->nodeType )
//	//					{
//	//						strAdUrl = (LPCTSTR)pChild->text;
//	//					}
//
//	//					pNode.Release();
//	//				}
//	//				//��ȡ��������
//	//				pNode = m_pXmlDoc->selectSingleNode(TEXT("//Validate/AdText"));
//	//				if( pNode != NULL )
//	//				{
//	//					MSXML2::IXMLDOMNodePtr pChild = pNode->firstChild;
//	//					if( pChild != NULL && MSXML2::NODE_TEXT == pChild->nodeType )
//	//					{
//	//						strAdText = (LPCTSTR)pChild->text;
//	//					}
//
//	//					pNode.Release();
//	//				}
//	//			}
//	//		}
//	//	}catch( _com_error &err )
//	//	{
//	//		//���ñ���
//	//		bValidate = FALSE;
//
//	//		if( CO_E_CLASSSTRING == err.Error() || CO_E_IIDSTRING == err.Error() )
//	//		{
//	//			ShowInformation(TEXT("�������ʧ�ܣ�ȱ��ϵͳ���msxml2.dll����߰汾��"),MB_ICONERROR,30);
//	//		}
//	//		else
//	//		{
//	//			TCHAR szMess[128] = TEXT("");
//	//			// add by humroly 2018/8/3
//	//			// replace func
//	//			_sntprintf_s(szMess, CountArray(szMess), TEXT("��֤�����г����쳣��(0x%x,%s)"), err.Error(), err.ErrorMessage());
//	//			ShowInformation( szMess,MB_ICONERROR,30 );
//	//		}
//	//	}
//	//}
//
//	//�ر�״̬
//	CloseStatusInfo();
//
//	//��ʾ��Ϣ
//	ShowInformation(TEXT("�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��"),MB_ICONERROR,30);
//
//	//���µ�¼
//	RestoreShow();
//
//	return true;
//}
//
////�����쳣
//bool __cdecl CSessionLogon::OnHttpDataException(enDownLoadError DownLoadError)
//{
//	//���Ƚ���
//	CloseStatusInfo();
//
//	//��ʾ��Ϣ
//	ShowInformation(TEXT("�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��"),MB_ICONERROR,30);
//
//	//���µ�¼
//	RestoreShow();
//
//	return true;
//}
//
////���ӹر�
//bool __cdecl CSessionLogon::OnHttpSocketClose(bool bCloseByServer)
//{
//	if( !bCloseByServer ) return true;
//
//	//���Ƚ���
//	CloseStatusInfo();
//
//	//��ʾ��Ϣ
//	ShowInformation(TEXT("�����ʺŲ����ڻ������������������֤���ٴγ��Ե�¼��"),MB_ICONERROR,30);
//
//	//���µ�¼
//	RestoreShow();
//
//	return true;
//}

//�����¼�
bool CSessionLogon::OnEventMissionLink(LONG nErrorCode)
{
	//������
	if (nErrorCode!=0L)
	{
		//��������
		/*CMissionManager * pMissionManager=GetMissionManager();
		if (pMissionManager->AvtiveSessionItem(this,true)==true) return true;*/

		//��ֹ����
		m_bActiveStatus = false;

		//�ر���ʾ
		CloseStatusInfo();

		//��ʾ��ʾ
		CInformation Information(CPlatformFrame::GetInstance());
		Information.ShowMessageBox(TEXT("���������еķ��������޷��ɹ����ӷ���������������վά�����棡"),MB_ICONERROR);

		//���µ�¼
		RestoreShow();
	}
	else
	{
		//��������
		WORD wPacketSize, wSubCmdID;
		BYTE cbBuffer[SOCKET_PACKET];

		//��������
		if (m_cbMissionType == MT_LOGON)
		{
			ASSERT(m_pDlgLogon != NULL);
			wPacketSize = m_pDlgLogon->ConstructLogonPacket(cbBuffer, sizeof(cbBuffer));
			wSubCmdID = SUB_GP_LOGON_MANAGETOOL;
		}

		//��������
		CSendPacketHelper Packet(cbBuffer + wPacketSize, sizeof(cbBuffer) - wPacketSize);

		//��������
		WCHAR szMachine[LEN_MACHINE_ID] = { 0 };
		WCHAR szNewMachine[LEN_MACHINE_ID] = { 0 };

		//��ȡ����
		//CWHService::GetMachineID(szMachine);
		CWHService::GetMachineID(szNewMachine);

		//׷������
		Packet.AddPacket(szMachine, sizeof(WCHAR) * LEN_MACHINE_ID, DTP_GP_MACHINE_ID);
		Packet.AddPacket(szNewMachine, sizeof(WCHAR) * LEN_MACHINE_ID, DTP_GP_NEW_COMPUTER_ID);

		//��������
		auto pVirtualLinkItem = GetVirtualLinkItem();
		pVirtualLinkItem->SendVirtualData(MDM_GP_LOGON, wSubCmdID, cbBuffer, wPacketSize + Packet.GetDataSize());

		//��ʾ��Ϣ
		ShowStatusInfo(TEXT("������֤�û���¼��Ϣ..."));
	}

	return true;
}

//�ر��¼�
bool CSessionLogon::OnEventMissionShut(BYTE cbShutReason)
{
	//��ֹ����
	m_bActiveStatus = false;

	//�رմ���
	if (cbShutReason!=SHUT_REASON_NORMAL)
	{
		//�رմ���
		CloseStatusInfo();
	}
	
	//�����¼�
	CDispatchManager* pDispatchManager = CDispatchManager::GetInstance();
	if (pDispatchManager != NULL) pDispatchManager->PostEventInform(EVENT_LOGOUT_NOTIFY, 1L);	

	return true;
}

//��ȡ�¼�
bool CSessionLogon::OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	WORD wCmd = LOCMMD(Command.wMainCmdID);
	switch(wCmd)
	{
	case MDM_GP_LOGON:			//��¼��Ϣ
		{
			return OnSocketMainLogon(Command.wSubCmdID,pData,wDataSize);
		}
	case MDM_CM_SYSTEM:			//ϵͳ��Ϣ
		{
			return OnSocketMainSystem(Command.wSubCmdID,pData,wDataSize);
		}
	case MDM_GP_USER_SERVICE:	//��ҷ���
		{
			return OnSocketMainService(Command.wSubCmdID,pData,wDataSize);
		}
	case MDM_GP_SERVER_LIST:	//��Ϸ����
	{
		return OnSocketMainServerList(Command.wSubCmdID, pData, wDataSize);
	}	
	}

	return false;
}

//��ʾ��¼
VOID CSessionLogon::ShowLogon()
{
	//��������
	if (m_pDlgLogon==NULL)
	{
		m_pDlgLogon=new CDlgLogon;
	}

	//��������
	if (m_pDlgLogon->m_hWnd==NULL)
	{
		m_pDlgLogon->Create(IDD_DLG_LOGON,CPlatformFrame::GetInstance());
	}

	//���ñ���
	m_cbMissionType = MT_LOGON;

	//��ʾ����
	m_pDlgLogon->ShowWindow(SW_SHOW);
	m_pDlgLogon->SetForegroundWindow();

	return;
}

//ִ�е�¼
bool CSessionLogon::PerformLogonMission(BYTE cbMissonType)
{
	//����״̬
	m_bActiveStatus = false;
	m_cbMissionType = cbMissonType;

	//��ȡ��ַ
	TCHAR szCustomServer[LEN_SERVER]=TEXT("");
	if (m_pDlgLogon!=NULL) lstrcpyn(szCustomServer,m_pDlgLogon->m_szLogonServer,CountArray(szCustomServer));

	//���õ�ַ
	auto pVirtualLinkItem = GetVirtualLinkItem();
	if (pVirtualLinkItem != NULL)
	{
		pVirtualLinkItem->GetVirtualLinkManager()->SetCurrentServer(szCustomServer);		
	}

	//��������
	if (pVirtualLinkItem->CreateVirtualLink(INVALID_WORD,0,TEXT(""),NULL,0) == false)
	{
		//������ʾ
		CInformation Information(CPlatformFrame::GetInstance());
		Information.ShowMessageBox(TEXT("����������ʧ�ܣ��������������Լ���������״����"),MB_ICONERROR);

		//���µ�¼
		RestoreShow();

		return false;
	}

	//����״̬
	m_bActiveStatus = true;

	//д���ַ
	CGlobalServer GlobalServer;
	GlobalServer.SetLastServerName(szCustomServer);

	//��ʾ����
	ShowStatusInfo(TEXT("�������ӷ��������������Ժ�Ƭ��..."));

	return true;
}

//��¼�û�
VOID CSessionLogon::UpdateUserInfo()
{
	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//������Ϣ
	TCHAR szUserID[16];
	// add by humroly 2018/8/3
	// replace func
	_sntprintf_s(szUserID, CountArray(szUserID), TEXT("%ld"), pGlobalUserData->dwUserID);

	//�ϴ��û�
	CRegKeyAide RegUserInfo;
	RegUserInfo.OpenRegKey(REG_USER_INFO,true);
	RegUserInfo.WriteValue(TEXT("LastUserID"),pGlobalUserData->dwUserID);
	if(m_pDlgLogon!=NULL)
		RegUserInfo.WriteValue(TEXT("LogonMode"),m_pDlgLogon->GetLogonMode());

	//��¼��Ϣ
	if (pParameterGlobal->m_AcountsRule!=AcountsRule_None)
	{
		//��������
		TCHAR szGameID[16]=TEXT("");
		TCHAR szStationID[16]=TEXT("");
		TCHAR szPassBuffer[MAX_TARGET_LEN]=TEXT("");

		//�û�����
		CRegKeyAide RegUserItem;
		RegUserItem.Attach(RegUserInfo.CreateItemKey(szUserID));

		//��Ϸ��ʶ
		if (pGlobalUserData->dwGameID!=0L)
		{
			// add by humroly 2018/8/3
			// replace func
			_sntprintf_s(szGameID, CountArray(szGameID), TEXT("%ld"), pGlobalUserData->dwGameID);
		}

		// add by humroly 2018/8/3
		// replace func
		_sntprintf_s(szStationID, CountArray(szStationID), TEXT("%ld"), pGlobalUserData->dwStationID);

		//�û�����
		if (pParameterGlobal->m_AcountsRule==AcountsRule_AccountsAndPass) 
		{
			LPCTSTR pszPassword = NULL;
			pszPassword = m_pDlgLogon->GetPassword();

			CXOREncrypt::EncryptData(pszPassword,szPassBuffer,CountArray(szPassBuffer));
		}

		//д����Ϣ
		RegUserItem.WriteString(TEXT("GameID"),szGameID);
		RegUserItem.WriteString(TEXT("UserAccount"),pGlobalUserData->szAccounts);
		RegUserItem.WriteString(TEXT("UserPassToken"),pGlobalUserData->szPassword);
		RegUserItem.WriteString(TEXT("StationID"),szStationID);
		if(m_pDlgLogon!=NULL && m_pDlgLogon->GetLogonMode()==LogonMode_BBS_Accounts)
		{
			CXOREncrypt::EncryptData(m_pDlgLogon->m_szPassword,szPassBuffer,CountArray(szPassBuffer));
			RegUserItem.WriteString(TEXT("BBSPassword"),szPassBuffer);
			RegUserItem.WriteString(TEXT("BBSAccount"),m_pDlgLogon->m_szAccounts);
		}
		else
			RegUserItem.WriteString(TEXT("UserPassword"),szPassBuffer);
	}

	//ɾ����¼
	if (pParameterGlobal->m_AcountsRule==AcountsRule_None)
	{
		RegUserInfo.RecurseDeleteKey(szUserID);
	}

	return;
}

//��ʾ״̬
VOID CSessionLogon::ShowStatusInfo(LPCTSTR pszStatusInfo)
{
	//��������
	if (m_pDlgStatus==NULL)
	{
		m_pDlgStatus=new CDlgStatus;
	}

	//��������
	if (m_pDlgStatus->m_hWnd==NULL)
	{
		m_pDlgStatus->Create(IDD_DLG_STATUS,CPlatformFrame::GetInstance());
	}

	//��ʾ����
	m_pDlgStatus->ShowStatusWnd(pszStatusInfo,m_pDlgLogon->m_hWnd,IDM_GIVE_UP_CONNECT);

	return;
}

//��¼����
bool CSessionLogon::OnSocketMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GP_LOGON_SUCCESS:	//��¼�ɹ�
	{
		return OnSocketSubLogonSuccess(pData,wDataSize);
	}
	case SUB_GP_LOGON_FAILURE:	//��¼ʧ��
	{
		return OnSocketSubLogonFailure(pData,wDataSize);
	}
	case SUB_GP_LOGON_FINISH:	//��¼���
	{
		return OnSocketSubLogonFinish(pData,wDataSize);
	}
	case SUB_GP_LOGOUT_NOTIFY:	//�ǳ�����
	{
		return OnSocketSubLogoutNotify(pData, wDataSize);
	}
	case SUB_GP_UPDATE_NOTIFY:	//������ʾ
	{
		return OnSocketSubUpdateNotify(pData,wDataSize);
	}
	case SUB_GP_LOGON_ACTIVE:	//�����ʺ�
	{
		return OnSocketSubActivateAccounts(pData,wDataSize);
	}
	case SUB_GP_SHOW_ONLINE:	//������ʾ
	{
		return OnSocketSubShowOnLine(pData,wDataSize);
	}
	case SUB_GP_UPDATE_SILENT:	//��̨����
	{
		return OnSocketSubUpdateSilent(pData,wDataSize);
	}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//ϵͳ����
bool CSessionLogon::OnSocketMainSystem(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch( wSubCmdID )
	{
	case SUB_CM_ACTION_MESSAGE:	//������Ϣ
		{
			return OnSocketSubActionMessage(pData,wDataSize);
		}
	//case SUB_CM_SAFE_PROMPT:	//��ȫ����
	//	{
	//		return OnSocketSubSafePrompt(pData,wDataSize);
	//	}
	case SUB_CM_SYSTEM_MESSAGE:  //ϵͳ��Ϣ
	{
		
		//Ч�����
		//ASSERT(wDataSize == sizeof(CMD_CM_SystemMessage));
		//if (wDataSize != sizeof(CMD_CM_SystemMessage)) return false;

		//��������
		CMD_CM_SystemMessage* pSystemMessage = (CMD_CM_SystemMessage*)pData;

		//������ʾ
		CInformation Information;
		Information.ShowMessageBox(pSystemMessage->szString, MB_ICONERROR);
	}
	}

	ASSERT(FALSE);
	return false;
}

//��ҷ���
bool CSessionLogon::OnSocketMainService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//�����б�
bool CSessionLogon::OnSocketMainServerList(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GP_LIST_TYPE:		//������Ϣ
	{
		//Ч�����
		ASSERT(wDataSize % sizeof(tagGameType) == 0);
		if (wDataSize % sizeof(tagGameType) != 0) return false;

		//��������
		WORD wItemCount = wDataSize / sizeof(tagGameType);
		tagGameType* pGameType = (tagGameType*)(pData);

		//��ȡ����
		ASSERT(CServerListManager::GetInstance() != NULL);
		CServerListManager* pServerListManager = CServerListManager::GetInstance();

		//��������
		for (WORD i = 0; i < wItemCount; i++)
		{
			pServerListManager->InsertGameType(pGameType++);
		}

		return true;
	}
	case SUB_GP_LIST_KIND:		//������Ϣ
	{
		//Ч�����
		ASSERT(wDataSize % sizeof(tagGameKind) == 0);
		if (wDataSize % sizeof(tagGameKind) != 0) return false;

		//��������
		WORD wItemCount = wDataSize / sizeof(tagGameKind);
		tagGameKind* pGameKind = (tagGameKind*)(pData);

		//��ȡ����
		ASSERT(CServerListManager::GetInstance() != NULL);
		CServerListManager* pServerListManager = CServerListManager::GetInstance();

		//��������
		for (WORD i = 0; i < wItemCount; i++)
		{
			pServerListManager->InsertGameKind(pGameKind++);
		}

		return true;
	}
	case SUB_GP_LIST_NODE:		//�ڵ���Ϣ
	{
		//Ч�����
		ASSERT(wDataSize % sizeof(tagGameNode) == 0);
		if (wDataSize % sizeof(tagGameNode) != 0) return false;

		//��������
		WORD wItemCount = wDataSize / sizeof(tagGameNode);
		tagGameNode* pGameNode = (tagGameNode*)(pData);

		//��ȡ����
		ASSERT(CServerListManager::GetInstance() != NULL);
		CServerListManager* pServerListManager = CServerListManager::GetInstance();

		//��������
		for (WORD i = 0; i < wItemCount; i++)
		{
			pServerListManager->InsertGameNode(pGameNode++);
		}

		return true;
	}
	case SUB_GP_LIST_SERVER:	//������Ϣ
	{
		//Ч�����
		ASSERT(wDataSize % sizeof(tagPCGameServer) == 0);
		if (wDataSize % sizeof(tagPCGameServer) != 0) return false;

		//��������
		WORD wItemCount = wDataSize / sizeof(tagPCGameServer);
		tagPCGameServer* pGameServer = (tagPCGameServer*)(pData);

		//��ȡ����
		CServerListManager* pServerListManager = CServerListManager::GetInstance();

		//��������
		for (WORD i = 0; i < wItemCount; i++)
		{
			pServerListManager->InsertGameServer(pGameServer++);
		}

		return true;
	}
	case SUB_GP_LIST_CUSTOM:	//������Ϣ
	{
		//Ч�����
		ASSERT(wDataSize % sizeof(tagGameCustom) == 0);
		if (wDataSize % sizeof(tagGameCustom) != 0) return false;

		//��������
		WORD wItemCount = wDataSize / sizeof(tagGameCustom);
		tagGameCustom* pGameCustom = (tagGameCustom*)(pData);

		//��ȡ����
		CServerListManager* pServerListManager = CServerListManager::GetInstance();

		//��������
		for (WORD i = 0; i < wItemCount; i++)
		{
			pServerListManager->InsertGameCustom(pGameCustom++);
		}

		return true;
	}
	case SUB_GP_LIST_FINISH:	//�б���Ϣ
	{
		//�б����
		ASSERT(CServerListManager::GetInstance() != NULL);
		if (CServerListManager::GetInstance() != NULL) CServerListManager::GetInstance()->OnEventListFinish();

		//��������
		CServerListManager::GetInstance()->UpdateOnlineCount();

		return true;
	}
	case SUB_GP_SERVER_FINISH:	//�������
	{
		//Ч������
		ASSERT(wDataSize % sizeof(WORD) == 0);
		if (wDataSize % sizeof(WORD) != 0) return false;

		//��ȡ����
		ASSERT(CServerListManager::GetInstance() != NULL);
		CServerListManager* pServerListManager = CServerListManager::GetInstance();

		//������
		for (UINT i = 0; i < (wDataSize / sizeof(WORD)); i++)
		{
			//m_GameKindIDPending.RemoveKey(((WORD*)pData)[i]);
			pServerListManager->OnEventKindFinish(((WORD*)pData)[i]);
		}

		//��ֹ����
		//if ((m_GameKindIDPending.GetCount() == 0L) && (m_bOnLineMission == false))
		{
			m_bActiveStatus = false;
		}

		return true;
	}
	case SUB_GR_KINE_ONLINE:	//��������
	{
		//��������
		CMD_GP_KindOnline* pKindOnline = (CMD_GP_KindOnline*)pData;
		WORD wHeadSize = (sizeof(CMD_GP_KindOnline) - sizeof(pKindOnline->OnLineInfoKind));

		//Ч������
		ASSERT((wDataSize >= wHeadSize) && (wDataSize == (wHeadSize + pKindOnline->wKindCount * sizeof(tagOnLineInfoKind))));
		if ((wDataSize < wHeadSize) || (wDataSize != (wHeadSize + pKindOnline->wKindCount * sizeof(tagOnLineInfoKind)))) return false;

		//��ȡ����
		CServerListManager* pServerListManager = CServerListManager::GetInstance();

		//��������
		for (WORD i = 0; i < pKindOnline->wKindCount; i++)
		{
			tagOnLineInfoKind* pOnLineInfoKind = &pKindOnline->OnLineInfoKind[i];
			pServerListManager->SetKindOnLineCount(pOnLineInfoKind->wKindID, pOnLineInfoKind->dwOnLineCount);
		}

		//��������
		pServerListManager->UpdateOnlineCount();

		return true;
	}
	case SUB_GR_SERVER_ONLINE:	//��������
	{
		//��������
		CMD_GP_ServerOnline* pServerOnline = (CMD_GP_ServerOnline*)pData;
		WORD wHeadSize = (sizeof(CMD_GP_ServerOnline) - sizeof(pServerOnline->OnLineInfoServer));

		//Ч������
		ASSERT((wDataSize >= wHeadSize) && (wDataSize == (wHeadSize + pServerOnline->wServerCount * sizeof(tagOnLineInfoServer))));
		if ((wDataSize < wHeadSize) || (wDataSize != (wHeadSize + pServerOnline->wServerCount * sizeof(tagOnLineInfoServer)))) return false;

		//��ȡ����
		CServerListManager* pServerListManager = CServerListManager::GetInstance();

		//��������
		for (WORD i = 0; i < pServerOnline->wServerCount; i++)
		{
			tagOnLineInfoServer* pOnLineInfoServer = &pServerOnline->OnLineInfoServer[i];
			pServerListManager->SetServerOnLineCount(pOnLineInfoServer->wServerID, pOnLineInfoServer->dwOnLineCount);
		}

		//��ֹ����
		//if ((m_GameKindIDPending.GetCount() == 0L) && (m_bOnLineMission == false))
		{
			//��ֹ����
			m_bActiveStatus = false;
		}

		return true;
	}
	}

	return false;
}

//��¼�ɹ�
bool CSessionLogon::OnSocketSubLogonSuccess(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize>=sizeof(CMD_GP_LogonSuccess));
	if (wDataSize<sizeof(CMD_GP_LogonSuccess)) return false;

	//��Ϣ����
	CMD_GP_LogonSuccess * pLogonSuccess=(CMD_GP_LogonSuccess *)pData;

	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//������Ϣ
	pGlobalUserData->cbAge=pLogonSuccess->cbAge;
	pGlobalUserData->wFaceID=pLogonSuccess->wFaceID;
	pGlobalUserData->cbGender=pLogonSuccess->cbGender;
	pGlobalUserData->dwGameID=pLogonSuccess->dwGameID;
	pGlobalUserData->dwUserID=pLogonSuccess->dwUserID;
	pGlobalUserData->dwCustomID=pLogonSuccess->dwCustomID;
	pGlobalUserData->dwExperience=pLogonSuccess->dwExperience;
	pGlobalUserData->cbMasterOrder = pLogonSuccess->cbMasterOrder;
	pGlobalUserData->dwProtectID = pLogonSuccess->dwProtectID;
	pGlobalUserData->cbMoorMachine = pLogonSuccess->cbMoorMachine;
	pGlobalUserData->lGold = pLogonSuccess->lUserGold;
	pGlobalUserData->lInsureGold = pLogonSuccess->lInsureGold;

	//�û��ʺ�
	CW2CT strAccounts(pLogonSuccess->szAccounts);
	lstrcpyn(pGlobalUserData->szAccounts,strAccounts,CountArray(pGlobalUserData->szAccounts));

	//�û�����
	CW2CT strPassword(pLogonSuccess->szPassword);
	lstrcpyn(pGlobalUserData->szPassword,strPassword,CountArray(pGlobalUserData->szPassword));

	//��չ��Ϣ
	VOID * pDataBuffer=NULL;
	tagDataDescribe DataDescribe;
	CRecvPacketHelper RecvPacket(pLogonSuccess+1,wDataSize-sizeof(CMD_GP_LogonSuccess));
	while (true)
	{
		//��ȡ����
		pDataBuffer=RecvPacket.GetData(DataDescribe);
		if (DataDescribe.wDataDescribe==DTP_NULL) break;

		//��ȡ����
		switch (DataDescribe.wDataDescribe)
		{
		case DTP_GP_AREA_INFO:		//������Ϣ
			{
				ASSERT(DataDescribe.wDataSize==sizeof(DTP_GP_AreaInfo));
				if (DataDescribe.wDataSize==sizeof(DTP_GP_AreaInfo))
				{
					CW2CT strArea(((DTP_GP_AreaInfo *)pDataBuffer)->szArea);
					CW2CT strCity(((DTP_GP_AreaInfo *)pDataBuffer)->szCity);
					CW2CT strProvince(((DTP_GP_AreaInfo *)pDataBuffer)->szProvince);
					lstrcpyn(pGlobalUserData->szArea,strArea,CountArray(pGlobalUserData->szArea));
					lstrcpyn(pGlobalUserData->szCity,strCity,CountArray(pGlobalUserData->szCity));
					lstrcpyn(pGlobalUserData->szProvince,strProvince,CountArray(pGlobalUserData->szProvince));
				}

				break;
			}
		case DTP_GP_MEMBER_INFO:	//��Ա��Ϣ
			{
				ASSERT(DataDescribe.wDataSize==sizeof(DTP_GP_MemberInfo));
				if (DataDescribe.wDataSize==sizeof(DTP_GP_MemberInfo))
				{
					DTP_GP_MemberInfo * pMemberInfo=(DTP_GP_MemberInfo *)pDataBuffer;
					pGlobalUserData->cbMemberOrder=pMemberInfo->cbMemberOrder;
					//pGlobalUserData->MemberOverDate=pMemberInfo->MemberOverDate;
				}

				break;
			}
		case DTP_GP_UNDER_WRITE:	//����ǩ��
			{
				CW2CT strUnderWrite((WCHAR *)pDataBuffer);
				lstrcpyn(pGlobalUserData->szUnderWrite,strUnderWrite,CountArray(pGlobalUserData->szUnderWrite));

				break;
			}
		case DTP_GP_NICKNAME:		//�û��ǳ�
			{
				CW2CT strNickName((WCHAR *)pDataBuffer);
				lstrcpyn(pGlobalUserData->szNickName,strNickName,CountArray(pGlobalUserData->szNickName));

				break;
			}
		}
	}

	//���ؼ���
	/*if (pLogonSuccess->dwCustomID!=0)
	{
		CCustomFaceManager * pCustomFaceManager=CCustomFaceManager::GetInstance();
		pCustomFaceManager->LoadUserCustomFace(pLogonSuccess->dwUserID,pLogonSuccess->dwCustomID,pGlobalUserData->CustomFaceInfo);
	}*/

	//������ʾ
	ShowStatusInfo(TEXT("���ڻ�ȡ��Ϸ�б�..."));

	return true;
}

//��¼ʧ��
bool CSessionLogon::OnSocketSubLogonFailure(VOID * pData, WORD wDataSize)
{
	//��������
	CMD_GP_LogonFailure * pLogonFailure=(CMD_GP_LogonFailure *)pData;

	//Ч������
	ASSERT(wDataSize>=(sizeof(CMD_GP_LogonFailure)-sizeof(pLogonFailure->szDescribeString)));
	if (wDataSize<(sizeof(CMD_GP_LogonFailure)-sizeof(pLogonFailure->szDescribeString))) return false;

	//�رմ���
	m_bActiveStatus = false;

	//���Ƚ���
	if ((m_pDlgStatus!=NULL)&&(m_pDlgStatus->m_hWnd!=NULL))
	{
		m_pDlgStatus->DestroyWindow();
	}

	//��������
	if( pLogonFailure->lErrorCode == LGF_NEED_ACTIVATE )
	{
		CGlobalStation *pGlobalStation = CGlobalStation::GetInstance();
		ASSERT(pGlobalStation&&m_pDlgLogon);
		if( pGlobalStation && m_pDlgLogon )
		{
			TCHAR szPassword[LEN_MD5] = TEXT("");
			CMD5Encrypt::EncryptData(m_pDlgLogon->GetPassword(),szPassword);

			//�����ַ
			TCHAR szURL[MAX_PATH] = TEXT("");
			// add by humroly 2018/8/3
			// replace func
			_sntprintf_s(szURL, CountArray(szURL), TEXT("http://service.abcdefgh.com/AccountsCheck.aspx?StationID=%ld&UserName=%s&Password=%s"),
				pGlobalStation->GetStationID(),m_pDlgLogon->GetAccounts(),szPassword );

			if( m_HttpServiceHelper.GetInterface() == NULL )
			{
				m_HttpServiceHelper.CreateInstance();

				m_HttpServiceHelper->SetHttpServiceSink(this);
			}

			m_HttpServiceHelper->ConnectServer(szURL);
			return true;
		}
	}
	else if( pLogonFailure->lErrorCode == LGF_CHANGE_ACCOUNT )
	{
		//��ʾ�����ʺ�
		CW2CT strDescribeString(pLogonFailure->szDescribeString);
		if( lstrlen(strDescribeString)>0 )
		{
			TCHAR szTips[128] = TEXT("");
			// add by humroly 2018/8/3
			// replace func
			_sntprintf_s(szTips, CountArray(szTips), TEXT("�����ʺ��Ѿ�����������ʺ� �� %s �������ס���ʺŲ�ʹ�����ʺŵ�¼��"), (LPCTSTR)strDescribeString);

			ShowInformation(szTips,MB_ICONINFORMATION|MB_OK,30,AfxGetMainWnd());

			//�����ʺ�
			if( m_pDlgLogon && m_pDlgLogon->m_hWnd )
			{
				m_pDlgLogon->DeleteAccounts(NULL);
				m_pDlgLogon->SetAccounts((LPCTSTR)strDescribeString);
			}

			m_cbMissionType = MT_LOGON;

			RestoreShow();

			return true;
		}
	}

	//��ʾ��Ϣ
	CW2CT strDescribeString(pLogonFailure->szDescribeString);
	if (lstrlen(strDescribeString)>0) ShowInformation(strDescribeString,MB_ICONERROR,60);

	//���µ�¼
	RestoreShow();

	return true;
}

//��¼���
bool CSessionLogon::OnSocketSubLogonFinish(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GP_LogonFinish));
	if (wDataSize!=sizeof(CMD_GP_LogonFinish)) return false;

	//��������
	CMD_GP_LogonFinish * pLogonFinish=(CMD_GP_LogonFinish *)pData;

	//��������
	CGlobalStation * pGlobalStation=CGlobalStation::GetInstance();
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

	//ʱ������
	pParameterGlobal->m_wIntermitTime=pLogonFinish->wIntermitTime;
	pParameterGlobal->m_wOnLineCountTime=pLogonFinish->wOnLineCountTime;

	//������Ϣ
	UpdateUserInfo();

	//ɾ���ؼ�
	SafeDelete(m_pDlgLogon);
	SafeDelete(m_pDlgStatus);

	//��ֹ����
	//m_bActiveStatus = false;

	//�����¼�
	CDispatchManager * pDispatchManager=CDispatchManager::GetInstance();
	if (pDispatchManager!=NULL) pDispatchManager->SendEventInform(EVENT_LOGON_SUCCESS,0L);

	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//�������
	//if ((pGlobalUserData->dwCustomID!=0)&&(pGlobalUserData->CustomFaceInfo.dwDataSize==0L))
	//{
	//	CCustomFaceManager * pCustomFaceManager=CCustomFaceManager::GetInstance();
	//	pCustomFaceManager->LoadUserCustomFace(pGlobalUserData->dwUserID,pGlobalUserData->dwCustomID);
	//}

	return true;
}

//��¼�ǳ�
bool CSessionLogon::OnSocketSubLogoutNotify(VOID* pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize <= sizeof(CMD_GP_LogoutNotify));
	if (wDataSize > sizeof(CMD_GP_LogoutNotify)) return false;

	//��������
	auto pLogoutNotify = (CMD_GP_LogoutNotify*)pData;

	//��ֹ����
	m_bActiveStatus = false;
	
	//�����¼�
	CDispatchManager* pDispatchManager = CDispatchManager::GetInstance();
	if (pDispatchManager != NULL) pDispatchManager->SendEventInform(EVENT_LOGOUT_NOTIFY, 0L);

	return true;
}

//������ʾ
bool CSessionLogon::OnSocketSubUpdateNotify(VOID * pData, WORD wDataSize)
{                                       
	//���Ƚ���
	CloseStatusInfo();

	//������ʾ
	CInformation Information(CPlatformFrame::GetInstance());
	Information.ShowMessageBox(TEXT("�汾����"), MB_ICONERROR);

	//���µ�¼
	RestoreShow();

	return true;
}

//������Ϣ
bool CSessionLogon::OnSocketSubActionMessage(VOID * pData, WORD wDataSize)
{
	//��������
	CMD_CM_ActionMessage * pActionMessage=(CMD_CM_ActionMessage *)pData;
	WORD wHeadSize=sizeof(CMD_CM_ActionMessage)-sizeof(pActionMessage->szString);

	//Ч�����
	ASSERT((wDataSize>wHeadSize)&&(wDataSize>=(wHeadSize+pActionMessage->wLength*sizeof(WCHAR))));
	if ((wDataSize<=wHeadSize)||(wDataSize<(wHeadSize+pActionMessage->wLength*sizeof(WCHAR)))) return false;

	//������Ϣ
	CW2CT strMessage(pActionMessage->szString);
	INT nResultCode = ShowInformation(strMessage,pActionMessage->nButtonType,0,AfxGetMainWnd());

	//��������
	WORD wExcursion=wHeadSize+pActionMessage->wLength*sizeof(WCHAR);

	////��ȡ����
	//while (wExcursion<wDataSize)
	//{
	//	//��������
	//	tagActionHead * pActionHead=(tagActionHead *)((BYTE *)pData+wExcursion);

	//	//Ч�����
	//	ASSERT((wExcursion+sizeof(tagActionHead))<=wDataSize);
	//	ASSERT((wExcursion+sizeof(tagActionHead)+pActionHead->wAppendSize)<=wDataSize);

	//	//Ч�����
	//	if ((wExcursion+sizeof(tagActionHead))>wDataSize) return false;
	//	if ((wExcursion+sizeof(tagActionHead)+pActionHead->wAppendSize)>wDataSize) return false;

	//	//��������
	//	if (nResultCode==pActionHead->uResponseID)
	//	{
	//		switch (pActionHead->cbActionType)
	//		{
	//		case ACT_BROWSE:	//�������
	//			{
	//				//��������
	//				WORD wDataPos=wExcursion+sizeof(tagActionHead);
	//				tagActionBrowse * pActionBrowse=(tagActionBrowse *)((BYTE *)pData+wDataPos);

	//				//I E ���
	//				if (pActionBrowse->cbBrowseType&BRT_IE)
	//				{
	//					CW2CT strBrowseUrl(pActionBrowse->szBrowseUrl);
	//					ShellExecute(NULL,TEXT("open"),strBrowseUrl,NULL,NULL,SW_NORMAL);
	//				}

	//				break;
	//			}
	//		case ACT_DOWN_LOAD:	//���ض���
	//			{
	//				//��������
	//				WORD wDataPos=wExcursion+sizeof(tagActionHead);
	//				tagActionDownLoad * pActionDownLoad=(tagActionDownLoad *)((BYTE *)pData+wDataPos);

	//				//I E ����
	//				if (pActionDownLoad->cbDownLoadMode&DLT_IE)
	//				{
	//					CW2CT strDownLoadUrl(pActionDownLoad->szDownLoadUrl);
	//					ShellExecute(NULL,TEXT("open"),strDownLoadUrl,NULL,NULL,SW_NORMAL);
	//				}

	//				//����ģ��
	//				if (pActionDownLoad->cbDownLoadMode&DLT_MODULE)
	//				{
	//					CW2CT strDownLoadUrl(pActionDownLoad->szDownLoadUrl);
	//					ShellExecute(NULL,TEXT("open"),strDownLoadUrl,NULL,NULL,SW_NORMAL);
	//				}

	//				break;
	//			}
	//		}
	//	}

	//	//����ƫ��
	//	wExcursion+=(sizeof(tagActionHead)+pActionHead->wAppendSize);
	//}

	return true;
}

//�ر�״̬
VOID CSessionLogon::CloseStatusInfo()
{
	//�ر���ʾ
	if ((m_pDlgStatus!=NULL)&&(m_pDlgStatus->m_hWnd!=NULL))
	{
		m_pDlgStatus->DestroyWindow();
	}

	return;
}

//�ָ�����
VOID CSessionLogon::RestoreShow()
{
	//��ʾ����
	ShowLogon();

	return;
}

//��ȫ����
bool CSessionLogon::OnSocketSubSafePrompt(VOID * pData, WORD wDataSize)
{
	//RestoreShow();

	return true;
}

//�����ʺ�
bool CSessionLogon::OnSocketSubActivateAccounts(VOID * pData, WORD wDataSize)
{
	//RestoreShow();

	return true;
}

//������ʾ
bool CSessionLogon::OnSocketSubShowOnLine(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(DWORD));
	if(wDataSize!=sizeof(DWORD)) return false;

	return true;
}

//��̨����
bool CSessionLogon::OnSocketSubUpdateSilent(VOID * pData, WORD wDataSize)
{
	return true;
}
//////////////////////////////////////////////////////////////////////////
