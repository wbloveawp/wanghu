#include "Stdafx.h"
#include "LogonServer.h"
#include "LogonServerDlg.h"


//////////////////////////////////////////////////////////////////////////

//程序对象
CLogonServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CLogonServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CLogonServerApp::CLogonServerApp()
{
}

//启动函数
BOOL CLogonServerApp::InitInstance()
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
	CLogonServerDlg LogonServerDlg;
	m_pMainWnd=&LogonServerDlg;
	LogonServerDlg.DoModal();

	return FALSE;
}

//退出函数
int CLogonServerApp::ExitInstance()
{
	//关闭模块
	//google::protobuf::ShutdownProtobufLibrary();

	return __super::ExitInstance();
}

//////////////////////////////////////////////////////////////////////////
