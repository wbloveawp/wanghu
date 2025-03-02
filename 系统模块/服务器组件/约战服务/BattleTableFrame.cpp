#include "StdAfx.h"
#include "math.h"
#include "DataBasePacket.h"
#include "BattleTableFrame.h"
#include "..\��Ϸ����\DataBasePacket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//��������
#define MIN_GPS_DISTANCE			100									//GPS����

#ifdef _DEBUG
#define TIME_BATTLE_TABLE_DISMISS	 600								//��ɢʱ��
#define TIME_BATTLE_AGREEMENT_DISMISS 60								//Э���ɢ
#else
#define TIME_BATTLE_TABLE_DISMISS	  10								//��ɢʱ��
#define TIME_BATTLE_AGREEMENT_DISMISS 60*2								//Э���ɢ
#endif

//////////////////////////////////////////////////////////////////////////

//����ṹ
struct tagUserRateItem
{
	WORD wChairID;
	WORD wAddupWinRate;
	BYTE cbControlStatus;
};

//////////////////////////////////////////////////////////////////////////

//����У��
bool IsGPSValid(double dLat, double dLng) 
{
	if (abs(dLat)>90.f) return false;
	if (abs(dLng)>180.f) return false;

	return true;
}

//����GPS����
double GetGPSDistance(double dLat1, double dLng1, double dLat2, double dLng2)
{
	//��ΧУ��
	if (IsGPSValid(dLat1,dLng1)==false) return 0;
	if (IsGPSValid(dLat2,dLng2)==false) return 0;

	//��������
  double dDistance=0.f;
  double dLonRes=102900.f,dLatRes=110000.f;

	//�������
	double dAbsLat=abs(dLat1-dLat2);
	double dAbsLng=abs(dLng1-dLng2);
  dDistance=sqrt(dAbsLat*dLatRes*dAbsLat*dLatRes + dAbsLng*dLonRes*dAbsLng*dLonRes);

  return dDistance;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CTableFrameBattle::CTableFrameBattle()
{
	//��־����
	m_bDeductCost = false;
	m_bReturnCost = false;
	m_bDismissNotify = false;
	m_bBattleService = false;
	m_bBattleFinish = false;

	//Э���ɢ
	m_dwLaunchTime = 0;
	m_dwLaunchUserID = 0;
	ZeroMemory(m_cbBallotResult, sizeof(m_cbBallotResult));

	//Լս��Ϣ
	m_dwConfigID = 0;
	m_dwBattleNum = 0;
	m_dwCreateTime = 0;
	m_dwBattleRule = 0;
	m_bBattleService = true;
	m_lBattleRecordID = 0;
	m_cbBattleState = BATTLE_STATE_FREE;
	ZeroMemory(&m_BattleConfig, sizeof(m_BattleConfig));

	//������Ϣ
	m_dwClubID = 0;
	m_wSwitchOption = 0;
	m_dwClubCreaterID = 0;

	//����ӿ�
	m_pIEventBattleItem = NULL;
	m_pIEventGameOption = NULL;
	m_pITCPSocketBattle = NULL;
}

//��������
CTableFrameBattle::~CTableFrameBattle()
{
}

//�ӿڲ�ѯ
VOID* CTableFrameBattle::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IEventBattleSink, Guid, dwQueryVer);
	return __super::QueryInterface(Guid, dwQueryVer);
}

//����˰��
SCORE CTableFrameBattle::AccountRevenue(SCORE lGameScore)
{
	//Լս����
	if (m_BattleConfig.cbScoreKind == SCORE_KIND_BATTLE) return 0L;

	//�߼�����
	auto pAdvancedOption = (tagAdvancedOption*)m_BattleConfig.cbAdvancedOption;

	if (lGameScore<= pAdvancedOption->lMinWinScore) return 0L;

	return (pAdvancedOption->lRevenueRate+m_BattleConfig.lGameRevenue) * lGameScore / 1000L;

}

//��������
WORD CTableFrameBattle::GetRelationChairID(DWORD dwUserID)
{
	auto pBattleUserItem = GetBattleUserItem(dwUserID);
	return pBattleUserItem ? pBattleUserItem->wChairID:INVALID_WORD;
}

//��������
tagRelationParameter* CTableFrameBattle::GetRelationParameter()
{
	//����ṹ
	static tagRelationParameter_Battle RelationParameter = {};

	//���ñ���
	RelationParameter.dwClubID = m_dwClubID;
	RelationParameter.cbScoreKind = m_BattleConfig.cbScoreKind;
	RelationParameter.dwRelationNum = m_dwBattleNum;
	RelationParameter.lRelationRecordID = m_lBattleRecordID;	

	return &RelationParameter;
}

//д�����
bool CTableFrameBattle::WriteUserScore(WORD wChairID, tagScoreInfo& ScoreInfo)
{
	if (!__super::WriteUserScore(wChairID, ScoreInfo))
	{
		return false;
	}

	//ͳ�Ƴɼ�
	if (m_dwBattleNum != 0)
	{
		//��ȡ��Ϣ
		tagBattleScoreItem* pUserScoreItem = NULL;

		//��������
		for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
		{
			if (m_BattleUserItemArray[i].wChairID == wChairID)
			{
				pUserScoreItem = &m_BattleUserItemArray[i];
				break;
			}
		}

		//ͳ�Ƴɼ�
		if (pUserScoreItem != NULL)
		{
			//ͳ�ƾ���
			if (ScoreInfo.lScore < 0) pUserScoreItem->lLostCount++;
			if (ScoreInfo.lScore > 0) pUserScoreItem->lWinCount++;
			if (ScoreInfo.lScore == 0) pUserScoreItem->lDrawCount++;

			//�ۼƷ���
			pUserScoreItem->lScore += ScoreInfo.lScore;
		}
	}

	return true;
}

//kk jjia
//qu zhuo zi canshu
CMD_GF_TableParam CTableFrameBattle::GetTableParam(tagScoreInfo ScoreInfoArray[], WORD wScoreCount)
{
	//��������
	CMD_GF_TableParam GFTableParam;
	ZeroMemory(&GFTableParam, sizeof(GFTableParam));

	//������Ϣ
	GFTableParam.lBaseScore = m_lBaseScore;
	GFTableParam.dwOwnerID = m_dwTableOwnerID;
	GFTableParam.dwBattleNum = m_dwBattleNum;

	//������Ϣ
	GFTableParam.wFinishCount = m_wFinishCount;
	GFTableParam.wPlayCount = m_BattleConfig.wPlayCount;

	//��¼��ʶ
	GFTableParam.lBattleRecordID = m_lBattleRecordID;
	return GFTableParam;

}


//д�����
bool CTableFrameBattle::WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount)
{
	//CTraceService::TraceString(_T("WriteTableScore_1"), TraceLevel_Exception);
	if (!__super::WriteTableScore(ScoreInfoArray, wScoreCount))//
	{
		return false;
	}
	//CTraceService::TraceString(_T("WriteTableScore_2"), TraceLevel_Exception);
	//ͳ�Ƴɼ�
	if (m_dwBattleNum != 0)
	{
		//��������
		tagScoreInfo* pScoreInfo = NULL;
		tagBattleScoreItem* pUserScoreItem = NULL;

		for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
		{
			//��ȡ��Ϣ
			pUserScoreItem = &m_BattleUserItemArray[i];
			pScoreInfo = &ScoreInfoArray[pUserScoreItem->wChairID];				

			//����˰��
			SCORE lClubRevenue = 0;// CalcCustomRevenue(pScoreInfo->lScore + pScoreInfo->lRevenue);//kk guan
						
			//test start
			//�߼�����
		//auto pAdvancedOption = (tagAdvancedOption*)m_BattleConfig.cbAdvancedOption;
			//CString s;
			//s.Format(_T("a16  lScore:%I64d,lClubRevenue:%I64d, lRevenue:%I64d, m_BattleConfig.cbScoreKind:%d , SCORE_KIND_BATTLE:%d"), pScoreInfo->lScore, lClubRevenue, pScoreInfo->lRevenue, m_BattleConfig.cbScoreKind, SCORE_KIND_BATTLE);
			//CTraceService::TraceString(s, TraceLevel_Exception); 
			//test 
			// 
			//���»���
			pUserScoreItem->lScore += pScoreInfo->lScore;//
			pUserScoreItem->lClubRevenue += lClubRevenue;
			pUserScoreItem->lSysRevenue += pScoreInfo->lRevenue-lClubRevenue;			

			//�޸�ʤ��
			switch (pScoreInfo->ScoreType)
			{
			case ScoreType_Win:		//ʤ��
			{
				pUserScoreItem->lWinCount++;
				break;
			}
			case ScoreType_Lose:	//���
			{
				pUserScoreItem->lLostCount++;
				break;
			}
			case ScoreType_Draw:	//�;�
			{
				pUserScoreItem->lDrawCount++;
				break;
			}
			}
		}
	}

	return true;
}

//Լս����
bool CTableFrameBattle::BattleFinish(BYTE cbReason)
{
	//��������
	bool bGameStarted = IsGameStarted();	

	//�ж�ԭ��
	if (m_dwBattleNum != 0 && cbReason != BFR_NORMAL)
	{
		//�˷�У��
		if (m_BattleConfig.lConsumeCount > 0 && (m_wFinishCount == 0 || (m_wFinishCount == 1 && bGameStarted == true)))
		{
			//�˻�����
			if (m_bDeductCost == true)
			{
				PerformReturnCost(m_BattleConfig.wPayType);
			}
			else
			{
				m_bReturnCost = true;
			}
		}

		//������Ϣ
		WORD wType = 0;
		TCHAR szMessage[128] = L"";

		//��ʱ��ɢ
		if (cbReason == BFR_OFFLINE_EXPIRE)
		{
			wType = SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE;
			StringCchCopy(szMessage, CountArray(szMessage), CWHService::FormatMessage(MIM_SERVER_0065, TEXT("��Ϸ��ʱʱ���ѵ�����ǰ����ҳ�ʱ����߳����涨ʱ�䣻Լս�Զ�������")));
		}

		//Э���ɢ
		if (cbReason == BFR_AGREEMENT_DISMISS)
		{
			wType = SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE;
			if (m_wFinishCount == 0) wType |= SMT_CLOSE_GAME;
			StringCchCopy(szMessage, CountArray(szMessage), CWHService::FormatMessage(MIM_SERVER_0066, TEXT("�÷��侭���Э�̣�һ��ͬ���ɢ��������Լս��")));
		}

		//��ϢȺ��
		if (szMessage[0] != 0)
		{
			for (WORD i = 0; i < m_wChairCount; i++)
			{
				//��ȡ�û�
				IServerUserItem* pIServerUserItem = GetTableUserItem(i);
				if ((pIServerUserItem == NULL) || (pIServerUserItem->IsClientReady() == false)) continue;

				//��������							
				m_pIServiceFunction->SendGameMessage(pIServerUserItem, szMessage, wType);
			}

			//��������
			WORD wEnumIndex = 0;
			IServerUserItem* pIServerUserItem = NULL;

			//�Թ��û�
			do
			{
				//��ȡ�û�
				pIServerUserItem = EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem == NULL) break;
				if (pIServerUserItem->IsClientReady() == false) continue;

				//��������							
				m_pIServiceFunction->SendGameMessage(pIServerUserItem, szMessage, wType);

			} while (true);
		}
	}

	//��ɱ�־
	m_bBattleFinish = (cbReason == BFR_NORMAL);	

	//��ɢ��Ϸ
	if (bGameStarted == true) DismissGame();	

	//Լս����
	if (m_dwBattleNum != 0) DismissBattleTable(!m_bReturnCost);

	return true;
}

//��ȡ����
bool CTableFrameBattle::GetOptionValueArray(WORD wOptionIndex, COptionValueArray& OptionValueArray)
{
	return m_pIServiceFunction->GetOptionValueArray(m_BattleConfig.cbEmployScene, wOptionIndex, OptionValueArray);
}

//Լս����
VOID CTableFrameBattle::SetBattleRule(DWORD dwAddRules, DWORD dwRemoveRules)
{
	//��ӹ���
	m_dwBattleRule |= dwAddRules;

	//�Ƴ�����
	m_dwBattleRule &= ~dwRemoveRules;	
}

//��Ϸ�¼�
bool CTableFrameBattle::OnEventSocketGame(WORD wSubCmdID, VOID* pData, WORD wDataSize, IServerUserItem* pIServerUserItem)
{
	//��ܵ���
	if (__super::OnEventSocketGame(wSubCmdID, pData, wDataSize, pIServerUserItem))
	{
		return true;
	}

	return true;
}

//����¼�
bool CTableFrameBattle::OnEventSocketFrame(WORD wSubCmdID, VOID* pData, WORD wDataSize, IServerUserItem* pIServerUserItem)
{
	//��ܵ���
	if (__super::OnEventSocketFrame(wSubCmdID, pData, wDataSize, pIServerUserItem))
	{
		return true;
	}

	switch (wSubCmdID)
	{
	case SUB_GF_MODIFY_CONFIG:		//�޸�����
	{
		//��ȡ����
		auto pModifyConfig = (CMD_GF_ModifyConfig*)pData;

		//У��״̬
		if (m_dwBattleNum != pModifyConfig->dwBattleNum) return false;
		if (pIServerUserItem->GetUserID() != m_dwTableOwnerID) return false;

		//��������
		if (pModifyConfig->wOptionMask == OPTION_MASK_VOICE)
		{
			m_BattleConfig.cbAllowVoice = pModifyConfig->cbOptionValue;
		}	

		//��������
		CMD_BS_S_ModifyTable ModifyBattleTable;
		ZeroMemory(&ModifyBattleTable, sizeof(ModifyBattleTable));

		//������Ϣ
		ModifyBattleTable.dwUserID = m_dwTableOwnerID;
		ModifyBattleTable.dwBattleNum = m_dwBattleNum;
		CopyMemory(&ModifyBattleTable.BattleConfig, &m_BattleConfig, sizeof(ModifyBattleTable.BattleConfig));

		//������Ϣ
		WORD wPacketSize = sizeof(ModifyBattleTable) - sizeof(ModifyBattleTable.szErrorMessage);
		m_pITCPSocketBattle->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_MODIFY_TABLE, &ModifyBattleTable, wPacketSize, 0);

		//����֪ͨ
		NotifyTableConfig();

		return true;
	}
	case SUB_GF_LAUNCH_DISMISS:		//�����ɢ
	{
		//�ж�����
		if (CServerRule::IsAllowAgreementDismiss(m_pGameServiceOption->dwServerRule) == false)
		{
			return false;
		}

		//�ж�״̬
		if (m_dwLaunchUserID != 0) return true;
		if (m_wFinishCount == 0) return true;

		//��ȡ�û�
		tagBattleUserItem* pBattleUserItem = GetBattleUserItem(pIServerUserItem->GetUserID());
		if (pBattleUserItem == NULL || pBattleUserItem->cbUserStatus == US_LOOKON) return true;

		//���ñ���
		m_dwLaunchTime = (DWORD)time(NULL);
		m_dwLaunchUserID = pIServerUserItem->GetUserID();
		m_cbBallotResult[pIServerUserItem->GetChairID()] = 1;

		//����ṹ
		CMD_GF_DismissNotify DismissNotify;
		ZeroMemory(&DismissNotify, sizeof(DismissNotify));

		//���ñ���
		DismissNotify.dwUserID = m_dwLaunchUserID;
		DismissNotify.wResidueTime = TIME_BATTLE_AGREEMENT_DISMISS;

		//��ȡ����
		if (m_pIEventCustomFrameTime != NULL)
		{
			DismissNotify.wResidueTime = m_pIEventCustomFrameTime->GetAgreementDismissTime();
		}

		//ͳ���û�
		for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
		{
			//��ȡ����
			pBattleUserItem = &m_BattleUserItemArray[i];
			if (pBattleUserItem->cbUserStatus == US_LOOKON) continue;

			//�����û�
			DismissNotify.wNeedAgreeCount++;
			DismissNotify.dwUserIDList[DismissNotify.wUserCount++] = pBattleUserItem->dwUserID;
		}

		//����ͬ������
		if (m_pIEventBattleItem != NULL)
		{
			DismissNotify.wNeedAgreeCount = m_pIEventBattleItem->GetDismissAgreeCount();
			if (DismissNotify.wNeedAgreeCount > DismissNotify.wUserCount) DismissNotify.wNeedAgreeCount = DismissNotify.wUserCount;
		}

		//��Ϸ�û�
		for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
		{
			//��ȡ����
			pBattleUserItem = &m_BattleUserItemArray[i];
			if (pBattleUserItem->cbUserStatus == US_LOOKON) continue;

			//��ȡ�û�
			IServerUserItem* pISendUserItem = GetTableUserItem(pBattleUserItem->wChairID);
			if ((pISendUserItem == NULL) || (pISendUserItem->IsClientReady() == false)) continue;

			//��������
			m_pIServiceFunction->SendDataToUser(pISendUserItem, MDM_GF_FRAME, SUB_GF_DISMISS_NOTIFY, &DismissNotify, sizeof(DismissNotify));
		}

		//��Ϸ��ͣ
		PauseGame();

		//���ö�ʱ��
		SetGameTimer(IDI_AGREEMENT_DISMISS, (DismissNotify.wResidueTime + 5) * 1000L, 1, NULL);

		return true;
	}
	case SUB_GF_BALLOT_DISMISS:		//ͶƱ��ɢ
	{
		//����У��
		ASSERT(wDataSize == sizeof(CMD_GF_BallotDismiss));
		if (wDataSize != sizeof(CMD_GF_BallotDismiss)) return false;

		//״̬�ж�
		if (m_dwLaunchUserID == 0) return true;

		//��ȡ����
		CMD_GF_BallotDismiss* pBallotDismiss = (CMD_GF_BallotDismiss*)pData;

		//ִ��ͶƱ
		return PerformBallotDismiss(pIServerUserItem->GetChairID(), pIServerUserItem->GetUserID(), pBallotDismiss->bAgreeDismiss);
	}
	}

	return true;
}

//���Կ�ʼ
bool CTableFrameBattle::TryStartGame()
{
	if (m_BattleConfig.lConsumeCount > 0 && m_bDeductCost == false)
	{
		////����֧��
		//if (m_BattleConfig.wPayType == PAY_TYPE_OWNER)
		//{
		//	//����ṹ
		//	DBR_GR_DeductBattleCost DeductBattleCost;
		//	ZeroMemory(&DeductBattleCost, sizeof(DeductBattleCost));

		//	//��������
		//	DeductBattleCost.wTableID = m_wTableID;
		//	DeductBattleCost.dwClubID = m_dwClubID;
		//	DeductBattleCost.dwUserID = m_dwTableOwnerID;
		//	DeductBattleCost.lConsumeCount = m_BattleConfig.lConsumeCount;
		//	DeductBattleCost.cbCurrencyKind = m_BattleConfig.cbCurrencyKind;

		//	//Ͷ������
		//	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DEDUCT_BATTLE_COST, m_dwBattleNum, 0L, 0L, &DeductBattleCost, sizeof(DeductBattleCost));

		//	return false;			
		//}

		//AA֧��
		if (m_BattleConfig.wPayType == PAY_TYPE_USER)
		{
			//��������
			tagBattleUser BattleUser;
			tagBattleScoreItem* pUserScoreItem = NULL;

			//����ṹ
			DBR_GR_DeductBattleCostAA DeductBattleCost;
			ZeroMemory(&DeductBattleCost, sizeof(DeductBattleCost));

			//��������
			DeductBattleCost.wTableID = m_wTableID;
			DeductBattleCost.dwClubID = m_dwClubID;
			DeductBattleCost.lConsumeCount = m_BattleConfig.lConsumeCount;
			DeductBattleCost.cbCurrencyKind = m_BattleConfig.cbCurrencyKind;

			//�����û�
			for (WORD i = 0; i < m_wChairCount; i++)
			{
				//��ȡ�û�
				IServerUserItem* pIServerUserItem = GetTableUserItem(i);
				if (pIServerUserItem == NULL) continue;
				if (pIServerUserItem->GetUserStatus() != US_READY) continue;

				//���ñ���
				BattleUser.dwUserID = pIServerUserItem->GetUserID();

				//��������
				CopyMemory(&DeductBattleCost.BattleUser[DeductBattleCost.wUserCount++], &BattleUser, sizeof(BattleUser));
			}

			//����δ�۷�
			if (DeductBattleCost.wUserCount > 0)
			{
				//Ͷ������
				WORD wStructSize = DeductBattleCost.wUserCount * sizeof(tagBattleUser);
				WORD wSendDataSize = sizeof(DeductBattleCost) - sizeof(DeductBattleCost.BattleUser) + wStructSize;
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DEDUCT_BATTLE_COSTAA, m_dwBattleNum, 0L, 0L, &DeductBattleCost, wSendDataSize);

				return false;
			}
		}
	}

	return __super::TryStartGame();
}

//��ʼ�¼�
bool CTableFrameBattle::OnEventGameStart()
{
	//У����
	if (m_dwBattleNum == 0) return false;
		
	//����Ϣ
	if (++m_wFinishCount == 1)
	{
		//���ñ���
		m_dwStartTime = (DWORD)time(NULL);
		m_cbBattleState = BATTLE_STATE_PLAY;
	}

	//�����û�
	DBR_GR_BattleLock BattleLock;
	ZeroMemory(&BattleLock, sizeof(BattleLock));

	//Լս��Ϣ
	BattleLock.dwBattleNum = m_dwBattleNum;
	BattleLock.cbScoreKind = m_BattleConfig.cbScoreKind;

	//�û�����
	tagBattleUserItem BattleUserItem;
	ZeroMemory(&BattleUserItem, sizeof(BattleUserItem));

	//�����û�
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//��ȡ�û�
		IServerUserItem* pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;
		if (GetBattleUserItem(pIServerUserItem->GetUserID()) != NULL) continue;

		//������Ϣ
		BattleUserItem.wChairID = i;
		BattleUserItem.dwUserID = pIServerUserItem->GetUserID();
		BattleUserItem.dwGameID = pIServerUserItem->GetGameID();
		BattleUserItem.cbGender = pIServerUserItem->GetUserInfo()->cbGender;
		StringCchCopy(BattleUserItem.szNickName, CountArray(BattleUserItem.szNickName), pIServerUserItem->GetAccounts());

		//������
		m_BattleUserItemArray.Add(BattleUserItem);

		//����û�
		BattleLock.dwUserID[BattleLock.wUserCount++] = BattleUserItem.dwUserID;
	}

	//Ͷ������
	if (BattleLock.wUserCount > 0)
	{
		WORD wStructSize = BattleLock.wUserCount * sizeof(BattleLock.dwUserID[0]);
		WORD wPacketSize = sizeof(BattleLock) - sizeof(BattleLock.dwUserID) + wStructSize;
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_BATTLE_LOCK, m_dwBattleNum, 0L, 0L, &BattleLock, wPacketSize);
	}
	
	//��������
	for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
	{
		//��ȡ��Ϣ
		tagBattleUserItem* pBattleUserItem = &m_BattleUserItemArray[i];

		//��ȡ�û�
		IServerUserItem* pIServerUserItem = GetTableUserItem(pBattleUserItem->wChairID);
		if (pIServerUserItem == NULL) continue;

		//ͷ����Ϣ
		pBattleUserItem->wFaceID = pIServerUserItem->GetUserInfo()->wFaceID;
		pBattleUserItem->dwCustomID = pIServerUserItem->GetUserInfo()->dwCustomID;
	}

	//���ͳɼ�
	NotifyTableGrade();

	//����״̬		
	NotifyTableParam();

	//�رն�ʱ��
	KillGameTimer(IDI_TABLE_DISMISS);

	return true;
}

//����ʱ��
bool CTableFrameBattle::OnEventGameConclude()
{
	//У����
	if (m_dwBattleNum == 0) return false;
	
	//���ͳɼ�
	NotifyTableGrade();

	//���Ӳ���
	NotifyTableParam();

	//���Խ�ɢ
	if (IsBattleFinished() == true)
	{
		//Լս����
		DismissBattleTable(!m_bReturnCost);
	}	
	else
	{
		//��ɢʱ��
		DWORD dwDismissTime = TIME_BATTLE_TABLE_DISMISS;
		if (m_pIEventCustomFrameTime != NULL)
		{
			dwDismissTime = m_pIEventCustomFrameTime->GetBattleDismissTime();
		}

		//���ö�ʱ��
		SetGameTimer(IDI_TABLE_DISMISS, dwDismissTime * 1000L, -1, 0);		
	}

	return true;
}

//�û�����
bool CTableFrameBattle::OnActionUserSitdown(IServerUserItem* pIServerUserItem)
{
	if (m_dwBattleNum != 0)
	{
		//�رն�ʱ��
		//KillGameTimer(IDI_TABLE_DISMISS);

		//����֪ͨ
		if (m_BattleConfig.wNotifyMask & NOTIFY_MASK_TABLE_USER)
		{
			//����ṹ
			CMD_BS_S_UserSitdown UserSitdown;
			ZeroMemory(&UserSitdown, sizeof(UserSitdown));

			//���ñ���
			UserSitdown.dwBattleNum = m_dwBattleNum;
			UserSitdown.dwUserID = pIServerUserItem->GetUserID();

			//�û���Ϣ
			UserSitdown.TableUserItem.wChairID = pIServerUserItem->GetChairID();
			UserSitdown.TableUserItem.dwGameID = pIServerUserItem->GetGameID();
			UserSitdown.TableUserItem.dwUserID = pIServerUserItem->GetUserID();
			UserSitdown.TableUserItem.wFaceID = pIServerUserItem->GetUserInfo()->wFaceID;
			UserSitdown.TableUserItem.cbGender = pIServerUserItem->GetUserInfo()->cbGender;
			UserSitdown.TableUserItem.dwCustomID = pIServerUserItem->GetUserInfo()->dwCustomID;			
			UserSitdown.TableUserItem.wGameOrder = g_GameLevelManager->CalcGameOrder(pIServerUserItem->GetExperience());
			UserSitdown.TableUserItem.wMemberOrder = g_GameLevelManager->CalcUserOrder(pIServerUserItem->GetMemberPoint());
			StringCchCopy(UserSitdown.TableUserItem.szNickName, CountArray(UserSitdown.TableUserItem.szNickName), pIServerUserItem->GetAccounts());
			StringCchCopy(UserSitdown.TableUserItem.szUserHeadUrl, CountArray(UserSitdown.TableUserItem.szUserHeadUrl), pIServerUserItem->GetUserInfo()->szUserHeadUrl);

			//������Ϣ		
			m_pITCPSocketBattle->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_USER_SITDOWN, &UserSitdown, sizeof(UserSitdown), 0);
		}
	}

	return true;
}

//�û�����
bool CTableFrameBattle::OnActionUserStandup(IServerUserItem* pIServerUserItem)
{
	//����֪ͨ
	if (m_dwBattleNum != 0)
	{
		//Լս�û���Ϣ
		tagBattleUserItem* pBattleUserItem = GetBattleUserItem(pIServerUserItem->GetUserID());
		if (m_wFinishCount == 0 || pBattleUserItem == NULL)
		{
			//����֪ͨ
			if (m_BattleConfig.wNotifyMask & NOTIFY_MASK_TABLE_USER)
			{
				//����ṹ
				CMD_BS_S_UserStandup UserStandup;
				ZeroMemory(&UserStandup, sizeof(UserStandup));

				//���ñ���
				UserStandup.dwBattleNum = m_dwBattleNum;
				UserStandup.wChairID = pIServerUserItem->GetChairID();				
				UserStandup.dwUserID = pIServerUserItem->GetUserID();

				//������Ϣ		
				m_pITCPSocketBattle->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_USER_STANDUP, &UserStandup, sizeof(UserStandup), 0);
			}

			//�����û�
			//if (pBattleUserItem && pBattleUserItem->wGameCount == 0)
			//{
			//	//�����û�
			//	DBR_GR_BattleUnlock BattleUnlock;
			//	ZeroMemory(&BattleUnlock, sizeof(BattleUnlock));

			//	//���÷���
			//	BattleUnlock.dwBattleNum = m_dwBattleNum;
			//	BattleUnlock.dwUserID[BattleUnlock.wUserCount++] = pBattleUserItem->dwUserID;					

			//	//Ͷ������
			//	WORD wStructSize = BattleUnlock.wUserCount * sizeof(BattleUnlock.dwUserID[0]);
			//	WORD wPacketSize = sizeof(BattleUnlock) - sizeof(BattleUnlock.dwUserID) + wStructSize;
			//	m_pIKernelDataBaseEngine->PostDataBaseRequest(m_dwBattleNum, DBR_GR_BATTLE_UNLOCK, 0L, 0L, &BattleUnlock, wPacketSize);					
			//}
		}
	}

	return true;
}

//�Թ�У��
bool CTableFrameBattle::EfficacyUserLookon(WORD wChairID, IServerUserItem* pIServerUserItem)
{
	//�������
	if (!__super::EfficacyUserLookon(wChairID, pIServerUserItem))
	{
		return false;
	}

	//Լս״̬
	if (m_bBattleService == false)
	{
		LPCTSTR pszDescribe = CWHService::FormatMessage(MIM_SERVER_0089, TEXT("ϵͳά���У�Լս������ʱֹͣ��������ά����ɺ����ԣ�"));
		SendRequestFailure(pIServerUserItem, pszDescribe, 0);
		return false;
	}

	//��ֹ�Թ�
	if (m_BattleConfig.cbLookonMode == LOOKON_MODE_FORBID)
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0091, TEXT("��Ǹ����ǰԼս���䲻�����Թۣ�")), 0);
		return false;
	}
	

	return true;
}

//����У��
bool CTableFrameBattle::EfficacyUserSitdown(WORD wChairID, IServerUserItem* pIServerUserItem)
{
	if (!__super::EfficacyUserSitdown(wChairID, pIServerUserItem))
	{
		return false;
	}

	//�û���Ϣ
	auto pServerUserInfo = pIServerUserItem->GetUserInfo();

	//Լս״̬
	if (m_bBattleService == false)
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0089, TEXT("ϵͳά���У�Լս������ʱֹͣ��������ά����ɺ����ԣ�")), 0);
		return false;
	}

	//δ��������
	if (m_dwBattleNum == 0)
	{
		//������Ϣ
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0104, TEXT("��Ǹ����������δ�����������ܼ��룡")), RFC_NULL);

		return false;
	}

	//Ӯ��֧��
	if (m_BattleConfig.wPayType == PAY_TYPE_WINNER)
	{
		//��������
		if (m_BattleConfig.cbCurrencyKind == CURRENCY_KIND_CARD && pServerUserInfo->lUserCard < m_BattleConfig.lConsumeCount)
		{
			//������Ϣ
			SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0107, L"��Ǹ�������ϵķ������������ܼ��룡"), RFC_NULL);

			return false;
		}		
	}

	//AA֧��
	if (m_BattleConfig.wPayType == PAY_TYPE_USER && m_bDeductCost==false)
	{
		//��Ҳ���
		if (m_BattleConfig.cbCurrencyKind == CURRENCY_KIND_GOLD &&  pServerUserInfo->lGold < m_BattleConfig.lConsumeCount)
		{
			//������Ϣ
			SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0106, L"��Ǹ�������ϵĽ�Ҳ���֧���볡�ѣ����ܼ��룡"), RFC_NULL);

			return false;
		}

		//��������
		if (m_BattleConfig.cbCurrencyKind == CURRENCY_KIND_CARD && pServerUserInfo->lUserCard < m_BattleConfig.lConsumeCount)
		{
			//������Ϣ
			SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0107, L"��Ǹ�������ϵķ������������ܼ��룡"), RFC_NULL);

			return false;
		}
	}	

	//ͬIP��GPS����<100m
	if (m_BattleConfig.cbAllowSameIP == FALSE && m_wFinishCount == 0)
	{
		//��������
		auto pServerUserItem = (CServerUserItem*)pIServerUserItem;

		//ͬIP���
		for (WORD i = 0; i < m_wChairCount; i++)
		{
			//��ȡ�û�
			auto pTableUserItem = (CServerUserItem*)GetTableUserItem(i);
			if (pTableUserItem == NULL) continue;

			//��ַ����				
			if (pServerUserItem == pTableUserItem) continue;
			if (pServerUserItem->GetClientIP() != pTableUserItem->GetClientIP()) continue;

			//������Ϣ
			SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0108, TEXT("��������IP���������������ͬ���޷����뱾��Լս��")), RFC_NULL);

			return false;
		}

		//��ȡ����
		auto dGPSLng1 = pServerUserItem->GetGlobalPosLng();
		auto dGPSLat1 = pServerUserItem->GetGlobalPosLat();

		//У������
		if (IsGPSValid(dGPSLat1, dGPSLng1) == false)
		{
			//������Ϣ
			SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0109, TEXT("�������ĵ���λ�û�ȡ�쳣���޷����뱾��Լս�����GPS��λ���ٴγ��ԣ�")), RFC_NULL);

			return false;
		}

		//λ�ü��
		for (WORD i = 0; i < m_wChairCount; i++)
		{
			//��ȡ�û�
			CServerUserItem* pTableUserItem = (CServerUserItem*)GetTableUserItem(i);
			if (pTableUserItem == NULL) continue;

			//��ַ����				
			if (pServerUserItem == pTableUserItem) continue;

			//��ȡ��γ��
			auto dGPSLng2 = pTableUserItem->GetGlobalPosLng();
			auto dGPSLat2 = pTableUserItem->GetGlobalPosLat();

			//�������
			if (GetGPSDistance(dGPSLat1, dGPSLng1, dGPSLat2, dGPSLng2) <= MIN_GPS_DISTANCE)
			{
				//������Ϣ
				LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0110, TEXT("�������ĵ���λ�ø���������ҵľ���С��%d�ף��޷����뱾��Լս��"), MIN_GPS_DISTANCE);
				SendRequestFailure(pIServerUserItem, pszMessage, RFC_NULL);

				return false;
			}
		}
	}

	return true;
}

//����У��
bool CTableFrameBattle::EfficacyUserStandup(WORD wChairID, IServerUserItem* pIServerUserItem, bool bInitiative)
{
	if (!__super::EfficacyUserStandup(wChairID, pIServerUserItem, bInitiative))
	{
		return false;
	}
	
	//�û�����
	BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

	//Լս�йܴ���
	if (bInitiative == true && GetBattleUserItem(pIServerUserItem->GetUserID())!=NULL)
	{
		//�ж�״̬
		if (cbUserStatus != US_LOOKON && m_dwBattleNum != 0 && m_wFinishCount > 0)
		{
			//if (pIServerUserItem->IsTrusteeUser() == false)
			//{
			//	pIServerUserItem->SetTrusteeUser(true);
			//	pIServerUserItem->SetClientReady(true);

			//	//����֪ͨ
			//	if (m_pITableUserAction != NULL) m_pITableUserAction->OnActionUserOffLine(pIServerUserItem);
			//}

			if (pIServerUserItem->IsClientReady() == false)
			{
				pIServerUserItem->SetClientReady(true);
			}

			//SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0121, TEXT("��Ǹ��Լս��δ��ɣ�����ʱ�����˳���")), 0);//kk guan

			//return false;//kk guan
		}
	}

	return true;
}

//ʱ���¼�
bool CTableFrameBattle::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	if (__super::OnEventTimer(dwTimerID, dwBindParameter) == false)
	{
		switch (dwTimerID)
		{
		case IDI_TABLE_DISMISS: 	//���ӽ�ɢ
		{
			//��ɢ��Ϸ
			if (IsGameStarted() == true)
			{
				DismissGame();
			}

			//Լս����
			if (m_dwBattleNum != 0)
			{
				//��ʱ��ɢ
				if (m_wFinishCount==0)
				{
					//��������Ӳ���ɢ
					if (m_dwClubID != 0) return true;

					//������Ϣ
					TCHAR szMessage[128] = L"";
					StringCchCopy(szMessage, CountArray(szMessage), CWHService::FormatMessage(MIM_SERVER_0070, TEXT("����ȴ�ʱ���ѵ�����ǰ��Ϸ���������������Զ���ɢ��")));

					//��ϢȺ��
					for (WORD i = 0; i < m_wChairCount; i++)
					{
						//��ȡ�û�
						IServerUserItem* pIServerUserItem = GetTableUserItem(i);
						if ((pIServerUserItem == NULL) || (pIServerUserItem->IsClientReady() == false)) continue;

						//��������							
						m_pIServiceFunction->SendGameMessage(pIServerUserItem, szMessage, SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);
					}									
				}	

				//Լս���
				BattleFinish(BFR_WAIT_EXPIRE);
			}

			return true;
		}
		case IDI_AGREEMENT_DISMISS:	//Э���ɢ
		{
			//ɾ����ʱ��
			KillGameTimer(IDI_AGREEMENT_DISMISS);

			for (WORD i = 0; i < m_wChairCount; i++)
			{
				//��������
				WORD wChairID = i;
				DWORD dwUserID = 0;

				//��ȡ�û�
				IServerUserItem* pIServerUserItem = GetTableUserItem(wChairID);
				if (pIServerUserItem != NULL) dwUserID = pIServerUserItem->GetUserID();

				//�����û�
				if (dwUserID == 0)
				{
					for (INT_PTR j = 0; j < m_BattleUserItemArray.GetCount(); j++)
					{
						if (wChairID == m_BattleUserItemArray[j].wChairID)
						{
							dwUserID = m_BattleUserItemArray[j].dwUserID;
							break;
						}
					}
				}

				//״̬�ж�
				if (dwUserID != 0 && m_cbBallotResult[i] == 0)
				{
					//ִ��ͶƱ
					PerformBallotDismiss(wChairID, dwUserID, true);
				}
			}

			//���ñ���
			m_dwLaunchTime = 0;
			m_dwLaunchUserID = 0;
			ZeroMemory(m_cbBallotResult, sizeof(m_cbBallotResult));

			return true;
		}
		}
	}

	return true;
}

//�ն�
VOID CTableFrameBattle::OnEventClientReady(IServerUserItem* pIServerUserItem)
{
	//У����
	if (m_dwBattleNum != 0)
	{
		//���ͳɼ�
		NotifyTableGrade();

		//����֪ͨ
		NotifyTableConfig(pIServerUserItem->GetChairID());

		//����֪ͨ
		NotifyTableParam(pIServerUserItem->GetChairID());		
	}

	//��ɢ����
	if (m_dwLaunchUserID != 0) SendDimissNotify(pIServerUserItem);
}

//���ӳ�ʼ��
VOID CTableFrameBattle::OnEventTableFrameInit(tagTableFrameParameter& TableFrameParameter)
{
	//��ȡ�ӿ�
	m_pITCPSocketBattle = TableFrameParameter.pITCPSocketService;
	m_pIEventGameOption = QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink, IEventGameOption);
	m_pIEventBattleItem = QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink, IEventBattleItem);
}

//���Ӳ���
bool CTableFrameBattle::NotifyTableParam(WORD wChairID)
{
	//��������
	CMD_GF_TableParam GFTableParam;
	ZeroMemory(&GFTableParam, sizeof(GFTableParam));

	//������Ϣ
	GFTableParam.lBaseScore = m_lBaseScore;
	GFTableParam.dwOwnerID = m_dwTableOwnerID;
	GFTableParam.dwBattleNum = m_dwBattleNum;

	//������Ϣ
	GFTableParam.wFinishCount = m_wFinishCount;
	GFTableParam.wPlayCount = m_BattleConfig.wPlayCount;

	//��¼��ʶ
	GFTableParam.lBattleRecordID = m_lBattleRecordID;

	//��Ϸ�û�
	SendTableData(wChairID, SUB_GF_TABLE_PARAM, &GFTableParam, sizeof(GFTableParam), MDM_GF_FRAME);

	//�Թ��û�
	SendLookonData(wChairID, SUB_GF_TABLE_PARAM, &GFTableParam, sizeof(GFTableParam), MDM_GF_FRAME);

	//״̬�ж�
	if ((m_BattleConfig.wNotifyMask & NOTIFY_MASK_TABLE_PARAM) != 0 && m_wFinishCount > 0 && wChairID==INVALID_CHAIR)
	{
		//����ṹ
		CMD_BS_S_TableParam SBTableParam;
		ZeroMemory(&SBTableParam, sizeof(SBTableParam));

		//������Ϣ		
		SBTableParam.dwBattleNum = m_dwBattleNum;
		SBTableParam.dwStartTime = m_dwStartTime;
		SBTableParam.wFinishCount = m_wFinishCount;
		SBTableParam.cbGameStatus = IsGameStarted() ? TRUE : FALSE;

		//��������
		m_pITCPSocketBattle->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_TABLE_PARAM, &SBTableParam, sizeof(SBTableParam), 0);
	}

	return true;
}

//Լս����
bool CTableFrameBattle::NotifyTableConfig(WORD wChairID)
{
	//��������
	CMD_GF_TableConfig TableConfig = {};
	
	//������Ϣ
	TableConfig.dwBattleNum = m_dwBattleNum;
	TableConfig.lWinnerScore = m_BattleConfig.lWinnerScore;
	TableConfig.lConsumeCount = m_BattleConfig.lConsumeCount;
	CopyMemory(&TableConfig.BattleConfig, &m_BattleConfig, sizeof(TableConfig.BattleConfig));

	//��Ϸ�û�
	SendTableData(wChairID, SUB_GF_TABLE_CONFIG, &TableConfig, sizeof(TableConfig), MDM_GF_FRAME);

	//�Թ��û�	
	SendLookonData(wChairID, SUB_GF_TABLE_CONFIG, &TableConfig, sizeof(TableConfig), MDM_GF_FRAME);

	return true;
}

//���ӳɼ�
bool CTableFrameBattle::NotifyTableGrade()
{
	//�ж�����
	if (m_BattleUserItemArray.GetCount() == 0)
	{
		return true;
	}

	//��ֹ���ͳɼ�
	if (CBattleRule::IsForbidSendGrade(m_pGameServiceOption->dwBattleRule))
	{
		return true;
	}

	//��������
	WORD wPacketSize = 0;
	BYTE cbDataBuffer[TABLE_USER_COUNT * sizeof(tagUserGradeItem)];

	//��������
	for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
	{
		if (wPacketSize + sizeof(tagUserGradeItem) > sizeof(cbDataBuffer))
		{
			//��������
			SendTableData(INVALID_CHAIR, SUB_GF_TABLE_GRADE, cbDataBuffer, wPacketSize, MDM_GF_FRAME);

			//�����Թ�
			SendLookonData(INVALID_CHAIR, SUB_GF_TABLE_GRADE, cbDataBuffer, wPacketSize, MDM_GF_FRAME);

			wPacketSize = 0;
		}

		//��ȡ����
		tagUserGradeItem* pUserGradeItem = (tagUserGradeItem*)(cbDataBuffer + wPacketSize);

		//���óɼ�
		pUserGradeItem->wChairID = m_BattleUserItemArray[i].wChairID;
		pUserGradeItem->dwUserID = m_BattleUserItemArray[i].dwUserID;
		pUserGradeItem->lAddupScore = m_BattleUserItemArray[i].lScore;
		StringCchCopy(pUserGradeItem->szNickName, CountArray(pUserGradeItem->szNickName), m_BattleUserItemArray[i].szNickName);

		//λ��ƫ��
		wPacketSize += sizeof(tagUserGradeItem);
	}

	//��������
	if (wPacketSize > 0)
	{
		//��������
		SendTableData(INVALID_CHAIR, SUB_GF_TABLE_GRADE, cbDataBuffer, wPacketSize, MDM_GF_FRAME);

		//�����Թ�
		SendLookonData(INVALID_CHAIR, SUB_GF_TABLE_GRADE, cbDataBuffer, wPacketSize, MDM_GF_FRAME);
	}

	return true;
}

//Լսս��
bool CTableFrameBattle::NotifyTableBattle(bool bRecordBattle)
{
	//�ж�����
	if (m_BattleUserItemArray.GetCount() == 0)
	{
		return true;
	}

	//�����û�
	DBR_GR_BattleUnlock BattleUnlock;
	ZeroMemory(&BattleUnlock, sizeof(BattleUnlock));

	//���÷���
	BattleUnlock.dwBattleNum = m_dwBattleNum;

	//�Ƴ��û�
	for (INT_PTR i = m_BattleUserItemArray.GetCount() - 1; i >= 0; i--)
	{
		//��ȡ����
		tagBattleUserItem* pBattleUserItem = &m_BattleUserItemArray[i];

		//����û�
		BattleUnlock.dwUserID[BattleUnlock.wUserCount++] = pBattleUserItem->dwUserID;
	}

	//�����û�
	if (BattleUnlock.wUserCount > 0)
	{
		//Ͷ������
		WORD wStructSize = BattleUnlock.wUserCount * sizeof(BattleUnlock.dwUserID[0]);
		WORD wPacketSize = sizeof(BattleUnlock) - sizeof(BattleUnlock.dwUserID) + wStructSize;
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_BATTLE_UNLOCK, m_dwBattleNum, 0L, 0L, &BattleUnlock, wPacketSize);
	}

	//kk jia start
		//����˰��
	//ս����¼
	if (bRecordBattle && CBattleRule::IsForbidBattleRecord(m_pGameServiceOption->dwBattleRule) == false)//kk 
	{
		//ֻ�۴�Ӯ��
		WORD wUserCount = (WORD)m_BattleUserItemArray.GetCount();
		BYTE bDaWin = 0;
		SCORE lDaWinScore = m_BattleUserItemArray[0].lScore;
		for (BYTE i = 0; i < wUserCount; i++)
		{
			SCORE lScore = m_BattleUserItemArray[i].lScore;
			lDaWinScore = m_BattleUserItemArray[bDaWin].lScore;
			if (lScore > lDaWinScore)
			{
				bDaWin = i;

			}
		}	
		
		SCORE lScore = m_BattleUserItemArray[bDaWin].lScore;
		if (lScore > 0)
		{
			SCORE lClubRevenue = CalcCustomRevenue(lScore);
			m_BattleUserItemArray[bDaWin].lClubRevenue = lClubRevenue;
			m_BattleUserItemArray[bDaWin].lScore -= lClubRevenue;

			//
			IServerUserItem* pIServerUserItem = GetTableUserItem(m_BattleUserItemArray[bDaWin].wChairID);
			if (pIServerUserItem != NULL)
			{
				//CString s;
				//s.Format(_T("a17_1  i:%d,lScore:%I64d,lClubRevenue:%I64d"), bDaWin, pIServerUserItem->GetUserScore(),  lClubRevenue);
				//CTraceService::TraceString(s, TraceLevel_Exception);

				pIServerUserItem->WriteUserScore(-lClubRevenue,0,0,0, ScoreType_Null);

				//s.Format(_T("a17_2  i:%d,lScore:%I64d,lClubRevenue:%I64d"), bDaWin, pIServerUserItem->GetUserScore(), lClubRevenue);
				//CTraceService::TraceString(s, TraceLevel_Exception);
			}

			/*CString s;
			s.Format(_T("a17  i:%d,lScore:%I64d,m_BattleUserItemArray[i].lScore:%I64d,lClubRevenue:%I64d, lRevenue:%I64d,m_BattleUserItemArray[i].lClubRevenue:%I64d"), i, lScore, m_BattleUserItemArray[i].lScore, lClubRevenue, m_BattleUserItemArray[i].lClubRevenue);
			CTraceService::TraceString(s, TraceLevel_Exception);*/
		}

		
	}
	//kk jia end
	//��д����
	if (CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule) == false)
	{
		//��������
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore, sizeof(WriteGameScore));

		//������Ϣ
		tagLockScoreInfo LockScoreInfo;
		ZeroMemory(&LockScoreInfo, sizeof(LockScoreInfo));
		LockScoreInfo.lMaxLockScore = m_pGameServiceOption->lMaxLockScore;
		LockScoreInfo.lLessLockScore = m_pGameServiceOption->lLessLockScore;
		LockScoreInfo.lRestrictScore = m_pGameServiceOption->lRestrictScore;

		//�����û�
		for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
		{
			//��ȡ�û�
			IServerUserItem* pIServerUserItem = GetTableUserItem(m_BattleUserItemArray[i].wChairID);
			if (pIServerUserItem != NULL)
			{
				//��ȡ����
				tagVariationScore VariationScore;
				ZeroMemory(&VariationScore, sizeof(VariationScore));
				pIServerUserItem->DistillVariation(VariationScore, LockScoreInfo);

				//�û���Ϣ
				WriteGameScore.dwUserID = pIServerUserItem->GetUserID();
				WriteGameScore.dwClientIP = pIServerUserItem->GetClientIP();
				WriteGameScore.dwPlayTimeCount = VariationScore.dwPlayTimeCount;

				//������Ϣ
				WriteGameScore.lRevenue = VariationScore.lRevenue;//kk
				WriteGameScore.lUnLockScore = VariationScore.lUnLockScore;

				//������Ϣ
				WriteGameScore.lScore = VariationScore.VariationScore.lScore;
				WriteGameScore.lWinCount = VariationScore.VariationScore.lWinCount;
				WriteGameScore.lLostCount = VariationScore.VariationScore.lLostCount;
				WriteGameScore.lDrawCount = VariationScore.VariationScore.lDrawCount;
				WriteGameScore.lFleeCount = VariationScore.VariationScore.lFleeCount;
				WriteGameScore.lExperience = VariationScore.VariationScore.lExperience;
				WriteGameScore.lWinStreak = VariationScore.VariationScore.lWinStreak;
				WriteGameScore.lMaxWinStreak = VariationScore.VariationScore.lMaxWinStreak;

				//Ͷ������
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_GAME_SCORE, WriteGameScore.dwUserID, 0, pIServerUserItem->GetTokenID(), &WriteGameScore, sizeof(WriteGameScore));
			}
		}
	}

	//��������(δ�˷�)
	if (m_bReturnCost == false)
	{
		//��������	
		CBattleUserItemArray BattleUserItemArray;

		//��������
		BattleUserItemArray.Append(m_BattleUserItemArray);
		m_BattleUserItemArray.RemoveAll();

		//�ɼ�����	
		WORD wRankCount = (WORD)BattleUserItemArray.GetCount();
		for (WORD wRankID = 1; wRankID <= wRankCount; wRankID++)
		{
			//��������		
			INT_PTR nIndex = 0;
			LONGLONG lScore = BattleUserItemArray[0].lScore;

			//���Ҹ߷�
			for (INT_PTR i = 1; i < BattleUserItemArray.GetCount(); i++)
			{
				//��������
				if (BattleUserItemArray[i].lScore > lScore)
				{
					//���ñ���
					nIndex = i;
					lScore = BattleUserItemArray[i].lScore;
				}

				//�������
				if (BattleUserItemArray[i].lScore == lScore)
				{
					//ʤ�ֲ���
					if (BattleUserItemArray[i].lWinCount > BattleUserItemArray[nIndex].lWinCount)
					{
						//���ñ���
						nIndex = i;
						lScore = BattleUserItemArray[i].lScore;
					}
				}
			}

			//��������		
			BattleUserItemArray[nIndex].wRankID = wRankID;
			BattleUserItemArray[nIndex].cbUserStatus = US_NULL;

			//��ȡ�û�
			IServerUserItem* pIServerUserItem = GetTableUserItem(BattleUserItemArray[nIndex].wChairID);
			if (pIServerUserItem != NULL)
			{
				//����״̬
				BattleUserItemArray[nIndex].cbUserStatus = pIServerUserItem->GetUserStatus();
			}

			//����û�
			m_BattleUserItemArray.Add(BattleUserItemArray[nIndex]);

			//�Ƴ���¼
			BattleUserItemArray.RemoveAt(nIndex);
		}
	}

	
	//����֧��-��Ӯ��֧��
	if (m_BattleConfig.cbCurrencyKind == CURRENCY_KIND_CARD && m_BattleConfig.wPayType == PAY_TYPE_WINNER)
	{
		//��ȡ��Ӯ��
		auto pBattleUserItem = &m_BattleUserItemArray[0];
		if (pBattleUserItem != NULL && pBattleUserItem->wRankID == 1)
		{
			//����ṹ
			DBR_GR_DeductBattleCost DeductBattleCost;
			ZeroMemory(&DeductBattleCost, sizeof(DeductBattleCost));

			//��������
			DeductBattleCost.wTableID = m_wTableID;
			DeductBattleCost.dwClubID = m_dwClubID;
			DeductBattleCost.wPayType = m_BattleConfig.wPayType;
			DeductBattleCost.dwUserID = pBattleUserItem->dwUserID;
			DeductBattleCost.lConsumeCount = m_BattleConfig.lConsumeCount;
			DeductBattleCost.cbCurrencyKind = m_BattleConfig.cbCurrencyKind;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DEDUCT_BATTLE_COST, DeductBattleCost.dwUserID, 0L, 0L, &DeductBattleCost, sizeof(DeductBattleCost));
		}
	}		

	//���֧��-Ӯ�ҽ���
	if (m_BattleConfig.cbCurrencyKind == CURRENCY_KIND_GOLD && m_BattleConfig.wPayType == PAY_TYPE_USER)
	{
		//����ṹ
		DBR_GR_BattleReward BattleReward = {};

		//����˰��
		auto pAdvancedOption = (tagAdvancedOption*)m_BattleConfig.cbAdvancedOption;
		auto lClubRevenue = (m_BattleConfig.lConsumeCount * m_BattleConfig.wPlayerCount) * pAdvancedOption->lRevenueRate / 1000;

		CString s;
		s.Format(_T("aaa1 lClubRevenue:%I64d , m_BattleConfig.lConsumeCount:%I64d , m_BattleConfig.wPlayerCount:%d"), lClubRevenue , m_BattleConfig.lConsumeCount , m_BattleConfig.wPlayerCount);
		CTraceService::TraceString(s, TraceLevel_Debug); 

		auto lSysRevenue =  (m_BattleConfig.lConsumeCount * m_BattleConfig.wPlayerCount)- m_BattleConfig.lWinnerScore*m_BattleConfig.wWinnerCount - lClubRevenue;

		for (int i = 0; i < m_BattleUserItemArray.GetCount(); i++)
		{
			auto pBattleUserItem = &m_BattleUserItemArray[i];
			if (pBattleUserItem == NULL || pBattleUserItem->wRankID != i + 1) continue;			

			//���ñ���
			BattleReward.dwUserID = pBattleUserItem->dwUserID;
			BattleReward.lRewardGold = m_BattleConfig.lWinnerScore;

			//��������
			pBattleUserItem->lRewardScore = m_BattleConfig.lWinnerScore;

			//����˰��
			pBattleUserItem->lClubRevenue = lClubRevenue/m_BattleConfig.wWinnerCount;
			pBattleUserItem->lSysRevenue = lSysRevenue/m_BattleConfig.wWinnerCount;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_BATTLE_REWARD, BattleReward.dwUserID, 0, 0, &BattleReward, sizeof(BattleReward));

			//�˳��ж�
			if (pBattleUserItem->wRankID >= m_BattleConfig.wWinnerCount) break;
		}
	}

	//ս����¼
	if (bRecordBattle && CBattleRule::IsForbidBattleRecord(m_pGameServiceOption->dwBattleRule) == false)//kk 
	{
		//����ṹ
		DBR_GR_BattleRecord BattleRecord;
		ZeroMemory(&BattleRecord, sizeof(BattleRecord));
		
		//Լս��Ϣ
		BattleRecord.dwBattleNum = m_dwBattleNum;
		BattleRecord.dwOwnerID = m_dwTableOwnerID;
		BattleRecord.bBattleFinish = IsBattleFinished();
		BattleRecord.lBattleRecordID = m_lBattleRecordID;

		//Ⱥ����Ϣ
		BattleRecord.dwClubID = m_dwClubID;
		BattleRecord.dwClubCreaterID = m_dwClubCreaterID;

		//֧����Ϣ
		BattleRecord.wPayType = m_BattleConfig.wPayType;  //lClubRevenue  BattleUserItem
		BattleRecord.dwPayUserID = m_BattleConfig.dwPayUserID;

		//������Ϣ
		BattleRecord.dwConfigID = m_dwConfigID;
		BattleRecord.dwCreateTime = m_dwStartTime;	
		BattleRecord.wFinishCount = m_wFinishCount;
		BattleRecord.wPlayCount = m_BattleConfig.wPlayCount;
		BattleRecord.lBaseScore = m_BattleConfig.lBaseScore;
		BattleRecord.lWinnerScore = m_BattleConfig.lWinnerScore;
		BattleRecord.cbScoreKind = m_BattleConfig.cbScoreKind;
		BattleRecord.cbSettleKind = m_BattleConfig.cbSettleKind;
		BattleRecord.cbAllowPublic = m_BattleConfig.cbAllowPublic;
		BattleRecord.lConsumeCount = m_BattleConfig.lConsumeCount;
		BattleRecord.cbCurrencyKind = m_BattleConfig.cbCurrencyKind;		
		

		//��������
		CString strOptionItem;
		CString strGameOption = TEXT("");

		//��ͨ����
		for (BYTE i = 0; i < m_BattleConfig.cbOptionSize; i++)
		{
			strOptionItem.Format(TEXT("%02X"), m_BattleConfig.cbGameOption[i]);
			/*if (strGameOption.IsEmpty() == false)
			{
				strGameOption.Append(TEXT("|"));
			}*/
			strGameOption.Append(strOptionItem);
		}

		//��Ϸ����
		StringCchCopy(BattleRecord.szGameOption, CountArray(BattleRecord.szGameOption), strGameOption.GetString());		

		//������Ϣ
		BattleRecord.wUserCount = (WORD)m_BattleUserItemArray.GetCount();
		CopyMemory(BattleRecord.BattleUserItem, m_BattleUserItemArray.GetData(), BattleRecord.wUserCount * sizeof(tagBattleUserItem));

		//Ͷ������
		WORD wPacketSize = BattleRecord.wUserCount * sizeof(tagBattleUserItem);
		wPacketSize += sizeof(BattleRecord) - sizeof(BattleRecord.BattleUserItem);
		m_pIAssistDataBaseEngine->PostDataBaseRequest(DBR_GR_BATTLE_RECORD, 0, 0, 0, &BattleRecord, wPacketSize);
	}

	//��������
	WORD wPacketSize = 0;
	BYTE cbDataBuffer[sizeof(tagBattleScoreItem) * TABLE_USER_COUNT];

	//��������
	for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
	{
		//ԼսժҪ
		if (m_pIEventBattleItem != NULL)
		{
			auto pszBattleAbstract = m_pIEventBattleItem->GetUserBattleAbstract(m_BattleUserItemArray[i].wChairID);
			if (pszBattleAbstract != NULL)
			{
				StringCchCopy(m_BattleUserItemArray[i].szBattleAbstract, CountArray(m_BattleUserItemArray[i].szBattleAbstract), m_pIEventBattleItem->GetUserBattleAbstract(m_BattleUserItemArray[i].wChairID));
			}
		}		

		//��ȡ����
		auto pUserScoreItem = (tagBattleScoreItem*)(cbDataBuffer + wPacketSize);
		CopyMemory(pUserScoreItem, &m_BattleUserItemArray[i], sizeof(tagBattleScoreItem));

		//λ��ƫ��
		wPacketSize += sizeof(tagBattleScoreItem);
	}

	//��������
	if (wPacketSize > 0)
	{
		//������Ϸ���
		SendTableData(INVALID_CHAIR, SUB_GF_TABLE_BATTLE, cbDataBuffer, wPacketSize, MDM_GF_FRAME);

		//�����Թ����
		SendLookonData(INVALID_CHAIR, SUB_GF_TABLE_BATTLE, cbDataBuffer, wPacketSize, MDM_GF_FRAME);
	}

	return true;
}

//�û��б�
bool CTableFrameBattle::NotifyTableUserList()
{
	//״̬�ж�
	//if (m_wFinishCount==0) return false;

	//���û�
	if (CBattleRule::IsForbidBindUser(m_pGameServiceOption->dwBattleRule)) return false;

	//����ṹ
	CMD_BS_S_TableUserList TableUserList;
	ZeroMemory(&TableUserList, sizeof(TableUserList));

	//���ñ���
	TableUserList.dwBattleNum = m_dwBattleNum;

	//��������
	tagBattleTableUser* pTableUserItem = NULL;

	//�����û�
	for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
	{
		//��ȡ�û�
		IServerUserItem* pIServerUserItem = GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL) continue;
		if (wChairID >= CountArray(TableUserList.TableUserItem)) break;

		//��ȡ����
		TableUserList.wUserCount++;
		pTableUserItem = &TableUserList.TableUserItem[wChairID];

		//���ñ���
		pTableUserItem->wChairID = wChairID;
		pTableUserItem->dwGameID = pIServerUserItem->GetGameID();
		pTableUserItem->dwUserID = pIServerUserItem->GetUserID();
		pTableUserItem->cbGender = pIServerUserItem->GetUserInfo()->cbGender;		
		pTableUserItem->wFaceID = pIServerUserItem->GetUserInfo()->wFaceID;
		pTableUserItem->dwCustomID = pIServerUserItem->GetUserInfo()->dwCustomID;
		pTableUserItem->wGameOrder = g_GameLevelManager->CalcGameOrder(pIServerUserItem->GetExperience());
		pTableUserItem->wMemberOrder = g_GameLevelManager->CalcUserOrder(pIServerUserItem->GetMemberPoint());
		StringCchCopy(pTableUserItem->szNickName, CountArray(pTableUserItem->szNickName), pIServerUserItem->GetAccounts());
	}

	//��������	
	m_pITCPSocketBattle->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_TABLE_USERLIST, &TableUserList, sizeof(TableUserList), 0);

	return true;
}

//���Խ�ɢ
bool CTableFrameBattle::IsBattleFinished()
{
	//��������
	if (m_BattleConfig.cbSettleKind == SETTLE_KIND_COUNT)
	{
		if (m_wFinishCount >= m_BattleConfig.wPlayCount)
		{
			return true;
		}
	}

	//�Զ������
	if (m_BattleConfig.cbSettleKind == SETTLE_KIND_CUSTOM)
	{
		return m_bBattleFinish;
	}

	return false;
}

//��ɢ����
bool CTableFrameBattle::DismissBattleTable(DWORD dwUserID)
{
	//״̬�ж�
	if (m_dwBattleNum == 0)
	{
		return false;
	}

	//��������
	bool bGameStarted = IsGameStarted();

	//�˷�У��
	if (m_BattleConfig.lConsumeCount > 0 && (m_wFinishCount == 0 || (m_wFinishCount == 1 && bGameStarted == true)))
	{
		//�˻�����
		if (m_bDeductCost == true)
		{
			PerformReturnCost(m_BattleConfig.wPayType);
		}
		else
		{
			m_bReturnCost = true;	
		}
	}

	//��������
	WORD wEnumIndex = 0;
	TCHAR szMessage[128] = L"";
	_snwprintf_s(szMessage, CountArray(szMessage), CWHService::FormatMessage(MIM_SERVER_0067, TEXT("�����ѽ�ɢ��ǰ���䣬������Լս��")));

	//�Թ����
	do
	{
		//��ȡ�û�
		auto pIServerUserItem = EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem == NULL) break;

		//Ч��״̬
		if (pIServerUserItem->IsClientReady() == false) continue;

		//�Ͽ�����
		m_pIServiceFunction->SendGameMessage(pIServerUserItem, szMessage, SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);

	} while (true);

	//�������
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//��ȡ�û�
		IServerUserItem* pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;
		if (pIServerUserItem->GetUserID() == dwUserID) continue;
		if (pIServerUserItem->IsClientReady() == false) continue;

		//��������
		if (m_wFinishCount == 0)
		{
			m_pIServiceFunction->SendGameMessage(pIServerUserItem, szMessage, SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);
		}
		else
		{
			m_pIServiceFunction->SendGameMessage(pIServerUserItem, szMessage, SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE);
		}
	}

	//��ɢ��Ϸ
	if (bGameStarted == true) DismissGame();

	//��ɢ����
	if (m_dwBattleNum != 0) DismissBattleTable(!m_bReturnCost);

	return true;
}

//��ɢ����
void CTableFrameBattle::DismissBattleTable(bool bRecordBattle)
{

	//CTraceService::TraceString(_T("��ɢ����"), TraceLevel_Exception);
	//״̬�ж�
	if (m_dwBattleNum == 0) return;

	//����ս��
	NotifyTableBattle(bRecordBattle);

	//��������
	CMD_BS_S_TableDismiss TableDismiss;
	ZeroMemory(&TableDismiss, sizeof(TableDismiss));

	//������Ϣ	
	TableDismiss.wTableID = m_wTableID;
	TableDismiss.dwBattleNum = m_dwBattleNum;
	TableDismiss.wServerID = m_pGameServiceOption->wServerID;

	//��������
	m_pITCPSocketBattle->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_TABLE_DISMISS, &TableDismiss, sizeof(TableDismiss), 0);

	//���ñ���	
	m_dwPlayCount = 0;
	m_dwStartTime = 0;
	m_wFinishCount = 0;
	m_dwBattleRule = 0;
	m_lBattleRecordID = 0;
	m_bBattleFinish = false;
	m_cbBattleState = BATTLE_STATE_FREE;

	//���ñ���
	m_dwLaunchTime = 0;
	m_dwLaunchUserID = 0;
	ZeroMemory(m_cbBallotResult, sizeof(m_cbBallotResult));

	//��������
	m_dwConfigID = 0;
	m_dwBattleNum = 0;
	m_dwTableOwnerID = 0;
	m_szTablePassword[0] = 0;	
	ZeroMemory(&m_BattleConfig, sizeof(m_BattleConfig));

	//����״̬
	SendTableStatus();

	//���õ׷�
	SetCellScore(m_pGameServiceOption->lCellScore);

	//��ɢ֪ͨ
	if (m_pIEventBattleItem) m_pIEventBattleItem->OnEventTableDismiss();

	//�Ƴ�Ԫ��
	m_BattleUserItemArray.RemoveAll();

	//�رն�ʱ��
	KillGameTimer(IDI_TABLE_DISMISS);
	KillGameTimer(IDI_KICKOUT_PLAYER);
	KillGameTimer(IDI_KICKOUT_LOOKON);
	KillGameTimer(IDI_AGREEMENT_DISMISS);

	//�û�����
	for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
	{
		//��ȡ�û�
		IServerUserItem* pIServerUserItem = GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL) continue;

		//�й��û�
		if (pIServerUserItem->IsTrusteeUser() == true)
		{
			//�û�����
			PerformStandUpAction(pIServerUserItem, true);

			//�߳�����
			if (pIServerUserItem->GetUserStatus() != US_NULL)
			{
				pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
			}
		}
		else
		{
			//У�����״̬
			if (m_bBattleService == false)
			{
				//�Ͽ�����
				m_pIServiceFunction->SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0078, TEXT("ϵͳ����ά����Լս������ʱֹͣ��������ά����ɺ����ԣ�")), SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);
			}
		}
	}

	//��������
	WORD wEnumIndex = 0;
	IServerUserItem* pIServerUserItem = NULL;

	//�Թ��û�
	do
	{
		//��ȡ�û�
		pIServerUserItem = EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem == NULL) break;

		//Ч��״̬
		if (pIServerUserItem->IsClientReady() == false) continue;

		//У�����״̬
		if (m_bBattleService == false)
		{
			//�Ͽ�����
			m_pIServiceFunction->SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0078, TEXT("ϵͳ����ά����Լս������ʱֹͣ��������ά����ɺ����ԣ�")), SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);
		}
		else
		{
			//�Ͽ�����
			m_pIServiceFunction->SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0122, TEXT("��ǰ���Թ۵ķ����ѽ�ɢ��")), SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);
		}

	} while (true);
}

//��������
bool CTableFrameBattle::CreateBattleTable(tagBattleCreateParam& CreateParam)
{
	//��������
	CopyMemory(&m_BattleConfig, &CreateParam.BattleConfig, sizeof(m_BattleConfig));

	//if (m_pIEventBattleItem != NULL)
		//CTraceService::TraceString(_T("m_pIEventBattleItem is not null"), TraceLevel_Exception);

	//У������
	if (VerifyBattleConfig(&m_BattleConfig, false) == false)
	{
		CTraceService::TraceString(_T("VerifyBattleConfig is false"), TraceLevel_Exception);
		return false;
	}
	//else
		//CTraceService::TraceString(_T("VerifyBattleConfig is false"), TraceLevel_Exception);

	//���ñ���
	m_dwPlayCount = 0;
	m_dwStartTime = 0;
	m_wFinishCount = 0;
	m_dwBattleRule = 0;
	m_bDeductCost = false;
	m_bBattleFinish = false;
	m_bReturnCost = false;
	m_bDismissNotify = false;
	m_cbBattleState = BATTLE_STATE_WAIT;
	m_dwConfigID = CreateParam.dwConfigID;
	m_dwBattleNum = CreateParam.dwBattleNum;
	m_dwCreateTime = CreateParam.dwCreateTime;
	m_dwTableOwnerID = CreateParam.dwOwnerID;
	StringCchCopy(m_szTablePassword, CountArray(m_szTablePassword), CreateParam.szPassword);

	//��¼��ʶ
	m_lBattleRecordID = BuildBattlleRecordID();

	//Ⱥ����Ϣ
	m_dwClubID = CreateParam.dwClubID;
	m_wSwitchOption = CreateParam.wSwitchOption;
	m_wClubServiceID = CreateParam.wClubServiceID;
	m_dwClubCreaterID = CreateParam.dwClubCreaterID;

	//���ܱ�־
	m_bSameIpRemind = !CBattleRule::IsForbidSameIPRemind(m_pGameServiceOption->dwBattleRule);

	//���ñ���
	m_dwLaunchTime = 0;
	m_dwLaunchUserID = 0;
	ZeroMemory(m_cbBallotResult, sizeof(m_cbBallotResult));

	//��������
	SetMaxPlayerCount(m_BattleConfig.wPlayerCount);

	//���ñ���
	m_ChargeUserArray.RemoveAll();
	m_BattleUserItemArray.RemoveAll();

	//����״̬
	SendTableStatus();

	//���õ׷�
	SetCellScore(m_BattleConfig.lBaseScore);

	//����֪ͨ
	if (m_pIEventBattleItem != NULL)
	{
		//����֪ͨ
		m_pIEventBattleItem->OnEventTableCreated(&CreateParam, sizeof(tagBattleCreateParam));
	}

	//��ɢʱ��
	DWORD dwDismissTime = TIME_BATTLE_TABLE_DISMISS;
	if (m_pIEventCustomFrameTime != NULL)
	{
		dwDismissTime = m_pIEventCustomFrameTime->GetBattleDismissTime();
	}

	//ʱ����
	if (dwDismissTime > 0)
	{
		DWORD dwTimeSpace = (DWORD)time(NULL) - m_dwCreateTime;
		if (dwTimeSpace >= dwDismissTime)
		{
			SetGameTimer(IDI_TABLE_DISMISS, dwDismissTime * 1000L, -1, 0);
		}
		else
		{
			SetGameTimer(IDI_TABLE_DISMISS, (dwDismissTime - dwTimeSpace) * 1000L, -1, 0);
		}
	}

	return true;
}

//�޸�����
bool CTableFrameBattle::ModifyBattleTable(tagBattleModifyParam& ModifyParam)
{
	//У����
	if (ModifyParam.dwBattleNum != m_dwBattleNum)
	{
		return false;
	}

	//У������
	if (m_dwClubID==0 && ModifyParam.dwUserID != m_dwTableOwnerID)
	{
		return false;
	}

	//У�����
	if (m_dwClubID!=0 && ModifyParam.dwUserID != m_dwClubCreaterID)
	{
		return false;
	}

	//У������	
	if (VerifyBattleConfig(&ModifyParam.BattleConfig, true) == false)
	{
		return false;
	}

	//��������	
	CopyMemory(&m_BattleConfig, &ModifyParam.BattleConfig, sizeof(m_BattleConfig));

	//��������
	VerifyBattleConfig(&m_BattleConfig, false);

	//�޸�֪ͨ
	if (m_pIEventBattleItem != NULL)
	{
		//�޸�֪ͨ
		m_pIEventBattleItem->OnEventTableModify(&ModifyParam, sizeof(ModifyParam));
	}

	//����֪ͨ
	NotifyTableConfig();

	return true;
}

//������Ϣ
void CTableFrameBattle::GetBattleTableItem(tagBattleTableItemEx& TableItem)
{
	//���ñ���
	ZeroMemory(&TableItem, sizeof(TableItem));

	//������Ϣ
	TableItem.wTableID = m_wTableID;
	TableItem.dwBattleNum = m_dwBattleNum;
	TableItem.dwOwnerID = m_dwTableOwnerID;
	TableItem.dwClubID = m_dwClubID;
	TableItem.wSwitchOption = m_wSwitchOption;
	TableItem.wClubServiceID = m_wClubServiceID;
	TableItem.dwClubCreaterID = m_dwClubCreaterID;	
	TableItem.wServerID = m_pGameServiceOption->wServerID;
	StringCchCopy(TableItem.szPassword, CountArray(TableItem.szPassword), m_szTablePassword);

	//������Ϣ  
	TableItem.dwConfigID = m_dwConfigID;
	TableItem.dwCreateTime = m_dwCreateTime;	
	CopyMemory(&TableItem.BattleConfig, &m_BattleConfig, sizeof(TableItem.BattleConfig));
}

//����״̬
void CTableFrameBattle::SetBattleService(bool bBattleService)
{
	//����״̬
	m_bBattleService = bBattleService;

	//ֹͣ����
	if (m_bBattleService == false)
	{
		//��������
		WORD wEnumIndex = 0;
		IServerUserItem* pIServerUserItem = NULL;

		//�Թ����
		do
		{
			//��ȡ�û�
			pIServerUserItem = EnumLookonUserItem(wEnumIndex++);
			if (pIServerUserItem == NULL) break;

			//Ч��״̬
			if (pIServerUserItem->IsClientReady() == false) continue;

			//�Ͽ�����
			m_pIServiceFunction->SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0078, TEXT("ϵͳ����ά����Լս������ʱֹͣ��������ά����ɺ����ԣ�")), SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);

		} while (true);

		//�������
		for (WORD i = 0; i < m_wChairCount; i++)
		{
			//��ȡ�û�
			IServerUserItem* pIServerUserItem = GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;

			if (m_wFinishCount == 0)
			{
				m_pIServiceFunction->SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0078, TEXT("ϵͳ����ά����Լս������ʱֹͣ��������ά����ɺ����ԣ�")), SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);
			}
			else
			{
				m_pIServiceFunction->SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0079, TEXT("ϵͳ����ά����������Ϸ��ɺ���ʱ�޷����뷿�䣬����ά����ɺ����ԣ�")), SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE);
			}
		}
	}
}

//ִ��ͶƱ
bool CTableFrameBattle::PerformBallotDismiss(WORD wChairID, DWORD dwUserID, bool bAgreeDismiss)
{
	//����У��
	ASSERT(wChairID < m_wChairCount);
	if (wChairID >= m_wChairCount) return false;

	//ͶƱ���			
	m_cbBallotResult[wChairID] = bAgreeDismiss ? 1 : 2;

	//����ṹ
	CMD_GF_DismissBallot DismissBallot;
	ZeroMemory(&DismissBallot, sizeof(DismissBallot));

	//���ñ���
	DismissBallot.dwUserID = dwUserID;
	DismissBallot.bAgreeDismiss = bAgreeDismiss;

	//��������
	WORD wAgreeCount = 0, wDisAgreeCount = 0;
	WORD wNeedAgreeCount = 0, wBallotUserCount = 0;

	//��������
	tagBattleUserItem* pBattleUserItem = NULL;

	//ͳ���û�
	for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
	{
		//��ȡ����
		pBattleUserItem = &m_BattleUserItemArray[i];

		//�����û�
		wBallotUserCount++;
	}

	//ͬ������
	wNeedAgreeCount = wBallotUserCount;
	if (m_pIEventBattleItem != NULL)
	{
		wNeedAgreeCount = m_pIEventBattleItem->GetDismissAgreeCount();
		if (wNeedAgreeCount > wBallotUserCount) wNeedAgreeCount = wBallotUserCount;
	}

	//��Ϸ�û�
	for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
	{
		//��ȡ����
		pBattleUserItem = &m_BattleUserItemArray[i];

		//��ȡ����
		WORD wChairID = pBattleUserItem->wChairID;

		//ͳ�ƽ��
		if (m_cbBallotResult[wChairID] == 1) wAgreeCount++;
		if (m_cbBallotResult[wChairID] == 2) wDisAgreeCount++;

		//��ȡ�û�
		IServerUserItem* pISendUserItem = GetTableUserItem(wChairID);
		if ((pISendUserItem == NULL) || (pISendUserItem->IsClientReady() == false)) continue;

		//��������
		m_pIServiceFunction->SendDataToUser(pISendUserItem, MDM_GF_FRAME, SUB_GF_DISMISS_BALLOT, &DismissBallot, sizeof(DismissBallot));
	}

	//��ɢʧ��
	if (wDisAgreeCount > (wBallotUserCount - wNeedAgreeCount))
	{
		//ɾ����ʱ��
		KillGameTimer(IDI_AGREEMENT_DISMISS);

		//������Ϸ
		ContinueGame();

		//���ñ���
		m_dwLaunchUserID = 0;
		ZeroMemory(m_cbBallotResult, sizeof(m_cbBallotResult));
	}

	//��ɢ�ɹ�
	if (wAgreeCount == wNeedAgreeCount)
	{
		//ɾ����ʱ��
		KillGameTimer(IDI_AGREEMENT_DISMISS);

		//���ñ���
		m_dwLaunchUserID = 0;
		ZeroMemory(m_cbBallotResult, sizeof(m_cbBallotResult));

		//��Ϸ�û�
		for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
		{
			//��ȡ����
			pBattleUserItem = &m_BattleUserItemArray[i];

			//��ȡ�û�
			IServerUserItem* pISendUserItem = GetTableUserItem(pBattleUserItem->wChairID);
			if ((pISendUserItem == NULL) || (pISendUserItem->IsClientReady() == false)) continue;

			//��������
			m_pIServiceFunction->SendDataToUser(pISendUserItem, MDM_GF_FRAME, SUB_GF_DISMISS_SUCCESS, NULL, 0);
		}

		//Լս����
		BattleFinish(BFR_AGREEMENT_DISMISS);
	}

	return true;
}

//�۷ѽ��
bool CTableFrameBattle::DeductCostResult(bool bDeductCostSuccess, DWORD dwPayUserID, DWORD dwFailUserID)
{
	//�۷�ʧ��
	if (!bDeductCostSuccess)
	{
		//AA֧��
		if (m_BattleConfig.wPayType == PAY_TYPE_USER)
		{
			//������Ϣ
			TCHAR szOtherMessage[128] = TEXT("");
			TCHAR szAppointMessage[128] = TEXT("");
			WORD wType = SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_ROOM;

			//�������
			if (m_BattleConfig.cbCurrencyKind == CURRENCY_KIND_GOLD)
			{
				//������ʾ			
				_snwprintf_s(szOtherMessage, CountArray(szOtherMessage), CWHService::FormatMessage(MIM_SERVER_0082, TEXT("��������ҽ�Ҳ��㣬��Ϸ�޷���ʼ��")));
				_snwprintf_s(szAppointMessage, CountArray(szAppointMessage), CWHService::FormatMessage(MIM_SERVER_0083, TEXT("���������Ͻ�Ҳ��㣬�޷�������Ϸ��")));
			}

			//��������
			if (m_BattleConfig.cbCurrencyKind == CURRENCY_KIND_CARD)
			{
				//������ʾ			
				_snwprintf_s(szOtherMessage, CountArray(szOtherMessage), CWHService::FormatMessage(MIM_SERVER_0082, TEXT("��������ҷ������㣬��Ϸ�޷���ʼ��")));
				_snwprintf_s(szAppointMessage, CountArray(szAppointMessage), CWHService::FormatMessage(MIM_SERVER_0083, TEXT("���������Ϸ������㣬�޷�������Ϸ��")));
			}

			//��������
			WORD wEnumIndex = 0;

			//�Թ����
			do
			{
				//��ȡ�û�
				auto pIServerUserItem = EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem == NULL) break;

				//Ч��״̬
				if (pIServerUserItem->IsClientReady() == false) continue;

				//�Ͽ�����
				m_pIServiceFunction->SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0078, TEXT("ϵͳ����ά����Լս������ʱֹͣ��������ά����ɺ����ԣ�")), SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);

			} while (true);

			//�������
			for (WORD i = 0; i < m_wChairCount; i++)
			{
				//��ȡ�û�
				IServerUserItem* pIServerUserItem = GetTableUserItem(i);
				if (pIServerUserItem == NULL) continue;
				if (pIServerUserItem->IsClientReady() == false) continue;

				//����״̬
				pIServerUserItem->SetUserStatus(US_SIT, m_wTableID, i);

				//������Ϣ
				if (dwFailUserID != pIServerUserItem->GetUserID())
				{
					m_pIServiceFunction->SendGameMessage(pIServerUserItem, szOtherMessage, wType);
				}
				else
				{
					m_pIServiceFunction->SendGameMessage(pIServerUserItem, szAppointMessage, wType);
				}
			}			
		}

		return false;					
	}	

	//�۷ѱ�ʶ
	m_bDeductCost = true;

	//�������
	m_BattleConfig.dwPayUserID = dwPayUserID;

	//AA֧��
	if (m_BattleConfig.wPayType == PAY_TYPE_USER)
	{
		return __super::TryStartGame();
	}

	return true;
}

//��¼��ʶ
SCORE CTableFrameBattle::BuildBattlleRecordID()
{
	//��̬����
	static WORD wBattleCount = 0;
	static DWORD dwLastSecond = (DWORD)time(NULL);

	//��������
	DWORD dwSecond = (DWORD)time(NULL);

	//���±���
	if (dwLastSecond != dwSecond)
	{
		wBattleCount = 0;
		dwLastSecond = dwSecond;
	}

	//��ʽ����ʶ
	TCHAR szRecordID[19];
	_sntprintf_s(szRecordID, CountArray(szRecordID), TEXT("%05d%10d%03d"), m_pGameServiceOption->wServerID, dwSecond, ++wBattleCount);

	//����ת��
	SCORE lRecordID = _tstoi64(szRecordID);

	return lRecordID;
}

//�˻�����
bool CTableFrameBattle::PerformReturnCost(WORD wPayType)
{
	//��ʶ�ж�
	if (m_bReturnCost == true)
	{
		return false;
	}

	//�˷ѱ�ʶ
	m_bReturnCost = true;

	//����֧��
	if (wPayType == PAY_TYPE_OWNER)
	{
		//��������
		DBR_GR_ReturnBattleCost ReturnBattleCost;
		ZeroMemory(&ReturnBattleCost, sizeof(ReturnBattleCost));

		//����ṹ
		ReturnBattleCost.dwClubID = m_dwClubID;
		ReturnBattleCost.dwUserID = m_BattleConfig.dwPayUserID;		
		ReturnBattleCost.lReturnCount = m_BattleConfig.lConsumeCount;
		ReturnBattleCost.cbCurrencyKind = m_BattleConfig.cbCurrencyKind;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_RETURN_BATTLE_COST, ReturnBattleCost.dwUserID, 0L, 0L, &ReturnBattleCost, sizeof(ReturnBattleCost));
	}

	//AA֧��
	if (wPayType == PAY_TYPE_USER)
	{
		//��������
		DBR_GR_ReturnBattleCost ReturnBattleCost;
		ZeroMemory(&ReturnBattleCost, sizeof(ReturnBattleCost));

		for (WORD i = 0; i < m_wChairCount; i++)
		{
			//��ȡ�û�
			IServerUserItem* pIServerUserItem = GetTableUserItem(i);

			//״̬�ж�
			if (pIServerUserItem == NULL) continue;			

			//����ṹ
			ReturnBattleCost.dwClubID = m_dwClubID;			
			ReturnBattleCost.dwUserID = pIServerUserItem->GetUserID();
			ReturnBattleCost.lReturnCount = m_BattleConfig.lConsumeCount;			
			ReturnBattleCost.cbCurrencyKind = m_BattleConfig.cbCurrencyKind;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_RETURN_BATTLE_COST, ReturnBattleCost.dwUserID, 0L, 0L, &ReturnBattleCost, sizeof(ReturnBattleCost));
		}
	}

	return true;
}

//����˰��
SCORE CTableFrameBattle::CalcCustomRevenue(SCORE lGameScore)
{
	//CString s;
	//s.Format(_T("bbbbb  lGameScore:%I64d, m_BattleConfig.cbScoreKind:%d , SCORE_KIND_BATTLE:%d,"), lGameScore,  m_BattleConfig.cbScoreKind, SCORE_KIND_BATTLE);
    //CTraceService::TraceString(s, TraceLevel_Exception); 
	//Լս����
	if (m_BattleConfig.cbScoreKind == SCORE_KIND_BATTLE) return 0L;

	//�߼�����
	//auto pAdvancedOption = (tagAdvancedOption*)m_BattleConfig.cbAdvancedOption;// kk guan

	//if (lGameScore <= pAdvancedOption->lMinWinScore) return 0L;//kk guan

	//return pAdvancedOption->lRevenueRate * lGameScore / 1000L;//kk guan

	//kk jia start
	if (lGameScore < 0)
	{
		return  0;
	}

	if (lGameScore < m_BattleConfig.lMinWin)
	{
		return  0;
	}
	if ( lGameScore >= m_BattleConfig.lMinWin && lGameScore < m_BattleConfig.lWin2)
	{
		return m_BattleConfig.lRevenueRate1;
	}
	if (lGameScore >= m_BattleConfig.lWin2 && lGameScore < m_BattleConfig.lWin3)
	{
		return m_BattleConfig.lRevenueRate2;
	}
	
	return m_BattleConfig.lRevenueRate3;
	
	//kk jia end	
}

//��ȡ�û�
IServerUserItem* CTableFrameBattle::GetFirstSitUserItem()
{
	//��������
	DWORD dwSitdownTime = (DWORD)-1;
	IServerUserItem* pIServerUserItem = NULL;

	for (int i = 0; i < m_wChairCount; i++)
	{
		auto pITableUserItem = GetTableUserItem(i);
		if (pITableUserItem == NULL) continue;
		if (pITableUserItem->GetLogonTime() < dwSitdownTime)
		{
			pIServerUserItem = pITableUserItem;
			dwSitdownTime = pITableUserItem->GetLogonTime();
		}
	}

	return pIServerUserItem;
}

//��ɢ����
bool CTableFrameBattle::SendDimissNotify(IServerUserItem* pIServerUserItem)
{
	//��ȡ�û�
	tagBattleUserItem* pBattleUserItem = GetBattleUserItem(pIServerUserItem->GetUserID());
	if (pBattleUserItem == NULL)
	{
		return true;
	}

	//��ȡʱ��
	DWORD dwCurrentTime = (DWORD)time(NULL);
	DWORD dwDismissTime = TIME_BATTLE_AGREEMENT_DISMISS;

	//��ȡ����
	if (m_pIEventCustomFrameTime != NULL)
	{
		dwDismissTime = m_pIEventCustomFrameTime->GetAgreementDismissTime();
	}

	//����ṹ
	CMD_GF_DismissNotify DismissNotify;
	ZeroMemory(&DismissNotify, sizeof(DismissNotify));

	//���ñ���
	DismissNotify.dwUserID = m_dwLaunchUserID;
	DismissNotify.wResidueTime = dwDismissTime - __min(dwCurrentTime - m_dwLaunchTime, dwDismissTime);

	//ͳ���û�
	for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
	{
		//��ȡ����
		pBattleUserItem = &m_BattleUserItemArray[i];

		//�����û�
		DismissNotify.wNeedAgreeCount++;
		DismissNotify.dwUserIDList[DismissNotify.wUserCount++] = pBattleUserItem->dwUserID;
	}

	//����ͬ������
	if (m_pIEventBattleItem != NULL)
	{
		DismissNotify.wNeedAgreeCount = m_pIEventBattleItem->GetDismissAgreeCount();
	}

	//��������
	if (DismissNotify.wNeedAgreeCount > DismissNotify.wUserCount)
	{
		DismissNotify.wNeedAgreeCount = DismissNotify.wUserCount;
	}

	//�ж�ʱ��
	if (DismissNotify.wResidueTime > 0)
	{
		//��������
		m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_FRAME, SUB_GF_DISMISS_NOTIFY, &DismissNotify, sizeof(DismissNotify));

		//����ṹ
		CMD_GF_DismissBallot DismissBallot;
		ZeroMemory(&DismissBallot, sizeof(DismissBallot));

		//Լս�û�
		for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
		{
			//��ȡ����
			pBattleUserItem = &m_BattleUserItemArray[i];

			//��ȡ����
			WORD wChairID = pBattleUserItem->wChairID;

			//��ȡ�û�
			IServerUserItem* pITableUserItem = GetTableUserItem(wChairID);
			if (pITableUserItem == NULL || m_cbBallotResult[wChairID] == 0) continue;

			//���ñ���
			DismissBallot.dwUserID = pITableUserItem->GetUserID();
			DismissBallot.bAgreeDismiss = (m_cbBallotResult[wChairID] == 1);

			//��������
			m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_FRAME, SUB_GF_DISMISS_BALLOT, &DismissBallot, sizeof(DismissBallot));
		}
	}

	return true;
}

//��ȡ�û�
tagBattleUserItem* CTableFrameBattle::GetBattleUserItem(DWORD dwUserID)
{
	//У���û�
	for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
	{
		//��ȡ����
		if (m_BattleUserItemArray[i].dwUserID == dwUserID)
		{
			return &m_BattleUserItemArray[i];
		}
	}

	return NULL;
}

//У������
bool CTableFrameBattle::VerifyBattleConfig(tagBattleConfig* pBattleConfig, bool bOnlyVerify)
{
	//��Ϸѡ��
	if (m_pIEventGameOption != NULL && pBattleConfig->cbOptionSize > 0)
	{
		if (m_pIEventGameOption->SetNormalOption(bOnlyVerify, pBattleConfig->cbGameOption, pBattleConfig->cbOptionSize) == false)
		{
			return false;
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
