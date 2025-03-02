#include "Stdafx.h"
#include "LogonServer.h"
#include "LogonServerDlg.h"


//////////////////////////////////////////////////////////////////////////

//�������
CLogonServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CLogonServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CLogonServerApp::CLogonServerApp()
{
}

//��������
BOOL CLogonServerApp::InitInstance()
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
	CLogonServerDlg LogonServerDlg;
	m_pMainWnd=&LogonServerDlg;
	LogonServerDlg.DoModal();

	return FALSE;
}

//�˳�����
int CLogonServerApp::ExitInstance()
{
	//�ر�ģ��
	//google::protobuf::ShutdownProtobufLibrary();

	return __super::ExitInstance();
}

//////////////////////////////////////////////////////////////////////////
