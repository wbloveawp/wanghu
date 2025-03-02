#include "StdAfx.h"
#include "Resource.h"
#include "GlobalUnits.h"
#include "DlgSessionServer.h"
#include	"DataDefine.h"
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

BEGIN_MESSAGE_MAP(CDlgSessionServer, CDlgSessionItem)
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
CDlgSessionServer::CDlgSessionServer() : CDlgSessionItem(IDD_SERVER_VIEW)
{
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
CDlgSessionServer::~CDlgSessionServer()
{
	ReleaseInterface(m_hRobotDebugInst, m_pIRobotDebug);
	ReleaseInterface(m_hClientDebugInst, m_pIClientDebug);
	ReleaseInterface(m_hCardDispatcherInst, m_pICardDispatcher);
}

//�ӿڲ�ѯ
VOID* CDlgSessionServer::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IRoomConfig, Guid, dwQueryVer);
	QUERYINTERFACE(IUserManagerSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IRoomConfig, Guid, dwQueryVer);
	return NULL;
}


//�ؼ���
VOID CDlgSessionServer::DoDataExchange(CDataExchange * pDX)
{
	//�󶨿ؼ�
	DDX_Control(pDX, IDC_COMBO_GAME_LIST, m_cbGameList);
	DDX_Control(pDX, IDC_COMBO_SERVER_LIST, m_cbServerList);
	DDX_Control(pDX, IDC_USER_LIST_CONTROL, m_UserListControl);

	__super::DoDataExchange(pDX);
}

//���ú���
BOOL CDlgSessionServer::OnInitDialog()
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

//�б����
VOID CDlgSessionServer::OnEventListFinish()
{
	//��������
	POSITION Position = NULL;
	CGameKindItem* pGameKindItem = NULL;

	//�������
	while (m_cbGameList.GetCount()) m_cbGameList.DeleteString(0);

	do
	{
		pGameKindItem = m_pServerListManager->EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//��������
		tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemDataPtr(m_cbGameList.AddString(pGameKindItem->m_GameKind.szKindName));
		if (pItemData != NULL) pItemData->dwDataPtr = pGameKindItem->m_GameKind.wKindID;

	} while (Position != NULL);
}

//������Ϣ
BOOL CDlgSessionServer::OnCommand(WPARAM wParam, LPARAM lParam)
{
	WORD wLCommandID = LOWORD(wParam);
	WORD wHCommandID = HIWORD(wParam);

	switch (wHCommandID)
	{
	case CBN_SELCHANGE:
	{
		//��Ϸ�б�
		if (m_cbGameList.m_hWnd == (HWND)lParam)
		{
			//��������
			CWordArray KindArray;
			for (INT i = 0; i < m_cbGameList.GetCount(); i++)
			{
				if (m_cbGameList.GetCheck(i) == static_cast<BOOL>(true))
				{
					tagItemData* pItemData = (tagItemData*)m_cbGameList.GetItemDataPtr(i);
					KindArray.Add(pItemData->dwDataPtr);
				}
			}

			//ɾ������				
			while (m_cbServerList.GetCount()) m_cbServerList.DeleteString(0);

			//���뷿��
			bool bInsert = false;
			POSITION Position = NULL;
			CGameServerItem* pGameServerItem = NULL;
			do
			{
				pGameServerItem = m_pServerListManager->EmunGameServerItem(Position);
				if (pGameServerItem != NULL)
				{
					//���ñ�ʶ
					bInsert = false;

					//ƥ������
					for (INT_PTR i = 0; i < KindArray.GetCount(); i++)
					{
						if (pGameServerItem->m_GameServer.wKindID == KindArray[i])
						{
							bInsert = true;
							break;
						}
					}

					if (bInsert == true)
					{
						m_cbServerList.SetItemData(m_cbServerList.AddString(pGameServerItem->m_GameServer.szServerName), pGameServerItem->m_GameServer.wServerID);
					}

				}
			} while (Position != NULL);
		}

		return TRUE;
	}
	}

	switch (wLCommandID)
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
			m_pVirtualLinkItem->SendVirtualData(MDM_GR_MANAGE, SUB_GR_KILL_USER, &KickUser, sizeof(KickUser));
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
			m_pVirtualLinkItem->SendVirtualData(MDM_GR_MANAGE, SUB_GR_DISMISSGAME, &DismissGame, sizeof(DismissGame));
        }

        return TRUE;
	}
	case IDM_SEND_CARD:
	{
		//��ȡ�û�
		IClientUserItem * pIClientUserItem = NULL;

		//ѡ������
		POSITION nSelectPos = m_UserListControl.GetFirstSelectedItemPosition();
		INT nSelectItem = m_UserListControl.GetNextSelectedItem(nSelectPos);
		DWORD_PTR lItemData = m_UserListControl.GetItemData(nSelectItem);
		if (lItemData != NULL) pIClientUserItem = ((IClientUserItem *)(lItemData));
		if (pIClientUserItem == NULL)
		{
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("��Ҳ����ڣ�"), MB_ICONINFORMATION);
			return TRUE;
		}
		CGameServerItem * pGameServerItem = NULL;
		pGameServerItem = m_pServerListManager->SearchGameServer(m_GameServer.wServerID);
		if (pGameServerItem == NULL)
		{
			MessageBox(TEXT("��ǰѡ���ServerΪ�գ�"));
			return TRUE;
		}
		//��ȡ����
		WORD wTableID = pIClientUserItem->GetTableID();

		WORD wTypeID = pGameServerItem->m_pGameKindItem->m_GameKind.wTypeID;

		//��������
		BYTE cbBuffer[SOCKET_BUFFER];
		ZeroMemory(cbBuffer, sizeof(cbBuffer));

		if (wTypeID == 3)
		{
			//�齫����
//#ifdef  DEBUG

			// ���Ͷ���
			typedef BYTE(_cdecl * SHOWMAHJONG)(CWnd *, WORD, CString *, int, BYTE *, BYTE *);

			// ��̬����
			SHOWMAHJONG ShowMahjong = NULL;
			static HMODULE hTestDLL = NULL;

			// ״̬�ж�
			if (hTestDLL != NULL) return TRUE;

			// ��ǰĿ¼
			TCHAR szDirectory[MAX_PATH];
			GetCurrentDirectory(sizeof(szDirectory), szDirectory);

			// ����Ŀ¼
			CString strDirectory;

			//�����ļ���
			TCHAR szPath[MAX_PATH] = TEXT("");
			TCHAR szOutBuf[MAX_PATH] = TEXT("");
			CString strApp = TEXT("Config");
			CString strConfigFileName;
			GetCurrentDirectory(sizeof(szPath), szPath);
			strConfigFileName.Format(TEXT("%s\\SparrowConfig.ini"), szPath);

			//�����
			BYTE cbMaxCardCount = GetPrivateProfileInt(strApp, TEXT("MaxRepertory"), 144, strConfigFileName);
			const WORD wGamePlayerCount = GetPrivateProfileInt(strApp, TEXT("GamePlayer"), 4, strConfigFileName);

			strDirectory.Format(TEXT("%s\\"), szDirectory);
			//strDirectory.Append(TEXT(MODULE_NAME));

			CGameServerItem * pGameServerItem = NULL;
			pGameServerItem = m_pServerListManager->SearchGameServer(m_GameServer.wServerID);
			if (pGameServerItem == NULL)
			{
				MessageBox(TEXT("��ǰѡ���ServerΪ�գ�"));
				return TRUE;
			}

			strDirectory.Append(pGameServerItem->m_pGameKindItem->m_GameKind.szKindName);
			SetCurrentDirectory(strDirectory);

			// �������
			hTestDLL = LoadLibrary(TEXT("MahjongClient.dll"));

			DWORD dwError = GetLastError();

			if (hTestDLL == NULL)
			{
				return TRUE;
			}

			// ��ȡ��ַ
			ShowMahjong = (SHOWMAHJONG)GetProcAddress(hTestDLL, "ShowMahjongDlg");
			if (ShowMahjong == NULL)
			{
				FreeLibrary(hTestDLL);
				return TRUE;
			}

			WORD wBankerUser = pIClientUserItem->GetChairID();//(WORD)wParam;
			//for (int i = 0; i < GAME_PLAYER; i++)
			//{
			//	if (wBankerUser == SwitchViewChairID(i))
			//	{
			//		wBankerUser = i;
			//		break;
			//	}
			//}
			CString strName[4];
			WORD wIndex = 0;
			IClientUserItem * pCurrentUserItem = m_PlazaUserManagerModule->EnumUserItem(wIndex);
			while (pCurrentUserItem != NULL)
			{

				if (pCurrentUserItem->GetTableID() == wTableID)
				{
					// ��������
					for (int i = 0; i < 4; i++)
					{

						if (pCurrentUserItem->GetChairID()==i)
						{
							strName[i] = pCurrentUserItem->GetAccounts();
							break;
						}
					}
				}
				pCurrentUserItem = m_PlazaUserManagerModule->EnumUserItem(++wIndex);
			}

			// ��ʾ����
			BYTE cbMagicData[6] = { 0 };
			BYTE cbCardData[MAX_REPERTORY_CNT] = { 0 };
			BOOL cbControlGameCount = ShowMahjong(this, wBankerUser, strName, wGamePlayerCount, cbCardData, cbMagicData);

			//// �ͷ�ģ��
			FreeLibrary(hTestDLL);
			hTestDLL = NULL;

			// ��ԭĿ¼
			SetCurrentDirectory(szDirectory);

			// ��������
			if (cbControlGameCount > 0)
			{
				// ����ṹ
				CMD_GR_SendCard SendCard;
				SendCard.wBankerUser = wBankerUser;
				SendCard.cbCardCount = cbMaxCardCount;
				SendCard.cbControlGameCount = cbControlGameCount;
				CopyMemory(SendCard.cbCardData, cbCardData, sizeof(BYTE) * SendCard.cbCardCount);

				// ��������
				CopyMemory(SendCard.cbMagicIndex, cbMagicData, sizeof(SendCard.cbMagicIndex));
				SendCard.cbMagicOpenPos = cbMagicData[4];

				//��������
				BYTE cbBuffer[SOCKET_BUFFER];
				ZeroMemory(cbBuffer, sizeof(cbBuffer));

				//��������
				CMD_GR_CardDispatcherDebug * pRoomDebug = (CMD_GR_CardDispatcherDebug *)cbBuffer;
				pRoomDebug->wTableID = wTableID;
				//������Ϣ
				if (sizeof(SendCard) != 0)
				{
					CopyMemory(cbBuffer + sizeof(CMD_GR_CardDispatcherDebug), &SendCard, sizeof(SendCard));
				}

				//��������
				WORD wSendSize = sizeof(CMD_GR_CardDispatcherDebug) + sizeof(SendCard);
				m_pVirtualLinkItem->SendVirtualData(MDM_GR_MANAGE, SUB_GR_CARDDISPATCHERDEBUG, cbBuffer, wSendSize);

			}
		}
		else if (wTypeID == 2)
		{
			//�˿�����
				//�������
				CString filePath;
				TCHAR szFilter[] = { TEXT("�����ļ�(*.acd)|*.acd||") };
				CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, NULL);

				//ѡ���ļ�
				if (dlg.DoModal() == IDOK)
				{
					//��ȡ�ļ���
					filePath = dlg.GetPathName();

					//���ļ�
					CFile file;
					if (file.Open(filePath, CFile::modeRead) == TRUE)
					{
						//�ļ�ͷ
						tagFileHead FileHead;
						file.Read(&FileHead, sizeof(FileHead));

						//�ļ���ʽ�ж�
						if (FileHead.cbFlags1 != 'W'
							|| FileHead.cbFlags2 != 'H'
							|| FileHead.cbFlags3 != 'A'
							|| FileHead.cbFlags4 != 'C')
						{
							//�رվ��
							file.Close();

							//������ʾ
							AfxMessageBox(TEXT("���������ļ����ݸ�ʽ����ȷ����ѡ����ȷ�������ļ���"));

							return 0;
						}

						//�汾�ж�
						if (FileHead.dwVesion != VESION)
						{
							//�رվ��
							file.Close();

							//������ʾ
							TCHAR szMsg[128];
							_sntprintf(szMsg, 128, TEXT("���������ļ��汾����ȷ����ѡ����ȷ�汾�������ļ������ߵ�ǰ�汾��%ld ��"), VESION);
							AfxMessageBox(szMsg);
							return 0;
						}

						//������Ϣ
						tagAllotCardInfo AllotCardInfo;
						file.Read(&AllotCardInfo, sizeof(AllotCardInfo));

						//��������
						tagConfigCard AllotCard;
						ZeroMemory(&AllotCard, sizeof(AllotCard));

						//��������
						BYTE cbCount = 0;
						for (WORD i = 0; i < AllotCardInfo.dwChairCount; i++)
						{
							//��ȡ����
							tagChairCardData CardData;
							file.Read(&CardData, sizeof(CardData));

							//�ж�����
							if (CardData.wCardCount >= MAX_GAME_HAND_CARD)
							{
								//�رվ��
								file.Close();

								//������ʾ
								AfxMessageBox(TEXT("2�������ݲ���ȷ����ѡ����ȷ�������ļ���"));

								return 0;
							}

							//��������
							CopyMemory(&AllotCard.cbCardData[cbCount], CardData.cbCardData, sizeof(BYTE)* CardData.wCardCount);
							cbCount += CardData.wCardCount;
						}

						if (AllotCardInfo.cbDipaiCard == 1)
						{
							//��ȡ����
							tagChairCardData CardData;
							file.Read(&CardData, sizeof(CardData));
							
							//��������
							CopyMemory(&AllotCard.cbCardData[cbCount], CardData.cbCardData, sizeof(BYTE) * CardData.wCardCount);
							cbCount += CardData.wCardCount;
						}

						AllotCard.bAllotFlag = TRUE;

						//��������
						CMD_GR_CardDispatcherDebug * pRoomDebug = (CMD_GR_CardDispatcherDebug *)cbBuffer;
						pRoomDebug->wTableID = wTableID;
						//������Ϣ
						if (sizeof(AllotCard) != 0)
						{
							CopyMemory(cbBuffer + sizeof(CMD_GR_CardDispatcherDebug), &AllotCard, sizeof(AllotCard));
						}

						//��������
						WORD wSendSize = sizeof(CMD_GR_CardDispatcherDebug) + sizeof(AllotCard);
						m_pVirtualLinkItem->SendVirtualData(MDM_GR_MANAGE, SUB_GR_CARDDISPATCHERDEBUG, cbBuffer, wSendSize);

						//�رվ��
						file.Close();
					}

				}
				else
				{
					//��������
					tagConfigCard AllotCard;
					ZeroMemory(&AllotCard, sizeof(AllotCard));

					//������Ϣ
					AllotCard.bAllotFlag = FALSE;

					//��������
					CMD_GR_CardDispatcherDebug * pRoomDebug = (CMD_GR_CardDispatcherDebug *)cbBuffer;
					pRoomDebug->wTableID = wTableID;
					//������Ϣ
					if (sizeof(AllotCard) != 0)
					{
						CopyMemory(cbBuffer + sizeof(CMD_GR_CardDispatcherDebug), &AllotCard, sizeof(AllotCard));
					}

					//��������
					WORD wSendSize = sizeof(CMD_GR_CardDispatcherDebug) + sizeof(AllotCard);
					m_pVirtualLinkItem->SendVirtualData(MDM_GR_MANAGE, SUB_GR_CARDDISPATCHERDEBUG, cbBuffer, wSendSize);
				}
		}
		return TRUE;
//#endif
	}
	default:
		break;
	}

	return __super::OnCommand(wParam, lParam);
}

//////////////////////////////////////////////////////////////////////
//��ť�¼�
//���뷿��
VOID CDlgSessionServer::OnBnClickedBtEnterRoom()
{
    //��ȡ������Ϣ
    GetConnectInfo();

	//У�鷿��
	if (m_cbServerList.GetCurSel() == -1)
	{
		MessageBox(TEXT("û��ѡ����Ϸ��������"));
		return;
	}

	//��ȡ��ʶ
	WORD wServerID = m_cbServerList.GetItemData(m_cbServerList.GetCurSel());
	
	//��������
	POSITION Position = NULL;
	CGameServerItem* pGameServerItem = NULL;

	pGameServerItem = m_pServerListManager->SearchGameServer(wServerID);
	if (pGameServerItem == NULL)
	{
		MessageBox(TEXT("��ǰѡ���ServerΪ�գ�"));
		return;
	}

	m_GameServer = pGameServerItem->m_GameServer;

	//��������
	CGlobalUserInfo* pGlobalUserInfo = CGlobalUserInfo::GetInstance();
	tagGlobalUserData* pGlobalUserData = pGlobalUserInfo->GetGlobalUserData();

	//��������
	m_bActiveStatus = true;

	//��������
	m_pVirtualLinkItem->CreateVirtualLink(wServerID, pGlobalUserData->dwUserID, pGlobalUserData->szPassword, NULL, 0);

	//��������
	CGameKindItem* pGameKindItem = m_pServerListManager->SearchGameKind(pGameServerItem->m_GameServer.wKindID);

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
VOID CDlgSessionServer::OnBnClickedBtExitRoom()
{
	//�ر�����
	m_pVirtualLinkItem->CloseVirtualLink();

	EnableButton(TRUE);
	ReleaseInterface(m_hCardDispatcherInst, m_pICardDispatcher);

	CleanListData();
}

//��Ϸ����
VOID CDlgSessionServer::OnBnClickedBtGameDebug()
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
VOID CDlgSessionServer::OnBnClickedBtRobotDebug()
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
VOID CDlgSessionServer::OnBnClickedCardDispatcher()
{
	////Ȩ���ж�
	//if (CUserRight::CanCheat(m_dwUserRight) == false)
	//{
	//	MessageBox(TEXT("����ǰû�е���Ȩ�ޣ�"));
	//	return;
	//}

//#ifdef  DEBUG

//	// ���Ͷ���
//	typedef BYTE(_cdecl * SHOWMAHJONG)(CWnd *, WORD, CString *, int, BYTE *, BYTE *);
//
//	// ��̬����
//	SHOWMAHJONG ShowMahjong = NULL;
//	static HMODULE hTestDLL = NULL;
//
//	// ״̬�ж�
//	if (hTestDLL != NULL) return ;
//
//	// ��ǰĿ¼
//	TCHAR szDirectory[MAX_PATH];
//	GetCurrentDirectory(sizeof(szDirectory), szDirectory);
//
//	// ����Ŀ¼
//	CString strDirectory;
//	// strDirectory.Format(TEXT("%s\\MODULE_NAME"),szDirectory);
//
//	// Add by humorly 2018/4/22
//	// ���·��
////#define MODULE_NAME					"SparrowSQ"
//#define GAME_PLAYER					4
////#define MAX_REPERTORY				108
//	strDirectory.Format(TEXT("%s\\"), szDirectory);
//	//strDirectory.Append(TEXT(MODULE_NAME));
//
//	CGameServerItem * pGameServerItem = NULL;
//	pGameServerItem = m_pServerListManager->SearchGameServer(m_GameServer.wServerID);
//	if (pGameServerItem == NULL)
//	{
//		MessageBox(TEXT("��ǰѡ���ServerΪ�գ�"));
//		return;
//	}
//
//	strDirectory.Append(pGameServerItem->m_pGameKindItem->m_GameKind.szKindName);
//	SetCurrentDirectory(strDirectory);
//
//	// �������
//	hTestDLL = LoadLibrary(TEXT("MahjongClient.dll"));
//
//	DWORD dwError = GetLastError();
//
//	if (hTestDLL == NULL)
//	{
//		return ;
//	}
//
//	// ��ȡ��ַ
//	ShowMahjong = (SHOWMAHJONG)GetProcAddress(hTestDLL, "ShowMahjongDlg");
//	if (ShowMahjong == NULL)
//	{
//		FreeLibrary(hTestDLL);
//		return ;
//	}
//
//	WORD wBankerUser = 0;//(WORD)wParam;
//	//for (int i = 0; i < GAME_PLAYER; i++)
//	//{
//	//	if (wBankerUser == SwitchViewChairID(i))
//	//	{
//	//		wBankerUser = i;
//	//		break;
//	//	}
//	//}
//	CString strName[GAME_PLAYER];
//
//	//// ��������
//	//for (int i = 0; i < GAME_PLAYER; i++)
//	//{
//	//	IClientUserItem * pIClientUserItem = GetTableUserItem(i);
//	//	if (pIClientUserItem != NULL)
//	//	{
//	//		strName[i] = pIClientUserItem->GetAccounts();
//	//	}
//	//}
//
//	// ��ʾ����
//	BYTE cbMagicData[3] = { 0 };
//	BYTE cbCardData[MAX_REPERTORY_CNT] = { 0 };
//	BOOL cbControlGameCount = ShowMahjong(this, wBankerUser, strName, GAME_PLAYER, cbCardData, cbMagicData);
//
//	//// �ͷ�ģ��
//	FreeLibrary(hTestDLL);
//	hTestDLL = NULL;
//
//	// ��ԭĿ¼
//	SetCurrentDirectory(szDirectory);
//
//	// ��������
//	if (cbControlGameCount > 0)
//	{
//		// ����ṹ
//		CMD_GR_SendCard SendCard;
//		SendCard.wBankerUser = wBankerUser;
//
//		//�����ļ���
//		TCHAR szPath[MAX_PATH] = TEXT("");
//		TCHAR szOutBuf[MAX_PATH] = TEXT("");
//		CString strApp = TEXT("Config");
//		CString strConfigFileName;
//		GetCurrentDirectory(sizeof(szPath), szPath);
//		strConfigFileName.Format(TEXT("%s\\SparrowConfig.ini"), szPath);
//
//		//�����
//		SendCard.cbCardCount = GetPrivateProfileInt(strApp, TEXT("MaxRepertory"), 144, strConfigFileName);
//		SendCard.cbControlGameCount = cbControlGameCount;
//		CopyMemory(SendCard.cbCardData, cbCardData, sizeof(BYTE) * SendCard.cbCardCount);
//
//		// ��������
//		SendCard.cbMagicOpenPos = cbMagicData[1];
//		SendCard.cbMagicIndex = cbMagicData[2];
//	}
//#endif

	return ;


	//if (m_pICardDispatcher == NULL)
	//{
	//	//��������
	//	TCHAR DebugName[LEN_KIND * 2] = _T("");
	//	TCHAR DebugFolder[LEN_KIND * 2] = _T("");
	//	//��ȡ�������
	//	size_t nProcessNameLen = _tcslen(m_GameKind.szProcessName);
	//	wcsncpy_s(DebugName, m_GameKind.szProcessName, nProcessNameLen - 4);
	//	wsprintf(DebugFolder, TEXT("%s"), DebugName);
	//	wsprintf(DebugName, TEXT("%s\\CardDispatcher.dll"), DebugName);

	//	//��ȡ����
	//	TCHAR szPath[MAX_PATH] = TEXT("");
	//	CWHService::GetWorkDirectory(szPath, sizeof(szPath));
	//	TCHAR szFileName[MAX_PATH] = TEXT("");
	//	_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\CardDispatcher\\%s"), szPath, DebugName);

	//	DWORD dwAttrib = GetFileAttributes(szFileName);
	//	if (!(INVALID_FILE_ATTRIBUTES != dwAttrib && 0 == (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
	//	{
	//		CString str;
	//		str.Format(TEXT("��������� %s �����ڣ�"), szFileName);
	//		MessageBox(str);
	//		return;
	//	}

	//	m_hCardDispatcherInst = LoadLibraryEx(szFileName, NULL, LOAD_WITH_ALTERED_SEARCH_PATH);

	//	if (m_hCardDispatcherInst)
	//	{
	//		typedef void* (*CREATE)();
	//		CREATE pFunction = (CREATE)GetProcAddress(m_hCardDispatcherInst, "CreateCardDispatcher");
	//		if (pFunction)
	//		{
	//			m_pICardDispatcher = static_cast<ICardDispatcher*>(pFunction());
	//			m_pICardDispatcher->Create(this, this, DebugFolder);
	//		}
	//	}
	//	else
	//	{
	//		DWORD dwLastError = GetLastError();
	//		CString str;
	//		str.Format(TEXT("���������ʧ�ܣ�������� %d ��"), dwLastError);
	//		MessageBox(str);
	//		return;
	//	}
	//}
	//// �ص�
	//if (m_pICardDispatcher)
	//{
	//	m_pICardDispatcher->ShowWindow(true);
	//}
}

// ��������Ϣ
bool CDlgSessionServer::OnCardDispatcherDebugInfo(WORD wTableID, void* pData, WORD nSize)
{
	//У��״̬
	if (m_pVirtualLinkItem->GetLinkStatus() != LINK_STATUS_LINKED)
	{
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
	m_pVirtualLinkItem->SendVirtualData(MDM_GR_MANAGE, SUB_GR_CARDDISPATCHERDEBUG, cbBuffer, wSendSize);

	return true;
}

//��ʾ��ť
VOID CDlgSessionServer::EnableButton(bool bEnabled)
{
	GetDlgItem(IDC_BT_ENTER_ROOM)->ShowWindow(bEnabled);
	//GetDlgItem(IDC_BT_ENTER_ROOM)->EnableWindow(bEnabled);	

	GetDlgItem(IDC_BT_EXIT_ROOM)->ShowWindow(!bEnabled);
	//GetDlgItem(IDC_BT_EXIT_ROOM)->EnableWindow(!bEnabled);

	GetDlgItem(IDC_BT_GAME_DEBUG)->EnableWindow(!bEnabled);
	GetDlgItem(IDC_BT_ROBOT_DEBUG)->EnableWindow(!bEnabled);
	GetDlgItem(IDC_BT_CARD_DISPATCHER)->EnableWindow(!bEnabled);}

//���͵�¼
bool CDlgSessionServer::SendLogonPacket()
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
	m_pVirtualLinkItem->SendVirtualData(MDM_GR_LOGON, SUB_GR_LOGON_COMPUTER, &LogonUserID, sizeof(LogonUserID));

	return true;
}

//��ȡ��Ϸ�汾
VOID CDlgSessionServer::GetProcessVersion(WORD wKindID, DWORD& dwProcessVersion)
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
VOID CDlgSessionServer::SetServiceStatus(enServiceStatus ServiceStatus)
{
	//���ñ���
	m_ServiceStatus = ServiceStatus;

	return;
}

//��ȡ������Ϣ
void CDlgSessionServer::GetConnectInfo()
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
VOID CDlgSessionServer::OnUserItemAcitve(IClientUserItem* pIClientUserItem)
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
VOID CDlgSessionServer::OnEventUserEnter(IClientUserItem* pIClientUserItem)
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
VOID CDlgSessionServer::OnUserItemDelete(IClientUserItem* pIClientUserItem)
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
VOID CDlgSessionServer::OnEventUserLeave(IClientUserItem* pIClientUserItem)
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
VOID CDlgSessionServer::OnUserFaceUpdate(IClientUserItem* pIClientUserItem)
{
	//��������
	tagClientUserInfo* pUserInfo = pIClientUserItem->GetUserInfo();
	tagCustomFaceInfo* pCustomFaceInfo = pIClientUserItem->GetCustomFaceInfo();

	return;
}

//�û�����
VOID CDlgSessionServer::OnUserItemUpdate(IClientUserItem* pIClientUserItem, const tagClientUserScore& LastScore)
{
	//��������
	tagClientUserInfo* pUserInfo = pIClientUserItem->GetUserInfo();
	tagClientUserInfo* pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();

	//����״̬
	m_UserListControl.UpdateUserItem(pIClientUserItem);

	return;
}

//�û�����
VOID CDlgSessionServer::OnUserItemUpdate(IClientUserItem* pIClientUserItem, const tagUserStatus& LastStatus)
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
//VOID CDlgSessionServer::OnUserItemUpdate(IClientUserItem* pIClientUserItem, const tagUserSegment& LastSegment)
//{
//	//��������
//	WORD wMeTableID = m_pIMySelfUserItem->GetTableID();
//	WORD wUserTableID = pIClientUserItem->GetTableID();
//
//	return;
//}

// ������Ϣ
bool CDlgSessionServer::SendDebugData(WORD nMessageID, WORD wTableID, void* pData, WORD wDataSize)
{
	//У������
	if (m_pVirtualLinkItem->GetLinkStatus() != LINK_STATUS_LINKED)
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
	m_pVirtualLinkItem->SendVirtualData(MAKECMMD(MDM_GR_DEBUG, SERVICE_MODULE_GAME), SUB_GR_DEBUG_PACKET, cbBuffer, sizeof(CMD_GR_DebugPacket)+ wDataSize);

	return true;
}

//�����б�����
VOID CDlgSessionServer::CleanListData()
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
bool CDlgSessionServer::OnEventMissionLink(LONG nErrorCode)
{
	//������ʾ
	if (nErrorCode != 0L)
	{
		//�رշ���
		PostMessage(WM_COMMAND, IDM_DELETE_SERVER_ITEM, 0);

		//��ʾ��Ϣ
		CInformation Information(AfxGetMainWnd());
		Information.ShowMessageBox(TEXT("�ܱ�Ǹ����Ϸ��������ʧ�ܣ����Ժ����Ի�������վ���棡"), MB_ICONINFORMATION, 0);
		return true;
	}

	//���͵�¼
	SendLogonPacket();

	//����״̬
	SetServiceStatus(ServiceStatus_Validate);

	return true;
}

//�ر��¼�
bool CDlgSessionServer::OnEventMissionShut(BYTE cbShutReason)
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

	//����״̬
	m_bActiveStatus = false;

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
bool CDlgSessionServer::OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize)
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
	}

	return true;
}

//��¼����
bool CDlgSessionServer::OnSocketMainLogon(WORD wSubCmdID, VOID* pData, WORD wDataSize)
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
bool CDlgSessionServer::OnSocketMainDebug(WORD wSubCmdID, VOID* pData, WORD wDataSize)
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
bool CDlgSessionServer::OnSocketSubLogonFinish(VOID* pData, WORD wDataSize)
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
bool CDlgSessionServer::OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize)
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
bool CDlgSessionServer::OnSocketSubLogonFailure(VOID* pData, WORD wDataSize)
{
	//Ч�����
	CMD_GR_LogonFailure* pLogonFailure = (CMD_GR_LogonFailure*)pData;
	ASSERT(wDataSize >= (sizeof(CMD_GR_LogonFailure) - sizeof(pLogonFailure->szDescribeString)));
	if (wDataSize < (sizeof(CMD_GR_LogonFailure) - sizeof(pLogonFailure->szDescribeString))) return false;

	//�رմ���
	m_pVirtualLinkItem->CloseVirtualLink();

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
bool CDlgSessionServer::OnSocketSubUpdateNotify(VOID* pData, WORD wDataSize)
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
bool CDlgSessionServer::OnSocketMainManager(WORD wSubCmdID, VOID* pData, WORD wDataSize)
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
bool CDlgSessionServer::OnSocketMainGameFrame(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)
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
bool CDlgSessionServer::OnSocketSubSystemMessage(VOID* pData, WORD wDataSize)
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
bool CDlgSessionServer::OnSocketMainUser(WORD wSubCmdID, VOID* pData, WORD wDataSize)
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
bool CDlgSessionServer::OnSocketSubUserEnter(VOID* pData, WORD wDataSize)
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

            case DTP_GR_USER_NICKNAME:		//�û��ǳ�
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
bool CDlgSessionServer::OnSocketSubUserScore(VOID* pData, WORD wDataSize)
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
bool CDlgSessionServer::OnSocketSubUserStatus(VOID* pData, WORD wDataSize)
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
bool CDlgSessionServer::OnSocketSubDebugPacket(VOID* pData, WORD wDataSize)
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
VOID CDlgSessionServer::OnNMRclickUserList(NMHDR* pNMHDR, LRESULT* pResult)
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
VOID CDlgSessionServer::TrackUserItemMenu(IClientUserItem* pIClientUserItem)
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

		//����ָ�
		UserInfoMenu.AppendSeparator();

		//����
		UserInfoMenu.AppendMenu(IDM_SEND_CARD, TEXT("����"), bEnableKick ? MF_ENABLED : MF_GRAYED);
	}

	//�����˵�
	UserInfoMenu.TrackPopupMenu(this);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
