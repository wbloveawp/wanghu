#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "MonitorCenter.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_CONNECT_CENTER			1									//������ʶ
#define IDI_CONNECT_VALIDATE		2									//������ʶ
#define IDI_MONITOR_SERVICE_PULSE	4									//��������
#define IDI_REPORT_JACKPOT_SCORE	5									//�ϱ��ʽ�

//ʱ�䶨��
#define TIME_MONITOR_SERVICE_PULSE	1L									//��������
#define TIME_REPORT_JACKPOT_SCORE	15L									//�ϱ��ʽ�

//////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����		
	m_StockUpdateID = 0;
	m_bMonitorService=false;	
	m_bWillConcludeService=false;

	//���ñ���
	m_pServiceOption=NULL;
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;	
	ZeroMemory(&m_MonitorItem,sizeof(m_MonitorItem));

	//�������
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;

	//��ʼ��
	m_StockGameKind.InitHashTable(31);

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

	//���ñ�ʶ
	m_StockUpdateID = 0;
	m_bMonitorService=true;
	m_bWillConcludeService=false;

	//��ʼ��
	m_ShareStockManager.StartStockManager(this);

	//����ʱ��
	ASSERT(m_pITimerEngine != NULL);
	m_pITimerEngine->SetTimer(IDI_MONITOR_SERVICE_PULSE, TIME_MONITOR_SERVICE_PULSE*1000L, TIMES_INFINITY, 0);

	//����ʱ��
	ASSERT(m_pITimerEngine != NULL);
	m_pITimerEngine->SetTimer(IDI_REPORT_JACKPOT_SCORE, TIME_REPORT_JACKPOT_SCORE * 1000L, TIMES_INFINITY, 0);	

	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//���ö���
	if (!m_ShareStockManager.CloseStockManager())
	{
		return false;
	}

	//�Ƴ�����
	m_StockGameKind.RemoveAllItem();

	//״̬����		
	m_StockUpdateID = 0;
	m_bMonitorService=false;
	m_bWillConcludeService=false;
	m_StockUpdateArray.RemoveAll();

	//���ñ���
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;

	//ɾ������
	SafeDeleteArray(m_pBindParameter);

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	switch (dwIdentifier)
	{
	case CT_LOAD_GLOBAL_CONFIG:			//��������
	{
		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڼ���ϵͳ����..."),TraceLevel_Normal);

		//����ṹ
		DBR_LoadGlobalConfig LoadGlobalConfig;
		ZeroMemory(&LoadGlobalConfig,sizeof(LoadGlobalConfig));

		//��������
		LoadGlobalConfig.dwConfigMask = 0;

		//��������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_LOAD_GLOBAL_CONFIG, 0, 0, 0, &LoadGlobalConfig, sizeof(LoadGlobalConfig));

		return true;
	}
	case CT_CONNECT_CENTER:				//��������
	{
		//���õ�ַ
		DWORD dwCenterServer=m_pInitParameter->m_CenterAddress.dwServiceAddr;

		//��������
		m_pITCPSocketCenter->Connect(htonl(dwCenterServer), m_pInitParameter->m_wCenterPort);

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڳ����������ķ�����..."),TraceLevel_Normal);

		return true;
	}
	case CT_SERVICE_WILLSTOP:			//����ֹͣ
	{
		//״̬�ж�
		if (m_bWillConcludeService) return true;

		//�ر��ж�
		if (!m_ShareStockManager.CloseStockManager())
		{
			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ȹرձ���������Ϸ��������ֹͣ����"), TraceLevel_Warning);

			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess = ER_FAILURE;

			//��������
			CServiceUnits* pServiceUnits = CServiceUnits::g_pServiceUnits;
			pServiceUnits->SendControlPacket(UI_SERVICE_WILLSTOP_RESULT, &ControlResult, sizeof(ControlResult));

			return true;
		}

		//����״̬
		m_bWillConcludeService = true;

		//�ж�״̬
		if (m_StockUpdateArray.GetCount() == 0)
		{
			//�¼�֪ͨ
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess = ER_SUCCESS;
			SendUIControlPacket(UI_SERVICE_WILLSTOP_RESULT, &ControlResult, sizeof(ControlResult));
		}

		return true;
	}
	case FT_FORWARD_TO_CENTER:			//ת������
	{
		//��ȡ����
		auto pForwardToCenter = (FP_ForwardToCenter*)pData;

		//��������
		m_pITCPSocketCenter->SendData(pForwardToCenter->wMainCmdID, pForwardToCenter->wSubCmdID, pForwardToCenter->cbDataBuffer, pForwardToCenter->wDataSize,0);

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
		//���õ�ַ
		DWORD dwCenterServer = m_pInitParameter->m_CenterAddress.dwServiceAddr;

		//��������
		m_pITCPSocketCenter->Connect(htonl(dwCenterServer), m_pInitParameter->m_wCenterPort);

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڳ����������ķ�����..."),TraceLevel_Normal);

		return true;
	}
	case IDI_MONITOR_SERVICE_PULSE:	 //��������
	{
		//ʱ�ӵ���
		m_ShareStockManager.OnEventTimerPulse();			

		return true;
	}
	case IDI_REPORT_JACKPOT_SCORE:	//�ϱ��ʽ�
	{
		//�ϱ��ʽ�
		//ReportGameJackpot();		

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

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//�����Ϣ
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//���Ԥ��
VOID CAttemperEngineSink::OnEventStockScoreAlarm(tagStockAlarmItem StockAlarmList[], WORD wItemCount)
{
	//���涨��
	BYTE cbBuffer[2048] = {0};

	//��ȡ����
	auto pForwardHead = (tagForwardHead*)cbBuffer;
	ZeroMemory(pForwardHead, sizeof(tagForwardHead));

	//ת������
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;

	//ת������
	pForwardHead->wItemCount = 1;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;

	//�������
	auto pSendExceptionAlarm = (CMD_CS_SendExceptionAlarm*)(pForwardHead + 1);

	//Ԥ������
	StringCchCopyA(pSendExceptionAlarm->szAlarmTitle, CountArray(pSendExceptionAlarm->szAlarmTitle), "Game Stock Alarm");

	for (int i = 0; i < wItemCount; i++)
	{
		//��������
		auto pGameKind = m_StockGameKind.SearchStockItem(StockAlarmList[i].wKindID);
		if (pGameKind != NULL)
		{
			StringCchCopy(pSendExceptionAlarm->szServiceName,CountArray(pSendExceptionAlarm->szServiceName), pGameKind->szKindName);
		}

		//����Ԥ����Ϣ		
		_sntprintf_s(pSendExceptionAlarm->szExceptionInfo, CountArray(pSendExceptionAlarm->szExceptionInfo), CountArray(pSendExceptionAlarm->szExceptionInfo), TEXT("��Ϸ�����ָ��֣�StockID=%d,BetAmount=%0.2f,StockScore=%0.2f ������IP: %s, ƽ̨����: %s"),
			StockAlarmList[i].wStockID, StockAlarmList[i].lBetAmount / (RATIO_VALUE * 1.f), StockAlarmList[i].lStockScore / (RATIO_VALUE * 1.f), m_pServiceOption->szServiceDomain, m_pInitParameter->m_szServerName);

		//��������				
		WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_SendExceptionAlarm);
		m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_SEND_EXCEPTION_ALARM, cbBuffer, wPacketSize, 0);		
	}
}

//�ʳ�Ԥ��
VOID CAttemperEngineSink::OnEventJackpotScoreAlarm(tagJackpotAlarmItem JackpotAlarmList[], WORD wItemCount)
{
	//���涨��
	BYTE cbBuffer[2048] = { 0 };

	//��ȡ����
	auto pForwardHead = (tagForwardHead*)cbBuffer;
	ZeroMemory(pForwardHead, sizeof(tagForwardHead));

	//ת������
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;

	//ת������
	pForwardHead->wItemCount = 1;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;

	//�������
	auto pSendExceptionAlarm = (CMD_CS_SendExceptionAlarm*)(pForwardHead + 1);

	//Ԥ������
	StringCchCopyA(pSendExceptionAlarm->szAlarmTitle, CountArray(pSendExceptionAlarm->szAlarmTitle), "Game Jackpot Alarm");

	for (int i = 0; i < wItemCount; i++)
	{	
		//��������
		auto pGameKind = m_StockGameKind.SearchStockItem(JackpotAlarmList[i].wKindID);
		if (pGameKind != NULL)
		{
			StringCchCopy(pSendExceptionAlarm->szServiceName, CountArray(pSendExceptionAlarm->szServiceName), pGameKind->szKindName);
		}

		//����Ԥ����Ϣ
		_sntprintf_s(pSendExceptionAlarm->szExceptionInfo, CountArray(pSendExceptionAlarm->szExceptionInfo), CountArray(pSendExceptionAlarm->szExceptionInfo), TEXT("��Ϸ�ʳس��ָ��֣�StockID=%d,BetAmount=%0.2f,LevelID=%d,StockScore=%0.3f ������IP: %s, ƽ̨����: %s"),
			JackpotAlarmList[i].wStockID, JackpotAlarmList[i].lBetAmount/ (RATIO_VALUE * 1.f), JackpotAlarmList[i].wLevelID, JackpotAlarmList[i].lStockScore / (RATIO_VALUE * 1.f), m_pServiceOption->szServiceDomain,m_pInitParameter->m_szServerName);

		//��������				
		WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_SendExceptionAlarm);
		m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_SEND_EXCEPTION_ALARM, cbBuffer, wPacketSize, 0);
	}
}

//������
VOID CAttemperEngineSink::OnEventStockScoreUpdate(tagStockVariationItem StockVariationList[], WORD wItemCount)
{
	//��������
	int nStepSize = 60;

	//�ְ�����
	for (int i = 0; i < wItemCount; i += nStepSize)
	{
		//��¼��ʶ
		if (m_bWillConcludeService) m_StockUpdateArray.Add(m_StockUpdateID);

		//��������
		auto nItemCount = (i + nStepSize) <= wItemCount ? nStepSize : wItemCount - i;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_UPDATE_STOCK_SCORE, 0L, 0L, m_StockUpdateID++, &StockVariationList[i], nItemCount * sizeof(StockVariationList[0]));
	}	
}

//״̬����
VOID CAttemperEngineSink::OnEventStockStateUpdate(tagStateVariationItem StateVariationList[], WORD wItemCount)
{
	//��������
	int nStepSize = 70;

	//�ְ�����
	for (int i = 0; i < wItemCount; i += nStepSize)
	{
		//��¼��ʶ
		if (m_bWillConcludeService) m_StockUpdateArray.Add(m_StockUpdateID);

		//��������
		auto nItemCount = (i + nStepSize) <= wItemCount ? nStepSize : wItemCount - i;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_UPDATE_STOCK_STATE, 0L, 0L, m_StockUpdateID++, &StateVariationList[i], nItemCount * sizeof(StateVariationList[0]));
	}
}

//�ʳظ���
VOID CAttemperEngineSink::OnEventJackpotScoreUpdate(tagJackpotVariationItem JackpotVariationList[], WORD wItemCount)
{
	//��������
	int nStepSize = 60;

	//�ְ�����
	for (int i = 0; i < wItemCount; i += nStepSize)
	{
		//��¼��ʶ
		if (m_bWillConcludeService) m_StockUpdateArray.Add(m_StockUpdateID);

		//��������
		auto nItemCount = (i + nStepSize) <= wItemCount ? nStepSize : wItemCount - i;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_UPDATE_JACKPOT_SCORE, 0L, 0L, m_StockUpdateID++, &JackpotVariationList[i], nItemCount * sizeof(JackpotVariationList[0]));
	}
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GAME_KIND_ITEM:		//��Ϸ����
	{
		return OnDBResultGameKindItem(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_STOCK_LIST_ITEM:		//�������
	{
		return OnDBResultStockListItem(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_SUPER_PLAYER_CONTROL:	//��R����
	{
		return OnDBResultSuperPlayerControl(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_LOAD_CONFIG_FINISH:	//�������
	{
		return OnDBResultLoadConfigFinish(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_QUERY_STOCK_ITEM:		//��ѯ���
	{
		return OnDBResultQueryStockItem(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_UPDATE_STOCK_SCORE:	//���¿��
	{
		return OnDBResultUpdateStockScore(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_UPDATE_STOCK_STATE:	//����״̬
	{
		return OnDBResultUpdateStockState(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_UPDATE_JACKPOT_SCORE:	//���²ʽ�
	{
		return OnDBResultUpdateJackpotScore(dwContextID, dwTokenID, pData, wDataSize);
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
	if (dwServiceID==NETWORK_CENTER)
	{
		//�����ж�
		if (nErrorCode!=0)
		{
			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ķ���������ʧ�� [ %ld ]��%ld �����������"),TraceLevel_Warning, nErrorCode, m_pInitParameter->m_wConnectTime);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L, TIMES_INFINITY, 0);

			return false;
		}

		//��������
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService,sizeof(RegisterService));

		//����ṹ
		RegisterService.wServiceMask = SERVICE_MASK_GAME;
		RegisterService.wServiceID= m_pServiceOption->wServiceID;
		RegisterService.wServiceModule = SERVICE_MODULE_MONITOR;		
		RegisterService.wServicePort= m_pServiceOption->wServicePort;	
		RegisterService.wServiceKindID = m_pServiceOption->wServiceKindID;
		StringCchCopy(RegisterService.szServiceName, CountArray(RegisterService.szServiceName), TEXT("��ط�����"));
		StringCchCopy(RegisterService.szServiceDomain, CountArray(RegisterService.szServiceDomain), m_pServiceOption->szServiceDomain);

		//��������
		m_pITCPSocketCenter->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_SERVICE,&RegisterService,sizeof(RegisterService),0);

		return true;
	}	

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	//��������
	if (dwServiceID ==NETWORK_CENTER)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:			//ע�����
			{
				return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_COMMON_SERVICE:		//������Ϣ
			{
				return OnTCPSocketMainCommonService(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_FORWARD_SERVICE:	//ת������
			{
				return OnTCPSocketMainForwardService(Command.wSubCmdID, pData, wDataSize);
			}
		case MDM_CS_MANAGER_SERVICE:	//�������
			{
				return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
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

			//ʱ�ӵ���
			m_ShareStockManager.OnEventTimerPulse();

			//�ϱ��ʽ�
			//ReportGameJackpot();

			return true;
		}
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
		{
			//Ч�����
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;
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


//�б��¼�
bool CAttemperEngineSink::OnTCPSocketMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{	
	case SUB_CS_S_SERVER_INSERT:	//�������
	{
		//Ч�����
		ASSERT(wDataSize % sizeof(tagGameServer) == 0);
		if (wDataSize % sizeof(tagGameServer) != 0) return false;

		//��������
		WORD wItemCount = wDataSize / sizeof(tagGameServer);
		tagGameServer* pGameServer = (tagGameServer*)pData;

		return true;
	}
	case SUB_CS_S_SERVER_REMOVE:	//�����Ƴ�
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_CS_S_ServiceRemove));
		if (wDataSize != sizeof(CMD_CS_S_ServiceRemove)) return false;

		//��������
		CMD_CS_S_ServiceRemove* pServiceRemove = (CMD_CS_S_ServiceRemove*)pData;
		
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
	case SUB_CS_APPEND_STOCK_ITEM:	  //��ӿ��
	{
		auto pAppendStockItem = (CMD_CS_AppendStockItem*)pData;

		//�������
		DBR_QueryStockItem QueryStockItem = {};
		QueryStockItem.wStockID = pAppendStockItem->wStockID;
		QueryStockItem.wCommandID = SUB_CS_APPEND_STOCK_ITEM;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_QUERY_STOCK_ITEM, 0, 0, 0, &QueryStockItem, sizeof(QueryStockItem));

		//�����־
		CTraceService::TraceString(TEXT("��ӿ�棺StockID=%d"), TraceLevel_Exception ,pAppendStockItem->wStockID);

		return true;
	}
	case SUB_CS_DELETE_STOCK_ITEM:	  //ɾ�����
	{
		auto pDeleteStockItem = (CMD_CS_DeleteStockItem*)pData;

		//ɾ�����
		m_ShareStockManager.DeleteStockItem(pDeleteStockItem->wStockID);

		//�����־
		CTraceService::TraceString(TEXT("ɾ����棺StockID=%d"), TraceLevel_Exception, pDeleteStockItem->wStockID);

		return true;
	}
	case SUB_CS_MODIFY_STOCK_ITEM:	  //�޸Ŀ��
	{
		auto pModifyStockItem = (CMD_CS_ModifyStockItem*)pData;

		//�������
		DBR_QueryStockItem QueryStockItem = {};
		QueryStockItem.wStockID = pModifyStockItem->wStockID;
		QueryStockItem.wCommandID = SUB_CS_MODIFY_STOCK_ITEM;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_QUERY_STOCK_ITEM, 0, 0, 0, &QueryStockItem, sizeof(QueryStockItem));

		//�����־
		CTraceService::TraceString(TEXT("�޸Ŀ�棺StockID=%d"), TraceLevel_Exception, pModifyStockItem->wStockID);

		return true;
	}
	case SUB_CS_ADJUST_STOCK_SCORE:	  //�������ֵ
	{
		auto pModifyStockScore= (CMD_CS_AdjustStockScore*)pData;

		//�������ֵ
		if (m_ShareStockManager.AdjustStockScore(pModifyStockScore->wStockID, pModifyStockScore->lChangeScore))
		{
			//�������
			DBR_AdjustStockScore AdjustStockScore = {};
			AdjustStockScore.wStockID = pModifyStockScore->wStockID;
			AdjustStockScore.lChangeScore = pModifyStockScore->lChangeScore;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_ADJUST_STOCK_SCORE, 0, 0, 0, &AdjustStockScore, sizeof(AdjustStockScore));

			//�����־
			CTraceService::TraceString(TEXT("�������ֵ��StockID=%d,ChangeScore=%I64d"), TraceLevel_Exception, pModifyStockScore->wStockID, pModifyStockScore->lChangeScore);
		}

		return true;
	}
	case SUB_CS_ADJUST_JACKPOT_SCORE: //�����ʽ�ֵ
	{
		auto pModifyJackpotScore = (CMD_CS_AdjustJackpotScore*)pData;

		//�����ʽ�
		if (m_ShareStockManager.AdjustJackpotScore(pModifyJackpotScore->wStockID, pModifyJackpotScore->wLevelID, pModifyJackpotScore->lChangeScore))
		{
			//�������
			DBR_AdjustJackpotScore AdjustJackpotScore = {};
			AdjustJackpotScore.wStockID = pModifyJackpotScore->wStockID;
			AdjustJackpotScore.wLevelID = pModifyJackpotScore->wLevelID;
			AdjustJackpotScore.lChangeScore = pModifyJackpotScore->lChangeScore;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_ADJUST_JACKPOT_SCORE, 0, 0, 0, &AdjustJackpotScore, sizeof(AdjustJackpotScore));

			//�����־
			CTraceService::TraceString(TEXT("�����ʽ�ֵ��StockID=%d,LevelID=%d,ChangeScore=%I64d"), TraceLevel_Exception, pModifyJackpotScore->wStockID, pModifyJackpotScore->wLevelID, pModifyJackpotScore->lChangeScore);
		}

		return true;
	}	
	case SUB_CS_UPDATE_SPLAYER_CONTROL: //���´�R����
	{
		//����ṹ
		DBR_LoadGlobalConfig LoadGlobalConfig;
		ZeroMemory(&LoadGlobalConfig, sizeof(LoadGlobalConfig));

		//��������
		LoadGlobalConfig.dwConfigMask = CONFIG_MASK_SPLAYER_CONTROL;

		//��������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_LOAD_GLOBAL_CONFIG, 0, 0, 0, &LoadGlobalConfig, sizeof(LoadGlobalConfig));

		return true;
	}	
	}

	return false;
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
			//��������
			if (pCommandCode->dwCommandParam != 0 )
			{
				//����ṹ
				DBR_LoadGlobalConfig LoadGlobalConfig;
				ZeroMemory(&LoadGlobalConfig, sizeof(LoadGlobalConfig));

				//��������
				LoadGlobalConfig.dwConfigMask = pCommandCode->dwCommandParam;

				//��������
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_LOAD_GLOBAL_CONFIG, 0, 0, 0, &LoadGlobalConfig, sizeof(LoadGlobalConfig));
			}
		}

		return true;
	}
	}

	return true;
}

//��Ϸ����
bool CAttemperEngineSink::OnDBResultGameKindItem(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//Ч�����
	ASSERT(wDataSize % sizeof(DBO_GameKind) == 0);
	if (wDataSize % sizeof(DBO_GameKind) != 0) return false;

	//��������
	auto pGameKind = (DBO_GameKind*)pData;
	WORD wItemCount = wDataSize / sizeof(DBO_GameKind);	

	//��������
	for (WORD i = 0; i < wItemCount; i++)
	{
		//��������
		tagGameKindEx GameKind;
		ZeroMemory(&GameKind, sizeof(GameKind));

		//��������
		GameKind.wModuleID = (pGameKind + i)->wModuleID;
		GameKind.wTypeID = (pGameKind + i)->wTypeID;
		GameKind.wJoinID = (pGameKind + i)->wJoinID;
		GameKind.wSortID = (pGameKind + i)->wSortID;
		GameKind.wKindID = (pGameKind + i)->wKindID;
		StringCchCopy(GameKind.szKindName, CountArray(GameKind.szKindName), (pGameKind + i)->szKindName);
		StringCchCopy(GameKind.szProcessName, CountArray(GameKind.szProcessName), (pGameKind + i)->szProcessName);

		//֧������
		GameKind.wSupportMask = (pGameKind + i)->wSupportMask;

		//�����б�
		if ((pGameKind + i)->bNullity == false)
		{
			//�������� 
			auto pGameKind = m_StockGameKind.SearchStockItem(GameKind.wKindID);
			if (pGameKind == NULL)		
			{
				pGameKind = m_StockGameKind.CreateStockItem(GameKind.wKindID);
			}

			//��������
			if (pGameKind != NULL)
			{
				CopyMemory(pGameKind,&GameKind,sizeof(GameKind));
			}
		}
		else
		{
			m_StockGameKind.RemoveStockItem(GameKind.wKindID);
		}
	}

	return true;
}

//�������
bool CAttemperEngineSink::OnDBResultStockListItem(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//У�����
	ASSERT(wDataSize%sizeof(tagStockItem)==0);
	if (wDataSize % sizeof(tagStockItem) != 0) return false;

	//��ȡ����
	auto pStockItem = (tagStockItem*)pData;
	auto wItemCount = wDataSize / sizeof(tagStockItem);	

	//��ӿ��
	m_ShareStockManager.AppendStockItem(pStockItem, wItemCount);	

	return true;
}

//��R����
bool CAttemperEngineSink::OnDBResultSuperPlayerControl(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//У�����
	ASSERT(wDataSize==sizeof(tagSuperPlayerControl));
	if (wDataSize != sizeof(tagSuperPlayerControl)) return false;

	//��ȡ����
	auto pSuperPlayerControl = (tagSuperPlayerControl*)pData;

	//Ӧ�ô�R����
	m_ShareStockManager.ApplySuperPlayerControl(pSuperPlayerControl);

	return true;
}

//�б����
bool CAttemperEngineSink::OnDBResultLoadConfigFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�¼�֪ͨ
	CP_ControlResult ControlResult;
	ControlResult.cbSuccess = ER_SUCCESS;
	SendUIControlPacket(UI_LOAD_CONFIG_RESULT, &ControlResult, sizeof(ControlResult));

	return true;
}

//��ѯ���
bool CAttemperEngineSink::OnDBResultQueryStockItem(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize == sizeof(DBO_QueryStockItem));
	if (wDataSize != sizeof(DBO_QueryStockItem)) return false;

	//��ȡ����
	auto pQueryStockItem = (DBO_QueryStockItem*)pData;

	//��ӿ��
	if (pQueryStockItem->wCommandID == SUB_CS_APPEND_STOCK_ITEM)
	{
		m_ShareStockManager.AppendStockItem(&pQueryStockItem->StockItem, 1);
	}

	//�༭���
	if (pQueryStockItem->wCommandID == SUB_CS_MODIFY_STOCK_ITEM)
	{
		//�޸Ŀ��
		m_ShareStockManager.ModifyStockItem(&pQueryStockItem->StockItem);
	}	

	return true;
}

//���¿��
bool CAttemperEngineSink::OnDBResultUpdateStockScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�Ƴ���ʶ
	RemoveStockUpdateID(dwTokenID);

	//�ж�״̬
	if (m_bWillConcludeService && m_StockUpdateArray.GetCount()==0)
	{
		//�¼�֪ͨ
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess = ER_SUCCESS;
		SendUIControlPacket(UI_SERVICE_WILLSTOP_RESULT, &ControlResult, sizeof(ControlResult));
	}

	return true;
}

//����״̬
bool CAttemperEngineSink::OnDBResultUpdateStockState(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�Ƴ���ʶ
	RemoveStockUpdateID(dwTokenID);

	//�ж�״̬
	if (m_bWillConcludeService && m_StockUpdateArray.GetCount() == 0)
	{
		//�¼�֪ͨ
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess = ER_SUCCESS;
		SendUIControlPacket(UI_SERVICE_WILLSTOP_RESULT, &ControlResult, sizeof(ControlResult));
	}

	return true;
}

//���²ʽ�
bool CAttemperEngineSink::OnDBResultUpdateJackpotScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�Ƴ���ʶ
	RemoveStockUpdateID(dwTokenID);

	//�ж�״̬
	if (m_bWillConcludeService && m_StockUpdateArray.GetCount() == 0)
	{
		//�¼�֪ͨ
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess = ER_SUCCESS;
		SendUIControlPacket(UI_SERVICE_WILLSTOP_RESULT, &ControlResult, sizeof(ControlResult));
	}

	return true;
}

//�ϱ��ʽ�
VOID CAttemperEngineSink::ReportGameJackpot()
{
	//��������
	CMD_CS_C_ReportJackpot ReportJackpot;
	ZeroMemory(&ReportJackpot, sizeof(ReportJackpot));

	//��������
	auto pGameKind = m_StockGameKind.GetHeadStockItem();
	while (pGameKind != NULL)
	{
		auto pCurrentGameKind = pGameKind;
		pGameKind = m_StockGameKind.GetNextStockItem(pCurrentGameKind);

		//���ñ���
		ReportJackpot.lJackpotScore = 0;
		ReportJackpot.wKindID = pCurrentGameKind->wKindID;

		//ͳ�Ʋʽ�
		if (m_ShareStockManager.CollectJackpotScore(STOCK_KIND_GOLD, ReportJackpot.wKindID, ReportJackpot.lJackpotScore))
		{
			//������Ϣ
			ASSERT(m_pITCPSocketCenter != NULL);
			m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_REPORT_JACKPOT, &ReportJackpot, sizeof(ReportJackpot), 0);
		}
	}
}

//�Ƴ���ʶ
VOID CAttemperEngineSink::RemoveStockUpdateID(DWORD dwUpdateID)
{
	for (int i = 0; i < m_StockUpdateArray.GetCount(); i++)
	{
		if (m_StockUpdateArray[i] == dwUpdateID)
		{
			m_StockUpdateArray.RemoveAt(i);
		}
	}
}

//��������
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//��������
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->m_UIControlSink.PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//////////////////////////////////////////////////////////////////////////
