#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//辅助时间
#define TIME_DISPATCH				1									//发牌时间
#ifdef DEBUG
//游戏时间
#define TIME_LESS					1									//最少时间
#define TIME_OUT_CARD				6									//出牌时间
#define TIME_START_GAME				6									//开始时间
#define TIME_CALL_SCORE				6									//叫分时间
#else
//游戏时间
#define TIME_LESS					1									//最少时间
#define TIME_OUT_CARD				6									//出牌时间
#define TIME_START_GAME				6									//开始时间
#define TIME_CALL_SCORE				6									//叫分时间

#endif // DEBUG

//游戏时间
#define IDI_INVEST				0										//下本时间
#define IDI_ADD_BET				2										//下注时间
#define IDI_START_GAME			1										//开始时间
#define IDI_OPEN_CARD			3										//开牌时间

//////////////////////////////////////////////////////////////////////////

//构造函数
CRobotUserItemSink::CRobotUserItemSink()
{
	//游戏变量
	m_wBankerUser=INVALID_CHAIR;
	m_lMinAddScore = 0;
	m_lTurnMaxScore = 0;
	m_lUserInvest = 0;
	m_lTurnMinScore = 0;
	m_lTotalAddBet = 0;
	m_lMaxBoboMultiple = 0;
	m_lUserScore = 0;
	m_bGiveUp = false;
	m_lUserInvest = 0;
	ZeroMemory(m_cbUserStatus, sizeof(m_cbUserStatus));

	//手上扑克
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//接口变量
	m_pITestUserItem =NULL;

	return;
}

//析构函数
CRobotUserItemSink::~CRobotUserItemSink()
{
}

//接口查询
VOID * __cdecl CRobotUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IRobotUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IRobotUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//初始接口
bool __cdecl CRobotUserItemSink::InitUserItemSink(IUnknownEx * pIUnknownEx)
{
	//查询接口
	m_pITestUserItem =QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IRobotUserItem);
	if (m_pITestUserItem ==NULL) return false;

	m_pITestUserItem->SetAdroidVersion(VERSION_CLIENT);

	return true;
}

//重置接口
bool __cdecl CRobotUserItemSink::ResetUserItemSink()
{
	//游戏变量
	m_wBankerUser = INVALID_CHAIR;
	m_lMinAddScore = 0;
	m_lUserInvest = 0;
	m_lTurnMaxScore = 0;
	m_lTurnMinScore = 0;
	m_lTotalAddBet = 0;
	//m_lMaxBoboMultiple = 0;
	//m_lUserScore = 0;

	//手上扑克
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount, sizeof(m_cbHandCardCount));

	return true;
}

//时间消息
bool __cdecl CRobotUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_INVEST:	//下本时间
	{
		m_pITestUserItem->KillGameTimer(IDI_INVEST);

		//设置下本
		CMD_C_UserInvest UserInvest;
		ZeroMemory(&UserInvest, sizeof(UserInvest));

		//m_lUserInvest 
		SCORE lUserInvest = m_lCellScore * (MIN_MUTIPLIE + static_cast<SCORE>(1+rand() % (MIN_MUTIPLIE*(1+rand()%MIN_MUTIPLIE))));
		if (lUserInvest > m_pITestUserItem->GetUserScore())
		{
			lUserInvest = m_pITestUserItem->GetUserScore();
		}

		m_lUserInvest = max(lUserInvest, m_lUserInvest);

		UserInvest.lInvestGold = m_lUserInvest;

		//发送数据
		m_pITestUserItem->SendSocketData(SUB_C_USER_INVEST, &UserInvest, sizeof(UserInvest));
		m_lUserInvest += m_lUserScore;
		m_lTotalAddBet += m_lCellScore;

		return true;
	}
	case IDI_START_GAME:	//开始游戏
	{
		m_pITestUserItem->KillGameTimer(IDI_START_GAME);

		//准备判断
		if (m_pITestUserItem->GetUserScore() > m_lCellScore * MIN_MUTIPLIE)
		{

			//开始判断
			m_pITestUserItem->SendUserReady(NULL, 0);
		}
		else
		{
			//分数不够，起立动作
			m_pITestUserItem->SetRobotAction(ROBOT_WAITSTANDUP,true);
		}

		return true;
	}
	case IDI_ADD_BET:	//用户下注
	{

		m_pITestUserItem->KillGameTimer(IDI_ADD_BET);
		BYTE cbOperatorCode[4] = { CODE_PASS ,CODE_FOLLOW ,CODE_SHOWHAND ,CODE_ADD };
		SCORE lAddSCore[4] = { 0,m_lTurnMinScore,m_lUserInvest - m_lTotalAddBet,m_lMinAddScore};

		BYTE bCanBet[4] = { false,false,false,false };
		//未梭哈
		if (m_lTotalAddBet != m_lUserInvest && rand() % 100 < 90)
		{
			//梭哈
			bCanBet[2] = true;

			//第一个下注，可休
			if (m_lTurnMinScore == 0 && m_lTurnMaxScore == 0)
			{
				//20%的概率休
				if (rand() % 100 < 20)
				{
					CMD_C_AddScore AddScore;
					ZeroMemory(&AddScore, sizeof(AddScore));
					AddScore.lScore = 0;
					AddScore.cbCode = CODE_PASS;
					//发送数据
					m_pITestUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

					m_lTotalAddBet += AddScore.lScore;

					return true;
				}
			}

			//可跟
			if (m_lUserInvest > m_lTotalAddBet)
			{
				bCanBet[1] = true;
			}
			//加注
			if (m_lUserInvest - m_lTotalAddBet > m_lMinAddScore && rand()%100<10)
			{
				bCanBet[1] = true;
			}
			else if (m_lUserInvest < m_lMinAddScore + m_lTotalAddBet)
			{
				bCanBet[2] = true;
			}
			BYTE cbIndex = rand() % 4;
			for (int i = 0; i < 4; ++i)
			{
				cbIndex = (cbIndex + 1) % 4;
				if (bCanBet[cbIndex])
				{
					CMD_C_AddScore AddScore;
					ZeroMemory(&AddScore, sizeof(AddScore));
					AddScore.lScore = lAddSCore[cbIndex];
					AddScore.cbCode = cbOperatorCode[cbIndex];
					//发送数据
					m_pITestUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

					m_lTotalAddBet += AddScore.lScore;

					return true;
				}
			}

		}
		

		{
			//放弃
			m_bGiveUp = true;
			m_pITestUserItem->SendSocketData(SUB_C_GIVE_UP, nullptr, 0);
			return true;
		}

		return true;
	}
	case IDI_OPEN_CARD:		//用户摊牌
	{

		m_pITestUserItem->KillGameTimer(IDI_OPEN_CARD);

		//变量定义
		const BYTE cbCount = 12;
		BYTE cbHandCardData[cbCount];
		ZeroMemory(cbHandCardData, sizeof(cbHandCardData));

		WORD wChairID = m_pITestUserItem->GetChairID();

		//保存扑克
		BYTE cbTempHandCardData[MAX_COUNT];
		CopyMemory(cbTempHandCardData, m_cbHandCardData[wChairID], sizeof(cbTempHandCardData));

		//组合索引
		BYTE cbIndexHead[cbCount] = { 0,1,0,2,0,3,1,2,1,3,2,3 };
		BYTE cbIndexTail[cbCount] = { 2,3,1,3,1,2,0,3,0,2,0,1 };

		//设置扑克
		for (int i = 0; i < cbCount; i++)
		{
			cbHandCardData[i] = cbTempHandCardData[cbIndexHead[i]];
		}

		//比较扑克
		BYTE cbMaxIndex = 0;
		int i = 0;
		for (i = cbMaxIndex + 2; i < CountArray(cbHandCardData); i += 2)
		{
			BYTE cbValue = m_GameLogic.CompareCard(&cbHandCardData[i], &cbHandCardData[cbMaxIndex], 2);
			if (cbValue == TRUE) cbMaxIndex = i;
		}

		//设置扑克
		m_cbHandCardData[wChairID][0] = cbTempHandCardData[cbIndexTail[cbMaxIndex]];
		m_cbHandCardData[wChairID][1] = cbTempHandCardData[cbIndexTail[cbMaxIndex + 1]];
		m_cbHandCardData[wChairID][2] = cbTempHandCardData[cbIndexHead[cbMaxIndex]];
		m_cbHandCardData[wChairID][3] = cbTempHandCardData[cbIndexHead[cbMaxIndex + 1]];

		CMD_C_OpenCard OpenCard;
		ZeroMemory(&OpenCard, sizeof(OpenCard));

		CopyMemory(OpenCard.cbCardData, &m_cbHandCardData[wChairID], sizeof(OpenCard.cbCardData));

		m_pITestUserItem->SendSocketData(SUB_C_OPEN_CARD, &OpenCard, sizeof(OpenCard));

		return true;
	}
	}

	return false;
}

//游戏消息
bool __cdecl CRobotUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:	//游戏开始
	{
		return OnSubGameStart(pData, wDataSize);
	}
	case SUB_S_USER_INVEST:	//用户下本
	{
		return OnUserInvest(pData, wDataSize);
	}
	case SUB_S_ADD_SCORE:	//用户加注
	{
		return OnAddScore(pData, wDataSize);
	}
	case SUB_S_SEND_CARD:	//发送手牌
	{
		return OnSubSendCard(pData, wDataSize);
	}
	case SUB_S_GAME_END:	//游戏结束
	{
		m_wBankerUser = INVALID_CHAIR;
		m_lMinAddScore = 0;
		m_lTurnMaxScore = 0;
		m_lUserInvest = 0;
		m_lTurnMinScore = 0;
		m_lTotalAddBet = 0;
		ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
		ZeroMemory(m_cbHandCardCount, sizeof(m_cbHandCardCount));

		CMD_S_GameEnd * pGameEnd = (CMD_S_GameEnd *)pData;

		UINT nElapse = rand() % TIME_START_GAME + TIME_LESS;
		m_pITestUserItem->SetGameTimer(IDI_START_GAME, nElapse + (rand() % TIME_OUT_CARD));
	

		return true;
	}
	case SUB_S_OPEN_START:	//开始摊牌
	{
		if (m_cbHandCardCount[m_pITestUserItem->GetChairID()] > 0)
		{
			//开牌
			UINT nElapse = rand() % TIME_START_GAME + TIME_LESS;
			m_pITestUserItem->SetGameTimer(IDI_OPEN_CARD, nElapse + (rand() % TIME_OUT_CARD));
		}
		return true;
	}
	case SUB_S_GIVE_UP:		//用户放弃
	{
		return true;
	}
	case SUB_S_UPDATE_SCORE:		//更新分数
	{
		CMD_S_UpdateScore * pUpdateScore = (CMD_S_UpdateScore *)pData;
		
		//m_lUserScore = pUpdateScore->lUserScore;
		
		return true;
	}
	case SUB_S_USER_AGREE_SCORE:		//用户放弃
	{
		return true;
	}
	case SUB_S_REQUEST_RULE:		//用户放弃
	{
		return true;
	}
	case SUB_S_OPEN_CARD: //用户开牌
	{
		return true;
	}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//游戏消息
bool __cdecl CRobotUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//场景消息
bool __cdecl CRobotUserItemSink::OnEventSendGameScene(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_TK_FREE:	//空闲状态
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//变量定义
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
			//IServerUserItem * pIServerUserItem= m_pITestUserItem->GetMeUserItem();

			m_lCellScore = pStatusFree->lCellScore;


			//准备判断
			if (m_pITestUserItem->GetUserScore() > m_lCellScore * MIN_MUTIPLIE)
			{
				
				//开始判断
				m_pITestUserItem->SendUserReady(NULL, 0);

			}
			else
			{
				//分数不够，起立动作
				m_pITestUserItem->SetRobotAction(ROBOT_WAITSTANDUP, true);
			}

			return true;
		}
	case GS_TK_INVEST:	//游戏状态
		{
			//效验数据
			if (wDataSize!=sizeof(CMD_S_StatusInvest)) return false;
			CMD_S_StatusInvest * pStatusInvest =(CMD_S_StatusInvest *)pData;

			m_wBankerUser = pStatusInvest->wBankerUser;
			m_lCellScore = pStatusInvest->lCellScore;

			//扑克数据
			WORD wMeChairID= m_pITestUserItem->GetChairID();

			//玩家设置
			if (pStatusInvest->cbPlayStatus[wMeChairID]== GUS_PLAYING && !pStatusInvest->bInvestFinish[wMeChairID])
			{
				UINT nElapse=rand()%TIME_OUT_CARD+TIME_LESS;
				m_pITestUserItem->SetGameTimer(IDI_INVEST,nElapse);
			}

			return true;
		}
	case GS_TK_SCORE: //下注状态
	{
		//效验数据
		if (wDataSize != sizeof(CMD_S_StatusScore)) return false;
		CMD_S_StatusScore * pStatusScore = (CMD_S_StatusScore *)pData;

		m_lCellScore = pStatusScore->lCellScore;
		m_wBankerUser = pStatusScore->wBankerUser;

		m_lMinAddScore = pStatusScore->lMinAddScore;
		m_lTurnMaxScore = pStatusScore->lTurnMaxScore;
		m_lTurnMinScore = pStatusScore->lTurnMinScore;

		return true;
	}
	case GS_TK_OPEN_CARD://开牌状态
	{
		//效验数据
		if (wDataSize != sizeof(CMD_S_StatusOpenCard)) return false;
		CMD_S_StatusOpenCard * pStatusInvest = (CMD_S_StatusOpenCard *)pData;

		m_lCellScore = pStatusInvest->lCellScore;
		m_wBankerUser = pStatusInvest->wBankerUser;

		//扑克数据
		WORD wMeChairID = m_pITestUserItem->GetChairID();

		CopyMemory(&m_cbHandCardData[wMeChairID][0], pStatusInvest->cbHandCard, sizeof(BYTE) * 2);

		for (WORD i = 0; i < GAME_PLAYER; i++)
		{
			if (m_cbUserStatus[i] == GUS_PLAYING)
			{
				CopyMemory(&m_cbHandCardData[i][2], pStatusInvest->cbMingCard[i], sizeof(BYTE) * 2);
				m_cbHandCardCount[i] = 4;
			}
		}

		//玩家设置
		if (pStatusInvest->cbPlayStatus[wMeChairID] == GUS_PLAYING && pStatusInvest->cbOpenFinish[wMeChairID]==0)
		{
			UINT nElapse = rand() % TIME_OUT_CARD + TIME_LESS;
			m_pITestUserItem->SetGameTimer(IDI_OPEN_CARD, nElapse);
		}

		return true;
	}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//用户进入
VOID __cdecl CRobotUserItemSink::OnEventUserEnter(IRobotUserItem * pITestUserItem, bool bLookonUser)
{
	return;
}

//用户离开
VOID __cdecl CRobotUserItemSink::OnEventUserLeave(IRobotUserItem * pITestUserItem, bool bLookonUser)
{
	return;
}

//用户积分
VOID __cdecl CRobotUserItemSink::OnEventUserScore(IRobotUserItem * pITestUserItem, bool bLookonUser)
{
	return;
}

//用户状态
VOID __cdecl CRobotUserItemSink::OnEventUserStatus(IRobotUserItem * pITestUserItem, bool bLookonUser)
{
	return;
}

//用户段位
VOID __cdecl CRobotUserItemSink::OnEventUserSegment(IRobotUserItem * pITestUserItem, bool bLookonUser)
{
	return;
}

//游戏开始
bool CRobotUserItemSink::OnSubGameStart(VOID * pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//变量定义
	CMD_S_GameStart * pGameStart=(CMD_S_GameStart *)pData;

	m_wBankerUser = pGameStart->wBankerUser;

	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		m_cbUserStatus[i] = pGameStart->cbPlayStatus[i];
	}

	m_lUserScore = pGameStart->lUserScore[m_pITestUserItem->GetChairID()];

	m_lMinAddScore = 0;
	m_lTurnMaxScore = 0;
	m_lTurnMinScore = 0;

	UINT nElapse=rand()%TIME_CALL_SCORE+TIME_LESS;
	m_pITestUserItem->SetGameTimer(IDI_INVEST,nElapse+(rand()%TIME_OUT_CARD));

	return true;
}

//用户下本
bool CRobotUserItemSink::OnUserInvest(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_UserInvest));
	if(wDataSize!=sizeof(CMD_S_UserInvest)) return false;
	//变量定义
	CMD_S_UserInvest * pUserInvest =(CMD_S_UserInvest *)pData;

	if (pUserInvest->wChairID == m_pITestUserItem->GetChairID())
	{
		m_lUserInvest = pUserInvest->lScore;
	}

	return true;
}

//用户下注
bool CRobotUserItemSink::OnAddScore(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize==sizeof(CMD_S_AddScore));
	if (wDataSize!=sizeof(CMD_S_AddScore)) return false;

	//变量定义
	CMD_S_AddScore * pAddScore =(CMD_S_AddScore *)pData;

	m_lMinAddScore = pAddScore->lMinAddScore;
	m_lTurnMaxScore = pAddScore->lTurnMaxScore;
	m_lTurnMinScore = pAddScore->lTurnMinScore;
	

	UINT nElapse = rand() % TIME_CALL_SCORE + TIME_LESS;

	if (pAddScore->wCurrentUser == m_pITestUserItem->GetChairID())
	{
		m_pITestUserItem->SetGameTimer(IDI_ADD_BET, nElapse + (rand() % TIME_OUT_CARD));
	}
	//else if (INVALID_CHAIR == pAddScore->wCurrentUser)
	//{
	//	//开牌
	//	m_pITestUserItem->SetGameTimer(IDI_OPEN_CARD, nElapse + (rand() % TIME_OUT_CARD));
	//}

	return true;
}

//游戏结束
bool CRobotUserItemSink::OnSubSendCard(VOID * pData, WORD wDataSize)
{
	//效验数据
	ASSERT(wDataSize == sizeof(CMD_S_SendCard));
	if (wDataSize != sizeof(CMD_S_SendCard)) return false;

	//变量定义
	CMD_S_SendCard * pSendCard = (CMD_S_SendCard *)pData;

	WORD wChairID = m_pITestUserItem->GetChairID();

	CopyMemory(&m_cbHandCardData[wChairID][0], pSendCard->cbHandCard, sizeof(BYTE)*2);

	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (m_cbUserStatus[i] == GUS_PLAYING)
		{
			CopyMemory(&m_cbHandCardData[i][2], pSendCard->cbMingCard[i], sizeof(BYTE) * 2);
			m_cbHandCardCount[i] = pSendCard->cbCardCount;
		}
	}
	
	UINT nElapse = rand() % TIME_CALL_SCORE + TIME_LESS;

	if (m_cbUserStatus[wChairID] != GUS_PLAYING) return true;

	//下注
	if(wChairID== pSendCard->wCurrentUser && m_lTotalAddBet< m_lUserInvest)
	{
		m_pITestUserItem->SetGameTimer(IDI_ADD_BET, nElapse + (rand() % TIME_OUT_CARD));
	}
	else if (pSendCard->wCurrentUser == INVALID_CHAIR)
	{
		//全部梭哈开牌
		m_pITestUserItem->SetGameTimer(IDI_OPEN_CARD, nElapse + (rand() % TIME_OUT_CARD));
	}

	return true;
}


//测试写日志
VOID CRobotUserItemSink::WriteControlLog(string & str)
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
	_sntprintf_s(szServiceDir, CountArray(szServiceDir), TEXT("%s\\CheXuan\\%d_%d.txt"), szWorkDir, TimeNow.GetDay(), m_pITestUserItem->GetTableID());

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
//组件创建函数
DECLARE_CREATE_MODULE(RobotUserItemSink);