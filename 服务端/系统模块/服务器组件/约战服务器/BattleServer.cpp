#include "Stdafx.h"
#include "BattleServer.h"
#include "BattleServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//程序对象
CBattleServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CBattleServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CBattleServerApp::CBattleServerApp()
{
}

//启动函数
BOOL CBattleServerApp::InitInstance()
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
	CBattleServerDlg BattleServerDlg;
	m_pMainWnd=&BattleServerDlg;
	BattleServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
