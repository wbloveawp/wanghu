#include "Stdafx.h"
#include "GlobalUnits.h"
#include "SimulateLinkDlg.h"

//////////////////////////////////////////////////////////////////////////


BEGIN_MESSAGE_MAP(CSimulateLinkDlg, CDialog)

	//按钮消息
	ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
	ON_BN_CLICKED(IDC_START, OnBnClickedStart)

	//系统消息
	ON_WM_TIMER()
	ON_MESSAGE(WM_STATUS_CLIENT_USER, OnStatusClientUser)

	ON_CBN_SELCHANGE(IDC_COMBO_GAME_NAME, &CSimulateLinkDlg::OnCbnSelchangeComboGameName)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CSimulateLinkDlg::CSimulateLinkDlg() : CDialog(IDD_SIMULATELINK_DIALOG)
{
	//设置变量
	m_wConnectIndex = 0;

	//配置信息
	m_wAccountsNum = 0;	

	//配置变量
	m_wServerPort = 9000;
	_sntprintf_s(m_szServerAddr,CountArray(m_szServerAddr),TEXT("127.0.0.1"));

	return;
}

//析构函数
CSimulateLinkDlg::~CSimulateLinkDlg()
{
	//设置变量
	m_wConnectIndex = 0;

	//配置信息
	m_wAccountsNum = 0;	

	//清除数据
	m_StockGameList.RemoveAllItem();

	//关闭连接
	for (INT_PTR i = 0; i < m_SimulateClientArray.GetCount(); i++)
	{
		CSimulateClient* pSimulateClient = m_SimulateClientArray[i];
		SafeDelete(pSimulateClient);
	}

	//移除元素
	m_SimulateClientArray.RemoveAll();	

}
//控件绑定
void CSimulateLinkDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MESSAGE_INFO, m_RichEditCtrl);
}

//创建函数
BOOL CSimulateLinkDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//加载配置
	LoadConfig();

	//设置图标
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	//限制输入
	((CEdit*)GetDlgItem(IDC_ACCOUNTS))->LimitText(32);
	((CEdit*)GetDlgItem(IDC_PASSWORD))->LimitText(32);
	((CEdit*)GetDlgItem(IDC_USER_COUNT))->LimitText(3);
	((CEdit*)GetDlgItem(IDC_SERVER_PORT))->LimitText(5);
	((CEdit*)GetDlgItem(IDC_SERVER_ADDR))->LimitText(32);
	((CEdit*)GetDlgItem(IDC_GAME_KIND))->LimitText(6);
	((CEdit*)GetDlgItem(IDC_GAME_VERSION))->LimitText(10);
	((CEdit*)GetDlgItem(IDC_MODULE_DLL_NAME))->LimitText(64);

	//默认选择
	CheckRadioButton(IDC_RADIO_CONCURRENT, IDC_RADIO_QUEUE, IDC_RADIO_CONCURRENT);

	//测试模式
	auto pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TEST_MODE);
	WORD wTestMode[] = {TEST_MODE_LOGON,TEST_MODE_GAME,TEST_MODE_CLUB,TEST_MODE_BATTLE };
	LPCTSTR pszTestMode[] = { TEXT("登录测试"),TEXT("游戏测试"),TEXT("茶社测试 "),TEXT("约战测试")};
	for (int i = 0; i < CountArray(wTestMode); i++)
	{
		pComboBox->SetItemData(pComboBox->InsertString(i, pszTestMode[i]), wTestMode[i]);
	}

	//默认选择
	pComboBox->SetCurSel(1);

	//房间类型
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_SERVER_TYPE);
	WORD wServerType[] = { 0,GAME_GENRE_GOLD,GAME_GENRE_BATTLE,GAME_GENRE_MATCH,GAME_GENRE_RANKING };	
	LPCTSTR szServerType[] = { TEXT("无类型"),TEXT("金币类型"),TEXT("约战类型"),TEXT("比赛类型"),TEXT("排位类型")};

	for (int i = 0; i < CountArray(wServerType); i++)
	{
		pComboBox->SetItemData(pComboBox->InsertString(i,szServerType[i]), wServerType[i]);
	}

	//默认选择
	pComboBox->SetCurSel(1);

	//房间等级
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_SERVER_LEVEL);
	LPCTSTR pszServerLevel[] = { TEXT("爱好者"),TEXT("初级"),TEXT("高级"),TEXT("专业"),TEXT("高手") };

	//构造级别
	for (INT i = 0; i < CountArray(pszServerLevel); i++)
	{
		WORD wServerLevel = i + 1;
		pComboBox->SetItemData(pComboBox->InsertString(i, pszServerLevel[i]), wServerLevel);
	}

	//默认选择
	pComboBox->SetCurSel(0);

	//游戏列表
	pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_GAME_NAME);

	//遍历游戏
	auto pStockGameItem = m_StockGameList.GetHeadStockItem();
	while (pStockGameItem != NULL)
	{
		pComboBox->SetItemDataPtr(pComboBox->AddString(pStockGameItem->szGameName), pStockGameItem);
		pStockGameItem = m_StockGameList.GetNextStockItem(pStockGameItem);
	}

	//默认选择
	pComboBox->SetCurSel(0);

	//模拟选择
	OnCbnSelchangeComboGameName();

	////测试变量
	/*SetDlgItemInt(IDC_USER_COUNT, 1);
	SetDlgItemInt(IDC_START_INDEX, 1);	

	SetDlgItemText(IDC_ACCOUNTS, TEXT("机器人"));
	SetDlgItemText(IDC_PASSWORD, TEXT("3112546"));	

	SetDlgItemText(IDC_SERVER_PORT, TEXT("9500"));
	SetDlgItemText(IDC_SERVER_ADDR, TEXT("172.16.11.115"));*/

	//测试变量
	SetDlgItemInt(IDC_USER_COUNT, 1);
	SetDlgItemInt(IDC_START_INDEX, 1);

	SetDlgItemText(IDC_ACCOUNTS, TEXT("机器人"));
	SetDlgItemText(IDC_PASSWORD, TEXT("3112546"));

	SetDlgItemInt(IDC_SERVER_PORT, m_wServerPort);
	SetDlgItemText(IDC_SERVER_ADDR, m_szServerAddr);

	return TRUE;
}

//消息解释
BOOL CSimulateLinkDlg::PreTranslateMessage(MSG* pMsg)
{
	//按键过虑
	if (pMsg->message == WM_KEYDOWN)
	{
		if ((pMsg->wParam == VK_ESCAPE) || (pMsg->wParam == VK_RETURN)) return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//停止按钮
VOID CSimulateLinkDlg::OnBnClickedStop()
{
	//设置变量
	m_wConnectIndex = 0;	
	g_GlobalUnits.m_bConnectStatus = false;

	//设置按钮
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	GetDlgItem(IDC_START)->EnableWindow(TRUE);

	//关闭连接
	for (int i = 0; i < m_wAccountsNum; i++)
	{
		CSimulateClient* pSimulateClient = m_SimulateClientArray[i];
		if (pSimulateClient != NULL)
		{
			pSimulateClient->CloseCleint();
		}
	}	

	return;
}

//开始按钮
VOID CSimulateLinkDlg::OnBnClickedStart()
{
	//变量定义
	WORD wStartIndex = 0;
	TCHAR szAccounts[32] = TEXT("");
	TCHAR szPassword[33] = TEXT("");
	TCHAR szModuleDllName[64] = TEXT("");

	//账号信息
	wStartIndex = (WORD)GetDlgItemInt(IDC_START_INDEX);
	m_wAccountsNum = (WORD)GetDlgItemInt(IDC_USER_COUNT);
	GetDlgItemText(IDC_ACCOUNTS, szAccounts, CountArray(szAccounts));
	GetDlgItemText(IDC_PASSWORD, szPassword, CountArray(szPassword));

	//连接信息	
	g_GlobalUnits.m_wServerPort = (WORD)GetDlgItemInt(IDC_SERVER_PORT);
	GetDlgItemText(IDC_SERVER_ADDR, g_GlobalUnits.m_szServerAddr, CountArray(g_GlobalUnits.m_szServerAddr));	

	//连接模式
	UINT uButtonID = GetCheckedRadioButton(IDC_RADIO_CONCURRENT, IDC_RADIO_QUEUE);
	if (uButtonID == IDC_RADIO_QUEUE)
	{
		g_GlobalUnits.m_cbConnectMode = CONNECT_MODE_QUEUE;
	}
	else
	{
		g_GlobalUnits.m_cbConnectMode = CONNECT_MODE_CONCURRENT;
	}

	//游戏配置
	g_GlobalUnits.m_wKindID = GetDlgItemInt(IDC_GAME_KIND);		
	g_GlobalUnits.m_dwProcessVersion = GetDlgItemInt(IDC_GAME_VERSION);
	GetDlgItemText(IDC_MODULE_DLL_NAME, szModuleDllName, CountArray(szModuleDllName));	

	//起始标识
	if (szAccounts[0] == 0)
	{
		AfxMessageBox(TEXT("用户起始账号不合法，请重新输入"), MB_ICONSTOP);
		return;
	}

	//用户密码
	if (szPassword[0] == 0)
	{
		AfxMessageBox(TEXT("用户密码不合法，请重新输入"), MB_ICONSTOP);
		return;
	}

	//连接数目
	if (m_wAccountsNum == 0)
	{
		AfxMessageBox(TEXT("连接数目不合法，请重新输入"), MB_ICONSTOP);
		return;
	}

	//连接地址
	if (g_GlobalUnits.m_szServerAddr[0] == 0)
	{
		AfxMessageBox(TEXT("连接地址不合法，请重新输入"), MB_ICONSTOP);
		return;
	}

	//连接端口
	if (g_GlobalUnits.m_wServerPort == 0)
	{
		AfxMessageBox(TEXT("连接端口不合法，请重新输入"), MB_ICONSTOP);
		return;
	}

	//获取控件
	auto pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_TEST_MODE);
	if (pComboBox->GetCurSel() == -1)
	{
		AfxMessageBox(TEXT("请选择测试模式！"), MB_ICONSTOP);
		return;
	}

	//房间类型
	g_GlobalUnits.m_cbTestMode = (WORD)pComboBox->GetItemData(pComboBox->GetCurSel());

	//游戏测试
	if (g_GlobalUnits.m_cbTestMode == TEST_MODE_GAME)
	{

		//获取控件
		auto pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_SERVER_TYPE);
		if (pComboBox->GetCurSel() == -1)
		{
			AfxMessageBox(TEXT("请选择需要测试的房间！"), MB_ICONSTOP);
			return;
		}

		//房间类型
		g_GlobalUnits.m_wServerType = (WORD)pComboBox->GetItemData(pComboBox->GetCurSel());

		//获取控件
		pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_SERVER_LEVEL);
		if (pComboBox->GetCurSel() == -1)
		{
			AfxMessageBox(TEXT("请选择需要测试的房间等级！"), MB_ICONSTOP);
			return;
		}

		//房间等级
		g_GlobalUnits.m_wServerLevel = (WORD)pComboBox->GetItemData(pComboBox->GetCurSel());

		//模块名称
		if (szModuleDllName[0] == 0)
		{
			AfxMessageBox(TEXT("机器人组件名称不合法，请重新输入"), MB_ICONSTOP);
			return;
		}

		//游戏版本
		if (g_GlobalUnits.m_dwProcessVersion == 0)
		{
			AfxMessageBox(TEXT("游戏版本标识不合法，请重新输入"), MB_ICONSTOP);
			return;
		}

		//重置服务
		g_RobotUserManager->ResetUserManager();

		//设置模块
		g_RobotUserManager->SetModuleDLLName(szModuleDllName);
	}			
	
	//释放对象
	for (auto i = 0; i < m_SimulateClientArray.GetCount(); i++)
	{
		SafeDelete(m_SimulateClientArray[i]);
	}

	//移除元素
	m_SimulateClientArray.RemoveAll();

	//创建用户
	for (INT_PTR i = 0; i < m_wAccountsNum; i++)
	{
		CSimulateClient* pSimulateClient = new CSimulateClient();
		m_SimulateClientArray.Add(pSimulateClient);
	}

	//配置用户
	for (INT_PTR i = 0; i < m_wAccountsNum; i++)
	{
		CSimulateClient* pSimulateClient = m_SimulateClientArray[i];

		CString strAccounts;
		strAccounts.Format(TEXT("%s%d"), szAccounts, wStartIndex+i);
		pSimulateClient->SetAccountInfo(strAccounts.GetString(), szPassword);
	}

	//设置变量
	m_wConnectIndex = 0;
	g_GlobalUnits.m_bConnectStatus = true;

	//设置按钮
	GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_START)->EnableWindow(FALSE);

	//设置时间
	SetTimer(IDI_CONNECT, TIME_CONNECT , NULL);

	//设置时间
	SetDlgItemInt(IDC_START_TIME,(UINT)GetTickCount64());

	return;
}

//加载配置
VOID CSimulateLinkDlg::LoadConfig()
{
	//工作目录
	TCHAR szDirectory[MAX_PATH];
	CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//读取配置
	//文件目录
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\SimulateLink.ini"), szDirectory);

	//服务配置
	m_wServerPort = GetPrivateProfileInt(TEXT("ServerInfo"), TEXT("ServerPort"), m_wServerPort, szFileName);
	GetPrivateProfileString(TEXT("ServerInfo"), TEXT("ServerAddr"), m_szServerAddr, m_szServerAddr, CountArray(m_szServerAddr), szFileName);

	//清除数据
	m_StockGameList.RemoveAllItem();

	//变量定义
	TCHAR szKeyName[32];
	tagGameItem GameItem;

	for (int i = 1; ; i++)
	{
		ZeroMemory(&GameItem,sizeof(GameItem));

		//读取游戏名称		
		_sntprintf_s(szKeyName,CountArray(szKeyName),TEXT("GameName%d"),i);
		GetPrivateProfileString(TEXT("GameList"), szKeyName, TEXT(""), GameItem.szGameName, CountArray(GameItem.szGameName), szFileName);

		//读取游戏标识
		_sntprintf_s(szKeyName, CountArray(szKeyName), TEXT("GameKindID%d"), i);
		GameItem.wKindID=GetPrivateProfileInt(TEXT("GameList"), szKeyName, 0, szFileName);

		//读取游戏版本
		_sntprintf_s(szKeyName, CountArray(szKeyName), TEXT("GameVersion%d"), i);
		GameItem.dwVersionID = GetPrivateProfileInt(TEXT("GameList"), szKeyName, 0, szFileName);

		//读取组件名称
		_sntprintf_s(szKeyName, CountArray(szKeyName), TEXT("GameRobotDllName%d"), i);
		GetPrivateProfileString(TEXT("GameList"), szKeyName, TEXT(""), GameItem.szRobotDllName, CountArray(GameItem.szRobotDllName), szFileName);

		//退出判断
		if (GameItem.szGameName[0] == 0) break;

		//添加游戏
		auto pStockGameItem = m_StockGameList.CreateStockItem();
		if (pStockGameItem != NULL) CopyMemory(pStockGameItem, &GameItem, sizeof(GameItem));
	}	
}

//时间消息
VOID CSimulateLinkDlg::OnTimer(UINT_PTR nIDEvent)
{
	//连接时间
	if (nIDEvent == IDI_CONNECT)
	{
		//状态判断
		if (g_GlobalUnits.m_bConnectStatus == false) return;

		//队列模式
		if (g_GlobalUnits.m_cbConnectMode == CONNECT_MODE_QUEUE)
		{
			//启动客户端
			auto pSimulateClient = m_SimulateClientArray[m_wConnectIndex++];
			if (pSimulateClient != NULL)
			{
				pSimulateClient->StartClient(g_GlobalUnits.m_szServerAddr, g_GlobalUnits.m_wServerPort);
			}

			//完成判断
			if (m_wConnectIndex >= m_SimulateClientArray.GetCount())
			{
				KillTimer(IDI_CONNECT);
			}
		}	
		
		//并发模式
		if (g_GlobalUnits.m_cbConnectMode == CONNECT_MODE_CONCURRENT)
		{
			for (int i = 0; i < m_SimulateClientArray.GetCount(); i++)
			{
				auto pSimulateClient = m_SimulateClientArray[i];
				pSimulateClient->StartClient(g_GlobalUnits.m_szServerAddr, g_GlobalUnits.m_wServerPort);				
			}

			KillTimer(IDI_CONNECT);
		}

		return;
	}	

	__super::OnTimer(nIDEvent);
}

//状态消息
LRESULT CSimulateLinkDlg::OnStatusClientUser(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	WORD wIdleCount = 0;
	WORD wWaitCount = 0;
	WORD wConnectCount = 0;

	//变量定义
	DWORD dwPlazaCount = 0;
	DWORD dwPlayerCount = 0;

	//获取状态
	for (INT_PTR i = 0; i < m_wAccountsNum; i++)
	{
		//获取状态
		CSimulateClient* pSimulateClient = m_SimulateClientArray[i];
		BYTE cbConnectStatus = pSimulateClient->GetConnectStatus();

		//状态统计
		switch (cbConnectStatus)
		{
		case SOCKET_STATUS_IDLE: { wIdleCount++; break; }
		case SOCKET_STATUS_WAIT: { wWaitCount++; break; }
		case SOCKET_STATUS_CONNECT: 
		{ 
			wConnectCount++; 

			//校验状态		
			if (pSimulateClient->m_ModuleServer.GetLinkStatus() == true)
			{
				dwPlayerCount++;
			}
			else
			{
				dwPlazaCount++;
			}

			break; 
		}
		}		
	}

	//设置界面
	SetDlgItemInt(IDC_IDLE_COUNT, wIdleCount);
	SetDlgItemInt(IDC_CONNECT_COUNT, wConnectCount);
	SetDlgItemInt(IDC_PLAZA_COUNT, dwPlazaCount);
	SetDlgItemInt(IDC_PLAYER_COUNT, dwPlayerCount);

	//重新连接
	/*if (m_bStart == true)
	{
		CSimulateClient* pSimulateClient = (CSimulateClient*)lParam;
		if (pSimulateClient->GetConnectStatus() == SOCKET_STATUS_IDLE) pSimulateClient->StartClient(g_GlobalUnits.m_szServerAddr, g_GlobalUnits.m_wServerPort);
	}*/

	return 0L;
}

//变更事件
void CSimulateLinkDlg::OnCbnSelchangeComboGameName()
{
	//获取控件
	auto pComboBox = (CComboBox*)GetDlgItem(IDC_COMBO_GAME_NAME);
	if (pComboBox->GetCurSel() != -1)
	{
		//获取选择
		auto pStockGameItem = (tagGameItem*)pComboBox->GetItemDataPtr(pComboBox->GetCurSel());
		if (pStockGameItem != NULL)
		{
			SetDlgItemInt(IDC_GAME_KIND, pStockGameItem->wKindID);
			SetDlgItemInt(IDC_GAME_VERSION, pStockGameItem->dwVersionID);
			SetDlgItemText(IDC_MODULE_DLL_NAME, pStockGameItem->szRobotDllName);
		}
	}
}

//////////////////////////////////////////////////////////////////////////