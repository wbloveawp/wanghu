#include "Stdafx.h"
#include "GateServer.h"
#include "GateServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//程序对象
CGateServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGateServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGateServerApp::CGateServerApp()
{
}

//启动函数
BOOL CGateServerApp::InitInstance()
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
	CGateServerDlg GateServerDlg;
	m_pMainWnd=&GateServerDlg;
	GateServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
