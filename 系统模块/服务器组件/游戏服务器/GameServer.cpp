#include "Stdafx.h"
#include "GameServer.h"
#include "GlobalUnits.h"
#include "GameServerDlg.h"
#include <dbghelp.h>

//////////////////////////////////////////////////////////////////////////

//�������
CGameServerApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameServerApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameServerApp::CGameServerApp()
{
}

//��������
BOOL CGameServerApp::InitInstance()
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

	//���ز���
	g_GlobalUnits.LoadServiceParameter();

	//��ʾ����
	CGameServerDlg GameServerDlg;
	m_pMainWnd=&GameServerDlg;
	GameServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
