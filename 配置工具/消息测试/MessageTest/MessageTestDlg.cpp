
// MessageTestDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MessageTest.h"
#include "MessageTestDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct CustomInfo
{
	TCHAR	szMachineID[LEN_MACHINE_ID];
	TCHAR	szMachineName[LEN_MACHINE_ID];
	TCHAR	szClientIP[LEN_ADDRESS];
	TCHAR	szPassword[LEN_PASSWORD];
	TCHAR	szNewPassword[LEN_PASSWORD];
	TCHAR	szMobile[LEN_MOBILE_PHONE];
	TCHAR	szAuthCode[LEN_AUTHCODE];
	DWORD	dwInsureOperateGameID;
	SCORE	lInsureOperateScore;
	TCHAR	szInsurePassword[LEN_PASSWORD];
	
};
struct RuntimeInfo
{
	DWORD	dwUserID;
	TCHAR	szLogonToken[LEN_LOGON_TOKEN];		//登录令牌
};
//
CustomInfo	gCustomInfo;
RuntimeInfo gRuntimeInfo;
// CMessageTestDlg 对话框

CMessageTestDlg::CMessageTestDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MESSAGETEST_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	ZeroMemory(m_bServiceToken, sizeof(m_bServiceToken));
	m_wCurModule = INVALID_WORD;

	//初始本机数据
	_swprintf(gCustomInfo.szMachineID, L"local");
	_swprintf(gCustomInfo.szMachineName, L"local");
	_swprintf(gCustomInfo.szClientIP, L"127.0.0.1");
	_swprintf(gCustomInfo.szPassword, L"123456");
	_swprintf(gCustomInfo.szNewPassword,L"");
	_swprintf(gCustomInfo.szMobile,L"123456789");
	_swprintf(gCustomInfo.szAuthCode, L"");
	gCustomInfo.dwInsureOperateGameID = 0;
	gCustomInfo.lInsureOperateScore = 0;
	_swprintf(gCustomInfo.szInsurePassword, L"123456");
}

void CMessageTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_LOG, m_log);
}

BEGIN_MESSAGE_MAP(CMessageTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CMessageTestDlg::OnBnClickedButtonConnect)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, &CMessageTestDlg::OnBnClickedButtonClose)
	ON_BN_CLICKED(IDC_BUTTON_TEST_LOGON, &CMessageTestDlg::OnBnClickedButtonTestLogon)
	ON_BN_CLICKED(IDC_BUTTON_TOKEN, &CMessageTestDlg::OnBnClickedButtonToken)
	ON_BN_CLICKED(IDC_BUTTON_REAL_AUTH, &CMessageTestDlg::OnBnClickedButtonRealAuth)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_LOG, &CMessageTestDlg::OnBnClickedButtonClearLog)
	ON_BN_CLICKED(IDC_BUTTON_BINDPHONE, &CMessageTestDlg::OnBnClickedButtonBindphone)
	ON_BN_CLICKED(IDC_BUTTON_DISBINDPHONE, &CMessageTestDlg::OnBnClickedButtonDisbindphone)
	ON_BN_CLICKED(IDC_BUTTON_REFRESH_DATA, &CMessageTestDlg::OnBnClickedButtonRefreshData)
	ON_BN_CLICKED(IDC_BUTTON_INSURE_QUERRY, &CMessageTestDlg::OnBnClickedButtonInsureQuerry)
	ON_BN_CLICKED(IDC_BUTTON_INSURE_SAVE, &CMessageTestDlg::OnBnClickedButtonInsureSave)
	ON_BN_CLICKED(IDC_BUTTON_INSURE_TAKE, &CMessageTestDlg::OnBnClickedButtonInsureTake)
	ON_BN_CLICKED(IDC_BUTTON_INSURE_TRANSFER, &CMessageTestDlg::OnBnClickedButtonInsureTransfer)
	ON_BN_CLICKED(IDC_BUTTON_INSURE_ENABLE, &CMessageTestDlg::OnBnClickedButtonInsureEnable)
	ON_BN_CLICKED(IDC_BUTTON_MESSAGE, &CMessageTestDlg::OnBnClickedButtonMessage)
END_MESSAGE_MAP()


// CMessageTestDlg 消息处理程序

BOOL CMessageTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(false);
	RefreshCustomInfo(true);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMessageTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialogEx::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMessageTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMessageTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//释放对象
VOID CMessageTestDlg::Release() {}
//接口查询
VOID* CMessageTestDlg::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPSocketSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPSocketSink, Guid, dwQueryVer);
	return 0;
};

//连接事件
bool CMessageTestDlg::OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode)
{
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(true);
	Log(L"[CONNECT]链接成功!");
	return true;
};
//关闭事件
bool CMessageTestDlg::OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason)
{
	Log(L"[SHUT] XX");
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(false);
	return true;
};
//读取事件
bool CMessageTestDlg::OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID* pData, WORD wDataSize)
{
	CString str;

	WORD wServiceModule = HICMMD(Command.wMainCmdID);
	WORD wMain = LOCMMD(Command.wMainCmdID);

	if (MDM_GG_GATE_SERVICE == wMain)
	{
		if (SUB_GG_S_LINK_SUCCESS == Command.wSubCmdID)
		{
			if (m_wCurModule < SERVICE_COUNT)
			{
				m_bServiceToken[m_wCurModule] = true;
				Log(L"注册令牌成功");
			}
		}
		else if (SUB_GG_S_LINK_FAILURE == Command.wSubCmdID)
		{

			CMD_GG_S_LinkFailure* pReply = (CMD_GG_S_LinkFailure*)pData;
			if (m_wCurModule < SERVICE_COUNT)
				m_bServiceToken[m_wCurModule] = false;
			Log(pReply->szErrorDescribe);
		}
		else
		{
			str.Format(L"[GATE] Module:%d M:%d S:%d Size:%d", wServiceModule, wMain, Command.wSubCmdID, wDataSize);
			Log(str);
		}
		return true;
	}
	else if (MDM_CM_SYSTEM == wMain)
	{
		if (SUB_CM_SYSTEM_MESSAGE == Command.wSubCmdID)
		{
			CMD_CM_SystemMessage* pReply = (CMD_CM_SystemMessage*)pData;
			Log(pReply->szString);
		}
		else if (SUB_CM_OPERATE_SUCCESS == Command.wSubCmdID)
		{
			Log(L"操作成功");
		}
		else if (SUB_CM_OPERATE_FAILURE == Command.wSubCmdID)
		{
			Log(L"操作失败");
		}
		else
		{
			str.Format(L"[SYSTEM] Module:%d M:%d S:%d Size:%d", wServiceModule, wMain, Command.wSubCmdID, wDataSize);
			Log(str);
		}
		return true;
	}

	//
	str.Format(L"[READ] Module:%d M:%d S:%d Size:%d", wServiceModule, wMain, Command.wSubCmdID, wDataSize);
	Log(str);
	try
	{
		switch (wServiceModule)
		{
		case SERVICE_MODULE_LOGON: MessageModuleLogon(wMain, Command.wSubCmdID, pData, wDataSize); break;
		case SERVICE_MODULE_GAME:break;
		default:
			break;
		}
	}
	catch (...)
	{
		Log(L"except");
	}
	return true;
};

bool CMessageTestDlg::Connect(TCHAR szAddr[LEN_SERVER], DWORD dwPort)
{
	//创建组件
	if (m_TCPSocketModule.GetInterface() == NULL)
	{
		if (m_TCPSocketModule.CreateInstance() == false) return false;
		if (m_TCPSocketModule->SetTCPSocketSink(QUERY_ME_INTERFACE(IUnknownEx)) == false) return false;
	}

	//地址转换
	CT2CA CurrentServer(szAddr);
	DWORD dwAddr = inet_addr(CurrentServer);
	//
	if (m_TCPSocketModule->Connect(dwAddr, dwPort) != CONNECT_SUCCESS) return false;

	return true;
};
bool CMessageTestDlg::Close()
{
	if (m_TCPSocketModule.GetInterface() != NULL) m_TCPSocketModule->CloseSocket();
	return true;
};


void CMessageTestDlg::OnBnClickedButtonConnect()
{
	GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(false);
	Connect(L"172.16.3.219", 5910);
	//Connect(L"127.0.0.1", 6404);
}
void CMessageTestDlg::Log(LPCTSTR pString)
{
	SYSTEMTIME tm;
	GetLocalTime(&tm);
	m_strlog.AppendFormat(L"[%02d:%02d:%02d]",tm.wHour,tm.wMinute,tm.wSecond);
	m_strlog += pString;
	m_strlog += L"\r\n";
	m_log.SetWindowTextW(m_strlog);
	m_log.LineScroll(m_log.GetLineCount());
};

void CMessageTestDlg::OnBnClickedButtonClose()
{
	// TODO: 在此添加控件通知处理程序代码
	Close();
}
void CMessageTestDlg::SendData(WORD wModule, WORD wMainCmd, WORD wSubCmd, LPVOID pData, WORD wDataSize)
{
	m_wCurModule = wModule;
	if (!m_bServiceToken[wModule])
	{
		Log(L"先注册令牌");
		return;
	}
	if (m_TCPSocketModule.GetInterface() != NULL)
		m_TCPSocketModule->SendData(MAKECMMD(wMainCmd, wModule), wSubCmd, pData, wDataSize);
};
void CMessageTestDlg::OnBnClickedButtonTestLogon()
{
	CMD_MB_LogonVisitor req;
	ZeroMemory(&req, sizeof(req));
	req.dwDeviceKind = DEVICE_MASK_MB;
	CMD5Encrypt::EncryptData(gCustomInfo.szMachineID, req.szMachineID);
	_snwprintf(req.szMachineName, CountArray(req.szMachineName), gCustomInfo.szMachineName);
	req.wModuleID = SERVICE_MODULE_LOGON;

	SendData(SERVICE_MODULE_LOGON, MDM_MB_LOGON, SUB_MB_LOGON_VISITOR, &req, sizeof(req));
}
void CMessageTestDlg::RegisteToken(WORD wModule)
{
	CMD_GG_C_LinkService req;
	ZeroMemory(&req, sizeof(req));
	req.wServiceModule = wModule;
	req.wServiceID = INVALID_WORD;
	if (m_bServiceToken[SERVICE_MODULE_LOGON])
	{
		req.dwUserID = gRuntimeInfo.dwUserID;
		CopyMemory(req.szLogonToken, gRuntimeInfo.szLogonToken,sizeof(req.szLogonToken));
	}

	if (m_TCPSocketModule.GetInterface() != NULL)
		m_TCPSocketModule->SendData(MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_C_LINK_SERVICE, &req, sizeof(req));
};

void CMessageTestDlg::GetItemText(int nID, TCHAR szValue[])
{
	CString str;
	GetDlgItemText(nID, str);
	_swprintf(szValue, str);
}
void CMessageTestDlg::RefreshCustomInfo(bool bUpdateUI)
{
	if (bUpdateUI)
	{
		SetDlgItemText(IDC_EDIT_MACHINE_NAME,CString(gCustomInfo.szMachineName));
		SetDlgItemText(IDC_EDIT_MACHINE_ID, gCustomInfo.szMachineID);
		SetDlgItemText(IDC_EDIT_CLIENT_IP, gCustomInfo.szClientIP);
		SetDlgItemText(IDC_EDIT_PASSWORD, gCustomInfo.szPassword);
		SetDlgItemText(IDC_EDIT_NEWPASSWORD, gCustomInfo.szNewPassword);
		SetDlgItemText(IDC_EDIT_AUTHCODE, gCustomInfo.szAuthCode);
		SetDlgItemText(IDC_EDIT_MOBILE, gCustomInfo.szMobile);

		CString str;
		str.Format(L"%d",gCustomInfo.dwInsureOperateGameID);
		SetDlgItemText(IDC_EDIT_INSURE_GAMEID, str);
		str.Format(L"%I64d", gCustomInfo.lInsureOperateScore);
		SetDlgItemText(IDC_EDIT_INSURE_SCORE, str);
		SetDlgItemText(IDC_EDIT_INSURE_PASSWORD, gCustomInfo.szInsurePassword);
	}
	else
	{
		GetItemText(IDC_EDIT_MACHINE_NAME, gCustomInfo.szMachineName);
		GetItemText(IDC_EDIT_MACHINE_ID, gCustomInfo.szMachineID);
		GetItemText(IDC_EDIT_CLIENT_IP, gCustomInfo.szClientIP);
		GetItemText(IDC_EDIT_PASSWORD, gCustomInfo.szPassword);
		GetItemText(IDC_EDIT_NEWPASSWORD, gCustomInfo.szNewPassword);
		GetItemText(IDC_EDIT_AUTHCODE, gCustomInfo.szAuthCode);
		GetItemText(IDC_EDIT_MOBILE, gCustomInfo.szMobile);
		//
		CString str;
		GetDlgItemText(IDC_EDIT_INSURE_GAMEID, str);
		gCustomInfo.dwInsureOperateGameID = _wtoll(str);
		GetDlgItemText(IDC_EDIT_INSURE_SCORE, str);
		gCustomInfo.lInsureOperateScore = _wtoll(str);
		GetItemText(IDC_EDIT_INSURE_PASSWORD, gCustomInfo.szInsurePassword);
	}
};

void CMessageTestDlg::OnBnClickedButtonToken()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_wCurModule == INVALID_WORD)
		m_wCurModule = SERVICE_MODULE_LOGON;
	RegisteToken(m_wCurModule);
}
void CMessageTestDlg::MessageModuleLogon(WORD wMainCmd, WORD wSubCmd, LPVOID pData, WORD wDataSize)
{
	if (MDM_MB_LOGON == wMainCmd)
	{
		if (SUB_MB_LOGON_FAILURE == wSubCmd)
		{
			CMD_MB_LogonFailure* pReply = (CMD_MB_LogonFailure*)pData;
			Log(pReply->szDescribeString);
		}
		else if (SUB_MB_LOGON_SUCCESS == wSubCmd)
		{
			CMD_MB_LogonSuccess* pReply = (CMD_MB_LogonSuccess*)pData;
			CString str;
			str.Format(L"登录成功! UserID:%d GameID:%d Score:%I64d Insure:%I64d",pReply->dwUserID,pReply->dwGameID,pReply->lUserGold,pReply->lUserInsure);
			Log(str);
			//
			gRuntimeInfo.dwUserID = pReply->dwUserID;
			//解子包
			BYTE* pPacket = (BYTE*)pData;
			int nOffset = sizeof(CMD_MB_LogonSuccess);
			while (nOffset + sizeof(tagDataDescribe) < wDataSize)
			{
				tagDataDescribe* pHead = (tagDataDescribe*)(pPacket + nOffset);
				switch (pHead->wDataDescribe)
				{
				case DTP_MB_LOGON_TOKEN:
				{
					CopyMemory(gRuntimeInfo.szLogonToken, (pHead + 1), sizeof(TCHAR) * LEN_LOGON_TOKEN);
					Log(gRuntimeInfo.szLogonToken);
				}break;
				default:
					break;
				}
				//
				nOffset += sizeof(tagDataDescribe) + pHead->wDataSize;
			}
		}
	}
	else if (MDM_MB_USER_SERVICE == wMainCmd)
	{
		/*
		if (SUB_MB_INSURE_QUERRY_RESULT == wSubCmd)
		{
			CMD_MB_InsureQuerryResult* pReply = (CMD_MB_InsureQuerryResult*)pData;
			CString str;
			str.Format(L"银行查询,Enable:%d InsureFee:%d,TransferFee:%d,ReserveAcount:%I64d DealMin:%I64d DealMax:%I64d DailyMax:%I64d InsureScore:%I64d Score:%I64d",
				pReply->cbEnable,pReply->SaveAndTake.dwFeeRate,pReply->SaveAndTake.dwFeeRate,pReply->SaveAndTake.lReserveAmount,
				pReply->SaveAndTake.lDealMin,pReply->SaveAndTake.lDealMax,pReply->SaveAndTake.lDailyMax,pReply->lInsureScore, pReply->lScore);
			Log(str);
		}
		else if (SUB_MB_INSURE_SUCCESS == wSubCmd)
		{
			CMD_MB_InsureSuccess* pReply = (CMD_MB_InsureSuccess*)pData;
			CString str;
			str.Format(L"银行操作成功 Insure:%I64d Score:%I64d",pReply->lInsureScore, pReply->lScore);
			Log(str);
		}
		else if (SUB_MB_INSURE_FAILURE == wSubCmd)
		{
			CMD_MB_InsureFailure* pReply = (CMD_MB_InsureFailure*)pData;
			CString str;
			str.Format(L"银行操作失败 Code:%d Desc:%s", pReply->dwResultCode, pReply->szDescribeString);
			Log(str);
		}
		*/
	}
};


void CMessageTestDlg::OnBnClickedButtonRealAuth()
{
	// TODO: 在此添加控件通知处理程序代码
	CMD_MB_RealAuth req;
	ZeroMemory(&req, sizeof(CMD_MB_RealAuth));
	//req.dwUserID = gRuntimeInfo.dwUserID;
	CMD5Encrypt::EncryptData(gCustomInfo.szPassword, req.szPassword);
	_swprintf(req.szCompellation, L"老虎");
	_swprintf(req.szPassPortID, L"123456879012345678");

	SendData(SERVICE_MODULE_LOGON,MDM_MB_USER_SERVICE, SUB_MB_REAL_AUTH, &req, sizeof(req));
}


void CMessageTestDlg::OnBnClickedButtonClearLog()
{
	// TODO: 在此添加控件通知处理程序代码
	m_strlog = L"";
	m_log.SetWindowTextW(m_strlog);
}


void CMessageTestDlg::OnBnClickedButtonBindphone()
{
	// TODO: 在此添加控件通知处理程序代码
	CMD_MB_BindMobilePhone req;
	ZeroMemory(&req, sizeof(req));
	CMD5Encrypt::EncryptData(gCustomInfo.szPassword, req.szPassword);
	CMD5Encrypt::EncryptData(gCustomInfo.szNewPassword, req.szNewPassword);
	CMD5Encrypt::EncryptData(gCustomInfo.szMachineID, req.szMachineID);
	req.dwUserID = gRuntimeInfo.dwUserID;
	_swprintf(req.szMobilePhone, gCustomInfo.szMobile);
	_swprintf(req.szAuthCode, gCustomInfo.szAuthCode);
	req.dwUnbindUserID = 0;

	SendData(SERVICE_MODULE_LOGON, MDM_MB_USER_SERVICE, SUB_MB_BIND_MOBILEPHONE, &req, sizeof(req));
}


void CMessageTestDlg::OnBnClickedButtonDisbindphone()
{
	// TODO: 在此添加控件通知处理程序代码
	CMD_MB_BindMobilePhone req;
	ZeroMemory(&req, sizeof(req));
	CMD5Encrypt::EncryptData(gCustomInfo.szPassword, req.szPassword);
	CMD5Encrypt::EncryptData(gCustomInfo.szNewPassword, req.szNewPassword);
	CMD5Encrypt::EncryptData(gCustomInfo.szMachineID, req.szMachineID);
	req.dwUserID = gRuntimeInfo.dwUserID;
	_swprintf(req.szMobilePhone, gCustomInfo.szMobile);
	_swprintf(req.szAuthCode, gCustomInfo.szAuthCode);
	req.dwUnbindUserID = gRuntimeInfo.dwUserID;

	SendData(SERVICE_MODULE_LOGON, MDM_MB_USER_SERVICE, SUB_MB_BIND_MOBILEPHONE, &req, sizeof(req));
}


void CMessageTestDlg::OnBnClickedButtonRefreshData()
{
	// TODO: 在此添加控件通知处理程序代码
	RefreshCustomInfo();
}


void CMessageTestDlg::OnBnClickedButtonInsureQuerry()
{
	// TODO: 在此添加控件通知处理程序代码
	SendData(SERVICE_MODULE_LOGON, MDM_MB_USER_SERVICE, SUB_MB_INSURE_QUERY,0,0);
}


void CMessageTestDlg::OnBnClickedButtonInsureSave()
{
	// TODO: 在此添加控件通知处理程序代码
	CMD_MB_InsureSave req;
	ZeroMemory(&req, sizeof(req));

	req.lSaveScore = gCustomInfo.lInsureOperateScore;
	CMD5Encrypt::EncryptData(gCustomInfo.szMachineID, req.szMachineID);

	SendData(SERVICE_MODULE_LOGON, MDM_MB_USER_SERVICE, SUB_MB_INSURE_SAVE, &req, sizeof(req));
}


void CMessageTestDlg::OnBnClickedButtonInsureTake()
{
	// TODO: 在此添加控件通知处理程序代码
	CMD_MB_InsureTake req;
	ZeroMemory(&req, sizeof(req));

	req.lTakeScore = gCustomInfo.lInsureOperateScore;
	CMD5Encrypt::EncryptData(gCustomInfo.szPassword, req.szPassword);
	CMD5Encrypt::EncryptData(gCustomInfo.szMachineID, req.szMachineID);

	SendData(SERVICE_MODULE_LOGON, MDM_MB_USER_SERVICE, SUB_MB_INSURE_TAKE, &req, sizeof(req));
}


void CMessageTestDlg::OnBnClickedButtonInsureTransfer()
{
	// TODO: 在此添加控件通知处理程序代码
	CMD_MB_InsureTransfer req;
	ZeroMemory(&req, sizeof(req));

	req.lTransferScore = gCustomInfo.lInsureOperateScore;
	req.dwGameID = gCustomInfo.dwInsureOperateGameID;
	CMD5Encrypt::EncryptData(gCustomInfo.szPassword, req.szPassword);
	CMD5Encrypt::EncryptData(gCustomInfo.szMachineID, req.szMachineID);

	SendData(SERVICE_MODULE_LOGON, MDM_MB_USER_SERVICE, SUB_MB_INSURE_TRANSFER, &req, sizeof(req));
}


void CMessageTestDlg::OnBnClickedButtonInsureEnable()
{
#if 0
	// TODO: 在此添加控件通知处理程序代码
	CMD_MB_InsureEnable req;
	ZeroMemory(&req, sizeof(req));
	CMD5Encrypt::EncryptData(gCustomInfo.szInsurePassword, req.szPassword);

	SendData(SERVICE_MODULE_LOGON, MDM_MB_USER_SERVICE, SUB_MB_INSURE_ENABLE, &req, sizeof(req));
#else

	CMD_MB_InsureResetPassword  req;
	CMD5Encrypt::EncryptData(L"123456", req.szSrcPassword);
	CMD5Encrypt::EncryptData(L"123456", req.szDstPassword);

	SendData(SERVICE_MODULE_LOGON, MDM_MB_USER_SERVICE, SUB_MB_INSURE_RESET_PASSWORD, &req, sizeof(req));
#endif
}


void CMessageTestDlg::OnBnClickedButtonMessage()
{
	SendData(SERVICE_MODULE_MESSAGE, MDM_AN_MESSAGE_SERVICE, SUB_AN_C_ENTER_MESSAGE,0,0);
}
