#include "StdAfx.h"
#include "TableFrameSink.h"
#include <locale>

//////////////////////////////////////////////////////////////////////////
//
#define LOG_TABLE(...) LOG_DEBUG(L"���Ӻ�:",m_pITableFrame->GetTableID(),##__VA_ARGS__)

CString GetFileDialogPath()
{
	CString strFileDlgPath;
	TCHAR szModuleDirectory[MAX_PATH];	//ģ��Ŀ¼
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
//��̬����

#define IDI_OUT_CARD_0				3									//���ƶ�ʱ��
#define IDI_OUT_CARD_1				4									//���ƶ�ʱ��
#define IDI_OUT_CARD_2				5									//���ƶ�ʱ��
#define IDI_OUT_CARD_3				6									//���ƶ�ʱ��
#define	IDI_SO_OPERATE				7									//��ʱ����ʱ��
#define IDI_GAMEEND					8									//���㶨ʱ��

#define TIME_CHANGE_CARD			5
#define TIME_CALL_CARD				5


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

//���캯��
CTableFrameSink::CTableFrameSink()
{
	//��Ϸ����

	//�������
	m_pITableFrame = NULL;
	m_pGameServiceOption = NULL;
	//m_pGameCustomRule = NULL;

	//��Ϸ���򣨷������ã�

	//�������
	srand((int)time(0));

	return;
}

//��������
CTableFrameSink::~CTableFrameSink(void)
{
}

//�ӿڲ�ѯ
void* CTableFrameSink::QueryInterface(const IID& Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink, Guid, dwQueryVer);
	QUERYINTERFACE(ITableUserAction, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink, Guid, dwQueryVer);
	return NULL;
}

//��ʼ��
bool  CTableFrameSink::InitTableFrameSink(IUnknownEx* pIUnknownEx)
{
	//��ѯ�ӿ�
	ASSERT(pIUnknownEx != NULL);
	m_pITableFrame = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, ITableFrameItem);

	if (m_pITableFrame == NULL)
		return false;

	//��ȡ����
	m_pGameServiceOption = m_pITableFrame->GetGameServiceOption();
	ASSERT(m_pGameServiceOption != NULL);

	//��ʼģʽ
	m_pITableFrame->SetStartMode(StartMode_AllReady);

	//�Զ�����
	//ASSERT (m_pITableFrame->GetCustomRule() != NULL);
	//m_pGameCustomRule = (tagCustomRule *)m_pITableFrame->GetCustomRule();

	/*m_bHuanSanZhang = m_pGameCustomRule->bHuanSanZhang;
	m_bHuJiaoZhuanYi = m_pGameCustomRule->bHuJiaoZhuanYi;
	m_bZiMoAddTimes = m_pGameCustomRule->bZiMoAddTimes;
	m_bTianDiHu = m_pGameCustomRule->bTianDiHu;
	m_bHaiDiLaoYue = m_pGameCustomRule->bHaiDiLaoYue;
	m_bMenQing = m_pGameCustomRule->bMenQing;
	m_cbTrusteeDelayTime = m_pGameCustomRule->cbTrusteeDelayTime;*/

	//��־
	//CString strDir = GetFileDialogPath() + TEXT("\\") + GAME_NAME + TEXT("��־");
	//CreateDirectory(strDir, NULL);
	//strDir.AppendFormat(TEXT("\\%s(%d)"), GAME_NAME, m_pITableFrame->GetGameServiceOption()->wServerID);
	//EasyLog::CLog::m_log.SetName(strDir.GetBuffer());

	//BYTE i = m_pITableFrame->GetDataBaseMode();
	return true;
}

//��λ����
void  CTableFrameSink::ResetTableFrameSink()
{
	//��Ϸ����
	return;
}

//���ٻ���
SCOREEX CTableFrameSink::QueryLessEnterScore(WORD wChairID, IServerUserItem* pIServerUserItem)
{
	return 0;
}

//��Ϸ��ʼ
bool CTableFrameSink::OnEventGameStart()
{

	//�����ţ�ǰ�˷�����Ҫ10��

	return true;
}

//��Ϸ����
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem* pIServerUserItem, BYTE cbReason)
{
	return true;
}

//���ͳ���
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem* pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	return true;
}

//��ʱ���¼�
bool CTableFrameSink::OnTimerMessage(DWORD wTimerID, WPARAM wBindParam)
{
	return true;
}

//��Ϸ��Ϣ����
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, VOID* pDataBuffer, WORD wDataSize, IServerUserItem* pIServerUserItem)
{
	return true;
}

//�����Ϣ����
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, VOID* pDataBuffer, WORD wDataSize, IServerUserItem* pIServerUserItem)
{
	return false;
}

bool CTableFrameSink::OnActionUserOffLine(IServerUserItem* pIServerUserItem)
{

	//��ҳ��ͽ�ҷ��������йܣ����ַ������й�
	/*
	if (((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0))
	{
		BYTE cbGameStatus = m_pITableFrame->GetGameStatus();
		if (cbGameStatus == GS_MJ_CHANGECARD || cbGameStatus == GS_MJ_CALL)
		{
			////�����йܱ�־
			//pIServerUserItem->SetTrusteeUser(true);
			//m_pITableFrame->SetGameTimer(IDI_OUT_CARD_0 + wChairID, m_cbTrusteeDelayTime * 1000, 1, 0);
		}
		else
		{
			//if (!((wChairID == m_wCurrentUser) || (m_wCurrentUser == INVALID_CHAIR && m_cbUserAction[wChairID] != WIK_NULL)))
			//{
			//	//�����йܱ�־
			//	pIServerUserItem->SetTrusteeUser(true);
			//}
		}
	}
	*/
	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserSitDown(IServerUserItem* pIServerUserItem, bool bLookonUser)
{
	//m_vecRecord[wChairID].clear();
	return true;
}

//�û�����
bool CTableFrameSink::OnActionUserStandUp(IServerUserItem* pIServerUserItem, bool bLookonUser)
{
	//ׯ������
	if (bLookonUser == false)
	{
		/*m_bTrustee[wChairID]=false;
		CMD_S_Trustee Trustee;
		Trustee.bTrustee=false;
		Trustee.wChairID = wChairID;
		m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_TRUSTEE,&Trustee,sizeof(Trustee));*/
	}

	//��;�뿪д��
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

		//����ģʽ
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0)
	{
		//Լս��ɢ
	}

	//������ʷ����
	//m_HistoryScore.ResetData();
	//m_vecRecord[wChairID].clear();
	return true;
}

//�û�ͬ��
bool  CTableFrameSink::OnActionUserReady(IServerUserItem* pIServerUserItem, VOID* pData, WORD wDataSize)
{
	//׼������Ͼּ�¼
	//ZeroMemory(m_tagHistoryChart[wChairID], sizeof(m_tagHistoryChart[wChairID]));

	return true;
}

bool CTableFrameSink::OnUserTrustee(WORD wChairID, bool bTrustee)
{
	//��������ģʽ�����й�
	if ((m_pGameServiceOption->wServerType & GAME_GENRE_GOLD) != 0)
	{
		return false;
	}

	//ɾ���йܶ�ʱ��
	m_pITableFrame->KillGameTimer(IDI_OUT_CARD_0 + wChairID);

	//
	IServerUserItem* pIServerUserItem = m_pITableFrame->GetTableUserItem(wChairID);
	if (bTrustee)
	{
		//�����йܱ�־
		pIServerUserItem->SetTrusteeUser(true);

		//ǰ�˷�����Ҫ10��,�ڷ�����ǰ�����йܣ��������ٲ���
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

//��ʼ����
WORD CTableFrameSink::GetStartPlayerCount()
{
	return 0;

}

//����׼��
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

//�׷ֱ��
VOID CTableFrameSink::OnEventBaseScoreVariation(LONG lBaseScore)
{
	//CString s;
	//s.Format(_T("a2  lBaseScore:%I64d"), lBaseScore);
	//CTraceService::TraceString(_T("s12345"), TraceLevel_Exception);
}

//�������
VOID CTableFrameSink::OnEventScoreVariation(IServerUserItem* pIServerUserItem)
{
	//���·���
	/*if (pIServerUserItem->GetUserStatus() != US_LOOKON)
	{
		UpdateUserScore(pIServerUserItem->GetChairID());
	}*/
}

//���ӽ�ɢ
VOID CTableFrameSink::OnEventTableDismiss()
{
	//��������
	//ZeroMemory(m_lUserScore, sizeof(m_lUserScore));
	//ZeroMemory(m_lUserFreeScore, sizeof(m_lUserFreeScore));
}

//�����޸�
VOID CTableFrameSink::OnEventTableModify(LPVOID pModifyParam, WORD wBufferSize)
{
	tagBattleCreateParam* pParam = (tagBattleCreateParam*)pModifyParam;

	CopyMemory(&m_BattleConfig, pParam, wBufferSize);

	// kk jia start
	//m_lWinnerScore1 = pParam->BattleConfig.lMinWin;						//Ӯ�ҷ���
	//m_lWinnerScore2 = pParam->BattleConfig.lWin2;						//Ӯ�ҷ���
	//m_lWinnerRevenue1 = pParam->BattleConfig.lRevenueRate1;					//Ӯ��˰��	

	//m_lWinnerScore3 = pParam->BattleConfig.lWin3;						//Ӯ�ҷ�	

	//m_lWinnerRevenue2 = pParam->BattleConfig.lRevenueRate2;					//Ӯ��˰��
	//m_lWinnerRevenue3 = pParam->BattleConfig.lRevenueRate3;					//Ӯ��˰��

	//CString s;
	//s.Format(_T("�޸����� m_lWinnerScore1:%I64d, m_lWinnerScore2:%I64d, m_lWinnerScore3:%I64d, m_lWinnerRevenue1:%I64d, m_lWinnerRevenue2:%I64d, m_lWinnerRevenue3:%I64d"), m_lWinnerScore1, m_lWinnerScore2, m_lWinnerScore3, m_lWinnerRevenue1, m_lWinnerRevenue2, m_lWinnerRevenue3);
	//CTraceService::TraceString(s, TraceLevel_Exception);

	//kk jia end

	//BYTE cbIndex = 0;

	////�Ⱥ���
	//bool bAllowBankerFirst = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;

	////����
	//bool bAllowSpecialCard = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? false : true;

	////�ؾ��������
	//bool bAllowDJWBigCard = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;

	////������
	//bool bAllowBetGGF = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;


	//SCORE lMaxBoboMutiple[3] = { 300,500,1000 };

	//BYTE cbSelectIndex = pParam->BattleConfig.cbGameOption[cbIndex++];
	//if (cbSelectIndex < 3)
	//{
	//	//Я������
	//	m_CustomConfig.lMaxBoboMultiple = lMaxBoboMutiple[cbSelectIndex];
	//	//CString s;
	//	//s.Format(_T("�޸����Ӻ� Я������:%ld"), m_CustomConfig.lMaxBoboMultiple);
	//	//CTraceService::TraceString(s, TraceLevel_Exception);

	//	//��ʼ����
	//	for (WORD i = 0; i < GAME_PLAYER; i++)
	//	{
	//		m_lUserScore[i] = 0;

	//		//m_lUserFreeScore[i] = m_CustomConfig.lMaxBoboMultiple * m_lCellScore;//kk guan
	//		//��ѯ�û�
	//		IServerUserItem* pIServerUserItem = m_pITableFrame->GetTableUserItem(i);//kk jia
	//		if (pIServerUserItem != NULL)//kk jia
	//			m_lUserFreeScore[i] = pIServerUserItem->GetUserScore();//kk jia
	//	}
	//}

	//WORD wMinBetValue = 0;

	//CopyMemory(&wMinBetValue, &pParam->BattleConfig.cbGameOption[cbIndex], sizeof(BYTE) * 2);

	////Я������
	//m_CustomConfig.lMinBetAmount = static_cast<SCORE>(wMinBetValue) * m_lCellScore;


	////�Ⱥ���
	//if (bAllowBankerFirst == true)
	//{
	//	m_CustomConfig.cbRuleOption |= GR_ALLOW_FIRSTER;
	//}
	//else
	//{
	//	m_CustomConfig.cbRuleOption &= ~GR_ALLOW_FIRSTER;
	//}

	////����
	//if (bAllowSpecialCard == true)
	//{
	//	m_CustomConfig.cbRuleOption |= GR_SHS_AND_SHL;
	//}
	//else
	//{
	//	m_CustomConfig.cbRuleOption &= ~GR_SHS_AND_SHL;
	//}

	////�ؾ��������
	//if (bAllowDJWBigCard == true)
	//{
	//	m_CustomConfig.cbRuleOption |= GR_DJW_BIG_CARD;
	//}
	//else
	//{
	//	m_CustomConfig.cbRuleOption &= ~GR_DJW_BIG_CARD;
	//}

	////��ע������
	//if (bAllowBetGGF == true)
	//{
	//	m_CustomConfig.cbRuleOption |= GR_ALLOW_BET_GGF;
	//}
	//else
	//{
	//	m_CustomConfig.cbRuleOption &= ~GR_ALLOW_BET_GGF;
	//}

	////���ù���
	//m_GameLogic.SetRuleOption(m_CustomConfig.cbRuleOption);


	////���±���
	//m_pITableFrame->SetSitdownGameMultiple(MIN_MUTIPLIE);

	////kk jia
	//m_pITableFrame->SetMinEnterScore(m_CustomConfig.lMaxBoboMultiple);

	////��������
	//m_pITableFrame->SetStandupGameMultiple(MIN_MUTIPLIE);

	m_pITableFrame->SetMaxPlayerCount(m_BattleConfig.BattleConfig.wPlayerCount);

	return;
}

//���Ӵ���
VOID CTableFrameSink::OnEventTableCreated(LPVOID pCreateConfig, WORD wBufferSize)
{
	tagBattleCreateParam* pParam = (tagBattleCreateParam*)pCreateConfig;

	CopyMemory(&m_BattleConfig, pParam, wBufferSize);

	//// kk jia start
	//m_lWinnerScore1 = pParam->BattleConfig.lMinWin;						//Ӯ�ҷ���
	//m_lWinnerScore2 = pParam->BattleConfig.lWin2;						//Ӯ�ҷ���
	//m_lWinnerRevenue1 = pParam->BattleConfig.lRevenueRate1;					//Ӯ��˰��	

	//m_lWinnerScore3 = pParam->BattleConfig.lWin3;						//Ӯ�ҷ�	

	//m_lWinnerRevenue2 = pParam->BattleConfig.lRevenueRate2;					//Ӯ��˰��
	//m_lWinnerRevenue3 = pParam->BattleConfig.lRevenueRate3;					//Ӯ��˰��

	//CString s;
	//s.Format(_T("create���� m_lWinnerScore1:%I64d, m_lWinnerScore2:%I64d, m_lWinnerScore3:%I64d, m_lWinnerRevenue1:%I64d, m_lWinnerRevenue2:%I64d, m_lWinnerRevenue3:%I64d"), m_lWinnerScore1, m_lWinnerScore2, m_lWinnerScore3, m_lWinnerRevenue1, m_lWinnerRevenue2, m_lWinnerRevenue3);
	//CTraceService::TraceString(s, TraceLevel_Exception);

	//kk jia end


	//BYTE cbIndex = 0;

	////�Ⱥ���
	//bool bAllowBankerFirst = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;

	////����
	//bool bAllowSpecialCard = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? false : true;

	////�ؾ��������
	//bool bAllowDJWBigCard = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;

	////������
	//bool bAllowBetGGF = pParam->BattleConfig.cbGameOption[cbIndex++] == 0 ? true : false;


	//SCORE lMaxBoboMutiple[3] = { 300,500,1000 };

	//BYTE cbSelectIndex = pParam->BattleConfig.cbGameOption[cbIndex++];
	//if (cbSelectIndex < 3)
	//{
	//	//Я������
	//	m_CustomConfig.lMaxBoboMultiple = lMaxBoboMutiple[cbSelectIndex];

	//	//CString s;
	//	//s.Format(_T("Я������:%ld"), m_CustomConfig.lMaxBoboMultiple);
	//	//CTraceService::TraceString(s, TraceLevel_Exception);

	//	//��ʼ����
	//	for (WORD i = 0; i < GAME_PLAYER; i++)
	//	{
	//		m_lUserScore[i] = 0;

	//		//m_lUserFreeScore[i] = m_CustomConfig.lMaxBoboMultiple * m_lCellScore;//kk guan
	//		//��ѯ�û�
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

	//	//Я������
	//m_CustomConfig.lMinBetAmount = static_cast<SCORE>(lMinBetValue) * m_lCellScore;


	////�Ⱥ���
	//if (bAllowBankerFirst == true)
	//{
	//	m_CustomConfig.cbRuleOption |= GR_ALLOW_FIRSTER;
	//}
	//else
	//{
	//	m_CustomConfig.cbRuleOption &= ~GR_ALLOW_FIRSTER;
	//}

	////����
	//if (bAllowSpecialCard == true)
	//{
	//	m_CustomConfig.cbRuleOption |= GR_SHS_AND_SHL;
	//}
	//else
	//{
	//	m_CustomConfig.cbRuleOption &= ~GR_SHS_AND_SHL;
	//}

	////�ؾ��������
	//if (bAllowDJWBigCard == true)
	//{
	//	m_CustomConfig.cbRuleOption |= GR_DJW_BIG_CARD;
	//}
	//else
	//{
	//	m_CustomConfig.cbRuleOption &= ~GR_DJW_BIG_CARD;
	//}

	////��ע������
	//if (bAllowBetGGF == true)
	//{
	//	m_CustomConfig.cbRuleOption |= GR_ALLOW_BET_GGF;
	//}
	//else
	//{
	//	m_CustomConfig.cbRuleOption &= ~GR_ALLOW_BET_GGF;
	//}

	////���ù���
	//m_GameLogic.SetRuleOption(m_CustomConfig.cbRuleOption);

	//m_wBattleCount = 0;

	////���±���
	//m_pITableFrame->SetSitdownGameMultiple(MIN_MUTIPLIE);
	////kk jia
	//m_pITableFrame->SetMinEnterScore(m_CustomConfig.lMaxBoboMultiple);

	////��������
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
