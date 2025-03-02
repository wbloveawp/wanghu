#include "StdAfx.h"
#include "math.h"
#include "TableFrame.h"
#include "GlobalUnits.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//常量定义
#define MAX_OFF_LINE				3									//断线次数
#define MIN_GPS_DISTANCE			100									//GPS距离

//时间定义
#define TIME_OFF_LINE				30L									//断线时间
#define TIME_KICKOUT_PLAYER			600L								//踢出时间
#define TIME_KICKOUT_LOOKON			10L									//踢出旁观

#ifdef _DEBUG
#define TIME_BATTLE_TABLE_DISMISS	 600								//解散时间
#define TIME_BATTLE_AGREEMENT_DISMISS 60								//协议解散

#define TIME_RANKING_TABLE_DISMISS	 300								//解散时间

#else
#define TIME_BATTLE_TABLE_DISMISS	  10								//解散时间
#define TIME_BATTLE_AGREEMENT_DISMISS 60*2								//协议解散

#define TIME_RANKING_TABLE_DISMISS	 300								//解散时间
#endif

//时间定义
#define ALLOW_LOOKON_TIME			300L								//允许旁观时间

//////////////////////////////////////////////////////////////////////////

//定义结构
struct tagUserRateItem
{
	WORD wChairID;
	WORD wAddupWinRate;
	BYTE cbControlStatus;
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameItem::CTableFrameItem()
{
	//固有属性
	m_wTableID=0;
	m_wChairCount=0;
	m_GameStartMode=StartMode_FullReady;

	//辅助变量
	m_dwLastEnterTime=0L;
	m_dwLastLeaveTime=0L;

	//配置变量
	m_wMaxPlayerCount=0;
	m_wSitdownMultiple = 0;
	m_wStandupMultiple = 0;

	//标志变量
	m_bVideoData=false;
	m_bWriteScore=false;
	m_bGamePaused=false;
	m_bGameStarted=false;
	m_bTableStarted=false;
	ZeroMemory(m_bAllowLookon,sizeof(m_bAllowLookon));

	//功能标识
	m_bSameIpRemind=false;

	//游戏变量
	m_lBaseScore=0;
	m_dwPlayCount=0;
	m_dwGameStartTime=0L;
	m_cbGameStatus=GAME_STATUS_FREE;
	ZeroMemory(m_dwPlayerID,sizeof(m_dwPlayerID));

	//桌子信息
	m_dwTableOwnerID=0L;	
	m_szTablePassword[0]=0;
	m_lMinEnterScore = 0;//kk jia 
	
	//平衡控制
	m_bControlCard=false;
	ZeroMemory(m_cbUserCardLevel,sizeof(m_cbUserCardLevel));

	//积分记录
	m_dwPlayTimeCount=0L;
	ZeroMemory(m_ScoreInfoArray,sizeof(m_ScoreInfoArray));

	//服务费记录
	memset(m_bCostGame,true,sizeof(m_bCostGame));

	//断线变量
	ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
	ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));
	ZeroMemory(m_OfflineScore,sizeof(m_OfflineScore));

	//用户数组
	ZeroMemory(m_TableUserItemArray,sizeof(m_TableUserItemArray));

	//踢人变量
	ZeroMemory(m_dwUserUnReadyTime,sizeof(m_dwUserUnReadyTime));

	//内核组件
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pIAssistDataBaseEngine=NULL;

	//组件接口
	m_pIGameStockQuery=NULL;
	m_pITableFrameSink=NULL;
	m_pITableUserAction=NULL;
	m_pIEventUserScore=NULL;
	m_pIServiceFunction=NULL;		
	m_pIEventServiceCost=NULL;
	m_pITableEventSink=NULL;
	m_pITableUserRequest=NULL;
	m_pIEventBaseScore=NULL;
	m_pIEventStartEfficacy=NULL;	
	m_pIRobotUserManager=NULL;		
	m_pIEventCustomFrameTime=NULL;
	m_pITableControlSendCard = NULL;

	//录像接口
	m_pIGameVideoItem=NULL;
	m_pIGameVideoManager=NULL;

	//配置信息	
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//析构函数
CTableFrameItem::~CTableFrameItem()
{
	//释放接口
	SafeRelease(m_pITableFrameSink);	

	return;
}

//接口查询
VOID * CTableFrameItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameItem,Guid,dwQueryVer);		
	QUERYINTERFACE(IGameBalanceControl, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameItem,Guid,dwQueryVer);
	return NULL;
}

//最低进入 KK JIA
VOID CTableFrameItem::SetMinEnterScore(LONGLONG lMinEnterScore)
{
	//设置底分
	m_lMinEnterScore = lMinEnterScore;

}

//单元积分
VOID CTableFrameItem::SetCellScore(LONG lCellScore)
{
	//设置底分
	m_lBaseScore=lCellScore;

	//变更通知
	if (m_pIEventBaseScore!=NULL)
	{
		m_pIEventBaseScore->OnEventBaseScoreVariation(m_lBaseScore);
	}
}

//空位置数目
WORD CTableFrameItem::GetNullChairCount()
{
	//变量定义
	WORD wNullChairCount=0;

	//设置用户
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) 
		{
			wNullChairCount++;
		}
	}

	return wNullChairCount;
}

//开始游戏
bool CTableFrameItem::StartGame()
{
	//效验状态
	ASSERT(m_bGameStarted==false);
	if (m_bGameStarted==true) return false;

	//设置变量
	m_dwPlayCount++;
	m_bGameStarted=true;
	m_bGamePaused=false;
	m_dwGameStartTime=(DWORD)time(NULL);
	ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));

	//设置变量
	ZeroMemory(m_dwUserUnReadyTime,sizeof(m_dwUserUnReadyTime));

	//关闭定时器
	KillGameTimer(IDI_KICKOUT_PLAYER);	

	//分数信息
	ZeroMemory(m_dwPlayerID,sizeof(m_dwPlayerID));
	ZeroMemory(m_ScoreInfoArray,sizeof(m_ScoreInfoArray));

	//设置用户
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//设置信息
		m_dwPlayerID[i]=pIServerUserItem->GetUserID();

		//开始校验
		if (m_pIEventStartEfficacy!=NULL)
		{
			if (m_pIEventStartEfficacy->IsAllowPlayerReady(i)==false)
			{
				continue;
			}
		}

		//改变状态
		pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,i);	
	}

	//设置状态
	if (m_bTableStarted==false)
	{
		//设置变量
		m_bTableStarted=true;
		
		//发送状态
		SendTableStatus();
	}

	//输赢控制
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0 && m_wChairCount!=MAX_CHAIR)
	{
		CalcUserCardLevel();
	}

	//服务费模式
	if (CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule))
	{
		//询问玩家
		for( WORD i = 0; i < m_wChairCount; i++ )
		{
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			m_bCostGame[i] = true;
			if( pIServerUserItem == NULL ) continue;

			if ( m_pIEventServiceCost )
				m_bCostGame[i] = !m_pIEventServiceCost->OnEventQueryCostable(pIServerUserItem,false);
			else if( m_pGameServiceOption->lRevenue != 0 )
				m_bCostGame[i] = false;
		}
	}

	//录像接口
	if (m_pIGameVideoManager!=NULL && CServerRule::IsRecordGameVideo(m_pGameServiceOption->dwServerRule))
	{
		//开始录像
		m_pIGameVideoItem=m_pIGameVideoManager->ActiveVideoItem(m_wTableID);
		if (m_pIGameVideoItem!=NULL) m_pIGameVideoItem->StartVideo();

		//设置标志
		m_bVideoData=true;

		for (WORD i=0;i<m_wChairCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;

			//发送场景			
			m_pITableFrameSink->OnEventSendGameScene(i,pIServerUserItem,m_cbGameStatus,true);
		}

		//还原标志
		m_bVideoData=false;
	}

	//通知事件
	ASSERT(m_pITableFrameSink != NULL);
	if (m_pITableFrameSink == NULL)
	{
		return true;
	}

	//游戏开始
	if (m_pITableFrameSink->OnEventGameStart() == false)
	{
		return true;
	}

	//通知事件
	m_pIServiceFunction->OnEventGameStart(this);
	
	//游戏开始
	OnEventGameStart();	

	return true;
}

//暂停游戏
bool CTableFrameItem::PauseGame()
{
	//状态判断
	ASSERT(m_bGamePaused == false);
	if (m_bGamePaused == true) return false;

	//暂停游戏
	//if ((m_bGameStarted == true) && (m_pITableFrameSink->OnEventGamePause() == false))
	if (m_pITableFrameSink->OnEventGamePause() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置状态
	m_bGamePaused = true;

	//更新状态
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;

		//发送状态
		CMD_GF_GameStatus GameStatus;
		GameStatus.bGamePaused = m_bGamePaused;
		GameStatus.cbGameStatus = m_cbGameStatus;
		GameStatus.cbAllowLookon = m_bAllowLookon[i] ? TRUE : FALSE;
		m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_FRAME, SUB_GF_GAME_STATUS, &GameStatus, sizeof(GameStatus));
	}	

	return true;
}

//继续游戏
bool CTableFrameItem::ContinueGame()
{
	//状态判断
	ASSERT (m_bGamePaused == true);
	if (m_bGamePaused == false) return false;

	//继续游戏
	if (m_pITableFrameSink->OnEventGameContinue() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置状态
	m_bGamePaused = false;

	//更新状态
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;

		//发送状态
		CMD_GF_GameStatus GameStatus;
		GameStatus.bGamePaused = m_bGamePaused;
		GameStatus.cbGameStatus = m_cbGameStatus;
		GameStatus.cbAllowLookon = m_bAllowLookon[i] ? TRUE : FALSE;
		m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_FRAME, SUB_GF_GAME_STATUS, &GameStatus, sizeof(GameStatus));
	}

	return true;
}

//解散游戏
bool CTableFrameItem::DismissGame()
{
	//状态判断
	ASSERT(m_bTableStarted==true);
	if (m_bTableStarted==false) return false;

	//设置状态
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//设置变量
		m_bTableStarted=false;

		//发送状态
		SendTableStatus();
	}

	//结束游戏
	if ((m_bGameStarted==true)&&(m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//记录分数
bool CTableFrameItem::RecordGameScore(LPCTSTR pszGameCourse)
{
	//构造结构
	DBR_GR_RecordDrawInfo RecordDrawInfo;
	ZeroMemory(&RecordDrawInfo, sizeof(RecordDrawInfo));

	//构造结构
	DBR_GR_RecordDrawScore RecordDrawScore;
	ZeroMemory(&RecordDrawScore, sizeof(RecordDrawScore));

	//设置变量
	RecordDrawInfo.wTableID = m_wTableID;
	RecordDrawInfo.lRecordID = BuildGameRecordID();
	RecordDrawInfo.dwPlayTimeCount = m_dwPlayTimeCount;
	RecordDrawInfo.cbScoreKind = SCORE_KIND_GOLD;

	//约战信息
	RecordDrawInfo.lRelationRecordID = GetRelationParameter()->lRelationRecordID;

	//获取时间
	GetLocalTime(&RecordDrawInfo.RecordTime);

	//设置变量
	RecordDrawScore.lRecordID = RecordDrawInfo.lRecordID;
	RecordDrawScore.lRelationRecordID = RecordDrawInfo.lRelationRecordID;

	//录像编号	
	if (m_pIGameVideoItem != NULL)
	{
		StringCchCopy(RecordDrawInfo.szVideoNumber, CountArray(RecordDrawInfo.szVideoNumber), m_pIGameVideoItem->GetVideoNumber());
		StringCchCopy(RecordDrawScore.szVideoNumber, CountArray(RecordDrawScore.szVideoNumber), m_pIGameVideoItem->GetVideoNumber());
	}

	//变量定义
	SCORE lScore = 0;
	SCORE lRevenue = 0;
	tagScoreRecordItem* pScoreRecordItem = NULL;

	//用户积分
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//校验用户
		if (m_dwPlayerID[i] == 0) continue;

		//分数过滤
		if (m_ScoreInfoArray[i].lScore == 0 &&
			m_ScoreInfoArray[i].lBetAmount == 0 && 
			m_ScoreInfoArray[i].ScoreType == ScoreType_Null && 
			(m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) == 0)
		{
			continue;
		}

		//用户数量
		RecordDrawInfo.wUserCount++;

		//获取子项
		pScoreRecordItem = &RecordDrawScore.ScoreRecordItem[RecordDrawScore.wUserCount++];

		//用户信息
		pScoreRecordItem->wChairID = i;
		pScoreRecordItem->dwUserID = m_dwPlayerID[i];

		//分数信息
		lRevenue += m_ScoreInfoArray[i].lRevenue;
		pScoreRecordItem->lScore = m_ScoreInfoArray[i].lScore;
		pScoreRecordItem->lRevenue = m_ScoreInfoArray[i].lRevenue;
		pScoreRecordItem->lBetAmount = m_ScoreInfoArray[i].lBetAmount;
		pScoreRecordItem->lBeforScore = m_ScoreInfoArray[i].lBeforScore;
		
		//查询用户
		IServerUserItem* pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem != NULL)
		{
			if (!pIServerUserItem->IsRobotUser())
			{
				RecordDrawInfo.lWasteCount -= pScoreRecordItem->lScore;
			}

			//分数类型
			RecordDrawInfo.cbScoreKind = pIServerUserItem->GetScoreKind();
		}
		else
		{
			RecordDrawInfo.lWasteCount -= pScoreRecordItem->lScore;
		}

		lScore += (long)abs((long)pScoreRecordItem->lScore);			

		//发送数据
		if (RecordDrawScore.wUserCount >= CountArray(RecordDrawScore.ScoreRecordItem))
		{
			//投递数据
			m_pIAssistDataBaseEngine->PostDataBaseRequest(DBR_GR_RECORD_DRAW_SCORE, 0, 0, 0, &RecordDrawScore, sizeof(RecordDrawScore));

			//重置人数
			RecordDrawScore.wUserCount = 0;
		}
	}

	//发送数据
	if (RecordDrawScore.wUserCount > 0)
	{
		//投递数据
		WORD wHeadSize = sizeof(RecordDrawScore) - sizeof(RecordDrawScore.ScoreRecordItem);
		WORD wDataSize = RecordDrawScore.wUserCount * sizeof(RecordDrawScore.ScoreRecordItem[0]);
		m_pIAssistDataBaseEngine->PostDataBaseRequest(DBR_GR_RECORD_DRAW_SCORE, 0, 0, 0, &RecordDrawScore, wHeadSize + wDataSize);
	}

	//校验人数
	if (RecordDrawInfo.wUserCount > 0)
	{
		//设置变量
		RecordDrawInfo.lWasteCount -= lRevenue;
		RecordDrawInfo.lRevenueCount = lRevenue;

		//损耗玩家
		DWORD dwWasteUserID = (DWORD)-1;
		if (RecordDrawInfo.lWasteCount > 0)
		{
			SCORE lScore = m_ScoreInfoArray[0].lScore;
			dwWasteUserID = m_dwPlayerID[0];
			for (WORD i = 0; i < m_wChairCount; i++)
			{
				if (m_ScoreInfoArray[i].lScore < lScore)
				{
					dwWasteUserID = m_dwPlayerID[i];
					lScore = m_ScoreInfoArray[i].lScore;
				}
			}
		}

		RecordDrawInfo.dwWasteUserID = dwWasteUserID;

		//游戏过程
		if (pszGameCourse != NULL && lstrlen(pszGameCourse) > 0)
		{
			StringCchCopy(RecordDrawInfo.szGameSourse, CountArray(RecordDrawInfo.szGameSourse), pszGameCourse);
		}

		//投递数据
		WORD wDataSize = CountStringBuffer(RecordDrawInfo.szGameSourse);
		WORD wHeadSize = sizeof(RecordDrawInfo) - sizeof(RecordDrawInfo.szGameSourse);
		m_pIAssistDataBaseEngine->PostDataBaseRequest(DBR_GR_RECORD_DRAW_INFO, 0, 0, 0, &RecordDrawInfo, wHeadSize + wDataSize);
	}

	return true;
}

//记录录像
bool CTableFrameItem::RecordGameVideo()
{
	//构造结构
	DBR_GR_RecordGameVideo RecordGameVideo;
	ZeroMemory(&RecordGameVideo, sizeof(RecordGameVideo));

	for (WORD i = 0; i < m_wChairCount; i++)
	{
		if (m_dwPlayerID[i] == 0) continue;
		if (RecordGameVideo.wUserCount >= CountArray(RecordGameVideo.dwUserID)) break;

		//设置变量
		RecordGameVideo.dwUserID[RecordGameVideo.wUserCount++] = m_dwPlayerID[i];
	}

	//约战信息
	RecordGameVideo.dwRelationNum = GetRelationParameter()->dwRelationNum;
	RecordGameVideo.lRelationRecordID = GetRelationParameter()->lRelationRecordID;

	//录像信息
	GetLocalTime(&RecordGameVideo.VideoCreateTime);
	StringCchCopy(RecordGameVideo.szVideoNumber, CountArray(RecordGameVideo.szVideoNumber), m_pIGameVideoItem->GetVideoNumber());

	//投递数据
	ASSERT(m_pIAssistDataBaseEngine != NULL);
	m_pIAssistDataBaseEngine->PostDataBaseRequest(DBR_GR_RECORD_GAME_VIDEO, 0, 0, 0, &RecordGameVideo, sizeof(RecordGameVideo));

	//构造结构
	CMD_SS_C_StoreVideo* pStoreVideo = (CMD_SS_C_StoreVideo*)m_cbBuffer;
	ZeroMemory(pStoreVideo, sizeof(CMD_SS_C_StoreVideo));

	//设置变量
	pStoreVideo->wKindID = m_pGameServiceOption->wKindID;
	pStoreVideo->wVideoSize = m_pIGameVideoItem->GetCompressSize();
	StringCchCopy(pStoreVideo->szVideoNumber, CountArray(pStoreVideo->szVideoNumber), m_pIGameVideoItem->GetVideoNumber());

	//获取时间
	pStoreVideo->VideoCreateTime = RecordGameVideo.VideoCreateTime;

	//拷贝数据
	CopyMemory(pStoreVideo + 1, m_pIGameVideoItem->GetCompressBuffer(), pStoreVideo->wVideoSize);

	//存储数据
	m_pIServiceFunction->StoreVideoData(m_cbBuffer, sizeof(CMD_SS_C_StoreVideo) + pStoreVideo->wVideoSize);

	return true;
}

//结束游戏
bool CTableFrameItem::ConcludeGame(LPCTSTR pszGameCourse)
{
	//效验状态
	ASSERT (m_bGameStarted==true);
	if (m_bGameStarted==false) return false;

	//游戏服务费
 	if( CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule) )
		CostForGameStart();

	//结束游戏
	m_bGamePaused=false;
	m_bGameStarted=false;	
	m_cbGameStatus=GAME_STATUS_FREE;
	m_dwPlayTimeCount=(DWORD)time(NULL)-m_dwGameStartTime;

	//停止录像
	if (m_pIGameVideoItem!=NULL) m_pIGameVideoItem->ConcludeVideo();

	//游戏记录
	if (CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
	{
		RecordGameScore(pszGameCourse);
	}

	//录像记录	
	if (m_pIGameVideoItem!=NULL) 
	{
		RecordGameVideo();			
	}	

	//设置用户
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_TableUserItemArray[i]!=NULL)
		{
			IServerUserItem * pIServerUserItem=m_TableUserItemArray[i];
			if (pIServerUserItem->GetUserStatus()!=US_OFFLINE) pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);
		}
	}

	//变量定义
	bool bOffLineWait=false;

	//断线处理
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//断线处理
		if (pIServerUserItem->GetUserStatus()==US_OFFLINE)
		{
			bOffLineWait=true;
		
			if ((m_pGameServiceOption->wServerType&(GAME_GENRE_MATCH|GAME_GENRE_BATTLE|GAME_GENRE_RANKING))!=0)
			{						
				pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);
			}
			else
			{
				PerformStandUpAction(pIServerUserItem);
			}

			continue;
		}

		//托管用户
		if (pIServerUserItem->IsTrusteeUser()==true && (m_pGameServiceOption->wServerType&(GAME_GENRE_MATCH|GAME_GENRE_BATTLE|GAME_GENRE_RANKING))==0)
		{
			PerformStandUpAction(pIServerUserItem);

			continue;
		}
	}

	//删除时间	
	if (bOffLineWait==true) KillGameTimer(IDI_OFF_LINE);

	//设置状态
	if (m_bTableStarted==true&&CServerRule::IsTableEndImmediate(m_pGameServiceOption->dwServerRule))
	{
		//设置变量
		m_bTableStarted=false;

		//发送状态
		if (m_pITableEventSink)
			m_pITableEventSink->OnEventTableStatus(QUERY_ME_INTERFACE(ITableFrameItem),FALSE);

		//发送状态
		SendTableStatus();
	}

	//断线变量
	ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
	ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));

	//游戏变量
	m_bWriteScore=false;
	m_dwGameStartTime=0L;
	ZeroMemory(m_dwPlayerID,sizeof(m_dwPlayerID));

	//积分记录
	m_dwPlayTimeCount=0L;
	ZeroMemory(m_ScoreInfoArray,sizeof(m_ScoreInfoArray));

	//通知比赛
	//if (m_pIMatchEventSink!=NULL) m_pIMatchEventSink->OnEventGameConclude(this, 0, NULL, m_cbGameStatus);

	//重置桌子
	ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink!=NULL) m_pITableFrameSink->ResetTableFrameSink();

	//通知事件
	m_pIServiceFunction->OnEventGameConclude(this);

	//结束通知
	OnEventGameConclude();

	//清理离线锁分
	for( INT_PTR i = 0; i < CountArray(m_OfflineScore); i++ )
	{
		if( m_OfflineScore[i].dwUserID == 0 ) continue;

		if( m_pITableEventSink && (m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE)))
			m_pITableEventSink->OnTableReqUnfreezeScore(m_OfflineScore[i].dwUserID,m_OfflineScore[i].lUnLockScore);

		m_OfflineScore[i].dwUserID = 0;
	}

	//服务费用
	SCORE lServiceFee = 0;
	if (m_pGameServiceOption->lLessScore != 0 && CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule))
	{
		lServiceFee = m_pGameServiceOption->lRevenue;
	}

	//计算积分
	SCORE lLessScore = m_pGameServiceOption->lLessScore;
	SCORE lStandupScore = __max(m_lBaseScore * m_wStandupMultiple, lLessScore) + lServiceFee;

	//起立统计
	WORD wStandupCount = 0;
	bool bAllowStandUp = true;	

	//起立处理
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//获取用户 
		if (m_TableUserItemArray[i] == NULL) continue;
		IServerUserItem * pIServerUserItem = m_TableUserItemArray[i];

		//变量定义
		bool bStandUp = false;
		LPCTSTR pszMessage = L"";

		//积分限制
		if (pIServerUserItem->GetUserScore() < lStandupScore)
		{
			//金币类型
			if (pIServerUserItem->GetScoreKind() == SCORE_KIND_GOLD && pIServerUserItem->IsLargessSubsidy() == false)
			{
				//设置标识
				bStandUp = true;

				//构造消息			
				pszMessage = CWHService::FormatMessageV(MIM_SERVER_0058, L"您的游戏币少于 " SCORE_STRING " 个，无法继续游戏！", SCORE_ZOOMOUT(lStandupScore));

				//发送消息
				SendGameMessage(pIServerUserItem, pszMessage, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT | SMT_SHOW_MOBILE);				
			}

			//茶社积分类型
			if (pIServerUserItem->GetScoreKind() == SCORE_KIND_CLUB)
			{
				//设置标识
				bStandUp = true;

				//构造消息			
				pszMessage = CWHService::FormatMessageV(MIM_SERVER_0057, L"您的积分少于 " SCORE_STRING " 个，无法继续游戏！", SCORE_ZOOMOUT(lStandupScore));

				//发送消息
				SendGameMessage(pIServerUserItem, pszMessage, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT | SMT_SHOW_MOBILE);
			}

			//执行起立
			if (bStandUp == true)
			{
				//起立处理
				PerformStandUpAction(pIServerUserItem);

				//机器玩家
				if (pIServerUserItem->IsRobotUser() == true)
				{
					m_pIRobotUserManager->PostponeRobotAction(pIServerUserItem->GetUserID(), ROBOT_ACTION_INSURE);
				}

				continue;
			}
		}		

		//锁定限制
		if ((m_pGameServiceOption->lLessLockScore > 0L) && (pIServerUserItem->GetUserFreeScore() < (lStandupScore-lServiceFee)) && m_wChairCount != MAX_CHAIR)
		{
			//金币类型
			if (pIServerUserItem->GetScoreKind() == SCORE_KIND_GOLD && pIServerUserItem->IsLargessSubsidy() == false)
			{
				//设置标识
				bStandUp = true;

				//构造消息			
				pszMessage = CWHService::FormatMessageV(MIM_SERVER_0058, L"您的游戏币少于 " SCORE_STRING " 个，无法继续游戏！", SCORE_ZOOMOUT(lStandupScore));

				//发送消息
				SendGameMessage(pIServerUserItem, pszMessage, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT | SMT_SHOW_MOBILE);				
			}

			//茶社积分类型
			if (pIServerUserItem->GetScoreKind() == SCORE_KIND_CLUB)
			{
				//设置标识
				bStandUp = true;

				//构造消息			
				pszMessage = CWHService::FormatMessageV(MIM_SERVER_0057, L"您的积分少于 " SCORE_STRING " 个，无法继续游戏！", SCORE_ZOOMOUT(lStandupScore));

				//发送消息
				SendGameMessage(pIServerUserItem, pszMessage, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT | SMT_SHOW_MOBILE);
			}

			//执行起立
			if (bStandUp == true)
			{
				//执行起立
				PerformStandUpAction(pIServerUserItem);

				//机器玩家
				if (pIServerUserItem->IsRobotUser() == true)
				{
					m_pIRobotUserManager->PostponeRobotAction(pIServerUserItem->GetUserID(), ROBOT_ACTION_INSURE);
				}				

				continue;
			}			
		}

		//关闭判断
		if (((g_GlobalUnits.IsAllowEnterGame() == false) || (g_GlobalUnits.IsAllowEnterRoom() == false)) && (pIServerUserItem->GetMasterOrder() == 0))
		{
			//发送消息
			SendGameMessage(pIServerUserItem, CWHService::FormatMessageV(MIM_SERVER_0060, L"请注意，游戏房间即将关闭或者不允许玩家进入，请您离开游戏桌子！"), SMT_EJECT | SMT_CHAT | SMT_CLOSE_GAME);

			//用户起立
			PerformStandUpAction(pIServerUserItem);

			continue;
		}

		//分数判断
		if (m_pGameServiceOption->lMaxEnterScore != 0L && pIServerUserItem->GetUserScore() > m_pGameServiceOption->lMaxEnterScore)
		{
			//金币类型
			if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0)
			{
				pszMessage = CWHService::FormatMessageV(MIM_SERVER_0062, L"您好，您的游戏币已经高于 " SCORE_STRING "，请您到别的房间游戏！", SCORE_ZOOMOUT(m_pGameServiceOption->lMaxEnterScore));
				SendGameMessage(pIServerUserItem, pszMessage, SMT_EJECT | SMT_CHAT | SMT_CLOSE_GAME);
			}			

			//起立判断
			PerformStandUpAction(pIServerUserItem);

			continue;
		}

		//机器起立
		if (pIServerUserItem->GetUserStatus() != US_OFFLINE && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH) == 0)
		{
			//机器处理
			if (pIServerUserItem->IsRobotUser() == true)
			{
				//查找机器
				CRobotUserItem * pIRobotUserItem = (CRobotUserItem *)m_pIRobotUserManager->SearchRobotUserItem(pIServerUserItem->GetUserID());
				if (pIRobotUserItem == NULL) continue;

				//离开判断
				if (pIRobotUserItem->JudgeRobotAction(ROBOT_WAITLEAVE))
				{
					//起立处理
					PerformStandUpAction(pIServerUserItem);

					//机器动作
					m_pIRobotUserManager->PostponeRobotAction(pIServerUserItem->GetUserID(), ROBOT_ACTION_LEAVE);

					continue;
				}

				//起立判断
				if (pIRobotUserItem->JudgeRobotAction(ROBOT_WAITSTANDUP))
				{
					//机器动作
					m_pIRobotUserManager->PostponeRobotAction(pIServerUserItem->GetUserID(), ROBOT_ACTION_STANDUP);

					continue;
				}
			}
		}
	}

	//非比赛房间
	if (((m_pGameServiceOption->wServerType&(GAME_GENRE_MATCH|GAME_GENRE_BATTLE))==0) && (m_GameStartMode==StartMode_AllReady || m_GameStartMode==StartMode_FullReady))
	{
		//变量定义
		WORD wDelayTime=0;
		WORD wFirstChairID=INVALID_CHAIR;
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if (GetTableUserItem(i)==NULL) continue;
			m_dwUserUnReadyTime[i]=(DWORD)time(NULL)+wDelayTime++;
			if (wFirstChairID==INVALID_CHAIR)
			{
				wFirstChairID=i;				
			}
		}

		//设置定时器
		if (wFirstChairID!=INVALID_CHAIR)
		{
			//获取时间
			DWORD dwElapse=TIME_KICKOUT_PLAYER;
			if (m_pIEventCustomFrameTime!=NULL)
			{
				dwElapse=m_pIEventCustomFrameTime->GetUnReadyKickOutTime();
			}

			//设置时钟
			SetGameTimer(IDI_KICKOUT_PLAYER,dwElapse*1000L,1,wFirstChairID);
		}
	}	

	//删除录像
	if (m_pIGameVideoManager!=NULL)
	{
		m_pIGameVideoManager->DeleteVideoItem(m_pIGameVideoItem);
		m_pIGameVideoItem=NULL;
	}	

	return true;
}

//计算税收
SCORE CTableFrameItem::AccountRevenue(SCORE lGameScore)
{
	if (lGameScore <= 0L) return 0L;

	//金币税收
	if ((CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule)==false) &&
		(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0)
	{
		return m_pGameServiceOption->lRevenue*lGameScore/1000L;
	}	

	return 0L;
}

//写入积分
bool CTableFrameItem::WriteUserScore(WORD wChairID, tagScoreInfo& ScoreInfo)
{
	//效验参数
	ASSERT((wChairID< m_wMaxPlayerCount)/*&&(lScore<=0L)*/);
	if ((wChairID>= m_wMaxPlayerCount)/*||(lScore>0L)*/) return false;

	//获取用户
	IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);	

	//写入积分
	if (pIServerUserItem!=NULL)
	{
		//校验类型
		if (((m_pGameServiceOption->wServerType & (GAME_GENRE_GOLD| GAME_GENRE_MATCH |GAME_GENRE_BATTLE)) != 0) && ScoreInfo.lScore<0)
		{
			//输分限制
			if (-ScoreInfo.lScore>pIServerUserItem->GetUserFreeScore())
			{
				ASSERT(FALSE);
				return false;
			}
		}
		CString s;
		s.Format(_T("shui1: i:%d, m_ScoreInfoArray[wChairID].lScore:%I64d , lRevenue:%I64d "), wChairID, m_ScoreInfoArray[wChairID].lScore, ScoreInfo.lRevenue);
		CTraceService::TraceString(s, TraceLevel_Debug);
		//积分记录
		m_ScoreInfoArray[wChairID].lScore+= ScoreInfo.lScore;
		m_ScoreInfoArray[wChairID].lRevenue += ScoreInfo.lRevenue;
		m_ScoreInfoArray[wChairID].ScoreType = ScoreInfo.ScoreType;	
		m_ScoreInfoArray[wChairID].lBetAmount += ScoreInfo.lBetAmount;		

		//用户分数
		if (!CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule))
		{
			m_ScoreInfoArray[wChairID].lBeforScore = pIServerUserItem->GetUserScore();
		}

		//用户积分
		pIServerUserItem->WriteUserScore(ScoreInfo.lScore, ScoreInfo.lRevenue, ScoreInfo.lBetAmount, ScoreInfo.lExperience, ScoreInfo.ScoreType);

		//用户状态
		if (pIServerUserItem->GetUserStatus()!=US_OFFLINE)
		{
			BYTE cbUserStatus=US_SIT;
			pIServerUserItem->SetUserStatus(cbUserStatus,m_wTableID,wChairID);
		}		
	}
	else
	{
		ASSERT(FALSE);	
		return false;
	}

	return true;
}

//写入积分
bool CTableFrameItem::WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount)
{
	//CTraceService::TraceString(_T("WriteTableScore_3"), TraceLevel_Exception);
	//效验参数
	ASSERT (wScoreCount== m_wMaxPlayerCount);
	if (wScoreCount!= m_wMaxPlayerCount) return false;

	//CTraceService::TraceString(_T("WriteTableScore_4"), TraceLevel_Exception);
	//效验积分
	if (m_wMaxPlayerCount != MAX_CHAIR)
	{
		//变量定义
		SCORE lGameScore=0;

		//积分统计
		for (WORD i=0;i< m_wMaxPlayerCount;i++)
		{
			if (ScoreInfoArray[i].ScoreType!=ScoreType_Null)
			{
				//用户效验
				if (m_dwPlayerID[i]==0L)
				{
					ASSERT(FALSE);
					return false;
				}

				//积分累计
				lGameScore+=ScoreInfoArray[i].lScore;
				lGameScore+=m_ScoreInfoArray[i].lScore;
			}
			else
			{
				//积分累计
				ASSERT(m_ScoreInfoArray[i].lScore<=0L);
				lGameScore+=m_ScoreInfoArray[i].lScore;
			}

			//获取用户
			IServerUserItem * pIServerUserItem = GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;

			//校验分数
			if (((m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD)) != 0) && ScoreInfoArray[i].lScore<0)
			{
				if ((-ScoreInfoArray[i].lScore) > pIServerUserItem->GetUserFreeScore())
				{
					ASSERT(FALSE);
					return false;
				}
			}
		}

		if (m_wMaxPlayerCount >1 && (m_pGameServiceOption->wServerType & (GAME_GENRE_MATCH|GAME_GENRE_BATTLE|GAME_GENRE_RANKING)) == 0)
		{
			//总和效验
			if (lGameScore > 0L)
			{
				ASSERT(FALSE);
				return false;
			}
		}
	}

	//CTraceService::TraceString(_T("WriteTableScore_5"), TraceLevel_Exception);

	//写入分数
	for (WORD i=0;i< m_wMaxPlayerCount;i++)
	{
		//写分判断
		if (m_dwPlayerID[i]==0L) continue;
		if (ScoreInfoArray[i].ScoreType==ScoreType_Null && (m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE)==0) continue;

		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		
		//写入积分
		if (pIServerUserItem!=NULL)
		{
			//用户积分
			SCORE lScore = ScoreInfoArray[i].lScore;
			SCORE lRevenue = ScoreInfoArray[i].lRevenue;
			LONG lBetAmount = ScoreInfoArray[i].lBetAmount;
			LONG lExperience = ScoreInfoArray[i].lExperience;
			enScoreType ScoreType = ScoreInfoArray[i].ScoreType;

			////计算税收
			//SCORE lRevenue=AccountRevenue(lScore);
			//SCORE lUserScore = ScoreInfoArray[i].lScore-lRevenue;

			//积分记录
			/*CString s;
			s.Format(_T("shui2: i:%d, m_ScoreInfoArray[i].lScore:%I64d ,lScore:%I64d, lRevenue:%I64d "), i, m_ScoreInfoArray[i].lScore, lScore,lRevenue);
			CTraceService::TraceString(s, TraceLevel_Debug);*/

			m_ScoreInfoArray[i].lScore += lScore;
			m_ScoreInfoArray[i].lRevenue += lRevenue;						
			m_ScoreInfoArray[i].ScoreType = ScoreType;						
			m_ScoreInfoArray[i].lBetAmount += lBetAmount;

			

			//用户分数
			if (!CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule))
			{
				m_ScoreInfoArray[i].lBeforScore = pIServerUserItem->GetUserScore();
			}

			/*CString s;
			s.Format(_T("WriteTableScore_6,lScore:%I64d, lRevenue:%I64d, lBetAmount:%d, lExperience:%d, ScoreType:%d"), lScore, lRevenue, lBetAmount, lExperience, ScoreType);
			CTraceService::TraceString(s, TraceLevel_Exception);*/
			//用户写分
			//pIServerUserItem->WriteUserScore(lScore, lRevenue, lBetAmount, lExperience, ScoreType);//kk guan
			pIServerUserItem->WriteUserScore(lScore - lRevenue, lRevenue, lBetAmount, lExperience, ScoreType);//kk jia
		}
		else
		{
			//提示信息
			CTraceService::TraceString(TEXT("执行了离线写分操作"),TraceLevel_Debug);

			//离线写分
			ASSERT (m_OfflineScore[i].dwUserID!=0);
			if (m_OfflineScore[i].dwUserID==0) continue;
			if (m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
			{
				ASSERT (m_OfflineScore[i].lUnLockScore>=ScoreInfoArray[i].lScore);
				if (m_OfflineScore[i].lUnLockScore<ScoreInfoArray[i].lScore) continue;
			}
			
			//计算税收
			SCORE lRevenue=AccountRevenue(ScoreInfoArray[i].lScore);			

			CString s;
			s.Format(_T("shui4: i:%d, m_ScoreInfoArray[i].lScore:%I64d , lRevenue:%I64d "), i, m_ScoreInfoArray[i].lScore, lRevenue);
			CTraceService::TraceString(s, TraceLevel_Debug);

			//积分记录
			m_ScoreInfoArray[i].lRevenue += lRevenue;
			m_ScoreInfoArray[i].ScoreType=ScoreInfoArray[i].ScoreType;
			m_ScoreInfoArray[i].lScore+=ScoreInfoArray[i].lScore-lRevenue;
			m_ScoreInfoArray[i].lBetAmount += ScoreInfoArray[i].lBetAmount;
			
			m_OfflineScore[i].lRevenue = lRevenue;
			m_OfflineScore[i].lScore = ScoreInfoArray[i].lScore-lRevenue;
			m_OfflineScore[i].ScoreType = ScoreInfoArray[i].ScoreType;
			m_OfflineScore[i].lBetAmount = ScoreInfoArray[i].lBetAmount;
						
			//修改胜负
			switch (ScoreInfoArray[i].ScoreType)
			{
			case ScoreType_Win:		//胜局
				{
					m_OfflineScore[i].lWinCount++;
					break;
				}
			case ScoreType_Lose:	//输局
				{
					m_OfflineScore[i].lLostCount++;
					break;
				}
			case ScoreType_Draw:	//和局
				{
					m_OfflineScore[i].lDrawCount++;
					break;
				}
			case ScoreType_Flee:	//逃局
				{
					m_OfflineScore[i].lFleeCount++;
					break;
				}
			}

			if (m_pITableEventSink)
				m_pITableEventSink->OnTableReqOfflineScore(m_OfflineScore[i]);

			m_OfflineScore[i].dwUserID=0;
		}
	}	

	//设置变量
	m_bWriteScore=true;

	return true;
}

//扣服务费
VOID CTableFrameItem::CostForGameStart()
{
	//收费模式
	ASSERT( CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule) );

	for( WORD i = 0; i < m_wChairCount; i++ )
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if( m_bCostGame[i] || pIServerUserItem == NULL ) 
			continue;

		SCORE lRevenue = m_pGameServiceOption->lRevenue;
		if( lRevenue == 0 && m_pIEventServiceCost )
			lRevenue = m_pIEventServiceCost->OnEventQueryCost(pIServerUserItem);
		ASSERT(lRevenue>=0);

		//错误判断（ANSON）
		if (lRevenue<0)
		{
			lRevenue=0L;
			TCHAR szBuffer[128]=TEXT("");
			_sntprintf_s(szBuffer,CountArray(szBuffer),TEXT("税收计算为 %I64d"),lRevenue);
			CTraceService::TraceString(szBuffer,TraceLevel_Debug);
		}

		m_bCostGame[i] = true;

		//积分记录
		CString s;
		s.Format(_T("shui1: i:%d, m_ScoreInfoArray[i].lScore:%I64d , lRevenue:%I64d "),i, m_ScoreInfoArray[i].lScore , lRevenue);
		CTraceService::TraceString(s, TraceLevel_Debug);

		m_ScoreInfoArray[i].lScore -= lRevenue;
		m_ScoreInfoArray[i].lRevenue += lRevenue;
		m_ScoreInfoArray[i].lBeforScore = pIServerUserItem->GetUserScore();

		//写入积分
		pIServerUserItem->WriteUserScore(-lRevenue, 0, 0, lRevenue, ScoreType_Service);
	}

	return;
}

//控制扑克
bool CTableFrameItem::IsControlCard()
{
	return m_bControlCard;
}

//扑克等级
BYTE CTableFrameItem::GetUserCardLevel(WORD wChairID)
{
	//校验参数
	ASSERT (wChairID<CountArray(m_cbUserCardLevel));
	if (wChairID>=CountArray(m_cbUserCardLevel)) return 0;

	return m_cbUserCardLevel[wChairID];
}

//寻找用户
IServerUserItem * CTableFrameItem::SearchUserItem(DWORD dwUserID)
{
	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//桌子用户
	for (WORD i=0;i<m_wChairCount;i++)
	{
		pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	}

	//旁观用户
	do
	{
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	} while (pIServerUserItem!=NULL);

	return NULL;
}

//游戏用户
IServerUserItem * CTableFrameItem::GetTableUserItem(WORD wChairID)
{
	//效验参数
	//ASSERT (wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return NULL;

	//获取用户
	return m_TableUserItemArray[wChairID];
}

//旁观用户
IServerUserItem * CTableFrameItem::EnumLookonUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_LookonUserItemArray.GetCount()) return NULL;
	return m_LookonUserItemArray[wEnumIndex];
}

//设置时间
bool CTableFrameItem::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	//效验参数
	ASSERT(dwTimerID<TIME_TABLE_FRAME_RANGE && dwElapse>0);
	if (dwTimerID>=TIME_TABLE_FRAME_RANGE || dwElapse==0) return false;

	//设置时间
	DWORD dwEngineTimerID=IDI_TABLE_FRAME_START+m_wTableID*TIME_TABLE_FRAME_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->SetTimer(dwEngineTimerID+dwTimerID,dwElapse,dwRepeat,dwBindParameter);

	return true;
}

//删除时间
bool CTableFrameItem::KillGameTimer(DWORD dwTimerID)
{
	//效验参数
	ASSERT(dwTimerID<TIME_TABLE_FRAME_RANGE);
	if (dwTimerID>=TIME_TABLE_FRAME_RANGE) return false;

	//删除时间
	DWORD dwEngineTimerID=IDI_TABLE_FRAME_START+m_wTableID*TIME_TABLE_FRAME_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->KillTimer(dwEngineTimerID+dwTimerID);

	return true;
}

//发送数据
bool CTableFrameItem::SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)
{

	//特殊处理
	if (pIServerUserItem==NULL)
	{
		return m_pIServiceFunction->SendBatchData(BG_COMPUTER,MDM_GF_GAME,wSubCmdID,NULL,0);
	}

	//状态效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//发送数据
	m_pIServiceFunction->SendDataToUser(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

	return true;
}

//发送数据
bool CTableFrameItem::SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//特殊处理
	if (pIServerUserItem==NULL)
	{
		return m_pIServiceFunction->SendBatchData(BG_COMPUTER,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
	}

	//状态效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//发送数据
	m_pIServiceFunction->SendDataToUser(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);

	return true;
}

//发送数据
bool CTableFrameItem::SendDebugData(IServerUserItem* pIServerUserItem, WORD wDebugKind, WORD wSubCmdID)
{
	//发送数据
	return m_pIServiceFunction->SendDebugData(pIServerUserItem, m_wTableID, wDebugKind, wSubCmdID, NULL, 0);
}

//发送数据
bool CTableFrameItem::SendDebugData(IServerUserItem* pIServerUserItem, WORD wDebugKind, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	return m_pIServiceFunction->SendDebugData(pIServerUserItem, m_wTableID, wDebugKind, wSubCmdID, pData, wDataSize);
}

//发送数据
bool CTableFrameItem::SendTableData(WORD wChairID, WORD wSubCmdID, WORD wMainCmdID, BYTE cbDataMask)
{
	//录像数据
	if (m_bVideoData || (cbDataMask & DATA_MASK_VIDEO) != 0)
	{
		if (m_pIGameVideoItem != NULL)
		{
			//插入数据
			m_pIGameVideoItem->InsertVideoData(wChairID, wMainCmdID, wSubCmdID, NULL, 0);

			//补录退出
			if (m_bVideoData) return true;
		}
	}

	//网络数据
	if ((cbDataMask & DATA_MASK_NETWORK)!=0)
	{
		//用户群发
		if (wChairID == INVALID_CHAIR)
		{
			for (WORD i = 0; i < m_wChairCount; i++)
			{
				//获取用户
				IServerUserItem* pIServerUserItem = GetTableUserItem(i);
				if (pIServerUserItem == NULL) continue;

				//效验状态
				ASSERT(pIServerUserItem->IsClientReady() == true);
				if (pIServerUserItem->IsClientReady() == false) continue;

				//发送数据
				m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_GAME, wSubCmdID, NULL, 0);
			}

			return true;
		}
		else
		{
			//获取用户
			IServerUserItem* pIServerUserItem = GetTableUserItem(wChairID);
			if (pIServerUserItem == NULL) return false;

			//效验状态
			ASSERT(pIServerUserItem->IsClientReady() == true);
			if (pIServerUserItem->IsClientReady() == false) return false;

			//发送数据
			m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_GAME, wSubCmdID, NULL, 0);

			return true;
		}
	}

	return false;
}

//发送数据
bool CTableFrameItem::SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID, BYTE cbDataMask)
{
	//录像数据
	if (m_bVideoData || (cbDataMask & DATA_MASK_VIDEO) != 0)
	{
		//录像数据
		if (m_pIGameVideoItem != NULL)
		{
			//插入数据
			m_pIGameVideoItem->InsertVideoData(wChairID, wMainCmdID, wSubCmdID, pData, wDataSize);

			//补录退出
			if (m_bVideoData) return true;
		}
	}

	//网络数据
	if ((cbDataMask & DATA_MASK_NETWORK) != 0)
	{
		//用户群发
		if (wChairID == INVALID_CHAIR)
		{
			for (WORD i = 0; i < m_wChairCount; i++)
			{
				//获取用户
				IServerUserItem* pIServerUserItem = GetTableUserItem(i);
				if ((pIServerUserItem == NULL) || (pIServerUserItem->IsClientReady() == false)) continue;

				//发送数据
				m_pIServiceFunction->SendDataToUser(pIServerUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
			}

			return true;
		}
		else
		{
			//获取用户
			IServerUserItem* pIServerUserItem = GetTableUserItem(wChairID);
			if ((pIServerUserItem == NULL) || (pIServerUserItem->IsClientReady() == false)) return false;

			//发送数据
			m_pIServiceFunction->SendDataToUser(pIServerUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);

			return true;
		}
	}

	return false;
}

//发送数据
bool CTableFrameItem::SendLookonData(WORD wChairID, WORD wSubCmdID, WORD wMainCmdID)
{
	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//枚举用户
	do
	{
		//获取用户
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//效验状态
		//ASSERT (pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//发送数据
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			m_pIServiceFunction->SendDataToUser(pIServerUserItem, wMainCmdID,wSubCmdID,NULL,0);
		}

	} while (true);

	return true;
}

//发送数据
bool CTableFrameItem::SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wMainCmdID)
{
	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//枚举用户
	do
	{
		//获取用户
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//效验状态
		//ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false; 

		//发送数据
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			m_pIServiceFunction->SendDataToUser(pIServerUserItem, wMainCmdID,wSubCmdID,pData,wDataSize);
		}

	} while (true);

	return true;
}

//发送消息
bool CTableFrameItem::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//变量定义
	WORD wEnumIndex=0;

	//发送消息
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

		//发送消息		
		m_pIServiceFunction->SendGameMessage(pIServerUserItem,lpszMessage,wType);
	}

	//枚举用户
	do
	{
		//获取用户
		IServerUserItem * pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//效验状态
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//发送消息		
		m_pIServiceFunction->SendGameMessage(pIServerUserItem,lpszMessage,wType);

	} while (true);

	return true;
}

//房间消息
bool CTableFrameItem::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//特殊处理
	if (pIServerUserItem==NULL)
	{
		return m_pIServiceFunction->SendRoomMessage(lpszMessage,wType);
	}

	//用户效验
	//ASSERT(pIServerUserItem!=NULL);
	//if (pIServerUserItem==NULL) return false;

	//发送消息
	m_pIServiceFunction->SendRoomMessage(pIServerUserItem,lpszMessage,wType);

	return true;
}

//请求事件
bool CTableFrameItem::PostDataBaseRequest(WORD wRequestID, DWORD dwEngineKeyID, DWORD dwTokenID)
{
	//校验标识
	ASSERT(wRequestID > DB_GAME_REQUEST_START && wRequestID <= (DB_GAME_REQUEST_START + DB_GAME_IDENTIFY_RANGE));
	if (wRequestID < DB_GAME_REQUEST_START && wRequestID >(DB_GAME_REQUEST_START + DB_GAME_IDENTIFY_RANGE)) return false;

	return m_pIDataBaseEngine->PostDataBaseRequest(wRequestID, dwEngineKeyID, 0, dwTokenID, NULL, 0);
}
	
//请求事件
bool CTableFrameItem::PostDataBaseRequest(WORD wRequestID, DWORD dwEngineKeyID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//校验标识
	ASSERT(wRequestID > DB_GAME_REQUEST_START && wRequestID <= (DB_GAME_REQUEST_START + DB_GAME_IDENTIFY_RANGE));
	if (wRequestID < DB_GAME_REQUEST_START && wRequestID >(DB_GAME_REQUEST_START + DB_GAME_IDENTIFY_RANGE)) return false;

	return m_pIDataBaseEngine->PostDataBaseRequest(wRequestID, dwEngineKeyID, 0, dwTokenID, pData, wDataSize);
}

//跑马灯公告
bool CTableFrameItem::SendMarqeeNotice(BYTE cbNoticeKind, VOID* pNoticeContent, WORD wNoticeSize)
{
	//发送消息
	return m_pIServiceFunction->SendMarqeeNotice(cbNoticeKind, pNoticeContent, wNoticeSize);
}

//游戏消息
bool CTableFrameItem::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//用户效验
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//发送消息
	return m_pIServiceFunction->SendGameMessage(pIServerUserItem,lpszMessage,wType);
}

//模拟配置
bool CTableFrameItem::ImitateGameOption(IServerUserItem* pIServerUserItem)
{
	//参数校验
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return false;

	//状态判断
	if (m_bGameStarted == true) return true;

	//比赛房间
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_MATCH) != 0)
	{
		if (pIServerUserItem->GetUserMatchStatus() != MUS_PLAYING)
		{
			return false;
		}
	}
	else
	{
		//设置状态
		if (pIServerUserItem->GetUserStatus() == US_SIT)
		{
			//获取方位
			WORD wChairID = pIServerUserItem->GetChairID();

			//准备校验
			if (m_pIEventStartEfficacy != NULL && m_pIEventStartEfficacy->IsAllowPlayerReady(wChairID) == false)
			{
				return false;
			}

			//设置状态
			pIServerUserItem->SetUserStatus(US_READY, m_wTableID, wChairID);
		}
	}

	//设置状态
	pIServerUserItem->SetClientReady(true);

	//开始判断
	if (pIServerUserItem->GetUserStatus() == US_READY)
	{
		//开始模式判断
		if (m_GameStartMode == StartMode_MasterControl)
		{
			//发送状态
			SendStartStatus();
		}
		else
		{
			//开始游戏
			if (EfficacyStartGame(pIServerUserItem->GetChairID()) == true)
			{
				TryStartGame();
			}
		}
	}

	return true;
}

//发送场景
bool CTableFrameItem::OnEventSendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	//用户效验
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;

	//录像数据
	if (m_pIGameVideoItem!=NULL && m_bVideoData==true)
	{
		m_pIGameVideoItem->InsertVideoData(pIServerUserItem->GetChairID(),MDM_GF_FRAME,SUB_GF_GAME_SCENE,pData,wDataSize);

		return true;
	}

	//发送场景
	ASSERT(m_pIServiceFunction!=NULL);
	m_pIServiceFunction->SendDataToUser(pIServerUserItem,MDM_GF_FRAME,SUB_GF_GAME_SCENE,pData,wDataSize);

	return true;
}

//离线锁分
VOID CTableFrameItem::FreezeGameScore(IServerUserItem *pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL&&m_bGameStarted);
	if( pIServerUserItem == NULL || !m_bGameStarted ) return;

	//变量定义
	WORD wChairId = pIServerUserItem->GetChairID();

	//设置变量
	ZeroMemory( &m_OfflineScore[wChairId],sizeof(m_OfflineScore[wChairId]) );
	m_OfflineScore[wChairId].dwUserID = pIServerUserItem->GetUserID();
	m_OfflineScore[wChairId].dwUserRight = pIServerUserItem->GetUserRight();
	m_OfflineScore[wChairId].dwClientIP = pIServerUserItem->GetClientIP();
	m_OfflineScore[wChairId].dwPlayTimeCount = (DWORD)time(NULL)-m_dwGameStartTime;

	m_OfflineScore[wChairId].lUnLockScore = m_pGameServiceOption->lLessScore;
	m_OfflineScore[wChairId].lExperience = (m_OfflineScore[wChairId].dwPlayTimeCount+59L)/60L;

	//设置玩家
	if (m_pGameServiceOption->wServerType & (GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
		pIServerUserItem->FreezeUserScore(m_OfflineScore[wChairId].lUnLockScore,true);

	return;
}

//扣除服务费
VOID CTableFrameItem::ChargeGameAndReset()
{
	if( CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule) )
	{
		//扣服务费
		CostForGameStart();

		//询问玩家
		if( !m_bGameStarted )
			memset(m_bCostGame,true,sizeof(m_bCostGame));
		else
		{
			for( WORD i = 0; i < m_wChairCount; i++ )
			{
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				m_bCostGame[i] = true;
				if( pIServerUserItem == NULL ) continue;

				if ( m_pIEventServiceCost )
					m_bCostGame[i] = !m_pIEventServiceCost->OnEventQueryCostable(pIServerUserItem,false);
				else if( m_pGameServiceOption->lRevenue != 0 )
					m_bCostGame[i] = false;
			}
		}
	}
}

//断线事件
bool CTableFrameItem::OnEventUserOffLine(IServerUserItem * pIServerUserItem)
{
	//参数效验
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//用户变量
	auto pUserInfo=pIServerUserItem->GetUserInfo();
	auto pITableUserItem=m_TableUserItemArray[pUserInfo->wChairID];

	//用户属性
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//游戏用户
	if (cbUserStatus!=US_LOOKON)
	{
		//效验用户
		ASSERT (pIServerUserItem==GetTableUserItem(wChairID));
		if (pIServerUserItem!=GetTableUserItem(wChairID)) return false;

		//变量定义
		bool bForbidOfflineStandup=CServerRule::IsForbidOfflineStandup(m_pGameServiceOption->dwServerRule);

		//断线处理
		if ((cbUserStatus==US_PLAYING)&&(m_wOffLineCount[wChairID]<MAX_OFF_LINE || bForbidOfflineStandup==true))
		{
			//用户设置
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);

			//托管处理
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0 ||
			  ((m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE)!=0 && GetRelationParameter()->dwRelationNum!=0))
			{
				if (pIServerUserItem->IsTrusteeUser() == false)
				{
					//设置托管
					pIServerUserItem->SetTrusteeUser(true);
					pIServerUserItem->SetClientReady(true);

					//掉线通知
					if (m_pITableUserAction != NULL) m_pITableUserAction->OnActionUserOffLine(pIServerUserItem);
				}

				return true;
			}

			//设置托管
			if (bForbidOfflineStandup==true) pIServerUserItem->SetTrusteeUser(true);			

			//事件通知
			if (m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(pIServerUserItem);

			//断线处理
			if (m_dwOffLineTime[wChairID]==0L && m_cbGameStatus>=GAME_STATUS_PLAY)
			{
				//设置变量
				m_wOffLineCount[wChairID]++;
				m_dwOffLineTime[wChairID]=(DWORD)time(NULL);

				//时间设置
				WORD wOffLineCount=GetOffLineUserCount();
				if (wOffLineCount==1) SetGameTimer(IDI_OFF_LINE,TIME_OFF_LINE*1000L,1,wChairID);
			}

			return true;
		}
	}

	//用户起立
	PerformStandUpAction(pIServerUserItem,true);

	//比赛处理
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		if (pIServerUserItem->IsTrusteeUser()==true) return true;
	}

	//删除用户
	if (pIServerUserItem->GetUserStatus()==US_FREE)
	{
		pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR); 
	}

	return true;
}

//重进事件
bool CTableFrameItem::OnEventUserReConnect(IServerUserItem * pIServerUserItem)
{
	//参数效验
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//用户变量
	auto pUserInfo=pIServerUserItem->GetUserInfo();
	auto pITableUserItem=m_TableUserItemArray[pUserInfo->wChairID];

	//用户属性
	auto wChairID=pIServerUserItem->GetChairID();
	auto cbUserStatus=pIServerUserItem->GetUserStatus();

	//设置变量
	m_dwOffLineTime[wChairID]=0L;

	return true;
}

//用户规则事件
bool CTableFrameItem::OnEventUserRule(IServerUserItem *pIServerUserItem)
{
	//判断加密解密
	if (GetSitUserCount()==1)
	{
		//变量定义
		auto pUserRule=pIServerUserItem->GetUserRule();

		//状态变量
		bool bTableLocked=IsTableLocked();

		//设置桌主
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE)==0)
		{
			//设置变量
			m_dwTableOwnerID=pIServerUserItem->GetUserID();
			StringCchCopy(m_szTablePassword,CountArray(m_szTablePassword), pUserRule->szPassword);
		}		

		//发送状态
		if (bTableLocked!=IsTableLocked()) 
		{
			if( m_pITableEventSink )
				m_pITableEventSink->OnEventTableLocked(QUERY_ME_INTERFACE(ITableFrameItem),IsTableLocked()?TRUE:FALSE);

			SendTableStatus();
		}
	}

	return true;
}

//分数变更
VOID CTableFrameItem::OnEventUserScoreChange(IServerUserItem *pIServerUserItem)
{
	//参数校验
	ASSERT(pIServerUserItem!=NULL);
	if(pIServerUserItem==NULL) return;

	//用户查找
	if(GetTableUserItem(pIServerUserItem->GetChairID())!=pIServerUserItem) return;

	//变更通知
	if(m_pIEventUserScore!=NULL)
		m_pIEventUserScore->OnEventScoreVariation(pIServerUserItem);

	return;
}

//定时器事件
bool CTableFrameItem::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//回调事件
	if ((dwTimerID>=0)&&(dwTimerID<TIME_TABLE_SINK_RANGE))
	{
		ASSERT(m_pITableFrameSink!=NULL);
		return m_pITableFrameSink->OnTimerMessage(dwTimerID,dwBindParameter);
	}

	//事件处理
	switch (dwTimerID)
	{
	case IDI_OFF_LINE:			//断线事件
	{
		//效验状态
		ASSERT(m_bGameStarted==true);
		if (m_bGameStarted==false) return true;

		//变量定义
		DWORD dwOffLineTime=0L;
		WORD wOffLineChair=INVALID_CHAIR;

		//寻找用户
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
			{
				wOffLineChair=i;
				dwOffLineTime=m_dwOffLineTime[i];
			}
		}

		//位置判断
		ASSERT (wOffLineChair!=INVALID_CHAIR);
		if (wOffLineChair==INVALID_CHAIR) return true;

		//用户判断
		ASSERT (dwBindParameter<m_wChairCount);
		if (wOffLineChair!=(WORD)dwBindParameter)
		{
			//时间计算
			DWORD dwCurrentTime=(DWORD)time(NULL);
			DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

			//设置时间
			dwLapseTime=__min(dwLapseTime,TIME_OFF_LINE*1000L-2000L);
			SetGameTimer(IDI_OFF_LINE,TIME_OFF_LINE*1000L-dwLapseTime,1,wOffLineChair);

			return true;
		}

		//获取用户
		ASSERT(GetTableUserItem(wOffLineChair)!=NULL);
		IServerUserItem * pIServerUserItem=GetTableUserItem(wOffLineChair);

		//结束游戏
		if (pIServerUserItem!=NULL)
		{
			//设置变量
			m_dwOffLineTime[wOffLineChair]=0L;

			//掉线超时处理
			if (CServerRule::IsForbidOfflineStandup(m_pGameServiceOption->dwServerRule)==false)
			{
				//用户起立
				PerformStandUpAction(pIServerUserItem,true);
			}
		}

		//继续时间
		if (m_bGameStarted==true)
		{
			//变量定义
			DWORD dwOffLineTime=0L;
			WORD wOffLineChair=INVALID_CHAIR;

			//寻找用户
			for (WORD i=0;i<m_wChairCount;i++)
			{
				if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
				{
					wOffLineChair=i;
					dwOffLineTime=m_dwOffLineTime[i];
				}
			}

			//设置时间
			if (wOffLineChair!=INVALID_CHAIR)
			{
				//时间计算
				DWORD dwCurrentTime=(DWORD)time(NULL);
				DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

				//设置时间
				dwLapseTime=__min(dwLapseTime,TIME_OFF_LINE * 1000L -2000L);
				SetGameTimer(IDI_OFF_LINE,TIME_OFF_LINE * 1000L -dwLapseTime,1,wOffLineChair);
			}
		}

		return true;
	}
	case IDI_KICKOUT_PLAYER:	//踢出玩家
	{
		//效验状态
		if (m_bGameStarted==true) return false;

		//变量定义
		DWORD dwUnReadyTime=0L;
		WORD wUnReadyChair=INVALID_CHAIR;

		//寻找用户
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if ((m_dwUserUnReadyTime[i]!=0L)&&((m_dwUserUnReadyTime[i]<dwUnReadyTime)||(wUnReadyChair==INVALID_CHAIR)))
			{
				wUnReadyChair=i;
				dwUnReadyTime=m_dwUserUnReadyTime[i];
			}
		}

		//位置判断
		ASSERT(dwUnReadyTime!=INVALID_CHAIR);
		if (wUnReadyChair==INVALID_CHAIR) return false;

		//用户判断
		ASSERT(dwBindParameter<m_wChairCount);
		if (wUnReadyChair!=(WORD)dwBindParameter)
		{
			//时间计算
			DWORD dwCurrentTime=DWORD(time(NULL));
			DWORD dwLapseTime=dwCurrentTime-m_dwUserUnReadyTime[wUnReadyChair];

			//获取时间
			DWORD dwElapse=TIME_KICKOUT_PLAYER;
			if (m_pIEventCustomFrameTime!=NULL)
			{
				dwElapse=m_pIEventCustomFrameTime->GetUnReadyKickOutTime();
			}

			//设置时间
			dwLapseTime=__min(dwLapseTime,dwElapse-1L);
			SetGameTimer(IDI_KICKOUT_PLAYER,(dwElapse-dwLapseTime)*1000L,1,wUnReadyChair);

			return true;
		}

		//获取用户
		ASSERT (GetTableUserItem(wUnReadyChair)!=NULL);
		IServerUserItem * pIServerUserItem=GetTableUserItem(wUnReadyChair);

		//准备提醒
		if (pIServerUserItem!=NULL)
		{
			//设置变量
			m_dwUserUnReadyTime[wUnReadyChair]=0;

			//发送消息
			m_pIServiceFunction->SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0068,L"由于您长时间没有开始游戏，系统请您重新选择游戏桌！"),SMT_CHAT|SMT_CLOSE_GAME|SMT_EJECT);

			//用户起立
			PerformStandUpAction(pIServerUserItem,true);
		}			

		//设置变量
		dwUnReadyTime=0;
		wUnReadyChair=INVALID_CHAIR;

		//继续寻找
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if ((m_dwUserUnReadyTime[i]!=0L)&&((m_dwUserUnReadyTime[i]<dwUnReadyTime)||(wUnReadyChair==INVALID_CHAIR)))
			{
				wUnReadyChair=i;
				dwUnReadyTime=m_dwUserUnReadyTime[i];
			}
		}

		if (wUnReadyChair!=INVALID_CHAIR)
		{
			//时间计算
			DWORD dwCurrentTime=(DWORD)time(NULL);
			DWORD dwLapseTime=dwCurrentTime-dwUnReadyTime;

			//获取时间
			DWORD dwElapse=TIME_KICKOUT_PLAYER;
			if (m_pIEventCustomFrameTime!=NULL)
			{
				dwElapse=m_pIEventCustomFrameTime->GetUnReadyKickOutTime();
			}

			//设置时间
			dwLapseTime=__min(dwLapseTime,dwElapse-1L);
			SetGameTimer(IDI_KICKOUT_PLAYER,(dwElapse-dwLapseTime)*1000L,1,wUnReadyChair);
		}
		else
		{
			//关闭定时器
			KillGameTimer(IDI_KICKOUT_PLAYER);
		}

		return true;
	}
	case IDI_KICKOUT_LOOKON:	//踢出旁观		
	{
		//判断数量
		if (m_LookonUserItemArray.GetCount() == 0)
		{
			return true;
		}

		//变量定义
		WORD wEnumIndex = 0;
		DWORD dwCurrentTime = (DWORD)time(NULL);
		IServerUserItem * pIServerUserItem = NULL;

		//枚举用户
		do
		{
			//获取用户
			pIServerUserItem = EnumLookonUserItem(wEnumIndex++);
			if (pIServerUserItem == NULL) break;

			//效验状态
			//ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady() == false) return false;

			//踢出判断
			if ((((CServerUserItem *)pIServerUserItem)->GetLookonTime() + ALLOW_LOOKON_TIME) < dwCurrentTime)
			{
				//发送数据					
				m_pIServiceFunction->SendGameMessage(pIServerUserItem, TEXT(""), SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);
			}

		} while (true);

		return true;
	}		
	}

	return false;
}

//游戏选项
VOID CTableFrameItem::OnEventClientReady(IServerUserItem* pIServerUserItem)
{
	return;
}

//桌子初始化
VOID CTableFrameItem::OnEventTableFrameInit(tagTableFrameParameter& TableFrameParameter)
{
	return;
}

//开始事件
bool CTableFrameItem::OnEventGameStart()
{
	return true;
}

//结束时间
bool CTableFrameItem::OnEventGameConclude()
{
	return true;
}

//用户坐下
bool CTableFrameItem::OnActionUserSitdown(IServerUserItem* pIServerUserItem)
{
	return true;
}

//用户起立
bool CTableFrameItem::OnActionUserStandup(IServerUserItem* pIServerUserItem)
{
	return true;
}

//旁观校验
bool CTableFrameItem::EfficacyUserLookon(WORD wChairID, IServerUserItem* pIServerUserItem)
{
	//获取用户
	auto pITableUserItem = GetTableUserItem(wChairID);
	//if (pITableUserItem == NULL) return false;

	//游戏状态
	if (m_bTableStarted == false && pIServerUserItem->GetMasterOrder() == 0 &&
		!CServerRule::IsSitForLookon(m_pGameServiceOption->dwServerRule))
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0085, L"游戏还没有开始，暂时不能旁观此游戏桌！"), 0);
		return false;
	}

	//椅子判断
	if (pITableUserItem == NULL &&
		!CServerRule::IsSitForLookon(m_pGameServiceOption->dwServerRule))
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0086, L"您所请求的位置没有游戏玩家，无法旁观此游戏桌！"), 0);
		return false;
	}

	//状态判断
	if ((g_GlobalUnits.IsAllowEnterGame() == false) && (pIServerUserItem->GetMasterOrder() == 0))
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0087, L"抱歉，此游戏桌暂时不允许用户进入！"), 0);
		return false;
	}

	//密码效验
	if (IsTableLocked() == true && !CUserRight::CanEnterWithoutPasswd(pIServerUserItem->GetUserRight()) &&
		pIServerUserItem->GetMasterOrder() == 0 && !CServerRule::IsForbidPasswordSet(m_pGameServiceOption->dwServerRule))
	{
		if (pIServerUserItem->IsTmpPasswdEmpty() == false)
		{
			if (!pIServerUserItem->ContrastTempPassword(m_szTablePassword))
			{
				//清理临时密码
				pIServerUserItem->SetTmpPassword(L"");

				SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0090, L"桌子密码错误，不能加入游戏！"), RFC_TMP_PASSWD_INCORRECT);
				return false;
			}

			//清理临时密码
			pIServerUserItem->SetTmpPassword(L"");
		}
		else if (!pIServerUserItem->ContrastTablePassword(m_szTablePassword))
		{
			SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0090, L"桌子密码错误，不能加入游戏！"), RFC_PASSWORD_INCORRECT);
			return false;
		}
	}

	return true;
}

//坐下校验
bool CTableFrameItem::EfficacyUserSitdown(WORD wChairID, IServerUserItem* pIServerUserItem)
{
	//获取用户
	auto pITableUserItem = GetTableUserItem(wChairID);

	//状态判断
	if ((g_GlobalUnits.IsAllowEnterGame() == false) && (pIServerUserItem->GetMasterOrder() == 0))
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0087, L"抱歉，此游戏桌暂时不允许用户进入！"), 0);
		return false;
	}

	//游戏状态
	if ((m_bTableStarted == true) &&
		m_pGameServiceAttrib->cbDynamicJoin == FALSE && !CServerRule::IsDynamicJoinGame(m_pGameServiceOption->dwServerRule)
		)
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0092, L"游戏已经开始了，暂时不能进入游戏桌！"), 0);
		return false;
	}

	//椅子判断
	if (pITableUserItem != NULL)
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessageV(MIM_SERVER_0093, L"椅子已经被 [ %s ] 捷足先登了，下次动作要快点了！", pITableUserItem->GetAccounts()), 0);
		return false;
	}

	//规则效验 kk jia
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
	{
		//CString s;
		//s.Format(_T("GetUserScore：%ld  ,< m_lMinEnterScore:%ld "), pIServerUserItem->GetUserScore(), m_lMinEnterScore);
		//CTraceService::TraceString(s, TraceLevel_Exception);


		//分数判断
		if (m_lMinEnterScore != 0L && pIServerUserItem->GetUserScore() < m_lMinEnterScore)
		{

			LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0101, L"加入游戏至少需要 " SCORE_STRING " 个积分，您的积分不足不能加入！", SCORE_ZOOMOUT(m_lMinEnterScore));
			SendRequestFailure(pIServerUserItem, pszMessage, RFC_SCORE_UNENOUGH);
			return false;
		}
	}
	//kk jia end

	//规则效验
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_MATCH) == 0)
	{
		//规则校验
		if (EfficacyAddrRule(pIServerUserItem) == false) return false;

		//排位赛不限制积分
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_RANKING) == 0)
		{
			if (EfficacyTableRule(pIServerUserItem) == false) return false;
			if (EfficacyScoreRule(pIServerUserItem) == false) return false;
		}
	}

	return true;
}

//起立校验
bool CTableFrameItem::EfficacyUserStandup(WORD wChairID, IServerUserItem* pIServerUserItem, bool bInitiative)
{
	//用户属性
	BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

	//游戏用户
	if ((m_bGameStarted == true) && ((cbUserStatus == US_PLAYING) || (cbUserStatus == US_OFFLINE)))
	{
		//强退禁止起立
		if (CServerRule::IsForbidForceStandup(m_pGameServiceOption->dwServerRule) == true)
		{
			//用户设置
			//pIServerUserItem->SetTrusteeUser(true);

			////提示信息
			//if (cbUserStatus == US_OFFLINE)
			//{
			//	CTraceService::TraceString(TEXT("玩家状态为US_OFFLINE时,调用起立！！！！！ "), TraceLevel_Exception);
			//}

			//掉线通知
			//if (m_pITableUserAction != NULL) m_pITableUserAction->OnActionUserOffLine(pIServerUserItem);

			//主动退出
			if (bInitiative == true)
			{
				SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0120, TEXT("抱歉，游戏尚未完成，您暂时不能离开！")), 0);
			}

			return false;
		}

		//结束原因
		BYTE cbReason = cbUserStatus == US_OFFLINE ? GER_NETWORK_ERROR : GER_USER_LEAVE;

		//结束游戏
		m_pITableFrameSink->OnEventGameConclude(wChairID, pIServerUserItem, cbReason);

		//离开判断
		if (m_TableUserItemArray[wChairID] != pIServerUserItem) return false;
	}	

	//比赛托管处理
	//if ((m_pGameServiceOption->wServerType & GAME_GENRE_MATCH) != 0 && bInitiative == true)
	//{
	//	//判断状态
	//	if (pIServerUserItem->GetUserMatchStatus() == MUS_PLAYING)
	//	{
	//		if (pIServerUserItem->IsTrusteeUser() == false)
	//		{
	//			pIServerUserItem->SetTrusteeUser(true);

	//			//掉线通知
	//			if (m_pITableUserAction != NULL) m_pITableUserAction->OnActionUserOffLine(pIServerUserItem);
	//		}

	//		return false;
	//	}
	//}

	return true;
}

//游戏事件
bool CTableFrameItem::OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(m_pITableFrameSink!=NULL);

	//消息处理
	return m_pITableFrameSink->OnGameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//框架事件
bool CTableFrameItem::OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//游戏处理
	if (m_pITableFrameSink->OnFrameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem)==true) return true;

	//默认处理
	switch (wSubCmdID)
	{
	case SUB_GF_GAME_OPTION:	//游戏配置
		{
			//效验参数
			ASSERT (wDataSize == sizeof(CMD_GF_GameOption));
			if (wDataSize != sizeof(CMD_GF_GameOption)) return false;

			//变量定义
			CMD_GF_GameOption * pGameOption = (CMD_GF_GameOption *)pData;

			//获取属性
			WORD wChairID = pIServerUserItem->GetChairID();
			BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

			//断线清理
			if ((cbUserStatus != US_LOOKON) && ((m_dwOffLineTime[wChairID] != 0L)))
			{
				//设置变量
				m_dwOffLineTime[wChairID] = 0L;

				//删除时间
				WORD wOffLineCount = GetOffLineUserCount();
				if (wOffLineCount == 0) KillGameTimer(IDI_OFF_LINE);
			}

			//设置状态
			pIServerUserItem->SetClientReady(true);
			pIServerUserItem->SetTrusteeUser(false);

			//判断状态
			if (cbUserStatus != US_LOOKON)
			{
				bool bAllowLookon = pGameOption->cbAllowLookon ? true : false;
				if (bAllowLookon != m_bAllowLookon[wChairID])
				{
					m_bAllowLookon[wChairID] = bAllowLookon;

					//构造消息
					CMD_GF_LookonStatus LookonStatus;
					LookonStatus.bAllowLookon = bAllowLookon;

					//发送消息
					for (INT_PTR i = 0; i < m_LookonUserItemArray.GetCount(); i++)
					{
						//获取用户
						IServerUserItem * pILookonUserItem = m_LookonUserItemArray[i];
						if (pILookonUserItem->GetChairID() != pIServerUserItem->GetChairID()) continue;

						//发送消息
						ASSERT(m_pIServiceFunction != NULL);
						m_pIServiceFunction->SendDataToUser(pILookonUserItem, MDM_GF_FRAME, SUB_GF_LOOKON_STATUS, &LookonStatus, sizeof(LookonStatus));
					}
				}
			}

			//发送状态
			CMD_GF_GameStatus GameStatus;
			GameStatus.bGamePaused = m_bGamePaused;
			GameStatus.cbGameStatus = m_cbGameStatus;
			GameStatus.cbAllowLookon = m_bAllowLookon[wChairID] ? TRUE : FALSE;
			m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_FRAME, SUB_GF_GAME_STATUS, &GameStatus, sizeof(GameStatus));

			//自动准备
			bool bAutoReady = false;

			//约战模式
			if ((m_pGameServiceOption->wServerType&(GAME_GENRE_BATTLE|GAME_GENRE_RANKING)) != 0 && GetRelationParameter()->dwRelationNum != 0)
			{
				//校验状态
				if (cbUserStatus==US_SIT && m_cbGameStatus==GAME_STATUS_FREE && (m_GameStartMode==StartMode_MasterControl || m_GameStartMode==StartMode_FullReady))
				{
					bAutoReady = true;
				}
			}

			//分组模式
			if (m_pGameServiceOption->wServerType&(TABLE_GENRE_DISTRIBUTE|GAME_GENRE_RANKING))
			{
				//判断状态
				if (cbUserStatus == US_SIT)
				{
					//请求事件
					if (m_pITableUserRequest != NULL && !m_pITableUserRequest->OnTableUserReqReady(pIServerUserItem, NULL, 0))
					{
						return false;
					}

					//事件通知
					if ((m_pITableUserAction != NULL) && (m_pITableUserAction->OnActionUserReady(pIServerUserItem, NULL, 0) == false))
					{
						ASSERT(FALSE);
						return false;
					}

					bAutoReady = true;
				}
			}			

			//事件调用
			OnEventClientReady(pIServerUserItem);

			//发送场景
			bool bSendSecret=((cbUserStatus!=US_LOOKON)||(m_bAllowLookon[wChairID]==true));
			m_pITableFrameSink->OnEventSendGameScene(wChairID,pIServerUserItem,m_cbGameStatus,bSendSecret);			

			//同IP提醒
			if (m_bSameIpRemind == true && GetSitUserCount() >= 3)
			{
				//变量定义		
				CWordArray ChairArray;
				CWordArray FilterArray;
				IServerUserItem* pIServerUserItem1 = NULL;
				IServerUserItem* pIServerUserItem2 = NULL;
				IServerUserItem* pIServerUserItem3 = NULL;

				//获取用户
				for (WORD wChair1 = 0; wChair1 < m_wChairCount; wChair1++)
				{
					//获取用户
					if (!(pIServerUserItem1 = GetTableUserItem(wChair1))) continue;

					//移除元素
					FilterArray.RemoveAll();

					//查找同IP
					for (WORD wChair2 = 0; wChair2 < m_wChairCount; wChair2++)
					{
						//条件过滤
						if (wChair1 == wChair2) continue;
						if (!(pIServerUserItem2 = GetTableUserItem(wChair2))) continue;
						if (pIServerUserItem2->GetClientIP() == pIServerUserItem1->GetClientIP()) continue;

						//过滤方位
						bool bExists = false;
						for (INT_PTR i = 0; i < FilterArray.GetCount(); i++)
						{
							if (wChair2 == FilterArray[i])
							{
								bExists = true;
								break;
							}
						}

						//存在判断
						if (bExists == true) continue;

						//添加椅子
						ChairArray.Add(wChair2);

						//查找同IP
						for (WORD wChair3 = wChair2 + 1; wChair3 < m_wChairCount; wChair3++)
						{
							//条件过滤
							if (wChair3 == wChair1) continue;
							if (!(pIServerUserItem3 = GetTableUserItem(wChair3))) continue;
							if (pIServerUserItem3->GetClientIP() != pIServerUserItem2->GetClientIP()) continue;

							//添加椅子
							ChairArray.Add(wChair3);
						}

						//判断数量
						if (ChairArray.GetCount() >= 2)
						{
							//变量定义					
							CStringW strMessage;
							TCHAR szMessage[128] = L"";

							//构造提示
							for (INT_PTR i = 0; i < ChairArray.GetCount(); i++)
							{
								_snwprintf_s(szMessage, CountArray(szMessage), L"玩家: %s", GetTableUserItem(ChairArray[i])->GetAccounts());
								strMessage.Append(szMessage);
								if (i < ChairArray.GetCount() - 1) strMessage.Append(L"与");
							}

							//连接提示
							strMessage.Append(L"的IP地址相同！");

							//发送消息
							SendGameMessage(pIServerUserItem1, CWHService::FormatMessage(MIM_SERVER_0071, L"请注意，本桌有玩家IP地址相同！"), SMT_TABLE_ROLL | SMT_SHOW_MOBILE);
						}

						//清空数组
						FilterArray.Append(ChairArray);
						ChairArray.RemoveAll();
					}
				}
			}
			
			//自动准备
			if (bAutoReady)
			{
				return OnEventSocketFrame(SUB_GF_USER_READY,0,0,pIServerUserItem);
			}

			return true;
		}
	case SUB_GF_USER_READY:		//用户准备
		{
			//获取属性
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//效验状态
			ASSERT(GetTableUserItem(wChairID)==pIServerUserItem);
			if (GetTableUserItem(wChairID)!=pIServerUserItem) return false;

			//效验状态
			//ASSERT(cbUserStatus==US_SIT);
			if (cbUserStatus!=US_SIT) return true;

			//准备验证
			if (m_pIEventStartEfficacy!=NULL)
			{
				if (m_pIEventStartEfficacy->IsAllowPlayerReady(wChairID)==false) 
				{
					return true;
				}
			}

			//设置变量
			m_dwUserUnReadyTime[wChairID]=0;

			//比赛类型
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
			{
				return true;
			}

			//桌子已解散
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE)!=0 && GetRelationParameter()->dwRelationNum==0)
			{		
				//发送消息
				SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0047,L"抱歉，本房间已被解散！"),SMT_CHAT|SMT_SHOW_MOBILE);

				return true;				
			}

			//分组模式
			if (m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)
			{
				//发送消息
				SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0072, L"系统重新分配桌子，请稍后！"),SMT_CHAT);

				//发送消息
				m_pIServiceFunction->InsertDistribute(pIServerUserItem);

				return true;
			}

			//请求事件
			if (m_pITableUserRequest != NULL && !m_pITableUserRequest->OnTableUserReqReady(pIServerUserItem,pData,wDataSize))
			{
				return false;
			}

			//事件通知
			if ((m_pITableUserAction!=NULL)&&(m_pITableUserAction->OnActionUserReady(pIServerUserItem,pData,wDataSize)==false))
			{
				ASSERT(FALSE);
				return false;
			}			

			//准备状态
			pIServerUserItem->SetUserStatus(US_READY, m_wTableID, wChairID);

			//开始判断
			if (EfficacyStartGame(wChairID)==true)
			{
				//开始模式
				if ((m_GameStartMode != StartMode_MasterControl))
				{
					//开始游戏
					TryStartGame();
				}
			}

			return true;
		}
	case SUB_GF_START_GAME:		//开始游戏
		{
			//用户信息
			WORD dwUserID = pIServerUserItem->GetUserID();
			WORD wChairID = pIServerUserItem->GetChairID();			
			BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

			//效验状态
			ASSERT (GetTableUserItem(wChairID) == pIServerUserItem);
			if (GetTableUserItem(wChairID) != pIServerUserItem) return false;

			//效验状态
			//ASSERT (cbUserStatus == US_READY);
			if (cbUserStatus == US_PLAYING ) return true;
			if (cbUserStatus != US_READY) return false;

			//校验模式
			ASSERT (m_GameStartMode == StartMode_MasterControl);
			if (m_GameStartMode != StartMode_MasterControl) return false;

			//验证接口
			ASSERT (m_pIEventStartEfficacy != NULL);
			if (m_pIEventStartEfficacy == NULL) return NULL;

			//校验身份
			if (dwUserID != m_dwTableOwnerID)
			{
				//获取方位			
				WORD wMasterChairID = m_pIEventStartEfficacy->GetMasterChairID();
				if (wMasterChairID != wChairID) return false;
			}					

			//开始判断
			if (EfficacyStartGame(INVALID_CHAIR) == false)
			{
				//构造结构
				CMD_GF_StartStatus StartStatus;
				ZeroMemory(&StartStatus,sizeof(StartStatus));

				//设置状态
				StartStatus.bEnabled = false;
				StartStatus.bVisibled = true;

				//发送数据
				m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_FRAME, SUB_GF_START_STATUS, &StartStatus, sizeof(StartStatus));
			}
			else
			{				
				//开始游戏
				TryStartGame();
			}

			return true;
		}
	case SUB_GF_USER_CHAT:		//用户聊天
		{
			//变量定义
			CMD_GF_UserChat * pUserChat=(CMD_GF_UserChat *)pData;

			//效验参数
			ASSERT(wDataSize>=(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatString)));
			ASSERT(wDataSize==(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(TCHAR)));

			//效验参数
			if (wDataSize<(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatString))) return false;
			if (wDataSize!=(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(TCHAR))) return false;

			//目标用户
			if ((pUserChat->dwTargetUserID!=0)&&(SearchUserItem(pUserChat->dwTargetUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//会员旁观
			if ((pIServerUserItem->GetUserStatus()==US_LOOKON)&&(pIServerUserItem->GetMasterOrder()==0))
			{
				if(g_GlobalUnits.GetAllowLookOnChat() == 0)
				{
					SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0073, TEXT("抱歉，为了避免影响游戏的正常进行，系统禁止玩家在旁观过程中发言！")),SMT_EJECT|SMT_CHAT);
					return true;
				}
				else if((g_GlobalUnits.GetAllowLookOnChat()==1)&&(pIServerUserItem->GetMemberOrder()==0))
				{
					TCHAR szBrowseUrl[256]=L"";
					TCHAR szMessage[512]=L"";

					SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0074, TEXT("抱歉！此房间旁观聊天功能只对会员开放")),SMT_EJECT|SMT_CHAT);
					return true;
				}
				
			}

			//状态判断
			if ((g_GlobalUnits.IsAllowGameChat()==false)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				//不是会员
				if( !g_GlobalUnits.IsMemberCanChat() || pIServerUserItem->GetMemberOrder() == 0 )
				{
					SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0075, TEXT("抱歉，此游戏房间不允许游戏聊天！")),SMT_CHAT);
					return true;
				}
			}

			//权限判断
			if (CUserRight::CanGameChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0076, TEXT("抱歉，您没有游戏聊天的权限，若需要帮助，请联系游戏客服咨询！")),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//禁言校验
			if (pIServerUserItem->GetShutUpStatus()!=0 && pIServerUserItem->GetShutUpOverTime()>time(NULL))
			{
				SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0076, TEXT("很抱歉，您已被管理员禁言，请联系客服了解详细情况！")), SMT_EJECT | SMT_CHAT);
				return true;
			}

			CMD_GF_UserChat UserChat;
			ZeroMemory(&UserChat,sizeof(UserChat));

			UserChat.crStringColor=pUserChat->crStringColor;
			UserChat.dwSendUserID=pUserChat->dwSendUserID;
			UserChat.dwTargetUserID=pUserChat->dwTargetUserID;
			StringCchCopy(UserChat.szChatString,CountArray(UserChat.szChatString), pUserChat->szChatString);
			m_pIServiceFunction->FilterWords(UserChat.szChatString, CountArray(UserChat.szChatString));
			UserChat.wChatLength=CountStringBuffer(UserChat.szChatString);
			wDataSize=sizeof(UserChat)-sizeof(UserChat.szChatString)+UserChat.wChatLength*sizeof(TCHAR);

			//游戏用户
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//获取用户
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

				//发送数据
				m_pIServiceFunction->SendDataToUser(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wDataSize);
			}			

			//发送旁观玩家
			SendLookonData(INVALID_CHAIR, SUB_GF_USER_CHAT, &UserChat, wDataSize, MDM_GF_FRAME);

			return true;
		}
	case SUB_GF_USER_VOICE:		//用户语音
		{
			//效验参数
			ASSERT(wDataSize>=sizeof(CMD_GF_SendVoice));
			if (wDataSize<sizeof(CMD_GF_SendVoice)) return false;

			//提取数据
			CMD_GF_SendVoice * pSendVoice=(CMD_GF_SendVoice *)pData;

			//语音开关
			if (CServerRule::IsAllowVoiceChat(m_pGameServiceOption->dwServerRule)==false)
			{
				//消息提醒
				SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0077,L"抱歉，本房间未开放语音聊天功能！"),SMT_EJECT|SMT_CHAT| SMT_PROMPT);
				return true;
			}

			//目标用户
			if ((pSendVoice->dwTargetUserID!=0)&&(SearchUserItem(pSendVoice->dwTargetUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}
			
			//变量定义
			BYTE cbDataBuffer[SOCKET_PACKET];			
			CMD_GF_UserVoice * pUserVoice=(CMD_GF_UserVoice *)cbDataBuffer;
			
			//构造结构
			pUserVoice->dwSendUserID=pIServerUserItem->GetUserID();
			CopyMemory((pUserVoice+1),(pSendVoice+1),sizeof(cbDataBuffer)-sizeof(CMD_GF_UserVoice));

			//计算大小
			WORD wSendSize=wDataSize-sizeof(CMD_GF_SendVoice)+sizeof(CMD_GF_UserVoice);

			//变量定义
			WORD wEnumIndex=0;	
			IServerUserItem * pISendUserItem=NULL;

			//游戏用户
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//获取用户
				pISendUserItem=GetTableUserItem(i);
				if ((pISendUserItem==NULL)||(pISendUserItem->IsClientReady()==false)) continue;
				if (pSendVoice->dwTargetUserID!=0 && pISendUserItem->GetUserID()!=pSendVoice->dwTargetUserID) continue;

				//发送数据
				m_pIServiceFunction->SendDataToUser(pISendUserItem,MDM_GF_FRAME,SUB_GF_USER_VOICE,&cbDataBuffer,wSendSize);
			}	

			//发送旁观玩家
			SendLookonData(INVALID_CHAIR, SUB_GF_USER_VOICE, &cbDataBuffer, wSendSize, MDM_GF_FRAME);			

			return true;
		}
	case SUB_GF_LOOKON_CONFIG:	//旁观配置
		{
			//效验参数
			ASSERT(wDataSize==sizeof(CMD_GF_LookonConfig));
			if (wDataSize<sizeof(CMD_GF_LookonConfig)) return false;

			//变量定义
			CMD_GF_LookonConfig * pLookonConfig=(CMD_GF_LookonConfig *)pData;

			//目标用户
			if ((pLookonConfig->dwUserID!=0)&&(SearchUserItem(pLookonConfig->dwUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//用户效验
			ASSERT(pIServerUserItem->GetUserStatus()!=US_LOOKON);
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return false;

			//旁观处理
			if (pLookonConfig->dwUserID!=0L)
			{
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//获取用户
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetUserID()!=pLookonConfig->dwUserID) continue;
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//构造消息
					CMD_GF_LookonStatus LookonStatus;
					LookonStatus.bAllowLookon=pLookonConfig->bAllowLookon;

					//发送消息
					ASSERT(m_pIServiceFunction!=NULL);
					m_pIServiceFunction->SendDataToUser(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));

					break;
				}
			}
			else
			{
				//设置判断
				bool bAllowLookon=(pLookonConfig->bAllowLookon==TRUE)?true:false;
				if (bAllowLookon==m_bAllowLookon[pIServerUserItem->GetChairID()]) return true;

				//设置变量
				m_bAllowLookon[pIServerUserItem->GetChairID()]=bAllowLookon;

				//构造消息
				CMD_GF_LookonStatus LookonStatus;
				LookonStatus.bAllowLookon=pLookonConfig->bAllowLookon;

				//发送消息
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//获取用户
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//发送消息
					ASSERT(m_pIServiceFunction!=NULL);
					m_pIServiceFunction->SendDataToUser(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));
				}
			}

			return true;
		}
	case SUB_GF_USER_LOOK2SIT:	//坐下消息
		{
			//状态判断
			ASSERT (pIServerUserItem->GetUserStatus()==US_LOOKON) ;
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON) return true;

			//变量定义
			CMD_GF_Look2Sit *pLook2Sit=(CMD_GF_Look2Sit *)pData;

			//效验参数
			ASSERT(pLook2Sit->wChairID<m_wChairCount);
			if(pLook2Sit->wChairID>=m_wChairCount) return true;

			//执行动作
			if( PerformStandUpAction(pIServerUserItem) )
				PerformSitDownAction(pLook2Sit->wChairID,pIServerUserItem);

			return true;
		}
	case SUB_GF_USER_SIT2LOOK:	//站起消息
		{	
			//状态判断
			ASSERT (pIServerUserItem->GetUserStatus()==US_SIT || pIServerUserItem->GetUserStatus()==US_PLAYING) ;
			if (pIServerUserItem->GetUserStatus()!=US_SIT && pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//变量定义
			CMD_GF_Sit2Look *pSit2Look=(CMD_GF_Sit2Look *)pData;

			//效验参数
			ASSERT(pSit2Look->wChairID<m_wChairCount);
			if(pSit2Look->wChairID>=m_wChairCount) return true;

			//执行动作
			if( PerformStandUpAction(pIServerUserItem) )
				PerformLookonAction(pSit2Look->wChairID,pIServerUserItem);

			return true;
		}		
	}

	return false;
}

//获取空位
WORD CTableFrameItem::GetNullChairID()
{
	//椅子搜索
	for (WORD i=0;i< m_wMaxPlayerCount;i++)
	{
		if (m_TableUserItemArray[i]==NULL)
		{
			return i;
		}
	}

	return INVALID_CHAIR;
}

//随机空位
WORD CTableFrameItem::GetRandNullChairID()
{
	//椅子搜索
	WORD wIndex = WORD(Random_Double*(m_wMaxPlayerCount-1));
	for (WORD i=wIndex;i< m_wMaxPlayerCount +wIndex;i++)
	{
		WORD wChairID=i% m_wMaxPlayerCount;
		if (m_TableUserItemArray[wChairID]==NULL)
		{
			return wChairID;			
		}
	}

	return INVALID_CHAIR;
}

//用户数目
WORD CTableFrameItem::GetSitUserCount()
{
	//变量定义
	WORD wUserCount=0;

	//数目统计
	for (WORD i=0;i< m_wMaxPlayerCount;i++)
	{
		if (GetTableUserItem(i)!=NULL) wUserCount++;
	}

	return wUserCount;
}

//旁观数目
WORD CTableFrameItem::GetLookonUserCount()
{
	//获取数目
	INT_PTR nLookonCount=m_LookonUserItemArray.GetCount();

	return (WORD)(nLookonCount);
}

//断线数目
WORD CTableFrameItem::GetOffLineUserCount()
{
	//变量定义
	WORD wOffLineCount=0;

	//断线人数
	for (WORD i=0;i< m_wMaxPlayerCount;i++)
	{
		if (m_dwOffLineTime[i]!=0L) wOffLineCount++;
	}

	return wOffLineCount;
}

//坐下倍数
VOID CTableFrameItem::SetSitdownGameMultiple(WORD wGameMultiple)
{
	m_wSitdownMultiple = wGameMultiple;
}

//起立倍数
VOID CTableFrameItem::SetStandupGameMultiple(WORD wGameMultiple)
{
	m_wStandupMultiple = wGameMultiple;
}

//关联椅子
WORD CTableFrameItem::GetRelationChairID(DWORD dwUserID)
{
	return INVALID_WORD;
}

//关联参数
tagRelationParameter * CTableFrameItem::GetRelationParameter()
{
	//构造结构
	static tagRelationParameter RelationParameter = {0};

	return &RelationParameter;
}

//未准备数目
WORD CTableFrameItem::GetUnReadyUserCount()
{
	//变量定义
	WORD wUnReadyCount=0;

	//未准备人数
	for (WORD i=0;i< m_wMaxPlayerCount;i++)
	{
		if (m_dwUserUnReadyTime[i]!=0) wUnReadyCount++;
	}

	return wUnReadyCount;
}

//真人数目
WORD CTableFrameItem::GetRealUserCount()
{
	//变量定义
	WORD wRealUserCount = 0;

	//机器人数
	for (WORD i = 0; i < m_wMaxPlayerCount; i++)
	{
		IServerUserItem* pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem != NULL && !pIServerUserItem->IsRobotUser()) wRealUserCount++;
	}

	return wRealUserCount;
}

//机器数目
WORD CTableFrameItem::GetRobotUserCount()
{
	//变量定义
	WORD wRobotUserCount=0;

	//机器人数
	for (WORD i=0;i< m_wMaxPlayerCount;i++)
	{
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem!=NULL && pIServerUserItem->IsRobotUser()==true) wRobotUserCount++;
	}

	return wRobotUserCount;
}

//配置桌子
bool CTableFrameItem::InitTableFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter)
{
	//设置变量
	m_wTableID=wTableID;
	m_lBaseScore=TableFrameParameter.pGameServiceOption->lCellScore;
	m_wChairCount=TableFrameParameter.pGameServiceAttrib->wChairCount;	
	m_wMaxPlayerCount=TableFrameParameter.pGameServiceAttrib->wChairCount;

	//配置参数	
	m_pGameServiceAttrib=TableFrameParameter.pGameServiceAttrib;
	m_pGameServiceOption=TableFrameParameter.pGameServiceOption;

	//组件接口
	m_pITimerEngine=TableFrameParameter.pITimerEngine;
	m_pIServiceFunction=TableFrameParameter.pIServiceFunction;
	m_pIGameVideoManager=TableFrameParameter.pIGameVideoManager;
	m_pIDataBaseEngine=TableFrameParameter.pIDataBaseEngineEngine;
	m_pIRobotUserManager=TableFrameParameter.pIRobotUserManager;	
	m_pIAssistDataBaseEngine=TableFrameParameter.pIAssistDataBaseEngine;		

	//回调接口
	m_pITableEventSink = TableFrameParameter.pITableEventSink;

	//创建桌子
	IGameServiceManager * pIGameServiceManager=TableFrameParameter.pIGameServiceManager;
	m_pITableFrameSink=(ITableFrameSink *)pIGameServiceManager->CreateTableFrameSink(IID_ITableFrameSink,VER_ITableFrameSink);

	//错误判断
	if (m_pITableFrameSink==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置桌子
	IUnknownEx * pITableFrame=QUERY_ME_INTERFACE(IUnknownEx);
	if (m_pITableFrameSink->InitTableFrameSink(pITableFrame)==false) return false;	

	//扩展接口
	m_pIGameStockQuery=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink, IGameStockQuery);
	m_pITableUserAction=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserAction);
	m_pIEventUserScore=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,IEventUserScore);
	m_pITableUserRequest = QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserRequest);
	m_pIEventBaseScore=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,IEventBaseScore);
	m_pIEventStartEfficacy=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,IEventStartEfficacy);
	m_pIEventServiceCost=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,IEventServiceCost);
	m_pIEventCustomFrameTime=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,IEventCustomFrameTime);
	m_pITableControlSendCard = QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink, ITableControlSendCard);

	//事件通知
	OnEventTableFrameInit(TableFrameParameter);

	return true;
}

//尝试开始
bool CTableFrameItem::TryStartGame()
{
	//更新状态
	if (m_pITableEventSink)
	{
		m_pITableEventSink->OnEventTableStatus(QUERY_ME_INTERFACE(ITableFrameItem), TRUE);
	}

	return StartGame();
}

//校验密码
bool CTableFrameItem::ContrastPassword(LPCTSTR pszPassword)
{
	//效验参数
	ASSERT (pszPassword != NULL);
	if (pszPassword == NULL) return false;

	bool bResult= lstrcmp(m_szTablePassword, pszPassword) == 0;

	if (bResult == false)
	{
		CTraceService::TraceString(TEXT("TablePassword:%ls,Password:%ls"), TraceLevel_Exception, m_szTablePassword, pszPassword);
	}

	return bResult;
}

//桌子状况
WORD CTableFrameItem::GetTableUserInfo(tagTableUserInfo & TableUserInfo)
{
	//设置变量
	ZeroMemory(&TableUserInfo,sizeof(TableUserInfo));

	//用户分析
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//用户类型
		if (pIServerUserItem->IsRobotUser()==false)
		{
			TableUserInfo.wTableUserCount++;			
			if (pIServerUserItem->IsClientReady()==true)
			{
				TableUserInfo.wClientReadyCount++;
			}
		}
		else
		{
			TableUserInfo.wTableRobotCount++;
		}

		//准备判断
		if (pIServerUserItem->GetUserStatus()==US_READY)
		{
			TableUserInfo.wTableReadyCount++;
		}
	}

	//最少数目
	switch (m_GameStartMode)
	{
	case StartMode_AllReady:		//所有准备
		{
			TableUserInfo.wMinUserCount=2;
			if (m_pGameServiceOption->wLessStartPlayer>TableUserInfo.wMinUserCount)
			{
				TableUserInfo.wMinUserCount=m_pGameServiceOption->wLessStartPlayer;
			}
			break;
		}
	case StartMode_Symmetry:		//配对开始
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case StartMode_TimeControl:	//时间控制
		{
			TableUserInfo.wMinUserCount=1;
			break;
		}
	default:						//默认模式
		{
			TableUserInfo.wMinUserCount=m_pGameServiceAttrib->wChairCount;
			break;
		}
	}

	return TableUserInfo.wTableRobotCount+TableUserInfo.wTableUserCount;
}

//起立动作
bool CTableFrameItem::PerformStandUpAction(IServerUserItem * pIServerUserItem, bool bInitiative)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	ASSERT(pIServerUserItem->GetChairID()<=m_wChairCount);

	//用户属性	
	WORD wChairID=pIServerUserItem->GetChairID();
	DWORD dwUserID=pIServerUserItem->GetUserID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	//请求事件
	if (m_pITableUserRequest && !m_pITableUserRequest->OnTableUserReqStandUp(pIServerUserItem, pIServerUserItem != pITableUserItem))
	{
		return false;
	}

	//扣服务费
	if (pIServerUserItem == pITableUserItem &&
		CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule))
	{
		if( !m_bCostGame[wChairID] )
		{
			m_bCostGame[wChairID] = true;

			SCORE lRevenue = m_pGameServiceOption->lRevenue;
			if( lRevenue == 0 && m_pIEventServiceCost )
				lRevenue = m_pIEventServiceCost->OnEventQueryCost(pIServerUserItem);
			ASSERT(lRevenue>=0);

			//积分记录
			m_ScoreInfoArray[wChairID].lScore -= lRevenue;
			m_ScoreInfoArray[wChairID].lRevenue += lRevenue;
			m_ScoreInfoArray[wChairID].lBeforScore = pIServerUserItem->GetUserScore();

			//写入积分
			pIServerUserItem->WriteUserScore(-lRevenue,0,0,lRevenue,ScoreType_Service);
		}
	}

	//起立校验
	if (!EfficacyUserStandup(wChairID, pIServerUserItem, bInitiative))
	{
		return true;
	}	

	//处理起立
	if (pIServerUserItem==pITableUserItem)
	{
		//设置变量
		m_dwLastLeaveTime=(DWORD)time(NULL);
		m_TableUserItemArray[wChairID]=NULL;		

		//事件通知
		if (m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserStandUp(pIServerUserItem,false);

		//起立事件
		OnActionUserStandup(pIServerUserItem);

		//用户状态
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetUserStatus((cbUserStatus==US_OFFLINE)?US_NULL:US_FREE,INVALID_TABLE,INVALID_CHAIR);

		//变量定义
		bool bTableLocked=IsTableLocked();
		bool bTableStarted=IsTableStarted();
		WORD wTableUserCount=GetSitUserCount();

		//重置桌子信息
		if (wTableUserCount==0 && (m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE)==0)
		{
			//设置变量
			m_dwPlayCount=0;
			m_dwTableOwnerID=0L;
			m_szTablePassword[0]=0;						

			//非比赛类型
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
			{
				//变更底分
				SetCellScore(m_pGameServiceOption->lCellScore);			
			}
		}		

		//结束桌子
		if ((m_bGameStarted==false)&&(m_bTableStarted==true))
		{
			if (m_GameStartMode==StartMode_Symmetry) m_bTableStarted=false;
			if (m_GameStartMode==StartMode_FullReady) m_bTableStarted=false;
			if (m_GameStartMode==StartMode_AllReady)
			{
				if(wTableUserCount<=1) m_bTableStarted=false;
			}
			if (CServerRule::IsTableEndImmediate(m_pGameServiceOption->dwServerRule)) m_bTableStarted=false;
		}

		//桌子状态
		if( bTableLocked != IsTableLocked() && m_pITableEventSink )
			m_pITableEventSink->OnEventTableLocked(QUERY_ME_INTERFACE(ITableFrameItem),IsTableLocked()?TRUE:FALSE);

		if( bTableStarted != IsTableStarted() && m_pITableEventSink )
			m_pITableEventSink->OnEventTableStatus(QUERY_ME_INTERFACE(ITableFrameItem),IsTableStarted()?TRUE:FALSE);

		//发送状态
		if ((bTableLocked!=IsTableLocked())||(bTableStarted!=IsTableStarted())) SendTableStatus();

		//设置变量
		m_dwUserUnReadyTime[wChairID]=0;

		//开始判断
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
		{
			//开始模式判断
			if (m_GameStartMode == StartMode_MasterControl)
			{
				//发送状态
				SendStartStatus();			
			}
			else
			{
				//开始游戏
				if (EfficacyStartGame(INVALID_CHAIR) == true)
				{
					TryStartGame();
				}
			}
		}

		//分组模式
		if (m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE && m_pGameServiceOption->cbDistributeMode==DISTRIBUTE_MODE_DELAYWAIT)
		{
			//游戏状态
			if (IsGameStarted()==false && m_dwPlayCount==0)
			{
				//桌子状况			
				tagTableUserInfo TableUserInfo;
				WORD wUserSitCount=GetTableUserInfo(TableUserInfo);

				//重新分组
				if (wUserSitCount<TableUserInfo.wMinUserCount)
				{
					for (int i=0; i<m_wChairCount; i++)
					{
						if (m_TableUserItemArray[i]!=NULL)
						{
							//发送消息
							SendGameMessage(m_TableUserItemArray[i], CWHService::FormatMessage(MIM_SERVER_0084, L"由于玩家离开，系统重新分配桌子，请稍后！"),SMT_CHAT);

							//发送消息
							m_pIServiceFunction->InsertDistribute(m_TableUserItemArray[i]);
						}
					}
				}
			}
		}

		return true;
	}
	else
	{
		//起立处理
		for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
		{
			if (pIServerUserItem==m_LookonUserItemArray[i])
			{
				//删除子项
				m_LookonUserItemArray.RemoveAt(i);

				//事件通知
				if (m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserStandUp(pIServerUserItem,true);

				//用户状态
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);

				return true;
			}
		}

		//错误断言
		ASSERT(FALSE);
	}

	return true;
}

//旁观动作
bool CTableFrameItem::PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT((pIServerUserItem!=NULL)&&(wChairID<m_wChairCount));
	ASSERT((pIServerUserItem->GetTableID()==INVALID_TABLE)&&(pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//变量定义
	auto pUserInfo=pIServerUserItem->GetUserInfo();
	auto pUserRule=pIServerUserItem->GetUserRule();
	
	//旁观检验
	if (EfficacyUserLookon(wChairID, pIServerUserItem) == false)
	{
		return false;
	}	

	//请求事件
	if (m_pITableUserRequest && !m_pITableUserRequest->OnTableUserReqSitDown(pIServerUserItem, true))
	{
		return false;
	}

	//设置用户
	m_LookonUserItemArray.Add(pIServerUserItem);

	//用户状态
	pIServerUserItem->SetClientReady(false);
	pIServerUserItem->SetUserStatus(US_LOOKON,m_wTableID,wChairID);

	//设置时间
	((CServerUserItem *)pIServerUserItem)->SetLookonTime((DWORD)time(NULL));

	//事件通知
	if (m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserSitDown(pIServerUserItem,true);	

	return true;
}

//坐下动作
bool CTableFrameItem::PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//效验参数
	ASSERT ((pIServerUserItem!=NULL)&&(wChairID<m_wChairCount));
	if ((pIServerUserItem == NULL) || (wChairID >= m_wChairCount)) return false;

	//校验状态
	BYTE cbLastStatus = pIServerUserItem->GetUserStatus();
	if (cbLastStatus == US_LOOKON)
	{
		ASSERT((pIServerUserItem->GetTableID() == INVALID_TABLE || pIServerUserItem->GetTableID() == m_wTableID));
		if ((pIServerUserItem->GetTableID() != INVALID_TABLE && pIServerUserItem->GetTableID() != m_wTableID)) return false;		
	}
	else
	{
		ASSERT((pIServerUserItem->GetTableID() == INVALID_TABLE) && (pIServerUserItem->GetChairID() == INVALID_CHAIR));
		if ((pIServerUserItem->GetTableID() != INVALID_TABLE) || (pIServerUserItem->GetChairID() != INVALID_CHAIR)) return false;
	}

	//变量定义	
	auto pUserInfo=pIServerUserItem->GetUserInfo();
	auto pUserRule=pIServerUserItem->GetUserRule();
	auto pITableUserItem=GetTableUserItem(wChairID);

	//坐下校验
	if (!EfficacyUserSitdown(wChairID, pIServerUserItem))
	{
		return false;
	}		

	//请求事件
	if (m_pITableUserRequest && !m_pITableUserRequest->OnTableUserReqSitDown(pIServerUserItem, false))
	{
		return false;
	}

	//旁观处理
	if (cbLastStatus == US_LOOKON)
	{
		//起立处理
		for (INT_PTR i = 0; i < m_LookonUserItemArray.GetCount(); i++)
		{
			if (pIServerUserItem == m_LookonUserItemArray[i])
			{
				//删除子项
				m_LookonUserItemArray.RemoveAt(i);

				//事件通知
				if (m_pITableUserAction != NULL) m_pITableUserAction->OnActionUserStandUp(pIServerUserItem, true);

				break;
			}
		}
	}
	
	//设置变量
	m_dwLastEnterTime=(DWORD)time(NULL);
	m_TableUserItemArray[wChairID]=pIServerUserItem;

	//用户状态	
	if (m_bTableStarted==true && m_bGameStarted && /*m_GameStartMode!=StartMode_TimeControl &&*/ m_pGameServiceAttrib->wChairCount==MAX_CHAIR)
	{
		//设置状态
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,wChairID);

		//查找玩家
		for (WORD i = 0; i < m_wChairCount; i++)
		{
			if (m_dwPlayerID[i] == pIServerUserItem->GetUserID())
			{
				m_dwPlayerID[i] = 0;
				break;
			}
		}

		//设置标识
		m_dwPlayerID[wChairID]=pIServerUserItem->GetUserID();
	}
	else
	{
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH) == 0)
		{
			//设置状态
			pIServerUserItem->SetClientReady(false);			
			pIServerUserItem->SetUserStatus(US_SIT, m_wTableID, wChairID);			
		}
		else
		{
			//准备状态
			pIServerUserItem->SetClientReady(pIServerUserItem->IsTrusteeUser());
			pIServerUserItem->SetUserStatus(US_READY, m_wTableID, wChairID);
		}		
	}

	//桌子信息
	if (GetSitUserCount()==1)
	{
		//状态变量
		bool bTableLocked=IsTableLocked();

		//设置桌主
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE)==0)
		{
			//设置变量
			m_dwTableOwnerID=pIServerUserItem->GetUserID();			
			StringCchCopy(m_szTablePassword,CountArray(m_szTablePassword), pUserRule->szPassword);
		}		

		if (bTableLocked!=IsTableLocked()) 
		{
			//桌子事件
			if( m_pITableEventSink )
				m_pITableEventSink->OnEventTableLocked(QUERY_ME_INTERFACE(ITableFrameItem),IsTableLocked()?TRUE:FALSE);

			//发送状态
			SendTableStatus();
		}
	}

	//开始模式判断
	if (((m_pGameServiceOption->wServerType&(GAME_GENRE_MATCH|GAME_GENRE_BATTLE))==0) && (m_GameStartMode==StartMode_AllReady || m_GameStartMode==StartMode_FullReady))
	{
		//记录时间
		m_dwUserUnReadyTime[wChairID]=(DWORD)time(NULL);		
		if (GetUnReadyUserCount()==1) 
		{
			//获取时间
			DWORD dwElapse=TIME_KICKOUT_PLAYER;
			if (m_pIEventCustomFrameTime!=NULL)
			{
				dwElapse=m_pIEventCustomFrameTime->GetUnReadyKickOutTime();
			}

			//设置时钟
			SetGameTimer(IDI_KICKOUT_PLAYER,dwElapse*1000L,1,wChairID);
		}
	}		

	//坐下通知
	if (m_pITableUserAction != NULL)
	{
		//坐下失败
		if (m_pITableUserAction->OnActionUserSitDown(pIServerUserItem, false)==false)
		{
			PerformStandUpAction(pIServerUserItem,false);

			return false;
		}
	}

	//机器用户
	if (pIServerUserItem->IsRobotUser() == true)
	{
		//查找机器
		auto pRobotUserItem = (CRobotUserItem*)m_pIRobotUserManager->SearchRobotUserItem(pIServerUserItem->GetUserID());
		if (pRobotUserItem != NULL)
		{
			//绑定接口
			pRobotUserItem->SetTableFrameItem(this);
		}
	}

	//开始模式判断
	if (m_GameStartMode == StartMode_MasterControl && m_pIEventStartEfficacy!=NULL)
	{
		//管理员校验
		if (m_pIEventStartEfficacy->GetMasterChairID() == wChairID)
		{
			//构造结构
			CMD_GF_StartStatus StartStatus;
			ZeroMemory(&StartStatus, sizeof(StartStatus));

			//设置状态
			StartStatus.bEnabled = false;
			StartStatus.bVisibled = true;

			//发送数据
			m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_FRAME, SUB_GF_START_STATUS, &StartStatus, sizeof(StartStatus));
		}
	}	

	//坐下事件
	OnActionUserSitdown(pIServerUserItem);	

	return true;
}

//重连动作
bool CTableFrameItem::PerformConnectAction(IServerUserItem *pIServerUserItem)
{
	//效验参数
	ASSERT(pIServerUserItem!=NULL);
	WORD wTableID = pIServerUserItem->GetTableID();
	WORD wChairID = pIServerUserItem->GetChairID();
	ASSERT(wChairID<m_wChairCount&&wTableID==m_wTableID);

	//变量定义
	IServerUserItem *pTableUserItem = GetTableUserItem(wChairID);
	ASSERT(pTableUserItem==pIServerUserItem&&pIServerUserItem->GetUserStatus()==US_OFFLINE);

	//请求事件
	if (m_pITableUserRequest && !m_pITableUserRequest->OnTableUserReqConnect(pIServerUserItem))
	{
		return false;
	}

	//事件通知
	if (m_pITableUserAction!=NULL)
	{
		m_pITableUserAction->OnActionUserConnect(pIServerUserItem);
	}

	return true;
}

//桌子状态
bool CTableFrameItem::SendTableStatus()
{
	//构造变量
	CMD_GR_TableStatus TableStatus;
	TableStatus.wTableID=m_wTableID;
	TableStatus.TableStatus.cbTableLock=IsTableLocked()?TRUE:FALSE;
	TableStatus.TableStatus.cbPlayStatus=IsTableStarted()?TRUE:FALSE;

	//电脑数据
	m_pIServiceFunction->SendBatchData(BG_COMPUTER,MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	//手机数据
	m_pIServiceFunction->SendDataToTable(m_wTableID, CLIENT_KIND_MOBILE, MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	return true;
}

//开始状态
bool CTableFrameItem::SendStartStatus()
{
	//验证接口
	ASSERT (m_pIEventStartEfficacy != NULL);
	if (m_pIEventStartEfficacy == NULL) return false;

	//校验状态
	ASSERT (m_cbGameStatus == GAME_STATUS_FREE);
	if (m_cbGameStatus != GAME_STATUS_FREE) return false;

	//变量定义
	WORD wMasterUserID = 0;
	bool bStartGame = EfficacyStartGame(INVALID_CHAIR);

	//获取管理员
	WORD wMasterChairID = m_pIEventStartEfficacy->GetMasterChairID();
	if (wMasterChairID != INVALID_CHAIR)
	{
		//获取用户信息
		IServerUserItem * pIMasterUserItem = GetTableUserItem(wMasterChairID);
		if (pIMasterUserItem != NULL)
		{
			//设置标识
			wMasterUserID = pIMasterUserItem->GetUserID();
			
			//构造结构
			CMD_GF_StartStatus StartStatus;
			ZeroMemory(&StartStatus, sizeof(StartStatus));

			//设置状态
			StartStatus.bVisibled = true;
			StartStatus.bEnabled = bStartGame;

			//发送数据
			m_pIServiceFunction->SendDataToUser(pIMasterUserItem, MDM_GF_FRAME, SUB_GF_START_STATUS, &StartStatus, sizeof(StartStatus));
		}
	}	

	//查找房主
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//获取用户
		IServerUserItem * pITableUserItem = GetTableUserItem(i);
		if (pITableUserItem == NULL) continue;
		if (pITableUserItem->GetUserID() == wMasterUserID) continue;

		//房主、老板、管理员
		if (pITableUserItem->GetUserID() == m_dwTableOwnerID)
		{
			//构造结构
			CMD_GF_StartStatus StartStatus;
			ZeroMemory(&StartStatus, sizeof(StartStatus));

			//设置状态
			StartStatus.bVisibled = true;
			StartStatus.bEnabled = bStartGame;

			//发送数据
			m_pIServiceFunction->SendDataToUser(pITableUserItem, MDM_GF_FRAME, SUB_GF_START_STATUS, &StartStatus, sizeof(StartStatus));
		}
	}	

	return true;
}

//请求失败
bool CTableFrameItem::SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, BYTE cbFailureCode)
{
	//构造数据
	CMD_GR_RequestFailure RequestFailure;
	RequestFailure.cbFailureCode=cbFailureCode;
	StringCchCopy(RequestFailure.szDescribeString,CountArray(RequestFailure.szDescribeString), pszDescribe);

	//发送数据
	WORD wHeadSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	WORD wSendSize=wHeadSize+CountStringBuffer(RequestFailure.szDescribeString);
	m_pIServiceFunction->SendDataToUser(pIServerUserItem,MDM_GR_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wSendSize);

	return true;
}

//版本检查
bool CTableFrameItem::PerformCheckVersion(DWORD dwFrameVersion, DWORD dwClientVersion, IServerUserItem * pIServerUserItem)
{
	//变量定义
	bool bMustUpdatePlaza=false;
	bool bMustUpdateClient=false;

	//框架版本
	if (dwFrameVersion!=0)
	{
		if (GetMainVer(dwFrameVersion)!=GetMainVer(VERSION_FRAME)) bMustUpdatePlaza=true;
		if (GetProductVer(dwFrameVersion)!=GetProductVer(VERSION_FRAME)) bMustUpdatePlaza=true;
	}

	//游戏版本
	if (GetMainVer(dwClientVersion)!=GetMainVer(m_pGameServiceAttrib->dwClientVersion)) bMustUpdateClient=true;
	if (GetProductVer(dwClientVersion)!=GetProductVer(m_pGameServiceAttrib->dwClientVersion)) bMustUpdateClient=true;

	//更新通知
	if ((bMustUpdatePlaza==true)||(bMustUpdateClient==true))
	{
		//变量定义
		CMD_GR_UpdateNotify UpdateNotify;
		ZeroMemory(&UpdateNotify,sizeof(UpdateNotify));

		//变量定义
		UpdateNotify.cbAdviceUpdateClient=FALSE;
		UpdateNotify.cbMustUpdatePlaza=bMustUpdatePlaza;
		UpdateNotify.cbMustUpdateClient=bMustUpdateClient;

		//当前版本
		UpdateNotify.dwCurrentPlazaVersion=VERSION_PLAZA;
		UpdateNotify.dwCurrentFrameVersion=VERSION_FRAME;
		UpdateNotify.dwCurrentClientVersion=m_pGameServiceAttrib->dwClientVersion;

		//发送消息
		m_pIServiceFunction->SendDataToUser(pIServerUserItem,MDM_GR_LOGON,SUB_GR_UPDATE_NOTIFY,&UpdateNotify,sizeof(UpdateNotify));

		return false;
	}

	return true;
}

//开始效验
bool CTableFrameItem::EfficacyStartGame(WORD wReadyChairID)
{
	//状态判断
	if (m_bGameStarted==true) return false;

	//模式过滤
	if (m_GameStartMode==StartMode_TimeControl) return false;	

	//准备人数
	WORD wReadyUserCount=0;
	for (WORD i=0;i<m_wChairCount;i++) 
	{
		//获取用户
		IServerUserItem * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem==NULL) continue;

		//用户统计
		if (pITableUserItem!=NULL)
		{
			//状态判断
			if (pITableUserItem->IsClientReady() == false)
			{				
				return false;
			}
			if ((wReadyChairID!=i)&&(pITableUserItem->GetUserStatus()!=US_READY)) 
			{
				//准备验证
				if (m_pIEventStartEfficacy!=NULL)
				{
					if (m_pIEventStartEfficacy->IsAllowPlayerReady(i)==false)
					{
						continue;
					}
				}

				return false;
			}

			//用户计数
			wReadyUserCount++;
		}
	}

	//开始处理
	switch (m_GameStartMode)
	{
	case StartMode_AllReady:		//所有准备
		{
			//变量定义
			WORD wStartCount=2;

			//获取人数
			if (m_pIEventStartEfficacy!=NULL)
			{
				wStartCount=__max(m_pIEventStartEfficacy->GetStartPlayerCount(),2);
			}

			return wReadyUserCount>=wStartCount;
		}
	case StartMode_Symmetry:		//对称开始
		{
			//数目判断
			if (wReadyUserCount==m_wChairCount) return true;
			if ((wReadyUserCount<2L)||(wReadyUserCount%2)!=0) return false;

			//位置判断
			for (WORD i=0;i<m_wChairCount/2;i++)
			{
				//获取用户
				IServerUserItem * pICurrentUserItem=GetTableUserItem(i);
				IServerUserItem * pITowardsUserItem=GetTableUserItem(i+m_wChairCount/2);

				//位置过滤
				if ((pICurrentUserItem==NULL)&&(pITowardsUserItem!=NULL)) return false;
				if ((pICurrentUserItem!=NULL)&&(pITowardsUserItem==NULL)) return false;
			}

			return true;
		}
	case StartMode_FullReady:		//满人开始
		{
			//变量定义
			WORD wStartCount=m_wChairCount;

			//获取人数
			if (m_pIEventStartEfficacy!=NULL)
			{
				wStartCount=m_pIEventStartEfficacy->GetStartPlayerCount();
			}

			return wReadyUserCount==wStartCount;
		}
	case StartMode_MasterControl:	//管理控制
		{
			//变量定义
			WORD wStartCount = 2;

			//获取人数
			if (m_pIEventStartEfficacy != NULL)
			{
				wStartCount = __max(m_pIEventStartEfficacy->GetStartPlayerCount(), 2);
			}

			return wReadyUserCount >= wStartCount;
		}
	default:
		{
			ASSERT(FALSE);
			return false;
		}
	}

	return false;
}

//地址效验
bool CTableFrameItem::EfficacyAddrRule(IServerUserItem * pIServerUserItem)
{
	//规则效验
	if (CServerRule::IsForbidIPLimit(m_pGameServiceOption->dwServerRule)) return true;

	//变量定义
	DWORD dwUserIP=pIServerUserItem->GetClientIP();

	//地址效验
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		IServerUserItem * pITableUserItem=GetTableUserItem(i);

		if ((pITableUserItem!=NULL)&&(pITableUserItem->GetClientIP()==dwUserIP))
		{
			if(pIServerUserItem->GetUserRule()->bLimitSameIP)
			{
				LPCTSTR pszDescribe= CWHService::FormatMessage(MIM_SERVER_0094, L"您设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在与您 IP 地址相同的玩家，不能加入游戏！");
				SendRequestFailure(pIServerUserItem,pszDescribe,0);
				return false;
			}
			else if(pITableUserItem->GetUserRule()->bLimitSameIP)
			{
				LPCTSTR pszDescribe= CWHService::FormatMessage(MIM_SERVER_0094, L"此桌设置了不跟相同 IP 地址的玩家游戏，此游戏桌存在与您 IP 地址相同的玩家，不能加入游戏！") ;
				SendRequestFailure(pIServerUserItem,pszDescribe,0);
				return false;			
			}
		}
	}
	return true;
}

//积分效验
bool CTableFrameItem::EfficacyScoreRule(IServerUserItem * pIServerUserItem)
{
	//变量定义
	auto pUserInfo=pIServerUserItem->GetUserInfo();
	auto wFleeRate = pIServerUserItem->GetUserFleeRate();
	auto wAddupWinRate = pIServerUserItem->GetUserWinRate();

	//积分范围
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//获取用户
		auto pITableUserItem=GetTableUserItem(i);

		//规则效验
		if (pITableUserItem!=NULL)
		{
			//获取规则
			auto pTableUserRule=pITableUserItem->GetUserRule();

			//逃率效验
			if ((pTableUserRule->bLimitFleeRate)&&(wFleeRate>pTableUserRule->wMaxFleeRate) && !CServerRule::IsForbidFleeRateSet(m_pGameServiceOption->dwServerRule))
			{
				LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0095, L"您的逃跑率太高，与 %s 设置的设置不符，不能加入游戏！",pITableUserItem->GetAccounts());
				SendRequestFailure(pIServerUserItem, pszMessage,0);
				return false;
			}

			//胜率效验
			if ((pTableUserRule->bLimitWinRate)&&(wAddupWinRate<pTableUserRule->wLessWinRate) && !CServerRule::IsForbidWinRateSet(m_pGameServiceOption->dwServerRule))
			{
				LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0096, L"您的胜率太低，与 %s 设置的设置不符，不能加入游戏！",pITableUserItem->GetAccounts());
				SendRequestFailure(pIServerUserItem, pszMessage,0);
				return false;
			}

			//积分效验
			if (pTableUserRule->bLimitGameScore==true && !CServerRule::IsForbidScoreLimit(m_pGameServiceOption->dwServerRule))
			{
				//变量定义
				SCORE lMinGameScore=__min(pTableUserRule->lLessGameScore,pITableUserItem->GetUserScore());

				//最高积分
				if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0 && pUserInfo->lScore>pTableUserRule->lMaxGameScore)
				{
					LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0097, L"您的积分太高，与 %s 设置的设置不符，不能加入游戏！",pITableUserItem->GetAccounts());
					SendRequestFailure(pIServerUserItem, pszMessage,0);
					return false;
				}

				//最低积分
				if (pUserInfo->lScore<lMinGameScore)
				{
					LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0098, L"您的积分太低，与 %s 设置的设置不符，不能加入游戏！",pITableUserItem->GetAccounts());
					SendRequestFailure(pIServerUserItem, pszMessage,0);
					return false;
				}
			}
		}
	}

	return true;
}

//桌子效验
bool CTableFrameItem::EfficacyTableRule(IServerUserItem * pIServerUserItem)
{
	//变量定义
	auto pUserInfo=pIServerUserItem->GetUserInfo();
	auto pUserRule=pIServerUserItem->GetUserRule();

	//服务费用
	SCORE lServiceFee=0;
	if (m_pGameServiceOption->lLessScore!=0 && CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule))
	{
		lServiceFee=m_pGameServiceOption->lRevenue;
	}

	//最低分数
	SCORE lLessScore = m_pGameServiceOption->lLessScore;	

	//积分限制
	SCORE lStidownScore = __max(m_lBaseScore*m_wSitdownMultiple,lLessScore)+lServiceFee;
	if (pIServerUserItem->GetUserScore() < lStidownScore)
	{
		//金币提示
		if (pIServerUserItem->GetScoreKind()==SCORE_KIND_GOLD)
		{
			LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0100, L"加入游戏至少需要 " SCORE_STRING " 个游戏币，您的游戏币不足不能加入！", SCORE_ZOOMOUT(lStidownScore));
			SendRequestFailure(pIServerUserItem, pszMessage, RFC_SCORE_UNENOUGH);

			return false;
		}

		//茶社积分提示
		if (pIServerUserItem->GetScoreKind() == SCORE_KIND_CLUB)
		{
			LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0101, L"加入游戏至少需要 " SCORE_STRING " 个积分，您的积分不足不能加入！", SCORE_ZOOMOUT(lStidownScore));
			SendRequestFailure(pIServerUserItem, pszMessage, RFC_SCORE_UNENOUGH);

			return false;
		}
	}		

	//锁定限制
	if ((m_pGameServiceOption->lLessLockScore>0L)&&(pIServerUserItem->GetUserFreeScore()<lStidownScore-lServiceFee))
	{
		//金币提示
		if (pIServerUserItem->GetScoreKind() == SCORE_KIND_GOLD)
		{
			LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0100, L"加入游戏至少需要" SCORE_STRING "个游戏币，您的游戏币不够，不能加入！", SCORE_ZOOMOUT(lStidownScore));
			SendRequestFailure(pIServerUserItem, pszMessage,RFC_SCORE_UNENOUGH);
		}

		//积分提示
		if (pIServerUserItem->GetScoreKind() == SCORE_KIND_CLUB)
		{
			LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0101, L"加入游戏至少需要" SCORE_STRING "个积分，您的积分不够，不能加入！", SCORE_ZOOMOUT(lStidownScore));
			SendRequestFailure(pIServerUserItem, pszMessage, RFC_SCORE_UNENOUGH);
		}

		return false;
	}

	//密码效验
	if ( (IsTableLocked()==true) &&
		!CUserRight::CanEnterWithoutPasswd(pIServerUserItem->GetUserRight()) &&
		pIServerUserItem->GetMasterOrder() == 0 && !CServerRule::IsForbidPasswordSet(m_pGameServiceOption->dwServerRule))
	{
		if( pIServerUserItem->IsTmpPasswdEmpty() == false )
		{
			if( !pIServerUserItem->ContrastTempPassword(m_szTablePassword) )
			{
				//清理临时密码
				pIServerUserItem->SetTmpPassword(L"");

				SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0090, L"桌子密码错误，不能加入游戏！"),RFC_TMP_PASSWD_INCORRECT);
				return false;
			}

			//清理临时密码
			pIServerUserItem->SetTmpPassword(L"");
		}
		else if( !pIServerUserItem->ContrastTablePassword(m_szTablePassword) )
		{
			SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0090, L"桌子密码错误，不能加入游戏！"),RFC_PASSWORD_INCORRECT);
			return false;
		}
	}	

	//人数限制
	if ((m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)==0 && GetSitUserCount()>=m_wMaxPlayerCount)
	{
		//发送消息
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessageV(MIM_SERVER_0103, L"本桌设置了只允许 %d 人游戏，您不能加入！", m_wMaxPlayerCount),RFC_NULL);

		return false;
	}

	return true;
}


//计算等级
VOID CTableFrameItem::CalcUserCardLevel()
{
	//设置变量
	m_bControlCard = false;
	ZeroMemory(&m_cbUserCardLevel, sizeof(m_cbUserCardLevel));

	//变量定义
	WORD wItemCount = 0;
	WORD wUserCount[3] = { 0,0,0 };
	tagUserRateItem* pUserRateItem = NULL;
	IServerUserItem* pIServerUserItem = NULL;
	tagUserRateItem UserRateItem[TABLE_USER_COUNT] = {};

	//遍历用户
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//获取用户
		pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;

		//获取对象
		pUserRateItem = &UserRateItem[wItemCount++];

		//设置变量
		pUserRateItem->wChairID = pIServerUserItem->GetChairID();
		pUserRateItem->wAddupWinRate = pIServerUserItem->GetAddupWinRate();
		pUserRateItem->cbControlStatus = pIServerUserItem->GetRegulateStatus(0);

		//统计数量
		wUserCount[pUserRateItem->cbControlStatus]++;
		if (wItemCount >= CountArray(UserRateItem)) break;
	}

	//控制校验
	if (wUserCount[CONTROL_STATUS_WHITELIST] != 0 || wUserCount[CONTROL_STATUS_BLACKLIST] != 0)
	{
		//状态权重
		WORD wStatusWight[] = { 2,3,1 };

		//快速排序
		for (int i = 1; i < wItemCount; i++)
		{
			//变量定义
			int nLeft = 0, nRight = i - 1;
			tagUserRateItem TempUserRateItem = UserRateItem[i];

			while (nLeft <= nRight)
			{
				int nMid = (nLeft + nRight) / 2;

				//状态权重
				if (wStatusWight[UserRateItem[i].cbControlStatus] > wStatusWight[UserRateItem[nMid].cbControlStatus])
				{
					nRight = nMid - 1;
				}
				else if (UserRateItem[i].cbControlStatus == UserRateItem[nMid].cbControlStatus)
				{
					//比较胜率
					if (UserRateItem[i].wAddupWinRate < UserRateItem[nMid].wAddupWinRate)
					{
						nRight = nMid - 1;
						continue;
					}

					nLeft = nMid + 1;
				}
				else
				{
					nLeft = nMid + 1;
				}
			}

			for (INT_PTR j = i - 1; j >= nLeft; j--)
			{
				UserRateItem[j + 1] = UserRateItem[j];
			}

			UserRateItem[nLeft] = TempUserRateItem;
		}

		//设置标识
		m_bControlCard = true;

		//设置级别
		for (int i = 0; i < wItemCount; i++)  m_cbUserCardLevel[UserRateItem[i].wChairID] = i + 1;
	}
}

//游戏记录标识
SCORE CTableFrameItem::BuildGameRecordID()
{
	//静态变量
	static WORD wRecordCount = 0;
	static DWORD dwLastSecond = (DWORD)time(NULL);

	//变量定义
	DWORD dwSecond = (DWORD)time(NULL);

	//更新变量
	if (dwLastSecond != dwSecond)
	{
		wRecordCount = 0;
		dwLastSecond = dwSecond;
	}

	//格式化标识
	TCHAR szRecordID[19];
	_sntprintf_s(szRecordID, CountArray(szRecordID), TEXT("%05d%10d%02d"), m_pGameServiceOption->wServerID, dwSecond, ++wRecordCount);

	//数据转换
	SCORE lRecordID = _tstoi64(szRecordID);

	return lRecordID;
}

//录像编号
VOID CTableFrameItem::BuildVideoChannel(TCHAR szBuffer[], WORD wBufferLen)
{
	//获取时间
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime); 

	//单位换算
	DWORD dwSecond=SystemTime.wHour*3600+SystemTime.wMinute*60+SystemTime.wSecond;

	//静态变量
	static WORD wVideoCount=0;
	static DWORD dwLastSecond=dwSecond;	

	//更新变量
	if (dwSecond!=dwLastSecond)
	{
		wVideoCount=0;
		dwLastSecond=dwSecond;
	}

	//房间标识
	WORD wServerID=m_pGameServiceOption->wServerID;

	//格式化编号
	_snwprintf_s(szBuffer,wBufferLen, wBufferLen,TEXT("%05d%04d%02d%02d%05d%03d"),wServerID,SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,dwSecond,++wVideoCount);
}

//配牌器
bool CTableFrameItem::ControlSendCard(IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize)
{
	//扩展效验
	if (m_pITableControlSendCard != NULL)
	{
		return m_pITableControlSendCard->OnEventControlSendCard(pIServerUserItem, pData, wDataSize);
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
