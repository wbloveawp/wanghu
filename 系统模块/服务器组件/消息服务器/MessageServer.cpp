#include "Stdafx.h"
#include "MessageServer.h"
#include "MessageServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//�������
CMessageServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMessageServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CMessageServerApp::CMessageServerApp()
{
}

//��������
BOOL CMessageServerApp::InitInstance()
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
	CMessageServerDlg MessageServerDlg;
	m_pMainWnd=&MessageServerDlg;
	MessageServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
