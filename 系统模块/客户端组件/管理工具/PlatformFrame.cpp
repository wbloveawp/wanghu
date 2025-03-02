
//ƽ̨�ļ�
#include "StdAfx.h"
#include "Resource.h"
#include "GlobalUnits.h"
#include "PlatformFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//���ư�ť
#define IDC_MIN						100									//��С��ť
#define IDC_MAX						101									//���ť
#define IDC_CLOSE					102									//�رհ�ť

//�ؼ���ʶ
#define IDC_SYSTEM_TRAY_ICON		303									//����ͼ��

//����ѡ��
#define TAB_SESSION_COMMAND			0									//����Ự
#define TAB_SESSION_OPTION			1									//ѡ��滭
#define TAB_SESSION_SERVER			2									//�������
#define TAB_SESSION_STOCK			3									//������
#define TAB_SESSION_CONFIG			4									//���ù���
#define TAB_SESSION_ROBOT			5									//AI����

//��ϷID
#define KIND_ID_XMOX				112									//Ϻ��ţţ
#define KIND_ID_TEEMPATTI			246									//TeenPatti
#define KIND_ID_DEALRUMMY			248									//DealRummy
#define KIND_ID_WINGO				250									//WinGo
#define KIND_ID_SAWABALOOT			247									//SawaBaloot
#define KIND_ID_SUPERTOUZI			800									//��������
#define KIND_ID_DRAGONTIGER			145									//������
//////////////////////////////////////////////////////////////////////////////////
//��̬����

CPlatformFrame * CPlatformFrame::m_pPlatformFrame=NULL;					//���ָ��

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPlatformFrame,CDialog)

	//ϵͳ��Ϣ
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_GETMINMAXINFO()
	ON_WM_SETTINGCHANGE()

	//�Զ���Ϣ
	ON_MESSAGE(WM_EVENT_INFORM, OnEventInformMessage)
	ON_NOTIFY(NM_RCLICK, IDC_USER_LIST_CONTROL, OnNMRclickUserList)
	ON_NOTIFY(TCN_SELCHANGE, IDC_MANAGER_OPTION, OnTcnSelchangeManagerOption)

	ON_WM_DESTROY()
	ON_WM_COPYDATA()

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////


//���캯��
CPlatformFrame::CPlatformFrame() : CDialog(IDD_GAME_PLAZA)
{
	//���ñ���	
	m_bLogonSuccess=false;	
	m_wTableID = INVALID_CHAIR;
	m_wServerType = 0;
	m_dwServerRule = 0;
	m_dwUserRight = 0;
	m_dwMasterRight = 0;
	m_cbItemIndex = -1;

	//����ģ��
	m_VirtualLinkLogon.SetServiceModule(SERVICE_MODULE_LOGON);
	m_VirtualLinkServer.SetServiceModule(SERVICE_MODULE_GAME);

	//��������
	m_VirtualLinkManager.InsertVirtualLink(&m_VirtualLinkLogon);
	m_VirtualLinkManager.InsertVirtualLink(&m_VirtualLinkServer);

	//��������
	m_VirtualLinkLogon.InsertSessionItem(&m_MissionLogon);

	//ƽ̨����
	ASSERT(m_pPlatformFrame==NULL);
	if (m_pPlatformFrame==NULL) m_pPlatformFrame=this;
	
	m_pIMySelfUserItem = NULL;


	return;
}

//��������
CPlatformFrame::~CPlatformFrame()
{
	//ƽ̨����
	ASSERT(m_pPlatformFrame==this);
	if (m_pPlatformFrame==this) m_pPlatformFrame=NULL;

	return;
}

//��Ϣ����
BOOL CPlatformFrame::PreTranslateMessage(MSG * pMsg)
{
	return __super::PreTranslateMessage(pMsg);
}

//��������
VOID CPlatformFrame::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}

//���֪ͨ
VOID CPlatformFrame::OnGameItemFinish()
{
	for (int i = 0; i < CountArray(m_pDlgSessionItem); i++)
	{
		m_pDlgSessionItem[i]->OnEventListFinish();
	}
}

//���֪ͨ
VOID CPlatformFrame::OnGameKindFinish(WORD wKindID)
{
}

//����֪ͨ
VOID CPlatformFrame::OnGameItemInsert(CGameListItem * pGameListItem)
{
}


//����֪ͨ
VOID CPlatformFrame::OnGameItemUpdate(CGameListItem * pGameListItem)
{
}

//ɾ��֪ͨ
VOID CPlatformFrame::OnGameItemDelete(CGameListItem * pGameListItem)
{
}

//����֪ͨ
VOID CPlatformFrame::OnUpdateOnlineCount(DWORD dwOnlineCount)
{
}


//��ʼ���ؼ�
VOID CPlatformFrame::InitControls()
{
	//��ȡ�ؼ�
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_MANAGER_OPTION);

	//��������
	pTabCtrl->InsertItem(TAB_SESSION_COMMAND, TEXT("����ģ��"));
	pTabCtrl->InsertItem(TAB_SESSION_OPTION, TEXT("����ѡ��"));	
	pTabCtrl->InsertItem(TAB_SESSION_SERVER, TEXT("������ͼ"));
	pTabCtrl->InsertItem(TAB_SESSION_ROBOT, TEXT("��������"));
	pTabCtrl->InsertItem(TAB_SESSION_STOCK, TEXT("������"));
	pTabCtrl->InsertItem(TAB_SESSION_CONFIG, TEXT("���ù���"));

	//���ô�С
	pTabCtrl->SetItemSize(CSize(110, 25));

	//�������
	m_pDlgSessionItem[TAB_SESSION_COMMAND] = &m_DlgSessionCommand;
	m_pDlgSessionItem[TAB_SESSION_OPTION] = &m_DlgSessionOption;
	m_pDlgSessionItem[TAB_SESSION_SERVER] = &m_pDlgSessionServer;
	m_pDlgSessionItem[TAB_SESSION_ROBOT] = &m_DlgSessionRobot;
	m_pDlgSessionItem[TAB_SESSION_STOCK] = &m_DlgSessionStock;
	m_pDlgSessionItem[TAB_SESSION_CONFIG] = &m_DlgSessionConfig;

	//��������
	m_VirtualLinkLogon.InsertSessionItem((CSessionItem*)m_pDlgSessionItem[TAB_SESSION_COMMAND]);
	m_VirtualLinkLogon.InsertSessionItem((CSessionItem*)m_pDlgSessionItem[TAB_SESSION_OPTION]);
	m_VirtualLinkLogon.InsertSessionItem((CSessionItem*)m_pDlgSessionItem[TAB_SESSION_ROBOT]);
	m_VirtualLinkLogon.InsertSessionItem((CSessionItem*)m_pDlgSessionItem[TAB_SESSION_STOCK]);
	m_VirtualLinkLogon.InsertSessionItem((CSessionItem*)m_pDlgSessionItem[TAB_SESSION_CONFIG]);
	m_VirtualLinkServer.InsertSessionItem((CSessionItem*)m_pDlgSessionItem[TAB_SESSION_SERVER]);	

	//��������
	CRect rcCreate(0, 0, 0, 0);

	//��������
	for (int i = 0; i < CountArray(m_pDlgSessionItem); i++)
	{
		//ָ���ж�
		if (m_pDlgSessionItem[i] == NULL) continue;
		if (m_pDlgSessionItem[i]->GetSafeHwnd() != NULL) continue;

		//��������		
		m_pDlgSessionItem[i]->CreateViewItem(rcCreate, pTabCtrl);

		//�����б�
		m_pDlgSessionItem[i]->SetServerListManager(&m_ServerListManager);
	}

	//��ʾ����
	ActiveViewItem(TAB_SESSION_COMMAND);
}

//��������
VOID CPlatformFrame::ActiveViewItem(BYTE cbItemIndex)
{
	//�ж�����
	if (m_cbItemIndex==cbItemIndex) return;

	//��������
	m_cbItemIndex=cbItemIndex;

	//�ؼ�����
	CRect rcTabCtrl;
	GetDlgItem(IDC_MANAGER_OPTION)->GetClientRect(&rcTabCtrl);

	//�ƶ�׼��
	HDWP hDwp=BeginDeferWindowPos(64);
	UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER;	

	//���ÿؼ�
	if (m_pDlgSessionItem[m_cbItemIndex]->m_hWnd!=NULL)
	{
		//�л���ͼ
		m_pDlgSessionItem[m_cbItemIndex]->SwitchViewItem(true);

		//��������
		DeferWindowPos(hDwp,*m_pDlgSessionItem[m_cbItemIndex],NULL, 10, 32, rcTabCtrl.Width()-15, rcTabCtrl.Height()-32-5,SWP_NOZORDER|SWP_SHOWWINDOW);
	}

	//���ÿؼ�
	for (int i=0; i<CountArray(m_pDlgSessionItem); i++)
	{
		if (i==m_cbItemIndex) continue;
		if (m_pDlgSessionItem[i]->m_hWnd!=NULL)
		{
			//�л���ͼ
			m_pDlgSessionItem[i]->SwitchViewItem(false);

			//��������
			DeferWindowPos(hDwp,*m_pDlgSessionItem[i],NULL, 10, 32, rcTabCtrl.Width()-15,rcTabCtrl.Height()-32-5,SWP_NOZORDER|SWP_HIDEWINDOW);
		}
	}

	//��������
	LockWindowUpdate();
	EndDeferWindowPos(hDwp);
	UnlockWindowUpdate();
}

//�����ؼ�
VOID CPlatformFrame::RectifyControl(INT nWidth, INT nHeight)
{
	//״̬�ж�
	if ((nWidth==0)||(nHeight==0)) return;

	//�ƶ�׼��
	HDWP hDwp=BeginDeferWindowPos(64);
	UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER;

	//��������
	LockWindowUpdate();
	EndDeferWindowPos(hDwp);
	UnlockWindowUpdate();

	//���½���
	RedrawWindow(NULL,NULL,RDW_ERASE|RDW_INVALIDATE);

	return;
}

//�ر���Ϣ
VOID CPlatformFrame::OnClose()
{
	//��¼��ʶ
	if(m_bLogonSuccess==true)
	{
		//ѡ����ʾ
		CInformation Information;
		INT nRetCode = Information.ShowMessageBox(TEXT("ϵͳ��ʾ"),TEXT("��ȷ��Ҫ�˳���������"),MB_YESNO);

		//��ʱ���˳�
		if(nRetCode==IDNO) return;
	}

	//ɾ���滭
	m_VirtualLinkLogon.DeleteSessionItem();

	//�ر�����
	m_VirtualLinkManager.ConcludeVirtualLink();
	m_VirtualLinkManager.DeleteVirtualLink(&m_VirtualLinkLogon);
	m_VirtualLinkManager.DeleteVirtualLink(&m_VirtualLinkServer);

	//���ٴ���
	DestroyWindow();

	__super::OnClose();
}

//������Ϣ
VOID CPlatformFrame::OnDestroy()
{
	__super::OnDestroy();	
}

//�滭����
BOOL CPlatformFrame::OnEraseBkgnd(CDC * pDC)
{
	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//�������
	pDC->SetBkMode(TRANSPARENT);
	pDC->FillSolidRect(rcClient,CR_CONTROL_BACK);

	return TRUE;
}

//λ����Ϣ
VOID CPlatformFrame::OnSize(UINT nType, INT cx, INT cy) 
{
	__super::OnSize(nType, cx, cy);

	//�����ؼ�
	RectifyControl(cx,cy);

	return;
}

//������Ϣ
BOOL CPlatformFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD dwCommandID = HIWORD(wParam);
	UINT nCommandID = LOWORD(wParam);

	return __super::OnCommand(wParam,lParam);
}

//��ʼ����
BOOL CPlatformFrame::OnInitDialog()
{
	__super::OnInitDialog();

	//����ͼ��
	HINSTANCE hInstance=AfxGetInstanceHandle();
	SetIcon(LoadIcon(hInstance,MAKEINTRESOURCE(IDR_MAINFRAME)),TRUE);
	SetIcon(LoadIcon(hInstance,MAKEINTRESOURCE(IDR_MAINFRAME)),FALSE);

	//���ñ���
	SetWindowText(TEXT("������"));

	//��ʼ���ؼ�
	InitControls();

	//���ûص�
	CServerListManager::GetInstance()->SetServerListViewSink(this);

	//ע���¼�
	CDispatchManager* pDispatchManager = CDispatchManager::GetInstance();
	if (pDispatchManager != NULL) pDispatchManager->RegisterEventWnd(m_hWnd);

	//��¼ϵͳ
	m_MissionLogon.ShowLogon();

	//�����б����
	m_DlgSessionRobot.Initialization(m_TCPSocketModule.GetInterface(), NULL, m_GameServer);


	return TRUE;  
}

//�ӿڲ�ѯ
VOID* CPlatformFrame::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	//QUERYINTERFACE(ITCPSocketSink, Guid, dwQueryVer);
	QUERYINTERFACE(IPlatformFrame, Guid, dwQueryVer);
	//QUERYINTERFACE(IUserManagerSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IPlatformFrame, Guid, dwQueryVer);
	return NULL;
}


//�Ҽ��б�
VOID CPlatformFrame::OnNMRclickUserList(NMHDR* pNMHDR, LRESULT* pResult)
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
VOID CPlatformFrame::TrackUserItemMenu(IClientUserItem* pIClientUserItem)
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
	}

	//�����˵�
	UserInfoMenu.TrackPopupMenu(this);

	return;
}

//ѡ����
VOID CPlatformFrame::OnTcnSelchangeManagerOption(NMHDR* pNMHDR, LRESULT* pResult)
{
	//��ȡ�ؼ�
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_MANAGER_OPTION);
	ActiveViewItem(pTabCtrl->GetCurSel());
}

//�¼���Ϣ
LRESULT CPlatformFrame::OnEventInformMessage(WPARAM wParam, LPARAM lParam)
{
	//�¼�����
	switch (wParam)
	{
	case EVENT_LOGON_SUCCESS:		//��¼���
	{
		//��ʾ����
		ShowWindow(SW_SHOW);
		SetForegroundWindow();

		//���ñ���
		m_bLogonSuccess=true;

		return 0L;
	}
	case EVENT_LOGOUT_NOTIFY:		//ע�����
	{
		//������ʾ
		CInformation Information(CPlatformFrame::GetInstance());

		//�ǳ�����
		if (lParam == 0)
		{
			Information.ShowMessageBox(TEXT("�����˺��������ط���¼�����������ߣ�"));
		}		
		else
		{
			Information.ShowMessageBox(TEXT("������������ӶϿ��ˣ�"));
		}

		//���ñ���
		m_bLogonSuccess = false;

		//�ر�����
		m_VirtualLinkManager.ConcludeVirtualLink();

		//��ȡ�ؼ�
		CTabCtrl* pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_MANAGER_OPTION);
		pTabCtrl->DeleteAllItems();

		//��ʼ���ؼ�
		InitControls();		

		//��ʾ����
		ShowWindow(SW_HIDE);
		
		//��ʾ��¼
		m_MissionLogon.ShowLogon();

		return 0L;
	}
	}

	return 0L;
}

//////////////////////////////////////////////////////////////////////////