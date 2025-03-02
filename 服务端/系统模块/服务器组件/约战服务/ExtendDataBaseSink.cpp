#include "stdafx.h"
#include "ExtendDataBaseSink.h"
#include "..\��Ϸ����\DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////

//�����Ϣ
void TraceDatabaseException(char* pFile, char* pFunction, int nLine, IDataBaseException* pIException)
{
	CString strMessage;
	strMessage.Format(TEXT("%s(%d):%hs(%s)"), CA2CT(pFile).m_psz, nLine, pFunction, pIException ? pIException->GetExceptionDescribe() : TEXT(""));
	CTraceService::TraceString(strMessage, TraceLevel_Exception);
}

///////////////////////////////////////////////////////////////////////////////////
//���캯��
CExtendDataBaseSink::CExtendDataBaseSink()
{
	//���ñ���
	m_pGameDBAide=NULL;					
	m_pIGameDBModule=NULL;	
	m_pAccountsDBAide=NULL;				
	m_pIAccountsDBModule=NULL;			
	m_pPlatformDBAide=NULL;				
	m_pIPlatformDBModule=NULL;			

	//���ñ���
	m_pGameServiceAttrib=NULL;			
	m_pGameServiceOption=NULL;			

	//�������
	m_pITimerEngine=NULL;				
	m_pIDataBaseEngine=NULL;			
	m_pIDataBaseEngineEvent=NULL;		
}

//��������
CExtendDataBaseSink::~CExtendDataBaseSink()
{
}

//�ӿڲ�ѯ
VOID* CExtendDataBaseSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IExtendDataBaseSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IExtendDataBaseSink, Guid, dwQueryVer);
	return NULL;
}

//��ʼ��
bool CExtendDataBaseSink::InitExtendDataBaseSink(tagExtendDBSinkParameter& ExtendDBSinkParameter)
{
	//��Ϸ���ݿ�
	m_pGameDBAide = ExtendDBSinkParameter.pGameDBAide;
	m_pIGameDBModule = ExtendDBSinkParameter.pIGameDBModule;

	//�˺����ݿ�
	m_pAccountsDBAide = ExtendDBSinkParameter.pAccountsDBAide;
	m_pIAccountsDBModule = ExtendDBSinkParameter.pIAccountsDBModule;

	//ƽ̨���ݿ�
	m_pPlatformDBAide = ExtendDBSinkParameter.pPlatformDBAide;
	m_pIPlatformDBModule = ExtendDBSinkParameter.pIPlatformDBModule;

	//���ñ���
	m_pGameServiceAttrib = ExtendDBSinkParameter.pGameServiceAttrib;
	m_pGameServiceOption = ExtendDBSinkParameter.pGameServiceOption;

	//�������
	m_pITimerEngine = ExtendDBSinkParameter.pITimerEngine;
	m_pIDataBaseEngine = ExtendDBSinkParameter.pIDataBaseEngine;
	m_pIDataBaseEngineEvent = ExtendDBSinkParameter.pIDataBaseEngineEvent;

	return true;
}

//ʱ���¼�
bool CExtendDataBaseSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//�����¼�
bool CExtendDataBaseSink::OnDataBaseEngineControl(WORD wControlID, VOID* pData, WORD wDataSize)
{
	return false;
}

//�����¼�
bool CExtendDataBaseSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//У���ʶ
	if (wRequestID >= DB_EXTEND_REQUEST_START)
	{
		switch (wRequestID)
		{
		case DBR_GR_BATTLE_LOCK:			//Լս����
		{
			return OnRequestBattleLock(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_BATTLE_UNLOCK:			//Լս����
		{
			return OnRequestBattleUnlock(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_BATTLE_RECORD:			//ս����¼
		{
			return OnRequestBattleRecord(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_BATTLE_REWARD:			//Լս����
		{
			return OnRequestBattleReward(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_DEDUCT_BATTLE_COST:		//�۳�����
		{
			return OnRequestDeductBattleCost(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_DEDUCT_BATTLE_COSTAA:	//�۳�����
		{
			return OnRequestDeductBattleCostAA(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_RETURN_BATTLE_COST:		//�˻�����
		{
			return OnRequestReturnBattleCost(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		}
	}

	return false;
}

//�����û�
bool CExtendDataBaseSink::OnRequestBattleLock(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//Ч�����
	ASSERT(wDataSize <= sizeof(DBR_GR_BattleLock));
	if (wDataSize > sizeof(DBR_GR_BattleLock)) return false;

	//��ȡ����
	DBR_GR_BattleLock* pBattleLock = (DBR_GR_BattleLock*)pData;

	try
	{
		//���ò���
		m_pGameDBAide->ResetParameter();

		//�����ʶ�б�
		CString strUserIDList=TEXT("[");

		for (int i = 0; i < pBattleLock->wUserCount; i++)
		{
			strUserIDList.AppendFormat(TEXT("%d"), pBattleLock->dwUserID[i]);
			if (i < pBattleLock->wUserCount - 1)
			{
				strUserIDList.Append(TEXT(","));
			}
		}

		//��ӻ�����		
		strUserIDList.Append(TEXT("]"));

		//Լս��Ϣ
		m_pGameDBAide->AddParameter(TEXT("@dwBattleNum"), pBattleLock->dwBattleNum);
		m_pGameDBAide->AddParameter(TEXT("@cbScoreKind"), pBattleLock->cbScoreKind);

		//�������		
		m_pGameDBAide->AddParameter(TEXT("@strUserIDList"), strUserIDList);

		//ִ������
		m_pGameDBAide->ExecuteProcess(TEXT("GSP_GR_LockBattleUser"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return true;
	}

	return true;
}

//Լս����
bool CExtendDataBaseSink::OnRequestBattleUnlock(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//Ч�����
	ASSERT(wDataSize <= sizeof(DBR_GR_BattleUnlock));
	if (wDataSize > sizeof(DBR_GR_BattleUnlock)) return false;

	//��ȡ����
	DBR_GR_BattleUnlock* pBattleUnlock = (DBR_GR_BattleUnlock*)pData;

	try
	{
		//���ò���
		m_pGameDBAide->ResetParameter();

		//�����ʶ�б�
		CString strUserIDList = TEXT("[");

		for (int i = 0; i < pBattleUnlock->wUserCount; i++)
		{
			strUserIDList.AppendFormat(TEXT("%d"), pBattleUnlock->dwUserID[i]);
			if (i < pBattleUnlock->wUserCount - 1)
			{
				strUserIDList.Append(TEXT(","));
			}
		}

		//��ӻ�����		
		strUserIDList.Append(TEXT("]"));

		//�������
		m_pGameDBAide->AddParameter(TEXT("@dwBattleNum"), pBattleUnlock->dwBattleNum);		
		m_pGameDBAide->AddParameter(TEXT("@strUserIDList"), strUserIDList);

		//ִ������
		m_pGameDBAide->ExecuteProcess(TEXT("GSP_GR_UnlockBattleUser"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return true;
	}

	return true;
}

//ս����¼
bool CExtendDataBaseSink::OnRequestBattleRecord(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize <= sizeof(DBR_GR_BattleRecord));
		if (wDataSize > sizeof(DBR_GR_BattleRecord)) return false;

		//������
		DBR_GR_BattleRecord* pBattleRecord = (DBR_GR_BattleRecord*)pData;

		//��������		
		CString strScoreItem, strItemList;
		LPCTSTR pszScoreFormat = TEXT("[%s]");
		LPCTSTR pszItemFormat = pszItemFormat = TEXT("\"ChairID\":%d,\"UserID\":%d,\"GameID\":%d,\"UserStatus\":%d,\"NickName\":\"%s\",\"FaceID\":%d,\"CustomID\":%d,\"Gender\":%d,\"RankID\":%d,\"Score\":%I64d,\"SysRevenue\":%I64d,\"ClubRevenue\":%I64d,\"RewardScore\":%I64d,\"WinCount\":%d,\"LostCount\":%d,\"DrawCount\":%d,\"Abstract\":\"%s\"");

		//��������
		WORD wStringSize = 0;
		BYTE cbNickName[64];
		CString strItem, strNickName;

		//�������
		strItemList.Append(TEXT("["));

		//��������		
		for (int i = 0; i < pBattleRecord->wUserCount; i++)
		{
			//��ȡ����
			tagBattleScoreItem* pBattleScoreItem = &pBattleRecord->BattleUserItem[i];

			//�����ǳ�
			wStringSize = sizeof(TCHAR) * lstrlen(pBattleScoreItem->szNickName);
			CopyMemory(cbNickName, pBattleScoreItem->szNickName, wStringSize);

			//��ʽ���ǳ�
			strNickName.Empty();
			for (int j = 0; j < wStringSize; j++)
			{
				strItem.Format(TEXT("%02x"), cbNickName[j]);
				strNickName.Append(strItem);
			}

			//��ʽ����
			strScoreItem.Format(pszItemFormat, pBattleScoreItem->wChairID, pBattleScoreItem->dwUserID, pBattleScoreItem->dwGameID, pBattleScoreItem->cbUserStatus, strNickName, pBattleScoreItem->wFaceID, pBattleScoreItem->dwCustomID, pBattleScoreItem->cbGender, pBattleScoreItem->wRankID,pBattleScoreItem->lScore, pBattleScoreItem->lSysRevenue, pBattleScoreItem->lClubRevenue, pBattleScoreItem->lRewardScore, pBattleScoreItem->lWinCount, pBattleScoreItem->lLostCount, pBattleScoreItem->lDrawCount, pBattleScoreItem->szBattleAbstract);

			//׷�ӷ���
			strItemList.Append(TEXT("{"));
			strItemList.Append(strScoreItem);
			strItemList.Append(TEXT("}"));

			// ��ӷָ���
			if (i < pBattleRecord->wUserCount - 1)
			{
				strItemList.Append(TEXT(","));
			}
		}

		//�������
		strItemList.Append(TEXT("]"));

		//�������
		m_pGameDBAide->ResetParameter();
		m_pGameDBAide->AddParameter(TEXT("@dwBattleNum"), pBattleRecord->dwBattleNum);
		m_pGameDBAide->AddParameter(TEXT("@cbBattleFinish"), pBattleRecord->bBattleFinish);
		m_pGameDBAide->AddParameter(TEXT("@lBattleRecordID"), pBattleRecord->lBattleRecordID);
		m_pGameDBAide->AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		m_pGameDBAide->AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);
		m_pGameDBAide->AddParameter(TEXT("@cbScoreKind"), pBattleRecord->cbScoreKind);
		m_pGameDBAide->AddParameter(TEXT("@dwOwnerID"), pBattleRecord->dwOwnerID);
		m_pGameDBAide->AddParameter(TEXT("@dwClubID"), pBattleRecord->dwClubID);
		m_pGameDBAide->AddParameter(TEXT("@dwClubCreaterID"), pBattleRecord->dwClubCreaterID);
		m_pGameDBAide->AddParameter(TEXT("@cbSettleKind"), pBattleRecord->cbSettleKind);
		m_pGameDBAide->AddParameter(TEXT("@wFinishCount"), pBattleRecord->wFinishCount);
		m_pGameDBAide->AddParameter(TEXT("@wPlayCount"), pBattleRecord->wPlayCount);
		m_pGameDBAide->AddParameter(TEXT("@dwPlayTimeCount"), (DWORD)time(NULL) - pBattleRecord->dwCreateTime);
		m_pGameDBAide->AddParameter(TEXT("@dwCreateTime"), pBattleRecord->dwCreateTime);
		m_pGameDBAide->AddParameter(TEXT("@lBaseScore"), pBattleRecord->lBaseScore);		
		m_pGameDBAide->AddParameter(TEXT("@lWinnerScore"), pBattleRecord->lWinnerScore);
		m_pGameDBAide->AddParameter(TEXT("@wPayType"), pBattleRecord->wPayType);
		m_pGameDBAide->AddParameter(TEXT("@dwPayUserID"), pBattleRecord->dwPayUserID);
		m_pGameDBAide->AddParameter(TEXT("@dwConfigID"), pBattleRecord->dwConfigID);
		m_pGameDBAide->AddParameter(TEXT("@cbAllowPublic"), pBattleRecord->cbAllowPublic);
		m_pGameDBAide->AddParameter(TEXT("@lConsumeCount"), pBattleRecord->lConsumeCount);
		m_pGameDBAide->AddParameter(TEXT("@cbCurrencyKind"), pBattleRecord->cbCurrencyKind);		
		m_pGameDBAide->AddParameter(TEXT("@strGameOption"), pBattleRecord->szGameOption);
		m_pGameDBAide->AddParameter(TEXT("@UserScoreString"), strItemList);

		//ִ�в�ѯ
		LONG lReturnValue = m_pGameDBAide->ExecuteProcess(TEXT("GSP_GR_RecordBattleGrade"), true);

		return lReturnValue == DB_SUCCESS;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//Լս����
bool CExtendDataBaseSink::OnRequestBattleReward(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GR_BattleReward));
		if (wDataSize != sizeof(DBR_GR_BattleReward)) return false;

		//������
		auto pBattleReward = (DBR_GR_BattleReward*)pData;

		//�������
		m_pGameDBAide->ResetParameter();
		m_pGameDBAide->AddParameter(TEXT("@dwUserID"), pBattleReward->dwUserID);
		m_pGameDBAide->AddParameter(TEXT("@dwRewardGold"), pBattleReward->lRewardGold);
		m_pGameDBAide->AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		m_pGameDBAide->AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);

		//ִ�в�ѯ
		LONG lReturnValue = m_pGameDBAide->ExecuteProcess(TEXT("GSP_GR_BattleReward"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_UserWealthUpdate UserWealthUpdate;
			ZeroMemory(&UserWealthUpdate, sizeof(UserWealthUpdate));

			//���ñ���
			UserWealthUpdate.dwUserID = pBattleReward->dwUserID;
			UserWealthUpdate.lChangeAmount = pBattleReward->lRewardGold;

			//���ñ���
			UserWealthUpdate.wMoneyID = GOODS_ID_GOLD;
			UserWealthUpdate.lBalanceAmount = m_pGameDBAide->GetValue_DWORD(TEXT("UserGold"));
			
			//������Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE, dwContextID, dwTokenID, &UserWealthUpdate, sizeof(UserWealthUpdate));
		}

		return lReturnValue == DB_SUCCESS;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//�۳�����
bool CExtendDataBaseSink::OnRequestDeductBattleCost(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(DBR_GR_DeductBattleCost));
	if (wDataSize != sizeof(DBR_GR_DeductBattleCost)) return false;

	//��ȡ����
	DBR_GR_DeductBattleCost* pDeductBattleCost = (DBR_GR_DeductBattleCost*)pData;

	try
	{
		//�������
		m_pGameDBAide->ResetParameter();
		m_pGameDBAide->AddParameter(TEXT("@dwUserID"), pDeductBattleCost->dwUserID);
		m_pGameDBAide->AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		m_pGameDBAide->AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);
		m_pGameDBAide->AddParameter(TEXT("@wClubID"), pDeductBattleCost->dwClubID);
		m_pGameDBAide->AddParameter(TEXT("@wPayType"), pDeductBattleCost->wPayType);
		m_pGameDBAide->AddParameter(TEXT("@ConsumeCount"), pDeductBattleCost->lConsumeCount);
		m_pGameDBAide->AddParameter(TEXT("@CurrencyKind"), pDeductBattleCost->cbCurrencyKind);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_pGameDBAide->AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_pGameDBAide->ExecuteProcess(TEXT("GSP_GR_DeductBattleCost"), true);

		//���컺��
		DBO_GR_DeductBattleCost DeductBattleCost;
		ZeroMemory(&DeductBattleCost, sizeof(DeductBattleCost));

		//ִ�гɹ�
		if (lResultCode == DB_SUCCESS)
		{
			//��ȡ����
			DeductBattleCost.dwPayUserID = m_pGameDBAide->GetValue_DWORD(TEXT("PayUserID"));
			DeductBattleCost.lUserGold = m_pGameDBAide->GetValue_DWORD(TEXT("UserGold"));
			DeductBattleCost.lUserCard = m_pGameDBAide->GetValue_DWORD(TEXT("UserCard"));

			//����ṹ
			DBO_GR_UserWealthUpdate UserWealthUpdate;
			ZeroMemory(&UserWealthUpdate, sizeof(UserWealthUpdate));

			//���ñ���
			UserWealthUpdate.dwUserID = DeductBattleCost.dwPayUserID;
			UserWealthUpdate.lChangeAmount = -pDeductBattleCost->lConsumeCount;

			//�������
			if (pDeductBattleCost->cbCurrencyKind == CURRENCY_KIND_GOLD)
			{
				//���ñ���
				UserWealthUpdate.wMoneyID = GOODS_ID_GOLD;
				UserWealthUpdate.lBalanceAmount = DeductBattleCost.lUserGold;
			}

			//��������
			if (pDeductBattleCost->cbCurrencyKind == CURRENCY_KIND_CARD)
			{
				//���ñ���
				UserWealthUpdate.wMoneyID = GOODS_ID_CARD;
				UserWealthUpdate.lBalanceAmount = DeductBattleCost.lUserCard;
			}

			//������Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE, dwContextID, dwTokenID, &UserWealthUpdate, sizeof(UserWealthUpdate));
		}
		else
		{
			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_pGameDBAide->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������			
			StringCchCopy(DeductBattleCost.szErrorMessage, CountArray(DeductBattleCost.szErrorMessage), CW2CT(DBVarValue.bstrVal).m_psz);
		}

		//���ñ���
		DeductBattleCost.lResultCode = lResultCode;
		DeductBattleCost.wTableID = pDeductBattleCost->wTableID;

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DEDUCT_BATTLE_COST, dwContextID, dwTokenID, &DeductBattleCost, sizeof(DeductBattleCost));

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//����ṹ
		DBO_GR_DeductBattleCost DeductBattleCost;
		ZeroMemory(&DeductBattleCost, sizeof(DeductBattleCost));

		//���ñ���
		DeductBattleCost.lResultCode = -1;
		DeductBattleCost.wTableID = pDeductBattleCost->wTableID;

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DEDUCT_BATTLE_COST, dwContextID, dwTokenID, &DeductBattleCost, sizeof(DeductBattleCost));

		return true;
	}

	return true;
}

//�۳�����
bool CExtendDataBaseSink::OnRequestDeductBattleCostAA(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//Ч�����
	ASSERT(wDataSize <= sizeof(DBR_GR_DeductBattleCostAA));
	if (wDataSize > sizeof(DBR_GR_DeductBattleCostAA)) return false;

	//��ȡ����
	auto pDeductBattleCost = (DBR_GR_DeductBattleCostAA*)pData;

	try
	{
		//��������		
		CString strUserList;

		//�������
		strUserList.Append(TEXT("["));

		for (int i = 0; i < pDeductBattleCost->wUserCount; i++)
		{
			//��ȡ����
			tagBattleUser* ptagBattleUser = &pDeductBattleCost->BattleUser[i];
			
			strUserList.AppendFormat(TEXT("%d"), ptagBattleUser->dwUserID);
			if (i < pDeductBattleCost->wUserCount - 1)
			{
				strUserList.Append(TEXT(","));
			}
		}

		//��ӻ�����		
		strUserList.Append(TEXT("]"));

		//�������
		m_pGameDBAide->ResetParameter();
		m_pGameDBAide->AddParameter(TEXT("@strUserList"), strUserList);
		m_pGameDBAide->AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		m_pGameDBAide->AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);
		m_pGameDBAide->AddParameter(TEXT("@wClubID"), pDeductBattleCost->dwClubID);
		m_pGameDBAide->AddParameter(TEXT("@ConsumeCount"), pDeductBattleCost->lConsumeCount);
		m_pGameDBAide->AddParameter(TEXT("@CurrencyKind"), pDeductBattleCost->cbCurrencyKind);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_pGameDBAide->AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_pGameDBAide->ExecuteProcess(TEXT("GSP_GR_DeductBattleCostAA"), true);

		//���컺��
		DBO_GR_DeductBattleCost DeductBattleCost;
		ZeroMemory(&DeductBattleCost, sizeof(DeductBattleCost));

		//ִ�гɹ�
		if (lResultCode == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_UserWealthUpdate UserWealthUpdate;
			ZeroMemory(&UserWealthUpdate, sizeof(UserWealthUpdate));

			//������
			UserWealthUpdate.lChangeAmount = -pDeductBattleCost->lConsumeCount;

			//��ȡ�Ƹ�
			while (m_pIGameDBModule->IsRecordsetEnd() == false)
			{
				//�û���ʶ
				UserWealthUpdate.dwUserID = m_pGameDBAide->GetValue_DWORD(TEXT("UserID"));

				//�������
				if (pDeductBattleCost->cbCurrencyKind == CURRENCY_KIND_GOLD)
				{
					//���ñ���
					UserWealthUpdate.wMoneyID = GOODS_ID_GOLD;					
					UserWealthUpdate.lBalanceAmount = m_pGameDBAide->GetValue_LONGLONG(TEXT("UserGold"));
				}

				//��������
				if (pDeductBattleCost->cbCurrencyKind == CURRENCY_KIND_CARD)
				{
					//���ñ���
					UserWealthUpdate.wMoneyID = GOODS_ID_CARD;				
					UserWealthUpdate.lBalanceAmount = m_pGameDBAide->GetValue_LONGLONG(TEXT("UserCard"));
				}

				//������Ϣ
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE, dwContextID, dwTokenID, &UserWealthUpdate, sizeof(UserWealthUpdate));

				//�ƶ���¼
				m_pIGameDBModule->MoveToNext();
			}
		}
		else
		{
			DeductBattleCost.dwFailUserID = m_pGameDBAide->GetValue_DWORD(TEXT("FailUserID"));
		}

		//���ñ���
		DeductBattleCost.lResultCode = lResultCode;
		DeductBattleCost.wTableID = pDeductBattleCost->wTableID;

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DEDUCT_BATTLE_COST, dwContextID, dwTokenID, &DeductBattleCost, sizeof(DeductBattleCost));

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//����ṹ
		DBO_GR_DeductBattleCost DeductBattleCost;
		ZeroMemory(&DeductBattleCost, sizeof(DeductBattleCost));

		//���ñ���
		DeductBattleCost.lResultCode = -1;
		DeductBattleCost.wTableID = pDeductBattleCost->wTableID;

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DEDUCT_BATTLE_COST, dwContextID, dwTokenID, &DeductBattleCost, sizeof(DeductBattleCost));

		return true;
	}

	return true;
}

//�۳�����
bool CExtendDataBaseSink::OnRequestReturnBattleCost(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(DBR_GR_ReturnBattleCost));
	if (wDataSize != sizeof(DBR_GR_ReturnBattleCost)) return false;

	//��ȡ����
	auto pReturnBattleCost = (DBR_GR_ReturnBattleCost*)pData;

	try
	{
		//�������
		m_pGameDBAide->ResetParameter();
		m_pGameDBAide->AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		m_pGameDBAide->AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);
		m_pGameDBAide->AddParameter(TEXT("@wClubID"), pReturnBattleCost->dwClubID);
		m_pGameDBAide->AddParameter(TEXT("@dwPayUserID"), pReturnBattleCost->dwUserID);
		m_pGameDBAide->AddParameter(TEXT("@lReturnCount"), pReturnBattleCost->lReturnCount);
		m_pGameDBAide->AddParameter(TEXT("@CurrencyKind"), pReturnBattleCost->cbCurrencyKind);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_pGameDBAide->AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_pGameDBAide->ExecuteProcess(TEXT("GSP_GR_ReturnBattleCost"), true);

		//���컺��
		DBO_GR_ReturnBattleCost ReturnBattleCost;
		ZeroMemory(&ReturnBattleCost, sizeof(ReturnBattleCost));

		//�û���Ϣ
		ReturnBattleCost.dwUserID = pReturnBattleCost->dwUserID;
		ReturnBattleCost.dwClubID = pReturnBattleCost->dwClubID;

		//ִ�гɹ�
		if (lResultCode == DB_SUCCESS)
		{
			//��ȡ����			
			ReturnBattleCost.lUserGold = m_pGameDBAide->GetValue_DWORD(TEXT("UserGold"));
			ReturnBattleCost.lUserCard = m_pGameDBAide->GetValue_DWORD(TEXT("UserCard"));

			//����ṹ
			DBO_GR_UserWealthUpdate UserWealthUpdate;
			ZeroMemory(&UserWealthUpdate, sizeof(UserWealthUpdate));

			//���ñ���
			UserWealthUpdate.dwUserID = ReturnBattleCost.dwUserID;
			UserWealthUpdate.lChangeAmount = pReturnBattleCost->lReturnCount;

			//�������
			if (pReturnBattleCost->cbCurrencyKind == CURRENCY_KIND_GOLD)
			{
				//���ñ���
				UserWealthUpdate.wMoneyID = GOODS_ID_GOLD;
				UserWealthUpdate.lBalanceAmount = ReturnBattleCost.lUserGold;
			}

			//��������
			if (pReturnBattleCost->cbCurrencyKind == CURRENCY_KIND_CARD)
			{
				//���ñ���
				UserWealthUpdate.wMoneyID = GOODS_ID_CARD;
				UserWealthUpdate.lBalanceAmount = ReturnBattleCost.lUserCard;
			}

			//������Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE, dwContextID, dwTokenID, &UserWealthUpdate, sizeof(UserWealthUpdate));
		}

		//�����Ϣ
		ReturnBattleCost.lResultCode = lResultCode;		

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RETURN_BATTLE_COST, dwContextID, dwTokenID, &ReturnBattleCost, sizeof(ReturnBattleCost));

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return true;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////
