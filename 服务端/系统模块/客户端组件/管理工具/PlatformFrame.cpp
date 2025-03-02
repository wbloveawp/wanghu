
//平台文件
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

//控制按钮
#define IDC_MIN						100									//最小按钮
#define IDC_MAX						101									//最大按钮
#define IDC_CLOSE					102									//关闭按钮

//控件标识
#define IDC_SYSTEM_TRAY_ICON		303									//任务图标

//配置选项
#define TAB_SESSION_COMMAND			0									//命令会话
#define TAB_SESSION_OPTION			1									//选项绘画
#define TAB_SESSION_SERVER			2									//房间管理
#define TAB_SESSION_STOCK			3									//库存管理
#define TAB_SESSION_CONFIG			4									//配置管理
#define TAB_SESSION_ROBOT			5									//AI管理

//游戏ID
#define KIND_ID_XMOX				112									//虾米牛牛
#define KIND_ID_TEEMPATTI			246									//TeenPatti
#define KIND_ID_DEALRUMMY			248									//DealRummy
#define KIND_ID_WINGO				250									//WinGo
#define KIND_ID_SAWABALOOT			247									//SawaBaloot
#define KIND_ID_SUPERTOUZI			800									//超级骰宝
#define KIND_ID_DRAGONTIGER			145									//龙虎斗
//////////////////////////////////////////////////////////////////////////////////
//静态变量

CPlatformFrame * CPlatformFrame::m_pPlatformFrame=NULL;					//框架指针

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPlatformFrame,CDialog)

	//系统消息
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_GETMINMAXINFO()
	ON_WM_SETTINGCHANGE()

	//自定消息
	ON_MESSAGE(WM_EVENT_INFORM, OnEventInformMessage)
	ON_NOTIFY(NM_RCLICK, IDC_USER_LIST_CONTROL, OnNMRclickUserList)
	ON_NOTIFY(TCN_SELCHANGE, IDC_MANAGER_OPTION, OnTcnSelchangeManagerOption)

	ON_WM_DESTROY()
	ON_WM_COPYDATA()

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////


//构造函数
CPlatformFrame::CPlatformFrame() : CDialog(IDD_GAME_PLAZA)
{
	//设置变量	
	m_bLogonSuccess=false;	
	m_wTableID = INVALID_CHAIR;
	m_wServerType = 0;
	m_dwServerRule = 0;
	m_dwUserRight = 0;
	m_dwMasterRight = 0;
	m_cbItemIndex = -1;

	//设置模块
	m_VirtualLinkLogon.SetServiceModule(SERVICE_MODULE_LOGON);
	m_VirtualLinkServer.SetServiceModule(SERVICE_MODULE_GAME);

	//插入子项
	m_VirtualLinkManager.InsertVirtualLink(&m_VirtualLinkLogon);
	m_VirtualLinkManager.InsertVirtualLink(&m_VirtualLinkServer);

	//插入任务
	m_VirtualLinkLogon.InsertSessionItem(&m_MissionLogon);

	//平台变量
	ASSERT(m_pPlatformFrame==NULL);
	if (m_pPlatformFrame==NULL) m_pPlatformFrame=this;
	
	m_pIMySelfUserItem = NULL;


	return;
}

//析构函数
CPlatformFrame::~CPlatformFrame()
{
	//平台变量
	ASSERT(m_pPlatformFrame==this);
	if (m_pPlatformFrame==this) m_pPlatformFrame=NULL;

	return;
}

//消息过虑
BOOL CPlatformFrame::PreTranslateMessage(MSG * pMsg)
{
	return __super::PreTranslateMessage(pMsg);
}

//交换数据
VOID CPlatformFrame::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
}

//完成通知
VOID CPlatformFrame::OnGameItemFinish()
{
	for (int i = 0; i < CountArray(m_pDlgSessionItem); i++)
	{
		m_pDlgSessionItem[i]->OnEventListFinish();
	}
}

//完成通知
VOID CPlatformFrame::OnGameKindFinish(WORD wKindID)
{
}

//插入通知
VOID CPlatformFrame::OnGameItemInsert(CGameListItem * pGameListItem)
{
}


//更新通知
VOID CPlatformFrame::OnGameItemUpdate(CGameListItem * pGameListItem)
{
}

//删除通知
VOID CPlatformFrame::OnGameItemDelete(CGameListItem * pGameListItem)
{
}

//总数通知
VOID CPlatformFrame::OnUpdateOnlineCount(DWORD dwOnlineCount)
{
}


//初始化控件
VOID CPlatformFrame::InitControls()
{
	//获取控件
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_MANAGER_OPTION);

	//插入子项
	pTabCtrl->InsertItem(TAB_SESSION_COMMAND, TEXT("管理模块"));
	pTabCtrl->InsertItem(TAB_SESSION_OPTION, TEXT("房间选项"));	
	pTabCtrl->InsertItem(TAB_SESSION_SERVER, TEXT("房间视图"));
	pTabCtrl->InsertItem(TAB_SESSION_ROBOT, TEXT("机器管理"));
	pTabCtrl->InsertItem(TAB_SESSION_STOCK, TEXT("库存管理"));
	pTabCtrl->InsertItem(TAB_SESSION_CONFIG, TEXT("配置管理"));

	//设置大小
	pTabCtrl->SetItemSize(CSize(110, 25));

	//添加子项
	m_pDlgSessionItem[TAB_SESSION_COMMAND] = &m_DlgSessionCommand;
	m_pDlgSessionItem[TAB_SESSION_OPTION] = &m_DlgSessionOption;
	m_pDlgSessionItem[TAB_SESSION_SERVER] = &m_pDlgSessionServer;
	m_pDlgSessionItem[TAB_SESSION_ROBOT] = &m_DlgSessionRobot;
	m_pDlgSessionItem[TAB_SESSION_STOCK] = &m_DlgSessionStock;
	m_pDlgSessionItem[TAB_SESSION_CONFIG] = &m_DlgSessionConfig;

	//配置任务
	m_VirtualLinkLogon.InsertSessionItem((CSessionItem*)m_pDlgSessionItem[TAB_SESSION_COMMAND]);
	m_VirtualLinkLogon.InsertSessionItem((CSessionItem*)m_pDlgSessionItem[TAB_SESSION_OPTION]);
	m_VirtualLinkLogon.InsertSessionItem((CSessionItem*)m_pDlgSessionItem[TAB_SESSION_ROBOT]);
	m_VirtualLinkLogon.InsertSessionItem((CSessionItem*)m_pDlgSessionItem[TAB_SESSION_STOCK]);
	m_VirtualLinkLogon.InsertSessionItem((CSessionItem*)m_pDlgSessionItem[TAB_SESSION_CONFIG]);
	m_VirtualLinkServer.InsertSessionItem((CSessionItem*)m_pDlgSessionItem[TAB_SESSION_SERVER]);	

	//变量定义
	CRect rcCreate(0, 0, 0, 0);

	//设置子项
	for (int i = 0; i < CountArray(m_pDlgSessionItem); i++)
	{
		//指针判断
		if (m_pDlgSessionItem[i] == NULL) continue;
		if (m_pDlgSessionItem[i]->GetSafeHwnd() != NULL) continue;

		//创建子项		
		m_pDlgSessionItem[i]->CreateViewItem(rcCreate, pTabCtrl);

		//配置列表
		m_pDlgSessionItem[i]->SetServerListManager(&m_ServerListManager);
	}

	//显示子项
	ActiveViewItem(TAB_SESSION_COMMAND);
}

//激活子项
VOID CPlatformFrame::ActiveViewItem(BYTE cbItemIndex)
{
	//判断索引
	if (m_cbItemIndex==cbItemIndex) return;

	//设置索引
	m_cbItemIndex=cbItemIndex;

	//控件区域
	CRect rcTabCtrl;
	GetDlgItem(IDC_MANAGER_OPTION)->GetClientRect(&rcTabCtrl);

	//移动准备
	HDWP hDwp=BeginDeferWindowPos(64);
	UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER;	

	//设置控件
	if (m_pDlgSessionItem[m_cbItemIndex]->m_hWnd!=NULL)
	{
		//切换视图
		m_pDlgSessionItem[m_cbItemIndex]->SwitchViewItem(true);

		//调整窗口
		DeferWindowPos(hDwp,*m_pDlgSessionItem[m_cbItemIndex],NULL, 10, 32, rcTabCtrl.Width()-15, rcTabCtrl.Height()-32-5,SWP_NOZORDER|SWP_SHOWWINDOW);
	}

	//设置控件
	for (int i=0; i<CountArray(m_pDlgSessionItem); i++)
	{
		if (i==m_cbItemIndex) continue;
		if (m_pDlgSessionItem[i]->m_hWnd!=NULL)
		{
			//切换视图
			m_pDlgSessionItem[i]->SwitchViewItem(false);

			//调整窗口
			DeferWindowPos(hDwp,*m_pDlgSessionItem[i],NULL, 10, 32, rcTabCtrl.Width()-15,rcTabCtrl.Height()-32-5,SWP_NOZORDER|SWP_HIDEWINDOW);
		}
	}

	//结束调整
	LockWindowUpdate();
	EndDeferWindowPos(hDwp);
	UnlockWindowUpdate();
}

//调整控件
VOID CPlatformFrame::RectifyControl(INT nWidth, INT nHeight)
{
	//状态判断
	if ((nWidth==0)||(nHeight==0)) return;

	//移动准备
	HDWP hDwp=BeginDeferWindowPos(64);
	UINT uFlags=SWP_NOACTIVATE|SWP_NOCOPYBITS|SWP_NOZORDER;

	//结束调整
	LockWindowUpdate();
	EndDeferWindowPos(hDwp);
	UnlockWindowUpdate();

	//更新界面
	RedrawWindow(NULL,NULL,RDW_ERASE|RDW_INVALIDATE);

	return;
}

//关闭消息
VOID CPlatformFrame::OnClose()
{
	//登录标识
	if(m_bLogonSuccess==true)
	{
		//选择提示
		CInformation Information;
		INT nRetCode = Information.ShowMessageBox(TEXT("系统提示"),TEXT("您确定要退出管理工具吗？"),MB_YESNO);

		//暂时不退出
		if(nRetCode==IDNO) return;
	}

	//删除绘画
	m_VirtualLinkLogon.DeleteSessionItem();

	//关闭链接
	m_VirtualLinkManager.ConcludeVirtualLink();
	m_VirtualLinkManager.DeleteVirtualLink(&m_VirtualLinkLogon);
	m_VirtualLinkManager.DeleteVirtualLink(&m_VirtualLinkServer);

	//销毁创口
	DestroyWindow();

	__super::OnClose();
}

//销毁消息
VOID CPlatformFrame::OnDestroy()
{
	__super::OnDestroy();	
}

//绘画背景
BOOL CPlatformFrame::OnEraseBkgnd(CDC * pDC)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//填充区域
	pDC->SetBkMode(TRANSPARENT);
	pDC->FillSolidRect(rcClient,CR_CONTROL_BACK);

	return TRUE;
}

//位置消息
VOID CPlatformFrame::OnSize(UINT nType, INT cx, INT cy) 
{
	__super::OnSize(nType, cx, cy);

	//调整控件
	RectifyControl(cx,cy);

	return;
}

//命令消息
BOOL CPlatformFrame::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD dwCommandID = HIWORD(wParam);
	UINT nCommandID = LOWORD(wParam);

	return __super::OnCommand(wParam,lParam);
}

//初始窗口
BOOL CPlatformFrame::OnInitDialog()
{
	__super::OnInitDialog();

	//设置图标
	HINSTANCE hInstance=AfxGetInstanceHandle();
	SetIcon(LoadIcon(hInstance,MAKEINTRESOURCE(IDR_MAINFRAME)),TRUE);
	SetIcon(LoadIcon(hInstance,MAKEINTRESOURCE(IDR_MAINFRAME)),FALSE);

	//设置标题
	SetWindowText(TEXT("管理工具"));

	//初始化控件
	InitControls();

	//设置回调
	CServerListManager::GetInstance()->SetServerListViewSink(this);

	//注册事件
	CDispatchManager* pDispatchManager = CDispatchManager::GetInstance();
	if (pDispatchManager != NULL) pDispatchManager->RegisterEventWnd(m_hWnd);

	//登录系统
	m_MissionLogon.ShowLogon();

	//设置列表对象
	m_DlgSessionRobot.Initialization(m_TCPSocketModule.GetInterface(), NULL, m_GameServer);


	return TRUE;  
}

//接口查询
VOID* CPlatformFrame::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	//QUERYINTERFACE(ITCPSocketSink, Guid, dwQueryVer);
	QUERYINTERFACE(IPlatformFrame, Guid, dwQueryVer);
	//QUERYINTERFACE(IUserManagerSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IPlatformFrame, Guid, dwQueryVer);
	return NULL;
}


//右键列表
VOID CPlatformFrame::OnNMRclickUserList(NMHDR* pNMHDR, LRESULT* pResult)
{
	//变量定义
	NMITEMACTIVATE* pListNotify = (NMITEMACTIVATE*)pNMHDR;

	//弹出菜单
	if (pListNotify->iItem != -1)
	{
		//选择数目
		UINT nSelectCount = m_UserListControl.GetSelectedCount();

		//选择处理
		if (nSelectCount > 1L)
		{
			//用户数组
			IClientUserItem* pIClientUserItem[32];
			ZeroMemory(pIClientUserItem, sizeof(pIClientUserItem));

			//变量定义
			WORD wUserCount = 0;
			POSITION nSelectPos = m_UserListControl.GetFirstSelectedItemPosition();

			//选择收集
			while ((nSelectPos != NULL) && (wUserCount < CountArray(pIClientUserItem)))
			{
				//获取选择
				INT nSelectItem = m_UserListControl.GetNextSelectedItem(nSelectPos);

				//插入用户
				DWORD_PTR lItemData = m_UserListControl.GetItemData(nSelectItem);
				if (lItemData != NULL) pIClientUserItem[wUserCount++] = ((IClientUserItem*)(lItemData));
			};

			//弹出菜单
			//if (wUserCount > 0) TrackUserItemMenu(pIClientUserItem, wUserCount);
		}
		else
		{
			//弹出菜单
			DWORD_PTR lItemData = m_UserListControl.GetItemData(pListNotify->iItem);
			if (lItemData != NULL) TrackUserItemMenu((IClientUserItem*)(lItemData));
		}
	}

	return;
}

//显示菜单
VOID CPlatformFrame::TrackUserItemMenu(IClientUserItem* pIClientUserItem)
{
	//效验参数
	ASSERT(pIClientUserItem != NULL);
	if (pIClientUserItem == NULL) return;

	//设置变量
	//m_MenuUserItemArray.RemoveAll();
	//m_MenuUserItemArray.Add(pIClientUserItem);

	//构造菜单
	CSkinMenu UserInfoMenu;
	UserInfoMenu.CreateMenu();

	//变量定义
	TCHAR szMenuString[256] = TEXT("");
	bool bMeUserItem = (pIClientUserItem == m_pIMySelfUserItem);

	//变量定义
	tagClientUserInfo * pUserInfo = pIClientUserItem->GetUserInfo();
	tagClientUserInfo* pMeUserInfo = m_pIMySelfUserItem->GetUserInfo();
	/*IClientUserItem * pIChatTargetItem = m_ChatControl.GetChatTargetUserItem();*/

	//插入分割
	UserInfoMenu.AppendSeparator();

	//常规菜单
	UserInfoMenu.AppendMenu(IDM_COPY_USER_NAME, TEXT("复制ID"));

	//会员菜单
	if (!bMeUserItem/*&& m_pIClientKernel->GetServerAttribute()->wServerType != GAME_GENRE_MATCH*/)
	{
		//插入分割
		UserInfoMenu.AppendSeparator();

		//变量定义
		BYTE  cbMeUserStatus = m_pIMySelfUserItem->GetUserStatus();
		BYTE  cbUserStatus = pIClientUserItem->GetUserStatus();
		bool bEnableKick = true;
		//	if (pMeUserInfo->cbMemberOrder <= pUserInfo->cbMemberOrder ||/* (cbMeUserStatus != US_SIT&&cbMeUserStatus != US_READY) ||*/
		//		(cbUserStatus != US_SIT&&cbUserStatus != US_READY))
		//		bEnableKick = false;

		//旁观菜单
		_sntprintf_s(szMenuString, CountArray(szMenuString), TEXT("踢 [ %s ] 离开"), pIClientUserItem->GetAccounts());
		UserInfoMenu.AppendMenu(IDM_KILL_USER, szMenuString, bEnableKick ? MF_ENABLED : MF_GRAYED);
	}

	//弹出菜单
	UserInfoMenu.TrackPopupMenu(this);

	return;
}

//选择变更
VOID CPlatformFrame::OnTcnSelchangeManagerOption(NMHDR* pNMHDR, LRESULT* pResult)
{
	//获取控件
	CTabCtrl* pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_MANAGER_OPTION);
	ActiveViewItem(pTabCtrl->GetCurSel());
}

//事件消息
LRESULT CPlatformFrame::OnEventInformMessage(WPARAM wParam, LPARAM lParam)
{
	//事件处理
	switch (wParam)
	{
	case EVENT_LOGON_SUCCESS:		//登录完成
	{
		//显示窗口
		ShowWindow(SW_SHOW);
		SetForegroundWindow();

		//设置变量
		m_bLogonSuccess=true;

		return 0L;
	}
	case EVENT_LOGOUT_NOTIFY:		//注销完成
	{
		//错误提示
		CInformation Information(CPlatformFrame::GetInstance());

		//登出提醒
		if (lParam == 0)
		{
			Information.ShowMessageBox(TEXT("您的账号在其他地方登录，您被迫下线！"));
		}		
		else
		{
			Information.ShowMessageBox(TEXT("与服务器的链接断开了！"));
		}

		//设置变量
		m_bLogonSuccess = false;

		//关闭连接
		m_VirtualLinkManager.ConcludeVirtualLink();

		//获取控件
		CTabCtrl* pTabCtrl = (CTabCtrl*)GetDlgItem(IDC_MANAGER_OPTION);
		pTabCtrl->DeleteAllItems();

		//初始化控件
		InitControls();		

		//显示窗口
		ShowWindow(SW_HIDE);
		
		//显示登录
		m_MissionLogon.ShowLogon();

		return 0L;
	}
	}

	return 0L;
}

//////////////////////////////////////////////////////////////////////////