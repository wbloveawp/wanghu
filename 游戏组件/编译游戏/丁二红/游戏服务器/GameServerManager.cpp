#include "StdAfx.h"
#include "Resource.h"
#include "Tableframesink.h"
#include "GameServerManager.h"

//////////////////////////////////////////////////////////////////////////

//全局变量
static CGameServiceManager			g_GameServiceManager;				//管理变量

//////////////////////////////////////////////////////////////////////////


//构造函数
CGameServiceManager::CGameServiceManager()
{
	m_hDllInstance = NULL;

	//构造属性
	m_GameServiceAttrib.cbDynamicJoin=TRUE;
	m_GameServiceAttrib.cbSegmentScore=FALSE;
	m_GameServiceAttrib.wKindID=KIND_ID;
	m_GameServiceAttrib.wChairCount=GAME_PLAYER;
	m_GameServiceAttrib.wSupporType=(GAME_GENRE_GOLD | GAME_GENRE_SCORE | GAME_GENRE_MATCH | GAME_GENRE_BATTLE);

	lstrcpyn(m_GameServiceAttrib.szGameName,GAME_NAME,CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName,TEXT("CheXuanDB"), CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName,TEXT("CheXuanServer.DLL"),CountArray(m_GameServiceAttrib.szServerDLLName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName,TEXT("CheXuan.EXE"),CountArray(m_GameServiceAttrib.szClientEXEName));
	m_GameServiceAttrib.dwServerVersion=VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion=VERSION_CLIENT;

	return;
}

//析构函数
CGameServiceManager::~CGameServiceManager(void)
{
}

//接口查询
VOID * __cdecl CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	//QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	//QUERYINTERFACE(IGameServiceCustomRule,Guid,dwQueryVer);
	//QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);

	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);

	return NULL;
}


//获取配置
bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	return m_pDlgCustomRule.SaveCustomRule(pcbCustomRule, wCustonSize);
}

//默认配置
bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	return m_pDlgCustomRule.DefaultCustomRule(pcbCustomRule, wCustonSize);
}

//创建窗口
HWND CGameServiceManager::CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//设置变量
	m_pDlgCustomRule.SetCustomRule(pcbCustomRule, wCustonSize);

	//创建窗口
	if (m_pDlgCustomRule.m_hWnd==NULL)
	{
		//设置资源
		AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

		//创建窗口
		m_pDlgCustomRule.Create(IDD_CUSTOM_RULE,pParentWnd);

		//还原资源
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//显示窗口
	m_pDlgCustomRule.SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER|SWP_SHOWWINDOW);

	return m_pDlgCustomRule.GetSafeHwnd();
}

//创建游戏桌
VOID * __cdecl CGameServiceManager::CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)
{
	//建立对象
	CTableFrameSink * pTableFrameSink=NULL;
	try
	{
		pTableFrameSink=new CTableFrameSink();
		if (pTableFrameSink==NULL) throw TEXT("创建失败");
		void * pObject=pTableFrameSink->QueryInterface(Guid,dwQueryVer);
		if (pObject==NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pTableFrameSink);

	return NULL;
}


//创建机器
VOID * __cdecl CGameServiceManager::CreateRobotUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	try
	{
		//加载模块
		if (m_hDllInstance == NULL)
		{
			m_hDllInstance = AfxLoadLibrary(TEXT("CheXuanRobot.dll"));
			if (m_hDllInstance == NULL) throw TEXT("ROBOT服务模块不存在");
		}

		//寻找函数
		ModuleCreateProc * CreateProc = (ModuleCreateProc *)GetProcAddress(m_hDllInstance, "CreateRobotUserItemSink");
		if (CreateProc == NULL) throw TEXT("ROBOT服务模块组件不合法");

		//创建组件
		return CreateProc(Guid, dwQueryVer);
	}
	catch (...) {}
	return NULL;
}

bool __cdecl CGameServiceManager::GetGameServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	//设置变量
	GameServiceAttrib=m_GameServiceAttrib;

	return true;
}


//参数修改
bool __cdecl CGameServiceManager::RectifyOptionParameter(tagGameServiceOption & GameServiceOption)
{
	//效验参数
	ASSERT(&GameServiceOption!=NULL);
	if (&GameServiceOption==NULL) return false;

	//单元积分
	GameServiceOption.lCellScore=__max(1L,GameServiceOption.lCellScore);

	//金币游戏
	if (GameServiceOption.wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
	{
		GameServiceOption.lMinEnterScore=__max(GameServiceOption.lCellScore*10L,GameServiceOption.lMinEnterScore);

		//锁定限制
		GameServiceOption.lLessLockScore=GameServiceOption.lLessScore;
		/*GameServiceOption.lMaxLockScore=1000000000L;*/
	}

	GameServiceOption.lMaxLockScore = 1000000000L;

	//输分限制(原来的积分上限)
	if (GameServiceOption.lRestrictScore!=0L)
	{
		GameServiceOption.lRestrictScore=__max(GameServiceOption.lRestrictScore,GameServiceOption.lMinEnterScore);
	}

	//积分上限
	if ((GameServiceOption.wServerType&GAME_GENRE_SCORE)==0)
	{
		if(GameServiceOption.lMaxEnterScore<=GameServiceOption.lMinEnterScore)
			GameServiceOption.lMaxEnterScore=0L;
		else if(GameServiceOption.lRestrictScore>0)
			GameServiceOption.lMaxEnterScore=__min(GameServiceOption.lMaxEnterScore,GameServiceOption.lRestrictScore);
	}

	return true;
}



//////////////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) VOID * __cdecl CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	//static CGameServiceManager GameServiceManager;
	//return GameServiceManager.QueryInterface(Guid,dwInterfaceVer);

	return g_GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}


//////////////////////////////////////////////////////////////////////////
