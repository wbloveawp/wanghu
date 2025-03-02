#include "StdAfx.h"
#include "Resource.h"
#include "Tableframesink.h"
#include "GameServerManager.h"

//////////////////////////////////////////////////////////////////////////

//机器定义
#ifndef _DEBUG
#define ANDROID_SERVICE_DLL_NAME	TEXT("sparrowhzdgkAndroid.dll")	//组件名字
#else
#define ANDROID_SERVICE_DLL_NAME	TEXT("sparrowhzdgkAndroid.dll")	//组件名字
#endif

//全局变量
//static CGameServiceManager			g_GameServiceManager;				//管理变量
//全局变量
static CGameServiceManager			g_GameServiceManager;				//管理变量


//////////////////////////////////////////////////////////////////////////

//构造函数
CGameServiceManager::CGameServiceManager()
{
	m_hDllInstance = NULL;

	//构造属性
	m_GameServiceAttrib.cbDynamicJoin = TRUE;
	m_GameServiceAttrib.cbSegmentScore = FALSE;
	m_GameServiceAttrib.wKindID = KIND_ID;
	m_GameServiceAttrib.wChairCount = GAME_PLAYER;
	m_GameServiceAttrib.wSupporType = (GAME_GENRE_GOLD | GAME_GENRE_SCORE | GAME_GENRE_MATCH | GAME_GENRE_BATTLE);

	lstrcpyn(m_GameServiceAttrib.szGameName, GAME_NAME, CountArray(m_GameServiceAttrib.szGameName));
	lstrcpyn(m_GameServiceAttrib.szDataBaseName, TEXT("sparrowhzdgkDB"), CountArray(m_GameServiceAttrib.szDataBaseName));
	lstrcpyn(m_GameServiceAttrib.szServerDLLName, TEXT("sparrowhzdgkServer.DLL"), CountArray(m_GameServiceAttrib.szServerDLLName));
	lstrcpyn(m_GameServiceAttrib.szClientEXEName, TEXT("sparrowhzdgk.EXE"), CountArray(m_GameServiceAttrib.szClientEXEName));
	m_GameServiceAttrib.dwServerVersion = VERSION_SERVER;
	m_GameServiceAttrib.dwClientVersion = VERSION_CLIENT;

	return;
}

//析构函数
CGameServiceManager::~CGameServiceManager()
{
	//删除对象
	SafeDelete(m_pDlgCustomRule);

	//释放 DLL
	if (m_hDllInstance!=NULL)
	{
		AfxFreeLibrary(m_hDllInstance);
		m_hDllInstance=NULL;
	}
}

//接口查询
void * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
	return NULL;
}

//创建游戏桌
void * CGameServiceManager::CreateTableFrameSink(const IID & Guid, DWORD dwQueryVer)
{
	//建立对象
	CTableFrameSink * pTableFrameSink=NULL;
	try
	{
		pTableFrameSink = new CTableFrameSink();
		if (pTableFrameSink == NULL) throw TEXT("创建失败");
		void* pObject = pTableFrameSink->QueryInterface(Guid, dwQueryVer);
		if (pObject == NULL) throw TEXT("接口查询失败");
		return pObject;
	}
	catch (...) {}

	//清理对象
	SafeDelete(pTableFrameSink);

	return NULL;
}

bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//效验状态
	ASSERT(m_pDlgCustomRule!=NULL);
	if (m_pDlgCustomRule==NULL) return false;

	//变量定义
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;

	//获取配置
	if (m_pDlgCustomRule->GetCustomRule(*pCustomRule)==false)
	{
		return false;
	}

	return true;
}

bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//变量定义
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;

	//设置变量
	pCustomRule->cbTimeOutCard=20;
	pCustomRule->cbTimeStartGame=30;
	pCustomRule->cbTimeOperateCard=10;
	pCustomRule->cbTimeWaitEnd=10;

	//规则定义
	pCustomRule->bHuanSanZhang = true;
	pCustomRule->bHuJiaoZhuanYi = true;
	pCustomRule->bZiMoAddTimes = true;	
	pCustomRule->bTianDiHu = true;
	pCustomRule->bHaiDiLaoYue = true;
	pCustomRule->bMenQing = true;
	pCustomRule->cbTrusteeDelayTime = 5;

	return true;
}

HWND CGameServiceManager::CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//创建窗口
	if (m_pDlgCustomRule==NULL)
	{
		m_pDlgCustomRule=new CDlgCustomRule;
	}

	//创建窗口
	if (m_pDlgCustomRule->m_hWnd==NULL)
	{
		//设置资源
		AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

		//创建窗口
		m_pDlgCustomRule->Create(IDD_CUSTOM_RULE,pParentWnd);

		//还原资源
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//设置变量
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	m_pDlgCustomRule->SetCustomRule(*((tagCustomRule *)pcbCustomRule));

	//显示窗口
	m_pDlgCustomRule->SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER|SWP_SHOWWINDOW);

	return m_pDlgCustomRule->GetSafeHwnd();
}


//创建机器
VOID*  CGameServiceManager::CreateRobotUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	try
	{
		//加载模块
		if (m_hDllInstance == NULL)
		{
			m_hDllInstance = AfxLoadLibrary(TEXT("sparrowhzdgkRobot.dll"));
			if (m_hDllInstance == NULL) throw TEXT("ROBOT服务模块不存在");
		}

		//寻找函数
		ModuleCreateProc* CreateProc = (ModuleCreateProc*)GetProcAddress(m_hDllInstance, "CreateRobotUserItemSink");
		if (CreateProc == NULL) throw TEXT("ROBOT服务模块组件不合法");

		//创建组件
		return CreateProc(Guid, dwQueryVer);
	}
	catch (...) {}
	return NULL;
}


bool CGameServiceManager::GetGameServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	//设置变量
	GameServiceAttrib = m_GameServiceAttrib;

	return true;
}


//参数修改
bool CGameServiceManager::RectifyOptionParameter(tagGameServiceOption & GameServiceOption)
{
	//效验参数
	ASSERT(&GameServiceOption != NULL);
	if (&GameServiceOption == NULL) return false;

	//单元积分
	GameServiceOption.lCellScore = __max(1L, GameServiceOption.lCellScore);

	//金币游戏
	if (GameServiceOption.wServerType & (GAME_GENRE_GOLD | SCORE_GENRE_POSITIVE))
	{
		GameServiceOption.lMinEnterScore = __max(GameServiceOption.lCellScore * 10L, GameServiceOption.lMinEnterScore);

		//锁定限制
		GameServiceOption.lLessLockScore = GameServiceOption.lLessScore;
		/*GameServiceOption.lMaxLockScore=1000000000L;*/
	}

	GameServiceOption.lMaxLockScore = 1000000000L;

	//输分限制(原来的积分上限)
	if (GameServiceOption.lRestrictScore != 0L)
	{
		GameServiceOption.lRestrictScore = __max(GameServiceOption.lRestrictScore, GameServiceOption.lMinEnterScore);
	}

	//积分上限
	if ((GameServiceOption.wServerType & GAME_GENRE_SCORE) == 0)
	{
		if (GameServiceOption.lMaxEnterScore <= GameServiceOption.lMinEnterScore)
			GameServiceOption.lMaxEnterScore = 0L;
		else if (GameServiceOption.lRestrictScore > 0)
			GameServiceOption.lMaxEnterScore = __min(GameServiceOption.lMaxEnterScore, GameServiceOption.lRestrictScore);
	}

	return true;
}


//////////////////////////////////////////////////////////////////////////

//建立对象函数
extern "C" __declspec(dllexport) void * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	return g_GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////
