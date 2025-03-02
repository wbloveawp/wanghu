#include "StdAfx.h"
#include "GlobalUnits.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//ϵͳʱ��
#define IDI_CONNECT_STORE			(IDI_ATTEMPER_SINK_START+0)			//����ʱ��
#define IDI_CONNECT_CENTER			(IDI_ATTEMPER_SINK_START+1)			//����ʱ��
#define IDI_CONNECT_SERVICE			(IDI_ATTEMPER_SINK_START+2)			//����ʱ��
#define IDI_REPORT_SERVER_INFO		(IDI_ATTEMPER_SINK_START+3)			//������Ϣ

//����ʱ��
#define IDI_LOAD_ROBOT_INFO			(IDI_ATTEMPER_SINK_START+10)		//�û���Ϣ
#define IDI_REGULAR_TIMER			(IDI_ATTEMPER_SINK_START+12)		//���涨ʱ
#define IDI_UPDATE_ROBOTCOUNT		(IDI_ATTEMPER_SINK_START+15)		//���»���
#define IDI_DISTRIBUTE_USER			(IDI_ATTEMPER_SINK_START+16)		//�����û�
#define IDI_DISTRIBUTE_ROBOT		(IDI_ATTEMPER_SINK_START+17)		//�������

//////////////////////////////////////////////////////////////////////////
//�궨��

#define TIME_USER_IN_TABLE			(60*3)								//��������ʱ��		
#define MIN_MESSAGEG_INTERVAL		1									//ϵͳ��Ϣ��Сʱ�������룩

//���ʱ��
#define MIN_INTERVAL_TIME			10									//���ʱ��
#define MAX_INTERVAL_TIME			1*60								//��ʱ��ʶ

//����ģʽ
#define SEARCH_MODE_NO_PLAYER		0									//û�����
#define SEARCH_MODE_MIN_PLAYER		0x01								//�������
#define SEARCH_MODE_MAX_PLAYER		0x02								//������

//��������
#define SEARCH_MASK_MATCH_SCORE		0x10								//ƥ�����

//////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_bCollectUser=false;
	m_bNeedStore=true;
	m_bNeekCenter = true;
	m_bNeedService=true;
	m_bWillConcludeService=false;

	//ʱ�����
	m_dwRegularTimes=0;
	m_dwLockTableTime=TIME_USER_IN_TABLE;

	//���Ʊ���
	m_dwIntervalTime=0;
	m_dwLastDisposeTime=0;

	//�������
	m_bCacheVideo=true;
	m_VideoDataQueue.RemoveData(false);

	//���ñ���	
	ZeroMemory(&m_StoreServer,sizeof(m_StoreServer));
	ZeroMemory(&m_ServiceItem,sizeof(m_ServiceItem));

	//����Ϣ
	m_pNormalParameter=NULL;
	m_pRobotParameter=NULL;

	//ϵͳ����
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//�ں˽ӿ�
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIGameServiceManager=NULL;
	m_pIGameServiceSustomTime=NULL;	

	//����ӿ�
	m_pITCPSocketStore=NULL;
	m_pITCPSocketCenter=NULL;
	m_pITCPSocketService=NULL;

	//���ݿ�ӿ�
	m_pIDataBaseEngine=NULL;

	//���ýӿ�
	m_pIGameVideoManager=NULL;
	m_pIExtendServiceManager=NULL;

	//��������
	ZeroMemory(&CMD_ConfigColumn,sizeof(CMD_ConfigColumn));
	ZeroMemory(&CMD_ConfigServer,sizeof(CMD_ConfigServer));	


	m_FreezeScoreMap.InitHashTable(PRIME_SERVER_USER);	
}

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{
}

//�ӿڲ�ѯ
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IServiceFunction,Guid,dwQueryVer);
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE(IServerUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableEventSink,Guid,dwQueryVer);
	QUERYINTERFACE(IGameVideoItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//����Ϣ
	m_pRobotParameter=new tagBindParameter[MAX_ROBOT];
	ZeroMemory(m_pRobotParameter,sizeof(tagBindParameter)*MAX_ROBOT);

	//����Ϣ
	m_pNormalParameter=new tagBindParameter[m_pGameServiceOption->wMaxPlayer];
	ZeroMemory(m_pNormalParameter,sizeof(tagBindParameter)*m_pGameServiceOption->wMaxPlayer);

	//�û��ӿ�
	m_ScoreManager.m_pIServerUserManager = QUERY_OBJECT_INTERFACE(m_ServerUserManager, IServerUserManager);

	//���÷���
	if (InitServerConfig() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (InitTableFrame() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//���ñ���
	if (InitExtendService() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//���û���
	if (InitRobotService() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//����¼��
	if (InitVideoService() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//������־
	if (InitLoggerService() == false)
	{
		ASSERT(FALSE);
		return false;
	}	

	//���涨ʱ��
	m_pITimerEngine->SetTimer(IDI_REGULAR_TIMER,1000L,TIMES_INFINITY,0);	

	//���ò���
	m_ServerUserManager.SetControlParamter(&m_ControlParameter);
	m_ServerUserManager.SetServiceParameter(m_pGameServiceOption);

	//���ýӿ�
	if (m_ServerUserManager.SetServerUserItemSink(QUERY_ME_INTERFACE(IServerUserItemSink))==false) return false;	

	//�������
	if (g_GlobalUnits.IsAllowRobotAttend() || g_GlobalUnits.IsAllowRobotSimulate())
	{
		m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ROBOT,TIME_DISTRIBUTE_ROBOT*1000L*2,(DWORD)(-1),NULL);	
	}

	//����ģʽ
	if (m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE && m_pGameServiceOption->cbDistributeMode==DISTRIBUTE_MODE_DELAYWAIT)	
	{
		//���ù���
		m_DistributeManager.SetDistributeRule(m_pGameServiceOption->cbDistributeRule);
		m_DistributeManager.SetMinPlayerCount(m_pGameServiceOption->wMinPlayerCount);

		//���ö�ʱ��
		m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_USER,m_pGameServiceOption->wDistributeTimeSpace*1000L,(DWORD)(-1),NULL);			
	}

	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//״̬����
	m_bCollectUser=false;
	m_bNeedStore=true;
	m_bNeekCenter=true;
	m_bNeedService=true;
	m_bWillConcludeService=false;

	//������Ϣ
	ZeroMemory(&m_StoreServer,sizeof(m_StoreServer));
	ZeroMemory(&m_ServiceItem,sizeof(m_ServiceItem));

	//ʱ�����
	m_dwRegularTimes=0;
	m_dwLockTableTime=TIME_USER_IN_TABLE;

	//�������
	m_bCacheVideo=true;
	m_VideoDataQueue.RemoveData(false);

	//����Ϣ
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pRobotParameter);

	//��������
	ZeroMemory(&CMD_ConfigColumn,sizeof(CMD_ConfigColumn));
	ZeroMemory(&CMD_ConfigServer,sizeof(CMD_ConfigServer));

	//ϵͳ����
	ZeroMemory(&m_SystemParameter,sizeof(m_SystemParameter));

	//�������
	m_GlobalItemManager.ResetData();
	m_ServerUserManager.DeleteUserItem();	

	//ֹͣ����
	m_RobotUserManager.ConcludeService();

	//ֹͣ����
	g_TimeLoggerService->ConcludeService();

	//ֹͣ����
	if(m_pIExtendServiceManager!=NULL) m_pIExtendServiceManager->ConcludeService();	

	//�������
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		CTableFrameItem * pTableFrame=m_TableFrameArray[i];
		SafeDelete(pTableFrame);
	}

	//��������
	m_TableFrameArray.RemoveAll();
	m_LockedTableTimes.RemoveAll();
	m_DistributeManager.RemoveAll();

	//��������
	g_GameGoodsManager->ResetData();
	g_GameLevelManager->ResetData();
	g_GlobalOptionManager->ResetData();

	//�ں˽ӿ�
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIGameServiceManager=NULL;
	m_pIExtendServiceManager=NULL;

	//����ӿ�
	m_pITCPSocketStore=NULL;
	m_pITCPSocketCenter=NULL;
	m_pITCPSocketService=NULL;

	//���ݿ�ӿ�
	m_pIDataBaseEngine=NULL;
	
	return true;
}


//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	switch(dwIdentifier)
	{
	case CT_CONNECT_STORE:			//���Ӵ洢
	{
		//��������
		m_pITCPSocketStore->Connect(m_StoreServer.szServiceDomain,m_StoreServer.wServicePort);   

		//��ʾ��Ϣ
 		CTraceService::TraceString(TEXT("���ڳ������Ӵ洢������..."),TraceLevel_Normal);     

		return true;
	}
	case CT_CONNECT_SERVICE:		//���ӱ���
	{
		//��������
		m_pITCPSocketService->Connect(m_ServiceItem.szServiceDomain, m_ServiceItem.wServicePort);

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڳ�������%s..."),TraceLevel_Normal, m_ServiceItem.szServiceName);

		return true;
	}	
	case CT_CONNECT_CENTER:			//��������
	{
		//��������
		DWORD dwCenterServer=0L;

		//���õ�ַ
		dwCenterServer=m_pInitParameter->m_CenterAddress.dwServiceAddr;

		//��������
		m_pITCPSocketCenter->Connect(htonl(dwCenterServer), m_pInitParameter->m_wCenterPort);

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڳ����������ķ�����..."),TraceLevel_Normal);

		return true;
	}
	case CT_DEBUG_SERVICE:			//���Է���
	{
		return true;
	}
	case CT_LOAD_SERVICE_CONFIG:	//��������		
	{	
		//��������
		DBR_GR_LoadConfig LoadConfig;
		ZeroMemory(&LoadConfig,sizeof(LoadConfig));

		//���ñ���
		LoadConfig.bInitService = true;
		LoadConfig.dwConfigMask = 0xFFFFFFFF;

		//Լս����
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) == 0)
		{
			LoadConfig.dwConfigMask &= ~CONFIG_MASK_BATTLE_OPTION;
		}

		//��������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_CONFIG,0,0,0, &LoadConfig,sizeof(LoadConfig));
			
		//���ػ�����
		if (g_GlobalUnits.IsAllowRobotAttend() || g_GlobalUnits.IsAllowRobotSimulate())
		{
			m_pITimerEngine->SetTimer(IDI_LOAD_ROBOT_INFO,TIME_LOAD_ROBOT_INFO*1000L,-1,NULL);
		}

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڼ��ط�������..."),TraceLevel_Normal);

		return true;
	}
	}

	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//ʱ���¼�
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//����ʱ��
	if ((dwTimerID>=IDI_ATTEMPER_SINK_START)&&(dwTimerID<=IDI_ATTEMPER_SINK_RANGE))
	{
		//����ʱ��
		switch (dwTimerID)
		{		
		case IDI_CONNECT_STORE:			//���Ӵ洢
		{
			//��������
			m_pITCPSocketStore->Connect(m_StoreServer.szServiceDomain,m_StoreServer.wServicePort);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ڳ�������%s..."),TraceLevel_Normal, m_StoreServer.szServiceName);

			return true;
		}
		case IDI_CONNECT_SERVICE:		//���ӷ���
		{
			//��������
			m_pITCPSocketService->Connect(m_ServiceItem.szServiceDomain, m_ServiceItem.wServicePort);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ڳ�������%s..."),TraceLevel_Normal,m_ServiceItem.szServiceName);

			return true;
		}		
		case IDI_CONNECT_CENTER:		//��������
		{
			//��������
			DWORD dwCenterServer=0L;
			tagAddressInfo * pAddressInfo=&m_pInitParameter->m_CenterAddress;

			//���õ�ַ
			dwCenterServer=pAddressInfo->dwServiceAddr;

			//��������
			m_pITCPSocketCenter->Connect(htonl(dwCenterServer), m_pInitParameter->m_wCenterPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ڳ����������ķ�����..."),TraceLevel_Normal);

			return true;
		}
		case IDI_REPORT_SERVER_INFO:	//������Ϣ
		{
			//��������
			CMD_CS_C_ServiceOnline ServiceOnLine;
			ZeroMemory(&ServiceOnLine,sizeof(ServiceOnLine));

			//���ñ���
			ServiceOnLine.wOnlineCount=m_ServerUserManager.GetUserItemCount();

			//������Ϣ
			ASSERT(m_pITCPSocketCenter!=NULL);
			m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE,SUB_CS_C_SERVICE_ONLINE,&ServiceOnLine,sizeof(ServiceOnLine),0);

			return true;
		}
		case IDI_LOAD_ROBOT_INFO:		//���ػ���
		{
			//��������
			DWORD dwBatchID=0;
			DWORD dwRobotCount=0;

			//�����ж�
			if(m_RobotUserManager.GetRobotLoadInfo(dwBatchID,dwRobotCount))
			{
				//����ṹ
				DBR_GR_LoadRobotUser LoadRobotUser;
				LoadRobotUser.dwBatchID=dwBatchID;
				LoadRobotUser.dwRobotCount=dwRobotCount;

				CTraceService::TraceString(TEXT("������ػ�����, BatchID=%d,RobotCount=%d"),TraceLevel_Info, dwBatchID, dwRobotCount);

				//���ػ���
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_ROBOTS,0L,0L,0L,&LoadRobotUser,sizeof(LoadRobotUser));
			}

			return true;
		}
		case IDI_DISTRIBUTE_USER:		//�����û�
		{
			//ִ�з���
			PerformDistribute();

			return true;
		}

		case IDI_DISTRIBUTE_ROBOT:	//�����û�
		{
			//��������
			bool bDisposeSitDown=true;

			//���ö�ʱ��
			DWORD dwElpase=TIME_DISTRIBUTE_ROBOT/5+DWORD(Random_Double*(TIME_DISTRIBUTE_ROBOT/5*4));					
			m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ROBOT,dwElpase*1000L,TIMES_INFINITY,NULL);

			//���·���
			if (CServerRule::IsMatchUserByScore(m_pGameServiceOption->dwServerRule))
			{
				m_ScoreManager.UpdateScoreData();
			}

			//��������
			if (m_RobotUserManager.GetRobotCount()>0 && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
			{
				//��������
				bool bAllowDynamicJoin=CServerRule::IsDynamicJoinGame(m_pGameServiceOption->dwServerRule);
				bool bAllowRobotAttend=g_GlobalUnits.IsAllowRobotAttend();
				bool bAllowRobotSimulate=g_GlobalUnits.IsAllowRobotSimulate();
				bool bAllowDistributeMode=(m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR);
					
				//ģ�⴦��
				if (bAllowRobotSimulate==true && bAllowDistributeMode==false && bDisposeSitDown==true)
				{
					//����״̬
					tagRobotUserInfo RobotSimulate; 
					m_RobotUserManager.GetRobotUserInfo(RobotSimulate,ROBOT_SIMULATE);

					//��������
					if (RobotSimulate.wFreeUserCount>0)
					{
						for (WORD i=0;i<8;i++)
						{
							//�������
							WORD wTableID=WORD(Random_Double*(__max(m_pGameServiceOption->wTableCount/3,1)));

							//��ȡ����
							CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
							if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

							//����״��
							tagTableUserInfo TableUserInfo;
							WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

							//��������
							WORD wChairCount=m_pGameServiceAttrib->wChairCount;	

							//�����ж�
							if (TableUserInfo.wTableUserCount>0) continue;
							if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(m_pGameServiceAttrib->wChairCount<MAX_CHAIR)) continue;

							//�����ж�
							if (RobotSimulate.wFreeUserCount>=TableUserInfo.wMinUserCount)
							{
								//��������
								WORD wHandleCount=0;
								WORD wWantRobotCount=TableUserInfo.wMinUserCount;

								//���ݵ���
								if (m_pGameServiceAttrib->wChairCount>TableUserInfo.wMinUserCount)
								{
									WORD wFreeUserCount=RobotSimulate.wFreeUserCount;
									WORD wOffUserCount=__min(wChairCount,wFreeUserCount)-TableUserInfo.wMinUserCount;
									wWantRobotCount+=((wOffUserCount > 0) ? WORD(Random_Double*(wOffUserCount+1)) : 0);
								}

								//���´���
								for (WORD j=0;j<RobotSimulate.wFreeUserCount;j++)
								{
									//��������
									WORD wChairID=pTableFrame->GetRandNullChairID();

									//��Ч����
									//ASSERT(wChairID!=INVALID_CHAIR);
									if (wChairID==INVALID_CHAIR) continue;

									//�û�����
									CRobotUserItem * pRobotUserItem=(CRobotUserItem *)RobotSimulate.pIRobotUserFree[j];
									if (pTableFrame->PerformSitDownAction(wChairID, pRobotUserItem->GetMeUserItem())==true)
									{
										//���ñ���
										wHandleCount++;

										//����ж�
										if (wHandleCount>=wWantRobotCount) 
										{
											return true;
										}
									}
								}

								if(wHandleCount > 0) return true;
							}
						}
					}
				}

				//��������
				bool bWaitContinue=true;
				bool bMaxPlayerGame=m_pGameServiceAttrib->wChairCount>=MAX_CHAIR;

				//��ȡʱ��
				DWORD dwCurrTime = (DWORD)time(NULL);
				if (dwCurrTime>=m_dwIntervalTime+m_dwLastDisposeTime)
				{
					//���ñ���
					bWaitContinue=false;
					m_dwLastDisposeTime=dwCurrTime;
					m_dwIntervalTime=MIN_INTERVAL_TIME/2; 
					if(bMaxPlayerGame==true)
					{
						m_dwIntervalTime=MIN_INTERVAL_TIME+DWORD(Random_Double*(MAX_INTERVAL_TIME-MIN_INTERVAL_TIME)); 
					}
				}

				//�����
				if (bAllowRobotAttend==true && bDisposeSitDown==true)
				{
					//����״̬
					tagRobotUserInfo RobotPassivity;
					m_RobotUserManager.GetRobotUserInfo(RobotPassivity,ROBOT_PASSIVITY);

					if (bAllowDistributeMode==true)
					{
						//���´���
						for (WORD j=0;j<RobotPassivity.wFreeUserCount;j++)
						{
							CRobotUserItem * pRobotUserItem=(CRobotUserItem *)RobotPassivity.pIRobotUserFree[j];
							if (InsertDistribute(pRobotUserItem->GetMeUserItem())==true) return true;
						}
					}
					else
					{
						//��������
						if (RobotPassivity.wFreeUserCount>0)
						{
							//������Ϸ
							if (bMaxPlayerGame==true)
							{
								for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
								{
									//��ȡ����
									CTableFrameItem * pTableFrame=m_TableFrameArray[i];
									if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

									//����״��
									tagTableUserInfo TableUserInfo;
									WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

									//�����ж�
									if (wUserSitCount>m_pGameServiceAttrib->wChairCount*2/3) continue;

									//��������
									IServerUserItem * pIServerUserItem=NULL;
									WORD wChairID=pTableFrame->GetRandNullChairID();

									//��Ч����
									ASSERT(wChairID!=INVALID_CHAIR);
									if (wChairID==INVALID_CHAIR) continue;

									//���´���
									for (WORD j=0;j<RobotPassivity.wFreeUserCount;j++)
									{
										//��ȡ�û�
										CRobotUserItem * pRobotUserItem=(CRobotUserItem *)RobotPassivity.pIRobotUserFree[j];
										IServerUserItem * pIServerUserItem= pRobotUserItem->GetMeUserItem();

										//ִ������
										if (pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==true) return true;
									}
								}
							}
							else
							{
								//��ȡʱ��
								if (bWaitContinue==false)
								{
									INT nStartTable=INT(Random_Double*__max(m_pGameServiceOption->wTableCount-1,1));
									for (INT_PTR i=nStartTable;i<(nStartTable+m_pGameServiceOption->wTableCount);i++)
									{
										//��ȡ����
										WORD wTableID=WORD(i%m_pGameServiceOption->wTableCount);
										CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
										if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

										//����״��
										tagTableUserInfo TableUserInfo;
										WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

										//�����ж�											
										if (TableUserInfo.wClientReadyCount==0) continue;									
										if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(WORD(Random_Double*10)>7)) continue;											

										//��������
										IServerUserItem * pIServerUserItem=NULL;
										WORD wChairID=pTableFrame->GetRandNullChairID();

										//��Ч����
										if (wChairID==INVALID_CHAIR) continue;

										//��������
										INT nAreaIndex=0;
										bool bMatchUserByScore=CServerRule::IsMatchUserByScore(m_pGameServiceOption->dwServerRule)&&(wUserSitCount>0);

										//��ȡ����
										if (bMatchUserByScore==true) nAreaIndex=GetTableScoreAreaIndex(pTableFrame);	

										//���´���
										for (WORD j=0;j<RobotPassivity.wFreeUserCount;j++)
										{
											//��ȡ�û�
											CRobotUserItem * pRobotUserItem=(CRobotUserItem *)RobotPassivity.pIRobotUserFree[j];
											IServerUserItem * pIServerUserItem= pRobotUserItem->GetMeUserItem();

											//ƥ�����
											if (bMatchUserByScore==true)
											{
												//��ȡ����																	
												INT nAreaIndex1=m_ScoreManager.GetScoreAreaIndex(pIServerUserItem->GetUserFreeScore());	

												//ƥ������
												if (nAreaIndex!=nAreaIndex1) continue;			
											}

											//ִ������
											if (pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==true) return true;
										}
									}
								}
							}
						}
					}
				}
	    
				//�����
				if (bAllowRobotAttend==true && bDisposeSitDown==true)
				{
					//����״̬
					tagRobotUserInfo RobotInitiative;
					m_RobotUserManager.GetRobotUserInfo(RobotInitiative,ROBOT_INITIATIVE);
					WORD wAllRobotCount = RobotInitiative.wFreeUserCount+RobotInitiative.wPlayUserCount+RobotInitiative.wSitdownUserCount;

					if (bAllowDistributeMode==true)
					{
						//���´���
						for (WORD j=0;j<RobotInitiative.wFreeUserCount;j++)
						{
							CRobotUserItem * pRobotUserItem=(CRobotUserItem *)RobotInitiative.pIRobotUserFree[j];
							if (InsertDistribute(pRobotUserItem->GetMeUserItem())==true) return true;
						}
					}
					else
					{
						//��������
						if (RobotInitiative.wFreeUserCount>0)
						{
							//��ȡʱ��
							if (bMaxPlayerGame==true || bWaitContinue==false)
							{
								INT nStartTable=INT(Random_Double*__max(m_pGameServiceOption->wTableCount-1,1));
								for (INT_PTR i=nStartTable;i<(nStartTable+m_pGameServiceOption->wTableCount);i++)
								{
									//��ȡ����
									WORD wTableID=WORD(i%m_pGameServiceOption->wTableCount);
									CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
									if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

									//����״��
									tagTableUserInfo TableUserInfo;
									WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

									//��������
									if (bMaxPlayerGame==false) 
									{									
										//�����ж�
										if ((wUserSitCount >= TableUserInfo.wMinUserCount) && (WORD(Random_Double * 10) > 7)) continue;
									}

									//��������
									IServerUserItem * pIServerUserItem=NULL;
									WORD wChairID=pTableFrame->GetRandNullChairID();

									//��Ч����
									if (wChairID==INVALID_CHAIR) continue;

									//��������
									INT nAreaIndex=0;
									bool bMatchUserByScore=CServerRule::IsMatchUserByScore(m_pGameServiceOption->dwServerRule)&&(wUserSitCount>0);

									//��ȡ����
									if (bMatchUserByScore==true) nAreaIndex=GetTableScoreAreaIndex(pTableFrame);	

									//���´���
									for (WORD j=0;j<RobotInitiative.wFreeUserCount;j++)
									{
										//��ȡ�û�
										CRobotUserItem * pRobotUserItem=(CRobotUserItem *)RobotInitiative.pIRobotUserFree[j];
										IServerUserItem * pIServerUserItem= pRobotUserItem->GetMeUserItem();

										//ƥ�����
										if (bMatchUserByScore==true)
										{
											//��ȡ����																	
											INT nAreaIndex1=m_ScoreManager.GetScoreAreaIndex(pIServerUserItem->GetUserFreeScore());	

											//ƥ������
											if (nAreaIndex!=nAreaIndex1) continue;			
										}

										//ִ������
										if (pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==true) return true;
									}
								}
							}
						}
					}
				}

				//״̬�ж�
				if (bWaitContinue==true) return true;

				//��������
				WORD wStandUpCount=0;
				WORD wRandStartChair=0;
				WORD wRandCount=WORD(Random_Double*2)+1;
				INT_PTR nIndex = INT_PTR(Random_Double*(__max(m_pGameServiceOption->wTableCount-1,1)));
				for (INT_PTR i=nIndex;i<m_pGameServiceOption->wTableCount+nIndex;++i)
				{
					//��ȡ����
					INT_PTR nTableIndex=i%m_pGameServiceOption->wTableCount;
					CTableFrameItem * pTableFrame=m_TableFrameArray[nTableIndex];
					if (pTableFrame->IsGameStarted()==true) continue;

					//����״��
					tagTableUserInfo TableUserInfo;
					WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

					//�û�����
					bool bRand=(WORD(Random_Double*100)>60);
					if (TableUserInfo.wTableRobotCount==0) continue;
					if ((TableUserInfo.wTableUserCount>0)&&(bAllowRobotAttend==true) && bRand) continue;
					if (TableUserInfo.wTableRobotCount>=TableUserInfo.wMinUserCount && bRand) continue;

					//��������
					for (WORD j=0;j<pTableFrame->GetChairCount();j++)
					{
						//��ȡ�û�
						IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
						if (pIServerUserItem==NULL) continue;

						//�û�����
						if (pIServerUserItem->IsRobotUser()==true)								
						{
							if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) continue;
							if (++wStandUpCount>=wRandCount) return true;
							break;
						}
					}
				}

				//��������
				nIndex = INT_PTR(Random_Double*(__max(m_pGameServiceOption->wTableCount-1,1)));
				for (INT_PTR i=nIndex;i<m_pGameServiceOption->wTableCount+nIndex;++i)
				{
					//��ȡ����
					INT_PTR nTableIndex=i%m_pGameServiceOption->wTableCount;
					CTableFrameItem * pTableFrame=m_TableFrameArray[nTableIndex];
					if (pTableFrame->IsGameStarted()==true) continue;

					//����״��
					tagTableUserInfo TableUserInfo;
					WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

					//�û�����
					bool bRand=(WORD(Random_Double*100)>70);
					if (TableUserInfo.wTableRobotCount==0) continue;
					if ((TableUserInfo.wTableUserCount>0)&&(bAllowRobotAttend==true) && bRand) continue;

					//��������	
					for (WORD j=0;j<pTableFrame->GetChairCount();j++)
					{
						//��ȡ�û�
						IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
						if (pIServerUserItem==NULL) continue;							

						//�û�����
						if (pIServerUserItem->IsRobotUser()==true)
						{
							if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) continue;
							if (++wStandUpCount>=wRandCount) return true;
							break;
						}
					}
				}
			}				
				
			return true;
		}		
		case IDI_REGULAR_TIMER:			//���涨ʱ��
		{
			//����ʱ��
			m_dwRegularTimes++;

			//��������
			POSITION pos = m_LockedTableTimes.GetStartPosition();
			WORD wTableID = INVALID_TABLE;
			DWORD dwLockedTableTimes = 0;
			while(pos!=NULL)
			{
				m_LockedTableTimes.GetNextAssoc(pos,wTableID,dwLockedTableTimes);
					
				//�������
				ASSERT(wTableID!=INVALID_TABLE);
				CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
				ASSERT(pTableFrame);
				if( !pTableFrame || !pTableFrame->IsTableLocked() || pTableFrame->IsTableStarted() )
				{
					m_LockedTableTimes.RemoveKey(wTableID);
					continue;
				}

				//��ʱ�ж�
				if( dwLockedTableTimes <= m_dwRegularTimes )
				{
					//�߳��������
					for( WORD i = 0; i < pTableFrame->GetChairCount(); i++ )
					{
						IServerUserItem *pIServerUserItem = pTableFrame->GetTableUserItem(i);
						if( pIServerUserItem == NULL ) continue;

						//������Ϣ
						LPCTSTR pszMessage=TEXT("��Ǹ�����ڱ�����ʱ��û�п�ʼ��Ϸ��ϵͳ����������ѡ�����ӣ�");
						SendGameMessage(pIServerUserItem,pszMessage,SMT_CHAT|SMT_EJECT|SMT_CLOSE_GAME);
						SendRoomMessage(pIServerUserItem,pszMessage,SMT_CHAT);
							
						//ִ������
						pTableFrame->PerformStandUpAction(pIServerUserItem);
					}

					//ɾ������
					m_LockedTableTimes.RemoveKey(wTableID);
				}
			}
				
			return true;
		}		
		}

		return false;
	}


	//������ʱ��
	if ((dwTimerID>=IDI_ROBOT_MANAGER_START)&&(dwTimerID<=IDI_ROBOT_MANAGER_RANGE))
	{
		//ʱ�䴦��
		m_RobotUserManager.OnEventTimerPulse(dwTimerID,dwBindParameter);

		return true;
	}
	
	//��չ��ʱ��
	if ((dwTimerID>=IDI_EXTEND_MODULE_START)&&(dwTimerID<IDI_EXTEND_MODULE_RANGE))
	{
		if(m_pIExtendServiceManager!=NULL) m_pIExtendServiceManager->OnEventTimer(dwTimerID,dwBindParameter);
		return true;
	}
	
	//����ʱ��
	if (dwTimerID>=IDI_TABLE_FRAME_START&&dwTimerID<=IDI_TABLE_FRAME_RANGE)
	{
		//���Ӻ���
		DWORD dwTableTimerID=dwTimerID-IDI_TABLE_FRAME_START;
		WORD wTableID=(WORD)(dwTableTimerID/TIME_TABLE_FRAME_RANGE);

		//ʱ��Ч��
		if (wTableID>=(WORD)m_TableFrameArray.GetCount()) 
		{
			ASSERT(FALSE);
			return false;
		}

		//ʱ��֪ͨ
		CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
		return pTableFrame->OnEventTimer(dwTableTimerID%TIME_TABLE_FRAME_RANGE,dwBindParameter);
	}

	return false;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	ZeroMemory(pBindParameter, sizeof(tagBindParameter));

	//���ñ���
	if (pBindParameter!=NULL)
	{
		pBindParameter->dwClientIP=dwClientIP;
		pBindParameter->dwContextID=dwSocketID;
		pBindParameter->dwActiveTime=(DWORD)time(NULL);
		pBindParameter->TokenParameter.dwClientIP=dwClientIP;

		return true;
	} 

	//�������
	ASSERT(FALSE);

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//������
	bool bResult=true;
	WORD wMainCmdID = LOCMMD(Command.wMainCmdID);

#ifdef _DEBUG
	{
		CString str;
		str.Format(L"[LH][Game]%d,M:%d,S:%d", Command.wMainCmdID, wMainCmdID, Command.wSubCmdID);
		OutputDebugString(str);
	}
#endif

	//ע�����
	if (wMainCmdID == MDM_CS_REGISTER)
	{
		return OnTCPNetworkMainRegister(Command.wSubCmdID, pData, wDataSize, dwSocketID);
	}

	//ͨ�÷���
	if (wMainCmdID == MDM_CS_COMMON_SERVICE)
	{
		return OnTCPNetworkMainCommonService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
	}

	try
	{
		switch (wMainCmdID)
		{
		case MDM_GR_LOGON:		//��¼����
		{
			bResult=OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
			break;
		}
		case MDM_GR_USER:		//�û�����
		{
			bResult=OnTCPNetworkMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
			break;
		}
		case MDM_GR_MANAGE:		//��������
		{
			bResult=OnTCPNetworkMainManage(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
			break;
		}
		case MDM_GF_FRAME:		//�������
		{
			bResult=OnScoketMainFrame(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
			break;
		}
		case MDM_GF_GAME:		//��Ϸ����
		{
			bResult=OnScoketMainGame(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
			break;
		}
		case MDM_GR_MEMBER:		//��Ա����
		{
			bResult=OnTCPNetworkMainMember(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
			break;
		}
		case MDM_GR_QUERY:		//��ѯ����
		{
			bResult=OnTCPNetworkMainQuery(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
			break;
		}
		case MDM_GR_MATCH:		//��������
		{
			bResult=OnTCPNetworkMainMatch(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
			break;
		}
		case MDM_GR_DEBUG:		//��������
		{
			bResult = OnTCPNetworkMainDebug(Command.wSubCmdID, pData, wDataSize, dwSocketID, dwTokenID);
			break;
		}	
		case MDM_GR_BATTLE:		//Լս����
		{
			bResult = OnTCPNetwrokMainBattle(Command.wSubCmdID, pData, wDataSize, dwSocketID, dwTokenID);
			break;			
		}
		}			
	} 
	catch (...)
	{
		//���ý��
		bResult=false;

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("wMainCmdID=%d,wSubCmdID=%d"),TraceLevel_Normal, Command.wMainCmdID, Command.wSubCmdID);
	}

	//ǿ���˳�
	if (bResult==false)
	{
		//����ṹ
		CMD_CS_C_ShutdownSocket ShutdownSocket;
		ShutdownSocket.bForceClose = false;

		//��������
		SendDataToGate(dwSocketID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));
	}

	return true;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��ȡ����
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//��������
	IServerUserItem * pIServerUserItem = NULL;
	tagTokenParameter * pTokenParameter = NULL;

	//ֱ��ģʽ
	if (pBindParameter->wServiceModule == SERVICE_MODULE_NONE)
	{
		//��ȡ����
		pTokenParameter = &pBindParameter->TokenParameter;

		//��ȡ�û�
		pIServerUserItem = (IServerUserItem*)pTokenParameter->pBindUserItem;
		if (pIServerUserItem == NULL) return false;

		//���ӱ�ʶ
		WORD wTableID = pIServerUserItem->GetTableID();

		//���ߴ���
		if (wTableID != INVALID_TABLE)
		{
			//�����
			pIServerUserItem->DetachBindStatus();

			ASSERT(wTableID < m_pGameServiceOption->wTableCount);
			m_TableFrameArray[wTableID]->OnEventUserOffLine(pIServerUserItem);
		}
		else
		{
			pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
		}
	}
	else
	{
		//��������
		auto pGlobalGateItem = pBindParameter->pGlobalGateItem;
		if (pGlobalGateItem != NULL)
		{
			//��������
			tagTokenParameter* pTokenParameter = NULL;

			do
			{
				//��ȡ����
				pTokenParameter = pGlobalGateItem->EnumTokenID(pTokenParameter);
				if (pTokenParameter == NULL) break;

				//��ȡ�û�
				pIServerUserItem = (IServerUserItem*)pTokenParameter->pBindUserItem;
				if (pIServerUserItem != NULL)
				{
					//���ӱ�ʶ
					WORD wTableID = pIServerUserItem->GetTableID();

					//���ߴ���
					if (wTableID != INVALID_TABLE)
					{
						//�����
						pIServerUserItem->DetachBindStatus();

						//����֪ͨ
						ASSERT(wTableID < m_pGameServiceOption->wTableCount);
						m_TableFrameArray[wTableID]->OnEventUserOffLine(pIServerUserItem);
					}
					else
					{
						pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
					}
				}				

			} while (true);

			//�Ƴ�����
			m_GlobalItemManager.DeleteGateItem(pBindParameter->dwServiceID);
		}		
	}

	//�����Ϣ
	ZeroMemory(pBindParameter, sizeof(tagBindParameter));

	return true;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��Ϸ�¼�
	if (wRequestID >= DB_GAME_RESPOND_START && wRequestID<= DB_GAME_RESPOND_START+DB_GAME_IDENTIFY_RANGE)
	{
		if (m_pIGameServiceManager != NULL)
		{
			m_pIGameServiceManager->OnEventDataBase(wRequestID, pData, wDataSize, dwContextID, dwTokenID);
		}
	}

	//��չ�¼�
	if (wRequestID >= DB_EXTEND_RESPOND_START && wRequestID <= DB_EXTEND_RESPOND_START + DB_EXTEND_IDENTIFY_RANGE)
	{
		if (m_pIExtendServiceManager != NULL)
		{
			return m_pIExtendServiceManager->OnEventDataBase(wRequestID, pData, wDataSize, dwContextID, dwTokenID);
		}
	}

	//����¼�
	if (wRequestID >= DB_FRAME_RESPOND_START && wRequestID <= DB_FRAME_RESPOND_START + DB_FRAME_IDENTIFY_RANGE)
	{
		switch (wRequestID)
		{
		case DBO_GR_ROBOT_USER:			//������Ϣ
		{
			return OnDBRobotUser(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_VERSION_INFO:		//�汾��Ϣ
		{
			return OnDBGameVersionInfo(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_CONFIG_RESULT:		//���ý��
		{
			return OnDBConfigResult(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_BATTLE_GAME_OPTION:	//��Ϸѡ��
		{
			return OnDBBattleOption(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_ROBOT_PARAMETER:	//��������
		{
			return OnDBRobotParameter(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_SUBSIDY_PARAMETER:	//�ͱ�����
		{
			return OnDBSubsidyParameter(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_CONTROL_PARAMETER:	//���Ʋ���
		{
			return OnDBControlParameter(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_LOGON_SUCCESS:		//��¼�ɹ�
		{
			return OnDBLogonSuccess(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_LOGON_FAILURE:		//��¼ʧ��
		{
			return OnDBLogonFailure(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_WEALTH_UPDATE:		//�Ƹ�����
		{
			return OnDBWealthUpdate(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_WRITE_GAME_SCORE:	//д�ֽ��
		{
			return OnDBWriteScoreResult(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_FILTER_WORDS:		//�������д�
		{
			return OnDBFilterWords(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_UPDATE_GOLD:		//���½��
		{
			return OnDBUpdateGold(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_WRITE_USER_PROP:	//д�����
		{
			return OnDBWriteUserProp(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_USER_GOODS_UPDATE:	//��Ʒ����
		{
			return OnDBUserGoodsUpdate(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_LARGESS_SUBSIDY:	//��ȡ�ͱ�
		{
			return OnDBLargessSubsidy(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_USER_CONTROL_INFO:	//������Ϣ
		{
			return OnDBUserControlInfo(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_USER_CONTROL_UPDATE://���Ƹ���
		{
			return OnDBUserControlUpdate(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_SEND_GIFT_SUCCESS:	//���ͳɹ�
		{
			return OnDBSendGiftSuccess(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_SEND_GIFT_FAILURE:	//����ʧ��
		{
			return OnDBSendGiftFailure(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_UPDATE_LOVELINESS:	//��������
		{
			return OnDBUpdateLoveLiness(dwContextID, dwTokenID, pData, wDataSize);
		}
		}
	}

	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode)
{
	//�������
	ITCPSocketService * pITCPSocketService = NULL;

	//��������
	if (dwServiceID==NETWORK_CENTER)
	{
		//�����ж�
		if (nErrorCode!=0)
		{
			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ķ���������ʧ�� [ %ld ]��%ld �����������"),TraceLevel_Warning, nErrorCode, m_pInitParameter->m_wConnectTime);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("����ע����Ϸ����..."),TraceLevel_Normal);

		//���ýӿ�
		pITCPSocketService = m_pITCPSocketCenter;	

		//���ö�ʱ��
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_REPORT_SERVER_INFO,TIME_REPORT_SERVER_INFO*1000L,TIMES_INFINITY,0);		
	}

	//�洢����
	if (dwServiceID==NETWORK_STORE)
	{
		//�����ж�
		if (nErrorCode!=0)
		{
			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("�洢����������ʧ�� [ %ld ]��%ld �����������"),TraceLevel_Warning, nErrorCode, m_pInitParameter->m_wConnectStore);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_STORE,m_pInitParameter->m_wConnectStore*1000L,1,0);

			return false;
		}

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("����ע����Ϸ����..."),TraceLevel_Normal);

		//���ýӿ�
		pITCPSocketService = m_pITCPSocketStore;
	}	

	//��������
	if (dwServiceID==NETWORK_SERVICE)
	{
		//�����ж�
		if (nErrorCode!=0)
		{
			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("%s����ʧ�� [ %ld ]��%ld �����������"),TraceLevel_Warning, m_ServiceItem.szServiceName, nErrorCode, m_pInitParameter->m_wConnectService);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_SERVICE,m_pInitParameter->m_wConnectService *1000L,1,0);

			return false;
		}

		//���ýӿ�
		pITCPSocketService = m_pITCPSocketService;
	}

	//ע�����
	if (pITCPSocketService != NULL)
	{
		CMD_CS_C_RegisterServer RegisterServer;
		ZeroMemory(&RegisterServer, sizeof(RegisterServer));

		//��������
		RegisterServer.wModuleID = m_pGameServiceAttrib->wKindID;
		RegisterServer.wKindID = m_pGameServiceOption->wKindID;
		RegisterServer.wNodeID = m_pGameServiceOption->wNodeID;
		RegisterServer.wSortID = m_pGameServiceOption->wSortID;
		RegisterServer.wVisibleMask = m_pGameServiceOption->wVisibleMask;
		RegisterServer.wServerID = m_pGameServiceOption->wServerID;
		RegisterServer.wServerType = m_pGameServiceOption->wServerType;
		RegisterServer.wServerLevel = m_pGameServiceOption->wServerLevel;
		RegisterServer.wTableCount = m_pGameServiceOption->wTableCount;
		RegisterServer.dwServerRule = m_pGameServiceOption->dwServerRule;
		RegisterServer.cbMatchType = m_pGameServiceOption->cbMatchType;
		RegisterServer.wServerPort = m_pGameServiceOption->wServerPort != 0 ? (m_pGameServiceOption->wServerPort) : (m_pITCPNetworkEngine->GetCurrentPort());
		RegisterServer.wChairCount = m_TableFrameArray[0]->m_wMaxPlayerCount;		
		RegisterServer.dwMaxPlayer = m_pGameServiceOption->wTableCount * m_TableFrameArray[0]->m_wMaxPlayerCount;
		RegisterServer.dwOnLineCount = m_ServerUserManager.GetUserItemCount();
		StringCchCopy(RegisterServer.szServerName, CountArray(RegisterServer.szServerName), m_pGameServiceOption->szServerName);

		//��ַ��Ϣ
		RegisterServer.dwServerAddr = inet_addr(CT2CA(m_pGameServiceOption->szServiceAddr));
		StringCchCopy(RegisterServer.szServerDomain, CountArray(RegisterServer.szServerDomain), m_pGameServiceOption->szServiceDomain);

		//��������
		RegisterServer.lCellScore = m_pGameServiceOption->lCellScore;
		RegisterServer.lMinEnterScore = m_pGameServiceOption->lMinEnterScore;
		RegisterServer.lMaxEnterScore = m_pGameServiceOption->lMaxEnterScore;

		//��������
		ASSERT(pITCPSocketService != NULL);
		pITCPSocketService->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_SERVER, &RegisterServer, sizeof(RegisterServer), 0);
	}	

	return true;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason)
{
	switch (dwServiceID)
	{	
	case NETWORK_CENTER:	//��������
	{
		//���ñ���
		m_bCollectUser=false;

		//ɾ��ʱ��
		m_pITimerEngine->KillTimer(IDI_REPORT_SERVER_INFO);

		//�����ж�
		if (m_bNeekCenter==true)
		{
			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("�����ķ����������ӹر��ˣ�%ld �����������"),TraceLevel_Warning, TIME_CONNECT_CENTER);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER, TIME_CONNECT_CENTER *1000L,1,0);
		}

		return true;
	}	
	case NETWORK_STORE:			//�洢����
	{
		//���ñ���
		m_bCacheVideo=true;

		//�����ж�
		if (m_bNeedStore==true)
		{
			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("��洢�����������ӹر��ˣ�%ld �����������"),TraceLevel_Warning, m_pInitParameter->m_wConnectStore);

			//����ʱ��
			ASSERT (m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_STORE, m_pInitParameter->m_wConnectStore *1000L,1,0);
		}

		return true;
	}
	case NETWORK_SERVICE:		 //��������
	{
		//�����ж�
		if (m_bNeedService==true)
		{
			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("��%s�����ӹر��ˣ�%ld �����������"),TraceLevel_Warning,m_ServiceItem.szServiceName, m_pInitParameter->m_wConnectService);

			//����ʱ��
			ASSERT (m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_SERVICE, m_pInitParameter->m_wConnectService *1000L,1,0);
		}

		return true;
	}
	}

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	try
	{
		//�洢����
		if (dwServiceID==NETWORK_STORE)
		{
			switch (Command.wMainCmdID)
			{
			case MDM_CS_REGISTER:		//ע����Ϣ
				{
					return OnTCPSocketMainStoreRegister(Command.wSubCmdID,pData,wDataSize);
				}
			}		
		}

		//��������
		if (dwServiceID==NETWORK_SERVICE)
		{
			switch (Command.wMainCmdID)
			{
			case MDM_CS_REGISTER:		 //ע����Ϣ
			{
				return OnTCPSocketMainServiceRegister(Command.wSubCmdID, pData, wDataSize);
			}
			default:
			{
				if (m_pIExtendServiceManager != NULL)
				{
					return m_pIExtendServiceManager->OnEventSocketRead(Command.wMainCmdID, Command.wSubCmdID, pData, wDataSize);
				}
			}
			}				
		}

		//��������
		if (dwServiceID==NETWORK_CENTER)
		{
			switch (Command.wMainCmdID)
			{
			case MDM_CS_REGISTER:			//ע�����
			{
				return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
			}
			case MDM_CS_COMMON_SERVICE:		//������Ϣ
			{
				return OnTCPSocketMainCommonService(Command.wSubCmdID,pData,wDataSize);
			}
			case MDM_CS_FORWARD_SERVICE:	//ת������
			{
				return OnTCPSocketMainForwardService(Command.wSubCmdID, pData, wDataSize);
			}
			case MDM_CS_USER_COLLECT:		//�û�����
			{
				return OnTCPSocketMainUserCollect(Command.wSubCmdID,pData,wDataSize);
			}
			case MDM_CS_QUERY_SERVICE:		//��ѯ����
			{
				return OnTCPSocketMainQueryService(Command.wSubCmdID,pData,wDataSize);
			}
			case MDM_CS_BROADCAST:			//�㲥��Ϣ
			{
				return OnTCPSocketMainBroadcast(Command.wSubCmdID,pData,wDataSize);
			}
			case MDM_CS_MANAGER_SERVICE:	//�������
			{
				return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
			}
			}
		}
	}
	catch (...)
	{
		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("dwServiceID=%d,wMainCmdID=%d,wSubCmdID=%d"),TraceLevel_Normal, dwServiceID, Command.wMainCmdID, Command.wSubCmdID);
	}

	return false;
}

//���ƿ�ʼ
bool CAttemperEngineSink::OnEventUserControlStart(IServerUserItem * pIServerUserItem)
{
	//����У��
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL) return false;
	
	//����ṹ
	DBR_GR_ControlStart ControlStart;
	ZeroMemory(&ControlStart,sizeof(ControlStart));

	//�û���Ϣ
	ControlStart.dwUserID = pIServerUserItem->GetUserID();		

	//��Ӯ�ۼ�
	ControlStart.wAddupWinRate = pIServerUserItem->GetAddupWinRate();
	ControlStart.lAddupWinScore = pIServerUserItem->GetAddupWinScore();
	ControlStart.lAddupLoseScore = pIServerUserItem->GetAddupLoseScore();

	//������Ϣ
	CopyMemory(&ControlStart.UserControlItem, pIServerUserItem->GetUserControlItem(), sizeof(tagUserControlItem));
		
	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_CONTROL_START, ControlStart.dwUserID, MAKELONG(pIServerUserItem->GetBindIndex(),0), pIServerUserItem->GetTokenID(), &ControlStart, sizeof(ControlStart));

	return true;
}

//���Ƹ���
bool CAttemperEngineSink::OnEventUserControlUpdate(IServerUserItem* pIServerUserItem)
{
	//����У��
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return false;

	//����ṹ
	DBR_GR_ControlUpdate ControlUpdate;
	ZeroMemory(&ControlUpdate, sizeof(ControlUpdate));

	//������Ϣ
	const tagUserControlItem* pUserControlItem = pIServerUserItem->GetUserControlItem();

	//�û���Ϣ
	ControlUpdate.dwUserID = pIServerUserItem->GetUserID();

	//��Ϸ��Ϣ
	ControlUpdate.lWinScore = pUserControlItem->lTotalWinScore;
	ControlUpdate.lLoseScore = pUserControlItem->lTotalLoseScore;
	ControlUpdate.lWinCount = pUserControlItem->lTotalWinCount;
	ControlUpdate.lLoseCount = pUserControlItem->lTotalLoseCount;
	ControlUpdate.lBetAmount = pUserControlItem->lTotalBetAmount;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_CONTROL_UPDATE, ControlUpdate.dwUserID, MAKELONG(pIServerUserItem->GetBindIndex(), 0), pIServerUserItem->GetTokenID(), &ControlUpdate, sizeof(ControlUpdate));

	return true;
}

//���ƽ���
bool CAttemperEngineSink::OnEventUserControlFinish(IServerUserItem * pIServerUserItem)
{
	//����У��
	ASSERT (pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return false;

	//����ṹ
	DBR_GR_ControlFinish ControlFinish;
	ZeroMemory(&ControlFinish, sizeof(ControlFinish));

	//�û���Ϣ
	ControlFinish.dwUserID = pIServerUserItem->GetUserID();
		
	//��Ӯ�ۼ�
	ControlFinish.wAddupWinRate = pIServerUserItem->GetAddupWinRate();
	ControlFinish.lAddupWinScore = pIServerUserItem->GetAddupWinScore();
	ControlFinish.lAddupLoseScore = pIServerUserItem->GetAddupLoseScore();

	//��ȡ����
	const tagUserControlItem * pUserControlItem = pIServerUserItem->GetUserControlItem();
		
	//��Ϸ��Ϣ
	ControlFinish.lWinScore = pUserControlItem->lTotalWinScore;
	ControlFinish.lLoseScore = pUserControlItem->lTotalLoseScore;
	ControlFinish.lWinCount = pUserControlItem->lTotalWinCount;
	ControlFinish.lLoseCount = pUserControlItem->lTotalLoseCount;
	ControlFinish.lBetAmount = pUserControlItem->lTotalBetAmount;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_CONTROL_FINISH,ControlFinish.dwUserID, MAKELONG(pIServerUserItem->GetBindIndex(), 0), pIServerUserItem->GetTokenID(), &ControlFinish, sizeof(ControlFinish));

	return true;
}

//�û�����
bool CAttemperEngineSink::OnEventUserItemScore(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//���ͳɼ�
	SendUserScore(pIServerUserItem);	

	//��������
	WORD wBindIndex=pIServerUserItem->GetBindIndex();
	
	//֪ͨ����
	if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		//��������
		CTableFrameItem * pTableFrame = m_TableFrameArray[pIServerUserItem->GetTableID()];
		if (pTableFrame != NULL)
		{
			pTableFrame->OnEventUserScoreChange(pIServerUserItem);
		}
	}

	//֪ͨ����
	if (m_pIExtendServiceManager != NULL)
	{
		m_pIExtendServiceManager->OnEventUserItemScore(pIServerUserItem);
	}

	//��ʱд��
	if (CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)==true)
	{
		//��������
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

		//������Ϣ
		tagLockScoreInfo LockScoreInfo;
		ZeroMemory(&LockScoreInfo,sizeof(LockScoreInfo));
		LockScoreInfo.lMaxLockScore=m_pGameServiceOption->lMaxLockScore;
		LockScoreInfo.lLessLockScore=m_pGameServiceOption->lLessLockScore;
		LockScoreInfo.lRestrictScore=m_pGameServiceOption->lRestrictScore;

		//��ȡ����
		tagVariationScore VariationScore;
		ZeroMemory(&VariationScore,sizeof(VariationScore));
		pIServerUserItem->DistillVariation(VariationScore,LockScoreInfo);

		//�û���Ϣ
		WriteGameScore.dwUserID=pIServerUserItem->GetUserID();
		WriteGameScore.dwClientIP=pIServerUserItem->GetClientIP();
		WriteGameScore.dwPlayTimeCount=VariationScore.dwPlayTimeCount;

		//������Ϣ
		WriteGameScore.lRevenue=VariationScore.lRevenue;
		WriteGameScore.lUnLockScore=VariationScore.lUnLockScore;
		
		//������Ϣ
		WriteGameScore.lScore=VariationScore.VariationScore.lScore;		
		WriteGameScore.lWinCount=VariationScore.VariationScore.lWinCount;
		WriteGameScore.lLostCount=VariationScore.VariationScore.lLostCount;
		WriteGameScore.lDrawCount=VariationScore.VariationScore.lDrawCount;
		WriteGameScore.lFleeCount=VariationScore.VariationScore.lFleeCount;
		WriteGameScore.lExperience=VariationScore.VariationScore.lExperience;
		WriteGameScore.lBetAmount= VariationScore.VariationScore.lBetAmount;
		WriteGameScore.lWinStreak=VariationScore.VariationScore.lWinStreak;
		WriteGameScore.lMaxWinStreak=VariationScore.VariationScore.lMaxWinStreak;

		//��ҷ���
		if (m_pGameServiceOption->wServerType & GAME_GENRE_GOLD)
		{
			WriteGameScore.wAddupWinRate = pIServerUserItem->GetAddupWinRate();
			WriteGameScore.lAddupWinScore = pIServerUserItem->GetAddupWinScore();
			WriteGameScore.lAddupLoseScore = pIServerUserItem->GetAddupLoseScore();
		}

		//��������
		if (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)
		{
			WriteGameScore.lMatchNo=pIServerUserItem->GetMatchNo();
			WriteGameScore.dwMatchID=pIServerUserItem->GetMatchID();			
		}

		//������Ϣ
		WriteGameScore.dwInoutIndex = pIServerUserItem->GetInoutIndex();

		//Ͷ������
		if (pIServerUserItem->IsRobotUser() == false)
		{
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_GAME_SCORE, WriteGameScore.dwUserID, MAKELONG(wBindIndex, 0), pIServerUserItem->GetTokenID(), &WriteGameScore, sizeof(WriteGameScore));
		}

		//���͵ͱ�(�����ý�ҷ�)
		if (m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD) && CServerRule::IsAllowLargessSubsidy(m_pGameServiceOption->dwServerRule)==true)
		{
			//�����ж�
			if (pIServerUserItem->IsMobileUser()==true && pIServerUserItem->GetResidualTimes()>0 && pIServerUserItem->IsLargessSubsidy()==false)
			{
				//�����ж�
				if (pIServerUserItem->GetSubsidyScore()<m_SystemParameter.lTakeSubsidyCondition)
				{
					//���ñ�ʶ
					pIServerUserItem->SetLargessSubsidy(true);

					//��������
					DBR_GR_LargessSubsidy LargessSubsidy;
					ZeroMemory(&LargessSubsidy,sizeof(LargessSubsidy));

					//���ñ���
					LargessSubsidy.dwUserID=pIServerUserItem->GetUserID();

					//������Ϣ
					LargessSubsidy.dwClientIP=pIServerUserItem->GetClientIP();			
					StringCchCopy(LargessSubsidy.szMachineID,CountArray(LargessSubsidy.szMachineID), pIServerUserItem->GetMachineID());

					//Ͷ������
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LARGESS_SUBSIDY, LargessSubsidy.dwUserID,MAKELONG(wBindIndex,0), pIServerUserItem->GetTokenID(),&LargessSubsidy,sizeof(LargessSubsidy));
				}
			}
		}
	}	

	return true;
}

//�û�״̬
bool CAttemperEngineSink::OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	CMD_GR_UserStatus UserStatus;
	auto pUserInfo=pIServerUserItem->GetUserInfo();

	//��������
	UserStatus.dwUserID=pUserInfo->dwUserID;
	UserStatus.UserStatus.wTableID=pUserInfo->wTableID;
	UserStatus.UserStatus.wChairID=pUserInfo->wChairID;
	UserStatus.UserStatus.cbUserStatus=pUserInfo->cbUserStatus;

	//��������
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();

	//�����ж�
	if (wTableID!=INVALID_TABLE && pIServerUserItem->GetUserStatus()<= US_LOOKON)
	{		
		if (wTableID!=wLastTableID)
		{
			//����ͬ���û�
			SendTableUserToUser(pIServerUserItem, CLIENT_KIND_MOBILE, INVALID_DWORD);

			//���͸�ͬ���û�
			SendUserToTableUser(pIServerUserItem, CLIENT_KIND_MOBILE, INVALID_DWORD);
		}	
	}

	//��������
	SendBatchData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));

	if (pIServerUserItem->GetUserStatus() >= US_SIT)
	{
		SendDataToTable(wTableID, CLIENT_KIND_MOBILE, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
	}
	else
	{
		//����״̬
		if (pIServerUserItem->IsMobileUser())
		{
			SendDataToUser(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
		}

		SendDataToTable(wLastTableID, CLIENT_KIND_MOBILE, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
	}	

	//�뿪�ж�
	if (pUserInfo->cbUserStatus==US_NULL)
	{
		//�û��ǳ�
		OnEventUserLogout(pIServerUserItem);		
	}

	return true;
}

//�û���λ
bool CAttemperEngineSink::OnEventUserItemRanking(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	return true;
}

//�û�Ȩ��
bool CAttemperEngineSink::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	auto pUserInfo=pIServerUserItem->GetUserInfo();

	DBR_GR_ManageUserRight ManageUserRight= {0};
	ManageUserRight.dwUserID = pUserInfo->dwUserID;
	ManageUserRight.dwAddRight = dwAddRight;
	ManageUserRight.dwRemoveRight = dwRemoveRight;

	//��������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MANAGE_USER_RIGHT, ManageUserRight.dwUserID, 0, 0, &ManageUserRight,sizeof(ManageUserRight));

	return true;
}

//�û�����
bool CAttemperEngineSink::OnEventUserFreezeScore(IServerUserItem *pIServerUserItem, SCORE lFreezeScore)
{
	SCORE lScore = 0;
	if (lFreezeScore == 0 && m_FreezeScoreMap.Lookup(pIServerUserItem->GetUserID(),lScore) )
	{
		m_FreezeScoreMap.RemoveKey(pIServerUserItem->GetUserID());
	}
	else
	{
		m_FreezeScoreMap[pIServerUserItem->GetUserID()]=lFreezeScore;
	}

	return true;
}

//�û��Ƹ�
VOID CAttemperEngineSink::OnEventUserItemWealth(IServerUserItem* pIServerUserItem, tagWealthVariation& WealthVariation)
{
	//Ч�����
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return;

	//ͬ���ͻ��� 
	if (WealthVariation.cbSyncTarget & SYNC_TARGET_CLIENT)
	{		
		//���嶨��
		BYTE cbBuffer[1024] = { 0 };

		//����Ƹ�
		CMD_CM_WealthUpdate* pWealthUpdate = (CMD_CM_WealthUpdate*)cbBuffer;
		ZeroMemory(pWealthUpdate, sizeof(CMD_CM_WealthUpdate));

		//���ñ���
		pWealthUpdate->wItemCount = 1;
		pWealthUpdate->WealthItem[0].lBalance = WealthVariation.lBalance;
		pWealthUpdate->WealthItem[0].wMoneyID = WealthVariation.wMoneyID;

		//������Ϣ
		WORD wHeadSize = sizeof(CMD_CM_WealthUpdate);
		WORD wDataSize = pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]);
		SendDataToUser(pIServerUserItem, MDM_CM_SYSTEM, SUB_CM_WEALTH_UPDATE, pWealthUpdate, wHeadSize + wDataSize);
	}	

	//ͬ�����ݿ�
	if (WealthVariation.cbSyncTarget & SYNC_TARGET_DATABASE)
	{
		//��������
		DBR_GR_WriteGameWealth WriteGameWealth;
		ZeroMemory(&WriteGameWealth, sizeof(WriteGameWealth));

		//�Ƹ���Ϣ
		WriteGameWealth.dwUserID = pIServerUserItem->GetUserID();
		WriteGameWealth.wReason = WealthVariation.wReason;
		WriteGameWealth.wMoneyID = WealthVariation.wMoneyID;
		WriteGameWealth.lVariationScore = WealthVariation.lVariationScore;

		//Ͷ������
		if (pIServerUserItem->IsRobotUser() == false)
		{
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_USER_WEALTH, WriteGameWealth.dwUserID, pIServerUserItem->GetBindIndex(), pIServerUserItem->GetTokenID(), &WriteGameWealth, sizeof(WriteGameWealth));
		}
	}

}

//�û��ȼ�
VOID CAttemperEngineSink::OnEventUserUserLevel(IServerUserItem* pIServerUserItem, BYTE cbLastLevelID, BYTE cbCurrLevelID)
{
	//Ч�����
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return;

	//���嶨��
	BYTE cbBuffer[SOCKET_PACKET] = { 0 };

	//��ȡ����
	auto  pServerUserPack = pIServerUserItem->GetUserPack();

	//�������
	CMD_GR_UserLevelUpgrade* pUserLevelUpgrade = (CMD_GR_UserLevelUpgrade*)cbBuffer;
	CSendPacketHelper SendPacket(cbBuffer + sizeof(CMD_GR_UserLevelUpgrade), sizeof(cbBuffer) - sizeof(CMD_GR_UserLevelUpgrade));

	//ѭ������
	for (auto LevelID = cbLastLevelID + 1; LevelID <= cbCurrLevelID; LevelID++)
	{
		//�������
		SendPacket.CleanData();

		//��ѯ����
		auto pUserLevelItem = g_GameLevelManager->SearchUserLevel(LevelID);
		if (pUserLevelItem != NULL)
		{
			//���ñ���
			pUserLevelUpgrade->cbMemberOrder = LevelID;
			pUserLevelUpgrade->dwUserID = pIServerUserItem->GetUserID();
			pUserLevelUpgrade->dwMemberPoint = pUserLevelItem->lNeedPoint;

			//��������
			if (LevelID == cbCurrLevelID)
			{
				pUserLevelUpgrade->dwMemberPoint = pIServerUserItem->GetMemberPoint();
			}

			//������Ϣ
			SendPacket.AddPacket(pUserLevelItem->szRewards, DTP_GR_GOODSLIST);

			//��Ʒ�б�
			CGoodsItemArray  MoneyGoodsList;
			CGoodsItemArray  NormalGoodsList;

			//������Ʒ
			if (g_GameGoodsManager->ParseFromJson(pUserLevelItem->szRewards, MoneyGoodsList, NormalGoodsList))
			{
				//��ʼ����
				pServerUserPack->BeginTrans();

				//ʣ����Ʒ
				WORD wResidueCount = 0;
				tagGoodsItem ResidueGoodsList[12] = { 0 };

				//���뱳��
				for (auto i = 0; i < NormalGoodsList.GetCount(); i++)
				{
					auto pGoodsItem = &NormalGoodsList[i];
					pServerUserPack->AppendGoods(pGoodsItem->wGoodsID, pGoodsItem->dwGoodsCount, pGoodsItem->wGoodsLevel, pGoodsItem->dwGoodsIndate, GOODS_REASON_USERLEVEL_UPGRADE);
				}

				//�ύ����
				pServerUserPack->SubmitTrans();

				//��ӲƸ�
				for (auto i = 0; i < MoneyGoodsList.GetCount(); i++)
				{
					pIServerUserItem->WriteUserWealth(MoneyGoodsList[i].wGoodsID, MoneyGoodsList[i].dwGoodsCount, GOODS_REASON_USERLEVEL_UPGRADE);
				}				
			}

			//��������
			WORD wDataSize = sizeof(CMD_GR_UserLevelUpgrade) + SendPacket.GetDataSize();
			SendDataToUser(pIServerUserItem, MDM_GR_USER, SUB_GR_USERLEVEL_UPGRADE, pUserLevelUpgrade, wDataSize);
		}
	}

	return;
}

//��Ϸ�ȼ�
VOID CAttemperEngineSink::OnEventUserGameLevel(IServerUserItem* pIServerUserItem, BYTE cbLastLevelID, BYTE cbCurrLevelID)
{
	//Ч�����
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return;

	//���嶨��
	BYTE cbBuffer[SOCKET_PACKET] = { 0 };

	//��ȡ����
	auto  pServerUserPack = pIServerUserItem->GetUserPack();

	//�������
	CMD_GR_GameLevelUpgrade* pGameLevelUpgrade = (CMD_GR_GameLevelUpgrade*)cbBuffer;
	CSendPacketHelper SendPacket(pGameLevelUpgrade + 1, sizeof(cbBuffer) - sizeof(CMD_GR_GameLevelUpgrade));

	//ѭ������
	for (auto LevelID = cbLastLevelID + 1; LevelID <= cbCurrLevelID; LevelID++)
	{
		//��ѯ����
		auto pGameLevelItem = g_GameLevelManager->SearchGameLevel(LevelID);
		if (pGameLevelItem != NULL)
		{
			//���ñ���
			pGameLevelUpgrade->cbGameOrder = LevelID;
			pGameLevelUpgrade->dwUserID = pIServerUserItem->GetUserID();
			pGameLevelUpgrade->lExperience = pGameLevelItem->lNeedPoint;

			//��������
			if (LevelID == cbCurrLevelID)
			{
				pGameLevelUpgrade->lExperience = pIServerUserItem->GetExperience();
			}

			//������Ϣ
			SendPacket.AddPacket(pGameLevelItem->szRewards, DTP_GR_GOODSLIST);

			//��Ʒ�б�
			CGoodsItemArray  MoneyGoodsList;
			CGoodsItemArray  NormalGoodsList;

			//������Ʒ
			if (g_GameGoodsManager->ParseFromJson(pGameLevelItem->szRewards, MoneyGoodsList, NormalGoodsList))
			{
				//��ʼ����
				pServerUserPack->BeginTrans();

				//ʣ����Ʒ
				WORD wResidueCount = 0;
				tagGoodsItem ResidueGoodsList[12] = { 0 };

				//���뱳��
				for (auto i = 0; i < NormalGoodsList.GetCount(); i++)
				{
					auto pGoodsItem = &NormalGoodsList[i];
					pServerUserPack->AppendGoods(pGoodsItem->wGoodsID, pGoodsItem->dwGoodsCount, pGoodsItem->wGoodsLevel, pGoodsItem->dwGoodsIndate, GOODS_REASON_GAMELEVEL_UPGRADE);			
				}

				//�ύ����
				pServerUserPack->SubmitTrans();

				//��ӲƸ�
				for (auto i = 0; i < MoneyGoodsList.GetCount(); i++)
				{
					pIServerUserItem->WriteUserWealth(MoneyGoodsList[i].wGoodsID, MoneyGoodsList[i].dwGoodsCount, GOODS_REASON_GAMELEVEL_UPGRADE);
				}				
			}

			//��������
			WORD wDataSize = sizeof(CMD_GR_GameLevelUpgrade) + SendPacket.GetDataSize();
			SendDataToUser(pIServerUserItem, MDM_GR_USER, SUB_GR_GAMELEVEL_UPGRADE, pGameLevelUpgrade, wDataSize);
		}
	}

	return;
}

//�û�����
VOID CAttemperEngineSink::OnEventUserItemPack(IServerUserItem* pIServerUserItem, WORD wGoodsIDList[], WORD wGoodsCount)
{
	//Ч�����
	ASSERT (pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return;

	//�û�����
	auto pServerUserPack = pIServerUserItem->GetUserPack();

	//���嶨��
	BYTE cbBuffer[SOCKET_PACKET] = { 0 };

	//�������
	auto pUserGoodsUpdate = (CMD_GR_UserGoodsUpdate*)cbBuffer;
	ZeroMemory(pUserGoodsUpdate, sizeof(CMD_GR_UserGoodsUpdate));

	//��������
	pUserGoodsUpdate->wGoodsCount = wGoodsCount;

	//�����Ʒ
	for (WORD i = 0; i < wGoodsCount; i++)
	{
		auto pGoodsHoldInfo = pServerUserPack->SearchGoods(wGoodsIDList[i]);
		if (pGoodsHoldInfo != NULL)
		{
			CopyMemory(&pUserGoodsUpdate->GoodsHoldInfo[i], pGoodsHoldInfo, sizeof(pUserGoodsUpdate->GoodsHoldInfo[0]));
		}
		else
		{
			//��ѯ��Ʒ
			auto GameGoodsItemPtr = g_GameGoodsManager->SearchGoodsItem<CGameGoodsItemPtr>(wGoodsIDList[i]);
			if (GameGoodsItemPtr)
			{
				pUserGoodsUpdate->GoodsHoldInfo[i].wGoodsLevel = 1;
				pUserGoodsUpdate->GoodsHoldInfo[i].wGoodsID = GameGoodsItemPtr->GetGoodsID();
				pUserGoodsUpdate->GoodsHoldInfo[i].wGoodsKind = GameGoodsItemPtr->GetGoodsKind();
			}
		}
	}

	//��������
	WORD wHeadSize = sizeof(CMD_GR_UserGoodsUpdate);
	WORD wDataSize = pUserGoodsUpdate->wGoodsCount * sizeof(pUserGoodsUpdate->GoodsHoldInfo[0]);
	SendDataToUser(pIServerUserItem, MDM_CM_SYSTEM, SUB_CM_GOODS_UPDATE, pUserGoodsUpdate, wHeadSize + wDataSize);

	//�������
	if (pServerUserPack->IsSubmitVariation() == true)
	{
		//�������
		DBR_GR_UserUpdateGoods UserUpdateGoods;
		ZeroMemory(&UserUpdateGoods, sizeof(UserUpdateGoods));

		//�û���ʶ
		UserUpdateGoods.dwUserID = pIServerUserItem->GetUserID();

		//��ȡ����
		if (pServerUserPack->DistillVariationGoods(UserUpdateGoods.szGoodsList, CountArray(UserUpdateGoods.szGoodsList)))
		{
			//Ͷ������	
			WORD wDataSize = CountStringBuffer(UserUpdateGoods.szGoodsList);
			WORD wHeadSize = sizeof(UserUpdateGoods) - sizeof(UserUpdateGoods.szGoodsList);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_UPDATE_GOODS, UserUpdateGoods.dwUserID, pIServerUserItem->GetBindIndex(), pIServerUserItem->GetTokenID(), &UserUpdateGoods, wHeadSize + wDataSize);
		}

		//�������
		DBR_GR_UserRecordGoods UserRecordGoods;
		ZeroMemory(&UserRecordGoods, sizeof(UserRecordGoods));

		//�û���ʶ
		UserRecordGoods.dwUserID = pIServerUserItem->GetUserID();

		//��ȡ����
		if (pServerUserPack->DistillVariationRecord(UserRecordGoods.szRecordList, CountArray(UserRecordGoods.szRecordList)))
		{
			//Ͷ������	
			WORD wDataSize = CountStringBuffer(UserRecordGoods.szRecordList);
			WORD wHeadSize = sizeof(UserRecordGoods) - sizeof(UserRecordGoods.szRecordList);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_RECORD_GOODS, UserRecordGoods.dwUserID, pIServerUserItem->GetBindIndex(), pIServerUserItem->GetTokenID(), &UserRecordGoods, wHeadSize + wDataSize);
		}

		//������
		pServerUserPack->CleanVariation();
	}
}

//��ȡ״̬
WORD CAttemperEngineSink::GetServiceStatus()
{
	return CServiceUnits::g_pServiceUnits->GetServiceStatus();
}

//��������
bool CAttemperEngineSink::SendBatchData(BYTE cbSendMask,WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	if ((cbSendMask&BG_COMPUTER)!=0)
	{
		m_RobotUserManager.SendDataToClient(wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	//��������
	WORD wEnumIndex = 0;
	IServerUserItem * pIServerUserItem = NULL;
	CGlobalGateItem * pGlobalGateItem = NULL;

	do
	{
		//ö�����
		pIServerUserItem = m_ServerUserManager.EnumUserItem(wEnumIndex++);
		if (pIServerUserItem == NULL) break;
		if (pIServerUserItem->GetBindIndex() == INVALID_WORD_INDEX) continue;

		//�������
		if (cbSendMask != BG_ALL_CLIENT)
		{
			if ((cbSendMask&BG_MOBILE) != 0 && !pIServerUserItem->IsMobileUser()) continue;
			if ((cbSendMask&BG_COMPUTER) != 0 && (pIServerUserItem->IsMobileUser() || pIServerUserItem->IsRobotUser())) continue;
		}

		//��ȡ����
		tagBindParameter * pBindParameter = (m_pNormalParameter + pIServerUserItem->GetBindIndex());

		//ֱ��
		if (pBindParameter->wServiceModule == SERVICE_MODULE_NONE)
		{
			//�ر�����
			if (wSubCmdID == SUB_CS_S_SHUTDOWN_SOCKET)
			{
				m_pITCPNetworkEngine->ShutDownSocket(pBindParameter->dwContextID);
			}
			else
			{
				//��������
				m_pITCPNetworkEngine->SendData(pBindParameter->dwContextID, wMainCmdID, wSubCmdID, pData, wDataSize, NULL, 0);
			}
		}
		else
		{
			pGlobalGateItem = pBindParameter->pGlobalGateItem;
			if (pGlobalGateItem != NULL)
			{
				pGlobalGateItem->m_TokenIDArray.Add(pIServerUserItem->GetTokenID());
			}
		}

	} while (true);

	//��������		
	DWORD * pdwTokenIDArray = NULL;
	WORD wCellCount = 0, wSendCount = 0, wTotalCount = 0;

	//���ñ���	
	pGlobalGateItem = NULL;

	do
	{
		//ö������
		pGlobalGateItem = m_GlobalItemManager.EnumGateItem(pGlobalGateItem);
		if (pGlobalGateItem == NULL) break;

		//У����Ŀ
		if (pGlobalGateItem->m_TokenIDArray.GetCount() == 0) continue;

		//���ñ���		
		wSendCount = 0;
		pdwTokenIDArray = pGlobalGateItem->m_TokenIDArray.GetData();
		wTotalCount = wCellCount = pGlobalGateItem->m_TokenIDArray.GetCount();

		//�з�����
		while (wCellCount > TOKEN_COUNT) wCellCount = wCellCount / 2;

		//ѭ������
		while (wSendCount < wTotalCount)
		{
			//��������
			if (wTotalCount - wSendCount < wCellCount)
			{
				wCellCount = wTotalCount - wSendCount;
			}

			//��������
			m_pITCPNetworkEngine->SendData(pGlobalGateItem->GetSocketID(), MAKECMMD(wMainCmdID, SERVICE_MODULE_GAME), wSubCmdID, pData, wDataSize, pdwTokenIDArray + wSendCount, wCellCount);

			//�ۼ�����
			wSendCount += wCellCount;
		}		

		//�������
		pGlobalGateItem->m_TokenIDArray.RemoveAll();

	} while (true);

	return true;
}

//��������
bool CAttemperEngineSink::SendDebugData(IServerUserItem* pIServerUserItem, WORD wTableID, WORD wDebugKind, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	//���嶨��
	BYTE cbBuffer[SOCKET_PACKET] = {0};

	//����ṹ
	auto pDebugPacket = (CMD_GR_DebugPacket*)cbBuffer;

	//���ñ���
	pDebugPacket->wTableID = wTableID;
	pDebugPacket->wDebugKind = wDebugKind;
	pDebugPacket->wGameCmdID = wSubCmdID;

	//���У��
	if (wDataSize + sizeof(CMD_GR_DebugPacket) > sizeof(cbBuffer))
	{
		return false;
	}

	//��������
	if (wDataSize > 0)
	{
		CopyMemory(pDebugPacket + 1, pData, wDataSize);
	}

	//���ݴ�С
	WORD wPacketSize = sizeof(CMD_GR_DebugPacket) + wDataSize;

	//У���û�
	if (pIServerUserItem != NULL )
	{
		if (CUserRight::CanCheat(pIServerUserItem->GetUserRight()))
		{
			SendDataToUser(pIServerUserItem,MDM_GR_DEBUG,SUB_GR_DEBUG_PACKET, pDebugPacket, wPacketSize);
		}
	}
	else
	{
		//��������
		WORD wEnumIndex = 0;

		do
		{
			//�����û�
			auto pIServerUserItem = m_ServerUserManager.EnumUserItem(wEnumIndex++);
			if (pIServerUserItem == NULL) break;

			//У��״̬
			if (pIServerUserItem->IsMobileUser() || pIServerUserItem->IsRobotUser()) continue;

			//У��Ȩ��
			if (CUserRight::CanCheat(pIServerUserItem->GetUserRight()))
			{
				SendDataToUser(pIServerUserItem, MDM_GR_DEBUG, SUB_GR_DEBUG_PACKET, pDebugPacket, wPacketSize);
			}

		} while (true);
	}		

	return true;
}

//��������
bool CAttemperEngineSink::SendDataToGate(DWORD dwSocketID, DWORD dwTokenID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��ȡ����
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);

	//��������
	if (wBindIndex >=INDEX_ROBOT)
	{
		//�����û�
		m_RobotUserManager.SendDataToClient(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
	}
	else 
	{
		//ֱ��ģʽ
		if (pBindParameter->wServiceModule == SERVICE_MODULE_NONE)
		{
			if (wSubCmdID == SUB_CS_S_SHUTDOWN_SOCKET)
			{
				//�ر�����
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
			}
			else
			{
				//��������
				m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID, pData, wDataSize, &dwTokenID, 0);
			}
		}
		else
		{			
			//��������
			m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(wMainCmdID,SERVICE_MODULE_GAME), wSubCmdID, pData, wDataSize, &dwTokenID, dwTokenID ? 1 : 0);
		}
	}

	return true;
}

//��������
bool CAttemperEngineSink::SendDataToUser(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//У������
	if (pIServerUserItem->GetBindIndex()==INVALID_WORD_INDEX) return false;

	//��ȡ����
	DWORD dwTokenID = pIServerUserItem->GetTokenID();
	tagBindParameter * pBindParameter = GetBindParameter(pIServerUserItem->GetBindIndex());

	//��������
	if (pIServerUserItem->IsRobotUser()==true)
	{
		//�����û�			
		m_RobotUserManager.SendDataToClient(pBindParameter->dwContextID,wMainCmdID,wSubCmdID,pData,wDataSize);
	}
	else
	{
		//�ر�����
		if (pBindParameter->wServiceModule == SERVICE_MODULE_NONE)
		{
			if (wSubCmdID == SUB_CS_S_SHUTDOWN_SOCKET)
			{
				//�ر�����
				m_pITCPNetworkEngine->ShutDownSocket(pBindParameter->dwContextID);
			}
			else
			{
				//��������
				m_pITCPNetworkEngine->SendData(pBindParameter->dwContextID, wMainCmdID, wSubCmdID, pData, wDataSize, &dwTokenID, 0);
			}
		}
		else
		{
			//�����û�							
			m_pITCPNetworkEngine->SendData(pBindParameter->dwContextID, MAKECMMD(wMainCmdID,SERVICE_MODULE_GAME),wSubCmdID, pData, wDataSize, &dwTokenID, dwTokenID ? 1 : 0);
		}
	}

	return true;
}

//��������
bool CAttemperEngineSink::SendDataToCenter(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	ASSERT(m_pITCPSocketCenter != NULL);
	return m_pITCPSocketCenter->SendData(wMainCmdID, wSubCmdID, pData, wDataSize,0);
}

//Ⱥ������
bool CAttemperEngineSink::SendDataToTable(WORD wCmdTable, BYTE cbClientKind, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//У�����
	//ASSERT(wCmdTable < m_TableFrameArray.GetCount());
	if (wCmdTable >= m_TableFrameArray.GetCount()) return false;

	//ö���û�
	WORD wEnumIndex=0;
	IServerUserItem * pITableUserItem = NULL;
	CTableFrameItem* pTableFrame = m_TableFrameArray[wCmdTable];

	//�����û�
	for (int i = 0; i < pTableFrame->GetChairCount(); i++)
	{
		pITableUserItem = pTableFrame->GetTableUserItem(i);
		if (pITableUserItem == NULL) continue;	

		//��չ����
		auto pExtendParameter = GetExtendParameter(pITableUserItem->GetBindIndex(), pITableUserItem->GetTokenID());
		if (pExtendParameter != NULL && (pExtendParameter->cbClientKind & cbClientKind) != 0)
		{
			SendDataToUser(pITableUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
		}
	}

	//�Թ��û�
	do
	{
		//ö���û�
		pITableUserItem = pTableFrame->EnumLookonUserItem(wEnumIndex++);
		if (pITableUserItem == NULL) break;		
		
		//��չ����
		auto pExtendParameter = GetExtendParameter(pITableUserItem->GetBindIndex(), pITableUserItem->GetTokenID());
		if (pExtendParameter != NULL && (pExtendParameter->cbClientKind & cbClientKind) != 0)
		{			
			SendDataToUser(pITableUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
		}

	} while (true);

	return true;
}

//�������¼�
VOID CAttemperEngineSink::OnEventTableLocked( ITableFrameItem *pITableFrame, BOOL bTableLocked )
{
	//Ч��
	ASSERT(pITableFrame);

	if( bTableLocked )
	{
		DWORD dwTableTimes=0L;
		if( m_LockedTableTimes.Lookup(pITableFrame->GetTableID(),dwTableTimes) == FALSE )
		{
			m_LockedTableTimes[pITableFrame->GetTableID()] = m_dwRegularTimes+m_dwLockTableTime;
		}
	}

	return;
}

//����״̬�¼�
VOID CAttemperEngineSink::OnEventTableStatus( ITableFrameItem *pITableFrame, BOOL bPlayStatus )
{
	//Ч��
	ASSERT(pITableFrame);

	if( !bPlayStatus )
	{
		//�ж�����
		if( pITableFrame->IsTableLocked() )
		{
			//���¼�ʱ
			m_LockedTableTimes[pITableFrame->GetTableID()] = m_dwRegularTimes+m_dwLockTableTime;
		}
	}
	else
	{
		//ɾ����ʱ
		m_LockedTableTimes.RemoveKey(pITableFrame->GetTableID());
	}

	return;
}

//����д������
VOID CAttemperEngineSink::OnTableReqOfflineScore( tagOfflineScore const &OfflineScore )
{
	//��ѯ����Ƿ�����
	IServerUserItem *pUserItem = m_ServerUserManager.SearchUserItem(OfflineScore.dwUserID);
	
	//����������
	if (pUserItem)
	{
		//�ⶳ����
		pUserItem->FreezeUserScore(OfflineScore.lUnLockScore,true);

		//д��
		pUserItem->WriteUserScore(OfflineScore.lScore, OfflineScore.lWinScore, OfflineScore.lBetAmount, OfflineScore.lRevenue,OfflineScore.ScoreType);

		return;
	}

	//�ⶳ���ּ�¼
	SCORE lFreezeScore = 0;
	if( m_FreezeScoreMap.Lookup(OfflineScore.dwUserID,lFreezeScore) )
	{
		if( lFreezeScore == OfflineScore.lUnLockScore )
			m_FreezeScoreMap.RemoveKey(OfflineScore.dwUserID);
		else m_FreezeScoreMap[OfflineScore.dwUserID] = lFreezeScore-OfflineScore.lUnLockScore;
	}

	//д������
	DBR_GR_WriteGameScore WriteGameScore;
	ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

	//�û���Ϣ
	WriteGameScore.dwUserID=OfflineScore.dwUserID;
	WriteGameScore.dwClientIP=OfflineScore.dwClientIP;
	WriteGameScore.dwPlayTimeCount=OfflineScore.dwPlayTimeCount;

	//������Ϣ
	WriteGameScore.lRevenue=OfflineScore.lRevenue;
	WriteGameScore.lUnLockScore=OfflineScore.lUnLockScore;
	WriteGameScore.lScore=OfflineScore.lScore;
	WriteGameScore.lBetAmount=OfflineScore.lBetAmount;
	WriteGameScore.lWinCount=OfflineScore.lWinCount;
	WriteGameScore.lLostCount=OfflineScore.lLostCount;
	WriteGameScore.lDrawCount=OfflineScore.lDrawCount;
	WriteGameScore.lFleeCount=OfflineScore.lFleeCount;
	WriteGameScore.lExperience=OfflineScore.lExperience;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_GAME_SCORE, WriteGameScore.dwUserID,0L,0L,&WriteGameScore,sizeof(WriteGameScore));

	return;
}

//�ⶳ��������
VOID CAttemperEngineSink::OnTableReqUnfreezeScore( DWORD dwUserID, SCORE lUnfreezeScore )
{
	//��ѯ����Ƿ�����
	IServerUserItem *pUserItem = m_ServerUserManager.SearchUserItem(dwUserID);

	//�������
	if( pUserItem )
	{
		pUserItem->FreezeUserScore(-lUnfreezeScore,true);
		return;
	}

	//����ⶳ����
	SCORE lFreezeScore = 0;
	if( m_FreezeScoreMap.Lookup(dwUserID,lFreezeScore) )
	{
		if( lFreezeScore == lUnfreezeScore )
			m_FreezeScoreMap.RemoveKey(dwUserID);
		else m_FreezeScoreMap[dwUserID] = lFreezeScore-lUnfreezeScore;

		return;
	}

	//�Ҳ������ּ�¼
	ASSERT(FALSE);
	return;
}


//������Ϣ
bool CAttemperEngineSink::SendRoomMessage(DWORD dwSocketID, DWORD dwTokenID, LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	StringCchCopy(SystemMessage.szString,CountArray(SystemMessage.szString), lpszMessage);

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	SendDataToGate(dwSocketID, dwTokenID, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wSendSize);

	//�ر�����
	if ((wType&(SMT_CLOSE_LINK | SMT_CLOSE_ROOM)) != 0)
	{
		//����ṹ
		CMD_CS_C_ShutdownSocket ShutdownSocket;
		ShutdownSocket.bForceClose = false;

		//��������
		SendDataToGate(dwSocketID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::SendRoomMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	StringCchCopy(SystemMessage.szString,CountArray(SystemMessage.szString), lpszMessage);

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������		
	SendBatchData(BG_COMPUTER,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

	//�ر�����
	if ((wType&(SMT_CLOSE_LINK | SMT_CLOSE_ROOM)) != 0)
	{
		//����ṹ
		CMD_CS_C_ShutdownSocket ShutdownSocket;
		ShutdownSocket.bForceClose = false;

		//��������
		SendBatchData(BG_COMPUTER,MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));		
	}

	return true;
}

//��Ϸ��Ϣ
bool CAttemperEngineSink::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem *pIServerUserItem = NULL;

	do 
	{
		pIServerUserItem = m_ServerUserManager.EnumUserItem(wEnumIndex++);
		if (pIServerUserItem == NULL) break;
		
		SendGameMessage(pIServerUserItem, lpszMessage, wType);
		
	} while (true);

	return true;
}

//������Ϣ
bool CAttemperEngineSink::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//У������
	if (pIServerUserItem->GetBindIndex() == INVALID_WORD_INDEX) return false;

	//��������
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//��������
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	StringCchCopy(SystemMessage.szString,CountArray(SystemMessage.szString), lpszMessage);

	//��������
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//��������
	SendDataToUser(pIServerUserItem, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wSendSize);

	//�ر�����
	if ((wType&(SMT_CLOSE_LINK | SMT_CLOSE_ROOM)) != 0)
	{
		//����ṹ
		CMD_CS_C_ShutdownSocket ShutdownSocket;
		ShutdownSocket.bForceClose = false;

		//��������
		SendDataToUser(pIServerUserItem, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));		
	}

	return true;
}

//��Ϸ��Ϣ
bool CAttemperEngineSink::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//Ч�����
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//У������
	if (pIServerUserItem->GetBindIndex() == INVALID_WORD_INDEX) return false;

	//��������
	if (pIServerUserItem->IsClientReady()==true)
	{
		//��������
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//��������
		SystemMessage.wType=wType;
		SystemMessage.wLength=lstrlen(lpszMessage)+1;
		StringCchCopy(SystemMessage.szString,CountArray(SystemMessage.szString), lpszMessage);

		//��������
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//��������
		SendDataToUser(pIServerUserItem, MDM_GF_FRAME, SUB_GF_SYSTEM_MESSAGE, &SystemMessage, wSendSize);		

		//�ر�����
		if ((wType&(SMT_CLOSE_LINK | SMT_CLOSE_ROOM)) != 0)
		{
			//����ṹ
			CMD_CS_C_ShutdownSocket ShutdownSocket;
			ShutdownSocket.bForceClose = false;

			//��������
			SendDataToUser(pIServerUserItem, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));
		}

		return true;
	}

	return false;
}

//����ƹ���
bool CAttemperEngineSink::SendMarqeeNotice(BYTE cbNoticeKind, VOID* pNoticeContent, WORD wNoticeSize)
{
	//��������
	auto MarqueeNoticeOptionPtr = g_GlobalOptionManager->GetOptionItem<CMarqueeNoticeOptionPtr>(OPTION_MASK_MARQUEE_NOTICE);
	if (!MarqueeNoticeOptionPtr) return false;

	//��������
	auto pNoticeRollOption = MarqueeNoticeOptionPtr->GetNoticeRollOption(cbNoticeKind);
	if (!pNoticeRollOption) return false;

	//���嶨��
	BYTE cbBuffer[1024];

	//����ṹ
	tagForwardHead* pForwadHead = (tagForwardHead*)cbBuffer;
	auto pMarqueeNotice = (CMD_CS_MarqueeNotice*)(pForwadHead + 1);

	//ת������
	pForwadHead->wItemCount = 1;
	pForwadHead->wTarget = FORWARD_TARGET_SERVICE;	
	pForwadHead->ForwardItem[0].wServiceID = 0;
	pForwadHead->ForwardItem[0].wServiceKind = 0;
	pForwadHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;

	//��������
	pMarqueeNotice->dwNoticeID = GetTickCount();
	pMarqueeNotice->bExclusive = pNoticeRollOption->bExclusive;
	pMarqueeNotice->cbPriority = pNoticeRollOption->cbPriority;
	pMarqueeNotice->wRollTimes = pNoticeRollOption->wRollTimes;
	pMarqueeNotice->wViewPlace = pNoticeRollOption->wViewPlace;
	pMarqueeNotice->cbNoticeKind = pNoticeRollOption->cbNoticeKind;
	pMarqueeNotice->dwStartRollTime = (DWORD)time(NULL);
	pMarqueeNotice->dwEndRollTime = pMarqueeNotice->dwStartRollTime + MarqueeNoticeOptionPtr->GetDurationTime();

	//��������
	pMarqueeNotice->wNoticeSize = wNoticeSize;
	CopyMemory(pMarqueeNotice->cbNoticeContent, pNoticeContent, pMarqueeNotice->wNoticeSize);

	//��������
	WORD wPacketSize = sizeof(CMD_CS_MarqueeNotice) - sizeof(pMarqueeNotice->cbNoticeContent) + pMarqueeNotice->wNoticeSize;
	SendDataToCenter(MDM_CS_FORWARD_SERVICE, SUB_CS_MARQUEE_NOTICE, cbBuffer, sizeof(tagForwardHead) + wPacketSize);

	return true;
}

//ע�����
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_REGISTER_SERVICE:	//ע�����
		{
			//Ч������
			ASSERT(wDataSize == sizeof(CMD_CS_C_RegisterService));
			if (wDataSize != sizeof(CMD_CS_C_RegisterService)) return false;

			//��Ϣ����
			CMD_CS_C_RegisterService * pRegisterService = (CMD_CS_C_RegisterService *)pData;

			//У��ģ��
			if (pRegisterService->wServiceModule != SERVICE_MODULE_GATE) return false;

			//����·��
			if (m_GlobalItemManager.SearchGateItem(pRegisterService->wServiceID) != NULL)
			{
				//��������
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure, sizeof(RegisterFailure));

				//���ñ���
				RegisterFailure.lErrorCode = 0;
				_sntprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), TEXT("�Ѿ�������ͬ��ʶ��%s������ע��ʧ��"), pRegisterService->szServiceName);

				//������Ϣ
				WORD wStringSize = CountStringBuffer(RegisterFailure.szDescribeString);
				WORD wSendSize = sizeof(RegisterFailure) - sizeof(RegisterFailure.szDescribeString) + wStringSize;
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_FAILURE, &RegisterFailure, wSendSize, NULL, 0);

				//�ж�����
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//���ð�
			WORD wBindIndex = LOWORD(dwSocketID);
			tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);

			//���ò���
			pBindParameter->dwServiceID = pRegisterService->wServiceID;
			pBindParameter->wServiceModule = pRegisterService->wServiceModule;

			//��������
			tagServiceItem ServiceItem;
			ZeroMemory(&ServiceItem, sizeof(ServiceItem));

			//����ṹ
			ServiceItem.wServiceID = pRegisterService->wServiceID;
			ServiceItem.wServicePort = pRegisterService->wServicePort;
			ServiceItem.wMaxConnect = pRegisterService->wMaxConnect;
			ServiceItem.wServiceModule = pRegisterService->wServiceModule;
			ServiceItem.dwServiceAddr = pRegisterService->dwServiceAddr;
			StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), pRegisterService->szServiceName);
			StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain), pRegisterService->szServiceDomain);

			//ע������
			pBindParameter->pGlobalGateItem = m_GlobalItemManager.ActiveGateItem(dwSocketID, ServiceItem);
			if (pBindParameter->pGlobalGateItem == NULL)
			{
				//��������
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure, sizeof(RegisterFailure));

				//���ñ���
				RegisterFailure.lErrorCode = 0;
				StringCchCopy(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), TEXT("����δ֪ԭ�򣬷���ע��ʧ�ܣ�"));

				//������Ϣ
				WORD wStringSize = CountStringBuffer(RegisterFailure.szDescribeString);
				WORD wSendSize = sizeof(RegisterFailure) - sizeof(RegisterFailure.szDescribeString) + wStringSize;
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_FAILURE, &RegisterFailure, wSendSize, NULL, 0);

				//�ж�����
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//��������
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_SUCCESS, NULL, 0);

			return true;
		}
	}

	return false;
}

//������Ϣ
bool CAttemperEngineSink::OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_LOGOUT_TOKEN:	//ע������
		{
			//���ð�
			WORD wBindIndex = LOWORD(dwSocketID);
			auto pBindParameter = (m_pNormalParameter + wBindIndex);

			//У�����
			if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return true;

			//��ȡ����
			auto pLogoutToken = (CMD_CS_C_LogoutToken *)pData;

			//�ǳ�����
			return OnTCPNetworkSubLogonLogout(NULL,0, dwSocketID, pLogoutToken->dwTokenID);
		}
		case SUB_CS_C_REGISTER_TOKEN:	//ע������
		{
			//���ð�
			WORD wBindIndex = LOWORD(dwSocketID);
			auto pBindParameter = (m_pNormalParameter + wBindIndex);

			//�Ƴ�����
			if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return true;

			//��ȡ����
			auto pGlobalGateItem = pBindParameter->pGlobalGateItem;

			//��ȡ����
			CMD_CS_C_RegisterToken * pRegisterToken = (CMD_CS_C_RegisterToken *)pData;

			//����ṹ
			CMD_CS_S_RegisterToken RegisterToken;
			ZeroMemory(&RegisterToken,sizeof(RegisterToken));

			//Ĭ��ʧ��
			RegisterToken.lResultCode = REGISTER_RESULT_FAILURE;

			//У������
			DWORD dwTokenCount = CollectTokenCount();
			if (dwTokenCount >= m_pGameServiceOption->wMaxPlayer)
			{
				RegisterToken.lResultCode = REGISTER_RESULT_LINKFULL;
			}
			else
			{
				//��ѯ�û�
				auto pTokenParameter = pGlobalGateItem->ActiveTokenID(pRegisterToken->dwTokenID);
				if (pTokenParameter != NULL)
				{
					//ע��ɹ�
					RegisterToken.lResultCode = REGISTER_RESULT_SUCCESS;

					//���õ�ַ
					pTokenParameter->dwUserID = pRegisterToken->dwUserID;
					pTokenParameter->dwClientIP = pRegisterToken->dwClientIP;					
				}
			}		

			//��������
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_REGISTER_TOKEN, &RegisterToken, sizeof(RegisterToken), &pRegisterToken->dwTokenID, 1);

			return true;
		}
	}

	return true;
}

//���Դ���
bool CAttemperEngineSink::OnTCPNetworkMainDebug(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_DEBUG_PACKET:		//�������ݰ�
	{
		return OnTCPNetworkSubDebugPacket(pData, wDataSize, dwSocketID, dwTokenID);
	}
	}

	return false;
}

//��¼����
bool CAttemperEngineSink::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_LOGON_LOGOUT:		//�û��ǳ�
		{
			return OnTCPNetworkSubLogonLogout(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_LOGON_MOBILE:		//�ֻ���¼
		{
			return OnTCPNetworkSubLogonMobile(pData, wDataSize, dwSocketID, dwTokenID);
		}
	case SUB_GR_LOGON_ROBOT:		//������¼
		{
			return OnTCPNetworkSubLogonRobot(pData, wDataSize, dwSocketID, dwTokenID);
		}
	case SUB_GR_LOGON_COMPUTER:		//���Ե�¼
		{
			return OnTCPNetworkSubLogonComputer(pData,wDataSize,dwSocketID,dwTokenID);
		}	
	}

	return false;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_USER_RULE:			//�û�����
		{
			return OnTCPNetworkSubUserRule(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_LOOKON:		//�û��Թ�
		{
			return OnTCPNetworkSubUserLookon(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_SITDOWN:		//�û�����
		{
			return OnTCPNetworkSubUserSitDown(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_STANDUP:		//�û�����
		{
			return OnTCPNetworkSubUserStandUp(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_CHAT:			//�û�����
		{
			return OnTCPNetworkSubUserChat(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_WISPER:		//�û�˽��
	case SUB_GR_WHSPER_REPLY:		//�Զ��ظ�
		{
			return OnTCPNetworkSubUserWisper(pData,wDataSize,dwSocketID,wSubCmdID,dwTokenID);
		}
	case SUB_GR_INVITE_USER:		//�����û�
		{
			return OnSocketSubInviteUser(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_REPULSE_SIT:	//�ܾ�����
		{
			return OnTCPNetworkSubUserRepulseSit(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_REQUEST_INFO:  //������Ϣ
		{
			return OnTCPNetworkSubUserRequestInfo(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_SWITCH_TABLE:	//��������
		{
 			return OnTCPNetworkSubUserSwitchTable(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_DESKMATE_INFO: //ͬ����Ϣ
		{
			return OnTCPNetworkSubUserDeskmateInfo(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_SEND_GIFT:			//��������
		{
			return OnTCPNetworkSubUserSendGift(pData, wDataSize, dwSocketID, dwTokenID);
		}
	}

	return false;
}

//������
bool CAttemperEngineSink::OnTCPNetworkMainManage(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_KILL_USER:			//�߳��û�
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_GR_KillUser));
			if (wDataSize!=sizeof(CMD_GR_KillUser)) return false;

			//��Ϣ����
			CMD_GR_KillUser * pKillUser=(CMD_GR_KillUser *)pData;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//Ȩ���ж�
			ASSERT(CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==true);
			if (CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==false) return false;

			//Ŀ���û�
			IServerUserItem * pITargerUserItem=m_ServerUserManager.SearchUserItem(pKillUser->dwTargetUserID);
			if (pITargerUserItem==NULL) return true;

			//������Ϣ
			SendRoomMessage(pITargerUserItem, CWHService::FormatMessage(MIM_SERVER_0001, TEXT("��Ǹ����������Ա�߳����䣬���ڱ��ߺ�һСʱ���ܽ��룬�����κ����ʣ�����ϵ��Ϸ�ͷ���")),SMT_CHAT|SMT_EJECT|SMT_CLOSE_LINK|SMT_CLOSE_ROOM);

			return true;
		}
	case SUB_GR_DISMISSGAME:		//��ɢ��Ϸ
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GR_DismissGame));
			if(wDataSize>sizeof(CMD_GR_DismissGame) ) return false;

			//��ȡ����
			CMD_GR_DismissGame * pDismissGame=(CMD_GR_DismissGame *)pData;
			if(pDismissGame==NULL) return false;
			
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//Ȩ���ж�
			ASSERT(CMasterRight::CanDismissGame(pIServerUserItem->GetMasterRight())==true);
			if (CMasterRight::CanDismissGame(pIServerUserItem->GetMasterRight())==false) return false;

			//�����ж�
			if(pDismissGame->wDismissTableNum<m_TableFrameArray.GetCount())
			{
				CTableFrameItem * pTableFrame=m_TableFrameArray[pDismissGame->wDismissTableNum];
				if ( !pTableFrame->IsGameStarted() ) return true;
				pTableFrame->DismissGame();
			}			
			
			return true;
		}	
	case SUB_GR_OPTION_SERVER:		//�������
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_GR_OptionServer));
			if (wDataSize!=sizeof(CMD_GR_OptionServer)) return false;

			//��Ϣ����
			CMD_GR_OptionServer * pOptionServer=(CMD_GR_OptionServer *)pData;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//Ȩ���ж�
			ASSERT(CMasterRight::CanManagerServer(pIServerUserItem->GetMasterRight())==true);
			if (CMasterRight::CanManagerServer(pIServerUserItem->GetMasterRight())==false) return false;

			bool bNotify = false;

			//��������
			if (pOptionServer->cbOptionFlags==OSF_ROOM_CHAT)
			{
				g_GlobalUnits.SetAllowRoomChat((pOptionServer->cbOptionValue==TRUE)?true:false);

				bNotify = true;
				CServerRule::SetForbidRoomChat( CMD_ConfigServer.dwServerRule,(pOptionServer->cbOptionValue==TRUE)?false:true );
			}

			//��Ϸ����
			if (pOptionServer->cbOptionFlags==OSF_GAME_CHAT)
			{
				g_GlobalUnits.SetAllowGameChat((pOptionServer->cbOptionValue==TRUE)?true:false);

				bNotify = true;
				CServerRule::SetForbidGameChat( CMD_ConfigServer.dwServerRule,(pOptionServer->cbOptionValue==TRUE)?false:true );
			}

			//����˽��
			if (pOptionServer->cbOptionFlags==OSF_ROOM_WISPER)
			{
				g_GlobalUnits.SetAllowWisper((pOptionServer->cbOptionValue==TRUE)?true:false);

				bNotify = true;
				CServerRule::SetForbidRoomWisper( CMD_ConfigServer.dwServerRule,(pOptionServer->cbOptionValue==TRUE)?false:true );
			}

			//������Ϸ
			if (pOptionServer->cbOptionFlags==OSF_ENTER_GAME)
			{
				g_GlobalUnits.SetAllowEnterGame((pOptionServer->cbOptionValue==TRUE)?true:false);
			}

			//���뷿��
			if (pOptionServer->cbOptionFlags==OSF_ENTER_ROOM)
			{
				g_GlobalUnits.SetAllowEnterRoom((pOptionServer->cbOptionValue==TRUE)?true:false);
			}

			//ͬ����Ϸ˽��
			if(pOptionServer->cbOptionFlags==OSF_PLAY_CHAT)
			{
				g_GlobalUnits.SetForbidPlayChat((pOptionServer->cbOptionValue==TRUE)?false:true);

				bNotify = true;
				CServerRule::SetForbidPlayChat( CMD_ConfigServer.dwServerRule,(pOptionServer->cbOptionValue==TRUE)?false:true );
			}

			//��ֹԶ������
			if (pOptionServer->cbOptionFlags == OSF_REMOTE_SEARCH )
			{
				g_GlobalUnits.SetForbidQueryUser((pOptionServer->cbOptionValue==TRUE)?false:true);

				bNotify = true;
				CServerRule::SetForbidQueryUser( CMD_ConfigServer.dwServerRule,(pOptionServer->cbOptionValue==TRUE)?false:true );
			}

			//��Ա������
			if (pOptionServer->cbOptionFlags==OSF_MEMBER_CAN_CHAT)
			{
				g_GlobalUnits.SetMemberCanChat((pOptionServer->cbOptionValue==TRUE)?true:false);

				bNotify = true;
				CServerRule::SetMemberCanChat( CMD_ConfigServer.dwServerRule,(pOptionServer->cbOptionValue==TRUE)?true:false );
			}

			//��ֹ�Թ�
			if (pOptionServer->cbOptionFlags == OSF_LOOK_ON )
			{
				g_GlobalUnits.SetForbidLookon((pOptionServer->cbOptionValue==TRUE)?false:true);

				bNotify = true;
				CServerRule::SetForbidLookon( CMD_ConfigServer.dwServerRule,(pOptionServer->cbOptionValue==TRUE)?false:true );
			}

			//�Թ�����
			if (pOptionServer->cbOptionFlags == OSF_LOOK_ON_CHAT)
			{
				g_GlobalUnits.SetAllowLookOnChat(pOptionServer->cbOptionValue);
			}

			//���������
			if (pOptionServer->cbOptionFlags==OSF_ROBOT_ATTEND)
			{
				//����Ȩ��
				bool bAllow = g_GlobalUnits.IsAllowRobotAttend();
				g_GlobalUnits.SetAllowRobotAttend((pOptionServer->cbOptionValue==TRUE)?true:false);

				//���ö�ʱ��
				if (bAllow!=g_GlobalUnits.IsAllowRobotAttend())
				{
					if (g_GlobalUnits.IsAllowRobotAttend())
					{
						m_pITimerEngine->SetTimer(IDI_LOAD_ROBOT_INFO,TIME_LOAD_ROBOT_INFO*1000L,1,NULL);
						m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ROBOT,TIME_DISTRIBUTE_ROBOT*1000L*2,TIMES_INFINITY,NULL);
					}
					else
					{
						m_pITimerEngine->KillTimer(IDI_LOAD_ROBOT_INFO);
					}
				}
			}

			//������ռ��
			if (pOptionServer->cbOptionFlags==OSF_ROBOT_SIMULATE)
			{
				//����Ȩ��
				bool bAllow = g_GlobalUnits.IsAllowRobotSimulate();
				g_GlobalUnits.SetAllowRobotSimulate((pOptionServer->cbOptionValue==TRUE)?true:false);

				//���ö�ʱ��
				if (bAllow!=g_GlobalUnits.IsAllowRobotSimulate())
				{
					if (g_GlobalUnits.IsAllowRobotSimulate())
					{
						m_pITimerEngine->SetTimer(IDI_LOAD_ROBOT_INFO,TIME_LOAD_ROBOT_INFO*1000L,1,NULL);
						m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ROBOT,TIME_DISTRIBUTE_ROBOT*1000L*2,TIMES_INFINITY,NULL);
					}
					else
					{
						m_pITimerEngine->KillTimer(IDI_LOAD_ROBOT_INFO);
					}
				}
			}

			//�����Զ��رշ���
			if (pOptionServer->cbOptionFlags == OSF_CLOSE_NOPLAYER)
			{
				g_GlobalUnits.SetCloseWhileNoplayer((pOptionServer->cbOptionValue==TRUE)?true:false);
			}
			
			//����֪ͨ
			if (bNotify)
			{
				//��������
				CMD_GR_ConfigRule cr = {0};
				cr.dwServerRule = CMD_ConfigServer.dwServerRule;

				SendBatchData(BG_COMPUTER,MDM_GR_CONFIG,SUB_GR_CONFIG_RULE,&cr,sizeof(cr) );

				m_pGameServiceOption->dwServerRule|=CMD_ConfigServer.dwServerRule;
			}

			//������Ϣ
			SendRoomMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0004,TEXT("�������������ò���ѡ�����óɹ�")), SMT_CHAT);

			//��������
			ModifyGameServer();

			return true;
		}
	case SUB_GR_KICK_ALL_USER:		//�߳��û�
		{
			//��Ϣ����
			CMD_GR_KickAllUser * pKillAllUser=(CMD_GR_KickAllUser *)pData;
			
			//Ч������
			ASSERT(wDataSize<=sizeof(CMD_GR_KickAllUser));
			if( wDataSize > sizeof(CMD_GR_KickAllUser) ) return false;
			ASSERT(wDataSize==CountStringBuffer(pKillAllUser->szKickMessage));
			if (wDataSize!=CountStringBuffer(pKillAllUser->szKickMessage)) return false;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//Ȩ���ж�
			ASSERT(CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==true);
			if (CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==false) return false;

			//��ɢ������Ϸ
			for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
			{
				//��ȡ����
				CTableFrameItem * pTableFrame=m_TableFrameArray[i];
				if ( !pTableFrame->IsGameStarted() ) continue;

				pTableFrame->DismissGame();
			}

			//��������
			WORD wEnumIndex = 0;
			IServerUserItem * pITargerUserItem = NULL;

			do
			{
				//ö���û�
				pITargerUserItem = m_ServerUserManager.EnumUserItem(wEnumIndex++);
				if (pITargerUserItem == NULL) break;
				if (pITargerUserItem != pIServerUserItem)
				{
					//������Ϣ
					SendRoomMessage(pITargerUserItem, pKillAllUser->szKickMessage, SMT_CHAT | SMT_EJECT | SMT_CLOSE_LINK | SMT_CLOSE_ROOM);
				}

			} while (true);

			return true;
		}
	case SUB_GR_SEND_MESSAGE:		//������Ϣ
		{
			//��Ϣ����
			CMD_GR_SendMessage * pSendMessage=(CMD_GR_SendMessage *)pData;

			//Ч������
			ASSERT(wDataSize==sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szString)+sizeof(TCHAR)*pSendMessage->wLength);
			if (wDataSize!=sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szString)+sizeof(TCHAR)*pSendMessage->wLength) 
				return false;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//Ȩ���ж�
			ASSERT(CMasterRight::CanIssueMessage(pIServerUserItem->GetMasterRight())==true);
			if (CMasterRight::CanIssueMessage(pIServerUserItem->GetMasterRight())==false) return false;

			//��������
			WORD wEnumIndex=0;
			IServerUserItem *pISendServerUserItem=NULL;

			do 
			{
				pISendServerUserItem = m_ServerUserManager.EnumUserItem(wEnumIndex++);
				if (pISendServerUserItem==NULL) break;
				if (pISendServerUserItem->IsMobileUser() == false)
				{
					//����ϵͳ��Ϣ
					if (pSendMessage->dwOptionFlags & OSW_GAME)
						SendGameMessage(pISendServerUserItem, pSendMessage->szString, pSendMessage->wType);
					if (pSendMessage->dwOptionFlags & OSW_ROOM)
						SendRoomMessage(pISendServerUserItem, pSendMessage->szString, pSendMessage->wType);
				}
			} while (true);

			return true;
		}
	case SUB_GR_LIMIT_USER_CHAT:	//��������
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_GR_LimitUserChat));
			if (wDataSize!=sizeof(CMD_GR_LimitUserChat)) return false;

			//��Ϣ����
			CMD_GR_LimitUserChat * pLimitUserChat=(CMD_GR_LimitUserChat *)pData;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//Ȩ���ж�
			ASSERT(CMasterRight::CanLimitUserChat(pIServerUserItem->GetMasterRight())==true);
			if (CMasterRight::CanLimitUserChat(pIServerUserItem->GetMasterRight())==false) return false;

			//Ŀ���û�
			IServerUserItem * pITargerUserItem=m_ServerUserManager.SearchUserItem(pLimitUserChat->dwTargetUserID);
			if (pITargerUserItem==NULL) return true;

			//��������
			DWORD dwAddRight = 0, dwRemoveRight = 0;

			//��������
			if (pLimitUserChat->cbLimitFlags==OSF_ROOM_CHAT)
			{
				if( pLimitUserChat->cbLimitValue == TRUE )
					dwAddRight |= UR_CANNOT_ROOM_CHAT;
				else
					dwRemoveRight |= UR_CANNOT_ROOM_CHAT;
			}

			//��Ϸ����
			if (pLimitUserChat->cbLimitFlags==OSF_GAME_CHAT)
			{
				if( pLimitUserChat->cbLimitValue == TRUE )
					dwAddRight |= UR_CANNOT_GAME_CHAT;
				else
					dwRemoveRight |= UR_CANNOT_GAME_CHAT;
			}

			//����˽��
			if (pLimitUserChat->cbLimitFlags==OSF_ROOM_WISPER)
			{
				if( pLimitUserChat->cbLimitValue == TRUE )
					dwAddRight |= UR_CANNOT_WISPER;
				else
					dwRemoveRight |= UR_CANNOT_WISPER;
			}

			//��������
			if(pLimitUserChat->cbLimitFlags==OSF_SEND_BUGLE)
			{
				if(pLimitUserChat->cbLimitValue == TRUE)
					dwAddRight |= UR_CANNOT_BUGLE;
				else
					dwRemoveRight |= UR_CANNOT_BUGLE;
			}

			if( dwAddRight != 0 || dwRemoveRight != 0 )
			{
				pITargerUserItem->ModifyUserRight(dwAddRight,dwRemoveRight);

				//����֪ͨ
				CMD_GR_ConfigUserRight cur = {0};
				cur.dwUserRight = pITargerUserItem->GetUserRight();

				SendDataToUser(pITargerUserItem,MDM_GR_CONFIG,SUB_GR_CONFIG_USER_RIGHT,&cur,sizeof(cur) );

				//������Ϣ
				SendRoomMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0005, TEXT("�û�����Ȩ�����óɹ�")), SMT_CHAT);
			}
			else return false;

			return true;
		}
	case SUB_GR_TABLE_RULE:			//�������
		{
			//Ч������
			ASSERT(wDataSize==sizeof(DWORD));
			if (wDataSize!=sizeof(DWORD)) return false;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//Ȩ���ж�
			ASSERT(CMasterRight::CanManagerServer(pIServerUserItem->GetMasterRight())==true);
			if (CMasterRight::CanManagerServer(pIServerUserItem->GetMasterRight())==false) return false;

			//��������
			DWORD dwServerRule=*(DWORD*)pData;
			CMD_ConfigServer.dwServerRule=(CMD_ConfigServer.dwServerRule&0x00ffffff);
			CMD_ConfigServer.dwServerRule|=(dwServerRule&0xff000000);
			m_pGameServiceOption->dwServerRule=CMD_ConfigServer.dwServerRule;

			//��������
			CMD_GR_ConfigRule cr = {0};
			cr.dwServerRule = CMD_ConfigServer.dwServerRule;

			//������Ϣ
			SendBatchData(BG_COMPUTER,MDM_GR_CONFIG,SUB_GR_CONFIG_RULE,&cr,sizeof(cr) );

			//��������
			ModifyGameServer();

			return true;
		}
	case SUB_GR_SERVER_OPTION:		//����ѡ��
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_GR_ManageServerOption));
			if (wDataSize!=sizeof(CMD_GR_ManageServerOption)) return false;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//Ȩ���ж�
			ASSERT(CMasterRight::CanManagerServer(pIServerUserItem->GetMasterRight())==true);
			if (CMasterRight::CanManagerServer(pIServerUserItem->GetMasterRight())==false) return false;

			//��������
			CMD_GR_ManageServerOption *pServerOption=(CMD_GR_ManageServerOption*)pData;

			m_pGameServiceOption->wNodeID=pServerOption->wNodeID;
			m_pGameServiceOption->wSortID=pServerOption->wSortID;
			m_pGameServiceOption->wKindID=pServerOption->wKindID;
			CServerRule::SetChargeModeFee(m_pGameServiceOption->dwServerRule,CServerRule::IsChargeModeFee(pServerOption->dwRevenueType));
			m_pGameServiceOption->lRevenue=pServerOption->lRevenue;
			if (pServerOption->szServerName[0] != 0 && _tcscmp(m_pGameServiceOption->szServerName, pServerOption->szServerName) != 0)
			{
				StringCchCopy(m_pGameServiceOption->szServerName, LEN_SERVER, pServerOption->szServerName);
			}

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("����ѡ��������������ע����Ϸ����..."),TraceLevel_Normal);

			//��������
			CMD_CS_C_ServerModify ModifyServer;
			ZeroMemory(&ModifyServer,sizeof(ModifyServer));

			//��������
			ModifyServer.wKindID=m_pGameServiceOption->wKindID;
			ModifyServer.wNodeID=m_pGameServiceOption->wNodeID;
			ModifyServer.wSortID=m_pGameServiceOption->wSortID;
			ModifyServer.wServerType=m_pGameServiceOption->wServerType;
			ModifyServer.wServerLevel=m_pGameServiceOption->wServerLevel;
			ModifyServer.dwServerRule = m_pGameServiceOption->dwServerRule;
			ModifyServer.wServerPort=m_pGameServiceOption->wServerPort!=0?(m_pGameServiceOption->wServerPort):(m_pITCPNetworkEngine->GetCurrentPort());
			ModifyServer.dwMaxPlayer=m_pGameServiceOption->wTableCount*m_TableFrameArray[0]->m_wMaxPlayerCount;
			ModifyServer.dwOnLineCount=m_ServerUserManager.GetUserItemCount();
			StringCchCopy(ModifyServer.szServerName, CountArray(ModifyServer.szServerName), m_pGameServiceOption->szServerName);

			//��ַ��Ϣ
			ModifyServer.dwServerAddr=inet_addr(CT2CA(m_pGameServiceOption->szServiceAddr));
			StringCchCopy(ModifyServer.szServerDomain,CountArray(ModifyServer.szServerDomain), m_pGameServiceOption->szServiceDomain);			

			//��������
			ModifyServer.lCellScore=m_pGameServiceOption->lCellScore;
			ModifyServer.lMinEnterScore=m_pGameServiceOption->lMinEnterScore;
			ModifyServer.lMaxEnterScore=m_pGameServiceOption->lMaxEnterScore;

			//��������
			ASSERT(m_pITCPSocketCenter!=NULL);
			m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE,SUB_CS_C_SERVER_MODIFY,&ModifyServer,sizeof(ModifyServer),0);

			//������Լս����λ
			if (m_pGameServiceOption->wServerType & (GAME_GENRE_MATCH| GAME_GENRE_BATTLE| GAME_GENRE_RANKING))
			{
				m_pITCPSocketService->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_SERVER_MODIFY, &ModifyServer, sizeof(ModifyServer), 0);
			}		

			//��������
			ModifyGameServer();

			return true;
		}
	case SUB_GR_CARDDISPATCHERDEBUG://��������Ϣ
		{
			if (wDataSize < sizeof(CMD_GR_CardDispatcherDebug)) return false;

			//��ȡ����
			CMD_GR_CardDispatcherDebug* pRoomDebug = (CMD_GR_CardDispatcherDebug*)pData;
			//ASSERT(pRoomDebug != NULL);

			//��ȡ�û�
			WORD wBindIndex = LOWORD(dwSocketID);
			IServerUserItem* pIServerUserItem = GetServerUserItem(LOWORD(dwSocketID), dwTokenID);
			if (pIServerUserItem == NULL) return false;

			//��������
			BYTE cbBuffer[SOCKET_BUFFER];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));
			WORD wSendDataSize = 0;
			CopyMemory(cbBuffer, pRoomDebug + 1, wDataSize - sizeof(CMD_GR_CardDispatcherDebug));
			wSendDataSize = wDataSize - sizeof(CMD_GR_CardDispatcherDebug);

			//tagCardDispatcher* pCardDispatcher = (tagCardDispatcher*)cbBuffer;

			//��Ϣ���� 
			if (pRoomDebug->wTableID != INVALID_TABLE)
			{
				//ָ����
				//ASSERT(pRoomDebug->wTableID < m_TableFrameArray.GetCount());
				if (pRoomDebug->wTableID >= m_TableFrameArray.GetCount()) return false;

				CTableFrameItem* pTableFrame = m_TableFrameArray[pRoomDebug->wTableID];
				//pTableFrame->OnEventSocketFrame(pRoomDebug->wSubCmdID, cbBuffer, wSendDataSize, pIServerUserItem);
				pTableFrame->ControlSendCard(pIServerUserItem, cbBuffer, wSendDataSize);
			}
			else
			{
				//����������
				for (BYTE i = 0; i < m_TableFrameArray.GetCount(); i++)
				{
					CTableFrameItem* pTableFrame = m_TableFrameArray[i];
					if (pTableFrame != NULL)
					{
						//pTableFrame->OnEventSocketFrame(pRoomDebug->wSubCmdID, cbBuffer, wSendDataSize, pIServerUserItem);
						pTableFrame->ControlSendCard(pIServerUserItem, cbBuffer, wSendDataSize);
					}
				}
			}
			return true;
		}
	}

	return false;
}

//��Ա����
bool CAttemperEngineSink::OnTCPNetworkMainMember(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch( wSubCmdID )
	{
	case SUB_GR_KICK_USER:			//�����û�
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_GR_KickUser));
			if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

			//��Ϣ����
			CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//Ŀ���û�
			IServerUserItem * pITargerUserItem=m_ServerUserManager.SearchUserItem(pKickUser->dwTargetUserID);
			if (pITargerUserItem==NULL) return true;

			//Ȩ���ж�
			ASSERT( CUserRight::CanKickUser(pIServerUserItem->GetUserRight()) );
			if ( !CUserRight::CanKickUser(pIServerUserItem->GetUserRight()) ) return false;

			//������״̬
			BYTE cbUserStatus = pITargerUserItem->GetUserStatus();
			BYTE cbMeStatus = pIServerUserItem->GetUserStatus();
			ASSERT( cbMeStatus==US_SIT || cbMeStatus==US_READY || cbMeStatus==US_PLAYING);
			if( cbMeStatus!=US_SIT && cbMeStatus!=US_READY && cbMeStatus!=US_PLAYING) return true;
			ASSERT( cbUserStatus==US_SIT || cbUserStatus==US_READY || cbUserStatus==US_LOOKON);
			if( cbUserStatus!=US_SIT && cbUserStatus!=US_READY && cbUserStatus!=US_LOOKON) return true;

			//����Ա�ȼ�
			ASSERT( pIServerUserItem->GetMemberOrder()>pITargerUserItem->GetMemberOrder() );
			if( pIServerUserItem->GetMemberOrder()<=pITargerUserItem->GetMemberOrder() ) return true;

			//�����߹���Ա
			ASSERT( pITargerUserItem->GetMasterRight()==0 && pITargerUserItem->GetMasterOrder()==0 );
			if( pITargerUserItem->GetMasterRight()!=0 || pITargerUserItem->GetMasterOrder()!=0 ) 
				return true;

			//�������䲻����
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
			{
				TCHAR szMessage[128] = {};
				_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��Ǹ�����������Ա���������ˣ�"));

				if(pIServerUserItem->GetTableID()==INVALID_TABLE)
					SendRoomMessage(pIServerUserItem, szMessage, SMT_EJECT|SMT_CHAT);
				else
					SendGameMessage(pIServerUserItem, szMessage, SMT_EJECT|SMT_CHAT);

				return true;
			}

			//������֤
			if (pITargerUserItem->GetTableID()!=INVALID_TABLE)
			{
				//��ȡָ��
				CTableFrameItem * pTableFrame=m_TableFrameArray[pITargerUserItem->GetTableID()];
				if(pTableFrame!=NULL)
				{
					//�������
					pTableFrame->PerformStandUpAction(pITargerUserItem);
				}
			}

			//���췢������
			BYTE cbBuffer[SOCKET_PACKET];
			WORD wSendSize = 0;
			CMD_CM_ActionMessage *pActionMessage = (CMD_CM_ActionMessage *)cbBuffer;
			pActionMessage->nButtonType = MB_OKCANCEL;
			pActionMessage->wType = SMT_CLOSE_GAME;
			StringCchCopy( pActionMessage->szString, CountArray(pActionMessage->szString), CWHService::FormatMessage(MIM_SERVER_0006, TEXT("��Ǹ��������Ա����������Ϸ��!")));
			pActionMessage->wLength = lstrlen(pActionMessage->szString)+1;

			wSendSize = sizeof(CMD_CM_ActionMessage)-sizeof(pActionMessage->szString)+CountStringBuffer(pActionMessage->szString);
			
			//��������
			SendDataToUser(pITargerUserItem, MDM_GF_FRAME, SUB_GF_ACTION_MESSAGE, cbBuffer, wSendSize);

			return true;
		}
	}

	return false;
}

//��ܴ���
bool CAttemperEngineSink::OnScoketMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�������
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//��Ϣ���� 
	CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketFrame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//��Ϸ����
bool CAttemperEngineSink::OnScoketMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�������
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//��Ϣ���� 
	CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketGame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//��ѯ����
bool CAttemperEngineSink::OnTCPNetworkMainQuery(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch(wSubCmdID)
	{
	case SUB_GR_QUERY_BY_GAMEID:
		{
			//Ч��
			ASSERT(wDataSize==sizeof(CMD_GR_QueryByGameID));
			if( wDataSize != sizeof(CMD_GR_QueryByGameID) )
				return false;

			//��������
			CMD_GR_QueryByGameID *pQueryByGameID = (CMD_GR_QueryByGameID *)pData;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//��ѯ����
			if( g_GlobalUnits.IsForbidQueryUser() && pIServerUserItem->GetMasterOrder() == 0 )
			{
				//������Ϣ
				CMD_GR_QueryNotFound NotFound = {0};
				NotFound.nResultCode = QUERY_FORBID_USE;

				WORD wSendSize = sizeof(NotFound)-sizeof(NotFound.szDescribeString)+CountStringBuffer(NotFound.szDescribeString);
				SendDataToUser(pIServerUserItem,MDM_GR_QUERY,SUB_GR_QUERY_NOT_FOUND,&NotFound,wSendSize);

				return true;
			}

			//������Ϣ
			CMD_CS_C_QueryByGameID QueryByGameID = {0};
			QueryByGameID.dwUserID = pIServerUserItem->GetUserID();
			QueryByGameID.dwQueryGameID = pQueryByGameID->dwGameID;

			m_pITCPSocketCenter->SendData(MDM_CS_QUERY_SERVICE,SUB_CS_C_QUERY_BY_GAMEID,&QueryByGameID,sizeof(QueryByGameID),0);

			return true;
		}
	case SUB_GR_QUERY_BY_ACCOUNTS:
		{
			//��������
			CMD_GR_QueryByAccounts *pQueryByAccounts = (CMD_GR_QueryByAccounts *)pData;
			
			//Ч��
			ASSERT(wDataSize==sizeof(CMD_GR_QueryByAccounts)-sizeof(pQueryByAccounts->szAccounts)+CountStringBuffer(pQueryByAccounts->szAccounts));
			if( wDataSize != sizeof(CMD_GR_QueryByAccounts)-sizeof(pQueryByAccounts->szAccounts)+CountStringBuffer(pQueryByAccounts->szAccounts) )
				return false;

			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//��ѯ����
			if( g_GlobalUnits.IsForbidQueryUser() && pIServerUserItem->GetMasterOrder() == 0 )
			{
				//������Ϣ
				CMD_GR_QueryNotFound NotFound = {0};
				NotFound.nResultCode = QUERY_FORBID_USE;

				WORD wSendSize = sizeof(NotFound)-sizeof(NotFound.szDescribeString)+CountStringBuffer(NotFound.szDescribeString);
				SendDataToUser(pIServerUserItem,MDM_GR_QUERY,SUB_GR_QUERY_NOT_FOUND,&NotFound,wSendSize);

				return true;
			}

			//������Ϣ
			CMD_CS_C_QueryByAccounts QueryByAccounts = {0};
			QueryByAccounts.dwUserID = pIServerUserItem->GetUserID();
			StringCchCopy( QueryByAccounts.szQueryAccounts,CountArray(QueryByAccounts.szQueryAccounts), pQueryByAccounts->szAccounts);

			WORD wSendSize = sizeof(QueryByAccounts)-sizeof(QueryByAccounts.szQueryAccounts)+CountStringBuffer(QueryByAccounts.szQueryAccounts);
			m_pITCPSocketCenter->SendData(MDM_CS_QUERY_SERVICE,SUB_CS_C_QUERY_BY_ACCOUNTS,&QueryByAccounts,wSendSize,0);

			return true;
		}
	}

	return false;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);	
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex, dwTokenID);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//����У��
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_MATCH) == 0)
	{
		return false;
	}

	//Ч��ӿ�
	ASSERT (m_pIExtendServiceManager!=NULL);
	if (m_pIExtendServiceManager==NULL) return false;		

	//��Ϣ����
	return m_pIExtendServiceManager->OnEventNetworkRead(wSubCmdID,pData,wDataSize,pIServerUserItem,dwSocketID,dwTokenID);
}

//Լս����
bool CAttemperEngineSink::OnTCPNetwrokMainBattle(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ��Ϣ
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem* pIServerUserItem = GetServerUserItem(wBindIndex, dwTokenID);

	//�û�Ч��
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return false;

	//����У��
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) == 0)
	{
		return false;
	}

	//Ч��ӿ�
	ASSERT(m_pIExtendServiceManager != NULL);
	if (m_pIExtendServiceManager == NULL) return false;

	//��Ϣ����
	return m_pIExtendServiceManager->OnEventNetworkRead(wSubCmdID, pData, wDataSize, pIServerUserItem, dwSocketID, dwTokenID);
}

//������¼
bool CAttemperEngineSink::OnTCPNetworkSubLogonRobot(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize >= sizeof(CMD_GR_LogonRobot));
	if (wDataSize < sizeof(CMD_GR_LogonRobot)) return false;

	//������Ϣ
	CMD_GR_LogonRobot * pLogonRobot = (CMD_GR_LogonRobot *)pData;
	pLogonRobot->szPassword[CountArray(pLogonRobot->szPassword) - 1] = 0;

	//��ȡ����
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);
	IServerUserItem * pICurrentUserItem = GetServerUserItem(wBindIndex, dwTokenID);

	//�ظ��ж�
	if (pICurrentUserItem != NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	CString strMessage;

	//�����汾	
	DWORD dwPlazaVersion=pLogonRobot->dwPlazaVersion;
	DWORD dwClientVersion=pLogonRobot->dwProcessVersion;
	if (PerformCheckVersion(DEVICE_KIND_ROBOT, dwPlazaVersion, dwClientVersion, dwSocketID, dwTokenID) == false) return true;

	//�л��ж�
	IServerUserItem * pIServerUserItem = m_ServerUserManager.SearchUserItem(pLogonRobot->dwUserID);
	if ((pIServerUserItem != NULL) && (pIServerUserItem->ContrastPassword(pLogonRobot->szPassword) == true))
	{
		if (pIServerUserItem->IsRobotUser() == true && pIServerUserItem->GetClientIP() != 0L)
		{
			return false;
		}

		//�л�����
		SwitchUserItemConnect(pIServerUserItem, wBindIndex, dwTokenID, false);

		return true;
	}

	//��������
	DBR_GR_LogonRobot LogonRobot;
	ZeroMemory(&LogonRobot,sizeof(LogonRobot));

	//��������	
	LogonRobot.dwClientIP=0;
	LogonRobot.szMachineID[0]=0;
	LogonRobot.dwUserID= pLogonRobot->dwUserID;	
	LogonRobot.lRequestScore= m_pGameServiceOption->lMaxLockScore;

	//Я����Ϣ
	LogonRobot.cbClientKind = CLIENT_KIND_ROBOT;

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_ROBOT, LogonRobot.dwUserID, dwSocketID, dwTokenID, &LogonRobot, sizeof(LogonRobot));

	return true;
}

//I D ��¼
bool CAttemperEngineSink::OnTCPNetworkSubLogonComputer(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GR_LogonComputer));
	if (wDataSize<sizeof(CMD_GR_LogonComputer)) return false;

	//������Ϣ
	CMD_GR_LogonComputer * pLogonComputer =(CMD_GR_LogonComputer *)pData;
	pLogonComputer->szPassword[CountArray(pLogonComputer->szPassword)-1]=0;

	//��ȡ����
	WORD wBindIndex=LOWORD(dwSocketID);
	tagTokenParameter * pTokenParameter=GetTokenParameter(wBindIndex, dwTokenID);
	IServerUserItem * pICurrentUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//�ظ��ж�
	if (pICurrentUserItem!=NULL)
	{ 
		ASSERT(FALSE);
		return false;
	}

	//��չ����
	auto pExtendParameter = (tagExtendParameter*)pTokenParameter->cbExtendData;

	//�汾��Ϣ
	pExtendParameter->cbClientKind = CLIENT_KIND_COMPUTER;
	pExtendParameter->dwPlazaVersion= pLogonComputer->dwPlazaVersion;
	pExtendParameter->dwFrameVersion= pLogonComputer->dwFrameVersion;
	pExtendParameter->dwProcessVersion= pLogonComputer->dwProcessVersion;

	//�����汾	
	DWORD dwPlazaVersion= pLogonComputer->dwPlazaVersion;
	DWORD dwClientVersion= pLogonComputer->dwProcessVersion;
	if (PerformCheckVersion(DEVICE_KIND_PC,dwPlazaVersion,dwClientVersion,dwSocketID,dwTokenID)==false) return true;

	//�л��ж�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLogonComputer->dwUserID);
	if ((pIServerUserItem!=NULL)&&(pIServerUserItem->ContrastPassword(pLogonComputer->szPassword)==true))
	{
		if(pIServerUserItem->IsRobotUser()==true && pIServerUserItem->GetClientIP()!=0L)
		{
			return false;
		}

		//�л�����
		SwitchUserItemConnect(pIServerUserItem,wBindIndex,dwTokenID,false);

		return true;
	}

	//��������
	DBR_GR_LogonComputer LogonComputer;
	ZeroMemory(&LogonComputer,sizeof(LogonComputer));

	//��������
	LogonComputer.dwUserID= pLogonComputer->dwUserID;
	LogonComputer.dwClientIP= pTokenParameter->dwClientIP;
	LogonComputer.lRequestScore=m_pGameServiceOption->lMaxLockScore;

	//���ñ���	
	StringCchCopy(LogonComputer.szPassword, CountArray(LogonComputer.szPassword), pLogonComputer->szPassword);

	//��������
	VOID * pDataBuffer=NULL;
	tagDataDescribe DataDescribe;
	CRecvPacketHelper RecvPacket(pLogonComputer +1,wDataSize-sizeof(CMD_GR_LogonComputer));

	//��չ��Ϣ
	while (true)
	{
		//��ȡ����
		pDataBuffer=RecvPacket.GetData(DataDescribe);
		if (DataDescribe.wDataDescribe==DTP_NULL) break;

		//���ݷ���
		switch (DataDescribe.wDataDescribe)
		{
		case DTP_GR_MACHINE_ID:	//������ʶ
			{
				ASSERT(pDataBuffer!=NULL);
				ASSERT(DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(TCHAR));
				if (DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(TCHAR))
				{
					StringCchCopy(LogonComputer.szMachineID,CountArray(LogonComputer.szMachineID), (TCHAR*)pDataBuffer);
				}

				break;
			}
		}
	}

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_COMPUTER, LogonComputer.dwUserID,dwSocketID,dwTokenID,&LogonComputer,sizeof(LogonComputer));

	return true;
}


//�û��ǳ�
bool CAttemperEngineSink::OnTCPNetworkSubLogonLogout(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ����
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);

	//��������
	auto pGlobalGateItem = pBindParameter->pGlobalGateItem;
	if (pGlobalGateItem == NULL) return true;

	//�󶨲���
	auto pIServerUserItem = GetServerUserItem(wBindIndex, dwTokenID);
	if (pIServerUserItem == NULL) return true;

	//ɾ������
	pGlobalGateItem->RemoveTokenID(dwTokenID);	

	//У������
	if (pIServerUserItem == NULL || pIServerUserItem->GetTokenID() != dwTokenID)
	{
		return true;
	}

	//���ӱ�ʶ
	WORD wTableID=pIServerUserItem->GetTableID();	

	//���ߴ���
	if (wTableID!=INVALID_TABLE)
	{
		//�����
		pIServerUserItem->DetachBindStatus();		

		//����֪ͨ
		ASSERT(wTableID<m_pGameServiceOption->wTableCount);
		m_TableFrameArray[wTableID]->OnEventUserOffLine(pIServerUserItem);
	}
	else
	{
		pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
	}	

	return true;
}

//�ֻ���¼
bool CAttemperEngineSink::OnTCPNetworkSubLogonMobile(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GR_LogonMobile));
	if (wDataSize<sizeof(CMD_GR_LogonMobile)) return false;

	//������Ϣ
	CMD_GR_LogonMobile * pLogonMobile=(CMD_GR_LogonMobile *)pData;
	pLogonMobile->szMachineID[CountArray(pLogonMobile->szMachineID)-1]=0; 

	//��ȡ����
	WORD wBindIndex=LOWORD(dwSocketID);
	auto pTokenParameter=GetTokenParameter(wBindIndex,dwTokenID);
	if (pTokenParameter == NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//��ȡ�û�
	auto pICurrentUserItem=(IServerUserItem*)pTokenParameter->pBindUserItem;

	//�ظ��ж�
	if (pICurrentUserItem!=NULL)
	{ 
		//ASSERT(FALSE);
		return false; 
	}

	//��չ����
	auto pExtendParameter = (tagExtendParameter*)pTokenParameter->cbExtendData;

	//�汾��Ϣ	
	pExtendParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pExtendParameter->dwPlazaVersion=pLogonMobile->dwPlazaVersion;
	pExtendParameter->dwProcessVersion=pLogonMobile->dwProcessVersion;

	//�汾У��
	DWORD dwPlazaVersion=pLogonMobile->dwPlazaVersion;
	DWORD dwClientVersion=pLogonMobile->dwProcessVersion;
	if (PerformCheckVersion(pLogonMobile->dwDeviceKind& DEVICE_MASK_MB,dwPlazaVersion,dwClientVersion,dwSocketID,dwTokenID)==false) return true;

	//�����˺����˲�����
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pTokenParameter->dwUserID);
	if (pIServerUserItem != NULL)
	{
		//�˻�����
		if ((pIServerUserItem->IsRobotUser() && (pTokenParameter->dwClientIP != 0L))
			|| (!pIServerUserItem->IsRobotUser() && (pTokenParameter->dwClientIP == 0L)))
		{
			SendRoomMessage(dwSocketID, dwTokenID, CWHService::FormatMessage(MIM_SERVER_0008, TEXT("���ʺ����ڴ˷�����Ϸ���Ҳ������߳�������ѯ����Ա��")), SMT_CHAT | SMT_EJECT | SMT_GLOBAL | SMT_CLOSE_ROOM);
			return false;
		}

		//�л�����
		SwitchUserItemConnect(pIServerUserItem, wBindIndex, dwTokenID, true);

		return true;
	}

	//��������
	DBR_GR_LogonMobile LogonMobile;
	ZeroMemory(&LogonMobile,sizeof(LogonMobile));

	//��������
	LogonMobile.dwUserID= pTokenParameter->dwUserID;
	LogonMobile.dwClientIP=pTokenParameter->dwClientIP;
	LogonMobile.dGlobalPosLat=pLogonMobile->dGlobalPosLat;
	LogonMobile.dGlobalPosLng=pLogonMobile->dGlobalPosLng;
	CopyMemory(LogonMobile.szPlaceName, pLogonMobile->szPlaceName,sizeof(LogonMobile.szPlaceName));
	LogonMobile.dwDeviceKind=pLogonMobile->dwDeviceKind;
	//CString s;
	//s.Format(_T("gameid:%d,dGlobalPosLat:%f, dGlobalPosLng:%f, szPlaceName:%s,szMachineID:%s"), pLogonMobile->wGameID, pLogonMobile->dGlobalPosLat, pLogonMobile->dGlobalPosLng, pLogonMobile->szPlaceName, pLogonMobile->szMachineID);
	//CTraceService::TraceString(s, TraceLevel_Info);
	//��������
	if (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)
	{
		//У������
		ASSERT (wDataSize==sizeof(CMD_GR_LogonMobile)+sizeof(tagLogonMatchDetails));
		if (wDataSize!=sizeof(CMD_GR_LogonMobile)+sizeof(tagLogonMatchDetails)) return false;

		//��ȡ����
		tagLogonMatchDetails * pLogonMatchDetails = (tagLogonMatchDetails *)(pLogonMobile + 1);

		//���ñ�ʶ
		LogonMobile.lMatchNo = pLogonMatchDetails->lMatchNo;
		LogonMobile.dwMatchID = pLogonMatchDetails->dwMatchID;	

		//У���ʶ
		if (LogonMobile.dwMatchID == 0 || LogonMobile.lMatchNo==0)
		{
			//����ʧ��
			SendLogonFailure(TEXT(""), 0, dwSocketID, dwTokenID);

			return false;
		}
	}

	//Լս����
	if (m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE)
	{
		//У������
		ASSERT (wDataSize<=sizeof(CMD_GR_LogonMobile)+sizeof(tagLogonBattleDetails));
		if (wDataSize>sizeof(CMD_GR_LogonMobile)+sizeof(tagLogonBattleDetails)) return false;

		//��ȡ����
		tagLogonBattleDetails * pLogonBattleDetails = (tagLogonBattleDetails *)(pLogonMobile + 1);
		pLogonBattleDetails->szTablePasswd[CountArray(pLogonBattleDetails->szTablePasswd) - 1] = 0;

		//��������
		WORD wBattleTableID = pLogonBattleDetails->wTableID;
		WORD wBattleChairID = pLogonBattleDetails->wChairID;

		//У������
		if (wBattleTableID>=m_TableFrameArray.GetCount())
		{
			ASSERT(FALSE);
			return false;
		}		

		//��������
		auto pTableFrame=m_TableFrameArray[wBattleTableID];
		if (pTableFrame->ContrastPassword(pLogonBattleDetails->szTablePasswd)==false)
		{
			//����ʧ��
			SendLogonFailure(CWHService::FormatMessage(MIM_SERVER_0007, TEXT("�ܱ�Ǹ������Լս������󣬲��������Լս���䣡")), 0, dwSocketID, dwTokenID);

			//�Ͽ�����
			if (LOWORD(dwSocketID) >= INDEX_ROBOT)
			{
				m_RobotUserManager.DeleteRobotUserItem(dwSocketID, false);
			}
			else
			{
				//����ṹ
				CMD_CS_C_ShutdownSocket ShutdownSocket;
				ShutdownSocket.bForceClose = false;

				//��������
				SendDataToGate(dwSocketID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));
			}

			return true;
		}

		//У������
		if (wBattleChairID!=INVALID_CHAIR)
		{
			//У������
			if (wBattleChairID >= pTableFrame->GetChairCount())
			{
				ASSERT(FALSE);
				return false;
			}			

			//У���û�
			if (!pLogonBattleDetails->bLookon)
			{
				auto pTableUserItem = pTableFrame->GetTableUserItem(wBattleChairID);
				if (pTableUserItem != NULL)
				{
					//����ʧ��
					SendLogonFailure(CWHService::FormatMessage(MIM_SERVER_0022, TEXT("û���ҵ����õ����ӣ����Ժ����ԣ�")), 0, dwSocketID, dwTokenID);
					return true;
				}
			}
		}
		else
		{
			//��ȡ����
			if (!pLogonBattleDetails->bLookon)
			{
				wBattleChairID = pTableFrame->GetRelationChairID(LogonMobile.dwUserID);				
			}
		}

		//��������
		auto pRelationParameter = (tagRelationParameter_Battle*)pTableFrame->GetRelationParameter();

		//������Ϣ
		LogonMobile.wTableID = wBattleTableID;
		LogonMobile.wChairID = wBattleChairID;
		LogonMobile.bLookon = pLogonBattleDetails->bLookon;				
		StringCchCopy(LogonMobile.szTablePasswd, CountArray(LogonMobile.szTablePasswd), pLogonBattleDetails->szTablePasswd);

		//Լս��Ϣ
		LogonMobile.dwClubID = pRelationParameter->dwClubID;
		LogonMobile.cbScoreKind = pRelationParameter->cbScoreKind;
		LogonMobile.dwBattleNum = pRelationParameter->dwRelationNum;
	}

	//��λ����
	if (m_pGameServiceOption->wServerType & GAME_GENRE_RANKING)
	{
		//У������
		ASSERT(wDataSize <= sizeof(CMD_GR_LogonMobile) + sizeof(tagLogonRankingDetails));
		if (wDataSize > sizeof(CMD_GR_LogonMobile) + sizeof(tagLogonRankingDetails)) return false;

		//��ȡ����
		tagLogonRankingDetails* pLogonRankingDetails = (tagLogonRankingDetails*)(pLogonMobile + 1);

		//У������
		if (pLogonRankingDetails->wTableID >= m_TableFrameArray.GetCount())
		{
			ASSERT(FALSE);
			return false;
		}

		//��������
		auto pTableFrame = m_TableFrameArray[pLogonRankingDetails->wTableID];
		if (pTableFrame->ContrastPassword(pLogonRankingDetails->szTablePasswd) == false)
		{
			//����ʧ��
			SendLogonFailure(CWHService::FormatMessage(MIM_SERVER_0117, TEXT("�ܱ�Ǹ��������λ������󣬲����������λ���䣡")), 0, dwSocketID, dwTokenID);

			//�Ͽ�����
			if (LOWORD(dwSocketID) >= INDEX_ROBOT)
			{
				m_RobotUserManager.DeleteRobotUserItem(dwSocketID, false);
			}
			else
			{
				//����ṹ
				CMD_CS_C_ShutdownSocket ShutdownSocket;
				ShutdownSocket.bForceClose = false;

				//��������
				SendDataToGate(dwSocketID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));
			}

			return true;
		}

		//��λ��Ϣ	
		LogonMobile.bLookon = pLogonRankingDetails->bLookon;
		LogonMobile.wTableID = pTableFrame->GetTableID();
		LogonMobile.wChairID = pLogonRankingDetails->wChairID;		
		LogonMobile.dwRankingNum = pTableFrame->GetRelationParameter()->dwRelationNum;
		StringCchCopy(LogonMobile.szTablePasswd, CountArray(LogonMobile.szTablePasswd), pLogonRankingDetails->szTablePasswd);
	}
 
	//������Ϣ
	StringCchCopy(LogonMobile.szMachineID,CountArray(LogonMobile.szMachineID), pLogonMobile->szMachineID);

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_MOBILE, LogonMobile.dwUserID,dwSocketID,dwTokenID,&LogonMobile,sizeof(LogonMobile));

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserRule(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GR_UserRule));
	if (wDataSize<sizeof(CMD_GR_UserRule)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��Ϣ����
	auto pUserRule=pIServerUserItem->GetUserRule();
	CMD_GR_UserRule * pCMDUserRule=(CMD_GR_UserRule *)pData;

	//�����־
	pUserRule->bLimitSameIP=((pCMDUserRule->cbRuleMask&UR_LIMIT_SAME_IP)>0);
	pUserRule->bLimitWinRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_WIN_RATE)>0);
	pUserRule->bLimitFleeRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_FLEE_RATE)>0);
	pUserRule->bLimitGameScore=((pCMDUserRule->cbRuleMask&UR_LIMIT_GAME_SCORE)>0);

	//��������
	pUserRule->szPassword[0]=0;
	pUserRule->wLessWinRate=pCMDUserRule->wLessWinRate;
	pUserRule->wMaxFleeRate=pCMDUserRule->wMaxFleeRate;
	pUserRule->lMaxGameScore=pCMDUserRule->lMaxGameScore;
	pUserRule->lLessGameScore=pCMDUserRule->lLessGameScore;

	//��������
	bool bUserSecreTable = false;
	if (wDataSize>sizeof(CMD_GR_UserRule))
	{
		//��������
		VOID * pDataBuffer=NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pCMDUserRule+1,wDataSize-sizeof(CMD_GR_UserRule));

		//��ȡ����
		while (true)
		{
			//��ȡ����
			pDataBuffer=RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe==DTP_NULL) break;

			//���ݷ���
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_GR_TABLE_PASSWORD:		//��������
				{
					ASSERT(pDataBuffer!=NULL);
					ASSERT(DataDescribe.wDataSize<=sizeof(pUserRule->szPassword));
					if (DataDescribe.wDataSize<=sizeof(pUserRule->szPassword))
					{
						bUserSecreTable = true;
						CopyMemory(&pUserRule->szPassword,pDataBuffer,DataDescribe.wDataSize);
						pUserRule->szPassword[CountArray(pUserRule->szPassword)-1]=0;
					}
					break;
				}
			}
		}
	}

	//�����¼�
	if( pIServerUserItem->GetTableID() != INVALID_TABLE )
	{
		CTableFrameItem * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
		pTableFrame->OnEventUserRule(pIServerUserItem);
	}

	return true;
}

//�û��Թ�
bool CAttemperEngineSink::OnTCPNetworkSubUserLookon(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	CMD_GR_UserLookon * pUserLookon=(CMD_GR_UserLookon *)pData;

	//Ч�����
	ASSERT (wDataSize>sizeof(CMD_GR_UserLookon)-sizeof(pUserLookon->szPassword) &&  wDataSize<=sizeof(CMD_GR_UserLookon));
	if (wDataSize <= sizeof(CMD_GR_UserLookon)-sizeof(pUserLookon->szPassword) || wDataSize>sizeof(CMD_GR_UserLookon))
	{
		return false;
	}

	//Ч������
	if (pUserLookon->wChairID>=m_pGameServiceAttrib->wChairCount) return false;
	if (pUserLookon->wTableID>=(WORD)m_TableFrameArray.GetCount()) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�ж�����
	if (pUserLookon->szPassword[0] != 0) pIServerUserItem->SetTmpPassword(pUserLookon->szPassword);

	//��Ϣ����
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	if ((wTableID==pUserLookon->wTableID)&&(wChairID==pUserLookon->wChairID)&&(cbUserStatus==US_LOOKON)) return true;

	//�û��ж�
	if (cbUserStatus==US_PLAYING)
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0010, TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��")),0);
		return true;
	}

	/*if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0 && pIServerUserItem->GetMasterOrder()==0)
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0011, TEXT("��Ǹ���������䲻�����Թۣ�")),0);
		return true;
	}*/

	//�뿪����
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
		CTraceService::TraceString(TEXT("�û�������OnTCPNetworkSubUserLookon"), TraceLevel_Info);
		if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
	}

	//Ȩ���ж�
	if( pIServerUserItem->GetMasterOrder() == 0 && g_GlobalUnits.IsForbidLookon() )
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0012, TEXT("��Ǹ���˷��䲻�����Թ���Ϸ��")),0);
		return true;
	}

	//���´���
	CTableFrameItem * pTableFrame=m_TableFrameArray[pUserLookon->wTableID];
	pTableFrame->PerformLookonAction(pUserLookon->wChairID,pIServerUserItem);

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserSitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	CMD_GR_UserSitDown * pUserSitDown=(CMD_GR_UserSitDown *)pData;

	//Ч�����
	ASSERT (wDataSize>sizeof(CMD_GR_UserSitDown)-sizeof(pUserSitDown->szPassword) && wDataSize <= sizeof(CMD_GR_UserSitDown));
	if (wDataSize<=sizeof(CMD_GR_UserSitDown)-sizeof(pUserSitDown->szPassword) || wDataSize>sizeof(CMD_GR_UserSitDown)) 
		return false;

	//Ч������
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_MATCH|TABLE_GENRE_DISTRIBUTE|GAME_GENRE_BATTLE|GAME_GENRE_RANKING))==0)
	{
		if (pUserSitDown->wChairID>=m_pGameServiceAttrib->wChairCount) return false;
		if (pUserSitDown->wTableID>=(WORD)m_TableFrameArray.GetCount()) return false;
	}

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�ж�����
	if( pUserSitDown->szPassword[0] != 0 )
		pIServerUserItem->SetTmpPassword( pUserSitDown->szPassword );

	//��Ϣ����
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
	{
		if (pUserSitDown->wTableID<m_TableFrameArray.GetCount() && pUserSitDown->wChairID<m_pGameServiceAttrib->wChairCount)
		{
			if ((wTableID==pUserSitDown->wTableID)&&(wChairID==pUserSitDown->wChairID)&&(cbUserStatus!=US_LOOKON)) return true;
		}
	}

	//�û��ж�
	if (cbUserStatus==US_PLAYING)
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0014, TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��")),0);
		return true;
	}

	//�뿪����
	if ((cbUserStatus!=US_LOOKON && wTableID!=INVALID_TABLE) || (cbUserStatus == US_LOOKON && wTableID != pUserSitDown->wTableID))
	{
		CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem,true)==false) return true;
	} 
 
	//��߽�������ж�
	if (m_pGameServiceOption->lMaxEnterScore!=0L && pIServerUserItem->GetUserScore()>m_pGameServiceOption->lMaxEnterScore)	
	{
		LPCTSTR pszMessage = TEXT("");
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0)
		{
			pszMessage = CWHService::FormatMessageV(MIM_SERVER_0017, TEXT("���ã�������ֻ����Ϸ��Ϊ" SCORE_STRING "���µ���Ҳ��ܲμ���Ϸ����������ķ�����Ϸ��"), SCORE_ZOOMOUT(m_pGameServiceOption->lMaxEnterScore));
		}

		SendRequestFailure(pIServerUserItem, pszMessage, 0);
		return true;
	}

	//ϵͳ����
	if ((m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)!=0)
	{
		return InsertDistribute(pIServerUserItem);
	}	

	//��ȡ����
	WORD wRequestTableID=pUserSitDown->wTableID;
	WORD wRequestChairID=pUserSitDown->wChairID;

	//��������
	if (wRequestTableID>=m_TableFrameArray.GetCount())
	{
		//��������
		wRequestTableID=SearchSuitableTable(SEARCH_MODE_MAX_PLAYER,pIServerUserItem);	

		//��Ч����
		if (wRequestTableID==INVALID_TABLE)
		{
			SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0020, TEXT("û���ҵ����õ����ӣ����Ժ����ԣ�")),0);

			return true;
		}
	}

	//��������
	if (wRequestChairID>=m_pGameServiceAttrib->wChairCount)
	{
		//��������
		CTableFrameItem * pTableFrame=m_TableFrameArray[wRequestTableID];

		//��������
		wRequestChairID=INVALID_CHAIR;

		//�ж�����
		if (wRequestChairID==INVALID_CHAIR)
		{
			wRequestChairID=pTableFrame->GetRandNullChairID();			
		}

		//��Ч����
		if (wRequestChairID==INVALID_CHAIR)
		{
			if (m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE)
			{
				SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0021, TEXT("�Բ����������ҵ�Լս��������������")),0);
			}
			else
			{
				SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0022, TEXT("û���ҵ����õ����ӣ����Ժ����ԣ�")),0);
			}

			return true;
		}
	}	

	//���´���
	CTableFrameItem * pTableFrame=m_TableFrameArray[wRequestTableID];
	pTableFrame->PerformSitDownAction(wRequestChairID,pIServerUserItem);

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserStandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_UserStandUp));
	if (wDataSize!=sizeof(CMD_GR_UserStandUp)) return false;

	//Ч������
	CMD_GR_UserStandUp * pUserStandUp=(CMD_GR_UserStandUp *)pData;
	if (pUserStandUp->wChairID>=m_pGameServiceAttrib->wChairCount) return false;
	if (pUserStandUp->wTableID>=(WORD)m_TableFrameArray.GetCount()) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//������Ϣ
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();

	//����ģʽ
	if (m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)
	{
		//�Ƴ�����
		RemoveDistribute(pIServerUserItem);

		//�˳��ж�
		if (wTableID==INVALID_TABLE) return true;
	}

	//��Ϣ����	
	if ((wTableID!=pUserStandUp->wTableID)||(wChairID!=pUserStandUp->wChairID)) return true;

	//�û��ж�
	if ((pUserStandUp->cbForceLeave==FALSE)&&(pIServerUserItem->GetUserStatus()==US_PLAYING))
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0010, TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��")), 0);
		return true;
	}

	//�뿪����
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem,true)==false) return true;
	}

	return true;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//��������
	CMD_GR_S_UserChat * pUserChat=(CMD_GR_S_UserChat *)pData;

	//Ч�����
	ASSERT(wDataSize>=(sizeof(CMD_GR_S_UserChat)-sizeof(pUserChat->szChatString)));
	ASSERT(wDataSize==(sizeof(CMD_GR_S_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(TCHAR)));

	//Ч�����
	if (wDataSize<(sizeof(CMD_GR_S_UserChat)-sizeof(pUserChat->szChatString))) return false;
	if (wDataSize!=(sizeof(CMD_GR_S_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(TCHAR))) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//�û�Ч��
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//Ѱ���û�
	IServerUserItem * pIRecvUserItem=NULL;
	if (pUserChat->dwTargetUserID!=0)
	{
		pIRecvUserItem=m_ServerUserManager.SearchUserItem(pUserChat->dwTargetUserID);
		if (pIRecvUserItem==NULL) return true;
	}

	//״̬�ж�
	if ((g_GlobalUnits.IsAllowRoomChat()==false)&&(pISendUserItem->GetMasterOrder()==0))
	{
		//���ǻ�Ա
		if( !g_GlobalUnits.IsMemberCanChat() || pISendUserItem->GetMemberOrder() == 0 )
		{
			SendRoomMessage(pISendUserItem, CWHService::FormatMessage(MIM_SERVER_0023, TEXT("��Ǹ������Ϸ���䲻����������ģ�")),SMT_CHAT);
			return true;
		}
	}

	//Ȩ���ж�
	if (CUserRight::CanRoomChat(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem, CWHService::FormatMessage(MIM_SERVER_0024, TEXT("��Ǹ����û�д������ĵ�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��")),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//������Ϣ
	CMD_GR_R_UserChat UserChat;
	ZeroMemory(&UserChat,sizeof(UserChat));
	
	StringCchCopy(UserChat.szChatString,CountArray(UserChat.szChatString), pUserChat->szChatString);


	//��ʱ����
// 	LARGE_INTEGER nFreq, nBefore, nAfter;
// 	DWORD dwTime;
// 	memset(&nFreq, 0x00, sizeof nFreq);
// 	memset(&nBefore, 0x00, sizeof nBefore);
// 	memset(&nAfter, 0x00, sizeof nAfter);
// 	dwTime = 0;
// 	QueryPerformanceFrequency(&nFreq);
// 	QueryPerformanceCounter(&nBefore); 

	m_WordsFilter.FilterWords(UserChat.szChatString,CountArray(UserChat.szChatString));

//	QueryPerformanceCounter(&nAfter);
//	dwTime =(DWORD)((nAfter.QuadPart - nBefore.QuadPart) *100000/ nFreq.QuadPart);
//	CString str;
//	str.Format(TEXT("��ʱ��%d����"),dwTime);
//	CTraceService::TraceString(str,TraceLevel_Debug);

	//��������
	UserChat.dwChatColor=pUserChat->dwChatColor;
	UserChat.wChatLength=pUserChat->wChatLength;
	UserChat.dwSendUserID=pISendUserItem->GetUserID();
	UserChat.dwTargetUserID=pUserChat->dwTargetUserID;
	UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

	//ת����Ϣ
	WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
	SendBatchData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_CHAT,&UserChat,wHeadSize+UserChat.wChatLength*sizeof(TCHAR));

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubDebugPacket(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч������
	ASSERT (wDataSize >= sizeof(CMD_GR_DebugPacket));
	if (wDataSize < sizeof(CMD_GR_DebugPacket)) return false;

	//��ȡ����
	auto pDebugPacket = (CMD_GR_DebugPacket*)pData;	

	//��ȡ�û�
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem* pIServerUserItem = GetServerUserItem(wBindIndex, dwTokenID);
	if (pIServerUserItem == NULL) return false;

	//У��Ȩ��
	if (!CUserRight::CanCheat(pIServerUserItem->GetUserRight()))
	{
		return false;
	}

	//��Ϣ���� 
	if (pDebugPacket->wTableID != INVALID_TABLE)
	{
		//У������
		ASSERT (pDebugPacket->wTableID < m_TableFrameArray.GetCount());
		if (pDebugPacket->wTableID >= m_TableFrameArray.GetCount()) return false;

		auto pTableFrame = m_TableFrameArray[pDebugPacket->wTableID];
		pTableFrame->OnEventSocketFrame(pDebugPacket->wGameCmdID, pDebugPacket+1, wDataSize-sizeof(CMD_GR_DebugPacket), pIServerUserItem);
	}
	else
	{
		//����������
		for (BYTE i = 0; i < m_TableFrameArray.GetCount(); i++)
		{
			auto pTableFrame = m_TableFrameArray[i];
			if (pTableFrame != NULL)
			{
				pTableFrame->OnEventSocketFrame(pDebugPacket->wGameCmdID, pDebugPacket+1, wDataSize-sizeof(CMD_GR_DebugPacket), pIServerUserItem);
			}
		}
	}

	return true;
}

//�û�˽��
bool CAttemperEngineSink::OnTCPNetworkSubUserWisper(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID, WORD wSubCmdID)
{
	//��������
	CMD_GR_S_UserWisper * pUserWisper=(CMD_GR_S_UserWisper *)pData;

	//Ч�����
	ASSERT(wDataSize>=(sizeof(CMD_GR_S_UserWisper)-sizeof(pUserWisper->szChatString)));
	ASSERT(wDataSize==(sizeof(CMD_GR_S_UserWisper)-sizeof(pUserWisper->szChatString)+pUserWisper->wChatLength*sizeof(TCHAR)));

	//Ч�����
	if (wDataSize<(sizeof(CMD_GR_S_UserWisper)-sizeof(pUserWisper->szChatString))) return false;
	if (wDataSize!=(sizeof(CMD_GR_S_UserWisper)-sizeof(pUserWisper->szChatString)+pUserWisper->wChatLength*sizeof(TCHAR))) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//�û�Ч��
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//Ѱ���û�
	IServerUserItem * pIRecvUserItem=m_ServerUserManager.SearchUserItem(pUserWisper->dwTargetUserID);
	if (pIRecvUserItem==NULL) return true;

	if( !EfficacyUserWhisper(pISendUserItem,pIRecvUserItem) )
		return true;

	//������Ϣ
	CMD_GR_R_UserWisper UserWisper;
	ZeroMemory(&UserWisper,sizeof(UserWisper));
	StringCchCopy(UserWisper.szChatString,CountArray(UserWisper.szChatString), pUserWisper->szChatString);
	m_WordsFilter.FilterWords(UserWisper.szChatString,CountArray(UserWisper.szChatString));

	//��������
	UserWisper.dwChatColor=pUserWisper->dwChatColor;
	UserWisper.wChatLength=pUserWisper->wChatLength;
	UserWisper.dwSendUserID=pISendUserItem->GetUserID();
	UserWisper.dwTargetUserID=pIRecvUserItem->GetUserID();
	UserWisper.wChatLength=CountStringBuffer(UserWisper.szChatString);

	//ת����Ϣ
	WORD wHeadSize=sizeof(UserWisper)-sizeof(UserWisper.szChatString);
	if (wSubCmdID == SUB_GR_USER_WISPER)
	{
		SendDataToUser(pISendUserItem, MDM_GR_USER, SUB_GR_USER_WISPER, &UserWisper, wHeadSize + UserWisper.wChatLength * sizeof(TCHAR));
	}

	SendDataToUser(pIRecvUserItem,MDM_GR_USER,SUB_GR_USER_WISPER,&UserWisper,wHeadSize+UserWisper.wChatLength*sizeof(TCHAR));

	return true;
}

//�����û�
bool CAttemperEngineSink::OnSocketSubInviteUser(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_S_InviteUser));
	if (wDataSize!=sizeof(CMD_GR_S_InviteUser)) return false;

	//Ч������
	CMD_GR_S_InviteUser * pInviteUser=(CMD_GR_S_InviteUser *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�û��ж�
	WORD wMeTableID=pIServerUserItem->GetTableID();
	BYTE cbMeUserStatus=pIServerUserItem->GetUserStatus();
	if ((wMeTableID==INVALID_TABLE)||(cbMeUserStatus==US_LOOKON)||(cbMeUserStatus==US_PLAYING)) return true;

	//�����û�
	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pInviteUser->dwTargetUserID);
	if (pITargetUserItem==NULL) return true;

	//�û��ж�
	BYTE cbUserStatus=pITargetUserItem->GetUserStatus();
	if ((cbUserStatus==US_PLAYING)||(cbUserStatus==US_OFFLINE)) return true;

	//��������
	CMD_GR_R_InviteUser InviteUser;
	ZeroMemory(&InviteUser,sizeof(InviteUser));

	//��������
	InviteUser.wTableID=pIServerUserItem->GetTableID();
	InviteUser.dwSendUserID=pIServerUserItem->GetUserID();

	//��������
	SendDataToUser(pITargetUserItem,MDM_GR_USER,SUB_GR_INVITE_USER,&InviteUser,sizeof(InviteUser));

	return true;
}

//�б�����
bool CAttemperEngineSink::OnDBColumnItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(tagColumnItem)==0);
	if (wDataSize%sizeof(tagColumnItem)!=0) return false;

	//��Ŀ����
	BYTE cbColumnCount=(BYTE)(wDataSize/sizeof(tagColumnItem));
	cbColumnCount=__min(cbColumnCount,CountArray(CMD_ConfigColumn.ColumnItem));

	//���ݴ���
	if (cbColumnCount>0)
	{
		//��������
		CMD_ConfigColumn.cbColumnCount=cbColumnCount;
		CopyMemory(CMD_ConfigColumn.ColumnItem,pData,cbColumnCount*sizeof(tagColumnItem));

		//�¼�֪ͨ
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess=ER_SUCCESS;
		SendUIControlPacket(UI_SERVICE_CONFIG_RESULT,&ControlResult,sizeof(ControlResult));
	}
	else
	{
		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("��Ϸ������Ϣ��ȡʧ��"),TraceLevel_Exception);

		//�¼�֪ͨ
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess=ER_FAILURE;
		SendUIControlPacket(UI_SERVICE_CONFIG_RESULT,&ControlResult,sizeof(ControlResult));
	}

	return true;
}

//���ý��
bool CAttemperEngineSink::OnDBConfigResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//Ч�����
	ASSERT (wDataSize==sizeof(DBO_GR_ConfigResult));
	if (wDataSize != sizeof(DBO_GR_ConfigResult)) return false;

	//��ȡ����
	DBO_GR_ConfigResult* pConfigResult = (DBO_GR_ConfigResult*)pData;

	//����ṹ
	CP_ControlResult ControlResult;
	ZeroMemory(&ControlResult,sizeof(ControlResult));

	//���ݴ���
	if (pConfigResult->bResultCode==true)
	{
		//��Ϸ�ȼ�
		if (pConfigResult->dwConfigMask & CONFIG_MASK_GAME_LEVEL)
		{
			g_GameLevelManager->ResetData();
			g_GameLevelManager->AppendGameLevel(g_GameLevelManager->m_ArrayGameLevel.GetData(), g_GameLevelManager->m_ArrayGameLevel.GetCount());
			g_GameLevelManager->AppendUserLevel(g_GameLevelManager->m_ArrayUserLevel.GetData(), g_GameLevelManager->m_ArrayUserLevel.GetCount());
			g_GameLevelManager->AppendRankLevel(g_GameLevelManager->m_ArrayRankLevel.GetData(), g_GameLevelManager->m_ArrayRankLevel.GetCount());
		}

		//��Ϸ��Ʒ
		if (pConfigResult->dwConfigMask & CONFIG_MASK_GAME_GOODS)
		{
			g_GameGoodsManager->ResetData();
			g_GameGoodsManager->AppendGameGoods(g_GameGoodsManager->m_ArrayGoodsInfo.GetData(), g_GameGoodsManager->m_ArrayGoodsInfo.GetCount());
		}

		//�л�����
		if (pConfigResult->dwConfigMask & GLOBAL_OPTION_MASK)
		{
			g_GlobalOptionManager->SwitchOptionItem(pConfigResult->dwConfigMask);
		}

		//�¼�֪ͨ
		ControlResult.cbSuccess = ER_SUCCESS;		
	}
	else
	{
		ControlResult.cbSuccess = ER_FAILURE;		
	}

	//�¼�֪ͨ
	if (pConfigResult->bInitService == true)
	{
		SendUIControlPacket(UI_SERVICE_CONFIG_RESULT, &ControlResult, sizeof(ControlResult));
	}	

	return true;
}

//�������д�
bool CAttemperEngineSink::OnDBFilterWords(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	if(dwContextID==0xffff)			//�������
	{
		return true;
	}
	else if(dwContextID==0xfffe)	//��ʼ����
	{
		m_WordsFilter.ResetFilter();
		return true;			
	}

	const TCHAR *pWords=(const TCHAR*)pData;
	m_WordsFilter.InsertKeyword(pWords);

	return true;
}

//��Ϸѡ��
bool CAttemperEngineSink::OnDBBattleOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У������
	ASSERT (wDataSize%sizeof(tagBattleGameOptionEx)==0);
	if (wDataSize%sizeof(tagBattleGameOptionEx)!=0) return false;

	//��������
	WORD wOptionCount=wDataSize/sizeof(tagBattleGameOptionEx);
	tagBattleGameOptionEx * pBattleGameOptionEx = (tagBattleGameOptionEx *)pData;

	//�Ƴ�����
	m_BattleGameOptionArray.RemoveAll();

	//��������
	for (WORD i=0; i<wOptionCount; i++)
	{
		m_BattleGameOptionArray.Add(*pBattleGameOptionEx++);
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBRobotUser(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_GameRobotInfo * pGameRobotInfo=(DBO_GR_GameRobotInfo *)pData;
	WORD wHeadSize=sizeof(DBO_GR_GameRobotInfo)-sizeof(pGameRobotInfo->RobotAccountsInfo);

	//Ч�����
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameRobotInfo->wRobotCount*sizeof(pGameRobotInfo->RobotAccountsInfo[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameRobotInfo->wRobotCount*sizeof(pGameRobotInfo->RobotAccountsInfo[0])))) return false;

	//���û���
	if (pGameRobotInfo->lResultCode==DB_SUCCESS)
	{
		m_RobotUserManager.InsertRobotInfo(pGameRobotInfo->RobotAccountsInfo,pGameRobotInfo->wRobotCount,pGameRobotInfo->dwBatchID);
	}

	return true;

}

//��������
bool CAttemperEngineSink::OnDBRobotParameter(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��������
	DBO_GR_RobotParameter * pGameRobotParameter=(DBO_GR_RobotParameter *)pData;
	WORD wHeadSize=sizeof(DBO_GR_RobotParameter)-sizeof(pGameRobotParameter->RobotParameter);

	//Ч�����
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameRobotParameter->wParameterCount*sizeof(pGameRobotParameter->RobotParameter[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameRobotParameter->wParameterCount*sizeof(pGameRobotParameter->RobotParameter[0])))) return false;

	//���û���
	if (pGameRobotParameter->lResultCode==DB_SUCCESS)
	{
		m_RobotUserManager.AddRobotParameter(pGameRobotParameter->RobotParameter,pGameRobotParameter->wParameterCount);
	}

	return true;
}

//�ͱ�����
bool CAttemperEngineSink::OnDBSubsidyParameter(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize==sizeof(DBO_GR_SubsidyParameter));
	if(wDataSize!=sizeof(DBO_GR_SubsidyParameter)) return false;

	//��ȡ����
	auto pSubsidyParameter =(DBO_GR_SubsidyParameter*)pData;

	//���ñ���
	m_SystemParameter.bLoadFlag=true;
	m_SystemParameter.lTakeSubsidyGold= pSubsidyParameter->lTakeSubsidyGold;
	m_SystemParameter.lTakeSubsidyCondition= pSubsidyParameter->lTakeSubsidyCondition;

	return true;
}

//���Ʋ���
bool CAttemperEngineSink::OnDBControlParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize == sizeof(DBO_GR_ControlParameter));
	if (wDataSize != sizeof(DBO_GR_ControlParameter)) return false;

	//��ȡ����
	auto pControlParameter = (DBO_GR_ControlParameter*)pData;

	//���Ʋ���
	m_ControlParameter.bControlEnabled = pControlParameter->bControlEnabled;
	m_ControlParameter.lBenchmarkValue = pControlParameter->lBenchmarkValue;
	m_ControlParameter.wWLControlRate = pControlParameter->wWLControlRate;
	m_ControlParameter.wBLControlRate = pControlParameter->wBLControlRate;
	m_ControlParameter.wStartPlayCount = pControlParameter->wStartPlayCount;
	m_ControlParameter.wInWhiteListRate = pControlParameter->wInWhiteListRate;
	m_ControlParameter.wInBlackListRate = pControlParameter->wInBlackListRate;
	m_ControlParameter.wOutWhiteListRate = pControlParameter->wOutWhiteListRate;
	m_ControlParameter.wOutBlackListRate = pControlParameter->wOutBlackListRate;	
	m_ControlParameter.wSupportControlMode = pControlParameter->wSupportControlMode;

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBLogonFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//�����ж�
	WORD wBindIndex = LOWORD(dwContextID);
	tagTokenParameter * pTokenParameter = GetTokenParameter(wBindIndex, dwTokenID);
	if (pTokenParameter == NULL) return false;

	//�����ж�
	auto pIServerUserItem = (IServerUserItem*)pTokenParameter->pBindUserItem;
	if (pIServerUserItem != NULL) return false;

	//���ʹ���
	DBO_GR_LogonFailure * pLogonFailure=(DBO_GR_LogonFailure *)pData;	

	//��������
	CMD_GR_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure, sizeof(LogonFailure));

	//������Ϣ
	LogonFailure.wLockKindID = pLogonFailure->wLockKindID;
	LogonFailure.wLockServerID = pLogonFailure->wLockServerID;
	LogonFailure.wLockServerKind = pLogonFailure->wLockServerKind;

	//������Ϣ
	LogonFailure.lErrorCode = pLogonFailure->lErrorCode;
	StringCchCopy(LogonFailure.szDescribeString, CountArray(LogonFailure.szDescribeString), pLogonFailure->szDescribeString);

	//��������
	WORD wStringSize = CountStringBuffer(LogonFailure.szDescribeString);
	WORD wSendSize = sizeof(LogonFailure) - sizeof(LogonFailure.szDescribeString) + wStringSize;

	//��������
	SendDataToGate(dwContextID, dwTokenID, MDM_GR_LOGON, SUB_GR_LOGON_FAILURE, &LogonFailure, wSendSize);

	//�Ͽ�����
	if (LOWORD(dwContextID)>=INDEX_ROBOT) 
	{
		m_RobotUserManager.DeleteRobotUserItem(dwContextID,false);
	}
	else
	{
		//����ṹ
		CMD_CS_C_ShutdownSocket ShutdownSocket;
		ShutdownSocket.bForceClose = false;

		//��������
		SendDataToGate(dwContextID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));
	}

	return true;
}

//��¼�ɹ�
bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��������
	WORD wBindIndex=LOWORD(dwContextID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//��ȡ����
	DBO_GR_LogonSuccess* pDBOLogonSuccess = (DBO_GR_LogonSuccess*)pData;

	//���Ʋ���
	tagTokenParameter * pTokenParameter=GetTokenParameter(wBindIndex,dwTokenID);
	if (pTokenParameter == NULL)
	{
		//�������
		DBR_GR_LeaveGameService LeaveGameService;
		ZeroMemory(&LeaveGameService, sizeof(LeaveGameService));

		//�û���Ϣ
		LeaveGameService.dwUserID = pDBOLogonSuccess->dwUserID;
		if (pDBOLogonSuccess->lFreeScore > 0L)
			LeaveGameService.lUnLockScore = pDBOLogonSuccess->lFreeScore;
		else
			LeaveGameService.lUnLockScore = 0;

		//��ʤ��Ϣ
		LeaveGameService.lWinStreak = pDBOLogonSuccess->lWinStreak;
		LeaveGameService.lMaxWinStreak = pDBOLogonSuccess->lMaxWinStreak;

		//������Ϣ
		LeaveGameService.cbLeaveReason = LEAVE_GAME_USER_CANCEL;
		LeaveGameService.dwInoutIndex = pDBOLogonSuccess->dwInoutIndex;

		//������Ϣ
		LeaveGameService.dwClientIP = pBindParameter->dwClientIP;
		StringCchCopy(LeaveGameService.szMachineID, LEN_MACHINE_ID, pDBOLogonSuccess->szMachineID);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_SERVICE, pDBOLogonSuccess->dwUserID, 0L, 0L, &LeaveGameService, sizeof(LeaveGameService));

		return true;
	}	
 
	//�����ж�
	if (pBindParameter->dwContextID!=dwContextID || pTokenParameter->dwTokenID!=dwTokenID)
	{
		//�������
		DBR_GR_LeaveGameService LeaveGameService;
		ZeroMemory(&LeaveGameService,sizeof(LeaveGameService));

		//�û���Ϣ
		LeaveGameService.dwUserID=pDBOLogonSuccess->dwUserID;
		if (pDBOLogonSuccess->lFreeScore>0L)
			LeaveGameService.lUnLockScore=pDBOLogonSuccess->lFreeScore;
		else
			LeaveGameService.lUnLockScore=0;

		//��ʤ��Ϣ
		LeaveGameService.lWinStreak = pDBOLogonSuccess->lWinStreak;
		LeaveGameService.lMaxWinStreak = pDBOLogonSuccess->lMaxWinStreak;

		//������Ϣ
		LeaveGameService.cbLeaveReason=LEAVE_GAME_BIND_ERROR;
		LeaveGameService.dwInoutIndex = pDBOLogonSuccess->dwInoutIndex;

		//������Ϣ
		LeaveGameService.dwClientIP=pBindParameter->dwClientIP;
		StringCchCopy(LeaveGameService.szMachineID, LEN_MACHINE_ID, pDBOLogonSuccess->szMachineID);		

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_SERVICE, pDBOLogonSuccess->dwUserID,0L,0L,&LeaveGameService,sizeof(LeaveGameService));

		return true;
	}

	//��չ����
	auto pExtendParameter = (tagExtendParameter*)pTokenParameter->cbExtendData;

	//��������
	bool bRobotUser=(wBindIndex>=INDEX_ROBOT);
	bool bMobileUser=(pExtendParameter->cbClientKind==CLIENT_KIND_MOBILE);

	//�л��ж�
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pDBOLogonSuccess->dwUserID);
	if (pIServerUserItem!=NULL)
	{		
		//��������
		DBR_GR_LeaveGameService LeaveGameService;
		ZeroMemory(&LeaveGameService,sizeof(LeaveGameService));

		//�û���Ϣ
		LeaveGameService.dwUserID=pDBOLogonSuccess->dwUserID;

		//�������
		if (pDBOLogonSuccess->lFreeScore>0L)
			LeaveGameService.lUnLockScore=pDBOLogonSuccess->lFreeScore;
		else 
			LeaveGameService.lUnLockScore=-0;

		//��ʤ����
		LeaveGameService.lWinStreak = pDBOLogonSuccess->lWinStreak;
		LeaveGameService.lMaxWinStreak = pDBOLogonSuccess->lMaxWinStreak;

		//������Ϣ
		LeaveGameService.cbLeaveReason=LEAVE_GAME_TWICE_LOGIN;
		LeaveGameService.dwInoutIndex=pDBOLogonSuccess->dwInoutIndex;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_SERVICE, pDBOLogonSuccess->dwUserID,0L,0L,&LeaveGameService,sizeof(LeaveGameService));

		//�л��û�
		SwitchUserItemConnect(pIServerUserItem,wBindIndex,dwTokenID,bMobileUser);

		return true;
	}

	//��߽�������ж�	
	if (m_pGameServiceOption->lMaxEnterScore != 0L && pDBOLogonSuccess->lGold > m_pGameServiceOption->lMaxEnterScore && pDBOLogonSuccess->cbMasterOrder==0)
	{
		//��������
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//������Ϣ
		LPCTSTR pszMessage = TEXT("");
		if ((m_pGameServiceOption->wServerType & (GAME_GENRE_GOLD)) != 0)
		{
			pszMessage = CWHService::FormatMessageV(MIM_SERVER_0017, TEXT("���ã�������ֻ�н��Ϊ" SCORE_STRING "���µ���Ҳſ��Խ��룡"), SCORE_ZOOMOUT(m_pGameServiceOption->lMaxEnterScore));
		}
		else
		{
			pszMessage = CWHService::FormatMessageV(MIM_SERVER_0019, TEXT("���ã�������ֻ����Ϸ����Ϊ" SCORE_STRING "���µ���Ҳſ��Խ��룡"), SCORE_ZOOMOUT(m_pGameServiceOption->lMaxEnterScore));
		}

		StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), pszMessage);

		//��������
		SystemMessage.wLength=lstrlen(pszMessage)+1;
		SystemMessage.wType=SMT_EJECT|SMT_CHAT|SMT_CLOSE_ROOM|SMT_CLOSE_LINK;

		//��������
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//��������
		SendDataToGate(pBindParameter->dwContextID, dwTokenID, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wSendSize);

		//�������
		//��������
		DBR_GR_LeaveGameService LeaveGameService;
		ZeroMemory(&LeaveGameService,sizeof(LeaveGameService));

		//�û���Ϣ
		LeaveGameService.dwUserID=pDBOLogonSuccess->dwUserID;
		if (pDBOLogonSuccess->lFreeScore > 0L)
		{
			LeaveGameService.lUnLockScore = pDBOLogonSuccess->lFreeScore;
		}
		else
		{
			LeaveGameService.lUnLockScore = 0;
		}

		//��ʤ����
		LeaveGameService.lWinStreak = pDBOLogonSuccess->lWinStreak;
		LeaveGameService.lMaxWinStreak = pDBOLogonSuccess->lMaxWinStreak;

		//������Ϣ
		LeaveGameService.cbLeaveReason=LEAVE_GAME_THAN_MAX_SCORE;
		LeaveGameService.dwInoutIndex = pDBOLogonSuccess->dwInoutIndex;

		//������Ϣ
		LeaveGameService.dwClientIP=pBindParameter->dwClientIP;
		StringCchCopy(LeaveGameService.szMachineID, LEN_MACHINE_ID, pDBOLogonSuccess->szMachineID);		
  
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_SERVICE, pDBOLogonSuccess->dwUserID,0L,0L,&LeaveGameService,sizeof(LeaveGameService));

		return true;
	}

	//�����ж�
	WORD wMaxPlayer=m_pGameServiceOption->wMaxPlayer;
	DWORD dwOnlineCount=m_ServerUserManager.GetUserItemCount();
	WORD wMemberOrder = g_GameLevelManager->CalcUserOrder(pDBOLogonSuccess->dwMemberPoint);
	if (bMobileUser==false&&(wMemberOrder==0)&&(pDBOLogonSuccess->cbMasterOrder==0)&&(dwOnlineCount>(DWORD)(wMaxPlayer-20L)))
	{
		//��������
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//������Ϣ
		StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), CWHService::FormatMessage(MIM_SERVER_0031, TEXT("��Ǹ�����ڴ˷����Ѿ���������ͨ��Ҳ��ܼ��������ˣ�")));

		//��������
		SystemMessage.wLength=lstrlen(SystemMessage.szString)+1;
		SystemMessage.wType=SMT_EJECT|SMT_CHAT|SMT_CLOSE_ROOM|SMT_CLOSE_LINK;

		//��������
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//��������
		SendDataToGate(pBindParameter->dwContextID, dwTokenID, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wSendSize);		

		//��������
		DBR_GR_LeaveGameService LeaveGameService;
		ZeroMemory(&LeaveGameService,sizeof(LeaveGameService));

		//�û���Ϣ
		LeaveGameService.dwUserID=pDBOLogonSuccess->dwUserID;
		if (pDBOLogonSuccess->lFreeScore>0L)
			LeaveGameService.lUnLockScore=pDBOLogonSuccess->lFreeScore;
		else
			LeaveGameService.lUnLockScore=0;

		//��ʤ����
		LeaveGameService.lWinStreak = pDBOLogonSuccess->lWinStreak;
		LeaveGameService.lMaxWinStreak = pDBOLogonSuccess->lMaxWinStreak;

		//������Ϣ
		LeaveGameService.cbLeaveReason=LEAVE_GAME_FULL;
		LeaveGameService.dwInoutIndex=pDBOLogonSuccess->dwInoutIndex;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_SERVICE, pDBOLogonSuccess->dwUserID,0L,0L,&LeaveGameService,sizeof(LeaveGameService));
		
		return true;
	}

	//��ֹ�ж�
	if ((g_GlobalUnits.IsAllowEnterRoom()==false)&&(pDBOLogonSuccess->cbMasterOrder==0))
	{
		//��������
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//������Ϣ
		StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), CWHService::FormatMessage(MIM_SERVER_0032, TEXT("��Ǹ������ϵͳά����ԭ�򣬴���Ϸ������ʱ�������û����룡")));

		//��������
		SystemMessage.wLength=lstrlen(SystemMessage.szString)+1;
		SystemMessage.wType=SMT_EJECT|SMT_CHAT|SMT_CLOSE_ROOM|SMT_CLOSE_LINK;

		//��������
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//��������
		SendDataToGate(pBindParameter->dwContextID, dwTokenID, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wSendSize);

		//�������
		//��������
		DBR_GR_LeaveGameService LeaveGameService;
		ZeroMemory(&LeaveGameService,sizeof(LeaveGameService));

		//�û���Ϣ
		LeaveGameService.dwUserID=pDBOLogonSuccess->dwUserID;
		if (pDBOLogonSuccess->lFreeScore>0L)
			LeaveGameService.lUnLockScore=pDBOLogonSuccess->lFreeScore;
		else
			LeaveGameService.lUnLockScore=0;

		//��ʤ����
		LeaveGameService.lWinStreak = pDBOLogonSuccess->lWinStreak;
		LeaveGameService.lMaxWinStreak = pDBOLogonSuccess->lMaxWinStreak;

		//������Ϣ
		LeaveGameService.cbLeaveReason=LEAVE_GAME_MANAGEMENT;
		LeaveGameService.dwInoutIndex = pDBOLogonSuccess->dwInoutIndex;

		//������Ϣ
		LeaveGameService.dwClientIP=pBindParameter->dwClientIP;
		StringCchCopy(LeaveGameService.szMachineID, LEN_MACHINE_ID, pDBOLogonSuccess->szMachineID);		

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_SERVICE, pDBOLogonSuccess->dwUserID,0L,0L,&LeaveGameService,sizeof(LeaveGameService));

		return true;
	}

	//���һ���
	if (bRobotUser==true && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
	{
		//���һ���
		DWORD dwUserID=pDBOLogonSuccess->dwUserID;
		CRobotUserItem * pRobotUserItem = (CRobotUserItem *) m_RobotUserManager.SearchRobotUserItem(dwUserID);

		//�޸Ļ���
		if (pRobotUserItem !=NULL)
		{
			//��ȡ����
			tagRobotParameter * pRobotParameter= pRobotUserItem->GetRobotParameter();

			//��������
			if ((pRobotParameter->lTakeMinScore!=0L)&&(pRobotParameter->lTakeMaxScore!=0L))
			{
				//��������
				SCORE lTakeMinScore=(SCORE)pRobotParameter->lTakeMinScore;
				SCORE lTakeMaxScore=(SCORE)__max(pRobotParameter->lTakeMaxScore,pRobotParameter->lTakeMinScore);

				//��������
				if ((lTakeMaxScore-lTakeMinScore)>0L)
				{
					SCORE lTakeScore = (lTakeMaxScore-lTakeMinScore)/10;
					pDBOLogonSuccess->lScore=(SCORE)(lTakeMinScore + (SCORE)(Random_Double*lTakeScore)*10);
				}
				else
				{
					pDBOLogonSuccess->lScore=(SCORE)lTakeMaxScore;					
				}

				pDBOLogonSuccess->lFreeScore=pDBOLogonSuccess->lScore;
			}
		}
	}

	//�û�����
	tagServerUserInfo UserInfo;
	tagUserInfoPlus UserInfoPlus;
	ZeroMemory(&UserInfo,sizeof(UserInfo));
	ZeroMemory(&UserInfoPlus,sizeof(UserInfoPlus));

	//��������
	UserInfo.wFaceID=pDBOLogonSuccess->wFaceID;
	UserInfo.dwUserID=pDBOLogonSuccess->dwUserID;
	UserInfo.dwGameID=pDBOLogonSuccess->dwGameID;	
	UserInfo.dwCustomID=pDBOLogonSuccess->dwCustomID;	
	StringCchCopy(UserInfo.szAccounts,CountArray(UserInfo.szAccounts), pDBOLogonSuccess->szAccounts);

	//�û�����
	UserInfo.cbAge=pDBOLogonSuccess->cbAge;
	UserInfo.cbGender=pDBOLogonSuccess->cbGender;
	StringCchCopy(UserInfo.szArea,CountArray(UserInfo.szArea), pDBOLogonSuccess->szArea);
	StringCchCopy(UserInfo.szCity,CountArray(UserInfo.szCity), pDBOLogonSuccess->szCity);
	StringCchCopy(UserInfo.szProvince,CountArray(UserInfo.szProvince), pDBOLogonSuccess->szProvince);
	StringCchCopy(UserInfo.szUnderWrite,CountArray(UserInfo.szUnderWrite), pDBOLogonSuccess->szUnderWrite);

	//״̬����
	UserInfo.cbUserStatus=US_FREE;
	UserInfo.wTableID=INVALID_TABLE;
	UserInfo.wChairID=INVALID_CHAIR;

	//������Ϣ	
	UserInfo.lGold=pDBOLogonSuccess->lGold;
	UserInfo.lScore=pDBOLogonSuccess->lScore;
	UserInfo.cbScoreKind=pDBOLogonSuccess->cbScoreKind;
	UserInfo.lUserCard =pDBOLogonSuccess->lUserCard;
	UserInfo.lInsureGold=pDBOLogonSuccess->lInsureGold;	
	UserInfo.lWinCount=pDBOLogonSuccess->lWinCount;
	UserInfo.lLostCount=pDBOLogonSuccess->lLostCount;
	UserInfo.lDrawCount=pDBOLogonSuccess->lDrawCount;
	UserInfo.lFleeCount=pDBOLogonSuccess->lFleeCount;
	UserInfo.lExperience=pDBOLogonSuccess->lExperience;
	UserInfo.lWinStreak=pDBOLogonSuccess->lWinStreak;
	UserInfo.lMaxWinStreak=pDBOLogonSuccess->lMaxWinStreak;

	//�ȼ���Ϣ
	UserInfo.cbMasterOrder = pDBOLogonSuccess->cbMasterOrder;
	UserInfo.dwMemberPoint = pDBOLogonSuccess->dwMemberPoint;
	StringCchCopy(UserInfo.szUserHeadUrl, CountArray(UserInfo.szUserHeadUrl), pDBOLogonSuccess->szUserHeadUrl);//kk jia

	//�û�Ȩ��
	UserInfoPlus.dwUserRight=pDBOLogonSuccess->dwUserRight;
	UserInfoPlus.dwMasterRight=pDBOLogonSuccess->dwMasterRight;

	//λ����Ϣ
	UserInfoPlus.dGlobalPosLat=pDBOLogonSuccess->dGlobalPosLat;
	UserInfoPlus.dGlobalPosLng=pDBOLogonSuccess->dGlobalPosLng;
	CopyMemory(UserInfoPlus.szPlaceName, pDBOLogonSuccess->szPlaceName, sizeof(UserInfoPlus.szPlaceName));

	//״̬��Ϣ
	UserInfoPlus.cbShutUpStatus = pDBOLogonSuccess->cbShutUpStatus;
	UserInfoPlus.dwShutUpOverTime = pDBOLogonSuccess->dwShutUpOverTime;

	//��Ӯ����
	UserInfoPlus.wAddupWinRate = pDBOLogonSuccess->wAddupWinRate;
	UserInfoPlus.lAddupWinScore = pDBOLogonSuccess->lAddupWinScore;
	UserInfoPlus.lAddupLoseScore = pDBOLogonSuccess->lAddupLoseScore;
	CopyMemory(&UserInfoPlus.UserControlItem, &pDBOLogonSuccess->UserControlItem, sizeof(UserInfoPlus.UserControlItem));

	SCORE lFreezeScore = 0L;
	m_FreezeScoreMap.Lookup(UserInfo.dwUserID,lFreezeScore);

	//������Ϣ
	UserInfoPlus.bRobotUser=bRobotUser;
	UserInfoPlus.bMobileUser=bMobileUser;	
	UserInfoPlus.lFreeScore=pDBOLogonSuccess->lFreeScore+lFreezeScore;
	if( CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule) )
		UserInfoPlus.lLockServiceScore = m_pGameServiceOption->lRevenue;
	UserInfoPlus.lFreezeScore = lFreezeScore;
	UserInfoPlus.dwInoutIndex=pDBOLogonSuccess->dwInoutIndex;
	UserInfoPlus.lRestrictScore=m_pGameServiceOption->lRestrictScore;

	//�����û�
	UserInfoPlus.dwTokenID=dwTokenID;
	UserInfoPlus.wBindIndex=wBindIndex;
	UserInfoPlus.dwClientIP=pTokenParameter->dwClientIP;
	
	//��������
	StringCchCopy(UserInfoPlus.szMachineID, LEN_MACHINE_ID, pDBOLogonSuccess->szMachineID);

	pIServerUserItem=m_ServerUserManager.ActiveUserItem(UserInfo,UserInfoPlus,pDBOLogonSuccess->szPassword);

	//�����ж�
	if (pIServerUserItem==NULL)
	{
		//�������
		ASSERT(FALSE);

		//�������		
		DBR_GR_LeaveGameService LeaveGameService;
		ZeroMemory(&LeaveGameService,sizeof(LeaveGameService));

		//�û���Ϣ
		LeaveGameService.dwUserID=pDBOLogonSuccess->dwUserID;
		if (pDBOLogonSuccess->lFreeScore>0L)
			LeaveGameService.lUnLockScore=pDBOLogonSuccess->lFreeScore;
		else
			LeaveGameService.lUnLockScore=0;

		//��ʤ����
		LeaveGameService.lWinStreak = pDBOLogonSuccess->lWinStreak;
		LeaveGameService.lMaxWinStreak = pDBOLogonSuccess->lMaxWinStreak;

		//������Ϣ
		LeaveGameService.cbLeaveReason=LEAVE_GAME_ACTIVE_FAILURE;
		LeaveGameService.dwInoutIndex = pDBOLogonSuccess->dwInoutIndex;

		//������Ϣ
		LeaveGameService.dwClientIP=pBindParameter->dwClientIP;
		StringCchCopy(LeaveGameService.szMachineID, LEN_MACHINE_ID, pDBOLogonSuccess->szMachineID);		

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_SERVICE, pDBOLogonSuccess->dwUserID,0L,0L,&LeaveGameService,sizeof(LeaveGameService));

		//�Ͽ��û�
		if (bRobotUser==true)
		{
			m_RobotUserManager.DeleteRobotUserItem(pBindParameter->dwContextID,false);
		}
		else 
		{
			//����ṹ
			CMD_CS_C_ShutdownSocket ShutdownSocket;
			ShutdownSocket.bForceClose = true;

			//��������
			SendDataToGate(pBindParameter->dwContextID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));
		}

		return true;
	}

	//�ͱ���Ϣ
	pIServerUserItem->SetResidualTimes(pDBOLogonSuccess->cbResidualTimes);
	pIServerUserItem->GetSubsidyInfo().lConsumeScore = pDBOLogonSuccess->lConsumeScore;

	//�����Ʒ
	if (pDBOLogonSuccess->wGoodsCount > 0)
	{
		pIServerUserItem->GetUserPack()->AppendGoods(pDBOLogonSuccess->GoodsHoldInfo, pDBOLogonSuccess->wGoodsCount);
	}

	//���û�
	pTokenParameter->pBindUserItem = pIServerUserItem;

	//��������
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		//���ñ���
		pIServerUserItem->SetMatchID(pDBOLogonSuccess->dwMatchID);
		pIServerUserItem->SetSignupCount(pDBOLogonSuccess->wSignupCount);
		pIServerUserItem->SetTicketCount(pDBOLogonSuccess->dwTicketCount);		

		//У��Ȩ��
		if (CUserRight::IsMatchUser(pIServerUserItem->GetUserRight()))
		{
			pIServerUserItem->SetUserMatchStatus(MUS_SIGNUP);			
			pIServerUserItem->SetMatchNo(pDBOLogonSuccess->lMatchNo);	
		}		
	}

	//��¼�¼�
	OnEventUserLogon(pIServerUserItem);	

	//��¼֪ͨ
	if (m_pIExtendServiceManager!=NULL) 
	{
		m_pIExtendServiceManager->OnEventUserLogon(pIServerUserItem);
	}

	//Լս����
	if (m_pGameServiceOption->wServerType&(GAME_GENRE_BATTLE | GAME_GENRE_RANKING) && pIServerUserItem->IsMobileUser()==true)
	{	
		//���´���
		if (pDBOLogonSuccess->bLookon == false)
		{
			//����ṹ
			CMD_GR_UserSitDown UserSitDown;
			ZeroMemory(&UserSitDown, sizeof(UserSitDown));

			//���ñ���
			UserSitDown.wChairID = pDBOLogonSuccess->wChairID;
			UserSitDown.wTableID = pDBOLogonSuccess->wTableID;
			StringCchCopy(UserSitDown.szPassword, CountArray(UserSitDown.szPassword), pDBOLogonSuccess->szTablePasswd);

			//���´���
			OnTCPNetworkSubUserSitDown(&UserSitDown, sizeof(UserSitDown), dwContextID, dwTokenID);
		}		

		//�Թ۴���
		if (pDBOLogonSuccess->bLookon == true)
		{
			//����ṹ
			CMD_GR_UserLookon UserLookon;
			ZeroMemory(&UserLookon, sizeof(UserLookon));

			//���ñ���
			UserLookon.wChairID = pDBOLogonSuccess->wChairID;
			UserLookon.wTableID = pDBOLogonSuccess->wTableID;
			StringCchCopy(UserLookon.szPassword, CountArray(UserLookon.szPassword), pDBOLogonSuccess->szTablePasswd);

			//�Թ۴���
			OnTCPNetworkSubUserLookon(&UserLookon, sizeof(UserLookon), dwContextID, dwTokenID);
		}
	}

	//�����û�
	if (m_bCollectUser==true)
	{
		//��������
		CMD_CS_C_EnterServer EnterServer;
		ZeroMemory(&EnterServer,sizeof(EnterServer));

		//���ñ���
		EnterServer.dwUserID=pIServerUserItem->GetUserID();
		EnterServer.dwGameID=pIServerUserItem->GetGameID();
		EnterServer.cbGender = pIServerUserItem->GetUserInfo()->cbGender;
		EnterServer.wFaceID = pIServerUserItem->GetUserInfo()->wFaceID;
		EnterServer.dwCustomID = pIServerUserItem->GetUserInfo()->dwCustomID;
		StringCchCopy(EnterServer.szNickName,CountArray(EnterServer.szNickName), pIServerUserItem->GetAccounts());

		//������Ϣ
		ASSERT(m_pITCPSocketCenter!=NULL);
		m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_ENTER_SERVER,&EnterServer,sizeof(EnterServer),0);		
	}

	//��������
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0)
	{
		//��������
		CMD_CS_C_ServiceOnline ServiceOnLine;
		ZeroMemory(&ServiceOnLine, sizeof(ServiceOnLine));

		//���ñ���
		ServiceOnLine.wOnlineCount = m_ServerUserManager.GetUserItemCount();

		//������Ϣ
		ASSERT(m_pITCPSocketCenter != NULL);
		m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_SERVICE_ONLINE, &ServiceOnLine, sizeof(ServiceOnLine), 0);
	}

	if (pDBOLogonSuccess->szDescribeString[0]!='\0')
	{
		//��������
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		StringCchCopy(SystemMessage.szString,CountArray(SystemMessage.szString), pDBOLogonSuccess->szDescribeString);

		//��������
		SystemMessage.wLength=lstrlen(SystemMessage.szString)+1;
		SystemMessage.wType=SMT_EJECT|SMT_CHAT;

		//��������
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//��������
		if (bRobotUser==true)
		{
			//�����û�
			m_RobotUserManager.SendDataToClient(pBindParameter->dwContextID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}
		else
		{
			//�����û�
			m_pITCPNetworkEngine->SendData(pBindParameter->dwContextID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize,&dwTokenID,1);
		}
	}

	return true;
}

//�û��Ƹ�
bool CAttemperEngineSink::OnDBWealthUpdate(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT (sizeof(DBO_GR_UserWealthUpdate)==wDataSize);
	if (sizeof(DBO_GR_UserWealthUpdate) != wDataSize) return false;

	//��ȡ����
	DBO_GR_UserWealthUpdate* pWealthUpdate = (DBO_GR_UserWealthUpdate*)pData;

	//�����û�
	IServerUserItem * pIServerUserItem = m_ServerUserManager.SearchUserItem(pWealthUpdate->dwUserID);
	if (pIServerUserItem == NULL) return true;	

	//���²Ƹ�
	auto pUserInfo = pIServerUserItem->GetUserInfo();

	//ͬ���ڴ�
	if (pWealthUpdate->lChangeAmount != 0)
	{
		pIServerUserItem->WriteUserWealth(pWealthUpdate->wMoneyID, pWealthUpdate->lChangeAmount, GOODS_REASON_SYNC);
	}	

	return true;
}

//д�ֽ��
bool CAttemperEngineSink::OnDBWriteScoreResult(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	BYTE cbReApply=*(BYTE*)pData;
	if (cbReApply==1)
	{
		WORD wBindIndex=LOWORD(dwContextID);
		IServerUserItem * pIServerUserItem = GetServerUserItem(wBindIndex,dwTokenID);
		if(pIServerUserItem!=NULL)
		{
			//�޸�Ȩ��
			pIServerUserItem->ModifyUserRight(0,UR_MATCH_USER);

			//����֪ͨ
			CMD_GR_ConfigUserRight cur = {0};
			cur.dwUserRight=pIServerUserItem->GetUserRight();

			SendDataToUser(pIServerUserItem,MDM_GR_CONFIG,SUB_GR_CONFIG_USER_RIGHT,&cur,sizeof(cur) );
		}
	}
	
	return true;
}

//�ͱ����
bool CAttemperEngineSink::OnDBLargessSubsidy(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT (wDataSize<=sizeof(DBO_GR_LargessSubsidy));
	if (wDataSize > sizeof(DBO_GR_LargessSubsidy)) return true;

	//��ȡ����
	DBO_GR_LargessSubsidy * pLargessSubsidy = (DBO_GR_LargessSubsidy *)pData;

	//��ȡ�û�
	IServerUserItem * pIServerUserItem = GetServerUserItem(LOWORD(dwContextID), dwTokenID);
	if (pIServerUserItem == NULL)
	{
		pIServerUserItem = m_ServerUserManager.SearchUserItem(pLargessSubsidy->dwUserID);
		if (pIServerUserItem == NULL) return true;
	}

	//���ñ�ʶ
	pIServerUserItem->SetLargessSubsidy(false);	

	//ִ�гɹ�
	if (pLargessSubsidy->bResultCode==true)
	{		
		//��ȡ��Ϣ
		auto pUserInfo=pIServerUserItem->GetUserInfo();

		//���½��
		pUserInfo->lGold = pLargessSubsidy->lCurrGold;

		//���·���
		if (m_pGameServiceOption->wServerType & (GAME_GENRE_GOLD | GAME_GENRE_BATTLE))
		{
			pUserInfo->lScore = pLargessSubsidy->lCurrGold;
		}

		//ת������
		CServerUserItem * pServerUserItem=(CServerUserItem *)pIServerUserItem;

		//��������
		if (m_pGameServiceOption->lRestrictScore==0)
		{
			pServerUserItem->SetFreeScore(pLargessSubsidy->lCurrGold);
			pServerUserItem->SetLockScore(pLargessSubsidy->lCurrGold);
		}
		else
		{
			pServerUserItem->SetFreeScore(m_pGameServiceOption->lRestrictScore);
			pServerUserItem->SetLockScore(pLargessSubsidy->lCurrGold-m_pGameServiceOption->lRestrictScore);
		}

		//���ͷ���
		SendUserScore(pIServerUserItem);

		//�������
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			//��������
			CTableFrameItem * pTableFrame = m_TableFrameArray[pIServerUserItem->GetTableID()];
			if (pTableFrame!=NULL) pTableFrame->OnEventUserScoreChange(pIServerUserItem);
		}

		//���´���
		pIServerUserItem->SetResidualTimes(pLargessSubsidy->cbResidualTimes);

		//������ʾ
		LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0034, TEXT("����ǰ��Ϸ�ҵ���" SCORE_STRING "��ϵͳ���Զ�������" SCORE_STRING "��������ĵͷ����ʹ������� % d �Σ�"), SCORE_ZOOMOUT(m_SystemParameter.lTakeSubsidyCondition), SCORE_ZOOMOUT(pLargessSubsidy->lCurrGold), pLargessSubsidy->cbResidualTimes);
		
		//������Ϣ
		SendGameMessage(pIServerUserItem, pszMessage,SMT_CHAT|SMT_EJECT|SMT_SHOW_MOBILE);
	}
	else
	{
		//������Ϣ
		//SendGameMessage(pIServerUserItem,pLargessSubsidy->szDescribeString,SMT_CHAT|SMT_EJECT|SMT_SHOW_MOBILE);
	}

	//�������
	if (pIServerUserItem->GetTableID()>=m_TableFrameArray.GetCount()) return true;

	//��ȡ����
	CTableFrameItem * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];

	//�߳����
	SCORE lServiceFee=0;
	SCORE lLessScore=m_pGameServiceOption->lLessScore;		
	if (m_pGameServiceOption->lLessScore!=0 && CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule))
	{
		lServiceFee=m_pGameServiceOption->lRevenue;
	}

	//��ȡ����
	SCORE lBaseScore=pTableFrame->GetCellScore();	
	SCORE lStandupScore = __max(lBaseScore * pTableFrame->m_wStandupMultiple, lLessScore) + lServiceFee;

	//��������
	if (pIServerUserItem->GetUserScore() < lStandupScore)
	{
		//������ʾ
		LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0035, TEXT("�װ���%s,������Ϸ������" SCORE_STRING "�������ܼ�����Ϸ��"), pIServerUserItem->GetAccounts(), SCORE_ZOOMOUT(lStandupScore));

		//������Ϣ
		SendGameMessage(pIServerUserItem, pszMessage, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT | SMT_SHOW_MOBILE);

		//�û�����
		pTableFrame->PerformStandUpAction(pIServerUserItem);

		return true;
	}

	//��������
	if ((m_pGameServiceOption->lLessLockScore>0L)&&(pIServerUserItem->GetUserFreeScore()< lStandupScore-lServiceFee))
	{
		//������ʾ
		LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0035, TEXT("�װ���%s,������Ϸ������" SCORE_STRING "�������ܼ�����Ϸ��"), pIServerUserItem->GetAccounts(), SCORE_ZOOMOUT(lStandupScore));

		//����Ϣ
		SendGameMessage(pIServerUserItem, pszMessage, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT | SMT_SHOW_MOBILE);

		//�û�����
		pTableFrame->PerformStandUpAction(pIServerUserItem);

		return true;
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBUserControlInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//У�����
	ASSERT(wDataSize == sizeof(DBO_GR_UserControlInfo));
	if (wDataSize != sizeof(DBO_GR_UserControlInfo)) return true;

	//��ȡ����
	auto pUserControlInfo = (DBO_GR_UserControlInfo*)pData;

	//�����û�
	auto pIServerUserItem = m_ServerUserManager.SearchUserItem(pUserControlInfo->dwUserID);
	if (pIServerUserItem != NULL)
	{
		auto pUserControlItem = pIServerUserItem->GetUserControlItem();
		CopyMemory(pUserControlItem, &pUserControlInfo->UserControlItem,sizeof(tagUserControlItem));
	}

	return true;
}

//���Ƹ���
bool CAttemperEngineSink::OnDBUserControlUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//У�����
	ASSERT(wDataSize == sizeof(DBO_GR_UserControlUpdate));
	if (wDataSize != sizeof(DBO_GR_UserControlUpdate)) return true;

	//��ȡ����
	auto pUserControlUpdate = (DBO_GR_UserControlUpdate*)pData;

	//��ȡ�û�
	IServerUserItem* pIServerUserItem = GetServerUserItem(LOWORD(dwContextID), dwTokenID);
	if (pIServerUserItem != NULL)
	{
		auto pUserControlItem = pIServerUserItem->GetUserControlItem();
		if (pUserControlItem->cbControlStatus != pUserControlUpdate->cbControlStatus && pUserControlUpdate->cbControlStatus == CONTROL_STATUS_NORMAL)
		{
			//��������
			ZeroMemory(pUserControlItem,sizeof(tagUserControlItem));
		}		
	}

	return true;
}

//ע���¼�
bool CAttemperEngineSink::OnTCPSocketMainStoreRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_SUCCESS:		//ע��ɹ�
		{
			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_STORE_RESULT,&ControlResult,sizeof(ControlResult));

			//���ñ�ʶ
			m_bCacheVideo = false;

			//������Ϣ
			tagBurthenInfo BurthenInfo;
			m_VideoDataQueue.GetBurthenInfo(BurthenInfo);			

			//��������
			if (BurthenInfo.dwDataPacketCount > 0)
			{
				//��������
				tagDataHead DataHead;
				BYTE cbBuffer[SOCKET_PACKET];

				do
				{
					//��ȡ����
					if (m_VideoDataQueue.DistillData(DataHead, cbBuffer, CountArray(cbBuffer), true) == false)
					{
						break;
					}

					//�洢����
					//CString s;
					//s.Format(_T("�洢����2 "));
					//CTraceService::TraceString(s, TraceLevel_Debug);
					StoreVideoData(cbBuffer, DataHead.wDataSize);

				} while (true);
			}

			return true;
		}
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
		{
			//Ч�����
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//�رմ���
			m_bNeedStore=false;
			m_pITCPSocketStore->CloseSocket();

			//��ʾ��Ϣ
			if (lstrlen(pRegisterFailure->szDescribeString) > 0)
			{
				CTraceService::TraceString(pRegisterFailure->szDescribeString, TraceLevel_Exception);
			}

			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_STORE_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return false;
}

//ע���¼�
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
		{
			//Ч�����
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//�رմ���
			m_bNeekCenter=false;
			m_pITCPSocketCenter->CloseSocket();

			//��ʾ��Ϣ
			if (lstrlen(pRegisterFailure->szDescribeString) > 0)
			{
				CTraceService::TraceString(pRegisterFailure->szDescribeString, TraceLevel_Exception);
			}

			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_FAILURE;
			SendUIControlPacket(UI_CENTER_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	case SUB_CS_S_REGISTER_SUCCESS:		//ע��ɹ�
		{		
			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CENTER_RESULT,&ControlResult,sizeof(ControlResult));

			//������־
			g_TimeLoggerService->InitFormat(TEXT("GameLogger"), TEXT("%Y%m%d%H"), new CLogByMinute(TEXT("%Y%m%d%H%M"), 30));				

			return true;
		}
	}

	return true;
}

//ע���¼�
bool CAttemperEngineSink::OnTCPSocketMainServiceRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
		{
			//Ч�����
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;
			ASSERT (wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//�رմ���
			m_bNeedService=false;
			m_pITCPSocketService->CloseSocket();

			//��ʾ��Ϣ
			if (lstrlen(pRegisterFailure->szDescribeString) > 0)
			{
				CTraceService::TraceString(pRegisterFailure->szDescribeString, TraceLevel_Exception);
			}

			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_FAILURE;
			SendUIControlPacket(UI_SERVICE_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	case SUB_CS_S_REGISTER_SUCCESS:		//ע��ɹ�
		{
			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_SERVICE_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//�б��¼�
bool CAttemperEngineSink::OnTCPSocketMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SERVICE_INSERT:		//�������
		{
			//Ч�����
			ASSERT (wDataSize==sizeof(tagServiceItem));
			if (wDataSize!=sizeof(tagServiceItem)) return false;

			//��ȡ����
			tagServiceItem * pServiceItem =(tagServiceItem *)(pData);

			//�洢����
			if (pServiceItem->wServiceModule == SERVICE_MODULE_STORE)
			{
				if ((m_ServiceItem.wServiceID == 0 || m_ServiceItem.wServiceID == MAX_SERVICE_ID) &&
					(pServiceItem->wServiceID == MAX_SERVICE_ID || pServiceItem->wServiceID == m_pGameServiceAttrib->wKindID))
				{
					CopyMemory(&m_StoreServer, pServiceItem, sizeof(tagServiceItem));
				}
			}

			//��������
			if (pServiceItem->wServiceModule == SERVICE_MODULE_MATCH)
			{
				CopyMemory(&m_ServiceItem, pServiceItem, sizeof(tagServiceItem));				
			}

			//Լս����
			if (pServiceItem->wServiceModule == SERVICE_MODULE_BATTLE)
			{
				CopyMemory(&m_ServiceItem, pServiceItem, sizeof(tagServiceItem));		
			}

			//��λ����
			if (pServiceItem->wServiceModule == SERVICE_MODULE_RANKING)
			{
				CopyMemory(&m_ServiceItem, pServiceItem, sizeof(tagServiceItem));
			}

			//��־����
			if (pServiceItem->wServiceModule == SERVICE_MODULE_LOGGER)
			{
				//��־����
				g_TimeLoggerService->SetLoggerServer(pServiceItem->szServiceDomain, pServiceItem->wServicePort);
			}		

			return true;			
		}	
	}

	return true;
}


//������Ϣ
bool CAttemperEngineSink::OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_USER_SHUTUP_STATUS:	//����״̬
	{
		//У������
		ASSERT(wDataSize == sizeof(CMD_CS_UserShutUpStatus));
		if (wDataSize != sizeof(CMD_CS_UserShutUpStatus)) return true;

		//��ȡ����
		auto pUserShutUpStatus = (CMD_CS_UserShutUpStatus*)pData;

		//�����û�
		auto pIServerUserItem = m_ServerUserManager.SearchUserItem(pUserShutUpStatus->dwUserID);
		if (pIServerUserItem != NULL)
		{
			pIServerUserItem->SetShutUpStatus(pUserShutUpStatus->cbShutUpStatus);
			pIServerUserItem->SetShutUpOverTime(pUserShutUpStatus->dwShutUpOverTime);
		}

		return true;
	}
	case SUB_CS_USER_CONTROL_APPEND:	//��ӿ���
	{
		//У������
		ASSERT(wDataSize == sizeof(CMD_CS_UserControlAppend));
		if (wDataSize != sizeof(CMD_CS_UserControlAppend)) return true;

		//��ȡ����
		auto pUserControlAppend = (CMD_CS_UserControlAppend*)pData;

		//��ѯ�û�
		auto pIServerUserItem = m_ServerUserManager.SearchUserItem(pUserControlAppend->dwUserID);
		if (pIServerUserItem != NULL)
		{
			//����ṹ
			DBR_GR_ControlQuery ControlQuery;
			ZeroMemory(&ControlQuery,sizeof(ControlQuery));

			//���ñ�ʶ
			ControlQuery.dwUserID = pUserControlAppend->dwUserID;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_CONTROL_QUERY, ControlQuery.dwUserID, 0L, 0L, &ControlQuery, sizeof(ControlQuery));
		}

		return true;
	}
	case SUB_CS_USER_CONTROL_REMOVE:	//�Ƴ�����
	{
		//У������
		ASSERT(wDataSize == sizeof(CMD_CS_UserControlRemove));
		if (wDataSize != sizeof(CMD_CS_UserControlRemove)) return true;

		//��ȡ����
		auto pUserControlRemove = (CMD_CS_UserControlRemove*)pData;

		//��ѯ�û�
		auto pIServerUserItem = m_ServerUserManager.SearchUserItem(pUserControlRemove->dwUserID);
		if (pIServerUserItem != NULL)
		{
			//���ƽ���
			OnEventUserControlFinish(pIServerUserItem);

			//�������
			auto pUserControlItem = pIServerUserItem->GetUserControlItem();
			ZeroMemory(pUserControlItem,sizeof(tagUserControlItem));
		}

		return true;
	}
	case SUB_CS_USER_RECHARGE_NOTICE:	//��ֵ֪ͨ
	{
		//У������
		ASSERT(wDataSize <= sizeof(CMD_CS_UserRechargeNotice));
		if (wDataSize > sizeof(CMD_CS_UserRechargeNotice)) return true;

		//��ȡ����
		auto pUserRechargeNotice = (CMD_CS_UserRechargeNotice*)pData;

		//�����û�
		auto pIServerUserItem = m_ServerUserManager.SearchUserItem(pUserRechargeNotice->dwUserID);
		if (pIServerUserItem == NULL) return true;

		//��������
		CGoodsItemArray MoneyArray, GoodsArray;
		if (g_GameGoodsManager->ParseFromJson(pUserRechargeNotice->szGoodsList, MoneyArray, GoodsArray))
		{
			if (MoneyArray.GetCount()==1 && MoneyArray[0].wGoodsID == GOODS_ID_GOLD && MoneyArray[0].dwGoodsCount>0)
			{
				//����ṹ
				CMD_GR_RechargeNotice RechargeNotice;
				ZeroMemory(&RechargeNotice,sizeof(RechargeNotice));

				//���ñ���
				RechargeNotice.dwUserID = pUserRechargeNotice->dwUserID;
				RechargeNotice.lPresentScore = MoneyArray[0].dwGoodsCount;

				//������Ϣ
				SendDataToUser(pIServerUserItem, MDM_GR_USER, SUB_GR_RECHARGE_NOTICE, &RechargeNotice, sizeof(RechargeNotice));

				//���ӻ���
				pIServerUserItem->WriteUserWealth(MoneyArray[0].wGoodsID, RechargeNotice.lPresentScore, GOODS_REASON_SYNC);
			}
		}

		return true;
	}
	case SUB_CS_APPEND_ROBOT_PARAMETER:		//��Ӳ���
	{
		//����У��
		ASSERT(sizeof(CMD_CS_AppendRobotParameter) == wDataSize);
		if (sizeof(CMD_CS_AppendRobotParameter) != wDataSize) return false;

		//��ȡ����
		auto pAddParameter = (CMD_CS_AppendRobotParameter*)pData;

		//��Ӳ���
		m_RobotUserManager.AddRobotParameter(&pAddParameter->RobotParameter, 1);

		return true;
	}
	case SUB_CS_MODIFY_ROBOT_PARAMETER:		//�޸Ĳ���
	{
		//����У��
		ASSERT(sizeof(CMD_CS_ModifyRobotParameter) == wDataSize);
		if (sizeof(CMD_CS_ModifyRobotParameter) != wDataSize) return false;

		//��ȡ����
		auto pModifyParameter = (CMD_CS_ModifyRobotParameter*)pData;

		//�޸Ĳ���
		m_RobotUserManager.AddRobotParameter(&pModifyParameter->RobotParameter, 1);

		return true;
	}
	case SUB_CS_DELETE_ROBOT_PARAMETER:		//ɾ������
	{
		//����У��
		ASSERT(sizeof(CMD_CS_DeleteRobotParameter) == wDataSize);
		if (sizeof(CMD_CS_DeleteRobotParameter) != wDataSize) return false;

		//��ȡ����
		auto pDeleteParameter = (CMD_CS_DeleteRobotParameter*)pData;

		//ɾ������
		m_RobotUserManager.RemoveRobotParameter(pDeleteParameter->dwBatchID);

		return true;
	}
	case SUB_CS_LOGOUT_NOTIFY:	//�ǳ�֪ͨ
	{
		ASSERT(wDataSize == sizeof(CMD_CS_LogoutNotify));
		CMD_CS_LogoutNotify* pLogoout = (CMD_CS_LogoutNotify*)pData;
		//�����û�
		auto pIServerUserItem = m_ServerUserManager.SearchUserItem(pLogoout->dwUserID);
		if (pIServerUserItem == NULL) return true;

		WORD wTableID = pIServerUserItem->GetTableID();
		//�뿪����
		if (wTableID != INVALID_TABLE)
		{
			CTableFrameItem* pTableFrame = m_TableFrameArray[wTableID];
			if (pTableFrame->PerformStandUpAction(pIServerUserItem, true) == false) return true;
		}

		return true;
	}
	}


	return true;
}

//�����¼�
bool CAttemperEngineSink::OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_COLLECT_USER:			//�û�����
		{
			//��������
			CMD_CS_C_EnterServer EnterServer;
			ZeroMemory(&EnterServer,sizeof(EnterServer));

			//�����û�
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem = NULL;

			do
			{
				//��ȡ�û�
				pIServerUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
				if (pIServerUserItem==NULL) break;

				//���ñ���
				EnterServer.dwUserID = pIServerUserItem->GetUserID();
				EnterServer.dwGameID = pIServerUserItem->GetGameID();
				EnterServer.cbGender = pIServerUserItem->GetUserInfo()->cbGender;
				EnterServer.wFaceID = pIServerUserItem->GetUserInfo()->wFaceID;
				EnterServer.dwCustomID = pIServerUserItem->GetUserInfo()->dwCustomID;
				StringCchCopy(EnterServer.szNickName, CountArray(EnterServer.szNickName), pIServerUserItem->GetAccounts());

				//��������
				ASSERT(m_pITCPSocketCenter!=NULL);
				m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_ENTER_SERVER,&EnterServer,sizeof(EnterServer),0);

			} while (true);

			//�㱨���
			m_bCollectUser=true;
			m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_COLLECT_FINISH,0);

			return true;
		}
	}

	return true;
}

//��ѯ�¼�
bool CAttemperEngineSink::OnTCPSocketMainQueryService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch(wSubCmdID)
	{
	case SUB_CS_S_QUERY_USER_RESULT:		//��ѯ���
		{
			//��������
			CMD_CS_S_QueryUserResult *pQueryResult = (CMD_CS_S_QueryUserResult *)pData;
			WORD wRecBaseSize = sizeof(CMD_CS_S_QueryUserResult)-sizeof(pQueryResult->szAccounts)+CountStringBuffer(pQueryResult->szAccounts);

			//Ч��
			ASSERT( wDataSize >= sizeof(CMD_CS_S_QueryUserResult)-sizeof(pQueryResult->szAccounts)+
				CountStringBuffer(pQueryResult->szAccounts) );
			if( wDataSize < sizeof(CMD_CS_S_QueryUserResult)-sizeof(pQueryResult->szAccounts)+
				CountStringBuffer(pQueryResult->szAccounts) )
				return false;

			//Ŀ�����
			IServerUserItem *pIServerUserItem = m_ServerUserManager.SearchUserItem(pQueryResult->dwSendUserID);
			if( !pIServerUserItem ) return true;

			//������
			BYTE cbBuffer[SOCKET_PACKET] = {0};
			tagGlobalUserInfo *pGlobalUserInfo = (tagGlobalUserInfo *)cbBuffer;
			pGlobalUserInfo->dwGameID = pQueryResult->dwGameID;
			pGlobalUserInfo->dwUserID = pQueryResult->dwUserID;
			StringCchCopy( pGlobalUserInfo->szNickName,CountArray(pGlobalUserInfo->szNickName), pQueryResult->szAccounts);

			//Я����Ϣ
			WORD wBaseSize = sizeof(tagGlobalUserInfo)-sizeof(pGlobalUserInfo->szNickName)+CountStringBuffer(pGlobalUserInfo->szNickName);
			CopyMemory( cbBuffer+wBaseSize,(BYTE *)pData+wRecBaseSize,wDataSize-wRecBaseSize );

			//������Ϣ
			SendDataToUser(pIServerUserItem,MDM_GR_QUERY,SUB_GR_QUERY_USER_RESULT,cbBuffer,wBaseSize+wDataSize-wRecBaseSize);

			return true;
		}
	case SUB_CS_S_QUERY_NOT_FOUND:			//��ѯ����
		{
			//Ч��
			ASSERT( wDataSize == sizeof(CMD_CS_S_QueryNotFound) );
			if( wDataSize != sizeof(CMD_CS_S_QueryNotFound) )
				return false;

			//��������
			CMD_CS_S_QueryNotFound *pNotFound = (CMD_CS_S_QueryNotFound *)pData;

			//Ŀ�����
			IServerUserItem *pIServerUserItem = m_ServerUserManager.SearchUserItem(pNotFound->dwSendUserID);
			if( !pIServerUserItem ) return true;

			//������Ϣ
			CMD_GR_QueryNotFound NotFound = {0};
			NotFound.nResultCode = QUERY_NOT_FOUND;
			
			//������Ϣ
			WORD wSendSize = sizeof(NotFound)-sizeof(NotFound.szDescribeString)+CountStringBuffer(NotFound.szDescribeString);
			SendDataToUser(pIServerUserItem,MDM_GR_QUERY,SUB_GR_QUERY_NOT_FOUND,&NotFound,wSendSize);

			return true;
		}
	}

	return true;
}

//�û���¼
VOID CAttemperEngineSink::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	//��ȡ����
	WORD wBindIndex=pIServerUserItem->GetBindIndex();
	bool bRobotUser=pIServerUserItem->IsRobotUser();
	tagBindParameter * pBindParameter=GetBindParameter(pIServerUserItem->GetBindIndex());

	//��¼����
	if (pIServerUserItem->IsMobileUser()==false) 
	{
		//��¼�ɹ�
		CMD_GR_LogonSuccess LogonSuccess;
		LogonSuccess.dwUserID=pIServerUserItem->GetUserID();
		LogonSuccess.dwUserRight=pIServerUserItem->GetUserRight();
		LogonSuccess.dwMasterRight=pIServerUserItem->GetMasterRight();
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_LOGON,SUB_GR_LOGON_SUCCESS,&LogonSuccess,sizeof(LogonSuccess));

		//�б�����
		const WORD wColumnInfoHead=sizeof(CMD_ConfigColumn)-sizeof(CMD_ConfigColumn.ColumnItem);
		WORD wColumnInfoSize=wColumnInfoHead+CMD_ConfigColumn.cbColumnCount*sizeof(CMD_ConfigColumn.ColumnItem[0]);
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_CONFIG,SUB_GR_CONFIG_COLUMN,&CMD_ConfigColumn,wColumnInfoSize);

		//��������
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_CONFIG,SUB_GR_CONFIG_SERVER,&CMD_ConfigServer,sizeof(CMD_ConfigServer));
		
		//�������
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_CONFIG,SUB_GR_CONFIG_FINISH,NULL,0);

		//�������
		SendUserEnterPacket(pIServerUserItem,pBindParameter->dwContextID, pIServerUserItem->GetTokenID(),false,true);

		//�����û�
		WORD wEnumIndex=0;
		IServerUserItem * pIServerUserItemSend=NULL;
		do
		{
			pIServerUserItemSend=m_ServerUserManager.EnumUserItem(wEnumIndex++);
			if (pIServerUserItemSend==NULL) break;
			if (pIServerUserItemSend!=pIServerUserItem)
			{
				SendUserEnterPacket(pIServerUserItemSend,pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),false);
			}
		} while (true);

		//����״̬
		CMD_GR_TableInfo TableInfo;
		TableInfo.wTableCount=(WORD)m_TableFrameArray.GetCount();
		ASSERT(TableInfo.wTableCount<CountArray(TableInfo.TableStatusArray));
		for (WORD i=0;i<TableInfo.wTableCount;i++)
		{
			CTableFrameItem * pTableFrame=m_TableFrameArray[i];
			TableInfo.TableStatusArray[i].cbTableLock=pTableFrame->IsTableLocked()?TRUE:FALSE;
			TableInfo.TableStatusArray[i].cbPlayStatus=pTableFrame->IsTableStarted()?TRUE:FALSE;
		}

		//����״̬
		WORD wHeadSize=sizeof(TableInfo)-sizeof(TableInfo.TableStatusArray);
		WORD wSendSize=wHeadSize+TableInfo.wTableCount*sizeof(TableInfo.TableStatusArray[0]);
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_STATUS,SUB_GR_TABLE_INFO,&TableInfo,wSendSize);

		//��ʾ�׷�
		if (CServerRule::IsForbidShowBaseScore(m_pGameServiceOption->dwServerRule)==false)
		{
			//���ӵ׷�
			CMD_GR_TableScore TableScore;
			TableScore.wTableCount=(WORD)m_TableFrameArray.GetCount();
			ASSERT(TableScore.wTableCount<CountArray(TableScore.lTableScoreArray));
			for (WORD i=0;i<TableScore.wTableCount;i++)
			{
				CTableFrameItem * pTableFrame=m_TableFrameArray[i];
				TableScore.lTableScoreArray[i]=pTableFrame->GetCellScore();
			}

			//���ӵ׷�
			wHeadSize=sizeof(TableScore)-sizeof(TableScore.lTableScoreArray);
			wSendSize=wHeadSize+TableScore.wTableCount*sizeof(TableScore.lTableScoreArray[0]);
			SendDataToGate(pBindParameter->dwContextID, pIServerUserItem->GetTokenID(),MDM_GR_STATUS,SUB_GR_TABLE_SCORE,&TableScore,wSendSize);
		}	

		//��¼���
		SendUserEnterPacket(pIServerUserItem,INVALID_DWORD,0,true);
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_LOGON,SUB_GR_LOGON_FINISH,NULL,0);
	}
	else
	{
		//��������
		CMD_GR_ConfigServer ConfigServer;
		ZeroMemory(&ConfigServer,sizeof(ConfigServer));

		//��������
		ConfigServer.wTableCount=m_pGameServiceOption->wTableCount;
		ConfigServer.wChairCount=m_pGameServiceAttrib->wChairCount;
		ConfigServer.wServerType=m_pGameServiceOption->wServerType;
		ConfigServer.dwServerRule=m_pGameServiceOption->dwServerRule;
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_CONFIG,SUB_GR_CONFIG_SERVER,&ConfigServer,sizeof(ConfigServer));

		//�������
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_CONFIG,SUB_GR_CONFIG_FINISH,NULL,0);

		//�û�����
		SendUserEnterPacket(pIServerUserItem,pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),true);

		//�������
		SendTableUserToUser(pIServerUserItem,CLIENT_KIND_MOBILE,INVALID_DWORD);

		//Ⱥ���û�
		SendUserEnterPacket(pIServerUserItem,INVALID_DWORD,0,true);

		//��¼���
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_LOGON,SUB_GR_LOGON_FINISH,NULL,0);

		//�Թ��û�
		if (pIServerUserItem->GetUserStatus()==US_LOOKON)
		{
			CTraceService::TraceString(TEXT("�û�������OnEventUserLogon"), TraceLevel_Info);
			m_TableFrameArray[pIServerUserItem->GetTableID()]->PerformStandUpAction(pIServerUserItem);
		}

		//����״̬
		CMD_GR_TableInfo TableInfo;
		TableInfo.wTableCount=(WORD)m_TableFrameArray.GetCount();
		ASSERT(TableInfo.wTableCount<CountArray(TableInfo.TableStatusArray));
		for (WORD i=0;i<TableInfo.wTableCount;i++)
		{
			CTableFrameItem * pTableFrame=m_TableFrameArray[i];
			TableInfo.TableStatusArray[i].cbTableLock=pTableFrame->IsTableLocked()?TRUE:FALSE;
			TableInfo.TableStatusArray[i].cbPlayStatus=pTableFrame->IsTableStarted()?TRUE:FALSE;			
		}

		//����״̬
		WORD wHeadSize=sizeof(TableInfo)-sizeof(TableInfo.TableStatusArray);
		WORD wSendSize=wHeadSize+TableInfo.wTableCount*sizeof(TableInfo.TableStatusArray[0]);
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_STATUS,SUB_GR_TABLE_INFO,&TableInfo,wSendSize);
		
		//���´���
		if (pIServerUserItem->GetTableID()==INVALID_TABLE)
		{
			//��ҷ�����������
			if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0) MobileUserImmediately(pIServerUserItem);					}	
	}

	return;
}

//�û��ǳ�
VOID CAttemperEngineSink::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	//У�����
	ASSERT (pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return;

	//���¿���
	if (pIServerUserItem->GetUserControlItem()->cbControlStatus != CONTROL_STATUS_NORMAL && pIServerUserItem->GetUserControlItem()->cbControlKind != CONTROL_KIND_CUSTOM)
	{
		OnEventUserControlUpdate(pIServerUserItem);		
	}

	//��ȡ����
	auto pServerUserPack = pIServerUserItem->GetUserPack();
	
	//������Ʒ
	if (pServerUserPack->GetVariationCount() > 0)
	{
		//�������
		DBR_GR_UserUpdateGoods UserUpdateGoods;
		ZeroMemory(&UserUpdateGoods, sizeof(UserUpdateGoods));

		//�û���ʶ
		UserUpdateGoods.dwUserID = pIServerUserItem->GetUserID();

		//��ȡ��Ʒ
		if (pServerUserPack->DistillVariationGoods(UserUpdateGoods.szGoodsList, CountArray(UserUpdateGoods.szGoodsList)))
		{
			//Ͷ������	
			WORD wDataSize = CountStringBuffer(UserUpdateGoods.szGoodsList);
			WORD wHeadSize = sizeof(UserUpdateGoods) - sizeof(UserUpdateGoods.szGoodsList);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_UPDATE_GOODS, UserUpdateGoods.dwUserID, pIServerUserItem->GetBindIndex(), pIServerUserItem->GetTokenID(), &UserUpdateGoods, wHeadSize + wDataSize);
		}

		//�������
		DBR_GR_UserRecordGoods UserRecordGoods;
		ZeroMemory(&UserRecordGoods, sizeof(UserRecordGoods));

		//�û���ʶ
		UserRecordGoods.dwUserID = pIServerUserItem->GetUserID();

		//��ȡ��¼
		if (pServerUserPack->DistillVariationRecord(UserRecordGoods.szRecordList, CountArray(UserRecordGoods.szRecordList)))
		{
			//Ͷ������	
			WORD wDataSize = CountStringBuffer(UserRecordGoods.szRecordList);
			WORD wHeadSize = sizeof(UserRecordGoods) - sizeof(UserRecordGoods.szRecordList);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_RECORD_GOODS, UserRecordGoods.dwUserID, pIServerUserItem->GetBindIndex(), pIServerUserItem->GetTokenID(), &UserRecordGoods, wHeadSize + wDataSize);
		}

		//������
		pServerUserPack->CleanVariation();
	}

	//��������
	DBR_GR_LeaveGameService LeaveGameService;
	ZeroMemory(&LeaveGameService,sizeof(LeaveGameService));

	//��ȡ����
	tagLockScoreInfo LockScoreInfo;
	tagVariationScore VariationScore;
	ZeroMemory(&LockScoreInfo,sizeof(LockScoreInfo));
	ZeroMemory(&VariationScore,sizeof(VariationScore));
	pIServerUserItem->DistillVariation(VariationScore,LockScoreInfo);
	pIServerUserItem->QueryRecordInfo(LeaveGameService.RecordInfo);

	//�û���Ϣ
	LeaveGameService.dwUserID=pIServerUserItem->GetUserID();
	LeaveGameService.dwClientIP=pIServerUserItem->GetClientIP();
	LeaveGameService.dwPlayTimeCount=pIServerUserItem->GetTotalPlayTimeCount();
	LeaveGameService.dwOnLineTimeCount=static_cast<DWORD>(time(NULL))-pIServerUserItem->GetLogonTime();

	//������Ϣ
	LeaveGameService.lRevenue=VariationScore.lRevenue;
	LeaveGameService.lUnLockScore=VariationScore.lUnLockScore;
	LeaveGameService.lScore=VariationScore.VariationScore.lScore;
	LeaveGameService.lWinCount=VariationScore.VariationScore.lWinCount;
	LeaveGameService.lLostCount=VariationScore.VariationScore.lLostCount;
	LeaveGameService.lDrawCount=VariationScore.VariationScore.lDrawCount;
	LeaveGameService.lFleeCount=VariationScore.VariationScore.lFleeCount;
	LeaveGameService.lExperience=VariationScore.VariationScore.lExperience;
	LeaveGameService.lWinStreak=VariationScore.VariationScore.lWinStreak;
	LeaveGameService.lMaxWinStreak=VariationScore.VariationScore.lMaxWinStreak;
	StringCchCopy(LeaveGameService.szMachineID, LEN_MACHINE_ID, pIServerUserItem->GetMachineID());
	LeaveGameService.dwInoutIndex=pIServerUserItem->GetInoutIndex();
	LeaveGameService.cbLeaveReason=LEAVE_GAME_NORMAL;	

	//��������
	if (pIServerUserItem->IsRobotUser() == true)
	{
		LeaveGameService.lScore = 0;
		LeaveGameService.lRevenue = 0;
	}

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_SERVICE, LeaveGameService.dwUserID,0L,0L,&LeaveGameService, sizeof(LeaveGameService));
	
	//�����û�
	if (m_bCollectUser==true)
	{
		CMD_CS_C_LeaveServer UserLeave;
		UserLeave.dwUserID=pIServerUserItem->GetUserID();
		m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_LEAVE_SERVER,&UserLeave,sizeof(UserLeave),0);
	}

	//�ǳ�֪ͨ
	if (m_pIExtendServiceManager!=NULL) 
	{
		m_pIExtendServiceManager->OnEventUserLogout(pIServerUserItem);
	}

	//�Ƴ�����
	RemoveDistribute(pIServerUserItem);

	//ɾ���û�
	m_ServerUserManager.DeleteUserItem(pIServerUserItem);	

	//��������
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0)
	{
		//��������
		CMD_CS_C_ServiceOnline ServiceOnLine;
		ZeroMemory(&ServiceOnLine, sizeof(ServiceOnLine));

		//���ñ���
		ServiceOnLine.wOnlineCount = m_ServerUserManager.GetUserItemCount();

		//������Ϣ
		ASSERT(m_pITCPSocketCenter != NULL);
		m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_SERVICE_ONLINE, &ServiceOnLine, sizeof(ServiceOnLine), 0);
	}

	return;
}

//�л�����
VOID CAttemperEngineSink::SwitchUserItemConnect(IServerUserItem * pIServerUserItem, WORD wTargetIndex, DWORD dwTokenID, bool bMobileUser)
{
	//Ч�����
	ASSERT((pIServerUserItem!=NULL)&&(wTargetIndex!=INVALID_WORD_INDEX));
	if ((pIServerUserItem==NULL)||(wTargetIndex==INVALID_WORD_INDEX)) return;

	//��������
	bool bRobotUser = (wTargetIndex >= INDEX_ROBOT);
	tagTokenParameter * pTokenParameter = GetTokenParameter(wTargetIndex, dwTokenID);
	if (pTokenParameter == NULL) return;

	//�Ͽ��û�
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD_INDEX && pIServerUserItem->GetTokenID()!=0)
	{
		//����֪ͨ
		SendRoomMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0038,TEXT("��ע�⣬�����ʺ�����һ�ط��������Ϸ�����ˣ��������뿪��")),SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM);
		
		//�Ͽ��û�
		if (pIServerUserItem->IsRobotUser()==true)
		{
			//��ȡ����
			tagBindParameter * pBindParameter = GetBindParameter(pIServerUserItem->GetBindIndex());
			m_RobotUserManager.DeleteRobotUserItem(pBindParameter->dwContextID,false);
		}
		else
		{
			//����ṹ
			CMD_CS_C_ShutdownSocket ShutdownSocket;
			ShutdownSocket.bForceClose = true;

			//��������
			SendDataToUser(pIServerUserItem, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));
		}

		//ɾ������
		auto pSourceParameter = GetBindParameter(pIServerUserItem->GetBindIndex());
		auto pGlobalGateItem = pSourceParameter->pGlobalGateItem;
		if (pGlobalGateItem != NULL)
		{
			pGlobalGateItem->RemoveTokenID(pIServerUserItem->GetTokenID());
		}
	}		

	//�����û�
	pIServerUserItem->SetMobileUser(bMobileUser);	
	pIServerUserItem->SetUserParameter(pTokenParameter->dwClientIP,wTargetIndex,dwTokenID,bRobotUser,false);

	//���û�
	pTokenParameter->pBindUserItem=pIServerUserItem;

	//״̬�л�
	if (pIServerUserItem->GetUserStatus()==US_OFFLINE)
	{
		WORD wTableID=pIServerUserItem->GetTableID();
		WORD wChairID=pIServerUserItem->GetChairID();
		ASSERT(wTableID!=INVALID_TABLE&&wChairID!=INVALID_CHAIR);

		CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame && pTableFrame->PerformConnectAction(pIServerUserItem))
		{
			pIServerUserItem->SetUserStatus(US_PLAYING,wTableID,wChairID);
		}
	}

	//��¼�¼�
	OnEventUserLogon(pIServerUserItem);

	//��¼֪ͨ
	if (m_pIExtendServiceManager!=NULL) 
	{
		m_pIExtendServiceManager->OnEventUserLogon(pIServerUserItem);
	}

	return;
}

//�汾���
bool CAttemperEngineSink::PerformCheckVersion(DWORD dwDeviceKind, DWORD dwPlazaVersion, DWORD dwClientVersion, DWORD dwSocketID, DWORD dwTokenID)
{
	//��ʶ����
	bool bMustUpdatePlaza=false;
	bool bMustUpdateClient=false,bAdviceUpdateClient=false;
	bool bSpecialPlazaVer=false,bSpecialClientVer=false;

	//�汾����	
	DWORD dwCurrentPlazaVersion=0;	
	DWORD dwCurrentClientVersion=m_pGameServiceAttrib->dwClientVersion;

	//��������
	tagModuleVersion * pModuleVersion=NULL;

	//����㳡�汾
	pModuleVersion=GetSpecialVersion(0,dwDeviceKind,dwPlazaVersion);
	if (pModuleVersion!=NULL) bSpecialPlazaVer=true;

	//������Ϸ�汾
	pModuleVersion=GetSpecialVersion(m_pGameServiceAttrib->wKindID,dwDeviceKind, dwPlazaVersion);
	if (pModuleVersion!=NULL) bSpecialClientVer=true;

	//��ǰ�㳡�汾
	pModuleVersion=GetCurrentVersion(0,dwDeviceKind);
	if (pModuleVersion!=NULL) dwCurrentPlazaVersion=pModuleVersion->dwVersion;

	//��ǰ��Ϸ�汾
	pModuleVersion=GetCurrentVersion(m_pGameServiceAttrib->wKindID,dwDeviceKind);
	if (pModuleVersion!=NULL) dwCurrentClientVersion=pModuleVersion->dwVersion;

	//�����汾
	if (!bSpecialPlazaVer)
	{
		//У��汾
		if (GetMainVer(dwPlazaVersion)!=GetMainVer(dwCurrentPlazaVersion)) bMustUpdatePlaza=true;
		if (GetProductVer(dwPlazaVersion)!=GetProductVer(dwCurrentPlazaVersion)) bMustUpdatePlaza=true;
	}

	//��Ϸ�汾
	if (!bSpecialClientVer)
	{
		//��Ϸ�汾
		if (GetSubVer(dwClientVersion)<GetSubVer(dwCurrentClientVersion)) bAdviceUpdateClient=true;
		if (GetMainVer(dwClientVersion)!=GetMainVer(dwCurrentClientVersion)) bMustUpdateClient=true;
		if (GetProductVer(dwClientVersion)!=GetProductVer(dwCurrentClientVersion)) bMustUpdateClient=true;
	}

	//����֪ͨ
	if ((bMustUpdatePlaza==true)||(bMustUpdateClient==true)||(bAdviceUpdateClient==true))
	{
		//��������
		CMD_GR_UpdateNotify UpdateNotify;
		ZeroMemory(&UpdateNotify,sizeof(UpdateNotify));

		//��������
		UpdateNotify.cbMustUpdatePlaza=bMustUpdatePlaza;
		UpdateNotify.cbMustUpdateClient=bMustUpdateClient;
		UpdateNotify.cbAdviceUpdateClient=bAdviceUpdateClient;

		//��ǰ�汾
		UpdateNotify.dwCurrentFrameVersion=VERSION_FRAME;
		UpdateNotify.dwCurrentPlazaVersion=dwCurrentPlazaVersion;		
		UpdateNotify.dwCurrentClientVersion=dwCurrentClientVersion;

		//������Ϣ
		SendDataToGate(dwSocketID,dwTokenID,MDM_GR_LOGON,SUB_GR_UPDATE_NOTIFY,&UpdateNotify,sizeof(UpdateNotify));

		//��ֹ�ж�
		if ((bMustUpdatePlaza==true)||(bMustUpdateClient==true))
		{
			ASSERT(LOWORD(dwSocketID)<INDEX_ROBOT);

			//����ṹ
			CMD_CS_C_ShutdownSocket ShutdownSocket;
			ShutdownSocket.bForceClose = false;

			//��������
			SendDataToGate(dwSocketID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));

			return false;
		}
	}

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ����
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem = GetServerUserItem(wBindIndex,dwTokenID);
	if (pIServerUserItem!=NULL) return true;

	//��������
	CMD_GR_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));

	//������Ϣ
	LogonFailure.lErrorCode=lErrorCode;	
	StringCchCopy(LogonFailure.szDescribeString,CountArray(LogonFailure.szDescribeString), pszString);

	//��������
	WORD wStringSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wSendSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString)+wStringSize;

	//��������
	SendDataToGate(dwSocketID,dwTokenID,MDM_GR_LOGON,SUB_GR_LOGON_FAILURE,&LogonFailure,wSendSize);

	return true;
}

//�����û�
bool CAttemperEngineSink::SendUserEnterPacket(IServerUserItem * pIServerUserItem, DWORD dwSocketID, DWORD dwTokenID, bool bAllLinks, bool bNotifySink)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	bool bHideUserInfo = false;

	//��ȡ�û�
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem* pITargetUserItem = GetServerUserItem(wBindIndex, dwTokenID);

	//������Ϣ
	if (bNotifySink == false && (pITargetUserItem == NULL || pITargetUserItem->GetMasterOrder() == 0))
	{
		//������Ϣ
		if (CServerRule::IsHideUserInfo(m_pGameServiceOption->dwServerRule) == true)
		{
			bHideUserInfo = true;			
		}
	}

	//���涨��
	BYTE cbMBBuffer[SOCKET_PACKET/2] = { 0 };
	BYTE cbPCBuffer[SOCKET_PACKET/2] = { 0 };

	//����ṹ
	WORD wMBDataSize = ConstructEnterPacket(pIServerUserItem, cbMBBuffer, sizeof(cbMBBuffer), CLIENT_KIND_MOBILE, bHideUserInfo);
	WORD wPCDataSize = ConstructEnterPacket(pIServerUserItem, cbPCBuffer, sizeof(cbPCBuffer), CLIENT_KIND_COMPUTER, bHideUserInfo);	

	//��������
	if (pITargetUserItem==NULL)
	{
		//��������
		SendBatchData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_ENTER, cbPCBuffer, wPCDataSize);

		//�����ֻ�
		SendDataToTable(pIServerUserItem->GetTableID(), CLIENT_KIND_MOBILE ,MDM_GR_USER, SUB_GR_USER_ENTER, cbMBBuffer, wMBDataSize);
	}
	else
	{
		if (pITargetUserItem->IsMobileUser()==false)
		{
			if (bAllLinks==true) 
			{
				SendBatchData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_ENTER, cbPCBuffer, wPCDataSize);
			}
			else 
			{
				SendDataToGate(dwSocketID,dwTokenID,MDM_GR_USER,SUB_GR_USER_ENTER, cbPCBuffer, wPCDataSize);
			}
		}
		else
		{
			if (dwSocketID == INVALID_DWORD)
			{
				SendDataToTable(pIServerUserItem->GetTableID(), CLIENT_KIND_MOBILE, MDM_GR_USER, SUB_GR_USER_ENTER, cbMBBuffer, wMBDataSize);
			}
			else
			{
				SendDataToGate(dwSocketID, dwTokenID, MDM_GR_USER, SUB_GR_USER_ENTER, cbMBBuffer, wMBDataSize);
			}		
		}
	}

	return true;
}

//����ʧ��
bool CAttemperEngineSink::SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, BYTE cbFailureCode)
{
	//��������
	CMD_GR_RequestFailure RequestFailure;
	RequestFailure.cbFailureCode=cbFailureCode;
	StringCchCopy(RequestFailure.szDescribeString,CountArray(RequestFailure.szDescribeString), pszDescribe);

	//��������
	WORD wStringSize=CountStringBuffer(RequestFailure.szDescribeString);
	WORD wSendSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString)+wStringSize;

	//��������
	SendDataToUser(pIServerUserItem,MDM_GR_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wSendSize);

	//����ṹ
	CMD_CS_C_ShutdownSocket ShutdownSocket;
	ShutdownSocket.bForceClose = false;

	//��������
	SendDataToUser(pIServerUserItem, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));

	return true;
}

//�󶨲���
tagBindParameter * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//��Ч����
	if (wBindIndex==INVALID_WORD_INDEX) return NULL;

	//��������
	if (wBindIndex<m_pGameServiceOption->wMaxPlayer)
	{
		return m_pNormalParameter+wBindIndex;
	}

	//��������
	if ((wBindIndex>=INDEX_ROBOT)&&(wBindIndex<(INDEX_ROBOT+MAX_ROBOT)))
	{
		return m_pRobotParameter+(wBindIndex-INDEX_ROBOT);
	}

	//�������
	ASSERT(FALSE);

	return NULL;
}

//��ȡ�û�
IServerUserItem * CAttemperEngineSink::GetServerUserItem(WORD wBindIndex, DWORD dwTokenID)
{
	//�󶨲���
	auto pTokenParameter = GetTokenParameter(wBindIndex, dwTokenID);
	if (pTokenParameter == NULL)
	{
		return NULL;
	}

	return (IServerUserItem*)pTokenParameter->pBindUserItem;
}

//���Ʋ���
tagTokenParameter * CAttemperEngineSink::GetTokenParameter(WORD wBindIndex, DWORD dwTokenID)
{
	//�󶨲���
	tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);
	if (pBindParameter == NULL) return NULL;

	//ֱ��ģʽ
	if (pBindParameter->wServiceModule == SERVICE_MODULE_NONE)
	{
		return &pBindParameter->TokenParameter;
	}
	else
	{
		//��ѯ����
		auto pGlobalGateItem = pBindParameter->pGlobalGateItem;
		if (pGlobalGateItem == NULL) return NULL;

		//��ѯ����
		auto pTokenParameter = pGlobalGateItem->SearchTokenID(dwTokenID);
		if (pTokenParameter == NULL || pTokenParameter->dwTokenID != dwTokenID) return NULL;

		return pTokenParameter;
	}

	return NULL;
}

//��չ����
tagExtendParameter* CAttemperEngineSink::GetExtendParameter(WORD wBindIndex, DWORD dwTokenID)
{
	//�󶨲���
	auto pTokenParameter = GetTokenParameter(wBindIndex, dwTokenID);
	if (pTokenParameter == NULL)
	{
		return NULL;
	}

	return (tagExtendParameter*)pTokenParameter->cbExtendData;
}

//��ǰ�汾
tagModuleVersion * CAttemperEngineSink::GetCurrentVersion(WORD wModuleID,DWORD dwDeviceKind)
{
	//����汾	
	for (INT_PTR i=0;i<m_ModuleVersionActive.GetCount();i++)
	{
		//�ж�����
		if (m_ModuleVersionActive[i].IsCurrent==false) continue;		
		if (m_ModuleVersionActive[i].wModuleID!=wModuleID) continue;		
		if (m_ModuleVersionActive[i].dwDeviceKind!=dwDeviceKind) continue;

		return &m_ModuleVersionActive[i];
	}

	return NULL;
}

//����汾
tagModuleVersion * CAttemperEngineSink::GetSpecialVersion(WORD wModuleID,DWORD dwDeviceKind,DWORD dwVersion)
{
	//����汾	
	for (INT_PTR i=0;i<m_ModuleVersionActive.GetCount();i++)
	{
		//�ж�����
		if (m_ModuleVersionActive[i].IsCurrent==true) continue; 
		if (m_ModuleVersionActive[i].wModuleID!=wModuleID) continue;
		if (m_ModuleVersionActive[i].dwVersion!=dwVersion) continue;
		if (m_ModuleVersionActive[i].dwDeviceKind!=dwDeviceKind) continue;

		return &m_ModuleVersionActive[i];
	}

	return NULL;
}

//�û�״��
WORD CAttemperEngineSink::GetRobotUserStatus(tagRobotUserStatus & RobotUserStatus)
{
	//��������
	WORD wEnumIndex = 0;
	IServerUserItem * pIServerUserItem=NULL;

	//���ñ���
	ZeroMemory(&RobotUserStatus,sizeof(RobotUserStatus));

	//�û���Ϣ
	do
	{
		//��ȡ�û�
		pIServerUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
		if (pIServerUserItem == NULL) break;

		//ͳ���ж�
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->IsRobotUser()==true))
		{
			//��ȡ����
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//�û�ͳ��
			switch (pIServerUserItem->GetUserStatus())
			{
			case US_FREE:
				{
					ASSERT(RobotUserStatus.wFreeUserCount<CountArray(RobotUserStatus.pServerUserFree));
					RobotUserStatus.pServerUserFree[RobotUserStatus.wFreeUserCount++]=pIServerUserItem;
					break;
				}
			case US_PLAYING:
				{
					ASSERT(RobotUserStatus.wPlayUserCount<CountArray(RobotUserStatus.pServerUserPlay));
					RobotUserStatus.pServerUserPlay[RobotUserStatus.wPlayUserCount++]=pIServerUserItem;
					break;
				}
			case US_SIT:
			case US_READY:
				{
					ASSERT(RobotUserStatus.wSitdownUserCount<CountArray(RobotUserStatus.pServerUserSitdown));
					RobotUserStatus.pServerUserSitdown[RobotUserStatus.wSitdownUserCount++]=pIServerUserItem;
					break;
				}
			}
		}

	} while (true);

	return RobotUserStatus.wFreeUserCount+RobotUserStatus.wPlayUserCount+RobotUserStatus.wSitdownUserCount;
}

//����״��
WORD CAttemperEngineSink::GetTableUserStatus(CTableFrameItem * pTableFrame, tagTableUserStatus & TableUserStatus)
{
	//���ñ���
	ZeroMemory(&TableUserStatus,sizeof(TableUserStatus));

	//�û�����
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//�û�����
		if (pIServerUserItem->IsRobotUser()==false)
		{
			TableUserStatus.wTableUserCount++;
		}
		else
		{
			TableUserStatus.wTableRobotCount++;
		}
	}

	//������Ŀ
	switch (pTableFrame->GetStartMode())
	{
	case StartMode_AllReady:	//����׼��
		{
			if(m_pGameServiceAttrib->wKindID==57)
				TableUserStatus.wLessUserCount=m_pGameServiceOption->wLessStartPlayer;
			else 
				TableUserStatus.wLessUserCount=2;

			break;
		}
	default:					//Ĭ��ģʽ
		{
			TableUserStatus.wLessUserCount=m_pGameServiceAttrib->wChairCount;
			break;
		}
	}

	return TableUserStatus.wTableRobotCount+TableUserStatus.wTableUserCount;
}


//�ֻ�������¼
bool CAttemperEngineSink::MobileUserImmediately(IServerUserItem * pIServerUserItem)
{
	//����У��
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//����ģʽ
	if (m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)
	{
		return InsertDistribute(pIServerUserItem);
	}

	//����ģʽ
	BYTE cbSearchMode=SEARCH_MODE_MAX_PLAYER;
	if (CServerRule::IsMatchUserByScore(m_pGameServiceOption->dwServerRule))
	{
		//���·���
		m_ScoreManager.UpdateScoreData();

		cbSearchMode|=SEARCH_MASK_MATCH_SCORE;
	}

	//���Ҳ�����
	return SearchTableAndSitdown(cbSearchMode,pIServerUserItem);
}

//�������û���Ϣ
bool CAttemperEngineSink::SendTableUserToUser(IServerUserItem * pITargetUserItem, BYTE cbClientKind, DWORD dwTableUserID)
{
	//У���û�
	ASSERT (pITargetUserItem != NULL);
	if (pITargetUserItem == NULL) return false;

	//��չ����
	auto pExtendParameter = GetExtendParameter(pITargetUserItem->GetBindIndex(), pITargetUserItem->GetTokenID());
	if (pExtendParameter == NULL) return true;
	if ((pExtendParameter->cbClientKind & cbClientKind) == 0) return true;

	//���ӱ�ʶ
	WORD wTableID = pITargetUserItem->GetTableID();
	if (wTableID >= m_TableFrameArray.GetCount()) return true;	

	//���嶨��
	WORD wDataSize = 0;
	BYTE cbBuffer[SOCKET_PACKET/2];		

	//��������
	WORD wEnumIndex = 0;
	CTableFrameItem* pTableFrame = m_TableFrameArray[wTableID];
	
	//������Ϣ
	bool bHideUserInfo = false;
	if (CServerRule::IsHideUserInfo(m_pGameServiceOption->dwServerRule) == true) bHideUserInfo = true;	

	//�ض����
	if (dwTableUserID != INVALID_DWORD)
	{
		auto pITableUserItem=pTableFrame->SearchUserItem(dwTableUserID);
		if (pITableUserItem != NULL && pITableUserItem!= pITargetUserItem)
		{
			//�������ݰ�
			wDataSize = ConstructEnterPacket(pITableUserItem, cbBuffer, sizeof(cbBuffer), pExtendParameter->cbClientKind, bHideUserInfo);

			//������Ϣ
			SendDataToUser(pITargetUserItem, MDM_GR_USER, SUB_GR_USER_ENTER, cbBuffer, wDataSize);
		}
	}
	else
	{
		//�����û�
		for (int i = 0; i < pTableFrame->GetChairCount(); i++)
		{
			auto pITableUserItem = pTableFrame->GetTableUserItem(i);
			if (pITableUserItem == NULL) continue;
			if (pITableUserItem == pITargetUserItem) continue;

			//�������ݰ�
			wDataSize = ConstructEnterPacket(pITableUserItem, cbBuffer, sizeof(cbBuffer), pExtendParameter->cbClientKind, bHideUserInfo);

			//������Ϣ
			SendDataToUser(pITargetUserItem, MDM_GR_USER, SUB_GR_USER_ENTER, cbBuffer, wDataSize);			
		}

		//�Թ��û�
		do
		{
			//ö���û�
			auto pITableUserItem = pTableFrame->EnumLookonUserItem(wEnumIndex++);
			if (pITableUserItem == NULL) break;
			if (pITableUserItem == pITargetUserItem) continue;

			//�������ݰ�
			wDataSize = ConstructEnterPacket(pITableUserItem, cbBuffer, sizeof(cbBuffer), pExtendParameter->cbClientKind, bHideUserInfo);

			//������Ϣ
			SendDataToUser(pITargetUserItem, MDM_GR_USER, SUB_GR_USER_ENTER, cbBuffer, wDataSize);

		} while (true);
	}
	
	return true;
}

//������Ϣ
bool CAttemperEngineSink::SendUserToTableUser(IServerUserItem* pIServerUserItem, BYTE cbClientKind, DWORD dwTableUserID)
{
	//У���û�
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return false;

	//���ӱ�ʶ
	WORD wTableID = pIServerUserItem->GetTableID();
	if (wTableID >= m_TableFrameArray.GetCount()) return true;

	//��������
	WORD wEnumIndex = 0;
	CTableFrameItem* pTableFrame = m_TableFrameArray[wTableID];

	//���嶨��
	BYTE cbPCBuffer[SOCKET_PACKET/2];
	BYTE cbMBBuffer[SOCKET_PACKET/2];

	//������Ϣ
	bool bHideUserInfo = false;
	if (CServerRule::IsHideUserInfo(m_pGameServiceOption->dwServerRule) == true) bHideUserInfo = true;

	//�������ݰ�
	auto wMBDataSize = ConstructEnterPacket(pIServerUserItem, cbMBBuffer, sizeof(cbMBBuffer), CLIENT_KIND_MOBILE, bHideUserInfo);
	auto wPCDataSize = ConstructEnterPacket(pIServerUserItem, cbPCBuffer, sizeof(cbPCBuffer),  CLIENT_KIND_COMPUTER, bHideUserInfo);
	
	//�ض����
	if (dwTableUserID != INVALID_DWORD)
	{
		auto pITableUserItem = pTableFrame->SearchUserItem(dwTableUserID);
		if (pITableUserItem != NULL && pITableUserItem != pIServerUserItem)
		{
			//��չ����
			auto pExtendParameter = GetExtendParameter(pITableUserItem->GetBindIndex(), pITableUserItem->GetTokenID());
			if (pExtendParameter != NULL && (pExtendParameter->cbClientKind & cbClientKind) != 0)
			{
				//������Ϣ
				if (pITableUserItem->IsMobileUser())
				{
					SendDataToUser(pITableUserItem, MDM_GR_USER, SUB_GR_USER_ENTER, cbMBBuffer, wMBDataSize);
				}
				else
				{
					SendDataToUser(pITableUserItem, MDM_GR_USER, SUB_GR_USER_ENTER, cbPCBuffer, wPCDataSize);
				}
			}			
		}
	}
	else
	{
		//�����û�
		for (int i = 0; i < pTableFrame->GetChairCount(); i++)
		{
			auto pITableUserItem = pTableFrame->GetTableUserItem(i);
			if (pITableUserItem == NULL) continue;
			if (pITableUserItem == pIServerUserItem) continue;

			//��չ����
			auto pExtendParameter = GetExtendParameter(pITableUserItem->GetBindIndex(), pITableUserItem->GetTokenID());
			if (pExtendParameter != NULL && (pExtendParameter->cbClientKind & cbClientKind) != 0)
			{
				//������Ϣ
				if (pITableUserItem->IsMobileUser())
				{
					SendDataToUser(pITableUserItem, MDM_GR_USER, SUB_GR_USER_ENTER, cbMBBuffer, wMBDataSize);
				}
				else
				{
					SendDataToUser(pITableUserItem, MDM_GR_USER, SUB_GR_USER_ENTER, cbPCBuffer, wPCDataSize);
				}
			}			
		}

		//�Թ��û�
		do
		{
			//ö���û�
			auto pITableUserItem = pTableFrame->EnumLookonUserItem(wEnumIndex++);
			if (pITableUserItem == NULL) break;
			if (pITableUserItem == pIServerUserItem) continue;

			//��չ����
			auto pExtendParameter = GetExtendParameter(pITableUserItem->GetBindIndex(), pITableUserItem->GetTokenID());
			if (pExtendParameter != NULL && (pExtendParameter->cbClientKind & cbClientKind) != 0)
			{
				//������Ϣ
				if (pITableUserItem->IsMobileUser())
				{
					SendDataToUser(pITableUserItem, MDM_GR_USER, SUB_GR_USER_ENTER, cbMBBuffer, wMBDataSize);
				}
				else
				{
					SendDataToUser(pITableUserItem, MDM_GR_USER, SUB_GR_USER_ENTER, cbPCBuffer, wPCDataSize);
				}
			}			

		} while (true);
	}

	return true;
}

//��������
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//��������
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->m_UIControlSink.PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//ִ�з���
bool CAttemperEngineSink::PerformDistribute()
{
	//����У��
	if (m_DistributeManager.GetNodeCount()<m_pGameServiceOption->wMinDistributeUser)
	{
		return false;
	}

	//�����û�
	while (true)
	{
		//��������
		CDistributeInfoArray DistributeInfoArray;

		 //��ȡ�û�
		WORD wRandCount=__max(m_pGameServiceOption->wMaxPartakeGameUser-m_pGameServiceOption->wMinPartakeGameUser+1,1);
		WORD wNeedCount=m_pGameServiceOption->wMinPartakeGameUser+rand()%wRandCount;
		wNeedCount=__min(m_DistributeManager.GetNodeCount(),wNeedCount);
		
		//ִ�з���
		if (m_DistributeManager.PerformDistribute(DistributeInfoArray,wNeedCount)==false)
		{
			break;
		}

		//��������
		WORD wFindTableID=SearchSuitableTable(SEARCH_MODE_NO_PLAYER,NULL);
		if (wFindTableID==INVALID_TABLE) break;

		//��������
		bool bSitSuccess=true;
		IServerUserItem * pIFailedUserItem=NULL;
		CTableFrameItem * pTableFrame=m_TableFrameArray[wFindTableID];

		//�������		
 		for (INT_PTR i=0; i<DistributeInfoArray.GetCount(); i++)
		{
			//��������
			WORD wChairID=pTableFrame->GetNullChairID();
			if (wChairID==INVALID_CHAIR) break;
				
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=DistributeInfoArray[i]->pIServerUserItem;
			if (pIServerUserItem!=NULL)
			{
				//�û�����
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					ITableFrameItem * pITableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
					pITableFrame->PerformStandUpAction(pIServerUserItem);
				}

				//�û�����
				if (pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==false)
				{
					bSitSuccess=false;
					pIFailedUserItem=pIServerUserItem;
					break;
				}
			}	
		}

		//�������
		for (INT_PTR i=0; i<DistributeInfoArray.GetCount(); i++)
		{
			//�û�ָ��
			IServerUserItem * pIServerUserItem=DistributeInfoArray[i]->pIServerUserItem;

			//�ɹ�����
			if (bSitSuccess==true || pIFailedUserItem==pIServerUserItem)
			{
				//�Ƴ�����
				RemoveDistribute(pIServerUserItem);
				continue;
			}

			//�û�����
			if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
			{
				//�������
				InsertDistribute(pIServerUserItem);
			}
		}
	}

	return true;
}

//ͳ������
DWORD CAttemperEngineSink::CollectTokenCount()
{
	//��������	
	DWORD dwTokenCount = 0;
	CGlobalGateItem * pGlobalGateItem= NULL;

	do
	{
		//ö������
		pGlobalGateItem = m_GlobalItemManager.EnumGateItem(pGlobalGateItem);
		if (pGlobalGateItem == NULL) break;

		//ͳ������
		dwTokenCount += pGlobalGateItem->GetTokenCount();

	} while (true);

	return dwTokenCount;
}

//��������
WORD CAttemperEngineSink::SearchSuitableTable(BYTE cbSearchMode,IServerUserItem * pIServerUserItem, WORD wLastTableID)
{
	//���ñ���
	WORD wRequestTableID=INVALID_TABLE;

	//��������
	if (cbSearchMode&SEARCH_MODE_NO_PLAYER)
	{
		//��������		
		for (WORD i=0; i<m_pGameServiceOption->wTableCount; i++)
		{
			//��ȡ����
			CTableFrameItem * pTableFrame=m_TableFrameArray[i];

			//�ж�״̬
			if (pTableFrame->IsTableLocked() || pTableFrame->IsGameStarted()) continue;			
			if (pTableFrame->GetNullChairCount()!=pTableFrame->GetChairCount()) continue;

			//���ñ���
			wRequestTableID=i;
			break;
		}

		return wRequestTableID;
	}

	//��������
	WORD wTableWeight=0;	
	WORD wTableWeightLevel=0;
	
	//��̬����
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsDynamicJoinGame(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;	

	//��������
	WORD wStartIndex=rand()%m_pGameServiceOption->wTableCount;
	for (WORD i=0; i<m_pGameServiceOption->wTableCount; i++)
	{
		//��ȡ����
		WORD wTableIndex=(i+wStartIndex)%m_pGameServiceOption->wTableCount;		
		CTableFrameItem * pTableFrame=m_TableFrameArray[wTableIndex];

		//��������
		if (pTableFrame->IsTableLocked()) continue;
		if (pTableFrame->GetTableID() == wLastTableID) continue;
		if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;		
		if (pTableFrame->GetNullChairCount()==pTableFrame->GetChairCount()) continue;
		if (pTableFrame->m_wMaxPlayerCount<=pTableFrame->GetSitUserCount()) continue;

		//������Ϣ
		tagTableUserInfo TableUerInfo;
		pTableFrame->GetTableUserInfo(TableUerInfo);

		//����Ȩֵ
		WORD wCurrWeightLevel=0,wCurrTableWeight=0;

		//ƥ�����
		if (cbSearchMode&SEARCH_MASK_MATCH_SCORE && pIServerUserItem!=NULL)
		{
			//��ȡ����
			INT nAreaIndex1=GetTableScoreAreaIndex(pTableFrame);				
			INT nAreaIndex2=m_ScoreManager.GetScoreAreaIndex(pIServerUserItem->GetUserFreeScore());	

			//ƥ������
			if (nAreaIndex1!=nAreaIndex2) continue;			
		}

		//�������
		if (cbSearchMode&SEARCH_MODE_MIN_PLAYER)
		{
			wCurrTableWeight=TableUerInfo.wTableReadyCount;
			wCurrWeightLevel=(TableUerInfo.wTableUserCount==0)?TableUerInfo.wTableRobotCount:0;
			wCurrTableWeight+=wCurrWeightLevel*m_pGameServiceAttrib->wChairCount;
		}

		//������
		if (cbSearchMode&SEARCH_MODE_MAX_PLAYER)
		{
			wCurrTableWeight=TableUerInfo.wTableReadyCount;
			wCurrWeightLevel=TableUerInfo.wTableUserCount+TableUerInfo.wTableRobotCount;			
			wCurrTableWeight+=wCurrWeightLevel*m_pGameServiceAttrib->wChairCount;
		}

		//Ȩ�رȽ�
		if (wCurrTableWeight>=wTableWeight && wCurrWeightLevel>=wTableWeightLevel)
		{
			//��¼Ȩ��
			wRequestTableID=wTableIndex;				
			wTableWeight=wCurrTableWeight;
			wTableWeightLevel=wCurrWeightLevel;
		}
	}

	//�жϽ��
	if (wRequestTableID==INVALID_TABLE && cbSearchMode&SEARCH_MODE_MIN_PLAYER)
	{
		//���ҿ���
		wStartIndex=rand()%m_pGameServiceOption->wTableCount;
		for (WORD i=0; i<m_pGameServiceOption->wTableCount; i++)
		{
			//��ȡ����
			WORD wTableIndex=(i+wStartIndex)%m_pGameServiceOption->wTableCount;			
			CTableFrameItem * pTableFrame=m_TableFrameArray[wTableIndex];

			//�ж�״̬
			if (pTableFrame->IsTableLocked()) continue;
			if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;			
			if (pTableFrame->GetNullChairCount()<pTableFrame->GetChairCount()) continue;	

			//���ñ���
			wRequestTableID=pTableFrame->GetTableID();
			break;
		}
	}

	//�жϽ��
	if (wRequestTableID==INVALID_TABLE)
	{
		//��������
		wStartIndex=rand()%m_pGameServiceOption->wTableCount;
		for (WORD i=0; i<m_pGameServiceOption->wTableCount; i++)
		{
			//��ȡ����
			WORD wTableIndex=(i+wStartIndex)%m_pGameServiceOption->wTableCount;			
			CTableFrameItem * pTableFrame=m_TableFrameArray[wTableIndex];

			//�ж�״̬
			if (pTableFrame->IsTableLocked()) continue;			
			if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;			
			if (pTableFrame->m_wMaxPlayerCount<=pTableFrame->GetSitUserCount()) continue;

			//ƥ�����
			if (cbSearchMode&SEARCH_MASK_MATCH_SCORE && pIServerUserItem!=NULL && pTableFrame->GetSitUserCount()>0)
			{
				//��ȡ����
				INT nAreaIndex1=GetTableScoreAreaIndex(pTableFrame);				
				INT nAreaIndex2=m_ScoreManager.GetScoreAreaIndex(pIServerUserItem->GetUserFreeScore());	

				//ƥ������
				if (nAreaIndex1!=nAreaIndex2) continue;			
			}

			//���ñ���
			wRequestTableID=pTableFrame->GetTableID();
			break;
		}
	}

	return wRequestTableID;
}

//��������
bool CAttemperEngineSink::SearchTableAndSitdown(BYTE cbSearchMode,IServerUserItem * pIServerUserItem, WORD wLastTableID)
{
	//��������
	WORD wFindTableID=SearchSuitableTable(cbSearchMode,pIServerUserItem,wLastTableID);
	if (wFindTableID!=INVALID_TABLE)
	{
		//��������
		CTableFrameItem * pTableFrame=m_TableFrameArray[wFindTableID];

		//��Ч����
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID!=INVALID_CHAIR)
		{
			//��������
			if (pIServerUserItem->GetTableID()!=INVALID_TABLE && pIServerUserItem->GetChairID()!=INVALID_CHAIR)
			{
				CTableFrameItem * pTableFramePrep=m_TableFrameArray[pIServerUserItem->GetTableID()];
				pTableFramePrep->PerformStandUpAction(pIServerUserItem);
			}

			//�û�����
			pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);

			return true;
		}
	}

	//ʧ��
	SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0039,TEXT("û�ҵ��ɽ������Ϸ����")),REQUEST_FAILURE_NORMAL);

	return true;
}


//�û�����
WORD CAttemperEngineSink::ConstructEnterPacket(IServerUserItem *pIServerUserItem,VOID * pBuffer,WORD wBufferSize, BYTE cbClientKind, bool bHideUserInfo)
{
	//����У��
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return 0;

	//ת������
	auto pUserInfo=pIServerUserItem->GetUserInfo();
	auto pServerUserItem = (CServerUserItem*)pIServerUserItem;

	//�ն�����
	if (cbClientKind == CLIENT_KIND_COMPUTER)
	{
		//����ṹ
		auto pUserInfoHead = (tagPCUserInfoHead*)pBuffer;
		CSendPacketHelper SendPacket((PBYTE)pBuffer + sizeof(tagPCUserInfoHead), wBufferSize - sizeof(tagPCUserInfoHead));

		//�û�����
		pUserInfoHead->bIsRobot = true;
		pUserInfoHead->wFaceID = pUserInfo->wFaceID;
		pUserInfoHead->dwGameID = pUserInfo->dwGameID;
		pUserInfoHead->dwUserID = pUserInfo->dwUserID;
		pUserInfoHead->dwCustomID = pUserInfo->dwCustomID;

		//�û�����
		pUserInfoHead->cbAge = pUserInfo->cbAge;
		pUserInfoHead->cbGender = pUserInfo->cbGender;
		pUserInfoHead->cbMasterOrder = pUserInfo->cbMasterOrder;

		//�û�״̬
		pUserInfoHead->wTableID = pUserInfo->wTableID;
		pUserInfoHead->wChairID = pUserInfo->wChairID;
		pUserInfoHead->cbUserStatus = pUserInfo->cbUserStatus;

		//�û�����
		pUserInfoHead->lGold = pUserInfo->lGold;
		pUserInfoHead->lScore = pUserInfo->lScore;
		pUserInfoHead->cbScoreKind = pUserInfo->cbScoreKind;
		pUserInfoHead->lInsureGold = pUserInfo->lInsureGold;
		pUserInfoHead->lWinCount = pUserInfo->lWinCount;
		pUserInfoHead->lLostCount = pUserInfo->lLostCount;
		pUserInfoHead->lDrawCount = pUserInfo->lDrawCount;
		pUserInfoHead->lFleeCount = pUserInfo->lFleeCount;
		pUserInfoHead->lExperience = pUserInfo->lExperience;
		pUserInfoHead->lWinStreak = pUserInfo->lWinStreak;

		//KK JIA
		SendPacket.AddPacket(pUserInfo->szUserHeadUrl, DTP_GR_USER_HEADURL);//258

		//�û��˺�
		SendPacket.AddPacket(pUserInfo->szAccounts, DTP_GR_USER_NICKNAME);//34

		//�û�ǩ��
		SendPacket.AddPacket(pUserInfo->szUnderWrite, DTP_GR_USER_UNDER_WRITE);//66

	
		/*CString s;
		s.Format(_T("bbbb szUserHeadUrl:%s, wBufferSize:%d - sizeof(tagPCUserInfoHead):%d"), pUserInfo->szUserHeadUrl, wBufferSize , sizeof(tagPCUserInfoHead));
		CTraceService::TraceString(s, TraceLevel_Info);
		*/

		return sizeof(tagPCUserInfoHead) + SendPacket.GetDataSize();		
	}
	else
	{
		//����ṹ
		auto pUserInfoHead = (tagMBUserInfoHead*)pBuffer;
		CSendPacketHelper SendPacket((PBYTE)pBuffer + sizeof(tagMBUserInfoHead), wBufferSize - sizeof(tagMBUserInfoHead));

		//�û�����
		pUserInfoHead->wFaceID = pUserInfo->wFaceID;
		pUserInfoHead->dwGameID = pUserInfo->dwGameID;
		pUserInfoHead->dwUserID = pUserInfo->dwUserID;
		pUserInfoHead->dwCustomID = pUserInfo->dwCustomID;
		
		//�û�����
		pUserInfoHead->cbAge = pUserInfo->cbAge;
		pUserInfoHead->cbGender = pUserInfo->cbGender;
		pUserInfoHead->cbMasterOrder = pUserInfo->cbMasterOrder;

		//�û�״̬
		pUserInfoHead->wTableID = pUserInfo->wTableID;
		pUserInfoHead->wChairID = pUserInfo->wChairID;
		pUserInfoHead->cbUserStatus = pUserInfo->cbUserStatus;

		//�û�����
		pUserInfoHead->lGold = pUserInfo->lGold;
		pUserInfoHead->lScore = pUserInfo->lScore;
		pUserInfoHead->cbScoreKind = pUserInfo->cbScoreKind;
		pUserInfoHead->lWinCount = pUserInfo->lWinCount;
		pUserInfoHead->lLostCount = pUserInfo->lLostCount;
		pUserInfoHead->lDrawCount = pUserInfo->lDrawCount;
		pUserInfoHead->lFleeCount = pUserInfo->lFleeCount;
		pUserInfoHead->lExperience = pUserInfo->lExperience;
		pUserInfoHead->lWinStreak = pUserInfo->lWinStreak;

		//λ����Ϣ
		pUserInfoHead->dGlobalPosLat = pServerUserItem->GetGlobalPosLat();
		pUserInfoHead->dGlobalPosLng = pServerUserItem->GetGlobalPosLng();
		CopyMemory(pUserInfoHead->szPlaceName, pServerUserItem->GetGlobalPlace(),sizeof(pUserInfoHead->szPlaceName));

		//������Ϣ
		if (bHideUserInfo == true)
		{
			pUserInfoHead->dwGameID = 0;
			pUserInfoHead->dwCustomID = 0;

			//��������
			if ((m_pGameServiceOption->wServerType & GAME_GENRE_MATCH) != 0)
			{
				pUserInfoHead->wFaceID = pUserInfoHead->cbGender == GENDER_MANKIND ? 10 : 11;
				SendPacket.AddPacket(TEXT("������"), DTP_GR_USER_NICKNAME);
			}
			else
			{
				pUserInfoHead->wFaceID = pUserInfoHead->cbGender == GENDER_MANKIND ? 0 : 35;
				SendPacket.AddPacket(TEXT("��Ϸ���"), DTP_GR_USER_NICKNAME);
			}
			//KK JIA
			SendPacket.AddPacket(pUserInfo->szUserHeadUrl, DTP_GR_USER_HEADURL);//258

		}
		else
		{
			//�û��˺�
			SendPacket.AddPacket(pUserInfo->szAccounts, DTP_GR_USER_NICKNAME);

			//�û�ǩ��
			SendPacket.AddPacket(pUserInfo->szUnderWrite, DTP_GR_USER_UNDER_WRITE);
			//KK JIA
			SendPacket.AddPacket(pUserInfo->szUserHeadUrl, DTP_GR_USER_HEADURL);//258

		}

		return sizeof(tagMBUserInfoHead) + SendPacket.GetDataSize();
	}

	return 0;
}

//�û�����
WORD CAttemperEngineSink::ConstructScorePacket(IServerUserItem * pIServerUserItem,VOID * pBuffer,WORD wBufferSize)
{
	//����У��
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return 0;

	//�û���Ϣ
	auto pUserInfo=pIServerUserItem->GetUserInfo();

	//��������
	auto pUserScore=(CMD_GR_UserScore*)pBuffer;

	//������Ϣ
	pUserScore->UserScore.lScore = pUserInfo->lScore;
	pUserScore->UserScore.cbScoreKind = pUserInfo->cbScoreKind;

	//������Ϣ
	pUserScore->dwUserID=pUserInfo->dwUserID;
	pUserScore->UserScore.lWinCount=pUserInfo->lWinCount;
	pUserScore->UserScore.lLostCount=pUserInfo->lLostCount;
	pUserScore->UserScore.lDrawCount=pUserInfo->lDrawCount;
	pUserScore->UserScore.lFleeCount=pUserInfo->lFleeCount;
	pUserScore->UserScore.lWinStreak = pUserInfo->lWinStreak;
	pUserScore->UserScore.lExperience=pUserInfo->lExperience;	

	return sizeof(CMD_GR_UserScore);	
}

//�û�״̬
WORD CAttemperEngineSink::ConstructStatusPacket(IServerUserItem *pIServerUserItem,VOID * pBuffer,WORD wBufferSize)
{
	//����У��
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return 0;

	//��ȡ�ṹ
	auto pUserStatus=(CMD_GR_UserStatus *)pBuffer;

	//��ȡ��Ϣ
	auto pUserInfo=pIServerUserItem->GetUserInfo();

	//��������
	pUserStatus->dwUserID=pUserInfo->dwUserID;
	pUserStatus->UserStatus.wTableID=pUserInfo->wTableID;
	pUserStatus->UserStatus.wChairID=pUserInfo->wChairID;
	pUserStatus->UserStatus.cbUserStatus=pUserInfo->cbUserStatus;

	return sizeof(CMD_GR_UserStatus);
}

//Ч��˽��Ȩ��
bool CAttemperEngineSink::EfficacyUserWhisper(IServerUserItem *pISendUserItem, IServerUserItem *pIRecvUserItem)
{
	//״̬�ж�
	BYTE cbMasterOrderSend=pISendUserItem->GetMasterOrder();
	BYTE cbMasterOrderRecv=pIRecvUserItem->GetMasterOrder();
	if ((g_GlobalUnits.IsAllowWisper()==false)&&(cbMasterOrderSend==0)&&(cbMasterOrderRecv==0))
	{
		//���ǻ�Ա
		if( !g_GlobalUnits.IsMemberCanChat() || pISendUserItem->GetMemberOrder() == 0 )
		{
			SendRoomMessage(pISendUserItem, CWHService::FormatMessage(MIM_SERVER_0040,TEXT("��Ǹ������Ϸ���䲻�����û�˽�ģ�")),SMT_EJECT|SMT_CHAT);
			return false;
		}
	}

	//Ȩ���ж�
	if (CUserRight::CanWisper(pISendUserItem->GetUserRight())==false&&cbMasterOrderSend==0&&cbMasterOrderRecv==0)
	{
		SendRoomMessage(pISendUserItem, CWHService::FormatMessage(MIM_SERVER_0041,TEXT("��Ǹ����û�з���˽�ĵ�Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��")),SMT_EJECT|SMT_CHAT);
		return false;
	}

	//ͬ����ֹ˽��
	if( g_GlobalUnits.IsForbidPlayChat() && pISendUserItem->GetUserStatus() == US_PLAYING &&
		pISendUserItem->GetTableID() == pIRecvUserItem->GetTableID() &&
		pIRecvUserItem->GetUserStatus() == US_PLAYING && cbMasterOrderSend == 0 && cbMasterOrderRecv == 0 )
	{
		if( !CUserRight::CanGamePlayChat(pISendUserItem->GetUserRight()) )
		{
			SendRoomMessage(pISendUserItem, CWHService::FormatMessage(MIM_SERVER_0042,TEXT("��Ǹ������Ϸ���䲻��������Ϸ����ͬ�������˽�ģ�")),SMT_EJECT|SMT_CHAT);
			return false;
		}
	}

	return true;
}

//Ч������
bool CAttemperEngineSink::EfficacySocketContext(DWORD dwSocketID, DWORD dwTokenID)
{
	//��������
	tagTokenParameter * pTokenParameter=GetTokenParameter(LOWORD(dwSocketID), dwTokenID);
	if (pTokenParameter == NULL) return false;

	//�����ж�
	return (pTokenParameter->dwTokenID==dwTokenID);
}

//�㲥�¼�
bool CAttemperEngineSink::OnTCPSocketMainBroadcast(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_GAME_BROADCAST:	//�û�����
		{
			//��Ϣ����
			CMD_Buffer * pBuffer=(CMD_Buffer *)pData;

			//Ч������
			ASSERT(wDataSize>=(sizeof(pBuffer->Head)));
			ASSERT(wDataSize==(sizeof(CMD_Buffer)-sizeof(pBuffer->cbBuffer)+pBuffer->Head.CmdInfo.wPacketSize));	
		}
	}
	return true;
}

//ʱ����
SCORE CAttemperEngineSink::GetIntervalSeconds(SYSTEMTIME stStart, SYSTEMTIME stEnd)
{
	CTime tStart(stStart);
	CTime tEnd(stEnd);
	CTimeSpan tsInverval=tStart-tEnd;
	return tsInverval.GetTotalSeconds();
}

//���дʹ���
bool CAttemperEngineSink::FilterWords(LPCTSTR szFiltered)
{
	return m_WordsFilter.FilterWords(szFiltered);
}

//���дʹ���
void CAttemperEngineSink::FilterWords(LPWSTR szFiltered, int nMsgLen)
{
	m_WordsFilter.FilterWords(szFiltered, nMsgLen);
}

//�������
bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_EXECUTE_COMMAND:			//ִ������
		{
			//����У��
			ASSERT (sizeof(tagExecuteCommand)==wDataSize);
			if (sizeof(tagExecuteCommand)!=wDataSize) return false;

			//��ȡ����
			auto pCommandCode=(tagExecuteCommand*)pData;

			//��������
			DBR_GR_LoadConfig LoadConfig;
			ZeroMemory(&LoadConfig, sizeof(LoadConfig));

			//��������
			if (pCommandCode->dwCommandCode&CMD_LOAD_CONFIG_DATA)
			{				
				//��������
				LoadConfig.dwConfigMask = pCommandCode->dwCommandParam;

				//��������
				if (pCommandCode->dwCommandParam & CONFIG_MASK_BATTLE_OPTION && (m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) == 0)
				{
					LoadConfig.dwConfigMask &= ~CONFIG_MASK_BATTLE_OPTION;
				}

				//Ͷ������
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_CONFIG, 0, 0, 0, &LoadConfig, sizeof(LoadConfig));
			}						

			return true;
		}
	case SUB_CS_S_MODIFY_OPTION:		//�޸�ѡ��
		{
			//����У��
			ASSERT (sizeof(tagModifyOption)==wDataSize);
			if (sizeof(tagModifyOption)!=wDataSize) return false;

			//��ȡ����
			auto pModifyOption=(tagModifyOption*)pData;

			//�Թ���Ϸ
			if (pModifyOption->dwServerRuleDel&SR_FORBID_LOOKON || pModifyOption->dwServerRuleAdd&SR_FORBID_LOOKON)
			{	
				CServerRule::SetForbidLookon(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleDel&SR_FORBID_LOOKON)?true:false);
			}

			//����ռ��
			if (pModifyOption->dwServerRuleDel&SR_ALLOW_ROBOT_SIMULATE || pModifyOption->dwServerRuleAdd&SR_ALLOW_ROBOT_SIMULATE)
			{	
				//����Ȩ��
				bool bAllow=g_GlobalUnits.IsAllowRobotSimulate();
				g_GlobalUnits.SetAllowRobotSimulate((pModifyOption->dwServerRuleAdd&SR_ALLOW_ROBOT_SIMULATE)?true:false);
				CServerRule::SetAllowRobotSimulate(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleAdd&SR_ALLOW_ROBOT_SIMULATE)?true:false);
				if (bAllow!=g_GlobalUnits.IsAllowRobotSimulate())
				{
					if (g_GlobalUnits.IsAllowRobotSimulate())
					{
						m_pITimerEngine->SetTimer(IDI_LOAD_ROBOT_INFO,TIME_LOAD_ROBOT_INFO*1000L,1,NULL);
						m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ROBOT,TIME_DISTRIBUTE_ROBOT*1000L*2,TIMES_INFINITY,NULL);
					}
					else
					{
						m_pITimerEngine->KillTimer(IDI_LOAD_ROBOT_INFO);
					}
				}
			}

			//�������
			if (pModifyOption->dwServerRuleDel&SR_ALLOW_ROBOT_ATTEND || pModifyOption->dwServerRuleAdd&SR_ALLOW_ROBOT_ATTEND)
			{	
				//����Ȩ��
				bool bAllow=g_GlobalUnits.IsAllowRobotAttend();
				g_GlobalUnits.SetAllowRobotAttend((pModifyOption->dwServerRuleAdd&SR_ALLOW_ROBOT_ATTEND)?true:false);
				CServerRule::SetAllowRobotAttend(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleAdd&SR_ALLOW_ROBOT_ATTEND)?true:false);
				//���ö�ʱ��
				if (bAllow!=g_GlobalUnits.IsAllowRobotAttend())
				{
					if (g_GlobalUnits.IsAllowRobotAttend())
					{
						m_pITimerEngine->SetTimer(IDI_LOAD_ROBOT_INFO,TIME_LOAD_ROBOT_INFO*1000L,1,NULL);
						m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ROBOT,TIME_DISTRIBUTE_ROBOT*1000L*2,TIMES_INFINITY,NULL);
					}
					else
					{
						m_pITimerEngine->KillTimer(IDI_LOAD_ROBOT_INFO);
					}
				}
			}				

			//���͵ͱ�
			if (pModifyOption->dwServerRuleDel&SR_ALLOW_LARGESS_SUBSIDY || pModifyOption->dwServerRuleAdd&SR_ALLOW_LARGESS_SUBSIDY)
			{	
				CServerRule::SetAllowLargessSubsidy(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleAdd&SR_ALLOW_LARGESS_SUBSIDY)?true:false);
			}

			//ǿ������
			if (pModifyOption->dwServerRuleDel&SR_FORBIT_FORCE_STANDUP || pModifyOption->dwServerRuleAdd&SR_FORBIT_FORCE_STANDUP)
			{	
				CServerRule::SetForbidForceStandup(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleDel&SR_FORBIT_FORCE_STANDUP)?true:false);
			}

			//��������
			if (pModifyOption->dwServerRuleDel&SR_FORBIT_OFFLINE_STANDUP || pModifyOption->dwServerRuleAdd&SR_FORBIT_OFFLINE_STANDUP)
			{	
				CServerRule::SetForbidOfflineStandup(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleDel&SR_FORBIT_OFFLINE_STANDUP)?true:false);
			}

			//��¼�ɼ�
			if (pModifyOption->dwServerRuleDel&SR_RECORD_GAME_SCORE || pModifyOption->dwServerRuleAdd&SR_RECORD_GAME_SCORE)
			{	
				CServerRule::SetRecordGameScore(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleAdd&SR_RECORD_GAME_SCORE)?true:false);
			}

			//��Ϸ¼��
			if (pModifyOption->dwServerRuleDel&SR_RECORD_GAME_VIDEO || pModifyOption->dwServerRuleAdd&SR_RECORD_GAME_VIDEO)
			{	
				CServerRule::SetRecordGameVideo(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleAdd&SR_RECORD_GAME_VIDEO)?true:false);
			}

			//��ʱд��
			if (pModifyOption->dwServerRuleDel&SR_IMMEDIATE_WRITE_SCORE || pModifyOption->dwServerRuleAdd&SR_IMMEDIATE_WRITE_SCORE)
			{	
				CServerRule::SetImmediateWriteScore(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleAdd&SR_IMMEDIATE_WRITE_SCORE)?true:false);
			}

			//��̬����
			if (pModifyOption->dwServerRuleDel&SR_DYNAMIC_JOIN_GAME || pModifyOption->dwServerRuleAdd&SR_DYNAMIC_JOIN_GAME)
			{	
				CServerRule::SetDynamicJoinGame(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleAdd&SR_DYNAMIC_JOIN_GAME)?true:false);
			}	

			//��������
			if (pModifyOption->dwServerRuleDel&SR_ALLOW_VOICE_CHAT || pModifyOption->dwServerRuleAdd&SR_ALLOW_VOICE_CHAT)
			{	
				CServerRule::SetAllowVoiceChat(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleAdd&SR_ALLOW_VOICE_CHAT)?true:false);
			}

			//ʤ������
			if (pModifyOption->dwTableRuleDel&TR_FORBID_WINRATE_SET || pModifyOption->dwTableRuleAdd&TR_FORBID_WINRATE_SET)
			{	
				CServerRule::SetForbidWinRateSet(m_pGameServiceOption->dwServerRule,(pModifyOption->dwTableRuleDel&TR_FORBID_WINRATE_SET)?true:false);
			}

			//��������
			if (pModifyOption->dwTableRuleDel&TR_FORBID_FLEERATE_SET || pModifyOption->dwTableRuleAdd&TR_FORBID_FLEERATE_SET)
			{	
				CServerRule::SetForbidFleeRateSet(m_pGameServiceOption->dwServerRule,(pModifyOption->dwTableRuleDel&TR_FORBID_FLEERATE_SET)?true:false);
			}

			//��ַ����
			if (pModifyOption->dwTableRuleDel&TR_FORBID_IP_LIMIT || pModifyOption->dwTableRuleAdd&TR_FORBID_IP_LIMIT)
			{	
				CServerRule::SetForbidIPLimit(m_pGameServiceOption->dwServerRule,(pModifyOption->dwTableRuleDel&TR_FORBID_IP_LIMIT)?true:false);
			}

			//�ɼ�����
			if (pModifyOption->dwTableRuleDel&TR_FORBID_SCORE_LIMIT || pModifyOption->dwTableRuleAdd&TR_FORBID_SCORE_LIMIT)
			{	
				CServerRule::SetForbidScoreLimit(m_pGameServiceOption->dwServerRule,(pModifyOption->dwTableRuleDel&TR_FORBID_SCORE_LIMIT)?true:false);
			}

			//���ܻ�ӭ����
			if (pModifyOption->dwTableRuleDel&TR_FORBID_BLACKLIST_SET || pModifyOption->dwTableRuleAdd&TR_FORBID_BLACKLIST_SET)
			{	
				CServerRule::SetForbidBlackListSet(m_pGameServiceOption->dwServerRule,(pModifyOption->dwTableRuleDel&TR_FORBID_BLACKLIST_SET)?true:false);
			}

			//��������
			if (pModifyOption->dwTableRuleDel&TR_FORBID_PASSWORD_SET || pModifyOption->dwTableRuleAdd&TR_FORBID_PASSWORD_SET)
			{	
				CServerRule::SetForbidPasswordSet(m_pGameServiceOption->dwServerRule,(pModifyOption->dwTableRuleDel&TR_FORBID_PASSWORD_SET)?true:false);
			}

			//�׷���ʾ
			if (pModifyOption->dwTableRuleDel&TR_FORBID_SHOW_BASESCORE || pModifyOption->dwTableRuleAdd&TR_FORBID_SHOW_BASESCORE)
			{	
				CServerRule::SetForbidShowBaseScore(m_pGameServiceOption->dwServerRule,(pModifyOption->dwTableRuleDel&TR_FORBID_SHOW_BASESCORE)?true:false);
			}

			//��������
			ModifyGameServer();

			return true;
		}
	}

	return true;
}

//���½��
bool CAttemperEngineSink::OnDBUpdateGold(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��������
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem *pIServerUserItem=GetServerUserItem(wBindIndex, dwTokenID);
	if(pIServerUserItem==NULL) return false;

	//����ṹ
	DBO_GR_RelockScore *pRelockScore=(DBO_GR_RelockScore*)pData;

	//��������
	auto pUserInfo=pIServerUserItem->GetUserInfo();

	CServerUserItem *pServerUserItem=(CServerUserItem *)m_ServerUserManager.SearchUserItem(pUserInfo->dwUserID);
	if(pServerUserItem!=NULL)
	{
		if(m_pGameServiceOption->lRestrictScore==0)
		{
			pServerUserItem->SetFreeScore(pRelockScore->lFreeScore);
			pServerUserItem->SetLockScore(pRelockScore->lFreeScore);
		}
		else
		{
			pServerUserItem->SetFreeScore(m_pGameServiceOption->lRestrictScore);
			pServerUserItem->SetLockScore(pRelockScore->lFreeScore-m_pGameServiceOption->lRestrictScore);
		}
	}

	if ((m_pGameServiceOption->wServerType & (GAME_GENRE_GOLD|GAME_GENRE_BATTLE)) != 0)
	{
		pUserInfo->lScore=pRelockScore->lGold;
		pUserInfo->lInsureGold=pRelockScore->lInsureGold;
	}
	
	SendUserScore(pIServerUserItem);
	
	return true;
}

//��������
bool CAttemperEngineSink::InitTableFrame()
{
	//��������
	m_TableFrameArray.SetSize(m_pGameServiceOption->wTableCount);
	ZeroMemory(m_TableFrameArray.GetData(), m_pGameServiceOption->wTableCount * sizeof(CTableFrameItem*));

	//���Ӳ���
	tagTableFrameParameter TableFrameParameter;
	ZeroMemory(&TableFrameParameter, sizeof(TableFrameParameter));

	//���Ӳ���
	TableFrameParameter.pITimerEngine = m_pITimerEngine;
	TableFrameParameter.pITCPSocketService = m_pITCPSocketService;
	TableFrameParameter.pGameServiceAttrib = m_pGameServiceAttrib;
	TableFrameParameter.pGameServiceOption = m_pGameServiceOption;
	TableFrameParameter.pIGameVideoManager = m_pIGameVideoManager;
	TableFrameParameter.pIGameServiceManager = m_pIGameServiceManager;
	TableFrameParameter.pIDataBaseEngineEngine = m_pIDataBaseEngine;
	TableFrameParameter.pIAssistDataBaseEngine = m_pIDataBaseEngine;
	TableFrameParameter.pIServiceFunction = QUERY_ME_INTERFACE(IServiceFunction);
	TableFrameParameter.pITableEventSink = QUERY_ME_INTERFACE(ITableEventSink);
	TableFrameParameter.pIRobotUserManager = QUERY_OBJECT_INTERFACE(m_RobotUserManager, IRobotUserManager);

	//��������
	for (WORD i = 0; i < m_pGameServiceOption->wTableCount; i++)
	{
		if (m_pIExtendServiceManager != NULL)
		{
			m_TableFrameArray[i] = (CTableFrameItem *)m_pIExtendServiceManager->CreateExtendTableFrame();
		}		
		else
		{
			m_TableFrameArray[i] = new CTableFrameItem;			
		}

		//��ʼ��
		if (m_TableFrameArray[i]->InitTableFrame(i, TableFrameParameter) == false) return false;		
	}

	return true;
}

//���÷���
bool CAttemperEngineSink::InitExtendService()
{
	//����У��
	if(m_pIExtendServiceManager==NULL) return true;

	//���Ӳ���
	tagExtendServiceParameter ExtendServiceParameter;
	ZeroMemory(&ExtendServiceParameter,sizeof(ExtendServiceParameter));

	//���ò���
	ExtendServiceParameter.pGameServiceOption=m_pGameServiceOption;
	ExtendServiceParameter.pGameServiceAttrib=m_pGameServiceAttrib;

	//�ں����
	ExtendServiceParameter.pITimerEngine=m_pITimerEngine;
	ExtendServiceParameter.pIDataBaseEngine = m_pIDataBaseEngine;
	ExtendServiceParameter.pITCPSocketService=m_pITCPSocketService;
	ExtendServiceParameter.pTCPNetworkEngine=m_pITCPNetworkEngine;	
	ExtendServiceParameter.pITCPSocketCenterServer=m_pITCPSocketCenter;

	//�������
	ExtendServiceParameter.pIMainServiceFrame=this;
	ExtendServiceParameter.pIRobotUserManager=&m_RobotUserManager;
	ExtendServiceParameter.pIServerUserManager=QUERY_OBJECT_INTERFACE(m_ServerUserManager,IServerUserManager);

	//��ʼ���ӿ�
	m_pIExtendServiceManager->InitServiceManager(ExtendServiceParameter);

	return true;
}

//���û���
bool CAttemperEngineSink::InitRobotService()
{
	//��������	
	m_RobotUserManager.m_pITimerEngine = m_pITimerEngine;
	m_RobotUserManager.m_pGameServiceAttrib = m_pGameServiceAttrib;
	m_RobotUserManager.m_pGameServiceOption = m_pGameServiceOption;
	m_RobotUserManager.m_pIGameServiceManager = m_pIGameServiceManager;
	m_RobotUserManager.m_pIGameServiceSustomTime = m_pIGameServiceSustomTime;
	m_RobotUserManager.m_pIServiceFunction = QUERY_ME_INTERFACE(IServiceFunction);
	m_RobotUserManager.m_pIServerUserManager = QUERY_OBJECT_INTERFACE(m_ServerUserManager, IServerUserManager);
	m_RobotUserManager.m_pITCPNetworkEngineEvent = QUERY_OBJECT_PTR_INTERFACE(m_pIAttemperEngine, ITCPNetworkEngineEvent);

	return m_RobotUserManager.StartService();
}

//¼�����
bool CAttemperEngineSink::InitVideoService()
{
	//¼��ӿ�
	if (m_pIGameVideoManager != NULL)
	{
		//��������
		tagGameVideoParameter GameVideoParameter;

		//����ṹ
		GameVideoParameter.pGameServiceOption = m_pGameServiceOption;
		GameVideoParameter.pGameServiceAttrib = m_pGameServiceAttrib;

		//����¼��
		m_pIGameVideoManager->InitVideoManager(GameVideoParameter);

		//���ýӿ�
		if (m_pIGameVideoManager->SetGameVideoItemSink(QUERY_ME_INTERFACE(IGameVideoItemSink)) == false)
		{
			ASSERT(FALSE);
			return false;
		}		
	}

	return true;
}

//���÷���
bool CAttemperEngineSink::InitServerConfig()
{
	CMD_ConfigServer.wKindID = m_pGameServiceAttrib->wKindID;
	CMD_ConfigServer.wTableCount = m_pGameServiceOption->wTableCount;
	CMD_ConfigServer.wChairCount = m_pGameServiceAttrib->wChairCount;
	CMD_ConfigServer.wServerType = m_pGameServiceOption->wServerType;
	CMD_ConfigServer.dwServerRule = m_pGameServiceOption->dwServerRule;

	return true;
}


//������־
bool CAttemperEngineSink::InitLoggerService()
{
	//ʹ������
	g_TimeLoggerService->EnableLocal(false);
	g_TimeLoggerService->EnableRemote(true);
	g_TimeLoggerService->EnablePrefix(false);
	g_TimeLoggerService->EnableSufix(true);

	//������Ϣ
	g_TimeLoggerService->SetFormat(TEXT("%Y%m%d%H%M"));
	g_TimeLoggerService->SetConcatStr(TEXT("_"));
	g_TimeLoggerService->SetSeparator(TEXT("|"));
	g_TimeLoggerService->SetLoggerInfo(TEXT("GameServer"), m_pGameServiceOption->szServerName, m_pInitParameter->m_szLoggerPath);

	//��������
	if (g_TimeLoggerService->StartService() == false)
	{
		ASSERT(FALSE);

		return false;
	}

	return true;
}

//��Ϸ����
bool CAttemperEngineSink::OnEventGameConclude(ITableFrameItem * pITableFrame)
{
	return true;
}

//��Ϸ��ʼ
bool CAttemperEngineSink::OnEventGameStart(ITableFrameItem * pITableFrame)
{
	//����ģʽ
	if ((m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)!=0 && m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
	{
		//ͬ����Ϣ
		tagSameTableInfo * pSameTableInfo=m_DistributeManager.GetUserSameTableInfo();
		if (pSameTableInfo!=NULL)
		{
			for (WORD i=0; i<pITableFrame->GetChairCount(); i++)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=pITableFrame->GetTableUserItem(i);
				if (pIServerUserItem==NULL) continue;

				//�Ƴ�ͬ��
				m_DistributeManager.RemoveSameTableInfo(pIServerUserItem->GetUserID());

				//ͬ����Ϣ
				pSameTableInfo->wPlayerIDSet[pSameTableInfo->wPlayerCount++]=pIServerUserItem->GetUserID();

				//����ͬ��
				m_DistributeManager.SetUserSameTableInfo(pIServerUserItem->GetUserID(),pSameTableInfo);

				//�˳��ж�
				if (pSameTableInfo->wPlayerCount >= CountArray(pSameTableInfo->wPlayerIDSet)) break;
			}
		}
	}

	return true;
}

//�������
bool CAttemperEngineSink::InsertDistribute(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//״̬�ж�
	ASSERT (pIServerUserItem->GetUserStatus()<US_PLAYING);
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING) return false;

	//ģʽ�ж�
	ASSERT (m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE);
	if ((m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)==0) return false;

	//��ʱģʽ
	if (m_pGameServiceOption->cbDistributeMode==DISTRIBUTE_MODE_DELAYWAIT)
	{
		//��ȡ����
		CTableFrameItem * pTableFrame=m_TableFrameArray[0];		

		//����У��
		if (pTableFrame->EfficacyAddrRule(pIServerUserItem)==false) return true;
		if (pTableFrame->EfficacyTableRule(pIServerUserItem)==false) return true;
		if (pTableFrame->EfficacyScoreRule(pIServerUserItem)==false) return true;

		//����ڵ�
		if (m_DistributeManager.InsertDistributeNode(pIServerUserItem)==false)
		{
			return pIServerUserItem->IsRobotUser()?false:true; 
		}

		//�û�����
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			WORD wTableID=pIServerUserItem->GetTableID();
			m_TableFrameArray[wTableID]->PerformStandUpAction(pIServerUserItem);
		}	

		//����֪ͨ
		SendDataToUser(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_WAIT_DISTRIBUTE,NULL,0);
	}

	//����ģʽ
	if (m_pGameServiceOption->cbDistributeMode==DISTRIBUTE_MODE_IMMEDIATELY)
	{
		//��������
		SearchTableAndSitdown(SEARCH_MODE_MAX_PLAYER,pIServerUserItem);		
	}

	return true;
}

//�Ƴ�����
bool CAttemperEngineSink::RemoveDistribute(IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��ʱģʽ
	if (m_pGameServiceOption->cbDistributeMode==DISTRIBUTE_MODE_DELAYWAIT)
	{
		//�Ƴ��ڵ�
		m_DistributeManager.RemoveDistributeNode(pIServerUserItem);
	}

	return true;
}


//¼��ʼ
void CAttemperEngineSink::OnEventVideoStart(IGameVideoItem * pIGameVideoItem)
{
	//����У��
	ASSERT (pIGameVideoItem!=NULL);
	if (pIGameVideoItem==NULL) return;

	//У������
	ASSERT (pIGameVideoItem->GetTableID()!=INVALID_TABLE);
	if (pIGameVideoItem->GetTableID()==INVALID_TABLE) return;

	//��ȡ����
	CTableFrameItem * pTableFrame=m_TableFrameArray[pIGameVideoItem->GetTableID()];

	//�������ݰ�
	WORD wSendSize=0;
	BYTE cbBuffer[SOCKET_PACKET];

	//ö�����
	for (WORD wChairID=0; wChairID<pTableFrame->GetChairCount(); wChairID++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		//�����û�		
		pIGameVideoItem->InsertVideoUser(pIServerUserItem);
	}

	//ö�����
	for (WORD wChairID=0; wChairID<pTableFrame->GetChairCount(); wChairID++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		//�����û���
		wSendSize=ConstructEnterPacket(pIServerUserItem,cbBuffer,sizeof(cbBuffer),CLIENT_KIND_MOBILE);
		pIGameVideoItem->InsertVideoData(INVALID_CHAIR,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wSendSize);
	}
}

//¼��ֹͣ
void CAttemperEngineSink::OnEventVideoConclude(IGameVideoItem * pIGameVideoItem)
{
	//����У��
	ASSERT (pIGameVideoItem!=NULL);
	if (pIGameVideoItem==NULL) return;

	//У������
	ASSERT (pIGameVideoItem->GetTableID()!=INVALID_TABLE);
	if (pIGameVideoItem->GetTableID()==INVALID_TABLE) return;

	//��ȡ����
	CTableFrameItem * pTableFrame=m_TableFrameArray[pIGameVideoItem->GetTableID()];

	//ö�����
	for (WORD wChairID=0; wChairID<pTableFrame->GetChairCount(); wChairID++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;			

		//���������
		CMD_GR_UserScore UserScore;		
		ConstructScorePacket(pIServerUserItem,&UserScore,sizeof(UserScore));
		pIGameVideoItem->InsertVideoData(INVALID_CHAIR,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));		

		//����״̬��
		CMD_GR_UserStatus UserStatus;
		ConstructStatusPacket(pIServerUserItem,&UserStatus,sizeof(UserStatus));
		pIGameVideoItem->InsertVideoData(INVALID_CHAIR,MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));
	}	
}

//�洢¼��
bool CAttemperEngineSink::StoreVideoData(VOID * pData,WORD wDataSize)
{
	//��������
	if (m_bCacheVideo==true)
	{
		return m_VideoDataQueue.InsertData(0,pData,wDataSize);
	}

	//��������
	return m_pITCPSocketStore->SendData(MDM_SS_STORE_SERVICE,SUB_SS_C_STORE_VIDEO,pData,wDataSize,0);
}

//��ֵ����
bool CAttemperEngineSink::GetOptionValueArray(WORD wEmployScene, WORD wOptionIndex, COptionValueArray & OptionValueArray)
{
	//���ñ���
	OptionValueArray.RemoveAll();

	//��������
	tagBattleGameOption* pBattleGameOption = NULL;
	for (INT_PTR i = 0; i < m_BattleGameOptionArray.GetCount(); i++)
	{
		if (m_BattleGameOptionArray[i].BattleGameOption.cbEmployScene == wEmployScene)
		{
			pBattleGameOption = &m_BattleGameOptionArray[i].BattleGameOption;
			break;
		}
	}

	//У����
	if (pBattleGameOption == NULL) return false;

	//��ȡ����
	auto JsonValuePtr = CWHJson::getValue(string(CT2CA(pBattleGameOption->szOptionJson)));
	if (JsonValuePtr->getType() != eJsonTypeObj) return false;

	//ת������
	auto JsonValueObjPtr = CJsonValueObjPtr::dynamicCast(JsonValuePtr);

	//����У��
	if (!JsonValueObjPtr->value["Count"].get()) return false;

	//��ȡ����
	WORD wCount = 0;
	CWHJsonInput::readJson(wCount, JsonValueObjPtr->value["Count"], false);

	//����У��
	if (wOptionIndex >= wCount) return false;

	//�������
	char szType[32];
	_snprintf_s(szType, 32, "type%d", wOptionIndex + 1);

	//����У��
	auto pDataPtr = JsonValueObjPtr->value["Data"].get();
	if (pDataPtr != NULL && pDataPtr->getType()== eJsonTypeObj)
	{
		auto pDataObjPtr = CJsonValueObjPtr::dynamicCast(pDataPtr);		
		auto pTypePtr = pDataObjPtr->value[szType].get();
		if (pTypePtr != NULL && pTypePtr->getType() == eJsonTypeObj)
		{
			auto pTypeObjPtr = CJsonValueObjPtr::dynamicCast(pTypePtr);
			auto pContentPtr = pTypeObjPtr->value["Content"].get();
			if (pContentPtr != NULL && pContentPtr->getType() == eJsonTypeArray)
			{
				auto pContentArrayPtr = CJsonValueArrayPtr::dynamicCast(pContentPtr);
				for (auto pContentItemPtr : pContentArrayPtr->value)
				{
					if (pContentItemPtr->getType() != eJsonTypeObj) continue;

					auto pContentItemObjPtr = CJsonValueObjPtr::dynamicCast(pContentItemPtr);
					auto pItemsPtr = pContentItemObjPtr->value["Items"].get();
					if (pItemsPtr && pItemsPtr->getType() == eJsonTypeArray)
					{
						auto pItemsArrayPtr = CJsonValueArrayPtr::dynamicCast(pItemsPtr);
						for (auto pItemPtr : pItemsArrayPtr->value)
						{
							if (pItemPtr->getType() != eJsonTypeObj) continue;
							
							auto pItemObjPtr = CJsonValueObjPtr::dynamicCast(pItemPtr);
							if (pItemObjPtr != NULL)
							{
								INT nValue = 0;
								CWHJsonInput::readJson(nValue, pItemObjPtr->value["Val"], false);
								OptionValueArray.Add(nValue);
							}							
						}
					}					
				}
			}
		}
	}

	return true;
}

//��������
INT CAttemperEngineSink::GetTableScoreAreaIndex(CTableFrameItem * pTableFrame)
{
	//����У��
	ASSERT (pTableFrame!=NULL);
	if (pTableFrame==NULL) return 0;

	//��������
	INT nIndex=0;
	INT nAreaIndex[15];
	WORD wSitUserCount=pTableFrame->GetSitUserCount();

	//���ñ���
	ZeroMemory(nAreaIndex,sizeof(nAreaIndex));

	//��ȡ����
	for (WORD i=0;i<pTableFrame->GetChairCount();i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//��������
		nIndex=m_ScoreManager.GetScoreAreaIndex(pIServerUserItem->GetUserFreeScore());
		if (wSitUserCount<=2) return nIndex;

		//���ñ���
		nAreaIndex[nIndex]++;
	}

	//���ñ���
	nIndex=0;
	INT nIndexCount=nAreaIndex[0];

	//�Ա���Ŀ	
	for (INT j=1; j<CountArray(nAreaIndex); j++)
	{
		if (nAreaIndex[j]>nIndexCount)
		{
			nIndex=j;
			nIndexCount=nAreaIndex[j];
		}
	}

	return nIndex;
}

//���ͳɼ�
bool CAttemperEngineSink::SendUserScore(IServerUserItem *pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�û���Ϣ
	auto pUserInfo=pIServerUserItem->GetUserInfo();

	//��������
	CMD_GR_UserScore UserScore;	
	ConstructScorePacket(pIServerUserItem,&UserScore,sizeof(UserScore));

	//��������
	SendBatchData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));

	//��������
	SendDataToTable(pIServerUserItem->GetTableID(), CLIENT_KIND_MOBILE, MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));

	return true;
}

//�ܾ�����
bool CAttemperEngineSink::OnTCPNetworkSubUserRepulseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_GR_UserRepulseSit));
	if (wDataSize!=sizeof(CMD_GR_UserRepulseSit)) return false;

	//��Ϣ����
	CMD_GR_UserRepulseSit * pUserRepulseSit=(CMD_GR_UserRepulseSit *)pData;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);
	if (pIServerUserItem==NULL) return false;

	//��ȡ����
	CTableFrameItem * pTableFrame=m_TableFrameArray[pUserRepulseSit->wTableID];
	//if (pTableFrame->IsGameStarted()==true) return true;

	//��ȡ�û�
	IServerUserItem * pRepulseIServerUserItem = pTableFrame->GetTableUserItem(pUserRepulseSit->wChairID);
	if (pRepulseIServerUserItem==NULL) return true;
	if(pRepulseIServerUserItem->GetUserID() != pUserRepulseSit->dwRepulseUserID)return true;

	//������Ϣ
	SendRoomMessage(pRepulseIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0048, TEXT("��������������˲�����ͬ����Ϸ��")),SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

	//�������
	pTableFrame->PerformStandUpAction(pRepulseIServerUserItem);

	return true;
}

//�����û���Ϣ
bool CAttemperEngineSink::OnTCPNetworkSubUserRequestInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_GR_UserRequestInfo));
	if (wDataSize!=sizeof(CMD_GR_UserRequestInfo)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	auto pUserRequestInfo=(CMD_GR_UserRequestInfo*)pData;

	//������Ϣ
	SendTableUserToUser(pIServerUserItem, CLIENT_KIND_MOBILE, pUserRequestInfo->dwUserID);

	return true;
}

//�������λ��
bool CAttemperEngineSink::OnTCPNetworkSubUserSwitchTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�û�״̬
	if (pIServerUserItem->GetUserStatus()==US_PLAYING)
	{
		//ʧ��
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0010, TEXT("��������Ϸ�У���ʱ�����뿪�����Ƚ�����ǰ��Ϸ��")), REQUEST_FAILURE_NORMAL);
		return true;
	}

	//����ģʽ
	if (m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)
	{
		return InsertDistribute(pIServerUserItem);
	}

	//����ģʽ
	BYTE cbSearchMode=SEARCH_MODE_MAX_PLAYER;
	if (CServerRule::IsMatchUserByScore(m_pGameServiceOption->dwServerRule))
	{
		//���·���
		m_ScoreManager.UpdateScoreData();

		cbSearchMode|=SEARCH_MASK_MATCH_SCORE;		
	}

	//������Ϣ
	WORD wLastTableID = INVALID_WORD;

	//��������
	if (pIServerUserItem->GetTableID() != INVALID_TABLE && pIServerUserItem->GetChairID() != INVALID_CHAIR)
	{
		wLastTableID = pIServerUserItem->GetTableID();
		CTableFrameItem * pTableFramePrep = m_TableFrameArray[pIServerUserItem->GetTableID()];
		pTableFramePrep->PerformStandUpAction(pIServerUserItem);
	}

	//���Ҳ�����
	return SearchTableAndSitdown(cbSearchMode, pIServerUserItem, wLastTableID);
}

//ͬ����Ϣ
bool CAttemperEngineSink::OnTCPNetworkSubUserDeskmateInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT (wDataSize==sizeof(CMD_GR_UserDeskmateInfo));
	if (wDataSize!=sizeof(CMD_GR_UserDeskmateInfo)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	auto pUserDeskmateInfo = (CMD_GR_UserDeskmateInfo*)pData;

	//У�����
	if (pUserDeskmateInfo->wTableID==INVALID_TABLE) return true;
	if (pUserDeskmateInfo->wTableID>=m_pGameServiceOption->wTableCount)return true;

	//������Ϣ
	WORD wChairCout=m_TableFrameArray[pUserDeskmateInfo->wTableID]->GetChairCount();
	for (WORD wIndex=0; wIndex<wChairCout; wIndex++)
	{
		//��ȡ�û�
		if (pUserDeskmateInfo->wChairID!=INVALID_CHAIR && wIndex!= pUserDeskmateInfo->wChairID)continue;
		IServerUserItem * pTagerIServerUserItem=m_TableFrameArray[pUserDeskmateInfo->wTableID]->GetTableUserItem(wIndex);
		if (pTagerIServerUserItem==NULL)continue;

		//��������
		BYTE cbBuffer[SOCKET_BUFFER]={0};
		WORD wDataSize=ConstructEnterPacket(pTagerIServerUserItem, cbBuffer, sizeof(cbBuffer), CLIENT_KIND_MOBILE);

		//������Ϣ
		SendDataToUser(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer, wDataSize);
	}

	return true;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkSubUserSendGift(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
    //Ч�����
    ASSERT(wDataSize == sizeof(CMD_GR_SendGift));
    if (wDataSize != sizeof(CMD_GR_SendGift)) return false;

    //��ȡ�û�
    WORD wBindIndex = LOWORD(dwSocketID);
    IServerUserItem* pIServerUserItem = GetServerUserItem(wBindIndex, dwTokenID);

    //�û�Ч��
    ASSERT(pIServerUserItem != NULL);
    if (pIServerUserItem == NULL) return false;

    //��������
    auto pSendGift = (CMD_GR_SendGift*)pData;

    //Ŀ���û�
    IServerUserItem* pITargerUserItem = m_ServerUserManager.SearchUserItem(pSendGift->dwUserID);
    if (pITargerUserItem == NULL) return true;

    //��������
	DBR_GR_SendGift SendGift;
    ZeroMemory(&SendGift, sizeof(SendGift));
	SendGift.dwUserID = pIServerUserItem->GetUserID();
	SendGift.dwTargetUserID = pSendGift->dwUserID;
	SendGift.dwGoodsID = pSendGift->dwGoodsID;
	SendGift.dwGoodsCount = pSendGift->dwGoodsCount;
	SendGift.dwClientIP = pIServerUserItem->GetClientIP();

	//��������
	auto pTableFrame = (CTableFrameItem*)m_TableFrameArray[pIServerUserItem->GetTableID()];
	auto pGameServiceOption = pTableFrame->GetGameServiceOption();
	SendGift.lMinTableScore = max(pGameServiceOption->lLessScore, pGameServiceOption->lMinEnterScore);
	SendGift.lCellScore = pGameServiceOption->lCellScore;
	SendGift.cbIsGoldRoom = (pGameServiceOption->wServerType&GAME_GENRE_GOLD)?1:0;

    //��������
    m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SEND_GIFT, SendGift.dwUserID, dwSocketID, dwTokenID, &SendGift, sizeof(SendGift));

    return true;
}

//д�����
bool CAttemperEngineSink::OnDBWriteUserProp(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(LONG));
	if(wDataSize!=sizeof(LONG)) return false;

	//��ȡ�û�
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//�û�Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//��������
	LONG lReturnValue=*(LONG*)pData;

	return true;
}

//���ͳɹ�
bool CAttemperEngineSink::OnDBSendGiftSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
    //Ч�����
    ASSERT(wDataSize >= sizeof(DBO_GR_SendGiftSuccess));
    if (wDataSize < sizeof(DBO_GR_SendGiftSuccess)) return false;

    //��ȡ�û�
    WORD wBindIndex = LOWORD(dwContextID);
    IServerUserItem* pIServerUserItem = GetServerUserItem(wBindIndex, dwTokenID);

	//��ȡ����
	DBO_GR_SendGiftSuccess* pSendGiftSuccess = (DBO_GR_SendGiftSuccess*)pData;

    //�û�Ч��
    ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem != NULL)
	{
		//ͬ���Ƹ�
		pIServerUserItem->WriteUserWealth(pSendGiftSuccess->wMoneyID, -pSendGiftSuccess->lGiftPrice, GOODS_REASON_SYNC);

		//���ͳɹ�
		CMD_GR_SendGiftSuccess SendGiftSuccess;
		ZeroMemory(&SendGiftSuccess, sizeof(SendGiftSuccess));
		SendGiftSuccess.dwLoveLiness = pSendGiftSuccess->dwLoveLiness;

		//������Ϣ
		SendDataToUser(pIServerUserItem, MDM_GR_USER, SUB_GR_SEND_GIFT_SUCCESS, &SendGiftSuccess, sizeof(SendGiftSuccess));

		//����ֵ���
		CMD_CM_LoveLinessUpdate LoveLinessUpdate;
		ZeroMemory(&LoveLinessUpdate, sizeof(LoveLinessUpdate));
		LoveLinessUpdate.dwLoveLiness = pSendGiftSuccess->dwMyLoveLiness;

		//��������
		SendDataToUser(pIServerUserItem, MDM_CM_SYSTEM, SUB_CM_LOVELINESS_UPDATE, &LoveLinessUpdate, sizeof(LoveLinessUpdate));

		//֪ͨ�������
		CMD_GR_RecvGift RecvGift;
		ZeroMemory(&RecvGift, sizeof(RecvGift));
		RecvGift.dwUserID = pIServerUserItem->GetUserID();
		RecvGift.dwTargetUserID = pSendGiftSuccess->dwTargetUserID;
		RecvGift.dwGoodsID = pSendGiftSuccess->dwGoodsID;
		RecvGift.dwGoodsCount = pSendGiftSuccess->dwGoodsCount;
		RecvGift.dwLoveLiness = pSendGiftSuccess->dwLoveLiness;

		//��������
		SendDataToTable(pIServerUserItem->GetTableID(), 0xFF, MDM_GR_USER, SUB_GR_RECV_GIFT, &RecvGift, sizeof(RecvGift));

		//Ŀ���û�
		auto pITargerUserItem = m_ServerUserManager.SearchUserItem(pSendGiftSuccess->dwTargetUserID);
		if (pITargerUserItem == NULL) return true;

		//�Է�����ֵ
		ZeroMemory(&LoveLinessUpdate, sizeof(LoveLinessUpdate));
		LoveLinessUpdate.dwLoveLiness = pSendGiftSuccess->dwTargetLoveLiness;

		//��������
		SendDataToUser(pITargerUserItem, MDM_CM_SYSTEM, SUB_CM_LOVELINESS_UPDATE, &LoveLinessUpdate, sizeof(LoveLinessUpdate));
	}

	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBSendGiftFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
    //Ч�����
    ASSERT(wDataSize <= sizeof(DBO_GR_SendGiftFailure));
    if (wDataSize > sizeof(DBO_GR_SendGiftFailure)) return false;

    //��ȡ����
	DBO_GR_SendGiftFailure* pSendGiftFailure = (DBO_GR_SendGiftFailure*)pData;

	//����ʧ��
	SendOperateFailure(dwContextID, dwTokenID, SUB_GR_SEND_GIFT, pSendGiftFailure->lErrorCode, pSendGiftFailure->szDescribeString);

	return true;
}

//��������
bool CAttemperEngineSink::OnDBUpdateLoveLiness(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
    //��ȡ�û�
    WORD wBindIndex = LOWORD(dwContextID);
    IServerUserItem* pIServerUserItem = GetServerUserItem(wBindIndex, dwTokenID);

    //�û�Ч��
    ASSERT(pIServerUserItem != NULL);
    if (pIServerUserItem == NULL) return false;

    //��ȡ����
    auto pDBOLoveLinessUpdate = (DBO_GR_UpdateLoveLiness*)pData;

	//���嶨��
	BYTE cbBuffer[SOCKET_PACKET] = { 0 };

    //�������
    auto pLoveLinessUpdate = (CMD_CM_LoveLinessUpdate*)cbBuffer;
    ZeroMemory(pLoveLinessUpdate, sizeof(CMD_CM_LoveLinessUpdate));

    //���ñ���
    pLoveLinessUpdate->dwLoveLiness = pDBOLoveLinessUpdate->dwLoveLiness;

    //��������
	SendDataToUser(pIServerUserItem, MDM_CM_SYSTEM, SUB_CM_LOVELINESS_UPDATE, pLoveLinessUpdate, sizeof(CMD_CM_LoveLinessUpdate));

    return true;
}

//����ʧ��
VOID CAttemperEngineSink::SendOperateFailure(DWORD dwSocketID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe)
{
	//���嶨��
	BYTE cbBuffer[SOCKET_PACKET] = { 0 };

    //��������
    CMD_CM_OperateFailure* pRequestFailure = (CMD_CM_OperateFailure*)cbBuffer;
    CSendPacketHelper SendPacket(cbBuffer + sizeof(CMD_CM_OperateFailure), sizeof(cbBuffer) - sizeof(CMD_CM_OperateFailure));

    //��ȡ�û�
    WORD wBindIndex = LOWORD(dwSocketID);
    IServerUserItem* pIServerUserItem = GetServerUserItem(wBindIndex, dwTokenID);

    //�û�Ч��
    ASSERT(pIServerUserItem != NULL);
    if (pIServerUserItem == NULL) return;

    //���ñ���
    pRequestFailure->lErrorCode = lErrorCode;
    pRequestFailure->wRequestCmdID = wRequestCmdID;
    
	//������Ϣ
	if (pszDescribe != NULL)  SendPacket.AddPacket(pszDescribe, DTP_CM_DESCRIBE);

    //�������
    WORD wDataSize = sizeof(CMD_CM_OperateFailure) + SendPacket.GetDataSize();
	SendDataToUser(pIServerUserItem, MDM_CM_SYSTEM, SUB_CM_OPERATE_FAILURE, pRequestFailure, wDataSize);

    return;
}

//�����ɹ�
VOID CAttemperEngineSink::SendOperateSuccess(DWORD dwSocketID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe, LPCTSTR pszGoodsList)
{
	//���嶨��
	BYTE cbBuffer[SOCKET_PACKET] = { 0 };

    //��������
    CMD_CM_OperateSuccess* pRequestSuccess = (CMD_CM_OperateSuccess*)cbBuffer;
    CSendPacketHelper SendPacket(cbBuffer + sizeof(CMD_CM_OperateSuccess), sizeof(cbBuffer) - sizeof(CMD_CM_OperateSuccess));

    //��ȡ�û�
    WORD wBindIndex = LOWORD(dwSocketID);
    IServerUserItem* pIServerUserItem = GetServerUserItem(wBindIndex, dwTokenID);

    //�û�Ч��
    ASSERT(pIServerUserItem != NULL);
    if (pIServerUserItem == NULL) return;

    //���ñ���
    pRequestSuccess->lErrorCode = lErrorCode;
    pRequestSuccess->wRequestCmdID = wRequestCmdID;    

    //������Ϣ
	if (pszDescribe != NULL)  SendPacket.AddPacket(pszDescribe, DTP_CM_DESCRIBE);
    if (pszGoodsList != NULL)  SendPacket.AddPacket(pszGoodsList, DTP_CM_GOODSLIST);

    //�������
    WORD wDataSize = sizeof(CMD_CM_OperateSuccess) + SendPacket.GetDataSize();
	SendDataToUser(pIServerUserItem, MDM_CM_SYSTEM, SUB_CM_OPERATE_SUCCESS, pRequestSuccess, wDataSize);
}

//�汾��Ϣ
bool CAttemperEngineSink::OnDBGameVersionInfo(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��ȡ����
	ASSERT (wDataSize%sizeof(tagModuleVersion)==0);
	if (wDataSize%sizeof(tagModuleVersion)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(tagModuleVersion);
	tagModuleVersion * pModuleVersion=(tagModuleVersion *)pData;

	//�Ƴ��汾
	m_ModuleVersionActive.RemoveAll();

	//��������
	for (int i=0; i<wItemCount; i++)
	{
		m_ModuleVersionActive.Add(*pModuleVersion++);
	}

	return true;
}

//��Ʒ����
bool CAttemperEngineSink::OnDBUserGoodsUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//У������
	ASSERT(wDataSize <= sizeof(DBO_GR_UserGoodsUpdate));
	if (wDataSize > sizeof(DBO_GR_UserGoodsUpdate)) return false;

	return true;
}

//�޸�����
bool CAttemperEngineSink::ModifyGameServer()
{
	//������Ϣ
	tagCreateServerInfo CreateServerInfo;
	ZeroMemory(&CreateServerInfo,sizeof(CreateServerInfo));

	//ģ������
	CreateServerInfo.wModuleID=m_pGameServiceAttrib->wKindID;
	CreateServerInfo.wServerID=m_pGameServiceOption->wServerID;
	CreateServerInfo.wDBListID = m_pGameServiceOption->wDBListID;
	
	//�ҽ�����
	CreateServerInfo.wKindID=m_pGameServiceOption->wKindID;
	CreateServerInfo.wNodeID=m_pGameServiceOption->wNodeID;
	CreateServerInfo.wSortID=m_pGameServiceOption->wSortID;
	CreateServerInfo.wVisibleMask=m_pGameServiceOption->wVisibleMask;

	//��������
	CreateServerInfo.lRevenue=m_pGameServiceOption->lRevenue;
	CreateServerInfo.lCellScore=m_pGameServiceOption->lCellScore;
	CreateServerInfo.lLessScore=m_pGameServiceOption->lLessScore;
	CreateServerInfo.lRestrictScore=m_pGameServiceOption->lRestrictScore;
	CreateServerInfo.lMinEnterScore=m_pGameServiceOption->lMinEnterScore;
	CreateServerInfo.lMaxEnterScore=m_pGameServiceOption->lMaxEnterScore;
	CreateServerInfo.lMinEnterMember=m_pGameServiceOption->lMinEnterMember;
	CreateServerInfo.lMaxEnterMember=m_pGameServiceOption->lMaxEnterMember;

	//��������
	StringCchCopy(CreateServerInfo.szServiceAddr,CountArray(CreateServerInfo.szServiceAddr), m_pGameServiceOption->szServiceAddr);
	StringCchCopy(CreateServerInfo.szServiceDomain,CountArray(CreateServerInfo.szServiceDomain), m_pGameServiceOption->szServiceDomain);

	//Լս����
	CreateServerInfo.dwBattleRule=m_pGameServiceOption->dwBattleRule;

	//�������
	CreateServerInfo.cbDistributeMode=m_pGameServiceOption->cbDistributeMode;
	CreateServerInfo.cbDistributeRule=m_pGameServiceOption->cbDistributeRule;
	CreateServerInfo.wMinPlayerCount=m_pGameServiceOption->wMinPlayerCount;
	CreateServerInfo.wMinDistributeUser=m_pGameServiceOption->wMinDistributeUser;	
	CreateServerInfo.wMinPartakeGameUser=m_pGameServiceOption->wMinPartakeGameUser;
	CreateServerInfo.wMaxPartakeGameUser=m_pGameServiceOption->wMaxPartakeGameUser;
	CreateServerInfo.wDistributeTimeSpace=m_pGameServiceOption->wDistributeTimeSpace;

	//������Ϣ
	CreateServerInfo.wMaxPlayer=m_pGameServiceOption->wMaxPlayer;
	CreateServerInfo.wTableCount=m_pGameServiceOption->wTableCount;
	CreateServerInfo.wServerType=m_pGameServiceOption->wServerType;
	CreateServerInfo.wServerPort=m_pGameServiceOption->wServerPort;
	CreateServerInfo.dwServerRule=m_pGameServiceOption->dwServerRule;
	CreateServerInfo.wServerLevel=m_pGameServiceOption->wServerLevel;
	CreateServerInfo.cbMatchType=m_pGameServiceOption->cbMatchType;
	CreateServerInfo.dwAttachUserRight=m_pGameServiceOption->dwAttachUserRight;	
	StringCchCopy(CreateServerInfo.szServerName,CountArray(CreateServerInfo.szServerName), m_pGameServiceOption->szServerName);

	//��չ����
	CWHService::GetMachineID(CreateServerInfo.szServiceMachine);
	CopyMemory(CreateServerInfo.cbCustomRule,m_pGameServiceOption->cbCustomRule,sizeof(CreateServerInfo.cbCustomRule));

	//��������
	tagGameServerInfo GameServerResult;
	CServerInfoManager ServerInfoManager;

	//�޸ķ���
	if (CreateServerInfo.wServerID!=0)
	{
		//�޸ķ���
		if (ServerInfoManager.ModifyGameServer(&CreateServerInfo,GameServerResult)==false) return true;

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("��Ϸ���������޸ĳɹ�"),TraceLevel_Normal);
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////

