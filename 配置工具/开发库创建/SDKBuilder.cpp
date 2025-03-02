#include "Stdafx.h"
#include "SDKBuilder.h"
#include "SDKBuilderDlg.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSDKBuilderApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

CSDKBuilderApp theApp;

//构造函数
CSDKBuilderApp::CSDKBuilderApp()
{
}

//初始化
BOOL CSDKBuilderApp::InitInstance()
{
	__super::InitInstance();

	//内部初始化
	AfxInitRichEdit();
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//显示窗口
	CSDKBuilderDlg SDKBuilderDlg;
	m_pMainWnd=&SDKBuilderDlg;
	SDKBuilderDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
