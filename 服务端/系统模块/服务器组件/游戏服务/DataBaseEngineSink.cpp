#include "StdAfx.h"
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//�����Ϣ
void TraceDatabaseException(char* pFile, char* pFunction, int nLine, IDataBaseException* pIException)
{
	CString strMessage;
	strMessage.Format(TEXT("%s(%d):%hs(%s)"), CA2CT(pFile).m_psz, nLine, pFunction, pIException ? pIException->GetExceptionDescribe() : TEXT(""));
	CTraceService::TraceString(strMessage, TraceLevel_Exception);
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//��������
	m_nSyncIndex = -1;

	//ϵͳ����
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//��չ�ӿ�
	m_pIExtendDataBaseSink = NULL;	

	//�������
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pIDataBaseEngineEvent=NULL;	
	m_pAttemperEngineSink = NULL;	
	
	//��������
	ZeroMemory(&m_LogonFailure,sizeof(m_LogonFailure));
	ZeroMemory(&m_LogonSuccess,sizeof(m_LogonSuccess));

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
	if ((m_PlatformDBModule.GetInterface()==NULL)&&(m_PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if ((m_AccountsDBModule.GetInterface() == NULL) && (m_AccountsDBModule.CreateInstance() == false))
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if ((m_GameDBModule.GetInterface()==NULL)&&(m_GameDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	try
	{
		//��������
		tagDataBaseParameter * pPlatformDBParameter=&m_pInitParameter->m_PlatformDBParameter;
		tagDataBaseParameter * pAccountsDBParameter=&m_pInitParameter->m_AccountsDBParameter;

		//������Ϣ
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->dwDataBaseAddr,pPlatformDBParameter->wDataBasePort,
		pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);	

		//������Ϣ
		m_AccountsDBModule->SetConnectionInfo(pAccountsDBParameter->dwDataBaseAddr, pAccountsDBParameter->wDataBasePort,
			pAccountsDBParameter->szDataBaseName, pAccountsDBParameter->szDataBaseUser, pAccountsDBParameter->szDataBasePass);

		//��������
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

		//��������
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//������Ϣ
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wDBListID"),m_pGameServiceOption->wDBListID);
		if (m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadDataBaseInfo"),true)==DB_SUCCESS)
		{
			//��������
			TCHAR szDBUser[32]=TEXT(""),szDBPassword[32]=TEXT("");
			TCHAR szDBUserRead[512]=TEXT(""),szDBPassRead[512]=TEXT("");
			TCHAR szDBName[32] = TEXT("");
			TCHAR szDBAddr[16] = TEXT("");

			//��ȡ��Ϣ
			WORD wDBPort=m_PlatformDBAide.GetValue_WORD(TEXT("DBPort"));
			m_PlatformDBAide.GetValue_String(TEXT("DBUser"),szDBUserRead,CountArray(szDBUserRead));
			m_PlatformDBAide.GetValue_String(TEXT("DBPassword"),szDBPassRead,CountArray(szDBPassRead));
			m_PlatformDBAide.GetValue_String(TEXT("DataBaseName"),szDBName,CountArray(szDBName));
			m_PlatformDBAide.GetValue_String(TEXT("DataBaseAddr"),szDBAddr,CountArray(szDBAddr));
					
			//��������
			CXOREncrypt::CrevasseData(szDBUserRead,szDBUser,CountArray(szDBUser));
			CXOREncrypt::CrevasseData(szDBPassRead,szDBPassword,CountArray(szDBPassword));

			//�������ݿ�
			m_GameDBModule->SetConnectionInfo(szDBAddr,wDBPort,szDBName,szDBUser,szDBPassword);

			//��������
			m_GameDBModule->OpenConnection();
			m_GameDBAide.SetDataBase(m_GameDBModule.GetInterface());
		}
		else
		{
			//������Ϣ
			TCHAR szErrorDescribe[128]=TEXT("");
			m_PlatformDBAide.GetValue_String(TEXT("ErrorDescribe"),szErrorDescribe,CountArray(szErrorDescribe));

			//��ʾ��Ϣ
			CTraceService::TraceString(szErrorDescribe,TraceLevel_Exception);

			return false;
		}

		//��Ϸ����
		if (m_pIGameDataBaseSink != NULL)
		{
			//��������
			tagGameDBSinkParameter GameDBSinkParameter;
			ZeroMemory(&GameDBSinkParameter, sizeof(GameDBSinkParameter));

			//ƽ̨���ݿ�
			GameDBSinkParameter.pPlatformDBAide = &m_PlatformDBAide;
			GameDBSinkParameter.pIPlatformDBModule = m_PlatformDBModule.GetInterface();

			//���ñ���
			GameDBSinkParameter.pGameServiceAttrib = m_pGameServiceAttrib;
			GameDBSinkParameter.pGameServiceOption = m_pGameServiceOption;

			//�������
			GameDBSinkParameter.pITimerEngine = m_pITimerEngine;
			GameDBSinkParameter.pIDataBaseEngine = m_pIDataBaseEngine;
			GameDBSinkParameter.pIDataBaseEngineEvent = m_pIDataBaseEngineEvent;

			//��ʼ��
			m_pIGameDataBaseSink->InitGameDataBaseSink(GameDBSinkParameter);
		}

		//��չ����
		if (m_pIExtendDataBaseSink != NULL)
		{
			//��������
			tagExtendDBSinkParameter ExtendDBSinkParameter;
			ZeroMemory(&ExtendDBSinkParameter, sizeof(ExtendDBSinkParameter));

			//��Ϸ���ݿ�
			ExtendDBSinkParameter.pGameDBAide = &m_GameDBAide;
			ExtendDBSinkParameter.pIGameDBModule = m_GameDBModule.GetInterface();

			//�˺����ݿ�
			ExtendDBSinkParameter.pAccountsDBAide = &m_AccountsDBAide;
			ExtendDBSinkParameter.pIAccountsDBModule = m_AccountsDBModule.GetInterface();

			//ƽ̨���ݿ�
			ExtendDBSinkParameter.pPlatformDBAide = &m_PlatformDBAide;
			ExtendDBSinkParameter.pIPlatformDBModule = m_PlatformDBModule.GetInterface();

			//���ñ���
			ExtendDBSinkParameter.pGameServiceAttrib = m_pGameServiceAttrib;
			ExtendDBSinkParameter.pGameServiceOption = m_pGameServiceOption;

			//�������
			ExtendDBSinkParameter.pITimerEngine = m_pITimerEngine;
			ExtendDBSinkParameter.pIDataBaseEngine = m_pIDataBaseEngine;
			ExtendDBSinkParameter.pIDataBaseEngineEvent = m_pIDataBaseEngineEvent;

			//��ʼ��
			m_pIExtendDataBaseSink->InitExtendDataBaseSink(ExtendDBSinkParameter);
		}		
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
		return false;
	}

	return true;
}

//ֹͣ�¼�
bool CDataBaseEngineSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	//��������
	UnlockRobotUser();

	//�������
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pIDataBaseEngineEvent=NULL;
	m_pAttemperEngineSink = NULL;
	
	//�ر�����
	if (m_GameDBModule.GetInterface()!=NULL)
	{
		m_GameDBModule->CloseConnection();
		m_GameDBModule.CloseInstance();
	}

	//�ر�����
	if (m_AccountsDBModule.GetInterface() != NULL)
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

	//�ͷŽӿ�
	if (m_pIGameDataBaseSink != NULL)
	{
		SafeRelease(m_pIGameDataBaseSink);
	}

	//�ͷŽӿ�
	if (m_pIExtendDataBaseSink != NULL)
	{
		SafeRelease(m_pIExtendDataBaseSink);
	}		

	return true;
}

//ʱ���¼�
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//��Ϸ����
	if (m_pIGameDataBaseSink != NULL)
	{
		m_pIGameDataBaseSink->OnDataBaseEngineTimer(dwTimerID, dwBindParameter);
	}
	

	//��չ����
	if (m_pIExtendDataBaseSink != NULL)
	{
		m_pIExtendDataBaseSink->OnDataBaseEngineTimer(dwTimerID, dwBindParameter);
	}

	return true;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	//��չ����
	if (m_pIGameDataBaseSink != NULL)
	{
		m_pIGameDataBaseSink->OnDataBaseEngineControl(wControlID, pData, wDataSize);
	}

	//��չ����
	if (m_pIExtendDataBaseSink != NULL)
	{
		m_pIExtendDataBaseSink->OnDataBaseEngineControl(wControlID, pData, wDataSize);
	}

	return true;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//��Ϸ����
	if (wRequestID > DB_GAME_REQUEST_START && wRequestID <= DB_GAME_REQUEST_START + DB_GAME_IDENTIFY_RANGE)
	{
		if (m_pIGameDataBaseSink)
		{
			return m_pIGameDataBaseSink->OnDataBaseEngineRequest(wRequestID, dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
	}

	//��չ����
	if (wRequestID > DB_EXTEND_REQUEST_START && wRequestID <= DB_EXTEND_REQUEST_START + DB_EXTEND_IDENTIFY_RANGE)
	{
		if (m_pIExtendDataBaseSink)
		{
			return m_pIExtendDataBaseSink->OnDataBaseEngineRequest(wRequestID, dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
	}

	//�������
	if (wRequestID > DB_FRAME_REQUEST_START && wRequestID <= DB_FRAME_REQUEST_START + DB_FRAME_IDENTIFY_RANGE)
	{
		switch (wRequestID)
		{
		case DBR_GR_LOAD_ROBOTS:			//���ػ���
		{
			return OnRequestLoadRobots(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_LOAD_CONFIG:			//��������
		{
			return OnRequestLoadConfig(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_RECORD_DRAW_INFO:		//��Ϸ��¼
		{
			return OnRequestRecordDrawInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_RECORD_DRAW_SCORE:		//��Ϸ��¼
		{
			return OnRequestRecordDrawScore(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_RECORD_GAME_VIDEO:		//��Ϸ¼��
		{
			return OnRequestRecordGameVideo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_MANAGE_USER_RIGHT:		//�û�Ȩ�޹���
		{
			return OnRequestManageUserRight(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_SEND_GIFT:				//��������
		{
			return OnRequestSendGift(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_UPDATE_LOVELINESS:		//��������
		{
			return OnRequestUpdateLoveLiness(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_LOGON_COMPUTER:			//���Ե�¼
		{
			return OnRequestLogonComputer(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_LOGON_MOBILE:			//�ֻ���¼
		{
			return OnRequestLogonMobile(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_LOGON_ROBOT:			//������¼
		{
			return OnRequestLogonRobot(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_WRITE_GAME_SCORE:		//��Ϸд��
		{
			return OnRequestWriteGameScore(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_WRITE_USER_WEALTH:		//�û��Ƹ�
		{
			return OnRequestWriteUserWealth(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_LARGESS_SUBSIDY:		//���͵ͱ�
		{
			return OnRequestLargessSubsidy(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_LEAVE_SERVICE:			//�뿪����
		{
			return OnRequestLeaveService(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_CONTROL_QUERY:			//���Ʋ�ѯ
		{
			return OnRequestControlQuery(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_CONTROL_START:			//���ƿ�ʼ
		{
			return OnRequestControlStart(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_CONTROL_FINISH:			//���ƽ���
		{
			return OnRequestControlFinish(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_CONTROL_UPDATE:			//���Ƹ���
		{
			return OnRequestControlUpdate(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_USER_QUERY_GOODS:		//��ѯ��Ʒ
		{
			return OnRequestUserQueryGoods(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_USER_UPDATE_GOODS:		//������Ʒ
		{
			return OnRequestUserUpdateGoods(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_USER_RECORD_GOODS:		//��¼��Ʒ
		{
			return OnRequestUserRecordGoods(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		}
	}	

	return true;
}

//���Ե�¼
bool CDataBaseEngineSink::OnRequestLogonComputer(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize>=sizeof(DBR_GR_LogonComputer));
		if (wDataSize<sizeof(DBR_GR_LogonComputer)) return false;

		//Ч������
		ASSERT(m_pAttemperEngineSink);
		if ((m_pAttemperEngineSink!=NULL)&&(m_pAttemperEngineSink->EfficacySocketContext(dwContextID,dwTokenID)==false))
		{
			return true;
		}

		//ִ�в�ѯ
		DBR_GR_LogonComputer * pLogonComputer =(DBR_GR_LogonComputer *)pData;
		ZeroMemory(&m_LogonSuccess,sizeof(m_LogonSuccess));

		//ת����ַ
		TCHAR szClientIP[16]=TEXT("");
 		BYTE * pClientIP=(BYTE *)&pLogonComputer->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"), pLogonComputer->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@szPassword"), pLogonComputer->szPassword);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@dwRequestScore"), pLogonComputer->lRequestScore);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"), pLogonComputer->szMachineID);

		//��������
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		{
			//���ò���
			m_GameDBAide.AddParameter(TEXT("@MatchID"), pLogonComputer->dwMatchID);
			m_GameDBAide.AddParameter(TEXT("@MatchNo"), pLogonComputer->lMatchNo);
		}

		//�������
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyComputer"),true);

		//������Ϣ
		m_LogonSuccess.lMatchNo= pLogonComputer->lMatchNo;
		m_LogonSuccess.dwMatchID= pLogonComputer->dwMatchID;

		//��������
		StringCchCopy(m_LogonSuccess.szMachineID, LEN_MACHINE_ID, pLogonComputer->szMachineID);
	
		//�������
		OnLogonDisposeResult(dwContextID,dwTokenID,lReturnValue,false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		OnLogonDisposeDBError(dwContextID,dwTokenID,pIException);

		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}
	catch(...)
	{
		//������
		OnLogonDisposeDBError(dwContextID,dwTokenID,NULL);

		return false;
	}

	return true;
}

//I D ��¼
bool CDataBaseEngineSink::OnRequestLogonMobile(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize>=sizeof(DBR_GR_LogonMobile));
		if (wDataSize<sizeof(DBR_GR_LogonMobile)) return false;

		//Ч������
		ASSERT(m_pAttemperEngineSink);
		if ((m_pAttemperEngineSink != NULL) && (m_pAttemperEngineSink->EfficacySocketContext(dwContextID, dwTokenID) == false))
		{
			return true;
		}

		//��ȡ����
		DBR_GR_LogonMobile * pLogonMobile=(DBR_GR_LogonMobile *)pData;		
		ZeroMemory(&m_LogonSuccess, sizeof(m_LogonSuccess));

		//ת����ַ
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonMobile->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pLogonMobile->dwUserID);		
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@dwRequestScore"),pLogonMobile->lRequestScore);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"), pLogonMobile->szMachineID);
		m_GameDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonMobile->dwDeviceKind);

		//��������
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		{
			//���ò���
			m_GameDBAide.AddParameter(TEXT("@MatchID"),pLogonMobile->dwMatchID);
			m_GameDBAide.AddParameter(TEXT("@MatchNo"),pLogonMobile->lMatchNo);
		}

		//Լս����
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE) != 0)
		{
			m_GameDBAide.AddParameter(TEXT("@dwClubID"), pLogonMobile->dwClubID);
			m_GameDBAide.AddParameter(TEXT("@dwBattleNum"), pLogonMobile->dwBattleNum);			
			m_GameDBAide.AddParameter(TEXT("@cbScoreKind"), pLogonMobile->cbScoreKind);
		}

		//��λ����
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_RANKING) != 0)
		{
			m_GameDBAide.AddParameter(TEXT("@dwRankingNum"), pLogonMobile->dwRankingNum);
		}

		//�������
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);	

		//ִ�в�ѯ
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyMobile"),true);

		//�ֻ�����
		m_LogonSuccess.dwDeviceKind=pLogonMobile->dwDeviceKind;		

		//λ����Ϣ
		m_LogonSuccess.dGlobalPosLat=pLogonMobile->dGlobalPosLat;
		m_LogonSuccess.dGlobalPosLng=pLogonMobile->dGlobalPosLng;
		CopyMemory(m_LogonSuccess.szPlaceName, pLogonMobile->szPlaceName, sizeof(m_LogonSuccess.szPlaceName));

		//������Ϣ
		m_LogonSuccess.lMatchNo=pLogonMobile->lMatchNo;
		m_LogonSuccess.dwMatchID=pLogonMobile->dwMatchID;	

		//������Ϣ
		m_LogonSuccess.bLookon = pLogonMobile->bLookon;
		m_LogonSuccess.wTableID= pLogonMobile->wTableID;
		m_LogonSuccess.wChairID = pLogonMobile->wChairID;
		StringCchCopy(m_LogonSuccess.szTablePasswd, CountArray(m_LogonSuccess.szTablePasswd), pLogonMobile->szTablePasswd);

		//�û���Ϣ
		StringCchCopy(m_LogonSuccess.szMachineID,CountArray(m_LogonSuccess.szMachineID), pLogonMobile->szMachineID);
	
		//�������
		OnLogonDisposeResult(dwContextID,dwTokenID,lResultCode,true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		OnLogonDisposeDBError(dwContextID,dwTokenID,pIException);

		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}
	catch(...)
	{
		//������
		OnLogonDisposeDBError(dwContextID,dwTokenID,NULL);

		return false;
	}

	return true;
}

//������¼
bool CDataBaseEngineSink::OnRequestLogonRobot(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize >= sizeof(DBR_GR_LogonRobot));
		if (wDataSize < sizeof(DBR_GR_LogonRobot)) return false;

		//Ч������
		ASSERT (m_pAttemperEngineSink);
		if ((m_pAttemperEngineSink != NULL) && (m_pAttemperEngineSink->EfficacySocketContext(dwContextID,dwTokenID) == false))
		{
			return true;
		}

		//ִ�в�ѯ
		DBR_GR_LogonRobot * pLogonRobot = (DBR_GR_LogonRobot *)pData;
		ZeroMemory(&m_LogonSuccess, sizeof(m_LogonSuccess));

		//ת����ַ
		TCHAR szClientIP[16] = TEXT("");
		BYTE * pClientIP = (BYTE *)&pLogonRobot->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"), pLogonRobot->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@dwRequestScore"), pLogonRobot->lRequestScore);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"), pLogonRobot->szMachineID);

		//��������
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH) != 0)
		{
			//���ò���
			m_GameDBAide.AddParameter(TEXT("@MatchID"), pLogonRobot->dwMatchID);
			m_GameDBAide.AddParameter(TEXT("@MatchNo"), pLogonRobot->lMatchNo);
		}

		//�������
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), m_LogonFailure.szDescribeString, sizeof(m_LogonFailure.szDescribeString), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyRobot"), true);

		//������Ϣ
		m_LogonSuccess.lMatchNo = pLogonRobot->lMatchNo;
		m_LogonSuccess.dwMatchID = pLogonRobot->dwMatchID;		

		//��������
		StringCchCopy(m_LogonSuccess.szMachineID, LEN_MACHINE_ID, pLogonRobot->szMachineID);

		//�������
		OnLogonDisposeResult(dwContextID, dwTokenID, lReturnValue, false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		OnLogonDisposeDBError(dwContextID, dwTokenID, pIException);

		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}
	catch (...)
	{
		//������
		OnLogonDisposeDBError(dwContextID, dwTokenID, NULL);

		return false;
	}

	return true;
}

//��Ϸд��
bool CDataBaseEngineSink::OnRequestWriteGameScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_WriteGameScore));
		if (wDataSize!=sizeof(DBR_GR_WriteGameScore)) return false;

		//������
		DBR_GR_WriteGameScore * pWriteGameScore=(DBR_GR_WriteGameScore *)pData;

		//ת����ַ
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pWriteGameScore->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pWriteGameScore->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@lScore"),pWriteGameScore->lScore);
		m_GameDBAide.AddParameter(TEXT("@lRevenue"),pWriteGameScore->lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lWinCount"),pWriteGameScore->lWinCount);
		m_GameDBAide.AddParameter(TEXT("@lLostCount"),pWriteGameScore->lLostCount);
		m_GameDBAide.AddParameter(TEXT("@lDrawCount"),pWriteGameScore->lDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lFleeCount"),pWriteGameScore->lFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lWinStreak"), pWriteGameScore->lWinStreak);
		m_GameDBAide.AddParameter(TEXT("@lMaxWinStreak"), pWriteGameScore->lMaxWinStreak);
		m_GameDBAide.AddParameter(TEXT("@lExperience"),pWriteGameScore->lExperience);
		m_GameDBAide.AddParameter(TEXT("@lUnLockScore"),pWriteGameScore->lUnLockScore);
		m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"),pWriteGameScore->dwPlayTimeCount);

		//��������
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//Լս��Ϸ
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
		{
			m_GameDBAide.AddParameter(TEXT("@dwInoutIndex"), pWriteGameScore->dwInoutIndex);			
		}

		//������Ϸ
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		{
			m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pWriteGameScore->dwMatchID);
			m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pWriteGameScore->lMatchNo);
		}

		//ִ�в�ѯ
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_WriteGameScore"),false);

		return lReturnValue==DB_SUCCESS;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//�û��Ƹ�
bool CDataBaseEngineSink::OnRequestWriteUserWealth(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//ִ�в�ѯ
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GR_WriteGameWealth));
		if (wDataSize != sizeof(DBR_GR_WriteGameWealth)) return false;

		//������
		DBR_GR_WriteGameWealth* pWriteGameWealth = (DBR_GR_WriteGameWealth*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();

		//�û���Ϣ
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pWriteGameWealth->dwUserID);

		//�����Ϣ
		m_AccountsDBAide.AddParameter(TEXT("@wMoneyID"), pWriteGameWealth->wMoneyID);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), pWriteGameWealth->wReason);
		m_AccountsDBAide.AddParameter(TEXT("@lVariationScore"), pWriteGameWealth->lVariationScore);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_WriteGameWealth"), true);

		//ִ�гɹ�
		if (lResultCode == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_UserWealthUpdate UserWealthUpdate;
			ZeroMemory(&UserWealthUpdate, sizeof(UserWealthUpdate));

			//���ñ���
			UserWealthUpdate.wMoneyID = pWriteGameWealth->wMoneyID;
			UserWealthUpdate.dwUserID = pWriteGameWealth->dwUserID;
			UserWealthUpdate.lBalanceAmount = m_AccountsDBAide.GetValue_LONGLONG(TEXT("BalanceAmount"));

			//��������			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE, dwContextID, dwTokenID, &UserWealthUpdate, sizeof(UserWealthUpdate));
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__,__FUNCTION__, __LINE__, pIException);	

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return false;
}

//���͵ͱ�
bool CDataBaseEngineSink::OnRequestLargessSubsidy(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_LargessSubsidy));
		if (wDataSize!=sizeof(DBR_GR_LargessSubsidy)) return false;

		//������
		DBR_GR_LargessSubsidy * pLargessSubsidy=(DBR_GR_LargessSubsidy *)pData;

		//ת����ַ
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pLargessSubsidy->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pLargessSubsidy->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwClientIP"),pLargessSubsidy->dwClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@szClientIP"),szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@szMachine"),pLargessSubsidy->szMachineID);

		//�������
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue= m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_LargessSubsidy"),true);

		//��������
		DBO_GR_LargessSubsidy LargessSubsidy;
		ZeroMemory(&LargessSubsidy,sizeof(LargessSubsidy));

		//���ñ���
		LargessSubsidy.bResultCode=lReturnValue==DB_SUCCESS;
		LargessSubsidy.dwUserID=pLargessSubsidy->dwUserID;

		//ִ�гɹ�
		if (LargessSubsidy.bResultCode==true)
		{
			LargessSubsidy.lCurrGold= m_AccountsDBAide.GetValue_LONGLONG(TEXT("CurrGold"));
			LargessSubsidy.cbResidualTimes= m_AccountsDBAide.GetValue_BYTE(TEXT("ResidualTimes"));
		}
		else
		{
			//��������
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			StringCchCopy(LargessSubsidy.szDescribeString,CountArray(LargessSubsidy.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);
		}

		//���ͽ��
		WORD wStringSize=CountStringBuffer(LargessSubsidy.szDescribeString);
		WORD wSendDataSize=sizeof(LargessSubsidy)-sizeof(LargessSubsidy.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LARGESS_SUBSIDY,dwContextID,dwTokenID,&LargessSubsidy,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//��ѯ��Ʒ
bool CDataBaseEngineSink::OnRequestUserQueryGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//��ȡ����
		DBR_GR_UserQueryGoods* pUserQueryGoods = (DBR_GR_UserQueryGoods*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUserQueryGoods->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@szGoodsIDList"), pUserQueryGoods->szGoodsIDList);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserQueryGoods"), true);

		//ִ�гɹ�
		if (lResultCode == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_UserGoodsUpdate UserGoodsUpdate;
			ZeroMemory(&UserGoodsUpdate, sizeof(UserGoodsUpdate));

			//ѭ����ȡ
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				//��Ʒ��Ϣ
				UserGoodsUpdate.GoodsHoldInfo[UserGoodsUpdate.wGoodsCount].wGoodsID = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsID"));
				UserGoodsUpdate.GoodsHoldInfo[UserGoodsUpdate.wGoodsCount].wGoodsKind = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsKind"));
				UserGoodsUpdate.GoodsHoldInfo[UserGoodsUpdate.wGoodsCount].wGoodsLevel = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsLevel"));
				UserGoodsUpdate.GoodsHoldInfo[UserGoodsUpdate.wGoodsCount].dwGoodsCount = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsCount"));
				UserGoodsUpdate.GoodsHoldInfo[UserGoodsUpdate.wGoodsCount].dwGoodsIndate = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsIndate"));
				UserGoodsUpdate.GoodsHoldInfo[UserGoodsUpdate.wGoodsCount].dwExpireTime = m_AccountsDBAide.GetValue_DWORD(TEXT("ExpireTime"));

				//��Ʒ����
				UserGoodsUpdate.wGoodsCount++;

				//������¼
				m_AccountsDBModule->MoveToNext();
			}

			//��������
			WORD wHeadSize = sizeof(UserGoodsUpdate) - sizeof(UserGoodsUpdate.GoodsHoldInfo);
			WORD wDataSize = UserGoodsUpdate.wGoodsCount * sizeof(UserGoodsUpdate.GoodsHoldInfo[0]);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_GOODS_UPDATE, dwContextID, dwTokenID, &UserGoodsUpdate, wHeadSize + wDataSize);
		}

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

//������Ʒ	
bool CDataBaseEngineSink::OnRequestUserUpdateGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//��ȡ����
		DBR_GR_UserUpdateGoods* pUserUpdateGoods = (DBR_GR_UserUpdateGoods*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUserUpdateGoods->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strGoodsList"), pUserUpdateGoods->szGoodsList);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserUpdateGoods"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__,__FUNCTION__, __LINE__, pIException);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//��¼��Ʒ	
bool CDataBaseEngineSink::OnRequestUserRecordGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//��ȡ����
		DBR_GR_UserRecordGoods* pUserRecordGoods = (DBR_GR_UserRecordGoods*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUserRecordGoods->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strRecordList"), pUserRecordGoods->szRecordList);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserRecordGoods"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__,__FUNCTION__, __LINE__, pIException);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//�뿪����
bool CDataBaseEngineSink::OnRequestLeaveService(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_LeaveGameService));
		if (wDataSize!=sizeof(DBR_GR_LeaveGameService)) return false;

		//������
		DBR_GR_LeaveGameService * pLeaveGameService=(DBR_GR_LeaveGameService *)pData;

		//ת����ַ
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pLeaveGameService->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pLeaveGameService->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@lScore"),pLeaveGameService->lScore);		
		m_GameDBAide.AddParameter(TEXT("@lRevenue"),pLeaveGameService->lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lWinCount"),pLeaveGameService->lWinCount);
		m_GameDBAide.AddParameter(TEXT("@lLostCount"),pLeaveGameService->lLostCount);
		m_GameDBAide.AddParameter(TEXT("@lDrawCount"),pLeaveGameService->lDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lFleeCount"),pLeaveGameService->lFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lWinStreak"), pLeaveGameService->lWinStreak);
		m_GameDBAide.AddParameter(TEXT("@lMaxWinStreak"), pLeaveGameService->lMaxWinStreak);
		m_GameDBAide.AddParameter(TEXT("@lExperience"),pLeaveGameService->lExperience);
		m_GameDBAide.AddParameter(TEXT("@lUnLockScore"),pLeaveGameService->lUnLockScore);
		m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"),pLeaveGameService->dwPlayTimeCount);
		m_GameDBAide.AddParameter(TEXT("@dwOnLineTimeCount"),pLeaveGameService->dwOnLineTimeCount);
		
		//��¼�ɼ�
		m_GameDBAide.AddParameter(TEXT("@lRecordScore"),pLeaveGameService->RecordInfo.VariationScore.lScore);
		m_GameDBAide.AddParameter(TEXT("@lRecordRevenue"),pLeaveGameService->RecordInfo.lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lRecordWinCount"),pLeaveGameService->RecordInfo.VariationScore.lWinCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordLostCount"),pLeaveGameService->RecordInfo.VariationScore.lLostCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordDrawCount"),pLeaveGameService->RecordInfo.VariationScore.lDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordFleeCount"),pLeaveGameService->RecordInfo.VariationScore.lFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordExperience"),pLeaveGameService->RecordInfo.VariationScore.lExperience);
		m_GameDBAide.AddParameter(TEXT("@dwRecordPlayTimeCount"),pLeaveGameService->RecordInfo.dwPlayTimeCount);

		//��������
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@szMachineID"),pLeaveGameService->szMachineID);
		m_GameDBAide.AddParameter(TEXT("@cbLeaveReason"),pLeaveGameService->cbLeaveReason);
		m_GameDBAide.AddParameter(TEXT("@dwInoutIndex"),pLeaveGameService->dwInoutIndex);		
		
		//ִ�в�ѯ
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LeaveGameServer"),false);

		return lReturnValue==DB_SUCCESS;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//���Ʋ�ѯ
bool CDataBaseEngineSink::OnRequestControlQuery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(DBR_GR_ControlQuery));
	if (wDataSize != sizeof(DBR_GR_ControlQuery)) return false;

	//��ȡ����
	auto pControlQuery = (DBR_GR_ControlQuery*)pData;

	//������
	DBO_GR_UserControlInfo UserControlInfo;
	ZeroMemory(&UserControlInfo,sizeof(UserControlInfo));

	//���ؿ���
	if (LoadControlInfo(pControlQuery->dwUserID, UserControlInfo.UserControlItem))
	{
		//�û���ʶ
		UserControlInfo.dwUserID = pControlQuery->dwUserID;

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_CONTROL_INFO, dwContextID, dwTokenID, &UserControlInfo, sizeof(UserControlInfo));
	}
	else
	{
		*pdwControlCode |= DB_RESULT_DEFER_DATA;
	}

	return true;
}

//���ƿ�ʼ
bool CDataBaseEngineSink::OnRequestControlStart(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//Ч�����
	ASSERT (wDataSize == sizeof(DBR_GR_ControlStart));
	if (wDataSize != sizeof(DBR_GR_ControlStart)) return false;

	//��ȡ����
	DBR_GR_ControlStart * pControlStart = (DBR_GR_ControlStart *)pData;

	try
	{
		//���ò���
		m_GameDBAide.ResetParameter();		
		m_GameDBAide.AddParameter(TEXT("@dwUserID"), pControlStart->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@cbControlMode"), pControlStart->UserControlItem.cbControlMode);
		m_GameDBAide.AddParameter(TEXT("@cbControlKind"), pControlStart->UserControlItem.cbControlKind);
		m_GameDBAide.AddParameter(TEXT("@cbControlStatus"), pControlStart->UserControlItem.cbControlStatus);
		m_GameDBAide.AddParameter(TEXT("@wTargetWinRate"), pControlStart->UserControlItem.wTargetWinRate);
		m_GameDBAide.AddParameter(TEXT("@wControlWinRate"), pControlStart->UserControlItem.wControlWinRate);
		m_GameDBAide.AddParameter(TEXT("@wInitailWinRate"), pControlStart->wAddupWinRate);		
		m_GameDBAide.AddParameter(TEXT("@lInitailWinScore"), pControlStart->lAddupWinScore);
		m_GameDBAide.AddParameter(TEXT("@lInitailLoseScore"), pControlStart->lAddupLoseScore);

		//ִ������
		m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_UserControlStart"), false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return true;
	}

	return true;
}

//���ƽ���
bool CDataBaseEngineSink::OnRequestControlFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//Ч�����
	ASSERT (wDataSize == sizeof(DBR_GR_ControlFinish));
	if (wDataSize != sizeof(DBR_GR_ControlFinish)) return false;

	//��ȡ����
	DBR_GR_ControlFinish * pControlFinish = (DBR_GR_ControlFinish *)pData;

	try
	{
		//���ò���
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"), pControlFinish->dwUserID);		
		m_GameDBAide.AddParameter(TEXT("@lTotalWinCount"), pControlFinish->lWinCount);
		m_GameDBAide.AddParameter(TEXT("@lTotalLoseCount"), pControlFinish->lLoseCount);
		m_GameDBAide.AddParameter(TEXT("@lTotalWinScore"), pControlFinish->lWinScore);
		m_GameDBAide.AddParameter(TEXT("@lTotalLoseScore"), pControlFinish->lLoseScore);
		m_GameDBAide.AddParameter(TEXT("@lTotalBetAmount"), pControlFinish->lBetAmount);

		//ִ������
		m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_UserControlFinish"), false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return true;
	}

	return true;
}

//���Ƹ���
bool CDataBaseEngineSink::OnRequestControlUpdate(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//Ч�����
	ASSERT (wDataSize == sizeof(DBR_GR_ControlUpdate));
	if (wDataSize != sizeof(DBR_GR_ControlUpdate)) return false;

	//��ȡ����
	DBR_GR_ControlUpdate * pControlUpdate = (DBR_GR_ControlUpdate *)pData;

	try
	{
		//���ò���
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"), pControlUpdate->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@lTotalWinCount"), pControlUpdate->lWinCount);
		m_GameDBAide.AddParameter(TEXT("@lTotalLoseCount"), pControlUpdate->lLoseCount);
		m_GameDBAide.AddParameter(TEXT("@lTotalWinScore"), pControlUpdate->lWinScore);
		m_GameDBAide.AddParameter(TEXT("@lTotalLoseScore"), pControlUpdate->lLoseScore);
		m_GameDBAide.AddParameter(TEXT("@lTotalBetAmount"), pControlUpdate->lBetAmount);

		//ִ������
		LONG lResultCode = m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_UserControlUpdate"), true);

		//ִ�гɹ�
		if (lResultCode == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_UserControlUpdate UserControlUpdate;
			ZeroMemory(&UserControlUpdate,sizeof(UserControlUpdate));

			//��Ʒ��Ϣ
			UserControlUpdate.dwUserID = pControlUpdate->dwUserID;
			UserControlUpdate.cbControlStatus = m_GameDBAide.GetValue_BYTE(TEXT("ControlStatus"));

			//Ͷ����Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_CONTROL_UPDATE, dwContextID, dwTokenID, &UserControlUpdate, sizeof(UserControlUpdate));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return true;
	}

	return true;
}

//ȫ������
bool CDataBaseEngineSink::LoadGlobalOption(DWORD dwContextID, DWORD dwTokenID, DWORD dwOptionMask)
{
	//��������
	TCHAR szOptionData[2048] = { 0 };

	try
	{
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

//Լս����
bool CDataBaseEngineSink::LoadBattleOption(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		LONG lReturnValue = m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LoadBattleGameOption"), true);

		//���سɹ�
		if (lReturnValue == DB_SUCCESS && m_GameDBModule->IsRecordsetEnd() == false)
		{
			//��������
			WORD wPacketSize = 0;
			BYTE cbBuffer[MAX_ASYNC_DATA];

			//�汾����
			while (m_GameDBModule->IsRecordsetEnd() == false)
			{
				//��������
				tagBattleGameOptionEx* pBattleGameOption = (tagBattleGameOptionEx*)&cbBuffer[wPacketSize];

				//��ȡ����
				pBattleGameOption->BattleGameOption.wKindID = m_GameDBAide.GetValue_WORD(TEXT("KindID"));
				pBattleGameOption->BattleGameOption.cbEmployScene = m_GameDBAide.GetValue_BYTE(TEXT("EmployScene"));
				m_GameDBAide.GetValue_String(TEXT("CustomOption"), pBattleGameOption->BattleGameOption.szOptionJson, CountArray(pBattleGameOption->BattleGameOption.szOptionJson));

				//���ô�С
				wPacketSize += sizeof(tagBattleGameOptionEx);

				//�ƶ���¼
				m_GameDBModule->MoveToNext();
			}

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_GAME_OPTION, dwContextID, dwTokenID, cbBuffer, wPacketSize);
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

//��������
bool CDataBaseEngineSink::LoadRobotParameter(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//��������
		DBO_GR_RobotParameter GameRobotParameter;
		ZeroMemory(&GameRobotParameter, sizeof(GameRobotParameter));

		//�û��ʻ�
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wKindID"), m_pGameServiceAttrib->wKindID);
		m_AccountsDBAide.AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);

		//ִ�в�ѯ
		GameRobotParameter.lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_LoadRobotConfigure"), true);

		//��ȡ��Ϣ
		for (WORD i = 0; i < CountArray(GameRobotParameter.RobotParameter); i++)
		{
			//�����ж�
			if (m_AccountsDBModule->IsRecordsetEnd() == true) break;

			//���Ч��
			ASSERT(GameRobotParameter.wParameterCount < CountArray(GameRobotParameter.RobotParameter));
			if (GameRobotParameter.wParameterCount >= CountArray(GameRobotParameter.RobotParameter)) break;

			//��ȡ����
			GameRobotParameter.wParameterCount++;
			GameRobotParameter.RobotParameter[i].dwBatchID = m_AccountsDBAide.GetValue_DWORD(TEXT("BatchID"));
			GameRobotParameter.RobotParameter[i].dwServiceMode = m_AccountsDBAide.GetValue_DWORD(TEXT("ServiceMode"));
			GameRobotParameter.RobotParameter[i].dwRobotCount = m_AccountsDBAide.GetValue_DWORD(TEXT("RobotCount"));
			GameRobotParameter.RobotParameter[i].dwEnterTime = m_AccountsDBAide.GetValue_DWORD(TEXT("EnterTime"));
			GameRobotParameter.RobotParameter[i].dwLeaveTime = m_AccountsDBAide.GetValue_DWORD(TEXT("LeaveTime"));
			GameRobotParameter.RobotParameter[i].lTakeMinScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("TakeMinScore"));
			GameRobotParameter.RobotParameter[i].lTakeMaxScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("TakeMaxScore"));
			GameRobotParameter.RobotParameter[i].dwEnterMinInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("EnterMinInterval"));
			GameRobotParameter.RobotParameter[i].dwEnterMaxInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("EnterMaxInterval"));
			GameRobotParameter.RobotParameter[i].dwLeaveMinInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("LeaveMinInterval"));
			GameRobotParameter.RobotParameter[i].dwLeaveMaxInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("LeaveMaxInterval"));
			GameRobotParameter.RobotParameter[i].dwSwitchMinInnings = m_AccountsDBAide.GetValue_DWORD(TEXT("SwitchMinInnings"));
			GameRobotParameter.RobotParameter[i].dwSwitchMaxInnings = m_AccountsDBAide.GetValue_DWORD(TEXT("SwitchMaxInnings"));

			//�ƶ���¼
			m_AccountsDBModule->MoveToNext();
		}

		//������Ϣ
		WORD wHeadSize = sizeof(GameRobotParameter) - sizeof(GameRobotParameter.RobotParameter);
		WORD wDataSize = GameRobotParameter.wParameterCount * sizeof(GameRobotParameter.RobotParameter[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ROBOT_PARAMETER, dwContextID, dwTokenID, &GameRobotParameter, wHeadSize + wDataSize);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//��������
		DBO_GR_RobotParameter GameRobotParameter;
		ZeroMemory(&GameRobotParameter, sizeof(GameRobotParameter));

		//�������
		GameRobotParameter.wParameterCount = 0L;
		GameRobotParameter.lResultCode = DB_ERROR;

		//������Ϣ
		WORD wHeadSize = sizeof(GameRobotParameter) - sizeof(GameRobotParameter.RobotParameter);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ROBOT_PARAMETER, dwContextID, dwTokenID, &GameRobotParameter, wHeadSize);
	}

	return false;
}

//�ͱ�����
bool CDataBaseEngineSink::LoadSubsidyParameter(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//��������
		DBO_GR_SubsidyParameter SubsidyParameter;
		ZeroMemory(&SubsidyParameter, sizeof(SubsidyParameter));

		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ�в�ѯ
		LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadSubsidyParameter"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��ȡ����
			SubsidyParameter.lTakeSubsidyGold = m_PlatformDBAide.GetValue_LONGLONG(TEXT("TakeSubsidyGold"));
			SubsidyParameter.lTakeSubsidyCondition = m_PlatformDBAide.GetValue_LONGLONG(TEXT("TakeSubsidyCondition"));
		}

		//���ͽ��
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SUBSIDY_PARAMETER, 0, 0, &SubsidyParameter, sizeof(SubsidyParameter));

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//���Ʋ���
bool CDataBaseEngineSink::LoadControlParameter(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//��������
		DBO_GR_ControlParameter ControlParameter;
		ZeroMemory(&ControlParameter, sizeof(ControlParameter));

		//�������
		m_PlatformDBAide.ResetParameter();

		//ִ�в�ѯ
		LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadControlParameter"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//���Ʋ���
			ControlParameter.bControlEnabled = m_PlatformDBAide.GetValue_BYTE(TEXT("ControlEnabled"));
			ControlParameter.lBenchmarkValue = m_PlatformDBAide.GetValue_LONGLONG(TEXT("BenchmarkValue"));
			ControlParameter.wWLControlRate = m_PlatformDBAide.GetValue_WORD(TEXT("WLControlRate"));
			ControlParameter.wBLControlRate = m_PlatformDBAide.GetValue_WORD(TEXT("BLControlRate"));
			ControlParameter.wInWhiteListRate = m_PlatformDBAide.GetValue_WORD(TEXT("InWhiteListRate"));
			ControlParameter.wInBlackListRate = m_PlatformDBAide.GetValue_WORD(TEXT("InBlackListRate"));
			ControlParameter.wOutWhiteListRate = m_PlatformDBAide.GetValue_WORD(TEXT("OutWhiteListRate"));
			ControlParameter.wOutBlackListRate = m_PlatformDBAide.GetValue_WORD(TEXT("OutBlackListRate"));
			ControlParameter.wStartPlayCount = m_PlatformDBAide.GetValue_WORD(TEXT("StartPlayCount"));
			ControlParameter.wSupportControlMode = m_PlatformDBAide.GetValue_WORD(TEXT("SupportControlMode"));
		}

		//���ͽ��
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CONTROL_PARAMETER, 0, 0, &ControlParameter, sizeof(ControlParameter));

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//��Ϸ�ȼ�
bool CDataBaseEngineSink::LoadGameLevelOption(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//�������
		m_PlatformDBAide.ResetParameter();
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameLevelInfo"), true);

		//���سɹ�
		if (lResultCode == DB_SUCCESS)
		{
			//��������
			g_GameLevelManager->m_ArrayGameLevel.RemoveAll();
			g_GameLevelManager->m_ArrayUserLevel.RemoveAll();
			g_GameLevelManager->m_ArrayRankLevel.RemoveAll();

			//�ȼ���Ϣ
			tagGameLevelItem GameLevelItem;

			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				//��ȡ����					
				GameLevelItem.wLevelID = m_PlatformDBAide.GetValue_WORD(TEXT("LevelID"));
				GameLevelItem.lNeedPoint = m_PlatformDBAide.GetValue_LONG(TEXT("NeedPoint"));
				m_PlatformDBAide.GetValue_String(TEXT("Rewards"), GameLevelItem.szRewards, CountArray(GameLevelItem.szRewards));

				//�����Ʒ
				g_GameLevelManager->m_ArrayGameLevel.Add(GameLevelItem);

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			//�������
			m_PlatformDBAide.ResetParameter();
			lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadUserLevelInfo"), true);

			//���سɹ�
			if (lResultCode == DB_SUCCESS)
			{
				//�ȼ���Ϣ
				tagUserLevelItem UserLevelItem;

				while (m_PlatformDBModule->IsRecordsetEnd() == false)
				{
					//��ȡ����					
					UserLevelItem.wLevelID = m_PlatformDBAide.GetValue_WORD(TEXT("LevelID"));
					UserLevelItem.lNeedPoint = m_PlatformDBAide.GetValue_LONG(TEXT("NeedPoint"));
					m_PlatformDBAide.GetValue_String(TEXT("Rewards"), UserLevelItem.szRewards, CountArray(UserLevelItem.szRewards));
					m_PlatformDBAide.GetValue_String(TEXT("LevelName"), UserLevelItem.szLevelName, CountArray(UserLevelItem.szLevelName));

					//�����Ʒ
					g_GameLevelManager->m_ArrayUserLevel.Add(UserLevelItem);

					//�ƶ���¼
					m_PlatformDBModule->MoveToNext();
				}
			}

			//�������
			m_PlatformDBAide.ResetParameter();
			lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadRankLevelInfo"), true);

			//���سɹ�
			if (lResultCode == DB_SUCCESS)
			{
				//�ȼ���Ϣ
				tagRankLevelItem RankLevelItem;

				while (m_PlatformDBModule->IsRecordsetEnd() == false)
				{
					//��ȡ����					
					RankLevelItem.wLevelID = m_PlatformDBAide.GetValue_WORD(TEXT("LevelID"));
					RankLevelItem.lNeedScore = m_PlatformDBAide.GetValue_LONG(TEXT("NeedScore"));
					RankLevelItem.cbMainOrder = m_PlatformDBAide.GetValue_BYTE(TEXT("MainOrder"));
					RankLevelItem.cbSubOrder = m_PlatformDBAide.GetValue_BYTE(TEXT("SubOrder"));
					RankLevelItem.cbStarOrder = m_PlatformDBAide.GetValue_BYTE(TEXT("StarOrder"));
					m_PlatformDBAide.GetValue_String(TEXT("LevelName"), RankLevelItem.szLevelName, CountArray(RankLevelItem.szLevelName));

					//��ӵȼ�
					g_GameLevelManager->m_ArrayRankLevel.Add(RankLevelItem);

					//�ƶ���¼
					m_PlatformDBModule->MoveToNext();
				}
			}
		}

		return lResultCode == DB_SUCCESS;
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

//��Ϸ��Ʒ
bool CDataBaseEngineSink::LoadGameGoodsOption(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//�������
		m_PlatformDBAide.ResetParameter();
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameGoodsInfo"), true);

		//���سɹ�
		if (lResultCode == DB_SUCCESS)
		{
			//��������
			g_GameGoodsManager->m_ArrayGoodsInfo.RemoveAll();

			//��Ʒ��Ϣ
			tagGoodsInfo GoodsInfo;

			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				//��ȡ����					
				GoodsInfo.wGoodsID = m_PlatformDBAide.GetValue_WORD(TEXT("GoodsID"));
				GoodsInfo.wGoodsKind = m_PlatformDBAide.GetValue_WORD(TEXT("GoodsKind"));
				GoodsInfo.bHasIndate = m_PlatformDBAide.GetValue_BYTE(TEXT("HasIndate"));
				GoodsInfo.wCanTradeType = m_PlatformDBAide.GetValue_WORD(TEXT("CanTradeType"));
				GoodsInfo.wCanStorePlace = m_PlatformDBAide.GetValue_WORD(TEXT("CanStorePlace"));
				m_PlatformDBAide.GetValue_String(TEXT("GoodsName"), GoodsInfo.szGoodsName, CountArray(GoodsInfo.szGoodsName));
				m_PlatformDBAide.GetValue_String(TEXT("GoodsOption"), GoodsInfo.szGoodsOption, CountArray(GoodsInfo.szGoodsOption));

				//�����Ʒ
				g_GameGoodsManager->m_ArrayGoodsInfo.Add(GoodsInfo);

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}
		}

		return lResultCode == DB_SUCCESS;
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

//�汾��Ϣ
bool CDataBaseEngineSink::LoadGameVersionInfo(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//�������
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wModuleID"), m_pGameServiceOption->wKindID);
		LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameVersionInfo"), true);

		//���سɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			WORD wPacketSize = 0;
			BYTE cbBuffer[SOCKET_PACKET];

			//��������
			tagModuleVersion* pModuleVersion = NULL;

			//�汾����
			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				//������Ϣ
				if ((wPacketSize + sizeof(tagModuleVersion)) > sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_VERSION_INFO, dwContextID, dwTokenID, cbBuffer, wPacketSize);
					wPacketSize = 0;
				}

				//��ȡ��Ϣ
				pModuleVersion = (tagModuleVersion*)(cbBuffer + wPacketSize);

				//��ȡ����
				pModuleVersion->IsCurrent = m_PlatformDBAide.GetValue_BYTE(TEXT("IsCurrent"));
				pModuleVersion->wModuleID = m_PlatformDBAide.GetValue_WORD(TEXT("ModuleID"));
				pModuleVersion->dwDeviceKind = m_PlatformDBAide.GetValue_BYTE(TEXT("DeviceKind"));
				pModuleVersion->dwVersion = m_PlatformDBAide.GetValue_DWORD(TEXT("Version"));

				//����λ��
				wPacketSize += sizeof(tagModuleVersion);

				//�ƶ���¼
				m_PlatformDBModule->MoveToNext();
			}

			//����ʣ��
			if (wPacketSize > 0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_VERSION_INFO, dwContextID, dwTokenID, cbBuffer, wPacketSize);
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
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FILTER_WORDS, 0xfffe, dwTokenID, NULL, 0);

			//��ȡ��Ϣ
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				TCHAR szSensitiveWords[32] = TEXT("");

				//��ȡ��Ϣ
				m_AccountsDBAide.GetValue_String(TEXT("ForbidWords"), szSensitiveWords, 32);

				//������Ϣ
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FILTER_WORDS, 0, dwTokenID, szSensitiveWords, 32);

				//�ƶ���¼
				m_AccountsDBModule->MoveToNext();
			};

			//������Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FILTER_WORDS, 0xffff, 0, NULL, 0);
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

//��������
bool CDataBaseEngineSink::UnlockRobotUser()
{
	try
	{
		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_UnlockRobotUser"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
	}

	return false;
}

//���ر���
bool CDataBaseEngineSink::LoadUserPack(DWORD dwUserID)
{
	try
	{
		//���ñ���
		m_LogonSuccess.wGoodsCount = 0;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserQueryGoods"), true);

		//ִ�гɹ�
		if (lResultCode == DB_SUCCESS)
		{
			//��ȡ��Ϣ
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				//��ȡ����
				m_LogonSuccess.GoodsHoldInfo[m_LogonSuccess.wGoodsCount].wGoodsID = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsID"));
				m_LogonSuccess.GoodsHoldInfo[m_LogonSuccess.wGoodsCount].wGoodsKind = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsKind"));
				m_LogonSuccess.GoodsHoldInfo[m_LogonSuccess.wGoodsCount].wGoodsLevel = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsLevel"));
				m_LogonSuccess.GoodsHoldInfo[m_LogonSuccess.wGoodsCount].dwGoodsCount = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsCount"));
				m_LogonSuccess.GoodsHoldInfo[m_LogonSuccess.wGoodsCount].dwGoodsIndate = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsIndate"));
				m_LogonSuccess.GoodsHoldInfo[m_LogonSuccess.wGoodsCount].dwExpireTime = m_AccountsDBAide.GetValue_DWORD(TEXT("ExpireTime"));

				//У������
				if (++m_LogonSuccess.wGoodsCount == CountArray(m_LogonSuccess.GoodsHoldInfo))
				{
					break;
				}				

				//�ƶ���¼
				m_AccountsDBModule->MoveToNext();
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

//��ѯ�ͱ�
bool CDataBaseEngineSink::LoadUserSubsidy(DWORD dwUserID)
{
	try
	{
		//���ñ���
		m_LogonSuccess.cbResidualTimes = 0;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@szMachine"), m_LogonSuccess.szMachineID);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//ִ�в�ѯ
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_QuerySubsidy"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//ʣ�����
			m_LogonSuccess.cbResidualTimes = m_AccountsDBAide.GetValue_BYTE(TEXT("ResidualTimes"));
			m_LogonSuccess.lConsumeScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("ConsumeScore"));
		}

		return true;
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return false;
}

//���ؿ���
bool CDataBaseEngineSink::LoadControlInfo(DWORD dwUserID, tagUserControlItem& UserControlItem)
{
	try
	{
		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);		

		//ִ�в�ѯ
		LONG lReturnValue = m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LoadUserControl"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//������Ϣ
			UserControlItem.cbControlMode = m_GameDBAide.GetValue_BYTE(TEXT("ControlMode"));
			UserControlItem.cbControlKind = m_GameDBAide.GetValue_BYTE(TEXT("ControlKind"));
			UserControlItem.cbControlStatus = m_GameDBAide.GetValue_BYTE(TEXT("ControlStatus"));
			UserControlItem.wTargetWinRate = m_GameDBAide.GetValue_WORD(TEXT("TargetWinRate"));
			UserControlItem.wControlWinRate = m_GameDBAide.GetValue_WORD(TEXT("ControlWinRate"));
			UserControlItem.lTotalWinCount = m_GameDBAide.GetValue_LONG(TEXT("TotalWinCount"));
			UserControlItem.lTotalLoseCount = m_GameDBAide.GetValue_LONG(TEXT("TotalLoseCount"));
			UserControlItem.lTotalWinScore = m_GameDBAide.GetValue_LONGLONG(TEXT("TotalWinScore"));
			UserControlItem.lTotalLoseScore = m_GameDBAide.GetValue_LONGLONG(TEXT("TotalLoseScore"));
			UserControlItem.lTotalBetAmount = m_GameDBAide.GetValue_LONGLONG(TEXT("TotalBetAmount"));
			UserControlItem.wControlPlayCount = m_GameDBAide.GetValue_WORD(TEXT("ControlPlayCount"));
			UserControlItem.dwControlDuration = m_GameDBAide.GetValue_DWORD(TEXT("ControlDuration"));

			//��ʼʱ��
			SYSTEMTIME stControlStartTime;
			m_GameDBAide.GetValue_SystemTime(TEXT("ControlStartTime"), stControlStartTime);

			//ת��ʱ��
			CTime TimeControlStart(stControlStartTime);
			UserControlItem.dwControlStartTime = TimeControlStart.GetTime();
		}
	}
	catch (...)
	{
		ASSERT(FALSE);

		return false;
	}

	return true;
}

//��¼�ɹ�
VOID CDataBaseEngineSink::OnLogonDisposeResult(DWORD dwContextID, DWORD dwTokenID, LONG lResultCode, bool bMobileClient)
{
	if (lResultCode==DB_SUCCESS)
	{
		//��������
		m_LogonSuccess.wFaceID=m_GameDBAide.GetValue_WORD(TEXT("FaceID"));
		m_LogonSuccess.dwUserID=m_GameDBAide.GetValue_DWORD(TEXT("UserID"));
		m_LogonSuccess.dwGameID=m_GameDBAide.GetValue_DWORD(TEXT("GameID"));
		m_LogonSuccess.dwCustomID=m_GameDBAide.GetValue_DWORD(TEXT("CustomID"));
		m_GameDBAide.GetValue_String(TEXT("Accounts"), m_LogonSuccess.szAccounts, CountArray(m_LogonSuccess.szAccounts));
		m_GameDBAide.GetValue_String(TEXT("LogonPass"), m_LogonSuccess.szPassword, CountArray(m_LogonSuccess.szPassword));

		//�û�����
		m_LogonSuccess.cbAge=m_GameDBAide.GetValue_BYTE(TEXT("Age"));
		m_LogonSuccess.cbGender=m_GameDBAide.GetValue_BYTE(TEXT("Gender"));
		m_GameDBAide.GetValue_String(TEXT("Area"),m_LogonSuccess.szArea,CountArray(m_LogonSuccess.szArea));
		m_GameDBAide.GetValue_String(TEXT("City"),m_LogonSuccess.szCity,CountArray(m_LogonSuccess.szCity));
		m_GameDBAide.GetValue_String(TEXT("Province"),m_LogonSuccess.szProvince,CountArray(m_LogonSuccess.szProvince));
		m_GameDBAide.GetValue_String(TEXT("UnderWrite"),m_LogonSuccess.szUnderWrite,CountArray(m_LogonSuccess.szUnderWrite));
		m_GameDBAide.GetValue_String(TEXT("userHeadUrl"), m_LogonSuccess.szUserHeadUrl, CountArray(m_LogonSuccess.szUserHeadUrl));//kk jia
				
		//������Ϣ
		m_LogonSuccess.lGold=m_GameDBAide.GetValue_LONGLONG(TEXT("Gold"));
		m_LogonSuccess.lScore=m_GameDBAide.GetValue_LONGLONG(TEXT("Score"));
		m_LogonSuccess.cbScoreKind=m_GameDBAide.GetValue_BYTE(TEXT("ScoreKind"));
		m_LogonSuccess.lUserCard =m_GameDBAide.GetValue_LONGLONG(TEXT("UserCard"));
		m_LogonSuccess.lFreeScore=m_GameDBAide.GetValue_LONGLONG(TEXT("FreeScore"));		
		m_LogonSuccess.lInsureGold=m_GameDBAide.GetValue_LONGLONG(TEXT("InsureScore"));		

		//��Ϸ��Ϣ
		m_LogonSuccess.lWinCount=m_GameDBAide.GetValue_LONG(TEXT("WinCount"));
		m_LogonSuccess.lLostCount=m_GameDBAide.GetValue_LONG(TEXT("LostCount"));
		m_LogonSuccess.lDrawCount=m_GameDBAide.GetValue_LONG(TEXT("DrawCount"));
		m_LogonSuccess.lFleeCount=m_GameDBAide.GetValue_LONG(TEXT("FleeCount"));
		m_LogonSuccess.lExperience=m_GameDBAide.GetValue_LONG(TEXT("Experience"));
		m_LogonSuccess.lWinStreak=m_GameDBAide.GetValue_LONG(TEXT("WinStreak"));
		m_LogonSuccess.lMaxWinStreak=m_GameDBAide.GetValue_LONG(TEXT("MaxWinStreak"));

		//�ȼ���Ϣ
		m_LogonSuccess.cbMasterOrder = m_GameDBAide.GetValue_BYTE(TEXT("MasterOrder"));
		m_LogonSuccess.dwMemberPoint = m_GameDBAide.GetValue_DWORD(TEXT("MemberPoint"));

		//״̬��Ϣ
		m_LogonSuccess.cbShutUpStatus = m_GameDBAide.GetValue_BYTE(TEXT("ShutUpStatus"));
		m_LogonSuccess.dwShutUpOverTime = m_GameDBAide.GetValue_DWORD(TEXT("ShutUpOverTime"));

		//������Ϣ
		m_LogonSuccess.dwUserRight=m_GameDBAide.GetValue_DWORD(TEXT("UserRight"));
		m_LogonSuccess.dwMasterRight=m_GameDBAide.GetValue_DWORD(TEXT("MasterRight"));

		//��������
		m_LogonSuccess.dwInoutIndex=m_GameDBAide.GetValue_DWORD(TEXT("InoutIndex"));

		//��ҷ���
		if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD && bMobileClient == true)
		{
			m_LogonSuccess.wAddupWinRate = m_GameDBAide.GetValue_WORD(TEXT("AddupWinRate"));
			m_LogonSuccess.lAddupWinScore = m_GameDBAide.GetValue_LONGLONG(TEXT("AddupWinScore"));
			m_LogonSuccess.lAddupLoseScore = m_GameDBAide.GetValue_LONGLONG(TEXT("AddupLoseScore"));			
		}

		//��������
		if (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH && bMobileClient==true) 
		{
			m_LogonSuccess.wSignupCount=m_GameDBAide.GetValue_WORD(TEXT("SignupCount"));
			m_LogonSuccess.dwTicketCount=m_GameDBAide.GetValue_DWORD(TEXT("TicketCount"));
		}

		//������Ϣ
		m_GameDBAide.GetParameter(TEXT("@strErrorDescribe"),m_LogonSuccess.szDescribeString,CountArray(m_LogonSuccess.szDescribeString));

		//���ر���
		LoadUserPack(m_LogonSuccess.dwUserID);

		//�ͱ���Ϣ
		if (CServerRule::IsAllowLargessSubsidy(m_pGameServiceOption->dwServerRule))
		{
			LoadUserSubsidy(m_LogonSuccess.dwUserID);
		}		

		//������Ϣ
		if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
		{
			LoadControlInfo(m_LogonSuccess.dwUserID, m_LogonSuccess.UserControlItem);
		}

		//���ͽ��
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_SUCCESS,dwContextID,dwTokenID,&m_LogonSuccess,sizeof(m_LogonSuccess));
	}
	else
	{
		//��ȡ��Ϣ		
		m_LogonFailure.lErrorCode=lResultCode;
		m_LogonFailure.wLockServerID=INVALID_WORD;
		m_GameDBAide.GetParameter(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,CountArray(m_LogonFailure.szDescribeString));

		//�û�����
		if (m_LogonFailure.lErrorCode==100)
		{
			try
			{
				m_LogonFailure.wLockKindID=m_GameDBAide.GetValue_WORD(TEXT("LockKindID"));
				m_LogonFailure.wLockServerID=m_GameDBAide.GetValue_WORD(TEXT("LockServerID"));
				m_LogonFailure.wLockServerKind=m_GameDBAide.GetValue_WORD(TEXT("LockServerKind"));
			}
			catch (...){}
		}

		//���ͽ��
		WORD wStringSize=CountStringBuffer(m_LogonFailure.szDescribeString);
		WORD wSendSize=sizeof(m_LogonFailure)-sizeof(m_LogonFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE,dwContextID,dwTokenID,&m_LogonFailure,wSendSize);
	}

	return;
}

//��¼ʧ��
VOID CDataBaseEngineSink::OnLogonDisposeDBError(DWORD dwContextID, DWORD dwTokenID, IDataBaseException * pIException)
{
	//�������
	m_LogonFailure.lErrorCode=DB_ERROR;
	m_LogonFailure.wLockServerID=INVALID_WORD;
	LPCTSTR pszErrorString= CWHService::FormatMessage(MIM_SERVER_0111,TEXT("�ܱ�Ǹ���������ݿ⴦��æ�������Ժ��ٴγ��Ի��߽�ȥ������Ϸ���䣡"));
	StringCchCopy(m_LogonFailure.szDescribeString,CountArray(m_LogonFailure.szDescribeString), pszErrorString);

	//���ͽ��
	WORD wStringSize=CountStringBuffer(m_LogonFailure.szDescribeString);
	WORD wSendSize=sizeof(m_LogonFailure)-sizeof(m_LogonFailure.szDescribeString)+wStringSize;
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE,dwContextID,dwTokenID,&m_LogonFailure,wSendSize);

	return;
}

//���ػ���
bool CDataBaseEngineSink::OnRequestLoadRobots(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//����У��
	ASSERT(sizeof(DBR_GR_LoadRobotUser)==wDataSize);
	if(sizeof(DBR_GR_LoadRobotUser)!=wDataSize) return false;

	try
	{		
		//��ȡ����
		DBR_GR_LoadRobotUser * pLoadRobotUser = (DBR_GR_LoadRobotUser *)pData;
		if(pLoadRobotUser==NULL) return false;

		//��������
		DBO_GR_GameRobotInfo GameRobotInfo;
		ZeroMemory(&GameRobotInfo,sizeof(GameRobotInfo));

		//��������
		GameRobotInfo.dwBatchID=pLoadRobotUser->dwBatchID;

		//�û��ʻ�
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_AccountsDBAide.AddParameter(TEXT("@dwBatchID"),pLoadRobotUser->dwBatchID);
		m_AccountsDBAide.AddParameter(TEXT("@dwRobotCount"),pLoadRobotUser->dwRobotCount);

		//ִ�в�ѯ
		GameRobotInfo.lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_LoadRobotUser"),true);

		//��ȡ��Ϣ
		for (WORD i=0;i<CountArray(GameRobotInfo.RobotAccountsInfo);i++)
		{
			//�����ж�
			if (m_AccountsDBModule->IsRecordsetEnd()==true) break;

			//���Ч��
			ASSERT(GameRobotInfo.wRobotCount<CountArray(GameRobotInfo.RobotAccountsInfo));
			if (GameRobotInfo.wRobotCount>=CountArray(GameRobotInfo.RobotAccountsInfo)) break;

			//��ȡ����
			GameRobotInfo.wRobotCount++;
			GameRobotInfo.RobotAccountsInfo[i].dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_AccountsDBAide.GetValue_String(TEXT("LogonPass"),GameRobotInfo.RobotAccountsInfo[i].szPassword,CountArray(GameRobotInfo.RobotAccountsInfo[i].szPassword));

			//�ƶ���¼
			m_AccountsDBModule->MoveToNext();
		}

		//������Ϣ
		WORD wHeadSize=sizeof(GameRobotInfo)-sizeof(GameRobotInfo.RobotAccountsInfo);
		WORD wDataSize=GameRobotInfo.wRobotCount*sizeof(GameRobotInfo.RobotAccountsInfo[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ROBOT_USER,dwContextID,dwTokenID,&GameRobotInfo,wHeadSize+wDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GR_GameRobotInfo GameRobotInfo;
		ZeroMemory(&GameRobotInfo,sizeof(GameRobotInfo));

		//�������
		GameRobotInfo.wRobotCount=0L;
		GameRobotInfo.lResultCode=DB_ERROR;

		//������Ϣ
		WORD wHeadSize=sizeof(GameRobotInfo)-sizeof(GameRobotInfo.RobotAccountsInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ROBOT_USER,dwContextID,dwTokenID,&GameRobotInfo,wHeadSize);
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestLoadConfig(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//��ȡ����
	auto pLoadConfig = (DBR_GR_LoadConfig*)pData;

	//������
	DBO_GR_ConfigResult ConfigResult;
	ZeroMemory(&ConfigResult,sizeof(ConfigResult));

	//��������
	ConfigResult.bInitService = pLoadConfig->bInitService;
	ConfigResult.dwConfigMask = pLoadConfig->dwConfigMask;

	do
	{
		//��Ϸ�ȼ�
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_GAME_LEVEL)
		{
			ConfigResult.bResultCode = LoadGameLevelOption(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//��Ϸ��Ʒ
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_GAME_GOODS)
		{
			ConfigResult.bResultCode = LoadGameGoodsOption(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//��������
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_ROBOT_PARAM)
		{
			ConfigResult.bResultCode = LoadRobotParameter(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//�ͱ�����
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_SUBSIDY_PARAM)
		{
			ConfigResult.bResultCode = LoadSubsidyParameter(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//���Ʋ���
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_CONTROL_PARAM)
		{
			ConfigResult.bResultCode = LoadControlParameter(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//Լս����
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_BATTLE_OPTION)
		{
			ConfigResult.bResultCode = LoadBattleOption(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//�汾��Ϣ
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_GAME_VERSION)
		{
			ConfigResult.bResultCode = LoadGameVersionInfo(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//���дʻ�
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_FILTER_WORDS)
		{
			ConfigResult.bResultCode = LoadFilterWords(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//ȫ������
		if (pLoadConfig->dwConfigMask & GLOBAL_OPTION_MASK)
		{
			ConfigResult.bResultCode = LoadGlobalOption(dwContextID, dwTokenID, pLoadConfig->dwConfigMask);
			if (!ConfigResult.bResultCode) break;
		}		

		//��Ϸ����
		if (pLoadConfig->dwConfigMask & GAME_CONFIG_MASK)
		{
			if (m_pIGameDataBaseSink != NULL)
			{
				ConfigResult.bResultCode = m_pIGameDataBaseSink->LoadGameConfig(pLoadConfig->dwConfigMask,0);
				if (!ConfigResult.bResultCode) break;
			}
		}

	} while (false);
	
	//���ͽ��
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CONFIG_RESULT, dwContextID, dwTokenID, &ConfigResult, sizeof(ConfigResult));
	
	return true;
}

//¼���¼
bool CDataBaseEngineSink::OnRequestRecordGameVideo(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize <= sizeof(DBR_GR_RecordGameVideo));
		if (wDataSize > sizeof(DBR_GR_RecordGameVideo)) return false;

		//������
		DBR_GR_RecordGameVideo * pRecordGameVideo = (DBR_GR_RecordGameVideo *)pData;

		//��������
		CString strUserID, strUserIDList;

		strUserIDList.AppendChar('[');

		//��������		
		for (int i = 0; i < pRecordGameVideo->wUserCount; i++)
		{
			//�û���ʶ
			strUserID.Format(TEXT("%d"), pRecordGameVideo->dwUserID[i]);

			if (i > 0)
			{
				strUserIDList.AppendChar(',');
			}

			strUserIDList.Append(strUserID);
		}

		strUserIDList.AppendChar(']');

		//�������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@dwBattleNum"), pRecordGameVideo->dwRelationNum);
		m_GameDBAide.AddParameter(TEXT("@lBattleRecordID"), pRecordGameVideo->lRelationRecordID);
		m_GameDBAide.AddParameter(TEXT("@strVideoNumber"), pRecordGameVideo->szVideoNumber);
		m_GameDBAide.AddParameter(TEXT("@VideoCreateTime"), pRecordGameVideo->VideoCreateTime);

		//�û���Ϣ
		m_GameDBAide.AddParameter(TEXT("@wUserCount"), pRecordGameVideo->wUserCount);
		m_GameDBAide.AddParameter(TEXT("@strUserIDList"), strUserIDList);

		//ִ�в�ѯ
		LONG lReturnValue = m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_RecordGameVideo"), false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//��Ϸ��¼
bool CDataBaseEngineSink::OnRequestRecordDrawInfo(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize <= sizeof(DBR_GR_RecordDrawInfo));
		if (wDataSize > sizeof(DBR_GR_RecordDrawInfo)) return false;

		//������
		DBR_GR_RecordDrawInfo * pRecordDrawInfo = (DBR_GR_RecordDrawInfo *)pData;

		//�������		
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@lRecordID"), pRecordDrawInfo->lRecordID);
		m_GameDBAide.AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@wTableID"), pRecordDrawInfo->wTableID);
		m_GameDBAide.AddParameter(TEXT("@wUserCount"), pRecordDrawInfo->wUserCount);
		m_GameDBAide.AddParameter(TEXT("@lWasteCount"), pRecordDrawInfo->lWasteCount);
		m_GameDBAide.AddParameter(TEXT("@dwWasterUserID"), pRecordDrawInfo->dwWasteUserID);
		m_GameDBAide.AddParameter(TEXT("@lRevenueCount"), pRecordDrawInfo->lRevenueCount);
		m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"), pRecordDrawInfo->dwPlayTimeCount);
		m_GameDBAide.AddParameter(TEXT("@ExecuteDate"), pRecordDrawInfo->RecordTime);
		m_GameDBAide.AddParameter(TEXT("@strVideoNumber"), pRecordDrawInfo->szVideoNumber);

		//Լս����
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE) != 0)
		{
			m_GameDBAide.AddParameter(TEXT("@cbSocreKind"), pRecordDrawInfo->cbScoreKind);
			m_GameDBAide.AddParameter(TEXT("@lBattleRecordID"), pRecordDrawInfo->lRelationRecordID);
			m_GameDBAide.AddParameter(TEXT("@strGameCourse"), pRecordDrawInfo->szGameSourse);
		}

		//��������
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_MATCH) != 0)
		{
			m_GameDBAide.AddParameter(TEXT("@lRelationMatchNo"), pRecordDrawInfo->lRelationRecordID);			
		}

		//ִ�в�ѯ
		m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_RecordDrawInfo"), false);		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//д�ּ�¼
bool CDataBaseEngineSink::OnRequestRecordDrawScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize <= sizeof(DBR_GR_RecordDrawScore));
		if (wDataSize > sizeof(DBR_GR_RecordDrawScore)) return false;

		//������
		DBR_GR_RecordDrawScore* pRecordDrawScore = (DBR_GR_RecordDrawScore*)pData;

		//�����ַ���
		CString strDrawScore, strScoreItem;
		LPCTSTR pszScoreFormat = TEXT("{\"UserID\":%d,\"ChairID\":%d,\"Score\":%I64d,\"Revenue\":%I64d,\"BetAmount\":%I64d,\"BeforeScore\":%I64d}");
		
		//�������
		strDrawScore.AppendChar('[');

		//�������
		for (WORD i = 0; i < pRecordDrawScore->wUserCount; i++)
		{
			//��ȡ����
			auto pScoreRecordItem = &pRecordDrawScore->ScoreRecordItem[i];

			//��ʽ������
			strScoreItem.Format(pszScoreFormat, pScoreRecordItem->dwUserID, pScoreRecordItem->wChairID, pScoreRecordItem->lScore, pScoreRecordItem->lRevenue, pScoreRecordItem->lBetAmount, pScoreRecordItem->lBeforScore);
			
			//����ַ���			
			if (i>0) strDrawScore.AppendChar(',');
			strDrawScore.Append(strScoreItem);			
		}

		//�������
		strDrawScore.AppendChar(']');
		
		//��������
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@lRecordID"), pRecordDrawScore->lRecordID);
		m_GameDBAide.AddParameter(TEXT("@strVideoNumber"), pRecordDrawScore->szVideoNumber);
		m_GameDBAide.AddParameter(TEXT("@strScoreItemList"), strDrawScore.GetString());

		//Լս����
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
		{
			m_GameDBAide.AddParameter(TEXT("@lBattleRecordID"), pRecordDrawScore->lRelationRecordID);
		}	

		//��������
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_MATCH) != 0)
		{
			m_GameDBAide.AddParameter(TEXT("@lRelationMatchNo"), pRecordDrawScore->lRelationRecordID);
		}

		//ִ�в�ѯ
		m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_RecordDrawScore"), false);		

		return true;
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

//�û�Ȩ��
bool CDataBaseEngineSink::OnRequestManageUserRight(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize==sizeof(DBR_GR_ManageUserRight));
		if (wDataSize!=sizeof(DBR_GR_ManageUserRight)) return false;

		//������
		DBR_GR_ManageUserRight * pManageUserRight=(DBR_GR_ManageUserRight *)pData;

		if((pManageUserRight->dwAddRight&UR_MATCH_USER)!=0 || (pManageUserRight->dwRemoveRight&UR_MATCH_USER)!=0)
		{
			//�������
			m_GameDBAide.ResetParameter();
			m_GameDBAide.AddParameter(TEXT("@dwUserID"),pManageUserRight->dwUserID);
			m_GameDBAide.AddParameter(TEXT("@dwAddRight"),pManageUserRight->dwAddRight&UR_MATCH_USER);
			m_GameDBAide.AddParameter(TEXT("@dwRemoveRight"),pManageUserRight->dwRemoveRight&UR_MATCH_USER);

			m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_ManageUserRight"),false);

			if((pManageUserRight->dwAddRight&~UR_MATCH_USER)==0 && (pManageUserRight->dwRemoveRight&~UR_MATCH_USER)==0)
			{
				return true;
			}
			else
			{
				pManageUserRight->dwAddRight=(pManageUserRight->dwAddRight&~UR_MATCH_USER);
				pManageUserRight->dwRemoveRight=(pManageUserRight->dwRemoveRight&~UR_MATCH_USER);
			}
		}

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pManageUserRight->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwAddRight"),pManageUserRight->dwAddRight);
		m_AccountsDBAide.AddParameter(TEXT("@dwRemoveRight"),pManageUserRight->dwRemoveRight);

		//ִ�в�ѯ
		LONG lReturnValue=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_ManageUserRight"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestSendGift(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
    try
    {
        //Ч�����
        ASSERT(wDataSize == sizeof(DBR_GR_SendGift));
        if (wDataSize != sizeof(DBR_GR_SendGift)) return false;

        //������
		DBR_GR_SendGift* pSendGift = (DBR_GR_SendGift*)pData;

        //ת����ַ
        TCHAR szClientIP[16] = TEXT("");
        BYTE* pClientIP = (BYTE*)&pSendGift->dwClientIP;
        _sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

        //�������
        m_AccountsDBAide.ResetParameter();
        m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pSendGift->dwUserID);
        m_AccountsDBAide.AddParameter(TEXT("@dwTargetUserID"), pSendGift->dwTargetUserID);
        m_AccountsDBAide.AddParameter(TEXT("@cbIsGoldRoom"), pSendGift->cbIsGoldRoom);
        m_AccountsDBAide.AddParameter(TEXT("@lCellScore"), pSendGift->lCellScore);
        m_AccountsDBAide.AddParameter(TEXT("@lMinTableScore"), pSendGift->lMinTableScore);
        m_AccountsDBAide.AddParameter(TEXT("@dwGoodsID"), pSendGift->dwGoodsID);
        m_AccountsDBAide.AddParameter(TEXT("@dwGoodsCount"), pSendGift->dwGoodsCount);
        m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);

        //�������
        TCHAR szDescribe[128] = TEXT("");
        m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

        //ִ�в�ѯ
        LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_SendGift"), true);

		//ִ�г�
		if (lReturnValue == DB_SUCCESS)
		{
            //��������
			DBO_GR_SendGiftSuccess OperateSuccess;
            ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

            //�������
			OperateSuccess.dwUserID = pSendGift->dwUserID;
            OperateSuccess.dwTargetUserID = pSendGift->dwTargetUserID;
            OperateSuccess.wMoneyID = m_AccountsDBAide.GetValue_WORD(TEXT("MoneyID"));
            OperateSuccess.lGiftPrice = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Price"));
            OperateSuccess.dwLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));
            OperateSuccess.dwMyLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("MyLoveLiness"));
            OperateSuccess.dwTargetLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("TargetLoveLiness"));
            OperateSuccess.dwGoodsID = pSendGift->dwGoodsID;
            OperateSuccess.dwGoodsCount = pSendGift->dwGoodsCount;

            //���ͽ��
            m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SEND_GIFT_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, sizeof(OperateSuccess));
		}
		else
		{
            //��������
			DBO_GR_SendGiftFailure OperateFailure;
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
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SEND_GIFT_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

        return true;
    }
    catch (IDataBaseException* pIException)
    {
        TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//��������
		DBO_GR_SendGiftFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = -1;
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_SERVER_0113, TEXT("�������ݿ�����쳣�������Ժ����ԣ�")));

		//���ͽ��
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SEND_GIFT_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

        return false;
    }

    return true;
}

//��������
bool CDataBaseEngineSink::UpdateUserLoveLiness(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID)
{
    try
    {
        //�������
        m_AccountsDBAide.ResetParameter();
        m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
        LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_QueryUserLoveLiness"), true);

        //���سɹ�
        if (lResultCode == DB_SUCCESS)
        {
            //���²Ƹ�
            DBO_GR_UpdateLoveLiness LoveLinessUpdate;
            ZeroMemory(&LoveLinessUpdate, sizeof(LoveLinessUpdate));

            //���ñ���
            LoveLinessUpdate.dwLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));

            //���ͽ��			
            m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_LOVELINESS, dwContextID, dwTokenID, &LoveLinessUpdate, sizeof(LoveLinessUpdate));
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
bool CDataBaseEngineSink::OnRequestUpdateLoveLiness(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
    //Ч�����
    ASSERT(wDataSize == sizeof(DBR_GR_UpdateLoveLiness));
    if (wDataSize != sizeof(DBR_GR_UpdateLoveLiness)) return false;

    //������
    DBR_GR_UpdateLoveLiness* pUpdateLoveLiness = (DBR_GR_UpdateLoveLiness*)pData;

    //��������
    UpdateUserLoveLiness(pUpdateLoveLiness->dwUserID, dwContextID, dwTokenID);

	return  true;
}

//////////////////////////////////////////////////////////////////////////

