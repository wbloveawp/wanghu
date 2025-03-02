#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBaseEngineSink.h"
#include <afxinet.h>

//////////////////////////////////////////////////////////////////////////
//��Ӻ���
const string script_append_friend =
"local key1=KEYS[1];"
"local key2=KEYS[2];"
"local field1=ARGV[1];"
"local field2=ARGV[2];"
"local extras1=ARGV[3];"
"local extras2=ARGV[4];"
"redis.call('HSET',key1, field1, extras1);"
"redis.call('HSET',key2, field2, extras2);"
"return 1";

//ɾ������
const string script_delete_friend =
"local key1=KEYS[1];"
"local key2=KEYS[2];"
"local field1=ARGV[1];"
"local field2=ARGV[2];"
"redis.call('HDEL',key1, field1);"
"redis.call('HDEL',key2, field2);"
"return 1";

//////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//�������
	m_pServiceOption=NULL;
	m_pInitParameter=NULL;	
	m_pIDataBaseEngineEvent=NULL;

	//�������
	m_pUserClient = NULL;
	m_pTokenClient = NULL;
	m_pFriendClient = NULL;
	m_pMasterClient = NULL;

	//��������
	ZeroMemory(&m_GPLogonFailure,sizeof(m_GPLogonFailure));
	ZeroMemory(&m_GPLogonSuccess,sizeof(m_GPLogonSuccess));
	ZeroMemory(&m_UserIndividual,sizeof(m_UserIndividual));

	return;
}

//��������
CDataBaseEngineSink::~CDataBaseEngineSink()
{
	//�ͷŶ���
	if (m_pUserClient) SafeDelete(m_pUserClient);
	if (m_pTokenClient) SafeDelete(m_pTokenClient);
	if (m_pFriendClient) SafeDelete(m_pFriendClient);
	if (m_pMasterClient) SafeDelete(m_pMasterClient);
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
	if ((m_AccountsDBModule.GetInterface()==NULL)&&(m_AccountsDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if ((m_PlatformDBModule.GetInterface()==NULL)&&(m_PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}	

	try
	{
		//��������
		tagDataBaseParameter * pAccountsDBParameter=&m_pInitParameter->m_AccountsDBParameter;
		tagDataBaseParameter * pPlatformDBParameter=&m_pInitParameter->m_PlatformDBParameter;	 

		//��������
		m_AccountsDBModule->SetConnectionInfo(pAccountsDBParameter->dwDataBaseAddr,pAccountsDBParameter->wDataBasePort,
		pAccountsDBParameter->szDataBaseName,pAccountsDBParameter->szDataBaseUser,pAccountsDBParameter->szDataBasePass);

		//��������
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->dwDataBaseAddr, pPlatformDBParameter->wDataBasePort,
		pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);
		
		//��������
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//��������
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());	

		//����ѡ��
		auto pUserConnOption = &m_pInitParameter->m_UserConnOption;
		auto pTokenConnOption = &m_pInitParameter->m_TokenConnOption;
		auto pFriendConnOption = &m_pInitParameter->m_FriendConnOption;
		auto pMasterConnOption = &m_pInitParameter->m_MasterConnOption;

		//��������
		ConnectionOptions ConnectionOpts;
		ConnectionPoolOptions ConnectionPoolopts;

		//��ʱ����
		ConnectionOpts.socket_timeout = std::chrono::milliseconds(1000);
		ConnectionOpts.connect_timeout = std::chrono::milliseconds(1000);

		//�û�����
		ConnectionOpts.db = pUserConnOption->wDB;
		ConnectionOpts.port = pUserConnOption->wPort;
		ConnectionOpts.host = string(CT2CA(pUserConnOption->szHost).m_psz);
		ConnectionOpts.user = string(CT2CA(pUserConnOption->szUser).m_psz);
		ConnectionOpts.password = string(CT2CA(pUserConnOption->szPassword).m_psz);		

		//��������
		m_pUserClient = new Redis(ConnectionOpts);
		if (m_pUserClient == NULL) throw TEXT("UserClient����ʧ��");

		//��������
		ConnectionOpts.db = pTokenConnOption->wDB;
		ConnectionOpts.port = pTokenConnOption->wPort;
		ConnectionOpts.host = string(CT2CA(pTokenConnOption->szHost).m_psz);
		ConnectionOpts.user = string(CT2CA(pTokenConnOption->szUser).m_psz);
		ConnectionOpts.password = string(CT2CA(pTokenConnOption->szPassword).m_psz);

		//��������		
		m_pTokenClient = new Redis(ConnectionOpts);
		if (m_pTokenClient == NULL) throw TEXT("TokenClient����ʧ��");

		////��������
		//ConnectionOpts.db = pFriendConnOption->wDB;
		//ConnectionOpts.port = pFriendConnOption->wPort;
		//ConnectionOpts.host = string(CT2CA(pFriendConnOption->szHost).m_psz);
		//ConnectionOpts.user = string(CT2CA(pFriendConnOption->szUser).m_psz);
		//ConnectionOpts.password = string(CT2CA(pFriendConnOption->szPassword).m_psz);

		////��������		
		//m_pFriendClient = new Redis(ConnectionOpts);
		//if (m_pFriendClient == NULL) throw TEXT("FriendClient����ʧ��");	

		//��������
		ConnectionOpts.db = pMasterConnOption->wDB;
		ConnectionOpts.port = pMasterConnOption->wPort;
		ConnectionOpts.host = string(CT2CA(pMasterConnOption->szHost).m_psz);
		ConnectionOpts.user = string(CT2CA(pMasterConnOption->szUser).m_psz);
		ConnectionOpts.password = string(CT2CA(pMasterConnOption->szPassword).m_psz);

		//��������		
		m_pMasterClient = new Redis(ConnectionOpts);
		if (m_pMasterClient == NULL) throw TEXT("MasterClient����ʧ��");

		//���ؽű�
		if (m_pFriendClient != NULL)
		{
			m_sScriptDeleteFriend = m_pFriendClient->script_load(script_delete_friend);
			m_sScriptAppendFriend = m_pFriendClient->script_load(script_append_friend);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

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
	//����Ԫ
	ASSERT (CServiceUnits::g_pServiceUnits!=NULL);
	auto pServiceUnits = CServiceUnits::g_pServiceUnits;

	//����������Ϣ
	if (&pServiceUnits->m_DataBaseEngineSink[0] == this)
	{
		CleanOnlineInfo(pServiceUnits->m_ServiceOption.wServiceID);
	}

	//���ö���
	m_AccountsDBAide.SetDataBase(NULL);
	m_PlatformDBAide.SetDataBase(NULL);	

	//�ر�����
	if (m_AccountsDBModule.GetInterface()!=NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
	}

	//�ر�����
	if (m_PlatformDBModule.GetInterface()!=NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}	

	//ɾ������
	SafeDelete(m_pUserClient);
	SafeDelete(m_pTokenClient);
	SafeDelete(m_pFriendClient);
	SafeDelete(m_pMasterClient);

	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	//��������
	ZeroMemory(&m_GPLogonFailure,sizeof(m_GPLogonFailure));
	ZeroMemory(&m_GPLogonSuccess,sizeof(m_GPLogonSuccess));
	ZeroMemory(&m_UserIndividual,sizeof(m_UserIndividual));

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
	CString strMessage;
	strMessage.Format(TEXT("ThreadID=%d,RequestID=%d"), ::GetCurrentThreadId(),wRequestID);
	CTraceService::TraceString(strMessage, TraceLevel_Debug);

	switch (wRequestID)
	{
	case DBR_GP_LOAD_CONFIG:			//��������
	{
		return OnRequestLoadConfig(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_GP_LOAD_RANKING_LIST:		//���ذ�
	{
		return OnRequestLoadRankingList(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_RESET_USER_DATA:		//��������
	{
		return OnRequestResetUserData(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_ONLINE_COUNT_INFO:		//������Ϣ
	{
		return OnRequestOnLineCountInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_GP_LOGON_ACCOUNTS:			//�ʺŵ�¼
	{
		return OnRequestPCLogonAccounts(dwContextID,dwTokenID,pData,wDataSize, pdwControlCode);
	}
	case DBR_GP_BIND_MACHINE:			//��������
	{
		return OnRequestPCBindMachine(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_UN_BIND_MACHINE:		//��������
	{
		return OnRequestPCUnbindMachine(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_REGISTER_ACCOUNTS:		//ע���ʺ�
	{
		return OnRequestPCRegisterAccounts(dwContextID,dwTokenID,pData,wDataSize, pdwControlCode);
	}
	case DBR_GP_QUERY_INDIVIDUAL:		//��ѯ����
	{
		return OnRequestPCQueryIndividual(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_MODIFY_ACCOUNTS:		//�޸��ʺ�
	{
		return OnRequestPCModifyAccounts(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_MODIFY_LOGON_PASS:		//�޸�����
	{
		return OnRequestPCModifyLogonPass(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_MODIFY_INSURE_PASS:		//�޸�����
	{
		return OnRequestPCModifyInsurePass(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_MODIFY_INDIVIDUAL:		//�޸�����
	{
		return OnRequestPCModifyIndividual(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_GP_QUERY_GAME_CONFIG:		//��ѯ����
	{
		return OnRequestPCQueryGameConfig(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_UPDATE_BASIC_CONFIG:	//��������
	{
		return OnRequestPCUpdateBasicConfig(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_UPDATE_WEIGHT_CONFIG:	//��������
	{
		return OnRequestPCUpdateWeightConfig(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_QUERY_PARAMETER:		//��ȡ����
	{
		return OnRequestPCQueryParameter(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_APPEND_PARAMETER:		//��Ӳ���
	{
		return OnRequestPCAppendParameter(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_MODIFY_PARAMETER:		//�޸Ĳ���
	{
		return OnRequestPCModifyParameter(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_DELETE_PARAMETER:		//ɾ������
	{
		return OnRequestPCDeleteParameter(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_QUERY_STOCK_INFO:		//��ѯ���
	{
		return OnRequestPCQueryStockInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_APPEND_STOCK_INFO:		//��ӿ��
	{
		return OnRequestPCAppendStockInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_MODIFY_STOCK_INFO:		//�޸Ŀ��
	{
		return OnRequestPCModifyStockInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GP_DELETE_STOCK_INFO:		//ɾ�����
	{
		return OnRequestPCDeleteStockInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGON_VISITOR:			//�ο͵�¼
	{
		return OnRequestMBLogonVisitor(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGON_ACCOUNTS:			//�ʺŵ�¼
	{
		return OnRequestMBLogonAccounts(dwContextID,dwTokenID,pData,wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGON_SECRETKEY:		//��Կ��¼
	{
		return OnRequestMBLogonSecretKey(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGON_MOBILECODE:		//�ֻ���֤��
	{
		return OnRequestMBLogonMobileCode(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGON_THIRDPARTY:		//��������¼
	{
		return OnRequestMBLogonThirdParty(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGON_MOBILEPHONE:		//�ֻ���¼
	{
		return OnRequestMBLogonMobilePhone(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_REGISTER_EMAIL:			//����ע��
	{
		return OnRequestMBRegisterEMail(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_REGISTER_ACCOUNTS:		//ע���ʺ�
	{
		return OnRequestMBRegisterAccounts(dwContextID,dwTokenID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_MB_REGISTER_MOBILEPHONE:	//�ֻ�ע��
	{
		return OnRequestMBRegisterMobilePhone(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGON_EMAIL:			//�����¼
	{
		return OnRequestMBLogonEMail(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGON_EMAILCODE:		//������֤���¼
	{
		return OnRequestMBLogonEMailCode(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LOGOUT_ACCOUNTS:		//�ǳ��˺�
	{
		return OnRequestMBLogoutAccounts(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_QUERY_WEALTH:			//��ѯ�Ƹ�
	{
		return OnRequestMBQueryWealth(dwContextID,dwTokenID,pData,wDataSize, pdwControlCode);
	}
	case DBR_MB_QUERY_RANKINGINFO:		//��ѯ��λ
	{
		return OnRequestMBQueryRankingInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_QUERY_LOVELINESS:		//��ѯ����
	{
		return OnRequestMBQueryLoveLiness(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_LIKE_PLAYER:			//�������
	{
		return OnRequestMBLikePlayer(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_MODIFY_LOGON_PASS:		//�޸�����
	{
		return OnRequestMBModifyLogonPass(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_MODIFY_INDIVIDUAL:		//�޸�����
	{
		return OnRequestMBModifyIndividual(dwContextID,dwTokenID,pData,wDataSize, pdwControlCode);
	}	
	case DBR_MB_MODIFY_SYSTEM_FACE:		//�޸�ͷ��
	{
		return OnRequestMBModifySystemFace(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_MODIFY_CUSTOM_FACE:		//�޸�ͷ��
	{
		return OnRequestMBModifyCustomFace(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_MB_EXCHANGE_MALLGOODS:		//�һ���Ʒ
	{
		return OnRequestMBExchangeMallGoods(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_EXCHANGE_GIFTBAGCODE:	//�һ������	
	{
		return OnRequestMBExchangeGiftbagCode(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_ACQUIRE_AUTHCODE:		//��ȡ��֤��
	{
		return OnRequestMBAcquireAuthCode(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_MB_BIND_MOBILEPHONE:		//���ֻ���
	{
		return OnRequestMBBindMobilePhone(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_UNBIND_MOBILEPHONE:		//����ֻ���
	{
		return OnRequestMBUnbindMobilePhone(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_RESET_LOGON_PASS:		//��������
	{
		return OnRequestMBResetLogonPasswd(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_MB_BIND_EMAIL:				//������
	{
		return OnRequestMBBindEMail(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_UNBIND_EMAIL:			//�������
	{
		return OnRequestMBUnbindEMail(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}	
	case DBR_MB_REQUEST_CHECKIN:		//����ǩ��
	{
		return OnRequestMBRequestCheckIn(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_BIND_INVITE_CODE:		//��������
	{
        return OnRequestMBBindInviteCode(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
    }
	case DBR_MB_SUBMIT_INFORM_INFO:		//�ύͶ��
	{
		return OnRequestMBSubmitInformInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_BIND_EXTERNAL_ACCOUNT:	//���˺�
	{
		return OnRequestMBBindExternalAccount(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_SEND_GIFT:				//��������
	{
		return OnRequestMBSendGift(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
    case DBR_MB_LOAD_GIFT:				//��������
    {
        return OnRequestMBLoadGift(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
    }
	case DBR_MB_GIFT_STATUS_UPDATE:		//״̬����
	{
        return OnRequestMBGiftStatusUpdate(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_WEALTH_TRANSFER:		//����ת��
	{
		return OnRequestMBWealthTranser(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_DRAW_COMMISSION:		//��ȡӶ��
	{
		return OnRequestMBDrawCommission(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_QUERY_TURNTABLE_LOTTERY: //��ѯת��
	{
		return OnRequestMBQueryTurntableLottery(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_REQUEST_TURNTABLE_LOTTERY: //����齱
	{
		return OnRequestMBRequestTurntableLottery(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_REAL_AUTH: //ʵ����֤
	{
		return OnRequestMBRequestRealAuthent(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_INSURE_QUERY: //���в�ѯ
	{
		return OnRequestMBRequestInsureQuery(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_INSURE_ENABLE://���п�ͨ
	{
		return OnRequestMBRequestInsureEnable(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_INSURE_SAVE: //���д洢
	{
		return OnRequestMBRequestInsureSave(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_INSURE_TAKE: //������ȡ
	{
		return OnRequestMBRequestInsureTake(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_INSURE_TRANSFER: //����ת��
	{
		return OnRequestMBRequestInsureTransfer(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_INSURE_RESET_PASSWORD://������������
	{
		return OnRequestMBInsureResetPassword(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_MB_QUERY_SUBSIDY://�ͱ���ѯ
	{
		return OnRequestMBQuerySubsidy(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	}

	return false;
}

//�ʺŵ�¼
bool CDataBaseEngineSink::OnRequestPCLogonAccounts(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_LogonByAccounts));
		if (wDataSize!=sizeof(DBR_GP_LogonByAccounts)) return false;

		//������
		DBR_GP_LogonByAccounts * pLogonByAccounts=(DBR_GP_LogonByAccounts *)pData;

		//ִ���ж�
		tagTokenParameter * pTokenParameter =(tagTokenParameter *)pLogonByAccounts->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID)
		{
			TCHAR szBuffer[128]=TEXT("");
			_sntprintf_s(szBuffer,CountArray(szBuffer),TEXT("OnRequestPCLogonAccounts Ͷ����ʱ��%ld ���룬������ʱ��%lld �� ����ִ��"),
				GetTickCount()-pLogonByAccounts->dwPostTime,time(NULL)- pTokenParameter->dwActiveTime);
			CTraceService::TraceString(szBuffer,TraceLevel_Exception);
			return true;
		}

		//��������
		LONG lReturnValue=DB_SUCCESS;
		DWORD dwNowTick=GetTickCount();

		//ת����ַ
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pLogonByAccounts->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//���Ȩ��
		if(pLogonByAccounts->dwCheckUserRight!=0)
		{
			//�������
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pLogonByAccounts->szAccounts);
			m_AccountsDBAide.AddParameter(TEXT("@dwcheckRight"),pLogonByAccounts->dwCheckUserRight);
			m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_GPLogonSuccess.szDescribeString,sizeof(m_GPLogonSuccess.szDescribeString),adParamOutput);

			//ִ�й���
			lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CheckUserRight"),false);
		}

		//��֤�ʺ�
		if(lReturnValue==DB_SUCCESS)
		{
			//�������
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonByAccounts->wGateID);
			m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonByAccounts->wLogonID);
			m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
			m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pLogonByAccounts->szAccounts);
			m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pLogonByAccounts->szPassword);			
			m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
			m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"),pLogonByAccounts->dwPlazaVersion);
			m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pLogonByAccounts->szMachineID);				
			m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_GPLogonSuccess.szDescribeString,sizeof(m_GPLogonSuccess.szDescribeString),adParamOutput);
			//m_AccountsDBAide.AddParameter(TEXT("@dwLogonServerID"),pLogonByAccounts->dwLogonServerID);

			//ִ�в�ѯ
			lReturnValue=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_EfficacyAccounts"),true);
		}
	
		//��������
		m_GPLogonSuccess.dwCheckUserRight=pLogonByAccounts->dwCheckUserRight;
		StringCchCopy(m_GPLogonSuccess.szPassword,CountArray(m_GPLogonSuccess.szPassword), pLogonByAccounts->szPassword);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID,dwTokenID,lReturnValue,CW2CT(DBVarValue.bstrVal).m_psz,false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//������
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004,L"�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),true);

		return false;
	}

	return true;
}

//ע�ᴦ��
bool CDataBaseEngineSink::OnRequestPCRegisterAccounts(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_RegisterAccounts));
		if (wDataSize!=sizeof(DBR_GP_RegisterAccounts)) return false;

		//������
		DBR_GP_RegisterAccounts * pRegisterAccounts=(DBR_GP_RegisterAccounts *)pData;

		//ִ���ж�
		tagTokenParameter * pTokenParameter = (tagTokenParameter *)pRegisterAccounts->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID)
		{
			CTraceService::TraceString(TEXT("OnRequestPCRegisterAccounts ����ִ��"),TraceLevel_Exception);
			return true;
		}

		//ת����ַ
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pRegisterAccounts->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pRegisterAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pRegisterAccounts->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pRegisterAccounts->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strBankPass"),pRegisterAccounts->szBankPass);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pRegisterAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbAge"),pRegisterAccounts->cbAge);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pRegisterAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strProvince"),pRegisterAccounts->szProvince);
		m_AccountsDBAide.AddParameter(TEXT("@strCity"),pRegisterAccounts->szCity);
		m_AccountsDBAide.AddParameter(TEXT("@strArea"),pRegisterAccounts->szArea);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"),pRegisterAccounts->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"),pRegisterAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strValidMachineSerial"),pRegisterAccounts->szValidComputerID);
		m_AccountsDBAide.AddParameter(TEXT("@cbPassportType"),pRegisterAccounts->cbPassportType);
		m_AccountsDBAide.AddParameter(TEXT("@strPassportID"),pRegisterAccounts->szPassportID);
		m_AccountsDBAide.AddParameter(TEXT("@strSpreader"),pRegisterAccounts->szSpreader);		
		m_AccountsDBAide.AddParameter(TEXT("@dwLogonServerID"),pRegisterAccounts->dwLogonServerID);
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_GPLogonSuccess.szDescribeString,sizeof(m_GPLogonSuccess.szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RegisterAccounts"),true);

		//�û�����
		StringCchCopy(m_GPLogonSuccess.szPassword,CountArray(m_GPLogonSuccess.szPassword), pRegisterAccounts->szPassword);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lReturnValue,CW2CT(DBVarValue.bstrVal).m_psz,false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//������
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004,L"�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"),true);

		//������
		DBR_GP_RegisterAccounts * pRegisterAccounts=(DBR_GP_RegisterAccounts *)pData;

		//ת����ַ
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pRegisterAccounts->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//ע����Ϣ
		CString strTrace;
		strTrace.Format(TEXT("Accounts=%s, NickName=%s, Password=%s, BankPass=%s, FaceID=%d, Age=%d, Gender=%d, Province=%s, City=%s, Area=%s, ")
			TEXT("ClientIP=%s, PlazaVersion=%d, ComputerID=%s, @PassportType=%d, PassportID=%s, Spreader=%s"), 
			pRegisterAccounts->szAccounts,pRegisterAccounts->szNickName,
			pRegisterAccounts->szPassword,pRegisterAccounts->szBankPass,
			pRegisterAccounts->wFaceID,pRegisterAccounts->cbAge, pRegisterAccounts->cbGender,pRegisterAccounts->szProvince,
			pRegisterAccounts->szCity,pRegisterAccounts->szArea, szClientIP,
			pRegisterAccounts->dwPlazaVersion,pRegisterAccounts->szMachineID,
			pRegisterAccounts->cbPassportType,pRegisterAccounts->szPassportID,pRegisterAccounts->szSpreader);

		//�������
		CTraceService::TraceString(strTrace,TraceLevel_Exception);

		return false;
	}

	return true;
}

//�޸��ʺ�
bool CDataBaseEngineSink::OnRequestPCModifyAccounts(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyAccounts));
		if (wDataSize!=sizeof(DBR_GP_ModifyAccounts)) return false;

		//������
		DBR_GP_ModifyAccounts * pModifyAccounts=(DBR_GP_ModifyAccounts *)pData;

		//ת����ַ
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pModifyAccounts->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyAccounts->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyAccounts->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"),pModifyAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strUnderWrite"),pModifyAccounts->szUnderWrite);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"),pModifyAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbAge"),pModifyAccounts->cbAge);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"),pModifyAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@strProvince"),pModifyAccounts->szProvince);
		m_AccountsDBAide.AddParameter(TEXT("@strCity"),pModifyAccounts->szCity);
		m_AccountsDBAide.AddParameter(TEXT("@strArea"),pModifyAccounts->szArea);
		m_AccountsDBAide.AddParameter(TEXT("@bOldVersion"),pModifyAccounts->bOldVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//�������
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyAccountsInfo"),false)==DB_SUCCESS)
		{
			//��������
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString,CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,dwTokenID,&OperateSuccess,wSendSize);
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.lErrorCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString,CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"����ϵͳ��æ�����Ժ����ԣ�"));

		//���ͽ��
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wSendSize);

		return false;
	}

	return true;
}

//�޸�����
bool CDataBaseEngineSink::OnRequestPCModifyLogonPass(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyLogonPass));
		if (wDataSize!=sizeof(DBR_GP_ModifyLogonPass)) return false;

		//������
		DBR_GP_ModifyLogonPass * pModifyLogonPass=(DBR_GP_ModifyLogonPass *)pData;

		//ת����ַ
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pModifyLogonPass->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyLogonPass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"),pModifyLogonPass->szScrPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"),pModifyLogonPass->szDesPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//�������
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyLogonPassword"),false)==DB_SUCCESS)
		{
			//��������
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString,CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,dwTokenID,&OperateSuccess,wSendSize);
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.lErrorCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString,CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode=pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"����ϵͳ��æ�����Ժ����ԣ�"));

		//���ͽ��
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wSendSize);

		return false;
	}

	return true;
}

//�޸�����
bool CDataBaseEngineSink::OnRequestPCModifyInsurePass(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyInsurePass));
		if (wDataSize!=sizeof(DBR_GP_ModifyInsurePass)) return false;

		//������
		DBR_GP_ModifyInsurePass * pModifyInsurePass=(DBR_GP_ModifyInsurePass *)pData;

		//ת����ַ
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pModifyInsurePass->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyInsurePass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"),pModifyInsurePass->szScrPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"),pModifyInsurePass->szDesPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//�������
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyInsurePassword"),false)==DB_SUCCESS)
		{
			//��������
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString,CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID,dwTokenID,&OperateSuccess,wSendSize);
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.lErrorCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString,CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode=pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"����ϵͳ��æ�����Ժ����ԣ�"));

		//���ͽ��
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID, dwTokenID, &OperateFailure,wSendSize);

		return false;
	}

	return true;
}

//�޸�����
bool CDataBaseEngineSink::OnRequestPCModifyIndividual(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GP_ModifyIndividual));
		if (wDataSize!=sizeof(DBR_GP_ModifyIndividual)) return false;

		//������
		DBR_GP_ModifyIndividual * pModifyIndividual=(DBR_GP_ModifyIndividual *)pData;

		//ת����ַ
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pModifyIndividual->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyIndividual->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"),pModifyIndividual->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"),pModifyIndividual->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@strQQ"),pModifyIndividual->szQQ);
		m_AccountsDBAide.AddParameter(TEXT("@strEMail"),pModifyIndividual->szEMail);
		m_AccountsDBAide.AddParameter(TEXT("@strSeatPhone"),pModifyIndividual->szSeatPhone);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"),pModifyIndividual->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strDwellingPlace"),pModifyIndividual->szDwellingPlace);
		m_AccountsDBAide.AddParameter(TEXT("@strUserNote"),pModifyIndividual->szUserNote);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//�������
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyUserIndividual"),false)==DB_SUCCESS)
		{
			//��������
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString,CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS,dwContextID, dwTokenID, &OperateSuccess,wSendSize);
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.lErrorCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString,CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID, dwTokenID, &OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode=pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString,CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"����ϵͳ��æ�����Ժ����ԣ�"));

		//���ͽ��
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE,dwContextID, dwTokenID, &OperateFailure,wSendSize);

		return false;
	}

	return true;
}

//��ѯ����
bool CDataBaseEngineSink::OnRequestPCQueryIndividual(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GP_QueryIndividual));
		if (wDataSize != sizeof(DBR_GP_QueryIndividual)) return false;

		//������
		DBR_GP_QueryIndividual* pQueryIndividual = (DBR_GP_QueryIndividual*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pQueryIndividual->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pQueryIndividual->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);

		//�������
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserIndividual"), true) == DB_SUCCESS)
		{
			//�û���Ϣ
			m_UserIndividual.dwUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_AccountsDBAide.GetValue_String(TEXT("UserNote"), m_UserIndividual.szUserNote, CountArray(m_UserIndividual.szUserNote));
			m_AccountsDBAide.GetValue_String(TEXT("Compellation"), m_UserIndividual.szCompellation, CountArray(m_UserIndividual.szCompellation));

			//�绰����
			m_AccountsDBAide.GetValue_String(TEXT("SeatPhone"), m_UserIndividual.szSeatPhone, CountArray(m_UserIndividual.szSeatPhone));
			m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"), m_UserIndividual.szMobilePhone, CountArray(m_UserIndividual.szMobilePhone));

			//��ϵ����
			m_AccountsDBAide.GetValue_String(TEXT("QQ"), m_UserIndividual.szQQ, CountArray(m_UserIndividual.szQQ));
			m_AccountsDBAide.GetValue_String(TEXT("EMail"), m_UserIndividual.szEMail, CountArray(m_UserIndividual.szEMail));
			m_AccountsDBAide.GetValue_String(TEXT("DwellingPlace"), m_UserIndividual.szDwellingPlace, CountArray(m_UserIndividual.szDwellingPlace));

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_USER_INDIVIDUAL, dwContextID, dwTokenID, &m_UserIndividual, sizeof(m_UserIndividual));
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"����ϵͳ��æ�����Ժ����ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestPCBindMachine(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GP_BindMachine));
		if (wDataSize != sizeof(DBR_GP_BindMachine)) return false;

		//������
		DBR_GP_BindMachine* pBindMachine = (DBR_GP_BindMachine*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pBindMachine->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pBindMachine->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pBindMachine->szMachineID);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_BindMachine"), false);

		//��������
		DBO_GP_BindMachineResult BindMachineResult = { 0 };
		BindMachineResult.lResultCode = lReturnValue;

		//��ȡ��Ϣ
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		StringCchCopy(BindMachineResult.szDescribeString, CountArray(BindMachineResult.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(BindMachineResult.szDescribeString);
		WORD wSendSize = sizeof(BindMachineResult) - sizeof(BindMachineResult.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BIND_MANCHINE_RESULT, dwContextID, dwTokenID, &BindMachineResult, wSendSize);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GP_BindMachineResult BindMachineResult = { 0 };

		//���ñ���
		BindMachineResult.lResultCode = pIException->GetExceptionResult();

		//��ȡ��Ϣ
		StringCchCopy(BindMachineResult.szDescribeString, CountArray(BindMachineResult.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(BindMachineResult.szDescribeString);
		WORD wSendSize = sizeof(BindMachineResult) - sizeof(BindMachineResult.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BIND_MANCHINE_RESULT, dwContextID, dwTokenID, &BindMachineResult, wSendSize);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestPCUnbindMachine(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GP_BindMachine));
		if (wDataSize != sizeof(DBR_GP_BindMachine)) return false;

		//������
		DBR_GP_UnbindMachine* pUnBindMachine = (DBR_GP_UnbindMachine*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUnBindMachine->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pUnBindMachine->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pUnBindMachine->szMachineID);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UnBindMachine"), false);

		//��������
		DBO_GP_BindMachineResult BindMachineResult = { 0 };
		BindMachineResult.lResultCode = lReturnValue;

		if (lReturnValue != DB_SUCCESS)
		{
			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
			StringCchCopy(BindMachineResult.szDescribeString, CountArray(BindMachineResult.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);
		}

		//���ͽ��
		WORD wStringSize = CountStringBuffer(BindMachineResult.szDescribeString);
		WORD wSendSize = sizeof(BindMachineResult) - sizeof(BindMachineResult.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BIND_MANCHINE_RESULT, dwContextID, dwTokenID, &BindMachineResult, wSendSize);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GP_BindMachineResult BindMachineResult = { 0 };

		//���ñ���
		BindMachineResult.lResultCode = pIException->GetExceptionResult();

		//��ȡ��Ϣ
		StringCchCopy(BindMachineResult.szDescribeString, CountArray(BindMachineResult.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(BindMachineResult.szDescribeString);
		WORD wSendSize = sizeof(BindMachineResult) - sizeof(BindMachineResult.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_BIND_MANCHINE_RESULT, dwContextID, dwTokenID, &BindMachineResult, wSendSize);

		return false;
	}

	return true;
}

//��ѯ����
bool CDataBaseEngineSink::OnRequestPCQueryGameConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT (wDataSize == sizeof(DBR_GP_QueryGameConfig));
		if (wDataSize != sizeof(DBR_GP_QueryGameConfig)) return false;

		//������
		auto pQueryGameConfig = (DBR_GP_QueryGameConfig*)pData;

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"), pQueryGameConfig->wKindID);
		
		//ִ�в�ѯ
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameBasicConfig"), true);
		if (lResultCode == DB_SUCCESS && m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//�������
			DBO_GP_GameBasicConfig BasicConfig = { 0 };

			//��ȡ����
			m_PlatformDBAide.GetValue_String(TEXT("ConfigValue"), BasicConfig.szBasicConfig,CountArray(BasicConfig.szBasicConfig));			

			//������Ϣ
			WORD wSendSize = CountStringBuffer(BasicConfig.szBasicConfig);
			wSendSize += sizeof(BasicConfig) - sizeof(BasicConfig.szBasicConfig);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_BASIC_CONFIG, dwContextID, dwTokenID, &BasicConfig, wSendSize);
		}		

		//��������
		for (BYTE cbTypeID = 0; cbTypeID < 2; cbTypeID++)
		{
			//�������
			m_PlatformDBAide.ResetParameter();
			m_PlatformDBAide.AddParameter(TEXT("@wKindID"), pQueryGameConfig->wKindID);
			m_PlatformDBAide.AddParameter(TEXT("@cbTypeID"), cbTypeID);

			//ִ�в�ѯ
			LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameWeightConfig"), true);
			if (lResultCode == DB_SUCCESS)
			{
				//�������
				DBO_GP_GameWeightConfig WeightConfig = {0};

				//��ȡ��Ϣ
				while (m_PlatformDBModule->IsRecordsetEnd() == false)
				{
					auto pWeightItem = &WeightConfig.TimesWeightItem[WeightConfig.wItemCount++];

					//��ȡ����
					pWeightItem->cbWeightType = cbTypeID;
					pWeightItem->dwTimesRange[0] = m_PlatformDBAide.GetValue_WORD(TEXT("MinTimes"));
					pWeightItem->dwTimesRange[1] = m_PlatformDBAide.GetValue_WORD(TEXT("MaxTimes"));
					pWeightItem->cbTimesKind = m_PlatformDBAide.GetValue_BYTE(TEXT("TimesKind"));
					pWeightItem->wTimesWeight = m_PlatformDBAide.GetValue_WORD(TEXT("WeightValue"));
					pWeightItem->fWightRatio = m_PlatformDBAide.GetValue_FLOAT(TEXT("WeightRatio"));					

					//�ƶ���¼
					m_PlatformDBModule->MoveToNext();
				}

				//���ͽ��
				WORD wSendSize = sizeof(WeightConfig) - sizeof(WeightConfig.TimesWeightItem);
				wSendSize += WeightConfig.wItemCount * sizeof(WeightConfig.TimesWeightItem[0]);				
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_WEIGHT_CONFIG, dwContextID, dwTokenID, &WeightConfig, wSendSize);
			}			
		}

		//ִ�н��
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_CONFIG_FINISH, dwContextID, dwTokenID, NULL, 0);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("���ݿ�����쳣�����Ժ��ٴγ��ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestPCUpdateBasicConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize <= sizeof(DBR_GP_UpdateBasicConfig));
		if (wDataSize > sizeof(DBR_GP_UpdateBasicConfig)) return false;

		//������
		auto pUpdateBasicConfig = (DBR_GP_UpdateBasicConfig*)pData;

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"), pUpdateBasicConfig->wKindID);
		m_PlatformDBAide.AddParameter(TEXT("@strBasicConfig"), pUpdateBasicConfig->szBasicConfig);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_UpdateGameBasicConfig"), false);
		if (lResultCode == DB_SUCCESS)
		{
			//��������
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//�������
			StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString),TEXT("�������ñ���ɹ���"));

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wSendSize);
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("���ݿ�����쳣�����Ժ��ٴγ��ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestPCUpdateWeightConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize <= sizeof(DBR_GP_UpdateWeightConfig));
		if (wDataSize > sizeof(DBR_GP_UpdateWeightConfig)) return false;

		//������
		auto pUpdateWeightConfig = (DBR_GP_UpdateWeightConfig*)pData;

		//���л�Ȩ��
		TCHAR szWeightConfig[4000] = { 0 };
		SerializeWeightConfig(pUpdateWeightConfig->TimesWeightItem, pUpdateWeightConfig->wItemCount, szWeightConfig,CountArray(szWeightConfig));

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"), pUpdateWeightConfig->wKindID);
		m_PlatformDBAide.AddParameter(TEXT("@cbWeightType"), pUpdateWeightConfig->cbTypeID);
		m_PlatformDBAide.AddParameter(TEXT("@strWeightConfig"), szWeightConfig);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_UpdateGameWeightConfig"), false);
		if (lResultCode == DB_SUCCESS)
		{
			//��������
			DBO_GP_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//�������
			StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString), TEXT("Ȩ�����ñ���ɹ���"));

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wSendSize);
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("���ݿ�����쳣�����Ժ��ٴγ��ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//��ȡ����
bool CDataBaseEngineSink::OnRequestPCQueryParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GP_QueryParameter));
		if (wDataSize != sizeof(DBR_GP_QueryParameter)) return false;

		//������
		DBR_GP_QueryParameter* pQueryParameter = (DBR_GP_QueryParameter*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wServerID"), pQueryParameter->wServerID);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RobotQueryParameter"), true);

		//ִ�н��
		OnRobotDisposeResult(dwContextID, dwTokenID, lResultCode, SUB_GP_QUERY_PARAMETER, pQueryParameter->wServerID);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("���ݿ�����쳣�����Ժ��ٴγ��ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//��Ӳ���
bool CDataBaseEngineSink::OnRequestPCAppendParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GP_AppendParameter));
		if (wDataSize != sizeof(DBR_GP_AppendParameter)) return false;

		//������
		DBR_GP_AppendParameter* pAppendParameter = (DBR_GP_AppendParameter*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wServerID"), pAppendParameter->wServerID);
		m_AccountsDBAide.AddParameter(TEXT("@dwServiceMode"), pAppendParameter->RobotParameter.dwServiceMode);
		m_AccountsDBAide.AddParameter(TEXT("@dwRobotCount"), pAppendParameter->RobotParameter.dwRobotCount);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterTime"), pAppendParameter->RobotParameter.dwEnterTime);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveTime"), pAppendParameter->RobotParameter.dwLeaveTime);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterMinInterval"), pAppendParameter->RobotParameter.dwEnterMinInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterMaxInterval"), pAppendParameter->RobotParameter.dwEnterMaxInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveMinInterval"), pAppendParameter->RobotParameter.dwLeaveMinInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveMaxInterval"), pAppendParameter->RobotParameter.dwLeaveMaxInterval);
		m_AccountsDBAide.AddParameter(TEXT("@lTakeMinScore"), pAppendParameter->RobotParameter.lTakeMinScore);
		m_AccountsDBAide.AddParameter(TEXT("@lTakeMaxScore"), pAppendParameter->RobotParameter.lTakeMaxScore);
		m_AccountsDBAide.AddParameter(TEXT("@dwSwitchMinInnings"), pAppendParameter->RobotParameter.dwSwitchMinInnings);
		m_AccountsDBAide.AddParameter(TEXT("@dwSwitchMaxInnings"), pAppendParameter->RobotParameter.dwSwitchMaxInnings);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RobotAppendParameter"), true);

		//ִ�н��
		OnRobotDisposeResult(dwContextID, dwTokenID, lResultCode, SUB_GP_APPEND_PARAMETER, pAppendParameter->wServerID);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("���ݿ�����쳣�����Ժ��ٴγ��ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//�޸Ĳ���
bool CDataBaseEngineSink::OnRequestPCModifyParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GP_ModifyParameter));
		if (wDataSize != sizeof(DBR_GP_ModifyParameter)) return false;

		//������
		DBR_GP_ModifyParameter* pModifyParameter = (DBR_GP_ModifyParameter*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwDatchID"), pModifyParameter->RobotParameter.dwBatchID);
		m_AccountsDBAide.AddParameter(TEXT("@dwServiceMode"), pModifyParameter->RobotParameter.dwServiceMode);
		m_AccountsDBAide.AddParameter(TEXT("@dwRobotCount"), pModifyParameter->RobotParameter.dwRobotCount);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterTime"), pModifyParameter->RobotParameter.dwEnterTime);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveTime"), pModifyParameter->RobotParameter.dwLeaveTime);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterMinInterval"), pModifyParameter->RobotParameter.dwEnterMinInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwEnterMaxInterval"), pModifyParameter->RobotParameter.dwEnterMaxInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveMinInterval"), pModifyParameter->RobotParameter.dwLeaveMinInterval);
		m_AccountsDBAide.AddParameter(TEXT("@dwLeaveMaxInterval"), pModifyParameter->RobotParameter.dwLeaveMaxInterval);
		m_AccountsDBAide.AddParameter(TEXT("@lTakeMinScore"), pModifyParameter->RobotParameter.lTakeMinScore);
		m_AccountsDBAide.AddParameter(TEXT("@lTakeMaxScore"), pModifyParameter->RobotParameter.lTakeMaxScore);
		m_AccountsDBAide.AddParameter(TEXT("@dwSwitchMinInnings"), pModifyParameter->RobotParameter.dwSwitchMinInnings);
		m_AccountsDBAide.AddParameter(TEXT("@dwSwitchMaxInnings"), pModifyParameter->RobotParameter.dwSwitchMaxInnings);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RobotModifyParameter"), true);

		//ִ�н��
		OnRobotDisposeResult(dwContextID, dwTokenID, lResultCode, SUB_GP_MODIFY_PARAMETER, pModifyParameter->wServerID);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("���ݿ�����쳣�����Ժ��ٴγ��ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//ɾ������
bool CDataBaseEngineSink::OnRequestPCDeleteParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GP_DeleteParameter));
		if (wDataSize != sizeof(DBR_GP_DeleteParameter)) return false;

		//������
		DBR_GP_DeleteParameter* pDeleteParameter = (DBR_GP_DeleteParameter*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwBatchID"), pDeleteParameter->dwBatchID);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RobotDeleteParameter"), true);

		//ִ�н��
		OnRobotDisposeResult(dwContextID, dwTokenID, lResultCode, SUB_GP_DELETE_PARAMETER, pDeleteParameter->wServerID);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("���ݿ�����쳣�����Ժ��ٴγ��ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//��ѯ���
bool CDataBaseEngineSink::OnRequestPCQueryStockInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GP_QueryStockInfo));
		if (wDataSize != sizeof(DBR_GP_QueryStockInfo)) return false;

		//������
		auto pQueryStockInfo = (DBR_GP_QueryStockInfo*)pData;

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@strKindIDList"), pQueryStockInfo->szKindID);

		//ִ�в�ѯ
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_QueryStockList"), true);

		if (lResultCode == DB_SUCCESS)
		{
			OnStockDisposeResult(dwContextID,dwTokenID,0,SUB_GP_QUERY_STOCK_INFO);			
		}		

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("���ݿ�����쳣�����Ժ��ٴγ��ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//��ӿ��
bool CDataBaseEngineSink::OnRequestPCAppendStockInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GP_AppendStockInfo));
		if (wDataSize != sizeof(DBR_GP_AppendStockInfo)) return false;

		//��ȡ����
		auto pAppendStockInfo = (DBR_GP_AppendStockInfo*)pData;

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"), pAppendStockInfo->StockInfo.wKindID);
		m_PlatformDBAide.AddParameter(TEXT("@lBetAmount"), pAppendStockInfo->StockInfo.lBetAmount);
		m_PlatformDBAide.AddParameter(TEXT("@wStockKind"), pAppendStockInfo->StockInfo.wStockKind);
		m_PlatformDBAide.AddParameter(TEXT("@wStockLevel"), pAppendStockInfo->StockInfo.wStockLevel);
		m_PlatformDBAide.AddParameter(TEXT("@strStockName"), pAppendStockInfo->StockInfo.szStockName);
		m_PlatformDBAide.AddParameter(TEXT("@lExtraScore"), pAppendStockInfo->StockInfo.lExtraScore);
		m_PlatformDBAide.AddParameter(TEXT("@lStockScore"), pAppendStockInfo->StockInfo.lStockScore);
		m_PlatformDBAide.AddParameter(TEXT("@cbEnableMode"), pAppendStockInfo->StockInfo.cbEnableMode);
		m_PlatformDBAide.AddParameter(TEXT("@dwEnableEndTime"), pAppendStockInfo->StockInfo.dwEnableEndTime);
		m_PlatformDBAide.AddParameter(TEXT("@dwEnableStartTime"), pAppendStockInfo->StockInfo.dwEnableStartTime);
		m_PlatformDBAide.AddParameter(TEXT("@strShrinkOption"), pAppendStockInfo->StockInfo.szShrinkOption);
		m_PlatformDBAide.AddParameter(TEXT("@strRelationIDList"), pAppendStockInfo->StockInfo.szRelationStockID);

		//���л�
		TCHAR szJackpotOption[2048] = { 0 };
		SerializeJackpotOption(pAppendStockInfo->StockInfo.GameJackpotItem, pAppendStockInfo->StockInfo.wJackpotCount, szJackpotOption, CountArray(szJackpotOption));

		//�ʳ���Ϣ
		m_PlatformDBAide.AddParameter(TEXT("@strJackpotInfo"), szJackpotOption);
		
		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_AppendStockInfo"), true);
		if (lResultCode == DB_SUCCESS)
		{
			OnStockDisposeResult(dwContextID, dwTokenID, 0, SUB_GP_APPEND_STOCK_INFO);					
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("���ݿ�����쳣�����Ժ��ٴγ��ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//�޸Ŀ��
bool CDataBaseEngineSink::OnRequestPCModifyStockInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GP_ModifyStockInfo));
		if (wDataSize != sizeof(DBR_GP_ModifyStockInfo)) return false;

		//��ȡ����
		auto pModifyStockInfo = (DBR_GP_ModifyStockInfo*)pData;

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wStockID"), pModifyStockInfo->StockInfo.wStockID);
		m_PlatformDBAide.AddParameter(TEXT("@wKindID"), pModifyStockInfo->StockInfo.wKindID);
		m_PlatformDBAide.AddParameter(TEXT("@lBetAmount"), pModifyStockInfo->StockInfo.lBetAmount);
		m_PlatformDBAide.AddParameter(TEXT("@wStockKind"), pModifyStockInfo->StockInfo.wStockKind);
		m_PlatformDBAide.AddParameter(TEXT("@wStockLevel"), pModifyStockInfo->StockInfo.wStockLevel);
		m_PlatformDBAide.AddParameter(TEXT("@strStockName"), pModifyStockInfo->StockInfo.szStockName);
		m_PlatformDBAide.AddParameter(TEXT("@lExtraScore"), pModifyStockInfo->StockInfo.lExtraScore);
		m_PlatformDBAide.AddParameter(TEXT("@cbEnableMode"), pModifyStockInfo->StockInfo.cbEnableMode);
		m_PlatformDBAide.AddParameter(TEXT("@dwEnableEndTime"), pModifyStockInfo->StockInfo.dwEnableEndTime);
		m_PlatformDBAide.AddParameter(TEXT("@dwEnableStartTime"), pModifyStockInfo->StockInfo.dwEnableStartTime);
		m_PlatformDBAide.AddParameter(TEXT("@strShrinkOption"), pModifyStockInfo->StockInfo.szShrinkOption);
		m_PlatformDBAide.AddParameter(TEXT("@strRelationIDList"), pModifyStockInfo->StockInfo.szRelationStockID);
		
		//���л�
		TCHAR szJackpotOption[2048] = { 0 };
		SerializeJackpotOption(pModifyStockInfo->StockInfo.GameJackpotItem, pModifyStockInfo->StockInfo.wJackpotCount, szJackpotOption, CountArray(szJackpotOption));

		//�ʳ���Ϣ
		m_PlatformDBAide.AddParameter(TEXT("@strJackpotInfo"), szJackpotOption);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_ModifyStockInfo"), true);
		if (lResultCode == DB_SUCCESS)
		{
			OnStockDisposeResult(dwContextID, dwTokenID, 0, SUB_GP_MODIFY_STOCK_INFO);			
		}
		else
		{
			//��������
			DBO_GP_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_PlatformDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("���ݿ�����쳣�����Ժ��ٴγ��ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//ɾ�����
bool CDataBaseEngineSink::OnRequestPCDeleteStockInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GP_DeleteStockInfo));
		if (wDataSize != sizeof(DBR_GP_DeleteStockInfo)) return false;

		//��ȡ����
		auto pDeleteStockInfo = (DBR_GP_DeleteStockInfo*)pData;

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wStockID"), pDeleteStockInfo->wStockID);

		//ִ�в�ѯ
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_DeleteStockInfo"), true);
		if (lResultCode == DB_SUCCESS)
		{
			OnStockDisposeResult(dwContextID, dwTokenID, 0, SUB_GP_DELETE_STOCK_INFO);			
		}	

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = m_PlatformDBModule->GetReturnValue();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("���ݿ�����쳣�����Ժ��ٴγ��ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//�ο͵�¼
bool CDataBaseEngineSink::OnRequestMBLogonVisitor(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_LogonVisitor));
		if (wDataSize != sizeof(DBR_MB_LogonVisitor)) return false;

		//������
		DBR_MB_LogonVisitor* pLogonVisitor = (DBR_MB_LogonVisitor*)pData;

		//ִ���ж�
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pLogonVisitor->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//ת����ַ
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pLogonVisitor->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//�������		
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonVisitor->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonVisitor->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pLogonVisitor->wMarketID);
        m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"), pLogonVisitor->dwSpreaderID);
        m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonVisitor->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pLogonVisitor->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pLogonVisitor->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pLogonVisitor->szMachineOSID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineName"), pLogonVisitor->szMachineName);
		m_AccountsDBAide.AddParameter(TEXT("@strPackageName"), pLogonVisitor->szPackageName);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyVisitor"), true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"), true);

		return false;
	}

	return true;
}

//�ʺŵ�¼
bool CDataBaseEngineSink::OnRequestMBLogonAccounts(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_LogonAccounts));
		if (wDataSize != sizeof(DBR_MB_LogonAccounts)) return false;

		//������
		DBR_MB_LogonAccounts* pLogonAccounts = (DBR_MB_LogonAccounts*)pData;

		//ִ���ж�
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pLogonAccounts->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//ת����ַ
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pLogonAccounts->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonAccounts->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonAccounts->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"), pLogonAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pLogonAccounts->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pLogonAccounts->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonAccounts->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pLogonAccounts->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pLogonAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pLogonAccounts->szMachineOSID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pLogonAccounts->szMobilePhone);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyAccounts"), true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"), true);

		return false;
	}

	return true;
}

//��Կ��¼
bool CDataBaseEngineSink::OnRequestMBLogonSecretKey(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_LogonSecretKey));
		if (wDataSize != sizeof(DBR_MB_LogonSecretKey)) return false;

		//������
		DBR_MB_LogonSecretKey* pLogonSecretKey = (DBR_MB_LogonSecretKey*)pData;

		//ִ���ж�
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pLogonSecretKey->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//��������
		string sLogonToken(CT2CA(pLogonSecretKey->szSecretKey).m_psz);

		//����JWT
		auto jwtPtr = CWHJwt::Parse(sLogonToken);
		if (jwtPtr->Verify() == false)
		{
			OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0008, L"���ĵ�¼�����ѹ��ڣ�"), true);

			return true;
		}

		//��ȡ��Կ
		auto sSecretKey = jwtPtr->Claims().Get<string>("SecretKey");
		if (sSecretKey.empty())
		{
			OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0008, L"���ĵ�¼�����ѹ��ڣ�"), true);

			return true;
		}		

		//ת����ַ
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pLogonSecretKey->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonSecretKey->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonSecretKey->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pLogonSecretKey->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strLogonToken"), CA2CT(sSecretKey.c_str()).m_psz);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pLogonSecretKey->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonSecretKey->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pLogonSecretKey->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pLogonSecretKey->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pLogonSecretKey->szMachineOSID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pLogonSecretKey->szMobilePhone);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacySecretKey"), true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"), true);

		return false;
	}

	return true;
}

//��֤���¼
bool CDataBaseEngineSink::OnRequestMBLogonMobileCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_LogonMobileCode));
		if (wDataSize != sizeof(DBR_MB_LogonMobileCode)) return false;

		//������
		auto pLogonMobileCode = (DBR_MB_LogonMobileCode*)pData;

		//ִ���ж�
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pLogonMobileCode->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//ת����ַ
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pLogonMobileCode->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonMobileCode->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonMobileCode->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCode"), pLogonMobileCode->szAuthCode);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pLogonMobileCode->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pLogonMobileCode->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"), pLogonMobileCode->dwSpreaderID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonMobileCode->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pLogonMobileCode->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pLogonMobileCode->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pLogonMobileCode->szMachineOSID);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyMobileCode"), true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"), true);

		return false;
	}

	return true;
}

//������¼
bool CDataBaseEngineSink::OnRequestMBLogonThirdParty(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_LogonThirdParty));
		if (wDataSize != sizeof(DBR_MB_LogonThirdParty)) return false;

		//������
		DBR_MB_LogonThirdParty* pLogonThirdParty = (DBR_MB_LogonThirdParty*)pData;

		//ִ���ж�
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pLogonThirdParty->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//ת����ַ
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pLogonThirdParty->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonThirdParty->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonThirdParty->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
        m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"), pLogonThirdParty->dwSpreaderID);
        m_AccountsDBAide.AddParameter(TEXT("@dwExternalID"), pLogonThirdParty->dwExternalID);
		m_AccountsDBAide.AddParameter(TEXT("@strUniqueID"), pLogonThirdParty->szUniqueID);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"), pLogonThirdParty->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strUserDatum"), pLogonThirdParty->szUserDatum);
		m_AccountsDBAide.AddParameter(TEXT("@strUserFaceUrl"), pLogonThirdParty->szUserFaceUrl);//kk jia
		m_AccountsDBAide.AddParameter(TEXT("@strCompellation"), pLogonThirdParty->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"), pLogonThirdParty->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pLogonThirdParty->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonThirdParty->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pLogonThirdParty->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pLogonThirdParty->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pLogonThirdParty->szMachineOSID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pLogonThirdParty->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strPackageName"), pLogonThirdParty->szPackageName);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyThirdParty"), true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true,true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"), true);

		return false;
	}

	return true;
}

//�ʺŵ�¼
bool CDataBaseEngineSink::OnRequestMBLogonMobilePhone(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_LogonMobilePhone));
		if (wDataSize != sizeof(DBR_MB_LogonMobilePhone)) return false;

		//������
		DBR_MB_LogonMobilePhone* pLogonMobilePhone = (DBR_MB_LogonMobilePhone*)pData;

		//ִ���ж�
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pLogonMobilePhone->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//ת����ַ
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pLogonMobilePhone->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonMobilePhone->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonMobilePhone->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pLogonMobilePhone->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pLogonMobilePhone->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pLogonMobilePhone->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonMobilePhone->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pLogonMobilePhone->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pLogonMobilePhone->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pLogonMobilePhone->szMachineOSID);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyMobilePhone"), true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"), true);

		return false;
	}

	return true;
}

//�ʺ�ע��
bool CDataBaseEngineSink::OnRequestMBRegisterAccounts(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_RegisterAccounts));
		if (wDataSize != sizeof(DBR_MB_RegisterAccounts)) return false;

		//������
		DBR_MB_RegisterAccounts* pRegisterAccounts = (DBR_MB_RegisterAccounts*)pData;

		//ִ���ж�
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pRegisterAccounts->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//ת����ַ
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pRegisterAccounts->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pRegisterAccounts->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pRegisterAccounts->wLogonID);
        m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
        m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"), pRegisterAccounts->dwSpreaderID);
		m_AccountsDBAide.AddParameter(TEXT("@strAccounts"), pRegisterAccounts->szAccounts);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"), pRegisterAccounts->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pRegisterAccounts->szLogonPass);		
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"), pRegisterAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"), pRegisterAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pRegisterAccounts->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pRegisterAccounts->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pRegisterAccounts->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pRegisterAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pRegisterAccounts->szMachineOSID);
		m_AccountsDBAide.AddParameter(TEXT("@strPackageName"), pRegisterAccounts->szPackageName);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_RegisterAccounts"), true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"), true);

		return false;
	}

	return true;
}

//�ֻ�ע�� 
bool CDataBaseEngineSink::OnRequestMBRegisterMobilePhone(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_RegisterMobilePhone));
		if (wDataSize != sizeof(DBR_MB_RegisterMobilePhone)) return false;

		//������
		auto pRegisterMobilePhone = (DBR_MB_RegisterMobilePhone*)pData;

		//ִ���ж�
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pRegisterMobilePhone->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//ת����ַ
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pRegisterMobilePhone->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pRegisterMobilePhone->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pRegisterMobilePhone->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"), pRegisterMobilePhone->dwSpreaderID);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCode"), pRegisterMobilePhone->szAuthCode);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pRegisterMobilePhone->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"), pRegisterMobilePhone->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pRegisterMobilePhone->szLogonPass);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"), pRegisterMobilePhone->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"), pRegisterMobilePhone->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pRegisterMobilePhone->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pRegisterMobilePhone->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pRegisterMobilePhone->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pRegisterMobilePhone->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pRegisterMobilePhone->szMachineOSID);
		m_AccountsDBAide.AddParameter(TEXT("@strPackageName"), pRegisterMobilePhone->szPackageName);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_RegisterMobilePhone"), true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"), true);

		return false;
	}

	return true;
}

//����ע�� 
bool CDataBaseEngineSink::OnRequestMBRegisterEMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_RegisterEMail));
		if (wDataSize != sizeof(DBR_MB_RegisterEMail)) return false;

		//������
		DBR_MB_RegisterEMail* pRegisterAccounts = (DBR_MB_RegisterEMail*)pData;

		//ִ���ж�
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pRegisterAccounts->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//ת����ַ
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pRegisterAccounts->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pRegisterAccounts->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pRegisterAccounts->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"), pRegisterAccounts->dwSpreaderID);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCode"), pRegisterAccounts->szAuthCode);
		m_AccountsDBAide.AddParameter(TEXT("@strEMail"), pRegisterAccounts->szEMail);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"), pRegisterAccounts->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pRegisterAccounts->szLogonPass);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"), pRegisterAccounts->wFaceID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"), pRegisterAccounts->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pRegisterAccounts->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pRegisterAccounts->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pRegisterAccounts->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pRegisterAccounts->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pRegisterAccounts->szMachineOSID);
		m_AccountsDBAide.AddParameter(TEXT("@strPackageName"), pRegisterAccounts->szPackageName);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_RegisterEMail"), true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"), true);

		return false;
	}

	return true;
}

//�����½
bool CDataBaseEngineSink::OnRequestMBLogonEMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_LogonEMail));
		if (wDataSize != sizeof(DBR_MB_LogonEMail)) return false;

		//������
		DBR_MB_LogonEMail* pLogonEMail = (DBR_MB_LogonEMail*)pData;

		//ִ���ж�
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pLogonEMail->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//ת����ַ
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pLogonEMail->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonEMail->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonEMail->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@strEMail"), pLogonEMail->szEMail);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pLogonEMail->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pLogonEMail->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonEMail->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pLogonEMail->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pLogonEMail->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pLogonEMail->szMachineOSID);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyEMail"), true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"), true);

		return false;
	}

	return true;
}

//��֤���¼
bool CDataBaseEngineSink::OnRequestMBLogonEMailCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_LogonEMailCode));
		if (wDataSize != sizeof(DBR_MB_LogonEMailCode)) return false;

		//������
		DBR_MB_LogonEMailCode* pLogonMobileCode = (DBR_MB_LogonEMailCode*)pData;

		//ִ���ж�
		tagTokenParameter* pTokenParameter = (tagTokenParameter*)pLogonMobileCode->pTokenParameter;
		if (pTokenParameter->dwTokenID != dwTokenID) return true;

		//ת����ַ
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pLogonMobileCode->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogonMobileCode->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogonMobileCode->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pTokenParameter->dwTokenID);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCode"), pLogonMobileCode->szAuthCode);
		m_AccountsDBAide.AddParameter(TEXT("@strEMail"), pLogonMobileCode->szEMail);
		m_AccountsDBAide.AddParameter(TEXT("@wMarketID"), pLogonMobileCode->wMarketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSpreaderID"), pLogonMobileCode->dwSpreaderID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonMobileCode->dwDeviceKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwPlazaVersion"), pLogonMobileCode->dwPlazaVersion);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientAddr);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pLogonMobileCode->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineOSID"), pLogonMobileCode->szMachineOSID);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyEMailAuthCode"), true);

		//�������
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);
		OnLogonDisposeResult(dwContextID, dwTokenID, lResultCode, CW2CT(DBVarValue.bstrVal).m_psz, true);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		OnLogonDisposeResult(dwContextID, dwTokenID, DB_ERROR, CWHService::FormatMessage(MIM_LOGON_0004, L"�������ݿ�����쳣�������Ժ����Ի�ѡ����һ��������¼��"), true);

		return false;
	}

	return true;
}

//�ʺŵǳ�
bool CDataBaseEngineSink::OnRequestMBLogoutAccounts(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT (wDataSize == sizeof(DBR_MB_LogoutAccounts));
		if (wDataSize != sizeof(DBR_MB_LogoutAccounts)) return false;

		//������
		DBR_MB_LogoutAccounts* pLogoutAccounts = (DBR_MB_LogoutAccounts*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wGateID"), pLogoutAccounts->wGateID);
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), pLogoutAccounts->wLogonID);
		m_AccountsDBAide.AddParameter(TEXT("@dwSocketID"), pLogoutAccounts->dwSocketID);
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pLogoutAccounts->dwUserID);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_LogoutAccounts"), false);

		//�ǳ����
		DBO_MB_LogoutResult LogoutResult;
		ZeroMemory(&LogoutResult,sizeof(LogoutResult));

		//���ñ���
		LogoutResult.lResultCode = lResultCode;
		LogoutResult.dwUserID = pLogoutAccounts->dwUserID;		

		//Ͷ����Ϣ
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_LOGOUT_RESULT, dwContextID, dwTokenID, &LogoutResult, sizeof(LogoutResult));

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//�������
bool CDataBaseEngineSink::OnRequestMBLikePlayer(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_LikePlayer));
		if (wDataSize != sizeof(DBR_MB_LikePlayer)) return false;

		//������
		DBR_MB_LikePlayer* pLikePlayer = (DBR_MB_LikePlayer*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pLikePlayer->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pLikePlayer->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwDstUserID"), pLikePlayer->dwDstUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//�������
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_LikePlayer"), true) == DB_SUCCESS)
		{
			//��������
			DWORD dwLinkedNumber = m_AccountsDBAide.GetValue_DWORD(TEXT("LikedNumber"));

			//����ṹ
			UserPublicInfo PublicInfo;
			PublicInfo.set_uid(pLikePlayer->dwDstUserID);
			//PublicInfo.set_likednumber(dwLinkedNumber);

			//������Ϣ
			UpdateUserCache(dwContextID, dwTokenID, PublicInfo);

			//��������
			DBO_MB_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateSuccess.wRequestCmdID = SUB_MB_LIKE_PLAYER;
			OperateSuccess.lResultCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wSendSize);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_LIKE_PLAYER;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.wRequestCmdID = SUB_MB_LIKE_PLAYER;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"����ϵͳ��æ�����Ժ����ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//�޸�����
bool CDataBaseEngineSink::OnRequestMBModifyLogonPass(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT (wDataSize == sizeof(DBR_MB_ModifyLogonPass));
		if (wDataSize != sizeof(DBR_MB_ModifyLogonPass)) return false;

		//������
		DBR_MB_ModifyLogonPass* pModifyLogonPass = (DBR_MB_ModifyLogonPass*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pModifyLogonPass->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pModifyLogonPass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"), pModifyLogonPass->szScrPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"), pModifyLogonPass->szDesPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//�������
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_ModifyLogonPassword"), false) == DB_SUCCESS)
		{
			//��������
			DBO_MB_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateSuccess.wRequestCmdID = SUB_MB_MODIFY_LOGON_PASS;
			OperateSuccess.lResultCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString),CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wSendSize);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_MODIFY_LOGON_PASS;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString),CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.wRequestCmdID = SUB_MB_MODIFY_LOGON_PASS;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"����ϵͳ��æ�����Ժ����ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//�޸�����
bool CDataBaseEngineSink::OnRequestMBModifyIndividual(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_MB_ModifyIndividual));
		if (wDataSize!=sizeof(DBR_MB_ModifyIndividual)) return false;

		//������
		DBR_MB_ModifyIndividual * pModifyIndividual=(DBR_MB_ModifyIndividual *)pData;

		//ת����ַ
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pModifyIndividual->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pModifyIndividual->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@cbGender"), pModifyIndividual->cbGender);
		m_AccountsDBAide.AddParameter(TEXT("@wInfoMask"), pModifyIndividual->wInfoMask);
		m_AccountsDBAide.AddParameter(TEXT("@strNickName"),pModifyIndividual->szNickName);
		m_AccountsDBAide.AddParameter(TEXT("@strUnderWrite"), pModifyIndividual->szUnderWrite);		
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribe,sizeof(szDescribe),adParamOutput);

		//�������
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_ModifyUserIndividual"),true)==DB_SUCCESS)
		{
			////////////////////////////////////////////////////////////////////////////////////////////////////
			//���²Ƹ�
			if (pModifyIndividual->wInfoMask & DTP_MB_NICKNAME)
			{
				//���²Ƹ�
				DBO_MB_WealthUpdate WealthUpdate;
				ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

				//���ñ���
				WealthUpdate.wItemCount = 1;
				WealthUpdate.WealthItem[0].wMoneyID = GOODS_ID_GOLD;
				WealthUpdate.WealthItem[0].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));

				//���ͽ��			
				WORD wHeadSize = sizeof(WealthUpdate) - sizeof(WealthUpdate.WealthItem);
				WORD wPacketSize = WealthUpdate.wItemCount * sizeof(WealthUpdate.WealthItem[0]) + wHeadSize;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_WEALTH_UPDATE, dwContextID, dwTokenID, &WealthUpdate, wPacketSize);
			}

			////////////////////////////////////////////////////////////////////////////////////////////////////
			//������Ϊ����
			DBO_MB_UserActionMask UserActionMask;
			ZeroMemory(&UserActionMask, sizeof(UserActionMask));

			//��ȡ����
			UserActionMask.dwActionMaskEver = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskEver"));
			UserActionMask.dwActionMaskPerDay = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskPerDay"));
			UserActionMask.dwActionMaskPerWeek = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskPerWeek"));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_ACTION_MASK, dwContextID, dwTokenID, &UserActionMask, sizeof(UserActionMask));

			////////////////////////////////////////////////////////////////////////////////////////////////////
			//������� 

			//��������
			DBO_MB_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateSuccess.wRequestCmdID = SUB_MB_MODIFY_INDIVIDUAL;
			OperateSuccess.lResultCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString,CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_SUCCESS,dwContextID, dwTokenID, &OperateSuccess,wSendSize);

			//����ṹ
			UserPublicInfo PublicInfo;
			PublicInfo.set_uid(pModifyIndividual->dwUserID);
			PublicInfo.set_gender(pModifyIndividual->cbGender);

			//�û��ǳ�
			if (pModifyIndividual->wInfoMask & DTP_MB_NICKNAME)
			{
				//ת������
				CHAR cbBuffer[128];
				int nStringLen = CWHService::Unicode2Utf8(pModifyIndividual->szNickName, cbBuffer, CountArray(cbBuffer));

				//�����ǳ�
				PublicInfo.set_nickname(cbBuffer, nStringLen);
			}

			//����ǩ��
			if (pModifyIndividual->wInfoMask & DTP_MB_UNDER_WRITE)
			{
				//ת������
				CHAR cbBuffer[256];
				int nStringLen = CWHService::Unicode2Utf8(pModifyIndividual->szUnderWrite, cbBuffer, CountArray(cbBuffer));

				//����ǩ��
				PublicInfo.set_underwrite(cbBuffer, nStringLen);
			}

			//������Ϣ
			UpdateUserCache(dwContextID, dwTokenID, PublicInfo);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_MODIFY_INDIVIDUAL;
			OperateFailure.lErrorCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString,CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE,dwContextID, dwTokenID, &OperateFailure,wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//�������
		OperateFailure.wRequestCmdID = SUB_MB_MODIFY_INDIVIDUAL;
		OperateFailure.lErrorCode=pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"����ϵͳ��æ�����Ժ����ԣ�"));

		//���ͽ��
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE,dwContextID, dwTokenID, &OperateFailure,wSendSize);

		return false;
	}

	return true;
}

//�޸�ͷ��
bool CDataBaseEngineSink::OnRequestMBModifySystemFace(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_ModifySystemFace));
		if (wDataSize != sizeof(DBR_MB_ModifySystemFace)) return false;

		//������
		DBR_MB_ModifySystemFace* pModifySystemFace = (DBR_MB_ModifySystemFace*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pModifySystemFace->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@wFaceID"), pModifySystemFace->wFaceID);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//�������
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_ModifySystemFace"), true) == DB_SUCCESS)
		{
			////////////////////////////////////////////////////////////////////////////////////////////////////
			//���²Ƹ�
			DBO_MB_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

			//���ñ���
			WealthUpdate.wItemCount = 1;
			WealthUpdate.WealthItem[0].wMoneyID = GOODS_ID_GOLD;
			WealthUpdate.WealthItem[0].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));

			//���ͽ��			
			WORD wHeadSize = sizeof(WealthUpdate) - sizeof(WealthUpdate.WealthItem);
			WORD wPacketSize = WealthUpdate.wItemCount * sizeof(WealthUpdate.WealthItem[0]) + wHeadSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_WEALTH_UPDATE, dwContextID, dwTokenID, &WealthUpdate, wPacketSize);

			////////////////////////////////////////////////////////////////////////////////////////////////////
			//������Ϊ����
			DBO_MB_UserActionMask UserActionMask;
			ZeroMemory(&UserActionMask, sizeof(UserActionMask));

			//��ȡ����
			UserActionMask.dwActionMaskEver = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskEver"));
			UserActionMask.dwActionMaskPerDay = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskPerDay"));
			UserActionMask.dwActionMaskPerWeek = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskPerWeek"));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_ACTION_MASK, dwContextID, dwTokenID, &UserActionMask, sizeof(UserActionMask));

			////////////////////////////////////////////////////////////////////////////////////////////////////
			//����ͷ��
			DBO_MB_UserFaceInfo UserFaceInfo;
			ZeroMemory(&UserFaceInfo, sizeof(UserFaceInfo));

			//��ȡ��Ϣ
			UserFaceInfo.wFaceID = m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			UserFaceInfo.dwCustomID = m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_FACE_INFO, dwContextID, dwTokenID, &UserFaceInfo, sizeof(UserFaceInfo));

			//����ṹ
			UserPublicInfo PublicInfo;
			PublicInfo.set_uid(pModifySystemFace->dwUserID);
			PublicInfo.set_faceid(UserFaceInfo.wFaceID);
			PublicInfo.set_customid(UserFaceInfo.dwCustomID);

			//������Ϣ
			UpdateUserCache(dwContextID, dwTokenID, PublicInfo);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_MODIFY_SYSTEM_FACE;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);


			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.wRequestCmdID = SUB_MB_MODIFY_SYSTEM_FACE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"����ϵͳ��æ�����Ժ����ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//�޸�ͷ��
bool CDataBaseEngineSink::OnRequestMBModifyCustomFace(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_ModifyCustomFace));
		if (wDataSize != sizeof(DBR_MB_ModifyCustomFace)) return false;

		//������
		DBR_MB_ModifyCustomFace* pModifyCustomFace = (DBR_MB_ModifyCustomFace*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pModifyCustomFace->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwCustomID"), pModifyCustomFace->dwCustomID);
		m_AccountsDBAide.AddParameter(TEXT("@strFacePath"), pModifyCustomFace->szFacePath);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//�������
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_ModifyCustomFace"), true) == DB_SUCCESS)
		{
			////////////////////////////////////////////////////////////////////////////////////////////////////
			//���²Ƹ�
			DBO_MB_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

			//���ñ���
			WealthUpdate.wItemCount = 1;
			WealthUpdate.WealthItem[0].wMoneyID = GOODS_ID_GOLD;
			WealthUpdate.WealthItem[0].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));

			//���ͽ��			
			WORD wHeadSize = sizeof(WealthUpdate) - sizeof(WealthUpdate.WealthItem);
			WORD wPacketSize = WealthUpdate.wItemCount * sizeof(WealthUpdate.WealthItem[0]) + wHeadSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_WEALTH_UPDATE, dwContextID, dwTokenID, &WealthUpdate, wPacketSize);

			////////////////////////////////////////////////////////////////////////////////////////////////////
			//������Ϊ����
			DBO_MB_UserActionMask UserActionMask;
			ZeroMemory(&UserActionMask, sizeof(UserActionMask));

			//��ȡ����
			UserActionMask.dwActionMaskEver = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskEver"));
			UserActionMask.dwActionMaskPerDay = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskPerDay"));
			UserActionMask.dwActionMaskPerWeek = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskPerWeek"));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_ACTION_MASK, dwContextID, dwTokenID, &UserActionMask, sizeof(UserActionMask));
			
			////////////////////////////////////////////////////////////////////////////////////////////////////
			//����ͷ��
			DBO_MB_UserFaceInfo UserFaceInfo;
			ZeroMemory(&UserFaceInfo, sizeof(UserFaceInfo));

			//��ȡ��Ϣ
			UserFaceInfo.wFaceID = m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			UserFaceInfo.dwCustomID = m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_FACE_INFO, dwContextID, dwTokenID, &UserFaceInfo, sizeof(UserFaceInfo));

			//����ṹ
			UserPublicInfo PublicInfo;
			PublicInfo.set_uid(pModifyCustomFace->dwUserID);
			PublicInfo.set_faceid(UserFaceInfo.wFaceID);
			PublicInfo.set_customid(UserFaceInfo.dwCustomID);

			//������Ϣ
			UpdateUserCache(dwContextID, dwTokenID, PublicInfo);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_MODIFY_CUSTOM_FACE;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.wRequestCmdID = SUB_MB_MODIFY_CUSTOM_FACE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"����ϵͳ��æ�����Ժ����ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//�һ���Ʒ
bool CDataBaseEngineSink::OnRequestMBExchangeMallGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_ExchangeMallGoods));
		if (wDataSize != sizeof(DBR_MB_ExchangeMallGoods)) return false;

		//������
		DBR_MB_ExchangeMallGoods* pExchangeMallGoods = (DBR_MB_ExchangeMallGoods*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pExchangeMallGoods->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwMallGoodsID"), pExchangeMallGoods->dwMallGoodsID);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_EXCHANGE_MALLGOODS);
		m_AccountsDBAide.AddParameter(TEXT("@dwTimeStamp"), time(NULL));

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//�������
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ExchangeMallGoods"), true) == DB_SUCCESS)
		{
			//������
			DBO_MB_ExchangeMallGoods ExchangeMallGoods;
			ZeroMemory(&ExchangeMallGoods, sizeof(ExchangeMallGoods));

			//����ṹ
			tagGoodsItem BuyGoodsItem;
			ZeroMemory(&BuyGoodsItem, sizeof(BuyGoodsItem));

			//��Ʒ��Ϣ
			BuyGoodsItem.wGoodsID = m_AccountsDBAide.GetValue_WORD(TEXT("IncGoodsID"));
			BuyGoodsItem.dwGoodsCount = m_AccountsDBAide.GetValue_DWORD(TEXT("IncGoodsCount"));
			BuyGoodsItem.dwGoodsIndate = m_AccountsDBAide.GetValue_DWORD(TEXT("IncGoodsIndate"));

			//��Ʒ��Ϣ
			ExchangeMallGoods.wIncGoodsID = BuyGoodsItem.wGoodsID;
			ExchangeMallGoods.dwIncGoodsCount = BuyGoodsItem.dwGoodsCount;
			ExchangeMallGoods.dwIncGoodsIndate = BuyGoodsItem.dwGoodsIndate;

			//��ʯ��Ϣ
			ExchangeMallGoods.lResultCode = DB_SUCCESS;			
			ExchangeMallGoods.wExpendMoneyID = m_AccountsDBAide.GetValue_WORD(TEXT("ExpendMoneyID"));
			ExchangeMallGoods.lMoneyBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("MoneyBalance"));			

			//���²Ƹ�
			UpdateUserWealth(pExchangeMallGoods->dwUserID, dwContextID, dwTokenID);

			//������Ʒ
			if (!g_GameGoodsManager->IsMoneyGoods(BuyGoodsItem.wGoodsID))
			{				
				UpdateUserGoods(pExchangeMallGoods->dwUserID, dwContextID, dwTokenID, &BuyGoodsItem, 1);
			}

			//Ͷ����Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_EXCHANGE_MALLGOODS, dwContextID, dwTokenID, &ExchangeMallGoods, sizeof(ExchangeMallGoods));
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������			
			OperateFailure.wRequestCmdID = SUB_MB_EXCHANGE_MALLGOODS;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.wRequestCmdID = SUB_MB_EXCHANGE_MALLGOODS;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"����ϵͳ��æ�����Ժ����ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//�һ������
bool CDataBaseEngineSink::OnRequestMBExchangeGiftbagCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_ExchangeGiftbagCode));
		if (wDataSize != sizeof(DBR_MB_ExchangeGiftbagCode)) return false;

		//������
		DBR_MB_ExchangeGiftbagCode* pExchangeGiftbagCode = (DBR_MB_ExchangeGiftbagCode*)pData;

		//ת����ַ
		TCHAR szClientAddr[16] = TEXT("");
		BYTE* pClientAddr = (BYTE*)&pExchangeGiftbagCode->dwClientIP;
		_sntprintf_s(szClientAddr, CountArray(szClientAddr), TEXT("%d.%d.%d.%d"), pClientAddr[0], pClientAddr[1], pClientAddr[2], pClientAddr[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pExchangeGiftbagCode->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_EXCHANGE_GIFTBAGCODE);
		m_AccountsDBAide.AddParameter(TEXT("@strGiftbagCode"), pExchangeGiftbagCode->szGiftbagCode);
		m_AccountsDBAide.AddParameter(TEXT("@strClientAddr"), szClientAddr);
		
		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//�������
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ExchangeGiftbagCode"), true) == DB_SUCCESS)
		{
			//������
			DBO_MB_ExchangeGiftbagCode ExchangeGiftbagCode;
			ZeroMemory(&ExchangeGiftbagCode, sizeof(ExchangeGiftbagCode));

			//��ʯ��Ϣ
			ExchangeGiftbagCode.lResultCode = DB_SUCCESS;
			m_AccountsDBAide.GetValue_String(TEXT("GiftBagGoodsList"), ExchangeGiftbagCode.szGiftBagGoodsList, CountArray(ExchangeGiftbagCode.szGiftBagGoodsList));

			//������Ʒ
			CGoodsItemArray GoodsArray, MoneyArray;
			g_GameGoodsManager->ParseFromJson(ExchangeGiftbagCode.szGiftBagGoodsList, MoneyArray, GoodsArray);

			//���²Ƹ�
			if (MoneyArray.GetCount() > 0)
			{
				UpdateUserWealth(pExchangeGiftbagCode->dwUserID, dwContextID, dwTokenID);
			}			

			//������Ʒ
			if (GoodsArray.GetCount())
			{
				UpdateUserGoods(pExchangeGiftbagCode->dwUserID, dwContextID, dwTokenID, GoodsArray.GetData(), GoodsArray.GetCount());
			}

			//Ͷ����Ϣ
			WORD wStringSize = CountStringBuffer(ExchangeGiftbagCode.szDescribeString);
			WORD wSendSize = sizeof(ExchangeGiftbagCode) - sizeof(ExchangeGiftbagCode.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_EXCHANGE_GIFTBAGCODE, dwContextID, dwTokenID, &ExchangeGiftbagCode, wSendSize);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������			
			OperateFailure.wRequestCmdID = SUB_MB_EXCHANGE_GIFTBAGCODE;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.wRequestCmdID = SUB_MB_EXCHANGE_GIFTBAGCODE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"����ϵͳ��æ�����Ժ����ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//������
bool CDataBaseEngineSink::OnRequestMBBindEMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_BindEMail));
		if (wDataSize != sizeof(DBR_MB_BindEMail)) return false;

		//������
		DBR_MB_BindEMail* pBindEMail = (DBR_MB_BindEMail*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pBindEMail->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pBindEMail->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pBindEMail->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strNewPassword"), pBindEMail->szNewPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strMachine"), pBindEMail->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strEMail"), pBindEMail->szEMail);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCodeMD5"), pBindEMail->szAuthCode);
		m_AccountsDBAide.AddParameter(TEXT("@dwUnbindUserID"), pBindEMail->dwUnbindUserID);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_BindEMail"), true);

		//��ȡ��Ϣ
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_MB_BindEMailSuccess BindSuccess;
			ZeroMemory(&BindSuccess, sizeof(BindSuccess));

			//�󶨺���
			StringCchCopy(BindSuccess.szEMail, CountArray(BindSuccess.szEMail), pBindEMail->szEMail);

			//�Ƹ���Ϣ
			BindSuccess.lUserCard = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserCard"));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������			
			StringCchCopy(BindSuccess.szNoticeString, CountArray(BindSuccess.szNoticeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(BindSuccess.szNoticeString);
			WORD wPacketSize = sizeof(BindSuccess) - sizeof(BindSuccess.szNoticeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_BIND_EMAIL_SUCCESS, dwContextID, dwTokenID, &BindSuccess, wPacketSize);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_BIND_EMAIL;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = SUB_MB_BIND_EMAIL;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//�������
bool CDataBaseEngineSink::OnRequestMBUnbindEMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_UnbindEMail));
		if (wDataSize != sizeof(DBR_MB_UnbindEMail)) return false;

		//������
		DBR_MB_UnbindEMail* pUnBindEMail = (DBR_MB_UnbindEMail*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pUnBindEMail->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUnBindEMail->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pUnBindEMail->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strMachine"), pUnBindEMail->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strEMail"), pUnBindEMail->szEMail);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCodeMD5"), pUnBindEMail->szAuthCode);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_UnBindEMail"), true);

		//��ȡ��Ϣ
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_MB_UnbindEMailSuccess UnBindMPSuccess;
			ZeroMemory(&UnBindMPSuccess, sizeof(UnBindMPSuccess));

			//�Ƹ���Ϣ
			UnBindMPSuccess.lUserCard = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserCard"));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������			
			StringCchCopy(UnBindMPSuccess.szNoticeString, CountArray(UnBindMPSuccess.szNoticeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(UnBindMPSuccess.szNoticeString);
			WORD wPacketSize = sizeof(UnBindMPSuccess) - sizeof(UnBindMPSuccess.szNoticeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_UNBIND_EMAIL_SUCCESS, dwContextID, dwTokenID, &UnBindMPSuccess, wPacketSize);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_UNBIND_EMAIL;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = SUB_MB_UNBIND_EMAIL;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//��ȡ��֤��
bool CDataBaseEngineSink::OnRequestMBAcquireAuthCode(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(DBR_MB_AcquireAuthCode));
	if (wDataSize != sizeof(DBR_MB_AcquireAuthCode)) return false;

	//������
	DBR_MB_AcquireAuthCode* pAcquireAuthCode = (DBR_MB_AcquireAuthCode*)pData;

	try
	{
		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE * pClientIP = (BYTE *)&pAcquireAuthCode->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pAcquireAuthCode->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pAcquireAuthCode->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strMachine"), pAcquireAuthCode->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@cbCodeKind"), pAcquireAuthCode->cbCodeKind);
		m_AccountsDBAide.AddParameter(TEXT("@cbAuthKind"), pAcquireAuthCode->cbAuthKind);
		m_AccountsDBAide.AddParameter(TEXT("@dwBindUserID"), pAcquireAuthCode->dwBindUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strMobileEmail"), pAcquireAuthCode->szMobileEmail);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCode"), pAcquireAuthCode->szAuthCode);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCodeMD5"), pAcquireAuthCode->szAuthCodeMD5);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_AcquireAuthCode"), true);

		//����ṹ
		DBO_MB_AcquireACResult AcquireACResult;
		ZeroMemory(&AcquireACResult, sizeof(AcquireACResult));

		//��֤����
		AcquireACResult.cbCodeKind = pAcquireAuthCode->cbCodeKind;
		AcquireACResult.cbAuthKind = pAcquireAuthCode->cbAuthKind;
		StringCchCopy(AcquireACResult.szAuthCode, CountArray(AcquireACResult.szAuthCode), pAcquireAuthCode->szAuthCode);
		StringCchCopy(AcquireACResult.szMobileEmail, CountArray(AcquireACResult.szMobileEmail), pAcquireAuthCode->szMobileEmail);

		//��ȡ��Ϣ
		CDBVarValue DBVarValue;
		m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

		//�����Ϣ
		AcquireACResult.lResultCode = lReturnValue;
		if (DBVarValue.bstrVal)
		{
			StringCchCopy(AcquireACResult.szNoticeString, CountArray(AcquireACResult.szNoticeString), CW2CT(DBVarValue.bstrVal).m_psz);
		}

		//�ֻ����Ѱ�
		if (AcquireACResult.lResultCode == AUTHECODE_MP_BINDED)
		{
			AcquireACResult.dwBindUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("BindUserID"));
		}

		//��֤���ѷ���
		if (AcquireACResult.lResultCode == AUTHECODE_AC_HAVESENT)
		{
			AcquireACResult.dwBindUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("BindUserID"));
		}

		//��������
		if (AcquireACResult.lResultCode == DB_SUCCESS && AcquireACResult.cbAuthKind == AUTH_KIND_RESETPASSWD)
		{
			AcquireACResult.dwBindUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("BindUserID"));
		}

		//������Ϣ
		WORD wStringSize = CountStringBuffer(AcquireACResult.szNoticeString);
		WORD wPacketSize = sizeof(AcquireACResult) - sizeof(AcquireACResult.szNoticeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_ACQUIREAC_RESULT, dwContextID, dwTokenID, &AcquireACResult, wPacketSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_MB_AcquireACResult AcquireACResult;
		ZeroMemory(&AcquireACResult, sizeof(AcquireACResult));

		//��֤����
		AcquireACResult.cbAuthKind = pAcquireAuthCode->cbAuthKind;

		//�����Ϣ
		AcquireACResult.lResultCode = AUTHECODE_DB_EXCEPTION;
		StringCchCopy(AcquireACResult.szNoticeString, CountArray(AcquireACResult.szNoticeString), CWHService::FormatMessage(MIM_LOGON_0006, TEXT("�������ݿ��쳣�����Ժ��ٴγ��ԣ�")));

		//������Ϣ
		WORD wStringSize = CountStringBuffer(AcquireACResult.szNoticeString);
		WORD wPacketSize = sizeof(AcquireACResult) - sizeof(AcquireACResult.szNoticeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_ACQUIREAC_RESULT, dwContextID, dwTokenID, &AcquireACResult, wPacketSize);

		return false;
	}

	return true;
}

//���ֻ���
bool CDataBaseEngineSink::OnRequestMBBindMobilePhone(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_BindMobilePhone));
		if (wDataSize != sizeof(DBR_MB_BindMobilePhone)) return false;

		//������
		DBR_MB_BindMobilePhone* pBindMobilePhone = (DBR_MB_BindMobilePhone*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE * pClientIP = (BYTE *)&pBindMobilePhone->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pBindMobilePhone->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pBindMobilePhone->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strNewPassword"), pBindMobilePhone->szNewPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strMachine"), pBindMobilePhone->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pBindMobilePhone->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCodeMD5"), pBindMobilePhone->szAuthCode);
		m_AccountsDBAide.AddParameter(TEXT("@dwUnbindUserID"), pBindMobilePhone->dwUnbindUserID);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_BindMobilePhone"), true);

		//��ȡ��Ϣ
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_MB_BindMPSuccess BindMPSuccess;
			ZeroMemory(&BindMPSuccess, sizeof(BindMPSuccess));

			//�󶨺���
			StringCchCopy(BindMPSuccess.szMobilePhone, CountArray(BindMPSuccess.szMobilePhone), pBindMobilePhone->szMobilePhone);

			//�Ƹ���Ϣ
			BindMPSuccess.lUserCard = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserCard"));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������			
			StringCchCopy(BindMPSuccess.szNoticeString, CountArray(BindMPSuccess.szNoticeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(BindMPSuccess.szNoticeString);
			WORD wPacketSize = sizeof(BindMPSuccess) - sizeof(BindMPSuccess.szNoticeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_BINDMP_SUCCESS, dwContextID, dwTokenID, &BindMPSuccess, wPacketSize);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_BIND_MOBILEPHONE;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = SUB_MB_BIND_MOBILEPHONE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//����ֻ���
bool CDataBaseEngineSink::OnRequestMBUnbindMobilePhone(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT (wDataSize == sizeof(DBR_MB_UnbindMobilePhone));
		if (wDataSize != sizeof(DBR_MB_UnbindMobilePhone)) return false;

		//������
		DBR_MB_UnbindMobilePhone* pUnBindMobilePhone = (DBR_MB_UnbindMobilePhone*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE * pClientIP = (BYTE *)&pUnBindMobilePhone->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUnBindMobilePhone->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pUnBindMobilePhone->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strMachine"), pUnBindMobilePhone->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strMobilePhone"), pUnBindMobilePhone->szMobilePhone);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCodeMD5"), pUnBindMobilePhone->szAuthCode);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_UnBindMobilePhone"), true);

		//��ȡ��Ϣ
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_MB_UnbindMPSuccess UnBindMPSuccess;
			ZeroMemory(&UnBindMPSuccess, sizeof(UnBindMPSuccess));

			//�Ƹ���Ϣ
			UnBindMPSuccess.lUserCard = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserCard"));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������			
			StringCchCopy(UnBindMPSuccess.szNoticeString, CountArray(UnBindMPSuccess.szNoticeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(UnBindMPSuccess.szNoticeString);
			WORD wPacketSize = sizeof(UnBindMPSuccess) - sizeof(UnBindMPSuccess.szNoticeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_UNBINDMP_SUCCESS, dwContextID, dwTokenID, &UnBindMPSuccess, wPacketSize);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_UNBIND_MOBILEPHONE;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = SUB_MB_UNBIND_MOBILEPHONE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestMBResetLogonPasswd(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_ResetLogonPasswd));
		if (wDataSize != sizeof(DBR_MB_ResetLogonPasswd)) return false;

		//������
		DBR_MB_ResetLogonPasswd* pResetLogonPasswd = (DBR_MB_ResetLogonPasswd*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwBindUserID"), pResetLogonPasswd->dwBindUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pResetLogonPasswd->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strMachine"), pResetLogonPasswd->szMachineID);
		m_AccountsDBAide.AddParameter(TEXT("@cbCodeKind"), pResetLogonPasswd->cbCodeKind);
		m_AccountsDBAide.AddParameter(TEXT("@strMobileEmail"), pResetLogonPasswd->szMobileEmail);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCodeMD5"), pResetLogonPasswd->szAuthCode);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_ResetLogonPasswd"), false);

		//��ȡ��Ϣ
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_MB_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateSuccess.lResultCode = 0;
			OperateSuccess.wRequestCmdID = SUB_MB_RESET_LOGON_PASS;
			StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wPacketSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wPacketSize);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_RESET_LOGON_PASS;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wPacketSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = SUB_MB_RESET_LOGON_PASS;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wPacketSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);

		return false;
	}

	return true;
}


//����ǩ��
bool CDataBaseEngineSink::OnRequestMBRequestCheckIn(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_RequestCheckIn));
		if (wDataSize != sizeof(DBR_MB_RequestCheckIn)) return false;

		//��ȡ����
		auto pRequestCheckIn = (DBR_MB_RequestCheckIn*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pRequestCheckIn->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pRequestCheckIn->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@cbQuery"), pRequestCheckIn->bQuery);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_EfficacyCheckIn"), true);

		//��ȡ��Ϣ
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_MB_CheckInResult CheckInResult;
			ZeroMemory(&CheckInResult, sizeof(CheckInResult));

			//ǩ����Ϣ
			CheckInResult.dwUserID = pRequestCheckIn->dwUserID;
			CheckInResult.wCheckInDays = m_AccountsDBAide.GetValue_WORD(TEXT("CheckInDays"));
			CheckInResult.cbCheckInState = m_AccountsDBAide.GetValue_BYTE(TEXT("CheckInState"));

			//�������
			tagGoodsItem GoodsItem;
			ZeroMemory(&GoodsItem, sizeof(GoodsItem));
			GoodsItem.wGoodsID = m_AccountsDBAide.GetValue_LONG(TEXT("RewardGoodsID"));
			GoodsItem.dwGoodsCount = m_AccountsDBAide.GetValue_LONG(TEXT("RewardAmount"));

			//���л�
			g_GameGoodsManager->SerializeToJson(&GoodsItem, 1, CheckInResult.szRewardGoodsList, CountArray(CheckInResult.szRewardGoodsList));

			//���ͽ��
			WORD wStringSize = CountStringBuffer(CheckInResult.szRewardGoodsList);
			WORD wPacketSize = sizeof(CheckInResult) - sizeof(CheckInResult.szRewardGoodsList) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_CHECKIN_RESULT, dwContextID, dwTokenID, &CheckInResult, wPacketSize);

			//���²Ƹ�
			UpdateUserWealth(CheckInResult.dwUserID, dwContextID, dwTokenID);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_BIND_INVITE_CODE;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = SUB_MB_BIND_INVITE_CODE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestMBBindInviteCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
    try
    {
        //Ч�����
        ASSERT(wDataSize == sizeof(DBR_MB_BindInviteCode));
        if (wDataSize != sizeof(DBR_MB_BindInviteCode)) return false;

        //������
		DBR_MB_BindInviteCode* pBindInviteCode = (DBR_MB_BindInviteCode*)pData;

        //ת����ַ
        TCHAR szClientIP[16] = TEXT("");
        BYTE* pClientIP = (BYTE*)&pBindInviteCode->dwClientIP;
        _sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

        //�������
        m_AccountsDBAide.ResetParameter();
        m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pBindInviteCode->dwUserID);
        m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pBindInviteCode->szPassword);
        m_AccountsDBAide.AddParameter(TEXT("@dwInviteCode"), pBindInviteCode->dwInviteCode);
		m_AccountsDBAide.AddParameter(TEXT("@strMachine"), pBindInviteCode->szMachineID);
        m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);

        //�������
        TCHAR szDescribe[128] = TEXT("");
        m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

        //ִ�в�ѯ
        LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_BindInviteCode"), true);

        //��ȡ��Ϣ
        if (lReturnValue == DB_SUCCESS)
        {
            //��������
			DBO_MB_BindInviteCode BindInviteCode;
            ZeroMemory(&BindInviteCode, sizeof(BindInviteCode));

			BindInviteCode.dwUserID = pBindInviteCode->dwUserID;
			BindInviteCode.dwParentGameID = m_AccountsDBAide.GetValue_DWORD(TEXT("ParentGameID"));
			BindInviteCode.wParentFaceID = m_AccountsDBAide.GetValue_WORD(TEXT("ParentFaceID"));
			BindInviteCode.dwMailID = m_AccountsDBAide.GetValue_DWORD(TEXT("MailID"));
			m_AccountsDBAide.GetValue_String(TEXT("ParentNickName"), BindInviteCode.szParentNickName, CountArray(BindInviteCode.szParentNickName));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������			
			StringCchCopy(BindInviteCode.szNoticeString, CountArray(BindInviteCode.szNoticeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(BindInviteCode.szNoticeString);
			WORD wPacketSize = sizeof(BindInviteCode) - sizeof(BindInviteCode.szNoticeString) + wStringSize;
            m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_BIND_INVITE_CODE, dwContextID, dwTokenID, &BindInviteCode, wPacketSize);
        }
        else
        {
            //��������
            DBO_MB_OperateFailure OperateFailure;
            ZeroMemory(&OperateFailure, sizeof(OperateFailure));

            //��ȡ��Ϣ
            CDBVarValue DBVarValue;
            m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

            //�������
            OperateFailure.wRequestCmdID = SUB_MB_BIND_INVITE_CODE;
            OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
            StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

            //���ͽ��
            WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
            WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
            m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
        }

        return true;
    }
    catch (IDataBaseException* pIException)
    {
        //�������
        LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
        TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

        //��������
        DBO_MB_OperateFailure OperateFailure;
        ZeroMemory(&OperateFailure, sizeof(OperateFailure));

        //���ñ���
        OperateFailure.wRequestCmdID = SUB_MB_BIND_INVITE_CODE;
        OperateFailure.lErrorCode = pIException->GetExceptionResult();
        StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

        //���ͽ��
        WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
        WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
        m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

        return false;
    }

    return true;
}

//�ύ����
bool CDataBaseEngineSink::OnRequestMBSubmitInformInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize <= sizeof(DBR_MB_SubmitInformInfo));
		if (wDataSize > sizeof(DBR_MB_SubmitInformInfo)) return false;

		//������
		auto pSubmitInformInfo = (DBR_MB_SubmitInformInfo*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pSubmitInformInfo->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
        m_AccountsDBAide.AddParameter(TEXT("@cbInformKind"), pSubmitInformInfo->cbInformKind);
        m_AccountsDBAide.AddParameter(TEXT("@cbInformSource"), pSubmitInformInfo->cbInformSource);
		m_AccountsDBAide.AddParameter(TEXT("@dwInformUserID"), pSubmitInformInfo->dwInformUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwBeInformUserID"), pSubmitInformInfo->dwBeInformUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strAttachmentUrl"), pSubmitInformInfo->szAttachmentUrl);
        m_AccountsDBAide.AddParameter(TEXT("@strInformMessage"), pSubmitInformInfo->szInformMessage);
        m_AccountsDBAide.AddParameter(TEXT("@strInformContent"), pSubmitInformInfo->szInformContent);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		
		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_SubmitInformInfo"), false);

		//��ȡ��Ϣ
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_MB_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//�������
			OperateSuccess.lResultCode = lReturnValue;
			OperateSuccess.wRequestCmdID = SUB_MB_SUBMIT_INFORM_INFO;						

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wSendSize);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.lErrorCode = lReturnValue;
			OperateFailure.wRequestCmdID = SUB_MB_SUBMIT_INFORM_INFO;			
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = SUB_MB_SUBMIT_INFORM_INFO;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//���˺�
bool CDataBaseEngineSink::OnRequestMBBindExternalAccount(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize >= sizeof(DBR_MB_BindExternalAccount));
		if (wDataSize < sizeof(DBR_MB_BindExternalAccount)) return false;

		//������
		auto pBindExternalAccount = (DBR_MB_BindExternalAccount*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pBindExternalAccount->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pBindExternalAccount->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwExternalID"), pBindExternalAccount->dwExternalID);
		m_AccountsDBAide.AddParameter(TEXT("@strUniqueID"), pBindExternalAccount->szUniqueID);
		m_AccountsDBAide.AddParameter(TEXT("@strFacePath"), pBindExternalAccount->szUserFaceUrl);
		m_AccountsDBAide.AddParameter(TEXT("@strUserDatum"), pBindExternalAccount->szUserDatum);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_BindExternalAccounts"), true);

		//��ȡ��Ϣ
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_MB_BindExternalAccount BindExternalAccount;
			ZeroMemory(&BindExternalAccount, sizeof(BindExternalAccount));

			//���ñ���
			BindExternalAccount.dwExternalID = pBindExternalAccount->dwExternalID;
			StringCchCopy(BindExternalAccount.szUniqueID,CountArray(BindExternalAccount.szUniqueID), pBindExternalAccount->szUniqueID);

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������			
			StringCchCopy(BindExternalAccount.szNoticeString, CountArray(BindExternalAccount.szNoticeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(BindExternalAccount.szNoticeString);
			WORD wPacketSize = sizeof(BindExternalAccount) - sizeof(BindExternalAccount.szNoticeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_BIND_EXTERNAL_ACCOUNT, dwContextID, dwTokenID, &BindExternalAccount, wPacketSize);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_BIND_EXTERNAL_ACCOUNT;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = SUB_MB_BIND_EXTERNAL_ACCOUNT;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//����ת��
bool CDataBaseEngineSink::OnRequestMBWealthTranser(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_WealthTransfer));
		if (wDataSize != sizeof(DBR_MB_WealthTransfer)) return false;

		//������
		auto pWealthTransfer = (DBR_MB_WealthTransfer*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pWealthTransfer->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pWealthTransfer->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwTargetGameID"), pWealthTransfer->dwTargetGameID);
		m_AccountsDBAide.AddParameter(TEXT("@cbCodeKind"), pWealthTransfer->cbCodeKind);
		m_AccountsDBAide.AddParameter(TEXT("@strAuthCode"), pWealthTransfer->szAuthCode);
		m_AccountsDBAide.AddParameter(TEXT("@lTransferAmount"), pWealthTransfer->lTransferAmount);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_WEALTH_TRANSFER);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pWealthTransfer->szMachineID);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_WealthTransfer"), true);

		//��ȡ��Ϣ
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_MB_WealthTransferSuccess WealthTransferSuccess;
			ZeroMemory(&WealthTransferSuccess,sizeof(WealthTransferSuccess));

			//���ñ���
			WealthTransferSuccess.dwUserID = pWealthTransfer->dwUserID;
			WealthTransferSuccess.dwMailID = m_AccountsDBAide.GetValue_DWORD(TEXT("MailID"));
			WealthTransferSuccess.dwTargetUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("TargetUserID"));

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_WEALTH_TRANSFER_SUCCESS, dwContextID, dwTokenID, &WealthTransferSuccess, sizeof(WealthTransferSuccess));

			//���²Ƹ�
			UpdateUserWealth(pWealthTransfer->dwUserID, dwContextID, dwTokenID);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_WEALTH_TRANSFER;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = SUB_MB_WEALTH_TRANSFER;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//ȡ��Ӷ��
bool CDataBaseEngineSink::OnRequestMBDrawCommission(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_DrawCommission));
		if (wDataSize != sizeof(DBR_MB_DrawCommission)) return false;

		//������
		auto pDrawCommission = (DBR_MB_DrawCommission*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pDrawCommission->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pDrawCommission->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@lDrawAmount"), pDrawCommission->lDrawAmount);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_DRAW_COMMISSION);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pDrawCommission->szMachineID);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_DrawCommission"), true);

		//��ȡ��Ϣ
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_MB_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateSuccess.wRequestCmdID = SUB_MB_DRAW_COMMISSION;
			OperateSuccess.lResultCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wSendSize);

			//���²Ƹ�
			UpdateUserWealth(pDrawCommission->dwUserID, dwContextID, dwTokenID);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_DRAW_COMMISSION;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = SUB_MB_DRAW_COMMISSION;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestMBSendGift(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
    try
    {
        //Ч�����
        ASSERT(wDataSize >= sizeof(DBR_MB_SendGift));
        if (wDataSize < sizeof(DBR_MB_SendGift)) return false;

        //������
        auto pSendGift = (DBR_MB_SendGift*)pData;

        //ת����ַ
        TCHAR szClientIP[16] = TEXT("");
        BYTE* pClientIP = (BYTE*)&pSendGift->dwClientIP;
        _sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

        //�������
        m_AccountsDBAide.ResetParameter();
        m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pSendGift->dwUserID);
        m_AccountsDBAide.AddParameter(TEXT("@dwTargetUserID"), pSendGift->dwTargetUserID);
        m_AccountsDBAide.AddParameter(TEXT("@dwGoodsID"), pSendGift->dwGoodsID);
        m_AccountsDBAide.AddParameter(TEXT("@dwGoodsCount"), pSendGift->dwGoodsCount);
        m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);

        //�������
        TCHAR szDescribe[128] = TEXT("");
        m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

        //ִ�в�ѯ
        LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_SendGift"), true);

        //��ȡ��Ϣ
        if (lReturnValue == DB_SUCCESS)
        {
            //��������
			DBO_MB_SendGiftSuccess OperateSuccess;
            ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

            //�������
            OperateSuccess.dwUserID = pSendGift->dwUserID;
            OperateSuccess.dwTargetUserID = pSendGift->dwTargetUserID;
            OperateSuccess.dwGoodsID = pSendGift->dwGoodsID;
            OperateSuccess.dwGoodsCount = pSendGift->dwGoodsCount;
            OperateSuccess.dwRecordID = m_AccountsDBAide.GetValue_DWORD(TEXT("RecordID"));
            OperateSuccess.dwLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));
            OperateSuccess.dwMyLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("MyLoveLiness"));
            OperateSuccess.dwTargetLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("TargetLoveLiness"));
            OperateSuccess.cbSendType = m_AccountsDBAide.GetValue_BYTE(TEXT("SendType"));

            //���²Ƹ�
            UpdateUserWealth(pSendGift->dwUserID, dwContextID, dwTokenID);

			//����ṹ
			UserPublicInfo PublicInfo;
			PublicInfo.set_uid(pSendGift->dwUserID);
			PublicInfo.set_loveliness(OperateSuccess.dwMyLoveLiness);

			//������Ϣ
			UpdateUserCache(dwContextID, dwTokenID, PublicInfo);

            //���ͽ��
            m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_SEND_GIFT_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, sizeof(OperateSuccess));
        }
        else
        {
            //��������
            DBO_MB_OperateFailure OperateFailure;
            ZeroMemory(&OperateFailure, sizeof(OperateFailure));

            //��ȡ��Ϣ
            CDBVarValue DBVarValue;
            m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

            //�������
            OperateFailure.wRequestCmdID = SUB_MB_SEND_GIFT;
            OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
            StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

            //���ͽ��
            WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
            WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
            m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
        }

        return true;
    }
    catch (IDataBaseException* pIException)
    {
        //�������
        LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
        TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

        //��������
        DBO_MB_OperateFailure OperateFailure;
        ZeroMemory(&OperateFailure, sizeof(OperateFailure));

        //���ñ���
        OperateFailure.wRequestCmdID = SUB_MB_SEND_GIFT;
        OperateFailure.lErrorCode = pIException->GetExceptionResult();
        StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

        //���ͽ��
        WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
        WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
        m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

        return false;
    }

    return true;
}

//��������
bool CDataBaseEngineSink::OnRequestMBLoadGift(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
    try
    {
        //Ч�����
        ASSERT(wDataSize >= sizeof(DBR_MB_LoadGift));
        if (wDataSize < sizeof(DBR_MB_LoadGift)) return false;

        //������
        auto pLoadGift = (DBR_MB_LoadGift*)pData;

        //�������
        m_AccountsDBAide.ResetParameter();
        m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pLoadGift->dwUserID);
        //m_AccountsDBAide.AddParameter(TEXT("@dwRecordID"), pLoadGift->dwRecordID);

        //�������
        TCHAR szDescribe[128] = TEXT("");
        m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

        //ִ�в�ѯ
        LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadGift"), true);

        //��ȡ��Ϣ
        if (lReturnValue == DB_SUCCESS)
        {
            //�������
			DBO_MB_UserGiftInfo UserGiftInfo;
            ZeroMemory(&UserGiftInfo, sizeof(UserGiftInfo));

            //��������
            while (m_AccountsDBModule->IsRecordsetEnd() == false)
            {
                if (UserGiftInfo.wItemCount >= CountArray(UserGiftInfo.GiftInfo))
                {
                    //��������
                    WORD wHeadSize = sizeof(UserGiftInfo) - sizeof(UserGiftInfo.GiftInfo);
                    WORD wDataSize = UserGiftInfo.wItemCount * sizeof(UserGiftInfo.GiftInfo[0]);
                    m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_GIFT_INFO, dwContextID, dwTokenID, &UserGiftInfo, wHeadSize + wDataSize);

                    //��������
					UserGiftInfo.wItemCount = 0;
                }

                //������Ϣ
                tagGiftInfo* pGiftInfo = &UserGiftInfo.GiftInfo[UserGiftInfo.wItemCount++];

                //��ȡ����
                pGiftInfo->dwRecordID = m_AccountsDBAide.GetValue_DWORD(TEXT("RecordID"));
                pGiftInfo->dwUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
                pGiftInfo->dwGoodsID = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsID"));
                pGiftInfo->dwGoodsCount = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsCount"));
                pGiftInfo->dwLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));
                m_AccountsDBAide.GetValue_String(TEXT("NickName"), pGiftInfo->szNickName, CountArray(pGiftInfo->szNickName));
                m_AccountsDBAide.GetValue_SystemTime(TEXT("CollectDateTime"), pGiftInfo->SendDate);

                //�ƶ���¼
                m_AccountsDBModule->MoveToNext();
            }

            //��������
            WORD wHeadSize = sizeof(UserGiftInfo) - sizeof(UserGiftInfo.GiftInfo);
            WORD wDataSize = UserGiftInfo.wItemCount * sizeof(UserGiftInfo.GiftInfo[0]);
            m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_GIFT_INFO, dwContextID, dwTokenID, &UserGiftInfo, wHeadSize + wDataSize);
        }
		
        //������Ϣ���
        m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_GIFT_INFO_FINISH, dwContextID, dwTokenID, NULL, NULL);

        return true;
    }
    catch (IDataBaseException* pIException)
    {
        //�������
        LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
        TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

        //��������
        DBO_MB_OperateFailure OperateFailure;
        ZeroMemory(&OperateFailure, sizeof(OperateFailure));

        //���ñ���
        OperateFailure.wRequestCmdID = SUB_MB_LOAD_GIFT;
        OperateFailure.lErrorCode = pIException->GetExceptionResult();
        StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

        //���ͽ��
        WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
        WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
        m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

        return false;
    }

    return true;
}

//����״̬����
bool CDataBaseEngineSink::OnRequestMBGiftStatusUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
    try
    {
        //Ч�����
        ASSERT(wDataSize >= sizeof(DBR_MB_GiftStatusUpdata));
        if (wDataSize < sizeof(DBR_MB_GiftStatusUpdata)) return false;

        //������
        auto pGiftStatusUpdata = (DBR_MB_GiftStatusUpdata*)pData;

        //�������
        m_AccountsDBAide.ResetParameter();
        m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pGiftStatusUpdata->dwUserID);
        m_AccountsDBAide.AddParameter(TEXT("@dwRecordID"), pGiftStatusUpdata->dwRecordID);

        //�������
        TCHAR szDescribe[128] = TEXT("");
        m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

        //ִ�в�ѯ
        LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_GiftStatusUpdate"), true);

        return true;
    }
    catch (IDataBaseException* pIException)
    {
        //�������
        LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
        TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

        //��������
        DBO_MB_OperateFailure OperateFailure;
        ZeroMemory(&OperateFailure, sizeof(OperateFailure));

        //���ñ���
        OperateFailure.wRequestCmdID = SUB_MB_GIFT_STATUS_UPDATE;
        OperateFailure.lErrorCode = pIException->GetExceptionResult();
        StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

        //���ͽ��
        WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
        WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
        m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

        return false;
    }

    return true;
}

//��ѯ�Ƹ�
bool CDataBaseEngineSink::OnRequestMBQueryWealth(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_MB_QueryWealth));
		if (wDataSize!=sizeof(DBR_MB_QueryWealth)) return false;

		//������
		DBR_MB_QueryWealth* pQueryWealth=(DBR_MB_QueryWealth*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pQueryWealth->dwUserID);

		//ִ�в�ѯ
		LONG lReturnValue=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryGameWealth"),true);

		//��ȡ��Ϣ
		if (lReturnValue==DB_SUCCESS)
		{
			//���²Ƹ�
			DBO_MB_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

			//���ñ���
			WealthUpdate.wItemCount = 2;
			WealthUpdate.WealthItem[0].wMoneyID = GOODS_ID_CARD;
			WealthUpdate.WealthItem[0].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserCard"));
			WealthUpdate.WealthItem[1].wMoneyID = GOODS_ID_GOLD;
			WealthUpdate.WealthItem[1].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));

			//���ͽ��			
			WORD wHeadSize = sizeof(WealthUpdate) - sizeof(WealthUpdate.WealthItem);
			WORD wPacketSize = WealthUpdate.wItemCount * sizeof(WealthUpdate.WealthItem[0]) + wHeadSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_WEALTH_UPDATE, dwContextID, dwTokenID, &WealthUpdate, wPacketSize);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_QUERY_WEALTH;
			OperateFailure.lErrorCode=m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE,dwContextID, dwTokenID, &OperateFailure,wSendSize);
		}
		
		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = SUB_MB_QUERY_WEALTH;
		OperateFailure.lErrorCode=pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE,dwContextID, dwTokenID, &OperateFailure,wSendSize);

		return false;
	}

	return true;
}

//��ѯ��λ
bool CDataBaseEngineSink::OnRequestMBQueryRankingInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_QueryRankingInfo));
		if (wDataSize != sizeof(DBR_MB_QueryRankingInfo)) return false;

		//������
		DBR_MB_QueryRankingInfo* pQueryRankingInfo = (DBR_MB_QueryRankingInfo*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pQueryRankingInfo->dwUserID);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadUserRankingInfo"), true);

		//��ȡ��Ϣ
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			tagUserRankingInfo UserRankingInfo;
			ZeroMemory(&UserRankingInfo,sizeof(UserRankingInfo));

			//����ṹ
			UserRankingInfo.dwRankID = m_AccountsDBAide.GetValue_DWORD(TEXT("RankID"));
			UserRankingInfo.wSeasonID = m_AccountsDBAide.GetValue_WORD(TEXT("SeasonID"));
			UserRankingInfo.lWinCount = m_AccountsDBAide.GetValue_LONG(TEXT("WinCount"));
			UserRankingInfo.lLostCount = m_AccountsDBAide.GetValue_LONG(TEXT("LostCount"));
			UserRankingInfo.lLoveHeart = m_AccountsDBAide.GetValue_LONG(TEXT("LoveHeart"));
			UserRankingInfo.lRankScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("RankScore"));
			UserRankingInfo.lWinStreakCount = m_AccountsDBAide.GetValue_LONG(TEXT("WinStreakCount"));			

			//���ͽ��			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_RANKING_INFO, dwContextID, dwTokenID, &UserRankingInfo, sizeof(UserRankingInfo));

		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_QUERY_RANKINGINFO;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = SUB_MB_QUERY_RANKINGINFO;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//��ѯ����
bool CDataBaseEngineSink::OnRequestMBQueryLoveLiness(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
    //Ч�����
    ASSERT(wDataSize == sizeof(DBR_MB_QueryLoveLiness));
    if (wDataSize != sizeof(DBR_MB_QueryLoveLiness)) return false;

    //������
	DBR_MB_QueryLoveLiness* pQueryLoveLiness = (DBR_MB_QueryLoveLiness*)pData;

	//��������
	UpdateUserLoveLiness(pQueryLoveLiness->dwUserID, dwContextID, dwTokenID);

	return true;
}
//��ѯ�ͱ�
bool CDataBaseEngineSink::OnRequestMBQuerySubsidy(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_SubsidyQuery));
		if (wDataSize != sizeof(DBR_MB_SubsidyQuery)) return false;

		//������
		DBR_MB_SubsidyQuery* pQuerySubsidy = (DBR_MB_SubsidyQuery*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pQuerySubsidy->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@szMachine"), pQuerySubsidy->szMachineID);

		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QuerySubsidy"), true);

		//��ȡ��Ϣ
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBR_MB_Subsidy Subsidy;
			ZeroMemory(&Subsidy, sizeof(Subsidy));

			//����ṹ
			Subsidy.cbDailyTimes = m_AccountsDBAide.GetValue_BYTE(TEXT("ClaimTimesPerAccounts"));
			Subsidy.cbResidualTimes = m_AccountsDBAide.GetValue_BYTE(TEXT("ClaimTimes"));
			Subsidy.lConsumeScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("ClaimCondition"));
			Subsidy.cbResidualTimes = Subsidy.cbDailyTimes <= Subsidy.cbResidualTimes ? 0 : Subsidy.cbDailyTimes - Subsidy.cbResidualTimes;
			//���ͽ��			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_RESULT_SUBSIDY, dwContextID, dwTokenID, &Subsidy, sizeof(DBR_MB_Subsidy));

		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_USER_SUBSIDY;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = SUB_MB_USER_SUBSIDY;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
};
//��������
bool CDataBaseEngineSink::OnRequestLoadConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//��ȡ����
	auto pLoadConfig = (DBR_GP_LoadConfig*)pData;

	//������
	DBO_GP_ConfigResult ConfigResult;
	ZeroMemory(&ConfigResult, sizeof(ConfigResult));

	//��������
	ConfigResult.bInitService = pLoadConfig->bInitService;
	ConfigResult.dwConfigMask = pLoadConfig->dwConfigMask;

	do
	{
		//��Ϸ�б�
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_GAME_LIST)
		{
			ConfigResult.bResultCode = LoadGameList(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//�汾��Ϣ
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_PLAZA_VERSION)
		{
			ConfigResult.bResultCode = LoadPlazaVersion(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//���дʻ�
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_FILTER_WORDS)
		{
			ConfigResult.bResultCode = LoadFilterWords(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//ת�˲���
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_TRANSFER_PARAM)
		{
			ConfigResult.bResultCode = LoadTransferParemter(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;			
		}

		//ȫ������
		if (pLoadConfig->dwConfigMask & GLOBAL_OPTION_MASK)
		{
			ConfigResult.bResultCode = LoadGlobalOption(dwContextID, dwTokenID, pLoadConfig->dwConfigMask);
			if (!ConfigResult.bResultCode) break;
		}

		//�ǳ�����
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_NICKANME_OPTION)
		{
			ConfigResult.bResultCode = LoadNickNameOption(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

	} while (false);

	//���ͽ��
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_CONFIG_RESULT, dwContextID, dwTokenID, &ConfigResult, sizeof(ConfigResult));

	return true;
}

//���ذ�
bool CDataBaseEngineSink::OnRequestLoadRankingList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//��������
		DBO_GP_RankingListData RankingListData;
		ZeroMemory(&RankingListData,sizeof(RankingListData));

		//���Ͷ���
		BYTE cbListType[] = { LIST_TYPE_PAY,LIST_TYPE_WEALTH,LIST_TYPE_BIGWINNER,LIST_TYPE_SPREAD,LIST_TYPE_COMMISSION };

		for (int i = 0; i < CountArray(cbListType); i++)
		{
			//����Key
			string sKey = RKEY_PREFIX_RANKINGLIST + to_string(cbListType[i]);

			//��ȡ����
			auto sRankingList = m_pMasterClient->get(sKey);
			if (sRankingList)
			{
				//��������
				RankingListData.cbListType = cbListType[i];
				StringCchCopyA(RankingListData.szRankingList,CountArray(RankingListData.szRankingList),sRankingList->c_str());

				//���ͽ��
				WORD wListSize = CountStringBufferA(RankingListData.szRankingList);
				WORD wPacketSize = sizeof(RankingListData) - sizeof(RankingListData.szRankingList) + wListSize;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_RANKING_LIST_DATA, dwContextID, dwTokenID, &RankingListData, wPacketSize);
			}
		}

		return true;
	}
	catch (exception& ex)
	{
		CTraceService::TraceString(TEXT("%hs"), TraceLevel_Exception, ex.what());

		//�ӳٴ���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;
	}

	return false;
}

//��������
bool CDataBaseEngineSink::OnRequestResetUserData(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//��ȡ����
		auto pResetUserData = (DBR_GP_ResetUserData*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wDayOfWeek"), pResetUserData->wDayOfWeek);
		m_AccountsDBAide.AddParameter(TEXT("@strUserIDList"), pResetUserData->szUserIDList);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ResetUserData"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//������Ϣ
bool CDataBaseEngineSink::OnRequestOnLineCountInfo(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//��������
		DBR_GP_OnLineCountInfo * pOnLineCountInfo=(DBR_GP_OnLineCountInfo *)pData;
		WORD wHeadSize=(sizeof(DBR_GP_OnLineCountInfo)-sizeof(pOnLineCountInfo->OnLineCountKind));

		//Ч������
		ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pOnLineCountInfo->wKindCount*sizeof(tagOnLineInfoKind))));
		if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pOnLineCountInfo->wKindCount*sizeof(tagOnLineInfoKind)))) return false;

		//������ʶ
		WCHAR szMachineID[LEN_MACHINE_ID];
		CWHService::GetMachineID(szMachineID);

		//������Ϣ
		TCHAR szOnLineCountKind[2048]=TEXT("");
		for (WORD i=0;i<pOnLineCountInfo->wKindCount;i++)
		{
			INT nLength=lstrlen(szOnLineCountKind);
			INT nBufferCount = CountArray(szOnLineCountKind)-nLength;
			_sntprintf_s(&szOnLineCountKind[nLength], nBufferCount, nBufferCount,TEXT("%d:%ld;"),pOnLineCountInfo->OnLineCountKind[i].wKindID,
				pOnLineCountInfo->OnLineCountKind[i].dwOnLineCount);
		}

		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@strMachineID"),szMachineID);
		m_PlatformDBAide.AddParameter(TEXT("@strMachineServer"),m_pInitParameter->m_szServerName);
		m_PlatformDBAide.AddParameter(TEXT("@dwOnLineCountSum"),pOnLineCountInfo->dwOnLineCountSum);
		m_PlatformDBAide.AddParameter(TEXT("@strOnLineCountKind"),szOnLineCountKind);

		//ִ������
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GP_OnLineCountInfo"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//�����
VOID CDataBaseEngineSink::OnStockDisposeResult(DWORD dwContextID, DWORD dwTokenID, DWORD dwErrorCode, WORD wSubCommdID)
{
	//���嶨��
	DBO_GP_StockOperateResult StockOperateResult = { 0 };

	//��������
	StockOperateResult.wPacketIdx = 0;
	StockOperateResult.wSubCommdID = wSubCommdID;

	//��ȡ��Ϣ
	while (m_PlatformDBModule->IsRecordsetEnd() == false)
	{
		//��ȡ����
		auto pStockInfo = &StockOperateResult.StockList[StockOperateResult.wStockCount++];

		//��ȡ��Ϣ
		pStockInfo->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
		pStockInfo->wStockID = m_PlatformDBAide.GetValue_WORD(TEXT("StockID"));
		pStockInfo->wStockKind = m_PlatformDBAide.GetValue_WORD(TEXT("StockKind"));
		pStockInfo->wStockLevel = m_PlatformDBAide.GetValue_WORD(TEXT("StockLevel"));
		pStockInfo->cbStockState = m_PlatformDBAide.GetValue_BYTE(TEXT("StockState"));
		pStockInfo->lBetAmount = m_PlatformDBAide.GetValue_LONG(TEXT("BetAmount"));

		//��������
		pStockInfo->cbEnableMode = m_PlatformDBAide.GetValue_BYTE(TEXT("EnableMode"));
		pStockInfo->dwEnableEndTime = m_PlatformDBAide.GetValue_DWORD(TEXT("EnableEndTime"));
		pStockInfo->dwEnableStartTime = m_PlatformDBAide.GetValue_DWORD(TEXT("EnableStartTime"));

		//��������
		pStockInfo->lStockScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("StockScore"));
		pStockInfo->lExtraScore = m_PlatformDBAide.GetValue_LONGLONG(TEXT("ExtraScore"));

		//��ˮ����
		pStockInfo->lShrinkProfit = m_PlatformDBAide.GetValue_LONGLONG(TEXT("ShrinkProfit"));

		//�������
		m_PlatformDBAide.GetValue_String(TEXT("StockName"), pStockInfo->szStockName, CountArray(pStockInfo->szStockName));

		//������ʶ
		m_PlatformDBAide.GetValue_String(TEXT("RelationIDList"), pStockInfo->szRelationStockID, CountArray(pStockInfo->szRelationStockID));

		//��ˮ����
		m_PlatformDBAide.GetValue_String(TEXT("ShrinkOption"), pStockInfo->szShrinkOption, CountArray(pStockInfo->szShrinkOption));

		//��������
		CHAR szJackpotOption[2048];
		m_PlatformDBAide.GetValue_String(TEXT("JackpotOption"), szJackpotOption, CountArray(szJackpotOption));

		//��������
		if (szJackpotOption[0] != 0)
		{
			pStockInfo->wJackpotCount = ParseJackpotOption(szJackpotOption, pStockInfo->GameJackpotItem,CountArray(pStockInfo->GameJackpotItem));
		}

		//�ƶ���¼
		m_PlatformDBModule->MoveToNext();

		//�����ж�
		if (StockOperateResult.wStockCount >= CountArray(StockOperateResult.StockList))
		{
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_STOCK_OPERATE_RESULT, dwContextID, dwTokenID, &StockOperateResult, sizeof(StockOperateResult));

			StockOperateResult.wPacketIdx++;
			StockOperateResult.wStockCount = 0;			
		}
	}

	//�����ж�
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_STOCK_OPERATE_RESULT, dwContextID, dwTokenID, &StockOperateResult, sizeof(StockOperateResult));
}

//�������
VOID CDataBaseEngineSink::OnRobotDisposeResult(DWORD dwContextID, DWORD dwTokenID, DWORD dwErrorCode,WORD wSubCommdID,WORD wServerID)
{
	//����ṹ
	DBO_GP_RobotOperateResult RobotOperateResult;
	ZeroMemory(&RobotOperateResult,sizeof(RobotOperateResult));

	//���ñ���
	RobotOperateResult.wSubCommdID=wSubCommdID;
	RobotOperateResult.wServerID=wServerID;

	if(dwErrorCode==DB_SUCCESS)
	{
		//��������
		WORD wParameterCount=0;

		while(m_AccountsDBModule->IsRecordsetEnd()==false)		
		{
			wParameterCount = RobotOperateResult.wParameterCount;
			RobotOperateResult.RobotParameter[wParameterCount].dwBatchID = m_AccountsDBAide.GetValue_DWORD(TEXT("BatchID"));
			RobotOperateResult.RobotParameter[wParameterCount].dwRobotCount = m_AccountsDBAide.GetValue_DWORD(TEXT("RobotCount"));
			RobotOperateResult.RobotParameter[wParameterCount].dwServiceMode = m_AccountsDBAide.GetValue_DWORD(TEXT("ServiceMode"));
			RobotOperateResult.RobotParameter[wParameterCount].dwEnterTime = m_AccountsDBAide.GetValue_DWORD(TEXT("EnterTime"));
			RobotOperateResult.RobotParameter[wParameterCount].dwLeaveTime = m_AccountsDBAide.GetValue_DWORD(TEXT("LeaveTime"));
			RobotOperateResult.RobotParameter[wParameterCount].dwEnterMinInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("EnterMinInterval"));
			RobotOperateResult.RobotParameter[wParameterCount].dwEnterMaxInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("EnterMaxInterval"));
			RobotOperateResult.RobotParameter[wParameterCount].dwLeaveMinInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("LeaveMinInterval"));
			RobotOperateResult.RobotParameter[wParameterCount].dwLeaveMaxInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("LeaveMaxInterval"));
			RobotOperateResult.RobotParameter[wParameterCount].dwSwitchMinInnings = m_AccountsDBAide.GetValue_DWORD(TEXT("SwitchMinInnings"));
			RobotOperateResult.RobotParameter[wParameterCount].dwSwitchMaxInnings = m_AccountsDBAide.GetValue_DWORD(TEXT("SwitchMaxInnings"));
			RobotOperateResult.RobotParameter[wParameterCount].lTakeMinScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("TakeMinScore"));
			RobotOperateResult.RobotParameter[wParameterCount].lTakeMaxScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("TakeMaxScore"));

			//���ñ���
			RobotOperateResult.wParameterCount++;

			//����ж�
			if(RobotOperateResult.wParameterCount>=CountArray(RobotOperateResult.RobotParameter))
			{
				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_ROBOT_OPERATE_RESULT,dwContextID, dwTokenID, &RobotOperateResult,sizeof(RobotOperateResult));

				//���ñ���
				ZeroMemory(&RobotOperateResult,sizeof(RobotOperateResult));
			}

			//�ƶ��α�
			m_AccountsDBModule->MoveToNext();
		}	
	}
	
	//�����С
	WORD wSendDataSize = sizeof(RobotOperateResult)-sizeof(RobotOperateResult.RobotParameter);
	wSendDataSize += RobotOperateResult.wParameterCount*sizeof(tagRobotParameter);

	//��������
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_ROBOT_OPERATE_RESULT,dwContextID, dwTokenID, &RobotOperateResult,wSendDataSize);
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
			CWHJsonInput::readJson(pJackpotItem->lInitialScore, JackpotItemObjPtr->value["FirstGold"], true);
			
			//�˳��ж�
			if (wItemCount == wBufferCount) break;
		}
	}

	return wItemCount;
}

//���л��ʳ�
bool CDataBaseEngineSink::SerializeJackpotOption(tagJackpotItem JackpotItem[], WORD wItemCount, TCHAR szJackpotOption[], WORD wBufferSize)
{
	//����Json
	CString strJackpotList, strJackpotItem;
	LPCTSTR pszJackpotFmt = L"{\"LevelID\":%d,\"StockScore\":%I64d,\"ExtraScore\":%I64d,\"VirtualScore\":%I64d,\"ShrinkRatio\":%d,\"PayoutProb\":%d,\"PayoutLimit\":%I64d,\"PayoutOrigin\":%I64d}";

	//��ʽ��
	for (int i = 0; i < wItemCount; i++)
	{
		strJackpotItem.Format(pszJackpotFmt, JackpotItem[i].wLevelID, JackpotItem[i].lStockScore, JackpotItem[i].lExtraScore, JackpotItem[i].lVirtualScore, 
			JackpotItem[i].wShrinkRatio, JackpotItem[i].wPayoutProb, JackpotItem[i].lPayoutLimit, JackpotItem[i].lPayoutOrigin);

		//��ӷָ���
		if (!strJackpotList.IsEmpty()) strJackpotList.AppendChar(L',');

		//�������
		strJackpotList.Append(strJackpotItem);
	}

	//�������
	strJackpotList.Insert(0, L'[');
	strJackpotList.AppendChar(L']');

	//У�黺���С
	if (strJackpotList.GetLength() > wBufferSize)
	{
		return false;
	}

	//�����ַ���
	StringCchCopy(szJackpotOption,wBufferSize,strJackpotList.GetString());

	return true;
}

//���л�Ȩ��
bool CDataBaseEngineSink::SerializeWeightConfig(tagTimesWeightItem WeightItem[], WORD wItemCount, TCHAR szWeightConfig[], WORD wBufferSize)
{
	//����Json
	CString strWeightList, strWeightItem;
	LPCTSTR pszWeightFmt = L"{\"MinTimes\":%d,\"MaxTimes\":%d,\"TimesKind\":%d,\"WeightValue\":%d}";

	//��ʽ��
	for (int i = 0; i < wItemCount; i++)
	{
		strWeightItem.Format(pszWeightFmt, WeightItem[i].dwTimesRange[0], WeightItem[i].dwTimesRange[1], WeightItem[i].cbTimesKind, WeightItem[i].wTimesWeight);

		//��ӷָ���
		if (!strWeightList.IsEmpty()) strWeightList.AppendChar(L',');

		//�������
		strWeightList.Append(strWeightItem);
	}

	//�������
	strWeightList.Insert(0, L'[');
	strWeightList.AppendChar(L']');

	//У�黺���С
	if (strWeightList.GetLength() > wBufferSize)
	{
		return false;
	}

	//�����ַ���
	StringCchCopy(szWeightConfig, wBufferSize, strWeightList.GetString());

	return true;
}

//�����Ϣ
void CDataBaseEngineSink::TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException)
{
	CString strMessage;
	strMessage.Format(TEXT("%s(%d):%hs(%s)"), CA2CT(pFile).m_psz, nLine, pFunction, pIException ? pIException->GetExceptionDescribe() : TEXT(""));
	CTraceService::TraceString(strMessage, TraceLevel_Exception);
}

//��ѯ�齱
bool CDataBaseEngineSink::OnRequestMBQueryTurntableLottery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_QueryTurntableLottery));
		if (wDataSize != sizeof(DBR_MB_QueryTurntableLottery)) return false;

		//������
		auto pQueryTurntableLottery = (DBR_MB_QueryTurntableLottery*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pQueryTurntableLottery->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pQueryTurntableLottery->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwRecordID"), pQueryTurntableLottery->dwRecordID);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pQueryTurntableLottery->szMachineID);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryTurntableLottery"), true);		

		//��ȡ��Ϣ
		if (lReturnValue == DB_SUCCESS)
		{
			//�齱����
			DBO_MB_TurntableLotteryData TurntableLotteryData;
			ZeroMemory(&TurntableLotteryData, sizeof(TurntableLotteryData));

			//�齱����
			TurntableLotteryData.wPayLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("PayLotteryTimes"));
			TurntableLotteryData.wUseLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("UseLotteryTimes"));
			TurntableLotteryData.wFreeLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("FreeLotteryTimes"));						
			TurntableLotteryData.wResidueLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("ResidueLotteryTimes"));
			TurntableLotteryData.wDailyMayLotteryTimes= m_AccountsDBAide.GetValue_WORD(TEXT("DailyMayLotteryTimes"));
			TurntableLotteryData.wGameTimeLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("GameTimeLotteryTimes"));
			TurntableLotteryData.wGameCountLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("GameCountLotteryTimes"));
			TurntableLotteryData.wGameExpendLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("GameExpendLotteryTimes"));

			//��������
			TurntableLotteryData.dwGameTimeProgress[0] = m_AccountsDBAide.GetValue_DWORD(TEXT("CurrGameTime"));
			TurntableLotteryData.dwGameTimeProgress[1] = m_AccountsDBAide.GetValue_DWORD(TEXT("SliceGameTime"));
			TurntableLotteryData.dwGameCountProgress[0] = m_AccountsDBAide.GetValue_DWORD(TEXT("CurrGameCount"));
			TurntableLotteryData.dwGameCountProgress[1] = m_AccountsDBAide.GetValue_DWORD(TEXT("SliceGameCount"));
			TurntableLotteryData.dwGameExpendProgress[0] = m_AccountsDBAide.GetValue_DWORD(TEXT("CurrGameExpend"));
			TurntableLotteryData.dwGameExpendProgress[1] = m_AccountsDBAide.GetValue_DWORD(TEXT("SliceGameExpend"));

			//ת�̼�¼
			m_AccountsDBAide.GetValue_String(TEXT("TurntableRecord"), TurntableLotteryData.szTurntableRecord,CountArray(TurntableLotteryData.szTurntableRecord));

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_TURNTABLE_LOTTERY_DATA, dwContextID, dwTokenID, &TurntableLotteryData, sizeof(TurntableLotteryData));
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.wRequestCmdID = SUB_MB_QUERY_TURNTABLE_LOTTERY;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"����ϵͳ��æ�����Ժ����ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//����齱
bool CDataBaseEngineSink::OnRequestMBRequestTurntableLottery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_RequestTurntableLottery));
		if (wDataSize != sizeof(DBR_MB_RequestTurntableLottery)) return false;

		//������
		auto pRequestTurntableLottery = (DBR_MB_RequestTurntableLottery*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pRequestTurntableLottery->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_TURNTABLE_LOTTERY);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RequestTurntableLottery"), true);

		//���촦��
		if (lReturnValue == 100)
		{
			//����ṹ
			DBR_MB_QueryTurntableLottery QueryTurntableLottery;
			ZeroMemory(&QueryTurntableLottery,sizeof(QueryTurntableLottery));

			//���ñ���
			QueryTurntableLottery.dwUserID = pRequestTurntableLottery->dwUserID;
			QueryTurntableLottery.dwClientIP = pRequestTurntableLottery->dwClientIP;
			StringCchCopy(QueryTurntableLottery.szMachineID,CountArray(QueryTurntableLottery.szMachineID), pRequestTurntableLottery->szMachineID);

			//��ѯ�齱����
			if (OnRequestMBQueryTurntableLottery(dwContextID, dwTokenID, &QueryTurntableLottery, sizeof(QueryTurntableLottery), pdwControlCode))
			{
				//�������
				m_AccountsDBAide.ResetParameter();
				m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pRequestTurntableLottery->dwUserID);
				m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_TURNTABLE_LOTTERY);

				//�������
				m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

				//ִ�в�ѯ
				lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_RequestTurntableLottery"), true);
			}			
		}

		//�齱���
		DBO_MB_TurntableLotteryResult TurntableLotteryResult;
		ZeroMemory(&TurntableLotteryResult,sizeof(TurntableLotteryResult));		

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��ȡ����
			TurntableLotteryResult.wHitIndex = m_AccountsDBAide.GetValue_WORD(TEXT("HitIndex"));
			TurntableLotteryResult.wUseLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("UseLotteryTimes"));
			TurntableLotteryResult.wResidueLotteryTimes = m_AccountsDBAide.GetValue_WORD(TEXT("ResidueLotteryTimes"));

			//���ͽ��
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_TURNTABLE_LOTTERY_RESULT, dwContextID, dwTokenID, &TurntableLotteryResult, sizeof(TurntableLotteryResult));
		}
		else
		{
			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_REQUEST_TURNTABLE_LOTTERY;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.wRequestCmdID = SUB_MB_REQUEST_TURNTABLE_LOTTERY;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"����ϵͳ��æ�����Ժ����ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}

	return true;
}

//���ر���
bool CDataBaseEngineSink::LoadUserPackGoods()
{
	try
	{
		//���ñ���
		m_MBLogonSuccess.wGoodsCount = 0;

		//��ǰʱ��
		DWORD dwCurrentTime = (DWORD)time(NULL)+10;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), m_MBLogonSuccess.dwUserID);		

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserQueryGoods"), true);

		//ִ�гɹ�
		if (lResultCode == DB_SUCCESS)
		{
			//��������
			WORD wExpireCount = 0;
			WORD wAccessorMask = 0;

			//��������
			CString strAccessorKind=TEXT("");

			//��ȡ��Ϣ
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				//��ȡ����
				auto pGoodsHoldInfo = &m_MBLogonSuccess.GoodsHoldInfo[m_MBLogonSuccess.wGoodsCount++];

				//��ȡ����
				pGoodsHoldInfo->wGoodsID = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsID"));
				pGoodsHoldInfo->wGoodsKind = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsKind"));
				pGoodsHoldInfo->wGoodsLevel = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsLevel"));
				pGoodsHoldInfo->dwGoodsCount = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsCount"));
				pGoodsHoldInfo->dwGoodsIndate = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsIndate"));
				pGoodsHoldInfo->dwExpireTime = m_AccountsDBAide.GetValue_DWORD(TEXT("ExpireTime"));

				//���ڴ���
				if (pGoodsHoldInfo->dwGoodsIndate > 0 && dwCurrentTime >= pGoodsHoldInfo->dwExpireTime)
				{
					//��������
					wExpireCount++;

					//�Ƴ���Ʒ
					m_MBLogonSuccess.wGoodsCount--;								
				}

				//У������
				if (m_MBLogonSuccess.wGoodsCount >= CountArray(m_MBLogonSuccess.GoodsHoldInfo))
				{
					break;
				}				

				//�ƶ���¼
				m_AccountsDBModule->MoveToNext();
			}	

			//�������
			if (!strAccessorKind.IsEmpty())
			{
				strAccessorKind.Append(TEXT("]"));
			}
			else
			{
				strAccessorKind.Append(TEXT("[]"));
			}			
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

//��λ��Ϣ
bool CDataBaseEngineSink::LoadUserRankingInfo()
{
	try
	{
		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), m_MBLogonSuccess.dwUserID);		

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadUserRankingInfo"), true);

		//��ȡ��Ϣ
		if (lReturnValue == DB_SUCCESS)
		{
			auto pUserRankingInfo = &m_MBLogonSuccess.UserRankingInfo;
			pUserRankingInfo->dwRankID = m_AccountsDBAide.GetValue_DWORD(TEXT("RankID"));
			pUserRankingInfo->wSeasonID = m_AccountsDBAide.GetValue_WORD(TEXT("SeasonID"));
			pUserRankingInfo->lWinCount = m_AccountsDBAide.GetValue_LONG(TEXT("WinCount"));
			pUserRankingInfo->lLostCount = m_AccountsDBAide.GetValue_LONG(TEXT("LostCount"));
			pUserRankingInfo->lLoveHeart = m_AccountsDBAide.GetValue_LONG(TEXT("LoveHeart"));
			pUserRankingInfo->lRankScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("RankScore"));
			pUserRankingInfo->lWinStreakCount = m_AccountsDBAide.GetValue_LONG(TEXT("WinStreakCount"));			
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

//��Ϸ�б�
bool CDataBaseEngineSink::LoadGameList(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//��������
		WORD wPacketSize = 0;
		BYTE cbBuffer[SOCKET_PACKET];

		//��������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameTypeItem"), true);

		//��������
		wPacketSize = 0;
		DBO_GP_GameType* pGameType = NULL;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//������Ϣ
			if ((wPacketSize + sizeof(DBO_GP_GameType)) > sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_TYPE_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);
				wPacketSize = 0;
			}

			//��ȡ��Ϣ
			pGameType = (DBO_GP_GameType*)(cbBuffer + wPacketSize);
			pGameType->wSortID = m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameType->wJoinID = m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameType->wTypeID = m_PlatformDBAide.GetValue_WORD(TEXT("TypeID"));
			m_PlatformDBAide.GetValue_String(TEXT("TypeName"), pGameType->szTypeName, CountArray(pGameType->szTypeName));

			//����λ��
			wPacketSize += sizeof(DBO_GP_GameType);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize > 0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_TYPE_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);

		//��ȡ����
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameKindItem"), true);

		//��������
		wPacketSize = 0;
		DBO_GP_GameKind* pGameKind = NULL;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//������Ϣ
			if ((wPacketSize + sizeof(DBO_GP_GameKind)) > sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);
				wPacketSize = 0;
			}

			//��ȡ��Ϣ
			pGameKind = (DBO_GP_GameKind*)(cbBuffer + wPacketSize);
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
			wPacketSize += sizeof(DBO_GP_GameKind);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize > 0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_KIND_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);

		//��ȡ�ڵ�
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameNodeItem"), true);

		//���ͽڵ�
		wPacketSize = 0;
		DBO_GP_GameNode* pGameNode = NULL;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//������Ϣ
			if ((wPacketSize + sizeof(DBO_GP_GameNode)) > sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_NODE_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);
				wPacketSize = 0;
			}

			//��ȡ��Ϣ
			pGameNode = (DBO_GP_GameNode*)(cbBuffer + wPacketSize);
			pGameNode->wSortID = m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameNode->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameNode->wJoinID = m_PlatformDBAide.GetValue_WORD(TEXT("JoinID"));
			pGameNode->wNodeID = m_PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
			m_PlatformDBAide.GetValue_String(TEXT("NodeName"), pGameNode->szNodeName, CountArray(pGameNode->szNodeName));

			//����λ��
			wPacketSize += sizeof(DBO_GP_GameNode);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize > 0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_NODE_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);

		//��ȡ����
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameCustomItem"), true);

		//���Ͷ���
		wPacketSize = 0;
		DBO_GP_GameCustom* pGameCustom = NULL;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//������Ϣ
			if ((wPacketSize + sizeof(DBO_GP_GameCustom)) > sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PAGE_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);
				wPacketSize = 0;
			}

			//��ȡ��Ϣ
			pGameCustom = (DBO_GP_GameCustom*)(cbBuffer + wPacketSize);
			pGameCustom->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameCustom->wNodeID = m_PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
			pGameCustom->wSortID = m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameCustom->wPageID = m_PlatformDBAide.GetValue_WORD(TEXT("PageID"));
			pGameCustom->wOperateType = m_PlatformDBAide.GetValue_WORD(TEXT("OperateType"));
			m_PlatformDBAide.GetValue_String(TEXT("DisplayName"), pGameCustom->szDisplayName, CountArray(pGameCustom->szDisplayName));
			m_PlatformDBAide.GetValue_String(TEXT("ResponseUrl"), pGameCustom->szReponseUrl, CountArray(pGameCustom->szReponseUrl));

			//����λ��
			wPacketSize += sizeof(DBO_GP_GameCustom);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize > 0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_PAGE_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);

		//��ȡ����
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameFilterItem"), true);

		//���͹���
		wPacketSize = 0;
		DBO_GP_GameFilter* pGameFilter = NULL;
		while (m_PlatformDBModule->IsRecordsetEnd() == false)
		{
			//������Ϣ
			if ((wPacketSize + sizeof(DBO_GP_GameFilter)) > sizeof(cbBuffer))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_FILTER_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);
				wPacketSize = 0;
			}

			//��ȡ��Ϣ
			pGameFilter = (DBO_GP_GameFilter*)(cbBuffer + wPacketSize);
			pGameFilter->wSortID = m_PlatformDBAide.GetValue_WORD(TEXT("SortID"));
			pGameFilter->wKindID = m_PlatformDBAide.GetValue_WORD(TEXT("KindID"));
			pGameFilter->wNodeID = m_PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
			pGameFilter->wServerID = m_PlatformDBAide.GetValue_WORD(TEXT("ServerID"));
			pGameFilter->wFilterMask = m_PlatformDBAide.GetValue_WORD(TEXT("SiftMask"));
			m_PlatformDBAide.GetValue_String(TEXT("ServerName"), pGameFilter->szServerName, CountArray(pGameFilter->szServerName));

			//����λ��
			wPacketSize += sizeof(DBO_GP_GameFilter);

			//�ƶ���¼
			m_PlatformDBModule->MoveToNext();
		}
		if (wPacketSize > 0) m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_GAME_FILTER_ITEM, dwContextID, dwTokenID, cbBuffer, wPacketSize);

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

//���дʻ�
bool CDataBaseEngineSink::LoadFilterWords(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//�������
		m_AccountsDBAide.ResetParameter();

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadConfineContent"), true);

		//��ȡ��Ϣ
		if (lReturnValue == DB_SUCCESS)
		{
			//��ʼ��Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_FILTER_WORDS, 0xfffe, dwTokenID, NULL, 0);

			//��ȡ��Ϣ
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				TCHAR szFribidWords[32] = TEXT("");

				//��ȡ��Ϣ
				m_AccountsDBAide.GetValue_String(TEXT("ForbidWords"), szFribidWords, 32);

				//������Ϣ
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_FILTER_WORDS, 0, dwTokenID, szFribidWords, 32);

				//�ƶ���¼
				m_AccountsDBModule->MoveToNext();
			};

			//������Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_FILTER_WORDS, 0xffff, 0, NULL, 0);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//ȫ������
bool CDataBaseEngineSink::LoadGlobalOption(DWORD dwContextID, DWORD dwTokenID, DWORD dwOptionMask)
{
	//��������
	TCHAR szOptionData[2048] = {0};

	try
	{
		//ǩ������
		if (dwOptionMask & OPTION_MASK_CHECKIN)
		{
			//�������
			m_PlatformDBAide.ResetParameter();
			LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadCheckInParameter"), true);

			//���سɹ�
			if (lReturnValue == DB_SUCCESS)
			{
				//��ȡ����
				m_PlatformDBAide.GetValue_String(TEXT("CheckInOption"), szOptionData, CountArray(szOptionData));

				//�������
				if (!g_GlobalOptionManager->AppendOptionItem(OPTION_MASK_CHECKIN, szOptionData))
				{
					return false;
				}
			}
		}

		//ת������
		if (dwOptionMask & OPTION_MASK_TURNTABLE)
		{		
			//�������
			m_PlatformDBAide.ResetParameter();
			LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadTurntableParameter"), true);

			//���سɹ�
			if (lReturnValue == DB_SUCCESS)
			{
				//��ȡ����
				m_PlatformDBAide.GetValue_String(TEXT("TurntableOption"), szOptionData, CountArray(szOptionData));

				//�������
				if (!g_GlobalOptionManager->AppendOptionItem(OPTION_MASK_TURNTABLE, szOptionData))
				{
					return false;
				}
			}			
		}	

		//���������
		if (dwOptionMask & OPTION_MASK_MARQUEE_NOTICE)
		{
			//�������
			m_PlatformDBAide.ResetParameter();
			LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadMarqueeNoticeOption"), true);

			//���سɹ�
			if (lReturnValue == DB_SUCCESS)
			{
				//��ȡ����
				m_PlatformDBAide.GetValue_String(TEXT("MarqueeNoticeOption"), szOptionData, CountArray(szOptionData));

				//�������
				if (!g_GlobalOptionManager->AppendOptionItem(OPTION_MASK_MARQUEE_NOTICE, szOptionData))
				{
					return false;
				}
			}
		}
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
//�ǳ�����
bool CDataBaseEngineSink::LoadNickNameOption(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//��������
		tagNickNameOption NickNameOption;
		ZeroMemory(&NickNameOption, sizeof(NickNameOption));

		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ�в�ѯ
		LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadNickNameOption"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//���ò���
			NickNameOption.wFreeModifyTimes = m_PlatformDBAide.GetValue_WORD(TEXT("FreeModifyTimes"));
			NickNameOption.wDailyModifyTimes = m_PlatformDBAide.GetValue_WORD(TEXT("DailyModifyTimes"));
			NickNameOption.dwGoodsID = m_PlatformDBAide.GetValue_DWORD(TEXT("GoodsID"));
			NickNameOption.dwGoodsCount = m_PlatformDBAide.GetValue_DWORD(TEXT("GoodsCount"));
		}

		//���ͽ��
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_NICKNAME_OPTION, 0, 0, &NickNameOption, sizeof(tagNickNameOption));

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
};
//�汾��Ϣ
bool CDataBaseEngineSink::LoadPlazaVersion(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wModuleID"), 0);
		LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameVersionInfo"), true);

		//���سɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			WORD wPacketSize = 0;
			BYTE cbBuffer[SOCKET_PACKET];

			//��������
			tagModuleVersion* pModuleVersion = NULL;

			//��������
			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				//������Ϣ
				if ((wPacketSize + sizeof(tagModuleVersion)) > sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_VERSION_INFO, dwContextID, dwTokenID, cbBuffer, wPacketSize);
					wPacketSize = 0;
				}

				//��ȡ��Ϣ
				pModuleVersion = (tagModuleVersion*)(cbBuffer + wPacketSize);

				//��ȡ����				
				pModuleVersion->IsCurrent = m_PlatformDBAide.GetValue_BYTE(TEXT("IsCurrent"));
				pModuleVersion->wModuleID = m_PlatformDBAide.GetValue_WORD(TEXT("ModuleID"));
				pModuleVersion->dwDeviceKind = m_PlatformDBAide.GetValue_DWORD(TEXT("DeviceKind"));
				pModuleVersion->dwVersion = m_PlatformDBAide.GetValue_DWORD(TEXT("Version"));

				//����λ��
				wPacketSize += sizeof(tagModuleVersion);

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			//����ʣ��
			if (wPacketSize > 0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_VERSION_INFO, dwContextID, dwTokenID, cbBuffer, wPacketSize);
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

//ת������
bool CDataBaseEngineSink::LoadTransferParemter(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//��������
		tagTransferParameter TransferParemter;
		ZeroMemory(&TransferParemter, sizeof(TransferParemter));

		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ�в�ѯ
		LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadTransferParameter"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//���Ʋ���
			TransferParemter.bTransferEnabled = m_PlatformDBAide.GetValue_BYTE(TEXT("TransferEnabled"));
			TransferParemter.wServiceFeeRate = m_PlatformDBAide.GetValue_WORD(TEXT("ServiceFeeRate"));
			TransferParemter.lReservedAmount = m_PlatformDBAide.GetValue_LONGLONG(TEXT("ReservedAmount"));
			TransferParemter.lMinTransferAmount = m_PlatformDBAide.GetValue_LONGLONG(TEXT("MinTransferAmount"));
			TransferParemter.lMaxTransferAmount = m_PlatformDBAide.GetValue_LONGLONG(TEXT("MaxTransferAmount"));
		}

		//���ͽ��
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_TRANSFER_PARAMETER, 0, 0, &TransferParemter, sizeof(TransferParemter));

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//������Ϣ
bool CDataBaseEngineSink::CleanOnlineInfo(WORD wLogonID)
{
	try
	{
		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wLogonID"), wLogonID);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CleanOnlineLogonInfo"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return false;
}

//������Ϣ
bool CDataBaseEngineSink::CacheUserInfo(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//��������
		int nBuffLen = 0;
		CHAR szBuffer[512];

		//����ṹ
		UserPublicInfo PublicInfo;				

		//������Ϣ
		PublicInfo.set_uid(m_MBLogonSuccess.dwUserID);
		PublicInfo.set_gameid(m_MBLogonSuccess.dwGameID);
		PublicInfo.set_gender(m_MBLogonSuccess.cbGender);
		PublicInfo.set_faceid(m_MBLogonSuccess.wFaceID);
		PublicInfo.set_customid(m_MBLogonSuccess.dwCustomID);
		PublicInfo.set_infopublic(m_MBLogonSuccess.cbInfoPublic);
		PublicInfo.set_experience(m_MBLogonSuccess.dwExperience);
		PublicInfo.set_memberpoint(m_MBLogonSuccess.dwMemberPoint);				
		PublicInfo.set_masterorder(m_MBLogonSuccess.wMasterOrder);
		PublicInfo.set_rankscore(m_MBLogonSuccess.UserRankingInfo.lRankScore);
		PublicInfo.set_loveliness(m_MBLogonSuccess.dwLoveLiness);
		
		//�����ǳ�
		nBuffLen = CWHService::Unicode2Utf8(m_MBLogonSuccess.szNickName, szBuffer,CountArray(szBuffer));		
		PublicInfo.set_nickname(szBuffer, nBuffLen);

		//�����ǳ�
		nBuffLen = CWHService::Unicode2Utf8(m_MBLogonSuccess.szUnderWrite, szBuffer, CountArray(szBuffer));
		PublicInfo.set_underwrite(szBuffer, nBuffLen);

		//�汾��Ϣ
		lhchat::InfoVersion Version;

		//��ʼ���汾		
		for (int i=0; i< UserInfoIndex_ARRAYSIZE; i++) Version.add_infoversion(1);		

		//���л�
		string sVersion;
		Version.SerializeToString(&sVersion);

		//���ð汾
		PublicInfo.set_infoversion(sVersion);

		//���л�
		string sPublicInfo;
		PublicInfo.SerializeToString(&sPublicInfo);

		//�������
		string sUserKey = RKEY_PREFIX_USER + to_string(PublicInfo.uid());		

		//����key
		m_pUserClient->set(sUserKey, sPublicInfo);		

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
//������Ϣ
bool CDataBaseEngineSink::UpdateUserCache(DWORD dwContextID, DWORD dwTokenID, UserPublicInfo& PublicInfo)
{
	try
	{
		//�������
		string sUserKey = RKEY_PREFIX_USER + to_string(PublicInfo.uid());

		//��ȡ��Ϣ
		auto sCacheUserInfo = m_pUserClient->get(sUserKey);
		if (!sCacheUserInfo) return false;

		//��������
		UserPublicInfo CachePublicInfo;
		if (!CachePublicInfo.ParseFromString(sCacheUserInfo.value()))
		{
			return false;
		}

		//������Ϣ
		//if (PublicInfo.has_faceid()) CachePublicInfo.set_faceid(PublicInfo.faceid());
		//if (PublicInfo.has_customid()) CachePublicInfo.set_customid(PublicInfo.customid());
		if (PublicInfo.has_infopublic()) CachePublicInfo.set_infopublic(PublicInfo.infopublic());
		//if (PublicInfo.has_likednumber()) CachePublicInfo.set_infopublic(PublicInfo.likednumber());
		//if (PublicInfo.has_faceframeid()) CachePublicInfo.set_faceframeid(PublicInfo.faceframeid());

		//�汾��Ϣ
		InfoVersion Version;
		Version.ParseFromString(CachePublicInfo.infoversion());

		//������Ϣ
		for (auto i = 0; i < Version.infoversion_size(); i++)
		{
			//���±�־
			bool bUpdateVersion = false;

			do
			{
				//�û��Ա�
				if (i == UII_GENDER && PublicInfo.has_gender())
				{
					if (PublicInfo.gender() != CachePublicInfo.gender())
					{
						bUpdateVersion = true;
						CachePublicInfo.set_gender(PublicInfo.gender());
						break;
					}
				}

				//�û��ǳ�
				if (i == UII_NICKNAME && PublicInfo.has_nickname())
				{
					bUpdateVersion = true;
					CachePublicInfo.set_nickname(PublicInfo.nickname());
					break;
				}

				//����ǩ��
				if (i == UII_UNDERWRITE && PublicInfo.has_underwrite())
				{
					bUpdateVersion = true;
					CachePublicInfo.set_underwrite(PublicInfo.underwrite());
					break;
				}

				//��λ����
				if (i == UII_RANKSCORE && PublicInfo.has_rankscore())
				{
					bUpdateVersion = true;
					CachePublicInfo.set_rankscore(PublicInfo.rankscore());
					break;
				}

				//��Ϸ����
				if (i == UII_EXPERIENCE && PublicInfo.has_experience())
				{
					bUpdateVersion = true;
					CachePublicInfo.set_experience(PublicInfo.experience());
					break;
				}

				//��Ա����
				if (i == UII_MEMBERPOINT && PublicInfo.has_memberpoint())
				{
					bUpdateVersion = true;
					CachePublicInfo.set_memberpoint(PublicInfo.memberpoint());
					break;
				}

				//����ȼ�
				if (i == UII_MASTERORDER && PublicInfo.has_masterorder())
				{
					bUpdateVersion = true;
					CachePublicInfo.set_masterorder(PublicInfo.masterorder());
					break;
				}

				//ͷ���ʶ
				if (i == UII_FACEID && (PublicInfo.has_faceid() || PublicInfo.has_customid()))
				{
					bUpdateVersion = true;
					if (PublicInfo.has_faceid()) CachePublicInfo.set_faceid(PublicInfo.faceid());
					if (PublicInfo.has_customid()) CachePublicInfo.set_customid(PublicInfo.customid());
					break;
				}

				//����ʶ
				//if (i == UII_FACEFRAMEID && PublicInfo.has_faceframeid())
				//{
				//	bUpdateVersion = true;
				//	CachePublicInfo.set_faceframeid(PublicInfo.faceframeid());
				//	break;
				//}

				//����ֵ
				if (i == UII_LOVELINESS && PublicInfo.has_loveliness())
				{
					bUpdateVersion = true;
					CachePublicInfo.set_loveliness(PublicInfo.loveliness());
					break;
				}

				//�����ʶ
				//if (i == UII_CHATFRAMEID && PublicInfo.has_chatframeid())
				//{
				//	bUpdateVersion = true;
				//	CachePublicInfo.set_chatframeid(PublicInfo.chatframeid());
				//	break;
				//}
			} while (false);

			//���°汾
			if (bUpdateVersion) Version.set_infoversion(i, Version.infoversion(i) + 1);
		}

		//���л�
		string sVersion;
		Version.SerializeToString(&sVersion);

		//���ð汾
		CachePublicInfo.set_infoversion(sVersion);

		//���л�
		string sCachePublicInfo;
		CachePublicInfo.SerializeToString(&sCachePublicInfo);

		//����key
		m_pUserClient->set(sUserKey, sCachePublicInfo);

		//���»���
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_CACHE_INFO, dwContextID, dwTokenID, (char*)sCachePublicInfo.data(), sCachePublicInfo.size());

		return true;
	}
	catch (exception& ex)
	{
		CTraceService::TraceString(CA2CT(ex.what()).m_psz, TraceLevel_Exception);
	}

	return false;
}

//����TOKEN
bool CDataBaseEngineSink::BuildLogonToken(DWORD dwUserID, LPCTSTR pszAccounts, LPCTSTR pszSecretKey, TCHAR szUserToken[LEN_LOGON_TOKEN])
{
	//����У��
	ASSERT(pszAccounts != NULL);
	if (pszAccounts == NULL) return false;

	try
	{
		//��������
		DWORD dwCurrentTime = time(NULL);

		//������Ϣ
		CWHClaims Claims;
		Claims.Set("UserID", dwUserID);
		Claims.Set("Accounts", string(CT2A(pszAccounts).m_psz));
		Claims.Set("SecretKey", string(CT2A(pszSecretKey).m_psz));
		Claims.Set("Time", dwCurrentTime);

		//����JWT
		string sToken = CWHJwt::Sign(Claims);

		//��������
		StringCchCopy(szUserToken, LEN_LOGON_TOKEN, CA2CT(sToken.c_str()));

		//����key
		CHAR szTokenKey[64];
		_snprintf_s(szTokenKey, CountArray(szTokenKey), RKEY_PREFIX_TOKEN "%d", dwUserID);

		//��������
		CHAR szTokenValue[512];
		_snprintf_s(szTokenValue, CountArray(szTokenValue), "{\"Token\":\"%s\",\"Expire\":%d}", sToken.c_str(), dwCurrentTime + ONE_WEEK);

		//�������
		StringView sTokenKey(szTokenKey);
		StringView sUserToken(szTokenValue);

		//����key
		m_pTokenClient->set(sTokenKey, szTokenValue);

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

//���²Ƹ�
bool CDataBaseEngineSink::UpdateUserWealth(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryGameWealth"), true);

		//���سɹ�
		if (lResultCode == DB_SUCCESS)
		{
			//���²Ƹ�
			DBO_MB_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

			//���ñ���
			WealthUpdate.wItemCount = 2;
			WealthUpdate.WealthItem[0].wMoneyID = GOODS_ID_CARD;
			WealthUpdate.WealthItem[0].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserCard"));
			WealthUpdate.WealthItem[1].wMoneyID = GOODS_ID_GOLD;
			WealthUpdate.WealthItem[1].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));
			
			//���ͽ��			
			WORD wHeadSize = sizeof(WealthUpdate) - sizeof(WealthUpdate.WealthItem);
			WORD wPacketSize = WealthUpdate.wItemCount * sizeof(WealthUpdate.WealthItem[0]) + wHeadSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_WEALTH_UPDATE, dwContextID, dwTokenID, &WealthUpdate, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

//������Ʒ
bool CDataBaseEngineSink::UpdateUserGoods(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID, tagGoodsItem GoodsItemList[], WORD wGoodsCount)
{
	try
	{
		//��������
		CString strGoodsID;
		CString strGoodsIDList;

		//������Ʒ�б�
		for (auto i = 0; i < wGoodsCount; i++)
		{
			//�������
			if (i == 0) strGoodsIDList.Append(TEXT("["));

			//��ʽ��
			strGoodsID.Format(TEXT("%d"), GoodsItemList[i].wGoodsID);

			//׷������
			strGoodsIDList.Append(strGoodsID);
			strGoodsIDList.Append((i < wGoodsCount - 1) ? TEXT(",") : TEXT("]"));
		}

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@szGoodsIDList"), strGoodsIDList);
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserQueryGoods"), true);

		//���سɹ�
		if (lResultCode == DB_SUCCESS)
		{
			//��������
			DBO_MB_GoodsUpdate UserGoodsUpdate;
			ZeroMemory(&UserGoodsUpdate, sizeof(UserGoodsUpdate));

			//��ȡ��Ϣ
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				//��ȡ����
				auto pGoodsHoldInfo = &UserGoodsUpdate.GoodsHoldInfo[UserGoodsUpdate.wGoodsCount++];

				//��Ʒ��Ϣ
				pGoodsHoldInfo->wGoodsID = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsID"));
				pGoodsHoldInfo->wGoodsKind = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsKind"));
				pGoodsHoldInfo->wGoodsLevel = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsLevel"));
				pGoodsHoldInfo->dwGoodsCount = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsCount"));
				pGoodsHoldInfo->dwGoodsIndate = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsIndate"));
				pGoodsHoldInfo->dwExpireTime = m_AccountsDBAide.GetValue_DWORD(TEXT("ExpireTime"));

				//�ƶ���¼
				m_AccountsDBModule->MoveToNext();
			}

			//��������
			WORD wHeadSize = sizeof(UserGoodsUpdate) - sizeof(UserGoodsUpdate.GoodsHoldInfo);
			WORD wPacketSize = UserGoodsUpdate.wGoodsCount * sizeof(UserGoodsUpdate.GoodsHoldInfo[0]) + wHeadSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_GOODS_UPDATE, dwContextID, dwTokenID, &UserGoodsUpdate, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

//��������
bool CDataBaseEngineSink::UpdateUserLoveLiness(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID)
{
    try
    {
        //�������
        m_AccountsDBAide.ResetParameter();
        m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
        LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryUserLoveLiness"), true);

        //���سɹ�
        if (lResultCode == DB_SUCCESS)
        {
            //���²Ƹ�
			DBO_MB_LoveLinessUpdate LoveLinessUpdate;
            ZeroMemory(&LoveLinessUpdate, sizeof(LoveLinessUpdate));

            //���ñ���
			LoveLinessUpdate.dwLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));

            //���ͽ��			
            m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_USER_LOVELINESS_UPDATE, dwContextID, dwTokenID, &LoveLinessUpdate, sizeof(LoveLinessUpdate));
        }

        return true;
    }
    catch (IDataBaseException* pIException)
    {
        TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
    }

    return false;
}

//��¼�ɹ�
VOID CDataBaseEngineSink::OnLogonDisposeResult(DWORD dwContextID, DWORD dwTokenID, LONG lResultCode, LPCTSTR pszErrorString, bool bMobileClient, bool isThirdLogon )
{
	if (bMobileClient==false)
	{
		if (lResultCode==DB_SUCCESS)
		{
			//��������
			m_GPLogonSuccess.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			m_GPLogonSuccess.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_GPLogonSuccess.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			m_GPLogonSuccess.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
			m_AccountsDBAide.GetValue_String(TEXT("Accounts"),m_GPLogonSuccess.szAccounts,CountArray(m_GPLogonSuccess.szAccounts));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"),m_GPLogonSuccess.szNickName,CountArray(m_GPLogonSuccess.szNickName));
			m_AccountsDBAide.GetValue_String(TEXT("Password"),m_GPLogonSuccess.szPassword,CountArray(m_GPLogonSuccess.szPassword));
			m_GPLogonSuccess.cbMasterOrder = m_AccountsDBAide.GetValue_BYTE(TEXT("MasterOrder"));
			m_GPLogonSuccess.dwProtectID = m_AccountsDBAide.GetValue_DWORD(TEXT("ProtectID"));
			m_GPLogonSuccess.cbMoorMachine = m_AccountsDBAide.GetValue_BYTE(TEXT("MoorMachine"));

			//�û�����
			m_GPLogonSuccess.cbAge=m_AccountsDBAide.GetValue_BYTE(TEXT("Age"));
			m_GPLogonSuccess.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
			m_AccountsDBAide.GetValue_String(TEXT("Area"),m_GPLogonSuccess.szArea,CountArray(m_GPLogonSuccess.szArea));
			m_AccountsDBAide.GetValue_String(TEXT("City"),m_GPLogonSuccess.szCity,CountArray(m_GPLogonSuccess.szCity));
			m_AccountsDBAide.GetValue_String(TEXT("Province"),m_GPLogonSuccess.szProvince,CountArray(m_GPLogonSuccess.szProvince));
			m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"),m_GPLogonSuccess.szUnderWrite,CountArray(m_GPLogonSuccess.szUnderWrite));

			//����ȼ�
			m_GPLogonSuccess.dwExperience = m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));

			//��Ա����
			m_GPLogonSuccess.dwMemberOverTime= m_AccountsDBAide.GetValue_DWORD(TEXT("MemberOverTime"));			

			//�Ƹ���Ϣ
			m_GPLogonSuccess.lUserGold = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Gold"));
			m_GPLogonSuccess.lInsureGold = m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureGold"));

			//������Ϣ
			m_GPLogonSuccess.wLockGateID = m_AccountsDBAide.GetValue_WORD(TEXT("LockGateID"));
			m_GPLogonSuccess.wLockLogonID = m_AccountsDBAide.GetValue_WORD(TEXT("LockLogonID"));
			m_GPLogonSuccess.dwLockSocketID = m_AccountsDBAide.GetValue_DWORD(TEXT("LockSocketID"));

			//������Ϣ
			m_GPLogonSuccess.dwPostTime=(DWORD)time(NULL);

			//���ͽ��
			WORD wStringSize=CountStringBuffer(m_GPLogonSuccess.szDescribeString);
			WORD wSendSize=sizeof(m_GPLogonSuccess)-sizeof(m_GPLogonSuccess.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOGON_SUCCESS,dwContextID,dwTokenID,&m_GPLogonSuccess,wSendSize);
		}
		else
		{
			//��ȡ��Ϣ
			StringCchCopy(m_GPLogonFailure.szDescribeString, CountArray(m_GPLogonFailure.szDescribeString), pszErrorString);

			//���ͽ��
			WORD wStringSize=CountStringBuffer(pszErrorString);
			WORD wSendSize=sizeof(m_GPLogonFailure)-sizeof(m_GPLogonFailure.szDescribeString)+wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_LOGON_FAILURE,dwContextID,dwTokenID,&m_GPLogonFailure,wSendSize);
		}
	}
	else
	{
		if (lResultCode==DB_SUCCESS)
		{
			//��������
			m_MBLogonSuccess.wFaceID=m_AccountsDBAide.GetValue_WORD(TEXT("FaceID"));
			m_MBLogonSuccess.cbGender=m_AccountsDBAide.GetValue_BYTE(TEXT("Gender"));
			m_MBLogonSuccess.dwCustomID=m_AccountsDBAide.GetValue_DWORD(TEXT("CustomID"));
			m_MBLogonSuccess.dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_MBLogonSuccess.dwGameID=m_AccountsDBAide.GetValue_DWORD(TEXT("GameID"));
			m_MBLogonSuccess.cbInfoPublic=m_AccountsDBAide.GetValue_BYTE(TEXT("InfoPublic"));
			m_MBLogonSuccess.dwUserRight=m_AccountsDBAide.GetValue_DWORD(TEXT("UserRight"));
			m_AccountsDBAide.GetValue_String(TEXT("UniqueID"), m_MBLogonSuccess.szUniqueID, CountArray(m_MBLogonSuccess.szUniqueID));
			m_AccountsDBAide.GetValue_String(TEXT("NickName"), m_MBLogonSuccess.szNickName,CountArray(m_MBLogonSuccess.szNickName));
			m_AccountsDBAide.GetValue_String(TEXT("Accounts"), m_MBLogonSuccess.szAccounts,CountArray(m_MBLogonSuccess.szAccounts));
			m_AccountsDBAide.GetValue_String(TEXT("Password"), m_MBLogonSuccess.szLogonPass,CountArray(m_MBLogonSuccess.szLogonPass));
			m_AccountsDBAide.GetValue_String(TEXT("InsurePass"), m_MBLogonSuccess.szInsurePass,CountArray(m_MBLogonSuccess.szInsurePass));

			//�û���Կ
			TCHAR szSecretKey[33] = {0};
			m_AccountsDBAide.GetValue_String(TEXT("SecretKey"), szSecretKey, CountArray(szSecretKey));

			//�û�����
			m_AccountsDBAide.GetValue_String(TEXT("Email"), m_MBLogonSuccess.szEmail, CountArray(m_MBLogonSuccess.szEmail));
			m_AccountsDBAide.GetValue_String(TEXT("UnderWrite"), m_MBLogonSuccess.szUnderWrite, CountArray(m_MBLogonSuccess.szUnderWrite));
			m_AccountsDBAide.GetValue_String(TEXT("MobilePhone"), m_MBLogonSuccess.szMobilePhone, CountArray(m_MBLogonSuccess.szMobilePhone));

			//����ȼ�
			m_MBLogonSuccess.dwExperience = m_AccountsDBAide.GetValue_DWORD(TEXT("Experience"));

            //����ֵ
            m_MBLogonSuccess.dwLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));

			//��Ա����
			m_MBLogonSuccess.wMasterOrder = m_AccountsDBAide.GetValue_WORD(TEXT("MasterOrder"));
			m_MBLogonSuccess.dwMemberPoint = m_AccountsDBAide.GetValue_DWORD(TEXT("MemberPoint"));
			m_MBLogonSuccess.dwMemberOverTime = m_AccountsDBAide.GetValue_DWORD(TEXT("MemberOverTime"));			

			//�û��Ƹ�
			m_MBLogonSuccess.lUserGold=m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));
			m_MBLogonSuccess.lUserCard=m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserCard"));
			m_MBLogonSuccess.lUserInsure=m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureGold"));		
			m_MBLogonSuccess.lCommission=m_AccountsDBAide.GetValue_LONGLONG(TEXT("Commission"));

			//��Ϊ����
			m_MBLogonSuccess.dwActionMaskEver = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskEver"));
			m_MBLogonSuccess.dwActionMaskPerDay = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskPerDay"));
			m_MBLogonSuccess.dwActionMaskPerWeek = m_AccountsDBAide.GetValue_DWORD(TEXT("ActionMaskPerWeek"));

			//������Ϣ
			m_MBLogonSuccess.wLockGateID = m_AccountsDBAide.GetValue_WORD(TEXT("LockGateID"));
			m_MBLogonSuccess.wLockLogonID = m_AccountsDBAide.GetValue_WORD(TEXT("LockLogonID"));
			m_MBLogonSuccess.dwLockSocketID = m_AccountsDBAide.GetValue_DWORD(TEXT("LockSocketID"));
			m_MBLogonSuccess.wLockKindID = m_AccountsDBAide.GetValue_WORD(TEXT("LockKindID"));
			m_MBLogonSuccess.wLockServerID = m_AccountsDBAide.GetValue_WORD(TEXT("LockServerID"));
			m_MBLogonSuccess.wLockServerKind = m_AccountsDBAide.GetValue_WORD(TEXT("LockServerKind"));				

			//������Ϣ
			m_MBLogonSuccess.dwParentGameID = m_AccountsDBAide.GetValue_DWORD(TEXT("ParentGameID"));
			m_MBLogonSuccess.wParentFaceID = m_AccountsDBAide.GetValue_WORD(TEXT("ParentFaceID"));
			m_AccountsDBAide.GetValue_String(TEXT("ParentNickName"), m_MBLogonSuccess.szParentNickName, CountArray(m_MBLogonSuccess.szParentNickName));

			//״̬��Ϣ
			m_MBLogonSuccess.cbShutUpStatus = m_AccountsDBAide.GetValue_BYTE(TEXT("ShutUpStatus"));
			m_MBLogonSuccess.dwShutUpOverTime = m_AccountsDBAide.GetValue_DWORD(TEXT("ShutUpOverTime"));

			//ǩ����Ϣ
			m_MBLogonSuccess.wCheckInDays = m_AccountsDBAide.GetValue_WORD(TEXT("CheckInDays"));
			m_MBLogonSuccess.cbCheckInState = m_AccountsDBAide.GetValue_BYTE(TEXT("CheckInState"));

			//�ǳ��޸���Ϣ
			m_MBLogonSuccess.wNickNameModifyTimes = m_AccountsDBAide.GetValue_WORD(TEXT("NickNameModifyTimes"));
			//ͷ���ַ kk jia
			m_MBLogonSuccess.szUserFaceUrl[0] = 0;
			if (isThirdLogon)
				m_AccountsDBAide.GetValue_String(TEXT("UserFaceUrl"), m_MBLogonSuccess.szUserFaceUrl, CountArray(m_MBLogonSuccess.szUserFaceUrl));

			CString s;
			s.Format(_T("1 szNickName:%s,szUserFaceUrl:%s,isThirdLogon:%d"), m_MBLogonSuccess.szNickName,m_MBLogonSuccess.szUserFaceUrl, isThirdLogon);
			CTraceService::TraceString(s, TraceLevel_Exception);

			m_MBLogonSuccess.wNickNameDailyModifyTimes = m_AccountsDBAide.GetValue_WORD(TEXT("NickNameDailyModifyTimes"));

			//������Ϣ
			m_MBLogonSuccess.dwLikedNumber = m_AccountsDBAide.GetValue_DWORD(TEXT("LikedNumber"));
			m_MBLogonSuccess.dwRegisterTime = m_AccountsDBAide.GetValue_DWORD(TEXT("RegisterTime"));			
			m_MBLogonSuccess.cbRegisterMode  = m_AccountsDBAide.GetValue_BYTE(TEXT("RegisterMode"));
			m_MBLogonSuccess.dwUIShieldMask = m_AccountsDBAide.GetValue_DWORD(TEXT("UIShieldMask"));		
			m_AccountsDBAide.GetValue_String(TEXT("ShieldKindList"), m_MBLogonSuccess.szShieldKindList, CountArray(m_MBLogonSuccess.szShieldKindList));

			//ʵ����Ϣ
			m_AccountsDBAide.GetValue_String(TEXT("Compellation"), m_MBLogonSuccess.RealAuth.szCompellation, CountArray(m_MBLogonSuccess.RealAuth.szCompellation));
			m_AccountsDBAide.GetValue_String(TEXT("PassPortID"), m_MBLogonSuccess.RealAuth.szPassPortID, CountArray(m_MBLogonSuccess.RealAuth.szPassPortID));

			//��¼����
			DWORD dwGameLogonTimes = m_AccountsDBAide.GetValue_DWORD(TEXT("GameLogonTimes"));

			//�������
			m_MBLogonSuccess.szPresentGoodsList[0] = 0;

			//ע������
			if (dwGameLogonTimes == 1)
			{
				m_AccountsDBAide.GetValue_String(TEXT("PresentGoodsList"), m_MBLogonSuccess.szPresentGoodsList, CountArray(m_MBLogonSuccess.szPresentGoodsList));
			}

			//��ȡ��Ϣ
			if (pszErrorString != NULL)
			{
				StringCchCopy(m_MBLogonSuccess.szDescribeString, CountArray(m_MBLogonSuccess.szDescribeString), pszErrorString);
			}		

			//������Ϣ
			if (dwGameLogonTimes == 1) CacheUserInfo(dwContextID, dwTokenID);

			//������Ϣ
			LoadUserPackGoods();

			//��������
			BuildLogonToken(m_MBLogonSuccess.dwUserID, m_MBLogonSuccess.szAccounts, szSecretKey, m_MBLogonSuccess.szLogonToken);	
			
			
			//���ͽ��
			WORD wDataSize=CountStringBuffer(m_MBLogonSuccess.szDescribeString);
			WORD wHeadSize=sizeof(m_MBLogonSuccess)-sizeof(m_MBLogonSuccess.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_LOGON_SUCCESS,dwContextID,dwTokenID,&m_MBLogonSuccess,wHeadSize+wDataSize);
		}
		else
		{
			//��������
			m_MBLogonFailure.lResultCode=lResultCode;

			//������Ϣ
			if (pszErrorString != NULL)
			{
				StringCchCopy(m_MBLogonFailure.szDescribeString, CountArray(m_MBLogonFailure.szDescribeString), pszErrorString);
			}			

			//���ͽ��
			WORD wDataSize=CountStringBuffer(m_MBLogonFailure.szDescribeString);
			WORD wHeadSize=sizeof(m_MBLogonFailure)-sizeof(m_MBLogonFailure.szDescribeString);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_LOGON_FAILURE,dwContextID,dwTokenID,&m_MBLogonFailure,wHeadSize+wDataSize);
		}
	}

	return;
}
//ʵ����֤
bool CDataBaseEngineSink::OnRequestMBRequestRealAuthent(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize >= sizeof(DBR_MB_RealAuth));
		if (wDataSize < sizeof(DBR_MB_RealAuth)) return false;

		//������
		auto pRealAuth = (DBR_MB_RealAuth*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pRealAuth->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@szPassword"), pRealAuth->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@szCompellation"), pRealAuth->szCompellation);
		m_AccountsDBAide.AddParameter(TEXT("@szPassPortID"), pRealAuth->szPassPortID);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//ִ�в�ѯ
		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_RealAuth"), true) == DB_SUCCESS)
		{

			//��������
			DBO_MB_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateSuccess.wRequestCmdID = SUB_MB_REAL_AUTH;
			OperateSuccess.lResultCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wSendSize);
		}
		else
		{
			//��������
			DBO_MB_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.wRequestCmdID = SUB_MB_REAL_AUTH;
			OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
			WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}
		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = SUB_MB_REAL_AUTH;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}
};
//���в�ѯ
bool CDataBaseEngineSink::OnRequestMBRequestInsureQuery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		ASSERT(wDataSize == sizeof(DBR_MB_InsureQuery));
		DBR_MB_InsureQuery* pInfo = (DBR_MB_InsureQuery*)pData;
		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pInfo->dwUserID);

		//
		DBR_MB_InsureQueryResult InsureResult;
		ZeroMemory(&InsureResult, sizeof(InsureResult));

		if (m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_InsureQuery"), true) == DB_SUCCESS)
		{
			InsureResult.cbEnable = m_AccountsDBAide.GetValue_BYTE(TEXT("Enable"));
			InsureResult.lScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			InsureResult.lInsureScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureScore"));

			InsureResult.SaveAndTake.lReserveAmount = m_AccountsDBAide.GetValue_LONGLONG(TEXT("SaveAndTakeReserveAmount"));
			InsureResult.SaveAndTake.lDealMin = m_AccountsDBAide.GetValue_LONGLONG(TEXT("SaveAndTakeDealMin"));
			InsureResult.SaveAndTake.lDealMax = m_AccountsDBAide.GetValue_LONGLONG(TEXT("SaveAndTakeDealMax"));
			InsureResult.SaveAndTake.lDailyMax = m_AccountsDBAide.GetValue_LONGLONG(TEXT("SaveAndTakeDailyMax"));
			InsureResult.SaveAndTake.dwFeeRate = m_AccountsDBAide.GetValue_DWORD(TEXT("SaveAndTakeFeeRate"));
			
			InsureResult.Transfer.lReserveAmount = m_AccountsDBAide.GetValue_LONGLONG(TEXT("TransferReserveAmount"));
			InsureResult.Transfer.lDealMin = m_AccountsDBAide.GetValue_LONGLONG(TEXT("TransferDealMin"));
			InsureResult.Transfer.lDealMax = m_AccountsDBAide.GetValue_LONGLONG(TEXT("TransferDealMax"));
			InsureResult.Transfer.lDailyMax = m_AccountsDBAide.GetValue_LONGLONG(TEXT("TransferDailyMax"));
			InsureResult.Transfer.dwFeeRate = m_AccountsDBAide.GetValue_DWORD(TEXT("TransferFeeRate"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_QUERY_RESULT, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = DBR_MB_INSURE_QUERY;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}
};
//���п�ͨ
bool CDataBaseEngineSink::OnRequestMBRequestInsureEnable(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		ASSERT(wDataSize == sizeof(DBR_MB_InsureEnable));
		DBR_MB_InsureEnable* pInfo = (DBR_MB_InsureEnable*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pInfo->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pInfo->szPassword);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//
		DBR_MB_InsureSuccess InsureResult;
		ZeroMemory(&InsureResult, sizeof(InsureResult));

		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_InsureEnable"), true);
		if (lResultCode == DB_SUCCESS)
		{
			InsureResult.lScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			InsureResult.lInsureScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureScore"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_SUCCESS, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}
		else
		{
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			DBR_MB_InsureFailure InsureResult;
			ZeroMemory(&InsureResult, sizeof(InsureResult));

			InsureResult.dwResultCode = lResultCode;
			StrCpy(InsureResult.szDescribeString, szDescribe);

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_FAILURE, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = DBR_MB_INSURE_ENABLE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}
};
//���д洢
bool CDataBaseEngineSink::OnRequestMBRequestInsureSave(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		ASSERT(wDataSize == sizeof(DBR_MB_InsureSave));
		DBR_MB_InsureSave* pInfo = (DBR_MB_InsureSave*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pInfo->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pInfo->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@nScore"), pInfo->lSaveScore);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_INSURE);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pInfo->szMachineID);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//
		DBR_MB_InsureSuccess InsureResult;
		ZeroMemory(&InsureResult, sizeof(InsureResult));

		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_InsureSave"), true);
		if (lResultCode == DB_SUCCESS)
		{
			InsureResult.lScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			InsureResult.lInsureScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureScore"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_SUCCESS, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}
		else
		{
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			DBR_MB_InsureFailure InsureResult;
			ZeroMemory(&InsureResult, sizeof(InsureResult));

			InsureResult.dwResultCode = lResultCode;
			StrCpy(InsureResult.szDescribeString, CW2CT(DBVarValue.bstrVal).m_psz);

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_FAILURE, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = DBR_MB_INSURE_SAVE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}
};
//������ȡ
bool CDataBaseEngineSink::OnRequestMBRequestInsureTake(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		ASSERT(wDataSize == sizeof(DBR_MB_InsureTake));
		DBR_MB_InsureTake* pInfo = (DBR_MB_InsureTake*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pInfo->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pInfo->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@nScore"), pInfo->lTakeScore);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pInfo->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_INSURE);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pInfo->szMachineID);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//
		DBR_MB_InsureSuccess InsureResult;
		ZeroMemory(&InsureResult, sizeof(InsureResult));

		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_InsureTake"), true);
		if (lResultCode == DB_SUCCESS)
		{
			InsureResult.lScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			InsureResult.lInsureScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureScore"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_SUCCESS, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}
		else
		{
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			DBR_MB_InsureFailure InsureResult;
			ZeroMemory(&InsureResult, sizeof(InsureResult));

			InsureResult.dwResultCode = lResultCode;
			StrCpy(InsureResult.szDescribeString, CW2CT(DBVarValue.bstrVal).m_psz);

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_FAILURE, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = DBR_MB_INSURE_TAKE;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}
};
//����ת��
bool CDataBaseEngineSink::OnRequestMBRequestInsureTransfer(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		ASSERT(wDataSize == sizeof(DBR_MB_InsureTransfer));
		DBR_MB_InsureTransfer* pInfo = (DBR_MB_InsureTransfer*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pInfo->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pInfo->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwGameID"), pInfo->dwGameID);
		m_AccountsDBAide.AddParameter(TEXT("@nScore"), pInfo->lTransferScore);
		m_AccountsDBAide.AddParameter(TEXT("@strPassword"), pInfo->szPassword);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_INSURE);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@strMachineID"), pInfo->szMachineID);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//
		DBR_MB_InsureSuccess InsureResult;
		ZeroMemory(&InsureResult, sizeof(InsureResult));

		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_InsureTransfer"), true);
		if (lResultCode == DB_SUCCESS)
		{
			InsureResult.lScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			InsureResult.lInsureScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureScore"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_SUCCESS, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));

			//�ʼ�֪ͨ
			DBR_MB_InsureMailNotify InsureMail;
			
			InsureMail.dwUserID = m_AccountsDBAide.GetValue_DWORD(TEXT("TargetUserID"));
			InsureMail.dwMailID = m_AccountsDBAide.GetValue_DWORD(TEXT("TargetMailID"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_MAIL_NOTIFY, dwContextID, dwTokenID, &InsureMail, sizeof(InsureMail));
		}
		else
		{
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			DBR_MB_InsureFailure InsureResult;
			ZeroMemory(&InsureResult, sizeof(InsureResult));

			InsureResult.dwResultCode = lResultCode;
			StrCpy(InsureResult.szDescribeString, CW2CT(DBVarValue.bstrVal).m_psz);

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_FAILURE, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}
		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_MB_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.wRequestCmdID = DBR_MB_INSURE_TRANSFER;
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pszDescribe);

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_MB_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}
};
//������������
bool CDataBaseEngineSink::OnRequestMBInsureResetPassword(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_MB_InsureResetPassword));
		if (wDataSize != sizeof(DBR_MB_InsureResetPassword)) return false;

		//������
		DBR_MB_InsureResetPassword* pModifyInsurePass = (DBR_MB_InsureResetPassword*)pData;

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE* pClientIP = (BYTE*)&pModifyInsurePass->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pModifyInsurePass->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strScrPassword"), pModifyInsurePass->szSrcPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strDesPassword"), pModifyInsurePass->szDstPassword);
		m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);

		//�������
		TCHAR szDescribe[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

		//�������
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_MB_ModifyInsurePassword"), true);
		if (lResultCode == DB_SUCCESS)
		{
			DBR_MB_InsureSuccess InsureResult;
			ZeroMemory(&InsureResult, sizeof(InsureResult));

			InsureResult.lScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Score"));
			InsureResult.lInsureScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("InsureScore"));

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_SUCCESS, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}
		else
		{
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			DBR_MB_InsureFailure InsureResult;
			ZeroMemory(&InsureResult, sizeof(InsureResult));

			InsureResult.dwResultCode = lResultCode;
			StrCpy(InsureResult.szDescribeString, CW2CT(DBVarValue.bstrVal).m_psz);

			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBR_MB_INSURE_FAILURE, dwContextID, dwTokenID, &InsureResult, sizeof(InsureResult));
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = pIException->GetExceptionResult();
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0005, L"����ϵͳ��æ�����Ժ����ԣ�"));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GP_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

		return false;
	}
};
//////////////////////////////////////////////////////////////////////////

