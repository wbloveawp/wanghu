#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_REPORT_ONLINE			1											//�ϱ�����

//ʱ���ʶ
#define IDI_CONNECT_CENTER			10											//������ʶ

//////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_bNeedBattle=true;

	//״̬����
	m_pServiceOption=NULL;
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_TCPSocketCenter=NULL;


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

	//���ýӿ�
	m_GlobalInfoManager.SetIAttemperEngine(m_pIAttemperEngine);
	m_GlobalInfoManager.SetServiceAttribute(m_ServiceAttribute);

	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//״̬����
	m_bNeedBattle=true;

	//�������
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_TCPSocketCenter=NULL;

	//���ö���
	m_GlobalInfoManager.ResetData();

	//ɾ������
	SafeDeleteArray(m_pBindParameter);	

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	switch (dwIdentifier)
	{
	case CT_CONNECT_CENTER:		//��������
		{
			//��ȡ��ַ					
			tagAddressInfo * pAddressInfo=&m_pInitParameter->m_CenterAddress;

			//��������
			m_TCPSocketCenter->Connect(htonl(pAddressInfo->dwServiceAddr), m_pInitParameter->m_wCenterPort);

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
	//�ϱ�����
	if (dwTimerID == IDI_REPORT_ONLINE)
	{
		//����ṹ
		CMD_CS_C_ServiceOnline ServiceOnline;
		ZeroMemory(&ServiceOnline,sizeof(ServiceOnline));

		//��������
		ServiceOnline.wOnlineCount=GetOnlineCount();

		//������Ϣ
		m_TCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_SERVICE_ONLINE,&ServiceOnline,sizeof(ServiceOnline),0);

		return true;
	}

	//��������
	if (dwTimerID==IDI_CONNECT_CENTER)
	{
		//��ȡ��ַ					
		tagAddressInfo * pAddressInfo=&m_pInitParameter->m_CenterAddress;

		//��������
		m_TCPSocketCenter->Connect(htonl(pAddressInfo->dwServiceAddr), m_pInitParameter->m_wCenterPort);

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڳ����������ķ�����..."),TraceLevel_Normal);

		return true;
	}
	
	//������Ϣ
	WORD wServiceID = LOWORD(dwTimerID);
	WORD wServiceIndex = HIWORD(dwTimerID)-1;		

	//�رմ���		
	CGlobalServiceInfo * pGlobalServiceItem = m_GlobalInfoManager.SearchServiceInfo(wServiceIndex, wServiceID);
	if (pGlobalServiceItem != NULL && pGlobalServiceItem->m_pITCPSocketService != NULL)
	{
		//���ӷ���
		pGlobalServiceItem->m_pITCPSocketService->Connect(pGlobalServiceItem->m_ServiceItem.szServiceDomain, pGlobalServiceItem->m_ServiceItem.wServicePort);

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڳ�������[%d]��%s..."), TraceLevel_Normal, wServiceID, m_ServiceAttribute[wServiceIndex].szServiceName);
	}

	return true;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//��ȡ����
	ASSERT (LOWORD(dwSocketID)<m_pServiceOption->wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pServiceOption->wMaxConnect) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//���ñ���	
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientIP=dwClientIP;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);	

	//�û���Ϣ
	pBindParameter->dwUserID = 0;		
	ZeroMemory(pBindParameter->szLogonToken,sizeof(pBindParameter->szLogonToken));

	//������Ϣ
	ZeroMemory(pBindParameter->LinkState, sizeof(pBindParameter->LinkState));
	ZeroMemory(pBindParameter->wServiceID, sizeof(pBindParameter->wServiceID));		

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//����ģ��
	WORD wServiceModule=HICMMD(Command.wMainCmdID);

	//У��ģ��
	if (wServiceModule > SERVICE_COUNT)
	{
		return false;
	}

	//���ط���
	if (wServiceModule == SERVICE_MODULE_GATE)
	{
		return OnTCPNetworkMainGateService(Command, pData, wDataSize, dwSocketID, dwTokenID);
	}

	return OnTCPNetworkMainInsideService(Command, pData, wDataSize, dwSocketID, dwTokenID);
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��ȡ����
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//�뿪����
	for (WORD i=0; i<CountArray(pBindParameter->LinkState); i++)
	{
		//У��״̬
		if (pBindParameter->LinkState[i] == LS_NULL) continue;

		//ע������
		LogoutToken(i, pBindParameter->wServiceID[i], pBindParameter->dwSocketID);
	}

	//�����Ϣ
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason)
{
	//��������
	if (dwServiceID==NETWORK_CENTER)
	{
		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("�����ķ����������ӹر��ˣ�%ld �����������"),TraceLevel_Warning, m_pInitParameter->m_wConnectTime);

		//����ʱ��
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L,1,0);

		return true;
	}
	//��������
	else
	{
		//��������
		WORD wServiceID = LOWORD(dwServiceID);
		WORD wServiceModule = HIWORD(dwServiceID);
		WORD wServiceIndex= wServiceModule -1;

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("��[%d]��%s�����ӶϿ���"), TraceLevel_Warning, wServiceID, m_ServiceAttribute[wServiceIndex].szServiceName);

		//�Ƴ�����
		m_GlobalInfoManager.DeleteServiceItem(wServiceIndex, wServiceID);

		//����ʱ��
		ASSERT(m_pITimerEngine != NULL);
		m_pITimerEngine->SetTimer(dwServiceID, 2000L, 1, 0);
		
		//�ر�����
		ShutdownServiceLink(wServiceModule, wServiceID);

		return true;
	}

	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode)
{
	//��������
	if (dwServiceID == NETWORK_CENTER)
	{
		//�����ж�
		if (nErrorCode!=0)
		{
			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ķ���������ʧ�� [ %ld ]��%ld �����������"),TraceLevel_Warning, nErrorCode, m_pInitParameter->m_wConnectTime);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,2000L,1,0);

			return false;
		}

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڻ�ȡ������Ϣ..."),TraceLevel_Normal);

		//��������
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService,sizeof(RegisterService));

		//���ñ���	
		RegisterService.wServiceMask = 0xFFFF;
		RegisterService.wServiceID=m_pServiceOption->wServiceID;
		RegisterService.wServiceModule= SERVICE_MODULE_GATE;
		RegisterService.wServicePort=m_pServiceOption->wServicePort;
		RegisterService.wMaxConnect = m_pServiceOption->wMaxConnect;
		RegisterService.wServiceKindID = m_pServiceOption->wServiceKindID;		
		StringCchCopy(RegisterService.szServiceDomain, CountArray(RegisterService.szServiceDomain), m_pServiceOption->szServiceDomain);

		//��������
		m_TCPSocketCenter->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_SERVICE, &RegisterService, sizeof(RegisterService), 0);

		return true;
	}
	else
	{
		//��������
		WORD wServiceID = LOWORD(dwServiceID);
		WORD wServiceModule = HIWORD(dwServiceID);		
		WORD wServiceIndex = HIWORD(dwServiceID) - 1;

		//�����ж�
		if (nErrorCode != 0)
		{
			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("��[%d]��%s����ʧ�� [ %ld ]��%ld �����������"), TraceLevel_Warning, wServiceID, 
				m_ServiceAttribute[wServiceIndex].szServiceName, nErrorCode, m_pInitParameter->m_wConnectTime);

			//����ʱ��
			ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(GetTimerID(wServiceIndex, wServiceID), 2000L, 1, 0);

			return false;
		}

		//��ȡ�ӿ�
		ITCPSocketService * pITCPSocketService = m_GlobalInfoManager.GetSocketService(wServiceIndex, wServiceID);
		if (pITCPSocketService != NULL)
		{
			//��������
			CMD_CS_C_RegisterService RegisterService;
			ZeroMemory(&RegisterService, sizeof(RegisterService));

			//���ñ���	
			RegisterService.wServiceID = m_pServiceOption->wServiceID;
			RegisterService.wServiceModule = SERVICE_MODULE_GATE;
			RegisterService.wServicePort = m_pServiceOption->wServicePort;
			RegisterService.wMaxConnect = m_pServiceOption->wMaxConnect;
			RegisterService.wServiceKindID = m_pServiceOption->wServiceKindID;
			StringCchCopy(RegisterService.szServiceDomain, CountArray(RegisterService.szServiceDomain), m_pServiceOption->szServiceDomain);

			//��������
			pITCPSocketService->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_SERVICE, &RegisterService, sizeof(RegisterService), 0);
		}

		return true;
	}

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	try
	{
		//��ȡ������
		WORD wMainCmdID = LOCMMD(Command.wMainCmdID);

		//��������
		if (dwServiceID == NETWORK_CENTER)
		{
			switch (wMainCmdID)
			{
			case MDM_CS_REGISTER:		//ע�����
				{
					return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
				}
			case MDM_CS_COMMON_SERVICE:	//������Ϣ
				{
					return OnTCPSocketMainServiceInfo(Command.wSubCmdID,pData,wDataSize);
				}		
			}

			return true;
		}
		else
		{
			switch (wMainCmdID)
			{
			case MDM_CS_REGISTER:		//ע�����
				{
					return OnTCPSocketMainRegister(dwServiceID, Command, pData, wDataSize);
				}
			case MDM_CS_COMMON_SERVICE:	//ͨ�÷���
				{
					return OnTCPSocketMainCommonService(dwServiceID, Command, pData, wDataSize, pdwTokenID, wTokenCount);
				}
			default:					//��������	
				{					
					return OnTCPSocketMainInsideService(dwServiceID, Command, pData, wDataSize, pdwTokenID, wTokenCount);
				}
			}

			return true;
		}		
	} 
	catch (...)
	{
		CTraceService::TraceString(TEXT("wServiceID=%d,wMainCmdID=%d,wSubCmdID=%d"),TraceLevel_Exception, dwServiceID, Command.wMainCmdID, Command.wSubCmdID);
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
	case SUB_CS_S_REGISTER_SUCCESS:		//ע��ɹ�
		{
			//�¼�����
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CENTER_RESULT ,&ControlResult,sizeof(ControlResult));		

			//���ö�ʱ��
			ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_REPORT_ONLINE,m_pInitParameter->m_wReportTime,-1,NULL);

			return true;
		}
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
		{
			//��������
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;

			//Ч�����
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//�رմ���
			m_TCPSocketCenter->CloseSocket();

			//��ʾ��Ϣ
			CW2CT strDescribeString(pRegisterFailure->szDescribeString);
			if (lstrlen(strDescribeString)>0) CTraceService::TraceString(strDescribeString,TraceLevel_Exception);

			return true;
		}
	}

	return true;
}

//���ط���
bool CAttemperEngineSink::OnTCPNetworkMainGateService(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ����
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);

	//����ģ��
	WORD wSubCmdID = Command.wSubCmdID;
	WORD wMainCmdID = LOCMMD(Command.wMainCmdID);

	//���ӷ���
	if (wMainCmdID == MDM_GG_GATE_SERVICE && wSubCmdID == SUB_GG_C_LINK_SERVICE)
	{
		//��ȡ����
		CMD_GG_C_LinkService * pLinkService = (CMD_GG_C_LinkService *)pData;

		//��������
		WORD wServiceIndex = pLinkService->wServiceModule - 1;

		//��������
		if (wServiceIndex > CountArray(pBindParameter->wServiceID))
		{
			return false;
		}

		//У������
		if (pLinkService->wServiceModule != SERVICE_MODULE_LOGON)
		{
			//У���û�
			if (pLinkService->dwUserID != pBindParameter->dwUserID || lstrcmp(pBindParameter->szLogonToken, pLinkService->szLogonToken) != 0)
			{
				//����ṹ
				CMD_GG_S_LinkFailure LinkFailure;
				ZeroMemory(&LinkFailure, sizeof(LinkFailure));

				//������Ϣ
				LinkFailure.wServiceID = pLinkService->wServiceID;
				LinkFailure.wServiceModule = pLinkService->wServiceModule;

				//������Ϣ
				LinkFailure.lErrorCode = 1;
				StringCchCopy(LinkFailure.szErrorDescribe, CountArray(LinkFailure.szErrorDescribe), CWHService::FormatMessage(MIM_GATE_0004, L"��¼������֤ʧ�ܣ�"));

				//������Ϣ
				WORD wPacketSize = sizeof(LinkFailure) - sizeof(LinkFailure.szErrorDescribe) + CountStringBuffer(LinkFailure.szErrorDescribe);
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_S_LINK_FAILURE, &LinkFailure, wPacketSize, NULL, 0);

				return false;
			}
		}

		//У��״̬
		if (pBindParameter->LinkState[wServiceIndex] != LS_NULL)
		{
			//����ṹ
			CMD_GG_S_LinkFailure LinkFailure;
			ZeroMemory(&LinkFailure, sizeof(LinkFailure));

			//������Ϣ
			LinkFailure.wServiceID= pLinkService->wServiceID;
			LinkFailure.wServiceModule = pLinkService->wServiceModule;

			//������Ϣ
			LinkFailure.lErrorCode = 1;
			StringCchCopy(LinkFailure.szErrorDescribe, CountArray(LinkFailure.szErrorDescribe), CWHService::FormatMessage(MIM_GATE_0001, L"��ǰ����ģ������״̬�쳣�������µ�¼�ٴν��룡"));

			//������Ϣ
			WORD wPacketSize = sizeof(LinkFailure) - sizeof(LinkFailure.szErrorDescribe) + CountStringBuffer(LinkFailure.szErrorDescribe);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MAKECMMD(MDM_GG_GATE_SERVICE,SERVICE_MODULE_GATE), SUB_GG_S_LINK_FAILURE, &LinkFailure, wPacketSize, NULL, 0);

			return false;
		}

		//���ñ���
		pBindParameter->LinkRecord[wServiceIndex].wTimes = 0;
		pBindParameter->LinkRecord[wServiceIndex].wReqServiceID = pLinkService->wServiceID;		
		CopyMemory(pBindParameter->LinkRecord[wServiceIndex].cbLinkExtend, pLinkService->cbLinkExtend, sizeof(pLinkService->cbLinkExtend));

		//�������
		pLinkService->wServiceID = AssignInsideService(pLinkService->wServiceModule, dwSocketID);		

		//У���ʶ
		if (pLinkService->wServiceID == INVALID_SERVICE_ID)
		{
			//��·��ӳ��
			if (m_ServiceAttribute[wServiceIndex].wServiceMapKind != MAP_KIND_ROUTE)
			{
				//���ü�¼
				ZeroMemory(&pBindParameter->LinkRecord[wServiceIndex], sizeof(pBindParameter->LinkRecord[wServiceIndex]));

				//����ṹ
				CMD_GG_S_LinkFailure LinkFailure;
				ZeroMemory(&LinkFailure, sizeof(LinkFailure));

				//������Ϣ
				LinkFailure.wServiceID = pLinkService->wServiceID;
				LinkFailure.wServiceModule = pLinkService->wServiceModule;

				//������Ϣ			
				LinkFailure.lErrorCode = 1;
				StringCchCopy(LinkFailure.szErrorDescribe, CountArray(LinkFailure.szErrorDescribe), CWHService::FormatMessage(MIM_GATE_0002, L"��������æ�������Ժ��ٴγ��ԣ�"));

				//������Ϣ
				WORD wPacketSize = sizeof(LinkFailure) - sizeof(LinkFailure.szErrorDescribe) + CountStringBuffer(LinkFailure.szErrorDescribe);
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_S_LINK_FAILURE, &LinkFailure, wPacketSize, NULL, 0);
			}

			return true;
		}

		//����У��
		if (pLinkService->wServiceID > NONE_SERVICE_ID)
		{
			//���ò���
			pBindParameter->LinkState[wServiceIndex] = LS_SHAKEHAND;
			pBindParameter->wServiceID[wServiceIndex] = pLinkService->wServiceID;

			//ע������
			CMD_CS_C_RegisterToken RegisterToken;
			ZeroMemory(&RegisterToken, sizeof(RegisterToken));

			//����ṹ 
			RegisterToken.dwTokenID = dwSocketID;
			RegisterToken.dwUserID = pBindParameter->dwUserID;
			RegisterToken.dwClientIP = pBindParameter->dwClientIP;

			//������Ϣ
			SendDataToService(wServiceIndex, pLinkService->wServiceID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_C_REGISTER_TOKEN, &RegisterToken, sizeof(RegisterToken));
		}
		else
		{
			//���ò���
			pBindParameter->LinkState[wServiceIndex] = LS_LINKING;
			pBindParameter->wServiceID[wServiceIndex] = pLinkService->wServiceID;

			//����ṹ
			CMD_GG_S_LinkSuccess LinkSuccess;
			ZeroMemory(&LinkSuccess, sizeof(LinkSuccess));

			//������Ϣ
			LinkSuccess.wServiceID = pLinkService->wServiceID;
			LinkSuccess.wServiceModule = pLinkService->wServiceModule;

			//������Ϣ
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_S_LINK_SUCCESS, &LinkSuccess, sizeof(LinkSuccess), NULL, 0);
		}

		return true;
	}

	//�뿪����
	if (wMainCmdID == MDM_GG_GATE_SERVICE && wSubCmdID == SUB_GG_C_DISLINK_SERVICE)
	{
		//��ȡ����
		CMD_GG_C_DislinkService * pDislinkService = (CMD_GG_C_DislinkService *)pData;

		//У������
		WORD wServiceIndex = pDislinkService->wServiceModule-1;
		if (wServiceIndex >= CountArray(pBindParameter->wServiceID))
		{
			return false;
		}

		//У��״̬
		if (pBindParameter->LinkState[wServiceIndex] == LS_NULL)
		{
			return true;
		}

		//ע������
		LogoutToken(wServiceIndex,pBindParameter->wServiceID[wServiceIndex], dwSocketID);

		//�ر�����
		ShutdownServiceLink(pDislinkService->wServiceModule, pBindParameter->wServiceID[wServiceIndex], dwSocketID);

		//ת����Ϣ
		return true;
	}	

	return false;
}

//�ڲ�����
bool CAttemperEngineSink::OnTCPNetworkMainInsideService(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ����
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=GetBindParameter(wBindIndex);

	//����ģ��
	WORD wServiceModule = HICMMD(Command.wMainCmdID);
	WORD wServiceIndex = wServiceModule - 1;

	//У��ģ��
	if (wServiceIndex >= CountArray(pBindParameter->wServiceID))
	{
		return false;
	}
	
	//��������
	auto LinkState = pBindParameter->LinkState[wServiceIndex];
	auto wServiceID = pBindParameter->wServiceID[wServiceIndex];

	//У��״̬
	if (LinkState != LS_LINKING)
	{
		//����ṹ
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage, sizeof(SystemMessage));

		//��������						
		SystemMessage.wType = SMT_EJECT;		
		StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), CWHService::FormatMessage(MIM_GATE_0003, L"������ķ�������æ��"));

		//��ӡ��Ϣ
		CTraceService::TraceString(TEXT("ServiceModule=%d,ErrorMsg=\"%s\""), TraceLevel_Info, wServiceModule,SystemMessage.szString);

		//������Ϣ
		WORD wPacketSize = sizeof(SystemMessage) - sizeof(SystemMessage.szString) + CountStringBuffer(SystemMessage.szString);
		m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(MDM_CM_SYSTEM, SERVICE_MODULE_GATE), SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wPacketSize, NULL, 0);

		return true;
	}

	//У���ʶ
	if (wServiceID == NONE_SERVICE_ID)
	{
		//��ȡ����
		auto pServiceHash = (tagServiceHash*)pData;

		//ӳ�����
		auto pGlobalServiceItem = m_GlobalInfoManager.MapServiceInfo(wServiceIndex, (WORD)pServiceHash->dwHashParam, pServiceHash->dwHashKeyID);
		if (pGlobalServiceItem == NULL)
		{
			//����ṹ
			CMD_CM_SystemMessage SystemMessage;
			ZeroMemory(&SystemMessage, sizeof(SystemMessage));

			//��������						
			SystemMessage.wType = SMT_EJECT;
			StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), CWHService::FormatMessage(MIM_GATE_0003, TEXT("������ķ��������ɴ")));

			//��ӡ��Ϣ
			CTraceService::TraceString(TEXT("[���񲻿ɴ�] ServiceModule=%d,UserID=%d,HashKeyID=%d,HashParam=%d"), TraceLevel_Info, wServiceModule,pBindParameter->dwUserID, pServiceHash->dwHashKeyID, pServiceHash->dwHashParam);

			//������Ϣ
			WORD wPacketSize = sizeof(SystemMessage) - sizeof(SystemMessage.szString) + CountStringBuffer(SystemMessage.szString);
			m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(MDM_CM_SYSTEM, SERVICE_MODULE_GATE), SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wPacketSize, NULL, 0);

			return true;
		}

		//���ñ�ʶ		
		wServiceID = pGlobalServiceItem->GetServiceID();	

		//��չ��Ϣ
		*(DWORD*)pServiceHash->cbExtendData = pBindParameter->dwUserID;
	}

	//ת����Ϣ
	return SendDataToService(wServiceIndex, wServiceID, dwSocketID, Command.wMainCmdID, Command.wSubCmdID,pData,wDataSize);
}

//�б��¼�
bool CAttemperEngineSink::OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_SERVICE_ATTRIB:	//��������
	{
		//Ч�����
		ASSERT (wDataSize % sizeof(tagServiceAttrib) == 0);
		if (wDataSize % sizeof(tagServiceAttrib) != 0) return false;

		//������Ŀ
		auto pServiceAttrib = (tagServiceAttrib*)pData;
		auto wItemCount = wDataSize/sizeof(tagServiceAttrib);

		for (int i = 0; i < wItemCount; i++)
		{
			//��������
			WORD wServiceIndex = pServiceAttrib->wServiceModule - 1;

			//У��ģ��
			if (wServiceIndex < CountArray(m_ServiceAttribute))
			{
				auto pServiceParameter = &m_ServiceAttribute[wServiceIndex];

				//��������
				CopyMemory(&m_ServiceAttribute[wServiceIndex], pServiceAttrib,sizeof(m_ServiceAttribute[0]));
			}

			pServiceAttrib++;
		}

		return true;
	}
	case SUB_CS_S_SERVICE_INSERT:	//�������
	{
		//Ч�����
		ASSERT(wDataSize % sizeof(tagServiceItem) == 0);
		if (wDataSize % sizeof(tagServiceItem) != 0) return false;

		//��������
		WORD wItemCount = wDataSize / sizeof(tagServiceItem);
		tagServiceItem* pServiceItem = (tagServiceItem*)pData;		

		for (WORD i = 0; i < wItemCount; i++)
		{
			//��������
			WORD wServiceIndex = pServiceItem->wServiceModule-1;
			if (wServiceIndex >= CountArray(m_ServiceAttribute)) continue;

			//У������
			if (!m_ServiceAttribute[wServiceIndex].bIsExternal) continue;

			//��������
			CGlobalServiceInfo* pGlobalServiceItem = m_GlobalInfoManager.SearchServiceInfo(wServiceIndex, pServiceItem->wServiceID);
			if (pGlobalServiceItem != NULL && pGlobalServiceItem->m_pITCPSocketService != NULL) continue;

			//�����ʶ
			DWORD dwServiceID = MAKELONG(pServiceItem->wServiceID, pServiceItem->wServiceModule);

			//�������
			pGlobalServiceItem = m_GlobalInfoManager.ActiveServiceInfo(wServiceIndex, dwServiceID, *pServiceItem);
			if (pGlobalServiceItem == NULL) break;

			//ɾ����ʱ��
			m_pITimerEngine->KillTimer(MAKELONG(pServiceItem->wServiceID, pServiceItem->wServiceModule));

			//���ӷ���
			pGlobalServiceItem->m_pITCPSocketService->Connect(pGlobalServiceItem->m_ServiceItem.szServiceDomain, pGlobalServiceItem->m_ServiceItem.wServicePort);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ڳ�������[%d]��%s..."), TraceLevel_Normal, pServiceItem->wServiceID, m_ServiceAttribute[wServiceIndex].szServiceName);

			pServiceItem++;
		}

		return true;
	}
	case SUB_CS_S_SERVER_INSERT:	//�������
	{
		//Ч�����
		ASSERT(wDataSize % sizeof(tagGameServer) == 0);
		if (wDataSize % sizeof(tagGameServer) != 0) return false;

		//��������
		WORD wItemCount = wDataSize / sizeof(tagGameServer);
		tagGameServer* pGameServer = (tagGameServer*)pData;

		//��ȡ����
		CServiceUnits* pServiceUnits = CServiceUnits::g_pServiceUnits;

		//��ȡ����
		for (WORD i = 0; i < wItemCount; i++)
		{
			//��������
			WORD wServiceIndex = SERVICE_MODULE_GAME-1;
			if (wServiceIndex >= CountArray(m_ServiceAttribute)) continue;

			//У������
			if (!m_ServiceAttribute[wServiceIndex].bIsExternal) continue;

			//��������
			CGlobalServiceInfo* pGlobalServiceItem = m_GlobalInfoManager.SearchServiceInfo(wServiceIndex, pGameServer->wServerID);
			if (pGlobalServiceItem != NULL && pGlobalServiceItem->m_pITCPSocketService != NULL) continue;

			//��������
			tagServiceItem ServiceItem;
			ZeroMemory(&ServiceItem, sizeof(ServiceItem));

			//��������
			ServiceItem.wServiceID = pGameServer->wServerID;
			ServiceItem.wServiceModule = SERVICE_MODULE_GAME;
			ServiceItem.wServicePort = pGameServer->wServerPort;
			ServiceItem.dwServiceAddr = pGameServer->dwServerAddr;
			StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), TEXT("��Ϸ������"));
			StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain), pGameServer->szServerDomain);

			//�����ʶ
			DWORD dwTimerID = MAKELONG(ServiceItem.wServiceID, SERVICE_MODULE_GAME);
			DWORD dwServiceID = MAKELONG(ServiceItem.wServiceID, SERVICE_MODULE_GAME);			

			//��������
			pGlobalServiceItem = m_GlobalInfoManager.ActiveServiceInfo(wServiceIndex, dwServiceID, ServiceItem);
			if (pGlobalServiceItem == NULL) break;

			//��չ����
			CopyMemory(pGlobalServiceItem->GetExtendData(), pGameServer, sizeof(tagGameServer));

			//ɾ����ʱ��
			m_pITimerEngine->KillTimer(dwTimerID);

			//���ӷ���
			pGlobalServiceItem->m_pITCPSocketService->Connect(ServiceItem.szServiceDomain, ServiceItem.wServicePort);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ڳ�������[%d]��%s..."), TraceLevel_Normal, ServiceItem.wServiceID, ServiceItem.szServiceName);

			pGameServer++;
		}

		return true;
	}
	case SUB_CS_S_SERVER_MODIFY:	//�����޸�
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_CS_S_ServerModify));
		if (wDataSize != sizeof(CMD_CS_S_ServerModify)) return false;

		//��Ϣ����
		CMD_CS_S_ServerModify* pServerModify = (CMD_CS_S_ServerModify*)pData;

		//���ҷ���
		auto pGlobalServerItem = m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GAME-1, pServerModify->wServerID);
		if (pGlobalServerItem != NULL)
		{
			auto pGameServer = (tagGameServer*)pGlobalServerItem->GetExtendData();
			if (pGameServer != NULL)
			{
				pGameServer->wServerLevel = pServerModify->wServerLevel;
				pGameServer->dwMasterRule = pServerModify->dwMasterRule;
				pGameServer->dwMaxPlayer = pServerModify->dwMaxPlayer;
				pGameServer->dwOnLineCount = pServerModify->dwOnLineCount;
			}			
		}

		return true;
	}
	case SUB_CS_S_SERVICE_REMOVE:	//�����Ƴ�
	case SUB_CS_S_SERVER_REMOVE:	//�����Ƴ�
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_CS_S_ServiceRemove));
		if (wDataSize != sizeof(CMD_CS_S_ServiceRemove)) return false;

		//��ȡ����
		CMD_CS_S_ServiceRemove* pServiceRemove = (CMD_CS_S_ServiceRemove*)pData;

		//У������
		WORD wServiceIndex = pServiceRemove->wServiceModule-1;
		if (wServiceIndex >= CountArray(m_ServiceAttribute)) return false;

		//ʱ���ʶ
		DWORD dwTimerID = MAKELONG(pServiceRemove->wServiceID, pServiceRemove->wServiceModule);

		//ɾ����ʱ��
		m_pITimerEngine->KillTimer(dwTimerID);

		//��ѯ����
		CGlobalServiceInfo* pGlobalServiceItem = m_GlobalInfoManager.SearchServiceInfo(wServiceIndex, pServiceRemove->wServiceID);
		if (pGlobalServiceItem != NULL)
		{
			//�Ƴ�����
			m_GlobalInfoManager.DeleteServiceItem(wServiceIndex, pServiceRemove->wServiceID);
		}

		return true;
	}
	case SUB_CS_S_SERVICE_ONLINE:	//������Ϣ
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_CS_S_ServiceOnline));
		if (wDataSize != sizeof(CMD_CS_S_ServiceOnline)) return false;

		//��Ϣ����
		CMD_CS_S_ServiceOnline* pServiceOnline = (CMD_CS_S_ServiceOnline*)pData;

		//��Ϸ����
		if (pServiceOnline->wServiceModule == SERVICE_MODULE_GAME)
		{
			//��������
			auto wServiceIndex = pServiceOnline->wServiceModule-1;

			//���ҷ���
			auto pGlobalServerItem = m_GlobalInfoManager.SearchServiceInfo(wServiceIndex, pServiceOnline->wServiceID);
			if (pGlobalServerItem != NULL)
			{
				auto pGameServer = (tagGameServer*)pGlobalServerItem->GetExtendData();
				if (pGameServer != NULL)
				{
					pGameServer->dwOnLineCount = pServiceOnline->wOnlineCount;
				}
			}
		}	

		return true;
	}	
	case SUB_CS_S_APPEND_ROUTE_ITEM://���·��
	{
		//Ч������
		ASSERT((wDataSize - sizeof(CMD_CS_S_AppendRouteItem)) % sizeof(tagRouteItem) == 0);
		if ((wDataSize - sizeof(CMD_CS_S_AppendRouteItem)) % sizeof(tagRouteItem) != 0) return false;

		//��ȡ����
		auto pAppendRouteItem = (CMD_CS_S_AppendRouteItem*)pData;

		//·������
		auto pRouteItem = (tagRouteItem*)(pAppendRouteItem + 1);
		auto wItemCount = (wDataSize - sizeof(CMD_CS_S_AppendRouteItem)) / sizeof(tagRouteItem);

		//��������
		auto wServiceIndex = pAppendRouteItem->wServiceModule - 1;

		//���ҷ���
		auto pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(wServiceIndex, pAppendRouteItem->wServiceID);
		if (pGlobalServiceInfo != NULL)
		{
			//�Ƴ�·��
			if (pAppendRouteItem->bTheFirst)
			{
				pGlobalServiceInfo->RemoveRouteItem();
			}

			//���·��
			pGlobalServiceInfo->AppendRouteItem(pRouteItem, wItemCount);
		}

		return true;
	}
	case SUB_CS_S_REMOVE_ROUTE_ITEM://�Ƴ�·��
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_CS_S_RemoveRouteItem));
		if (wDataSize != sizeof(CMD_CS_S_RemoveRouteItem)) return false;

		//��ȡ����
		auto pRemoveRouteItem = (CMD_CS_S_RemoveRouteItem*)pData;

		//��������
		auto wServiceIndex = pRemoveRouteItem->wServiceModule - 1;

		//���ҷ���
		auto pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(wServiceIndex, pRemoveRouteItem->wServiceID);
		if (pGlobalServiceInfo != NULL)
		{
			//�Ƴ�·��
			pGlobalServiceInfo->RemoveRouteItem(pRemoveRouteItem->dwRouteID);
		}

		return true;
	}
	case SUB_CS_S_QUERY_ROUTE_RESULT://��ѯ���
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_CS_S_QueryRouteResult));
		if (wDataSize != sizeof(CMD_CS_S_QueryRouteResult)) return false;

		//��ȡ����
		auto pQueryRouteResult = (CMD_CS_S_QueryRouteResult*)pData;

		//�󶨲���
		auto pBindParameter = GetBindParameter(LOWORD(pQueryRouteResult->dwTokenID));
		if (pBindParameter == NULL) return true;
		if (pBindParameter->dwSocketID != pQueryRouteResult->dwTokenID) return true;

		//δ��ѯ������
		if (pQueryRouteResult->wServiceID == NONE_SERVICE_ID)
		{
			//����ṹ
			CMD_GG_S_LinkFailure LinkFailure;
			ZeroMemory(&LinkFailure, sizeof(LinkFailure));

			//������Ϣ
			LinkFailure.wServiceID = INVALID_SERVICE_ID;
			LinkFailure.wServiceModule = pQueryRouteResult->wServiceModule;

			//������Ϣ			
			LinkFailure.lErrorCode = 1;
			StringCchCopy(LinkFailure.szErrorDescribe, CountArray(LinkFailure.szErrorDescribe), CWHService::FormatMessage(MIM_GATE_0002, L"��������æ�������Ժ��ٴγ��ԣ�"));

			//������Ϣ
			WORD wPacketSize = sizeof(LinkFailure) - sizeof(LinkFailure.szErrorDescribe) + CountStringBuffer(LinkFailure.szErrorDescribe);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_S_LINK_FAILURE, &LinkFailure, wPacketSize, NULL, 0);
		}
		else
		{
			//��������
			auto wServiceIndex = pQueryRouteResult->wServiceModule - 1;
			auto pLinkRecord = &pBindParameter->LinkRecord[wServiceIndex];

			//���Ӽ�¼
			pLinkRecord->wServiceID[pLinkRecord->wTimes++] = pQueryRouteResult->wServiceID;

			//���ò���
			pBindParameter->LinkState[wServiceIndex] = LS_SHAKEHAND;
			pBindParameter->wServiceID[wServiceIndex] = pQueryRouteResult->wServiceID;

			//ע������
			CMD_CS_C_RegisterToken RegisterToken;
			ZeroMemory(&RegisterToken, sizeof(RegisterToken));

			//����ṹ 
			RegisterToken.dwUserID = pBindParameter->dwUserID;
			RegisterToken.dwClientIP = pBindParameter->dwClientIP;
			RegisterToken.dwTokenID = pQueryRouteResult->dwTokenID;

			//������Ϣ
			SendDataToService(wServiceIndex, pQueryRouteResult->wServiceID, RegisterToken.dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_C_REGISTER_TOKEN, &RegisterToken, sizeof(RegisterToken));
		}

		return true;
	}
	}

	return true;
}

//ע�����
bool CAttemperEngineSink::OnTCPSocketMainRegister(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize)
{
	//������Ϣ
	WORD wServiceID = LOWORD(dwServiceID);
	WORD wServiceIndex = HIWORD(dwServiceID)-1;	

	switch (Command.wSubCmdID)
	{
	case SUB_CS_S_REGISTER_SUCCESS:		//ע��ɹ�
		{
			CTraceService::TraceString(TEXT("[%d]��%s���ӳɹ�"), TraceLevel_Normal, wServiceID, m_ServiceAttribute[wServiceIndex].szServiceName);

			return true;
		}
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
		{
			//��������
			CMD_CS_S_RegisterFailure * pRegisterFailure = (CMD_CS_S_RegisterFailure *)pData;

			//Ч�����
			ASSERT(wDataSize >= (sizeof(CMD_CS_S_RegisterFailure) - sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize < (sizeof(CMD_CS_S_RegisterFailure) - sizeof(pRegisterFailure->szDescribeString))) return false;

			//�رմ���			
			ITCPSocketService * pITCPSocketService = m_GlobalInfoManager.GetSocketService(wServiceIndex,wServiceID);
			if (pITCPSocketService != NULL)
			{
				pITCPSocketService->CloseSocket();
			}

			//��ʾ��Ϣ
			CW2CT strDescribeString(pRegisterFailure->szDescribeString);
			if (lstrlen(strDescribeString) > 0) CTraceService::TraceString(strDescribeString, TraceLevel_Exception);

			return true;
		}
	}	

	return true;
}

//ͨ�÷���
bool CAttemperEngineSink::OnTCPSocketMainCommonService(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	//������Ϣ
	WORD wServiceID = LOWORD(dwServiceID);
	WORD wServiceModule = HIWORD(dwServiceID);
	WORD wServiceIndex = HIWORD(dwServiceID)-1;	

	//У������
	if (wServiceIndex >= CountArray(m_ServiceAttribute)) return false;

	switch (Command.wSubCmdID)
	{
	case SUB_CS_S_REGISTER_TOKEN:		//ע������
	{
		//��������	
		CMD_CS_S_RegisterToken* pRegisterToken = (CMD_CS_S_RegisterToken*)pData;

		//��ȡ����
		DWORD dwTokenID = *pdwTokenID;

		//�󶨲���
		bool bLinkSuccess = false;
		auto pBindParameter = GetBindParameter(LOWORD(dwTokenID));
		if (pBindParameter == NULL) return true;

		//���Ӽ�¼
		auto pLinkRecord = &pBindParameter->LinkRecord[wServiceIndex];

		//ע��ɹ�
		if (pRegisterToken->lResultCode == REGISTER_RESULT_SUCCESS)
		{
			//ע������
			if (pBindParameter->dwSocketID == dwTokenID && pBindParameter->LinkState[wServiceIndex] == LS_SHAKEHAND)
			{
				bLinkSuccess = true;				
			}
			else
			{
				//ע������
				LogoutToken(wServiceIndex, pBindParameter->wServiceID[wServiceIndex], dwTokenID);

				return true;
			}
		}	
		
		//��������
		if (pRegisterToken->lResultCode == REGISTER_RESULT_LINKFULL)
		{
			//δָ������
			if (pLinkRecord->wReqServiceID == INVALID_SERVICE_ID)
			{
				//�������
				WORD wAssignServiceID = AssignInsideService(wServiceModule, dwTokenID);
				if (wAssignServiceID != INVALID_SERVICE_ID)
				{
					//ע������
					CMD_CS_C_RegisterToken RegisterToken;
					ZeroMemory(&RegisterToken, sizeof(RegisterToken));

					//����ṹ 
					RegisterToken.dwTokenID = dwTokenID;
					RegisterToken.dwUserID = pBindParameter->dwUserID;
					RegisterToken.dwClientIP = pBindParameter->dwClientIP;

					//������Ϣ
					SendDataToService(wServiceIndex, wAssignServiceID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_C_REGISTER_TOKEN, &RegisterToken, sizeof(RegisterToken));

					return true;
				}				
			}
		}		

		//���ӳɹ�
		if (bLinkSuccess == true)
		{
			//����״̬
			pBindParameter->LinkState[wServiceIndex] = LS_LINKING;
			pBindParameter->wServiceID[wServiceIndex] = wServiceID;			

			//����ṹ
			CMD_GG_S_LinkSuccess LinkSuccess;
			ZeroMemory(&LinkSuccess, sizeof(LinkSuccess));

			//������Ϣ
			LinkSuccess.wServiceID = pBindParameter->wServiceID[wServiceIndex];
			LinkSuccess.wServiceModule = wServiceModule;

			//������Ϣ
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_S_LINK_SUCCESS, &LinkSuccess, sizeof(LinkSuccess), NULL, 0);
		}
		else
		{
			//����״̬
			pBindParameter->LinkState[wServiceIndex] = LS_NULL;
			pBindParameter->wServiceID[wServiceIndex] = NONE_SERVICE_ID;			

			//����ṹ
			CMD_GG_S_LinkFailure LinkFailure;
			ZeroMemory(&LinkFailure, sizeof(LinkFailure));

			//������Ϣ
			LinkFailure.wServiceModule = wServiceModule;
			LinkFailure.wServiceID = pBindParameter->wServiceID[wServiceIndex];

			//������Ϣ			
			LinkFailure.lErrorCode = 1;
			StringCchCopy(LinkFailure.szErrorDescribe, CountArray(LinkFailure.szErrorDescribe), CWHService::FormatMessage(MIM_GATE_0002, L"��������æ�������Ժ��ٴγ��ԣ�"));

			//������Ϣ
			WORD wPacketSize = sizeof(LinkFailure) - sizeof(LinkFailure.szErrorDescribe) + CountStringBuffer(LinkFailure.szErrorDescribe);
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_S_LINK_FAILURE, &LinkFailure, wPacketSize, NULL, 0);
		}		

		return true;
	}
	case SUB_CS_S_TOKEN_PARAMETER:		//���Ʋ���
	{
		//��ȡ����
		CMD_CS_S_TokenParameter * pTokenParameter = (CMD_CS_S_TokenParameter*)pData;
		
		//��ȡ����
		DWORD dwTokenID = *pdwTokenID;

		//�󶨲���
		auto pBindParameter = GetBindParameter(LOWORD(dwTokenID));
		if (pBindParameter == NULL) return true;

		//���Ʋ���
		pBindParameter->dwUserID = pTokenParameter->dwUserID;
		StringCchCopy(pBindParameter->szLogonToken, CountArray(pBindParameter->szLogonToken),pTokenParameter->szLogonToken);	

		return true;
	}
	case SUB_CS_S_SHUTDOWN_SOCKET:		//�ر�����
	{
		//����У��
		ASSERT (wDataSize == sizeof(CMD_CS_C_ShutdownSocket));
		if (wDataSize != sizeof(CMD_CS_C_ShutdownSocket)) return true;

		//��ȡ����
		CMD_CS_C_ShutdownSocket* pShutdownSocket = (CMD_CS_C_ShutdownSocket*)pData;

		//��������	
		DWORD dwTokenID = 0;
		tagBindParameter * pBindParameter = NULL;

		//ѭ������
		for (WORD i = 0; i < wTokenCount; i++)
		{
			//��ȡ����
			dwTokenID = *pdwTokenID++;

			//У������
			pBindParameter = GetBindParameter(LOWORD(dwTokenID));
			if (pBindParameter->dwSocketID != dwTokenID) continue;
			if (pBindParameter->LinkState[wServiceIndex] == LS_NULL) continue;

			//ǿ�ƹر�
			if (pShutdownSocket->bForceClose == true)
			{
				//�ر�����
				m_pITCPNetworkEngine->ShutDownSocket(pBindParameter->dwSocketID);
			}
			else
			{
				//ע������
				LogoutToken(wServiceIndex, pBindParameter->wServiceID[wServiceIndex], dwTokenID);

				//�ر�����
				ShutdownServiceLink(m_ServiceAttribute[wServiceIndex].wServiceModule, wServiceID, dwTokenID);
			}			
		}

		return true;
	}		
	}

	return true;
}

//�ڲ�����
bool CAttemperEngineSink::OnTCPSocketMainInsideService(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD *pdwTokenID, WORD wTokenCount)
{
	//������Ϣ
	WORD wServiceIndex = HICMMD(Command.wMainCmdID)-1;
	if (wServiceIndex >= CountArray(m_ServiceAttribute)) return true;
	
	//��������	
	DWORD dwTokenID = 0;	
	tagBindParameter * pBindParameter = NULL;

	//ѭ������
	for (WORD i = 0; i < wTokenCount; i++)
	{
		//��ȡ����
		dwTokenID = *pdwTokenID++;

		//У������
		if (dwTokenID != 0)
		{
			pBindParameter = GetBindParameter(LOWORD(dwTokenID));
			if (pBindParameter->dwSocketID != dwTokenID) continue;
			if (pBindParameter->LinkState[wServiceIndex] == LS_NULL) continue;

			//������Ϣ			
			m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, Command.wMainCmdID, Command.wSubCmdID, pData, wDataSize, NULL, 0);
		}
		else
		{
			//��������
			WORD wBindIndex = 0;

			do
			{
				//��ȡ����
				pBindParameter = GetBindParameter(wBindIndex++);
				if (pBindParameter == NULL) break;
				if (pBindParameter->dwSocketID == 0) continue;
				if (pBindParameter->LinkState[wServiceIndex] != LS_LINKING) continue;

				//������Ϣ			
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID, Command.wMainCmdID, Command.wSubCmdID, pData, wDataSize, NULL, 0);

			} while (true);		
		}
	}

	return true;
}

//��������
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//��������
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->m_UIControlSink.PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//������Ϣ
bool CAttemperEngineSink::SendDataToService(WORD wServiceIndex, WORD wServiceID, DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//ö������
	ITCPSocketService * pITCPSocketService = m_GlobalInfoManager.GetSocketService(wServiceIndex, wServiceID);
	if (pITCPSocketService == NULL)
	{
		pITCPSocketService = m_GlobalInfoManager.GetSocketService(wServiceIndex, 0);
	}

	//���񲻿���
	if (pITCPSocketService == NULL)
	{
		//����ṹ
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage, sizeof(SystemMessage));

		//��������						
		SystemMessage.wType = SMT_EJECT | SMT_CLOSE_LINK;
		_snwprintf_s(SystemMessage.szString,CountArray(SystemMessage.szString), L"%s�����á����Ժ��ٴ����ԣ�",m_ServiceAttribute[wServiceIndex].szServiceName);

		//��ӡ��Ϣ
		CTraceService::TraceString(TEXT("ServiceID=%d,%s"),TraceLevel_Info, wServiceID, SystemMessage.szString);

		//������Ϣ
		WORD wPacketSize = sizeof(SystemMessage) - sizeof(SystemMessage.szString) + CountStringBuffer(SystemMessage.szString);
		m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(MDM_CM_SYSTEM, SERVICE_MODULE_GATE), SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wPacketSize, NULL,0);

		return false;
	}

	//��������
	if (pITCPSocketService != NULL)
	{
		pITCPSocketService->SendData(wMainCmdID, wSubCmdID, pData, wDataSize, dwSocketID);
	}

	return true;
}

//�������
WORD CAttemperEngineSink::AssignInsideService(WORD wServiceModule, DWORD dwSocketID)
{
	//��ȡ����
	auto wBindIndex = LOWORD(dwSocketID);
	auto pBindParameter = GetBindParameter(wBindIndex);
	if (pBindParameter == NULL) return INVALID_SERVICE_ID;

	//��������
	auto wServiceIndex = wServiceModule-1;
	if (wServiceIndex >=CountArray(m_ServiceAttribute)) return INVALID_SERVICE_ID;

	//У�����
	auto pLinkRecord = &pBindParameter->LinkRecord[wServiceIndex];	
	if (pLinkRecord->wTimes>=CountArray(pLinkRecord->wServiceID)) return INVALID_SERVICE_ID;

	//ָ������
	if (pLinkRecord->wReqServiceID != INVALID_SERVICE_ID)
	{
		//���ҷ���
		if (m_GlobalInfoManager.SearchServiceInfo(wServiceIndex, pLinkRecord->wReqServiceID))
		{
			return pLinkRecord->wReqServiceID;
		}

		return INVALID_SERVICE_ID;
	}

	//��������
	WORD wAssignServiceID = INVALID_SERVICE_ID;

	switch (wServiceModule)
	{	
	case SERVICE_MODULE_GAME:		//��Ϸ����
	{
		//��չ��Ϣ
		auto pLinkExtend = (tagLinkExtend_Server*)pLinkRecord->cbLinkExtend;

		//��������
		DWORD dwMaxPlayer = 0;
		DWORD dwLeftCount = INVALID_DWORD;
		BYTE cbPlayerRatio[] = { 80,100 };
		WORD wFindServerType = pLinkExtend->wServerType & (GAME_GENRE_GOLD| GAME_GENRE_SCORE| GAME_GENRE_MATCH| GAME_GENRE_BATTLE| GAME_GENRE_RANKING);

		//���䷿��
		for (int nIndex = 0; nIndex < CountArray(cbPlayerRatio); nIndex++)
		{
			//���ñ�ʶ
			wAssignServiceID = INVALID_WORD;

			//��������			
			CGlobalServiceInfo* pGlobalServerItem = NULL;

			do
			{
				pGlobalServerItem = m_GlobalInfoManager.EnumServiceInfo(wServiceIndex, pGlobalServerItem);
				if (pGlobalServerItem == NULL) break;

				//��������
				auto pGameServer = (tagGameServer *)pGlobalServerItem->GetExtendData();
				WORD wServerType = pGameServer->wServerType& (GAME_GENRE_GOLD | GAME_GENRE_SCORE | GAME_GENRE_MATCH | GAME_GENRE_BATTLE | GAME_GENRE_RANKING);

				//�����ж�
				if (pGameServer->wKindID == pLinkExtend->wKindID && pGameServer->wServerLevel == pLinkExtend->wServerLevel 
					&& (pGameServer->dwMasterRule&MR_FORBID_ENTER_ROOM)==0 && (wFindServerType & wServerType) != 0)
				{
					//�������
					bool bServiceExits = false;
					for (auto j = 0; j < pLinkRecord->wTimes; j++)
					{
						if (pLinkRecord->wServiceID[j] == pGameServer->wServerID)
						{
							bServiceExits = true;
							break;
						}
					}

					//δ������
					if (bServiceExits == false)
					{
						//��������
						dwMaxPlayer = (WORD)(pGameServer->dwMaxPlayer / 100.f * cbPlayerRatio[nIndex]);

						//�Ƚ�����
						if ((dwMaxPlayer > pGameServer->dwOnLineCount) && (dwMaxPlayer - pGameServer->dwOnLineCount < dwLeftCount))
						{
							wAssignServiceID = pGameServer->wServerID;
							dwLeftCount = dwMaxPlayer - pGameServer->dwOnLineCount;
						}						
					}
				}

			} while (pGlobalServerItem!=NULL);

			//��������
			dwLeftCount = INVALID_DWORD;

			//�˳��ж�
			if (wAssignServiceID != INVALID_WORD) break;
		}

		break;
	}
	default:
	{
		//��������
		INT nServiceCount = m_GlobalInfoManager.GetServiceCount(wServiceIndex);		
		if (nServiceCount == 0) break;

		//��������
		auto pServiceAttrib = &m_ServiceAttribute[wServiceIndex];

		//״̬У��
		if (pServiceAttrib->bIsStateful==false)
		{
			wAssignServiceID = 0;
		}
		else
		{
			//�����ʶ
			DWORD dwObjectID = dwSocketID;

			//��ϣӳ��
			if (pServiceAttrib->wServiceMapKind == MAP_KIND_HASH)
			{
				if (pBindParameter->dwUserID != 0) dwObjectID = pBindParameter->dwUserID;

				//ӳ�����
				auto  pGlobalServiceItem = m_GlobalInfoManager.MapServiceInfo(wServiceIndex, 0, dwObjectID);
				if (pGlobalServiceItem != NULL)
				{
					wAssignServiceID = pGlobalServiceItem->GetServiceID();
				}
			}

			//·��ӳ��
			if (pServiceAttrib->wServiceMapKind == MAP_KIND_ROUTE)
			{
				auto pLinkExtend=(tagLinkExtend_Route*)pBindParameter->LinkRecord[wServiceIndex].cbLinkExtend;

				//ӳ�����
				auto  pGlobalServiceItem = m_GlobalInfoManager.MapServiceInfo(wServiceIndex, 0, pLinkExtend->dwRouteKeyID);
				if (pGlobalServiceItem != NULL)
				{
					wAssignServiceID = pGlobalServiceItem->GetServiceID();
				}
				else
				{
					//����ṹ
					CMD_CS_C_QueryRouteItem QueryRouteItem = {};

					//���ñ���
					QueryRouteItem.dwTokenID = dwSocketID;
					QueryRouteItem.dwRouteID = pLinkExtend->dwRouteKeyID;					
					QueryRouteItem.wServiceModule = pServiceAttrib->wServiceModule;

					//������Ϣ
					m_TCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE,SUB_CS_C_QUERY_ROUTE_ITEM,&QueryRouteItem,sizeof(QueryRouteItem),dwSocketID);
				}
			}							
		}	

		break;
	}
	}
	
	//���Ӽ�¼
	if (wAssignServiceID != INVALID_SERVICE_ID)
	{
		pLinkRecord->wServiceID[pLinkRecord->wTimes++] = wAssignServiceID;
	}

	return wAssignServiceID;
}

//ע������
VOID CAttemperEngineSink::LogoutToken(WORD wServiceIndex, WORD wServiceID, DWORD dwTokenID)
{
	if (wServiceID != 0)
	{
		//����ṹ
		CMD_CS_C_LogoutToken LogoutToken;
		ZeroMemory(&LogoutToken, sizeof(LogoutToken));

		//���ñ���
		LogoutToken.dwTokenID = dwTokenID;

		//������Ϣ
		SendDataToService(wServiceIndex, wServiceID, dwTokenID, MDM_CS_COMMON_SERVICE, SUB_CS_C_LOGOUT_TOKEN, &LogoutToken, sizeof(LogoutToken));
	}	
}

//�ر�����
void CAttemperEngineSink::ShutdownServiceLink(WORD wServiceModule, WORD wServiceID)
{
	//��ȡ����
	tagBindParameter * pBindParameter = NULL;
	WORD wServiceIndex = wServiceModule-1;

	//��������
	auto nServiceCount = m_GlobalInfoManager.GetServiceCount(wServiceModule);

	//ö������
	for (int i = 0; i < m_pServiceOption->wMaxConnect; i++)
	{
		pBindParameter = m_pBindParameter + i;
		if (pBindParameter->LinkState[wServiceIndex] == LS_NULL) continue;
		if (pBindParameter->wServiceID[wServiceIndex] != wServiceID && nServiceCount!=0) continue;

		//�Ͽ�����
		ShutdownServiceLink(wServiceModule, pBindParameter->wServiceID[wServiceIndex], pBindParameter->dwSocketID);
	}
}

//�ر�����
void CAttemperEngineSink::ShutdownServiceLink(WORD wServiceModule, WORD wServiceID, DWORD dwSoceketID)
{
	//��������		
	CMD_GG_S_LinkShutdown LinkShutdown;
	ZeroMemory(&LinkShutdown, sizeof(LinkShutdown));

	//����ṹ
	LinkShutdown.wServiceID = wServiceID;
	LinkShutdown.wServiceModule = wServiceModule;

	//���ӹر�
	m_pITCPNetworkEngine->SendData(dwSoceketID, MAKECMMD(MDM_GG_GATE_SERVICE, SERVICE_MODULE_GATE), SUB_GG_S_LINK_SHUTDOWN, &LinkShutdown, sizeof(LinkShutdown), NULL, 0);

	//��ȡ����
	WORD wBindIndex = LOWORD(dwSoceketID);
	tagBindParameter * pBindParameter = GetBindParameter(wBindIndex);

	//��ȡ����
	WORD wServiceIndex = wServiceModule-1;

	//����״̬
	pBindParameter->LinkState[wServiceIndex] = LS_NULL;
	pBindParameter->wServiceID[wServiceIndex] = NONE_SERVICE_ID;	

	//ö������
	for (int i = 0; i < CountArray(pBindParameter->LinkState); i++)
	{
		if (pBindParameter->LinkState[i] != LS_NULL) return;
	}

	//�ر�����
	m_pITCPNetworkEngine->ShutDownSocket(dwSoceketID);
}

//ʱ�ӱ�ʶ
DWORD CAttemperEngineSink::GetTimerID(WORD wServiceIndex, WORD wServiceID)
{
	//У������
	ASSERT (wServiceIndex < CountArray(m_ServiceAttribute));
	if (wServiceIndex >= CountArray(m_ServiceAttribute)) return 0;

	return MAKELONG(wServiceID, m_ServiceAttribute[wServiceIndex].wServiceModule);
}

//�����ʶ
DWORD CAttemperEngineSink::GetServiceID(WORD wServiceIndex, WORD wServiceID)
{
	//У������
	ASSERT(wServiceIndex < CountArray(m_ServiceAttribute));
	if (wServiceIndex >= CountArray(m_ServiceAttribute)) return 0;

	return MAKELONG(wServiceID, m_ServiceAttribute[wServiceIndex].wServiceModule);
}

//��������
WORD CAttemperEngineSink::GetOnlineCount()
{
	//��������
	WORD wBindIndex=0;
	WORD wOnlineCount=0;
	tagBindParameter * pBindParameter = NULL;

	do
	{
		//��ȡ����
		pBindParameter=GetBindParameter(wBindIndex++);
		if (pBindParameter == NULL) break;
		if (pBindParameter->dwSocketID == 0) continue;

		//ͳ������
		wOnlineCount++;
	} while (true);

	return wOnlineCount;
}

//�󶨲���
tagBindParameter * CAttemperEngineSink::GetBindParameter(WORD wBindIndex)
{
	//��Ч����
	if (wBindIndex==INVALID_WORD) return NULL;

	//��������
	if (wBindIndex<m_pServiceOption->wMaxConnect)
	{
		return m_pBindParameter+wBindIndex;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

