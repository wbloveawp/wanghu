#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_COLLECT_ONLINE_INFO		1									//ͳ������
#define IDI_UPLOAD_GATE_LIST		2									//�ϴ�����	
#define IDI_UPDATE_STOCK_INFO		3									//���¿��	
#define IDI_UPDATE_SERVER_LIST		4									//�����б�
#define IDI_UPDATE_RANKING_LIST		5									//��������


//��������
#define MAX_TABLE_COUNT				32									//�������	

//////////////////////////////////////////////////////////////////////////
//ƽ̨����
struct tagPlatformUserCount
{
	DWORD							dwGlobalCount;						//�û�����
	DWORD							dwPlayerCount;						//�������
	DWORD							dwAndroidCount;						//��������
};

//////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����
	m_bWillConcludeService=false;
	m_wCollectItem=INVALID_WORD;

	//���ñ���
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;

	//�������
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
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
	m_pBindParameter=new tagBindParameter[m_pInitParameter->m_wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pInitParameter->m_wMaxConnect);

	//����ʱ��
	ASSERT(m_pITimerEngine != NULL);
	m_pITimerEngine->SetTimer(IDI_UPDATE_SERVER_LIST, 10 * 1000L, TIMES_INFINITY, 0);

	//����ʱ��
	ASSERT(m_pITimerEngine != NULL);
	m_pITimerEngine->SetTimer(IDI_UPDATE_RANKING_LIST, 60 * 1000L, TIMES_INFINITY, 0);		

	//����ʱ��
	ASSERT(m_pITimerEngine != NULL);
	m_pITimerEngine->SetTimer(IDI_UPLOAD_GATE_LIST, m_pInitParameter->m_wUploadTime * 1000L, TIMES_INFINITY, 0);

	//����ʱ��
	ASSERT(m_pITimerEngine!=NULL);
	m_pITimerEngine->SetTimer(IDI_COLLECT_ONLINE_INFO,m_pInitParameter->m_wCollectTime * 1000L,TIMES_INFINITY,0);

	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//״̬����
	m_bWillConcludeService=false;
	m_wCollectItem=INVALID_WORD;
	m_WaitCollectItemArray.RemoveAll();

	//���ñ���
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;

	//ɾ������
	SafeDeleteArray(m_pBindParameter);

	//�������	
	m_GlobalInfoManager.ResetData();

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	switch (dwIdentifier)
	{
	case CT_LOAD_GLOBAL_CONFIG:		//��������
	{
		//��������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_LOAD_CONFIG, 0, 0, 0, NULL, 0);

		return true;
	}
	case CT_SERVICE_WILLSTOP:		//ֹͣ����
	{
		//״̬�ж�
		if (m_bWillConcludeService) return true;

		//����״̬
		m_bWillConcludeService = true;

		//�¼�֪ͨ
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess = ER_SUCCESS;
		SendUIControlPacket(UI_WILLSTOP_RESULT, &ControlResult, sizeof(ControlResult));			

		return true;
	}
	case CT_DEBUG_SERVICE:			//���Է���
	{
		CMD_CS_C_ModifyStockItem ModifyStockItem;
		ModifyStockItem.wStockID = 1;
		
		OnTCPNetworkMasterService(SUB_CS_C_MODIFY_STOCK_ITEM, &ModifyStockItem, sizeof(ModifyStockItem), 0);


		CMD_CS_C_AdjustStockScore AdjustStockScore;
		AdjustStockScore.wStockID = 4;
		AdjustStockScore.lChangeScore = 1000;		

		OnTCPNetworkMasterService(SUB_CS_C_ADJUST_STOCK_SCORE,&AdjustStockScore,sizeof(AdjustStockScore),0);


		CMD_CS_C_UpdateSPlayerControl UpdateSPlayerControl = {};		

		OnTCPNetworkMasterService(SUB_CS_C_UPDATE_SPLAYER_CONTROL, &UpdateSPlayerControl, sizeof(UpdateSPlayerControl), 0);

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
	case IDI_COLLECT_ONLINE_INFO:	//ͳ������
	{
		//��������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_ONLINE_COUNT_INFO, 0, 0, 0, NULL, 0);

		return true;
	}	
	case IDI_UPLOAD_GATE_LIST:		//�ϴ�����
	{
		//�����б�
		if (m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_GATE) > 0)
		{
			UpdateGateList();

			return true;
		}

		return true;
	}
	case IDI_UPDATE_SERVER_LIST:		//�ϱ������б�
	{
		//����ṹ
		DBR_GP_UpdateServerList UpdateServerList;
		ZeroMemory(&UpdateServerList, sizeof(UpdateServerList));

		//��������
		UpdateServerList.cbUpdateKind = UPDATE_KIND_MODIFY;

		//��������
		CStringW strServerList, strServerItem;

		//ö�ٶ���
		POSITION Position = NULL;
		CGlobalServerInfo * pGlobalServerInfo = NULL;

		//��������	
		do
		{
			pGlobalServerInfo = (CGlobalServerInfo*)m_GlobalInfoManager.EnumServiceInfo(SERVICE_MODULE_GAME, Position);
			if (pGlobalServerInfo == NULL) break;

			//��ʽ��
			FormatServerItem(pGlobalServerInfo, strServerItem);
			
			//�����ж�
			if (strServerList.GetLength() + strServerItem.GetLength() >= CountArray(UpdateServerList.szServerList))
			{
				strServerList.Append(L"]");

				//������Ϣ
				StringCchCopy(UpdateServerList.szServerList, CountArray(UpdateServerList.szServerList), strServerList.GetString());

				//��������
				WORD wDataSize = CountStringBufferW(UpdateServerList.szServerList);
				WORD wHeadSize = sizeof(UpdateServerList) - sizeof(UpdateServerList.szServerList);
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_SERVER_LIST, 0L, 0L, 0L, &UpdateServerList, wHeadSize + wDataSize);

				//�������
				strServerList.Empty();
			}

			//ƴ������
			strServerList.Append(strServerList.IsEmpty() ? L"[" : L",");
			strServerList.Append(strServerItem.GetString());					

		} while (Position!=NULL);

		//�����б�
		if (strServerList.IsEmpty() == false)
		{
			strServerList.Append(L"]");

			//������Ϣ
			StringCchCopy(UpdateServerList.szServerList, CountArray(UpdateServerList.szServerList), strServerList.GetString());

			//��������
			WORD wDataSize = CountStringBufferW(UpdateServerList.szServerList);
			WORD wHeadSize = sizeof(UpdateServerList) - sizeof(UpdateServerList.szServerList);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_SERVER_LIST, 0L, 0L, 0L, &UpdateServerList, wHeadSize + wDataSize);
		}

		return true;
	}	
	case IDI_UPDATE_RANKING_LIST:		//���°�
	{
		//��������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_RANKING_LIST, 0L, 0L, 0L, NULL, 0);

		return true;
	}
	}

	return false;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//��ȡ����
	ASSERT(LOWORD(dwSocketID)<m_pInitParameter->m_wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pInitParameter->m_wMaxConnect) return false;

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
	switch (Command.wMainCmdID)
	{
	case MDM_CS_REGISTER:			//����ע��
	{
		return OnTCPNetworkMainRegister(Command.wSubCmdID,pData,wDataSize,dwSocketID);
	}
	case MDM_CS_COMMON_SERVICE:		//������Ϣ
	{
		return OnTCPNetworkMainCommonService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
	}
	case MDM_CS_USER_COLLECT:		//�û�����
	{
		return OnTCPNetworkMainUserCollect(Command.wSubCmdID,pData,wDataSize,dwSocketID);
	}
	case MDM_CS_QUERY_SERVICE:		//��Ϣ��ѯ
	{
		return OnTCPNetworkMainQueryService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
	}
	case MDM_CS_BROADCAST:			//�㲥��Ϣ
	{
		return OnTCPNetworkMainBroadcast(Command.wSubCmdID,pData,wDataSize,dwSocketID);
	}
	case MDM_CS_MASTER_SERVICE:		//��̨����
	{
		return OnTCPNetworkMasterService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
	}
	case MDM_CS_MANAGER_SERVICE:	//�������
	{
		return OnTCPNetWorkMainManagerService(Command.wSubCmdID,pData,wDataSize,dwSocketID);
	}
	case MDM_CS_FORWARD_SERVICE:	//ת������
	{
		return OnTCPNetworkMainForwardService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
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

	//��Ϸ����
	if (pBindParameter->wServiceModule == SERVICE_MODULE_GAME)	
	{
		//���ܴ���
		if (wBindIndex==m_wCollectItem)
		{
			//���ñ���
			m_wCollectItem=INVALID_WORD;

			//�����л�
			if (m_WaitCollectItemArray.GetCount()>0)
			{
				//��ȡ����
				INT_PTR nWaitCount=m_WaitCollectItemArray.GetCount();
				m_wCollectItem=m_WaitCollectItemArray[nWaitCount-1];

				//ɾ������
				m_WaitCollectItemArray.RemoveAt(nWaitCount-1);

				//������Ϣ
				DWORD dwSocketID=(m_pBindParameter+m_wCollectItem)->dwSocketID;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_USER,NULL,0);
			}
		}
		else
		{
			//ɾ���ȴ�
			for (INT_PTR i=0;i<m_WaitCollectItemArray.GetCount();i++)
			{
				if (wBindIndex==m_WaitCollectItemArray[i])
				{
					m_WaitCollectItemArray.RemoveAt(i);
					break;
				}
			}
		}
	}
	
	//ע������
	if (pBindParameter->wServiceModule != SERVICE_MODULE_NONE)
	{
		//���ҷ���
		CGlobalServiceInfo * pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(pBindParameter->wServiceModule, pBindParameter->wServiceID);		
		if (pGlobalServiceInfo != NULL)
		{
			//��������
			CMD_CS_S_ServiceRemove ServiceRemove;
			ZeroMemory(&ServiceRemove, sizeof(ServiceRemove));

			//����ṹ
			ServiceRemove.wServiceID = pBindParameter->wServiceID;			
			ServiceRemove.wServiceModule = pBindParameter->wServiceModule;
			ServiceRemove.wServicePort = pGlobalServiceInfo->m_ServiceItem.wServicePort;

			//��������
			if (pBindParameter->wServiceModule == SERVICE_MODULE_GAME)
			{
				m_pITCPNetworkEngine->SendDataBatch(MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVER_REMOVE, &ServiceRemove, sizeof(ServiceRemove), 0L);
			}
			else
			{
				m_pITCPNetworkEngine->SendDataBatch(MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVICE_REMOVE, &ServiceRemove, sizeof(ServiceRemove), 0L);
			}			
		}

		//��Ϸ����
		if (pBindParameter->wServiceModule == SERVICE_MODULE_GAME)
		{
			//ɾ������
			auto pGlobalServerInfo = (CGlobalServerInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GAME, pBindParameter->wServiceID);
			if (pGlobalServerInfo != NULL)
			{
				UpdateServerItem(pGlobalServerInfo, UPDATE_KIND_DELETE);
			}

			//ɾ������
			m_GlobalInfoManager.DeleteServerInfo(pBindParameter->wServiceID);
		}

		//��¼����
		else if (pBindParameter->wServiceModule == SERVICE_MODULE_LOGON)
		{
			//ɾ����¼
			m_GlobalInfoManager.DeleteLogonInfo(pBindParameter->wServiceID);
		}
		else
		{
			m_GlobalInfoManager.DeleteServiceInfo(pBindParameter->wServiceModule, pBindParameter->wServiceID);
		}
	}
		
	//�����Ϣ
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return true;
}

//���ݿ��¼�
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GP_UNLOCK_PLAYER:		//�������
	{
		return OnDBUnlockPlayer(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GP_CONFIG_FINISH:		//�������
	{
		return OnDBConfigFinish(dwContextID, dwTokenID, pData, wDataSize);
	}	
	case DBO_GP_UPDATA_RANKING_LIST: //���°�
	{
		return OnDBUpdateRankingList(dwContextID, dwTokenID, pData, wDataSize);
	}
	}

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason)
{
	return false;
}

//�����¼�
bool CAttemperEngineSink::OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode)
{
	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	//�������
	ASSERT(FALSE);

	return true;
}

//ע�����
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_REGISTER_SERVICE:	//ע�����
	{
		//У������
		ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterService));
		if (wDataSize != sizeof(CMD_CS_C_RegisterService)) return false;

		//��ȡ����
		CMD_CS_C_RegisterService * pRegisterService = (CMD_CS_C_RegisterService *)pData;

		//��������
		CMD_CS_S_RegisterFailure RegisterFailure;
		ZeroMemory(&RegisterFailure, sizeof(RegisterFailure));

		//��ѯ����
		auto pServiceAttrib = CServiceAttribManager::SearchServiceAttrib(pRegisterService->wServiceModule);
		if (pServiceAttrib == NULL)
		{
			//���ñ���
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), L"ϵͳδ��ѯ��ģ���ʶΪ[%d]�ķ�����Ϣ��ע��ʧ��", pRegisterService->wServiceModule);
		}

		//У����Ŀ
		if (RegisterFailure.lErrorCode == 0 && m_GlobalInfoManager.GetServiceItemCount(pRegisterService->wServiceModule) >= pServiceAttrib->dwMaxInstanceNum)
		{
			//���ñ���
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), L"ϵͳ����ע���[%s]ʵ����Ŀ�Ѵﵽ%d����ע��ʧ�ܣ�", pServiceAttrib->szServiceName, pServiceAttrib->dwMaxInstanceNum);
		}

		//���ҷ���
		if (RegisterFailure.lErrorCode == 0 && m_GlobalInfoManager.SearchServiceInfo(pRegisterService->wServiceModule, pRegisterService->wServiceID) != NULL)
		{
			//���ñ���
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), TEXT("ϵͳ���Ѵ�����ͬ��ʶ��[%s]��ע��ʧ��"), pServiceAttrib->szServiceName);
		}

		//ע��ʧ��
		if (RegisterFailure.lErrorCode != 0)
		{
			//������Ϣ
			WORD wStringSize = CountStringBuffer(RegisterFailure.szDescribeString);
			WORD wSendSize = sizeof(RegisterFailure) - sizeof(RegisterFailure.szDescribeString) + wStringSize;
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_FAILURE, &RegisterFailure, wSendSize, NULL, 0);

			//�ж�����
			m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

			return true;
		}

		//���ð�
		WORD wBindIndex = LOWORD(dwSocketID);
		(m_pBindParameter + wBindIndex)->wServiceID = pRegisterService->wServiceID;
		(m_pBindParameter + wBindIndex)->wServiceModule = pRegisterService->wServiceModule;

		//��������
		tagServiceItem ServiceItem;
		ZeroMemory(&ServiceItem, sizeof(ServiceItem));

		//��������
		ServiceItem.wServiceID = pRegisterService->wServiceID;
		ServiceItem.wServiceModule = pRegisterService->wServiceModule;
		ServiceItem.wServicePort = pRegisterService->wServicePort;
		ServiceItem.wMaxConnect= pRegisterService->wMaxConnect;
		ServiceItem.dwServiceAddr = pRegisterService->dwServiceAddr;			
		ServiceItem.wServiceKindID = pRegisterService->wServiceKindID;
		StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), pServiceAttrib->szServiceName);
		StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain), pRegisterService->szServiceDomain);

		//ע�����
		m_GlobalInfoManager.ActiveServiceInfo(dwSocketID, ServiceItem);

		//Ⱥ������
		m_pITCPNetworkEngine->SendDataBatch(MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVICE_INSERT, &ServiceItem, sizeof(ServiceItem), 0L);

		//���ط���
		if (pRegisterService->wServiceModule == SERVICE_MODULE_GATE)
		{
			//���ͷ�������
			SendServiceAttrib(dwSocketID);

			//���Ҵ���
			CGlobalAgentInfo* pGlobalAgentItem = (CGlobalAgentInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_AGENT, ServiceItem.wServiceID);
			if (pGlobalAgentItem != NULL && pGlobalAgentItem->m_AgentItem.wAgentKind == AGENT_KIND_GATE)
			{
				//����ṹ
				if (pGlobalAgentItem->GetServicePortCount() == 0)
				{
					tagPortPair PortPair;
					ZeroMemory(&PortPair, sizeof(PortPair));

					//�˿�ӳ��
					PortPair.wForwardPort = ServiceItem.wServicePort;
					PortPair.wServicePort = ServiceItem.wServicePort;

					//���ӳ��
					pGlobalAgentItem->InsertServicePort(&PortPair);
				}

				//ת������
				tagForwardItem ForwardItem;
				ZeroMemory(&ForwardItem, sizeof(ForwardItem));

				//���ñ�ʶ
				ForwardItem.wServiceID = pGlobalAgentItem->GetServiceID();

				//������Ϣ
				SendDataToService(SERVICE_MODULE_AGENT, ForwardItem, MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVICE_INSERT, &ServiceItem, sizeof(ServiceItem));
			}
		}	

		//���ͷ���
		PushServiceListItem(pRegisterService->wServiceMask, dwSocketID, pRegisterService->wServiceModule == SERVICE_MODULE_GATE);

		//������־
		if(pRegisterService->wServiceModule != SERVICE_MODULE_LOGGER)
			PushServiceListItem(SERVICE_MASK_LOGGER, dwSocketID, false);

		//��¼����
		if (pRegisterService->wServiceModule == SERVICE_MODULE_LOGON)
		{
			SendGameJackpot(dwSocketID);
		}

		//ע��ɹ�
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_SUCCESS, NULL, 0);

		//Ⱥ������
		m_pITCPNetworkEngine->AllowBatchSend(dwSocketID, true, 0L);

		//����֪ͨ
		if (pRegisterService->wServiceModule == SERVICE_MODULE_GAME || pRegisterService->wServiceModule == SERVICE_MODULE_LOGON || pRegisterService->wServiceModule == SERVICE_MODULE_MESSAGE)
		{
			if (m_wCollectItem == INVALID_WORD)
			{
				m_wCollectItem = wBindIndex;
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_USER_COLLECT, SUB_CS_S_COLLECT_USER, NULL, 0);
			}
			else m_WaitCollectItemArray.Add(wBindIndex);
		}	

		return true;
	}	
	case SUB_CS_C_REGISTER_AGENT:	//ע�����
	{
		//Ч������
		ASSERT (wDataSize >= sizeof(CMD_CS_C_RegisterAgent));
		if (wDataSize < sizeof(CMD_CS_C_RegisterAgent)) return false;

		//��Ϣ����
		CMD_CS_C_RegisterAgent * pRegisterAgent = (CMD_CS_C_RegisterAgent *)pData;

		//��������
		CMD_CS_S_RegisterFailure RegisterFailure;
		ZeroMemory(&RegisterFailure, sizeof(RegisterFailure));

		//��ѯ����
		auto pServiceAttrib = CServiceAttribManager::SearchServiceAttrib(SERVICE_MODULE_AGENT);
		if (pServiceAttrib == NULL)
		{
			//���ñ���
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), L"ϵͳδ��ѯ��ģ���ʶΪ[%d]�ķ�����Ϣ��ע��ʧ��", SERVICE_MODULE_AGENT);
		}

		//У����Ŀ
		if (RegisterFailure.lErrorCode == 0 && m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_AGENT) >= pServiceAttrib->dwMaxInstanceNum)
		{
			//���ñ���
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), L"ϵͳ��[%s]ע���ʵ����Ŀ�Ѵﵽ%d����ע��ʧ�ܣ�", pServiceAttrib->szServiceName, pServiceAttrib->dwMaxInstanceNum);
		}

		//���ҷ���
		if (RegisterFailure.lErrorCode == 0 && m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_AGENT, pRegisterAgent->wAgentID) != NULL)
		{
			//���ñ���
			RegisterFailure.lErrorCode = -1;
			StringCchCopy(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), TEXT("ϵͳ���Ѵ�����ͬ��ʶ�Ĵ��������Ϸ����ע��ʧ��"));
		}

		//ע��ʧ��
		if (RegisterFailure.lErrorCode != 0)
		{
			//������Ϣ
			WORD wStringSize = CountStringBuffer(RegisterFailure.szDescribeString);
			WORD wSendSize = sizeof(RegisterFailure) - sizeof(RegisterFailure.szDescribeString) + wStringSize;
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_FAILURE, &RegisterFailure, wSendSize, NULL, 0);

			//�ж�����
			m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

			return true;
		}				

		//���ð�
		WORD wBindIndex = LOWORD(dwSocketID);
		(m_pBindParameter + wBindIndex)->wServiceModule = SERVICE_MODULE_AGENT;
		(m_pBindParameter + wBindIndex)->wServiceID = pRegisterAgent->wAgentID;

		//��������
		tagServiceItem ServiceItem;
		ZeroMemory(&ServiceItem, sizeof(ServiceItem));

		//��������
		ServiceItem.wServiceID = pRegisterAgent->wAgentID;
		ServiceItem.wServiceModule = SERVICE_MODULE_AGENT;
		ServiceItem.wMaxConnect = pRegisterAgent->wMaxConnect;
		ServiceItem.dwServiceAddr = pRegisterAgent->dwServiceAddr;			
		StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), TEXT("���������"));
		StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain), pRegisterAgent->szServiceDomain);

		//��������
		tagAgentItem AgentItem;
		ZeroMemory(&AgentItem, sizeof(AgentItem));

		//��������
		AgentItem.wAgentKind= pRegisterAgent->wAgentKind;

		//ע�����
		if (m_GlobalInfoManager.ActiveAgentInfo(dwSocketID, ServiceItem, AgentItem) == false)
		{
			return false;
		}

		//��ѯ����
		CGlobalAgentInfo *pGlobalAgentItem = (CGlobalAgentInfo *)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_AGENT,ServiceItem.wServiceID);
		if (pGlobalAgentItem != NULL)
		{
			//��ȡ�˿�
			tagPortPair * pPortPair=(tagPortPair *)(pRegisterAgent + 1);
			WORD wPortCount=(wDataSize-sizeof(CMD_CS_C_RegisterAgent))/sizeof(tagPortPair);

			//����˿�
			for (int i=0; i<wPortCount; i++)
			{
				pGlobalAgentItem->InsertServicePort(pPortPair++);
			}
		}

		//���ش���
		if (AgentItem.wAgentKind==AGENT_KIND_GATE)
		{
			//��������
			SendServiceListItem(SERVICE_MODULE_GATE, dwSocketID, false);

			//��Ӷ˿�
			if (pGlobalAgentItem->GetServicePortCount()==0)
			{
				//��������
				CGlobalServiceInfo * pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GATE, ServiceItem.wServiceID);
				if (pGlobalServiceInfo != NULL)
				{
					//������Ϣ
					tagServiceItem * pServiceItem = &pGlobalServiceInfo->m_ServiceItem;

					//����ṹ
					tagPortPair PortPair;
					ZeroMemory(&PortPair, sizeof(PortPair));

					//�˿�ӳ��
					PortPair.wForwardPort = pServiceItem->wServicePort;
					PortPair.wServicePort = pServiceItem->wServicePort;

					//���ӳ��
					pGlobalAgentItem->InsertServicePort(&PortPair);						
				}
			}
		}

		//ע��ɹ�
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_SUCCESS,NULL,0);

		//Ⱥ������
		m_pITCPNetworkEngine->AllowBatchSend(dwSocketID, true, 0L);				

		return true;
	}
	case SUB_CS_C_REGISTER_SERVER:	//ע�᷿��
	{
		//Ч������
		ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterServer));
		if (wDataSize!=sizeof(CMD_CS_C_RegisterServer)) return false;

		//��Ϣ����
		CMD_CS_C_RegisterServer * pRegisterServer=(CMD_CS_C_RegisterServer *)pData;

		//��������
		CMD_CS_S_RegisterFailure RegisterFailure;
		ZeroMemory(&RegisterFailure, sizeof(RegisterFailure));

		//��ѯ����
		auto pServiceAttrib = CServiceAttribManager::SearchServiceAttrib(SERVICE_MODULE_GAME);
		if (pServiceAttrib == NULL)
		{
			//���ñ���
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), L"ϵͳδ��ѯ��ģ���ʶΪ[%d]�ķ�����Ϣ��ע��ʧ��", SERVICE_MODULE_GAME);
		}

		//У����Ŀ
		if (RegisterFailure.lErrorCode==0 && m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_GAME) >= pServiceAttrib->dwMaxInstanceNum)
		{
			//���ñ���
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), L"ϵͳ��[%s]ע���ʵ����Ŀ�Ѵﵽ%d����ע��ʧ�ܣ�", pServiceAttrib->szServiceName, pServiceAttrib->dwMaxInstanceNum);
		}

		//���ҷ���
		if (RegisterFailure.lErrorCode == 0 && m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GAME,pRegisterServer->wServerID)!=NULL)
		{
			//���ñ���
			RegisterFailure.lErrorCode=-1;
			StringCchCopy(RegisterFailure.szDescribeString,CountArray(RegisterFailure.szDescribeString), TEXT("�Ѿ�������ͬ��ʶ����Ϸ������Ϸ����ע��ʧ��"));			
		}

		//ע��ʧ��
		if (RegisterFailure.lErrorCode!=0)
		{
			//������Ϣ
			WORD wStringSize = CountStringBuffer(RegisterFailure.szDescribeString);
			WORD wSendSize = sizeof(RegisterFailure) - sizeof(RegisterFailure.szDescribeString) + wStringSize;
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_FAILURE, &RegisterFailure, wSendSize, NULL, 0);

			//�ж�����
			m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

			return true;
		}

		//���ð�
		WORD wBindIndex=LOWORD(dwSocketID);
		(m_pBindParameter+wBindIndex)->wServiceModule=SERVICE_MODULE_GAME;
		(m_pBindParameter+wBindIndex)->wServiceID=pRegisterServer->wServerID;

		//��������
		tagServiceItem ServiceItem;
		ZeroMemory(&ServiceItem, sizeof(ServiceItem));

		//��������
		ServiceItem.wServiceID = pRegisterServer->wServerID;
		ServiceItem.wServiceModule = SERVICE_MODULE_GAME;
		ServiceItem.wServicePort = pRegisterServer->wServerPort;			
		ServiceItem.dwServiceAddr = pRegisterServer->dwServerAddr;			
		StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), TEXT("��Ϸ������"));
		StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain), pRegisterServer->szServerDomain);

		//��������
		tagGameServer GameServer;
		ZeroMemory(&GameServer,sizeof(GameServer));

		//��������
		GameServer.wModuleID = pRegisterServer->wModuleID;
		GameServer.wKindID=pRegisterServer->wKindID;
		GameServer.wNodeID=pRegisterServer->wNodeID;
		GameServer.wSortID=pRegisterServer->wSortID;
		GameServer.wVisibleMask=pRegisterServer->wVisibleMask;
		GameServer.wServerID=pRegisterServer->wServerID;
		GameServer.wServerPort=pRegisterServer->wServerPort;
		GameServer.wServerType=pRegisterServer->wServerType;			
		GameServer.wServerLevel = pRegisterServer->wServerLevel;
		GameServer.wChairCount = pRegisterServer->wChairCount;
		GameServer.dwMaxPlayer=pRegisterServer->dwMaxPlayer;
		GameServer.dwOnLineCount=pRegisterServer->dwOnLineCount;
		GameServer.dwServerRule=pRegisterServer->dwServerRule;
		GameServer.dwServerAddr=pRegisterServer->dwServerAddr;	
		StringCchCopy(GameServer.szServerName,CountArray(GameServer.szServerName), pRegisterServer->szServerName);
		StringCchCopy(GameServer.szServerDomain,CountArray(GameServer.szServerDomain), pRegisterServer->szServerDomain);

		//��������
		GameServer.lCellScore=pRegisterServer->lCellScore;
		GameServer.lMinEnterScore=pRegisterServer->lMinEnterScore;
		GameServer.lMaxEnterScore=pRegisterServer->lMaxEnterScore;

		//ע�᷿��
		m_GlobalInfoManager.ActiveServerInfo(dwSocketID, ServiceItem, GameServer);

		//Ⱥ������
		//m_pITCPNetworkEngine->SendDataBatch(MDM_CS_COMMON_SERVICE,SUB_CS_S_SERVICE_INSERT,&ServiceItem,sizeof(ServiceItem),0L);

		//Ⱥ������
		m_pITCPNetworkEngine->SendDataBatch(MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVER_INSERT, &GameServer, sizeof(GameServer), 0L);

		//���ͼ��
		PushServiceListItem(SERVICE_MASK_MONITOR,dwSocketID,false);

		//������־
		PushServiceListItem(SERVICE_MASK_LOGGER, dwSocketID,false);

		//���ʹ洢
		PushServiceListItem(SERVICE_MASK_STORE, dwSocketID, false);

		//���ͱ���
		if ((pRegisterServer->wServerType&GAME_GENRE_MATCH)!=0)
		{
			//���ͱ���
			PushServiceListItem(SERVICE_MASK_MATCH, dwSocketID,false);				
		}

		//����Լս
		if ((pRegisterServer->wServerType&GAME_GENRE_BATTLE)!=0)
		{
			//����Լս
			PushServiceListItem(SERVICE_MASK_BATTLE, dwSocketID,false);
		}

		//������λ
		if ((pRegisterServer->wServerType & GAME_GENRE_RANKING) != 0)
		{
			//������λ
			PushServiceListItem(SERVICE_MASK_RANKING, dwSocketID,false);
		}		

		//���ͷ���
		SendServerListItem(dwSocketID);			

		//ע��ɹ�
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_SUCCESS,NULL,0);

		//Ⱥ������
		m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);			

		//����֪ͨ
		if (m_wCollectItem==INVALID_WORD)
		{
			m_wCollectItem=wBindIndex;
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_USER,NULL,0);
		}
		else m_WaitCollectItemArray.Add(wBindIndex);

		//���»���
		auto pGlobalServerInfo = (CGlobalServerInfo *)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GAME, ServiceItem.wServiceID);
		if (pGlobalServerInfo != NULL)
		{
			UpdateServerItem(pGlobalServerInfo,UPDATE_KIND_MODIFY);
		}

		return true;
	}
	case SUB_CS_C_REGISTER_MASTER:  //ע���̨
	{
		//Ч������
		ASSERT(wDataSize >= sizeof(CMD_CS_C_RegisterMaster));
		if (wDataSize < sizeof(CMD_CS_C_RegisterMaster)) return false;

		//��Ϣ����
		CMD_CS_C_RegisterMaster* pRegisterMaster = (CMD_CS_C_RegisterMaster*)pData;

		//����ģ��
		WORD wServiceModule = SERVICE_MODULE_MASTER;

		//��������
		CMD_CS_S_RegisterFailure RegisterFailure;
		ZeroMemory(&RegisterFailure, sizeof(RegisterFailure));

		//��ѯ����
		auto pServiceAttrib = CServiceAttribManager::SearchServiceAttrib(wServiceModule);
		if (pServiceAttrib == NULL)
		{
			//���ñ���
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), L"ϵͳδ��ѯ��ģ���ʶΪ[%d]�ķ�����Ϣ��ע��ʧ��", wServiceModule);
		}

		//У����Ŀ
		if (RegisterFailure.lErrorCode == 0 && m_GlobalInfoManager.GetServiceItemCount(wServiceModule) >= pServiceAttrib->dwMaxInstanceNum)
		{
			//���ñ���
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), L"ϵͳ����ע���[%s]ʵ����Ŀ�Ѵﵽ%d����ע��ʧ�ܣ�", pServiceAttrib->szServiceName, pServiceAttrib->dwMaxInstanceNum);
		}

		//���ҷ���
		if (RegisterFailure.lErrorCode == 0 && m_GlobalInfoManager.SearchServiceInfo(wServiceModule, pRegisterMaster->wServiceID) != NULL)
		{
			//���ñ���
			RegisterFailure.lErrorCode = -1;
			_snwprintf_s(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), TEXT("ϵͳ���Ѵ�����ͬ��ʶ��[%s]��ע��ʧ��"), pServiceAttrib->szServiceName);
		}

		//ע��ʧ��
		if (RegisterFailure.lErrorCode != 0)
		{
			//������Ϣ
			WORD wStringSize = CountStringBuffer(RegisterFailure.szDescribeString);
			WORD wSendSize = sizeof(RegisterFailure) - sizeof(RegisterFailure.szDescribeString) + wStringSize;
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_FAILURE, &RegisterFailure, wSendSize, NULL, 0);

			//�ж�����
			m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

			return true;
		}

		//���ð�
		WORD wBindIndex = LOWORD(dwSocketID);
		(m_pBindParameter + wBindIndex)->wServiceModule = SERVICE_MODULE_MASTER;
		(m_pBindParameter + wBindIndex)->wServiceID = pRegisterMaster->wServiceID;

		//��������
		tagServiceItem ServiceItem;
		ZeroMemory(&ServiceItem, sizeof(ServiceItem));

		//��������
		ServiceItem.wServiceID = pRegisterMaster->wServiceID;
		ServiceItem.wServiceModule = SERVICE_MODULE_MASTER;
		StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), TEXT("��̨����"));		

		//ע���̨
		if (m_GlobalInfoManager.ActiveServiceInfo(dwSocketID, ServiceItem) == false)
		{
			return false;
		}	

		//ע��ɹ�
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_REGISTER, SUB_CS_S_REGISTER_SUCCESS, NULL, 0);

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
	case SUB_CS_C_SERVICE_ONLINE:	 //��������
	{
		//Ч������
		ASSERT (wDataSize==sizeof(CMD_CS_C_ServiceOnline));
		if (wDataSize!=sizeof(CMD_CS_C_ServiceOnline)) return false;

		//��Ϣ����
		CMD_CS_C_ServiceOnline * pServiceOnline =(CMD_CS_C_ServiceOnline *)pData;

		//��ȡ����
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//��Ϸģ��
		if (pBindParameter->wServiceModule == SERVICE_MODULE_GAME)
		{
			//���ҷ���
			WORD wServerID = pBindParameter->wServiceID;
			CGlobalServerInfo * pGlobalServerInfo = (CGlobalServerInfo *)m_GlobalInfoManager.SearchServiceInfo(pBindParameter->wServiceModule, wServerID);

			//��������
			if (pGlobalServerInfo != NULL)
			{
				//��������
				CMD_CS_S_ServiceOnline ServiceOnline;
				ZeroMemory(&ServiceOnline, sizeof(ServiceOnline));

				//���ñ���
				pGlobalServerInfo->m_GameServer.dwOnLineCount = pServiceOnline->wOnlineCount;

				//���ñ���
				ServiceOnline.wServiceID = wServerID;
				ServiceOnline.wServiceModule = SERVICE_MODULE_GAME;
				ServiceOnline.wOnlineCount = pServiceOnline->wOnlineCount;

				//����֪ͨ
				SendDataToAllService(SERVICE_MODULE_LOGON,0, MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVICE_ONLINE, &ServiceOnline, sizeof(ServiceOnline));

				//����֪ͨ
				SendDataToAllService(SERVICE_MODULE_GATE,0, MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVICE_ONLINE, &ServiceOnline, sizeof(ServiceOnline));
			}
		}
		else
		{
			//��������
			CGlobalServiceInfo* pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(pBindParameter->wServiceModule, pBindParameter->wServiceID);
			if (pGlobalServiceInfo != NULL)
			{
				pGlobalServiceInfo->m_wOnlineCount = pServiceOnline->wOnlineCount;
			}
		}

		return true;
	}
	case SUB_CS_C_REPORT_JACKPOT:	 //�ϱ�����
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_CS_C_ReportJackpot));
		if (wDataSize != sizeof(CMD_CS_C_ReportJackpot)) return false;

		//��Ϣ����
		auto pReportJackpot = (CMD_CS_C_ReportJackpot*)pData;

		//��ȡ����
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//����Ч��
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_MONITOR);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_MONITOR) return false;

		//���ҷ���
		auto pGlobalMonitorInfo = (CGlobalMonitorInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_MONITOR, pBindParameter->wServiceID);

		//У����
		if (pGlobalMonitorInfo != NULL)
		{
			//���ҽ���
			auto pGlobalGameStock = pGlobalMonitorInfo->m_StockGameStock.SearchStockItem(pReportJackpot->wKindID);
			if (pGlobalGameStock == NULL)
			{
				pGlobalGameStock = pGlobalMonitorInfo->m_StockGameStock.CreateStockItem(pReportJackpot->wKindID);
			}

			if (pGlobalGameStock != NULL)
			{
				//���²ʽ�	
				pGlobalGameStock->m_wKindID = pReportJackpot->wKindID;
				pGlobalGameStock->m_lJackpotScore = pReportJackpot->lJackpotScore;

				//�㲥��Ϣ
				CMD_CS_S_GameJackpot GameJackpot;
				ZeroMemory(&GameJackpot, sizeof(GameJackpot));

				//���ý���
				GameJackpot.wKindID = pReportJackpot->wKindID;
				GameJackpot.lJackpotScore = pReportJackpot->lJackpotScore;

				//����֪ͨ							
				m_pITCPNetworkEngine->SendDataBatch(MDM_CS_COMMON_SERVICE, SUB_CS_S_GAME_JACKPOT, &GameJackpot, sizeof(GameJackpot), 0L);
			}				
		}

		return true;
	}
	case SUB_CS_C_SERVER_MODIFY:	 //�����޸�
	{
		//Ч������
		ASSERT(wDataSize==sizeof(CMD_CS_C_ServerModify));
		if (wDataSize!=sizeof(CMD_CS_C_ServerModify)) return false;

		//��Ϣ����
		CMD_CS_C_ServerModify * pServerModify=(CMD_CS_C_ServerModify *)pData;

		//��ȡ����
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//����Ч��
		ASSERT(pBindParameter->wServiceModule== SERVICE_MODULE_GAME);
		if (pBindParameter->wServiceModule!= SERVICE_MODULE_GAME) return false;

		//���ҷ���
		WORD wServerID=pBindParameter->wServiceID;
		CGlobalServerInfo * pGlobalServerInfo=(CGlobalServerInfo *)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GAME,wServerID);

		//�����޸�
		if (pGlobalServerInfo!=NULL)
		{
			//��������
			CMD_CS_S_ServerModify ServerModify;
			ZeroMemory(&ServerModify,sizeof(ServerModify));

			//���ñ���
			pGlobalServerInfo->m_ServiceItem.wServicePort = pServerModify->wServerPort;
			pGlobalServerInfo->m_ServiceItem.dwServiceAddr = pServerModify->dwServerAddr;
			StringCchCopy(pGlobalServerInfo->m_ServiceItem.szServiceDomain, CountArray(pGlobalServerInfo->m_ServiceItem.szServiceDomain), pServerModify->szServerDomain);

			//���ñ���
			pGlobalServerInfo->m_GameServer.wSortID=pServerModify->wSortID;
			pGlobalServerInfo->m_GameServer.wKindID=pServerModify->wKindID;
			pGlobalServerInfo->m_GameServer.wNodeID=pServerModify->wNodeID;
			pGlobalServerInfo->m_GameServer.wServerPort=pServerModify->wServerPort;		
			pGlobalServerInfo->m_GameServer.wServerLevel =pServerModify->wServerLevel;
			pGlobalServerInfo->m_GameServer.dwServerRule=pServerModify->dwServerRule;
			pGlobalServerInfo->m_GameServer.dwMaxPlayer=pServerModify->dwMaxPlayer;
			pGlobalServerInfo->m_GameServer.dwOnLineCount=pServerModify->dwOnLineCount;
			pGlobalServerInfo->m_GameServer.dwServerAddr = pServerModify->dwServerAddr;
			StringCchCopy(pGlobalServerInfo->m_GameServer.szServerName,LEN_SERVER, pServerModify->szServerName);
			StringCchCopy(pGlobalServerInfo->m_GameServer.szServerDomain,CountArray(pGlobalServerInfo->m_GameServer.szServerDomain), pServerModify->szServerDomain);

			//������Ϣ
			pGlobalServerInfo->m_GameServer.lCellScore = pServerModify->lCellScore;
			pGlobalServerInfo->m_GameServer.lMinEnterScore = pServerModify->lMinEnterScore;
			pGlobalServerInfo->m_GameServer.lMaxEnterScore = pServerModify->lMaxEnterScore;

			//���ñ���
			ServerModify.wServerID=wServerID;
			ServerModify.wSortID=pServerModify->wSortID;
			ServerModify.wKindID=pServerModify->wKindID;
			ServerModify.wNodeID=pServerModify->wNodeID;
			ServerModify.wServerPort=pServerModify->wServerPort;
			ServerModify.wServerType=pServerModify->wServerType;
			ServerModify.wServerLevel =pServerModify->wServerLevel;
			ServerModify.dwMaxPlayer=pServerModify->dwMaxPlayer;
			ServerModify.dwOnLineCount=pServerModify->dwOnLineCount;
			ServerModify.dwServerAddr=pServerModify->dwServerAddr;
			ServerModify.dwServerRule=pServerModify->dwServerRule;
			StringCchCopy(ServerModify.szServerName,CountArray(ServerModify.szServerName), pServerModify->szServerName);
			StringCchCopy(ServerModify.szServerDomain,CountArray(ServerModify.szServerDomain), pServerModify->szServerDomain);

			//������Ϣ
			ServerModify.lCellScore = pServerModify->lCellScore;
			ServerModify.lMinEnterScore = pServerModify->lMinEnterScore;
			ServerModify.lMaxEnterScore = pServerModify->lMaxEnterScore;

			//����֪ͨ							
			m_pITCPNetworkEngine->SendDataBatch(MDM_CS_COMMON_SERVICE,SUB_CS_S_SERVER_MODIFY,&ServerModify,sizeof(ServerModify),0L);
		}

		return true;
	}		
	case SUB_CS_C_QUERY_ROUTE_ITEM:	 //��ѯ·��
	{
		//Ч������
		ASSERT (wDataSize == sizeof(CMD_CS_C_QueryRouteItem));
		if (wDataSize != sizeof(CMD_CS_C_QueryRouteItem)) return false;

		//��Ϣ����
		auto pQueryRouteItem = (CMD_CS_C_QueryRouteItem*)pData;

		//��ѯ���
		CMD_CS_S_QueryRouteResult QueryRouteResult = {};
		QueryRouteResult.dwRouteID = pQueryRouteItem->dwRouteID;
		QueryRouteResult.dwTokenID = pQueryRouteItem->dwTokenID;
		QueryRouteResult.wServiceModule = pQueryRouteItem->wServiceModule;

		//���ҷ���
		auto pGlobalServiceInfo = m_GlobalInfoManager.LookupService(pQueryRouteItem->wServiceModule, pQueryRouteItem->wServiceKind, pQueryRouteItem->dwRouteID);
		if (pGlobalServiceInfo == NULL)
		{
			//�������
			pGlobalServiceInfo = m_GlobalInfoManager.AssignService(pQueryRouteItem->wServiceModule, pQueryRouteItem->wServiceKind, pQueryRouteItem->dwRouteID);
		}

		if (pGlobalServiceInfo != NULL)
		{
			//��ѯ·��
			auto pRouteItem = pGlobalServiceInfo->QueryRouteItem(pQueryRouteItem->dwRouteID);
			if (pRouteItem != NULL)
			{
				QueryRouteResult.wServiceID = pGlobalServiceInfo->GetServiceID();

				//����·��
				if (pRouteItem->cbRouteState == ROUTE_STATE_NULL)
				{
					//����ṹ
					CMD_CS_S_ActiveRouteItem ActiveRouteItem = {};
					ActiveRouteItem.dwRouteID = pQueryRouteItem->dwRouteID;

					//������Ϣ
					if (SendDataToService(pQueryRouteItem->wServiceModule, QueryRouteResult.wServiceID, MDM_CS_COMMON_SERVICE, SUB_CS_S_ACTIVE_ROUTE_ITEM, &ActiveRouteItem, sizeof(ActiveRouteItem)))
					{
						//����״̬
						pRouteItem->cbRouteState = ROUTE_STATE_ACTIVE;
					}
				}
			}			
		}

		//�ظ���Ϣ
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_QUERY_ROUTE_RESULT, &QueryRouteResult, sizeof(QueryRouteResult), &pQueryRouteItem->dwTokenID, 1);

		return true;
	}
	case SUB_CS_C_ACTIVE_ROUTE_ITEM: //����·��
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_CS_C_ActiveRouteItem));
		if (wDataSize != sizeof(CMD_CS_C_ActiveRouteItem)) return false;

		//��Ϣ����
		auto pActiveRouteItem = (CMD_CS_C_ActiveRouteItem*)pData;

		//ӳ�����
		auto pGlobalServiceInfo = m_GlobalInfoManager.LookupService(pActiveRouteItem->wServiceModule, pActiveRouteItem->wServiceKind, pActiveRouteItem->dwRouteID);
		if (pGlobalServiceInfo == NULL)
		{
			//�������
			pGlobalServiceInfo = m_GlobalInfoManager.AssignService(pActiveRouteItem->wServiceModule, pActiveRouteItem->wServiceKind, pActiveRouteItem->dwRouteID);
		}

		if (pGlobalServiceInfo!=NULL)
		{
			//��ѯ·��
			auto pRouteItem = pGlobalServiceInfo->QueryRouteItem(pActiveRouteItem->dwRouteID);
			if (pRouteItem != NULL)
			{
				//����·��
				if (pRouteItem->cbRouteState == ROUTE_STATE_NULL)
				{
					//����ṹ
					CMD_CS_S_ActiveRouteItem ActiveRouteItem = {};
					ActiveRouteItem.dwRouteID = pActiveRouteItem->dwRouteID;

					//������Ϣ
					if (SendDataToService(pActiveRouteItem->wServiceModule, pGlobalServiceInfo->GetServiceID(), MDM_CS_COMMON_SERVICE, SUB_CS_S_ACTIVE_ROUTE_ITEM, &ActiveRouteItem, sizeof(ActiveRouteItem)))
					{
						//����״̬
						pRouteItem->cbRouteState = ROUTE_STATE_ACTIVE;
					}
				}
			}
		}
		else
		{
			CTraceService::TraceString(TEXT("����ӳ��ʧ�ܣ�ServiceModule=%d,ServiceKind=%d,RouteID=%d"),TraceLevel_Exception, pActiveRouteItem->wServiceModule, pActiveRouteItem->wServiceKind, pActiveRouteItem->dwRouteID);
		}

		return true;
	}
	case SUB_CS_C_APPEND_ROUTE_ITEM: //���·��
	{
		//Ч������
		ASSERT((wDataSize - sizeof(CMD_CS_C_AppendRouteItem)) % sizeof(tagRouteItem) == 0);
		if ((wDataSize - sizeof(CMD_CS_C_AppendRouteItem)) % sizeof(tagRouteItem) != 0) return false;

		//��ȡ����
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//��ȡ����
		auto pAppendRouteItem = (CMD_CS_C_AppendRouteItem*)pData;

		//·������
		auto pRouteItem = (tagRouteItem*)(pAppendRouteItem + 1);
		auto wItemCount = (wDataSize - sizeof(CMD_CS_C_AppendRouteItem)) / sizeof(tagRouteItem);

		for (int i = 0; i < wItemCount; i++)
		{
			//���ҷ���
			auto pGlobalServiceInfo = m_GlobalInfoManager.LookupService(pAppendRouteItem->wServiceModule, pAppendRouteItem->wServiceKind, pRouteItem->dwRouteID);
			if (pGlobalServiceInfo == NULL)
			{
				//�������
				pGlobalServiceInfo = m_GlobalInfoManager.AssignService(pAppendRouteItem->wServiceModule, pAppendRouteItem->wServiceKind, pRouteItem->dwRouteID, pBindParameter->wServiceID);
			}

			if (pGlobalServiceInfo != NULL)
			{
				//��ѯ·��
				auto pQueryRouteItem = pGlobalServiceInfo->QueryRouteItem(pRouteItem->dwRouteID);
				if (pQueryRouteItem != NULL)
				{
					pQueryRouteItem->cbRouteState = pRouteItem->cbRouteState;
				}
			}

			pRouteItem++;
		}		

		//�㲥����
		if (pAppendRouteItem->bTheLast)
		{
			//���嶨��
			BYTE cbBuffer[SOCKET_PACKET];

			//ö�ٱ���
			tagRouteItem* pRouteItem = NULL;
			auto wPacketSize = sizeof(CMD_CS_S_AppendRouteItem);
			auto pAppendRouteItem1 = (CMD_CS_S_AppendRouteItem*)cbBuffer;			

			//���ñ���			
			pAppendRouteItem1->bTheFirst = true;
			pAppendRouteItem1->wServiceID = pBindParameter->wServiceID;
			pAppendRouteItem1->wServiceModule = pAppendRouteItem->wServiceModule;

			//���ҷ���
			auto pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(pAppendRouteItem->wServiceModule, pBindParameter->wServiceID);
			if (pGlobalServiceInfo != NULL)
			{
				//��������
				POSITION Position = NULL;

				do
				{
					pRouteItem = pGlobalServiceInfo->EnumRouteItem(Position);
					if (pRouteItem == NULL) break;

					//��������
					if (pRouteItem->cbRouteState == ROUTE_STATE_SERVICE)
					{
						CopyMemory(&cbBuffer[wPacketSize], pRouteItem, sizeof(tagRouteItem));
						wPacketSize += sizeof(tagRouteItem);
					}

					//�Ƴ��ж�
					if (wPacketSize + sizeof(tagRouteItem) > CountArray(cbBuffer))
					{
						m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_APPEND_ROUTE_ITEM, cbBuffer, wPacketSize, NULL, 0);

						//���ñ���
						pAppendRouteItem1->bTheFirst = false;
						wPacketSize = sizeof(CMD_CS_S_AppendRouteItem);
					}

				} while (Position != NULL);

				//�����ж�
				if (wPacketSize > sizeof(CMD_CS_S_AppendRouteItem))
				{
					m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_APPEND_ROUTE_ITEM, cbBuffer, wPacketSize, NULL, 0);
				}
			}
		}

		return true;
	}
	case SUB_CS_C_UPDATE_ROUTE_ITEM: //����·��
	{
		//Ч������
		ASSERT (wDataSize == sizeof(CMD_CS_C_UpdateRouteItem));
		if (wDataSize != sizeof(CMD_CS_C_UpdateRouteItem)) return false;

		//��Ϣ����
		auto pUpdateRouteItem = (CMD_CS_C_UpdateRouteItem*)pData;

		//��ȡ����
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//��ѯ����
		auto pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(pBindParameter->wServiceModule, pBindParameter->wServiceID);
		if (pGlobalServiceInfo != NULL)
		{
			//��ѯ·��
			auto pRouteItem = pGlobalServiceInfo->QueryRouteItem(pUpdateRouteItem->dwRouteID);
			if (pRouteItem != NULL)
			{
				pRouteItem->cbRouteState = pUpdateRouteItem->cbRouteState;
				if (pRouteItem->cbRouteState != ROUTE_STATE_SERVICE)
				{
					pGlobalServiceInfo->RemoveRouteItem(pUpdateRouteItem->dwRouteID);

					//����ṹ
					CMD_CS_S_RemoveRouteItem RemoveRouteItem = {};

					//���ñ���
					RemoveRouteItem.dwRouteID = pUpdateRouteItem->dwRouteID;
					RemoveRouteItem.wServiceID = pGlobalServiceInfo->GetServiceID();
					RemoveRouteItem.wServiceModule = pGlobalServiceInfo->GetServiceModule();

					//��������
					SendDataToAllService(SERVICE_MODULE_GATE, 0, MDM_CS_COMMON_SERVICE, SUB_CS_S_REMOVE_ROUTE_ITEM, &RemoveRouteItem, sizeof(RemoveRouteItem));
				}
				else
				{
					//���涨��
					BYTE cbBuffer[1024] = {};
					auto pAppendRouteItem = (CMD_CS_S_AppendRouteItem*)cbBuffer;

					//���ñ���
					pAppendRouteItem->wServiceID = pBindParameter->wServiceID;
					pAppendRouteItem->wServiceModule = pBindParameter->wServiceModule;

					//��������
					CopyMemory(pAppendRouteItem+1, pRouteItem, sizeof(tagRouteItem));

					//��������
					SendDataToAllService(SERVICE_MODULE_GATE,0,MDM_CS_COMMON_SERVICE, SUB_CS_S_APPEND_ROUTE_ITEM, cbBuffer,sizeof(CMD_CS_S_AppendRouteItem)+sizeof(tagRouteItem));
				}
			}
		}		

		return true;
	}
	case SUB_CS_C_REMOVE_ROUTE_ITEM: //�Ƴ�·��
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_CS_C_RemoveRouteItem));
		if (wDataSize != sizeof(CMD_CS_C_RemoveRouteItem)) return false;

		//��Ϣ����
		auto pRemoveRouteItem = (CMD_CS_C_RemoveRouteItem*)pData;

		//��ȡ����
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//��ѯ����
		auto pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(pBindParameter->wServiceModule, pBindParameter->wServiceID);
		if (pGlobalServiceInfo != NULL)
		{
			//��ѯ·��
			pGlobalServiceInfo->RemoveRouteItem(pRemoveRouteItem->dwRouteID);

			//����ṹ
			CMD_CS_S_RemoveRouteItem RemoveRouteItem = {};

			//���ñ���
			RemoveRouteItem.dwRouteID = pRemoveRouteItem->dwRouteID;
			RemoveRouteItem.wServiceID = pBindParameter->wServiceID;
			RemoveRouteItem.wServiceModule = pBindParameter->wServiceModule;

			//������Ϣ
			SendDataToAllService(SERVICE_MODULE_GATE, 0, MDM_CS_COMMON_SERVICE, SUB_CS_S_REMOVE_ROUTE_ITEM, &RemoveRouteItem, sizeof(RemoveRouteItem));
		}

		return true;
	}
	}

	return false;
}

//�û�����
bool CAttemperEngineSink::OnTCPNetworkMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_ENTER_SERVER:		//�û�����
	{
		//Ч������
		ASSERT(wDataSize==sizeof(CMD_CS_C_EnterServer));
		if (wDataSize!=sizeof(CMD_CS_C_EnterServer)) return false;

		//��Ϣ����
		CMD_CS_C_EnterServer * pEnterServer=(CMD_CS_C_EnterServer *)pData;
			
		//��ȡ����
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//����Ч��
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_GAME);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_GAME) return false;

		//���ҷ���
		auto pGlobalServerInfo = (CGlobalServerInfo *)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GAME, pBindParameter->wServiceID);
		if (pGlobalServerInfo == NULL) return false;

		//�����û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pEnterServer->dwUserID);
		if (pGlobalUserInfo == NULL)
		{
			//��������
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo, sizeof(GlobalUserInfo));

			//���ñ���
			GlobalUserInfo.dwUserID = pEnterServer->dwUserID;
			GlobalUserInfo.dwGameID = pEnterServer->dwGameID;
			GlobalUserInfo.cbGender = pEnterServer->cbGender;
			GlobalUserInfo.wFaceID = pEnterServer->wFaceID;
			GlobalUserInfo.dwCustomID = pEnterServer->dwCustomID;
			StringCchCopy(GlobalUserInfo.szNickName, CountArray(GlobalUserInfo.szNickName), pEnterServer->szNickName);

			//�����û�
			pGlobalUserInfo = m_GlobalInfoManager.ActiveUserInfo(GlobalUserInfo);
			if (pGlobalUserInfo == NULL) return false;
		}

		//�󶨷���		
		pGlobalUserInfo->BindServerInfo(pGlobalServerInfo);				

		return true;
	}
	case SUB_CS_C_LEAVE_SERVER:		//�û��뿪
	{
		//Ч������
		ASSERT(wDataSize==sizeof(CMD_CS_C_LeaveServer));
		if (wDataSize!=sizeof(CMD_CS_C_LeaveServer)) return false;

		//��Ϣ����
		CMD_CS_C_LeaveServer * pLeaveServer=(CMD_CS_C_LeaveServer *)pData;

		//��ȡ����
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//����Ч��
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_GAME);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_GAME) return false;

		//���ҷ���
		auto pGlobalServerInfo = (CGlobalServerInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GAME, pBindParameter->wServiceID);
		if (pGlobalServerInfo == NULL) return false;

		//�����û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pLeaveServer->dwUserID);
		if (pGlobalUserInfo == NULL) return true;

		//������
		pGlobalUserInfo->UnBindServerInfo(pGlobalServerInfo);

		//ɾ���û�
		if (pGlobalUserInfo->BindServiceCount() == 0)
		{
			m_GlobalInfoManager.DeleteUserInfo(pLeaveServer->dwUserID);
		}

		return true;
	}
	case SUB_CS_C_ENTER_MESSAGE:	//�û�����
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_CS_C_EnterMessage));
		if (wDataSize != sizeof(CMD_CS_C_EnterMessage)) return false;

		//��Ϣ����
		CMD_CS_C_EnterMessage* pEnterMessage = (CMD_CS_C_EnterMessage*)pData;

		//��ȡ����
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//����Ч��
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_MESSAGE);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_MESSAGE) return false;

		//���ҷ���
		auto pGlobalMessageInfo = m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_MESSAGE, pBindParameter->wServiceID);
		if (pGlobalMessageInfo == NULL) return false;

		//�����û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pEnterMessage->dwUserID);
		if (pGlobalUserInfo == NULL)
		{
			//��������
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo, sizeof(GlobalUserInfo));

			//���ñ���
			GlobalUserInfo.dwUserID = pEnterMessage->dwUserID;						

			//�����û�
			pGlobalUserInfo = m_GlobalInfoManager.ActiveUserInfo(GlobalUserInfo);
			if (pGlobalUserInfo == NULL) return false;
		}

		//�󶨷���		
		pGlobalUserInfo->BindMessageInfo(pGlobalMessageInfo);

		return true;
	}
	case SUB_CS_C_LEAVE_MESSAGE:	//�û��뿪
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_CS_C_LeaveMessage));
		if (wDataSize != sizeof(CMD_CS_C_LeaveMessage)) return false;

		//��Ϣ����
		CMD_CS_C_LeaveMessage* pLeaveMessage = (CMD_CS_C_LeaveMessage*)pData;

		//��ȡ����
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//����Ч��
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_MESSAGE);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_MESSAGE) return false;

		//���ҷ���
		auto pGlobalMessageInfo = m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_MESSAGE, pBindParameter->wServiceID);
		if (pGlobalMessageInfo == NULL) return false;

		//�����û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pLeaveMessage->dwUserID);
		if (pGlobalUserInfo == NULL) return true;

		//������
		pGlobalUserInfo->UnBindMessageInfo(pGlobalMessageInfo);

		//ɾ���û�
		if (pGlobalUserInfo->BindServiceCount() == 0)
		{
			m_GlobalInfoManager.DeleteUserInfo(pLeaveMessage->dwUserID);
		}

		return true;
	}
	case SUB_CS_C_ENTER_CLUBPLAZA:	//�û�����
	{
		//Ч������
		ASSERT (wDataSize == sizeof(CMD_CS_C_EnterClubPlaza));
		if (wDataSize != sizeof(CMD_CS_C_EnterClubPlaza)) return false;

		//��Ϣ����
		auto pEnterClubPlaza = (CMD_CS_C_EnterClubPlaza*)pData;		

		//��ȡ����
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//����Ч��
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_CLUBPLAZA);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_CLUBPLAZA) return false;

		//���ҷ���
		auto pGlobalLogonInfo = (CGlobalLogonInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_CLUBPLAZA, pBindParameter->wServiceID);
		if (pGlobalLogonInfo == NULL) return false;

		//�����û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pEnterClubPlaza->dwUserID);
		if (pGlobalUserInfo != NULL)
		{
			//����״̬
			pGlobalUserInfo->m_cbCBUserStatus = US_CB_ONLINE;

			//��������
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo, sizeof(GlobalUserInfo));

			//���ñ���
			GlobalUserInfo.dwUserID = pGlobalUserInfo->m_dwUserID;
			GlobalUserInfo.dwGameID = pGlobalUserInfo->m_dwGameID;
			GlobalUserInfo.cbGender = pGlobalUserInfo->m_cbGender;			
			StringCchCopy(GlobalUserInfo.szNickName, CountArray(GlobalUserInfo.szNickName), pGlobalUserInfo->m_szNickName);
			StringCchCopy(GlobalUserInfo.szUserHeadUrl, CountArray(GlobalUserInfo.szNickName), pGlobalUserInfo->m_szUserHeadUrl);//kk jia

			//״̬��Ϣ
			GlobalUserInfo.cbIMUserStatus = pGlobalUserInfo->m_cbIMUserStatus;
			GlobalUserInfo.cbCBUserStatus = pGlobalUserInfo->m_cbCBUserStatus;

			//ͷ���ʶ
			GlobalUserInfo.wFaceID = pGlobalUserInfo->m_wFaceID;
			GlobalUserInfo.dwCustomID = pGlobalUserInfo->m_dwCustomID;

			//״̬��Ϣ
			GlobalUserInfo.cbShutUpStatus = pGlobalUserInfo->m_cbShutUpStatus;
			GlobalUserInfo.dwShutUpOverTime = pGlobalUserInfo->m_dwShutUpOverTime;

			//������Ϣ
			GlobalUserInfo.SocketInfo.wGateID = pGlobalUserInfo->m_wGateID;
			GlobalUserInfo.SocketInfo.dwTokenID = pGlobalUserInfo->m_dwTokenID;

			//�㲥��Ϣ
			SendDataToAllService(SERVICE_MODULE_CLUB, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_CB_USER_ONLINE, &GlobalUserInfo, sizeof(tagGlobalUserInfo));

			//�㲥��Ϣ
			SendDataToAllService(SERVICE_MODULE_CLUBPLAZA, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_CB_USER_ONLINE, &GlobalUserInfo, sizeof(tagGlobalUserInfo));
		}

		return true;
	}
	case SUB_CS_C_LEAVE_CLUBPLAZA:	//�û��뿪
	{
		//Ч������
		ASSERT (wDataSize == sizeof(CMD_CS_C_LeaveClubPlaza));
		if (wDataSize != sizeof(CMD_CS_C_LeaveClubPlaza)) return false;

		//��Ϣ����
		auto pLeaveClubPlaza = (CMD_CS_C_LeaveClubPlaza*)pData;

		//��ȡ����
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//����Ч��
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_CLUBPLAZA);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_CLUBPLAZA) return false;

		//���ҷ���
		auto pGlobalLogonInfo = (CGlobalLogonInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_CLUBPLAZA, pBindParameter->wServiceID);
		if (pGlobalLogonInfo == NULL) return false;

		//�����û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pLeaveClubPlaza->dwUserID);
		if (pGlobalUserInfo != NULL)
		{
			//����״̬
			pGlobalUserInfo->m_cbCBUserStatus = US_CB_OFFLINE;

			//��������
			CMD_CS_CB_UserOffline UserOffline = {};			
			UserOffline.dwUserID = pLeaveClubPlaza->dwUserID;

			//�㲥��Ϣ
			SendDataToAllService(SERVICE_MODULE_CLUB, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_CB_USER_OFFLINE, &UserOffline, sizeof(UserOffline));

			//�㲥��Ϣ
			SendDataToAllService(SERVICE_MODULE_CLUBPLAZA, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_CB_USER_OFFLINE, &UserOffline, sizeof(UserOffline));
		}

		return true;
	}
	case SUB_CS_C_USER_ONLINE:		//�û�����
	{
		//Ч������
		ASSERT (wDataSize == sizeof(CMD_CS_C_UserOnline));
		if (wDataSize != sizeof(CMD_CS_C_UserOnline)) return false;

		//��Ϣ����
		CMD_CS_C_UserOnline* pUserOnline = (CMD_CS_C_UserOnline*)pData;
		pUserOnline->szNickName[CountArray(pUserOnline->szNickName) - 1] = 0;

		//��ȡ����
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//����Ч��
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_LOGON);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_LOGON) return false;

		//���ҷ���
		auto pGlobalLogonInfo = (CGlobalLogonInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_LOGON, pBindParameter->wServiceID);
		if (pGlobalLogonInfo == NULL) return false;

		//�����û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pUserOnline->dwUserID);
		if (pGlobalUserInfo == NULL)
		{
			//��������
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo, sizeof(GlobalUserInfo));

			//���ñ���
			GlobalUserInfo.dwUserID = pUserOnline->dwUserID;
			GlobalUserInfo.dwGameID = pUserOnline->dwGameID;
			GlobalUserInfo.cbGender = pUserOnline->cbGender;
			StringCchCopy(GlobalUserInfo.szNickName, CountArray(GlobalUserInfo.szNickName), pUserOnline->szNickName);

			//ͷ���ʶ
			GlobalUserInfo.wFaceID = pUserOnline->wFaceID;
			GlobalUserInfo.dwCustomID = pUserOnline->dwCustomID;

			//״̬��Ϣ
			GlobalUserInfo.cbShutUpStatus = pUserOnline->cbShutUpStatus;
			GlobalUserInfo.dwShutUpOverTime = pUserOnline->dwShutUpOverTime;

			//������Ϣ
			GlobalUserInfo.SocketInfo.wGateID = pUserOnline->wGateID;
			GlobalUserInfo.SocketInfo.dwTokenID = pUserOnline->dwTokenID;

			//�����û�
			pGlobalUserInfo = m_GlobalInfoManager.ActiveUserInfo(GlobalUserInfo);
			if (pGlobalUserInfo == NULL) return false;	

			//�㲥��Ϣ
			SendDataToAllService(SERVICE_MODULE_CHAT,0, MDM_CS_FORWARD_SERVICE, SUB_CS_IM_USER_ONLINE, &GlobalUserInfo, sizeof(GlobalUserInfo));
		}
		else
		{
			//�û���Ϣ
			pGlobalUserInfo->m_dwGameID = pUserOnline->dwGameID;
			pGlobalUserInfo->m_cbGender = pUserOnline->cbGender;
			pGlobalUserInfo->m_wFaceID = pUserOnline->wFaceID;
			pGlobalUserInfo->m_dwCustomID = pUserOnline->dwCustomID;
			StringCchCopy(pGlobalUserInfo->m_szNickName, CountArray(pGlobalUserInfo->m_szNickName), pUserOnline->szNickName);
			StringCchCopy(pGlobalUserInfo->m_szUserHeadUrl, CountArray(pGlobalUserInfo->m_szUserHeadUrl), pUserOnline->szUserHeadUrl
			);//kk jia

			//״̬��Ϣ
			pGlobalUserInfo->m_cbShutUpStatus = pUserOnline->cbShutUpStatus;
			pGlobalUserInfo->m_dwShutUpOverTime = pUserOnline->dwShutUpOverTime;

			//������Ϣ
			pGlobalUserInfo->m_wGateID = pUserOnline->wGateID;
			pGlobalUserInfo->m_dwTokenID = pUserOnline->dwTokenID;
		}			

		//�󶨷���
		pGlobalUserInfo->BindLogonInfo(pGlobalLogonInfo);

		return true;
	}
	case SUB_CS_C_USER_OFFLINE:		//�û�����
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_CS_C_UserOffline));
		if (wDataSize != sizeof(CMD_CS_C_UserOffline)) return false;

		//��Ϣ����
		CMD_CS_C_UserOffline* pUserOffline = (CMD_CS_C_UserOffline*)pData;
			
		//��ȡ����
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//����Ч��
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_LOGON);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_LOGON) return false;

		//���ҷ���
		auto pGlobalLogonInfo = (CGlobalLogonInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_LOGON, pBindParameter->wServiceID);
		if (pGlobalLogonInfo == NULL) return false;

		//�����û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pUserOffline->dwUserID);
		if (pGlobalUserInfo != NULL)
		{
			//��������
			if (pGlobalUserInfo->m_cbIMUserStatus == US_IM_ONLINE)
			{
				//����ṹ
				CMD_CS_IM_UserOffline UserOffline = {};
				UserOffline.dwUserID = pUserOffline->dwUserID;

				//�㲥��Ϣ
				SendDataToAllService(SERVICE_MODULE_CHAT, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_IM_USER_OFFLINE, &UserOffline, sizeof(UserOffline));
			}			

			//��������
			if (pGlobalUserInfo->m_cbCBUserStatus == US_CB_ONLINE)
			{
				//��������
				CMD_CS_CB_UserOffline UserOffline = {};				
				UserOffline.dwUserID = pUserOffline->dwUserID;

				//�㲥��Ϣ
				SendDataToAllService(SERVICE_MODULE_CLUB, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_CB_USER_OFFLINE, &UserOffline, sizeof(UserOffline));

				//�㲥��Ϣ
				SendDataToAllService(SERVICE_MODULE_CLUBPLAZA, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_CB_USER_OFFLINE, &UserOffline, sizeof(UserOffline));
			}

			//������Ϣ
			pGlobalUserInfo->m_wGateID = 0;
			pGlobalUserInfo->m_dwTokenID = 0;

			//�󶨷���
			pGlobalUserInfo->UnBindLogonInfo(pGlobalLogonInfo);

			//ɾ���û�
			if (pGlobalUserInfo->BindServiceCount() == 0)
			{
				m_GlobalInfoManager.DeleteUserInfo(pUserOffline->dwUserID);
			}
		}					

		return true;
	}	
	case SUB_CS_C_USER_UPDATE:		//�û�����
	{
		//Ч������
		ASSERT(wDataSize == sizeof(CMD_CS_C_UserUpdate));
		if (wDataSize != sizeof(CMD_CS_C_UserUpdate)) return false;

		//��Ϣ����
		auto pUserDataUpdate = (CMD_CS_C_UserUpdate*)pData;
		
		//��ȡ����
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//����Ч��
		ASSERT(pBindParameter->wServiceModule == SERVICE_MODULE_LOGON);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_LOGON) return false;

		//���ҷ���
		auto pGlobalLogonInfo = (CGlobalLogonInfo*)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_LOGON, pBindParameter->wServiceID);
		if (pGlobalLogonInfo == NULL) return false;

		//�����û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pUserDataUpdate->dwUserID);
		if (pGlobalUserInfo != NULL)
		{
			//�û��ǳ�
			if (pUserDataUpdate->dwInfoMask & INFO_MASK_NICKNAME)
			{
				StringCchCopy(pGlobalUserInfo->m_szNickName, CountArray(pGlobalUserInfo->m_szNickName), pUserDataUpdate->szNickName);
				CTraceService::TraceString(TEXT("���%d�����ǳƣ�%s"), TraceLevel_Debug,pUserDataUpdate->dwUserID, pUserDataUpdate->szNickName);
			}
			StringCchCopy(pGlobalUserInfo->m_szUserHeadUrl, CountArray(pGlobalUserInfo->m_szUserHeadUrl), pUserDataUpdate->szUserHeadUrl);//kk jia
			//�û�ͷ��
			if (pUserDataUpdate->dwInfoMask & INFO_MASK_FACEID)
			{
				pGlobalUserInfo->m_wFaceID = pUserDataUpdate->wFaceID;
				pGlobalUserInfo->m_dwCustomID = pUserDataUpdate->dwCustomID;
				CTraceService::TraceString(TEXT("���%d����ͷ��%d|%d"), TraceLevel_Debug, pUserDataUpdate->dwUserID,pUserDataUpdate->wFaceID, pUserDataUpdate->dwCustomID);
			}	

			//��������
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo, sizeof(GlobalUserInfo));

			//���ñ���
			GlobalUserInfo.dwUserID = pGlobalUserInfo->m_dwUserID;
			GlobalUserInfo.dwGameID = pGlobalUserInfo->m_dwGameID;
			GlobalUserInfo.cbGender = pGlobalUserInfo->m_cbGender;
			StringCchCopy(GlobalUserInfo.szNickName, CountArray(GlobalUserInfo.szNickName), pGlobalUserInfo->m_szNickName);
			StringCchCopy(GlobalUserInfo.szUserHeadUrl, CountArray(GlobalUserInfo.szUserHeadUrl), pGlobalUserInfo->m_szUserHeadUrl);//kk jia

			//״̬��Ϣ
			GlobalUserInfo.cbIMUserStatus = pGlobalUserInfo->m_cbIMUserStatus;
			GlobalUserInfo.cbCBUserStatus = pGlobalUserInfo->m_cbCBUserStatus;

			//ͷ���ʶ
			GlobalUserInfo.wFaceID = pGlobalUserInfo->m_wFaceID;
			GlobalUserInfo.dwCustomID = pGlobalUserInfo->m_dwCustomID;

			//״̬��Ϣ
			GlobalUserInfo.cbShutUpStatus = pGlobalUserInfo->m_cbShutUpStatus;
			GlobalUserInfo.dwShutUpOverTime = pGlobalUserInfo->m_dwShutUpOverTime;

			//������Ϣ
			GlobalUserInfo.SocketInfo.wGateID = pGlobalUserInfo->m_wGateID;
			GlobalUserInfo.SocketInfo.dwTokenID = pGlobalUserInfo->m_dwTokenID;

			//�㲥��Ϣ
			SendDataToAllService(SERVICE_MODULE_CLUB, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_CB_USER_UPDATE, &GlobalUserInfo, sizeof(tagGlobalUserInfo));

			//�㲥��Ϣ
			SendDataToAllService(SERVICE_MODULE_CLUBPLAZA, 0, MDM_CS_FORWARD_SERVICE, SUB_CS_CB_USER_UPDATE, &GlobalUserInfo, sizeof(tagGlobalUserInfo));
		}

		return true;
	}
	case SUB_CS_C_COLLECT_USER:		//�����û�	
	{
		//��ȡ����
		WORD wBindIndex = LOWORD(dwSocketID);
		tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

		//��������
		WORD wBuffSize = 0;
		BYTE cbBuffer[SOCKET_PACKET];

		//�û���Ϣ		
		auto pUserInfoItem = (tagGlobalUserInfo*)cbBuffer;
		
		//��������
		POSITION Position = NULL;
		CGlobalUserInfo* pGlobalUserInfo= NULL;

		do
		{
			//ö���û�
			pGlobalUserInfo = m_GlobalInfoManager.EnumUserInfo(Position);
			if (pGlobalUserInfo == NULL) break;

			//�����ж�
			if (wBuffSize + sizeof(tagGlobalUserInfo) >= sizeof(cbBuffer))
			{
				m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_USER_COLLECT, SUB_CS_S_USER_LIST, cbBuffer, wBuffSize, NULL, 0);

				//��������
				wBuffSize = 0;
				pUserInfoItem = (tagGlobalUserInfo*)cbBuffer;
			}

			//������Ϣ
			pUserInfoItem->dwUserID = pGlobalUserInfo->GetUserID();
			pUserInfoItem->dwGameID = pGlobalUserInfo->GetGameID();
			pUserInfoItem->cbGender = pGlobalUserInfo->GetGender();
			StringCchCopy(pUserInfoItem->szNickName,CountArray(pUserInfoItem->szNickName), pGlobalUserInfo->m_szNickName);	
			StringCchCopy(pUserInfoItem->szUserHeadUrl, CountArray(pUserInfoItem->szUserHeadUrl), pGlobalUserInfo->m_szUserHeadUrl);//kk jia

			//ͷ����Ϣ
			pUserInfoItem->wFaceID = pGlobalUserInfo->GetFaceID();
			pUserInfoItem->dwCustomID = pGlobalUserInfo->GetCustomID();

			//�û�״̬
			pUserInfoItem->cbIMUserStatus = pGlobalUserInfo->m_cbIMUserStatus;
			pUserInfoItem->cbCBUserStatus = pGlobalUserInfo->m_cbCBUserStatus;

			//������Ϣ
			pUserInfoItem->SocketInfo.wGateID = pGlobalUserInfo->GetGateID();
			pUserInfoItem->SocketInfo.dwTokenID = pGlobalUserInfo->GetTokenID();

			//״̬��Ϣ
			pUserInfoItem->cbShutUpStatus = pGlobalUserInfo->m_cbShutUpStatus;
			pUserInfoItem->dwShutUpOverTime = pGlobalUserInfo->m_dwShutUpOverTime;

			//���ñ���
			pUserInfoItem++;

			wBuffSize += sizeof(tagGlobalUserInfo);

		} while (Position != NULL);

		//�����ж�
		if (wBuffSize > 0)
		{
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_USER_COLLECT, SUB_CS_S_USER_LIST, cbBuffer, wBuffSize, NULL, 0);
		}

		//�������
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_USER_COLLECT, SUB_CS_S_COLLECT_FINISH, cbBuffer, wBuffSize, NULL, 0);

		return true;
	}
	case SUB_CS_C_COLLECT_FINISH:	//�������
	{
		//��ȡ����
		WORD wBindIndex=LOWORD(dwSocketID);
		tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

		//���ñ���
		m_wCollectItem=INVALID_WORD;

		//�����л�
		if (m_WaitCollectItemArray.GetCount()>0)
		{
			//�л�����
			INT_PTR nWaitCount=m_WaitCollectItemArray.GetCount();
			m_wCollectItem=m_WaitCollectItemArray[nWaitCount-1];

			//ɾ������
			m_WaitCollectItemArray.RemoveAt(nWaitCount-1);

			//������Ϣ
			DWORD dwSocketID=(m_pBindParameter+m_wCollectItem)->dwSocketID;
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_USER_COLLECT,SUB_CS_S_COLLECT_USER,NULL,0);
		}

		return true;
	}
	}

	return false;
}

//��Ϣ��ѯ
bool CAttemperEngineSink::OnTCPNetworkMainQueryService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch(wSubCmdID)
	{
	case SUB_CS_C_QUERY_BY_GAMEID:		//������ϷID��ѯ
		{
			return OnTCPNetwordSubQueryByGameID(pData,wDataSize,dwSocketID);
		}
	case SUB_CS_C_QUERY_BY_ACCOUNTS:	//������Ϸ�ʻ���ѯ
		{
			return OnTCPNetwordSubQueryByAccounts(pData,wDataSize,dwSocketID);
		}
	}

	return false;
}

//��ѯ�û�
bool CAttemperEngineSink::OnTCPNetwordSubQueryByGameID(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	//Ч��
	ASSERT(wDataSize==sizeof(CMD_CS_C_QueryByGameID));
	if( wDataSize != sizeof(CMD_CS_C_QueryByGameID) ) return false;

	//��������
	CMD_CS_C_QueryByGameID *pQueryByGameID = (CMD_CS_C_QueryByGameID *)pData;

	//�����û�
	POSITION pos = NULL;
	CGlobalUserInfo *pGlobalUserInfo = m_GlobalInfoManager.EnumUserInfo(pos);
	while(pGlobalUserInfo)
	{
		//�ҵ��û�
		if( pGlobalUserInfo->GetGameID() == pQueryByGameID->dwQueryGameID )
		{
			//��������
			BYTE cbBuffer[SOCKET_PACKET+sizeof(CMD_CS_S_QueryUserResult)+sizeof(tagGameServerItem)] = {0};

			//������Ϣ
			CMD_CS_S_QueryUserResult *pQueryResult = (CMD_CS_S_QueryUserResult *)cbBuffer;
			pQueryResult->dwSendUserID = pQueryByGameID->dwUserID;
			pQueryResult->dwUserID = pGlobalUserInfo->GetUserID();
			pQueryResult->dwGameID = pGlobalUserInfo->GetGameID();
			StringCchCopy( pQueryResult->szAccounts,CountArray(pQueryResult->szAccounts), pGlobalUserInfo->GetNickName());

			//������Ϣ
			WORD wBaseSize = sizeof(CMD_CS_S_QueryUserResult)-sizeof(pQueryResult->szAccounts)+CountStringBuffer(pQueryResult->szAccounts);
			CSendPacketHelper SendPacket(cbBuffer+wBaseSize,sizeof(cbBuffer)-wBaseSize);			

			//��������
			CGlobalServerInfo* pGlobalServerInfo = pGlobalUserInfo->GetBindServerInfo();
			if (pGlobalServerInfo != NULL)
			{
				tagGameServerItem ServerItem={0};
				ServerItem.wServerID = pGlobalServerInfo->m_GameServer.wServerID;
				ServerItem.wServerPort = pGlobalServerInfo->m_GameServer.wServerPort;
				ServerItem.dwServerAddr = pGlobalServerInfo->m_GameServer.dwServerAddr;
				StringCchCopy(ServerItem.szServerName, CountArray(ServerItem.szServerName), pGlobalServerInfo->m_GameServer.szServerName);

				WORD wSize = sizeof(ServerItem) - sizeof(ServerItem.szServerName) + CountStringBuffer(ServerItem.szServerName);
				if (!SendPacket.AddPacket(&ServerItem, wSize, DTP_GR_SERVER_INFO)) break;
			}			

			WORD wSendSize = wBaseSize+SendPacket.GetDataSize();
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_QUERY_SERVICE,SUB_CS_S_QUERY_USER_RESULT,cbBuffer,wSendSize,NULL,0);

			return true;
		}

		if( !pos ) break;
		pGlobalUserInfo = m_GlobalInfoManager.EnumUserInfo(pos);
	}

	//�Ҳ����û�,����֪ͨ
	CMD_CS_S_QueryNotFound NotFound = {0};
	NotFound.dwSendUserID = pQueryByGameID->dwUserID;
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_QUERY_SERVICE,SUB_CS_S_QUERY_NOT_FOUND,&NotFound,sizeof(NotFound),NULL,0);

	return true;
}

//��ѯ�û�
bool CAttemperEngineSink::OnTCPNetwordSubQueryByAccounts(VOID *pData, WORD wDataSize, DWORD dwSocketID)
{
	//��������
	CMD_CS_C_QueryByAccounts *pQueryByAccounts = (CMD_CS_C_QueryByAccounts *)pData;

	//Ч��
	ASSERT(wDataSize == sizeof(CMD_CS_C_QueryByAccounts)-sizeof(pQueryByAccounts->szQueryAccounts)+CountStringBuffer(pQueryByAccounts->szQueryAccounts));
	if( wDataSize != sizeof(CMD_CS_C_QueryByAccounts)-sizeof(pQueryByAccounts->szQueryAccounts)+CountStringBuffer(pQueryByAccounts->szQueryAccounts) ) 
		return false;

	//�����û�
	POSITION pos = NULL;
	CGlobalUserInfo *pGlobalUserInfo = m_GlobalInfoManager.EnumUserInfo(pos);
	while( pGlobalUserInfo )
	{
		//�ҵ��û�
		CW2W strAccount(pGlobalUserInfo->GetNickName());
		_wcslwr_s(strAccount,sizeof(strAccount));
		if( lstrcmp((LPCTSTR)strAccount,pQueryByAccounts->szQueryAccounts) == 0 )
		{
			//��������
			BYTE cbBuffer[SOCKET_PACKET] = {0};

			//������Ϣ
			CMD_CS_S_QueryUserResult *pQueryResult = (CMD_CS_S_QueryUserResult *)cbBuffer;
			pQueryResult->dwSendUserID = pQueryByAccounts->dwUserID;
			pQueryResult->dwUserID = pGlobalUserInfo->GetUserID();
			pQueryResult->dwGameID = pGlobalUserInfo->GetGameID();
			StringCchCopy( pQueryResult->szAccounts,CountArray(pQueryResult->szAccounts), pGlobalUserInfo->GetNickName());

			//������Ϣ
			WORD wBaseSize = sizeof(CMD_CS_S_QueryUserResult)-sizeof(pQueryResult->szAccounts)+CountStringBuffer(pQueryResult->szAccounts);
			CSendPacketHelper SendPacket(cbBuffer+wBaseSize,sizeof(cbBuffer)-wBaseSize);
			
			//��������
			CGlobalServerInfo* pGlobalServerInfo = pGlobalUserInfo->GetBindServerInfo();
			if (pGlobalServerInfo != NULL)
			{
				tagGameServerItem ServerItem = { 0 };
				ServerItem.wServerID = pGlobalServerInfo->m_GameServer.wServerID;
				ServerItem.wServerPort = pGlobalServerInfo->m_GameServer.wServerPort;
				ServerItem.dwServerAddr = pGlobalServerInfo->m_GameServer.dwServerAddr;
				StringCchCopy(ServerItem.szServerName, CountArray(ServerItem.szServerName), pGlobalServerInfo->m_GameServer.szServerName);

				WORD wSize = sizeof(ServerItem) - sizeof(ServerItem.szServerName) + CountStringBuffer(ServerItem.szServerName);
				if (!SendPacket.AddPacket(&ServerItem, wSize, DTP_GR_SERVER_INFO)) break;
			}

			WORD wSendSize = wBaseSize + SendPacket.GetDataSize();
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_QUERY_SERVICE,SUB_CS_S_QUERY_USER_RESULT,cbBuffer,wSendSize,NULL,0);

			return true;
		}

		if( !pos ) break;
		pGlobalUserInfo = m_GlobalInfoManager.EnumUserInfo(pos);
	}

	//�Ҳ����û�,����֪ͨ
	CMD_CS_S_QueryNotFound NotFound = {0};
	NotFound.dwSendUserID = pQueryByAccounts->dwUserID;
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_QUERY_SERVICE,SUB_CS_S_QUERY_NOT_FOUND,&NotFound,sizeof(NotFound),NULL,0);

	return true;
}

//�������
bool CAttemperEngineSink::OnDBConfigFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//�¼�֪ͨ
	CP_ControlResult ControlResult;
	ControlResult.cbSuccess = ER_SUCCESS;
	SendUIControlPacket(UI_CONFIG_RESULT, &ControlResult, sizeof(ControlResult));

	return true;
}

//�������
bool CAttemperEngineSink::OnDBUnlockPlayer(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��������
	auto pUnlockPlayer = (DBO_GP_UnlockPlayer*)pData;

	//�������
	CMD_CS_S_OperateResult OperateResult;
	ZeroMemory(&OperateResult, sizeof(OperateResult));

	//���ñ���
	OperateResult.wRequestCmdID = SUB_CS_C_UNLOCK_PLAYER;

	//���ͽ��
	WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
	WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);
	
	//��������
	m_pITCPNetworkEngine->SendData(dwContextID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize+wMsgSize, NULL, 0);

	return true;
}

//���°�
bool CAttemperEngineSink::OnDBUpdateRankingList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//������Ϣ
	SendDataToAllService(SERVICE_MODULE_LOGON,0, MDM_CS_FORWARD_SERVICE, SUB_CS_RANKING_LIST_UPDATE, pData, wDataSize);

	return true;
}

//���ͷ���
bool CAttemperEngineSink::SendServiceListItem(WORD wServiceModule, DWORD dwSocketID, bool bPushRouteTable)
{
	//��������
	WORD wPacketSize = 0L;
	POSITION Position = NULL;
	BYTE cbBuffer[SOCKET_PACKET];

	//�ռ�����
	do
	{
		//��ȡ����
		tagServiceItem * pServiceItem = (tagServiceItem *)(cbBuffer + wPacketSize);
		CGlobalServiceInfo * pGlobalServiceInfo = m_GlobalInfoManager.EnumServiceInfo(wServiceModule,Position);

		//��������
		if (pGlobalServiceInfo != NULL)
		{
			wPacketSize += sizeof(tagServiceItem);
			CopyMemory(pServiceItem, &pGlobalServiceInfo->m_ServiceItem, sizeof(tagServiceItem));
		}

	} while (Position != NULL);

	//��������
	if (wPacketSize > 0)
	{
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVICE_INSERT, cbBuffer, wPacketSize,NULL,0);
	}

	//����·�ɱ�
	if (bPushRouteTable == true)
	{
		//���ñ���
		Position = NULL;

		//ö�ٱ���
		POSITION Position1 = NULL;
		tagRouteItem* pRouteItem = NULL;		
		auto pAppendRouteItem=(CMD_CS_S_AppendRouteItem*)cbBuffer;

		//�ռ�����
		do
		{
			//��ȡ����
			auto pGlobalServiceInfo = m_GlobalInfoManager.EnumServiceInfo(wServiceModule, Position);
			if (pGlobalServiceInfo != NULL)
			{	
				//���ñ���
				Position1 = NULL;
				wPacketSize = sizeof(CMD_CS_S_AppendRouteItem);
				pAppendRouteItem->bTheFirst = true;
				pAppendRouteItem->wServiceID = pGlobalServiceInfo->GetServiceID();
				pAppendRouteItem->wServiceModule = pGlobalServiceInfo->GetServiceModule();

				do
				{
					pRouteItem=pGlobalServiceInfo->EnumRouteItem(Position1);
					if (pRouteItem == NULL) break;

					//��������
					if (pRouteItem->cbRouteState == ROUTE_STATE_SERVICE)
					{
						CopyMemory(&cbBuffer[wPacketSize], pRouteItem, sizeof(tagRouteItem));
						wPacketSize += sizeof(tagRouteItem);
					}

					//�Ƴ��ж�
					if (wPacketSize + sizeof(tagRouteItem) > CountArray(cbBuffer))
					{
						m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_APPEND_ROUTE_ITEM, cbBuffer, wPacketSize, NULL, 0);

						//���ñ���
						pAppendRouteItem->bTheFirst = false;
						wPacketSize = sizeof(CMD_CS_S_AppendRouteItem);						
					}

				} while (Position1!=NULL);

				//�����ж�
				if (wPacketSize > sizeof(CMD_CS_S_AppendRouteItem))
				{
					m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_APPEND_ROUTE_ITEM, cbBuffer, wPacketSize, NULL, 0);					
				}
			}

		} while (Position != NULL);
	}

	return true;
}

//���ͷ���
bool CAttemperEngineSink::PushServiceListItem(WORD wServiceMask, DWORD dwSocketID, bool bPushRouteTable)
{
	//���ͷ����б�
	if (wServiceMask & SERVICE_MASK_GAME) SendServerListItem(dwSocketID);
	if (wServiceMask & SERVICE_MASK_CHAT) SendServiceListItem(SERVICE_MODULE_CHAT, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_SYNC) SendServiceListItem(SERVICE_MODULE_SYNC, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_CLUB) SendServiceListItem(SERVICE_MODULE_CLUB, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_QUERY) SendServiceListItem(SERVICE_MODULE_QUERY, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_LOGON) SendServiceListItem(SERVICE_MODULE_LOGON, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_BATTLE) SendServiceListItem(SERVICE_MODULE_BATTLE, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_MATCH) SendServiceListItem(SERVICE_MODULE_MATCH, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_STORE) SendServiceListItem(SERVICE_MODULE_STORE, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_MESSAGE) SendServiceListItem(SERVICE_MODULE_MESSAGE, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_RANKING) SendServiceListItem(SERVICE_MODULE_RANKING, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_LOGGER) SendServiceListItem(SERVICE_MODULE_LOGGER, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_MONITOR) SendServiceListItem(SERVICE_MODULE_MONITOR, dwSocketID, bPushRouteTable);
	if (wServiceMask & SERVICE_MASK_CLUBPLAZA) SendServiceListItem(SERVICE_MODULE_CLUBPLAZA, dwSocketID, bPushRouteTable);

	return true;
}

//�����б�
bool CAttemperEngineSink::SendServerListItem(DWORD dwSocketID)
{
	//��������
	WORD wPacketSize=0L;
	POSITION Position=NULL;
	BYTE cbBuffer[SOCKET_PACKET];

	//������Ϣ
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_COMMON_SERVICE,SUB_CS_S_SERVER_INFO,NULL,0);

	//�ռ�����
	do
	{
		//��������
		if ((wPacketSize+sizeof(tagGameServer))>sizeof(cbBuffer))
		{
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_COMMON_SERVICE,SUB_CS_S_SERVER_INSERT,cbBuffer,wPacketSize,NULL,0);
			wPacketSize=0;
		}

		//��ȡ����
		auto pGameServer=(tagGameServer *)(cbBuffer+wPacketSize);
		auto pGlobalServerInfo=(CGlobalServerInfo *)m_GlobalInfoManager.EnumServiceInfo(SERVICE_MODULE_GAME,Position);

		//��������
		if (pGlobalServerInfo!=NULL)
		{
			wPacketSize+=sizeof(tagGameServer);
			CopyMemory(pGameServer,&pGlobalServerInfo->m_GameServer,sizeof(tagGameServer));
		}

	} while (Position!=NULL);

	//��������
	if (wPacketSize>0) 
	{
		m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_COMMON_SERVICE,SUB_CS_S_SERVER_INSERT,cbBuffer,wPacketSize,NULL,0);
	}

	//�������
	m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_COMMON_SERVICE,SUB_CS_S_SERVER_FINISH,NULL,0);

	return true;
}

//���ͷ���
bool CAttemperEngineSink::SendServiceAttrib(DWORD dwSocketID)
{
	//��������
	WORD wIndex = 0;
	WORD wBufferSize = 0L;
	BYTE cbBuffer[SOCKET_PACKET] = {0};

	do
	{
		//ö�ٷ���
		auto pServiceAttrib = CServiceAttribManager::EnumServiceAttrib(wIndex++);
		if (pServiceAttrib == NULL) break;		

		//��������
		CopyMemory(&cbBuffer[wBufferSize], pServiceAttrib, sizeof(tagServiceAttrib));
		wBufferSize += sizeof(tagServiceAttrib);

	} while(true);	

	//��������
	m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVICE_ATTRIB, cbBuffer, wBufferSize, NULL, 0);

	return true;
}

//���Ͳʳ�
bool CAttemperEngineSink::SendGameJackpot(DWORD dwSocketID)
{
	//��������
	POSITION Position = NULL;
	CGlobalMonitorInfo* pGlobalServiceInfo = NULL;

	//���嶨��
	WORD wBuffSize = 0;
	BYTE cbBuffer[SOCKET_PACKET];

	do
	{
		pGlobalServiceInfo = (CGlobalMonitorInfo*)m_GlobalInfoManager.EnumServiceInfo(SERVICE_MODULE_MONITOR, Position);
		if (pGlobalServiceInfo == NULL) break;

		//��������
		auto pGlobalGameStock = pGlobalServiceInfo->m_StockGameStock.GetHeadStockItem();
		while (pGlobalGameStock)
		{
			auto pCurrentGameStock = pGlobalGameStock;
			pGlobalGameStock = pGlobalServiceInfo->m_StockGameStock.GetNextStockItem(pCurrentGameStock);

			//��ȡ����
			auto pGameJackpot = (CMD_CS_S_GameJackpot*)&cbBuffer[wBuffSize];

			//���ñ���
			pGameJackpot->wKindID = pCurrentGameStock->m_wKindID;
			pGameJackpot->lJackpotScore = pCurrentGameStock->m_lJackpotScore;

			//�����С
			wBuffSize += sizeof(CMD_CS_S_GameJackpot);
		}		

	} while (Position != NULL);

	//��������
	if (wBuffSize > 0)
	{
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_GAME_JACKPOT, cbBuffer, wBuffSize, NULL, 0);
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

//��������
bool CAttemperEngineSink::SendDataToAllService(WORD wServiceModule, WORD wExcludeID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	POSITION Position = NULL;
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;

	do
	{
		pGlobalServiceInfo= m_GlobalInfoManager.EnumServiceInfo(wServiceModule, Position);
		if (pGlobalServiceInfo==NULL) break;

		//У���ʶ
		if (pGlobalServiceInfo->GetServiceID() != wExcludeID)
		{	
			DWORD dwSocketID = pGlobalServiceInfo->GetSocketID();
			m_pITCPNetworkEngine->SendData(dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize, NULL, 0);
		}

	} while (Position != NULL);

	return true;
}

//��������
bool CAttemperEngineSink::SendDataToService(WORD wServiceModule, WORD wServiceID, WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	//���ҷ���
	CGlobalServiceInfo* pGlobalServiceInfo = m_GlobalInfoManager.SearchServiceInfo(wServiceModule, wServiceID);
	if (pGlobalServiceInfo == NULL) return false;

	//��������
	DWORD dwSocketID = pGlobalServiceInfo->GetSocketID();
	m_pITCPNetworkEngine->SendData(dwSocketID, wMainCmdID, wSubCmdID, pData, wDataSize, NULL, 0);

	return true;
}

//��Ϣ����
bool CAttemperEngineSink::SendDataToService(WORD wServiceModule, tagForwardItem& ForwardItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//ӳ�����
	if (ForwardItem.wServiceID == INVALID_WORD)
	{
		//ӳ�����
		auto pGlobalServiceInfo = m_GlobalInfoManager.LookupService(wServiceModule, ForwardItem.wServiceKind, ForwardItem.dwHashKeyID);
		if (pGlobalServiceInfo == NULL)
		{
			CTraceService::TraceString(TEXT("MapService��ServiceModule:%d,ServiceKind=%d,HashKeyID=%d"),TraceLevel_Exception, wServiceModule, ForwardItem.wServiceKind, ForwardItem.dwHashKeyID);
			return false;
		}

		SendDataToService(wServiceModule, pGlobalServiceInfo->GetServiceID() , wMainCmdID, wSubCmdID, pData, wDataSize);
	}	
	else if (ForwardItem.wServiceID == 0)
	{
		SendDataToAllService(wServiceModule, ForwardItem.wExcludeID, wMainCmdID, wSubCmdID, pData, wDataSize);
	}
	else
	{
		SendDataToService(wServiceModule, ForwardItem.wServiceID, wMainCmdID, wSubCmdID, pData, wDataSize);
	}	

	return true;
}

//�û�����
bool CAttemperEngineSink::SendDataToUser(WORD wServerID, DWORD dwUserID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//���·���
VOID CAttemperEngineSink::UpdateServerItem(CGlobalServerInfo* pGlobalServerInfo, BYTE cbUpdateKind)
{
	//����ṹ
	DBR_GP_UpdateServerList  UpdateServerList;
	ZeroMemory(&UpdateServerList, sizeof(UpdateServerList));

	//���ñ���
	UpdateServerList.cbUpdateKind = cbUpdateKind;

	//��ʽ������
	CStringW strServerItem;
	FormatServerItem(pGlobalServerInfo, strServerItem);
	_snwprintf_s(UpdateServerList.szServerList, CountArray(UpdateServerList.szServerList), L"[%s]", strServerItem.GetString());	

	//��������
	WORD wDataSize = CountStringBufferW(UpdateServerList.szServerList);
	WORD wHeadSize = sizeof(UpdateServerList) - sizeof(UpdateServerList.szServerList);
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_SERVER_LIST, 0L, 0L, 0L, &UpdateServerList, wHeadSize + wDataSize);
}

//��ʽ������
VOID CAttemperEngineSink::FormatServerItem(CGlobalServerInfo* pGlobalServerInfo, CStringW& strServerItem)
{
	//��������
	LPCWSTR pszServerFormat = L"{\"ServerID\":%d,\"ServerName\":\"%s\",\"ServerRule\":%d,\"OnlineCount\":%d,\"MaxConnect\":%d}";

	//��ʽ��
	auto pGameServer = &pGlobalServerInfo->m_GameServer;
	strServerItem.Format(pszServerFormat, pGameServer->wServerID, CT2CW(pGameServer->szServerName).m_psz, pGameServer->dwMasterRule, pGameServer->dwOnLineCount, pGameServer->dwMaxPlayer);	
}

//���´���
VOID CAttemperEngineSink::UploadAgentList()
{
	//��������
	POSITION Position = NULL;
	CMapPortOnline MapPortOnline;	
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;
	
	do
	{
		//ö������
		CGlobalServiceInfo * pGlobalServiceInfo = m_GlobalInfoManager.EnumServiceInfo(SERVICE_MODULE_GATE, Position);
		if (pGlobalServiceInfo != NULL)
		{
			MapPortOnline[pGlobalServiceInfo->m_ServiceItem.wServicePort] = pGlobalServiceInfo->m_wOnlineCount;
		}

	} while (Position != NULL);

	//��������
	Position = NULL;
	WORD wOnlineCount = 0;
	tagPortPair * pPortPair = NULL;
	tagServiceItem * pServiceItem = NULL;
	CGlobalAgentInfo * pGlobalAgentItem = NULL;

	//��������
	CStringA strAgentList, strMappedList;
	CStringA strAgentItem, strMappedItem;

	//��������
	LPCSTR pszAgentFormat = "{\"GateID\":%d,\"ServiceDomain\":\"%ls\",\"MappedList\":[%s],\"OnlineCount\":%d,\"MaxConnect\":%d}";
	LPCSTR pszMappedFormat = "{\"ServicePort\":%d,\"ForwardPort\":%d,\"OnlineCount\":%d}";

	//�ռ�����
	do
	{
		//ö������
		pGlobalAgentItem = (CGlobalAgentInfo *)m_GlobalInfoManager.EnumServiceInfo(SERVICE_MODULE_AGENT, Position);
		if (pGlobalAgentItem == NULL) break;
		if (pGlobalAgentItem->GetServicePortCount() == 0) continue;
		if (pGlobalAgentItem->m_AgentItem.wAgentKind != AGENT_KIND_GATE) continue;
		
		//�������
		strMappedList.Empty();

		//�˿�ӳ���б�
		for (int i = 0; i < pGlobalAgentItem->GetServicePortCount(); i++)
		{
			//�˿ڶ�
			pPortPair=&pGlobalAgentItem->m_PortPairArray[i];

			//��������
			wOnlineCount=0;
			MapPortOnline.Lookup(pPortPair->wForwardPort,wOnlineCount);

			//��ӷָ���
			if (strMappedList.IsEmpty()==false) strMappedList.Append(",");

			//��ʽ�ַ���
			strMappedItem.Format(pszMappedFormat,pPortPair->wServicePort,pPortPair->wForwardPort,wOnlineCount);
			strMappedList.Append(strMappedItem);			
		}

		//��ӷָ���
		if (strAgentList.IsEmpty()==false) strAgentList.Append(",");

		//������Ϣ
		pServiceItem=&pGlobalAgentItem->m_ServiceItem;
		strAgentItem.Format(pszAgentFormat, pServiceItem->wServiceID, pServiceItem->szServiceDomain, strMappedList, pGlobalAgentItem->m_wOnlineCount, pGlobalAgentItem->m_ServiceItem.wMaxConnect);
		strAgentList.Append(strAgentItem);

		//��ӷָ���
	} while (Position != NULL);

	//�������
	strAgentList.Insert(0,"[");
	strAgentList.Append("]");

	//����ṹ
	DBR_UpdateGateList UpdateGateList;
	ZeroMemory(&UpdateGateList, sizeof(UpdateGateList));

	//���ñ���
	StringCchCopyA(UpdateGateList.szGateList, CountArray(UpdateGateList.szGateList), strAgentList);

	//��������
	WORD wDataSize = CountStringBufferA(UpdateGateList.szGateList);
	WORD wHeadSize = sizeof(UpdateGateList) - sizeof(UpdateGateList.szGateList);
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_GATE_LIST, 0L, 0L, 0L, &UpdateGateList, wHeadSize + wDataSize);
}

//�ϴ�����
VOID CAttemperEngineSink::UpdateGateList()
{
	//��������
	POSITION Position = NULL;	
	tagServiceItem * pServiceItem = NULL;
	CGlobalServiceInfo * pGlobalServiceInfo = NULL;

	//��������
	CStringA strAgentList;
	CStringA strAgentItem, strMappedItem;

	//��������
	LPCSTR pszAgentFormat = "{\"GateID\":%d,\"ServiceDomain\":\"%ls\",\"ServicePort\":%d,\"OnlineCount\":%d,\"MaxConnect\":%d}";	

	//�ռ�����
	do
	{
		//ö������
		pGlobalServiceInfo = m_GlobalInfoManager.EnumServiceInfo(SERVICE_MODULE_GATE, Position);
		if (pGlobalServiceInfo == NULL) break;

		//������Ϣ
		pServiceItem = &pGlobalServiceInfo->m_ServiceItem;

		//��ӷָ���
		if (strAgentList.IsEmpty()==false) strAgentList.Append(",");

		//������Ϣ		
		strAgentItem.Format(pszAgentFormat, pServiceItem->wServiceID, pServiceItem->szServiceDomain, pServiceItem->wServicePort, pGlobalServiceInfo->m_wOnlineCount, pGlobalServiceInfo->m_ServiceItem.wMaxConnect);
		strAgentList.Append(strAgentItem);

		//��ӷָ���
	} while (Position != NULL);

	//�������
	strAgentList.Insert(0, "[");
	strAgentList.Append("]");	

	//����ṹ
	DBR_UpdateGateList UpdateGateList;
	ZeroMemory(&UpdateGateList, sizeof(UpdateGateList));

	//���ñ���
	StringCchCopyA(UpdateGateList.szGateList, CountArray(UpdateGateList.szGateList), strAgentList);

	//��������
	WORD wDataSize = CountStringBufferA(UpdateGateList.szGateList);
	WORD wHeadSize = sizeof(UpdateGateList) - sizeof(UpdateGateList.szGateList);
	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UPDATE_GATE_LIST, 0L,0L, 0L, &UpdateGateList, wHeadSize + wDataSize);
}

//�㲥��Ϣ
bool CAttemperEngineSink::OnTCPNetworkMainBroadcast(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{	
	switch (wSubCmdID)
	{
	case SUB_CS_C_GAME_BROADCAST:
		{
			//��Ϣ����
			CMD_Buffer * pBuffer=(CMD_Buffer *)pData;

			//Ч������
			ASSERT(wDataSize>=(sizeof(pBuffer->Head)));
			ASSERT(wDataSize==(sizeof(CMD_Buffer)-sizeof(pBuffer->cbBuffer)+pBuffer->Head.CmdInfo.wPacketSize));

			//��ȡ����
			WORD wBindIndex=LOWORD(dwSocketID);
			tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

			//����Ч��
			ASSERT(pBindParameter->wServiceModule==SERVICE_MODULE_GAME);
			if (pBindParameter->wServiceModule != SERVICE_MODULE_GAME) return false;

			//��������
			for (int i=0; i<m_pInitParameter->m_wMaxConnect; i++)
			{
				//��ȡ����
				pBindParameter=(m_pBindParameter+i);
				if(pBindParameter!=NULL && pBindParameter->wServiceModule != SERVICE_MODULE_GAME)
					continue;

				//��������
				m_pITCPNetworkEngine->SendData(pBindParameter->dwSocketID,MDM_CS_BROADCAST,SUB_CS_C_GAME_BROADCAST,pData,wDataSize,NULL,0);

			}
			break;
		}
	}
	
	return true;
}

//��վ֪ͨ
bool CAttemperEngineSink::OnTCPNetworkMasterService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	CString str;
	str.Format(L"[WEB] CMD:%d Size:%d",wSubCmdID,wDataSize);
	CTraceService::TraceString(str,enTraceLevel::TraceLevel_Debug);

	switch (wSubCmdID)
	{
	case SUB_CS_C_MODIFY_SERVER:		//�޸ķ���
	{	
		//У������
		ASSERT(wDataSize == sizeof(CMD_CS_C_ModifyServer));
		if (wDataSize != sizeof(CMD_CS_C_ModifyServer)) return true;

		//��ȡ����
		auto pModifyServer = (CMD_CS_C_ModifyServer*)pData;
		pModifyServer->szOperateID[CountArray(pModifyServer->szOperateID) - 1] = 0;

		//Ȩ�޹���
		pModifyServer->dwAppendRule &= (MR_FORBID_ENTER_ROOM);
		pModifyServer->dwRemoveRule &= (MR_FORBID_ENTER_ROOM);

		//��������	
		auto pGlobalServerInfo = (CGlobalServerInfo *)m_GlobalInfoManager.SearchServiceInfo(SERVICE_MODULE_GAME, pModifyServer->wServerID);
		if (pGlobalServerInfo != NULL)
		{
			//����Ȩ��
			if (pModifyServer->dwAppendRule != 0)
			{
				pGlobalServerInfo->m_GameServer.dwMasterRule|= pModifyServer->dwAppendRule;
			}

			if (pModifyServer->dwRemoveRule != 0)
			{
				pGlobalServerInfo->m_GameServer.dwMasterRule &= ~pModifyServer->dwRemoveRule;
			}

			//У��Ȩ��
			if (pModifyServer->dwAppendRule != 0 || pModifyServer->dwRemoveRule != 0)
			{
				//����ṹ
				CMD_CS_S_ServerModify ServerModify;
				ZeroMemory(&ServerModify, sizeof(ServerModify));

				//���ñ���
				ServerModify.wKindID = pGlobalServerInfo->m_GameServer.wKindID;
				ServerModify.wSortID = pGlobalServerInfo->m_GameServer.wSortID;
				ServerModify.wNodeID = pGlobalServerInfo->m_GameServer.wNodeID;
				ServerModify.wServerID = pGlobalServerInfo->m_GameServer.wServerID;
				ServerModify.wServerPort = pGlobalServerInfo->m_GameServer.wServerPort;
				ServerModify.dwServerRule = pGlobalServerInfo->m_GameServer.dwServerRule;
				ServerModify.dwMasterRule = pGlobalServerInfo->m_GameServer.dwMasterRule;
				ServerModify.dwMaxPlayer = pGlobalServerInfo->m_GameServer.dwMasterRule;
				ServerModify.dwMaxPlayer = pGlobalServerInfo->m_GameServer.dwMaxPlayer;
				ServerModify.wServerLevel = pGlobalServerInfo->m_GameServer.wServerLevel;
				ServerModify.dwOnLineCount = pGlobalServerInfo->m_GameServer.dwOnLineCount;
				StringCchCopy(ServerModify.szServerName, CountArray(ServerModify.szServerName), pGlobalServerInfo->m_GameServer.szServerName);
				StringCchCopy(ServerModify.szServerDomain, CountArray(ServerModify.szServerDomain), pGlobalServerInfo->m_GameServer.szServerDomain);	

				//����֪ͨ
				SendDataToAllService(SERVICE_MODULE_GATE,0, MDM_CS_COMMON_SERVICE, SUB_CS_S_SERVER_MODIFY, &ServerModify, sizeof(ServerModify));
			}

			//���·���
			UpdateServerItem(pGlobalServerInfo, UPDATE_KIND_MODIFY);

			//�������
			CMD_CS_S_OperateResult OperateResult;
			ZeroMemory(&OperateResult,sizeof(OperateResult));

			//���ñ���
			OperateResult.wRequestCmdID = SUB_CS_C_MODIFY_SERVER;
			StringCchCopy(OperateResult.szOperateID,CountArray(OperateResult.szOperateID), pModifyServer->szOperateID);

			//������Ϣ
			WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
			WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);
		}		
		else
		{
			//�������
			CMD_CS_S_OperateResult OperateResult;
			ZeroMemory(&OperateResult, sizeof(OperateResult));

			//���ñ���
			OperateResult.wRequestCmdID = SUB_CS_C_MODIFY_SERVER;
			StringCchCopy(OperateResult.szOperateID, CountArray(OperateResult.szOperateID), pModifyServer->szOperateID);

			//�����Ϣ
			OperateResult.lResultCode = -1;
			StringCchCopy(OperateResult.szErrorDescribe,CountArray(OperateResult.szErrorDescribe),TEXT("The room does not exist."));

			//������Ϣ
			WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
			WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);
		}

		return true;
	}
	case SUB_CS_C_COLLECT_SERVER:		//�ռ�����
	{
		return true;
	}
	case SUB_CS_C_UNLOCK_PLAYER:		//�������
	{
		//У������
		ASSERT(wDataSize == sizeof(CMD_CS_C_UnlockPlayer));
		if (wDataSize != sizeof(CMD_CS_C_UnlockPlayer)) return true;

		//��ȡ����
		auto pUnlockPlayer = (CMD_CS_C_UnlockPlayer*)pData;
		
		//��ѯ�û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pUnlockPlayer->dwUserID);
		if (pGlobalUserInfo != NULL && pGlobalUserInfo->GetBindServerInfo()!=NULL)
		{
			//�������
			CMD_CS_S_OperateResult OperateResult;
			ZeroMemory(&OperateResult, sizeof(OperateResult));

			//���ñ���
			OperateResult.wRequestCmdID = SUB_CS_C_UNLOCK_PLAYER;

			//�������
			OperateResult.lResultCode = 1;
			StringCchCopy(OperateResult.szErrorDescribe, CountArray(OperateResult.szErrorDescribe), TEXT("The player is currently in the game and is not allowed to unlock."));

			//��������			
			WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
			WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize+wMsgSize, NULL, 0);
		}
		else
		{
			//����ṹ
			DBR_GP_UnlockPlayer UnlockPlayer;
			ZeroMemory(&UnlockPlayer,sizeof(UnlockPlayer));

			//���ñ���
			UnlockPlayer.dwUserID = pUnlockPlayer->dwUserID;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GP_UNLOCK_PLAYER, UnlockPlayer.dwUserID, dwSocketID,0, &UnlockPlayer, sizeof(UnlockPlayer));
		}		

		return true;
	}
	case SUB_CS_C_KICKOUT_PLAYER:		//�߳����
	{
		//У������
		ASSERT(wDataSize <= sizeof(CMD_CS_C_KickoutPlayer));
		if (wDataSize > sizeof(CMD_CS_C_KickoutPlayer)) return true;

		//��ȡ����
		auto pKickoutPlayer = (CMD_CS_C_KickoutPlayer*)pData;

		//��ѯ�û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pKickoutPlayer->dwUserID);
		if (pGlobalUserInfo != NULL && pGlobalUserInfo->GetBindLogonInfo() != NULL)
		{
			//��¼����
			auto pGlobalLogonInfo = pGlobalUserInfo->GetBindLogonInfo();

			//����ṹ
			CMD_CS_LogoutNotify LogoutNotify;

			//���ñ���
			LogoutNotify.dwUserID = pKickoutPlayer->dwUserID;			
			StringCchCopy(LogoutNotify.szNotifyContent, CountArray(LogoutNotify.szNotifyContent), CWHService::FormatMessage(MIM_LOGON_0012, TEXT("��Ǹ�����ѱ������ߣ�")));

			//������Ϣ
			SendDataToService(SERVICE_MODULE_LOGON, pGlobalLogonInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_LOGOUT_NOTIFY, &LogoutNotify, sizeof(LogoutNotify));
		}
		
		//�������
		CMD_CS_S_OperateResult OperateResult;
		ZeroMemory(&OperateResult, sizeof(OperateResult));

		//���ñ���
		OperateResult.wRequestCmdID = SUB_CS_C_KICKOUT_PLAYER;

		//�������
		OperateResult.lResultCode = 0;
		StringCchCopy(OperateResult.szErrorDescribe, CountArray(OperateResult.szErrorDescribe), TEXT("Kicked out successful."));

		//��������			
		WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
		WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);

		return true;
	}
	case SUB_CS_C_USER_MAIL_NOTIFY:		//�ʼ�֪ͨ
	{
		//У������
		ASSERT (wDataSize%sizeof(CMD_CS_MailNotify)==0);
		if (wDataSize % sizeof(CMD_CS_MailNotify) != 0) return false;

		//��ȡ����
		auto pMailNotify = (CMD_CS_MailNotify*)pData;
		WORD wItemCount = wDataSize / sizeof(CMD_CS_MailNotify);
		
		//ת����Ϣ
		SendDataToAllService(SERVICE_MODULE_MESSAGE,0,MDM_CS_FORWARD_SERVICE, SUB_CS_MAIL_NOTIFY, pData, wDataSize);

		return true;
	}
	case SUB_CS_C_USER_MAIL_INSERT:		//�ʼ�����
	{
		//У������
		ASSERT(wDataSize % sizeof(CMD_CS_MailInsert) == 0);
		if (wDataSize % sizeof(CMD_CS_MailInsert) != 0) return false;

		//�������
		CMD_CS_S_OperateResult OperateResult;
		ZeroMemory(&OperateResult, sizeof(OperateResult));

		//���ñ���
		OperateResult.wRequestCmdID = SUB_CS_C_USER_MAIL_INSERT;

		//У������
		if (m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_MESSAGE)>0)
		{
			//����ṹ
			tagForwardItem ForwardItem;
			ZeroMemory(&ForwardItem, sizeof(ForwardItem));

			//���ñ���
			ForwardItem.wServiceID = INVALID_WORD;
			ForwardItem.dwHashKeyID = time(NULL);

			//ת����Ϣ
			SendDataToService(SERVICE_MODULE_MESSAGE, ForwardItem, MDM_CS_FORWARD_SERVICE, SUB_CS_MAIL_INSERT, pData, wDataSize);

			//�������
			OperateResult.lResultCode = 0;
			StringCchCopy(OperateResult.szErrorDescribe, CountArray(OperateResult.szErrorDescribe), TEXT("The server has received the request and the email is being sent."));
		}
		else
		{
			//�������
			OperateResult.lResultCode = 1;
			StringCchCopy(OperateResult.szErrorDescribe, CountArray(OperateResult.szErrorDescribe), TEXT("The system is not registered to process emails."));			
		}		

		//���ͽ��
		WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
		WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);

		return true;
	}
	case SUB_CS_C_USER_INFO_UPDATE:		//��Ϣ����
	{
		//У������
		ASSERT(wDataSize == sizeof(CMD_CS_C_UserInfoUpdate));
		if (wDataSize != sizeof(CMD_CS_C_UserInfoUpdate)) return false;

		//��ȡ����
		auto pUserInfoUpdate = (CMD_CS_C_UserInfoUpdate*)pData;		
		
		return true;
	}
	case SUB_CS_C_SHUTDOWN_ACCOUNT:		//�ر��˺�
	{
		//У������
		ASSERT(wDataSize <= sizeof(CMD_CS_C_ShutDownAccount));
		if (wDataSize > sizeof(CMD_CS_C_ShutDownAccount)) return false;

		//��ȡ����
		auto pShutDownAccount = (CMD_CS_C_ShutDownAccount*)pData;		

		//�����û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pShutDownAccount->dwUserID);
		if (pGlobalUserInfo != NULL)
		{
			//����ṹ
			CMD_CS_LogoutNotify LogoutNotify;

			//���ñ���
			LogoutNotify.dwUserID = pShutDownAccount->dwUserID;
			StringCchCopy(LogoutNotify.szNotifyContent, CountArray(LogoutNotify.szNotifyContent), CWHService::FormatMessage(MIM_LOGON_0011, TEXT("��Ǹ�������˺��ѱ����ᣡ")));

			//����Ϸ
			if (pGlobalUserInfo->GetBindServerInfo() != NULL)
			{
				//��¼����
				auto pGlobalServerInfo = pGlobalUserInfo->GetBindServerInfo();

				//������Ϣ
				SendDataToService(SERVICE_MODULE_GAME, pGlobalServerInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_LOGOUT_NOTIFY, &LogoutNotify, sizeof(LogoutNotify));
			}

			//�ڴ���
			if (pGlobalUserInfo->GetBindLogonInfo() != NULL)
			{
				//��¼����
				auto pGlobalLogonInfo = pGlobalUserInfo->GetBindLogonInfo();

				//������Ϣ
				SendDataToService(SERVICE_MODULE_LOGON, pGlobalLogonInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_LOGOUT_NOTIFY, &LogoutNotify, sizeof(LogoutNotify));
			}

		}

		return true;
	}
	case SUB_CS_C_WEALTH_UPDATE:		//�Ƹ�����
	{
		//У������
		ASSERT(wDataSize == sizeof(CMD_CS_C_WealthUpdate));
		if (wDataSize != sizeof(CMD_CS_C_WealthUpdate)) return false;

		//��ȡ����
		auto pWealthUpdate = (CMD_CS_C_WealthUpdate*)pData;

		//�����û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pWealthUpdate->dwUserID);
		if (pGlobalUserInfo != NULL && pGlobalUserInfo->GetBindLogonInfo() != NULL)
		{
			//��¼����
			auto pGlobalLogonInfo = pGlobalUserInfo->GetBindLogonInfo();

			//����ṹ
			CMD_CS_UserWealthUpdate UserWealthUpdate;

			//���ñ���
			UserWealthUpdate.dwUserID = pWealthUpdate->dwUserID;

			//������Ϣ
			SendDataToService(SERVICE_MODULE_LOGON, pGlobalLogonInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_USER_WEALTH_UPDATE, &UserWealthUpdate, sizeof(UserWealthUpdate));
		}

		return true;
	}	
	case SUB_CS_C_RECHARGE_NOTICE:		//��ֵ֪ͨ
	{
		//У������
		ASSERT(wDataSize <= sizeof(CMD_CS_C_RechargeNotice));
		if (wDataSize > sizeof(CMD_CS_C_RechargeNotice)) return false;

		//��ȡ����
		auto pRechargeNotice = (CMD_CS_C_RechargeNotice*)pData;

		//�����û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pRechargeNotice->dwUserID);
		if (pGlobalUserInfo != NULL)
		{
			//����ṹ
			CMD_CS_UserRechargeNotice UserRechargeNotice;

			//���ñ���
			UserRechargeNotice.dwUserID = pRechargeNotice->dwUserID;
			StringCchCopy(UserRechargeNotice.szGoodsList, CountArray(UserRechargeNotice.szGoodsList), pRechargeNotice->szGoodsList);

			//����Ϸ
			if (pGlobalUserInfo->GetBindServerInfo() != NULL)
			{
				//��¼����
				auto pGlobalServerInfo = pGlobalUserInfo->GetBindServerInfo();		

				//������Ϣ
				SendDataToService(SERVICE_MODULE_GAME, pGlobalServerInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_USER_RECHARGE_NOTICE, &UserRechargeNotice, sizeof(UserRechargeNotice));
			}
			//�ڴ���
			if (pGlobalUserInfo->GetBindLogonInfo() != NULL)
			{
				//��¼����
				auto pGlobalLogonInfo = pGlobalUserInfo->GetBindLogonInfo();

				//��������
				if (pRechargeNotice->bUpdateMask==true)
				{
					//��������
					CMD_CS_UpdateActionMask UpdateActionMask;
					ZeroMemory(&UpdateActionMask, sizeof(UpdateActionMask));

					//���ñ���
					UpdateActionMask.dwUserID = pRechargeNotice->dwUserID;
					UpdateActionMask.dwActionMaskEver = pRechargeNotice->dwActionMaskEver;
					UpdateActionMask.dwActionMaskPerDay = pRechargeNotice->dwActionMaskPerDay;
					UpdateActionMask.dwActionMaskPerWeek = pRechargeNotice->dwActionMaskPerWeek;

					//������Ϣ
					SendDataToService(SERVICE_MODULE_LOGON, pGlobalLogonInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_UPDATE_ACTION_MASK, &UpdateActionMask, sizeof(UpdateActionMask));
				}

				//��ֵ֪ͨ
				if (pGlobalUserInfo->GetBindServerInfo() == NULL)
				{
					//������Ϣ
					SendDataToService(SERVICE_MODULE_LOGON, pGlobalLogonInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_USER_RECHARGE_NOTICE, &UserRechargeNotice, sizeof(UserRechargeNotice));
				}
			}
		}

		return true;
	}	
	case SUB_CS_C_MODIFY_SHUTUP_STATUS:	//�޸�״̬
	{
		//У������
		ASSERT(wDataSize == sizeof(CMD_CS_C_ModifyShutUpStatus));
		if (wDataSize != sizeof(CMD_CS_C_ModifyShutUpStatus)) return false;

		//��ȡ����
		auto pModifyShutUpStatus = (CMD_CS_C_ModifyShutUpStatus*)pData;

		//����ṹ
		CMD_CS_UserShutUpStatus UserShutUpStatus;
		ZeroMemory(&UserShutUpStatus,sizeof(UserShutUpStatus));

		//����״̬
		UserShutUpStatus.dwUserID = pModifyShutUpStatus->dwUserID;
		UserShutUpStatus.cbShutUpStatus = pModifyShutUpStatus->cbShutUpStatus;
		UserShutUpStatus.dwShutUpOverTime = pModifyShutUpStatus->dwShutUpOverTime;

		//�����û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pModifyShutUpStatus->dwUserID);
		if (pGlobalUserInfo != NULL)
		{
			//����״̬
			pGlobalUserInfo->m_cbShutUpStatus = pModifyShutUpStatus->cbShutUpStatus;
			pGlobalUserInfo->m_dwShutUpOverTime = pModifyShutUpStatus->dwShutUpOverTime;

			//֪ͨ��¼
			if (pGlobalUserInfo->GetBindLogonInfo() != NULL)
			{
				//��¼����
				auto pGlobalLogonInfo = pGlobalUserInfo->GetBindLogonInfo();

				//������Ϣ
				SendDataToService(SERVICE_MODULE_LOGON, pGlobalLogonInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_USER_SHUTUP_STATUS, pData, wDataSize);
			}

			//֪ͨ����
			if (pGlobalUserInfo->GetBindServerInfo() != NULL)
			{
				//��¼����
				auto pGlobalServerInfo = pGlobalUserInfo->GetBindServerInfo();

				//������Ϣ
				SendDataToService(SERVICE_MODULE_GAME, pGlobalServerInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, SUB_CS_USER_SHUTUP_STATUS, pData, wDataSize);
			}
		}

		//�㲥��Ϣ		
		SendDataToAllService(SERVICE_MODULE_CHAT,0, MDM_CS_FORWARD_SERVICE, SUB_CS_USER_SHUTUP_STATUS, pData, wDataSize);

		return true;
	}
	case SUB_CS_C_RELOAD_GAME_CONFIG:	//��������
	{
		//��ȡ����
		auto pReloadGameConfig = (CMD_CS_C_ReloadGameConfig*)pData;

		//����ṹ
		tagExecuteCommand ExecuteCommand = {};
		
		//���ñ���
		ExecuteCommand.dwCommandCode = CMD_LOAD_CONFIG_DATA;
		ExecuteCommand.dwCommandParam = pReloadGameConfig->dwConfigMask;

		CTraceService::TraceString(TEXT("ServiceMask=%d,ConfigMask=%d"), TraceLevel_Exception, pReloadGameConfig->dwServiceMask, pReloadGameConfig->dwConfigMask);

		//ת����Ϸ
		if (pReloadGameConfig->dwServiceMask & SERVICE_MASK_GAME)
		{
			SendDataToAllService(SERVICE_MODULE_GAME,0, MDM_CS_MANAGER_SERVICE, SUB_CS_S_EXECUTE_COMMAND, &ExecuteCommand, sizeof(ExecuteCommand));
		}

		//ת����¼
		if (pReloadGameConfig->dwServiceMask & SERVICE_MASK_LOGON)
		{
			SendDataToAllService(SERVICE_MODULE_LOGON,0, MDM_CS_MANAGER_SERVICE, SUB_CS_S_EXECUTE_COMMAND, &ExecuteCommand, sizeof(ExecuteCommand));
		}

		//ת��Լս
		if (pReloadGameConfig->dwServiceMask & SERVICE_MASK_BATTLE)
		{
			SendDataToAllService(SERVICE_MODULE_BATTLE,0, MDM_CS_MANAGER_SERVICE, SUB_CS_S_EXECUTE_COMMAND, &ExecuteCommand, sizeof(ExecuteCommand));
		}

		//ת����λ
		if (pReloadGameConfig->dwServiceMask & SERVICE_MASK_RANKING)
		{
			SendDataToAllService(SERVICE_MODULE_RANKING,0, MDM_CS_MANAGER_SERVICE, SUB_CS_S_EXECUTE_COMMAND, &ExecuteCommand, sizeof(ExecuteCommand));
		}

		//ת���������
		if (pReloadGameConfig->dwServiceMask & SERVICE_MASK_CLUB)
		{
			SendDataToAllService(SERVICE_MODULE_CLUB, 0, MDM_CS_MANAGER_SERVICE, SUB_CS_S_EXECUTE_COMMAND, &ExecuteCommand, sizeof(ExecuteCommand));
		}

		//ת������㳡
		if (pReloadGameConfig->dwServiceMask & SERVICE_MASK_CLUBPLAZA)
		{
			SendDataToAllService(SERVICE_MODULE_CLUBPLAZA, 0, MDM_CS_MANAGER_SERVICE, SUB_CS_S_EXECUTE_COMMAND, &ExecuteCommand, sizeof(ExecuteCommand));
		}

		return true;
	}
	case SUB_CS_C_APPEND_FILTER_WORDS:	//��Ӵʻ�
	{
		//��ȡ����
		auto pAppendFilterWords = (CMD_CS_C_AppendFilterWords*)pData;

		//����ṹ
		CMD_CS_FilterWordsUpdate FilterWordsUpdate;
		ZeroMemory(&FilterWordsUpdate,sizeof(FilterWordsUpdate));

		//���ñ���
		FilterWordsUpdate.bDeleted = false;
		StringCchCopy(FilterWordsUpdate.szFilterWords,CountArray(FilterWordsUpdate.szFilterWords), pAppendFilterWords->szFilterWords);

		//ת������
		SendDataToAllService(SERVICE_MODULE_CHAT,0, MDM_CS_FORWARD_SERVICE, SUB_CS_FILTER_WORDS_UPDATE, &FilterWordsUpdate, sizeof(FilterWordsUpdate));

		//ת����¼
		SendDataToAllService(SERVICE_MODULE_LOGON,0, MDM_CS_FORWARD_SERVICE, SUB_CS_FILTER_WORDS_UPDATE, &FilterWordsUpdate, sizeof(FilterWordsUpdate));

		return true;
	}
	case SUB_CS_C_DELETE_FILTER_WORDS:	//ɾ���ʻ�
	{
		//��ȡ����
		auto pDeleteFilterWords = (CMD_CS_C_DeleteFilterWords*)pData;

		//����ṹ
		CMD_CS_FilterWordsUpdate FilterWordsUpdate;
		ZeroMemory(&FilterWordsUpdate, sizeof(FilterWordsUpdate));

		//���ñ���
		FilterWordsUpdate.bDeleted = true;
		StringCchCopy(FilterWordsUpdate.szFilterWords, CountArray(FilterWordsUpdate.szFilterWords), pDeleteFilterWords->szFilterWords);

		//ת������
		SendDataToAllService(SERVICE_MODULE_CHAT,0,MDM_CS_FORWARD_SERVICE, SUB_CS_FILTER_WORDS_UPDATE, &FilterWordsUpdate, sizeof(FilterWordsUpdate));

		//ת����¼
		SendDataToAllService(SERVICE_MODULE_LOGON,0,MDM_CS_FORWARD_SERVICE, SUB_CS_FILTER_WORDS_UPDATE, &FilterWordsUpdate, sizeof(FilterWordsUpdate));

		return true;
	}
	case SUB_CS_C_APPEND_USER_CONTROL:  //��ӿ���
	{
		//��ȡ����
		auto pAppendUserControl = (CMD_CS_C_AppendUserControl*)pData;
		
		//�����û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pAppendUserControl->dwUserID);
		if (pGlobalUserInfo)
		{
			auto pBindServerInfo = pGlobalUserInfo->GetBindServerInfo();
			if (pBindServerInfo && pBindServerInfo->m_GameServer.wServerType&GAME_GENRE_GOLD)
			{
				//����ṹ
				CMD_CS_UserControlAppend UserControlAppend;
				ZeroMemory(&UserControlAppend,sizeof(UserControlAppend));

				//���ñ�ʶ
				UserControlAppend.dwUserID = pAppendUserControl->dwUserID;

				//ת������
				SendDataToService(SERVICE_MODULE_GAME, pBindServerInfo->m_GameServer.wServerID, MDM_CS_FORWARD_SERVICE, SUB_CS_USER_CONTROL_APPEND, &UserControlAppend, sizeof(UserControlAppend));
			}
		}		

		return true;
	}
	case SUB_CS_C_REMOVE_USER_CONTROL:  //�Ƴ�����
	{
		//��ȡ����
		auto pRemoveUserControl = (CMD_CS_C_RemoveUserControl*)pData;

		//�����û�
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pRemoveUserControl->dwUserID);
		if (pGlobalUserInfo)
		{
			auto pBindServerInfo = pGlobalUserInfo->GetBindServerInfo();
			if (pBindServerInfo && pBindServerInfo->m_GameServer.wServerType & GAME_GENRE_GOLD)
			{
				//����ṹ
				CMD_CS_UserControlRemove RemoveUserControl;
				ZeroMemory(&RemoveUserControl, sizeof(RemoveUserControl));

				//���ñ�ʶ
				RemoveUserControl.dwUserID = pRemoveUserControl->dwUserID;

				//ת������
				SendDataToService(SERVICE_MODULE_GAME, pBindServerInfo->m_GameServer.wServerID, MDM_CS_FORWARD_SERVICE, SUB_CS_USER_CONTROL_REMOVE, &RemoveUserControl, sizeof(RemoveUserControl));
			}
		}

		return true;
	}
	case SUB_CS_C_APPEND_STOCK_ITEM:	//��ӿ��
	{
		//��ȡ����
		auto pAppendStockItem = (CMD_CS_C_AppendStockItem*)pData;

		//����ṹ
		CMD_CS_AppendStockItem  AppendStockItem = {};
		AppendStockItem.wStockID = pAppendStockItem->wStockID;
		StringCchCopy(AppendStockItem.szOperateID,CountArray(AppendStockItem.szOperateID), pAppendStockItem->szOperateID);

		//������Ϣ
		SendDataToAllService(SERVICE_MODULE_MONITOR,0, MDM_CS_FORWARD_SERVICE, SUB_CS_APPEND_STOCK_ITEM,&AppendStockItem,sizeof(AppendStockItem));

		//�������
		CMD_CS_S_OperateResult OperateResult = {};

		//���ñ���
		OperateResult.wRequestCmdID = SUB_CS_C_APPEND_STOCK_ITEM;
		OperateResult.lResultCode = m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_MONITOR) ? 0 : -1;
		StringCchCopy(OperateResult.szOperateID, CountArray(OperateResult.szOperateID), pAppendStockItem->szOperateID);

		//���ͽ��
		WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
		WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);

		//��������
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);
		
		return true;
	}
	case SUB_CS_C_DELETE_STOCK_ITEM:	//ɾ�����
	{
		//��ȡ����
		auto pDeleteStockItem = (CMD_CS_C_DeleteStockItem*)pData;

		//����ṹ
		CMD_CS_DeleteStockItem  DeleteStockItem = {};
		DeleteStockItem.wStockID = pDeleteStockItem->wStockID;
		StringCchCopy(DeleteStockItem.szOperateID, CountArray(DeleteStockItem.szOperateID), pDeleteStockItem->szOperateID);

		//������Ϣ
		SendDataToAllService(SERVICE_MODULE_MONITOR,0, MDM_CS_FORWARD_SERVICE, SUB_CS_DELETE_STOCK_ITEM, &DeleteStockItem, sizeof(DeleteStockItem));

		//�������
		CMD_CS_S_OperateResult OperateResult = {};

		//���ñ���
		OperateResult.wRequestCmdID = SUB_CS_C_DELETE_STOCK_ITEM;
		OperateResult.lResultCode = m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_MONITOR) ? 0 : -1;
		StringCchCopy(OperateResult.szOperateID, CountArray(OperateResult.szOperateID), pDeleteStockItem->szOperateID);

		//���ͽ��
		WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
		WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);

		//��������
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);
		
		return true;
	}
	case SUB_CS_C_MODIFY_STOCK_ITEM:	//�޸Ŀ��
	{
		//��ȡ����
		auto pModifyStockItem = (CMD_CS_C_ModifyStockItem*)pData;

		//����ṹ
		CMD_CS_ModifyStockItem  ModifyStockItem = {};
		ModifyStockItem.wStockID = pModifyStockItem->wStockID;
		StringCchCopy(ModifyStockItem.szOperateID, CountArray(ModifyStockItem.szOperateID), pModifyStockItem->szOperateID);

		//������Ϣ
		SendDataToAllService(SERVICE_MODULE_MONITOR,0, MDM_CS_FORWARD_SERVICE, SUB_CS_MODIFY_STOCK_ITEM, &ModifyStockItem, sizeof(ModifyStockItem));

		//�������
		CMD_CS_S_OperateResult OperateResult = {};

		//���ñ���
		OperateResult.wRequestCmdID = SUB_CS_C_MODIFY_STOCK_ITEM;
		OperateResult.lResultCode = m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_MONITOR) ? 0 : -1;
		StringCchCopy(OperateResult.szOperateID, CountArray(OperateResult.szOperateID), pModifyStockItem->szOperateID);

		//���ͽ��
		WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
		WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);

		//��������
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);		

		return true;
	}
	case SUB_CS_C_ADJUST_STOCK_SCORE:	//�������ֵ
	{
		//��ȡ����
		auto pAdjustStockScore = (CMD_CS_C_AdjustStockScore*)pData;

		//����ṹ
		CMD_CS_AdjustStockScore  AdjustStockScore = {};
		AdjustStockScore.wStockID = pAdjustStockScore->wStockID;
		AdjustStockScore.lChangeScore = pAdjustStockScore->lChangeScore;
		StringCchCopy(AdjustStockScore.szOperateID, CountArray(AdjustStockScore.szOperateID), pAdjustStockScore->szOperateID);

		//������Ϣ
		SendDataToAllService(SERVICE_MODULE_MONITOR,0, MDM_CS_FORWARD_SERVICE, SUB_CS_ADJUST_STOCK_SCORE, &AdjustStockScore, sizeof(AdjustStockScore));

		//�������
		CMD_CS_S_OperateResult OperateResult = {};

		//���ñ���
		OperateResult.wRequestCmdID = SUB_CS_C_ADJUST_STOCK_SCORE;
		OperateResult.lResultCode = m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_MONITOR) ? 0 : -1;
		StringCchCopy(OperateResult.szOperateID, CountArray(OperateResult.szOperateID), pAdjustStockScore->szOperateID);

		//���ͽ��
		WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
		WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);

		//��������
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);

		return true;
	}
	case SUB_CS_C_ADJUST_JACKPOT_SCORE: //�����ʽ�
	{
		//��ȡ����
		auto pAdjustJackpotScore = (CMD_CS_C_AdjustJackpotScore*)pData;

		//����ṹ
		CMD_CS_AdjustJackpotScore  AdjustJackpotScore = {};
		AdjustJackpotScore.wStockID = pAdjustJackpotScore->wStockID;
		AdjustJackpotScore.wLevelID = pAdjustJackpotScore->wLevelID;
		AdjustJackpotScore.lChangeScore = pAdjustJackpotScore->lChangeScore;
		StringCchCopy(AdjustJackpotScore.szOperateID, CountArray(AdjustJackpotScore.szOperateID), pAdjustJackpotScore->szOperateID);

		//������Ϣ
		SendDataToAllService(SERVICE_MODULE_MONITOR,0, MDM_CS_FORWARD_SERVICE, SUB_CS_ADJUST_JACKPOT_SCORE, &AdjustJackpotScore, sizeof(AdjustJackpotScore));

		//�������
		CMD_CS_S_OperateResult OperateResult = {};

		//���ñ���
		OperateResult.wRequestCmdID = SUB_CS_C_ADJUST_JACKPOT_SCORE;
		OperateResult.lResultCode = m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_MONITOR) ? 0 : -1;
		StringCchCopy(OperateResult.szOperateID, CountArray(OperateResult.szOperateID), pAdjustJackpotScore->szOperateID);

		//���ͽ��
		WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
		WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);

		//��������
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);
		
		return true;
	}
	case SUB_CS_C_UPDATE_SPLAYER_CONTROL://���´�R����
	{
		//��ȡ����
		auto pUpdateSPlayerControl = (CMD_CS_C_UpdateSPlayerControl*)pData;

		//������Ϣ
		SendDataToAllService(SERVICE_MODULE_MONITOR,0, MDM_CS_FORWARD_SERVICE, SUB_CS_UPDATE_SPLAYER_CONTROL, NULL, 0);

		//�������
		CMD_CS_S_OperateResult OperateResult = {};

		//���ñ���
		OperateResult.wRequestCmdID = SUB_CS_C_UPDATE_SPLAYER_CONTROL;
		OperateResult.lResultCode = m_GlobalInfoManager.GetServiceItemCount(SERVICE_MODULE_MONITOR) ? 0 : -1;
		StringCchCopy(OperateResult.szOperateID, CountArray(OperateResult.szOperateID), pUpdateSPlayerControl->szOperateID);

		//���ͽ��
		WORD wMsgSize = CountStringBuffer(OperateResult.szErrorDescribe);
		WORD wHeadSize = sizeof(OperateResult) - sizeof(OperateResult.szErrorDescribe);

		//��������
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_MASTER_SERVICE, SUB_CS_S_OPERATE_RESULT, &OperateResult, wHeadSize + wMsgSize, NULL, 0);

		return true;
	}
	case SUB_CS_C_PUBLISH_MARQUEE_NOTICE:  //��������
	{
		//��ȡ����
		auto pPublishMarqueeNotice = (CMD_CS_C_PublishMarqueeNotice*)pData;

		//����ṹ
		CMD_CS_MarqueeNotice MarqueeNotice;
		ZeroMemory(&MarqueeNotice,sizeof(MarqueeNotice));

		//��������
		MarqueeNotice.wRollTimes = 0;
		MarqueeNotice.cbPriority = 0;
		MarqueeNotice.cbNoticeKind = NOTICE_KIND_SYSTEM;

		//��������
		MarqueeNotice.dwNoticeID = pPublishMarqueeNotice->dwNoticeID;
		MarqueeNotice.bExclusive = pPublishMarqueeNotice->bExclusive;
		MarqueeNotice.wViewPlace = pPublishMarqueeNotice->wViewPlace;
		MarqueeNotice.dwEndRollTime = pPublishMarqueeNotice->dwEndRollTime;
		MarqueeNotice.dwStartRollTime = pPublishMarqueeNotice->dwStartRollTime;		

		//����ṹ
		auto pNoticeSystem= (tagNoticeKind_System*)MarqueeNotice.cbNoticeContent;
		StringCchCopy(pNoticeSystem->szNoticeContent, CountArray(pNoticeSystem->szNoticeContent), pPublishMarqueeNotice->szNoticeContent);

		//��������
		MarqueeNotice.wNoticeSize = CountStringBuffer(pNoticeSystem->szNoticeContent);
		MarqueeNotice.wNoticeSize += sizeof(tagNoticeKind_System)-sizeof(pNoticeSystem->szNoticeContent);
		CopyMemory(MarqueeNotice.cbNoticeContent, pNoticeSystem, MarqueeNotice.wNoticeSize);

		//��������
		WORD wPacketSize = sizeof(CMD_CS_MarqueeNotice) - sizeof(MarqueeNotice.cbNoticeContent) + MarqueeNotice.wNoticeSize;
		SendDataToAllService(SERVICE_MODULE_MESSAGE,0,MDM_CS_FORWARD_SERVICE, SUB_CS_MARQUEE_NOTICE, &MarqueeNotice, wPacketSize);

		return true;
	}
	}

	return true;
}

//�������
bool CAttemperEngineSink::OnTCPNetWorkMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_MODIFY_OPTION:		//�޸�ѡ��
	{
		auto pModifyOption = (CMD_CS_C_ModifyOption*)pData;

		//ѭ������
		for (int i = 0; i < pModifyOption->wServerCount; i++)
		{
			//ת������
			tagForwardItem ForwardItem;
			ZeroMemory(&ForwardItem, sizeof(ForwardItem));

			//���ñ�ʶ
			ForwardItem.wServiceID = pModifyOption->wServerIDList[i];

			//������Ϣ
			SendDataToService(SERVICE_MODULE_GAME, ForwardItem, MDM_CS_MANAGER_SERVICE, SUB_CS_S_MODIFY_OPTION, &pModifyOption->ModifyOption, sizeof(pModifyOption->ModifyOption));
		}

		return true;
	}
	case SUB_CS_C_EXECUTE_COMMAND:		//ִ������
	{
		m_pITCPNetworkEngine->SendDataBatch(MDM_CS_MANAGER_SERVICE, SUB_CS_S_EXECUTE_COMMAND, pData, wDataSize, 0L);

		return true;
	}
	}
	
	return true;
}

//ת������
bool CAttemperEngineSink::OnTCPNetworkMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID)
{
	//��ȡͷ��
	WORD wHeadSize = sizeof(tagForwardHead);
	auto pForwardHead = (tagForwardHead*)pData;

	//ת�����û�
	if (pForwardHead->wTarget == FORWARD_TARGET_USER)
	{
		//�û���Ϣ
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pForwardHead->dwUserID);
		if (pGlobalUserInfo != NULL)
		{
			//ת��˳��
			for (auto i = 0; i < pForwardHead->wItemCount; i++)
			{
				//��������
				CGlobalServiceInfo* pGlobalServiceInfo = NULL;				
				tagForwardItem* pForwardItem = &pForwardHead->ForwardItem[i];	

				//��¼����
				if (pForwardItem->wServiceModule == SERVICE_MODULE_LOGON && pGlobalUserInfo->GetBindLogonInfo() != NULL)
				{
					pGlobalServiceInfo = pGlobalUserInfo->GetBindLogonInfo();
				}

				//�������
				if (pForwardItem->wServiceModule == SERVICE_MODULE_GAME && pGlobalUserInfo->GetBindServerInfo() != NULL)
				{
					pGlobalServiceInfo = pGlobalUserInfo->GetBindServerInfo();
				}

				//��Ϣ����
				if (pForwardItem->wServiceModule == SERVICE_MODULE_MESSAGE && pGlobalUserInfo->GetBindMessageInfo() != NULL)
				{
					pGlobalServiceInfo = pGlobalUserInfo->GetBindMessageInfo();
				}				

				//�������ڷ���
				if (pGlobalServiceInfo != NULL)
				{
					SendDataToService(pForwardItem->wServiceModule, pGlobalServiceInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, wSubCmdID, pForwardHead + 1, wDataSize - wHeadSize);
				}
			}
		}	
	}

	//ת��������
	if (pForwardHead->wTarget == FORWARD_TARGET_SERVICE)
	{
		//�û���Ϣ
		auto pGlobalUserInfo = m_GlobalInfoManager.SearchUserInfo(pForwardHead->dwUserID);		

		//˳��ת��
		for (auto i = 0; i < pForwardHead->wItemCount; i++)
		{	
			//��������
			CGlobalServiceInfo* pGlobalServiceInfo = NULL;
			tagForwardItem* pForwardItem = &pForwardHead->ForwardItem[i];			

			//У���û�
			if (pGlobalUserInfo != NULL)
			{
				//��¼����
				if (pForwardItem->wServiceModule == SERVICE_MODULE_LOGON && pGlobalUserInfo->GetBindLogonInfo() != NULL)
				{
					pGlobalServiceInfo = pGlobalUserInfo->GetBindLogonInfo();
				}

				//�������
				if (pForwardItem->wServiceModule == SERVICE_MODULE_GAME && pGlobalUserInfo->GetBindServerInfo() != NULL)
				{
					pGlobalServiceInfo = pGlobalUserInfo->GetBindServerInfo();
				}

				//��Ϣ����
				if (pForwardItem->wServiceModule == SERVICE_MODULE_MESSAGE && pGlobalUserInfo->GetBindMessageInfo() != NULL)
				{
					pGlobalServiceInfo = pGlobalUserInfo->GetBindMessageInfo();
				}		

				//�������ڷ���
				if (pGlobalServiceInfo != NULL)
				{
					SendDataToService(pForwardItem->wServiceModule, pGlobalServiceInfo->GetServiceID(), MDM_CS_FORWARD_SERVICE, wSubCmdID, pForwardHead + 1, wDataSize - wHeadSize);
					continue;
				}				
			}

			//ӳ�����			
			SendDataToService(pForwardItem->wServiceModule, *pForwardItem, MDM_CS_FORWARD_SERVICE, wSubCmdID, pForwardHead + 1, wDataSize - wHeadSize);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
