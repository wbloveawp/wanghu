#include "StdAfx.h"
#include "TableFrameSink.h"
#include <locale>

//////////////////////////////////////////////////////////////////////////
//
#define LOG_TABLE(...) LOG_DEBUG(L"桌子号:",m_pITableFrame->GetTableID(),##__VA_ARGS__)

CString GetFileDialogPath()
{
	CString strFileDlgPath;
	TCHAR szModuleDirectory[MAX_PATH];	//模块目录
	GetModuleFileName(AfxGetInstanceHandle(), szModuleDirectory, sizeof(szModuleDirectory));
	int nModuleLen = lstrlen(szModuleDirectory);
	int nProcessLen = lstrlen(AfxGetApp()->m_pszExeName) + lstrlen(TEXT(".EXE")) + 1;
	if (nModuleLen <= nProcessLen)
	{
		return TEXT("");
	}
	szModuleDirectory[nModuleLen - nProcessLen] = 0;
	strFileDlgPath = szModuleDirectory;
	return strFileDlgPath;
}
CString FormatScoreString(SCOREEX lUserGameScore[GAME_PLAYER])
{
	CString str(L"[");
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		str.AppendFormat(L"%.2f ", lUserGameScore[i]);
	}
	str.AppendFormat(L"]");
	return str;
}
//////////////////////////////////////////////////////////////////////////
//静态变量

#define IDI_OUT_CARD_0				3									//出牌定时器
#define IDI_OUT_CARD_1				4									//出牌定时器
#define IDI_OUT_CARD_2				5									//出牌定时器
#define IDI_OUT_CARD_3				6									//出牌定时器
#define	IDI_SO_OPERATE				7									//超时代打定时器
#define IDI_GAMEEND					8									//结算定时器

#define TIME_CHANGE_CARD			5
#define TIME_CALL_CARD				5


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

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量

	//组件变量
	m_pITableFrame = NULL;
	m_pGameServiceOption = NULL;
	//m_pGameCustomRule = NULL;

	//游戏规则（房卡配置）

	//随机种子
	srand((int)time(0));

	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
}

//接口查询
void* CTableFrameSink::QueryInterface(const IID& Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
	QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
	return NULL;
}

//初始化
bool  CTableFrameSink::InitTableFrameSink(IUnknownEx* pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx != NULL);
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrameItem);

	if (m_pITableFrame == NULL)
		return false;

	//获取参数
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption != NULL);

	//开始模式
	m_pITableFrame->SetStartMode(StartMode_AllReady);

	//自定规则
	//ASSERT (m_pITableFrame->GetCustomRule() != NULL);
	//m_pGameCustomRule = (tagCustomRule *)m_pITableFrame->GetCustomRule();

	/*m_bHuanSanZhang = m_pGameCustomRule->bHuanSanZhang;
	m_bHuJiaoZhuanYi = m_pGameCustomRule->bHuJiaoZhuanYi;
	m_bZiMoAddTimes = m_pGameCustomRule->bZiMoAddTimes;
	m_bTianDiHu = m_pGameCustomRule->bTianDiHu;
	m_bHaiDiLaoYue = m_pGameCustomRule->bHaiDiLaoYue;
	m_bMenQing = m_pGameCustomRule->bMenQing;
	m_cbTrusteeDelayTime = m_pGameCustomRule->cbTrusteeDelayTime;*/

	//日志
	//CString strDir = GetFileDialogPath() + TEXT("\\") + GAME_NAME + TEXT("日志");
	//CreateDirectory(strDir, NULL);
	//strDir.AppendFormat(TEXT("\\%s(%d)"), GAME_NAME, m_pITableFrame->GetGameServiceOption()->wServerID);
	//EasyLog::CLog::m_log.SetName(strDir.GetBuffer());

	//BYTE i = m_pITableFrame->GetDataBaseMode();
	return true;
}

//复位桌子
void  CTableFrameSink::ResetTableFrameSink()
{
	//游戏变量
	return;
}

//最少积分
SCOREEX CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem* pIServerUserItem)
{
	return 0;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{

	//换三张，前端发牌需要10秒

	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem* pIServerUserItem, BYTE cbReason)
{
	return true;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem* pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	return true;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	return true;
}

//游戏消息处理
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID* pDataBuffer, WORD wDataSize, IServerUserItem* pIServerUserItem)
{
	return true;
}

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID* pDataBuffer, WORD wDataSize, IServerUserItem* pIServerUserItem)
{
	return false;
}

bool CTableFrameSink::OnActionUserOffLine(IServerUserItem* pIServerUserItem)
{

	//金币场和金币房卡可以托管，积分房卡不托管
	/*
	if (((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0))
	{
		BYTE cbGameStatus = m_pITableFrame->GetGameStatus();
		if (cbGameStatus == GS_MJ_CHANGECARD || cbGameStatus == GS_MJ_CALL)
		{
			////设置托管标志
			//pIServerUserItem->SetTrusteeUser(true);
			//m_pITableFrame->SetGameTimer(IDI_OUT_CARD_0 + wChairID, m_cbTrusteeDelayTime * 1000, 1, 0);
		}
		else
		{
			//if (!((wChairID == m_wCurrentUser) || (m_wCurrentUser == INVALID_CHAIR && m_cbUserAction[wChairID] != WIK_NULL)))
			//{
			//	//设置托管标志
			//	pIServerUserItem->SetTrusteeUser(true);
			//}
		}
	}
	*/
	return true;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(IServerUserItem* pIServerUserItem, bool bLookonUser)
{
	//m_vecRecord[wChairID].clear();
	return true;
}

//用户起来
bool CTableFrameSink::OnActionUserStandUp(IServerUserItem* pIServerUserItem, bool bLookonUser)
{
	//庄家设置
	if (bLookonUser == false)
	{
		/*m_bTrustee[wChairID]=false;
		CMD_S_Trustee Trustee;
		Trustee.bTrustee=false;
		Trustee.wChairID = wChairID;
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));*/
	}

	//中途离开写分
	/*BYTE cbUserStatus = pIServerUserItem->GetUserStatus();
	if (m_pITableFrame->GetGameStatus() == GS_MJ_PLAY && m_bWaitWriteScore[wChairID] == true)
	{
		SCOREEX lWinScore = 0;
		for (WORD j=0; j<MAX_CHART_COUNT; j++)
		{
			if (m_tagHistoryChart[wChairID][j].charttype != INVALID_CHARTTYPE)
			{
				lWinScore += m_tagHistoryChart[wChairID][j].lScore;
			}
		}
		m_bWaitWriteScore[wChairID] = false;

		tagScoreInfo ScoreInfo;
		ZeroMemory(&ScoreInfo, sizeof(ScoreInfo));
		ScoreInfo.cbType = (lWinScore > 0 ? SCORE_TYPE_WIN : SCORE_TYPE_LOSE);
		if (lWinScore > 0 && ((m_pGameServiceOption->wServerType&GAME_GENRE_PERSONAL)==0 || m_pITableFrame->GetDataBaseMode()==1))
		{
			ScoreInfo.lRevenue = m_pITableFrame->CalculateRevenue(wChairID, lWinScore);
		}
		ScoreInfo.lScore = lWinScore - ScoreInfo.lRevenue;

		m_pITableFrame->WriteUserScore(wChairID, ScoreInfo,20,20);*/
		//}

		//房卡模式
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0)
	{
		//约战解散
	}

	//重置历史积分
	//m_HistoryScore.ResetData();
	//m_vecRecord[wChairID].clear();
	return true;
}

//用户同意
bool  CTableFrameSink::OnActionUserReady(IServerUserItem* pIServerUserItem, VOID* pData, WORD wDataSize)
{
	//准备清空上局记录
	//ZeroMemory(m_tagHistoryChart[wChairID], sizeof(m_tagHistoryChart[wChairID]));

	return true;
}

bool CTableFrameSink::OnUserTrustee(WORD wChairID, bool bTrustee)
{
	//房卡积分模式禁用托管
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0)
	{
		return false;
	}

	//删除托管定时器
	m_pITableFrame->KillGameTimer(IDI_OUT_CARD_0 + wChairID);

	//
	IServerUserItem* pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if (bTrustee)
	{
		//设置托管标志
		pIServerUserItem->SetTrusteeUser(true);

		//前端发牌需要10秒,在发完牌前主动托管，等完牌再操作
		CTime ctCurTime = CTime::GetCurrentTime();
		//CTimeSpan TimeSpan = ctCurTime - m_ctStartTime;
		//BYTE cbTempTime = TimeSpan.GetSeconds() < 10 ? (10 - TimeSpan.GetSeconds()) : 1;

		//m_pITableFrame->SetGameTimer(IDI_OUT_CARD_0 + wChairID, cbTempTime * 1000, -1, NULL);
	}
	else
	{
		pIServerUserItem->SetTrusteeUser(false);
	}

	return true;
}

//开始人数
WORD CTableFrameSink::GetStartPlayerCount()
{
	return 0;

}

//允许准备
bool CTableFrameSink::IsAllowPlayerReady(WORD wChairID)
{
	/*IServerUserItem* pServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if (pServerUserItem == NULL) return false;

	if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0
		&& pServerUserItem->GetScoreKind() == SCORE_KIND_BATTLE)
	{
		return m_lUserFreeScore[wChairID] >= m_CustomConfig.lMinBetAmount;
	}
	return (GetUserScore(wChairID) >= m_lCellScore * MIN_MUTIPLIE);*/
	return true;
}

//底分变更
VOID CTableFrameSink::OnEventBaseScoreVariation(LONG lBaseScore)
{
	//CString s;
	//s.Format(_T("a2  lBaseScore:%I64d"), lBaseScore);
	//CTraceService::TraceString(_T("s12345"), TraceLevel_Exception);
}

//分数变更
VOID CTableFrameSink::OnEventScoreVariation(IServerUserItem* pIServerUserItem)
{
	//更新分数
	/*if (pIServerUserItem->GetUserStatus() != US_LOOKON)
	{
		UpdateUserScore(pIServerUserItem->GetChairID());
	}*/
}

//桌子解散
VOID CTableFrameSink::OnEventTableDismiss()
{
	//清理数据
	//ZeroMemory(m_lUserScore, sizeof(m_lUserScore));
	//ZeroMemory(m_lUserFreeScore, sizeof(m_lUserFreeScore));
}

//桌子修改
VOID CTableFrameSink::OnEventTableModify(LPVOID pModifyParam, WORD wBufferSize)
{
	tagBattleCreateParam* pParam = (tagBattleCreateParam*)pModifyParam;

	CopyMemory(&m_BattleConfig, pParam, wBufferSize);

	// kk jia start
	//m_lWinnerScore1 = pParam->BattleConfig.lMinWin;						//赢家分数
	//m_lWinnerScore2 = pParam->BattleConfig.lWin2;						//赢家分数
	//m_lWinnerRevenue1 = pParam->BattleConfig.lRevenueRate1;					//赢家税收	

	//m_lWinnerScore3 = pParam->BattleConfig.lWin3;						//赢家分	

	//m_lWinnerRevenue2 = pParam->BattleConfig.lRevenueRate2;					//赢家税收
	//m_lWinnerRevenue3 = pParam->BattleConfig.lRevenueRate3;					//赢家税收

	//CString s;
	//s.Format(_T("修改桌子 m_lWinnerScore1:%I64d, m_lWinnerScore2:%I64d, m_lWinnerScore3:%I64d, m_lWinnerRevenue1:%I64d, m_lWinnerRevenue2:%I64d, m_lWinnerRevenue3:%I64d"), m_lWinnerScore1, m_lWinnerScore2, m_lWinnerScore3, m_lWinnerRevenue1, m_lWinnerRevenue2, m_lWinnerRevenue3);
	//CTraceService::TraceString(s, TraceLevel_Exception);

	//kk jia end

	//BYTE cbIndex = 0;

	////先后手
	//bool bAllowBankerFirst = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;

	////特牌
	//bool bAllowSpecialCard = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? false : true;

	////地九王算大牌
	//bool bAllowDJWBigCard = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;

	////滚滚翻
	//bool bAllowBetGGF = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;


	//SCORE lMaxBoboMutiple[3] = { 300,500,1000 };

	//BYTE cbSelectIndex = pParam->BattleConfig.cbGameOption[cbIndex++];
	//if (cbSelectIndex < 3)
	//{
	//	//携带筹码
	//	m_CustomConfig.lMaxBoboMultiple = lMaxBoboMutiple[cbSelectIndex];
	//	//CString s;
	//	//s.Format(_T("修改桌子后 携带筹码:%ld"), m_CustomConfig.lMaxBoboMultiple);
	//	//CTraceService::TraceString(s, TraceLevel_Exception);

	//	//初始分数
	//	for (WORD i = 0; i < GAME_PLAYER; i++)
	//	{
	//		m_lUserScore[i] = 0;

	//		//m_lUserFreeScore[i] = m_CustomConfig.lMaxBoboMultiple * m_lCellScore;//kk guan
	//		//查询用户
	//		IServerUserItem* pIServerUserItem = m_pITableFrame->GetTableUserItem(i);//kk jia
	//		if (pIServerUserItem != NULL)//kk jia
	//			m_lUserFreeScore[i] = pIServerUserItem->GetUserScore();//kk jia
	//	}
	//}

	//WORD wMinBetValue = 0;

	//CopyMemory(&wMinBetValue, &pParam->BattleConfig.cbGameOption[cbIndex], sizeof(BYTE) * 2);

	////携带筹码
	//m_CustomConfig.lMinBetAmount = static_cast<SCORE>(wMinBetValue) * m_lCellScore;


	////先后手
	//if (bAllowBankerFirst == true)
	//{
	//	m_CustomConfig.cbRuleOption |= GR_ALLOW_FIRSTER;
	//}
	//else
	//{
	//	m_CustomConfig.cbRuleOption &= ~GR_ALLOW_FIRSTER;
	//}

	////特牌
	//if (bAllowSpecialCard == true)
	//{
	//	m_CustomConfig.cbRuleOption |= GR_SHS_AND_SHL;
	//}
	//else
	//{
	//	m_CustomConfig.cbRuleOption &= ~GR_SHS_AND_SHL;
	//}

	////地九王算大牌
	//if (bAllowDJWBigCard == true)
	//{
	//	m_CustomConfig.cbRuleOption |= GR_DJW_BIG_CARD;
	//}
	//else
	//{
	//	m_CustomConfig.cbRuleOption &= ~GR_DJW_BIG_CARD;
	//}

	////下注滚滚翻
	//if (bAllowBetGGF == true)
	//{
	//	m_CustomConfig.cbRuleOption |= GR_ALLOW_BET_GGF;
	//}
	//else
	//{
	//	m_CustomConfig.cbRuleOption &= ~GR_ALLOW_BET_GGF;
	//}

	////设置规则
	//m_GameLogic.SetRuleOption(m_CustomConfig.cbRuleOption);


	////坐下倍数
	//m_pITableFrame->SetSitdownGameMultiple(MIN_MUTIPLIE);

	////kk jia
	//m_pITableFrame->SetMinEnterScore(m_CustomConfig.lMaxBoboMultiple);

	////起立倍数
	//m_pITableFrame->SetStandupGameMultiple(MIN_MUTIPLIE);

	m_pITableFrame->SetMaxPlayerCount(m_BattleConfig.BattleConfig.wPlayerCount);

	return;
}

//桌子创建
VOID CTableFrameSink::OnEventTableCreated(LPVOID pCreateConfig, WORD wBufferSize)
{
	tagBattleCreateParam* pParam = (tagBattleCreateParam*)pCreateConfig;

	CopyMemory(&m_BattleConfig, pParam, wBufferSize);

	//// kk jia start
	//m_lWinnerScore1 = pParam->BattleConfig.lMinWin;						//赢家分数
	//m_lWinnerScore2 = pParam->BattleConfig.lWin2;						//赢家分数
	//m_lWinnerRevenue1 = pParam->BattleConfig.lRevenueRate1;					//赢家税收	

	//m_lWinnerScore3 = pParam->BattleConfig.lWin3;						//赢家分	

	//m_lWinnerRevenue2 = pParam->BattleConfig.lRevenueRate2;					//赢家税收
	//m_lWinnerRevenue3 = pParam->BattleConfig.lRevenueRate3;					//赢家税收

	//CString s;
	//s.Format(_T("create桌子 m_lWinnerScore1:%I64d, m_lWinnerScore2:%I64d, m_lWinnerScore3:%I64d, m_lWinnerRevenue1:%I64d, m_lWinnerRevenue2:%I64d, m_lWinnerRevenue3:%I64d"), m_lWinnerScore1, m_lWinnerScore2, m_lWinnerScore3, m_lWinnerRevenue1, m_lWinnerRevenue2, m_lWinnerRevenue3);
	//CTraceService::TraceString(s, TraceLevel_Exception);

	//kk jia end


	//BYTE cbIndex = 0;

	////先后手
	//bool bAllowBankerFirst = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;

	////特牌
	//bool bAllowSpecialCard = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? false : true;

	////地九王算大牌
	//bool bAllowDJWBigCard = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;

	////滚滚翻
	//bool bAllowBetGGF = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;


	//SCORE lMaxBoboMutiple[3] = { 300,500,1000 };

	//BYTE cbSelectIndex = pParam->BattleConfig.cbGameOption[cbIndex++];
	//if (cbSelectIndex < 3)
	//{
	//	//携带筹码
	//	m_CustomConfig.lMaxBoboMultiple = lMaxBoboMutiple[cbSelectIndex];

	//	//CString s;
	//	//s.Format(_T("携带筹码:%ld"), m_CustomConfig.lMaxBoboMultiple);
	//	//CTraceService::TraceString(s, TraceLevel_Exception);

	//	//初始分数
	//	for (WORD i = 0; i < GAME_PLAYER; i++)
	//	{
	//		m_lUserScore[i] = 0;

	//		//m_lUserFreeScore[i] = m_CustomConfig.lMaxBoboMultiple * m_lCellScore;//kk guan
	//		//查询用户
	//		IServerUserItem* pIServerUserItem = m_pITableFrame->GetTableUserItem(i);//kk jia
	//		if (pIServerUserItem != NULL)//kk jia
	//			m_lUserFreeScore[i] = pIServerUserItem->GetUserScore();//kk jia
	//	}
	//}

	//SCORE lMinBetValue = 0;

	//CopyMemory(&lMinBetValue, &pParam->BattleConfig.cbGameOption[cbIndex], sizeof(BYTE) * 4);
	//m_lMinBetValue = lMinBetValue;

	///*CString s;
	//	s.Format(_T("lMinBetValue:%ld"), lMinBetValue);
	//	CTraceService::TraceString(s, TraceLevel_Exception);*/

	//	//携带筹码
	//m_CustomConfig.lMinBetAmount = static_cast<SCORE>(lMinBetValue) * m_lCellScore;


	////先后手
	//if (bAllowBankerFirst == true)
	//{
	//	m_CustomConfig.cbRuleOption |= GR_ALLOW_FIRSTER;
	//}
	//else
	//{
	//	m_CustomConfig.cbRuleOption &= ~GR_ALLOW_FIRSTER;
	//}

	////特牌
	//if (bAllowSpecialCard == true)
	//{
	//	m_CustomConfig.cbRuleOption |= GR_SHS_AND_SHL;
	//}
	//else
	//{
	//	m_CustomConfig.cbRuleOption &= ~GR_SHS_AND_SHL;
	//}

	////地九王算大牌
	//if (bAllowDJWBigCard == true)
	//{
	//	m_CustomConfig.cbRuleOption |= GR_DJW_BIG_CARD;
	//}
	//else
	//{
	//	m_CustomConfig.cbRuleOption &= ~GR_DJW_BIG_CARD;
	//}

	////下注滚滚翻
	//if (bAllowBetGGF == true)
	//{
	//	m_CustomConfig.cbRuleOption |= GR_ALLOW_BET_GGF;
	//}
	//else
	//{
	//	m_CustomConfig.cbRuleOption &= ~GR_ALLOW_BET_GGF;
	//}

	////设置规则
	//m_GameLogic.SetRuleOption(m_CustomConfig.cbRuleOption);

	//m_wBattleCount = 0;

	////坐下倍数
	//m_pITableFrame->SetSitdownGameMultiple(MIN_MUTIPLIE);
	////kk jia
	//m_pITableFrame->SetMinEnterScore(m_CustomConfig.lMaxBoboMultiple);

	////起立倍数
	//m_pITableFrame->SetStandupGameMultiple(MIN_MUTIPLIE);

	m_pITableFrame->SetMaxPlayerCount(m_BattleConfig.BattleConfig.wPlayerCount);
	return;
}

bool CTableFrameSink::OnEventControlSendCard(IServerUserItem* pIServerUserItem, void* pData, WORD wDataSize)
{
	//ASSERT(wDataSize == sizeof(tagConfigCard));
	//if (wDataSize != sizeof(tagConfigCard)) return false;

	//tagConfigCard* pConfigCard = (tagConfigCard*)pData;

	//m_bAllotCard = true;
	////m_wControBankerUser = 0;
	//BYTE cbCardCount = 0;

	//for (BYTE i = 0; i < GAME_PLAYER; ++i)
	//{
	//	CopyMemory(&m_cbControlCardData[i], &pConfigCard->cbCardData[cbCardCount], sizeof(BYTE) * MAX_COUNT);
	//	cbCardCount += MAX_COUNT;
	//}

	return true;
}

//////////////////////////////////////////////////////////////////////////
