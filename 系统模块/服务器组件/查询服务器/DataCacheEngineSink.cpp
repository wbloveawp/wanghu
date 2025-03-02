#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataCacheEngineSink.h"
#include "Agora/RtcTokenBuilder.h"
#include <afxinet.h>

///////////////////////////////////////////////////////////////////////////////////

//�����б�
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

//���캯��
CDataCacheEngineSink::CDataCacheEngineSink()
{
	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//��������
CDataCacheEngineSink::~CDataCacheEngineSink()
{
}

//�ӿڲ�ѯ
VOID * CDataCacheEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CDataCacheEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//��������
	if ((m_AccountsDBModule.GetInterface() == NULL) && (m_AccountsDBModule.CreateInstance() == false))
	{
		ASSERT(FALSE);
		return false;
	}

	try
	{
		//��������
		tagDataBaseParameter* pAccountsDBParameter = &m_pInitParameter->m_AccountsDBParameter;

		//��������
		m_AccountsDBModule->SetConnectionInfo(pAccountsDBParameter->dwDataBaseAddr, pAccountsDBParameter->wDataBasePort,
			CW2CT(pAccountsDBParameter->szDataBaseName), CW2CT(pAccountsDBParameter->szDataBaseUser), CW2CT(pAccountsDBParameter->szDataBasePass));

		//��������
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//����ѡ��
		auto pUserRConnOption = &m_pInitParameter->m_UserRConnOption;
		auto pUserWConnOption = &m_pInitParameter->m_UserWConnOption;
		auto pFriendConnOption = &m_pInitParameter->m_FriendConnOption;

		//��������
		ConnectionOptions ConnectionOpts;
		ConnectionPoolOptions ConnectionPoolopts;

		//��Ϣ����
		ConnectionOpts.db = pUserRConnOption->wDB;
		ConnectionOpts.port = pUserRConnOption->wPort;
		ConnectionOpts.host = string(CT2CA(pUserRConnOption->szHost).m_psz);
		ConnectionOpts.user = string(CT2CA(pUserRConnOption->szUser).m_psz);
		ConnectionOpts.password = string(CT2CA(pUserRConnOption->szPassword).m_psz);

		//��ʼ��
		m_pUserRClient = new Redis(ConnectionOpts);
		if (m_pUserRClient == NULL) throw "UserRClient����ʧ��";

		//��Ϣ����
		ConnectionOpts.db = pUserWConnOption->wDB;
		ConnectionOpts.port = pUserWConnOption->wPort;
		ConnectionOpts.host = string(CT2CA(pUserWConnOption->szHost).m_psz);
		ConnectionOpts.user = string(CT2CA(pUserWConnOption->szUser).m_psz);
		ConnectionOpts.password = string(CT2CA(pUserWConnOption->szPassword).m_psz);

		//��ʼ��
		m_pUserWClient = new Redis(ConnectionOpts);
		if (m_pUserWClient == NULL) throw "UserWClient����ʧ��";

		//��������
		ConnectionOpts.db = pFriendConnOption->wDB;
		ConnectionOpts.port = pFriendConnOption->wPort;
		ConnectionOpts.host = string(CT2CA(pFriendConnOption->szHost).m_psz);
		ConnectionOpts.user = string(CT2CA(pFriendConnOption->szUser).m_psz);
		ConnectionOpts.password = string(CT2CA(pFriendConnOption->szPassword).m_psz);

		//��������		
		m_pFriendClient = new Redis(ConnectionOpts);
		if (m_pFriendClient == NULL) throw("FriendClient����ʧ��");

		//���ؽű�
		m_ScriptFriendList = m_pFriendClient->script_load(script_friend_list);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
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

//ֹͣ�¼�
bool CDataCacheEngineSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	
	//�ر�����
	SafeDelete(m_pUserRClient);
	SafeDelete(m_pFriendClient);

	//���ö���
	m_AccountsDBAide.SetDataBase(NULL);

	//�ر�����
	if (m_AccountsDBModule.GetInterface() != NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
	}

	//�������
	m_pInitParameter = NULL;
	m_pIDataBaseEngineEvent = NULL;

	return true;
}

//ʱ���¼�
bool CDataCacheEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{	

	return false;
}

//�����¼�
bool CDataCacheEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�����¼�
bool CDataCacheEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	switch (wRequestID)
	{
	case DBR_GR_SYNC_USERINFO:		//ͬ����Ϣ
	{
		return OnRequestSyncUserInfo(dwContextID,dwTokenID,pData,wDataSize,pdwControlCode);
	}
	case DBR_GR_QUERY_USERINFO:		//��ѯ��Ϣ
	{
		return OnRequestQueryUserInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_QUERY_BLACKLIST:	//�������б�
	{
		return OnRequestQueryBlackList(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_QUERY_FRIENDLIST:	//�����б�
	{
		return OnRequestQueryFriendList(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_QUERY_FRIENDINFO:	//������Ϣ
	{
		return OnRequestQueryFriendInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_QUERY_FRIENDEXTRAS:	//������Ϣ
	{
		return OnRequestQueryFriendExtras(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_BUILD_ACCESSTOKEN:	//��������
	{
		return OnRequestBuildAccessToken(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	}

	return true;
}

//ͬ����Ϣ
bool CDataCacheEngineSink::OnRequestSyncUserInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//��ȡ����
	auto pServiceRoute = (tagServiceHash*)pData;

	//�û���ʶ
	DWORD dwUserID = *(DWORD*)pServiceRoute->cbExtendData;

	//��������
	SyncUserInfoRequest SyncRequest;
	SyncRequest.ParseFromArray(pServiceRoute+1, wDataSize-sizeof(tagServiceHash));

	//ͬ�����
	SyncUserInfoResult SyncResult;
	SyncResult.set_retcode(OperateResultCode::ORC_SUCCESS);

	try
	{
		auto TickCount1 = GetTickCount64();

		//����key
		vector<string> vKeys;
		map<UINT32, InfoVersion> mapVersion;

		//У������
		/*if (SyncRequest.uid_size() != SyncRequest.infoversion_size())
		{
			throw exception("ͬ������ı�ʶ�����Ͱ汾��Ϣ������һ�£�");
		}*/

		for (int i=0; i< SyncRequest.uid_size(); i++)
		{
			vKeys.push_back(RKEY_PREFIX_USER + to_string(SyncRequest.uid(i)));

			//�����л�
			InfoVersion infoVersion;
			infoVersion.ParseFromString(SyncRequest.infoversion(i));

			//�����ֵ�    
			mapVersion.insert(make_pair(SyncRequest.uid(i), infoVersion));
		}

		//��ȡ�û���Ϣ
		vector<OptionalString> vValues;
		m_pUserRClient->mget(vKeys.begin(), vKeys.end(), std::back_inserter(vValues));

		//����ṹ
		UserInfoList UserList;

		for (int i = 0; i < vValues.size(); i++)
		{
			if (!vValues[i])
			{
				//��������
				list<string> lUserInfo;
				vector<UINT32> vUserID;
				list<tagIMUserPublicInfo> lIMUserInfo;

				//��ӱ�ʶ
				vUserID.push_back(SyncRequest.uid(i));

				//��ѯ�û�
				QueryUserInfo(vUserID, lIMUserInfo);

				//�����û�
				CacheUserInfo(lIMUserInfo, lUserInfo);

				//����û�
				if (lUserInfo.size() == 1)
				{
					vValues[i] = OptionalString(lUserInfo.front());
				}

				if (!vValues[i]) continue;
			}

			//�����л�
			UserPublicInfo userInfo;
			userInfo.ParseFromString(vValues[i].value());

			//�����л�
			InfoVersion infoVersion;
			infoVersion.ParseFromString(userInfo.infoversion());

			auto iter = mapVersion.find(userInfo.uid());
			if (iter != mapVersion.end())
			{				
				//���°汾               
				for (int j = 0; j < infoVersion.infoversion_size(); j++)
				{
					//У���С
					if (j >= iter->second.infoversion_size())
					{
						continue;
					}

					//У��汾
					if (iter->second.infoversion(j) != infoVersion.infoversion(j))
					{
						continue;
					}

					//�����Ϣ
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

				//���л�
				string sUserInfo;
				userInfo.SerializeToString(&sUserInfo);

				//�����ж�
				if (UserList.ByteSize() + sUserInfo.size() >= SOCKET_PACKET)
				{
					//���л�
					string sUserList;
					UserList.SerializeToString(&sUserList);

					//Ͷ����Ϣ
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USERINFO_LIST, dwContextID, dwTokenID, (char*)sUserList.data(), sUserList.size());

					//�����Ϣ
					UserList.Clear();
				}

				//�����Ϣ
				UserList.add_userinfo(sUserInfo);
			}
		}

		//ʣ�෢��
		if (UserList.ByteSize()>0)
		{
			//���л�
			string sUserList;
			UserList.SerializeToString(&sUserList);

			//Ͷ����Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USERINFO_LIST, dwContextID, dwTokenID, (char*)sUserList.data(), sUserList.size());
		}

		//���л�
		string sSyncResult;
		SyncResult.SerializeToString(&sSyncResult);

		//Ͷ����Ϣ
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
		CTraceService::TraceString(TEXT("δ֪�쳣"), TraceLevel_Exception);

		//
		SyncResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}

	//���л�
	string sSyncResult;
	SyncResult.SerializeToString(&sSyncResult);

	//Ͷ����Ϣ
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SYNC_USERINFO_RESULT, dwContextID, dwTokenID, (char*)sSyncResult.data(), sSyncResult.size());

	return true;
}

//��ѯ��Ϣ
bool CDataCacheEngineSink::OnRequestQueryUserInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//��ȡ����
	auto pServiceRoute = (tagServiceHash*)pData;

	//�û���ʶ
	DWORD dwUserID = *(DWORD*)pServiceRoute->cbExtendData;

	//��ѯ���
	lhchat::QueryUserInfoRequest QueryRequest;
	QueryRequest.ParseFromArray(pServiceRoute+1, wDataSize-sizeof(tagServiceHash));

	//��ѯ���
	lhchat::QueryUserInfoResult QueryResult;
	QueryResult.set_retcode(OperateResultCode::ORC_SUCCESS);

	try
	{
		auto TickCount1 = GetTickCount64();

		//��������
		vector<UINT32> vGameID;
		vector<UINT32> vUserID;
		vector<std::string> vkeys;

		//��GID����
		if (QueryRequest.type() == ST_BY_GID)
		{
			for (int i = 0; i < QueryRequest.objectid_size(); i++)
			{
				vGameID.push_back(QueryRequest.objectid(i));
			}

			//���ұ�ʶ
			QueryUserID(vGameID, vUserID);

			//�����ʶ
			QueryRequest.clear_objectid();

			//����key			
			for (int i = 0; i < vUserID.size(); i++)
			{
				QueryRequest.add_objectid(vUserID[i]);				
			}

			//�����ʶ
			vUserID.clear();
		}

		//����key			
		for (int i = 0; i < QueryRequest.objectid_size(); i++)
		{
			vkeys.push_back(RKEY_PREFIX_USER + to_string(QueryRequest.objectid(i)));
		}		

		//��ȡ��Ϣ����
		vector<OptionalString> values;
		m_pUserRClient->mget(vkeys.begin(), vkeys.end(), std::back_inserter(values));		

		//��������
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
				//���л�
				string sUserList;
				UserList.SerializeToString(&sUserList);

				//Ͷ����Ϣ
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USERINFO_LIST, dwContextID, dwTokenID, (char*)sUserList.data(), sUserList.size());

				//�����Ϣ
				UserList.Clear();
			}

			//�����Ϣ
			UserList.add_userinfo(values[i].value());
		}

		//�����û�
		if (vUserID.size() > 0)
		{
			//��������
			list<string> lUserInfo;
			list<tagIMUserPublicInfo> lIMUserInfo;

			//��ѯ�û�
			QueryUserInfo(vUserID, lIMUserInfo);

			//�����û�
			CacheUserInfo(lIMUserInfo, lUserInfo);

			//����û�
			for (auto item : lUserInfo)
			{
				if (UserList.ByteSize() + item.size() >= SOCKET_PACKET)
				{
					//���л�
					string sUserList;
					UserList.SerializeToString(&sUserList);

					//Ͷ����Ϣ
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USERINFO_LIST, dwContextID, dwTokenID, (char*)sUserList.data(), sUserList.size());

					//�����Ϣ
					UserList.Clear();
				}

				//�����Ϣ
				UserList.add_userinfo(item);
			}
		}

		//ʣ�෢��
		if (UserList.ByteSize()>0)
		{
			//���л�
			string sUserList;
			UserList.SerializeToString(&sUserList);

			//Ͷ����Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USERINFO_LIST, dwContextID, dwTokenID, (char*)sUserList.data(), sUserList.size());
		}

		//���л�
		string sQueryResult;
		QueryResult.SerializeToString(&sQueryResult);

		//Ͷ����Ϣ
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
		CTraceService::TraceString(TEXT("δ֪�쳣"), TraceLevel_Exception);

		//
		QueryResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}

	//���л�
	string sQueryResult;
	QueryResult.SerializeToString(&sQueryResult);

	//Ͷ����Ϣ
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_USERINFO_RESULT, dwContextID, dwTokenID, (char*)sQueryResult.data(), sQueryResult.size());	

	return true;
}

//��ѯ������
bool CDataCacheEngineSink::OnRequestQueryBlackList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//��ȡ����
	auto pServiceRoute = (tagServiceHash*)pData;

	//�û���ʶ
	DWORD dwUserID = *(DWORD*)pServiceRoute->cbExtendData;

	//��ѯ���
	lhchat::QueryBlackListRequest QueryRequest;
	QueryRequest.ParseFromArray(pServiceRoute + 1, wDataSize - sizeof(tagServiceHash));

	//��ѯ���
	lhchat::QueryBlackListResult QueryResult;
	QueryResult.set_retcode(OperateResultCode::ORC_SUCCESS);

	try
	{
		auto TickCount1 = GetTickCount64();

		//����key    
		string sKey = RKEY_PREFIX_UBLACKLIST + to_string(dwUserID);

		//�������
		string sPageSize = to_string(QueryRequest.pagesize());
		string sPageIndex = to_string(QueryRequest.pageindex());

		//��������
		std::vector<std::string> vValues;
		m_pFriendClient->evalsha(m_ScriptFriendList, { sKey }, { sPageSize, sPageIndex }, std::back_inserter(vValues));

		//У����
		if (vValues.size() == 0)
		{
			throw exception("��ѯ�������쳣��");			
		}

		//ʣ������
		QueryResult.set_leftcount(atoi(vValues[0].data()));

		//����û���ʶ
		for (size_t i = 1; i < vValues.size(); i++)
		{
			QueryResult.add_uid(atoi(vValues[i].data()));
		}

		//���л�
		string sQueryResult;
		QueryResult.SerializeToString(&sQueryResult);

		//Ͷ����Ϣ
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
		CTraceService::TraceString(TEXT("δ֪�쳣"), TraceLevel_Exception);

		//
		QueryResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}

	//���л�
	string sQueryResult;
	QueryResult.SerializeToString(&sQueryResult);

	//Ͷ����Ϣ
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_BLACKLIST_RESULT, dwContextID, dwTokenID, (char*)sQueryResult.data(), sQueryResult.size());

	return true;
}

//��ѯ�����б�
bool CDataCacheEngineSink::OnRequestQueryFriendList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//��ȡ����
	auto pServiceRoute = (tagServiceHash*)pData;

	//�û���ʶ
	DWORD dwUserID = *(DWORD*)pServiceRoute->cbExtendData;

	//��ѯ���
	lhchat::QueryFriendListRequest QueryRequest;
	QueryRequest.ParseFromArray(pServiceRoute + 1, wDataSize - sizeof(tagServiceHash));

	//��ѯ���
	lhchat::QueryFriendListResult QueryResult;
	QueryResult.set_retcode(OperateResultCode::ORC_SUCCESS);

	try
	{
		auto TickCount1 = GetTickCount64();

		//����key    
		string sKey = RKEY_PREFIX_UFRIEND + to_string(dwUserID);

		//�������
		string sPageSize = to_string(QueryRequest.pagesize());
		string sPageIndex = to_string(QueryRequest.pageindex());

		//��������
		std::vector<std::string> vValues;
		//m_pFriendClient->evalsha(m_ScriptFriendList, { sKey }, { sPageSize, sPageIndex }, std::back_inserter(vValues));

		m_pFriendClient->hkeys(sKey, back_inserter(vValues));

		//У����
		//if (vValues.size() == 0)
		//{
		//	throw exception("��ѯ�����б��쳣��");
		//}

		//ʣ������
		QueryResult.set_leftcount(0);

		//��Ӻ��ѱ�ʶ
		for (size_t i = 0; i < vValues.size(); i++)
		{
			QueryResult.add_friendid(atoi(vValues[i].data()));
		}

		//���л�
		string sQueryResult;
		QueryResult.SerializeToString(&sQueryResult);

		//Ͷ����Ϣ
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
		CTraceService::TraceString(TEXT("δ֪�쳣"), TraceLevel_Exception);

		//
		QueryResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}

	//���л�
	string sQueryResult;
	QueryResult.SerializeToString(&sQueryResult);

	//Ͷ����Ϣ
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_FRIENDLIST_RESULT, dwContextID, dwTokenID, (char*)sQueryResult.data(), sQueryResult.size());

	return true;
}

//��ѯ������Ϣ
bool CDataCacheEngineSink::OnRequestQueryFriendInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//��ȡ����
	auto pServiceRoute = (tagServiceHash*)pData;

	//�û���ʶ
	DWORD dwUserID = *(DWORD*)pServiceRoute->cbExtendData;

	//��ѯ���
	lhchat::QueryFriendInfoRequest QueryRequest;
	QueryRequest.ParseFromArray(pServiceRoute + 1, wDataSize - sizeof(tagServiceHash));

	//��ѯ���
	lhchat::QueryFriendInfoResult QueryResult;
	QueryResult.set_retcode(OperateResultCode::ORC_SUCCESS);

	try
	{
		auto TickCount1 = GetTickCount64();

		//����key    
		string sKeyHash = RKEY_PREFIX_UFRIEND "hash_" + to_string(dwUserID);

		//����key
		vector<string> vKeys;
		vector<StringView> vFields;
		for (int i = 0; i < QueryRequest.friendid_size(); i++)
		{
			vFields.push_back(to_string(QueryRequest.friendid(i)));
			vKeys.push_back(RKEY_PREFIX_USER + to_string(QueryRequest.friendid(i)));
		}

		//��ȡ����
		vector<OptionalString> vValuesInfo;
		vector<OptionalString> vValuesExtras;
		m_pUserRClient->mget(vKeys.begin(), vKeys.end(), back_inserter(vValuesInfo));
		m_pFriendClient->hmget(sKeyHash, vFields.begin(), vFields.end(), back_inserter(vValuesExtras));

		//У���С
		if (vValuesInfo.size() != vValuesExtras.size())
		{
			throw exception("���ѻ�����Ϣ�͸�����Ϣ������ƥ�䣡");
		}

		//����ṹ
		FriendInfoList FriendList;

		for (int i=0; i<vValuesInfo.size(); i++)
		{
			if (bool(vValuesInfo[i]))
			{
				//����У��
				if (FriendList.ByteSize() + vValuesInfo[i].value().size() + vValuesExtras[i].value().size() >= SOCKET_PACKET)
				{
					//���л�
					string sFriendList;
					FriendList.SerializeToString(&sFriendList);

					//Ͷ����Ϣ
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FRIENDINFO_LIST, dwContextID, dwTokenID, (char*)sFriendList.data(), sFriendList.size());

					//�������
					FriendList.Clear();
				}

				FriendList.add_userinfo(vValuesInfo[i].value());
				FriendList.add_extrasinfo(vValuesExtras[i].value());
			}
		}	

		//ʣ�෢��
		if (FriendList.userinfo_size() > 0)
		{
			//���л�
			string sFriendList;
			FriendList.SerializeToString(&sFriendList);

			//Ͷ����Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FRIENDINFO_LIST, dwContextID, dwTokenID, (char*)sFriendList.data(), sFriendList.size());
		}

		//���л�
		string sQueryResult;
		QueryResult.SerializeToString(&sQueryResult);

		//Ͷ����Ϣ
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
		CTraceService::TraceString(TEXT("δ֪�쳣"), TraceLevel_Exception);

		//
		QueryResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}

	//���л�
	string sQueryResult;
	QueryResult.SerializeToString(&sQueryResult);

	//Ͷ����Ϣ
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_FRIENDINFO_RESULT, dwContextID, dwTokenID, (char*)sQueryResult.data(), sQueryResult.size());

	return true;
}

//��ѯ������չ
bool CDataCacheEngineSink::OnRequestQueryFriendExtras(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//��ȡ����
	auto pServiceRoute = (tagServiceHash*)pData;

	//�û���ʶ
	DWORD dwUserID = *(DWORD*)pServiceRoute->cbExtendData;

	//��ѯ���
	lhchat::QueryFriendInfoRequest QueryRequest;
	QueryRequest.ParseFromArray(pServiceRoute + 1, wDataSize - sizeof(tagServiceHash));

	//��ѯ���
	lhchat::QueryFriendInfoResult QueryResult;
	QueryResult.set_retcode(OperateResultCode::ORC_SUCCESS);

	try
	{
		auto TickCount1 = GetTickCount64();

		//����key    
		string sKey = RKEY_PREFIX_UFRIEND "hash_" + to_string(dwUserID);

		//����key
		vector<StringView> vFields;
		for (int i = 0; i < QueryRequest.friendid_size(); i++)
		{
			vFields.push_back(to_string(QueryRequest.friendid(i)));			
		}

		//��ȡ����
		vector<OptionalString> vValues;
		m_pFriendClient->hmget(sKey, vFields.begin(), vFields.end(), back_inserter(vValues));

		//����ṹ
		FriendExtrasList ExtrasList;

		for (auto val : vValues)
		{
			if (bool(val))
			{
				//����У��
				if (ExtrasList.ByteSize() + val.value().size() >= SOCKET_PACKET)
				{
					//���л�
					string sExtrasList;
					ExtrasList.SerializeToString(&sExtrasList);

					//Ͷ����Ϣ
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FRIENDEXTRAS_LIST, dwContextID, dwTokenID, (char*)sExtrasList.data(), sExtrasList.size());

					//�������
					ExtrasList.Clear();
				}

				ExtrasList.add_extrasinfo(val.value());
			}
		}

		//ʣ�෢��
		if (ExtrasList.extrasinfo_size() > 0)
		{
			//���л�
			string sExtrasList;
			ExtrasList.SerializeToString(&sExtrasList);

			//Ͷ����Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FRIENDEXTRAS_LIST, dwContextID, dwTokenID, (char*)sExtrasList.data(), sExtrasList.size());
		}

		//���л�
		string sQueryResult;
		QueryResult.SerializeToString(&sQueryResult);

		//Ͷ����Ϣ
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
		CTraceService::TraceString(TEXT("δ֪�쳣"), TraceLevel_Exception);

		//
		QueryResult.set_retcode(OperateResultCode::ORC_FAILURE);
	}

	//���л�
	string sQueryResult;
	QueryResult.SerializeToString(&sQueryResult);

	//Ͷ����Ϣ
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_QUERY_FRIENDEXTRAS_RESULT, dwContextID, dwTokenID, (char*)sQueryResult.data(), sQueryResult.size());

	return true;
}

//��������
bool CDataCacheEngineSink::OnRequestBuildAccessToken(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//��ȡ����
	auto pServiceRoute = (tagServiceHash*)pData;

	//�û���ʶ
	DWORD dwUserID = *(DWORD*)pServiceRoute->cbExtendData;

	//��������
	lhchat::BuildAccessTokenRequest BuildTokenRequest;
	BuildTokenRequest.ParseFromArray(pServiceRoute + 1, wDataSize - sizeof(tagServiceHash));

	//��ѯ���
	lhchat::BuildAccessTokenResult BuildTokenResult;
	BuildTokenResult.set_channelkind(BuildTokenRequest.channelkind());
	BuildTokenResult.set_channelname(BuildTokenRequest.channelname());
	
	try
	{
		//��������
		const string appID = "dd996a48ea304766aa883db2bdb81467";
		const string appCert = "ff25a17ebc9e4b059e6953e7ace3089e";		
		
		//���ñ���
		uint32_t expireTime = 3600*24;
		uint32_t currentTime = time(NULL);
		uint32_t privilegeExpiredTs = currentTime + expireTime;

		//����Ȩ��
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

		//��������
		auto accessToken = agora::tools::RtcTokenBuilder::buildTokenWithUid(appID, appCert, BuildTokenRequest.channelname(), dwUserID, userRole, privilegeExpiredTs);

		//��������
		BuildTokenResult.set_accesstoken(accessToken);

		//���л�
		string sBuildTokenResult;
		BuildTokenResult.SerializeToString(&sBuildTokenResult);

		//Ͷ����Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BUILD_ACCESSTOKEN_RESULT, dwContextID, dwTokenID, (char*)sBuildTokenResult.data(), sBuildTokenResult.size());

		return true;

	}
	catch (exception& ex)
	{
		CTraceService::TraceString(CA2CT(ex.what()).m_psz, TraceLevel_Exception);
	}
	catch (...)
	{
		CTraceService::TraceString(TEXT("δ֪�쳣"), TraceLevel_Exception);
	}

	//���л�
	string sBuildTokenResult;
	BuildTokenResult.SerializeToString(&sBuildTokenResult);

	//Ͷ����Ϣ
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BUILD_ACCESSTOKEN_RESULT, dwContextID, dwTokenID, (char*)sBuildTokenResult.data(), sBuildTokenResult.size());

	return true;
}

//��ѯȨ��
WORD CDataCacheEngineSink::QueryMasterOrder(UINT32 iUserID)
{
	//��������
	WORD wMasterOrder = 0;

	try
	{
		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), iUserID);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryMasterOrder"), true);		

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��ȡ����
			wMasterOrder = m_AccountsDBAide.GetValue_WORD(TEXT("MasterOrder"));				
		}

		return wMasterOrder;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return wMasterOrder;
}

//��ѯ��ʶ
bool CDataCacheEngineSink::QueryUserID(vector<UINT32>& vGameID, vector<UINT32>& vUserID)
{
	try
	{
		//��������
		CString strGameIDList;

		//�����б�
		for (auto GameID : vGameID)
		{
			//�������
			if (strGameIDList.IsEmpty())
			{
				strGameIDList.Append(TEXT("["));
			}
			else
			{
				strGameIDList.Append(TEXT(","));
			}

			//��ʽ��
			strGameIDList.AppendFormat(TEXT("%d"), GameID);
		}

		//������
		strGameIDList.Append(TEXT("]"));

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strGameID"), strGameIDList.GetString());

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserID"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��ȡ��Ϣ
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				//��ȡ����
				auto dwUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));				

				//����û�
				vUserID.push_back(dwUserID);

				//�ƶ���¼
				m_AccountsDBModule->MoveToNext();
			}
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//������Ϣ
bool CDataCacheEngineSink::QueryUserInfo(vector<UINT32>& vUserID, list<tagIMUserPublicInfo>& lUserPublicInfo)
{
	try
	{
		//��������
		CString strUserIDList;

		//�����б�
		for (auto userID : vUserID)
		{
			//�������
			if (strUserIDList.IsEmpty())
			{
				strUserIDList.Append(TEXT("["));
			}
			else
			{
				strUserIDList.Append(TEXT(","));
			}

			//��ʽ��
			strUserIDList.AppendFormat(TEXT("%d"), userID);			
		}

		//������
		strUserIDList.Append(TEXT("]"));				

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strUserID"), strUserIDList.GetString());

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserInfo"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��ȡ��Ϣ
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				//��������
				tagIMUserPublicInfo UserPublicInfo;
				ZeroMemory(&UserPublicInfo, sizeof(UserPublicInfo));

				//��ȡ����
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
	
				//����û�
				lUserPublicInfo.push_back(UserPublicInfo);

				//�ƶ���¼
				m_AccountsDBModule->MoveToNext();
			}
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//�����û�
bool CDataCacheEngineSink::CacheUserInfo(list<tagIMUserPublicInfo>& lIMUserPublicInfo, list<string>& lUserPublicInfo)
{
	try
	{
		//��������
		UserPublicInfo PublicInfo;
		map<string, string>	MapKeyValues;
		
		for (auto userInfo : lIMUserPublicInfo)
		{
			//��������
			int nBuffLen = 0;
			CHAR szBuffer[512];			

			//������Ϣ
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

			//�����ǳ�
			nBuffLen = CWHService::Unicode2Utf8(userInfo.szNickName, szBuffer, CountArray(szBuffer));
			PublicInfo.set_nickname(szBuffer, nBuffLen);

			//�����ǳ�
			nBuffLen = CWHService::Unicode2Utf8(userInfo.szUnderWrite, szBuffer, CountArray(szBuffer));
			PublicInfo.set_underwrite(szBuffer, nBuffLen);

			//�汾��Ϣ
			lhchat::InfoVersion Version;

			//���ð汾
			for (int i=0; i<UserInfoIndex_ARRAYSIZE; i++) Version.add_infoversion(1);

			//���л�
			string sVersion;
			Version.SerializeToString(&sVersion);

			//���ð汾
			PublicInfo.set_infoversion(sVersion);

			//���л�
			string sPublicInfo;
			PublicInfo.SerializeToString(&sPublicInfo);

			//�������
			lUserPublicInfo.push_back(sPublicInfo);

			//��������
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

//�����Ϣ
void CDataCacheEngineSink::TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException)
{
	CString strMessage;
	strMessage.Format(TEXT("%s(%d):%hs(%s)"), CA2CT(pFile).m_psz, nLine, pFunction, pIException ? pIException->GetExceptionDescribe() : TEXT(""));
	CTraceService::TraceString(strMessage, TraceLevel_Exception);

}
//////////////////////////////////////////////////////////////////////////

