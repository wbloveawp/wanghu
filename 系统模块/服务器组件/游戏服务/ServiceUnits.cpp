#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//宏定义
#define					IDI_DELAY_CLOSE				1					//延迟关闭

//静态变量
CServiceUnits *			CServiceUnits::g_pServiceUnits=NULL;			//对象指针

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServiceUnits, CWnd)
	ON_MESSAGE(WM_UICONTROL_REQUEST,OnUIControlRequest)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CServiceUnits::CServiceUnits()
{
	//设置变量
	m_bForceClose = false;
	m_ServiceStatus=SERVICE_STATUS_STOP;

	//组件配置
	ZeroMemory(&m_GameServiceAttrib,sizeof(m_GameServiceAttrib));
	ZeroMemory(&m_GameServiceOption,sizeof(m_GameServiceOption));

	//设置接口
	m_pIServiceUnitsSink=NULL;

	//设置接口
	m_MonitorClient.SetMonitorClientSink(this);

	//初始化
	m_MonitorClient.InitializeClient();

	//设置对象
	ASSERT(g_pServiceUnits==NULL);
	if (g_pServiceUnits==NULL) g_pServiceUnits=this;

	return;
}

//析构函数
CServiceUnits::~CServiceUnits()
{
	ConcludeService();
}

//接口查询
VOID * CServiceUnits::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceUnits,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceUnits,Guid,dwQueryVer);	
	return NULL;
}

//启动服务
bool CServiceUnits::StartService()
{
	//效验状态
	ASSERT(m_ServiceStatus==SERVICE_STATUS_STOP);
	if (m_ServiceStatus!=SERVICE_STATUS_STOP) return false;

	//设置变量
	m_bForceClose = false;

	//设置状态
	SetServiceStatus(SERVICE_STATUS_CONFIG);

	//创建窗口
	if (m_hWnd==NULL)
	{
		//创建窗口
		CRect rcCreate(0,0,0,0);
		Create(NULL,NULL,WS_CHILD,rcCreate,AfxGetMainWnd(),100);

		//设置接口
		m_UIControlSink.SetHookWindow(m_hWnd);
	}

	//监控终端
	if (m_MonitorClient.StartService() == false)
	{
		ConcludeService();
		return false;
	}

	//创建模块
	if (CreateServiceDLL()==false)
	{
		ConcludeService();
		return false;
	}

	//调整参数
	if (RectifyServiceParameter()==false)
	{
		ConcludeService();
		return false;
	}

	//配置服务
	if (InitializeService()==false)
	{
		ConcludeService();
		return false;
	}

	//启动数据库
	if (StartDataBaseService() == false)
	{
		ConcludeService();
		return false;
	}

	//启动内核
	if (StartKernelService()==false)
	{
		ConcludeService();
		return false;
	}		

	//启动服务
	if (m_GameServiceManager->StartService() == false)
	{
		CTraceService::TraceString(TEXT("游戏模块启动失败！"), TraceLevel_Exception);
		ConcludeService();
		return false;
	}

	//加载配置
	SendControlPacket(CT_LOAD_SERVICE_CONFIG,NULL,0);

	return true;
}

//调试服务
bool CServiceUnits::DebugService()
{
	//效验状态
	//ASSERT(m_ServiceStatus == SERVICE_STATUS_SERVICE);
	if (m_ServiceStatus != SERVICE_STATUS_SERVICE) return false;

	//调试服务
	SendControlPacket(CT_DEBUG_SERVICE, NULL, 0);

	return true;
}

//停止服务
bool CServiceUnits::ConcludeService()
{
	if(m_ServiceStatus!=SERVICE_STATUS_WILLSTOP && m_ServiceStatus!=SERVICE_STATUS_STOP)
	{
		//统计在线
		DWORD dwOnlineCount = m_AttemperEngineSink.m_ServerUserManager.GetUserItemCount();

		bool bQuery=m_bForceClose;
		for (int i = 0; i < m_AttemperEngineSink.m_TableFrameArray.GetCount(); i++)
		{
			CTableFrameItem* pTable = m_AttemperEngineSink.m_TableFrameArray[i];
			if (pTable != NULL && pTable->IsGameStarted())
			{
				if (!bQuery && AfxMessageBox(TEXT("当前还有玩家正在游戏中，是否强制关闭房间？"), MB_YESNO) != IDYES)
				{
					if (AfxMessageBox(TEXT("是否设置禁止用户进入游戏？"), MB_YESNO) != IDYES)
					{
						g_GlobalUnits.SetAllowEnterGame(true);
					}
					else
					{
						g_GlobalUnits.SetAllowEnterGame(false);
					}
					return false;
				}
				bQuery = true;
				pTable->DismissGame();
			}
		}		
			
		LPCTSTR pszKickMessage = CWHService::FormatMessage(MIM_SERVER_0119, TEXT("抱歉，由于此房间即将关闭，为了保障您的帐号安全，服务器主动与您断开连接！"));
		
		//变量定义
		WORD wEnumIndex=0;
		WORD wKickCount=0;
		IServerUserItem * pITargerUserItem=NULL;

		do
		{
			pITargerUserItem=m_AttemperEngineSink.m_ServerUserManager.EnumUserItem(wEnumIndex++);
			if (pITargerUserItem == NULL) break;

			//统计数量
			wKickCount++;

			//发送消息			
			m_AttemperEngineSink.SendRoomMessage(pITargerUserItem, pszKickMessage,SMT_CHAT | SMT_EJECT | SMT_CLOSE_LINK | SMT_CLOSE_ROOM | SMT_CLOSE_GAME);
			
		} while (true);		

		if(dwOnlineCount>0)
		{
			SetTimer(IDI_DELAY_CLOSE,__max(dwOnlineCount*5, wKickCount*15),NULL);
			SetServiceStatus(SERVICE_STATUS_WILLSTOP);
			return false;
		}		
	}

	SetServiceStatus(SERVICE_STATUS_STOP);

	//停止服务
	m_MonitorClient.ConcludeService();

	//内核引擎
	if (m_TimerEngine.GetInterface() != NULL) m_TimerEngine->ConcludeService();		
	if (m_AttemperEngine.GetInterface() != NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface() != NULL) m_TCPNetworkEngine->ConcludeService();

	//数据库引擎
	if (m_DataBaseEngine.GetInterface()!=NULL) m_DataBaseEngine->ConcludeService();			

	//网络引擎
	if (m_TCPSocketStore.GetInterface() != NULL) m_TCPSocketStore->ConcludeService();
	if (m_TCPSocketService.GetInterface() != NULL) m_TCPSocketService->ConcludeService();
	if (m_TCPSocketCenter.GetInterface() != NULL) m_TCPSocketCenter->ConcludeService();

	//停止服务
#ifdef VIDEO_MODULE_HELPER
	if (m_GameVideoManager.GetInterface() != NULL) m_GameVideoManager.CloseInstance();
#endif
#ifdef MATCH_MODULE_HELPER
	if (m_MatchServiceManager.GetInterface() != NULL) m_MatchServiceManager.CloseInstance();
#endif
#ifdef BATTLE_MODULE_HELPER
	if (m_BattleServiceManager.GetInterface() != NULL) m_BattleServiceManager.CloseInstance();
#endif
#ifdef RANKING_MODULE_HELPER
	if (m_RankingServiceManager.GetInterface() != NULL) m_RankingServiceManager.CloseInstance();
#endif

	//停止服务
	if (m_GameServiceManager.GetInterface() != NULL)
	{
		m_GameServiceManager->ConcludeService();
		m_GameServiceManager.CloseInstance();
	}

	return true;
}

//设置接口
bool CServiceUnits::SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink)
{
	//设置变量
	m_pIServiceUnitsSink=pIServiceUnitsSink;

	return true;
}

//游戏配置
bool CServiceUnits::SetServiceCollocate(LPCTSTR pszGameModule, const tagGameServiceOption & GameServiceOption)
{
	//效验状态
	ASSERT(m_ServiceStatus==SERVICE_STATUS_STOP);
	if (m_ServiceStatus!=SERVICE_STATUS_STOP) return false;

	//配置模块
	m_GameServiceOption=GameServiceOption;
	m_GameServiceManager.SetModuleCreateInfo(pszGameModule,GAME_SERVICE_CREATE_NAME);

	//设置参数
	m_MonitorClient.SetServiceParameter(SERVICE_MODULE_GAME, m_GameServiceOption.wServerID, m_GameServiceOption.szServerName);

	return true;
}

//创建模块
bool CServiceUnits::CreateServiceDLL()
{
	//时间引擎
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TimerEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//调度引擎
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_AttemperEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//网络引擎
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TCPNetworkEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//网络组件
	if ((m_TCPSocketStore.GetInterface()==NULL)&&(m_TCPSocketStore.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TCPSocketStore.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//网络组件
	if ((m_TCPSocketService.GetInterface()==NULL)&&(m_TCPSocketService.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TCPSocketService.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//网络组件
	if ((m_TCPSocketCenter.GetInterface()==NULL)&&(m_TCPSocketCenter.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TCPSocketCenter.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//数据组件
	if ((m_DataBaseEngine.GetInterface() == NULL) && (m_DataBaseEngine.CreateInstance() == false))
	{
		CTraceService::TraceString(m_DataBaseEngine.GetErrorDescribe(), TraceLevel_Exception);
		return false;
	}

	//游戏模块
	if ((m_GameServiceManager.GetInterface()==NULL)&&(m_GameServiceManager.CreateInstance()==false))
	{
		CTraceService::TraceString(m_GameServiceManager.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//录像模块		
	if (CServerRule::IsRecordGameVideo(m_GameServiceOption.dwServerRule))
	{
#ifdef VIDEO_MODULE_HELPER	
		if ((m_GameVideoManager.GetInterface() == NULL) && (m_GameVideoManager.CreateInstance() == false))
		{
			CTraceService::TraceString(m_GameVideoManager.GetErrorDescribe(), TraceLevel_Exception);
			return false;
		}
	}
#endif

	//比赛模块
	if ((m_GameServiceOption.wServerType&GAME_GENRE_MATCH)!=0)
	{
#ifdef  MATCH_MODULE_HELPER

		if ((m_MatchServiceManager.GetInterface()==NULL)&&(m_MatchServiceManager.CreateInstance()==false))
		{
			CTraceService::TraceString(m_MatchServiceManager.GetErrorDescribe(),TraceLevel_Exception);
			return false;
		}
#endif
	}

	//约战模块
	if ((m_GameServiceOption.wServerType & GAME_GENRE_BATTLE) != 0)
	{
#ifdef  BATTLE_MODULE_HELPER

		if ((m_BattleServiceManager.GetInterface() == NULL) && (m_BattleServiceManager.CreateInstance() == false))
		{
			CTraceService::TraceString(m_BattleServiceManager.GetErrorDescribe(), TraceLevel_Exception);
			return false;
		}
#endif
	}

	//排位模块
	if ((m_GameServiceOption.wServerType & GAME_GENRE_RANKING) != 0)
	{
#ifdef  RANKING_MODULE_HELPER

		if ((m_RankingServiceManager.GetInterface() == NULL) && (m_RankingServiceManager.CreateInstance() == false))
		{
			CTraceService::TraceString(m_RankingServiceManager.GetErrorDescribe(), TraceLevel_Exception);
			return false;
		}
#endif
	}

	return true;
}

//配置组件
bool CServiceUnits::InitializeService()
{
	//组件接口
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	IUnknownEx * pITCPSocketStore=m_TCPSocketStore.GetInterface();	
	IUnknownEx * pITCPSocketService=m_TCPSocketService.GetInterface();
	IUnknownEx * pITCPSocketCenterServer=m_TCPSocketCenter.GetInterface();

	//数据接口
	IUnknownEx * pIDataBaseEngineSink[CountArray(m_DataBaseEngineSink)];
	for (WORD i=0;i<CountArray(pIDataBaseEngineSink);i++)
	{
		pIDataBaseEngineSink[i]=QUERY_OBJECT_INTERFACE(m_DataBaseEngineSink[i],IUnknownEx);
	}

	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//绑定接口
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false;
	if (m_DataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineSink, CountArray(pIDataBaseEngineSink)) == false) return false;
	
	//查询接口
	IGameServiceCustomTime * pIGameServiceSustomTime=QUERY_OBJECT_PTR_INTERFACE(m_GameServiceManager.GetInterface(),IGameServiceCustomTime);

	//时间引擎
	if (pIGameServiceSustomTime!=NULL)
	{
		//单元时间
		DWORD dwTimeCell=dwTimeCell=pIGameServiceSustomTime->GetTimerEngineTimeCell();
		if (dwTimeCell>TIME_CELL) dwTimeCell=TIME_CELL;

		//设置时间
		m_TimerEngine->SetTimeCell(dwTimeCell);
	}		

	//内核组件
	if (m_TimerEngine->SetTimerEngineEvent(pIAttemperEngine)==false) return false;
	if (m_AttemperEngine->SetNetworkEngine(pITCPNetworkEngine)==false) return false;
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return false;

	//网络服务
	m_TCPSocketStore->SetServiceID(NETWORK_STORE);
	m_TCPSocketCenter->SetServiceID(NETWORK_CENTER);
	m_TCPSocketService->SetServiceID(NETWORK_SERVICE);
	if (m_TCPSocketStore->SetTCPSocketEvent(pIAttemperEngine)==false) return false;	
	if (m_TCPSocketService->SetTCPSocketEvent(pIAttemperEngine)==false) return false;
	if (m_TCPSocketCenter->SetTCPSocketEvent(pIAttemperEngine)==false) return false;	

	//设置回调
	m_DataBaseEngine->SetDBExceptionHookFun(&CMonitorClient::DBExceptionHookFun);

	//调度回调
	m_AttemperEngineSink.m_pInitParameter=&m_InitParameter;
	m_AttemperEngineSink.m_pGameServiceAttrib=&m_GameServiceAttrib;
	m_AttemperEngineSink.m_pGameServiceOption=&m_GameServiceOption;
	m_AttemperEngineSink.m_pITimerEngine=m_TimerEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPSocketStore=m_TCPSocketStore.GetInterface();
	m_AttemperEngineSink.m_pITCPSocketService=m_TCPSocketService.GetInterface();
	m_AttemperEngineSink.m_pITCPSocketCenter=m_TCPSocketCenter.GetInterface();	
	m_AttemperEngineSink.m_pIGameServiceSustomTime=pIGameServiceSustomTime;
	m_AttemperEngineSink.m_pIGameServiceManager=m_GameServiceManager.GetInterface();
	m_AttemperEngineSink.m_pIDataBaseEngine= m_DataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pIAttemperEngine = m_AttemperEngine.GetInterface();

	//录像模块
#ifdef  VIDEO_MODULE_HELPER
	m_AttemperEngineSink.m_pIGameVideoManager = m_GameVideoManager.GetInterface();
#endif	

	//比赛模块
	if ((m_GameServiceOption.wServerType & GAME_GENRE_MATCH) != 0)
	{
#ifdef  MATCH_MODULE_HELPER
		m_AttemperEngineSink.m_pIExtendServiceManager = m_MatchServiceManager.GetInterface();
#endif
	}

	//约战模块
	if ((m_GameServiceOption.wServerType & GAME_GENRE_BATTLE) != 0)
	{
#ifdef  BATTLE_MODULE_HELPER
		m_AttemperEngineSink.m_pIExtendServiceManager = m_BattleServiceManager.GetInterface();
#endif
	}

	//排位模块
	if ((m_GameServiceOption.wServerType & GAME_GENRE_RANKING) != 0)
	{
#ifdef  RANKING_MODULE_HELPER
		m_AttemperEngineSink.m_pIExtendServiceManager = m_RankingServiceManager.GetInterface();		
#endif
	}

	//数据库回调
	for (WORD i=0;i<CountArray(m_DataBaseEngineSink);i++)
	{
		m_DataBaseEngineSink[i].m_pInitParameter=&m_InitParameter;
		m_DataBaseEngineSink[i].m_pGameServiceAttrib=&m_GameServiceAttrib;
		m_DataBaseEngineSink[i].m_pGameServiceOption=&m_GameServiceOption;
		m_DataBaseEngineSink[i].m_pITimerEngine = m_TimerEngine.GetInterface();
		m_DataBaseEngineSink[i].m_pIDataBaseEngine = m_DataBaseEngine.GetInterface();		
		m_DataBaseEngineSink[i].m_pIDataBaseEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIAttemperEngine,IDataBaseEngineEvent);
		m_DataBaseEngineSink[i].m_pAttemperEngineSink = &m_AttemperEngineSink;

		//数据钩子
		m_DataBaseEngineSink[i].m_pIGameDataBaseSink = (IGameDataBaseSink*)m_GameServiceManager->CreateDataBaseEngineSink(IID_IGameDataBaseSink, VER_IGameDataBaseSink);

		//服务钩子
		if (m_AttemperEngineSink.m_pIExtendServiceManager)
		{
			m_DataBaseEngineSink[i].m_pIExtendDataBaseSink = (IExtendDataBaseSink*)m_AttemperEngineSink.m_pIExtendServiceManager->CreateExtendDataBaseSink(IID_IExtendDataBaseSink,VER_IExtendDataBaseSink);
		}
	}

	//配置网络
	WORD wMaxConnect=m_GameServiceOption.wMaxPlayer;
	WORD wServicePort=m_GameServiceOption.wServerPort;
	if (m_TCPNetworkEngine->SetServiceParameter(wServicePort,wMaxConnect,szCompilation)==false) return false;

	return true;
}

//调整参数
bool CServiceUnits::RectifyServiceParameter()
{
	//加载参数
	m_InitParameter.LoadInitParameter();

	//读取属性
	m_GameServiceManager->GetGameServiceAttrib(m_GameServiceAttrib);
	if (lstrcmp(m_GameServiceAttrib.szServerDLLName,m_GameServiceManager.m_szModuleDllName)!=0)
	{
		CTraceService::TraceString(TEXT("游戏模块的绑定 DLL 名字与配置模块 DLL 名字不一致"),TraceLevel_Exception);
		return false;
	}

	//调整参数
	if (m_GameServiceManager->RectifyOptionParameter(m_GameServiceOption)==false)
	{
		CTraceService::TraceString(TEXT("游戏模块调整配置参数失败"),TraceLevel_Exception);
		return false;
	}

	//积分上限
	if (m_GameServiceOption.lRestrictScore!=0L)
	{
		m_GameServiceOption.lRestrictScore=__max(m_GameServiceOption.lRestrictScore,m_GameServiceOption.lLessScore);
	}

	//服务类型
	if ((m_GameServiceOption.wServerType&m_GameServiceAttrib.wSupporType)==0)
	{
		CTraceService::TraceString(TEXT("游戏模块不支持配置中指定的房间模式类型"),TraceLevel_Exception);
		return false;
	}

	//最大人数
	WORD wMaxPlayer=m_GameServiceOption.wTableCount*m_GameServiceAttrib.wChairCount;
	m_GameServiceOption.wMaxPlayer=__max(m_GameServiceOption.wMaxPlayer,wMaxPlayer);

	//挂接设置
	if (m_GameServiceOption.wSortID==0) m_GameServiceOption.wSortID=500;
	if (m_GameServiceOption.wKindID==0) m_GameServiceOption.wKindID=m_GameServiceAttrib.wKindID;

	//游戏记录
	if (m_GameServiceOption.wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))
	{
		CServerRule::SetRecordGameScore(m_GameServiceOption.dwServerRule,true);
	}

	//最低进入积分
	if (m_GameServiceOption.wServerType&(GAME_GENRE_SCORE| GAME_GENRE_BATTLE))
	{
		m_GameServiceOption.lLessScore = 0;
	}

	//服务地址
	if (m_GameServiceOption.szServiceAddr[0]==0)
	{
		//转化地址
		TCHAR szClientIP[16]=TEXT("");
 		BYTE * pClientIP=(BYTE *)&m_InitParameter.m_ServiceAddress.dwServiceAddr;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);
	
		//拷贝地址
		StringCchCopy(m_GameServiceOption.szServiceAddr,CountArray(m_GameServiceOption.szServiceDomain), szClientIP);
	}
	
	//服务域名
	if (m_GameServiceOption.szServiceDomain[0]==0)
	{
		StringCchCopy(m_GameServiceOption.szServiceDomain,CountArray(m_GameServiceOption.szServiceDomain), m_InitParameter.m_ServiceAddress.szServiceDomain);
	}	

	//比赛房间
	if (m_GameServiceOption.wServerType&GAME_GENRE_MATCH)
	{
		//游戏记录
		CServerRule::SetRecordGameScore(m_GameServiceOption.dwServerRule,true);

		//即时写分
		CServerRule::SetImmediateWriteScore(m_GameServiceOption.dwServerRule,true);
	}

	//约战房间
	if (m_GameServiceOption.wServerType&GAME_GENRE_BATTLE)
	{
		//坐下旁观
		CServerRule::SetSitForLookon(m_GameServiceOption.dwServerRule,true);
	}

	//强制禁止起立
	if ((m_GameServiceOption.wServerType & (GAME_GENRE_BATTLE | GAME_GENRE_RANKING | GAME_GENRE_MATCH)) != 0)
	{
		CServerRule::SetForbidForceStandup(m_GameServiceOption.dwServerRule, true);
	}

	//分组设置
	if (m_GameServiceOption.wServerType&TABLE_GENRE_DISTRIBUTE && m_GameServiceOption.cbDistributeMode==DISTRIBUTE_MODE_DELAYWAIT)
	{
		//分组间隔
		if (m_GameServiceOption.wDistributeTimeSpace==0)
		{
			m_GameServiceOption.wDistributeTimeSpace=10;
		}

		//最小游戏人数
		if (m_GameServiceOption.wMinPartakeGameUser<2)
		{
			m_GameServiceOption.wMinPartakeGameUser=2;
		}

		//最大游戏人数
		if (m_GameServiceOption.wMaxPartakeGameUser>m_GameServiceAttrib.wChairCount)
		{
			m_GameServiceOption.wMinPartakeGameUser=m_GameServiceAttrib.wChairCount;
		}

		//调整人数
		if (m_GameServiceOption.wMaxPartakeGameUser<m_GameServiceOption.wMinPartakeGameUser)
		{
			m_GameServiceOption.wMaxPartakeGameUser=m_GameServiceOption.wMinPartakeGameUser;
		}

		//最小分组人数
		if (m_GameServiceOption.wMinDistributeUser<m_GameServiceOption.wMinPartakeGameUser)
		{
			m_GameServiceOption.wMinDistributeUser=m_GameServiceOption.wMinPartakeGameUser;
		}
	}	

	//房间禁止公聊
	bool bAllow = CServerRule::IsForbidRoomChat(m_GameServiceOption.dwServerRule)?false:true;
	g_GlobalUnits.SetAllowRoomChat( bAllow );

	//房间禁止私聊
	bAllow = CServerRule::IsForbidRoomWisper(m_GameServiceOption.dwServerRule)?false:true;
	g_GlobalUnits.SetAllowWisper( bAllow );

	//游戏禁止公聊
	bAllow = CServerRule::IsForbidGameChat(m_GameServiceOption.dwServerRule)?false:true;
	g_GlobalUnits.SetAllowGameChat( bAllow );

	//同桌禁止私聊
	bool bForbid = CServerRule::IsForbidPlayChat(m_GameServiceOption.dwServerRule);
	g_GlobalUnits.SetForbidPlayChat( bForbid );

	//允许机器人陪打
	bAllow = CServerRule::IsAllowRobotAttend(m_GameServiceOption.dwServerRule);
	g_GlobalUnits.SetAllowRobotAttend(bAllow);

	//允许机器人占座
	bAllow = CServerRule::IsAllowRobotSimulate(m_GameServiceOption.dwServerRule);
	g_GlobalUnits.SetAllowRobotSimulate(bAllow);	

	//会员可聊天
	bool bMemCanChat = CServerRule::IsMemberCanChat(m_GameServiceOption.dwServerRule);
	g_GlobalUnits.SetMemberCanChat( bMemCanChat );

	//禁止旁观
	bool bForbidLookon = CServerRule::IsForbidLookon(m_GameServiceOption.dwServerRule);
	g_GlobalUnits.SetForbidLookon( bForbidLookon );

	return true;
}

//启动内核
bool CServiceUnits::StartKernelService()
{
	//时间引擎
	if (m_TimerEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//调度引擎
	if (m_AttemperEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//网络引擎
	if (m_TCPSocketCenter->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}	

	//启动服务
	if (StartSocketService() == false)
	{
		return false;
	}

	//启动服务
	if (StartExtendService() == false)
	{
		return false;
	}		

	return true;
}

//启动网络
bool CServiceUnits::StartNetworkService()
{
	//网络引擎
	if (m_TCPNetworkEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//启动网络
bool CServiceUnits::StartSocketService()
{
	//网络引擎
	if (CServerRule::IsRecordGameVideo(m_GameServiceOption.dwServerRule) && m_TCPSocketStore->StartService() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//网络引擎
	if (m_GameServiceOption.wServerType&(GAME_GENRE_MATCH| GAME_GENRE_BATTLE | GAME_GENRE_RANKING ) && m_TCPSocketService->StartService() == false)
	{
		ASSERT(FALSE);
		return false;
	}	

	return true;
}

//启动服务
bool CServiceUnits::StartExtendService()
{
	//启动服务
#ifdef MATCH_MODULE_HELPER
	if (m_MatchServiceManager.GetInterface() != NULL && m_MatchServiceManager->StartService() == false)
	{
		ASSERT(FALSE);
		return false;
	}
#endif

	//启动服务
#ifdef BATTLE_MODULE_HELPER
	if (m_BattleServiceManager.GetInterface() != NULL && m_BattleServiceManager->StartService() == false)
	{
		ASSERT(FALSE);
		return false;
	}
#endif

	//启动服务
#ifdef RANKING_MODULE_HELPER
	if (m_RankingServiceManager.GetInterface() != NULL && m_RankingServiceManager->StartService() == false)
	{
		ASSERT(FALSE);
		return false;
	}
#endif

	return true;
}

//启动数据库
bool CServiceUnits::StartDataBaseService()
{
	//内核数据库
	if (m_DataBaseEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//设置状态
bool CServiceUnits::SetServiceStatus(WORD ServiceStatus)
{
	//状态判断
	if (m_ServiceStatus!=ServiceStatus)
	{
		//错误通知
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE && m_ServiceStatus!=SERVICE_STATUS_WILLSTOP)&&(ServiceStatus==SERVICE_STATUS_STOP))
		{
			CTraceService::TraceString(TEXT("服务启动失败"),TraceLevel_Exception);
		}

		//设置变量
		m_ServiceStatus=ServiceStatus;

		//更新状态
		m_MonitorClient.UpdateServiceStatus(ServiceStatus);

		//状态通知
		ASSERT(m_pIServiceUnitsSink!=NULL);
		if (m_pIServiceUnitsSink!=NULL) m_pIServiceUnitsSink->OnServiceUnitsStatus(m_ServiceStatus);	
	}

	return true;
}

//发送控制
bool CServiceUnits::SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize)
{
	//状态效验
	ASSERT(m_AttemperEngine.GetInterface()!=NULL);
	if (m_AttemperEngine.GetInterface()==NULL) return false;

	//发送控制
	m_AttemperEngine->OnEventControl(wControlID,pData,wDataSize);

	return true;
}

//控制消息
LRESULT CServiceUnits::OnUIControlRequest(WPARAM wParam, LPARAM lParam)
{
	//获取数据
	tagDataHead DataHead;
	VOID * pData=m_UIControlSink.GetControlRequest(DataHead);

	//数据处理
	switch (DataHead.dwIdentifier)
	{	
	case UI_CENTER_RESULT:			//中心结果
	{
		//效验消息
		ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
		if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

		//变量定义
		CP_ControlResult * pControlResult=(CP_ControlResult *)pData;

		//失败处理
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_FAILURE))
		{
			ConcludeService();
			return 0;
		}

		//成功处理
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_SUCCESS))
		{
			//启动网络
			if (StartNetworkService() == false)
			{
				ConcludeService();
				return 0;
			}

			//开启录像
			if (CServerRule::IsRecordGameVideo(m_GameServiceOption.dwServerRule))
			{
				//加载配置
				SendControlPacket(CT_CONNECT_STORE, NULL, 0);
			}
			//类型服务
			else if ((m_GameServiceOption.wServerType & (GAME_GENRE_MATCH| GAME_GENRE_BATTLE| GAME_GENRE_RANKING)) != 0)
			{
				//加载配置
				SendControlPacket(CT_CONNECT_SERVICE, NULL, 0);
			}			
			else
			{
				//设置状态
				SetServiceStatus(SERVICE_STATUS_SERVICE);
			}
		}

		return 0;
	}	
	case UI_SERVICE_RESULT:			//服务结果
	{
		//效验消息
		ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
		if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

		//变量定义
		CP_ControlResult * pControlResult=(CP_ControlResult *)pData;

		//失败处理
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_FAILURE))
		{
			ConcludeService();
			return 0;
		}

		//成功处理
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_SUCCESS))
		{
			//设置状态
			SetServiceStatus(SERVICE_STATUS_SERVICE);
		}

		return 0;
	}
	case UI_STORE_RESULT:			//存储结果
	{
		//效验消息
		ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
		if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

		//变量定义
		CP_ControlResult * pControlResult=(CP_ControlResult *)pData;

		//失败处理
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_FAILURE))
		{
			ConcludeService();
			return 0;
		}

		//成功处理
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_SUCCESS))
		{
			//类型服务
			if ((m_GameServiceOption.wServerType & (GAME_GENRE_MATCH | GAME_GENRE_BATTLE | GAME_GENRE_RANKING)) != 0)
			{
				//加载配置
				SendControlPacket(CT_CONNECT_SERVICE, NULL, 0);
			}
			else
			{
				//设置状态
				SetServiceStatus(SERVICE_STATUS_SERVICE);				
			}
		}

		return 0;
	}	
	case UI_SERVICE_CONFIG_RESULT:	//配置结果
	{
		//效验消息
		ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
		if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

		//变量定义
		CP_ControlResult * pControlResult=(CP_ControlResult *)pData;

		//失败处理
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_FAILURE))
		{
			ConcludeService();
			return 0;
		}

		//成功处理
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_SUCCESS))
		{
			//连接中心
			SendControlPacket(CT_CONNECT_CENTER, NULL, 0);
		}

		return 0;
	}
	case UI_SERVICE_WILLSTOP_RESULT: //预停止结果
	{
		//效验消息
		ASSERT(DataHead.wDataSize == sizeof(CP_ControlResult));
		if (DataHead.wDataSize != sizeof(CP_ControlResult)) return 0L;

		//变量定义
		CP_ControlResult* pControlResult = (CP_ControlResult*)pData;

		//成功处理
		if ((m_ServiceStatus == SERVICE_STATUS_WILLSTOP) && (pControlResult->cbSuccess == ER_SUCCESS))
		{
			//停止服务
			ConcludeService();
			return 0L;
		}

		//失败处理
		if ((m_ServiceStatus == SERVICE_STATUS_WILLSTOP) && (pControlResult->cbSuccess == ER_FAILURE))
		{
			//提示信息
			CTraceService::TraceString(TEXT("服务停止失败！"), TraceLevel_Warning);

			//设置状态
			SetServiceStatus(SERVICE_STATUS_SERVICE);
			return 0L;
		}

		return 0L;
	}
	}

	return 0;
}

void CServiceUnits::OnTimer(UINT_PTR nIDEvent)
{
	if(nIDEvent==IDI_DELAY_CLOSE)
	{
		KillTimer(IDI_DELAY_CLOSE);
		ConcludeService();
	}

	__super::OnTimer(nIDEvent);
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(ServiceUnits);

//////////////////////////////////////////////////////////////////////////


