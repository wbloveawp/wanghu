#include "Stdafx.h"
#include "Resource.h"
#include "MonitorServerDlg.h"

//////////////////////////////////////////////////////////////////////////
//消息定义

#define WM_PROCESS_CMD_LINE			(WM_USER+100)						//处理命令

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMonitorServerDlg, CDialog)
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedConcludeService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
	ON_BN_CLICKED(IDC_OPTION_SERVICE, OnBnClickedOptionService)

	//自定消息
	ON_MESSAGE(WM_PROCESS_CMD_LINE, OnMessageProcessCmdLine)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CMonitorServerDlg::CMonitorServerDlg() : CDialog(IDD_DLG_MONITOR_SERVER)
{
}

//析构函数
CMonitorServerDlg::~CMonitorServerDlg()
{
}

//控件绑定
VOID CMonitorServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_RichEditTrace);
}

//初始化函数
BOOL CMonitorServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("监控服务器 -- [ 停止 ]"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//初始化参数
	CInitParameter * pInitParameter=m_ServiceUnits.GetInitParameter();

	//网络类型
	m_ModuleDBParameter.SetPlatformDBParameter(pInitParameter->m_PlatformDBParameter);	

	//命令处理
	LPCTSTR pszCmdLine = AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0] != 0) PostMessage(WM_PROCESS_CMD_LINE, 0, (LPARAM)pszCmdLine);

	return TRUE;
}

//确定消息
VOID CMonitorServerDlg::OnOK()
{
	return;
}

//取消函数
VOID CMonitorServerDlg::OnCancel()
{
	if (m_ServiceUnits.GetServiceStatus()!=SERVICE_STATUS_STOP)
	{
		LPCTSTR pszQuestion=TEXT("监控服务器正在运行中，请先停止服务！");
		AfxMessageBox(pszQuestion, MB_OK);
		return;		
	}

	//停止服务
	m_ServiceUnits.ConcludeService();

	__super::OnCancel();
}

//消息解释
BOOL CMonitorServerDlg::PreTranslateMessage(MSG * pMsg)
{
	//按键过滤
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//服务状态
VOID CMonitorServerDlg::OnServiceUnitsStatus(WORD ServiceStatus)
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
			GetDlgItem(IDC_OPTION_SERVICE)->EnableWindow(TRUE);

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
			GetDlgItem(IDC_OPTION_SERVICE)->EnableWindow(FALSE);

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
			GetDlgItem(IDC_OPTION_SERVICE)->EnableWindow(FALSE);

			//提示信息
			CTraceService::TraceString(TEXT("服务启动成功"),TraceLevel_Normal);

			break;
		}
	}

	return;
}

//设置标题
VOID CMonitorServerDlg::SetWindowStatusText(LPCTSTR pszServiceStatus)
{
	//获取对象
	const CInitParameter * pInitParameter = m_ServiceUnits.GetInitParameter();
	ASSERT(pInitParameter!=NULL);

	//约战配置
	const tagServiceOption * pServiceOption = m_ServiceUnits.GetServiceOption();
	ASSERT(pServiceOption !=NULL);
	
	//变量定义
	TCHAR szWindowText[128]=TEXT("");
	_sntprintf_s(szWindowText,CountArray(szWindowText),TEXT("监控服务器[%d] -- [ %s ]"), pServiceOption->wServicePort,pszServiceStatus);

	//设置标题
	SetWindowText(szWindowText);

	return;
}

//启动服务
VOID CMonitorServerDlg::OnBnClickedStartService()
{
	//设置接口
	if (m_ServiceUnits.SetServiceUnitsSink(this)==false)
	{
		CTraceService::TraceString(TEXT("绑定服务组件状态接口失败"),TraceLevel_Exception);
		return;
	}

	//启动服务
	try
	{
		m_ServiceUnits.StartService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//停止服务
VOID CMonitorServerDlg::OnBnClickedConcludeService()
{
	//停止服务
	try
	{
		m_ServiceUnits.ConcludeService(true);
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//配置服务
VOID CMonitorServerDlg::OnBnClickedOptionService()
{	
	//设置资源
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//变量定义
	CDlgServiceOption DlgServiceOption;
	DlgServiceOption.SetDataFileName(TEXT("Monitor.Data"));

	if (DlgServiceOption.DoModal() == IDOK)
	{
		//输出日志
		CTraceService::TraceString(TEXT("服务配置加载成功！"), TraceLevel_Normal);

		//设置配置
		m_ServiceUnits.SetServiceOption(*DlgServiceOption.GetServiceOption());

		//设置按钮
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

		return;
	}
}

//关闭询问
BOOL CMonitorServerDlg::OnQueryEndSession()
{
	//提示消息
	if (m_ServiceUnits.GetServiceStatus()!=SERVICE_STATUS_STOP)
	{
		CTraceService::TraceString(TEXT("服务正在运行中，系统要求注销回话请求失败"),TraceLevel_Warning);
		return FALSE;
	}

	return TRUE;
}


//命令处理
LRESULT CMonitorServerDlg::OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam)
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
			DlgServiceOption.SetDataFileName(TEXT("Monitor.Data"));

			//获取配置
			tagServiceOption * pServiceOption = DlgServiceOption.GetServiceOption(wServiceID);
			if (pServiceOption != NULL)
			{
				//输出日志
				CTraceService::TraceString(TEXT("服务配置加载成功！"), TraceLevel_Normal);

				//设置配置
				m_ServiceUnits.SetServiceOption(*pServiceOption);

				//设置按钮
				GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
				GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
			}
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
