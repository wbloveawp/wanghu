#include "Stdafx.h"
#include "Resource.h"
#include "InitParameter.h"
#include "LogonServerDlg.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define WM_PROCESS_CMD_LINE			(WM_USER+100)						//处理命令

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CLogonServerDlg, CDialog)
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_LOAD_OPTION, OnBnClickedLoadOption)
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedConcludeService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)

	//自定消息
	ON_MESSAGE(WM_PROCESS_CMD_LINE, OnMessageProcessCmdLine)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CLogonServerDlg::CLogonServerDlg() : CDialog(IDD_DLG_LOGON_SERVER)
{
	m_pServiceUnits = NULL;
}

//析构函数
CLogonServerDlg::~CLogonServerDlg()
{
	if (m_pServiceUnits != NULL) SafeDelete(m_pServiceUnits);
}

//控件绑定
VOID CLogonServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_RichEditTrace);
}

//初始化函数
BOOL CLogonServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//创建对象
	if (m_pServiceUnits == NULL)
	{
		m_pServiceUnits = new CServiceUnits();
	}

	//设置标题
	SetWindowStatusText(TEXT("停止"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//命令处理
	LPCTSTR pszCmdLine = AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0] != 0) PostMessage(WM_PROCESS_CMD_LINE, 0, (LPARAM)pszCmdLine);	

	return TRUE;
}

//确定消息
VOID CLogonServerDlg::OnOK()
{
	return;
}

//取消函数
VOID CLogonServerDlg::OnCancel()
{
	//关闭询问
	if (m_pServiceUnits->GetServiceStatus()!=SERVICE_STATUS_STOP)
	{
		LPCTSTR pszQuestion=TEXT("登录服务器正在运行中，您确实要关闭服务器吗？");
		if (AfxMessageBox(pszQuestion,MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)!=IDYES) return;
	}

	//停止服务
	m_pServiceUnits->ConcludeService();

	__super::OnCancel();
}

//消息解释
BOOL CLogonServerDlg::PreTranslateMessage(MSG * pMsg)
{
	//按键过滤
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//服务状态
VOID CLogonServerDlg::OnServiceUnitsStatus(WORD ServiceStatus)
{
	//状态设置
	switch (ServiceStatus)
	{
	case SERVICE_STATUS_STOP:	//停止状态
		{
			//设置标题
			SetWindowStatusText(TEXT("停止"));

			//设置按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_LOAD_OPTION)->EnableWindow(TRUE);

			//提示信息
			CTraceService::TraceString(TEXT("服务停止成功"),TraceLevel_Normal);

			break;
		}
	case SERVICE_STATUS_CONFIG:	//配置状态
		{
			//设置标题
			SetWindowStatusText(TEXT("初始化"));

			//设置按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_LOAD_OPTION)->EnableWindow(FALSE);

			//提示信息
			CTraceService::TraceString(TEXT("正在初始化组件..."),TraceLevel_Normal);

			break;
		}
	case SERVICE_STATUS_SERVICE:	//服务状态
		{
			//设置标题
			SetWindowStatusText(TEXT("运行"));

			//设置按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_LOAD_OPTION)->EnableWindow(FALSE);

			//提示信息
			CTraceService::TraceString(TEXT("服务启动成功"),TraceLevel_Normal);

			break;
		}
	}

	return;
}

//启动服务
VOID CLogonServerDlg::OnBnClickedStartService()
{
	//设置接口
	if (m_pServiceUnits->SetServiceUnitsSink(this)==false)
	{
		CTraceService::TraceString(TEXT("绑定服务组件状态接口失败"),TraceLevel_Exception);
		return;
	}

	//启动服务
	try
	{
		m_pServiceUnits->StartService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//停止服务
VOID CLogonServerDlg::OnBnClickedConcludeService()
{

	//停止服务
	try
	{
		m_pServiceUnits->ConcludeService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//加载配置
VOID CLogonServerDlg::OnBnClickedLoadOption()
{
	//变量定义
	CDlgServiceOption DlgServiceOption;
	DlgServiceOption.SetDataFileName(TEXT("Logon.Data"));

	if (DlgServiceOption.DoModal() == IDOK)
	{
		//输出日志
		CTraceService::TraceString(TEXT("服务配置加载成功！"), TraceLevel_Normal);

		//设置配置
		m_pServiceUnits->SetServiceOption(*DlgServiceOption.GetServiceOption());

		//设置按钮
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

		return;
	}
}

//设置标题
VOID CLogonServerDlg::SetWindowStatusText(LPCTSTR pszServiceStatus)
{
	//获取对象
	const CInitParameter * pInitParameter = m_pServiceUnits->GetInitParameter();
	ASSERT(pInitParameter!=NULL);

	//登陆配置
	const tagServiceOption* pServiceOption = m_pServiceUnits->GeServiceOption();
	ASSERT(pServiceOption != NULL);
	
	//变量定义
	TCHAR szWindowText[128]=TEXT("");
	_sntprintf_s(szWindowText,CountArray(szWindowText),TEXT("%s 登录服务器[%d] -- [ %s ]"),pInitParameter->m_szServerName, pServiceOption->wServicePort,pszServiceStatus);

	//设置标题
	SetWindowText(szWindowText);

	return;
}

//关闭询问
BOOL CLogonServerDlg::OnQueryEndSession()
{
	//提示消息
	if (m_pServiceUnits->GetServiceStatus()!=SERVICE_STATUS_STOP)
	{
		CTraceService::TraceString(TEXT("服务正在运行中，系统要求注销回话请求失败"),TraceLevel_Warning);
		return FALSE;
	}

	return TRUE;
}


//命令处理
LRESULT CLogonServerDlg::OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR lpszComLine = (LPCTSTR)(lParam);

	//参数处理
	if (lpszComLine[0] != 0)
	{
		//变量定义
		WORD wServiceID = 0;

		//提取变量
		UINT nStringLength = 0;
		CString strItemString;
		LPCTSTR lpszBeginString = lpszComLine;

		//加载服务标识
		LPCTSTR pszServiceID = TEXT("/ServiceID:");
		UINT nServiceIDLength = lstrlen(pszServiceID);

		//提取参数
		while (true)
		{
			//参数分析
			LPCTSTR lpszEndString = _tcschr(lpszBeginString, TEXT(' '));
			nStringLength = (lpszEndString == NULL) ? lstrlen(lpszBeginString) : (UINT)(lpszEndString - lpszBeginString);

			//传输窗口
			if ((nStringLength >= nServiceIDLength) && (memcmp(lpszBeginString, pszServiceID, nServiceIDLength * sizeof(TCHAR)) == 0))
			{
				//提取子项
				CopyMemory(strItemString.GetBufferSetLength(nStringLength - nServiceIDLength), lpszBeginString + nServiceIDLength,
					(nStringLength - nServiceIDLength) * sizeof(TCHAR));
				strItemString.ReleaseBuffer();

				//提取参数
				wServiceID = (WORD)(_tstoi(strItemString));

				break;
			}

			//设置变量
			if (lpszEndString == NULL) break;
			lpszBeginString = (lpszEndString + 1);
		}

		if (wServiceID != 0)
		{
			//构造对象
			CDlgServiceOption DlgServiceOption;
			DlgServiceOption.SetDataFileName(TEXT("Logon.Data"));

			//获取配置
			tagServiceOption * pServiceOption = DlgServiceOption.GetServiceOption(wServiceID);
			if (pServiceOption != NULL)
			{
				//设置接口
				if (m_pServiceUnits->SetServiceUnitsSink(this) == false)
				{
					CTraceService::TraceString(TEXT("绑定服务组件状态接口失败"), TraceLevel_Exception);
					return 0;
				}

				//输出日志
				CTraceService::TraceString(TEXT("服务配置加载成功！"), TraceLevel_Normal);				

				//设置配置
				m_pServiceUnits->SetServiceOption(*pServiceOption);

				//设置按钮
				GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
				GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
			}
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
