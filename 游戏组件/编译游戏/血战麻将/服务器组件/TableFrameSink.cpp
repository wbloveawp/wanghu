#include "StdAfx.h"
#include "TableFrameSink.h"
#include <locale>
#include "CLog.h"
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
const WORD			CTableFrameSink::m_wPlayerCount = GAME_PLAYER;			//游戏人数

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
	m_lSiceCount=0;
	m_wBankerUser=INVALID_CHAIR;
	m_wFirstWinUser=INVALID_CHAIR;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	ZeroMemory( m_GangScore,sizeof(m_GangScore) );
	ZeroMemory( m_bPlayStatus,sizeof(m_bPlayStatus) );
	ZeroMemory( m_lGameScore,sizeof(m_lGameScore) );
	m_cbWinCount = 0;
	m_lCellScore = 0;
	memset( m_wWinOrder,INVALID_CHAIR,sizeof(m_wWinOrder) );
	ZeroMemory( m_lGameTax,sizeof(m_lGameTax) );
	
	m_cbChangeType = INVALID_BYTE;
	ZeroMemory(m_cbChangeCardDataBefore, sizeof(m_cbChangeCardDataBefore));
	ZeroMemory(m_cbChangeCardDataAfter, sizeof(m_cbChangeCardDataAfter));
	ZeroMemory(m_bChangeCard, sizeof(m_bChangeCard));
	m_cbTrusteeDelayTime = 5;

	//缺门信息
	ZeroMemory(m_bCallCard, sizeof(m_bCallCard));
	ZeroMemory(m_cbCallCard, sizeof(m_cbCallCard));

	//出牌信息
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//堆立信息
	m_wHeapHead = INVALID_CHAIR;
	m_wHeapTail = INVALID_CHAIR;
	m_cbMinusHeadCount = 0;
	m_cbMinusLastCount = 0;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//发牌信息
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));

	//运行变量
	m_cbProvideCard=0;
	m_wResumeUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;
	m_wProvideGang=INVALID_CHAIR;

	//状态变量
	m_bSendStatus=false;
	m_bGangStatus = false;
	m_bGangOutStatus = false;
	ZeroMemory(m_bEnjoinChiHu,sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng,sizeof(m_bEnjoinChiPeng));

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//组合扑克
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//结束信息
	m_cbChiHuCard=0;
	ZeroMemory(m_cbChiHuCardArray, sizeof(m_cbChiHuCardArray));
	ZeroMemory( m_dwChiHuKind,sizeof(m_dwChiHuKind) );
	memset( m_wProvider,INVALID_CHAIR,sizeof(m_wProvider) );
	ZeroMemory(m_bWaitWriteScore, sizeof(m_bWaitWriteScore));

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;
	m_pGameCustomRule = NULL;

	//游戏规则（房卡配置）
	m_bHuanSanZhang = true;
	m_bHuJiaoZhuanYi = true;
	m_bZiMoAddTimes = true;
	m_bTianDiHu = true;
	m_bHaiDiLaoYue = true;
	m_bMenQing = true;
	
	ZeroMemory(&m_stRecord,sizeof(m_stRecord));
	ZeroMemory(m_tagHistoryChart, sizeof(m_tagHistoryChart));
	ZeroMemory(m_cbHistoryChartCount, sizeof(m_cbHistoryChartCount));
	ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));

	//随机种子
	srand((int)time(0));
	
#ifdef CARD_CONFIG
	ZeroMemory(m_cbconfigCard, sizeof(m_cbconfigCard));
#endif

	//游戏视频
	m_hVideoInst = NULL;
	m_pGameVideo = NULL;
	m_hVideoInst = LoadLibrary(TEXT("sparrowhzdgkVideo.dll"));
	if (m_hVideoInst)
	{
		typedef void * (*CREATE)();
		CREATE GameVideo = (CREATE)GetProcAddress(m_hVideoInst, "CreateGameVideo");
		if (GameVideo)
		{
			m_pGameVideo = static_cast<IGameVideo*>(GameVideo());
		}
	}

	ZeroMemory(m_lCurTotalWinloseScore, sizeof(m_lCurTotalWinloseScore));
	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
	if (m_pGameVideo)
	{
		delete m_pGameVideo;
		m_pGameVideo = NULL;
	}

	if (m_hVideoInst)
	{
		FreeLibrary(m_hVideoInst);
		m_hVideoInst = NULL;
	}
}

//接口查询
void * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool  CTableFrameSink::InitTableFrameSink(IUnknownEx* pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrameItem);
	
	if (m_pITableFrame==NULL)
		return false;

	//获取参数
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption!=NULL);

	//开始模式
	m_pITableFrame->SetStartMode(StartMode_AllReady);
	
	//自定规则
	/*ASSERT (m_pITableFrame->GetCustomRule() != NULL);
	m_pGameCustomRule = (tagCustomRule *)m_pITableFrame->GetCustomRule();
	
	m_bHuanSanZhang = m_pGameCustomRule->bHuanSanZhang;
	m_bHuJiaoZhuanYi = m_pGameCustomRule->bHuJiaoZhuanYi;
	m_bZiMoAddTimes = m_pGameCustomRule->bZiMoAddTimes;
	m_bTianDiHu = m_pGameCustomRule->bTianDiHu;
	m_bHaiDiLaoYue = m_pGameCustomRule->bHaiDiLaoYue;
	m_bMenQing = m_pGameCustomRule->bMenQing;
	m_cbTrusteeDelayTime = m_pGameCustomRule->cbTrusteeDelayTime;*/

	//日志
	CString strDir = GetFileDialogPath() + TEXT("\\") + GAME_NAME + TEXT("日志");
	CreateDirectory(strDir, NULL);
	strDir.AppendFormat(TEXT("\\%s(%d)"), GAME_NAME, m_pITableFrame->GetGameServiceOption()->wServerID);
	EasyLog::CLog::m_log.SetName(strDir.GetBuffer());

	//BYTE i = m_pITableFrame->GetDataBaseMode();
	return true;
}

//复位桌子
void  CTableFrameSink::ResetTableFrameSink()
{
	//游戏变量
	m_lSiceCount=0;
	ZeroMemory(m_cbCardIndex,sizeof(m_cbCardIndex));
	ZeroMemory(m_bTrustee,sizeof(m_bTrustee));
	ZeroMemory( m_GangScore,sizeof(m_GangScore) );
	ZeroMemory( m_lGameScore,sizeof(m_lGameScore) );
	ZeroMemory( m_bPlayStatus,sizeof(m_bPlayStatus) );
	m_cbWinCount = 0;
	memset( m_wWinOrder,INVALID_CHAIR,sizeof(m_wWinOrder) );
	ZeroMemory( m_lGameTax,sizeof(m_lGameTax) );
	
	m_cbChangeType = INVALID_BYTE;
	ZeroMemory(m_cbChangeCardDataBefore, sizeof(m_cbChangeCardDataBefore));
	ZeroMemory(m_cbChangeCardDataAfter, sizeof(m_cbChangeCardDataAfter));
	ZeroMemory(m_bChangeCard, sizeof(m_bChangeCard));

	//缺门信息
	ZeroMemory(m_bCallCard, sizeof(m_bCallCard));
	ZeroMemory(m_cbCallCard, sizeof(m_cbCallCard));

	//出牌信息
	m_cbOutCardData=0;
	m_cbOutCardCount=0;
	m_wOutCardUser=INVALID_CHAIR;
	ZeroMemory(m_cbDiscardCard,sizeof(m_cbDiscardCard));
	ZeroMemory(m_cbDiscardCount,sizeof(m_cbDiscardCount));

	//堆立信息
	m_wHeapHead = INVALID_CHAIR;
	m_wHeapTail = INVALID_CHAIR;
	m_cbMinusHeadCount = 0;
	m_cbMinusLastCount = 0;
	ZeroMemory(m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

	//发牌信息
	m_cbSendCardData=0;
	m_cbSendCardCount=0;
	m_cbLeftCardCount=0;
	ZeroMemory(m_cbRepertoryCard,sizeof(m_cbRepertoryCard));

	//运行变量
	m_cbProvideCard=0;
	m_wResumeUser=INVALID_CHAIR;
	m_wBankerUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;
	m_wProvideUser=INVALID_CHAIR;
	m_wProvideGang=INVALID_CHAIR;

	//状态变量
	m_bSendStatus=false;
	m_bGangStatus = false;
	m_bGangOutStatus = false;
	ZeroMemory(m_bEnjoinChiHu,sizeof(m_bEnjoinChiHu));
	ZeroMemory(m_bEnjoinChiPeng,sizeof(m_bEnjoinChiPeng));

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbOperateCard,sizeof(m_cbOperateCard));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//组合扑克
	ZeroMemory(m_WeaveItemArray,sizeof(m_WeaveItemArray));
	ZeroMemory(m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

	//结束信息
	m_cbChiHuCard=0;
	ZeroMemory(m_cbChiHuCardArray, sizeof(m_cbChiHuCardArray));
	ZeroMemory(m_dwChiHuKind,sizeof(m_dwChiHuKind));
	ZeroMemory(m_bWaitWriteScore, sizeof(m_bWaitWriteScore));

	/*ZeroMemory(m_tagHistoryChart, sizeof(m_tagHistoryChart));*/
	ZeroMemory(m_cbHistoryChartCount, sizeof(m_cbHistoryChartCount));

	for( WORD i = 0; i < GAME_PLAYER; i++ )
		m_ChiHuRight[i].SetEmpty();

	ZeroMemory(m_lCurTotalWinloseScore, sizeof(m_lCurTotalWinloseScore));
	return;
}

//最少积分
SCOREEX CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem)
{
	return 0;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{
	ZeroMemory(m_tagHistoryChart, sizeof(m_tagHistoryChart));

	//换三张，前端发牌需要10秒
	m_ctStartTime = CTime::GetCurrentTime();
	if (m_bHuanSanZhang)
	{
		//设置状态
		m_ctChangeTime = CTime::GetCurrentTime();
		m_pITableFrame->SetGameStatus(GS_MJ_CHANGECARD);

		//设置超时定时器
		m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
		m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, (TIME_CHANGE_CARD + 10) * 1000, 1, 0);
	}
	else
	{
		//设置状态
		m_ctChangeTime = CTime::GetCurrentTime();
		m_pITableFrame->SetGameStatus(GS_MJ_CALL);

		//设置超时定时器
		m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
		m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, (TIME_CALL_CARD + 10) * 1000, 1, 0);
	}

	//混乱扑克
	m_cbLeftCardCount=CountArray(m_cbRepertoryCard);
	m_GameLogic.RandCardData(m_cbRepertoryCard,CountArray(m_cbRepertoryCard));

	m_lCellScore = m_pITableFrame->GetCellScore();
#ifdef CARD_CONFIG

	CString strPath;
	GetModuleFileName(NULL, strPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	strPath.ReleaseBuffer();
	strPath = strPath.Left(strPath.ReverseFind('\\'));
	CString strSaveFileName;
	strSaveFileName.Format(TEXT("%s\\ConfigCard\\SparrowXZ.card"), strPath);
	CFile file;
	if (file.Open(strSaveFileName, CFile::modeRead))
	{
		BYTE cbCardData[MAX_REPERTORY];
		ZeroMemory(cbCardData, sizeof(cbCardData));
		file.Read(&cbCardData, sizeof(cbCardData));
		file.Close();

		CopyMemory(m_cbRepertoryCard, cbCardData, sizeof(m_cbRepertoryCard));
	}

	//if (m_cbconfigCard[0] != 0 && m_cbconfigCard[1] != 0 && m_cbconfigCard[2] != 0)
	//{
	//	CopyMemory(m_cbRepertoryCard, m_cbconfigCard, sizeof(m_cbRepertoryCard));
	//}
#endif

	//设置变量
	m_lSiceCount = MAKELONG(MAKEWORD(rand()%6+1,rand()%6+1),MAKEWORD(rand()%6+1,rand()%6+1));

	//庄家设置
	if (m_wFirstWinUser==INVALID_CHAIR) 
	{
		m_wBankerUser = ((BYTE)(m_lSiceCount>>24)+(BYTE)(m_lSiceCount>>16)-1)%GAME_PLAYER;
	}
	else 
	{
		m_wBankerUser = m_wFirstWinUser;
		m_lSiceCount = m_lSiceCount&0x0000FFFF;
	}

	//分发扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if( m_pITableFrame->GetTableUserItem(i) != NULL )
		{
			m_bPlayStatus[i] = true;
			m_cbLeftCardCount-=(MAX_COUNT-1);

			m_cbMinusHeadCount += (MAX_COUNT - 1);

			m_GameLogic.SwitchToCardIndex(&m_cbRepertoryCard[m_cbLeftCardCount],MAX_COUNT-1,m_cbCardIndex[i]);
		}
		else
		{
			m_bPlayStatus[i] = false;
		}
	}

	//发送扑克
	m_cbMinusHeadCount++;
	m_cbSendCardCount++;
	m_cbSendCardData=m_cbRepertoryCard[--m_cbLeftCardCount];
	m_cbCardIndex[m_wBankerUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

	//设置变量
	m_cbProvideCard=0;
	m_wCurrentUser=m_wBankerUser;
	
	//庄家开始为供应者，放最后一张牌
	m_wProvideUser = m_wBankerUser;

	//堆立信息
	WORD wSice = WORD(m_lSiceCount&0xFFFF);
	BYTE cbSiceTakeCount= HIBYTE(wSice)+LOBYTE(wSice);
	BYTE cbSamllSiceTakeCount=__min(HIBYTE(wSice),LOBYTE(wSice));
	WORD wTakeChairID=(m_wBankerUser+cbSiceTakeCount-1)%GAME_PLAYER;
	BYTE cbTakeCount=MAX_REPERTORY-m_cbLeftCardCount;
	

	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//计算数目
		//真实CHAIRID 为1,3的28张，0,2为26张

		BYTE cbValidCount = 0;
		if (wTakeChairID == 1 || wTakeChairID == 3)
		{
			cbValidCount = HEAP_FULL_COUNT + 2-m_cbHeapCardInfo[wTakeChairID][1]-((i==0)?(cbSamllSiceTakeCount)*2:0);
		}
		else if (wTakeChairID == 0 || wTakeChairID == 2)
		{
			cbValidCount = HEAP_FULL_COUNT -m_cbHeapCardInfo[wTakeChairID][1]-((i==0)?(cbSamllSiceTakeCount)*2:0);
		}

		BYTE cbRemoveCount=__min(cbValidCount,cbTakeCount);

		//提取扑克
		cbTakeCount-=cbRemoveCount;
		m_cbHeapCardInfo[wTakeChairID][(i==0)?1:0]+=cbRemoveCount;

		//完成判断
		if (cbTakeCount==0)
		{
			m_wHeapHead=wTakeChairID;
			m_wHeapTail=(m_wBankerUser+cbSiceTakeCount-1)%GAME_PLAYER;
				
			break;
		}
		//切换索引
		wTakeChairID=(wTakeChairID + GAME_PLAYER - 1)%GAME_PLAYER;
	}
	
	//不换三张
	if (!m_bHuanSanZhang)
	{
		//动作分析
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			//庄家判断
			if (i==m_wBankerUser)
			{
				//杠牌判断
				tagGangCardResult GangCardResult;
				m_cbUserAction[i]|=m_GameLogic.AnalyseGangCard(m_cbCardIndex[i],NULL,0,GangCardResult);

				//胡牌判断
				CChiHuRight chr;
				m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]--;
				m_cbUserAction[i]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],NULL,0,m_cbSendCardData,chr, m_cbCallCard[i]);
				m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;
			}
		}
	}

	//构造数据
	CMD_S_GameStart GameStart;
	ZeroMemory(&GameStart, sizeof(GameStart));

	GameStart.lSiceCount=m_lSiceCount;
	GameStart.wBankerUser=m_wBankerUser;
	GameStart.wCurrentUser=m_wCurrentUser;
	GameStart.wHeapHand = m_wHeapHead;
	GameStart.wHeapTail = m_wHeapTail;
	GameStart.cbLeftCardCount = m_cbLeftCardCount;
	CopyMemory(GameStart.cbHeapCardInfo,m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));
	GameStart.bHuanSanZhang = m_bHuanSanZhang;

	if (m_pGameVideo)
	{
		m_pGameVideo->StartVideo(m_pITableFrame, GAME_PLAYER);
	}

	bool bFirstRecord = true;

	//历史积分
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//变量定义
		tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

		//设置变量
		GameStart.lTurnScore[i]=pHistoryScore->lTurnScore;
		GameStart.lCollectScore[i]=pHistoryScore->lCollectScore;
	}

	//发送数据
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_bPlayStatus[i] == false) continue;

		//设置变量
		GameStart.cbUserAction=m_cbUserAction[i];
		ZeroMemory(GameStart.cbCardData,sizeof(GameStart.cbCardData));
		m_GameLogic.SwitchToCardData(m_cbCardIndex[i],GameStart.cbCardData);

		GameStart.cbSendCardData = (i == m_wBankerUser ? m_cbSendCardData : INVALID_BYTE);

		if (m_bHuanSanZhang)
		{
			BYTE cbChangeCard[3] = { 0 };
			m_GameLogic.AutoHuangSanZhang(m_cbCardIndex[i], cbChangeCard);
			CopyMemory(GameStart.cbPriorChangeCardData, cbChangeCard, sizeof(cbChangeCard));
		}

		//发送数据
		m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		m_pITableFrame->SendLookonData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));

		IServerUserItem *pServerUserItem = m_pITableFrame->GetTableUserItem(i);

		if (m_pGameVideo)
		{
			Video_GameStart video;
			ZeroMemory(&video, sizeof(video));

			video.bHuanSanZhang = m_bHuanSanZhang;
			video.bHuJiaoZhuanYi = m_bHuJiaoZhuanYi;
			video.bZiMoAddTimes = m_bZiMoAddTimes;
			video.bTianDiHu = m_bTianDiHu;
			video.bHaiDiLaoYue = m_bHaiDiLaoYue;
			video.bMenQing = m_bMenQing;

			video.lSiceCount = m_lSiceCount;
			video.wBankerUser = GameStart.wBankerUser;
			video.wCurrentUser = GameStart.wCurrentUser;
			video.cbUserAction = GameStart.cbUserAction;
			CopyMemory(video.cbCardData, GameStart.cbCardData, sizeof(video.cbCardData));

			video.cbLeftCardCount = GameStart.cbLeftCardCount;
			video.cbSendCardData = GameStart.cbSendCardData;

			//CopyMemory(video.szNickName, pServerUserItem->GetUserInfo()->sz, sizeof(video.szNickName));
			video.wChairID = i;
			video.lScore = pServerUserItem->GetUserScore();

			m_pGameVideo->AddVideoData(SUB_S_GAME_START, &video, bFirstRecord);

			if (bFirstRecord == true)
			{
				bFirstRecord = false;
			}
		}
	}

	//设置门清
	m_GameLogic.SetEnableMenQing(m_bMenQing);

	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_NORMAL:		//常规结束
		{
			m_pITableFrame->KillGameTimer(IDI_OUT_CARD_0);
			m_pITableFrame->KillGameTimer(IDI_OUT_CARD_1);
			m_pITableFrame->KillGameTimer(IDI_OUT_CARD_2);
			m_pITableFrame->KillGameTimer(IDI_OUT_CARD_3);
			m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

			//变量定义
			CMD_S_GameEnd& GameEnd = m_stGameEnd;
			ZeroMemory(&GameEnd, sizeof(GameEnd));
			GameEnd.wLeftUser = INVALID_CHAIR;

			//结束信息
			WORD wWinner = INVALID_CHAIR;

			BYTE cbCardIndex[GAME_PLAYER][MAX_INDEX];
			ZeroMemory(cbCardIndex, sizeof(cbCardIndex));
			CopyMemory(cbCardIndex, m_cbCardIndex, sizeof(m_cbCardIndex));

			CopyMemory(GameEnd.cbChiHuCardArray, m_cbChiHuCardArray, sizeof(m_cbChiHuCardArray));
			
			//未吃胡玩家
			BYTE cbLeftUserCount = 0;	
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				if (m_cbChiHuCardArray[i] != 0)
				{
					m_GameLogic.RemoveCard(cbCardIndex[i], m_cbChiHuCardArray[i]);
					GameEnd.cbCardCount[i] = m_GameLogic.SwitchToCardData(cbCardIndex[i], GameEnd.cbCardData[i]);
				}
				else
				{
					GameEnd.cbCardCount[i] = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameEnd.cbCardData[i]);
				}

				m_ChiHuRight[i].GetRightData(&GameEnd.dwChiHuRight[i], MAX_RIGHT_COUNT);

				if (m_ChiHuRight[i].IsEmpty()) 
				{
					cbLeftUserCount++;
				}

				GameEnd.cbPlayerGiveup[i] = (m_ChiHuRight[i].IsEmpty() && !m_bPlayStatus[i]) ? true : false;
			}

			SCOREEX lHuaZhuScore[GAME_PLAYER];
			SCOREEX lChaJiaoScore[GAME_PLAYER];
			SCOREEX lHuaZhuScoreEx[GAME_PLAYER][GAME_PLAYER];
			SCOREEX lChaJiaoScoreEx[GAME_PLAYER][GAME_PLAYER];
			SCOREEX lLostHuaZhuScore[GAME_PLAYER][GAME_PLAYER];
			SCOREEX lLostChaJiaoScore[GAME_PLAYER][GAME_PLAYER];
			SCOREEX lGangScore[GAME_PLAYER];
			ZeroMemory(lGangScore, sizeof(lGangScore));
			ZeroMemory(lHuaZhuScore, sizeof(lHuaZhuScore));
			ZeroMemory(lChaJiaoScore, sizeof(lChaJiaoScore));
			ZeroMemory(lHuaZhuScoreEx, sizeof(lHuaZhuScoreEx));
			ZeroMemory(lChaJiaoScoreEx, sizeof(lChaJiaoScoreEx));
			ZeroMemory(lLostHuaZhuScore, sizeof(lLostHuaZhuScore));
			ZeroMemory(lLostChaJiaoScore, sizeof(lLostChaJiaoScore));
			
			//流局
			if (IsLiuju())
			{
				//没有胡牌的玩家
				ASSERT (cbLeftUserCount >= 1);

				SCOREEX lOffsetScore[GAME_PLAYER];
				memset(lOffsetScore, 0, sizeof(lOffsetScore));

				for (WORD i=0; i<GAME_PLAYER; i++)
				{
					if (m_bPlayStatus[i] && m_ChiHuRight[i].IsEmpty())
					{
						//查花猪(如果被查花猪，则不查大叫)
						bool bHuaZhu = m_GameLogic.IsHuaZhu(m_cbCardIndex[i], m_WeaveItemArray[i], m_cbWeaveItemCount[i], m_cbCallCard[i]);
						if (bHuaZhu)
						{
							memset(lOffsetScore, 0, sizeof(lOffsetScore));

							//花猪赔给非花猪玩家64倍点数。
							for (WORD j=0; j<GAME_PLAYER; j++)
							{
								if (j == i || !m_bPlayStatus[j])
								{
									continue;
								}

								if (m_GameLogic.IsTingCard(m_cbCardIndex[j], m_WeaveItemArray[j], m_cbWeaveItemCount[j], m_cbCallCard[j]))
								{
									lOffsetScore[j] = m_lCellScore * 64L;
									lOffsetScore[i] -= m_lCellScore * 64L;
								}
							}

							//仅金币房生效
							LOG_TABLE(L"查花猪玩家椅子号:",i);
							ChangeUserPaidScore(lOffsetScore);
							if (D_UnEquals(lOffsetScore[i], 0))
							{
								WORD wChartHuZhuIndex = m_cbHistoryChartCount[i]++;
								m_tagHistoryChart[i][wChartHuZhuIndex].charttype = CHAHUAZHU_TYPE;
								m_tagHistoryChart[i][wChartHuZhuIndex].lScore = lOffsetScore[i];
								lHuaZhuScore[i] = lOffsetScore[i];
								//游戏流水记录
								for (int nIdx = 0; nIdx < GAME_PLAYER; ++nIdx)
								{
									if (nIdx == i)
										continue;
									if (D_UnGreaterThen(lOffsetScore[nIdx], 0))
										continue;

									ASSERT(m_cbHistoryChartCount[nIdx] < MAX_CHART_COUNT);
									WORD wChartIndex = m_cbHistoryChartCount[nIdx]++;
									m_tagHistoryChart[nIdx][wChartIndex].charttype = CHAHUAZHU_TYPE;
									m_tagHistoryChart[nIdx][wChartIndex].lTimes = 1;
									m_tagHistoryChart[nIdx][wChartIndex].bEffectChairID[i] = true;
									m_tagHistoryChart[nIdx][wChartIndex].lScore = lOffsetScore[nIdx];
									//
									m_tagHistoryChart[i][wChartHuZhuIndex].bEffectChairID[nIdx] = true;
									//
									lHuaZhuScore[nIdx] = lOffsetScore[nIdx];
									lHuaZhuScoreEx[i][nIdx] = lOffsetScore[nIdx];
									lLostHuaZhuScore[nIdx][i] = lOffsetScore[nIdx];
								}
							}
							
							//流水清单
							//退税玩家
							ASSERT (m_cbHistoryChartCount[i] < MAX_CHART_COUNT);
							WORD wChartHuZhuIndex = m_cbHistoryChartCount[i]++;
							m_tagHistoryChart[i][wChartHuZhuIndex].charttype = TUISHUI_TYPE;
							m_tagHistoryChart[i][wChartHuZhuIndex].lTimes = 1;
							SCOREEX lTuiShuiScore = 0LL;
							bool bTuiShuiScore = false;

							//退回杠别人的分数
							for (WORD j=0; j<m_GangScore[i].cbGangCount; j++)
							{
								//存在杠分
								if (D_GreaterThen(m_GangScore[i].lScore[j][i], 0))
								{
									for (WORD k=0; k<GAME_PLAYER; k++)
									{
										if (k == i || D_Equals(m_GangScore[i].lScore[j][k], 0))
										{
											continue;
										}

										bTuiShuiScore = true;
										lTuiShuiScore += m_GangScore[i].lScore[j][k];
										m_tagHistoryChart[i][wChartHuZhuIndex].bEffectChairID[k] = true;

										//被退税玩家
										ASSERT (m_cbHistoryChartCount[k] < MAX_CHART_COUNT);
										WORD wTuiShuiChartIndex = m_cbHistoryChartCount[k]++;
										m_tagHistoryChart[k][wTuiShuiChartIndex].charttype = TUISHUI_TYPE;
										m_tagHistoryChart[k][wTuiShuiChartIndex].lTimes = 1;
										m_tagHistoryChart[k][wTuiShuiChartIndex].lScore = -m_GangScore[i].lScore[j][k];
										m_tagHistoryChart[k][wTuiShuiChartIndex].bEffectChairID[i] = true; 
									}
								}


								//重置无效
								ZeroMemory(m_GangScore[i].lScore[j], sizeof(m_GangScore[i].lScore[j]));
								m_GangScore[i].nGangType[j] = INVALID_GANG;
							}

							m_tagHistoryChart[i][wChartHuZhuIndex].lScore = lTuiShuiScore;

							//不存在退税
							if (!bTuiShuiScore)
							{
								m_cbHistoryChartCount[i]--;
								ZeroMemory(&m_tagHistoryChart[i][wChartHuZhuIndex], sizeof(m_tagHistoryChart[i][wChartHuZhuIndex]));
							}
						}
						//不是花猪则查大叫
						else
						{
							//牌局结束（走三家或抓光牌）没有听牌退税
							if (!m_GameLogic.IsTingCard(m_cbCardIndex[i], m_WeaveItemArray[i], m_cbWeaveItemCount[i], m_cbCallCard[i]))
							{
								memset(lOffsetScore, 0, sizeof(lOffsetScore));

								//没听牌的玩家（花猪不用）赔给听牌的玩家最大的可能番数
								for (WORD j=0; j<GAME_PLAYER; j++)
								{
									if (j == i || !m_bPlayStatus[j]) 
									{
										continue;
									}
									if (m_GameLogic.IsTingCard(m_cbCardIndex[j], m_WeaveItemArray[j], m_cbWeaveItemCount[j], m_cbCallCard[j]))
									{
										//查找最大番数
										WORD wMaxFanShu = 0;
										CChiHuRight chr;
										for (BYTE k=0; k<MAX_INDEX; k++)
										{
											BYTE cbCurrentCard = m_GameLogic.SwitchToCardData(k);
											if (WIK_CHI_HU == m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[j], m_WeaveItemArray[j], m_cbWeaveItemCount[j], cbCurrentCard, chr, m_cbCallCard[j]))
											{
												FiltrateRight(j, chr);
												WORD wFanShuTemp = GetChiHuFanCount(chr);
		
												//取最大番数
												if (wFanShuTemp > wMaxFanShu )
												{
													wMaxFanShu = wFanShuTemp;
												}
											}
										}

										lOffsetScore[j] = m_lCellScore * wMaxFanShu;
										lOffsetScore[i] -= m_lCellScore * wMaxFanShu;
									}
								}

								//仅金币房生效
								LOG_TABLE(L"查大叫玩家椅子号:", i);
								ChangeUserPaidScore(lOffsetScore);

								if (D_UnEquals(lOffsetScore[i], 0))
								{

									WORD wChartChaDaJiaoIndex = m_cbHistoryChartCount[i]++;
									m_tagHistoryChart[i][wChartChaDaJiaoIndex].charttype = CHADAJIAO_TYPE;
									m_tagHistoryChart[i][wChartChaDaJiaoIndex].lScore = lOffsetScore[i];
									lChaJiaoScore[i] = lOffsetScore[i];
									//游戏流水记录
									for (int nIdx = 0; nIdx < GAME_PLAYER; ++nIdx)
									{
										if (nIdx == i)
											continue;
										if (D_UnGreaterThen(lOffsetScore[nIdx], 0))
											continue;

										ASSERT(m_cbHistoryChartCount[nIdx] < MAX_CHART_COUNT);
										WORD wChartIndex = m_cbHistoryChartCount[nIdx]++;
										m_tagHistoryChart[nIdx][wChartIndex].charttype = CHADAJIAO_TYPE;
										m_tagHistoryChart[nIdx][wChartIndex].lTimes = 1;
										m_tagHistoryChart[nIdx][wChartIndex].bEffectChairID[i] = true;
										m_tagHistoryChart[nIdx][wChartIndex].lScore = lOffsetScore[nIdx];
										//
										m_tagHistoryChart[i][wChartChaDaJiaoIndex].bEffectChairID[nIdx] = true;
										//
										lChaJiaoScore[nIdx] = lOffsetScore[nIdx];
										lChaJiaoScoreEx[i][nIdx] = lOffsetScore[nIdx];
										lLostChaJiaoScore[nIdx][i] = lOffsetScore[nIdx];
									}

								}

								//流水清单
								//退税玩家
								ASSERT (m_cbHistoryChartCount[i] < MAX_CHART_COUNT);
								WORD wChartIndex = m_cbHistoryChartCount[i]++;
								m_tagHistoryChart[i][wChartIndex].charttype = TUISHUI_TYPE;
								m_tagHistoryChart[i][wChartIndex].lTimes = 1;
								SCOREEX lTuiShuiScore = 0LL;
								bool bTuiShuiScore = false;

								//退回杠别人的分数
								for (WORD j=0; j<m_GangScore[i].cbGangCount; j++)
								{
									//存在杠分
									if (D_GreaterThen(m_GangScore[i].lScore[j][i], 0))
									{
										for (WORD k=0; k<GAME_PLAYER; k++)
										{
											if (k == i || D_Equals(m_GangScore[i].lScore[j][k], 0))
											{
												continue;
											}

											bTuiShuiScore = true;
											lTuiShuiScore += m_GangScore[i].lScore[j][k];
											m_tagHistoryChart[i][wChartIndex].bEffectChairID[k] = true;

											//被退税玩家
											ASSERT (m_cbHistoryChartCount[k] < MAX_CHART_COUNT);
											WORD wTuiShuiChartIndex = m_cbHistoryChartCount[k]++;
											m_tagHistoryChart[k][wTuiShuiChartIndex].charttype = TUISHUI_TYPE;
											m_tagHistoryChart[k][wTuiShuiChartIndex].lTimes = 1;
											m_tagHistoryChart[k][wTuiShuiChartIndex].lScore = -m_GangScore[i].lScore[j][k];
											m_tagHistoryChart[k][wTuiShuiChartIndex].bEffectChairID[i] = true; 
										}
									}


									//重置无效
									ZeroMemory(m_GangScore[i].lScore[j], sizeof(m_GangScore[i].lScore[j]));
									m_GangScore[i].nGangType[j] = INVALID_GANG;
								}

								m_tagHistoryChart[i][wChartIndex].lScore = lTuiShuiScore;

								//不存在退税
								if (!bTuiShuiScore)
								{
									m_cbHistoryChartCount[i]--;
									ZeroMemory(&m_tagHistoryChart[i][wChartIndex], sizeof(m_tagHistoryChart[i][wChartIndex]));
								}
							}
						}
					}
				}
			}
			//走三家
			else if (m_cbLeftCardCount != 0 && cbLeftUserCount == 1)
			{
				for (WORD i=0; i<GAME_PLAYER; i++)
				{
					if (m_bPlayStatus[i] && m_ChiHuRight[i].IsEmpty())
					{
						//牌局结束（走三家或抓光牌）没有听牌退税
						if (!m_GameLogic.IsTingCard(m_cbCardIndex[i], m_WeaveItemArray[i], m_cbWeaveItemCount[i], m_cbCallCard[i]))
						{
							//流水清单
							//退税玩家
							ASSERT (m_cbHistoryChartCount[i] < MAX_CHART_COUNT);
							WORD wChartIndex = m_cbHistoryChartCount[i]++;
							m_tagHistoryChart[i][wChartIndex].charttype = TUISHUI_TYPE;
							m_tagHistoryChart[i][wChartIndex].lTimes = 1;
							SCOREEX lTuiShuiScore = 0LL;
							bool bTuiShuiScore = false;

							//退回杠别人的分数
							for (WORD j=0; j<m_GangScore[i].cbGangCount; j++)
							{
								//存在杠分
								if (D_GreaterThen(m_GangScore[i].lScore[j][i], 0))
								{
									for (WORD k=0; k<GAME_PLAYER; k++)
									{
										if (k == i || D_Equals(m_GangScore[i].lScore[j][k], 0))
										{
											continue;
										}

										bTuiShuiScore = true;
										lTuiShuiScore += m_GangScore[i].lScore[j][k];
										m_tagHistoryChart[i][wChartIndex].bEffectChairID[k] = true;

										//被退税玩家
										ASSERT (m_cbHistoryChartCount[k] < MAX_CHART_COUNT);
										WORD wTuiShuiChartIndex = m_cbHistoryChartCount[k]++;
										m_tagHistoryChart[k][wTuiShuiChartIndex].charttype = TUISHUI_TYPE;
										m_tagHistoryChart[k][wTuiShuiChartIndex].lTimes = 1;
										m_tagHistoryChart[k][wTuiShuiChartIndex].lScore = -m_GangScore[i].lScore[j][k];
										m_tagHistoryChart[k][wTuiShuiChartIndex].bEffectChairID[i] = true; 
									}
								}


								//重置无效
								ZeroMemory(m_GangScore[i].lScore[j], sizeof(m_GangScore[i].lScore[j]));
								m_GangScore[i].nGangType[j] = INVALID_GANG;
							}

							m_tagHistoryChart[i][wChartIndex].lScore = lTuiShuiScore;

							//不存在退税
							if (!bTuiShuiScore)
							{
								m_cbHistoryChartCount[i]--;
								ZeroMemory(&m_tagHistoryChart[i][wChartIndex], sizeof(m_tagHistoryChart[i][wChartIndex]));
							}
						}
					}
				}
			}

			//算杠分
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				for (WORD j=0; j<m_GangScore[i].cbGangCount; j++)
				{
					for (WORD k=0; k<GAME_PLAYER; k++)
					{
						lGangScore[k] += m_GangScore[i].lScore[j][k];
					}
				}
			}
			
			ZeroMemory( m_lGameScore,sizeof(m_lGameScore) );
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				for (WORD j=0; j<MAX_CHART_COUNT; j++)
				{
					if (m_tagHistoryChart[i][j].charttype != INVALID_CHARTTYPE)
					{
						m_lGameScore[i] += m_tagHistoryChart[i][j].lScore;
					}
				}
			}

			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));
		
			//统计积分
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				if (NULL == m_pITableFrame->GetTableUserItem(i)) 
				{
					continue;
				}

				//设置积分 
				if (m_lGameScore[i] > 0L && ((m_pGameServiceOption->wServerType& GAME_GENRE_GOLD)==0 ))
				{
					m_lGameTax[i] = m_pITableFrame->AccountRevenue(m_lGameScore[i]);
					m_lGameScore[i] -= m_lGameTax[i];
				}

				if (m_lGameScore[i] > 0L) 
				{
					ScoreInfoArray[i].ScoreType = ScoreType_Win;
				}
				else if (m_lGameScore[i] < 0L) 
				{
					ScoreInfoArray[i].ScoreType = ScoreType_Lose;
				}
				else 
				{
					ScoreInfoArray[i].ScoreType = ScoreType_Draw;
				}

				//历史积分
				m_HistoryScore.OnEventUserScore(i,m_lGameScore[i]);

				ScoreInfoArray[i].lScore   = m_lGameScore[i];
				ScoreInfoArray[i].lRevenue = m_lGameTax[i];
				
				//约战记录
				if (m_stRecord.nCount < MAX_RECORD_COUNT)
				{
					m_stRecord.lDetailScore[i][m_stRecord.nCount] = ScoreInfoArray[i].lScore;
					m_stRecord.lAllScore[i] += ScoreInfoArray[i].lScore;
				}
			}
			
			m_stRecord.nCount++;
			
			//房卡模式
			/*if ((m_pGameServiceOption->wServerType & GAME_GENRE_PERSONAL) != 0)
			{
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
			}*/

			CopyMemory(GameEnd.lGameTax, m_lGameTax, sizeof(m_lGameTax));
			CopyMemory(GameEnd.lGangScore, lGangScore, sizeof(lGangScore));
			CopyMemory(GameEnd.wWinOrder, m_wWinOrder, sizeof(m_wWinOrder));
			CopyMemory(GameEnd.wProvideUser, m_wProvider, sizeof(m_wProvider));
			CopyMemory(GameEnd.lGameScore, m_lGameScore, sizeof(m_lGameScore));
			CopyMemory(GameEnd.lHuaZhuScore, lHuaZhuScoreEx, sizeof(lHuaZhuScoreEx));
			CopyMemory(GameEnd.lChaJiaoScore, lChaJiaoScoreEx, sizeof(lChaJiaoScoreEx));
			CopyMemory(GameEnd.lLostHuaZhuScore, lLostHuaZhuScore, sizeof(lLostHuaZhuScore));
			CopyMemory(GameEnd.lLostChaJiaoScore, lLostChaJiaoScore, sizeof(lLostChaJiaoScore));

			CopyMemory(GameEnd.tagHistoryChart, m_tagHistoryChart, sizeof(m_tagHistoryChart));
			
			//打印流水信息			
			/*CString strHistoryChart;
			CTime time = CTime::GetCurrentTime();
            strHistoryChart.Format(TEXT("table id is %d, 结算时间: %d/%d/%d-%d:%d:%d,----\n"), m_pITableFrame->GetTableID(),
				time.GetYear(), time.GetMonth(), time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());
            m_pITableFrame->SendGameLogData(strHistoryChart);*/

			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				CString strnickname;
				IServerUserItem *pIServerCur = m_pITableFrame->GetTableUserItem(i);
				if (NULL == pIServerCur) 
				{
					continue;
				}

				/*for (WORD j=0; j<MAX_CHART_COUNT; j++)
				{
					if (m_tagHistoryChart[i][j].charttype != INVALID_CHARTTYPE)
					{
                        strnickname.Format(TEXT("table id is %d, 当前昵称[%s], 流水类型 %d 倍数 %I64d, 得分 %.2f GameEndscore = %.2f 作用玩家 ----"), m_pITableFrame->GetTableID(),
							pIServerCur->GetNickName(), m_tagHistoryChart[i][j].charttype, m_tagHistoryChart[i][j].lTimes, m_tagHistoryChart[i][j].lScore,
							GameEnd.lGameScore[i]);

						for (WORD k=0; k<GAME_PLAYER; k++)
						{
							CString streffect;
							if (m_tagHistoryChart[i][j].bEffectChairID[k] == true)
							{
								IServerUserItem *pIServereffec = m_pITableFrame->GetTableUserItem(k);
								if (pIServereffec)
								{
									CString streffect;
									streffect.Format(TEXT("作用昵称[%s]  "), pIServereffec->GetNickName());

									strnickname += streffect;
								}
							}
						}
                        m_pITableFrame->SendGameLogData(strnickname);
						
					}
				}*/
			}

			if (m_pGameVideo)
			{
				Video_GameEnd VideoGameEnd;
				ZeroMemory(&VideoGameEnd, sizeof(VideoGameEnd));

				CopyMemory(VideoGameEnd.cbCardCount, GameEnd.cbCardCount, sizeof(VideoGameEnd.cbCardCount));
				CopyMemory(VideoGameEnd.cbCardData, GameEnd.cbCardData, sizeof(VideoGameEnd.cbCardData));
				CopyMemory(VideoGameEnd.wProvideUser, GameEnd.wProvideUser, sizeof(VideoGameEnd.wProvideUser));
				CopyMemory(VideoGameEnd.dwChiHuRight, GameEnd.dwChiHuRight, sizeof(VideoGameEnd.dwChiHuRight));
				CopyMemory(VideoGameEnd.lGameScore, GameEnd.lGameScore, sizeof(VideoGameEnd.lGameScore));
				CopyMemory(VideoGameEnd.lGameTax, GameEnd.lGameTax, sizeof(VideoGameEnd.lGameTax));
				CopyMemory(VideoGameEnd.tagHistoryChart, GameEnd.tagHistoryChart, sizeof(VideoGameEnd.tagHistoryChart));

				//组合扑克
				CopyMemory(VideoGameEnd.WeaveItemArray, m_WeaveItemArray, sizeof(m_WeaveItemArray));
				CopyMemory(VideoGameEnd.cbWeaveCount, m_cbWeaveItemCount, sizeof(m_cbWeaveItemCount));

				CopyMemory(VideoGameEnd.cbChiHuCardArray, m_cbChiHuCardArray, sizeof(m_cbChiHuCardArray));

				m_pGameVideo->AddVideoData(SUB_S_GAME_END, &VideoGameEnd);
				m_pGameVideo->StopAndSaveVideo(m_pGameServiceOption->wServerID, m_pITableFrame->GetTableID());
			}

			//写入积分
			for (WORD i = 0; i<GAME_PLAYER; i++)
			{
				if (NULL == m_pITableFrame->GetTableUserItem(i))
				{
					continue;
				}

				//未参与游戏，或中途离开已写分玩家
				if (m_bPlayStatus[i] == false && m_bWaitWriteScore[i] == false)
				{
					ScoreInfoArray[i].ScoreType = ScoreType_Null;
					continue;
				}

				//房卡模式
				if (((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0) && (m_RoomCardRecord.nCount < MAX_RECORD_COUNT))
				{
					m_RoomCardRecord.lDetailScore[i][m_RoomCardRecord.nCount] = ScoreInfoArray[i].lScore;
				}
				m_vecRecord[i].push_back(ScoreInfoArray[i].lScore);
				if (m_vecRecord[i].size() > 30)
					m_vecRecord[i].erase(m_vecRecord[i].begin());
			}
			m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray));

			//房卡模式
			if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0)
			{
				m_RoomCardRecord.nCount++;
			}

			//发送结束信息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

			//结束游戏

			//m_pITableFrame->ConcludeGame(GS_MJ_FREE);
			m_pITableFrame->SetGameStatus(GS_MJ_GAMEEND);
			m_pITableFrame->SetGameTimer(IDI_GAMEEND, DWORD(m_pGameCustomRule->cbTimeWaitEnd) * 1000, 1, 0);
			
			//房卡模式
			if (((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0))
			{
				//约战解散
				ZeroMemory(&m_stRecord, sizeof(m_stRecord));
				ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
			}

			return true;
		}
	case GER_DISMISS:		//游戏解散
		{
			m_pITableFrame->KillGameTimer(IDI_OUT_CARD_0);
			m_pITableFrame->KillGameTimer(IDI_OUT_CARD_1);
			m_pITableFrame->KillGameTimer(IDI_OUT_CARD_2);
			m_pITableFrame->KillGameTimer(IDI_OUT_CARD_3);
			m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

			//变量定义
			CMD_S_GameEnd& GameEnd = m_stGameEnd;
			ZeroMemory(&GameEnd, sizeof(GameEnd));
			GameEnd.wLeftUser = INVALID_CHAIR;

			//设置变量
			memset(GameEnd.wProvideUser, INVALID_CHAIR, sizeof(GameEnd.wProvideUser) );

			//拷贝扑克
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				GameEnd.cbCardCount[i] = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameEnd.cbCardData[i]);
			}
			CopyMemory(GameEnd.tagHistoryChart, m_tagHistoryChart, sizeof(m_tagHistoryChart));

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_MJ_FREE);
			//
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_FREE_START);
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_FREE_START);

			return true;
		}
	case GER_NETWORK_ERROR:		//网络错误
	case GER_USER_LEAVE:		//用户强退
		{
			m_pITableFrame->KillGameTimer(IDI_OUT_CARD_0);
			m_pITableFrame->KillGameTimer(IDI_OUT_CARD_1);
			m_pITableFrame->KillGameTimer(IDI_OUT_CARD_2);
			m_pITableFrame->KillGameTimer(IDI_OUT_CARD_3);
			m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

			//变量定义
			CMD_S_GameEnd& GameEnd = m_stGameEnd;
			ZeroMemory(&GameEnd, sizeof(GameEnd));
			GameEnd.wLeftUser = wChairID;

			//结束信息
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				GameEnd.cbCardCount[i] = m_GameLogic.SwitchToCardData(m_cbCardIndex[i], GameEnd.cbCardData[i]);
				m_ChiHuRight[i].GetRightData(&GameEnd.dwChiHuRight[i], MAX_RIGHT_COUNT );
			}

			SCOREEX lGangScore[GAME_PLAYER];
			ZeroMemory(lGangScore, sizeof(lGangScore));

			//并扣4倍点数
			m_bPlayStatus[wChairID] = false;
			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				//未胡牌玩家
				if (m_bPlayStatus[i]) 
				{
					m_lGameScore[i] += 4L * m_lCellScore;
					m_lGameScore[wChairID] -= 4L * m_lCellScore;
				}
				m_lGameScore[i] += lGangScore[i];
			}

			//算杠分
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				for (WORD j=0; j<m_GangScore[i].cbGangCount; j++)
				{
					for (WORD k=0; k<GAME_PLAYER; k++)
					{
						lGangScore[k] += m_GangScore[i].lScore[j][k];
					}
				}
			}

			for( WORD i = 0; i < GAME_PLAYER; i++ )
			{
				m_lGameScore[i] += lGangScore[i];
			}

			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(&ScoreInfoArray, sizeof(ScoreInfoArray));

			//统计积分
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				if (NULL == m_pITableFrame->GetTableUserItem(i)) 
				{
					continue;
				}

				//设置积分
				SCOREEX lGameTax = 0L;

				if (m_lGameScore[i] > 0L && ((m_pGameServiceOption->wServerType& GAME_GENRE_GOLD)==0 ))
				{
					lGameTax = m_pITableFrame->AccountRevenue( m_lGameScore[i]);
					m_lGameScore[i] -= lGameTax;
				}
				else
				{
					//防止负分
					IServerUserItem *pUserItem = m_pITableFrame->GetTableUserItem(i);
					if (GAME_GENRE_SCORE != m_pGameServiceOption->wServerType && -m_lGameScore[i] > pUserItem->GetUserScore())
					{
						m_lGameScore[i] = -pUserItem->GetUserScore();
					}
				}

				if (m_lGameScore[i] > 0L) 
				{
					ScoreInfoArray[i].ScoreType = ScoreType_Win;
				}
				else if (m_lGameScore[i] < 0L) 
				{
					ScoreInfoArray[i].ScoreType = ScoreType_Lose;
				}
				else 
				{
					ScoreInfoArray[i].ScoreType = ScoreType_Draw;
				}

				//历史积分
				m_HistoryScore.OnEventUserScore(i, m_lGameScore[i]);

				ScoreInfoArray[i].lScore   = m_lGameScore[i];
				ScoreInfoArray[i].lRevenue = m_lGameTax[i];
			}

			//写入积分
			m_pITableFrame->WriteTableScore(ScoreInfoArray, CountArray(ScoreInfoArray));

			CopyMemory(GameEnd.lGameScore, m_lGameScore, sizeof(m_lGameScore));
			CopyMemory(GameEnd.lGangScore, lGangScore, sizeof(lGangScore));
			CopyMemory(GameEnd.wWinOrder, m_wWinOrder, sizeof(m_wWinOrder));
			CopyMemory(GameEnd.wProvideUser, m_wProvider, sizeof(m_wProvider));
			
			CopyMemory(GameEnd.tagHistoryChart, m_tagHistoryChart, sizeof(m_tagHistoryChart));

			//发送结束信息
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GAME_END, &GameEnd, sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_MJ_FREE);
			//
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_FREE_START);
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_FREE_START);

			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);
	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	CTraceService::TraceString(_T("发送场景 1"), TraceLevel_Exception);
	switch (cbGameStatus)
	{
	case GS_MJ_FREE:	//空闲状态
		{
		CTraceService::TraceString(_T("发送场景 1 空闲状态"), TraceLevel_Exception);
			//私人房间配置
			if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) !=0 )
			{
				//BYTE *pGameRule = m_pITableFrame->GetGameRule();

				////pGameRule[1]人数配置  pGameRule[2]最大人数，麻将是固定4人玩
				//m_bHuanSanZhang = (pGameRule[3] == TRUE ? true : false);
				//m_bHuJiaoZhuanYi = (pGameRule[4] == TRUE ? true : false);
				//m_bZiMoAddTimes = (pGameRule[5] == TRUE ? true : false);
				//m_bTianDiHu = (pGameRule[6] == TRUE ? true : false);
				//m_bHaiDiLaoYue = (pGameRule[7] == TRUE ? true : false);
				//m_bMenQing = (pGameRule[8] == TRUE ? true : false);

				//设置人数
				//m_pITableFrame->SetTableChairCount(GAME_PLAYER);
			}

			//变量定义
			CMD_S_StatusFree StatusFree;
			memset(&StatusFree,0,sizeof(StatusFree));

			//构造数据
			StatusFree.wBankerUser=m_wBankerUser;
			StatusFree.lCellScore = m_pITableFrame->GetCellScore();
			CopyMemory(StatusFree.bTrustee,m_bTrustee,sizeof(m_bTrustee));
			//StatusFree.bCheatMode = CServerRule::IsAllowAvertDebugMode(m_pGameServiceOption->dwServerRule);

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusFree.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusFree.lCollectScore[i]=pHistoryScore->lCollectScore;
			}
			
			//时间信息
			StatusFree.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;		
			StatusFree.cbTimeOperateCard = m_pGameCustomRule->cbTimeOperateCard;
			StatusFree.cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
			StatusFree.cbTimeWaitEnd = m_pGameCustomRule->cbTimeWaitEnd;

			StatusFree.bHuanSanZhang = m_bHuanSanZhang;
			StatusFree.bHuJiaoZhuanYi = m_bHuJiaoZhuanYi;
			StatusFree.bZiMoAddTimes = m_bZiMoAddTimes;
			StatusFree.bTianDiHu = m_bTianDiHu;
			StatusFree.bHaiDiLaoYue = m_bHaiDiLaoYue;
			StatusFree.bMenQing = m_bMenQing;
			//StatusFree.cbPlayMode = m_pITableFrame->GetDataBaseMode();
			//房卡模式
			if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) !=0)
			{
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
			}

			StatusFree.wServerLevel = m_pGameServiceOption->wServerLevel;

			//金币场和金币房卡可以托管，积分房卡不托管
			if (((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0 ))
			{
				//重连取消托管定时器
				m_pITableFrame->KillGameTimer(IDI_OUT_CARD_0 + wChiarID);
				pIServerUserItem->SetTrusteeUser(false);
				m_bTrustee[wChiarID] = false;
			}

			//发送场景
			return m_pITableFrame->OnEventSendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_MJ_CHANGECARD:	//换三张
		{
			OnUserTrustee(wChiarID, false);
			//变量定义
			CMD_S_StatusChangeCard StatusChangeCard;
			memset(&StatusChangeCard, 0, sizeof(StatusChangeCard));

			//构造数据
			StatusChangeCard.wBankerUser = m_wBankerUser;
			StatusChangeCard.lCellScore = m_lCellScore;
			CopyMemory(StatusChangeCard.bTrustee, m_bTrustee, sizeof(m_bTrustee));

			//堆立信息
			StatusChangeCard.wHeapHand = m_wHeapHead;
			StatusChangeCard.wHeapTail = m_wHeapTail;
			CopyMemory(StatusChangeCard.cbHeapCardInfo, m_cbHeapCardInfo, sizeof(m_cbHeapCardInfo));
			
			CopyMemory(StatusChangeCard.bChangeCard, m_bChangeCard, sizeof(m_bChangeCard));
			CopyMemory(StatusChangeCard.cbChangeCardDataBefore, m_cbChangeCardDataBefore, sizeof(m_cbChangeCardDataBefore));

			if (m_bHuanSanZhang)
			{
				BYTE cbChangeCard[3] = { 0 };
				m_GameLogic.AutoHuangSanZhang(m_cbCardIndex[wChiarID], cbChangeCard);
				CopyMemory(StatusChangeCard.cbPriorChangeCardData, cbChangeCard, sizeof(cbChangeCard));
			}

			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				StatusChangeCard.cbCardCount[i] = m_GameLogic.GetCardCount(m_cbCardIndex[i]);
			}

			m_GameLogic.SwitchToCardData(m_cbCardIndex[wChiarID], StatusChangeCard.cbCardData);
			StatusChangeCard.cbSendCardData = ((m_cbSendCardData!=0) && (m_wProvideUser==wChiarID)) ? m_cbSendCardData : 0x00;

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusChangeCard.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusChangeCard.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//时间信息
			StatusChangeCard.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;		
			StatusChangeCard.cbTimeOperateCard = m_pGameCustomRule->cbTimeOperateCard;
			StatusChangeCard.cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
			StatusChangeCard.cbTimeWaitEnd = m_pGameCustomRule->cbTimeWaitEnd;
			
			StatusChangeCard.bHuanSanZhang = m_bHuanSanZhang;
			StatusChangeCard.bHuJiaoZhuanYi = m_bHuJiaoZhuanYi;
			StatusChangeCard.bZiMoAddTimes = m_bZiMoAddTimes;
			StatusChangeCard.bTianDiHu = m_bTianDiHu;
			StatusChangeCard.bHaiDiLaoYue = m_bHaiDiLaoYue;
			StatusChangeCard.bMenQing = m_bMenQing;
			//StatusChangeCard.cbPlayMode = m_pITableFrame->GetDataBaseMode();
			//房卡模式
			if (((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0))
			{
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
			}

			StatusChangeCard.wServerLevel = m_pGameServiceOption->wServerLevel;

			CTime ctCurTime = CTime::GetCurrentTime();
			CTimeSpan TimeSpan = ctCurTime - m_ctChangeTime;
			StatusChangeCard.cbTimeRemain = TIME_CHANGE_CARD + 10 - TimeSpan.GetSeconds();
			StatusChangeCard.cbTimeRemain = (StatusChangeCard.cbTimeRemain > TIME_CHANGE_CARD ? TIME_CHANGE_CARD : StatusChangeCard.cbTimeRemain);

			//金币场和金币房卡可以托管，积分房卡不托管
			if (((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0))
			{
				//重连取消托管定时器
				m_pITableFrame->KillGameTimer(IDI_OUT_CARD_0 + wChiarID);
				pIServerUserItem->SetTrusteeUser(false);
				m_bTrustee[wChiarID] = false;
			}

			//发送场景
			return m_pITableFrame->OnEventSendGameScene(pIServerUserItem, &StatusChangeCard, sizeof(StatusChangeCard));
		}
	case GS_MJ_CALL:	//缺门状态
		{
			OnUserTrustee(wChiarID, false);
			//变量定义
			CMD_S_StatusCall StatusCall;
			memset(&StatusCall, 0, sizeof(StatusCall));

			//构造数据
			StatusCall.wBankerUser = m_wBankerUser;
			StatusCall.lCellScore = m_lCellScore;
			CopyMemory(StatusCall.bTrustee, m_bTrustee, sizeof(m_bTrustee));

			//缺门信息
			CopyMemory(StatusCall.bCallCard, m_bCallCard, sizeof(m_bCallCard));
			CopyMemory(StatusCall.cbCallCard, m_cbCallCard, sizeof(m_cbCallCard));

			//堆立信息
			StatusCall.wHeapHand = m_wHeapHead;
			StatusCall.wHeapTail = m_wHeapTail;
			CopyMemory(StatusCall.cbHeapCardInfo, m_cbHeapCardInfo, sizeof(m_cbHeapCardInfo));

			//扑克数据
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				StatusCall.cbCardCount[i] = m_GameLogic.GetCardCount(m_cbCardIndex[i]);
			}

			m_GameLogic.SwitchToCardData(m_cbCardIndex[wChiarID], StatusCall.cbCardData);
			StatusCall.cbSendCardData = ((m_cbSendCardData!=0) && (m_wProvideUser==wChiarID)) ? m_cbSendCardData : 0x00;
			
			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusCall.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusCall.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//时间信息
			StatusCall.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;		
			StatusCall.cbTimeOperateCard = m_pGameCustomRule->cbTimeOperateCard;
			StatusCall.cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
			StatusCall.cbTimeWaitEnd = m_pGameCustomRule->cbTimeWaitEnd;
			
			StatusCall.bHuanSanZhang = m_bHuanSanZhang;
			StatusCall.bHuJiaoZhuanYi = m_bHuJiaoZhuanYi;
			StatusCall.bZiMoAddTimes = m_bZiMoAddTimes;
			StatusCall.bTianDiHu = m_bTianDiHu;
			StatusCall.bHaiDiLaoYue = m_bHaiDiLaoYue;
			StatusCall.bMenQing = m_bMenQing;
			//StatusCall.cbPlayMode = m_pITableFrame->GetDataBaseMode();
			//房卡模式
			if (((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0))
			{
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
			}

			StatusCall.wServerLevel = m_pGameServiceOption->wServerLevel;

			CTime ctCurTime = CTime::GetCurrentTime();
			CTimeSpan TimeSpan = ctCurTime - m_ctChangeTime;

			if (!m_bHuanSanZhang)
			{
				StatusCall.cbTimeRemain = TIME_CALL_CARD + 10 - TimeSpan.GetSeconds();
				StatusCall.cbTimeRemain = (StatusCall.cbTimeRemain > TIME_CALL_CARD ? TIME_CALL_CARD : StatusCall.cbTimeRemain);
			}
			else
			{
				StatusCall.cbTimeRemain = TIME_CALL_CARD + 2 - TimeSpan.GetSeconds();
			}

			//金币场和金币房卡可以托管，积分房卡不托管
			if (((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0))
			{
				//重连取消托管定时器
				m_pITableFrame->KillGameTimer(IDI_OUT_CARD_0 + wChiarID);
				pIServerUserItem->SetTrusteeUser(false);
				m_bTrustee[wChiarID] = false;
			}

			//发送场景
			return m_pITableFrame->OnEventSendGameScene(pIServerUserItem, &StatusCall, sizeof(StatusCall));
		}
	case GS_MJ_PLAY:	//游戏状态
		{
			OnUserTrustee(wChiarID, false);
			//变量定义
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//游戏变量
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.wCurrentUser=m_wCurrentUser;
			StatusPlay.lCellScore = m_lCellScore;
			CopyMemory(StatusPlay.bTrustee,m_bTrustee,sizeof(m_bTrustee));
			CopyMemory(StatusPlay.wWinOrder,m_wWinOrder,sizeof(m_wWinOrder));
			CopyMemory(StatusPlay.cbCallCard,m_cbCallCard,sizeof(m_cbCallCard));

			//状态变量
			StatusPlay.cbActionCard=m_cbProvideCard;
			StatusPlay.cbLeftCardCount=m_cbLeftCardCount;
			StatusPlay.cbActionMask=(m_bResponse[wChiarID]==false)?m_cbUserAction[wChiarID]:WIK_NULL;

			//历史记录
			StatusPlay.wOutCardUser=m_wOutCardUser;
			StatusPlay.cbOutCardData=m_cbOutCardData;
			CopyMemory(StatusPlay.cbDiscardCard,m_cbDiscardCard,sizeof(StatusPlay.cbDiscardCard));
			CopyMemory(StatusPlay.cbDiscardCount,m_cbDiscardCount,sizeof(StatusPlay.cbDiscardCount));

			//组合扑克
			CopyMemory(StatusPlay.WeaveItemArray,m_WeaveItemArray,sizeof(m_WeaveItemArray));
			CopyMemory(StatusPlay.cbWeaveCount,m_cbWeaveItemCount,sizeof(m_cbWeaveItemCount));

			//堆立信息
			StatusPlay.wHeapHand = m_wHeapHead;
			StatusPlay.wHeapTail = m_wHeapTail;
			CopyMemory(StatusPlay.cbHeapCardInfo,m_cbHeapCardInfo,sizeof(m_cbHeapCardInfo));

			BYTE cbCardIndex[GAME_PLAYER][MAX_INDEX];
			ZeroMemory(cbCardIndex, sizeof(cbCardIndex));
			CopyMemory(cbCardIndex, m_cbCardIndex, sizeof(m_cbCardIndex));

			//扑克数据
			for (WORD i = 0; i < GAME_PLAYER; i++)
			{
				StatusPlay.cbCardCount[i] = m_GameLogic.GetCardCount(m_cbCardIndex[i]);

				if (m_cbChiHuCardArray[i] != 0)
				{
					m_GameLogic.RemoveCard(cbCardIndex[i], m_cbChiHuCardArray[i]);
					StatusPlay.cbCardCount[i] -= 1;
				}
			}

			//扑克数据
			m_GameLogic.SwitchToCardData(cbCardIndex[wChiarID], StatusPlay.cbCardData);
			StatusPlay.cbSendCardData=((m_cbSendCardData!=0)&&(m_wProvideUser==wChiarID))?m_cbSendCardData:0x00;
			
			CopyMemory(StatusPlay.cbChiHuCardArray, m_cbChiHuCardArray, sizeof(m_cbChiHuCardArray));

			//算杠分
			for (WORD i = 0; i<GAME_PLAYER; i++)
			{
				for (WORD j = 0; j<m_GangScore[i].cbGangCount; j++)
				{
					for (WORD k = 0; k<GAME_PLAYER; k++)
					{
						StatusPlay.lGangScore[k] += m_GangScore[i].lScore[j][k];
					}
				}
			}

			CopyMemory(StatusPlay.lHuScore, m_lGameScore, sizeof(StatusPlay.lHuScore));

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				//变量定义
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);

				//设置变量
				StatusPlay.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusPlay.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//时间信息
			StatusPlay.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;		
			StatusPlay.cbTimeOperateCard = m_pGameCustomRule->cbTimeOperateCard;
			StatusPlay.cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
			StatusPlay.cbTimeWaitEnd = m_pGameCustomRule->cbTimeWaitEnd;

			CopyMemory(StatusPlay.tagHistoryChart, m_tagHistoryChart, sizeof(m_tagHistoryChart));

			StatusPlay.bHuanSanZhang = m_bHuanSanZhang;
			StatusPlay.bHuJiaoZhuanYi = m_bHuJiaoZhuanYi;
			StatusPlay.bZiMoAddTimes = m_bZiMoAddTimes;
			StatusPlay.bTianDiHu = m_bTianDiHu;
			StatusPlay.bHaiDiLaoYue = m_bHaiDiLaoYue;
			StatusPlay.bMenQing = m_bMenQing;
			//StatusPlay.cbPlayMode = m_pITableFrame->GetDataBaseMode();
			//房卡模式
			if (((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0))
			{
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
			}

			StatusPlay.wServerLevel = m_pGameServiceOption->wServerLevel;
			CopyMemory(StatusPlay.cbPlayerStatus, m_bPlayStatus, sizeof(StatusPlay.cbPlayerStatus));
			CTime ctCurTime = CTime::GetCurrentTime();
			CTimeSpan TimeSpan = ctCurTime - m_ctChangeTime;

			BYTE cbTempTime = (m_cbUserAction[wChiarID] != WIK_NULL ? StatusPlay.cbTimeOperateCard : StatusPlay.cbTimeOutCard);
			StatusPlay.cbTimeRemain = cbTempTime - TimeSpan.GetSeconds();

			//CString cs;
			//cs.Format(TEXT("cbTimeRemain = %d, cbTempTime = %d, TimeSpan = %d"), StatusPlay.cbTimeRemain, cbTempTime, TimeSpan.GetSeconds());
			//CTraceService::TraceString(cs,TraceLevel_Exception);

			//金币场和金币房卡可以托管，积分房卡不托管
			if (((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0))
			{
				//重连取消托管定时器
				m_pITableFrame->KillGameTimer(IDI_OUT_CARD_0 + wChiarID);
				pIServerUserItem->SetTrusteeUser(false);
				m_bTrustee[wChiarID] = false;
			}

			//听牌判断
			if (m_GameLogic.AnalyseTingCard(m_cbCardIndex[wChiarID], m_WeaveItemArray[wChiarID], m_cbWeaveItemCount[wChiarID], m_cbCallCard[wChiarID]))
			{
				SendTingData(wChiarID, m_cbCardIndex[wChiarID], m_WeaveItemArray[wChiarID], m_cbWeaveItemCount[wChiarID]);
			}

			//发送场景
			return m_pITableFrame->OnEventSendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	case GS_MJ_GAMEEND:
	{
						  CMD_S_StatusGameEnd Status;
						  ZeroMemory(&Status, sizeof(Status));
						  //
						  OnUserTrustee(wChiarID, false);

						  //游戏变量
						  Status.wBankerUser = m_wBankerUser;
						  Status.wCurrentUser = m_wCurrentUser;
						  Status.lCellScore = m_lCellScore;
						  CopyMemory(Status.bTrustee, m_bTrustee, sizeof(m_bTrustee));
						  CopyMemory(Status.wWinOrder, m_wWinOrder, sizeof(m_wWinOrder));
						  CopyMemory(Status.cbCallCard, m_cbCallCard, sizeof(m_cbCallCard));

						  //状态变量
						  Status.cbActionCard = m_cbProvideCard;
						  Status.cbLeftCardCount = m_cbLeftCardCount;
						  Status.cbActionMask = (m_bResponse[wChiarID] == false) ? m_cbUserAction[wChiarID] : WIK_NULL;

						  //历史记录
						  Status.wOutCardUser = m_wOutCardUser;
						  Status.cbOutCardData = m_cbOutCardData;
						  CopyMemory(Status.cbDiscardCard, m_cbDiscardCard, sizeof(Status.cbDiscardCard));
						  CopyMemory(Status.cbDiscardCount, m_cbDiscardCount, sizeof(Status.cbDiscardCount));

						  //组合扑克
						  CopyMemory(Status.WeaveItemArray, m_WeaveItemArray, sizeof(m_WeaveItemArray));
						  CopyMemory(Status.cbWeaveCount, m_cbWeaveItemCount, sizeof(m_cbWeaveItemCount));

						  //堆立信息
						  Status.wHeapHand = m_wHeapHead;
						  Status.wHeapTail = m_wHeapTail;
						  CopyMemory(Status.cbHeapCardInfo, m_cbHeapCardInfo, sizeof(m_cbHeapCardInfo));

						  BYTE cbCardIndex[GAME_PLAYER][MAX_INDEX];
						  ZeroMemory(cbCardIndex, sizeof(cbCardIndex));
						  CopyMemory(cbCardIndex, m_cbCardIndex, sizeof(m_cbCardIndex));

						  //扑克数据
						  for (WORD i = 0; i < GAME_PLAYER; i++)
						  {
							  Status.cbCardCount[i] = m_GameLogic.GetCardCount(m_cbCardIndex[i]);

							  if (m_cbChiHuCardArray[i] != 0)
							  {
								  m_GameLogic.RemoveCard(cbCardIndex[i], m_cbChiHuCardArray[i]);
								  Status.cbCardCount[i] -= 1;
							  }
						  }

						  //扑克数据
						  m_GameLogic.SwitchToCardData(cbCardIndex[wChiarID], Status.cbCardData);
						  Status.cbSendCardData = ((m_cbSendCardData != 0) && (m_wProvideUser == wChiarID)) ? m_cbSendCardData : 0x00;

						  CopyMemory(Status.cbChiHuCardArray, m_cbChiHuCardArray, sizeof(m_cbChiHuCardArray));

						  //算杠分
						  for (WORD i = 0; i<GAME_PLAYER; i++)
						  {
							  for (WORD j = 0; j<m_GangScore[i].cbGangCount; j++)
							  {
								  for (WORD k = 0; k<GAME_PLAYER; k++)
								  {
									  Status.lGangScore[k] += m_GangScore[i].lScore[j][k];
								  }
							  }
						  }

						  CopyMemory(Status.lHuScore, m_lGameScore, sizeof(Status.lHuScore));

						  //历史积分
						  for (WORD i = 0; i<GAME_PLAYER; i++)
						  {
							  //变量定义
							  tagHistoryScore * pHistoryScore = m_HistoryScore.GetHistoryScore(i);

							  //设置变量
							  Status.lTurnScore[i] = pHistoryScore->lTurnScore;
							  Status.lCollectScore[i] = pHistoryScore->lCollectScore;
						  }

						  //时间信息
						  Status.cbTimeOutCard = m_pGameCustomRule->cbTimeOutCard;
						  Status.cbTimeOperateCard = m_pGameCustomRule->cbTimeOperateCard;
						  Status.cbTimeStartGame = m_pGameCustomRule->cbTimeStartGame;
						  Status.cbTimeWaitEnd = m_pGameCustomRule->cbTimeWaitEnd;

						  CopyMemory(Status.tagHistoryChart, m_tagHistoryChart, sizeof(m_tagHistoryChart));

						  Status.bHuanSanZhang = m_bHuanSanZhang;
						  Status.bHuJiaoZhuanYi = m_bHuJiaoZhuanYi;
						  Status.bZiMoAddTimes = m_bZiMoAddTimes;
						  Status.bTianDiHu = m_bTianDiHu;
						  Status.bHaiDiLaoYue = m_bHaiDiLaoYue;
						  Status.bMenQing = m_bMenQing;
						  //Status.cbPlayMode = m_pITableFrame->GetDataBaseMode();
						  //房卡模式
						  if (((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0))
						  {
							  m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_RECORD, &m_stRecord, sizeof(m_stRecord));
						  }

						  Status.wServerLevel = m_pGameServiceOption->wServerLevel;
						  CopyMemory(Status.cbPlayerStatus, m_bPlayStatus, sizeof(Status.cbPlayerStatus));
						  CTime ctCurTime = CTime::GetCurrentTime();
						  CTimeSpan TimeSpan = ctCurTime - m_ctChangeTime;

						  BYTE cbTempTime = (m_cbUserAction[wChiarID] != WIK_NULL ? Status.cbTimeOperateCard : Status.cbTimeOutCard);
						  Status.cbTimeRemain = cbTempTime - TimeSpan.GetSeconds();

						  //CString cs;
						  //cs.Format(TEXT("cbTimeRemain = %d, cbTempTime = %d, TimeSpan = %d"), StatusPlay.cbTimeRemain, cbTempTime, TimeSpan.GetSeconds());
						  //CTraceService::TraceString(cs,TraceLevel_Exception);

						  //金币场和金币房卡可以托管，积分房卡不托管
						  if (((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0))
						  {
							  //重连取消托管定时器
							  m_pITableFrame->KillGameTimer(IDI_OUT_CARD_0 + wChiarID);
							  pIServerUserItem->SetTrusteeUser(false);
							  m_bTrustee[wChiarID] = false;
						  }

						  CopyMemory(&Status.stGameEnd, &m_stGameEnd, sizeof(Status.stGameEnd));
						  //发送场景
						  return m_pITableFrame->OnEventSendGameScene(pIServerUserItem, &Status, sizeof(Status));
	}
	}

	return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	if (wTimerID >= IDI_OUT_CARD_0 && wTimerID <= IDI_OUT_CARD_3)
	{
		m_pITableFrame->KillGameTimer(wTimerID);

		//金币场和金币房卡可以托管，积分房卡不托管
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD ) != 0 )
		{
			return true;
		}

		WORD wChairID = wTimerID - IDI_OUT_CARD_0;
		BYTE cbGameStatus = m_pITableFrame->GetGameStatus();

		//
		ASSERT(m_pITableFrame->GetTableUserItem(wChairID)->IsTrusteeUser());

		switch (cbGameStatus)
		{
		case GS_MJ_CHANGECARD:
		{
			if (m_cbChangeCardDataBefore[wChairID][0] == 0 && m_cbChangeCardDataBefore[wChairID][1] == 0 && m_cbChangeCardDataBefore[wChairID][2] == 0)
			{
				BYTE cbChangeCard[3] = { 0 };
				m_GameLogic.AutoHuangSanZhang(m_cbCardIndex[wChairID], cbChangeCard);
				OnUserChangeCard(wChairID, cbChangeCard);
			}

			return true;
		}
		case GS_MJ_CALL:
		{
			if (m_bCallCard[wChairID] == false)
			{
				BYTE cbWanCount = 0;
				BYTE cbTiaoCount = 0;
				BYTE cbTongCount = 0;
				for (int i = 0; i < 9; i++)
					cbWanCount += m_cbCardIndex[wChairID][i];
				for (int i = 9; i < 18; i++)
					cbTiaoCount += m_cbCardIndex[wChairID][i];
				for (int i = 18; i < 27; i++)
					cbTongCount += m_cbCardIndex[wChairID][i];
				BYTE cbCallCard = 0x01;
				BYTE cbMin = cbWanCount;
				if (cbTiaoCount < cbMin)
				{
					cbCallCard = 0x02;
					cbMin = cbTiaoCount;
				}
				if (cbTongCount<cbMin)
				{
					cbCallCard = 0x03;
					cbMin = cbTongCount;
				}

				OnUserCallCard(wChairID, cbCallCard);
			}
			return true;
		}
		case GS_MJ_PLAY:
		{
			if (wChairID == m_wCurrentUser)
			{
				BYTE cardindex = INVALID_BYTE;

				if (m_GameLogic.IsValidCard(m_cbSendCardData))
					cardindex = m_GameLogic.SwitchToCardIndex(m_cbSendCardData);
				else
				{
					for (int i = 0; i < MAX_INDEX; i++)
					{
						if (m_cbCardIndex[wChairID][i]>0)
						{
							cardindex = i;
							break;
						}
					}
				}

				//碰完别人家的牌然后托管，此时m_cbSendCardData不是发给托管玩家，然后会移除失败
				ASSERT(cardindex != INVALID_BYTE);

				if (m_cbCardIndex[wChairID][cardindex] == 0)
				{
					//如果找不到牌就从后往前取牌
					for (int i = MAX_INDEX - 1; i >= 0; i--)
					{
						if (m_cbCardIndex[wChairID][i] > 0)
						{
							cardindex = i;
							break;
						}
					}
				}

				//如果轮到自己有操作，先过然后再出牌
				if (m_cbUserAction[wChairID] != WIK_NULL)
				{
					OnUserOperateCard(wChairID, WIK_NULL, 0);
				}

				OnUserOutCard(wChairID, m_GameLogic.SwitchToCardData(cardindex));
			}
			else if (m_wCurrentUser == INVALID_CHAIR && m_cbUserAction[wChairID] != WIK_NULL)
			{
				OnUserOperateCard(wChairID, WIK_NULL, 0);
			}
			return true;
		}
		default:
			break;
		}

		return true;
	}
	else if (wTimerID == IDI_SO_OPERATE)
	{
		m_pITableFrame->KillGameTimer(wTimerID);

		//金币场和金币房卡可以托管，积分房卡不托管
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0 )
		{
			return true;
		}

		BYTE cbGameStatus = m_pITableFrame->GetGameStatus();

		switch (cbGameStatus)
		{
		case GS_MJ_CHANGECARD:
		{
			for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
				if (!pIServerUserItem)
				{
					continue;
				}

				if (m_cbChangeCardDataBefore[wChairID][0] == 0 && m_cbChangeCardDataBefore[wChairID][1] == 0 && m_cbChangeCardDataBefore[wChairID][2] == 0)
				{
					BYTE cbChangeCard[3] = { 0 };
					m_GameLogic.AutoHuangSanZhang(m_cbCardIndex[wChairID], cbChangeCard);
					OnUserChangeCard(wChairID, cbChangeCard);

					//m_bTrustee[wChairID] = true;
					//pIServerUserItem->SetTrusteeUser(true);
				}
			}

			return true;
		}
		case GS_MJ_CALL:
		{
			for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
				if (!pIServerUserItem)
				{
					continue;
				}

				if (m_bCallCard[wChairID] == false)
				{
					BYTE cbWanCount = 0;
					BYTE cbTiaoCount = 0;
					BYTE cbTongCount = 0;
					for (int i = 0; i < 9; i++)
						cbWanCount += m_cbCardIndex[wChairID][i];
					for (int i = 9; i < 18; i++)
						cbTiaoCount += m_cbCardIndex[wChairID][i];
					for (int i = 18; i < 27; i++)
						cbTongCount += m_cbCardIndex[wChairID][i];
					BYTE cbCallCard = 0x01;
					BYTE cbMin = cbWanCount;
					if (cbTiaoCount < cbMin)
					{
						cbCallCard = 0x02;
						cbMin = cbTiaoCount;
					}
					if (cbTongCount<cbMin)
					{
						cbCallCard = 0x03;
						cbMin = cbTongCount;
					}

					OnUserCallCard(wChairID, cbCallCard);

					//m_bTrustee[wChairID] = true;
					//pIServerUserItem->SetTrusteeUser(true);
				}
			}
						 
			return true;
		}
		case GS_MJ_PLAY:
		{
			if (m_wCurrentUser != INVALID_CHAIR)
			{
				IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(m_wCurrentUser);
				if (pIServerUserItem == NULL)
				{
					ASSERT(NULL);
					break;
				}

				BYTE cardindex = INVALID_BYTE;

				if (m_GameLogic.IsValidCard(m_cbSendCardData))
					cardindex = m_GameLogic.SwitchToCardIndex(m_cbSendCardData);
				else
				{
					for (int i = 0; i < MAX_INDEX; i++)
					{
						if (m_cbCardIndex[m_wCurrentUser][i]>0)
						{
							cardindex = i;
							break;
						}
					}
				}

				//碰完别人家的牌然后托管，此时m_cbSendCardData不是发给托管玩家，然后会移除失败
				ASSERT(cardindex != INVALID_BYTE);

				if (m_cbCardIndex[m_wCurrentUser][cardindex] == 0)
				{
					//如果找不到牌就从后往前取牌
					for (int i = MAX_INDEX - 1; i >= 0; i--)
					{
						if (m_cbCardIndex[m_wCurrentUser][i] > 0)
						{
							cardindex = i;
							break;
						}
					}
				}

				m_bTrustee[m_wCurrentUser] = true;
				pIServerUserItem->SetTrusteeUser(true);

				CMD_S_Trustee Trustee;
				ZeroMemory(&Trustee, sizeof(Trustee));
				Trustee.bTrustee = true;
				Trustee.wChairID = m_wCurrentUser;
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_TRUSTEE, &Trustee, sizeof(Trustee));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_TRUSTEE, &Trustee, sizeof(Trustee));

				//如果轮到自己有操作，先过然后再出牌
				if (m_cbUserAction[m_wCurrentUser] != WIK_NULL)
				{
					OnUserOperateCard(m_wCurrentUser, WIK_NULL, 0);
				}

				OnUserOutCard(m_wCurrentUser, m_GameLogic.SwitchToCardData(cardindex));
			}
			else
			{
				for (WORD wChairID = 0; wChairID < GAME_PLAYER; wChairID++)
				{
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
					if (pIServerUserItem == NULL)
					{
						continue;
					}

					if (m_wCurrentUser == INVALID_CHAIR && m_cbUserAction[wChairID] != WIK_NULL)
					{
						m_bTrustee[wChairID] = true;
						pIServerUserItem->SetTrusteeUser(true);

						CMD_S_Trustee Trustee;
						ZeroMemory(&Trustee, sizeof(Trustee));
						Trustee.bTrustee = true;
						Trustee.wChairID = wChairID;
						m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_TRUSTEE, &Trustee, sizeof(Trustee));
						m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_TRUSTEE, &Trustee, sizeof(Trustee));

						OnUserOperateCard(wChairID, WIK_NULL, 0);
					}
				}
			}

			return true;
		}
		default:
			break;
		}

		return true;
	}
	else if (wTimerID == IDI_GAMEEND)
	{

		m_pITableFrame->ConcludeGame(GS_MJ_FREE);
		//
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_FREE_START);
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_FREE_START);
	}

	return false;
}

//游戏消息处理
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID* pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	switch (wSubCmdID)
	{
	case SUB_C_CALL_CARD:		//缺门消息
		{
			//效验消息
			ASSERT (wDataSize == sizeof(CMD_C_CallCard));
			if (wDataSize != sizeof(CMD_C_CallCard)) 
			{
				return false;
			}

			//用户效验
			//if (pIServerUserItem->GetUserStatus() != US_PLAYING) 
			//{
			//	return false;
			//}

			//游戏状态效验
			if (m_pITableFrame->GetGameStatus() != GS_MJ_CALL) 
			{
				return false;
			}

			//消息处理
			CMD_C_CallCard * pCallCard = (CMD_C_CallCard *)pDataBuffer;

			return OnUserCallCard(pIServerUserItem->GetChairID(), pCallCard->cbCallCard);
		}
	case SUB_C_REQUEST_VIEWCHART:		//请求流水记录
		{
			CMD_S_ViewChart ViewChart;
			ZeroMemory(&ViewChart, sizeof(ViewChart));
			
			CopyMemory(ViewChart.tagHistoryChart, m_tagHistoryChart, sizeof(m_tagHistoryChart));

			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_VIEW_CHART, &ViewChart, sizeof(ViewChart));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_VIEW_CHART, &ViewChart, sizeof(ViewChart));

			return true;
		}
	case SUB_C_OUT_CARD:		//出牌消息
		{
			//效验消息
			ASSERT (wDataSize == sizeof(CMD_C_OutCard));
			if (wDataSize != sizeof(CMD_C_OutCard)) 
			{
				return false;
			}

			//用户效验
			//if (pIServerUserItem->GetUserStatus() != US_PLAYING) 
			//{
			//	return false;
			//}

			//游戏状态效验
			if (m_pITableFrame->GetGameStatus() != GS_MJ_PLAY) 
			{
				//CTraceService::TraceString(TEXT("GameStatusINVALID, client = 1"), TraceLevel_Exception);
                //m_pITableFrame->SendGameLogData(TEXT("GameStatusINVALID, client = 1"));
				return false;
			}

			//消息处理
			CMD_C_OutCard * pOutCard=(CMD_C_OutCard *)pDataBuffer;
			return OnUserOutCard(pIServerUserItem->GetChairID(), pOutCard->cbCardData, true);
		}
	case SUB_C_OPERATE_CARD:	//操作消息
		{
			//效验消息
			ASSERT (wDataSize == sizeof(CMD_C_OperateCard));
			if (wDataSize != sizeof(CMD_C_OperateCard)) 
			{
				return false;
			}

			//用户效验
			//if (pIServerUserItem->GetUserStatus() != US_PLAYING) 
			//{
			//	return false;
			//}
			
			//游戏状态效验
			if (m_pITableFrame->GetGameStatus() != GS_MJ_PLAY) 
			{
				return false;
			}

			//消息处理
			CMD_C_OperateCard * pOperateCard = (CMD_C_OperateCard *)pDataBuffer;
			return OnUserOperateCard(pIServerUserItem->GetChairID(), pOperateCard->cbOperateCode, pOperateCard->cbOperateCard);
		}
	case SUB_C_TRUSTEE:			//用户托管
		{
			//效验消息
			ASSERT (wDataSize == sizeof(CMD_C_Trustee));
			if (wDataSize != sizeof(CMD_C_Trustee)) 
			{
				return false;
			}

			//用户效验
			//if (pIServerUserItem->GetUserStatus() != US_PLAYING) 
			//{
			//	return false;
			//}

			//游戏状态效验
			if (m_pITableFrame->GetGameStatus() == GS_MJ_FREE) 
			{
				return true;
			}
			
			CMD_C_Trustee *pTrustee = (CMD_C_Trustee *)pDataBuffer;

			return OnUserTrustee(pIServerUserItem->GetChairID(), pTrustee->bTrustee);
		}
	case SUB_C_CHANGE_CARD:	//换三张
		{
			//效验消息
			ASSERT (wDataSize == sizeof(CMD_C_ChangeCard));
			if (wDataSize != sizeof(CMD_C_ChangeCard)) 
			{
				return false;
			}

			//用户效验
			//BYTE cbstatus = pIServerUserItem->GetUserStatus();
			//if (pIServerUserItem->GetUserStatus() != US_PLAYING) 
			//{
			//	return true;
			//}

			//游戏状态效验
			if (m_pITableFrame->GetGameStatus() != GS_MJ_CHANGECARD) 
			{
				return true;
			}

			//消息处理
			CMD_C_ChangeCard * pChangeCard = (CMD_C_ChangeCard *)pDataBuffer;
			return OnUserChangeCard(pIServerUserItem->GetChairID(), pChangeCard->cbChangeCardData, true);
		}
	case SUB_C_ASK_HANDCARD:
		{
			CMD_S_Askhandcard Askhandcard;
			ZeroMemory(&Askhandcard, sizeof(Askhandcard));

			m_GameLogic.SwitchToCardData(m_cbCardIndex[pIServerUserItem->GetChairID()], Askhandcard.cbCardData);
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_ASK_HANDCARD, &Askhandcard, sizeof(Askhandcard));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_ASK_HANDCARD, &Askhandcard, sizeof(Askhandcard));

			return true;
		}
	case SUB_C_REQUEST_RCRecord:
		{
			if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) == 0)
			{
				return true;
			}

			CMD_S_RoomCardRecord RoomCardRecord;
			ZeroMemory(&RoomCardRecord, sizeof(RoomCardRecord));

			/*if (m_pITableFrame->GetDataBaseMode()==0)
				CopyMemory(&RoomCardRecord, &m_RoomCardRecord, sizeof(m_RoomCardRecord));
			else
			{
				RoomCardRecord.nCount = m_vecRecord[pIServerUserItem->GetChairID()].size();
				for (int i = 0; i < RoomCardRecord.nCount; i++)
				{
					RoomCardRecord.lDetailScore[pIServerUserItem->GetChairID()][i] = m_vecRecord[pIServerUserItem->GetChairID()][i];
				}
			}*/
			m_pITableFrame->SendTableData(pIServerUserItem->GetChairID(), SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));
			m_pITableFrame->SendLookonData(pIServerUserItem->GetChairID(), SUB_S_ROOMCARD_RECORD, &RoomCardRecord, sizeof(RoomCardRecord));

			return true;
		}
	case SUB_C_REQUEST_HU_DATA:
		{
			//效验消息
			ASSERT(wDataSize == 0);
			if (wDataSize != 0)
			{
				return false;
			}

			WORD wChairID = pIServerUserItem->GetChairID();
			CMD_S_HU_DATA huData;
			ZeroMemory(&huData, sizeof(huData));

			//听牌判断
			if (!m_GameLogic.AnalyseTingCard(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID], m_cbCallCard[wChairID]))
			{
				//发送数据
				m_pITableFrame->SendTableData(wChairID, SUB_S_HU_DATA, &huData, sizeof(huData));
			}
			else
			{
				BYTE cbCount = 0;
				CChiHuRight chr;
				for (BYTE j = 0; j < MAX_INDEX; j++)
				{
					BYTE cbCurrentCard = m_GameLogic.SwitchToCardData(j);
					if (!m_GameLogic.IsValidCard(cbCurrentCard))
					{
						continue;
					}

					chr.SetEmpty();
					if (WIK_CHI_HU == m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wChairID], m_WeaveItemArray[wChairID], m_cbWeaveItemCount[wChairID], cbCurrentCard, chr, m_cbCallCard[wChairID]))
					{
						huData.cbHuCardData[huData.cbHuCardCount] = cbCurrentCard;
						huData.cbHuCardRemainingCount[huData.cbHuCardCount] = GetRemainingCount(wChairID, cbCurrentCard);
						huData.cbHuFan[huData.cbHuCardCount] = m_GameLogic.GetUserHuFan(chr);
						ASSERT(huData.cbHuFan[huData.cbHuCardCount] != INVALID_BYTE);

						huData.cbHuCardCount++;
					}
				}

				//发送数据
				m_pITableFrame->SendTableData(wChairID, SUB_S_HU_DATA, &huData, sizeof(huData));
			}

			return true;
		}
#ifdef CARD_CONFIG
	case SUB_C_CARD_CONFIG:
		{
			//效验数据
			ASSERT (wDataSize == sizeof(CMD_C_CardConfig));
			if (wDataSize != sizeof(CMD_C_CardConfig))
			{
				return true;
			}

			//消息处理
			CMD_C_CardConfig *pCardConfig = (CMD_C_CardConfig *)pDataBuffer;

			CopyMemory(m_cbconfigCard, pCardConfig->cbConfigCard, sizeof(m_cbconfigCard));

			return true;
		}
#endif
	}

	return false;
}

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}

bool CTableFrameSink::OnActionUserOffLine(IServerUserItem * pIServerUserItem)
{

	//金币场和金币房卡可以托管，积分房卡不托管
	if (((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0 ))
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

	return true;
}

//用户坐下
bool CTableFrameSink::OnActionUserSitDown(IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//m_vecRecord[wChairID].clear();
	return true;
}

//用户起来
bool CTableFrameSink::OnActionUserStandUp(IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//庄家设置
	if (bLookonUser==false)
	{
		/*m_bTrustee[wChairID]=false;
		CMD_S_Trustee Trustee;
		Trustee.bTrustee=false;
		Trustee.wChairID = wChairID;
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));*/
	}

	m_wFirstWinUser=INVALID_CHAIR;

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
		ZeroMemory(&m_stRecord,sizeof(m_stRecord));
		ZeroMemory(&m_RoomCardRecord, sizeof(m_RoomCardRecord));
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

//用户选缺
bool CTableFrameSink::OnUserCallCard(WORD wChairID, BYTE cbCallCard)
{
	//设置缺门
	m_bCallCard[wChairID] = true;
	if (cbCallCard == 0x01) m_cbCallCard[wChairID] = 0x01;
	else if (cbCallCard == 0x02) m_cbCallCard[wChairID] = 0x11;
	else if (cbCallCard == 0x03) m_cbCallCard[wChairID] = 0x21;

	//是否完成
	BYTE cbCallCount = 0;
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		if (m_bCallCard[i] == true)
			cbCallCount++;
	}

	//发送消息
	CMD_S_CallCard CallCard;
	CallCard.wCallUser = wChairID;
	CallCard.cbCallCard = m_cbCallCard[wChairID];

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CALL_CARD, &CallCard, sizeof(CallCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CALL_CARD, &CallCard, sizeof(CallCard));

	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_CALL_CARD, &CallCard);
	}

	//开始判断
	if ((cbCallCount == GAME_PLAYER))
	{
		//设置用户
		m_wCurrentUser=m_wBankerUser;
		
		bool bFirstRecord = true;
		for (WORD i=0;i<GAME_PLAYER;i++)
		{
			IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
			if (!pIServerUserItem)
			{
				continue;
			}
			
			//发送消息
			CMD_S_BankerInfo BankerInfo;
			ZeroMemory(&BankerInfo, sizeof(BankerInfo));
			BankerInfo.wBankerUser = m_wBankerUser;
			BankerInfo.wCurrentUser = m_wCurrentUser;
			CopyMemory(BankerInfo.cbCallCard, m_cbCallCard, sizeof(m_cbCallCard));

			//庄家判断
			if (i == m_wBankerUser)
			{
				//杠牌判断
				tagGangCardResult GangCardResult;
				m_cbUserAction[i] |= m_GameLogic.AnalyseGangCard(m_cbCardIndex[i], NULL, 0, GangCardResult);

				//胡牌判断
				CChiHuRight chr;
				if (m_bHuanSanZhang)
				{
					m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbChangeCardDataAfter[i][0])]--;
					m_cbUserAction[i] |= m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i], NULL ,0, m_cbChangeCardDataAfter[i][0], chr, m_cbCallCard[i]);
					m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbChangeCardDataAfter[i][0])]++;
				}
				else
				{
					m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]--;
					m_cbUserAction[i]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],NULL,0,m_cbSendCardData,chr, m_cbCallCard[i]);
					m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;
				}
			}

			BankerInfo.cbUserAction = m_cbUserAction[i];

			//发送消息
			m_pITableFrame->SendTableData(i, SUB_S_BANKER_INFO, &BankerInfo, sizeof(BankerInfo));
			m_pITableFrame->SendLookonData(i, SUB_S_BANKER_INFO, &BankerInfo, sizeof(BankerInfo));

			if (m_pGameVideo)
			{
				m_pGameVideo->AddVideoData(SUB_S_BANKER_INFO, &BankerInfo, bFirstRecord);

				if (bFirstRecord == true)
				{
					bFirstRecord = false;
				}
			}
		}

		//设置状态
		m_ctChangeTime = CTime::GetCurrentTime();
		m_pITableFrame->SetGameStatus(GS_MJ_PLAY);

		//设置超时定时器
		m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

		BYTE cbIDITime = m_cbUserAction[m_wBankerUser] == WIK_NULL ? m_pGameCustomRule->cbTimeOutCard + 1 : m_pGameCustomRule->cbTimeOperateCard + 1;
		m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, cbIDITime * 1000, 1, 0);

		if (m_wCurrentUser != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentUser) && m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsTrusteeUser())
		{
			m_pITableFrame->SetGameTimer(IDI_OUT_CARD_0 + m_wCurrentUser, m_cbTrusteeDelayTime * 1000, 1, 0);
		}

		//听牌判断
		if (m_GameLogic.AnalyseTingCard(m_cbCardIndex[m_wBankerUser], 0, 0, m_cbCallCard[m_wBankerUser]))
		{
			SendTingData(m_wBankerUser, m_cbCardIndex[m_wBankerUser], 0, 0);
		}
	}

	return true;
}

//用户出牌
bool CTableFrameSink::OnUserOutCard(WORD wChairID, BYTE cbCardData, bool bClient)
{
	//错误断言
	ASSERT(wChairID==m_wCurrentUser);
	ASSERT(m_GameLogic.IsValidCard(cbCardData)==true);

	//效验参数
	if (wChairID != m_wCurrentUser)
	{
		CString strTrace;
        strTrace.Format(TEXT("table id is %d, wChairID != m_wCurrentUser, client = %d"), m_pITableFrame->GetTableID(), bClient);
		//CTraceService::TraceString(strTrace, TraceLevel_Exception);

        //m_pITableFrame->SendGameLogData(strTrace);
		return false;
	}
	if (m_GameLogic.IsValidCard(cbCardData) == false)
	{
		CString strTrace;
        strTrace.Format(TEXT("table id is %d, cbCardData = %d, client = %d"), m_pITableFrame->GetTableID(), cbCardData, bClient);
		//CTraceService::TraceString(strTrace, TraceLevel_Exception);
        //m_pITableFrame->SendGameLogData(strTrace);
		return false;
	}

	//删除扑克
	if (m_GameLogic.RemoveCard(m_cbCardIndex[wChairID],cbCardData)==false)
	{
		ASSERT(FALSE);

		CString strTrace;
        strTrace.Format(TEXT("table id is %d, RemoveCardinvalid, cbCardData = %d, client = %d"), m_pITableFrame->GetTableID(), cbCardData, bClient);
		//CTraceService::TraceString(strTrace, TraceLevel_Exception);
        //m_pITableFrame->SendGameLogData(strTrace);
		return false;
	}

	//设置变量
	m_bSendStatus=true;
	if( m_bGangStatus )
	{
		m_bGangStatus = false;
		m_bGangOutStatus = true;
		m_wProvideGang = INVALID_CHAIR;
	}
	m_cbUserAction[wChairID]=WIK_NULL;
	m_cbPerformAction[wChairID]=WIK_NULL;

	//出牌记录
	m_cbOutCardCount++;
	m_wOutCardUser=wChairID;
	m_cbOutCardData=cbCardData;

	//构造数据
	CMD_S_OutCard OutCard;
	OutCard.wOutCardUser=wChairID;
	OutCard.cbOutCardData=cbCardData;
	OutCard.bSysOut = !bClient;

	//发送消息
	m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OUT_CARD,&OutCard,sizeof(OutCard));

	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_OUT_CARD, &OutCard);
	}

	m_wProvideUser=wChairID;
	m_cbProvideCard=cbCardData;

	//用户切换
	m_wCurrentUser=(wChairID+GAME_PLAYER+1)%GAME_PLAYER;
	while( !m_bPlayStatus[m_wCurrentUser] ) 
		m_wCurrentUser=(m_wCurrentUser+GAME_PLAYER+1)%GAME_PLAYER;

	//响应判断
	bool bAroseAction=EstimateUserRespond(wChairID,cbCardData,EstimatKind_OutCard);

	//派发扑克
	if (bAroseAction==false) DispatchCardData(m_wCurrentUser);

	return true;
}

//用户操作
bool CTableFrameSink::OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard)
{
	//效验用户
	if ((wChairID != m_wCurrentUser) && (m_wCurrentUser != INVALID_CHAIR)) 
	{
		return true;
	}

	//被动动作
	if (m_wCurrentUser == INVALID_CHAIR)
	{
		//效验状态
		if (m_bResponse[wChairID] == true) 
		{
			return true;
		}
		if ((cbOperateCode != WIK_NULL) && ((m_cbUserAction[wChairID] & cbOperateCode) == 0))
		{
			return true;
		}

		//变量定义
		WORD wTargetUser = wChairID;
		BYTE cbTargetAction = cbOperateCode;

		//设置变量
		m_bResponse[wChairID] = true;
		m_cbPerformAction[wChairID] = cbOperateCode;
		m_cbOperateCard[wChairID] = ((cbOperateCard == 0) ? m_cbProvideCard : cbOperateCard);

		//即一轮内，如果同一张牌上家打出不胡，那么也不能胡下家
		if ((cbTargetAction == WIK_NULL) && (WIK_CHI_HU & m_cbUserAction[wTargetUser]) && (m_wProvideUser != wTargetUser))
		{
			m_bEnjoinChiHu[wTargetUser] = true;
		}

		//执行判断
		for (WORD i=0; i<GAME_PLAYER; i++)
		{
			if (!m_bPlayStatus[i])
			{
				continue;
			}

			//获取动作
			BYTE cbUserAction = ((m_bResponse[i] == false) ? m_cbUserAction[i] : m_cbPerformAction[i]);

			//优先级别
			BYTE cbUserActionRank = m_GameLogic.GetUserActionRank(cbUserAction);
			BYTE cbTargetActionRank = m_GameLogic.GetUserActionRank(cbTargetAction);

			//动作判断
			if (cbUserActionRank>cbTargetActionRank)
			{
				wTargetUser = i;
				cbTargetAction = cbUserAction;
			}
		}

		if (m_bResponse[wTargetUser] == false) 
		{
			return true;
		}

		//吃胡等待
		if (cbTargetAction == WIK_CHI_HU)
		{
			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				if ((m_bResponse[i] == false) && (m_cbUserAction[i] & WIK_CHI_HU))
				{
					return true;
				}
			}
		}

		//放弃操作
		if (cbTargetAction==WIK_NULL)
		{
			//用户状态
			ZeroMemory(m_bResponse, sizeof(m_bResponse));
			ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
			ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
			ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));

			//发送扑克
			DispatchCardData(m_wResumeUser);

			return true;
		}

		//变量定义
		BYTE cbTargetCard = m_cbOperateCard[wTargetUser];

		//出牌变量
		m_cbOutCardData = 0;
		m_bSendStatus = true;
		m_wOutCardUser = INVALID_CHAIR;
		m_bEnjoinChiHu[wTargetUser] = false;

		//胡牌操作
		if (cbTargetAction == WIK_CHI_HU)
		{
			//结束信息
			m_cbChiHuCard = cbTargetCard;
			
			m_cbChiHuCardArray[wTargetUser] = cbTargetCard;
			
			//吃胡得分
			SCOREEX lChiHuScore[GAME_PLAYER];
			ZeroMemory(lChiHuScore, sizeof(lChiHuScore));
			
			//番数
			WORD wFanShu[GAME_PLAYER];
			ZeroMemory(wFanShu, sizeof(wFanShu));

			for (WORD i=(m_wProvideUser+GAME_PLAYER-1)%GAME_PLAYER; i!=m_wProvideUser; i = (i+GAME_PLAYER-1)%GAME_PLAYER)
			{
				//过虑判断
				if ((m_cbPerformAction[i] & WIK_CHI_HU) == 0 || !(m_bPlayStatus[i]))
				{
					continue;
				}

				//胡牌判断
				BYTE cbWeaveItemCount = m_cbWeaveItemCount[i];
				tagWeaveItem * pWeaveItem = m_WeaveItemArray[i];
				m_dwChiHuKind[i] = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i], pWeaveItem, cbWeaveItemCount, m_cbChiHuCard, m_ChiHuRight[i], m_cbCallCard[i]);

				//插入扑克
				if (m_dwChiHuKind[i] != WIK_NULL) 
				{
					m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbChiHuCard)]++;
					lChiHuScore[i] = ProcessChiHuUser(i, wFanShu[i]);
				}
				//
				lChiHuScore[m_wProvideUser] -= lChiHuScore[i];
			}
			//赔付处理
			ChangeUserPaidScore(lChiHuScore);

			//点炮玩家
			m_stRecord.cbFangPao[m_wProvideUser]++;

			//插入流水清单
			//点炮玩家
			ASSERT (m_cbHistoryChartCount[m_wProvideUser] < MAX_CHART_COUNT);
			WORD wProvideChartIndex = m_cbHistoryChartCount[m_wProvideUser]++;
			WORD wProvideFanShu = 0;
			SCOREEX lProvideChiHuScore = 0;

			for (WORD i=0; i<GAME_PLAYER; i++)
			{
				if (i == m_wProvideUser || D_Equals(lChiHuScore[i], 0))
				{
					continue;
				}
				
				//吃胡玩家，允许一炮多响
				if (m_dwChiHuKind[i] != WIK_NULL)
				{
					//被点炮玩家
					ASSERT (m_cbHistoryChartCount[i] < MAX_CHART_COUNT);
					WORD wChartIndex = m_cbHistoryChartCount[i]++;
					m_tagHistoryChart[i][wChartIndex].charttype = BEIDIANPAO_TYPE;
					m_tagHistoryChart[i][wChartIndex].lTimes = wFanShu[i];
					m_tagHistoryChart[i][wChartIndex].lScore = lChiHuScore[i];
					m_tagHistoryChart[i][wChartIndex].bEffectChairID[m_wProvideUser] = true;
					
					//点炮玩家
					wProvideFanShu += wFanShu[i];
					lProvideChiHuScore += lChiHuScore[i];
					m_tagHistoryChart[m_wProvideUser][wProvideChartIndex].bEffectChairID[i] = true;
				}
			}
			
			//点炮玩家
			m_tagHistoryChart[m_wProvideUser][wProvideChartIndex].charttype = DIANPAO_TYPE;
			m_tagHistoryChart[m_wProvideUser][wProvideChartIndex].lTimes = wProvideFanShu;
			m_tagHistoryChart[m_wProvideUser][wProvideChartIndex].lScore = -lProvideChiHuScore;
			lChiHuScore[m_wProvideUser] = -lProvideChiHuScore;

			//发送吃胡
			for (WORD i=(m_wProvideUser+GAME_PLAYER-1)%GAME_PLAYER; i!=m_wProvideUser; i = (i+GAME_PLAYER-1)%GAME_PLAYER)
			{
				//过虑判断
				if (!(m_bPlayStatus[i]))
				{
					continue;
				}

				//插入扑克
				if (m_dwChiHuKind[i] != WIK_NULL) 
				{
					CMD_S_ChiHu ChiHu;
					ZeroMemory(&ChiHu, sizeof(ChiHu));

					ChiHu.wChiHuUser = i;
					ChiHu.wProviderUser = m_wProvideUser;
					ChiHu.lGameScore = m_lGameScore[i];
					ChiHu.cbCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[i]);
					ChiHu.cbChiHuCard = m_cbProvideCard;
					m_ChiHuRight[wChairID].GetRightData(&(ChiHu.dwChiHuRight), MAX_RIGHT_COUNT);
					ChiHu.cbWinOrder = m_cbWinCount;
					CopyMemory(ChiHu.tagHistoryChart, m_tagHistoryChart, sizeof(m_tagHistoryChart));
					SCOREEX lTmpChiHuScore[GAME_PLAYER] = {0};
					lTmpChiHuScore[i] = lChiHuScore[i];
					lTmpChiHuScore[m_wProvideUser] = -lChiHuScore[i];
					CopyMemory(ChiHu.lHuScore, lTmpChiHuScore, sizeof(lChiHuScore));

					//算杠分					
					for (WORD j = 0; j < m_cbHistoryChartCount[wChairID]; j++)
					{
						if (m_tagHistoryChart[wChairID][j].charttype == HUJIAOZHUANYI_TYPE && D_GreaterThen(m_tagHistoryChart[wChairID][j].lScore, 0))
						{
							ChiHu.lGangScore[wChairID] += m_tagHistoryChart[wChairID][j].lScore;
							ChiHu.lGangScore[m_wProvideUser] -= m_tagHistoryChart[wChairID][j].lScore;
						}
					}

					m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CHI_HU, &ChiHu, sizeof(ChiHu));
					m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CHI_HU, &ChiHu, sizeof(ChiHu));
					//
					UpdateUserStatus();
					LOG_TABLE(L"吃胡操作分数:", FormatScoreString(lChiHuScore));
					m_bWaitWriteScore[i] = true;

					Video_ChiHu VideoChiHu;
					ZeroMemory(&VideoChiHu, sizeof(VideoChiHu));
					VideoChiHu.wChiHuUser = ChiHu.wChiHuUser;
					VideoChiHu.wProviderUser = ChiHu.wProviderUser;
					VideoChiHu.lGameScore = ChiHu.lGameScore;
					VideoChiHu.cbCardCount = ChiHu.cbCardCount;
					VideoChiHu.cbChiHuCard = ChiHu.cbChiHuCard;
					VideoChiHu.cbWinOrder = ChiHu.cbWinOrder;

					if (m_pGameVideo)
					{
						m_pGameVideo->AddVideoData(SUB_S_CHI_HU, &VideoChiHu);
					}
				}
			}

			for (WORD i=(m_wProvideUser+GAME_PLAYER-1)%GAME_PLAYER; i!=m_wProvideUser; i = (i+GAME_PLAYER-1)%GAME_PLAYER)
			{
				//过虑判断
				if ((m_cbPerformAction[i] & WIK_CHI_HU) == 0 || !(m_bPlayStatus[i]))
				{
					continue;
				}

				//插入扑克
				if (m_dwChiHuKind[i] != WIK_NULL) 
				{
					m_bPlayStatus[i] = false;
					IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
					if (pIServerUserItem != NULL)
					{
						//pIServerUserItem->SetUserStatus(US_SIT, m_pITableFrame->GetTableID(), i);
					}
				}
			}

			//判断结束
			BYTE cbPlayCount = 0;
			for (WORD i=0; i<GAME_PLAYER; i++ )
			{
				if (m_bPlayStatus[i])
				{
					cbPlayCount++;
				}
			}

			if (cbPlayCount < 2) 
			{
				OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
			}
			else
			{
				//用户状态
				ZeroMemory(m_bResponse, sizeof(m_bResponse));
				ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
				ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
				ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));

				//发送扑克
				m_wCurrentUser = (wTargetUser + GAME_PLAYER + 1) % GAME_PLAYER;
				while (!m_bPlayStatus[m_wCurrentUser])
					m_wCurrentUser = (m_wCurrentUser + GAME_PLAYER + 1) % GAME_PLAYER;
				DispatchCardData(m_wCurrentUser);
			}

			return true;
		}

		//用户状态
		ZeroMemory(m_bResponse, sizeof(m_bResponse));
		ZeroMemory(m_cbUserAction, sizeof(m_cbUserAction));
		ZeroMemory(m_cbOperateCard, sizeof(m_cbOperateCard));
		ZeroMemory(m_cbPerformAction, sizeof(m_cbPerformAction));

		//组合扑克
		ASSERT (m_cbWeaveItemCount[wTargetUser] < 4);
		WORD wIndex = m_cbWeaveItemCount[wTargetUser]++;
		m_WeaveItemArray[wTargetUser][wIndex].cbPublicCard = TRUE;
		m_WeaveItemArray[wTargetUser][wIndex].cbCenterCard = cbTargetCard;
		m_WeaveItemArray[wTargetUser][wIndex].cbWeaveKind = cbTargetAction;
		m_WeaveItemArray[wTargetUser][wIndex].wProvideUser = (m_wProvideUser == INVALID_CHAIR) ? wTargetUser : m_wProvideUser;

		//删除扑克
		switch (cbTargetAction)
		{
		case WIK_PENG:		//碰牌操作
			{
				//删除扑克
				BYTE cbRemoveCard[] = {cbTargetCard, cbTargetCard};

				//在RELEASE模式下，ASSERT不会对参数里的表达式进行计算，VERIFY会计算里面的表达式（如果是函数，将会调用该函数）
				VERIFY (m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], cbRemoveCard,2));

				BYTE cbIDITime = m_pGameCustomRule->cbTimeOutCard + 1;
				m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, cbIDITime * 1000, 1, 0);

				break;
			}
		case WIK_GANG:		//杠牌操作
			{
				//删除扑克,被动动作只存在放杠
				BYTE cbRemoveCard[] = {cbTargetCard, cbTargetCard, cbTargetCard};
				VERIFY (m_GameLogic.RemoveCard(m_cbCardIndex[wTargetUser], cbRemoveCard, CountArray(cbRemoveCard)) );

				break;
			}
		default:
			ASSERT (FALSE);
			return false;
		}

		SCOREEX lCurGangScore[GAME_PLAYER];
		ZeroMemory(lCurGangScore, sizeof(lCurGangScore));

		//明杠 (刮风流水清单)
		if (cbTargetAction == WIK_GANG)
		{
			lCurGangScore[wTargetUser] = m_lCellScore * 2;
			lCurGangScore[m_wProvideUser] = -m_lCellScore * 2;
			//赔付处理
			ChangeUserPaidScore(lCurGangScore);

			//刮风玩家
			ASSERT (m_cbHistoryChartCount[wTargetUser] < MAX_CHART_COUNT);
			WORD wTargetChartIndex = m_cbHistoryChartCount[wTargetUser]++;
			m_tagHistoryChart[wTargetUser][wTargetChartIndex].charttype = GUAFENG_TYPE;
			m_tagHistoryChart[wTargetUser][wTargetChartIndex].lTimes = 2;
			m_tagHistoryChart[wTargetUser][wTargetChartIndex].lScore = lCurGangScore[wTargetUser];
			m_tagHistoryChart[wTargetUser][wTargetChartIndex].bEffectChairID[m_wProvideUser] = true;

			//被刮风玩家
			ASSERT (m_cbHistoryChartCount[m_wProvideUser] < MAX_CHART_COUNT);
			WORD wProvideChartIndex = m_cbHistoryChartCount[m_wProvideUser]++;
			m_tagHistoryChart[m_wProvideUser][wProvideChartIndex].charttype = BEIGUAFENG_TYPE;
			m_tagHistoryChart[m_wProvideUser][wProvideChartIndex].lTimes = 2;
			m_tagHistoryChart[m_wProvideUser][wProvideChartIndex].lScore = lCurGangScore[m_wProvideUser];
			m_tagHistoryChart[m_wProvideUser][wProvideChartIndex].bEffectChairID[wTargetUser] = true;
		}

		//构造结果
		CMD_S_OperateResult OperateResult;
		ZeroMemory(&OperateResult, sizeof(OperateResult));
		OperateResult.wOperateUser = wTargetUser;
		OperateResult.cbOperateCard = cbTargetCard;
		OperateResult.cbOperateCode = cbTargetAction;
		OperateResult.wProvideUser = (m_wProvideUser == INVALID_CHAIR) ? wTargetUser : m_wProvideUser;

		CopyMemory(OperateResult.tagHistoryChart, m_tagHistoryChart, sizeof(m_tagHistoryChart));

		//如果非杠牌
		//碰完分析能否杠
		if (cbTargetAction != WIK_GANG)
		{
			m_wProvideUser = INVALID_CHAIR;
			m_cbProvideCard = INVALID_BYTE;

			tagGangCardResult gcr;
			m_cbUserAction[wTargetUser] |= m_GameLogic.AnalyseGangCard(m_cbCardIndex[wTargetUser], m_WeaveItemArray[wTargetUser], m_cbWeaveItemCount[wTargetUser], gcr);
			OperateResult.cbActionMask |= m_cbUserAction[wTargetUser];

			if (m_GameLogic.AnalyseTingCard(m_cbCardIndex[wTargetUser], m_WeaveItemArray[wTargetUser], m_cbWeaveItemCount[wTargetUser], m_cbCallCard[wTargetUser]))
			{
				SendTingData(wTargetUser, m_cbCardIndex[wTargetUser], m_WeaveItemArray[wTargetUser], m_cbWeaveItemCount[wTargetUser]);
			}
		}

		//发送消息
		m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
		m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
		
		LOG_TABLE(L"操作分数:", FormatScoreString(lCurGangScore));
		if (m_pGameVideo)
		{
			Video_OperateResult VideoOperateResult;
			ZeroMemory(&VideoOperateResult, sizeof(VideoOperateResult));

			VideoOperateResult.wOperateUser = OperateResult.wOperateUser;
			VideoOperateResult.cbOperateCard = OperateResult.cbOperateCard;
			VideoOperateResult.cbOperateCode = OperateResult.cbOperateCode;
			VideoOperateResult.wProvideUser = OperateResult.wProvideUser;

			m_pGameVideo->AddVideoData(SUB_S_OPERATE_RESULT, &VideoOperateResult);
		}

		//设置用户
		m_bGangOutStatus = false;
		m_wCurrentUser = wTargetUser;
		m_ctChangeTime = CTime::GetCurrentTime();

		//杠牌处理
		if (cbTargetAction == WIK_GANG)
		{
			//发送信息
			//明杠：放杠牌玩家输2倍底分，杠牌玩家赢2倍底分
			CMD_S_GangScore gs;
			ZeroMemory (&gs, sizeof(gs));
			gs.wChairId = wTargetUser;
			gs.cbXiaYu = FALSE;
			gs.lGangScore[wTargetUser] = lCurGangScore[wTargetUser];
			gs.lGangScore[m_wProvideUser] = lCurGangScore[m_wProvideUser];
			m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GANG_SCORE, &gs, sizeof(gs));
			m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GANG_SCORE, &gs, sizeof(gs));
			//
			UpdateUserStatus();

			//杠得分
			BYTE cbIndex = m_GangScore[wTargetUser].cbGangCount++;
			m_GangScore[wTargetUser].lScore[cbIndex][wTargetUser] = lCurGangScore[wTargetUser];
			m_GangScore[wTargetUser].lScore[cbIndex][m_wProvideUser] = lCurGangScore[m_wProvideUser];
			m_GangScore[wTargetUser].nGangType[cbIndex] = MING_GANG;
			m_bGangStatus = true;
			m_wProvideGang = OperateResult.wProvideUser;
			//明杠刮风 
			m_stRecord.cbGuaFeng[wTargetUser]++;

			DispatchCardData(wTargetUser, false);
		}

		return true;
	}

	//主动动作
	if (m_wCurrentUser == wChairID)
	{
		//效验操作
		//ASSERT ((cbOperateCode != WIK_NULL) && ((m_cbUserAction[wChairID] & cbOperateCode) != 0));
		if ((cbOperateCode == WIK_NULL)||((m_cbUserAction[wChairID] & cbOperateCode) == 0))
		{
			return true;
		}

		//扑克效验
		//ASSERT ((cbOperateCode == WIK_NULL) || (cbOperateCode == WIK_CHI_HU) || (m_GameLogic.IsValidCard(cbOperateCard) == true));
		if ((cbOperateCode != WIK_NULL) && (cbOperateCode != WIK_CHI_HU) && (m_GameLogic.IsValidCard(cbOperateCard) == false)) 
		{
			return true;
		}

		//设置变量
		m_bSendStatus = true;
		m_bEnjoinChiHu[m_wCurrentUser] = false;
		m_cbUserAction[m_wCurrentUser] = WIK_NULL;
		m_cbPerformAction[m_wCurrentUser] = WIK_NULL;

		bool bPublic = false;

		//执行动作
		switch (cbOperateCode)
		{
		case WIK_GANG:			//杠牌操作
			{
				SCOREEX lCurGangScore[GAME_PLAYER];
				ZeroMemory(lCurGangScore, sizeof(lCurGangScore));

				//变量定义
				BYTE cbWeaveIndex = 0xFF;
				BYTE cbCardIndex = m_GameLogic.SwitchToCardIndex(cbOperateCard);
				
				BYTE cbValidCount = 0;
				for (WORD i=0; i<GAME_PLAYER; i++)
				{
					if (!m_bPlayStatus[i] || i == wChairID) 
					{
						continue;
					}

					cbValidCount++;
				}

				//杠牌处理
				if (m_cbCardIndex[wChairID][cbCardIndex] == 1)
				{
					//寻找组合
					for (BYTE i=0; i<m_cbWeaveItemCount[wChairID]; i++)
					{
						BYTE cbWeaveKind = m_WeaveItemArray[wChairID][i].cbWeaveKind;
						BYTE cbCenterCard = m_WeaveItemArray[wChairID][i].cbCenterCard;
						if ((cbCenterCard == cbOperateCard) && (cbWeaveKind == WIK_PENG))
						{
							bPublic = true;
							cbWeaveIndex = i;
							break;
						}
					}

					//效验动作
					ASSERT (cbWeaveIndex != 0xFF);
					if (cbWeaveIndex == 0xFF) 
					{
						return false;
					}

					//计算各玩家输赢杠分
					for (int i = 0; i < GAME_PLAYER; ++i)
					{
						if (!m_bPlayStatus[i] || i == wChairID)
							continue;

						lCurGangScore[i] = -m_lCellScore;
						lCurGangScore[wChairID] += m_lCellScore;
					}
					//赔付处理
					ChangeUserPaidScore(lCurGangScore);

					//组合扑克
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard = TRUE;
					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser = wChairID;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind = cbOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard = cbOperateCard;

					//补杠刮风 
					m_stRecord.cbGuaFeng[wChairID]++;

					//补杠 (刮风流水清单)
					//刮风玩家
					ASSERT (m_cbHistoryChartCount[wChairID] < MAX_CHART_COUNT);
					WORD wTargetChartIndex = m_cbHistoryChartCount[wChairID]++;
					m_tagHistoryChart[wChairID][wTargetChartIndex].charttype = GUAFENG_TYPE;
					m_tagHistoryChart[wChairID][wTargetChartIndex].lTimes = cbValidCount * 1;
					m_tagHistoryChart[wChairID][wTargetChartIndex].lScore = lCurGangScore[wChairID];

					//被刮风玩家
					for (WORD i=0; i<GAME_PLAYER; i++)
					{
						if (D_Equals(lCurGangScore[i], 0) || i == wChairID)
						{
							continue;
						}
						ASSERT (m_cbHistoryChartCount[i] < MAX_CHART_COUNT);
						WORD wChartIndex = m_cbHistoryChartCount[i]++;
						m_tagHistoryChart[i][wChartIndex].charttype = BEIGUAFENG_TYPE;
						m_tagHistoryChart[i][wChartIndex].lTimes = 1;
						m_tagHistoryChart[i][wChartIndex].lScore = lCurGangScore[i];
						m_tagHistoryChart[i][wChartIndex].bEffectChairID[wChairID] = true;

						m_tagHistoryChart[wChairID][wTargetChartIndex].bEffectChairID[i] = true;
					}
				}
				else
				{
					//扑克效验
					ASSERT (m_cbCardIndex[wChairID][cbCardIndex] == 4);
					if (m_cbCardIndex[wChairID][cbCardIndex] != 4) 
					{
						return false;
					}

					//设置变量
					bPublic = false;
					cbWeaveIndex = m_cbWeaveItemCount[wChairID]++;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbPublicCard = FALSE;
					m_WeaveItemArray[wChairID][cbWeaveIndex].wProvideUser = wChairID;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbWeaveKind = cbOperateCode;
					m_WeaveItemArray[wChairID][cbWeaveIndex].cbCenterCard = cbOperateCard;

					//暗杠下雨 
					m_stRecord.cbXiaYu[wChairID]++;

					//计算各玩家输赢杠分
					for (int i = 0; i < GAME_PLAYER; ++i)
					{
						if (!m_bPlayStatus[i] || i == wChairID)
							continue;

						lCurGangScore[i] = -m_lCellScore * 2;
						lCurGangScore[wChairID] += m_lCellScore * 2;
					}
					//赔付处理
					ChangeUserPaidScore(lCurGangScore);

					//暗杠 (暗杠流水清单)
					//暗杠玩家
					ASSERT (m_cbHistoryChartCount[wChairID] < MAX_CHART_COUNT);
					WORD wTargetChartIndex = m_cbHistoryChartCount[wChairID]++;
					m_tagHistoryChart[wChairID][wTargetChartIndex].charttype = XIAYU_TYPE;
					m_tagHistoryChart[wChairID][wTargetChartIndex].lTimes = cbValidCount * 2;
					m_tagHistoryChart[wChairID][wTargetChartIndex].lScore = lCurGangScore[wChairID];

					//被暗杠玩家
					for (WORD i=0; i<GAME_PLAYER; i++)
					{
						if (D_Equals(lCurGangScore[i], 0) || i == wChairID)
						{
							continue;
						}

						ASSERT (m_cbHistoryChartCount[i] < MAX_CHART_COUNT);
						WORD wChartIndex = m_cbHistoryChartCount[i]++;
						m_tagHistoryChart[i][wChartIndex].charttype = BEIXIAYU_TYPE;
						m_tagHistoryChart[i][wChartIndex].lTimes = 2;
						m_tagHistoryChart[i][wChartIndex].lScore = lCurGangScore[i];
						m_tagHistoryChart[i][wChartIndex].bEffectChairID[wChairID] = true;

						m_tagHistoryChart[wChairID][wTargetChartIndex].bEffectChairID[i] = true;
					}
				}

				//删除扑克
				m_cbCardIndex[wChairID][cbCardIndex] = 0;

				m_bGangStatus = true;

				//杠牌得分 已胡牌玩家不输分
				SCOREEX lScore = bPublic ? m_lCellScore : m_lCellScore * 2L;
				BYTE cbGangIndex = m_GangScore[wChairID].cbGangCount++;

				m_GangScore[wChairID].nGangType[cbGangIndex] = (bPublic ? BU_GANG : AN_GANG);
				for (WORD i=0; i<GAME_PLAYER; i++)
				{
					if (i == wChairID)
					{
						continue;
					}

					m_GangScore[wChairID].lScore[cbGangIndex][i] = lCurGangScore[i];
					m_GangScore[wChairID].lScore[cbGangIndex][wChairID] += -lCurGangScore[i];
				}

				//构造结果
				CMD_S_OperateResult OperateResult;
				ZeroMemory(&OperateResult, sizeof(OperateResult));
				OperateResult.wOperateUser = wChairID;
				OperateResult.wProvideUser = wChairID;
				OperateResult.cbOperateCode = cbOperateCode;
				OperateResult.cbOperateCard = cbOperateCard;

				CopyMemory(OperateResult.tagHistoryChart, m_tagHistoryChart, sizeof(m_tagHistoryChart));

				//发送消息
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_OPERATE_RESULT, &OperateResult, sizeof(OperateResult));
				LOG_TABLE(L"操作分数:", FormatScoreString(lCurGangScore));

				if (m_pGameVideo)
				{
					Video_OperateResult VideoOperateResult;
					ZeroMemory(&VideoOperateResult, sizeof(VideoOperateResult));

					VideoOperateResult.wOperateUser = OperateResult.wOperateUser;
					VideoOperateResult.cbOperateCard = OperateResult.cbOperateCard;
					VideoOperateResult.cbOperateCode = OperateResult.cbOperateCode;
					VideoOperateResult.wProvideUser = OperateResult.wProvideUser;

					m_pGameVideo->AddVideoData(SUB_S_OPERATE_RESULT, &VideoOperateResult);
				}

				//发送信息
				CMD_S_GangScore gs;
				ZeroMemory(&gs, sizeof(gs));
				gs.wChairId = wChairID;
				ZeroMemory(&gs, sizeof(gs));
				gs.cbXiaYu = bPublic ? FALSE : TRUE;
				for (WORD i = 0; i < GAME_PLAYER; i++)
				{
					if (i == wChairID) 
					{
						continue;
					}
					
					//每位玩家输2倍底分,杠牌玩家赢2*输钱人数底分，已胡牌玩家不输分
					gs.lGangScore[i] = lCurGangScore[i];
					gs.lGangScore[wChairID] += -lCurGangScore[i];
				}

				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_GANG_SCORE, &gs, sizeof(gs));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_GANG_SCORE, &gs, sizeof(gs));
				//
				UpdateUserStatus();
				//效验动作(判断抢杠湖)
				bool bAroseAction = false;
				if (bPublic == true) 
				{
					bAroseAction = EstimateUserRespond(wChairID, cbOperateCard, EstimatKind_GangCard);
				}

				//发送扑克
				if (bAroseAction == false)
				{
					DispatchCardData(wChairID, false);
				}

				return true;
			}
		case WIK_CHI_HU:		//吃胡操作
			{
				//自摸玩家
				m_stRecord.cbZiMo[m_wCurrentUser]++;

				//吃牌权位
				if (m_cbOutCardCount == 0)
				{
					m_wProvideUser = m_wCurrentUser;
					m_cbProvideCard = m_cbSendCardData;
				}

				//普通胡牌
				BYTE cbWeaveItemCount = m_cbWeaveItemCount[wChairID];
				tagWeaveItem * pWeaveItem = m_WeaveItemArray[wChairID];
				m_GameLogic.RemoveCard(m_cbCardIndex[wChairID], &m_cbProvideCard, 1);
				m_dwChiHuKind[wChairID] = m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wChairID], pWeaveItem, cbWeaveItemCount, m_cbProvideCard,
					m_ChiHuRight[wChairID], m_cbCallCard[wChairID]);
				m_cbCardIndex[wChairID][m_GameLogic.SwitchToCardIndex(m_cbProvideCard)]++;

				//结束信息
				m_cbChiHuCard = m_cbProvideCard;

				m_cbChiHuCardArray[wChairID] = m_cbProvideCard;
				
				//吃胡得分
				SCOREEX lZiMoChiHuScore = 0L;

				//番数
				WORD wZiMoFanShu = 0L;

				lZiMoChiHuScore = ProcessChiHuUser(wChairID, wZiMoFanShu);
				
				m_bPlayStatus[wChairID] = false;

				BYTE cbValidCount = 0;
				for (WORD i=0; i<GAME_PLAYER; i++)
				{
					if (!m_bPlayStatus[i] || i == wChairID) 
					{
						continue;
					}

					cbValidCount++;
				}
				ASSERT(cbValidCount > 0);
				lZiMoChiHuScore = lZiMoChiHuScore / lZiMoChiHuScore;
				//吃胡得分
				SCOREEX lCurChihuScore[GAME_PLAYER];
				ZeroMemory(lCurChihuScore, sizeof(lCurChihuScore));
				//计算各玩家输赢胡分
				for (int i = 0; i < GAME_PLAYER; ++i)
				{
					if (!m_bPlayStatus[i] || i == wChairID)
						continue;

					lCurChihuScore[i] = -lZiMoChiHuScore;
					lCurChihuScore[wChairID] += lZiMoChiHuScore;
				}
				//赔付处理
				ChangeUserPaidScore(lCurChihuScore);

				//插入流水清单
				//自摸玩家
				ASSERT (m_cbHistoryChartCount[wChairID] < MAX_CHART_COUNT);
				WORD wZiMoChartIndex = m_cbHistoryChartCount[wChairID]++;
				m_tagHistoryChart[wChairID][wZiMoChartIndex].charttype = ZIMO_TYPE;
				m_tagHistoryChart[wChairID][wZiMoChartIndex].lTimes = wZiMoFanShu * cbValidCount;
				m_tagHistoryChart[wChairID][wZiMoChartIndex].lScore = lCurChihuScore[wChairID];;

				for (WORD i=0; i<GAME_PLAYER; i++)
				{
					if (D_Equals(lCurChihuScore[i], 0) || i == wChairID)
					{
						continue;
					}

					m_tagHistoryChart[wChairID][wZiMoChartIndex].bEffectChairID[i] = true;

					//被自摸玩家
					ASSERT (m_cbHistoryChartCount[i] < MAX_CHART_COUNT);
					WORD wChartIndex = m_cbHistoryChartCount[i]++;
					m_tagHistoryChart[i][wChartIndex].charttype = BEIZIMO_TYPE;
					m_tagHistoryChart[i][wChartIndex].lTimes = wZiMoFanShu;
					m_tagHistoryChart[i][wChartIndex].lScore = lCurChihuScore[i];
					m_tagHistoryChart[i][wChartIndex].bEffectChairID[wChairID] = true;
				}

				//发送吃胡
				CMD_S_ChiHu ChiHu;
				ZeroMemory(&ChiHu, sizeof(ChiHu));

				ChiHu.wChiHuUser = wChairID;
				ChiHu.wProviderUser = m_wProvideUser;
				ChiHu.lGameScore = m_lGameScore[wChairID];
				ChiHu.cbCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[wChairID]);
				ChiHu.cbChiHuCard = m_cbProvideCard;
				m_ChiHuRight[wChairID].GetRightData(&(ChiHu.dwChiHuRight), MAX_RIGHT_COUNT);
				ChiHu.cbWinOrder = m_cbWinCount;
				CopyMemory(ChiHu.tagHistoryChart, m_tagHistoryChart, sizeof(m_tagHistoryChart));
				CopyMemory(ChiHu.lHuScore, lCurChihuScore, sizeof(lCurChihuScore));

				//算杠分
				for (WORD i = 0; i<GAME_PLAYER; i++)
				{
					for (WORD j = 0; j<m_GangScore[i].cbGangCount; j++)
					{
						for (WORD k = 0; k<GAME_PLAYER; k++)
						{
							ChiHu.lGangScore[k] += m_GangScore[i].lScore[j][k];
						}
					}
				}

				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CHI_HU, &ChiHu, sizeof(ChiHu));
				m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CHI_HU, &ChiHu, sizeof(ChiHu));
				//
				UpdateUserStatus();
				LOG_TABLE(L"吃胡操作分数:", FormatScoreString(lCurChihuScore));
				m_bWaitWriteScore[wChairID] = true;

				if (m_pGameVideo)
				{
					Video_ChiHu VideoChiHu;
					ZeroMemory(&VideoChiHu, sizeof(VideoChiHu));
					VideoChiHu.wChiHuUser = ChiHu.wChiHuUser;
					VideoChiHu.wProviderUser = ChiHu.wProviderUser;
					VideoChiHu.lGameScore = ChiHu.lGameScore;
					VideoChiHu.cbCardCount = ChiHu.cbCardCount;
					VideoChiHu.cbChiHuCard = ChiHu.cbChiHuCard;
					VideoChiHu.cbWinOrder = ChiHu.cbWinOrder;

					m_pGameVideo->AddVideoData(SUB_S_CHI_HU, &VideoChiHu);
				}

				//判断结束
				BYTE cbPlayCount = 0;
				for (WORD i=0; i<GAME_PLAYER; i++ )
				{
					if (m_bPlayStatus[i]) 
					{
						cbPlayCount++;
					}
				}

				if (cbPlayCount < 2) 
				{
					OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
				}
				else
				{
					//发牌
					m_wCurrentUser = (m_wCurrentUser+GAME_PLAYER+1)%GAME_PLAYER;
					while (!m_bPlayStatus[m_wCurrentUser])
					{
						m_wCurrentUser = (m_wCurrentUser + GAME_PLAYER + 1) % GAME_PLAYER;
					}
					DispatchCardData(m_wCurrentUser, false);
				}

				return true;
			}
		}

		return true;
	}

	return false;
}

//用户换三张
bool CTableFrameSink::OnUserChangeCard(WORD wChairID, BYTE cbChangeCardData[MAX_CHANGE_CARDCOUNT], bool bClient)
{
	//校验是否已经换过
	ASSERT (m_cbChangeCardDataBefore[wChairID][0] == 0 && m_cbChangeCardDataBefore[wChairID][1] == 0 && m_cbChangeCardDataBefore[wChairID][2] == 0);
	if (!(m_cbChangeCardDataBefore[wChairID][0] == 0 && m_cbChangeCardDataBefore[wChairID][1] == 0 && m_cbChangeCardDataBefore[wChairID][2] == 0))
	{
		CString strTrace;
        strTrace.Format(TEXT("table id is %d, ISCHANGE, client = %d"), m_pITableFrame->GetTableID(), bClient);
		//CTraceService::TraceString(strTrace, TraceLevel_Exception);
        //m_pITableFrame->SendGameLogData(strTrace);
		return false;
	}

	//校验同一花色
	BYTE cbCardColor = cbChangeCardData[0] & MASK_COLOR;
	for (WORD i=0; i<MAX_CHANGE_CARDCOUNT; i++)
	{
		if (cbCardColor != (cbChangeCardData[i] & MASK_COLOR))
		{
			CString strTrace;
            strTrace.Format(TEXT("table id is %d ,cbCardColorINVALID, client = %d"), m_pITableFrame->GetTableID(), bClient);
			//CTraceService::TraceString(strTrace, TraceLevel_Exception);
            //m_pITableFrame->SendGameLogData(strTrace);
			return false;
		}
	}
	
	//赋值换牌前扑克
	CopyMemory(m_cbChangeCardDataBefore[wChairID], cbChangeCardData, sizeof(m_cbChangeCardDataBefore[wChairID]));
	
	//换三张标识
	m_bChangeCard[wChairID] = true;

	//校验全部玩家选择换牌
	for (WORD i=0; i<GAME_PLAYER; i++)
	{
		for (WORD j=0; j<MAX_CHANGE_CARDCOUNT; j++)
		{
			if (m_cbChangeCardDataBefore[i][j] == 0)
			{
				return true;
			}
		}
	}

	//全部玩家选择换牌完成
	m_cbChangeType = rand() % MAX_CHANGE_TYPE;

	//设置状态
	m_ctChangeTime = CTime::GetCurrentTime();
	m_pITableFrame->SetGameStatus(GS_MJ_CALL);
	
	//设置超时定时器
	m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);
	m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, (TIME_CALL_CARD + 2) * 1000, 1, 0);

	//顺时针
	if (m_cbChangeType == 0)
	{
		for (WORD i=0; i<GAME_PLAYER; i++)
		{
			BYTE cbChangeChair = (i - 1 + GAME_PLAYER) % GAME_PLAYER;

			//赋值换牌后扑克
			CopyMemory(m_cbChangeCardDataAfter[cbChangeChair], m_cbChangeCardDataBefore[i], sizeof(m_cbChangeCardDataBefore[i]));
		}
	}
	//逆时针
	else if (m_cbChangeType == 1)
	{
		for (WORD i=0; i<GAME_PLAYER; i++)
		{
			BYTE cbChangeChair = (i + 1) % GAME_PLAYER;

			//赋值换牌后扑克
			CopyMemory(m_cbChangeCardDataAfter[cbChangeChair], m_cbChangeCardDataBefore[i], sizeof(m_cbChangeCardDataBefore[i]));
		}
	}
	//对家
	else if (m_cbChangeType == 2)
	{
		for (WORD i=0; i<GAME_PLAYER; i++)
		{
			BYTE cbChangeChair = (i + 2) % GAME_PLAYER;

			//赋值换牌后扑克
			CopyMemory(m_cbChangeCardDataAfter[cbChangeChair], m_cbChangeCardDataBefore[i], sizeof(m_cbChangeCardDataBefore[i]));
		}
	}	
	
	//移除换三张扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (!pIServerUserItem)
		{
			continue;
		}

		m_GameLogic.RemoveCard(m_cbCardIndex[i], m_cbChangeCardDataBefore[i], MAX_CHANGE_CARDCOUNT);
	}
	
	//更新换三张后扑克
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (!pIServerUserItem)
		{
			continue;
		}
			
		for (WORD j=0; j<MAX_CHANGE_CARDCOUNT; j++)
		{
			m_cbCardIndex[i][m_GameLogic.SwitchToCardIndex(m_cbChangeCardDataAfter[i][j])]++;
		}
	}

	//庄家处理 如果发的牌选择为换三张的牌
	bool bChangeCard = false;
	for (WORD i=0; i<MAX_CHANGE_CARDCOUNT; i++)
	{
		if (m_cbChangeCardDataBefore[m_wBankerUser][i] == m_cbSendCardData)
		{
			bChangeCard = true;
			break;
		}
	}

	if (bChangeCard)
	{
		BYTE cbBankerCardData[MAX_COUNT];
		ZeroMemory(cbBankerCardData, sizeof(cbBankerCardData));

		m_GameLogic.SwitchToCardData(m_cbCardIndex[m_wBankerUser], cbBankerCardData);

		m_cbSendCardData = cbBankerCardData[13];
	}

	
	//发送换牌结果
	CMD_S_ChangeCardResult ChangeCardResult;

	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (!pIServerUserItem)
		{
			continue;
		}
		
		ZeroMemory(&ChangeCardResult, sizeof(ChangeCardResult));
		ChangeCardResult.cbChangeType = m_cbChangeType;

		CopyMemory(ChangeCardResult.cbChangeCardResult, m_cbChangeCardDataAfter[i], sizeof(ChangeCardResult.cbChangeCardResult));
		ChangeCardResult.cbSendCardData = (i == m_wBankerUser ? m_cbSendCardData : 0);

		//发送消息
		m_pITableFrame->SendTableData(i, SUB_S_CHANGE_CARD_RESULT, &ChangeCardResult, sizeof(ChangeCardResult));
		m_pITableFrame->SendLookonData(i, SUB_S_CHANGE_CARD_RESULT, &ChangeCardResult, sizeof(ChangeCardResult));
	}

	if (m_pGameVideo)
	{
		Video_ChangeCardResult VideoChangeCardResult;
		ZeroMemory(&VideoChangeCardResult, sizeof(VideoChangeCardResult));

		VideoChangeCardResult.cbChangeType = m_cbChangeType;
		CopyMemory(VideoChangeCardResult.cbChangeCardDataBefore, m_cbChangeCardDataBefore, sizeof(VideoChangeCardResult.cbChangeCardDataBefore));
		CopyMemory(VideoChangeCardResult.cbChangeCardDataAfter, m_cbChangeCardDataAfter, sizeof(VideoChangeCardResult.cbChangeCardDataAfter));
		VideoChangeCardResult.cbSendCardData = m_cbSendCardData;

		m_pGameVideo->AddVideoData(SUB_S_CHANGE_CARD_RESULT, &VideoChangeCardResult);
	}
	
	return true;
}

bool CTableFrameSink::OnUserTrustee(WORD wChairID, bool bTrustee)
{
	//房卡积分模式禁用托管
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0 )
	{
		return false;
	}

	m_bTrustee[wChairID] = bTrustee;

	CMD_S_Trustee Trustee;
	ZeroMemory(&Trustee, sizeof(Trustee));
	Trustee.bTrustee = bTrustee;
	Trustee.wChairID = wChairID;
	m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_TRUSTEE, &Trustee, sizeof(Trustee));
	m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_TRUSTEE, &Trustee, sizeof(Trustee));

	//删除托管定时器
	m_pITableFrame->KillGameTimer(IDI_OUT_CARD_0 + wChairID);

	//
	IServerUserItem * pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if (bTrustee)
	{
		//设置托管标志
		pIServerUserItem->SetTrusteeUser(true);

		//前端发牌需要10秒,在发完牌前主动托管，等完牌再操作
		CTime ctCurTime = CTime::GetCurrentTime();
		CTimeSpan TimeSpan = ctCurTime - m_ctStartTime;
		BYTE cbTempTime = TimeSpan.GetSeconds() < 10 ? (10 - TimeSpan.GetSeconds()) : 1;

		m_pITableFrame->SetGameTimer(IDI_OUT_CARD_0 + wChairID, cbTempTime * 1000, -1, NULL);
	}
	else
	{
		pIServerUserItem->SetTrusteeUser(false);
	}

	return true;
}

//发送操作
bool CTableFrameSink::SendOperateNotify()
{
	//发送提示
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		if (m_cbUserAction[i]!=WIK_NULL)
		{
			//构造数据
			CMD_S_OperateNotify OperateNotify;
			OperateNotify.wResumeUser=m_wResumeUser;
			OperateNotify.cbActionCard=m_cbProvideCard;
			OperateNotify.cbActionMask=m_cbUserAction[i];

			//发送数据
			m_pITableFrame->SendTableData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
			m_pITableFrame->SendLookonData(i,SUB_S_OPERATE_NOTIFY,&OperateNotify,sizeof(OperateNotify));
		}
		else
		{
			CMD_S_WaitOperate WaitOperate;
			ZeroMemory(&WaitOperate, sizeof(WaitOperate));

			WaitOperate.wProvideUser = m_wProvideUser;

			m_pITableFrame->SendTableData(i,SUB_S_WAIT_OPERATE,&WaitOperate,sizeof(WaitOperate));
			m_pITableFrame->SendLookonData(i,SUB_S_WAIT_OPERATE,&WaitOperate,sizeof(WaitOperate));
		}
	}

	m_ctChangeTime = CTime::GetCurrentTime();
	return true;
}

//派发扑克
bool CTableFrameSink::DispatchCardData(WORD wCurrentUser, bool bTail)
{
	//状态效验
	ASSERT (wCurrentUser != INVALID_CHAIR);
	if (wCurrentUser == INVALID_CHAIR)
	{
		return false;
	}

	//判断结束
	BYTE cbPlayCount = 0;
	for (WORD i = 0; i<GAME_PLAYER; i++)
	{
		if (m_bPlayStatus[i])
		{
			cbPlayCount++;
		}
	}

	if (cbPlayCount < 2)
	{
		OnEventGameConclude(INVALID_CHAIR, NULL, GER_NORMAL);
		return true;
	}

	//丢弃扑克
	if ((m_wOutCardUser!=INVALID_CHAIR)&&(m_cbOutCardData!=0))
	{
		m_cbDiscardCount[m_wOutCardUser]++;
		m_cbDiscardCard[m_wOutCardUser][m_cbDiscardCount[m_wOutCardUser] - 1] = m_cbOutCardData;
	}

	//荒庄结束
	if (m_cbLeftCardCount == 0)
	{
		m_cbChiHuCard = 0;
		m_wProvideUser = INVALID_CHAIR;
		OnEventGameConclude(m_wProvideUser, NULL, GER_NORMAL);

		return true;
	}

	//设置变量
	m_cbOutCardData = 0;
	m_wCurrentUser = wCurrentUser;
	m_wOutCardUser = INVALID_CHAIR;
	m_bEnjoinChiHu[wCurrentUser] = false;
	m_ctChangeTime = CTime::GetCurrentTime();

	//杠后炮
	if (m_bGangOutStatus)
	{
		m_bGangOutStatus = false;
	}

	//发牌处理
	if (m_bSendStatus == true)
	{
		//发送扑克
		m_cbSendCardCount++;
		m_cbSendCardData = m_cbRepertoryCard[--m_cbLeftCardCount];

		//if (!m_bTrustee[wCurrentUser])
		{
			//胡牌判断
			CChiHuRight chr;
			m_cbUserAction[wCurrentUser] |= m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[wCurrentUser], m_WeaveItemArray[wCurrentUser], 
				m_cbWeaveItemCount[wCurrentUser], m_cbSendCardData, chr, m_cbCallCard[wCurrentUser]);
		}

		//加牌
		m_cbCardIndex[wCurrentUser][m_GameLogic.SwitchToCardIndex(m_cbSendCardData)]++;

		//设置变量
		m_wProvideUser = wCurrentUser;
		m_cbProvideCard = m_cbSendCardData;

		//杠牌判断
		if ((m_bEnjoinChiPeng[wCurrentUser] == false) && (m_cbLeftCardCount > 0) /*&& !m_bTrustee[wCurrentUser]*/)
		{
			tagGangCardResult GangCardResult;
			ZeroMemory(&GangCardResult, sizeof(GangCardResult));
			m_cbUserAction[wCurrentUser] |= m_GameLogic.AnalyseGangCard(m_cbCardIndex[wCurrentUser], m_WeaveItemArray[wCurrentUser], m_cbWeaveItemCount[wCurrentUser], GangCardResult);
		}
	}

	if (m_GameLogic.AnalyseTingCard(m_cbCardIndex[wCurrentUser], m_WeaveItemArray[wCurrentUser], m_cbWeaveItemCount[wCurrentUser], m_cbCallCard[wCurrentUser]) && !m_bTrustee[wCurrentUser])
	{
		SendTingData(wCurrentUser, m_cbCardIndex[wCurrentUser], m_WeaveItemArray[wCurrentUser], m_cbWeaveItemCount[wCurrentUser]);
	}

	//堆立信息
	ASSERT ((m_wHeapHead != INVALID_CHAIR) && (m_wHeapTail != INVALID_CHAIR));
	if (!bTail)
	{
		//真实CHAIRID 为1,3的28张，0,2为26张

		//切换索引
		BYTE cbHeapCount = m_cbHeapCardInfo[m_wHeapHead][0] + m_cbHeapCardInfo[m_wHeapHead][1];
		
		if (m_wHeapHead == 1 || m_wHeapHead == 3)
		{
			if (cbHeapCount == HEAP_FULL_COUNT + 2) 
			{

				m_wHeapHead = (m_wHeapHead - 1 + GAME_PLAYER) % GAME_PLAYER;
			}
			m_cbHeapCardInfo[m_wHeapHead][0]++;
		}
		else if (m_wHeapHead == 0 || m_wHeapHead == 2)
		{
			if (cbHeapCount == HEAP_FULL_COUNT) 
			{

				m_wHeapHead = (m_wHeapHead - 1 + GAME_PLAYER) % GAME_PLAYER;
			}
			m_cbHeapCardInfo[m_wHeapHead][0]++;

		}

		m_cbMinusHeadCount++;
	}
	else
	{
		//切换索引
		BYTE cbHeapCount = m_cbHeapCardInfo[m_wHeapTail][0] + m_cbHeapCardInfo[m_wHeapTail][1];
		
		if (m_wHeapTail == 1 || m_wHeapTail == 3)
		{
			if (cbHeapCount == HEAP_FULL_COUNT + 2) 
			{
				m_wHeapTail = (m_wHeapTail + 1) % GAME_PLAYER;
			}
			m_cbHeapCardInfo[m_wHeapTail][1]++;
		}
		else if (m_wHeapTail == 0 || m_wHeapTail == 2)
		{
			if (cbHeapCount == HEAP_FULL_COUNT) 
			{
				m_wHeapTail = (m_wHeapTail + 1) % GAME_PLAYER;
			}
			m_cbHeapCardInfo[m_wHeapTail][1]++;
		}

		m_cbMinusLastCount++;
	}

	//构造数据
	CMD_S_SendCard SendCard;
	ZeroMemory(&SendCard, sizeof(SendCard));
	SendCard.wCurrentUser = wCurrentUser;
	SendCard.bTail = bTail;

	//过滤非发牌玩家的发牌数据和权位
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		IServerUserItem* pIServerUserItem = m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem == NULL)
		{
			continue;
		}

		SendCard.cbActionMask = (i == wCurrentUser ? m_cbUserAction[wCurrentUser] : WIK_NULL);
		SendCard.cbCardData = (i == wCurrentUser ? ((m_bSendStatus == true) ? m_cbSendCardData : 0x00) : 0);;

		//发送数据
		m_pITableFrame->SendTableData(i, SUB_S_SEND_CARD, &SendCard, sizeof(SendCard));
	}
	
	if (m_pGameVideo)
	{
		m_pGameVideo->AddVideoData(SUB_S_SEND_CARD, &SendCard);
	}

	BYTE cbIDITime = m_cbUserAction[m_wCurrentUser] == WIK_NULL ? m_pGameCustomRule->cbTimeOutCard + 1 : m_pGameCustomRule->cbTimeOperateCard + 1;
	m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, cbIDITime * 1000, 1, 0);

	if (m_wCurrentUser != INVALID_CHAIR && m_pITableFrame->GetTableUserItem(m_wCurrentUser) && m_pITableFrame->GetTableUserItem(m_wCurrentUser)->IsTrusteeUser())
	{
		m_pITableFrame->SetGameTimer(IDI_OUT_CARD_0 + m_wCurrentUser, m_cbTrusteeDelayTime * 1000, 1, 0);
	}

	return true;
}

//响应判断
bool CTableFrameSink::EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind)
{
	//变量定义
	bool bAroseAction=false;

	//用户状态
	ZeroMemory(m_bResponse,sizeof(m_bResponse));
	ZeroMemory(m_cbUserAction,sizeof(m_cbUserAction));
	ZeroMemory(m_cbPerformAction,sizeof(m_cbPerformAction));

	//动作判断
	for (WORD i=0;i<GAME_PLAYER;i++)
	{
		//用户过滤
		if (wCenterUser==i||!m_bPlayStatus[i] /*|| m_bTrustee[i]*/) continue;

		//出牌类型
		if (EstimatKind==EstimatKind_OutCard)
		{
			//缺一门的牌不能碰杠
			BYTE cbCenterCardColor = (cbCenterCard & MASK_COLOR) >> 4;
			BYTE cbCallCardColor = (m_cbCallCard[i] & MASK_COLOR) >> 4;
			
			//吃碰判断
			if (m_bEnjoinChiPeng[i]==false && cbCenterCardColor != cbCallCardColor)
			{
				//碰牌判断
				m_cbUserAction[i]|=m_GameLogic.EstimatePengCard(m_cbCardIndex[i],cbCenterCard);

				//杠牌判断
				if (m_cbLeftCardCount>0) 
				{
					m_cbUserAction[i]|=m_GameLogic.EstimateGangCard(m_cbCardIndex[i],cbCenterCard);
				}
			}
		}

		//胡牌判断
		if (m_bEnjoinChiHu[i]==false )
		{
			//吃胡判断
			CChiHuRight chr;
			BYTE cbWeaveCount=m_cbWeaveItemCount[i];
			m_cbUserAction[i]|=m_GameLogic.AnalyseChiHuCard(m_cbCardIndex[i],m_WeaveItemArray[i],cbWeaveCount,cbCenterCard,chr,m_cbCallCard[i]);
		}

		//结果判断
		if (m_cbUserAction[i]!=WIK_NULL) 
			bAroseAction=true;
	}

	//结果处理
	if (bAroseAction==true) 
	{
		//设置变量
		m_wProvideUser=wCenterUser;
		m_cbProvideCard=cbCenterCard;
		m_wResumeUser=m_wCurrentUser;
		m_wCurrentUser=INVALID_CHAIR;

		//发送提示
		SendOperateNotify();

		BYTE cbIDITime = m_pGameCustomRule->cbTimeOperateCard + 1;
		m_pITableFrame->SetGameTimer(IDI_SO_OPERATE, cbIDITime * 1000, 1, 0);

		return true;
	}

	return false;
}

//
SCOREEX CTableFrameSink::ProcessChiHuUser(WORD wChairId, WORD &wOutFanShu)
{
	//引用权位
	CChiHuRight &chr = m_ChiHuRight[wChairId];
	
	//基本番型（不重复计算，只记录最大番型）
	FiltrateRight(wChairId, chr);

	WORD wFanShu = GetChiHuFanCount(chr);
	wOutFanShu = wFanShu;
	SCOREEX lChiHuScore = wFanShu * m_lCellScore;

	SCOREEX lOutChiHuScore = 0L;
	
	//呼叫转移 (玩家杠牌后杠上炮 点炮玩家将杠牌分转移给胡牌玩家)
	if (m_bHuJiaoZhuanYi && (m_wProvideUser != wChairId) && m_bGangOutStatus && !m_bGangStatus)
	{
		//如果一炮多响，则转移给第一位玩家(只转移杠别人的分，就是正数的杠分,被杠的分数不转移)
		if (m_GangScore[m_wProvideUser].cbGangCount != 0)
		{
			//算杠分
			SCOREEX lProvideGangScore = 0LL;
			for (WORD j=0; j<m_GangScore[m_wProvideUser].cbGangCount; j++)
			{
				lProvideGangScore += m_GangScore[m_wProvideUser].lScore[j][m_wProvideUser];
			}
			
			//流水清单
			//呼叫转移玩家
			ASSERT (m_cbHistoryChartCount[m_wProvideUser] < MAX_CHART_COUNT);
			WORD wProvideChartIndex = m_cbHistoryChartCount[m_wProvideUser]++;
			m_tagHistoryChart[m_wProvideUser][wProvideChartIndex].charttype = HUJIAOZHUANYI_TYPE;
			m_tagHistoryChart[m_wProvideUser][wProvideChartIndex].lTimes = 1;
			m_tagHistoryChart[m_wProvideUser][wProvideChartIndex].lScore = -lProvideGangScore;
			m_tagHistoryChart[m_wProvideUser][wProvideChartIndex].bEffectChairID[wChairId] = true;

			//被呼叫转移玩家
			ASSERT (m_cbHistoryChartCount[wChairId] < MAX_CHART_COUNT);
			WORD wChartIndex = m_cbHistoryChartCount[wChairId]++;
			m_tagHistoryChart[wChairId][wChartIndex].charttype = HUJIAOZHUANYI_TYPE;
			m_tagHistoryChart[wChairId][wChartIndex].lTimes = 1;
			m_tagHistoryChart[wChairId][wChartIndex].lScore = lProvideGangScore;
			m_tagHistoryChart[wChairId][wChartIndex].bEffectChairID[m_wProvideUser] = true;

			//转移杠分
			for (WORD i=0; i<m_GangScore[m_wProvideUser].cbGangCount; i++)
			{
				ASSERT (m_GangScore[m_wProvideUser].lScore[i][m_wProvideUser] > 0);

				//明杠转移
				if (m_GangScore[m_wProvideUser].nGangType[i] == MING_GANG)
				{
					BYTE cbIndex = m_GangScore[wChairId].cbGangCount++;
					m_GangScore[wChairId].lScore[cbIndex][wChairId] = m_GangScore[m_wProvideUser].lScore[i][m_wProvideUser];

					//寻找被杠的玩家
					WORD wBeiGangChairID = INVALID_CHAIR;
					for (WORD j=0; j<GAME_PLAYER; j++)
					{
						if (m_GangScore[m_wProvideUser].lScore[i][m_wProvideUser] == -(m_GangScore[m_wProvideUser].lScore[i][j]))
						{
							wBeiGangChairID = j;
							break;
						}
					}

					ASSERT (wBeiGangChairID != INVALID_CHAIR);
					m_GangScore[wChairId].lScore[cbIndex][wBeiGangChairID] = -m_GangScore[wChairId].lScore[cbIndex][wChairId];
				}
				//补杠和暗杠转移转移
				else if (m_GangScore[m_wProvideUser].nGangType[i] == BU_GANG || m_GangScore[m_wProvideUser].nGangType[i] == AN_GANG)
				{
					BYTE cbIndex = m_GangScore[wChairId].cbGangCount++;
					m_GangScore[wChairId].lScore[cbIndex][wChairId] = m_GangScore[m_wProvideUser].lScore[i][m_wProvideUser] * 2 / 3;
					
					for (WORD j=0; j<GAME_PLAYER; j++)
					{
						if (j == m_wProvideUser || j == wChairId)
						{
							continue;
						}

						m_GangScore[wChairId].lScore[cbIndex][j] = -(m_GangScore[m_wProvideUser].lScore[i][m_wProvideUser] / 3);
					}						
				}
				
				//重置无效
				ZeroMemory(m_GangScore[m_wProvideUser].lScore[i], sizeof(m_GangScore[m_wProvideUser].lScore[i]));
				m_GangScore[m_wProvideUser].nGangType[i] = INVALID_GANG;
			}

			m_GangScore[m_wProvideUser].cbGangCount = 0;
		}

	}

	//抢杠杠分不算
	if (!(chr&CHR_QIANG_GANG).IsEmpty())
	{
		m_GangScore[m_wProvideUser].cbGangCount--;
		BYTE cbIndex = m_GangScore[m_wProvideUser].cbGangCount;
		
		//重置无效
		ZeroMemory(m_GangScore[m_wProvideUser].lScore[cbIndex], sizeof(m_GangScore[m_wProvideUser].lScore[cbIndex]));
		m_GangScore[m_wProvideUser].nGangType[cbIndex] = INVALID_GANG;
	}

	//自摸
	if (m_wProvideUser == wChairId)
	{
		BYTE cbValidCount = 0;
		for (WORD i=0; i<GAME_PLAYER; i++)
		{
			if (!m_bPlayStatus[i] || i == wChairId) 
			{
				continue;
			}
			
			//每位玩家输相应倍数的底分，胡牌玩家赢相应倍数*输钱人数的底分，已胡牌玩家不输分
			m_lGameScore[i] -= lChiHuScore;
			cbValidCount++;
		}

		m_lGameScore[wChairId] += (cbValidCount * lChiHuScore);

		lOutChiHuScore = cbValidCount * lChiHuScore;
	}
	//点炮
	else
	{
		m_lGameScore[m_wProvideUser] -= lChiHuScore;
		m_lGameScore[wChairId] += lChiHuScore;

		lOutChiHuScore = lChiHuScore;
	}

	//设置变量
	m_wProvider[wChairId] = m_wProvideUser;
	m_wWinOrder[m_cbWinCount++] = wChairId;
	if (m_cbWinCount == 1) 
	{
		m_wFirstWinUser = wChairId;
	}
	m_bGangStatus = false;
	m_bGangOutStatus = false;
	m_wProvideGang = INVALID_CHAIR;

	//m_pITableFrame->SetPlayingStatusLeave(wChairId, TRUE);
	//m_pITableFrame->GetTableUserItem(wChairId)->SetUserStatus(US_SIT, m_pITableFrame->GetTableID(), wChairId);

	//发送消息
	//CMD_S_ChiHu ChiHu;
	//ChiHu.wChiHuUser = wChairId;
	//ChiHu.wProviderUser = m_wProvideUser;
	//ChiHu.lGameScore = m_lGameScore[wChairId];
	//ChiHu.cbCardCount = m_GameLogic.GetCardCount(m_cbCardIndex[wChairId]);
	//ChiHu.cbChiHuCard = m_cbProvideCard;
	//ChiHu.cbWinOrder = m_cbWinCount;
	//CopyMemory(ChiHu.tagHistoryChart, m_tagHistoryChart, sizeof(m_tagHistoryChart));

	//m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_CHI_HU, &ChiHu, sizeof(ChiHu));
	//m_pITableFrame->SendLookonData(INVALID_CHAIR, SUB_S_CHI_HU, &ChiHu, sizeof(ChiHu));

	return lOutChiHuScore;
}

//
void CTableFrameSink::FiltrateRight(WORD wChairId, CChiHuRight &chr)
{
	//临时权位
	CChiHuRight tempchr;
	tempchr.SetEmpty();

	//自摸
	if (m_wCurrentUser == wChairId)
	{		
		tempchr |= CHR_ZIMO;

		if (GetChiHuFanCount(tempchr) > GetChiHuFanCount(chr))
		{
			chr.SetEmpty();
			chr |= CHR_ZIMO;
		}
	}

	//天胡
	if (m_cbSendCardCount == 1 && m_cbOutCardCount == 0)
	{
		tempchr |= CHR_TIAN_HU;

		if (GetChiHuFanCount(tempchr) > GetChiHuFanCount(chr))
		{
			chr.SetEmpty();
			chr |= CHR_TIAN_HU;
		}
	}
	//地胡
	else if (m_cbSendCardCount == 1 && m_cbOutCardCount == 1)
	{
		tempchr |= CHR_DI_HU;

		if (GetChiHuFanCount(tempchr) > GetChiHuFanCount(chr))
		{
			chr.SetEmpty();
			chr |= CHR_DI_HU;
		}
	}

	//权位增加另外加番
	//抢杠胡
	if (m_wCurrentUser == INVALID_CHAIR && m_bGangStatus)
	{
		chr |= CHR_QIANG_GANG;
	}
	//杠上炮
	if (m_bGangOutStatus && !m_bGangStatus)
	{
		chr |= CHR_GANG_SHANG_PAO;
	}
	//杠上花
	if (m_wCurrentUser == wChairId && m_bGangStatus)
	{
		chr |= CHR_GANG_KAI;
	}
	//海底捞月(自模胡才算，点炮不算捞月)
	if (m_cbLeftCardCount == 0 && m_wCurrentUser == wChairId)
	{
		chr |= CHR_HAIDI_LAOYUE;
	}
}

//获取吃胡番数
WORD CTableFrameSink::GetChiHuFanCount(const CChiHuRight & ChiHuRight)
{
	//普通番
	WORD wFanShu = 0;
	WORD wAddFan = 1;

	//平胡
	if (!(ChiHuRight & CHR_PING_HU).IsEmpty())
	{
		wFanShu = 1;
	}
	//自摸胡
	else if(!(ChiHuRight & CHR_ZIMO).IsEmpty())
	{
		wFanShu = (m_bZiMoAddTimes == true ? 2 : 1);
	}
	//门清
	else if(!(ChiHuRight & CHR_MEN_QING).IsEmpty())
	{
		wFanShu = (m_bMenQing == true ? 2 : 1);
	}
	//对对胡
	else if(!(ChiHuRight & CHR_DUI_DUI_HU).IsEmpty())
	{
		wFanShu = 2;
	}
	//清一色
	else if(!(ChiHuRight & CHR_QING_YI_SE).IsEmpty())
	{
		wFanShu = 4;
	}
	//带幺九
	else if(!(ChiHuRight & CHR_DAI_YAO_JIU).IsEmpty())
	{
		wFanShu = 4;
	}
	//七小对
	else if(!(ChiHuRight & CHR_QI_XIAO_DUI).IsEmpty())
	{
		wFanShu = 4;
	}
	//金钩钩（单吊）
	else if(!(ChiHuRight & CHR_JIN_GOUGOU).IsEmpty())
	{
		wFanShu = 4;
	}
	//清对（清一色的对对胡）
	else if(!(ChiHuRight & CHR_QING_DUI).IsEmpty())
	{
		wFanShu = 8;
	}
	//龙七对
	else if(!(ChiHuRight & CHR_LONG_QI_DUI).IsEmpty())
	{
		wFanShu = 16;
	}
	//清七对（清一色的七小对）
	else if(!(ChiHuRight & CHR_QING_QI_DUI).IsEmpty())
	{
		wFanShu = 16;
	}
	//清幺九
	else if(!(ChiHuRight & CHR_QING_YAO_JIU).IsEmpty())
	{
		wFanShu = 16;
	}
	//将金钩钩
	else if(!(ChiHuRight & CHR_JIANG_JIN_GOUGOU).IsEmpty())
	{
		wFanShu = 16;
	}
	//清金钩钩
	else if(!(ChiHuRight & CHR_QING_JIN_GOUGOU).IsEmpty())
	{
		wFanShu = 16;
	}
	//天胡
	else if(!(ChiHuRight & CHR_TIAN_HU).IsEmpty())
	{
		wFanShu = (m_bTianDiHu == true ? 32 : 1);
	}
	//地胡
	else if(!(ChiHuRight & CHR_DI_HU).IsEmpty())
	{
		wFanShu = (m_bTianDiHu == true ? 32 : 1);
	}
	//清龙七对
	else if(!(ChiHuRight & CHR_QING_LONG_QI_DUI).IsEmpty())
	{
		wFanShu = 32;
	}
	//十八罗汉
	else if(!(ChiHuRight & CHR_SHIBA_LUOHAN).IsEmpty())
	{
		wFanShu = 64;
	}
	//清十八罗汉
	else if(!(ChiHuRight & CHR_QING_SHIBA_LUOHAN).IsEmpty())
	{
		wFanShu = 256;
	}

	ASSERT( wFanShu > 0 );

	//额外番
	//杠上花
	if (!(ChiHuRight & CHR_GANG_KAI).IsEmpty())
	{
		wAddFan *= 2;
	}

	//杠上炮
	if (!(ChiHuRight & CHR_GANG_SHANG_PAO).IsEmpty())
	{
		wAddFan *= 2;
	}

	//抢杠
	if (!(ChiHuRight & CHR_QIANG_GANG).IsEmpty())
	{
		wAddFan *= 2;
	}

	//海底捞月
	if (!(ChiHuRight & CHR_HAIDI_LAOYUE).IsEmpty())
	{
		wAddFan *= 2;
	}

	return wFanShu * wAddFan;
}

//写日志文件
void CTableFrameSink::WriteInfo(LPCTSTR pszString, const TCHAR szServerName[LEN_SERVER])
{
	//设置语言区域
	char* old_locale = _strdup( setlocale(LC_CTYPE,NULL) );
	setlocale( LC_CTYPE, "chs" );

	CStdioFile myFile;
	CString strServer;
	strServer.Format(TEXT("血战麻将[%s]调试日志"), szServerName);
	BOOL bOpen = myFile.Open(strServer, CFile::modeReadWrite|CFile::modeCreate|CFile::modeNoTruncate);
	if ( bOpen )
	{	
		myFile.SeekToEnd();
		myFile.WriteString( pszString );
		myFile.Flush();
		myFile.Close();
	}

	//还原区域设定
	setlocale( LC_CTYPE, old_locale );
	free( old_locale );
}

//发送听牌数据，打哪些牌胡哪些牌 
void CTableFrameSink::SendTingData(WORD wChairID, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount)
{
	if (!m_pITableFrame->GetTableUserItem(wChairID)->IsRobotUser())
	{
		int a = 3;
	}

	CMD_S_TING_DATA TingData;
	ZeroMemory(&TingData, sizeof(TingData));

	m_GameLogic.GetTingDataEx(m_cbCardIndex[wChairID], WeaveItem, cbWeaveCount, TingData.cbOutCardCount, TingData.cbOutCardData, TingData.cbHuCardCount, TingData.cbHuCardData, TingData.cbHuFan, m_cbCallCard[wChairID]);
	for (int i = 0; i<MAX_COUNT; i++)
	{
		if (TingData.cbHuCardCount[i]>0)
		{
			for (int j = 0; j<TingData.cbHuCardCount[i]; j++)
			{
				TingData.cbHuCardRemainingCount[i][j] = GetRemainingCount(wChairID, TingData.cbHuCardData[i][j]);
			}
		}
		else
		{
			break;
		}
	}

	m_pITableFrame->SendTableData(wChairID, SUB_S_TING_DATA, &TingData, sizeof(TingData));
}

//获取指定玩家某张牌未出数量
BYTE CTableFrameSink::GetRemainingCount(WORD wChairID, BYTE cbCardData)
{
	ASSERT(m_GameLogic.IsValidCard(cbCardData));
	if (!m_GameLogic.IsValidCard(cbCardData))
	{
		CString strTrace;
		strTrace.Format(TEXT("InValidCard=%d"), cbCardData);
		//CTraceService::TraceString(strTrace, TraceLevel_Exception);
	}

	BYTE cbIndex = m_GameLogic.SwitchToCardIndex(cbCardData);
	BYTE nRemainCount = 0;

	//剩余牌墩
	for (int i = m_cbMinusLastCount; i<MAX_REPERTORY - m_cbMinusHeadCount; i++)
	{
		if (m_cbRepertoryCard[i] == cbCardData)
		{
			nRemainCount++;
		}
	}

	//其他玩家手牌
	for (WORD i = 0; i<GAME_PLAYER; i++)
	{
		if (!m_bPlayStatus[i])
		{
			continue;
		}

		if (i == wChairID)
		{
			continue;
		}

		nRemainCount += m_cbCardIndex[i][cbIndex];
	}

	return nRemainCount;
}


//一对一赔付计算
//一对多赔付计算
//多对一赔付计算
void CTableFrameSink::ChangeUserPaidScore(SCOREEX lUserGameScore[GAME_PLAYER])
{
	//仅金币房生效
	if (((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0) )
		return;

	int nWinCount = 0;
	int nLoseCount = 0;
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		if (D_GreaterThen(lUserGameScore[i], 0))
			++nWinCount;
		else if (D_LessThen(lUserGameScore[i], 0))
			++nLoseCount;
	}
	//流局不处理
	if (nWinCount == 0 || nLoseCount == 0)
	{
		for (int nTmpIdx = 0; nTmpIdx < GAME_PLAYER; ++nTmpIdx)	m_lCurTotalWinloseScore[nTmpIdx] += lUserGameScore[nTmpIdx];
		return;
	}
	//多赔多不处理
	if (nWinCount > 1 && nLoseCount > 1)
	{
		for (int nTmpIdx = 0; nTmpIdx < GAME_PLAYER; ++nTmpIdx)	m_lCurTotalWinloseScore[nTmpIdx] += lUserGameScore[nTmpIdx];
		return;
	}

	WORD wTargetChair = INVALID_CHAIR;
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		if (D_Equals(0, lUserGameScore[i]))
			continue;
		wTargetChair = i;
		//获取赔分玩家位置
		if (nLoseCount == 1 && D_GreaterThen(0, lUserGameScore[i]))
			break;
		//获取赢分玩家位置
		else if (nWinCount == 1 && D_GreaterThen(lUserGameScore[i], 0))
			break;
	}
	//计算玩家剩余携带
	SCOREEX lUserTakeScore[GAME_PLAYER];
	memset(lUserTakeScore, 0, sizeof(lUserTakeScore));
	for (int i = 0; i < GAME_PLAYER; ++i)
	{
		IServerUserItem* pUser = m_pITableFrame->GetTableUserItem(i);
		if (pUser == nullptr)
			continue;

		lUserTakeScore[i] += m_lCurTotalWinloseScore[i] + pUser->GetUserScore();
		//防负
		if (D_LessThen(lUserTakeScore[i], 0))
			lUserTakeScore[i] = 0;
	}

	LOG_TABLE(L"各玩家剩余携带:", FormatScoreString(lUserTakeScore), L"当前总输赢:", FormatScoreString(m_lCurTotalWinloseScore));
	LOG_TABLE(L"调整分数，前:", FormatScoreString(lUserGameScore));

	ASSERT(wTargetChair != INVALID_CHAIR);
	SCOREEX lOffsetScore = 0;
	//多赔一,一赔一
	if (D_GreaterThen(lUserGameScore[wTargetChair], 0) || (D_LessThen(lUserGameScore[wTargetChair], 0) && nWinCount == 1))
	{
		//等比例赔付
		lOffsetScore = lUserGameScore[wTargetChair] - lUserTakeScore[wTargetChair];
		if (D_GreaterThen(lOffsetScore, 0))
		{
			for (int i = 0; i < GAME_PLAYER; ++i)
			{
				if (i == wTargetChair)
					continue;
				if (D_Equals(lUserGameScore[i], 0))
					continue;

				ASSERT(D_UnGreaterThen(lUserGameScore[i], 0));
				lUserGameScore[i] -= ((double)lUserGameScore[i] / lUserGameScore[wTargetChair]) * lOffsetScore;
			}
			lUserGameScore[wTargetChair] -= lOffsetScore;
		}

		//以小博大
		for (int i = 0; i < GAME_PLAYER; ++i)
		{
			if (i == wTargetChair)
				continue;
			if (D_Equals(lUserGameScore[i], 0))
				continue;

			if (D_GreaterThen(-lUserGameScore[i], lUserTakeScore[i]))
			{
				lUserGameScore[wTargetChair] += (lUserTakeScore[i] + lUserGameScore[i]);
				lUserGameScore[i] = -lUserTakeScore[i];
			}
		}
	}
	//一赔多
	if (D_LessThen(lUserGameScore[wTargetChair], 0))
	{
		//等比例赔付
		lOffsetScore = -lUserGameScore[wTargetChair] - lUserTakeScore[wTargetChair];
		if (D_GreaterThen(lOffsetScore, 0))
		{
			for (int i = 0; i < GAME_PLAYER; ++i)
			{
				if (i == wTargetChair)
					continue;
				if (D_Equals(lUserGameScore[i], 0))
					continue;

				ASSERT(D_GreaterThen(lUserGameScore[i], 0));
				lUserGameScore[i] += ((double)lUserGameScore[i] / lUserGameScore[wTargetChair]) * lOffsetScore;
				//被赔付方获取值为0,只支付下家
				if (D_Equals(lUserGameScore[i], 0))
				{
					SCOREEX tmp = lUserGameScore[wTargetChair];
					memset(lUserGameScore, 0, sizeof(SCOREEX)*GAME_PLAYER);

					lUserGameScore[wTargetChair] = tmp;
					lUserGameScore[i] = -(lUserGameScore[wTargetChair] + lOffsetScore);
					break;
				}
			}
			lUserGameScore[wTargetChair] += lOffsetScore;
		}

		//以小博大
		for (int i = 0; i < GAME_PLAYER; ++i)
		{
			if (i == wTargetChair)
				continue;
			if (D_Equals(lUserGameScore[i], 0))
				continue;

			if (D_GreaterThen(lUserGameScore[i], lUserTakeScore[i]))
			{
				lUserGameScore[wTargetChair] += (lUserGameScore[i] - lUserTakeScore[i]);
				lUserGameScore[i] = lUserTakeScore[i];
			}
		}
	}

	LOG_TABLE(L"调整分数，后:", FormatScoreString(lUserGameScore));

	//更新总输赢分数
	for (int nTmpIdx = 0; nTmpIdx < GAME_PLAYER; ++nTmpIdx)
	{
		m_lCurTotalWinloseScore[nTmpIdx] += lUserGameScore[nTmpIdx];
	}
};
//流局判定
bool CTableFrameSink::IsLiuju()
{
	if (m_cbLeftCardCount == 0)
		return true;

	
	int nLoseCount = 0;
	int nGiveupCount = 0;
	for (WORD i = 0; i < GAME_PLAYER; i++)
	{
		if (m_ChiHuRight[i].IsEmpty())
		{
			if (!m_bPlayStatus[i])
				++nLoseCount;
			else
				++nGiveupCount;
		}
	}
	//未胡牌玩家大于2人，未胡牌玩家中没有认输玩家算流局
	if (nLoseCount >= 2)
		return true;

	//出现三个认输玩家，并且没有胡牌玩家，算流局
	if (nGiveupCount >= 3)
		return true;

	return false;
};
//更新玩家状态
void CTableFrameSink::UpdateUserStatus()
{
	//更新总输赢分数
	for (int nTmpIdx = 0; nTmpIdx < GAME_PLAYER; ++nTmpIdx)
	{
		//非结算状态更新玩家状态
		if (m_bPlayStatus[nTmpIdx])
		{
			IServerUserItem* pUser = m_pITableFrame->GetTableUserItem(nTmpIdx);
			ASSERT(pUser != nullptr);
			if (D_UnGreaterThen(pUser->GetUserScore() + m_lCurTotalWinloseScore[nTmpIdx], 0))
			{
				m_bPlayStatus[nTmpIdx] = false;
				m_bWaitWriteScore[nTmpIdx] = true;
				//输完(考虑退税情况，这里不让玩家离开)
				//pUser->SetUserStatus(US_SIT, m_pITableFrame->GetTableID(), nTmpIdx);

				LOG_TABLE(L"玩家:", pUser->GetGameID(), L"已输完，当前总输赢:", m_lCurTotalWinloseScore[nTmpIdx]);

				CMD_S_UserGiveup notify;
				ZeroMemory(&notify, sizeof(notify));
				notify.wChairID = nTmpIdx;
				m_pITableFrame->SendTableData(INVALID_CHAIR, SUB_S_USER_GIVEUP, &notify, sizeof(notify));
			}
		}
	}
};


//开始人数
WORD CTableFrameSink::GetStartPlayerCount()
{
	return m_wPlayerCount;

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
	CTraceService::TraceString(_T("s12345"), TraceLevel_Exception);

	//设置变量
	m_lCellScore = lBaseScore;
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

//解散时间 
DWORD CTableFrameSink::GetBattleDismissTime()
{
	return 0;
	//return TIME_DISMISS_TABLE;
}

//踢人时间
DWORD CTableFrameSink::GetUnReadyKickOutTime()
{
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
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


//同意人数
WORD CTableFrameSink::GetDismissAgreeCount()
{
	//变量定义
	WORD wPlayerCount = 0;

	if (m_pITableFrame->GetGameStatus() == GAME_STATUS_FREE)
	{
		//用户状态
		for (WORD i = 0; i < m_wPlayerCount; i++)
		{
			//获取用户
			IServerUserItem* pIServerUser = m_pITableFrame->GetTableUserItem(i);
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
			IServerUserItem* pIServerUser = m_pITableFrame->GetTableUserItem(i);
			if (pIServerUser != NULL ) wPlayerCount++;
		}
	}
	return wPlayerCount;
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
