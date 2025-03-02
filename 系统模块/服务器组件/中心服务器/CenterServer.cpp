#include "Stdafx.h"
#include "CenterServer.h"
#include "CenterServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//程序对象
CCenterServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCenterServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CCenterServerApp::CCenterServerApp()
{
}

//启动函数
BOOL CCenterServerApp::InitInstance()
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
	CCenterServerDlg CenterServerDlg;
	m_pMainWnd=&CenterServerDlg;
	CenterServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
