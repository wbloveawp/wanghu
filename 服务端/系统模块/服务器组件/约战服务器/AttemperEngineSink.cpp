#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_CONNECT_CENTER			1									//������ʶ
#define IDI_CONNECT_VALIDATE		2									//������ʶ
#define IDI_LOAD_BATTLE_OPTION		3									//Լս����

//ʱ������
#define TIME_LOAD_BATTLE_OPTION		60									//��������

//��������
#define MAX_TABLE_COUNT_REQUEST		64									//�������		

//////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����		
	m_bBattleService=false;

	//���ñ���
	m_pServiceOption=NULL;
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;	

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
	m_bBattleService=true;
	m_SynchroItemArray.RemoveAll();

	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//״̬����		
	m_bBattleService=false;
	m_SynchroItemArray.RemoveAll();

	//���ñ���
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;

	//ɾ������
	SafeDeleteArray(m_pBindParameter);

	//�������
	m_BattleManager.ResetData();
	m_GlobalInfoManager.ResetData();	

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	switch (dwIdentifier)
	{
	case CT_LOAD_BATTLE_OPTION:		//��������
	{
		//��������			
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_BATTLE_OPTION,0,0,0,0,0);

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڼ���Լս����..."),TraceLevel_Normal);

		return true;
	}
	case CT_CONNECT_CENTER:		//��������
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
	case IDI_CONNECT_CENTER:		//��������
		{
			//��������
			auto pAddressInfo=&m_pInitParameter->m_CenterAddress;

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
	//��������
	bool bResult = true;
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
	case MDM_GB_BATTLE_SERVICE:		//Լս����
	{
		bResult = OnTCPNetWorkMainGBBattleService(Command.wSubCmdID,pData,wDataSize,dwSocketID,dwTokenID);
		break;
	}
	case MDM_BS_BATTLE_SERVICE:		//Լս����
	{
		bResult = OnTCPNetWorkMainBSBattleService(Command.wSubCmdID,pData,wDataSize,dwSocketID,dwTokenID);
		break;
	}
	case MDM_BC_BATTLE_SERVICE:		//Լս���� 
	{
		bResult = OnTCPNetWorkMainBCBattleService(Command.wSubCmdID, pData, wDataSize, dwSocketID, dwTokenID);
		break;
	}
	}

	//�Ͽ�����
	if (bResult == false)
	{
		//����ṹ
		CMD_CS_C_ShutdownSocket ShutdownSocket;
		ShutdownSocket.bForceClose = false;

		//��ȡ��Ϣ
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//��������
		SendDataToGate(pBindParameter->wServiceID,MDM_CS_COMMON_SERVICE,SUB_CS_S_SHUTDOWN_SOCKET,&ShutdownSocket,sizeof(ShutdownSocket),&dwTokenID,1);
	}

	return true;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	auto pBindParameter=(m_pBindParameter+wBindIndex);

	//��Ϸ����
	if (pBindParameter->wServiceModule==SERVICE_MODULE_GAME)
	{
		//��������
		auto pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(pBindParameter->wServiceID);
		if (pGlobalServerItem!=NULL)
		{
			//��ȡ����
			tagGameServer * pGameServer=&pGlobalServerItem->m_GameServer;

			//�Ƴ�����
			m_BattleManager.RemoveServer(pGameServer->wKindID,pGameServer->wServerID);

			//ע������
			m_GlobalInfoManager.DeleteServerItem(pBindParameter->wServiceID);					
		}
	}

	//���ط���
	if (pBindParameter->wServiceModule == SERVICE_MODULE_GATE)
	{
		//��������
		auto pGlobalGateItem=m_GlobalInfoManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem!=NULL)
		{
			//ע������
			m_GlobalInfoManager.DeleteGateItem(pBindParameter->wServiceID);					
		}
	}

	//�������
	if (pBindParameter->wServiceModule == SERVICE_MODULE_CLUB)
	{
		//��������
		auto pGlobalClubItem = m_GlobalInfoManager.SearchClubItem(pBindParameter->wServiceID);
		if (pGlobalClubItem != NULL)
		{
			//ע������
			m_GlobalInfoManager.DeleteClubItem(pBindParameter->wServiceID);
		}
	}
	
	//�����Ϣ
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GR_OPERATE_FAILURE:			//����ʧ��
	{
		return OnDBOperateFailure(dwContextID, dwTokenID, pData, wDataSize);
	}	
	case DBO_GR_BATTLE_GAME_OPTION:			//��Ϸѡ��
	{
		return OnDBBattleGameOption(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_BATTLE_BASE_OPTION:			//����ѡ��
	{
		return OnDBBattleBaseOption(dwContextID, dwTokenID, pData,wDataSize);
	}	
	case DBO_GR_BATTLE_GLOBAL_OPTION:		//ȫ������
	{
		return OnDBBattleGlobalOption(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_BATTLE_OPTION_FINISH:		//ѡ�����	
	{
		return OnDBBattleOptionFinish(dwContextID, dwTokenID, pData,wDataSize);
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
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}

		//��������
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService,sizeof(RegisterService));

		//����ṹ
		RegisterService.wServiceID= m_pServiceOption->wServiceID;
		RegisterService.wServiceModule = SERVICE_MODULE_BATTLE;
		RegisterService.wServicePort= m_pServiceOption->wServicePort;
		RegisterService.wServiceKindID = m_pServiceOption->wServiceKindID;
		StringCchCopy(RegisterService.szServiceName, CountArray(RegisterService.szServiceName), TEXT("Լս������"));
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
			return OnTCPSocketMainRegister(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_CS_COMMON_SERVICE:		//������Ϣ
		{
			return OnTCPSocketMainCommonService(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_CS_MANAGER_SERVICE:	//�������
		{
			return OnTCPSocketMainManagerService(Command.wSubCmdID, pData, wDataSize);
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
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_SUCCESS:		//ע��ɹ�
	{
		//�¼�֪ͨ
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess = ER_SUCCESS;
		SendUIControlPacket(UI_CENTER_RESULT, &ControlResult, sizeof(ControlResult));

		return true;
	}
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
	{
		//Ч�����
		auto pRegisterFailure = (CMD_CS_S_RegisterFailure*)pData;
		ASSERT(wDataSize >= (sizeof(CMD_CS_S_RegisterFailure) - sizeof(pRegisterFailure->szDescribeString)));
		if (wDataSize < (sizeof(CMD_CS_S_RegisterFailure) - sizeof(pRegisterFailure->szDescribeString))) return false;

		//�رմ���
		m_pITCPSocketCenter->CloseSocket();

		//��ʾ��Ϣ
		CW2CT strDescribeString(pRegisterFailure->szDescribeString);
		if (lstrlen(strDescribeString) > 0) CTraceService::TraceString(strDescribeString, TraceLevel_Exception);

		//�¼�֪ͨ
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess = ER_FAILURE;
		SendUIControlPacket(UI_CENTER_RESULT, &ControlResult, sizeof(ControlResult));

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
			auto pServiceItem =(tagServiceItem *)pData;

			return true;
		}
	case SUB_CS_S_SERVICE_REMOVE:	//�����Ƴ�
		{
			//����У��
			ASSERT (wDataSize==sizeof(CMD_CS_S_ServiceRemove));
			if (wDataSize!=sizeof(CMD_CS_S_ServiceRemove)) return false;

			//��ȡ����
			auto pServiceRemove =(CMD_CS_S_ServiceRemove *)pData;


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

			//����״̬
			auto bLastBattleService = m_bBattleService;

			//��������
			if (pCommandCode->dwCommandCode & CMD_LOAD_CONFIG_DATA)
			{
				//Լս����
				if (pCommandCode->dwCommandParam & CONFIG_MASK_BATTLE_OPTION)
				{
					//��������
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_BATTLE_OPTION, 0, 0, 0, 0, 0);
				}
			}

			//ֹͣ����
			if (pCommandCode->dwCommandCode&CMD_STOP_BATTLE_SERVICE)
			{
				//���ñ�ʶ
				m_bBattleService=false;
			}

			//��������
			if (pCommandCode->dwCommandCode&CMD_START_BATTLE_SERVICE)
			{
				//���ñ�ʶ
				m_bBattleService=true;
			}			

			//״̬���
			if (bLastBattleService != m_bBattleService)
			{
				//����ѡ��
				CMD_BS_C_BattleOption BattleOption;
				BattleOption.bBattleService = m_bBattleService;

				//Լսѡ��
				m_pITCPNetworkEngine->SendDataBatch(MDM_BS_BATTLE_SERVICE, SUB_BS_C_BATTLE_OPTION, &BattleOption, sizeof(BattleOption), SERVICE_MASK_GAME);
			}

			return true;
		}
	}

	return true;
}

//ת����Ϣ
bool CAttemperEngineSink::OnTCPSocketMainForwardService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//ע�����
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{	
	case SUB_CS_C_REGISTER_SERVER:	//ע������
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterServer));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterServer)) return false;

			//��Ϣ����
			CMD_CS_C_RegisterServer * pRegisterServer=(CMD_CS_C_RegisterServer *)pData;

			//��������
			if (m_GlobalInfoManager.SearchServerItem(pRegisterServer->wServerID)!=NULL)
			{
				//��������
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//���ñ���
				RegisterFailure.lErrorCode=0;
				StringCchCopy(RegisterFailure.szDescribeString,CountArray(RegisterFailure.szDescribeString), TEXT("�Ѿ�������ͬ��ʶ����Ϸ���ӷ������ӷ���ע��ʧ��"));

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
			(m_pBindParameter+wBindIndex)->wServiceModule=SERVICE_MODULE_GAME;
			(m_pBindParameter+wBindIndex)->wServiceID=pRegisterServer->wServerID;

			//��������
			tagGameServer GameServer;
			ZeroMemory(&GameServer,sizeof(GameServer));

			//��������
			GameServer.wKindID=pRegisterServer->wKindID;
			GameServer.wVisibleMask=pRegisterServer->wVisibleMask;
			GameServer.wServerID=pRegisterServer->wServerID;
			GameServer.wServerPort=pRegisterServer->wServerPort;
			GameServer.wServerType=pRegisterServer->wServerType;			
			GameServer.dwServerAddr=pRegisterServer->dwServerAddr;
			StringCchCopy(GameServer.szServerDomain,CountArray(GameServer.szServerDomain), pRegisterServer->szServerDomain);

			//ע������
			m_GlobalInfoManager.ActiveServerItem(dwSocketID,GameServer);

			//Ⱥ������
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true, SERVICE_MASK_GAME);

			//����ѡ��
			CMD_BS_C_BattleOption BattleOption;
			BattleOption.bBattleService = m_bBattleService;

			//Լսѡ��
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_BATTLE_OPTION, &BattleOption, sizeof(BattleOption), NULL, 0);

			//����ṹ
			tagServerItem ServerItem;
			ZeroMemory(&ServerItem,sizeof(ServerItem));

			//���ñ���
			ServerItem.wKindID=pRegisterServer->wKindID;
			ServerItem.wServerID=pRegisterServer->wServerID;
			ServerItem.wServerType=pRegisterServer->wServerType;
			ServerItem.wTableCount=pRegisterServer->wTableCount;			

			//��ӷ���
			if (m_BattleManager.AppendServer(ServerItem)==false)
			{
				//��������
				WORD wPacketSize=0;
				BYTE cbBuffer[SOCKET_BUFFER];

				//��������
				CTableItemArray TableItemArray;
				tagBattleTableItemEx * pTableItemEx=NULL;							

				do
				{
					//ö������
					pTableItemEx=m_BattleManager.EnumTableItem(pTableItemEx);
					if (pTableItemEx==NULL) break;

					//�ж�����
					if (pTableItemEx->wServerID==pRegisterServer->wServerID)
					{
						//�ж�����
						if (pTableItemEx->wTableID==INVALID_TABLE)
						{
							TableItemArray.Add(pTableItemEx);
							continue;
						}

						//�����ж�
						if (wPacketSize+sizeof(tagBattleTableItemEx)>sizeof(cbBuffer))
						{
							//��������
							m_pITCPNetworkEngine->SendData(dwSocketID,MDM_BS_BATTLE_SERVICE,SUB_BS_C_SYNCHRO_TABLE,cbBuffer,wPacketSize,NULL,0);
							wPacketSize=0;
						}

						//��ȡ����
						tagBattleTableItemEx * pTableItemMove=(tagBattleTableItemEx *)(cbBuffer+wPacketSize);
						CopyMemory(pTableItemMove,pTableItemEx,sizeof(tagBattleTableItemEx));

						//λ��ƫ��
						wPacketSize+=sizeof(tagBattleTableItemEx);
					}

				} while (true);

				//��������
				if (wPacketSize>0)
				{
					m_pITCPNetworkEngine->SendData(dwSocketID,MDM_BS_BATTLE_SERVICE, SUB_BS_C_SYNCHRO_TABLE,cbBuffer,wPacketSize,NULL,0);
				}

				//�Ƴ�����
				for (INT_PTR i=0; i<TableItemArray.GetCount(); i++)
				{
					pTableItemEx=TableItemArray[i];
					m_BattleManager.RemoveTable(pTableItemEx->dwBattleNum);
				}
			}
			else
			{
				//�������
				AppendSynchroItem(pRegisterServer->wServerID);

				//��������
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_BS_BATTLE_SERVICE, SUB_BS_C_SYNCHRO_TABLE,NULL,0);
			}

			//ע��ɹ�
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_SUCCESS, NULL, 0);

			return true;
		}	
	case SUB_CS_C_REGISTER_SERVICE:	//ע�����
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterService));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterService)) return false;

			//��Ϣ����
			auto pRegisterService =(CMD_CS_C_RegisterService *)pData;

			//У�����
			if (pRegisterService->wServiceModule != SERVICE_MODULE_GATE && pRegisterService->wServiceModule != SERVICE_MODULE_CLUB)
			{
				return false;
			}

			//���ҷ���
			if ((pRegisterService->wServiceModule == SERVICE_MODULE_GATE && m_GlobalInfoManager.SearchGateItem(pRegisterService->wServiceID) != NULL) || 
				(pRegisterService->wServiceModule == SERVICE_MODULE_CLUB && m_GlobalInfoManager.SearchClubItem(pRegisterService->wServiceID) != NULL))
			{
				//��������
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//���ñ���
				RegisterFailure.lErrorCode=0;
				_sntprintf_s(RegisterFailure.szDescribeString,CountArray(RegisterFailure.szDescribeString), TEXT("�Ѿ�������ͬ��ʶ��%s������ע��ʧ��"), pRegisterService->szServiceName);

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
			(m_pBindParameter + wBindIndex)->wServiceID = pRegisterService->wServiceID;
			(m_pBindParameter+wBindIndex)->wServiceModule= pRegisterService->wServiceModule;			

			//��������
			tagServiceItem ServiceItem;
			ZeroMemory(&ServiceItem,sizeof(ServiceItem));

			//��������
			ServiceItem.wServiceID= pRegisterService->wServiceID;
			ServiceItem.wServicePort= pRegisterService->wServicePort;
			ServiceItem.wServiceModule = pRegisterService->wServiceModule;
			ServiceItem.wMaxConnect= pRegisterService->wMaxConnect;
			ServiceItem.dwServiceAddr= pRegisterService->dwServiceAddr;
			StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), pRegisterService->szServiceName);
			StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain), pRegisterService->szServiceDomain);

			//ע������
			if (ServiceItem.wServiceModule == SERVICE_MODULE_GATE)
			{
				m_GlobalInfoManager.ActiveGateItem(dwSocketID, ServiceItem);
			}			

			//ע����
			if (ServiceItem.wServiceModule == SERVICE_MODULE_CLUB)
			{
				m_GlobalInfoManager.ActiveClubItem(dwSocketID, ServiceItem);
			}

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
	case SUB_CS_C_SERVER_MODIFY:	//�����޸�
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_ServerModify));
			if (wDataSize!=sizeof(CMD_CS_C_ServerModify)) return false;

			//��Ϣ����
			auto pServerModify=(CMD_CS_C_ServerModify *)pData;

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			auto pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			ASSERT(pBindParameter->wServiceModule==SERVICE_MODULE_GAME);
			if (pBindParameter->wServiceModule!= SERVICE_MODULE_GAME) return false;

			//��������
			WORD wServerID=pBindParameter->wServiceID;
			auto pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(wServerID);

			//�����޸�
			if (pGlobalServerItem!=NULL)
			{
				//���ñ���
				pGlobalServerItem->m_GameServer.wKindID=pServerModify->wKindID;
				pGlobalServerItem->m_GameServer.wServerPort=pServerModify->wServerPort;				
				pGlobalServerItem->m_GameServer.dwMaxPlayer=pServerModify->dwMaxPlayer;
				pGlobalServerItem->m_GameServer.dwServerAddr=pServerModify->dwServerAddr;
				StringCchCopy(pGlobalServerItem->m_GameServer.szServerDomain,CountArray(pGlobalServerItem->m_GameServer.szServerDomain), pServerModify->szServerDomain);
			}

			return true;
		}
	case SUB_CS_C_LOGOUT_TOKEN:
		{
			//���ð�
			WORD wBindIndex = LOWORD(dwSocketID);
			auto pBindParameter = (m_pBindParameter + wBindIndex);

			//У�����
			if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return true;

			//��ȡ����
			auto pLogoutToken = (CMD_CS_C_LogoutToken *)pData;

			//��������
			auto pGlobalGateItem = m_GlobalInfoManager.SearchGateItem(pBindParameter->wServiceID);
			if (pGlobalGateItem != NULL)
			{
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
			auto pRegisterToken = (CMD_CS_C_RegisterToken *)pData;

			//����ṹ
			CMD_CS_S_RegisterToken RegisterToken;
			ZeroMemory(&RegisterToken, sizeof(RegisterToken));

			//Ĭ�ϳɹ�
			RegisterToken.lResultCode = REGISTER_RESULT_FAILURE;			

			//��������
			auto pGlobalGateItem = m_GlobalInfoManager.SearchGateItem(pBindParameter->wServiceID);
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

	return false;
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
		auto pGlobalGateItem = m_GlobalInfoManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem == NULL) return NULL;

		//��ѯ�û�
		auto pTokenParameter = pGlobalGateItem->SearchTokenID(dwTokenID);
		if (pTokenParameter == NULL || pTokenParameter->dwTokenID != dwTokenID) return NULL;

		return pTokenParameter;
	}

	return NULL;
}

//��������
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//��������
	auto pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->m_UIControlSink.PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//��������
VOID CAttemperEngineSink::ConstructTableItem(tagBattleTableItemEx * pTableItemEx, tagBattleTableItem * pTableItem)
{
	//��������
	pTableItem->dwClubID = pTableItemEx->dwClubID;
	pTableItem->wTableID = pTableItemEx->wTableID;
	pTableItem->wServerID = pTableItemEx->wServerID;
	pTableItem->dwOwnerID = pTableItemEx->dwOwnerID;
	pTableItem->dwBattleNum = pTableItemEx->dwBattleNum;
	pTableItem->dwCreateTime = pTableItemEx->dwCreateTime;
	pTableItem->wKindID = pTableItemEx->BattleConfig.wKindID;	
	pTableItem->bLockState = pTableItemEx->szPassword[0]!=0;
	pTableItem->lWinnerScore = pTableItemEx->BattleConfig.lWinnerScore;

	//������Ϣ
	pTableItem->dwConfigID = pTableItemEx->dwConfigID;
	CopyMemory(&pTableItem->BattleConfig, &pTableItemEx->BattleConfig, sizeof(pTableItem->BattleConfig));	
}

//��������
WORD CAttemperEngineSink::ConstructTableList(CTableItemArray& TableItemArray, CTableParamArray& TableParamArray, INT &nStartIndex, BYTE cbBuffer[], WORD wBufferSize)
{
	//��������
	WORD wPacketSize = 0;
	tagBattleTableItem* pTableItem = NULL;
	tagBattleTableParam* pTableParam = NULL;
	tagBattleTableItemEx* pTableItemEx = NULL;
	tagBattleTableParamEx* pTableParamEx = NULL;

	//�����б�
	for (INT_PTR i = nStartIndex; i < TableItemArray.GetCount(); i++)
	{
		//���ӹ���
		if (TableItemArray[i]->wTableID == INVALID_TABLE) continue;

		//�����ж�
		if (wPacketSize + sizeof(tagBattleTableItem) + sizeof(tagBattleTableParam) + TABLE_USER_COUNT * sizeof(tagBattleTableUser) > wBufferSize)
		{
			nStartIndex = i;
			return wPacketSize;
		}

		//��ȡ����
		pTableItemEx = TableItemArray[i];
		pTableParamEx = TableParamArray[i];

		//��������
		pTableItem = (tagBattleTableItem*)&cbBuffer[wPacketSize];

		//��������
		ConstructTableItem(pTableItemEx, pTableItem);

		//��������			
		wPacketSize += sizeof(tagBattleTableItem);

		//���Ӳ���
		pTableParam = (tagBattleTableParam*)&cbBuffer[wPacketSize];
		ZeroMemory(pTableParam,sizeof(tagBattleTableParam));

		//���ò���			
		pTableParam->dwBattleNum = pTableItemEx->dwBattleNum;
		pTableParam->dwStartTime = pTableParamEx->dwStartTime;
		pTableParam->wFinishCount = pTableParamEx->wFinishCount;
		pTableParam->cbGameStatus = pTableParamEx->cbGameStatus;

		//�����û�
		for (int i = 0; i < CountArray(pTableParamEx->TableUserItem); i++)
		{
			if (pTableParamEx->TableUserItem[i].dwUserID == 0) continue;
			CopyMemory(&pTableParam->TableUserItem[pTableParam->wUserCount++], &pTableParamEx->TableUserItem[i], sizeof(tagBattleTableUser));

			//�˳��ж�										
			if (pTableParam->wUserCount == pTableParamEx->wUserCount) break;
		}

		//���ô�С			
		wPacketSize += sizeof(tagBattleTableParam) + pTableParam->wUserCount * sizeof(tagBattleTableUser);
	}

	//��������
	nStartIndex = (INT)TableItemArray.GetCount();

	return wPacketSize;
}

//У���淨
bool CAttemperEngineSink::VerifyBattleConfig(tagBattleConfig * pBattleConfig)
{
	//��������
	tagBattleBaseOptionEx* pBattleOptionEx = NULL;
	for (INT_PTR i = 0; i < m_BattleBaseOptionActive.GetCount(); i++)
	{
		if (m_BattleBaseOptionActive[i].BattleOption.wKindID == pBattleConfig->wKindID &&
			m_BattleBaseOptionActive[i].BattleOption.cbScoreKind == pBattleConfig->cbScoreKind &&
			m_BattleBaseOptionActive[i].BattleOption.wPlayerCount == pBattleConfig->wPlayerCount &&
			m_BattleBaseOptionActive[i].BattleOption.cbEmployScene == pBattleConfig->cbEmployScene)
		{
			pBattleOptionEx = &m_BattleBaseOptionActive[i];
			break;
		}
	}

	//���ò�����
	if (pBattleOptionEx == NULL)
	{
		CTraceService::TraceString(TEXT("�Ҳ���Լս����"), TraceLevel_Exception);

		return false;
	}

	//��������		    			
	WORD wPayType = pBattleConfig->wPayType;
	BYTE cbScoreKind = pBattleConfig->cbScoreKind;
	BYTE cbSettleKind = pBattleConfig->cbSettleKind;
	BYTE cbCurrencyKind = pBattleConfig->cbCurrencyKind;
	tagBattleBaseOption* pBattleOption = &pBattleOptionEx->BattleOption;

	//֧����ʽ
	if ((wPayType & pBattleOption->wSuportPayType) == 0)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE-1"), TraceLevel_Exception);

		return false;
	}

	//����������
	if (cbSettleKind == SETTLE_KIND_COUNT && (pBattleOption->wOptionMask & OPTION_MASK_COUNT) == 0)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE-2"), TraceLevel_Exception);

		return false;
	}

	//�Զ������
	if (cbSettleKind == SETTLE_KIND_CUSTOM && (pBattleOption->wOptionMask & OPTION_MASK_CUSTOM) == 0)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE-3"), TraceLevel_Exception);

		return false;
	}

	//��������
	if (cbCurrencyKind != CURRENCY_KIND_GOLD && cbCurrencyKind != CURRENCY_KIND_CARD)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-4"), TraceLevel_Exception);

		ASSERT(FALSE);
		return false;
	}

	//�������
	if ((pBattleOption->wOptionMask & OPTION_MASK_GOLD) == 0 && cbCurrencyKind == CURRENCY_KIND_GOLD)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-5"), TraceLevel_Exception);
		return false;
	}

	//��������
	if ((pBattleOption->wOptionMask & OPTION_MASK_CARD) == 0 && cbCurrencyKind == CURRENCY_KIND_CARD)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-6"), TraceLevel_Exception);
		return false;
	}

	//����У��
	if ((pBattleOption->wOptionMask & OPTION_MASK_VOICE) == 0 && pBattleConfig->cbAllowVoice == TRUE)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-7"), TraceLevel_Exception);
		return false;
	}

	//����У��
	if ((pBattleOption->wOptionMask & OPTION_MASK_PUBLIC) == 0 && pBattleConfig->cbAllowPublic == TRUE)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-8"), TraceLevel_Exception);
		return false;
	}

	//ͬIPУ��
	if ((pBattleOption->wOptionMask & OPTION_MASK_SAMEIP) == 0 && pBattleConfig->cbAllowSameIP == TRUE)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-9"), TraceLevel_Exception);
		return false;
	}

	//�Թ�У��
	if ((pBattleOption->wOptionMask & OPTION_MASK_LOOKON) == 0 && pBattleConfig->cbLookonMode != LOOKON_MODE_FORBID)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-10"), TraceLevel_Exception);
		return false;
	}

	//��������
	bool bOptionExists = false;
	BYTE cbConsumeIndex = 0;
	LONGLONG lConsumeCount = 0;

	//�������
	for (int i = 0; i < pBattleOption->cbItemCount; i++)
	{
		//��������
		if ((pBattleConfig->cbSettleKind == SETTLE_KIND_CUSTOM) ||
			(pBattleConfig->cbSettleKind == SETTLE_KIND_COUNT && pBattleOption->OptionItem[i].wPlayCount == pBattleConfig->wPlayCount))
		{
			//���ñ���
			bOptionExists = true;

			//���ķ���
			if (cbCurrencyKind == CURRENCY_KIND_CARD)
			{
				if (wPayType == PAY_TYPE_USER) lConsumeCount = pBattleOption->OptionItem[i].lRoomCardAA;
				if (wPayType == PAY_TYPE_OWNER) lConsumeCount = pBattleOption->OptionItem[i].lRoomCard;
				if (wPayType == PAY_TYPE_WINNER) lConsumeCount = pBattleOption->OptionItem[i].lRoomCard;
			}

			//���Ľ��
			if (cbCurrencyKind == CURRENCY_KIND_GOLD)
			{
				lConsumeCount = pBattleConfig->lEntryCost;
			}

			//��������
			if (lConsumeCount != 0) cbConsumeIndex = i + 1;

			break;
		}		
	}

	//�����ж�
	if (bOptionExists == false)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-11"), TraceLevel_Exception);

		return false;
	}

	//�׷��ж�
	if ((pBattleConfig->lBaseScore<pBattleOption->lMinBaseScore || pBattleConfig->lBaseScore>pBattleOption->lMaxBaseScore))
	{
		CString strMessage;
		strMessage.Format(TEXT("SUB_GP_CREATE_TABLE_LUA-12 KIndID=%d,BaseScore=%I64d,MinBaseSocre=%I64d,MaxBaseScore=%I64d"), pBattleConfig->wKindID, pBattleConfig->lBaseScore, pBattleOption->lMinBaseScore, pBattleOption->lMaxBaseScore);
		CTraceService::TraceString(strMessage, TraceLevel_Exception);

		return false;
	}

	//�볡���ж�
	if (cbCurrencyKind == CURRENCY_KIND_GOLD && (pBattleConfig->lEntryCost<pBattleOption->lMinEntryCost || pBattleConfig->lEntryCost>pBattleOption->lMaxEntryCost))
	{
		CString strMessage;
		strMessage.Format(TEXT("SUB_GP_CREATE_TABLE_LUA-13 KIndID=%d,lEntryCost=%I64d,lMinEntryCost=%I64d,lMaxEntryCost=%I64d"), pBattleConfig->wKindID, pBattleConfig->lEntryCost, pBattleOption->lMinEntryCost, pBattleOption->lMaxEntryCost);
		CTraceService::TraceString(strMessage, TraceLevel_Exception);

		return false;
	}

	return true;
}

//У������
bool CAttemperEngineSink::VerifyBattleConfig(tagBattleConfig* pBattleConfig, tagBattleConfigEx& BattleConfig)
{
	//��������
	tagBattleBaseOptionEx* pBattleOptionEx = NULL;
	for (INT_PTR i = 0; i < m_BattleBaseOptionActive.GetCount(); i++)
	{
		if (m_BattleBaseOptionActive[i].BattleOption.wKindID == pBattleConfig->wKindID &&
			m_BattleBaseOptionActive[i].BattleOption.cbScoreKind == pBattleConfig->cbScoreKind &&
			m_BattleBaseOptionActive[i].BattleOption.wPlayerCount == pBattleConfig->wPlayerCount &&
			m_BattleBaseOptionActive[i].BattleOption.cbEmployScene == pBattleConfig->cbEmployScene)
		{
			pBattleOptionEx = &m_BattleBaseOptionActive[i];
			break;
		}
	}

	//���ò�����
	if (pBattleOptionEx == NULL)
	{
		CTraceService::TraceString(TEXT("�Ҳ���Լս����"), TraceLevel_Exception);

		return false;
	}

	//��������		    			
	WORD wPayType = pBattleConfig->wPayType;
	BYTE cbScoreKind = pBattleConfig->cbScoreKind;
	BYTE cbSettleKind = pBattleConfig->cbSettleKind;
	BYTE cbCurrencyKind = pBattleConfig->cbCurrencyKind;
	tagBattleBaseOption* pBattleOption = &pBattleOptionEx->BattleOption;

	//֧����ʽ
	if ((wPayType & pBattleOption->wSuportPayType) == 0)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE-1"), TraceLevel_Exception);

		return false;
	}

	//����������
	if (cbSettleKind == SETTLE_KIND_COUNT && (pBattleOption->wOptionMask&OPTION_MASK_COUNT)==0)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE-2"), TraceLevel_Exception);

		return false;
	}

	//�Զ������
	if (cbSettleKind == SETTLE_KIND_CUSTOM && (pBattleOption->wOptionMask&OPTION_MASK_CUSTOM)==0)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE-3"), TraceLevel_Exception);

		return false;
	}

	//��������
	if (cbCurrencyKind != CURRENCY_KIND_GOLD && cbCurrencyKind != CURRENCY_KIND_CARD)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-4"), TraceLevel_Exception);

		ASSERT(FALSE);
		return false;
	}

	//�������
	if ((pBattleOption->wOptionMask & OPTION_MASK_GOLD) == 0 && cbCurrencyKind == CURRENCY_KIND_GOLD)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-5"), TraceLevel_Exception);
		return false;		
	}

	//��������
	if ((pBattleOption->wOptionMask & OPTION_MASK_CARD) == 0 && cbCurrencyKind == CURRENCY_KIND_CARD)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-6"), TraceLevel_Exception);
		return false;		
	}

	//����У��
	if ((pBattleOption->wOptionMask & OPTION_MASK_VOICE) == 0 && pBattleConfig->cbAllowVoice==TRUE)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-7"), TraceLevel_Exception);
		return false;		
	}	

	//����У��
	if ((pBattleOption->wOptionMask & OPTION_MASK_PUBLIC) == 0 && pBattleConfig->cbAllowPublic == TRUE)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-8"), TraceLevel_Exception);
		return false;
	}

	//ͬIPУ��
	if ((pBattleOption->wOptionMask & OPTION_MASK_SAMEIP) == 0 && pBattleConfig->cbAllowSameIP == TRUE)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-9"), TraceLevel_Exception);
		return false;
	}

	//�Թ�У��
	if ((pBattleOption->wOptionMask & OPTION_MASK_LOOKON) == 0 && pBattleConfig->cbLookonMode != LOOKON_MODE_FORBID)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-10"), TraceLevel_Exception);
		return false;
	}



	//��������
	bool bOptionExists = false;
	BYTE cbConsumeIndex = 0;
	LONGLONG lConsumeCount = 0;

	//�������
	for (int i = 0; i < pBattleOption->cbItemCount; i++)
	{
		//��������
		if ((pBattleConfig->cbSettleKind == SETTLE_KIND_COUNT && pBattleOption->OptionItem[i].wPlayCount == pBattleConfig->wPlayCount) || 
			(pBattleConfig->cbSettleKind == SETTLE_KIND_CUSTOM))
		{
			//���ñ���
			bOptionExists = true;

			//���ķ���
			if (cbCurrencyKind == CURRENCY_KIND_CARD)
			{
				if (wPayType == PAY_TYPE_USER) lConsumeCount = pBattleOption->OptionItem[i].lRoomCardAA;
				if (wPayType == PAY_TYPE_OWNER) lConsumeCount = pBattleOption->OptionItem[i].lRoomCard;
				if (wPayType == PAY_TYPE_WINNER) lConsumeCount = pBattleOption->OptionItem[i].lRoomCard;
			}						

			//���Ľ��
			if (cbCurrencyKind == CURRENCY_KIND_GOLD)
			{
				lConsumeCount = pBattleConfig->lEntryCost;
			}

			//��������
			if (lConsumeCount != 0) cbConsumeIndex = i + 1;

			break;
		}
	}

	//�����ж�
	if (bOptionExists == false)
	{
		CTraceService::TraceString(TEXT("SUB_GP_CREATE_TABLE_LUA-11"), TraceLevel_Exception);

		return false;
	}	

	//�׷��ж�
	if ((pBattleConfig->lBaseScore<pBattleOption->lMinBaseScore || pBattleConfig->lBaseScore>pBattleOption->lMaxBaseScore))
	{
		CString strMessage;
		strMessage.Format(TEXT("SUB_GP_CREATE_TABLE_LUA-12 KIndID=%d,BaseScore=%I64d,MinBaseSocre=%I64d,MaxBaseScore=%I64d"), pBattleConfig->wKindID, pBattleConfig->lBaseScore, pBattleOption->lMinBaseScore, pBattleOption->lMaxBaseScore);
		CTraceService::TraceString(strMessage, TraceLevel_Exception);

		return false;
	}

	//�볡���ж�
	if (cbCurrencyKind == CURRENCY_KIND_GOLD && (pBattleConfig->lEntryCost<pBattleOption->lMinEntryCost || pBattleConfig->lEntryCost>pBattleOption->lMaxEntryCost))
	{
		CString strMessage;
		strMessage.Format(TEXT("SUB_GP_CREATE_TABLE_LUA-13 KIndID=%d,lEntryCost=%I64d,lMinEntryCost=%I64d,lMaxEntryCost=%I64d"), pBattleConfig->wKindID, pBattleConfig->lEntryCost, pBattleOption->lMinEntryCost, pBattleOption->lMaxEntryCost);
		CTraceService::TraceString(strMessage, TraceLevel_Exception);

		return false;
	}	

	//��ʼ������
	ZeroMemory(&BattleConfig, sizeof(BattleConfig));

	//��������	
	BattleConfig.wKindID = pBattleConfig->wKindID;
	BattleConfig.wPayType = pBattleConfig->wPayType;
	BattleConfig.wPlayCount = pBattleConfig->wPlayCount;
	BattleConfig.cbScoreKind = pBattleConfig->cbScoreKind;
	BattleConfig.cbEmployScene = pBattleConfig->cbEmployScene;
	BattleConfig.lBaseScore = pBattleConfig->lBaseScore;
	BattleConfig.lEntryCost = pBattleConfig->lEntryCost;
	BattleConfig.wPlayerCount = pBattleConfig->wPlayerCount;
	BattleConfig.cbSettleKind = pBattleConfig->cbSettleKind;
	BattleConfig.cbCurrencyKind = pBattleConfig->cbCurrencyKind;
	
	//ѡ������
	BattleConfig.cbLookonMode = pBattleConfig->cbLookonMode;
	BattleConfig.cbAllowVoice = pBattleConfig->cbAllowVoice;
	BattleConfig.cbAllowPublic = pBattleConfig->cbAllowPublic;	
	BattleConfig.cbAllowSameIP = pBattleConfig->cbAllowSameIP;


	//kk jia start
	BattleConfig.lMinWin = pBattleConfig->lMinWin;
	BattleConfig.lWin2 = pBattleConfig->lWin2;
	BattleConfig.lWin3 = pBattleConfig->lWin3;
	BattleConfig.lRevenueRate1 = pBattleConfig->lRevenueRate1;
	BattleConfig.lRevenueRate2 = pBattleConfig->lRevenueRate2;
	BattleConfig.lRevenueRate3 = pBattleConfig->lRevenueRate3;
	//kk jia end
	//CString s;
	//s.Format(_T("BattleConfig�������� m_lWinnerScore1:%I64d, m_lWinnerScore2:%I64d, m_lWinnerScore3:%I64d, m_lWinnerRevenue1:%I64d, m_lWinnerRevenue2:%I64d, m_lWinnerRevenue3:%I64d"), BattleConfig.lMinWin, BattleConfig.lWin2, BattleConfig.lWin3, BattleConfig.lRevenueRate1, BattleConfig.lRevenueRate2, BattleConfig.lRevenueRate3);
	//CTraceService::TraceString(s, TraceLevel_Exception);

	//������Ϣ
	BattleConfig.lConsumeCount = lConsumeCount;
	BattleConfig.cbConsumeIndex = cbConsumeIndex;

	//�߼�����
	BattleConfig.cbAdvancedSize = pBattleConfig->cbAdvancedSize;
	CopyMemory(BattleConfig.cbAdvancedOption, pBattleConfig->cbAdvancedOption, BattleConfig.cbAdvancedSize);

	//���⴦��
	if (BattleConfig.cbAdvancedSize == 0)
	{
		ZeroMemory(BattleConfig.cbAdvancedOption, sizeof(BattleConfig.cbAdvancedOption));
	}

	//�������
	if (BattleConfig.cbCurrencyKind == CURRENCY_KIND_GOLD)
	{
		auto pAdvancedOption = (tagAdvancedOption*)BattleConfig.cbAdvancedOption;
		BattleConfig.wWinnerCount = pBattleOptionEx->BattleOption.wWinnerCount;
		BattleConfig.lWinnerRevenue = pBattleOptionEx->BattleOption.lWinnerRevenue;
		BattleConfig.lWinnerScore = (lConsumeCount * pBattleConfig->wPlayerCount * (1000L - BattleConfig.lWinnerRevenue- pAdvancedOption->lRevenueRate) / 1000L)/BattleConfig.wWinnerCount;
	}	
	else
	{
		//����˰��
		BattleConfig.lGameRevenue = pBattleOptionEx->BattleOption.lWinnerRevenue;
	}
		
	//������Ϣ		
	BattleConfig.wBattleOptionID = pBattleOptionEx->wOptionID;
	BattleConfig.wNotifyMask = NOTIFY_MASK_TABLE_PARAM | NOTIFY_MASK_TABLE_USER;

	//��Ϸ����
	BattleConfig.cbOptionSize = pBattleConfig->cbOptionSize;
	CopyMemory(BattleConfig.cbGameOption, pBattleConfig->cbGameOption, BattleConfig.cbOptionSize);	

	return true;
}

//���ͬ��
VOID CAttemperEngineSink::AppendSynchroItem(WORD wServerID)
{
	for (int i = 0; i < m_SynchroItemArray.GetCount(); i++)
	{
		if (m_SynchroItemArray[i] == wServerID)
		{
			return;
		}
	}

	m_SynchroItemArray.Add(wServerID);
}

//�Ƴ�ͬ��
VOID CAttemperEngineSink::RemoveSynchroItem(WORD wServerID)
{
	for (int i = 0; i < m_SynchroItemArray.GetCount(); i++)
	{
		if (m_SynchroItemArray[i] == wServerID)
		{
			m_SynchroItemArray.RemoveAt(i);
			break;
		}
	}
}

// ��������
bool CAttemperEngineSink::VerifyTableCount(DWORD dwClubID, WORD wTableCount)
{
	//��������
	WORD wMaxTableCount = 0;

	//����Լս
	if (dwClubID == 0)
	{
		wMaxTableCount = m_GlobalOption.wNormalMaxTableCount;
	}
	else
	{
		wMaxTableCount = m_GlobalOption.wClubMaxTableCount;		
	}

	return wMaxTableCount >= wTableCount; 
}

//����ʧ��
VOID CAttemperEngineSink::SendOperateFailure(WORD wServiceID, DWORD dwClubID, tagSocketInfo& SocketInfo, LONG lErrorCode, LPCTSTR pszDescribe)
{
	//��������
	CMD_BC_S_OperateFailure OperateFailure;
	ZeroMemory(&OperateFailure, sizeof(OperateFailure));

	//����ṹ
	OperateFailure.dwClubID = dwClubID;
	OperateFailure.SocketInfo = SocketInfo;
	OperateFailure.lErrorCode = lErrorCode;
	StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), pszDescribe);

	//������Ϣ
	WORD wStringSize = CountStringBuffer(OperateFailure.szErrorMessage);
	WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage) + wStringSize;
	SendDataToClub(wServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_OPERATE_FAILURE, &OperateFailure, wSendSize);
}

//����ʧ��
VOID CAttemperEngineSink::SendOperateFailure(WORD wServiceID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe)
{
	//��������
	auto pRequestFailure = (CMD_CM_OperateFailure*)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer + sizeof(CMD_CM_OperateFailure), sizeof(m_cbBuffer) - sizeof(CMD_CM_OperateFailure));

	//���ñ���
	pRequestFailure->lErrorCode = lErrorCode;
	pRequestFailure->wRequestCmdID = wRequestCmdID;

	//������Ϣ
	if (pszDescribe != NULL)  SendPacket.AddPacket(pszDescribe, DTP_CM_DESCRIBE);

	//�������
	WORD wDataSize = sizeof(CMD_CM_OperateFailure) + SendPacket.GetDataSize();
	SendDataToGate(wServiceID, MDM_CM_SYSTEM, SUB_CM_OPERATE_FAILURE, pRequestFailure, wDataSize, &dwTokenID, 1);

	return;
}

//�����ɹ�
VOID CAttemperEngineSink::SendOperateSuccess(WORD wServiceID, DWORD dwClubID, tagSocketInfo& SocketInfo, LONG lErrorCode, LPCTSTR pszDescribe)
{
	//��������
	CMD_BC_S_OperateSuccess OperateSuccess;
	ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

	//����ṹ
	OperateSuccess.dwClubID = dwClubID;
	OperateSuccess.SocketInfo = SocketInfo;
	OperateSuccess.lOperateCode = lErrorCode;
	StringCchCopy(OperateSuccess.szDescribeString, CountArray(OperateSuccess.szDescribeString), pszDescribe);

	//������Ϣ
	WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
	WORD wSendSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
	SendDataToClub(wServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_OPERATE_SUCCESS, &OperateSuccess, wSendSize);
}

//�����ɹ�
VOID CAttemperEngineSink::SendOperateSuccess(WORD wServiceID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe)
{
	//��������
	auto pRequestSuccess = (CMD_CM_OperateSuccess*)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer + sizeof(CMD_CM_OperateSuccess), sizeof(m_cbBuffer) - sizeof(CMD_CM_OperateSuccess));

	//���ñ���
	pRequestSuccess->lErrorCode = lErrorCode;
	pRequestSuccess->wRequestCmdID = wRequestCmdID;

	//������Ϣ
	if (pszDescribe != NULL)  SendPacket.AddPacket(pszDescribe, DTP_CM_DESCRIBE);	

	//�������
	WORD wDataSize = sizeof(CMD_CM_OperateSuccess) + SendPacket.GetDataSize();
	SendDataToGate(wServiceID, MDM_CM_SYSTEM, SUB_CM_OPERATE_SUCCESS, pRequestSuccess, wDataSize, &dwTokenID, 1);

	return;
}

//��Ϸѡ��
WORD CAttemperEngineSink::ConstructGameOption(INT& nStartIndex, WORD wModuleID, BYTE cbEmployScene, BYTE cbBuffer[], WORD wBufferSize)
{
	//��������
	WORD wPacketSize = 0;
	WORD wOptionSize = 0;
	
	//ö������
	for (INT_PTR i = nStartIndex; i < m_BattleGameOptionActive.GetCount(); i++)
	{
		if (m_BattleGameOptionActive[i].BattleGameOption.cbEmployScene == cbEmployScene &&
			(wModuleID == INVALID_WORD || wModuleID == m_BattleGameOptionActive[i].BattleGameOption.wKindID))
		{
			//�մ�����
			if (m_BattleGameOptionActive[i].BattleGameOption.szOptionJson[0] == 0) continue;

			//���㳤��
			wOptionSize = CountStringBuffer(m_BattleGameOptionActive[i].BattleGameOption.szOptionJson);

			//����ж�
			if (wPacketSize + sizeof(tagBattleGameOption) - sizeof(m_BattleGameOptionActive[i].BattleGameOption.szOptionJson) + wOptionSize > wBufferSize)
			{
				nStartIndex = i;
				return wPacketSize;
			}

			auto pBattleGameOptionEx = &m_BattleGameOptionActive[i];

			//���ñ�ʶ
			*(WORD*)(cbBuffer + wPacketSize) = m_BattleGameOptionActive[i].BattleGameOption.wKindID;
			wPacketSize += sizeof(WORD);

			//���ñ�ʶ
			*(BYTE*)(cbBuffer + wPacketSize) = m_BattleGameOptionActive[i].BattleGameOption.cbEmployScene;
			wPacketSize += sizeof(BYTE);

			//���ó���
			*(WORD*)(cbBuffer + wPacketSize) = wOptionSize;
			wPacketSize += sizeof(WORD);

			//��������
			CopyMemory(cbBuffer + wPacketSize, m_BattleGameOptionActive[i].BattleGameOption.szOptionJson, wOptionSize);
			wPacketSize += wOptionSize;
		}
	}

	//��������
	nStartIndex = m_BattleGameOptionActive.GetCount();

	return wPacketSize;
}

//Լսѡ��
WORD CAttemperEngineSink::ConstructBaseOption(INT& nStartIndex, WORD wModuleID, BYTE cbEmployScene, BYTE cbBuffer[], WORD wBufferSize)
{
	//��������
	WORD wPacketSize = 0;
	
	//ö������
	for (INT_PTR i = nStartIndex; i < m_BattleBaseOptionActive.GetCount(); i++)
	{
		if (m_BattleBaseOptionActive[i].BattleOption.cbEmployScene == cbEmployScene &&
			(wModuleID == INVALID_WORD || wModuleID == m_BattleBaseOptionActive[i].BattleOption.wKindID))
		{
			//У���С
			if (wPacketSize + sizeof(tagBattleBaseOption) > wBufferSize)
			{
				nStartIndex = i;
				return wPacketSize;
			}

			//��ȡ����
			auto pBattleBaseOption = (tagBattleBaseOption*)(cbBuffer + wPacketSize);
			CopyMemory(pBattleBaseOption, &m_BattleBaseOptionActive[i].BattleOption, sizeof(tagBattleBaseOption));

			//λ��ƫ��
			wPacketSize += sizeof(tagBattleBaseOption);
		}
	}

	nStartIndex = m_BattleBaseOptionActive.GetCount();

	return wPacketSize;
}

//���ӷ���
bool CAttemperEngineSink::SendDataToGame(WORD wServerID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	auto pGlobalServerItem=m_GlobalInfoManager.SearchServerItem(wServerID);
	if (pGlobalServerItem==NULL) return false;

	//��������
	auto dwSocketID= pGlobalServerItem->GetSocketID();
	m_pITCPNetworkEngine->SendData(dwSocketID,wMainCmdID,wSubCmdID,pData,wDataSize,NULL,0);

	return true;
}

//��ݷ���
bool CAttemperEngineSink::SendDataToClub(WORD wServiceID, WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	//���Ҳ��
	auto pGlobalClubItem = m_GlobalInfoManager.SearchClubItem(wServiceID);
	if (pGlobalClubItem == NULL) return false;

	//��������
	auto dwSocketID = pGlobalClubItem->GetSocketID();
	m_pITCPNetworkEngine->SendData(dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize, NULL, 0);

	return true;
}

//���ط���
bool CAttemperEngineSink::SendDataToGate(WORD wServiceID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	//��������
	auto pGlobalGateItem = m_GlobalInfoManager.SearchGateItem(wServiceID);
	if (pGlobalGateItem == NULL) return false;

	//��������
	DWORD dwSocketID = pGlobalGateItem->GetSocketID();
	m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(wMainCmdID, SERVICE_MODULE_BATTLE), wSubCmdID, pData, wDataSize, pdwTokenID, wTokenCount);

	return true;
}

//Լս����
bool CAttemperEngineSink::OnTCPNetWorkMainGBBattleService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ��Ϣ
	WORD wBindIndex = LOWORD(dwSocketID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);

	//��ȡ����
	auto pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	switch (wSubCmdID)
	{
	case SUB_GB_C_QUERY_OPTION:			//��ѯ����
	{
		//����У��
		ASSERT (wDataSize == sizeof(CMD_GB_C_QueryOption));
		if (wDataSize != sizeof(CMD_GB_C_QueryOption)) return true;

		//��ȡ����
		auto pQueryOption = (CMD_GB_C_QueryOption*)pData;

		//��������
		INT nStartIndex = 0;

		//��������
		while (nStartIndex < m_BattleBaseOptionActive.GetCount())
		{
			auto wPacketSize = ConstructBaseOption(nStartIndex, pQueryOption->wKindID, EMPLOY_SCENE_CLUB_OUTSIDE, m_cbBuffer,CountArray(m_cbBuffer));
			if (wPacketSize > 0)
			{
				//��������
				SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_BATTLE_BASE_OPTION, m_cbBuffer, wPacketSize, &dwTokenID, 1);
			}
		}

		nStartIndex = 0;

		//��Ϸ����
		while (nStartIndex < m_BattleGameOptionActive.GetCount())
		{
			auto wPacketSize = ConstructGameOption(nStartIndex, pQueryOption->wKindID, EMPLOY_SCENE_CLUB_OUTSIDE, m_cbBuffer, CountArray(m_cbBuffer));
			if (wPacketSize > 0)
			{
				//��������
				SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_BATTLE_GAME_OPTION, m_cbBuffer, wPacketSize, &dwTokenID, 1);
			}
		}
		
		//��������
		SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_BATTLE_OPTION_FINISH, NULL, 0, &dwTokenID, 1);

		return true;
	}
	case SUB_GB_C_QUERY_TABLE:			//��ѯ����	
	{
		//��ֵ����
		CTableItemArray TableItemArray;
		CTableParamArray TableParamArray;

		//��ȡ����				
		m_BattleManager.GetUserTableList(pTokenParameter->dwUserID, TableItemArray, TableParamArray);

		//��������
		INT nStartIndex = 0;
		
		//�����б�
		while (nStartIndex < TableItemArray.GetCount())
		{
			auto wPacketSize = ConstructTableList(TableItemArray, TableParamArray, nStartIndex, m_cbBuffer, CountArray(m_cbBuffer));
			if (wPacketSize > 0)
			{
				//��������
				SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_ITEM, m_cbBuffer, wPacketSize, &dwTokenID, 1);
				wPacketSize = 0;
			}
		}				

		//�������
		SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_FINISH, NULL, 0, &dwTokenID, 1);

		return true;
	}	
	case SUB_GB_C_SEARCH_TABLE:			//��������	
	{
		//����У��
		ASSERT (sizeof(CMD_GB_C_SearchTable) == wDataSize);
		if (sizeof(CMD_GB_C_SearchTable) != wDataSize) return false;

		//��ȡ����
		auto pSearchTable = (CMD_GB_C_SearchTable*)pData;

		//��ѯ����
		auto pTableItemEx = m_BattleManager.GetTableItem(pSearchTable->dwBattleNum);
		if (pTableItemEx == NULL || pTableItemEx->wTableID == INVALID_TABLE)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_QUERY_TABLE, BATTLE_CODE_QUERY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0013, TEXT("��Ǹ��ϵͳû�в�ѯ������������ӣ���ȷ�����Ӻ��ٴγ��ԣ�")));

			return true;
		}

		//��ѯ����
		auto pTableParam = m_BattleManager.GetTableParam(pSearchTable->dwBattleNum);
		if (pTableParam == NULL)
		{
			//����ʧ��
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_QUERY_TABLE, BATTLE_CODE_QUERY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0013, TEXT("��Ǹ��ϵͳû�в�ѯ������������ӣ���ȷ�����Ӻ��ٴγ��ԣ�")));

			return true;
		}

		//У���û�
		if (pTableItemEx->dwClubID != 0)
		{
			//�������
			CMD_BC_S_UserEfficacy UserEfficacy = {};

			//����ṹ						
			UserEfficacy.dwUserID = pTokenParameter->dwUserID;
			UserEfficacy.dwClubID = pTableItemEx->dwClubID;
			UserEfficacy.dwBattleNum = pTableItemEx->dwBattleNum;
			UserEfficacy.wBattleID = m_pServiceOption->wServiceID;

			//������Ϣ
			UserEfficacy.UserSocketInfo.dwTokenID = dwTokenID;
			UserEfficacy.UserSocketInfo.wGateID = pBindParameter->wServiceID;

			//������Ϣ
			SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_USER_EFFICACY, &UserEfficacy, sizeof(UserEfficacy));

			return true;
		}
			

		//��������
		auto pBattleTableItem = (tagBattleTableItem*)m_cbBuffer;
		auto pBattleTableParam = (tagBattleTableParam*)(pBattleTableItem + 1);

		//��������
		ConstructTableItem(pTableItemEx, pBattleTableItem);

		//���Ӳ���
		pBattleTableParam->wUserCount = 0;
		pBattleTableParam->dwStartTime = pTableParam->dwStartTime;
		pBattleTableParam->wFinishCount = pTableParam->wFinishCount;
		pBattleTableParam->cbGameStatus = pTableParam->cbGameStatus;
		pBattleTableParam->dwBattleNum = pBattleTableItem->dwBattleNum;

		//������Ϣ
		SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_SEARCH_RESULT, m_cbBuffer, sizeof(tagBattleTableItem) + sizeof(tagBattleTableParam), &dwTokenID, 1);

		return true;
	}
	case SUB_GB_C_CREATE_TABLE:			//��������	
	{
		//����У��
		ASSERT(sizeof(CMD_GB_C_CreateTable) >= wDataSize);
		if (sizeof(CMD_GB_C_CreateTable) < wDataSize) return false;

		//��ȡ����
		auto pCreateTable = (CMD_GB_C_CreateTable*)pData;

		//����״̬
		if (m_bBattleService == false)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_BATTLE_0002, TEXT("ϵͳά���У�Լս������ʱֹͣ��������ά����ɺ����ԣ�")));

			return true;
		}

		//��������
		tagBattleConfigEx BattleConfig;
		ZeroMemory(&BattleConfig, sizeof(BattleConfig));

		//��ȡ����
		auto pBattleConfig = &pCreateTable->BattleConfig;

		//У������
		if (VerifyBattleConfig(pBattleConfig, BattleConfig) == false)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_BATTLE_0003, TEXT("���Ӵ���ʧ�ܣ�")));

			return true;
		}

		//�ж�����
		auto wTableCount = m_BattleManager.GetTableCountByKindID(pTokenParameter->dwUserID, 0, 0);
		if (VerifyTableCount(0, wTableCount + 1) == false)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_BATTLE_0004, TEXT("��Ǹ����������������Ŀ�Ѿ��ﵽ���ޣ��������ʣ�����ϵ�ͷ���")));

			return true;
		}

		//��������
		auto pTableItemEx = m_BattleManager.CreateTable(pTokenParameter->dwUserID,0, BattleConfig);
		if (pTableItemEx == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessageV(MIM_BATTLE_0005, TEXT("��Ǹ�����Ӵ���ʧ��[%d]�����Ժ��ٴγ��ԣ�"), 2));

			return true;
		}

		//��������
		auto pTableOwner = m_BattleManager.CreateTableOwner(pTableItemEx->dwBattleNum);
		if (pTableOwner == NULL)
		{
			//�Ƴ�����
			m_BattleManager.RemoveTable(pTableItemEx->dwBattleNum);

			//����ʧ��
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessageV(MIM_BATTLE_0005, TEXT("��Ǹ�����Ӵ���ʧ��[%d]�����Ժ��ٴγ��ԣ�"), 3));

			return true;
		}

		//��ȡ��Ϣ
		auto pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL)
		{
			//�Ƴ�����
			m_BattleManager.RemoveTable(pTableItemEx->dwBattleNum);
			m_BattleManager.RemoveTableOwner(pTableItemEx->dwBattleNum);

			//����ʧ��
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessageV(MIM_BATTLE_0005, TEXT("��Ǹ�����Ӵ���ʧ��[%d]�����Ժ��ٴγ��ԣ�"), 4));

			return true;
		}

		//��������
		StringCchCopy(pTableItemEx->szPassword, CountArray(pTableItemEx->szPassword), pCreateTable->szTablePassword);

		//������Ϣ			
		pTableOwner->dwUserID = pTokenParameter->dwUserID;
		
		//������Ϣ
		pTableOwner->SocketInfo.dwTokenID = dwTokenID;
		pTableOwner->SocketInfo.wGateID = pBindParameter->wServiceID;

		//����ṹ
		CMD_BS_C_CreateTable CreateTable;
		ZeroMemory(&CreateTable, sizeof(CreateTable));

		//�û���Ϣ			
		CreateTable.dwUserID = pTokenParameter->dwUserID;
		StringCchCopy(CreateTable.szPassword, CountArray(CreateTable.szPassword), pTableItemEx->szPassword);

		//������Ϣ
		CreateTable.dwConfigID = pTableItemEx->dwConfigID;
		CreateTable.dwBattleNum = pTableItemEx->dwBattleNum;		
		CopyMemory(&CreateTable.BattleConfig, &BattleConfig, sizeof(CreateTable.BattleConfig));

		//������Ϣ
		SendDataToGame(pTableItemEx->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_CREATE_TABLE, &CreateTable, sizeof(CreateTable));

		return true;
	}
	case SUB_GB_C_MODIFY_TABLE:			//�޸�����
	{
		//����У��
		ASSERT (sizeof(CMD_GB_C_ModifyTable) >= wDataSize);
		if (sizeof(CMD_GB_C_ModifyTable) < wDataSize) return false;

		//��ȡ����
		auto pModifyTable = (CMD_GB_C_ModifyTable*)pData;

		//����״̬
		if (m_bBattleService == false)
		{
			//����ʧ��
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_MODIFY_TABLE, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0002, TEXT("ϵͳά���У�Լս������ʱֹͣ��������ά����ɺ����ԣ�")));

			return true;
		}

		//��������
		auto pTableItemEx = m_BattleManager.GetTableItem(pModifyTable->dwBattleNum);
		if (pTableItemEx == NULL)
		{
			//����ʧ��
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_MODIFY_TABLE, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0009, TEXT("��Ǹ��ϵͳδ��ѯ����Ҫ�޸ĵ����ӣ���ˢ�º��ٴγ��ԣ�")));

			return true;
		}

		//У��״̬
		auto pTableParamEx = m_BattleManager.GetTableParam(pModifyTable->dwBattleNum);
		if (pTableParamEx->wFinishCount > 0)
		{
			//����ʧ��
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_MODIFY_TABLE, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0018, TEXT("��Ϸ�Ѿ���ʼ�����������޸�ʧ�ܣ�")));

			return true;
		}

		//У���û�			
		if (pTableItemEx->dwOwnerID != pTokenParameter->dwUserID)
		{
			//����ʧ��
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_MODIFY_TABLE, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0010, TEXT("��Ǹ�������Ǹò�����ϰ壬û��Ȩ���޸��������ã�")));

			return true;
		}

		//��������
		tagBattleConfigEx BattleConfig;
		ZeroMemory(&BattleConfig, sizeof(BattleConfig));

		//��ȡ����
		auto pBattleConfig = &pModifyTable->BattleConfig;

		//У������
		if (VerifyBattleConfig(pBattleConfig, BattleConfig) == false)
		{
			//����ʧ��
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_MODIFY_TABLE, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0011, TEXT("�����޸�ʧ�ܣ�")));

			return true;
		}

		//����ṹ
		CMD_BS_C_ModifyTable ModifyTable;
		ZeroMemory(&ModifyTable, sizeof(ModifyTable));

		//�û���Ϣ			
		ModifyTable.wTableID = pTableItemEx->wTableID;
		ModifyTable.dwUserID = pTokenParameter->dwUserID;
		ModifyTable.dwBattleNum = pModifyTable->dwBattleNum;

		//Լս����
		CopyMemory(&ModifyTable.BattleConfig, &BattleConfig, sizeof(BattleConfig));

		//������Ϣ
		ModifyTable.SocketInfo.dwTokenID = dwTokenID;
		ModifyTable.SocketInfo.wGateID = pBindParameter->wServiceID;

		//������Ϣ
		SendDataToGame(pTableItemEx->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_MODIFY_TABLE, &ModifyTable, sizeof(ModifyTable));

		return true;
	}
	case SUB_GB_C_DISMISS_TABLE:		//��ɢ����
	{
		//����У��
		ASSERT (sizeof(CMD_GB_C_DismissTable) == wDataSize);
		if (sizeof(CMD_GB_C_DismissTable) != wDataSize) return false;

		//��ȡ����
		auto pDismissTable = (CMD_GB_C_DismissTable*)pData;

		//��������
		auto pTableItemEx = m_BattleManager.GetTableItem(pDismissTable->dwBattleNum);
		if (pTableItemEx == NULL)
		{
			//����ʧ��
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_DISMISS_TABLE, BATTLE_CODE_DISMISS_FAILED, CWHService::FormatMessage(MIM_BATTLE_0012, TEXT("��Ǹ��ϵͳδ��ѯ����Ҫ��ɢ�����ӣ���ˢ�º��ٴγ��ԣ�")));

			return true;
		}

		//�Ƿ���
		if (pTokenParameter->dwUserID != pTableItemEx->dwOwnerID)
		{
			//����ʧ��
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_GB_C_DISMISS_TABLE, BATTLE_CODE_DISMISS_FAILED, CWHService::FormatMessage(MIM_BATTLE_0014, TEXT("��Ǹ�������Ǹ����ӵķ�����û��Ȩ�޽�ɢ��")));

			return true;
		}

		//����ṹ
		CMD_BS_C_DismissTable DismissTable;
		ZeroMemory(&DismissTable, sizeof(DismissTable));

		//������Ϣ
		DismissTable.SocketInfo.dwTokenID = dwTokenID;
		DismissTable.SocketInfo.wGateID = pBindParameter->wServiceID;

		//���ñ���
		DismissTable.wTableID = pTableItemEx->wTableID;
		DismissTable.dwUserID = pTokenParameter->dwUserID;
		DismissTable.dwBattleNum = pDismissTable->dwBattleNum;

		//��������
		SendDataToGame(pTableItemEx->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_DISMISS_TABLE, &DismissTable, sizeof(DismissTable));

		return true;
	}	
	case SUB_GB_C_REQUEST_TABLELIST:	//�����б�
	{
		//����У��
		ASSERT (sizeof(CMD_GB_C_RequestTableList) == wDataSize);
		if (sizeof(CMD_GB_C_RequestTableList) != wDataSize) return false;

		//��ȡ����
		auto pRequestTableList = (CMD_GB_C_RequestTableList*)pData;

		//��������
		int nTableCount = __min(pRequestTableList->wTableCount, MAX_TABLE_COUNT_REQUEST);

		//��ѯ����
		CTableItemArray TableItemArray;
		CTableParamArray TableParamArray;

		//��ȡ����				
		m_BattleManager.GetRandTableList(nTableCount, TableItemArray, TableParamArray);

		//��������
		INT nStartIndex = 0;

		//�����б�
		while (nStartIndex < TableItemArray.GetCount())
		{
			auto wPacketSize = ConstructTableList(TableItemArray, TableParamArray, nStartIndex, m_cbBuffer, CountArray(m_cbBuffer));
			if (wPacketSize > 0)
			{
				//��������
				SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_ITEM, m_cbBuffer, wPacketSize, &dwTokenID, 1);
				wPacketSize = 0;
			}
		}

		//�������
		SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_FINISH, NULL, 0, &dwTokenID, 1);

		return true;
	}
	case SUB_GB_C_REQUEST_TABLEPARAM:	//���Ӳ���
	{
		//����У��
		ASSERT (wDataSize > sizeof(CMD_GB_C_RequestTableParam));
		if (wDataSize <= sizeof(CMD_GB_C_RequestTableParam)) return true;

		//��ȡ����
		auto pRequestTableParam = (CMD_GB_C_RequestTableParam*)pData;

		//У������
		ASSERT (pRequestTableParam->wTableCount <= MAX_TABLE_COUNT_REQUEST);
		if (pRequestTableParam->wTableCount > MAX_TABLE_COUNT_REQUEST) return true;

		//����У��
		ASSERT (pRequestTableParam->wTableCount * sizeof(pRequestTableParam->dwBattleNum[0]) + sizeof(CMD_GB_C_RequestTableParam) == wDataSize);
		if (pRequestTableParam->wTableCount * sizeof(pRequestTableParam->dwBattleNum[0]) + sizeof(CMD_GB_C_RequestTableParam) != wDataSize) return true;

		//��������
		WORD wPacketSize = 0;

		for (int i = 0; i < pRequestTableParam->wTableCount; i++)
		{
			//����״̬
			auto pTableParamEx = m_BattleManager.GetTableParam(pRequestTableParam->dwBattleNum[i]);
			if (pTableParamEx == NULL) continue;

			//�����ж�
			if (wPacketSize + sizeof(tagBattleTableParam) + sizeof(tagBattleTableUser) * TABLE_USER_COUNT > sizeof(m_cbBuffer))
			{
				//��������
				SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_PARAM, m_cbBuffer, wPacketSize, &dwTokenID, 1);

				//���ñ���
				wPacketSize = 0;
			}

			//��ȡ����			
			auto pTableParam = (tagBattleTableParam*)&m_cbBuffer[wPacketSize];

			//���ñ���
			ZeroMemory(pTableParam, sizeof(tagBattleTableParam));
			pTableParam->dwBattleNum = pRequestTableParam->dwBattleNum[i];
			pTableParam->dwStartTime = pTableParamEx->dwStartTime;
			pTableParam->wFinishCount = pTableParamEx->wFinishCount;
			pTableParam->cbGameStatus = pTableParamEx->cbGameStatus;

			//�����û�
			for (int i = 0; i < CountArray(pTableParamEx->TableUserItem); i++)
			{
				if (pTableParamEx->TableUserItem[i].dwUserID == 0) continue;
				CopyMemory(&pTableParam->TableUserItem[pTableParam->wUserCount++], &pTableParamEx->TableUserItem[i], sizeof(tagBattleTableUser));

				//�˳��ж�										
				if (pTableParam->wUserCount == pTableParamEx->wUserCount) break;
			}

			//���ô�С			
			wPacketSize += sizeof(tagBattleTableParam)+pTableParam->wUserCount*sizeof(tagBattleTableUser);
		}

		//��������
		if (wPacketSize > 0)
		{
			SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_PARAM, m_cbBuffer, wPacketSize, &dwTokenID, 1);
		}

		//�������
		SendDataToGate(pBindParameter->wServiceID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_FINISH, NULL, 0, &dwTokenID, 1);

		return true;		
	} 	
	}
	return false;
}

//Լս����
bool CAttemperEngineSink::OnTCPNetWorkMainBSBattleService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	switch (wSubCmdID)
	{	
	case SUB_BS_S_CREATE_TABLE:		//��������
	{
		//����У��
		ASSERT(sizeof(CMD_BS_S_CreateTable) >= wDataSize);
		if (sizeof(CMD_BS_S_CreateTable) < wDataSize) return false;

		//��ȡ����
		auto pCreateTable = (CMD_BS_S_CreateTable *)pData;

		//��������
		auto pTableItemEx = m_BattleManager.GetTableItem(pCreateTable->dwBattleNum);
		if (pTableItemEx == NULL) return true;

		//����״̬
		auto pTableParamEx = m_BattleManager.GetTableParam(pCreateTable->dwBattleNum);
		if (pTableParamEx == NULL) return true;

		//��������
		auto pTableOwner = m_BattleManager.SearchTableOwner(pCreateTable->dwBattleNum);
		if (pTableOwner == NULL) return true;

		//��ȡ����
		auto pSocketInfo = &pTableOwner->SocketInfo;

		//����ʧ��
		if (pCreateTable->lResultCode != 0)
		{
			//��������
			bool bCreateSuccess = false;

			//����δ�ҵ�
			if (pCreateTable->lResultCode == BATTLE_CODE_TABLE_UNFOUND)
			{
				//��������
				tagBattleTableItemEx * pNewTableItem = m_BattleManager.CreateTable(pTableItemEx->dwOwnerID, pTableItemEx->dwClubID, pTableItemEx->BattleConfig, pTableItemEx->wServerID);
				if (pNewTableItem != NULL)
				{
					//��������
					tagTableOwner * pNewTableOwner = m_BattleManager.CreateTableOwner(pNewTableItem->dwBattleNum);
					if (pNewTableOwner != NULL)
					{
						//���ñ�ʶ
						bCreateSuccess = true;

						//�淨��Ϣ
						pNewTableItem->dwConfigID = pTableItemEx->dwConfigID;

						//������Ϣ
						pNewTableItem->wClubServiceID = pTableItemEx->wClubServiceID;
						pNewTableItem->wSwitchOption = pTableItemEx->wSwitchOption;
						pNewTableItem->dwClubCreaterID = pTableItemEx->dwClubCreaterID;

						//��������
						StringCchCopy(pNewTableItem->szPassword, CountArray(pNewTableItem->szPassword), pTableItemEx->szPassword);

						//������Ϣ
						CopyMemory(pNewTableOwner, pTableOwner, sizeof(tagTableOwner));

						//��������
						CMD_BS_C_CreateTable CreateTable;
						ZeroMemory(&CreateTable, sizeof(CreateTable));

						//�û���Ϣ			
						CreateTable.dwUserID = pNewTableItem->dwOwnerID;
						StringCchCopy(CreateTable.szPassword, CountArray(CreateTable.szPassword), pNewTableItem->szPassword);

						//������Ϣ			
						CreateTable.dwClubID = pNewTableItem->dwClubID;
						CreateTable.wSwitchOption = pNewTableItem->wSwitchOption;
						CreateTable.wClubServiceID = pNewTableItem->wClubServiceID;
						CreateTable.dwClubCreaterID = pNewTableItem->dwClubCreaterID;							

						//������Ϣ
						CreateTable.dwConfigID = pNewTableItem->dwConfigID;
						CreateTable.dwBattleNum = pNewTableItem->dwBattleNum;
						CopyMemory(&CreateTable.BattleConfig, &pNewTableItem->BattleConfig, sizeof(CreateTable.BattleConfig));

						//������Ϣ
						SendDataToGame(pNewTableItem->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_CREATE_TABLE, &CreateTable, sizeof(CreateTable));
					}
					else
					{
						//�Ƴ�����							
						m_BattleManager.RemoveTable(pNewTableItem->dwBattleNum);
					}
				}
			}

			//����ʧ��
			if (bCreateSuccess == false)
			{
				//����ʧ��
				if (pTableItemEx->dwClubID != 0)
				{
					SendOperateFailure(pTableItemEx->wClubServiceID, pTableItemEx->dwClubID, *pSocketInfo, BATTLE_CODE_CREATE_FAILED, pCreateTable->szErrorMessage);
				}
				else
				{
					SendOperateFailure(pSocketInfo->wGateID, pSocketInfo->dwTokenID, SUB_GB_C_CREATE_TABLE, pCreateTable->lResultCode, pCreateTable->szErrorMessage);					
				}
			}

			//�Ƴ�����
			m_BattleManager.RemoveTable(pCreateTable->dwBattleNum);
		}
		else
		{
			//���ñ���
			pTableItemEx->wTableID = pCreateTable->wTableID;
			pTableItemEx->dwCreateTime = pCreateTable->dwCreateTime;

			//����֧��
			if (pTableItemEx->BattleConfig.wPayType == PAY_TYPE_OWNER && pCreateTable->dwPayUserID==pTableOwner->dwUserID)
			{
				//����Ƹ�
				CMD_CM_WealthUpdate* pWealthUpdate = (CMD_CM_WealthUpdate*)m_cbBuffer;
				ZeroMemory(pWealthUpdate, sizeof(CMD_CM_WealthUpdate));

				//���֧��
				if (pTableItemEx->BattleConfig.cbCurrencyKind == CURRENCY_KIND_GOLD)
				{
					pWealthUpdate->WealthItem[pWealthUpdate->wItemCount].wMoneyID = GOODS_ID_GOLD;
					pWealthUpdate->WealthItem[pWealthUpdate->wItemCount++].lBalance = pCreateTable->lUserGold;
				}

				//����֧��
				if (pTableItemEx->BattleConfig.cbCurrencyKind == CURRENCY_KIND_CARD)
				{
					pWealthUpdate->WealthItem[pWealthUpdate->wItemCount].wMoneyID = GOODS_ID_CARD;
					pWealthUpdate->WealthItem[pWealthUpdate->wItemCount++].lBalance = pCreateTable->lUserCard;
				}

				//������Ϣ
				WORD wHeadSize = sizeof(CMD_CM_WealthUpdate);
				WORD wDataSize = pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]);
				SendDataToGate(pSocketInfo->wGateID, MDM_CM_SYSTEM, SUB_CM_WEALTH_UPDATE, pWealthUpdate, wHeadSize + wDataSize, &pSocketInfo->dwTokenID, 1);
			}
			
			//��������
			auto pBattleTableItem = (tagBattleTableItem*)m_cbBuffer;
			auto pBattleTableParam = (tagBattleTableParam*)(pBattleTableItem+1);
		
			//��������
			ConstructTableItem(pTableItemEx, pBattleTableItem);

			//���Ӳ���
			pBattleTableParam->dwStartTime = 0;
			pBattleTableParam->wUserCount = 0;
			pBattleTableParam->wFinishCount = 0;
			pBattleTableParam->cbGameStatus = FALSE;
			pBattleTableParam->dwBattleNum = pBattleTableItem->dwBattleNum;

			//����ʧ��
			if (pTableItemEx->dwClubID != 0)
			{
				//�����ɹ�
				SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_TABLE_CREATE, m_cbBuffer, sizeof(tagBattleTableItem)+sizeof(tagBattleTableParam));

				//�����ɹ�
				SendOperateSuccess(pTableItemEx->wClubServiceID, pTableItemEx->dwClubID, *pSocketInfo, pCreateTable->lResultCode, pCreateTable->szErrorMessage);
			}
			else
			{
				//������Ϣ
				SendDataToGate(pSocketInfo->wGateID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_CREATE, m_cbBuffer, sizeof(tagBattleTableItem) + sizeof(tagBattleTableParam), &pSocketInfo->dwTokenID, 1);

				//�����ɹ�
				SendOperateSuccess(pSocketInfo->wGateID, pSocketInfo->dwTokenID, SUB_GB_C_CREATE_TABLE, pCreateTable->lResultCode, pCreateTable->szErrorMessage);
			}
		}

		//�Ƴ�����
		m_BattleManager.RemoveTableOwner(pCreateTable->dwBattleNum);

		return true;
	}	
	case SUB_BS_S_MODIFY_TABLE:		//�޸�����
	{
		//����У��
		ASSERT(sizeof(CMD_BS_S_ModifyTable) >= wDataSize);
		if (sizeof(CMD_BS_S_ModifyTable) < wDataSize) return false;

		//��ȡ����
		auto pModifyTable = (CMD_BS_S_ModifyTable*)pData;

		//��������
		auto pTableItemEx = m_BattleManager.GetTableItem(pModifyTable->dwBattleNum);
		if (pTableItemEx == NULL) return true;

		//������Ϣ
		auto pSocketInfo = &pModifyTable->SocketInfo;

		//�޸��޸�
		if (pModifyTable->cbResultCode != 0)
		{
			if (pTableItemEx->dwClubID != 0)
			{
				SendOperateFailure(pTableItemEx->wClubServiceID, pTableItemEx->dwClubID, *pSocketInfo, pModifyTable->cbResultCode, pModifyTable->szErrorMessage);
			}
			else
			{
				SendOperateFailure(pSocketInfo->wGateID, pSocketInfo->dwTokenID, SUB_BS_C_MODIFY_TABLE, pModifyTable->cbResultCode, pModifyTable->szErrorMessage);
			}
		}
		else
		{
			//��������
			auto pTableItemEx = m_BattleManager.GetTableItem(pModifyTable->dwBattleNum);
			if (pTableItemEx == NULL) return true;			

			//��������
			CopyMemory(&pTableItemEx->BattleConfig, &pModifyTable->BattleConfig, sizeof(pTableItemEx->BattleConfig));

			//�������
			tagBattleTableItem BattleTableItem = {};

			//��������
			ConstructTableItem(pTableItemEx, &BattleTableItem);

			//У�����
			if (pTableItemEx->dwClubID != 0)
			{
				//������Ϣ
				SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_TABLE_UPDATE, &BattleTableItem, sizeof(BattleTableItem));

				//�����ɹ�
				SendOperateSuccess(pTableItemEx->wClubServiceID, pTableItemEx->dwClubID, *pSocketInfo, pModifyTable->cbResultCode, pModifyTable->szErrorMessage);
			}
			else
			{
				//������Ϣ
				SendDataToGate(pSocketInfo->wGateID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_TABLE_UPDATE, &BattleTableItem, sizeof(BattleTableItem), &pSocketInfo->dwTokenID, 1);

				//�����ɹ�
				SendOperateSuccess(pSocketInfo->wGateID, pSocketInfo->dwTokenID, SUB_BS_C_MODIFY_TABLE, pModifyTable->cbResultCode, pModifyTable->szErrorMessage);
			}
		}

		return true;
	}
	case SUB_BS_S_TABLE_SYNCHRO:	//����ͬ��
	{
		//����У��
		ASSERT (wDataSize % sizeof(tagBattleTableItemEx) == 0);
		if (wDataSize % sizeof(tagBattleTableItemEx) != 0) return false;

		//��������
		WORD wTableCount = wDataSize / sizeof(tagBattleTableItemEx);
		auto pTableItemEx = (tagBattleTableItemEx *)pData;

		//�������
		for (WORD i = 0; i < wTableCount; i++)
		{
			m_BattleManager.AddTableItem(*pTableItemEx++);
		}

		return true;
	}
	case SUB_BS_S_SYNCHRO_FINISH:	//ͬ�����
	{
		//����Ϣ
		WORD wBindIndex = LOWORD(dwSocketID);
		auto pBindParameter = (m_pBindParameter + wBindIndex);

		//�Ƴ�����
		RemoveSynchroItem(pBindParameter->wServiceID);

		//ͬ�����
		if (m_SynchroItemArray.GetCount() == 0)
		{
			//���鶨��
			CTableItemArray TableItemArray;
			CTableParamArray TableParamArray;

			//��������
			DWORD dwClubID = 0;
			POSITION Position=NULL;						
			
			do
			{
				dwClubID=m_BattleManager.EnumTableClubID(Position);
				if (dwClubID == 0) break;

				//�Ƴ�Ԫ��
				TableItemArray.RemoveAll();
				TableParamArray.RemoveAll();

				//��ȡ����				
				m_BattleManager.GetClubTableList(dwClubID, TableItemArray, TableParamArray);

				if (TableItemArray.GetCount() > 0)
				{
					//��������
					INT nStartIndex = 0;
					auto pTableItemEx = TableItemArray[0];

					//�����б�
					while (nStartIndex < TableItemArray.GetCount())
					{
						auto wPacketSize = ConstructTableList(TableItemArray, TableParamArray, nStartIndex, m_cbBuffer, CountArray(m_cbBuffer));
						if (wPacketSize > 0)
						{
							//��������
							SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_TABLE_ITEM, m_cbBuffer, wPacketSize);
							wPacketSize = 0;
						}
					}
				}

			} while (Position!=NULL);

		}

		return true;
	}
	case SUB_BS_S_TABLE_USERLIST:	//�����û�
	{
		//����У��
		ASSERT(sizeof(CMD_BS_S_TableUserList) >= wDataSize);
		if (sizeof(CMD_BS_S_TableUserList) < wDataSize) return false;

		//��ȡ����
		auto pTableUserList = (CMD_BS_S_TableUserList *)pData;

		//��������
		auto pTableParamEx = m_BattleManager.GetTableParam(pTableUserList->dwBattleNum);
		if (pTableParamEx != NULL)
		{
			//���ñ���								
			pTableParamEx->wUserCount = pTableUserList->wUserCount;
			CopyMemory(pTableParamEx->TableUserItem, pTableUserList->TableUserItem, sizeof(pTableParamEx->TableUserItem));
		}

		return true;
	}
	case SUB_BS_S_TABLE_PARAM:		//���Ӳ���
	{
		//����У��
		ASSERT(sizeof(CMD_BS_S_TableParam) == wDataSize);
		if (sizeof(CMD_BS_S_TableParam) != wDataSize) return false;

		//��ȡ����
		auto pTableParam = (CMD_BS_S_TableParam *)pData;

		//��������
		auto pTableParamEx = m_BattleManager.GetTableParam(pTableParam->dwBattleNum);
		if (pTableParamEx != NULL)
		{
			//���ñ���
			pTableParamEx->dwStartTime = pTableParam->dwStartTime;
			pTableParamEx->wFinishCount = pTableParam->wFinishCount;
			pTableParamEx->cbGameStatus = pTableParam->cbGameStatus;
		}

		auto pTableItemEx = m_BattleManager.GetTableItem(pTableParam->dwBattleNum);
		if (pTableItemEx != NULL && pTableItemEx->dwClubID != 0)
		{
			//�������
			CMD_BC_S_TableParam TableParam = {};

			//���ñ���								
			TableParam.dwClubID = pTableItemEx->dwClubID;
			TableParam.dwBattleNum = pTableItemEx->dwBattleNum;
			TableParam.dwStartTime = pTableParam->dwStartTime;
			TableParam.wFinishCount = pTableParam->wFinishCount;
			TableParam.cbGameStatus = pTableParam->cbGameStatus;

			//������Ϣ				
			SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_TABLE_PARAM, &TableParam, sizeof(TableParam));
		}

		return true;
	}	
	case SUB_BS_S_UPDATE_CONFIG:	//��������
	{
		//����У��
		ASSERT(sizeof(CMD_BS_S_UpdateConfig) == wDataSize);
		if (sizeof(CMD_BS_S_UpdateConfig) != wDataSize) return false;

		//��ȡ����
		auto pUpdateConfig = (CMD_BS_S_UpdateConfig *)pData;

		//�㲥��Ϣ
		auto pTableItemEx = m_BattleManager.GetTableItem(pUpdateConfig->dwBattleNum);
		if (pTableItemEx != NULL)
		{
			//��������
			pTableItemEx->BattleConfig.dwPayUserID = pUpdateConfig->dwPayUserID;
		}

		return true;
	}
	case SUB_BS_S_BATTLE_FINISH:	//Լս���
	{
		//����У��
		ASSERT(sizeof(CMD_BS_S_BattleFinish) == wDataSize);
		if (sizeof(CMD_BS_S_BattleFinish) != wDataSize) return false;

		//��ȡ����
		auto pBattleFinish = (CMD_BS_S_BattleFinish*)pData;

		//��������
		auto pTableItemEx = m_BattleManager.GetTableItem(pBattleFinish->dwBattleNum);
		if (pTableItemEx != NULL)
		{
			//��������				
			auto pTableParamEx = m_BattleManager.GetTableParam(pBattleFinish->dwBattleNum);			
			if (pTableParamEx != NULL)
			{
				//�㲥��Ϣ
				if (pTableItemEx->dwClubID != 0)
				{
					//У�����
					if (pTableParamEx && pTableParamEx->wFinishCount > 0)
					{
						//�������
						auto pIMBattleFinish = (CMD_BC_S_BattleFinish*)m_cbBuffer;

						//���ñ���
						pIMBattleFinish->wUserCount = 0;
						pIMBattleFinish->dwClubID = pTableItemEx->dwClubID;
						pIMBattleFinish->dwBattleNum = pTableItemEx->dwBattleNum;

						//��������							
						DWORD* pdwUserID = (DWORD*)(pIMBattleFinish + 1);
						for (int i = 0; i < CountArray(pTableParamEx->TableUserItem); i++)
						{
							if (pTableParamEx->TableUserItem[i].dwUserID == 0) continue;
							*pdwUserID++ = pTableParamEx->TableUserItem[i].dwUserID;
							if (++pIMBattleFinish->wUserCount == pTableParamEx->wUserCount) break;
						}

						//�㲥��Ϣ					
						WORD wPacketSize = sizeof(CMD_BC_S_BattleFinish) + sizeof(DWORD) * pIMBattleFinish->wUserCount;
						SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_BATTLE_FINISH, m_cbBuffer, wPacketSize);
					}
				}

				//����״̬
				if (pTableParamEx != NULL)
				{
					//���ñ���							
					pTableParamEx->dwStartTime = 0;
					pTableParamEx->wFinishCount = 0;
				}				
			}
		}

		return true;
	}
	case SUB_BS_S_TABLE_DISMISS:	//���ӽ�ɢ
	{
		//����У��
		ASSERT (sizeof(CMD_BS_S_TableDismiss) == wDataSize);
		if (sizeof(CMD_BS_S_TableDismiss) != wDataSize) return false;

		//��ȡ����
		auto pTableDismiss = (CMD_BS_S_TableDismiss *)pData;

		//��������
		auto pTableItemEx = m_BattleManager.GetTableItem(pTableDismiss->dwBattleNum);
		if (pTableItemEx != NULL)
		{
			//У����ֲ�
			if (pTableItemEx->dwClubID != 0)
			{
				//�������
				auto pIMTableDismiss = (CMD_BC_S_TableDismiss*)m_cbBuffer;

				//���ñ���								
				pIMTableDismiss->wUserCount = 0;
				pIMTableDismiss->dwClubID = pTableItemEx->dwClubID;
				pIMTableDismiss->dwBattleNum = pTableItemEx->dwBattleNum;

				//����״̬
				auto pTableParamEx = m_BattleManager.GetTableParam(pTableDismiss->dwBattleNum);
				if (pTableParamEx != NULL && pTableParamEx->wFinishCount >= 1)
				{
					DWORD* pdwUserID = (DWORD*)(pIMTableDismiss + 1);
					for (int i = 0; i < CountArray(pTableParamEx->TableUserItem); i++)
					{
						if (pTableParamEx->TableUserItem[i].dwUserID == 0) continue;
						*pdwUserID++ = pTableParamEx->TableUserItem[i].dwUserID;
						if (++pIMTableDismiss->wUserCount == pTableParamEx->wUserCount) break;
					}
				}

				//������Ϣ					
				WORD wPacketSize = sizeof(CMD_BC_S_TableDismiss) + sizeof(DWORD) * pIMTableDismiss->wUserCount;
				SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_TABLE_DISMISS, m_cbBuffer, wPacketSize);
			}

			//�ж�����
			if (pTableDismiss->wServerID == pTableItemEx->wServerID)
			{
				//�Ƴ�����
				m_BattleManager.RemoveTable(pTableDismiss->dwBattleNum);
				m_BattleManager.RemoveTableOwner(pTableDismiss->dwBattleNum);
			}
		}			
		return true;
	}
	case SUB_BS_S_DISMISS_TABLE:	//��ɢ���
	{
		//����У��
		ASSERT (sizeof(CMD_BS_S_DismissTable) >= wDataSize);
		if (sizeof(CMD_BS_S_DismissTable) < wDataSize) return false;

		//��ȡ����
		auto pDismissTable = (CMD_BS_S_DismissTable *)pData;

		//������Ϣ
		auto pSocketInfo = &pDismissTable->SocketInfo;

		//��ɢʧ��
		if (pDismissTable->cbResultCode != 0)
		{
			//����ʧ��
			SendOperateFailure(pSocketInfo->wGateID, pSocketInfo->dwTokenID, SUB_GB_C_DISMISS_TABLE, pDismissTable->cbResultCode, pDismissTable->szDescribeString);
		}
		else
		{
			//����ṹ
			CMD_GB_S_TableDismiss TableDismiss = {};
			
			//���ñ���				
			TableDismiss.dwBattleNum = pDismissTable->dwBattleNum;

			//��������
			SendDataToGate(pSocketInfo->wGateID, MDM_GB_BATTLE_SERVICE, SUB_GB_S_TABLE_DISMISS, &TableDismiss, sizeof(TableDismiss), &pSocketInfo->dwTokenID, 1);
		}		

		return true;
	}
	case SUB_BS_S_USER_SITDOWN:		//�û�����
	{
		//����У��
		ASSERT(sizeof(CMD_BS_S_UserSitdown) >= wDataSize);
		if (sizeof(CMD_BS_S_UserSitdown) < wDataSize) return false;

		//��ȡ����
		auto pUserSitDown = (CMD_BS_S_UserSitdown *)pData;

		//�㲥��Ϣ
		auto pTableItemEx = m_BattleManager.GetTableItem(pUserSitDown->dwBattleNum);
		if (pTableItemEx != NULL && pTableItemEx->dwClubID != 0)
		{
			//�������
			CMD_BC_S_UserSitdown UserSitdown = {};

			//���ñ���								
			UserSitdown.dwClubID = pTableItemEx->dwClubID;
			UserSitdown.dwBattleNum = pTableItemEx->dwBattleNum;
			UserSitdown.dwUserID = pUserSitDown->TableUserItem.dwUserID;
			CopyMemory(&UserSitdown.TableUserItem, &pUserSitDown->TableUserItem, sizeof(UserSitdown.TableUserItem));

			//������Ϣ					
			SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_USER_SITDOWN, &UserSitdown, sizeof(UserSitdown));
		}

		//��������
		auto pTableParamEx = m_BattleManager.GetTableParam(pUserSitDown->dwBattleNum);
		if (pTableParamEx != NULL)
		{
			//������Ϣ							
			pTableParamEx->wUserCount++;
			WORD wChairID = pUserSitDown->TableUserItem.wChairID;
			if (wChairID < CountArray(pTableParamEx->TableUserItem))
			{
				CopyMemory(&pTableParamEx->TableUserItem[wChairID], &pUserSitDown->TableUserItem, sizeof(pUserSitDown->TableUserItem));
			}
		}

		return true;
	}
	case SUB_BS_S_USER_STANDUP:		//�û�����
	{
		//����У��
		ASSERT(sizeof(CMD_BS_S_UserStandup) >= wDataSize);
		if (sizeof(CMD_BS_S_UserStandup) < wDataSize) return false;

		//��ȡ����
		auto pUserStandup = (CMD_BS_S_UserStandup *)pData;

		//�㲥��Ϣ
		auto pTableItemEx = m_BattleManager.GetTableItem(pUserStandup->dwBattleNum);
		if (pTableItemEx != NULL && pTableItemEx->dwClubID != 0)
		{
			//�������
			CMD_BC_S_UserStandup UserStandup = {};

			//���ñ���								
			UserStandup.dwClubID = pTableItemEx->dwClubID;
			UserStandup.dwUserID = pUserStandup->dwUserID;
			UserStandup.wChairID = pUserStandup->wChairID;
			UserStandup.dwBattleNum = pTableItemEx->dwBattleNum;

			//������Ϣ					
			SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_USER_STANDUP, &UserStandup, sizeof(UserStandup));
		}

		//����״̬
		auto pTableParamEx = m_BattleManager.GetTableParam(pUserStandup->dwBattleNum);
		if (pTableParamEx != NULL)
		{
			//��������
			if (pUserStandup->dwUserID != 0)
			{
				pTableParamEx->wUserCount--;
				if (pUserStandup->wChairID < CountArray(pTableParamEx->TableUserItem))
				{
					ZeroMemory(&pTableParamEx->TableUserItem[pUserStandup->wChairID], sizeof(pTableParamEx->TableUserItem[0]));
				}
			}
			else
			{
				pTableParamEx->wUserCount = 0;
				ZeroMemory(&pTableParamEx->TableUserItem, sizeof(pTableParamEx->TableUserItem));
			}
		}

		return true;
	}
	case SUB_BS_S_CLUB_SCORE:		//�������
	{
		//����У��
		ASSERT(sizeof(CMD_BS_S_ClubScore) == wDataSize);
		if (sizeof(CMD_BS_S_ClubScore) != wDataSize) return false;

		//��ȡ����
		auto pClubScore = (CMD_BS_S_ClubScore*)pData;

		//�㲥��Ϣ
		auto pTableItemEx = m_BattleManager.GetTableItem(pClubScore->dwBattleNum);
		if (pTableItemEx != NULL)
		{
			//�������
			CMD_BC_S_ClubScore ClubScore = {};

			//���ñ���
			ClubScore.dwUserID = pClubScore->dwUserID;
			ClubScore.lRevenue = pClubScore->lRevenue;
			ClubScore.dwClubID = pClubScore->dwClubID;
			ClubScore.lClubScore = pClubScore->lClubScore;

			//������Ϣ					
			SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_CLUB_SCORE, &ClubScore, sizeof(ClubScore));
		}		

		return true;
	}
	case SUB_BS_S_CLUB_WEALTH:		//����Ƹ�
	{
		//����У��
		ASSERT(sizeof(CMD_BS_S_ClubWealth) == wDataSize);
		if (sizeof(CMD_BS_S_ClubWealth) != wDataSize) return false;

		//��ȡ����
		auto pClubWealth = (CMD_BS_S_ClubWealth*)pData;

		//�㲥��Ϣ
		auto pTableItemEx = m_BattleManager.GetTableItem(pClubWealth->dwBattleNum);
		if (pTableItemEx != NULL)
		{
			//�������
			CMD_BC_S_ClubWealth ClubWealth = {};

			//���ñ���
			ClubWealth.dwClubID = pClubWealth->dwClubID;
			ClubWealth.dwPayUserID = pClubWealth->dwPayUserID;			
			ClubWealth.cbWealthMask = WEALTH_MASK_CARD;
			ClubWealth.lUserCard = pClubWealth->lUserCard;

			//������Ϣ					
			SendDataToClub(pTableItemEx->wClubServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_CLUB_WEALTH, &ClubWealth, sizeof(ClubWealth));
		}

		return true;
	}
	}

	return false;
}

//Լս����
bool CAttemperEngineSink::OnTCPNetWorkMainBCBattleService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ��Ϣ
	WORD wBindIndex = LOWORD(dwSocketID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);

	switch (wSubCmdID)
	{
	case SUB_BC_C_QUERY_OPTION:		//��ѯ����
	{
		//����У��
		ASSERT(wDataSize == sizeof(CMD_BC_C_QueryOption));
		if (wDataSize != sizeof(CMD_BC_C_QueryOption)) return true;

		//��ȡ����
		auto pQueryOption = (CMD_BC_C_QueryOption*)pData;

		//��������
		INT nStartIndex = 0;
		WORD wHeadSize = sizeof(pQueryOption->SocketInfo);

		//����������Ϣ
		CopyMemory(m_cbBuffer,&pQueryOption->SocketInfo,sizeof(pQueryOption->SocketInfo));

		//��������
		while (nStartIndex < m_BattleBaseOptionActive.GetCount())
		{			
			auto wPacketSize = ConstructBaseOption(nStartIndex, pQueryOption->wKindID, pQueryOption->cbEmployScene, &m_cbBuffer[wHeadSize], CountArray(m_cbBuffer)-wHeadSize);
			if (wPacketSize > 0)
			{
				//��������
				SendDataToClub(pBindParameter->wServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_BATTLE_BASE_OPTION, m_cbBuffer, wPacketSize+ wHeadSize);
			}
		}

		nStartIndex = 0;

		//��Ϸ����
		while (nStartIndex < m_BattleGameOptionActive.GetCount())
		{
			auto wPacketSize = ConstructGameOption(nStartIndex, pQueryOption->wKindID, pQueryOption->cbEmployScene, &m_cbBuffer[wHeadSize],CountArray(m_cbBuffer)-wHeadSize);
			if (wPacketSize > 0)
			{
				//��������
				SendDataToClub(pBindParameter->wServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_BATTLE_GAME_OPTION, m_cbBuffer, wPacketSize+wHeadSize);
			}
		}

		//��������
		SendDataToClub(pBindParameter->wServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_BATTLE_OPTION_FINISH, m_cbBuffer, wHeadSize);

		return true;
	}
	case SUB_BC_C_MODIFY_SCORE:		//�޸ķ���
	{
		//��ȡ����
		ASSERT(wDataSize == sizeof(CMD_BC_C_ModifyScore));
		if (wDataSize != sizeof(CMD_BC_C_ModifyScore)) return false;

		//��ȡ����
		auto pModifyScore = (CMD_BC_C_ModifyScore*)pData;

		//����ṹ
		CMD_BS_C_ModifyClubScore ModifyClubScore;
		ZeroMemory(&ModifyClubScore,sizeof(ModifyClubScore));

		//���ñ���
		ModifyClubScore.dwUserID = pModifyScore->dwUserID;
		ModifyClubScore.dwClubID = pModifyScore->dwClubID;
		ModifyClubScore.cbChangeType = pModifyScore->cbChangeType;
		ModifyClubScore.lChangeScore = pModifyScore->lChangeScore;
		StringCchCopy(ModifyClubScore.szClubName, CountArray(ModifyClubScore.szClubName), pModifyScore->szClubName);
		StringCchCopy(ModifyClubScore.szOperaterNickName, CountArray(ModifyClubScore.szOperaterNickName), pModifyScore->szOperaterNickName);

		//��������
		SendDataToGame(pModifyScore->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_MODIFY_CLUB_SCORE, &ModifyClubScore, sizeof(ModifyClubScore));

		return true;
	}	
	case SUB_BC_C_CLUB_DELETE:		//����ɾ��
	{
		//��ȡ����
		ASSERT (wDataSize==sizeof(CMD_BC_C_ClubDelete));
		if (wDataSize!=sizeof(CMD_BC_C_ClubDelete)) return false;

		//��ȡ����
		auto pDeleteClub=(CMD_BC_C_ClubDelete*)pData;

		//��ѯ����
		CTableItemArray TableItemArray;				
		CTableParamArray TableParamArray;	

		//��ȡ����				
		m_BattleManager.GetClubTableList(pDeleteClub->dwClubID,TableItemArray,TableParamArray);	

		//����ṹ
		CMD_BS_C_DismissTable DismissTable;
		ZeroMemory(&DismissTable,sizeof(DismissTable));

		for (INT_PTR i=0; i<TableItemArray.GetCount(); i++)
		{
			//��ȡ����
			tagBattleTableItemEx * pTableItemEx=TableItemArray[i];

			//���ñ���
			DismissTable.wTableID=pTableItemEx->wTableID;
			DismissTable.dwBattleNum=pTableItemEx->dwBattleNum;
			DismissTable.dwUserID=pTableItemEx->dwClubCreaterID;

			//��������				
			SendDataToGame(pTableItemEx->wServerID,MDM_BS_BATTLE_SERVICE,SUB_BS_C_DISMISS_TABLE,&DismissTable,sizeof(DismissTable));
		}

		return true;
	}
	case SUB_BC_C_CLUB_UPDATE:		//�������
	{
		//��ȡ����
		ASSERT(wDataSize >= sizeof(CMD_BC_C_ClubUpdate));
		if (wDataSize < sizeof(CMD_BC_C_ClubUpdate)) return false;

		//��ȡ����
		auto pUpdateClub = (CMD_BC_C_ClubUpdate*)pData;

		//���¹���
		if (pUpdateClub->cbUpdateKind == UPCLUB_KIND_RULES)
		{
			//��������
			auto pIMClubRules=(tagClubRules *)(pUpdateClub+1);

			//��ѯ����
			CTableItemArray TableItemArray;
			CTableParamArray TableParamArray;

			//��ȡ����				
			m_BattleManager.GetClubTableList(pUpdateClub->dwClubID, TableItemArray, TableParamArray);

			//����ṹ
			CMD_BS_C_UpdateClubInfo UpdateClubInfo;
			ZeroMemory(&UpdateClubInfo, sizeof(UpdateClubInfo));

			for (INT_PTR i = 0; i < TableItemArray.GetCount(); i++)
			{
				//��ȡ����
				tagBattleTableItemEx * pTableItemEx = TableItemArray[i];

				//�޸Ĺ���
				pTableItemEx->wSwitchOption = pIMClubRules->wSwitchOption;

				//������Ϣ
				UpdateClubInfo.wTableID = pTableItemEx->wTableID;
				UpdateClubInfo.dwClubID = pUpdateClub->dwClubID;
				UpdateClubInfo.dwBattleNum = pTableItemEx->dwBattleNum;

				//������Ϣ
				UpdateClubInfo.wSwitchOption = pTableItemEx->wSwitchOption;
				UpdateClubInfo.dwClubCreaterID = pTableItemEx->dwClubCreaterID;

				//��������				
				SendDataToGame(pTableItemEx->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_UPDATE_CLUB_INFO, &UpdateClubInfo, sizeof(UpdateClubInfo));
			}
		}

		return true;
	}
	case SUB_BC_C_CLUB_TRANSFER:	//����ת��
	{
		//��ȡ����
		ASSERT (wDataSize == sizeof(CMD_BC_C_ClubTransfer));
		if (wDataSize != sizeof(CMD_BC_C_ClubTransfer)) return false;

		//��ȡ����
		auto pTransferClub = (CMD_BC_C_ClubTransfer*)pData;

		//��ѯ����
		CTableItemArray TableItemArray;
		CTableParamArray TableParamArray;

		//��ȡ����				
		m_BattleManager.GetClubTableList(pTransferClub->dwClubID, TableItemArray, TableParamArray);

		//����ṹ
		CMD_BS_C_UpdateClubInfo UpdateClubInfo;
		ZeroMemory(&UpdateClubInfo, sizeof(UpdateClubInfo));

		for (INT_PTR i = 0; i < TableItemArray.GetCount(); i++)
		{
			//��ȡ����
			tagBattleTableItemEx * pTableItemEx = TableItemArray[i];
				
			//�޸Ĺ���
			pTableItemEx->dwClubCreaterID = pTransferClub->dwNewCreaterID;

			//������Ϣ
			UpdateClubInfo.wTableID = pTableItemEx->wTableID;
			UpdateClubInfo.dwClubID = pTransferClub->dwClubID;
			UpdateClubInfo.dwBattleNum = pTableItemEx->dwBattleNum;

			//������Ϣ
			UpdateClubInfo.wSwitchOption = pTableItemEx->wSwitchOption;
			UpdateClubInfo.dwClubCreaterID = pTransferClub->dwNewCreaterID;

			//��������				
			SendDataToGame(pTableItemEx->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_UPDATE_CLUB_INFO, &UpdateClubInfo, sizeof(UpdateClubInfo));
		}

		return true;
	}	
	case SUB_BC_C_EFFICACY_RESULT:	//��֤���
	{
		//����У��
		ASSERT (wDataSize==sizeof(CMD_BC_C_EfficacyResult));
		if (wDataSize!=sizeof(CMD_BC_C_EfficacyResult)) return true;

		//��ȡ����
		auto pEfficacyResult=(CMD_BC_C_EfficacyResult*)pData;

		//������Ϣ
		auto pSocketInfo=&pEfficacyResult->UserSocketInfo;

		//��ѯ����
		auto pTableItemEx=m_BattleManager.GetTableItem(pEfficacyResult->dwBattleNum);
		if (pTableItemEx==NULL || pTableItemEx->wTableID==INVALID_TABLE)
		{
			//����ʧ��
			SendOperateFailure(pSocketInfo->wGateID, pSocketInfo->dwTokenID, SUB_GB_C_QUERY_TABLE, BATTLE_CODE_QUERY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0001, TEXT("��Ǹ��ϵͳû�в�ѯ������������ӣ���ȷ�����Ӻ��ٴγ��ԣ�")));

			return true;
		}

		//��֤ʧ��
		if (pEfficacyResult->bResult==false)
		{
			//����ʧ��
			SendOperateFailure(pSocketInfo->wGateID, pSocketInfo->dwTokenID, SUB_GB_C_QUERY_TABLE, BATTLE_CODE_QUERY_FAILED, pEfficacyResult->szErrorMessage);

			return true;
		}

		//��ѯ����
		auto pTableParam = m_BattleManager.GetTableParam(pEfficacyResult->dwBattleNum);
		if (pTableParam == NULL )
		{
			//����ʧ��
			SendOperateFailure(pSocketInfo->wGateID, pSocketInfo->dwTokenID, SUB_GB_C_QUERY_TABLE, BATTLE_CODE_QUERY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0001, TEXT("��Ǹ��ϵͳû�в�ѯ������������ӣ���ȷ�����Ӻ��ٴγ��ԣ�")));

			return true;
		}

		//��������
		auto pBattleTableItem = (tagBattleTableItem*)m_cbBuffer;
		auto pBattleTableParam = (tagBattleTableParam*)(pBattleTableItem + 1);

		//��������
		ConstructTableItem(pTableItemEx, pBattleTableItem);

		//���Ӳ���
		pBattleTableParam->wUserCount = 0;
		pBattleTableParam->dwStartTime = pTableParam->dwStartTime;		
		pBattleTableParam->wFinishCount = pTableParam->wFinishCount;
		pBattleTableParam->cbGameStatus = pTableParam->cbGameStatus;
		pBattleTableParam->dwBattleNum = pBattleTableItem->dwBattleNum;

		//������Ϣ
		SendDataToGate(pSocketInfo->wGateID,MDM_GB_BATTLE_SERVICE, SUB_GB_S_SEARCH_RESULT,m_cbBuffer,sizeof(tagBattleTableItem)+sizeof(tagBattleTableParam),&pSocketInfo->dwTokenID,1);

		return true;
	}	
	case SUB_BC_C_QUERY_TABLE:		//��ѯ����
	{
		//����У��
		ASSERT(sizeof(CMD_BC_C_QueryTable) == wDataSize);
		if (sizeof(CMD_BC_C_QueryTable) != wDataSize) return false;

		//��ȡ����
		auto pQueryTable = (CMD_BC_C_QueryTable*)pData;

		//����ͬ����
		if (m_SynchroItemArray.GetCount() > 0) return true;

		//��ֵ����
		CTableItemArray TableItemArray;
		CTableParamArray TableParamArray;

		//��ȡ����				
		m_BattleManager.GetClubTableList(pQueryTable->dwClubID, TableItemArray, TableParamArray);

		//��������
		INT nStartIndex = 0;

		//�����б�
		while (nStartIndex < TableItemArray.GetCount())
		{
			auto wPacketSize = ConstructTableList(TableItemArray, TableParamArray, nStartIndex, m_cbBuffer, CountArray(m_cbBuffer));
			if (wPacketSize > 0)
			{
				//��������
				SendDataToClub(pBindParameter->wServiceID, MDM_BC_BATTLE_SERVICE, SUB_BC_S_TABLE_ITEM, m_cbBuffer, wPacketSize);
				wPacketSize = 0;
			}
		}
		
		return true;
	}
	case SUB_BC_C_CREATE_TABLE:		//��������
	{
		//����У��
		ASSERT (wDataSize == sizeof(CMD_BC_C_CreateTable));
		if (wDataSize != sizeof(CMD_BC_C_CreateTable)) return true;

		//��ȡ����
		auto pCreateTable = (CMD_BC_C_CreateTable*)pData;

		//������Ϣ
		auto pSocketInfo = &pCreateTable->SocketInfo;

		//����״̬
		if (m_bBattleService == false)
		{
			SendOperateFailure(pBindParameter->wServiceID, pCreateTable->dwClubID, *pSocketInfo, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_BATTLE_0002, TEXT("ϵͳά���У�Լս������ʱֹͣ��������ά����ɺ����ԣ�")));

			return true;
		}

		//��������
		tagBattleConfigEx BattleConfig;
		ZeroMemory(&BattleConfig, sizeof(BattleConfig));

		//��ȡ����
		auto pBattleConfig = &pCreateTable->BattleConfig;

		//У������
		if (VerifyBattleConfig(pBattleConfig, BattleConfig) == false)
		{
			SendOperateFailure(pBindParameter->wServiceID, pCreateTable->dwClubID, *pSocketInfo, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_BATTLE_0003, TEXT("���Ӵ���ʧ�ܣ�")));

			return true;
		}			

		//�ж�����
		auto wTableCount = m_BattleManager.GetTableCountByClubID(pCreateTable->dwClubID);
		if (VerifyTableCount(pCreateTable->dwClubID, wTableCount+1)==false)
		{
			SendOperateFailure(pBindParameter->wServiceID, pCreateTable->dwClubID, *pSocketInfo, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_BATTLE_0004, TEXT("��Ǹ����������������Ŀ�Ѿ��ﵽ���ޣ��������ʣ�����ϵ�ͷ���")));

			return true;
		}

		//��������
		auto pTableItemEx = m_BattleManager.CreateTable(pCreateTable->dwUserID, pCreateTable->dwClubID, BattleConfig);
		if (pTableItemEx == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, pCreateTable->dwClubID, *pSocketInfo, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessageV(MIM_BATTLE_0005, TEXT("��Ǹ�����Ӵ���ʧ��[%d]�����Ժ��ٴγ��ԣ�"), 2));

			return true;
		}

		//��������
		auto pTableOwner = m_BattleManager.CreateTableOwner(pTableItemEx->dwBattleNum);
		if (pTableOwner == NULL)
		{
			//�Ƴ�����
			m_BattleManager.RemoveTable(pTableItemEx->dwBattleNum);

			SendOperateFailure(pBindParameter->wServiceID, pCreateTable->dwClubID, *pSocketInfo, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessageV(MIM_BATTLE_0005, TEXT("��Ǹ�����Ӵ���ʧ��[%d]�����Ժ��ٴγ��ԣ�"), 3));

			return true;
		}

		//������Ϣ			
		pTableItemEx->wClubServiceID = pBindParameter->wServiceID;
		pTableItemEx->wSwitchOption = pCreateTable->wSwitchOption;
		pTableItemEx->dwClubCreaterID = pCreateTable->dwClubCreaterID;		

		//�淨��Ϣ
		pTableItemEx->dwConfigID = pCreateTable->dwConfigID;

		//������Ϣ			
		pTableOwner->dwUserID = pCreateTable->dwUserID;
			
		//������Ϣ
		CopyMemory(&pTableOwner->SocketInfo, pSocketInfo, sizeof(pTableOwner->SocketInfo));

		//����ṹ
		CMD_BS_C_CreateTable CreateTable;
		ZeroMemory(&CreateTable, sizeof(CreateTable));

		//�û���Ϣ			
		CreateTable.dwUserID = pCreateTable->dwUserID;
		StringCchCopy(CreateTable.szPassword, CountArray(CreateTable.szPassword), pTableItemEx->szPassword);

		//������Ϣ			
		CreateTable.dwClubID = pCreateTable->dwClubID;
		CreateTable.wSwitchOption = pCreateTable->wSwitchOption;
		CreateTable.wClubServiceID = pBindParameter->wServiceID;
		CreateTable.dwClubCreaterID = pCreateTable->dwClubCreaterID;			

		//������Ϣ
		CreateTable.dwConfigID = pTableItemEx->dwConfigID;
		CreateTable.dwBattleNum = pTableItemEx->dwBattleNum;
		CopyMemory(&CreateTable.BattleConfig, &BattleConfig, sizeof(CreateTable.BattleConfig));

		//������Ϣ
		SendDataToGame(pTableItemEx->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_CREATE_TABLE, &CreateTable, sizeof(CreateTable));

		return true;
	}
	case SUB_BC_C_MODIFY_TABLE:		//�޸�����
	{
		//����У��
		ASSERT (sizeof(CMD_BC_C_ModifyTable) >= wDataSize);
		if (sizeof(CMD_BC_C_ModifyTable) < wDataSize) return false;

		//��ȡ����
		auto pModifyTable = (CMD_BC_C_ModifyTable*)pData;

		//������Ϣ
		auto pSocketInfo = &pModifyTable->SocketInfo;

		//����״̬
		if (m_bBattleService == false)
		{
			SendOperateFailure(pBindParameter->wServiceID, pModifyTable->dwClubID, *pSocketInfo, BATTLE_CODE_STOP_SERVICE, CWHService::FormatMessage(MIM_BATTLE_0002, TEXT("ϵͳά���У�Լս������ʱֹͣ��������ά����ɺ����ԣ�")));

			return true;
		}

		//��������
		auto pTableItemEx = m_BattleManager.GetTableItem(pModifyTable->dwBattleNum);
		if (pTableItemEx == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, pModifyTable->dwClubID, *pSocketInfo, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0009, TEXT("��Ǹ��ϵͳδ��ѯ����Ҫ�޸ĵ����ӣ���ˢ�º��ٴγ��ԣ�")));

			return true;
		}

		//У���û�			
		if (pTableItemEx->dwClubCreaterID != pModifyTable->dwUserID)
		{
			SendOperateFailure(pBindParameter->wServiceID, pModifyTable->dwClubID, *pSocketInfo, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0010, TEXT("��Ǹ�������Ǹò�����ϰ壬û��Ȩ���޸��������ã�")));

			return true;
		}

		//��������
		tagBattleConfigEx BattleConfig;
		ZeroMemory(&BattleConfig, sizeof(BattleConfig));

		//��ȡ����
		auto pBattleConfig = &pModifyTable->BattleConfig;

		//У������
		if (VerifyBattleConfig(pBattleConfig, BattleConfig) == false)
		{
			SendOperateFailure(pBindParameter->wServiceID, pModifyTable->dwClubID, *pSocketInfo, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_BATTLE_0011, TEXT("�����޸�ʧ�ܣ�")));

			return true;
		}

		//����ṹ
		CMD_BS_C_ModifyTable ModifyTable;
		ZeroMemory(&ModifyTable, sizeof(ModifyTable));

		//�û���Ϣ			
		ModifyTable.wTableID = pTableItemEx->wTableID;
		ModifyTable.dwUserID = pModifyTable->dwUserID;
		ModifyTable.dwBattleNum = pModifyTable->dwBattleNum;

		//Լս����
		CopyMemory(&ModifyTable.BattleConfig, &BattleConfig, sizeof(BattleConfig));

		//������Ϣ
		CopyMemory(&ModifyTable.SocketInfo, pSocketInfo,sizeof(ModifyTable.SocketInfo));

		//������Ϣ
		SendDataToGame(pTableItemEx->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_MODIFY_TABLE, &ModifyTable, sizeof(ModifyTable));

		return true;
	}
	case SUB_BC_C_DISMISS_TABLE:	//��ɢ����
	{
		//����У��
		ASSERT (sizeof(CMD_BC_C_DismissTable) == wDataSize);
		if (sizeof(CMD_BC_C_DismissTable) != wDataSize) return false;

		//��ȡ����
		auto pDismissTable = (CMD_BC_C_DismissTable*)pData;

		//��������
		auto pTableItemEx = m_BattleManager.GetTableItem(pDismissTable->dwBattleNum);
		if (pTableItemEx == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, 0, pDismissTable->SocketInfo, BATTLE_CODE_DISMISS_FAILED, CWHService::FormatMessage(MIM_BATTLE_0012, TEXT("��Ǹ��ϵͳδ��ѯ����Ҫ��ɢ�����ӣ���ˢ�º��ٴγ��ԣ�")));

			return true;
		}

		//����ṹ
		CMD_BS_C_DismissTable DismissTable;
		ZeroMemory(&DismissTable, sizeof(DismissTable));

		//������Ϣ
		CopyMemory(&DismissTable.SocketInfo, &pDismissTable->SocketInfo,sizeof(DismissTable.SocketInfo));

		//������Ϣ
		DismissTable.wTableID = pTableItemEx->wTableID;
		DismissTable.dwUserID = pDismissTable->dwUserID;
		DismissTable.dwBattleNum = pDismissTable->dwBattleNum;		

		//��������
		SendDataToGame(pTableItemEx->wServerID, MDM_BS_BATTLE_SERVICE, SUB_BS_C_DISMISS_TABLE, &DismissTable, sizeof(DismissTable));

		return true;
	}		
	}

	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT(wDataSize <= sizeof(DBO_GR_OperateFailure));
	if (wDataSize > sizeof(DBO_GR_OperateFailure)) return false;

	//��ȡ����
	auto pOperateFailure = (DBO_GR_OperateFailure *)pData;

	//��ȡ����
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);

	//����ʧ��
	SendOperateFailure(pBindParameter->wServiceID, dwTokenID, 0, pOperateFailure->lErrorCode, pOperateFailure->szErrorMessage);

	return true;
}

//����ѡ��
bool CAttemperEngineSink::OnDBBattleBaseOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��ȡ����
	ASSERT (wDataSize%sizeof(tagBattleBaseOptionEx)==0);
	if (wDataSize%sizeof(tagBattleBaseOptionEx)!=0) return false;

	//��������
	WORD wOptionCount=wDataSize/sizeof(tagBattleBaseOptionEx);
	auto pBattleBaseOption =(tagBattleBaseOptionEx *)pData;

	//��������
	for (int i=0; i<wOptionCount; i++)
	{
		m_BattleBaseOptionBuffer.Add(*pBattleBaseOption++);
	}	

	return true;
}

//ȫ��ѡ��
bool CAttemperEngineSink::OnDBBattleGlobalOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��ȡ����
	ASSERT (wDataSize == sizeof(tagBattleGlobalOption));
	if (wDataSize != sizeof(tagBattleGlobalOption)) return false;

	//��������
	auto pGlobalOption = (tagBattleGlobalOption *)pData;

	//��������
	CopyMemory(&m_GlobalOption, pGlobalOption, sizeof(m_GlobalOption));

	return true;
}

//�Զ�ѡ��
bool CAttemperEngineSink::OnDBBattleGameOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��ȡ����
	ASSERT(wDataSize % sizeof(tagBattleGameOptionEx) == 0);
	if (wDataSize % sizeof(tagBattleGameOptionEx) != 0) return false;

	//��������
	WORD wOptionCount = wDataSize / sizeof(tagBattleGameOptionEx);
	auto pBattleGameOption = (tagBattleGameOptionEx *)pData;

	//��������
	for (int i = 0; i < wOptionCount; i++)
	{
		m_BattleGameOptionBuffer.Add(*pBattleGameOption++);
	}

	return true;
}

//ѡ�����
bool CAttemperEngineSink::OnDBBattleOptionFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��������
	m_BattleBaseOptionActive.RemoveAll();
	m_BattleBaseOptionActive.Append(m_BattleBaseOptionBuffer);
	m_BattleBaseOptionBuffer.RemoveAll();

	//��������
	m_BattleGameOptionActive.RemoveAll();
	m_BattleGameOptionActive.Append(m_BattleGameOptionBuffer);
	m_BattleGameOptionBuffer.RemoveAll();

	//�¼�֪ͨ
	CP_ControlResult ControlResult;
	ControlResult.cbSuccess=ER_SUCCESS;
	SendUIControlPacket(UI_LOAD_OPTION_RESULT,&ControlResult,sizeof(ControlResult));
	
	return true;
}
//////////////////////////////////////////////////////////////////////////
