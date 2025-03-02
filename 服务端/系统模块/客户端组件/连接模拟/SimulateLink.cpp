#include "Stdafx.h"
#include "SimulateLink.h"
#include "SimulateLinkDlg.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSimulateLinkApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CSimulateLinkApp::CSimulateLinkApp()
{
}

//创建函数
BOOL CSimulateLinkApp::InitInstance()
{
	__super::InitInstance();

	//环境配置
	AfxOleInit();
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//显示窗口
	CSimulateLinkDlg SimulateLinkDlg;
	m_pMainWnd = &SimulateLinkDlg;
	SimulateLinkDlg.DoModal();

	return FALSE;
}

//退出函数
INT CSimulateLinkApp::ExitInstance()
{
	return __super::ExitInstance();
}

//////////////////////////////////////////////////////////////////////////

//对象说明
CSimulateLinkApp theApp;

//////////////////////////////////////////////////////////////////////////
