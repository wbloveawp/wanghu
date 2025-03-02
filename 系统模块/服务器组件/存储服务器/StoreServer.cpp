#include "Stdafx.h"
#include "StoreServer.h"
#include "StoreServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//�������
CStoreServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CStoreServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CStoreServerApp::CStoreServerApp()
{
}

//��������
BOOL CStoreServerApp::InitInstance()
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
	CStoreServerDlg StoreServerDlg;
	m_pMainWnd=&StoreServerDlg;
	StoreServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
