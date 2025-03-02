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

//构造函数
CGamePlazaApp::CGamePlazaApp()
{
	//设置变量
	//m_pIDispatch=NULL;

	//设置变量	
	m_AtomPlazaClass=NULL;
	m_hSocketMutex=INVALID_SOCKET;

	return;
}

//初始化函数
BOOL CGamePlazaApp::InitInstance()
{
	__super::InitInstance();

	//全局单元
	if (g_GlobalUnits.InitGlobalUnits()==false)
	{
		AfxMessageBox(TEXT("游戏大厅组件初始化失败，程序即将关闭！"),MB_ICONINFORMATION);
		return FALSE;
	}

	CGlobalStation * pGlobalStation=CGlobalStation::GetInstance();

	//广场类名
	TCHAR szStationPlazaClass[32]=TEXT("");
	// add by humroly 2018/8/3
	// replace func
	_sntprintf_s(szStationPlazaClass, CountArray(szStationPlazaClass), TEXT("%s_RobotManager"), szPlazaClass);

#ifndef _DEBUG
#ifndef ADMIN_PLAZA

	//配置网络
	WSADATA WSAData;
	if (WSAStartup(MAKEWORD(2,2),&WSAData)!=0) return FALSE;	

#endif
#endif
	//环境配置
	AfxOleInit();
	AfxInitRichEdit2();
	InitCommonControls();

	//创建对象
	//m_pIDispatch=new CImpIDispatch;
	AfxEnableControlContainer(new CCustomOccManager); 

	//设置表名
	SetRegistryKey(szProductKey);

	//建立窗口
	g_pPlatformFrame = new CPlatformFrame;
	g_pPlatformFrame->Create(IDD_GAME_PLAZA,CWnd::FromHandle(GetDesktopWindow()));

	//设置变量
	m_pMainWnd=g_pPlatformFrame;

	return TRUE;
}

//退出函数
int CGamePlazaApp::ExitInstance()
{
	//if (m_pIDispatch != NULL) SafeDelete(m_pIDispatch);
	if (g_pPlatformFrame!=NULL) SafeDelete(g_pPlatformFrame);

	return CWinApp::ExitInstance();
}

//////////////////////////////////////////////////////////////////////////

//应用程序对象
CGamePlazaApp theApp;

//////////////////////////////////////////////////////////////////////////