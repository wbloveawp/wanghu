#include "StdAfx.h"
#include "Resource.h"
#include "GlobalUnits.h"
#include "DlgRoomConfig.h"
#include "DispatchManager.h"
//#include "DlgInformation.h"

//////////////////////////////////////////////////////////////////////////////////

//�궨��
#define COLOR_FRAME					RGB(239,249,255)					//�����ɫ
#define COLOR_BORAD					RGB(170,170,170)					//�߿���ɫ

//��ϷID
#define KIND_ID_XMOX				112									//Ϻ��ţţ
#define KIND_ID_TEEMPATTI			246									//TeenPatti
#define KIND_ID_DEALRUMMY			248									//DealRummy
#define KIND_ID_WINGO				250									//WinGo
#define KIND_ID_SAWABALOOT			247									//SawaBaloot
#define KIND_ID_SUPERTOUZI			800									//��������
#define KIND_ID_DRAGONTIGER			145									//������
#define KIND_ID_ANIMALBATTLE		123									//��������
#define KIND_ID_BALOOTCARD			251									//Baloot����С��Ϸ

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CDlgRoomConfig, CDlgOptionItem)
	//ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BT_ENTER_ROOM, OnBnClickedBtEnterRoom)
	ON_BN_CLICKED(IDC_BT_EXIT_ROOM, OnBnClickedBtExitRoom)
	ON_BN_CLICKED(IDC_BT_GAME_DEBUG, OnBnClickedBtGameDebug)
	ON_BN_CLICKED(IDC_BT_ROBOT_DEBUG, OnBnClickedBtRobotDebug)
	ON_BN_CLICKED(IDC_BT_CARD_DISPATCHER, OnBnClickedCardDispatcher)
	ON_NOTIFY(NM_RCLICK, IDC_USER_LIST_CONTROL, OnNMRclickUserList)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDlgRoomConfig::CDlgRoomConfig() : CDlgOptionItem(IDD_ROOM_CONFIG)
{
	m_pServerList = NULL;
	m_pIRobotDebug = NULL;
	m_pIClientDebug = NULL;
	m_pICardDispatcher = NULL;
	m_hRobotDebugInst = NULL;
	m_hClientDebugInst = NULL;
	m_hCardDispatcherInst = NULL;

	m_wCurrentPort = PORT_LOGON;
	m_dwCurrentServer = INADDR_NONE;
}

//��������
CDlgRoomConfig::~CDlgRoomConfig()
{
	ReleaseInterface(m_hRobotDebugInst, m_pIRobotDebug);
	ReleaseInterface(m_hClientDebugInst, m_pIClientDebug);
	ReleaseInterface(m_hCardDispatcherInst, m_pICardDispatcher);
}

//�ӿڲ�ѯ
VOID* CDlgRoomConfig::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPSocketSink, Guid, dwQueryVer);
	QUERYINTERFACE(IRoomConfig, Guid, dwQueryVer);
	QUERYINTERFACE(IUserManagerSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IRoomConfig, Guid, dwQueryVer);
	return NULL;
}


//�ؼ���
VOID CDlgRoomConfig::DoDataExchange(CDataExchange * pDX)
{
	DDX_Control(pDX, IDC_USER_LIST_CONTROL, m_UserListControl);

	__super::DoDataExchange(pDX);
}

//���ú���
BOOL CDlgRoomConfig::OnInitDialog()
{
	__super::OnInitDialog();

	//���ýӿ�
	//m_MissionMaster.SetManagerOperateCenter(this);

	//���ûص�
	//CServerListManager::GetInstance()->SetServerListViewSink(this);

	//�������
	if (m_PlazaUserManagerModule.CreateInstance() == false) throw TEXT("�û������������ʧ�ܣ�");
	//�������
	//CUserInformation* pUserInformation = CUserInformation::GetInstance();
	IUnknownEx* pIUnknownEx = QUERY_ME_INTERFACE(IRoomConfig);
	if (m_PlazaUserManagerModule->SetUserManagerSink(pIUnknownEx) == false) throw TEXT("�û������������ʧ�ܣ�");
	//if (m_PlazaUserManagerModule->SetUserInformation(pUserInformation) == false) throw TEXT("�û������������ʧ�ܣ�");

	//��¼ϵͳ
	//m_MissionLogon->ShowLogon();

	//���ð�ť
	EnableButton(true);

	return TRUE;
}

//��ʼ��
VOID CDlgRoomConfig::Initialization(CCheckComboBox* pServerList, CServerListManager* pServerListManager)
{
	m_pServerList = pServerList;
	m_ServerListManager = pServerListManager;

}

//�ؼ�ʹ��
VOID CDlgRoomConfig::EnableControls(BOOL bEnabled)
{
	//ʹ������
	//GetDlgItem(IDC_BT_EXECUTE_CMD)->EnableWindow(bEnabled);
}

//��������
WORD CDlgRoomConfig::ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize)
{
	return 0;
}

//������Ϣ
BOOL CDlgRoomConfig::OnCommand(WPARAM wParam, LPARAM lParam)
{
	UINT nCommandID = LOWORD(wParam);

	switch (nCommandID)
	{
	case IDM_COPY_USER_NAME:	//�����ǳ�
	{
		//Ч��״̬
		//ASSERT(m_MenuUserItemArray.GetCount() > 0);
		//if (m_MenuUserItemArray.GetCount() == 0) return TRUE;

		//�����ַ�
		IClientUserItem* pIClientUserItem = NULL;
		//ѡ������
		POSITION nSelectPos = m_UserListControl.GetFirstSelectedItemPosition();
		INT nSelectItem = m_UserListControl.GetNextSelectedItem(nSelectPos);
		DWORD_PTR lItemData = m_UserListControl.GetItemData(nSelectItem);
		if (lItemData != NULL) pIClientUserItem = ((IClientUserItem*)(lItemData));

		TCHAR strGameID[20];
		if (pIClientUserItem != NULL)
			_sntprintf_s(strGameID, CountArray(strGameID), TEXT("%d"), pIClientUserItem->GetGameID());
		CWHService::CopyToClipBoard(strGameID);

		//�����ַ�
		/*CControlUI * pEditChat = GetControlByName(szEditChatControlName);
		if (pEditChat)
		{
		pEditChat->SetText(pIClientUserItem->GetNickName());
		pEditChat->SetFocus();
		}*/

		return TRUE;
	}
	case IDM_KILL_USER:             //���û�����
	{
		//Ч��״̬
		//ASSERT(m_MenuUserItemArray.GetCount() > 0);
		//if (m_MenuUserItemArray.GetCount() == 0) return TRUE;

		//��ȡ�û�
		IClientUserItem* pIClientUserItem = NULL;

		//ѡ������
		POSITION nSelectPos = m_UserListControl.GetFirstSelectedItemPosition();
		INT nSelectItem = m_UserListControl.GetNextSelectedItem(nSelectPos);
		DWORD_PTR lItemData = m_UserListControl.GetItemData(nSelectItem);
		if (lItemData != NULL) pIClientUserItem = ((IClientUserItem*)(lItemData));
		if (pIClientUserItem == NULL) break;

		////������Ϣ
		//bool bHideUserInfo = CServerRule::IsAllowAvertDebugMode(m_dwServerRule);
		//if ((bHideUserInfo == true) && (m_pIMySelfUserItem->GetMasterOrder() == 0) &&
		//	(m_pIMySelfUserItem->GetMemberOrder() <= pIClientUserItem->GetMemberOrder())) return FALSE;

		//��ȡ����
		WORD wTableID = pIClientUserItem->GetTableID();
		WORD wChairID = pIClientUserItem->GetChairID();
		BYTE cbUserStatus = pIClientUserItem->GetUserStatus();
		if (cbUserStatus == US_PLAYING)
		{
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("������ѿ�ʼ��Ϸ�������ߣ�"), MB_ICONINFORMATION);
			return TRUE;
		}

		//��������
		CMD_GR_KickUser KickUser;
		KickUser.dwTargetUserID = pIClientUserItem->GetUserID();

		//��������
		if (m_pIMySelfUserItem->GetMasterOrder() != 0)
		{
			m_TCPSocketModule->SendData(MAKECMMD(MDM_GR_MANAGE, SERVICE_MODULE_GAME), SUB_GR_KILL_USER, &KickUser, sizeof(KickUser));
		}
		else
		{
			//��������
			//CPlatformFrame* pPlatrformFrame = CPlatformFrame::GetInstance();
			/*CPlazaViewServer * pPlazaViewServer = pPlatrformFrame->GetPlazaViewServer();
			ITableViewFrame * pITableViewFrame = pPlazaViewServer->GetTableViewFrame();

			//������Ϸ
			if (pITableViewFrame->GetChairCount() >= INDEX_ENTER_CHAIR)
			{
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("�ܱ�Ǹ��������Ϸ�������ˣ�"), MB_ICONQUESTION);
			return TRUE;
			}*/

			//m_TCPSocketModule->SendData(MDM_GR_USER, SUB_GR_USER_KICK_USER, &KickUser, sizeof(KickUser));
		}
		return TRUE;
	}
	case IDM_DISMISS_TABLE:					//��ɢ����
	{
        //��ȡ�û�
        IClientUserItem* pIClientUserItem = NULL;

        //ѡ������
        POSITION nSelectPos = m_UserListControl.GetFirstSelectedItemPosition();
        INT nSelectItem = m_UserListControl.GetNextSelectedItem(nSelectPos);
        DWORD_PTR lItemData = m_UserListControl.GetItemData(nSelectItem);
        if (lItemData != NULL) pIClientUserItem = ((IClientUserItem*)(lItemData));
		if (pIClientUserItem == NULL)
		{
            CInformation Information(this);
            Information.ShowMessageBox(TEXT("��Ҳ����ڣ�"), MB_ICONINFORMATION);
            return TRUE;
		}

        //��ȡ����
        WORD wTableID = pIClientUserItem->GetTableID();

        //��������
		CMD_GR_DismissGame DismissGame;
		DismissGame.wDismissTableNum = wTableID;

        //��������
        if (m_pIMySelfUserItem->GetMasterOrder() != 0)
        {
            m_TCPSocketModule->SendData(MAKECMMD(MDM_GR_MANAGE, SERVICE_MODULE_GAME), SUB_GR_DISMISSGAME, &DismissGame, sizeof(DismissGame));
        }

        return TRUE;
	}
	default:
		break;
	}

	return __super::OnCommand(wParam, lParam);
}

//////////////////////////////////////////////////////////////////////
//��ť�¼�
//���뷿��
VOID CDlgRoomConfig::OnBnClickedBtEnterRoom()
{
    //��ȡ������Ϣ
    GetConnectInfo();

	//��ȡ����	
	WORD wServerID = INVALID_WORD;
	for (INT i = 0; i < m_pServerList->GetCount(); i++)
	{
		if (m_pServerList->GetCheck(i) == static_cast<BOOL>(true))
		{
			//��ȡ����
			tagItemData* pItemData = (tagItemData*)m_pServerList->GetItemData(i);
			wServerID = (WORD)pItemData->dwDataPtr;
			break;
		}
	}

	if (wServerID == INVALID_WORD)
	{
		MessageBox(TEXT("û��ѡ����Ϸ��������"));
		return;
	}

	//��������
	POSITION Position = NULL;
	CGameServerItem* pGameServerItem = NULL;

	pGameServerItem = m_ServerListManager->SearchGameServer(wServerID);
	if (pGameServerItem == NULL)
	{
		MessageBox(TEXT("��ǰѡ���ServerΪ�գ�"));
		return;
	}
	m_GameServer = pGameServerItem->m_GameServer;

	//�������
	if (m_TCPSocketModule.CreateInstance() == false) MessageBox(TEXT("�޷��������������������Ϸ�������ʧ�ܣ�"));

	//��������
	m_TCPSocketModule->SetTCPSocketSink(QUERY_ME_INTERFACE(IUnknownEx));

	//�������� pGameServerItem->m_GameServer.dwServerAddrDX, pGameServerItem->m_GameServer.wServerPort
	if (m_TCPSocketModule->Connect(m_dwCurrentServer, m_wCurrentPort) != CONNECT_SUCCESS)
	{
		MessageBox(TEXT("��Ϸ��������ʧ�ܣ�����ϵͳ�������ò����Ƿ���ȷ��"));
		return;
	}

	CGameKindItem* pGameKindItem = m_ServerListManager->SearchGameKind(pGameServerItem->m_GameServer.wKindID);
	//�������
	TCHAR szServerTitle[128] = TEXT("");
	if (pGameKindItem)
	{
		m_GameKind = pGameKindItem->m_GameKind;

		_sntprintf_s(szServerTitle, CountArray(szServerTitle), TEXT("��Ϸ���� > %s > %s"), pGameKindItem->m_GameKind.szKindName, pGameServerItem->m_GameServer.szServerName);
	}
	else
	{
		_sntprintf_s(szServerTitle, CountArray(szServerTitle), TEXT("��Ϸ���� > %s"), pGameServerItem->m_GameServer.szServerName);
	}

	//���ñ���
	SetWindowText(szServerTitle);
}

//�˳�����
VOID CDlgRoomConfig::OnBnClickedBtExitRoom()
{
	//�ر�����
	m_TCPSocketModule->CloseSocket();

	EnableButton(TRUE);
	ReleaseInterface(m_hCardDispatcherInst, m_pICardDispatcher);

	CleanListData();

	//�����Ļ
	//m_ChatMessage.CleanScreen();
}

//��Ϸ����
VOID CDlgRoomConfig::OnBnClickedBtGameDebug()
{
	//Ȩ���ж�
	if (CUserRight::CanCheat(m_dwUserRight) == false)
	{
		MessageBox(TEXT("����ǰû�е���Ȩ�ޣ�"));
		return;
	}

	if (m_pIClientDebug == NULL)
	{
		//��������
		TCHAR szDebugName[LEN_KIND * 2] = TEXT("");

		//��ȡ�������
		int nProcessNameLen = lstrlen(m_GameKind.szProcessName)+1;
		lstrcpyn(szDebugName, m_GameKind.szProcessName, nProcessNameLen - 4);
		_sntprintf(szDebugName, CountArray(szDebugName),TEXT("%sClientDebug.dll"), szDebugName);

		//��ȡ����
		TCHAR szWorkPath[MAX_PATH] = TEXT("");
		CWHService::GetWorkDirectory(szWorkPath, sizeof(szWorkPath));

		//����·��
		TCHAR szFileName[MAX_PATH] = TEXT("");
		_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\%s"), szWorkPath, szDebugName);

		//�����ļ�
		DWORD dwAttrib = GetFileAttributes(szFileName);
		if (!(INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
		{
			CString strMessage;
			strMessage.Format(TEXT("������� %s �����ڣ�"), szFileName);
			MessageBox(strMessage);
			return;
		}

		//�������
		m_hClientDebugInst = LoadLibraryEx(szFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

		//��ȡ����
		if (m_hClientDebugInst)
		{
			typedef void* (*CREATE)();
			CREATE pFunction = (CREATE)GetProcAddress(m_hClientDebugInst, "CreateClientDebug");
			if (pFunction)
			{
				m_pIClientDebug = static_cast<IClientDebug*>(pFunction());
				m_pIClientDebug->Create(this, this);
			}
		}
		else
		{
			CString strMessage;
			strMessage.Format(TEXT("���������ʧ�ܣ�������� %d ��"), GetLastError());
			MessageBox(strMessage);
			return;
		}
	}

	// �ص�
	if (m_pIClientDebug)
	{
		m_pIClientDebug->ShowWindow(true);
	}
}

//��������
VOID CDlgRoomConfig::OnBnClickedBtRobotDebug()
{
	//Ȩ���ж�
	if (CUserRight::CanCheat(m_dwUserRight) == false)
	{
		MessageBox(TEXT("����ǰû�е���Ȩ�ޣ�"));
		return;
	}

	if (m_pIRobotDebug == NULL)
	{
		//��������
		TCHAR szDebugName[LEN_KIND * 2] = TEXT("");

		//��ȡ�������
		int nProcessNameLen = lstrlen(m_GameKind.szProcessName)+1;
		lstrcpyn(szDebugName, m_GameKind.szProcessName, nProcessNameLen - 4);
		_sntprintf(szDebugName, CountArray(szDebugName), TEXT("%sRobotDebug.dll"), szDebugName);

		//��ȡ����
		TCHAR szWorkPath[MAX_PATH] = TEXT("");
		CWHService::GetWorkDirectory(szWorkPath, sizeof(szWorkPath));

		//����·��
		TCHAR szFileName[MAX_PATH] = TEXT("");
		_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\%s"), szWorkPath, szDebugName);

		//�����ļ�
		DWORD dwAttrib = GetFileAttributes(szFileName);
		if (!(INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
		{
			CString strMessage;
			strMessage.Format(TEXT("������� %s �����ڣ�"), szFileName);
			MessageBox(strMessage);
			return;
		}

		//�������
		m_hRobotDebugInst = LoadLibraryEx(szFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

		//��ȡ����
		if (m_hRobotDebugInst)
		{
			typedef void* (*CREATE)();
			CREATE pFunction = (CREATE)GetProcAddress(m_hRobotDebugInst, "CreateRobotDebug");
			if (pFunction)
			{
				m_pIRobotDebug = static_cast<IClientDebug*>(pFunction());
				m_pIRobotDebug->Create(this, this);
			}
		}
		else
		{
			CString strMessage;
			strMessage.Format(TEXT("���������ʧ�ܣ�������� %d ��"), GetLastError());
			MessageBox(strMessage);
			return;
		}
	}

	// �ص�
	if (m_pIRobotDebug)
	{
		m_pIRobotDebug->ShowWindow(true);
	}
}

//������
VOID CDlgRoomConfig::OnBnClickedCardDispatcher()
{
	//Ȩ���ж�
	if (CUserRight::CanCheat(m_dwUserRight) == false)
	{
		MessageBox(TEXT("����ǰû�е���Ȩ�ޣ�"));
		return;
	}

	if (m_pICardDispatcher == NULL)
	{
		//��������
		TCHAR DebugName[LEN_KIND * 2] = _T("");
		TCHAR DebugFolder[LEN_KIND * 2] = _T("");
		//��ȡ�������
		size_t nProcessNameLen = _tcslen(m_GameKind.szProcessName);
		wcsncpy_s(DebugName, m_GameKind.szProcessName, nProcessNameLen - 4);
		wsprintf(DebugFolder, TEXT("%s"), DebugName);
		wsprintf(DebugName, TEXT("%s\\CardDispatcher.dll"), DebugName);

		//��ȡ����
		TCHAR szPath[MAX_PATH] = TEXT("");
		CWHService::GetWorkDirectory(szPath, sizeof(szPath));
		TCHAR szFileName[MAX_PATH] = TEXT("");
		_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\%s"), szPath, DebugName);

		DWORD dwAttrib = GetFileAttributes(szFileName);
		if (!(INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
		{
			CString str;
			str.Format(TEXT("��������� %s �����ڣ�"), szFileName);
			MessageBox(str);
			return;
		}

		m_hCardDispatcherInst = LoadLibraryEx(szFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

		if (m_hCardDispatcherInst)
		{
			typedef void* (*CREATE)();
			CREATE pFunction = (CREATE)GetProcAddress(m_hCardDispatcherInst, "CreateCardDispatcher");
			if (pFunction)
			{
				m_pICardDispatcher = static_cast<ICardDispatcher*>(pFunction());
				m_pICardDispatcher->Create(this, this, DebugFolder);
			}
		}
		else
		{
			DWORD dwLastError = GetLastError();
			CString str;
			str.Format(TEXT("���������ʧ�ܣ�������� %d ��"), dwLastError);
			MessageBox(str);
			return;
		}
	}
	// �ص�
	if (m_pICardDispatcher)
	{
		m_pICardDispatcher->ShowWindow(true);
	}
}

// ��������Ϣ
bool CDlgRoomConfig::OnCardDispatcherDebugInfo(WORD wTableID, void* pData, WORD nSize)
{
	if (m_TCPSocketModule.GetInterface() == NULL)
	{
		ASSERT(FALSE);
		return false;
	}
	//��������
	BYTE cbBuffer[SOCKET_BUFFER];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//��������
	CMD_GR_CardDispatcherDebug* pRoomDebug = (CMD_GR_CardDispatcherDebug*)cbBuffer;
	pRoomDebug->wTableID = wTableID;
	//������Ϣ
	if (nSize != 0)
	{
		CopyMemory(cbBuffer + sizeof(CMD_GR_CardDispatcherDebug), pData, nSize);
	}

	//��������
	WORD wSendSize = sizeof(CMD_GR_CardDispatcherDebug) + nSize;
	m_TCPSocketModule->SendData(MAKECMMD(MDM_GR_MANAGE, SERVICE_MODULE_GAME), SUB_GR_CARDDISPATCHERDEBUG, cbBuffer, wSendSize);

	return true;
}

//��ʾ��ť
VOID CDlgRoomConfig::EnableButton(bool bEnabled)
{
	GetDlgItem(IDC_BT_ENTER_ROOM)->EnableWindow(bEnabled);
	GetDlgItem(IDC_BT_EXIT_ROOM)->EnableWindow(!bEnabled);
	GetDlgItem(IDC_BT_GAME_DEBUG)->EnableWindow(!bEnabled);
	GetDlgItem(IDC_BT_ROBOT_DEBUG)->EnableWindow(!bEnabled);
	GetDlgItem(IDC_BT_CARD_DISPATCHER)->EnableWindow(!bEnabled);}

//���͵�¼
bool CDlgRoomConfig::SendLogonPacket()
{
	//��������
	CMD_GR_LogonComputer LogonUserID;
	ZeroMemory(&LogonUserID, sizeof(LogonUserID));

	//��������
	CGlobalUserInfo* pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	tagGlobalUserData* pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

	//��Ϸ�汾
	//LPCTSTR pszProcessName = m_GameKind.szProcessName;
	//CWHService::GetModuleVersion(pszProcessName, LogonUserID.dwProcessVersion);

	//������Ϣ
	LogonUserID.dwPlazaVersion = 0; // g_GlobalUnits.GetPlazaVersion();
	LogonUserID.dwFrameVersion = 0; // g_GlobalUnits.GetFrameVersion();
	GetProcessVersion(m_GameKind.wKindID, LogonUserID.dwProcessVersion);

	//��¼��Ϣ
	LogonUserID.dwUserID = pGlobalUserData->dwUserID;
	//��������
	TCHAR szPassword[LEN_MD5];
	CMD5Encrypt::EncryptData(TEXT("123456"), szPassword);
	//	lstrcpyn(LogonUserID.szServerPasswd, szPassword, CountArray(LogonUserID.szServerPasswd));
	lstrcpyn(LogonUserID.szPassword, pGlobalUserData->szPassword, CountArray(LogonUserID.szPassword));

	//��������
	m_TCPSocketModule->SendData(MAKECMMD(MDM_GR_LOGON, SERVICE_MODULE_GAME), SUB_GR_LOGON_COMPUTER, &LogonUserID, sizeof(LogonUserID));

	return true;
}

//��ȡ��Ϸ�汾
VOID CDlgRoomConfig::GetProcessVersion(WORD wKindID, DWORD& dwProcessVersion)
{
	if (wKindID == KIND_ID_XMOX)
		dwProcessVersion = 134217729;
	else if (wKindID == KIND_ID_TEEMPATTI)
		dwProcessVersion = 134217729;
	else if (wKindID == KIND_ID_DEALRUMMY)
		dwProcessVersion = 134217729;
	else if (wKindID == KIND_ID_WINGO)
		dwProcessVersion = 134217729;
	else if (wKindID == KIND_ID_SAWABALOOT)
		dwProcessVersion = 101187585;
	else if (wKindID == KIND_ID_SUPERTOUZI)
		dwProcessVersion = 134217729;
    else if (wKindID == KIND_ID_BALOOTCARD)
        dwProcessVersion = 134283265;
	else if (wKindID == KIND_ID_DRAGONTIGER)
		dwProcessVersion = 101187585;
	else if (wKindID == KIND_ID_ANIMALBATTLE)
		dwProcessVersion = 101187585;	
	else
		dwProcessVersion = 101187585;
}

//����״̬
VOID CDlgRoomConfig::SetServiceStatus(enServiceStatus ServiceStatus)
{
	//���ñ���
	m_ServiceStatus = ServiceStatus;

	return;
}

//��ȡ������Ϣ
void CDlgRoomConfig::GetConnectInfo()
{
	//��ȡ��Ϣ
	CGlobalServer GlobalServer;
	TCHAR szCurrentServer[LEN_SERVER] = TEXT("");
    GlobalServer.GetLastServerName(szCurrentServer);
	if (szCurrentServer[0] == 0)
		GlobalServer.GetServerItemName(0, szCurrentServer);

	tagServerItem ServerItem;
	ZeroMemory(&ServerItem, sizeof(ServerItem));

	//���ñ���
	m_wCurrentPort = PORT_LOGON;
	m_dwCurrentServer = INADDR_NONE;

	//��ȡ��Ϣ
	CParameterGlobal* pParameterGlobal = CParameterGlobal::GetInstance();
	bool bSuccess = GlobalServer.GetServerItemInfo(szCurrentServer, pParameterGlobal->m_NetworkKind, ServerItem);

	//ת����ַ
	if (bSuccess == true)
	{
		//ʹ������
		if (false)
		{
			LPHOSTENT lpHost = gethostbyname(CT2CA(ServerItem.szServerHost));
			if (lpHost != NULL)
			{
				ServerItem.dwServerAddr = ((LPIN_ADDR)lpHost->h_addr)->s_addr;

				//���µ�ַ
				if (ServerItem.szServerHost[0] != 0)
					GlobalServer.WriteServerItemInfo(szCurrentServer, pParameterGlobal->m_NetworkKind, ServerItem);
			}
		}

		//ʹ�õ�ַ
		if (ServerItem.dwServerAddr == INADDR_NONE)
		{
			CT2CA strServerDomain(ServerItem.szServerHost);
			ServerItem.dwServerAddr = inet_addr(strServerDomain);
		}
	}

	//���ַ���
	if (ServerItem.dwServerAddr == INADDR_NONE)
	{
		//��ַת��
		CT2CA CurrentServer(szCurrentServer);
		ServerItem.dwServerAddr = inet_addr(CurrentServer);

		//��������
		if (ServerItem.dwServerAddr == INADDR_NONE)
		{
			LPHOSTENT lpHost = gethostbyname(CurrentServer);
			if (lpHost != NULL) ServerItem.dwServerAddr = ((LPIN_ADDR)lpHost->h_addr)->s_addr;
		}
	}

	//�������
	if (ServerItem.dwServerAddr != INADDR_NONE)
	{
		//���ñ���
		m_wCurrentPort = ServerItem.wServerPort;
		m_dwCurrentServer = ServerItem.dwServerAddr;
	}
}

//�û�����
VOID CDlgRoomConfig::OnUserItemAcitve(IClientUserItem* pIClientUserItem)
{
	//�ж��Լ�
	if (m_pIMySelfUserItem == NULL)
	{
		m_pIMySelfUserItem = pIClientUserItem;
	}

	//��������
	BYTE cbUserStatus = pIClientUserItem->GetUserStatus();
	if ((cbUserStatus >= US_SIT) && (cbUserStatus != US_LOOKON))
	{
		WORD wTableID = pIClientUserItem->GetTableID();
		WORD wChairID = pIClientUserItem->GetChairID();
		//m_pITableViewFrame->SetClientUserItem(wTableID, wChairID, pIClientUserItem);
	}

	//�û�����
	OnEventUserEnter(pIClientUserItem);

	return;
}

//�û�����
VOID CDlgRoomConfig::OnEventUserEnter(IClientUserItem* pIClientUserItem)
{
	//�����Լ�
	//	if (m_pIMySelfUserItem == NULL)
	{
		//��������
		CGlobalUserInfo* pGlobalUserInfo = CGlobalUserInfo::GetInstance();

		//�Լ��ж�
		if (pGlobalUserInfo->GetGlobalUserData()->dwUserID == pIClientUserItem->GetUserID())
		{
			//���ñ���
			m_pIMySelfUserItem = pIClientUserItem;

			//���ý���
			m_UserListControl.SetMySelfUserID(m_pIMySelfUserItem->GetUserID());
			//m_UserListControl.SetServerRule(m_dwServerRule);
		}
	}

	//�����û�
	m_UserListControl.InsertUserItem(pIClientUserItem);
	//	m_UserListControl.SortItemData();

	//����״̬
	m_UserListControl.UpdateUserItem(pIClientUserItem);

	return;
}

//�û�ɾ��
VOID CDlgRoomConfig::OnUserItemDelete(IClientUserItem* pIClientUserItem)
{
	//��������
	WORD wLastTableID = pIClientUserItem->GetTableID();
	WORD wLastChairID = pIClientUserItem->GetChairID();
	DWORD dwLeaveUserID = pIClientUserItem->GetUserID();
	tagClientUserInfo* pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();

	//����״̬
	m_UserListControl.UpdateUserItem(pIClientUserItem);

	//�û�����
	OnEventUserLeave(pIClientUserItem);

	return;
}

//�û��뿪
VOID CDlgRoomConfig::OnEventUserLeave(IClientUserItem* pIClientUserItem)
{
	//��������
	DWORD dwLeaveUserID = pIClientUserItem->GetUserID();

	//ɾ���û�
	m_UserListControl.DeleteUserItem(pIClientUserItem);

	//����״̬
	m_UserListControl.UpdateUserItem(pIClientUserItem);


	return;
}
//�û�����
VOID CDlgRoomConfig::OnUserFaceUpdate(IClientUserItem* pIClientUserItem)
{
	//��������
	tagClientUserInfo* pUserInfo = pIClientUserItem->GetUserInfo();
	tagCustomFaceInfo* pCustomFaceInfo = pIClientUserItem->GetCustomFaceInfo();

	return;
}

//�û�����
VOID CDlgRoomConfig::OnUserItemUpdate(IClientUserItem* pIClientUserItem, const tagClientUserScore& LastScore)
{
	//��������
	tagClientUserInfo* pUserInfo = pIClientUserItem->GetUserInfo();
	tagClientUserInfo* pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();

	//����״̬
	m_UserListControl.UpdateUserItem(pIClientUserItem);

	return;
}

//�û�����
VOID CDlgRoomConfig::OnUserItemUpdate(IClientUserItem* pIClientUserItem, const tagUserStatus& LastStatus)
{
	//��������
	tagClientUserInfo* pUserInfo = pIClientUserItem->GetUserInfo();
	tagClientUserInfo* pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();
	WORD wNowTableID = pIClientUserItem->GetTableID(), wLastTableID = LastStatus.wTableID;
	WORD wNowChairID = pIClientUserItem->GetChairID(), wLastChairID = LastStatus.wChairID;
	BYTE cbNowStatus = pIClientUserItem->GetUserStatus(), cbLastStatus = LastStatus.cbUserStatus;

	//�û�����
	//m_pPlazaViewServerCtrl->OnEventUserUpdate(pIClientUserItem);

	//����״̬
	m_UserListControl.UpdateUserItem(pIClientUserItem);

	return;
}

////�û�����
//VOID CDlgRoomConfig::OnUserItemUpdate(IClientUserItem* pIClientUserItem, const tagUserSegment& LastSegment)
//{
//	//��������
//	WORD wMeTableID = m_pIMySelfUserItem->GetTableID();
//	WORD wUserTableID = pIClientUserItem->GetTableID();
//
//	return;
//}

// ������Ϣ
bool CDlgRoomConfig::SendDebugData(WORD nMessageID, WORD wTableID, void* pData, WORD wDataSize)
{
	//У������
	if (m_TCPSocketModule.GetInterface() == NULL)
	{
		return false;
	}

	//���嶨��
	BYTE cbBuffer[SOCKET_BUFFER];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//��ȡ�û�
	if (nMessageID == SUB_C_GET_SERVER_USER)
	{
		//��������
		WORD wIndex = 0;
		WORD wEnumIndex = 0;
		IClientUserItem* pIClientUserItem = NULL;

		//���鶨��
		tagDebugUserInfo UserInfoList[MAX_CHAIR];
		ZeroMemory(UserInfoList, sizeof(UserInfoList));

		do
		{
			pIClientUserItem = m_PlazaUserManagerModule->EnumUserItem(wEnumIndex++);
			if (pIClientUserItem != NULL)
			{
				UserInfoList[wIndex].bIsRobot = pIClientUserItem->IsRobot();
				UserInfoList[wIndex].dwUserID = pIClientUserItem->GetUserID();
				UserInfoList[wIndex].dwGameID = pIClientUserItem->GetGameID();
				UserInfoList[wIndex].lUserScore = pIClientUserItem->GetUserScore();
				UserInfoList[wIndex].lInsureScore = pIClientUserItem->GetInsureGold();
				//UserInfo.lHistoryKindIDWin[wIndex] = pIClientUserItem->GetUserInfo()->lHistoryKindIDWin;
				UserInfoList[wIndex].wTableID = pIClientUserItem->GetTableID();
				UserInfoList[wIndex].wChairID = pIClientUserItem->GetChairID();
				UserInfoList[wIndex].cbUserStatus = pIClientUserItem->GetUserStatus();
				lstrcpyn(UserInfoList[wIndex].szNickName, pIClientUserItem->GetAccounts(), LEN_NICKNAME);

				wIndex++;
			}
		} while (pIClientUserItem != NULL);

		if (wIndex > 0)
		{
			m_pIClientDebug->OnEventDebugMessage(SUB_S_SERVER_USER_LIST, wTableID,&UserInfoList, wIndex * sizeof(UserInfoList[0]));
		}

		return true;
	}

	//��������
	auto pDebugPacket = (CMD_GR_DebugPacket *)cbBuffer;

	//���ñ���
	pDebugPacket->wTableID = wTableID;
	pDebugPacket->wGameCmdID = nMessageID + SUB_GF_GAME_FRAME_MIN;	

	//��������
	if (wDataSize != 0)
	{
		CopyMemory(pDebugPacket+1, pData, wDataSize);
	}

	//��������
	m_TCPSocketModule->SendData(MAKECMMD(MDM_GR_DEBUG, SERVICE_MODULE_GAME), SUB_GR_DEBUG_PACKET, cbBuffer, sizeof(CMD_GR_DebugPacket)+ wDataSize);

	return true;
}

//�����б�����
VOID CDlgRoomConfig::CleanListData()
{
	m_UserListControl.DeleteAllItems();

	//ö���û�
	WORD wEnumIndex = 0;
	IClientUserItem* pIClientUserItem = NULL;
	do
	{
		pIClientUserItem = m_PlazaUserManagerModule->EnumUserItem(wEnumIndex);
		if (pIClientUserItem != NULL)
		{
			m_PlazaUserManagerModule->DeleteUserItem(pIClientUserItem);
		}
	} while (pIClientUserItem != NULL);

}


//�����¼�
bool CDlgRoomConfig::OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode)
{
	//������ʾ
	if (nErrorCode != 0L)
	{
		//�ر���ʾ
		//	m_DlgStatus.HideStatusWindow();

		//�رշ���
		PostMessage(WM_COMMAND, IDM_DELETE_SERVER_ITEM, 0);

		//��ʾ��Ϣ
		CInformation Information(AfxGetMainWnd());
		Information.ShowMessageBox(TEXT("�ܱ�Ǹ����Ϸ��������ʧ�ܣ����Ժ����Ի�������վ���棡"), MB_ICONINFORMATION, 0);
		return true;
	}

	//��������
	CMD_GG_C_LinkService linkService;
	ZeroMemory(&linkService, sizeof(linkService));

	//linkService.dwStationID = 1000;
	linkService.wServiceID = m_GameServer.wServerID;
	linkService.wServiceModule = SERVICE_MODULE_GAME;

	//��������
	m_TCPSocketModule->SendData(MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_C_LINK_SERVICE, &linkService, sizeof(linkService));

	////���͵�¼
	//SendLogonPacket();

	////����״̬
	//SetServiceStatus(ServiceStatus_Validate);

	return true;
}

//�ر��¼�
bool CDlgRoomConfig::OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason)
{
	//�رմ���
	if (cbShutReason != SHUT_REASON_NORMAL)
	{
		if (m_ServiceStatus == ServiceStatus_ServiceIng)
		{
			MessageBox(TEXT("���������ԭ�򣬷�������ʧ�ܣ����Ժ����³��ԣ�"));
		}
		else
		{
			MessageBox(TEXT("���������ԭ�򣬷�������ʧ�ܣ����Ժ����³��ԣ�"));
		}
	}
	else
	{
		CInformation Information(AfxGetMainWnd());
		Information.ShowMessageBox(TEXT("�����뿪���䣡"), MB_ICONINFORMATION, 0);
	}

	//����״̬
	m_ServiceStatus = ServiceStatus_NetworkDown;
	m_wTableID = INVALID_CHAIR;
	EnableButton(true);
	CleanListData();

	//�ͷž��
	ReleaseInterface(m_hRobotDebugInst, m_pIRobotDebug);
	ReleaseInterface(m_hClientDebugInst, m_pIClientDebug);
	ReleaseInterface(m_hCardDispatcherInst, m_pICardDispatcher);

	//�������
	TCHAR szServerTitle[128] = TEXT("");
	_sntprintf_s(szServerTitle, CountArray(szServerTitle), TEXT("��Ϸ����"));

	//���ñ���
	SetWindowText(szServerTitle);

	return true;
}

//��ȡ�¼�
bool CDlgRoomConfig::OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID* pData, WORD wDataSize)
{
	bool br = false;
	WORD wCmd = LOCMMD(Command.wMainCmdID);
	switch (wCmd)
	{
	case MDM_GR_LOGON:		//��¼��Ϣ
	{
		br = OnSocketMainLogon(Command.wSubCmdID, pData, wDataSize);

		return true;
	}
	case MDM_GR_CONFIG:		//������Ϣ
	{
		//br = OnSocketMainConfig(Command.wSubCmdID, pData, wDataSize);

		return true;
	}
	case MDM_GR_DEBUG:		//��������
	{
		br = OnSocketMainDebug(Command.wSubCmdID, pData, wDataSize);

		return true;
	}
	case MDM_GR_USER:		//�û���Ϣ
	{
		br = OnSocketMainUser(Command.wSubCmdID, pData, wDataSize);

		return true;
	}
	case MDM_GR_MANAGE:		//������Ϣ
	{
		br = OnSocketMainManager(Command.wSubCmdID, pData, wDataSize);

		return true;
	}
	case MDM_CM_SYSTEM:		//ϵͳ��Ϣ
	{
		if (Command.wSubCmdID == SUB_CM_SYSTEM_MESSAGE)
		{
			CMD_CM_SystemMessage* pSystemMessage = (CMD_CM_SystemMessage*)pData;

			::ReleaseCapture();
			CInformation Information(AfxGetMainWnd());
			Information.ShowMessageBox(pSystemMessage->szString, MB_ICONINFORMATION, 0);
		}

		return true;
	}	
	case MDM_GF_GAME:		//��Ϸ��Ϣ
	case MDM_GF_FRAME:		//�����Ϣ
	{
		br = OnSocketMainGameFrame(Command.wMainCmdID, Command.wSubCmdID, pData, wDataSize);

		return true;
	}
	case MDM_GG_GATE_SERVICE:		//�������
	{
		//���͵�¼
		SendLogonPacket();

		//����״̬
		SetServiceStatus(ServiceStatus_Validate);
	}
	}

	return true;
}

//��¼����
bool CDlgRoomConfig::OnSocketMainLogon(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_LOGON_SUCCESS:	//��¼�ɹ�
	{
		return OnSocketSubLogonSuccess(pData, wDataSize);
	}
	case SUB_GR_LOGON_FAILURE:	//��¼ʧ��
	{
		return OnSocketSubLogonFailure(pData, wDataSize);
	}
	case SUB_GR_LOGON_FINISH:	//��¼���
	{
		return OnSocketSubLogonFinish(pData, wDataSize);
	}
	case SUB_GR_UPDATE_NOTIFY:	//������ʾ
	{
		return OnSocketSubUpdateNotify(pData, wDataSize);
	}
	}

	return true;
}

//���Դ���
bool CDlgRoomConfig::OnSocketMainDebug(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GR_DEBUG_PACKET:		//�������ݰ�
	{
		return OnSocketSubDebugPacket(pData, wDataSize);
	}
	}

	return  true;
}

//��¼���
bool CDlgRoomConfig::OnSocketSubLogonFinish(VOID* pData, WORD wDataSize)
{
	//����У��
	//ASSERT(wDataSize == sizeof(CMD_GR_LogonFinish));
	//if (wDataSize != sizeof(CMD_GR_LogonFinish)) return false;

	////��ȡ����
	//CMD_GR_LogonFinish* pLogonFinish = (CMD_GR_LogonFinish*)pData;

	//����״̬
	SetServiceStatus(ServiceStatus_ServiceIng);

	return true;
}

//��¼�ɹ�
bool CDlgRoomConfig::OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GR_LogonSuccess));
	if (wDataSize != sizeof(CMD_GR_LogonSuccess)) return false;

	//��Ϣ����
	CMD_GR_LogonSuccess* pLogonSuccess = (CMD_GR_LogonSuccess*)pData;

	//�������
	m_dwUserRight = pLogonSuccess->dwUserRight;
	m_dwMasterRight = pLogonSuccess->dwMasterRight;

	//����״̬
	SetServiceStatus(ServiceStatus_RecvInfo);

	EnableButton(false);
	ReleaseInterface(m_hCardDispatcherInst, m_pICardDispatcher);

	return true;
}

//��¼ʧ��
bool CDlgRoomConfig::OnSocketSubLogonFailure(VOID* pData, WORD wDataSize)
{
	//Ч�����
	CMD_GR_LogonFailure* pLogonFailure = (CMD_GR_LogonFailure*)pData;
	ASSERT(wDataSize >= (sizeof(CMD_GR_LogonFailure) - sizeof(pLogonFailure->szDescribeString)));
	if (wDataSize < (sizeof(CMD_GR_LogonFailure) - sizeof(pLogonFailure->szDescribeString))) return false;

	//�رմ���
	m_TCPSocketModule->CloseSocket();

	//����״̬
	SetServiceStatus(ServiceStatus_NetworkDown);

	EnableButton(true);
	//��ʾ��Ϣ
	LPCTSTR pszDescribeString = pLogonFailure->szDescribeString;
	if (lstrlen(pszDescribeString) > 0)
	{
		MessageBox(pszDescribeString);

		CleanListData();
	}

	return true;
}

//������ʾ
bool CDlgRoomConfig::OnSocketSubUpdateNotify(VOID* pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GR_UpdateNotify));
	if (wDataSize != sizeof(CMD_GR_UpdateNotify)) return false;

	//��������
	CMD_GR_UpdateNotify* pUpdateNotify = (CMD_GR_UpdateNotify*)pData;
	if (pUpdateNotify->cbMustUpdatePlaza)
	{
		//��ʾ��Ϣ
		CInformation Information(AfxGetMainWnd());
		Information.ShowMessageBox(TEXT("�����汾��Ҫ���£�"), MB_ICONERROR);
	}
	else if (pUpdateNotify->cbMustUpdateClient)
	{
		//��ʾ��Ϣ
		CInformation Information(AfxGetMainWnd());
		Information.ShowMessageBox(TEXT("��Ϸ�汾��Ҫ���£�"), MB_ICONERROR);
	}

	return true;
}

//������
bool CDlgRoomConfig::OnSocketMainManager(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	//switch (wSubCmdID)
	//{
	//case SUB_GR_OPTION_CURRENT:	//��ǰ����
	//{
	//	//Ч�����
	//	ASSERT(wDataSize == sizeof(CMD_GR_OptionCurrent));
	//	if (wDataSize != sizeof(CMD_GR_OptionCurrent)) return false;

	//	//��Ϣ����
	//	if (m_pDlgServerManager != NULL)
	//	{
	//		CMD_GR_OptionCurrent* pOptionCurrent = (CMD_GR_OptionCurrent*)pData;
	//		m_pDlgServerManager->SetServerType(m_wServerType);
	//		m_pDlgServerManager->SetServerOptionInfo(pOptionCurrent->ServerOptionInfo, pOptionCurrent->dwRuleMask);
	//	}

	//	return true;
	//}
	//}

	return true;
}

//��Ϸ����
bool CDlgRoomConfig::OnSocketMainGameFrame(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize <= SOCKET_PACKET);
	if (wDataSize > SOCKET_PACKET) return false;

	switch (wSubCmdID)
	{
	case SUB_GF_SYSTEM_MESSAGE:		//ϵͳ��Ϣ
	{
		return OnSocketSubSystemMessage(pData, wDataSize);
	}

	return true;
	}

	return true;
}

//ϵͳ��Ϣ
bool CDlgRoomConfig::OnSocketSubSystemMessage(VOID* pData, WORD wDataSize)
{
	//��������
	CMD_CM_SystemMessage* pSystemMessage = (CMD_CM_SystemMessage*)pData;
	WORD wHeadSize = sizeof(CMD_CM_SystemMessage) - sizeof(pSystemMessage->szString);

	//Ч�����
	ASSERT((wDataSize > wHeadSize) && (wDataSize == (wHeadSize + pSystemMessage->wLength * sizeof(TCHAR))));
	if ((wDataSize <= wHeadSize) || (wDataSize != (wHeadSize + pSystemMessage->wLength * sizeof(TCHAR)))) return false;

	//��ʾ��Ϣ
	if ((pSystemMessage->wType & SMT_CHAT) /*&& (m_pIStringMessage != NULL)*/)
	{
		//m_ChatMessage.InsertSystemString(pSystemMessage->szString);
	}

	//������Ϣ
	if (pSystemMessage->wType & SMT_EJECT)
	{
		::ReleaseCapture();
		CInformation Information(AfxGetMainWnd());
		Information.ShowMessageBox(pSystemMessage->szString, MB_ICONINFORMATION, 0);
	}

	return true;
}

//�û�����
bool CDlgRoomConfig::OnSocketMainUser(WORD wSubCmdID, VOID* pData, WORD wDataSize)
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
	}

	return true;
}

//�û�����
bool CDlgRoomConfig::OnSocketSubUserEnter(VOID* pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize >= sizeof(tagPCUserInfoHead));
	if (wDataSize < sizeof(tagPCUserInfoHead)) return false;

	//��������
	tagClientUserInfo UserInfo;
	tagCustomFaceInfo CustomFaceInfo;
	ZeroMemory(&UserInfo, sizeof(UserInfo));
	ZeroMemory(&CustomFaceInfo, sizeof(CustomFaceInfo));

	//��Ϣ����
	tagPCUserInfoHead* pUserInfoHead = (tagPCUserInfoHead*)pData;

	//��������
	CGlobalUserInfo* pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	tagGlobalUserData* pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

	//��������	
	bool bMySelfUserItem = pGlobalUserData->dwUserID == pUserInfoHead->dwUserID;
	bool bMasterUserOrder = ((pUserInfoHead->cbMasterOrder > 0));

	//��ȡ��Ϣ
	if ((bMySelfUserItem == true) || (bMasterUserOrder == true))
	{
		//�û�����
		UserInfo.wFaceID = pUserInfoHead->wFaceID;
		UserInfo.dwGameID = pUserInfoHead->dwGameID;
		UserInfo.dwUserID = pUserInfoHead->dwUserID;
		UserInfo.dwCustomID = pUserInfoHead->dwCustomID;
		lstrcpyn(UserInfo.szAccounts, pGlobalUserData->szAccounts, CountArray(UserInfo.szAccounts));

		//�û�״̬
		UserInfo.wTableID = pUserInfoHead->wTableID;
		UserInfo.wChairID = pUserInfoHead->wChairID;
		UserInfo.cbUserStatus = pUserInfoHead->cbUserStatus;

		//�û�����
		UserInfo.cbGender = pUserInfoHead->cbGender;
		UserInfo.cbMasterOrder = pUserInfoHead->cbMasterOrder;

		//�û�����
		UserInfo.lGold = pUserInfoHead->lGold;
		UserInfo.lScore = pUserInfoHead->lScore;
		UserInfo.lInsureGold = pUserInfoHead->lInsureGold;
		UserInfo.lWinCount = pUserInfoHead->lWinCount;
		UserInfo.lLostCount = pUserInfoHead->lLostCount;
		UserInfo.lDrawCount = pUserInfoHead->lDrawCount;
		UserInfo.lFleeCount = pUserInfoHead->lFleeCount;
		UserInfo.lExperience = pUserInfoHead->lExperience;
		//UserInfo.lLoveHeart = pUserInfoHead->lLoveHeart;
		//UserInfo.lIntegralCount = pUserInfoHead->lIntegralCount;
		//UserInfo.lHistoryKindIDWin = pUserInfoHead->lHistoryKindIDWin;		
	}
	else
	{
		//�û���Ϣ
		UserInfo.dwUserID = pUserInfoHead->dwUserID;
		UserInfo.dwGameID = pUserInfoHead->dwGameID;
		UserInfo.bIsRobot = pUserInfoHead->bIsRobot;
		
		//�û�����
		UserInfo.cbGender = pUserInfoHead->cbGender;		
		UserInfo.cbMasterOrder = pUserInfoHead->cbMasterOrder;

		//�û�״̬
		UserInfo.wTableID = pUserInfoHead->wTableID;
		UserInfo.wChairID = pUserInfoHead->wChairID;
		UserInfo.cbUserStatus = pUserInfoHead->cbUserStatus;

		//�û�����
		UserInfo.cbGender = pUserInfoHead->cbGender;
		UserInfo.cbMasterOrder = pUserInfoHead->cbMasterOrder;

		//�û�����
		UserInfo.lGold = pUserInfoHead->lGold;
		UserInfo.lScore = pUserInfoHead->lScore;
		UserInfo.lInsureGold = pUserInfoHead->lInsureGold;
		UserInfo.lWinCount = pUserInfoHead->lWinCount;
		UserInfo.lLostCount = pUserInfoHead->lLostCount;
		UserInfo.lDrawCount = pUserInfoHead->lDrawCount;
		UserInfo.lFleeCount = pUserInfoHead->lFleeCount;
		UserInfo.lExperience = pUserInfoHead->lExperience;

        //��������
        VOID* pDataBuffer = NULL;
        tagDataDescribe DataDescribe;
        CRecvPacketHelper RecvPacket(pUserInfoHead + 1, wDataSize - sizeof(tagPCUserInfoHead));

        //��չ��Ϣ
        while (true)
        {
        	pDataBuffer = RecvPacket.GetData(DataDescribe);
        	if (DataDescribe.wDataDescribe == DTP_NULL) break;
        	switch (DataDescribe.wDataDescribe)
        	{
            case DTP_GR_USER_ACCOUNTS:		//�û��ǳ�
            {
            	ASSERT(pDataBuffer != NULL);
            	ASSERT(DataDescribe.wDataSize <= sizeof(UserInfo.szAccounts));
            	if (DataDescribe.wDataSize <= sizeof(UserInfo.szAccounts))
            	{
            		CopyMemory(&UserInfo.szAccounts, pDataBuffer, DataDescribe.wDataSize);
            		UserInfo.szAccounts[CountArray(UserInfo.szAccounts) - 1] = 0;
            	}
            	break;
            }
            default:
            	break;
            }
        }
	}

	//�����û�
	IClientUserItem* pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(UserInfo.dwUserID);
	if (pIClientUserItem == NULL) pIClientUserItem = m_PlazaUserManagerModule->ActiveUserItem(UserInfo, CustomFaceInfo);

	return true;
}

//�û�����
bool CDlgRoomConfig::OnSocketSubUserScore(VOID* pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GR_UserScore));
	if (wDataSize < sizeof(CMD_GR_UserScore)) return false;

	//Ѱ���û�
	CMD_GR_UserScore* pUserScore = (CMD_GR_UserScore*)pData;
	IClientUserItem* pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(pUserScore->dwUserID);

	//�û��ж�
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL) return true;

	//��������
	CGlobalUserInfo* pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	tagGlobalUserData* pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

	//��������
	bool bMySelfUserItem = pGlobalUserData->dwUserID == pUserScore->dwUserID;
	//bool bAvertDebugMode = CServerRule::IsAllowAvertDebugMode(m_dwServerRule);
	bool bMasterUserOrder = ((pIClientUserItem->GetMasterOrder() > 0) || (m_pIMySelfUserItem->GetMasterOrder() > 0));

	//�����û�
	if ((bMySelfUserItem == true) || (bMasterUserOrder == true))
	{
		m_PlazaUserManagerModule->UpdateUserItemScore(pIClientUserItem, &pUserScore->UserScore);
	}

	return true;
}

//�û�״̬
bool CDlgRoomConfig::OnSocketSubUserStatus(VOID* pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(CMD_GR_UserStatus));
	if (wDataSize < sizeof(CMD_GR_UserStatus)) return false;

	//Ѱ���û�
	CMD_GR_UserStatus* pUserStatus = (CMD_GR_UserStatus*)pData;

	//��������
	m_wTableID = pUserStatus->UserStatus.wTableID;

	IClientUserItem* pIClientUserItem = m_PlazaUserManagerModule->SearchUserByUserID(pUserStatus->dwUserID);

	//�û��ж�
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL) return true;

	//֮ǰ״̬
	tagUserStatus LastUserStatus;
	ZeroMemory(&LastUserStatus, sizeof(LastUserStatus));
	LastUserStatus.wTableID = pIClientUserItem->GetTableID();
	LastUserStatus.wChairID = pIClientUserItem->GetChairID();
	LastUserStatus.cbUserStatus = pIClientUserItem->GetUserStatus();

	//����״̬
	if (pUserStatus->UserStatus.cbUserStatus == US_NULL)
	{
		//ɾ���û�
		m_PlazaUserManagerModule->DeleteUserItem(pIClientUserItem);

		////��ȡ����
		//CServerListData * pServerListData = CServerListData::GetInstance();

		////��������
		//pServerListData->SetServerOnLineCount(m_GameServer.wServerID, m_PlazaUserManagerModule->GetActiveUserCount());
	}
	else
	{
		//�����û�
		m_PlazaUserManagerModule->UpdateUserItemStatus(pIClientUserItem, &pUserStatus->UserStatus);

		//CString strUserGameID;
		//strUserGameID.Format(TEXT("[GAMEID_%d]"), pIClientUserItem->GetGameID());
		//m_ChatMessage.InsertUserStatus(pIClientUserItem->GetNickName(), strUserGameID, pUserStatus->UserStatus, LastUserStatus);
	}

	//if (m_pIClientDebug)m_pIClientDebug->OnEventUserStatus(pIClientUserItem);

	return true;
}

//�������ݰ�
bool CDlgRoomConfig::OnSocketSubDebugPacket(VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pDebugPacket = (CMD_GR_DebugPacket*)pData;

	//��Ϸ����
	if (pDebugPacket->wDebugKind == DEBUG_KIND_GAME_CONTROL)
	{
		if (m_pIClientDebug != NULL)
		{
			m_pIClientDebug->OnEventDebugMessage(pDebugPacket->wGameCmdID, pDebugPacket->wTableID, pDebugPacket+1,wDataSize-sizeof(CMD_GR_DebugPacket));
		}
	}

	//��������
	if (pDebugPacket->wDebugKind == DEBUG_KIND_ROBOT_CONFIG)
	{
		if (m_pIRobotDebug != NULL)
		{
			m_pIRobotDebug->OnEventDebugMessage(pDebugPacket->wGameCmdID, pDebugPacket->wTableID, pDebugPacket + 1, wDataSize - sizeof(CMD_GR_DebugPacket));
		}
	}

	return true;
}

//�Ҽ��б�
VOID CDlgRoomConfig::OnNMRclickUserList(NMHDR* pNMHDR, LRESULT* pResult)
{
	//��������
	NMITEMACTIVATE* pListNotify = (NMITEMACTIVATE*)pNMHDR;

	//�����˵�
	if (pListNotify->iItem != -1)
	{
		//ѡ����Ŀ
		UINT nSelectCount = m_UserListControl.GetSelectedCount();

		//ѡ����
		if (nSelectCount > 1L)
		{
			//�û�����
			IClientUserItem* pIClientUserItem[32];
			ZeroMemory(pIClientUserItem, sizeof(pIClientUserItem));

			//��������
			WORD wUserCount = 0;
			POSITION nSelectPos = m_UserListControl.GetFirstSelectedItemPosition();

			//ѡ���ռ�
			while ((nSelectPos != NULL) && (wUserCount < CountArray(pIClientUserItem)))
			{
				//��ȡѡ��
				INT nSelectItem = m_UserListControl.GetNextSelectedItem(nSelectPos);

				//�����û�
				DWORD_PTR lItemData = m_UserListControl.GetItemData(nSelectItem);
				if (lItemData != NULL) pIClientUserItem[wUserCount++] = ((IClientUserItem*)(lItemData));
			};

			//�����˵�
			//if (wUserCount > 0) TrackUserItemMenu(pIClientUserItem, wUserCount);
		}
		else
		{
			//�����˵�
			DWORD_PTR lItemData = m_UserListControl.GetItemData(pListNotify->iItem);
			if (lItemData != NULL) TrackUserItemMenu((IClientUserItem*)(lItemData));
		}
	}

	return;
}

//��ʾ�˵�
VOID CDlgRoomConfig::TrackUserItemMenu(IClientUserItem* pIClientUserItem)
{
	//Ч�����
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL) return;

	//���ñ���
	//m_MenuUserItemArray.RemoveAll();
	//m_MenuUserItemArray.Add(pIClientUserItem);

	//����˵�
	CSkinMenu UserInfoMenu;
	UserInfoMenu.CreateMenu();

	//��������
	TCHAR szMenuString[256] = TEXT("");
	bool bMeUserItem = (pIClientUserItem == m_pIMySelfUserItem);

	//��������
	tagClientUserInfo * pUserInfo = pIClientUserItem->GetUserInfo();
	tagClientUserInfo* pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();
	/*IClientUserItem * pIChatTargetItem = m_ChatControl.GetChatTargetUserItem();*/

	//����ָ�
	UserInfoMenu.AppendSeparator();

	//����˵�
	UserInfoMenu.AppendMenu(IDM_COPY_USER_NAME, TEXT("����ID"));

	//��Ա�˵�
	if (!bMeUserItem/*&& m_pIClientKernel->GetServerAttribute()->wServerType != GAME_GENRE_MATCH*/)
	{
		//����ָ�
		UserInfoMenu.AppendSeparator();

		//��������
		BYTE  cbMeUserStatus = m_pIMySelfUserItem->GetUserStatus();
		BYTE  cbUserStatus = pIClientUserItem->GetUserStatus();
		bool bEnableKick = true;
		//	if (pMeUserInfo->cbMemberOrder <= pUserInfo->cbMemberOrder ||/* (cbMeUserStatus != US_SIT&&cbMeUserStatus != US_READY) ||*/
		//		(cbUserStatus != US_SIT&&cbUserStatus != US_READY))
		//		bEnableKick = false;

		//�Թ۲˵�
		_sntprintf_s(szMenuString, CountArray(szMenuString), TEXT("�� [ %s ] �뿪"), pIClientUserItem->GetAccounts());
		UserInfoMenu.AppendMenu(IDM_KILL_USER, szMenuString, bEnableKick ? MF_ENABLED : MF_GRAYED);

        //����ָ�
        UserInfoMenu.AppendSeparator();

		//��ɢ����
		UserInfoMenu.AppendMenu(IDM_DISMISS_TABLE, TEXT("��ɢ����"), bEnableKick ? MF_ENABLED : MF_GRAYED);
	}

	//�����˵�
	UserInfoMenu.TrackPopupMenu(this);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
