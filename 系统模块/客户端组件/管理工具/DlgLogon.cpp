#include "Stdafx.h"
#include "Resource.h"
#include "DlgLogon.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTabCtrlLogon, CTabCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgLogon, CSkinDialog)

	//控件消息
	ON_EN_CHANGE(IDC_PASSWORD, OnEnChangePassword)
	ON_CBN_SELCHANGE(IDC_GAME_ID, OnSelchangeUserID)
	ON_CBN_SELCHANGE(IDC_ACCOUNTS, OnSelchangeAccounts)
	ON_CBN_EDITCHANGE(IDC_ACCOUNTS, OnEditChangeAccounts)
	ON_CBN_EDITCHANGE(IDC_GAME_ID, OnEditChangeUserID)
	ON_CBN_DROPDOWN(IDC_ACCOUNTS, OnCBDrawDownAccounts)
	ON_CBN_DROPDOWN(IDC_GAME_ID, OnCBDrawDownGameID)

	//按钮消息
	ON_BN_CLICKED(IDC_DELETE_RECORD, OnBnClickedDelete)
	ON_BN_CLICKED(IDC_PROXY_TEST, OnBnClickedProxyTest)
	ON_BN_CLICKED(IDC_PROXY_OPTION, OnBnClickedProxyOption)
	ON_BN_CLICKED(IDC_REM_PASSWORD, OnBnClickedRemPassword)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CTabCtrlLogon::CTabCtrlLogon()
{
}

//析够函数
CTabCtrlLogon::~CTabCtrlLogon()
{
}

//重画消息
void CTabCtrlLogon::OnPaint()
{
	///变量定义
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//颜色定义
	COLORREF crTXSelect=RGB(0,0,0);
	COLORREF crTXNormal=RGB(0,0,0);
	COLORREF crBackGround=CSkinDialog::m_SkinAttribute.m_crBackGround;
	COLORREF crControlBorder=RGB(30,153,255);

	//绘画背景
	CRect rcClip;
	dc.GetClipBox(&rcClip);
	dc.FillSolidRect(&rcClip,crBackGround);

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());

	//绘画边框
	rcClient.top+=18;
	dc.Draw3dRect(&rcClient,crControlBorder,crControlBorder);

	//绘画按钮
	CPen LinePen(PS_SOLID,1,crControlBorder);
	CPen * pOldPen=dc.SelectObject(&LinePen);

	for (INT i=0;i<GetItemCount();i++)
	{
		//变量定义
		TCITEM ItemInfo;
		ZeroMemory(&ItemInfo,sizeof(ItemInfo));

		//变量定义
		TCHAR szBuffer[100];
		ItemInfo.mask=TCIF_TEXT|TCIF_IMAGE;
		ItemInfo.pszText=szBuffer;
		ItemInfo.cchTextMax=sizeof(szBuffer);

		//获取信息
		CRect rcItem;
		GetItem(i,&ItemInfo);
		GetItemRect(i,&rcItem);

		//填充背景
		dc.FillSolidRect(&rcItem,crBackGround);

		//绘画边框
		dc.MoveTo(rcItem.left,rcItem.bottom-1);
		dc.LineTo(rcItem.left,rcItem.top+2);
		dc.LineTo(rcItem.left+2,rcItem.top);
		dc.LineTo(rcItem.right-2,rcItem.top);
		dc.LineTo(rcItem.right,rcItem.top+2);
		dc.LineTo(rcItem.right,rcItem.bottom-1);

		//绘画边框
		if (i!=GetCurSel())
		{
			rcItem.top+=2;
			dc.SetTextColor(crTXNormal);
			dc.MoveTo(rcItem.left,rcItem.bottom-1);
			dc.LineTo(rcItem.right,rcItem.bottom-1);
			DrawText(dc,szBuffer,lstrlen(szBuffer),&rcItem,DT_VCENTER|DT_SINGLELINE|DT_CENTER);
		}
		else 
		{
			rcItem.top+=3;
			dc.SetTextColor(crTXSelect);
			DrawText(dc,szBuffer,lstrlen(szBuffer),&rcItem,DT_VCENTER|DT_SINGLELINE|DT_CENTER);
		}
	}

	//清理资源
	dc.SelectObject(pOldPen);
	LinePen.DeleteObject();

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDlgLogon::CDlgLogon() : CSkinDialog(IDD_DLG_LOGON)
{
	//登录信息
	m_dwGameID=0;
	m_szAccounts[0]=0;
	m_szPassword[0]=0;
	m_szLogonServer[0]=0;
	m_szAccountTitle[0]=0;

	//状态变量
	m_bEditPass=false;
	m_bEditAccount = false;
	m_bEditUserId = false;
	m_bProxyMode=false;
	m_cbVerifyMode=0;

	//模式变量
	m_LogonMode=LogonMode_Accounts;
	m_NetworkKind=NetworkKind_Default;

	//位置信息
	m_nFullWidth=0;
	m_nFullHeight=0;

	return;
}

//析构函数
CDlgLogon::~CDlgLogon()
{
}

//控件绑定
VOID CDlgLogon::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//绑定按钮
	DDX_Control(pDX, IDOK, m_btLogon);
	DDX_Control(pDX, IDC_PROXY_TEST, m_btProxyTest);
	DDX_Control(pDX, IDC_PROXY_OPTION, m_btProxyOption);
	DDX_Control(pDX, IDC_PROXY_TYPE, m_cmProxyServerType);

	//绑定编辑
	DDX_Control(pDX, IDC_GAME_ID, m_cmGameID);
	DDX_Control(pDX, IDC_ACCOUNTS, m_cmAccounts);
	DDX_Control(pDX, IDC_PASSWORD, m_edPassword);
	DDX_Control(pDX, IDC_PROXY_PORT, m_edProxyPort);
	DDX_Control(pDX, IDC_LOGON_SERVER, m_cmLogonServer);
	DDX_Control(pDX, IDC_PROXY_SERVER, m_edProxyServer);
	DDX_Control(pDX, IDC_PROXY_USER, m_edProxyUserName);
	DDX_Control(pDX, IDC_PROXY_PASS, m_edProxyPassword);
}

//初始化函数
BOOL CDlgLogon::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("游戏登录"));
	
	//设置标题
	if (m_ImageTitle.IsNull()==true)
	{
		m_ImageTitle.LoadImage(AfxGetInstanceHandle(),TEXT("TITLE_DLG_LOGON"));
	}

	//变量定义
	TCHAR szBrowseUrl[MAX_PATH]=TEXT("");
	CGlobalStation * pGlobalStation=CGlobalStation::GetInstance();
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

	//工作目录
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//构造路径
	TCHAR szAssistantPath[MAX_PATH]=TEXT("");

	// add by humroly 2018/8/3
	// replace func
	_sntprintf_s(szAssistantPath,CountArray(szAssistantPath),TEXT("%s\\Collocate\\Collocate.INI"),szDirectory);

	//限制输入
	((CEdit *)(GetDlgItem(IDC_PROXY_PORT)))->LimitText(5);
	((CEdit *)(GetDlgItem(IDC_PROXY_USER)))->LimitText(31);
	((CEdit *)(GetDlgItem(IDC_PROXY_PASS)))->LimitText(31);
	((CEdit *)(GetDlgItem(IDC_PROXY_SERVER)))->LimitText(31);
	((CComboBox *)(GetDlgItem(IDC_GAME_ID)))->LimitText(11);
	((CEdit *)(GetDlgItem(IDC_PASSWORD)))->LimitText(LEN_PASSWORD-1);
	((CComboBox *)(GetDlgItem(IDC_ACCOUNTS)))->LimitText(LEN_ACCOUNTS-1);

	//登录模式
	SwitchLogonMode(pParameterGlobal->m_LogonMode);

	//记住密码
	bool bRemPassword=(pParameterGlobal->m_AcountsRule==AcountsRule_AccountsAndPass);
	if (bRemPassword==true) ((CButton *)GetDlgItem(IDC_REM_PASSWORD))->SetCheck(BST_CHECKED);

	//加载信息
	LoadAccountsInfo();
	LoadLogonServerInfo();
	LoadProxyServerInfo();

	m_LogonMode=LogonMode_Accounts;
	
	//登录模式
	SwitchLogonMode(m_LogonMode);

	//框架位置
	CRect rcFrame;
	GetWindowRect(&m_rcNormalFrame);
	GetDlgItem(IDC_BORDER)->GetWindowRect(&rcFrame);

	//记录位置
	m_nFullWidth=m_rcNormalFrame.Width();
	m_nFullHeight=m_rcNormalFrame.Height();

	//调整位置
	m_rcNormalFrame.bottom=rcFrame.top;
	SetWindowPos(NULL,m_rcNormalFrame.left,m_rcNormalFrame.top,m_rcNormalFrame.Width(),m_rcNormalFrame.Height(),SWP_NOZORDER);

	//构造地址
	TCHAR szBillUrl[256]=TEXT("");
	pGlobalStation->GetRedirectionLink(JUMP_ID_AD_LOGON,szBillUrl,CountArray(szBillUrl));

	//居中窗口
	CenterWindow();

	return FALSE;
}

//命令函数
BOOL CDlgLogon::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD dwCommandID = HIWORD(wParam);
	UINT nCommandID = LOWORD(wParam);

	//取消连接
	if (nCommandID == IDM_GIVE_UP_CONNECT)
	{
		//执行登录
		CSessionLogon* pMissionLogon = CSessionLogon::GetInstance();
		if (pMissionLogon != NULL)
		{
			pMissionLogon->CloseStatusInfo();
			pMissionLogon->RestoreShow();
		}

		return TRUE;
	}

	return __super::OnCommand(wParam, lParam);
}

//消息解释
BOOL CDlgLogon::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//确定函数
VOID CDlgLogon::OnOK()
{
	//获取信息
	if (GetInformation()==false) return;

	//隐藏窗口
	ShowWindow(SW_HIDE);

	//执行登录
	CSessionLogon * pMissionLogon=CSessionLogon::GetInstance();
	if (pMissionLogon!=NULL) pMissionLogon->PerformLogonMission(MT_LOGON);

	return;
}

//取消消息
VOID CDlgLogon::OnCancel()
{
	//关闭窗口
	DestroyWindow();
	AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);

	return;
}

//构造数据
WORD CDlgLogon::ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize)
{
	//变量定义
	TCHAR szPassword[LEN_MD5];
	CMD5Encrypt::EncryptData(m_szPassword,szPassword);

	//发送数据
	switch (m_LogonMode)
	{
	case LogonMode_GameID:			//I D 登录
		{
			//变量定义
			CGlobalStation * pGlobalStation=CGlobalStation::GetInstance();
			CMD_GP_LogonByGameID * pLogonByGameID=(CMD_GP_LogonByGameID *)cbBuffer;

			//构造数据
			pLogonByGameID->dwGameID=m_dwGameID;
			//pLogonByGameID->dwStationID=pGlobalStation->GetStationID();
			pLogonByGameID->dwPlazaVersion = 0; // g_GlobalUnits.GetPlazaVersion();
			lstrcpynW(pLogonByGameID->szPassword,CT2CW(szPassword),CountArray(pLogonByGameID->szPassword));
			
			return sizeof(CMD_GP_LogonByGameID);
		}
	case LogonMode_Accounts:		//帐号登录
		{
			//变量定义
			CGlobalStation * pGlobalStation=CGlobalStation::GetInstance();
			CMD_GP_LogonByAccounts * pLogonByAccounts=(CMD_GP_LogonByAccounts *)cbBuffer;

			//构造数据
			//pLogonByAccounts->dwStationID=pGlobalStation->GetStationID();
			pLogonByAccounts->dwPlazaVersion = 0; // g_GlobalUnits.GetPlazaVersion();
			lstrcpynW(pLogonByAccounts->szPassword,CT2CW(szPassword),CountArray(pLogonByAccounts->szPassword));
			lstrcpynW(pLogonByAccounts->szAccounts,CT2CW(m_szAccounts),CountArray(pLogonByAccounts->szAccounts));

			return sizeof(CMD_GP_LogonByAccounts);
		}
	case LogonMode_BBS_Accounts:	//论坛帐号
		{
			//变量定义
			//CGlobalStation *pGlobalStation=CGlobalStation::GetInstance();
			//CMD_GP_LogonByBBSAccounts * pLogonByBBSAccounts=(CMD_GP_LogonByBBSAccounts *)cbBuffer;

			////构造数据
			//pLogonByBBSAccounts->dwStationID=pGlobalStation->GetStationID();
			//pLogonByBBSAccounts->dwPlazaVersion=g_GlobalUnits.GetPlazaVersion();
			//lstrcpynW(pLogonByBBSAccounts->szPassword,CT2CW(szPassword),CountArray(pLogonByBBSAccounts->szPassword));
			//lstrcpynW(pLogonByBBSAccounts->szAccounts,CT2CW(m_szAccounts),CountArray(pLogonByBBSAccounts->szAccounts));
   //         pLogonByBBSAccounts->cbVerifyMode=m_cbVerifyMode;

   //         return sizeof(CMD_GP_LogonByBBSAccounts);
		}
	}

	return 0;
}

//读取帐号
VOID CDlgLogon::LoadAccountsInfo()
{
	//清除信息
	for (int i = m_cmAccounts.GetCount() - 1; i >= 0; i--)
	{
		m_cmAccounts.DeleteString(i);
	}

	//读取信息
	CRegKeyAide RegUserInfo;
	if (RegUserInfo.OpenRegKey(REG_USER_INFO,false)==true)
	{
		DWORD dwIndex=0L,dwUserID=0L;
		TCHAR szKeyItemName[32],szReadBuffer[32];
		do
		{
			//读取键项
			if (RegUserInfo.EnumItemString(dwIndex++,szKeyItemName,sizeof(szKeyItemName))==false) break;

			//获取索引
			dwUserID=_ttol(szKeyItemName);
			if (dwUserID==0L) continue;

			//打开子键
			CRegKeyAide RegUserItem;
			if (RegUserItem.Attach(RegUserInfo.OpenItemKey(szKeyItemName))==NULL) continue;

			//站点 I D
			szReadBuffer[0]=0;
			RegUserItem.GetString(TEXT("StationID"),szReadBuffer,CountArray(szReadBuffer));
			DWORD dwUserStationID=(DWORD)_ttoi(szReadBuffer);

            //站点判断
			if(dwUserStationID!=0 && !g_GlobalUnits.m_GlobalStation.IsCompStation(dwUserStationID)) continue;

			//游戏 I D
			szReadBuffer[0]=0;
			RegUserItem.GetString(TEXT("GameID"),szReadBuffer,CountArray(szReadBuffer));
			if ((szReadBuffer[0]!=0)&&(ComboBoxFindString(&m_cmGameID,szReadBuffer)==LB_ERR))
			{
				m_cmGameID.SetItemData(m_cmGameID.AddString(szReadBuffer),dwUserID);
			}

			//用户帐号
			szReadBuffer[0]=0;
			if(m_LogonMode==LogonMode_BBS_Accounts)
				RegUserItem.GetString(TEXT("BBSAccount"),szReadBuffer,CountArray(szReadBuffer));
			else
				RegUserItem.GetString(TEXT("UserAccount"),szReadBuffer,CountArray(szReadBuffer));
			if ((szReadBuffer[0]!=0)&&(ComboBoxFindString(&m_cmAccounts,szReadBuffer)==LB_ERR))
			{
				m_cmAccounts.SetItemData(m_cmAccounts.AddString(szReadBuffer),dwUserID);
			}

		} while (true);

		//上次用户
		if ((dwUserID=RegUserInfo.GetValue(TEXT("LastUserID"),0L))!=0L)
		{
			INT nItemCount=m_cmAccounts.GetCount();
			for (INT nIndex=0;nIndex<nItemCount;nIndex++)
			{
				if (m_cmAccounts.GetItemData(nIndex)==dwUserID)
				{
					SetCurrentUserID(dwUserID);
					break;
				}
			}
		}
	}

	//设置选择
	if ((m_cmAccounts.GetCurSel()==LB_ERR)&&(m_cmAccounts.GetCount()>0))
	{
		SetCurrentUserID((DWORD)m_cmAccounts.GetItemData(0));
	}

	return;
}

//地址信息
VOID CDlgLogon::LoadLogonServerInfo()
{
	//读取数目
	CGlobalServer GlobalServer;
	WORD wItemCount=GlobalServer.GetItemCount();

	//上次登录
	TCHAR szLastServer[LEN_SERVER]=TEXT("");
	GlobalServer.GetLastServerName(szLastServer);

	//插入信息
	if (szLastServer[0]!=0) m_cmLogonServer.SetCurSel(m_cmLogonServer.AddString(szLastServer));

	//读取信息
	for (WORD i=0;i<wItemCount;i++)
	{
		//读取信息
		TCHAR szServerItem[LEN_SERVER]=TEXT("");
		GlobalServer.GetServerItemName(i,szServerItem);

		//插入信息
		if ((szServerItem[0]!=0)&&(ComboBoxFindString(&m_cmLogonServer,szServerItem)==LB_ERR))
		{
			m_cmLogonServer.AddString(szServerItem);
		}
	}

	//默认填充
	if (m_cmLogonServer.GetCount()==0)
	{
		lstrcpyn(szLastServer,szLogonServer,CountArray(szLastServer));
		m_cmLogonServer.SetCurSel(m_cmLogonServer.AddString(szLastServer));
	}

	//设置选择
	if (m_cmLogonServer.GetCurSel()==LB_ERR) m_cmLogonServer.SetCurSel(0);

	return;
}

//代理信息
VOID CDlgLogon::LoadProxyServerInfo()
{
	//变量定义
	LPCTSTR szProxyType[]={TEXT("不使用代理"),TEXT("HTTP 代理"),TEXT("SOCKS 4"),TEXT("SOCKS 5")};
	enProxyServerType ProxyServerType[]={ProxyType_None,ProxyType_Http,ProxyType_Socks4,ProxyType_Socks5};

	//获取信息
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();
	enProxyServerType CurrentProxyType=pParameterGlobal->m_ProxyServerType;
	tagProxyServerInfo & ProxyServerInfo=pParameterGlobal->m_ProxyServerInfo;

	//加载类型
	for (INT i=0;i<CountArray(szProxyType);i++)
	{
		ASSERT(i<CountArray(ProxyServerType));
		INT nItem=m_cmProxyServerType.AddString(szProxyType[i]);
		m_cmProxyServerType.SetItemData(nItem,ProxyServerType[i]);
		if (CurrentProxyType==ProxyServerType[i]) m_cmProxyServerType.SetCurSel(nItem);
	}

	//设置信息
	SetDlgItemText(IDC_PROXY_USER,ProxyServerInfo.szUserName);
	SetDlgItemText(IDC_PROXY_PASS,ProxyServerInfo.szPassword);
	SetDlgItemText(IDC_PROXY_SERVER,ProxyServerInfo.szProxyServer);
	if (ProxyServerInfo.wProxyPort!=0) SetDlgItemInt(IDC_PROXY_PORT,ProxyServerInfo.wProxyPort);

	//设置选择
	if (m_cmProxyServerType.GetCurSel()==CB_ERR) m_cmProxyServerType.SetCurSel(0);

	return;
}

//查找数据
INT CDlgLogon::ComboBoxFindString(CComboBox * pComboBox, LPCTSTR pszFindString)
{
	//变量定义
	CString strBuffer;
	INT nStringLen=lstrlen(pszFindString);
	INT nItemHandle=LB_ERR,nItemFind=LB_ERR;

	do 
	{
		//查找数据
		nItemFind=pComboBox->FindString((nItemHandle==LB_ERR)?0:(nItemHandle+1),pszFindString);
		if ((nItemFind==LB_ERR)||(nItemHandle==nItemFind)||(nItemFind<nItemHandle)) return LB_ERR;

		//数据对比
		nItemHandle=nItemFind;
		pComboBox->GetLBText(nItemHandle,strBuffer);
		if( !lstrcmp(pszFindString,strBuffer) ) return nItemHandle;
	} while (true);

	return LB_ERR;
}

//获取信息
bool CDlgLogon::GetInformation()
{
	//登录地址
	CString strLogonServer;
	GetDlgItemText(IDC_LOGON_SERVER,strLogonServer);
	strLogonServer.TrimLeft();strLogonServer.TrimRight();

	//地址判断
	if (strLogonServer.IsEmpty()==true)
	{
		ShowInformation(TEXT("请选择“登录服务器”，若遇到困难请点击“新手指南”查询帮助！"),MB_ICONERROR,0);
		m_cmLogonServer.SetFocus();
		return false;
	}
	else lstrcpyn(m_szLogonServer,strLogonServer,CountArray(m_szLogonServer));

	//登录帐号
	switch (m_LogonMode)
	{
	case LogonMode_GameID:		//ID 登录
		{
			//获取帐号
			CString strUserID;
			GetDlgItemText(IDC_GAME_ID,strUserID);
			strUserID.TrimLeft();strUserID.TrimRight();

			//帐号判断
			m_dwGameID=_ttol(strUserID);
			if (m_dwGameID==0L)
			{
				ShowInformation(TEXT("游戏 ID 不能为空，请重新输入！"),MB_ICONERROR,0);
				m_cmGameID.SetFocus();
				return false;
			}

			break;
		}
	case LogonMode_Accounts:	//帐号登录
	case LogonMode_BBS_Accounts://论坛登录
		{
			//获取帐号
			CString strAccount;
			GetDlgItemText(IDC_ACCOUNTS,strAccount);
			strAccount.TrimLeft();strAccount.TrimRight();

			//帐号判断
			if (strAccount.IsEmpty())
			{
				ShowInformation(TEXT("用户帐号不能为空，请重新输入！"),MB_ICONERROR,0);
				m_cmAccounts.SetFocus();
				return false;
			}

			//设置变量
			lstrcpyn(m_szAccounts,strAccount,CountArray(m_szAccounts));

			break;
		}
	}

	//用户密码
	if (m_bEditPass==true)
	{
		m_bEditPass=false;
		m_edPassword.GetWindowText(m_szPassword,CountArray(m_szPassword));
	}
	if (m_szPassword[0]==0)
	{
		ShowInformation(TEXT("登录密码不能为空，请重新输入登录密码！"),MB_ICONERROR,30);
		m_edPassword.SetFocus();
		return false;
	}

	//代理类型
	CComboBox * pComProxyType=(CComboBox *)GetDlgItem(IDC_PROXY_TYPE);
	enProxyServerType ProxyServerType=(enProxyServerType)pComProxyType->GetItemData(pComProxyType->GetCurSel());

	//代理信息
	tagProxyServerInfo ProxyServerInfo;
	ZeroMemory(&ProxyServerInfo,sizeof(ProxyServerInfo));
	ProxyServerInfo.wProxyPort=GetDlgItemInt(IDC_PROXY_PORT);
	GetDlgItemText(IDC_PROXY_USER,ProxyServerInfo.szUserName,CountArray(ProxyServerInfo.szUserName));
	GetDlgItemText(IDC_PROXY_PASS,ProxyServerInfo.szPassword,CountArray(ProxyServerInfo.szPassword));
	GetDlgItemText(IDC_PROXY_SERVER,ProxyServerInfo.szProxyServer,CountArray(ProxyServerInfo.szProxyServer));

	//效验代理
	if (ProxyServerType!=ProxyType_None)
	{
		//代理地址
		if (ProxyServerInfo.szProxyServer[0]==0)
		{
			ShowInformation(TEXT("代理服务器地址不能为空，请重新输入！"),MB_ICONERROR,30);
			if (m_bProxyMode==false) SwitchProxyMode(true);
			m_edProxyServer.SetFocus();
			return false;
		}

		//代理端口
		if (ProxyServerInfo.wProxyPort==0)
		{
			ShowInformation(TEXT("请输入代理服务器端口号，例如：1080！"),MB_ICONERROR,30);
			if (m_bProxyMode==false) SwitchProxyMode(true);
			m_edProxyPort.SetFocus();
			return false;
		}
	}

	//变量定义
	ASSERT(CParameterGlobal::GetInstance()!=NULL);
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

	//保存配置
	if (pParameterGlobal!=NULL)
	{
		pParameterGlobal->m_LogonMode=m_LogonMode;
		pParameterGlobal->m_ProxyServerType=ProxyServerType;
		pParameterGlobal->m_ProxyServerInfo=ProxyServerInfo;
	}

	return true;
}

//设置选择
VOID CDlgLogon::SetCurrentUserID(DWORD dwUserID)
{
	//更新选择
	UINT uComboBoxIDs[]={IDC_ACCOUNTS,IDC_GAME_ID};
	for (INT i=0;i<CountArray(uComboBoxIDs);i++)
	{
		//获取控件
		CComboBox * pComboBox=(CComboBox *)GetDlgItem(uComboBoxIDs[i]);

		//搜索数据
		INT nItemCount=pComboBox->GetCount();
		for (INT nIndex=0;nIndex<nItemCount;nIndex++)
		{
			if (dwUserID==(DWORD)pComboBox->GetItemData(nIndex))
			{
				pComboBox->SetCurSel(nIndex);
				break;
			}
		}
	}

	//更新密码
	UpdateUserPassWord(dwUserID);

	return;
}

//更新密码
VOID CDlgLogon::UpdateUserPassWord(DWORD dwUserID)
{
	//读取密码
	if (dwUserID!=0L)
	{
		//构造键名
		TCHAR szKeyName[64]=TEXT("");
		// add by humroly 2018/8/3
		// replace func
		_sntprintf_s(szKeyName, CountArray(szKeyName), TEXT("%s\\%ld"), REG_USER_INFO, dwUserID);

		//打开注册表
		CRegKeyAide RegPassWord;
		if (RegPassWord.OpenRegKey(szKeyName,false)==true)
		{
			//读取密码
			TCHAR szPassBuffer[256]=TEXT("");
			if(m_LogonMode==LogonMode_BBS_Accounts)
				RegPassWord.GetString(TEXT("BBSPassword"),szPassBuffer,CountArray(szPassBuffer));
			else
				RegPassWord.GetString(TEXT("UserPassword"),szPassBuffer,CountArray(szPassBuffer));

			//设置密码
			CXOREncrypt::CrevasseData(szPassBuffer,m_szPassword,CountArray(m_szPassword));
			m_edPassword.SetWindowText((m_szPassword[0]==0)?TEXT(""):TEXT("**********"));

			//设置变量
			m_bEditPass=false;

			((CButton *)GetDlgItem(IDC_REM_PASSWORD))->SetCheck((m_szPassword[0]==0?BST_UNCHECKED:BST_CHECKED));
			OnBnClickedRemPassword();

			return;
		}
	}

	//设置密码
	m_edPassword.SetWindowText(TEXT(""));

	//记录密码标志
	((CButton *)GetDlgItem(IDC_REM_PASSWORD))->SetCheck(BST_UNCHECKED);
	OnBnClickedRemPassword();

	//设置变量
	m_szPassword[0]=0;
	m_bEditPass=false;
	
	return;
}

//代理模式
VOID CDlgLogon::SwitchProxyMode(bool bProxyMode)
{
	//设置变量
	m_bProxyMode=bProxyMode;

	//控件设置
	GetDlgItem(IDC_PROXY_TYPE)->EnableWindow(m_bProxyMode);
	GetDlgItem(IDC_PROXY_PORT)->EnableWindow(m_bProxyMode);
	GetDlgItem(IDC_PROXY_USER)->EnableWindow(m_bProxyMode);
	GetDlgItem(IDC_PROXY_PASS)->EnableWindow(m_bProxyMode);
	GetDlgItem(IDC_PROXY_TEST)->EnableWindow(m_bProxyMode);
	GetDlgItem(IDC_PROXY_SERVER)->EnableWindow(m_bProxyMode);

	//移动位置
	INT nHeight=m_bProxyMode?m_nFullHeight:m_rcNormalFrame.Height();
	SetWindowPos(NULL,0,0,m_nFullWidth,nHeight,SWP_NOMOVE|SWP_NOZORDER|SWP_NOCOPYBITS);

	return;
}

//切换模式
VOID CDlgLogon::SwitchLogonMode(enLogonMode LogonMode)
{
	//设置变量
	m_LogonMode=LogonMode;

	LoadAccountsInfo();
	//设置界面
	switch (m_LogonMode)
	{
	case LogonMode_Accounts:	//帐号登录
		{
			//设置控件
			m_cmGameID.ShowWindow(SW_HIDE);
			m_cmAccounts.ShowWindow(SW_SHOW);
			SetDlgItemText(IDC_TYPE_NAME,TEXT("游戏帐号："));

			//设置焦点
			if (GetDlgItem(IDC_ACCOUNTS)->GetWindowTextLength()>0)
			{
				m_edPassword.SetFocus();
				m_edPassword.SetSel(0,-1);
			}
			else GetDlgItem(IDC_ACCOUNTS)->SetFocus();

			break;
		}
	}

	return;
}

//代理测试
VOID CDlgLogon::OnBnClickedProxyTest()
{
	//代理类型
	CComboBox * pComProxyType=(CComboBox *)GetDlgItem(IDC_PROXY_TYPE);
	enProxyServerType ProxyServerType=(enProxyServerType)pComProxyType->GetItemData(pComProxyType->GetCurSel());

	//代理信息
	tagProxyServerInfo ProxyServerInfo;
	ZeroMemory(&ProxyServerInfo,sizeof(ProxyServerInfo));
	ProxyServerInfo.wProxyPort=GetDlgItemInt(IDC_PROXY_PORT);
	GetDlgItemText(IDC_PROXY_USER,ProxyServerInfo.szUserName,CountArray(ProxyServerInfo.szUserName));
	GetDlgItemText(IDC_PROXY_PASS,ProxyServerInfo.szPassword,CountArray(ProxyServerInfo.szPassword));
	GetDlgItemText(IDC_PROXY_SERVER,ProxyServerInfo.szProxyServer,CountArray(ProxyServerInfo.szProxyServer));

	//效验代理
	if (ProxyServerType!=ProxyType_None)
	{
		//代理地址
		if (ProxyServerInfo.szProxyServer[0]==0)
		{
			ShowInformation(TEXT("代理服务器地址不能为空，请重新输入！"),MB_ICONERROR,30);
			if (m_bProxyMode==false) SwitchProxyMode(true);
			m_edProxyServer.SetFocus();
			return;
		}

		//代理端口
		if (ProxyServerInfo.wProxyPort==0)
		{
			ShowInformation(TEXT("请输入代理服务器端口号，例如：1080！"),MB_ICONERROR,30);
			if (m_bProxyMode==false) SwitchProxyMode(true);
			m_edProxyPort.SetFocus();
			return;
		}
	}
	else 
	{
		//错误提示
		ShowInformation(TEXT("请先配置代理服务器连接信息！"),MB_ICONERROR,30);
		pComProxyType->SetFocus();
		return;
	}

	//创建组件
	CNetworkManagerHelper NetworkManagerModule;
	if (NetworkManagerModule.CreateInstance()==false)
	{
		ShowInformation(TEXT("网络服务管理组件创建失败，测试失败！"),MB_ICONERROR,30);
		return;
	}

	//代理测试
	switch (NetworkManagerModule->ProxyServerTesting(ProxyServerType,ProxyServerInfo))
	{
	case CONNECT_SUCCESS:				//连接成功
		{
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("代理服务器工作正常！"),MB_ICONINFORMATION);
			return;
		}
	case CONNECT_PROXY_USER_INVALID:	//用户错误
		{
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("代理服务器用户名或者密码错误！"),MB_ICONERROR);
			return;
		}
	default:							//默认处理
		{
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("代理服务器不存在或者连接失败！"),MB_ICONERROR);
			return;
		}
	}

	return;
}

//网络设置
VOID CDlgLogon::OnBnClickedProxyOption()
{
	//设置模式
	SwitchProxyMode(!m_bProxyMode);

	return;
}

//记住密码
VOID CDlgLogon::OnBnClickedRemPassword()
{
	//变量定义
	ASSERT(CParameterGlobal::GetInstance()!=NULL);
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

	//获取配置
	bool bRemPassword=(((CButton *)GetDlgItem(IDC_REM_PASSWORD))->GetCheck()==BST_CHECKED);

	//写入配置
	if (bRemPassword==true)
	{
		pParameterGlobal->m_AcountsRule=AcountsRule_AccountsAndPass;
	}

	//写入配置
	if ((bRemPassword==false)&&(pParameterGlobal->m_AcountsRule==AcountsRule_AccountsAndPass))
	{
		pParameterGlobal->m_AcountsRule=AcountsRule_Accounts;
	}

	return;
}

//密码改变
VOID CDlgLogon::OnEnChangePassword()
{
	m_bEditPass=true;
}

//选择改变
VOID CDlgLogon::OnSelchangeAccounts()
{
	//设置变量
	m_bEditAccount = false;

	//变量定义
	CComboBox * pComBoxAccounts=(CComboBox *)GetDlgItem(IDC_ACCOUNTS);

	//获取选择
	INT nCurrentSel=pComBoxAccounts->GetCurSel();
	if (nCurrentSel!=LB_ERR) SetCurrentUserID((DWORD)pComBoxAccounts->GetItemData(nCurrentSel));

	return;
}

//选择改变
VOID CDlgLogon::OnSelchangeUserID()
{
	//设置变量
	m_bEditUserId = false;

	//变量定义
	CComboBox * pComBoxGameID=(CComboBox *)GetDlgItem(IDC_GAME_ID);

	//获取选择
	INT nCurrentSel=pComBoxGameID->GetCurSel();
	if (nCurrentSel!=LB_ERR) SetCurrentUserID((DWORD)pComBoxGameID->GetItemData(nCurrentSel));

	return;
}

//输入改变
VOID CDlgLogon::OnEditChangeAccounts()
{
	//获取帐户
	CString strAccounts;
	m_cmAccounts.GetWindowText(strAccounts);

	//寻找记录
	INT nSelIndex = ComboBoxFindString(&m_cmAccounts,strAccounts);
	if( nSelIndex != CB_ERR )
	{
		//设置变量
		m_bEditAccount = false;

		m_cmAccounts.SetCurSel(nSelIndex);

		UpdateUserPassWord((DWORD)m_cmAccounts.GetItemData(nSelIndex));

		m_cmAccounts.SetEditSel(-1,-1);
	}
	else if( !m_bEditAccount )
	{
		//设置变量
		m_bEditAccount = true;

		//更新控件
		UpdateUserPassWord(0);
	}

	return;
}

//设置帐号
VOID CDlgLogon::SetAccounts(LPCTSTR lpszAccounts)
{
	m_cmAccounts.SetWindowText(lpszAccounts);
	m_edPassword.SetWindowText(TEXT(""));
	((CButton *)GetDlgItem(IDC_REM_PASSWORD))->SetCheck(BST_UNCHECKED);
	OnBnClickedRemPassword();
	return;
}

//输入改变
VOID CDlgLogon::OnEditChangeUserID()
{
	//获取帐户
	CString strUserId;
	m_cmGameID.GetWindowText(strUserId);

	//寻找记录
	INT nSelIndex = ComboBoxFindString(&m_cmGameID,strUserId);
	if( nSelIndex != CB_ERR )
	{
		//设置变量
		m_bEditUserId = false;

		m_cmGameID.SetCurSel(nSelIndex);

		UpdateUserPassWord((DWORD)m_cmGameID.GetItemData(nSelIndex));

		m_cmGameID.SetEditSel(-1,-1);
	}
	else if( !m_bEditAccount )
	{
		//设置变量
		m_bEditUserId = true;

		//更新控件
		UpdateUserPassWord(0);
	}

	return;
}

//删除用户
VOID CDlgLogon::OnBnClickedDelete() 
{
	//获取窗口
	UINT uComboBoxID=(m_LogonMode==LogonMode_GameID)?IDC_GAME_ID:IDC_ACCOUNTS;
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(uComboBoxID);

	//数据判断
	INT nItem=pComboBox->GetCurSel();
	if (nItem==LB_ERR)
	{
		pComboBox->SetFocus();
		pComboBox->SetWindowText(TEXT(""));
		return;
	}

	//删除键项
	CRegKeyAide RegUserID;
	DWORD dwUserID=(DWORD)pComboBox->GetItemData(nItem);
	if (RegUserID.OpenRegKey(REG_USER_INFO,false)==true)
	{
		TCHAR szUserID[16]=TEXT("");
		// add by humroly 2018/8/3
		// replace func
		_sntprintf_s(szUserID, CountArray(szUserID), TEXT("%ld"), dwUserID);
		RegUserID.RecurseDeleteKey(szUserID);
	}

	//删除选择
	CComboBox * pOtherComboBox=NULL;
	UINT uComboBoxIDs[]={IDC_ACCOUNTS,IDC_GAME_ID};
	for (INT i=0;i<CountArray(uComboBoxIDs);i++)
	{
		pOtherComboBox=(CComboBox *)GetDlgItem(uComboBoxIDs[i]);
		for (INT j=0;j<pOtherComboBox->GetCount();j++)
		{
			if ((DWORD)pOtherComboBox->GetItemData(j)==dwUserID)
			{
				pOtherComboBox->DeleteString(j);
				break;
			}
		}
	}

	//更新信息
	if (pComboBox->GetCount()==0) UpdateUserPassWord(0L);
	else SetCurrentUserID((DWORD)pComboBox->GetItemData(0));

	//设置焦点
	pComboBox->SetFocus();

	return;
}

//删除帐号
VOID CDlgLogon::DeleteAccounts(LPCTSTR lpszAccounts)
{
	if( lpszAccounts == NULL )
	{
		//删除当前帐号
		OnBnClickedDelete();
		return;
	}

	//获取窗口
	UINT uComboBoxID=(m_LogonMode==LogonMode_GameID)?IDC_GAME_ID:IDC_ACCOUNTS;
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(uComboBoxID);

	//查询帐号
	INT nItem = pComboBox->FindStringExact(-1,lpszAccounts);
	if( nItem == LB_ERR ) return;

	//删除键项
	CRegKeyAide RegUserID;
	DWORD dwUserID=(DWORD)pComboBox->GetItemData(nItem);
	if (RegUserID.OpenRegKey(REG_USER_INFO,false)==true)
	{
		TCHAR szUserID[16]=TEXT("");
		// add by humroly 2018/8/3
		// replace func
		_sntprintf_s(szUserID, CountArray(szUserID), TEXT("%ld"), dwUserID);
		RegUserID.RecurseDeleteKey(szUserID);
	}

	//删除选择
	CComboBox * pOtherComboBox=NULL;
	UINT uComboBoxIDs[]={IDC_ACCOUNTS,IDC_GAME_ID};
	for (INT i=0;i<CountArray(uComboBoxIDs);i++)
	{
		pOtherComboBox=(CComboBox *)GetDlgItem(uComboBoxIDs[i]);
		for (INT j=0;j<pOtherComboBox->GetCount();j++)
		{
			if ((DWORD)pOtherComboBox->GetItemData(j)==dwUserID)
			{
				pOtherComboBox->DeleteString(j);
				break;
			}
		}
	}

	//更新信息
	if (pComboBox->GetCount()==0) UpdateUserPassWord(0L);
	else SetCurrentUserID((DWORD)pComboBox->GetItemData(0));

	return;
}

//下拉列表
VOID CDlgLogon::OnCBDrawDownAccounts()
{
	m_cmAccounts.SetEditSel(0,0);
}

//下拉列表
VOID CDlgLogon::OnCBDrawDownGameID()
{
	m_cmGameID.SetEditSel(0,0);
}

//////////////////////////////////////////////////////////////////////////
