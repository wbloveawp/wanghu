#include "Stdafx.h"
#include "MonitorServer.h"
#include "MonitorServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//程序对象
CMonitorServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMonitorServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CMonitorServerApp::CMonitorServerApp()
{
}

//启动函数
BOOL CMonitorServerApp::InitInstance()
{
	__super::InitInstance();

	//设置组件
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//设置注册表
	SetRegistryKey(szProduct);

	//设置异常过滤
	SetUnhandledExceptionFilter(CCrashCapture::ExceptionFilter);

	//显示窗口
	CMonitorServerDlg MonitorServerDlg;
	m_pMainWnd=&MonitorServerDlg;
	MonitorServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
