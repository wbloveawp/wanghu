#include "StdAfx.h"
#include "ServiceUnits.h"
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
	//��������
	if ((m_AccountsDBModule.GetInterface() == NULL) && (m_AccountsDBModule.CreateInstance() == false))
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if ((m_PlatformDBModule.GetInterface() == NULL) && (m_PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	try
	{
		//��������
		tagDataBaseParameter * pPlatformParameter=&m_pInitParameter->m_PlatformDBParameter;
		tagDataBaseParameter * pAccountsParameter=&m_pInitParameter->m_AccountsDBParameter;

		//��������
		m_PlatformDBModule->SetConnectionInfo(pPlatformParameter->dwDataBaseAddr, pPlatformParameter->wDataBasePort,
		CW2CT(pPlatformParameter->szDataBaseName),CW2CT(pPlatformParameter->szDataBaseUser),CW2CT(pPlatformParameter->szDataBasePass));

		//��������
		m_AccountsDBModule->SetConnectionInfo(pAccountsParameter->dwDataBaseAddr, pAccountsParameter->wDataBasePort,
			CW2CT(pAccountsParameter->szDataBaseName), CW2CT(pAccountsParameter->szDataBaseUser), CW2CT(pAccountsParameter->szDataBasePass));

		//��������
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

		//��������
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//����ѡ��
		auto pRedisConnOption = &m_pInitParameter->m_RedisConnOption;

		//��������
		ConnectionOptions ConnectionOpts;
		
		//�û�����
		ConnectionOpts.db = pRedisConnOption->wDB;
		ConnectionOpts.port = pRedisConnOption->wPort;
		ConnectionOpts.host = string(CT2CA(pRedisConnOption->szHost).m_psz);
		ConnectionOpts.user = string(CT2CA(pRedisConnOption->szUser).m_psz);
		ConnectionOpts.password = string(CT2CA(pRedisConnOption->szPassword).m_psz);

		//��ʱʱ��
		ConnectionOpts.socket_timeout = std::chrono::milliseconds(1000);
		ConnectionOpts.connect_timeout = std::chrono::milliseconds(1000);

		//��������
		m_pRedisClient = new Redis(ConnectionOpts);
		if (m_pRedisClient == NULL) throw TEXT("RedisClient����ʧ��");

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
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

//ֹͣ�¼�
bool CDataBaseEngineSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	//���ö���
	m_PlatformDBAide.SetDataBase(NULL);
	m_AccountsDBAide.SetDataBase(NULL);

	//ɾ������
	SafeDelete(m_pRedisClient);	

	//�ر�����
	if (m_PlatformDBModule.GetInterface()!=NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}

	//�ر�����
	if (m_AccountsDBModule.GetInterface() != NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
	}

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
	case DBR_GP_LOAD_CONFIG:		//��������
	{
		return OnRequestLoadConfig(dwContextID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_UNLOCK_PLAYER:		//�������
	{
		return OnRequestUnlockPlayer(dwContextID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_REQUEST_WEBPAGE:	//������ҳ
	{
		return OnRequestRequestWebPage(dwContextID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_ONLINE_COUNT_INFO: //ͳ������
	{
		return OnRequestOnLineCountInfo(dwContextID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_GP_UPDATE_GATE_LIST:	//��������
	{
		return OnRequestUpdateGateList(dwContextID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_UPDATE_SERVER_LIST:	//���·���
	{
		return OnRequestUpdateServerList(dwContextID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_UPDATE_RANKING_LIST://���°�
	{
		return OnRequestUpdateRankingList(dwContextID, pData, wDataSize, pdwControlCode);
	}
	}

	return false;
}

//���ؿ��
bool CDataBaseEngineSink::OnRequestLoadConfig(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CONFIG_FINISH, dwContextID, 0, NULL, 0);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}
}

//�������
bool CDataBaseEngineSink::OnRequestUnlockPlayer(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//У�����
	ASSERT(wDataSize == sizeof(DBR_GP_UnlockPlayer));
	if (wDataSize != sizeof(DBR_GP_UnlockPlayer)) return false;

	//��������
	auto pUnlcokPlayer = (DBR_GP_UnlockPlayer*)pData;

	//������
	DBO_GP_UnlockPlayer UnlockPlayer;
	ZeroMemory(&UnlockPlayer, sizeof(UnlockPlayer));

	//���ñ���
	UnlockPlayer.dwUserID = pUnlcokPlayer->dwUserID;	

	try
	{
		//ִ�в�ѯ
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUnlcokPlayer->dwUserID);
		
		//ִ�в�ѯ
		m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CleanOnlineInfo"), false);

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_UNLOCK_PLAYER, dwContextID, 0, &UnlockPlayer, sizeof(UnlockPlayer));

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

//������ҳ
bool CDataBaseEngineSink::OnRequestRequestWebPage(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//��ȡ����
	ASSERT(wDataSize <= sizeof(DBR_GP_RequestWebPage));
	if (wDataSize > sizeof(DBR_GP_RequestWebPage)) return false;

	//��ȡ����
	DBR_GP_RequestWebPage * pRequestWebPage = (DBR_GP_RequestWebPage *)pData;

	//������ҳ
	m_WininetHttp.RequestWebPage(pRequestWebPage->szWebUrl, Hr_Post, NULL, pRequestWebPage->szPostData);

	//�������
	if (m_WininetHttp.GetLastError() != Hi_Success)
	{
		//�������
		CTraceService::TraceString(TEXT("��ҳ�������[%d]��%hs"), TraceLevel_Exception, m_WininetHttp.GetLastError(), pRequestWebPage->szWebUrl);
	}

	return true;
}

//������Ϣ
bool CDataBaseEngineSink::OnRequestOnLineCountInfo(DWORD dwContextID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//�������
		m_PlatformDBAide.ResetParameter();
		
		//ִ������
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_OnLineCountCollect"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}
}

//��������
bool CDataBaseEngineSink::OnRequestUpdateGateList(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//��ȡ����
		DBR_UpdateGateList* pUpdateGateList = (DBR_UpdateGateList*)pData;

		//�������
		StringView sGateKey(RKEY_PREFIX_GATELIST);
		StringView sGateValue(pUpdateGateList->szGateList);

		//����key
		m_pRedisClient->set(sGateKey, sGateValue);

		return true;
	}
	catch (exception& ex)
	{
		TCHAR szDescribe[512] = { 0 };
		_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("Redis��������ʧ�ܣ�%hs"), ex.what());
		CTraceService::TraceString(szDescribe, TraceLevel_Exception);
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
	}

	return false;
}

//�ϱ�����
bool CDataBaseEngineSink::OnRequestUpdateServerList(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//��ȡ����
		auto pUpdateServerList = (DBR_GP_UpdateServerList*)pData;

		//����У��
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
			//����ת��
			string szServerList;
			CWHService::Unicode2Utf8(pUpdateServerList->szServerList, szServerList);

			//��������
			auto JsonValueObj = CWHJson::getValue(szServerList);
			if (JsonValueObj->getType() != eJsonTypeArray)
			{
				throw L"�����б��ʽ����";
			}

			//ת������
			auto JsonValueArrayPtr = CJsonValueArrayPtr::dynamicCast(JsonValueObj);

			//��������
			for (auto ItemPtr : JsonValueArrayPtr->value)
			{
				auto ServerItemPtr = CJsonValueObjPtr::dynamicCast(ItemPtr);

				//��ȡ��Ϣ
				WORD wServerID;
				CWHJsonInput::readJson(wServerID, ServerItemPtr->value["ServerID"], true);

				//ת������
				string sServerID = to_string(wServerID);

				//�������
				StringView sField(sServerID);
				StringView sKey(RKEY_PREFIX_SERVERLIST);

				//�޸ķ���
				if (pUpdateServerList->cbUpdateKind == UPDATE_KIND_MODIFY)
				{
					//��ȡ�ַ���
					string sServerItem;
					CWHJson::writeValue(ServerItemPtr, sServerItem);

					//����key
					m_pRedisClient->hset(sKey, sField, sServerItem);
				}

				//ɾ������
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
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
	}

	return false;
}

//���°�
bool CDataBaseEngineSink::OnRequestUpdateRankingList(DWORD dwContextID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//���鶨��
		TCHAR szRankingList[8000] = { 0 };

		//////////////////////////////////////////////////////////////////////////////////////////////
		//��ֵ���а�

		//ִ�в�ѯ
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@cbListType"), LIST_TYPE_PAY);

		//ִ�в�ѯ
		auto lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryPayRankingList"), true);

		//У����
		if (lResultCode == DB_SUCCESS)
		{
			//��ȡ����
			m_PlatformDBAide.GetValue_String(TEXT("RankingList"), szRankingList, CountArray(szRankingList));

			//ת���ַ�
			string sValue(CT2CA(szRankingList).m_psz);		
			string sKey = RKEY_PREFIX_RANKINGLIST + to_string(LIST_TYPE_PAY);

			//����key
			m_pRedisClient->set(sKey, sValue);
		}	

		//////////////////////////////////////////////////////////////////////////////////////////////
		//�Ƹ����а�
		
		//ִ�в�ѯ
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@cbListType"), LIST_TYPE_WEALTH);

		//ִ�в�ѯ
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryWealthRankingList"), true);

		//У����
		if (lResultCode == DB_SUCCESS)
		{
			//��ȡ����
			m_PlatformDBAide.GetValue_String(TEXT("RankingList"), szRankingList, CountArray(szRankingList));

			//ת���ַ�
			string sValue(CT2CA(szRankingList).m_psz);
			string sKey = RKEY_PREFIX_RANKINGLIST + to_string(LIST_TYPE_WEALTH);

			//����key
			m_pRedisClient->set(sKey, sValue);
		}	

		//////////////////////////////////////////////////////////////////////////////////////////////
		//Ӯȡ���а�

		//ִ�в�ѯ
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@cbListType"), LIST_TYPE_BIGWINNER);

		//ִ�в�ѯ
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryBigWinerRankingList"), true);

		//У����
		if (lResultCode == DB_SUCCESS)
		{
			//��ȡ����
			m_PlatformDBAide.GetValue_String(TEXT("RankingList"), szRankingList, CountArray(szRankingList));

			//ת���ַ�
			string sValue(CT2CA(szRankingList).m_psz);
			string sKey = RKEY_PREFIX_RANKINGLIST + to_string(LIST_TYPE_BIGWINNER);

			//����key
			m_pRedisClient->set(sKey, sValue);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		//�Ƽ����а�

		//ִ�в�ѯ
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@cbListType"), LIST_TYPE_SPREAD);

		//ִ�в�ѯ
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QuerySpreadRankingList"), true);

		//У����
		if (lResultCode == DB_SUCCESS)
		{
			//��ȡ����
			m_PlatformDBAide.GetValue_String(TEXT("RankingList"), szRankingList, CountArray(szRankingList));

			//ת���ַ�
			string sValue(CT2CA(szRankingList).m_psz);
			string sKey = RKEY_PREFIX_RANKINGLIST + to_string(LIST_TYPE_SPREAD);

			//����key
			m_pRedisClient->set(sKey, sValue);
		}

		//////////////////////////////////////////////////////////////////////////////////////////////
		//Ӷ�����а�

		//ִ�в�ѯ
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@cbListType"), LIST_TYPE_COMMISSION);

		//ִ�в�ѯ
		lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryCommissionRankingList"), true);

		//У����
		if (lResultCode == DB_SUCCESS)
		{
			//��ȡ����
			m_PlatformDBAide.GetValue_String(TEXT("RankingList"), szRankingList, CountArray(szRankingList));

			//ת���ַ�
			string sValue(CT2CA(szRankingList).m_psz);
			string sKey = RKEY_PREFIX_RANKINGLIST + to_string(LIST_TYPE_COMMISSION);

			//����key
			m_pRedisClient->set(sKey, sValue);
		}

		//������Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_UPDATA_RANKING_LIST, dwContextID, 0, NULL, 0);

		return true;
	}
	catch (exception& ex)
	{
		CTraceService::TraceString(TEXT("%hs"), TraceLevel_Exception, ex.what());
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////

