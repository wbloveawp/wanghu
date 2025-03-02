#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//��Ϸ����
#define	STOCK_TAX						2									//�ؿۺ���

//////////////////////////////////////////////////////////////////////////

#define IDI_OPEN_CARD			    1				//���ƶ�ʱ
#define IDI_OPERATE_TIMEOUT			2				//������ʱ

//ʱ���ʶ
#ifdef _DEBUG
#define TIME_DELAY					5				//��ʱʱ��
#define TIME_OPEN_CARD				30				//����ʱ��	
#define TIME_OPERATE_CARD			15				//������ʱ��	
#define TIME_DISMISS_TABLE			120				//��ɢʱ��
#define TIME_KICK_OUT_1				15				//����ʱ��
#define TIME_KICK_OUT_2				30				//����ʱ��
#else
#define TIME_DELAY					5				//��ʱʱ��
#define TIME_OPEN_CARD				30				//����ʱ��	
#define TIME_OPERATE_CARD			15				//������ʱ��	
#define TIME_DISMISS_TABLE			300				//��ɢʱ��
#define TIME_KICK_OUT_1				25				//����ʱ��
#define TIME_KICK_OUT_2				40				//����ʱ��
#endif

//////////////////////////////////////////////////////////////////////////


//���캯��
CTableFrameSink::CTableFrameSink()
{
	//��Ϸ����
	ZeroMemory(m_cbPlayerStatus,sizeof(m_cbPlayerStatus));
	ZeroMemory(m_cbShowHand,sizeof(m_cbShowHand));
	ZeroMemory(m_cbOpenFinish,sizeof(m_cbOpenFinish));
	ZeroMemory(m_bInvestFinish,sizeof(m_bInvestFinish));
	ZeroMemory(m_bUserReConnect,sizeof(m_bUserReConnect));

	m_wFirstUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	m_wOperaCount=0;
	m_cbBalanceCount=0;
	m_lBalanceScore=0;
	m_lMinAddScore=0;
	m_lTurnMaxScore = 0;
	m_lTurnMinScore = 0;
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lLoseScore,sizeof(m_lLoseScore));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lTotalScore,sizeof(m_lTotalScore));
	ZeroMemory(m_lUserFreeScore,sizeof(m_lUserFreeScore));

	//��������
	m_dwDelayTime=TIME_DELAY;

	//���ñ���
	ZeroMemory(&m_CustomConfig,sizeof(m_CustomConfig));

	//��������
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));	

	//�������
	m_pITableFrame=NULL;
	m_pIEventBattleSink =NULL;
	m_pIEventBattleSink=NULL;
	m_pGameServiceOption=NULL;	
	m_pIGameServiceDynamicRule=NULL;

	//�������
	m_bGameRule=false;
	m_wSetRuleUser=INVALID_CHAIR;

	//���Ʊ���
	m_bAllotCard=false;
	m_wControBankerUser=INVALID_CHAIR;
	ZeroMemory(m_cbControlCardData,sizeof(m_cbControlCardData));

	return;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
}

//�ӿڲ�ѯ--��������Ϣ�汾
void *  __cdecl CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(IEventBaseScore,Guid,dwQueryVer);
	QUERYINTERFACE(IEventUserScore,Guid,dwQueryVer);	
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE(ITableControlSendCard,Guid,dwQueryVer);
	QUERYINTERFACE(IEventBattleItem,Guid,dwQueryVer);
	QUERYINTERFACE(IEventStartEfficacy,Guid,dwQueryVer);	
	QUERYINTERFACE(IEventCustomFrameTime,Guid,dwQueryVer);	
#ifdef __SPECIAL___
	QUERYINTERFACE(ITableUserActionEX,Guid,dwQueryVer);	
#endif
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ��
bool  __cdecl CTableFrameSink::InitTableFrameSink(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(pIUnknownEx != NULL);
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrameItem);
	if (m_pITableFrame == NULL) return false;

	//��ѯ����
	if (m_pITableFrame->GetGameServiceOption()->wServerType & GAME_GENRE_BATTLE)
	{
		m_pIEventBattleSink = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IEventBattleSink);
		if (m_pIEventBattleSink == NULL)
		{
			CTraceService::TraceString(TEXT("��Ϸ���� CTableFrameSink ��ѯ IEventBattleSink �ӿ�ʧ��"), TraceLevel_Exception);
			return false;
		}
	}


	//��Ϸ����
	//m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();

	//��ȡ����
	tagCustomConfig * pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
	m_CustomConfig = *pCustomConfig;

	//���ù���
	m_GameLogic.SetRuleOption(m_CustomConfig.cbRuleOption);

	//��ʼģʽ
		//Լս����
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
	{
		m_pITableFrame->SetStartMode(StartMode_FullReady);
	}
	else
	{
		m_pITableFrame->SetStartMode(StartMode_AllReady);
	}


	//���ñ���	
	m_lCellScore = m_pGameServiceOption->lCellScore;
	m_bGameRule = (m_CustomConfig.cbRuleOption & GR_ALLOW_CUSTOM_SCORE) == 0;

	//��������
	if (m_CustomConfig.lMinBetAmount < m_lCellScore * 2)
	{
		m_CustomConfig.lMinBetAmount = m_lCellScore * 2;
	}

	//Լս����
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
	{
		m_dwDelayTime = TIME_DELAY + 1800;

		//���ñ���
		//if (m_pIGameServiceDynamicRule!=NULL) m_pIGameServiceDynamicRule->SetMaxGameMultiple(MIN_MUTIPLIE);
	}


	//���±���
	m_pITableFrame->SetSitdownGameMultiple(MIN_MUTIPLIE);

	//��������
	m_pITableFrame->SetStandupGameMultiple(MIN_MUTIPLIE);

	return true;
}

//��λ����
void  __cdecl CTableFrameSink::ResetTableFrameSink()
{
	//��Ϸ����
	ZeroMemory(m_cbPlayerStatus,sizeof(m_cbPlayerStatus));
	ZeroMemory(m_cbShowHand,sizeof(m_cbShowHand));
	ZeroMemory(m_cbOpenFinish,sizeof(m_cbOpenFinish));
	ZeroMemory(m_bInvestFinish,sizeof(m_bInvestFinish));

	//m_wBankerUser=INVALID_CHAIR;			
	m_wFirstUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;				

	m_wOperaCount=0;
	m_cbBalanceCount=0;
	m_lBalanceScore=0;
	m_lMinAddScore = 0;
	m_lTurnMaxScore = 0;
	m_lTurnMinScore = 0;
	ZeroMemory(m_lLoseScore,sizeof(m_lLoseScore));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_lTotalScore,sizeof(m_lTotalScore));

	//��������
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));
	ZeroMemory(m_cbControlCardData,sizeof(m_cbControlCardData));

	return;
}

////��ʼģʽ
//enStartMode  CTableFrameSink::GetGameStartMode()
//{
//	return m_GameStartMode;
//}

//��Ϸ״̬
bool  __cdecl CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	ASSERT (wChairID<m_wPlayerCount && (m_cbPlayerStatus[wChairID]==GUS_PLAYING || m_lLoseScore[wChairID]<0));
	if(wChairID<m_wPlayerCount && (m_cbPlayerStatus[wChairID]==GUS_PLAYING || m_lLoseScore[wChairID]<0)) return true;

	return false;
}

//��Ϸ��ʼ
bool  __cdecl CTableFrameSink::OnEventGameStart()
{
	//����״̬
	m_pITableFrame->SetGameStatus(GS_TK_INVEST);

	++m_wBattleCount;

	//�û�״̬
	WORD wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//��ȡ�û�
		IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);

		if (pIServerUser==NULL)
		{
			m_cbPlayerStatus[i]=GUS_NULL;
		}
		else
		{
			m_bUserReConnect[i]=FALSE;
			m_cbPlayerStatus[i]=GUS_PLAYING;
			if (GetUserScore(i)<m_lCellScore*MIN_MUTIPLIE)
			{
				m_cbPlayerStatus[i]=GUS_LOOKON;
			}

			//ͳ������
			if (m_cbPlayerStatus[i]==GUS_PLAYING) wPlayerCount++;
		}
	}

	//������Ϸ
	if (wPlayerCount<2)
	{
		//������Ϸ
		return m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
	}

	//��������
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_pITableFrame->GetTableUserItem(i)==NULL)continue;
		
		LONGLONG lUserScore=GetUserScore(i); 		
		m_lUserBeginScore[i]=lUserScore;
		if (m_lUserScore[i]>lUserScore)
		{
			m_lUserScore[i]=lUserScore;
		}
	}

	//����ׯ��
	if(m_bAllotCard)
	{
		for(WORD i=0;i<m_wPlayerCount;i++)
		{
			if (m_cbPlayerStatus[i]!=GUS_PLAYING)continue;
			m_wBankerUser=i;
			break;

		}
		//m_wBankerUser = m_wControBankerUser;
	}

	//�׾����
	if (m_wBankerUser==INVALID_CHAIR)
	{
		m_wBankerUser = rand()%m_wPlayerCount;
	}

	if(m_bAllotCard)
	{
		//ȷ��ׯ��
		while(m_cbPlayerStatus[m_wBankerUser]!=GUS_PLAYING)
		{
			m_wBankerUser =(m_wBankerUser+1)%m_wPlayerCount;
		} 
	}
	else if(m_bAllotCard==false)
	{
		do
		{
			m_wBankerUser =(m_wBankerUser+1)%m_wPlayerCount;
		} while(m_cbPlayerStatus[m_wBankerUser]!=GUS_PLAYING);
	}

//#ifdef _DEBUG
//		m_wBankerUser=m_wSetRuleUser;
//#endif


//#ifdef _DEBUG
//m_wBankerUser=1;
//#endif

	//��ǰ�û�
	m_wCurrentUser=m_wBankerUser;

	//���·���
	UpdateUserScore(INVALID_CHAIR);

	//���ñ���
	CMD_S_GameStart GameStart;
	GameStart.wBankerUser=m_wBankerUser;	
	CopyMemory(GameStart.lUserScore,m_lUserScore,sizeof(m_lUserScore));
	CopyMemory(GameStart.cbPlayStatus,m_cbPlayerStatus,sizeof(m_cbPlayerStatus));

	//��������
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_pITableFrame->GetTableUserItem(i)==NULL)continue;

		//���ͷ���
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

		//��ʱ��ʱ��
		SetUserGameTimer(i,25,m_dwDelayTime);
	}

	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	

	return true;
}

//��Ϸ����
bool  __cdecl CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//�������
		{
			//ȡ������
			if(m_bAllotCard)
			{
				m_bAllotCard=false;
				m_wControBankerUser=INVALID_CHAIR;
				ZeroMemory(m_cbControlCardData,sizeof(m_cbControlCardData));
			}

			//��������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			GameEnd.cbEndReason=END_REASON_NORMAL;
			CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(GameEnd.cbCardData));

			//��������
			WORD wGiveupCount=0;
			WORD wPlayerCount=0;

			//�жϷ���
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (m_cbPlayerStatus[i]==GUS_NULL && m_lTotalScore[i]>0)
				{
					wGiveupCount++;
				}
			}

			//�����÷�
			BYTE cbPlayCount=0;
			BYTE cbPlayerStatus[GAME_PLAYER];
			BYTE cbAllowFirster=(m_CustomConfig.cbRuleOption&GR_ALLOW_FIRSTER)?TRUE:FALSE;
			CopyMemory(cbPlayerStatus,m_cbPlayerStatus,sizeof(cbPlayerStatus));
			
			do
			{
				//��������
				BYTE cbSpeCount = 2;
				struct LoseInfo
				{
					WORD wPlyObjUser[GAME_PLAYER];
					WORD wPlyObjCount;
				}LoseUserInof[GAME_PLAYER];
				ZeroMemory(LoseUserInof,sizeof(LoseUserInof));

				//�ж����
				BYTE cbLoseUser[GAME_PLAYER];
				ZeroMemory(cbLoseUser,sizeof(cbLoseUser));

                for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (cbPlayerStatus[i]!=GUS_PLAYING)continue;

					for (WORD j=0;j<m_wPlayerCount;j++)
					{
						if (cbPlayerStatus[j]!=GUS_PLAYING || i==j) continue;

						//�Ƚ�����
						tagCompareResult CompareResult;
						if (CompareHandCard(i,j,CompareResult)==-1) 
						{
							cbLoseUser[i]=TRUE;
							break;
						}
					}
				}

				//�������   
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (cbPlayerStatus[i]!=GUS_PLAYING || cbLoseUser[i]==FALSE)continue;

					for (WORD t=0;t<m_wPlayerCount;t++)
					{
						//ׯ������
						WORD j=(t+m_wBankerUser)%GAME_PLAYER;
						if (cbPlayerStatus[j]!=GUS_PLAYING || i==j /*|| cbLoseUser[j]==TRUE*/)continue;

						//�Ƚ�����
						tagCompareResult CompareResult;
						if (CompareHandCard(i,j,CompareResult)==-1) 
						{
							LoseUserInof[i].wPlyObjUser[LoseUserInof[i].wPlyObjCount++] = j;
						}
					}
				}

				//��ҵ÷�
				LONGLONG lUserGetScore[GAME_PLAYER];
				ZeroMemory(lUserGetScore,sizeof(lUserGetScore));

				//��ע����
				LONGLONG lTotalScore[GAME_PLAYER];
				CopyMemory(lTotalScore,m_lTotalScore,sizeof(m_lTotalScore));

				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (cbPlayerStatus[i]!=GUS_PLAYING) continue;

					LoseInfo *pLoseInfo = &LoseUserInof[i];
					WORD wPlyCount=pLoseInfo->wPlyObjCount;

					//������
					LONGLONG lMaxLostScore=0;
					for (WORD j=0;j<wPlyCount;j++)
					{						
						WORD wCurUser = pLoseInfo->wPlyObjUser[j];
						if (cbPlayerStatus[wCurUser]!=GUS_PLAYING)continue;
						if (lTotalScore[wCurUser]>lMaxLostScore) 
						{							
							lMaxLostScore=lTotalScore[wCurUser];
						}
					}

					//��������					
					if (lTotalScore[i]<lMaxLostScore) lMaxLostScore=lTotalScore[i];

					//����÷�
					while (wPlyCount>0 && lMaxLostScore>0)
					{
						//���Ҵ�Ӯ��
						WORD wIndex = 0;
						WORD wMaxUser = pLoseInfo->wPlyObjUser[0];
						for (WORD j=1;j<wPlyCount;j++)
						{
							//��ǰ�û�
							WORD wCurUser = pLoseInfo->wPlyObjUser[j];
							if (cbPlayerStatus[wCurUser]!=GUS_PLAYING) continue;

							//�Ƚ�����
							tagCompareResult CompareResult;
							CompareHandCard(wMaxUser,wCurUser,CompareResult);

							//���С��
							if (CompareResult.nResult==-1) 
							{
								wIndex=j;
								wMaxUser=wCurUser;
							}

							//�������
							if (CompareResult.nResult==0 && CompareResult.nTailResult<0)
							{
								wIndex=j;
								wMaxUser=wCurUser;
							}
						}

						//����÷�						
						LONGLONG lTempScore=__min(lTotalScore[wMaxUser],lMaxLostScore);						
						lUserGetScore[wMaxUser]+=lTempScore;
						lMaxLostScore-=lTempScore;
						lUserGetScore[i]-=lTempScore;
						m_lTotalScore[i]-=lTempScore;						

						//�жϽ���
						if (lMaxLostScore==0 || wPlyCount==1) break;

						//�����û�
						WORD wTempUser[GAME_PLAYER],wTempCount=0;
						CopyMemory(wTempUser,pLoseInfo->wPlyObjUser,sizeof(WORD)*wPlyCount);
						wTempUser[wIndex]=INVALID_CHAIR;
						for (WORD j=0;j<wPlyCount;j++)
						{
							if (wTempUser[j]!=INVALID_CHAIR)
							{
								pLoseInfo->wPlyObjUser[wTempCount++]=wTempUser[j];
							}
						}

						ASSERT(wTempCount==wPlyCount-1);
						wPlyCount=wTempCount;
					}
				}

				//������
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					if (cbPlayerStatus[i]!=GUS_PLAYING) continue;
					GameEnd.lGameScore[i]+=lUserGetScore[i];
				}

			} while(false);

			///////////////////////////////////////////////��Ƥ����///////////////////////////////////////////

			//������ҽ���Ƥ�� 
			if (wGiveupCount>0)
			{
				//��������
				WORD wPlayerCount=0;
				WORD wMaxChairID=INVALID_CHAIR;
				BYTE cbWinnerOrder[GAME_PLAYER];
				BYTE cbCompareStatus[GAME_PLAYER];

				//���ñ���
				ZeroMemory(cbWinnerOrder,sizeof(cbWinnerOrder));
				CopyMemory(cbCompareStatus,m_cbPlayerStatus,sizeof(m_cbPlayerStatus));

				//β������
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					//���ñ���
					wMaxChairID=INVALID_CHAIR;

					for (WORD j=0;j<m_wPlayerCount;j++)
					{
						//״̬����
						if (cbCompareStatus[j]!=GUS_PLAYING) continue;

						//���ñ���
						if (wMaxChairID==INVALID_CHAIR) 
						{
							wMaxChairID=j;						
							continue;
						}

						//�Ƚ�β��
						INT nTailResult=m_GameLogic.CompareCard(m_cbHandCardData[j],m_cbHandCardData[wMaxChairID],2);
						if (nTailResult==0xFF) continue;

						//�������
						if (nTailResult==TRUE) wMaxChairID=j;

						//������
						if (nTailResult==FALSE)
						{
							//ׯ������
							if (cbAllowFirster==TRUE)
							{
								//ׯ������
								if (j==m_wBankerUser) wMaxChairID=j;					
								if (j!=m_wBankerUser && wMaxChairID!=m_wBankerUser)
								{
									//��ʱ���ׯ����
									WORD wChairDis1=(j+(j<m_wBankerUser?GAME_PLAYER:0))-m_wBankerUser;
									WORD wChairDis2=(wMaxChairID+(wMaxChairID<m_wBankerUser?GAME_PLAYER:0))-m_wBankerUser;

									//�Ƚ�λ��
									if (wChairDis1<wChairDis2) wMaxChairID=j;						
								}									
							}
							else
							{
								//�Ƚ�β��
								INT nHeadResult=m_GameLogic.CompareCard(&m_cbHandCardData[j][2],&m_cbHandCardData[wMaxChairID][2],2);
								if (nHeadResult==0xFF) continue;

								//�������
								if (nHeadResult==TRUE) wMaxChairID=j;

								//������
								if (nHeadResult==FALSE)
								{
								}
							}
						}					
					}

					//����״̬
					if (wMaxChairID!=INVALID_CHAIR)
					{
						cbCompareStatus[wMaxChairID]=GUS_NULL;
						cbWinnerOrder[wPlayerCount++]=wMaxChairID;
					}
				}

				//��Ƥ����
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					//�������
					if (m_cbPlayerStatus[i]!=GUS_PLAYING && m_lLoseScore[i]!=0)
					{
						//���÷���
						LONGLONG lTotalScore=__max(m_lLoseScore[i],-m_lLoseScore[i]);
						GameEnd.lGameScore[i]=-lTotalScore;		

						do
						{
							for (WORD j=0; j<wPlayerCount; j++)
							{
								//�������
								WORD wWinerID=cbWinnerOrder[j];
								LONGLONG lLoseScore=__min(lTotalScore,m_lUserScore[wWinerID]);

								//���÷���								
								GameEnd.lGameScore[wWinerID]+=lLoseScore;

								//���·���
								lTotalScore-=lLoseScore;
								m_lTotalScore[i]-=lLoseScore;

								if (lTotalScore==0 || m_lTotalScore[i]==0) break;
							}
						} while (lTotalScore>0 && m_lTotalScore[i]>0);											
					}
				}	
			}

			///////////////////////////////////////////////��Ƥ����///////////////////////////////////////////

			//����˰��
			LONGLONG lRevenue=m_pGameServiceOption->lRevenue;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (GameEnd.lGameScore[i]>0)
				{
					GameEnd.lGameTax[i]=m_pITableFrame->AccountRevenue(GameEnd.lGameScore[i]);
					
					if (GameEnd.lGameTax[i]>0)
						GameEnd.lGameScore[i]-=GameEnd.lGameTax[i];
				}

				//�û��ܱ�
				if (m_lUserScore[i]>0) m_lUserScore[i]+=GameEnd.lGameScore[i];
			}

			//�õ�����û�
			WORD wMaxUser=INVALID_CHAIR;

			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//ׯ������
				WORD j=(i+m_wBankerUser)%GAME_PLAYER;
				
				if (GameEnd.lGameScore[j]>0)
				{
					wMaxUser = j;
				}
			}

			GameEnd.wMaxUser=wMaxUser;
			
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

			//д�����
			for (WORD i=0;i<m_wPlayerCount;i++)
			{	
				IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserItem==NULL) continue;

				ScoreInfo[i].ScoreType=ScoreType_Draw;
				if (GameEnd.lGameScore[i]!=0)
				{
					ScoreInfo[i].ScoreType=(GameEnd.lGameScore[i]>0L)?ScoreType_Win:ScoreType_Lose;
				}
				ScoreInfo[i].lScore=GameEnd.lGameScore[i]+GameEnd.lGameTax[i];

				if (m_cbPlayerStatus[i]==GUS_LOOKON || m_cbPlayerStatus[i]==GUS_DYNAMIC)
				{
					ScoreInfo[i].ScoreType=ScoreType_Null;
				}

				//���·���								
				if ((m_pGameServiceOption->wServerType& GAME_GENRE_BATTLE)!=0)
					//&& pIServerUserItem->GetScoreKind()==SCORE_KIND_BATTLE)
				{
					m_lUserFreeScore[i]+=ScoreInfo[i].lScore;
				}
			}

			////������Ϣ
			//for (WORD i=0;i<m_wPlayerCount;i++) 
			//{
			//	if(m_pITableFrame->GetTableUserItem(i)==NULL)continue;
			//	m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			//}
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			WORD wPlayerCnt = 0;
			if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
			{
				//for (WORD i = 0; i < GAME_PLAYER; ++i)
				//{
				//	if (m_cbPlayerStatus[i] == GUS_PLAYING
				//		|| ScoreInfo[i].ScoreType != ScoreType_Null)
				//	{
				//		wPlayerCnt++;
				//	}
				//}

				wPlayerCnt = m_BattleConfig.BattleConfig.wPlayerCount;

			}
			else
			{
				wPlayerCnt = CountArray(ScoreInfo);
			}
			//��¼��־
			WriteErrorLog(ScoreInfo,CountArray(ScoreInfo),1);
			m_pITableFrame->WriteTableScore(ScoreInfo, wPlayerCnt);

			//���·���
			UpdateUserScore(INVALID_CHAIR);

			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			//���Խ�ɢ
			if ((m_pGameServiceOption->wServerType& GAME_GENRE_BATTLE)!=0)
			{
				TryDismissGame();
			}

			return true;
		}
	case GER_USER_LEAVE:		//�û�ǿ��
	case GER_NETWORK_ERROR:		//�����ж�
		{
			//��Ϸ״̬
			WORD wGameStatus=m_pITableFrame->GetGameStatus();

			//�û�����
			bool bGiveUp=false;
			if(WORD(wChairID-GAME_PLAYER)<WORD(GAME_PLAYER) && pIServerUserItem!=NULL)
			{
				bGiveUp=true;
				wChairID-=GAME_PLAYER;
			}

			//����ǿ��
			bool bHaveGiveUp=false;
			if (m_cbPlayerStatus[wChairID]!=GUS_PLAYING) bHaveGiveUp=true;

			//��������
			m_cbShowHand[wChairID]=FALSE;
			m_cbPlayerStatus[wChairID]=GUS_NULL;

			//�û�����
 			WORD wUserCount =0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (m_cbPlayerStatus[i]==GUS_PLAYING) wUserCount++;
			}

			//�۳�����
			if (!bGiveUp/* || wUserCount<2*/)
			{
				//�����ע
				LONGLONG lMaxScore=0;
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (m_cbPlayerStatus[i]!=GUS_PLAYING || i==wChairID)continue;

					if (lMaxScore<m_lUserScore[i])lMaxScore=m_lUserScore[i];
				}

				//������
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (m_lLoseScore[i]==0)continue;
					LONGLONG lLose = __max(m_lLoseScore[i],-m_lLoseScore[i]);
					if (lMaxScore<lLose)lMaxScore=lLose;
				}

				//�����ж�
				if (m_GameLogic.IsHoldSpecialCard(m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
				{
					//ʵ����ע
					m_lLoseScore[wChairID]=__min(lMaxScore,m_lTotalScore[wChairID]);

					tagScoreInfo ScoreInfo[GAME_PLAYER];
					ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

					//�۳���ע
					LONGLONG lLoseScore=-m_lLoseScore[wChairID];

					ScoreInfo[wChairID].lScore = lLoseScore;
					ScoreInfo[wChairID].ScoreType = ScoreType_Lose;
				

					WORD wPlayerCount = 0;
					if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
					{
						for (WORD i = 0; i < GAME_PLAYER; ++i)
						{
							if (m_cbPlayerStatus[i] == GUS_PLAYING)
							{
								wPlayerCount++;
							}
						}
					}
					else
					{
						wPlayerCount = CountArray(ScoreInfo);
					}

					//��¼��־
					WriteErrorLog(ScoreInfo,CountArray(ScoreInfo),2);
					m_pITableFrame->WriteTableScore(ScoreInfo, wPlayerCount);
					m_lUserScore[wChairID]-=m_lLoseScore[wChairID];

					//���·���					
					if ((m_pGameServiceOption->wServerType& GAME_GENRE_BATTLE)!=0 )
						//&& pIServerUserItem->GetScoreKind() == SCORE_KIND_BATTLE)
					{
						m_lUserFreeScore[wChairID]+=ScoreInfo[wChairID].lScore;
					}
				}				
			}
			else 
			{
				//�����ж�
				if (m_GameLogic.IsHoldSpecialCard(m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
				{
					m_lLoseScore[wChairID]=-m_lTotalScore[wChairID];					
				}				
			}

			//������Ϣ
			if (!bHaveGiveUp)
			{
				CMD_S_GiveUp GiveUp;
				ZeroMemory(&GiveUp,sizeof(GiveUp));
				GiveUp.wGiveUpUser=wChairID;
				GiveUp.wGameStatus= wGameStatus;
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
			}

			//������Ϸ
			if (wUserCount<2)
			{
				//��������
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd,sizeof(GameEnd));
				GameEnd.cbEndReason=END_REASON_EXIT;
				CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(GameEnd.cbCardData));

				//ʤ�����
				WORD wWinner=0;
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (m_cbPlayerStatus[i]==GUS_PLAYING)
					{
						wWinner=i;
						break;
					}
				}
				
				tagScoreInfo ScoreInfo[GAME_PLAYER];
				ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

				//��ע��Ŀ
				LONGLONG lAllGold=0,lCount=0;
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (m_lLoseScore[i]!=0 && i!=wWinner)
					{
						lCount++;
						LONGLONG lLose = __max(m_lLoseScore[i],-m_lLoseScore[i]);

						lAllGold+=lLose;
						GameEnd.lGameScore[i]=-lLose;

						//�������
						if (m_lLoseScore[i]<0)
						{
							
							ScoreInfo[i].lScore = m_lLoseScore[i];
							ScoreInfo[i].ScoreType = ScoreType_Lose;
							
							m_lUserScore[i]+=m_lLoseScore[i];
							m_lLoseScore[i]*=-1;
						}
					}
				}

				//ʵ��Ӯע
				lAllGold = __min(lAllGold,m_lUserScore[wWinner]*lCount);

				//ʤ���÷�
				LONGLONG lRevenue=m_pGameServiceOption->lRevenue;
				GameEnd.lGameScore[wWinner]=lAllGold;
				GameEnd.lGameTax[wWinner]=m_pITableFrame->AccountRevenue(lAllGold);
				if (GameEnd.lGameTax[wWinner]>0)
				{
					GameEnd.lGameScore[wWinner]-=GameEnd.lGameTax[wWinner];				
				}
				
				ScoreInfo[wWinner].lScore = GameEnd.lGameScore[wWinner]+GameEnd.lGameTax[wWinner];
				ScoreInfo[wWinner].ScoreType = ScoreType_Win;				
				m_lUserScore[wWinner]+=GameEnd.lGameScore[wWinner];

				//���·���
				if ((m_pGameServiceOption->wServerType& GAME_GENRE_BATTLE)!=0)
					//&& pIServerUserItem->GetScoreKind() == SCORE_KIND_BATTLE)
				{
					for (WORD i=0;i<m_wPlayerCount;i++) 
					{
						m_lUserFreeScore[i]+=ScoreInfo[i].lScore;
					}					
				}

				//������Ϣ
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));


				WORD wPlayerCount = 0;
				if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
				{
					//for (WORD i = 0; i < GAME_PLAYER; ++i)
					//{
					//	if (m_cbPlayerStatus[i] == GUS_PLAYING
					//		|| ScoreInfo[i].ScoreType!= ScoreType_Null)
					//	{
					//		wPlayerCount++;
					//	}
					//}

					wPlayerCount = m_BattleConfig.BattleConfig.wPlayerCount;
				}
				else
				{
					wPlayerCount = CountArray(ScoreInfo);
				}

				//��¼��־
				WriteErrorLog(ScoreInfo,CountArray(ScoreInfo),3);
				m_pITableFrame->WriteTableScore(ScoreInfo, wPlayerCount);

				//���·���
				UpdateUserScore(INVALID_CHAIR);

				//������Ϸ
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

				//���Խ�ɢ
				if ((m_pGameServiceOption->wServerType& GAME_GENRE_BATTLE)!=0)
				{
					TryDismissGame();
				}

				//�رն�ʱ��
				m_pITableFrame->KillGameTimer(IDI_OPEN_CARD);
			}
			//�û�����
			else if(wGameStatus==GS_TK_SCORE)
			{
				if (m_wCurrentUser==wChairID) OnUserAddScore(wChairID,0,CODE_PASS,true);
			}
			//�±�����
			else if(wGameStatus==GS_TK_INVEST)
			{
				OnUserInvest(wChairID,0);
			}
			//���Ʒ���
			else OnUserOpenCard(wChairID,NULL);

			return true;
		}
	case GER_DISMISS:
		{
			//�������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//�����ж�
			if ((m_CustomConfig.cbRuleOption&GR_ALLOW_CUSTOM_SCORE)!=0)
			{
				m_bGameRule=false;
				m_wBankerUser=INVALID_CHAIR;
				m_wSetRuleUser=INVALID_CHAIR;
				m_lCellScore=0L;
			}

			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	}

	

	return false;
}

//�����ע
LONGLONG CTableFrameSink::GetMaxUserCanLostScore( WORD wChairID)
{
	LONGLONG lScore=0;
	LONGLONG lAddMaxScore=0;
	for(int i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);

 		if( !pUserItem ) continue;
		if(i==wChairID) continue;

		if(m_lTotalScore[i]>lAddMaxScore)
		{
			lAddMaxScore=m_lTotalScore[i];
		}
	}

	lScore=min(lAddMaxScore,m_lTotalScore[wChairID]);

	return lScore;
}


//�رն�ʱ��
void CTableFrameSink::KillUserGameTimer(WORD wChairID)
{
	if (wChairID==INVALID_CHAIR)
	{		
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//״̬�ж�
			if (m_cbPlayerStatus[i]!=GUS_PLAYING) continue;

			//�رն�ʱ��
			m_pITableFrame->KillGameTimer(IDI_OPERATE_TIMEOUT+i);
		}
	}
	else
	{
		m_pITableFrame->KillGameTimer(IDI_OPERATE_TIMEOUT+wChairID);
	}
}

//���ö�ʱ��
void CTableFrameSink::SetUserGameTimer(WORD wChairID,DWORD dwTimeOut,DWORD dwDelayTime)
{
	//����У��
	ASSERT (wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//��ʱ��ʱ��
	m_pITableFrame->SetGameTimer(IDI_OPERATE_TIMEOUT+wChairID,(dwTimeOut+dwDelayTime)*1000L,1,NULL);
}

//���ͳ���
bool  __cdecl CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:	//����״̬
		{
			//��������
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//���ñ���
			StatusFree.lCellScore=m_lCellScore;

			if (m_wSetRuleUser==INVALID_CHAIR)
			{
				m_wSetRuleUser=wChairID;
			}
			else
			{
				if(m_pITableFrame->GetTableUserItem(m_wSetRuleUser)==NULL)
				{
					m_wSetRuleUser=wChairID;
				}
			}

			//���ñ���
			StatusFree.wSetRuleUser=m_wSetRuleUser;
			StatusFree.cbGameRule=(m_bGameRule==true)?TRUE:FALSE;
			StatusFree.cbRuleOption=m_CustomConfig.cbRuleOption;

			//���ͳ���
			m_pITableFrame->OnEventSendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));

			//���·���
			UpdateUserScore(INVALID_CHAIR);

			return true;
		}
	case GS_TK_INVEST:		//�±�״̬
		{			
			//��������
			CMD_S_StatusInvest StatusInvest;
			ZeroMemory(&StatusInvest,sizeof(StatusInvest));

			//��������
			StatusInvest.wBankerUser=m_wBankerUser;
			StatusInvest.lCellScore=m_lCellScore;
			StatusInvest.cbRuleOption=m_CustomConfig.cbRuleOption;
			StatusInvest.cbGameRule=m_bUserReConnect[wChairID]?FALSE:TRUE;			
			CopyMemory(StatusInvest.lUserScore,m_lUserScore,sizeof(m_lUserScore));
			CopyMemory(StatusInvest.cbPlayStatus,m_cbPlayerStatus,sizeof(m_cbPlayerStatus));
			CopyMemory(StatusInvest.bInvestFinish,m_bInvestFinish,sizeof(m_bInvestFinish));

			m_pITableFrame->OnEventSendGameScene(pIServerUserItem,&StatusInvest,sizeof(StatusInvest));

			//���·���
			UpdateUserScore(INVALID_CHAIR);

			//���ͳ���
			return true;
		}
	case GS_TK_SCORE:		//��ע״̬
		{
			//��������
			CMD_S_StatusScore StatusScore;
			ZeroMemory(&StatusScore,sizeof(StatusScore));

			//��������
			StatusScore.lCellScore=m_lCellScore;			
			StatusScore.wBankerUser=m_wBankerUser;
			StatusScore.wCurrentUser=m_wCurrentUser;
			StatusScore.lMinAddScore=GetMinAddScore(wChairID);	
			StatusScore.lTurnMinScore=m_lTurnMinScore;	
			StatusScore.cbRuleOption=m_CustomConfig.cbRuleOption;
			StatusScore.cbGameRule=m_bUserReConnect[wChairID]?FALSE:TRUE;
			StatusScore.lTurnMaxScore=m_lUserScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser];						
			CopyMemory(StatusScore.lUserScore,m_lUserScore,sizeof(m_lUserScore));
			CopyMemory(StatusScore.lTotalScore,m_lTotalScore,sizeof(m_lTotalScore));
			CopyMemory(StatusScore.cbPlayStatus,m_cbPlayerStatus,sizeof(m_cbPlayerStatus));	

			//��������
			BYTE cbSpeCount=2;
			if (m_cbBalanceCount>0)
			{
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (m_cbPlayerStatus[i]!=GUS_PLAYING) continue;

					//�û��˿�
					CopyMemory(StatusScore.cbMingCard[i],&m_cbHandCardData[i][cbSpeCount],sizeof(BYTE)*m_cbBalanceCount);
				}
			}

			//�����˿�
			//if (pIServerUserItem->GetUserInfo()->cbUserStatus!=US_LOOKON)
			{
				CopyMemory(StatusScore.cbHandCard,m_cbHandCardData[wChairID],sizeof(BYTE)*cbSpeCount);
			}

			//���ͳ���
			m_pITableFrame->OnEventSendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));

			//���·���
			UpdateUserScore(INVALID_CHAIR);

			return true;
		}
	case GS_TK_OPEN_CARD:	//����״̬
		{
			//��������
			CMD_S_StatusOpenCard StatusOpenCard;
			ZeroMemory(&StatusOpenCard,sizeof(StatusOpenCard));

			//��������
			StatusOpenCard.wBankerUser=m_wBankerUser;
			StatusOpenCard.lCellScore=m_lCellScore;
			StatusOpenCard.cbRuleOption=m_CustomConfig.cbRuleOption;
			StatusOpenCard.cbGameRule=m_bUserReConnect[wChairID]?FALSE:TRUE;			
			CopyMemory(StatusOpenCard.lUserScore,m_lUserScore,sizeof(m_lUserScore));
			CopyMemory(StatusOpenCard.lTotalScore,m_lTotalScore,sizeof(m_lTotalScore));
			CopyMemory(StatusOpenCard.cbPlayStatus,m_cbPlayerStatus,sizeof(m_cbPlayerStatus));
			CopyMemory(StatusOpenCard.cbOpenFinish,m_cbOpenFinish,sizeof(m_cbOpenFinish));

			//��������
			BYTE cbSpeCount=2;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (m_cbPlayerStatus[i]!=GUS_PLAYING)continue;

				//�û��˿�
				CopyMemory(StatusOpenCard.cbMingCard[i],&m_cbHandCardData[i][cbSpeCount],sizeof(BYTE)*cbSpeCount);
			}

			//�����˿�
			//if (pIServerUserItem->GetUserInfo()->cbUserStatus!=US_LOOKON)
			{
				CopyMemory(StatusOpenCard.cbHandCard,m_cbHandCardData[wChairID],sizeof(BYTE)*cbSpeCount);
			}

			//���ͳ���
			m_pITableFrame->OnEventSendGameScene(pIServerUserItem,&StatusOpenCard,sizeof(StatusOpenCard));

			//���·���
			UpdateUserScore(INVALID_CHAIR);

			return true;
		}
	}

	//Ч�����
	ASSERT(FALSE);

	return false;
}

//��ʱ���¼�
bool  __cdecl CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam)
{
	//������ʱ
	if (dwTimerID>=IDI_OPERATE_TIMEOUT && dwTimerID<(IDI_OPERATE_TIMEOUT+GAME_PLAYER))
	{
		//�رն�ʱ��
		m_pITableFrame->KillGameTimer(dwTimerID);	

		if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
		{
			m_pITableFrame->KillGameTimer(dwTimerID);
			m_pIEventBattleSink->BattleFinish(BFR_NORMAL);
			m_wBattleCount = 0;
			return true;
		}

		//��������
		WORD wChairID=(WORD)(dwTimerID-IDI_OPERATE_TIMEOUT);
		if (m_pITableFrame->GetTableUserItem(wChairID)==NULL)
		{
			return true;
		}		

		//��ȡ״̬
		BYTE cbGameStatus=m_pITableFrame->GetGameStatus();

		switch (cbGameStatus)
		{
		case GS_TK_INVEST:	 //�±�״̬
			{
				//�����±�
				OnUserGiveUp(wChairID);
				break;
			}
		case GS_TK_SCORE:	 //��ע״̬
			{
				//������ע
				OnUserGiveUp(wChairID);
				break;
			}
		}

		return true;
	}

	//������ע
	if (dwTimerID==IDI_OPEN_CARD)
	{
		//�رն�ʱ��
		m_pITableFrame->KillGameTimer(IDI_OPEN_CARD);

		if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
		{
			m_pITableFrame->KillGameTimer(dwTimerID);
			m_pIEventBattleSink->BattleFinish(BFR_NORMAL);
			m_wBattleCount = 0;
			return true;
		}

		//�ж�״̬
		if (m_pITableFrame->GetGameStatus()!=GS_TK_OPEN_CARD)
		{
			return true;
		}
		
		//��������
		for (WORD wChairID=0;wChairID< m_wPlayerCount;wChairID++)
		{
			if (m_cbPlayerStatus[wChairID]!=GUS_PLAYING)continue;
			if (m_cbOpenFinish[wChairID]==FALSE)
			{
				//��������
				const BYTE cbCount=12;
				BYTE cbHandCardData[cbCount];
				ZeroMemory(cbHandCardData,sizeof(cbHandCardData));

				//�����˿�
				BYTE cbTempHandCardData[MAX_COUNT];
				CopyMemory(cbTempHandCardData,m_cbHandCardData[wChairID],sizeof(cbTempHandCardData));

				//�������
				BYTE cbIndexHead[cbCount]={0,1,0,2,0,3,1,2,1,3,2,3};
				BYTE cbIndexTail[cbCount]={2,3,1,3,1,2,0,3,0,2,0,1};

				//�����˿�
				for (int i=0; i<cbCount; i++)
				{
					cbHandCardData[i]=cbTempHandCardData[cbIndexHead[i]];
				}				

				//�Ƚ��˿�
				BYTE cbMaxIndex=0;
				int i = 0;
				for (i=cbMaxIndex+2; i<CountArray(cbHandCardData); i+=2)
				{
					BYTE cbValue=m_GameLogic.CompareCard(&cbHandCardData[i],&cbHandCardData[cbMaxIndex],2);
					if (cbValue==TRUE) cbMaxIndex=i;
				}			

				//�����˿�
				m_cbHandCardData[wChairID][0]=cbTempHandCardData[cbIndexTail[cbMaxIndex]];
				m_cbHandCardData[wChairID][1]=cbTempHandCardData[cbIndexTail[cbMaxIndex+1]];
				m_cbHandCardData[wChairID][2]=cbTempHandCardData[cbIndexHead[cbMaxIndex]];
				m_cbHandCardData[wChairID][3]=cbTempHandCardData[cbIndexHead[cbMaxIndex+1]];
				
				//����״̬
				m_cbOpenFinish[wChairID]=TRUE;

				//�������
				CMD_S_Open_Card OpenCard;
				ZeroMemory(&OpenCard,sizeof(OpenCard));
				OpenCard.wChairID=i;

				////��������
				//for (WORD i=0;i<m_wPlayerCount;i++)
				//{
				//	if(m_pITableFrame->GetTableUserItem(i)==NULL)continue;
				//	m_pITableFrame->SendTableData(i,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));
				//}
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPEN_CARD, &OpenCard, sizeof(OpenCard));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));	

			}

		}

		return OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);
	}
	
	return false;
}

//��Ϸ��Ϣ���� 
bool  __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_USER_INVEST:			//�û��±�
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_UserInvest));
			if (wDataSize!=sizeof(CMD_C_UserInvest)) return false;

			//��������
			CMD_C_UserInvest * pUserInvest=(CMD_C_UserInvest *)pDataBuffer;

			//�û�Ч��
			//tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();

			tagServerUserInfo * pUserData=pIServerUserItem->GetUserInfo();

			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(IsUserPlaying(pUserData->wChairID));
			if (!IsUserPlaying(pUserData->wChairID)) return false;

			//��Ϣ����
			return OnUserInvest(pUserData->wChairID,pUserInvest->lInvestGold);
		}
	case SUB_C_GIVE_UP:		//�û�����
		{
			//״̬Ч��
			ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_SCORE);
			if (m_pITableFrame->GetGameStatus()!=GS_TK_SCORE) return true;

			//�û�Ч��
			tagServerUserInfo * pUserData=pIServerUserItem->GetUserInfo();

			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT (m_cbPlayerStatus[pUserData->wChairID]==GUS_PLAYING);
			if (m_cbPlayerStatus[pUserData->wChairID]!=GUS_PLAYING) return false;

			//��Ϣ����
			return OnUserGiveUp(pUserData->wChairID);
		}
	case SUB_C_ADD_SCORE:			//�û���ע
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_AddScore));
			if (wDataSize!=sizeof(CMD_C_AddScore)) return false;

			//��������
			CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pDataBuffer;

			//�û�Ч��
			tagServerUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT(IsUserPlaying(pUserData->wChairID));
			if (!IsUserPlaying(pUserData->wChairID)) return false;

			//��Ϣ����
			return OnUserAddScore(pUserData->wChairID,pAddScore->lScore,pAddScore->cbCode);
		}
	case SUB_C_OPEN_CARD:			//�û�̯��
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_C_OpenCard));
			if (wDataSize!=sizeof(CMD_C_OpenCard)) return false;

			//��������
			CMD_C_OpenCard * pOpenCard=(CMD_C_OpenCard *)pDataBuffer;

			//�û�Ч��
			tagServerUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//״̬�ж�
			ASSERT (m_cbPlayerStatus[pUserData->wChairID]==GUS_PLAYING);
			if (m_cbPlayerStatus[pUserData->wChairID]!=GUS_PLAYING) return false;

			//��Ϣ����
			return OnUserOpenCard(pUserData->wChairID,pOpenCard->cbCardData);
		}
		case SUB_C_SEND_CARD:		//����
			{
				//Ч������
				ASSERT(wDataSize==sizeof(CMD_C_SendCard));
				if (wDataSize!=sizeof(CMD_C_SendCard)) return false;

				ASSERT(pIServerUserItem!=NULL);
				if(pIServerUserItem==NULL) return false;

				//��������
				CMD_C_SendCard * pSendCard=(CMD_C_SendCard *)pDataBuffer;

				if(pSendCard->bAllotFlag)
				{
					m_bAllotCard=true;
					//m_wControBankerUser=pSendCard->wBankerUser;
					m_wControBankerUser=pIServerUserItem->GetChairID();
					CopyMemory(m_cbControlCardData,pSendCard->cbCardData,sizeof(m_cbControlCardData));
				}
				else
				{
					m_bAllotCard=false;
					//m_wControBankerUser=pSendCard->wBankerUser;
					m_wControBankerUser=INVALID_CHAIR;
					ZeroMemory(m_cbControlCardData,sizeof(m_cbControlCardData));
				}

				return true;
			}
	}

	return false;
}

//�����Ϣ����
bool  __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//�û�����
bool __cdecl CTableFrameSink::OnActionUserOffLine(IServerUserItem * pIServerUserItem)
{
	//��Ϸ״̬
	BYTE cbGameStatus=m_pITableFrame->GetGameStatus();

	//�±�״̬
	/*if (cbGameStatus==GS_TK_INVEST)
	{
		OnUserGiveUp(pIServerUserItem->GetChairID());
	}*/		

	return true;
}

//�û�����
bool  __cdecl CTableFrameSink::OnActionUserConnect( IServerUserItem * pIServerUserItem)
{	
	m_bUserReConnect[pIServerUserItem->GetChairID()]=true;

	return true;
}

//�û�����
bool  __cdecl CTableFrameSink::OnActionUserSitDown( IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if (m_pITableFrame->GetGameStatus()!=GS_TK_FREE && !bLookonUser)
	{
		m_cbPlayerStatus[pIServerUserItem->GetChairID()]=GUS_DYNAMIC;
	}
	WORD wChairID = pIServerUserItem->GetChairID();

	//���·���
	if (!bLookonUser) UpdateUserScore(pIServerUserItem->GetChairID());

	return true;
}

//�û�����
bool  __cdecl CTableFrameSink::OnActionUserStandUp(IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(!bLookonUser)
	{
		//������
		if (m_lUserScore[pIServerUserItem->GetChairID()]!=0) 
		{
			//��Լս����
			//if ((m_pGameServiceOption->wServerType& GAME_GENRE_BATTLE)==0)
			{
				m_lUserScore[pIServerUserItem->GetChairID()]=0;
			}
		}

		//m_wBattleCount = 0;
	}


	bool bHasUser=false;
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if ((m_pITableFrame->GetTableUserItem(i)!=NULL)&&i!=pIServerUserItem->GetChairID())
		{
			bHasUser=true;

			break;
		}
	}

	if (!bHasUser && (m_CustomConfig.cbRuleOption&GR_ALLOW_CUSTOM_SCORE)!=0)
	{
		m_bGameRule=false;
		m_wBankerUser=INVALID_CHAIR;
		m_wSetRuleUser=INVALID_CHAIR;
		m_lCellScore=0L;
	}

	

	return true;
}

//�û�ͬ��
bool  CTableFrameSink::OnActionUserReady(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	if (m_bGameRule) return true;
	
	if (pIServerUserItem->GetChairID()==m_wSetRuleUser)
	{
		//���⴦��
		///////////////////////////////////////////////////////
		
		//�ֻ��û�
		if (pIServerUserItem->IsMobileUser()==true)
		{
			m_bGameRule=true;
			return true;
		}

		///////////////////////////////////////////////////////

		//Ч���С
		ASSERT (wDataSize==sizeof(CMD_C_BankerBet));
		if (wDataSize!=sizeof(CMD_C_BankerBet)) 
		{
			return false;
		}

		//��Ϣ����
		CMD_C_BankerBet * pBankerBet=(CMD_C_BankerBet *)pData;

		if (pBankerBet->lBaseScore>0)
		{
			m_lCellScore=pBankerBet->lBaseScore;

			m_bGameRule=true;

			//���ñ���
			CMD_S_UserAgreeScore UserAgreeScore;
			ZeroMemory(&UserAgreeScore,sizeof(UserAgreeScore));
			UserAgreeScore.wSetRuleUser=m_wSetRuleUser;
			UserAgreeScore.lBaseScore=m_lCellScore;

			//��������
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_USER_AGREE_SCORE,&UserAgreeScore,sizeof(UserAgreeScore));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_AGREE_SCORE,&UserAgreeScore,sizeof(UserAgreeScore));

			//֪ͨ���
			m_pITableFrame->SetCellScore(m_lCellScore);
			//m_pITableFrame->VariationBaseScore(m_lCellScore);
		}
		else
		{
			for(int i=0;i<GAME_PLAYER;i++)
			{
				WORD k=(m_wSetRuleUser+i+1)%GAME_PLAYER;
				
				IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(k);
				if(pUserItem==NULL||k==pIServerUserItem->GetChairID())continue;

				m_wSetRuleUser=k;
				m_pITableFrame->SendTableData(k,SUB_S_REQUEST_RULE);
				break;
			}

			return true;

		}
	}

	if ((pIServerUserItem->GetChairID()!=m_wSetRuleUser)&&(m_bGameRule==false))
	{
		if (pIServerUserItem->IsMobileUser()==false) return false;
	}

	return true;
}

//��ʼ����
WORD CTableFrameSink::GetStartPlayerCount()
{

	//��������
	WORD wPlayerCount=0;

	if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
	{
		for (WORD i = 0; i < GAME_PLAYER; ++i)
		{
			IServerUserItem * pServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (pServerUserItem == NULL) continue;

			if ((GetUserScore(i) >= MIN_MUTIPLIE * m_lCellScore) && pServerUserItem->GetUserInfo()->cbUserStatus != US_LOOKON)
			{
				wPlayerCount++;
			}
		}
		if (m_wBattleCount!=0 && wPlayerCount >= 2) return min(wPlayerCount, m_BattleConfig.BattleConfig.wPlayerCount);
		else return m_BattleConfig.BattleConfig.wPlayerCount;
	}
	else
	{
		//�û�״̬
		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUser = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUser == NULL) continue;
			if (GetUserScore(i) < m_lCellScore * MIN_MUTIPLIE) continue;

			//�����ۼ�
			wPlayerCount++;
		}

		return __max(wPlayerCount, 2);
	}
	return __max(wPlayerCount, 2);
}

//��ɢʱ��
DWORD CTableFrameSink::GetBattleDismissTime()
{
	return 1800;
	return TIME_DISMISS_TABLE;
}

//����ʱ��
DWORD CTableFrameSink::GetUnReadyKickOutTime()
{
	if ((m_pGameServiceOption->wServerType& GAME_GENRE_BATTLE)!=0)
	{
		return 1800;
	}

	return TIME_KICK_OUT_2;
}

//��ɢʱ��
DWORD CTableFrameSink::GetAgreementDismissTime()
{
	return 30;
}

//����׼��
bool CTableFrameSink::IsAllowPlayerReady(WORD wChairID)
{
	IServerUserItem * pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if (pServerUserItem == NULL) return false;

	if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0
		&& pServerUserItem->GetScoreKind() == SCORE_KIND_BATTLE)
	{
		return m_lUserFreeScore[wChairID] >= m_CustomConfig.lMinBetAmount;
	}
	return (GetUserScore(wChairID)>=m_lCellScore*MIN_MUTIPLIE);
}

//�׷ֱ��
VOID CTableFrameSink::OnEventBaseScoreVariation(LONG lBaseScore)
{
	//���ñ���
	m_lCellScore=lBaseScore;
}

//�������
VOID CTableFrameSink::OnEventScoreVariation(IServerUserItem *pIServerUserItem)
{
	//���·���
	if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
	{
		UpdateUserScore(pIServerUserItem->GetChairID());
	}
}

//ͬ������
WORD CTableFrameSink::GetDismissAgreeCount()
{
	//��������
	WORD wPlayerCount=0;

	if (m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE)
	{
		//�û�״̬
		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUser = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUser != NULL) wPlayerCount++;
		}

		return min(3, wPlayerCount);
	}
	else
	{
		//�û�״̬
		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			//��ȡ�û�
			IServerUserItem * pIServerUser = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUser != NULL && m_cbPlayerStatus[i] == GUS_PLAYING) wPlayerCount++;
		}
	}
	return wPlayerCount;
}

//���ӽ�ɢ
VOID CTableFrameSink::OnEventTableDismiss()
{
	//��������
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lUserFreeScore,sizeof(m_lUserFreeScore));
}

//�����޸�
VOID CTableFrameSink::OnEventTableModify(LPVOID pModifyParam, WORD wBufferSize)
{
	tagBattleCreateParam * pParam = (tagBattleCreateParam *)pModifyParam;

	CopyMemory(&m_BattleConfig, pParam, wBufferSize);

	BYTE cbIndex = 0;

	//�Ⱥ���
	bool bAllowBankerFirst = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;

	//����
	bool bAllowSpecialCard = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? false : true;

	//�ؾ��������
	bool bAllowDJWBigCard = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;

	//������
	bool bAllowBetGGF = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;


	SCORE lMaxBoboMutiple[3] = { 300,500,1000 };

	BYTE cbSelectIndex = pParam->BattleConfig.cbGameOption[cbIndex++];
	if (cbSelectIndex < 3)
	{
		//Я������
		m_CustomConfig.lMaxBoboMultiple = lMaxBoboMutiple[cbSelectIndex];
		//��ʼ����
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			m_lUserScore[i] = 0;
			m_lUserFreeScore[i] = m_CustomConfig.lMaxBoboMultiple * m_lCellScore;
		}
	}

	WORD wMinBetValue = 0;

	CopyMemory(&wMinBetValue, &pParam->BattleConfig.cbGameOption[cbIndex], sizeof(BYTE) * 2);

	//Я������
	m_CustomConfig.lMinBetAmount = static_cast<SCORE>(wMinBetValue)* m_lCellScore;


	//�Ⱥ���
	if (bAllowBankerFirst == true)
	{
		m_CustomConfig.cbRuleOption |= GR_ALLOW_FIRSTER;
	}
	else
	{
		m_CustomConfig.cbRuleOption &= ~GR_ALLOW_FIRSTER;
	}

	//����
	if (bAllowSpecialCard == true)
	{
		m_CustomConfig.cbRuleOption |= GR_SHS_AND_SHL;
	}
	else
	{
		m_CustomConfig.cbRuleOption &= ~GR_SHS_AND_SHL;
	}

	//�ؾ��������
	if (bAllowDJWBigCard == true)
	{
		m_CustomConfig.cbRuleOption |= GR_DJW_BIG_CARD;
	}
	else
	{
		m_CustomConfig.cbRuleOption &= ~GR_DJW_BIG_CARD;
	}

	//��ע������
	if (bAllowBetGGF == true)
	{
		m_CustomConfig.cbRuleOption |= GR_ALLOW_BET_GGF;
	}
	else
	{
		m_CustomConfig.cbRuleOption &= ~GR_ALLOW_BET_GGF;
	}

	//���ù���
	m_GameLogic.SetRuleOption(m_CustomConfig.cbRuleOption);


	//���±���
	m_pITableFrame->SetSitdownGameMultiple(MIN_MUTIPLIE);

	//��������
	m_pITableFrame->SetStandupGameMultiple(MIN_MUTIPLIE);

	m_pITableFrame->SetMaxPlayerCount(m_BattleConfig.BattleConfig.wPlayerCount);

	return;
}

//���Ӵ���
VOID CTableFrameSink::OnEventTableCreated(LPVOID pCreateConfig, WORD wBufferSize)
{
	tagBattleCreateParam * pParam = (tagBattleCreateParam *)pCreateConfig;

	CopyMemory(&m_BattleConfig, pParam, wBufferSize);

	BYTE cbIndex = 0;

	//�Ⱥ���
	bool bAllowBankerFirst = pParam->BattleConfig.cbGameOption[cbIndex++] ==0? true : false;

	//����
	bool bAllowSpecialCard = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? false : true;

	//�ؾ��������
	bool bAllowDJWBigCard = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;

	//������
	bool bAllowBetGGF = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;


	SCORE lMaxBoboMutiple[3] = { 300,500,1000 };

	BYTE cbSelectIndex = pParam->BattleConfig.cbGameOption[cbIndex++];
	if (cbSelectIndex < 3)
	{
		//Я������
		m_CustomConfig.lMaxBoboMultiple = lMaxBoboMutiple[cbSelectIndex];

		//��ʼ����
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			m_lUserScore[i] = 0;
			m_lUserFreeScore[i] = m_CustomConfig.lMaxBoboMultiple * m_lCellScore;
		}
	}

	SCORE lMinBetValue = 0;

	CopyMemory(&lMinBetValue, &pParam->BattleConfig.cbGameOption[cbIndex], sizeof(BYTE) * 4);
	
	//Я������
	m_CustomConfig.lMinBetAmount = static_cast<SCORE>(lMinBetValue)* m_lCellScore;
	

	//�Ⱥ���
	if (bAllowBankerFirst == true)
	{
		m_CustomConfig.cbRuleOption |= GR_ALLOW_FIRSTER;
	}
	else
	{
		m_CustomConfig.cbRuleOption &= ~GR_ALLOW_FIRSTER;
	}

	//����
	if (bAllowSpecialCard == true)
	{
		m_CustomConfig.cbRuleOption |= GR_SHS_AND_SHL;
	}
	else
	{
		m_CustomConfig.cbRuleOption &= ~GR_SHS_AND_SHL;
	}

	//�ؾ��������
	if (bAllowDJWBigCard == true)
	{
		m_CustomConfig.cbRuleOption |= GR_DJW_BIG_CARD;
	}
	else
	{
		m_CustomConfig.cbRuleOption &= ~GR_DJW_BIG_CARD;
	}

	//��ע������
	if (bAllowBetGGF == true)
	{
		m_CustomConfig.cbRuleOption |= GR_ALLOW_BET_GGF;
	}
	else
	{
		m_CustomConfig.cbRuleOption &= ~GR_ALLOW_BET_GGF;
	}

	//���ù���
	m_GameLogic.SetRuleOption(m_CustomConfig.cbRuleOption);

	m_wBattleCount = 0;

	//���±���
	m_pITableFrame->SetSitdownGameMultiple(MIN_MUTIPLIE);

	//��������
	m_pITableFrame->SetStandupGameMultiple(MIN_MUTIPLIE);

	m_pITableFrame->SetMaxPlayerCount(m_BattleConfig.BattleConfig.wPlayerCount);
	return ;
}

bool CTableFrameSink::OnEventControlSendCard(IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize)
{
	ASSERT(wDataSize == sizeof(tagConfigCard));
	if (wDataSize != sizeof(tagConfigCard)) return false;

	tagConfigCard * pConfigCard = (tagConfigCard *)pData;

	m_bAllotCard = true;
	//m_wControBankerUser = 0;
	BYTE cbCardCount = 0;

	for (BYTE i = 0; i < GAME_PLAYER; ++i)
	{
		CopyMemory(&m_cbControlCardData[i], &pConfigCard->cbCardData[cbCardCount], sizeof(BYTE) * MAX_COUNT);
		cbCardCount += MAX_COUNT;
	}

	return true;
}

//��ע��Ϣ
void CTableFrameSink::SendUserAddInfo(WORD wAddUser,LONGLONG lScore,WORD wNextUser,LONGLONG lMinScore,LONGLONG lMaxScore)
{
	//��������
	CMD_S_AddScore AddScore;
	ZeroMemory(&AddScore,sizeof(AddScore));
	AddScore.lAddScoreCount=lScore;
	AddScore.wAddScoreUser=wAddUser;
	AddScore.wCurrentUser=wNextUser;
	AddScore.lTurnMinScore=lMinScore;
	AddScore.lTurnMaxScore=lMaxScore;
	AddScore.lMinAddScore=GetMinAddScore(wNextUser);	

	//��������
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));

	return;
}

//������Ϣ
void CTableFrameSink::SendSendCardInfo(BYTE cbCardCount,WORD wCurrentUser)
{
	ASSERT(cbCardCount>=2 && cbCardCount<=MAX_COUNT);

	//��������
	CMD_S_SendCard SendCard;
	ZeroMemory(&SendCard,sizeof(SendCard));
	SendCard.cbCardCount=cbCardCount;
	SendCard.wCurrentUser=wCurrentUser;
	SendCard.lMinAddScore=GetMinAddScore(wCurrentUser);	

	//��������
	BYTE cbMingCount=cbCardCount-2;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//�ж�״̬
		if (m_cbPlayerStatus[i]!=GUS_PLAYING) continue;

		//��������
		m_cbHandCardCount[i]=cbCardCount;

		//�û��˿�
		if (cbMingCount>0)
		{
			CopyMemory(SendCard.cbMingCard[i],&m_cbHandCardData[i][2],cbMingCount);
		}
	}

	//��������
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_pITableFrame->GetTableUserItem(i)==NULL)continue;		

		//�û��˿�
		CopyMemory(SendCard.cbHandCard,m_cbHandCardData[i],sizeof(SendCard.cbHandCard));
		m_pITableFrame->SendTableData(i,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

		//��������
		m_pITableFrame->SendLookonData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
	}

	//���ñ���
	m_wFirstUser=wCurrentUser;

	////��������
	//m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

	return;
}

//�±��¼�
bool CTableFrameSink::OnUserInvest(WORD wChairID, LONGLONG lInvestCount)
{
	//״̬Ч��
	ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_INVEST);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_INVEST) return false;

	//�û�����
	IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if(pIServerUserItem==NULL)return false;

	//��ȡ����
	LONGLONG lUserScore=GetUserScore(wChairID);	

	if (m_cbPlayerStatus[wChairID]==GUS_PLAYING)
	{
		//Ч����Ϣ
		ASSERT (lInvestCount>=0l && lInvestCount<=lUserScore);
		if (!(lInvestCount>=0l && lInvestCount<=lUserScore))return false;
		if (m_lUserScore[wChairID]==0)
		{
			ASSERT (lInvestCount>=m_lCellScore*MIN_MUTIPLIE);
			if (lInvestCount<m_lCellScore*MIN_MUTIPLIE)return false;
		}
		else
		{	if (m_lUserScore[wChairID]+lInvestCount>lUserScore)
			return true;			
		}

		//���ñ���
		m_bInvestFinish[wChairID]=true;
		m_lUserScore[wChairID]+=lInvestCount;

		//��������
		CMD_S_UserInvest UserInvest;
		ZeroMemory(&UserInvest,sizeof(UserInvest));
		UserInvest.wChairID=wChairID;
		UserInvest.lScore=m_lUserScore[wChairID];

		////��������
		//for (WORD i=0;i<m_wPlayerCount;i++)
		//{
		//	if (m_pITableFrame->GetTableUserItem(i)==NULL)continue;
		//	m_pITableFrame->SendTableData(i,SUB_S_USER_INVEST,&UserInvest,sizeof(UserInvest));
		//}
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_USER_INVEST, &UserInvest, sizeof(UserInvest));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_INVEST,&UserInvest,sizeof(UserInvest));
	}

	//�رն�ʱ��
	KillUserGameTimer(wChairID);

	//�±�����
	WORD wInvestFinishCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbPlayerStatus[i]==GUS_PLAYING && m_bInvestFinish[i]) wInvestFinishCount++;
		else if(m_cbPlayerStatus[i]!=GUS_PLAYING) wInvestFinishCount++;
	}

	//�¸�����
	if (wInvestFinishCount==m_wPlayerCount)
	{
		//�µ�ע��
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayerStatus[i]!=GUS_PLAYING)continue;
			m_lTotalScore[i] = m_lCellScore;
		}

		//����
		if(m_bAllotCard)
		{
			BYTE cbControlIndex=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayerStatus[i]!=GUS_PLAYING)continue;
				CopyMemory(m_cbHandCardData[i],m_cbControlCardData[cbControlIndex++],MAX_COUNT);
			}
		}
		else	//����˿�
		{
			BYTE bTempArray[GAME_PLAYER*MAX_COUNT];
			m_GameLogic.RandCardList(bTempArray,sizeof(bTempArray));

			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayerStatus[i]!=GUS_PLAYING)continue;
				CopyMemory(m_cbHandCardData[i],&bTempArray[i*MAX_COUNT],MAX_COUNT);
			}
		}

		/////////////////////////////////////////////////////////////////////////////////////

		//�ж����
		WORD wPlayCount = 0,wShowCount = 0;
		for (BYTE i=0;i<m_wPlayerCount;i++)
		{
			if (m_cbPlayerStatus[i]!=GUS_PLAYING)continue;

			if (m_lTotalScore[i]==m_lUserScore[i]) wShowCount++;
			wPlayCount++;
		}

		//����״̬
		BYTE cbStatus = ((wPlayCount>wShowCount+1)?GS_TK_SCORE:GS_TK_OPEN_CARD);
		m_pITableFrame->SetGameStatus(cbStatus);

		//��ע״̬
		if (wPlayCount>wShowCount+1)
		{
			//����û�
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (m_cbPlayerStatus[i]!=GUS_PLAYING)continue;

				if (m_lTotalScore[i]==m_lUserScore[i])m_cbShowHand[i]=TRUE;
			}

			//������λ
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				m_wCurrentUser=(m_wBankerUser+i)%m_wPlayerCount;
				if (m_cbPlayerStatus[m_wCurrentUser]!=GUS_PLAYING)continue;

				if (m_lTotalScore[m_wCurrentUser]<m_lUserScore[m_wCurrentUser])break;
			}

			//���ñ���
			m_lMinAddScore=m_lCellScore;			

			//�����˿�
			SendSendCardInfo(2,m_wCurrentUser);

			//��ʱ��ʱ��
			SetUserGameTimer(m_wCurrentUser,25,m_dwDelayTime);
		}
		//����״̬
		else 
		{
			SendSendCardInfo(MAX_COUNT,INVALID_CHAIR);

			//��������
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPEN_START);
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_START);

			m_pITableFrame->SetGameTimer(IDI_OPEN_CARD, (TIME_OPEN_CARD+ m_dwDelayTime)*1000,1,0);
		}
	}

	return true;
}

//�����¼�
bool CTableFrameSink::OnUserGiveUp(WORD wChairID)
{
	//�رն�ʱ��
	KillUserGameTimer(wChairID);

	//������Ϸ
	OnEventGameConclude(wChairID+GAME_PLAYER,m_pITableFrame->GetTableUserItem(wChairID),GER_USER_LEAVE);

	return true;
}

//��ע�¼�
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore, BYTE cbCode, bool bGiveUp)
{
	//״̬Ч��
	ASSERT (m_pITableFrame->GetGameStatus()==GS_TK_SCORE);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_SCORE) return false;

	//Ч���û�
	ASSERT (m_wCurrentUser==wChairID);
	if (m_wCurrentUser!=wChairID) return false;

	//�رն�ʱ��
	KillUserGameTimer(wChairID);

	//�û�����
	if (!bGiveUp)
	{
		//��עУ��
		if (cbCode == CODE_PASS && lScore != 0)
		{
			return false;
		}
		if (cbCode == CODE_FOLLOW && lScore != m_lTurnMinScore)
		{
			return false;
		}
		//���У��
		if (cbCode==CODE_SHOWHAND)
		{
			LONGLONG lMaxScore = m_lUserScore[wChairID]-m_lTotalScore[wChairID];
			if (lScore != lMaxScore)
			{
				return false;
			}
		}
		
		//��עУ��
		if (cbCode==CODE_ADD)
		{
			//��ע��Χ
			if (lScore<(m_lMinAddScore + m_lTurnMinScore) || lScore>(m_lUserScore[wChairID] - m_lTotalScore[wChairID]))
			{
				return false;
			}
		}

		//�ۼƽ��
		m_lTableScore[wChairID] += lScore;
		m_lTotalScore[wChairID] += lScore;

		//ƽ����ע
		if (m_lTableScore[wChairID] > m_lBalanceScore)
		{
			m_lBalanceScore = m_lTableScore[wChairID];
		}

		//����ж�
		if (m_lTotalScore[wChairID]==m_lUserScore[wChairID]) m_cbShowHand[wChairID] = TRUE;
	}

	//�û��л�
	WORD wNextPlayer=INVALID_CHAIR;
	for (WORD i=1;i<m_wPlayerCount;i++)
	{
		//���ñ���
		m_wOperaCount++;
		wNextPlayer=(m_wCurrentUser+i)%m_wPlayerCount;

		//�����ж�
		if ((m_cbPlayerStatus[wNextPlayer]==GUS_PLAYING)&&(m_cbShowHand[wNextPlayer]==FALSE))break;
	}

	//�жϵ�ע
	bool bFinishTurn=false;
	if (m_wOperaCount>=m_wPlayerCount)
	{
		bool bValid = false;
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//�жϵ�ע
			if ((m_cbPlayerStatus[i] == GUS_PLAYING) && (m_lTableScore[i] < m_lBalanceScore) && (m_cbShowHand[i] == FALSE))
			{
				bValid = true;
				break;
			}
		}

		//���ֽ���
		//if (i==m_wPlayerCount) bFinishTurn=true;
		if (!bValid) bFinishTurn = true;
		
	}

	//�ж����
	WORD wPlayCount = 0,wShowCount = 0;
	for (BYTE i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayerStatus[i]!=GUS_PLAYING) continue;

		if (m_cbShowHand[i]==TRUE) wShowCount++;
		wPlayCount++;
	}

	//ֻʣһ���û��(��Ϊ����ע�����������)����ȫ��
	if(!bFinishTurn && wPlayCount<=wShowCount+1 && m_lTableScore[wNextPlayer]>=m_lBalanceScore) bFinishTurn=true;

	//�����ж�
	if (bFinishTurn && wShowCount==0)
	{
		//��������
		WORD wNoAddCount=0;
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if (m_cbPlayerStatus[i]==GUS_PLAYING && m_lTableScore[i]==0)wNoAddCount++;
		}

		//���ƽ���
		if (wPlayCount==wNoAddCount)
		{
			//��������
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			GameEnd.cbEndReason=END_REASON_PASS;
			CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

			//�������
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if (m_lLoseScore[i]<0 || m_cbPlayerStatus[i]==GUS_PLAYING) GameEnd.lGameScore[i]=0;
			}

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//������Ϸ
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	}

	//������ע
	if (!bFinishTurn)
	{
		//��ǰ�û�
		m_wCurrentUser=wNextPlayer;

		//���ñ���		
		m_lTurnMinScore = min(m_lBalanceScore-m_lTableScore[m_wCurrentUser],m_lUserScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser]);
		m_lTurnMaxScore = m_lUserScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser];

		//��С��ע
		if (m_CustomConfig.cbRuleOption&GR_ALLOW_BET_GGF)
		{
			//��������
			WORD wLastChairID=m_wFirstUser;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//����λ��
				WORD wTempChairID=(wChairID-i+m_wPlayerCount)%m_wPlayerCount;
				if (m_wFirstUser==wTempChairID) break;
 				if (m_cbPlayerStatus[wTempChairID]!=GUS_PLAYING) continue;					

				//�Ƚ���ע
				if (m_lTotalScore[wTempChairID]>m_lTotalScore[wLastChairID])
				{
					wLastChairID=wTempChairID;
				}

				break;
			}
 
			//�����ע			
			m_lMinAddScore = m_lTotalScore[wLastChairID]*2-m_lTotalScore[m_wCurrentUser]-m_lTurnMinScore;
		}

		//������Ϣ
		SendUserAddInfo(wChairID,lScore,m_wCurrentUser,m_lTurnMinScore,m_lTurnMaxScore);

		//��ʱ��ʱ��
		SetUserGameTimer(m_wCurrentUser,25,m_dwDelayTime);

		return true;
	}

	//��������
	m_wOperaCount=0;	
	m_lBalanceScore = 0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//���ñ���
	m_cbBalanceCount++;

	//�жϿ���
	bool bOpenStatus = (m_cbBalanceCount!=3)?false:true;

	//������Ϣ
	if (bOpenStatus) SendUserAddInfo(wChairID,lScore,INVALID_CHAIR,0,0);

	//��������
	if (!bOpenStatus)
	{
		//����ж�
		BYTE cbSendCount=0;
		if (wPlayCount>0 && wShowCount>=wPlayCount-1)
		{
			//���ñ���
			bOpenStatus = true;
			cbSendCount = MAX_COUNT;
		}
		else cbSendCount = m_cbBalanceCount+2;

		//��ǰ�û�
		if (!bOpenStatus)
		{
			//�����С
			WORD wHeadMaxUser=INVALID_CHAIR;
			BYTE cbSingleCard[m_wPlayerCount];
			ZeroMemory(cbSingleCard,sizeof(cbSingleCard));
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				WORD wCurUser = (i+m_wBankerUser)%GAME_PLAYER;
				if (m_cbPlayerStatus[wCurUser]==GUS_PLAYING && m_cbShowHand[wCurUser]==FALSE)
				{
					if(wHeadMaxUser==INVALID_CHAIR)wHeadMaxUser=wCurUser;
					cbSingleCard[wCurUser]=m_GameLogic.GetSpeCardValue(&m_cbHandCardData[wCurUser][cbSendCount-1],1);
				}
			}

			//�Ƚϴ�С
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				WORD wCurUser = (i+m_wBankerUser)%GAME_PLAYER;
				if (m_cbPlayerStatus[wCurUser]!=GUS_PLAYING || wCurUser==wHeadMaxUser || m_cbShowHand[wCurUser]==TRUE)continue;
				
				if (cbSingleCard[wCurUser]==cbSingleCard[wHeadMaxUser])
				{
					if (cbSingleCard[wCurUser]==3)
					{
						BYTE cbFirstValue=m_GameLogic.GetCardValue(m_cbHandCardData[wCurUser][cbSendCount-1]);
						BYTE cbNextValue=m_GameLogic.GetCardValue(m_cbHandCardData[wHeadMaxUser][cbSendCount-1]);

						if (cbFirstValue>cbNextValue)wHeadMaxUser=wCurUser;

					}
				}
				if (cbSingleCard[wCurUser]>cbSingleCard[wHeadMaxUser])wHeadMaxUser=wCurUser;
			}

			//��ǰ�û�
			m_wCurrentUser = wHeadMaxUser;

			//С����ע
			m_lTurnMinScore = 0;			
			m_lTurnMaxScore = m_lUserScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser];

			//��С��ע
			if (m_CustomConfig.cbRuleOption&GR_ALLOW_BET_GGF)
			{
				m_lMinAddScore = m_lTotalScore[m_wCurrentUser];
			}

			//������Ϣ
			SendUserAddInfo(wChairID,lScore,INVALID_CHAIR,m_lTurnMinScore,m_lTurnMaxScore);
		}
		//�������
		else 
		{
			SendUserAddInfo(wChairID,lScore,INVALID_CHAIR,0,0);
		}

		//������Ϣ
		SendSendCardInfo(cbSendCount,((!bOpenStatus)?m_wCurrentUser:INVALID_CHAIR));

		//��ʱ��ʱ��
		SetUserGameTimer(m_wCurrentUser,25,m_dwDelayTime);
	}

	//�û�����
	if (bOpenStatus)
	{	
		//����״̬
		m_pITableFrame->SetGameStatus(GS_TK_OPEN_CARD);

		//��������
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPEN_START);
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_START);

		m_pITableFrame->SetGameTimer(IDI_OPEN_CARD, (TIME_OPEN_CARD + m_dwDelayTime) *1000,1,0);
	
	}

	return true;
}

//�����¼�
bool CTableFrameSink::OnUserOpenCard(WORD wChairID, BYTE cbCardData[])
{
	//״̬Ч��
	ASSERT (m_pITableFrame->GetGameStatus()==GS_TK_OPEN_CARD);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_OPEN_CARD) return false;

	//�رն�ʱ��
	KillUserGameTimer(wChairID);

	//��������
	if (cbCardData!=NULL)
	{
		//Ч������
		BYTE cbTempData[MAX_COUNT];
		CopyMemory(cbTempData,cbCardData,sizeof(cbTempData));

		BYTE cbTempHandCardData[MAX_COUNT];
		CopyMemory(cbTempHandCardData,m_cbHandCardData[wChairID],sizeof(cbTempHandCardData));


		bool bRemove = m_GameLogic.RemoveCard(m_cbHandCardData[wChairID],MAX_COUNT,cbTempData,MAX_COUNT);
		ASSERT(bRemove);
		if(!bRemove)
		{
			CString strInfo=TEXT("�û����������쳣��");

			strInfo+=TEXT("\r\n��������Ϊ��");
			for(int i=0;i<MAX_COUNT;i++)
			{
				CString strTempInfo;
				strTempInfo.Format(TEXT(" %2x "),cbTempData[i]);
				strInfo+=strTempInfo;
			}
			strInfo+=TEXT("\r\nʵ������Ϊ��");
			for(int i=0;i<MAX_COUNT;i++)
			{
				CString strTempInfo;
				strTempInfo.Format(TEXT(" %2x "),cbTempHandCardData[i]);
				strInfo+=strTempInfo;
			}
			NcaTextOut(strInfo);

			//�Ƚϴ�С
			BYTE cbSpeCount = 2;
			BYTE cbValue=m_GameLogic.CompareCard(cbTempHandCardData,&cbTempHandCardData[cbSpeCount],cbSpeCount);
			if(cbValue==TRUE)
			{
				CopyMemory(&cbCardData[cbSpeCount],cbTempHandCardData,cbSpeCount);
				CopyMemory(cbCardData,&cbTempHandCardData[cbSpeCount],cbSpeCount);
			}
			else 
			{
				CopyMemory(cbCardData,cbTempHandCardData,MAX_COUNT);
			}

		}

		//ͷβ��С
		BYTE cbSpeCount = 2;
		BYTE cbValue=m_GameLogic.CompareCard(cbCardData,&cbCardData[cbSpeCount],cbSpeCount);
		ASSERT(cbValue!=TRUE);
		if(cbValue==TRUE)
		{
			return false;
		}

		//���⴦��
		m_GameLogic.ChangeSpeCard(cbCardData,MAX_COUNT);//!

		//�û��˿�
		CopyMemory(m_cbHandCardData[wChairID],cbCardData,sizeof(BYTE)*MAX_COUNT);
		m_cbOpenFinish[wChairID] = TRUE;

		//�������
		CMD_S_Open_Card OpenCard;
		ZeroMemory(&OpenCard,sizeof(OpenCard));
		OpenCard.wChairID=wChairID;

		////��������
		//for (WORD i=0;i<m_wPlayerCount;i++)
		//{
		//	if(m_pITableFrame->GetTableUserItem(i)==NULL)continue;
		//	m_pITableFrame->SendTableData(i,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));
		//}
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPEN_CARD, &OpenCard, sizeof(OpenCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));	
	}

	//��������
	BYTE bUserCount=0;
	for(WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbOpenFinish[i]==TRUE && m_cbPlayerStatus[i]==GUS_PLAYING)bUserCount++;
		else if(m_cbPlayerStatus[i]!=GUS_PLAYING) bUserCount++;
	}

	//������Ϸ
	if (bUserCount == m_wPlayerCount)
	{
		KillUserGameTimer(INVALID_CHAIR);
		m_pITableFrame->KillGameTimer(IDI_OPEN_CARD);
		return OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);
	}

	return true;
}

//��ʾ��Ϣ
void CTableFrameSink::PopBrowseAction(IServerUserItem *pIUserItem, LPCWSTR szTips, LPCWSTR szUrl,WORD wType)
{
	////��������
	//BYTE cbBuffer[SOCKET_PACKET] = {0};
	////���췢������
	//WORD wSendSize = 0;
	//CMD_CM_ActionMessage *pActionMessage = (CMD_CM_ActionMessage *)cbBuffer;
	//pActionMessage->nButtonType = MB_OKCANCEL;
	//pActionMessage->wType = wType;
	//lstrcpynW( pActionMessage->szString,szTips,CountArray(pActionMessage->szString) );
	//pActionMessage->wLength = lstrlenW(pActionMessage->szString)+1;

	//wSendSize = sizeof(CMD_CM_ActionMessage)-sizeof(pActionMessage->szString)+CountStringBufferW(pActionMessage->szString);

	////���춯����Ϣ
	//tagActionHead *pActionHead = (tagActionHead *)((BYTE *)cbBuffer+wSendSize);
	//pActionHead->uResponseID = IDOK;
	//pActionHead->cbActionType = ACT_BROWSE;

	//wSendSize += sizeof(tagActionHead);

	//tagActionBrowse *pActionBrowse = (tagActionBrowse *)((BYTE *)cbBuffer+wSendSize);
	//pActionBrowse->cbBrowseType = BRT_IE;
	//_snwprintf( pActionBrowse->szBrowseUrl,CountArray(pActionBrowse->szBrowseUrl),L"%s", szUrl);

	//pActionHead->wAppendSize = sizeof(tagActionBrowse)-sizeof(pActionBrowse->szBrowseUrl)+CountStringBufferW(pActionBrowse->szBrowseUrl);
	//wSendSize += pActionHead->wAppendSize;
	//ASSERT( wSendSize<=CountArray(cbBuffer) );

	////���Ͷ�����Ϣ
	//m_pITableFrame->SendUserData(pIUserItem,SUB_CM_ACTION_MESSAGE,cbBuffer,wSendSize);
}

//��ȡ��ע
LONGLONG CTableFrameSink::GetMinAddScore(WORD wChairID)
{
	if (m_cbBalanceCount==0 && m_wCurrentUser==wChairID)
	{
		if (m_lTurnMinScore == 0)
		{
			return (m_CustomConfig.lMinBetAmount-1)*m_lCellScore;
		}
	}

	return m_lMinAddScore;
}

//��ȡ����
LONGLONG CTableFrameSink::GetUserScore(WORD wChairID)
{
	//��ѯ�û�
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if (pIServerUserItem==NULL) return 0L;

	//��ȡ����
	LONGLONG lUserScore=pIServerUserItem->GetUserFreeScore();
	LONGLONG lMaxBoboCount=m_CustomConfig.lMaxBoboMultiple*m_lCellScore;

	//Լս����
	if ((m_pGameServiceOption->wServerType& GAME_GENRE_BATTLE)!=0
		&& pIServerUserItem->GetScoreKind()==SCORE_KIND_BATTLE)
	{
		lUserScore=m_lUserFreeScore[wChairID];
	}

	//Լս����
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0
		&& pIServerUserItem->GetScoreKind() != SCORE_KIND_BATTLE)
	{
		lUserScore = min(lUserScore, m_lUserFreeScore[wChairID]);
	}


	return lUserScore;
}

//�Ƚ��˿�
int CTableFrameSink::CompareHandCard(WORD wChairID1,WORD wChairID2,tagCompareResult & CompareResult)
{
	//ׯ������
	BYTE cbAllowFirster=(m_CustomConfig.cbRuleOption&GR_ALLOW_FIRSTER)?TRUE:FALSE;

	//�Ƚϴ�С
	BYTE cbSpeCount=2;
	INT nTailResult=m_GameLogic.CompareCard(m_cbHandCardData[wChairID1],m_cbHandCardData[wChairID2],cbSpeCount);
	INT nHeadResult=m_GameLogic.CompareCard(&m_cbHandCardData[wChairID1][cbSpeCount],&m_cbHandCardData[wChairID2][cbSpeCount],cbSpeCount);
	if (nHeadResult==0xff) nHeadResult=-1;
	if (nTailResult==0xff) nTailResult=-1;

	//ͷ�����
	if (nHeadResult==FALSE && cbAllowFirster==TRUE)
	{
		//��� ׯ������
		if (wChairID1==m_wBankerUser) nHeadResult=TRUE;
		if (wChairID2==m_wBankerUser) nHeadResult=-1;
		if (wChairID1!=m_wBankerUser && wChairID2!=m_wBankerUser)
		{
			//��ʱ���ׯ����
			WORD wChairDis1=(wChairID1+(wChairID1<m_wBankerUser?GAME_PLAYER:0))-m_wBankerUser;
			WORD wChairDis2=(wChairID2+(wChairID2<m_wBankerUser?GAME_PLAYER:0))-m_wBankerUser;
			
			//���ñ���
			nHeadResult=(wChairDis1<wChairDis2)?TRUE:-1;
		}
	}

	//β�����
	if (nTailResult==FALSE && cbAllowFirster==TRUE)
	{
		//��� ׯ������
		if (wChairID1==m_wBankerUser)
		{
			nTailResult=TRUE;
		}

		if (wChairID2==m_wBankerUser)
		{
			nTailResult=-1;
		}

		if (wChairID1!=m_wBankerUser && wChairID2!=m_wBankerUser)
		{
			//��ʱ���ׯ����
			WORD wChairDis1=(wChairID1+(wChairID1<m_wBankerUser?GAME_PLAYER:0))-m_wBankerUser;
			WORD wChairDis2=(wChairID2+(wChairID2<m_wBankerUser?GAME_PLAYER:0))-m_wBankerUser;
			
			//���ñ���
			nTailResult=(wChairDis1<wChairDis2)?TRUE:-1;
		}
	}

	//�������
	/*if (m_GameLogic.GetCardType(m_cbHandCardData[wChairID1],2)==m_GameLogic.GetCardType(m_cbHandCardData[wChairID2],2))
	{
		if (m_GameLogic.GetCardType(m_cbHandCardData[wChairID1],2)==0)
		{
			nTailResult=FALSE;			
		}
	} */

	//ͷβ���
	CompareResult.nHeadResult=nHeadResult;
	CompareResult.nTailResult=nTailResult;

	//���ս��
	if ((nHeadResult+nTailResult)>0) CompareResult.nResult=1;
	if ((nHeadResult+nTailResult)<0) CompareResult.nResult=-1;
	if ((nHeadResult+nTailResult)==0) CompareResult.nResult=0;		

	return CompareResult.nResult;
}

//���Խ�ɢ
void CTableFrameSink::TryDismissGame()
{
	//��������
	BYTE cbPlayerCount=0;

	//ͳ������
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//�û��ж�	
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;
		if (pIServerUserItem->GetUserInfo()->cbUserStatus == US_LOOKON) continue;


		//�ȽϷ���
		if (GetUserScore(i) >= m_lCellScore * MIN_MUTIPLIE)
		{
			cbPlayerCount++;
		}
	}

	//ʣ��1�˽�ɢ����
	if (cbPlayerCount==1 && m_pIEventBattleSink!=NULL)
	{
		m_pIEventBattleSink->BattleFinish(BFR_NORMAL);
		m_wBattleCount = 0;
	}
}

//���·���
void  CTableFrameSink::UpdateUserScore(WORD wChairID)
{
	//��������
	CMD_S_UpdateScore UpdateScore;
	ZeroMemory(&UpdateScore,sizeof(UpdateScore));

	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//�û��ж�			
			if (m_pITableFrame->GetTableUserItem(i)==NULL)
			{
				continue;
			}

			//���ñ���
			UpdateScore.wChairID=i;
			UpdateScore.lUserScore=GetUserScore(i);

			//������Ϣ
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_UPDATE_SCORE,&UpdateScore,sizeof(UpdateScore));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UPDATE_SCORE, &UpdateScore, sizeof(UpdateScore));
		}
	}
	else
	{
		//���ñ���
		UpdateScore.wChairID=wChairID;
		UpdateScore.lUserScore=GetUserScore(wChairID);

		//������Ϣ
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_UPDATE_SCORE,&UpdateScore,sizeof(UpdateScore));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UPDATE_SCORE, &UpdateScore, sizeof(UpdateScore));
	}
}

//��¼�쳣
void CTableFrameSink::WriteErrorLog(tagScoreInfo ScoreInfoArray[], WORD wScoreCount,int nType)
{
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(i);

		if (pUserItem!=NULL)
		{
			LONGLONG lUserScore=GetUserScore(i);
			if (ScoreInfoArray[i].lScore+lUserScore<0 && m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
			{
				CString strType=TEXT("");
				strType.Format(TEXT("д�����ͣ�%d"),nType);
				NcaTextOut(strType);

				//��¼����
				CString strInfo=TEXT("");

				//strInfo.Format(TEXT("[%d]���ָ��֣�д�������%I64d��ʵ�ʷ�����%I64d,��ע��Ŀ��%I64d,�׷֣�%I64d"),pUserItem->GetUserID(),ScoreInfoArray[i].lScore,lUserScore,m_lTableScore[i],10*m_lCellScore);
				strInfo.Format(TEXT("[%d]���ָ��֣���ʼʱ�û�������%I64d�������û�������%I64d"),pUserItem->GetUserID(),m_lUserBeginScore[i],lUserScore);

				NcaTextOut(strInfo);

				CString strRoomType;
				strRoomType=TEXT("��ҷ���");

				strInfo.Format(TEXT("д�������%I64d,��ע��Ŀ��%I64d,�׷֣�%I64d,�������ͣ�%s"),ScoreInfoArray[i].lScore,m_lTableScore[i],MIN_MUTIPLIE*m_lCellScore,strRoomType);
				NcaTextOut(strInfo);

				if(ScoreInfoArray[i].lScore<0&&lUserScore>=0)
				{
					ScoreInfoArray[i].lScore=-lUserScore;
				}

			}
		}
	}
}

//����д��־
VOID CTableFrameSink::WriteControlLog(string & str)
{
	str += "\n";

	//����·��
	TCHAR szWorkDir[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szWorkDir, CountArray(szWorkDir));

	//�ļ�Ŀ¼
	TCHAR szServiceDir[MAX_PATH] = TEXT("");
	TCHAR szDircetory[MAX_PATH] = TEXT("");
	CTime TimeNow = CTime::GetCurrentTime();

	_sntprintf_s(szDircetory, CountArray(szDircetory), TEXT("%s\\CheXuan"), szWorkDir);

	if (!PathIsDirectory(szDircetory))
	{
		CreateDirectory(szDircetory, NULL);
	}

	_sntprintf_s(szServiceDir, CountArray(szServiceDir), TEXT("%s\\CheXuan\\%d_%d.txt"), szWorkDir, TimeNow.GetDay(),m_pITableFrame->GetTableID());

	CString filePath;
	CFile  file;
	//��ȡ�ļ���
	filePath.AppendFormat(TEXT("%s"), szServiceDir);

	if (file.Open(filePath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite) == TRUE)
	{
		try
		{
			file.SeekToEnd();
			file.Write(str.c_str(), str.size());
		}
		catch (...)
		{
			AfxMessageBox(TEXT("д�ļ��쳣��"));
		}

		//�رվ��
		file.Close();
	}
}

//////////////////////////////////////////////////////////////////////////
