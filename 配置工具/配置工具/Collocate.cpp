#include "Stdafx.h"
#include "Collocate.h"
#include "CollocateDlg.h"

//////////////////////////////////////////////////////////////////////////////////

//程序对象
CCollocateApp theApp;

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCollocateApp, CWinApp)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CCollocateApp::CCollocateApp()
{
}

//启动函数
BOOL CCollocateApp::InitInstance()
{
	__super::InitInstance();

	//设置组件
	AfxInitRichEdit2();
	InitCommonControls();
	AfxEnableControlContainer();

	//设置注册
	SetRegistryKey(szProduct);

	//配置网络
	WSADATA WSAData;
	WSAStartup(MAKEWORD(2, 2), &WSAData);

	//获取路径
	TCHAR szWorkDir[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szWorkDir, CountArray(szWorkDir));

	//设置路径
	SetCurrentDirectory(szWorkDir);

	//启动检测
	if (StartDetermine() == true)
	{
		//变量定义
		CCollocateDlg CollocateDlg;
		m_pMainWnd = &CollocateDlg;

		//显示窗口
		CollocateDlg.DoModal();
	}

	return FALSE;
}

//进程检测
bool CCollocateApp::StartDetermine()
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////
