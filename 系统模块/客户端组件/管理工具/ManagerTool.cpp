#include "Stdafx.h"
#include "tlhelp32.h"
#include "ManagerTool.h"
#include "GlobalUnits.h"
#include "PlatformFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

CPlatformFrame * g_pPlatformFrame=NULL;

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGamePlazaApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CGamePlazaApp::CGamePlazaApp()
{
	//���ñ���
	//m_pIDispatch=NULL;

	//���ñ���	
	m_AtomPlazaClass=NULL;
	m_hSocketMutex=INVALID_SOCKET;

	return;
}

//��ʼ������
BOOL CGamePlazaApp::InitInstance()
{
	__super::InitInstance();

	//ȫ�ֵ�Ԫ
	if (g_GlobalUnits.InitGlobalUnits()==false)
	{
		AfxMessageBox(TEXT("��Ϸ���������ʼ��ʧ�ܣ����򼴽��رգ�"),MB_ICONINFORMATION);
		return FALSE;
	}

	CGlobalStation * pGlobalStation=CGlobalStation::GetInstance();

	//�㳡����
	TCHAR szStationPlazaClass[32]=TEXT("");
	// add by humroly 2018/8/3
	// replace func
	_sntprintf_s(szStationPlazaClass, CountArray(szStationPlazaClass), TEXT("%s_RobotManager"), szPlazaClass);

#ifndef _DEBUG
#ifndef ADMIN_PLAZA

	//��������
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2,2),&WSAData)!=0) return FALSE;	

#endif
#endif
	//��������
	AfxOleInit();
	AfxInitRichEdit2();
	InitCommonControls();

	//��������
	//m_pIDispatch=new CImpIDispatch;
	AfxEnableControlContainer(new CCustomOccManager); 

	//���ñ���
	SetRegistryKey(szProductKey);

	//��������
	g_pPlatformFrame = new CPlatformFrame;
	g_pPlatformFrame->Create(IDD_GAME_PLAZA,CWnd::FromHandle(GetDesktopWindow()));

	//���ñ���
	m_pMainWnd=g_pPlatformFrame;

	return TRUE;
}

//�˳�����
int CGamePlazaApp::ExitInstance()
{
	//if (m_pIDispatch != NULL) SafeDelete(m_pIDispatch);
	if (g_pPlatformFrame!=NULL) SafeDelete(g_pPlatformFrame);

	return CWinApp::ExitInstance();
}

//////////////////////////////////////////////////////////////////////////

//Ӧ�ó������
CGamePlazaApp theApp;

//////////////////////////////////////////////////////////////////////////