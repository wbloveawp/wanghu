#include "StdAfx.h"
#include "GlobalUnits.h"
#include "SimulateClient.h"
#include "SimulateLinkDlg.h"


//////////////////////////////////////////////////////////////////////////
//消息映射

BEGIN_MESSAGE_MAP(CSimulateClient, CWnd)

	//系统消息
	ON_WM_TIMER()

	//登录消息
	ON_MESSAGE(WM_LOGON_LINK_SHUT, OnLogonLinkShut)
	ON_MESSAGE(WM_LOGON_LINK_FAILURE, OnLogonLinkFailure)
	ON_MESSAGE(WM_LOGON_LOGON_FINISH, OnLogonLogonFinish)
	ON_MESSAGE(WM_LOGON_LOGON_FAILURE, OnLogonLogonFailure)

	//比赛消息
	ON_MESSAGE(WM_MATCH_LINK_SHUT, OnMatchLinkShut)
	ON_MESSAGE(WM_MATCH_LINK_FAILURE, OnMatchLinkFailure)
	ON_MESSAGE(WM_MATCH_LOGON_FINISH, OnMatchLogonFinish)
	ON_MESSAGE(WM_MATCH_LOGON_FAILURE, OnMatchLogonFailure)

	//茶社消息
	ON_MESSAGE(WM_CLUB_LINK_SHUT, OnClubLinkShut)
	ON_MESSAGE(WM_CLUB_LINK_FAILURE, OnClubLinkFailure)
	ON_MESSAGE(WM_CLUB_LOGON_FINISH, OnClubLogonFinish)
	ON_MESSAGE(WM_CLUB_LOGON_FAILURE, OnClubLogonFailure)

	//茶社广场
	ON_MESSAGE(WM_CLUBPLAZA_LINK_SHUT, OnClubPlazaLinkShut)
	ON_MESSAGE(WM_CLUBPLAZA_LINK_FAILURE, OnClubPlazaLinkFailure)
	ON_MESSAGE(WM_CLUBPLAZA_LOGON_FINISH, OnClubPlazaLogonFinish)
	ON_MESSAGE(WM_CLUBPLAZA_LOGON_FAILURE, OnClubPlazaLogonFailure)

	//约战服务
	ON_MESSAGE(WM_BATTLE_LINK_SHUT, OnBattleLinkShut)
	ON_MESSAGE(WM_BATTLE_LINK_FAILURE, OnBattleLinkFailure)
	ON_MESSAGE(WM_BATTLE_LOGON_FINISH, OnBattleLogonFinish)
	ON_MESSAGE(WM_BATTLE_LOGON_FAILURE, OnBattleLogonFailure)

	//房间消息
	ON_MESSAGE(WM_SERVER_LINK_SHUT, OnServerLinkShut)
	ON_MESSAGE(WM_SERVER_LINK_FAILURE, OnServerLinkFailure)
	ON_MESSAGE(WM_SERVER_LOGON_FINISH, OnServerLogonFinish)
	ON_MESSAGE(WM_SERVER_LOGON_FAILURE, OnServerLogonFailure)
	ON_MESSAGE(WM_SERVER_MATCH_COMPLETED, OnServerMatchCompleted)	

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CSimulateClient::CSimulateClient():
	m_ModuleLogon(this),
	m_ModuleMatch(this),
	m_ModuleServer(this),
	m_ModuleBattle(this),
	m_ModuleClub(this),
	m_ModuleClubPlaza(this)
{
	//设置变量
	m_szAccounts[0] = 0;
	m_szPassword[0] = 0;
	ZeroMemory(&m_GlobalUserData, sizeof(m_GlobalUserData));

	//设置接口
	m_VirtualLinkManager.SetVirtualManagerSink(this);

	//添加链接
	m_VirtualLinkManager.InsertVirtualLink(&m_ModuleLogon);
	m_VirtualLinkManager.InsertVirtualLink(&m_ModuleMatch);
	m_VirtualLinkManager.InsertVirtualLink(&m_ModuleServer);	
	m_VirtualLinkManager.InsertVirtualLink(&m_ModuleBattle);
	m_VirtualLinkManager.InsertVirtualLink(&m_ModuleClub);
	m_VirtualLinkManager.InsertVirtualLink(&m_ModuleClubPlaza);
}

//析构函数
CSimulateClient::~CSimulateClient()
{
}

//连接通知
bool CSimulateClient::OnEventTCPSocketLink(INT nErrorCode)
{
	//连接成功
	if (nErrorCode == 0)
	{
		//打印信息
		CString strMessage;
		strMessage.Format(TEXT("Accounts=%s: 服务连接成功!"), m_szAccounts);
		InsertString(strMessage.GetString(), RGB(128, 128, 128));

		m_ModuleLogon.LogonByAccounts(m_szAccounts,m_szPassword);
	}

	//更新状态
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}	 

	return true;
}

//关闭通知
bool CSimulateClient::OnEventTCPSocketShut(BYTE cbShutReason)
{
	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 与服务器的连接断开了!"), m_szAccounts);
	InsertString(strMessage.GetString(), RGB(255, 0, 0));

	//更新状态
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return true;
}

//设置时钟
VOID CSimulateClient::SetGameTimer(UINT uTimerID, DWORD dwElapse)
{
	SetTimer(uTimerID, dwElapse, NULL);
}

//结束时钟
VOID CSimulateClient::KillGameTimer(UINT uTimerID)
{
	KillTimer(uTimerID);
}

//插入消息
VOID CSimulateClient::InsertString(LPCTSTR pszString, COLORREF crColor)
{
	//变量定义
	CHARFORMAT2 CharFormat;
	ZeroMemory(&CharFormat, sizeof(CharFormat));

	//设置变量
	CharFormat.cbSize = sizeof(CharFormat);
	CharFormat.dwMask = CFM_COLOR;
	CharFormat.crTextColor = crColor;

	//构造信息
	CString strContent;
	strContent.Format(TEXT("[%I64d]%s"), GetTickCount64(), pszString);

	//插入信息
	if (AfxGetMainWnd() != NULL)
	{
		auto pRichEditCtrl = &((CSimulateLinkDlg*)AfxGetMainWnd())->m_RichEditCtrl;
		if (pRichEditCtrl->GetLineCount()>=1000)
		{
			//删除信息
			pRichEditCtrl->SetSel(0L, -1L);
			pRichEditCtrl->ReplaceSel(NULL);			
		}

		pRichEditCtrl->InsertString(strContent.GetString(), CharFormat);
		pRichEditCtrl->InsertString(TEXT("\n"), CharFormat);
	}	

	return;
}

//时间消息
VOID CSimulateClient::OnTimer(UINT_PTR nIDEvent)
{
	//校验标识
	if (nIDEvent >= IDI_MODULE_START)
	{
		//模块列表
		IModuleItem* ModuleItemList[] = { &m_ModuleLogon,&m_ModuleMatch,&m_ModuleServer,&m_ModuleBattle,&m_ModuleClub,&m_ModuleClubPlaza};

		//校验标识
		for (int i = 0; i < CountArray(ModuleItemList); i++)
		{
			auto pIModuleItem = ModuleItemList[i];

			//登录模块
			if (nIDEvent >= pIModuleItem->GetTimerStart() && nIDEvent <= pIModuleItem->GetTimerRange())
			{
				pIModuleItem->OnEventTimer(nIDEvent - pIModuleItem->GetTimerStart(), 0);
				break;
			}
		}		
	}
	else
	{

	}

	__super::OnTimer(nIDEvent);
}

//投递消息
VOID CSimulateClient::PostWindowMessage(UINT MsgID, WPARAM wParam, LPARAM lParam)
{
	if (m_hWnd != NULL) PostMessage(MsgID, wParam, lParam);
}

//关闭客户端
VOID CSimulateClient::CloseCleint()
{
	//校验状态
	if (m_VirtualLinkManager.GetSocketStatus() == SOCKET_STATUS_CONNECT)
	{
		m_VirtualLinkManager.PerformClose();
	}	
}

//启动客户端
bool CSimulateClient::StartClient(LPCTSTR pszServerAddr, WORD wServerPort)
{
	//校验用户
	ASSERT (m_szAccounts[0]!=0 && m_szPassword[0]!=0);
	if ((m_szAccounts[0] == 0 || m_szPassword[0] == 0)) return false;

	//校验状态
	if (m_VirtualLinkManager.GetSocketStatus() == SOCKET_STATUS_CONNECT)
	{
		return false;
	}

	//创建窗口
	if (m_hWnd == NULL)
	{
		CRect rcCreate(0, 0, 0, 0);
		Create(NULL, NULL, WS_CHILD, rcCreate, GetDesktopWindow(), 100);
	}

	//发起连接
	m_VirtualLinkManager.PerformConnect(pszServerAddr, wServerPort);

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 正在连接服务中..."),m_szAccounts);
	InsertString(strMessage.GetString(),RGB(128,128,128));

	return true;
}

//设置账号
VOID CSimulateClient::SetAccountInfo(LPCTSTR pszAccounts, LPCTSTR pszPassword)
{
	//拷贝数据
	StringCchCopy(m_szAccounts,CountArray(m_szAccounts), pszAccounts);
	StringCchCopy(m_szPassword, CountArray(m_szPassword), pszPassword);
}

//链接关闭
LRESULT CSimulateClient::OnLogonLinkShut(WPARAM wParam, LPARAM lParam)
{
	//更新状态
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return 0;
}

//链接失败
LRESULT CSimulateClient::OnLogonLinkFailure(WPARAM wParam, LPARAM lParam)
{
	//更新状态
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return 0;
}

//登录完成
LRESULT CSimulateClient::OnLogonLogonFinish(WPARAM wParam, LPARAM lParam)
{
	//测试模式
	switch (g_GlobalUnits.m_cbTestMode)
	{
	case TEST_MODE_GAME:		//游戏测试
	{
		//断线重连
		if (m_GlobalUserData.wLockServerID != 0)
		{
			m_ModuleServer.EnterServer(g_GlobalUnits.m_wKindID, m_GlobalUserData.wLockServerID, g_GlobalUnits.m_wServerLevel);

			return 0;
		}

		switch (g_GlobalUnits.m_wServerType)
		{
		case GAME_GENRE_GOLD:		//金币房间
		{
			m_ModuleServer.EnterServer(g_GlobalUnits.m_wKindID, INVALID_WORD, g_GlobalUnits.m_wServerLevel);
			break;
		}
		case GAME_GENRE_MATCH:		//比赛房间
		{
			m_ModuleMatch.EnterService();
			break;
		}
		}

		break;
	}
	case TEST_MODE_CLUB:			//茶社测试
	{
		m_ModuleClubPlaza.EnterService();

		break;
	}
	case TEST_MODE_BATTLE:			//约战测试
	{
		m_ModuleBattle.EnterService();

		break;
	}
	}		

	//更新状态
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return 0;
}

//登录失败
LRESULT CSimulateClient::OnLogonLogonFailure(WPARAM wParam, LPARAM lParam)
{
	//重置模块
	m_ModuleServer.ResetModule();

	//更新状态
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return 0;
}

//链接关闭
LRESULT CSimulateClient::OnMatchLinkShut(WPARAM wParam, LPARAM lParam)
{
	m_ModuleMatch.ResetModule();

	return 0;
}

//链接失败
LRESULT CSimulateClient::OnMatchLinkFailure(WPARAM wParam, LPARAM lParam)
{
	m_ModuleMatch.ResetModule();

	return 0;
}

//登录完成
LRESULT CSimulateClient::OnMatchLogonFinish(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

//登录失败
LRESULT CSimulateClient::OnMatchLogonFailure(WPARAM wParam, LPARAM lParam)
{
	m_ModuleMatch.ResetModule();


	return 0;
}

//链接关闭
LRESULT CSimulateClient::OnClubLinkShut(WPARAM wParam, LPARAM lParam)
{
	m_ModuleClub.ResetModule();

	return 0;
}

//链接失败
LRESULT CSimulateClient::OnClubLinkFailure(WPARAM wParam, LPARAM lParam)
{
	m_ModuleClub.ResetModule();

	return 0;
}

//登录完成
LRESULT CSimulateClient::OnClubLogonFinish(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//登录失败
LRESULT CSimulateClient::OnClubLogonFailure(WPARAM wParam, LPARAM lParam)
{
	m_ModuleClub.ResetModule();

	return 0;
}

//链接关闭
LRESULT CSimulateClient::OnClubPlazaLinkShut(WPARAM wParam, LPARAM lParam)
{
	m_ModuleClubPlaza.ResetModule();

	return 0;
}

//链接失败
LRESULT CSimulateClient::OnClubPlazaLinkFailure(WPARAM wParam, LPARAM lParam)
{
	m_ModuleClubPlaza.ResetModule();

	return 0;
}

//登录完成
LRESULT CSimulateClient::OnClubPlazaLogonFinish(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//登录失败
LRESULT CSimulateClient::OnClubPlazaLogonFailure(WPARAM wParam, LPARAM lParam)
{
	m_ModuleClubPlaza.ResetModule();

	return 0;
}

//链接关闭
LRESULT CSimulateClient::OnBattleLinkShut(WPARAM wParam, LPARAM lParam)
{
	m_ModuleBattle.ResetModule();

	return 0;
}

//链接失败
LRESULT CSimulateClient::OnBattleLinkFailure(WPARAM wParam, LPARAM lParam)
{
	m_ModuleBattle.ResetModule();

	return 0;
}

//登录完成
LRESULT CSimulateClient::OnBattleLogonFinish(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

//登录失败
LRESULT CSimulateClient::OnBattleLogonFailure(WPARAM wParam, LPARAM lParam)
{
	m_ModuleBattle.ResetModule();

	return 0;
}

//链接关闭
LRESULT CSimulateClient::OnServerLinkShut(WPARAM wParam, LPARAM lParam)
{
	//重置模块
	m_ModuleServer.ResetModule();

	//获取标识
	auto wServiceID = (WORD)lParam;
	auto bGameStarted = (bool)wParam;

	//重连判断
	if (bGameStarted == true && wServiceID != 0 && g_GlobalUnits.m_bConnectStatus==true)
	{
		m_ModuleServer.EnterServer(0,wServiceID,0);
	}

	//更新状态
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return 0;
}

//链接失败
LRESULT CSimulateClient::OnServerLinkFailure(WPARAM wParam, LPARAM lParam)
{
	//关闭连接
	m_ModuleServer.CloseVirtualLink();

	//重置模块
	m_ModuleServer.ResetModule();

	switch (g_GlobalUnits.m_wServerType)
	{
	case GAME_GENRE_MATCH:		//比赛房间
	{
		m_ModuleMatch.EnterService();
		break;
	}
	}

	//更新状态
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return 0;
}

//登录完成
LRESULT CSimulateClient::OnServerLogonFinish(WPARAM wParam, LPARAM lParam)
{
	//更新状态
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return 0;
}

//登录失败
LRESULT CSimulateClient::OnServerLogonFailure(WPARAM wParam, LPARAM lParam)
{
	//更新状态
	if (AfxGetMainWnd() != NULL)
	{
		AfxGetMainWnd()->PostMessage(WM_STATUS_CLIENT_USER, 0, 0);
	}

	return 0;
}

//比赛完成
LRESULT CSimulateClient::OnServerMatchCompleted(WPARAM wParam, LPARAM lParam)
{
	//关闭房间
	m_ModuleServer.CloseVirtualLink();

	//进入比赛
	if (m_ModuleMatch.GetLinkStatus()==false) m_ModuleMatch.EnterService();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
