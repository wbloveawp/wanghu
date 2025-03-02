#include "StdAfx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
#define	STOCK_TAX						2									//回扣宏量

//////////////////////////////////////////////////////////////////////////

#define IDI_OPEN_CARD			    1				//开牌定时
#define IDI_OPERATE_TIMEOUT			2				//操作超时

//时间标识
#ifdef _DEBUG
#define TIME_DELAY					5				//延时时间
#define TIME_OPEN_CARD				30				//开牌时间	
#define TIME_OPERATE_CARD			15				//操作定时器	
#define TIME_DISMISS_TABLE			120				//解散时间
#define TIME_KICK_OUT_1				15				//踢人时间
#define TIME_KICK_OUT_2				30				//踢人时间
#else
#define TIME_DELAY					5				//延时时间
#define TIME_OPEN_CARD				30				//开牌时间	
#define TIME_OPERATE_CARD			15				//操作定时器	
#define TIME_DISMISS_TABLE			300				//解散时间
#define TIME_KICK_OUT_1				25				//踢人时间
#define TIME_KICK_OUT_2				40				//踢人时间
#endif

//////////////////////////////////////////////////////////////////////////


//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量
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

	//变量定义
	m_dwDelayTime=TIME_DELAY;

	//配置变量
	ZeroMemory(&m_CustomConfig,sizeof(m_CustomConfig));

	//手牌数据
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));	

	//组件变量
	m_pITableFrame=NULL;
	m_pIEventBattleSink =NULL;
	m_pIEventBattleSink=NULL;
	m_pGameServiceOption=NULL;	
	m_pIGameServiceDynamicRule=NULL;

	//规则变量
	m_bGameRule=false;
	m_wSetRuleUser=INVALID_CHAIR;

	//控制变量
	m_bAllotCard=false;
	m_wControBankerUser=INVALID_CHAIR;
	ZeroMemory(m_cbControlCardData,sizeof(m_cbControlCardData));

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询--检测相关信息版本
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

//初始化
bool  __cdecl CTableFrameSink::InitTableFrameSink(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx != NULL);
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrameItem);
	if (m_pITableFrame == NULL) return false;

	//查询配置
	if (m_pITableFrame->GetGameServiceOption()->wServerType & GAME_GENRE_BATTLE)
	{
		m_pIEventBattleSink = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IEventBattleSink);
		if (m_pIEventBattleSink == NULL)
		{
			CTraceService::TraceString(TEXT("游戏桌子 CTableFrameSink 查询 IEventBattleSink 接口失败"), TraceLevel_Exception);
			return false;
		}
	}


	//游戏配置
	//m_pGameServiceAttrib = m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();

	//获取配置
	tagCustomConfig * pCustomConfig = (tagCustomConfig *)m_pGameServiceOption->cbCustomRule;
	m_CustomConfig = *pCustomConfig;

	//设置规则
	m_GameLogic.SetRuleOption(m_CustomConfig.cbRuleOption);

	//开始模式
		//约战房间
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
	{
		m_pITableFrame->SetStartMode(StartMode_FullReady);
	}
	else
	{
		m_pITableFrame->SetStartMode(StartMode_AllReady);
	}


	//设置变量	
	m_lCellScore = m_pGameServiceOption->lCellScore;
	m_bGameRule = (m_CustomConfig.cbRuleOption & GR_ALLOW_CUSTOM_SCORE) == 0;

	//调整配置
	if (m_CustomConfig.lMinBetAmount < m_lCellScore * 2)
	{
		m_CustomConfig.lMinBetAmount = m_lCellScore * 2;
	}

	//约战房间
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
	{
		m_dwDelayTime = TIME_DELAY + 1800;

		//设置倍数
		//if (m_pIGameServiceDynamicRule!=NULL) m_pIGameServiceDynamicRule->SetMaxGameMultiple(MIN_MUTIPLIE);
	}


	//坐下倍数
	m_pITableFrame->SetSitdownGameMultiple(MIN_MUTIPLIE);

	//起立倍数
	m_pITableFrame->SetStandupGameMultiple(MIN_MUTIPLIE);

	return true;
}

//复位桌子
void  __cdecl CTableFrameSink::ResetTableFrameSink()
{
	//游戏变量
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

	//手牌数据
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));
	ZeroMemory(m_cbControlCardData,sizeof(m_cbControlCardData));

	return;
}

////开始模式
//enStartMode  CTableFrameSink::GetGameStartMode()
//{
//	return m_GameStartMode;
//}

//游戏状态
bool  __cdecl CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	ASSERT (wChairID<m_wPlayerCount && (m_cbPlayerStatus[wChairID]==GUS_PLAYING || m_lLoseScore[wChairID]<0));
	if(wChairID<m_wPlayerCount && (m_cbPlayerStatus[wChairID]==GUS_PLAYING || m_lLoseScore[wChairID]<0)) return true;

	return false;
}

//游戏开始
bool  __cdecl CTableFrameSink::OnEventGameStart()
{
	//设置状态
	m_pITableFrame->SetGameStatus(GS_TK_INVEST);

	++m_wBattleCount;

	//用户状态
	WORD wPlayerCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//获取用户
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

			//统计人数
			if (m_cbPlayerStatus[i]==GUS_PLAYING) wPlayerCount++;
		}
	}

	//结束游戏
	if (wPlayerCount<2)
	{
		//结束游戏
		return m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);
	}

	//更新簸簸
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

	//控制庄家
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

	//首局随机
	if (m_wBankerUser==INVALID_CHAIR)
	{
		m_wBankerUser = rand()%m_wPlayerCount;
	}

	if(m_bAllotCard)
	{
		//确认庄家
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

	//当前用户
	m_wCurrentUser=m_wBankerUser;

	//更新分数
	UpdateUserScore(INVALID_CHAIR);

	//设置变量
	CMD_S_GameStart GameStart;
	GameStart.wBankerUser=m_wBankerUser;	
	CopyMemory(GameStart.lUserScore,m_lUserScore,sizeof(m_lUserScore));
	CopyMemory(GameStart.cbPlayStatus,m_cbPlayerStatus,sizeof(m_cbPlayerStatus));

	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_pITableFrame->GetTableUserItem(i)==NULL)continue;

		//发送分数
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

		//超时定时器
		SetUserGameTimer(i,25,m_dwDelayTime);
	}

	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	

	return true;
}

//游戏结束
bool  __cdecl CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			//取消配牌
			if(m_bAllotCard)
			{
				m_bAllotCard=false;
				m_wControBankerUser=INVALID_CHAIR;
				ZeroMemory(m_cbControlCardData,sizeof(m_cbControlCardData));
			}

			//构造数据
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			GameEnd.cbEndReason=END_REASON_NORMAL;
			CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(GameEnd.cbCardData));

			//变量定义
			WORD wGiveupCount=0;
			WORD wPlayerCount=0;

			//判断放弃
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				if (m_cbPlayerStatus[i]==GUS_NULL && m_lTotalScore[i]>0)
				{
					wGiveupCount++;
				}
			}

			//结束得分
			BYTE cbPlayCount=0;
			BYTE cbPlayerStatus[GAME_PLAYER];
			BYTE cbAllowFirster=(m_CustomConfig.cbRuleOption&GR_ALLOW_FIRSTER)?TRUE:FALSE;
			CopyMemory(cbPlayerStatus,m_cbPlayerStatus,sizeof(cbPlayerStatus));
			
			do
			{
				//辅助变量
				BYTE cbSpeCount = 2;
				struct LoseInfo
				{
					WORD wPlyObjUser[GAME_PLAYER];
					WORD wPlyObjCount;
				}LoseUserInof[GAME_PLAYER];
				ZeroMemory(LoseUserInof,sizeof(LoseUserInof));

				//判断输家
				BYTE cbLoseUser[GAME_PLAYER];
				ZeroMemory(cbLoseUser,sizeof(cbLoseUser));

                for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (cbPlayerStatus[i]!=GUS_PLAYING)continue;

					for (WORD j=0;j<m_wPlayerCount;j++)
					{
						if (cbPlayerStatus[j]!=GUS_PLAYING || i==j) continue;

						//比较手牌
						tagCompareResult CompareResult;
						if (CompareHandCard(i,j,CompareResult)==-1) 
						{
							cbLoseUser[i]=TRUE;
							break;
						}
					}
				}

				//查找输家   
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (cbPlayerStatus[i]!=GUS_PLAYING || cbLoseUser[i]==FALSE)continue;

					for (WORD t=0;t<m_wPlayerCount;t++)
					{
						//庄家优先
						WORD j=(t+m_wBankerUser)%GAME_PLAYER;
						if (cbPlayerStatus[j]!=GUS_PLAYING || i==j /*|| cbLoseUser[j]==TRUE*/)continue;

						//比较手牌
						tagCompareResult CompareResult;
						if (CompareHandCard(i,j,CompareResult)==-1) 
						{
							LoseUserInof[i].wPlyObjUser[LoseUserInof[i].wPlyObjCount++] = j;
						}
					}
				}

				//玩家得分
				LONGLONG lUserGetScore[GAME_PLAYER];
				ZeroMemory(lUserGetScore,sizeof(lUserGetScore));

				//下注分数
				LONGLONG lTotalScore[GAME_PLAYER];
				CopyMemory(lTotalScore,m_lTotalScore,sizeof(m_lTotalScore));

				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (cbPlayerStatus[i]!=GUS_PLAYING) continue;

					LoseInfo *pLoseInfo = &LoseUserInof[i];
					WORD wPlyCount=pLoseInfo->wPlyObjCount;

					//最大输分
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

					//调整分数					
					if (lTotalScore[i]<lMaxLostScore) lMaxLostScore=lTotalScore[i];

					//计算得分
					while (wPlyCount>0 && lMaxLostScore>0)
					{
						//查找大赢家
						WORD wIndex = 0;
						WORD wMaxUser = pLoseInfo->wPlyObjUser[0];
						for (WORD j=1;j<wPlyCount;j++)
						{
							//当前用户
							WORD wCurUser = pLoseInfo->wPlyObjUser[j];
							if (cbPlayerStatus[wCurUser]!=GUS_PLAYING) continue;

							//比较手牌
							tagCompareResult CompareResult;
							CompareHandCard(wMaxUser,wCurUser,CompareResult);

							//结果小于
							if (CompareResult.nResult==-1) 
							{
								wIndex=j;
								wMaxUser=wCurUser;
							}

							//结果等于
							if (CompareResult.nResult==0 && CompareResult.nTailResult<0)
							{
								wIndex=j;
								wMaxUser=wCurUser;
							}
						}

						//结算得分						
						LONGLONG lTempScore=__min(lTotalScore[wMaxUser],lMaxLostScore);						
						lUserGetScore[wMaxUser]+=lTempScore;
						lMaxLostScore-=lTempScore;
						lUserGetScore[i]-=lTempScore;
						m_lTotalScore[i]-=lTempScore;						

						//判断结束
						if (lMaxLostScore==0 || wPlyCount==1) break;

						//清理用户
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

				//保存结果
				for (WORD i=0;i<GAME_PLAYER;i++)
				{
					if (cbPlayerStatus[i]!=GUS_PLAYING) continue;
					GameEnd.lGameScore[i]+=lUserGetScore[i];
				}

			} while(false);

			///////////////////////////////////////////////分皮处理///////////////////////////////////////////

			//放弃玩家结算皮分 
			if (wGiveupCount>0)
			{
				//变量定义
				WORD wPlayerCount=0;
				WORD wMaxChairID=INVALID_CHAIR;
				BYTE cbWinnerOrder[GAME_PLAYER];
				BYTE cbCompareStatus[GAME_PLAYER];

				//设置变量
				ZeroMemory(cbWinnerOrder,sizeof(cbWinnerOrder));
				CopyMemory(cbCompareStatus,m_cbPlayerStatus,sizeof(m_cbPlayerStatus));

				//尾牌排序
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					//设置变量
					wMaxChairID=INVALID_CHAIR;

					for (WORD j=0;j<m_wPlayerCount;j++)
					{
						//状态过滤
						if (cbCompareStatus[j]!=GUS_PLAYING) continue;

						//设置变量
						if (wMaxChairID==INVALID_CHAIR) 
						{
							wMaxChairID=j;						
							continue;
						}

						//比较尾牌
						INT nTailResult=m_GameLogic.CompareCard(m_cbHandCardData[j],m_cbHandCardData[wMaxChairID],2);
						if (nTailResult==0xFF) continue;

						//结果大于
						if (nTailResult==TRUE) wMaxChairID=j;

						//结果相等
						if (nTailResult==FALSE)
						{
							//庄家先手
							if (cbAllowFirster==TRUE)
							{
								//庄家优先
								if (j==m_wBankerUser) wMaxChairID=j;					
								if (j!=m_wBankerUser && wMaxChairID!=m_wBankerUser)
								{
									//逆时针近庄优先
									WORD wChairDis1=(j+(j<m_wBankerUser?GAME_PLAYER:0))-m_wBankerUser;
									WORD wChairDis2=(wMaxChairID+(wMaxChairID<m_wBankerUser?GAME_PLAYER:0))-m_wBankerUser;

									//比较位置
									if (wChairDis1<wChairDis2) wMaxChairID=j;						
								}									
							}
							else
							{
								//比较尾牌
								INT nHeadResult=m_GameLogic.CompareCard(&m_cbHandCardData[j][2],&m_cbHandCardData[wMaxChairID][2],2);
								if (nHeadResult==0xFF) continue;

								//结果大于
								if (nHeadResult==TRUE) wMaxChairID=j;

								//结果相等
								if (nHeadResult==FALSE)
								{
								}
							}
						}					
					}

					//设置状态
					if (wMaxChairID!=INVALID_CHAIR)
					{
						cbCompareStatus[wMaxChairID]=GUS_NULL;
						cbWinnerOrder[wPlayerCount++]=wMaxChairID;
					}
				}

				//分皮结算
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					//丢弃玩家
					if (m_cbPlayerStatus[i]!=GUS_PLAYING && m_lLoseScore[i]!=0)
					{
						//设置分数
						LONGLONG lTotalScore=__max(m_lLoseScore[i],-m_lLoseScore[i]);
						GameEnd.lGameScore[i]=-lTotalScore;		

						do
						{
							for (WORD j=0; j<wPlayerCount; j++)
							{
								//计算输分
								WORD wWinerID=cbWinnerOrder[j];
								LONGLONG lLoseScore=__min(lTotalScore,m_lUserScore[wWinerID]);

								//设置分数								
								GameEnd.lGameScore[wWinerID]+=lLoseScore;

								//更新分数
								lTotalScore-=lLoseScore;
								m_lTotalScore[i]-=lLoseScore;

								if (lTotalScore==0 || m_lTotalScore[i]==0) break;
							}
						} while (lTotalScore>0 && m_lTotalScore[i]>0);											
					}
				}	
			}

			///////////////////////////////////////////////分皮处理///////////////////////////////////////////

			//计算税收
			LONGLONG lRevenue=m_pGameServiceOption->lRevenue;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (GameEnd.lGameScore[i]>0)
				{
					GameEnd.lGameTax[i]=m_pITableFrame->AccountRevenue(GameEnd.lGameScore[i]);
					
					if (GameEnd.lGameTax[i]>0)
						GameEnd.lGameScore[i]-=GameEnd.lGameTax[i];
				}

				//用户总币
				if (m_lUserScore[i]>0) m_lUserScore[i]+=GameEnd.lGameScore[i];
			}

			//得到最大用户
			WORD wMaxUser=INVALID_CHAIR;

			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//庄家优先
				WORD j=(i+m_wBankerUser)%GAME_PLAYER;
				
				if (GameEnd.lGameScore[j]>0)
				{
					wMaxUser = j;
				}
			}

			GameEnd.wMaxUser=wMaxUser;
			
			tagScoreInfo ScoreInfo[GAME_PLAYER];
			ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

			//写入积分
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

				//更新分数								
				if ((m_pGameServiceOption->wServerType& GAME_GENRE_BATTLE)!=0)
					//&& pIServerUserItem->GetScoreKind()==SCORE_KIND_BATTLE)
				{
					m_lUserFreeScore[i]+=ScoreInfo[i].lScore;
				}
			}

			////发送信息
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
			//记录日志
			WriteErrorLog(ScoreInfo,CountArray(ScoreInfo),1);
			m_pITableFrame->WriteTableScore(ScoreInfo, wPlayerCnt);

			//更新分数
			UpdateUserScore(INVALID_CHAIR);

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			//尝试解散
			if ((m_pGameServiceOption->wServerType& GAME_GENRE_BATTLE)!=0)
			{
				TryDismissGame();
			}

			return true;
		}
	case GER_USER_LEAVE:		//用户强退
	case GER_NETWORK_ERROR:		//网络中断
		{
			//游戏状态
			WORD wGameStatus=m_pITableFrame->GetGameStatus();

			//用户放弃
			bool bGiveUp=false;
			if(WORD(wChairID-GAME_PLAYER)<WORD(GAME_PLAYER) && pIServerUserItem!=NULL)
			{
				bGiveUp=true;
				wChairID-=GAME_PLAYER;
			}

			//放弃强退
			bool bHaveGiveUp=false;
			if (m_cbPlayerStatus[wChairID]!=GUS_PLAYING) bHaveGiveUp=true;

			//清理数据
			m_cbShowHand[wChairID]=FALSE;
			m_cbPlayerStatus[wChairID]=GUS_NULL;

			//用户人数
 			WORD wUserCount =0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (m_cbPlayerStatus[i]==GUS_PLAYING) wUserCount++;
			}

			//扣除分数
			if (!bGiveUp/* || wUserCount<2*/)
			{
				//最大下注
				LONGLONG lMaxScore=0;
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (m_cbPlayerStatus[i]!=GUS_PLAYING || i==wChairID)continue;

					if (lMaxScore<m_lUserScore[i])lMaxScore=m_lUserScore[i];
				}

				//最大输分
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (m_lLoseScore[i]==0)continue;
					LONGLONG lLose = __max(m_lLoseScore[i],-m_lLoseScore[i]);
					if (lMaxScore<lLose)lMaxScore=lLose;
				}

				//特牌判断
				if (m_GameLogic.IsHoldSpecialCard(m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
				{
					//实质下注
					m_lLoseScore[wChairID]=__min(lMaxScore,m_lTotalScore[wChairID]);

					tagScoreInfo ScoreInfo[GAME_PLAYER];
					ZeroMemory(ScoreInfo,sizeof(ScoreInfo));

					//扣除下注
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

					//记录日志
					WriteErrorLog(ScoreInfo,CountArray(ScoreInfo),2);
					m_pITableFrame->WriteTableScore(ScoreInfo, wPlayerCount);
					m_lUserScore[wChairID]-=m_lLoseScore[wChairID];

					//更新分数					
					if ((m_pGameServiceOption->wServerType& GAME_GENRE_BATTLE)!=0 )
						//&& pIServerUserItem->GetScoreKind() == SCORE_KIND_BATTLE)
					{
						m_lUserFreeScore[wChairID]+=ScoreInfo[wChairID].lScore;
					}
				}				
			}
			else 
			{
				//特牌判断
				if (m_GameLogic.IsHoldSpecialCard(m_cbHandCardData[wChairID],m_cbHandCardCount[wChairID])==false)
				{
					m_lLoseScore[wChairID]=-m_lTotalScore[wChairID];					
				}				
			}

			//发送消息
			if (!bHaveGiveUp)
			{
				CMD_S_GiveUp GiveUp;
				ZeroMemory(&GiveUp,sizeof(GiveUp));
				GiveUp.wGiveUpUser=wChairID;
				GiveUp.wGameStatus= wGameStatus;
				m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
				m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GIVE_UP,&GiveUp,sizeof(GiveUp));
			}

			//结束游戏
			if (wUserCount<2)
			{
				//构造数据
				CMD_S_GameEnd GameEnd;
				ZeroMemory(&GameEnd,sizeof(GameEnd));
				GameEnd.cbEndReason=END_REASON_EXIT;
				CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(GameEnd.cbCardData));

				//胜利玩家
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

				//总注数目
				LONGLONG lAllGold=0,lCount=0;
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (m_lLoseScore[i]!=0 && i!=wWinner)
					{
						lCount++;
						LONGLONG lLose = __max(m_lLoseScore[i],-m_lLoseScore[i]);

						lAllGold+=lLose;
						GameEnd.lGameScore[i]=-lLose;

						//放弃玩家
						if (m_lLoseScore[i]<0)
						{
							
							ScoreInfo[i].lScore = m_lLoseScore[i];
							ScoreInfo[i].ScoreType = ScoreType_Lose;
							
							m_lUserScore[i]+=m_lLoseScore[i];
							m_lLoseScore[i]*=-1;
						}
					}
				}

				//实质赢注
				lAllGold = __min(lAllGold,m_lUserScore[wWinner]*lCount);

				//胜方得分
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

				//更新分数
				if ((m_pGameServiceOption->wServerType& GAME_GENRE_BATTLE)!=0)
					//&& pIServerUserItem->GetScoreKind() == SCORE_KIND_BATTLE)
				{
					for (WORD i=0;i<m_wPlayerCount;i++) 
					{
						m_lUserFreeScore[i]+=ScoreInfo[i].lScore;
					}					
				}

				//发送信息
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

				//记录日志
				WriteErrorLog(ScoreInfo,CountArray(ScoreInfo),3);
				m_pITableFrame->WriteTableScore(ScoreInfo, wPlayerCount);

				//更新分数
				UpdateUserScore(INVALID_CHAIR);

				//结束游戏
				m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

				//尝试解散
				if ((m_pGameServiceOption->wServerType& GAME_GENRE_BATTLE)!=0)
				{
					TryDismissGame();
				}

				//关闭定时器
				m_pITableFrame->KillGameTimer(IDI_OPEN_CARD);
			}
			//用户放弃
			else if(wGameStatus==GS_TK_SCORE)
			{
				if (m_wCurrentUser==wChairID) OnUserAddScore(wChairID,0,CODE_PASS,true);
			}
			//下本放弃
			else if(wGameStatus==GS_TK_INVEST)
			{
				OnUserInvest(wChairID,0);
			}
			//开牌放弃
			else OnUserOpenCard(wChairID,NULL);

			return true;
		}
	case GER_DISMISS:
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//配置判断
			if ((m_CustomConfig.cbRuleOption&GR_ALLOW_CUSTOM_SCORE)!=0)
			{
				m_bGameRule=false;
				m_wBankerUser=INVALID_CHAIR;
				m_wSetRuleUser=INVALID_CHAIR;
				m_lCellScore=0L;
			}

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	}

	

	return false;
}

//最大下注
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


//关闭定时器
void CTableFrameSink::KillUserGameTimer(WORD wChairID)
{
	if (wChairID==INVALID_CHAIR)
	{		
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//状态判断
			if (m_cbPlayerStatus[i]!=GUS_PLAYING) continue;

			//关闭定时器
			m_pITableFrame->KillGameTimer(IDI_OPERATE_TIMEOUT+i);
		}
	}
	else
	{
		m_pITableFrame->KillGameTimer(IDI_OPERATE_TIMEOUT+wChairID);
	}
}

//设置定时器
void CTableFrameSink::SetUserGameTimer(WORD wChairID,DWORD dwTimeOut,DWORD dwDelayTime)
{
	//参数校验
	ASSERT (wChairID<GAME_PLAYER);
	if (wChairID>=GAME_PLAYER) return;

	//超时定时器
	m_pITableFrame->SetGameTimer(IDI_OPERATE_TIMEOUT+wChairID,(dwTimeOut+dwDelayTime)*1000L,1,NULL);
}

//发送场景
bool  __cdecl CTableFrameSink::OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:	//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//设置变量
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

			//设置变量
			StatusFree.wSetRuleUser=m_wSetRuleUser;
			StatusFree.cbGameRule=(m_bGameRule==true)?TRUE:FALSE;
			StatusFree.cbRuleOption=m_CustomConfig.cbRuleOption;

			//发送场景
			m_pITableFrame->OnEventSendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));

			//更新分数
			UpdateUserScore(INVALID_CHAIR);

			return true;
		}
	case GS_TK_INVEST:		//下本状态
		{			
			//构造数据
			CMD_S_StatusInvest StatusInvest;
			ZeroMemory(&StatusInvest,sizeof(StatusInvest));

			//设置数据
			StatusInvest.wBankerUser=m_wBankerUser;
			StatusInvest.lCellScore=m_lCellScore;
			StatusInvest.cbRuleOption=m_CustomConfig.cbRuleOption;
			StatusInvest.cbGameRule=m_bUserReConnect[wChairID]?FALSE:TRUE;			
			CopyMemory(StatusInvest.lUserScore,m_lUserScore,sizeof(m_lUserScore));
			CopyMemory(StatusInvest.cbPlayStatus,m_cbPlayerStatus,sizeof(m_cbPlayerStatus));
			CopyMemory(StatusInvest.bInvestFinish,m_bInvestFinish,sizeof(m_bInvestFinish));

			m_pITableFrame->OnEventSendGameScene(pIServerUserItem,&StatusInvest,sizeof(StatusInvest));

			//更新分数
			UpdateUserScore(INVALID_CHAIR);

			//发送场景
			return true;
		}
	case GS_TK_SCORE:		//下注状态
		{
			//构造数据
			CMD_S_StatusScore StatusScore;
			ZeroMemory(&StatusScore,sizeof(StatusScore));

			//设置数据
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

			//明牌数据
			BYTE cbSpeCount=2;
			if (m_cbBalanceCount>0)
			{
				for (WORD i=0;i<m_wPlayerCount;i++)
				{
					if (m_cbPlayerStatus[i]!=GUS_PLAYING) continue;

					//用户扑克
					CopyMemory(StatusScore.cbMingCard[i],&m_cbHandCardData[i][cbSpeCount],sizeof(BYTE)*m_cbBalanceCount);
				}
			}

			//手上扑克
			//if (pIServerUserItem->GetUserInfo()->cbUserStatus!=US_LOOKON)
			{
				CopyMemory(StatusScore.cbHandCard,m_cbHandCardData[wChairID],sizeof(BYTE)*cbSpeCount);
			}

			//发送场景
			m_pITableFrame->OnEventSendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));

			//更新分数
			UpdateUserScore(INVALID_CHAIR);

			return true;
		}
	case GS_TK_OPEN_CARD:	//开牌状态
		{
			//构造数据
			CMD_S_StatusOpenCard StatusOpenCard;
			ZeroMemory(&StatusOpenCard,sizeof(StatusOpenCard));

			//设置数据
			StatusOpenCard.wBankerUser=m_wBankerUser;
			StatusOpenCard.lCellScore=m_lCellScore;
			StatusOpenCard.cbRuleOption=m_CustomConfig.cbRuleOption;
			StatusOpenCard.cbGameRule=m_bUserReConnect[wChairID]?FALSE:TRUE;			
			CopyMemory(StatusOpenCard.lUserScore,m_lUserScore,sizeof(m_lUserScore));
			CopyMemory(StatusOpenCard.lTotalScore,m_lTotalScore,sizeof(m_lTotalScore));
			CopyMemory(StatusOpenCard.cbPlayStatus,m_cbPlayerStatus,sizeof(m_cbPlayerStatus));
			CopyMemory(StatusOpenCard.cbOpenFinish,m_cbOpenFinish,sizeof(m_cbOpenFinish));

			//明牌数据
			BYTE cbSpeCount=2;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (m_cbPlayerStatus[i]!=GUS_PLAYING)continue;

				//用户扑克
				CopyMemory(StatusOpenCard.cbMingCard[i],&m_cbHandCardData[i][cbSpeCount],sizeof(BYTE)*cbSpeCount);
			}

			//手上扑克
			//if (pIServerUserItem->GetUserInfo()->cbUserStatus!=US_LOOKON)
			{
				CopyMemory(StatusOpenCard.cbHandCard,m_cbHandCardData[wChairID],sizeof(BYTE)*cbSpeCount);
			}

			//发送场景
			m_pITableFrame->OnEventSendGameScene(pIServerUserItem,&StatusOpenCard,sizeof(StatusOpenCard));

			//更新分数
			UpdateUserScore(INVALID_CHAIR);

			return true;
		}
	}

	//效验错误
	ASSERT(FALSE);

	return false;
}

//定时器事件
bool  __cdecl CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam)
{
	//操作超时
	if (dwTimerID>=IDI_OPERATE_TIMEOUT && dwTimerID<(IDI_OPERATE_TIMEOUT+GAME_PLAYER))
	{
		//关闭定时器
		m_pITableFrame->KillGameTimer(dwTimerID);	

		if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
		{
			m_pITableFrame->KillGameTimer(dwTimerID);
			m_pIEventBattleSink->BattleFinish(BFR_NORMAL);
			m_wBattleCount = 0;
			return true;
		}

		//变量定义
		WORD wChairID=(WORD)(dwTimerID-IDI_OPERATE_TIMEOUT);
		if (m_pITableFrame->GetTableUserItem(wChairID)==NULL)
		{
			return true;
		}		

		//获取状态
		BYTE cbGameStatus=m_pITableFrame->GetGameStatus();

		switch (cbGameStatus)
		{
		case GS_TK_INVEST:	 //下本状态
			{
				//放弃下本
				OnUserGiveUp(wChairID);
				break;
			}
		case GS_TK_SCORE:	 //下注状态
			{
				//放弃下注
				OnUserGiveUp(wChairID);
				break;
			}
		}

		return true;
	}

	//结束下注
	if (dwTimerID==IDI_OPEN_CARD)
	{
		//关闭定时器
		m_pITableFrame->KillGameTimer(IDI_OPEN_CARD);

		if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
		{
			m_pITableFrame->KillGameTimer(dwTimerID);
			m_pIEventBattleSink->BattleFinish(BFR_NORMAL);
			m_wBattleCount = 0;
			return true;
		}

		//判断状态
		if (m_pITableFrame->GetGameStatus()!=GS_TK_OPEN_CARD)
		{
			return true;
		}
		
		//发送数据
		for (WORD wChairID=0;wChairID< m_wPlayerCount;wChairID++)
		{
			if (m_cbPlayerStatus[wChairID]!=GUS_PLAYING)continue;
			if (m_cbOpenFinish[wChairID]==FALSE)
			{
				//变量定义
				const BYTE cbCount=12;
				BYTE cbHandCardData[cbCount];
				ZeroMemory(cbHandCardData,sizeof(cbHandCardData));

				//保存扑克
				BYTE cbTempHandCardData[MAX_COUNT];
				CopyMemory(cbTempHandCardData,m_cbHandCardData[wChairID],sizeof(cbTempHandCardData));

				//组合索引
				BYTE cbIndexHead[cbCount]={0,1,0,2,0,3,1,2,1,3,2,3};
				BYTE cbIndexTail[cbCount]={2,3,1,3,1,2,0,3,0,2,0,1};

				//设置扑克
				for (int i=0; i<cbCount; i++)
				{
					cbHandCardData[i]=cbTempHandCardData[cbIndexHead[i]];
				}				

				//比较扑克
				BYTE cbMaxIndex=0;
				int i = 0;
				for (i=cbMaxIndex+2; i<CountArray(cbHandCardData); i+=2)
				{
					BYTE cbValue=m_GameLogic.CompareCard(&cbHandCardData[i],&cbHandCardData[cbMaxIndex],2);
					if (cbValue==TRUE) cbMaxIndex=i;
				}			

				//设置扑克
				m_cbHandCardData[wChairID][0]=cbTempHandCardData[cbIndexTail[cbMaxIndex]];
				m_cbHandCardData[wChairID][1]=cbTempHandCardData[cbIndexTail[cbMaxIndex+1]];
				m_cbHandCardData[wChairID][2]=cbTempHandCardData[cbIndexHead[cbMaxIndex]];
				m_cbHandCardData[wChairID][3]=cbTempHandCardData[cbIndexHead[cbMaxIndex+1]];
				
				//设置状态
				m_cbOpenFinish[wChairID]=TRUE;

				//构造变量
				CMD_S_Open_Card OpenCard;
				ZeroMemory(&OpenCard,sizeof(OpenCard));
				OpenCard.wChairID=i;

				////发送数据
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

//游戏消息处理 
bool  __cdecl CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_USER_INVEST:			//用户下本
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_UserInvest));
			if (wDataSize!=sizeof(CMD_C_UserInvest)) return false;

			//变量定义
			CMD_C_UserInvest * pUserInvest=(CMD_C_UserInvest *)pDataBuffer;

			//用户效验
			//tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();

			tagServerUserInfo * pUserData=pIServerUserItem->GetUserInfo();

			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(IsUserPlaying(pUserData->wChairID));
			if (!IsUserPlaying(pUserData->wChairID)) return false;

			//消息处理
			return OnUserInvest(pUserData->wChairID,pUserInvest->lInvestGold);
		}
	case SUB_C_GIVE_UP:		//用户放弃
		{
			//状态效验
			ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_SCORE);
			if (m_pITableFrame->GetGameStatus()!=GS_TK_SCORE) return true;

			//用户效验
			tagServerUserInfo * pUserData=pIServerUserItem->GetUserInfo();

			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT (m_cbPlayerStatus[pUserData->wChairID]==GUS_PLAYING);
			if (m_cbPlayerStatus[pUserData->wChairID]!=GUS_PLAYING) return false;

			//消息处理
			return OnUserGiveUp(pUserData->wChairID);
		}
	case SUB_C_ADD_SCORE:			//用户加注
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_AddScore));
			if (wDataSize!=sizeof(CMD_C_AddScore)) return false;

			//变量定义
			CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pDataBuffer;

			//用户效验
			tagServerUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(IsUserPlaying(pUserData->wChairID));
			if (!IsUserPlaying(pUserData->wChairID)) return false;

			//消息处理
			return OnUserAddScore(pUserData->wChairID,pAddScore->lScore,pAddScore->cbCode);
		}
	case SUB_C_OPEN_CARD:			//用户摊牌
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_OpenCard));
			if (wDataSize!=sizeof(CMD_C_OpenCard)) return false;

			//变量定义
			CMD_C_OpenCard * pOpenCard=(CMD_C_OpenCard *)pDataBuffer;

			//用户效验
			tagServerUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT (m_cbPlayerStatus[pUserData->wChairID]==GUS_PLAYING);
			if (m_cbPlayerStatus[pUserData->wChairID]!=GUS_PLAYING) return false;

			//消息处理
			return OnUserOpenCard(pUserData->wChairID,pOpenCard->cbCardData);
		}
		case SUB_C_SEND_CARD:		//配牌
			{
				//效验数据
				ASSERT(wDataSize==sizeof(CMD_C_SendCard));
				if (wDataSize!=sizeof(CMD_C_SendCard)) return false;

				ASSERT(pIServerUserItem!=NULL);
				if(pIServerUserItem==NULL) return false;

				//变量定义
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

//框架消息处理
bool  __cdecl CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

//用户断线
bool __cdecl CTableFrameSink::OnActionUserOffLine(IServerUserItem * pIServerUserItem)
{
	//游戏状态
	BYTE cbGameStatus=m_pITableFrame->GetGameStatus();

	//下本状态
	/*if (cbGameStatus==GS_TK_INVEST)
	{
		OnUserGiveUp(pIServerUserItem->GetChairID());
	}*/		

	return true;
}

//用户重连
bool  __cdecl CTableFrameSink::OnActionUserConnect( IServerUserItem * pIServerUserItem)
{	
	m_bUserReConnect[pIServerUserItem->GetChairID()]=true;

	return true;
}

//用户坐下
bool  __cdecl CTableFrameSink::OnActionUserSitDown( IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if (m_pITableFrame->GetGameStatus()!=GS_TK_FREE && !bLookonUser)
	{
		m_cbPlayerStatus[pIServerUserItem->GetChairID()]=GUS_DYNAMIC;
	}
	WORD wChairID = pIServerUserItem->GetChairID();

	//更新分数
	if (!bLookonUser) UpdateUserScore(pIServerUserItem->GetChairID());

	return true;
}

//用户起来
bool  __cdecl CTableFrameSink::OnActionUserStandUp(IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	if(!bLookonUser)
	{
		//清理金币
		if (m_lUserScore[pIServerUserItem->GetChairID()]!=0) 
		{
			//非约战房间
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

//用户同意
bool  CTableFrameSink::OnActionUserReady(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)
{
	if (m_bGameRule) return true;
	
	if (pIServerUserItem->GetChairID()==m_wSetRuleUser)
	{
		//特殊处理
		///////////////////////////////////////////////////////
		
		//手机用户
		if (pIServerUserItem->IsMobileUser()==true)
		{
			m_bGameRule=true;
			return true;
		}

		///////////////////////////////////////////////////////

		//效验大小
		ASSERT (wDataSize==sizeof(CMD_C_BankerBet));
		if (wDataSize!=sizeof(CMD_C_BankerBet)) 
		{
			return false;
		}

		//消息处理
		CMD_C_BankerBet * pBankerBet=(CMD_C_BankerBet *)pData;

		if (pBankerBet->lBaseScore>0)
		{
			m_lCellScore=pBankerBet->lBaseScore;

			m_bGameRule=true;

			//设置变量
			CMD_S_UserAgreeScore UserAgreeScore;
			ZeroMemory(&UserAgreeScore,sizeof(UserAgreeScore));
			UserAgreeScore.wSetRuleUser=m_wSetRuleUser;
			UserAgreeScore.lBaseScore=m_lCellScore;

			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_USER_AGREE_SCORE,&UserAgreeScore,sizeof(UserAgreeScore));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_AGREE_SCORE,&UserAgreeScore,sizeof(UserAgreeScore));

			//通知变更
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

//开始人数
WORD CTableFrameSink::GetStartPlayerCount()
{

	//变量定义
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
		//用户状态
		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			//获取用户
			IServerUserItem * pIServerUser = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUser == NULL) continue;
			if (GetUserScore(i) < m_lCellScore * MIN_MUTIPLIE) continue;

			//人数累加
			wPlayerCount++;
		}

		return __max(wPlayerCount, 2);
	}
	return __max(wPlayerCount, 2);
}

//解散时间
DWORD CTableFrameSink::GetBattleDismissTime()
{
	return 1800;
	return TIME_DISMISS_TABLE;
}

//踢人时间
DWORD CTableFrameSink::GetUnReadyKickOutTime()
{
	if ((m_pGameServiceOption->wServerType& GAME_GENRE_BATTLE)!=0)
	{
		return 1800;
	}

	return TIME_KICK_OUT_2;
}

//解散时间
DWORD CTableFrameSink::GetAgreementDismissTime()
{
	return 30;
}

//允许准备
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

//底分变更
VOID CTableFrameSink::OnEventBaseScoreVariation(LONG lBaseScore)
{
	//设置变量
	m_lCellScore=lBaseScore;
}

//分数变更
VOID CTableFrameSink::OnEventScoreVariation(IServerUserItem *pIServerUserItem)
{
	//更新分数
	if (pIServerUserItem->GetUserStatus()!=US_LOOKON)
	{
		UpdateUserScore(pIServerUserItem->GetChairID());
	}
}

//同意人数
WORD CTableFrameSink::GetDismissAgreeCount()
{
	//变量定义
	WORD wPlayerCount=0;

	if (m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE)
	{
		//用户状态
		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			//获取用户
			IServerUserItem * pIServerUser = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUser != NULL) wPlayerCount++;
		}

		return min(3, wPlayerCount);
	}
	else
	{
		//用户状态
		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			//获取用户
			IServerUserItem * pIServerUser = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUser != NULL && m_cbPlayerStatus[i] == GUS_PLAYING) wPlayerCount++;
		}
	}
	return wPlayerCount;
}

//桌子解散
VOID CTableFrameSink::OnEventTableDismiss()
{
	//清理数据
	ZeroMemory(m_lUserScore,sizeof(m_lUserScore));
	ZeroMemory(m_lUserFreeScore,sizeof(m_lUserFreeScore));
}

//桌子修改
VOID CTableFrameSink::OnEventTableModify(LPVOID pModifyParam, WORD wBufferSize)
{
	tagBattleCreateParam * pParam = (tagBattleCreateParam *)pModifyParam;

	CopyMemory(&m_BattleConfig, pParam, wBufferSize);

	BYTE cbIndex = 0;

	//先后手
	bool bAllowBankerFirst = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;

	//特牌
	bool bAllowSpecialCard = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? false : true;

	//地九王算大牌
	bool bAllowDJWBigCard = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;

	//滚滚翻
	bool bAllowBetGGF = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;


	SCORE lMaxBoboMutiple[3] = { 300,500,1000 };

	BYTE cbSelectIndex = pParam->BattleConfig.cbGameOption[cbIndex++];
	if (cbSelectIndex < 3)
	{
		//携带筹码
		m_CustomConfig.lMaxBoboMultiple = lMaxBoboMutiple[cbSelectIndex];
		//初始分数
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			m_lUserScore[i] = 0;
			m_lUserFreeScore[i] = m_CustomConfig.lMaxBoboMultiple * m_lCellScore;
		}
	}

	WORD wMinBetValue = 0;

	CopyMemory(&wMinBetValue, &pParam->BattleConfig.cbGameOption[cbIndex], sizeof(BYTE) * 2);

	//携带筹码
	m_CustomConfig.lMinBetAmount = static_cast<SCORE>(wMinBetValue)* m_lCellScore;


	//先后手
	if (bAllowBankerFirst == true)
	{
		m_CustomConfig.cbRuleOption |= GR_ALLOW_FIRSTER;
	}
	else
	{
		m_CustomConfig.cbRuleOption &= ~GR_ALLOW_FIRSTER;
	}

	//特牌
	if (bAllowSpecialCard == true)
	{
		m_CustomConfig.cbRuleOption |= GR_SHS_AND_SHL;
	}
	else
	{
		m_CustomConfig.cbRuleOption &= ~GR_SHS_AND_SHL;
	}

	//地九王算大牌
	if (bAllowDJWBigCard == true)
	{
		m_CustomConfig.cbRuleOption |= GR_DJW_BIG_CARD;
	}
	else
	{
		m_CustomConfig.cbRuleOption &= ~GR_DJW_BIG_CARD;
	}

	//下注滚滚翻
	if (bAllowBetGGF == true)
	{
		m_CustomConfig.cbRuleOption |= GR_ALLOW_BET_GGF;
	}
	else
	{
		m_CustomConfig.cbRuleOption &= ~GR_ALLOW_BET_GGF;
	}

	//设置规则
	m_GameLogic.SetRuleOption(m_CustomConfig.cbRuleOption);


	//坐下倍数
	m_pITableFrame->SetSitdownGameMultiple(MIN_MUTIPLIE);

	//起立倍数
	m_pITableFrame->SetStandupGameMultiple(MIN_MUTIPLIE);

	m_pITableFrame->SetMaxPlayerCount(m_BattleConfig.BattleConfig.wPlayerCount);

	return;
}

//桌子创建
VOID CTableFrameSink::OnEventTableCreated(LPVOID pCreateConfig, WORD wBufferSize)
{
	tagBattleCreateParam * pParam = (tagBattleCreateParam *)pCreateConfig;

	CopyMemory(&m_BattleConfig, pParam, wBufferSize);

	BYTE cbIndex = 0;

	//先后手
	bool bAllowBankerFirst = pParam->BattleConfig.cbGameOption[cbIndex++] ==0? true : false;

	//特牌
	bool bAllowSpecialCard = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? false : true;

	//地九王算大牌
	bool bAllowDJWBigCard = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;

	//滚滚翻
	bool bAllowBetGGF = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;


	SCORE lMaxBoboMutiple[3] = { 300,500,1000 };

	BYTE cbSelectIndex = pParam->BattleConfig.cbGameOption[cbIndex++];
	if (cbSelectIndex < 3)
	{
		//携带筹码
		m_CustomConfig.lMaxBoboMultiple = lMaxBoboMutiple[cbSelectIndex];

		//初始分数
		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			m_lUserScore[i] = 0;
			m_lUserFreeScore[i] = m_CustomConfig.lMaxBoboMultiple * m_lCellScore;
		}
	}

	SCORE lMinBetValue = 0;

	CopyMemory(&lMinBetValue, &pParam->BattleConfig.cbGameOption[cbIndex], sizeof(BYTE) * 4);
	
	//携带筹码
	m_CustomConfig.lMinBetAmount = static_cast<SCORE>(lMinBetValue)* m_lCellScore;
	

	//先后手
	if (bAllowBankerFirst == true)
	{
		m_CustomConfig.cbRuleOption |= GR_ALLOW_FIRSTER;
	}
	else
	{
		m_CustomConfig.cbRuleOption &= ~GR_ALLOW_FIRSTER;
	}

	//特牌
	if (bAllowSpecialCard == true)
	{
		m_CustomConfig.cbRuleOption |= GR_SHS_AND_SHL;
	}
	else
	{
		m_CustomConfig.cbRuleOption &= ~GR_SHS_AND_SHL;
	}

	//地九王算大牌
	if (bAllowDJWBigCard == true)
	{
		m_CustomConfig.cbRuleOption |= GR_DJW_BIG_CARD;
	}
	else
	{
		m_CustomConfig.cbRuleOption &= ~GR_DJW_BIG_CARD;
	}

	//下注滚滚翻
	if (bAllowBetGGF == true)
	{
		m_CustomConfig.cbRuleOption |= GR_ALLOW_BET_GGF;
	}
	else
	{
		m_CustomConfig.cbRuleOption &= ~GR_ALLOW_BET_GGF;
	}

	//设置规则
	m_GameLogic.SetRuleOption(m_CustomConfig.cbRuleOption);

	m_wBattleCount = 0;

	//坐下倍数
	m_pITableFrame->SetSitdownGameMultiple(MIN_MUTIPLIE);

	//起立倍数
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

//加注信息
void CTableFrameSink::SendUserAddInfo(WORD wAddUser,LONGLONG lScore,WORD wNextUser,LONGLONG lMinScore,LONGLONG lMaxScore)
{
	//构造数据
	CMD_S_AddScore AddScore;
	ZeroMemory(&AddScore,sizeof(AddScore));
	AddScore.lAddScoreCount=lScore;
	AddScore.wAddScoreUser=wAddUser;
	AddScore.wCurrentUser=wNextUser;
	AddScore.lTurnMinScore=lMinScore;
	AddScore.lTurnMaxScore=lMaxScore;
	AddScore.lMinAddScore=GetMinAddScore(wNextUser);	

	//发送数据
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));

	return;
}

//发牌信息
void CTableFrameSink::SendSendCardInfo(BYTE cbCardCount,WORD wCurrentUser)
{
	ASSERT(cbCardCount>=2 && cbCardCount<=MAX_COUNT);

	//构造数据
	CMD_S_SendCard SendCard;
	ZeroMemory(&SendCard,sizeof(SendCard));
	SendCard.cbCardCount=cbCardCount;
	SendCard.wCurrentUser=wCurrentUser;
	SendCard.lMinAddScore=GetMinAddScore(wCurrentUser);	

	//明牌数据
	BYTE cbMingCount=cbCardCount-2;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//判断状态
		if (m_cbPlayerStatus[i]!=GUS_PLAYING) continue;

		//设置数量
		m_cbHandCardCount[i]=cbCardCount;

		//用户扑克
		if (cbMingCount>0)
		{
			CopyMemory(SendCard.cbMingCard[i],&m_cbHandCardData[i][2],cbMingCount);
		}
	}

	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_pITableFrame->GetTableUserItem(i)==NULL)continue;		

		//用户扑克
		CopyMemory(SendCard.cbHandCard,m_cbHandCardData[i],sizeof(SendCard.cbHandCard));
		m_pITableFrame->SendTableData(i,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

		//发送数据
		m_pITableFrame->SendLookonData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
	}

	//设置变量
	m_wFirstUser=wCurrentUser;

	////发送数据
	//m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));

	return;
}

//下本事件
bool CTableFrameSink::OnUserInvest(WORD wChairID, LONGLONG lInvestCount)
{
	//状态效验
	ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_INVEST);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_INVEST) return false;

	//用户积分
	IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if(pIServerUserItem==NULL)return false;

	//获取分数
	LONGLONG lUserScore=GetUserScore(wChairID);	

	if (m_cbPlayerStatus[wChairID]==GUS_PLAYING)
	{
		//效验信息
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

		//设置变量
		m_bInvestFinish[wChairID]=true;
		m_lUserScore[wChairID]+=lInvestCount;

		//构造数据
		CMD_S_UserInvest UserInvest;
		ZeroMemory(&UserInvest,sizeof(UserInvest));
		UserInvest.wChairID=wChairID;
		UserInvest.lScore=m_lUserScore[wChairID];

		////发送数据
		//for (WORD i=0;i<m_wPlayerCount;i++)
		//{
		//	if (m_pITableFrame->GetTableUserItem(i)==NULL)continue;
		//	m_pITableFrame->SendTableData(i,SUB_S_USER_INVEST,&UserInvest,sizeof(UserInvest));
		//}
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_USER_INVEST, &UserInvest, sizeof(UserInvest));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_USER_INVEST,&UserInvest,sizeof(UserInvest));
	}

	//关闭定时器
	KillUserGameTimer(wChairID);

	//下本人数
	WORD wInvestFinishCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbPlayerStatus[i]==GUS_PLAYING && m_bInvestFinish[i]) wInvestFinishCount++;
		else if(m_cbPlayerStatus[i]!=GUS_PLAYING) wInvestFinishCount++;
	}

	//下个环节
	if (wInvestFinishCount==m_wPlayerCount)
	{
		//下低注数
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayerStatus[i]!=GUS_PLAYING)continue;
			m_lTotalScore[i] = m_lCellScore;
		}

		//配牌
		if(m_bAllotCard)
		{
			BYTE cbControlIndex=0;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayerStatus[i]!=GUS_PLAYING)continue;
				CopyMemory(m_cbHandCardData[i],m_cbControlCardData[cbControlIndex++],MAX_COUNT);
			}
		}
		else	//随机扑克
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

		//判断梭哈
		WORD wPlayCount = 0,wShowCount = 0;
		for (BYTE i=0;i<m_wPlayerCount;i++)
		{
			if (m_cbPlayerStatus[i]!=GUS_PLAYING)continue;

			if (m_lTotalScore[i]==m_lUserScore[i]) wShowCount++;
			wPlayCount++;
		}

		//设置状态
		BYTE cbStatus = ((wPlayCount>wShowCount+1)?GS_TK_SCORE:GS_TK_OPEN_CARD);
		m_pITableFrame->SetGameStatus(cbStatus);

		//下注状态
		if (wPlayCount>wShowCount+1)
		{
			//梭哈用户
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if (m_cbPlayerStatus[i]!=GUS_PLAYING)continue;

				if (m_lTotalScore[i]==m_lUserScore[i])m_cbShowHand[i]=TRUE;
			}

			//查找下位
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				m_wCurrentUser=(m_wBankerUser+i)%m_wPlayerCount;
				if (m_cbPlayerStatus[m_wCurrentUser]!=GUS_PLAYING)continue;

				if (m_lTotalScore[m_wCurrentUser]<m_lUserScore[m_wCurrentUser])break;
			}

			//设置变量
			m_lMinAddScore=m_lCellScore;			

			//发送扑克
			SendSendCardInfo(2,m_wCurrentUser);

			//超时定时器
			SetUserGameTimer(m_wCurrentUser,25,m_dwDelayTime);
		}
		//开牌状态
		else 
		{
			SendSendCardInfo(MAX_COUNT,INVALID_CHAIR);

			//发送数据
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPEN_START);
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_START);

			m_pITableFrame->SetGameTimer(IDI_OPEN_CARD, (TIME_OPEN_CARD+ m_dwDelayTime)*1000,1,0);
		}
	}

	return true;
}

//放弃事件
bool CTableFrameSink::OnUserGiveUp(WORD wChairID)
{
	//关闭定时器
	KillUserGameTimer(wChairID);

	//结束游戏
	OnEventGameConclude(wChairID+GAME_PLAYER,m_pITableFrame->GetTableUserItem(wChairID),GER_USER_LEAVE);

	return true;
}

//加注事件
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore, BYTE cbCode, bool bGiveUp)
{
	//状态效验
	ASSERT (m_pITableFrame->GetGameStatus()==GS_TK_SCORE);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_SCORE) return false;

	//效验用户
	ASSERT (m_wCurrentUser==wChairID);
	if (m_wCurrentUser!=wChairID) return false;

	//关闭定时器
	KillUserGameTimer(wChairID);

	//用户操作
	if (!bGiveUp)
	{
		//下注校验
		if (cbCode == CODE_PASS && lScore != 0)
		{
			return false;
		}
		if (cbCode == CODE_FOLLOW && lScore != m_lTurnMinScore)
		{
			return false;
		}
		//梭哈校验
		if (cbCode==CODE_SHOWHAND)
		{
			LONGLONG lMaxScore = m_lUserScore[wChairID]-m_lTotalScore[wChairID];
			if (lScore != lMaxScore)
			{
				return false;
			}
		}
		
		//加注校验
		if (cbCode==CODE_ADD)
		{
			//下注范围
			if (lScore<(m_lMinAddScore + m_lTurnMinScore) || lScore>(m_lUserScore[wChairID] - m_lTotalScore[wChairID]))
			{
				return false;
			}
		}

		//累计金币
		m_lTableScore[wChairID] += lScore;
		m_lTotalScore[wChairID] += lScore;

		//平衡下注
		if (m_lTableScore[wChairID] > m_lBalanceScore)
		{
			m_lBalanceScore = m_lTableScore[wChairID];
		}

		//梭哈判断
		if (m_lTotalScore[wChairID]==m_lUserScore[wChairID]) m_cbShowHand[wChairID] = TRUE;
	}

	//用户切换
	WORD wNextPlayer=INVALID_CHAIR;
	for (WORD i=1;i<m_wPlayerCount;i++)
	{
		//设置变量
		m_wOperaCount++;
		wNextPlayer=(m_wCurrentUser+i)%m_wPlayerCount;

		//继续判断
		if ((m_cbPlayerStatus[wNextPlayer]==GUS_PLAYING)&&(m_cbShowHand[wNextPlayer]==FALSE))break;
	}

	//判断等注
	bool bFinishTurn=false;
	if (m_wOperaCount>=m_wPlayerCount)
	{
		bool bValid = false;
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//判断等注
			if ((m_cbPlayerStatus[i] == GUS_PLAYING) && (m_lTableScore[i] < m_lBalanceScore) && (m_cbShowHand[i] == FALSE))
			{
				bValid = true;
				break;
			}
		}

		//当轮结束
		//if (i==m_wPlayerCount) bFinishTurn=true;
		if (!bValid) bFinishTurn = true;
		
	}

	//判断梭哈
	WORD wPlayCount = 0,wShowCount = 0;
	for (BYTE i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayerStatus[i]!=GUS_PLAYING) continue;

		if (m_cbShowHand[i]==TRUE) wShowCount++;
		wPlayCount++;
	}

	//只剩一玩家没梭(因为其下注大于所有玩家)或者全梭
	if(!bFinishTurn && wPlayCount<=wShowCount+1 && m_lTableScore[wNextPlayer]>=m_lBalanceScore) bFinishTurn=true;

	//不下判断
	if (bFinishTurn && wShowCount==0)
	{
		//让牌人数
		WORD wNoAddCount=0;
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if (m_cbPlayerStatus[i]==GUS_PLAYING && m_lTableScore[i]==0)wNoAddCount++;
		}

		//让牌结束
		if (wPlayCount==wNoAddCount)
		{
			//构造数据
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			GameEnd.cbEndReason=END_REASON_PASS;
			CopyMemory(GameEnd.cbCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

			//放弃玩家
			for(WORD i=0;i<GAME_PLAYER;i++)
			{
				if (m_lLoseScore[i]<0 || m_cbPlayerStatus[i]==GUS_PLAYING) GameEnd.lGameScore[i]=0;
			}

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	}

	//继续加注
	if (!bFinishTurn)
	{
		//当前用户
		m_wCurrentUser=wNextPlayer;

		//设置变量		
		m_lTurnMinScore = min(m_lBalanceScore-m_lTableScore[m_wCurrentUser],m_lUserScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser]);
		m_lTurnMaxScore = m_lUserScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser];

		//最小加注
		if (m_CustomConfig.cbRuleOption&GR_ALLOW_BET_GGF)
		{
			//变量定义
			WORD wLastChairID=m_wFirstUser;
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//计算位置
				WORD wTempChairID=(wChairID-i+m_wPlayerCount)%m_wPlayerCount;
				if (m_wFirstUser==wTempChairID) break;
 				if (m_cbPlayerStatus[wTempChairID]!=GUS_PLAYING) continue;					

				//比较下注
				if (m_lTotalScore[wTempChairID]>m_lTotalScore[wLastChairID])
				{
					wLastChairID=wTempChairID;
				}

				break;
			}
 
			//计算加注			
			m_lMinAddScore = m_lTotalScore[wLastChairID]*2-m_lTotalScore[m_wCurrentUser]-m_lTurnMinScore;
		}

		//发送信息
		SendUserAddInfo(wChairID,lScore,m_wCurrentUser,m_lTurnMinScore,m_lTurnMaxScore);

		//超时定时器
		SetUserGameTimer(m_wCurrentUser,25,m_dwDelayTime);

		return true;
	}

	//清理数据
	m_wOperaCount=0;	
	m_lBalanceScore = 0L;
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));

	//设置变量
	m_cbBalanceCount++;

	//判断开牌
	bool bOpenStatus = (m_cbBalanceCount!=3)?false:true;

	//发送信息
	if (bOpenStatus) SendUserAddInfo(wChairID,lScore,INVALID_CHAIR,0,0);

	//继续发牌
	if (!bOpenStatus)
	{
		//梭哈判断
		BYTE cbSendCount=0;
		if (wPlayCount>0 && wShowCount>=wPlayCount-1)
		{
			//设置变量
			bOpenStatus = true;
			cbSendCount = MAX_COUNT;
		}
		else cbSendCount = m_cbBalanceCount+2;

		//当前用户
		if (!bOpenStatus)
		{
			//牌面大小
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

			//比较大小
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

			//当前用户
			m_wCurrentUser = wHeadMaxUser;

			//小大下注
			m_lTurnMinScore = 0;			
			m_lTurnMaxScore = m_lUserScore[m_wCurrentUser]-m_lTotalScore[m_wCurrentUser];

			//最小加注
			if (m_CustomConfig.cbRuleOption&GR_ALLOW_BET_GGF)
			{
				m_lMinAddScore = m_lTotalScore[m_wCurrentUser];
			}

			//发送信息
			SendUserAddInfo(wChairID,lScore,INVALID_CHAIR,m_lTurnMinScore,m_lTurnMaxScore);
		}
		//梭哈结束
		else 
		{
			SendUserAddInfo(wChairID,lScore,INVALID_CHAIR,0,0);
		}

		//发送信息
		SendSendCardInfo(cbSendCount,((!bOpenStatus)?m_wCurrentUser:INVALID_CHAIR));

		//超时定时器
		SetUserGameTimer(m_wCurrentUser,25,m_dwDelayTime);
	}

	//用户开牌
	if (bOpenStatus)
	{	
		//设置状态
		m_pITableFrame->SetGameStatus(GS_TK_OPEN_CARD);

		//发送数据
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OPEN_START);
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_START);

		m_pITableFrame->SetGameTimer(IDI_OPEN_CARD, (TIME_OPEN_CARD + m_dwDelayTime) *1000,1,0);
	
	}

	return true;
}

//开牌事件
bool CTableFrameSink::OnUserOpenCard(WORD wChairID, BYTE cbCardData[])
{
	//状态效验
	ASSERT (m_pITableFrame->GetGameStatus()==GS_TK_OPEN_CARD);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_OPEN_CARD) return false;

	//关闭定时器
	KillUserGameTimer(wChairID);

	//开牌数据
	if (cbCardData!=NULL)
	{
		//效验数据
		BYTE cbTempData[MAX_COUNT];
		CopyMemory(cbTempData,cbCardData,sizeof(cbTempData));

		BYTE cbTempHandCardData[MAX_COUNT];
		CopyMemory(cbTempHandCardData,m_cbHandCardData[wChairID],sizeof(cbTempHandCardData));


		bool bRemove = m_GameLogic.RemoveCard(m_cbHandCardData[wChairID],MAX_COUNT,cbTempData,MAX_COUNT);
		ASSERT(bRemove);
		if(!bRemove)
		{
			CString strInfo=TEXT("用户开牌数据异常：");

			strInfo+=TEXT("\r\n发送数据为：");
			for(int i=0;i<MAX_COUNT;i++)
			{
				CString strTempInfo;
				strTempInfo.Format(TEXT(" %2x "),cbTempData[i]);
				strInfo+=strTempInfo;
			}
			strInfo+=TEXT("\r\n实际数据为：");
			for(int i=0;i<MAX_COUNT;i++)
			{
				CString strTempInfo;
				strTempInfo.Format(TEXT(" %2x "),cbTempHandCardData[i]);
				strInfo+=strTempInfo;
			}
			NcaTextOut(strInfo);

			//比较大小
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

		//头尾大小
		BYTE cbSpeCount = 2;
		BYTE cbValue=m_GameLogic.CompareCard(cbCardData,&cbCardData[cbSpeCount],cbSpeCount);
		ASSERT(cbValue!=TRUE);
		if(cbValue==TRUE)
		{
			return false;
		}

		//特殊处理
		m_GameLogic.ChangeSpeCard(cbCardData,MAX_COUNT);//!

		//用户扑克
		CopyMemory(m_cbHandCardData[wChairID],cbCardData,sizeof(BYTE)*MAX_COUNT);
		m_cbOpenFinish[wChairID] = TRUE;

		//构造变量
		CMD_S_Open_Card OpenCard;
		ZeroMemory(&OpenCard,sizeof(OpenCard));
		OpenCard.wChairID=wChairID;

		////发送数据
		//for (WORD i=0;i<m_wPlayerCount;i++)
		//{
		//	if(m_pITableFrame->GetTableUserItem(i)==NULL)continue;
		//	m_pITableFrame->SendTableData(i,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));
		//}
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPEN_CARD, &OpenCard, sizeof(OpenCard));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));	
	}

	//开牌人数
	BYTE bUserCount=0;
	for(WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbOpenFinish[i]==TRUE && m_cbPlayerStatus[i]==GUS_PLAYING)bUserCount++;
		else if(m_cbPlayerStatus[i]!=GUS_PLAYING) bUserCount++;
	}

	//结束游戏
	if (bUserCount == m_wPlayerCount)
	{
		KillUserGameTimer(INVALID_CHAIR);
		m_pITableFrame->KillGameTimer(IDI_OPEN_CARD);
		return OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);
	}

	return true;
}

//提示信息
void CTableFrameSink::PopBrowseAction(IServerUserItem *pIUserItem, LPCWSTR szTips, LPCWSTR szUrl,WORD wType)
{
	////变量定义
	//BYTE cbBuffer[SOCKET_PACKET] = {0};
	////构造发送数据
	//WORD wSendSize = 0;
	//CMD_CM_ActionMessage *pActionMessage = (CMD_CM_ActionMessage *)cbBuffer;
	//pActionMessage->nButtonType = MB_OKCANCEL;
	//pActionMessage->wType = wType;
	//lstrcpynW( pActionMessage->szString,szTips,CountArray(pActionMessage->szString) );
	//pActionMessage->wLength = lstrlenW(pActionMessage->szString)+1;

	//wSendSize = sizeof(CMD_CM_ActionMessage)-sizeof(pActionMessage->szString)+CountStringBufferW(pActionMessage->szString);

	////构造动作消息
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

	////发送动作消息
	//m_pITableFrame->SendUserData(pIUserItem,SUB_CM_ACTION_MESSAGE,cbBuffer,wSendSize);
}

//获取加注
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

//获取分数
LONGLONG CTableFrameSink::GetUserScore(WORD wChairID)
{
	//查询用户
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);
	if (pIServerUserItem==NULL) return 0L;

	//获取分数
	LONGLONG lUserScore=pIServerUserItem->GetUserFreeScore();
	LONGLONG lMaxBoboCount=m_CustomConfig.lMaxBoboMultiple*m_lCellScore;

	//约战房间
	if ((m_pGameServiceOption->wServerType& GAME_GENRE_BATTLE)!=0
		&& pIServerUserItem->GetScoreKind()==SCORE_KIND_BATTLE)
	{
		lUserScore=m_lUserFreeScore[wChairID];
	}

	//约战房间
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0
		&& pIServerUserItem->GetScoreKind() != SCORE_KIND_BATTLE)
	{
		lUserScore = min(lUserScore, m_lUserFreeScore[wChairID]);
	}


	return lUserScore;
}

//比较扑克
int CTableFrameSink::CompareHandCard(WORD wChairID1,WORD wChairID2,tagCompareResult & CompareResult)
{
	//庄家先手
	BYTE cbAllowFirster=(m_CustomConfig.cbRuleOption&GR_ALLOW_FIRSTER)?TRUE:FALSE;

	//比较大小
	BYTE cbSpeCount=2;
	INT nTailResult=m_GameLogic.CompareCard(m_cbHandCardData[wChairID1],m_cbHandCardData[wChairID2],cbSpeCount);
	INT nHeadResult=m_GameLogic.CompareCard(&m_cbHandCardData[wChairID1][cbSpeCount],&m_cbHandCardData[wChairID2][cbSpeCount],cbSpeCount);
	if (nHeadResult==0xff) nHeadResult=-1;
	if (nTailResult==0xff) nTailResult=-1;

	//头牌相等
	if (nHeadResult==FALSE && cbAllowFirster==TRUE)
	{
		//相等 庄家优先
		if (wChairID1==m_wBankerUser) nHeadResult=TRUE;
		if (wChairID2==m_wBankerUser) nHeadResult=-1;
		if (wChairID1!=m_wBankerUser && wChairID2!=m_wBankerUser)
		{
			//逆时针近庄优先
			WORD wChairDis1=(wChairID1+(wChairID1<m_wBankerUser?GAME_PLAYER:0))-m_wBankerUser;
			WORD wChairDis2=(wChairID2+(wChairID2<m_wBankerUser?GAME_PLAYER:0))-m_wBankerUser;
			
			//设置变量
			nHeadResult=(wChairDis1<wChairDis2)?TRUE:-1;
		}
	}

	//尾牌相等
	if (nTailResult==FALSE && cbAllowFirster==TRUE)
	{
		//相等 庄家优先
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
			//逆时针近庄优先
			WORD wChairDis1=(wChairID1+(wChairID1<m_wBankerUser?GAME_PLAYER:0))-m_wBankerUser;
			WORD wChairDis2=(wChairID2+(wChairID2<m_wBankerUser?GAME_PLAYER:0))-m_wBankerUser;
			
			//设置变量
			nTailResult=(wChairDis1<wChairDis2)?TRUE:-1;
		}
	}

	//特殊情况
	/*if (m_GameLogic.GetCardType(m_cbHandCardData[wChairID1],2)==m_GameLogic.GetCardType(m_cbHandCardData[wChairID2],2))
	{
		if (m_GameLogic.GetCardType(m_cbHandCardData[wChairID1],2)==0)
		{
			nTailResult=FALSE;			
		}
	} */

	//头尾结果
	CompareResult.nHeadResult=nHeadResult;
	CompareResult.nTailResult=nTailResult;

	//最终结果
	if ((nHeadResult+nTailResult)>0) CompareResult.nResult=1;
	if ((nHeadResult+nTailResult)<0) CompareResult.nResult=-1;
	if ((nHeadResult+nTailResult)==0) CompareResult.nResult=0;		

	return CompareResult.nResult;
}

//尝试解散
void CTableFrameSink::TryDismissGame()
{
	//变量定义
	BYTE cbPlayerCount=0;

	//统计人数
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		//用户判断	
		IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;
		if (pIServerUserItem->GetUserInfo()->cbUserStatus == US_LOOKON) continue;


		//比较分数
		if (GetUserScore(i) >= m_lCellScore * MIN_MUTIPLIE)
		{
			cbPlayerCount++;
		}
	}

	//剩余1人解散桌子
	if (cbPlayerCount==1 && m_pIEventBattleSink!=NULL)
	{
		m_pIEventBattleSink->BattleFinish(BFR_NORMAL);
		m_wBattleCount = 0;
	}
}

//更新分数
void  CTableFrameSink::UpdateUserScore(WORD wChairID)
{
	//变量定义
	CMD_S_UpdateScore UpdateScore;
	ZeroMemory(&UpdateScore,sizeof(UpdateScore));

	if (wChairID==INVALID_CHAIR)
	{
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//用户判断			
			if (m_pITableFrame->GetTableUserItem(i)==NULL)
			{
				continue;
			}

			//设置变量
			UpdateScore.wChairID=i;
			UpdateScore.lUserScore=GetUserScore(i);

			//发送消息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_UPDATE_SCORE,&UpdateScore,sizeof(UpdateScore));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UPDATE_SCORE, &UpdateScore, sizeof(UpdateScore));
		}
	}
	else
	{
		//设置变量
		UpdateScore.wChairID=wChairID;
		UpdateScore.lUserScore=GetUserScore(wChairID);

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_UPDATE_SCORE,&UpdateScore,sizeof(UpdateScore));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_UPDATE_SCORE, &UpdateScore, sizeof(UpdateScore));
	}
}

//记录异常
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
				strType.Format(TEXT("写分类型：%d"),nType);
				NcaTextOut(strType);

				//记录负分
				CString strInfo=TEXT("");

				//strInfo.Format(TEXT("[%d]出现负分，写入分数：%I64d，实际分数：%I64d,下注数目：%I64d,底分：%I64d"),pUserItem->GetUserID(),ScoreInfoArray[i].lScore,lUserScore,m_lTableScore[i],10*m_lCellScore);
				strInfo.Format(TEXT("[%d]出现负分，开始时用户分数：%I64d，现在用户分数：%I64d"),pUserItem->GetUserID(),m_lUserBeginScore[i],lUserScore);

				NcaTextOut(strInfo);

				CString strRoomType;
				strRoomType=TEXT("金币房间");

				strInfo.Format(TEXT("写入分数：%I64d,下注数目：%I64d,底分：%I64d,房间类型：%s"),ScoreInfoArray[i].lScore,m_lTableScore[i],MIN_MUTIPLIE*m_lCellScore,strRoomType);
				NcaTextOut(strInfo);

				if(ScoreInfoArray[i].lScore<0&&lUserScore>=0)
				{
					ScoreInfoArray[i].lScore=-lUserScore;
				}

			}
		}
	}
}

//测试写日志
VOID CTableFrameSink::WriteControlLog(string & str)
{
	str += "\n";

	//构造路径
	TCHAR szWorkDir[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szWorkDir, CountArray(szWorkDir));

	//文件目录
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
	//获取文件名
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
			AfxMessageBox(TEXT("写文件异常！"));
		}

		//关闭句柄
		file.Close();
	}
}

//////////////////////////////////////////////////////////////////////////
