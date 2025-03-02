#include "Stdafx.h"
#include "LoggerServer.h"
#include "LoggerServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//�������
CLoggerServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CLoggerServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CLoggerServerApp::CLoggerServerApp()
{
}

//��������
BOOL CLoggerServerApp::InitInstance()
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
	CLoggerServerDlg LoggerServerDlg;
	m_pMainWnd=&LoggerServerDlg;
	LoggerServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
