#include "Stdafx.h"
#include "ClubPlaza.h"
#include "ClubPlazaDlg.h"

//////////////////////////////////////////////////////////////////////////

//程序对象
CClubPlazaApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CClubPlazaApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CClubPlazaApp::CClubPlazaApp()
{
}

//启动函数
BOOL CClubPlazaApp::InitInstance()
{
	__super::InitInstance();

	//设置组件
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//设置注册表
	SetRegistryKey(szProduct);

	//显示窗口
	CClubPlazaDlg CorrespondServerDlg;
	m_pMainWnd=&CorrespondServerDlg;
	CorrespondServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
