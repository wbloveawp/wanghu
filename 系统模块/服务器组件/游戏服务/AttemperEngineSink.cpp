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

//系统时间
#define IDI_CONNECT_STORE			(IDI_ATTEMPER_SINK_START+0)			//连接时间
#define IDI_CONNECT_CENTER			(IDI_ATTEMPER_SINK_START+1)			//连接时间
#define IDI_CONNECT_SERVICE			(IDI_ATTEMPER_SINK_START+2)			//连接时间
#define IDI_REPORT_SERVER_INFO		(IDI_ATTEMPER_SINK_START+3)			//房间信息

//辅助时间
#define IDI_LOAD_ROBOT_INFO			(IDI_ATTEMPER_SINK_START+10)		//用户信息
#define IDI_REGULAR_TIMER			(IDI_ATTEMPER_SINK_START+12)		//常规定时
#define IDI_UPDATE_ROBOTCOUNT		(IDI_ATTEMPER_SINK_START+15)		//更新机器
#define IDI_DISTRIBUTE_USER			(IDI_ATTEMPER_SINK_START+16)		//分组用户
#define IDI_DISTRIBUTE_ROBOT		(IDI_ATTEMPER_SINK_START+17)		//分配机器

//////////////////////////////////////////////////////////////////////////
//宏定义

#define TIME_USER_IN_TABLE			(60*3)								//玩家最长锁桌时间		
#define MIN_MESSAGEG_INTERVAL		1									//系统消息最小时间间隔（秒）

//间隔时间
#define MIN_INTERVAL_TIME			10									//间隔时间
#define MAX_INTERVAL_TIME			1*60								//临时标识

//查找模式
#define SEARCH_MODE_NO_PLAYER		0									//没有玩家
#define SEARCH_MODE_MIN_PLAYER		0x01								//最少玩家
#define SEARCH_MODE_MAX_PLAYER		0x02								//最多玩家

//查找掩码
#define SEARCH_MASK_MATCH_SCORE		0x10								//匹配分数

//////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//状态变量
	m_bCollectUser=false;
	m_bNeedStore=true;
	m_bNeekCenter = true;
	m_bNeedService=true;
	m_bWillConcludeService=false;

	//时间变量
	m_dwRegularTimes=0;
	m_dwLockTableTime=TIME_USER_IN_TABLE;

	//控制变量
	m_dwIntervalTime=0;
	m_dwLastDisposeTime=0;

	//缓存变量
	m_bCacheVideo=true;
	m_VideoDataQueue.RemoveData(false);

	//配置变量	
	ZeroMemory(&m_StoreServer,sizeof(m_StoreServer));
	ZeroMemory(&m_ServiceItem,sizeof(m_ServiceItem));

	//绑定信息
	m_pNormalParameter=NULL;
	m_pRobotParameter=NULL;

	//系统配置
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//内核接口
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIGameServiceManager=NULL;
	m_pIGameServiceSustomTime=NULL;	

	//网络接口
	m_pITCPSocketStore=NULL;
	m_pITCPSocketCenter=NULL;
	m_pITCPSocketService=NULL;

	//数据库接口
	m_pIDataBaseEngine=NULL;

	//设置接口
	m_pIGameVideoManager=NULL;
	m_pIExtendServiceManager=NULL;

	//配置数据
	ZeroMemory(&CMD_ConfigColumn,sizeof(CMD_ConfigColumn));
	ZeroMemory(&CMD_ConfigServer,sizeof(CMD_ConfigServer));	


	m_FreezeScoreMap.InitHashTable(PRIME_SERVER_USER);	
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{
}

//接口查询
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

//启动事件
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//绑定信息
	m_pRobotParameter=new tagBindParameter[MAX_ROBOT];
	ZeroMemory(m_pRobotParameter,sizeof(tagBindParameter)*MAX_ROBOT);

	//绑定信息
	m_pNormalParameter=new tagBindParameter[m_pGameServiceOption->wMaxPlayer];
	ZeroMemory(m_pNormalParameter,sizeof(tagBindParameter)*m_pGameServiceOption->wMaxPlayer);

	//用户接口
	m_ScoreManager.m_pIServerUserManager = QUERY_OBJECT_INTERFACE(m_ServerUserManager, IServerUserManager);

	//配置房间
	if (InitServerConfig() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//配置桌子
	if (InitTableFrame() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//配置比赛
	if (InitExtendService() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//配置机器
	if (InitRobotService() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//配置录像
	if (InitVideoService() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//配置日志
	if (InitLoggerService() == false)
	{
		ASSERT(FALSE);
		return false;
	}	

	//常规定时器
	m_pITimerEngine->SetTimer(IDI_REGULAR_TIMER,1000L,TIMES_INFINITY,0);	

	//设置参数
	m_ServerUserManager.SetControlParamter(&m_ControlParameter);
	m_ServerUserManager.SetServiceParameter(m_pGameServiceOption);

	//设置接口
	if (m_ServerUserManager.SetServerUserItemSink(QUERY_ME_INTERFACE(IServerUserItemSink))==false) return false;	

	//分配机器
	if (g_GlobalUnits.IsAllowRobotAttend() || g_GlobalUnits.IsAllowRobotSimulate())
	{
		m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ROBOT,TIME_DISTRIBUTE_ROBOT*1000L*2,(DWORD)(-1),NULL);	
	}

	//分组模式
	if (m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE && m_pGameServiceOption->cbDistributeMode==DISTRIBUTE_MODE_DELAYWAIT)	
	{
		//设置规则
		m_DistributeManager.SetDistributeRule(m_pGameServiceOption->cbDistributeRule);
		m_DistributeManager.SetMinPlayerCount(m_pGameServiceOption->wMinPlayerCount);

		//设置定时器
		m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_USER,m_pGameServiceOption->wDistributeTimeSpace*1000L,(DWORD)(-1),NULL);			
	}

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//状态变量
	m_bCollectUser=false;
	m_bNeedStore=true;
	m_bNeekCenter=true;
	m_bNeedService=true;
	m_bWillConcludeService=false;

	//服务信息
	ZeroMemory(&m_StoreServer,sizeof(m_StoreServer));
	ZeroMemory(&m_ServiceItem,sizeof(m_ServiceItem));

	//时间变量
	m_dwRegularTimes=0;
	m_dwLockTableTime=TIME_USER_IN_TABLE;

	//缓存变量
	m_bCacheVideo=true;
	m_VideoDataQueue.RemoveData(false);

	//绑定信息
	SafeDeleteArray(m_pNormalParameter);
	SafeDeleteArray(m_pRobotParameter);

	//配置数据
	ZeroMemory(&CMD_ConfigColumn,sizeof(CMD_ConfigColumn));
	ZeroMemory(&CMD_ConfigServer,sizeof(CMD_ConfigServer));

	//系统参数
	ZeroMemory(&m_SystemParameter,sizeof(m_SystemParameter));

	//组件变量
	m_GlobalItemManager.ResetData();
	m_ServerUserManager.DeleteUserItem();	

	//停止服务
	m_RobotUserManager.ConcludeService();

	//停止服务
	g_TimeLoggerService->ConcludeService();

	//停止服务
	if(m_pIExtendServiceManager!=NULL) m_pIExtendServiceManager->ConcludeService();	

	//桌子组件
	for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
	{
		CTableFrameItem * pTableFrame=m_TableFrameArray[i];
		SafeDelete(pTableFrame);
	}

	//清理数组
	m_TableFrameArray.RemoveAll();
	m_LockedTableTimes.RemoveAll();
	m_DistributeManager.RemoveAll();

	//重置配置
	g_GameGoodsManager->ResetData();
	g_GameLevelManager->ResetData();
	g_GlobalOptionManager->ResetData();

	//内核接口
	m_pITimerEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pIGameServiceManager=NULL;
	m_pIExtendServiceManager=NULL;

	//网络接口
	m_pITCPSocketStore=NULL;
	m_pITCPSocketCenter=NULL;
	m_pITCPSocketService=NULL;

	//数据库接口
	m_pIDataBaseEngine=NULL;
	
	return true;
}


//控制事件
bool CAttemperEngineSink::OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	switch(dwIdentifier)
	{
	case CT_CONNECT_STORE:			//连接存储
	{
		//发起连接
		m_pITCPSocketStore->Connect(m_StoreServer.szServiceDomain,m_StoreServer.wServicePort);   

		//提示消息
 		CTraceService::TraceString(TEXT("正在尝试连接存储服务器..."),TraceLevel_Normal);     

		return true;
	}
	case CT_CONNECT_SERVICE:		//连接比赛
	{
		//发起连接
		m_pITCPSocketService->Connect(m_ServiceItem.szServiceDomain, m_ServiceItem.wServicePort);

		//提示消息
		CTraceService::TraceString(TEXT("正在尝试连接%s..."),TraceLevel_Normal, m_ServiceItem.szServiceName);

		return true;
	}	
	case CT_CONNECT_CENTER:			//连接中心
	{
		//变量定义
		DWORD dwCenterServer=0L;

		//设置地址
		dwCenterServer=m_pInitParameter->m_CenterAddress.dwServiceAddr;

		//发起连接
		m_pITCPSocketCenter->Connect(htonl(dwCenterServer), m_pInitParameter->m_wCenterPort);

		//提示消息
		CTraceService::TraceString(TEXT("正在尝试连接中心服务器..."),TraceLevel_Normal);

		return true;
	}
	case CT_DEBUG_SERVICE:			//调试服务
	{
		return true;
	}
	case CT_LOAD_SERVICE_CONFIG:	//加载配置		
	{	
		//配置掩码
		DBR_GR_LoadConfig LoadConfig;
		ZeroMemory(&LoadConfig,sizeof(LoadConfig));

		//设置变量
		LoadConfig.bInitService = true;
		LoadConfig.dwConfigMask = 0xFFFFFFFF;

		//约战房间
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) == 0)
		{
			LoadConfig.dwConfigMask &= ~CONFIG_MASK_BATTLE_OPTION;
		}

		//加载配置
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_CONFIG,0,0,0, &LoadConfig,sizeof(LoadConfig));
			
		//加载机器人
		if (g_GlobalUnits.IsAllowRobotAttend() || g_GlobalUnits.IsAllowRobotSimulate())
		{
			m_pITimerEngine->SetTimer(IDI_LOAD_ROBOT_INFO,TIME_LOAD_ROBOT_INFO*1000L,-1,NULL);
		}

		//提示消息
		CTraceService::TraceString(TEXT("正在加载房间配置..."),TraceLevel_Normal);

		return true;
	}
	}

	return false;
}

//调度事件
bool CAttemperEngineSink::OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//时间事件
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//引擎时间
	if ((dwTimerID>=IDI_ATTEMPER_SINK_START)&&(dwTimerID<=IDI_ATTEMPER_SINK_RANGE))
	{
		//引擎时间
		switch (dwTimerID)
		{		
		case IDI_CONNECT_STORE:			//连接存储
		{
			//发起连接
			m_pITCPSocketStore->Connect(m_StoreServer.szServiceDomain,m_StoreServer.wServicePort);

			//提示消息
			CTraceService::TraceString(TEXT("正在尝试连接%s..."),TraceLevel_Normal, m_StoreServer.szServiceName);

			return true;
		}
		case IDI_CONNECT_SERVICE:		//连接服务
		{
			//发起连接
			m_pITCPSocketService->Connect(m_ServiceItem.szServiceDomain, m_ServiceItem.wServicePort);

			//提示消息
			CTraceService::TraceString(TEXT("正在尝试连接%s..."),TraceLevel_Normal,m_ServiceItem.szServiceName);

			return true;
		}		
		case IDI_CONNECT_CENTER:		//连接中心
		{
			//变量定义
			DWORD dwCenterServer=0L;
			tagAddressInfo * pAddressInfo=&m_pInitParameter->m_CenterAddress;

			//设置地址
			dwCenterServer=pAddressInfo->dwServiceAddr;

			//发起连接
			m_pITCPSocketCenter->Connect(htonl(dwCenterServer), m_pInitParameter->m_wCenterPort);

			//提示消息
			CTraceService::TraceString(TEXT("正在尝试连接中心服务器..."),TraceLevel_Normal);

			return true;
		}
		case IDI_REPORT_SERVER_INFO:	//房间信息
		{
			//变量定义
			CMD_CS_C_ServiceOnline ServiceOnLine;
			ZeroMemory(&ServiceOnLine,sizeof(ServiceOnLine));

			//设置变量
			ServiceOnLine.wOnlineCount=m_ServerUserManager.GetUserItemCount();

			//发送信息
			ASSERT(m_pITCPSocketCenter!=NULL);
			m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE,SUB_CS_C_SERVICE_ONLINE,&ServiceOnLine,sizeof(ServiceOnLine),0);

			return true;
		}
		case IDI_LOAD_ROBOT_INFO:		//加载机器
		{
			//变量定义
			DWORD dwBatchID=0;
			DWORD dwRobotCount=0;

			//加载判断
			if(m_RobotUserManager.GetRobotLoadInfo(dwBatchID,dwRobotCount))
			{
				//构造结构
				DBR_GR_LoadRobotUser LoadRobotUser;
				LoadRobotUser.dwBatchID=dwBatchID;
				LoadRobotUser.dwRobotCount=dwRobotCount;

				CTraceService::TraceString(TEXT("请求加载机器人, BatchID=%d,RobotCount=%d"),TraceLevel_Info, dwBatchID, dwRobotCount);

				//加载机器
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_ROBOTS,0L,0L,0L,&LoadRobotUser,sizeof(LoadRobotUser));
			}

			return true;
		}
		case IDI_DISTRIBUTE_USER:		//分组用户
		{
			//执行分组
			PerformDistribute();

			return true;
		}

		case IDI_DISTRIBUTE_ROBOT:	//分配用户
		{
			//变量定义
			bool bDisposeSitDown=true;

			//设置定时器
			DWORD dwElpase=TIME_DISTRIBUTE_ROBOT/5+DWORD(Random_Double*(TIME_DISTRIBUTE_ROBOT/5*4));					
			m_pITimerEngine->SetTimer(IDI_DISTRIBUTE_ROBOT,dwElpase*1000L,TIMES_INFINITY,NULL);

			//更新分数
			if (CServerRule::IsMatchUserByScore(m_pGameServiceOption->dwServerRule))
			{
				m_ScoreManager.UpdateScoreData();
			}

			//动作处理
			if (m_RobotUserManager.GetRobotCount()>0 && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
			{
				//变量定义
				bool bAllowDynamicJoin=CServerRule::IsDynamicJoinGame(m_pGameServiceOption->dwServerRule);
				bool bAllowRobotAttend=g_GlobalUnits.IsAllowRobotAttend();
				bool bAllowRobotSimulate=g_GlobalUnits.IsAllowRobotSimulate();
				bool bAllowDistributeMode=(m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)&&(m_pGameServiceAttrib->wChairCount < MAX_CHAIR);
					
				//模拟处理
				if (bAllowRobotSimulate==true && bAllowDistributeMode==false && bDisposeSitDown==true)
				{
					//机器状态
					tagRobotUserInfo RobotSimulate; 
					m_RobotUserManager.GetRobotUserInfo(RobotSimulate,ROBOT_SIMULATE);

					//机器处理
					if (RobotSimulate.wFreeUserCount>0)
					{
						for (WORD i=0;i<8;i++)
						{
							//随机桌子
							WORD wTableID=WORD(Random_Double*(__max(m_pGameServiceOption->wTableCount/3,1)));

							//获取桌子
							CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
							if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

							//桌子状况
							tagTableUserInfo TableUserInfo;
							WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

							//变量定义
							WORD wChairCount=m_pGameServiceAttrib->wChairCount;	

							//分配判断
							if (TableUserInfo.wTableUserCount>0) continue;
							if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(m_pGameServiceAttrib->wChairCount<MAX_CHAIR)) continue;

							//坐下判断
							if (RobotSimulate.wFreeUserCount>=TableUserInfo.wMinUserCount)
							{
								//变量定义
								WORD wHandleCount=0;
								WORD wWantRobotCount=TableUserInfo.wMinUserCount;

								//数据调整
								if (m_pGameServiceAttrib->wChairCount>TableUserInfo.wMinUserCount)
								{
									WORD wFreeUserCount=RobotSimulate.wFreeUserCount;
									WORD wOffUserCount=__min(wChairCount,wFreeUserCount)-TableUserInfo.wMinUserCount;
									wWantRobotCount+=((wOffUserCount > 0) ? WORD(Random_Double*(wOffUserCount+1)) : 0);
								}

								//坐下处理
								for (WORD j=0;j<RobotSimulate.wFreeUserCount;j++)
								{
									//变量定义
									WORD wChairID=pTableFrame->GetRandNullChairID();

									//无效过滤
									//ASSERT(wChairID!=INVALID_CHAIR);
									if (wChairID==INVALID_CHAIR) continue;

									//用户坐下
									CRobotUserItem * pRobotUserItem=(CRobotUserItem *)RobotSimulate.pIRobotUserFree[j];
									if (pTableFrame->PerformSitDownAction(wChairID, pRobotUserItem->GetMeUserItem())==true)
									{
										//设置变量
										wHandleCount++;

										//完成判断
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

				//变量定义
				bool bWaitContinue=true;
				bool bMaxPlayerGame=m_pGameServiceAttrib->wChairCount>=MAX_CHAIR;

				//获取时间
				DWORD dwCurrTime = (DWORD)time(NULL);
				if (dwCurrTime>=m_dwIntervalTime+m_dwLastDisposeTime)
				{
					//设置变量
					bWaitContinue=false;
					m_dwLastDisposeTime=dwCurrTime;
					m_dwIntervalTime=MIN_INTERVAL_TIME/2; 
					if(bMaxPlayerGame==true)
					{
						m_dwIntervalTime=MIN_INTERVAL_TIME+DWORD(Random_Double*(MAX_INTERVAL_TIME-MIN_INTERVAL_TIME)); 
					}
				}

				//陪打处理
				if (bAllowRobotAttend==true && bDisposeSitDown==true)
				{
					//被动状态
					tagRobotUserInfo RobotPassivity;
					m_RobotUserManager.GetRobotUserInfo(RobotPassivity,ROBOT_PASSIVITY);

					if (bAllowDistributeMode==true)
					{
						//坐下处理
						for (WORD j=0;j<RobotPassivity.wFreeUserCount;j++)
						{
							CRobotUserItem * pRobotUserItem=(CRobotUserItem *)RobotPassivity.pIRobotUserFree[j];
							if (InsertDistribute(pRobotUserItem->GetMeUserItem())==true) return true;
						}
					}
					else
					{
						//被动处理
						if (RobotPassivity.wFreeUserCount>0)
						{
							//百人游戏
							if (bMaxPlayerGame==true)
							{
								for (INT_PTR i=0;i<(m_pGameServiceOption->wTableCount);i++)
								{
									//获取桌子
									CTableFrameItem * pTableFrame=m_TableFrameArray[i];
									if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

									//桌子状况
									tagTableUserInfo TableUserInfo;
									WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

									//分配判断
									if (wUserSitCount>m_pGameServiceAttrib->wChairCount*2/3) continue;

									//变量定义
									IServerUserItem * pIServerUserItem=NULL;
									WORD wChairID=pTableFrame->GetRandNullChairID();

									//无效过滤
									ASSERT(wChairID!=INVALID_CHAIR);
									if (wChairID==INVALID_CHAIR) continue;

									//坐下处理
									for (WORD j=0;j<RobotPassivity.wFreeUserCount;j++)
									{
										//获取用户
										CRobotUserItem * pRobotUserItem=(CRobotUserItem *)RobotPassivity.pIRobotUserFree[j];
										IServerUserItem * pIServerUserItem= pRobotUserItem->GetMeUserItem();

										//执行坐下
										if (pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==true) return true;
									}
								}
							}
							else
							{
								//获取时间
								if (bWaitContinue==false)
								{
									INT nStartTable=INT(Random_Double*__max(m_pGameServiceOption->wTableCount-1,1));
									for (INT_PTR i=nStartTable;i<(nStartTable+m_pGameServiceOption->wTableCount);i++)
									{
										//获取桌子
										WORD wTableID=WORD(i%m_pGameServiceOption->wTableCount);
										CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
										if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

										//桌子状况
										tagTableUserInfo TableUserInfo;
										WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

										//分配判断											
										if (TableUserInfo.wClientReadyCount==0) continue;									
										if ((wUserSitCount>=TableUserInfo.wMinUserCount)&&(WORD(Random_Double*10)>7)) continue;											

										//变量定义
										IServerUserItem * pIServerUserItem=NULL;
										WORD wChairID=pTableFrame->GetRandNullChairID();

										//无效过滤
										if (wChairID==INVALID_CHAIR) continue;

										//变量定义
										INT nAreaIndex=0;
										bool bMatchUserByScore=CServerRule::IsMatchUserByScore(m_pGameServiceOption->dwServerRule)&&(wUserSitCount>0);

										//获取区间
										if (bMatchUserByScore==true) nAreaIndex=GetTableScoreAreaIndex(pTableFrame);	

										//坐下处理
										for (WORD j=0;j<RobotPassivity.wFreeUserCount;j++)
										{
											//获取用户
											CRobotUserItem * pRobotUserItem=(CRobotUserItem *)RobotPassivity.pIRobotUserFree[j];
											IServerUserItem * pIServerUserItem= pRobotUserItem->GetMeUserItem();

											//匹配分数
											if (bMatchUserByScore==true)
											{
												//获取区间																	
												INT nAreaIndex1=m_ScoreManager.GetScoreAreaIndex(pIServerUserItem->GetUserFreeScore());	

												//匹配索引
												if (nAreaIndex!=nAreaIndex1) continue;			
											}

											//执行坐下
											if (pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==true) return true;
										}
									}
								}
							}
						}
					}
				}
	    
				//陪打处理
				if (bAllowRobotAttend==true && bDisposeSitDown==true)
				{
					//主动状态
					tagRobotUserInfo RobotInitiative;
					m_RobotUserManager.GetRobotUserInfo(RobotInitiative,ROBOT_INITIATIVE);
					WORD wAllRobotCount = RobotInitiative.wFreeUserCount+RobotInitiative.wPlayUserCount+RobotInitiative.wSitdownUserCount;

					if (bAllowDistributeMode==true)
					{
						//坐下处理
						for (WORD j=0;j<RobotInitiative.wFreeUserCount;j++)
						{
							CRobotUserItem * pRobotUserItem=(CRobotUserItem *)RobotInitiative.pIRobotUserFree[j];
							if (InsertDistribute(pRobotUserItem->GetMeUserItem())==true) return true;
						}
					}
					else
					{
						//主动处理
						if (RobotInitiative.wFreeUserCount>0)
						{
							//获取时间
							if (bMaxPlayerGame==true || bWaitContinue==false)
							{
								INT nStartTable=INT(Random_Double*__max(m_pGameServiceOption->wTableCount-1,1));
								for (INT_PTR i=nStartTable;i<(nStartTable+m_pGameServiceOption->wTableCount);i++)
								{
									//获取桌子
									WORD wTableID=WORD(i%m_pGameServiceOption->wTableCount);
									CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
									if ((pTableFrame->IsGameStarted()==true)&&(bAllowDynamicJoin==false)) continue;

									//桌子状况
									tagTableUserInfo TableUserInfo;
									WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

									//人数调整
									if (bMaxPlayerGame==false) 
									{									
										//人数判断
										if ((wUserSitCount >= TableUserInfo.wMinUserCount) && (WORD(Random_Double * 10) > 7)) continue;
									}

									//变量定义
									IServerUserItem * pIServerUserItem=NULL;
									WORD wChairID=pTableFrame->GetRandNullChairID();

									//无效过滤
									if (wChairID==INVALID_CHAIR) continue;

									//变量定义
									INT nAreaIndex=0;
									bool bMatchUserByScore=CServerRule::IsMatchUserByScore(m_pGameServiceOption->dwServerRule)&&(wUserSitCount>0);

									//获取区间
									if (bMatchUserByScore==true) nAreaIndex=GetTableScoreAreaIndex(pTableFrame);	

									//坐下处理
									for (WORD j=0;j<RobotInitiative.wFreeUserCount;j++)
									{
										//获取用户
										CRobotUserItem * pRobotUserItem=(CRobotUserItem *)RobotInitiative.pIRobotUserFree[j];
										IServerUserItem * pIServerUserItem= pRobotUserItem->GetMeUserItem();

										//匹配分数
										if (bMatchUserByScore==true)
										{
											//获取区间																	
											INT nAreaIndex1=m_ScoreManager.GetScoreAreaIndex(pIServerUserItem->GetUserFreeScore());	

											//匹配索引
											if (nAreaIndex!=nAreaIndex1) continue;			
										}

										//执行坐下
										if (pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==true) return true;
									}
								}
							}
						}
					}
				}

				//状态判断
				if (bWaitContinue==true) return true;

				//起立处理
				WORD wStandUpCount=0;
				WORD wRandStartChair=0;
				WORD wRandCount=WORD(Random_Double*2)+1;
				INT_PTR nIndex = INT_PTR(Random_Double*(__max(m_pGameServiceOption->wTableCount-1,1)));
				for (INT_PTR i=nIndex;i<m_pGameServiceOption->wTableCount+nIndex;++i)
				{
					//获取桌子
					INT_PTR nTableIndex=i%m_pGameServiceOption->wTableCount;
					CTableFrameItem * pTableFrame=m_TableFrameArray[nTableIndex];
					if (pTableFrame->IsGameStarted()==true) continue;

					//桌子状况
					tagTableUserInfo TableUserInfo;
					WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

					//用户过虑
					bool bRand=(WORD(Random_Double*100)>60);
					if (TableUserInfo.wTableRobotCount==0) continue;
					if ((TableUserInfo.wTableUserCount>0)&&(bAllowRobotAttend==true) && bRand) continue;
					if (TableUserInfo.wTableRobotCount>=TableUserInfo.wMinUserCount && bRand) continue;

					//起立处理
					for (WORD j=0;j<pTableFrame->GetChairCount();j++)
					{
						//获取用户
						IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
						if (pIServerUserItem==NULL) continue;

						//用户起立
						if (pIServerUserItem->IsRobotUser()==true)								
						{
							if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) continue;
							if (++wStandUpCount>=wRandCount) return true;
							break;
						}
					}
				}

				//起立处理
				nIndex = INT_PTR(Random_Double*(__max(m_pGameServiceOption->wTableCount-1,1)));
				for (INT_PTR i=nIndex;i<m_pGameServiceOption->wTableCount+nIndex;++i)
				{
					//获取桌子
					INT_PTR nTableIndex=i%m_pGameServiceOption->wTableCount;
					CTableFrameItem * pTableFrame=m_TableFrameArray[nTableIndex];
					if (pTableFrame->IsGameStarted()==true) continue;

					//桌子状况
					tagTableUserInfo TableUserInfo;
					WORD wUserSitCount=pTableFrame->GetTableUserInfo(TableUserInfo);

					//用户过虑
					bool bRand=(WORD(Random_Double*100)>70);
					if (TableUserInfo.wTableRobotCount==0) continue;
					if ((TableUserInfo.wTableUserCount>0)&&(bAllowRobotAttend==true) && bRand) continue;

					//起立处理	
					for (WORD j=0;j<pTableFrame->GetChairCount();j++)
					{
						//获取用户
						IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(j);
						if (pIServerUserItem==NULL) continue;							

						//用户起立
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
		case IDI_REGULAR_TIMER:			//常规定时器
		{
			//增加时间
			m_dwRegularTimes++;

			//遍历锁桌
			POSITION pos = m_LockedTableTimes.GetStartPosition();
			WORD wTableID = INVALID_TABLE;
			DWORD dwLockedTableTimes = 0;
			while(pos!=NULL)
			{
				m_LockedTableTimes.GetNextAssoc(pos,wTableID,dwLockedTableTimes);
					
				//检查桌子
				ASSERT(wTableID!=INVALID_TABLE);
				CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
				ASSERT(pTableFrame);
				if( !pTableFrame || !pTableFrame->IsTableLocked() || pTableFrame->IsTableStarted() )
				{
					m_LockedTableTimes.RemoveKey(wTableID);
					continue;
				}

				//超时判断
				if( dwLockedTableTimes <= m_dwRegularTimes )
				{
					//踢出所有玩家
					for( WORD i = 0; i < pTableFrame->GetChairCount(); i++ )
					{
						IServerUserItem *pIServerUserItem = pTableFrame->GetTableUserItem(i);
						if( pIServerUserItem == NULL ) continue;

						//发送消息
						LPCTSTR pszMessage=TEXT("抱歉，由于本桌长时间没有开始游戏，系统将请您重新选择桌子！");
						SendGameMessage(pIServerUserItem,pszMessage,SMT_CHAT|SMT_EJECT|SMT_CLOSE_GAME);
						SendRoomMessage(pIServerUserItem,pszMessage,SMT_CHAT);
							
						//执行起立
						pTableFrame->PerformStandUpAction(pIServerUserItem);
					}

					//删除索引
					m_LockedTableTimes.RemoveKey(wTableID);
				}
			}
				
			return true;
		}		
		}

		return false;
	}


	//机器人时间
	if ((dwTimerID>=IDI_ROBOT_MANAGER_START)&&(dwTimerID<=IDI_ROBOT_MANAGER_RANGE))
	{
		//时间处理
		m_RobotUserManager.OnEventTimerPulse(dwTimerID,dwBindParameter);

		return true;
	}
	
	//扩展定时器
	if ((dwTimerID>=IDI_EXTEND_MODULE_START)&&(dwTimerID<IDI_EXTEND_MODULE_RANGE))
	{
		if(m_pIExtendServiceManager!=NULL) m_pIExtendServiceManager->OnEventTimer(dwTimerID,dwBindParameter);
		return true;
	}
	
	//桌子时间
	if (dwTimerID>=IDI_TABLE_FRAME_START&&dwTimerID<=IDI_TABLE_FRAME_RANGE)
	{
		//桌子号码
		DWORD dwTableTimerID=dwTimerID-IDI_TABLE_FRAME_START;
		WORD wTableID=(WORD)(dwTableTimerID/TIME_TABLE_FRAME_RANGE);

		//时间效验
		if (wTableID>=(WORD)m_TableFrameArray.GetCount()) 
		{
			ASSERT(FALSE);
			return false;
		}

		//时间通知
		CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
		return pTableFrame->OnEventTimer(dwTableTimerID%TIME_TABLE_FRAME_RANGE,dwBindParameter);
	}

	return false;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);
	ZeroMemory(pBindParameter, sizeof(tagBindParameter));

	//设置变量
	if (pBindParameter!=NULL)
	{
		pBindParameter->dwClientIP=dwClientIP;
		pBindParameter->dwContextID=dwSocketID;
		pBindParameter->dwActiveTime=(DWORD)time(NULL);
		pBindParameter->TokenParameter.dwClientIP=dwClientIP;

		return true;
	} 

	//错误断言
	ASSERT(FALSE);

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//处理结果
	bool bResult=true;
	WORD wMainCmdID = LOCMMD(Command.wMainCmdID);

#ifdef _DEBUG
	{
		CString str;
		str.Format(L"[LH][Game]%d,M:%d,S:%d", Command.wMainCmdID, wMainCmdID, Command.wSubCmdID);
		OutputDebugString(str);
	}
#endif

	//注册服务
	if (wMainCmdID == MDM_CS_REGISTER)
	{
		return OnTCPNetworkMainRegister(Command.wSubCmdID, pData, wDataSize, dwSocketID);
	}

	//通用服务
	if (wMainCmdID == MDM_CS_COMMON_SERVICE)
	{
		return OnTCPNetworkMainCommonService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
	}

	try
	{
		switch (wMainCmdID)
		{
		case MDM_GR_LOGON:		//登录命令
		{
			bResult=OnTCPNetworkMainLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
			break;
		}
		case MDM_GR_USER:		//用户命令
		{
			bResult=OnTCPNetworkMainUser(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
			break;
		}
		case MDM_GR_MANAGE:		//管理命令
		{
			bResult=OnTCPNetworkMainManage(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
			break;
		}
		case MDM_GF_FRAME:		//框架命令
		{
			bResult=OnScoketMainFrame(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
			break;
		}
		case MDM_GF_GAME:		//游戏命令
		{
			bResult=OnScoketMainGame(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
			break;
		}
		case MDM_GR_MEMBER:		//会员命令
		{
			bResult=OnTCPNetworkMainMember(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
			break;
		}
		case MDM_GR_QUERY:		//查询命令
		{
			bResult=OnTCPNetworkMainQuery(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
			break;
		}
		case MDM_GR_MATCH:		//比赛命令
		{
			bResult=OnTCPNetworkMainMatch(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
			break;
		}
		case MDM_GR_DEBUG:		//调试命令
		{
			bResult = OnTCPNetworkMainDebug(Command.wSubCmdID, pData, wDataSize, dwSocketID, dwTokenID);
			break;
		}	
		case MDM_GR_BATTLE:		//约战命令
		{
			bResult = OnTCPNetwrokMainBattle(Command.wSubCmdID, pData, wDataSize, dwSocketID, dwTokenID);
			break;			
		}
		}			
	} 
	catch (...)
	{
		//设置结果
		bResult=false;

		//提示消息
		CTraceService::TraceString(TEXT("wMainCmdID=%d,wSubCmdID=%d"),TraceLevel_Normal, Command.wMainCmdID, Command.wSubCmdID);
	}

	//强制退出
	if (bResult==false)
	{
		//构造结构
		CMD_CS_C_ShutdownSocket ShutdownSocket;
		ShutdownSocket.bForceClose = false;

		//发送数据
		SendDataToGate(dwSocketID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));
	}

	return true;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//获取参数
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//变量定义
	IServerUserItem * pIServerUserItem = NULL;
	tagTokenParameter * pTokenParameter = NULL;

	//直连模式
	if (pBindParameter->wServiceModule == SERVICE_MODULE_NONE)
	{
		//获取参数
		pTokenParameter = &pBindParameter->TokenParameter;

		//获取用户
		pIServerUserItem = (IServerUserItem*)pTokenParameter->pBindUserItem;
		if (pIServerUserItem == NULL) return false;

		//桌子标识
		WORD wTableID = pIServerUserItem->GetTableID();

		//断线处理
		if (wTableID != INVALID_TABLE)
		{
			//解除绑定
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
		//服务子项
		auto pGlobalGateItem = pBindParameter->pGlobalGateItem;
		if (pGlobalGateItem != NULL)
		{
			//变量定义
			tagTokenParameter* pTokenParameter = NULL;

			do
			{
				//获取参数
				pTokenParameter = pGlobalGateItem->EnumTokenID(pTokenParameter);
				if (pTokenParameter == NULL) break;

				//获取用户
				pIServerUserItem = (IServerUserItem*)pTokenParameter->pBindUserItem;
				if (pIServerUserItem != NULL)
				{
					//桌子标识
					WORD wTableID = pIServerUserItem->GetTableID();

					//断线处理
					if (wTableID != INVALID_TABLE)
					{
						//解除绑定
						pIServerUserItem->DetachBindStatus();

						//掉线通知
						ASSERT(wTableID < m_pGameServiceOption->wTableCount);
						m_TableFrameArray[wTableID]->OnEventUserOffLine(pIServerUserItem);
					}
					else
					{
						pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
					}
				}				

			} while (true);

			//移除服务
			m_GlobalItemManager.DeleteGateItem(pBindParameter->dwServiceID);
		}		
	}

	//清除信息
	ZeroMemory(pBindParameter, sizeof(tagBindParameter));

	return true;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//游戏事件
	if (wRequestID >= DB_GAME_RESPOND_START && wRequestID<= DB_GAME_RESPOND_START+DB_GAME_IDENTIFY_RANGE)
	{
		if (m_pIGameServiceManager != NULL)
		{
			m_pIGameServiceManager->OnEventDataBase(wRequestID, pData, wDataSize, dwContextID, dwTokenID);
		}
	}

	//扩展事件
	if (wRequestID >= DB_EXTEND_RESPOND_START && wRequestID <= DB_EXTEND_RESPOND_START + DB_EXTEND_IDENTIFY_RANGE)
	{
		if (m_pIExtendServiceManager != NULL)
		{
			return m_pIExtendServiceManager->OnEventDataBase(wRequestID, pData, wDataSize, dwContextID, dwTokenID);
		}
	}

	//框架事件
	if (wRequestID >= DB_FRAME_RESPOND_START && wRequestID <= DB_FRAME_RESPOND_START + DB_FRAME_IDENTIFY_RANGE)
	{
		switch (wRequestID)
		{
		case DBO_GR_ROBOT_USER:			//机器信息
		{
			return OnDBRobotUser(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_VERSION_INFO:		//版本信息
		{
			return OnDBGameVersionInfo(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_CONFIG_RESULT:		//配置结果
		{
			return OnDBConfigResult(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_BATTLE_GAME_OPTION:	//游戏选项
		{
			return OnDBBattleOption(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_ROBOT_PARAMETER:	//机器参数
		{
			return OnDBRobotParameter(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_SUBSIDY_PARAMETER:	//低保参数
		{
			return OnDBSubsidyParameter(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_CONTROL_PARAMETER:	//控制参数
		{
			return OnDBControlParameter(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_LOGON_SUCCESS:		//登录成功
		{
			return OnDBLogonSuccess(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_LOGON_FAILURE:		//登录失败
		{
			return OnDBLogonFailure(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_WEALTH_UPDATE:		//财富更新
		{
			return OnDBWealthUpdate(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_WRITE_GAME_SCORE:	//写分结果
		{
			return OnDBWriteScoreResult(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_FILTER_WORDS:		//加载敏感词
		{
			return OnDBFilterWords(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_UPDATE_GOLD:		//更新金币
		{
			return OnDBUpdateGold(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_WRITE_USER_PROP:	//写入道具
		{
			return OnDBWriteUserProp(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_USER_GOODS_UPDATE:	//物品更新
		{
			return OnDBUserGoodsUpdate(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_LARGESS_SUBSIDY:	//领取低保
		{
			return OnDBLargessSubsidy(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_USER_CONTROL_INFO:	//控制信息
		{
			return OnDBUserControlInfo(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_USER_CONTROL_UPDATE://控制更新
		{
			return OnDBUserControlUpdate(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_SEND_GIFT_SUCCESS:	//赠送成功
		{
			return OnDBSendGiftSuccess(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_SEND_GIFT_FAILURE:	//赠送失败
		{
			return OnDBSendGiftFailure(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_UPDATE_LOVELINESS:	//更新魅力
		{
			return OnDBUpdateLoveLiness(dwContextID, dwTokenID, pData, wDataSize);
		}
		}
	}

	return false;
}

//连接事件
bool CAttemperEngineSink::OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode)
{
	//网络服务
	ITCPSocketService * pITCPSocketService = NULL;

	//中心连接
	if (dwServiceID==NETWORK_CENTER)
	{
		//错误判断
		if (nErrorCode!=0)
		{
			//提示消息
			CTraceService::TraceString(TEXT("中心服务器连接失败 [ %ld ]，%ld 秒后将重新连接"),TraceLevel_Warning, nErrorCode, m_pInitParameter->m_wConnectTime);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}

		//提示消息
		CTraceService::TraceString(TEXT("正在注册游戏服务..."),TraceLevel_Normal);

		//设置接口
		pITCPSocketService = m_pITCPSocketCenter;	

		//设置定时器
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_REPORT_SERVER_INFO,TIME_REPORT_SERVER_INFO*1000L,TIMES_INFINITY,0);		
	}

	//存储连接
	if (dwServiceID==NETWORK_STORE)
	{
		//错误判断
		if (nErrorCode!=0)
		{
			//提示消息
			CTraceService::TraceString(TEXT("存储服务器连接失败 [ %ld ]，%ld 秒后将重新连接"),TraceLevel_Warning, nErrorCode, m_pInitParameter->m_wConnectStore);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_STORE,m_pInitParameter->m_wConnectStore*1000L,1,0);

			return false;
		}

		//提示消息
		CTraceService::TraceString(TEXT("正在注册游戏服务..."),TraceLevel_Normal);

		//设置接口
		pITCPSocketService = m_pITCPSocketStore;
	}	

	//服务连接
	if (dwServiceID==NETWORK_SERVICE)
	{
		//错误判断
		if (nErrorCode!=0)
		{
			//提示消息
			CTraceService::TraceString(TEXT("%s连接失败 [ %ld ]，%ld 秒后将重新连接"),TraceLevel_Warning, m_ServiceItem.szServiceName, nErrorCode, m_pInitParameter->m_wConnectService);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_SERVICE,m_pInitParameter->m_wConnectService *1000L,1,0);

			return false;
		}

		//设置接口
		pITCPSocketService = m_pITCPSocketService;
	}

	//注册服务
	if (pITCPSocketService != NULL)
	{
		CMD_CS_C_RegisterServer RegisterServer;
		ZeroMemory(&RegisterServer, sizeof(RegisterServer));

		//构造数据
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

		//地址信息
		RegisterServer.dwServerAddr = inet_addr(CT2CA(m_pGameServiceOption->szServiceAddr));
		StringCchCopy(RegisterServer.szServerDomain, CountArray(RegisterServer.szServerDomain), m_pGameServiceOption->szServiceDomain);

		//分数设置
		RegisterServer.lCellScore = m_pGameServiceOption->lCellScore;
		RegisterServer.lMinEnterScore = m_pGameServiceOption->lMinEnterScore;
		RegisterServer.lMaxEnterScore = m_pGameServiceOption->lMaxEnterScore;

		//发送数据
		ASSERT(pITCPSocketService != NULL);
		pITCPSocketService->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_SERVER, &RegisterServer, sizeof(RegisterServer), 0);
	}	

	return true;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason)
{
	switch (dwServiceID)
	{	
	case NETWORK_CENTER:	//中心连接
	{
		//设置变量
		m_bCollectUser=false;

		//删除时间
		m_pITimerEngine->KillTimer(IDI_REPORT_SERVER_INFO);

		//重连判断
		if (m_bNeekCenter==true)
		{
			//提示消息
			CTraceService::TraceString(TEXT("与中心服务器的连接关闭了，%ld 秒后将重新连接"),TraceLevel_Warning, TIME_CONNECT_CENTER);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER, TIME_CONNECT_CENTER *1000L,1,0);
		}

		return true;
	}	
	case NETWORK_STORE:			//存储连接
	{
		//设置变量
		m_bCacheVideo=true;

		//重连判断
		if (m_bNeedStore==true)
		{
			//提示消息
			CTraceService::TraceString(TEXT("与存储服务器的连接关闭了，%ld 秒后将重新连接"),TraceLevel_Warning, m_pInitParameter->m_wConnectStore);

			//设置时间
			ASSERT (m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_STORE, m_pInitParameter->m_wConnectStore *1000L,1,0);
		}

		return true;
	}
	case NETWORK_SERVICE:		 //服务连接
	{
		//重连判断
		if (m_bNeedService==true)
		{
			//提示消息
			CTraceService::TraceString(TEXT("与%s的连接关闭了，%ld 秒后将重新连接"),TraceLevel_Warning,m_ServiceItem.szServiceName, m_pInitParameter->m_wConnectService);

			//设置时间
			ASSERT (m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_SERVICE, m_pInitParameter->m_wConnectService *1000L,1,0);
		}

		return true;
	}
	}

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	try
	{
		//存储连接
		if (dwServiceID==NETWORK_STORE)
		{
			switch (Command.wMainCmdID)
			{
			case MDM_CS_REGISTER:		//注册信息
				{
					return OnTCPSocketMainStoreRegister(Command.wSubCmdID,pData,wDataSize);
				}
			}		
		}

		//服务连接
		if (dwServiceID==NETWORK_SERVICE)
		{
			switch (Command.wMainCmdID)
			{
			case MDM_CS_REGISTER:		 //注册信息
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

		//中心连接
		if (dwServiceID==NETWORK_CENTER)
		{
			switch (Command.wMainCmdID)
			{
			case MDM_CS_REGISTER:			//注册服务
			{
				return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
			}
			case MDM_CS_COMMON_SERVICE:		//服务信息
			{
				return OnTCPSocketMainCommonService(Command.wSubCmdID,pData,wDataSize);
			}
			case MDM_CS_FORWARD_SERVICE:	//转发服务
			{
				return OnTCPSocketMainForwardService(Command.wSubCmdID, pData, wDataSize);
			}
			case MDM_CS_USER_COLLECT:		//用户汇总
			{
				return OnTCPSocketMainUserCollect(Command.wSubCmdID,pData,wDataSize);
			}
			case MDM_CS_QUERY_SERVICE:		//查询服务
			{
				return OnTCPSocketMainQueryService(Command.wSubCmdID,pData,wDataSize);
			}
			case MDM_CS_BROADCAST:			//广播消息
			{
				return OnTCPSocketMainBroadcast(Command.wSubCmdID,pData,wDataSize);
			}
			case MDM_CS_MANAGER_SERVICE:	//管理服务
			{
				return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
			}
			}
		}
	}
	catch (...)
	{
		//提示消息
		CTraceService::TraceString(TEXT("dwServiceID=%d,wMainCmdID=%d,wSubCmdID=%d"),TraceLevel_Normal, dwServiceID, Command.wMainCmdID, Command.wSubCmdID);
	}

	return false;
}

//控制开始
bool CAttemperEngineSink::OnEventUserControlStart(IServerUserItem * pIServerUserItem)
{
	//参数校验
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL) return false;
	
	//构造结构
	DBR_GR_ControlStart ControlStart;
	ZeroMemory(&ControlStart,sizeof(ControlStart));

	//用户信息
	ControlStart.dwUserID = pIServerUserItem->GetUserID();		

	//输赢累计
	ControlStart.wAddupWinRate = pIServerUserItem->GetAddupWinRate();
	ControlStart.lAddupWinScore = pIServerUserItem->GetAddupWinScore();
	ControlStart.lAddupLoseScore = pIServerUserItem->GetAddupLoseScore();

	//控制信息
	CopyMemory(&ControlStart.UserControlItem, pIServerUserItem->GetUserControlItem(), sizeof(tagUserControlItem));
		
	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_CONTROL_START, ControlStart.dwUserID, MAKELONG(pIServerUserItem->GetBindIndex(),0), pIServerUserItem->GetTokenID(), &ControlStart, sizeof(ControlStart));

	return true;
}

//控制更新
bool CAttemperEngineSink::OnEventUserControlUpdate(IServerUserItem* pIServerUserItem)
{
	//参数校验
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return false;

	//构造结构
	DBR_GR_ControlUpdate ControlUpdate;
	ZeroMemory(&ControlUpdate, sizeof(ControlUpdate));

	//控制信息
	const tagUserControlItem* pUserControlItem = pIServerUserItem->GetUserControlItem();

	//用户信息
	ControlUpdate.dwUserID = pIServerUserItem->GetUserID();

	//游戏信息
	ControlUpdate.lWinScore = pUserControlItem->lTotalWinScore;
	ControlUpdate.lLoseScore = pUserControlItem->lTotalLoseScore;
	ControlUpdate.lWinCount = pUserControlItem->lTotalWinCount;
	ControlUpdate.lLoseCount = pUserControlItem->lTotalLoseCount;
	ControlUpdate.lBetAmount = pUserControlItem->lTotalBetAmount;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_CONTROL_UPDATE, ControlUpdate.dwUserID, MAKELONG(pIServerUserItem->GetBindIndex(), 0), pIServerUserItem->GetTokenID(), &ControlUpdate, sizeof(ControlUpdate));

	return true;
}

//控制结束
bool CAttemperEngineSink::OnEventUserControlFinish(IServerUserItem * pIServerUserItem)
{
	//参数校验
	ASSERT (pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return false;

	//构造结构
	DBR_GR_ControlFinish ControlFinish;
	ZeroMemory(&ControlFinish, sizeof(ControlFinish));

	//用户信息
	ControlFinish.dwUserID = pIServerUserItem->GetUserID();
		
	//输赢累计
	ControlFinish.wAddupWinRate = pIServerUserItem->GetAddupWinRate();
	ControlFinish.lAddupWinScore = pIServerUserItem->GetAddupWinScore();
	ControlFinish.lAddupLoseScore = pIServerUserItem->GetAddupLoseScore();

	//获取对象
	const tagUserControlItem * pUserControlItem = pIServerUserItem->GetUserControlItem();
		
	//游戏信息
	ControlFinish.lWinScore = pUserControlItem->lTotalWinScore;
	ControlFinish.lLoseScore = pUserControlItem->lTotalLoseScore;
	ControlFinish.lWinCount = pUserControlItem->lTotalWinCount;
	ControlFinish.lLoseCount = pUserControlItem->lTotalLoseCount;
	ControlFinish.lBetAmount = pUserControlItem->lTotalBetAmount;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_CONTROL_FINISH,ControlFinish.dwUserID, MAKELONG(pIServerUserItem->GetBindIndex(), 0), pIServerUserItem->GetTokenID(), &ControlFinish, sizeof(ControlFinish));

	return true;
}

//用户积分
bool CAttemperEngineSink::OnEventUserItemScore(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送成绩
	SendUserScore(pIServerUserItem);	

	//变量定义
	WORD wBindIndex=pIServerUserItem->GetBindIndex();
	
	//通知桌子
	if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
	{
		//变量定义
		CTableFrameItem * pTableFrame = m_TableFrameArray[pIServerUserItem->GetTableID()];
		if (pTableFrame != NULL)
		{
			pTableFrame->OnEventUserScoreChange(pIServerUserItem);
		}
	}

	//通知服务
	if (m_pIExtendServiceManager != NULL)
	{
		m_pIExtendServiceManager->OnEventUserItemScore(pIServerUserItem);
	}

	//即时写分
	if (CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule)==true)
	{
		//变量定义
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

		//锁定信息
		tagLockScoreInfo LockScoreInfo;
		ZeroMemory(&LockScoreInfo,sizeof(LockScoreInfo));
		LockScoreInfo.lMaxLockScore=m_pGameServiceOption->lMaxLockScore;
		LockScoreInfo.lLessLockScore=m_pGameServiceOption->lLessLockScore;
		LockScoreInfo.lRestrictScore=m_pGameServiceOption->lRestrictScore;

		//提取积分
		tagVariationScore VariationScore;
		ZeroMemory(&VariationScore,sizeof(VariationScore));
		pIServerUserItem->DistillVariation(VariationScore,LockScoreInfo);

		//用户信息
		WriteGameScore.dwUserID=pIServerUserItem->GetUserID();
		WriteGameScore.dwClientIP=pIServerUserItem->GetClientIP();
		WriteGameScore.dwPlayTimeCount=VariationScore.dwPlayTimeCount;

		//积分信息
		WriteGameScore.lRevenue=VariationScore.lRevenue;
		WriteGameScore.lUnLockScore=VariationScore.lUnLockScore;
		
		//积分信息
		WriteGameScore.lScore=VariationScore.VariationScore.lScore;		
		WriteGameScore.lWinCount=VariationScore.VariationScore.lWinCount;
		WriteGameScore.lLostCount=VariationScore.VariationScore.lLostCount;
		WriteGameScore.lDrawCount=VariationScore.VariationScore.lDrawCount;
		WriteGameScore.lFleeCount=VariationScore.VariationScore.lFleeCount;
		WriteGameScore.lExperience=VariationScore.VariationScore.lExperience;
		WriteGameScore.lBetAmount= VariationScore.VariationScore.lBetAmount;
		WriteGameScore.lWinStreak=VariationScore.VariationScore.lWinStreak;
		WriteGameScore.lMaxWinStreak=VariationScore.VariationScore.lMaxWinStreak;

		//金币房间
		if (m_pGameServiceOption->wServerType & GAME_GENRE_GOLD)
		{
			WriteGameScore.wAddupWinRate = pIServerUserItem->GetAddupWinRate();
			WriteGameScore.lAddupWinScore = pIServerUserItem->GetAddupWinScore();
			WriteGameScore.lAddupLoseScore = pIServerUserItem->GetAddupLoseScore();
		}

		//比赛房间
		if (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)
		{
			WriteGameScore.lMatchNo=pIServerUserItem->GetMatchNo();
			WriteGameScore.dwMatchID=pIServerUserItem->GetMatchID();			
		}

		//附加信息
		WriteGameScore.dwInoutIndex = pIServerUserItem->GetInoutIndex();

		//投递请求
		if (pIServerUserItem->IsRobotUser() == false)
		{
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_GAME_SCORE, WriteGameScore.dwUserID, MAKELONG(wBindIndex, 0), pIServerUserItem->GetTokenID(), &WriteGameScore, sizeof(WriteGameScore));
		}

		//赠送低保(仅适用金币房)
		if (m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD) && CServerRule::IsAllowLargessSubsidy(m_pGameServiceOption->dwServerRule)==true)
		{
			//类型判断
			if (pIServerUserItem->IsMobileUser()==true && pIServerUserItem->GetResidualTimes()>0 && pIServerUserItem->IsLargessSubsidy()==false)
			{
				//条件判断
				if (pIServerUserItem->GetSubsidyScore()<m_SystemParameter.lTakeSubsidyCondition)
				{
					//设置标识
					pIServerUserItem->SetLargessSubsidy(true);

					//变量定义
					DBR_GR_LargessSubsidy LargessSubsidy;
					ZeroMemory(&LargessSubsidy,sizeof(LargessSubsidy));

					//设置变量
					LargessSubsidy.dwUserID=pIServerUserItem->GetUserID();

					//附加信息
					LargessSubsidy.dwClientIP=pIServerUserItem->GetClientIP();			
					StringCchCopy(LargessSubsidy.szMachineID,CountArray(LargessSubsidy.szMachineID), pIServerUserItem->GetMachineID());

					//投递请求
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LARGESS_SUBSIDY, LargessSubsidy.dwUserID,MAKELONG(wBindIndex,0), pIServerUserItem->GetTokenID(),&LargessSubsidy,sizeof(LargessSubsidy));
				}
			}
		}
	}	

	return true;
}

//用户状态
bool CAttemperEngineSink::OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	CMD_GR_UserStatus UserStatus;
	auto pUserInfo=pIServerUserItem->GetUserInfo();

	//构造数据
	UserStatus.dwUserID=pUserInfo->dwUserID;
	UserStatus.UserStatus.wTableID=pUserInfo->wTableID;
	UserStatus.UserStatus.wChairID=pUserInfo->wChairID;
	UserStatus.UserStatus.cbUserStatus=pUserInfo->cbUserStatus;

	//变量定义
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();

	//换桌判断
	if (wTableID!=INVALID_TABLE && pIServerUserItem->GetUserStatus()<= US_LOOKON)
	{		
		if (wTableID!=wLastTableID)
		{
			//发送同桌用户
			SendTableUserToUser(pIServerUserItem, CLIENT_KIND_MOBILE, INVALID_DWORD);

			//发送给同桌用户
			SendUserToTableUser(pIServerUserItem, CLIENT_KIND_MOBILE, INVALID_DWORD);
		}	
	}

	//发送数据
	SendBatchData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));

	if (pIServerUserItem->GetUserStatus() >= US_SIT)
	{
		SendDataToTable(wTableID, CLIENT_KIND_MOBILE, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
	}
	else
	{
		//发送状态
		if (pIServerUserItem->IsMobileUser())
		{
			SendDataToUser(pIServerUserItem, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
		}

		SendDataToTable(wLastTableID, CLIENT_KIND_MOBILE, MDM_GR_USER, SUB_GR_USER_STATUS, &UserStatus, sizeof(UserStatus));
	}	

	//离开判断
	if (pUserInfo->cbUserStatus==US_NULL)
	{
		//用户登出
		OnEventUserLogout(pIServerUserItem);		
	}

	return true;
}

//用户排位
bool CAttemperEngineSink::OnEventUserItemRanking(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	return true;
}

//用户权限
bool CAttemperEngineSink::OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	auto pUserInfo=pIServerUserItem->GetUserInfo();

	DBR_GR_ManageUserRight ManageUserRight= {0};
	ManageUserRight.dwUserID = pUserInfo->dwUserID;
	ManageUserRight.dwAddRight = dwAddRight;
	ManageUserRight.dwRemoveRight = dwRemoveRight;

	//发送请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MANAGE_USER_RIGHT, ManageUserRight.dwUserID, 0, 0, &ManageUserRight,sizeof(ManageUserRight));

	return true;
}

//用户冻结
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

//用户财富
VOID CAttemperEngineSink::OnEventUserItemWealth(IServerUserItem* pIServerUserItem, tagWealthVariation& WealthVariation)
{
	//效验参数
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return;

	//同步客户端 
	if (WealthVariation.cbSyncTarget & SYNC_TARGET_CLIENT)
	{		
		//缓冲定义
		BYTE cbBuffer[1024] = { 0 };

		//构造财富
		CMD_CM_WealthUpdate* pWealthUpdate = (CMD_CM_WealthUpdate*)cbBuffer;
		ZeroMemory(pWealthUpdate, sizeof(CMD_CM_WealthUpdate));

		//设置变量
		pWealthUpdate->wItemCount = 1;
		pWealthUpdate->WealthItem[0].lBalance = WealthVariation.lBalance;
		pWealthUpdate->WealthItem[0].wMoneyID = WealthVariation.wMoneyID;

		//发送消息
		WORD wHeadSize = sizeof(CMD_CM_WealthUpdate);
		WORD wDataSize = pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]);
		SendDataToUser(pIServerUserItem, MDM_CM_SYSTEM, SUB_CM_WEALTH_UPDATE, pWealthUpdate, wHeadSize + wDataSize);
	}	

	//同步数据库
	if (WealthVariation.cbSyncTarget & SYNC_TARGET_DATABASE)
	{
		//变量定义
		DBR_GR_WriteGameWealth WriteGameWealth;
		ZeroMemory(&WriteGameWealth, sizeof(WriteGameWealth));

		//财富信息
		WriteGameWealth.dwUserID = pIServerUserItem->GetUserID();
		WriteGameWealth.wReason = WealthVariation.wReason;
		WriteGameWealth.wMoneyID = WealthVariation.wMoneyID;
		WriteGameWealth.lVariationScore = WealthVariation.lVariationScore;

		//投递请求
		if (pIServerUserItem->IsRobotUser() == false)
		{
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_USER_WEALTH, WriteGameWealth.dwUserID, pIServerUserItem->GetBindIndex(), pIServerUserItem->GetTokenID(), &WriteGameWealth, sizeof(WriteGameWealth));
		}
	}

}

//用户等级
VOID CAttemperEngineSink::OnEventUserUserLevel(IServerUserItem* pIServerUserItem, BYTE cbLastLevelID, BYTE cbCurrLevelID)
{
	//效验参数
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return;

	//缓冲定义
	BYTE cbBuffer[SOCKET_PACKET] = { 0 };

	//获取对象
	auto  pServerUserPack = pIServerUserItem->GetUserPack();

	//构造对象
	CMD_GR_UserLevelUpgrade* pUserLevelUpgrade = (CMD_GR_UserLevelUpgrade*)cbBuffer;
	CSendPacketHelper SendPacket(cbBuffer + sizeof(CMD_GR_UserLevelUpgrade), sizeof(cbBuffer) - sizeof(CMD_GR_UserLevelUpgrade));

	//循环发送
	for (auto LevelID = cbLastLevelID + 1; LevelID <= cbCurrLevelID; LevelID++)
	{
		//清除数据
		SendPacket.CleanData();

		//查询奖励
		auto pUserLevelItem = g_GameLevelManager->SearchUserLevel(LevelID);
		if (pUserLevelItem != NULL)
		{
			//设置变量
			pUserLevelUpgrade->cbMemberOrder = LevelID;
			pUserLevelUpgrade->dwUserID = pIServerUserItem->GetUserID();
			pUserLevelUpgrade->dwMemberPoint = pUserLevelItem->lNeedPoint;

			//调整点数
			if (LevelID == cbCurrLevelID)
			{
				pUserLevelUpgrade->dwMemberPoint = pIServerUserItem->GetMemberPoint();
			}

			//奖励信息
			SendPacket.AddPacket(pUserLevelItem->szRewards, DTP_GR_GOODSLIST);

			//物品列表
			CGoodsItemArray  MoneyGoodsList;
			CGoodsItemArray  NormalGoodsList;

			//解析物品
			if (g_GameGoodsManager->ParseFromJson(pUserLevelItem->szRewards, MoneyGoodsList, NormalGoodsList))
			{
				//开始事务
				pServerUserPack->BeginTrans();

				//剩余物品
				WORD wResidueCount = 0;
				tagGoodsItem ResidueGoodsList[12] = { 0 };

				//加入背包
				for (auto i = 0; i < NormalGoodsList.GetCount(); i++)
				{
					auto pGoodsItem = &NormalGoodsList[i];
					pServerUserPack->AppendGoods(pGoodsItem->wGoodsID, pGoodsItem->dwGoodsCount, pGoodsItem->wGoodsLevel, pGoodsItem->dwGoodsIndate, GOODS_REASON_USERLEVEL_UPGRADE);
				}

				//提交事务
				pServerUserPack->SubmitTrans();

				//添加财富
				for (auto i = 0; i < MoneyGoodsList.GetCount(); i++)
				{
					pIServerUserItem->WriteUserWealth(MoneyGoodsList[i].wGoodsID, MoneyGoodsList[i].dwGoodsCount, GOODS_REASON_USERLEVEL_UPGRADE);
				}				
			}

			//发送数据
			WORD wDataSize = sizeof(CMD_GR_UserLevelUpgrade) + SendPacket.GetDataSize();
			SendDataToUser(pIServerUserItem, MDM_GR_USER, SUB_GR_USERLEVEL_UPGRADE, pUserLevelUpgrade, wDataSize);
		}
	}

	return;
}

//游戏等级
VOID CAttemperEngineSink::OnEventUserGameLevel(IServerUserItem* pIServerUserItem, BYTE cbLastLevelID, BYTE cbCurrLevelID)
{
	//效验参数
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return;

	//缓冲定义
	BYTE cbBuffer[SOCKET_PACKET] = { 0 };

	//获取对象
	auto  pServerUserPack = pIServerUserItem->GetUserPack();

	//构造对象
	CMD_GR_GameLevelUpgrade* pGameLevelUpgrade = (CMD_GR_GameLevelUpgrade*)cbBuffer;
	CSendPacketHelper SendPacket(pGameLevelUpgrade + 1, sizeof(cbBuffer) - sizeof(CMD_GR_GameLevelUpgrade));

	//循环发送
	for (auto LevelID = cbLastLevelID + 1; LevelID <= cbCurrLevelID; LevelID++)
	{
		//查询奖励
		auto pGameLevelItem = g_GameLevelManager->SearchGameLevel(LevelID);
		if (pGameLevelItem != NULL)
		{
			//设置变量
			pGameLevelUpgrade->cbGameOrder = LevelID;
			pGameLevelUpgrade->dwUserID = pIServerUserItem->GetUserID();
			pGameLevelUpgrade->lExperience = pGameLevelItem->lNeedPoint;

			//调整点数
			if (LevelID == cbCurrLevelID)
			{
				pGameLevelUpgrade->lExperience = pIServerUserItem->GetExperience();
			}

			//奖励信息
			SendPacket.AddPacket(pGameLevelItem->szRewards, DTP_GR_GOODSLIST);

			//物品列表
			CGoodsItemArray  MoneyGoodsList;
			CGoodsItemArray  NormalGoodsList;

			//解析物品
			if (g_GameGoodsManager->ParseFromJson(pGameLevelItem->szRewards, MoneyGoodsList, NormalGoodsList))
			{
				//开始事务
				pServerUserPack->BeginTrans();

				//剩余物品
				WORD wResidueCount = 0;
				tagGoodsItem ResidueGoodsList[12] = { 0 };

				//加入背包
				for (auto i = 0; i < NormalGoodsList.GetCount(); i++)
				{
					auto pGoodsItem = &NormalGoodsList[i];
					pServerUserPack->AppendGoods(pGoodsItem->wGoodsID, pGoodsItem->dwGoodsCount, pGoodsItem->wGoodsLevel, pGoodsItem->dwGoodsIndate, GOODS_REASON_GAMELEVEL_UPGRADE);			
				}

				//提交事务
				pServerUserPack->SubmitTrans();

				//添加财富
				for (auto i = 0; i < MoneyGoodsList.GetCount(); i++)
				{
					pIServerUserItem->WriteUserWealth(MoneyGoodsList[i].wGoodsID, MoneyGoodsList[i].dwGoodsCount, GOODS_REASON_GAMELEVEL_UPGRADE);
				}				
			}

			//发送数据
			WORD wDataSize = sizeof(CMD_GR_GameLevelUpgrade) + SendPacket.GetDataSize();
			SendDataToUser(pIServerUserItem, MDM_GR_USER, SUB_GR_GAMELEVEL_UPGRADE, pGameLevelUpgrade, wDataSize);
		}
	}

	return;
}

//用户背包
VOID CAttemperEngineSink::OnEventUserItemPack(IServerUserItem* pIServerUserItem, WORD wGoodsIDList[], WORD wGoodsCount)
{
	//效验参数
	ASSERT (pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return;

	//用户背包
	auto pServerUserPack = pIServerUserItem->GetUserPack();

	//缓冲定义
	BYTE cbBuffer[SOCKET_PACKET] = { 0 };

	//构造对象
	auto pUserGoodsUpdate = (CMD_GR_UserGoodsUpdate*)cbBuffer;
	ZeroMemory(pUserGoodsUpdate, sizeof(CMD_GR_UserGoodsUpdate));

	//设置数量
	pUserGoodsUpdate->wGoodsCount = wGoodsCount;

	//打包物品
	for (WORD i = 0; i < wGoodsCount; i++)
	{
		auto pGoodsHoldInfo = pServerUserPack->SearchGoods(wGoodsIDList[i]);
		if (pGoodsHoldInfo != NULL)
		{
			CopyMemory(&pUserGoodsUpdate->GoodsHoldInfo[i], pGoodsHoldInfo, sizeof(pUserGoodsUpdate->GoodsHoldInfo[0]));
		}
		else
		{
			//查询物品
			auto GameGoodsItemPtr = g_GameGoodsManager->SearchGoodsItem<CGameGoodsItemPtr>(wGoodsIDList[i]);
			if (GameGoodsItemPtr)
			{
				pUserGoodsUpdate->GoodsHoldInfo[i].wGoodsLevel = 1;
				pUserGoodsUpdate->GoodsHoldInfo[i].wGoodsID = GameGoodsItemPtr->GetGoodsID();
				pUserGoodsUpdate->GoodsHoldInfo[i].wGoodsKind = GameGoodsItemPtr->GetGoodsKind();
			}
		}
	}

	//发送数据
	WORD wHeadSize = sizeof(CMD_GR_UserGoodsUpdate);
	WORD wDataSize = pUserGoodsUpdate->wGoodsCount * sizeof(pUserGoodsUpdate->GoodsHoldInfo[0]);
	SendDataToUser(pIServerUserItem, MDM_CM_SYSTEM, SUB_CM_GOODS_UPDATE, pUserGoodsUpdate, wHeadSize + wDataSize);

	//更新入库
	if (pServerUserPack->IsSubmitVariation() == true)
	{
		//构造对象
		DBR_GR_UserUpdateGoods UserUpdateGoods;
		ZeroMemory(&UserUpdateGoods, sizeof(UserUpdateGoods));

		//用户标识
		UserUpdateGoods.dwUserID = pIServerUserItem->GetUserID();

		//提取数据
		if (pServerUserPack->DistillVariationGoods(UserUpdateGoods.szGoodsList, CountArray(UserUpdateGoods.szGoodsList)))
		{
			//投递数据	
			WORD wDataSize = CountStringBuffer(UserUpdateGoods.szGoodsList);
			WORD wHeadSize = sizeof(UserUpdateGoods) - sizeof(UserUpdateGoods.szGoodsList);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_UPDATE_GOODS, UserUpdateGoods.dwUserID, pIServerUserItem->GetBindIndex(), pIServerUserItem->GetTokenID(), &UserUpdateGoods, wHeadSize + wDataSize);
		}

		//构造对象
		DBR_GR_UserRecordGoods UserRecordGoods;
		ZeroMemory(&UserRecordGoods, sizeof(UserRecordGoods));

		//用户标识
		UserRecordGoods.dwUserID = pIServerUserItem->GetUserID();

		//提取数据
		if (pServerUserPack->DistillVariationRecord(UserRecordGoods.szRecordList, CountArray(UserRecordGoods.szRecordList)))
		{
			//投递数据	
			WORD wDataSize = CountStringBuffer(UserRecordGoods.szRecordList);
			WORD wHeadSize = sizeof(UserRecordGoods) - sizeof(UserRecordGoods.szRecordList);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_RECORD_GOODS, UserRecordGoods.dwUserID, pIServerUserItem->GetBindIndex(), pIServerUserItem->GetTokenID(), &UserRecordGoods, wHeadSize + wDataSize);
		}

		//清除变更
		pServerUserPack->CleanVariation();
	}
}

//获取状态
WORD CAttemperEngineSink::GetServiceStatus()
{
	return CServiceUnits::g_pServiceUnits->GetServiceStatus();
}

//发送数据
bool CAttemperEngineSink::SendBatchData(BYTE cbSendMask,WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//机器数据
	if ((cbSendMask&BG_COMPUTER)!=0)
	{
		m_RobotUserManager.SendDataToClient(wMainCmdID,wSubCmdID,pData,wDataSize);
	}

	//变量定义
	WORD wEnumIndex = 0;
	IServerUserItem * pIServerUserItem = NULL;
	CGlobalGateItem * pGlobalGateItem = NULL;

	do
	{
		//枚举玩家
		pIServerUserItem = m_ServerUserManager.EnumUserItem(wEnumIndex++);
		if (pIServerUserItem == NULL) break;
		if (pIServerUserItem->GetBindIndex() == INVALID_WORD_INDEX) continue;

		//过滤玩家
		if (cbSendMask != BG_ALL_CLIENT)
		{
			if ((cbSendMask&BG_MOBILE) != 0 && !pIServerUserItem->IsMobileUser()) continue;
			if ((cbSendMask&BG_COMPUTER) != 0 && (pIServerUserItem->IsMobileUser() || pIServerUserItem->IsRobotUser())) continue;
		}

		//获取网关
		tagBindParameter * pBindParameter = (m_pNormalParameter + pIServerUserItem->GetBindIndex());

		//直连
		if (pBindParameter->wServiceModule == SERVICE_MODULE_NONE)
		{
			//关闭网络
			if (wSubCmdID == SUB_CS_S_SHUTDOWN_SOCKET)
			{
				m_pITCPNetworkEngine->ShutDownSocket(pBindParameter->dwContextID);
			}
			else
			{
				//发送数据
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

	//变量定义		
	DWORD * pdwTokenIDArray = NULL;
	WORD wCellCount = 0, wSendCount = 0, wTotalCount = 0;

	//设置变量	
	pGlobalGateItem = NULL;

	do
	{
		//枚举网关
		pGlobalGateItem = m_GlobalItemManager.EnumGateItem(pGlobalGateItem);
		if (pGlobalGateItem == NULL) break;

		//校验数目
		if (pGlobalGateItem->m_TokenIDArray.GetCount() == 0) continue;

		//设置变量		
		wSendCount = 0;
		pdwTokenIDArray = pGlobalGateItem->m_TokenIDArray.GetData();
		wTotalCount = wCellCount = pGlobalGateItem->m_TokenIDArray.GetCount();

		//切分数量
		while (wCellCount > TOKEN_COUNT) wCellCount = wCellCount / 2;

		//循环发送
		while (wSendCount < wTotalCount)
		{
			//余量调整
			if (wTotalCount - wSendCount < wCellCount)
			{
				wCellCount = wTotalCount - wSendCount;
			}

			//发送数据
			m_pITCPNetworkEngine->SendData(pGlobalGateItem->GetSocketID(), MAKECMMD(wMainCmdID, SERVICE_MODULE_GAME), wSubCmdID, pData, wDataSize, pdwTokenIDArray + wSendCount, wCellCount);

			//累计数量
			wSendCount += wCellCount;
		}		

		//清除令牌
		pGlobalGateItem->m_TokenIDArray.RemoveAll();

	} while (true);

	return true;
}

//发送数据
bool CAttemperEngineSink::SendDebugData(IServerUserItem* pIServerUserItem, WORD wTableID, WORD wDebugKind, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	//缓冲定义
	BYTE cbBuffer[SOCKET_PACKET] = {0};

	//构造结构
	auto pDebugPacket = (CMD_GR_DebugPacket*)cbBuffer;

	//设置变量
	pDebugPacket->wTableID = wTableID;
	pDebugPacket->wDebugKind = wDebugKind;
	pDebugPacket->wGameCmdID = wSubCmdID;

	//溢出校验
	if (wDataSize + sizeof(CMD_GR_DebugPacket) > sizeof(cbBuffer))
	{
		return false;
	}

	//拷贝数据
	if (wDataSize > 0)
	{
		CopyMemory(pDebugPacket + 1, pData, wDataSize);
	}

	//数据大小
	WORD wPacketSize = sizeof(CMD_GR_DebugPacket) + wDataSize;

	//校验用户
	if (pIServerUserItem != NULL )
	{
		if (CUserRight::CanCheat(pIServerUserItem->GetUserRight()))
		{
			SendDataToUser(pIServerUserItem,MDM_GR_DEBUG,SUB_GR_DEBUG_PACKET, pDebugPacket, wPacketSize);
		}
	}
	else
	{
		//变量定义
		WORD wEnumIndex = 0;

		do
		{
			//过滤用户
			auto pIServerUserItem = m_ServerUserManager.EnumUserItem(wEnumIndex++);
			if (pIServerUserItem == NULL) break;

			//校验状态
			if (pIServerUserItem->IsMobileUser() || pIServerUserItem->IsRobotUser()) continue;

			//校验权限
			if (CUserRight::CanCheat(pIServerUserItem->GetUserRight()))
			{
				SendDataToUser(pIServerUserItem, MDM_GR_DEBUG, SUB_GR_DEBUG_PACKET, pDebugPacket, wPacketSize);
			}

		} while (true);
	}		

	return true;
}

//发送数据
bool CAttemperEngineSink::SendDataToGate(DWORD dwSocketID, DWORD dwTokenID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//获取参数
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);

	//发送数据
	if (wBindIndex >=INDEX_ROBOT)
	{
		//机器用户
		m_RobotUserManager.SendDataToClient(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize);
	}
	else 
	{
		//直连模式
		if (pBindParameter->wServiceModule == SERVICE_MODULE_NONE)
		{
			if (wSubCmdID == SUB_CS_S_SHUTDOWN_SOCKET)
			{
				//关闭连接
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);
			}
			else
			{
				//发送数据
				m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID, pData, wDataSize, &dwTokenID, 0);
			}
		}
		else
		{			
			//发送数据
			m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(wMainCmdID,SERVICE_MODULE_GAME), wSubCmdID, pData, wDataSize, &dwTokenID, dwTokenID ? 1 : 0);
		}
	}

	return true;
}

//发送数据
bool CAttemperEngineSink::SendDataToUser(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//校验索引
	if (pIServerUserItem->GetBindIndex()==INVALID_WORD_INDEX) return false;

	//获取参数
	DWORD dwTokenID = pIServerUserItem->GetTokenID();
	tagBindParameter * pBindParameter = GetBindParameter(pIServerUserItem->GetBindIndex());

	//发送数据
	if (pIServerUserItem->IsRobotUser()==true)
	{
		//机器用户			
		m_RobotUserManager.SendDataToClient(pBindParameter->dwContextID,wMainCmdID,wSubCmdID,pData,wDataSize);
	}
	else
	{
		//关闭网络
		if (pBindParameter->wServiceModule == SERVICE_MODULE_NONE)
		{
			if (wSubCmdID == SUB_CS_S_SHUTDOWN_SOCKET)
			{
				//关闭连接
				m_pITCPNetworkEngine->ShutDownSocket(pBindParameter->dwContextID);
			}
			else
			{
				//发送数据
				m_pITCPNetworkEngine->SendData(pBindParameter->dwContextID, wMainCmdID, wSubCmdID, pData, wDataSize, &dwTokenID, 0);
			}
		}
		else
		{
			//常规用户							
			m_pITCPNetworkEngine->SendData(pBindParameter->dwContextID, MAKECMMD(wMainCmdID,SERVICE_MODULE_GAME),wSubCmdID, pData, wDataSize, &dwTokenID, dwTokenID ? 1 : 0);
		}
	}

	return true;
}

//发送数据
bool CAttemperEngineSink::SendDataToCenter(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	ASSERT(m_pITCPSocketCenter != NULL);
	return m_pITCPSocketCenter->SendData(wMainCmdID, wSubCmdID, pData, wDataSize,0);
}

//群发数据
bool CAttemperEngineSink::SendDataToTable(WORD wCmdTable, BYTE cbClientKind, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//校验参数
	//ASSERT(wCmdTable < m_TableFrameArray.GetCount());
	if (wCmdTable >= m_TableFrameArray.GetCount()) return false;

	//枚举用户
	WORD wEnumIndex=0;
	IServerUserItem * pITableUserItem = NULL;
	CTableFrameItem* pTableFrame = m_TableFrameArray[wCmdTable];

	//在座用户
	for (int i = 0; i < pTableFrame->GetChairCount(); i++)
	{
		pITableUserItem = pTableFrame->GetTableUserItem(i);
		if (pITableUserItem == NULL) continue;	

		//扩展参数
		auto pExtendParameter = GetExtendParameter(pITableUserItem->GetBindIndex(), pITableUserItem->GetTokenID());
		if (pExtendParameter != NULL && (pExtendParameter->cbClientKind & cbClientKind) != 0)
		{
			SendDataToUser(pITableUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
		}
	}

	//旁观用户
	do
	{
		//枚举用户
		pITableUserItem = pTableFrame->EnumLookonUserItem(wEnumIndex++);
		if (pITableUserItem == NULL) break;		
		
		//扩展参数
		auto pExtendParameter = GetExtendParameter(pITableUserItem->GetBindIndex(), pITableUserItem->GetTokenID());
		if (pExtendParameter != NULL && (pExtendParameter->cbClientKind & cbClientKind) != 0)
		{			
			SendDataToUser(pITableUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
		}

	} while (true);

	return true;
}

//桌子锁事件
VOID CAttemperEngineSink::OnEventTableLocked( ITableFrameItem *pITableFrame, BOOL bTableLocked )
{
	//效验
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

//桌子状态事件
VOID CAttemperEngineSink::OnEventTableStatus( ITableFrameItem *pITableFrame, BOOL bPlayStatus )
{
	//效验
	ASSERT(pITableFrame);

	if( !bPlayStatus )
	{
		//判断锁桌
		if( pITableFrame->IsTableLocked() )
		{
			//重新记时
			m_LockedTableTimes[pITableFrame->GetTableID()] = m_dwRegularTimes+m_dwLockTableTime;
		}
	}
	else
	{
		//删除记时
		m_LockedTableTimes.RemoveKey(pITableFrame->GetTableID());
	}

	return;
}

//离线写分请求
VOID CAttemperEngineSink::OnTableReqOfflineScore( tagOfflineScore const &OfflineScore )
{
	//查询玩家是否在线
	IServerUserItem *pUserItem = m_ServerUserManager.SearchUserItem(OfflineScore.dwUserID);
	
	//如果玩家在线
	if (pUserItem)
	{
		//解冻锁分
		pUserItem->FreezeUserScore(OfflineScore.lUnLockScore,true);

		//写分
		pUserItem->WriteUserScore(OfflineScore.lScore, OfflineScore.lWinScore, OfflineScore.lBetAmount, OfflineScore.lRevenue,OfflineScore.ScoreType);

		return;
	}

	//解冻锁分记录
	SCORE lFreezeScore = 0;
	if( m_FreezeScoreMap.Lookup(OfflineScore.dwUserID,lFreezeScore) )
	{
		if( lFreezeScore == OfflineScore.lUnLockScore )
			m_FreezeScoreMap.RemoveKey(OfflineScore.dwUserID);
		else m_FreezeScoreMap[OfflineScore.dwUserID] = lFreezeScore-OfflineScore.lUnLockScore;
	}

	//写分请求
	DBR_GR_WriteGameScore WriteGameScore;
	ZeroMemory(&WriteGameScore,sizeof(WriteGameScore));

	//用户信息
	WriteGameScore.dwUserID=OfflineScore.dwUserID;
	WriteGameScore.dwClientIP=OfflineScore.dwClientIP;
	WriteGameScore.dwPlayTimeCount=OfflineScore.dwPlayTimeCount;

	//积分信息
	WriteGameScore.lRevenue=OfflineScore.lRevenue;
	WriteGameScore.lUnLockScore=OfflineScore.lUnLockScore;
	WriteGameScore.lScore=OfflineScore.lScore;
	WriteGameScore.lBetAmount=OfflineScore.lBetAmount;
	WriteGameScore.lWinCount=OfflineScore.lWinCount;
	WriteGameScore.lLostCount=OfflineScore.lLostCount;
	WriteGameScore.lDrawCount=OfflineScore.lDrawCount;
	WriteGameScore.lFleeCount=OfflineScore.lFleeCount;
	WriteGameScore.lExperience=OfflineScore.lExperience;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_GAME_SCORE, WriteGameScore.dwUserID,0L,0L,&WriteGameScore,sizeof(WriteGameScore));

	return;
}

//解冻锁分请求
VOID CAttemperEngineSink::OnTableReqUnfreezeScore( DWORD dwUserID, SCORE lUnfreezeScore )
{
	//查询玩家是否在线
	IServerUserItem *pUserItem = m_ServerUserManager.SearchUserItem(dwUserID);

	//如果在线
	if( pUserItem )
	{
		pUserItem->FreezeUserScore(-lUnfreezeScore,true);
		return;
	}

	//否则解冻锁分
	SCORE lFreezeScore = 0;
	if( m_FreezeScoreMap.Lookup(dwUserID,lFreezeScore) )
	{
		if( lFreezeScore == lUnfreezeScore )
			m_FreezeScoreMap.RemoveKey(dwUserID);
		else m_FreezeScoreMap[dwUserID] = lFreezeScore-lUnfreezeScore;

		return;
	}

	//找不到锁分记录
	ASSERT(FALSE);
	return;
}


//房间消息
bool CAttemperEngineSink::SendRoomMessage(DWORD dwSocketID, DWORD dwTokenID, LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	StringCchCopy(SystemMessage.szString,CountArray(SystemMessage.szString), lpszMessage);

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	SendDataToGate(dwSocketID, dwTokenID, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wSendSize);

	//关闭网络
	if ((wType&(SMT_CLOSE_LINK | SMT_CLOSE_ROOM)) != 0)
	{
		//构造结构
		CMD_CS_C_ShutdownSocket ShutdownSocket;
		ShutdownSocket.bForceClose = false;

		//发送数据
		SendDataToGate(dwSocketID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));
	}

	return true;
}

//房间消息
bool CAttemperEngineSink::SendRoomMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	StringCchCopy(SystemMessage.szString,CountArray(SystemMessage.szString), lpszMessage);

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据		
	SendBatchData(BG_COMPUTER,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);

	//关闭网络
	if ((wType&(SMT_CLOSE_LINK | SMT_CLOSE_ROOM)) != 0)
	{
		//构造结构
		CMD_CS_C_ShutdownSocket ShutdownSocket;
		ShutdownSocket.bForceClose = false;

		//批量发送
		SendBatchData(BG_COMPUTER,MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));		
	}

	return true;
}

//游戏消息
bool CAttemperEngineSink::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
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

//房间消息
bool CAttemperEngineSink::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//校验索引
	if (pIServerUserItem->GetBindIndex() == INVALID_WORD_INDEX) return false;

	//变量定义
	CMD_CM_SystemMessage SystemMessage;
	ZeroMemory(&SystemMessage,sizeof(SystemMessage));

	//构造数据
	SystemMessage.wType=wType;
	SystemMessage.wLength=lstrlen(lpszMessage)+1;
	StringCchCopy(SystemMessage.szString,CountArray(SystemMessage.szString), lpszMessage);

	//数据属性
	WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
	WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

	//发送数据
	SendDataToUser(pIServerUserItem, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wSendSize);

	//关闭网络
	if ((wType&(SMT_CLOSE_LINK | SMT_CLOSE_ROOM)) != 0)
	{
		//构造结构
		CMD_CS_C_ShutdownSocket ShutdownSocket;
		ShutdownSocket.bForceClose = false;

		//发送数据
		SendDataToUser(pIServerUserItem, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));		
	}

	return true;
}

//游戏消息
bool CAttemperEngineSink::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//效验参数
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//校验索引
	if (pIServerUserItem->GetBindIndex() == INVALID_WORD_INDEX) return false;

	//发送数据
	if (pIServerUserItem->IsClientReady()==true)
	{
		//变量定义
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//构造数据
		SystemMessage.wType=wType;
		SystemMessage.wLength=lstrlen(lpszMessage)+1;
		StringCchCopy(SystemMessage.szString,CountArray(SystemMessage.szString), lpszMessage);

		//数据属性
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//发送数据
		SendDataToUser(pIServerUserItem, MDM_GF_FRAME, SUB_GF_SYSTEM_MESSAGE, &SystemMessage, wSendSize);		

		//关闭网络
		if ((wType&(SMT_CLOSE_LINK | SMT_CLOSE_ROOM)) != 0)
		{
			//构造结构
			CMD_CS_C_ShutdownSocket ShutdownSocket;
			ShutdownSocket.bForceClose = false;

			//发送数据
			SendDataToUser(pIServerUserItem, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));
		}

		return true;
	}

	return false;
}

//跑马灯公告
bool CAttemperEngineSink::SendMarqeeNotice(BYTE cbNoticeKind, VOID* pNoticeContent, WORD wNoticeSize)
{
	//查找配置
	auto MarqueeNoticeOptionPtr = g_GlobalOptionManager->GetOptionItem<CMarqueeNoticeOptionPtr>(OPTION_MASK_MARQUEE_NOTICE);
	if (!MarqueeNoticeOptionPtr) return false;

	//滚动配置
	auto pNoticeRollOption = MarqueeNoticeOptionPtr->GetNoticeRollOption(cbNoticeKind);
	if (!pNoticeRollOption) return false;

	//缓冲定义
	BYTE cbBuffer[1024];

	//构造结构
	tagForwardHead* pForwadHead = (tagForwardHead*)cbBuffer;
	auto pMarqueeNotice = (CMD_CS_MarqueeNotice*)(pForwadHead + 1);

	//转发设置
	pForwadHead->wItemCount = 1;
	pForwadHead->wTarget = FORWARD_TARGET_SERVICE;	
	pForwadHead->ForwardItem[0].wServiceID = 0;
	pForwadHead->ForwardItem[0].wServiceKind = 0;
	pForwadHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;

	//设置数据
	pMarqueeNotice->dwNoticeID = GetTickCount();
	pMarqueeNotice->bExclusive = pNoticeRollOption->bExclusive;
	pMarqueeNotice->cbPriority = pNoticeRollOption->cbPriority;
	pMarqueeNotice->wRollTimes = pNoticeRollOption->wRollTimes;
	pMarqueeNotice->wViewPlace = pNoticeRollOption->wViewPlace;
	pMarqueeNotice->cbNoticeKind = pNoticeRollOption->cbNoticeKind;
	pMarqueeNotice->dwStartRollTime = (DWORD)time(NULL);
	pMarqueeNotice->dwEndRollTime = pMarqueeNotice->dwStartRollTime + MarqueeNoticeOptionPtr->GetDurationTime();

	//公告内容
	pMarqueeNotice->wNoticeSize = wNoticeSize;
	CopyMemory(pMarqueeNotice->cbNoticeContent, pNoticeContent, pMarqueeNotice->wNoticeSize);

	//发送数据
	WORD wPacketSize = sizeof(CMD_CS_MarqueeNotice) - sizeof(pMarqueeNotice->cbNoticeContent) + pMarqueeNotice->wNoticeSize;
	SendDataToCenter(MDM_CS_FORWARD_SERVICE, SUB_CS_MARQUEE_NOTICE, cbBuffer, sizeof(tagForwardHead) + wPacketSize);

	return true;
}

//注册服务
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_REGISTER_SERVICE:	//注册服务
		{
			//效验数据
			ASSERT(wDataSize == sizeof(CMD_CS_C_RegisterService));
			if (wDataSize != sizeof(CMD_CS_C_RegisterService)) return false;

			//消息定义
			CMD_CS_C_RegisterService * pRegisterService = (CMD_CS_C_RegisterService *)pData;

			//校验模块
			if (pRegisterService->wServiceModule != SERVICE_MODULE_GATE) return false;

			//查找路由
			if (m_GlobalItemManager.SearchGateItem(pRegisterService->wServiceID) != NULL)
			{
				//变量定义
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure, sizeof(RegisterFailure));

				//设置变量
				RegisterFailure.lErrorCode = 0;
				_sntprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), TEXT("已经存在相同标识的%s，服务注册失败"), pRegisterService->szServiceName);

				//发送消息
				WORD wStringSize = CountStringBuffer(RegisterFailure.szDescribeString);
				WORD wSendSize = sizeof(RegisterFailure) - sizeof(RegisterFailure.szDescribeString) + wStringSize;
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_FAILURE, &RegisterFailure, wSendSize, NULL, 0);

				//中断网络
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//设置绑定
			WORD wBindIndex = LOWORD(dwSocketID);
			tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);

			//设置参数
			pBindParameter->dwServiceID = pRegisterService->wServiceID;
			pBindParameter->wServiceModule = pRegisterService->wServiceModule;

			//变量定义
			tagServiceItem ServiceItem;
			ZeroMemory(&ServiceItem, sizeof(ServiceItem));

			//构造结构
			ServiceItem.wServiceID = pRegisterService->wServiceID;
			ServiceItem.wServicePort = pRegisterService->wServicePort;
			ServiceItem.wMaxConnect = pRegisterService->wMaxConnect;
			ServiceItem.wServiceModule = pRegisterService->wServiceModule;
			ServiceItem.dwServiceAddr = pRegisterService->dwServiceAddr;
			StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), pRegisterService->szServiceName);
			StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain), pRegisterService->szServiceDomain);

			//注册网关
			pBindParameter->pGlobalGateItem = m_GlobalItemManager.ActiveGateItem(dwSocketID, ServiceItem);
			if (pBindParameter->pGlobalGateItem == NULL)
			{
				//变量定义
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure, sizeof(RegisterFailure));

				//设置变量
				RegisterFailure.lErrorCode = 0;
				StringCchCopy(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), TEXT("由于未知原因，服务注册失败！"));

				//发送消息
				WORD wStringSize = CountStringBuffer(RegisterFailure.szDescribeString);
				WORD wSendSize = sizeof(RegisterFailure) - sizeof(RegisterFailure.szDescribeString) + wStringSize;
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_FAILURE, &RegisterFailure, wSendSize, NULL, 0);

				//中断网络
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//发送数据
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_SUCCESS, NULL, 0);

			return true;
		}
	}

	return false;
}

//服务信息
bool CAttemperEngineSink::OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_LOGOUT_TOKEN:	//注销令牌
		{
			//设置绑定
			WORD wBindIndex = LOWORD(dwSocketID);
			auto pBindParameter = (m_pNormalParameter + wBindIndex);

			//校验服务
			if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return true;

			//提取数据
			auto pLogoutToken = (CMD_CS_C_LogoutToken *)pData;

			//登出处理
			return OnTCPNetworkSubLogonLogout(NULL,0, dwSocketID, pLogoutToken->dwTokenID);
		}
		case SUB_CS_C_REGISTER_TOKEN:	//注册令牌
		{
			//设置绑定
			WORD wBindIndex = LOWORD(dwSocketID);
			auto pBindParameter = (m_pNormalParameter + wBindIndex);

			//移除令牌
			if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return true;

			//获取对象
			auto pGlobalGateItem = pBindParameter->pGlobalGateItem;

			//提取数据
			CMD_CS_C_RegisterToken * pRegisterToken = (CMD_CS_C_RegisterToken *)pData;

			//构造结构
			CMD_CS_S_RegisterToken RegisterToken;
			ZeroMemory(&RegisterToken,sizeof(RegisterToken));

			//默认失败
			RegisterToken.lResultCode = REGISTER_RESULT_FAILURE;

			//校验数量
			DWORD dwTokenCount = CollectTokenCount();
			if (dwTokenCount >= m_pGameServiceOption->wMaxPlayer)
			{
				RegisterToken.lResultCode = REGISTER_RESULT_LINKFULL;
			}
			else
			{
				//查询用户
				auto pTokenParameter = pGlobalGateItem->ActiveTokenID(pRegisterToken->dwTokenID);
				if (pTokenParameter != NULL)
				{
					//注册成功
					RegisterToken.lResultCode = REGISTER_RESULT_SUCCESS;

					//设置地址
					pTokenParameter->dwUserID = pRegisterToken->dwUserID;
					pTokenParameter->dwClientIP = pRegisterToken->dwClientIP;					
				}
			}		

			//发送数据
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_REGISTER_TOKEN, &RegisterToken, sizeof(RegisterToken), &pRegisterToken->dwTokenID, 1);

			return true;
		}
	}

	return true;
}

//调试处理
bool CAttemperEngineSink::OnTCPNetworkMainDebug(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_DEBUG_PACKET:		//调试数据包
	{
		return OnTCPNetworkSubDebugPacket(pData, wDataSize, dwSocketID, dwTokenID);
	}
	}

	return false;
}

//登录处理
bool CAttemperEngineSink::OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_LOGON_LOGOUT:		//用户登出
		{
			return OnTCPNetworkSubLogonLogout(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_LOGON_MOBILE:		//手机登录
		{
			return OnTCPNetworkSubLogonMobile(pData, wDataSize, dwSocketID, dwTokenID);
		}
	case SUB_GR_LOGON_ROBOT:		//机器登录
		{
			return OnTCPNetworkSubLogonRobot(pData, wDataSize, dwSocketID, dwTokenID);
		}
	case SUB_GR_LOGON_COMPUTER:		//电脑登录
		{
			return OnTCPNetworkSubLogonComputer(pData,wDataSize,dwSocketID,dwTokenID);
		}	
	}

	return false;
}

//用户处理
bool CAttemperEngineSink::OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_USER_RULE:			//用户规则
		{
			return OnTCPNetworkSubUserRule(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_LOOKON:		//用户旁观
		{
			return OnTCPNetworkSubUserLookon(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_SITDOWN:		//用户坐下
		{
			return OnTCPNetworkSubUserSitDown(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_STANDUP:		//用户起立
		{
			return OnTCPNetworkSubUserStandUp(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_CHAT:			//用户聊天
		{
			return OnTCPNetworkSubUserChat(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_WISPER:		//用户私聊
	case SUB_GR_WHSPER_REPLY:		//自动回复
		{
			return OnTCPNetworkSubUserWisper(pData,wDataSize,dwSocketID,wSubCmdID,dwTokenID);
		}
	case SUB_GR_INVITE_USER:		//邀请用户
		{
			return OnSocketSubInviteUser(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_REPULSE_SIT:	//拒绝厌友
		{
			return OnTCPNetworkSubUserRepulseSit(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_REQUEST_INFO:  //请求信息
		{
			return OnTCPNetworkSubUserRequestInfo(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_SWITCH_TABLE:	//更换桌子
		{
 			return OnTCPNetworkSubUserSwitchTable(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_USER_DESKMATE_INFO: //同桌信息
		{
			return OnTCPNetworkSubUserDeskmateInfo(pData,wDataSize,dwSocketID,dwTokenID);
		}
	case SUB_GR_SEND_GIFT:			//赠送礼物
		{
			return OnTCPNetworkSubUserSendGift(pData, wDataSize, dwSocketID, dwTokenID);
		}
	}

	return false;
}

//管理处理
bool CAttemperEngineSink::OnTCPNetworkMainManage(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_KILL_USER:			//踢出用户
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_KillUser));
			if (wDataSize!=sizeof(CMD_GR_KillUser)) return false;

			//消息处理
			CMD_GR_KillUser * pKillUser=(CMD_GR_KillUser *)pData;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//权限判断
			ASSERT(CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==true);
			if (CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==false) return false;

			//目标用户
			IServerUserItem * pITargerUserItem=m_ServerUserManager.SearchUserItem(pKillUser->dwTargetUserID);
			if (pITargerUserItem==NULL) return true;

			//发送消息
			SendRoomMessage(pITargerUserItem, CWHService::FormatMessage(MIM_SERVER_0001, TEXT("抱歉，您被管理员踢出房间，需在被踢后一小时才能进入，若有任何疑问，请联系游戏客服！")),SMT_CHAT|SMT_EJECT|SMT_CLOSE_LINK|SMT_CLOSE_ROOM);

			return true;
		}
	case SUB_GR_DISMISSGAME:		//解散游戏
		{
			//参数校验
			ASSERT(wDataSize==sizeof(CMD_GR_DismissGame));
			if(wDataSize>sizeof(CMD_GR_DismissGame) ) return false;

			//提取数据
			CMD_GR_DismissGame * pDismissGame=(CMD_GR_DismissGame *)pData;
			if(pDismissGame==NULL) return false;
			
			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//权限判断
			ASSERT(CMasterRight::CanDismissGame(pIServerUserItem->GetMasterRight())==true);
			if (CMasterRight::CanDismissGame(pIServerUserItem->GetMasterRight())==false) return false;

			//桌号判断
			if(pDismissGame->wDismissTableNum<m_TableFrameArray.GetCount())
			{
				CTableFrameItem * pTableFrame=m_TableFrameArray[pDismissGame->wDismissTableNum];
				if ( !pTableFrame->IsGameStarted() ) return true;
				pTableFrame->DismissGame();
			}			
			
			return true;
		}	
	case SUB_GR_OPTION_SERVER:		//房间管理
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_OptionServer));
			if (wDataSize!=sizeof(CMD_GR_OptionServer)) return false;

			//消息处理
			CMD_GR_OptionServer * pOptionServer=(CMD_GR_OptionServer *)pData;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//权限判断
			ASSERT(CMasterRight::CanManagerServer(pIServerUserItem->GetMasterRight())==true);
			if (CMasterRight::CanManagerServer(pIServerUserItem->GetMasterRight())==false) return false;

			bool bNotify = false;

			//大厅聊天
			if (pOptionServer->cbOptionFlags==OSF_ROOM_CHAT)
			{
				g_GlobalUnits.SetAllowRoomChat((pOptionServer->cbOptionValue==TRUE)?true:false);

				bNotify = true;
				CServerRule::SetForbidRoomChat( CMD_ConfigServer.dwServerRule,(pOptionServer->cbOptionValue==TRUE)?false:true );
			}

			//游戏聊天
			if (pOptionServer->cbOptionFlags==OSF_GAME_CHAT)
			{
				g_GlobalUnits.SetAllowGameChat((pOptionServer->cbOptionValue==TRUE)?true:false);

				bNotify = true;
				CServerRule::SetForbidGameChat( CMD_ConfigServer.dwServerRule,(pOptionServer->cbOptionValue==TRUE)?false:true );
			}

			//大厅私聊
			if (pOptionServer->cbOptionFlags==OSF_ROOM_WISPER)
			{
				g_GlobalUnits.SetAllowWisper((pOptionServer->cbOptionValue==TRUE)?true:false);

				bNotify = true;
				CServerRule::SetForbidRoomWisper( CMD_ConfigServer.dwServerRule,(pOptionServer->cbOptionValue==TRUE)?false:true );
			}

			//进入游戏
			if (pOptionServer->cbOptionFlags==OSF_ENTER_GAME)
			{
				g_GlobalUnits.SetAllowEnterGame((pOptionServer->cbOptionValue==TRUE)?true:false);
			}

			//进入房间
			if (pOptionServer->cbOptionFlags==OSF_ENTER_ROOM)
			{
				g_GlobalUnits.SetAllowEnterRoom((pOptionServer->cbOptionValue==TRUE)?true:false);
			}

			//同桌游戏私聊
			if(pOptionServer->cbOptionFlags==OSF_PLAY_CHAT)
			{
				g_GlobalUnits.SetForbidPlayChat((pOptionServer->cbOptionValue==TRUE)?false:true);

				bNotify = true;
				CServerRule::SetForbidPlayChat( CMD_ConfigServer.dwServerRule,(pOptionServer->cbOptionValue==TRUE)?false:true );
			}

			//禁止远程搜索
			if (pOptionServer->cbOptionFlags == OSF_REMOTE_SEARCH )
			{
				g_GlobalUnits.SetForbidQueryUser((pOptionServer->cbOptionValue==TRUE)?false:true);

				bNotify = true;
				CServerRule::SetForbidQueryUser( CMD_ConfigServer.dwServerRule,(pOptionServer->cbOptionValue==TRUE)?false:true );
			}

			//会员可聊天
			if (pOptionServer->cbOptionFlags==OSF_MEMBER_CAN_CHAT)
			{
				g_GlobalUnits.SetMemberCanChat((pOptionServer->cbOptionValue==TRUE)?true:false);

				bNotify = true;
				CServerRule::SetMemberCanChat( CMD_ConfigServer.dwServerRule,(pOptionServer->cbOptionValue==TRUE)?true:false );
			}

			//禁止旁观
			if (pOptionServer->cbOptionFlags == OSF_LOOK_ON )
			{
				g_GlobalUnits.SetForbidLookon((pOptionServer->cbOptionValue==TRUE)?false:true);

				bNotify = true;
				CServerRule::SetForbidLookon( CMD_ConfigServer.dwServerRule,(pOptionServer->cbOptionValue==TRUE)?false:true );
			}

			//旁观聊天
			if (pOptionServer->cbOptionFlags == OSF_LOOK_ON_CHAT)
			{
				g_GlobalUnits.SetAllowLookOnChat(pOptionServer->cbOptionValue);
			}

			//机器人陪打
			if (pOptionServer->cbOptionFlags==OSF_ROBOT_ATTEND)
			{
				//设置权限
				bool bAllow = g_GlobalUnits.IsAllowRobotAttend();
				g_GlobalUnits.SetAllowRobotAttend((pOptionServer->cbOptionValue==TRUE)?true:false);

				//设置定时器
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

			//机器人占座
			if (pOptionServer->cbOptionFlags==OSF_ROBOT_SIMULATE)
			{
				//设置权限
				bool bAllow = g_GlobalUnits.IsAllowRobotSimulate();
				g_GlobalUnits.SetAllowRobotSimulate((pOptionServer->cbOptionValue==TRUE)?true:false);

				//设置定时器
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

			//设置自动关闭房间
			if (pOptionServer->cbOptionFlags == OSF_CLOSE_NOPLAYER)
			{
				g_GlobalUnits.SetCloseWhileNoplayer((pOptionServer->cbOptionValue==TRUE)?true:false);
			}
			
			//发送通知
			if (bNotify)
			{
				//构造数据
				CMD_GR_ConfigRule cr = {0};
				cr.dwServerRule = CMD_ConfigServer.dwServerRule;

				SendBatchData(BG_COMPUTER,MDM_GR_CONFIG,SUB_GR_CONFIG_RULE,&cr,sizeof(cr) );

				m_pGameServiceOption->dwServerRule|=CMD_ConfigServer.dwServerRule;
			}

			//发送消息
			SendRoomMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0004,TEXT("服务器房间配置参数选项设置成功")), SMT_CHAT);

			//更新配置
			ModifyGameServer();

			return true;
		}
	case SUB_GR_KICK_ALL_USER:		//踢出用户
		{
			//消息处理
			CMD_GR_KickAllUser * pKillAllUser=(CMD_GR_KickAllUser *)pData;
			
			//效验数据
			ASSERT(wDataSize<=sizeof(CMD_GR_KickAllUser));
			if( wDataSize > sizeof(CMD_GR_KickAllUser) ) return false;
			ASSERT(wDataSize==CountStringBuffer(pKillAllUser->szKickMessage));
			if (wDataSize!=CountStringBuffer(pKillAllUser->szKickMessage)) return false;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//权限判断
			ASSERT(CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==true);
			if (CMasterRight::CanKillUser(pIServerUserItem->GetMasterRight())==false) return false;

			//解散所有游戏
			for (INT_PTR i=0;i<m_TableFrameArray.GetCount();i++)
			{
				//获取桌子
				CTableFrameItem * pTableFrame=m_TableFrameArray[i];
				if ( !pTableFrame->IsGameStarted() ) continue;

				pTableFrame->DismissGame();
			}

			//变量定义
			WORD wEnumIndex = 0;
			IServerUserItem * pITargerUserItem = NULL;

			do
			{
				//枚举用户
				pITargerUserItem = m_ServerUserManager.EnumUserItem(wEnumIndex++);
				if (pITargerUserItem == NULL) break;
				if (pITargerUserItem != pIServerUserItem)
				{
					//发送消息
					SendRoomMessage(pITargerUserItem, pKillAllUser->szKickMessage, SMT_CHAT | SMT_EJECT | SMT_CLOSE_LINK | SMT_CLOSE_ROOM);
				}

			} while (true);

			return true;
		}
	case SUB_GR_SEND_MESSAGE:		//发布消息
		{
			//消息处理
			CMD_GR_SendMessage * pSendMessage=(CMD_GR_SendMessage *)pData;

			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szString)+sizeof(TCHAR)*pSendMessage->wLength);
			if (wDataSize!=sizeof(CMD_GR_SendMessage)-sizeof(pSendMessage->szString)+sizeof(TCHAR)*pSendMessage->wLength) 
				return false;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//权限判断
			ASSERT(CMasterRight::CanIssueMessage(pIServerUserItem->GetMasterRight())==true);
			if (CMasterRight::CanIssueMessage(pIServerUserItem->GetMasterRight())==false) return false;

			//变量定义
			WORD wEnumIndex=0;
			IServerUserItem *pISendServerUserItem=NULL;

			do 
			{
				pISendServerUserItem = m_ServerUserManager.EnumUserItem(wEnumIndex++);
				if (pISendServerUserItem==NULL) break;
				if (pISendServerUserItem->IsMobileUser() == false)
				{
					//发送系统消息
					if (pSendMessage->dwOptionFlags & OSW_GAME)
						SendGameMessage(pISendServerUserItem, pSendMessage->szString, pSendMessage->wType);
					if (pSendMessage->dwOptionFlags & OSW_ROOM)
						SendRoomMessage(pISendServerUserItem, pSendMessage->szString, pSendMessage->wType);
				}
			} while (true);

			return true;
		}
	case SUB_GR_LIMIT_USER_CHAT:	//限制聊天
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_LimitUserChat));
			if (wDataSize!=sizeof(CMD_GR_LimitUserChat)) return false;

			//消息处理
			CMD_GR_LimitUserChat * pLimitUserChat=(CMD_GR_LimitUserChat *)pData;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//权限判断
			ASSERT(CMasterRight::CanLimitUserChat(pIServerUserItem->GetMasterRight())==true);
			if (CMasterRight::CanLimitUserChat(pIServerUserItem->GetMasterRight())==false) return false;

			//目标用户
			IServerUserItem * pITargerUserItem=m_ServerUserManager.SearchUserItem(pLimitUserChat->dwTargetUserID);
			if (pITargerUserItem==NULL) return true;

			//变量定义
			DWORD dwAddRight = 0, dwRemoveRight = 0;

			//大厅聊天
			if (pLimitUserChat->cbLimitFlags==OSF_ROOM_CHAT)
			{
				if( pLimitUserChat->cbLimitValue == TRUE )
					dwAddRight |= UR_CANNOT_ROOM_CHAT;
				else
					dwRemoveRight |= UR_CANNOT_ROOM_CHAT;
			}

			//游戏聊天
			if (pLimitUserChat->cbLimitFlags==OSF_GAME_CHAT)
			{
				if( pLimitUserChat->cbLimitValue == TRUE )
					dwAddRight |= UR_CANNOT_GAME_CHAT;
				else
					dwRemoveRight |= UR_CANNOT_GAME_CHAT;
			}

			//大厅私聊
			if (pLimitUserChat->cbLimitFlags==OSF_ROOM_WISPER)
			{
				if( pLimitUserChat->cbLimitValue == TRUE )
					dwAddRight |= UR_CANNOT_WISPER;
				else
					dwRemoveRight |= UR_CANNOT_WISPER;
			}

			//发送喇叭
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

				//发送通知
				CMD_GR_ConfigUserRight cur = {0};
				cur.dwUserRight = pITargerUserItem->GetUserRight();

				SendDataToUser(pITargerUserItem,MDM_GR_CONFIG,SUB_GR_CONFIG_USER_RIGHT,&cur,sizeof(cur) );

				//发送消息
				SendRoomMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0005, TEXT("用户聊天权限配置成功")), SMT_CHAT);
			}
			else return false;

			return true;
		}
	case SUB_GR_TABLE_RULE:			//房间管理
		{
			//效验数据
			ASSERT(wDataSize==sizeof(DWORD));
			if (wDataSize!=sizeof(DWORD)) return false;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//权限判断
			ASSERT(CMasterRight::CanManagerServer(pIServerUserItem->GetMasterRight())==true);
			if (CMasterRight::CanManagerServer(pIServerUserItem->GetMasterRight())==false) return false;

			//变量定义
			DWORD dwServerRule=*(DWORD*)pData;
			CMD_ConfigServer.dwServerRule=(CMD_ConfigServer.dwServerRule&0x00ffffff);
			CMD_ConfigServer.dwServerRule|=(dwServerRule&0xff000000);
			m_pGameServiceOption->dwServerRule=CMD_ConfigServer.dwServerRule;

			//构造数据
			CMD_GR_ConfigRule cr = {0};
			cr.dwServerRule = CMD_ConfigServer.dwServerRule;

			//发送消息
			SendBatchData(BG_COMPUTER,MDM_GR_CONFIG,SUB_GR_CONFIG_RULE,&cr,sizeof(cr) );

			//更新配置
			ModifyGameServer();

			return true;
		}
	case SUB_GR_SERVER_OPTION:		//房间选项
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_ManageServerOption));
			if (wDataSize!=sizeof(CMD_GR_ManageServerOption)) return false;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//权限判断
			ASSERT(CMasterRight::CanManagerServer(pIServerUserItem->GetMasterRight())==true);
			if (CMasterRight::CanManagerServer(pIServerUserItem->GetMasterRight())==false) return false;

			//变量定义
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

			//提示消息
			CTraceService::TraceString(TEXT("房间选项变更，正在重新注册游戏服务..."),TraceLevel_Normal);

			//变量定义
			CMD_CS_C_ServerModify ModifyServer;
			ZeroMemory(&ModifyServer,sizeof(ModifyServer));

			//构造数据
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

			//地址信息
			ModifyServer.dwServerAddr=inet_addr(CT2CA(m_pGameServiceOption->szServiceAddr));
			StringCchCopy(ModifyServer.szServerDomain,CountArray(ModifyServer.szServerDomain), m_pGameServiceOption->szServiceDomain);			

			//分数设置
			ModifyServer.lCellScore=m_pGameServiceOption->lCellScore;
			ModifyServer.lMinEnterScore=m_pGameServiceOption->lMinEnterScore;
			ModifyServer.lMaxEnterScore=m_pGameServiceOption->lMaxEnterScore;

			//发送数据
			ASSERT(m_pITCPSocketCenter!=NULL);
			m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE,SUB_CS_C_SERVER_MODIFY,&ModifyServer,sizeof(ModifyServer),0);

			//比赛、约战、排位
			if (m_pGameServiceOption->wServerType & (GAME_GENRE_MATCH| GAME_GENRE_BATTLE| GAME_GENRE_RANKING))
			{
				m_pITCPSocketService->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_SERVER_MODIFY, &ModifyServer, sizeof(ModifyServer), 0);
			}		

			//更新配置
			ModifyGameServer();

			return true;
		}
	case SUB_GR_CARDDISPATCHERDEBUG://配牌器信息
		{
			if (wDataSize < sizeof(CMD_GR_CardDispatcherDebug)) return false;

			//提取数据
			CMD_GR_CardDispatcherDebug* pRoomDebug = (CMD_GR_CardDispatcherDebug*)pData;
			//ASSERT(pRoomDebug != NULL);

			//获取用户
			WORD wBindIndex = LOWORD(dwSocketID);
			IServerUserItem* pIServerUserItem = GetServerUserItem(LOWORD(dwSocketID), dwTokenID);
			if (pIServerUserItem == NULL) return false;

			//变量定义
			BYTE cbBuffer[SOCKET_BUFFER];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));
			WORD wSendDataSize = 0;
			CopyMemory(cbBuffer, pRoomDebug + 1, wDataSize - sizeof(CMD_GR_CardDispatcherDebug));
			wSendDataSize = wDataSize - sizeof(CMD_GR_CardDispatcherDebug);

			//tagCardDispatcher* pCardDispatcher = (tagCardDispatcher*)cbBuffer;

			//消息处理 
			if (pRoomDebug->wTableID != INVALID_TABLE)
			{
				//指定桌
				//ASSERT(pRoomDebug->wTableID < m_TableFrameArray.GetCount());
				if (pRoomDebug->wTableID >= m_TableFrameArray.GetCount()) return false;

				CTableFrameItem* pTableFrame = m_TableFrameArray[pRoomDebug->wTableID];
				//pTableFrame->OnEventSocketFrame(pRoomDebug->wSubCmdID, cbBuffer, wSendDataSize, pIServerUserItem);
				pTableFrame->ControlSendCard(pIServerUserItem, cbBuffer, wSendDataSize);
			}
			else
			{
				//遍历所有桌
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

//会员处理
bool CAttemperEngineSink::OnTCPNetworkMainMember(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch( wSubCmdID )
	{
	case SUB_GR_KICK_USER:			//踢走用户
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_GR_KickUser));
			if (wDataSize!=sizeof(CMD_GR_KickUser)) return false;

			//消息处理
			CMD_GR_KickUser * pKickUser=(CMD_GR_KickUser *)pData;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//目标用户
			IServerUserItem * pITargerUserItem=m_ServerUserManager.SearchUserItem(pKickUser->dwTargetUserID);
			if (pITargerUserItem==NULL) return true;

			//权限判断
			ASSERT( CUserRight::CanKickUser(pIServerUserItem->GetUserRight()) );
			if ( !CUserRight::CanKickUser(pIServerUserItem->GetUserRight()) ) return false;

			//检查玩家状态
			BYTE cbUserStatus = pITargerUserItem->GetUserStatus();
			BYTE cbMeStatus = pIServerUserItem->GetUserStatus();
			ASSERT( cbMeStatus==US_SIT || cbMeStatus==US_READY || cbMeStatus==US_PLAYING);
			if( cbMeStatus!=US_SIT && cbMeStatus!=US_READY && cbMeStatus!=US_PLAYING) return true;
			ASSERT( cbUserStatus==US_SIT || cbUserStatus==US_READY || cbUserStatus==US_LOOKON);
			if( cbUserStatus!=US_SIT && cbUserStatus!=US_READY && cbUserStatus!=US_LOOKON) return true;

			//检查会员等级
			ASSERT( pIServerUserItem->GetMemberOrder()>pITargerUserItem->GetMemberOrder() );
			if( pIServerUserItem->GetMemberOrder()<=pITargerUserItem->GetMemberOrder() ) return true;

			//不能踢管理员
			ASSERT( pITargerUserItem->GetMasterRight()==0 && pITargerUserItem->GetMasterOrder()==0 );
			if( pITargerUserItem->GetMasterRight()!=0 || pITargerUserItem->GetMasterOrder()!=0 ) 
				return true;

			//比赛房间不能踢
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
			{
				TCHAR szMessage[128] = {};
				_sntprintf_s(szMessage, CountArray(szMessage), TEXT("抱歉，比赛房间会员不允许踢人！"));

				if(pIServerUserItem->GetTableID()==INVALID_TABLE)
					SendRoomMessage(pIServerUserItem, szMessage, SMT_EJECT|SMT_CHAT);
				else
					SendGameMessage(pIServerUserItem, szMessage, SMT_EJECT|SMT_CHAT);

				return true;
			}

			//桌号验证
			if (pITargerUserItem->GetTableID()!=INVALID_TABLE)
			{
				//获取指针
				CTableFrameItem * pTableFrame=m_TableFrameArray[pITargerUserItem->GetTableID()];
				if(pTableFrame!=NULL)
				{
					//玩家起立
					pTableFrame->PerformStandUpAction(pITargerUserItem);
				}
			}

			//构造发送数据
			BYTE cbBuffer[SOCKET_PACKET];
			WORD wSendSize = 0;
			CMD_CM_ActionMessage *pActionMessage = (CMD_CM_ActionMessage *)cbBuffer;
			pActionMessage->nButtonType = MB_OKCANCEL;
			pActionMessage->wType = SMT_CLOSE_GAME;
			StringCchCopy( pActionMessage->szString, CountArray(pActionMessage->szString), CWHService::FormatMessage(MIM_SERVER_0006, TEXT("抱歉！您被会员玩家请出了游戏桌!")));
			pActionMessage->wLength = lstrlen(pActionMessage->szString)+1;

			wSendSize = sizeof(CMD_CM_ActionMessage)-sizeof(pActionMessage->szString)+CountStringBuffer(pActionMessage->szString);
			
			//发送数据
			SendDataToUser(pITargerUserItem, MDM_GF_FRAME, SUB_GF_ACTION_MESSAGE, cbBuffer, wSendSize);

			return true;
		}
	}

	return false;
}

//框架处理
bool CAttemperEngineSink::OnScoketMainFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//处理过虑
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketFrame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//游戏处理
bool CAttemperEngineSink::OnScoketMainGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//处理过虑
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	if ((wTableID==INVALID_TABLE)||(wChairID==INVALID_CHAIR)) return true;

	//消息处理 
	CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
	return pTableFrame->OnEventSocketGame(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//查询处理
bool CAttemperEngineSink::OnTCPNetworkMainQuery(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch(wSubCmdID)
	{
	case SUB_GR_QUERY_BY_GAMEID:
		{
			//效验
			ASSERT(wDataSize==sizeof(CMD_GR_QueryByGameID));
			if( wDataSize != sizeof(CMD_GR_QueryByGameID) )
				return false;

			//变量定义
			CMD_GR_QueryByGameID *pQueryByGameID = (CMD_GR_QueryByGameID *)pData;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//查询限制
			if( g_GlobalUnits.IsForbidQueryUser() && pIServerUserItem->GetMasterOrder() == 0 )
			{
				//发送信息
				CMD_GR_QueryNotFound NotFound = {0};
				NotFound.nResultCode = QUERY_FORBID_USE;

				WORD wSendSize = sizeof(NotFound)-sizeof(NotFound.szDescribeString)+CountStringBuffer(NotFound.szDescribeString);
				SendDataToUser(pIServerUserItem,MDM_GR_QUERY,SUB_GR_QUERY_NOT_FOUND,&NotFound,wSendSize);

				return true;
			}

			//构造信息
			CMD_CS_C_QueryByGameID QueryByGameID = {0};
			QueryByGameID.dwUserID = pIServerUserItem->GetUserID();
			QueryByGameID.dwQueryGameID = pQueryByGameID->dwGameID;

			m_pITCPSocketCenter->SendData(MDM_CS_QUERY_SERVICE,SUB_CS_C_QUERY_BY_GAMEID,&QueryByGameID,sizeof(QueryByGameID),0);

			return true;
		}
	case SUB_GR_QUERY_BY_ACCOUNTS:
		{
			//变量定义
			CMD_GR_QueryByAccounts *pQueryByAccounts = (CMD_GR_QueryByAccounts *)pData;
			
			//效验
			ASSERT(wDataSize==sizeof(CMD_GR_QueryByAccounts)-sizeof(pQueryByAccounts->szAccounts)+CountStringBuffer(pQueryByAccounts->szAccounts));
			if( wDataSize != sizeof(CMD_GR_QueryByAccounts)-sizeof(pQueryByAccounts->szAccounts)+CountStringBuffer(pQueryByAccounts->szAccounts) )
				return false;

			//获取用户
			IServerUserItem * pIServerUserItem=GetServerUserItem(LOWORD(dwSocketID),dwTokenID);
			if (pIServerUserItem==NULL) return false;

			//查询限制
			if( g_GlobalUnits.IsForbidQueryUser() && pIServerUserItem->GetMasterOrder() == 0 )
			{
				//发送信息
				CMD_GR_QueryNotFound NotFound = {0};
				NotFound.nResultCode = QUERY_FORBID_USE;

				WORD wSendSize = sizeof(NotFound)-sizeof(NotFound.szDescribeString)+CountStringBuffer(NotFound.szDescribeString);
				SendDataToUser(pIServerUserItem,MDM_GR_QUERY,SUB_GR_QUERY_NOT_FOUND,&NotFound,wSendSize);

				return true;
			}

			//构造信息
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

//比赛命令
bool CAttemperEngineSink::OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);	
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex, dwTokenID);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//类型校验
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_MATCH) == 0)
	{
		return false;
	}

	//效验接口
	ASSERT (m_pIExtendServiceManager!=NULL);
	if (m_pIExtendServiceManager==NULL) return false;		

	//消息处理
	return m_pIExtendServiceManager->OnEventNetworkRead(wSubCmdID,pData,wDataSize,pIServerUserItem,dwSocketID,dwTokenID);
}

//约战命令
bool CAttemperEngineSink::OnTCPNetwrokMainBattle(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//获取信息
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem* pIServerUserItem = GetServerUserItem(wBindIndex, dwTokenID);

	//用户效验
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return false;

	//类型校验
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) == 0)
	{
		return false;
	}

	//效验接口
	ASSERT(m_pIExtendServiceManager != NULL);
	if (m_pIExtendServiceManager == NULL) return false;

	//消息处理
	return m_pIExtendServiceManager->OnEventNetworkRead(wSubCmdID, pData, wDataSize, pIServerUserItem, dwSocketID, dwTokenID);
}

//机器登录
bool CAttemperEngineSink::OnTCPNetworkSubLogonRobot(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize >= sizeof(CMD_GR_LogonRobot));
	if (wDataSize < sizeof(CMD_GR_LogonRobot)) return false;

	//处理消息
	CMD_GR_LogonRobot * pLogonRobot = (CMD_GR_LogonRobot *)pData;
	pLogonRobot->szPassword[CountArray(pLogonRobot->szPassword) - 1] = 0;

	//获取参数
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);
	IServerUserItem * pICurrentUserItem = GetServerUserItem(wBindIndex, dwTokenID);

	//重复判断
	if (pICurrentUserItem != NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	CString strMessage;

	//大厅版本	
	DWORD dwPlazaVersion=pLogonRobot->dwPlazaVersion;
	DWORD dwClientVersion=pLogonRobot->dwProcessVersion;
	if (PerformCheckVersion(DEVICE_KIND_ROBOT, dwPlazaVersion, dwClientVersion, dwSocketID, dwTokenID) == false) return true;

	//切换判断
	IServerUserItem * pIServerUserItem = m_ServerUserManager.SearchUserItem(pLogonRobot->dwUserID);
	if ((pIServerUserItem != NULL) && (pIServerUserItem->ContrastPassword(pLogonRobot->szPassword) == true))
	{
		if (pIServerUserItem->IsRobotUser() == true && pIServerUserItem->GetClientIP() != 0L)
		{
			return false;
		}

		//切换连接
		SwitchUserItemConnect(pIServerUserItem, wBindIndex, dwTokenID, false);

		return true;
	}

	//变量定义
	DBR_GR_LogonRobot LogonRobot;
	ZeroMemory(&LogonRobot,sizeof(LogonRobot));

	//构造数据	
	LogonRobot.dwClientIP=0;
	LogonRobot.szMachineID[0]=0;
	LogonRobot.dwUserID= pLogonRobot->dwUserID;	
	LogonRobot.lRequestScore= m_pGameServiceOption->lMaxLockScore;

	//携带信息
	LogonRobot.cbClientKind = CLIENT_KIND_ROBOT;

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_ROBOT, LogonRobot.dwUserID, dwSocketID, dwTokenID, &LogonRobot, sizeof(LogonRobot));

	return true;
}

//I D 登录
bool CAttemperEngineSink::OnTCPNetworkSubLogonComputer(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GR_LogonComputer));
	if (wDataSize<sizeof(CMD_GR_LogonComputer)) return false;

	//处理消息
	CMD_GR_LogonComputer * pLogonComputer =(CMD_GR_LogonComputer *)pData;
	pLogonComputer->szPassword[CountArray(pLogonComputer->szPassword)-1]=0;

	//获取参数
	WORD wBindIndex=LOWORD(dwSocketID);
	tagTokenParameter * pTokenParameter=GetTokenParameter(wBindIndex, dwTokenID);
	IServerUserItem * pICurrentUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//重复判断
	if (pICurrentUserItem!=NULL)
	{ 
		ASSERT(FALSE);
		return false;
	}

	//扩展数据
	auto pExtendParameter = (tagExtendParameter*)pTokenParameter->cbExtendData;

	//版本信息
	pExtendParameter->cbClientKind = CLIENT_KIND_COMPUTER;
	pExtendParameter->dwPlazaVersion= pLogonComputer->dwPlazaVersion;
	pExtendParameter->dwFrameVersion= pLogonComputer->dwFrameVersion;
	pExtendParameter->dwProcessVersion= pLogonComputer->dwProcessVersion;

	//大厅版本	
	DWORD dwPlazaVersion= pLogonComputer->dwPlazaVersion;
	DWORD dwClientVersion= pLogonComputer->dwProcessVersion;
	if (PerformCheckVersion(DEVICE_KIND_PC,dwPlazaVersion,dwClientVersion,dwSocketID,dwTokenID)==false) return true;

	//切换判断
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pLogonComputer->dwUserID);
	if ((pIServerUserItem!=NULL)&&(pIServerUserItem->ContrastPassword(pLogonComputer->szPassword)==true))
	{
		if(pIServerUserItem->IsRobotUser()==true && pIServerUserItem->GetClientIP()!=0L)
		{
			return false;
		}

		//切换连接
		SwitchUserItemConnect(pIServerUserItem,wBindIndex,dwTokenID,false);

		return true;
	}

	//变量定义
	DBR_GR_LogonComputer LogonComputer;
	ZeroMemory(&LogonComputer,sizeof(LogonComputer));

	//构造数据
	LogonComputer.dwUserID= pLogonComputer->dwUserID;
	LogonComputer.dwClientIP= pTokenParameter->dwClientIP;
	LogonComputer.lRequestScore=m_pGameServiceOption->lMaxLockScore;

	//设置变量	
	StringCchCopy(LogonComputer.szPassword, CountArray(LogonComputer.szPassword), pLogonComputer->szPassword);

	//变量定义
	VOID * pDataBuffer=NULL;
	tagDataDescribe DataDescribe;
	CRecvPacketHelper RecvPacket(pLogonComputer +1,wDataSize-sizeof(CMD_GR_LogonComputer));

	//扩展信息
	while (true)
	{
		//提取数据
		pDataBuffer=RecvPacket.GetData(DataDescribe);
		if (DataDescribe.wDataDescribe==DTP_NULL) break;

		//数据分析
		switch (DataDescribe.wDataDescribe)
		{
		case DTP_GR_MACHINE_ID:	//机器标识
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

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_COMPUTER, LogonComputer.dwUserID,dwSocketID,dwTokenID,&LogonComputer,sizeof(LogonComputer));

	return true;
}


//用户登出
bool CAttemperEngineSink::OnTCPNetworkSubLogonLogout(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//获取参数
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);

	//网关子项
	auto pGlobalGateItem = pBindParameter->pGlobalGateItem;
	if (pGlobalGateItem == NULL) return true;

	//绑定参数
	auto pIServerUserItem = GetServerUserItem(wBindIndex, dwTokenID);
	if (pIServerUserItem == NULL) return true;

	//删除令牌
	pGlobalGateItem->RemoveTokenID(dwTokenID);	

	//校验令牌
	if (pIServerUserItem == NULL || pIServerUserItem->GetTokenID() != dwTokenID)
	{
		return true;
	}

	//桌子标识
	WORD wTableID=pIServerUserItem->GetTableID();	

	//断线处理
	if (wTableID!=INVALID_TABLE)
	{
		//解除绑定
		pIServerUserItem->DetachBindStatus();		

		//断线通知
		ASSERT(wTableID<m_pGameServiceOption->wTableCount);
		m_TableFrameArray[wTableID]->OnEventUserOffLine(pIServerUserItem);
	}
	else
	{
		pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
	}	

	return true;
}

//手机登录
bool CAttemperEngineSink::OnTCPNetworkSubLogonMobile(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GR_LogonMobile));
	if (wDataSize<sizeof(CMD_GR_LogonMobile)) return false;

	//处理消息
	CMD_GR_LogonMobile * pLogonMobile=(CMD_GR_LogonMobile *)pData;
	pLogonMobile->szMachineID[CountArray(pLogonMobile->szMachineID)-1]=0; 

	//获取参数
	WORD wBindIndex=LOWORD(dwSocketID);
	auto pTokenParameter=GetTokenParameter(wBindIndex,dwTokenID);
	if (pTokenParameter == NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//获取用户
	auto pICurrentUserItem=(IServerUserItem*)pTokenParameter->pBindUserItem;

	//重复判断
	if (pICurrentUserItem!=NULL)
	{ 
		//ASSERT(FALSE);
		return false; 
	}

	//扩展数据
	auto pExtendParameter = (tagExtendParameter*)pTokenParameter->cbExtendData;

	//版本信息	
	pExtendParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pExtendParameter->dwPlazaVersion=pLogonMobile->dwPlazaVersion;
	pExtendParameter->dwProcessVersion=pLogonMobile->dwProcessVersion;

	//版本校验
	DWORD dwPlazaVersion=pLogonMobile->dwPlazaVersion;
	DWORD dwClientVersion=pLogonMobile->dwProcessVersion;
	if (PerformCheckVersion(pLogonMobile->dwDeviceKind& DEVICE_MASK_MB,dwPlazaVersion,dwClientVersion,dwSocketID,dwTokenID)==false) return true;

	//机器人和真人不许互踢
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pTokenParameter->dwUserID);
	if (pIServerUserItem != NULL)
	{
		//人机互踢
		if ((pIServerUserItem->IsRobotUser() && (pTokenParameter->dwClientIP != 0L))
			|| (!pIServerUserItem->IsRobotUser() && (pTokenParameter->dwClientIP == 0L)))
		{
			SendRoomMessage(dwSocketID, dwTokenID, CWHService::FormatMessage(MIM_SERVER_0008, TEXT("该帐号已在此房间游戏，且不允许踢出，请咨询管理员！")), SMT_CHAT | SMT_EJECT | SMT_GLOBAL | SMT_CLOSE_ROOM);
			return false;
		}

		//切换连接
		SwitchUserItemConnect(pIServerUserItem, wBindIndex, dwTokenID, true);

		return true;
	}

	//变量定义
	DBR_GR_LogonMobile LogonMobile;
	ZeroMemory(&LogonMobile,sizeof(LogonMobile));

	//构造数据
	LogonMobile.dwUserID= pTokenParameter->dwUserID;
	LogonMobile.dwClientIP=pTokenParameter->dwClientIP;
	LogonMobile.dGlobalPosLat=pLogonMobile->dGlobalPosLat;
	LogonMobile.dGlobalPosLng=pLogonMobile->dGlobalPosLng;
	CopyMemory(LogonMobile.szPlaceName, pLogonMobile->szPlaceName,sizeof(LogonMobile.szPlaceName));
	LogonMobile.dwDeviceKind=pLogonMobile->dwDeviceKind;
	//CString s;
	//s.Format(_T("gameid:%d,dGlobalPosLat:%f, dGlobalPosLng:%f, szPlaceName:%s,szMachineID:%s"), pLogonMobile->wGameID, pLogonMobile->dGlobalPosLat, pLogonMobile->dGlobalPosLng, pLogonMobile->szPlaceName, pLogonMobile->szMachineID);
	//CTraceService::TraceString(s, TraceLevel_Info);
	//比赛房间
	if (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)
	{
		//校验数据
		ASSERT (wDataSize==sizeof(CMD_GR_LogonMobile)+sizeof(tagLogonMatchDetails));
		if (wDataSize!=sizeof(CMD_GR_LogonMobile)+sizeof(tagLogonMatchDetails)) return false;

		//提取数据
		tagLogonMatchDetails * pLogonMatchDetails = (tagLogonMatchDetails *)(pLogonMobile + 1);

		//设置标识
		LogonMobile.lMatchNo = pLogonMatchDetails->lMatchNo;
		LogonMobile.dwMatchID = pLogonMatchDetails->dwMatchID;	

		//校验标识
		if (LogonMobile.dwMatchID == 0 || LogonMobile.lMatchNo==0)
		{
			//发送失败
			SendLogonFailure(TEXT(""), 0, dwSocketID, dwTokenID);

			return false;
		}
	}

	//约战房间
	if (m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE)
	{
		//校验数据
		ASSERT (wDataSize<=sizeof(CMD_GR_LogonMobile)+sizeof(tagLogonBattleDetails));
		if (wDataSize>sizeof(CMD_GR_LogonMobile)+sizeof(tagLogonBattleDetails)) return false;

		//提取数据
		tagLogonBattleDetails * pLogonBattleDetails = (tagLogonBattleDetails *)(pLogonMobile + 1);
		pLogonBattleDetails->szTablePasswd[CountArray(pLogonBattleDetails->szTablePasswd) - 1] = 0;

		//变量定义
		WORD wBattleTableID = pLogonBattleDetails->wTableID;
		WORD wBattleChairID = pLogonBattleDetails->wChairID;

		//校验桌子
		if (wBattleTableID>=m_TableFrameArray.GetCount())
		{
			ASSERT(FALSE);
			return false;
		}		

		//查找桌子
		auto pTableFrame=m_TableFrameArray[wBattleTableID];
		if (pTableFrame->ContrastPassword(pLogonBattleDetails->szTablePasswd)==false)
		{
			//发送失败
			SendLogonFailure(CWHService::FormatMessage(MIM_SERVER_0007, TEXT("很抱歉，您的约战密码错误，不允许进入约战房间！")), 0, dwSocketID, dwTokenID);

			//断开连接
			if (LOWORD(dwSocketID) >= INDEX_ROBOT)
			{
				m_RobotUserManager.DeleteRobotUserItem(dwSocketID, false);
			}
			else
			{
				//构造结构
				CMD_CS_C_ShutdownSocket ShutdownSocket;
				ShutdownSocket.bForceClose = false;

				//发送数据
				SendDataToGate(dwSocketID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));
			}

			return true;
		}

		//校验椅子
		if (wBattleChairID!=INVALID_CHAIR)
		{
			//校验椅子
			if (wBattleChairID >= pTableFrame->GetChairCount())
			{
				ASSERT(FALSE);
				return false;
			}			

			//校验用户
			if (!pLogonBattleDetails->bLookon)
			{
				auto pTableUserItem = pTableFrame->GetTableUserItem(wBattleChairID);
				if (pTableUserItem != NULL)
				{
					//发送失败
					SendLogonFailure(CWHService::FormatMessage(MIM_SERVER_0022, TEXT("没有找到可用的椅子，请稍后再试！")), 0, dwSocketID, dwTokenID);
					return true;
				}
			}
		}
		else
		{
			//获取椅子
			if (!pLogonBattleDetails->bLookon)
			{
				wBattleChairID = pTableFrame->GetRelationChairID(LogonMobile.dwUserID);				
			}
		}

		//关联参数
		auto pRelationParameter = (tagRelationParameter_Battle*)pTableFrame->GetRelationParameter();

		//桌椅信息
		LogonMobile.wTableID = wBattleTableID;
		LogonMobile.wChairID = wBattleChairID;
		LogonMobile.bLookon = pLogonBattleDetails->bLookon;				
		StringCchCopy(LogonMobile.szTablePasswd, CountArray(LogonMobile.szTablePasswd), pLogonBattleDetails->szTablePasswd);

		//约战信息
		LogonMobile.dwClubID = pRelationParameter->dwClubID;
		LogonMobile.cbScoreKind = pRelationParameter->cbScoreKind;
		LogonMobile.dwBattleNum = pRelationParameter->dwRelationNum;
	}

	//排位房间
	if (m_pGameServiceOption->wServerType & GAME_GENRE_RANKING)
	{
		//校验数据
		ASSERT(wDataSize <= sizeof(CMD_GR_LogonMobile) + sizeof(tagLogonRankingDetails));
		if (wDataSize > sizeof(CMD_GR_LogonMobile) + sizeof(tagLogonRankingDetails)) return false;

		//提取数据
		tagLogonRankingDetails* pLogonRankingDetails = (tagLogonRankingDetails*)(pLogonMobile + 1);

		//校验桌子
		if (pLogonRankingDetails->wTableID >= m_TableFrameArray.GetCount())
		{
			ASSERT(FALSE);
			return false;
		}

		//查找桌子
		auto pTableFrame = m_TableFrameArray[pLogonRankingDetails->wTableID];
		if (pTableFrame->ContrastPassword(pLogonRankingDetails->szTablePasswd) == false)
		{
			//发送失败
			SendLogonFailure(CWHService::FormatMessage(MIM_SERVER_0117, TEXT("很抱歉，您的排位密码错误，不允许进入排位房间！")), 0, dwSocketID, dwTokenID);

			//断开连接
			if (LOWORD(dwSocketID) >= INDEX_ROBOT)
			{
				m_RobotUserManager.DeleteRobotUserItem(dwSocketID, false);
			}
			else
			{
				//构造结构
				CMD_CS_C_ShutdownSocket ShutdownSocket;
				ShutdownSocket.bForceClose = false;

				//发送数据
				SendDataToGate(dwSocketID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));
			}

			return true;
		}

		//排位信息	
		LogonMobile.bLookon = pLogonRankingDetails->bLookon;
		LogonMobile.wTableID = pTableFrame->GetTableID();
		LogonMobile.wChairID = pLogonRankingDetails->wChairID;		
		LogonMobile.dwRankingNum = pTableFrame->GetRelationParameter()->dwRelationNum;
		StringCchCopy(LogonMobile.szTablePasswd, CountArray(LogonMobile.szTablePasswd), pLogonRankingDetails->szTablePasswd);
	}
 
	//附加信息
	StringCchCopy(LogonMobile.szMachineID,CountArray(LogonMobile.szMachineID), pLogonMobile->szMachineID);

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_MOBILE, LogonMobile.dwUserID,dwSocketID,dwTokenID,&LogonMobile,sizeof(LogonMobile));

	return true;
}

//用户规则
bool CAttemperEngineSink::OnTCPNetworkSubUserRule(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize>=sizeof(CMD_GR_UserRule));
	if (wDataSize<sizeof(CMD_GR_UserRule)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//消息处理
	auto pUserRule=pIServerUserItem->GetUserRule();
	CMD_GR_UserRule * pCMDUserRule=(CMD_GR_UserRule *)pData;

	//规则标志
	pUserRule->bLimitSameIP=((pCMDUserRule->cbRuleMask&UR_LIMIT_SAME_IP)>0);
	pUserRule->bLimitWinRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_WIN_RATE)>0);
	pUserRule->bLimitFleeRate=((pCMDUserRule->cbRuleMask&UR_LIMIT_FLEE_RATE)>0);
	pUserRule->bLimitGameScore=((pCMDUserRule->cbRuleMask&UR_LIMIT_GAME_SCORE)>0);

	//规则属性
	pUserRule->szPassword[0]=0;
	pUserRule->wLessWinRate=pCMDUserRule->wLessWinRate;
	pUserRule->wMaxFleeRate=pCMDUserRule->wMaxFleeRate;
	pUserRule->lMaxGameScore=pCMDUserRule->lMaxGameScore;
	pUserRule->lLessGameScore=pCMDUserRule->lLessGameScore;

	//桌子密码
	bool bUserSecreTable = false;
	if (wDataSize>sizeof(CMD_GR_UserRule))
	{
		//变量定义
		VOID * pDataBuffer=NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pCMDUserRule+1,wDataSize-sizeof(CMD_GR_UserRule));

		//提取处理
		while (true)
		{
			//提取数据
			pDataBuffer=RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe==DTP_NULL) break;

			//数据分析
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_GR_TABLE_PASSWORD:		//桌子密码
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

	//加密事件
	if( pIServerUserItem->GetTableID() != INVALID_TABLE )
	{
		CTableFrameItem * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
		pTableFrame->OnEventUserRule(pIServerUserItem);
	}

	return true;
}

//用户旁观
bool CAttemperEngineSink::OnTCPNetworkSubUserLookon(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	CMD_GR_UserLookon * pUserLookon=(CMD_GR_UserLookon *)pData;

	//效验参数
	ASSERT (wDataSize>sizeof(CMD_GR_UserLookon)-sizeof(pUserLookon->szPassword) &&  wDataSize<=sizeof(CMD_GR_UserLookon));
	if (wDataSize <= sizeof(CMD_GR_UserLookon)-sizeof(pUserLookon->szPassword) || wDataSize>sizeof(CMD_GR_UserLookon))
	{
		return false;
	}

	//效验数据
	if (pUserLookon->wChairID>=m_pGameServiceAttrib->wChairCount) return false;
	if (pUserLookon->wTableID>=(WORD)m_TableFrameArray.GetCount()) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//判断密码
	if (pUserLookon->szPassword[0] != 0) pIServerUserItem->SetTmpPassword(pUserLookon->szPassword);

	//消息处理
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	if ((wTableID==pUserLookon->wTableID)&&(wChairID==pUserLookon->wChairID)&&(cbUserStatus==US_LOOKON)) return true;

	//用户判断
	if (cbUserStatus==US_PLAYING)
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0010, TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！")),0);
		return true;
	}

	/*if((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0 && pIServerUserItem->GetMasterOrder()==0)
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0011, TEXT("抱歉，比赛房间不允许旁观！")),0);
		return true;
	}*/

	//离开处理
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
		CTraceService::TraceString(TEXT("用户起立：OnTCPNetworkSubUserLookon"), TraceLevel_Info);
		if (pTableFrame->PerformStandUpAction(pIServerUserItem)==false) return true;
	}

	//权限判断
	if( pIServerUserItem->GetMasterOrder() == 0 && g_GlobalUnits.IsForbidLookon() )
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0012, TEXT("抱歉，此房间不允许旁观游戏！")),0);
		return true;
	}

	//坐下处理
	CTableFrameItem * pTableFrame=m_TableFrameArray[pUserLookon->wTableID];
	pTableFrame->PerformLookonAction(pUserLookon->wChairID,pIServerUserItem);

	return true;
}

//用户坐下
bool CAttemperEngineSink::OnTCPNetworkSubUserSitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	CMD_GR_UserSitDown * pUserSitDown=(CMD_GR_UserSitDown *)pData;

	//效验参数
	ASSERT (wDataSize>sizeof(CMD_GR_UserSitDown)-sizeof(pUserSitDown->szPassword) && wDataSize <= sizeof(CMD_GR_UserSitDown));
	if (wDataSize<=sizeof(CMD_GR_UserSitDown)-sizeof(pUserSitDown->szPassword) || wDataSize>sizeof(CMD_GR_UserSitDown)) 
		return false;

	//效验数据
	if ((m_pGameServiceOption->wServerType&(GAME_GENRE_MATCH|TABLE_GENRE_DISTRIBUTE|GAME_GENRE_BATTLE|GAME_GENRE_RANKING))==0)
	{
		if (pUserSitDown->wChairID>=m_pGameServiceAttrib->wChairCount) return false;
		if (pUserSitDown->wTableID>=(WORD)m_TableFrameArray.GetCount()) return false;
	}

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//判断密码
	if( pUserSitDown->szPassword[0] != 0 )
		pIServerUserItem->SetTmpPassword( pUserSitDown->szPassword );

	//消息处理
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

	//用户判断
	if (cbUserStatus==US_PLAYING)
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0014, TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！")),0);
		return true;
	}

	//离开处理
	if ((cbUserStatus!=US_LOOKON && wTableID!=INVALID_TABLE) || (cbUserStatus == US_LOOKON && wTableID != pUserSitDown->wTableID))
	{
		CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem,true)==false) return true;
	} 
 
	//最高进入积分判断
	if (m_pGameServiceOption->lMaxEnterScore!=0L && pIServerUserItem->GetUserScore()>m_pGameServiceOption->lMaxEnterScore)	
	{
		LPCTSTR pszMessage = TEXT("");
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0)
		{
			pszMessage = CWHService::FormatMessageV(MIM_SERVER_0017, TEXT("您好，本房间只有游戏币为" SCORE_STRING "以下的玩家才能参加游戏，请您到别的房间游戏！"), SCORE_ZOOMOUT(m_pGameServiceOption->lMaxEnterScore));
		}

		SendRequestFailure(pIServerUserItem, pszMessage, 0);
		return true;
	}

	//系统配桌
	if ((m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)!=0)
	{
		return InsertDistribute(pIServerUserItem);
	}	

	//获取变量
	WORD wRequestTableID=pUserSitDown->wTableID;
	WORD wRequestChairID=pUserSitDown->wChairID;

	//查找桌子
	if (wRequestTableID>=m_TableFrameArray.GetCount())
	{
		//查找桌子
		wRequestTableID=SearchSuitableTable(SEARCH_MODE_MAX_PLAYER,pIServerUserItem);	

		//无效桌子
		if (wRequestTableID==INVALID_TABLE)
		{
			SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0020, TEXT("没有找到可用的桌子，请稍后再试！")),0);

			return true;
		}
	}

	//查找椅子
	if (wRequestChairID>=m_pGameServiceAttrib->wChairCount)
	{
		//变量定义
		CTableFrameItem * pTableFrame=m_TableFrameArray[wRequestTableID];

		//变量定义
		wRequestChairID=INVALID_CHAIR;

		//判断椅子
		if (wRequestChairID==INVALID_CHAIR)
		{
			wRequestChairID=pTableFrame->GetRandNullChairID();			
		}

		//无效椅子
		if (wRequestChairID==INVALID_CHAIR)
		{
			if (m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE)
			{
				SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0021, TEXT("对不起，您所查找的约战房间人数已满！")),0);
			}
			else
			{
				SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0022, TEXT("没有找到可用的椅子，请稍后再试！")),0);
			}

			return true;
		}
	}	

	//坐下处理
	CTableFrameItem * pTableFrame=m_TableFrameArray[wRequestTableID];
	pTableFrame->PerformSitDownAction(wRequestChairID,pIServerUserItem);

	return true;
}

//用户起立
bool CAttemperEngineSink::OnTCPNetworkSubUserStandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_UserStandUp));
	if (wDataSize!=sizeof(CMD_GR_UserStandUp)) return false;

	//效验数据
	CMD_GR_UserStandUp * pUserStandUp=(CMD_GR_UserStandUp *)pData;
	if (pUserStandUp->wChairID>=m_pGameServiceAttrib->wChairCount) return false;
	if (pUserStandUp->wTableID>=(WORD)m_TableFrameArray.GetCount()) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//桌椅信息
	WORD wTableID=pIServerUserItem->GetTableID();
	WORD wChairID=pIServerUserItem->GetChairID();

	//分组模式
	if (m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)
	{
		//移除分组
		RemoveDistribute(pIServerUserItem);

		//退出判断
		if (wTableID==INVALID_TABLE) return true;
	}

	//消息处理	
	if ((wTableID!=pUserStandUp->wTableID)||(wChairID!=pUserStandUp->wChairID)) return true;

	//用户判断
	if ((pUserStandUp->cbForceLeave==FALSE)&&(pIServerUserItem->GetUserStatus()==US_PLAYING))
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0010, TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！")), 0);
		return true;
	}

	//离开处理
	if (wTableID!=INVALID_TABLE)
	{
		CTableFrameItem * pTableFrame=m_TableFrameArray[wTableID];
		if (pTableFrame->PerformStandUpAction(pIServerUserItem,true)==false) return true;
	}

	return true;
}

//用户聊天
bool CAttemperEngineSink::OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//变量定义
	CMD_GR_S_UserChat * pUserChat=(CMD_GR_S_UserChat *)pData;

	//效验参数
	ASSERT(wDataSize>=(sizeof(CMD_GR_S_UserChat)-sizeof(pUserChat->szChatString)));
	ASSERT(wDataSize==(sizeof(CMD_GR_S_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(TCHAR)));

	//效验参数
	if (wDataSize<(sizeof(CMD_GR_S_UserChat)-sizeof(pUserChat->szChatString))) return false;
	if (wDataSize!=(sizeof(CMD_GR_S_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(TCHAR))) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//用户效验
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//寻找用户
	IServerUserItem * pIRecvUserItem=NULL;
	if (pUserChat->dwTargetUserID!=0)
	{
		pIRecvUserItem=m_ServerUserManager.SearchUserItem(pUserChat->dwTargetUserID);
		if (pIRecvUserItem==NULL) return true;
	}

	//状态判断
	if ((g_GlobalUnits.IsAllowRoomChat()==false)&&(pISendUserItem->GetMasterOrder()==0))
	{
		//不是会员
		if( !g_GlobalUnits.IsMemberCanChat() || pISendUserItem->GetMemberOrder() == 0 )
		{
			SendRoomMessage(pISendUserItem, CWHService::FormatMessage(MIM_SERVER_0023, TEXT("抱歉，此游戏房间不允许大厅公聊！")),SMT_CHAT);
			return true;
		}
	}

	//权限判断
	if (CUserRight::CanRoomChat(pISendUserItem->GetUserRight())==false)
	{
		SendRoomMessage(pISendUserItem, CWHService::FormatMessage(MIM_SERVER_0024, TEXT("抱歉，您没有大厅公聊的权限，若需要帮助，请联系游戏客服咨询！")),SMT_EJECT|SMT_CHAT);
		return true;
	}

	//构造消息
	CMD_GR_R_UserChat UserChat;
	ZeroMemory(&UserChat,sizeof(UserChat));
	
	StringCchCopy(UserChat.szChatString,CountArray(UserChat.szChatString), pUserChat->szChatString);


	//耗时测试
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
//	str.Format(TEXT("耗时：%d毫秒"),dwTime);
//	CTraceService::TraceString(str,TraceLevel_Debug);

	//构造数据
	UserChat.dwChatColor=pUserChat->dwChatColor;
	UserChat.wChatLength=pUserChat->wChatLength;
	UserChat.dwSendUserID=pISendUserItem->GetUserID();
	UserChat.dwTargetUserID=pUserChat->dwTargetUserID;
	UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);

	//转发消息
	WORD wHeadSize=sizeof(UserChat)-sizeof(UserChat.szChatString);
	SendBatchData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_CHAT,&UserChat,wHeadSize+UserChat.wChatLength*sizeof(TCHAR));

	return true;
}

//调试数据
bool CAttemperEngineSink::OnTCPNetworkSubDebugPacket(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验数据
	ASSERT (wDataSize >= sizeof(CMD_GR_DebugPacket));
	if (wDataSize < sizeof(CMD_GR_DebugPacket)) return false;

	//提取数据
	auto pDebugPacket = (CMD_GR_DebugPacket*)pData;	

	//获取用户
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem* pIServerUserItem = GetServerUserItem(wBindIndex, dwTokenID);
	if (pIServerUserItem == NULL) return false;

	//校验权限
	if (!CUserRight::CanCheat(pIServerUserItem->GetUserRight()))
	{
		return false;
	}

	//消息处理 
	if (pDebugPacket->wTableID != INVALID_TABLE)
	{
		//校验桌号
		ASSERT (pDebugPacket->wTableID < m_TableFrameArray.GetCount());
		if (pDebugPacket->wTableID >= m_TableFrameArray.GetCount()) return false;

		auto pTableFrame = m_TableFrameArray[pDebugPacket->wTableID];
		pTableFrame->OnEventSocketFrame(pDebugPacket->wGameCmdID, pDebugPacket+1, wDataSize-sizeof(CMD_GR_DebugPacket), pIServerUserItem);
	}
	else
	{
		//遍历所有桌
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

//用户私聊
bool CAttemperEngineSink::OnTCPNetworkSubUserWisper(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID, WORD wSubCmdID)
{
	//变量定义
	CMD_GR_S_UserWisper * pUserWisper=(CMD_GR_S_UserWisper *)pData;

	//效验参数
	ASSERT(wDataSize>=(sizeof(CMD_GR_S_UserWisper)-sizeof(pUserWisper->szChatString)));
	ASSERT(wDataSize==(sizeof(CMD_GR_S_UserWisper)-sizeof(pUserWisper->szChatString)+pUserWisper->wChatLength*sizeof(TCHAR)));

	//效验参数
	if (wDataSize<(sizeof(CMD_GR_S_UserWisper)-sizeof(pUserWisper->szChatString))) return false;
	if (wDataSize!=(sizeof(CMD_GR_S_UserWisper)-sizeof(pUserWisper->szChatString)+pUserWisper->wChatLength*sizeof(TCHAR))) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pISendUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//用户效验
	ASSERT(pISendUserItem!=NULL);
	if (pISendUserItem==NULL) return false;

	//寻找用户
	IServerUserItem * pIRecvUserItem=m_ServerUserManager.SearchUserItem(pUserWisper->dwTargetUserID);
	if (pIRecvUserItem==NULL) return true;

	if( !EfficacyUserWhisper(pISendUserItem,pIRecvUserItem) )
		return true;

	//构造消息
	CMD_GR_R_UserWisper UserWisper;
	ZeroMemory(&UserWisper,sizeof(UserWisper));
	StringCchCopy(UserWisper.szChatString,CountArray(UserWisper.szChatString), pUserWisper->szChatString);
	m_WordsFilter.FilterWords(UserWisper.szChatString,CountArray(UserWisper.szChatString));

	//构造数据
	UserWisper.dwChatColor=pUserWisper->dwChatColor;
	UserWisper.wChatLength=pUserWisper->wChatLength;
	UserWisper.dwSendUserID=pISendUserItem->GetUserID();
	UserWisper.dwTargetUserID=pIRecvUserItem->GetUserID();
	UserWisper.wChatLength=CountStringBuffer(UserWisper.szChatString);

	//转发消息
	WORD wHeadSize=sizeof(UserWisper)-sizeof(UserWisper.szChatString);
	if (wSubCmdID == SUB_GR_USER_WISPER)
	{
		SendDataToUser(pISendUserItem, MDM_GR_USER, SUB_GR_USER_WISPER, &UserWisper, wHeadSize + UserWisper.wChatLength * sizeof(TCHAR));
	}

	SendDataToUser(pIRecvUserItem,MDM_GR_USER,SUB_GR_USER_WISPER,&UserWisper,wHeadSize+UserWisper.wChatLength*sizeof(TCHAR));

	return true;
}

//邀请用户
bool CAttemperEngineSink::OnSocketSubInviteUser(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_S_InviteUser));
	if (wDataSize!=sizeof(CMD_GR_S_InviteUser)) return false;

	//效验数据
	CMD_GR_S_InviteUser * pInviteUser=(CMD_GR_S_InviteUser *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//用户判断
	WORD wMeTableID=pIServerUserItem->GetTableID();
	BYTE cbMeUserStatus=pIServerUserItem->GetUserStatus();
	if ((wMeTableID==INVALID_TABLE)||(cbMeUserStatus==US_LOOKON)||(cbMeUserStatus==US_PLAYING)) return true;

	//搜索用户
	IServerUserItem * pITargetUserItem=m_ServerUserManager.SearchUserItem(pInviteUser->dwTargetUserID);
	if (pITargetUserItem==NULL) return true;

	//用户判断
	BYTE cbUserStatus=pITargetUserItem->GetUserStatus();
	if ((cbUserStatus==US_PLAYING)||(cbUserStatus==US_OFFLINE)) return true;

	//变量定义
	CMD_GR_R_InviteUser InviteUser;
	ZeroMemory(&InviteUser,sizeof(InviteUser));

	//构造数据
	InviteUser.wTableID=pIServerUserItem->GetTableID();
	InviteUser.dwSendUserID=pIServerUserItem->GetUserID();

	//发送数据
	SendDataToUser(pITargetUserItem,MDM_GR_USER,SUB_GR_INVITE_USER,&InviteUser,sizeof(InviteUser));

	return true;
}

//列表子项
bool CAttemperEngineSink::OnDBColumnItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize%sizeof(tagColumnItem)==0);
	if (wDataSize%sizeof(tagColumnItem)!=0) return false;

	//数目调整
	BYTE cbColumnCount=(BYTE)(wDataSize/sizeof(tagColumnItem));
	cbColumnCount=__min(cbColumnCount,CountArray(CMD_ConfigColumn.ColumnItem));

	//数据处理
	if (cbColumnCount>0)
	{
		//拷贝数据
		CMD_ConfigColumn.cbColumnCount=cbColumnCount;
		CopyMemory(CMD_ConfigColumn.ColumnItem,pData,cbColumnCount*sizeof(tagColumnItem));

		//事件通知
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess=ER_SUCCESS;
		SendUIControlPacket(UI_SERVICE_CONFIG_RESULT,&ControlResult,sizeof(ControlResult));
	}
	else
	{
		//提示消息
		CTraceService::TraceString(TEXT("游戏配置信息读取失败"),TraceLevel_Exception);

		//事件通知
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess=ER_FAILURE;
		SendUIControlPacket(UI_SERVICE_CONFIG_RESULT,&ControlResult,sizeof(ControlResult));
	}

	return true;
}

//配置结果
bool CAttemperEngineSink::OnDBConfigResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//效验参数
	ASSERT (wDataSize==sizeof(DBO_GR_ConfigResult));
	if (wDataSize != sizeof(DBO_GR_ConfigResult)) return false;

	//提取数据
	DBO_GR_ConfigResult* pConfigResult = (DBO_GR_ConfigResult*)pData;

	//构造结构
	CP_ControlResult ControlResult;
	ZeroMemory(&ControlResult,sizeof(ControlResult));

	//数据处理
	if (pConfigResult->bResultCode==true)
	{
		//游戏等级
		if (pConfigResult->dwConfigMask & CONFIG_MASK_GAME_LEVEL)
		{
			g_GameLevelManager->ResetData();
			g_GameLevelManager->AppendGameLevel(g_GameLevelManager->m_ArrayGameLevel.GetData(), g_GameLevelManager->m_ArrayGameLevel.GetCount());
			g_GameLevelManager->AppendUserLevel(g_GameLevelManager->m_ArrayUserLevel.GetData(), g_GameLevelManager->m_ArrayUserLevel.GetCount());
			g_GameLevelManager->AppendRankLevel(g_GameLevelManager->m_ArrayRankLevel.GetData(), g_GameLevelManager->m_ArrayRankLevel.GetCount());
		}

		//游戏物品
		if (pConfigResult->dwConfigMask & CONFIG_MASK_GAME_GOODS)
		{
			g_GameGoodsManager->ResetData();
			g_GameGoodsManager->AppendGameGoods(g_GameGoodsManager->m_ArrayGoodsInfo.GetData(), g_GameGoodsManager->m_ArrayGoodsInfo.GetCount());
		}

		//切换配置
		if (pConfigResult->dwConfigMask & GLOBAL_OPTION_MASK)
		{
			g_GlobalOptionManager->SwitchOptionItem(pConfigResult->dwConfigMask);
		}

		//事件通知
		ControlResult.cbSuccess = ER_SUCCESS;		
	}
	else
	{
		ControlResult.cbSuccess = ER_FAILURE;		
	}

	//事件通知
	if (pConfigResult->bInitService == true)
	{
		SendUIControlPacket(UI_SERVICE_CONFIG_RESULT, &ControlResult, sizeof(ControlResult));
	}	

	return true;
}

//加载敏感词
bool CAttemperEngineSink::OnDBFilterWords(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	if(dwContextID==0xffff)			//加载完成
	{
		return true;
	}
	else if(dwContextID==0xfffe)	//开始加载
	{
		m_WordsFilter.ResetFilter();
		return true;			
	}

	const TCHAR *pWords=(const TCHAR*)pData;
	m_WordsFilter.InsertKeyword(pWords);

	return true;
}

//游戏选项
bool CAttemperEngineSink::OnDBBattleOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验数据
	ASSERT (wDataSize%sizeof(tagBattleGameOptionEx)==0);
	if (wDataSize%sizeof(tagBattleGameOptionEx)!=0) return false;

	//计算数量
	WORD wOptionCount=wDataSize/sizeof(tagBattleGameOptionEx);
	tagBattleGameOptionEx * pBattleGameOptionEx = (tagBattleGameOptionEx *)pData;

	//移除配置
	m_BattleGameOptionArray.RemoveAll();

	//拷贝配置
	for (WORD i=0; i<wOptionCount; i++)
	{
		m_BattleGameOptionArray.Add(*pBattleGameOptionEx++);
	}

	return true;
}

//机器信息
bool CAttemperEngineSink::OnDBRobotUser(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_GameRobotInfo * pGameRobotInfo=(DBO_GR_GameRobotInfo *)pData;
	WORD wHeadSize=sizeof(DBO_GR_GameRobotInfo)-sizeof(pGameRobotInfo->RobotAccountsInfo);

	//效验参数
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameRobotInfo->wRobotCount*sizeof(pGameRobotInfo->RobotAccountsInfo[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameRobotInfo->wRobotCount*sizeof(pGameRobotInfo->RobotAccountsInfo[0])))) return false;

	//设置机器
	if (pGameRobotInfo->lResultCode==DB_SUCCESS)
	{
		m_RobotUserManager.InsertRobotInfo(pGameRobotInfo->RobotAccountsInfo,pGameRobotInfo->wRobotCount,pGameRobotInfo->dwBatchID);
	}

	return true;

}

//机器参数
bool CAttemperEngineSink::OnDBRobotParameter(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//变量定义
	DBO_GR_RobotParameter * pGameRobotParameter=(DBO_GR_RobotParameter *)pData;
	WORD wHeadSize=sizeof(DBO_GR_RobotParameter)-sizeof(pGameRobotParameter->RobotParameter);

	//效验参数
	ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGameRobotParameter->wParameterCount*sizeof(pGameRobotParameter->RobotParameter[0]))));
	if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGameRobotParameter->wParameterCount*sizeof(pGameRobotParameter->RobotParameter[0])))) return false;

	//设置机器
	if (pGameRobotParameter->lResultCode==DB_SUCCESS)
	{
		m_RobotUserManager.AddRobotParameter(pGameRobotParameter->RobotParameter,pGameRobotParameter->wParameterCount);
	}

	return true;
}

//低保参数
bool CAttemperEngineSink::OnDBSubsidyParameter(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize==sizeof(DBO_GR_SubsidyParameter));
	if(wDataSize!=sizeof(DBO_GR_SubsidyParameter)) return false;

	//提取数据
	auto pSubsidyParameter =(DBO_GR_SubsidyParameter*)pData;

	//设置变量
	m_SystemParameter.bLoadFlag=true;
	m_SystemParameter.lTakeSubsidyGold= pSubsidyParameter->lTakeSubsidyGold;
	m_SystemParameter.lTakeSubsidyCondition= pSubsidyParameter->lTakeSubsidyCondition;

	return true;
}

//控制参数
bool CAttemperEngineSink::OnDBControlParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize == sizeof(DBO_GR_ControlParameter));
	if (wDataSize != sizeof(DBO_GR_ControlParameter)) return false;

	//提取数据
	auto pControlParameter = (DBO_GR_ControlParameter*)pData;

	//控制参数
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

//登录失败
bool CAttemperEngineSink::OnDBLogonFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//在线判断
	WORD wBindIndex = LOWORD(dwContextID);
	tagTokenParameter * pTokenParameter = GetTokenParameter(wBindIndex, dwTokenID);
	if (pTokenParameter == NULL) return false;

	//在线判断
	auto pIServerUserItem = (IServerUserItem*)pTokenParameter->pBindUserItem;
	if (pIServerUserItem != NULL) return false;

	//发送错误
	DBO_GR_LogonFailure * pLogonFailure=(DBO_GR_LogonFailure *)pData;	

	//构造数据
	CMD_GR_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure, sizeof(LogonFailure));

	//锁定信息
	LogonFailure.wLockKindID = pLogonFailure->wLockKindID;
	LogonFailure.wLockServerID = pLogonFailure->wLockServerID;
	LogonFailure.wLockServerKind = pLogonFailure->wLockServerKind;

	//错误信息
	LogonFailure.lErrorCode = pLogonFailure->lErrorCode;
	StringCchCopy(LogonFailure.szDescribeString, CountArray(LogonFailure.szDescribeString), pLogonFailure->szDescribeString);

	//数据属性
	WORD wStringSize = CountStringBuffer(LogonFailure.szDescribeString);
	WORD wSendSize = sizeof(LogonFailure) - sizeof(LogonFailure.szDescribeString) + wStringSize;

	//发送数据
	SendDataToGate(dwContextID, dwTokenID, MDM_GR_LOGON, SUB_GR_LOGON_FAILURE, &LogonFailure, wSendSize);

	//断开连接
	if (LOWORD(dwContextID)>=INDEX_ROBOT) 
	{
		m_RobotUserManager.DeleteRobotUserItem(dwContextID,false);
	}
	else
	{
		//构造结构
		CMD_CS_C_ShutdownSocket ShutdownSocket;
		ShutdownSocket.bForceClose = false;

		//发送数据
		SendDataToGate(dwContextID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));
	}

	return true;
}

//登录成功
bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwContextID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//提取数据
	DBO_GR_LogonSuccess* pDBOLogonSuccess = (DBO_GR_LogonSuccess*)pData;

	//令牌参数
	tagTokenParameter * pTokenParameter=GetTokenParameter(wBindIndex,dwTokenID);
	if (pTokenParameter == NULL)
	{
		//解除锁定
		DBR_GR_LeaveGameService LeaveGameService;
		ZeroMemory(&LeaveGameService, sizeof(LeaveGameService));

		//用户信息
		LeaveGameService.dwUserID = pDBOLogonSuccess->dwUserID;
		if (pDBOLogonSuccess->lFreeScore > 0L)
			LeaveGameService.lUnLockScore = pDBOLogonSuccess->lFreeScore;
		else
			LeaveGameService.lUnLockScore = 0;

		//连胜信息
		LeaveGameService.lWinStreak = pDBOLogonSuccess->lWinStreak;
		LeaveGameService.lMaxWinStreak = pDBOLogonSuccess->lMaxWinStreak;

		//连接信息
		LeaveGameService.cbLeaveReason = LEAVE_GAME_USER_CANCEL;
		LeaveGameService.dwInoutIndex = pDBOLogonSuccess->dwInoutIndex;

		//连接信息
		LeaveGameService.dwClientIP = pBindParameter->dwClientIP;
		StringCchCopy(LeaveGameService.szMachineID, LEN_MACHINE_ID, pDBOLogonSuccess->szMachineID);

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_SERVICE, pDBOLogonSuccess->dwUserID, 0L, 0L, &LeaveGameService, sizeof(LeaveGameService));

		return true;
	}	
 
	//废弃判断
	if (pBindParameter->dwContextID!=dwContextID || pTokenParameter->dwTokenID!=dwTokenID)
	{
		//解除锁定
		DBR_GR_LeaveGameService LeaveGameService;
		ZeroMemory(&LeaveGameService,sizeof(LeaveGameService));

		//用户信息
		LeaveGameService.dwUserID=pDBOLogonSuccess->dwUserID;
		if (pDBOLogonSuccess->lFreeScore>0L)
			LeaveGameService.lUnLockScore=pDBOLogonSuccess->lFreeScore;
		else
			LeaveGameService.lUnLockScore=0;

		//连胜信息
		LeaveGameService.lWinStreak = pDBOLogonSuccess->lWinStreak;
		LeaveGameService.lMaxWinStreak = pDBOLogonSuccess->lMaxWinStreak;

		//连接信息
		LeaveGameService.cbLeaveReason=LEAVE_GAME_BIND_ERROR;
		LeaveGameService.dwInoutIndex = pDBOLogonSuccess->dwInoutIndex;

		//连接信息
		LeaveGameService.dwClientIP=pBindParameter->dwClientIP;
		StringCchCopy(LeaveGameService.szMachineID, LEN_MACHINE_ID, pDBOLogonSuccess->szMachineID);		

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_SERVICE, pDBOLogonSuccess->dwUserID,0L,0L,&LeaveGameService,sizeof(LeaveGameService));

		return true;
	}

	//扩展数据
	auto pExtendParameter = (tagExtendParameter*)pTokenParameter->cbExtendData;

	//变量定义
	bool bRobotUser=(wBindIndex>=INDEX_ROBOT);
	bool bMobileUser=(pExtendParameter->cbClientKind==CLIENT_KIND_MOBILE);

	//切换判断
	IServerUserItem * pIServerUserItem=m_ServerUserManager.SearchUserItem(pDBOLogonSuccess->dwUserID);
	if (pIServerUserItem!=NULL)
	{		
		//变量定义
		DBR_GR_LeaveGameService LeaveGameService;
		ZeroMemory(&LeaveGameService,sizeof(LeaveGameService));

		//用户信息
		LeaveGameService.dwUserID=pDBOLogonSuccess->dwUserID;

		//解除锁定
		if (pDBOLogonSuccess->lFreeScore>0L)
			LeaveGameService.lUnLockScore=pDBOLogonSuccess->lFreeScore;
		else 
			LeaveGameService.lUnLockScore=-0;

		//连胜局数
		LeaveGameService.lWinStreak = pDBOLogonSuccess->lWinStreak;
		LeaveGameService.lMaxWinStreak = pDBOLogonSuccess->lMaxWinStreak;

		//请求信息
		LeaveGameService.cbLeaveReason=LEAVE_GAME_TWICE_LOGIN;
		LeaveGameService.dwInoutIndex=pDBOLogonSuccess->dwInoutIndex;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_SERVICE, pDBOLogonSuccess->dwUserID,0L,0L,&LeaveGameService,sizeof(LeaveGameService));

		//切换用户
		SwitchUserItemConnect(pIServerUserItem,wBindIndex,dwTokenID,bMobileUser);

		return true;
	}

	//最高进入积分判断	
	if (m_pGameServiceOption->lMaxEnterScore != 0L && pDBOLogonSuccess->lGold > m_pGameServiceOption->lMaxEnterScore && pDBOLogonSuccess->cbMasterOrder==0)
	{
		//变量定义
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//构造消息
		LPCTSTR pszMessage = TEXT("");
		if ((m_pGameServiceOption->wServerType & (GAME_GENRE_GOLD)) != 0)
		{
			pszMessage = CWHService::FormatMessageV(MIM_SERVER_0017, TEXT("您好，本房间只有金币为" SCORE_STRING "以下的玩家才可以进入！"), SCORE_ZOOMOUT(m_pGameServiceOption->lMaxEnterScore));
		}
		else
		{
			pszMessage = CWHService::FormatMessageV(MIM_SERVER_0019, TEXT("您好，本房间只有游戏积分为" SCORE_STRING "以下的玩家才可以进入！"), SCORE_ZOOMOUT(m_pGameServiceOption->lMaxEnterScore));
		}

		StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), pszMessage);

		//构造数据
		SystemMessage.wLength=lstrlen(pszMessage)+1;
		SystemMessage.wType=SMT_EJECT|SMT_CHAT|SMT_CLOSE_ROOM|SMT_CLOSE_LINK;

		//数据属性
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//发送数据
		SendDataToGate(pBindParameter->dwContextID, dwTokenID, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wSendSize);

		//解除锁定
		//变量定义
		DBR_GR_LeaveGameService LeaveGameService;
		ZeroMemory(&LeaveGameService,sizeof(LeaveGameService));

		//用户信息
		LeaveGameService.dwUserID=pDBOLogonSuccess->dwUserID;
		if (pDBOLogonSuccess->lFreeScore > 0L)
		{
			LeaveGameService.lUnLockScore = pDBOLogonSuccess->lFreeScore;
		}
		else
		{
			LeaveGameService.lUnLockScore = 0;
		}

		//连胜局数
		LeaveGameService.lWinStreak = pDBOLogonSuccess->lWinStreak;
		LeaveGameService.lMaxWinStreak = pDBOLogonSuccess->lMaxWinStreak;

		//请求信息
		LeaveGameService.cbLeaveReason=LEAVE_GAME_THAN_MAX_SCORE;
		LeaveGameService.dwInoutIndex = pDBOLogonSuccess->dwInoutIndex;

		//连接信息
		LeaveGameService.dwClientIP=pBindParameter->dwClientIP;
		StringCchCopy(LeaveGameService.szMachineID, LEN_MACHINE_ID, pDBOLogonSuccess->szMachineID);		
  
		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_SERVICE, pDBOLogonSuccess->dwUserID,0L,0L,&LeaveGameService,sizeof(LeaveGameService));

		return true;
	}

	//满人判断
	WORD wMaxPlayer=m_pGameServiceOption->wMaxPlayer;
	DWORD dwOnlineCount=m_ServerUserManager.GetUserItemCount();
	WORD wMemberOrder = g_GameLevelManager->CalcUserOrder(pDBOLogonSuccess->dwMemberPoint);
	if (bMobileUser==false&&(wMemberOrder==0)&&(pDBOLogonSuccess->cbMasterOrder==0)&&(dwOnlineCount>(DWORD)(wMaxPlayer-20L)))
	{
		//变量定义
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//构造消息
		StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), CWHService::FormatMessage(MIM_SERVER_0031, TEXT("抱歉，由于此房间已经人满，普通玩家不能继续进入了！")));

		//构造数据
		SystemMessage.wLength=lstrlen(SystemMessage.szString)+1;
		SystemMessage.wType=SMT_EJECT|SMT_CHAT|SMT_CLOSE_ROOM|SMT_CLOSE_LINK;

		//数据属性
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//发送数据
		SendDataToGate(pBindParameter->dwContextID, dwTokenID, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wSendSize);		

		//变量定义
		DBR_GR_LeaveGameService LeaveGameService;
		ZeroMemory(&LeaveGameService,sizeof(LeaveGameService));

		//用户信息
		LeaveGameService.dwUserID=pDBOLogonSuccess->dwUserID;
		if (pDBOLogonSuccess->lFreeScore>0L)
			LeaveGameService.lUnLockScore=pDBOLogonSuccess->lFreeScore;
		else
			LeaveGameService.lUnLockScore=0;

		//连胜局数
		LeaveGameService.lWinStreak = pDBOLogonSuccess->lWinStreak;
		LeaveGameService.lMaxWinStreak = pDBOLogonSuccess->lMaxWinStreak;

		//请求信息
		LeaveGameService.cbLeaveReason=LEAVE_GAME_FULL;
		LeaveGameService.dwInoutIndex=pDBOLogonSuccess->dwInoutIndex;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_SERVICE, pDBOLogonSuccess->dwUserID,0L,0L,&LeaveGameService,sizeof(LeaveGameService));
		
		return true;
	}

	//禁止判断
	if ((g_GlobalUnits.IsAllowEnterRoom()==false)&&(pDBOLogonSuccess->cbMasterOrder==0))
	{
		//变量定义
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//构造消息
		StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), CWHService::FormatMessage(MIM_SERVER_0032, TEXT("抱歉，由于系统维护的原因，此游戏房间暂时不允许用户进入！")));

		//构造数据
		SystemMessage.wLength=lstrlen(SystemMessage.szString)+1;
		SystemMessage.wType=SMT_EJECT|SMT_CHAT|SMT_CLOSE_ROOM|SMT_CLOSE_LINK;

		//数据属性
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//发送数据
		SendDataToGate(pBindParameter->dwContextID, dwTokenID, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wSendSize);

		//解除锁定
		//变量定义
		DBR_GR_LeaveGameService LeaveGameService;
		ZeroMemory(&LeaveGameService,sizeof(LeaveGameService));

		//用户信息
		LeaveGameService.dwUserID=pDBOLogonSuccess->dwUserID;
		if (pDBOLogonSuccess->lFreeScore>0L)
			LeaveGameService.lUnLockScore=pDBOLogonSuccess->lFreeScore;
		else
			LeaveGameService.lUnLockScore=0;

		//连胜局数
		LeaveGameService.lWinStreak = pDBOLogonSuccess->lWinStreak;
		LeaveGameService.lMaxWinStreak = pDBOLogonSuccess->lMaxWinStreak;

		//请求信息
		LeaveGameService.cbLeaveReason=LEAVE_GAME_MANAGEMENT;
		LeaveGameService.dwInoutIndex = pDBOLogonSuccess->dwInoutIndex;

		//连接信息
		LeaveGameService.dwClientIP=pBindParameter->dwClientIP;
		StringCchCopy(LeaveGameService.szMachineID, LEN_MACHINE_ID, pDBOLogonSuccess->szMachineID);		

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_SERVICE, pDBOLogonSuccess->dwUserID,0L,0L,&LeaveGameService,sizeof(LeaveGameService));

		return true;
	}

	//查找机器
	if (bRobotUser==true && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
	{
		//查找机器
		DWORD dwUserID=pDBOLogonSuccess->dwUserID;
		CRobotUserItem * pRobotUserItem = (CRobotUserItem *) m_RobotUserManager.SearchRobotUserItem(dwUserID);

		//修改积分
		if (pRobotUserItem !=NULL)
		{
			//获取配置
			tagRobotParameter * pRobotParameter= pRobotUserItem->GetRobotParameter();

			//调整积分
			if ((pRobotParameter->lTakeMinScore!=0L)&&(pRobotParameter->lTakeMaxScore!=0L))
			{
				//变量定义
				SCORE lTakeMinScore=(SCORE)pRobotParameter->lTakeMinScore;
				SCORE lTakeMaxScore=(SCORE)__max(pRobotParameter->lTakeMaxScore,pRobotParameter->lTakeMinScore);

				//调整积分
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

	//用户变量
	tagServerUserInfo UserInfo;
	tagUserInfoPlus UserInfoPlus;
	ZeroMemory(&UserInfo,sizeof(UserInfo));
	ZeroMemory(&UserInfoPlus,sizeof(UserInfoPlus));

	//属性资料
	UserInfo.wFaceID=pDBOLogonSuccess->wFaceID;
	UserInfo.dwUserID=pDBOLogonSuccess->dwUserID;
	UserInfo.dwGameID=pDBOLogonSuccess->dwGameID;	
	UserInfo.dwCustomID=pDBOLogonSuccess->dwCustomID;	
	StringCchCopy(UserInfo.szAccounts,CountArray(UserInfo.szAccounts), pDBOLogonSuccess->szAccounts);

	//用户资料
	UserInfo.cbAge=pDBOLogonSuccess->cbAge;
	UserInfo.cbGender=pDBOLogonSuccess->cbGender;
	StringCchCopy(UserInfo.szArea,CountArray(UserInfo.szArea), pDBOLogonSuccess->szArea);
	StringCchCopy(UserInfo.szCity,CountArray(UserInfo.szCity), pDBOLogonSuccess->szCity);
	StringCchCopy(UserInfo.szProvince,CountArray(UserInfo.szProvince), pDBOLogonSuccess->szProvince);
	StringCchCopy(UserInfo.szUnderWrite,CountArray(UserInfo.szUnderWrite), pDBOLogonSuccess->szUnderWrite);

	//状态设置
	UserInfo.cbUserStatus=US_FREE;
	UserInfo.wTableID=INVALID_TABLE;
	UserInfo.wChairID=INVALID_CHAIR;

	//积分信息	
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

	//等级信息
	UserInfo.cbMasterOrder = pDBOLogonSuccess->cbMasterOrder;
	UserInfo.dwMemberPoint = pDBOLogonSuccess->dwMemberPoint;
	StringCchCopy(UserInfo.szUserHeadUrl, CountArray(UserInfo.szUserHeadUrl), pDBOLogonSuccess->szUserHeadUrl);//kk jia

	//用户权限
	UserInfoPlus.dwUserRight=pDBOLogonSuccess->dwUserRight;
	UserInfoPlus.dwMasterRight=pDBOLogonSuccess->dwMasterRight;

	//位置信息
	UserInfoPlus.dGlobalPosLat=pDBOLogonSuccess->dGlobalPosLat;
	UserInfoPlus.dGlobalPosLng=pDBOLogonSuccess->dGlobalPosLng;
	CopyMemory(UserInfoPlus.szPlaceName, pDBOLogonSuccess->szPlaceName, sizeof(UserInfoPlus.szPlaceName));

	//状态信息
	UserInfoPlus.cbShutUpStatus = pDBOLogonSuccess->cbShutUpStatus;
	UserInfoPlus.dwShutUpOverTime = pDBOLogonSuccess->dwShutUpOverTime;

	//输赢控制
	UserInfoPlus.wAddupWinRate = pDBOLogonSuccess->wAddupWinRate;
	UserInfoPlus.lAddupWinScore = pDBOLogonSuccess->lAddupWinScore;
	UserInfoPlus.lAddupLoseScore = pDBOLogonSuccess->lAddupLoseScore;
	CopyMemory(&UserInfoPlus.UserControlItem, &pDBOLogonSuccess->UserControlItem, sizeof(UserInfoPlus.UserControlItem));

	SCORE lFreezeScore = 0L;
	m_FreezeScoreMap.Lookup(UserInfo.dwUserID,lFreezeScore);

	//附加信息
	UserInfoPlus.bRobotUser=bRobotUser;
	UserInfoPlus.bMobileUser=bMobileUser;	
	UserInfoPlus.lFreeScore=pDBOLogonSuccess->lFreeScore+lFreezeScore;
	if( CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule) )
		UserInfoPlus.lLockServiceScore = m_pGameServiceOption->lRevenue;
	UserInfoPlus.lFreezeScore = lFreezeScore;
	UserInfoPlus.dwInoutIndex=pDBOLogonSuccess->dwInoutIndex;
	UserInfoPlus.lRestrictScore=m_pGameServiceOption->lRestrictScore;

	//激活用户
	UserInfoPlus.dwTokenID=dwTokenID;
	UserInfoPlus.wBindIndex=wBindIndex;
	UserInfoPlus.dwClientIP=pTokenParameter->dwClientIP;
	
	//机器序列
	StringCchCopy(UserInfoPlus.szMachineID, LEN_MACHINE_ID, pDBOLogonSuccess->szMachineID);

	pIServerUserItem=m_ServerUserManager.ActiveUserItem(UserInfo,UserInfoPlus,pDBOLogonSuccess->szPassword);

	//错误判断
	if (pIServerUserItem==NULL)
	{
		//错误断言
		ASSERT(FALSE);

		//解除锁定		
		DBR_GR_LeaveGameService LeaveGameService;
		ZeroMemory(&LeaveGameService,sizeof(LeaveGameService));

		//用户信息
		LeaveGameService.dwUserID=pDBOLogonSuccess->dwUserID;
		if (pDBOLogonSuccess->lFreeScore>0L)
			LeaveGameService.lUnLockScore=pDBOLogonSuccess->lFreeScore;
		else
			LeaveGameService.lUnLockScore=0;

		//连胜局数
		LeaveGameService.lWinStreak = pDBOLogonSuccess->lWinStreak;
		LeaveGameService.lMaxWinStreak = pDBOLogonSuccess->lMaxWinStreak;

		//请求信息
		LeaveGameService.cbLeaveReason=LEAVE_GAME_ACTIVE_FAILURE;
		LeaveGameService.dwInoutIndex = pDBOLogonSuccess->dwInoutIndex;

		//连接信息
		LeaveGameService.dwClientIP=pBindParameter->dwClientIP;
		StringCchCopy(LeaveGameService.szMachineID, LEN_MACHINE_ID, pDBOLogonSuccess->szMachineID);		

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_SERVICE, pDBOLogonSuccess->dwUserID,0L,0L,&LeaveGameService,sizeof(LeaveGameService));

		//断开用户
		if (bRobotUser==true)
		{
			m_RobotUserManager.DeleteRobotUserItem(pBindParameter->dwContextID,false);
		}
		else 
		{
			//构造结构
			CMD_CS_C_ShutdownSocket ShutdownSocket;
			ShutdownSocket.bForceClose = true;

			//发送数据
			SendDataToGate(pBindParameter->dwContextID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));
		}

		return true;
	}

	//低保信息
	pIServerUserItem->SetResidualTimes(pDBOLogonSuccess->cbResidualTimes);
	pIServerUserItem->GetSubsidyInfo().lConsumeScore = pDBOLogonSuccess->lConsumeScore;

	//添加物品
	if (pDBOLogonSuccess->wGoodsCount > 0)
	{
		pIServerUserItem->GetUserPack()->AppendGoods(pDBOLogonSuccess->GoodsHoldInfo, pDBOLogonSuccess->wGoodsCount);
	}

	//绑定用户
	pTokenParameter->pBindUserItem = pIServerUserItem;

	//比赛类型
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		//设置变量
		pIServerUserItem->SetMatchID(pDBOLogonSuccess->dwMatchID);
		pIServerUserItem->SetSignupCount(pDBOLogonSuccess->wSignupCount);
		pIServerUserItem->SetTicketCount(pDBOLogonSuccess->dwTicketCount);		

		//校验权限
		if (CUserRight::IsMatchUser(pIServerUserItem->GetUserRight()))
		{
			pIServerUserItem->SetUserMatchStatus(MUS_SIGNUP);			
			pIServerUserItem->SetMatchNo(pDBOLogonSuccess->lMatchNo);	
		}		
	}

	//登录事件
	OnEventUserLogon(pIServerUserItem);	

	//登录通知
	if (m_pIExtendServiceManager!=NULL) 
	{
		m_pIExtendServiceManager->OnEventUserLogon(pIServerUserItem);
	}

	//约战房间
	if (m_pGameServiceOption->wServerType&(GAME_GENRE_BATTLE | GAME_GENRE_RANKING) && pIServerUserItem->IsMobileUser()==true)
	{	
		//坐下处理
		if (pDBOLogonSuccess->bLookon == false)
		{
			//构造结构
			CMD_GR_UserSitDown UserSitDown;
			ZeroMemory(&UserSitDown, sizeof(UserSitDown));

			//设置变量
			UserSitDown.wChairID = pDBOLogonSuccess->wChairID;
			UserSitDown.wTableID = pDBOLogonSuccess->wTableID;
			StringCchCopy(UserSitDown.szPassword, CountArray(UserSitDown.szPassword), pDBOLogonSuccess->szTablePasswd);

			//坐下处理
			OnTCPNetworkSubUserSitDown(&UserSitDown, sizeof(UserSitDown), dwContextID, dwTokenID);
		}		

		//旁观处理
		if (pDBOLogonSuccess->bLookon == true)
		{
			//构造结构
			CMD_GR_UserLookon UserLookon;
			ZeroMemory(&UserLookon, sizeof(UserLookon));

			//设置变量
			UserLookon.wChairID = pDBOLogonSuccess->wChairID;
			UserLookon.wTableID = pDBOLogonSuccess->wTableID;
			StringCchCopy(UserLookon.szPassword, CountArray(UserLookon.szPassword), pDBOLogonSuccess->szTablePasswd);

			//旁观处理
			OnTCPNetworkSubUserLookon(&UserLookon, sizeof(UserLookon), dwContextID, dwTokenID);
		}
	}

	//汇总用户
	if (m_bCollectUser==true)
	{
		//变量定义
		CMD_CS_C_EnterServer EnterServer;
		ZeroMemory(&EnterServer,sizeof(EnterServer));

		//设置变量
		EnterServer.dwUserID=pIServerUserItem->GetUserID();
		EnterServer.dwGameID=pIServerUserItem->GetGameID();
		EnterServer.cbGender = pIServerUserItem->GetUserInfo()->cbGender;
		EnterServer.wFaceID = pIServerUserItem->GetUserInfo()->wFaceID;
		EnterServer.dwCustomID = pIServerUserItem->GetUserInfo()->dwCustomID;
		StringCchCopy(EnterServer.szNickName,CountArray(EnterServer.szNickName), pIServerUserItem->GetAccounts());

		//发送消息
		ASSERT(m_pITCPSocketCenter!=NULL);
		m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_ENTER_SERVER,&EnterServer,sizeof(EnterServer),0);		
	}

	//更新人数
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0)
	{
		//变量定义
		CMD_CS_C_ServiceOnline ServiceOnLine;
		ZeroMemory(&ServiceOnLine, sizeof(ServiceOnLine));

		//设置变量
		ServiceOnLine.wOnlineCount = m_ServerUserManager.GetUserItemCount();

		//发送信息
		ASSERT(m_pITCPSocketCenter != NULL);
		m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_SERVICE_ONLINE, &ServiceOnLine, sizeof(ServiceOnLine), 0);
	}

	if (pDBOLogonSuccess->szDescribeString[0]!='\0')
	{
		//变量定义
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		StringCchCopy(SystemMessage.szString,CountArray(SystemMessage.szString), pDBOLogonSuccess->szDescribeString);

		//构造数据
		SystemMessage.wLength=lstrlen(SystemMessage.szString)+1;
		SystemMessage.wType=SMT_EJECT|SMT_CHAT;

		//数据属性
		WORD wHeadSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString);
		WORD wSendSize=wHeadSize+CountStringBuffer(SystemMessage.szString);

		//发送数据
		if (bRobotUser==true)
		{
			//机器用户
			m_RobotUserManager.SendDataToClient(pBindParameter->dwContextID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize);
		}
		else
		{
			//常规用户
			m_pITCPNetworkEngine->SendData(pBindParameter->dwContextID,MDM_CM_SYSTEM,SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wSendSize,&dwTokenID,1);
		}
	}

	return true;
}

//用户财富
bool CAttemperEngineSink::OnDBWealthUpdate(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT (sizeof(DBO_GR_UserWealthUpdate)==wDataSize);
	if (sizeof(DBO_GR_UserWealthUpdate) != wDataSize) return false;

	//提取数据
	DBO_GR_UserWealthUpdate* pWealthUpdate = (DBO_GR_UserWealthUpdate*)pData;

	//查找用户
	IServerUserItem * pIServerUserItem = m_ServerUserManager.SearchUserItem(pWealthUpdate->dwUserID);
	if (pIServerUserItem == NULL) return true;	

	//更新财富
	auto pUserInfo = pIServerUserItem->GetUserInfo();

	//同步内存
	if (pWealthUpdate->lChangeAmount != 0)
	{
		pIServerUserItem->WriteUserWealth(pWealthUpdate->wMoneyID, pWealthUpdate->lChangeAmount, GOODS_REASON_SYNC);
	}	

	return true;
}

//写分结果
bool CAttemperEngineSink::OnDBWriteScoreResult(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	BYTE cbReApply=*(BYTE*)pData;
	if (cbReApply==1)
	{
		WORD wBindIndex=LOWORD(dwContextID);
		IServerUserItem * pIServerUserItem = GetServerUserItem(wBindIndex,dwTokenID);
		if(pIServerUserItem!=NULL)
		{
			//修改权限
			pIServerUserItem->ModifyUserRight(0,UR_MATCH_USER);

			//发送通知
			CMD_GR_ConfigUserRight cur = {0};
			cur.dwUserRight=pIServerUserItem->GetUserRight();

			SendDataToUser(pIServerUserItem,MDM_GR_CONFIG,SUB_GR_CONFIG_USER_RIGHT,&cur,sizeof(cur) );
		}
	}
	
	return true;
}

//低保结果
bool CAttemperEngineSink::OnDBLargessSubsidy(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT (wDataSize<=sizeof(DBO_GR_LargessSubsidy));
	if (wDataSize > sizeof(DBO_GR_LargessSubsidy)) return true;

	//提取数据
	DBO_GR_LargessSubsidy * pLargessSubsidy = (DBO_GR_LargessSubsidy *)pData;

	//获取用户
	IServerUserItem * pIServerUserItem = GetServerUserItem(LOWORD(dwContextID), dwTokenID);
	if (pIServerUserItem == NULL)
	{
		pIServerUserItem = m_ServerUserManager.SearchUserItem(pLargessSubsidy->dwUserID);
		if (pIServerUserItem == NULL) return true;
	}

	//设置标识
	pIServerUserItem->SetLargessSubsidy(false);	

	//执行成功
	if (pLargessSubsidy->bResultCode==true)
	{		
		//获取信息
		auto pUserInfo=pIServerUserItem->GetUserInfo();

		//更新金币
		pUserInfo->lGold = pLargessSubsidy->lCurrGold;

		//更新分数
		if (m_pGameServiceOption->wServerType & (GAME_GENRE_GOLD | GAME_GENRE_BATTLE))
		{
			pUserInfo->lScore = pLargessSubsidy->lCurrGold;
		}

		//转换类型
		CServerUserItem * pServerUserItem=(CServerUserItem *)pIServerUserItem;

		//设置锁分
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

		//发送分数
		SendUserScore(pIServerUserItem);

		//分数变更
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			//变量定义
			CTableFrameItem * pTableFrame = m_TableFrameArray[pIServerUserItem->GetTableID()];
			if (pTableFrame!=NULL) pTableFrame->OnEventUserScoreChange(pIServerUserItem);
		}

		//更新次数
		pIServerUserItem->SetResidualTimes(pLargessSubsidy->cbResidualTimes);

		//构造提示
		LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0034, TEXT("您当前游戏币低于" SCORE_STRING "，系统已自动补足至" SCORE_STRING "，您今天的低分赠送次数还有 % d 次！"), SCORE_ZOOMOUT(m_SystemParameter.lTakeSubsidyCondition), SCORE_ZOOMOUT(pLargessSubsidy->lCurrGold), pLargessSubsidy->cbResidualTimes);
		
		//发送消息
		SendGameMessage(pIServerUserItem, pszMessage,SMT_CHAT|SMT_EJECT|SMT_SHOW_MOBILE);
	}
	else
	{
		//发送消息
		//SendGameMessage(pIServerUserItem,pLargessSubsidy->szDescribeString,SMT_CHAT|SMT_EJECT|SMT_SHOW_MOBILE);
	}

	//检查桌子
	if (pIServerUserItem->GetTableID()>=m_TableFrameArray.GetCount()) return true;

	//获取桌子
	CTableFrameItem * pTableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];

	//踢出检测
	SCORE lServiceFee=0;
	SCORE lLessScore=m_pGameServiceOption->lLessScore;		
	if (m_pGameServiceOption->lLessScore!=0 && CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule))
	{
		lServiceFee=m_pGameServiceOption->lRevenue;
	}

	//获取设置
	SCORE lBaseScore=pTableFrame->GetCellScore();	
	SCORE lStandupScore = __max(lBaseScore * pTableFrame->m_wStandupMultiple, lLessScore) + lServiceFee;

	//积分限制
	if (pIServerUserItem->GetUserScore() < lStandupScore)
	{
		//构造提示
		LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0035, TEXT("亲爱的%s,您的游戏币少于" SCORE_STRING "个，不能继续游戏！"), pIServerUserItem->GetAccounts(), SCORE_ZOOMOUT(lStandupScore));

		//发送消息
		SendGameMessage(pIServerUserItem, pszMessage, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT | SMT_SHOW_MOBILE);

		//用户起立
		pTableFrame->PerformStandUpAction(pIServerUserItem);

		return true;
	}

	//锁定限制
	if ((m_pGameServiceOption->lLessLockScore>0L)&&(pIServerUserItem->GetUserFreeScore()< lStandupScore-lServiceFee))
	{
		//构造提示
		LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0035, TEXT("亲爱的%s,您的游戏币少于" SCORE_STRING "个，不能继续游戏！"), pIServerUserItem->GetAccounts(), SCORE_ZOOMOUT(lStandupScore));

		//发消息
		SendGameMessage(pIServerUserItem, pszMessage, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT | SMT_SHOW_MOBILE);

		//用户起立
		pTableFrame->PerformStandUpAction(pIServerUserItem);

		return true;
	}

	return true;
}

//控制信息
bool CAttemperEngineSink::OnDBUserControlInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//校验参数
	ASSERT(wDataSize == sizeof(DBO_GR_UserControlInfo));
	if (wDataSize != sizeof(DBO_GR_UserControlInfo)) return true;

	//提取数据
	auto pUserControlInfo = (DBO_GR_UserControlInfo*)pData;

	//查找用户
	auto pIServerUserItem = m_ServerUserManager.SearchUserItem(pUserControlInfo->dwUserID);
	if (pIServerUserItem != NULL)
	{
		auto pUserControlItem = pIServerUserItem->GetUserControlItem();
		CopyMemory(pUserControlItem, &pUserControlInfo->UserControlItem,sizeof(tagUserControlItem));
	}

	return true;
}

//控制更新
bool CAttemperEngineSink::OnDBUserControlUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//校验参数
	ASSERT(wDataSize == sizeof(DBO_GR_UserControlUpdate));
	if (wDataSize != sizeof(DBO_GR_UserControlUpdate)) return true;

	//提取数据
	auto pUserControlUpdate = (DBO_GR_UserControlUpdate*)pData;

	//获取用户
	IServerUserItem* pIServerUserItem = GetServerUserItem(LOWORD(dwContextID), dwTokenID);
	if (pIServerUserItem != NULL)
	{
		auto pUserControlItem = pIServerUserItem->GetUserControlItem();
		if (pUserControlItem->cbControlStatus != pUserControlUpdate->cbControlStatus && pUserControlUpdate->cbControlStatus == CONTROL_STATUS_NORMAL)
		{
			//重置数据
			ZeroMemory(pUserControlItem,sizeof(tagUserControlItem));
		}		
	}

	return true;
}

//注册事件
bool CAttemperEngineSink::OnTCPSocketMainStoreRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_SUCCESS:		//注册成功
		{
			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_STORE_RESULT,&ControlResult,sizeof(ControlResult));

			//设置标识
			m_bCacheVideo = false;

			//负荷信息
			tagBurthenInfo BurthenInfo;
			m_VideoDataQueue.GetBurthenInfo(BurthenInfo);			

			//发送数据
			if (BurthenInfo.dwDataPacketCount > 0)
			{
				//变量定义
				tagDataHead DataHead;
				BYTE cbBuffer[SOCKET_PACKET];

				do
				{
					//提取数据
					if (m_VideoDataQueue.DistillData(DataHead, cbBuffer, CountArray(cbBuffer), true) == false)
					{
						break;
					}

					//存储数据
					//CString s;
					//s.Format(_T("存储数据2 "));
					//CTraceService::TraceString(s, TraceLevel_Debug);
					StoreVideoData(cbBuffer, DataHead.wDataSize);

				} while (true);
			}

			return true;
		}
	case SUB_CS_S_REGISTER_FAILURE:		//注册失败
		{
			//效验参数
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//关闭处理
			m_bNeedStore=false;
			m_pITCPSocketStore->CloseSocket();

			//显示消息
			if (lstrlen(pRegisterFailure->szDescribeString) > 0)
			{
				CTraceService::TraceString(pRegisterFailure->szDescribeString, TraceLevel_Exception);
			}

			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_STORE_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return false;
}

//注册事件
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	
	case SUB_CS_S_REGISTER_FAILURE:		//注册失败
		{
			//效验参数
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//关闭处理
			m_bNeekCenter=false;
			m_pITCPSocketCenter->CloseSocket();

			//显示消息
			if (lstrlen(pRegisterFailure->szDescribeString) > 0)
			{
				CTraceService::TraceString(pRegisterFailure->szDescribeString, TraceLevel_Exception);
			}

			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_FAILURE;
			SendUIControlPacket(UI_CENTER_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	case SUB_CS_S_REGISTER_SUCCESS:		//注册成功
		{		
			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CENTER_RESULT,&ControlResult,sizeof(ControlResult));

			//创建日志
			g_TimeLoggerService->InitFormat(TEXT("GameLogger"), TEXT("%Y%m%d%H"), new CLogByMinute(TEXT("%Y%m%d%H%M"), 30));				

			return true;
		}
	}

	return true;
}

//注册事件
bool CAttemperEngineSink::OnTCPSocketMainServiceRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_FAILURE:		//注册失败
		{
			//效验参数
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;
			ASSERT (wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//关闭处理
			m_bNeedService=false;
			m_pITCPSocketService->CloseSocket();

			//显示消息
			if (lstrlen(pRegisterFailure->szDescribeString) > 0)
			{
				CTraceService::TraceString(pRegisterFailure->szDescribeString, TraceLevel_Exception);
			}

			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_FAILURE;
			SendUIControlPacket(UI_SERVICE_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	case SUB_CS_S_REGISTER_SUCCESS:		//注册成功
		{
			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_SERVICE_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//列表事件
bool CAttemperEngineSink::OnTCPSocketMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SERVICE_INSERT:		//服务插入
		{
			//效验参数
			ASSERT (wDataSize==sizeof(tagServiceItem));
			if (wDataSize!=sizeof(tagServiceItem)) return false;

			//提取数据
			tagServiceItem * pServiceItem =(tagServiceItem *)(pData);

			//存储服务
			if (pServiceItem->wServiceModule == SERVICE_MODULE_STORE)
			{
				if ((m_ServiceItem.wServiceID == 0 || m_ServiceItem.wServiceID == MAX_SERVICE_ID) &&
					(pServiceItem->wServiceID == MAX_SERVICE_ID || pServiceItem->wServiceID == m_pGameServiceAttrib->wKindID))
				{
					CopyMemory(&m_StoreServer, pServiceItem, sizeof(tagServiceItem));
				}
			}

			//比赛服务
			if (pServiceItem->wServiceModule == SERVICE_MODULE_MATCH)
			{
				CopyMemory(&m_ServiceItem, pServiceItem, sizeof(tagServiceItem));				
			}

			//约战服务
			if (pServiceItem->wServiceModule == SERVICE_MODULE_BATTLE)
			{
				CopyMemory(&m_ServiceItem, pServiceItem, sizeof(tagServiceItem));		
			}

			//排位服务
			if (pServiceItem->wServiceModule == SERVICE_MODULE_RANKING)
			{
				CopyMemory(&m_ServiceItem, pServiceItem, sizeof(tagServiceItem));
			}

			//日志服务
			if (pServiceItem->wServiceModule == SERVICE_MODULE_LOGGER)
			{
				//日志服务
				g_TimeLoggerService->SetLoggerServer(pServiceItem->szServiceDomain, pServiceItem->wServicePort);
			}		

			return true;			
		}	
	}

	return true;
}


//服务信息
bool CAttemperEngineSink::OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_USER_SHUTUP_STATUS:	//禁言状态
	{
		//校验数据
		ASSERT(wDataSize == sizeof(CMD_CS_UserShutUpStatus));
		if (wDataSize != sizeof(CMD_CS_UserShutUpStatus)) return true;

		//提取数据
		auto pUserShutUpStatus = (CMD_CS_UserShutUpStatus*)pData;

		//查找用户
		auto pIServerUserItem = m_ServerUserManager.SearchUserItem(pUserShutUpStatus->dwUserID);
		if (pIServerUserItem != NULL)
		{
			pIServerUserItem->SetShutUpStatus(pUserShutUpStatus->cbShutUpStatus);
			pIServerUserItem->SetShutUpOverTime(pUserShutUpStatus->dwShutUpOverTime);
		}

		return true;
	}
	case SUB_CS_USER_CONTROL_APPEND:	//添加控制
	{
		//校验数据
		ASSERT(wDataSize == sizeof(CMD_CS_UserControlAppend));
		if (wDataSize != sizeof(CMD_CS_UserControlAppend)) return true;

		//提取数据
		auto pUserControlAppend = (CMD_CS_UserControlAppend*)pData;

		//查询用户
		auto pIServerUserItem = m_ServerUserManager.SearchUserItem(pUserControlAppend->dwUserID);
		if (pIServerUserItem != NULL)
		{
			//构造结构
			DBR_GR_ControlQuery ControlQuery;
			ZeroMemory(&ControlQuery,sizeof(ControlQuery));

			//设置标识
			ControlQuery.dwUserID = pUserControlAppend->dwUserID;

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_CONTROL_QUERY, ControlQuery.dwUserID, 0L, 0L, &ControlQuery, sizeof(ControlQuery));
		}

		return true;
	}
	case SUB_CS_USER_CONTROL_REMOVE:	//移除控制
	{
		//校验数据
		ASSERT(wDataSize == sizeof(CMD_CS_UserControlRemove));
		if (wDataSize != sizeof(CMD_CS_UserControlRemove)) return true;

		//提取数据
		auto pUserControlRemove = (CMD_CS_UserControlRemove*)pData;

		//查询用户
		auto pIServerUserItem = m_ServerUserManager.SearchUserItem(pUserControlRemove->dwUserID);
		if (pIServerUserItem != NULL)
		{
			//控制结束
			OnEventUserControlFinish(pIServerUserItem);

			//清除控制
			auto pUserControlItem = pIServerUserItem->GetUserControlItem();
			ZeroMemory(pUserControlItem,sizeof(tagUserControlItem));
		}

		return true;
	}
	case SUB_CS_USER_RECHARGE_NOTICE:	//充值通知
	{
		//校验数据
		ASSERT(wDataSize <= sizeof(CMD_CS_UserRechargeNotice));
		if (wDataSize > sizeof(CMD_CS_UserRechargeNotice)) return true;

		//提取数据
		auto pUserRechargeNotice = (CMD_CS_UserRechargeNotice*)pData;

		//查找用户
		auto pIServerUserItem = m_ServerUserManager.SearchUserItem(pUserRechargeNotice->dwUserID);
		if (pIServerUserItem == NULL) return true;

		//解析数据
		CGoodsItemArray MoneyArray, GoodsArray;
		if (g_GameGoodsManager->ParseFromJson(pUserRechargeNotice->szGoodsList, MoneyArray, GoodsArray))
		{
			if (MoneyArray.GetCount()==1 && MoneyArray[0].wGoodsID == GOODS_ID_GOLD && MoneyArray[0].dwGoodsCount>0)
			{
				//构造结构
				CMD_GR_RechargeNotice RechargeNotice;
				ZeroMemory(&RechargeNotice,sizeof(RechargeNotice));

				//设置变量
				RechargeNotice.dwUserID = pUserRechargeNotice->dwUserID;
				RechargeNotice.lPresentScore = MoneyArray[0].dwGoodsCount;

				//发送消息
				SendDataToUser(pIServerUserItem, MDM_GR_USER, SUB_GR_RECHARGE_NOTICE, &RechargeNotice, sizeof(RechargeNotice));

				//增加积分
				pIServerUserItem->WriteUserWealth(MoneyArray[0].wGoodsID, RechargeNotice.lPresentScore, GOODS_REASON_SYNC);
			}
		}

		return true;
	}
	case SUB_CS_APPEND_ROBOT_PARAMETER:		//添加参数
	{
		//参数校验
		ASSERT(sizeof(CMD_CS_AppendRobotParameter) == wDataSize);
		if (sizeof(CMD_CS_AppendRobotParameter) != wDataSize) return false;

		//提取数据
		auto pAddParameter = (CMD_CS_AppendRobotParameter*)pData;

		//添加参数
		m_RobotUserManager.AddRobotParameter(&pAddParameter->RobotParameter, 1);

		return true;
	}
	case SUB_CS_MODIFY_ROBOT_PARAMETER:		//修改参数
	{
		//参数校验
		ASSERT(sizeof(CMD_CS_ModifyRobotParameter) == wDataSize);
		if (sizeof(CMD_CS_ModifyRobotParameter) != wDataSize) return false;

		//提取数据
		auto pModifyParameter = (CMD_CS_ModifyRobotParameter*)pData;

		//修改参数
		m_RobotUserManager.AddRobotParameter(&pModifyParameter->RobotParameter, 1);

		return true;
	}
	case SUB_CS_DELETE_ROBOT_PARAMETER:		//删除参数
	{
		//参数校验
		ASSERT(sizeof(CMD_CS_DeleteRobotParameter) == wDataSize);
		if (sizeof(CMD_CS_DeleteRobotParameter) != wDataSize) return false;

		//提取数据
		auto pDeleteParameter = (CMD_CS_DeleteRobotParameter*)pData;

		//删除参数
		m_RobotUserManager.RemoveRobotParameter(pDeleteParameter->dwBatchID);

		return true;
	}
	case SUB_CS_LOGOUT_NOTIFY:	//登出通知
	{
		ASSERT(wDataSize == sizeof(CMD_CS_LogoutNotify));
		CMD_CS_LogoutNotify* pLogoout = (CMD_CS_LogoutNotify*)pData;
		//查找用户
		auto pIServerUserItem = m_ServerUserManager.SearchUserItem(pLogoout->dwUserID);
		if (pIServerUserItem == NULL) return true;

		WORD wTableID = pIServerUserItem->GetTableID();
		//离开处理
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

//汇总事件
bool CAttemperEngineSink::OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_COLLECT_USER:			//用户汇总
		{
			//变量定义
			CMD_CS_C_EnterServer EnterServer;
			ZeroMemory(&EnterServer,sizeof(EnterServer));

			//发送用户
			WORD wEnumIndex=0;
			IServerUserItem * pIServerUserItem = NULL;

			do
			{
				//获取用户
				pIServerUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
				if (pIServerUserItem==NULL) break;

				//设置变量
				EnterServer.dwUserID = pIServerUserItem->GetUserID();
				EnterServer.dwGameID = pIServerUserItem->GetGameID();
				EnterServer.cbGender = pIServerUserItem->GetUserInfo()->cbGender;
				EnterServer.wFaceID = pIServerUserItem->GetUserInfo()->wFaceID;
				EnterServer.dwCustomID = pIServerUserItem->GetUserInfo()->dwCustomID;
				StringCchCopy(EnterServer.szNickName, CountArray(EnterServer.szNickName), pIServerUserItem->GetAccounts());

				//发送数据
				ASSERT(m_pITCPSocketCenter!=NULL);
				m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_ENTER_SERVER,&EnterServer,sizeof(EnterServer),0);

			} while (true);

			//汇报完成
			m_bCollectUser=true;
			m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_COLLECT_FINISH,0);

			return true;
		}
	}

	return true;
}

//查询事件
bool CAttemperEngineSink::OnTCPSocketMainQueryService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch(wSubCmdID)
	{
	case SUB_CS_S_QUERY_USER_RESULT:		//查询结果
		{
			//变量定义
			CMD_CS_S_QueryUserResult *pQueryResult = (CMD_CS_S_QueryUserResult *)pData;
			WORD wRecBaseSize = sizeof(CMD_CS_S_QueryUserResult)-sizeof(pQueryResult->szAccounts)+CountStringBuffer(pQueryResult->szAccounts);

			//效验
			ASSERT( wDataSize >= sizeof(CMD_CS_S_QueryUserResult)-sizeof(pQueryResult->szAccounts)+
				CountStringBuffer(pQueryResult->szAccounts) );
			if( wDataSize < sizeof(CMD_CS_S_QueryUserResult)-sizeof(pQueryResult->szAccounts)+
				CountStringBuffer(pQueryResult->szAccounts) )
				return false;

			//目标玩家
			IServerUserItem *pIServerUserItem = m_ServerUserManager.SearchUserItem(pQueryResult->dwSendUserID);
			if( !pIServerUserItem ) return true;

			//构造结果
			BYTE cbBuffer[SOCKET_PACKET] = {0};
			tagGlobalUserInfo *pGlobalUserInfo = (tagGlobalUserInfo *)cbBuffer;
			pGlobalUserInfo->dwGameID = pQueryResult->dwGameID;
			pGlobalUserInfo->dwUserID = pQueryResult->dwUserID;
			StringCchCopy( pGlobalUserInfo->szNickName,CountArray(pGlobalUserInfo->szNickName), pQueryResult->szAccounts);

			//携带信息
			WORD wBaseSize = sizeof(tagGlobalUserInfo)-sizeof(pGlobalUserInfo->szNickName)+CountStringBuffer(pGlobalUserInfo->szNickName);
			CopyMemory( cbBuffer+wBaseSize,(BYTE *)pData+wRecBaseSize,wDataSize-wRecBaseSize );

			//发送信息
			SendDataToUser(pIServerUserItem,MDM_GR_QUERY,SUB_GR_QUERY_USER_RESULT,cbBuffer,wBaseSize+wDataSize-wRecBaseSize);

			return true;
		}
	case SUB_CS_S_QUERY_NOT_FOUND:			//查询不到
		{
			//效验
			ASSERT( wDataSize == sizeof(CMD_CS_S_QueryNotFound) );
			if( wDataSize != sizeof(CMD_CS_S_QueryNotFound) )
				return false;

			//变量定义
			CMD_CS_S_QueryNotFound *pNotFound = (CMD_CS_S_QueryNotFound *)pData;

			//目标玩家
			IServerUserItem *pIServerUserItem = m_ServerUserManager.SearchUserItem(pNotFound->dwSendUserID);
			if( !pIServerUserItem ) return true;

			//构造消息
			CMD_GR_QueryNotFound NotFound = {0};
			NotFound.nResultCode = QUERY_NOT_FOUND;
			
			//发送信息
			WORD wSendSize = sizeof(NotFound)-sizeof(NotFound.szDescribeString)+CountStringBuffer(NotFound.szDescribeString);
			SendDataToUser(pIServerUserItem,MDM_GR_QUERY,SUB_GR_QUERY_NOT_FOUND,&NotFound,wSendSize);

			return true;
		}
	}

	return true;
}

//用户登录
VOID CAttemperEngineSink::OnEventUserLogon(IServerUserItem * pIServerUserItem)
{
	//获取参数
	WORD wBindIndex=pIServerUserItem->GetBindIndex();
	bool bRobotUser=pIServerUserItem->IsRobotUser();
	tagBindParameter * pBindParameter=GetBindParameter(pIServerUserItem->GetBindIndex());

	//登录处理
	if (pIServerUserItem->IsMobileUser()==false) 
	{
		//登录成功
		CMD_GR_LogonSuccess LogonSuccess;
		LogonSuccess.dwUserID=pIServerUserItem->GetUserID();
		LogonSuccess.dwUserRight=pIServerUserItem->GetUserRight();
		LogonSuccess.dwMasterRight=pIServerUserItem->GetMasterRight();
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_LOGON,SUB_GR_LOGON_SUCCESS,&LogonSuccess,sizeof(LogonSuccess));

		//列表配置
		const WORD wColumnInfoHead=sizeof(CMD_ConfigColumn)-sizeof(CMD_ConfigColumn.ColumnItem);
		WORD wColumnInfoSize=wColumnInfoHead+CMD_ConfigColumn.cbColumnCount*sizeof(CMD_ConfigColumn.ColumnItem[0]);
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_CONFIG,SUB_GR_CONFIG_COLUMN,&CMD_ConfigColumn,wColumnInfoSize);

		//房间配置
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_CONFIG,SUB_GR_CONFIG_SERVER,&CMD_ConfigServer,sizeof(CMD_ConfigServer));
		
		//配置完成
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_CONFIG,SUB_GR_CONFIG_FINISH,NULL,0);

		//玩家数据
		SendUserEnterPacket(pIServerUserItem,pBindParameter->dwContextID, pIServerUserItem->GetTokenID(),false,true);

		//在线用户
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

		//桌子状态
		CMD_GR_TableInfo TableInfo;
		TableInfo.wTableCount=(WORD)m_TableFrameArray.GetCount();
		ASSERT(TableInfo.wTableCount<CountArray(TableInfo.TableStatusArray));
		for (WORD i=0;i<TableInfo.wTableCount;i++)
		{
			CTableFrameItem * pTableFrame=m_TableFrameArray[i];
			TableInfo.TableStatusArray[i].cbTableLock=pTableFrame->IsTableLocked()?TRUE:FALSE;
			TableInfo.TableStatusArray[i].cbPlayStatus=pTableFrame->IsTableStarted()?TRUE:FALSE;
		}

		//桌子状态
		WORD wHeadSize=sizeof(TableInfo)-sizeof(TableInfo.TableStatusArray);
		WORD wSendSize=wHeadSize+TableInfo.wTableCount*sizeof(TableInfo.TableStatusArray[0]);
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_STATUS,SUB_GR_TABLE_INFO,&TableInfo,wSendSize);

		//显示底分
		if (CServerRule::IsForbidShowBaseScore(m_pGameServiceOption->dwServerRule)==false)
		{
			//桌子底分
			CMD_GR_TableScore TableScore;
			TableScore.wTableCount=(WORD)m_TableFrameArray.GetCount();
			ASSERT(TableScore.wTableCount<CountArray(TableScore.lTableScoreArray));
			for (WORD i=0;i<TableScore.wTableCount;i++)
			{
				CTableFrameItem * pTableFrame=m_TableFrameArray[i];
				TableScore.lTableScoreArray[i]=pTableFrame->GetCellScore();
			}

			//桌子底分
			wHeadSize=sizeof(TableScore)-sizeof(TableScore.lTableScoreArray);
			wSendSize=wHeadSize+TableScore.wTableCount*sizeof(TableScore.lTableScoreArray[0]);
			SendDataToGate(pBindParameter->dwContextID, pIServerUserItem->GetTokenID(),MDM_GR_STATUS,SUB_GR_TABLE_SCORE,&TableScore,wSendSize);
		}	

		//登录完成
		SendUserEnterPacket(pIServerUserItem,INVALID_DWORD,0,true);
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_LOGON,SUB_GR_LOGON_FINISH,NULL,0);
	}
	else
	{
		//变量定义
		CMD_GR_ConfigServer ConfigServer;
		ZeroMemory(&ConfigServer,sizeof(ConfigServer));

		//房间配置
		ConfigServer.wTableCount=m_pGameServiceOption->wTableCount;
		ConfigServer.wChairCount=m_pGameServiceAttrib->wChairCount;
		ConfigServer.wServerType=m_pGameServiceOption->wServerType;
		ConfigServer.dwServerRule=m_pGameServiceOption->dwServerRule;
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_CONFIG,SUB_GR_CONFIG_SERVER,&ConfigServer,sizeof(ConfigServer));

		//配置完成
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_CONFIG,SUB_GR_CONFIG_FINISH,NULL,0);

		//用户进入
		SendUserEnterPacket(pIServerUserItem,pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),true);

		//玩家数据
		SendTableUserToUser(pIServerUserItem,CLIENT_KIND_MOBILE,INVALID_DWORD);

		//群发用户
		SendUserEnterPacket(pIServerUserItem,INVALID_DWORD,0,true);

		//登录完成
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_LOGON,SUB_GR_LOGON_FINISH,NULL,0);

		//旁观用户
		if (pIServerUserItem->GetUserStatus()==US_LOOKON)
		{
			CTraceService::TraceString(TEXT("用户起立：OnEventUserLogon"), TraceLevel_Info);
			m_TableFrameArray[pIServerUserItem->GetTableID()]->PerformStandUpAction(pIServerUserItem);
		}

		//桌子状态
		CMD_GR_TableInfo TableInfo;
		TableInfo.wTableCount=(WORD)m_TableFrameArray.GetCount();
		ASSERT(TableInfo.wTableCount<CountArray(TableInfo.TableStatusArray));
		for (WORD i=0;i<TableInfo.wTableCount;i++)
		{
			CTableFrameItem * pTableFrame=m_TableFrameArray[i];
			TableInfo.TableStatusArray[i].cbTableLock=pTableFrame->IsTableLocked()?TRUE:FALSE;
			TableInfo.TableStatusArray[i].cbPlayStatus=pTableFrame->IsTableStarted()?TRUE:FALSE;			
		}

		//桌子状态
		WORD wHeadSize=sizeof(TableInfo)-sizeof(TableInfo.TableStatusArray);
		WORD wSendSize=wHeadSize+TableInfo.wTableCount*sizeof(TableInfo.TableStatusArray[0]);
		SendDataToGate(pBindParameter->dwContextID,pIServerUserItem->GetTokenID(),MDM_GR_STATUS,SUB_GR_TABLE_INFO,&TableInfo,wSendSize);
		
		//坐下处理
		if (pIServerUserItem->GetTableID()==INVALID_TABLE)
		{
			//金币房间立即坐下
			if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0) MobileUserImmediately(pIServerUserItem);					}	
	}

	return;
}

//用户登出
VOID CAttemperEngineSink::OnEventUserLogout(IServerUserItem * pIServerUserItem)
{
	//校验参数
	ASSERT (pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return;

	//更新控制
	if (pIServerUserItem->GetUserControlItem()->cbControlStatus != CONTROL_STATUS_NORMAL && pIServerUserItem->GetUserControlItem()->cbControlKind != CONTROL_KIND_CUSTOM)
	{
		OnEventUserControlUpdate(pIServerUserItem);		
	}

	//获取对象
	auto pServerUserPack = pIServerUserItem->GetUserPack();
	
	//更新物品
	if (pServerUserPack->GetVariationCount() > 0)
	{
		//构造对象
		DBR_GR_UserUpdateGoods UserUpdateGoods;
		ZeroMemory(&UserUpdateGoods, sizeof(UserUpdateGoods));

		//用户标识
		UserUpdateGoods.dwUserID = pIServerUserItem->GetUserID();

		//提取物品
		if (pServerUserPack->DistillVariationGoods(UserUpdateGoods.szGoodsList, CountArray(UserUpdateGoods.szGoodsList)))
		{
			//投递数据	
			WORD wDataSize = CountStringBuffer(UserUpdateGoods.szGoodsList);
			WORD wHeadSize = sizeof(UserUpdateGoods) - sizeof(UserUpdateGoods.szGoodsList);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_UPDATE_GOODS, UserUpdateGoods.dwUserID, pIServerUserItem->GetBindIndex(), pIServerUserItem->GetTokenID(), &UserUpdateGoods, wHeadSize + wDataSize);
		}

		//构造对象
		DBR_GR_UserRecordGoods UserRecordGoods;
		ZeroMemory(&UserRecordGoods, sizeof(UserRecordGoods));

		//用户标识
		UserRecordGoods.dwUserID = pIServerUserItem->GetUserID();

		//提取记录
		if (pServerUserPack->DistillVariationRecord(UserRecordGoods.szRecordList, CountArray(UserRecordGoods.szRecordList)))
		{
			//投递数据	
			WORD wDataSize = CountStringBuffer(UserRecordGoods.szRecordList);
			WORD wHeadSize = sizeof(UserRecordGoods) - sizeof(UserRecordGoods.szRecordList);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_RECORD_GOODS, UserRecordGoods.dwUserID, pIServerUserItem->GetBindIndex(), pIServerUserItem->GetTokenID(), &UserRecordGoods, wHeadSize + wDataSize);
		}

		//清除变更
		pServerUserPack->CleanVariation();
	}

	//变量定义
	DBR_GR_LeaveGameService LeaveGameService;
	ZeroMemory(&LeaveGameService,sizeof(LeaveGameService));

	//提取积分
	tagLockScoreInfo LockScoreInfo;
	tagVariationScore VariationScore;
	ZeroMemory(&LockScoreInfo,sizeof(LockScoreInfo));
	ZeroMemory(&VariationScore,sizeof(VariationScore));
	pIServerUserItem->DistillVariation(VariationScore,LockScoreInfo);
	pIServerUserItem->QueryRecordInfo(LeaveGameService.RecordInfo);

	//用户信息
	LeaveGameService.dwUserID=pIServerUserItem->GetUserID();
	LeaveGameService.dwClientIP=pIServerUserItem->GetClientIP();
	LeaveGameService.dwPlayTimeCount=pIServerUserItem->GetTotalPlayTimeCount();
	LeaveGameService.dwOnLineTimeCount=static_cast<DWORD>(time(NULL))-pIServerUserItem->GetLogonTime();

	//积分信息
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

	//调整分数
	if (pIServerUserItem->IsRobotUser() == true)
	{
		LeaveGameService.lScore = 0;
		LeaveGameService.lRevenue = 0;
	}

	//投递请求
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LEAVE_SERVICE, LeaveGameService.dwUserID,0L,0L,&LeaveGameService, sizeof(LeaveGameService));
	
	//汇总用户
	if (m_bCollectUser==true)
	{
		CMD_CS_C_LeaveServer UserLeave;
		UserLeave.dwUserID=pIServerUserItem->GetUserID();
		m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT,SUB_CS_C_LEAVE_SERVER,&UserLeave,sizeof(UserLeave),0);
	}

	//登出通知
	if (m_pIExtendServiceManager!=NULL) 
	{
		m_pIExtendServiceManager->OnEventUserLogout(pIServerUserItem);
	}

	//移除分组
	RemoveDistribute(pIServerUserItem);

	//删除用户
	m_ServerUserManager.DeleteUserItem(pIServerUserItem);	

	//更新人数
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0)
	{
		//变量定义
		CMD_CS_C_ServiceOnline ServiceOnLine;
		ZeroMemory(&ServiceOnLine, sizeof(ServiceOnLine));

		//设置变量
		ServiceOnLine.wOnlineCount = m_ServerUserManager.GetUserItemCount();

		//发送信息
		ASSERT(m_pITCPSocketCenter != NULL);
		m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_SERVICE_ONLINE, &ServiceOnLine, sizeof(ServiceOnLine), 0);
	}

	return;
}

//切换连接
VOID CAttemperEngineSink::SwitchUserItemConnect(IServerUserItem * pIServerUserItem, WORD wTargetIndex, DWORD dwTokenID, bool bMobileUser)
{
	//效验参数
	ASSERT((pIServerUserItem!=NULL)&&(wTargetIndex!=INVALID_WORD_INDEX));
	if ((pIServerUserItem==NULL)||(wTargetIndex==INVALID_WORD_INDEX)) return;

	//变量定义
	bool bRobotUser = (wTargetIndex >= INDEX_ROBOT);
	tagTokenParameter * pTokenParameter = GetTokenParameter(wTargetIndex, dwTokenID);
	if (pTokenParameter == NULL) return;

	//断开用户
	if (pIServerUserItem->GetBindIndex()!=INVALID_WORD_INDEX && pIServerUserItem->GetTokenID()!=0)
	{
		//发送通知
		SendRoomMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0038,TEXT("请注意，您的帐号在另一地方进入此游戏房间了，您被迫离开！")),SMT_CHAT|SMT_EJECT|SMT_GLOBAL|SMT_CLOSE_ROOM);
		
		//断开用户
		if (pIServerUserItem->IsRobotUser()==true)
		{
			//获取参数
			tagBindParameter * pBindParameter = GetBindParameter(pIServerUserItem->GetBindIndex());
			m_RobotUserManager.DeleteRobotUserItem(pBindParameter->dwContextID,false);
		}
		else
		{
			//构造结构
			CMD_CS_C_ShutdownSocket ShutdownSocket;
			ShutdownSocket.bForceClose = true;

			//发送数据
			SendDataToUser(pIServerUserItem, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));
		}

		//删除令牌
		auto pSourceParameter = GetBindParameter(pIServerUserItem->GetBindIndex());
		auto pGlobalGateItem = pSourceParameter->pGlobalGateItem;
		if (pGlobalGateItem != NULL)
		{
			pGlobalGateItem->RemoveTokenID(pIServerUserItem->GetTokenID());
		}
	}		

	//激活用户
	pIServerUserItem->SetMobileUser(bMobileUser);	
	pIServerUserItem->SetUserParameter(pTokenParameter->dwClientIP,wTargetIndex,dwTokenID,bRobotUser,false);

	//绑定用户
	pTokenParameter->pBindUserItem=pIServerUserItem;

	//状态切换
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

	//登录事件
	OnEventUserLogon(pIServerUserItem);

	//登录通知
	if (m_pIExtendServiceManager!=NULL) 
	{
		m_pIExtendServiceManager->OnEventUserLogon(pIServerUserItem);
	}

	return;
}

//版本检查
bool CAttemperEngineSink::PerformCheckVersion(DWORD dwDeviceKind, DWORD dwPlazaVersion, DWORD dwClientVersion, DWORD dwSocketID, DWORD dwTokenID)
{
	//标识定义
	bool bMustUpdatePlaza=false;
	bool bMustUpdateClient=false,bAdviceUpdateClient=false;
	bool bSpecialPlazaVer=false,bSpecialClientVer=false;

	//版本定义	
	DWORD dwCurrentPlazaVersion=0;	
	DWORD dwCurrentClientVersion=m_pGameServiceAttrib->dwClientVersion;

	//变量定义
	tagModuleVersion * pModuleVersion=NULL;

	//特殊广场版本
	pModuleVersion=GetSpecialVersion(0,dwDeviceKind,dwPlazaVersion);
	if (pModuleVersion!=NULL) bSpecialPlazaVer=true;

	//特殊游戏版本
	pModuleVersion=GetSpecialVersion(m_pGameServiceAttrib->wKindID,dwDeviceKind, dwPlazaVersion);
	if (pModuleVersion!=NULL) bSpecialClientVer=true;

	//当前广场版本
	pModuleVersion=GetCurrentVersion(0,dwDeviceKind);
	if (pModuleVersion!=NULL) dwCurrentPlazaVersion=pModuleVersion->dwVersion;

	//当前游戏版本
	pModuleVersion=GetCurrentVersion(m_pGameServiceAttrib->wKindID,dwDeviceKind);
	if (pModuleVersion!=NULL) dwCurrentClientVersion=pModuleVersion->dwVersion;

	//大厅版本
	if (!bSpecialPlazaVer)
	{
		//校验版本
		if (GetMainVer(dwPlazaVersion)!=GetMainVer(dwCurrentPlazaVersion)) bMustUpdatePlaza=true;
		if (GetProductVer(dwPlazaVersion)!=GetProductVer(dwCurrentPlazaVersion)) bMustUpdatePlaza=true;
	}

	//游戏版本
	if (!bSpecialClientVer)
	{
		//游戏版本
		if (GetSubVer(dwClientVersion)<GetSubVer(dwCurrentClientVersion)) bAdviceUpdateClient=true;
		if (GetMainVer(dwClientVersion)!=GetMainVer(dwCurrentClientVersion)) bMustUpdateClient=true;
		if (GetProductVer(dwClientVersion)!=GetProductVer(dwCurrentClientVersion)) bMustUpdateClient=true;
	}

	//更新通知
	if ((bMustUpdatePlaza==true)||(bMustUpdateClient==true)||(bAdviceUpdateClient==true))
	{
		//变量定义
		CMD_GR_UpdateNotify UpdateNotify;
		ZeroMemory(&UpdateNotify,sizeof(UpdateNotify));

		//变量定义
		UpdateNotify.cbMustUpdatePlaza=bMustUpdatePlaza;
		UpdateNotify.cbMustUpdateClient=bMustUpdateClient;
		UpdateNotify.cbAdviceUpdateClient=bAdviceUpdateClient;

		//当前版本
		UpdateNotify.dwCurrentFrameVersion=VERSION_FRAME;
		UpdateNotify.dwCurrentPlazaVersion=dwCurrentPlazaVersion;		
		UpdateNotify.dwCurrentClientVersion=dwCurrentClientVersion;

		//发送消息
		SendDataToGate(dwSocketID,dwTokenID,MDM_GR_LOGON,SUB_GR_UPDATE_NOTIFY,&UpdateNotify,sizeof(UpdateNotify));

		//中止判断
		if ((bMustUpdatePlaza==true)||(bMustUpdateClient==true))
		{
			ASSERT(LOWORD(dwSocketID)<INDEX_ROBOT);

			//构造结构
			CMD_CS_C_ShutdownSocket ShutdownSocket;
			ShutdownSocket.bForceClose = false;

			//发送数据
			SendDataToGate(dwSocketID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));

			return false;
		}
	}

	return true;
}

//登录失败
bool CAttemperEngineSink::SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID, DWORD dwTokenID)
{
	//获取参数
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem = GetServerUserItem(wBindIndex,dwTokenID);
	if (pIServerUserItem!=NULL) return true;

	//构造数据
	CMD_GR_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));

	//错误信息
	LogonFailure.lErrorCode=lErrorCode;	
	StringCchCopy(LogonFailure.szDescribeString,CountArray(LogonFailure.szDescribeString), pszString);

	//数据属性
	WORD wStringSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wSendSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString)+wStringSize;

	//发送数据
	SendDataToGate(dwSocketID,dwTokenID,MDM_GR_LOGON,SUB_GR_LOGON_FAILURE,&LogonFailure,wSendSize);

	return true;
}

//发送用户
bool CAttemperEngineSink::SendUserEnterPacket(IServerUserItem * pIServerUserItem, DWORD dwSocketID, DWORD dwTokenID, bool bAllLinks, bool bNotifySink)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	bool bHideUserInfo = false;

	//获取用户
	WORD wBindIndex = LOWORD(dwSocketID);
	IServerUserItem* pITargetUserItem = GetServerUserItem(wBindIndex, dwTokenID);

	//调整信息
	if (bNotifySink == false && (pITargetUserItem == NULL || pITargetUserItem->GetMasterOrder() == 0))
	{
		//隐藏信息
		if (CServerRule::IsHideUserInfo(m_pGameServiceOption->dwServerRule) == true)
		{
			bHideUserInfo = true;			
		}
	}

	//缓存定义
	BYTE cbMBBuffer[SOCKET_PACKET/2] = { 0 };
	BYTE cbPCBuffer[SOCKET_PACKET/2] = { 0 };

	//构造结构
	WORD wMBDataSize = ConstructEnterPacket(pIServerUserItem, cbMBBuffer, sizeof(cbMBBuffer), CLIENT_KIND_MOBILE, bHideUserInfo);
	WORD wPCDataSize = ConstructEnterPacket(pIServerUserItem, cbPCBuffer, sizeof(cbPCBuffer), CLIENT_KIND_COMPUTER, bHideUserInfo);	

	//发送数据
	if (pITargetUserItem==NULL)
	{
		//发给电脑
		SendBatchData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_ENTER, cbPCBuffer, wPCDataSize);

		//发给手机
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

//请求失败
bool CAttemperEngineSink::SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, BYTE cbFailureCode)
{
	//构造数据
	CMD_GR_RequestFailure RequestFailure;
	RequestFailure.cbFailureCode=cbFailureCode;
	StringCchCopy(RequestFailure.szDescribeString,CountArray(RequestFailure.szDescribeString), pszDescribe);

	//数据数据
	WORD wStringSize=CountStringBuffer(RequestFailure.szDescribeString);
	WORD wSendSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString)+wStringSize;

	//发送数据
	SendDataToUser(pIServerUserItem,MDM_GR_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wSendSize);

	//构造结构
	CMD_CS_C_ShutdownSocket ShutdownSocket;
	ShutdownSocket.bForceClose = false;

	//发送数据
	SendDataToUser(pIServerUserItem, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket));

	return true;
}

//绑定参数
tagBindParameter * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//无效连接
	if (wBindIndex==INVALID_WORD_INDEX) return NULL;

	//常规连接
	if (wBindIndex<m_pGameServiceOption->wMaxPlayer)
	{
		return m_pNormalParameter+wBindIndex;
	}

	//机器连接
	if ((wBindIndex>=INDEX_ROBOT)&&(wBindIndex<(INDEX_ROBOT+MAX_ROBOT)))
	{
		return m_pRobotParameter+(wBindIndex-INDEX_ROBOT);
	}

	//错误断言
	ASSERT(FALSE);

	return NULL;
}

//获取用户
IServerUserItem * CAttemperEngineSink::GetServerUserItem(WORD wBindIndex, DWORD dwTokenID)
{
	//绑定参数
	auto pTokenParameter = GetTokenParameter(wBindIndex, dwTokenID);
	if (pTokenParameter == NULL)
	{
		return NULL;
	}

	return (IServerUserItem*)pTokenParameter->pBindUserItem;
}

//令牌参数
tagTokenParameter * CAttemperEngineSink::GetTokenParameter(WORD wBindIndex, DWORD dwTokenID)
{
	//绑定参数
	tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);
	if (pBindParameter == NULL) return NULL;

	//直连模式
	if (pBindParameter->wServiceModule == SERVICE_MODULE_NONE)
	{
		return &pBindParameter->TokenParameter;
	}
	else
	{
		//查询网关
		auto pGlobalGateItem = pBindParameter->pGlobalGateItem;
		if (pGlobalGateItem == NULL) return NULL;

		//查询令牌
		auto pTokenParameter = pGlobalGateItem->SearchTokenID(dwTokenID);
		if (pTokenParameter == NULL || pTokenParameter->dwTokenID != dwTokenID) return NULL;

		return pTokenParameter;
	}

	return NULL;
}

//扩展参数
tagExtendParameter* CAttemperEngineSink::GetExtendParameter(WORD wBindIndex, DWORD dwTokenID)
{
	//绑定参数
	auto pTokenParameter = GetTokenParameter(wBindIndex, dwTokenID);
	if (pTokenParameter == NULL)
	{
		return NULL;
	}

	return (tagExtendParameter*)pTokenParameter->cbExtendData;
}

//当前版本
tagModuleVersion * CAttemperEngineSink::GetCurrentVersion(WORD wModuleID,DWORD dwDeviceKind)
{
	//特殊版本	
	for (INT_PTR i=0;i<m_ModuleVersionActive.GetCount();i++)
	{
		//判断类型
		if (m_ModuleVersionActive[i].IsCurrent==false) continue;		
		if (m_ModuleVersionActive[i].wModuleID!=wModuleID) continue;		
		if (m_ModuleVersionActive[i].dwDeviceKind!=dwDeviceKind) continue;

		return &m_ModuleVersionActive[i];
	}

	return NULL;
}

//特殊版本
tagModuleVersion * CAttemperEngineSink::GetSpecialVersion(WORD wModuleID,DWORD dwDeviceKind,DWORD dwVersion)
{
	//特殊版本	
	for (INT_PTR i=0;i<m_ModuleVersionActive.GetCount();i++)
	{
		//判断类型
		if (m_ModuleVersionActive[i].IsCurrent==true) continue; 
		if (m_ModuleVersionActive[i].wModuleID!=wModuleID) continue;
		if (m_ModuleVersionActive[i].dwVersion!=dwVersion) continue;
		if (m_ModuleVersionActive[i].dwDeviceKind!=dwDeviceKind) continue;

		return &m_ModuleVersionActive[i];
	}

	return NULL;
}

//用户状况
WORD CAttemperEngineSink::GetRobotUserStatus(tagRobotUserStatus & RobotUserStatus)
{
	//变量定义
	WORD wEnumIndex = 0;
	IServerUserItem * pIServerUserItem=NULL;

	//设置变量
	ZeroMemory(&RobotUserStatus,sizeof(RobotUserStatus));

	//用户信息
	do
	{
		//获取用户
		pIServerUserItem=m_ServerUserManager.EnumUserItem(wEnumIndex++);
		if (pIServerUserItem == NULL) break;

		//统计判断
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->IsRobotUser()==true))
		{
			//获取属性
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//用户统计
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

//桌子状况
WORD CAttemperEngineSink::GetTableUserStatus(CTableFrameItem * pTableFrame, tagTableUserStatus & TableUserStatus)
{
	//设置变量
	ZeroMemory(&TableUserStatus,sizeof(TableUserStatus));

	//用户分析
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//用户类型
		if (pIServerUserItem->IsRobotUser()==false)
		{
			TableUserStatus.wTableUserCount++;
		}
		else
		{
			TableUserStatus.wTableRobotCount++;
		}
	}

	//最少数目
	switch (pTableFrame->GetStartMode())
	{
	case StartMode_AllReady:	//所有准备
		{
			if(m_pGameServiceAttrib->wKindID==57)
				TableUserStatus.wLessUserCount=m_pGameServiceOption->wLessStartPlayer;
			else 
				TableUserStatus.wLessUserCount=2;

			break;
		}
	default:					//默认模式
		{
			TableUserStatus.wLessUserCount=m_pGameServiceAttrib->wChairCount;
			break;
		}
	}

	return TableUserStatus.wTableRobotCount+TableUserStatus.wTableUserCount;
}


//手机立即登录
bool CAttemperEngineSink::MobileUserImmediately(IServerUserItem * pIServerUserItem)
{
	//参数校验
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//分组模式
	if (m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)
	{
		return InsertDistribute(pIServerUserItem);
	}

	//查找模式
	BYTE cbSearchMode=SEARCH_MODE_MAX_PLAYER;
	if (CServerRule::IsMatchUserByScore(m_pGameServiceOption->dwServerRule))
	{
		//更新分数
		m_ScoreManager.UpdateScoreData();

		cbSearchMode|=SEARCH_MASK_MATCH_SCORE;
	}

	//查找并坐下
	return SearchTableAndSitdown(cbSearchMode,pIServerUserItem);
}

//发可视用户信息
bool CAttemperEngineSink::SendTableUserToUser(IServerUserItem * pITargetUserItem, BYTE cbClientKind, DWORD dwTableUserID)
{
	//校验用户
	ASSERT (pITargetUserItem != NULL);
	if (pITargetUserItem == NULL) return false;

	//扩展参数
	auto pExtendParameter = GetExtendParameter(pITargetUserItem->GetBindIndex(), pITargetUserItem->GetTokenID());
	if (pExtendParameter == NULL) return true;
	if ((pExtendParameter->cbClientKind & cbClientKind) == 0) return true;

	//桌子标识
	WORD wTableID = pITargetUserItem->GetTableID();
	if (wTableID >= m_TableFrameArray.GetCount()) return true;	

	//缓冲定义
	WORD wDataSize = 0;
	BYTE cbBuffer[SOCKET_PACKET/2];		

	//变量定义
	WORD wEnumIndex = 0;
	CTableFrameItem* pTableFrame = m_TableFrameArray[wTableID];
	
	//隐藏信息
	bool bHideUserInfo = false;
	if (CServerRule::IsHideUserInfo(m_pGameServiceOption->dwServerRule) == true) bHideUserInfo = true;	

	//特定玩家
	if (dwTableUserID != INVALID_DWORD)
	{
		auto pITableUserItem=pTableFrame->SearchUserItem(dwTableUserID);
		if (pITableUserItem != NULL && pITableUserItem!= pITargetUserItem)
		{
			//构造数据包
			wDataSize = ConstructEnterPacket(pITableUserItem, cbBuffer, sizeof(cbBuffer), pExtendParameter->cbClientKind, bHideUserInfo);

			//发送消息
			SendDataToUser(pITargetUserItem, MDM_GR_USER, SUB_GR_USER_ENTER, cbBuffer, wDataSize);
		}
	}
	else
	{
		//在座用户
		for (int i = 0; i < pTableFrame->GetChairCount(); i++)
		{
			auto pITableUserItem = pTableFrame->GetTableUserItem(i);
			if (pITableUserItem == NULL) continue;
			if (pITableUserItem == pITargetUserItem) continue;

			//构造数据包
			wDataSize = ConstructEnterPacket(pITableUserItem, cbBuffer, sizeof(cbBuffer), pExtendParameter->cbClientKind, bHideUserInfo);

			//发送消息
			SendDataToUser(pITargetUserItem, MDM_GR_USER, SUB_GR_USER_ENTER, cbBuffer, wDataSize);			
		}

		//旁观用户
		do
		{
			//枚举用户
			auto pITableUserItem = pTableFrame->EnumLookonUserItem(wEnumIndex++);
			if (pITableUserItem == NULL) break;
			if (pITableUserItem == pITargetUserItem) continue;

			//构造数据包
			wDataSize = ConstructEnterPacket(pITableUserItem, cbBuffer, sizeof(cbBuffer), pExtendParameter->cbClientKind, bHideUserInfo);

			//发送消息
			SendDataToUser(pITargetUserItem, MDM_GR_USER, SUB_GR_USER_ENTER, cbBuffer, wDataSize);

		} while (true);
	}
	
	return true;
}

//发送信息
bool CAttemperEngineSink::SendUserToTableUser(IServerUserItem* pIServerUserItem, BYTE cbClientKind, DWORD dwTableUserID)
{
	//校验用户
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return false;

	//桌子标识
	WORD wTableID = pIServerUserItem->GetTableID();
	if (wTableID >= m_TableFrameArray.GetCount()) return true;

	//变量定义
	WORD wEnumIndex = 0;
	CTableFrameItem* pTableFrame = m_TableFrameArray[wTableID];

	//缓冲定义
	BYTE cbPCBuffer[SOCKET_PACKET/2];
	BYTE cbMBBuffer[SOCKET_PACKET/2];

	//隐藏信息
	bool bHideUserInfo = false;
	if (CServerRule::IsHideUserInfo(m_pGameServiceOption->dwServerRule) == true) bHideUserInfo = true;

	//构造数据包
	auto wMBDataSize = ConstructEnterPacket(pIServerUserItem, cbMBBuffer, sizeof(cbMBBuffer), CLIENT_KIND_MOBILE, bHideUserInfo);
	auto wPCDataSize = ConstructEnterPacket(pIServerUserItem, cbPCBuffer, sizeof(cbPCBuffer),  CLIENT_KIND_COMPUTER, bHideUserInfo);
	
	//特定玩家
	if (dwTableUserID != INVALID_DWORD)
	{
		auto pITableUserItem = pTableFrame->SearchUserItem(dwTableUserID);
		if (pITableUserItem != NULL && pITableUserItem != pIServerUserItem)
		{
			//扩展参数
			auto pExtendParameter = GetExtendParameter(pITableUserItem->GetBindIndex(), pITableUserItem->GetTokenID());
			if (pExtendParameter != NULL && (pExtendParameter->cbClientKind & cbClientKind) != 0)
			{
				//发送消息
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
		//在座用户
		for (int i = 0; i < pTableFrame->GetChairCount(); i++)
		{
			auto pITableUserItem = pTableFrame->GetTableUserItem(i);
			if (pITableUserItem == NULL) continue;
			if (pITableUserItem == pIServerUserItem) continue;

			//扩展参数
			auto pExtendParameter = GetExtendParameter(pITableUserItem->GetBindIndex(), pITableUserItem->GetTokenID());
			if (pExtendParameter != NULL && (pExtendParameter->cbClientKind & cbClientKind) != 0)
			{
				//发送消息
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

		//旁观用户
		do
		{
			//枚举用户
			auto pITableUserItem = pTableFrame->EnumLookonUserItem(wEnumIndex++);
			if (pITableUserItem == NULL) break;
			if (pITableUserItem == pIServerUserItem) continue;

			//扩展参数
			auto pExtendParameter = GetExtendParameter(pITableUserItem->GetBindIndex(), pITableUserItem->GetTokenID());
			if (pExtendParameter != NULL && (pExtendParameter->cbClientKind & cbClientKind) != 0)
			{
				//发送消息
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

//发送请求
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//发送数据
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->m_UIControlSink.PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//执行分组
bool CAttemperEngineSink::PerformDistribute()
{
	//人数校验
	if (m_DistributeManager.GetNodeCount()<m_pGameServiceOption->wMinDistributeUser)
	{
		return false;
	}

	//分配用户
	while (true)
	{
		//变量定义
		CDistributeInfoArray DistributeInfoArray;

		 //获取用户
		WORD wRandCount=__max(m_pGameServiceOption->wMaxPartakeGameUser-m_pGameServiceOption->wMinPartakeGameUser+1,1);
		WORD wNeedCount=m_pGameServiceOption->wMinPartakeGameUser+rand()%wRandCount;
		wNeedCount=__min(m_DistributeManager.GetNodeCount(),wNeedCount);
		
		//执行分组
		if (m_DistributeManager.PerformDistribute(DistributeInfoArray,wNeedCount)==false)
		{
			break;
		}

		//查找桌子
		WORD wFindTableID=SearchSuitableTable(SEARCH_MODE_NO_PLAYER,NULL);
		if (wFindTableID==INVALID_TABLE) break;

		//变量定义
		bool bSitSuccess=true;
		IServerUserItem * pIFailedUserItem=NULL;
		CTableFrameItem * pTableFrame=m_TableFrameArray[wFindTableID];

		//玩家坐下		
 		for (INT_PTR i=0; i<DistributeInfoArray.GetCount(); i++)
		{
			//变量定义
			WORD wChairID=pTableFrame->GetNullChairID();
			if (wChairID==INVALID_CHAIR) break;
				
			//获取用户
			IServerUserItem * pIServerUserItem=DistributeInfoArray[i]->pIServerUserItem;
			if (pIServerUserItem!=NULL)
			{
				//用户起立
				if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
				{
					ITableFrameItem * pITableFrame=m_TableFrameArray[pIServerUserItem->GetTableID()];
					pITableFrame->PerformStandUpAction(pIServerUserItem);
				}

				//用户坐下
				if (pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem)==false)
				{
					bSitSuccess=false;
					pIFailedUserItem=pIServerUserItem;
					break;
				}
			}	
		}

		//结果处理
		for (INT_PTR i=0; i<DistributeInfoArray.GetCount(); i++)
		{
			//用户指针
			IServerUserItem * pIServerUserItem=DistributeInfoArray[i]->pIServerUserItem;

			//成功处理
			if (bSitSuccess==true || pIFailedUserItem==pIServerUserItem)
			{
				//移除分组
				RemoveDistribute(pIServerUserItem);
				continue;
			}

			//用户起立
			if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
			{
				//插入分组
				InsertDistribute(pIServerUserItem);
			}
		}
	}

	return true;
}

//统计令牌
DWORD CAttemperEngineSink::CollectTokenCount()
{
	//变量定义	
	DWORD dwTokenCount = 0;
	CGlobalGateItem * pGlobalGateItem= NULL;

	do
	{
		//枚举网关
		pGlobalGateItem = m_GlobalItemManager.EnumGateItem(pGlobalGateItem);
		if (pGlobalGateItem == NULL) break;

		//统计数量
		dwTokenCount += pGlobalGateItem->GetTokenCount();

	} while (true);

	return dwTokenCount;
}

//查找桌子
WORD CAttemperEngineSink::SearchSuitableTable(BYTE cbSearchMode,IServerUserItem * pIServerUserItem, WORD wLastTableID)
{
	//设置变量
	WORD wRequestTableID=INVALID_TABLE;

	//无人桌子
	if (cbSearchMode&SEARCH_MODE_NO_PLAYER)
	{
		//查找桌子		
		for (WORD i=0; i<m_pGameServiceOption->wTableCount; i++)
		{
			//获取桌子
			CTableFrameItem * pTableFrame=m_TableFrameArray[i];

			//判断状态
			if (pTableFrame->IsTableLocked() || pTableFrame->IsGameStarted()) continue;			
			if (pTableFrame->GetNullChairCount()!=pTableFrame->GetChairCount()) continue;

			//设置变量
			wRequestTableID=i;
			break;
		}

		return wRequestTableID;
	}

	//变量定义
	WORD wTableWeight=0;	
	WORD wTableWeightLevel=0;
	
	//动态加入
	bool bDynamicJoin=true;
	if (m_pGameServiceAttrib->cbDynamicJoin==FALSE) bDynamicJoin=false;
	if (CServerRule::IsDynamicJoinGame(m_pGameServiceOption->dwServerRule)==false) bDynamicJoin=false;	

	//查找桌子
	WORD wStartIndex=rand()%m_pGameServiceOption->wTableCount;
	for (WORD i=0; i<m_pGameServiceOption->wTableCount; i++)
	{
		//获取桌子
		WORD wTableIndex=(i+wStartIndex)%m_pGameServiceOption->wTableCount;		
		CTableFrameItem * pTableFrame=m_TableFrameArray[wTableIndex];

		//条件过滤
		if (pTableFrame->IsTableLocked()) continue;
		if (pTableFrame->GetTableID() == wLastTableID) continue;
		if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;		
		if (pTableFrame->GetNullChairCount()==pTableFrame->GetChairCount()) continue;
		if (pTableFrame->m_wMaxPlayerCount<=pTableFrame->GetSitUserCount()) continue;

		//桌子信息
		tagTableUserInfo TableUerInfo;
		pTableFrame->GetTableUserInfo(TableUerInfo);

		//计算权值
		WORD wCurrWeightLevel=0,wCurrTableWeight=0;

		//匹配分数
		if (cbSearchMode&SEARCH_MASK_MATCH_SCORE && pIServerUserItem!=NULL)
		{
			//获取区间
			INT nAreaIndex1=GetTableScoreAreaIndex(pTableFrame);				
			INT nAreaIndex2=m_ScoreManager.GetScoreAreaIndex(pIServerUserItem->GetUserFreeScore());	

			//匹配索引
			if (nAreaIndex1!=nAreaIndex2) continue;			
		}

		//最少玩家
		if (cbSearchMode&SEARCH_MODE_MIN_PLAYER)
		{
			wCurrTableWeight=TableUerInfo.wTableReadyCount;
			wCurrWeightLevel=(TableUerInfo.wTableUserCount==0)?TableUerInfo.wTableRobotCount:0;
			wCurrTableWeight+=wCurrWeightLevel*m_pGameServiceAttrib->wChairCount;
		}

		//最多玩家
		if (cbSearchMode&SEARCH_MODE_MAX_PLAYER)
		{
			wCurrTableWeight=TableUerInfo.wTableReadyCount;
			wCurrWeightLevel=TableUerInfo.wTableUserCount+TableUerInfo.wTableRobotCount;			
			wCurrTableWeight+=wCurrWeightLevel*m_pGameServiceAttrib->wChairCount;
		}

		//权重比较
		if (wCurrTableWeight>=wTableWeight && wCurrWeightLevel>=wTableWeightLevel)
		{
			//记录权重
			wRequestTableID=wTableIndex;				
			wTableWeight=wCurrTableWeight;
			wTableWeightLevel=wCurrWeightLevel;
		}
	}

	//判断结果
	if (wRequestTableID==INVALID_TABLE && cbSearchMode&SEARCH_MODE_MIN_PLAYER)
	{
		//查找空桌
		wStartIndex=rand()%m_pGameServiceOption->wTableCount;
		for (WORD i=0; i<m_pGameServiceOption->wTableCount; i++)
		{
			//获取桌子
			WORD wTableIndex=(i+wStartIndex)%m_pGameServiceOption->wTableCount;			
			CTableFrameItem * pTableFrame=m_TableFrameArray[wTableIndex];

			//判断状态
			if (pTableFrame->IsTableLocked()) continue;
			if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;			
			if (pTableFrame->GetNullChairCount()<pTableFrame->GetChairCount()) continue;	

			//设置变量
			wRequestTableID=pTableFrame->GetTableID();
			break;
		}
	}

	//判断结果
	if (wRequestTableID==INVALID_TABLE)
	{
		//查找桌子
		wStartIndex=rand()%m_pGameServiceOption->wTableCount;
		for (WORD i=0; i<m_pGameServiceOption->wTableCount; i++)
		{
			//获取桌子
			WORD wTableIndex=(i+wStartIndex)%m_pGameServiceOption->wTableCount;			
			CTableFrameItem * pTableFrame=m_TableFrameArray[wTableIndex];

			//判断状态
			if (pTableFrame->IsTableLocked()) continue;			
			if ((pTableFrame->IsGameStarted()==true)&&(bDynamicJoin==false)) continue;			
			if (pTableFrame->m_wMaxPlayerCount<=pTableFrame->GetSitUserCount()) continue;

			//匹配分数
			if (cbSearchMode&SEARCH_MASK_MATCH_SCORE && pIServerUserItem!=NULL && pTableFrame->GetSitUserCount()>0)
			{
				//获取区间
				INT nAreaIndex1=GetTableScoreAreaIndex(pTableFrame);				
				INT nAreaIndex2=m_ScoreManager.GetScoreAreaIndex(pIServerUserItem->GetUserFreeScore());	

				//匹配索引
				if (nAreaIndex1!=nAreaIndex2) continue;			
			}

			//设置变量
			wRequestTableID=pTableFrame->GetTableID();
			break;
		}
	}

	return wRequestTableID;
}

//查找坐下
bool CAttemperEngineSink::SearchTableAndSitdown(BYTE cbSearchMode,IServerUserItem * pIServerUserItem, WORD wLastTableID)
{
	//查找桌子
	WORD wFindTableID=SearchSuitableTable(cbSearchMode,pIServerUserItem,wLastTableID);
	if (wFindTableID!=INVALID_TABLE)
	{
		//变量定义
		CTableFrameItem * pTableFrame=m_TableFrameArray[wFindTableID];

		//无效过滤
		WORD wChairID=pTableFrame->GetRandNullChairID();
		if (wChairID!=INVALID_CHAIR)
		{
			//起立处理
			if (pIServerUserItem->GetTableID()!=INVALID_TABLE && pIServerUserItem->GetChairID()!=INVALID_CHAIR)
			{
				CTableFrameItem * pTableFramePrep=m_TableFrameArray[pIServerUserItem->GetTableID()];
				pTableFramePrep->PerformStandUpAction(pIServerUserItem);
			}

			//用户坐下
			pTableFrame->PerformSitDownAction(wChairID,pIServerUserItem);

			return true;
		}
	}

	//失败
	SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0039,TEXT("没找到可进入的游戏桌！")),REQUEST_FAILURE_NORMAL);

	return true;
}


//用户进入
WORD CAttemperEngineSink::ConstructEnterPacket(IServerUserItem *pIServerUserItem,VOID * pBuffer,WORD wBufferSize, BYTE cbClientKind, bool bHideUserInfo)
{
	//参数校验
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return 0;

	//转换对象
	auto pUserInfo=pIServerUserItem->GetUserInfo();
	auto pServerUserItem = (CServerUserItem*)pIServerUserItem;

	//终端类型
	if (cbClientKind == CLIENT_KIND_COMPUTER)
	{
		//构造结构
		auto pUserInfoHead = (tagPCUserInfoHead*)pBuffer;
		CSendPacketHelper SendPacket((PBYTE)pBuffer + sizeof(tagPCUserInfoHead), wBufferSize - sizeof(tagPCUserInfoHead));

		//用户属性
		pUserInfoHead->bIsRobot = true;
		pUserInfoHead->wFaceID = pUserInfo->wFaceID;
		pUserInfoHead->dwGameID = pUserInfo->dwGameID;
		pUserInfoHead->dwUserID = pUserInfo->dwUserID;
		pUserInfoHead->dwCustomID = pUserInfo->dwCustomID;

		//用户属性
		pUserInfoHead->cbAge = pUserInfo->cbAge;
		pUserInfoHead->cbGender = pUserInfo->cbGender;
		pUserInfoHead->cbMasterOrder = pUserInfo->cbMasterOrder;

		//用户状态
		pUserInfoHead->wTableID = pUserInfo->wTableID;
		pUserInfoHead->wChairID = pUserInfo->wChairID;
		pUserInfoHead->cbUserStatus = pUserInfo->cbUserStatus;

		//用户积分
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

		//用户账号
		SendPacket.AddPacket(pUserInfo->szAccounts, DTP_GR_USER_NICKNAME);//34

		//用户签名
		SendPacket.AddPacket(pUserInfo->szUnderWrite, DTP_GR_USER_UNDER_WRITE);//66

	
		/*CString s;
		s.Format(_T("bbbb szUserHeadUrl:%s, wBufferSize:%d - sizeof(tagPCUserInfoHead):%d"), pUserInfo->szUserHeadUrl, wBufferSize , sizeof(tagPCUserInfoHead));
		CTraceService::TraceString(s, TraceLevel_Info);
		*/

		return sizeof(tagPCUserInfoHead) + SendPacket.GetDataSize();		
	}
	else
	{
		//构造结构
		auto pUserInfoHead = (tagMBUserInfoHead*)pBuffer;
		CSendPacketHelper SendPacket((PBYTE)pBuffer + sizeof(tagMBUserInfoHead), wBufferSize - sizeof(tagMBUserInfoHead));

		//用户属性
		pUserInfoHead->wFaceID = pUserInfo->wFaceID;
		pUserInfoHead->dwGameID = pUserInfo->dwGameID;
		pUserInfoHead->dwUserID = pUserInfo->dwUserID;
		pUserInfoHead->dwCustomID = pUserInfo->dwCustomID;
		
		//用户属性
		pUserInfoHead->cbAge = pUserInfo->cbAge;
		pUserInfoHead->cbGender = pUserInfo->cbGender;
		pUserInfoHead->cbMasterOrder = pUserInfo->cbMasterOrder;

		//用户状态
		pUserInfoHead->wTableID = pUserInfo->wTableID;
		pUserInfoHead->wChairID = pUserInfo->wChairID;
		pUserInfoHead->cbUserStatus = pUserInfo->cbUserStatus;

		//用户积分
		pUserInfoHead->lGold = pUserInfo->lGold;
		pUserInfoHead->lScore = pUserInfo->lScore;
		pUserInfoHead->cbScoreKind = pUserInfo->cbScoreKind;
		pUserInfoHead->lWinCount = pUserInfo->lWinCount;
		pUserInfoHead->lLostCount = pUserInfo->lLostCount;
		pUserInfoHead->lDrawCount = pUserInfo->lDrawCount;
		pUserInfoHead->lFleeCount = pUserInfo->lFleeCount;
		pUserInfoHead->lExperience = pUserInfo->lExperience;
		pUserInfoHead->lWinStreak = pUserInfo->lWinStreak;

		//位置信息
		pUserInfoHead->dGlobalPosLat = pServerUserItem->GetGlobalPosLat();
		pUserInfoHead->dGlobalPosLng = pServerUserItem->GetGlobalPosLng();
		CopyMemory(pUserInfoHead->szPlaceName, pServerUserItem->GetGlobalPlace(),sizeof(pUserInfoHead->szPlaceName));

		//隐藏信息
		if (bHideUserInfo == true)
		{
			pUserInfoHead->dwGameID = 0;
			pUserInfoHead->dwCustomID = 0;

			//比赛房间
			if ((m_pGameServiceOption->wServerType & GAME_GENRE_MATCH) != 0)
			{
				pUserInfoHead->wFaceID = pUserInfoHead->cbGender == GENDER_MANKIND ? 10 : 11;
				SendPacket.AddPacket(TEXT("参赛者"), DTP_GR_USER_NICKNAME);
			}
			else
			{
				pUserInfoHead->wFaceID = pUserInfoHead->cbGender == GENDER_MANKIND ? 0 : 35;
				SendPacket.AddPacket(TEXT("游戏玩家"), DTP_GR_USER_NICKNAME);
			}
			//KK JIA
			SendPacket.AddPacket(pUserInfo->szUserHeadUrl, DTP_GR_USER_HEADURL);//258

		}
		else
		{
			//用户账号
			SendPacket.AddPacket(pUserInfo->szAccounts, DTP_GR_USER_NICKNAME);

			//用户签名
			SendPacket.AddPacket(pUserInfo->szUnderWrite, DTP_GR_USER_UNDER_WRITE);
			//KK JIA
			SendPacket.AddPacket(pUserInfo->szUserHeadUrl, DTP_GR_USER_HEADURL);//258

		}

		return sizeof(tagMBUserInfoHead) + SendPacket.GetDataSize();
	}

	return 0;
}

//用户分数
WORD CAttemperEngineSink::ConstructScorePacket(IServerUserItem * pIServerUserItem,VOID * pBuffer,WORD wBufferSize)
{
	//参数校验
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return 0;

	//用户信息
	auto pUserInfo=pIServerUserItem->GetUserInfo();

	//变量定义
	auto pUserScore=(CMD_GR_UserScore*)pBuffer;

	//积分信息
	pUserScore->UserScore.lScore = pUserInfo->lScore;
	pUserScore->UserScore.cbScoreKind = pUserInfo->cbScoreKind;

	//局数信息
	pUserScore->dwUserID=pUserInfo->dwUserID;
	pUserScore->UserScore.lWinCount=pUserInfo->lWinCount;
	pUserScore->UserScore.lLostCount=pUserInfo->lLostCount;
	pUserScore->UserScore.lDrawCount=pUserInfo->lDrawCount;
	pUserScore->UserScore.lFleeCount=pUserInfo->lFleeCount;
	pUserScore->UserScore.lWinStreak = pUserInfo->lWinStreak;
	pUserScore->UserScore.lExperience=pUserInfo->lExperience;	

	return sizeof(CMD_GR_UserScore);	
}

//用户状态
WORD CAttemperEngineSink::ConstructStatusPacket(IServerUserItem *pIServerUserItem,VOID * pBuffer,WORD wBufferSize)
{
	//参数校验
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return 0;

	//获取结构
	auto pUserStatus=(CMD_GR_UserStatus *)pBuffer;

	//获取信息
	auto pUserInfo=pIServerUserItem->GetUserInfo();

	//构造数据
	pUserStatus->dwUserID=pUserInfo->dwUserID;
	pUserStatus->UserStatus.wTableID=pUserInfo->wTableID;
	pUserStatus->UserStatus.wChairID=pUserInfo->wChairID;
	pUserStatus->UserStatus.cbUserStatus=pUserInfo->cbUserStatus;

	return sizeof(CMD_GR_UserStatus);
}

//效验私聊权限
bool CAttemperEngineSink::EfficacyUserWhisper(IServerUserItem *pISendUserItem, IServerUserItem *pIRecvUserItem)
{
	//状态判断
	BYTE cbMasterOrderSend=pISendUserItem->GetMasterOrder();
	BYTE cbMasterOrderRecv=pIRecvUserItem->GetMasterOrder();
	if ((g_GlobalUnits.IsAllowWisper()==false)&&(cbMasterOrderSend==0)&&(cbMasterOrderRecv==0))
	{
		//不是会员
		if( !g_GlobalUnits.IsMemberCanChat() || pISendUserItem->GetMemberOrder() == 0 )
		{
			SendRoomMessage(pISendUserItem, CWHService::FormatMessage(MIM_SERVER_0040,TEXT("抱歉，此游戏房间不允许用户私聊！")),SMT_EJECT|SMT_CHAT);
			return false;
		}
	}

	//权限判断
	if (CUserRight::CanWisper(pISendUserItem->GetUserRight())==false&&cbMasterOrderSend==0&&cbMasterOrderRecv==0)
	{
		SendRoomMessage(pISendUserItem, CWHService::FormatMessage(MIM_SERVER_0041,TEXT("抱歉，您没有发送私聊的权限，若需要帮助，请联系游戏客服咨询！")),SMT_EJECT|SMT_CHAT);
		return false;
	}

	//同桌禁止私聊
	if( g_GlobalUnits.IsForbidPlayChat() && pISendUserItem->GetUserStatus() == US_PLAYING &&
		pISendUserItem->GetTableID() == pIRecvUserItem->GetTableID() &&
		pIRecvUserItem->GetUserStatus() == US_PLAYING && cbMasterOrderSend == 0 && cbMasterOrderRecv == 0 )
	{
		if( !CUserRight::CanGamePlayChat(pISendUserItem->GetUserRight()) )
		{
			SendRoomMessage(pISendUserItem, CWHService::FormatMessage(MIM_SERVER_0042,TEXT("抱歉，此游戏房间不允许在游戏中与同桌的玩家私聊！")),SMT_EJECT|SMT_CHAT);
			return false;
		}
	}

	return true;
}

//效验连接
bool CAttemperEngineSink::EfficacySocketContext(DWORD dwSocketID, DWORD dwTokenID)
{
	//变量定义
	tagTokenParameter * pTokenParameter=GetTokenParameter(LOWORD(dwSocketID), dwTokenID);
	if (pTokenParameter == NULL) return false;

	//废弃判断
	return (pTokenParameter->dwTokenID==dwTokenID);
}

//广播事件
bool CAttemperEngineSink::OnTCPSocketMainBroadcast(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_GAME_BROADCAST:	//用户喇叭
		{
			//消息定义
			CMD_Buffer * pBuffer=(CMD_Buffer *)pData;

			//效验数据
			ASSERT(wDataSize>=(sizeof(pBuffer->Head)));
			ASSERT(wDataSize==(sizeof(CMD_Buffer)-sizeof(pBuffer->cbBuffer)+pBuffer->Head.CmdInfo.wPacketSize));	
		}
	}
	return true;
}

//时间间隔
SCORE CAttemperEngineSink::GetIntervalSeconds(SYSTEMTIME stStart, SYSTEMTIME stEnd)
{
	CTime tStart(stStart);
	CTime tEnd(stEnd);
	CTimeSpan tsInverval=tStart-tEnd;
	return tsInverval.GetTotalSeconds();
}

//敏感词过滤
bool CAttemperEngineSink::FilterWords(LPCTSTR szFiltered)
{
	return m_WordsFilter.FilterWords(szFiltered);
}

//敏感词过滤
void CAttemperEngineSink::FilterWords(LPWSTR szFiltered, int nMsgLen)
{
	m_WordsFilter.FilterWords(szFiltered, nMsgLen);
}

//管理服务
bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_EXECUTE_COMMAND:			//执行命令
		{
			//参数校验
			ASSERT (sizeof(tagExecuteCommand)==wDataSize);
			if (sizeof(tagExecuteCommand)!=wDataSize) return false;

			//提取数据
			auto pCommandCode=(tagExecuteCommand*)pData;

			//配置掩码
			DBR_GR_LoadConfig LoadConfig;
			ZeroMemory(&LoadConfig, sizeof(LoadConfig));

			//加载配置
			if (pCommandCode->dwCommandCode&CMD_LOAD_CONFIG_DATA)
			{				
				//配置掩码
				LoadConfig.dwConfigMask = pCommandCode->dwCommandParam;

				//开房配置
				if (pCommandCode->dwCommandParam & CONFIG_MASK_BATTLE_OPTION && (m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) == 0)
				{
					LoadConfig.dwConfigMask &= ~CONFIG_MASK_BATTLE_OPTION;
				}

				//投递请求
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_CONFIG, 0, 0, 0, &LoadConfig, sizeof(LoadConfig));
			}						

			return true;
		}
	case SUB_CS_S_MODIFY_OPTION:		//修改选项
		{
			//参数校验
			ASSERT (sizeof(tagModifyOption)==wDataSize);
			if (sizeof(tagModifyOption)!=wDataSize) return false;

			//提取数据
			auto pModifyOption=(tagModifyOption*)pData;

			//旁观游戏
			if (pModifyOption->dwServerRuleDel&SR_FORBID_LOOKON || pModifyOption->dwServerRuleAdd&SR_FORBID_LOOKON)
			{	
				CServerRule::SetForbidLookon(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleDel&SR_FORBID_LOOKON)?true:false);
			}

			//机器占座
			if (pModifyOption->dwServerRuleDel&SR_ALLOW_ROBOT_SIMULATE || pModifyOption->dwServerRuleAdd&SR_ALLOW_ROBOT_SIMULATE)
			{	
				//设置权限
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

			//机器陪打
			if (pModifyOption->dwServerRuleDel&SR_ALLOW_ROBOT_ATTEND || pModifyOption->dwServerRuleAdd&SR_ALLOW_ROBOT_ATTEND)
			{	
				//设置权限
				bool bAllow=g_GlobalUnits.IsAllowRobotAttend();
				g_GlobalUnits.SetAllowRobotAttend((pModifyOption->dwServerRuleAdd&SR_ALLOW_ROBOT_ATTEND)?true:false);
				CServerRule::SetAllowRobotAttend(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleAdd&SR_ALLOW_ROBOT_ATTEND)?true:false);
				//设置定时器
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

			//赠送低保
			if (pModifyOption->dwServerRuleDel&SR_ALLOW_LARGESS_SUBSIDY || pModifyOption->dwServerRuleAdd&SR_ALLOW_LARGESS_SUBSIDY)
			{	
				CServerRule::SetAllowLargessSubsidy(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleAdd&SR_ALLOW_LARGESS_SUBSIDY)?true:false);
			}

			//强退起立
			if (pModifyOption->dwServerRuleDel&SR_FORBIT_FORCE_STANDUP || pModifyOption->dwServerRuleAdd&SR_FORBIT_FORCE_STANDUP)
			{	
				CServerRule::SetForbidForceStandup(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleDel&SR_FORBIT_FORCE_STANDUP)?true:false);
			}

			//掉线起立
			if (pModifyOption->dwServerRuleDel&SR_FORBIT_OFFLINE_STANDUP || pModifyOption->dwServerRuleAdd&SR_FORBIT_OFFLINE_STANDUP)
			{	
				CServerRule::SetForbidOfflineStandup(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleDel&SR_FORBIT_OFFLINE_STANDUP)?true:false);
			}

			//记录成绩
			if (pModifyOption->dwServerRuleDel&SR_RECORD_GAME_SCORE || pModifyOption->dwServerRuleAdd&SR_RECORD_GAME_SCORE)
			{	
				CServerRule::SetRecordGameScore(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleAdd&SR_RECORD_GAME_SCORE)?true:false);
			}

			//游戏录像
			if (pModifyOption->dwServerRuleDel&SR_RECORD_GAME_VIDEO || pModifyOption->dwServerRuleAdd&SR_RECORD_GAME_VIDEO)
			{	
				CServerRule::SetRecordGameVideo(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleAdd&SR_RECORD_GAME_VIDEO)?true:false);
			}

			//即时写分
			if (pModifyOption->dwServerRuleDel&SR_IMMEDIATE_WRITE_SCORE || pModifyOption->dwServerRuleAdd&SR_IMMEDIATE_WRITE_SCORE)
			{	
				CServerRule::SetImmediateWriteScore(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleAdd&SR_IMMEDIATE_WRITE_SCORE)?true:false);
			}

			//动态加入
			if (pModifyOption->dwServerRuleDel&SR_DYNAMIC_JOIN_GAME || pModifyOption->dwServerRuleAdd&SR_DYNAMIC_JOIN_GAME)
			{	
				CServerRule::SetDynamicJoinGame(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleAdd&SR_DYNAMIC_JOIN_GAME)?true:false);
			}	

			//语音聊天
			if (pModifyOption->dwServerRuleDel&SR_ALLOW_VOICE_CHAT || pModifyOption->dwServerRuleAdd&SR_ALLOW_VOICE_CHAT)
			{	
				CServerRule::SetAllowVoiceChat(m_pGameServiceOption->dwServerRule,(pModifyOption->dwServerRuleAdd&SR_ALLOW_VOICE_CHAT)?true:false);
			}

			//胜率设置
			if (pModifyOption->dwTableRuleDel&TR_FORBID_WINRATE_SET || pModifyOption->dwTableRuleAdd&TR_FORBID_WINRATE_SET)
			{	
				CServerRule::SetForbidWinRateSet(m_pGameServiceOption->dwServerRule,(pModifyOption->dwTableRuleDel&TR_FORBID_WINRATE_SET)?true:false);
			}

			//逃率设置
			if (pModifyOption->dwTableRuleDel&TR_FORBID_FLEERATE_SET || pModifyOption->dwTableRuleAdd&TR_FORBID_FLEERATE_SET)
			{	
				CServerRule::SetForbidFleeRateSet(m_pGameServiceOption->dwServerRule,(pModifyOption->dwTableRuleDel&TR_FORBID_FLEERATE_SET)?true:false);
			}

			//地址设置
			if (pModifyOption->dwTableRuleDel&TR_FORBID_IP_LIMIT || pModifyOption->dwTableRuleAdd&TR_FORBID_IP_LIMIT)
			{	
				CServerRule::SetForbidIPLimit(m_pGameServiceOption->dwServerRule,(pModifyOption->dwTableRuleDel&TR_FORBID_IP_LIMIT)?true:false);
			}

			//成绩设置
			if (pModifyOption->dwTableRuleDel&TR_FORBID_SCORE_LIMIT || pModifyOption->dwTableRuleAdd&TR_FORBID_SCORE_LIMIT)
			{	
				CServerRule::SetForbidScoreLimit(m_pGameServiceOption->dwServerRule,(pModifyOption->dwTableRuleDel&TR_FORBID_SCORE_LIMIT)?true:false);
			}

			//不受欢迎设置
			if (pModifyOption->dwTableRuleDel&TR_FORBID_BLACKLIST_SET || pModifyOption->dwTableRuleAdd&TR_FORBID_BLACKLIST_SET)
			{	
				CServerRule::SetForbidBlackListSet(m_pGameServiceOption->dwServerRule,(pModifyOption->dwTableRuleDel&TR_FORBID_BLACKLIST_SET)?true:false);
			}

			//密码设置
			if (pModifyOption->dwTableRuleDel&TR_FORBID_PASSWORD_SET || pModifyOption->dwTableRuleAdd&TR_FORBID_PASSWORD_SET)
			{	
				CServerRule::SetForbidPasswordSet(m_pGameServiceOption->dwServerRule,(pModifyOption->dwTableRuleDel&TR_FORBID_PASSWORD_SET)?true:false);
			}

			//底分显示
			if (pModifyOption->dwTableRuleDel&TR_FORBID_SHOW_BASESCORE || pModifyOption->dwTableRuleAdd&TR_FORBID_SHOW_BASESCORE)
			{	
				CServerRule::SetForbidShowBaseScore(m_pGameServiceOption->dwServerRule,(pModifyOption->dwTableRuleDel&TR_FORBID_SHOW_BASESCORE)?true:false);
			}

			//更新配置
			ModifyGameServer();

			return true;
		}
	}

	return true;
}

//更新金币
bool CAttemperEngineSink::OnDBUpdateGold(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//变量定义
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem *pIServerUserItem=GetServerUserItem(wBindIndex, dwTokenID);
	if(pIServerUserItem==NULL) return false;

	//构造结构
	DBO_GR_RelockScore *pRelockScore=(DBO_GR_RelockScore*)pData;

	//变量定义
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

//配置桌子
bool CAttemperEngineSink::InitTableFrame()
{
	//桌子容器
	m_TableFrameArray.SetSize(m_pGameServiceOption->wTableCount);
	ZeroMemory(m_TableFrameArray.GetData(), m_pGameServiceOption->wTableCount * sizeof(CTableFrameItem*));

	//桌子参数
	tagTableFrameParameter TableFrameParameter;
	ZeroMemory(&TableFrameParameter, sizeof(TableFrameParameter));

	//桌子参数
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

	//创建桌子
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

		//初始化
		if (m_TableFrameArray[i]->InitTableFrame(i, TableFrameParameter) == false) return false;		
	}

	return true;
}

//配置服务
bool CAttemperEngineSink::InitExtendService()
{
	//参数校验
	if(m_pIExtendServiceManager==NULL) return true;

	//桌子参数
	tagExtendServiceParameter ExtendServiceParameter;
	ZeroMemory(&ExtendServiceParameter,sizeof(ExtendServiceParameter));

	//配置参数
	ExtendServiceParameter.pGameServiceOption=m_pGameServiceOption;
	ExtendServiceParameter.pGameServiceAttrib=m_pGameServiceAttrib;

	//内核组件
	ExtendServiceParameter.pITimerEngine=m_pITimerEngine;
	ExtendServiceParameter.pIDataBaseEngine = m_pIDataBaseEngine;
	ExtendServiceParameter.pITCPSocketService=m_pITCPSocketService;
	ExtendServiceParameter.pTCPNetworkEngine=m_pITCPNetworkEngine;	
	ExtendServiceParameter.pITCPSocketCenterServer=m_pITCPSocketCenter;

	//服务组件
	ExtendServiceParameter.pIMainServiceFrame=this;
	ExtendServiceParameter.pIRobotUserManager=&m_RobotUserManager;
	ExtendServiceParameter.pIServerUserManager=QUERY_OBJECT_INTERFACE(m_ServerUserManager,IServerUserManager);

	//初始化接口
	m_pIExtendServiceManager->InitServiceManager(ExtendServiceParameter);

	return true;
}

//配置机器
bool CAttemperEngineSink::InitRobotService()
{
	//机器人类	
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

//录像服务
bool CAttemperEngineSink::InitVideoService()
{
	//录像接口
	if (m_pIGameVideoManager != NULL)
	{
		//变量定义
		tagGameVideoParameter GameVideoParameter;

		//构造结构
		GameVideoParameter.pGameServiceOption = m_pGameServiceOption;
		GameVideoParameter.pGameServiceAttrib = m_pGameServiceAttrib;

		//配置录像
		m_pIGameVideoManager->InitVideoManager(GameVideoParameter);

		//设置接口
		if (m_pIGameVideoManager->SetGameVideoItemSink(QUERY_ME_INTERFACE(IGameVideoItemSink)) == false)
		{
			ASSERT(FALSE);
			return false;
		}		
	}

	return true;
}

//配置房间
bool CAttemperEngineSink::InitServerConfig()
{
	CMD_ConfigServer.wKindID = m_pGameServiceAttrib->wKindID;
	CMD_ConfigServer.wTableCount = m_pGameServiceOption->wTableCount;
	CMD_ConfigServer.wChairCount = m_pGameServiceAttrib->wChairCount;
	CMD_ConfigServer.wServerType = m_pGameServiceOption->wServerType;
	CMD_ConfigServer.dwServerRule = m_pGameServiceOption->dwServerRule;

	return true;
}


//配置日志
bool CAttemperEngineSink::InitLoggerService()
{
	//使能设置
	g_TimeLoggerService->EnableLocal(false);
	g_TimeLoggerService->EnableRemote(true);
	g_TimeLoggerService->EnablePrefix(false);
	g_TimeLoggerService->EnableSufix(true);

	//配置信息
	g_TimeLoggerService->SetFormat(TEXT("%Y%m%d%H%M"));
	g_TimeLoggerService->SetConcatStr(TEXT("_"));
	g_TimeLoggerService->SetSeparator(TEXT("|"));
	g_TimeLoggerService->SetLoggerInfo(TEXT("GameServer"), m_pGameServiceOption->szServerName, m_pInitParameter->m_szLoggerPath);

	//启动服务
	if (g_TimeLoggerService->StartService() == false)
	{
		ASSERT(FALSE);

		return false;
	}

	return true;
}

//游戏结束
bool CAttemperEngineSink::OnEventGameConclude(ITableFrameItem * pITableFrame)
{
	return true;
}

//游戏开始
bool CAttemperEngineSink::OnEventGameStart(ITableFrameItem * pITableFrame)
{
	//分组模式
	if ((m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)!=0 && m_pGameServiceAttrib->wChairCount < MAX_CHAIR)
	{
		//同桌信息
		tagSameTableInfo * pSameTableInfo=m_DistributeManager.GetUserSameTableInfo();
		if (pSameTableInfo!=NULL)
		{
			for (WORD i=0; i<pITableFrame->GetChairCount(); i++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=pITableFrame->GetTableUserItem(i);
				if (pIServerUserItem==NULL) continue;

				//移除同桌
				m_DistributeManager.RemoveSameTableInfo(pIServerUserItem->GetUserID());

				//同桌信息
				pSameTableInfo->wPlayerIDSet[pSameTableInfo->wPlayerCount++]=pIServerUserItem->GetUserID();

				//设置同桌
				m_DistributeManager.SetUserSameTableInfo(pIServerUserItem->GetUserID(),pSameTableInfo);

				//退出判断
				if (pSameTableInfo->wPlayerCount >= CountArray(pSameTableInfo->wPlayerIDSet)) break;
			}
		}
	}

	return true;
}

//插入分组
bool CAttemperEngineSink::InsertDistribute(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//状态判断
	ASSERT (pIServerUserItem->GetUserStatus()<US_PLAYING);
	if (pIServerUserItem->GetUserStatus()>=US_PLAYING) return false;

	//模式判断
	ASSERT (m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE);
	if ((m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)==0) return false;

	//延时模式
	if (m_pGameServiceOption->cbDistributeMode==DISTRIBUTE_MODE_DELAYWAIT)
	{
		//获取桌子
		CTableFrameItem * pTableFrame=m_TableFrameArray[0];		

		//规则校验
		if (pTableFrame->EfficacyAddrRule(pIServerUserItem)==false) return true;
		if (pTableFrame->EfficacyTableRule(pIServerUserItem)==false) return true;
		if (pTableFrame->EfficacyScoreRule(pIServerUserItem)==false) return true;

		//插入节点
		if (m_DistributeManager.InsertDistributeNode(pIServerUserItem)==false)
		{
			return pIServerUserItem->IsRobotUser()?false:true; 
		}

		//用户起立
		if (pIServerUserItem->GetTableID()!=INVALID_TABLE)
		{
			WORD wTableID=pIServerUserItem->GetTableID();
			m_TableFrameArray[wTableID]->PerformStandUpAction(pIServerUserItem);
		}	

		//发送通知
		SendDataToUser(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_WAIT_DISTRIBUTE,NULL,0);
	}

	//立即模式
	if (m_pGameServiceOption->cbDistributeMode==DISTRIBUTE_MODE_IMMEDIATELY)
	{
		//查找桌子
		SearchTableAndSitdown(SEARCH_MODE_MAX_PLAYER,pIServerUserItem);		
	}

	return true;
}

//移除分组
bool CAttemperEngineSink::RemoveDistribute(IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//延时模式
	if (m_pGameServiceOption->cbDistributeMode==DISTRIBUTE_MODE_DELAYWAIT)
	{
		//移除节点
		m_DistributeManager.RemoveDistributeNode(pIServerUserItem);
	}

	return true;
}


//录像开始
void CAttemperEngineSink::OnEventVideoStart(IGameVideoItem * pIGameVideoItem)
{
	//参数校验
	ASSERT (pIGameVideoItem!=NULL);
	if (pIGameVideoItem==NULL) return;

	//校验桌号
	ASSERT (pIGameVideoItem->GetTableID()!=INVALID_TABLE);
	if (pIGameVideoItem->GetTableID()==INVALID_TABLE) return;

	//获取桌子
	CTableFrameItem * pTableFrame=m_TableFrameArray[pIGameVideoItem->GetTableID()];

	//构造数据包
	WORD wSendSize=0;
	BYTE cbBuffer[SOCKET_PACKET];

	//枚举玩家
	for (WORD wChairID=0; wChairID<pTableFrame->GetChairCount(); wChairID++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		//插入用户		
		pIGameVideoItem->InsertVideoUser(pIServerUserItem);
	}

	//枚举玩家
	for (WORD wChairID=0; wChairID<pTableFrame->GetChairCount(); wChairID++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;

		//构造用户包
		wSendSize=ConstructEnterPacket(pIServerUserItem,cbBuffer,sizeof(cbBuffer),CLIENT_KIND_MOBILE);
		pIGameVideoItem->InsertVideoData(INVALID_CHAIR,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer,wSendSize);
	}
}

//录像停止
void CAttemperEngineSink::OnEventVideoConclude(IGameVideoItem * pIGameVideoItem)
{
	//参数校验
	ASSERT (pIGameVideoItem!=NULL);
	if (pIGameVideoItem==NULL) return;

	//校验桌号
	ASSERT (pIGameVideoItem->GetTableID()!=INVALID_TABLE);
	if (pIGameVideoItem->GetTableID()==INVALID_TABLE) return;

	//获取桌子
	CTableFrameItem * pTableFrame=m_TableFrameArray[pIGameVideoItem->GetTableID()];

	//枚举玩家
	for (WORD wChairID=0; wChairID<pTableFrame->GetChairCount(); wChairID++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(wChairID);
		if (pIServerUserItem==NULL) continue;			

		//构造分数包
		CMD_GR_UserScore UserScore;		
		ConstructScorePacket(pIServerUserItem,&UserScore,sizeof(UserScore));
		pIGameVideoItem->InsertVideoData(INVALID_CHAIR,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));		

		//构造状态包
		CMD_GR_UserStatus UserStatus;
		ConstructStatusPacket(pIServerUserItem,&UserStatus,sizeof(UserStatus));
		pIGameVideoItem->InsertVideoData(INVALID_CHAIR,MDM_GR_USER,SUB_GR_USER_STATUS,&UserStatus,sizeof(UserStatus));
	}	
}

//存储录像
bool CAttemperEngineSink::StoreVideoData(VOID * pData,WORD wDataSize)
{
	//缓存数据
	if (m_bCacheVideo==true)
	{
		return m_VideoDataQueue.InsertData(0,pData,wDataSize);
	}

	//发送数据
	return m_pITCPSocketStore->SendData(MDM_SS_STORE_SERVICE,SUB_SS_C_STORE_VIDEO,pData,wDataSize,0);
}

//数值数组
bool CAttemperEngineSink::GetOptionValueArray(WORD wEmployScene, WORD wOptionIndex, COptionValueArray & OptionValueArray)
{
	//设置变量
	OptionValueArray.RemoveAll();

	//查找配置
	tagBattleGameOption* pBattleGameOption = NULL;
	for (INT_PTR i = 0; i < m_BattleGameOptionArray.GetCount(); i++)
	{
		if (m_BattleGameOptionArray[i].BattleGameOption.cbEmployScene == wEmployScene)
		{
			pBattleGameOption = &m_BattleGameOptionArray[i].BattleGameOption;
			break;
		}
	}

	//校验结果
	if (pBattleGameOption == NULL) return false;

	//获取对象
	auto JsonValuePtr = CWHJson::getValue(string(CT2CA(pBattleGameOption->szOptionJson)));
	if (JsonValuePtr->getType() != eJsonTypeObj) return false;

	//转换对象
	auto JsonValueObjPtr = CJsonValueObjPtr::dynamicCast(JsonValuePtr);

	//配置校验
	if (!JsonValueObjPtr->value["Count"].get()) return false;

	//获取数量
	WORD wCount = 0;
	CWHJsonInput::readJson(wCount, JsonValueObjPtr->value["Count"], false);

	//参数校验
	if (wOptionIndex >= wCount) return false;

	//定义变量
	char szType[32];
	_snprintf_s(szType, 32, "type%d", wOptionIndex + 1);

	//配置校验
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

//分数区间
INT CAttemperEngineSink::GetTableScoreAreaIndex(CTableFrameItem * pTableFrame)
{
	//参数校验
	ASSERT (pTableFrame!=NULL);
	if (pTableFrame==NULL) return 0;

	//变量定义
	INT nIndex=0;
	INT nAreaIndex[15];
	WORD wSitUserCount=pTableFrame->GetSitUserCount();

	//设置变量
	ZeroMemory(nAreaIndex,sizeof(nAreaIndex));

	//获取区间
	for (WORD i=0;i<pTableFrame->GetChairCount();i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=pTableFrame->GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//变量定义
		nIndex=m_ScoreManager.GetScoreAreaIndex(pIServerUserItem->GetUserFreeScore());
		if (wSitUserCount<=2) return nIndex;

		//设置变量
		nAreaIndex[nIndex]++;
	}

	//设置变量
	nIndex=0;
	INT nIndexCount=nAreaIndex[0];

	//对比数目	
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

//发送成绩
bool CAttemperEngineSink::SendUserScore(IServerUserItem *pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//用户信息
	auto pUserInfo=pIServerUserItem->GetUserInfo();

	//变量定义
	CMD_GR_UserScore UserScore;	
	ConstructScorePacket(pIServerUserItem,&UserScore,sizeof(UserScore));

	//发送数据
	SendBatchData(BG_COMPUTER,MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));

	//发送数据
	SendDataToTable(pIServerUserItem->GetTableID(), CLIENT_KIND_MOBILE, MDM_GR_USER,SUB_GR_USER_SCORE,&UserScore,sizeof(UserScore));

	return true;
}

//拒绝厌友
bool CAttemperEngineSink::OnTCPNetworkSubUserRepulseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_GR_UserRepulseSit));
	if (wDataSize!=sizeof(CMD_GR_UserRepulseSit)) return false;

	//消息处理
	CMD_GR_UserRepulseSit * pUserRepulseSit=(CMD_GR_UserRepulseSit *)pData;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);
	if (pIServerUserItem==NULL) return false;

	//获取桌子
	CTableFrameItem * pTableFrame=m_TableFrameArray[pUserRepulseSit->wTableID];
	//if (pTableFrame->IsGameStarted()==true) return true;

	//获取用户
	IServerUserItem * pRepulseIServerUserItem = pTableFrame->GetTableUserItem(pUserRepulseSit->wChairID);
	if (pRepulseIServerUserItem==NULL) return true;
	if(pRepulseIServerUserItem->GetUserID() != pUserRepulseSit->dwRepulseUserID)return true;

	//发送消息
	SendRoomMessage(pRepulseIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0048, TEXT("此桌有玩家设置了不与您同桌游戏！")),SMT_EJECT|SMT_CHAT|SMT_CLOSE_GAME);

	//弹起玩家
	pTableFrame->PerformStandUpAction(pRepulseIServerUserItem);

	return true;
}

//请求用户信息
bool CAttemperEngineSink::OnTCPNetworkSubUserRequestInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_GR_UserRequestInfo));
	if (wDataSize!=sizeof(CMD_GR_UserRequestInfo)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	auto pUserRequestInfo=(CMD_GR_UserRequestInfo*)pData;

	//发送信息
	SendTableUserToUser(pIServerUserItem, CLIENT_KIND_MOBILE, pUserRequestInfo->dwUserID);

	return true;
}

//请求更换位置
bool CAttemperEngineSink::OnTCPNetworkSubUserSwitchTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//用户状态
	if (pIServerUserItem->GetUserStatus()==US_PLAYING)
	{
		//失败
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0010, TEXT("您正在游戏中，暂时不能离开，请先结束当前游戏！")), REQUEST_FAILURE_NORMAL);
		return true;
	}

	//分组模式
	if (m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)
	{
		return InsertDistribute(pIServerUserItem);
	}

	//查找模式
	BYTE cbSearchMode=SEARCH_MODE_MAX_PLAYER;
	if (CServerRule::IsMatchUserByScore(m_pGameServiceOption->dwServerRule))
	{
		//更新分数
		m_ScoreManager.UpdateScoreData();

		cbSearchMode|=SEARCH_MASK_MATCH_SCORE;		
	}

	//桌子信息
	WORD wLastTableID = INVALID_WORD;

	//起立处理
	if (pIServerUserItem->GetTableID() != INVALID_TABLE && pIServerUserItem->GetChairID() != INVALID_CHAIR)
	{
		wLastTableID = pIServerUserItem->GetTableID();
		CTableFrameItem * pTableFramePrep = m_TableFrameArray[pIServerUserItem->GetTableID()];
		pTableFramePrep->PerformStandUpAction(pIServerUserItem);
	}

	//查找并坐下
	return SearchTableAndSitdown(cbSearchMode, pIServerUserItem, wLastTableID);
}

//同桌信息
bool CAttemperEngineSink::OnTCPNetworkSubUserDeskmateInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//效验参数
	ASSERT (wDataSize==sizeof(CMD_GR_UserDeskmateInfo));
	if (wDataSize!=sizeof(CMD_GR_UserDeskmateInfo)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwSocketID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	auto pUserDeskmateInfo = (CMD_GR_UserDeskmateInfo*)pData;

	//校验参数
	if (pUserDeskmateInfo->wTableID==INVALID_TABLE) return true;
	if (pUserDeskmateInfo->wTableID>=m_pGameServiceOption->wTableCount)return true;

	//发送消息
	WORD wChairCout=m_TableFrameArray[pUserDeskmateInfo->wTableID]->GetChairCount();
	for (WORD wIndex=0; wIndex<wChairCout; wIndex++)
	{
		//获取用户
		if (pUserDeskmateInfo->wChairID!=INVALID_CHAIR && wIndex!= pUserDeskmateInfo->wChairID)continue;
		IServerUserItem * pTagerIServerUserItem=m_TableFrameArray[pUserDeskmateInfo->wTableID]->GetTableUserItem(wIndex);
		if (pTagerIServerUserItem==NULL)continue;

		//变量定义
		BYTE cbBuffer[SOCKET_BUFFER]={0};
		WORD wDataSize=ConstructEnterPacket(pTagerIServerUserItem, cbBuffer, sizeof(cbBuffer), CLIENT_KIND_MOBILE);

		//发送消息
		SendDataToUser(pIServerUserItem,MDM_GR_USER,SUB_GR_USER_ENTER,cbBuffer, wDataSize);
	}

	return true;
}

//赠送礼物
bool CAttemperEngineSink::OnTCPNetworkSubUserSendGift(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
    //效验参数
    ASSERT(wDataSize == sizeof(CMD_GR_SendGift));
    if (wDataSize != sizeof(CMD_GR_SendGift)) return false;

    //获取用户
    WORD wBindIndex = LOWORD(dwSocketID);
    IServerUserItem* pIServerUserItem = GetServerUserItem(wBindIndex, dwTokenID);

    //用户效验
    ASSERT(pIServerUserItem != NULL);
    if (pIServerUserItem == NULL) return false;

    //变量定义
    auto pSendGift = (CMD_GR_SendGift*)pData;

    //目标用户
    IServerUserItem* pITargerUserItem = m_ServerUserManager.SearchUserItem(pSendGift->dwUserID);
    if (pITargerUserItem == NULL) return true;

    //变量定义
	DBR_GR_SendGift SendGift;
    ZeroMemory(&SendGift, sizeof(SendGift));
	SendGift.dwUserID = pIServerUserItem->GetUserID();
	SendGift.dwTargetUserID = pSendGift->dwUserID;
	SendGift.dwGoodsID = pSendGift->dwGoodsID;
	SendGift.dwGoodsCount = pSendGift->dwGoodsCount;
	SendGift.dwClientIP = pIServerUserItem->GetClientIP();

	//查找桌子
	auto pTableFrame = (CTableFrameItem*)m_TableFrameArray[pIServerUserItem->GetTableID()];
	auto pGameServiceOption = pTableFrame->GetGameServiceOption();
	SendGift.lMinTableScore = max(pGameServiceOption->lLessScore, pGameServiceOption->lMinEnterScore);
	SendGift.lCellScore = pGameServiceOption->lCellScore;
	SendGift.cbIsGoldRoom = (pGameServiceOption->wServerType&GAME_GENRE_GOLD)?1:0;

    //发送请求
    m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SEND_GIFT, SendGift.dwUserID, dwSocketID, dwTokenID, &SendGift, sizeof(SendGift));

    return true;
}

//写入道具
bool CAttemperEngineSink::OnDBWriteUserProp(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(LONG));
	if(wDataSize!=sizeof(LONG)) return false;

	//获取用户
	WORD wBindIndex=LOWORD(dwContextID);
	IServerUserItem * pIServerUserItem=GetServerUserItem(wBindIndex,dwTokenID);

	//用户效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//变量定义
	LONG lReturnValue=*(LONG*)pData;

	return true;
}

//赠送成功
bool CAttemperEngineSink::OnDBSendGiftSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
    //效验参数
    ASSERT(wDataSize >= sizeof(DBO_GR_SendGiftSuccess));
    if (wDataSize < sizeof(DBO_GR_SendGiftSuccess)) return false;

    //获取用户
    WORD wBindIndex = LOWORD(dwContextID);
    IServerUserItem* pIServerUserItem = GetServerUserItem(wBindIndex, dwTokenID);

	//提取数据
	DBO_GR_SendGiftSuccess* pSendGiftSuccess = (DBO_GR_SendGiftSuccess*)pData;

    //用户效验
    ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem != NULL)
	{
		//同步财富
		pIServerUserItem->WriteUserWealth(pSendGiftSuccess->wMoneyID, -pSendGiftSuccess->lGiftPrice, GOODS_REASON_SYNC);

		//赠送成功
		CMD_GR_SendGiftSuccess SendGiftSuccess;
		ZeroMemory(&SendGiftSuccess, sizeof(SendGiftSuccess));
		SendGiftSuccess.dwLoveLiness = pSendGiftSuccess->dwLoveLiness;

		//发送消息
		SendDataToUser(pIServerUserItem, MDM_GR_USER, SUB_GR_SEND_GIFT_SUCCESS, &SendGiftSuccess, sizeof(SendGiftSuccess));

		//魅力值变更
		CMD_CM_LoveLinessUpdate LoveLinessUpdate;
		ZeroMemory(&LoveLinessUpdate, sizeof(LoveLinessUpdate));
		LoveLinessUpdate.dwLoveLiness = pSendGiftSuccess->dwMyLoveLiness;

		//发送数据
		SendDataToUser(pIServerUserItem, MDM_CM_SYSTEM, SUB_CM_LOVELINESS_UPDATE, &LoveLinessUpdate, sizeof(LoveLinessUpdate));

		//通知桌子玩家
		CMD_GR_RecvGift RecvGift;
		ZeroMemory(&RecvGift, sizeof(RecvGift));
		RecvGift.dwUserID = pIServerUserItem->GetUserID();
		RecvGift.dwTargetUserID = pSendGiftSuccess->dwTargetUserID;
		RecvGift.dwGoodsID = pSendGiftSuccess->dwGoodsID;
		RecvGift.dwGoodsCount = pSendGiftSuccess->dwGoodsCount;
		RecvGift.dwLoveLiness = pSendGiftSuccess->dwLoveLiness;

		//发送数据
		SendDataToTable(pIServerUserItem->GetTableID(), 0xFF, MDM_GR_USER, SUB_GR_RECV_GIFT, &RecvGift, sizeof(RecvGift));

		//目标用户
		auto pITargerUserItem = m_ServerUserManager.SearchUserItem(pSendGiftSuccess->dwTargetUserID);
		if (pITargerUserItem == NULL) return true;

		//对方魅力值
		ZeroMemory(&LoveLinessUpdate, sizeof(LoveLinessUpdate));
		LoveLinessUpdate.dwLoveLiness = pSendGiftSuccess->dwTargetLoveLiness;

		//发送数据
		SendDataToUser(pITargerUserItem, MDM_CM_SYSTEM, SUB_CM_LOVELINESS_UPDATE, &LoveLinessUpdate, sizeof(LoveLinessUpdate));
	}

	return true;
}

//赠送失败
bool CAttemperEngineSink::OnDBSendGiftFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
    //效验参数
    ASSERT(wDataSize <= sizeof(DBO_GR_SendGiftFailure));
    if (wDataSize > sizeof(DBO_GR_SendGiftFailure)) return false;

    //提取数据
	DBO_GR_SendGiftFailure* pSendGiftFailure = (DBO_GR_SendGiftFailure*)pData;

	//发送失败
	SendOperateFailure(dwContextID, dwTokenID, SUB_GR_SEND_GIFT, pSendGiftFailure->lErrorCode, pSendGiftFailure->szDescribeString);

	return true;
}

//更新魅力
bool CAttemperEngineSink::OnDBUpdateLoveLiness(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
    //获取用户
    WORD wBindIndex = LOWORD(dwContextID);
    IServerUserItem* pIServerUserItem = GetServerUserItem(wBindIndex, dwTokenID);

    //用户效验
    ASSERT(pIServerUserItem != NULL);
    if (pIServerUserItem == NULL) return false;

    //提取数据
    auto pDBOLoveLinessUpdate = (DBO_GR_UpdateLoveLiness*)pData;

	//缓冲定义
	BYTE cbBuffer[SOCKET_PACKET] = { 0 };

    //定义变量
    auto pLoveLinessUpdate = (CMD_CM_LoveLinessUpdate*)cbBuffer;
    ZeroMemory(pLoveLinessUpdate, sizeof(CMD_CM_LoveLinessUpdate));

    //设置变量
    pLoveLinessUpdate->dwLoveLiness = pDBOLoveLinessUpdate->dwLoveLiness;

    //发送数据
	SendDataToUser(pIServerUserItem, MDM_CM_SYSTEM, SUB_CM_LOVELINESS_UPDATE, pLoveLinessUpdate, sizeof(CMD_CM_LoveLinessUpdate));

    return true;
}

//操作失败
VOID CAttemperEngineSink::SendOperateFailure(DWORD dwSocketID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe)
{
	//缓冲定义
	BYTE cbBuffer[SOCKET_PACKET] = { 0 };

    //变量定义
    CMD_CM_OperateFailure* pRequestFailure = (CMD_CM_OperateFailure*)cbBuffer;
    CSendPacketHelper SendPacket(cbBuffer + sizeof(CMD_CM_OperateFailure), sizeof(cbBuffer) - sizeof(CMD_CM_OperateFailure));

    //获取用户
    WORD wBindIndex = LOWORD(dwSocketID);
    IServerUserItem* pIServerUserItem = GetServerUserItem(wBindIndex, dwTokenID);

    //用户效验
    ASSERT(pIServerUserItem != NULL);
    if (pIServerUserItem == NULL) return;

    //设置变量
    pRequestFailure->lErrorCode = lErrorCode;
    pRequestFailure->wRequestCmdID = wRequestCmdID;
    
	//叠加信息
	if (pszDescribe != NULL)  SendPacket.AddPacket(pszDescribe, DTP_CM_DESCRIBE);

    //发送完成
    WORD wDataSize = sizeof(CMD_CM_OperateFailure) + SendPacket.GetDataSize();
	SendDataToUser(pIServerUserItem, MDM_CM_SYSTEM, SUB_CM_OPERATE_FAILURE, pRequestFailure, wDataSize);

    return;
}

//操作成功
VOID CAttemperEngineSink::SendOperateSuccess(DWORD dwSocketID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe, LPCTSTR pszGoodsList)
{
	//缓冲定义
	BYTE cbBuffer[SOCKET_PACKET] = { 0 };

    //变量定义
    CMD_CM_OperateSuccess* pRequestSuccess = (CMD_CM_OperateSuccess*)cbBuffer;
    CSendPacketHelper SendPacket(cbBuffer + sizeof(CMD_CM_OperateSuccess), sizeof(cbBuffer) - sizeof(CMD_CM_OperateSuccess));

    //获取用户
    WORD wBindIndex = LOWORD(dwSocketID);
    IServerUserItem* pIServerUserItem = GetServerUserItem(wBindIndex, dwTokenID);

    //用户效验
    ASSERT(pIServerUserItem != NULL);
    if (pIServerUserItem == NULL) return;

    //设置变量
    pRequestSuccess->lErrorCode = lErrorCode;
    pRequestSuccess->wRequestCmdID = wRequestCmdID;    

    //叠加信息
	if (pszDescribe != NULL)  SendPacket.AddPacket(pszDescribe, DTP_CM_DESCRIBE);
    if (pszGoodsList != NULL)  SendPacket.AddPacket(pszGoodsList, DTP_CM_GOODSLIST);

    //发送完成
    WORD wDataSize = sizeof(CMD_CM_OperateSuccess) + SendPacket.GetDataSize();
	SendDataToUser(pIServerUserItem, MDM_CM_SYSTEM, SUB_CM_OPERATE_SUCCESS, pRequestSuccess, wDataSize);
}

//版本信息
bool CAttemperEngineSink::OnDBGameVersionInfo(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//提取数据
	ASSERT (wDataSize%sizeof(tagModuleVersion)==0);
	if (wDataSize%sizeof(tagModuleVersion)!=0) return false;

	//变量定义
	WORD wItemCount=wDataSize/sizeof(tagModuleVersion);
	tagModuleVersion * pModuleVersion=(tagModuleVersion *)pData;

	//移除版本
	m_ModuleVersionActive.RemoveAll();

	//变量定义
	for (int i=0; i<wItemCount; i++)
	{
		m_ModuleVersionActive.Add(*pModuleVersion++);
	}

	return true;
}

//物品更新
bool CAttemperEngineSink::OnDBUserGoodsUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//校验数据
	ASSERT(wDataSize <= sizeof(DBO_GR_UserGoodsUpdate));
	if (wDataSize > sizeof(DBO_GR_UserGoodsUpdate)) return false;

	return true;
}

//修改配置
bool CAttemperEngineSink::ModifyGameServer()
{
	//房间信息
	tagCreateServerInfo CreateServerInfo;
	ZeroMemory(&CreateServerInfo,sizeof(CreateServerInfo));

	//模块索引
	CreateServerInfo.wModuleID=m_pGameServiceAttrib->wKindID;
	CreateServerInfo.wServerID=m_pGameServiceOption->wServerID;
	CreateServerInfo.wDBListID = m_pGameServiceOption->wDBListID;
	
	//挂接配置
	CreateServerInfo.wKindID=m_pGameServiceOption->wKindID;
	CreateServerInfo.wNodeID=m_pGameServiceOption->wNodeID;
	CreateServerInfo.wSortID=m_pGameServiceOption->wSortID;
	CreateServerInfo.wVisibleMask=m_pGameServiceOption->wVisibleMask;

	//积分配置
	CreateServerInfo.lRevenue=m_pGameServiceOption->lRevenue;
	CreateServerInfo.lCellScore=m_pGameServiceOption->lCellScore;
	CreateServerInfo.lLessScore=m_pGameServiceOption->lLessScore;
	CreateServerInfo.lRestrictScore=m_pGameServiceOption->lRestrictScore;
	CreateServerInfo.lMinEnterScore=m_pGameServiceOption->lMinEnterScore;
	CreateServerInfo.lMaxEnterScore=m_pGameServiceOption->lMaxEnterScore;
	CreateServerInfo.lMinEnterMember=m_pGameServiceOption->lMinEnterMember;
	CreateServerInfo.lMaxEnterMember=m_pGameServiceOption->lMaxEnterMember;

	//服务配置
	StringCchCopy(CreateServerInfo.szServiceAddr,CountArray(CreateServerInfo.szServiceAddr), m_pGameServiceOption->szServiceAddr);
	StringCchCopy(CreateServerInfo.szServiceDomain,CountArray(CreateServerInfo.szServiceDomain), m_pGameServiceOption->szServiceDomain);

	//约战规则
	CreateServerInfo.dwBattleRule=m_pGameServiceOption->dwBattleRule;

	//分组参数
	CreateServerInfo.cbDistributeMode=m_pGameServiceOption->cbDistributeMode;
	CreateServerInfo.cbDistributeRule=m_pGameServiceOption->cbDistributeRule;
	CreateServerInfo.wMinPlayerCount=m_pGameServiceOption->wMinPlayerCount;
	CreateServerInfo.wMinDistributeUser=m_pGameServiceOption->wMinDistributeUser;	
	CreateServerInfo.wMinPartakeGameUser=m_pGameServiceOption->wMinPartakeGameUser;
	CreateServerInfo.wMaxPartakeGameUser=m_pGameServiceOption->wMaxPartakeGameUser;
	CreateServerInfo.wDistributeTimeSpace=m_pGameServiceOption->wDistributeTimeSpace;

	//房间信息
	CreateServerInfo.wMaxPlayer=m_pGameServiceOption->wMaxPlayer;
	CreateServerInfo.wTableCount=m_pGameServiceOption->wTableCount;
	CreateServerInfo.wServerType=m_pGameServiceOption->wServerType;
	CreateServerInfo.wServerPort=m_pGameServiceOption->wServerPort;
	CreateServerInfo.dwServerRule=m_pGameServiceOption->dwServerRule;
	CreateServerInfo.wServerLevel=m_pGameServiceOption->wServerLevel;
	CreateServerInfo.cbMatchType=m_pGameServiceOption->cbMatchType;
	CreateServerInfo.dwAttachUserRight=m_pGameServiceOption->dwAttachUserRight;	
	StringCchCopy(CreateServerInfo.szServerName,CountArray(CreateServerInfo.szServerName), m_pGameServiceOption->szServerName);

	//扩展配置
	CWHService::GetMachineID(CreateServerInfo.szServiceMachine);
	CopyMemory(CreateServerInfo.cbCustomRule,m_pGameServiceOption->cbCustomRule,sizeof(CreateServerInfo.cbCustomRule));

	//变量定义
	tagGameServerInfo GameServerResult;
	CServerInfoManager ServerInfoManager;

	//修改房间
	if (CreateServerInfo.wServerID!=0)
	{
		//修改房间
		if (ServerInfoManager.ModifyGameServer(&CreateServerInfo,GameServerResult)==false) return true;

		//提示消息
		CTraceService::TraceString(TEXT("游戏房间配置修改成功"),TraceLevel_Normal);
	}

	return true;
}
//////////////////////////////////////////////////////////////////////////

