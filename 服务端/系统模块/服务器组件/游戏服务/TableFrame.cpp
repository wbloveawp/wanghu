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
//��������
#define MAX_OFF_LINE				3									//���ߴ���
#define MIN_GPS_DISTANCE			100									//GPS����

//ʱ�䶨��
#define TIME_OFF_LINE				30L									//����ʱ��
#define TIME_KICKOUT_PLAYER			600L								//�߳�ʱ��
#define TIME_KICKOUT_LOOKON			10L									//�߳��Թ�

#ifdef _DEBUG
#define TIME_BATTLE_TABLE_DISMISS	 600								//��ɢʱ��
#define TIME_BATTLE_AGREEMENT_DISMISS 60								//Э���ɢ

#define TIME_RANKING_TABLE_DISMISS	 300								//��ɢʱ��

#else
#define TIME_BATTLE_TABLE_DISMISS	  10								//��ɢʱ��
#define TIME_BATTLE_AGREEMENT_DISMISS 60*2								//Э���ɢ

#define TIME_RANKING_TABLE_DISMISS	 300								//��ɢʱ��
#endif

//ʱ�䶨��
#define ALLOW_LOOKON_TIME			300L								//�����Թ�ʱ��

//////////////////////////////////////////////////////////////////////////

//����ṹ
struct tagUserRateItem
{
	WORD wChairID;
	WORD wAddupWinRate;
	BYTE cbControlStatus;
};

//////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrameItem::CTableFrameItem()
{
	//��������
	m_wTableID=0;
	m_wChairCount=0;
	m_GameStartMode=StartMode_FullReady;

	//��������
	m_dwLastEnterTime=0L;
	m_dwLastLeaveTime=0L;

	//���ñ���
	m_wMaxPlayerCount=0;
	m_wSitdownMultiple = 0;
	m_wStandupMultiple = 0;

	//��־����
	m_bVideoData=false;
	m_bWriteScore=false;
	m_bGamePaused=false;
	m_bGameStarted=false;
	m_bTableStarted=false;
	ZeroMemory(m_bAllowLookon,sizeof(m_bAllowLookon));

	//���ܱ�ʶ
	m_bSameIpRemind=false;

	//��Ϸ����
	m_lBaseScore=0;
	m_dwPlayCount=0;
	m_dwGameStartTime=0L;
	m_cbGameStatus=GAME_STATUS_FREE;
	ZeroMemory(m_dwPlayerID,sizeof(m_dwPlayerID));

	//������Ϣ
	m_dwTableOwnerID=0L;	
	m_szTablePassword[0]=0;
	m_lMinEnterScore = 0;//kk jia 
	
	//ƽ�����
	m_bControlCard=false;
	ZeroMemory(m_cbUserCardLevel,sizeof(m_cbUserCardLevel));

	//���ּ�¼
	m_dwPlayTimeCount=0L;
	ZeroMemory(m_ScoreInfoArray,sizeof(m_ScoreInfoArray));

	//����Ѽ�¼
	memset(m_bCostGame,true,sizeof(m_bCostGame));

	//���߱���
	ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
	ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));
	ZeroMemory(m_OfflineScore,sizeof(m_OfflineScore));

	//�û�����
	ZeroMemory(m_TableUserItemArray,sizeof(m_TableUserItemArray));

	//���˱���
	ZeroMemory(m_dwUserUnReadyTime,sizeof(m_dwUserUnReadyTime));

	//�ں����
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pIAssistDataBaseEngine=NULL;

	//����ӿ�
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

	//¼��ӿ�
	m_pIGameVideoItem=NULL;
	m_pIGameVideoManager=NULL;

	//������Ϣ	
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	return;
}

//��������
CTableFrameItem::~CTableFrameItem()
{
	//�ͷŽӿ�
	SafeRelease(m_pITableFrameSink);	

	return;
}

//�ӿڲ�ѯ
VOID * CTableFrameItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameItem,Guid,dwQueryVer);		
	QUERYINTERFACE(IGameBalanceControl, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameItem,Guid,dwQueryVer);
	return NULL;
}

//��ͽ��� KK JIA
VOID CTableFrameItem::SetMinEnterScore(LONGLONG lMinEnterScore)
{
	//���õ׷�
	m_lMinEnterScore = lMinEnterScore;

}

//��Ԫ����
VOID CTableFrameItem::SetCellScore(LONG lCellScore)
{
	//���õ׷�
	m_lBaseScore=lCellScore;

	//���֪ͨ
	if (m_pIEventBaseScore!=NULL)
	{
		m_pIEventBaseScore->OnEventBaseScoreVariation(m_lBaseScore);
	}
}

//��λ����Ŀ
WORD CTableFrameItem::GetNullChairCount()
{
	//��������
	WORD wNullChairCount=0;

	//�����û�
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) 
		{
			wNullChairCount++;
		}
	}

	return wNullChairCount;
}

//��ʼ��Ϸ
bool CTableFrameItem::StartGame()
{
	//Ч��״̬
	ASSERT(m_bGameStarted==false);
	if (m_bGameStarted==true) return false;

	//���ñ���
	m_dwPlayCount++;
	m_bGameStarted=true;
	m_bGamePaused=false;
	m_dwGameStartTime=(DWORD)time(NULL);
	ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));

	//���ñ���
	ZeroMemory(m_dwUserUnReadyTime,sizeof(m_dwUserUnReadyTime));

	//�رն�ʱ��
	KillGameTimer(IDI_KICKOUT_PLAYER);	

	//������Ϣ
	ZeroMemory(m_dwPlayerID,sizeof(m_dwPlayerID));
	ZeroMemory(m_ScoreInfoArray,sizeof(m_ScoreInfoArray));

	//�����û�
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//������Ϣ
		m_dwPlayerID[i]=pIServerUserItem->GetUserID();

		//��ʼУ��
		if (m_pIEventStartEfficacy!=NULL)
		{
			if (m_pIEventStartEfficacy->IsAllowPlayerReady(i)==false)
			{
				continue;
			}
		}

		//�ı�״̬
		pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,i);	
	}

	//����״̬
	if (m_bTableStarted==false)
	{
		//���ñ���
		m_bTableStarted=true;
		
		//����״̬
		SendTableStatus();
	}

	//��Ӯ����
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0 && m_wChairCount!=MAX_CHAIR)
	{
		CalcUserCardLevel();
	}

	//�����ģʽ
	if (CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule))
	{
		//ѯ�����
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

	//¼��ӿ�
	if (m_pIGameVideoManager!=NULL && CServerRule::IsRecordGameVideo(m_pGameServiceOption->dwServerRule))
	{
		//��ʼ¼��
		m_pIGameVideoItem=m_pIGameVideoManager->ActiveVideoItem(m_wTableID);
		if (m_pIGameVideoItem!=NULL) m_pIGameVideoItem->StartVideo();

		//���ñ�־
		m_bVideoData=true;

		for (WORD i=0;i<m_wChairCount;i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUserItem=GetTableUserItem(i);
			if (pIServerUserItem==NULL) continue;

			//���ͳ���			
			m_pITableFrameSink->OnEventSendGameScene(i,pIServerUserItem,m_cbGameStatus,true);
		}

		//��ԭ��־
		m_bVideoData=false;
	}

	//֪ͨ�¼�
	ASSERT(m_pITableFrameSink != NULL);
	if (m_pITableFrameSink == NULL)
	{
		return true;
	}

	//��Ϸ��ʼ
	if (m_pITableFrameSink->OnEventGameStart() == false)
	{
		return true;
	}

	//֪ͨ�¼�
	m_pIServiceFunction->OnEventGameStart(this);
	
	//��Ϸ��ʼ
	OnEventGameStart();	

	return true;
}

//��ͣ��Ϸ
bool CTableFrameItem::PauseGame()
{
	//״̬�ж�
	ASSERT(m_bGamePaused == false);
	if (m_bGamePaused == true) return false;

	//��ͣ��Ϸ
	//if ((m_bGameStarted == true) && (m_pITableFrameSink->OnEventGamePause() == false))
	if (m_pITableFrameSink->OnEventGamePause() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//����״̬
	m_bGamePaused = true;

	//����״̬
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;

		//����״̬
		CMD_GF_GameStatus GameStatus;
		GameStatus.bGamePaused = m_bGamePaused;
		GameStatus.cbGameStatus = m_cbGameStatus;
		GameStatus.cbAllowLookon = m_bAllowLookon[i] ? TRUE : FALSE;
		m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_FRAME, SUB_GF_GAME_STATUS, &GameStatus, sizeof(GameStatus));
	}	

	return true;
}

//������Ϸ
bool CTableFrameItem::ContinueGame()
{
	//״̬�ж�
	ASSERT (m_bGamePaused == true);
	if (m_bGamePaused == false) return false;

	//������Ϸ
	if (m_pITableFrameSink->OnEventGameContinue() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	//����״̬
	m_bGamePaused = false;

	//����״̬
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;

		//����״̬
		CMD_GF_GameStatus GameStatus;
		GameStatus.bGamePaused = m_bGamePaused;
		GameStatus.cbGameStatus = m_cbGameStatus;
		GameStatus.cbAllowLookon = m_bAllowLookon[i] ? TRUE : FALSE;
		m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_FRAME, SUB_GF_GAME_STATUS, &GameStatus, sizeof(GameStatus));
	}

	return true;
}

//��ɢ��Ϸ
bool CTableFrameItem::DismissGame()
{
	//״̬�ж�
	ASSERT(m_bTableStarted==true);
	if (m_bTableStarted==false) return false;

	//����״̬
	if ((m_bGameStarted==false)&&(m_bTableStarted==true))
	{
		//���ñ���
		m_bTableStarted=false;

		//����״̬
		SendTableStatus();
	}

	//������Ϸ
	if ((m_bGameStarted==true)&&(m_pITableFrameSink->OnEventGameConclude(INVALID_CHAIR,NULL,GER_DISMISS)==false))
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//��¼����
bool CTableFrameItem::RecordGameScore(LPCTSTR pszGameCourse)
{
	//����ṹ
	DBR_GR_RecordDrawInfo RecordDrawInfo;
	ZeroMemory(&RecordDrawInfo, sizeof(RecordDrawInfo));

	//����ṹ
	DBR_GR_RecordDrawScore RecordDrawScore;
	ZeroMemory(&RecordDrawScore, sizeof(RecordDrawScore));

	//���ñ���
	RecordDrawInfo.wTableID = m_wTableID;
	RecordDrawInfo.lRecordID = BuildGameRecordID();
	RecordDrawInfo.dwPlayTimeCount = m_dwPlayTimeCount;
	RecordDrawInfo.cbScoreKind = SCORE_KIND_GOLD;

	//Լս��Ϣ
	RecordDrawInfo.lRelationRecordID = GetRelationParameter()->lRelationRecordID;

	//��ȡʱ��
	GetLocalTime(&RecordDrawInfo.RecordTime);

	//���ñ���
	RecordDrawScore.lRecordID = RecordDrawInfo.lRecordID;
	RecordDrawScore.lRelationRecordID = RecordDrawInfo.lRelationRecordID;

	//¼����	
	if (m_pIGameVideoItem != NULL)
	{
		StringCchCopy(RecordDrawInfo.szVideoNumber, CountArray(RecordDrawInfo.szVideoNumber), m_pIGameVideoItem->GetVideoNumber());
		StringCchCopy(RecordDrawScore.szVideoNumber, CountArray(RecordDrawScore.szVideoNumber), m_pIGameVideoItem->GetVideoNumber());
	}

	//��������
	SCORE lScore = 0;
	SCORE lRevenue = 0;
	tagScoreRecordItem* pScoreRecordItem = NULL;

	//�û�����
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//У���û�
		if (m_dwPlayerID[i] == 0) continue;

		//��������
		if (m_ScoreInfoArray[i].lScore == 0 &&
			m_ScoreInfoArray[i].lBetAmount == 0 && 
			m_ScoreInfoArray[i].ScoreType == ScoreType_Null && 
			(m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) == 0)
		{
			continue;
		}

		//�û�����
		RecordDrawInfo.wUserCount++;

		//��ȡ����
		pScoreRecordItem = &RecordDrawScore.ScoreRecordItem[RecordDrawScore.wUserCount++];

		//�û���Ϣ
		pScoreRecordItem->wChairID = i;
		pScoreRecordItem->dwUserID = m_dwPlayerID[i];

		//������Ϣ
		lRevenue += m_ScoreInfoArray[i].lRevenue;
		pScoreRecordItem->lScore = m_ScoreInfoArray[i].lScore;
		pScoreRecordItem->lRevenue = m_ScoreInfoArray[i].lRevenue;
		pScoreRecordItem->lBetAmount = m_ScoreInfoArray[i].lBetAmount;
		pScoreRecordItem->lBeforScore = m_ScoreInfoArray[i].lBeforScore;
		
		//��ѯ�û�
		IServerUserItem* pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem != NULL)
		{
			if (!pIServerUserItem->IsRobotUser())
			{
				RecordDrawInfo.lWasteCount -= pScoreRecordItem->lScore;
			}

			//��������
			RecordDrawInfo.cbScoreKind = pIServerUserItem->GetScoreKind();
		}
		else
		{
			RecordDrawInfo.lWasteCount -= pScoreRecordItem->lScore;
		}

		lScore += (long)abs((long)pScoreRecordItem->lScore);			

		//��������
		if (RecordDrawScore.wUserCount >= CountArray(RecordDrawScore.ScoreRecordItem))
		{
			//Ͷ������
			m_pIAssistDataBaseEngine->PostDataBaseRequest(DBR_GR_RECORD_DRAW_SCORE, 0, 0, 0, &RecordDrawScore, sizeof(RecordDrawScore));

			//��������
			RecordDrawScore.wUserCount = 0;
		}
	}

	//��������
	if (RecordDrawScore.wUserCount > 0)
	{
		//Ͷ������
		WORD wHeadSize = sizeof(RecordDrawScore) - sizeof(RecordDrawScore.ScoreRecordItem);
		WORD wDataSize = RecordDrawScore.wUserCount * sizeof(RecordDrawScore.ScoreRecordItem[0]);
		m_pIAssistDataBaseEngine->PostDataBaseRequest(DBR_GR_RECORD_DRAW_SCORE, 0, 0, 0, &RecordDrawScore, wHeadSize + wDataSize);
	}

	//У������
	if (RecordDrawInfo.wUserCount > 0)
	{
		//���ñ���
		RecordDrawInfo.lWasteCount -= lRevenue;
		RecordDrawInfo.lRevenueCount = lRevenue;

		//������
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

		//��Ϸ����
		if (pszGameCourse != NULL && lstrlen(pszGameCourse) > 0)
		{
			StringCchCopy(RecordDrawInfo.szGameSourse, CountArray(RecordDrawInfo.szGameSourse), pszGameCourse);
		}

		//Ͷ������
		WORD wDataSize = CountStringBuffer(RecordDrawInfo.szGameSourse);
		WORD wHeadSize = sizeof(RecordDrawInfo) - sizeof(RecordDrawInfo.szGameSourse);
		m_pIAssistDataBaseEngine->PostDataBaseRequest(DBR_GR_RECORD_DRAW_INFO, 0, 0, 0, &RecordDrawInfo, wHeadSize + wDataSize);
	}

	return true;
}

//��¼¼��
bool CTableFrameItem::RecordGameVideo()
{
	//����ṹ
	DBR_GR_RecordGameVideo RecordGameVideo;
	ZeroMemory(&RecordGameVideo, sizeof(RecordGameVideo));

	for (WORD i = 0; i < m_wChairCount; i++)
	{
		if (m_dwPlayerID[i] == 0) continue;
		if (RecordGameVideo.wUserCount >= CountArray(RecordGameVideo.dwUserID)) break;

		//���ñ���
		RecordGameVideo.dwUserID[RecordGameVideo.wUserCount++] = m_dwPlayerID[i];
	}

	//Լս��Ϣ
	RecordGameVideo.dwRelationNum = GetRelationParameter()->dwRelationNum;
	RecordGameVideo.lRelationRecordID = GetRelationParameter()->lRelationRecordID;

	//¼����Ϣ
	GetLocalTime(&RecordGameVideo.VideoCreateTime);
	StringCchCopy(RecordGameVideo.szVideoNumber, CountArray(RecordGameVideo.szVideoNumber), m_pIGameVideoItem->GetVideoNumber());

	//Ͷ������
	ASSERT(m_pIAssistDataBaseEngine != NULL);
	m_pIAssistDataBaseEngine->PostDataBaseRequest(DBR_GR_RECORD_GAME_VIDEO, 0, 0, 0, &RecordGameVideo, sizeof(RecordGameVideo));

	//����ṹ
	CMD_SS_C_StoreVideo* pStoreVideo = (CMD_SS_C_StoreVideo*)m_cbBuffer;
	ZeroMemory(pStoreVideo, sizeof(CMD_SS_C_StoreVideo));

	//���ñ���
	pStoreVideo->wKindID = m_pGameServiceOption->wKindID;
	pStoreVideo->wVideoSize = m_pIGameVideoItem->GetCompressSize();
	StringCchCopy(pStoreVideo->szVideoNumber, CountArray(pStoreVideo->szVideoNumber), m_pIGameVideoItem->GetVideoNumber());

	//��ȡʱ��
	pStoreVideo->VideoCreateTime = RecordGameVideo.VideoCreateTime;

	//��������
	CopyMemory(pStoreVideo + 1, m_pIGameVideoItem->GetCompressBuffer(), pStoreVideo->wVideoSize);

	//�洢����
	m_pIServiceFunction->StoreVideoData(m_cbBuffer, sizeof(CMD_SS_C_StoreVideo) + pStoreVideo->wVideoSize);

	return true;
}

//������Ϸ
bool CTableFrameItem::ConcludeGame(LPCTSTR pszGameCourse)
{
	//Ч��״̬
	ASSERT (m_bGameStarted==true);
	if (m_bGameStarted==false) return false;

	//��Ϸ�����
 	if( CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule) )
		CostForGameStart();

	//������Ϸ
	m_bGamePaused=false;
	m_bGameStarted=false;	
	m_cbGameStatus=GAME_STATUS_FREE;
	m_dwPlayTimeCount=(DWORD)time(NULL)-m_dwGameStartTime;

	//ֹͣ¼��
	if (m_pIGameVideoItem!=NULL) m_pIGameVideoItem->ConcludeVideo();

	//��Ϸ��¼
	if (CServerRule::IsRecordGameScore(m_pGameServiceOption->dwServerRule)==true)
	{
		RecordGameScore(pszGameCourse);
	}

	//¼���¼	
	if (m_pIGameVideoItem!=NULL) 
	{
		RecordGameVideo();			
	}	

	//�����û�
	for (WORD i=0;i<m_wChairCount;i++)
	{
		if (m_TableUserItemArray[i]!=NULL)
		{
			IServerUserItem * pIServerUserItem=m_TableUserItemArray[i];
			if (pIServerUserItem->GetUserStatus()!=US_OFFLINE) pIServerUserItem->SetUserStatus(US_SIT,m_wTableID,i);
		}
	}

	//��������
	bool bOffLineWait=false;

	//���ߴ���
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//���ߴ���
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

		//�й��û�
		if (pIServerUserItem->IsTrusteeUser()==true && (m_pGameServiceOption->wServerType&(GAME_GENRE_MATCH|GAME_GENRE_BATTLE|GAME_GENRE_RANKING))==0)
		{
			PerformStandUpAction(pIServerUserItem);

			continue;
		}
	}

	//ɾ��ʱ��	
	if (bOffLineWait==true) KillGameTimer(IDI_OFF_LINE);

	//����״̬
	if (m_bTableStarted==true&&CServerRule::IsTableEndImmediate(m_pGameServiceOption->dwServerRule))
	{
		//���ñ���
		m_bTableStarted=false;

		//����״̬
		if (m_pITableEventSink)
			m_pITableEventSink->OnEventTableStatus(QUERY_ME_INTERFACE(ITableFrameItem),FALSE);

		//����״̬
		SendTableStatus();
	}

	//���߱���
	ZeroMemory(m_wOffLineCount,sizeof(m_wOffLineCount));
	ZeroMemory(m_dwOffLineTime,sizeof(m_dwOffLineTime));

	//��Ϸ����
	m_bWriteScore=false;
	m_dwGameStartTime=0L;
	ZeroMemory(m_dwPlayerID,sizeof(m_dwPlayerID));

	//���ּ�¼
	m_dwPlayTimeCount=0L;
	ZeroMemory(m_ScoreInfoArray,sizeof(m_ScoreInfoArray));

	//֪ͨ����
	//if (m_pIMatchEventSink!=NULL) m_pIMatchEventSink->OnEventGameConclude(this, 0, NULL, m_cbGameStatus);

	//��������
	ASSERT(m_pITableFrameSink!=NULL);
	if (m_pITableFrameSink!=NULL) m_pITableFrameSink->ResetTableFrameSink();

	//֪ͨ�¼�
	m_pIServiceFunction->OnEventGameConclude(this);

	//����֪ͨ
	OnEventGameConclude();

	//������������
	for( INT_PTR i = 0; i < CountArray(m_OfflineScore); i++ )
	{
		if( m_OfflineScore[i].dwUserID == 0 ) continue;

		if( m_pITableEventSink && (m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE)))
			m_pITableEventSink->OnTableReqUnfreezeScore(m_OfflineScore[i].dwUserID,m_OfflineScore[i].lUnLockScore);

		m_OfflineScore[i].dwUserID = 0;
	}

	//�������
	SCORE lServiceFee = 0;
	if (m_pGameServiceOption->lLessScore != 0 && CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule))
	{
		lServiceFee = m_pGameServiceOption->lRevenue;
	}

	//�������
	SCORE lLessScore = m_pGameServiceOption->lLessScore;
	SCORE lStandupScore = __max(m_lBaseScore * m_wStandupMultiple, lLessScore) + lServiceFee;

	//����ͳ��
	WORD wStandupCount = 0;
	bool bAllowStandUp = true;	

	//��������
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//��ȡ�û� 
		if (m_TableUserItemArray[i] == NULL) continue;
		IServerUserItem * pIServerUserItem = m_TableUserItemArray[i];

		//��������
		bool bStandUp = false;
		LPCTSTR pszMessage = L"";

		//��������
		if (pIServerUserItem->GetUserScore() < lStandupScore)
		{
			//�������
			if (pIServerUserItem->GetScoreKind() == SCORE_KIND_GOLD && pIServerUserItem->IsLargessSubsidy() == false)
			{
				//���ñ�ʶ
				bStandUp = true;

				//������Ϣ			
				pszMessage = CWHService::FormatMessageV(MIM_SERVER_0058, L"������Ϸ������ " SCORE_STRING " �����޷�������Ϸ��", SCORE_ZOOMOUT(lStandupScore));

				//������Ϣ
				SendGameMessage(pIServerUserItem, pszMessage, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT | SMT_SHOW_MOBILE);				
			}

			//�����������
			if (pIServerUserItem->GetScoreKind() == SCORE_KIND_CLUB)
			{
				//���ñ�ʶ
				bStandUp = true;

				//������Ϣ			
				pszMessage = CWHService::FormatMessageV(MIM_SERVER_0057, L"���Ļ������� " SCORE_STRING " �����޷�������Ϸ��", SCORE_ZOOMOUT(lStandupScore));

				//������Ϣ
				SendGameMessage(pIServerUserItem, pszMessage, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT | SMT_SHOW_MOBILE);
			}

			//ִ������
			if (bStandUp == true)
			{
				//��������
				PerformStandUpAction(pIServerUserItem);

				//�������
				if (pIServerUserItem->IsRobotUser() == true)
				{
					m_pIRobotUserManager->PostponeRobotAction(pIServerUserItem->GetUserID(), ROBOT_ACTION_INSURE);
				}

				continue;
			}
		}		

		//��������
		if ((m_pGameServiceOption->lLessLockScore > 0L) && (pIServerUserItem->GetUserFreeScore() < (lStandupScore-lServiceFee)) && m_wChairCount != MAX_CHAIR)
		{
			//�������
			if (pIServerUserItem->GetScoreKind() == SCORE_KIND_GOLD && pIServerUserItem->IsLargessSubsidy() == false)
			{
				//���ñ�ʶ
				bStandUp = true;

				//������Ϣ			
				pszMessage = CWHService::FormatMessageV(MIM_SERVER_0058, L"������Ϸ������ " SCORE_STRING " �����޷�������Ϸ��", SCORE_ZOOMOUT(lStandupScore));

				//������Ϣ
				SendGameMessage(pIServerUserItem, pszMessage, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT | SMT_SHOW_MOBILE);				
			}

			//�����������
			if (pIServerUserItem->GetScoreKind() == SCORE_KIND_CLUB)
			{
				//���ñ�ʶ
				bStandUp = true;

				//������Ϣ			
				pszMessage = CWHService::FormatMessageV(MIM_SERVER_0057, L"���Ļ������� " SCORE_STRING " �����޷�������Ϸ��", SCORE_ZOOMOUT(lStandupScore));

				//������Ϣ
				SendGameMessage(pIServerUserItem, pszMessage, SMT_CHAT | SMT_CLOSE_GAME | SMT_EJECT | SMT_SHOW_MOBILE);
			}

			//ִ������
			if (bStandUp == true)
			{
				//ִ������
				PerformStandUpAction(pIServerUserItem);

				//�������
				if (pIServerUserItem->IsRobotUser() == true)
				{
					m_pIRobotUserManager->PostponeRobotAction(pIServerUserItem->GetUserID(), ROBOT_ACTION_INSURE);
				}				

				continue;
			}			
		}

		//�ر��ж�
		if (((g_GlobalUnits.IsAllowEnterGame() == false) || (g_GlobalUnits.IsAllowEnterRoom() == false)) && (pIServerUserItem->GetMasterOrder() == 0))
		{
			//������Ϣ
			SendGameMessage(pIServerUserItem, CWHService::FormatMessageV(MIM_SERVER_0060, L"��ע�⣬��Ϸ���伴���رջ��߲�������ҽ��룬�����뿪��Ϸ���ӣ�"), SMT_EJECT | SMT_CHAT | SMT_CLOSE_GAME);

			//�û�����
			PerformStandUpAction(pIServerUserItem);

			continue;
		}

		//�����ж�
		if (m_pGameServiceOption->lMaxEnterScore != 0L && pIServerUserItem->GetUserScore() > m_pGameServiceOption->lMaxEnterScore)
		{
			//�������
			if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0)
			{
				pszMessage = CWHService::FormatMessageV(MIM_SERVER_0062, L"���ã�������Ϸ���Ѿ����� " SCORE_STRING "����������ķ�����Ϸ��", SCORE_ZOOMOUT(m_pGameServiceOption->lMaxEnterScore));
				SendGameMessage(pIServerUserItem, pszMessage, SMT_EJECT | SMT_CHAT | SMT_CLOSE_GAME);
			}			

			//�����ж�
			PerformStandUpAction(pIServerUserItem);

			continue;
		}

		//��������
		if (pIServerUserItem->GetUserStatus() != US_OFFLINE && (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH) == 0)
		{
			//��������
			if (pIServerUserItem->IsRobotUser() == true)
			{
				//���һ���
				CRobotUserItem * pIRobotUserItem = (CRobotUserItem *)m_pIRobotUserManager->SearchRobotUserItem(pIServerUserItem->GetUserID());
				if (pIRobotUserItem == NULL) continue;

				//�뿪�ж�
				if (pIRobotUserItem->JudgeRobotAction(ROBOT_WAITLEAVE))
				{
					//��������
					PerformStandUpAction(pIServerUserItem);

					//��������
					m_pIRobotUserManager->PostponeRobotAction(pIServerUserItem->GetUserID(), ROBOT_ACTION_LEAVE);

					continue;
				}

				//�����ж�
				if (pIRobotUserItem->JudgeRobotAction(ROBOT_WAITSTANDUP))
				{
					//��������
					m_pIRobotUserManager->PostponeRobotAction(pIServerUserItem->GetUserID(), ROBOT_ACTION_STANDUP);

					continue;
				}
			}
		}
	}

	//�Ǳ�������
	if (((m_pGameServiceOption->wServerType&(GAME_GENRE_MATCH|GAME_GENRE_BATTLE))==0) && (m_GameStartMode==StartMode_AllReady || m_GameStartMode==StartMode_FullReady))
	{
		//��������
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

		//���ö�ʱ��
		if (wFirstChairID!=INVALID_CHAIR)
		{
			//��ȡʱ��
			DWORD dwElapse=TIME_KICKOUT_PLAYER;
			if (m_pIEventCustomFrameTime!=NULL)
			{
				dwElapse=m_pIEventCustomFrameTime->GetUnReadyKickOutTime();
			}

			//����ʱ��
			SetGameTimer(IDI_KICKOUT_PLAYER,dwElapse*1000L,1,wFirstChairID);
		}
	}	

	//ɾ��¼��
	if (m_pIGameVideoManager!=NULL)
	{
		m_pIGameVideoManager->DeleteVideoItem(m_pIGameVideoItem);
		m_pIGameVideoItem=NULL;
	}	

	return true;
}

//����˰��
SCORE CTableFrameItem::AccountRevenue(SCORE lGameScore)
{
	if (lGameScore <= 0L) return 0L;

	//���˰��
	if ((CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule)==false) &&
		(m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)!=0)
	{
		return m_pGameServiceOption->lRevenue*lGameScore/1000L;
	}	

	return 0L;
}

//д�����
bool CTableFrameItem::WriteUserScore(WORD wChairID, tagScoreInfo& ScoreInfo)
{
	//Ч�����
	ASSERT((wChairID< m_wMaxPlayerCount)/*&&(lScore<=0L)*/);
	if ((wChairID>= m_wMaxPlayerCount)/*||(lScore>0L)*/) return false;

	//��ȡ�û�
	IServerUserItem * pIServerUserItem=GetTableUserItem(wChairID);	

	//д�����
	if (pIServerUserItem!=NULL)
	{
		//У������
		if (((m_pGameServiceOption->wServerType & (GAME_GENRE_GOLD| GAME_GENRE_MATCH |GAME_GENRE_BATTLE)) != 0) && ScoreInfo.lScore<0)
		{
			//�������
			if (-ScoreInfo.lScore>pIServerUserItem->GetUserFreeScore())
			{
				ASSERT(FALSE);
				return false;
			}
		}
		CString s;
		s.Format(_T("shui1: i:%d, m_ScoreInfoArray[wChairID].lScore:%I64d , lRevenue:%I64d "), wChairID, m_ScoreInfoArray[wChairID].lScore, ScoreInfo.lRevenue);
		CTraceService::TraceString(s, TraceLevel_Debug);
		//���ּ�¼
		m_ScoreInfoArray[wChairID].lScore+= ScoreInfo.lScore;
		m_ScoreInfoArray[wChairID].lRevenue += ScoreInfo.lRevenue;
		m_ScoreInfoArray[wChairID].ScoreType = ScoreInfo.ScoreType;	
		m_ScoreInfoArray[wChairID].lBetAmount += ScoreInfo.lBetAmount;		

		//�û�����
		if (!CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule))
		{
			m_ScoreInfoArray[wChairID].lBeforScore = pIServerUserItem->GetUserScore();
		}

		//�û�����
		pIServerUserItem->WriteUserScore(ScoreInfo.lScore, ScoreInfo.lRevenue, ScoreInfo.lBetAmount, ScoreInfo.lExperience, ScoreInfo.ScoreType);

		//�û�״̬
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

//д�����
bool CTableFrameItem::WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount)
{
	//CTraceService::TraceString(_T("WriteTableScore_3"), TraceLevel_Exception);
	//Ч�����
	ASSERT (wScoreCount== m_wMaxPlayerCount);
	if (wScoreCount!= m_wMaxPlayerCount) return false;

	//CTraceService::TraceString(_T("WriteTableScore_4"), TraceLevel_Exception);
	//Ч�����
	if (m_wMaxPlayerCount != MAX_CHAIR)
	{
		//��������
		SCORE lGameScore=0;

		//����ͳ��
		for (WORD i=0;i< m_wMaxPlayerCount;i++)
		{
			if (ScoreInfoArray[i].ScoreType!=ScoreType_Null)
			{
				//�û�Ч��
				if (m_dwPlayerID[i]==0L)
				{
					ASSERT(FALSE);
					return false;
				}

				//�����ۼ�
				lGameScore+=ScoreInfoArray[i].lScore;
				lGameScore+=m_ScoreInfoArray[i].lScore;
			}
			else
			{
				//�����ۼ�
				ASSERT(m_ScoreInfoArray[i].lScore<=0L);
				lGameScore+=m_ScoreInfoArray[i].lScore;
			}

			//��ȡ�û�
			IServerUserItem * pIServerUserItem = GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;

			//У�����
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
			//�ܺ�Ч��
			if (lGameScore > 0L)
			{
				ASSERT(FALSE);
				return false;
			}
		}
	}

	//CTraceService::TraceString(_T("WriteTableScore_5"), TraceLevel_Exception);

	//д�����
	for (WORD i=0;i< m_wMaxPlayerCount;i++)
	{
		//д���ж�
		if (m_dwPlayerID[i]==0L) continue;
		if (ScoreInfoArray[i].ScoreType==ScoreType_Null && (m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE)==0) continue;

		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		
		//д�����
		if (pIServerUserItem!=NULL)
		{
			//�û�����
			SCORE lScore = ScoreInfoArray[i].lScore;
			SCORE lRevenue = ScoreInfoArray[i].lRevenue;
			LONG lBetAmount = ScoreInfoArray[i].lBetAmount;
			LONG lExperience = ScoreInfoArray[i].lExperience;
			enScoreType ScoreType = ScoreInfoArray[i].ScoreType;

			////����˰��
			//SCORE lRevenue=AccountRevenue(lScore);
			//SCORE lUserScore = ScoreInfoArray[i].lScore-lRevenue;

			//���ּ�¼
			/*CString s;
			s.Format(_T("shui2: i:%d, m_ScoreInfoArray[i].lScore:%I64d ,lScore:%I64d, lRevenue:%I64d "), i, m_ScoreInfoArray[i].lScore, lScore,lRevenue);
			CTraceService::TraceString(s, TraceLevel_Debug);*/

			m_ScoreInfoArray[i].lScore += lScore;
			m_ScoreInfoArray[i].lRevenue += lRevenue;						
			m_ScoreInfoArray[i].ScoreType = ScoreType;						
			m_ScoreInfoArray[i].lBetAmount += lBetAmount;

			

			//�û�����
			if (!CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule))
			{
				m_ScoreInfoArray[i].lBeforScore = pIServerUserItem->GetUserScore();
			}

			/*CString s;
			s.Format(_T("WriteTableScore_6,lScore:%I64d, lRevenue:%I64d, lBetAmount:%d, lExperience:%d, ScoreType:%d"), lScore, lRevenue, lBetAmount, lExperience, ScoreType);
			CTraceService::TraceString(s, TraceLevel_Exception);*/
			//�û�д��
			//pIServerUserItem->WriteUserScore(lScore, lRevenue, lBetAmount, lExperience, ScoreType);//kk guan
			pIServerUserItem->WriteUserScore(lScore - lRevenue, lRevenue, lBetAmount, lExperience, ScoreType);//kk jia
		}
		else
		{
			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("ִ��������д�ֲ���"),TraceLevel_Debug);

			//����д��
			ASSERT (m_OfflineScore[i].dwUserID!=0);
			if (m_OfflineScore[i].dwUserID==0) continue;
			if (m_pGameServiceOption->wServerType&(GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
			{
				ASSERT (m_OfflineScore[i].lUnLockScore>=ScoreInfoArray[i].lScore);
				if (m_OfflineScore[i].lUnLockScore<ScoreInfoArray[i].lScore) continue;
			}
			
			//����˰��
			SCORE lRevenue=AccountRevenue(ScoreInfoArray[i].lScore);			

			CString s;
			s.Format(_T("shui4: i:%d, m_ScoreInfoArray[i].lScore:%I64d , lRevenue:%I64d "), i, m_ScoreInfoArray[i].lScore, lRevenue);
			CTraceService::TraceString(s, TraceLevel_Debug);

			//���ּ�¼
			m_ScoreInfoArray[i].lRevenue += lRevenue;
			m_ScoreInfoArray[i].ScoreType=ScoreInfoArray[i].ScoreType;
			m_ScoreInfoArray[i].lScore+=ScoreInfoArray[i].lScore-lRevenue;
			m_ScoreInfoArray[i].lBetAmount += ScoreInfoArray[i].lBetAmount;
			
			m_OfflineScore[i].lRevenue = lRevenue;
			m_OfflineScore[i].lScore = ScoreInfoArray[i].lScore-lRevenue;
			m_OfflineScore[i].ScoreType = ScoreInfoArray[i].ScoreType;
			m_OfflineScore[i].lBetAmount = ScoreInfoArray[i].lBetAmount;
						
			//�޸�ʤ��
			switch (ScoreInfoArray[i].ScoreType)
			{
			case ScoreType_Win:		//ʤ��
				{
					m_OfflineScore[i].lWinCount++;
					break;
				}
			case ScoreType_Lose:	//���
				{
					m_OfflineScore[i].lLostCount++;
					break;
				}
			case ScoreType_Draw:	//�;�
				{
					m_OfflineScore[i].lDrawCount++;
					break;
				}
			case ScoreType_Flee:	//�Ӿ�
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

	//���ñ���
	m_bWriteScore=true;

	return true;
}

//�۷����
VOID CTableFrameItem::CostForGameStart()
{
	//�շ�ģʽ
	ASSERT( CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule) );

	for( WORD i = 0; i < m_wChairCount; i++ )
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if( m_bCostGame[i] || pIServerUserItem == NULL ) 
			continue;

		SCORE lRevenue = m_pGameServiceOption->lRevenue;
		if( lRevenue == 0 && m_pIEventServiceCost )
			lRevenue = m_pIEventServiceCost->OnEventQueryCost(pIServerUserItem);
		ASSERT(lRevenue>=0);

		//�����жϣ�ANSON��
		if (lRevenue<0)
		{
			lRevenue=0L;
			TCHAR szBuffer[128]=TEXT("");
			_sntprintf_s(szBuffer,CountArray(szBuffer),TEXT("˰�ռ���Ϊ %I64d"),lRevenue);
			CTraceService::TraceString(szBuffer,TraceLevel_Debug);
		}

		m_bCostGame[i] = true;

		//���ּ�¼
		CString s;
		s.Format(_T("shui1: i:%d, m_ScoreInfoArray[i].lScore:%I64d , lRevenue:%I64d "),i, m_ScoreInfoArray[i].lScore , lRevenue);
		CTraceService::TraceString(s, TraceLevel_Debug);

		m_ScoreInfoArray[i].lScore -= lRevenue;
		m_ScoreInfoArray[i].lRevenue += lRevenue;
		m_ScoreInfoArray[i].lBeforScore = pIServerUserItem->GetUserScore();

		//д�����
		pIServerUserItem->WriteUserScore(-lRevenue, 0, 0, lRevenue, ScoreType_Service);
	}

	return;
}

//�����˿�
bool CTableFrameItem::IsControlCard()
{
	return m_bControlCard;
}

//�˿˵ȼ�
BYTE CTableFrameItem::GetUserCardLevel(WORD wChairID)
{
	//У�����
	ASSERT (wChairID<CountArray(m_cbUserCardLevel));
	if (wChairID>=CountArray(m_cbUserCardLevel)) return 0;

	return m_cbUserCardLevel[wChairID];
}

//Ѱ���û�
IServerUserItem * CTableFrameItem::SearchUserItem(DWORD dwUserID)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//�����û�
	for (WORD i=0;i<m_wChairCount;i++)
	{
		pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	}

	//�Թ��û�
	do
	{
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if ((pIServerUserItem!=NULL)&&(pIServerUserItem->GetUserID()==dwUserID)) return pIServerUserItem;
	} while (pIServerUserItem!=NULL);

	return NULL;
}

//��Ϸ�û�
IServerUserItem * CTableFrameItem::GetTableUserItem(WORD wChairID)
{
	//Ч�����
	//ASSERT (wChairID<m_wChairCount);
	if (wChairID>=m_wChairCount) return NULL;

	//��ȡ�û�
	return m_TableUserItemArray[wChairID];
}

//�Թ��û�
IServerUserItem * CTableFrameItem::EnumLookonUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_LookonUserItemArray.GetCount()) return NULL;
	return m_LookonUserItemArray[wEnumIndex];
}

//����ʱ��
bool CTableFrameItem::SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)
{
	//Ч�����
	ASSERT(dwTimerID<TIME_TABLE_FRAME_RANGE && dwElapse>0);
	if (dwTimerID>=TIME_TABLE_FRAME_RANGE || dwElapse==0) return false;

	//����ʱ��
	DWORD dwEngineTimerID=IDI_TABLE_FRAME_START+m_wTableID*TIME_TABLE_FRAME_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->SetTimer(dwEngineTimerID+dwTimerID,dwElapse,dwRepeat,dwBindParameter);

	return true;
}

//ɾ��ʱ��
bool CTableFrameItem::KillGameTimer(DWORD dwTimerID)
{
	//Ч�����
	ASSERT(dwTimerID<TIME_TABLE_FRAME_RANGE);
	if (dwTimerID>=TIME_TABLE_FRAME_RANGE) return false;

	//ɾ��ʱ��
	DWORD dwEngineTimerID=IDI_TABLE_FRAME_START+m_wTableID*TIME_TABLE_FRAME_RANGE;
	if (m_pITimerEngine!=NULL) m_pITimerEngine->KillTimer(dwEngineTimerID+dwTimerID);

	return true;
}

//��������
bool CTableFrameItem::SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID)
{

	//���⴦��
	if (pIServerUserItem==NULL)
	{
		return m_pIServiceFunction->SendBatchData(BG_COMPUTER,MDM_GF_GAME,wSubCmdID,NULL,0);
	}

	//״̬Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//��������
	m_pIServiceFunction->SendDataToUser(pIServerUserItem,MDM_GF_GAME,wSubCmdID,NULL,0);

	return true;
}

//��������
bool CTableFrameItem::SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//���⴦��
	if (pIServerUserItem==NULL)
	{
		return m_pIServiceFunction->SendBatchData(BG_COMPUTER,MDM_GF_GAME,wSubCmdID,pData,wDataSize);
	}

	//״̬Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)&&(pIServerUserItem->IsClientReady()==false)) return false;

	//��������
	m_pIServiceFunction->SendDataToUser(pIServerUserItem,MDM_GF_GAME,wSubCmdID,pData,wDataSize);

	return true;
}

//��������
bool CTableFrameItem::SendDebugData(IServerUserItem* pIServerUserItem, WORD wDebugKind, WORD wSubCmdID)
{
	//��������
	return m_pIServiceFunction->SendDebugData(pIServerUserItem, m_wTableID, wDebugKind, wSubCmdID, NULL, 0);
}

//��������
bool CTableFrameItem::SendDebugData(IServerUserItem* pIServerUserItem, WORD wDebugKind, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	return m_pIServiceFunction->SendDebugData(pIServerUserItem, m_wTableID, wDebugKind, wSubCmdID, pData, wDataSize);
}

//��������
bool CTableFrameItem::SendTableData(WORD wChairID, WORD wSubCmdID, WORD wMainCmdID, BYTE cbDataMask)
{
	//¼������
	if (m_bVideoData || (cbDataMask & DATA_MASK_VIDEO) != 0)
	{
		if (m_pIGameVideoItem != NULL)
		{
			//��������
			m_pIGameVideoItem->InsertVideoData(wChairID, wMainCmdID, wSubCmdID, NULL, 0);

			//��¼�˳�
			if (m_bVideoData) return true;
		}
	}

	//��������
	if ((cbDataMask & DATA_MASK_NETWORK)!=0)
	{
		//�û�Ⱥ��
		if (wChairID == INVALID_CHAIR)
		{
			for (WORD i = 0; i < m_wChairCount; i++)
			{
				//��ȡ�û�
				IServerUserItem* pIServerUserItem = GetTableUserItem(i);
				if (pIServerUserItem == NULL) continue;

				//Ч��״̬
				ASSERT(pIServerUserItem->IsClientReady() == true);
				if (pIServerUserItem->IsClientReady() == false) continue;

				//��������
				m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_GAME, wSubCmdID, NULL, 0);
			}

			return true;
		}
		else
		{
			//��ȡ�û�
			IServerUserItem* pIServerUserItem = GetTableUserItem(wChairID);
			if (pIServerUserItem == NULL) return false;

			//Ч��״̬
			ASSERT(pIServerUserItem->IsClientReady() == true);
			if (pIServerUserItem->IsClientReady() == false) return false;

			//��������
			m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_GAME, wSubCmdID, NULL, 0);

			return true;
		}
	}

	return false;
}

//��������
bool CTableFrameItem::SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID, BYTE cbDataMask)
{
	//¼������
	if (m_bVideoData || (cbDataMask & DATA_MASK_VIDEO) != 0)
	{
		//¼������
		if (m_pIGameVideoItem != NULL)
		{
			//��������
			m_pIGameVideoItem->InsertVideoData(wChairID, wMainCmdID, wSubCmdID, pData, wDataSize);

			//��¼�˳�
			if (m_bVideoData) return true;
		}
	}

	//��������
	if ((cbDataMask & DATA_MASK_NETWORK) != 0)
	{
		//�û�Ⱥ��
		if (wChairID == INVALID_CHAIR)
		{
			for (WORD i = 0; i < m_wChairCount; i++)
			{
				//��ȡ�û�
				IServerUserItem* pIServerUserItem = GetTableUserItem(i);
				if ((pIServerUserItem == NULL) || (pIServerUserItem->IsClientReady() == false)) continue;

				//��������
				m_pIServiceFunction->SendDataToUser(pIServerUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);
			}

			return true;
		}
		else
		{
			//��ȡ�û�
			IServerUserItem* pIServerUserItem = GetTableUserItem(wChairID);
			if ((pIServerUserItem == NULL) || (pIServerUserItem->IsClientReady() == false)) return false;

			//��������
			m_pIServiceFunction->SendDataToUser(pIServerUserItem, wMainCmdID, wSubCmdID, pData, wDataSize);

			return true;
		}
	}

	return false;
}

//��������
bool CTableFrameItem::SendLookonData(WORD wChairID, WORD wSubCmdID, WORD wMainCmdID)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//ö���û�
	do
	{
		//��ȡ�û�
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//Ч��״̬
		//ASSERT (pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//��������
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			m_pIServiceFunction->SendDataToUser(pIServerUserItem, wMainCmdID,wSubCmdID,NULL,0);
		}

	} while (true);

	return true;
}

//��������
bool CTableFrameItem::SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wMainCmdID)
{
	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;

	//ö���û�
	do
	{
		//��ȡ�û�
		pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//Ч��״̬
		//ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false; 

		//��������
		if ((wChairID==INVALID_CHAIR)||(pIServerUserItem->GetChairID()==wChairID))
		{
			m_pIServiceFunction->SendDataToUser(pIServerUserItem, wMainCmdID,wSubCmdID,pData,wDataSize);
		}

	} while (true);

	return true;
}

//������Ϣ
bool CTableFrameItem::SendGameMessage(LPCTSTR lpszMessage, WORD wType)
{
	//��������
	WORD wEnumIndex=0;

	//������Ϣ
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

		//������Ϣ		
		m_pIServiceFunction->SendGameMessage(pIServerUserItem,lpszMessage,wType);
	}

	//ö���û�
	do
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//Ч��״̬
		ASSERT(pIServerUserItem->IsClientReady()==true);
		if (pIServerUserItem->IsClientReady()==false) return false;

		//������Ϣ		
		m_pIServiceFunction->SendGameMessage(pIServerUserItem,lpszMessage,wType);

	} while (true);

	return true;
}

//������Ϣ
bool CTableFrameItem::SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//���⴦��
	if (pIServerUserItem==NULL)
	{
		return m_pIServiceFunction->SendRoomMessage(lpszMessage,wType);
	}

	//�û�Ч��
	//ASSERT(pIServerUserItem!=NULL);
	//if (pIServerUserItem==NULL) return false;

	//������Ϣ
	m_pIServiceFunction->SendRoomMessage(pIServerUserItem,lpszMessage,wType);

	return true;
}

//�����¼�
bool CTableFrameItem::PostDataBaseRequest(WORD wRequestID, DWORD dwEngineKeyID, DWORD dwTokenID)
{
	//У���ʶ
	ASSERT(wRequestID > DB_GAME_REQUEST_START && wRequestID <= (DB_GAME_REQUEST_START + DB_GAME_IDENTIFY_RANGE));
	if (wRequestID < DB_GAME_REQUEST_START && wRequestID >(DB_GAME_REQUEST_START + DB_GAME_IDENTIFY_RANGE)) return false;

	return m_pIDataBaseEngine->PostDataBaseRequest(wRequestID, dwEngineKeyID, 0, dwTokenID, NULL, 0);
}
	
//�����¼�
bool CTableFrameItem::PostDataBaseRequest(WORD wRequestID, DWORD dwEngineKeyID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//У���ʶ
	ASSERT(wRequestID > DB_GAME_REQUEST_START && wRequestID <= (DB_GAME_REQUEST_START + DB_GAME_IDENTIFY_RANGE));
	if (wRequestID < DB_GAME_REQUEST_START && wRequestID >(DB_GAME_REQUEST_START + DB_GAME_IDENTIFY_RANGE)) return false;

	return m_pIDataBaseEngine->PostDataBaseRequest(wRequestID, dwEngineKeyID, 0, dwTokenID, pData, wDataSize);
}

//����ƹ���
bool CTableFrameItem::SendMarqeeNotice(BYTE cbNoticeKind, VOID* pNoticeContent, WORD wNoticeSize)
{
	//������Ϣ
	return m_pIServiceFunction->SendMarqeeNotice(cbNoticeKind, pNoticeContent, wNoticeSize);
}

//��Ϸ��Ϣ
bool CTableFrameItem::SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)
{
	//�û�Ч��
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//������Ϣ
	return m_pIServiceFunction->SendGameMessage(pIServerUserItem,lpszMessage,wType);
}

//ģ������
bool CTableFrameItem::ImitateGameOption(IServerUserItem* pIServerUserItem)
{
	//����У��
	ASSERT(pIServerUserItem != NULL);
	if (pIServerUserItem == NULL) return false;

	//״̬�ж�
	if (m_bGameStarted == true) return true;

	//��������
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_MATCH) != 0)
	{
		if (pIServerUserItem->GetUserMatchStatus() != MUS_PLAYING)
		{
			return false;
		}
	}
	else
	{
		//����״̬
		if (pIServerUserItem->GetUserStatus() == US_SIT)
		{
			//��ȡ��λ
			WORD wChairID = pIServerUserItem->GetChairID();

			//׼��У��
			if (m_pIEventStartEfficacy != NULL && m_pIEventStartEfficacy->IsAllowPlayerReady(wChairID) == false)
			{
				return false;
			}

			//����״̬
			pIServerUserItem->SetUserStatus(US_READY, m_wTableID, wChairID);
		}
	}

	//����״̬
	pIServerUserItem->SetClientReady(true);

	//��ʼ�ж�
	if (pIServerUserItem->GetUserStatus() == US_READY)
	{
		//��ʼģʽ�ж�
		if (m_GameStartMode == StartMode_MasterControl)
		{
			//����״̬
			SendStartStatus();
		}
		else
		{
			//��ʼ��Ϸ
			if (EfficacyStartGame(pIServerUserItem->GetChairID()) == true)
			{
				TryStartGame();
			}
		}
	}

	return true;
}

//���ͳ���
bool CTableFrameItem::OnEventSendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	//�û�Ч��
	ASSERT((pIServerUserItem!=NULL)&&(pIServerUserItem->IsClientReady()==true));
	if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) return false;

	//¼������
	if (m_pIGameVideoItem!=NULL && m_bVideoData==true)
	{
		m_pIGameVideoItem->InsertVideoData(pIServerUserItem->GetChairID(),MDM_GF_FRAME,SUB_GF_GAME_SCENE,pData,wDataSize);

		return true;
	}

	//���ͳ���
	ASSERT(m_pIServiceFunction!=NULL);
	m_pIServiceFunction->SendDataToUser(pIServerUserItem,MDM_GF_FRAME,SUB_GF_GAME_SCENE,pData,wDataSize);

	return true;
}

//��������
VOID CTableFrameItem::FreezeGameScore(IServerUserItem *pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL&&m_bGameStarted);
	if( pIServerUserItem == NULL || !m_bGameStarted ) return;

	//��������
	WORD wChairId = pIServerUserItem->GetChairID();

	//���ñ���
	ZeroMemory( &m_OfflineScore[wChairId],sizeof(m_OfflineScore[wChairId]) );
	m_OfflineScore[wChairId].dwUserID = pIServerUserItem->GetUserID();
	m_OfflineScore[wChairId].dwUserRight = pIServerUserItem->GetUserRight();
	m_OfflineScore[wChairId].dwClientIP = pIServerUserItem->GetClientIP();
	m_OfflineScore[wChairId].dwPlayTimeCount = (DWORD)time(NULL)-m_dwGameStartTime;

	m_OfflineScore[wChairId].lUnLockScore = m_pGameServiceOption->lLessScore;
	m_OfflineScore[wChairId].lExperience = (m_OfflineScore[wChairId].dwPlayTimeCount+59L)/60L;

	//�������
	if (m_pGameServiceOption->wServerType & (GAME_GENRE_GOLD|SCORE_GENRE_POSITIVE))
		pIServerUserItem->FreezeUserScore(m_OfflineScore[wChairId].lUnLockScore,true);

	return;
}

//�۳������
VOID CTableFrameItem::ChargeGameAndReset()
{
	if( CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule) )
	{
		//�۷����
		CostForGameStart();

		//ѯ�����
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

//�����¼�
bool CTableFrameItem::OnEventUserOffLine(IServerUserItem * pIServerUserItem)
{
	//����Ч��
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�û�����
	auto pUserInfo=pIServerUserItem->GetUserInfo();
	auto pITableUserItem=m_TableUserItemArray[pUserInfo->wChairID];

	//�û�����
	WORD wChairID=pIServerUserItem->GetChairID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

	//��Ϸ�û�
	if (cbUserStatus!=US_LOOKON)
	{
		//Ч���û�
		ASSERT (pIServerUserItem==GetTableUserItem(wChairID));
		if (pIServerUserItem!=GetTableUserItem(wChairID)) return false;

		//��������
		bool bForbidOfflineStandup=CServerRule::IsForbidOfflineStandup(m_pGameServiceOption->dwServerRule);

		//���ߴ���
		if ((cbUserStatus==US_PLAYING)&&(m_wOffLineCount[wChairID]<MAX_OFF_LINE || bForbidOfflineStandup==true))
		{
			//�û�����
			pIServerUserItem->SetClientReady(false);
			pIServerUserItem->SetUserStatus(US_OFFLINE,m_wTableID,wChairID);

			//�йܴ���
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0 ||
			  ((m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE)!=0 && GetRelationParameter()->dwRelationNum!=0))
			{
				if (pIServerUserItem->IsTrusteeUser() == false)
				{
					//�����й�
					pIServerUserItem->SetTrusteeUser(true);
					pIServerUserItem->SetClientReady(true);

					//����֪ͨ
					if (m_pITableUserAction != NULL) m_pITableUserAction->OnActionUserOffLine(pIServerUserItem);
				}

				return true;
			}

			//�����й�
			if (bForbidOfflineStandup==true) pIServerUserItem->SetTrusteeUser(true);			

			//�¼�֪ͨ
			if (m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserOffLine(pIServerUserItem);

			//���ߴ���
			if (m_dwOffLineTime[wChairID]==0L && m_cbGameStatus>=GAME_STATUS_PLAY)
			{
				//���ñ���
				m_wOffLineCount[wChairID]++;
				m_dwOffLineTime[wChairID]=(DWORD)time(NULL);

				//ʱ������
				WORD wOffLineCount=GetOffLineUserCount();
				if (wOffLineCount==1) SetGameTimer(IDI_OFF_LINE,TIME_OFF_LINE*1000L,1,wChairID);
			}

			return true;
		}
	}

	//�û�����
	PerformStandUpAction(pIServerUserItem,true);

	//��������
	if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
	{
		if (pIServerUserItem->IsTrusteeUser()==true) return true;
	}

	//ɾ���û�
	if (pIServerUserItem->GetUserStatus()==US_FREE)
	{
		pIServerUserItem->SetUserStatus(US_NULL,INVALID_TABLE,INVALID_CHAIR); 
	}

	return true;
}

//�ؽ��¼�
bool CTableFrameItem::OnEventUserReConnect(IServerUserItem * pIServerUserItem)
{
	//����Ч��
	ASSERT(pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�û�����
	auto pUserInfo=pIServerUserItem->GetUserInfo();
	auto pITableUserItem=m_TableUserItemArray[pUserInfo->wChairID];

	//�û�����
	auto wChairID=pIServerUserItem->GetChairID();
	auto cbUserStatus=pIServerUserItem->GetUserStatus();

	//���ñ���
	m_dwOffLineTime[wChairID]=0L;

	return true;
}

//�û������¼�
bool CTableFrameItem::OnEventUserRule(IServerUserItem *pIServerUserItem)
{
	//�жϼ��ܽ���
	if (GetSitUserCount()==1)
	{
		//��������
		auto pUserRule=pIServerUserItem->GetUserRule();

		//״̬����
		bool bTableLocked=IsTableLocked();

		//��������
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE)==0)
		{
			//���ñ���
			m_dwTableOwnerID=pIServerUserItem->GetUserID();
			StringCchCopy(m_szTablePassword,CountArray(m_szTablePassword), pUserRule->szPassword);
		}		

		//����״̬
		if (bTableLocked!=IsTableLocked()) 
		{
			if( m_pITableEventSink )
				m_pITableEventSink->OnEventTableLocked(QUERY_ME_INTERFACE(ITableFrameItem),IsTableLocked()?TRUE:FALSE);

			SendTableStatus();
		}
	}

	return true;
}

//�������
VOID CTableFrameItem::OnEventUserScoreChange(IServerUserItem *pIServerUserItem)
{
	//����У��
	ASSERT(pIServerUserItem!=NULL);
	if(pIServerUserItem==NULL) return;

	//�û�����
	if(GetTableUserItem(pIServerUserItem->GetChairID())!=pIServerUserItem) return;

	//���֪ͨ
	if(m_pIEventUserScore!=NULL)
		m_pIEventUserScore->OnEventScoreVariation(pIServerUserItem);

	return;
}

//��ʱ���¼�
bool CTableFrameItem::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//�ص��¼�
	if ((dwTimerID>=0)&&(dwTimerID<TIME_TABLE_SINK_RANGE))
	{
		ASSERT(m_pITableFrameSink!=NULL);
		return m_pITableFrameSink->OnTimerMessage(dwTimerID,dwBindParameter);
	}

	//�¼�����
	switch (dwTimerID)
	{
	case IDI_OFF_LINE:			//�����¼�
	{
		//Ч��״̬
		ASSERT(m_bGameStarted==true);
		if (m_bGameStarted==false) return true;

		//��������
		DWORD dwOffLineTime=0L;
		WORD wOffLineChair=INVALID_CHAIR;

		//Ѱ���û�
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
			{
				wOffLineChair=i;
				dwOffLineTime=m_dwOffLineTime[i];
			}
		}

		//λ���ж�
		ASSERT (wOffLineChair!=INVALID_CHAIR);
		if (wOffLineChair==INVALID_CHAIR) return true;

		//�û��ж�
		ASSERT (dwBindParameter<m_wChairCount);
		if (wOffLineChair!=(WORD)dwBindParameter)
		{
			//ʱ�����
			DWORD dwCurrentTime=(DWORD)time(NULL);
			DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

			//����ʱ��
			dwLapseTime=__min(dwLapseTime,TIME_OFF_LINE*1000L-2000L);
			SetGameTimer(IDI_OFF_LINE,TIME_OFF_LINE*1000L-dwLapseTime,1,wOffLineChair);

			return true;
		}

		//��ȡ�û�
		ASSERT(GetTableUserItem(wOffLineChair)!=NULL);
		IServerUserItem * pIServerUserItem=GetTableUserItem(wOffLineChair);

		//������Ϸ
		if (pIServerUserItem!=NULL)
		{
			//���ñ���
			m_dwOffLineTime[wOffLineChair]=0L;

			//���߳�ʱ����
			if (CServerRule::IsForbidOfflineStandup(m_pGameServiceOption->dwServerRule)==false)
			{
				//�û�����
				PerformStandUpAction(pIServerUserItem,true);
			}
		}

		//����ʱ��
		if (m_bGameStarted==true)
		{
			//��������
			DWORD dwOffLineTime=0L;
			WORD wOffLineChair=INVALID_CHAIR;

			//Ѱ���û�
			for (WORD i=0;i<m_wChairCount;i++)
			{
				if ((m_dwOffLineTime[i]!=0L)&&((m_dwOffLineTime[i]<dwOffLineTime)||(wOffLineChair==INVALID_CHAIR)))
				{
					wOffLineChair=i;
					dwOffLineTime=m_dwOffLineTime[i];
				}
			}

			//����ʱ��
			if (wOffLineChair!=INVALID_CHAIR)
			{
				//ʱ�����
				DWORD dwCurrentTime=(DWORD)time(NULL);
				DWORD dwLapseTime=dwCurrentTime-m_dwOffLineTime[wOffLineChair];

				//����ʱ��
				dwLapseTime=__min(dwLapseTime,TIME_OFF_LINE * 1000L -2000L);
				SetGameTimer(IDI_OFF_LINE,TIME_OFF_LINE * 1000L -dwLapseTime,1,wOffLineChair);
			}
		}

		return true;
	}
	case IDI_KICKOUT_PLAYER:	//�߳����
	{
		//Ч��״̬
		if (m_bGameStarted==true) return false;

		//��������
		DWORD dwUnReadyTime=0L;
		WORD wUnReadyChair=INVALID_CHAIR;

		//Ѱ���û�
		for (WORD i=0;i<m_wChairCount;i++)
		{
			if ((m_dwUserUnReadyTime[i]!=0L)&&((m_dwUserUnReadyTime[i]<dwUnReadyTime)||(wUnReadyChair==INVALID_CHAIR)))
			{
				wUnReadyChair=i;
				dwUnReadyTime=m_dwUserUnReadyTime[i];
			}
		}

		//λ���ж�
		ASSERT(dwUnReadyTime!=INVALID_CHAIR);
		if (wUnReadyChair==INVALID_CHAIR) return false;

		//�û��ж�
		ASSERT(dwBindParameter<m_wChairCount);
		if (wUnReadyChair!=(WORD)dwBindParameter)
		{
			//ʱ�����
			DWORD dwCurrentTime=DWORD(time(NULL));
			DWORD dwLapseTime=dwCurrentTime-m_dwUserUnReadyTime[wUnReadyChair];

			//��ȡʱ��
			DWORD dwElapse=TIME_KICKOUT_PLAYER;
			if (m_pIEventCustomFrameTime!=NULL)
			{
				dwElapse=m_pIEventCustomFrameTime->GetUnReadyKickOutTime();
			}

			//����ʱ��
			dwLapseTime=__min(dwLapseTime,dwElapse-1L);
			SetGameTimer(IDI_KICKOUT_PLAYER,(dwElapse-dwLapseTime)*1000L,1,wUnReadyChair);

			return true;
		}

		//��ȡ�û�
		ASSERT (GetTableUserItem(wUnReadyChair)!=NULL);
		IServerUserItem * pIServerUserItem=GetTableUserItem(wUnReadyChair);

		//׼������
		if (pIServerUserItem!=NULL)
		{
			//���ñ���
			m_dwUserUnReadyTime[wUnReadyChair]=0;

			//������Ϣ
			m_pIServiceFunction->SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0068,L"��������ʱ��û�п�ʼ��Ϸ��ϵͳ��������ѡ����Ϸ����"),SMT_CHAT|SMT_CLOSE_GAME|SMT_EJECT);

			//�û�����
			PerformStandUpAction(pIServerUserItem,true);
		}			

		//���ñ���
		dwUnReadyTime=0;
		wUnReadyChair=INVALID_CHAIR;

		//����Ѱ��
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
			//ʱ�����
			DWORD dwCurrentTime=(DWORD)time(NULL);
			DWORD dwLapseTime=dwCurrentTime-dwUnReadyTime;

			//��ȡʱ��
			DWORD dwElapse=TIME_KICKOUT_PLAYER;
			if (m_pIEventCustomFrameTime!=NULL)
			{
				dwElapse=m_pIEventCustomFrameTime->GetUnReadyKickOutTime();
			}

			//����ʱ��
			dwLapseTime=__min(dwLapseTime,dwElapse-1L);
			SetGameTimer(IDI_KICKOUT_PLAYER,(dwElapse-dwLapseTime)*1000L,1,wUnReadyChair);
		}
		else
		{
			//�رն�ʱ��
			KillGameTimer(IDI_KICKOUT_PLAYER);
		}

		return true;
	}
	case IDI_KICKOUT_LOOKON:	//�߳��Թ�		
	{
		//�ж�����
		if (m_LookonUserItemArray.GetCount() == 0)
		{
			return true;
		}

		//��������
		WORD wEnumIndex = 0;
		DWORD dwCurrentTime = (DWORD)time(NULL);
		IServerUserItem * pIServerUserItem = NULL;

		//ö���û�
		do
		{
			//��ȡ�û�
			pIServerUserItem = EnumLookonUserItem(wEnumIndex++);
			if (pIServerUserItem == NULL) break;

			//Ч��״̬
			//ASSERT(pIServerUserItem->IsClientReady()==true);
			if (pIServerUserItem->IsClientReady() == false) return false;

			//�߳��ж�
			if ((((CServerUserItem *)pIServerUserItem)->GetLookonTime() + ALLOW_LOOKON_TIME) < dwCurrentTime)
			{
				//��������					
				m_pIServiceFunction->SendGameMessage(pIServerUserItem, TEXT(""), SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);
			}

		} while (true);

		return true;
	}		
	}

	return false;
}

//��Ϸѡ��
VOID CTableFrameItem::OnEventClientReady(IServerUserItem* pIServerUserItem)
{
	return;
}

//���ӳ�ʼ��
VOID CTableFrameItem::OnEventTableFrameInit(tagTableFrameParameter& TableFrameParameter)
{
	return;
}

//��ʼ�¼�
bool CTableFrameItem::OnEventGameStart()
{
	return true;
}

//����ʱ��
bool CTableFrameItem::OnEventGameConclude()
{
	return true;
}

//�û�����
bool CTableFrameItem::OnActionUserSitdown(IServerUserItem* pIServerUserItem)
{
	return true;
}

//�û�����
bool CTableFrameItem::OnActionUserStandup(IServerUserItem* pIServerUserItem)
{
	return true;
}

//�Թ�У��
bool CTableFrameItem::EfficacyUserLookon(WORD wChairID, IServerUserItem* pIServerUserItem)
{
	//��ȡ�û�
	auto pITableUserItem = GetTableUserItem(wChairID);
	//if (pITableUserItem == NULL) return false;

	//��Ϸ״̬
	if (m_bTableStarted == false && pIServerUserItem->GetMasterOrder() == 0 &&
		!CServerRule::IsSitForLookon(m_pGameServiceOption->dwServerRule))
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0085, L"��Ϸ��û�п�ʼ����ʱ�����Թ۴���Ϸ����"), 0);
		return false;
	}

	//�����ж�
	if (pITableUserItem == NULL &&
		!CServerRule::IsSitForLookon(m_pGameServiceOption->dwServerRule))
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0086, L"���������λ��û����Ϸ��ң��޷��Թ۴���Ϸ����"), 0);
		return false;
	}

	//״̬�ж�
	if ((g_GlobalUnits.IsAllowEnterGame() == false) && (pIServerUserItem->GetMasterOrder() == 0))
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0087, L"��Ǹ������Ϸ����ʱ�������û����룡"), 0);
		return false;
	}

	//����Ч��
	if (IsTableLocked() == true && !CUserRight::CanEnterWithoutPasswd(pIServerUserItem->GetUserRight()) &&
		pIServerUserItem->GetMasterOrder() == 0 && !CServerRule::IsForbidPasswordSet(m_pGameServiceOption->dwServerRule))
	{
		if (pIServerUserItem->IsTmpPasswdEmpty() == false)
		{
			if (!pIServerUserItem->ContrastTempPassword(m_szTablePassword))
			{
				//������ʱ����
				pIServerUserItem->SetTmpPassword(L"");

				SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0090, L"����������󣬲��ܼ�����Ϸ��"), RFC_TMP_PASSWD_INCORRECT);
				return false;
			}

			//������ʱ����
			pIServerUserItem->SetTmpPassword(L"");
		}
		else if (!pIServerUserItem->ContrastTablePassword(m_szTablePassword))
		{
			SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0090, L"����������󣬲��ܼ�����Ϸ��"), RFC_PASSWORD_INCORRECT);
			return false;
		}
	}

	return true;
}

//����У��
bool CTableFrameItem::EfficacyUserSitdown(WORD wChairID, IServerUserItem* pIServerUserItem)
{
	//��ȡ�û�
	auto pITableUserItem = GetTableUserItem(wChairID);

	//״̬�ж�
	if ((g_GlobalUnits.IsAllowEnterGame() == false) && (pIServerUserItem->GetMasterOrder() == 0))
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0087, L"��Ǹ������Ϸ����ʱ�������û����룡"), 0);
		return false;
	}

	//��Ϸ״̬
	if ((m_bTableStarted == true) &&
		m_pGameServiceAttrib->cbDynamicJoin == FALSE && !CServerRule::IsDynamicJoinGame(m_pGameServiceOption->dwServerRule)
		)
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0092, L"��Ϸ�Ѿ���ʼ�ˣ���ʱ���ܽ�����Ϸ����"), 0);
		return false;
	}

	//�����ж�
	if (pITableUserItem != NULL)
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessageV(MIM_SERVER_0093, L"�����Ѿ��� [ %s ] �����ȵ��ˣ��´ζ���Ҫ����ˣ�", pITableUserItem->GetAccounts()), 0);
		return false;
	}

	//����Ч�� kk jia
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
	{
		//CString s;
		//s.Format(_T("GetUserScore��%ld  ,< m_lMinEnterScore:%ld "), pIServerUserItem->GetUserScore(), m_lMinEnterScore);
		//CTraceService::TraceString(s, TraceLevel_Exception);


		//�����ж�
		if (m_lMinEnterScore != 0L && pIServerUserItem->GetUserScore() < m_lMinEnterScore)
		{

			LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0101, L"������Ϸ������Ҫ " SCORE_STRING " �����֣����Ļ��ֲ��㲻�ܼ��룡", SCORE_ZOOMOUT(m_lMinEnterScore));
			SendRequestFailure(pIServerUserItem, pszMessage, RFC_SCORE_UNENOUGH);
			return false;
		}
	}
	//kk jia end

	//����Ч��
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_MATCH) == 0)
	{
		//����У��
		if (EfficacyAddrRule(pIServerUserItem) == false) return false;

		//��λ�������ƻ���
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_RANKING) == 0)
		{
			if (EfficacyTableRule(pIServerUserItem) == false) return false;
			if (EfficacyScoreRule(pIServerUserItem) == false) return false;
		}
	}

	return true;
}

//����У��
bool CTableFrameItem::EfficacyUserStandup(WORD wChairID, IServerUserItem* pIServerUserItem, bool bInitiative)
{
	//�û�����
	BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

	//��Ϸ�û�
	if ((m_bGameStarted == true) && ((cbUserStatus == US_PLAYING) || (cbUserStatus == US_OFFLINE)))
	{
		//ǿ�˽�ֹ����
		if (CServerRule::IsForbidForceStandup(m_pGameServiceOption->dwServerRule) == true)
		{
			//�û�����
			//pIServerUserItem->SetTrusteeUser(true);

			////��ʾ��Ϣ
			//if (cbUserStatus == US_OFFLINE)
			//{
			//	CTraceService::TraceString(TEXT("���״̬ΪUS_OFFLINEʱ,������������������ "), TraceLevel_Exception);
			//}

			//����֪ͨ
			//if (m_pITableUserAction != NULL) m_pITableUserAction->OnActionUserOffLine(pIServerUserItem);

			//�����˳�
			if (bInitiative == true)
			{
				SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0120, TEXT("��Ǹ����Ϸ��δ��ɣ�����ʱ�����뿪��")), 0);
			}

			return false;
		}

		//����ԭ��
		BYTE cbReason = cbUserStatus == US_OFFLINE ? GER_NETWORK_ERROR : GER_USER_LEAVE;

		//������Ϸ
		m_pITableFrameSink->OnEventGameConclude(wChairID, pIServerUserItem, cbReason);

		//�뿪�ж�
		if (m_TableUserItemArray[wChairID] != pIServerUserItem) return false;
	}	

	//�����йܴ���
	//if ((m_pGameServiceOption->wServerType & GAME_GENRE_MATCH) != 0 && bInitiative == true)
	//{
	//	//�ж�״̬
	//	if (pIServerUserItem->GetUserMatchStatus() == MUS_PLAYING)
	//	{
	//		if (pIServerUserItem->IsTrusteeUser() == false)
	//		{
	//			pIServerUserItem->SetTrusteeUser(true);

	//			//����֪ͨ
	//			if (m_pITableUserAction != NULL) m_pITableUserAction->OnActionUserOffLine(pIServerUserItem);
	//		}

	//		return false;
	//	}
	//}

	return true;
}

//��Ϸ�¼�
bool CTableFrameItem::OnEventSocketGame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(m_pITableFrameSink!=NULL);

	//��Ϣ����
	return m_pITableFrameSink->OnGameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem);
}

//����¼�
bool CTableFrameItem::OnEventSocketFrame(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	//��Ϸ����
	if (m_pITableFrameSink->OnFrameMessage(wSubCmdID,pData,wDataSize,pIServerUserItem)==true) return true;

	//Ĭ�ϴ���
	switch (wSubCmdID)
	{
	case SUB_GF_GAME_OPTION:	//��Ϸ����
		{
			//Ч�����
			ASSERT (wDataSize == sizeof(CMD_GF_GameOption));
			if (wDataSize != sizeof(CMD_GF_GameOption)) return false;

			//��������
			CMD_GF_GameOption * pGameOption = (CMD_GF_GameOption *)pData;

			//��ȡ����
			WORD wChairID = pIServerUserItem->GetChairID();
			BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

			//��������
			if ((cbUserStatus != US_LOOKON) && ((m_dwOffLineTime[wChairID] != 0L)))
			{
				//���ñ���
				m_dwOffLineTime[wChairID] = 0L;

				//ɾ��ʱ��
				WORD wOffLineCount = GetOffLineUserCount();
				if (wOffLineCount == 0) KillGameTimer(IDI_OFF_LINE);
			}

			//����״̬
			pIServerUserItem->SetClientReady(true);
			pIServerUserItem->SetTrusteeUser(false);

			//�ж�״̬
			if (cbUserStatus != US_LOOKON)
			{
				bool bAllowLookon = pGameOption->cbAllowLookon ? true : false;
				if (bAllowLookon != m_bAllowLookon[wChairID])
				{
					m_bAllowLookon[wChairID] = bAllowLookon;

					//������Ϣ
					CMD_GF_LookonStatus LookonStatus;
					LookonStatus.bAllowLookon = bAllowLookon;

					//������Ϣ
					for (INT_PTR i = 0; i < m_LookonUserItemArray.GetCount(); i++)
					{
						//��ȡ�û�
						IServerUserItem * pILookonUserItem = m_LookonUserItemArray[i];
						if (pILookonUserItem->GetChairID() != pIServerUserItem->GetChairID()) continue;

						//������Ϣ
						ASSERT(m_pIServiceFunction != NULL);
						m_pIServiceFunction->SendDataToUser(pILookonUserItem, MDM_GF_FRAME, SUB_GF_LOOKON_STATUS, &LookonStatus, sizeof(LookonStatus));
					}
				}
			}

			//����״̬
			CMD_GF_GameStatus GameStatus;
			GameStatus.bGamePaused = m_bGamePaused;
			GameStatus.cbGameStatus = m_cbGameStatus;
			GameStatus.cbAllowLookon = m_bAllowLookon[wChairID] ? TRUE : FALSE;
			m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_FRAME, SUB_GF_GAME_STATUS, &GameStatus, sizeof(GameStatus));

			//�Զ�׼��
			bool bAutoReady = false;

			//Լսģʽ
			if ((m_pGameServiceOption->wServerType&(GAME_GENRE_BATTLE|GAME_GENRE_RANKING)) != 0 && GetRelationParameter()->dwRelationNum != 0)
			{
				//У��״̬
				if (cbUserStatus==US_SIT && m_cbGameStatus==GAME_STATUS_FREE && (m_GameStartMode==StartMode_MasterControl || m_GameStartMode==StartMode_FullReady))
				{
					bAutoReady = true;
				}
			}

			//����ģʽ
			if (m_pGameServiceOption->wServerType&(TABLE_GENRE_DISTRIBUTE|GAME_GENRE_RANKING))
			{
				//�ж�״̬
				if (cbUserStatus == US_SIT)
				{
					//�����¼�
					if (m_pITableUserRequest != NULL && !m_pITableUserRequest->OnTableUserReqReady(pIServerUserItem, NULL, 0))
					{
						return false;
					}

					//�¼�֪ͨ
					if ((m_pITableUserAction != NULL) && (m_pITableUserAction->OnActionUserReady(pIServerUserItem, NULL, 0) == false))
					{
						ASSERT(FALSE);
						return false;
					}

					bAutoReady = true;
				}
			}			

			//�¼�����
			OnEventClientReady(pIServerUserItem);

			//���ͳ���
			bool bSendSecret=((cbUserStatus!=US_LOOKON)||(m_bAllowLookon[wChairID]==true));
			m_pITableFrameSink->OnEventSendGameScene(wChairID,pIServerUserItem,m_cbGameStatus,bSendSecret);			

			//ͬIP����
			if (m_bSameIpRemind == true && GetSitUserCount() >= 3)
			{
				//��������		
				CWordArray ChairArray;
				CWordArray FilterArray;
				IServerUserItem* pIServerUserItem1 = NULL;
				IServerUserItem* pIServerUserItem2 = NULL;
				IServerUserItem* pIServerUserItem3 = NULL;

				//��ȡ�û�
				for (WORD wChair1 = 0; wChair1 < m_wChairCount; wChair1++)
				{
					//��ȡ�û�
					if (!(pIServerUserItem1 = GetTableUserItem(wChair1))) continue;

					//�Ƴ�Ԫ��
					FilterArray.RemoveAll();

					//����ͬIP
					for (WORD wChair2 = 0; wChair2 < m_wChairCount; wChair2++)
					{
						//��������
						if (wChair1 == wChair2) continue;
						if (!(pIServerUserItem2 = GetTableUserItem(wChair2))) continue;
						if (pIServerUserItem2->GetClientIP() == pIServerUserItem1->GetClientIP()) continue;

						//���˷�λ
						bool bExists = false;
						for (INT_PTR i = 0; i < FilterArray.GetCount(); i++)
						{
							if (wChair2 == FilterArray[i])
							{
								bExists = true;
								break;
							}
						}

						//�����ж�
						if (bExists == true) continue;

						//�������
						ChairArray.Add(wChair2);

						//����ͬIP
						for (WORD wChair3 = wChair2 + 1; wChair3 < m_wChairCount; wChair3++)
						{
							//��������
							if (wChair3 == wChair1) continue;
							if (!(pIServerUserItem3 = GetTableUserItem(wChair3))) continue;
							if (pIServerUserItem3->GetClientIP() != pIServerUserItem2->GetClientIP()) continue;

							//�������
							ChairArray.Add(wChair3);
						}

						//�ж�����
						if (ChairArray.GetCount() >= 2)
						{
							//��������					
							CStringW strMessage;
							TCHAR szMessage[128] = L"";

							//������ʾ
							for (INT_PTR i = 0; i < ChairArray.GetCount(); i++)
							{
								_snwprintf_s(szMessage, CountArray(szMessage), L"���: %s", GetTableUserItem(ChairArray[i])->GetAccounts());
								strMessage.Append(szMessage);
								if (i < ChairArray.GetCount() - 1) strMessage.Append(L"��");
							}

							//������ʾ
							strMessage.Append(L"��IP��ַ��ͬ��");

							//������Ϣ
							SendGameMessage(pIServerUserItem1, CWHService::FormatMessage(MIM_SERVER_0071, L"��ע�⣬���������IP��ַ��ͬ��"), SMT_TABLE_ROLL | SMT_SHOW_MOBILE);
						}

						//�������
						FilterArray.Append(ChairArray);
						ChairArray.RemoveAll();
					}
				}
			}
			
			//�Զ�׼��
			if (bAutoReady)
			{
				return OnEventSocketFrame(SUB_GF_USER_READY,0,0,pIServerUserItem);
			}

			return true;
		}
	case SUB_GF_USER_READY:		//�û�׼��
		{
			//��ȡ����
			WORD wChairID=pIServerUserItem->GetChairID();
			BYTE cbUserStatus=pIServerUserItem->GetUserStatus();

			//Ч��״̬
			ASSERT(GetTableUserItem(wChairID)==pIServerUserItem);
			if (GetTableUserItem(wChairID)!=pIServerUserItem) return false;

			//Ч��״̬
			//ASSERT(cbUserStatus==US_SIT);
			if (cbUserStatus!=US_SIT) return true;

			//׼����֤
			if (m_pIEventStartEfficacy!=NULL)
			{
				if (m_pIEventStartEfficacy->IsAllowPlayerReady(wChairID)==false) 
				{
					return true;
				}
			}

			//���ñ���
			m_dwUserUnReadyTime[wChairID]=0;

			//��������
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
			{
				return true;
			}

			//�����ѽ�ɢ
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE)!=0 && GetRelationParameter()->dwRelationNum==0)
			{		
				//������Ϣ
				SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0047,L"��Ǹ���������ѱ���ɢ��"),SMT_CHAT|SMT_SHOW_MOBILE);

				return true;				
			}

			//����ģʽ
			if (m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)
			{
				//������Ϣ
				SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0072, L"ϵͳ���·������ӣ����Ժ�"),SMT_CHAT);

				//������Ϣ
				m_pIServiceFunction->InsertDistribute(pIServerUserItem);

				return true;
			}

			//�����¼�
			if (m_pITableUserRequest != NULL && !m_pITableUserRequest->OnTableUserReqReady(pIServerUserItem,pData,wDataSize))
			{
				return false;
			}

			//�¼�֪ͨ
			if ((m_pITableUserAction!=NULL)&&(m_pITableUserAction->OnActionUserReady(pIServerUserItem,pData,wDataSize)==false))
			{
				ASSERT(FALSE);
				return false;
			}			

			//׼��״̬
			pIServerUserItem->SetUserStatus(US_READY, m_wTableID, wChairID);

			//��ʼ�ж�
			if (EfficacyStartGame(wChairID)==true)
			{
				//��ʼģʽ
				if ((m_GameStartMode != StartMode_MasterControl))
				{
					//��ʼ��Ϸ
					TryStartGame();
				}
			}

			return true;
		}
	case SUB_GF_START_GAME:		//��ʼ��Ϸ
		{
			//�û���Ϣ
			WORD dwUserID = pIServerUserItem->GetUserID();
			WORD wChairID = pIServerUserItem->GetChairID();			
			BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

			//Ч��״̬
			ASSERT (GetTableUserItem(wChairID) == pIServerUserItem);
			if (GetTableUserItem(wChairID) != pIServerUserItem) return false;

			//Ч��״̬
			//ASSERT (cbUserStatus == US_READY);
			if (cbUserStatus == US_PLAYING ) return true;
			if (cbUserStatus != US_READY) return false;

			//У��ģʽ
			ASSERT (m_GameStartMode == StartMode_MasterControl);
			if (m_GameStartMode != StartMode_MasterControl) return false;

			//��֤�ӿ�
			ASSERT (m_pIEventStartEfficacy != NULL);
			if (m_pIEventStartEfficacy == NULL) return NULL;

			//У�����
			if (dwUserID != m_dwTableOwnerID)
			{
				//��ȡ��λ			
				WORD wMasterChairID = m_pIEventStartEfficacy->GetMasterChairID();
				if (wMasterChairID != wChairID) return false;
			}					

			//��ʼ�ж�
			if (EfficacyStartGame(INVALID_CHAIR) == false)
			{
				//����ṹ
				CMD_GF_StartStatus StartStatus;
				ZeroMemory(&StartStatus,sizeof(StartStatus));

				//����״̬
				StartStatus.bEnabled = false;
				StartStatus.bVisibled = true;

				//��������
				m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_FRAME, SUB_GF_START_STATUS, &StartStatus, sizeof(StartStatus));
			}
			else
			{				
				//��ʼ��Ϸ
				TryStartGame();
			}

			return true;
		}
	case SUB_GF_USER_CHAT:		//�û�����
		{
			//��������
			CMD_GF_UserChat * pUserChat=(CMD_GF_UserChat *)pData;

			//Ч�����
			ASSERT(wDataSize>=(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatString)));
			ASSERT(wDataSize==(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(TCHAR)));

			//Ч�����
			if (wDataSize<(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatString))) return false;
			if (wDataSize!=(sizeof(CMD_GF_UserChat)-sizeof(pUserChat->szChatString)+pUserChat->wChatLength*sizeof(TCHAR))) return false;

			//Ŀ���û�
			if ((pUserChat->dwTargetUserID!=0)&&(SearchUserItem(pUserChat->dwTargetUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//��Ա�Թ�
			if ((pIServerUserItem->GetUserStatus()==US_LOOKON)&&(pIServerUserItem->GetMasterOrder()==0))
			{
				if(g_GlobalUnits.GetAllowLookOnChat() == 0)
				{
					SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0073, TEXT("��Ǹ��Ϊ�˱���Ӱ����Ϸ���������У�ϵͳ��ֹ������Թ۹����з��ԣ�")),SMT_EJECT|SMT_CHAT);
					return true;
				}
				else if((g_GlobalUnits.GetAllowLookOnChat()==1)&&(pIServerUserItem->GetMemberOrder()==0))
				{
					TCHAR szBrowseUrl[256]=L"";
					TCHAR szMessage[512]=L"";

					SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0074, TEXT("��Ǹ���˷����Թ����칦��ֻ�Ի�Ա����")),SMT_EJECT|SMT_CHAT);
					return true;
				}
				
			}

			//״̬�ж�
			if ((g_GlobalUnits.IsAllowGameChat()==false)&&(pIServerUserItem->GetMasterOrder()==0L))
			{
				//���ǻ�Ա
				if( !g_GlobalUnits.IsMemberCanChat() || pIServerUserItem->GetMemberOrder() == 0 )
				{
					SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0075, TEXT("��Ǹ������Ϸ���䲻������Ϸ���죡")),SMT_CHAT);
					return true;
				}
			}

			//Ȩ���ж�
			if (CUserRight::CanGameChat(pIServerUserItem->GetUserRight())==false)
			{
				SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0076, TEXT("��Ǹ����û����Ϸ�����Ȩ�ޣ�����Ҫ����������ϵ��Ϸ�ͷ���ѯ��")),SMT_EJECT|SMT_CHAT);
				return true;
			}

			//����У��
			if (pIServerUserItem->GetShutUpStatus()!=0 && pIServerUserItem->GetShutUpOverTime()>time(NULL))
			{
				SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0076, TEXT("�ܱ�Ǹ�����ѱ�����Ա���ԣ�����ϵ�ͷ��˽���ϸ�����")), SMT_EJECT | SMT_CHAT);
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

			//��Ϸ�û�
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//��ȡ�û�
				IServerUserItem * pIServerUserItem=GetTableUserItem(i);
				if ((pIServerUserItem==NULL)||(pIServerUserItem->IsClientReady()==false)) continue;

				//��������
				m_pIServiceFunction->SendDataToUser(pIServerUserItem,MDM_GF_FRAME,SUB_GF_USER_CHAT,&UserChat,wDataSize);
			}			

			//�����Թ����
			SendLookonData(INVALID_CHAIR, SUB_GF_USER_CHAT, &UserChat, wDataSize, MDM_GF_FRAME);

			return true;
		}
	case SUB_GF_USER_VOICE:		//�û�����
		{
			//Ч�����
			ASSERT(wDataSize>=sizeof(CMD_GF_SendVoice));
			if (wDataSize<sizeof(CMD_GF_SendVoice)) return false;

			//��ȡ����
			CMD_GF_SendVoice * pSendVoice=(CMD_GF_SendVoice *)pData;

			//��������
			if (CServerRule::IsAllowVoiceChat(m_pGameServiceOption->dwServerRule)==false)
			{
				//��Ϣ����
				SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0077,L"��Ǹ��������δ�����������칦�ܣ�"),SMT_EJECT|SMT_CHAT| SMT_PROMPT);
				return true;
			}

			//Ŀ���û�
			if ((pSendVoice->dwTargetUserID!=0)&&(SearchUserItem(pSendVoice->dwTargetUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}
			
			//��������
			BYTE cbDataBuffer[SOCKET_PACKET];			
			CMD_GF_UserVoice * pUserVoice=(CMD_GF_UserVoice *)cbDataBuffer;
			
			//����ṹ
			pUserVoice->dwSendUserID=pIServerUserItem->GetUserID();
			CopyMemory((pUserVoice+1),(pSendVoice+1),sizeof(cbDataBuffer)-sizeof(CMD_GF_UserVoice));

			//�����С
			WORD wSendSize=wDataSize-sizeof(CMD_GF_SendVoice)+sizeof(CMD_GF_UserVoice);

			//��������
			WORD wEnumIndex=0;	
			IServerUserItem * pISendUserItem=NULL;

			//��Ϸ�û�
			for (WORD i=0;i<m_wChairCount;i++)
			{
				//��ȡ�û�
				pISendUserItem=GetTableUserItem(i);
				if ((pISendUserItem==NULL)||(pISendUserItem->IsClientReady()==false)) continue;
				if (pSendVoice->dwTargetUserID!=0 && pISendUserItem->GetUserID()!=pSendVoice->dwTargetUserID) continue;

				//��������
				m_pIServiceFunction->SendDataToUser(pISendUserItem,MDM_GF_FRAME,SUB_GF_USER_VOICE,&cbDataBuffer,wSendSize);
			}	

			//�����Թ����
			SendLookonData(INVALID_CHAIR, SUB_GF_USER_VOICE, &cbDataBuffer, wSendSize, MDM_GF_FRAME);			

			return true;
		}
	case SUB_GF_LOOKON_CONFIG:	//�Թ�����
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GF_LookonConfig));
			if (wDataSize<sizeof(CMD_GF_LookonConfig)) return false;

			//��������
			CMD_GF_LookonConfig * pLookonConfig=(CMD_GF_LookonConfig *)pData;

			//Ŀ���û�
			if ((pLookonConfig->dwUserID!=0)&&(SearchUserItem(pLookonConfig->dwUserID)==NULL))
			{
				ASSERT(FALSE);
				return true;
			}

			//�û�Ч��
			ASSERT(pIServerUserItem->GetUserStatus()!=US_LOOKON);
			if (pIServerUserItem->GetUserStatus()==US_LOOKON) return false;

			//�Թ۴���
			if (pLookonConfig->dwUserID!=0L)
			{
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//��ȡ�û�
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetUserID()!=pLookonConfig->dwUserID) continue;
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//������Ϣ
					CMD_GF_LookonStatus LookonStatus;
					LookonStatus.bAllowLookon=pLookonConfig->bAllowLookon;

					//������Ϣ
					ASSERT(m_pIServiceFunction!=NULL);
					m_pIServiceFunction->SendDataToUser(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));

					break;
				}
			}
			else
			{
				//�����ж�
				bool bAllowLookon=(pLookonConfig->bAllowLookon==TRUE)?true:false;
				if (bAllowLookon==m_bAllowLookon[pIServerUserItem->GetChairID()]) return true;

				//���ñ���
				m_bAllowLookon[pIServerUserItem->GetChairID()]=bAllowLookon;

				//������Ϣ
				CMD_GF_LookonStatus LookonStatus;
				LookonStatus.bAllowLookon=pLookonConfig->bAllowLookon;

				//������Ϣ
				for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
				{
					//��ȡ�û�
					IServerUserItem * pILookonUserItem=m_LookonUserItemArray[i];
					if (pILookonUserItem->GetChairID()!=pIServerUserItem->GetChairID()) continue;

					//������Ϣ
					ASSERT(m_pIServiceFunction!=NULL);
					m_pIServiceFunction->SendDataToUser(pILookonUserItem,MDM_GF_FRAME,SUB_GF_LOOKON_STATUS,&LookonStatus,sizeof(LookonStatus));
				}
			}

			return true;
		}
	case SUB_GF_USER_LOOK2SIT:	//������Ϣ
		{
			//״̬�ж�
			ASSERT (pIServerUserItem->GetUserStatus()==US_LOOKON) ;
			if (pIServerUserItem->GetUserStatus()!=US_LOOKON) return true;

			//��������
			CMD_GF_Look2Sit *pLook2Sit=(CMD_GF_Look2Sit *)pData;

			//Ч�����
			ASSERT(pLook2Sit->wChairID<m_wChairCount);
			if(pLook2Sit->wChairID>=m_wChairCount) return true;

			//ִ�ж���
			if( PerformStandUpAction(pIServerUserItem) )
				PerformSitDownAction(pLook2Sit->wChairID,pIServerUserItem);

			return true;
		}
	case SUB_GF_USER_SIT2LOOK:	//վ����Ϣ
		{	
			//״̬�ж�
			ASSERT (pIServerUserItem->GetUserStatus()==US_SIT || pIServerUserItem->GetUserStatus()==US_PLAYING) ;
			if (pIServerUserItem->GetUserStatus()!=US_SIT && pIServerUserItem->GetUserStatus()!=US_PLAYING) return true;

			//��������
			CMD_GF_Sit2Look *pSit2Look=(CMD_GF_Sit2Look *)pData;

			//Ч�����
			ASSERT(pSit2Look->wChairID<m_wChairCount);
			if(pSit2Look->wChairID>=m_wChairCount) return true;

			//ִ�ж���
			if( PerformStandUpAction(pIServerUserItem) )
				PerformLookonAction(pSit2Look->wChairID,pIServerUserItem);

			return true;
		}		
	}

	return false;
}

//��ȡ��λ
WORD CTableFrameItem::GetNullChairID()
{
	//��������
	for (WORD i=0;i< m_wMaxPlayerCount;i++)
	{
		if (m_TableUserItemArray[i]==NULL)
		{
			return i;
		}
	}

	return INVALID_CHAIR;
}

//�����λ
WORD CTableFrameItem::GetRandNullChairID()
{
	//��������
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

//�û���Ŀ
WORD CTableFrameItem::GetSitUserCount()
{
	//��������
	WORD wUserCount=0;

	//��Ŀͳ��
	for (WORD i=0;i< m_wMaxPlayerCount;i++)
	{
		if (GetTableUserItem(i)!=NULL) wUserCount++;
	}

	return wUserCount;
}

//�Թ���Ŀ
WORD CTableFrameItem::GetLookonUserCount()
{
	//��ȡ��Ŀ
	INT_PTR nLookonCount=m_LookonUserItemArray.GetCount();

	return (WORD)(nLookonCount);
}

//������Ŀ
WORD CTableFrameItem::GetOffLineUserCount()
{
	//��������
	WORD wOffLineCount=0;

	//��������
	for (WORD i=0;i< m_wMaxPlayerCount;i++)
	{
		if (m_dwOffLineTime[i]!=0L) wOffLineCount++;
	}

	return wOffLineCount;
}

//���±���
VOID CTableFrameItem::SetSitdownGameMultiple(WORD wGameMultiple)
{
	m_wSitdownMultiple = wGameMultiple;
}

//��������
VOID CTableFrameItem::SetStandupGameMultiple(WORD wGameMultiple)
{
	m_wStandupMultiple = wGameMultiple;
}

//��������
WORD CTableFrameItem::GetRelationChairID(DWORD dwUserID)
{
	return INVALID_WORD;
}

//��������
tagRelationParameter * CTableFrameItem::GetRelationParameter()
{
	//����ṹ
	static tagRelationParameter RelationParameter = {0};

	return &RelationParameter;
}

//δ׼����Ŀ
WORD CTableFrameItem::GetUnReadyUserCount()
{
	//��������
	WORD wUnReadyCount=0;

	//δ׼������
	for (WORD i=0;i< m_wMaxPlayerCount;i++)
	{
		if (m_dwUserUnReadyTime[i]!=0) wUnReadyCount++;
	}

	return wUnReadyCount;
}

//������Ŀ
WORD CTableFrameItem::GetRealUserCount()
{
	//��������
	WORD wRealUserCount = 0;

	//��������
	for (WORD i = 0; i < m_wMaxPlayerCount; i++)
	{
		IServerUserItem* pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem != NULL && !pIServerUserItem->IsRobotUser()) wRealUserCount++;
	}

	return wRealUserCount;
}

//������Ŀ
WORD CTableFrameItem::GetRobotUserCount()
{
	//��������
	WORD wRobotUserCount=0;

	//��������
	for (WORD i=0;i< m_wMaxPlayerCount;i++)
	{
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem!=NULL && pIServerUserItem->IsRobotUser()==true) wRobotUserCount++;
	}

	return wRobotUserCount;
}

//��������
bool CTableFrameItem::InitTableFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter)
{
	//���ñ���
	m_wTableID=wTableID;
	m_lBaseScore=TableFrameParameter.pGameServiceOption->lCellScore;
	m_wChairCount=TableFrameParameter.pGameServiceAttrib->wChairCount;	
	m_wMaxPlayerCount=TableFrameParameter.pGameServiceAttrib->wChairCount;

	//���ò���	
	m_pGameServiceAttrib=TableFrameParameter.pGameServiceAttrib;
	m_pGameServiceOption=TableFrameParameter.pGameServiceOption;

	//����ӿ�
	m_pITimerEngine=TableFrameParameter.pITimerEngine;
	m_pIServiceFunction=TableFrameParameter.pIServiceFunction;
	m_pIGameVideoManager=TableFrameParameter.pIGameVideoManager;
	m_pIDataBaseEngine=TableFrameParameter.pIDataBaseEngineEngine;
	m_pIRobotUserManager=TableFrameParameter.pIRobotUserManager;	
	m_pIAssistDataBaseEngine=TableFrameParameter.pIAssistDataBaseEngine;		

	//�ص��ӿ�
	m_pITableEventSink = TableFrameParameter.pITableEventSink;

	//��������
	IGameServiceManager * pIGameServiceManager=TableFrameParameter.pIGameServiceManager;
	m_pITableFrameSink=(ITableFrameSink *)pIGameServiceManager->CreateTableFrameSink(IID_ITableFrameSink,VER_ITableFrameSink);

	//�����ж�
	if (m_pITableFrameSink==NULL)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	IUnknownEx * pITableFrame=QUERY_ME_INTERFACE(IUnknownEx);
	if (m_pITableFrameSink->InitTableFrameSink(pITableFrame)==false) return false;	

	//��չ�ӿ�
	m_pIGameStockQuery=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink, IGameStockQuery);
	m_pITableUserAction=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserAction);
	m_pIEventUserScore=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,IEventUserScore);
	m_pITableUserRequest = QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,ITableUserRequest);
	m_pIEventBaseScore=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,IEventBaseScore);
	m_pIEventStartEfficacy=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,IEventStartEfficacy);
	m_pIEventServiceCost=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,IEventServiceCost);
	m_pIEventCustomFrameTime=QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink,IEventCustomFrameTime);
	m_pITableControlSendCard = QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink, ITableControlSendCard);

	//�¼�֪ͨ
	OnEventTableFrameInit(TableFrameParameter);

	return true;
}

//���Կ�ʼ
bool CTableFrameItem::TryStartGame()
{
	//����״̬
	if (m_pITableEventSink)
	{
		m_pITableEventSink->OnEventTableStatus(QUERY_ME_INTERFACE(ITableFrameItem), TRUE);
	}

	return StartGame();
}

//У������
bool CTableFrameItem::ContrastPassword(LPCTSTR pszPassword)
{
	//Ч�����
	ASSERT (pszPassword != NULL);
	if (pszPassword == NULL) return false;

	bool bResult= lstrcmp(m_szTablePassword, pszPassword) == 0;

	if (bResult == false)
	{
		CTraceService::TraceString(TEXT("TablePassword:%ls,Password:%ls"), TraceLevel_Exception, m_szTablePassword, pszPassword);
	}

	return bResult;
}

//����״��
WORD CTableFrameItem::GetTableUserInfo(tagTableUserInfo & TableUserInfo)
{
	//���ñ���
	ZeroMemory(&TableUserInfo,sizeof(TableUserInfo));

	//�û�����
	for (WORD i=0;i<m_pGameServiceAttrib->wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pIServerUserItem=GetTableUserItem(i);
		if (pIServerUserItem==NULL) continue;

		//�û�����
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

		//׼���ж�
		if (pIServerUserItem->GetUserStatus()==US_READY)
		{
			TableUserInfo.wTableReadyCount++;
		}
	}

	//������Ŀ
	switch (m_GameStartMode)
	{
	case StartMode_AllReady:		//����׼��
		{
			TableUserInfo.wMinUserCount=2;
			if (m_pGameServiceOption->wLessStartPlayer>TableUserInfo.wMinUserCount)
			{
				TableUserInfo.wMinUserCount=m_pGameServiceOption->wLessStartPlayer;
			}
			break;
		}
	case StartMode_Symmetry:		//��Կ�ʼ
		{
			TableUserInfo.wMinUserCount=2;
			break;
		}
	case StartMode_TimeControl:	//ʱ�����
		{
			TableUserInfo.wMinUserCount=1;
			break;
		}
	default:						//Ĭ��ģʽ
		{
			TableUserInfo.wMinUserCount=m_pGameServiceAttrib->wChairCount;
			break;
		}
	}

	return TableUserInfo.wTableRobotCount+TableUserInfo.wTableUserCount;
}

//��������
bool CTableFrameItem::PerformStandUpAction(IServerUserItem * pIServerUserItem, bool bInitiative)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	ASSERT(pIServerUserItem->GetTableID()==m_wTableID);
	ASSERT(pIServerUserItem->GetChairID()<=m_wChairCount);

	//�û�����	
	WORD wChairID=pIServerUserItem->GetChairID();
	DWORD dwUserID=pIServerUserItem->GetUserID();
	BYTE cbUserStatus=pIServerUserItem->GetUserStatus();
	IServerUserItem * pITableUserItem=GetTableUserItem(wChairID);

	//�����¼�
	if (m_pITableUserRequest && !m_pITableUserRequest->OnTableUserReqStandUp(pIServerUserItem, pIServerUserItem != pITableUserItem))
	{
		return false;
	}

	//�۷����
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

			//���ּ�¼
			m_ScoreInfoArray[wChairID].lScore -= lRevenue;
			m_ScoreInfoArray[wChairID].lRevenue += lRevenue;
			m_ScoreInfoArray[wChairID].lBeforScore = pIServerUserItem->GetUserScore();

			//д�����
			pIServerUserItem->WriteUserScore(-lRevenue,0,0,lRevenue,ScoreType_Service);
		}
	}

	//����У��
	if (!EfficacyUserStandup(wChairID, pIServerUserItem, bInitiative))
	{
		return true;
	}	

	//��������
	if (pIServerUserItem==pITableUserItem)
	{
		//���ñ���
		m_dwLastLeaveTime=(DWORD)time(NULL);
		m_TableUserItemArray[wChairID]=NULL;		

		//�¼�֪ͨ
		if (m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserStandUp(pIServerUserItem,false);

		//�����¼�
		OnActionUserStandup(pIServerUserItem);

		//�û�״̬
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetUserStatus((cbUserStatus==US_OFFLINE)?US_NULL:US_FREE,INVALID_TABLE,INVALID_CHAIR);

		//��������
		bool bTableLocked=IsTableLocked();
		bool bTableStarted=IsTableStarted();
		WORD wTableUserCount=GetSitUserCount();

		//����������Ϣ
		if (wTableUserCount==0 && (m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE)==0)
		{
			//���ñ���
			m_dwPlayCount=0;
			m_dwTableOwnerID=0L;
			m_szTablePassword[0]=0;						

			//�Ǳ�������
			if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
			{
				//����׷�
				SetCellScore(m_pGameServiceOption->lCellScore);			
			}
		}		

		//��������
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

		//����״̬
		if( bTableLocked != IsTableLocked() && m_pITableEventSink )
			m_pITableEventSink->OnEventTableLocked(QUERY_ME_INTERFACE(ITableFrameItem),IsTableLocked()?TRUE:FALSE);

		if( bTableStarted != IsTableStarted() && m_pITableEventSink )
			m_pITableEventSink->OnEventTableStatus(QUERY_ME_INTERFACE(ITableFrameItem),IsTableStarted()?TRUE:FALSE);

		//����״̬
		if ((bTableLocked!=IsTableLocked())||(bTableStarted!=IsTableStarted())) SendTableStatus();

		//���ñ���
		m_dwUserUnReadyTime[wChairID]=0;

		//��ʼ�ж�
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)==0)
		{
			//��ʼģʽ�ж�
			if (m_GameStartMode == StartMode_MasterControl)
			{
				//����״̬
				SendStartStatus();			
			}
			else
			{
				//��ʼ��Ϸ
				if (EfficacyStartGame(INVALID_CHAIR) == true)
				{
					TryStartGame();
				}
			}
		}

		//����ģʽ
		if (m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE && m_pGameServiceOption->cbDistributeMode==DISTRIBUTE_MODE_DELAYWAIT)
		{
			//��Ϸ״̬
			if (IsGameStarted()==false && m_dwPlayCount==0)
			{
				//����״��			
				tagTableUserInfo TableUserInfo;
				WORD wUserSitCount=GetTableUserInfo(TableUserInfo);

				//���·���
				if (wUserSitCount<TableUserInfo.wMinUserCount)
				{
					for (int i=0; i<m_wChairCount; i++)
					{
						if (m_TableUserItemArray[i]!=NULL)
						{
							//������Ϣ
							SendGameMessage(m_TableUserItemArray[i], CWHService::FormatMessage(MIM_SERVER_0084, L"��������뿪��ϵͳ���·������ӣ����Ժ�"),SMT_CHAT);

							//������Ϣ
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
		//��������
		for (INT_PTR i=0;i<m_LookonUserItemArray.GetCount();i++)
		{
			if (pIServerUserItem==m_LookonUserItemArray[i])
			{
				//ɾ������
				m_LookonUserItemArray.RemoveAt(i);

				//�¼�֪ͨ
				if (m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserStandUp(pIServerUserItem,true);

				//�û�״̬
				pIServerUserItem->SetClientReady(false);
				pIServerUserItem->SetUserStatus(US_FREE,INVALID_TABLE,INVALID_CHAIR);

				return true;
			}
		}

		//�������
		ASSERT(FALSE);
	}

	return true;
}

//�Թ۶���
bool CTableFrameItem::PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT((pIServerUserItem!=NULL)&&(wChairID<m_wChairCount));
	ASSERT((pIServerUserItem->GetTableID()==INVALID_TABLE)&&(pIServerUserItem->GetChairID()==INVALID_CHAIR));

	//��������
	auto pUserInfo=pIServerUserItem->GetUserInfo();
	auto pUserRule=pIServerUserItem->GetUserRule();
	
	//�Թۼ���
	if (EfficacyUserLookon(wChairID, pIServerUserItem) == false)
	{
		return false;
	}	

	//�����¼�
	if (m_pITableUserRequest && !m_pITableUserRequest->OnTableUserReqSitDown(pIServerUserItem, true))
	{
		return false;
	}

	//�����û�
	m_LookonUserItemArray.Add(pIServerUserItem);

	//�û�״̬
	pIServerUserItem->SetClientReady(false);
	pIServerUserItem->SetUserStatus(US_LOOKON,m_wTableID,wChairID);

	//����ʱ��
	((CServerUserItem *)pIServerUserItem)->SetLookonTime((DWORD)time(NULL));

	//�¼�֪ͨ
	if (m_pITableUserAction!=NULL) m_pITableUserAction->OnActionUserSitDown(pIServerUserItem,true);	

	return true;
}

//���¶���
bool CTableFrameItem::PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	//Ч�����
	ASSERT ((pIServerUserItem!=NULL)&&(wChairID<m_wChairCount));
	if ((pIServerUserItem == NULL) || (wChairID >= m_wChairCount)) return false;

	//У��״̬
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

	//��������	
	auto pUserInfo=pIServerUserItem->GetUserInfo();
	auto pUserRule=pIServerUserItem->GetUserRule();
	auto pITableUserItem=GetTableUserItem(wChairID);

	//����У��
	if (!EfficacyUserSitdown(wChairID, pIServerUserItem))
	{
		return false;
	}		

	//�����¼�
	if (m_pITableUserRequest && !m_pITableUserRequest->OnTableUserReqSitDown(pIServerUserItem, false))
	{
		return false;
	}

	//�Թ۴���
	if (cbLastStatus == US_LOOKON)
	{
		//��������
		for (INT_PTR i = 0; i < m_LookonUserItemArray.GetCount(); i++)
		{
			if (pIServerUserItem == m_LookonUserItemArray[i])
			{
				//ɾ������
				m_LookonUserItemArray.RemoveAt(i);

				//�¼�֪ͨ
				if (m_pITableUserAction != NULL) m_pITableUserAction->OnActionUserStandUp(pIServerUserItem, true);

				break;
			}
		}
	}
	
	//���ñ���
	m_dwLastEnterTime=(DWORD)time(NULL);
	m_TableUserItemArray[wChairID]=pIServerUserItem;

	//�û�״̬	
	if (m_bTableStarted==true && m_bGameStarted && /*m_GameStartMode!=StartMode_TimeControl &&*/ m_pGameServiceAttrib->wChairCount==MAX_CHAIR)
	{
		//����״̬
		pIServerUserItem->SetClientReady(false);
		pIServerUserItem->SetUserStatus(US_PLAYING,m_wTableID,wChairID);

		//�������
		for (WORD i = 0; i < m_wChairCount; i++)
		{
			if (m_dwPlayerID[i] == pIServerUserItem->GetUserID())
			{
				m_dwPlayerID[i] = 0;
				break;
			}
		}

		//���ñ�ʶ
		m_dwPlayerID[wChairID]=pIServerUserItem->GetUserID();
	}
	else
	{
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH) == 0)
		{
			//����״̬
			pIServerUserItem->SetClientReady(false);			
			pIServerUserItem->SetUserStatus(US_SIT, m_wTableID, wChairID);			
		}
		else
		{
			//׼��״̬
			pIServerUserItem->SetClientReady(pIServerUserItem->IsTrusteeUser());
			pIServerUserItem->SetUserStatus(US_READY, m_wTableID, wChairID);
		}		
	}

	//������Ϣ
	if (GetSitUserCount()==1)
	{
		//״̬����
		bool bTableLocked=IsTableLocked();

		//��������
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE)==0)
		{
			//���ñ���
			m_dwTableOwnerID=pIServerUserItem->GetUserID();			
			StringCchCopy(m_szTablePassword,CountArray(m_szTablePassword), pUserRule->szPassword);
		}		

		if (bTableLocked!=IsTableLocked()) 
		{
			//�����¼�
			if( m_pITableEventSink )
				m_pITableEventSink->OnEventTableLocked(QUERY_ME_INTERFACE(ITableFrameItem),IsTableLocked()?TRUE:FALSE);

			//����״̬
			SendTableStatus();
		}
	}

	//��ʼģʽ�ж�
	if (((m_pGameServiceOption->wServerType&(GAME_GENRE_MATCH|GAME_GENRE_BATTLE))==0) && (m_GameStartMode==StartMode_AllReady || m_GameStartMode==StartMode_FullReady))
	{
		//��¼ʱ��
		m_dwUserUnReadyTime[wChairID]=(DWORD)time(NULL);		
		if (GetUnReadyUserCount()==1) 
		{
			//��ȡʱ��
			DWORD dwElapse=TIME_KICKOUT_PLAYER;
			if (m_pIEventCustomFrameTime!=NULL)
			{
				dwElapse=m_pIEventCustomFrameTime->GetUnReadyKickOutTime();
			}

			//����ʱ��
			SetGameTimer(IDI_KICKOUT_PLAYER,dwElapse*1000L,1,wChairID);
		}
	}		

	//����֪ͨ
	if (m_pITableUserAction != NULL)
	{
		//����ʧ��
		if (m_pITableUserAction->OnActionUserSitDown(pIServerUserItem, false)==false)
		{
			PerformStandUpAction(pIServerUserItem,false);

			return false;
		}
	}

	//�����û�
	if (pIServerUserItem->IsRobotUser() == true)
	{
		//���һ���
		auto pRobotUserItem = (CRobotUserItem*)m_pIRobotUserManager->SearchRobotUserItem(pIServerUserItem->GetUserID());
		if (pRobotUserItem != NULL)
		{
			//�󶨽ӿ�
			pRobotUserItem->SetTableFrameItem(this);
		}
	}

	//��ʼģʽ�ж�
	if (m_GameStartMode == StartMode_MasterControl && m_pIEventStartEfficacy!=NULL)
	{
		//����ԱУ��
		if (m_pIEventStartEfficacy->GetMasterChairID() == wChairID)
		{
			//����ṹ
			CMD_GF_StartStatus StartStatus;
			ZeroMemory(&StartStatus, sizeof(StartStatus));

			//����״̬
			StartStatus.bEnabled = false;
			StartStatus.bVisibled = true;

			//��������
			m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_FRAME, SUB_GF_START_STATUS, &StartStatus, sizeof(StartStatus));
		}
	}	

	//�����¼�
	OnActionUserSitdown(pIServerUserItem);	

	return true;
}

//��������
bool CTableFrameItem::PerformConnectAction(IServerUserItem *pIServerUserItem)
{
	//Ч�����
	ASSERT(pIServerUserItem!=NULL);
	WORD wTableID = pIServerUserItem->GetTableID();
	WORD wChairID = pIServerUserItem->GetChairID();
	ASSERT(wChairID<m_wChairCount&&wTableID==m_wTableID);

	//��������
	IServerUserItem *pTableUserItem = GetTableUserItem(wChairID);
	ASSERT(pTableUserItem==pIServerUserItem&&pIServerUserItem->GetUserStatus()==US_OFFLINE);

	//�����¼�
	if (m_pITableUserRequest && !m_pITableUserRequest->OnTableUserReqConnect(pIServerUserItem))
	{
		return false;
	}

	//�¼�֪ͨ
	if (m_pITableUserAction!=NULL)
	{
		m_pITableUserAction->OnActionUserConnect(pIServerUserItem);
	}

	return true;
}

//����״̬
bool CTableFrameItem::SendTableStatus()
{
	//�������
	CMD_GR_TableStatus TableStatus;
	TableStatus.wTableID=m_wTableID;
	TableStatus.TableStatus.cbTableLock=IsTableLocked()?TRUE:FALSE;
	TableStatus.TableStatus.cbPlayStatus=IsTableStarted()?TRUE:FALSE;

	//��������
	m_pIServiceFunction->SendBatchData(BG_COMPUTER,MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	//�ֻ�����
	m_pIServiceFunction->SendDataToTable(m_wTableID, CLIENT_KIND_MOBILE, MDM_GR_STATUS,SUB_GR_TABLE_STATUS,&TableStatus,sizeof(TableStatus));

	return true;
}

//��ʼ״̬
bool CTableFrameItem::SendStartStatus()
{
	//��֤�ӿ�
	ASSERT (m_pIEventStartEfficacy != NULL);
	if (m_pIEventStartEfficacy == NULL) return false;

	//У��״̬
	ASSERT (m_cbGameStatus == GAME_STATUS_FREE);
	if (m_cbGameStatus != GAME_STATUS_FREE) return false;

	//��������
	WORD wMasterUserID = 0;
	bool bStartGame = EfficacyStartGame(INVALID_CHAIR);

	//��ȡ����Ա
	WORD wMasterChairID = m_pIEventStartEfficacy->GetMasterChairID();
	if (wMasterChairID != INVALID_CHAIR)
	{
		//��ȡ�û���Ϣ
		IServerUserItem * pIMasterUserItem = GetTableUserItem(wMasterChairID);
		if (pIMasterUserItem != NULL)
		{
			//���ñ�ʶ
			wMasterUserID = pIMasterUserItem->GetUserID();
			
			//����ṹ
			CMD_GF_StartStatus StartStatus;
			ZeroMemory(&StartStatus, sizeof(StartStatus));

			//����״̬
			StartStatus.bVisibled = true;
			StartStatus.bEnabled = bStartGame;

			//��������
			m_pIServiceFunction->SendDataToUser(pIMasterUserItem, MDM_GF_FRAME, SUB_GF_START_STATUS, &StartStatus, sizeof(StartStatus));
		}
	}	

	//���ҷ���
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem = GetTableUserItem(i);
		if (pITableUserItem == NULL) continue;
		if (pITableUserItem->GetUserID() == wMasterUserID) continue;

		//�������ϰ塢����Ա
		if (pITableUserItem->GetUserID() == m_dwTableOwnerID)
		{
			//����ṹ
			CMD_GF_StartStatus StartStatus;
			ZeroMemory(&StartStatus, sizeof(StartStatus));

			//����״̬
			StartStatus.bVisibled = true;
			StartStatus.bEnabled = bStartGame;

			//��������
			m_pIServiceFunction->SendDataToUser(pITableUserItem, MDM_GF_FRAME, SUB_GF_START_STATUS, &StartStatus, sizeof(StartStatus));
		}
	}	

	return true;
}

//����ʧ��
bool CTableFrameItem::SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, BYTE cbFailureCode)
{
	//��������
	CMD_GR_RequestFailure RequestFailure;
	RequestFailure.cbFailureCode=cbFailureCode;
	StringCchCopy(RequestFailure.szDescribeString,CountArray(RequestFailure.szDescribeString), pszDescribe);

	//��������
	WORD wHeadSize=sizeof(RequestFailure)-sizeof(RequestFailure.szDescribeString);
	WORD wSendSize=wHeadSize+CountStringBuffer(RequestFailure.szDescribeString);
	m_pIServiceFunction->SendDataToUser(pIServerUserItem,MDM_GR_USER,SUB_GR_REQUEST_FAILURE,&RequestFailure,wSendSize);

	return true;
}

//�汾���
bool CTableFrameItem::PerformCheckVersion(DWORD dwFrameVersion, DWORD dwClientVersion, IServerUserItem * pIServerUserItem)
{
	//��������
	bool bMustUpdatePlaza=false;
	bool bMustUpdateClient=false;

	//��ܰ汾
	if (dwFrameVersion!=0)
	{
		if (GetMainVer(dwFrameVersion)!=GetMainVer(VERSION_FRAME)) bMustUpdatePlaza=true;
		if (GetProductVer(dwFrameVersion)!=GetProductVer(VERSION_FRAME)) bMustUpdatePlaza=true;
	}

	//��Ϸ�汾
	if (GetMainVer(dwClientVersion)!=GetMainVer(m_pGameServiceAttrib->dwClientVersion)) bMustUpdateClient=true;
	if (GetProductVer(dwClientVersion)!=GetProductVer(m_pGameServiceAttrib->dwClientVersion)) bMustUpdateClient=true;

	//����֪ͨ
	if ((bMustUpdatePlaza==true)||(bMustUpdateClient==true))
	{
		//��������
		CMD_GR_UpdateNotify UpdateNotify;
		ZeroMemory(&UpdateNotify,sizeof(UpdateNotify));

		//��������
		UpdateNotify.cbAdviceUpdateClient=FALSE;
		UpdateNotify.cbMustUpdatePlaza=bMustUpdatePlaza;
		UpdateNotify.cbMustUpdateClient=bMustUpdateClient;

		//��ǰ�汾
		UpdateNotify.dwCurrentPlazaVersion=VERSION_PLAZA;
		UpdateNotify.dwCurrentFrameVersion=VERSION_FRAME;
		UpdateNotify.dwCurrentClientVersion=m_pGameServiceAttrib->dwClientVersion;

		//������Ϣ
		m_pIServiceFunction->SendDataToUser(pIServerUserItem,MDM_GR_LOGON,SUB_GR_UPDATE_NOTIFY,&UpdateNotify,sizeof(UpdateNotify));

		return false;
	}

	return true;
}

//��ʼЧ��
bool CTableFrameItem::EfficacyStartGame(WORD wReadyChairID)
{
	//״̬�ж�
	if (m_bGameStarted==true) return false;

	//ģʽ����
	if (m_GameStartMode==StartMode_TimeControl) return false;	

	//׼������
	WORD wReadyUserCount=0;
	for (WORD i=0;i<m_wChairCount;i++) 
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem=GetTableUserItem(i);
		if (pITableUserItem==NULL) continue;

		//�û�ͳ��
		if (pITableUserItem!=NULL)
		{
			//״̬�ж�
			if (pITableUserItem->IsClientReady() == false)
			{				
				return false;
			}
			if ((wReadyChairID!=i)&&(pITableUserItem->GetUserStatus()!=US_READY)) 
			{
				//׼����֤
				if (m_pIEventStartEfficacy!=NULL)
				{
					if (m_pIEventStartEfficacy->IsAllowPlayerReady(i)==false)
					{
						continue;
					}
				}

				return false;
			}

			//�û�����
			wReadyUserCount++;
		}
	}

	//��ʼ����
	switch (m_GameStartMode)
	{
	case StartMode_AllReady:		//����׼��
		{
			//��������
			WORD wStartCount=2;

			//��ȡ����
			if (m_pIEventStartEfficacy!=NULL)
			{
				wStartCount=__max(m_pIEventStartEfficacy->GetStartPlayerCount(),2);
			}

			return wReadyUserCount>=wStartCount;
		}
	case StartMode_Symmetry:		//�Գƿ�ʼ
		{
			//��Ŀ�ж�
			if (wReadyUserCount==m_wChairCount) return true;
			if ((wReadyUserCount<2L)||(wReadyUserCount%2)!=0) return false;

			//λ���ж�
			for (WORD i=0;i<m_wChairCount/2;i++)
			{
				//��ȡ�û�
				IServerUserItem * pICurrentUserItem=GetTableUserItem(i);
				IServerUserItem * pITowardsUserItem=GetTableUserItem(i+m_wChairCount/2);

				//λ�ù���
				if ((pICurrentUserItem==NULL)&&(pITowardsUserItem!=NULL)) return false;
				if ((pICurrentUserItem!=NULL)&&(pITowardsUserItem==NULL)) return false;
			}

			return true;
		}
	case StartMode_FullReady:		//���˿�ʼ
		{
			//��������
			WORD wStartCount=m_wChairCount;

			//��ȡ����
			if (m_pIEventStartEfficacy!=NULL)
			{
				wStartCount=m_pIEventStartEfficacy->GetStartPlayerCount();
			}

			return wReadyUserCount==wStartCount;
		}
	case StartMode_MasterControl:	//�������
		{
			//��������
			WORD wStartCount = 2;

			//��ȡ����
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

//��ַЧ��
bool CTableFrameItem::EfficacyAddrRule(IServerUserItem * pIServerUserItem)
{
	//����Ч��
	if (CServerRule::IsForbidIPLimit(m_pGameServiceOption->dwServerRule)) return true;

	//��������
	DWORD dwUserIP=pIServerUserItem->GetClientIP();

	//��ַЧ��
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		IServerUserItem * pITableUserItem=GetTableUserItem(i);

		if ((pITableUserItem!=NULL)&&(pITableUserItem->GetClientIP()==dwUserIP))
		{
			if(pIServerUserItem->GetUserRule()->bLimitSameIP)
			{
				LPCTSTR pszDescribe= CWHService::FormatMessage(MIM_SERVER_0094, L"�������˲�����ͬ IP ��ַ�������Ϸ������Ϸ���������� IP ��ַ��ͬ����ң����ܼ�����Ϸ��");
				SendRequestFailure(pIServerUserItem,pszDescribe,0);
				return false;
			}
			else if(pITableUserItem->GetUserRule()->bLimitSameIP)
			{
				LPCTSTR pszDescribe= CWHService::FormatMessage(MIM_SERVER_0094, L"���������˲�����ͬ IP ��ַ�������Ϸ������Ϸ���������� IP ��ַ��ͬ����ң����ܼ�����Ϸ��") ;
				SendRequestFailure(pIServerUserItem,pszDescribe,0);
				return false;			
			}
		}
	}
	return true;
}

//����Ч��
bool CTableFrameItem::EfficacyScoreRule(IServerUserItem * pIServerUserItem)
{
	//��������
	auto pUserInfo=pIServerUserItem->GetUserInfo();
	auto wFleeRate = pIServerUserItem->GetUserFleeRate();
	auto wAddupWinRate = pIServerUserItem->GetUserWinRate();

	//���ַ�Χ
	for (WORD i=0;i<m_wChairCount;i++)
	{
		//��ȡ�û�
		auto pITableUserItem=GetTableUserItem(i);

		//����Ч��
		if (pITableUserItem!=NULL)
		{
			//��ȡ����
			auto pTableUserRule=pITableUserItem->GetUserRule();

			//����Ч��
			if ((pTableUserRule->bLimitFleeRate)&&(wFleeRate>pTableUserRule->wMaxFleeRate) && !CServerRule::IsForbidFleeRateSet(m_pGameServiceOption->dwServerRule))
			{
				LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0095, L"����������̫�ߣ��� %s ���õ����ò��������ܼ�����Ϸ��",pITableUserItem->GetAccounts());
				SendRequestFailure(pIServerUserItem, pszMessage,0);
				return false;
			}

			//ʤ��Ч��
			if ((pTableUserRule->bLimitWinRate)&&(wAddupWinRate<pTableUserRule->wLessWinRate) && !CServerRule::IsForbidWinRateSet(m_pGameServiceOption->dwServerRule))
			{
				LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0096, L"����ʤ��̫�ͣ��� %s ���õ����ò��������ܼ�����Ϸ��",pITableUserItem->GetAccounts());
				SendRequestFailure(pIServerUserItem, pszMessage,0);
				return false;
			}

			//����Ч��
			if (pTableUserRule->bLimitGameScore==true && !CServerRule::IsForbidScoreLimit(m_pGameServiceOption->dwServerRule))
			{
				//��������
				SCORE lMinGameScore=__min(pTableUserRule->lLessGameScore,pITableUserItem->GetUserScore());

				//��߻���
				if ((m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)==0 && pUserInfo->lScore>pTableUserRule->lMaxGameScore)
				{
					LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0097, L"���Ļ���̫�ߣ��� %s ���õ����ò��������ܼ�����Ϸ��",pITableUserItem->GetAccounts());
					SendRequestFailure(pIServerUserItem, pszMessage,0);
					return false;
				}

				//��ͻ���
				if (pUserInfo->lScore<lMinGameScore)
				{
					LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0098, L"���Ļ���̫�ͣ��� %s ���õ����ò��������ܼ�����Ϸ��",pITableUserItem->GetAccounts());
					SendRequestFailure(pIServerUserItem, pszMessage,0);
					return false;
				}
			}
		}
	}

	return true;
}

//����Ч��
bool CTableFrameItem::EfficacyTableRule(IServerUserItem * pIServerUserItem)
{
	//��������
	auto pUserInfo=pIServerUserItem->GetUserInfo();
	auto pUserRule=pIServerUserItem->GetUserRule();

	//�������
	SCORE lServiceFee=0;
	if (m_pGameServiceOption->lLessScore!=0 && CServerRule::IsChargeModeFee(m_pGameServiceOption->dwServerRule))
	{
		lServiceFee=m_pGameServiceOption->lRevenue;
	}

	//��ͷ���
	SCORE lLessScore = m_pGameServiceOption->lLessScore;	

	//��������
	SCORE lStidownScore = __max(m_lBaseScore*m_wSitdownMultiple,lLessScore)+lServiceFee;
	if (pIServerUserItem->GetUserScore() < lStidownScore)
	{
		//�����ʾ
		if (pIServerUserItem->GetScoreKind()==SCORE_KIND_GOLD)
		{
			LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0100, L"������Ϸ������Ҫ " SCORE_STRING " ����Ϸ�ң�������Ϸ�Ҳ��㲻�ܼ��룡", SCORE_ZOOMOUT(lStidownScore));
			SendRequestFailure(pIServerUserItem, pszMessage, RFC_SCORE_UNENOUGH);

			return false;
		}

		//���������ʾ
		if (pIServerUserItem->GetScoreKind() == SCORE_KIND_CLUB)
		{
			LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0101, L"������Ϸ������Ҫ " SCORE_STRING " �����֣����Ļ��ֲ��㲻�ܼ��룡", SCORE_ZOOMOUT(lStidownScore));
			SendRequestFailure(pIServerUserItem, pszMessage, RFC_SCORE_UNENOUGH);

			return false;
		}
	}		

	//��������
	if ((m_pGameServiceOption->lLessLockScore>0L)&&(pIServerUserItem->GetUserFreeScore()<lStidownScore-lServiceFee))
	{
		//�����ʾ
		if (pIServerUserItem->GetScoreKind() == SCORE_KIND_GOLD)
		{
			LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0100, L"������Ϸ������Ҫ" SCORE_STRING "����Ϸ�ң�������Ϸ�Ҳ��������ܼ��룡", SCORE_ZOOMOUT(lStidownScore));
			SendRequestFailure(pIServerUserItem, pszMessage,RFC_SCORE_UNENOUGH);
		}

		//������ʾ
		if (pIServerUserItem->GetScoreKind() == SCORE_KIND_CLUB)
		{
			LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0101, L"������Ϸ������Ҫ" SCORE_STRING "�����֣����Ļ��ֲ��������ܼ��룡", SCORE_ZOOMOUT(lStidownScore));
			SendRequestFailure(pIServerUserItem, pszMessage, RFC_SCORE_UNENOUGH);
		}

		return false;
	}

	//����Ч��
	if ( (IsTableLocked()==true) &&
		!CUserRight::CanEnterWithoutPasswd(pIServerUserItem->GetUserRight()) &&
		pIServerUserItem->GetMasterOrder() == 0 && !CServerRule::IsForbidPasswordSet(m_pGameServiceOption->dwServerRule))
	{
		if( pIServerUserItem->IsTmpPasswdEmpty() == false )
		{
			if( !pIServerUserItem->ContrastTempPassword(m_szTablePassword) )
			{
				//������ʱ����
				pIServerUserItem->SetTmpPassword(L"");

				SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0090, L"����������󣬲��ܼ�����Ϸ��"),RFC_TMP_PASSWD_INCORRECT);
				return false;
			}

			//������ʱ����
			pIServerUserItem->SetTmpPassword(L"");
		}
		else if( !pIServerUserItem->ContrastTablePassword(m_szTablePassword) )
		{
			SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0090, L"����������󣬲��ܼ�����Ϸ��"),RFC_PASSWORD_INCORRECT);
			return false;
		}
	}	

	//��������
	if ((m_pGameServiceOption->wServerType&TABLE_GENRE_DISTRIBUTE)==0 && GetSitUserCount()>=m_wMaxPlayerCount)
	{
		//������Ϣ
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessageV(MIM_SERVER_0103, L"����������ֻ���� %d ����Ϸ�������ܼ��룡", m_wMaxPlayerCount),RFC_NULL);

		return false;
	}

	return true;
}


//����ȼ�
VOID CTableFrameItem::CalcUserCardLevel()
{
	//���ñ���
	m_bControlCard = false;
	ZeroMemory(&m_cbUserCardLevel, sizeof(m_cbUserCardLevel));

	//��������
	WORD wItemCount = 0;
	WORD wUserCount[3] = { 0,0,0 };
	tagUserRateItem* pUserRateItem = NULL;
	IServerUserItem* pIServerUserItem = NULL;
	tagUserRateItem UserRateItem[TABLE_USER_COUNT] = {};

	//�����û�
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//��ȡ�û�
		pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;

		//��ȡ����
		pUserRateItem = &UserRateItem[wItemCount++];

		//���ñ���
		pUserRateItem->wChairID = pIServerUserItem->GetChairID();
		pUserRateItem->wAddupWinRate = pIServerUserItem->GetAddupWinRate();
		pUserRateItem->cbControlStatus = pIServerUserItem->GetRegulateStatus(0);

		//ͳ������
		wUserCount[pUserRateItem->cbControlStatus]++;
		if (wItemCount >= CountArray(UserRateItem)) break;
	}

	//����У��
	if (wUserCount[CONTROL_STATUS_WHITELIST] != 0 || wUserCount[CONTROL_STATUS_BLACKLIST] != 0)
	{
		//״̬Ȩ��
		WORD wStatusWight[] = { 2,3,1 };

		//��������
		for (int i = 1; i < wItemCount; i++)
		{
			//��������
			int nLeft = 0, nRight = i - 1;
			tagUserRateItem TempUserRateItem = UserRateItem[i];

			while (nLeft <= nRight)
			{
				int nMid = (nLeft + nRight) / 2;

				//״̬Ȩ��
				if (wStatusWight[UserRateItem[i].cbControlStatus] > wStatusWight[UserRateItem[nMid].cbControlStatus])
				{
					nRight = nMid - 1;
				}
				else if (UserRateItem[i].cbControlStatus == UserRateItem[nMid].cbControlStatus)
				{
					//�Ƚ�ʤ��
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

		//���ñ�ʶ
		m_bControlCard = true;

		//���ü���
		for (int i = 0; i < wItemCount; i++)  m_cbUserCardLevel[UserRateItem[i].wChairID] = i + 1;
	}
}

//��Ϸ��¼��ʶ
SCORE CTableFrameItem::BuildGameRecordID()
{
	//��̬����
	static WORD wRecordCount = 0;
	static DWORD dwLastSecond = (DWORD)time(NULL);

	//��������
	DWORD dwSecond = (DWORD)time(NULL);

	//���±���
	if (dwLastSecond != dwSecond)
	{
		wRecordCount = 0;
		dwLastSecond = dwSecond;
	}

	//��ʽ����ʶ
	TCHAR szRecordID[19];
	_sntprintf_s(szRecordID, CountArray(szRecordID), TEXT("%05d%10d%02d"), m_pGameServiceOption->wServerID, dwSecond, ++wRecordCount);

	//����ת��
	SCORE lRecordID = _tstoi64(szRecordID);

	return lRecordID;
}

//¼����
VOID CTableFrameItem::BuildVideoChannel(TCHAR szBuffer[], WORD wBufferLen)
{
	//��ȡʱ��
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime); 

	//��λ����
	DWORD dwSecond=SystemTime.wHour*3600+SystemTime.wMinute*60+SystemTime.wSecond;

	//��̬����
	static WORD wVideoCount=0;
	static DWORD dwLastSecond=dwSecond;	

	//���±���
	if (dwSecond!=dwLastSecond)
	{
		wVideoCount=0;
		dwLastSecond=dwSecond;
	}

	//�����ʶ
	WORD wServerID=m_pGameServiceOption->wServerID;

	//��ʽ�����
	_snwprintf_s(szBuffer,wBufferLen, wBufferLen,TEXT("%05d%04d%02d%02d%05d%03d"),wServerID,SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,dwSecond,++wVideoCount);
}

//������
bool CTableFrameItem::ControlSendCard(IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize)
{
	//��չЧ��
	if (m_pITableControlSendCard != NULL)
	{
		return m_pITableControlSendCard->OnEventControlSendCard(pIServerUserItem, pData, wDataSize);
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
