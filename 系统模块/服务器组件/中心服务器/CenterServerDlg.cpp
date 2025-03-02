#include "Stdafx.h"
#include "Resource.h"
#include "CenterServerDlg.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCenterServerDlg, CDialog)
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedConcludeService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
	ON_BN_CLICKED(IDC_DEBUG_SERVICE, OnBnClickedDebugService)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CCenterServerDlg::CCenterServerDlg() : CDialog(IDD_DLG_CENTER_SERVER)
{
}

//析构函数
CCenterServerDlg::~CCenterServerDlg()
{
}

//控件绑定
VOID CCenterServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_RichEditTrace);
}

//初始化函数
BOOL CCenterServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("中心服务器 -- [ 停止 ]"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	return TRUE;
}

//确定消息
VOID CCenterServerDlg::OnOK()
{
	return;
}

//取消函数
VOID CCenterServerDlg::OnCancel()
{
	if (m_ServiceUnits.GetServiceStatus()!=SERVICE_STATUS_STOP)
	{
		LPCTSTR pszQuestion=TEXT("中心服务器正在运行中，您确实要关闭服务器吗？");
		if (AfxMessageBox(pszQuestion,MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)!=IDYES) return;
	}

	//停止服务
	m_ServiceUnits.ConcludeService();

	__super::OnCancel();
}

//消息解释
BOOL CCenterServerDlg::PreTranslateMessage(MSG * pMsg)
{
	//按键过滤
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//服务状态
VOID CCenterServerDlg::OnServiceUnitsStatus(WORD ServiceStatus)
{
	//状态设置
	switch (ServiceStatus)
	{
	case SERVICE_STATUS_STOP:	//停止状态
		{
			//设置标题
			SetWindowText(TEXT("中心服务器 -- [ 停止 ]"));

			//设置按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

			//提示信息
			CTraceService::TraceString(TEXT("服务停止成功"),TraceLevel_Normal);

			break;
		}
	case SERVICE_STATUS_CONFIG:	//配置状态
		{
			//设置标题
			SetWindowText(TEXT("中心服务器 -- [ 初始化 ]"));

			//设置按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//提示信息
			CTraceService::TraceString(TEXT("正在初始化组件..."),TraceLevel_Normal);

			break;
		}
	case SERVICE_STATUS_SERVICE:	//服务状态
		{
			//设置标题
			SetWindowText(TEXT("中心服务器 -- [ 运行 ]"));

			//设置按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

			//提示信息
			CTraceService::TraceString(TEXT("服务启动成功"),TraceLevel_Normal);

			break;
		}
	}

	return;
}

//启动服务
VOID CCenterServerDlg::OnBnClickedStartService()
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

//调试服务
VOID CCenterServerDlg::OnBnClickedDebugService()
{
	//启动服务
	try
	{
		m_ServiceUnits.DebugService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//停止服务
VOID CCenterServerDlg::OnBnClickedConcludeService()
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

//关闭询问
BOOL CCenterServerDlg::OnQueryEndSession()
{
	//提示消息
	if (m_ServiceUnits.GetServiceStatus()!=SERVICE_STATUS_STOP)
	{
		CTraceService::TraceString(TEXT("服务正在运行中，系统要求注销回话请求失败"),TraceLevel_Warning);
		return FALSE;
	}

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
