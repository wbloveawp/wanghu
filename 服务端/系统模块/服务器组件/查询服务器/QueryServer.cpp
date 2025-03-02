#include "Stdafx.h"
#include "QueryServer.h"
#include "QueryServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//�������
CQueryServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CQueryServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CQueryServerApp::CQueryServerApp()
{
}

//��������
BOOL CQueryServerApp::InitInstance()
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
	CQueryServerDlg QueryServerDlg;
	m_pMainWnd=&QueryServerDlg;
	QueryServerDlg.DoModal();

	return FALSE;
}

//�˳�����
int CQueryServerApp::ExitInstance()
{
	//�ر�ģ��
	google::protobuf::ShutdownProtobufLibrary();

	return __super::ExitInstance();
}

//////////////////////////////////////////////////////////////////////////
