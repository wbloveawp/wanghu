#include "Stdafx.h"
#include "MessageServer.h"
#include "MessageServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//程序对象
CMessageServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMessageServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CMessageServerApp::CMessageServerApp()
{
}

//启动函数
BOOL CMessageServerApp::InitInstance()
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
	CMessageServerDlg MessageServerDlg;
	m_pMainWnd=&MessageServerDlg;
	MessageServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
