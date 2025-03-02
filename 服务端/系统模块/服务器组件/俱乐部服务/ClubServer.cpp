#include "Stdafx.h"
#include "ClubServer.h"
#include "ClubServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//程序对象
CClubServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CClubServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CClubServerApp::CClubServerApp()
{
}

//启动函数
BOOL CClubServerApp::InitInstance()
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
	CClubServerDlg CorrespondServerDlg;
	m_pMainWnd=&CorrespondServerDlg;
	CorrespondServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
