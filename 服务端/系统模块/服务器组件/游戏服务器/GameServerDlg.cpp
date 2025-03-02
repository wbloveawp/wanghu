#include "Stdafx.h"
#include "Resource.h"
#include "GlobalUnits.h"
#include "GameServerDlg.h"
#include "DlgServerList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//消息定义

#define WM_PROCESS_CMD_LINE			(WM_USER+100)						//处理命令

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CGameServerDlg, CDialog)
	ON_WM_QUERYENDSESSION()
	ON_BN_CLICKED(IDC_OPEN_SERVER, OnBnClickedOpenServer)
	ON_BN_CLICKED(IDC_STOP_SERVICE, OnBnClickedConcludeService)
	ON_BN_CLICKED(IDC_DEBUG_SERVICE, OnBnClickedDebugService)
	ON_BN_CLICKED(IDC_START_SERVICE, OnBnClickedStartService)
	ON_BN_CLICKED(IDC_CREATE_SERVER, OnBnClickedCreateServer)
	ON_BN_CLICKED(IDC_OPTION_SERVER, OnBnClickedOptionServer)	
	


	//自定消息
	ON_MESSAGE(WM_PROCESS_CMD_LINE,OnMessageProcessCmdLine)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameServerDlg::CGameServerDlg() : CDialog(IDD_DLG_GAME_SERVER)
{
	//模块参数
	m_bOptionSuccess=false;	
	ZeroMemory(&m_ModuleInitParameter,sizeof(m_ModuleInitParameter));

	return;
}

//析构函数
CGameServerDlg::~CGameServerDlg()
{
}

//控件绑定
VOID CGameServerDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_RichEditTrace);
}

//初始化函数
BOOL CGameServerDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置标题
	SetWindowText(TEXT("游戏服务器 -- [ 停止 ]"));

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//信息组件
	if (CTraceService::SetTraceService(QUERY_OBJECT_INTERFACE(m_RichEditTrace,IUnknownEx))==false)
	{
		CTraceService::TraceString(TEXT("信息输出接口设置失败，系统运行信息将无法显示"),TraceLevel_Exception);
	}

	//命令处理
	LPCTSTR pszCmdLine=AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0]!=0) PostMessage(WM_PROCESS_CMD_LINE,0,(LPARAM)pszCmdLine);

	//创建组件
	if (m_ServiceUnitsModule.GetInterface() == NULL)
	{
		//创建组件
		if (m_ServiceUnitsModule.CreateInstance() == false)
		{
			CTraceService::TraceString(TEXT("游戏服务组件创建失败"), TraceLevel_Exception);			
		}

		//设置接口
		if (m_ServiceUnitsModule->SetServiceUnitsSink(this) == false)
		{
			CTraceService::TraceString(TEXT("绑定服务组件状态接口失败"), TraceLevel_Exception);			
		}
	}

	return TRUE;
}

//确定消息
VOID CGameServerDlg::OnOK()
{
	return;
}

//取消函数
VOID CGameServerDlg::OnCancel()
{
	//关闭询问
	if (m_ServiceUnitsModule.GetInterface()!=NULL)
	{
		if (m_ServiceUnitsModule->GetServiceStatus()!=SERVICE_STATUS_STOP)
		{
			LPCTSTR pszQuestion=TEXT("游戏服务器正在运行中，您确实要关闭服务器吗？");
			if (AfxMessageBox(pszQuestion,MB_YESNO|MB_DEFBUTTON2|MB_ICONQUESTION)!=IDYES) return;
		}

		//停止服务
		if (!m_ServiceUnitsModule->ConcludeService())
			return;
	}

	__super::OnCancel();
}

//消息解释
BOOL CGameServerDlg::PreTranslateMessage(MSG * pMsg)
{
	//按键过滤
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE))
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//服务状态
VOID CGameServerDlg::OnServiceUnitsStatus(WORD ServiceStatus)
{
	//状态设置
	switch (ServiceStatus)
	{
	case SERVICE_STATUS_STOP:	//停止状态
		{
			//更新标题
			UpdateServerTitle();

			//设置按钮
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(TRUE);
			GetDlgItem(IDC_RUN_PARAMETER)->EnableWindow(FALSE);
			GetDlgItem(IDC_SERVICE_CONTROL)->EnableWindow(FALSE);

			//提示信息
			CTraceService::TraceString(TEXT("服务停止成功"),TraceLevel_Normal);

			//关闭判断
			if (g_GlobalUnits.m_bFailureClose==true) PostMessage(WM_CLOSE);

			break;
		}
	case SERVICE_STATUS_CONFIG:	//配置状态
		{
			//更新标题
			UpdateServerTitle();

			//设置按钮
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RUN_PARAMETER)->EnableWindow(FALSE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_SERVICE_CONTROL)->EnableWindow(FALSE);

			//提示信息
			CTraceService::TraceString(TEXT("正在初始化组件..."),TraceLevel_Normal);

			break;
		}
	case SERVICE_STATUS_SERVICE:	//服务状态
		{
			//更新标题
			UpdateServerTitle();

			//设置按钮
			GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
			GetDlgItem(IDC_RUN_PARAMETER)->EnableWindow(TRUE);
			GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);
			GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(FALSE);
			GetDlgItem(IDC_SERVICE_CONTROL)->EnableWindow(TRUE);

			//提示信息
			CTraceService::TraceString(TEXT("服务启动成功"),TraceLevel_Normal);

			break;
		}
	case SERVICE_STATUS_WILLSTOP://即将关闭
		{
			//设置按钮
			GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);

			break;
		}
	}

	return;
}

//启动服务
VOID CGameServerDlg::OnBnClickedStartService()
{
	//参数效验
	ASSERT(m_bOptionSuccess==true);
	if (m_bOptionSuccess==false) return;

	//创建组件
	if (m_ServiceUnitsModule.GetInterface()==NULL)
	{
		//创建组件
		if (m_ServiceUnitsModule.CreateInstance()==false)
		{
			CTraceService::TraceString(TEXT("游戏服务组件创建失败"),TraceLevel_Exception);
			return;
		}

		//设置接口
		if (m_ServiceUnitsModule->SetServiceUnitsSink(this)==false)
		{
			CTraceService::TraceString(TEXT("绑定服务组件状态接口失败"),TraceLevel_Exception);
			return;
		}
	}

	//启动服务
	try
	{
		//设置模块
		LPCTSTR pszServerDLLName=m_ModuleInitParameter.GameServiceAttrib.szServerDLLName;		
		m_ServiceUnitsModule->SetServiceCollocate(pszServerDLLName,m_ModuleInitParameter.GameServiceOption);

		//启动服务
		m_ServiceUnitsModule->StartService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//停止服务
VOID CGameServerDlg::OnBnClickedConcludeService()
{
	//效验状态
	ASSERT(m_ServiceUnitsModule.GetInterface()!=NULL);
	if (m_ServiceUnitsModule.GetInterface()==NULL) return;

	//停止服务
	try
	{
		m_ServiceUnitsModule->ConcludeService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//打开房间
VOID CGameServerDlg::OnBnClickedOpenServer()
{
	//设置资源
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//配置窗口
	CDlgServerList DlgServerList;
	DlgServerList.DoModal();

	//加载配置
	if (DlgServerList.m_bOptionSuccess==true)
	{
		//变量定义
		tagModuleInitParameter * pModuleInitParameter=&DlgServerList.m_ModuleInitParameter;

		//设置变量
		m_bOptionSuccess=true;
		m_ModuleInitParameter.GameServiceAttrib=pModuleInitParameter->GameServiceAttrib;
		m_ModuleInitParameter.GameServiceOption=pModuleInitParameter->GameServiceOption;

		//状态处理
		OnLoadModuleParameter();
	}

	return;
}

//创建房间
VOID CGameServerDlg::OnBnClickedCreateServer()
{
	//设置资源
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//配置房间
	CDlgServerWizard DlgServerWizard;
	if (DlgServerWizard.DoModal()!=IDOK) return;

	//设置变量
	m_bOptionSuccess=true;
	DlgServerWizard.GetGameServiceAttrib(m_ModuleInitParameter.GameServiceAttrib);
	DlgServerWizard.GetGameServiceOption(m_ModuleInitParameter.GameServiceOption);

	//状态处理
	OnLoadModuleParameter();

	return;	
}

//配置房间
VOID CGameServerDlg::OnBnClickedOptionServer()
{
	//游戏模块
	CGameServiceManagerHelper GameServiceManager;
	GameServiceManager.SetModuleCreateInfo(m_ModuleInitParameter.GameServiceAttrib.szServerDLLName,GAME_SERVICE_CREATE_NAME);

	//加载模块
	if (GameServiceManager.CreateInstance()==false)
	{
		AfxMessageBox(TEXT("服务组件不存在或者加载失败，房间配置失败"),MB_ICONERROR);
		return;
	}

	//设置资源
	AfxSetResourceHandle(GetModuleHandle(MODULE_MANAGER_DLL_NAME));

	//配置房间
	CDlgServerWizard DlgServerWizard;
	DlgServerWizard.SetGameServiceManager(GameServiceManager.GetInterface());
	DlgServerWizard.SetGameServiceOption(m_ModuleInitParameter.GameServiceOption);

	//配置房间
	if (DlgServerWizard.DoModal()!=IDOK) return;

	//获取属性
	tagGameServiceAttrib GameServiceAttrib;
	DlgServerWizard.GetGameServiceAttrib(GameServiceAttrib);

	//设置变量
	m_bOptionSuccess=true;
	DlgServerWizard.GetGameServiceAttrib(m_ModuleInitParameter.GameServiceAttrib);
	DlgServerWizard.GetGameServiceOption(m_ModuleInitParameter.GameServiceOption);

	//状态处理
	OnLoadModuleParameter();

	return;
}

//服务调试
VOID CGameServerDlg::OnBnClickedDebugService()
{
	//创建组件
	if (m_ServiceUnitsModule.GetInterface() == NULL)
	{
		return;		
	}

	//调试服务
	try
	{
		//启动服务
		m_ServiceUnitsModule->DebugService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

//更新标题
VOID CGameServerDlg::UpdateServerTitle()
{
	//构造标题
	if (m_bOptionSuccess==true)
	{
		//房间信息
		LPCTSTR pszGameName=m_ModuleInitParameter.GameServiceAttrib.szGameName;
		LPCTSTR pszServerName=m_ModuleInitParameter.GameServiceOption.szServerName;

		//获取状态
		LPCTSTR pszStatus=TEXT("停止");
		WORD ServiceStatus=SERVICE_STATUS_STOP;
		if (m_ServiceUnitsModule.GetInterface()!=NULL) ServiceStatus=m_ServiceUnitsModule->GetServiceStatus();

		//构造状态
		switch (ServiceStatus)
		{
		case SERVICE_STATUS_CONFIG:		//配置状态
			{
				pszStatus=TEXT("初始化");
				break; 
			}
		case SERVICE_STATUS_SERVICE:		//服务状态
			{ 
				pszStatus=TEXT("运行");
				break;
			}
		}

		//构造标题
		TCHAR szServerTitle[128]=TEXT("");
		_sntprintf_s(szServerTitle,CountArray(szServerTitle),TEXT("[ %s - %s ] -- [ %s ]"),pszGameName,pszServerName,pszStatus);

		//设置标题
		SetWindowText(szServerTitle);
	}
	else
	{
		SetWindowText(TEXT("游戏服务器 -- [ 停止 ]"));
		return;
	}

	return;
}

//配置成功
VOID CGameServerDlg::OnLoadModuleParameter()
{
	//参数效验
	ASSERT(m_bOptionSuccess==true);
	if (m_bOptionSuccess==false) return;

	//更新标题
	UpdateServerTitle();

	//设置按钮
	GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
	GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(TRUE);

	//设置控件
	SetDlgItemText(IDC_GAME_NAME,m_ModuleInitParameter.GameServiceAttrib.szGameName);
	if(m_ModuleInitParameter.GameServiceOption.wServerPort!=0)
		SetDlgItemInt(IDC_SERVER_PORT,m_ModuleInitParameter.GameServiceOption.wServerPort);
	else
		SetDlgItemText(IDC_SERVER_PORT,TEXT("自动适配"));
	SetDlgItemText(IDC_SERVER_NAME,m_ModuleInitParameter.GameServiceOption.szServerName);

	//构造提示
	TCHAR szString[256]=TEXT("");
	LPCTSTR pszServerName=m_ModuleInitParameter.GameServiceOption.szServerName;
	_sntprintf_s(szString,CountArray(szString),TEXT("[ %s ] 房间参数加载成功"),pszServerName);

	//输出信息
	CTraceService::TraceString(szString,TraceLevel_Normal);

	//自动启动
	if (g_GlobalUnits.m_bAutoStart==true) OnBnClickedStartService();

	return;
}

//关闭询问
BOOL CGameServerDlg::OnQueryEndSession()
{
	//提示消息
	if ((m_ServiceUnitsModule.GetInterface()!=NULL)&&(m_ServiceUnitsModule->GetServiceStatus()!=SERVICE_STATUS_STOP))
	{
		CTraceService::TraceString(TEXT("服务正在运行中，系统要求注销回话请求失败"),TraceLevel_Warning);
		return FALSE;
	}

	return TRUE;
}

//命令处理
LRESULT CGameServerDlg::OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam)
{
	LPCTSTR lpszComLine=(LPCTSTR)(lParam);

	//参数处理
	if (lpszComLine[0]!=0)
	{
		//变量定义
		WORD wServerID = 0;

		//提取变量
		UINT nStringLength=0;
		CString strItemString;
		LPCTSTR lpszBeginString=lpszComLine;

		//加载房间标识
		LPCTSTR pszServerID=TEXT("/ServerID:");
		UINT nServerIDLength=lstrlen(pszServerID);

		//提取参数
		while (true)
		{
			//参数分析
			LPCTSTR lpszEndString=_tcschr(lpszBeginString,TEXT(' '));
			nStringLength=(lpszEndString==NULL)?lstrlen(lpszBeginString):(UINT)(lpszEndString-lpszBeginString);

			//传输窗口
			if ((nStringLength>=nServerIDLength)&&(memcmp(lpszBeginString,pszServerID,nServerIDLength*sizeof(TCHAR))==0))
			{
				//提取子项
				CopyMemory(strItemString.GetBufferSetLength(nStringLength-nServerIDLength),lpszBeginString+nServerIDLength,
					(nStringLength-nServerIDLength)*sizeof(TCHAR));
				strItemString.ReleaseBuffer();

				//提取参数
				wServerID=(WORD)(_tstoi(strItemString));

				break;
			}

			//设置变量
			if (lpszEndString==NULL) break;
			lpszBeginString=(lpszEndString+1);
		}

		if( wServerID != 0 )
		{
			CDlgServerList DlgServerList;
			if( DlgServerList.LoadGameServer(wServerID) )
			{

				//设置变量
				g_GlobalUnits.m_bAutoStart = true;

				//变量定义
				tagModuleInitParameter * pModuleInitParameter=&DlgServerList.m_ModuleInitParameter;

				//设置变量
				m_bOptionSuccess=true;
				m_ModuleInitParameter.GameServiceAttrib=pModuleInitParameter->GameServiceAttrib;
				m_ModuleInitParameter.GameServiceOption=pModuleInitParameter->GameServiceOption;

				//状态处理
				OnLoadModuleParameter();
			}
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
