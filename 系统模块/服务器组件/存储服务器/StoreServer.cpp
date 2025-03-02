#include "Stdafx.h"
#include "StoreServer.h"
#include "StoreServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//程序对象
CStoreServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CStoreServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CStoreServerApp::CStoreServerApp()
{
}

//启动函数
BOOL CStoreServerApp::InitInstance()
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
	CStoreServerDlg StoreServerDlg;
	m_pMainWnd=&StoreServerDlg;
	StoreServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
