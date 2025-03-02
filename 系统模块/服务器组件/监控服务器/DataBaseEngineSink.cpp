#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//��������
CDataBaseEngineSink::~CDataBaseEngineSink()
{
}

//�ӿڲ�ѯ
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{

	try
	{
		//��������
		if ((m_PlatformDBModule.GetInterface() == NULL) && (m_PlatformDBModule.CreateInstance() == false))
		{
			ASSERT(FALSE);
			return false;
		}

		//��������
		tagDataBaseParameter * pPlatformDBParameter = &m_pInitParameter->m_PlatformDBParameter;

		//��������
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->dwDataBaseAddr, pPlatformDBParameter->wDataBasePort,
		CW2CT(pPlatformDBParameter->szDataBaseName),CW2CT(pPlatformDBParameter->szDataBaseUser),CW2CT(pPlatformDBParameter->szDataBasePass));
	
		//��������
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//ֹͣ�¼�
bool CDataBaseEngineSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	//���ö���
	m_PlatformDBAide.SetDataBase(NULL);

	//�ر�����
	if (m_PlatformDBModule.GetInterface() != NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}

	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//ʱ���¼�
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	switch (wRequestID)
	{		
	case DBR_LOAD_GLOBAL_CONFIG:		//��������
	{
		return OnDBEventLoadGlobalConfig(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_QUERY_STOCK_ITEM:			//��ѯ���
	{
		return OnDBEventQueryStockItem(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_UPDATE_STOCK_SCORE:		//���¿��
	{
		return OnDBEventUpdateStockScore(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_UPDATE_STOCK_STATE:		//����״̬
	{
		return OnDBEventUpdateStockState(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_UPDATE_JACKPOT_SCORE:		//���²ʳ�
	{
		return OnDBEventUpdateJackpotScore(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_ADJUST_STOCK_SCORE:		//�������
	{
		return OnDBEventAdjustStockScore(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_ADJUST_JACKPOT_SCORE:		//�����ʽ�
	{
		return OnDBEventAdjustJackpotScore(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	}

	return false;
}

//���ؿ��
bool CDataBaseEngineSink::OnDBEventLoadGlobalConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//��ȡ����
		auto pLoadGlobalConfig = (DBR_LoadGlobalConfig*)pData;

		//��Ϸ�б�
		if (pLoadGlobalConfig->dwConfigMask & CONFIG_MASK_GAME_LIST)
		{
			//��ȡ����
			m_PlatformDBAide.ResetParameter();
			m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameKindItem"), true);

			//���嶨��
			WORD wPacketSize = 0;
			BYTE cbBuffer[SOCKET_PACKET] = { 0 };

			//��������			
			DBO_GameKind* pGameKind = NULL;
			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				//������Ϣ
				if ((wPacketSize + sizeof(DBO_GameKind)) > sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GAME_KIND_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);
					wPacketSize = 0;
				}

				//��ȡ��Ϣ
				pGameKind = (DBO_GameKind*)(cbBuffer + wPacketSize);
				pGameKind->wModuleID = m_PlatformDBAide.GetValue_WORD(TEXT("ModuleID"));
				pGameKind->bNullity = m_PlatformDBAide.GetValue_BYTE(TEXT("Nullity"));
				pGameKind->wSortID = m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
				pGameKind->wTypeID = m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
				pGameKind->wJoinID = m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
				pGameKind->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
				pGameKind->wSupportMask = m_PlatformDBAide.GetValue_WORD(TEXT("SupportMask"));
				m_PlatformDBAide.GetValue_String(TEXT("KindName"), pGameKind->szKindName, CountArray(pGameKind->szKindName));
				m_PlatformDBAide.GetValue_String(TEXT("ProcessName"), pGameKind->szProcessName, CountArray(pGameKind->szProcessName));

				//����λ��
				wPacketSize += sizeof(DBO_GameKind);

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			if (wPacketSize > 0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GAME_KIND_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);
		}

		//��Ϸ���
		if (pLoadGlobalConfig->dwConfigMask & CONFIG_MASK_GAME_STOCK)
		{
			//ִ�в�ѯ
			m_PlatformDBAide.ResetParameter();

			//ִ�в�ѯ
			LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadStockInfo"), true);

			if (lResultCode == DB_SUCCESS)
			{
				//���嶨��
				WORD wBufferSize = 0;
				BYTE cbBuffer[SOCKET_BUFFER] = { 0 };

				//��ȡ��Ϣ
				while (m_PlatformDBModule->IsRecordsetEnd() == false)
				{
					//��ȡ����
					tagStockItem* pStockItem = (tagStockItem*)&cbBuffer[wBufferSize];
					ZeroMemory(pStockItem, sizeof(tagStockItem));

					//��ȡ��Ϣ
					pStockItem->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
					pStockItem->wStockID = m_PlatformDBAide.GetValue_WORD(TEXT("StockID"));
					pStockItem->wStockKind = m_PlatformDBAide.GetValue_WORD(TEXT("StockKind"));
					pStockItem->wStockLevel = m_PlatformDBAide.GetValue_WORD(TEXT("StockLevel"));
					pStockItem->lBetAmount = m_PlatformDBAide.GetValue_LONG(TEXT("BetAmount"));

					//��������
					pStockItem->cbEnableMode = m_PlatformDBAide.GetValue_BYTE(TEXT("EnableMode"));
					pStockItem->dwEnableEndTime = m_PlatformDBAide.GetValue_DWORD(TEXT("EnableEndTime"));
					pStockItem->dwEnableStartTime = m_PlatformDBAide.GetValue_DWORD(TEXT("EnableStartTime"));

					//��������
					pStockItem->lStockScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("StockScore"));
					pStockItem->lExtraScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("ExtraScore"));

					//��ˮ����
					pStockItem->lShrinkProfit = m_PlatformDBAide.GetValue_LONGLONG(TEXT("ShrinkProfit"));

					//������ʶ
					CHAR szRelationIDList[128] = { 0 };
					m_PlatformDBAide.GetValue_String(TEXT("RelationIDList"), szRelationIDList, CountArray(szRelationIDList));

					//��������
					if (szRelationIDList[0] != 0)
					{
						pStockItem->wRelationCount = ParseRelationID(szRelationIDList, pStockItem->RelationStockID, CountArray(pStockItem->RelationStockID));
					}

					//��ˮ����
					CHAR szShrinkOption[256] = { 0 };
					m_PlatformDBAide.GetValue_String(TEXT("ShrinkOption"), szShrinkOption, CountArray(szShrinkOption));

					//��������
					if (szShrinkOption[0] != 0)
					{
						pStockItem->wShrinkCount = ParseShrinkOption(szShrinkOption, pStockItem->ShrinkItem, CountArray(pStockItem->ShrinkItem));
					}

					//��������
					CHAR szJackpotOption[1024] = { 0 };
					m_PlatformDBAide.GetValue_String(TEXT("JackpotOption"), szJackpotOption, CountArray(szJackpotOption));

					//��������
					if (szJackpotOption[0] != 0)
					{
						pStockItem->wJackpotCount = ParseJackpotOption(szJackpotOption, pStockItem->GameJackpotItem, CountArray(pStockItem->GameJackpotItem));
					}

					//���»���
					wBufferSize += sizeof(tagStockItem);

					//�ƶ���¼
					m_PlatformDBModule->MoveToNext();

					//�����ж�
					if (wBufferSize + sizeof(tagStockItem) > sizeof(cbBuffer))
					{
						//������Ϣ
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_STOCK_LIST_ITEM, dwContextID, 0, cbBuffer, wBufferSize);
						wBufferSize = 0;
					}
				}

				//�����ж�
				if (wBufferSize > 0)
				{
					//������Ϣ
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_STOCK_LIST_ITEM, dwContextID, 0, cbBuffer, wBufferSize);
				}
			}
		}

		//��R����
		if (pLoadGlobalConfig->dwConfigMask & CONFIG_MASK_SPLAYER_CONTROL)
		{
			//ִ�в�ѯ
			m_PlatformDBAide.ResetParameter();

			//ִ�в�ѯ
			LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadSuperPlayerControl"), true);

			if (lResultCode == DB_SUCCESS)
			{
				//��������
				tagSuperPlayerControl SuperPlayerControl = {};

				//����״̬
				SuperPlayerControl.bEanbleState = (bool)m_PlatformDBAide.GetValue_BYTE(TEXT("EnabledState"));

				//��ˮ����
				CHAR szShrinkOption[256] = { 0 };
				m_PlatformDBAide.GetValue_String(TEXT("ShrinkOption"), szShrinkOption, CountArray(szShrinkOption));

				//��������
				if (szShrinkOption[0] != 0)
				{
					SuperPlayerControl.wShrinkCount = ParseShrinkOption(szShrinkOption, SuperPlayerControl.ShrinkItem, CountArray(SuperPlayerControl.ShrinkItem));
				}

				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_SUPER_PLAYER_CONTROL, dwContextID, 0, &SuperPlayerControl, sizeof(SuperPlayerControl));
			}
		}

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LOAD_CONFIG_FINISH, dwContextID, 0, NULL, 0);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//��ѯ���
bool CDataBaseEngineSink::OnDBEventQueryStockItem(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//ת������
		auto pQueryStockItem = (DBR_QueryStockItem*)pData;

		//ִ�в�ѯ
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wStockID"), pQueryStockItem->wStockID);

		//ִ�в�ѯ
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryStockInfo"), true);

		if (lResultCode == DB_SUCCESS && m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//���嶨��
			DBO_QueryStockItem QueryStockItem = {};
			auto pStockItem = &QueryStockItem.StockItem;

			//�����ʶ
			QueryStockItem.wCommandID = pQueryStockItem->wCommandID;

			//��ȡ��Ϣ
			pStockItem->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pStockItem->wStockID = m_PlatformDBAide.GetValue_WORD(TEXT("StockID"));
			pStockItem->wStockKind = m_PlatformDBAide.GetValue_WORD(TEXT("StockKind"));
			pStockItem->wStockLevel = m_PlatformDBAide.GetValue_WORD(TEXT("StockLevel"));
			pStockItem->lBetAmount = m_PlatformDBAide.GetValue_LONG(TEXT("BetAmount"));

			//��������
			pStockItem->cbEnableMode = m_PlatformDBAide.GetValue_BYTE(TEXT("EnableMode"));
			pStockItem->dwEnableEndTime = m_PlatformDBAide.GetValue_DWORD(TEXT("EnableEndTime"));
			pStockItem->dwEnableStartTime = m_PlatformDBAide.GetValue_DWORD(TEXT("EnableStartTime"));

			//��������
			pStockItem->lStockScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("StockScore"));
			pStockItem->lExtraScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("ExtraScore"));
			
			//��ˮ����
			pStockItem->lShrinkProfit = m_PlatformDBAide.GetValue_LONGLONG(TEXT("ShrinkProfit"));

			//������ʶ
			CHAR szRelationIDList[128] = { 0 };
			m_PlatformDBAide.GetValue_String(TEXT("RelationIDList"), szRelationIDList, CountArray(szRelationIDList));

			//��������
			if (szRelationIDList[0] != 0)
			{
				pStockItem->wRelationCount = ParseRelationID(szRelationIDList, pStockItem->RelationStockID, CountArray(pStockItem->RelationStockID));
			}

			//��ˮ����
			CHAR szShrinkOption[256] = { 0 };
			m_PlatformDBAide.GetValue_String(TEXT("ShrinkOption"), szShrinkOption, CountArray(szShrinkOption));

			//��������
			if (szShrinkOption[0] != 0)
			{
				pStockItem->wShrinkCount = ParseShrinkOption(szShrinkOption, pStockItem->ShrinkItem, CountArray(pStockItem->ShrinkItem));
			}

			//��������
			CHAR szJackpotOption[1024] = { 0 };
			m_PlatformDBAide.GetValue_String(TEXT("JackpotOption"), szJackpotOption, CountArray(szJackpotOption));

			//��������
			if (szJackpotOption[0] != 0)
			{
				pStockItem->wJackpotCount = ParseJackpotOption(szJackpotOption, pStockItem->GameJackpotItem, CountArray(pStockItem->GameJackpotItem));
			}				

			//������Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_QUERY_STOCK_ITEM, dwContextID, 0, &QueryStockItem, sizeof(QueryStockItem));
		}		

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//���¿��
bool CDataBaseEngineSink::OnDBEventUpdateStockScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//У�����
	ASSERT(wDataSize % sizeof(tagStockVariationItem) == 0);
	if (wDataSize % sizeof(tagStockVariationItem) != 0) return false;

	//��ȡ����
	auto pVariationItem = (tagStockVariationItem*)pData;
	auto wItemCount = wDataSize/sizeof(tagStockVariationItem);

	//����Json
	CString strVariationList, strVariationItem;
	LPCTSTR pszVariationFmt = TEXT("{\"StockID\":%d,\"Score\":%I64d,\"Profit\":%I64d}");

	for (int i = 0; i < wItemCount; i++)
	{
		strVariationItem.Format(pszVariationFmt, pVariationItem->wStockID, pVariationItem->lVariationScore, pVariationItem->lVariationProfit);
		if (!strVariationList.IsEmpty())
		{
			strVariationList.Append(TEXT(","));
		}
		strVariationList.Append(strVariationItem);

		pVariationItem++;
	}

	//�������
	strVariationList.Insert(0, L'[');
	strVariationList.AppendChar(L']');

	try
	{
		//ִ�в�ѯ
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@strVariationList"), strVariationList.GetString());

		//ִ�и���
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_UpdateStockScore"), false);

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_UPDATE_STOCK_SCORE, dwContextID, dwTokenID, NULL, 0);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//����״̬
bool CDataBaseEngineSink::OnDBEventUpdateStockState(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//У�����
	ASSERT(wDataSize%sizeof(tagStateVariationItem)==0);
	if (wDataSize%sizeof(tagStateVariationItem)!=0) return false;

	//��ȡ����
	auto pVariationItem = (tagStateVariationItem*)pData;
	auto wItemCount = wDataSize/sizeof(tagStateVariationItem);

	//����Json
	CString strStockStateList,strStockStateItem;	
	LPCTSTR pszStockStateFmt = TEXT("{\"StockID\":%d,\"State\":%d,\"ControlID\":%I64d}");

	for (int i = 0; i < wItemCount; i++)
	{
		strStockStateItem.Format(pszStockStateFmt, pVariationItem->wStockID, pVariationItem->cbStockState, pVariationItem->lStockControlID);
		if (!strStockStateList.IsEmpty()) 
		{			
			strStockStateList.Append(TEXT(","));
		}
		strStockStateList.Append(strStockStateItem);

		pVariationItem++;
	}

	//�������
	strStockStateList.Insert(0, L'[');
	strStockStateList.AppendChar(L']');

	try
	{
		//ִ�в�ѯ
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@strStockStateLIst"), strStockStateList.GetString());

		//ִ�и���
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_UpdateStockState"), false);

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_UPDATE_STOCK_STATE, dwContextID, dwTokenID, NULL, 0);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//���²ʳ�
bool CDataBaseEngineSink::OnDBEventUpdateJackpotScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//У�����
	ASSERT(wDataSize % sizeof(tagJackpotVariationItem) == 0);
	if (wDataSize % sizeof(tagJackpotVariationItem) != 0) return false;

	//��ȡ����
	auto pVariationItem = (tagJackpotVariationItem*)pData;
	auto wItemCount = wDataSize/sizeof(tagJackpotVariationItem);

	//����Json
	CString strVariationList, strVariationItem;
	LPCTSTR pszVariationFmt = TEXT("{\"StockID\":%d,\"LevelID\":%d,\"Score\":%I64d}");

	for (int i = 0; i < wItemCount; i++)
	{
		strVariationItem.Format(pszVariationFmt, pVariationItem->wStockID, pVariationItem->wLevelID, pVariationItem->lVariationScore);
		if (!strVariationList.IsEmpty())
		{
			strVariationList.Append(TEXT(","));
		}
		strVariationList.Append(strVariationItem);

		pVariationItem++;
	}

	//�������
	strVariationList.Insert(0, L'[');
	strVariationList.AppendChar(L']');

	try
	{
		//ִ�в�ѯ
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@strVariationList"), strVariationList.GetString());

		//ִ�и���
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_UpdateJackpotScore"), false);

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_UPDATE_JACKPOT_SCORE, dwContextID, dwTokenID, NULL, 0);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//�������
bool CDataBaseEngineSink::OnDBEventAdjustStockScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//У�����
	ASSERT (wDataSize==sizeof(DBR_AdjustStockScore));
	if (wDataSize != sizeof(DBR_AdjustStockScore)) return false;

	//��ȡ����
	auto pAdjustStockScore  = (DBR_AdjustStockScore*)pData;

	try
	{
		//ִ�в�ѯ
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wStockID"), pAdjustStockScore->wStockID);
		m_PlatformDBAide.AddParameter(TEXT("@lChangeScore"), pAdjustStockScore->lChangeScore);
		
		//ִ�и���
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_AdjustStockScore"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//�����ʽ�
bool CDataBaseEngineSink::OnDBEventAdjustJackpotScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//У�����
	ASSERT(wDataSize == sizeof(DBR_AdjustJackpotScore));
	if (wDataSize != sizeof(DBR_AdjustJackpotScore)) return false;

	//��ȡ����
	auto pAdjustJackpotScore = (DBR_AdjustJackpotScore*)pData;

	try
	{
		//ִ�в�ѯ
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wStockID"), pAdjustJackpotScore->wStockID);
		m_PlatformDBAide.AddParameter(TEXT("@wLevelID"), pAdjustJackpotScore->wLevelID);
		m_PlatformDBAide.AddParameter(TEXT("@lChangeScore"), pAdjustJackpotScore->lChangeScore);
		
		//ִ�и���
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_AdjustJackpotScore"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//����������ʶ
WORD CDataBaseEngineSink::ParseRelationID(LPCSTR szRelationIDList, WORD RelationStockID[], WORD wBufferCount)
{
	//��ȡ����
	auto JsonValuePtr = CWHJson::getValue(szRelationIDList);
	if (JsonValuePtr->getType() != eJsonTypeArray)
	{
		return 0;
	}

	//��������
	WORD wItemCount = 0;

	//��ȡ����
	auto RelationArrayPtr = CJsonValueArrayPtr::dynamicCast(JsonValuePtr);

	for (auto RelationItemPtr : RelationArrayPtr->value)
	{
		if (RelationItemPtr->getType() == eJsonTypeNum)
		{
			//ת������
			auto RelationNumPtr = CJsonValueNumPtr::dynamicCast(RelationItemPtr);

			//��ȡ����
			RelationStockID[wItemCount++] = (WORD)RelationNumPtr->value;

			//�˳��ж�
			if (wItemCount == wBufferCount) break;
		}
	}

	return wItemCount;
}

//������ˮ����
WORD CDataBaseEngineSink::ParseShrinkOption(LPCSTR szShrinkOption, tagShrinkItem ShrinkItem[], WORD wBufferCount)
{
	//��ȡ����
	auto JsonValuePtr = CWHJson::getValue(szShrinkOption);
	if (JsonValuePtr->getType() != eJsonTypeArray)
	{
		return 0;
	}

	//��������
	WORD wItemCount = 0;

	//��ȡ����
	auto ShrinkArrayPtr = CJsonValueArrayPtr::dynamicCast(JsonValuePtr);

	for (auto ShrinkItemPtr : ShrinkArrayPtr->value)
	{
		if (ShrinkItemPtr->getType() == eJsonTypeObj)
		{
			//ת������
			auto ShrinkItemObjPtr = CJsonValueObjPtr::dynamicCast(ShrinkItemPtr);

			//��ȡ����
			auto pShrinkItem = &ShrinkItem[wItemCount++];

			//��ȡ����
			CWHJsonInput::readJson(pShrinkItem->wShrinkRatio, ShrinkItemObjPtr->value["ShrinkRatio"], true);
			CWHJsonInput::readJson(pShrinkItem->lShrinkProfit, ShrinkItemObjPtr->value["ShrinkProfit"], true);

			//�˳��ж�
			if (wItemCount == wBufferCount) break;
		}
	}

	return wItemCount;
}

//�����ʽ�����
WORD CDataBaseEngineSink::ParseJackpotOption(LPCSTR szJackpotOption, tagJackpotItem JackPotItem[], WORD wBufferCount)
{
	//��ȡ����
	auto JsonValuePtr = CWHJson::getValue(szJackpotOption);
	if (JsonValuePtr->getType() != eJsonTypeArray)
	{
		return 0;
	}

	//��������
	WORD wItemCount = 0;

	//��ȡ����
	auto JackpotArrayPtr = CJsonValueArrayPtr::dynamicCast(JsonValuePtr);

	for (auto JackpotItemPtr : JackpotArrayPtr->value)
	{
		if (JackpotItemPtr->getType() == eJsonTypeObj)
		{
			//ת������
			auto JackpotItemObjPtr = CJsonValueObjPtr::dynamicCast(JackpotItemPtr);

			//��ȡ����
			auto pJackpotItem = &JackPotItem[wItemCount++];

			//��ȡ����
			CWHJsonInput::readJson(pJackpotItem->wLevelID, JackpotItemObjPtr->value["LevelID"], true);
			CWHJsonInput::readJson(pJackpotItem->lStockScore, JackpotItemObjPtr->value["StockScore"], true);
			CWHJsonInput::readJson(pJackpotItem->lExtraScore, JackpotItemObjPtr->value["ExtraScore"], true);
			CWHJsonInput::readJson(pJackpotItem->lVirtualScore, JackpotItemObjPtr->value["VirtualScore"], true);
			CWHJsonInput::readJson(pJackpotItem->wShrinkRatio, JackpotItemObjPtr->value["ShrinkRatio"], true);
			CWHJsonInput::readJson(pJackpotItem->wPayoutProb, JackpotItemObjPtr->value["PayoutProb"], true);
			CWHJsonInput::readJson(pJackpotItem->lPayoutLimit, JackpotItemObjPtr->value["PayoutLimit"], true);
			CWHJsonInput::readJson(pJackpotItem->lPayoutOrigin, JackpotItemObjPtr->value["PayoutOrigin"], true);

			//�˳��ж�
			if (wItemCount == wBufferCount) break;
		}
	}

	return wItemCount;
}

//�����Ϣ
void CDataBaseEngineSink::TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException)
{
	CString strMessage;
	strMessage.Format(TEXT("%hs(%d):%hs(%s)"), pFile, nLine, pFunction, pIException ? pIException->GetExceptionDescribe() : TEXT(""));
	CTraceService::TraceString(strMessage, TraceLevel_Exception);
}
//////////////////////////////////////////////////////////////////////////

