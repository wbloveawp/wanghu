#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_CONNECT_VALIDATE		1									//������ʶ
#define IDI_CONNECT_CENTER		2									//������ʶ

//////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����		
	m_bLoggerService=false;

	//���ñ���
	m_pServiceOption=NULL;
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;	
	ZeroMemory(&m_LoggerItem,sizeof(m_LoggerItem));

	//�������
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;

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
	m_bLoggerService=true;

	//����·��
	m_LoggerManager.SetLogPath(m_pInitParameter->m_szLoggerPath);

	//��������
	m_LoggerManager.StartService();	

	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//״̬����		
	m_bLoggerService=false;

	//���ñ���
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;

	//ɾ������
	SafeDeleteArray(m_pBindParameter);

	//ֹͣ����
	m_LoggerManager.ConcludeService();

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	switch (dwIdentifier)
	{
	case CT_LOAD_LOGGER_OPTION:		//��������
		{
			//��������			
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_LOGGER_OPTION,0,0,0,0,0);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ڼ�����־����..."),TraceLevel_Normal);

			return true;
		}
	case CT_CONNECT_CENTER:			//��������
		{
			//���õ�ַ
			DWORD dwCenterServer=m_pInitParameter->m_CenterAddress.dwServiceAddr;

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
	case IDI_CONNECT_CENTER:	//��������
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

	//ע�����
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
	case MDM_LS_LOGGER_SERVICE:		//��־����
		{
			return OnTCPNetworkMainLoggerService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
		}
	}

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

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
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
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}

		//��������
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService,sizeof(RegisterService));

		//����ṹ
		RegisterService.wServiceID= m_pServiceOption->wServiceID;
		RegisterService.wServiceModule = SERVICE_MODULE_LOGGER;
		RegisterService.wServicePort= m_pServiceOption->wServicePort;	
		RegisterService.wServiceKindID = m_pServiceOption->wServiceKindID;
		StringCchCopy(RegisterService.szServiceName, CountArray(RegisterService.szServiceName), TEXT("��־������"));
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
	case SUB_CS_S_SERVICE_INSERT:	//�������
		{
			//����У��
			ASSERT (wDataSize%sizeof(tagServiceItem)==0);
			if (wDataSize%sizeof(tagServiceItem)!=0) return false;

			//��ȡ����
			tagServiceItem * pServiceItem =(tagServiceItem *)pData;

			return true;
		}
	case SUB_CS_S_SERVICE_REMOVE:	//�����Ƴ�
		{
			//����У��
			ASSERT (wDataSize==sizeof(CMD_CS_S_ServiceRemove));
			if (wDataSize!=sizeof(CMD_CS_S_ServiceRemove)) return false;

			//��ȡ����
			CMD_CS_S_ServiceRemove * pServiceRemove =(CMD_CS_S_ServiceRemove *)pData;

			return true;
		}
	}

	return true;
}

//�������
bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//ע�����
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	return false;
}

//����״̬
bool CAttemperEngineSink::OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	return false;
}

//����״̬
bool CAttemperEngineSink::OnTCPNetworkMainLoggerService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID)
{
	//д����־
	if (wSubCmdID == SUB_LC_WRITE_LOGGER)
	{
		//У������
		ASSERT(wDataSize >= sizeof(CMD_LC_WriteTimeLogger));
		if (wDataSize < sizeof(CMD_LC_WriteTimeLogger)) return true;

		//��ȡ����
		auto pWriteTimeLogger = (CMD_LC_WriteTimeLogger*)pData;

		//������־
		CDayLogger& DayLogger = m_LoggerManager.GetDayLogger(*(tagTimeLogInfo*)pWriteTimeLogger);

		//��������
		WORD wPacketSize = 0;
		LPBYTE pcbString = (LPBYTE)(pWriteTimeLogger + 1);

		//��־��Ϣ	
		while (wPacketSize < wDataSize - sizeof(CMD_LC_WriteTimeLogger))
		{
			//��ȡ��С
			WORD wStringSize = *(WORD*)pcbString;
			pcbString += sizeof(WORD);
			wPacketSize += sizeof(WORD);

			//�����־
			DayLogger.LogAny((LPTSTR)pcbString);

			//���ñ���
			pcbString += wStringSize;
			wPacketSize += wStringSize;
		}

		return true;
	}

	return false;
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
