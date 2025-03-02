#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_SERVICE_PULSE			1									//��������
#define IDI_LOAD_GAME_LIST			2									//�����б�
#define IDI_CONNECT_CENTER			3									//������ʶ
#define IDI_COLLECT_ONLINE_INFO		4									//ͳ������
#define IDI_COLLECT_JACKPOT_INFO	5									//ͳ�ƽ���

//��������
#define INVALIDATE_CODE_TIME		60									//��Чʱ��	

//////////////////////////////////////////////////////////////////////////
//���붨��
#define SUPPORT_MASK_PC				0x0001								//����֧��
#define SUPPORT_MASK_MOBILE			0x0002								//�ֻ�֧��

//////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_dwPCFuncption=0;
	m_dwMBFuncption=0;
	m_pServiceOption=NULL;
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;	
	m_dwShowOnLineInfo=SHOW_SERVER_USER_COUNT;

	//��������
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));

	//�������
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;

	//�汾����
	m_bUpdateSilent=false;

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
	m_pBindParameter=new tagBindParameter[m_pServiceOption->wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pServiceOption->wMaxConnect);	

	//����ʱ��
	ASSERT(m_pITimerEngine != NULL);
	m_pITimerEngine->SetTimer(IDI_SERVICE_PULSE, 1 * 1000L, TIMES_INFINITY, 0);

	//����ʱ��
	ASSERT(m_pITimerEngine!=NULL);
	m_pITimerEngine->SetTimer(IDI_COLLECT_ONLINE_INFO,m_pInitParameter->m_wCollectTime*1000L,TIMES_INFINITY,0);		
	

	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//��������
	m_WordsFilter.ResetFilter();
	m_GlobalItemManager.ResetData();
	m_ServerListManager.ResetServerList();

	//�������
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;

	//��������
	m_pServiceOption=NULL;
	SafeDeleteArray(m_pBindParameter);

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	switch (dwIdentifier)
	{
	case CT_LOAD_CONFIG_DATA:	//��������
	{
		//�����б�
		m_ServerListManager.DisuseKernelItem();

		//��������
		DBR_GP_LoadConfig LoadConfig;
		ZeroMemory(&LoadConfig, sizeof(LoadConfig));

		//���ñ���
		LoadConfig.bInitService = true;
		LoadConfig.dwConfigMask = 0xFFFFFFFF;		

		//��������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_CONFIG, 0, 0, 0, &LoadConfig, sizeof(LoadConfig));

		//�������а�
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_RANKING_LIST, 0, 0, 0, NULL, 0);		

		return true;
	}
	case CT_CONNECT_CENTER:		//��������
	{
		//��������
		DWORD dwCenterServer=0L;

		//���õ�ַ
		dwCenterServer=m_pInitParameter->m_CenterAddress.dwServiceAddr;

		//��������
		m_pITCPSocketCenter->Connect(htonl(dwCenterServer), m_pInitParameter->m_wCenterPort);

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڳ����������ķ�����..."),TraceLevel_Normal);

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
	case IDI_LOAD_GAME_LIST:		//�����б�
	{
		//�����б�
		m_ServerListManager.DisuseKernelItem();

		//��������
		DBR_GP_LoadConfig LoadConfig;
		ZeroMemory(&LoadConfig, sizeof(LoadConfig));

		//���ñ���
		LoadConfig.dwConfigMask = CONFIG_MASK_GAME_LIST;

		//��������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_CONFIG, 0, 0, 0, &LoadConfig, sizeof(LoadConfig));				

		return true;
	}
	case IDI_CONNECT_CENTER:		//��������
	{
		//��������						
		tagAddressInfo * pAddressInfo=&m_pInitParameter->m_CenterAddress;

		//���õ�ַ
		DWORD dwCenterServer=pAddressInfo->dwServiceAddr;

		//��������
		m_pITCPSocketCenter->Connect(htonl(dwCenterServer), m_pInitParameter->m_wCenterPort);

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڳ����������ķ�����..."),TraceLevel_Normal);

		return true;
	}
	case IDI_COLLECT_ONLINE_INFO:	//ͳ������
	{
		//��������
		DBR_GP_OnLineCountInfo OnLineCountInfo;
		ZeroMemory(&OnLineCountInfo,sizeof(OnLineCountInfo));

		//��ȡ����
		OnLineCountInfo.dwOnLineCountSum=m_ServerListManager.CollectOnlineInfo();

		//��ȡ����
		POSITION KindPosition=NULL;
		do
		{
			//��ȡ����
			CGameKindItem * pGameKindItem=m_ServerListManager.EmunGameKindItem(KindPosition);

			//���ñ���
			if (pGameKindItem!=NULL)
			{
				//����ж�
				if (OnLineCountInfo.wKindCount>=CountArray(OnLineCountInfo.OnLineCountKind))
				{
					ASSERT(FALSE);
					break;
				}

				WORD wKindIndex=OnLineCountInfo.wKindCount++;
				OnLineCountInfo.OnLineCountKind[wKindIndex].wKindID=pGameKindItem->GetKindID();
				OnLineCountInfo.OnLineCountKind[wKindIndex].dwOnLineCount=pGameKindItem->m_dwOnLineCount;
			}

		} while (KindPosition!=NULL);
		
		//��������
		WORD wHeadSize=sizeof(OnLineCountInfo)-sizeof(OnLineCountInfo.OnLineCountKind);
		WORD wSendSize=wHeadSize+OnLineCountInfo.wKindCount*sizeof(OnLineCountInfo.OnLineCountKind[0]);
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_ONLINE_COUNT_INFO,0,0,0,&OnLineCountInfo,wSendSize);

		return true;
	}
	case IDI_SERVICE_PULSE:			//��������
	{
		//��̬����
		static bool bTimeInit = false;
		static SYSTEMTIME LastSystemTime;

		//��ʼ��ʱ��
		if (bTimeInit == false)
		{
			bTimeInit = true;
			GetLocalTime(&LastSystemTime);
		}

		//��������
		SYSTEMTIME CurrSysteTime;
		GetLocalTime(&CurrSysteTime);

		//�����л�
		if (CurrSysteTime.wDay != LastSystemTime.wDay)
		{
			OnEventDaySwitch();
		}

		//����ʱ��
		LastSystemTime = CurrSysteTime;

		return true;
	}
	}

	return false;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//��ȡ����
	ASSERT(LOWORD(dwSocketID)< m_pServiceOption->wMaxConnect);
	if (LOWORD(dwSocketID)>= m_pServiceOption->wMaxConnect) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	//���ñ���
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ������	
	bool bResult = false;
	WORD wMainCmdID = LOCMMD(Command.wMainCmdID);

	//ע�����
	if (wMainCmdID==MDM_CS_REGISTER) 
	{
		return OnTCPNetworkMainRegister(Command.wSubCmdID, pData, wDataSize, dwSocketID);
	}

	//ͨ�÷���
	if (wMainCmdID==MDM_CS_COMMON_SERVICE)	
	{
		return OnTCPNetworkMainCommonService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
	}	

	switch (wMainCmdID)
	{	
	case MDM_GP_LOGON:			 //��¼����
	{
		bResult=OnTCPNetworkMainPCLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
		break;
	}
	case MDM_GP_SERVER_LIST:	 //�б�����
	{
		bResult=OnTCPNetworkMainPCServerList(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
		break;
	}
	case MDM_GP_USER_SERVICE:	 //��������
	{
		bResult=OnTCPNetworkMainPCUserService(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
		break;
	}
	case MDM_GP_CONFIG_SERVICE:	 //���÷���
	{
		bResult=OnTCPNetworkMainPCConfigService(Command.wSubCmdID, pData, wDataSize, dwSocketID, dwTokenID);
		break;
	}
	case MDM_GP_ROBOT_SERVICE:	 //��������
	{
		bResult=OnTCPNetworkMainPCRobotService(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
		break;
	}
	case MDM_GP_STOCK_SERVICE:	 //������
	{
		bResult = OnTCPNetworkMainPCStockService(Command.wSubCmdID, pData, wDataSize, dwSocketID, dwTokenID);
		break;
	}	
	case MDM_GP_MANAGER_SERVICE: //�������	
	{
		bResult=OnTCPNetworkMainPCManagerService(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
		break;
	}
	case MDM_MB_LOGON:			 //��¼����
	{
		bResult=OnTCPNetworkMainMBLogon(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
		break;
	}
	case MDM_MB_SERVER_LIST:	 //�б�����
	{
		bResult=OnTCPNetworkMainMBServerList(Command.wSubCmdID,pData,wDataSize,dwSocketID, dwTokenID);
		break;
	}	
	case MDM_MB_USER_SERVICE:	 //�û�����
	{
		bResult=OnTCPNetworkMainMBUserService(Command.wSubCmdID, pData, wDataSize, dwSocketID, dwTokenID);
		break;
	}
	}

	//�ر�����
	if (bResult == false)
	{
		//����ṹ
		CMD_CS_C_ShutdownSocket ShutdownSocket;
		ShutdownSocket.bForceClose = false;

		//��������
		SendDataToGate(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket), &dwTokenID, 1);
	}

	return true;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��ȡ����
	auto pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return false;

	//��תģʽ
	if (pBindParameter->wServiceModule == SERVICE_MODULE_GATE)
	{
		//�Ƴ�����
		m_GlobalItemManager.DeleteGateItem(pBindParameter->wServiceID);
	}

	//�����Ϣ
	ZeroMemory(pBindParameter, sizeof(tagBindParameter));

	return true;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GP_GAME_TYPE_ITEM:			//��������
	{
		return OnDBGameTypeItem(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_GAME_KIND_ITEM:			//��������
	{
		return OnDBGameKindItem(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_GAME_NODE_ITEM:			//�ڵ�����
	{
		return OnDBGameNodeItem(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_GAME_PAGE_ITEM:			//��������
	{
		return OnDBGamePageItem(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_GAME_FILTER_ITEM:		//��������
	{
		return OnDBGameFilterItem(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_CONFIG_RESULT:			//���ý��
	{
		return OnDBConfigResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_FILTER_WORDS:			//���д���Ϣ
	{
		return OnDBFilterWordsInfo(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_RANKING_LIST_DATA:		//������
	{
		return OnDBRankingListData(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_VERSION_INFO:			//�汾��Ϣ
	{
		return OnDBModuleVersionInfo(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_TRANSFER_PARAMETER:		//ת�˲���
	{
		return OnDBTransferParameter(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_LOGON_SUCCESS:			//��¼�ɹ�
	{
		return OnDBPCLogonSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_LOGON_FAILURE:			//��¼ʧ��
	{
		return OnDBPCLogonFailure(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_USER_INDIVIDUAL:		//�û���Ϣ
	{
		return OnDBPCUserIndividual(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_OPERATE_SUCCESS:		//�����ɹ�
	{
		return OnDBPCOperateSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_OPERATE_FAILURE:		//����ʧ��
	{
		return OnDBPCOperateFailure(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_GAME_BASIC_CONFIG:		//��������
	{
		return OnDBPCGameBasicConfig(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_GAME_WEIGHT_CONFIG:		//Ȩ������
	{
		return OnDBPCGameWeightConfig(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_GAME_CONFIG_FINISH:		//�������
	{
		return OnDBPCGameConfigFinish(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_ROBOT_OPERATE_RESULT:	//�������
	{
		return OnDBPCRobotOperateResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_STOCK_OPERATE_RESULT:	//�������
	{
		return OnDBPCStockOperateResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_FACE_INFO:			//�û�ͷ��
	{
		return OnDBMBUserFaceInfo(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_CACHE_INFO:		//�û�����
	{
		return OnDBMBUserCacheInfo(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_ACTION_MASK:		//��Ϊ����
	{
		return OnDBMBUserActionMask(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_RANKING_INFO:		//��λ��Ϣ
	{
		return OnDBMBUserRankingInfo(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_EXCHANGE_MALLGOODS:		//�һ���Ʒ 
	{
		return OnDBMBExchangeMallGoods(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_EXCHANGE_GIFTBAGCODE:	//�һ������
	{
		return OnDBMBExchangeGiftbagCode(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_BINDMP_SUCCESS:			//�󶨳ɹ�
	{
		return OnDBMBBindMPSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_UNBINDMP_SUCCESS:		//���ɹ�
	{
		return OnDBMBUnbindMPSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_BIND_EMAIL_SUCCESS:		//�󶨳ɹ�
	{
		return OnDBMBBindEMailSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_UNBIND_EMAIL_SUCCESS:	//���ɹ�
	{
		return OnDBMBUnbindEMailSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_ACQUIREAC_RESULT:		//��ȡ���
	{
		return OnDBMBAcquireACResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_PACK_GOODS:		//������Ʒ
	{
		return OnDBMBUserPackGoods(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_GOODS_UPDATE:		//��Ʒ����
	{
		return OnDBMBUserGoodsUpdate(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_WEALTH_UPDATE:		//�Ƹ�����
	{
		return OnDBMBUserWealthUpdate(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_LOVELINESS_UPDATE: //��������
	{
		return OnDBMBUserLoveLinessUpdate(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_LOGON_SUCCESS:			//��¼�ɹ�
	{
		return OnDBMBLogonSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_LOGON_FAILURE:			//��¼ʧ��
	{
		return OnDBMBLogonFailure(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_LOGOUT_RESULT:			//�ǳ����
	{
		return OnDBMBLogoutResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_POST_MESSAGE:			//Ͷ����Ϣ
	{
		return OnDBMBPostMessage(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_CHECKIN_RESULT:			//ǩ�����
	{
		return OnDBMBCheckInResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_BIND_INVITE_CODE:		//��������
	{
		return OnDBMBBindInviteCode(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_BIND_EXTERNAL_ACCOUNT:	//���˺�
	{
		return OnDBMBBindExternalAccount(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_SEND_GIFT_SUCCESS:		//��������ɹ�
	{
		return OnDBMBSendGiftSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_GIFT_INFO:			//������Ϣ
	{
		return OnDBMBUserGiftInfo(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_USER_GIFT_INFO_FINISH:	//������Ϣ���
	{
		return OnDBMBUserGiftInfoFinish(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_WEALTH_TRANSFER_SUCCESS: //ת�˳ɹ�
	{
		return OnDBMBWealthTransferSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_TURNTABLE_LOTTERY_DATA:	//ת������
	{
		return OnDBMBTurntableLotteryData(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_TURNTABLE_LOTTERY_RESULT: //�齱���
	{
		return OnDBMBTurntableLotteryResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_MB_INSURE_QUERY_RESULT:	//���в�ѯ���
	{
		return OnDBMBInsureQueryResult(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_MB_INSURE_SUCCESS:		//�����ɹ�
	{
		return OnDBMBInsureSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_MB_INSURE_MAIL_NOTIFY:	//�����ʼ�֪ͨ
	{
		return OnDBMBInsureMailNotify(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_MB_INSURE_FAILURE:		//����ʧ��
	{
		return OnDBMBInsureFailure(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_MB_RESULT_SUBSIDY:	//�ͱ���ѯ
	{
		return OnDBMBSubsidyQuery(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_OPERATE_SUCCESS:		//�����ɹ�
	{
		return OnDBMBOperateSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_MB_OPERATE_FAILURE:		//����ʧ��
	{
		return OnDBMBOperateFailure(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_MB_NICKNAME_OPTION:	//�ǳ�����
	{
		return OnDBMBNickNameOption(dwContextID, dwTokenID, pData, wDataSize);
	}
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
			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("�����ķ����������ӹر��ˣ�%ld �����������"),TraceLevel_Warning, m_pInitParameter->m_wConnectTime);

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
	if (dwServiceID ==NETWORK_CENTER)
	{
		//�����ж�
		if (nErrorCode!=0)
		{
			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ķ���������ʧ�� [ %ld ]��%ld �����������"),TraceLevel_Warning, nErrorCode, m_pInitParameter->m_wConnectTime);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڻ�ȡ�㳡��Ϸ�б�..."),TraceLevel_Normal);

		//��������
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService,sizeof(RegisterService));

		//���ñ���
		RegisterService.wServiceID = m_pServiceOption->wServiceID;
		RegisterService.wServiceModule = SERVICE_MODULE_LOGON;
		RegisterService.wServicePort= m_pServiceOption->wServicePort;
		RegisterService.wServiceKindID = m_pServiceOption->wServiceKindID;
		RegisterService.wServiceMask = SERVICE_MASK_GAME | SERVICE_MASK_LOGON;		
		StringCchCopy(RegisterService.szServiceName,CountArray(RegisterService.szServiceName), L"��¼������");
		StringCchCopy(RegisterService.szServiceDomain,CountArray(RegisterService.szServiceDomain), m_pServiceOption->szServiceDomain);

		//��������
		m_pITCPSocketCenter->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_SERVICE,&RegisterService,sizeof(RegisterService),0);

		return true;
	}

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	//��������
	if (dwServiceID==NETWORK_CENTER)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:		//ע�����
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
		}
		case MDM_CS_COMMON_SERVICE:	//������Ϣ
		{
			return OnTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
		}	
		case MDM_CS_USER_COLLECT:	//�û��ռ�
		{
			return OnTCPSocketMainUserCollect(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_CS_FORWARD_SERVICE://ת������
		{
			return OnTCPSocketMainForwardService(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_CS_MANAGER_SERVICE://�������
		{
			return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
		}
		}
	}

	//�������
	ASSERT(FALSE);

	return true;
}

//ע�����
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
		{
			//��������
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;

			//Ч�����
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//�رմ���
			m_pITCPSocketCenter->CloseSocket();

			//��ʾ��Ϣ
			if (lstrlen(pRegisterFailure->szDescribeString) > 0)
			{
				CTraceService::TraceString(pRegisterFailure->szDescribeString, TraceLevel_Exception);
			}

			return true;
		}
	}

	return true;
}

//ע�����
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_REGISTER_SERVICE:		//ע�����
		{
			//Ч������
			ASSERT(wDataSize == sizeof (CMD_CS_C_RegisterService));
			if (wDataSize != sizeof (CMD_CS_C_RegisterService)) return false;

			//��Ϣ����
			CMD_CS_C_RegisterService * pRegisterService = (CMD_CS_C_RegisterService *)pData;

			//У�����
			if (pRegisterService->wServiceModule != SERVICE_MODULE_GATE) return false;

			//����·��
			if (m_GlobalItemManager.SearchGateItem(pRegisterService->wServiceID) != NULL)
			{
				//��������
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure, sizeof(RegisterFailure));

				//���ñ���
				RegisterFailure.lErrorCode = 0;
				_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString),L"�Ѿ�������ͬ��Ϸ��ʶ��%s������ע��ʧ��", pRegisterService->szServiceName);

				//������Ϣ
				WORD wStringSize = CountStringBuffer(RegisterFailure.szDescribeString);
				WORD wSendSize = sizeof(RegisterFailure) - sizeof(RegisterFailure.szDescribeString) + wStringSize;
				SendDataToGate(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_FAILURE, &RegisterFailure, wSendSize, NULL, 0);

				//�ж�����
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//���ð�
			tagBindParameter * pBindParameter = GetBindParameter(dwSocketID);
			if (pBindParameter == NULL) return false;

			//���ð�
			pBindParameter->wServiceID = pRegisterService->wServiceID;
			pBindParameter->wServiceModule = pRegisterService->wServiceModule;			
			
			//��������
			tagServiceItem ServiceItem;
			ZeroMemory(&ServiceItem, sizeof(ServiceItem));

			//����ṹ
			ServiceItem.wServiceID = pRegisterService->wServiceID;
			ServiceItem.wMaxConnect = pRegisterService->wMaxConnect;
			ServiceItem.wServicePort = pRegisterService->wServicePort;
			ServiceItem.wServiceModule = pRegisterService->wServiceModule;
			ServiceItem.dwServiceAddr = pRegisterService->dwServiceAddr;			
			StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), pRegisterService->szServiceName);
			StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain), pRegisterService->szServiceDomain);

			//ע������
			pBindParameter->pGlobalGateItem = m_GlobalItemManager.ActiveGateItem(dwSocketID, ServiceItem);

			//��������
			SendDataToGate(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_SUCCESS, NULL, 0);			

			return true;
		}
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_LOGOUT_TOKEN:	//ע������
		{
			//�󶨲���
			tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
			if (pBindParameter == NULL) return false;

			//��ȡ����
			CMD_CS_C_LogoutToken * pLogoutToken = (CMD_CS_C_LogoutToken *)pData;

			//��ȡ����
			CGlobalGateItem * pGlobalGateItem = (CGlobalGateItem *)pBindParameter->pGlobalGateItem;
			if (pGlobalGateItem == NULL) return true;

			//�˺ŵǳ�
			auto pTokenParameter = pGlobalGateItem->SearchTokenID(pLogoutToken->dwTokenID);
			if (pTokenParameter != NULL && pTokenParameter->pBindUserItem!=NULL)
			{
				//�û���Ϣ
				auto pGlobalUserItem = (CGlobalUserItem *)pTokenParameter->pBindUserItem;

				//����ṹ
				DBR_MB_LogoutAccounts LogoutAccounts;
				ZeroMemory(&LogoutAccounts,sizeof(LogoutAccounts));

				//���ñ���
				LogoutAccounts.wGateID = pGlobalGateItem->GetServiceID();
				LogoutAccounts.wLogonID = m_pServiceOption->wServiceID;
				LogoutAccounts.dwSocketID = pLogoutToken->dwTokenID;
				LogoutAccounts.dwUserID = pGlobalUserItem->GetUserID();

				//Ͷ������
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGOUT_ACCOUNTS, LogoutAccounts.dwUserID, dwSocketID, pLogoutToken->dwTokenID, &LogoutAccounts, sizeof(LogoutAccounts));				

				//�Ƴ��û�
				m_GlobalItemManager.DeleteUserItem(pGlobalUserItem->GetUserID());
			}			

			//ע������
			pGlobalGateItem->RemoveTokenID(pLogoutToken->dwTokenID);
			
			//�ǳ�����
			return true;
		}
	case SUB_CS_C_REGISTER_TOKEN: //ע������
		{
			//�󶨲���
			tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
			if (pBindParameter == NULL) return false;

			//��ȡ����
			CGlobalGateItem * pGlobalGateItem = (CGlobalGateItem *)pBindParameter->pGlobalGateItem;
			if (pGlobalGateItem == NULL) return true;

			//��ȡ����
			CMD_CS_C_RegisterToken * pRegisterToken = (CMD_CS_C_RegisterToken *)pData;

			//����ṹ
			CMD_CS_S_RegisterToken RegisterToken;
			ZeroMemory(&RegisterToken, sizeof(RegisterToken));

			//Ĭ��ʧ��
			RegisterToken.lResultCode = REGISTER_RESULT_FAILURE;

			//��ȡ����
			tagTokenParameter * pTokenParameter = pGlobalGateItem->ActiveTokenID(pRegisterToken->dwTokenID);
			if (pTokenParameter != NULL)
			{
				//ע����
				RegisterToken.lResultCode = REGISTER_RESULT_SUCCESS;

				//���õ�ַ		
				pTokenParameter->dwUserID = pRegisterToken->dwUserID;
				pTokenParameter->dwClientIP = pRegisterToken->dwClientIP;
			}

			//��������
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_REGISTER_TOKEN, &RegisterToken, sizeof(RegisterToken), &pRegisterToken->dwTokenID, 1);

			return true;
		}
	}
	return true;
}

//��¼����
bool CAttemperEngineSink::OnTCPNetworkMainPCLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_LOGON_GAMEID:		//�ο͵�¼
		{
			return OnTCPNetworkSubPCLogonByGameID(pData,wDataSize,dwSocketID, dwTokenID);
		}
	case SUB_GP_LOGON_ACCOUNTS:		//�ʺŵ�¼
		{
			return OnTCPNetworkSubPCLogonByAccounts(pData,wDataSize,dwSocketID, dwTokenID);
		}
	case SUB_GP_LOGON_MANAGETOOL:	//������
		{
			return OnTCPNetworkSubPCLogonManageTool(pData,wDataSize,dwSocketID, dwTokenID);
		}
	case SUB_GP_REGISTER_ACCOUNTS:	//�ʺ�ע��
		{
			return OnTCPNetworkSubPCRegisterAccounts(pData,wDataSize,dwSocketID, dwTokenID);
		}
	}

	return false;
}

//�б���
bool CAttemperEngineSink::OnTCPNetworkMainPCServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_GET_LIST:			//��ȡ�б�
		{
			ASSERT (wDataSize==sizeof(DWORD));
			if (wDataSize!=sizeof(DWORD)) return true;

			//��ȡվ��
      DWORD dwStationID=*(DWORD*)pData;

			//��������
			SendGameTypeInfo(dwSocketID, dwTokenID);

			//������Ϸ
			SendGameKindInfo(dwSocketID, dwTokenID, SUPPORT_MASK_PC);

			//����ҳ��
			SendGamePageInfo(dwSocketID, dwTokenID, 0);

			//�������
			SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_FINISH,&dwTokenID,1);

			return true;
		}
	case SUB_GP_GET_SERVER:			//��ȡ����
		{
			//Ч������
			ASSERT(wDataSize%sizeof(WORD)==0);
			if (wDataSize%sizeof(WORD)!=0) return false;

			//�����б�
			UINT nKindCount=wDataSize/sizeof(WORD);
			for (UINT i=1;i<nKindCount;i++)
			{
				SendGameNodeInfo(dwSocketID, dwTokenID, ((WORD *)pData)[i]);
				SendGamePageInfo(dwSocketID, dwTokenID, ((WORD *)pData)[i]);
				SendGameServerInfo(dwSocketID, dwTokenID,((WORD *)pData)[i],((WORD *)pData)[0]);
			}

			//�������
			SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_SERVER_FINISH,pData,wDataSize,&dwTokenID,1);

			return true;
		}
	case SUB_GP_GET_ONLINE:			//��ȡ����
		{
			//��������
			CMD_GP_GetOnline * pGetOnline=(CMD_GP_GetOnline *)pData;
			WORD wHeadSize=(sizeof(CMD_GP_GetOnline)-sizeof(pGetOnline->wOnLineServerID));

			//Ч������
			ASSERT((wDataSize>=wHeadSize)&&(wDataSize==(wHeadSize+pGetOnline->wServerCount*sizeof(WORD))));
			if ((wDataSize<wHeadSize)||(wDataSize!=(wHeadSize+pGetOnline->wServerCount*sizeof(WORD)))) return false;

			//��������
			CMD_GP_KindOnline KindOnline;
			CMD_GP_ServerOnline ServerOnline;
			ZeroMemory(&KindOnline,sizeof(KindOnline));
			ZeroMemory(&ServerOnline,sizeof(ServerOnline));

			//��ȡ����
			POSITION KindPosition=NULL;
			do
			{
				//��ȡ����
				CGameKindItem * pGameKindItem=m_ServerListManager.EmunGameKindItem(KindPosition);

				//���ñ���
				if (pGameKindItem!=NULL)
				{
					WORD wKindIndex=KindOnline.wKindCount++;
					KindOnline.OnLineInfoKind[wKindIndex].wKindID= pGameKindItem->GetKindID();
					KindOnline.OnLineInfoKind[wKindIndex].dwOnLineCount=pGameKindItem->m_dwOnLineCount;
				}

				//����ж�
				if (KindOnline.wKindCount>=CountArray(KindOnline.OnLineInfoKind))
				{
					ASSERT(FALSE);
					break;
				}

			} while (KindPosition!=NULL);

			//��ȡ����
			for (WORD i=0;i<pGetOnline->wServerCount;i++)
			{
				//��ȡ����
				WORD wServerID=pGetOnline->wOnLineServerID[i];
				CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(wServerID);

				//���ñ���
				if (pGameServerItem!=NULL)
				{
					WORD wServerIndex=ServerOnline.wServerCount++;
					ServerOnline.OnLineInfoServer[wServerIndex].wServerID=wServerID;
					ServerOnline.OnLineInfoServer[wServerIndex].dwOnLineCount=pGameServerItem->m_GameServer.dwOnLineCount;
				}

				//����ж�
				if (ServerOnline.wServerCount>=CountArray(ServerOnline.OnLineInfoServer))
				{
					ASSERT(FALSE);
					break;
				}
			}

			//��������
			if (KindOnline.wKindCount>0)
			{
				WORD wHeadSize=sizeof(KindOnline)-sizeof(KindOnline.OnLineInfoKind);
				WORD wSendSize=wHeadSize+KindOnline.wKindCount*sizeof(KindOnline.OnLineInfoKind[0]);
				SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GR_KINE_ONLINE,&KindOnline,wSendSize,&dwTokenID,1);
			}

			//��������
			if (ServerOnline.wServerCount>0)
			{
				WORD wHeadSize=sizeof(ServerOnline)-sizeof(ServerOnline.OnLineInfoServer);
				WORD wSendSize=wHeadSize+ServerOnline.wServerCount*sizeof(ServerOnline.OnLineInfoServer[0]);
				SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GR_SERVER_ONLINE,&ServerOnline,wSendSize,&dwTokenID,1);
			}

			return true;
		}
	case SUB_GP_GET_COLLECTION:		//��ȡ�ղ�
		{
			return true;
		}
	}

	return false;
}

//������
bool CAttemperEngineSink::OnTCPNetworkMainPCUserService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_MODIFY_ACCOUNTS:	//�޸��ʺ�	
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_ModifyAccounts));
			if (wDataSize!=sizeof(CMD_GP_ModifyAccounts)) return false;

			//��������
			CMD_GP_ModifyAccounts * pModifyAccounts=(CMD_GP_ModifyAccounts *)pData;

			//�ַ��ض�
			pModifyAccounts->szArea[CountArray(pModifyAccounts->szArea)-1]=0;
			pModifyAccounts->szCity[CountArray(pModifyAccounts->szCity)-1]=0;
			pModifyAccounts->szProvince[CountArray(pModifyAccounts->szProvince)-1]=0;
			pModifyAccounts->szAccounts[CountArray(pModifyAccounts->szAccounts)-1]=0;
			pModifyAccounts->szPassword[CountArray(pModifyAccounts->szPassword)-1]=0;
			pModifyAccounts->szUnderWrite[CountArray(pModifyAccounts->szUnderWrite)-1]=0;

			//���Ʋ���
			tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParameter == NULL) return false;

			//��������
			DBR_GP_ModifyAccounts ModifyAccounts;
			ZeroMemory(&ModifyAccounts,sizeof(ModifyAccounts));

			//�����Ա�
			ModifyAccounts.cbAge=pModifyAccounts->cbAge;
			ModifyAccounts.cbGender=pModifyAccounts->cbGender;

			//��֤����
			ModifyAccounts.dwUserID=pModifyAccounts->dwUserID;
			ModifyAccounts.dwClientIP= pTokenParameter->dwClientIP;
			StringCchCopy(ModifyAccounts.szPassword, CountArray(ModifyAccounts.szPassword), pModifyAccounts->szPassword);

			//�ʺ�����
			ModifyAccounts.wFaceID=pModifyAccounts->wFaceID;
			StringCchCopy(ModifyAccounts.szAccounts,CountArray(ModifyAccounts.szAccounts), pModifyAccounts->szAccounts);
			StringCchCopy(ModifyAccounts.szPassword,CountArray(ModifyAccounts.szPassword), pModifyAccounts->szPassword);
			StringCchCopy(ModifyAccounts.szUnderWrite,CountArray(ModifyAccounts.szUnderWrite), pModifyAccounts->szUnderWrite);

			//��������
			StringCchCopy(ModifyAccounts.szArea,CountArray(ModifyAccounts.szArea), pModifyAccounts->szArea);
			StringCchCopy(ModifyAccounts.szCity,CountArray(ModifyAccounts.szCity), pModifyAccounts->szCity);
			StringCchCopy(ModifyAccounts.szProvince,CountArray(ModifyAccounts.szProvince), pModifyAccounts->szProvince);
			ModifyAccounts.bOldVersion=(wSubCmdID==SUB_GP_MODIFY_ACCOUNTS);

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_ACCOUNTS, ModifyAccounts.dwUserID, dwSocketID,dwTokenID,&ModifyAccounts,sizeof(ModifyAccounts));

			return true;
		}
	case SUB_GP_MODIFY_LOGON_PASS:	//�޸�����		
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_ModifyLogonPass));
			if (wDataSize!=sizeof(CMD_GP_ModifyLogonPass)) return false;

			//������Ϣ
			CMD_GP_ModifyLogonPass * pModifyLogonPass=(CMD_GP_ModifyLogonPass *)pData;
			pModifyLogonPass->szDesPassword[CountArray(pModifyLogonPass->szDesPassword)-1]=0;
			pModifyLogonPass->szScrPassword[CountArray(pModifyLogonPass->szScrPassword)-1]=0;

			//���Ʋ���
			tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParameter == NULL) return false;

			//��������
			DBR_GP_ModifyLogonPass ModifyLogonPass;
			ZeroMemory(&ModifyLogonPass,sizeof(ModifyLogonPass));

			//��������
			ModifyLogonPass.dwUserID=pModifyLogonPass->dwUserID;
			ModifyLogonPass.dwClientIP= pTokenParameter->dwClientIP;
			StringCchCopy(ModifyLogonPass.szDesPassword,CountArray(ModifyLogonPass.szDesPassword), pModifyLogonPass->szDesPassword);
			StringCchCopy(ModifyLogonPass.szScrPassword,CountArray(ModifyLogonPass.szScrPassword), pModifyLogonPass->szScrPassword);

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_LOGON_PASS, ModifyLogonPass.dwUserID,dwSocketID,dwTokenID,&ModifyLogonPass,sizeof(ModifyLogonPass));

			return true;
		}		
	case SUB_GP_MODIFY_INSURE_PASS:	//�޸�����
		{
			//Ч�����
			ASSERT (wDataSize==sizeof(CMD_GP_ModifyInsurePass));
			if (wDataSize!=sizeof(CMD_GP_ModifyInsurePass)) return false;

			//������Ϣ
			CMD_GP_ModifyInsurePass * pModifyInsurePass=(CMD_GP_ModifyInsurePass *)pData;
			pModifyInsurePass->szDesPassword[CountArray(pModifyInsurePass->szDesPassword)-1]=0;
			pModifyInsurePass->szScrPassword[CountArray(pModifyInsurePass->szScrPassword)-1]=0;

			//���Ʋ���
			tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParameter == NULL) return false;

			//��������
			DBR_GP_ModifyInsurePass ModifyInsurePass;
			ZeroMemory(&ModifyInsurePass,sizeof(ModifyInsurePass));

			//��������
			ModifyInsurePass.dwUserID=pModifyInsurePass->dwUserID;
			ModifyInsurePass.dwClientIP= pTokenParameter->dwClientIP;
			StringCchCopy(ModifyInsurePass.szDesPassword,CountArray(ModifyInsurePass.szDesPassword), pModifyInsurePass->szDesPassword);
			StringCchCopy(ModifyInsurePass.szScrPassword,CountArray(ModifyInsurePass.szScrPassword), pModifyInsurePass->szScrPassword);

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_INSURE_PASS, ModifyInsurePass.dwUserID, dwSocketID,dwTokenID,&ModifyInsurePass,sizeof(ModifyInsurePass));

			return true;
		}
	case SUB_GP_MODIFY_INDIVIDUAL:	//�޸�����
		{
			//Ч�����
			ASSERT(wDataSize>=sizeof(CMD_GP_ModifyIndividual));
			if (wDataSize<sizeof(CMD_GP_ModifyIndividual)) return false;

			//������Ϣ
			CMD_GP_ModifyIndividual * pModifyIndividual=(CMD_GP_ModifyIndividual *)pData;
			pModifyIndividual->szPassword[CountArray(pModifyIndividual->szPassword)-1]=0;

			//���Ʋ���
			tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParameter == NULL) return false;

			//��������
			DBR_GP_ModifyIndividual ModifyIndividual;
			ZeroMemory(&ModifyIndividual,sizeof(ModifyIndividual));

			//���ñ���
			ModifyIndividual.dwUserID=pModifyIndividual->dwUserID;
			ModifyIndividual.dwClientIP= pTokenParameter->dwClientIP;
			StringCchCopy(ModifyIndividual.szPassword,CountArray(ModifyIndividual.szPassword),pModifyIndividual->szPassword);

			//��������
			VOID * pDataBuffer=NULL;
			tagDataDescribe DataDescribe;
			CRecvPacketHelper RecvPacket(pModifyIndividual+1,wDataSize-sizeof(CMD_GP_ModifyIndividual));

			//��չ��Ϣ
			while (true)
			{
				pDataBuffer=RecvPacket.GetData(DataDescribe);
				if (DataDescribe.wDataDescribe==DTP_NULL) break;
				switch (DataDescribe.wDataDescribe)
				{
				case DTP_GP_UI_USER_NOTE:		//�û���ע
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szUserNote));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szUserNote))
						{
							CopyMemory(&ModifyIndividual.szUserNote,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szUserNote[CountArray(ModifyIndividual.szUserNote)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_COMPELLATION:	//��ʵ����
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szCompellation));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szCompellation))
						{
							CopyMemory(&ModifyIndividual.szCompellation,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szCompellation[CountArray(ModifyIndividual.szCompellation)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_SEAT_PHONE:		//�̶��绰
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szSeatPhone));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szSeatPhone))
						{
							CopyMemory(ModifyIndividual.szSeatPhone,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szSeatPhone[CountArray(ModifyIndividual.szSeatPhone)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_MOBILE_PHONE:	//�ƶ��绰
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szMobilePhone));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szMobilePhone))
						{
							CopyMemory(ModifyIndividual.szMobilePhone,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szMobilePhone[CountArray(ModifyIndividual.szMobilePhone)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_QQ:				//Q Q ����
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szQQ));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szQQ))
						{
							CopyMemory(ModifyIndividual.szQQ,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szQQ[CountArray(ModifyIndividual.szQQ)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_EMAIL:			//�����ʼ�
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szEMail));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szEMail))
						{
							CopyMemory(ModifyIndividual.szEMail,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szEMail[CountArray(ModifyIndividual.szEMail)-1]=0;
						}
						break;
					}
				case DTP_GP_UI_DWELLING_PLACE:	//��ϵ��ַ
					{
						ASSERT(pDataBuffer!=NULL);
						ASSERT(DataDescribe.wDataSize<=sizeof(ModifyIndividual.szDwellingPlace));
						if (DataDescribe.wDataSize<=sizeof(ModifyIndividual.szDwellingPlace))
						{
							CopyMemory(ModifyIndividual.szDwellingPlace,pDataBuffer,DataDescribe.wDataSize);
							ModifyIndividual.szDwellingPlace[CountArray(ModifyIndividual.szDwellingPlace)-1]=0;
						}
						break;
					}
				}
			}

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_INDIVIDUAL, ModifyIndividual.dwUserID, dwSocketID,dwTokenID,&ModifyIndividual,sizeof(ModifyIndividual));

			return true;
		}	
	case SUB_GP_QUERY_INDIVIDUAL:	//��ѯ��Ϣ
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_QueryIndividual));
			if (wDataSize!=sizeof(CMD_GP_QueryIndividual)) return false;

			//������Ϣ
			CMD_GP_QueryIndividual * pQueryIndividual=(CMD_GP_QueryIndividual *)pData;

			//���Ʋ���
			tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParameter == NULL) return false;

			//��������
			DBR_GP_QueryIndividual QueryIndividual;
			ZeroMemory(&QueryIndividual,sizeof(QueryIndividual));

			//��������
			QueryIndividual.dwUserID=pQueryIndividual->dwUserID;
			QueryIndividual.dwClientIP= pTokenParameter->dwClientIP;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_INDIVIDUAL, QueryIndividual.dwUserID,dwSocketID,dwTokenID,&QueryIndividual,sizeof(QueryIndividual));

			return true;
		}
	case SUB_GP_BIND_MACHINE:		//��������
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_BindMachine));
			if (wDataSize!=sizeof(CMD_GP_BindMachine)) return false;

			//������Ϣ
			CMD_GP_BindMachine * pBindMachine=(CMD_GP_BindMachine *)pData;
			pBindMachine->szPassword[CountArray(pBindMachine->szPassword)-1] = 0;
			pBindMachine->szMachineID[CountArray(pBindMachine->szMachineID)-1] = 0;

			//��������
			DBR_GP_BindMachine BindMachine = {0};

			//��������
			BindMachine.dwUserID=pBindMachine->dwUserID;
			StringCchCopy( BindMachine.szPassword,CountArray(BindMachine.szPassword), pBindMachine->szPassword);
			StringCchCopy( BindMachine.szMachineID,CountArray(BindMachine.szMachineID), pBindMachine->szMachineID);

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_BIND_MACHINE, BindMachine.dwUserID,dwSocketID,dwTokenID,&BindMachine,sizeof(BindMachine));

			return true;
		}
	case SUB_GP_UN_BIND_MACHINE:	//��������
		{
			//Ч�����
			ASSERT(wDataSize==sizeof(CMD_GP_UnbindMachine));
			if (wDataSize!=sizeof(CMD_GP_UnbindMachine)) return false;

			//������Ϣ
			CMD_GP_UnbindMachine * pUnbindMachine=(CMD_GP_UnbindMachine *)pData;
			pUnbindMachine->szPassword[CountArray(pUnbindMachine->szPassword)-1] = 0;
			pUnbindMachine->szMachineID[CountArray(pUnbindMachine->szMachineID)-1] = 0;

			//��������
			DBR_GP_UnbindMachine UnbindMachine = {0};

			//��������
			UnbindMachine.dwUserID=pUnbindMachine->dwUserID;
			StringCchCopy( UnbindMachine.szPassword,CountArray(UnbindMachine.szPassword), pUnbindMachine->szPassword);
			StringCchCopy( UnbindMachine.szMachineID,CountArray(UnbindMachine.szMachineID), pUnbindMachine->szMachineID);

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UN_BIND_MACHINE, UnbindMachine.dwUserID,dwSocketID,dwTokenID,&UnbindMachine,sizeof(UnbindMachine));

			return true;
		}	
	}

	return false;
}

//��¼����
bool CAttemperEngineSink::OnTCPNetworkMainMBLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_MB_LOGON_EMAIL:		//�����½
	{
		return OnTCPNetworkSubMBLogonEMail(pData, wDataSize, dwSocketID, dwTokenID);
	}
	case SUB_MB_LOGON_EMAILCODE:   //������֤��
	{
		return OnTCPNetworkSubMBLogonEMailCode(pData, wDataSize, dwSocketID, dwTokenID);
	}
	case SUB_MB_LOGON_VISITOR:		//�ο͵�¼
	{
		return OnTCPNetworkSubMBLogonVisitor(pData,wDataSize,dwSocketID,dwTokenID);
	}
	case SUB_MB_LOGON_SECRETKEY:	//��Կ��¼
	{
		return OnTCPNetworkSubMBLogonSecretKey(pData,wDataSize,dwSocketID,dwTokenID);
	}
	case SUB_MB_LOGON_ACCOUNTS:		//�ʺŵ�¼
	{
		return OnTCPNetworkSubMBLogonAccounts(pData,wDataSize,dwSocketID,dwTokenID);			
	}	
	case SUB_MB_LOGON_MOBILECODE:	//�ֻ���֤��
	{
		return OnTCPNetworkSubMBLogonMobileCode(pData, wDataSize, dwSocketID, dwTokenID);
	}
	case SUB_MB_LOGON_THIRDPARTY:	//��������½
	{
		return OnTCPNetworkSubMBLogonThirdParty(pData,wDataSize,dwSocketID,dwTokenID);			
	}
	case SUB_MB_LOGON_MOBILEPHONE:	//�ֻ���½
	{
		return OnTCPNetworkSubMBLogonMobilePhone(pData, wDataSize, dwSocketID, dwTokenID);
	}
	case SUB_MB_REGISTER_EMAIL:		//����ע��
	{
		return OnTCPNetworkSubMBRegisterEMail(pData, wDataSize, dwSocketID, dwTokenID);
	}	
	case SUB_MB_REGISTER_ACCOUNTS:	//�ʺ�ע��
	{
		return OnTCPNetworkSubMBRegisterAccounts(pData, wDataSize, dwSocketID, dwTokenID);
	}
	case SUB_MB_REGISTER_MOBILEPHONE://�ֻ�ע��
	{
		return OnTCPNetworkSubMBRegisterMobilePhone(pData, wDataSize, dwSocketID, dwTokenID);
	}
	}

	return false;
}

//�б���
bool CAttemperEngineSink::OnTCPNetworkMainMBServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_MB_GET_LIST:		//��ȡ����
		{			
			//Ч������
			ASSERT (wDataSize==sizeof(DWORD));
			if (wDataSize!=sizeof(DWORD)) return false;

			//��ȡ����
			auto pTokenParemeter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParemeter == NULL) return false;

			//������Ϸ
			SendMobileKindInfo(dwSocketID,dwTokenID, (tagExtendParemter*)pTokenParemeter->cbExtendData);

			//�������
			SendDataToGate(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_FINISH,&dwTokenID,1);

			return true;
		}
	case SUB_MB_GET_SERVER:		//��ȡ����
		{
			//Ч������
			ASSERT (wDataSize==sizeof(tagGetServer));
			if (wDataSize!=sizeof(tagGetServer)) return false;

			//��ȡ����
			tagGetServer * pGetServer=(tagGetServer *)pData;

			//��ȡ����
			auto pTokenParemeter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParemeter == NULL) return false;

			//�����б�
			SendMobileServerInfo(dwSocketID, dwTokenID, pGetServer->wKindID, (tagExtendParemter*)pTokenParemeter->cbExtendData);

			//�������
			SendDataToGate(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_SERVER_FINISH,pData,wDataSize,&dwTokenID,1);

			return true;
		}	
	case SUB_MB_GET_ONLINE:		//��ȡ����
		{
			//Ч������
			ASSERT (wDataSize == sizeof(tagGetOnline));
			if (wDataSize != sizeof(tagGetOnline)) return false;

			//��ȡ����
			tagGetOnline* pGetOnline = (tagGetOnline*)pData;

			//��ȡ����
			auto pTokenParemeter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParemeter == NULL) return false;

			//����������Ϣ
			SendMobileOnlineInfo(dwSocketID, dwTokenID, pGetOnline->wKindID, (tagExtendParemter*)pTokenParemeter->cbExtendData);

			return true;
		}
	case SUB_MB_GET_JACKPOT:	//��ȡ����
		{
			//��ȡ����
			auto pTokenParemeter = GetTokenParameter(dwSocketID, dwTokenID);
			if (pTokenParemeter == NULL) return false;

			//���ͽ�����Ϣ
			SendMobileJackpotInfo(dwSocketID, dwTokenID, (tagExtendParemter*)pTokenParemeter->cbExtendData);

			return true;
		}
	}

	return false;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkMainMBUserService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{	
	case SUB_MB_LIKE_PLAYER:		//�������
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_LikePlayer));
		if (wDataSize != sizeof(CMD_MB_LikePlayer)) return false;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��ȡ����
		CMD_MB_LikePlayer* pLikePlayer = (CMD_MB_LikePlayer*)pData;

		//��������
		DBR_MB_LikePlayer LikePlayer;
		ZeroMemory(&LikePlayer, sizeof(LikePlayer));

		//��������
		LikePlayer.dwUserID = pTokenParameter->dwUserID;
		LikePlayer.dwDstUserID = pLikePlayer->dwDstUserID;
		LikePlayer.dwClientIP = pTokenParameter->dwClientIP;				

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LIKE_PLAYER, LikePlayer.dwUserID, dwSocketID, dwTokenID, &LikePlayer, sizeof(LikePlayer));

		return true;
	}
	case SUB_MB_MODIFY_LOGON_PASS:	//�޸�����
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_ModifyLogonPass));
		if (wDataSize != sizeof(CMD_MB_ModifyLogonPass)) return false;

		//������Ϣ
		CMD_MB_ModifyLogonPass* pModifyLogonPass = (CMD_MB_ModifyLogonPass*)pData;
		pModifyLogonPass->szDesPassword[CountArray(pModifyLogonPass->szDesPassword) - 1] = 0;
		pModifyLogonPass->szScrPassword[CountArray(pModifyLogonPass->szScrPassword) - 1] = 0;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_ModifyLogonPass ModifyLogonPass;
		ZeroMemory(&ModifyLogonPass, sizeof(ModifyLogonPass));

		//��������
		ModifyLogonPass.dwUserID = pTokenParameter->dwUserID;
		ModifyLogonPass.dwClientIP = pTokenParameter->dwClientIP;		
		StringCchCopy(ModifyLogonPass.szDesPassword, CountArray(ModifyLogonPass.szDesPassword), pModifyLogonPass->szDesPassword);
		StringCchCopy(ModifyLogonPass.szScrPassword, CountArray(ModifyLogonPass.szScrPassword), pModifyLogonPass->szScrPassword);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_MODIFY_LOGON_PASS, ModifyLogonPass.dwUserID, dwSocketID, dwTokenID, &ModifyLogonPass, sizeof(ModifyLogonPass));

		return true;
	}	
	case SUB_MB_MODIFY_INDIVIDUAL:	//�޸�����
	{
		//Ч�����
		ASSERT(wDataSize >= sizeof(CMD_MB_ModifyIndividual));
		if (wDataSize < sizeof(CMD_MB_ModifyIndividual)) return false;

		//������Ϣ
		CMD_MB_ModifyIndividual* pModifyIndividual = (CMD_MB_ModifyIndividual*)pData;
		
		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_ModifyIndividual ModifyIndividual;
		ZeroMemory(&ModifyIndividual, sizeof(ModifyIndividual));

		//���ñ���
		ModifyIndividual.dwUserID = pTokenParameter->dwUserID;
		ModifyIndividual.cbGender = pModifyIndividual->cbGender;
		ModifyIndividual.dwClientIP = pTokenParameter->dwClientIP;		
		
		//��������
		VOID* pDataBuffer = NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pModifyIndividual + 1, wDataSize - sizeof(CMD_MB_ModifyIndividual));

		//��չ��Ϣ
		while (true)
		{
			pDataBuffer = RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe == DTP_NULL) break;
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_MB_NICKNAME:			//�û��ǳ�
			{
				ASSERT(pDataBuffer != NULL);
				ASSERT(DataDescribe.wDataSize <= sizeof(ModifyIndividual.szNickName));
				if (DataDescribe.wDataSize <= sizeof(ModifyIndividual.szNickName))
				{
					ModifyIndividual.wInfoMask |= DTP_MB_NICKNAME;
					CopyMemory(&ModifyIndividual.szNickName, pDataBuffer, DataDescribe.wDataSize);
					ModifyIndividual.szNickName[CountArray(ModifyIndividual.szNickName) - 1] = 0;
				}
				break;
			}
			case DTP_MB_UNDER_WRITE:		//����ǩ��
			{
				ASSERT(pDataBuffer != NULL);
				ASSERT(DataDescribe.wDataSize <= sizeof(ModifyIndividual.szUnderWrite));
				if (DataDescribe.wDataSize <= sizeof(ModifyIndividual.szUnderWrite))
				{
					ModifyIndividual.wInfoMask |= DTP_MB_UNDER_WRITE;
					CopyMemory(&ModifyIndividual.szUnderWrite, pDataBuffer, DataDescribe.wDataSize);
					ModifyIndividual.szUnderWrite[CountArray(ModifyIndividual.szUnderWrite) - 1] = 0;
				}
				break;
			}
			}
		}

		//У���ǳ�
		if ((ModifyIndividual.wInfoMask&DTP_MB_NICKNAME)!=0 && m_WordsFilter.FilterWords(ModifyIndividual.szNickName))
		{
			//����ʧ��
			SendOperateFailure(dwSocketID, dwTokenID, SUB_MB_MODIFY_INDIVIDUAL, -1, CWHService::FormatMessage(MIM_LOGON_0009, TEXT("��Ǹ������������ǳƺ��������ַ�����������ǳƺ����ٴ��޸ģ�")));

			return true;
		}

		//У��ǩ��
		if ((ModifyIndividual.wInfoMask&DTP_MB_UNDER_WRITE)!=0 && m_WordsFilter.FilterWords(ModifyIndividual.szUnderWrite))
		{
			//����ʧ��
			SendOperateFailure(dwSocketID, dwTokenID, SUB_MB_MODIFY_INDIVIDUAL, -1, CWHService::FormatMessage(MIM_LOGON_0010, TEXT("��Ǹ����������ĸ���ǩ�����������ַ���������������ٴ��޸ģ�")));

			return true;
		}

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_MODIFY_INDIVIDUAL, ModifyIndividual.dwUserID, dwSocketID, dwTokenID, &ModifyIndividual, sizeof(ModifyIndividual));

		return true;
	}
	case SUB_MB_QUERY_WEALTH:		//��ѯ�Ƹ�
	{
		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_QueryWealth QueryWealth;
		ZeroMemory(&QueryWealth, sizeof(QueryWealth));

		//��������
		QueryWealth.dwUserID = pTokenParameter->dwUserID;
		QueryWealth.dwClientIP = pTokenParameter->dwClientIP;		

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_QUERY_WEALTH, QueryWealth.dwUserID, dwSocketID, dwTokenID, &QueryWealth, sizeof(QueryWealth));

		return true;
	}
	case SUB_MB_QUERY_RANKINGINFO:	//��ѯ��λ
	{
		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_QueryRankingInfo QueryRankingInfo;
		ZeroMemory(&QueryRankingInfo, sizeof(QueryRankingInfo));

		//��������
		QueryRankingInfo.dwUserID = pTokenParameter->dwUserID;
		QueryRankingInfo.dwClientIP = pTokenParameter->dwClientIP;		

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_QUERY_RANKINGINFO, QueryRankingInfo.dwUserID, dwSocketID, dwTokenID, &QueryRankingInfo, sizeof(QueryRankingInfo));

		return true;
	}	
	case SUB_MB_MODIFY_SYSTEM_FACE:	//�޸�ͷ��
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_ModifySystemFace));
		if (wDataSize != sizeof(CMD_MB_ModifySystemFace)) return false;

		//������Ϣ
		CMD_MB_ModifySystemFace* pModifySystemFace = (CMD_MB_ModifySystemFace*)pData;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_ModifySystemFace ModifySystemFace;
		ZeroMemory(&ModifySystemFace, sizeof(ModifySystemFace));

		//��������
		ModifySystemFace.dwUserID = pTokenParameter->dwUserID;
		ModifySystemFace.wFaceID = pModifySystemFace->wFaceID;
		ModifySystemFace.dwClientIP = pTokenParameter->dwClientIP;		
		StringCchCopy(ModifySystemFace.szMachineID, CountArray(ModifySystemFace.szMachineID), pModifySystemFace->szMachineID);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_MODIFY_SYSTEM_FACE, ModifySystemFace.dwUserID, dwSocketID, dwTokenID, &ModifySystemFace, sizeof(ModifySystemFace));

		return true;
	}
	case SUB_MB_MODIFY_CUSTOM_FACE:	//�޸�ͷ��
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_ModifyCustomFace));
		if (wDataSize != sizeof(CMD_MB_ModifyCustomFace)) return false;

		//������Ϣ
		CMD_MB_ModifyCustomFace* pModifyCustomFace = (CMD_MB_ModifyCustomFace*)pData;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_ModifyCustomFace ModifyCustomFace;
		ZeroMemory(&ModifyCustomFace, sizeof(ModifyCustomFace));

		//��������
		ModifyCustomFace.dwUserID = pTokenParameter->dwUserID;
		ModifyCustomFace.dwCustomID = pModifyCustomFace->dwCustomID;				
		StringCchCopy(ModifyCustomFace.szFacePath,CountArray(ModifyCustomFace.szFacePath), pModifyCustomFace->szFacePath);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_MODIFY_CUSTOM_FACE, ModifyCustomFace.dwUserID, dwSocketID, dwTokenID, &ModifyCustomFace, sizeof(ModifyCustomFace));

		return true;
	}	
	case SUB_MB_EXCHANGE_MALLGOODS: //�һ���Ʒ
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_ExchangeMallGoods));
		if (wDataSize != sizeof(CMD_MB_ExchangeMallGoods)) return false;

		//������Ϣ
		CMD_MB_ExchangeMallGoods* pExchangeMallGoods = (CMD_MB_ExchangeMallGoods*)pData;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_ExchangeMallGoods ExchangeMallGoods;
		ZeroMemory(&ExchangeMallGoods, sizeof(ExchangeMallGoods));

		//��������
		ExchangeMallGoods.dwUserID = pTokenParameter->dwUserID;
		ExchangeMallGoods.dwMallGoodsID = pExchangeMallGoods->dwMallGoodsID;		

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_EXCHANGE_MALLGOODS, ExchangeMallGoods.dwUserID, dwSocketID, dwTokenID, &ExchangeMallGoods, sizeof(ExchangeMallGoods));

		return true;
	}
	case SUB_MB_EXCHANGE_GIFTBAGCODE: //�һ������
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_ExchangeGiftbagCode));
		if (wDataSize != sizeof(CMD_MB_ExchangeGiftbagCode)) return false;

		//������Ϣ
		CMD_MB_ExchangeGiftbagCode* pExchangeGiftbagCode = (CMD_MB_ExchangeGiftbagCode*)pData;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_ExchangeGiftbagCode ExchangeGiftbagCode;
		ZeroMemory(&ExchangeGiftbagCode, sizeof(ExchangeGiftbagCode));

		//��������
		ExchangeGiftbagCode.dwUserID = pTokenParameter->dwUserID;
		ExchangeGiftbagCode.dwClientIP = pTokenParameter->dwClientIP;		
		StringCchCopy(ExchangeGiftbagCode.szGiftbagCode,CountArray(ExchangeGiftbagCode.szGiftbagCode), pExchangeGiftbagCode->szGiftbagCode);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_EXCHANGE_GIFTBAGCODE, ExchangeGiftbagCode.dwUserID, dwSocketID, dwTokenID, &ExchangeGiftbagCode, sizeof(ExchangeGiftbagCode));

		return true;
	}
	case SUB_MB_ACQUIRE_AUTHCODE:	//��ȡ��֤��
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_AcquireAuthCode));
		if (wDataSize != sizeof(CMD_MB_AcquireAuthCode)) return false;

		//������Ϣ
		CMD_MB_AcquireAuthCode* pAcquireAuthCode = (CMD_MB_AcquireAuthCode*)pData;
		pAcquireAuthCode->szPassword[CountArray(pAcquireAuthCode->szPassword) - 1] = 0;
		pAcquireAuthCode->szMachineID[CountArray(pAcquireAuthCode->szMachineID) - 1] = 0;
		pAcquireAuthCode->szMobileEmail[CountArray(pAcquireAuthCode->szMobileEmail) - 1] = 0;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		
		//��������
		DBR_MB_AcquireAuthCode AcquireAuthCode;
		ZeroMemory(&AcquireAuthCode, sizeof(AcquireAuthCode));

		//�û���Ϣ
		AcquireAuthCode.dwUserID = pAcquireAuthCode->dwUserID;
		StringCchCopy(AcquireAuthCode.szPassword, CountArray(AcquireAuthCode.szPassword), pAcquireAuthCode->szPassword);

		//��֤��Ϣ
		AcquireAuthCode.cbCodeKind = pAcquireAuthCode->cbCodeKind;
		AcquireAuthCode.cbAuthKind = pAcquireAuthCode->cbAuthKind;
		AcquireAuthCode.dwBindUserID = pAcquireAuthCode->dwBindUserID;
		StringCchCopy(AcquireAuthCode.szMobileEmail, CountArray(AcquireAuthCode.szMobileEmail), pAcquireAuthCode->szMobileEmail);

		//������֤��
		MakeAuthCode(AcquireAuthCode.szAuthCode, CountArray(AcquireAuthCode.szAuthCode));

		//������֤��			
		WCHAR szValidateCode[64];
		_snwprintf_s(szValidateCode, CountArray(szValidateCode), L"AutchCode:%c-%c-%c-%c-%c-%c", AcquireAuthCode.szAuthCode[0], AcquireAuthCode.szAuthCode[1], AcquireAuthCode.szAuthCode[2], AcquireAuthCode.szAuthCode[3], AcquireAuthCode.szAuthCode[4], AcquireAuthCode.szAuthCode[5]);

		//������֤��
		CMD5Encrypt::EncryptData(szValidateCode, AcquireAuthCode.szAuthCodeMD5);

		//������Ϣ
		AcquireAuthCode.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(AcquireAuthCode.szMachineID, CountArray(AcquireAuthCode.szMachineID), pAcquireAuthCode->szMachineID);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_ACQUIRE_AUTHCODE, AcquireAuthCode.dwUserID, dwSocketID, dwTokenID, &AcquireAuthCode, sizeof(AcquireAuthCode));

		return true;
	}
	case SUB_MB_BIND_MOBILEPHONE:	//���ֻ���
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_BindMobilePhone));
		if (wDataSize != sizeof(CMD_MB_BindMobilePhone)) return false;

		//������Ϣ
		CMD_MB_BindMobilePhone* pBindMobilePhone = (CMD_MB_BindMobilePhone*)pData;
		pBindMobilePhone->szPassword[CountArray(pBindMobilePhone->szPassword) - 1] = 0;
		pBindMobilePhone->szNewPassword[CountArray(pBindMobilePhone->szNewPassword) - 1] = 0;
		pBindMobilePhone->szMachineID[CountArray(pBindMobilePhone->szMachineID) - 1] = 0;
		pBindMobilePhone->szAuthCode[CountArray(pBindMobilePhone->szAuthCode) - 1] = 0;
		pBindMobilePhone->szMobilePhone[CountArray(pBindMobilePhone->szMobilePhone) - 1] = 0;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;

		//��������
		DBR_MB_BindMobilePhone BindMobilePhone;
		ZeroMemory(&BindMobilePhone, sizeof(BindMobilePhone));

		//�û���Ϣ
		BindMobilePhone.dwUserID = pBindMobilePhone->dwUserID;
		StringCchCopy(BindMobilePhone.szPassword, CountArray(BindMobilePhone.szPassword), pBindMobilePhone->szPassword);
		StringCchCopy(BindMobilePhone.szNewPassword, CountArray(BindMobilePhone.szNewPassword), pBindMobilePhone->szNewPassword);

		//��֤��Ϣ
		BindMobilePhone.dwUnbindUserID = pBindMobilePhone->dwUnbindUserID;
		StringCchCopy(BindMobilePhone.szAuthCode, CountArray(BindMobilePhone.szAuthCode), pBindMobilePhone->szAuthCode);
		StringCchCopy(BindMobilePhone.szMobilePhone, CountArray(BindMobilePhone.szMobilePhone), pBindMobilePhone->szMobilePhone);

		//������Ϣ			
		BindMobilePhone.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(BindMobilePhone.szMachineID, CountArray(BindMobilePhone.szMachineID), pBindMobilePhone->szMachineID);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_BIND_MOBILEPHONE, BindMobilePhone.dwUserID, dwSocketID, dwTokenID, &BindMobilePhone, sizeof(BindMobilePhone));

		return true;
	}
	case SUB_MB_UNBIND_MOBILEPHONE:	//����ֻ���
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_UnbindMobilePhone));
		if (wDataSize != sizeof(CMD_MB_UnbindMobilePhone)) return false;

		//������Ϣ
		CMD_MB_UnbindMobilePhone* pUnbindMobilePhone = (CMD_MB_UnbindMobilePhone*)pData;
		pUnbindMobilePhone->szPassword[CountArray(pUnbindMobilePhone->szPassword) - 1] = 0;
		pUnbindMobilePhone->szMachineID[CountArray(pUnbindMobilePhone->szMachineID) - 1] = 0;
		pUnbindMobilePhone->szAuthCode[CountArray(pUnbindMobilePhone->szAuthCode) - 1] = 0;
		pUnbindMobilePhone->szMobilePhone[CountArray(pUnbindMobilePhone->szMobilePhone) - 1] = 0;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;

		//��������
		DBR_MB_UnbindMobilePhone UnbindMobilePhone;
		ZeroMemory(&UnbindMobilePhone, sizeof(UnbindMobilePhone));

		//�û���Ϣ
		UnbindMobilePhone.dwUserID = pUnbindMobilePhone->dwUserID;
		StringCchCopy(UnbindMobilePhone.szPassword, CountArray(UnbindMobilePhone.szPassword), pUnbindMobilePhone->szPassword);

		//��֤��Ϣ
		StringCchCopy(UnbindMobilePhone.szAuthCode, CountArray(UnbindMobilePhone.szAuthCode), pUnbindMobilePhone->szAuthCode);
		StringCchCopy(UnbindMobilePhone.szMobilePhone, CountArray(UnbindMobilePhone.szMobilePhone), pUnbindMobilePhone->szMobilePhone);

		//������Ϣ		
		UnbindMobilePhone.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(UnbindMobilePhone.szMachineID, CountArray(UnbindMobilePhone.szMachineID),pUnbindMobilePhone->szMachineID);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_UNBIND_MOBILEPHONE, UnbindMobilePhone.dwUserID, dwSocketID, dwTokenID, &UnbindMobilePhone, sizeof(UnbindMobilePhone));

		return true;
	}
	case SUB_MB_BIND_EMAIL:			//������
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_BindEMail));
		if (wDataSize != sizeof(CMD_MB_BindEMail)) return false;

		//������Ϣ
		CMD_MB_BindEMail* pBindEMail = (CMD_MB_BindEMail*)pData;
		pBindEMail->szPassword[CountArray(pBindEMail->szPassword) - 1] = 0;
		pBindEMail->szNewPassword[CountArray(pBindEMail->szNewPassword) - 1] = 0;
		pBindEMail->szMachineID[CountArray(pBindEMail->szMachineID) - 1] = 0;
		pBindEMail->szAuthCode[CountArray(pBindEMail->szAuthCode) - 1] = 0;
		pBindEMail->szEMail[CountArray(pBindEMail->szEMail) - 1] = 0;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;

		//��������
		DBR_MB_BindEMail BindEMail;
		ZeroMemory(&BindEMail, sizeof(BindEMail));

		//�û���Ϣ
		BindEMail.dwUserID = pBindEMail->dwUserID;
		StringCchCopy(BindEMail.szPassword, CountArray(BindEMail.szPassword), pBindEMail->szPassword);
		StringCchCopy(BindEMail.szNewPassword, CountArray(BindEMail.szNewPassword), pBindEMail->szNewPassword);

		//��֤��Ϣ
		BindEMail.dwUnbindUserID = pBindEMail->dwUnbindUserID;
		StringCchCopy(BindEMail.szAuthCode, CountArray(BindEMail.szAuthCode), pBindEMail->szAuthCode);
		StringCchCopy(BindEMail.szEMail, CountArray(BindEMail.szEMail), pBindEMail->szEMail);

		//������Ϣ			
		BindEMail.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(BindEMail.szMachineID, CountArray(BindEMail.szMachineID), pBindEMail->szMachineID);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_BIND_EMAIL, BindEMail.dwUserID, dwSocketID, dwTokenID, &BindEMail, sizeof(BindEMail));

		return true;
	}
	case SUB_MB_UNBIND_EMAIL:		//�������
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_UnbindEMail));
		if (wDataSize != sizeof(CMD_MB_UnbindEMail)) return false;

		//������Ϣ
		CMD_MB_UnbindEMail* pUnbindEMail = (CMD_MB_UnbindEMail*)pData;
		pUnbindEMail->szPassword[CountArray(pUnbindEMail->szPassword) - 1] = 0;
		pUnbindEMail->szMachineID[CountArray(pUnbindEMail->szMachineID) - 1] = 0;
		pUnbindEMail->szAuthCode[CountArray(pUnbindEMail->szAuthCode) - 1] = 0;
		pUnbindEMail->szEMail[CountArray(pUnbindEMail->szEMail) - 1] = 0;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;

		//��������
		DBR_MB_UnbindEMail UnbindEMail;
		ZeroMemory(&UnbindEMail, sizeof(UnbindEMail));

		//�û���Ϣ
		UnbindEMail.dwUserID = pUnbindEMail->dwUserID;
		StringCchCopy(UnbindEMail.szPassword, CountArray(UnbindEMail.szPassword), pUnbindEMail->szPassword);

		//��֤��Ϣ
		StringCchCopy(UnbindEMail.szAuthCode, CountArray(UnbindEMail.szAuthCode), pUnbindEMail->szAuthCode);
		StringCchCopy(UnbindEMail.szEMail, CountArray(UnbindEMail.szEMail), pUnbindEMail->szEMail);

		//������Ϣ		
		UnbindEMail.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(UnbindEMail.szMachineID, CountArray(UnbindEMail.szMachineID), pUnbindEMail->szMachineID);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_UNBIND_EMAIL, UnbindEMail.dwUserID, dwSocketID, dwTokenID, &UnbindEMail, sizeof(UnbindEMail));

		return true;
	}
	case SUB_MB_RESET_LOGON_PASS:	//��������
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_ResetLogonPasswd));
		if (wDataSize != sizeof(CMD_MB_ResetLogonPasswd)) return false;

		//������Ϣ
		CMD_MB_ResetLogonPasswd* pResetLogonPasswd = (CMD_MB_ResetLogonPasswd*)pData;
		pResetLogonPasswd->szPassword[CountArray(pResetLogonPasswd->szPassword) - 1] = 0;
		pResetLogonPasswd->szMachineID[CountArray(pResetLogonPasswd->szMachineID) - 1] = 0;
		pResetLogonPasswd->szAuthCode[CountArray(pResetLogonPasswd->szAuthCode) - 1] = 0;
		pResetLogonPasswd->szMobileEmail[CountArray(pResetLogonPasswd->szMobileEmail) - 1] = 0;

		//��������
		DBR_MB_ResetLogonPasswd ResetLogonPasswd;
		ZeroMemory(&ResetLogonPasswd, sizeof(ResetLogonPasswd));

		//��֤��Ϣ
		ResetLogonPasswd.cbCodeKind = pResetLogonPasswd->cbCodeKind;
		ResetLogonPasswd.dwBindUserID = pResetLogonPasswd->dwBindUserID;
		StringCchCopy(ResetLogonPasswd.szPassword, CountArray(ResetLogonPasswd.szPassword), pResetLogonPasswd->szPassword);
		StringCchCopy(ResetLogonPasswd.szAuthCode, CountArray(ResetLogonPasswd.szAuthCode), pResetLogonPasswd->szAuthCode);
		StringCchCopy(ResetLogonPasswd.szMobileEmail, CountArray(ResetLogonPasswd.szMobileEmail), pResetLogonPasswd->szMobileEmail);

		//������Ϣ			
		StringCchCopy(ResetLogonPasswd.szMachineID, CountArray(ResetLogonPasswd.szMachineID), pResetLogonPasswd->szMachineID);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_RESET_LOGON_PASS, ResetLogonPasswd.dwBindUserID, dwSocketID, dwTokenID, &ResetLogonPasswd, sizeof(ResetLogonPasswd));

		return true;
	}	
	case SUB_MB_REQUEST_CHECKIN:	 //����ǩ��
	{
		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��ȡ����
		auto pRequestCheckIn = (CMD_MB_RequestCheckIn*)pData;

		//ǩ������
		BOOL bQuery = false;
		auto CheckInOptionPtr = g_GlobalOptionManager->GetOptionItem<CCheckInOptionPtr>(OPTION_MASK_CHECKIN);
		if (CheckInOptionPtr && lstrcmp(pRequestCheckIn->szOptionDigest, CheckInOptionPtr->GetOptionDigest()) != 0)
		{
			bQuery = true;

			//ǩ������
			CMD_MB_CheckInOption CheckInOption = {};

			//���ñ���
			CheckInOption.bCheckInEnabled = CheckInOptionPtr->IsCheckInEnabled();
			StringCchCopy(CheckInOption.szOptionDigest, CountArray(CheckInOption.szOptionDigest), CheckInOptionPtr->GetOptionDigest());
			StringCchCopy(CheckInOption.szCheckInRewards, CountArray(CheckInOption.szCheckInRewards), CheckInOptionPtr->GetCheckInRewards());

			//ǩ������
			WORD wStringSize = CountStringBuffer(CheckInOption.szCheckInRewards);
			WORD wPacketSize = sizeof(CheckInOption) - sizeof(CheckInOption.szCheckInRewards) + wStringSize;
			SendDataToGate(dwSocketID, MDM_MB_USER_SERVICE, SUB_MB_CHECKIN_OPTION, &CheckInOption, wPacketSize, &dwTokenID, 1);
		}

		//��������
		DBR_MB_RequestCheckIn RequestCheckIn;
		ZeroMemory(&RequestCheckIn, sizeof(RequestCheckIn));

		//�û���Ϣ
		RequestCheckIn.dwUserID = pTokenParameter->dwUserID;
		RequestCheckIn.bQuery = bQuery;
		RequestCheckIn.dwClientIP = pTokenParameter->dwClientIP;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_REQUEST_CHECKIN, RequestCheckIn.dwUserID, dwSocketID, dwTokenID, &RequestCheckIn, sizeof(RequestCheckIn));

		return true;
	}
	case SUB_MB_BIND_INVITE_CODE:	//��������
	{
        //Ч�����
        ASSERT(wDataSize == sizeof(CMD_MB_BindInviteCode));
        if (wDataSize != sizeof(CMD_MB_BindInviteCode)) return false;

        //������Ϣ
		CMD_MB_BindInviteCode* pBindInviteCode = (CMD_MB_BindInviteCode*)pData;
		pBindInviteCode->szMachineID[CountArray(pBindInviteCode->szMachineID) - 1] = 0;

        //���Ʋ���
        tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
        if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

        //��������
		DBR_MB_BindInviteCode BindInviteCode;
        ZeroMemory(&BindInviteCode, sizeof(BindInviteCode));

        //�û���Ϣ
		BindInviteCode.dwUserID = pTokenParameter->dwUserID;
        BindInviteCode.dwInviteCode = pBindInviteCode->dwInviteCode;		
        StringCchCopy(BindInviteCode.szPassword, CountArray(BindInviteCode.szPassword), pBindInviteCode->szPassword);

        //������Ϣ			
		BindInviteCode.dwClientIP = pTokenParameter->dwClientIP;
        StringCchCopy(BindInviteCode.szMachineID, CountArray(BindInviteCode.szMachineID), pBindInviteCode->szMachineID);

        //Ͷ������
        m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_BIND_INVITE_CODE, BindInviteCode.dwUserID, dwSocketID, dwTokenID, &BindInviteCode, sizeof(BindInviteCode));

        return true;
	}
	case SUB_MB_SUBMIT_INFORM_INFO:	//�ύͶ��
	{
		//Ч�����
		ASSERT (wDataSize >= sizeof(CMD_MB_SubmitInformInfo));
		if (wDataSize < sizeof(CMD_MB_SubmitInformInfo)) return false;

		//��ȡ����
		auto pSubmitInformInfo = (CMD_MB_SubmitInformInfo*)pData;
		
		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//����ṹ
		DBR_MB_SubmitInformInfo SubmitInformInfo;
		ZeroMemory(&SubmitInformInfo, sizeof(SubmitInformInfo));

		//���ñ���
		SubmitInformInfo.dwInformUserID = pTokenParameter->dwUserID;
        SubmitInformInfo.cbInformKind = pSubmitInformInfo->cbInformKind;
        SubmitInformInfo.cbInformSource = pSubmitInformInfo->cbInformSource;
		SubmitInformInfo.dwBeInformUserID = pSubmitInformInfo->dwBeInformUserID;		

        //��������
        VOID* pDataBuffer = NULL;
        tagDataDescribe DataDescribe;
        CRecvPacketHelper RecvPacket(pSubmitInformInfo + 1, wDataSize - sizeof(CMD_MB_SubmitInformInfo));

        //��չ��Ϣ
        while (true)
        {
            pDataBuffer = RecvPacket.GetData(DataDescribe);
            if (DataDescribe.wDataDescribe == DTP_NULL) break;
            switch (DataDescribe.wDataDescribe)
            {
            case DTP_MB_INFORM_ATTACHMENTURL:			//������ַ
            {
                ASSERT(pDataBuffer != NULL);
                ASSERT(DataDescribe.wDataSize <= sizeof(SubmitInformInfo.szAttachmentUrl));
                if (DataDescribe.wDataSize <= sizeof(SubmitInformInfo.szAttachmentUrl))
                {
                    CopyMemory(&SubmitInformInfo.szAttachmentUrl, pDataBuffer, DataDescribe.wDataSize);
					SubmitInformInfo.szAttachmentUrl[CountArray(SubmitInformInfo.szAttachmentUrl) - 1] = 0;
                }
                break;
            }
            case DTP_MB_INFORM_MESSAGE:					//Ͷ����Ϣ
            {
                ASSERT(pDataBuffer != NULL);
                ASSERT(DataDescribe.wDataSize <= sizeof(SubmitInformInfo.szInformMessage));
                if (DataDescribe.wDataSize <= sizeof(SubmitInformInfo.szInformMessage))
                {
                    CopyMemory(&SubmitInformInfo.szInformMessage, pDataBuffer, DataDescribe.wDataSize);
                    SubmitInformInfo.szInformMessage[CountArray(SubmitInformInfo.szInformMessage) - 1] = 0;
                }
                break;
            }
            case DTP_MB_INFORM_CONTENT:					//Ͷ������
            {
                ASSERT(pDataBuffer != NULL);
                ASSERT(DataDescribe.wDataSize <= sizeof(SubmitInformInfo.szInformContent));
                if (DataDescribe.wDataSize <= sizeof(SubmitInformInfo.szInformContent))
                {
                    CopyMemory(&SubmitInformInfo.szInformContent, pDataBuffer, DataDescribe.wDataSize);
                    SubmitInformInfo.szInformContent[CountArray(SubmitInformInfo.szInformContent) - 1] = 0;
                }
                break;
            }
            }
        }

		//������Ϣ			
		SubmitInformInfo.dwClientIP = pTokenParameter->dwClientIP;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_SUBMIT_INFORM_INFO, SubmitInformInfo.dwInformUserID, dwSocketID, dwTokenID, &SubmitInformInfo, sizeof(SubmitInformInfo));

		return true;
	}
	case SUB_MB_BIND_EXTERNAL_ACCOUNT://���˺�
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_BindExternalAccount));
		if (wDataSize != sizeof(CMD_MB_BindExternalAccount)) return false;

		//������Ϣ
		auto pBindExternalAccount = (CMD_MB_BindExternalAccount*)pData;
		
		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_BindExternalAccount BindExternalAccount;
		ZeroMemory(&BindExternalAccount, sizeof(BindExternalAccount));

		//�ⲿ��Ϣ
		BindExternalAccount.dwUserID = pTokenParameter->dwUserID;
		BindExternalAccount.dwExternalID = pBindExternalAccount->dwExternalID;		
		StringCchCopy(BindExternalAccount.szUniqueID, CountArray(BindExternalAccount.szUniqueID), pBindExternalAccount->szUniqueID);

		//������Ϣ			
		BindExternalAccount.dwClientIP = pTokenParameter->dwClientIP;

		//��չ��Ϣ
		if (wDataSize > sizeof(CMD_MB_BindExternalAccount))
		{
			//��������
			VOID* pDataBuffer = NULL;
			tagDataDescribe DataDescribe;
			CRecvPacketHelper RecvPacket(pBindExternalAccount + 1, wDataSize - sizeof(CMD_MB_BindExternalAccount));

			//��ȡ��Ϣ
			while (true)
			{
				//��ȡ����
				pDataBuffer = RecvPacket.GetData(DataDescribe);
				if (DataDescribe.wDataDescribe == DTP_NULL) break;

				//��ȡ����
				switch (DataDescribe.wDataDescribe)
				{
				case DTP_MB_USERFACE_URL:		//ͷ���ַ
				{
					ASSERT(DataDescribe.wDataSize <= sizeof(BindExternalAccount.szUserFaceUrl));
					if (DataDescribe.wDataSize <= sizeof(BindExternalAccount.szUserFaceUrl))
					{
						BindExternalAccount.szUserFaceUrl[CountArray(BindExternalAccount.szUserFaceUrl) - 1L] = 0;
						StringCchCopy(BindExternalAccount.szUserFaceUrl, CountArray(BindExternalAccount.szUserFaceUrl), (LPCTSTR)pDataBuffer);
					}

					break;
				}
				case DTP_MB_EXTERNAL_DATUM:		//�ⲿ����
				{
					ASSERT(DataDescribe.wDataSize <= sizeof(BindExternalAccount.szUserDatum));
					if (DataDescribe.wDataSize <= sizeof(BindExternalAccount.szUserDatum))
					{
						BindExternalAccount.szUserDatum[CountArray(BindExternalAccount.szUserDatum) - 1L] = 0;
						StringCchCopy(BindExternalAccount.szUserDatum, CountArray(BindExternalAccount.szUserDatum), (LPCTSTR)pDataBuffer);
					}

					break;
				}
				}
			}
		}		

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_BIND_EXTERNAL_ACCOUNT, BindExternalAccount.dwUserID, dwSocketID, dwTokenID, &BindExternalAccount, sizeof(BindExternalAccount));

		return true;
	}
	case SUB_MB_SEND_GIFT:			 //��������
	{
        //Ч�����
        ASSERT(wDataSize == sizeof(CMD_MB_SendGift));
        if (wDataSize != sizeof(CMD_MB_SendGift)) return false;

        //������Ϣ
        auto pSendGift = (CMD_MB_SendGift*)pData;

        //���Ʋ���
        tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
        if (pTokenParameter == NULL) return false;
        if (pTokenParameter->pBindUserItem == NULL) return false;

        //��������
		DBR_MB_SendGift SendGift;
        ZeroMemory(&SendGift, sizeof(SendGift));

		//���ñ���
		SendGift.dwUserID = pTokenParameter->dwUserID;
		SendGift.dwTargetUserID = pSendGift->dwUserID;
		SendGift.dwGoodsID = pSendGift->dwGoodsID;
		SendGift.dwGoodsCount = pSendGift->dwGoodsCount;

        //������Ϣ			
		SendGift.dwClientIP = pTokenParameter->dwClientIP;

        //Ͷ������
        m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_SEND_GIFT, SendGift.dwUserID, dwSocketID, dwTokenID, &SendGift, sizeof(SendGift));

		return true;
	}
	case SUB_MB_LOAD_GIFT:			 //��������
	{
        //Ч�����
        ASSERT(wDataSize == sizeof(CMD_MB_LoadGift));
        if (wDataSize != sizeof(CMD_MB_LoadGift)) return false;

        //������Ϣ
        auto pLoadGift = (CMD_MB_LoadGift*)pData;

        //���Ʋ���
        tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
        if (pTokenParameter == NULL) return false;
        if (pTokenParameter->pBindUserItem == NULL) return false;

        //��������
		DBR_MB_LoadGift LoadGift;
        ZeroMemory(&LoadGift, sizeof(LoadGift));

        //���ñ���
		LoadGift.dwUserID = pTokenParameter->dwUserID;
        //LoadGift.dwRecordID = pLoadGift->dwRecordID;

        //Ͷ������
        m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOAD_GIFT, LoadGift.dwUserID, dwSocketID, dwTokenID, &LoadGift, sizeof(LoadGift));

        return true;
	}
	case SUB_MB_GIFT_STATUS_UPDATE:		//����״̬����
	{
        //Ч�����
        ASSERT(wDataSize == sizeof(CMD_MB_GiftStatusUpdate));
        if (wDataSize != sizeof(CMD_MB_GiftStatusUpdate)) return false;

        //������Ϣ
        auto pGiftStatusUpdate = (CMD_MB_GiftStatusUpdate*)pData;

        //���Ʋ���
        tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
        if (pTokenParameter == NULL) return false;
        if (pTokenParameter->pBindUserItem == NULL) return false;

        //��������
		DBR_MB_GiftStatusUpdata GiftStatusUpdata;
        ZeroMemory(&GiftStatusUpdata, sizeof(GiftStatusUpdata));

        //���ñ���
		GiftStatusUpdata.dwUserID = pTokenParameter->dwUserID;
		GiftStatusUpdata.dwRecordID = pGiftStatusUpdate->dwRecordID;

        //Ͷ������
        m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_GIFT_STATUS_UPDATE, GiftStatusUpdata.dwUserID, dwSocketID, dwTokenID, &GiftStatusUpdata, sizeof(GiftStatusUpdata));

        return true;
	}
	case SUB_MB_QUERY_TURNTABLE_LOTTERY://��ѯת�̳齱����
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_QueryTurntableLottery));
		if (wDataSize != sizeof(CMD_MB_QueryTurntableLottery)) return false;

		//��ȡ����
		auto pQueryTurntableLottery = (CMD_MB_QueryTurntableLottery*)pData;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��ѯ����
		auto TurntableOptionPtr = g_GlobalOptionManager->GetOptionItem<CTurntableOptionPtr>(OPTION_MASK_TURNTABLE);
		if (!TurntableOptionPtr)
		{
			return false;
		}

		//У��ժҪ
		if (lstrcmp(pQueryTurntableLottery->szOptionDigest, TurntableOptionPtr->GetOptionDigest()) != 0)
		{
			//�������
			CMD_MB_TurntableOption TurntableOption;
			ZeroMemory(&TurntableOption,sizeof(TurntableOption));

			//���ñ���
			StringCchCopy(TurntableOption.szOptionDigest, CountArray(TurntableOption.szOptionDigest) ,TurntableOptionPtr->GetOptionDigest());
			StringCchCopy(TurntableOption.szTurantableAward, CountArray(TurntableOption.szTurantableAward), TurntableOptionPtr->GetTurntableAwards());

			//��������		
			WORD wAwardSize = CountStringBuffer(TurntableOption.szTurantableAward);
			WORD wPacketSize = sizeof(TurntableOption) - sizeof(TurntableOption.szTurantableAward) + wAwardSize;
			SendDataToGate(dwSocketID, MDM_MB_USER_SERVICE, SUB_MB_TURNTABLE_OPTION, &TurntableOption, wPacketSize, &dwTokenID, 1);
		}

		//��������
		DBR_MB_QueryTurntableLottery QueryTurntableLottery;
		ZeroMemory(&QueryTurntableLottery, sizeof(QueryTurntableLottery));

		//���ñ���
		QueryTurntableLottery.dwUserID = pTokenParameter->dwUserID;
		QueryTurntableLottery.dwClientIP = pTokenParameter->dwClientIP;
		QueryTurntableLottery.dwRecordID = pQueryTurntableLottery->dwRecordID;
		StringCchCopy(QueryTurntableLottery.szMachineID, CountArray(QueryTurntableLottery.szMachineID), pQueryTurntableLottery->szMachineID);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_QUERY_TURNTABLE_LOTTERY, QueryTurntableLottery.dwUserID, dwSocketID, dwTokenID, &QueryTurntableLottery, sizeof(QueryTurntableLottery));

		return true;
	}
	case SUB_MB_REQUEST_TURNTABLE_LOTTERY:	//����ת�̳齱
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_RequestTurntableLottery));
		if (wDataSize != sizeof(CMD_MB_RequestTurntableLottery)) return false;

		//��ȡ����
		auto pRequestTurntableLottery = (CMD_MB_RequestTurntableLottery*)pData;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��ѯ����
		auto TurntableOptionPtr = g_GlobalOptionManager->GetOptionItem<CTurntableOptionPtr>(OPTION_MASK_TURNTABLE);
		if (!TurntableOptionPtr)
		{
			return false;
		}

		//У��ժҪ
		if (lstrcmp(pRequestTurntableLottery->szOptionDigest, TurntableOptionPtr->GetOptionDigest()) != 0)
		{
			//�������
			CMD_MB_TurntableOption TurntableOption;
			ZeroMemory(&TurntableOption, sizeof(TurntableOption));

			//���ñ���
			StringCchCopy(TurntableOption.szOptionDigest, CountArray(TurntableOption.szOptionDigest), TurntableOptionPtr->GetOptionDigest());
			StringCchCopy(TurntableOption.szTurantableAward, CountArray(TurntableOption.szTurantableAward), TurntableOptionPtr->GetTurntableAwards());

			//��������		
			WORD wAwardSize = CountStringBuffer(TurntableOption.szTurantableAward);
			WORD wPacketSize = sizeof(TurntableOption) - sizeof(TurntableOption.szTurantableAward);
			SendDataToGate(dwSocketID, MDM_MB_USER_SERVICE, SUB_MB_TURNTABLE_OPTION, &TurntableOption, wPacketSize, &dwTokenID, 1);
		}

		//��������
		DBR_MB_RequestTurntableLottery RequestTurntableLottery;
		ZeroMemory(&RequestTurntableLottery, sizeof(RequestTurntableLottery));

		//���ñ���
		RequestTurntableLottery.dwUserID = pTokenParameter->dwUserID;
		RequestTurntableLottery.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(RequestTurntableLottery.szMachineID, CountArray(RequestTurntableLottery.szMachineID), pRequestTurntableLottery->szMachineID);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_REQUEST_TURNTABLE_LOTTERY, RequestTurntableLottery.dwUserID, dwSocketID, dwTokenID, &RequestTurntableLottery, sizeof(RequestTurntableLottery));

		return true;
	}
	case SUB_MB_QUERY_RANKING_LIST:	//��ѯ��
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_QueryRankingList));
		if (wDataSize != sizeof(CMD_MB_QueryRankingList)) return false;

		//��ȡ����
		auto pQueryRankingList = (CMD_MB_QueryRankingList*)pData;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//������
		CMD_MB_RankingListData RankingListData;
		ZeroMemory(&RankingListData,sizeof(RankingListData));

		//������
		RankingListData.cbListType = pQueryRankingList->cbListType;

		//��ѯ��
		auto pRankingListItem = m_RankingListManager.SearchRankingList(RankingListData.cbListType);
		if (pRankingListItem)
		{
			StringCchCopy(RankingListData.szRankingList,CountArray(RankingListData.szRankingList), pRankingListItem->GetRankingList());
		}

		//��������		
		WORD wListSize = CountStringBuffer(RankingListData.szRankingList);
		WORD wPacketSize = sizeof(RankingListData) - sizeof(RankingListData.szRankingList) + wListSize;
		SendDataToGate(dwSocketID, MDM_MB_USER_SERVICE, SUB_MB_RANKING_LIST_DATA, &RankingListData, wPacketSize, &dwTokenID, 1);

		return true;
	}
	case SUB_MB_WEALTH_TRANSFER:	//����ת��
	{
		//Ч�����
		ASSERT (wDataSize == sizeof(CMD_MB_WealthTransfer));
		if (wDataSize != sizeof(CMD_MB_WealthTransfer)) return false;

		//������Ϣ
		auto pWealthTransfer = (CMD_MB_WealthTransfer*)pData;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_WealthTransfer WealthTransfer;
		ZeroMemory(&WealthTransfer, sizeof(WealthTransfer));

		//���ñ���
		WealthTransfer.dwUserID = pTokenParameter->dwUserID;
		WealthTransfer.cbCodeKind = pWealthTransfer->cbCodeKind;
		StringCchCopy(WealthTransfer.szAuthCode,CountArray(WealthTransfer.szAuthCode), pWealthTransfer->szAuthCode);

		//ת����Ϣ
		WealthTransfer.dwTargetGameID = pWealthTransfer->dwTargetGameID;
		WealthTransfer.lTransferAmount = pWealthTransfer->lTransferAmount;

		//������Ϣ
		WealthTransfer.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(WealthTransfer.szMachineID,CountArray(WealthTransfer.szMachineID), pWealthTransfer->szMachineID);
	
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_WEALTH_TRANSFER, WealthTransfer.dwUserID, dwSocketID, dwTokenID, &WealthTransfer, sizeof(WealthTransfer));

		return true;
	}
	case SUB_MB_DRAW_COMMISSION:	//ȡ��Ӷ��
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_DrawCommission));
		if (wDataSize != sizeof(CMD_MB_DrawCommission)) return false;

		//������Ϣ
		auto pDrawCommission = (CMD_MB_DrawCommission*)pData;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_DrawCommission DrawCommission;
		ZeroMemory(&DrawCommission, sizeof(DrawCommission));

		//���ñ���
		DrawCommission.dwUserID = pTokenParameter->dwUserID;
		DrawCommission.lDrawAmount = pDrawCommission->lDarwinAmount;

		//������Ϣ
		DrawCommission.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(DrawCommission.szMachineID, CountArray(DrawCommission.szMachineID), pDrawCommission->szMachineID);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_DRAW_COMMISSION, DrawCommission.dwUserID, dwSocketID, dwTokenID, &DrawCommission, sizeof(DrawCommission));

		return true;
	}
	case SUB_MB_INSURE_QUERY:		//���в�ѯ
	{
		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_InsureQuery InsureQuery;
		ZeroMemory(&InsureQuery, sizeof(InsureQuery));

		//���ñ���
		InsureQuery.dwUserID = pTokenParameter->dwUserID;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_INSURE_QUERY, InsureQuery.dwUserID, dwSocketID, dwTokenID, &InsureQuery, sizeof(InsureQuery));
		return true;
	}
	case SUB_MB_INSURE_ENABLE:		//���п�ͨ
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_InsureEnable));
		if (wDataSize != sizeof(CMD_MB_InsureEnable)) return false;

		//������Ϣ
		auto pInsureEnable = (CMD_MB_InsureEnable*)pData;
		pInsureEnable->szPassword[LEN_PASSWORD - 1] = 0;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_InsureEnable InsureEnable;
		ZeroMemory(&InsureEnable, sizeof(InsureEnable));

		//���ñ���
		InsureEnable.dwUserID = pTokenParameter->dwUserID;
		StringCchCopy(InsureEnable.szPassword, CountArray(InsureEnable.szPassword), pInsureEnable->szPassword);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_INSURE_ENABLE, InsureEnable.dwUserID, dwSocketID, dwTokenID, &InsureEnable, sizeof(InsureEnable));
		return true;
	}
	case SUB_MB_INSURE_SAVE:		//���д洢
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_InsureSave));
		if (wDataSize != sizeof(CMD_MB_InsureSave)) return false;

		//������Ϣ
		auto pInsureSave = (CMD_MB_InsureSave*)pData;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_InsureSave InsureSave;
		ZeroMemory(&InsureSave, sizeof(InsureSave));

		//���ñ���
		InsureSave.dwUserID = pTokenParameter->dwUserID;
		InsureSave.lSaveScore = pInsureSave->lSaveScore;

		//������Ϣ
		InsureSave.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(InsureSave.szMachineID, CountArray(InsureSave.szMachineID), pInsureSave->szMachineID);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_INSURE_SAVE, InsureSave.dwUserID, dwSocketID, dwTokenID, &InsureSave, sizeof(InsureSave));
		return true;
	}
	case SUB_MB_INSURE_TAKE:		//������ȡ
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_InsureTake));
		if (wDataSize != sizeof(CMD_MB_InsureTake)) return false;

		//������Ϣ
		auto pInsureTake = (CMD_MB_InsureTake*)pData;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_InsureTake InsureTake;
		ZeroMemory(&InsureTake, sizeof(InsureTake));

		//���ñ���
		InsureTake.dwUserID = pTokenParameter->dwUserID;
		InsureTake.lTakeScore = pInsureTake->lTakeScore;
		StringCchCopy(InsureTake.szPassword, CountArray(InsureTake.szPassword), pInsureTake->szPassword);

		//������Ϣ
		InsureTake.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(InsureTake.szMachineID, CountArray(InsureTake.szMachineID), pInsureTake->szMachineID);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_INSURE_TAKE, InsureTake.dwUserID, dwSocketID, dwTokenID, &InsureTake, sizeof(InsureTake));
		return true;
	}
	case SUB_MB_INSURE_TRANSFER:		//����ת��
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_InsureTransfer));
		if (wDataSize != sizeof(CMD_MB_InsureTransfer)) return false;

		//������Ϣ
		auto pInsureTransfer = (CMD_MB_InsureTransfer*)pData;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_InsureTransfer InsureTransfer;
		ZeroMemory(&InsureTransfer, sizeof(InsureTransfer));

		//���ñ���
		InsureTransfer.dwUserID = pTokenParameter->dwUserID;
		InsureTransfer.lTransferScore = pInsureTransfer->lTransferScore;
		InsureTransfer.dwGameID = pInsureTransfer->dwGameID;
		StringCchCopy(InsureTransfer.szPassword, CountArray(InsureTransfer.szPassword), pInsureTransfer->szPassword);

		//������Ϣ
		InsureTransfer.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(InsureTransfer.szMachineID, CountArray(InsureTransfer.szMachineID), pInsureTransfer->szMachineID);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_INSURE_TRANSFER, InsureTransfer.dwUserID, dwSocketID, dwTokenID, &InsureTransfer, sizeof(InsureTransfer));
		return true;
	}
	case SUB_MB_INSURE_RESET_PASSWORD:		//������������
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_InsureResetPassword));
		if (wDataSize != sizeof(CMD_MB_InsureResetPassword)) return false;

		//������Ϣ
		auto pInsureReset = (CMD_MB_InsureResetPassword*)pData;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_InsureResetPassword InsureReset;
		ZeroMemory(&InsureReset, sizeof(InsureReset));

		//���ñ���
		InsureReset.dwUserID = pTokenParameter->dwUserID;
		StringCchCopy(InsureReset.szSrcPassword, CountArray(InsureReset.szSrcPassword), pInsureReset->szSrcPassword);
		StringCchCopy(InsureReset.szDstPassword, CountArray(InsureReset.szDstPassword), pInsureReset->szDstPassword);

		//������Ϣ
		InsureReset.dwClientIP = pTokenParameter->dwClientIP;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_INSURE_RESET_PASSWORD, InsureReset.dwUserID, dwSocketID, dwTokenID, &InsureReset, sizeof(InsureReset));
		return true;
	}
	case SUB_MB_REAL_AUTH:		//ʵ����֤
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_RealAuth));
		if (wDataSize != sizeof(CMD_MB_RealAuth)) return false;

		//������Ϣ
		CMD_MB_RealAuth* pRealAuth = (CMD_MB_RealAuth*)pData;
		pRealAuth->szPassword[CountArray(pRealAuth->szPassword) - 1] = 0;
		pRealAuth->szCompellation[CountArray(pRealAuth->szCompellation) - 1] = 0;
		pRealAuth->szPassPortID[CountArray(pRealAuth->szPassPortID) - 1] = 0;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_RealAuth RealAuthent;

		//��������
		RealAuthent.dwUserID = pTokenParameter->dwUserID;
		StringCchCopy(RealAuthent.szPassword, CountArray(RealAuthent.szPassword), pRealAuth->szPassword);
		StringCchCopy(RealAuthent.szCompellation, CountArray(RealAuthent.szCompellation), pRealAuth->szCompellation);
		StringCchCopy(RealAuthent.szPassPortID, CountArray(RealAuthent.szPassPortID), pRealAuth->szPassPortID);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_REAL_AUTH, RealAuthent.dwUserID, dwSocketID, dwTokenID, &RealAuthent, sizeof(RealAuthent));
		return true;
	}
	case SUB_MB_QUERY_SUBSIDY:	//��ѯ�ͱ�
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_MB_SubsidyQuery));
		if (wDataSize != sizeof(CMD_MB_SubsidyQuery)) return false;

		//������Ϣ
		auto pSubsidy = (CMD_MB_SubsidyQuery*)pData;

		//���Ʋ���
		tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return false;
		if (pTokenParameter->pBindUserItem == NULL) return false;

		//��������
		DBR_MB_SubsidyQuery Subsidy;
		ZeroMemory(&Subsidy, sizeof(Subsidy));

		//���ñ���
		Subsidy.dwUserID = pTokenParameter->dwUserID;
		Subsidy.dwClientIP = pTokenParameter->dwClientIP;
		StringCchCopy(Subsidy.szMachineID, CountArray(Subsidy.szMachineID), pSubsidy->szMachineID);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_QUERY_SUBSIDY, Subsidy.dwUserID, dwSocketID, dwTokenID, &Subsidy, sizeof(Subsidy));

		return true;
	}
	}

	return false;
}

//�������
bool CAttemperEngineSink::OnTCPNetworkMainPCManagerService(WORD wSubCmdID, VOID *pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_MODIFY_OPTION:			//�޸�ѡ��
	{
		auto pModifyOption = (CMD_GP_ModifyOption*)pData;

		//����ṹ
		CMD_CS_C_ModifyOption ModifyOption = {};
		
		//���ñ���
		ModifyOption.ModifyOption = pModifyOption->ModifyOption;
		ModifyOption.wServerCount = pModifyOption->wServerCount;
		CopyMemory(ModifyOption.wServerIDList, pModifyOption->wServerIDList, ModifyOption.wServerCount*sizeof(ModifyOption.wServerIDList[0]));

		//��������
		WORD wSendSize = sizeof(ModifyOption) - sizeof(ModifyOption.wServerIDList);
		wSendSize += ModifyOption.wServerCount * sizeof(ModifyOption.wServerIDList[0]);
		m_pITCPSocketCenter->SendData(MDM_CS_MANAGER_SERVICE, SUB_CS_C_MODIFY_OPTION, pData, wDataSize, dwTokenID);
		break;
	}
	case SUB_GP_EXECUTE_COMMAND:		//ִ������
	{
		//��������
		m_pITCPSocketCenter->SendData(MDM_CS_MANAGER_SERVICE, SUB_CS_C_EXECUTE_COMMAND, pData, wDataSize, dwTokenID);
		break;
	}
	}		

	//�����ɹ�
	SendDataToGate(dwSocketID, MDM_GP_MANAGER_SERVICE, SUB_GP_MANAGE_SUCCESS,&dwTokenID,1);

	return true;
}

//���÷���
bool CAttemperEngineSink::OnTCPNetworkMainPCConfigService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_QUERY_GAME_CONFIG:		//��ѯ����
	{
		//����У��
		ASSERT(wDataSize == sizeof(CMD_GP_QueryGameConfig));
		if (wDataSize != sizeof(CMD_GP_QueryGameConfig)) return false;

		//��ȡ����
		auto pQueryGameConfig = (CMD_GP_QueryGameConfig*)pData;
		ASSERT(pQueryGameConfig != NULL);

		//����ṹ
		DBR_GP_QueryGameConfig QueryGameConfig = {};
		QueryGameConfig.wKindID = pQueryGameConfig->wKindID;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_GAME_CONFIG, 0, dwSocketID, dwTokenID, &QueryGameConfig, sizeof(QueryGameConfig));

		return true;
	}
	case SUB_GP_UPDATE_BASIC_CONFIG:		//��������
	{
		//����У��
		ASSERT (wDataSize <= sizeof(CMD_GP_UpdateBasicConfig));
		if (wDataSize > sizeof(CMD_GP_UpdateBasicConfig)) return false;

		//��ȡ����
		auto pUpdateBasicConfig = (CMD_GP_UpdateBasicConfig*)pData;
		ASSERT(pUpdateBasicConfig != NULL);

		//����ṹ
		DBR_GP_UpdateBasicConfig UpdateBasicConfig = {};
		UpdateBasicConfig.wKindID = pUpdateBasicConfig->wKindID;
		StringCchCopy(UpdateBasicConfig.szBasicConfig,CountArray(UpdateBasicConfig.szBasicConfig), pUpdateBasicConfig->szBasicConfig);
		
		//Ͷ������
		WORD wSendSize = CountStringBuffer(UpdateBasicConfig.szBasicConfig);
		wSendSize += sizeof(UpdateBasicConfig) - sizeof(UpdateBasicConfig.szBasicConfig);
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_BASIC_CONFIG, 0, dwSocketID, dwTokenID, &UpdateBasicConfig, wSendSize);

		return true;
	}
	case SUB_GP_UPDATE_WEIGHT_CONFIG:		//��������
	{
		//����У��
		ASSERT(wDataSize <= sizeof(CMD_GP_UpdateWeightConfig));
		if (wDataSize > sizeof(CMD_GP_UpdateWeightConfig)) return false;

		//��ȡ����
		auto pUpdateWeightConfig = (CMD_GP_UpdateWeightConfig*)pData;
		ASSERT(pUpdateWeightConfig != NULL);

		//����ṹ
		DBR_GP_UpdateWeightConfig UpdateWeightConfig;
		UpdateWeightConfig.wKindID = pUpdateWeightConfig->wKindID;
		UpdateWeightConfig.cbTypeID = pUpdateWeightConfig->cbTypeID;
		UpdateWeightConfig.wItemCount = pUpdateWeightConfig->wItemCount;
		CopyMemory(&UpdateWeightConfig.TimesWeightItem, &pUpdateWeightConfig->TimesWeightItem, sizeof(UpdateWeightConfig.TimesWeightItem));

		//Ͷ������
		WORD wSendSize = sizeof(UpdateWeightConfig) - sizeof(UpdateWeightConfig.TimesWeightItem);
		wSendSize += UpdateWeightConfig.wItemCount * sizeof(UpdateWeightConfig.TimesWeightItem[0]);
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_WEIGHT_CONFIG, 0, dwSocketID, dwTokenID, &UpdateWeightConfig, wSendSize);

		return true;
	}	
	}

	return false;
}

//��������
bool CAttemperEngineSink::OnTCPNetworkMainPCRobotService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_QUERY_PARAMETER:		//��ȡ����
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_QueryParameter));
			if(wDataSize!=sizeof(CMD_GP_QueryParameter)) return false;

			//��ȡ����
			CMD_GP_QueryParameter * pQueryParameter = (CMD_GP_QueryParameter *)pData;
			ASSERT(pQueryParameter!=NULL);

			//����ṹ
			DBR_GP_QueryParameter QueryParameter;
			QueryParameter.wServerID = pQueryParameter->wServerID;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_PARAMETER,0,dwSocketID,dwTokenID,&QueryParameter,sizeof(QueryParameter));

			return true;
		}
	case SUB_GP_APPEND_PARAMETER:		//��Ӳ���
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_AppendParameter));
			if(wDataSize!=sizeof(CMD_GP_AppendParameter)) return false;

			//��ȡ����
			CMD_GP_AppendParameter * pAppendParameter = (CMD_GP_AppendParameter *)pData;
			ASSERT(pAppendParameter!=NULL);

			//����ṹ
			DBR_GP_AppendParameter AppendParameter;
			AppendParameter.wServerID = pAppendParameter->wServerID;
			CopyMemory(&AppendParameter.RobotParameter,&pAppendParameter->RobotParameter,sizeof(tagRobotParameter));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_APPEND_PARAMETER,0,dwSocketID,dwTokenID,&AppendParameter,sizeof(AppendParameter));

			return true;
		}
	case SUB_GP_MODIFY_PARAMETER:	//�޸Ĳ���
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_ModifyParameter));
			if(wDataSize!=sizeof(CMD_GP_ModifyParameter)) return false;

			//��ȡ����
			CMD_GP_ModifyParameter * pModifyParameter = (CMD_GP_ModifyParameter *)pData;
			ASSERT(pModifyParameter!=NULL);

			//����ṹ
			DBR_GP_ModifyParameter ModifyParameter;
			ModifyParameter.wServerID=pModifyParameter->wServerID;
			CopyMemory(&ModifyParameter.RobotParameter,&pModifyParameter->RobotParameter,sizeof(tagRobotParameter));

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_PARAMETER,0,dwSocketID,dwTokenID,&ModifyParameter,sizeof(ModifyParameter));
			
			return true;
		}
	case SUB_GP_DELETE_PARAMETER:	//ɾ������
		{
			//����У��
			ASSERT(wDataSize==sizeof(CMD_GP_DeleteParameter));
			if(wDataSize!=sizeof(CMD_GP_DeleteParameter)) return false;

			//��ȡ����
			CMD_GP_DeleteParameter * pDeleteParameter = (CMD_GP_DeleteParameter *)pData;
			ASSERT(pDeleteParameter!=NULL);

			//����ṹ
			DBR_GP_DeleteParameter DeleteParameter;
			DeleteParameter.wServerID = pDeleteParameter->wServerID;
			DeleteParameter.dwBatchID = pDeleteParameter->dwBatchID;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_DELETE_PARAMETER,0,dwSocketID,dwTokenID,&DeleteParameter,sizeof(DeleteParameter));
			
			return true;
		}
	}

	return false;
}

//������
bool CAttemperEngineSink::OnTCPNetworkMainPCStockService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GP_QUERY_STOCK_INFO:		//��ѯ���
	{
		//����У��
		ASSERT(wDataSize == sizeof(CMD_GP_QueryStockInfo));
		if (wDataSize != sizeof(CMD_GP_QueryStockInfo)) return false;

		//��ȡ����
		auto pQueryStockInfo = (CMD_GP_QueryStockInfo*)pData;
		
		//����ṹ
		DBR_GP_QueryStockInfo QueryStockInfo = {};
		StringCchCopy(QueryStockInfo.szKindID,CountArray(QueryStockInfo.szKindID), pQueryStockInfo->szKindID);
		
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_QUERY_STOCK_INFO, 0, dwSocketID, dwTokenID, &QueryStockInfo, sizeof(QueryStockInfo));

		return true;
	}
	case SUB_GP_APPEND_STOCK_INFO:		//��ӿ��
	{
		//����У��
		ASSERT(wDataSize == sizeof(CMD_GP_AppendStockInfo));
		if (wDataSize != sizeof(CMD_GP_AppendStockInfo)) return false;

		//��ȡ����
		auto pAppendStockInfo = (CMD_GP_AppendStockInfo*)pData;

		//����ṹ
		DBR_GP_AppendStockInfo AppendStockInfo = {};
		CopyMemory(&AppendStockInfo.StockInfo, &pAppendStockInfo->StockInfo, sizeof(AppendStockInfo.StockInfo));

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_APPEND_STOCK_INFO, 0, dwSocketID, dwTokenID, &AppendStockInfo, sizeof(AppendStockInfo));

		return true;
	}
	case SUB_GP_MODIFY_STOCK_INFO:		//�޸Ŀ��
	{
		//����У��
		ASSERT(wDataSize == sizeof(CMD_GP_ModifyStockInfo));
		if (wDataSize != sizeof(CMD_GP_ModifyStockInfo)) return false;

		//��ȡ����
		auto pModifyStockInfo = (CMD_GP_ModifyStockInfo*)pData;

		//����ṹ
		DBR_GP_ModifyStockInfo ModifyStockInfo = {};
		CopyMemory(&ModifyStockInfo.StockInfo, &pModifyStockInfo->StockInfo, sizeof(ModifyStockInfo.StockInfo));

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_MODIFY_STOCK_INFO, 0, dwSocketID, dwTokenID, &ModifyStockInfo, sizeof(ModifyStockInfo));

		return true;
	}
	case SUB_GP_DELETE_STOCK_INFO:		//ɾ�����
	{
		//����У��
		ASSERT(wDataSize == sizeof(CMD_GP_DeleteStockInfo));
		if (wDataSize != sizeof(CMD_GP_DeleteStockInfo)) return false;

		//��ȡ����
		auto pDeleteStockInfo = (CMD_GP_DeleteStockInfo*)pData;

		//����ṹ
		DBR_GP_DeleteStockInfo DeleteStockInfo = {};
		DeleteStockInfo.wStockID = pDeleteStockInfo->wStockID;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_DELETE_STOCK_INFO, 0, dwSocketID, dwTokenID, &DeleteStockInfo, sizeof(DeleteStockInfo));

		return true;
	}
	case SUB_GP_ADJUST_STOCK_SCORE:		//�������
	{
		//����У��
		ASSERT(wDataSize == sizeof(CMD_GP_AdjustStockScore));
		if (wDataSize != sizeof(CMD_GP_AdjustStockScore)) return false;

		//��ȡ����
		auto pAdjustStockScore = (CMD_GP_AdjustStockScore*)pData;

		//���嶨��
		WORD wBufferSize = 0;
		BYTE cbBuffer[512] = { 0 };

		//����ṹ
		tagForwardHead* pForwadHead = (tagForwardHead*)cbBuffer;

		//ת������
		pForwadHead->wItemCount = 1;
		pForwadHead->wTarget = FORWARD_TARGET_SERVICE;
		pForwadHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MONITOR;

		//����ṹ
		auto pFWAdjustStockScore = (CMD_CS_AdjustStockScore*)(pForwadHead + 1);

		//���ñ���
		pFWAdjustStockScore->wStockID = pAdjustStockScore->wStockID;
		pFWAdjustStockScore->lChangeScore = pAdjustStockScore->lChangeScore;

		//��������
		wBufferSize = sizeof(tagForwardHead) + sizeof(CMD_CS_AdjustStockScore);
		m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_ADJUST_STOCK_SCORE, cbBuffer, wBufferSize, dwTokenID);		

		//��������
		CMD_GP_StockOperateResult StockOperateResult;
		ZeroMemory(&StockOperateResult, sizeof(StockOperateResult));

		//�������
		StockOperateResult.wSubCommdID = SUB_GP_ADJUST_STOCK_SCORE;		

		//�����С
		WORD wPacketSize = sizeof(StockOperateResult) - sizeof(StockOperateResult.StockList);
		wPacketSize += sizeof(StockOperateResult.StockList[0]) * StockOperateResult.wStockCount;

		//��������
		SendDataToGate(dwSocketID, MDM_GP_STOCK_SERVICE, SUB_GP_STOCK_OPERATE_RESULT, &StockOperateResult, wPacketSize, &dwTokenID, 1);

		return true;
	}
	case SUB_GP_ADJUST_JACKPOT_SCORE:	//�����ʽ�
	{
		//����У��
		ASSERT(wDataSize == sizeof(CMD_GP_AdjustJackpotScore));
		if (wDataSize != sizeof(CMD_GP_AdjustJackpotScore)) return false;

		//��ȡ����
		auto pAdjustJackpotScore = (CMD_GP_AdjustJackpotScore*)pData;

		//���嶨��
		WORD wBufferSize = 0;
		BYTE cbBuffer[512] = { 0 };

		//����ṹ
		tagForwardHead* pForwadHead = (tagForwardHead*)cbBuffer;

		//ת������
		pForwadHead->wItemCount = 1;
		pForwadHead->wTarget = FORWARD_TARGET_SERVICE;
		pForwadHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MONITOR;

		//����ṹ
		auto pFWAdjustJackpotScore = (CMD_CS_AdjustJackpotScore*)(pForwadHead + 1);

		//���ñ���
		pFWAdjustJackpotScore->wStockID = pAdjustJackpotScore->wStockID;
		pFWAdjustJackpotScore->wLevelID = pAdjustJackpotScore->wLevelID;
		pFWAdjustJackpotScore->lChangeScore = pAdjustJackpotScore->lChangeScore;

		//��������
		wBufferSize = sizeof(tagForwardHead) + sizeof(CMD_CS_AdjustJackpotScore);
		m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_ADJUST_JACKPOT_SCORE, cbBuffer, wBufferSize, dwTokenID);

		//��������
		CMD_GP_StockOperateResult StockOperateResult;
		ZeroMemory(&StockOperateResult, sizeof(StockOperateResult));

		//�������
		StockOperateResult.wSubCommdID = SUB_GP_ADJUST_JACKPOT_SCORE;

		//�����С
		WORD wPacketSize = sizeof(StockOperateResult) - sizeof(StockOperateResult.StockList);
		wPacketSize += sizeof(StockOperateResult.StockList[0]) * StockOperateResult.wStockCount;

		//��������
		SendDataToGate(dwSocketID, MDM_GP_STOCK_SERVICE, SUB_GP_STOCK_OPERATE_RESULT, &StockOperateResult, wPacketSize, &dwTokenID, 1);

		return true;
	}
	}

	return false;
}

//�ο͵�¼
bool CAttemperEngineSink::OnTCPNetworkSubPCLogonByGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID,DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GP_LogonByGameID));
	if (wDataSize<sizeof(CMD_GP_LogonByGameID)) return false;

	//������Ϣ
	CMD_GP_LogonByGameID * pLogonByGameID=(CMD_GP_LogonByGameID *)pData;
	pLogonByGameID->szPassword[CountArray(pLogonByGameID->szPassword)-1]=0;

	//���Ʋ���
	tagTokenParameter * pTokenParameter=GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//��������
	pExtendParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pExtendParameter->dwPlazaVersion=pLogonByGameID->dwPlazaVersion;

	//Ч��汾
	if (CheckPlazaVersion(DEVICE_KIND_PC,pLogonByGameID->dwPlazaVersion,dwSocketID,dwTokenID)==false) return true;

	//��������
	DBR_GP_LogonByGameID LogonByGameID;
	ZeroMemory(&LogonByGameID,sizeof(LogonByGameID));

	//������Ϣ
	LogonByGameID.dwPostTime=GetTickCount();
	LogonByGameID.pTokenParameter= pTokenParameter;

	//��������
	LogonByGameID.dwGameID=pLogonByGameID->dwGameID;
	LogonByGameID.dwClientIP=pTokenParameter->dwClientIP;
	StringCchCopy(LogonByGameID.szPassword, CountArray(LogonByGameID.szPassword), pLogonByGameID->szPassword);

	//��������
	VOID * pDataBuffer=NULL;
	tagDataDescribe DataDescribe;
	CRecvPacketHelper RecvPacket(pLogonByGameID+1,wDataSize-sizeof(CMD_GP_LogonByGameID));

	//��չ��Ϣ
	while (true)
	{
		//��ȡ����
		pDataBuffer=RecvPacket.GetData(DataDescribe);
		if (DataDescribe.wDataDescribe==DTP_NULL) break;

		//���ݷ���
		switch (DataDescribe.wDataDescribe)
		{
		case DTP_GP_MACHINE_ID:	//������ʶ
			{
				ASSERT(pDataBuffer!=NULL);
				ASSERT(DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR));
				if (DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR))
				{
					StringCchCopy( LogonByGameID.szMachineID,CountArray(LogonByGameID.szMachineID), (WCHAR*)pDataBuffer);
				}

				break;
			}
		}
	}

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_GAMEID, LogonByGameID.dwGameID,dwSocketID,dwTokenID,&LogonByGameID,sizeof(LogonByGameID));

	return true;
}

//�ʺŵ�¼
bool CAttemperEngineSink::OnTCPNetworkSubPCLogonByAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID,DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GP_LogonByAccounts));
	if (wDataSize<sizeof(CMD_GP_LogonByAccounts)) return false;

	//������Ϣ
	CMD_GP_LogonByAccounts * pLogonByAccounts=(CMD_GP_LogonByAccounts *)pData;
	pLogonByAccounts->szAccounts[CountArray(pLogonByAccounts->szAccounts)-1]=0;
	pLogonByAccounts->szPassword[CountArray(pLogonByAccounts->szPassword)-1]=0;

	//���ð�
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//��ȡ����
	tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//��������
	pExtendParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pExtendParameter->dwPlazaVersion=pLogonByAccounts->dwPlazaVersion;
	
	//�汾�ж�
	if (CheckPlazaVersion(DEVICE_KIND_PC,pLogonByAccounts->dwPlazaVersion,dwSocketID,dwTokenID)==false) return true;

	//��������
	DBR_GP_LogonByAccounts LogonByAccounts;
	ZeroMemory(&LogonByAccounts,sizeof(LogonByAccounts));

	//������Ϣ
	LogonByAccounts.dwPostTime=GetTickCount();
	LogonByAccounts.pTokenParameter = pTokenParameter;
	LogonByAccounts.wGateID = pBindParameter->wServiceID;
	LogonByAccounts.wLogonID = m_pServiceOption->wServiceID;

	//��������
	LogonByAccounts.dwCheckUserRight=0;
	LogonByAccounts.szNewComputerID[0]=0;			
	LogonByAccounts.dwLogonServerID = m_pServiceOption->wServiceID;
	LogonByAccounts.dwPlazaVersion=pLogonByAccounts->dwPlazaVersion;	
	LogonByAccounts.dwClientIP= pTokenParameter->dwClientIP;
	StringCchCopy(LogonByAccounts.szAccounts,CountArray(LogonByAccounts.szAccounts), pLogonByAccounts->szAccounts);
	StringCchCopy(LogonByAccounts.szPassword,CountArray(LogonByAccounts.szPassword), pLogonByAccounts->szPassword);

	//��չ��Ϣ
	VOID * pDataBuffer=NULL;
	tagDataDescribe DataDescribe;
	CRecvPacketHelper RecvPacket(pLogonByAccounts+1,wDataSize-sizeof(CMD_GP_LogonByAccounts));
	while (true)
	{
		//��ȡ����
		pDataBuffer=RecvPacket.GetData(DataDescribe);
		if (DataDescribe.wDataDescribe==DTP_NULL) break;

		//���ݷ���
		switch (DataDescribe.wDataDescribe)
		{
		case DTP_GP_MACHINE_ID:	//������ʶ
			{
				ASSERT(pDataBuffer!=NULL);
				ASSERT(DataDescribe.wDataSize==LEN_MACHINE_ID*sizeof(WCHAR));
				if (DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR))
				{
					StringCchCopy( LogonByAccounts.szMachineID, CountArray(LogonByAccounts.szMachineID), (WCHAR*)pDataBuffer);
				}

				break;
			}
		}
	}

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_ACCOUNTS,0,dwSocketID,dwTokenID,&LogonByAccounts,sizeof(LogonByAccounts));

	return true;
}

//���ߵ�¼
bool CAttemperEngineSink::OnTCPNetworkSubPCLogonManageTool(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GP_LogonByAccounts));
	if (wDataSize<sizeof(CMD_GP_LogonByAccounts)) return false;

	//������Ϣ
	CMD_GP_LogonByAccounts * pLogonByAccounts=(CMD_GP_LogonByAccounts *)pData;
	pLogonByAccounts->szAccounts[CountArray(pLogonByAccounts->szAccounts)-1]=0;
	pLogonByAccounts->szPassword[CountArray(pLogonByAccounts->szPassword)-1]=0;

	//���ð�
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//��ȡ����
	tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//��������
	pExtendParameter->cbClientKind = CLIENT_KIND_COMPUTER;
	pExtendParameter->dwPlazaVersion = pLogonByAccounts->dwPlazaVersion;

	//�汾�ж�
	if (CheckPlazaVersion(DEVICE_KIND_PC,pLogonByAccounts->dwPlazaVersion,dwSocketID,dwTokenID)==false) return true;

	//��������
	DBR_GP_LogonByAccounts LogonByAccounts;
	ZeroMemory(&LogonByAccounts,sizeof(LogonByAccounts));

	//������Ϣ
	LogonByAccounts.dwPostTime=GetTickCount();
	LogonByAccounts.pTokenParameter = pTokenParameter;
	LogonByAccounts.wGateID = pBindParameter->wServiceID;
	LogonByAccounts.wLogonID = m_pServiceOption->wServiceID;

	//��������
	LogonByAccounts.szNewComputerID[0]=0;	
	LogonByAccounts.dwLogonServerID = m_pServiceOption->wServiceID;
	LogonByAccounts.dwPlazaVersion=pLogonByAccounts->dwPlazaVersion;
	LogonByAccounts.dwCheckUserRight=UR_CAN_MANAGER_ANDROID;
	LogonByAccounts.dwClientIP= pTokenParameter->dwClientIP;
	StringCchCopy(LogonByAccounts.szAccounts, CountArray(LogonByAccounts.szAccounts), pLogonByAccounts->szAccounts);
	StringCchCopy(LogonByAccounts.szPassword, CountArray(LogonByAccounts.szPassword), pLogonByAccounts->szPassword);

	//��չ��Ϣ
	VOID * pDataBuffer=NULL;
	tagDataDescribe DataDescribe;
	CRecvPacketHelper RecvPacket(pLogonByAccounts+1,wDataSize-sizeof(CMD_GP_LogonByAccounts));
	while (true)
	{
		//��ȡ����
		pDataBuffer=RecvPacket.GetData(DataDescribe);
		if (DataDescribe.wDataDescribe==DTP_NULL) break;

		//���ݷ���
		switch (DataDescribe.wDataDescribe)
		{
		case DTP_GP_MACHINE_ID:	//������ʶ
			{
				ASSERT(pDataBuffer!=NULL);
				ASSERT(DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR));
				if (DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR))
				{
					StringCchCopy( LogonByAccounts.szMachineID,CountArray(LogonByAccounts.szMachineID), (WCHAR*)pDataBuffer);
				}

				break;
			}
		}
	}

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOGON_ACCOUNTS,0,dwSocketID,dwTokenID,&LogonByAccounts,sizeof(LogonByAccounts));

	return true;
}

//�ʺ�ע��
bool CAttemperEngineSink::OnTCPNetworkSubPCRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_GP_RegisterAccounts));
	if (wDataSize<sizeof(CMD_GP_RegisterAccounts)) return false;

	//������Ϣ
	CMD_GP_RegisterAccounts * pRegisterAccounts=(CMD_GP_RegisterAccounts *)pData;
	pRegisterAccounts->szAccounts[CountArray(pRegisterAccounts->szAccounts)-1]=0;
	pRegisterAccounts->szPassword[CountArray(pRegisterAccounts->szPassword)-1]=0;
	pRegisterAccounts->szBankPass[CountArray(pRegisterAccounts->szBankPass)-1]=0;

	//��ȡ����
	tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//��������
	pExtendParameter->cbClientKind=CLIENT_KIND_COMPUTER;
	pExtendParameter->dwPlazaVersion=pRegisterAccounts->dwPlazaVersion;

	//Ч��汾
	if (CheckPlazaVersion(DEVICE_KIND_PC,pRegisterAccounts->dwPlazaVersion,dwSocketID,dwTokenID)==false) return true;

	//��������
	DBR_GP_RegisterAccounts RegisterAccounts;
	ZeroMemory(&RegisterAccounts,sizeof(RegisterAccounts));

	//������Ϣ
	RegisterAccounts.dwPostTime=GetTickCount();
	RegisterAccounts.pTokenParameter = pTokenParameter;

	//��������
	RegisterAccounts.wFaceID=pRegisterAccounts->wFaceID;
	RegisterAccounts.dwClientIP = pTokenParameter->dwClientIP;
	RegisterAccounts.dwPlazaVersion=pRegisterAccounts->dwPlazaVersion;
	StringCchCopy(RegisterAccounts.szAccounts,CountArray(RegisterAccounts.szAccounts), pRegisterAccounts->szAccounts);
	StringCchCopy(RegisterAccounts.szNickName,CountArray(RegisterAccounts.szAccounts), pRegisterAccounts->szAccounts);
	StringCchCopy(RegisterAccounts.szPassword,CountArray(RegisterAccounts.szPassword), pRegisterAccounts->szPassword);
	StringCchCopy(RegisterAccounts.szBankPass,CountArray(RegisterAccounts.szBankPass), pRegisterAccounts->szBankPass);
	RegisterAccounts.dwLogonServerID=m_pServiceOption->wServiceID;
	RegisterAccounts.szValidComputerID[0]=0;

	//��չ��Ϣ
	VOID * pDataBuffer=NULL;
	tagDataDescribe DataDescribe;
	CRecvPacketHelper RecvPacket(pRegisterAccounts+1,wDataSize-sizeof(CMD_GP_RegisterAccounts));
	while (true)
	{
		//��ȡ����
		pDataBuffer=RecvPacket.GetData(DataDescribe);
		if (DataDescribe.wDataDescribe==DTP_NULL) break;

		//���ݷ���
		switch (DataDescribe.wDataDescribe)
		{
		case DTP_GP_USER_INFO:		//�û���Ϣ
			{
				ASSERT(DataDescribe.wDataSize==sizeof(DTP_GP_UserInfo));
				if (DataDescribe.wDataSize==sizeof(DTP_GP_UserInfo))
				{
					//��������
					DTP_GP_UserInfo * pUserInfo=(DTP_GP_UserInfo *)pDataBuffer;
					pUserInfo->szArea[CountArray(pUserInfo->szArea)-1]=0;
					pUserInfo->szCity[CountArray(pUserInfo->szCity)-1]=0;
					pUserInfo->szProvince[CountArray(pUserInfo->szProvince)-1]=0;

					//��������
					RegisterAccounts.cbAge=pUserInfo->cbAge;
					RegisterAccounts.cbGender=pUserInfo->cbGender;
					StringCchCopy(RegisterAccounts.szArea,CountArray(RegisterAccounts.szArea), pUserInfo->szArea);
					StringCchCopy(RegisterAccounts.szCity,CountArray(RegisterAccounts.szCity), pUserInfo->szCity);
					StringCchCopy(RegisterAccounts.szProvince,CountArray(RegisterAccounts.szProvince), pUserInfo->szProvince);
				}
				break;
			}
		case DTP_GP_MACHINE_ID:	//������ʶ
			{
				ASSERT(pDataBuffer!=NULL);
				ASSERT(DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR));
				if (DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR))
				{
					StringCchCopy( RegisterAccounts.szMachineID,CountArray(RegisterAccounts.szMachineID), (WCHAR*)pDataBuffer);
				}

				break;
			}
		case DTP_GP_PASSPORT:		//�����Ϣ
			{
				ASSERT(pDataBuffer);

				//��������
				DTP_GP_Passport *pPassport = (DTP_GP_Passport *)pDataBuffer;

				ASSERT(DataDescribe.wDataSize == sizeof(DTP_GP_Passport)-sizeof(pPassport->szPassportID)+
					CountStringBuffer(pPassport->szPassportID));
				if( DataDescribe.wDataSize == sizeof(DTP_GP_Passport)-sizeof(pPassport->szPassportID)+
					CountStringBuffer(pPassport->szPassportID) )
				{
					RegisterAccounts.cbPassportType = pPassport->cbPassportType;
					StringCchCopy( RegisterAccounts.szPassportID,CountArray(RegisterAccounts.szPassportID), pPassport->szPassportID);
				}

				break;
			}
		case DTP_GP_SPREADER:		//�Ƽ����
			{
				ASSERT(pDataBuffer);

				StringCchCopy( RegisterAccounts.szSpreader,CountArray(RegisterAccounts.szSpreader), (WCHAR*)pDataBuffer);

				break;
			}
		case DTP_GP_NICKNAME:		//�û��ǳ�
			{
				ASSERT(pDataBuffer);

				StringCchCopy(RegisterAccounts.szNickName,CountArray(RegisterAccounts.szNickName), (WCHAR*)pDataBuffer);
				break;
			}
		case DTP_GP_VERIFY_COMPUTER_ID:	//У�����
			{
				ASSERT(pDataBuffer!=NULL);
				ASSERT(DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR));
				if (DataDescribe.wDataSize== LEN_MACHINE_ID *sizeof(WCHAR))
				{
					StringCchCopy(RegisterAccounts.szValidComputerID,CountArray(RegisterAccounts.szValidComputerID), (WCHAR*)pDataBuffer);
				}
			}
		}
	}

	//������ȫ
	if(RegisterAccounts.szValidComputerID[0]==0)
	{
		StringCchCopy(RegisterAccounts.szValidComputerID,CountArray(RegisterAccounts.szValidComputerID), (WCHAR*)RegisterAccounts.szMachineID);
	}

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_REGISTER_ACCOUNTS,0,dwSocketID,dwTokenID,&RegisterAccounts,sizeof(RegisterAccounts));

	return true;
}

//�ο͵�¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonVisitor(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT (wDataSize>=sizeof(CMD_MB_LogonVisitor));
	if (wDataSize<sizeof(CMD_MB_LogonVisitor)) return false;

	//���ð�
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//��ȡ����
	tagTokenParameter * pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//������Ϣ
	CMD_MB_LogonVisitor* pLogonVisitor =(CMD_MB_LogonVisitor*)pData;
	pLogonVisitor->szMachineID[CountArray(pLogonVisitor->szMachineID)-1]=0;
	pLogonVisitor->szMachineName[CountArray(pLogonVisitor->szMachineName)-1]=0;	
	
	//��������
	pExtendParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID= pLogonVisitor->wModuleID;
	pExtendParameter->dwDeviceKind= pLogonVisitor->dwDeviceKind;
	pExtendParameter->dwAppVersion= pLogonVisitor->dwAppVersion;
	pExtendParameter->dwPlazaVersion= pLogonVisitor->dwPlazaVersion;

	//Ч��汾
	if (CheckPlazaVersion(pLogonVisitor->dwDeviceKind&DEVICE_MASK_MB, pLogonVisitor->dwPlazaVersion,dwSocketID,dwTokenID)==false) return true;

	//��������
	DBR_MB_LogonVisitor LogonVisitor;
	ZeroMemory(&LogonVisitor,sizeof(LogonVisitor));

	//������Ϣ
	LogonVisitor.pTokenParameter = pTokenParameter;
	LogonVisitor.wGateID = pBindParameter->wServiceID;
	LogonVisitor.wLogonID = m_pServiceOption->wServiceID;	

	//��������	
	LogonVisitor.wMarketID= pLogonVisitor->wMarketID;
	LogonVisitor.dwClientIP=pTokenParameter->dwClientIP;
	LogonVisitor.dwPlazaVersion= pLogonVisitor->dwAppVersion;
    LogonVisitor.dwDeviceKind = pLogonVisitor->dwDeviceKind;
    LogonVisitor.dwSpreaderID = pLogonVisitor->dwSpreaderID;
	StringCchCopy(LogonVisitor.szMachineID, CountArray(LogonVisitor.szMachineID), pLogonVisitor->szMachineID);
	StringCchCopy(LogonVisitor.szMachineName, CountArray(LogonVisitor.szMachineName), pLogonVisitor->szMachineName);
	StringCchCopy(LogonVisitor.szMachineOSID, CountArray(LogonVisitor.szMachineOSID), pLogonVisitor->szMachineOSID);

	//��չ��Ϣ
	if (wDataSize > sizeof(CMD_MB_LogonVisitor))
	{
		//��������
		VOID* pDataBuffer = NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pLogonVisitor + 1, wDataSize - sizeof(CMD_MB_LogonVisitor));

		//��ȡ��Ϣ
		while (true)
		{
			//��ȡ����
			pDataBuffer = RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe == DTP_NULL) break;

			//��ȡ����
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_MB_PACKAGE_NAME:		//Ӧ�ð���
			{
				ASSERT(DataDescribe.wDataSize <= sizeof(LogonVisitor.szPackageName));
				if (DataDescribe.wDataSize <= sizeof(LogonVisitor.szPackageName))
				{
					LogonVisitor.szPackageName[CountArray(LogonVisitor.szPackageName) - 1L] = 0;
					StringCchCopy(LogonVisitor.szPackageName, CountArray(LogonVisitor.szPackageName), (LPCTSTR)pDataBuffer);
				}

				break;
			}
			}
		}
	}

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_VISITOR, dwTokenID,dwSocketID,dwTokenID,&LogonVisitor,sizeof(LogonVisitor));

	return true;
}

//��Կ��¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonSecretKey(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize >= sizeof(CMD_MB_LogonSecretKey));
	if (wDataSize < sizeof(CMD_MB_LogonSecretKey)) return false;

	//�󶨲���
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//��ȡ����
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//������Ϣ
	CMD_MB_LogonSecretKey* pLogonSecretKey = (CMD_MB_LogonSecretKey*)pData;
	pLogonSecretKey->szSecretKey[CountArray(pLogonSecretKey->szSecretKey) - 1] = 0;
	pLogonSecretKey->szMachineID[CountArray(pLogonSecretKey->szMachineID) - 1] = 0;
	pLogonSecretKey->szMobilePhone[CountArray(pLogonSecretKey->szMobilePhone) - 1] = 0;

	//��������
	pExtendParameter->cbClientKind = CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID = pLogonSecretKey->wModuleID;
	pExtendParameter->dwDeviceKind = pLogonSecretKey->dwDeviceKind;
	pExtendParameter->dwAppVersion = pLogonSecretKey->dwAppVersion;
	pExtendParameter->dwPlazaVersion = pLogonSecretKey->dwPlazaVersion;

	//�汾�ж�
	if (CheckPlazaVersion(pLogonSecretKey->dwDeviceKind & DEVICE_MASK_MB, pLogonSecretKey->dwPlazaVersion, dwSocketID, dwTokenID) == false) return true;

	//��������
	DBR_MB_LogonSecretKey LogonSecretKey;
	ZeroMemory(&LogonSecretKey, sizeof(LogonSecretKey));

	//������Ϣ
	LogonSecretKey.pTokenParameter = pTokenParameter;
	LogonSecretKey.wGateID = pBindParameter->wServiceID;
	LogonSecretKey.wLogonID = m_pServiceOption->wServiceID;

	//��������	
	LogonSecretKey.dwUserID = pLogonSecretKey->dwUserID;
	LogonSecretKey.wMarketID = pLogonSecretKey->wMarketID;
	LogonSecretKey.dwClientIP = pTokenParameter->dwClientIP;
	LogonSecretKey.dwPlazaVersion = pLogonSecretKey->dwAppVersion;
	LogonSecretKey.dwDeviceKind = pLogonSecretKey->dwDeviceKind;
	StringCchCopy(LogonSecretKey.szSecretKey, CountArray(LogonSecretKey.szSecretKey), pLogonSecretKey->szSecretKey);
	StringCchCopy(LogonSecretKey.szMachineID, CountArray(LogonSecretKey.szMachineID), pLogonSecretKey->szMachineID);	
	StringCchCopy(LogonSecretKey.szMobilePhone, CountArray(LogonSecretKey.szMobilePhone), pLogonSecretKey->szMobilePhone);
	StringCchCopy(LogonSecretKey.szMachineOSID, CountArray(LogonSecretKey.szMachineOSID), pLogonSecretKey->szMachineOSID);

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_SECRETKEY, LogonSecretKey.dwUserID, dwSocketID, dwTokenID, &LogonSecretKey, sizeof(LogonSecretKey));

	return true;
}

//�ʺŵ�¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	CTraceService::TraceString(_T("s1"), TraceLevel_Normal);
	//Ч�����
	ASSERT(wDataSize>=sizeof(CMD_MB_LogonAccounts));
	if (wDataSize<sizeof(CMD_MB_LogonAccounts)) return false;
	CTraceService::TraceString(_T("s2"), TraceLevel_Normal);
	//�󶨲���
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;
	CTraceService::TraceString(_T("s3"), TraceLevel_Normal);
	//��ȡ����
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;
	CTraceService::TraceString(_T("s4"), TraceLevel_Normal);
	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//������Ϣ
	CMD_MB_LogonAccounts * pLogonAccounts=(CMD_MB_LogonAccounts *)pData;
	pLogonAccounts->szAccounts[CountArray(pLogonAccounts->szAccounts)-1]=0;
	pLogonAccounts->szPassword[CountArray(pLogonAccounts->szPassword)-1]=0;
	pLogonAccounts->szMachineID[CountArray(pLogonAccounts->szMachineID)-1]=0;
	pLogonAccounts->szMobilePhone[CountArray(pLogonAccounts->szMobilePhone)-1]=0;

	//��������
	pExtendParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID=pLogonAccounts->wModuleID;
	pExtendParameter->dwDeviceKind=pLogonAccounts->dwDeviceKind;
	pExtendParameter->dwAppVersion=pLogonAccounts->dwAppVersion;
	pExtendParameter->dwPlazaVersion=pLogonAccounts->dwPlazaVersion;
	
	CString s;
	s.Format(_T("1 szAccounts:%s"), pLogonAccounts->szAccounts);
	CTraceService::TraceString(s, TraceLevel_Normal);
	
	//�汾�ж�
	if (CheckPlazaVersion(pLogonAccounts->dwDeviceKind & DEVICE_MASK_MB, pLogonAccounts->dwPlazaVersion, dwSocketID, dwTokenID) == false)
	{
		CString s;
		s.Format(_T("2 szAccounts:%s"), pLogonAccounts->szAccounts);
		CTraceService::TraceString(s, TraceLevel_Normal);

		return true;
	}
	s.Format(_T("3 szAccounts:%s"), pLogonAccounts->szAccounts);
	CTraceService::TraceString(s, TraceLevel_Normal);

	//��������
	DBR_MB_LogonAccounts LogonAccounts;
	ZeroMemory(&LogonAccounts,sizeof(LogonAccounts));

	//������Ϣ
	LogonAccounts.pTokenParameter = pTokenParameter;
	LogonAccounts.wGateID = pBindParameter->wServiceID;
	LogonAccounts.wLogonID = m_pServiceOption->wServiceID;

	//��������	
	LogonAccounts.wMarketID=pLogonAccounts->wMarketID;
	LogonAccounts.dwClientIP = pTokenParameter->dwClientIP;
	LogonAccounts.dwPlazaVersion= pLogonAccounts->dwAppVersion;
	LogonAccounts.dwDeviceKind = pLogonAccounts->dwDeviceKind;
	StringCchCopy(LogonAccounts.szAccounts,CountArray(LogonAccounts.szAccounts), pLogonAccounts->szAccounts);
	StringCchCopy(LogonAccounts.szPassword,CountArray(LogonAccounts.szPassword), pLogonAccounts->szPassword);
	StringCchCopy(LogonAccounts.szMachineID,CountArray(LogonAccounts.szMachineID), pLogonAccounts->szMachineID);
	StringCchCopy(LogonAccounts.szMobilePhone,CountArray(LogonAccounts.szMobilePhone), pLogonAccounts->szMobilePhone);
	StringCchCopy(LogonAccounts.szMachineOSID, CountArray(LogonAccounts.szMachineOSID), pLogonAccounts->szMachineOSID);

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_ACCOUNTS,dwTokenID,dwSocketID,dwTokenID,&LogonAccounts,sizeof(LogonAccounts));

	return true;
}


//����ע��
bool CAttemperEngineSink::OnTCPNetworkSubMBRegisterEMail(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize >= sizeof(CMD_MB_RegisterEMail));
	if (wDataSize < sizeof(CMD_MB_RegisterEMail)) return false;

	//�󶨲���
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//��ȡ����
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//������Ϣ
	CMD_MB_RegisterEMail* pRegisterAccounts = (CMD_MB_RegisterEMail*)pData;
	pRegisterAccounts->szAuthCode[CountArray(pRegisterAccounts->szAuthCode) - 1] = 0;
	pRegisterAccounts->szNickName[CountArray(pRegisterAccounts->szNickName) - 1] = 0;
	pRegisterAccounts->szMachineID[CountArray(pRegisterAccounts->szMachineID) - 1] = 0;
	pRegisterAccounts->szLogonPass[CountArray(pRegisterAccounts->szLogonPass) - 1] = 0;
	pRegisterAccounts->szEMail[CountArray(pRegisterAccounts->szEMail) - 1] = 0;

	//��������
	pExtendParameter->cbClientKind = CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID = pRegisterAccounts->wModuleID;
	pExtendParameter->dwDeviceKind = pRegisterAccounts->dwDeviceKind;
	pExtendParameter->dwAppVersion = pRegisterAccounts->dwAppVersion;
	pExtendParameter->dwPlazaVersion = pRegisterAccounts->dwPlazaVersion;

	//Ч��汾
	if (CheckPlazaVersion(pRegisterAccounts->dwDeviceKind & DEVICE_MASK_MB, pRegisterAccounts->dwPlazaVersion, dwSocketID, dwTokenID) == false) return true;

	//��������
	DBR_MB_RegisterEMail RegisterAccounts;
	ZeroMemory(&RegisterAccounts, sizeof(RegisterAccounts));

	//������Ϣ
	RegisterAccounts.pTokenParameter = pTokenParameter;
	RegisterAccounts.wGateID = pBindParameter->wServiceID;
	RegisterAccounts.wLogonID = m_pServiceOption->wServiceID;

	//��������
	RegisterAccounts.wFaceID = pRegisterAccounts->wFaceID;
	RegisterAccounts.cbGender = pRegisterAccounts->cbGender;
	RegisterAccounts.wMarketID = pRegisterAccounts->wMarketID;
	RegisterAccounts.dwClientIP = pTokenParameter->dwClientIP;
	RegisterAccounts.dwPlazaVersion = pRegisterAccounts->dwAppVersion;
	RegisterAccounts.dwDeviceKind = pRegisterAccounts->dwDeviceKind;
	RegisterAccounts.dwSpreaderID = pRegisterAccounts->dwSpreaderID;
	StringCchCopy(RegisterAccounts.szAuthCode, CountArray(RegisterAccounts.szAuthCode), pRegisterAccounts->szAuthCode);
	StringCchCopy(RegisterAccounts.szNickName, CountArray(RegisterAccounts.szNickName), pRegisterAccounts->szNickName);
	StringCchCopy(RegisterAccounts.szMachineID, CountArray(RegisterAccounts.szMachineID), pRegisterAccounts->szMachineID);
	StringCchCopy(RegisterAccounts.szLogonPass, CountArray(RegisterAccounts.szLogonPass), pRegisterAccounts->szLogonPass);
	StringCchCopy(RegisterAccounts.szEMail, CountArray(RegisterAccounts.szEMail), pRegisterAccounts->szEMail);
	StringCchCopy(RegisterAccounts.szMachineOSID, CountArray(RegisterAccounts.szMachineOSID), pRegisterAccounts->szMachineOSID);

	//��չ��Ϣ
	if (wDataSize > sizeof(CMD_MB_RegisterEMail))
	{
		//��������
		VOID* pDataBuffer = NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pRegisterAccounts + 1, wDataSize - sizeof(CMD_MB_RegisterEMail));

		//��ȡ��Ϣ
		while (true)
		{
			//��ȡ����
			pDataBuffer = RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe == DTP_NULL) break;

			//��ȡ����
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_MB_PACKAGE_NAME:		//Ӧ�ð���
			{
				ASSERT(DataDescribe.wDataSize <= sizeof(RegisterAccounts.szPackageName));
				if (DataDescribe.wDataSize <= sizeof(RegisterAccounts.szPackageName))
				{
					RegisterAccounts.szPackageName[CountArray(RegisterAccounts.szPackageName) - 1L] = 0;
					StringCchCopy(RegisterAccounts.szPackageName, CountArray(RegisterAccounts.szPackageName), (LPCTSTR)pDataBuffer);
				}

				break;
			}
			}
		}
	}

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_REGISTER_EMAIL, dwTokenID, dwSocketID, dwTokenID, &RegisterAccounts, sizeof(RegisterAccounts));

	return true;
}

//�ʺ�ע��
bool CAttemperEngineSink::OnTCPNetworkSubMBRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT (wDataSize>=sizeof(CMD_MB_RegisterAccounts));
	if (wDataSize<sizeof(CMD_MB_RegisterAccounts)) return false;

	//�󶨲���
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//��ȡ����
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//������Ϣ
	CMD_MB_RegisterAccounts * pRegisterAccounts=(CMD_MB_RegisterAccounts *)pData;
	pRegisterAccounts->szAccounts[CountArray(pRegisterAccounts->szAccounts) - 1] = 0;
	pRegisterAccounts->szNickName[CountArray(pRegisterAccounts->szNickName)-1]=0;
	pRegisterAccounts->szMachineID[CountArray(pRegisterAccounts->szMachineID)-1]=0;
	pRegisterAccounts->szLogonPass[CountArray(pRegisterAccounts->szLogonPass)-1]=0;

	//��������
	pExtendParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID=pRegisterAccounts->wModuleID;
	pExtendParameter->dwDeviceKind=pRegisterAccounts->dwDeviceKind;
	pExtendParameter->dwAppVersion=pRegisterAccounts->dwAppVersion;
	pExtendParameter->dwPlazaVersion=pRegisterAccounts->dwPlazaVersion;

	//Ч��汾
	if (CheckPlazaVersion(pRegisterAccounts->dwDeviceKind & DEVICE_MASK_MB,pRegisterAccounts->dwPlazaVersion,dwSocketID,dwTokenID)==false) return true;

	//��������
	DBR_MB_RegisterAccounts RegisterAccounts;
	ZeroMemory(&RegisterAccounts,sizeof(RegisterAccounts));

	//������Ϣ
	RegisterAccounts.pTokenParameter = pTokenParameter;
	RegisterAccounts.wGateID = pBindParameter->wServiceID;
	RegisterAccounts.wLogonID = m_pServiceOption->wServiceID;

	//��������
	RegisterAccounts.wFaceID=pRegisterAccounts->wFaceID;
	RegisterAccounts.cbGender=pRegisterAccounts->cbGender;	
	RegisterAccounts.wMarketID=pRegisterAccounts->wMarketID;
	RegisterAccounts.dwClientIP=pTokenParameter->dwClientIP;
	RegisterAccounts.dwPlazaVersion=pRegisterAccounts->dwAppVersion;
    RegisterAccounts.dwDeviceKind = pRegisterAccounts->dwDeviceKind;
    RegisterAccounts.dwSpreaderID = pRegisterAccounts->dwSpreaderID;
	StringCchCopy(RegisterAccounts.szAccounts, CountArray(RegisterAccounts.szAccounts), pRegisterAccounts->szAccounts);
	StringCchCopy(RegisterAccounts.szNickName,CountArray(RegisterAccounts.szNickName), pRegisterAccounts->szNickName);
	StringCchCopy(RegisterAccounts.szMachineID,CountArray(RegisterAccounts.szMachineID), pRegisterAccounts->szMachineID);
	StringCchCopy(RegisterAccounts.szLogonPass,CountArray(RegisterAccounts.szLogonPass), pRegisterAccounts->szLogonPass);
	StringCchCopy(RegisterAccounts.szMachineOSID, CountArray(RegisterAccounts.szMachineOSID), pRegisterAccounts->szMachineOSID);

	//��չ��Ϣ
	if (wDataSize > sizeof(CMD_MB_RegisterAccounts))
	{
		//��������
		VOID* pDataBuffer = NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pRegisterAccounts + 1, wDataSize - sizeof(CMD_MB_RegisterAccounts));

		//��ȡ��Ϣ
		while (true)
		{
			//��ȡ����
			pDataBuffer = RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe == DTP_NULL) break;

			//��ȡ����
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_MB_PACKAGE_NAME:		//Ӧ�ð���
			{
				ASSERT(DataDescribe.wDataSize <= sizeof(RegisterAccounts.szPackageName));
				if (DataDescribe.wDataSize <= sizeof(RegisterAccounts.szPackageName))
				{
					RegisterAccounts.szPackageName[CountArray(RegisterAccounts.szPackageName) - 1L] = 0;
					StringCchCopy(RegisterAccounts.szPackageName, CountArray(RegisterAccounts.szPackageName), (LPCTSTR)pDataBuffer);
				}

				break;
			}
			}
		}
	}

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_REGISTER_ACCOUNTS, dwTokenID, dwSocketID,dwTokenID,&RegisterAccounts,sizeof(RegisterAccounts));

	return true;
}

//�ֻ�ע��
bool CAttemperEngineSink::OnTCPNetworkSubMBRegisterMobilePhone(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize >= sizeof(CMD_MB_RegisterMobilePhone));
	if (wDataSize < sizeof(CMD_MB_RegisterMobilePhone)) return false;

	//�󶨲���
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//��ȡ����
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//������Ϣ
	auto pRegisterAccounts = (CMD_MB_RegisterMobilePhone*)pData;
	pRegisterAccounts->szAuthCode[CountArray(pRegisterAccounts->szAuthCode) - 1] = 0;
	pRegisterAccounts->szNickName[CountArray(pRegisterAccounts->szNickName) - 1] = 0;
	pRegisterAccounts->szMachineID[CountArray(pRegisterAccounts->szMachineID) - 1] = 0;
	pRegisterAccounts->szLogonPass[CountArray(pRegisterAccounts->szLogonPass) - 1] = 0;
	pRegisterAccounts->szMobilePhone[CountArray(pRegisterAccounts->szMobilePhone) - 1] = 0;

	//��������
	pExtendParameter->cbClientKind = CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID = pRegisterAccounts->wModuleID;
	pExtendParameter->dwDeviceKind = pRegisterAccounts->dwDeviceKind;
	pExtendParameter->dwAppVersion = pRegisterAccounts->dwAppVersion;
	pExtendParameter->dwPlazaVersion = pRegisterAccounts->dwPlazaVersion;

	//Ч��汾
	if (CheckPlazaVersion(pRegisterAccounts->dwDeviceKind & DEVICE_MASK_MB, pRegisterAccounts->dwPlazaVersion, dwSocketID, dwTokenID) == false) return true;

	//��������
	DBR_MB_RegisterMobilePhone RegisterMobilePhone;
	ZeroMemory(&RegisterMobilePhone, sizeof(RegisterMobilePhone));

	//������Ϣ
	RegisterMobilePhone.pTokenParameter = pTokenParameter;
	RegisterMobilePhone.wGateID = pBindParameter->wServiceID;
	RegisterMobilePhone.wLogonID = m_pServiceOption->wServiceID;

	//��������
	RegisterMobilePhone.wFaceID = pRegisterAccounts->wFaceID;
	RegisterMobilePhone.cbGender = pRegisterAccounts->cbGender;
	RegisterMobilePhone.wMarketID = pRegisterAccounts->wMarketID;
	RegisterMobilePhone.dwClientIP = pTokenParameter->dwClientIP;
	RegisterMobilePhone.dwPlazaVersion = pRegisterAccounts->dwAppVersion;
	RegisterMobilePhone.dwDeviceKind = pRegisterAccounts->dwDeviceKind;
	RegisterMobilePhone.dwSpreaderID = pRegisterAccounts->dwSpreaderID;
	StringCchCopy(RegisterMobilePhone.szAuthCode, CountArray(RegisterMobilePhone.szAuthCode), pRegisterAccounts->szAuthCode);
	StringCchCopy(RegisterMobilePhone.szNickName, CountArray(RegisterMobilePhone.szNickName), pRegisterAccounts->szNickName);
	StringCchCopy(RegisterMobilePhone.szMachineID, CountArray(RegisterMobilePhone.szMachineID), pRegisterAccounts->szMachineID);
	StringCchCopy(RegisterMobilePhone.szLogonPass, CountArray(RegisterMobilePhone.szLogonPass), pRegisterAccounts->szLogonPass);
	StringCchCopy(RegisterMobilePhone.szMobilePhone, CountArray(RegisterMobilePhone.szMobilePhone), pRegisterAccounts->szMobilePhone);
	StringCchCopy(RegisterMobilePhone.szMachineOSID, CountArray(RegisterMobilePhone.szMachineOSID), pRegisterAccounts->szMachineOSID);

	//��չ��Ϣ
	if (wDataSize > sizeof(CMD_MB_RegisterMobilePhone))
	{
		//��������
		VOID* pDataBuffer = NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pRegisterAccounts + 1, wDataSize - sizeof(CMD_MB_RegisterMobilePhone));

		//��ȡ��Ϣ
		while (true)
		{
			//��ȡ����
			pDataBuffer = RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe == DTP_NULL) break;

			//��ȡ����
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_MB_PACKAGE_NAME:		//Ӧ�ð���
			{
				ASSERT(DataDescribe.wDataSize <= sizeof(RegisterMobilePhone.szPackageName));
				if (DataDescribe.wDataSize <= sizeof(RegisterMobilePhone.szPackageName))
				{
					RegisterMobilePhone.szPackageName[CountArray(RegisterMobilePhone.szPackageName) - 1L] = 0;
					StringCchCopy(RegisterMobilePhone.szPackageName, CountArray(RegisterMobilePhone.szPackageName), (LPCTSTR)pDataBuffer);
				}

				break;
			}
			}
		}
	}

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_REGISTER_MOBILEPHONE, dwTokenID, dwSocketID, dwTokenID, &RegisterMobilePhone, sizeof(RegisterMobilePhone));

	return true;
}


//�����¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonEMail(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize >= sizeof(CMD_MB_LogonEMail));
	if (wDataSize < sizeof(CMD_MB_LogonEMail)) return false;

	//�󶨲���
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//��ȡ����
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//������Ϣ
	CMD_MB_LogonEMail* pLogonEMail = (CMD_MB_LogonEMail*)pData;
	pLogonEMail->szPassword[CountArray(pLogonEMail->szPassword) - 1] = 0;
	pLogonEMail->szMachineID[CountArray(pLogonEMail->szMachineID) - 1] = 0;
	pLogonEMail->szEMail[CountArray(pLogonEMail->szEMail) - 1] = 0;

	//��������
	pExtendParameter->cbClientKind = CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID = pLogonEMail->wModuleID;
	pExtendParameter->dwDeviceKind = pLogonEMail->dwDeviceKind;
	pExtendParameter->dwAppVersion = pLogonEMail->dwAppVersion;
	pExtendParameter->dwPlazaVersion = pLogonEMail->dwPlazaVersion;

	//�汾�ж�
	if (CheckPlazaVersion(pLogonEMail->dwDeviceKind & DEVICE_MASK_MB, pLogonEMail->dwPlazaVersion, dwSocketID, dwTokenID) == false) return true;

	//��������
	DBR_MB_LogonEMail LogonEMail;
	ZeroMemory(&LogonEMail, sizeof(LogonEMail));

	//������Ϣ
	LogonEMail.pTokenParameter = pTokenParameter;
	LogonEMail.wGateID = pBindParameter->wServiceID;
	LogonEMail.wLogonID = m_pServiceOption->wServiceID;

	//��������	
	LogonEMail.wMarketID = pLogonEMail->wMarketID;
	LogonEMail.dwClientIP = pTokenParameter->dwClientIP;
	LogonEMail.dwPlazaVersion = pLogonEMail->dwAppVersion;
	LogonEMail.dwDeviceKind = pLogonEMail->dwDeviceKind;
	StringCchCopy(LogonEMail.szPassword, CountArray(LogonEMail.szPassword), pLogonEMail->szPassword);
	StringCchCopy(LogonEMail.szMachineID, CountArray(LogonEMail.szMachineID), pLogonEMail->szMachineID);
	StringCchCopy(LogonEMail.szEMail, CountArray(LogonEMail.szEMail), pLogonEMail->szEMail);
	StringCchCopy(LogonEMail.szMachineOSID, CountArray(LogonEMail.szMachineOSID), pLogonEMail->szMachineOSID);

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_EMAIL, dwTokenID, dwSocketID, dwTokenID, &LogonEMail, sizeof(LogonEMail));

	return true;
}

//������֤���¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonEMailCode(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize >= sizeof(CMD_MB_LogonEMailCode));
	if (wDataSize < sizeof(CMD_MB_LogonEMailCode)) return false;

	//�󶨲���
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//��ȡ����
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//������Ϣ
	CMD_MB_LogonEMailCode* pLogonMobileCode = (CMD_MB_LogonEMailCode*)pData;
	pLogonMobileCode->szAuthCode[CountArray(pLogonMobileCode->szAuthCode) - 1] = 0;
	pLogonMobileCode->szMachineID[CountArray(pLogonMobileCode->szMachineID) - 1] = 0;
	pLogonMobileCode->szEMail[CountArray(pLogonMobileCode->szEMail) - 1] = 0;

	//��������
	pExtendParameter->cbClientKind = CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID = pLogonMobileCode->wModuleID;
	pExtendParameter->dwDeviceKind = pLogonMobileCode->dwDeviceKind;
	pExtendParameter->dwAppVersion = pLogonMobileCode->dwAppVersion;
	pExtendParameter->dwPlazaVersion = pLogonMobileCode->dwPlazaVersion;

	//�汾�ж�
	if (CheckPlazaVersion(pLogonMobileCode->dwDeviceKind & DEVICE_MASK_MB, pLogonMobileCode->dwPlazaVersion, dwSocketID, dwTokenID) == false) return true;

	//��������
	DBR_MB_LogonEMailCode LogonMobileCode;
	ZeroMemory(&LogonMobileCode, sizeof(LogonMobileCode));

	//������Ϣ
	LogonMobileCode.pTokenParameter = pTokenParameter;
	LogonMobileCode.wGateID = pBindParameter->wServiceID;
	LogonMobileCode.wLogonID = m_pServiceOption->wServiceID;

	//��������	
	LogonMobileCode.wMarketID = pLogonMobileCode->wMarketID;
	LogonMobileCode.dwClientIP = pTokenParameter->dwClientIP;
	LogonMobileCode.dwPlazaVersion = pLogonMobileCode->dwAppVersion;
	LogonMobileCode.dwDeviceKind = pLogonMobileCode->dwDeviceKind;
	LogonMobileCode.dwSpreaderID = pLogonMobileCode->dwSpreaderID;
	StringCchCopy(LogonMobileCode.szAuthCode, CountArray(LogonMobileCode.szAuthCode), pLogonMobileCode->szAuthCode);
	StringCchCopy(LogonMobileCode.szMachineID, CountArray(LogonMobileCode.szMachineID), pLogonMobileCode->szMachineID);
	StringCchCopy(LogonMobileCode.szEMail, CountArray(LogonMobileCode.szEMail), pLogonMobileCode->szEMail);
	StringCchCopy(LogonMobileCode.szMachineOSID, CountArray(LogonMobileCode.szMachineOSID), pLogonMobileCode->szMachineOSID);

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_EMAILCODE, dwTokenID, dwSocketID, dwTokenID, &LogonMobileCode, sizeof(LogonMobileCode));

	return true;
}

//��֤���¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonMobileCode(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize >= sizeof(CMD_MB_LogonMobileCode));
	if (wDataSize < sizeof(CMD_MB_LogonMobileCode)) return false;

	//�󶨲���
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//��ȡ����
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//������Ϣ
	CMD_MB_LogonMobileCode * pLogonMobileCode = (CMD_MB_LogonMobileCode *)pData;
	pLogonMobileCode->szAuthCode[CountArray(pLogonMobileCode->szAuthCode) - 1] = 0;
	pLogonMobileCode->szMachineID[CountArray(pLogonMobileCode->szMachineID) - 1] = 0;
	pLogonMobileCode->szMobilePhone[CountArray(pLogonMobileCode->szMobilePhone) - 1] = 0;

	//��������
	pExtendParameter->cbClientKind = CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID = pLogonMobileCode->wModuleID;
	pExtendParameter->dwDeviceKind = pLogonMobileCode->dwDeviceKind;
	pExtendParameter->dwAppVersion = pLogonMobileCode->dwAppVersion;
	pExtendParameter->dwPlazaVersion = pLogonMobileCode->dwPlazaVersion;

	//�汾�ж�
	if (CheckPlazaVersion(pLogonMobileCode->dwDeviceKind & DEVICE_MASK_MB, pLogonMobileCode->dwPlazaVersion, dwSocketID, dwTokenID) == false) return true;

	//��������
	DBR_MB_LogonMobileCode LogonMobileCode;
	ZeroMemory(&LogonMobileCode, sizeof(LogonMobileCode));

	//������Ϣ
	LogonMobileCode.pTokenParameter = pTokenParameter;
	LogonMobileCode.wGateID = pBindParameter->wServiceID;
	LogonMobileCode.wLogonID = m_pServiceOption->wServiceID;

	//��������	
	LogonMobileCode.wMarketID = pLogonMobileCode->wMarketID;
	LogonMobileCode.dwClientIP = pTokenParameter->dwClientIP;
	LogonMobileCode.dwPlazaVersion = pLogonMobileCode->dwAppVersion;
	LogonMobileCode.dwDeviceKind = pLogonMobileCode->dwDeviceKind;
	LogonMobileCode.dwSpreaderID = pLogonMobileCode->dwSpreaderID;
	StringCchCopy(LogonMobileCode.szAuthCode, CountArray(LogonMobileCode.szAuthCode), pLogonMobileCode->szAuthCode);
	StringCchCopy(LogonMobileCode.szMachineID, CountArray(LogonMobileCode.szMachineID), pLogonMobileCode->szMachineID);
	StringCchCopy(LogonMobileCode.szMobilePhone, CountArray(LogonMobileCode.szMobilePhone), pLogonMobileCode->szMobilePhone);
	StringCchCopy(LogonMobileCode.szMachineOSID, CountArray(LogonMobileCode.szMachineOSID), pLogonMobileCode->szMachineOSID);

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_MOBILECODE, dwTokenID, dwSocketID, dwTokenID, &LogonMobileCode, sizeof(LogonMobileCode));

	return true;
}

//������¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonThirdParty(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT (wDataSize>=sizeof(CMD_MB_LogonThirdParty));
	if (wDataSize<sizeof(CMD_MB_LogonThirdParty)) return false;

	//�󶨲���
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//��ȡ����
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//������Ϣ
	CMD_MB_LogonThirdParty * pLogonThirdParty=(CMD_MB_LogonThirdParty *)pData;
	pLogonThirdParty->szUniqueID[CountArray(pLogonThirdParty->szUniqueID)-1]=0;
	pLogonThirdParty->szNickName[CountArray(pLogonThirdParty->szNickName)-1]=0;
	pLogonThirdParty->szMachineID[CountArray(pLogonThirdParty->szMachineID)-1]=0;
	pLogonThirdParty->szMobilePhone[CountArray(pLogonThirdParty->szMobilePhone)-1]=0;
	pLogonThirdParty->szCompellation[CountArray(pLogonThirdParty->szCompellation)-1]=0;

	//��������
	pExtendParameter->cbClientKind=CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID=pLogonThirdParty->wModuleID;
	pExtendParameter->dwDeviceKind=pLogonThirdParty->dwDeviceKind;
	pExtendParameter->dwAppVersion=pLogonThirdParty->dwAppVersion;
	pExtendParameter->dwPlazaVersion=pLogonThirdParty->dwPlazaVersion;

	//�汾�ж�
	if (CheckPlazaVersion(pLogonThirdParty->dwDeviceKind & DEVICE_MASK_MB,pLogonThirdParty->dwPlazaVersion,dwSocketID,dwTokenID)==false) return true;

	//��������
	DBR_MB_LogonThirdParty LogonThirdParty;
	ZeroMemory(&LogonThirdParty,sizeof(LogonThirdParty));

	//������Ϣ
	LogonThirdParty.pTokenParameter = pTokenParameter;
	LogonThirdParty.wGateID = pBindParameter->wServiceID;
	LogonThirdParty.wLogonID = m_pServiceOption->wServiceID;

	//��������		
	LogonThirdParty.cbGender=pLogonThirdParty->cbGender;	
	LogonThirdParty.wMarketID=pLogonThirdParty->wMarketID;
	LogonThirdParty.dwClientIP = pTokenParameter->dwClientIP;
	LogonThirdParty.dwExternalID=pLogonThirdParty->dwExternalID;	
	LogonThirdParty.dwPlazaVersion=pLogonThirdParty->dwAppVersion;
    LogonThirdParty.dwDeviceKind = pLogonThirdParty->dwDeviceKind;
    LogonThirdParty.dwSpreaderID = pLogonThirdParty->dwSpreaderID;
	StringCchCopy(LogonThirdParty.szUniqueID,CountArray(LogonThirdParty.szUniqueID), pLogonThirdParty->szUniqueID);
	StringCchCopy(LogonThirdParty.szNickName,CountArray(LogonThirdParty.szNickName), pLogonThirdParty->szNickName);
	StringCchCopy(LogonThirdParty.szMachineID,CountArray(LogonThirdParty.szMachineID), pLogonThirdParty->szMachineID);
	StringCchCopy(LogonThirdParty.szMachineOSID, CountArray(LogonThirdParty.szMachineOSID), pLogonThirdParty->szMachineOSID);
	StringCchCopy(LogonThirdParty.szMobilePhone,CountArray(LogonThirdParty.szMobilePhone), pLogonThirdParty->szMobilePhone);	
	StringCchCopy(LogonThirdParty.szCompellation,CountArray(LogonThirdParty.szCompellation), pLogonThirdParty->szCompellation);
	
	//��չ��Ϣ
	if (wDataSize > sizeof(CMD_MB_LogonThirdParty))
	{
		//��������
		VOID* pDataBuffer = NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pLogonThirdParty + 1, wDataSize - sizeof(CMD_MB_LogonThirdParty));

		//��ȡ��Ϣ
		while (true)
		{
			//��ȡ����
			pDataBuffer = RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe == DTP_NULL) break;

			//��ȡ����
			switch (DataDescribe.wDataDescribe)
			{	
			case DTP_MB_USERFACE_URL:		//ͷ���ַ
			{
				ASSERT(DataDescribe.wDataSize <= sizeof(LogonThirdParty.szUserFaceUrl));
				if (DataDescribe.wDataSize <= sizeof(LogonThirdParty.szUserFaceUrl))
				{
					LogonThirdParty.szUserFaceUrl[CountArray(LogonThirdParty.szUserFaceUrl) - 1L] = 0;
					StringCchCopy(LogonThirdParty.szUserFaceUrl, CountArray(LogonThirdParty.szUserFaceUrl), (LPCTSTR)pDataBuffer);
				}

				break;
			}
			case DTP_MB_EXTERNAL_DATUM:		//�ⲿ����
			{
				ASSERT (DataDescribe.wDataSize <= sizeof(LogonThirdParty.szUserDatum));
				if (DataDescribe.wDataSize <= sizeof(LogonThirdParty.szUserDatum))
				{
					LogonThirdParty.szUserDatum[CountArray(LogonThirdParty.szUserDatum) - 1L] = 0;
					StringCchCopy(LogonThirdParty.szUserDatum, CountArray(LogonThirdParty.szUserDatum), (LPCTSTR)pDataBuffer);
				}

				break;
			}
			case DTP_MB_PACKAGE_NAME:		//Ӧ�ð���
			{
				ASSERT(DataDescribe.wDataSize <= sizeof(LogonThirdParty.szPackageName));
				if (DataDescribe.wDataSize <= sizeof(LogonThirdParty.szPackageName))
				{
					LogonThirdParty.szPackageName[CountArray(LogonThirdParty.szPackageName) - 1L] = 0;
					StringCchCopy(LogonThirdParty.szPackageName, CountArray(LogonThirdParty.szPackageName), (LPCTSTR)pDataBuffer);
				}
				break;
			}
			}
		}
	}
	////CString s;
	//s.Format(_T(""))/*;*/
	//CTraceService::TraceString(LogonThirdParty.szUserFaceUrl, TraceLevel_Normal);
	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_THIRDPARTY, dwTokenID, dwSocketID,dwTokenID,&LogonThirdParty,sizeof(LogonThirdParty));

	return true;
}

//�ֻ���¼
bool CAttemperEngineSink::OnTCPNetworkSubMBLogonMobilePhone(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//Ч�����
	ASSERT(wDataSize >= sizeof(CMD_MB_LogonMobilePhone));
	if (wDataSize < sizeof(CMD_MB_LogonMobilePhone)) return false;

	//�󶨲���
	tagBindParameter* pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return true;

	//��ȡ����
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//������Ϣ
	CMD_MB_LogonMobilePhone * pLogonMobilePhone = (CMD_MB_LogonMobilePhone *)pData;
	pLogonMobilePhone->szPassword[CountArray(pLogonMobilePhone->szPassword) - 1] = 0;
	pLogonMobilePhone->szMachineID[CountArray(pLogonMobilePhone->szMachineID) - 1] = 0;
	pLogonMobilePhone->szMobilePhone[CountArray(pLogonMobilePhone->szMobilePhone) - 1] = 0;

	//��������
	pExtendParameter->cbClientKind = CLIENT_KIND_MOBILE;
	pExtendParameter->wModuleID = pLogonMobilePhone->wModuleID;
	pExtendParameter->dwDeviceKind = pLogonMobilePhone->dwDeviceKind;
	pExtendParameter->dwAppVersion = pLogonMobilePhone->dwAppVersion;
	pExtendParameter->dwPlazaVersion = pLogonMobilePhone->dwPlazaVersion;

	//�汾�ж�
	if (CheckPlazaVersion(pLogonMobilePhone->dwDeviceKind & DEVICE_MASK_MB, pLogonMobilePhone->dwPlazaVersion, dwSocketID, dwTokenID) == false) return true;

	//��������
	DBR_MB_LogonMobilePhone LogonMobilePhone;
	ZeroMemory(&LogonMobilePhone, sizeof(LogonMobilePhone));

	//������Ϣ
	LogonMobilePhone.pTokenParameter = pTokenParameter;
	LogonMobilePhone.wGateID = pBindParameter->wServiceID;
	LogonMobilePhone.wLogonID = m_pServiceOption->wServiceID;

	//��������	
	LogonMobilePhone.wMarketID = pLogonMobilePhone->wMarketID;
	LogonMobilePhone.dwClientIP = pTokenParameter->dwClientIP;
	LogonMobilePhone.dwPlazaVersion = pLogonMobilePhone->dwAppVersion;
	LogonMobilePhone.dwDeviceKind = pLogonMobilePhone->dwDeviceKind;
	StringCchCopy(LogonMobilePhone.szPassword, CountArray(LogonMobilePhone.szPassword), pLogonMobilePhone->szPassword);
	StringCchCopy(LogonMobilePhone.szMachineID, CountArray(LogonMobilePhone.szMachineID), pLogonMobilePhone->szMachineID);
	StringCchCopy(LogonMobilePhone.szMobilePhone, CountArray(LogonMobilePhone.szMobilePhone), pLogonMobilePhone->szMobilePhone);
	StringCchCopy(LogonMobilePhone.szMachineOSID, CountArray(LogonMobilePhone.szMachineOSID), pLogonMobilePhone->szMachineOSID);

	//Ͷ������
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_LOGON_MOBILEPHONE, dwTokenID, dwSocketID, dwTokenID, &LogonMobilePhone, sizeof(LogonMobilePhone));

	return true;
}

//�б��¼�
bool CAttemperEngineSink::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{	
	case SUB_CS_S_SERVICE_INSERT:
		{
			//Ч�����
			ASSERT(wDataSize % sizeof(tagServiceItem) == 0);
			if (wDataSize % sizeof(tagServiceItem) != 0) return false;

			//��������
			WORD wItemCount = wDataSize / sizeof(tagServiceItem);
			tagServiceItem * pServiceItem = (tagServiceItem *)pData;

			return true;
		}
	case SUB_CS_S_SERVICE_REMOVE:
		{
			//Ч�����
			ASSERT (wDataSize == sizeof(CMD_CS_S_ServiceRemove));
			if (wDataSize != sizeof(CMD_CS_S_ServiceRemove)) return false;

			//��������
			CMD_CS_S_ServiceRemove * pServiceRemove = (CMD_CS_S_ServiceRemove *)pData;

			return true;
		}
	case SUB_CS_S_SERVER_INFO:		//������Ϣ
		{
			//�����б�
			m_ServerListManager.DisuseServerItem();

			return true;
		}
	case SUB_CS_S_SERVICE_ONLINE:	//��������
		{
			//Ч�����
			ASSERT (wDataSize==sizeof(CMD_CS_S_ServiceOnline));
			if (wDataSize!=sizeof(CMD_CS_S_ServiceOnline)) return false;

			//��������
			CMD_CS_S_ServiceOnline * pServiceOnline =(CMD_CS_S_ServiceOnline *)pData;

			//У��ģ��
			if (pServiceOnline->wServiceModule != SERVICE_MODULE_GAME) return true;

			//���ҷ���
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServiceOnline->wServiceID);

			//��������
			if (pGameServerItem!=NULL)
			{
				pGameServerItem->m_GameServer.dwOnLineCount= pServiceOnline->wOnlineCount;
			}

			return true;
		}
	case SUB_CS_S_SERVER_INSERT:	//�������
		{
			//Ч�����
			ASSERT(wDataSize%sizeof(tagGameServer)==0);
			if (wDataSize%sizeof(tagGameServer)!=0) return false;

			//��������
			WORD wItemCount=wDataSize/sizeof(tagGameServer);
			tagGameServer * pGameServer=(tagGameServer *)pData;

			//��������
			for (WORD i=0;i<wItemCount;i++)
			{
				m_ServerListManager.InsertGameServer(pGameServer++);			
			}

			return true;
		}
	case SUB_CS_S_SERVER_REMOVE:	//�����Ƴ�
		{
			//Ч�����
			ASSERT(wDataSize == sizeof(CMD_CS_S_ServiceRemove));
			if (wDataSize != sizeof(CMD_CS_S_ServiceRemove)) return false;

			//��������
			CMD_CS_S_ServiceRemove* pServiceRemove = (CMD_CS_S_ServiceRemove*)pData;

			//���ҷ���
			auto pGameServerItem = m_ServerListManager.SearchGameServer(pServiceRemove->wServiceID);
			if (pGameServerItem != NULL)
			{
				//���ͱ�ʶ
				WORD wKindID = pGameServerItem->m_GameServer.wKindID;

				//��������
				m_ServerListManager.DeleteGameServer(pServiceRemove->wServiceID);
			}			

			return true;
		}
	case SUB_CS_S_GAME_JACKPOT:		//��Ϸ����
		{
			//Ч�����
			ASSERT (wDataSize%sizeof(CMD_CS_S_GameJackpot)==0);
			if ((wDataSize%sizeof(CMD_CS_S_GameJackpot))!=0) return false;

			//��������
			auto pGameJackpot = (CMD_CS_S_GameJackpot*)pData;
			WORD wJackpotCount = wDataSize / sizeof(CMD_CS_S_GameJackpot);

			for (int i = 0; i < wJackpotCount; i++)
			{
				//��������
				auto pGameKindItem = m_ServerListManager.SearchGameKind(pGameJackpot->wKindID);
				if (pGameKindItem != NULL)
				{
					pGameKindItem->m_ulLastTickCount = GetTickCount64();
					pGameKindItem->m_lJackpotScore = pGameJackpot->lJackpotScore;
				}

				pGameJackpot++;
			}			

			return true;
		}
	case SUB_CS_S_SERVER_MODIFY:	//�����޸�
		{
			//Ч�����
			ASSERT(wDataSize%sizeof(CMD_CS_S_ServerModify)==0);
			if (wDataSize%sizeof(CMD_CS_S_ServerModify)!=0) return false;

			//��������
			CMD_CS_S_ServerModify * pServerModify=(CMD_CS_S_ServerModify *)pData;

			//���ҷ���
			CGameServerItem * pGameServerItem=m_ServerListManager.SearchGameServer(pServerModify->wServerID);
			CGameFilterItem * pGameFilterItem=m_ServerListManager.SearchGameFilter(pServerModify->wServerID);

			//���÷���
			if (pGameServerItem!=NULL)
			{
				//������Ϣ
				if (pGameFilterItem!=NULL)
				{
					//���ñ���
					pServerModify->wSortID=pGameFilterItem->FilterSortID(pServerModify->wSortID);
					pServerModify->wKindID=pGameFilterItem->FilterKindID(pServerModify->wKindID);
					pServerModify->wNodeID=pGameFilterItem->FilterNodeID(pServerModify->wNodeID);

					//��������
					LPCTSTR pszServerName=pGameFilterItem->FilterServerName(pServerModify->szServerName);
					StringCchCopy(pServerModify->szServerName,CountArray(pServerModify->szServerName), pszServerName);
				}

				//���÷���
				pGameServerItem->m_GameServer.wNodeID=pServerModify->wNodeID;
				pGameServerItem->m_GameServer.wSortID=pServerModify->wSortID;
				pGameServerItem->m_GameServer.wKindID=pServerModify->wKindID;
				pGameServerItem->m_GameServer.wVisibleMask=pServerModify->wVisibleMask;
				pGameServerItem->m_GameServer.wServerPort=pServerModify->wServerPort;
				pGameServerItem->m_GameServer.wServerType=pServerModify->wServerType;
				pGameServerItem->m_GameServer.dwMaxPlayer=pServerModify->dwMaxPlayer;
				pGameServerItem->m_GameServer.dwOnLineCount=pServerModify->dwOnLineCount;
				pGameServerItem->m_GameServer.dwServerAddr=pServerModify->dwServerAddr;
				StringCchCopy(pGameServerItem->m_GameServer.szServerName,LEN_SERVER, pServerModify->szServerName);
				StringCchCopy(pGameServerItem->m_GameServer.szServerDomain,LEN_DOMAIN, pServerModify->szServerDomain);

				//��������
				pGameServerItem->m_GameServer.lCellScore=pServerModify->lCellScore;
				pGameServerItem->m_GameServer.lMinEnterScore=pServerModify->lMinEnterScore;
				pGameServerItem->m_GameServer.lMaxEnterScore=pServerModify->lMaxEnterScore;
			}

			return true;
		}	
	case SUB_CS_S_SERVER_FINISH:	//�������
		{
			//�����б�
			m_ServerListManager.CleanServerItem();
			m_ServerListManager.CollectOnlineInfo();

			//�¼�����
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CENTER_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}			
	}

	return true;
}

//�û��ռ�
bool CAttemperEngineSink::OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_COLLECT_USER:		//�ռ��û�
	{
		//��������
		CGlobalUserItem* pGlobalUserItem = NULL;

		do
		{
			pGlobalUserItem = m_GlobalItemManager.EnumUserItem(pGlobalUserItem);
			if (pGlobalUserItem != NULL)
			{
				//����ṹ
				CMD_CS_C_UserOnline UserOnline;
				ZeroMemory(&UserOnline, sizeof(UserOnline));

				//���ñ���
				UserOnline.dwUserID = pGlobalUserItem->m_dwUserID;
				UserOnline.dwGameID = pGlobalUserItem->m_dwGameID;
				StringCchCopy(UserOnline.szNickName, CountArray(UserOnline.szNickName), pGlobalUserItem->m_szNickName);
				StringCchCopy(UserOnline.szUserHeadUrl, CountArray(UserOnline.szUserHeadUrl), pGlobalUserItem->m_szUserHeadUrl);//kk jia

				//�û�����
				UserOnline.cbGender = pGlobalUserItem->m_cbGender;
				UserOnline.wFaceID = pGlobalUserItem->m_wFaceID;
				UserOnline.dwCustomID = pGlobalUserItem->m_dwCustomID;

				//״̬��Ϣ
				UserOnline.cbShutUpStatus = pGlobalUserItem->m_cbShutUpStatus;
				UserOnline.dwShutUpOverTime = pGlobalUserItem->m_dwShutUpOverTime;

				//������Ϣ
				UserOnline.wGateID = pGlobalUserItem->GetGateID();
				UserOnline.dwTokenID = pGlobalUserItem->GetTokenID();

				//�û�����
				m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_USER_ONLINE, &UserOnline, sizeof(UserOnline), 0);
			}

		} while (pGlobalUserItem!=NULL);		

		//�ռ����
		m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_COLLECT_FINISH, NULL, 0, 0);

		return true;
	}
	}

	return true;
}

//ת������
bool CAttemperEngineSink::OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_USER_SHUTUP_STATUS:	//����״̬
	{
		//У������
		ASSERT(wDataSize == sizeof(CMD_CS_UserShutUpStatus));
		if (wDataSize != sizeof(CMD_CS_UserShutUpStatus)) return true;

		//��ȡ����
		auto pUserShutUpStatus = (CMD_CS_UserShutUpStatus*)pData;

		//�����û�
		auto pGlobalUserItem = m_GlobalItemManager.SearchUserItem(pUserShutUpStatus->dwUserID);
		if (pGlobalUserItem != NULL)
		{
			pGlobalUserItem->m_cbShutUpStatus = pUserShutUpStatus->cbShutUpStatus;
			pGlobalUserItem->m_dwShutUpOverTime = pUserShutUpStatus->dwShutUpOverTime;

			//��������
			auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGlobalUserItem->GetGateID());
			if (pGlobalGateItem != NULL)
			{
				//����ṹ
				CMD_MB_ShutUpStatusUpdate ShutUpStatusUpdate;
				ZeroMemory(&ShutUpStatusUpdate,sizeof(ShutUpStatusUpdate));

				//���ñ���
				ShutUpStatusUpdate.cbShutUpStatus = pUserShutUpStatus->cbShutUpStatus;
				ShutUpStatusUpdate.dwShutUpOverTime = pUserShutUpStatus->dwShutUpOverTime;

				//������Ϣ	
				DWORD dwTokenID = pGlobalUserItem->GetTokenID();
				SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_MB_USER_SERVICE, SUB_MB_SHUTUP_STATUS_UPDATE, &ShutUpStatusUpdate, sizeof(ShutUpStatusUpdate),&dwTokenID,1);
			}
		}	

		return true;
	}	
	case SUB_CS_UPDATE_ACTION_MASK:	//��������
	{
		//��ȡ����
		auto pUpdateActionMask= (CMD_CS_UpdateActionMask*)pData;

		//�����û�
		auto pGlobalUserItem = m_GlobalItemManager.SearchUserItem(pUpdateActionMask->dwUserID);
		if (pGlobalUserItem != NULL)
		{
			//��ѯ����
			auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGlobalUserItem->GetGateID());
			if (pGlobalGateItem != NULL)
			{
				//��������
				CMD_MB_UserActionMask UserActionMask = {};

				//���ñ���
				UserActionMask.dwActionMaskEver = pUpdateActionMask->dwActionMaskEver;
				UserActionMask.dwActionMaskPerDay = pUpdateActionMask->dwActionMaskPerDay;
				UserActionMask.dwActionMaskPerWeek = pUpdateActionMask->dwActionMaskPerWeek;

				//������Ϣ
				auto dwTokenID = pGlobalUserItem->GetTokenID();
				auto dwSocketID = pGlobalGateItem->GetSocketID();				

				//������Ϣ
				SendDataToGate(dwSocketID, MDM_MB_USER_SERVICE, SUB_MB_USER_ACTION_MASK, &UserActionMask, sizeof(UserActionMask),&dwTokenID,1);
			}			
		}		

		return true;
	}
	case SUB_CS_LOGOUT_NOTIFY:		//�ǳ�����
	{
		//��ȡ����
		auto pLogoutNotify = (CMD_CS_LogoutNotify*)pData;

		//�����û�
		auto pGlobalUserItem = m_GlobalItemManager.SearchUserItem(pLogoutNotify->dwUserID);
		if (pGlobalUserItem == NULL) return true;


		//��������
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGlobalUserItem->GetGateID());
		if (pGlobalGateItem == NULL) return true;

		//���Ʋ���
		auto pTokenParameter = pGlobalGateItem->SearchTokenID(pGlobalUserItem->GetTokenID());
		if (pTokenParameter == NULL) return true;

		//��չ����
		auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

		//���Զ�
		if (pExtendParameter->cbClientKind == CLIENT_KIND_COMPUTER)
		{
			//����ṹ
			CMD_GP_LogoutNotify LogoutNotify;
			ZeroMemory(&LogoutNotify, sizeof(LogoutNotify));

			//���ñ���
			StringCchCopy(LogoutNotify.szDescribeString, CountArray(LogoutNotify.szDescribeString), pLogoutNotify->szNotifyContent);

			//��������		
			WORD wMsgSize = CountStringBuffer(LogoutNotify.szDescribeString);
			WORD wHeadSize = sizeof(LogoutNotify) - sizeof(LogoutNotify.szDescribeString);
			SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_GP_LOGON, SUB_GP_LOGOUT_NOTIFY, &LogoutNotify, wHeadSize + wMsgSize, &pGlobalUserItem->m_dwTokenID, 1);
		}		
		else
		{
			//����ṹ
			CMD_MB_LogoutNotify LogoutNotify;
			ZeroMemory(&LogoutNotify, sizeof(LogoutNotify));

			//���ñ���
			StringCchCopy(LogoutNotify.szDescribeString, CountArray(LogoutNotify.szDescribeString), pLogoutNotify->szNotifyContent);

			//��������		
			WORD wMsgSize = CountStringBuffer(LogoutNotify.szDescribeString);
			WORD wHeadSize = sizeof(LogoutNotify) - sizeof(LogoutNotify.szDescribeString);
			SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_MB_LOGON, SUB_MB_LOGOUT_NOTIFY, &LogoutNotify, wHeadSize + wMsgSize, &pGlobalUserItem->m_dwTokenID, 1);
		}

		//����ṹ
		CMD_CS_C_ShutdownSocket ShutdownSocket = {true};		

		//�ر�����
		SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket),&pGlobalUserItem->m_dwTokenID,1);

		return true;
	}
	case SUB_CS_FILTER_WORDS_UPDATE:	//�ʻ����
	{
		//��ȡ����
		auto pFilterWordsUpdate = (CMD_CS_FilterWordsUpdate*)pData;

		//ɾ���ж�
		if (pFilterWordsUpdate->bDeleted == false)
		{
			m_WordsFilter.InsertKeyword(pFilterWordsUpdate->szFilterWords);
		}
		else
		{
			m_WordsFilter.DeleteKeyword(pFilterWordsUpdate->szFilterWords);
		}

		return true;
	}	
	case SUB_CS_RANKING_LIST_UPDATE:	//�񵥸���
	{
		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_RANKING_LIST, 0, 0, 0, NULL, 0);

		return true;
	}
	case SUB_CS_USER_WEALTH_UPDATE:		//�Ƹ�����
	{
		//У������
		ASSERT(wDataSize == sizeof(CMD_CS_UserWealthUpdate));
		if (wDataSize != sizeof(CMD_CS_UserWealthUpdate)) return true;

		//��ȡ����
		auto pUserWealthUpdate = (CMD_CS_UserWealthUpdate*)pData;

		//�����û�
		auto pGlobalUserItem = m_GlobalItemManager.SearchUserItem(pUserWealthUpdate->dwUserID);
		if (pGlobalUserItem == NULL) return true;

		//��������
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGlobalUserItem->GetGateID());
		if (pGlobalGateItem == NULL) return true;

		//����ṹ
		DBR_MB_QueryWealth QueryWealth;
		ZeroMemory(&QueryWealth, sizeof(QueryWealth));

		//��������
		QueryWealth.dwUserID = pUserWealthUpdate->dwUserID;

		//Ͷ������		
		DWORD dwTokenID = pGlobalUserItem->GetTokenID();
		DWORD dwSocketID = pGlobalGateItem->GetSocketID();
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_QUERY_WEALTH, 0, dwSocketID, dwTokenID, &QueryWealth, sizeof(QueryWealth));

		return true;
	}
	case SUB_CS_USER_RECHARGE_NOTICE:	//��ֵ֪ͨ
	{
		//У������
		ASSERT(wDataSize == sizeof(CMD_CS_UserRechargeNotice));
		if (wDataSize != sizeof(CMD_CS_UserRechargeNotice)) return true;

		//��ȡ����
		auto pUserRechargeNotice = (CMD_CS_UserRechargeNotice*)pData;

		//�����û�
		auto pGlobalUserItem = m_GlobalItemManager.SearchUserItem(pUserRechargeNotice->dwUserID);
		if (pGlobalUserItem == NULL) return true;

		//��������
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGlobalUserItem->GetGateID());
		if (pGlobalGateItem == NULL) return true;

		//����ṹ
		DBR_MB_QueryWealth QueryWealth;
		ZeroMemory(&QueryWealth, sizeof(QueryWealth));

		//��������
		QueryWealth.dwUserID = pUserRechargeNotice->dwUserID;

		//������Ϣ
		DWORD dwTokenID = pGlobalUserItem->GetTokenID();
		DWORD dwSocketID = pGlobalGateItem->GetSocketID();		

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_MB_QUERY_WEALTH, 0, dwSocketID, dwTokenID, &QueryWealth, sizeof(QueryWealth));

		//�����ɹ�
		SendOperateSuccess(dwSocketID, dwTokenID,0,0,NULL, pUserRechargeNotice->szGoodsList);

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
	case SUB_CS_S_EXECUTE_COMMAND:		//ִ������
		{
			//����У��
			ASSERT (sizeof(tagExecuteCommand)==wDataSize);
			if (sizeof(tagExecuteCommand)!=wDataSize) return false;

			//��ȡ����
			auto pCommandCode=(tagExecuteCommand*)pData;

			//��������
			if (pCommandCode->dwCommandCode== CMD_LOAD_CONFIG_DATA && pCommandCode->dwCommandParam!=0)
			{
				//��������
				DBR_GP_LoadConfig LoadConfig;
				ZeroMemory(&LoadConfig, sizeof(LoadConfig));

				//���ñ���
				LoadConfig.dwConfigMask = pCommandCode->dwCommandParam;

				//��������
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_CONFIG, 0, 0, 0, &LoadConfig, sizeof(LoadConfig));
			}			

			return true;
		}
	}

	return true;
}

//��¼�ɹ�
bool CAttemperEngineSink::OnDBPCLogonSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	auto pBindParameter = GetBindParameter(dwContextID);
	if (pBindParameter == NULL) return true;

	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID,dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��������
	DBO_GP_LogonSuccess * pDBOLogonSuccess=(DBO_GP_LogonSuccess *)pData;
	CMD_GP_LogonSuccess * pCMDLogonSuccess=(CMD_GP_LogonSuccess *)m_cbBuffer;

	//���Ͷ���
	WORD wHeadSize=sizeof(CMD_GP_LogonSuccess);
	CSendPacketHelper SendPacket(m_cbBuffer+wHeadSize,sizeof(m_cbBuffer)-wHeadSize);

	//���ñ���
	ZeroMemory(pCMDLogonSuccess,sizeof(CMD_GP_LogonSuccess));

	//��������
	pCMDLogonSuccess->cbAge=pDBOLogonSuccess->cbAge;
	pCMDLogonSuccess->wFaceID=pDBOLogonSuccess->wFaceID;
	pCMDLogonSuccess->cbGender=pDBOLogonSuccess->cbGender;
	pCMDLogonSuccess->dwGameID=pDBOLogonSuccess->dwGameID;
	pCMDLogonSuccess->dwUserID=pDBOLogonSuccess->dwUserID;
	pCMDLogonSuccess->dwCustomID=pDBOLogonSuccess->dwCustomID;
	pCMDLogonSuccess->dwProtectID = pDBOLogonSuccess->dwProtectID;
	pCMDLogonSuccess->cbMoorMachine = pDBOLogonSuccess->cbMoorMachine;
	pCMDLogonSuccess->cbMasterOrder = pDBOLogonSuccess->cbMasterOrder;
	StringCchCopy(pCMDLogonSuccess->szAccounts,CountArray(pCMDLogonSuccess->szAccounts), pDBOLogonSuccess->szAccounts);
	StringCchCopy(pCMDLogonSuccess->szPassword,CountArray(pCMDLogonSuccess->szPassword), pDBOLogonSuccess->szPassword);

	//����ȼ�
	pCMDLogonSuccess->dwExperience = pDBOLogonSuccess->dwExperience;

	//�Ƹ���Ϣ
	pCMDLogonSuccess->lUserGold = pDBOLogonSuccess->lUserGold;
	pCMDLogonSuccess->lInsureGold = pDBOLogonSuccess->lInsureGold;

	//��Ա�ȼ�
	pCMDLogonSuccess->dwMemberPoint = pDBOLogonSuccess->dwMemberPoint;
	pCMDLogonSuccess->dwMemberOverTime = pDBOLogonSuccess->dwMemberOverTime;

	//������Ϣ
	if ((pDBOLogonSuccess->szArea[0]!=0)||(pDBOLogonSuccess->szCity[0]!=0)||(pDBOLogonSuccess->szProvince[0]!=0))
	{
		DTP_GP_AreaInfo AreaInfo;
		ZeroMemory(&AreaInfo,sizeof(AreaInfo));
		StringCchCopy(AreaInfo.szArea,CountArray(AreaInfo.szArea), pDBOLogonSuccess->szArea);
		StringCchCopy(AreaInfo.szCity,CountArray(AreaInfo.szCity), pDBOLogonSuccess->szCity);
		StringCchCopy(AreaInfo.szProvince,CountArray(AreaInfo.szProvince), pDBOLogonSuccess->szProvince);
		SendPacket.AddPacket(&AreaInfo,sizeof(AreaInfo),DTP_GP_AREA_INFO);
	}	

	//����ǩ��
	if (pDBOLogonSuccess->szUnderWrite[0]!=0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szUnderWrite,CountStringBuffer(pDBOLogonSuccess->szUnderWrite),DTP_GP_UNDER_WRITE);
	}

	//�û��ǳ�
	if(pDBOLogonSuccess->szNickName[0]!=0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szNickName,CountStringBuffer(pDBOLogonSuccess->szNickName),DTP_GP_NICKNAME);
	}

	//��������
	DWORD dwOnlineInfoMask=m_dwShowOnLineInfo;
	if (pCMDLogonSuccess->cbMasterOrder>0) dwOnlineInfoMask=SHOW_TOTAL_USER_COUNT|SHOW_KIND_USER_COUNT|SHOW_SERVER_USER_COUNT;

	//������ʾ������Ϣ����
	SendDataToGate(dwContextID,MDM_GP_LOGON,SUB_GP_SHOW_ONLINE,&dwOnlineInfoMask,sizeof(dwOnlineInfoMask),&dwTokenID,1);

	//��¼�ɹ�
	WORD wSendSize=SendPacket.GetDataSize()+sizeof(CMD_GP_LogonSuccess);
	SendDataToGate(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_SUCCESS,m_cbBuffer,wSendSize,&dwTokenID,1);

	//����ṹ
	CMD_CS_S_TokenParameter TokenParameter;
	ZeroMemory(&TokenParameter, sizeof(TokenParameter));

	//���Ʋ���
	TokenParameter.dwUserID = pDBOLogonSuccess->dwUserID;
	StringCchCopy(TokenParameter.szLogonToken, CountArray(TokenParameter.szLogonToken), pDBOLogonSuccess->szPassword);

	//��������
	WORD wPacketSize = CountStringBuffer(TokenParameter.szLogonToken);
	wPacketSize += sizeof(TokenParameter) - sizeof(TokenParameter.szLogonToken);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CS_COMMON_SERVICE, SUB_CS_S_TOKEN_PARAMETER, &TokenParameter, wPacketSize, &dwTokenID, 1);

	//��������
	bool bSendAll=(pDBOLogonSuccess->cbMasterOrder>0)||(pDBOLogonSuccess->dwCheckUserRight==UR_CAN_MANAGER_ANDROID);

	//�����б�
	SendGameTypeInfo(dwContextID,dwTokenID);

	//��������
	SendGameKindInfo(dwContextID,dwTokenID,bSendAll?(SUPPORT_MASK_PC|SUPPORT_MASK_MOBILE):SUPPORT_MASK_PC);

	//����ҳ��
	SendGamePageInfo(dwContextID,dwTokenID,0);

	//���ͷ���
	if (bSendAll==true) SendGameServerInfo(dwContextID,dwTokenID,0,0);

	//�б����
	SendDataToGate(dwContextID,MDM_GP_SERVER_LIST,SUB_GP_LIST_FINISH,&dwTokenID,1);

	//��¼���
	CMD_GP_LogonFinish LogonFinish;
	ZeroMemory(&LogonFinish,sizeof(LogonFinish));
	LogonFinish.wIntermitTime=m_pInitParameter->m_wIntermitTime;
	LogonFinish.wOnLineCountTime=m_pInitParameter->m_wOnLineCountTime;
	LogonFinish.dwFunctionOption=m_dwPCFuncption;	
	SendDataToGate(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_FINISH,&LogonFinish,sizeof(LogonFinish),&dwTokenID,1);

	//////////////////////////////////////////////////////////////////////////////////////////////////

	//����ṹ
	tagGlobalUserInfo GlobalUserInfo;
	ZeroMemory(&GlobalUserInfo, sizeof(GlobalUserInfo));

	//�û���Ϣ
	GlobalUserInfo.dwUserID = pDBOLogonSuccess->dwUserID;
	GlobalUserInfo.dwGameID = pDBOLogonSuccess->dwGameID;
	GlobalUserInfo.cbGender = pDBOLogonSuccess->cbGender;
	GlobalUserInfo.wFaceID = pDBOLogonSuccess->wFaceID;
	GlobalUserInfo.dwCustomID = pDBOLogonSuccess->dwCustomID;	
	StringCchCopy(GlobalUserInfo.szNickName, CountArray(GlobalUserInfo.szNickName), pDBOLogonSuccess->szNickName);

	//������Ϣ
	GlobalUserInfo.SocketInfo.wGateID = pBindParameter->wServiceID;
	GlobalUserInfo.SocketInfo.dwTokenID = pTokenParameter->dwTokenID;

	//�����û�
	auto pGlobalUserItem = m_GlobalItemManager.ActiveUserItem(GlobalUserInfo);
	if (pGlobalUserItem != NULL)
	{
		//�ռ��û�
		CMD_CS_C_UserOnline  UserOnline;
		ZeroMemory(&UserOnline, sizeof(UserOnline));

		//���ñ���
		UserOnline.dwUserID = pDBOLogonSuccess->dwUserID;
		UserOnline.dwGameID = pDBOLogonSuccess->dwGameID;
		StringCchCopy(UserOnline.szNickName, CountArray(UserOnline.szNickName), pDBOLogonSuccess->szNickName);
		//StringCchCopy(UserOnline.szUserHeadUrl, CountArray(UserOnline.szUserHeadUrl), pDBOLogonSuccess->szUserFaceUrl);//kk jia

		//�û�����
		UserOnline.cbGender = pDBOLogonSuccess->cbGender;
		UserOnline.wFaceID = pDBOLogonSuccess->wFaceID;
		UserOnline.dwCustomID = pDBOLogonSuccess->dwCustomID;

		//������Ϣ
		UserOnline.wGateID = pBindParameter->wServiceID;
		UserOnline.dwTokenID = pTokenParameter->dwTokenID;		

		//�û�����
		m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_USER_ONLINE, &UserOnline, sizeof(UserOnline), 0);
	}

	//���û�
	pTokenParameter->pBindUserItem = pGlobalUserItem;
	pTokenParameter->dwUserID = pGlobalUserItem->GetUserID();

	//���봦��
	if (pDBOLogonSuccess->wLockGateID != 0 && pDBOLogonSuccess->wLockLogonID != 0 && pDBOLogonSuccess->dwLockSocketID != 0)
	{
		//��������
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pDBOLogonSuccess->wLockGateID);
		if (pGlobalGateItem != NULL)
		{
			//����ṹ
			CMD_GP_LogoutNotify LogoutNotify;
			ZeroMemory(&LogoutNotify, sizeof(LogoutNotify));

			//���ñ���
			StringCchCopy(LogoutNotify.szDescribeString, CountArray(LogoutNotify.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0007, TEXT("���������˺��ڱ𴦵�¼�����������ߣ�")));

			//��������
			WORD wMsgSize = CountStringBuffer(LogoutNotify.szDescribeString);
			WORD wHeadSize = sizeof(LogoutNotify) - sizeof(LogoutNotify.szDescribeString);
			SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_GP_LOGON, SUB_GP_LOGOUT_NOTIFY, &LogoutNotify, wHeadSize + wMsgSize, &pDBOLogonSuccess->dwLockSocketID, 1);

			//�Ƴ�����
			if (pDBOLogonSuccess->wLockLogonID == m_pServiceOption->wServiceID)
			{
				pGlobalGateItem->RemoveTokenID(pDBOLogonSuccess->dwLockSocketID);
			}

			//����ṹ
			CMD_CS_C_ShutdownSocket ShutdownSocket = { true };

			//�ر�����
			SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket), &pDBOLogonSuccess->dwLockSocketID, 1);
		}
	}

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBPCLogonFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��������
	CMD_GP_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));
	DBO_GP_LogonFailure * pLogonFailure=(DBO_GP_LogonFailure *)pData;

	//��������
	LogonFailure.lErrorCode=pLogonFailure->lErrorCode;
	StringCchCopy(LogonFailure.szDescribeString,CountArray(LogonFailure.szDescribeString), pLogonFailure->szDescribeString);

	//��������
	WORD wStringSize=CountStringBuffer(LogonFailure.szDescribeString);
	WORD wSendSize=sizeof(LogonFailure)-sizeof(LogonFailure.szDescribeString)+wStringSize;
	SendDataToGate(dwContextID,MDM_GP_LOGON,SUB_GP_LOGON_FAILURE,&LogonFailure,wSendSize,&dwTokenID,1);

	//����ṹ
	CMD_CS_C_ShutdownSocket ShutdownSocket;
	ShutdownSocket.bForceClose = false;
	
	//�ر�����
	SendDataToGate(dwContextID,MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket),&dwTokenID,1);

	return true;
}

//�û���Ϣ
bool CAttemperEngineSink::OnDBPCUserIndividual(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��������
	DBO_GP_UserIndividual * pDBOUserIndividual=(DBO_GP_UserIndividual *)pData;
	CMD_GP_UserIndividual * pCMDUserIndividual=(CMD_GP_UserIndividual *)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer+sizeof(CMD_GP_UserIndividual),sizeof(m_cbBuffer)-sizeof(CMD_GP_UserIndividual));

	//���ñ���
	ZeroMemory(pCMDUserIndividual,sizeof(CMD_GP_UserIndividual));

	//��������
	pCMDUserIndividual->dwUserID=pDBOUserIndividual->dwUserID;

	//�û���Ϣ
	if (pDBOUserIndividual->szUserNote[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szUserNote);
		SendPacket.AddPacket(pDBOUserIndividual->szUserNote,wBufferSize,DTP_GP_UI_USER_NOTE);
	}

	//��ʵ����
	if (pDBOUserIndividual->szCompellation[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szCompellation);
		SendPacket.AddPacket(pDBOUserIndividual->szCompellation,wBufferSize,DTP_GP_UI_COMPELLATION);
	}

	//�绰����
	if (pDBOUserIndividual->szSeatPhone[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szSeatPhone);
		SendPacket.AddPacket(pDBOUserIndividual->szSeatPhone,wBufferSize,DTP_GP_UI_SEAT_PHONE);
	}

	//�ƶ��绰
	if (pDBOUserIndividual->szMobilePhone[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szMobilePhone);
		SendPacket.AddPacket(pDBOUserIndividual->szMobilePhone,wBufferSize,DTP_GP_UI_MOBILE_PHONE);
	}

	//��ϵ����
	if (pDBOUserIndividual->szQQ[0]!=0) 
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szQQ);
		SendPacket.AddPacket(pDBOUserIndividual->szQQ,wBufferSize,DTP_GP_UI_QQ);
	}

	//�����ʼ�
	if (pDBOUserIndividual->szEMail[0]!=0) 
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szEMail);
		SendPacket.AddPacket(pDBOUserIndividual->szEMail,wBufferSize,DTP_GP_UI_EMAIL);
	}

	//��ϵ��ַ
	if (pDBOUserIndividual->szDwellingPlace[0]!=0)
	{
		WORD wBufferSize=CountStringBuffer(pDBOUserIndividual->szDwellingPlace);
		SendPacket.AddPacket(pDBOUserIndividual->szDwellingPlace,wBufferSize,DTP_GP_UI_DWELLING_PLACE);
	}

	//������Ϣ
	WORD wSendSize=sizeof(CMD_GP_UserIndividual)+SendPacket.GetDataSize();
	SendDataToGate(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_USER_INDIVIDUAL,m_cbBuffer,wSendSize,&dwTokenID,1);

	return true;
}

//�����ɹ�
bool CAttemperEngineSink::OnDBPCOperateSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//��������
	if (pExtendParameter->cbClientKind == CLIENT_KIND_COMPUTER)
	{
		//��ȡ����
		DBO_GP_OperateSuccess* pOperateSuccess = (DBO_GP_OperateSuccess*)pData;

		//��������
		CMD_GP_OperateSuccess OperateSuccess;
		ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));		

		//��������
		OperateSuccess.lResultCode = pOperateSuccess->lResultCode;
		StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString),pOperateSuccess->szDescribeString);

		//��������
		WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
		WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
		SendDataToGate(dwContextID, MDM_GP_USER_SERVICE, SUB_GP_OPERATE_SUCCESS, &OperateSuccess, wSendSize,&dwTokenID,1);
	}

	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBPCOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//��������
	if (pExtendParameter->cbClientKind == CLIENT_KIND_COMPUTER)
	{
		//��������
		CMD_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));
		DBO_GP_OperateFailure* pOperateFailure = (DBO_GP_OperateFailure*)pData;

		//��������
		OperateFailure.lErrorCode = pOperateFailure->lErrorCode;
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), pOperateFailure->szDescribeString);

		//��������
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		SendDataToGate(dwContextID, MDM_GP_USER_SERVICE, SUB_GP_OPERATE_FAILURE, &OperateFailure, wSendSize,&dwTokenID,1);
	}

	return true;
}

//��������
bool CAttemperEngineSink::OnDBPCGameBasicConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��ȡ����
	auto pBasicConfig = (DBO_GP_GameBasicConfig*)pData;

	//����ṹ
	CMD_GP_GameBasicConfig BasicConfig = {};

	//��������
	StringCchCopy(BasicConfig.szBasicConfig,CountArray(BasicConfig.szBasicConfig), pBasicConfig->szBasicConfig);

	//��������
	WORD wSendSize = CountStringBuffer(BasicConfig.szBasicConfig);
	wSendSize += sizeof(BasicConfig) - sizeof(BasicConfig.szBasicConfig);
	SendDataToGate(dwContextID, MDM_GP_CONFIG_SERVICE, SUB_GP_GAME_BASIC_CONFIG, &BasicConfig, wSendSize, &dwTokenID, 1);

	return true;
}

//Ȩ������
bool CAttemperEngineSink::OnDBPCGameWeightConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��ȡ����
	auto pWeightConfig = (DBO_GP_GameWeightConfig*)pData;

	//����ṹ
	CMD_GP_GameWeightConfig WeightConfig = {};

	//��������
	WeightConfig.wItemCount = pWeightConfig->wItemCount;
	CopyMemory(WeightConfig.TimesWeightItem, pWeightConfig->TimesWeightItem,sizeof(WeightConfig.TimesWeightItem));

	//��������
	WORD wSendSize = sizeof(WeightConfig) - sizeof(WeightConfig.TimesWeightItem);
	wSendSize += WeightConfig.wItemCount * sizeof(WeightConfig.TimesWeightItem[0]);
	SendDataToGate(dwContextID, MDM_GP_CONFIG_SERVICE, SUB_GP_GAME_WEIGHT_CONFIG, &WeightConfig, wSendSize, &dwTokenID, 1);

	return true;
}

//�������
bool CAttemperEngineSink::OnDBPCGameConfigFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��������
	SendDataToGate(dwContextID, MDM_GP_CONFIG_SERVICE, SUB_GP_GAME_CONFIG_FINISH, NULL, 0, &dwTokenID, 1);

	return true;
}

//��������
bool CAttemperEngineSink::OnDBPCRobotOperateResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��������
	auto pRobotOperateResult = (DBO_GP_RobotOperateResult*)pData;

	//��������
	CMD_GP_RobotOperateResult RobotOperateResult;
	ZeroMemory(&RobotOperateResult, sizeof(RobotOperateResult));

	//�������
	RobotOperateResult.wSubCommdID = pRobotOperateResult->wSubCommdID;
	RobotOperateResult.wParameterCount = pRobotOperateResult->wParameterCount;
	CopyMemory(RobotOperateResult.RobotParameter, pRobotOperateResult->RobotParameter, sizeof(tagRobotParameter) * RobotOperateResult.wParameterCount);

	//�����С
	WORD wPacketSize= sizeof(RobotOperateResult) - sizeof(RobotOperateResult.RobotParameter);
	wPacketSize += sizeof(tagRobotParameter) * RobotOperateResult.wParameterCount;

	//��������
	SendDataToGate(dwContextID, MDM_GP_ROBOT_SERVICE, SUB_GP_ROBOT_OPERATE_RESULT, &RobotOperateResult, wPacketSize,&dwTokenID,1);

	//�����ж�
	if (RobotOperateResult.wParameterCount == 1)
	{
		//���嶨��
		WORD wBufferSize = 0;
		BYTE cbBuffer[1024] = {0};		

		//����ṹ
		tagForwardHead* pForwadHead = (tagForwardHead*)cbBuffer;
		
		//ת������
		pForwadHead->wItemCount = 1;
		pForwadHead->wTarget = FORWARD_TARGET_SERVICE;
		pForwadHead->ForwardItem[0].wServiceID = pRobotOperateResult->wServerID;
		pForwadHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_GAME;

		//��Ӳ���
		if (RobotOperateResult.wSubCommdID == SUB_GP_APPEND_PARAMETER)
		{
			//����ṹ
			auto pAppendParameter= (CMD_CS_AppendRobotParameter*)(pForwadHead+1);

			//���ñ���
			CopyMemory(&pAppendParameter->RobotParameter, &RobotOperateResult.RobotParameter[0], sizeof(tagRobotParameter));

			//��������
			wBufferSize = sizeof(tagForwardHead) + sizeof(CMD_CS_AppendRobotParameter);
			m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_APPEND_ROBOT_PARAMETER, cbBuffer, wBufferSize, dwTokenID);
		}

		//�޸Ĳ���
		if (RobotOperateResult.wSubCommdID == SUB_GP_MODIFY_PARAMETER)
		{
			//����ṹ
			auto pModifyParameter = (CMD_CS_ModifyRobotParameter*)(pForwadHead + 1);

			//��������
			CopyMemory(&pModifyParameter->RobotParameter, &RobotOperateResult.RobotParameter[0], sizeof(tagRobotParameter));

			//��������
			wBufferSize = sizeof(tagForwardHead) + sizeof(CMD_CS_ModifyRobotParameter);
			m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_MODIFY_ROBOT_PARAMETER, cbBuffer, wBufferSize, dwTokenID);
		}

		//ɾ������
		if (RobotOperateResult.wSubCommdID == SUB_GP_DELETE_PARAMETER)
		{
			//����ṹ
			auto pDeleteParameter = (CMD_CS_DeleteRobotParameter*)(pForwadHead + 1);

			//����ṹ
			pDeleteParameter->dwBatchID = RobotOperateResult.RobotParameter[0].dwBatchID;

			//��������
			wBufferSize = sizeof(tagForwardHead) + sizeof(CMD_CS_DeleteRobotParameter);
			m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_DELETE_ROBOT_PARAMETER, cbBuffer, wBufferSize, dwTokenID);
		}
	}

	return true;
}

//�������
bool CAttemperEngineSink::OnDBPCStockOperateResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��������
	auto pStockOperateResult = (DBO_GP_StockOperateResult*)pData;

	//��������
	CMD_GP_StockOperateResult StockOperateResult;
	ZeroMemory(&StockOperateResult, sizeof(StockOperateResult));

	//�������
	StockOperateResult.wPacketIdx = pStockOperateResult->wPacketIdx;
	StockOperateResult.wSubCommdID = pStockOperateResult->wSubCommdID;
	StockOperateResult.wStockCount = pStockOperateResult->wStockCount;
	CopyMemory(StockOperateResult.StockList, pStockOperateResult->StockList, sizeof(StockOperateResult.StockList[0]) * StockOperateResult.wStockCount);

	//�����С
	WORD wPacketSize = sizeof(StockOperateResult) - sizeof(StockOperateResult.StockList);
	wPacketSize += sizeof(StockOperateResult.StockList[0]) * StockOperateResult.wStockCount;

	//��������
	SendDataToGate(dwContextID, MDM_GP_STOCK_SERVICE, SUB_GP_STOCK_OPERATE_RESULT, &StockOperateResult, wPacketSize, &dwTokenID, 1);

	//�����ж�
	if (StockOperateResult.wStockCount == 1)
	{
		//���嶨��
		WORD wBufferSize = 0;
		BYTE cbBuffer[1024] = { 0 };

		//����ṹ
		tagForwardHead* pForwadHead = (tagForwardHead*)cbBuffer;

		//ת������
		pForwadHead->wItemCount = 1;
		pForwadHead->wTarget = FORWARD_TARGET_SERVICE;
		pForwadHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MONITOR;

		//��ӿ��
		if (StockOperateResult.wSubCommdID == SUB_GP_APPEND_STOCK_INFO)
		{
			//����ṹ
			auto pAppendStockItem = (CMD_CS_AppendStockItem*)(pForwadHead + 1);

			//���ñ���
			pAppendStockItem->szOperateID[0] = 0;
			pAppendStockItem->wStockID = StockOperateResult.StockList[0].wStockID;			

			//��������
			wBufferSize = sizeof(tagForwardHead) + sizeof(CMD_CS_AppendStockItem);
			m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_APPEND_STOCK_ITEM, cbBuffer, wBufferSize, dwTokenID);
		}

		//�޸Ŀ��
		if (StockOperateResult.wSubCommdID == SUB_GP_MODIFY_STOCK_INFO)
		{
			//����ṹ
			auto pModifyStockItem = (CMD_CS_ModifyStockItem*)(pForwadHead + 1);

			//���ñ���
			pModifyStockItem->szOperateID[0] = 0;
			pModifyStockItem->wStockID = StockOperateResult.StockList[0].wStockID;

			//��������
			wBufferSize = sizeof(tagForwardHead) + sizeof(CMD_CS_ModifyStockItem);
			m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_MODIFY_STOCK_ITEM, cbBuffer, wBufferSize, dwTokenID);
		}

		//ɾ�����
		if (StockOperateResult.wSubCommdID == SUB_GP_DELETE_STOCK_INFO)
		{
			//����ṹ
			auto pDeleteStockItem = (CMD_CS_DeleteStockItem*)(pForwadHead + 1);

			//����ṹ
			pDeleteStockItem->wStockID = StockOperateResult.StockList[0].wStockID;

			//��������
			wBufferSize = sizeof(tagForwardHead) + sizeof(CMD_CS_DeleteStockItem);
			m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_DELETE_STOCK_ITEM, cbBuffer, wBufferSize, dwTokenID);
		}
	}

	return true;
}

//��Ϸ����
bool CAttemperEngineSink::OnDBGameTypeItem(DWORD dwContextID,DWORD dwTokenID,VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GameType)==0);
	if (wDataSize%sizeof(DBO_GP_GameType)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GameType);
	DBO_GP_GameType * pGameType=(DBO_GP_GameType *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagGameType GameType;
		ZeroMemory(&GameType,sizeof(GameType));

		//��������
		GameType.wTypeID=(pGameType+i)->wTypeID;
		GameType.wJoinID=(pGameType+i)->wJoinID;
		GameType.wSortID=(pGameType+i)->wSortID;
		StringCchCopy(GameType.szTypeName,CountArray(GameType.szTypeName), (pGameType + i)->szTypeName);

		//�����б�
		m_ServerListManager.InsertGameType(&GameType);
	}

	return true;
}

//��Ϸ����
bool CAttemperEngineSink::OnDBGameKindItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GameKind)==0);
	if (wDataSize%sizeof(DBO_GP_GameKind)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GameKind);
	DBO_GP_GameKind * pGameKind=(DBO_GP_GameKind *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagGameKindEx GameKind;
		ZeroMemory(&GameKind,sizeof(GameKind));

		//��������
		GameKind.wModuleID = (pGameKind+i)->wModuleID;
		GameKind.wTypeID=(pGameKind+i)->wTypeID;
		GameKind.wJoinID=(pGameKind+i)->wJoinID;
		GameKind.wSortID=(pGameKind+i)->wSortID;
		GameKind.wKindID=(pGameKind+i)->wKindID;
		GameKind.dwOnLineCount=m_ServerListManager.CollectOnlineInfo((pGameKind+i)->wKindID);
		StringCchCopy(GameKind.szKindName,CountArray(GameKind.szKindName), (pGameKind + i)->szKindName);
		StringCchCopy(GameKind.szProcessName,CountArray(GameKind.szProcessName), (pGameKind + i)->szProcessName);

		//֧������
		GameKind.wSupportMask=(pGameKind+i)->wSupportMask;

		//�����б�
		if ((pGameKind + i)->bNullity == false)
		{
			m_ServerListManager.InsertGameKind(&GameKind);
		}		
		else
		{
			m_ServerListManager.DeleteGameKind(GameKind.wKindID);
		}
	}

	return true;
}

//��Ϸ�ڵ�
bool CAttemperEngineSink::OnDBGameNodeItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GameNode)==0);
	if (wDataSize%sizeof(DBO_GP_GameNode)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GameNode);
	DBO_GP_GameNode * pGameNode=(DBO_GP_GameNode *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagGameNode GameNode;
		ZeroMemory(&GameNode,sizeof(GameNode));

		//��������
		GameNode.wKindID=(pGameNode+i)->wKindID;
		GameNode.wJoinID=(pGameNode+i)->wJoinID;
		GameNode.wSortID=(pGameNode+i)->wSortID;
		GameNode.wNodeID=(pGameNode+i)->wNodeID;
		StringCchCopy(GameNode.szNodeName,CountArray(GameNode.szNodeName), (pGameNode + i)->szNodeName);

		//�����б�
		m_ServerListManager.InsertGameNode(&GameNode);
	}

	return true;
}

//��Ϸ����
bool CAttemperEngineSink::OnDBGamePageItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GameCustom)==0);
	if (wDataSize%sizeof(DBO_GP_GameCustom)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GameCustom);
	DBO_GP_GameCustom * pGameCustom=(DBO_GP_GameCustom *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagGameCustom GameCustom;
		ZeroMemory(&GameCustom,sizeof(GameCustom));

		//��������
		GameCustom.wKindID=(pGameCustom+i)->wKindID;
		GameCustom.wNodeID=(pGameCustom+i)->wNodeID;
		GameCustom.wSortID=(pGameCustom+i)->wSortID;
		GameCustom.wPageID=(pGameCustom+i)->wPageID;
		GameCustom.wOperateType=(pGameCustom+i)->wOperateType;
		StringCchCopy(GameCustom.szDisplayName,CountArray(GameCustom.szDisplayName), (pGameCustom + i)->szDisplayName);
		StringCchCopy(GameCustom.szReponseUrl,CountArray(GameCustom.szReponseUrl), (pGameCustom + i)->szReponseUrl);

		//�����б�
		m_ServerListManager.InsertGameCustom(&GameCustom);
	}

	return true;
}

//��Ϸ����
bool CAttemperEngineSink::OnDBGameFilterItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize%sizeof(DBO_GP_GameFilter)==0);
	if (wDataSize%sizeof(DBO_GP_GameFilter)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(DBO_GP_GameFilter);
	DBO_GP_GameFilter * pGameFilter=(DBO_GP_GameFilter *)pData;

	//��������
	for (WORD i=0;i<wItemCount;i++)
	{
		//��������
		tagGameFilter GameFilter;
		ZeroMemory(&GameFilter,sizeof(GameFilter));

		//��������
		GameFilter.wSortID=(pGameFilter+i)->wSortID;
		GameFilter.wNodeID=(pGameFilter+i)->wNodeID;
		GameFilter.wKindID=(pGameFilter+i)->wKindID;
		GameFilter.wServerID=(pGameFilter+i)->wServerID;
		GameFilter.wFilterMask=(pGameFilter+i)->wFilterMask;
		StringCchCopy(GameFilter.szServerName,CountArray(GameFilter.szServerName), (pGameFilter + i)->szServerName);

		//�����б�
		m_ServerListManager.InsertGameFilter(&GameFilter);
	}

	return true;
}

//���ý��
bool CAttemperEngineSink::OnDBConfigResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(DBO_GP_ConfigResult));
	if (wDataSize != sizeof(DBO_GP_ConfigResult)) return false;

	//��ȡ����
	DBO_GP_ConfigResult* pConfigResult = (DBO_GP_ConfigResult*)pData;

	//����ṹ
	CP_ControlResult ControlResult;
	ZeroMemory(&ControlResult, sizeof(ControlResult));

	//���ݴ���
	if (pConfigResult->bResultCode == true)
	{
		//�¼�֪ͨ
		ControlResult.cbSuccess = ER_SUCCESS;

		//�л�����
		if (pConfigResult->dwConfigMask & GLOBAL_OPTION_MASK)
		{
			g_GlobalOptionManager->SwitchOptionItem(pConfigResult->dwConfigMask);
		}
	}
	else
	{
		ControlResult.cbSuccess = ER_FAILURE;
	}

	//�¼�֪ͨ
	if (pConfigResult->bInitService == true)
	{
		SendUIControlPacket(UI_LOAD_CONFIG_RESULT, &ControlResult, sizeof(ControlResult));
	}

	return true;
}

//ȫ������
bool CAttemperEngineSink::OnDBGlobalOption(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	return true;
}

//���д���Ϣ
bool CAttemperEngineSink::OnDBFilterWordsInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	if (dwContextID == 0xffff)			//�������
	{
		return true;
	}
	else if (dwContextID == 0xfffe)		//��ʼ����
	{
		m_WordsFilter.ResetFilter();
		return true;
	}

	const TCHAR* pWords = (const TCHAR*)pData;
	m_WordsFilter.InsertKeyword(pWords);

	return true;
}

//������
bool CAttemperEngineSink::OnDBRankingListData(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pRankingListData = (DBO_GP_RankingListData*)pData;

	//��Ӱ�
	m_RankingListManager.AppendRankingList(pRankingListData->cbListType, CA2CT(pRankingListData->szRankingList).m_psz);

	return true;
}

//ת�˲���
bool CAttemperEngineSink::OnDBTransferParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize==sizeof(tagTransferParameter));
	if (wDataSize != sizeof(tagTransferParameter)) return true;

	//��������
	CopyMemory(&m_TransferParameter, pData, wDataSize);

	return true;
}

//�汾��Ϣ
bool CAttemperEngineSink::OnDBModuleVersionInfo(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��ȡ����
	ASSERT (wDataSize%sizeof(tagModuleVersion)==0);
	if (wDataSize%sizeof(tagModuleVersion)!=0) return false;

	//��������
	WORD wItemCount=wDataSize/sizeof(tagModuleVersion);
	tagModuleVersion * pModuleVersion=(tagModuleVersion *)pData;

	//�Ƴ��汾
	m_ModuleVersionActive.RemoveAll();

	//��������
	for (int i=0; i<wItemCount; i++)
	{
		m_ModuleVersionActive.Add(*pModuleVersion++);
	}

	return true;
}

//�󶨻���
bool CAttemperEngineSink::OnDBBindMachineResult(DWORD dwContextID,DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//�󶨲���
	tagBindParameter* pBindParameter = GetBindParameter(dwContextID);
	if (pBindParameter == NULL) return true;

	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��������
	DBO_GP_BindMachineResult * pBindMachineResult=(DBO_GP_BindMachineResult *)pData;

	//Ч�����
	ASSERT(wDataSize==sizeof(DBO_GP_BindMachineResult)-sizeof(pBindMachineResult->szDescribeString)+CountStringBuffer(pBindMachineResult->szDescribeString));
	if( wDataSize != sizeof(DBO_GP_BindMachineResult)-sizeof(pBindMachineResult->szDescribeString)+CountStringBuffer(pBindMachineResult->szDescribeString) )
		return false;

	//�ж�����
	ASSERT(LOWORD(dwContextID)< m_pServiceOption->wMaxConnect);
	if (pBindParameter->dwSocketID!=dwContextID) return true;

	if( pBindMachineResult->lResultCode == DB_SUCCESS )
	{
		//��������
		CMD_GP_OperateSuccess OperateSuccess;
		ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

		//��������
		OperateSuccess.lResultCode=pBindMachineResult->lResultCode;
		StringCchCopy(OperateSuccess.szDescribeString,CountArray(OperateSuccess.szDescribeString), pBindMachineResult->szDescribeString);

		//��������
		WORD wStringSize=CountStringBuffer(OperateSuccess.szDescribeString);
		WORD wSendSize=sizeof(OperateSuccess)-sizeof(OperateSuccess.szDescribeString)+wStringSize;
		SendDataToGate(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_OPERATE_SUCCESS,&OperateSuccess,wSendSize,&dwTokenID,1);
	}
	else
	{
		//��������
		CMD_GP_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//��������
		OperateFailure.lErrorCode=pBindMachineResult->lResultCode;
		StringCchCopy(OperateFailure.szDescribeString,CountArray(OperateFailure.szDescribeString), pBindMachineResult->szDescribeString);

		//��������
		WORD wStringSize=CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize=sizeof(OperateFailure)-sizeof(OperateFailure.szDescribeString)+wStringSize;
		SendDataToGate(dwContextID,MDM_GP_USER_SERVICE,SUB_GP_OPERATE_FAILURE,&OperateFailure,wSendSize,&dwTokenID,1);
	}

	return true;
}

//����ƹ���
bool CAttemperEngineSink::SendMarqeeNotice(BYTE cbNoticeKind, VOID* pNoticeContent, WORD wNoticeSize)
{
	//��������
	auto MarqueeNoticeOptionPtr = g_GlobalOptionManager->GetOptionItem<CMarqueeNoticeOptionPtr>(OPTION_MASK_MARQUEE_NOTICE);
	if (!MarqueeNoticeOptionPtr) return false;

	//��������
	auto pNoticeRollOption = MarqueeNoticeOptionPtr->GetNoticeRollOption(cbNoticeKind);
	if (!pNoticeRollOption) return false;

	//���嶨��
	BYTE cbBuffer[1024];

	//����ṹ
	tagForwardHead* pForwadHead = (tagForwardHead*)cbBuffer;
	auto pMarqueeNotice = (CMD_CS_MarqueeNotice*)(pForwadHead + 1);

	//ת������
	pForwadHead->wItemCount = 1;
	pForwadHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwadHead->ForwardItem[0].wServiceID = 0;
	pForwadHead->ForwardItem[0].wServiceKind = 0;
	pForwadHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;

	//��������
	pMarqueeNotice->dwNoticeID = GetTickCount();
	pMarqueeNotice->bExclusive = pNoticeRollOption->bExclusive;
	pMarqueeNotice->cbPriority = pNoticeRollOption->cbPriority;
	pMarqueeNotice->wRollTimes = pNoticeRollOption->wRollTimes;
	pMarqueeNotice->wViewPlace = pNoticeRollOption->wViewPlace;
	pMarqueeNotice->cbNoticeKind = pNoticeRollOption->cbNoticeKind;
	pMarqueeNotice->dwStartRollTime = (DWORD)time(NULL);
	pMarqueeNotice->dwEndRollTime = pMarqueeNotice->dwStartRollTime + MarqueeNoticeOptionPtr->GetDurationTime();

	//��������
	pMarqueeNotice->wNoticeSize = wNoticeSize;
	CopyMemory(pMarqueeNotice->cbNoticeContent, pNoticeContent, pMarqueeNotice->wNoticeSize);

	//��������
	WORD wPacketSize = sizeof(CMD_CS_MarqueeNotice) - sizeof(pMarqueeNotice->cbNoticeContent) + pMarqueeNotice->wNoticeSize;
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_MARQUEE_NOTICE, cbBuffer, sizeof(tagForwardHead) + wPacketSize,0);

	return true;
}

//�汾���
bool CAttemperEngineSink::CheckPlazaVersion(DWORD dwDeviceKind, DWORD dwPlazaVersion, DWORD dwSocketID, DWORD dwTokenID)
{
	//�汾�ж�
	bool bMustUpdate=false,bAdviceUpdate=false,bSpecialVersion=false;
	
	//��������
	DWORD dwClientVersion=0;
	tagModuleVersion * pModuleVersion=NULL;		

	//����汾
	pModuleVersion=GetSpecialVersion(0,dwDeviceKind,dwPlazaVersion);
	if (pModuleVersion!=NULL) bSpecialVersion=true;

	//��ǰ�汾
	pModuleVersion=GetCurrentVersion(0,dwDeviceKind);
	if (pModuleVersion!=NULL) dwClientVersion=pModuleVersion->dwVersion;

	//У��汾
	if (!bSpecialVersion) 
	{
		//�жϰ汾
		if (GetSubVer(dwPlazaVersion)<GetSubVer(dwClientVersion)) bAdviceUpdate=true;
		if (GetMainVer(dwPlazaVersion)!=GetMainVer(dwClientVersion)) bMustUpdate=true;
		if (GetProductVer(dwPlazaVersion)!=GetProductVer(dwClientVersion)) bMustUpdate=true;
	}

	//������ʾ
	if ((bMustUpdate==true)||(bAdviceUpdate==true))
	{
		//��������
		CMD_GP_UpdateNotify UpdateNotify;
		ZeroMemory(&UpdateNotify,sizeof(UpdateNotify));

		//��������
		UpdateNotify.cbMustUpdate=bMustUpdate;
		UpdateNotify.cbAdviceUpdate=bAdviceUpdate;
		UpdateNotify.dwCurrentVersion=dwClientVersion;

		//������Ϣ
		if (bMustUpdate || (bAdviceUpdate && !m_bUpdateSilent))
		{
			SendDataToGate(dwSocketID,MDM_GP_LOGON,SUB_GP_UPDATE_NOTIFY,&UpdateNotify,sizeof(UpdateNotify),&dwTokenID,1);
		}
		else
		{
			SendDataToGate(dwSocketID,MDM_GP_LOGON,SUB_GP_UPDATE_SILENT,&UpdateNotify,sizeof(UpdateNotify),&dwTokenID,1);
		}

		//�ж��ж�
		if (bMustUpdate==true) 
		{
			//����ṹ
			CMD_CS_C_ShutdownSocket ShutdownSocket;
			ShutdownSocket.bForceClose = false;

			//�ر�����
			SendDataToGate(dwSocketID,MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket),&dwTokenID,1);

			return false;
		}
	}

	return true;
}

//����ʧ��
VOID CAttemperEngineSink::SendOperateFailure(DWORD dwSocketID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe)
{
	//��������
	CMD_CM_OperateFailure * pRequestFailure = (CMD_CM_OperateFailure*)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer + sizeof(CMD_CM_OperateFailure), sizeof(m_cbBuffer) - sizeof(CMD_CM_OperateFailure));

	//���ñ���
	pRequestFailure->lErrorCode = lErrorCode;
	pRequestFailure->wRequestCmdID = wRequestCmdID;	

	//������Ϣ
	if (pszDescribe != NULL)  SendPacket.AddPacket(pszDescribe, DTP_CM_DESCRIBE);

	//�������
	WORD wDataSize = sizeof(CMD_CM_OperateFailure) + SendPacket.GetDataSize();
	SendDataToGate(dwSocketID, MDM_CM_SYSTEM, SUB_CM_OPERATE_FAILURE, pRequestFailure, wDataSize,&dwTokenID,1);

	return;
}

//�����ɹ�
VOID CAttemperEngineSink::SendOperateSuccess(DWORD dwSocketID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe, LPCTSTR pszGoodsList)
{
	//��������
	CMD_CM_OperateSuccess* pRequestSuccess = (CMD_CM_OperateSuccess*)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer + sizeof(CMD_CM_OperateSuccess), sizeof(m_cbBuffer) - sizeof(CMD_CM_OperateSuccess));
	
	//���ñ���
	pRequestSuccess->lErrorCode = lErrorCode;
	pRequestSuccess->wRequestCmdID = wRequestCmdID;
	
	//������Ϣ
	if (pszDescribe != NULL)  SendPacket.AddPacket(pszDescribe, DTP_CM_DESCRIBE);
	if (pszGoodsList != NULL)  SendPacket.AddPacket(pszGoodsList, DTP_CM_GOODSLIST);

	//�������
	WORD wDataSize = sizeof(CMD_CM_OperateSuccess) + SendPacket.GetDataSize();
	SendDataToGate(dwSocketID, MDM_CM_SYSTEM, SUB_CM_OPERATE_SUCCESS, pRequestSuccess, wDataSize,&dwTokenID,1);

	return;
}

//��������
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//��������
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->m_UIControlSink.PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}


//�����л�
VOID CAttemperEngineSink::OnEventDaySwitch()
{
	//��ȡʱ��
	SYSTEMTIME CurrSystemTime;
	GetLocalTime(&CurrSystemTime);

	//��������
	CDWordArray UserIDList;
	CString strUserIDList = TEXT("[");

	//��������		
	CGlobalUserItem* pGlobalUserItem = NULL;
	pGlobalUserItem = m_GlobalItemManager.EnumUserItem(NULL);

	//��������
	DBR_GP_ResetUserData ResetUserData;
	ZeroMemory(&ResetUserData, sizeof(ResetUserData));

	//��������
	while (pGlobalUserItem != NULL)
	{
		//����û�		
		UserIDList.Add(pGlobalUserItem->GetUserID());

		if (strUserIDList != TEXT("[")) strUserIDList.AppendChar(TEXT(','));
		strUserIDList.AppendFormat(TEXT("%d"), pGlobalUserItem->GetUserID());

		//��ȡ����
		CGlobalUserItem* pGlobalUserCurrent = pGlobalUserItem;
		pGlobalUserItem = m_GlobalItemManager.EnumUserItem(pGlobalUserCurrent);		

		//��������
		if (strUserIDList.GetLength()+30 >= CountArray(ResetUserData.szUserIDList))
		{
			//��ӻ�����
			strUserIDList.Append(TEXT("]"));

			//��������
			ResetUserData.wDayOfWeek = CurrSystemTime.wDayOfWeek;
			StringCchCopy(ResetUserData.szUserIDList, CountArray(ResetUserData.szUserIDList), strUserIDList);

			//Ͷ������
			WORD wDataSize = CountStringBuffer(ResetUserData.szUserIDList);
			WORD wHeadSize = sizeof(ResetUserData) - sizeof(ResetUserData.szUserIDList);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_RESET_USER_DATA, 0L, 0L, 0L, &ResetUserData, wHeadSize + wDataSize);

			//���ñ���
			strUserIDList = TEXT("[");
		}
	}

	//����ʣ��
	if (strUserIDList!=TEXT("["))
	{
		//��ӻ�����
		strUserIDList.Append(TEXT("]"));
		
		//��������
		DBR_GP_ResetUserData ResetUserData;
		ZeroMemory(&ResetUserData, sizeof(ResetUserData));

		//��������
		ResetUserData.wDayOfWeek = CurrSystemTime.wDayOfWeek;
		StringCchCopy(ResetUserData.szUserIDList, CountArray(ResetUserData.szUserIDList), strUserIDList);

		//Ͷ������
		WORD wDataSize = CountStringBuffer(ResetUserData.szUserIDList);
		WORD wHeadSize = sizeof(ResetUserData) - sizeof(ResetUserData.szUserIDList);
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_RESET_USER_DATA, 0L, 0L, 0L, &ResetUserData, wHeadSize + wDataSize);
	}

	//��Ϣ֪ͨ
	if (UserIDList.GetCount() > 0)
	{
		//����ṹ
		CMD_MB_ResetUserData ResetUserData;
		ZeroMemory(&ResetUserData, sizeof(ResetUserData));

		//���ñ���
		ResetUserData.wDayID = CurrSystemTime.wDay;
		ResetUserData.wDayOfWeek = CurrSystemTime.wDayOfWeek;

		//����֪ͨ
		for (auto i = 0; i < UserIDList.GetCount(); i++)
		{
			auto pGlobalUserItem = m_GlobalItemManager.SearchUserItem(UserIDList[i]);
			if (pGlobalUserItem != NULL)
			{
				//��������				
				auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGlobalUserItem->GetGateID());
				if (pGlobalGateItem != NULL)
				{
					DWORD dwTokenID = pGlobalUserItem->GetTokenID();
					SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_MB_USER_SERVICE, SUB_MB_RESET_USERDATA, &ResetUserData, sizeof(ResetUserData),&dwTokenID,1);
				}				
			}
		}
	}	
}

//��������
VOID CAttemperEngineSink::SendGameTypeInfo(DWORD dwSocketID,DWORD dwTokenID)
{
	//��������
	WORD wSendSize=0;
	POSITION Position=NULL;
	CGameTypeItem * pGameTypeItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameTypeCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameType))>sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_TYPE,m_cbBuffer,wSendSize,&dwTokenID,1);
			wSendSize=0;
		}

		//��ȡ����
		pGameTypeItem=m_ServerListManager.EmunGameTypeItem(Position);
		if (pGameTypeItem==NULL) break;

		//��������
		CopyMemory(m_cbBuffer+wSendSize,&pGameTypeItem->m_GameType,sizeof(tagGameType));
		wSendSize+=sizeof(tagGameType);
	}

	//����ʣ��
	if (wSendSize>0) SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_TYPE,m_cbBuffer,wSendSize,&dwTokenID,1);

	return;
}

//��������
VOID CAttemperEngineSink::SendGameKindInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wSupportMask)
{
	//��������
	WORD wSendSize=0;
	POSITION Position=NULL;
	tagGameKind * pGameKind=NULL;
	CGameKindItem * pGameKindItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameKindCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameKind))>sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_KIND,m_cbBuffer,wSendSize, &dwTokenID, 1);
			wSendSize=0;
		}

		//��ȡ����
		pGameKindItem=m_ServerListManager.EmunGameKindItem(Position);
		if (pGameKindItem==NULL) break;

		//��������
		if ((pGameKindItem->m_GameKind.wSupportMask&wSupportMask)==0) continue;

		//��ȡ����
		pGameKind=(tagGameKind *)(m_cbBuffer+wSendSize);

		//��������
		CopyMemory(pGameKind, &pGameKindItem->m_GameKind, sizeof(tagGameKind));

		//��������
		pGameKind->dwOnLineCount=pGameKindItem->m_dwOnLineCount;
		wSendSize+=sizeof(tagGameKind);
	}

	//����ʣ��
	if (wSendSize>0) SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_KIND,m_cbBuffer,wSendSize,&dwTokenID,1);

	return;
}

//���ͽڵ�
VOID CAttemperEngineSink::SendGameNodeInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wKindID)
{
	//��������
	WORD wSendSize=0;
	POSITION Position=NULL;
	CGameNodeItem * pGameNodeItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameNodeCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameNode))>sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_NODE,m_cbBuffer,wSendSize,&dwTokenID,1);
			wSendSize=0;
		}

		//��ȡ����
		pGameNodeItem=m_ServerListManager.EmunGameNodeItem(Position);
		if (pGameNodeItem==NULL) break;

		//��������
		if (pGameNodeItem->m_GameNode.wKindID==wKindID)
		{
			CopyMemory(m_cbBuffer+wSendSize,&pGameNodeItem->m_GameNode,sizeof(tagGameNode));
			wSendSize+=sizeof(tagGameNode);
		}
	}

	//����ʣ��
	if (wSendSize>0) SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_NODE,m_cbBuffer,wSendSize,&dwTokenID,1);

	return;
}

//���ͷ���
VOID CAttemperEngineSink::SendGameServerInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wKindID, WORD wClientMask)
{
	//��������
	WORD wSendSize=0;
	POSITION Position=NULL;
	tagPCGameServer PCGameServer={0};
	CGameServerItem * pGameServerItem=NULL;

	

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameServerCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameServer))>sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_SERVER,m_cbBuffer,wSendSize,&dwTokenID,1);
			wSendSize=0;
		}

		//��ȡ����
		pGameServerItem=m_ServerListManager.EmunGameServerItem(Position);
		if (pGameServerItem==NULL) break;

		//��������
		if ((pGameServerItem->m_GameServer.wKindID==wKindID && pGameServerItem->m_GameServer.wVisibleMask&wClientMask) 
			|| wKindID==0 )
		{
			//����ṹ
			PCGameServer.wKindID=pGameServerItem->m_GameServer.wKindID;	
			PCGameServer.wNodeID=pGameServerItem->m_GameServer.wNodeID;	
			PCGameServer.wSortID=pGameServerItem->m_GameServer.wSortID;	
			PCGameServer.wServerID=pGameServerItem->m_GameServer.wServerID;
			PCGameServer.wServerPort=pGameServerItem->m_GameServer.wServerPort;
			PCGameServer.wServerType=pGameServerItem->m_GameServer.wServerType;
			PCGameServer.dwOnLineCount=pGameServerItem->m_GameServer.dwOnLineCount;
			PCGameServer.dwServerAddr=pGameServerItem->m_GameServer.dwServerAddr;
			StringCchCopy(PCGameServer.szServerName,CountArray(PCGameServer.szServerName), pGameServerItem->m_GameServer.szServerName);

			//������Ϣ
			PCGameServer.lCellScore=pGameServerItem->m_GameServer.lCellScore;
			PCGameServer.lMinEnterScore=pGameServerItem->m_GameServer.lMinEnterScore;
			PCGameServer.lMaxEnterScore=pGameServerItem->m_GameServer.lMaxEnterScore;

			//��������
			CopyMemory(m_cbBuffer+wSendSize,&PCGameServer,sizeof(tagPCGameServer));
			wSendSize+=sizeof(tagPCGameServer);
		}
	}

	//����ʣ��
	if (wSendSize>0) SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_SERVER,m_cbBuffer,wSendSize,&dwTokenID,1);

	return;
}

//���Ͷ���
VOID CAttemperEngineSink::SendGamePageInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wKindID)
{
	//��������
	WORD wSendSize=0;
	POSITION Position=NULL;
	CGameCustomItem * pGameCustomItem=NULL;

	//ö������
	for (DWORD i=0;i<m_ServerListManager.GetGameCustomCount();i++)
	{
		//��������
		if ((wSendSize+sizeof(tagGameCustom))>sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_CUSTOM,m_cbBuffer,wSendSize,&dwTokenID,1);
			wSendSize=0;
		}

		//��ȡ����
		pGameCustomItem=m_ServerListManager.EmunGameCustomItem(Position);
		if (pGameCustomItem==NULL) break;

		//��������
		if (pGameCustomItem->m_GameCustom.wKindID==wKindID)
		{
			CopyMemory(m_cbBuffer+wSendSize,&pGameCustomItem->m_GameCustom,sizeof(tagGameCustom));
			wSendSize+=sizeof(tagGameCustom);
		}
	}

	//����ʣ��
	if (wSendSize>0) SendDataToGate(dwSocketID,MDM_GP_SERVER_LIST,SUB_GP_LIST_CUSTOM,m_cbBuffer,wSendSize,&dwTokenID,1);

	return;
}

//��������
VOID CAttemperEngineSink::SendMobileKindInfo(DWORD dwSocketID, DWORD dwTokenID, tagExtendParemter* pExtendParemter)
{
	//��������
	WORD wSendSize=0;
	bool bShielded = false;
	POSITION Position=NULL;
	CGameKindItem * pGameKindItem=NULL;

	do
	{
		//��ȡ����
		pGameKindItem = m_ServerListManager.EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//��������
		if ((wSendSize + sizeof(tagGameKind)) > sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_KIND, m_cbBuffer, wSendSize,&dwTokenID,1);
			wSendSize = 0;
		}
			
		//֧�ֹ���
		if ((pGameKindItem->m_GameKind.wSupportMask & SUPPORT_MASK_MOBILE) == 0) continue;
		
		//���ñ�ʶ
		bShielded = false;

		//�������� 
		for (int i = 0; i < pExtendParemter->wKindCount; i++)
		{
			if (pGameKindItem->m_GameKind.wKindID == pExtendParemter->wShieldKindList[i])
			{
				bShielded = true;
				break;
			}
		}
		CString s;
		s.Format(_T("wKindID:%d,wModuleID:%d,bShielded:%d"), pGameKindItem->m_GameKind.wKindID, pGameKindItem->m_GameKind.wModuleID, bShielded);
		CTraceService::TraceString(s, TraceLevel_Exception);

		//�����ж�
		if (bShielded == false)
		{
			//��������
			CopyMemory(m_cbBuffer + wSendSize, &pGameKindItem->m_GameKind, sizeof(tagGameKind));
			wSendSize += sizeof(tagGameKind);
		}

	} while (Position!=NULL);

	//����ʣ��
	if (wSendSize>0) SendDataToGate(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_KIND,m_cbBuffer,wSendSize,&dwTokenID,1);

	return;
}

//��������
VOID CAttemperEngineSink::SendMobileOnlineInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wModuleID, tagExtendParemter* pExtendParemter)
{
	//��������
	WORD wSendSize = 0;
	bool bShielded = false;
	POSITION Position = NULL;
	CGameServerItem* pGameServerItem = NULL;

	//ö�ٷ���
	do
	{
		pGameServerItem = m_ServerListManager.EmunGameServerItem(Position);
		if (pGameServerItem == NULL) break;

		//��������
		if ((wSendSize + sizeof(tagOnlineInfo)) > sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_ONLINE, m_cbBuffer, wSendSize,&dwTokenID,1);
			wSendSize = 0;
		}	

		//���ñ�ʶ
		bShielded = false;

		//�������� 
		for (int i = 0; i < pExtendParemter->wKindCount; i++)
		{
			if (pGameServerItem->m_GameServer.wKindID == pExtendParemter->wShieldKindList[i])
			{
				bShielded = true;
				break;
			}
		}

		//��������
		if (bShielded==false && pGameServerItem->m_GameServer.wVisibleMask & MASK_VISIBLE_MOBILE &&
			(pGameServerItem->m_GameServer.wKindID == wModuleID || wModuleID == INVALID_WORD))
		{
			//��ȡ����
			auto pOnlineInfo = (tagOnlineInfo*)(m_cbBuffer+wSendSize);

			//������Ϣ
			pOnlineInfo->wServerID = pGameServerItem->m_GameServer.wServerID;
			pOnlineInfo->dwOnlineCount = pGameServerItem->m_GameServer.dwOnLineCount;

			//��������			
			wSendSize += sizeof(tagOnlineInfo);
		}

	} while (Position != NULL);

	//����ʣ��
	if (wSendSize > 0) SendDataToGate(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_ONLINE, m_cbBuffer, wSendSize,&dwTokenID,1);

	return;
}

//���ͽ���
VOID CAttemperEngineSink::SendMobileJackpotInfo(DWORD dwSocketID, DWORD dwTokenID, tagExtendParemter* pExtendParemter)
{
	//��������
	WORD wSendSize = 0;
	SCORE lJacketScore = 0;
	POSITION Position = NULL;
	CGameKindItem* pGameKindItem = NULL;

	do
	{
		//��ȡ����
		pGameKindItem = m_ServerListManager.EmunGameKindItem(Position);
		if (pGameKindItem == NULL) break;

		//��������
		if ((wSendSize + sizeof(tagJackpotInfo)) > sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_JACKPOT, m_cbBuffer, wSendSize, &dwTokenID, 1);
			wSendSize = 0;
		}

		if (pGameKindItem->m_ulLastTickCount > 0)
		{
			//��ȡ����
			auto pJackpotInfo = (tagJackpotInfo*)(m_cbBuffer + wSendSize);

			//������Ϣ
			pJackpotInfo->wKindID = pGameKindItem->m_GameKind.wKindID;
			pJackpotInfo->lJackpotScore = pGameKindItem->m_lJackpotScore;

			//��������			
			wSendSize += sizeof(tagJackpotInfo);
		}

	} while (Position != NULL);

	//��������
	if (wSendSize > 0) SendDataToGate(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_JACKPOT, m_cbBuffer, wSendSize, &dwTokenID, 1);

	return;
}

//���ͷ���
VOID CAttemperEngineSink::SendMobileServerInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wModuleID, tagExtendParemter* pExtendParemter)
{
	//��������
	WORD wSendSize=0;
	bool bShielded = false;
	POSITION Position=NULL;
	CGameServerItem * pGameServerItem=NULL;
	tagMBGameServer MBGameServer={0};	

	do
	{
		//��ȡ����
		pGameServerItem = m_ServerListManager.EmunGameServerItem(Position);
		if (pGameServerItem == NULL) break;

		//��������
		if ((wSendSize + sizeof(tagMBGameServer)) > sizeof(m_cbBuffer))
		{
			SendDataToGate(dwSocketID, MDM_MB_SERVER_LIST, SUB_MB_LIST_SERVER, m_cbBuffer, wSendSize,&dwTokenID,1);
			wSendSize = 0;
		}

		//���ñ�ʶ
		bShielded = false;

		//�������� 
		for (int i = 0; i < pExtendParemter->wKindCount; i++)
		{
			if (pGameServerItem->m_GameServer.wKindID == pExtendParemter->wShieldKindList[i])
			{
				bShielded = true;
				break;
			}
		}

		//��������
		if (bShielded==false && pGameServerItem->m_GameServer.wVisibleMask & MASK_VISIBLE_MOBILE &&
			(pGameServerItem->m_GameServer.wKindID == wModuleID || wModuleID == INVALID_WORD))
		{
			//����ṹ
			MBGameServer.wKindID = pGameServerItem->m_GameServer.wKindID;
			MBGameServer.wSortID = pGameServerItem->m_GameServer.wSortID;
			MBGameServer.wServerID = pGameServerItem->m_GameServer.wServerID;
			MBGameServer.wServerPort = pGameServerItem->m_GameServer.wServerPort;
			MBGameServer.wServerType = pGameServerItem->m_GameServer.wServerType;
			MBGameServer.wServerLevel = pGameServerItem->m_GameServer.wServerLevel;
			MBGameServer.wChairCount = pGameServerItem->m_GameServer.wChairCount;
			MBGameServer.dwMaxPlayer = pGameServerItem->m_GameServer.dwMaxPlayer;
			MBGameServer.dwOnLineCount = pGameServerItem->m_GameServer.dwOnLineCount;
			MBGameServer.dwServerAddr = pGameServerItem->m_GameServer.dwServerAddr;
			StringCchCopy(MBGameServer.szServerName, CountArray(MBGameServer.szServerName), pGameServerItem->m_GameServer.szServerName);

			//��������
			StringCchCopy(MBGameServer.szServerDomain, CountArray(MBGameServer.szServerDomain), pGameServerItem->m_GameServer.szServerDomain);

			//������Ϣ
			MBGameServer.lCellScore = pGameServerItem->m_GameServer.lCellScore;
			MBGameServer.lMinEnterScore = pGameServerItem->m_GameServer.lMinEnterScore;
			MBGameServer.lMaxEnterScore = pGameServerItem->m_GameServer.lMaxEnterScore;

			//��������
			CopyMemory(m_cbBuffer + wSendSize, &MBGameServer, sizeof(tagMBGameServer));
			wSendSize += sizeof(tagMBGameServer);
		}

	} while (Position!=NULL);

	//����ʣ��
	if (wSendSize>0) SendDataToGate(dwSocketID,MDM_MB_SERVER_LIST,SUB_MB_LIST_SERVER, m_cbBuffer,wSendSize,&dwTokenID,1);

	return;
}

//���ͱ���
VOID CAttemperEngineSink::SendUserPackGoods(DWORD dwSocketID, DWORD dwTokenID, tagGoodsHoldInfo GoodsHoldInfo[], WORD wGoodsCount)
{
	//�������
	auto pPackGoods = (CMD_CM_PackGoods*)m_cbBuffer;
	ZeroMemory(pPackGoods, sizeof(CMD_CM_PackGoods));

	//���ñ���
	pPackGoods->wGoodsCount = wGoodsCount;
	CopyMemory(pPackGoods->GoodsHoldInfo, GoodsHoldInfo, pPackGoods->wGoodsCount * sizeof(pPackGoods->GoodsHoldInfo[0]));

	//��������
	WORD wPacketSize = sizeof(CMD_CM_GoodsUpdate) + pPackGoods->wGoodsCount * sizeof(pPackGoods->GoodsHoldInfo[0]);
	SendDataToGate(dwSocketID, MDM_CM_SYSTEM, SUB_CM_PACK_GOODS, pPackGoods, wPacketSize,&dwTokenID,1);
}
//�����ǳ�����
VOID CAttemperEngineSink::SendNickNameOption(DWORD dwSocketID, DWORD dwTokenID, WORD wModifyTime, WORD wDailyModifyTimes)
{
	//�������
	auto pHead = (CMD_CM_ConfigUpdate*)m_cbBuffer;
	ZeroMemory(pHead, sizeof(CMD_CM_ConfigUpdate));

	//���ñ���
	pHead->dwType = CM_CONFIG_TYPE_NICKNAME;

	CMD_CM_Config_NickName* pData = (CMD_CM_Config_NickName*)(pHead + 1);
	pData->wFreeModifyTimes = m_NickNameOption.wFreeModifyTimes;
	pData->wDailyModifyTimes = m_NickNameOption.wDailyModifyTimes;
	pData->dwGoodsID = m_NickNameOption.dwGoodsID;
	pData->dwGoodsCount = m_NickNameOption.dwGoodsCount;
	//
	if (wModifyTime >= pData->wFreeModifyTimes)
		pData->wFreeModifyTimes = 0;
	else
		pData->wFreeModifyTimes -= wModifyTime;


	//��������
	WORD wPacketSize = sizeof(CMD_CM_ConfigUpdate) + sizeof(CMD_CM_Config_NickName);
	SendDataToGate(dwSocketID, MDM_CM_SYSTEM, SUB_CM_CONFIG_UPDATE, pHead, wPacketSize, &dwTokenID, 1);
};
//�û�ͷ��
bool CAttemperEngineSink::OnDBMBUserFaceInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��ȡ����
	auto pUserFaceInfo = (DBO_MB_UserFaceInfo*)pData;

	//��������
	CMD_MB_UserFaceInfo UserFaceInfo;
	ZeroMemory(&UserFaceInfo, sizeof(UserFaceInfo));	

	//���ñ���
	UserFaceInfo.wFaceID = pUserFaceInfo->wFaceID;
	UserFaceInfo.dwCustomID = pUserFaceInfo->dwCustomID;

	//������Ϣ
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_USER_FACE_INFO, &UserFaceInfo, sizeof(UserFaceInfo),&dwTokenID,1);

	return true;
}

//�û�����
bool CAttemperEngineSink::OnDBMBUserCacheInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��������
	UserPublicInfo CachePublicInfo;
	if (CachePublicInfo.ParseFromArray((char*)pData, wDataSize))
	{	
		//����ṹ
		CMD_CS_C_UserUpdate UserUpdate = {};

		//���ñ���
		UserUpdate.dwUserID=CachePublicInfo.uid();

		//�û��ǳ�
		if (CachePublicInfo.has_nickname())
		{
			UserUpdate.dwInfoMask |= INFO_MASK_NICKNAME;

			//ת���ǳ�
			TCHAR szNickName[LEN_NICKNAME] = {0};
			CWHService::Utf82Unicode(CachePublicInfo.nickname().c_str(), szNickName,CountArray(szNickName));

			//�����ǳ�
			StringCchCopy(UserUpdate.szNickName,CountArray(UserUpdate.szNickName), szNickName);
		}

		//ͷ���ʶ
		if ((CachePublicInfo.has_faceid() && CachePublicInfo.has_customid()))
		{
			UserUpdate.dwInfoMask |= INFO_MASK_FACEID;
			UserUpdate.wFaceID= CachePublicInfo.faceid();
			UserUpdate.dwCustomID=CachePublicInfo.customid();
		}

		//���ݸ���
		m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_USER_UPDATE, &UserUpdate, sizeof(UserUpdate), 0);
	}

	//������Ϣ
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_USER_CACHE_INFO, pData, wDataSize,&dwTokenID,1);

	return true;
}

//�һ���Ʒ
bool CAttemperEngineSink::OnDBMBExchangeMallGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��ȡ����
	auto pExchangeMallGoods = (DBO_MB_ExchangeMallGoods*)pData;

	//������Ʒ
	tagGoodsItem GoodsItem = {};	
	GoodsItem.wGoodsID = pExchangeMallGoods->wIncGoodsID;
	GoodsItem.dwGoodsCount = pExchangeMallGoods->dwIncGoodsCount;
	GoodsItem.dwGoodsIndate = pExchangeMallGoods->dwIncGoodsIndate;

	//�����б�
	TCHAR szGoodsList[128];
	g_GameGoodsManager->SerializeToJson(&GoodsItem,1, szGoodsList, CountArray(szGoodsList));

	//�����ɹ�
	SendOperateSuccess(dwContextID, dwTokenID, SUB_MB_EXCHANGE_MALLGOODS, pExchangeMallGoods->lResultCode, pExchangeMallGoods->szDescribeString, szGoodsList);

	return true;
}

//�һ������
bool CAttemperEngineSink::OnDBMBExchangeGiftbagCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��ȡ����
	auto pExchangeGiftbagCode = (DBO_MB_ExchangeGiftbagCode*)pData;

	//�����ɹ�
	SendOperateSuccess(dwContextID, dwTokenID, SUB_MB_EXCHANGE_GIFTBAGCODE, pExchangeGiftbagCode->lResultCode, pExchangeGiftbagCode->szDescribeString, pExchangeGiftbagCode->szGiftBagGoodsList);

	return true;
}


//ǩ�����
bool CAttemperEngineSink::OnDBMBCheckInResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��ȡ����
	auto pCheckInResult = (DBO_MB_CheckInResult*)pData;

	//������
	CMD_MB_CheckInResult CheckInResult = {};

	//���ñ���
	CheckInResult.wCheckInDays = pCheckInResult->wCheckInDays;
	CheckInResult.cbCheckInState = pCheckInResult->cbCheckInState;

	//���ͽ��
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_CHECKIN_RESULT, &CheckInResult, sizeof(CheckInResult), &dwTokenID, 1);

	//�����ɹ�
	SendOperateSuccess(dwContextID, dwTokenID, SUB_MB_REQUEST_CHECKIN, 0, TEXT(""), pCheckInResult->szRewardGoodsList);

	return true;
}

//��������ɹ�
bool CAttemperEngineSink::OnDBMBBindInviteCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
    //�ж�����
    auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
    if (pTokenParameter == NULL) return true;

    //Ч�����
    ASSERT(wDataSize <= sizeof(DBO_MB_BindInviteCode));
    if (wDataSize > sizeof(DBO_MB_BindInviteCode)) return false;

    //��ȡ����
	auto  pBindInviteCode = (DBO_MB_BindInviteCode*)pData;

    //��������
    CMD_MB_BindInviteCodeSuccess BindInviteCodeSuccess;
    ZeroMemory(&BindInviteCodeSuccess, sizeof(BindInviteCodeSuccess));

	//��������
	BindInviteCodeSuccess.dwParentGameID = pBindInviteCode->dwParentGameID;
	BindInviteCodeSuccess.wParentFaceID = pBindInviteCode->wParentFaceID;
    StringCchCopy(BindInviteCodeSuccess.szParentNickName, CountArray(BindInviteCodeSuccess.szParentNickName), pBindInviteCode->szParentNickName);
    StringCchCopy(BindInviteCodeSuccess.szNoticeString, CountArray(BindInviteCodeSuccess.szNoticeString), pBindInviteCode->szNoticeString);

    //��������
    WORD wStringSize = CountStringBuffer(BindInviteCodeSuccess.szNoticeString);
    WORD wPacketSize = sizeof(BindInviteCodeSuccess) - sizeof(BindInviteCodeSuccess.szNoticeString) + wStringSize;
    SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_BIND_INVITE_CODE_SUCCESS, &BindInviteCodeSuccess, wPacketSize,&dwTokenID,1);

	//�������
	BYTE cbBuffer[256];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//��ȡ����
	tagForwardHead* pForwardHead = (tagForwardHead*)cbBuffer;

	//����ת��ͷ
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwardHead->dwUserID = pBindInviteCode->dwUserID;

	//ת����Ϣ
	pForwardHead->wItemCount = 1;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;

	//ת������				
	CMD_CS_MailNotify* pMailNotify = (CMD_CS_MailNotify*)(pForwardHead + 1);

	//�����ʼ�
	pMailNotify->dwUserID = pBindInviteCode->dwUserID;
	pMailNotify->dwMailID = pBindInviteCode->dwMailID;

	//��������
	wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_MailNotify);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_MAIL_NOTIFY, cbBuffer, wPacketSize, 0);

    return true;
}

//���˺�
bool CAttemperEngineSink::OnDBMBBindExternalAccount(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//Ч�����
	ASSERT(wDataSize <= sizeof(DBO_MB_BindExternalAccount));
	if (wDataSize > sizeof(DBO_MB_BindExternalAccount)) return false;

	//��ȡ����
	auto pBindExternalAccount = (DBO_MB_BindExternalAccount*)pData;

	//��������
	CMD_MB_BindExternalAccountSuccess BindExternalAccountSuccess;
	ZeroMemory(&BindExternalAccountSuccess, sizeof(BindExternalAccountSuccess));

	//��������
	BindExternalAccountSuccess.dwExternalID = pBindExternalAccount->dwExternalID;
	StringCchCopy(BindExternalAccountSuccess.szUniqueID, CountArray(BindExternalAccountSuccess.szUniqueID), pBindExternalAccount->szUniqueID);
	StringCchCopy(BindExternalAccountSuccess.szNoticeString, CountArray(BindExternalAccountSuccess.szNoticeString), pBindExternalAccount->szNoticeString);

	//��������
	WORD wStringSize = CountStringBuffer(BindExternalAccountSuccess.szNoticeString);
	WORD wPacketSize = sizeof(BindExternalAccountSuccess) - sizeof(BindExternalAccountSuccess.szNoticeString) + wStringSize;
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_BIND_EXTERNAL_ACCOUNT_SUCCESS, &BindExternalAccountSuccess, wPacketSize, &dwTokenID, 1);

	return true;
}

//��������ɹ�
bool CAttemperEngineSink::OnDBMBSendGiftSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
    //�ж�����
    auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
    if (pTokenParameter == NULL) return true;

    //Ч�����
    ASSERT(wDataSize <= sizeof(DBO_MB_SendGiftSuccess));
    if (wDataSize > sizeof(DBO_MB_SendGiftSuccess)) return false;

    //��ȡ����
    auto  pSendGiftSuccess = (DBO_MB_SendGiftSuccess*)pData;

	//���ͳɹ�
	CMD_MB_SendGiftSuccess SendGiftSuccess;
	ZeroMemory(&SendGiftSuccess, sizeof(SendGiftSuccess));
    SendGiftSuccess.dwTargetUserID = pSendGiftSuccess->dwTargetUserID;
    SendGiftSuccess.dwGoodsID = pSendGiftSuccess->dwGoodsID;
    SendGiftSuccess.dwGoodsCount = pSendGiftSuccess->dwGoodsCount;
    SendGiftSuccess.dwLoveLiness = pSendGiftSuccess->dwLoveLiness;
    SendGiftSuccess.cbSendType = pSendGiftSuccess->cbSendType;

	//������Ϣ
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_SEND_GIFT_SUCCESS, &SendGiftSuccess, sizeof(SendGiftSuccess), &dwTokenID, 1);

    //����ֵ���
    CMD_CM_LoveLinessUpdate LoveLinessUpdate;
    ZeroMemory(&LoveLinessUpdate, sizeof(LoveLinessUpdate));
    LoveLinessUpdate.dwLoveLiness = pSendGiftSuccess->dwMyLoveLiness;

    //��������
    SendDataToGate(dwContextID, MDM_CM_SYSTEM, SUB_CM_LOVELINESS_UPDATE, &LoveLinessUpdate, sizeof(LoveLinessUpdate), &dwTokenID, 1);

	//Ⱥ��������
	if (pSendGiftSuccess->cbSendType == 0)
	{
		//������Ϣ
		CMD_MB_GlobalGiftNotify GlobalGiftNotify;
        ZeroMemory(&GlobalGiftNotify, sizeof(GlobalGiftNotify));
        GlobalGiftNotify.dwUserID = pSendGiftSuccess->dwUserID;
        GlobalGiftNotify.dwTargetUserID = pSendGiftSuccess->dwTargetUserID;
        GlobalGiftNotify.dwGoodsID = pSendGiftSuccess->dwGoodsID;
		GlobalGiftNotify.dwGoodsCount = pSendGiftSuccess->dwGoodsCount;
		GlobalGiftNotify.dwLoveLiness = pSendGiftSuccess->dwLoveLiness;
		GlobalGiftNotify.cbSendType = pSendGiftSuccess->cbSendType;

		//Ⱥ������
		SendBatchData(MDM_MB_USER_SERVICE, SUB_MB_GLOBAL_GIFT_NOTIFY, &GlobalGiftNotify, sizeof(GlobalGiftNotify));
	}

    //����Ŀ���û�
    auto pGlobalUserItem = m_GlobalItemManager.SearchUserItem(pSendGiftSuccess->dwTargetUserID);
	if (pGlobalUserItem != NULL)
	{
		//��������
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGlobalUserItem->GetGateID());
		if (pGlobalGateItem != NULL)
		{
			//֪ͨ�Է�
			SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_MB_USER_SERVICE, SUB_MB_RECV_GIFT_NOTIFY, NULL, 0,&pGlobalUserItem->m_dwTokenID,1);

			//�Է�����ֵ
			ZeroMemory(&LoveLinessUpdate, sizeof(LoveLinessUpdate));
			LoveLinessUpdate.dwLoveLiness = pSendGiftSuccess->dwTargetLoveLiness;

			//��������
			SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_CM_SYSTEM, SUB_CM_LOVELINESS_UPDATE, &LoveLinessUpdate, sizeof(LoveLinessUpdate),&pGlobalUserItem->m_dwTokenID,1);
		}		
	}	
	
	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBMBUserGiftInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
    //��ȡ����
    auto pDBUserGiftInfo = (DBO_MB_UserGiftInfo*)pData;

    //��ȡ����
    auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
    if (pTokenParameter == NULL) return true;

    //�������
	CMD_MB_UserGiftInfo* pUserGiftInfo = (CMD_MB_UserGiftInfo*)m_cbBuffer;
    ZeroMemory(pUserGiftInfo, sizeof(CMD_MB_UserGiftInfo));

    //��������		
    pUserGiftInfo->wItemCount = pDBUserGiftInfo->wItemCount;
    if (pUserGiftInfo->wItemCount > 0)
    {
        CopyMemory(pUserGiftInfo->GiftInfo, pDBUserGiftInfo->GiftInfo, pUserGiftInfo->wItemCount * sizeof(pUserGiftInfo->GiftInfo[0]));
    }

    //��������
    WORD wPacketSize = sizeof(CMD_MB_UserGiftInfo) + pUserGiftInfo->wItemCount * sizeof(pUserGiftInfo->GiftInfo[0]);
    SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_GIFT_INFO, pUserGiftInfo, wPacketSize,&dwTokenID,1);

    return true;
}

//������Ϣ���
bool CAttemperEngineSink::OnDBMBUserGiftInfoFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
    //��ȡ����
    auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
    if (pTokenParameter == NULL) return true;

    //��������
    SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_GIFT_INFO_FINISH, NULL, 0, &dwTokenID, 1);

	return true;
}

//ת�˳ɹ�
bool CAttemperEngineSink::OnDBMBWealthTransferSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//Ч�����
	ASSERT (wDataSize == sizeof(DBO_MB_WealthTransferSuccess));
	if (wDataSize > sizeof(DBO_MB_WealthTransferSuccess)) return false;

	//��ȡ����
	auto  pWealthTransferSuccess = (DBO_MB_WealthTransferSuccess*)pData;

	//�������
	BYTE cbBuffer[256];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//��ȡ����
	tagForwardHead* pForwardHead = (tagForwardHead*)cbBuffer;

	//����ת��ͷ
	pForwardHead->wTarget = FORWARD_TARGET_USER;
	pForwardHead->dwUserID = pWealthTransferSuccess->dwTargetUserID;

	//ת����Ϣ
	pForwardHead->wItemCount = 1;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;

	//ת������				
	CMD_CS_MailNotify* pMailNotify = (CMD_CS_MailNotify*)(pForwardHead + 1);

	//�����ʼ�
	pMailNotify->dwMailID = pWealthTransferSuccess->dwMailID;
	pMailNotify->dwUserID = pWealthTransferSuccess->dwTargetUserID;	

	//��������
	WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_MailNotify);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_MAIL_NOTIFY, cbBuffer, wPacketSize, 0);

	//�����ɹ�
	SendOperateSuccess(dwContextID, dwTokenID, SUB_MB_WEALTH_TRANSFER, DB_SUCCESS, TEXT(""), NULL);

	return true;
}

//ת�̳齱����
bool CAttemperEngineSink::OnDBMBTurntableLotteryData(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��ȡ����
	auto pTurntableLotteryData = (DBO_MB_TurntableLotteryData*)pData;

	//�������
	CMD_MB_TurntableLotteryData TurntableLotteryData;
	ZeroMemory(&TurntableLotteryData,sizeof(TurntableLotteryData));

	//�齱����
	TurntableLotteryData.wPayLotteryTimes = pTurntableLotteryData->wPayLotteryTimes;
	TurntableLotteryData.wUseLotteryTimes = pTurntableLotteryData->wUseLotteryTimes;
	TurntableLotteryData.wFreeLotteryTimes = pTurntableLotteryData->wFreeLotteryTimes;	
	TurntableLotteryData.wResidueLotteryTimes = pTurntableLotteryData->wResidueLotteryTimes;
	TurntableLotteryData.wDailyMayLotteryTimes = pTurntableLotteryData->wDailyMayLotteryTimes;	
	TurntableLotteryData.wGameTimeLotteryTimes = pTurntableLotteryData->wGameTimeLotteryTimes;
	TurntableLotteryData.wGameCountLotteryTimes = pTurntableLotteryData->wGameCountLotteryTimes;
	TurntableLotteryData.wGameExpendLotteryTimes = pTurntableLotteryData->wGameExpendLotteryTimes;

	//��������
	CopyMemory(TurntableLotteryData.dwGameTimeProgress, pTurntableLotteryData->dwGameTimeProgress,sizeof(TurntableLotteryData.dwGameTimeProgress));
	CopyMemory(TurntableLotteryData.dwGameCountProgress, pTurntableLotteryData->dwGameCountProgress, sizeof(TurntableLotteryData.dwGameCountProgress));
	CopyMemory(TurntableLotteryData.dwGameExpendProgress, pTurntableLotteryData->dwGameExpendProgress, sizeof(TurntableLotteryData.dwGameExpendProgress));

	//ת�̼�¼
	StringCchCopy(TurntableLotteryData.szTurantableRecord,CountArray(TurntableLotteryData.szTurantableRecord), pTurntableLotteryData->szTurntableRecord);

	//�����С
	WORD wRecordSize = CountStringBuffer(TurntableLotteryData.szTurantableRecord);
	WORD wPacketSize = sizeof(TurntableLotteryData) - sizeof(TurntableLotteryData.szTurantableRecord) + wRecordSize;

	//��������
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_TURNTABLE_LOTTERY_DATA, &TurntableLotteryData, wPacketSize, &dwTokenID, 1);

	return true;	
}

//ת�̳齱���
bool CAttemperEngineSink::OnDBMBTurntableLotteryResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��ȡ����
	auto pTurntableLotteryResult = (DBO_MB_TurntableLotteryResult*)pData;

	//�������
	CMD_MB_TurntableLotteryResult TurntableLotteryResult;
	ZeroMemory(&TurntableLotteryResult, sizeof(TurntableLotteryResult));

	//���ñ���
	TurntableLotteryResult.wHitIndex = pTurntableLotteryResult->wHitIndex;
	TurntableLotteryResult.wUseLotteryTimes = pTurntableLotteryResult->wUseLotteryTimes;
	TurntableLotteryResult.wResidueLotteryTimes = pTurntableLotteryResult->wResidueLotteryTimes;

	//��������
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_TURNTABLE_LOTTERY_RESULT, &TurntableLotteryResult, sizeof(TurntableLotteryResult), &dwTokenID, 1);

	return true;
}

//��Ϊ����
bool CAttemperEngineSink::OnDBMBUserActionMask(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��ȡ����
	auto pUserActionMask= (DBO_MB_UserActionMask*)pData;

	//��������
	CMD_MB_UserActionMask UserActionMask;
	ZeroMemory(&UserActionMask, sizeof(UserActionMask));

	//���ñ���
	UserActionMask.dwActionMaskEver = pUserActionMask->dwActionMaskEver;
	UserActionMask.dwActionMaskPerDay = pUserActionMask->dwActionMaskPerDay;
	UserActionMask.dwActionMaskPerWeek = pUserActionMask->dwActionMaskPerWeek;

	//������Ϣ
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_USER_ACTION_MASK, &UserActionMask, sizeof(UserActionMask), &dwTokenID, 1);

	return true;
}

//��λ��Ϣ
bool CAttemperEngineSink::OnDBMBUserRankingInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//����У��
	ASSERT(wDataSize == sizeof(tagUserRankingInfo));
	if (wDataSize != sizeof(tagUserRankingInfo)) return false;

	//��ȡ����
	auto pUserRankingInfo = (tagUserRankingInfo*)pData;
	
	//��������
	CMD_MB_UserRankingInfo RankingResult;
	ZeroMemory(&RankingResult, sizeof(RankingResult));
	CopyMemory(&RankingResult.UserRankingInfo, pUserRankingInfo, sizeof(RankingResult.UserRankingInfo));

	//��������
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_USER_RANKING_INFO, &RankingResult, sizeof(RankingResult), &dwTokenID, 1);

	return true;
}

//��¼�ɹ�
bool CAttemperEngineSink::OnDBMBLogonSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�󶨲���
	auto pBindParameter = GetBindParameter(dwContextID);
	if (pBindParameter == NULL) return NULL;

	//�ж�����
	tagTokenParameter* pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;	

	//��������
	DBO_MB_LogonSuccess* pDBOLogonSuccess = (DBO_MB_LogonSuccess*)pData;

	//��������
	CMD_MB_LogonSuccess* pLogonSuccess = (CMD_MB_LogonSuccess*)m_cbBuffer;
	ZeroMemory(pLogonSuccess, sizeof(CMD_MB_LogonSuccess));

	//��������
	pLogonSuccess->wFaceID = pDBOLogonSuccess->wFaceID;
	pLogonSuccess->cbGender = pDBOLogonSuccess->cbGender;
	pLogonSuccess->dwCustomID = pDBOLogonSuccess->dwCustomID;
	pLogonSuccess->dwGameID = pDBOLogonSuccess->dwGameID;
	pLogonSuccess->dwUserID = pDBOLogonSuccess->dwUserID;
	StringCchCopy(pLogonSuccess->szNickName, CountArray(pLogonSuccess->szNickName), pDBOLogonSuccess->szNickName);
	StringCchCopy(pLogonSuccess->szAccounts, CountArray(pLogonSuccess->szAccounts), pDBOLogonSuccess->szAccounts);
	StringCchCopy(pLogonSuccess->szLogonPass, CountArray(pLogonSuccess->szLogonPass), pDBOLogonSuccess->szLogonPass);
	StringCchCopy(pLogonSuccess->szInsurePass, CountArray(pLogonSuccess->szInsurePass), pDBOLogonSuccess->szInsurePass);

	//����ȼ�
	pLogonSuccess->dwExperience = pDBOLogonSuccess->dwExperience;

	//����ֵ
	pLogonSuccess->dwLoveLiness = pDBOLogonSuccess->dwLoveLiness;

	//�û�Ȩ��
	pLogonSuccess->dwUserRight = pDBOLogonSuccess->dwUserRight;

	//��Ա����
	pLogonSuccess->wMasterOrder = pDBOLogonSuccess->wMasterOrder;
	pLogonSuccess->dwMemberPoint = pDBOLogonSuccess->dwMemberPoint;
	pLogonSuccess->dwMemberOverTime = pDBOLogonSuccess->dwMemberOverTime;

	//�û��Ƹ�
	pLogonSuccess->lUserGold = pDBOLogonSuccess->lUserGold;
	pLogonSuccess->lUserCard = pDBOLogonSuccess->lUserCard;
	pLogonSuccess->lUserInsure = pDBOLogonSuccess->lUserInsure;	
	pLogonSuccess->lCommission = pDBOLogonSuccess->lCommission;

	//��Ϊ����
	pLogonSuccess->dwActionMaskEver = pDBOLogonSuccess->dwActionMaskEver;
	pLogonSuccess->dwActionMaskPerDay = pDBOLogonSuccess->dwActionMaskPerDay;
	pLogonSuccess->dwActionMaskPerWeek = pDBOLogonSuccess->dwActionMaskPerWeek;

	//������Ϣ
	pLogonSuccess->dwParentGameID = pDBOLogonSuccess->dwParentGameID;
	pLogonSuccess->wParentFaceID = pDBOLogonSuccess->wParentFaceID;
	StringCchCopy(pLogonSuccess->szParentNickName, CountArray(pLogonSuccess->szParentNickName), pDBOLogonSuccess->szParentNickName);

	//������Ϣ
	pLogonSuccess->wLockKindID = pDBOLogonSuccess->wLockKindID;
	pLogonSuccess->wLockServerID = pDBOLogonSuccess->wLockServerID;
	pLogonSuccess->wLockServerKind = pDBOLogonSuccess->wLockServerKind;	

	//״̬��Ϣ
	pLogonSuccess->cbShutUpStatus = pDBOLogonSuccess->cbShutUpStatus;
	pLogonSuccess->dwShutUpOverTime = pDBOLogonSuccess->dwShutUpOverTime;

	//����״̬
	if (pLogonSuccess->cbShutUpStatus > 0 && pLogonSuccess->dwShutUpOverTime <= time(NULL))
	{
		pLogonSuccess->cbShutUpStatus = 0;
		pLogonSuccess->dwShutUpOverTime = 0;
	}

	//������Ϣ
	pLogonSuccess->dwServerTime = time(NULL);
	pLogonSuccess->dwLikedNumber = pDBOLogonSuccess->dwLikedNumber;		
	pLogonSuccess->cbRegisterMode = pDBOLogonSuccess->cbRegisterMode;
	pLogonSuccess->dwRegisterTime = pDBOLogonSuccess->dwRegisterTime;
	pLogonSuccess->dwUIShieldMask = pDBOLogonSuccess->dwUIShieldMask;	

	//���Ͷ���
	WORD wHeadSize = sizeof(CMD_MB_LogonSuccess);
	CSendPacketHelper SendPacket(m_cbBuffer + wHeadSize, sizeof(m_cbBuffer) - wHeadSize);

	//ת������
	if (m_TransferParameter.bTransferEnabled)
	{
		//ת������
		DTP_GP_TransferParam TransferParam;
		ZeroMemory(&TransferParam, sizeof(TransferParam));

		//���ñ���
		TransferParam.bTransferEnabled = m_TransferParameter.bTransferEnabled;
		TransferParam.wServiceFeeRate = m_TransferParameter.wServiceFeeRate;
		TransferParam.lReservedAmount = m_TransferParameter.lReservedAmount;
		TransferParam.lMinTransferAmount = m_TransferParameter.lMinTransferAmount;
		TransferParam.lMaxTransferAmount = m_TransferParameter.lMaxTransferAmount;

		//ת������
		SendPacket.AddPacket(&TransferParam, sizeof(TransferParam), DTP_MB_TRANSFER_PARAM);
	}	

	//��������
	if (pDBOLogonSuccess->szEmail[0] != 0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szEmail, CountStringBuffer(pDBOLogonSuccess->szEmail), DTP_MB_USER_EMAIL);
	}

	//�û�UID
	if (pDBOLogonSuccess->szUniqueID[0] != 0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szUniqueID, CountStringBuffer(pDBOLogonSuccess->szUniqueID), DTP_MB_UNIQUEID);
	}

	//����ǩ��
	if (pDBOLogonSuccess->szUnderWrite[0] != 0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szUnderWrite, CountStringBuffer(pDBOLogonSuccess->szUnderWrite), DTP_MB_UNDER_WRITE);
	}	

	//�ֻ�����
	if (pDBOLogonSuccess->szMobilePhone[0] != 0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szMobilePhone, CountStringBuffer(pDBOLogonSuccess->szMobilePhone), DTP_MB_MOBILE_PHONE);
	}

	//��¼����
	if (pDBOLogonSuccess->szLogonToken[0] != 0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szLogonToken, CountStringBuffer(pDBOLogonSuccess->szLogonToken), DTP_MB_LOGON_TOKEN);
	}

	
	//ע������
	if (pDBOLogonSuccess->szPresentGoodsList[0] != 0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szPresentGoodsList, CountStringBuffer(pDBOLogonSuccess->szPresentGoodsList), DTP_MB_REGISTER_PRESENT);
	}

	//ʵ����Ϣ
	if (pDBOLogonSuccess->RealAuth.szCompellation[0] != 0)
	{
		//����ṹ
		DTP_MB_RealAuth RealAuth;
		ZeroMemory(&RealAuth, sizeof(RealAuth));

		//���ñ���
		CopyMemory(RealAuth.szCompellation,pDBOLogonSuccess->RealAuth.szCompellation,sizeof(RealAuth.szCompellation));
		CopyMemory(RealAuth.szPassPortID,pDBOLogonSuccess->RealAuth.szPassPortID,sizeof(RealAuth.szPassPortID));
		//
		SendPacket.AddPacket(&RealAuth, sizeof(RealAuth), DTP_MB_REAL_AUTH);
	}

	//CString s;
	//s.Format(_T("2 szNickName:%s,pDBOLogonSuccess->szUserFaceUrl:%s"), pDBOLogonSuccess->szNickName,pDBOLogonSuccess->szUserFaceUrl);
	//CTraceService::TraceString(s, TraceLevel_Debug); 

	//��¼���� kk jia
	if (pDBOLogonSuccess->szUserFaceUrl[0] != 0)
	{
		SendPacket.AddPacket(pDBOLogonSuccess->szUserFaceUrl, CountStringBuffer(pDBOLogonSuccess->szUserFaceUrl), DTP_MB_USER_INFO);
	}
	

	//����ṹ
	CMD_CS_S_TokenParameter TokenParameter;
	ZeroMemory(&TokenParameter,sizeof(TokenParameter));

	//���Ʋ���
	TokenParameter.dwUserID = pDBOLogonSuccess->dwUserID;
	StringCchCopy(TokenParameter.szLogonToken,CountArray(TokenParameter.szLogonToken), pDBOLogonSuccess->szLogonToken);

	//��������
	WORD wPacketSize = CountStringBuffer(TokenParameter.szLogonToken);
	wPacketSize += sizeof(TokenParameter) - sizeof(TokenParameter.szLogonToken);
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CS_COMMON_SERVICE, SUB_CS_S_TOKEN_PARAMETER, &TokenParameter, wPacketSize, &dwTokenID, 1);

	//��¼�ɹ�
	wPacketSize = SendPacket.GetDataSize() + sizeof(CMD_MB_LogonSuccess);
	SendDataToGate(dwContextID, MDM_MB_LOGON, SUB_MB_LOGON_SUCCESS, pLogonSuccess, wPacketSize, &dwTokenID, 1);

	//��չ����
	auto pExtendParameter = (tagExtendParemter*)pTokenParameter->cbExtendData;

	//���������б� 
	auto JsonValuePtr = CWHJson::getValue(string(CT2CA(pDBOLogonSuccess->szShieldKindList)));
	if (JsonValuePtr->getType() == eJsonTypeArray)
	{
		//ת������
		auto KindObjArrayPtr = CJsonValueArrayPtr::dynamicCast(JsonValuePtr);

		for (auto KindObjPtr : KindObjArrayPtr->value)
		{
			if (KindObjPtr->getType() == eJsonTypeNum)
			{
				auto KindIDPtr = CJsonValueNumPtr::dynamicCast(KindObjPtr);
				if (pExtendParameter->wKindCount < CountArray(pExtendParameter->wShieldKindList))
				{
					pExtendParameter->wShieldKindList[pExtendParameter->wKindCount++] = (WORD)KindIDPtr->value;
				}				
			}
		}
	}

	//��ȡ��ʶ
	WORD wIndex = LOWORD(dwContextID);
	WORD wModuleID = pExtendParameter->wModuleID;

	//������Ϣ
	SendMobileKindInfo(dwContextID, dwTokenID, pExtendParameter);

	//������Ϣ
	SendMobileJackpotInfo(dwContextID, dwTokenID, pExtendParameter);

	//������Ϣ
	SendMobileServerInfo(dwContextID, dwTokenID, wModuleID, pExtendParameter);	

	//������Ʒ
	if (pDBOLogonSuccess->wGoodsCount>0)
	{
		SendUserPackGoods(dwContextID, dwTokenID, pDBOLogonSuccess->GoodsHoldInfo, pDBOLogonSuccess->wGoodsCount);
	}
	//�ǳ�����
	SendNickNameOption(dwContextID, dwTokenID,pDBOLogonSuccess->wNickNameModifyTimes, pDBOLogonSuccess->wNickNameDailyModifyTimes);

	//���֪ͨ
	SendDataToGate(dwContextID, MDM_MB_SERVER_LIST, SUB_MB_LIST_FINISH, &dwTokenID, 1);

	//��������
	CMD_MB_LogonFinish LogonFinish;
	ZeroMemory(&LogonFinish, sizeof(LogonFinish));

	//��¼���
	LogonFinish.dwFuncOption = m_dwMBFuncption;

	//��������
	SendDataToGate(dwContextID, MDM_MB_LOGON, SUB_MB_LOGON_FINISH, &LogonFinish, sizeof(LogonFinish), &dwTokenID, 1);

	//����ṹ
	tagGlobalUserInfo GlobalUserInfo;
	ZeroMemory(&GlobalUserInfo,sizeof(GlobalUserInfo));

	//�û���Ϣ
	GlobalUserInfo.dwUserID = pDBOLogonSuccess->dwUserID;
	GlobalUserInfo.dwGameID = pDBOLogonSuccess->dwGameID;
	GlobalUserInfo.wFaceID= pDBOLogonSuccess->wFaceID;
	GlobalUserInfo.dwCustomID = pDBOLogonSuccess->dwCustomID;
	StringCchCopy(GlobalUserInfo.szNickName, CountArray(GlobalUserInfo.szNickName), pDBOLogonSuccess->szNickName);

	//������Ϣ
	GlobalUserInfo.SocketInfo.wGateID = pBindParameter->wServiceID;
	GlobalUserInfo.SocketInfo.dwTokenID = pTokenParameter->dwTokenID;

	//״̬��Ϣ
	GlobalUserInfo.cbShutUpStatus = pDBOLogonSuccess->cbShutUpStatus;
	GlobalUserInfo.dwShutUpOverTime = pDBOLogonSuccess->dwShutUpOverTime;	

	//�����û�
	auto pGlobalUserItem = m_GlobalItemManager.ActiveUserItem(GlobalUserInfo);
	if (pGlobalUserItem != NULL)
	{
		//�ռ��û�
		CMD_CS_C_UserOnline  UserOnline;
		ZeroMemory(&UserOnline, sizeof(UserOnline));

		//���ñ���
		UserOnline.dwUserID = pDBOLogonSuccess->dwUserID;
		UserOnline.dwGameID = pDBOLogonSuccess->dwGameID;
		StringCchCopy(UserOnline.szNickName, CountArray(UserOnline.szNickName), pDBOLogonSuccess->szNickName);
		StringCchCopy(UserOnline.szUserHeadUrl, CountArray(UserOnline.szUserHeadUrl), pDBOLogonSuccess->szUserFaceUrl);//kk jia
		//�û�����
		UserOnline.wFaceID = pDBOLogonSuccess->wFaceID;
		UserOnline.cbGender = pDBOLogonSuccess->cbGender;
		UserOnline.dwCustomID = pDBOLogonSuccess->dwCustomID;

		//������Ϣ
		UserOnline.wGateID = pBindParameter->wServiceID;
		UserOnline.dwTokenID = pTokenParameter->dwTokenID;

		//״̬��Ϣ
		UserOnline.cbShutUpStatus = pDBOLogonSuccess->cbShutUpStatus;
		UserOnline.dwShutUpOverTime = pDBOLogonSuccess->dwShutUpOverTime;		

		//�û�����
		m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_USER_ONLINE, &UserOnline, sizeof(UserOnline), 0);
	}	

	//�񵥴�����߲���
	auto MarqueeNoticeOptionPtr = g_GlobalOptionManager->GetOptionItem<CMarqueeNoticeOptionPtr>(OPTION_MASK_MARQUEE_NOTICE);
	if (MarqueeNoticeOptionPtr)
	{
		//������
		BYTE cbListType[10] = { 0 };
		auto nTypeCount = MarqueeNoticeOptionPtr->GetListTypeArray(cbListType, CountArray(cbListType));

		for (int i = 0; i < nTypeCount; i++)
		{
			auto pRankingListItem = m_RankingListManager.SearchRankingList(cbListType[i]);
			if (pRankingListItem != NULL)
			{
				auto pListUserItem = pRankingListItem->SearchListUser(pDBOLogonSuccess->dwUserID);
				if (pListUserItem != NULL && pListUserItem->wRankID<=MarqueeNoticeOptionPtr->GetReachRankID())
				{
					//�������
					tagNoticeKind_UserLogon NoticeUserLogon;
					ZeroMemory(&NoticeUserLogon, sizeof(NoticeUserLogon));

					//���ñ���
					NoticeUserLogon.cbListType = cbListType[i];
					NoticeUserLogon.wRankID = pListUserItem->wRankID;
					StringCchCopy(NoticeUserLogon.szNickName, CountArray(NoticeUserLogon.szNickName), pDBOLogonSuccess->szNickName);

					//���͹���
					if (SendMarqeeNotice(NOTICE_KIND_LOGON, &NoticeUserLogon, sizeof(NoticeUserLogon)))
					{
						break;
					}					
				}
			}
		}
	}

	//���û�
	pTokenParameter->pBindUserItem = pGlobalUserItem;
	pTokenParameter->dwUserID = pGlobalUserItem->GetUserID();

	//���봦��
	if (pDBOLogonSuccess->wLockGateID != 0 && pDBOLogonSuccess->wLockLogonID != 0 && pDBOLogonSuccess->dwLockSocketID != 0)
	{
		//��������
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pDBOLogonSuccess->wLockGateID);
		if (pGlobalGateItem != NULL)
		{
			//����ṹ
			CMD_MB_LogoutNotify LogoutNotify;
			ZeroMemory(&LogoutNotify, sizeof(LogoutNotify));

			//���ñ���
			StringCchCopy(LogoutNotify.szDescribeString, CountArray(LogoutNotify.szDescribeString), CWHService::FormatMessage(MIM_LOGON_0007, TEXT("���������˺��ڱ𴦵�¼�����������ߣ�")));

			//��������
			WORD wMsgSize = CountStringBuffer(LogoutNotify.szDescribeString);
			WORD wHeadSize = sizeof(LogoutNotify) - sizeof(LogoutNotify.szDescribeString);			
			SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_MB_LOGON, SUB_MB_LOGOUT_NOTIFY,&LogoutNotify, wHeadSize + wMsgSize,&pDBOLogonSuccess->dwLockSocketID,1);

			//�Ƴ�����
			if (pDBOLogonSuccess->wLockLogonID == m_pServiceOption->wServiceID)
			{				
				pGlobalGateItem->RemoveTokenID(pDBOLogonSuccess->dwLockSocketID);
			}

			//����ṹ
			CMD_CS_C_ShutdownSocket ShutdownSocket = {true};			

			//�ر�����
			SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket),&pDBOLogonSuccess->dwLockSocketID,1);
		}		
	}	

	return true;
}

//��¼ʧ��
bool CAttemperEngineSink::OnDBMBLogonFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��������
	CMD_MB_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure, sizeof(LogonFailure));
	DBO_MB_LogonFailure* pLogonFailure = (DBO_MB_LogonFailure*)pData;

	//��������
	LogonFailure.lResultCode = pLogonFailure->lResultCode;
	StringCchCopy(LogonFailure.szDescribeString, CountArray(LogonFailure.szDescribeString), pLogonFailure->szDescribeString);

	//��������
	WORD wStringSize = CountStringBuffer(LogonFailure.szDescribeString);
	WORD wSendSize = sizeof(LogonFailure) - sizeof(LogonFailure.szDescribeString) + wStringSize;
	SendDataToGate(dwContextID, MDM_MB_LOGON, SUB_MB_LOGON_FAILURE, &LogonFailure, wSendSize,&dwTokenID,1);

	//����ṹ
	CMD_CS_C_ShutdownSocket ShutdownSocket;
	ShutdownSocket.bForceClose = true;

	//�ر�����
	SendDataToGate(dwContextID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket),&dwTokenID,1);

	return true;
}

//�ǳ����
bool CAttemperEngineSink::OnDBMBLogoutResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize == sizeof(DBO_MB_LogoutResult));
	if (wDataSize > sizeof(DBO_MB_LogoutResult)) return false;

	//��ȡ����
	DBO_MB_LogoutResult* pLogoutResult = (DBO_MB_LogoutResult*)pData;

	//�㲥״̬
	if (pLogoutResult->lResultCode == DB_SUCCESS)
	{
		//�ռ��û�
		CMD_CS_C_UserOffline  UserOffline;
		ZeroMemory(&UserOffline, sizeof(UserOffline));

		//���ñ���
		UserOffline.dwUserID = pLogoutResult->dwUserID;
		
		//������Ϣ
		m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_USER_OFFLINE, &UserOffline, sizeof(UserOffline), 0);
	}

	return true;
}

//�󶨳ɹ�
bool CAttemperEngineSink::OnDBMBBindMPSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//Ч�����
	ASSERT(wDataSize <= sizeof(DBO_MB_BindMPSuccess));
	if (wDataSize > sizeof(DBO_MB_BindMPSuccess)) return false;

	//��ȡ����
	DBO_MB_BindMPSuccess* pBindMPSuccess = (DBO_MB_BindMPSuccess*)pData;

	//��������
	CMD_MB_BindMPSuccess BindMPSuccess;
	ZeroMemory(&BindMPSuccess, sizeof(BindMPSuccess));

	//��������
	StringCchCopy(BindMPSuccess.szMobilePhone, CountArray(BindMPSuccess.szMobilePhone), pBindMPSuccess->szMobilePhone);
	StringCchCopy(BindMPSuccess.szNoticeString, CountArray(BindMPSuccess.szNoticeString), pBindMPSuccess->szNoticeString);

	//��������
	WORD wStringSize = CountStringBuffer(BindMPSuccess.szNoticeString);
	WORD wPacketSize = sizeof(BindMPSuccess) - sizeof(BindMPSuccess.szNoticeString) + wStringSize;
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_BINDMP_SUCCESS, &BindMPSuccess, wPacketSize,&dwTokenID,1);

	return true;
}

//���ɹ�
bool CAttemperEngineSink::OnDBMBUnbindMPSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//Ч�����
	ASSERT(wDataSize <= sizeof(DBO_MB_UnbindMPSuccess));
	if (wDataSize > sizeof(DBO_MB_UnbindMPSuccess)) return false;

	//��ȡ����
	DBO_MB_UnbindMPSuccess* pUnbindMPSuccess = (DBO_MB_UnbindMPSuccess*)pData;

	//��������
	CMD_MB_UnbindMPSuccess UnbindMPSuccess;
	ZeroMemory(&UnbindMPSuccess, sizeof(UnbindMPSuccess));

	//��������
	StringCchCopy(UnbindMPSuccess.szNoticeString, CountArray(UnbindMPSuccess.szNoticeString), pUnbindMPSuccess->szNoticeString);

	//��������
	WORD wStringSize = CountStringBuffer(UnbindMPSuccess.szNoticeString);
	WORD wPacketSize = sizeof(UnbindMPSuccess) - sizeof(UnbindMPSuccess.szNoticeString) + wStringSize;
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_UNBINDMP_SUCCESS, &UnbindMPSuccess, wPacketSize,&dwTokenID,1);

	return true;
}

//��ȡ���
bool CAttemperEngineSink::OnDBMBAcquireACResult(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//Ч�����
	ASSERT(wDataSize <= sizeof(DBO_MB_AcquireACResult));
	if (wDataSize > sizeof(DBO_MB_AcquireACResult)) return false;

	//��ȡ����
	DBO_MB_AcquireACResult* pAcquireACResult = (DBO_MB_AcquireACResult*)pData;

	//��ȡ�ɹ�
	if (pAcquireACResult->lResultCode == 0)
	{
		BYTE cbBuffer[1024] = { 0 };
		tagForwardHead* pForwardHead = (tagForwardHead*)cbBuffer;
		ZeroMemory(pForwardHead,sizeof(tagForwardHead));

		//ת��ͷ��
		pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
		pForwardHead->wItemCount = 1;
		pForwardHead->ForwardItem[0].wServiceID = 0;
		pForwardHead->ForwardItem[0].wExcludeID = 0;
		pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;

		//������Ϣ
		CMD_CS_SendAuthCode * pSendAuthCode = (CMD_CS_SendAuthCode *)(pForwardHead + 1);

		//��֤��Ϣ
		pSendAuthCode->cbCodeKind = pAcquireACResult->cbCodeKind;
		pSendAuthCode->cbAuthKind = pAcquireACResult->cbAuthKind;
		StringCchCopy(pSendAuthCode->szAuthCode, CountArray(pSendAuthCode->szAuthCode), pAcquireACResult->szAuthCode);
		StringCchCopy(pSendAuthCode->szMobileEmail, CountArray(pSendAuthCode->szMobileEmail), pAcquireACResult->szMobileEmail);		

		//��������
		WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_SendAuthCode);
		m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_SEND_AUTHCODE, pForwardHead, wPacketSize,0);
	}

	//��������
	CMD_MB_AcquireACResult AcquireACResult;
	ZeroMemory(&AcquireACResult, sizeof(AcquireACResult));

	//��֤��Ϣ
	AcquireACResult.cbAuthKind = pAcquireACResult->cbAuthKind;
	AcquireACResult.dwBindUserID = pAcquireACResult->dwBindUserID;

	//�����Ϣ
	AcquireACResult.lResultCode = pAcquireACResult->lResultCode;
	StringCchCopy(AcquireACResult.szNoticeString, CountArray(AcquireACResult.szNoticeString), pAcquireACResult->szNoticeString);

	//��������
	WORD wStringSize = CountStringBuffer(AcquireACResult.szNoticeString);
	WORD wPacketSize = sizeof(AcquireACResult) - sizeof(AcquireACResult.szNoticeString) + wStringSize;
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_ACQUIREAC_RESULT, &AcquireACResult, wPacketSize,&dwTokenID,1);

	return true;
}

//�󶨳ɹ�
bool CAttemperEngineSink::OnDBMBBindEMailSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//Ч�����
	ASSERT(wDataSize <= sizeof(DBO_MB_BindEMailSuccess));
	if (wDataSize > sizeof(DBO_MB_BindEMailSuccess)) return false;

	//��ȡ����
	DBO_MB_BindEMailSuccess* pBindEMailSuccess = (DBO_MB_BindEMailSuccess*)pData;

	//��������
	CMD_MB_BindEMailSuccess BindEMailSuccess;
	ZeroMemory(&BindEMailSuccess, sizeof(BindEMailSuccess));

	//��������
	StringCchCopy(BindEMailSuccess.szEMail, CountArray(BindEMailSuccess.szEMail), pBindEMailSuccess->szEMail);
	StringCchCopy(BindEMailSuccess.szNoticeString, CountArray(BindEMailSuccess.szNoticeString), pBindEMailSuccess->szNoticeString);

	//��������
	WORD wStringSize = CountStringBuffer(BindEMailSuccess.szNoticeString);
	WORD wPacketSize = sizeof(BindEMailSuccess) - sizeof(BindEMailSuccess.szNoticeString) + wStringSize;
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_BIND_EMAIL_SUCCESS, &BindEMailSuccess, wPacketSize, &dwTokenID, 1);

	return true;
}

//���ɹ�
bool CAttemperEngineSink::OnDBMBUnbindEMailSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//Ч�����
	ASSERT(wDataSize <= sizeof(DBO_MB_UnbindEMailSuccess));
	if (wDataSize > sizeof(DBO_MB_UnbindEMailSuccess)) return false;

	//��ȡ����
	DBO_MB_UnbindEMailSuccess* pUnBindEMailSuccess = (DBO_MB_UnbindEMailSuccess*)pData;

	//��������
	CMD_MB_UnbindEMailSuccess UnBindEMailSuccess;
	ZeroMemory(&UnBindEMailSuccess, sizeof(UnBindEMailSuccess));

	//��������
	StringCchCopy(UnBindEMailSuccess.szNoticeString, CountArray(UnBindEMailSuccess.szNoticeString), pUnBindEMailSuccess->szNoticeString);

	//��������
	WORD wStringSize = CountStringBuffer(UnBindEMailSuccess.szNoticeString);
	WORD wPacketSize = sizeof(UnBindEMailSuccess) - sizeof(UnBindEMailSuccess.szNoticeString) + wStringSize;
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_UNBIND_EMAIL_SUCCESS, &UnBindEMailSuccess, wPacketSize, &dwTokenID, 1);

	return true;
}

//������Ʒ
bool CAttemperEngineSink::OnDBMBUserPackGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pUserPackGoods = (DBO_MB_PackGoods *)pData;

	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//�������
	auto pPackGoods= (CMD_CM_PackGoods*)m_cbBuffer;
	ZeroMemory(pPackGoods, sizeof(CMD_CM_PackGoods));

	//���ñ���
	pPackGoods->wGoodsCount = pUserPackGoods->wGoodsCount;
	CopyMemory(pPackGoods->GoodsHoldInfo, pUserPackGoods->GoodsHoldInfo, pPackGoods->wGoodsCount * sizeof(pPackGoods->GoodsHoldInfo[0]));

	//��������
	WORD wPacketSize = sizeof(CMD_CM_GoodsUpdate) + pPackGoods->wGoodsCount * sizeof(pPackGoods->GoodsHoldInfo[0]);
	SendDataToGate(dwContextID, MDM_CM_SYSTEM, SUB_CM_PACK_GOODS, pPackGoods, wPacketSize,&dwTokenID,1);

	return true;
}

//��Ʒ����
bool CAttemperEngineSink::OnDBMBUserGoodsUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pUserGoodsUpdate = (DBO_MB_GoodsUpdate*)pData;

	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//�������
	auto pGoodsUpdate = (CMD_CM_GoodsUpdate*)m_cbBuffer;
	ZeroMemory(pGoodsUpdate, sizeof(CMD_CM_GoodsUpdate));

	//���ñ���
	pGoodsUpdate->wGoodsCount = pUserGoodsUpdate->wGoodsCount;
	CopyMemory(pGoodsUpdate->GoodsHoldInfo, pUserGoodsUpdate->GoodsHoldInfo, pGoodsUpdate->wGoodsCount * sizeof(pGoodsUpdate->GoodsHoldInfo[0]));

	//��������
	WORD wPacketSize = sizeof(CMD_CM_GoodsUpdate) + pGoodsUpdate->wGoodsCount * sizeof(pGoodsUpdate->GoodsHoldInfo[0]);
	SendDataToGate(dwContextID, MDM_CM_SYSTEM, SUB_CM_GOODS_UPDATE, pGoodsUpdate, wPacketSize,&dwTokenID,1);

	return true;
}

//�Ƹ�����
bool CAttemperEngineSink::OnDBMBUserWealthUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pUserWealthUpdate = (DBO_MB_WealthUpdate*)pData;

	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//�������
	auto pWealthUpdate = (CMD_CM_WealthUpdate*)m_cbBuffer;
	ZeroMemory(pWealthUpdate, sizeof(CMD_CM_WealthUpdate));

	//���ñ���
	pWealthUpdate->wItemCount = pUserWealthUpdate->wItemCount;
	CopyMemory(pWealthUpdate->WealthItem, pUserWealthUpdate->WealthItem, pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]));

	//��������
	WORD wPacketSize = sizeof(CMD_CM_WealthUpdate) + pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]);
	SendDataToGate(dwContextID, MDM_CM_SYSTEM, SUB_CM_WEALTH_UPDATE, pWealthUpdate, wPacketSize,&dwTokenID,1);

	return true;
}

//��������
bool CAttemperEngineSink::OnDBMBUserLoveLinessUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
    //Ч�����
    ASSERT(wDataSize <= sizeof(DBO_MB_LoveLinessUpdate));
    if (wDataSize > sizeof(DBO_MB_LoveLinessUpdate)) return false;

    //��ȡ����
    auto pDBOLoveLinessUpdate = (DBO_MB_LoveLinessUpdate*)pData;

    //�ж�����
    auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
    if (pTokenParameter == NULL) return true;

    //�������
	CMD_CM_LoveLinessUpdate LoveLinessUpdate;
    ZeroMemory(&LoveLinessUpdate, sizeof(LoveLinessUpdate));

    //���ñ���
	LoveLinessUpdate.dwLoveLiness = pDBOLoveLinessUpdate->dwLoveLiness;

    //��������
    SendDataToGate(dwContextID, MDM_CM_SYSTEM, SUB_CM_LOVELINESS_UPDATE, &LoveLinessUpdate, sizeof(LoveLinessUpdate),&dwTokenID,1);

    return true;
}

//Ͷ����Ϣ
bool CAttemperEngineSink::OnDBMBPostMessage(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��ȡ����
	DBO_MB_PostMessage* pDBPostMessage = (DBO_MB_PostMessage*)pData;

	//�㲥״̬
	auto pForwardHead = (tagForwardHead*)m_cbBuffer;
	ZeroMemory(pForwardHead, sizeof(tagForwardHead));

	//ת������
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;

	//ת������
	pForwardHead->wItemCount = 1;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CHAT;
	pForwardHead->ForwardItem[0].dwHashKeyID = pDBPostMessage->dwHashKeyID;
	pForwardHead->ForwardItem[0].wServiceKind = pDBPostMessage->wServiceKind;	

	//Ͷ����Ϣ
	auto pPostMessage = (CMD_CS_PostMessage*)(pForwardHead + 1);
	ZeroMemory(pPostMessage, sizeof(CMD_CS_PostMessage));

	//ɢ����Ϣ
	pPostMessage->dwHashKeyID = pDBPostMessage->dwHashKeyID;

	//��������
	CopyMemory(pPostMessage+1, pDBPostMessage->cbMessageData, pDBPostMessage->wMessageSize);

	//��������
	WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_PostMessage) + pDBPostMessage->wMessageSize;
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_POST_MESSAGE, pForwardHead, wPacketSize, 0);

	return true;
}
//���в�ѯ���
bool CAttemperEngineSink::OnDBMBInsureQueryResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//
	ASSERT(wDataSize == sizeof(DBR_MB_InsureQueryResult));
	DBR_MB_InsureQueryResult* pInsureResult = (DBR_MB_InsureQueryResult*)pData;

	//
	CMD_MB_InsureQueryResult InsureResult;
	ZeroMemory(&InsureResult, sizeof(CMD_MB_InsureQueryResult));
	InsureResult.cbEnable = pInsureResult->cbEnable;
	CopyMemory(&InsureResult.SaveAndTake, &pInsureResult->SaveAndTake, sizeof(InsureResult.SaveAndTake));
	CopyMemory(&InsureResult.Transfer, &pInsureResult->Transfer, sizeof(InsureResult.Transfer));
	InsureResult.lScore = pInsureResult->lScore;
	InsureResult.lInsureScore = pInsureResult->lInsureScore;

	//��������
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_INSURE_QUERY_RESULT, &InsureResult, sizeof(InsureResult), &dwTokenID, 1);
	return true;
};
//���гɹ�
bool CAttemperEngineSink::OnDBMBInsureSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//
	ASSERT(wDataSize == sizeof(DBR_MB_InsureSuccess));
	DBR_MB_InsureSuccess* pInsure = (DBR_MB_InsureSuccess*)pData;

	//
	CMD_MB_InsureSuccess Insure;
	ZeroMemory(&Insure, sizeof(CMD_MB_InsureSuccess));
	Insure.lScore = pInsure->lScore;
	Insure.lInsureScore = pInsure->lInsureScore;

	//��������
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_INSURE_SUCCESS, &Insure, sizeof(Insure), &dwTokenID, 1);
	return true;
}
//����ʧ��
bool CAttemperEngineSink::OnDBMBInsureFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//
	ASSERT(wDataSize == sizeof(DBR_MB_InsureFailure));
	DBR_MB_InsureFailure* pInsure = (DBR_MB_InsureFailure*)pData;

	//
	CMD_MB_InsureFailure Insure;
	ZeroMemory(&Insure, sizeof(CMD_MB_InsureFailure));

	Insure.dwResultCode = pInsure->dwResultCode;
	CopyMemory(Insure.szDescribeString, pInsure->szDescribeString, sizeof(Insure.szDescribeString));

	//��������
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_INSURE_FAILURE, &Insure, sizeof(Insure), &dwTokenID, 1);

	return true;
}
//�����ʼ�֪ͨ
bool CAttemperEngineSink::OnDBMBInsureMailNotify(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//
	ASSERT(wDataSize == sizeof(DBR_MB_InsureMailNotify));
	DBR_MB_InsureMailNotify* pInsureMail = (DBR_MB_InsureMailNotify*)pData;

	//�������
	BYTE cbBuffer[256];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//��ȡ����
	tagForwardHead* pForwardHead = (tagForwardHead*)cbBuffer;

	//����ת��ͷ
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwardHead->dwUserID = pInsureMail->dwUserID;

	//ת����Ϣ
	pForwardHead->wItemCount = 1;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;

	//ת������				
	CMD_CS_MailNotify* pMailNotify = (CMD_CS_MailNotify*)(pForwardHead + 1);

	//�����ʼ�
	pMailNotify->dwUserID = pInsureMail->dwUserID;
	pMailNotify->dwMailID = pInsureMail->dwMailID;

	//��������
	WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_MailNotify);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_MAIL_NOTIFY, cbBuffer, wPacketSize, 0);
	
	return true;
}
//�ͱ���ѯ
bool CAttemperEngineSink::OnDBMBSubsidyQuery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//
	ASSERT(wDataSize == sizeof(DBR_MB_Subsidy));
	DBR_MB_Subsidy* pSubsidy = (DBR_MB_Subsidy*)pData;

	//
	CMD_MB_Subsidy Subsidy;
	ZeroMemory(&Subsidy, sizeof(CMD_MB_Subsidy));

	Subsidy.cbDailyTimes = pSubsidy->cbDailyTimes;
	Subsidy.cbResidualTimes = pSubsidy->cbResidualTimes;
	Subsidy.lConsumeScore = pSubsidy->lConsumeScore;

	//��������
	SendDataToGate(dwContextID, MDM_MB_USER_SERVICE, SUB_MB_USER_SUBSIDY, &Subsidy, sizeof(Subsidy), &dwTokenID, 1);

	return true;
};
//�����ɹ�
bool CAttemperEngineSink::OnDBMBOperateSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��ȡ����
	DBO_MB_OperateSuccess* pOperateSuccess = (DBO_MB_OperateSuccess*)pData;

	//�����ɹ�
	SendOperateSuccess(dwContextID, dwTokenID, pOperateSuccess->wRequestCmdID, pOperateSuccess->lResultCode, pOperateSuccess->szDescribeString);

	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBMBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�ж�����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL) return true;

	//��ȡ����
	DBO_MB_OperateFailure* pOperateFailure = (DBO_MB_OperateFailure*)pData;

	//����ʧ��
	SendOperateFailure(dwContextID,dwTokenID, pOperateFailure->wRequestCmdID, pOperateFailure->lErrorCode, pOperateFailure->szDescribeString);

	return true;	
}
//�ǳ�����
bool CAttemperEngineSink::OnDBMBNickNameOption(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	CopyMemory(&m_NickNameOption, pData, sizeof(tagNickNameOption));
	return true;
}

//������֤��
VOID CAttemperEngineSink::MakeAuthCode(WCHAR szAutoCode[], BYTE cbBufferSize)
{
	//У�����
	ASSERT(cbBufferSize > 0);
	if (cbBufferSize <= 0) return;

	//�ַ�������
	LPCTSTR szCharacterSet = L"0123456789";

	//��������
	LPCTSTR pszCharacter = NULL;
	WORD wStringLen = lstrlen(szCharacterSet);

	//������֤��
	for (int i = 0; i < cbBufferSize - 1; i++)
	{
		szAutoCode[i] = szCharacterSet[rand() % wStringLen];
	}

	//StringCchCopy(szAutoCode, cbBufferSize, TEXT("123456"));

	//��ӽ�����
	szAutoCode[cbBufferSize - 1] = 0;
}

//�󶨲���
tagBindParameter* CAttemperEngineSink::GetBindParameter(DWORD dwSocketID)
{
	//��Ч����
	WORD wBindIndex = LOWORD(dwSocketID);	
	if (wBindIndex == INVALID_WORD_INDEX) return NULL;

	//��������
	if (wBindIndex < m_pServiceOption->wMaxConnect)
	{
		auto pBindParameter = m_pBindParameter + wBindIndex;

		//У������
		if (pBindParameter->dwSocketID != dwSocketID)
		{
			return NULL;
		}

		return pBindParameter;
	}	

	//�������
	ASSERT(FALSE);

	return NULL;
}

//���Ʋ���
tagTokenParameter* CAttemperEngineSink::GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID)
{
	//�󶨲���
	auto pBindParameter = GetBindParameter(dwSocketID);
	if (pBindParameter == NULL) return NULL;

	//ת��ģʽ
	if (pBindParameter->wServiceModule == SERVICE_MODULE_GATE)
	{
		//��ѯ����
		CGlobalGateItem* pGlobalGateItem = pBindParameter->pGlobalGateItem;
		if (pGlobalGateItem == NULL) return NULL;

		//��ѯ�û�
		tagTokenParameter* pTokenParameter = pGlobalGateItem->SearchTokenID(dwTokenID);
		if (pTokenParameter == NULL || pTokenParameter->dwTokenID != dwTokenID) return NULL;

		return pTokenParameter;
	}

	return NULL;
}

//��������
bool CAttemperEngineSink::SendDataToGate(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, DWORD* pdwTokenID, WORD wTokenCount)
{
	//��������
	return m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(wMainCmdID, SERVICE_MODULE_LOGON), wSubCmdID, pdwTokenID, wTokenCount);
}

//��������
bool CAttemperEngineSink::SendDataToGate(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD* pdwTokenID, WORD wTokenCount)
{
	//��������
	return m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(wMainCmdID, SERVICE_MODULE_LOGON), wSubCmdID, pData, wDataSize, pdwTokenID, wTokenCount);
}

//Ⱥ������
bool CAttemperEngineSink::SendBatchData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	//��������
	CGlobalGateItem* pGlobalGateItem = NULL;

	//��������
	DWORD* pdwTokenIDArray = NULL;
	WORD wCellCount = 0, wSendCount = 0, wTotalCount = 0;

	//���ҷ���
	do
	{
		//ö������ 
		pGlobalGateItem = m_GlobalItemManager.EnumGateItem(pGlobalGateItem);
		if (pGlobalGateItem == NULL) break;

		//��������
		if (pGlobalGateItem->GetTokenCount() == 0) continue;

		//���ñ���		
		wSendCount = 0;
		wCellCount = pGlobalGateItem->GetTokenCount();
		wTotalCount = pGlobalGateItem->GetTokenCount();
		pdwTokenIDArray = pGlobalGateItem->m_TokenIDArray.GetData();

		//�з�����
		while ((sizeof(WORD) + wCellCount * sizeof(DWORD) + wDataSize) > SOCKET_PACKET)
		{
			wCellCount = wCellCount / 2;
		}

		//��������
		if (wCellCount > TOKEN_COUNT) wCellCount = TOKEN_COUNT;

		//ѭ������
		while (wSendCount < wTotalCount)
		{
			//��������
			if (wTotalCount - wSendCount < wCellCount)
			{
				wCellCount = wTotalCount - wSendCount;
			}

			//��������
			SendDataToGate(pGlobalGateItem->GetSocketID(), wMainCmdID, wSubCmdID, pData, wDataSize, pdwTokenIDArray + wSendCount, wCellCount);

			//��������			
			wSendCount += wCellCount;
		}

	} while (true);

    return true;
}

//��ǰ�汾
tagModuleVersion* CAttemperEngineSink::GetCurrentVersion(WORD wModuleID, DWORD dwDeviceKind)
{
	//����汾	
	for (INT_PTR i = 0; i < m_ModuleVersionActive.GetCount(); i++)
	{
		//�ж�����
		if (m_ModuleVersionActive[i].IsCurrent == false) continue;
		if (m_ModuleVersionActive[i].wModuleID != wModuleID) continue;
		if (m_ModuleVersionActive[i].dwDeviceKind != dwDeviceKind) continue;

		return &m_ModuleVersionActive[i];
	}

	return NULL;
}

//����汾
tagModuleVersion* CAttemperEngineSink::GetSpecialVersion(WORD wModuleID, DWORD dwDeviceKind, DWORD dwVersion)
{
	//����汾	
	for (INT_PTR i = 0; i < m_ModuleVersionActive.GetCount(); i++)
	{
		//�ж�����
		if (m_ModuleVersionActive[i].IsCurrent == true) continue;
		if (m_ModuleVersionActive[i].wModuleID != wModuleID) continue;
		if (m_ModuleVersionActive[i].dwVersion != dwVersion) continue;
		if (m_ModuleVersionActive[i].dwDeviceKind != dwDeviceKind) continue;

		return &m_ModuleVersionActive[i];
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////


