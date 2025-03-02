#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//�궨��
#define					IDI_DELAY_CLOSE				1					//�ӳٹر�

//��̬����
CServiceUnits *			CServiceUnits::g_pServiceUnits=NULL;			//����ָ��

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServiceUnits, CWnd)
	ON_MESSAGE(WM_UICONTROL_REQUEST,OnUIControlRequest)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CServiceUnits::CServiceUnits()
{
	//���ñ���
	m_bForceClose = false;
	m_ServiceStatus=SERVICE_STATUS_STOP;

	//�������
	ZeroMemory(&m_GameServiceAttrib,sizeof(m_GameServiceAttrib));
	ZeroMemory(&m_GameServiceOption,sizeof(m_GameServiceOption));

	//���ýӿ�
	m_pIServiceUnitsSink=NULL;

	//���ýӿ�
	m_MonitorClient.SetMonitorClientSink(this);

	//��ʼ��
	m_MonitorClient.InitializeClient();

	//���ö���
	ASSERT(g_pServiceUnits==NULL);
	if (g_pServiceUnits==NULL) g_pServiceUnits=this;

	return;
}

//��������
CServiceUnits::~CServiceUnits()
{
	ConcludeService();
}

//�ӿڲ�ѯ
VOID * CServiceUnits::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGameServiceUnits,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IGameServiceUnits,Guid,dwQueryVer);	
	return NULL;
}

//��������
bool CServiceUnits::StartService()
{
	//Ч��״̬
	ASSERT(m_ServiceStatus==SERVICE_STATUS_STOP);
	if (m_ServiceStatus!=SERVICE_STATUS_STOP) return false;

	//���ñ���
	m_bForceClose = false;

	//����״̬
	SetServiceStatus(SERVICE_STATUS_CONFIG);

	//��������
	if (m_hWnd==NULL)
	{
		//��������
		CRect rcCreate(0,0,0,0);
		Create(NULL,NULL,WS_CHILD,rcCreate,AfxGetMainWnd(),100);

		//���ýӿ�
		m_UIControlSink.SetHookWindow(m_hWnd);
	}

	//����ն�
	if (m_MonitorClient.StartService() == false)
	{
		ConcludeService();
		return false;
	}

	//����ģ��
	if (CreateServiceDLL()==false)
	{
		ConcludeService();
		return false;
	}

	//��������
	if (RectifyServiceParameter()==false)
	{
		ConcludeService();
		return false;
	}

	//���÷���
	if (InitializeService()==false)
	{
		ConcludeService();
		return false;
	}

	//�������ݿ�
	if (StartDataBaseService() == false)
	{
		ConcludeService();
		return false;
	}

	//�����ں�
	if (StartKernelService()==false)
	{
		ConcludeService();
		return false;
	}		

	//��������
	if (m_GameServiceManager->StartService() == false)
	{
		CTraceService::TraceString(TEXT("��Ϸģ������ʧ�ܣ�"), TraceLevel_Exception);
		ConcludeService();
		return false;
	}

	//��������
	SendControlPacket(CT_LOAD_SERVICE_CONFIG,NULL,0);

	return true;
}

//���Է���
bool CServiceUnits::DebugService()
{
	//Ч��״̬
	//ASSERT(m_ServiceStatus == SERVICE_STATUS_SERVICE);
	if (m_ServiceStatus != SERVICE_STATUS_SERVICE) return false;

	//���Է���
	SendControlPacket(CT_DEBUG_SERVICE, NULL, 0);

	return true;
}

//ֹͣ����
bool CServiceUnits::ConcludeService()
{
	if(m_ServiceStatus!=SERVICE_STATUS_WILLSTOP && m_ServiceStatus!=SERVICE_STATUS_STOP)
	{
		//ͳ������
		DWORD dwOnlineCount = m_AttemperEngineSink.m_ServerUserManager.GetUserItemCount();

		bool bQuery=m_bForceClose;
		for (int i = 0; i < m_AttemperEngineSink.m_TableFrameArray.GetCount(); i++)
		{
			CTableFrameItem* pTable = m_AttemperEngineSink.m_TableFrameArray[i];
			if (pTable != NULL && pTable->IsGameStarted())
			{
				if (!bQuery && AfxMessageBox(TEXT("��ǰ�������������Ϸ�У��Ƿ�ǿ�ƹرշ��䣿"), MB_YESNO) != IDYES)
				{
					if (AfxMessageBox(TEXT("�Ƿ����ý�ֹ�û�������Ϸ��"), MB_YESNO) != IDYES)
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
			
		LPCTSTR pszKickMessage = CWHService::FormatMessage(MIM_SERVER_0119, TEXT("��Ǹ�����ڴ˷��伴���رգ�Ϊ�˱��������ʺŰ�ȫ�����������������Ͽ����ӣ�"));
		
		//��������
		WORD wEnumIndex=0;
		WORD wKickCount=0;
		IServerUserItem * pITargerUserItem=NULL;

		do
		{
			pITargerUserItem=m_AttemperEngineSink.m_ServerUserManager.EnumUserItem(wEnumIndex++);
			if (pITargerUserItem == NULL) break;

			//ͳ������
			wKickCount++;

			//������Ϣ			
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

	//ֹͣ����
	m_MonitorClient.ConcludeService();

	//�ں�����
	if (m_TimerEngine.GetInterface() != NULL) m_TimerEngine->ConcludeService();		
	if (m_AttemperEngine.GetInterface() != NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface() != NULL) m_TCPNetworkEngine->ConcludeService();

	//���ݿ�����
	if (m_DataBaseEngine.GetInterface()!=NULL) m_DataBaseEngine->ConcludeService();			

	//��������
	if (m_TCPSocketStore.GetInterface() != NULL) m_TCPSocketStore->ConcludeService();
	if (m_TCPSocketService.GetInterface() != NULL) m_TCPSocketService->ConcludeService();
	if (m_TCPSocketCenter.GetInterface() != NULL) m_TCPSocketCenter->ConcludeService();

	//ֹͣ����
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

	//ֹͣ����
	if (m_GameServiceManager.GetInterface() != NULL)
	{
		m_GameServiceManager->ConcludeService();
		m_GameServiceManager.CloseInstance();
	}

	return true;
}

//���ýӿ�
bool CServiceUnits::SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink)
{
	//���ñ���
	m_pIServiceUnitsSink=pIServiceUnitsSink;

	return true;
}

//��Ϸ����
bool CServiceUnits::SetServiceCollocate(LPCTSTR pszGameModule, const tagGameServiceOption & GameServiceOption)
{
	//Ч��״̬
	ASSERT(m_ServiceStatus==SERVICE_STATUS_STOP);
	if (m_ServiceStatus!=SERVICE_STATUS_STOP) return false;

	//����ģ��
	m_GameServiceOption=GameServiceOption;
	m_GameServiceManager.SetModuleCreateInfo(pszGameModule,GAME_SERVICE_CREATE_NAME);

	//���ò���
	m_MonitorClient.SetServiceParameter(SERVICE_MODULE_GAME, m_GameServiceOption.wServerID, m_GameServiceOption.szServerName);

	return true;
}

//����ģ��
bool CServiceUnits::CreateServiceDLL()
{
	//ʱ������
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TimerEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//��������
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_AttemperEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//��������
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TCPNetworkEngine.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//�������
	if ((m_TCPSocketStore.GetInterface()==NULL)&&(m_TCPSocketStore.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TCPSocketStore.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//�������
	if ((m_TCPSocketService.GetInterface()==NULL)&&(m_TCPSocketService.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TCPSocketService.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//�������
	if ((m_TCPSocketCenter.GetInterface()==NULL)&&(m_TCPSocketCenter.CreateInstance()==false))
	{
		CTraceService::TraceString(m_TCPSocketCenter.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//�������
	if ((m_DataBaseEngine.GetInterface() == NULL) && (m_DataBaseEngine.CreateInstance() == false))
	{
		CTraceService::TraceString(m_DataBaseEngine.GetErrorDescribe(), TraceLevel_Exception);
		return false;
	}

	//��Ϸģ��
	if ((m_GameServiceManager.GetInterface()==NULL)&&(m_GameServiceManager.CreateInstance()==false))
	{
		CTraceService::TraceString(m_GameServiceManager.GetErrorDescribe(),TraceLevel_Exception);
		return false;
	}

	//¼��ģ��		
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

	//����ģ��
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

	//Լսģ��
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

	//��λģ��
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

//�������
bool CServiceUnits::InitializeService()
{
	//����ӿ�
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	IUnknownEx * pITCPSocketStore=m_TCPSocketStore.GetInterface();	
	IUnknownEx * pITCPSocketService=m_TCPSocketService.GetInterface();
	IUnknownEx * pITCPSocketCenterServer=m_TCPSocketCenter.GetInterface();

	//���ݽӿ�
	IUnknownEx * pIDataBaseEngineSink[CountArray(m_DataBaseEngineSink)];
	for (WORD i=0;i<CountArray(pIDataBaseEngineSink);i++)
	{
		pIDataBaseEngineSink[i]=QUERY_OBJECT_INTERFACE(m_DataBaseEngineSink[i],IUnknownEx);
	}

	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//�󶨽ӿ�
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false;
	if (m_DataBaseEngine->SetDataBaseEngineSink(pIDataBaseEngineSink, CountArray(pIDataBaseEngineSink)) == false) return false;
	
	//��ѯ�ӿ�
	IGameServiceCustomTime * pIGameServiceSustomTime=QUERY_OBJECT_PTR_INTERFACE(m_GameServiceManager.GetInterface(),IGameServiceCustomTime);

	//ʱ������
	if (pIGameServiceSustomTime!=NULL)
	{
		//��Ԫʱ��
		DWORD dwTimeCell=dwTimeCell=pIGameServiceSustomTime->GetTimerEngineTimeCell();
		if (dwTimeCell>TIME_CELL) dwTimeCell=TIME_CELL;

		//����ʱ��
		m_TimerEngine->SetTimeCell(dwTimeCell);
	}		

	//�ں����
	if (m_TimerEngine->SetTimerEngineEvent(pIAttemperEngine)==false) return false;
	if (m_AttemperEngine->SetNetworkEngine(pITCPNetworkEngine)==false) return false;
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return false;

	//�������
	m_TCPSocketStore->SetServiceID(NETWORK_STORE);
	m_TCPSocketCenter->SetServiceID(NETWORK_CENTER);
	m_TCPSocketService->SetServiceID(NETWORK_SERVICE);
	if (m_TCPSocketStore->SetTCPSocketEvent(pIAttemperEngine)==false) return false;	
	if (m_TCPSocketService->SetTCPSocketEvent(pIAttemperEngine)==false) return false;
	if (m_TCPSocketCenter->SetTCPSocketEvent(pIAttemperEngine)==false) return false;	

	//���ûص�
	m_DataBaseEngine->SetDBExceptionHookFun(&CMonitorClient::DBExceptionHookFun);

	//���Ȼص�
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

	//¼��ģ��
#ifdef  VIDEO_MODULE_HELPER
	m_AttemperEngineSink.m_pIGameVideoManager = m_GameVideoManager.GetInterface();
#endif	

	//����ģ��
	if ((m_GameServiceOption.wServerType & GAME_GENRE_MATCH) != 0)
	{
#ifdef  MATCH_MODULE_HELPER
		m_AttemperEngineSink.m_pIExtendServiceManager = m_MatchServiceManager.GetInterface();
#endif
	}

	//Լսģ��
	if ((m_GameServiceOption.wServerType & GAME_GENRE_BATTLE) != 0)
	{
#ifdef  BATTLE_MODULE_HELPER
		m_AttemperEngineSink.m_pIExtendServiceManager = m_BattleServiceManager.GetInterface();
#endif
	}

	//��λģ��
	if ((m_GameServiceOption.wServerType & GAME_GENRE_RANKING) != 0)
	{
#ifdef  RANKING_MODULE_HELPER
		m_AttemperEngineSink.m_pIExtendServiceManager = m_RankingServiceManager.GetInterface();		
#endif
	}

	//���ݿ�ص�
	for (WORD i=0;i<CountArray(m_DataBaseEngineSink);i++)
	{
		m_DataBaseEngineSink[i].m_pInitParameter=&m_InitParameter;
		m_DataBaseEngineSink[i].m_pGameServiceAttrib=&m_GameServiceAttrib;
		m_DataBaseEngineSink[i].m_pGameServiceOption=&m_GameServiceOption;
		m_DataBaseEngineSink[i].m_pITimerEngine = m_TimerEngine.GetInterface();
		m_DataBaseEngineSink[i].m_pIDataBaseEngine = m_DataBaseEngine.GetInterface();		
		m_DataBaseEngineSink[i].m_pIDataBaseEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIAttemperEngine,IDataBaseEngineEvent);
		m_DataBaseEngineSink[i].m_pAttemperEngineSink = &m_AttemperEngineSink;

		//���ݹ���
		m_DataBaseEngineSink[i].m_pIGameDataBaseSink = (IGameDataBaseSink*)m_GameServiceManager->CreateDataBaseEngineSink(IID_IGameDataBaseSink, VER_IGameDataBaseSink);

		//������
		if (m_AttemperEngineSink.m_pIExtendServiceManager)
		{
			m_DataBaseEngineSink[i].m_pIExtendDataBaseSink = (IExtendDataBaseSink*)m_AttemperEngineSink.m_pIExtendServiceManager->CreateExtendDataBaseSink(IID_IExtendDataBaseSink,VER_IExtendDataBaseSink);
		}
	}

	//��������
	WORD wMaxConnect=m_GameServiceOption.wMaxPlayer;
	WORD wServicePort=m_GameServiceOption.wServerPort;
	if (m_TCPNetworkEngine->SetServiceParameter(wServicePort,wMaxConnect,szCompilation)==false) return false;

	return true;
}

//��������
bool CServiceUnits::RectifyServiceParameter()
{
	//���ز���
	m_InitParameter.LoadInitParameter();

	//��ȡ����
	m_GameServiceManager->GetGameServiceAttrib(m_GameServiceAttrib);
	if (lstrcmp(m_GameServiceAttrib.szServerDLLName,m_GameServiceManager.m_szModuleDllName)!=0)
	{
		CTraceService::TraceString(TEXT("��Ϸģ��İ� DLL ����������ģ�� DLL ���ֲ�һ��"),TraceLevel_Exception);
		return false;
	}

	//��������
	if (m_GameServiceManager->RectifyOptionParameter(m_GameServiceOption)==false)
	{
		CTraceService::TraceString(TEXT("��Ϸģ��������ò���ʧ��"),TraceLevel_Exception);
		return false;
	}

	//��������
	if (m_GameServiceOption.lRestrictScore!=0L)
	{
		m_GameServiceOption.lRestrictScore=__max(m_GameServiceOption.lRestrictScore,m_GameServiceOption.lLessScore);
	}

	//��������
	if ((m_GameServiceOption.wServerType&m_GameServiceAttrib.wSupporType)==0)
	{
		CTraceService::TraceString(TEXT("��Ϸģ�鲻֧��������ָ���ķ���ģʽ����"),TraceLevel_Exception);
		return false;
	}

	//�������
	WORD wMaxPlayer=m_GameServiceOption.wTableCount*m_GameServiceAttrib.wChairCount;
	m_GameServiceOption.wMaxPlayer=__max(m_GameServiceOption.wMaxPlayer,wMaxPlayer);

	//�ҽ�����
	if (m_GameServiceOption.wSortID==0) m_GameServiceOption.wSortID=500;
	if (m_GameServiceOption.wKindID==0) m_GameServiceOption.wKindID=m_GameServiceAttrib.wKindID;

	//��Ϸ��¼
	if (m_GameServiceOption.wServerType&(GAME_GENRE_GOLD|GAME_GENRE_MATCH))
	{
		CServerRule::SetRecordGameScore(m_GameServiceOption.dwServerRule,true);
	}

	//��ͽ������
	if (m_GameServiceOption.wServerType&(GAME_GENRE_SCORE| GAME_GENRE_BATTLE))
	{
		m_GameServiceOption.lLessScore = 0;
	}

	//�����ַ
	if (m_GameServiceOption.szServiceAddr[0]==0)
	{
		//ת����ַ
		TCHAR szClientIP[16]=TEXT("");
 		BYTE * pClientIP=(BYTE *)&m_InitParameter.m_ServiceAddress.dwServiceAddr;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);
	
		//������ַ
		StringCchCopy(m_GameServiceOption.szServiceAddr,CountArray(m_GameServiceOption.szServiceDomain), szClientIP);
	}
	
	//��������
	if (m_GameServiceOption.szServiceDomain[0]==0)
	{
		StringCchCopy(m_GameServiceOption.szServiceDomain,CountArray(m_GameServiceOption.szServiceDomain), m_InitParameter.m_ServiceAddress.szServiceDomain);
	}	

	//��������
	if (m_GameServiceOption.wServerType&GAME_GENRE_MATCH)
	{
		//��Ϸ��¼
		CServerRule::SetRecordGameScore(m_GameServiceOption.dwServerRule,true);

		//��ʱд��
		CServerRule::SetImmediateWriteScore(m_GameServiceOption.dwServerRule,true);
	}

	//Լս����
	if (m_GameServiceOption.wServerType&GAME_GENRE_BATTLE)
	{
		//�����Թ�
		CServerRule::SetSitForLookon(m_GameServiceOption.dwServerRule,true);
	}

	//ǿ�ƽ�ֹ����
	if ((m_GameServiceOption.wServerType & (GAME_GENRE_BATTLE | GAME_GENRE_RANKING | GAME_GENRE_MATCH)) != 0)
	{
		CServerRule::SetForbidForceStandup(m_GameServiceOption.dwServerRule, true);
	}

	//��������
	if (m_GameServiceOption.wServerType&TABLE_GENRE_DISTRIBUTE && m_GameServiceOption.cbDistributeMode==DISTRIBUTE_MODE_DELAYWAIT)
	{
		//������
		if (m_GameServiceOption.wDistributeTimeSpace==0)
		{
			m_GameServiceOption.wDistributeTimeSpace=10;
		}

		//��С��Ϸ����
		if (m_GameServiceOption.wMinPartakeGameUser<2)
		{
			m_GameServiceOption.wMinPartakeGameUser=2;
		}

		//�����Ϸ����
		if (m_GameServiceOption.wMaxPartakeGameUser>m_GameServiceAttrib.wChairCount)
		{
			m_GameServiceOption.wMinPartakeGameUser=m_GameServiceAttrib.wChairCount;
		}

		//��������
		if (m_GameServiceOption.wMaxPartakeGameUser<m_GameServiceOption.wMinPartakeGameUser)
		{
			m_GameServiceOption.wMaxPartakeGameUser=m_GameServiceOption.wMinPartakeGameUser;
		}

		//��С��������
		if (m_GameServiceOption.wMinDistributeUser<m_GameServiceOption.wMinPartakeGameUser)
		{
			m_GameServiceOption.wMinDistributeUser=m_GameServiceOption.wMinPartakeGameUser;
		}
	}	

	//�����ֹ����
	bool bAllow = CServerRule::IsForbidRoomChat(m_GameServiceOption.dwServerRule)?false:true;
	g_GlobalUnits.SetAllowRoomChat( bAllow );

	//�����ֹ˽��
	bAllow = CServerRule::IsForbidRoomWisper(m_GameServiceOption.dwServerRule)?false:true;
	g_GlobalUnits.SetAllowWisper( bAllow );

	//��Ϸ��ֹ����
	bAllow = CServerRule::IsForbidGameChat(m_GameServiceOption.dwServerRule)?false:true;
	g_GlobalUnits.SetAllowGameChat( bAllow );

	//ͬ����ֹ˽��
	bool bForbid = CServerRule::IsForbidPlayChat(m_GameServiceOption.dwServerRule);
	g_GlobalUnits.SetForbidPlayChat( bForbid );

	//������������
	bAllow = CServerRule::IsAllowRobotAttend(m_GameServiceOption.dwServerRule);
	g_GlobalUnits.SetAllowRobotAttend(bAllow);

	//���������ռ��
	bAllow = CServerRule::IsAllowRobotSimulate(m_GameServiceOption.dwServerRule);
	g_GlobalUnits.SetAllowRobotSimulate(bAllow);	

	//��Ա������
	bool bMemCanChat = CServerRule::IsMemberCanChat(m_GameServiceOption.dwServerRule);
	g_GlobalUnits.SetMemberCanChat( bMemCanChat );

	//��ֹ�Թ�
	bool bForbidLookon = CServerRule::IsForbidLookon(m_GameServiceOption.dwServerRule);
	g_GlobalUnits.SetForbidLookon( bForbidLookon );

	return true;
}

//�����ں�
bool CServiceUnits::StartKernelService()
{
	//ʱ������
	if (m_TimerEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_AttemperEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_TCPSocketCenter->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}	

	//��������
	if (StartSocketService() == false)
	{
		return false;
	}

	//��������
	if (StartExtendService() == false)
	{
		return false;
	}		

	return true;
}

//��������
bool CServiceUnits::StartNetworkService()
{
	//��������
	if (m_TCPNetworkEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//��������
bool CServiceUnits::StartSocketService()
{
	//��������
	if (CServerRule::IsRecordGameVideo(m_GameServiceOption.dwServerRule) && m_TCPSocketStore->StartService() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_GameServiceOption.wServerType&(GAME_GENRE_MATCH| GAME_GENRE_BATTLE | GAME_GENRE_RANKING ) && m_TCPSocketService->StartService() == false)
	{
		ASSERT(FALSE);
		return false;
	}	

	return true;
}

//��������
bool CServiceUnits::StartExtendService()
{
	//��������
#ifdef MATCH_MODULE_HELPER
	if (m_MatchServiceManager.GetInterface() != NULL && m_MatchServiceManager->StartService() == false)
	{
		ASSERT(FALSE);
		return false;
	}
#endif

	//��������
#ifdef BATTLE_MODULE_HELPER
	if (m_BattleServiceManager.GetInterface() != NULL && m_BattleServiceManager->StartService() == false)
	{
		ASSERT(FALSE);
		return false;
	}
#endif

	//��������
#ifdef RANKING_MODULE_HELPER
	if (m_RankingServiceManager.GetInterface() != NULL && m_RankingServiceManager->StartService() == false)
	{
		ASSERT(FALSE);
		return false;
	}
#endif

	return true;
}

//�������ݿ�
bool CServiceUnits::StartDataBaseService()
{
	//�ں����ݿ�
	if (m_DataBaseEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//����״̬
bool CServiceUnits::SetServiceStatus(WORD ServiceStatus)
{
	//״̬�ж�
	if (m_ServiceStatus!=ServiceStatus)
	{
		//����֪ͨ
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE && m_ServiceStatus!=SERVICE_STATUS_WILLSTOP)&&(ServiceStatus==SERVICE_STATUS_STOP))
		{
			CTraceService::TraceString(TEXT("��������ʧ��"),TraceLevel_Exception);
		}

		//���ñ���
		m_ServiceStatus=ServiceStatus;

		//����״̬
		m_MonitorClient.UpdateServiceStatus(ServiceStatus);

		//״̬֪ͨ
		ASSERT(m_pIServiceUnitsSink!=NULL);
		if (m_pIServiceUnitsSink!=NULL) m_pIServiceUnitsSink->OnServiceUnitsStatus(m_ServiceStatus);	
	}

	return true;
}

//���Ϳ���
bool CServiceUnits::SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize)
{
	//״̬Ч��
	ASSERT(m_AttemperEngine.GetInterface()!=NULL);
	if (m_AttemperEngine.GetInterface()==NULL) return false;

	//���Ϳ���
	m_AttemperEngine->OnEventControl(wControlID,pData,wDataSize);

	return true;
}

//������Ϣ
LRESULT CServiceUnits::OnUIControlRequest(WPARAM wParam, LPARAM lParam)
{
	//��ȡ����
	tagDataHead DataHead;
	VOID * pData=m_UIControlSink.GetControlRequest(DataHead);

	//���ݴ���
	switch (DataHead.dwIdentifier)
	{	
	case UI_CENTER_RESULT:			//���Ľ��
	{
		//Ч����Ϣ
		ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
		if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

		//��������
		CP_ControlResult * pControlResult=(CP_ControlResult *)pData;

		//ʧ�ܴ���
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_FAILURE))
		{
			ConcludeService();
			return 0;
		}

		//�ɹ�����
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_SUCCESS))
		{
			//��������
			if (StartNetworkService() == false)
			{
				ConcludeService();
				return 0;
			}

			//����¼��
			if (CServerRule::IsRecordGameVideo(m_GameServiceOption.dwServerRule))
			{
				//��������
				SendControlPacket(CT_CONNECT_STORE, NULL, 0);
			}
			//���ͷ���
			else if ((m_GameServiceOption.wServerType & (GAME_GENRE_MATCH| GAME_GENRE_BATTLE| GAME_GENRE_RANKING)) != 0)
			{
				//��������
				SendControlPacket(CT_CONNECT_SERVICE, NULL, 0);
			}			
			else
			{
				//����״̬
				SetServiceStatus(SERVICE_STATUS_SERVICE);
			}
		}

		return 0;
	}	
	case UI_SERVICE_RESULT:			//������
	{
		//Ч����Ϣ
		ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
		if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

		//��������
		CP_ControlResult * pControlResult=(CP_ControlResult *)pData;

		//ʧ�ܴ���
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_FAILURE))
		{
			ConcludeService();
			return 0;
		}

		//�ɹ�����
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_SUCCESS))
		{
			//����״̬
			SetServiceStatus(SERVICE_STATUS_SERVICE);
		}

		return 0;
	}
	case UI_STORE_RESULT:			//�洢���
	{
		//Ч����Ϣ
		ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
		if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

		//��������
		CP_ControlResult * pControlResult=(CP_ControlResult *)pData;

		//ʧ�ܴ���
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_FAILURE))
		{
			ConcludeService();
			return 0;
		}

		//�ɹ�����
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_SUCCESS))
		{
			//���ͷ���
			if ((m_GameServiceOption.wServerType & (GAME_GENRE_MATCH | GAME_GENRE_BATTLE | GAME_GENRE_RANKING)) != 0)
			{
				//��������
				SendControlPacket(CT_CONNECT_SERVICE, NULL, 0);
			}
			else
			{
				//����״̬
				SetServiceStatus(SERVICE_STATUS_SERVICE);				
			}
		}

		return 0;
	}	
	case UI_SERVICE_CONFIG_RESULT:	//���ý��
	{
		//Ч����Ϣ
		ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
		if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

		//��������
		CP_ControlResult * pControlResult=(CP_ControlResult *)pData;

		//ʧ�ܴ���
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_FAILURE))
		{
			ConcludeService();
			return 0;
		}

		//�ɹ�����
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_SUCCESS))
		{
			//��������
			SendControlPacket(CT_CONNECT_CENTER, NULL, 0);
		}

		return 0;
	}
	case UI_SERVICE_WILLSTOP_RESULT: //Ԥֹͣ���
	{
		//Ч����Ϣ
		ASSERT(DataHead.wDataSize == sizeof(CP_ControlResult));
		if (DataHead.wDataSize != sizeof(CP_ControlResult)) return 0L;

		//��������
		CP_ControlResult* pControlResult = (CP_ControlResult*)pData;

		//�ɹ�����
		if ((m_ServiceStatus == SERVICE_STATUS_WILLSTOP) && (pControlResult->cbSuccess == ER_SUCCESS))
		{
			//ֹͣ����
			ConcludeService();
			return 0L;
		}

		//ʧ�ܴ���
		if ((m_ServiceStatus == SERVICE_STATUS_WILLSTOP) && (pControlResult->cbSuccess == ER_FAILURE))
		{
			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("����ֹͣʧ�ܣ�"), TraceLevel_Warning);

			//����״̬
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

//�����������
DECLARE_CREATE_MODULE(ServiceUnits);

//////////////////////////////////////////////////////////////////////////


