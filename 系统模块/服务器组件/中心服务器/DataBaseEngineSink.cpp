#include "StdAfx.h"
#include "ServiceUnits.h"
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
	//创建对象
	if ((m_AccountsDBModule.GetInterface() == NULL) && (m_AccountsDBModule.CreateInstance() == false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_PlatformDBModule.GetInterface() == NULL) && (m_PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	try
	{
		//变量定义
		tagDataBaseParameter * pPlatformParameter=&m_pInitParameter->m_PlatformDBParameter;
		tagDataBaseParameter * pAccountsParameter=&m_pInitParameter->m_AccountsDBParameter;

		//设置连接
		m_PlatformDBModule->SetConnectionInfo(pPlatformParameter->dwDataBaseAddr, pPlatformParameter->wDataBasePort,
		CW2CT(pPlatformParameter->szDataBaseName),CW2CT(pPlatformParameter->szDataBaseUser),CW2CT(pPlatformParameter->szDataBasePass));

		//设置连接
		m_AccountsDBModule->SetConnectionInfo(pAccountsParameter->dwDataBaseAddr, pAccountsParameter->wDataBasePort,
			CW2CT(pAccountsParameter->szDataBaseName), CW2CT(pAccountsParameter->szDataBaseUser), CW2CT(pAccountsParameter->szDataBasePass));

		//发起连接
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

		//发起连接
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//连接选项
		auto pRedisConnOption = &m_pInitParameter->m_RedisConnOption;

		//变量定义
		ConnectionOptions ConnectionOpts;
		
		//用户连接
		ConnectionOpts.db = pRedisConnOption->wDB;
		ConnectionOpts.port = pRedisConnOption->wPort;
		ConnectionOpts.host = string(CT2CA(pRedisConnOption->szHost).m_psz);
		ConnectionOpts.user = string(CT2CA(pRedisConnOption->szUser).m_psz);
		ConnectionOpts.password = string(CT2CA(pRedisConnOption->szPassword).m_psz);

		//超时时长
		ConnectionOpts.socket_timeout = std::chrono::milliseconds(1000);
		ConnectionOpts.connect_timeout = std::chrono::milliseconds(1000);

		//创建对象
		m_pRedisClient = new Redis(ConnectionOpts);
		if (m_pRedisClient == NULL) throw TEXT("RedisClient创建失败");

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		return false;
	}
	catch (std::exception ex)
	{
		CTraceService::TraceString(CA2CT(ex.what()).m_psz, TraceLevel_Exception);

		return false;
	}

	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	//设置对象
	m_PlatformDBAide.SetDataBase(NULL);
	m_AccountsDBAide.SetDataBase(NULL);

	//删除对象
	SafeDelete(m_pRedisClient);	

	//关闭连接
	if (m_PlatformDBModule.GetInterface()!=NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}

	//关闭连接
	if (m_AccountsDBModule.GetInterface() != NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
	}

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
	case DBR_GP_LOAD_CONFIG:		//加载配置
	{
		return OnRequestLoadConfig(dwContextID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_UNLOCK_PLAYER:		//解锁玩家
	{
		return OnRequestUnlockPlayer(dwContextID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_REQUEST_WEBPAGE:	//请求网页
	{
		return OnRequestRequestWebPage(dwContextID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_ONLINE_COUNT_INFO: //统计在线
	{
		return OnRequestOnLineCountInfo(dwContextID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_GP_UPDATE_GATE_LIST:	//更新网关
	{
		return OnRequestUpdateGateList(dwContextID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_UPDATE_SERVER_LIST:	//更新房间
	{
		return OnRequestUpdateServerList(dwContextID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_UPDATE_RANKING_LIST://更新榜单
	{
		return OnRequestUpdateRankingList(dwContextID, pData, wDataSize, pdwControlCode);
	}
	}

	return false;
}

//加载库存
bool CDataBaseEngineSink::OnRequestLoadConfig(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//发送信息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CONFIG_FINISH, dwContextID, 0, NULL, 0);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}
}

//解锁玩家
bool CDataBaseEngineSink::OnRequestUnlockPlayer(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//校验参数
	ASSERT(wDataSize == sizeof(DBR_GP_UnlockPlayer));
	if (wDataSize != sizeof(DBR_GP_UnlockPlayer)) return false;

	//变量定义
	auto pUnlcokPlayer = (DBR_GP_UnlockPlayer*)pData;

	//构造结果
	DBO_GP_UnlockPlayer UnlockPlayer;
	ZeroMemory(&UnlockPlayer, sizeof(UnlockPlayer));

	//设置变量
	UnlockPlayer.dwUserID = pUnlcokPlayer->dwUserID;	

	try
	{
		//执行查询
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUnlcokPlayer->dwUserID);
		
		//执行查询
		m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CleanOnlineInfo"), false);

		//发送信息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_UNLOCK_PLAYER, dwContextID, 0, &UnlockPlayer, sizeof(UnlockPlayer));

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

//请求网页
bool CDataBaseEngineSink::OnRequestRequestWebPage(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//提取数据
	ASSERT(wDataSize <= sizeof(DBR_GP_RequestWebPage));
	if (wDataSize > sizeof(DBR_GP_RequestWebPage)) return false;

	//提取数据
	DBR_GP_RequestWebPage * pRequestWebPage = (DBR_GP_RequestWebPage *)pData;

	//请求网页
	m_WininetHttp.RequestWebPage(pRequestWebPage->szWebUrl, Hr_Post, NULL, pRequestWebPage->szPostData);

	//请求错误
	if (m_WininetHttp.GetLastError() != Hi_Success)
	{
		//输出错误
		CTraceService::TraceString(TEXT("网页请求错误[%d]：%hs"), TraceLevel_Exception, m_WininetHttp.GetLastError(), pRequestWebPage->szWebUrl);
	}

	return true;
}

//在线信息
bool CDataBaseEngineSink::OnRequestOnLineCountInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//构造参数
		m_PlatformDBAide.ResetParameter();
		
		//执行命令
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_OnLineCountCollect"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}
}

//更新网关
bool CDataBaseEngineSink::OnRequestUpdateGateList(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//提取数据
		DBR_UpdateGateList* pUpdateGateList = (DBR_UpdateGateList*)pData;

		//构造变量
		StringView sGateKey(RKEY_PREFIX_GATELIST);
		StringView sGateValue(pUpdateGateList->szGateList);

		//设置key
		m_pRedisClient->set(sGateKey, sGateValue);

		return true;
	}
	catch (exception& ex)
	{
		TCHAR szDescribe[512] = { 0 };
		_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("Redis更新网关失败：%hs"), ex.what());
		CTraceService::TraceString(szDescribe, TraceLevel_Exception);
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
	}

	return false;
}

//上报房间
bool CDataBaseEngineSink::OnRequestUpdateServerList(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//提取数据
		auto pUpdateServerList = (DBR_GP_UpdateServerList*)pData;

		//数据校验
		if (pUpdateServerList->szServerList[0] == 0)
		{
			if (pUpdateServerList->cbUpdateKind == UPDATE_KIND_DELETE)
			{
				StringView sKey(RKEY_PREFIX_SERVERLIST);
				m_pRedisClient->del(sKey);
			}
		}
		else
		{
			//编码转换
			string szServerList;
			CWHService::Unicode2Utf8(pUpdateServerList->szServerList, szServerList);

			//解析数组
			auto JsonValueObj = CWHJson::getValue(szServerList);
			if (JsonValueObj->getType() != eJsonTypeArray)
			{
				throw L"房间列表格式错误！";
			}

			//转换对象
			auto JsonValueArrayPtr = CJsonValueArrayPtr::dynamicCast(JsonValueObj);

			//遍历数组
			for (auto ItemPtr : JsonValueArrayPtr->value)
			{
				auto ServerItemPtr = CJsonValueObjPtr::dynamicCast(ItemPtr);

				//读取信息
				WORD wServerID;
				CWHJsonInput::readJson(wServerID, ServerItemPtr->value["ServerID"], true);

				//转换类型
				string sServerID = to_string(wServerID);

				//构造变量
				StringView sField(sServerID);
				StringView sKey(RKEY_PREFIX_SERVERLIST);

				//修改房间
				if (pUpdateServerList->cbUpdateKind == UPDATE_KIND_MODIFY)
				{
					//获取字符串
					string sServerItem;
					CWHJson::writeValue(ServerItemPtr, sServerItem);

					//设置key
					m_pRedisClient->hset(sKey, sField, sServerItem);
				}

				//删除房间
				if (pUpdateServerList->cbUpdateKind == UPDATE_KIND_DELETE)
				{
					m_pRedisClient->hdel(sKey, sField);
				}
			}
		}

		return true;
	}
	catch (exception& ex)
	{
		CTraceService::TraceString(TEXT("%hs"), TraceLevel_Exception, ex.what());
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
	}

	return false;
}

//更新榜单
bool CDataBaseEngineSink::OnRequestUpdateRankingList(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//数组定义
		TCHAR szRankingList[8000] = { 0 };

		//////////////////////////////////////////////////////////////////////////////////////////////
		//充值排行榜

		//执行查询
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@cbListType"), LIST_TYPE_PAY);

		//执行查询
		auto lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryPayRankingList"), true);

		//校验结果
		if (lResultCode == DB_SUCCESS)
		{
			//读取数据
			m_PlatformDBAide.GetValue_String(TEXT("RankingList"), szRankingList, CountArray(szRankingList));

			//转换字符
			string sValue(CT2CA(szRankingList).m_psz);		
			string sKey = RKEY_PREFIX_RANKINGLIST + to_string(LIST_TYPE_PAY);

			//设置key
			m_pRedisClient->set(sKey, sValue);
		}	

		//////////////////////////////////////////////////////////////////////////////////////////////
		//财富排行榜
		
		//执行查询
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@cbListType"), LIST_TYPE_WEALTH);

		//执行查询
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryWealthRankingList"), true);

		//校验结果
		if (lResultCode == DB_SUCCESS)
		{
			//读取数据
			m_PlatformDBAide.GetValue_String(TEXT("RankingList"), szRankingList, CountArray(szRankingList));

			//转换字符
			string sValue(CT2CA(szRankingList).m_psz);
			string sKey = RKEY_PREFIX_RANKINGLIST + to_string(LIST_TYPE_WEALTH);

			//设置key
			m_pRedisClient->set(sKey, sValue);
		}	

		//////////////////////////////////////////////////////////////////////////////////////////////
		//赢取排行榜

		//执行查询
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@cbListType"), LIST_TYPE_BIGWINNER);

		//执行查询
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryBigWinerRankingList"), true);

		//校验结果
		if (lResultCode == DB_SUCCESS)
		{
			//读取数据
			m_PlatformDBAide.GetValue_String(TEXT("RankingList"), szRankingList, CountArray(szRankingList));

			//转换字符
			string sValue(CT2CA(szRankingList).m_psz);
			string sKey = RKEY_PREFIX_RANKINGLIST + to_string(LIST_TYPE_BIGWINNER);

			//设置key
			m_pRedisClient->set(sKey, sValue);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		//推荐排行榜

		//执行查询
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@cbListType"), LIST_TYPE_SPREAD);

		//执行查询
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QuerySpreadRankingList"), true);

		//校验结果
		if (lResultCode == DB_SUCCESS)
		{
			//读取数据
			m_PlatformDBAide.GetValue_String(TEXT("RankingList"), szRankingList, CountArray(szRankingList));

			//转换字符
			string sValue(CT2CA(szRankingList).m_psz);
			string sKey = RKEY_PREFIX_RANKINGLIST + to_string(LIST_TYPE_SPREAD);

			//设置key
			m_pRedisClient->set(sKey, sValue);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		//佣金排行榜

		//执行查询
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@cbListType"), LIST_TYPE_COMMISSION);

		//执行查询
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryCommissionRankingList"), true);

		//校验结果
		if (lResultCode == DB_SUCCESS)
		{
			//读取数据
			m_PlatformDBAide.GetValue_String(TEXT("RankingList"), szRankingList, CountArray(szRankingList));

			//转换字符
			string sValue(CT2CA(szRankingList).m_psz);
			string sKey = RKEY_PREFIX_RANKINGLIST + to_string(LIST_TYPE_COMMISSION);

			//设置key
			m_pRedisClient->set(sKey, sValue);
		}

		//发送信息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_UPDATA_RANKING_LIST, dwContextID, 0, NULL, 0);

		return true;
	}
	catch (exception& ex)
	{
		CTraceService::TraceString(TEXT("%hs"), TraceLevel_Exception, ex.what());
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

