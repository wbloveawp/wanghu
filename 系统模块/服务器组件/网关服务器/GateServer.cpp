#include "Stdafx.h"
#include "GateServer.h"
#include "GateServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//�������
CGateServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGateServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CGateServerApp::CGateServerApp()
{
}

//��������
BOOL CGateServerApp::InitInstance()
{
	__super::InitInstance();

	//�������
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//����ע���
	SetRegistryKey(szProduct);

	//�����쳣����
	SetUnhandledExceptionFilter(CCrashCapture::ExceptionFilter);

	//��ʾ����
	CGateServerDlg GateServerDlg;
	m_pMainWnd=&GateServerDlg;
	GateServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
