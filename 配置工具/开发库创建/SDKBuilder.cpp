#include "Stdafx.h"
#include "SDKBuilder.h"
#include "SDKBuilderDlg.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSDKBuilderApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

CSDKBuilderApp theApp;

//���캯��
CSDKBuilderApp::CSDKBuilderApp()
{
}

//��ʼ��
BOOL CSDKBuilderApp::InitInstance()
{
	__super::InitInstance();

	//�ڲ���ʼ��
	AfxInitRichEdit();
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//��ʾ����
	CSDKBuilderDlg SDKBuilderDlg;
	m_pMainWnd=&SDKBuilderDlg;
	SDKBuilderDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////////////
