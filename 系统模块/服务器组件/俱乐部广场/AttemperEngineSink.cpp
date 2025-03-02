#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_CONNECT_CENTER			1									//������ʶ
#define IDI_LOAD_BATTLE_OPTION		2									//�߼�����
#define IDI_LOAD_FORBID_WORDS		3									//���ؽ���

//ʱ������
#define TIME_LOAD_BATTLE_OPTION		60									//��������

	
//////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//���ñ���		
	m_bLogicSevice=false;
	m_bNeedConnStatus=true;
	m_pLogicOption=NULL;
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;			

	//�������
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPSocketCenter = NULL;
	m_pITCPNetworkEngine=NULL;	

	return;
}

//��������
CAttemperEngineSink::~CAttemperEngineSink()
{
}

//�ӿڲ�ѯ
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//�󶨲���
	m_pBindParameter=new tagBindParameter[m_pLogicOption->wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pLogicOption->wMaxConnect);

	//����״̬
	m_bLogicSevice=true;

	//���ؽ���
	m_pITimerEngine->SetTimer(IDI_LOAD_FORBID_WORDS,1000L,1,0);

	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//���ñ���
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;	
	m_pITCPSocketCenter=NULL;

	//����״̬
	m_bLogicSevice=false;
	m_bNeedConnStatus=true;

	//ɾ������
	SafeDeleteArray(m_pBindParameter);

	//��������
	m_GlobalItemManager.ResetData();		

	//ɾ���û�
	m_ClubUserManager.DeleteUserItem();	

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{	
	case CT_CONNECT_CENTER:		//��������
		{
			//��ȡ��ַ					
			auto pAddressInfo=&m_pInitParameter->m_CenterAddress;

			//��������
			m_pITCPSocketCenter->Connect(htonl(pAddressInfo->dwServiceAddr), m_pInitParameter->m_wCenterPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ڳ����������ķ�����..."),TraceLevel_Normal);

			return true;
		}
	case CT_LOAD_CLUB_OPTION:		//��������
		{
			//��������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_CLUB_OPTION,0, 0, 0,NULL,0);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ڳ��Լ���Ⱥ��������Ϣ..."),TraceLevel_Normal);

			return true;
		}
	}

	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//ʱ���¼�
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	switch (dwTimerID)
	{	
	case IDI_CONNECT_CENTER:		//��������
		{
			//��ȡ��ַ					
			auto pAddressInfo=&m_pInitParameter->m_CenterAddress;

			//��������
			m_pITCPSocketCenter->Connect(htonl(pAddressInfo->dwServiceAddr), m_pInitParameter->m_wCenterPort);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ڳ����������ķ�����..."),TraceLevel_Normal);

			return true;
		}
	case IDI_LOAD_FORBID_WORDS:		//���ؽ���
		{
			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_FORBID_WORDS,0,0,0,NULL,0);

			return true;
		}
	}

	return false;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//��ȡ����
	ASSERT(LOWORD(dwSocketID)<m_pLogicOption->wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pLogicOption->wMaxConnect) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	auto pBindParameter=(m_pBindParameter+wBindIndex);

	//���ñ���
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientIP=dwClientIP;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);	

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ������
	WORD wMainCmdID = LOCMMD(Command.wMainCmdID);

	//ע������
	if (wMainCmdID == MDM_CS_REGISTER)
	{
		return OnTCPNetworkMainRegister(Command.wSubCmdID, pData, wDataSize, dwSocketID);
	}

	//ͨ�÷���
	if (wMainCmdID == MDM_CS_COMMON_SERVICE)
	{
		return OnTCPNetworkMainCommonService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
	}

	switch (wMainCmdID)
	{
	case MDM_CLUBPLAZA_SERVICE:		//�߼�����
		{
			return OnTCPNetworkMainClubPlazaService(Command.wSubCmdID,pData,wDataSize,dwSocketID,dwTokenID);
		}
	}

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	auto pBindParameter=(m_pBindParameter+wBindIndex);

	//���ط���
	if (pBindParameter->wServiceModule==SERVICE_MODULE_GATE)
	{
		//��������
		auto pGlobalGateItem=m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem!=NULL)
		{
			//ע������
			m_GlobalItemManager.DeleteGateItem(pBindParameter->wServiceID);					
		}
	}

	//�����Ϣ
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	try
	{
		switch (wRequestID)
		{
		case DBO_GR_LOGON_SUCCESS:		//��¼�ɹ�
		{
			return OnDBLogonSuccess(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_LOGON_FAILURE:		//��¼ʧ��
		{
			return OnDBLogonFailure(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_LOGON_FINISH:		//��¼���
		{
			return OnDBLogonFinish(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_CLUB_NAMECARD:		//������Ƭ
		{
			return OnDBClubNameCard(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_SEARCH_CLUB:		//����Ⱥ��
		{
			return OnDBSearchClub(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_CREATE_CLUB:		//����Ⱥ��
		{
			return OnDBCreateClub(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_UPDATE_CLUB:		//����Ⱥ��
		{
			return OnDBUpdateClub(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_DELETE_CLUB:		//ɾ��Ⱥ��
		{
			return OnDBDeleteClub(dwContextID, dwTokenID, pData, wDataSize);
		}		
		case DBO_GR_INSERT_MEMBER:		//��ӳ�Ա
		{
			return OnDBInsertMember(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_UPDATE_MEMBER:		//���³�Ա
		{
			return OnDBUpdateMember(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_DELETE_MEMBER:		//ɾ����Ա
		{
			return OnDBDeleteMember(dwContextID, dwTokenID, pData, wDataSize);
		}		
		case DBO_GR_MODIFY_SCORE:		//�޸ķ���
		{
			return OnDBModifyScore(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_APPLY_MESSAGE:		//������Ϣ
		{
			return OnDBApplyMessage(dwContextID, dwTokenID, pData, wDataSize);
		}		
		case DBO_GR_CLUB_OPTION:		//Ⱥ������
		{
			return OnDBClubOption(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_OPTION_FINISH:		//�������
		{
			return OnDBOptionFinish(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_FIRBID_WORDS:		//��ֹ�ʻ�
		{
			return OnDBForbidWords(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_WEALTH_UPDATE:		//�Ƹ�����
		{
			return OnDBWealthUpdate(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_APPLY_DELETE:		//����ɾ��
		{
			return OnDBApplyDelete(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_DETECT_CLUBNAME:	//�������
		{
			return OnDBDetectClubName(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_OPERATE_SUCCESS:	//�����ɹ�
		{
			return OnDBOperateSuccess(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_OPERATE_FAILURE:	//����ʧ��
		{
			return OnDBOperateFailure(dwContextID, dwTokenID, pData, wDataSize);
		}	
		}
	}
	catch (...)
	{
		//������ʾ
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("OnEventDataBase(SubCmdID=%d)"),wRequestID);

		//��ʾ��Ϣ
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);
	}

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason)
{
	switch (dwServiceID)
	{
	case NETWORK_CENTER:	//��������
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("�����ķ����������ӹر��ˣ�%ld �����������"),m_pInitParameter->m_wConnectTime);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return true;
		}
	}

	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode)
{
	//��������
	if (dwServiceID==NETWORK_CENTER)
	{
		//�����ж�
		if (nErrorCode!=0)
		{
			//������ʾ
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("���ķ���������ʧ�� [ %ld ]��%ld �����������"),
				nErrorCode,m_pInitParameter->m_wConnectTime);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}

		//��������
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService,sizeof(RegisterService));

		//����ṹ
		RegisterService.wServiceID=m_pLogicOption->wServiceID;
		RegisterService.wServiceModule=SERVICE_MODULE_CLUBPLAZA;
		RegisterService.wMaxConnect = m_pLogicOption->wMaxConnect;
		RegisterService.wServicePort=m_pLogicOption->wServicePort;
		lstrcpyn(RegisterService.szServiceName, TEXT("����㳡"), CountArray(RegisterService.szServiceName));
		lstrcpyn(RegisterService.szServiceDomain,m_pLogicOption->szServiceDomain,CountArray(RegisterService.szServiceDomain));

		//��������
		m_pITCPSocketCenter->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_SERVICE,&RegisterService,sizeof(RegisterService),0);

		return true;
	}	

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD *pdwTokenID, WORD wTokenCount)
{
	//��������
	if (dwServiceID==NETWORK_CENTER)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:			//ע�����
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
		}
		case MDM_CS_USER_COLLECT:		//�û��ռ�
		{
			return OnTCPSocketMainUserCollect(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_CS_COMMON_SERVICE:		//������Ϣ
		{
			return OnTCPSocketMainCommonService(Command.wSubCmdID,pData,wDataSize);
		}
		case MDM_CS_MANAGER_SERVICE:	//�������
		{
			return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
		}
		case MDM_CS_FORWARD_SERVICE:	//ת������
		{
			return OnTCPSocketMainForwardService(Command.wSubCmdID, pData, wDataSize);
		}
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//ע���¼�
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_SUCCESS:		//ע��ɹ�
		{
			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CENTER_RESULT,&ControlResult,sizeof(ControlResult));

			//�����û�
			m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_COLLECT_USER, NULL, 0, 0);

			return true;
		}
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
		{
			//Ч�����
			auto pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//�رմ���
			m_pITCPSocketCenter->CloseSocket();

			//��ʾ��Ϣ
			CW2CT strDescribeString(pRegisterFailure->szDescribeString);
			if (lstrlen(strDescribeString)>0) CTraceService::TraceString(strDescribeString,TraceLevel_Exception);

			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_FAILURE;
			SendUIControlPacket(UI_CENTER_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}

//�ռ��¼�
bool CAttemperEngineSink::OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_USER_LIST:		//�û��б�
	{
		//У�����
		ASSERT (wDataSize % sizeof(tagGlobalUserInfo) == 0);
		if (wDataSize % sizeof(tagGlobalUserInfo) != 0) return true;

		//��ȡ����
		auto pUserItem = (tagGlobalUserInfo*)pData;
		WORD wUserCount = wDataSize / sizeof(tagGlobalUserInfo);		

		//��������
		tagClubUserInfo UserInfo;
		ZeroMemory(&UserInfo, sizeof(UserInfo));

		//��������
		CClubUserItem* pServerUserItem = NULL;

		//�����û�
		for (WORD i = 0; i < wUserCount; i++)
		{
			//У��״̬
			if (pUserItem->cbCBUserStatus == US_CB_OFFLINE) continue;

			//�����û�
			pServerUserItem = m_ClubUserManager.SearchUserItem(pUserItem->dwUserID);
			if (pServerUserItem != NULL)
			{
				pServerUserItem->SetSocketInfo(pUserItem->SocketInfo);
				pServerUserItem->SetUserStatus(pUserItem->cbCBUserStatus);
			}
			else
			{
				//�����û�
				UserInfo.dwUserID = pUserItem->dwUserID;
				UserInfo.cbGender = pUserItem->cbGender;
				UserInfo.cbUserStatus = pUserItem->cbCBUserStatus;
				StringCchCopy(UserInfo.szNickName, CountArray(UserInfo.szNickName), pUserItem->szNickName);

				//ͷ����Ϣ
				UserInfo.wFaceID = pUserItem->wFaceID;
				UserInfo.dwCustomID = pUserItem->dwCustomID;
				StringCchCopy(UserInfo.szUserHeadUrl, CountArray(UserInfo.szUserHeadUrl), pUserItem->szUserHeadUrl);//kk jia
				

				//�����û�
				m_ClubUserManager.InsertUserItem(UserInfo, pUserItem->SocketInfo);
			}

			pUserItem++;
		}

		return true;
	}
	case SUB_CS_S_COLLECT_FINISH:	//�б����
	{
		return true;
	}
	}

	return true;
}

//�б��¼�
bool CAttemperEngineSink::OnTCPSocketMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SERVICE_INSERT:	//״̬����
	{
		//����У��
		ASSERT(wDataSize % sizeof(tagServiceItem) == 0);
		if (wDataSize % sizeof(tagServiceItem) != 0) return false;

		//��ȡ����
		auto pServiceItem = (tagServiceItem *)pData;

		return true;
	}
	case SUB_CS_S_SERVICE_REMOVE:	//״̬�Ƴ�
	{
		//����У��
		ASSERT(wDataSize == sizeof(CMD_CS_S_ServiceRemove));
		if (wDataSize != sizeof(CMD_CS_S_ServiceRemove)) return false;

		//��ȡ����
		auto pServiceRemove = (CMD_CS_S_ServiceRemove *)pData;

		return true;
	}	
	}

	return true;
}


//�������
bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_EXECUTE_COMMAND:			//ִ������
		{
			//����У��
			ASSERT(sizeof(tagExecuteCommand) == wDataSize);
			if (sizeof(tagExecuteCommand) != wDataSize) return false;

			//��ȡ����
			auto pCommandCode = (tagExecuteCommand*)pData;

			//��������
			if (pCommandCode->dwCommandCode & CMD_LOAD_CONFIG_DATA)
			{
				//���дʻ�
				if (pCommandCode->dwCommandParam & CONFIG_MASK_FILTER_WORDS)
				{
					//��������
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_FORBID_WORDS, 0, 0, 0, 0, 0);
				}

				//��������
				if (pCommandCode->dwCommandParam & CONFIG_MASK_CLUB_OPTION)
				{
					//��������
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_CLUB_OPTION, 0, 0, 0, 0, 0);
				}
			}

			//ֹͣ����
			if (pCommandCode->dwCommandCode&CMD_STOP_CLUB_SERVICE)
			{
				//���ñ�ʶ
				m_bLogicSevice=false;
			}

			//��������
			if (pCommandCode->dwCommandCode&CMD_START_CLUB_SERVICE)
			{
				//���ñ�ʶ
				m_bLogicSevice=true;
			}

			return true;
		}
	}

	return true;
}

//�߼�ת��
bool CAttemperEngineSink::OnTCPSocketMainForwardService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_CB_USER_ONLINE:			//�û�����
	{
		//����У��
		ASSERT (wDataSize == sizeof(tagGlobalUserInfo));
		if (wDataSize != sizeof(tagGlobalUserInfo)) return false;

		//��ȡ����
		auto pGlobalUserInfo = (tagGlobalUserInfo*)pData;

		//���Ҷ���
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pGlobalUserInfo->dwUserID);
		if (pClubUserItem != NULL)
		{
			//�û���Ϣ
			auto pClubUserInfo = pClubUserItem->GetUserInfo(); 

			//���ñ���
			pClubUserInfo->wFaceID = pGlobalUserInfo->wFaceID;
			pClubUserInfo->dwCustomID = pGlobalUserInfo->dwCustomID;
			pClubUserInfo->cbUserStatus = pGlobalUserInfo->cbCBUserStatus;
			StringCchCopy(pClubUserInfo->szNickName,CountArray(pClubUserInfo->szNickName),pGlobalUserInfo->szNickName);

			//��������				
			pClubUserItem->SetSocketInfo(pGlobalUserInfo->SocketInfo);
		}
		else
		{
			//����ṹ
			tagClubUserInfo ClubUserInfo = {};

			//���ñ���
			ClubUserInfo.dwUserID = pGlobalUserInfo->dwUserID;
			ClubUserInfo.dwGameID = pGlobalUserInfo->dwGameID;
			ClubUserInfo.cbGender = pGlobalUserInfo->cbGender;
			ClubUserInfo.wFaceID = pGlobalUserInfo->wFaceID;
			ClubUserInfo.dwCustomID = pGlobalUserInfo->dwCustomID;
			ClubUserInfo.cbUserStatus = pGlobalUserInfo->cbCBUserStatus;
			StringCchCopy(ClubUserInfo.szNickName, CountArray(ClubUserInfo.szNickName), pGlobalUserInfo->szNickName);
			StringCchCopy(ClubUserInfo.szUserHeadUrl, CountArray(ClubUserInfo.szUserHeadUrl), pGlobalUserInfo->szUserHeadUrl);


			//�����û�
			m_ClubUserManager.InsertUserItem(ClubUserInfo, pGlobalUserInfo->SocketInfo);
		}

		return true;
	}
	case SUB_CS_CB_USER_OFFLINE:			//�û�����
	{
		//����У��
		ASSERT (wDataSize == sizeof(CMD_CS_CB_UserOffline));
		if (wDataSize != sizeof(CMD_CS_CB_UserOffline)) return false;

		//��ȡ����
		auto pUserOffline = (CMD_CS_CB_UserOffline*)pData;

		//ɾ���û�
		m_ClubUserManager.DeleteUserItem(pUserOffline->dwUserID);

		return true;
	}			
	}

	return true;
}

//�߼�����
bool CAttemperEngineSink::OnTCPNetworkMainClubPlazaService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID,DWORD dwTokenID)
{
	//��ȡ����
	WORD wBindIndex=LOWORD(dwSocketID);
	auto pBindParameter=(m_pBindParameter+wBindIndex);

	//���Ʋ���
	auto pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	//ֹͣ����
	if (m_bLogicSevice==false)
	{
		//������Ϣ
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//��������						
		SystemMessage.wType=SMT_EJECT|SMT_CLOSE_LINK;
		StringCchCopy(SystemMessage.szString,CountArray(SystemMessage.szString), CWHService::FormatMessage(MIM_CLUBPLAZA_0001,TEXT("ϵͳά���У�������ʱֹͣ�������Ժ��ٴ����ԣ�")));

		//������Ϣ
		SystemMessage.wLength=CountStringBuffer(SystemMessage.szString);
		WORD wPacketSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString)+ SystemMessage.wLength;
		SendDataToGate(pBindParameter->wServiceID, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wPacketSize,&dwTokenID,1);

		return true;
	}	

	try
	{
		switch (wSubCmdID)
		{		
		case SUB_CP_C_ENTER_SERVICE:	//�������
		{
			//����ṹ
			DBR_GR_LogonService LogonService;
			ZeroMemory(&LogonService,sizeof(LogonService));

			//���ñ���
			LogonService.dwUserID=pTokenParameter->dwUserID;			

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_SERVICE, LogonService.dwUserID,dwSocketID,dwTokenID,&LogonService,sizeof(LogonService));

			return true;
		}		
		case SUB_CP_C_SEARCH_CLUB:		//����Ⱥ��
		{
			//����У��
			ASSERT (wDataSize==sizeof(CMD_CP_C_SearchClub));
			if (wDataSize!=sizeof(CMD_CP_C_SearchClub)) return true;

			//��ȡ����
			auto pSearchClub=(CMD_CP_C_SearchClub *)pData;				

			//����ṹ
			DBR_GR_SearchClub SearchClub;
			ZeroMemory(&SearchClub, sizeof(SearchClub));

			//���ñ���
			SearchClub.dwClubID = pSearchClub->dwClubID;
			SearchClub.dwUserID = pTokenParameter->dwUserID;				

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SEARCH_CLUB, SearchClub.dwUserID, dwSocketID, dwTokenID, &SearchClub, sizeof(SearchClub));

			return true;
		}
		case SUB_CP_C_CREATE_CLUB:		//����Ⱥ��
		{
			//����У��
			ASSERT (wDataSize==sizeof(CMD_CP_C_CreateClub));
			if (wDataSize!=sizeof(CMD_CP_C_CreateClub)) return true;

			//��ȡ����
			auto pCreateClub=(CMD_CP_C_CreateClub *)pData;

			//��������
			pCreateClub->szClubName[CountArray(pCreateClub->szClubName)-1]=0;
			pCreateClub->szClubIntroduce[CountArray(pCreateClub->szClubIntroduce)-1]=0;

			//��ѯ�û�
			auto pServerUserItem=m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
			if (pServerUserItem==NULL) return true;

			//��ȡ����
			auto pSocketInfo=pServerUserItem->GetSocketInfo();

			//У�������
			if (m_WordFilter.FilterWords(pCreateClub->szClubName)==true)
			{
				//��ȡ����
				CMD_CP_S_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure,sizeof(OperateFailure));

				//���ñ���		
				StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0002, TEXT("��Ǹ, ���������а������дʻ�, ���紴��ʧ�ܣ�")));

				//��������
				WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
				wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
				SendDataToGate(pSocketInfo->wGateID,MDM_CLUBPLAZA_SERVICE,SUB_CP_S_OPERATE_FAILURE,&OperateFailure,wPacketSize,&dwTokenID,1);				

				return true;
			}

			//У��������
			if (m_WordFilter.FilterWords(pCreateClub->szClubIntroduce)==true)
			{
				//��ȡ����
				CMD_CP_S_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure,sizeof(OperateFailure));

				//���ñ���		
				StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0003, TEXT("��Ǹ,��������а������дʻ�,���紴��ʧ�ܣ�")));

				//��������
				WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
				wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
				SendDataToGate(pSocketInfo->wGateID,MDM_CLUBPLAZA_SERVICE,SUB_CP_S_OPERATE_FAILURE,&OperateFailure,wPacketSize,&dwTokenID,1);				

				return true;
			}

			//����ṹ
			DBR_GR_CreateClub CreateClub;
			ZeroMemory(&CreateClub,sizeof(CreateClub));

			//Ⱥ����Ϣ
			CreateClub.dwCreateID= pTokenParameter->dwUserID;

			//Ⱥ������
			CreateClub.wClubKind= pCreateClub->wClubKind;
			CreateClub.wClubIconID = pCreateClub->wClubIconID;
			StringCchCopy(CreateClub.szClubName,CountArray(CreateClub.szClubName), pCreateClub->szClubName);
			StringCchCopy(CreateClub.szClubIntroduce,CountArray(CreateClub.szClubIntroduce), pCreateClub->szClubIntroduce);

			//Ⱥ�����
			CreateClub.cbPayType = pCreateClub->cbPayType;
			CreateClub.dwSwitchOption = pCreateClub->dwSwitchOption;
			CreateClub.cbCurrencyKind = pCreateClub->cbCurrencyKind;		

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_CREATE_CLUB, CreateClub.dwCreateID,dwSocketID,dwTokenID,&CreateClub,sizeof(CreateClub));

			return true;
		}
		case SUB_CP_C_UPDATE_CLUB:		//����Ⱥ��
		{
			//����У��
			ASSERT (wDataSize>=sizeof(CMD_CP_C_UpdateClub));
			if (wDataSize<sizeof(CMD_CP_C_UpdateClub)) return true;

			//��ȡ����
			auto pUpdateClub=(CMD_CP_C_UpdateClub *)pData;

			//��ѯ�û�
			auto pServerUserItem=m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
			if (pServerUserItem==NULL) return true;

			//��������
			LPBYTE pcbBuffer=(LPBYTE)pData;
			WORD wOffsetPos=sizeof(CMD_CP_C_UpdateClub);

			//���¹���
			if ((pUpdateClub->cbUpdateKind&UPCLUB_KIND_RULES) != 0)
			{
				//����ƫ��
				wOffsetPos+=sizeof(tagClubUpdate_Rules);
			}

			//���¹���
			if ((pUpdateClub->cbUpdateKind & UPCLUB_KIND_NOTICE) != 0)
			{
				//��ȡ����
				auto pClubNotice = (tagClubUpdate_Notice*)(pcbBuffer + wOffsetPos);
				pClubNotice->szNoticeContent[CountArray(pClubNotice->szNoticeContent) - 1] = 0;
				
				//У�鹫������
				if (m_WordFilter.FilterWords(pClubNotice->szNoticeContent) == true)
				{
					//��ȡ����
					tagSocketInfo* pSocketInfo = pServerUserItem->GetSocketInfo();

					//��ȡ����
					CMD_CP_S_OperateFailure OperateFailure;
					ZeroMemory(&OperateFailure, sizeof(OperateFailure));

					//���ñ���		
					StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0016, TEXT("��Ǹ,���������а������дʻ�,�޸�ʧ�ܣ�")));

					//��������
					WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
					wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
					SendDataToGate(pSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_OPERATE_FAILURE, &OperateFailure, wPacketSize, &dwTokenID, 1);

					return true;
				}

				//����ƫ��
				wOffsetPos += sizeof(tagClubUpdate_Notice);
			}

			//��������
			if ((pUpdateClub->cbUpdateKind&UPCLUB_KIND_PROPERTY)!=0)
			{
				//��ȡ����
				auto pClubProperty=(tagClubUpdate_Property *)(pcbBuffer+wOffsetPos);
				pClubProperty->szClubName[CountArray(pClubProperty->szClubName)-1] = 0;
				pClubProperty->szClubIntroduce[CountArray(pClubProperty->szClubIntroduce)-1]=0;

				//У���������
				if (m_WordFilter.FilterWords(pClubProperty->szClubName) == true)
				{
					//��ȡ����
					tagSocketInfo * pSocketInfo = pServerUserItem->GetSocketInfo();

					//��ȡ����
					CMD_CP_S_OperateFailure OperateFailure;
					ZeroMemory(&OperateFailure, sizeof(OperateFailure));

					//���ñ���		
					StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0004, TEXT("��Ǹ,���������а������дʻ�,�޸�ʧ�ܣ�")));

					//��������
					WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
					wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
					SendDataToGate(pSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_OPERATE_FAILURE, &OperateFailure, wPacketSize, &dwTokenID, 1);

					return true;
				}

				//У��������
				if (m_WordFilter.FilterWords(pClubProperty->szClubIntroduce)==true)
				{
					//��ȡ����
					tagSocketInfo * pSocketInfo=pServerUserItem->GetSocketInfo();

					//��ȡ����
					CMD_CP_S_OperateFailure OperateFailure;
					ZeroMemory(&OperateFailure,sizeof(OperateFailure)); 

					//���ñ���		
					StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0005, TEXT("��Ǹ,��������а������дʻ�,�޸�ʧ�ܣ�")));

					//��������
					WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
					wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
					SendDataToGate(pSocketInfo->wGateID,MDM_CLUBPLAZA_SERVICE,SUB_CP_S_OPERATE_FAILURE,&OperateFailure,wPacketSize,&dwTokenID,1);				

					return true;
				}
			}

			//����ṹ
			auto pDBUpdateClub=(DBR_GR_UpdateClub *)m_cbBuffer;

			//Ⱥ����Ϣ
			pDBUpdateClub->dwClubID=pUpdateClub->dwClubID;
			pDBUpdateClub->dwUserID = pTokenParameter->dwUserID;
			pDBUpdateClub->cbUpdateKind=pUpdateClub->cbUpdateKind;			

			//��������
			CopyMemory(pDBUpdateClub+1,pUpdateClub+1,wDataSize-sizeof(CMD_CP_C_UpdateClub));

			//Ͷ������
			WORD wPacketSize=sizeof(DBR_GR_UpdateClub)+wDataSize-sizeof(CMD_CP_C_UpdateClub);		
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_UPDATE_CLUB, pDBUpdateClub->dwUserID, dwSocketID,dwTokenID,m_cbBuffer,wPacketSize);

			return true;
		}
		case SUB_CP_C_DELETE_CLUB:		//ɾ��Ⱥ��
		{
			//����У��
			ASSERT (wDataSize==sizeof(CMD_CP_C_DeleteClub));
			if (wDataSize!=sizeof(CMD_CP_C_DeleteClub)) return true;

			//��ȡ����
			auto  pDeleteClub=(CMD_CP_C_DeleteClub *)pData;

			//����ṹ
			DBR_GR_DeleteClub DeleteClub;
			ZeroMemory(&DeleteClub,sizeof(DeleteClub));

			//Ⱥ���ʶ
			DeleteClub.dwClubID=pDeleteClub->dwClubID;				
			DeleteClub.dwCreaterID= pTokenParameter->dwUserID;				

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_CLUB, DeleteClub.dwCreaterID,dwSocketID,dwTokenID,&DeleteClub,sizeof(DeleteClub));

			return true;
		}				
		case SUB_CP_C_UPDATE_MEMBER:	//���³�Ա
		{
			//����У��
			ASSERT (wDataSize >= sizeof(CMD_CP_C_UpdateMember));
			if (wDataSize < sizeof(CMD_CP_C_UpdateMember)) return true;

			//��ȡ����
			auto pIMUpdateMember = (CMD_CP_C_UpdateMember *)pData;

			//��ѯ�û�
			auto pServerUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
			if (pServerUserItem == NULL) return true;

			//��ȡ����
			auto pIMSocketInfo = pServerUserItem->GetSocketInfo();

			//���³�Ա����
			if (pIMUpdateMember->wUpdateKind == UPMEMBER_KIND_TYPE)
			{
				//��Ա����
				BYTE cbMemberType = *(BYTE *)(pIMUpdateMember + 1);

				//У������
				if (cbMemberType > MEMBER_TYPE_EMPLOYER)
				{
					//��ȡ����
					CMD_CP_S_OperateFailure OperateFailure;
					ZeroMemory(&OperateFailure, sizeof(OperateFailure));

					//���ñ���		
					StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0006, TEXT("��Ǹ,��Ա�����쳣,����ʧ�ܣ�")));

					//��������
					WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
					wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
					SendDataToGate(pIMSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_OPERATE_FAILURE, &OperateFailure, wPacketSize, &pIMSocketInfo->dwTokenID, 1);

					return true;
				}
			}

			//���³�Ա״̬
			if (pIMUpdateMember->wUpdateKind == UPMEMBER_KIND_STATUS)
			{
				//��Ա����
				BYTE cbMemberStatus = *(BYTE *)(pIMUpdateMember + 1);

				//У��״̬
				if (cbMemberStatus > MEMBER_STATUS_FREEZE)
				{
					//��ȡ����
					CMD_CP_S_OperateFailure OperateFailure;
					ZeroMemory(&OperateFailure, sizeof(OperateFailure));

					//���ñ���		
					StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0007, TEXT("��Ǹ,��Ա״̬�쳣,����ʧ�ܣ�")));

					//��������
					WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
					wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
					SendDataToGate(pIMSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_OPERATE_FAILURE, &OperateFailure, wPacketSize, &pIMSocketInfo->dwTokenID, 1);

					return true;
				}
			}

			//����ṹ
			auto pUpdateMember = (DBR_GR_UpdateMember *)m_cbBuffer;

			//Ⱥ����Ϣ
			pUpdateMember->dwClubID = pIMUpdateMember->dwClubID;
			pUpdateMember->dwMemberID = pIMUpdateMember->dwMemberID;

			//�û���Ϣ
			pUpdateMember->dwUserID = pTokenParameter->dwUserID;
			pUpdateMember->wUpdateKind = pIMUpdateMember->wUpdateKind;

			//��������
			CopyMemory(pUpdateMember+1, pIMUpdateMember+1, wDataSize-sizeof(CMD_CP_C_UpdateMember));

			//Ͷ������
			WORD wPacketSize = sizeof(DBR_GR_UpdateMember)+wDataSize-sizeof(CMD_CP_C_UpdateMember);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_UPDATE_MEMBER, pUpdateMember->dwUserID, dwSocketID, dwTokenID, m_cbBuffer, wPacketSize);

			return true;
		}
		case SUB_CP_C_DELETE_MEMBER:	//ɾ����Ա
		{
			//����У��
			ASSERT (wDataSize==sizeof(CMD_CP_C_DeleteMember));
			if (wDataSize!=sizeof(CMD_CP_C_DeleteMember)) return true;

			//��ȡ����
			auto pDeleteMember=(CMD_CP_C_DeleteMember *)pData;

			//����ṹ
			DBR_GR_DeleteMember DeleteMember;
			ZeroMemory(&DeleteMember,sizeof(DeleteMember));

			//Ⱥ����Ϣ
			DeleteMember.dwClubID=pDeleteMember->dwClubID;
			DeleteMember.dwMemberID=pDeleteMember->dwMemberID;

			//������Ϣ
			DeleteMember.cbDeleteKind=pDeleteMember->cbDeleteKind;

			//�û���Ϣ
			DeleteMember.dwUserID= pTokenParameter->dwUserID;
				
			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_MEMBER, DeleteMember.dwUserID,dwSocketID,dwTokenID,&DeleteMember,sizeof(DeleteMember));

			return true;
		}		
		case SUB_CP_C_APPLY_REQUEST:	//��������
		{
			//����У��
			ASSERT (wDataSize==sizeof(CMD_CP_C_ApplyRequest));
			if (wDataSize!=sizeof(CMD_CP_C_ApplyRequest)) return true;

			//��ȡ����
			auto pApplyRequest=(CMD_CP_C_ApplyRequest *)pData;

			//����ṹ
			DBR_GR_ApplyRequest ApplyRequest;
			ZeroMemory(&ApplyRequest,sizeof(ApplyRequest));

			//Ⱥ����Ϣ			
			ApplyRequest.dwClubID=pApplyRequest->dwClubID;
			ApplyRequest.dwApplyerID= pTokenParameter->dwUserID;
				
			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_APPLY_REQUEST, ApplyRequest.dwApplyerID,dwSocketID,dwTokenID,&ApplyRequest,sizeof(ApplyRequest));

			return true;
		}
		case SUB_CP_C_APPLY_RESPOND:	//������Ӧ
		{
			//����У��
			ASSERT (wDataSize==sizeof(CMD_CP_C_ApplyRespond));
			if (wDataSize!=sizeof(CMD_CP_C_ApplyRespond)) return true;

			//��ȡ����
			auto pApplyRespond=(CMD_CP_C_ApplyRespond *)pData;

			//����ṹ
			DBR_GR_ApplyRespond ApplyRespond;
			ZeroMemory(&ApplyRespond,sizeof(ApplyRespond));

			//���ñ���
			ApplyRespond.dwMessageID=pApplyRespond->dwMessageID;

			//Ⱥ����Ϣ
			ApplyRespond.dwClubID=pApplyRespond->dwClubID;
			ApplyRespond.dwApplyerID=pApplyRespond->dwApplyerID;
			ApplyRespond.cbApplyStatus=pApplyRespond->cbApplyStatus;			

			//�û���Ϣ
			ApplyRespond.dwUserID= pTokenParameter->dwUserID;
				
			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_APPLY_RESPOND, ApplyRespond.dwUserID,dwSocketID,dwTokenID,&ApplyRespond,sizeof(ApplyRespond));

			return true;
		}
		case SUB_CP_C_APPLY_DELETE:		//����ɾ��
		{
			//����У��
			ASSERT (wDataSize==sizeof(CMD_CP_C_ApplyDelete));
			if (wDataSize!=sizeof(CMD_CP_C_ApplyDelete)) return true;

			//��ȡ����
			auto pApplyDelete=(CMD_CP_C_ApplyDelete *)pData;			

			//����ṹ
			DBR_GR_ApplyDelete ApplyDelete;
			ZeroMemory(&ApplyDelete,sizeof(ApplyDelete));

			//���ñ���
			ApplyDelete.dwClubID = pApplyDelete->dwClubID;
			ApplyDelete.dwUserID = pTokenParameter->dwUserID;			

			//���ñ���
			ApplyDelete.dwMessageID=pApplyDelete->dwMessageID;
			ApplyDelete.cbMessageType=pApplyDelete->cbMessageType;
			
			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_APPLY_DELETE, ApplyDelete.dwUserID,dwSocketID,dwTokenID,&ApplyDelete,sizeof(ApplyDelete));

			return true;
		}
		case SUB_CP_C_APPLY_REPEAL:		//���볷��
		{
			//����У��
			ASSERT (wDataSize==sizeof(CMD_CP_C_ApplyRepeal));
			if (wDataSize!=sizeof(CMD_CP_C_ApplyRepeal)) return true;

			//��ȡ����
			auto pApplyRepeal=(CMD_CP_C_ApplyRepeal *)pData;			

			//����ṹ
			DBR_GR_ApplyRepeal ApplyRepeal;
			ZeroMemory(&ApplyRepeal,sizeof(ApplyRepeal));

			//���ñ���
			ApplyRepeal.dwMessageID=pApplyRepeal->dwMessageID;

			//���ñ���
			ApplyRepeal.dwUserID= pTokenParameter->dwUserID;
				
			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_APPLY_REPEAL, ApplyRepeal.dwUserID,dwSocketID,dwTokenID,&ApplyRepeal,sizeof(ApplyRepeal));

			return true;
		}
		case SUB_CP_C_SETTLE_BATTLE:	//Լս����
		{
			//����У��
			ASSERT (wDataSize==sizeof(CMD_CP_C_SettleBattle));
			if (wDataSize!=sizeof(CMD_CP_C_SettleBattle)) return true;

			//��ȡ����
			auto pSettleBattle=(CMD_CP_C_SettleBattle *)pData;

			//����ṹ
			DBR_GR_SettleBattle SettleBattle;
			ZeroMemory(&SettleBattle,sizeof(SettleBattle));

			//���ñ���
			SettleBattle.dwDateID=pSettleBattle->dwDateID;
			SettleBattle.dwClubID=pSettleBattle->dwClubID;
			SettleBattle.dwMemberID=pSettleBattle->dwMemberID;
			SettleBattle.wSettleCount=pSettleBattle->wSettleCount;

			//�û���Ϣ
			SettleBattle.dwCreaterID= pTokenParameter->dwUserID;
				
			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SETTLE_BATTLE, SettleBattle.dwCreaterID,dwSocketID,dwTokenID,&SettleBattle,sizeof(SettleBattle));

			return true;
		}	
		case SUB_CP_C_MODIFY_SCORE:		//�޸Ļ���
		{
			//����У��
			ASSERT(wDataSize == sizeof(CMD_CP_C_ModifyScore));
			if (wDataSize != sizeof(CMD_CP_C_ModifyScore)) return true;

			//��ȡ����
			auto pModifyScore = (CMD_CP_C_ModifyScore *)pData;

			//����ṹ
			DBR_GR_ModifyScore ModifyScore;
			ZeroMemory(&ModifyScore, sizeof(ModifyScore));

			//���ñ���				
			ModifyScore.dwUserID = pTokenParameter->dwUserID;
			ModifyScore.dwClubID = pModifyScore->dwClubID;
			ModifyScore.dwMemberID = pModifyScore->dwMemberID;
			ModifyScore.lModifyScore= pModifyScore->lModifyScore;				
				
			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MODIFY_SCORE, ModifyScore.dwUserID,dwSocketID, dwTokenID, &ModifyScore, sizeof(ModifyScore));

			return true;
		}
		case SUB_CP_C_DELETE_RECORD:	//ɾ����¼
		{
			//����У��
			ASSERT(wDataSize == sizeof(CMD_CP_C_DeleteRecord));
			if (wDataSize != sizeof(CMD_CP_C_DeleteRecord)) return true;

			//��ȡ����
			auto pDeleteRecord = (CMD_CP_C_DeleteRecord*)pData;

			//����ṹ
			DBR_GR_DeleteRecord DeleteRecord = {};

			//���ñ���
			DeleteRecord.dwClubID = pDeleteRecord->dwClubID;
			DeleteRecord.dwUserID = pTokenParameter->dwUserID;
			DeleteRecord.dwRecordID = pDeleteRecord->dwRecordID;
			DeleteRecord.cbRecordKind = pDeleteRecord->cbRecordKind;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_RECORD, pTokenParameter->dwUserID, dwSocketID, dwTokenID, &DeleteRecord, sizeof(DeleteRecord));

			return true;
		}
		case SUB_CP_C_DETECT_CLUBNAME:	//�������
		{
			//����У��
			ASSERT(wDataSize == sizeof(CMD_CP_C_DetectClubName));
			if (wDataSize != sizeof(CMD_CP_C_DetectClubName)) return true;

			//��ȡ����
			auto pDetectClubName = (CMD_CP_C_DetectClubName*)pData;

			//У�������
			if (m_WordFilter.FilterWords(pDetectClubName->szClubName) == true)
			{
				//��ȡ����
				CMD_CP_S_DetectResult DetectResult = {};

				//���ñ���		
				DetectResult.bUsable = false;
				StringCchCopy(DetectResult.szErrorDescribe, CountArray(DetectResult.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0001, TEXT("�������ư������дʻ㣡")));

				//��������
				SendDataToGate(pBindParameter->wServiceID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_DETECT_RESULT, &DetectResult, sizeof(DetectResult), &dwTokenID, 1);

				return true;
			}

			//����ṹ
			DBR_GR_DetectClubName DetectClubName = {};
			
			//���ñ���
			StringCchCopy(DetectClubName.szClubName, CountArray(DetectClubName.szClubName) ,pDetectClubName->szClubName);

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DETECT_CLUBNAME, pTokenParameter->dwUserID, dwSocketID, dwTokenID, &DetectClubName, sizeof(DetectClubName));

			return true;
		}		
		}
	}
	catch (...)
	{
		//������ʾ
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("OnTCPNetworkMainClubPlazaService(SubCmdID=%d)"),wSubCmdID);

		//��ʾ��Ϣ
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);
	}

	return true;
}

//ע�����
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{		
	case SUB_CS_C_REGISTER_SERVICE:	//ע������
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterService));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterService)) return false;

			//��Ϣ����
			auto pRegisterService =(CMD_CS_C_RegisterService *)pData;

			//У��ģ��
			if (pRegisterService->wServiceModule != SERVICE_MODULE_GATE) return false;

			//��������
			if (m_GlobalItemManager.SearchGateItem(pRegisterService->wServiceID)!=NULL)
			{
				//��������
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//���ñ���
				RegisterFailure.lErrorCode=0;
				StringCchCopy(RegisterFailure.szDescribeString,CountArray(RegisterFailure.szDescribeString),TEXT("�Ѿ�������ͬ��ʶ�����ط������ط���ע��ʧ��"));

				//������Ϣ
				WORD wStringSize=CountStringBuffer(RegisterFailure.szDescribeString);
				WORD wSendSize=sizeof(RegisterFailure)-sizeof(RegisterFailure.szDescribeString)+wStringSize;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_FAILURE,&RegisterFailure,wSendSize,NULL,0);

				//�ж�����
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//���ð�
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->wServiceModule= SERVICE_MODULE_GATE;
			(m_pBindParameter+wBindIndex)->wServiceID= pRegisterService->wServiceID;

			//��������
			tagServiceItem ServiceItem;
			ZeroMemory(&ServiceItem,sizeof(ServiceItem));

			//��������
			ServiceItem.wServiceID= pRegisterService->wServiceID;
			ServiceItem.wServiceModule = SERVICE_MODULE_GATE;
			ServiceItem.wServicePort= pRegisterService->wServicePort;
			ServiceItem.wMaxConnect= pRegisterService->wMaxConnect;
			ServiceItem.dwServiceAddr= pRegisterService->dwServiceAddr;
			StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), pRegisterService->szServiceName);
			StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain), pRegisterService->szServiceDomain);

			//ע�᷿��
			m_GlobalItemManager.ActiveGateItem(dwSocketID, ServiceItem);

			//ע��ɹ�
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_SUCCESS,NULL,0);			

			//Ⱥ������
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);			

			return true;
		}
	}

	return false;
}

//����״̬
bool CAttemperEngineSink::OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_LOGOUT_TOKEN:
	{
		//���ð�
		WORD wBindIndex = LOWORD(dwSocketID);
		auto pBindParameter = (m_pBindParameter + wBindIndex);

		//У�����
		if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return true;

		//��ȡ����
		auto pLogoutToken = (CMD_CS_C_LogoutToken*)pData;

		//��������
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem != NULL)
		{
			//�󶨲���
			auto pTokenParameter = pGlobalGateItem->SearchTokenID(pLogoutToken->dwTokenID);
			if (pTokenParameter != NULL)
			{
				//�Ƴ��û�
				m_ClubUserManager.DeleteUserItem(pTokenParameter->dwUserID);				

				//����ṹ
				CMD_CS_C_LeaveClubPlaza LeaveClubPlaza = {};

				//���ñ���
				LeaveClubPlaza.dwUserID = pTokenParameter->dwUserID;

				//�㲥��Ϣ
				m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_LEAVE_CLUBPLAZA, &LeaveClubPlaza, sizeof(LeaveClubPlaza), 0);
			}			

			//�Ƴ�����
			pGlobalGateItem->RemoveTokenID(pLogoutToken->dwTokenID);
		}

		return true;
	}
	case SUB_CS_C_REGISTER_TOKEN:
	{
		//���ð�
		WORD wBindIndex = LOWORD(dwSocketID);
		auto pBindParameter = (m_pBindParameter + wBindIndex);

		//�Ƴ�����
		if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return true;

		//��ȡ����
		auto pRegisterToken = (CMD_CS_C_RegisterToken*)pData;

		//����ṹ
		CMD_CS_S_RegisterToken RegisterToken;
		ZeroMemory(&RegisterToken, sizeof(RegisterToken));

		//Ĭ�ϳɹ�
		RegisterToken.lResultCode = REGISTER_RESULT_FAILURE;

		//��������
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem != NULL)
		{
			//��ѯ�û�
			auto pTokenParameter = pGlobalGateItem->ActiveTokenID(pRegisterToken->dwTokenID);
			if (pTokenParameter != NULL)
			{
				//ע��ɹ�
				RegisterToken.lResultCode = REGISTER_RESULT_SUCCESS;

				//��������		
				pTokenParameter->dwUserID = pRegisterToken->dwUserID;
				pTokenParameter->dwClientIP = pRegisterToken->dwClientIP;
			}
		}

		//��������
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_REGISTER_TOKEN, &RegisterToken, sizeof(RegisterToken), &pRegisterToken->dwTokenID, 1);

		return true;
	}
	}
	return true;
}

//��ȡ�ṹ
tagForwardHead* CAttemperEngineSink::GetForwardHead()
{
	auto pForwardHead=(tagForwardHead*)m_cbBuffer;
	ZeroMemory(pForwardHead,sizeof(tagForwardHead));

	return pForwardHead;
}

//��������
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//��������
	auto pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->m_UIControlSink.PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//���Ʋ���
tagTokenParameter* CAttemperEngineSink::GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ��Ϣ
	WORD wBindIndex = LOWORD(dwSocketID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);
	if (pBindParameter == NULL) return NULL;

	//ת��ģʽ
	if (pBindParameter->wServiceModule == SERVICE_MODULE_GATE)
	{
		//��ѯ����
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem == NULL) return NULL;

		//��ѯ�û�
		auto pTokenParameter = pGlobalGateItem->SearchTokenID(dwTokenID);
		if (pTokenParameter == NULL || pTokenParameter->dwTokenID != dwTokenID) return NULL;

		return pTokenParameter;
	}

	return NULL;
}

//���ط���
bool CAttemperEngineSink::SendDataToGate(WORD wServerID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD *pdwTokenID, WORD wTokenCount)
{
	//��������
	auto pGlobalGateItem=m_GlobalItemManager.SearchGateItem(wServerID);
	if (pGlobalGateItem==NULL) return false;

	//��������
	m_pITCPNetworkEngine->SendData(pGlobalGateItem->GetSocketID(),MAKECMMD(wMainCmdID,SERVICE_MODULE_CLUBPLAZA),wSubCmdID,pData,wDataSize,pdwTokenID,wTokenCount);

	return true;
}

//��¼�ɹ�
bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT (wDataSize>=sizeof(DBO_GR_LogonSuccess));
	if (wDataSize<sizeof(DBO_GR_LogonSuccess)) return false;

	//��ȡ����
	auto pLogonSuccess=(DBO_GR_LogonSuccess *)pData;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);

	//����ṹ
	CMD_CS_C_EnterClubPlaza EnterClubPlaza = {};
	
	//���ñ���
	EnterClubPlaza.dwUserID = pLogonSuccess->dwUserID;

	//�㲥��Ϣ
	m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_ENTER_CLUBPLAZA, &EnterClubPlaza, sizeof(EnterClubPlaza), 0);

	//��¼�ɹ�
	SendDataToGate(pBindParameter->wServiceID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_LOGON_SUCCESS, NULL, 0, &dwTokenID, 1);

	//����ṹ
	CMD_CP_S_ClubOption ClubOption;
	ZeroMemory(&ClubOption, sizeof(ClubOption));

	//��������
	CopyMemory(&ClubOption.ClubOption, &m_ClubOption, sizeof(tagClubOption));

	//��������
	SendDataToGate(pBindParameter->wServiceID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_OPTION, &ClubOption, sizeof(ClubOption), &dwTokenID, 1);	

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBLogonFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT (wDataSize<=sizeof(DBO_GR_LogonFailure));
	if (wDataSize>sizeof(DBO_GR_LogonFailure)) return false;

	//��ȡ����
	auto pLogonFailure=(DBO_GR_LogonFailure *)pData;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);

	//��ȡ����
	CMD_CP_S_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));

	//���ñ���			
	LogonFailure.lErrorCode=pLogonFailure->lErrorCode;			
	StringCchCopy(LogonFailure.szErrorDescribe,CountArray(LogonFailure.szErrorDescribe), pLogonFailure->szErrorDescribe);

	//��������
	WORD wPacketSize=CountStringBuffer(LogonFailure.szErrorDescribe);
	wPacketSize+=sizeof(LogonFailure)-sizeof(LogonFailure.szErrorDescribe);
	SendDataToGate(pBindParameter->wServiceID,MDM_CLUBPLAZA_SERVICE,SUB_CP_S_LOGON_FAILURE,&LogonFailure,wPacketSize,&dwTokenID,1);
	
	return true;
}

//��¼���
bool CAttemperEngineSink::OnDBLogonFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);

	//��������
	SendDataToGate(pBindParameter->wServiceID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_LOGON_FINISH, NULL, 0, &dwTokenID, 1);

	return true;
}

//�����б�
bool CAttemperEngineSink::OnDBClubNameCard(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);

	//��������
	SendDataToGate(pBindParameter->wServiceID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_NAMECARD, pData, wDataSize, &dwTokenID, 1);

	return true;
}

//����Ⱥ��
bool CAttemperEngineSink::OnDBCreateClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT (wDataSize==sizeof(DBO_GR_CreateClub));
	if (wDataSize!=sizeof(DBO_GR_CreateClub)) return false;

	//��ȡ����
	auto pCreateClub=(DBO_GR_CreateClub *)pData;

	//����·��
	CMD_CS_C_ActiveRouteItem ActiveRouteItem = {};

	//���ñ���
	ActiveRouteItem.dwRouteID = pCreateClub->ClubNameCard.dwClubID;
	ActiveRouteItem.wServiceKind = 0;
	ActiveRouteItem.wServiceModule = SERVICE_MODULE_CLUB;

	//������Ϣ
	m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE,SUB_CS_C_ACTIVE_ROUTE_ITEM,&ActiveRouteItem,sizeof(ActiveRouteItem),0);

	//��ѯ�û�
	auto pClubUserItem=m_ClubUserManager.SearchUserItem(pCreateClub->ClubNameCard.dwCreaterID);
	if (pClubUserItem !=NULL)
	{
		//������Ϣ
		auto pSocketInfo = pClubUserItem->GetSocketInfo();

		//��������
		SendDataToGate(pSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_NAMECARD, &pCreateClub->ClubNameCard, sizeof(pCreateClub->ClubNameCard), &pSocketInfo->dwTokenID,1);
	}

	return true;
}

//����Ⱥ��
bool CAttemperEngineSink::OnDBUpdateClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT(wDataSize >= sizeof(DBO_GR_UpdateClub));
	if (wDataSize < sizeof(DBO_GR_UpdateClub)) return false;

	//��ȡ����
	auto pDBUpdateClub = (DBO_GR_UpdateClub *)pData;

	//����ṹ
	auto pForwardHead = GetForwardHead();

	//���ñ���	
	pForwardHead->wItemCount = 1;
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CLUB;
	pForwardHead->ForwardItem[0].dwHashKeyID = pDBUpdateClub->dwClubID;

	//��ȡ����
	auto pUpdateClub = (CMD_CS_CB_UpdateClub *)(pForwardHead + 1);

	//���ñ���
	pUpdateClub->dwClubID = pDBUpdateClub->dwClubID;
	pUpdateClub->cbUpdateKind = pDBUpdateClub->cbUpdateKind;
	CopyMemory(pUpdateClub + 1, pDBUpdateClub + 1, wDataSize - sizeof(DBO_GR_UpdateClub));

	//�㲥��Ϣ
	WORD wPacketSize = sizeof(tagForwardHead);
	wPacketSize += sizeof(CMD_CS_CB_UpdateClub) + wDataSize - sizeof(DBO_GR_UpdateClub);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_CB_UPDATE_CLUB, pForwardHead, wPacketSize, 0);

	return true;
}

//ɾ��Ⱥ��
bool CAttemperEngineSink::OnDBDeleteClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT (wDataSize == sizeof(DBO_GR_DeleteClub));
	if (wDataSize != sizeof(DBO_GR_DeleteClub)) return false;

	//��ȡ����
	auto pDBDeleteClub = (DBO_GR_DeleteClub *)pData;

	//����ṹ
	auto pForwardHead = GetForwardHead();

	//���ñ���	
	pForwardHead->wItemCount = 1;
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CLUB;
	pForwardHead->ForwardItem[0].dwHashKeyID = pDBDeleteClub->dwClubID;

	//��ȡ����
	auto pDeleteClub = (CMD_CS_CB_DeleteClub *)(pForwardHead + 1);
	ZeroMemory(pDeleteClub, sizeof(CMD_CS_CB_DeleteClub));

	//���ñ���
	pDeleteClub->dwClubID = pDBDeleteClub->dwClubID;

	//�㲥��Ϣ
	WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_CB_DeleteClub);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_CB_DELETE_CLUB, pForwardHead, wPacketSize, 0);	

	return true;
}

//����Ⱥ��
bool CAttemperEngineSink::OnDBSearchClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT (wDataSize == sizeof(DBO_GR_SearchClub));
	if (wDataSize != sizeof(DBO_GR_SearchClub)) return false;

	//��ȡ����
	auto pSearchClub = (DBO_GR_SearchClub *)pData;
	
	//��ѯ�û�
	auto pClubUserItem = m_ClubUserManager.SearchUserItem(pSearchClub->dwUserID);
	if (pClubUserItem != NULL)
	{
		//����ṹ
		CMD_CP_S_SearchResult SearchResult = {};		
		CopyMemory(&SearchResult.ClubNameCard, &pSearchClub->ClubNameCard, sizeof(SearchResult.ClubNameCard));

		//������Ϣ
		auto pSocketInfo = pClubUserItem->GetSocketInfo();

		//������Ϣ
		SendDataToGate(pSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_SEARCH_RESULT,&SearchResult,sizeof(SearchResult),&pSocketInfo->dwTokenID,1);
	}	

	return true;
}

//��ӳ�Ա
bool CAttemperEngineSink::OnDBInsertMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT (wDataSize==sizeof(DBO_GR_InsertMember));
	if (wDataSize!=sizeof(DBO_GR_InsertMember)) return false;

	//��ȡ����
	auto pInsertMember=(DBO_GR_InsertMember *)pData;

	//����ṹ
	auto pForwardHead=GetForwardHead();

	//����ͷ��
	pForwardHead->wItemCount = 1;
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CLUB;
	pForwardHead->ForwardItem[0].dwHashKeyID = pInsertMember->dwClubID;

	//��ȡ����
	auto pIMInsertMember=(CMD_CS_CB_InsertMember *)(pForwardHead+1);
	ZeroMemory(pIMInsertMember,sizeof(CMD_CS_CB_InsertMember));

	//���ñ���
	pIMInsertMember->dwClubID=pInsertMember->dwClubID;
	pIMInsertMember->wMemberCount = pInsertMember->ClubNameCard.wMemberCount;
	CopyMemory(&pIMInsertMember->MemberUser,&pInsertMember->ClubMemberUser,sizeof(pIMInsertMember->MemberUser));

	//�㲥��Ϣ
	WORD wPacketSize=sizeof(tagForwardHead)+sizeof(CMD_CS_CB_InsertMember);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE,SUB_CS_CB_INSERT_MEMBER,pForwardHead,wPacketSize,0);

	//��ѯ�û�
	auto pClubUserItem=m_ClubUserManager.SearchUserItem(pInsertMember->ClubMemberUser.UserInfo.dwUserID);
	if (pClubUserItem !=NULL)
	{
		//������Ϣ
		auto pSocketInfo = pClubUserItem->GetSocketInfo();

		//��������
		SendDataToGate(pSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_NAMECARD, &pInsertMember->ClubNameCard, sizeof(pInsertMember->ClubNameCard), &pSocketInfo->dwTokenID, 1);
	}

	return true;
}

//���³�Ա
bool CAttemperEngineSink::OnDBUpdateMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT (wDataSize == sizeof(DBO_GR_UpdateMember));
	if (wDataSize != sizeof(DBO_GR_UpdateMember)) return false;

	//��ȡ����
	auto pUpdateMember = (DBO_GR_UpdateMember *)pData;

	//����ṹ
	auto pForwardHead = GetForwardHead();

	//����ͷ��
	pForwardHead->wItemCount = 1;
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CLUB;
	pForwardHead->ForwardItem[0].dwHashKeyID = pUpdateMember->dwClubID;

	//��ȡ����
	auto pIMUpdateMember = (CMD_CS_CB_UpdateMember *)(pForwardHead + 1);
	ZeroMemory(pIMUpdateMember, sizeof(CMD_CS_CB_UpdateMember));

	//���ñ���
	pIMUpdateMember->dwClubID = pUpdateMember->dwClubID;
	pIMUpdateMember->wUpdateKind = pUpdateMember->wUpdateKind;
	CopyMemory(&pIMUpdateMember->ClubMember, &pUpdateMember->ClubMember, sizeof(pIMUpdateMember->ClubMember));

	//�㲥��Ϣ
	WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_CB_UpdateMember);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_CB_UPDATE_MEMBER, pForwardHead, wPacketSize, 0);	

	return true;
}

//ɾ����Ա
bool CAttemperEngineSink::OnDBDeleteMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT (wDataSize==sizeof(DBO_GR_DeleteMember));
	if (wDataSize!=sizeof(DBO_GR_DeleteMember)) return false;

	//��ȡ����
	auto pDeleteMember=(DBO_GR_DeleteMember *)pData;

	//����ṹ
	auto pForwardHead=GetForwardHead();

	//����ͷ��
	pForwardHead->wItemCount = 1;
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CLUB;
	pForwardHead->ForwardItem[0].dwHashKeyID = pDeleteMember->dwClubID;	

	//��ȡ����
	auto pIMDeleteMember=(CMD_CS_CB_DeleteMember *)(pForwardHead+1);
	ZeroMemory(pIMDeleteMember,sizeof(CMD_CS_CB_DeleteMember));

	//���ñ���
	pIMDeleteMember->dwClubID=pDeleteMember->dwClubID;
	pIMDeleteMember->dwMemberID=pDeleteMember->dwMemberID;
	pIMDeleteMember->wMemberCount = pDeleteMember->wMemberCount;

	//�㲥��Ϣ
	WORD wPacketSize=sizeof(tagForwardHead)+sizeof(CMD_CS_CB_DeleteMember);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE,SUB_CS_CB_DELETE_MEMBER,pForwardHead,wPacketSize,0);

	//�Ƴ�Ⱥ��
	auto pServerUserItem = m_ClubUserManager.SearchUserItem(pDeleteMember->dwMemberID);
	if (pServerUserItem != NULL)
	{
		//��ȡ����		
		auto pSocketInfo = pServerUserItem->GetSocketInfo();

		//����ṹ
		CMD_CP_S_ClubDelete  ClubDelete;
		ZeroMemory(&ClubDelete,sizeof(ClubDelete));

		//���ñ���
		ClubDelete.dwClubID = pDeleteMember->dwClubID;

		//�ϰ�����
		if (pDeleteMember->cbDeleteKind == 1)
		{
			StringCchCopy(ClubDelete.szNotifyContent, CountArray(ClubDelete.szNotifyContent), CWHService::FormatMessageV(MIM_CLUBPLAZA_0010, TEXT("��Ǹ�����ѱ��Ƴ�[%s]�Ĳ��硣"), pDeleteMember->szClubName));			
		}
		else
		{
			StringCchCopy(ClubDelete.szNotifyContent, CountArray(ClubDelete.szNotifyContent), CWHService::FormatMessageV(MIM_CLUBPLAZA_0011, TEXT("�����˳�[%s]�Ĳ��硣"), pDeleteMember->szClubName));			
		}

		//��������
		WORD wPacketSize = CountStringBuffer(ClubDelete.szNotifyContent);
		wPacketSize += sizeof(ClubDelete) - sizeof(ClubDelete.szNotifyContent);
		SendDataToGate(pSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_DELETE, &ClubDelete, wPacketSize, &pSocketInfo->dwTokenID, 1);						
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBApplyMessage(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT (wDataSize>=sizeof(DBO_GR_ApplyMessage));
	if (wDataSize<sizeof(DBO_GR_ApplyMessage)) return false;

	//��ȡ����
	auto pDBApplyMessage=(DBO_GR_ApplyMessage *)pData;

	//��������Ϣ
	if (pDBApplyMessage->cbMessageType== APPLY_MSG_TYPE_REQUEST)
	{
		//�����û�
		auto pServerUserItem = m_ClubUserManager.SearchUserItem(pDBApplyMessage->dwUserID);
		if (pServerUserItem != NULL && pServerUserItem->GetUserStatus() == US_CB_ONLINE)
		{
			//ת������		
			auto pSocketInfo = pServerUserItem->GetSocketInfo();

			//��ȡ����
			auto pApplyMessage = (CMD_CP_S_ApplyMessage *)m_cbBuffer;

			//���ñ���
			pApplyMessage->cbMessageType = pDBApplyMessage->cbMessageType;
			pApplyMessage->wMessageCount = pDBApplyMessage->wMessageCount;

			//���ñ���	
			CopyMemory(pApplyMessage + 1, pDBApplyMessage + 1, wDataSize - sizeof(DBO_GR_ApplyMessage));

			//��������
			WORD wPacketSize = sizeof(CMD_CP_S_ApplyMessage) + wDataSize - sizeof(DBO_GR_ApplyMessage);
			SendDataToGate(pSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_APPLY_MESSAGE, pApplyMessage, wPacketSize, &pSocketInfo->dwTokenID, 1);
		}
	}

	//��Ӧ����Ϣ
	if (pDBApplyMessage->cbMessageType == APPLY_MSG_TYPE_RESPOND)
	{
		//����ṹ
		auto pForwardHead = GetForwardHead();

		//���ñ���	
		pForwardHead->wItemCount = 1;
		pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
		pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
		pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CLUB;
		pForwardHead->ForwardItem[0].dwHashKeyID = pDBApplyMessage->dwClubID;

		//��ȡ����
		auto pApplyMessage = (CMD_CS_CB_ApplyMessage *)(pForwardHead + 1);
		ZeroMemory(pApplyMessage, sizeof(CMD_CS_CB_ApplyMessage));

		//���ñ���		
		pApplyMessage->dwClubID = pDBApplyMessage->dwClubID;		
		pApplyMessage->wMessageCount = pDBApplyMessage->wMessageCount;

		//���ñ���	
		CopyMemory(pApplyMessage + 1, pDBApplyMessage + 1, wDataSize - sizeof(DBO_GR_ApplyMessage));

		//��������
		WORD wPacketSize = sizeof(tagForwardHead)+sizeof(CMD_CS_CB_ApplyMessage)+wDataSize-sizeof(DBO_GR_ApplyMessage);
		m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_CB_APPLY_MESSAGE, pForwardHead, wPacketSize, 0);
	}	

	return true;
}

//�޸ķ���
bool CAttemperEngineSink::OnDBModifyScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT(wDataSize == sizeof(DBO_GR_ModifyScore));
	if (wDataSize != sizeof(DBO_GR_ModifyScore)) return false;

	//��ȡ����
	auto pDBOModifyScore = (DBO_GR_ModifyScore *)pData;

	//����ṹ
	auto pForwardHead = GetForwardHead();

	//���ñ���	
	pForwardHead->wItemCount = 1;
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CLUB;
	pForwardHead->ForwardItem[0].dwHashKeyID = pDBOModifyScore->dwClubID;

	//��ȡ����
	CMD_CS_CB_ModifyScore * pModifyScore = (CMD_CS_CB_ModifyScore *)(pForwardHead + 1);
	ZeroMemory(pModifyScore, sizeof(CMD_CS_CB_ModifyScore));

	//���ñ���
	pModifyScore->wKindID = pDBOModifyScore->wKindID;
	pModifyScore->wServerID= pDBOModifyScore->wServerID;
	pModifyScore->dwClubID = pDBOModifyScore->dwClubID;
	pModifyScore->dwMemberID = pDBOModifyScore->dwMemberID;
	pModifyScore->dwPayUserID = pDBOModifyScore->dwPayUserID;
	pModifyScore->lMemberScore = pDBOModifyScore->lMemberScore;
	pModifyScore->lModifyScore = pDBOModifyScore->lModifyScore;		

	//�㲥��Ϣ
	WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_CB_ModifyScore);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_CB_MODIFY_SCORE, pForwardHead, wPacketSize, 0);
	
	//���²Ƹ�
	auto pServerUserItem = m_ClubUserManager.SearchUserItem(pDBOModifyScore->dwPayUserID);
	if (pServerUserItem != NULL)
	{
		//ת������		
		auto pSocketInfo = pServerUserItem->GetSocketInfo();

		//��ȡ����
		CMD_CP_S_OperateSuccess OperateSuccess;
		ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

		//���ñ���		
		OperateSuccess.lOperateCode = 0;
		_snwprintf_s(OperateSuccess.szOperateNotice, CountArray(OperateSuccess.szOperateNotice), TEXT("%s: %s%I64d"), pDBOModifyScore->szNickName, pDBOModifyScore->lModifyScore >= 0 ? L"+" : L"", pDBOModifyScore->lModifyScore / SCORE_MULTIPLE);

		//��������
		WORD wPacketSize = CountStringBuffer(OperateSuccess.szOperateNotice);
		wPacketSize += sizeof(OperateSuccess) - sizeof(OperateSuccess.szOperateNotice);
		SendDataToGate(pSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_OPERATE_SUCCESS, &OperateSuccess, wPacketSize, &pSocketInfo->dwTokenID, 1);
	}	

	return true;
}

//Ⱥ������
bool CAttemperEngineSink::OnDBClubOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT (wDataSize % sizeof(tagClubOption) == 0);
	if (wDataSize % sizeof(tagClubOption) != 0) return false;

	//��������
	auto pClubOption = (tagClubOption*)pData;
	WORD wOptionCount = wDataSize / sizeof(tagClubOption);	

	//�Ƴ�����
	m_ClubOption= *pClubOption;	

	return true;
}

//�������
bool CAttemperEngineSink::OnDBOptionFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//�¼�֪ͨ
	CP_ControlResult ControlResult;
	ControlResult.cbSuccess= ER_SUCCESS/*m_ClubOptionArray.GetCount()>0?ER_SUCCESS:ER_FAILURE*/;
	SendUIControlPacket(UI_LOAD_CLUB_OPTION,&ControlResult,sizeof(ControlResult));

	return true;
}

//��ֹ�ʻ�
bool CAttemperEngineSink::OnDBForbidWords(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��ʼ����
	if(dwContextID==0xFFFE)
	{
		//���ö���
		m_WordFilter.ResetFilter();

		//�رն�ʱ��
		m_pITimerEngine->KillTimer(IDI_LOAD_FORBID_WORDS);

		return true;			
	}

	//��������
	WORD wBufferPos=0;
	BYTE cbWordsCount=0;
	BYTE * pBuffer=(LPBYTE)pData;
	TCHAR szKeyWords[32]=TEXT("");

	//��ȡ����
	while (wBufferPos<wDataSize)
	{
		cbWordsCount=*(BYTE *)&pBuffer[wBufferPos++];
		CopyMemory(szKeyWords,&pBuffer[wBufferPos],cbWordsCount);
		wBufferPos+=cbWordsCount;

		//��ӹؼ���
		m_WordFilter.InsertKeyword(szKeyWords);
	}   

	return true;
}

//�Ƹ�����
bool CAttemperEngineSink::OnDBWealthUpdate(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT (wDataSize<=sizeof(DBO_GR_WealthUpdate));
	if (wDataSize>sizeof(DBO_GR_WealthUpdate)) return false;

	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);
	if (pBindParameter->dwSocketID != dwContextID) return true;

	//��ȡ����
	auto pUserWealthUpdate=(DBO_GR_WealthUpdate *)pData;

	//�������
	auto pWealthUpdate = (CMD_CM_WealthUpdate*)m_cbBuffer;
	ZeroMemory(pWealthUpdate, sizeof(CMD_CM_WealthUpdate));

	//���ñ���
	pWealthUpdate->wItemCount = pUserWealthUpdate->wItemCount;
	CopyMemory(pWealthUpdate->WealthItem, pUserWealthUpdate->WealthItem, pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]));

	//��������
	WORD wPacketSize = sizeof(CMD_CM_WealthUpdate) + pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]);
	SendDataToGate(pBindParameter->wServiceID, MDM_CM_SYSTEM, SUB_CM_WEALTH_UPDATE, pWealthUpdate, wPacketSize, &dwTokenID, 1);

	return true;
}

//ɾ�����
bool CAttemperEngineSink::OnDBApplyDelete(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);
	if (pBindParameter->dwSocketID != dwContextID) return true;

	//У�����
	ASSERT (wDataSize>=sizeof(DBO_GR_ApplyDelete));
	if (wDataSize<sizeof(DBO_GR_ApplyDelete)) return false;

	//��ȡ����
	auto pApplyDelete =(DBO_GR_ApplyDelete *)pData;

	//������
	auto pCPApplyDelete = (CMD_CP_S_ApplyDelete*)m_cbBuffer;
	WORD wMessageSize = wDataSize - sizeof(DBO_GR_ApplyDelete);

	//��Ϣ����
	pCPApplyDelete->wMessageCount = wMessageSize / sizeof(DWORD);

	//������ʶ
	CopyMemory(pCPApplyDelete+1, pApplyDelete+1, wMessageSize);

	//��������	
	SendDataToGate(pBindParameter->wServiceID,MDM_CLUBPLAZA_SERVICE,SUB_CP_S_APPLY_DELETE, pCPApplyDelete, sizeof(CMD_CP_S_ApplyDelete)+wMessageSize,&dwTokenID,1);

	//��Ӧ����
	if (pApplyDelete->cbMessageType == APPLY_MSG_TYPE_RESPOND)
	{
		//����ṹ
		auto pForwardHead = GetForwardHead();

		//���ñ���	
		pForwardHead->wItemCount = 1;
		pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
		pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
		pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CLUB;
		pForwardHead->ForwardItem[0].dwHashKeyID = pApplyDelete->dwClubID;

		//��ȡ����
		auto pFDApplyDelete = (CMD_CS_CB_ApplyDelete*)(pForwardHead + 1);

		//���ñ���
		pFDApplyDelete->dwUserID = pApplyDelete->dwUserID;
		pFDApplyDelete->dwClubID = pApplyDelete->dwClubID;
		pFDApplyDelete->wMessageCount = wMessageSize /sizeof(DWORD);

		//������ʶ
		CopyMemory(pFDApplyDelete+1, pApplyDelete+1, wMessageSize);

		//��������
		WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_CB_ApplyDelete) + wMessageSize;
		m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_CB_APPLY_DELETE, pForwardHead, wPacketSize, 0);
	}

	return true;
}

//�������
bool CAttemperEngineSink::OnDBDetectClubName(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);
	if (pBindParameter->dwSocketID != dwContextID) return true;

	//��ȡ����
	auto pDetectClubName = (DBO_GR_DetectClubName*)pData;

	//��ȡ����
	CMD_CP_S_DetectResult DetectResult = {};
	
	//���ñ���		
	DetectResult.bUsable = pDetectClubName->bUsable;
	StringCchCopy(DetectResult.szErrorDescribe,CountArray(DetectResult.szErrorDescribe), pDetectClubName->szErrorDescribe);

	//��������
	SendDataToGate(pBindParameter->wServiceID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_DETECT_RESULT, &DetectResult, sizeof(DetectResult), &dwTokenID, 1);

	return true;
}

//�����ɹ�
bool CAttemperEngineSink::OnDBOperateSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);
	if (pBindParameter->dwSocketID != dwContextID) return true;

	//У�����
	ASSERT (wDataSize<=sizeof(DBO_GR_OperateSuccess));
	if (wDataSize>sizeof(DBO_GR_OperateSuccess)) return false;	

	//��ȡ����
	auto pOperateSuccess=(DBO_GR_OperateSuccess *)pData;

	//��ȡ����
	//CMD_CP_S_OperateSuccess OperateSuccess;
	//ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

	////���ñ���		
	//OperateSuccess.lOperateCode=pOperateSuccess->lOperateCode;		
	//StringCchCopy(OperateSuccess.szOperateNotice,CountArray(OperateSuccess.szOperateNotice), pOperateSuccess->szOperateNotice);

	////��������
	//WORD wPacketSize=CountStringBuffer(OperateSuccess.szOperateNotice);
	//wPacketSize+=sizeof(OperateSuccess)-sizeof(OperateSuccess.szOperateNotice);
	//SendDataToGate(pBindParameter->wServiceID,MDM_CLUBPLAZA_SERVICE,SUB_CP_S_OPERATE_SUCCESS,&OperateSuccess,wPacketSize,&dwTokenID,1);

	//��������
	auto pRequestSuccess = (CMD_CM_OperateSuccess*)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer + sizeof(CMD_CM_OperateSuccess), sizeof(m_cbBuffer) - sizeof(CMD_CM_OperateSuccess));

	//���ñ���
	pRequestSuccess->wRequestCmdID = 0;
	pRequestSuccess->lErrorCode = pOperateSuccess->lOperateCode;	

	//������Ϣ
	if (pOperateSuccess->szOperateNotice[0] != 0)  SendPacket.AddPacket(pOperateSuccess->szOperateNotice, DTP_CM_DESCRIBE);

	//�������
	WORD wPacketSize = sizeof(CMD_CM_OperateSuccess) + SendPacket.GetDataSize();
	SendDataToGate(pBindParameter->wServiceID, MDM_CM_SYSTEM, SUB_CM_OPERATE_SUCCESS, pRequestSuccess, wPacketSize, &dwTokenID, 1);
	
	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��������
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);
	if (pBindParameter->dwSocketID != dwContextID) return true;

	//У�����
	ASSERT (wDataSize<=sizeof(DBO_GR_OperateFailure));
	if (wDataSize>sizeof(DBO_GR_OperateFailure)) return false;	

	//��ȡ����
	auto pOperateFailure=(DBO_GR_OperateFailure *)pData;

	//��ȡ����
	//CMD_CP_S_OperateFailure OperateFailure;
	//ZeroMemory(&OperateFailure,sizeof(OperateFailure));

	////���ñ���		
	//OperateFailure.lErrorCode=pOperateFailure->lErrorCode;		
	//StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), pOperateFailure->szErrorDescribe);

	////��������
	//WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
	//wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
	//SendDataToGate(pBindParameter->wServiceID,MDM_CLUBPLAZA_SERVICE,SUB_CP_S_OPERATE_FAILURE,&OperateFailure,wPacketSize,&dwTokenID,1);

	//��������
	auto pRequestFailure = (CMD_CM_OperateFailure*)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer + sizeof(CMD_CM_OperateFailure), sizeof(m_cbBuffer) - sizeof(CMD_CM_OperateFailure));

	//���ñ���
	pRequestFailure->wRequestCmdID = 0;
	pRequestFailure->lErrorCode = pOperateFailure->lErrorCode;	

	//������Ϣ
	if (pOperateFailure->szErrorDescribe[0] != 0)  SendPacket.AddPacket(pOperateFailure->szErrorDescribe, DTP_CM_DESCRIBE);

	//�������
	WORD wPacketSize = sizeof(CMD_CM_OperateFailure) + SendPacket.GetDataSize();
	SendDataToGate(pBindParameter->wServiceID, MDM_CM_SYSTEM, SUB_CM_OPERATE_FAILURE, pRequestFailure, wPacketSize, &dwTokenID, 1);
		
	return true;
}

//////////////////////////////////////////////////////////////////////////

