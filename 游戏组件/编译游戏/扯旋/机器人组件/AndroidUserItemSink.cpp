#include "Stdafx.h"
#include "AndroidUserItemSink.h"

//////////////////////////////////////////////////////////////////////////

//����ʱ��
#define TIME_DISPATCH				1									//����ʱ��
#ifdef DEBUG
//��Ϸʱ��
#define TIME_LESS					1									//����ʱ��
#define TIME_OUT_CARD				6									//����ʱ��
#define TIME_START_GAME				6									//��ʼʱ��
#define TIME_CALL_SCORE				6									//�з�ʱ��
#else
//��Ϸʱ��
#define TIME_LESS					1									//����ʱ��
#define TIME_OUT_CARD				6									//����ʱ��
#define TIME_START_GAME				6									//��ʼʱ��
#define TIME_CALL_SCORE				6									//�з�ʱ��

#endif // DEBUG

//��Ϸʱ��
#define IDI_INVEST				0										//�±�ʱ��
#define IDI_ADD_BET				2										//��עʱ��
#define IDI_START_GAME			1										//��ʼʱ��
#define IDI_OPEN_CARD			3										//����ʱ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CRobotUserItemSink::CRobotUserItemSink()
{
	//��Ϸ����
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

	//�����˿�
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount,sizeof(m_cbHandCardCount));

	//�ӿڱ���
	m_pITestUserItem =NULL;

	return;
}

//��������
CRobotUserItemSink::~CRobotUserItemSink()
{
}

//�ӿڲ�ѯ
VOID * __cdecl CRobotUserItemSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IRobotUserItemSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IRobotUserItemSink,Guid,dwQueryVer);
	return NULL;
}

//��ʼ�ӿ�
bool __cdecl CRobotUserItemSink::InitUserItemSink(IUnknownEx * pIUnknownEx)
{
	//��ѯ�ӿ�
	m_pITestUserItem =QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx, IRobotUserItem);
	if (m_pITestUserItem ==NULL) return false;

	m_pITestUserItem->SetAdroidVersion(VERSION_CLIENT);

	return true;
}

//���ýӿ�
bool __cdecl CRobotUserItemSink::ResetUserItemSink()
{
	//��Ϸ����
	m_wBankerUser = INVALID_CHAIR;
	m_lMinAddScore = 0;
	m_lUserInvest = 0;
	m_lTurnMaxScore = 0;
	m_lTurnMinScore = 0;
	m_lTotalAddBet = 0;
	//m_lMaxBoboMultiple = 0;
	//m_lUserScore = 0;

	//�����˿�
	ZeroMemory(m_cbHandCardData, sizeof(m_cbHandCardData));
	ZeroMemory(m_cbHandCardCount, sizeof(m_cbHandCardCount));

	return true;
}

//ʱ����Ϣ
bool __cdecl CRobotUserItemSink::OnEventTimer(UINT nTimerID)
{
	switch (nTimerID)
	{
	case IDI_INVEST:	//�±�ʱ��
	{
		m_pITestUserItem->KillGameTimer(IDI_INVEST);

		//�����±�
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

		//��������
		m_pITestUserItem->SendSocketData(SUB_C_USER_INVEST, &UserInvest, sizeof(UserInvest));
		m_lUserInvest += m_lUserScore;
		m_lTotalAddBet += m_lCellScore;

		return true;
	}
	case IDI_START_GAME:	//��ʼ��Ϸ
	{
		m_pITestUserItem->KillGameTimer(IDI_START_GAME);

		//׼���ж�
		if (m_pITestUserItem->GetUserScore() > m_lCellScore * MIN_MUTIPLIE)
		{

			//��ʼ�ж�
			m_pITestUserItem->SendUserReady(NULL, 0);
		}
		else
		{
			//������������������
			m_pITestUserItem->SetRobotAction(ROBOT_WAITSTANDUP,true);
		}

		return true;
	}
	case IDI_ADD_BET:	//�û���ע
	{

		m_pITestUserItem->KillGameTimer(IDI_ADD_BET);
		BYTE cbOperatorCode[4] = { CODE_PASS ,CODE_FOLLOW ,CODE_SHOWHAND ,CODE_ADD };
		SCORE lAddSCore[4] = { 0,m_lTurnMinScore,m_lUserInvest - m_lTotalAddBet,m_lMinAddScore};

		BYTE bCanBet[4] = { false,false,false,false };
		//δ���
		if (m_lTotalAddBet != m_lUserInvest && rand() % 100 < 90)
		{
			//���
			bCanBet[2] = true;

			//��һ����ע������
			if (m_lTurnMinScore == 0 && m_lTurnMaxScore == 0)
			{
				//20%�ĸ�����
				if (rand() % 100 < 20)
				{
					CMD_C_AddScore AddScore;
					ZeroMemory(&AddScore, sizeof(AddScore));
					AddScore.lScore = 0;
					AddScore.cbCode = CODE_PASS;
					//��������
					m_pITestUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

					m_lTotalAddBet += AddScore.lScore;

					return true;
				}
			}

			//�ɸ�
			if (m_lUserInvest > m_lTotalAddBet)
			{
				bCanBet[1] = true;
			}
			//��ע
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
					//��������
					m_pITestUserItem->SendSocketData(SUB_C_ADD_SCORE, &AddScore, sizeof(AddScore));

					m_lTotalAddBet += AddScore.lScore;

					return true;
				}
			}

		}
		

		{
			//����
			m_bGiveUp = true;
			m_pITestUserItem->SendSocketData(SUB_C_GIVE_UP, nullptr, 0);
			return true;
		}

		return true;
	}
	case IDI_OPEN_CARD:		//�û�̯��
	{

		m_pITestUserItem->KillGameTimer(IDI_OPEN_CARD);

		//��������
		const BYTE cbCount = 12;
		BYTE cbHandCardData[cbCount];
		ZeroMemory(cbHandCardData, sizeof(cbHandCardData));

		WORD wChairID = m_pITestUserItem->GetChairID();

		//�����˿�
		BYTE cbTempHandCardData[MAX_COUNT];
		CopyMemory(cbTempHandCardData, m_cbHandCardData[wChairID], sizeof(cbTempHandCardData));

		//�������
		BYTE cbIndexHead[cbCount] = { 0,1,0,2,0,3,1,2,1,3,2,3 };
		BYTE cbIndexTail[cbCount] = { 2,3,1,3,1,2,0,3,0,2,0,1 };

		//�����˿�
		for (int i = 0; i < cbCount; i++)
		{
			cbHandCardData[i] = cbTempHandCardData[cbIndexHead[i]];
		}

		//�Ƚ��˿�
		BYTE cbMaxIndex = 0;
		int i = 0;
		for (i = cbMaxIndex + 2; i < CountArray(cbHandCardData); i += 2)
		{
			BYTE cbValue = m_GameLogic.CompareCard(&cbHandCardData[i], &cbHandCardData[cbMaxIndex], 2);
			if (cbValue == TRUE) cbMaxIndex = i;
		}

		//�����˿�
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

//��Ϸ��Ϣ
bool __cdecl CRobotUserItemSink::OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_S_GAME_START:	//��Ϸ��ʼ
	{
		return OnSubGameStart(pData, wDataSize);
	}
	case SUB_S_USER_INVEST:	//�û��±�
	{
		return OnUserInvest(pData, wDataSize);
	}
	case SUB_S_ADD_SCORE:	//�û���ע
	{
		return OnAddScore(pData, wDataSize);
	}
	case SUB_S_SEND_CARD:	//��������
	{
		return OnSubSendCard(pData, wDataSize);
	}
	case SUB_S_GAME_END:	//��Ϸ����
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
	case SUB_S_OPEN_START:	//��ʼ̯��
	{
		if (m_cbHandCardCount[m_pITestUserItem->GetChairID()] > 0)
		{
			//����
			UINT nElapse = rand() % TIME_START_GAME + TIME_LESS;
			m_pITestUserItem->SetGameTimer(IDI_OPEN_CARD, nElapse + (rand() % TIME_OUT_CARD));
		}
		return true;
	}
	case SUB_S_GIVE_UP:		//�û�����
	{
		return true;
	}
	case SUB_S_UPDATE_SCORE:		//���·���
	{
		CMD_S_UpdateScore * pUpdateScore = (CMD_S_UpdateScore *)pData;
		
		//m_lUserScore = pUpdateScore->lUserScore;
		
		return true;
	}
	case SUB_S_USER_AGREE_SCORE:		//�û�����
	{
		return true;
	}
	case SUB_S_REQUEST_RULE:		//�û�����
	{
		return true;
	}
	case SUB_S_OPEN_CARD: //�û�����
	{
		return true;
	}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//��Ϸ��Ϣ
bool __cdecl CRobotUserItemSink::OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//������Ϣ
bool __cdecl CRobotUserItemSink::OnEventSendGameScene(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize)
{
	switch (cbGameStatus)
	{
	case GS_TK_FREE:	//����״̬
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_S_StatusFree));
			if (wDataSize!=sizeof(CMD_S_StatusFree)) return false;

			//��������
			CMD_S_StatusFree * pStatusFree=(CMD_S_StatusFree *)pData;
			//IServerUserItem * pIServerUserItem= m_pITestUserItem->GetMeUserItem();

			m_lCellScore = pStatusFree->lCellScore;


			//׼���ж�
			if (m_pITestUserItem->GetUserScore() > m_lCellScore * MIN_MUTIPLIE)
			{
				
				//��ʼ�ж�
				m_pITestUserItem->SendUserReady(NULL, 0);

			}
			else
			{
				//������������������
				m_pITestUserItem->SetRobotAction(ROBOT_WAITSTANDUP, true);
			}

			return true;
		}
	case GS_TK_INVEST:	//��Ϸ״̬
		{
			//Ч������
			if (wDataSize!=sizeof(CMD_S_StatusInvest)) return false;
			CMD_S_StatusInvest * pStatusInvest =(CMD_S_StatusInvest *)pData;

			m_wBankerUser = pStatusInvest->wBankerUser;
			m_lCellScore = pStatusInvest->lCellScore;

			//�˿�����
			WORD wMeChairID= m_pITestUserItem->GetChairID();

			//�������
			if (pStatusInvest->cbPlayStatus[wMeChairID]== GUS_PLAYING && !pStatusInvest->bInvestFinish[wMeChairID])
			{
				UINT nElapse=rand()%TIME_OUT_CARD+TIME_LESS;
				m_pITestUserItem->SetGameTimer(IDI_INVEST,nElapse);
			}

			return true;
		}
	case GS_TK_SCORE: //��ע״̬
	{
		//Ч������
		if (wDataSize != sizeof(CMD_S_StatusScore)) return false;
		CMD_S_StatusScore * pStatusScore = (CMD_S_StatusScore *)pData;

		m_lCellScore = pStatusScore->lCellScore;
		m_wBankerUser = pStatusScore->wBankerUser;

		m_lMinAddScore = pStatusScore->lMinAddScore;
		m_lTurnMaxScore = pStatusScore->lTurnMaxScore;
		m_lTurnMinScore = pStatusScore->lTurnMinScore;

		return true;
	}
	case GS_TK_OPEN_CARD://����״̬
	{
		//Ч������
		if (wDataSize != sizeof(CMD_S_StatusOpenCard)) return false;
		CMD_S_StatusOpenCard * pStatusInvest = (CMD_S_StatusOpenCard *)pData;

		m_lCellScore = pStatusInvest->lCellScore;
		m_wBankerUser = pStatusInvest->wBankerUser;

		//�˿�����
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

		//�������
		if (pStatusInvest->cbPlayStatus[wMeChairID] == GUS_PLAYING && pStatusInvest->cbOpenFinish[wMeChairID]==0)
		{
			UINT nElapse = rand() % TIME_OUT_CARD + TIME_LESS;
			m_pITestUserItem->SetGameTimer(IDI_OPEN_CARD, nElapse);
		}

		return true;
	}
	}

	//�������
	ASSERT(FALSE);

	return false;
}

//�û�����
VOID __cdecl CRobotUserItemSink::OnEventUserEnter(IRobotUserItem * pITestUserItem, bool bLookonUser)
{
	return;
}

//�û��뿪
VOID __cdecl CRobotUserItemSink::OnEventUserLeave(IRobotUserItem * pITestUserItem, bool bLookonUser)
{
	return;
}

//�û�����
VOID __cdecl CRobotUserItemSink::OnEventUserScore(IRobotUserItem * pITestUserItem, bool bLookonUser)
{
	return;
}

//�û�״̬
VOID __cdecl CRobotUserItemSink::OnEventUserStatus(IRobotUserItem * pITestUserItem, bool bLookonUser)
{
	return;
}

//�û���λ
VOID __cdecl CRobotUserItemSink::OnEventUserSegment(IRobotUserItem * pITestUserItem, bool bLookonUser)
{
	return;
}

//��Ϸ��ʼ
bool CRobotUserItemSink::OnSubGameStart(VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize==sizeof(CMD_S_GameStart));
	if (wDataSize!=sizeof(CMD_S_GameStart)) return false;

	//��������
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

//�û��±�
bool CRobotUserItemSink::OnUserInvest(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_UserInvest));
	if(wDataSize!=sizeof(CMD_S_UserInvest)) return false;
	//��������
	CMD_S_UserInvest * pUserInvest =(CMD_S_UserInvest *)pData;

	if (pUserInvest->wChairID == m_pITestUserItem->GetChairID())
	{
		m_lUserInvest = pUserInvest->lScore;
	}

	return true;
}

//�û���ע
bool CRobotUserItemSink::OnAddScore(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize==sizeof(CMD_S_AddScore));
	if (wDataSize!=sizeof(CMD_S_AddScore)) return false;

	//��������
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
	//	//����
	//	m_pITestUserItem->SetGameTimer(IDI_OPEN_CARD, nElapse + (rand() % TIME_OUT_CARD));
	//}

	return true;
}

//��Ϸ����
bool CRobotUserItemSink::OnSubSendCard(VOID * pData, WORD wDataSize)
{
	//Ч������
	ASSERT(wDataSize == sizeof(CMD_S_SendCard));
	if (wDataSize != sizeof(CMD_S_SendCard)) return false;

	//��������
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

	//��ע
	if(wChairID== pSendCard->wCurrentUser && m_lTotalAddBet< m_lUserInvest)
	{
		m_pITestUserItem->SetGameTimer(IDI_ADD_BET, nElapse + (rand() % TIME_OUT_CARD));
	}
	else if (pSendCard->wCurrentUser == INVALID_CHAIR)
	{
		//ȫ���������
		m_pITestUserItem->SetGameTimer(IDI_OPEN_CARD, nElapse + (rand() % TIME_OUT_CARD));
	}

	return true;
}


//����д��־
VOID CRobotUserItemSink::WriteControlLog(string & str)
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
	_sntprintf_s(szServiceDir, CountArray(szServiceDir), TEXT("%s\\CheXuan\\%d_%d.txt"), szWorkDir, TimeNow.GetDay(), m_pITestUserItem->GetTableID());

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
//�����������
DECLARE_CREATE_MODULE(RobotUserItemSink);