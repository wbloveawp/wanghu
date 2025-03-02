#include "Stdafx.h"
#include "MonitorServer.h"
#include "MonitorServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//�������
CMonitorServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMonitorServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CMonitorServerApp::CMonitorServerApp()
{
}

//��������
BOOL CMonitorServerApp::InitInstance()
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
	CMonitorServerDlg MonitorServerDlg;
	m_pMainWnd=&MonitorServerDlg;
	MonitorServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
