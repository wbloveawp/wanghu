#include "Stdafx.h"
#include "ClubPlaza.h"
#include "ClubPlazaDlg.h"

//////////////////////////////////////////////////////////////////////////

//�������
CClubPlazaApp theApp;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CClubPlazaApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CClubPlazaApp::CClubPlazaApp()
{
}

//��������
BOOL CClubPlazaApp::InitInstance()
{
	__super::InitInstance();

	//�������
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//����ע���
	SetRegistryKey(szProduct);

	//��ʾ����
	CClubPlazaDlg CorrespondServerDlg;
	m_pMainWnd=&CorrespondServerDlg;
	CorrespondServerDlg.DoModal();

	return FALSE;
}

//////////////////////////////////////////////////////////////////////////
