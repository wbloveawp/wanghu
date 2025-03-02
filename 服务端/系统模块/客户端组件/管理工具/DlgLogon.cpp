#include "Stdafx.h"
#include "Resource.h"
#include "DlgLogon.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CTabCtrlLogon, CTabCtrl)
	ON_WM_PAINT()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CDlgLogon, CSkinDialog)

	//�ؼ���Ϣ
	ON_EN_CHANGE(IDC_PASSWORD, OnEnChangePassword)
	ON_CBN_SELCHANGE(IDC_GAME_ID, OnSelchangeUserID)
	ON_CBN_SELCHANGE(IDC_ACCOUNTS, OnSelchangeAccounts)
	ON_CBN_EDITCHANGE(IDC_ACCOUNTS, OnEditChangeAccounts)
	ON_CBN_EDITCHANGE(IDC_GAME_ID, OnEditChangeUserID)
	ON_CBN_DROPDOWN(IDC_ACCOUNTS, OnCBDrawDownAccounts)
	ON_CBN_DROPDOWN(IDC_GAME_ID, OnCBDrawDownGameID)

	//��ť��Ϣ
	ON_BN_CLICKED(IDC_DELETE_RECORD, OnBnClickedDelete)
	ON_BN_CLICKED(IDC_PROXY_TEST, OnBnClickedProxyTest)
	ON_BN_CLICKED(IDC_PROXY_OPTION, OnBnClickedProxyOption)
	ON_BN_CLICKED(IDC_REM_PASSWORD, OnBnClickedRemPassword)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CTabCtrlLogon::CTabCtrlLogon()
{
}

//��������
CTabCtrlLogon::~CTabCtrlLogon()
{
}

//�ػ���Ϣ
void CTabCtrlLogon::OnPaint()
{
	///��������
	CPaintDC dc(this);

	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��ɫ����
	COLORREF crTXSelect=RGB(0,0,0);
	COLORREF crTXNormal=RGB(0,0,0);
	COLORREF crBackGround=CSkinDialog::m_SkinAttribute.m_crBackGround;
	COLORREF crControlBorder=RGB(30,153,255);

	//�滭����
	CRect rcClip;
	dc.GetClipBox(&rcClip);
	dc.FillSolidRect(&rcClip,crBackGround);

	//���� DC
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());

	//�滭�߿�
	rcClient.top+=18;
	dc.Draw3dRect(&rcClient,crControlBorder,crControlBorder);

	//�滭��ť
	CPen LinePen(PS_SOLID,1,crControlBorder);
	CPen * pOldPen=dc.SelectObject(&LinePen);

	for (INT i=0;i<GetItemCount();i++)
	{
		//��������
		TCITEM ItemInfo;
		ZeroMemory(&ItemInfo,sizeof(ItemInfo));

		//��������
		TCHAR szBuffer[100];
		ItemInfo.mask=TCIF_TEXT|TCIF_IMAGE;
		ItemInfo.pszText=szBuffer;
		ItemInfo.cchTextMax=sizeof(szBuffer);

		//��ȡ��Ϣ
		CRect rcItem;
		GetItem(i,&ItemInfo);
		GetItemRect(i,&rcItem);

		//��䱳��
		dc.FillSolidRect(&rcItem,crBackGround);

		//�滭�߿�
		dc.MoveTo(rcItem.left,rcItem.bottom-1);
		dc.LineTo(rcItem.left,rcItem.top+2);
		dc.LineTo(rcItem.left+2,rcItem.top);
		dc.LineTo(rcItem.right-2,rcItem.top);
		dc.LineTo(rcItem.right,rcItem.top+2);
		dc.LineTo(rcItem.right,rcItem.bottom-1);

		//�滭�߿�
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

	//������Դ
	dc.SelectObject(pOldPen);
	LinePen.DeleteObject();

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDlgLogon::CDlgLogon() : CSkinDialog(IDD_DLG_LOGON)
{
	//��¼��Ϣ
	m_dwGameID=0;
	m_szAccounts[0]=0;
	m_szPassword[0]=0;
	m_szLogonServer[0]=0;
	m_szAccountTitle[0]=0;

	//״̬����
	m_bEditPass=false;
	m_bEditAccount = false;
	m_bEditUserId = false;
	m_bProxyMode=false;
	m_cbVerifyMode=0;

	//ģʽ����
	m_LogonMode=LogonMode_Accounts;
	m_NetworkKind=NetworkKind_Default;

	//λ����Ϣ
	m_nFullWidth=0;
	m_nFullHeight=0;

	return;
}

//��������
CDlgLogon::~CDlgLogon()
{
}

//�ؼ���
VOID CDlgLogon::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);

	//�󶨰�ť
	DDX_Control(pDX, IDOK, m_btLogon);
	DDX_Control(pDX, IDC_PROXY_TEST, m_btProxyTest);
	DDX_Control(pDX, IDC_PROXY_OPTION, m_btProxyOption);
	DDX_Control(pDX, IDC_PROXY_TYPE, m_cmProxyServerType);

	//�󶨱༭
	DDX_Control(pDX, IDC_GAME_ID, m_cmGameID);
	DDX_Control(pDX, IDC_ACCOUNTS, m_cmAccounts);
	DDX_Control(pDX, IDC_PASSWORD, m_edPassword);
	DDX_Control(pDX, IDC_PROXY_PORT, m_edProxyPort);
	DDX_Control(pDX, IDC_LOGON_SERVER, m_cmLogonServer);
	DDX_Control(pDX, IDC_PROXY_SERVER, m_edProxyServer);
	DDX_Control(pDX, IDC_PROXY_USER, m_edProxyUserName);
	DDX_Control(pDX, IDC_PROXY_PASS, m_edProxyPassword);
}

//��ʼ������
BOOL CDlgLogon::OnInitDialog()
{
	__super::OnInitDialog();

	//���ñ���
	SetWindowText(TEXT("��Ϸ��¼"));
	
	//���ñ���
	if (m_ImageTitle.IsNull()==true)
	{
		m_ImageTitle.LoadImage(AfxGetInstanceHandle(),TEXT("TITLE_DLG_LOGON"));
	}

	//��������
	TCHAR szBrowseUrl[MAX_PATH]=TEXT("");
	CGlobalStation * pGlobalStation=CGlobalStation::GetInstance();
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

	//����Ŀ¼
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//����·��
	TCHAR szAssistantPath[MAX_PATH]=TEXT("");

	// add by humroly 2018/8/3
	// replace func
	_sntprintf_s(szAssistantPath,CountArray(szAssistantPath),TEXT("%s\\Collocate\\Collocate.INI"),szDirectory);

	//��������
	((CEdit *)(GetDlgItem(IDC_PROXY_PORT)))->LimitText(5);
	((CEdit *)(GetDlgItem(IDC_PROXY_USER)))->LimitText(31);
	((CEdit *)(GetDlgItem(IDC_PROXY_PASS)))->LimitText(31);
	((CEdit *)(GetDlgItem(IDC_PROXY_SERVER)))->LimitText(31);
	((CComboBox *)(GetDlgItem(IDC_GAME_ID)))->LimitText(11);
	((CEdit *)(GetDlgItem(IDC_PASSWORD)))->LimitText(LEN_PASSWORD-1);
	((CComboBox *)(GetDlgItem(IDC_ACCOUNTS)))->LimitText(LEN_ACCOUNTS-1);

	//��¼ģʽ
	SwitchLogonMode(pParameterGlobal->m_LogonMode);

	//��ס����
	bool bRemPassword=(pParameterGlobal->m_AcountsRule==AcountsRule_AccountsAndPass);
	if (bRemPassword==true) ((CButton *)GetDlgItem(IDC_REM_PASSWORD))->SetCheck(BST_CHECKED);

	//������Ϣ
	LoadAccountsInfo();
	LoadLogonServerInfo();
	LoadProxyServerInfo();

	m_LogonMode=LogonMode_Accounts;
	
	//��¼ģʽ
	SwitchLogonMode(m_LogonMode);

	//���λ��
	CRect rcFrame;
	GetWindowRect(&m_rcNormalFrame);
	GetDlgItem(IDC_BORDER)->GetWindowRect(&rcFrame);

	//��¼λ��
	m_nFullWidth=m_rcNormalFrame.Width();
	m_nFullHeight=m_rcNormalFrame.Height();

	//����λ��
	m_rcNormalFrame.bottom=rcFrame.top;
	SetWindowPos(NULL,m_rcNormalFrame.left,m_rcNormalFrame.top,m_rcNormalFrame.Width(),m_rcNormalFrame.Height(),SWP_NOZORDER);

	//�����ַ
	TCHAR szBillUrl[256]=TEXT("");
	pGlobalStation->GetRedirectionLink(JUMP_ID_AD_LOGON,szBillUrl,CountArray(szBillUrl));

	//���д���
	CenterWindow();

	return FALSE;
}

//�����
BOOL CDlgLogon::OnCommand(WPARAM wParam, LPARAM lParam)
{
	DWORD dwCommandID = HIWORD(wParam);
	UINT nCommandID = LOWORD(wParam);

	//ȡ������
	if (nCommandID == IDM_GIVE_UP_CONNECT)
	{
		//ִ�е�¼
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

//��Ϣ����
BOOL CDlgLogon::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//ȷ������
VOID CDlgLogon::OnOK()
{
	//��ȡ��Ϣ
	if (GetInformation()==false) return;

	//���ش���
	ShowWindow(SW_HIDE);

	//ִ�е�¼
	CSessionLogon * pMissionLogon=CSessionLogon::GetInstance();
	if (pMissionLogon!=NULL) pMissionLogon->PerformLogonMission(MT_LOGON);

	return;
}

//ȡ����Ϣ
VOID CDlgLogon::OnCancel()
{
	//�رմ���
	DestroyWindow();
	AfxGetMainWnd()->PostMessage(WM_CLOSE,0,0);

	return;
}

//��������
WORD CDlgLogon::ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize)
{
	//��������
	TCHAR szPassword[LEN_MD5];
	CMD5Encrypt::EncryptData(m_szPassword,szPassword);

	//��������
	switch (m_LogonMode)
	{
	case LogonMode_GameID:			//I D ��¼
		{
			//��������
			CGlobalStation * pGlobalStation=CGlobalStation::GetInstance();
			CMD_GP_LogonByGameID * pLogonByGameID=(CMD_GP_LogonByGameID *)cbBuffer;

			//��������
			pLogonByGameID->dwGameID=m_dwGameID;
			//pLogonByGameID->dwStationID=pGlobalStation->GetStationID();
			pLogonByGameID->dwPlazaVersion = 0; // g_GlobalUnits.GetPlazaVersion();
			lstrcpynW(pLogonByGameID->szPassword,CT2CW(szPassword),CountArray(pLogonByGameID->szPassword));
			
			return sizeof(CMD_GP_LogonByGameID);
		}
	case LogonMode_Accounts:		//�ʺŵ�¼
		{
			//��������
			CGlobalStation * pGlobalStation=CGlobalStation::GetInstance();
			CMD_GP_LogonByAccounts * pLogonByAccounts=(CMD_GP_LogonByAccounts *)cbBuffer;

			//��������
			//pLogonByAccounts->dwStationID=pGlobalStation->GetStationID();
			pLogonByAccounts->dwPlazaVersion = 0; // g_GlobalUnits.GetPlazaVersion();
			lstrcpynW(pLogonByAccounts->szPassword,CT2CW(szPassword),CountArray(pLogonByAccounts->szPassword));
			lstrcpynW(pLogonByAccounts->szAccounts,CT2CW(m_szAccounts),CountArray(pLogonByAccounts->szAccounts));

			return sizeof(CMD_GP_LogonByAccounts);
		}
	case LogonMode_BBS_Accounts:	//��̳�ʺ�
		{
			//��������
			//CGlobalStation *pGlobalStation=CGlobalStation::GetInstance();
			//CMD_GP_LogonByBBSAccounts * pLogonByBBSAccounts=(CMD_GP_LogonByBBSAccounts *)cbBuffer;

			////��������
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

//��ȡ�ʺ�
VOID CDlgLogon::LoadAccountsInfo()
{
	//�����Ϣ
	for (int i = m_cmAccounts.GetCount() - 1; i >= 0; i--)
	{
		m_cmAccounts.DeleteString(i);
	}

	//��ȡ��Ϣ
	CRegKeyAide RegUserInfo;
	if (RegUserInfo.OpenRegKey(REG_USER_INFO,false)==true)
	{
		DWORD dwIndex=0L,dwUserID=0L;
		TCHAR szKeyItemName[32],szReadBuffer[32];
		do
		{
			//��ȡ����
			if (RegUserInfo.EnumItemString(dwIndex++,szKeyItemName,sizeof(szKeyItemName))==false) break;

			//��ȡ����
			dwUserID=_ttol(szKeyItemName);
			if (dwUserID==0L) continue;

			//���Ӽ�
			CRegKeyAide RegUserItem;
			if (RegUserItem.Attach(RegUserInfo.OpenItemKey(szKeyItemName))==NULL) continue;

			//վ�� I D
			szReadBuffer[0]=0;
			RegUserItem.GetString(TEXT("StationID"),szReadBuffer,CountArray(szReadBuffer));
			DWORD dwUserStationID=(DWORD)_ttoi(szReadBuffer);

            //վ���ж�
			if(dwUserStationID!=0 && !g_GlobalUnits.m_GlobalStation.IsCompStation(dwUserStationID)) continue;

			//��Ϸ I D
			szReadBuffer[0]=0;
			RegUserItem.GetString(TEXT("GameID"),szReadBuffer,CountArray(szReadBuffer));
			if ((szReadBuffer[0]!=0)&&(ComboBoxFindString(&m_cmGameID,szReadBuffer)==LB_ERR))
			{
				m_cmGameID.SetItemData(m_cmGameID.AddString(szReadBuffer),dwUserID);
			}

			//�û��ʺ�
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

		//�ϴ��û�
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

	//����ѡ��
	if ((m_cmAccounts.GetCurSel()==LB_ERR)&&(m_cmAccounts.GetCount()>0))
	{
		SetCurrentUserID((DWORD)m_cmAccounts.GetItemData(0));
	}

	return;
}

//��ַ��Ϣ
VOID CDlgLogon::LoadLogonServerInfo()
{
	//��ȡ��Ŀ
	CGlobalServer GlobalServer;
	WORD wItemCount=GlobalServer.GetItemCount();

	//�ϴε�¼
	TCHAR szLastServer[LEN_SERVER]=TEXT("");
	GlobalServer.GetLastServerName(szLastServer);

	//������Ϣ
	if (szLastServer[0]!=0) m_cmLogonServer.SetCurSel(m_cmLogonServer.AddString(szLastServer));

	//��ȡ��Ϣ
	for (WORD i=0;i<wItemCount;i++)
	{
		//��ȡ��Ϣ
		TCHAR szServerItem[LEN_SERVER]=TEXT("");
		GlobalServer.GetServerItemName(i,szServerItem);

		//������Ϣ
		if ((szServerItem[0]!=0)&&(ComboBoxFindString(&m_cmLogonServer,szServerItem)==LB_ERR))
		{
			m_cmLogonServer.AddString(szServerItem);
		}
	}

	//Ĭ�����
	if (m_cmLogonServer.GetCount()==0)
	{
		lstrcpyn(szLastServer,szLogonServer,CountArray(szLastServer));
		m_cmLogonServer.SetCurSel(m_cmLogonServer.AddString(szLastServer));
	}

	//����ѡ��
	if (m_cmLogonServer.GetCurSel()==LB_ERR) m_cmLogonServer.SetCurSel(0);

	return;
}

//������Ϣ
VOID CDlgLogon::LoadProxyServerInfo()
{
	//��������
	LPCTSTR szProxyType[]={TEXT("��ʹ�ô���"),TEXT("HTTP ����"),TEXT("SOCKS 4"),TEXT("SOCKS 5")};
	enProxyServerType ProxyServerType[]={ProxyType_None,ProxyType_Http,ProxyType_Socks4,ProxyType_Socks5};

	//��ȡ��Ϣ
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();
	enProxyServerType CurrentProxyType=pParameterGlobal->m_ProxyServerType;
	tagProxyServerInfo & ProxyServerInfo=pParameterGlobal->m_ProxyServerInfo;

	//��������
	for (INT i=0;i<CountArray(szProxyType);i++)
	{
		ASSERT(i<CountArray(ProxyServerType));
		INT nItem=m_cmProxyServerType.AddString(szProxyType[i]);
		m_cmProxyServerType.SetItemData(nItem,ProxyServerType[i]);
		if (CurrentProxyType==ProxyServerType[i]) m_cmProxyServerType.SetCurSel(nItem);
	}

	//������Ϣ
	SetDlgItemText(IDC_PROXY_USER,ProxyServerInfo.szUserName);
	SetDlgItemText(IDC_PROXY_PASS,ProxyServerInfo.szPassword);
	SetDlgItemText(IDC_PROXY_SERVER,ProxyServerInfo.szProxyServer);
	if (ProxyServerInfo.wProxyPort!=0) SetDlgItemInt(IDC_PROXY_PORT,ProxyServerInfo.wProxyPort);

	//����ѡ��
	if (m_cmProxyServerType.GetCurSel()==CB_ERR) m_cmProxyServerType.SetCurSel(0);

	return;
}

//��������
INT CDlgLogon::ComboBoxFindString(CComboBox * pComboBox, LPCTSTR pszFindString)
{
	//��������
	CString strBuffer;
	INT nStringLen=lstrlen(pszFindString);
	INT nItemHandle=LB_ERR,nItemFind=LB_ERR;

	do 
	{
		//��������
		nItemFind=pComboBox->FindString((nItemHandle==LB_ERR)?0:(nItemHandle+1),pszFindString);
		if ((nItemFind==LB_ERR)||(nItemHandle==nItemFind)||(nItemFind<nItemHandle)) return LB_ERR;

		//���ݶԱ�
		nItemHandle=nItemFind;
		pComboBox->GetLBText(nItemHandle,strBuffer);
		if( !lstrcmp(pszFindString,strBuffer) ) return nItemHandle;
	} while (true);

	return LB_ERR;
}

//��ȡ��Ϣ
bool CDlgLogon::GetInformation()
{
	//��¼��ַ
	CString strLogonServer;
	GetDlgItemText(IDC_LOGON_SERVER,strLogonServer);
	strLogonServer.TrimLeft();strLogonServer.TrimRight();

	//��ַ�ж�
	if (strLogonServer.IsEmpty()==true)
	{
		ShowInformation(TEXT("��ѡ�񡰵�¼������������������������������ָ�ϡ���ѯ������"),MB_ICONERROR,0);
		m_cmLogonServer.SetFocus();
		return false;
	}
	else lstrcpyn(m_szLogonServer,strLogonServer,CountArray(m_szLogonServer));

	//��¼�ʺ�
	switch (m_LogonMode)
	{
	case LogonMode_GameID:		//ID ��¼
		{
			//��ȡ�ʺ�
			CString strUserID;
			GetDlgItemText(IDC_GAME_ID,strUserID);
			strUserID.TrimLeft();strUserID.TrimRight();

			//�ʺ��ж�
			m_dwGameID=_ttol(strUserID);
			if (m_dwGameID==0L)
			{
				ShowInformation(TEXT("��Ϸ ID ����Ϊ�գ����������룡"),MB_ICONERROR,0);
				m_cmGameID.SetFocus();
				return false;
			}

			break;
		}
	case LogonMode_Accounts:	//�ʺŵ�¼
	case LogonMode_BBS_Accounts://��̳��¼
		{
			//��ȡ�ʺ�
			CString strAccount;
			GetDlgItemText(IDC_ACCOUNTS,strAccount);
			strAccount.TrimLeft();strAccount.TrimRight();

			//�ʺ��ж�
			if (strAccount.IsEmpty())
			{
				ShowInformation(TEXT("�û��ʺŲ���Ϊ�գ����������룡"),MB_ICONERROR,0);
				m_cmAccounts.SetFocus();
				return false;
			}

			//���ñ���
			lstrcpyn(m_szAccounts,strAccount,CountArray(m_szAccounts));

			break;
		}
	}

	//�û�����
	if (m_bEditPass==true)
	{
		m_bEditPass=false;
		m_edPassword.GetWindowText(m_szPassword,CountArray(m_szPassword));
	}
	if (m_szPassword[0]==0)
	{
		ShowInformation(TEXT("��¼���벻��Ϊ�գ������������¼���룡"),MB_ICONERROR,30);
		m_edPassword.SetFocus();
		return false;
	}

	//��������
	CComboBox * pComProxyType=(CComboBox *)GetDlgItem(IDC_PROXY_TYPE);
	enProxyServerType ProxyServerType=(enProxyServerType)pComProxyType->GetItemData(pComProxyType->GetCurSel());

	//������Ϣ
	tagProxyServerInfo ProxyServerInfo;
	ZeroMemory(&ProxyServerInfo,sizeof(ProxyServerInfo));
	ProxyServerInfo.wProxyPort=GetDlgItemInt(IDC_PROXY_PORT);
	GetDlgItemText(IDC_PROXY_USER,ProxyServerInfo.szUserName,CountArray(ProxyServerInfo.szUserName));
	GetDlgItemText(IDC_PROXY_PASS,ProxyServerInfo.szPassword,CountArray(ProxyServerInfo.szPassword));
	GetDlgItemText(IDC_PROXY_SERVER,ProxyServerInfo.szProxyServer,CountArray(ProxyServerInfo.szProxyServer));

	//Ч�����
	if (ProxyServerType!=ProxyType_None)
	{
		//�����ַ
		if (ProxyServerInfo.szProxyServer[0]==0)
		{
			ShowInformation(TEXT("�����������ַ����Ϊ�գ����������룡"),MB_ICONERROR,30);
			if (m_bProxyMode==false) SwitchProxyMode(true);
			m_edProxyServer.SetFocus();
			return false;
		}

		//����˿�
		if (ProxyServerInfo.wProxyPort==0)
		{
			ShowInformation(TEXT("���������������˿ںţ����磺1080��"),MB_ICONERROR,30);
			if (m_bProxyMode==false) SwitchProxyMode(true);
			m_edProxyPort.SetFocus();
			return false;
		}
	}

	//��������
	ASSERT(CParameterGlobal::GetInstance()!=NULL);
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

	//��������
	if (pParameterGlobal!=NULL)
	{
		pParameterGlobal->m_LogonMode=m_LogonMode;
		pParameterGlobal->m_ProxyServerType=ProxyServerType;
		pParameterGlobal->m_ProxyServerInfo=ProxyServerInfo;
	}

	return true;
}

//����ѡ��
VOID CDlgLogon::SetCurrentUserID(DWORD dwUserID)
{
	//����ѡ��
	UINT uComboBoxIDs[]={IDC_ACCOUNTS,IDC_GAME_ID};
	for (INT i=0;i<CountArray(uComboBoxIDs);i++)
	{
		//��ȡ�ؼ�
		CComboBox * pComboBox=(CComboBox *)GetDlgItem(uComboBoxIDs[i]);

		//��������
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

	//��������
	UpdateUserPassWord(dwUserID);

	return;
}

//��������
VOID CDlgLogon::UpdateUserPassWord(DWORD dwUserID)
{
	//��ȡ����
	if (dwUserID!=0L)
	{
		//�������
		TCHAR szKeyName[64]=TEXT("");
		// add by humroly 2018/8/3
		// replace func
		_sntprintf_s(szKeyName, CountArray(szKeyName), TEXT("%s\\%ld"), REG_USER_INFO, dwUserID);

		//��ע���
		CRegKeyAide RegPassWord;
		if (RegPassWord.OpenRegKey(szKeyName,false)==true)
		{
			//��ȡ����
			TCHAR szPassBuffer[256]=TEXT("");
			if(m_LogonMode==LogonMode_BBS_Accounts)
				RegPassWord.GetString(TEXT("BBSPassword"),szPassBuffer,CountArray(szPassBuffer));
			else
				RegPassWord.GetString(TEXT("UserPassword"),szPassBuffer,CountArray(szPassBuffer));

			//��������
			CXOREncrypt::CrevasseData(szPassBuffer,m_szPassword,CountArray(m_szPassword));
			m_edPassword.SetWindowText((m_szPassword[0]==0)?TEXT(""):TEXT("**********"));

			//���ñ���
			m_bEditPass=false;

			((CButton *)GetDlgItem(IDC_REM_PASSWORD))->SetCheck((m_szPassword[0]==0?BST_UNCHECKED:BST_CHECKED));
			OnBnClickedRemPassword();

			return;
		}
	}

	//��������
	m_edPassword.SetWindowText(TEXT(""));

	//��¼�����־
	((CButton *)GetDlgItem(IDC_REM_PASSWORD))->SetCheck(BST_UNCHECKED);
	OnBnClickedRemPassword();

	//���ñ���
	m_szPassword[0]=0;
	m_bEditPass=false;
	
	return;
}

//����ģʽ
VOID CDlgLogon::SwitchProxyMode(bool bProxyMode)
{
	//���ñ���
	m_bProxyMode=bProxyMode;

	//�ؼ�����
	GetDlgItem(IDC_PROXY_TYPE)->EnableWindow(m_bProxyMode);
	GetDlgItem(IDC_PROXY_PORT)->EnableWindow(m_bProxyMode);
	GetDlgItem(IDC_PROXY_USER)->EnableWindow(m_bProxyMode);
	GetDlgItem(IDC_PROXY_PASS)->EnableWindow(m_bProxyMode);
	GetDlgItem(IDC_PROXY_TEST)->EnableWindow(m_bProxyMode);
	GetDlgItem(IDC_PROXY_SERVER)->EnableWindow(m_bProxyMode);

	//�ƶ�λ��
	INT nHeight=m_bProxyMode?m_nFullHeight:m_rcNormalFrame.Height();
	SetWindowPos(NULL,0,0,m_nFullWidth,nHeight,SWP_NOMOVE|SWP_NOZORDER|SWP_NOCOPYBITS);

	return;
}

//�л�ģʽ
VOID CDlgLogon::SwitchLogonMode(enLogonMode LogonMode)
{
	//���ñ���
	m_LogonMode=LogonMode;

	LoadAccountsInfo();
	//���ý���
	switch (m_LogonMode)
	{
	case LogonMode_Accounts:	//�ʺŵ�¼
		{
			//���ÿؼ�
			m_cmGameID.ShowWindow(SW_HIDE);
			m_cmAccounts.ShowWindow(SW_SHOW);
			SetDlgItemText(IDC_TYPE_NAME,TEXT("��Ϸ�ʺţ�"));

			//���ý���
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

//�������
VOID CDlgLogon::OnBnClickedProxyTest()
{
	//��������
	CComboBox * pComProxyType=(CComboBox *)GetDlgItem(IDC_PROXY_TYPE);
	enProxyServerType ProxyServerType=(enProxyServerType)pComProxyType->GetItemData(pComProxyType->GetCurSel());

	//������Ϣ
	tagProxyServerInfo ProxyServerInfo;
	ZeroMemory(&ProxyServerInfo,sizeof(ProxyServerInfo));
	ProxyServerInfo.wProxyPort=GetDlgItemInt(IDC_PROXY_PORT);
	GetDlgItemText(IDC_PROXY_USER,ProxyServerInfo.szUserName,CountArray(ProxyServerInfo.szUserName));
	GetDlgItemText(IDC_PROXY_PASS,ProxyServerInfo.szPassword,CountArray(ProxyServerInfo.szPassword));
	GetDlgItemText(IDC_PROXY_SERVER,ProxyServerInfo.szProxyServer,CountArray(ProxyServerInfo.szProxyServer));

	//Ч�����
	if (ProxyServerType!=ProxyType_None)
	{
		//�����ַ
		if (ProxyServerInfo.szProxyServer[0]==0)
		{
			ShowInformation(TEXT("�����������ַ����Ϊ�գ����������룡"),MB_ICONERROR,30);
			if (m_bProxyMode==false) SwitchProxyMode(true);
			m_edProxyServer.SetFocus();
			return;
		}

		//����˿�
		if (ProxyServerInfo.wProxyPort==0)
		{
			ShowInformation(TEXT("���������������˿ںţ����磺1080��"),MB_ICONERROR,30);
			if (m_bProxyMode==false) SwitchProxyMode(true);
			m_edProxyPort.SetFocus();
			return;
		}
	}
	else 
	{
		//������ʾ
		ShowInformation(TEXT("�������ô��������������Ϣ��"),MB_ICONERROR,30);
		pComProxyType->SetFocus();
		return;
	}

	//�������
	CNetworkManagerHelper NetworkManagerModule;
	if (NetworkManagerModule.CreateInstance()==false)
	{
		ShowInformation(TEXT("�����������������ʧ�ܣ�����ʧ�ܣ�"),MB_ICONERROR,30);
		return;
	}

	//�������
	switch (NetworkManagerModule->ProxyServerTesting(ProxyServerType,ProxyServerInfo))
	{
	case CONNECT_SUCCESS:				//���ӳɹ�
		{
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("�������������������"),MB_ICONINFORMATION);
			return;
		}
	case CONNECT_PROXY_USER_INVALID:	//�û�����
		{
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("����������û��������������"),MB_ICONERROR);
			return;
		}
	default:							//Ĭ�ϴ���
		{
			CInformation Information(this);
			Information.ShowMessageBox(TEXT("��������������ڻ�������ʧ�ܣ�"),MB_ICONERROR);
			return;
		}
	}

	return;
}

//��������
VOID CDlgLogon::OnBnClickedProxyOption()
{
	//����ģʽ
	SwitchProxyMode(!m_bProxyMode);

	return;
}

//��ס����
VOID CDlgLogon::OnBnClickedRemPassword()
{
	//��������
	ASSERT(CParameterGlobal::GetInstance()!=NULL);
	CParameterGlobal * pParameterGlobal=CParameterGlobal::GetInstance();

	//��ȡ����
	bool bRemPassword=(((CButton *)GetDlgItem(IDC_REM_PASSWORD))->GetCheck()==BST_CHECKED);

	//д������
	if (bRemPassword==true)
	{
		pParameterGlobal->m_AcountsRule=AcountsRule_AccountsAndPass;
	}

	//д������
	if ((bRemPassword==false)&&(pParameterGlobal->m_AcountsRule==AcountsRule_AccountsAndPass))
	{
		pParameterGlobal->m_AcountsRule=AcountsRule_Accounts;
	}

	return;
}

//����ı�
VOID CDlgLogon::OnEnChangePassword()
{
	m_bEditPass=true;
}

//ѡ��ı�
VOID CDlgLogon::OnSelchangeAccounts()
{
	//���ñ���
	m_bEditAccount = false;

	//��������
	CComboBox * pComBoxAccounts=(CComboBox *)GetDlgItem(IDC_ACCOUNTS);

	//��ȡѡ��
	INT nCurrentSel=pComBoxAccounts->GetCurSel();
	if (nCurrentSel!=LB_ERR) SetCurrentUserID((DWORD)pComBoxAccounts->GetItemData(nCurrentSel));

	return;
}

//ѡ��ı�
VOID CDlgLogon::OnSelchangeUserID()
{
	//���ñ���
	m_bEditUserId = false;

	//��������
	CComboBox * pComBoxGameID=(CComboBox *)GetDlgItem(IDC_GAME_ID);

	//��ȡѡ��
	INT nCurrentSel=pComBoxGameID->GetCurSel();
	if (nCurrentSel!=LB_ERR) SetCurrentUserID((DWORD)pComBoxGameID->GetItemData(nCurrentSel));

	return;
}

//����ı�
VOID CDlgLogon::OnEditChangeAccounts()
{
	//��ȡ�ʻ�
	CString strAccounts;
	m_cmAccounts.GetWindowText(strAccounts);

	//Ѱ�Ҽ�¼
	INT nSelIndex = ComboBoxFindString(&m_cmAccounts,strAccounts);
	if( nSelIndex != CB_ERR )
	{
		//���ñ���
		m_bEditAccount = false;

		m_cmAccounts.SetCurSel(nSelIndex);

		UpdateUserPassWord((DWORD)m_cmAccounts.GetItemData(nSelIndex));

		m_cmAccounts.SetEditSel(-1,-1);
	}
	else if( !m_bEditAccount )
	{
		//���ñ���
		m_bEditAccount = true;

		//���¿ؼ�
		UpdateUserPassWord(0);
	}

	return;
}

//�����ʺ�
VOID CDlgLogon::SetAccounts(LPCTSTR lpszAccounts)
{
	m_cmAccounts.SetWindowText(lpszAccounts);
	m_edPassword.SetWindowText(TEXT(""));
	((CButton *)GetDlgItem(IDC_REM_PASSWORD))->SetCheck(BST_UNCHECKED);
	OnBnClickedRemPassword();
	return;
}

//����ı�
VOID CDlgLogon::OnEditChangeUserID()
{
	//��ȡ�ʻ�
	CString strUserId;
	m_cmGameID.GetWindowText(strUserId);

	//Ѱ�Ҽ�¼
	INT nSelIndex = ComboBoxFindString(&m_cmGameID,strUserId);
	if( nSelIndex != CB_ERR )
	{
		//���ñ���
		m_bEditUserId = false;

		m_cmGameID.SetCurSel(nSelIndex);

		UpdateUserPassWord((DWORD)m_cmGameID.GetItemData(nSelIndex));

		m_cmGameID.SetEditSel(-1,-1);
	}
	else if( !m_bEditAccount )
	{
		//���ñ���
		m_bEditUserId = true;

		//���¿ؼ�
		UpdateUserPassWord(0);
	}

	return;
}

//ɾ���û�
VOID CDlgLogon::OnBnClickedDelete() 
{
	//��ȡ����
	UINT uComboBoxID=(m_LogonMode==LogonMode_GameID)?IDC_GAME_ID:IDC_ACCOUNTS;
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(uComboBoxID);

	//�����ж�
	INT nItem=pComboBox->GetCurSel();
	if (nItem==LB_ERR)
	{
		pComboBox->SetFocus();
		pComboBox->SetWindowText(TEXT(""));
		return;
	}

	//ɾ������
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

	//ɾ��ѡ��
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

	//������Ϣ
	if (pComboBox->GetCount()==0) UpdateUserPassWord(0L);
	else SetCurrentUserID((DWORD)pComboBox->GetItemData(0));

	//���ý���
	pComboBox->SetFocus();

	return;
}

//ɾ���ʺ�
VOID CDlgLogon::DeleteAccounts(LPCTSTR lpszAccounts)
{
	if( lpszAccounts == NULL )
	{
		//ɾ����ǰ�ʺ�
		OnBnClickedDelete();
		return;
	}

	//��ȡ����
	UINT uComboBoxID=(m_LogonMode==LogonMode_GameID)?IDC_GAME_ID:IDC_ACCOUNTS;
	CComboBox * pComboBox=(CComboBox *)GetDlgItem(uComboBoxID);

	//��ѯ�ʺ�
	INT nItem = pComboBox->FindStringExact(-1,lpszAccounts);
	if( nItem == LB_ERR ) return;

	//ɾ������
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

	//ɾ��ѡ��
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

	//������Ϣ
	if (pComboBox->GetCount()==0) UpdateUserPassWord(0L);
	else SetCurrentUserID((DWORD)pComboBox->GetItemData(0));

	return;
}

//�����б�
VOID CDlgLogon::OnCBDrawDownAccounts()
{
	m_cmAccounts.SetEditSel(0,0);
}

//�����б�
VOID CDlgLogon::OnCBDrawDownGameID()
{
	m_cmGameID.SetEditSel(0,0);
}

//////////////////////////////////////////////////////////////////////////
