#include "Stdafx.h"
#include "Collocate.h"
#include "CollocateDlg.h"

//////////////////////////////////////////////////////////////////////////////////

//�������
CCollocateApp theApp;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCollocateApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CCollocateApp::CCollocateApp()
{
}

//��������
BOOL CCollocateApp::InitInstance()
{
	__super::InitInstance();

	//�������
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//����ע��
	SetRegistryKey(szProduct);

	//��������
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	//��ȡ·��
	TCHAR szWorkDir[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szWorkDir, CountArray(szWorkDir));

	//����·��
	SetCurrentDirectory(szWorkDir);

	//�������
	if (StartDetermine() == true)
	{
		//��������
		CCollocateDlg CollocateDlg;
		m_pMainWnd = &CollocateDlg;

		//��ʾ����
		CollocateDlg.DoModal();
	}

	return FALSE;
}

//���̼��
bool CCollocateApp::StartDetermine()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
