#include "pch.h"
#include "Stdafx.h"
#include "AnalyseTool.h"
#include "AnalyseToolDlg.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CAnalyseToolApp::CAnalyseToolApp()
{
}

//接口查询
VOID* CAnalyseToolApp::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	return NULL;
}


//创建函数
BOOL CAnalyseToolApp::InitInstance()
{
	__super::InitInstance();

	//环境配置
	AfxOleInit();
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//显示窗口
	CAnalyseToolDlg AnalyseToolDlg;
	m_pMainWnd = &AnalyseToolDlg;
	AnalyseToolDlg.DoModal();

	return true;
}

//退出函数
INT CAnalyseToolApp::ExitInstance()
{

	return __super::ExitInstance();
}

//////////////////////////////////////////////////////////////////////////

//对象说明
CAnalyseToolApp theApp;

//////////////////////////////////////////////////////////////////////////
