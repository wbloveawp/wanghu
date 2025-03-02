#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataCacheEngineSink.h"
#include "Agora/RtcTokenBuilder.h"
#include <afxinet.h>

///////////////////////////////////////////////////////////////////////////////////

//好友列表
const std::string script_friend_list =
"local result={};"
"local key=KEYS[1];"
"local size=tonumber(ARGV[1]);"
"local index=tonumber(ARGV[2]);"
"local count = redis.call('ZCOUNT', key, 0, -1);"
"result = redis.call('ZRANGE', key, index*size, (index+1)*size);"
"local left = ((index+1)*size>count) and 0 or (count-(index+1)*size);"
"table.insert(result, 1, tostring(left));"
"return result";

//////////////////////////////////////////////////////////////////////////

//构造函数
CDataCacheEngineSink::CDataCacheEngineSink()
{
	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//析构函数
CDataCacheEngineSink::~CDataCacheEngineSink()
{
}

//接口查询
VOID * CDataCacheEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CDataCacheEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//创建对象
	if ((m_AccountsDBModule.GetInterface() == NULL) && (m_AccountsDBModule.CreateInstance() == false))
	{
		ASSERT(FALSE);
		return false;
	}

	try
	{
		//变量定义
		tagDataBaseParameter* pAccountsDBParameter = &m_pInitParameter->m_AccountsDBParameter;

		//设置连接
		m_AccountsDBModule->SetConnectionInfo(pAccountsDBParameter->dwDataBaseAddr, pAccountsDBParameter->wDataBasePort,
			CW2CT(pAccountsDBParameter->szDataBaseName), CW2CT(pAccountsDBParameter->szDataBaseUser), CW2CT(pAccountsDBParameter->szDataBasePass));

		//发起连接
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//连接选项
		auto pUserRConnOption = &m_pInitParameter->m_UserRConnOption;
		auto pUserWConnOption = &m_pInitParameter->m_UserWConnOption;
		auto pFriendConnOption = &m_pInitParameter->m_FriendConnOption;

		//变量定义
		ConnectionOptions ConnectionOpts;
		ConnectionPoolOptions ConnectionPoolopts;

		//消息连接
		ConnectionOpts.db = pUserRConnOption->wDB;
		ConnectionOpts.port = pUserRConnOption->wPort;
		ConnectionOpts.host = string(CT2CA(pUserRConnOption->szHost).m_psz);
		ConnectionOpts.user = string(CT2CA(pUserRConnOption->szUser).m_psz);
		ConnectionOpts.password = string(CT2CA(pUserRConnOption->szPassword).m_psz);

		//初始化
		m_pUserRClient = new Redis(ConnectionOpts);
		if (m_pUserRClient == NULL) throw "UserRClient创建失败";

		//消息连接
		ConnectionOpts.db = pUserWConnOption->wDB;
		ConnectionOpts.port = pUserWConnOption->wPort;
		ConnectionOpts.host = string(CT2CA(pUserWConnOption->szHost).m_psz);
		ConnectionOpts.user = string(CT2CA(pUserWConnOption->szUser).m_psz);
		ConnectionOpts.password = string(CT2CA(pUserWConnOption->szPassword).m_psz);

		//初始化
		m_pUserWClient = new Redis(ConnectionOpts);
		if (m_pUserWClient == NULL) throw "UserWClient创建失败";

		//队列连接
		ConnectionOpts.db = pFriendConnOption->wDB;
		ConnectionOpts.port = pFriendConnOption->wPort;
		ConnectionOpts.host = string(CT2CA(pFriendConnOption->szHost).m_psz);
		ConnectionOpts.user = string(CT2CA(pFriendConnOption->szUser).m_psz);
		ConnectionOpts.password = string(CT2CA(pFriendConnOption->szPassword).m_psz);

		//创建对象		
		m_pFriendClient = new Redis(ConnectionOpts);
		if (m_pFriendClient == NULL) throw("FriendClient创建失败");

		//加载脚本
		m_ScriptFriendList = m_pFriendClient->script_load(script_friend_list);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

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
bool CDataCacheEngineSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	
	//关闭连接
	SafeDelete(m_pUserRClient);
	SafeDelete(m_pFriendClient);

	//设置对象
	m_AccountsDBAide.SetDataBase(NULL);

	//关闭连接
	if (m_AccountsDBModule.GetInterface() != NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
	}

	//组件变量
	m_pInitParameter = NULL;
	m_pIDataBaseEngineEvent = NULL;

	return true;
}

//时间事件
bool CDataCacheEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{	

	return false;
}

//控制事件
bool CDataCacheEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//请求事件
bool CDataCacheEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	switch (wRequestID)
	{
	case DBR_GR_SYNC_USERINFO:		//同步消息
	{
		return OnRequestSyncUserInfo(dwContextID,dwTokenID,pData,wDataSize,pdwControlCode);
	}
	case DBR_GR_QUERY_USERINFO:		//查询消息
	{
		return OnRequestQueryUserInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_QUERY_BLACKLIST:	//黑名单列表
	{
		return OnRequestQueryBlackList(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_QUERY_FRIENDLIST:	//好友列表
	{
		return OnRequestQueryFriendList(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_QUERY_FRIENDINFO:	//好友信息
	{
		return OnRequestQueryFriendInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_QUERY_FRIENDEXTRAS:	//附加信息
	{
		return OnRequestQueryFriendExtras(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_BUILD_ACCESSTOKEN:	//生成令牌
	{
		return OnRequestBuildAccessToken(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	}

	return true;
}

//同步消息
bool CDataCacheEngineSink::OnRequestSyncUserInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//提取数据
	auto pServiceRoute = (tagServiceHash*)pData;

	//用户标识
	DWORD dwUserID = *(DWORD*)pServiceRoute->cbExtendData;

	//解析数据
	SyncUserInfoRequest SyncRequest;
	SyncRequest.ParseFromArray(pServiceRoute+1, wDataSize-sizeof(tagServiceHash));

	//同步结果
	SyncUserInfoResult SyncResult;
	SyncResult.set_retcode(OperateResultCode::ORC_SUCCESS);

	try
	{
		auto TickCount1 = GetTickCount64();

		//构造key
		vector<string> vKeys;
		map<UINT32, InfoVersion> mapVersion;

		//校验数量
		/*if (SyncRequest.uid_size() != SyncRequest.infoversion_size())
		{
			throw exception("同步请求的标识数量和版本信息数量不一致！");
		}*/

		for (int i=0; i< SyncRequest.uid_size(); i++)
		{
			vKeys.push_back(RKEY_PREFIX_USER + to_string(SyncRequest.uid(i)));

			//反序列化
			InfoVersion infoVersion;
			infoVersion.ParseFromString(SyncRequest.infoversion(i));

			//加入字典    
			mapVersion.insert(make_pair(SyncRequest.uid(i), infoVersion));
		}

		//读取用户消息
		vector<OptionalString> vValues;
		m_pUserRClient->mget(vKeys.begin(), vKeys.end(), std::back_inserter(vValues));

		//构造结构
		UserInfoList UserList;

		for (int i = 0; i < vValues.size(); i++)
		{
			if (!vValues[i])
			{
				//变量定义
				list<string> lUserInfo;
				vector<UINT32> vUserID;
				list<tagIMUserPublicInfo> lIMUserInfo;

				//添加标识
				vUserID.push_back(SyncRequest.uid(i));

				//查询用户
				QueryUserInfo(vUserID, lIMUserInfo);

				//缓存用户
				CacheUserInfo(lIMUserInfo, lUserInfo);

				//添加用户
				if (lUserInfo.size() == 1)
				{
					vValues[i] = OptionalString(lUserInfo.front());
				}

				if (!vValues[i]) continue;
			}

			//反序列化
			UserPublicInfo userInfo;
			userInfo.ParseFromString(vValues[i].value());

			//反序列化
			InfoVersion infoVersion;
			infoVersion.ParseFromString(userInfo.infoversion());

			auto iter = mapVersion.find(userInfo.uid());
			if (iter != mapVersion.end())
			{				
				//更新版本               
				for (int j = 0; j < infoVersion.infoversion_size(); j++)
				{
					//校验大小
					if (j >= iter->second.infoversion_size())
					{
						continue;
					}

					//校验版本
					if (iter->second.infoversion(j) != infoVersion.infoversion(j))
					{
						continue;
					}

					//清除信息
					if (UII_GENDER == j) userInfo.clear_gender();
					if (UII_GAMEID == j) userInfo.clear_gameid();
					if (UII_NICKNAME == j) userInfo.clear_nickname();
					if (UII_RANKSCORE == j) userInfo.clear_rankscore();
					if (UII_UNDERWRITE == j) userInfo.clear_underwrite();
					if (UII_EXPERIENCE == j) userInfo.clear_experience();
					if (UII_MEMBERPOINT == j) userInfo.clear_memberpoint();										
					if (UII_MASTERORDER == j) userInfo.clear_masterorder();										
					if (UII_FACEID == j) { userInfo.clear_faceid(); userInfo.clear_customid(); }
				}

				//序列化
				string sUserInfo;
				userInfo.SerializeToString(&sUserInfo);

				//发送判断
				if (UserList.ByteSize() + sUserInfo.size() >= SOCKET_PACKET)
				{
					//序列化
					string sUserList;
					UserList.SerializeToString(&sUserList);

					//投递消息
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USERINFO_LIST, dwContextID, dwTokenID, (char*)sUserList.data(), sUserList.size());

					//清除信息
					UserList.Clear();
				}

				//添加信息
				UserList.add_userinfo(sUserInfo);
			}
		}

		//剩余发送
		if (UserList.ByteSize()>0)
		{
			//序列化
			string sUserList;
			UserList.SerializeToString(&sUserList);

			//投递消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USERINFO_LIST, dwContextID, dwTokenID, (char*)sUserList.data(), sUserList.size());
		}

		//序列化
		string sSyncResult;
		SyncResult.SerializeToString(&sSyncResult);

		//投递消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SYNC_USERINFO_RESULT, dwContextID, dwTokenID, (char*)sSyncResult.data(), sSyncResult.size());

		CString strMessage;
		strMessage.Format(TEXT("ThreadID=%d,TickCount3=%I64d,TickCount=%I64d"), GetCurrentThreadId(), GetTickCount64() - TickCount1, GetTickCount64());
		CTraceService::TraceString(strMessage, TraceLevel_Debug);

		return true;

	}
	catch (exception& ex)
	{
		CTraceService::TraceString(CA2CT(ex.what()).m_psz, TraceLevel_Exception);

		//
		SyncResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}
	catch (...)
	{
		CTraceService::TraceString(TEXT("未知异常"), TraceLevel_Exception);

		//
		SyncResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}

	//序列化
	string sSyncResult;
	SyncResult.SerializeToString(&sSyncResult);

	//投递消息
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SYNC_USERINFO_RESULT, dwContextID, dwTokenID, (char*)sSyncResult.data(), sSyncResult.size());

	return true;
}

//查询消息
bool CDataCacheEngineSink::OnRequestQueryUserInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//提取数据
	auto pServiceRoute = (tagServiceHash*)pData;

	//用户标识
	DWORD dwUserID = *(DWORD*)pServiceRoute->cbExtendData;

	//查询结果
	lhchat::QueryUserInfoRequest QueryRequest;
	QueryRequest.ParseFromArray(pServiceRoute+1, wDataSize-sizeof(tagServiceHash));

	//查询结果
	lhchat::QueryUserInfoResult QueryResult;
	QueryResult.set_retcode(OperateResultCode::ORC_SUCCESS);

	try
	{
		auto TickCount1 = GetTickCount64();

		//变量定义
		vector<UINT32> vGameID;
		vector<UINT32> vUserID;
		vector<std::string> vkeys;

		//按GID查找
		if (QueryRequest.type() == ST_BY_GID)
		{
			for (int i = 0; i < QueryRequest.objectid_size(); i++)
			{
				vGameID.push_back(QueryRequest.objectid(i));
			}

			//查找标识
			QueryUserID(vGameID, vUserID);

			//清理标识
			QueryRequest.clear_objectid();

			//构造key			
			for (int i = 0; i < vUserID.size(); i++)
			{
				QueryRequest.add_objectid(vUserID[i]);				
			}

			//清除标识
			vUserID.clear();
		}

		//构造key			
		for (int i = 0; i < QueryRequest.objectid_size(); i++)
		{
			vkeys.push_back(RKEY_PREFIX_USER + to_string(QueryRequest.objectid(i)));
		}		

		//读取消息内容
		vector<OptionalString> values;
		m_pUserRClient->mget(vkeys.begin(), vkeys.end(), std::back_inserter(values));		

		//变量定义
		lhchat:UserInfoList UserList;

		for (int i=0; i<values.size(); i++)
		{
			if (!values[i])
			{
				vUserID.push_back(QueryRequest.objectid(i));
				continue;
			}

			if (UserList.ByteSize() + values[i].value().size() >= SOCKET_PACKET)
			{
				//序列化
				string sUserList;
				UserList.SerializeToString(&sUserList);

				//投递消息
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USERINFO_LIST, dwContextID, dwTokenID, (char*)sUserList.data(), sUserList.size());

				//清除信息
				UserList.Clear();
			}

			//添加信息
			UserList.add_userinfo(values[i].value());
		}

		//加载用户
		if (vUserID.size() > 0)
		{
			//变量定义
			list<string> lUserInfo;
			list<tagIMUserPublicInfo> lIMUserInfo;

			//查询用户
			QueryUserInfo(vUserID, lIMUserInfo);

			//缓存用户
			CacheUserInfo(lIMUserInfo, lUserInfo);

			//添加用户
			for (auto item : lUserInfo)
			{
				if (UserList.ByteSize() + item.size() >= SOCKET_PACKET)
				{
					//序列化
					string sUserList;
					UserList.SerializeToString(&sUserList);

					//投递消息
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USERINFO_LIST, dwContextID, dwTokenID, (char*)sUserList.data(), sUserList.size());

					//清除信息
					UserList.Clear();
				}

				//添加信息
				UserList.add_userinfo(item);
			}
		}

		//剩余发送
		if (UserList.ByteSize()>0)
		{
			//序列化
			string sUserList;
			UserList.SerializeToString(&sUserList);

			//投递消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USERINFO_LIST, dwContextID, dwTokenID, (char*)sUserList.data(), sUserList.size());
		}

		//序列化
		string sQueryResult;
		QueryResult.SerializeToString(&sQueryResult);

		//投递消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_USERINFO_RESULT, dwContextID, dwTokenID, (char*)sQueryResult.data(), sQueryResult.size());

		CString strMessage;
		strMessage.Format(TEXT("ThreadID=%d,TickCount3=%I64d,TickCount=%I64d"), GetCurrentThreadId(), GetTickCount64() - TickCount1, GetTickCount64());
		CTraceService::TraceString(strMessage, TraceLevel_Debug);

		return true;

	}
	catch (exception& ex)
	{
		CTraceService::TraceString(CA2CT(ex.what()).m_psz, TraceLevel_Exception);

		//
		QueryResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}
	catch (...)
	{
		CTraceService::TraceString(TEXT("未知异常"), TraceLevel_Exception);

		//
		QueryResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}

	//序列化
	string sQueryResult;
	QueryResult.SerializeToString(&sQueryResult);

	//投递消息
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_USERINFO_RESULT, dwContextID, dwTokenID, (char*)sQueryResult.data(), sQueryResult.size());	

	return true;
}

//查询黑名单
bool CDataCacheEngineSink::OnRequestQueryBlackList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//提取数据
	auto pServiceRoute = (tagServiceHash*)pData;

	//用户标识
	DWORD dwUserID = *(DWORD*)pServiceRoute->cbExtendData;

	//查询结果
	lhchat::QueryBlackListRequest QueryRequest;
	QueryRequest.ParseFromArray(pServiceRoute + 1, wDataSize - sizeof(tagServiceHash));

	//查询结果
	lhchat::QueryBlackListResult QueryResult;
	QueryResult.set_retcode(OperateResultCode::ORC_SUCCESS);

	try
	{
		auto TickCount1 = GetTickCount64();

		//构造key    
		string sKey = RKEY_PREFIX_UBLACKLIST + to_string(dwUserID);

		//构造参数
		string sPageSize = to_string(QueryRequest.pagesize());
		string sPageIndex = to_string(QueryRequest.pageindex());

		//变量定义
		std::vector<std::string> vValues;
		m_pFriendClient->evalsha(m_ScriptFriendList, { sKey }, { sPageSize, sPageIndex }, std::back_inserter(vValues));

		//校验结果
		if (vValues.size() == 0)
		{
			throw exception("查询黑名单异常！");			
		}

		//剩余数量
		QueryResult.set_leftcount(atoi(vValues[0].data()));

		//添加用户标识
		for (size_t i = 1; i < vValues.size(); i++)
		{
			QueryResult.add_uid(atoi(vValues[i].data()));
		}

		//序列化
		string sQueryResult;
		QueryResult.SerializeToString(&sQueryResult);

		//投递消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_BLACKLIST_RESULT, dwContextID, dwTokenID, (char*)sQueryResult.data(), sQueryResult.size());

		CString strMessage;
		strMessage.Format(TEXT("ThreadID=%d,TickCount3=%I64d,TickCount=%I64d"), GetCurrentThreadId(), GetTickCount64() - TickCount1, GetTickCount64());
		CTraceService::TraceString(strMessage, TraceLevel_Debug);

		return true;

	}
	catch (exception& ex)
	{
		CTraceService::TraceString(CA2CT(ex.what()).m_psz, TraceLevel_Exception);

		//
		QueryResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}
	catch (...)
	{
		CTraceService::TraceString(TEXT("未知异常"), TraceLevel_Exception);

		//
		QueryResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}

	//序列化
	string sQueryResult;
	QueryResult.SerializeToString(&sQueryResult);

	//投递消息
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_BLACKLIST_RESULT, dwContextID, dwTokenID, (char*)sQueryResult.data(), sQueryResult.size());

	return true;
}

//查询好友列表
bool CDataCacheEngineSink::OnRequestQueryFriendList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//提取数据
	auto pServiceRoute = (tagServiceHash*)pData;

	//用户标识
	DWORD dwUserID = *(DWORD*)pServiceRoute->cbExtendData;

	//查询结果
	lhchat::QueryFriendListRequest QueryRequest;
	QueryRequest.ParseFromArray(pServiceRoute + 1, wDataSize - sizeof(tagServiceHash));

	//查询结果
	lhchat::QueryFriendListResult QueryResult;
	QueryResult.set_retcode(OperateResultCode::ORC_SUCCESS);

	try
	{
		auto TickCount1 = GetTickCount64();

		//构造key    
		string sKey = RKEY_PREFIX_UFRIEND + to_string(dwUserID);

		//构造参数
		string sPageSize = to_string(QueryRequest.pagesize());
		string sPageIndex = to_string(QueryRequest.pageindex());

		//变量定义
		std::vector<std::string> vValues;
		//m_pFriendClient->evalsha(m_ScriptFriendList, { sKey }, { sPageSize, sPageIndex }, std::back_inserter(vValues));

		m_pFriendClient->hkeys(sKey, back_inserter(vValues));

		//校验结果
		//if (vValues.size() == 0)
		//{
		//	throw exception("查询好友列表异常！");
		//}

		//剩余数量
		QueryResult.set_leftcount(0);

		//添加好友标识
		for (size_t i = 0; i < vValues.size(); i++)
		{
			QueryResult.add_friendid(atoi(vValues[i].data()));
		}

		//序列化
		string sQueryResult;
		QueryResult.SerializeToString(&sQueryResult);

		//投递消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_FRIENDLIST_RESULT, dwContextID, dwTokenID, (char*)sQueryResult.data(), sQueryResult.size());

		CString strMessage;
		strMessage.Format(TEXT("ThreadID=%d,TickCount3=%I64d,TickCount=%I64d"), GetCurrentThreadId(), GetTickCount64() - TickCount1, GetTickCount64());
		CTraceService::TraceString(strMessage, TraceLevel_Debug);

		return true;

	}
	catch (exception& ex)
	{
		CTraceService::TraceString(CA2CT(ex.what()).m_psz, TraceLevel_Exception);

		//
		QueryResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}
	catch (...)
	{
		CTraceService::TraceString(TEXT("未知异常"), TraceLevel_Exception);

		//
		QueryResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}

	//序列化
	string sQueryResult;
	QueryResult.SerializeToString(&sQueryResult);

	//投递消息
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_FRIENDLIST_RESULT, dwContextID, dwTokenID, (char*)sQueryResult.data(), sQueryResult.size());

	return true;
}

//查询好友信息
bool CDataCacheEngineSink::OnRequestQueryFriendInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//提取数据
	auto pServiceRoute = (tagServiceHash*)pData;

	//用户标识
	DWORD dwUserID = *(DWORD*)pServiceRoute->cbExtendData;

	//查询结果
	lhchat::QueryFriendInfoRequest QueryRequest;
	QueryRequest.ParseFromArray(pServiceRoute + 1, wDataSize - sizeof(tagServiceHash));

	//查询结果
	lhchat::QueryFriendInfoResult QueryResult;
	QueryResult.set_retcode(OperateResultCode::ORC_SUCCESS);

	try
	{
		auto TickCount1 = GetTickCount64();

		//构造key    
		string sKeyHash = RKEY_PREFIX_UFRIEND "hash_" + to_string(dwUserID);

		//构造key
		vector<string> vKeys;
		vector<StringView> vFields;
		for (int i = 0; i < QueryRequest.friendid_size(); i++)
		{
			vFields.push_back(to_string(QueryRequest.friendid(i)));
			vKeys.push_back(RKEY_PREFIX_USER + to_string(QueryRequest.friendid(i)));
		}

		//读取数据
		vector<OptionalString> vValuesInfo;
		vector<OptionalString> vValuesExtras;
		m_pUserRClient->mget(vKeys.begin(), vKeys.end(), back_inserter(vValuesInfo));
		m_pFriendClient->hmget(sKeyHash, vFields.begin(), vFields.end(), back_inserter(vValuesExtras));

		//校验大小
		if (vValuesInfo.size() != vValuesExtras.size())
		{
			throw exception("好友基础信息和附加信息数量不匹配！");
		}

		//构造结构
		FriendInfoList FriendList;

		for (int i=0; i<vValuesInfo.size(); i++)
		{
			if (bool(vValuesInfo[i]))
			{
				//发送校验
				if (FriendList.ByteSize() + vValuesInfo[i].value().size() + vValuesExtras[i].value().size() >= SOCKET_PACKET)
				{
					//序列化
					string sFriendList;
					FriendList.SerializeToString(&sFriendList);

					//投递消息
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FRIENDINFO_LIST, dwContextID, dwTokenID, (char*)sFriendList.data(), sFriendList.size());

					//清除数据
					FriendList.Clear();
				}

				FriendList.add_userinfo(vValuesInfo[i].value());
				FriendList.add_extrasinfo(vValuesExtras[i].value());
			}
		}	

		//剩余发送
		if (FriendList.userinfo_size() > 0)
		{
			//序列化
			string sFriendList;
			FriendList.SerializeToString(&sFriendList);

			//投递消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FRIENDINFO_LIST, dwContextID, dwTokenID, (char*)sFriendList.data(), sFriendList.size());
		}

		//序列化
		string sQueryResult;
		QueryResult.SerializeToString(&sQueryResult);

		//投递消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_FRIENDINFO_RESULT, dwContextID, dwTokenID, (char*)sQueryResult.data(), sQueryResult.size());

		CString strMessage;
		strMessage.Format(TEXT("ThreadID=%d,TickCount3=%I64d,TickCount=%I64d"), GetCurrentThreadId(), GetTickCount64() - TickCount1, GetTickCount64());
		CTraceService::TraceString(strMessage, TraceLevel_Debug);

		return true;

	}
	catch (exception& ex)
	{
		CTraceService::TraceString(CA2CT(ex.what()).m_psz, TraceLevel_Exception);

		//
		QueryResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}
	catch (...)
	{
		CTraceService::TraceString(TEXT("未知异常"), TraceLevel_Exception);

		//
		QueryResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}

	//序列化
	string sQueryResult;
	QueryResult.SerializeToString(&sQueryResult);

	//投递消息
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_FRIENDINFO_RESULT, dwContextID, dwTokenID, (char*)sQueryResult.data(), sQueryResult.size());

	return true;
}

//查询好友扩展
bool CDataCacheEngineSink::OnRequestQueryFriendExtras(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//提取数据
	auto pServiceRoute = (tagServiceHash*)pData;

	//用户标识
	DWORD dwUserID = *(DWORD*)pServiceRoute->cbExtendData;

	//查询结果
	lhchat::QueryFriendInfoRequest QueryRequest;
	QueryRequest.ParseFromArray(pServiceRoute + 1, wDataSize - sizeof(tagServiceHash));

	//查询结果
	lhchat::QueryFriendInfoResult QueryResult;
	QueryResult.set_retcode(OperateResultCode::ORC_SUCCESS);

	try
	{
		auto TickCount1 = GetTickCount64();

		//构造key    
		string sKey = RKEY_PREFIX_UFRIEND "hash_" + to_string(dwUserID);

		//构造key
		vector<StringView> vFields;
		for (int i = 0; i < QueryRequest.friendid_size(); i++)
		{
			vFields.push_back(to_string(QueryRequest.friendid(i)));			
		}

		//读取数据
		vector<OptionalString> vValues;
		m_pFriendClient->hmget(sKey, vFields.begin(), vFields.end(), back_inserter(vValues));

		//构造结构
		FriendExtrasList ExtrasList;

		for (auto val : vValues)
		{
			if (bool(val))
			{
				//发送校验
				if (ExtrasList.ByteSize() + val.value().size() >= SOCKET_PACKET)
				{
					//序列化
					string sExtrasList;
					ExtrasList.SerializeToString(&sExtrasList);

					//投递消息
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FRIENDEXTRAS_LIST, dwContextID, dwTokenID, (char*)sExtrasList.data(), sExtrasList.size());

					//清除数据
					ExtrasList.Clear();
				}

				ExtrasList.add_extrasinfo(val.value());
			}
		}

		//剩余发送
		if (ExtrasList.extrasinfo_size() > 0)
		{
			//序列化
			string sExtrasList;
			ExtrasList.SerializeToString(&sExtrasList);

			//投递消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FRIENDEXTRAS_LIST, dwContextID, dwTokenID, (char*)sExtrasList.data(), sExtrasList.size());
		}

		//序列化
		string sQueryResult;
		QueryResult.SerializeToString(&sQueryResult);

		//投递消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_FRIENDEXTRAS_RESULT, dwContextID, dwTokenID, (char*)sQueryResult.data(), sQueryResult.size());

		CString strMessage;
		strMessage.Format(TEXT("ThreadID=%d,TickCount3=%I64d,TickCount=%I64d"), GetCurrentThreadId(), GetTickCount64() - TickCount1, GetTickCount64());
		CTraceService::TraceString(strMessage, TraceLevel_Debug);

		return true;

	}
	catch (exception& ex)
	{
		CTraceService::TraceString(CA2CT(ex.what()).m_psz, TraceLevel_Exception);

		//
		QueryResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}
	catch (...)
	{
		CTraceService::TraceString(TEXT("未知异常"), TraceLevel_Exception);

		//
		QueryResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}

	//序列化
	string sQueryResult;
	QueryResult.SerializeToString(&sQueryResult);

	//投递消息
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_FRIENDEXTRAS_RESULT, dwContextID, dwTokenID, (char*)sQueryResult.data(), sQueryResult.size());

	return true;
}

//生成令牌
bool CDataCacheEngineSink::OnRequestBuildAccessToken(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//提取数据
	auto pServiceRoute = (tagServiceHash*)pData;

	//用户标识
	DWORD dwUserID = *(DWORD*)pServiceRoute->cbExtendData;

	//令牌请求
	lhchat::BuildAccessTokenRequest BuildTokenRequest;
	BuildTokenRequest.ParseFromArray(pServiceRoute + 1, wDataSize - sizeof(tagServiceHash));

	//查询结果
	lhchat::BuildAccessTokenResult BuildTokenResult;
	BuildTokenResult.set_channelkind(BuildTokenRequest.channelkind());
	BuildTokenResult.set_channelname(BuildTokenRequest.channelname());
	
	try
	{
		//常量定义
		const string appID = "dd996a48ea304766aa883db2bdb81467";
		const string appCert = "ff25a17ebc9e4b059e6953e7ace3089e";		
		
		//设置变量
		uint32_t expireTime = 3600*24;
		uint32_t currentTime = time(NULL);
		uint32_t privilegeExpiredTs = currentTime + expireTime;

		//设置权限
		agora::tools::UserRole userRole;
		if (BuildTokenRequest.channelkind() == ChannelKind::CK_SYSTEM)
		{
			if (BuildTokenRequest.masterorder() > 0 && QueryMasterOrder(dwUserID)>0)
			{
				userRole = agora::tools::UserRole::Role_Admin;
			}
			else
			{
				userRole = agora::tools::UserRole::Role_Subscriber;
			}
			
			BuildTokenResult.set_channelname("zhchat_system_channel");
		}
		else
		{
			userRole = agora::tools::UserRole::Role_Admin;
		}

		//生成令牌
		auto accessToken = agora::tools::RtcTokenBuilder::buildTokenWithUid(appID, appCert, BuildTokenRequest.channelname(), dwUserID, userRole, privilegeExpiredTs);

		//设置令牌
		BuildTokenResult.set_accesstoken(accessToken);

		//序列化
		string sBuildTokenResult;
		BuildTokenResult.SerializeToString(&sBuildTokenResult);

		//投递消息
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BUILD_ACCESSTOKEN_RESULT, dwContextID, dwTokenID, (char*)sBuildTokenResult.data(), sBuildTokenResult.size());

		return true;

	}
	catch (exception& ex)
	{
		CTraceService::TraceString(CA2CT(ex.what()).m_psz, TraceLevel_Exception);
	}
	catch (...)
	{
		CTraceService::TraceString(TEXT("未知异常"), TraceLevel_Exception);
	}

	//序列化
	string sBuildTokenResult;
	BuildTokenResult.SerializeToString(&sBuildTokenResult);

	//投递消息
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BUILD_ACCESSTOKEN_RESULT, dwContextID, dwTokenID, (char*)sBuildTokenResult.data(), sBuildTokenResult.size());

	return true;
}

//查询权限
WORD CDataCacheEngineSink::QueryMasterOrder(UINT32 iUserID)
{
	//变量定义
	WORD wMasterOrder = 0;

	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), iUserID);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryMasterOrder"), true);		

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//读取数据
			wMasterOrder = m_AccountsDBAide.GetValue_WORD(TEXT("MasterOrder"));				
		}

		return wMasterOrder;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return wMasterOrder;
}

//查询标识
bool CDataCacheEngineSink::QueryUserID(vector<UINT32>& vGameID, vector<UINT32>& vUserID)
{
	try
	{
		//变量定义
		CString strGameIDList;

		//构造列表
		for (auto GameID : vGameID)
		{
			//添加括号
			if (strGameIDList.IsEmpty())
			{
				strGameIDList.Append(TEXT("["));
			}
			else
			{
				strGameIDList.Append(TEXT(","));
			}

			//格式化
			strGameIDList.AppendFormat(TEXT("%d"), GameID);
		}

		//回括号
		strGameIDList.Append(TEXT("]"));

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strGameID"), strGameIDList.GetString());

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserID"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//读取信息
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				//读取数据
				auto dwUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));				

				//添加用户
				vUserID.push_back(dwUserID);

				//移动记录
				m_AccountsDBModule->MoveToNext();
			}
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//加载信息
bool CDataCacheEngineSink::QueryUserInfo(vector<UINT32>& vUserID, list<tagIMUserPublicInfo>& lUserPublicInfo)
{
	try
	{
		//变量定义
		CString strUserIDList;

		//构造列表
		for (auto userID : vUserID)
		{
			//添加括号
			if (strUserIDList.IsEmpty())
			{
				strUserIDList.Append(TEXT("["));
			}
			else
			{
				strUserIDList.Append(TEXT(","));
			}

			//格式化
			strUserIDList.AppendFormat(TEXT("%d"), userID);			
		}

		//回括号
		strUserIDList.Append(TEXT("]"));				

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strUserID"), strUserIDList.GetString());

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserInfo"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//读取信息
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				//变量定义
				tagIMUserPublicInfo UserPublicInfo;
				ZeroMemory(&UserPublicInfo, sizeof(UserPublicInfo));

				//读取数据
				UserPublicInfo.dwUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
				UserPublicInfo.dwGameID = m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
				UserPublicInfo.cbGender = m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
				UserPublicInfo.wMasterOrder = m_AccountsDBAide.GetValue_WORD(TEXT("MasterOrder"));
				UserPublicInfo.cbInfoPublic = m_AccountsDBAide.GetValue_BYTE(TEXT("InfoPublic"));
				UserPublicInfo.wFaceID = m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
				UserPublicInfo.dwCustomID = m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
				UserPublicInfo.lRankScore  = m_AccountsDBAide.GetValue_LONGLONG(TEXT("RankScore"));
				UserPublicInfo.dwExperience = m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));
				UserPublicInfo.dwMemberPoint = m_AccountsDBAide.GetValue_DWORD(TEXT("MemberPoint"));
				UserPublicInfo.dwLikedNumber = m_AccountsDBAide.GetValue_DWORD(TEXT("LikedNumber"));
				UserPublicInfo.dwLoveliness= m_AccountsDBAide.GetValue_DWORD(TEXT("Loveliness"));				
				m_AccountsDBAide.GetValue_String(TEXT("NickName"), UserPublicInfo.szNickName,CountArray(UserPublicInfo.szNickName));
				m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"), UserPublicInfo.szUnderWrite, CountArray(UserPublicInfo.szUnderWrite));
	
				//添加用户
				lUserPublicInfo.push_back(UserPublicInfo);

				//移动记录
				m_AccountsDBModule->MoveToNext();
			}
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//缓冲用户
bool CDataCacheEngineSink::CacheUserInfo(list<tagIMUserPublicInfo>& lIMUserPublicInfo, list<string>& lUserPublicInfo)
{
	try
	{
		//变量定义
		UserPublicInfo PublicInfo;
		map<string, string>	MapKeyValues;
		
		for (auto userInfo : lIMUserPublicInfo)
		{
			//变量定义
			int nBuffLen = 0;
			CHAR szBuffer[512];			

			//设置信息
			PublicInfo.set_uid(userInfo.dwUserID);
			PublicInfo.set_gameid(userInfo.dwGameID);
			PublicInfo.set_gender(userInfo.cbGender);
			PublicInfo.set_faceid(userInfo.wFaceID);
			PublicInfo.set_customid(userInfo.dwCustomID);
			PublicInfo.set_infopublic(userInfo.cbInfoPublic);
			PublicInfo.set_rankscore(userInfo.lRankScore);
			PublicInfo.set_experience(userInfo.dwExperience);
			PublicInfo.set_loveliness(userInfo.dwLoveliness);
			PublicInfo.set_memberpoint(userInfo.dwMemberPoint);
			PublicInfo.set_masterorder(userInfo.wMasterOrder);

			//设置昵称
			nBuffLen = CWHService::Unicode2Utf8(userInfo.szNickName, szBuffer, CountArray(szBuffer));
			PublicInfo.set_nickname(szBuffer, nBuffLen);

			//设置昵称
			nBuffLen = CWHService::Unicode2Utf8(userInfo.szUnderWrite, szBuffer, CountArray(szBuffer));
			PublicInfo.set_underwrite(szBuffer, nBuffLen);

			//版本信息
			lhchat::InfoVersion Version;

			//设置版本
			for (int i=0; i<UserInfoIndex_ARRAYSIZE; i++) Version.add_infoversion(1);

			//序列化
			string sVersion;
			Version.SerializeToString(&sVersion);

			//设置版本
			PublicInfo.set_infoversion(sVersion);

			//序列化
			string sPublicInfo;
			PublicInfo.SerializeToString(&sPublicInfo);

			//添加数据
			lUserPublicInfo.push_back(sPublicInfo);

			//插入数据
			string sUserKey = RKEY_PREFIX_USER + to_string(PublicInfo.uid());
			MapKeyValues.insert(make_pair(sUserKey, sPublicInfo));
		}			

		if (MapKeyValues.size() > 0)
		{
			m_pUserWClient->msetnx(MapKeyValues.begin(), MapKeyValues.end());
		}		

		return true;
	}
	catch (exception& ex)
	{
		TCHAR szDescribe[512] = { 0 };
		_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("%hs"), ex.what());
		CTraceService::TraceString(szDescribe, TraceLevel_Exception);
	}

	return false;
}

//输出信息
void CDataCacheEngineSink::TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException)
{
	CString strMessage;
	strMessage.Format(TEXT("%s(%d):%hs(%s)"), CA2CT(pFile).m_psz, nLine, pFunction, pIException ? pIException->GetExceptionDescribe() : TEXT(""));
	CTraceService::TraceString(strMessage, TraceLevel_Exception);

}
//////////////////////////////////////////////////////////////////////////

