#include "Stdafx.h"
#include "CenterServer.h"
#include "CenterServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//�������
CCenterServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCenterServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CCenterServerApp::CCenterServerApp()
{
}

//��������
BOOL CCenterServerApp::InitInstance()
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
	CCenterServerDlg CenterServerDlg;
	m_pMainWnd=&CenterServerDlg;
	CenterServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
