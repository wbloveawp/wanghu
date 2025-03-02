#include "Stdafx.h"
#include "QueryServer.h"
#include "QueryServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//程序对象
CQueryServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CQueryServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CQueryServerApp::CQueryServerApp()
{
}

//启动函数
BOOL CQueryServerApp::InitInstance()
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
	CQueryServerDlg QueryServerDlg;
	m_pMainWnd=&QueryServerDlg;
	QueryServerDlg.DoModal();

	return FALSE;
}

//退出函数
int CQueryServerApp::ExitInstance()
{
	//关闭模块
	google::protobuf::ShutdownProtobufLibrary();

	return __super::ExitInstance();
}

//////////////////////////////////////////////////////////////////////////
