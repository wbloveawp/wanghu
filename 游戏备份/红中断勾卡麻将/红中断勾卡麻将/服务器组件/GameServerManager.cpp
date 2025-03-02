#include "StdAfx.h"
#include "Resource.h"
#include "Tableframesink.h"
#include "GameServerManager.h"

//////////////////////////////////////////////////////////////////////////

//��������
#ifndef _DEBUG
#define ANDROID_SERVICE_DLL_NAME	TEXT("sparrowhzdgkAndroid.dll")	//�������
#else
#define ANDROID_SERVICE_DLL_NAME	TEXT("sparrowhzdgkAndroid.dll")	//�������
#endif

//ȫ�ֱ���
//static CGameServiceManager			g_GameServiceManager;				//�������
//ȫ�ֱ���
static CGameServiceManager			g_GameServiceManager;				//�������


//////////////////////////////////////////////////////////////////////////

//���캯��
CGameServiceManager::CGameServiceManager()
{
	m_hDllInstance = NULL;

	//��������
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

//��������
CGameServiceManager::~CGameServiceManager()
{
	//ɾ������
	SafeDelete(m_pDlgCustomRule);

	//�ͷ� DLL
	if (m_hDllInstance!=NULL)
	{
		AfxFreeLibrary(m_hDllInstance);
		m_hDllInstance=NULL;
	}
}

//�ӿڲ�ѯ
void * CGameServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceManager,Guid,dwQueryVer);
	QUERYINTERFACE(IGameServiceCustomRule,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceManager,Guid,dwQueryVer);
	return NULL;
}

//������Ϸ��
void * CGameServiceManager::CreateTableFrameSink(const IID & Guid, DWORD dwQueryVer)
{
	//��������
	CTableFrameSink * pTableFrameSink=NULL;
	try
	{
		pTableFrameSink = new CTableFrameSink();
		if (pTableFrameSink == NULL) throw TEXT("����ʧ��");
		void* pObject = pTableFrameSink->QueryInterface(Guid, dwQueryVer);
		if (pObject == NULL) throw TEXT("�ӿڲ�ѯʧ��");
		return pObject;
	}
	catch (...) {}

	//�������
	SafeDelete(pTableFrameSink);

	return NULL;
}

bool CGameServiceManager::SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//Ч��״̬
	ASSERT(m_pDlgCustomRule!=NULL);
	if (m_pDlgCustomRule==NULL) return false;

	//��������
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;

	//��ȡ����
	if (m_pDlgCustomRule->GetCustomRule(*pCustomRule)==false)
	{
		return false;
	}

	return true;
}

bool CGameServiceManager::DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)
{
	//��������
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	tagCustomRule * pCustomRule=(tagCustomRule *)pcbCustomRule;

	//���ñ���
	pCustomRule->cbTimeOutCard=20;
	pCustomRule->cbTimeStartGame=30;
	pCustomRule->cbTimeOperateCard=10;
	pCustomRule->cbTimeWaitEnd=10;

	//������
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
	//��������
	if (m_pDlgCustomRule==NULL)
	{
		m_pDlgCustomRule=new CDlgCustomRule;
	}

	//��������
	if (m_pDlgCustomRule->m_hWnd==NULL)
	{
		//������Դ
		AfxSetResourceHandle(GetModuleHandle(m_GameServiceAttrib.szServerDLLName));

		//��������
		m_pDlgCustomRule->Create(IDD_CUSTOM_RULE,pParentWnd);

		//��ԭ��Դ
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//���ñ���
	ASSERT(wCustonSize>=sizeof(tagCustomRule));
	m_pDlgCustomRule->SetCustomRule(*((tagCustomRule *)pcbCustomRule));

	//��ʾ����
	m_pDlgCustomRule->SetWindowPos(NULL,rcCreate.left,rcCreate.top,rcCreate.Width(),rcCreate.Height(),SWP_NOZORDER|SWP_SHOWWINDOW);

	return m_pDlgCustomRule->GetSafeHwnd();
}


//��������
VOID*  CGameServiceManager::CreateRobotUserItemSink(REFGUID Guid, DWORD dwQueryVer)
{
	try
	{
		//����ģ��
		if (m_hDllInstance == NULL)
		{
			m_hDllInstance = AfxLoadLibrary(TEXT("sparrowhzdgkRobot.dll"));
			if (m_hDllInstance == NULL) throw TEXT("ROBOT����ģ�鲻����");
		}

		//Ѱ�Һ���
		ModuleCreateProc* CreateProc = (ModuleCreateProc*)GetProcAddress(m_hDllInstance, "CreateRobotUserItemSink");
		if (CreateProc == NULL) throw TEXT("ROBOT����ģ��������Ϸ�");

		//�������
		return CreateProc(Guid, dwQueryVer);
	}
	catch (...) {}
	return NULL;
}


bool CGameServiceManager::GetGameServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)
{
	//���ñ���
	GameServiceAttrib = m_GameServiceAttrib;

	return true;
}


//�����޸�
bool CGameServiceManager::RectifyOptionParameter(tagGameServiceOption & GameServiceOption)
{
	//Ч�����
	ASSERT(&GameServiceOption != NULL);
	if (&GameServiceOption == NULL) return false;

	//��Ԫ����
	GameServiceOption.lCellScore = __max(1L, GameServiceOption.lCellScore);

	//�����Ϸ
	if (GameServiceOption.wServerType & (GAME_GENRE_GOLD | SCORE_GENRE_POSITIVE))
	{
		GameServiceOption.lMinEnterScore = __max(GameServiceOption.lCellScore * 10L, GameServiceOption.lMinEnterScore);

		//��������
		GameServiceOption.lLessLockScore = GameServiceOption.lLessScore;
		/*GameServiceOption.lMaxLockScore=1000000000L;*/
	}

	GameServiceOption.lMaxLockScore = 1000000000L;

	//�������(ԭ���Ļ�������)
	if (GameServiceOption.lRestrictScore != 0L)
	{
		GameServiceOption.lRestrictScore = __max(GameServiceOption.lRestrictScore, GameServiceOption.lMinEnterScore);
	}

	//��������
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

//����������
extern "C" __declspec(dllexport) void * CreateGameServiceManager(const GUID & Guid, DWORD dwInterfaceVer)
{
	return g_GameServiceManager.QueryInterface(Guid,dwInterfaceVer);
}

//////////////////////////////////////////////////////////////////////////
