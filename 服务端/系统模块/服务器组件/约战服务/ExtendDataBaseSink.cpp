#include "stdafx.h"
#include "ExtendDataBaseSink.h"
#include "..\游戏服务\DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////

//输出信息
void TraceDatabaseException(char* pFile, char* pFunction, int nLine, IDataBaseException* pIException)
{
	CString strMessage;
	strMessage.Format(TEXT("%s(%d):%hs(%s)"), CA2CT(pFile).m_psz, nLine, pFunction, pIException ? pIException->GetExceptionDescribe() : TEXT(""));
	CTraceService::TraceString(strMessage, TraceLevel_Exception);
}

///////////////////////////////////////////////////////////////////////////////////
//构造函数
CExtendDataBaseSink::CExtendDataBaseSink()
{
	//设置变量
	m_pGameDBAide=NULL;					
	m_pIGameDBModule=NULL;	
	m_pAccountsDBAide=NULL;				
	m_pIAccountsDBModule=NULL;			
	m_pPlatformDBAide=NULL;				
	m_pIPlatformDBModule=NULL;			

	//配置变量
	m_pGameServiceAttrib=NULL;			
	m_pGameServiceOption=NULL;			

	//组件变量
	m_pITimerEngine=NULL;				
	m_pIDataBaseEngine=NULL;			
	m_pIDataBaseEngineEvent=NULL;		
}

//析构函数
CExtendDataBaseSink::~CExtendDataBaseSink()
{
}

//接口查询
VOID* CExtendDataBaseSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IExtendDataBaseSink, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IExtendDataBaseSink, Guid, dwQueryVer);
	return NULL;
}

//初始化
bool CExtendDataBaseSink::InitExtendDataBaseSink(tagExtendDBSinkParameter& ExtendDBSinkParameter)
{
	//游戏数据库
	m_pGameDBAide = ExtendDBSinkParameter.pGameDBAide;
	m_pIGameDBModule = ExtendDBSinkParameter.pIGameDBModule;

	//账号数据库
	m_pAccountsDBAide = ExtendDBSinkParameter.pAccountsDBAide;
	m_pIAccountsDBModule = ExtendDBSinkParameter.pIAccountsDBModule;

	//平台数据库
	m_pPlatformDBAide = ExtendDBSinkParameter.pPlatformDBAide;
	m_pIPlatformDBModule = ExtendDBSinkParameter.pIPlatformDBModule;

	//配置变量
	m_pGameServiceAttrib = ExtendDBSinkParameter.pGameServiceAttrib;
	m_pGameServiceOption = ExtendDBSinkParameter.pGameServiceOption;

	//组件变量
	m_pITimerEngine = ExtendDBSinkParameter.pITimerEngine;
	m_pIDataBaseEngine = ExtendDBSinkParameter.pIDataBaseEngine;
	m_pIDataBaseEngineEvent = ExtendDBSinkParameter.pIDataBaseEngineEvent;

	return true;
}

//时间事件
bool CExtendDataBaseSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//控制事件
bool CExtendDataBaseSink::OnDataBaseEngineControl(WORD wControlID, VOID* pData, WORD wDataSize)
{
	return false;
}

//请求事件
bool CExtendDataBaseSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//校验标识
	if (wRequestID >= DB_EXTEND_REQUEST_START)
	{
		switch (wRequestID)
		{
		case DBR_GR_BATTLE_LOCK:			//约战锁定
		{
			return OnRequestBattleLock(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_BATTLE_UNLOCK:			//约战解锁
		{
			return OnRequestBattleUnlock(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_BATTLE_RECORD:			//战绩记录
		{
			return OnRequestBattleRecord(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_BATTLE_REWARD:			//约战奖励
		{
			return OnRequestBattleReward(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_DEDUCT_BATTLE_COST:		//扣除费用
		{
			return OnRequestDeductBattleCost(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_DEDUCT_BATTLE_COSTAA:	//扣除费用
		{
			return OnRequestDeductBattleCostAA(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_RETURN_BATTLE_COST:		//退还费用
		{
			return OnRequestReturnBattleCost(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		}
	}

	return false;
}

//锁定用户
bool CExtendDataBaseSink::OnRequestBattleLock(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//效验参数
	ASSERT(wDataSize <= sizeof(DBR_GR_BattleLock));
	if (wDataSize > sizeof(DBR_GR_BattleLock)) return false;

	//提取数据
	DBR_GR_BattleLock* pBattleLock = (DBR_GR_BattleLock*)pData;

	try
	{
		//重置参数
		m_pGameDBAide->ResetParameter();

		//构造标识列表
		CString strUserIDList=TEXT("[");

		for (int i = 0; i < pBattleLock->wUserCount; i++)
		{
			strUserIDList.AppendFormat(TEXT("%d"), pBattleLock->dwUserID[i]);
			if (i < pBattleLock->wUserCount - 1)
			{
				strUserIDList.Append(TEXT(","));
			}
		}

		//添加回括号		
		strUserIDList.Append(TEXT("]"));

		//约战信息
		m_pGameDBAide->AddParameter(TEXT("@dwBattleNum"), pBattleLock->dwBattleNum);
		m_pGameDBAide->AddParameter(TEXT("@cbScoreKind"), pBattleLock->cbScoreKind);

		//插入参数		
		m_pGameDBAide->AddParameter(TEXT("@strUserIDList"), strUserIDList);

		//执行锁定
		m_pGameDBAide->ExecuteProcess(TEXT("GSP_GR_LockBattleUser"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return true;
	}

	return true;
}

//约战解锁
bool CExtendDataBaseSink::OnRequestBattleUnlock(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//效验参数
	ASSERT(wDataSize <= sizeof(DBR_GR_BattleUnlock));
	if (wDataSize > sizeof(DBR_GR_BattleUnlock)) return false;

	//提取数据
	DBR_GR_BattleUnlock* pBattleUnlock = (DBR_GR_BattleUnlock*)pData;

	try
	{
		//重置参数
		m_pGameDBAide->ResetParameter();

		//构造标识列表
		CString strUserIDList = TEXT("[");

		for (int i = 0; i < pBattleUnlock->wUserCount; i++)
		{
			strUserIDList.AppendFormat(TEXT("%d"), pBattleUnlock->dwUserID[i]);
			if (i < pBattleUnlock->wUserCount - 1)
			{
				strUserIDList.Append(TEXT(","));
			}
		}

		//添加回括号		
		strUserIDList.Append(TEXT("]"));

		//插入参数
		m_pGameDBAide->AddParameter(TEXT("@dwBattleNum"), pBattleUnlock->dwBattleNum);		
		m_pGameDBAide->AddParameter(TEXT("@strUserIDList"), strUserIDList);

		//执行锁定
		m_pGameDBAide->ExecuteProcess(TEXT("GSP_GR_UnlockBattleUser"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return true;
	}

	return true;
}

//战绩记录
bool CExtendDataBaseSink::OnRequestBattleRecord(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize <= sizeof(DBR_GR_BattleRecord));
		if (wDataSize > sizeof(DBR_GR_BattleRecord)) return false;

		//请求处理
		DBR_GR_BattleRecord* pBattleRecord = (DBR_GR_BattleRecord*)pData;

		//变量定义		
		CString strScoreItem, strItemList;
		LPCTSTR pszScoreFormat = TEXT("[%s]");
		LPCTSTR pszItemFormat = pszItemFormat = TEXT("\"ChairID\":%d,\"UserID\":%d,\"GameID\":%d,\"UserStatus\":%d,\"NickName\":\"%s\",\"FaceID\":%d,\"CustomID\":%d,\"Gender\":%d,\"RankID\":%d,\"Score\":%I64d,\"SysRevenue\":%I64d,\"ClubRevenue\":%I64d,\"RewardScore\":%I64d,\"WinCount\":%d,\"LostCount\":%d,\"DrawCount\":%d,\"Abstract\":\"%s\"");

		//变量定义
		WORD wStringSize = 0;
		BYTE cbNickName[64];
		CString strItem, strNickName;

		//添加括号
		strItemList.Append(TEXT("["));

		//构造数据		
		for (int i = 0; i < pBattleRecord->wUserCount; i++)
		{
			//提取数据
			tagBattleScoreItem* pBattleScoreItem = &pBattleRecord->BattleUserItem[i];

			//拷贝昵称
			wStringSize = sizeof(TCHAR) * lstrlen(pBattleScoreItem->szNickName);
			CopyMemory(cbNickName, pBattleScoreItem->szNickName, wStringSize);

			//格式化昵称
			strNickName.Empty();
			for (int j = 0; j < wStringSize; j++)
			{
				strItem.Format(TEXT("%02x"), cbNickName[j]);
				strNickName.Append(strItem);
			}

			//格式分数
			strScoreItem.Format(pszItemFormat, pBattleScoreItem->wChairID, pBattleScoreItem->dwUserID, pBattleScoreItem->dwGameID, pBattleScoreItem->cbUserStatus, strNickName, pBattleScoreItem->wFaceID, pBattleScoreItem->dwCustomID, pBattleScoreItem->cbGender, pBattleScoreItem->wRankID,pBattleScoreItem->lScore, pBattleScoreItem->lSysRevenue, pBattleScoreItem->lClubRevenue, pBattleScoreItem->lRewardScore, pBattleScoreItem->lWinCount, pBattleScoreItem->lLostCount, pBattleScoreItem->lDrawCount, pBattleScoreItem->szBattleAbstract);

			//追加分数
			strItemList.Append(TEXT("{"));
			strItemList.Append(strScoreItem);
			strItemList.Append(TEXT("}"));

			// 添加分隔符
			if (i < pBattleRecord->wUserCount - 1)
			{
				strItemList.Append(TEXT(","));
			}
		}

		//添加括号
		strItemList.Append(TEXT("]"));

		//构造参数
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

		//执行查询
		LONG lReturnValue = m_pGameDBAide->ExecuteProcess(TEXT("GSP_GR_RecordBattleGrade"), true);

		return lReturnValue == DB_SUCCESS;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//约战奖励
bool CExtendDataBaseSink::OnRequestBattleReward(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GR_BattleReward));
		if (wDataSize != sizeof(DBR_GR_BattleReward)) return false;

		//请求处理
		auto pBattleReward = (DBR_GR_BattleReward*)pData;

		//构造参数
		m_pGameDBAide->ResetParameter();
		m_pGameDBAide->AddParameter(TEXT("@dwUserID"), pBattleReward->dwUserID);
		m_pGameDBAide->AddParameter(TEXT("@dwRewardGold"), pBattleReward->lRewardGold);
		m_pGameDBAide->AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		m_pGameDBAide->AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);

		//执行查询
		LONG lReturnValue = m_pGameDBAide->ExecuteProcess(TEXT("GSP_GR_BattleReward"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_UserWealthUpdate UserWealthUpdate;
			ZeroMemory(&UserWealthUpdate, sizeof(UserWealthUpdate));

			//设置变量
			UserWealthUpdate.dwUserID = pBattleReward->dwUserID;
			UserWealthUpdate.lChangeAmount = pBattleReward->lRewardGold;

			//设置变量
			UserWealthUpdate.wMoneyID = GOODS_ID_GOLD;
			UserWealthUpdate.lBalanceAmount = m_pGameDBAide->GetValue_DWORD(TEXT("UserGold"));
			
			//发送消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE, dwContextID, dwTokenID, &UserWealthUpdate, sizeof(UserWealthUpdate));
		}

		return lReturnValue == DB_SUCCESS;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//扣除费用
bool CExtendDataBaseSink::OnRequestDeductBattleCost(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//效验参数
	ASSERT(wDataSize == sizeof(DBR_GR_DeductBattleCost));
	if (wDataSize != sizeof(DBR_GR_DeductBattleCost)) return false;

	//提取数据
	DBR_GR_DeductBattleCost* pDeductBattleCost = (DBR_GR_DeductBattleCost*)pData;

	try
	{
		//构造参数
		m_pGameDBAide->ResetParameter();
		m_pGameDBAide->AddParameter(TEXT("@dwUserID"), pDeductBattleCost->dwUserID);
		m_pGameDBAide->AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		m_pGameDBAide->AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);
		m_pGameDBAide->AddParameter(TEXT("@wClubID"), pDeductBattleCost->dwClubID);
		m_pGameDBAide->AddParameter(TEXT("@wPayType"), pDeductBattleCost->wPayType);
		m_pGameDBAide->AddParameter(TEXT("@ConsumeCount"), pDeductBattleCost->lConsumeCount);
		m_pGameDBAide->AddParameter(TEXT("@CurrencyKind"), pDeductBattleCost->cbCurrencyKind);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_pGameDBAide->AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_pGameDBAide->ExecuteProcess(TEXT("GSP_GR_DeductBattleCost"), true);

		//构造缓冲
		DBO_GR_DeductBattleCost DeductBattleCost;
		ZeroMemory(&DeductBattleCost, sizeof(DeductBattleCost));

		//执行成功
		if (lResultCode == DB_SUCCESS)
		{
			//读取数据
			DeductBattleCost.dwPayUserID = m_pGameDBAide->GetValue_DWORD(TEXT("PayUserID"));
			DeductBattleCost.lUserGold = m_pGameDBAide->GetValue_DWORD(TEXT("UserGold"));
			DeductBattleCost.lUserCard = m_pGameDBAide->GetValue_DWORD(TEXT("UserCard"));

			//构造结构
			DBO_GR_UserWealthUpdate UserWealthUpdate;
			ZeroMemory(&UserWealthUpdate, sizeof(UserWealthUpdate));

			//设置变量
			UserWealthUpdate.dwUserID = DeductBattleCost.dwPayUserID;
			UserWealthUpdate.lChangeAmount = -pDeductBattleCost->lConsumeCount;

			//金币类型
			if (pDeductBattleCost->cbCurrencyKind == CURRENCY_KIND_GOLD)
			{
				//设置变量
				UserWealthUpdate.wMoneyID = GOODS_ID_GOLD;
				UserWealthUpdate.lBalanceAmount = DeductBattleCost.lUserGold;
			}

			//房卡类型
			if (pDeductBattleCost->cbCurrencyKind == CURRENCY_KIND_CARD)
			{
				//设置变量
				UserWealthUpdate.wMoneyID = GOODS_ID_CARD;
				UserWealthUpdate.lBalanceAmount = DeductBattleCost.lUserCard;
			}

			//发送消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE, dwContextID, dwTokenID, &UserWealthUpdate, sizeof(UserWealthUpdate));
		}
		else
		{
			//获取信息
			CDBVarValue DBVarValue;
			m_pGameDBAide->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量			
			StringCchCopy(DeductBattleCost.szErrorMessage, CountArray(DeductBattleCost.szErrorMessage), CW2CT(DBVarValue.bstrVal).m_psz);
		}

		//设置变量
		DeductBattleCost.lResultCode = lResultCode;
		DeductBattleCost.wTableID = pDeductBattleCost->wTableID;

		//发送消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DEDUCT_BATTLE_COST, dwContextID, dwTokenID, &DeductBattleCost, sizeof(DeductBattleCost));

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//构造结构
		DBO_GR_DeductBattleCost DeductBattleCost;
		ZeroMemory(&DeductBattleCost, sizeof(DeductBattleCost));

		//设置变量
		DeductBattleCost.lResultCode = -1;
		DeductBattleCost.wTableID = pDeductBattleCost->wTableID;

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DEDUCT_BATTLE_COST, dwContextID, dwTokenID, &DeductBattleCost, sizeof(DeductBattleCost));

		return true;
	}

	return true;
}

//扣除费用
bool CExtendDataBaseSink::OnRequestDeductBattleCostAA(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//效验参数
	ASSERT(wDataSize <= sizeof(DBR_GR_DeductBattleCostAA));
	if (wDataSize > sizeof(DBR_GR_DeductBattleCostAA)) return false;

	//提取数据
	auto pDeductBattleCost = (DBR_GR_DeductBattleCostAA*)pData;

	try
	{
		//变量定义		
		CString strUserList;

		//添加括号
		strUserList.Append(TEXT("["));

		for (int i = 0; i < pDeductBattleCost->wUserCount; i++)
		{
			//获取对象
			tagBattleUser* ptagBattleUser = &pDeductBattleCost->BattleUser[i];
			
			strUserList.AppendFormat(TEXT("%d"), ptagBattleUser->dwUserID);
			if (i < pDeductBattleCost->wUserCount - 1)
			{
				strUserList.Append(TEXT(","));
			}
		}

		//添加回括号		
		strUserList.Append(TEXT("]"));

		//构造参数
		m_pGameDBAide->ResetParameter();
		m_pGameDBAide->AddParameter(TEXT("@strUserList"), strUserList);
		m_pGameDBAide->AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		m_pGameDBAide->AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);
		m_pGameDBAide->AddParameter(TEXT("@wClubID"), pDeductBattleCost->dwClubID);
		m_pGameDBAide->AddParameter(TEXT("@ConsumeCount"), pDeductBattleCost->lConsumeCount);
		m_pGameDBAide->AddParameter(TEXT("@CurrencyKind"), pDeductBattleCost->cbCurrencyKind);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_pGameDBAide->AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_pGameDBAide->ExecuteProcess(TEXT("GSP_GR_DeductBattleCostAA"), true);

		//构造缓冲
		DBO_GR_DeductBattleCost DeductBattleCost;
		ZeroMemory(&DeductBattleCost, sizeof(DeductBattleCost));

		//执行成功
		if (lResultCode == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_UserWealthUpdate UserWealthUpdate;
			ZeroMemory(&UserWealthUpdate, sizeof(UserWealthUpdate));

			//变更金额
			UserWealthUpdate.lChangeAmount = -pDeductBattleCost->lConsumeCount;

			//读取财富
			while (m_pIGameDBModule->IsRecordsetEnd() == false)
			{
				//用户标识
				UserWealthUpdate.dwUserID = m_pGameDBAide->GetValue_DWORD(TEXT("UserID"));

				//金币类型
				if (pDeductBattleCost->cbCurrencyKind == CURRENCY_KIND_GOLD)
				{
					//设置变量
					UserWealthUpdate.wMoneyID = GOODS_ID_GOLD;					
					UserWealthUpdate.lBalanceAmount = m_pGameDBAide->GetValue_LONGLONG(TEXT("UserGold"));
				}

				//房卡类型
				if (pDeductBattleCost->cbCurrencyKind == CURRENCY_KIND_CARD)
				{
					//设置变量
					UserWealthUpdate.wMoneyID = GOODS_ID_CARD;				
					UserWealthUpdate.lBalanceAmount = m_pGameDBAide->GetValue_LONGLONG(TEXT("UserCard"));
				}

				//发送消息
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE, dwContextID, dwTokenID, &UserWealthUpdate, sizeof(UserWealthUpdate));

				//移动记录
				m_pIGameDBModule->MoveToNext();
			}
		}
		else
		{
			DeductBattleCost.dwFailUserID = m_pGameDBAide->GetValue_DWORD(TEXT("FailUserID"));
		}

		//设置变量
		DeductBattleCost.lResultCode = lResultCode;
		DeductBattleCost.wTableID = pDeductBattleCost->wTableID;

		//发送消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DEDUCT_BATTLE_COST, dwContextID, dwTokenID, &DeductBattleCost, sizeof(DeductBattleCost));

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//构造结构
		DBO_GR_DeductBattleCost DeductBattleCost;
		ZeroMemory(&DeductBattleCost, sizeof(DeductBattleCost));

		//设置变量
		DeductBattleCost.lResultCode = -1;
		DeductBattleCost.wTableID = pDeductBattleCost->wTableID;

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DEDUCT_BATTLE_COST, dwContextID, dwTokenID, &DeductBattleCost, sizeof(DeductBattleCost));

		return true;
	}

	return true;
}

//扣除费用
bool CExtendDataBaseSink::OnRequestReturnBattleCost(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//效验参数
	ASSERT(wDataSize == sizeof(DBR_GR_ReturnBattleCost));
	if (wDataSize != sizeof(DBR_GR_ReturnBattleCost)) return false;

	//提取数据
	auto pReturnBattleCost = (DBR_GR_ReturnBattleCost*)pData;

	try
	{
		//构造参数
		m_pGameDBAide->ResetParameter();
		m_pGameDBAide->AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		m_pGameDBAide->AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);
		m_pGameDBAide->AddParameter(TEXT("@wClubID"), pReturnBattleCost->dwClubID);
		m_pGameDBAide->AddParameter(TEXT("@dwPayUserID"), pReturnBattleCost->dwUserID);
		m_pGameDBAide->AddParameter(TEXT("@lReturnCount"), pReturnBattleCost->lReturnCount);
		m_pGameDBAide->AddParameter(TEXT("@CurrencyKind"), pReturnBattleCost->cbCurrencyKind);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_pGameDBAide->AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lResultCode = m_pGameDBAide->ExecuteProcess(TEXT("GSP_GR_ReturnBattleCost"), true);

		//构造缓冲
		DBO_GR_ReturnBattleCost ReturnBattleCost;
		ZeroMemory(&ReturnBattleCost, sizeof(ReturnBattleCost));

		//用户信息
		ReturnBattleCost.dwUserID = pReturnBattleCost->dwUserID;
		ReturnBattleCost.dwClubID = pReturnBattleCost->dwClubID;

		//执行成功
		if (lResultCode == DB_SUCCESS)
		{
			//读取数据			
			ReturnBattleCost.lUserGold = m_pGameDBAide->GetValue_DWORD(TEXT("UserGold"));
			ReturnBattleCost.lUserCard = m_pGameDBAide->GetValue_DWORD(TEXT("UserCard"));

			//构造结构
			DBO_GR_UserWealthUpdate UserWealthUpdate;
			ZeroMemory(&UserWealthUpdate, sizeof(UserWealthUpdate));

			//设置变量
			UserWealthUpdate.dwUserID = ReturnBattleCost.dwUserID;
			UserWealthUpdate.lChangeAmount = pReturnBattleCost->lReturnCount;

			//金币类型
			if (pReturnBattleCost->cbCurrencyKind == CURRENCY_KIND_GOLD)
			{
				//设置变量
				UserWealthUpdate.wMoneyID = GOODS_ID_GOLD;
				UserWealthUpdate.lBalanceAmount = ReturnBattleCost.lUserGold;
			}

			//房卡类型
			if (pReturnBattleCost->cbCurrencyKind == CURRENCY_KIND_CARD)
			{
				//设置变量
				UserWealthUpdate.wMoneyID = GOODS_ID_CARD;
				UserWealthUpdate.lBalanceAmount = ReturnBattleCost.lUserCard;
			}

			//发送消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE, dwContextID, dwTokenID, &UserWealthUpdate, sizeof(UserWealthUpdate));
		}

		//结果信息
		ReturnBattleCost.lResultCode = lResultCode;		

		//发送消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RETURN_BATTLE_COST, dwContextID, dwTokenID, &ReturnBattleCost, sizeof(ReturnBattleCost));

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return true;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////
