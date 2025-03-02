#include "Stdafx.h"
#include "BattleServer.h"
#include "BattleServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//�������
CBattleServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CBattleServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CBattleServerApp::CBattleServerApp()
{
}

//��������
BOOL CBattleServerApp::InitInstance()
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
	CBattleServerDlg BattleServerDlg;
	m_pMainWnd=&BattleServerDlg;
	BattleServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
