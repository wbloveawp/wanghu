#include "Stdafx.h"
#include "LoggerServer.h"
#include "LoggerServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//程序对象
CLoggerServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CLoggerServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CLoggerServerApp::CLoggerServerApp()
{
}

//启动函数
BOOL CLoggerServerApp::InitInstance()
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
	CLoggerServerDlg LoggerServerDlg;
	m_pMainWnd=&LoggerServerDlg;
	LoggerServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
