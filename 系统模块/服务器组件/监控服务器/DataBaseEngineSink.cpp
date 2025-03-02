#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//析构函数
CDataBaseEngineSink::~CDataBaseEngineSink()
{
}

//接口查询
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{

	try
	{
		//创建对象
		if ((m_PlatformDBModule.GetInterface() == NULL) && (m_PlatformDBModule.CreateInstance() == false))
		{
			ASSERT(FALSE);
			return false;
		}

		//变量定义
		tagDataBaseParameter * pPlatformDBParameter = &m_pInitParameter->m_PlatformDBParameter;

		//设置连接
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->dwDataBaseAddr, pPlatformDBParameter->wDataBasePort,
		CW2CT(pPlatformDBParameter->szDataBaseName),CW2CT(pPlatformDBParameter->szDataBaseUser),CW2CT(pPlatformDBParameter->szDataBasePass));
	
		//发起连接
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	//设置对象
	m_PlatformDBAide.SetDataBase(NULL);

	//关闭连接
	if (m_PlatformDBModule.GetInterface() != NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}

	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//时间事件
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//控制事件
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//请求事件
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	switch (wRequestID)
	{		
	case DBR_LOAD_GLOBAL_CONFIG:		//加载配置
	{
		return OnDBEventLoadGlobalConfig(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_QUERY_STOCK_ITEM:			//查询库存
	{
		return OnDBEventQueryStockItem(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_UPDATE_STOCK_SCORE:		//更新库存
	{
		return OnDBEventUpdateStockScore(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_UPDATE_STOCK_STATE:		//更新状态
	{
		return OnDBEventUpdateStockState(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_UPDATE_JACKPOT_SCORE:		//更新彩池
	{
		return OnDBEventUpdateJackpotScore(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_ADJUST_STOCK_SCORE:		//调整库存
	{
		return OnDBEventAdjustStockScore(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_ADJUST_JACKPOT_SCORE:		//调整彩金
	{
		return OnDBEventAdjustJackpotScore(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	}

	return false;
}

//加载库存
bool CDataBaseEngineSink::OnDBEventLoadGlobalConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//提取数据
		auto pLoadGlobalConfig = (DBR_LoadGlobalConfig*)pData;

		//游戏列表
		if (pLoadGlobalConfig->dwConfigMask & CONFIG_MASK_GAME_LIST)
		{
			//读取类型
			m_PlatformDBAide.ResetParameter();
			m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameKindItem"), true);

			//缓冲定义
			WORD wPacketSize = 0;
			BYTE cbBuffer[SOCKET_PACKET] = { 0 };

			//发送类型			
			DBO_GameKind* pGameKind = NULL;
			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				//发送信息
				if ((wPacketSize + sizeof(DBO_GameKind)) > sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GAME_KIND_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);
					wPacketSize = 0;
				}

				//读取信息
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

				//设置位移
				wPacketSize += sizeof(DBO_GameKind);

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}

			if (wPacketSize > 0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GAME_KIND_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);
		}

		//游戏库存
		if (pLoadGlobalConfig->dwConfigMask & CONFIG_MASK_GAME_STOCK)
		{
			//执行查询
			m_PlatformDBAide.ResetParameter();

			//执行查询
			LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_LoadStockInfo"), true);

			if (lResultCode == DB_SUCCESS)
			{
				//缓冲定义
				WORD wBufferSize = 0;
				BYTE cbBuffer[SOCKET_BUFFER] = { 0 };

				//读取信息
				while (m_PlatformDBModule->IsRecordsetEnd() == false)
				{
					//获取对象
					tagStockItem* pStockItem = (tagStockItem*)&cbBuffer[wBufferSize];
					ZeroMemory(pStockItem, sizeof(tagStockItem));

					//读取信息
					pStockItem->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
					pStockItem->wStockID = m_PlatformDBAide.GetValue_WORD(TEXT("StockID"));
					pStockItem->wStockKind = m_PlatformDBAide.GetValue_WORD(TEXT("StockKind"));
					pStockItem->wStockLevel = m_PlatformDBAide.GetValue_WORD(TEXT("StockLevel"));
					pStockItem->lBetAmount = m_PlatformDBAide.GetValue_LONG(TEXT("BetAmount"));

					//启用配置
					pStockItem->cbEnableMode = m_PlatformDBAide.GetValue_BYTE(TEXT("EnableMode"));
					pStockItem->dwEnableEndTime = m_PlatformDBAide.GetValue_DWORD(TEXT("EnableEndTime"));
					pStockItem->dwEnableStartTime = m_PlatformDBAide.GetValue_DWORD(TEXT("EnableStartTime"));

					//积分配置
					pStockItem->lStockScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("StockScore"));
					pStockItem->lExtraScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("ExtraScore"));

					//抽水利润
					pStockItem->lShrinkProfit = m_PlatformDBAide.GetValue_LONGLONG(TEXT("ShrinkProfit"));

					//关联标识
					CHAR szRelationIDList[128] = { 0 };
					m_PlatformDBAide.GetValue_String(TEXT("RelationIDList"), szRelationIDList, CountArray(szRelationIDList));

					//解析配置
					if (szRelationIDList[0] != 0)
					{
						pStockItem->wRelationCount = ParseRelationID(szRelationIDList, pStockItem->RelationStockID, CountArray(pStockItem->RelationStockID));
					}

					//抽水配置
					CHAR szShrinkOption[256] = { 0 };
					m_PlatformDBAide.GetValue_String(TEXT("ShrinkOption"), szShrinkOption, CountArray(szShrinkOption));

					//解析配置
					if (szShrinkOption[0] != 0)
					{
						pStockItem->wShrinkCount = ParseShrinkOption(szShrinkOption, pStockItem->ShrinkItem, CountArray(pStockItem->ShrinkItem));
					}

					//奖池配置
					CHAR szJackpotOption[1024] = { 0 };
					m_PlatformDBAide.GetValue_String(TEXT("JackpotOption"), szJackpotOption, CountArray(szJackpotOption));

					//解析配置
					if (szJackpotOption[0] != 0)
					{
						pStockItem->wJackpotCount = ParseJackpotOption(szJackpotOption, pStockItem->GameJackpotItem, CountArray(pStockItem->GameJackpotItem));
					}

					//更新缓冲
					wBufferSize += sizeof(tagStockItem);

					//移动记录
					m_PlatformDBModule->MoveToNext();

					//发送判断
					if (wBufferSize + sizeof(tagStockItem) > sizeof(cbBuffer))
					{
						//发送信息
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_STOCK_LIST_ITEM, dwContextID, 0, cbBuffer, wBufferSize);
						wBufferSize = 0;
					}
				}

				//发送判断
				if (wBufferSize > 0)
				{
					//发送信息
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_STOCK_LIST_ITEM, dwContextID, 0, cbBuffer, wBufferSize);
				}
			}
		}

		//大R控制
		if (pLoadGlobalConfig->dwConfigMask & CONFIG_MASK_SPLAYER_CONTROL)
		{
			//执行查询
			m_PlatformDBAide.ResetParameter();

			//执行查询
			LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadSuperPlayerControl"), true);

			if (lResultCode == DB_SUCCESS)
			{
				//变量定义
				tagSuperPlayerControl SuperPlayerControl = {};

				//启用状态
				SuperPlayerControl.bEanbleState = (bool)m_PlatformDBAide.GetValue_BYTE(TEXT("EnabledState"));

				//抽水配置
				CHAR szShrinkOption[256] = { 0 };
				m_PlatformDBAide.GetValue_String(TEXT("ShrinkOption"), szShrinkOption, CountArray(szShrinkOption));

				//解析配置
				if (szShrinkOption[0] != 0)
				{
					SuperPlayerControl.wShrinkCount = ParseShrinkOption(szShrinkOption, SuperPlayerControl.ShrinkItem, CountArray(SuperPlayerControl.ShrinkItem));
				}

				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_SUPER_PLAYER_CONTROL, dwContextID, 0, &SuperPlayerControl, sizeof(SuperPlayerControl));
			}
		}

		//发送信息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_LOAD_CONFIG_FINISH, dwContextID, 0, NULL, 0);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}

	return true;
}

//查询库存
bool CDataBaseEngineSink::OnDBEventQueryStockItem(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//转换对象
		auto pQueryStockItem = (DBR_QueryStockItem*)pData;

		//执行查询
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wStockID"), pQueryStockItem->wStockID);

		//执行查询
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryStockInfo"), true);

		if (lResultCode == DB_SUCCESS && m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//缓冲定义
			DBO_QueryStockItem QueryStockItem = {};
			auto pStockItem = &QueryStockItem.StockItem;

			//命令标识
			QueryStockItem.wCommandID = pQueryStockItem->wCommandID;

			//读取信息
			pStockItem->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pStockItem->wStockID = m_PlatformDBAide.GetValue_WORD(TEXT("StockID"));
			pStockItem->wStockKind = m_PlatformDBAide.GetValue_WORD(TEXT("StockKind"));
			pStockItem->wStockLevel = m_PlatformDBAide.GetValue_WORD(TEXT("StockLevel"));
			pStockItem->lBetAmount = m_PlatformDBAide.GetValue_LONG(TEXT("BetAmount"));

			//启用配置
			pStockItem->cbEnableMode = m_PlatformDBAide.GetValue_BYTE(TEXT("EnableMode"));
			pStockItem->dwEnableEndTime = m_PlatformDBAide.GetValue_DWORD(TEXT("EnableEndTime"));
			pStockItem->dwEnableStartTime = m_PlatformDBAide.GetValue_DWORD(TEXT("EnableStartTime"));

			//积分配置
			pStockItem->lStockScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("StockScore"));
			pStockItem->lExtraScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("ExtraScore"));
			
			//抽水利润
			pStockItem->lShrinkProfit = m_PlatformDBAide.GetValue_LONGLONG(TEXT("ShrinkProfit"));

			//关联标识
			CHAR szRelationIDList[128] = { 0 };
			m_PlatformDBAide.GetValue_String(TEXT("RelationIDList"), szRelationIDList, CountArray(szRelationIDList));

			//解析配置
			if (szRelationIDList[0] != 0)
			{
				pStockItem->wRelationCount = ParseRelationID(szRelationIDList, pStockItem->RelationStockID, CountArray(pStockItem->RelationStockID));
			}

			//抽水配置
			CHAR szShrinkOption[256] = { 0 };
			m_PlatformDBAide.GetValue_String(TEXT("ShrinkOption"), szShrinkOption, CountArray(szShrinkOption));

			//解析配置
			if (szShrinkOption[0] != 0)
			{
				pStockItem->wShrinkCount = ParseShrinkOption(szShrinkOption, pStockItem->ShrinkItem, CountArray(pStockItem->ShrinkItem));
			}

			//奖池配置
			CHAR szJackpotOption[1024] = { 0 };
			m_PlatformDBAide.GetValue_String(TEXT("JackpotOption"), szJackpotOption, CountArray(szJackpotOption));

			//解析配置
			if (szJackpotOption[0] != 0)
			{
				pStockItem->wJackpotCount = ParseJackpotOption(szJackpotOption, pStockItem->GameJackpotItem, CountArray(pStockItem->GameJackpotItem));
			}				

			//发送信息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_QUERY_STOCK_ITEM, dwContextID, 0, &QueryStockItem, sizeof(QueryStockItem));
		}		

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//更新库存
bool CDataBaseEngineSink::OnDBEventUpdateStockScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//校验参数
	ASSERT(wDataSize % sizeof(tagStockVariationItem) == 0);
	if (wDataSize % sizeof(tagStockVariationItem) != 0) return false;

	//提取数据
	auto pVariationItem = (tagStockVariationItem*)pData;
	auto wItemCount = wDataSize/sizeof(tagStockVariationItem);

	//构造Json
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

	//添加括号
	strVariationList.Insert(0, L'[');
	strVariationList.AppendChar(L']');

	try
	{
		//执行查询
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@strVariationList"), strVariationList.GetString());

		//执行更新
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_UpdateStockScore"), false);

		//发送消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_UPDATE_STOCK_SCORE, dwContextID, dwTokenID, NULL, 0);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//更新状态
bool CDataBaseEngineSink::OnDBEventUpdateStockState(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//校验参数
	ASSERT(wDataSize%sizeof(tagStateVariationItem)==0);
	if (wDataSize%sizeof(tagStateVariationItem)!=0) return false;

	//提取数据
	auto pVariationItem = (tagStateVariationItem*)pData;
	auto wItemCount = wDataSize/sizeof(tagStateVariationItem);

	//构造Json
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

	//添加括号
	strStockStateList.Insert(0, L'[');
	strStockStateList.AppendChar(L']');

	try
	{
		//执行查询
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@strStockStateLIst"), strStockStateList.GetString());

		//执行更新
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_UpdateStockState"), false);

		//发送消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_UPDATE_STOCK_STATE, dwContextID, dwTokenID, NULL, 0);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//更新彩池
bool CDataBaseEngineSink::OnDBEventUpdateJackpotScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//校验参数
	ASSERT(wDataSize % sizeof(tagJackpotVariationItem) == 0);
	if (wDataSize % sizeof(tagJackpotVariationItem) != 0) return false;

	//提取数据
	auto pVariationItem = (tagJackpotVariationItem*)pData;
	auto wItemCount = wDataSize/sizeof(tagJackpotVariationItem);

	//构造Json
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

	//添加括号
	strVariationList.Insert(0, L'[');
	strVariationList.AppendChar(L']');

	try
	{
		//执行查询
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@strVariationList"), strVariationList.GetString());

		//执行更新
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_UpdateJackpotScore"), false);

		//发送消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_UPDATE_JACKPOT_SCORE, dwContextID, dwTokenID, NULL, 0);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//调整库存
bool CDataBaseEngineSink::OnDBEventAdjustStockScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//校验参数
	ASSERT (wDataSize==sizeof(DBR_AdjustStockScore));
	if (wDataSize != sizeof(DBR_AdjustStockScore)) return false;

	//提取数据
	auto pAdjustStockScore  = (DBR_AdjustStockScore*)pData;

	try
	{
		//执行查询
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wStockID"), pAdjustStockScore->wStockID);
		m_PlatformDBAide.AddParameter(TEXT("@lChangeScore"), pAdjustStockScore->lChangeScore);
		
		//执行更新
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_AdjustStockScore"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//调整彩金
bool CDataBaseEngineSink::OnDBEventAdjustJackpotScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//校验参数
	ASSERT(wDataSize == sizeof(DBR_AdjustJackpotScore));
	if (wDataSize != sizeof(DBR_AdjustJackpotScore)) return false;

	//提取数据
	auto pAdjustJackpotScore = (DBR_AdjustJackpotScore*)pData;

	try
	{
		//执行查询
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wStockID"), pAdjustJackpotScore->wStockID);
		m_PlatformDBAide.AddParameter(TEXT("@wLevelID"), pAdjustJackpotScore->wLevelID);
		m_PlatformDBAide.AddParameter(TEXT("@lChangeScore"), pAdjustJackpotScore->lChangeScore);
		
		//执行更新
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_AdjustJackpotScore"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//解析关联标识
WORD CDataBaseEngineSink::ParseRelationID(LPCSTR szRelationIDList, WORD RelationStockID[], WORD wBufferCount)
{
	//获取对象
	auto JsonValuePtr = CWHJson::getValue(szRelationIDList);
	if (JsonValuePtr->getType() != eJsonTypeArray)
	{
		return 0;
	}

	//变量定义
	WORD wItemCount = 0;

	//获取对象
	auto RelationArrayPtr = CJsonValueArrayPtr::dynamicCast(JsonValuePtr);

	for (auto RelationItemPtr : RelationArrayPtr->value)
	{
		if (RelationItemPtr->getType() == eJsonTypeNum)
		{
			//转换对象
			auto RelationNumPtr = CJsonValueNumPtr::dynamicCast(RelationItemPtr);

			//获取对象
			RelationStockID[wItemCount++] = (WORD)RelationNumPtr->value;

			//退出判断
			if (wItemCount == wBufferCount) break;
		}
	}

	return wItemCount;
}

//解析抽水配置
WORD CDataBaseEngineSink::ParseShrinkOption(LPCSTR szShrinkOption, tagShrinkItem ShrinkItem[], WORD wBufferCount)
{
	//获取对象
	auto JsonValuePtr = CWHJson::getValue(szShrinkOption);
	if (JsonValuePtr->getType() != eJsonTypeArray)
	{
		return 0;
	}

	//变量定义
	WORD wItemCount = 0;

	//获取对象
	auto ShrinkArrayPtr = CJsonValueArrayPtr::dynamicCast(JsonValuePtr);

	for (auto ShrinkItemPtr : ShrinkArrayPtr->value)
	{
		if (ShrinkItemPtr->getType() == eJsonTypeObj)
		{
			//转换对象
			auto ShrinkItemObjPtr = CJsonValueObjPtr::dynamicCast(ShrinkItemPtr);

			//获取对象
			auto pShrinkItem = &ShrinkItem[wItemCount++];

			//读取配置
			CWHJsonInput::readJson(pShrinkItem->wShrinkRatio, ShrinkItemObjPtr->value["ShrinkRatio"], true);
			CWHJsonInput::readJson(pShrinkItem->lShrinkProfit, ShrinkItemObjPtr->value["ShrinkProfit"], true);

			//退出判断
			if (wItemCount == wBufferCount) break;
		}
	}

	return wItemCount;
}

//解析彩金配置
WORD CDataBaseEngineSink::ParseJackpotOption(LPCSTR szJackpotOption, tagJackpotItem JackPotItem[], WORD wBufferCount)
{
	//获取对象
	auto JsonValuePtr = CWHJson::getValue(szJackpotOption);
	if (JsonValuePtr->getType() != eJsonTypeArray)
	{
		return 0;
	}

	//变量定义
	WORD wItemCount = 0;

	//获取对象
	auto JackpotArrayPtr = CJsonValueArrayPtr::dynamicCast(JsonValuePtr);

	for (auto JackpotItemPtr : JackpotArrayPtr->value)
	{
		if (JackpotItemPtr->getType() == eJsonTypeObj)
		{
			//转换对象
			auto JackpotItemObjPtr = CJsonValueObjPtr::dynamicCast(JackpotItemPtr);

			//获取对象
			auto pJackpotItem = &JackPotItem[wItemCount++];

			//读取配置
			CWHJsonInput::readJson(pJackpotItem->wLevelID, JackpotItemObjPtr->value["LevelID"], true);
			CWHJsonInput::readJson(pJackpotItem->lStockScore, JackpotItemObjPtr->value["StockScore"], true);
			CWHJsonInput::readJson(pJackpotItem->lExtraScore, JackpotItemObjPtr->value["ExtraScore"], true);
			CWHJsonInput::readJson(pJackpotItem->lVirtualScore, JackpotItemObjPtr->value["VirtualScore"], true);
			CWHJsonInput::readJson(pJackpotItem->wShrinkRatio, JackpotItemObjPtr->value["ShrinkRatio"], true);
			CWHJsonInput::readJson(pJackpotItem->wPayoutProb, JackpotItemObjPtr->value["PayoutProb"], true);
			CWHJsonInput::readJson(pJackpotItem->lPayoutLimit, JackpotItemObjPtr->value["PayoutLimit"], true);
			CWHJsonInput::readJson(pJackpotItem->lPayoutOrigin, JackpotItemObjPtr->value["PayoutOrigin"], true);

			//退出判断
			if (wItemCount == wBufferCount) break;
		}
	}

	return wItemCount;
}

//输出信息
void CDataBaseEngineSink::TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException)
{
	CString strMessage;
	strMessage.Format(TEXT("%hs(%d):%hs(%s)"), pFile, nLine, pFunction, pIException ? pIException->GetExceptionDescribe() : TEXT(""));
	CTraceService::TraceString(strMessage, TraceLevel_Exception);
}
//////////////////////////////////////////////////////////////////////////

