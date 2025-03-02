#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//ʱ���ʶ
#define IDI_CONNECT_CENTER			1									//������ʶ
#define IDI_CONNECT_BATTLE			2									//����ʱ��
#define IDI_SERVICE_PULSE			3									//��������
#define IDI_DETECTION_TABLE			4									//�������

//////////////////////////////////////////////////////////////////////////
//λ������
#define LOCATION_MASK_INCLUB		0x01								//����
#define LOCATION_MASK_OUTCLUB		0x02								//����

//��������
#define FREE_TABLE_COUNT			1									//��������

//////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//���ñ���		
	m_bClubSevice=false;
	m_bNeedConnStatus=true;
	m_pClubOption=NULL;
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;		
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));	
	ZeroMemory(&m_BattleItem,sizeof(m_BattleItem));

	//�������
	m_pITimerEngine=NULL;
	m_pIAttemperEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;
	m_pITCPSocketBattle=NULL;

	//���Ա���
	m_MapInvokeCount1.InitHashTable(33);
	m_MapInvokeCount2.InitHashTable(33);

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
	m_pBindParameter=new tagBindParameter[m_pClubOption->wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pClubOption->wMaxConnect);

	//����״̬
	m_bClubSevice=true;

	//���Ա���
	m_MapInvokeCount1.RemoveAll();
	m_MapInvokeCount2.RemoveAll();

	//���ýӿ�
	m_GlobalServiceManager.SetIAttemperEngine(m_pIAttemperEngine);

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
	m_pITCPSocketBattle = NULL;
	ZeroMemory(&m_BattleItem, sizeof(m_BattleItem));

	//���Ա���
	m_MapInvokeCount1.RemoveAll();
	m_MapInvokeCount2.RemoveAll();

	//����״̬
	m_bClubSevice=false;
	m_bNeedConnStatus=true;

	//ɾ������
	SafeDeleteArray(m_pBindParameter);

	//�������	
	m_ClubUserManager.DeleteUserItem();
	m_ClubInfoManager.RemoveClubItem();

	//��������
	m_GlobalItemManager.ResetData();
	m_GlobalServiceManager.ResetData();

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{
	case CT_CONNECT_BATTLE:			//����Լս
	{
		//��������
		m_pITCPSocketBattle->Connect(m_BattleItem.szServiceDomain, m_BattleItem.wServicePort);

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڳ�������Լս������..."), TraceLevel_Normal);

		return true;
	}
	case CT_CONNECT_CENTER:		//��������
	{
		//�ر�ʱ��
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->KillTimer(IDI_CONNECT_CENTER);

		//��ȡ��ַ					
		tagAddressInfo * pAddressInfo=&m_pInitParameter->m_CenterAddress;

		//��������
		m_pITCPSocketCenter->Connect(htonl(pAddressInfo->dwServiceAddr), m_pInitParameter->m_wCenterPort);

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
	case IDI_CONNECT_BATTLE:	//����Լս
	{
		//��������
		m_pITCPSocketBattle->Connect(m_BattleItem.szServiceDomain, m_BattleItem.wServicePort);

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڳ�������%s..."), TraceLevel_Normal, m_BattleItem.szServiceName);

		return true;
	}
	case IDI_CONNECT_CENTER:	//��������
	{
		//��ȡ��ַ					
		auto pAddressInfo=&m_pInitParameter->m_CenterAddress;

		//��������
		m_pITCPSocketCenter->Connect(htonl(pAddressInfo->dwServiceAddr), m_pInitParameter->m_wCenterPort);

		//��ʾ��Ϣ
		CTraceService::TraceString(TEXT("���ڳ����������ķ�����..."),TraceLevel_Normal);

		return true;
	}
	case IDI_SERVICE_PULSE:		//��������
	{
		//��ȡʱ��
		SYSTEMTIME SystemTime;
		GetSystemTime(&SystemTime);

		//��ǰʱ��
		DWORD dwCurrentTime = (DWORD)time(NULL);

		//�賿3��-5����
		if (SystemTime.wHour >= 3 && SystemTime.wHour <= 5)
		{
			//��������			
			CClubInfoItem* pClubInfoItem = NULL;

			do
			{
				//ö�ٲ���
				auto pLastClubInfoItem = pClubInfoItem;
				pClubInfoItem = m_ClubInfoManager.EnumClubItem(pLastClubInfoItem);

				//У�����
				if (pClubInfoItem != NULL)
				{
					//ж�ش���
					if (pClubInfoItem->GetClubState() == CLUB_STATE_ONLINE && pClubInfoItem->GetOnlineCount() == 0 && (dwCurrentTime - pClubInfoItem->GetActiveTime() >= ONE_WEEK || pClubInfoItem->GetUnloadFlag()))
					{
						//����ṹ
						CMD_CS_C_UpdateRouteItem UpdateRouteItem = {};

						//���ñ���
						UpdateRouteItem.dwRouteID = pClubInfoItem->GetClubID();
						UpdateRouteItem.cbRouteState = ROUTE_STATE_NULL;

						//������Ϣ
						m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_UPDATE_ROUTE_ITEM, &UpdateRouteItem, sizeof(UpdateRouteItem), 0);

						//�Ƴ�Ⱥ��
						m_ClubInfoManager.RemoveClubItem(pClubInfoItem->GetClubID());
					}
				}

			} while (pClubInfoItem != NULL);
		}
	}
	case IDI_DETECTION_TABLE:	//�������
	{
		//��������			
		CClubInfoItem* pClubInfoItem = NULL;

		do
		{
			//ö�ٲ���
			auto pLastClubInfoItem = pClubInfoItem;
			pClubInfoItem = m_ClubInfoManager.EnumClubItem(pLastClubInfoItem);

			//У�����
			if (pClubInfoItem != NULL)
			{
				//ж�ش���
				if (pClubInfoItem->GetClubState() == CLUB_STATE_ONLINE)
				{
					//��������
					INT nIndex = 0;

					do
					{
						auto pBattleConfig = pClubInfoItem->EnumConfig(nIndex++);
						if (pBattleConfig == NULL) break;

						//У������
						while (pBattleConfig->wTableCount < FREE_TABLE_COUNT)
						{
							//��������
							pBattleConfig->wTableCount++;

							CTraceService::TraceString(TEXT("��������1"),TraceLevel_Debug);

							//��������
							CreateBattleTable(pClubInfoItem, pBattleConfig);
						}

					} while (true);
				}
			}

		} while (pClubInfoItem != NULL);		

		return true;
	}	
	}

	//������Ϣ
	WORD wServiceID = LOWORD(dwTimerID);
	WORD wServiceModule = HIWORD(dwTimerID);

	//Լս����
	if (wServiceModule == SERVICE_MODULE_BATTLE)
	{
		auto pGlobalServiceItem = m_GlobalServiceManager.SearchBattleService(wServiceID);
		if (pGlobalServiceItem != NULL && pGlobalServiceItem->m_pITCPSocketService != NULL)
		{
			//���ӷ���
			pGlobalServiceItem->m_pITCPSocketService->Connect(pGlobalServiceItem->m_ServiceItem.szServiceDomain, pGlobalServiceItem->m_ServiceItem.wServicePort);

			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("���ڳ�������[%d]��Լս������..."), TraceLevel_Normal, wServiceID);
		}

		return true;
	}	

	return false;
}

//Ӧ���¼�
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//��ȡ����
	ASSERT(LOWORD(dwSocketID)<m_pClubOption->wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pClubOption->wMaxConnect) return false;

	//��������
	WORD wBindIndex=LOWORD(dwSocketID);
	auto pBindParameter=(m_pBindParameter+wBindIndex);

	//���ñ���
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientIP=dwClientIP;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);	

	//������Ϣ
	pBindParameter->wServiceID = 0;
	pBindParameter->wServiceModule = 0;
	pBindParameter->pBindServiceItem = NULL;

	return true;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	DWORD dwCmdKey=MAKELONG(Command.wMainCmdID,Command.wSubCmdID);
	m_MapInvokeCount1[dwCmdKey]++;

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
	case MDM_CLUB_SERVICE:		//�������
		{
			OnTCPNetworkMainClubService(Command.wSubCmdID,pData,wDataSize,dwSocketID,dwTokenID);

			return true;
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
		m_GlobalItemManager.DeleteGateItem(pBindParameter->wServiceID);							
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
	case DBO_GR_CLUB_LIST:			//�����б�
	{
		return OnDBClubList(dwContextID, dwTokenID,pData,wDataSize);
	}
	case DBO_GR_CLUB_LIST_FINISH:	//�б����
	{
		return OnDBClubListFinish(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBO_GR_CLUB_ITEM:			//��������
	{
		return OnDBClubItem(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBO_GR_CLUB_ITEM_FINISH:	//�������
	{
		return OnDBClubItemFinish(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBO_GR_CLUB_MEMBER:		//�����Ա
	{
		return OnDBClubMember(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBO_GR_BATTLE_CONFIG:		//Լս�淨
	{
		return OnDBBattleConfig(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBO_GR_KIND_SETTING:		//��������
	{
		return OnDBKindSetting(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_MODIFY_SETTING:		//�޸�����
	{
		return OnDBKindSettingModify(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_APPLY_MESSAGE:		//������Ϣ
	{
		return OnDBApplyMessage(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_UPDATE_MEMBER:		//���³�Ա
	{
		return OnDBUpdateMember(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBO_GR_APPEND_CONFIG:		//����淨
	{
		return OnDBAppendConfig(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_MODIFY_CONFIG:		//�޸��淨
	{
		return OnDBModifyConfig(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_DELETE_CONFIG:		//ɾ���淨
	{
		return OnDBDeleteConfig(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_OPERATE_FAILURE:	//����ʧ��
	{
		return OnDBOperateFailure(dwContextID, dwTokenID, pData, wDataSize);
	}
	}

	return false;
}

//�ر��¼�
bool CAttemperEngineSink::OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason)
{
	if (dwServiceID==NETWORK_CENTER)
	{
		//������ʾ
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("�����ķ����������ӹر��ˣ�%ld �����������"),m_pInitParameter->m_wConnectCenter);

		//��ʾ��Ϣ
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);

		//����ʱ��
		ASSERT(m_pITimerEngine!=NULL);
		m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectCenter*1000L,1,0);

		return true;
	}

	//Լս����
	if (dwServiceID == NETWORK_BATTLE)
	{
		//������ʾ
		TCHAR szDescribe[128] = TEXT("");
		_sntprintf_s(szDescribe, CountArray(szDescribe), TEXT("��Լս�����������ӹر��ˣ�%ld �����������"), m_pInitParameter->m_wConnectBattle);

		//��ʾ��Ϣ
		CTraceService::TraceString(szDescribe, TraceLevel_Warning);

		//�ر�ʱ��
		ASSERT(m_pITimerEngine != NULL);
		m_pITimerEngine->KillTimer(IDI_DETECTION_TABLE);

		//����ʱ��
		ASSERT(m_pITimerEngine != NULL);
		m_pITimerEngine->SetTimer(IDI_CONNECT_BATTLE, m_pInitParameter->m_wConnectBattle * 1000L, 1, 0);		

		return true;
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
				nErrorCode,m_pInitParameter->m_wConnectCenter);

			//��ʾ��Ϣ
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//����ʱ��
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectCenter*1000L,1,0);

			return false;
		}

		//��������
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService,sizeof(RegisterService));

		//����ṹ
		RegisterService.wServiceID=m_pClubOption->wServiceID;
		RegisterService.wServiceModule=SERVICE_MODULE_CLUB;
		RegisterService.wServiceMask=SERVICE_MASK_BATTLE;
		RegisterService.wServicePort=m_pClubOption->wServicePort;
		StringCchCopy(RegisterService.szServiceName, CountArray(RegisterService.szServiceName), TEXT("���ֲ�����"));
		StringCchCopy(RegisterService.szServiceDomain, CountArray(RegisterService.szServiceDomain),m_pClubOption->szServiceDomain);

		//��������
		m_pITCPSocketCenter->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_SERVICE,&RegisterService,sizeof(RegisterService),0);

		return true;
	}

	//Լս����
	if (dwServiceID == NETWORK_BATTLE)
	{
		//�����ж�
		if (nErrorCode != 0)
		{
			//��ʾ��Ϣ
			CTraceService::TraceString(TEXT("Լս����������ʧ�� [ %ld ]��%ld �����������"), TraceLevel_Warning, nErrorCode, m_pInitParameter->m_wConnectBattle);

			//����ʱ��
			ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_BATTLE, m_pInitParameter->m_wConnectBattle * 1000L, 1, 0);

			return false;
		}

		//��������
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService, sizeof(RegisterService));

		//���ñ���	
		RegisterService.wServiceID = m_pClubOption->wServiceID;
		RegisterService.wServiceModule = SERVICE_MODULE_CLUB;
		RegisterService.wServicePort = m_pClubOption->wServicePort;
		RegisterService.wMaxConnect = m_pClubOption->wMaxConnect;
		RegisterService.wServiceKindID = m_pClubOption->wServiceKindID;
		StringCchCopy(RegisterService.szServiceDomain, CountArray(RegisterService.szServiceDomain), m_pClubOption->szServiceDomain);

		//��������
		m_pITCPSocketBattle->SendData(MDM_CS_REGISTER, SUB_CS_C_REGISTER_SERVICE, &RegisterService, sizeof(RegisterService), 0);
	}

	return false;
}

//��ȡ�¼�
bool CAttemperEngineSink::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD *pdwTokenID, WORD wTokenCount)
{
	//��������
	if (dwServiceID == NETWORK_CENTER)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:			//ע�����
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
		}
		case MDM_CS_USER_COLLECT:		//�û�����
		{
			return OnTCPSocketMainUserCollect(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_CS_COMMON_SERVICE:		//������Ϣ
		{
			return OnTCPSocketMainComonService(Command.wSubCmdID,pData,wDataSize);
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

		return true;
	}
	
	//Լս����
	if (dwServiceID == NETWORK_BATTLE)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:		//ע�����
		{
			return OnTCPSocketBattleMainRegister(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_CS_COMMON_SERVICE:	//ͨ�÷���
		{
			return OnTCPSocketBattleMainCommonService(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_BC_BATTLE_SERVICE:	//��������	
		{
			return OnTCPSocketBattleMainBattleService(Command.wSubCmdID, pData, wDataSize);
		}
		}

		return true;
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

			//����ʱ��
			ASSERT(m_pITimerEngine != NULL);
			m_pITimerEngine->SetTimer(IDI_SERVICE_PULSE, 60*1000L, -1, 0);						

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

//�û�����
bool CAttemperEngineSink::OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_USER_LIST:		//�û��б�
	{
		//У�����
		ASSERT(wDataSize % sizeof(tagGlobalUserInfo) == 0);
		if (wDataSize % sizeof(tagGlobalUserInfo) != 0) return true;

		//��ȡ����
		WORD wUserCount = wDataSize / sizeof(tagGlobalUserInfo);
		tagGlobalUserInfo* pUserItem = (tagGlobalUserInfo*)pData;

		//��������
		tagClubUserInfo UserInfo;
		ZeroMemory(&UserInfo, sizeof(UserInfo));

		//��������
		CClubUserItem* pClubUserItem = NULL;

		//�����û�
		for (WORD i = 0; i < wUserCount; i++)
		{
			//У��״̬
			if (pUserItem->cbCBUserStatus == US_CB_OFFLINE) continue;

			//�����û�
			pClubUserItem = m_ClubUserManager.SearchUserItem(pUserItem->dwUserID);
			if (pClubUserItem != NULL)
			{
				pClubUserItem->SetSocketInfo(pUserItem->SocketInfo);
				pClubUserItem->SetUserStatus(US_CB_ONLINE);
			}
			else
			{
				//�����û�
				UserInfo.dwUserID = pUserItem->dwUserID;
				UserInfo.cbUserStatus = US_CB_ONLINE;
				UserInfo.dwGameID = pUserItem->dwGameID;
				UserInfo.cbGender = pUserItem->cbGender;
				StringCchCopy(UserInfo.szNickName, CountArray(UserInfo.szNickName), pUserItem->szNickName);
				StringCchCopy(UserInfo.szUserHeadUrl, CountArray(UserInfo.szUserHeadUrl), pUserItem->szUserHeadUrl);//kk jia

				//ͷ����Ϣ
				UserInfo.wFaceID = pUserItem->wFaceID;
				UserInfo.dwCustomID = pUserItem->dwCustomID;

				//�����û�
				tagSocketInfo SocketInfo = {};
				/*CString s;
				s.Format(_T("�����û�1 szNickName:%s,szUserHeadUrl:%s"), pUserItem->szNickName, pUserItem->szUserHeadUrl);
				CTraceService::TraceString(s, TraceLevel_Exception);*/

				m_ClubUserManager.InsertUserItem(UserInfo, pUserItem->SocketInfo);
			}

			pUserItem++;
		}

		return true;
	}
	case SUB_CS_S_COLLECT_FINISH:	//�б����
	{
		//���嶨��
		BYTE cbBuffer[SOCKET_BUFFER];
		CClubInfoItem* pClubInfoItem = NULL;

		//��������
		tagRouteItem* pRouteItem = NULL;
		auto wPacketSize = sizeof(CMD_CS_C_AppendRouteItem);
		auto pAppendRouteItem = (CMD_CS_C_AppendRouteItem*)cbBuffer;

		//���ñ���			
		pAppendRouteItem->bTheLast = false;
		pAppendRouteItem->wServiceKind = m_pClubOption->wServiceKindID;
		pAppendRouteItem->wServiceModule = SERVICE_MODULE_CLUB;

		do
		{
			//ö�ٲ���
			auto pLastClubInfoItem = pClubInfoItem;
			pClubInfoItem = m_ClubInfoManager.EnumClubItem(pLastClubInfoItem);

			//У�����
			if (pClubInfoItem != NULL && pClubInfoItem->GetClubState()!=CLUB_STATE_OFFLINE)
			{
				if (wPacketSize + sizeof(tagRouteItem) > sizeof(cbBuffer))
				{
					//������Ϣ					
					m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_APPEND_ROUTE_ITEM, cbBuffer, wPacketSize, 0);

					//���ô�С		
					wPacketSize = sizeof(CMD_CS_C_AppendRouteItem);
				}

				//��ȡ����
				pRouteItem = (tagRouteItem*)&cbBuffer[wPacketSize];

				//·����Ϣ
				pRouteItem->dwRouteID = pClubInfoItem->GetClubID();
				pRouteItem->cbRouteState = pClubInfoItem->GetClubState()==CLUB_STATE_ONLINE?ROUTE_STATE_SERVICE:ROUTE_STATE_ACTIVE;

				//���Ӵ�С
				wPacketSize += sizeof(tagRouteItem);
			}

		} while (pClubInfoItem != NULL);

		//ʣ�෢��
		if (wPacketSize > sizeof(CMD_CS_C_AppendRouteItem))
		{
			//���ñ���
			pAppendRouteItem->bTheLast = true;	

			//������Ϣ					
			m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_APPEND_ROUTE_ITEM, cbBuffer, wPacketSize, 0);
		}

		return true;
	}
	}

	return true;
}

//�б��¼�
bool CAttemperEngineSink::OnTCPSocketMainComonService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{	
	case SUB_CS_S_SERVICE_INSERT:	//�������
	{
		//Ч�����
		ASSERT(wDataSize % sizeof(tagServiceItem) == 0);
		if (wDataSize % sizeof(tagServiceItem) != 0) return false;

		//��������
		WORD wItemCount = wDataSize / sizeof(tagServiceItem);
		tagServiceItem* pServiceItem = (tagServiceItem*)pData;

		//Լս����
		if (pServiceItem->wServiceModule == SERVICE_MODULE_BATTLE)
		{
			CopyMemory(&m_BattleItem, pServiceItem, sizeof(m_BattleItem));			
		}				

		return true;
	}
	case SUB_CS_S_SERVICE_REMOVE:	//״̬�Ƴ�
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(CMD_CS_S_ServiceRemove));
		if (wDataSize != sizeof(CMD_CS_S_ServiceRemove)) return false;

		//��ȡ����
		auto pServiceRemove = (CMD_CS_S_ServiceRemove*)pData;

		//Լս�Ƴ�
		if (pServiceRemove->wServiceModule == SERVICE_MODULE_BATTLE)
		{
			ZeroMemory(&m_BattleItem, sizeof(m_BattleItem));
		}		

		return true;
	}
	case SUB_CS_S_ACTIVE_ROUTE_ITEM: //����·��
	{
		//Ч�����
		ASSERT (wDataSize == sizeof(CMD_CS_S_ActiveRouteItem));
		if (wDataSize != sizeof(CMD_CS_S_ActiveRouteItem)) return false;

		//��ȡ����
		auto pActiveRouteItem = (CMD_CS_S_ActiveRouteItem*)pData;

		//���Ҳ���
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pActiveRouteItem->dwRouteID);
		if (pClubInfoItem != NULL)
		{
			//����״̬�򼴽�����
			if (pClubInfoItem->GetClubState() >= CLUB_STATE_ONLINE)
			{
				//����ṹ
				CMD_CS_C_UpdateRouteItem UpdateRouteItem = {};

				//���ñ���
				UpdateRouteItem.cbRouteState = ROUTE_STATE_SERVICE;
				UpdateRouteItem.dwRouteID = pClubInfoItem->GetClubID();

				//������Ϣ
				m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_UPDATE_ROUTE_ITEM, &UpdateRouteItem, sizeof(UpdateRouteItem), 0);				
			}
		}
		else
		{
			//�������
			tagClubItem ClubItem = {};
			ClubItem.ClubProperty.dwClubID = pActiveRouteItem->dwRouteID;

			//��������
			pClubInfoItem = m_ClubInfoManager.ActiveClubItem(ClubItem);
			if (pClubInfoItem != NULL)
			{
				//����״̬
				pClubInfoItem->SetClubState(CLUB_STATE_LOADING);

				//���ز���		
				DBR_GR_LoadClubItem LoadClubItem;
				ZeroMemory(&LoadClubItem, sizeof(LoadClubItem));

				//����ṹ
				LoadClubItem.bActiveRoute = true;
				LoadClubItem.dwClubID = pActiveRouteItem->dwRouteID;				

				//���س�Ա
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_CLUB_ITEM, LoadClubItem.dwClubID, 0, 0, &LoadClubItem, sizeof(LoadClubItem));
			}
		}		

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

			//ֹͣ����
			if (pCommandCode->dwCommandCode&CMD_STOP_CLUB_SERVICE)
			{
				//���ñ�ʶ
				m_bClubSevice=false;
			}

			//��������
			if (pCommandCode->dwCommandCode&CMD_START_CLUB_SERVICE)
			{
				//���ñ�ʶ
				m_bClubSevice=true;
			}

			//���ز���
			if (pCommandCode->dwCommandCode & CMD_RELOAD_CLUB_DATA)
			{
				//��������
				CClubInfoItem* pClubInfoItem = NULL;

				do
				{
					//ö�ٲ���
					auto pLastClubInfoItem = pClubInfoItem;
					pClubInfoItem = m_ClubInfoManager.EnumClubItem(pLastClubInfoItem);

					//У��״̬
					if (pClubInfoItem != NULL && pClubInfoItem->GetClubState() == CLUB_STATE_ONLINE)
					{
						//���ñ�־
						pClubInfoItem->SetUnloadFlag(true);						
					}

				} while (pClubInfoItem != NULL);
			}

			//��������
			if (pCommandCode->dwCommandCode&CMD_PRINT_CLUB_INFO)
			{
				//��������
				DWORD dwOnlineCount=0;
				DWORD dwTotalCount=m_ClubUserManager.GetUserItemCount();

				//��������
				DWORD dwEnumIndex=0;
				CClubUserItem * pClubUserItem=NULL;
				
				do
				{
					pClubUserItem=m_ClubUserManager.EnumUserItem(pClubUserItem);
					if (pClubUserItem==NULL) break;
					if (pClubUserItem->GetUserStatus()==US_CB_ONLINE) dwOnlineCount++;
				} while (true);

				CString strMessage;
				strMessage.Format(TEXT("��������[%d] ��ǰ����������[%d]"),dwTotalCount,dwOnlineCount);
				CTraceService::TraceString(strMessage,TraceLevel_Warning);

				//ɾ���û�
				DWORD dwCmdKey=0L;
				ULONGLONG ulInvokeCount=0;
				POSITION Position=m_MapInvokeCount1.GetStartPosition();
				while (Position!=NULL)
				{
					m_MapInvokeCount1.GetNextAssoc(Position,dwCmdKey,ulInvokeCount);					
					strMessage.Format(TEXT("Network: MCmdID[%d] SCmdID[%d] InvokeCount[%ld]"),LOWORD(dwCmdKey),HIWORD(dwCmdKey),ulInvokeCount);
					CTraceService::TraceString(strMessage,TraceLevel_Warning);
				}

				Position=m_MapInvokeCount2.GetStartPosition();
				while (Position!=NULL)
				{
					m_MapInvokeCount2.GetNextAssoc(Position,dwCmdKey,ulInvokeCount);					
					strMessage.Format(TEXT("Socket: MCmdID[%d] SCmdID[%d] InvokeCount[%ld]"),LOWORD(dwCmdKey),HIWORD(dwCmdKey),ulInvokeCount);
					CTraceService::TraceString(strMessage,TraceLevel_Warning);
				}
			}

			return true;
		}
	}

	return true;
}

//ת����Ϣ
bool CAttemperEngineSink::OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_CB_USER_ONLINE:			//�û�����
	{
		//����У��
		ASSERT(wDataSize == sizeof(tagGlobalUserInfo));
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
			StringCchCopy(pClubUserInfo->szNickName, CountArray(pClubUserInfo->szNickName), pGlobalUserInfo->szNickName);

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
			StringCchCopy(ClubUserInfo.szUserHeadUrl, CountArray(ClubUserInfo.szUserHeadUrl), pGlobalUserInfo->szUserHeadUrl);//kk jia

			CString s;
			s.Format(_T("�����û�2 szNickName:%s,szUserHeadUrl:%s"), pGlobalUserInfo->szNickName, pGlobalUserInfo->szUserHeadUrl);
			CTraceService::TraceString(s, TraceLevel_Exception);
			//�����û�
			m_ClubUserManager.InsertUserItem(ClubUserInfo, pGlobalUserInfo->SocketInfo);
		}

		return true;
	}
	case SUB_CS_CB_USER_OFFLINE:		//�û�����
	{
		//����У��
		ASSERT(wDataSize == sizeof(CMD_CS_CB_UserOffline));
		if (wDataSize != sizeof(CMD_CS_CB_UserOffline)) return false;

		//��ȡ����
		auto pUserOffline = (CMD_CS_CB_UserOffline*)pData;

		//���Ҷ���
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pUserOffline->dwUserID);
		if (pClubUserItem != NULL)
		{
			//����״̬
			pClubUserItem->SetUserStatus(US_CB_OFFLINE);

			//��������
			ZeroMemory(pClubUserItem->GetSocketInfo(),sizeof(tagSocketInfo));
		}	

		return true;
	}	
	case SUB_CS_CB_USER_UPDATE:			//�û�����
	{
		//����У��
		ASSERT(wDataSize == sizeof(tagGlobalUserInfo));
		if (wDataSize != sizeof(tagGlobalUserInfo)) return false;

		//��ȡ����
		auto pGlobalUserInfo = (tagGlobalUserInfo*)pData;

		//���Ҷ���
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pGlobalUserInfo->dwUserID);
		if (pClubUserItem != NULL)
		{
			//�û���Ϣ
			auto pClubUserInfo = pClubUserItem->GetUserInfo();

			//������Ϣ
			pClubUserInfo->wFaceID = pGlobalUserInfo->wFaceID;
			pClubUserInfo->dwCustomID = pGlobalUserInfo->dwCustomID;						
			StringCchCopy(pClubUserInfo->szNickName, CountArray(pClubUserInfo->szNickName), pGlobalUserInfo->szNickName);			
		}

		return true;
	}
	case SUB_CS_CB_UPDATE_CLUB:			//�޸Ĳ���
	{
		//��ȡ����
		ASSERT(wDataSize >= sizeof(CMD_CS_CB_UpdateClub));
		if (wDataSize < sizeof(CMD_CS_CB_UpdateClub)) return false;

		//��ȡ����
		auto pUpdateClub = (CMD_CS_CB_UpdateClub*)pData;

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pUpdateClub->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//����ṹ
			CMD_CB_S_ClubUpdate* pClubUpdate = (CMD_CB_S_ClubUpdate*)m_cbBuffer;

			//���ñ���
			pClubUpdate->dwClubID = pUpdateClub->dwClubID;
			pClubUpdate->cbUpdateKind = pUpdateClub->cbUpdateKind;

			//���¹���
			if (pUpdateClub->cbUpdateKind == UPCLUB_KIND_RULES)
			{
				//�������
				tagClubRules ClubRules;
				CopyMemory(&ClubRules, pClubInfoItem->GetClubRules(), sizeof(ClubRules));

				//��������
				CopyMemory(pClubUpdate + 1, pUpdateClub + 1, sizeof(tagClubRules));

				//��������
				CopyMemory(pClubInfoItem->GetClubRules(), pUpdateClub + 1, sizeof(tagClubRules));

				//��������
				WORD wPacketSize = sizeof(CMD_CB_S_ClubUpdate) + sizeof(tagClubRules);
				SendDataToMember(pUpdateClub->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_UPDATE, pClubUpdate, wPacketSize, LOCATION_MASK_INCLUB);

				//У����˽����
				if ((ClubRules.wSwitchOption & (SWITCH_OPTION_PRIVATE | SWITCH_OPTION_SHUTUP)) != (pClubInfoItem->GetClubRules()->wSwitchOption & (SWITCH_OPTION_PRIVATE | SWITCH_OPTION_SHUTUP)))
				{
					//��ȡ����
					CMD_BC_C_ClubUpdate* pFWClubUpdate=(CMD_BC_C_ClubUpdate*)m_cbBuffer;

					//���ñ���
					pFWClubUpdate->dwClubID = pUpdateClub->dwClubID;
					pFWClubUpdate->cbUpdateKind = pUpdateClub->cbUpdateKind;
					CopyMemory(pFWClubUpdate + 1, pClubInfoItem->GetClubRules(), sizeof(tagClubRules));

					//ɾ��֪ͨ
					WORD wPacketSize = sizeof(CMD_BC_C_ClubUpdate) + sizeof(tagClubRules);
					m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_CLUB_UPDATE, pFWClubUpdate, wPacketSize,0);
				}
			}

			//���¹���
			if (pUpdateClub->cbUpdateKind == UPCLUB_KIND_NOTICE)
			{
				//�������
				tagClubNotice ClubNotice;
				CopyMemory(&ClubNotice, pClubInfoItem->GetClubNotice(), sizeof(ClubNotice));

				//��������
				CopyMemory(pClubUpdate + 1, pUpdateClub + 1, sizeof(tagClubNotice));

				//��������
				CopyMemory(pClubInfoItem->GetClubNotice(), pUpdateClub + 1, sizeof(tagClubNotice));

				//��������
				WORD wPacketSize = sizeof(CMD_CB_S_ClubUpdate) + sizeof(tagClubNotice);
				SendDataToMember(pUpdateClub->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_UPDATE, pClubUpdate, wPacketSize, LOCATION_MASK_INCLUB);				
			}

			//��������
			if (pUpdateClub->cbUpdateKind == UPCLUB_KIND_PROPERTY)
			{
				//��������
				CopyMemory(pClubUpdate + 1, pUpdateClub + 1, sizeof(tagClubProperty));

				//��������
				CopyMemory(pClubInfoItem->GetClubProperty(), pUpdateClub + 1, sizeof(tagClubProperty));

				//��������
				WORD wPacketSize = sizeof(CMD_CB_S_ClubUpdate) + sizeof(tagClubProperty);
				SendDataToMember(pUpdateClub->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_UPDATE, pClubUpdate, wPacketSize, LOCATION_MASK_INCLUB);

				//����ṹ
				tagClubNameCard ClubNameCard;
				ZeroMemory(&ClubNameCard, sizeof(ClubNameCard));

				//����Ⱥ��Ƭ
				ConstructClubNameCard(pClubInfoItem->GetClubProperty(), &ClubNameCard);

				//������Ƭ
				SendDataToMember(pUpdateClub->dwClubID, SERVICE_MODULE_CLUBPLAZA, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_NAMECARD, &ClubNameCard, sizeof(ClubNameCard), LOCATION_MASK_INCLUB | LOCATION_MASK_OUTCLUB);
			}
		}

		return true;
	}
	case SUB_CS_CB_DELETE_CLUB:			//ɾ������
	{
		//��ȡ����
		ASSERT (wDataSize == sizeof(CMD_CS_CB_DeleteClub));
		if (wDataSize != sizeof(CMD_CS_CB_DeleteClub)) return false;

		//��ȡ����
		auto pDeleteClub = (CMD_CS_CB_DeleteClub*)pData;

		//ɾ������
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pDeleteClub->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//�������
			tagClubProperty* pClubProperty = pClubInfoItem->GetClubProperty();

			//����ṹ
			CMD_CS_C_UpdateRouteItem UpdateRouteItem = {};

			//���ñ���
			UpdateRouteItem.dwRouteID = pClubInfoItem->GetClubID();
			UpdateRouteItem.cbRouteState = ROUTE_STATE_NULL;

			//������Ϣ
			m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_UPDATE_ROUTE_ITEM, &UpdateRouteItem, sizeof(UpdateRouteItem), 0);

			//����ṹ
			CMD_CP_S_ClubDelete ClubDelete;
			ZeroMemory(&ClubDelete, sizeof(ClubDelete));

			//��������
			ClubDelete.dwClubID = pDeleteClub->dwClubID;
			StringCchCopy(ClubDelete.szNotifyContent, CountArray(ClubDelete.szNotifyContent), CWHService::FormatMessageV(MIM_CLUB_0012, TEXT("���ѽ�ɢ����[%s]��"), pClubProperty->szClubName));

			//���ҹ���
			CClubUserItem* pClubUserItem = m_ClubUserManager.SearchUserItem(pClubProperty->dwCreaterID);
			if (pClubUserItem != NULL && pClubUserItem->GetUserStatus() == US_CB_ONLINE)
			{
				//��������
				SendDataToUser(SERVICE_MODULE_CLUBPLAZA, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_DELETE, &ClubDelete, sizeof(ClubDelete), pClubUserItem->GetSocketInfo());
			}

			//�Ƴ�����
			pClubInfoItem->RemoveMember(pClubProperty->dwCreaterID);

			//������ʾ
			StringCchCopy(ClubDelete.szNotifyContent, CountArray(ClubDelete.szNotifyContent), CWHService::FormatMessageV(MIM_CLUB_0013, TEXT("������Ĳ���[%s]�ѱ��ϰ��ɢ��"), pClubProperty->szClubName));

			//��������
			SendDataToMember(ClubDelete.dwClubID, SERVICE_MODULE_CLUBPLAZA, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_DELETE, &ClubDelete, sizeof(ClubDelete), LOCATION_MASK_INCLUB | LOCATION_MASK_OUTCLUB);

			//ɾ������
			m_ClubInfoManager.RemoveClubItem(pDeleteClub->dwClubID);

			//��ȡ����
			CMD_BC_C_ClubDelete BCClubDelete;
			ZeroMemory(&BCClubDelete, sizeof(BCClubDelete));

			//���ñ���
			BCClubDelete.dwClubID = pDeleteClub->dwClubID;

			//ɾ��֪ͨ
			m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_CLUB_DELETE, &BCClubDelete, sizeof(BCClubDelete),0);
		}

		return true;
	}		
	case SUB_CS_CB_INSERT_MEMBER:		//��ӳ�Ա
	{
		//��ȡ����
		ASSERT (wDataSize == sizeof(CMD_CS_CB_InsertMember));
		if (wDataSize != sizeof(CMD_CS_CB_InsertMember)) return false;

		//��ȡ����
		auto pInsertMember = (CMD_CS_CB_InsertMember*)pData;

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pInsertMember->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//����ṹ
			CMD_CB_S_MemberInsert MemberInsert;
			ZeroMemory(&MemberInsert, sizeof(MemberInsert));

			//���ñ���
			MemberInsert.dwClubID = pInsertMember->dwClubID;
			MemberInsert.wMemberCount = pInsertMember->wMemberCount;
			CopyMemory(&MemberInsert.MemberUser, &pInsertMember->MemberUser, sizeof(MemberInsert.MemberUser));

			//��������
			SendDataToMember(MemberInsert.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_INSERT, &MemberInsert, sizeof(MemberInsert), LOCATION_MASK_INCLUB);

			//��ӳ�Ա
			pClubInfoItem->GetClubProperty()->wMemberCount=pInsertMember->wMemberCount;
			pClubInfoItem->AppendMember(pInsertMember->MemberUser.MemberInfo);

			//����û�
			auto pClubUserItem = m_ClubUserManager.SearchUserItem(pInsertMember->MemberUser.UserInfo.dwUserID);
			if (pClubUserItem != NULL)
			{
				//��ȡ����
				auto pClubUserInfo = pClubUserItem->GetUserInfo();
				auto pMemberUserInfo = &pInsertMember->MemberUser.UserInfo;

				//���ñ���
				pClubUserInfo->wFaceID = pMemberUserInfo->wFaceID;
				pClubUserInfo->cbGender = pMemberUserInfo->cbGender;
				pClubUserInfo->dwCustomID = pMemberUserInfo->dwCustomID;
				StringCchCopy(pClubUserInfo->szNickName,CountArray(pClubUserInfo->szNickName), pMemberUserInfo->szNickName);
				StringCchCopy(pClubUserInfo->szUserHeadUrl, CountArray(pClubUserInfo->szUserHeadUrl), pMemberUserInfo->szUserHeadUrl);//kk jia
			}
			else
			{
				tagSocketInfo SocketInfo = {};
				m_ClubUserManager.InsertUserItem(pInsertMember->MemberUser.UserInfo, SocketInfo);

				

				CString s;
				s.Format(_T("�����û�3 szNickName:%s,szUserHeadUrl:%s"), pInsertMember->MemberUser.UserInfo.szNickName, pInsertMember->MemberUser.UserInfo.szUserHeadUrl);
				CTraceService::TraceString(s, TraceLevel_Exception);
			}

			//����ṹ
			tagClubNameCard ClubNameCard;
			ZeroMemory(&ClubNameCard, sizeof(ClubNameCard));

			//����Ⱥ��Ƭ
			ConstructClubNameCard(pClubInfoItem->GetClubProperty(), &ClubNameCard);

			//������Ƭ
			SendDataToMember(pInsertMember->dwClubID, SERVICE_MODULE_CLUBPLAZA, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_NAMECARD, &ClubNameCard, sizeof(ClubNameCard), LOCATION_MASK_OUTCLUB);
		}

		return true;
	}
	case SUB_CS_CB_UPDATE_MEMBER:		//���³�Ա
	{
		//��ȡ����
		ASSERT(wDataSize == sizeof(CMD_CS_CB_UpdateMember));
		if (wDataSize != sizeof(CMD_CS_CB_UpdateMember)) return false;

		//��ȡ����
		auto pUpdateMember = (CMD_CS_CB_UpdateMember*)pData;

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pUpdateMember->dwClubID);
		if (pClubInfoItem == NULL) return true;

		//��ѯ��Ա
		auto pIMClubProperty = pClubInfoItem->GetClubProperty();

		//��ѯ��Ա
		auto pClubMember = pClubInfoItem->SearchMember(pUpdateMember->ClubMember.dwMemberID);
		if (pClubMember == NULL) return true;

		//��������
		BYTE cbLastMemberType = pClubMember->cbMemberType;

		//���³�Ա
		CopyMemory(pClubMember, &pUpdateMember->ClubMember, sizeof(tagClubMemberInfo));

		//���³�Ա����
		if (pUpdateMember->wUpdateKind == UPMEMBER_KIND_TYPE)
		{
			//�����û�
			auto pClubUserItem = m_ClubUserManager.SearchUserItem(pUpdateMember->ClubMember.dwMemberID);
			if (pClubUserItem != NULL && pClubUserItem->GetUserStatus() == US_CB_ONLINE)
			{
				//���ù���Ա
				if (cbLastMemberType != MEMBER_TYPE_MANAGER && pUpdateMember->ClubMember.cbMemberType == MEMBER_TYPE_MANAGER && (pUpdateMember->ClubMember.dwManageRight & MANAGE_RIGHT_AUDIT_APPLY) != 0)
				{
					//�����û�
					SendApplyMessageToUser(pIMClubProperty->dwClubID, pClubUserItem->GetSocketInfo());
				}
			}	
		}

		//����ṹ
		CMD_CB_S_MemberUpdate MemberUpdate;
		ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));

		//���ñ���
		MemberUpdate.dwClubID = pUpdateMember->dwClubID;
		CopyMemory(&MemberUpdate.ClubMember, &pUpdateMember->ClubMember, sizeof(MemberUpdate.ClubMember));

		//��������
		SendDataToMember(MemberUpdate.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), LOCATION_MASK_INCLUB);

		return true;
	}
	case SUB_CS_CB_DELETE_MEMBER:		//ɾ����Ա
	{
		//��ȡ����
		ASSERT(wDataSize == sizeof(CMD_CS_CB_DeleteMember));
		if (wDataSize != sizeof(CMD_CS_CB_DeleteMember)) return false;

		//��ȡ����
		auto pDeleteMember = (CMD_CS_CB_DeleteMember*)pData;

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pDeleteMember->dwClubID);
		if (pClubInfoItem != NULL && pClubInfoItem->SearchMember(pDeleteMember->dwMemberID))
		{
			//��������
			tagClubProperty* pClubProperty = pClubInfoItem->GetClubProperty();

			//ɾ����Ա
			pClubProperty->wMemberCount= pDeleteMember->wMemberCount;
			pClubInfoItem->RemoveMember(pDeleteMember->dwMemberID);

			//����ṹ
			CMD_CB_S_MemberDelete MemberDelete;
			ZeroMemory(&MemberDelete, sizeof(MemberDelete));

			//���ñ���
			MemberDelete.dwClubID = pDeleteMember->dwClubID;
			MemberDelete.dwMemberID = pDeleteMember->dwMemberID;
			MemberDelete.wMemberCount = pDeleteMember->wMemberCount;

			//��������
			SendDataToMember(MemberDelete.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_DELETE, &MemberDelete, sizeof(MemberDelete), LOCATION_MASK_INCLUB);

			//���´���
			if (pDeleteMember->dwMemberID == pClubProperty->dwPayrollerID)
			{
				//����ṹ
				CMD_CB_S_ClubUpdate* pClubUpdate = (CMD_CB_S_ClubUpdate*)m_cbBuffer;

				//���ñ���
				pClubUpdate->cbUpdateKind = UPCLUB_KIND_PROPERTY;
				pClubUpdate->dwClubID = pDeleteMember->dwClubID;

				//��������
				CopyMemory(pClubUpdate + 1, pClubProperty, sizeof(tagClubProperty));

				//��������
				WORD wPacketSize = sizeof(CMD_CB_S_ClubUpdate) + sizeof(tagClubProperty);
				SendDataToMember(pDeleteMember->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_UPDATE, pClubUpdate, wPacketSize, LOCATION_MASK_INCLUB);
			}

			//����ṹ
			tagClubNameCard ClubNameCard;
			ZeroMemory(&ClubNameCard, sizeof(ClubNameCard));

			//����Ⱥ��Ƭ
			ConstructClubNameCard(pClubInfoItem->GetClubProperty(), &ClubNameCard);

			//������Ƭ
			SendDataToMember(pDeleteMember->dwClubID, SERVICE_MODULE_CLUBPLAZA, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_NAMECARD, &ClubNameCard, sizeof(ClubNameCard), LOCATION_MASK_OUTCLUB);
		}

		return true;
	}
	case SUB_CS_CB_APPLY_MESSAGE:		//������Ϣ
	{
		//��ȡ����
		ASSERT(wDataSize >= sizeof(CMD_CS_CB_ApplyMessage));
		if (wDataSize < sizeof(CMD_CS_CB_ApplyMessage)) return false;

		//��ȡ����
		auto pApplyMessage = (CMD_CS_CB_ApplyMessage*)pData;

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pApplyMessage->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//��������
			auto pClubApplyMessage = (tagClubApplyMessage*)(pApplyMessage + 1);

			//������Ϣ
			for (WORD i = 0; i < pApplyMessage->wMessageCount; i++)
			{
				auto pMessageItem = pClubInfoItem->SearchMessage(pClubApplyMessage->dwMessageID);
				if (pMessageItem == NULL)
				{
					pClubInfoItem->AppendMessage(*pClubApplyMessage);
				}
				else
				{
					CopyMemory(pMessageItem, pClubApplyMessage, sizeof(tagClubApplyMessage));
				}				

				pClubApplyMessage++;
			}

			//����ṹ
			auto pClubMessage = (CMD_CB_S_ApplyMessage*)m_cbBuffer;
			ZeroMemory(pClubMessage, sizeof(CMD_CB_S_ApplyMessage));

			//���ñ���
			pClubMessage->wMessageCount = pApplyMessage->wMessageCount;

			//��������
			CopyMemory(pClubMessage + 1, pApplyMessage + 1, wDataSize - sizeof(CMD_CS_CB_ApplyMessage));

			//�����С
			WORD wPacketSize = sizeof(CMD_CB_S_ApplyMessage) + wDataSize - sizeof(CMD_CS_CB_ApplyMessage);

			//��������
			POSITION Position = NULL;
			tagClubMemberInfo* pClubMember = NULL;

			do
			{
				//ö�ٳ�Ա
				pClubMember = pClubInfoItem->EnumMember(Position);
				if (pClubMember == NULL) break;				

				//���������Ա
				if (pClubMember->cbMemberType == MEMBER_TYPE_EMPLOYER || (pClubMember->cbMemberType == MEMBER_TYPE_MANAGER && (pClubMember->dwManageRight & MANAGE_RIGHT_AUDIT_APPLY) != 0))
				{
					//�����û�
					CClubUserItem* pClubUserItem = m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
					if (pClubUserItem == NULL || pClubUserItem->GetActiveClubID() != pApplyMessage->dwClubID) continue;

					//��������
					SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_APPLY_MESSAGE, m_cbBuffer, wPacketSize, pClubUserItem->GetSocketInfo());
				}

			} while (Position != NULL);
		}

		return true;
	}
	case SUB_CS_CB_APPLY_DELETE:		//����ɾ��
	{
		//��ȡ����
		ASSERT(wDataSize >= sizeof(CMD_CS_CB_ApplyDelete));
		if (wDataSize < sizeof(CMD_CS_CB_ApplyDelete)) return false;

		//��ȡ����
		auto pApplyDelete = (CMD_CS_CB_ApplyDelete*)pData;

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pApplyDelete->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//��Ϣ��ʶ
			auto pdwMessageID = (DWORD*)(pApplyDelete + 1);

			//������Ϣ
			for (WORD i = 0; i < pApplyDelete->wMessageCount; i++)
			{
				//�Ƴ���Ϣ
				pClubInfoItem->RemoveMessage(*pdwMessageID++);
			}

			//����ṹ
			auto pCBApplyDelete = (CMD_CB_S_ApplyDelete*)m_cbBuffer;
			ZeroMemory(pCBApplyDelete, sizeof(CMD_CB_S_ApplyDelete));

			//���ñ���
			pCBApplyDelete->wMessageCount = pApplyDelete->wMessageCount;

			//��������
			CopyMemory(pCBApplyDelete + 1, pApplyDelete + 1, wDataSize - sizeof(CMD_CS_CB_ApplyDelete));

			//�����С
			WORD wPacketSize = sizeof(CMD_CB_S_ApplyDelete) + wDataSize - sizeof(CMD_CS_CB_ApplyDelete);

			//��������
			POSITION Position = NULL;
			tagClubMemberInfo* pClubMember = NULL;

			do
			{
				//ö�ٳ�Ա
				pClubMember = pClubInfoItem->EnumMember(Position);
				if (pClubMember == NULL) break;

				//���������Ա
				if (pClubMember->dwMemberID!=pApplyDelete->dwUserID && (pClubMember->cbMemberType == MEMBER_TYPE_EMPLOYER || (pClubMember->cbMemberType == MEMBER_TYPE_MANAGER && (pClubMember->dwManageRight & MANAGE_RIGHT_AUDIT_APPLY) != 0)))
				{
					//�����û�
					auto pClubUserItem = m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
					if (pClubUserItem == NULL || pClubUserItem->GetActiveClubID() != pApplyDelete->dwClubID) continue;

					//��������
					SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_APPLY_DELETE, m_cbBuffer, wPacketSize, pClubUserItem->GetSocketInfo());
				}

			} while (Position != NULL);
		}

		return true;
	}
	case SUB_CS_CB_MODIFY_SCORE:		//�޸Ļ���
	{
		//��ȡ����
		ASSERT(wDataSize == sizeof(CMD_CS_CB_ModifyScore));
		if (wDataSize != sizeof(CMD_CS_CB_ModifyScore)) return false;

		//��ȡ����
		auto pModifyScore = (CMD_CS_CB_ModifyScore*)pData;

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pModifyScore->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//��ѯ��Ա
			tagClubMemberInfo* pClubMember = pClubInfoItem->SearchMember(pModifyScore->dwMemberID);
			if (pClubMember != NULL)
			{
				//���·���
				pClubMember->lClubScore = pModifyScore->lMemberScore;

				CString strMessage;
				strMessage.Format(TEXT("MemberID=%d,MemberScore=%I64d"), pModifyScore->dwMemberID, pModifyScore->lMemberScore);
				CTraceService::TraceString(strMessage, TraceLevel_Exception);

				//����ṹ
				CMD_CB_S_MemberUpdate MemberUpdate;
				ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));

				//���ñ���
				MemberUpdate.dwClubID = pModifyScore->dwClubID;
				CopyMemory(&MemberUpdate.ClubMember, pClubMember, sizeof(MemberUpdate.ClubMember));

				//��������
				SendDataToMember(MemberUpdate.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), LOCATION_MASK_INCLUB);

				//��Ա��ʾ
				CClubUserItem* pClubUserItem = m_ClubUserManager.SearchUserItem(pModifyScore->dwMemberID);
				if (pClubUserItem != NULL && pClubUserItem->GetActiveClubID() == MemberUpdate.dwClubID)
				{
					//��ȡ����		
					tagSocketInfo* pIMSocketInfo = pClubUserItem->GetSocketInfo();

					//������Ϣ
					CMD_CM_SystemMessage SystemMessage;
					ZeroMemory(&SystemMessage, sizeof(SystemMessage));

					//��������						
					SystemMessage.wType = SMT_EJECT;
					StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), CWHService::FormatMessageV(MIM_CLUB_0014, TEXT("�����ϰ�,����%s%lld��"), pModifyScore->lModifyScore >= 0 ? TEXT("����") : TEXT("��ȥ"), abs(pModifyScore->lModifyScore / SCORE_MULTIPLE)));
					

					//������Ϣ
					SystemMessage.wLength = CountStringBuffer(SystemMessage.szString);
					WORD wPacketSize = sizeof(SystemMessage) - sizeof(SystemMessage.szString) + SystemMessage.wLength;
					SendDataToGate(pIMSocketInfo->wGateID, SERVICE_MODULE_CLUBPLAZA, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wPacketSize, &pIMSocketInfo->dwTokenID, 1);
				}
			}

			//��ѯ����
			if (pModifyScore->wKindID != 0 && pModifyScore->wServerID != 0)
			{
				//��ȡ����
				CMD_BC_C_ModifyScore ModifyScore;
				ZeroMemory(&ModifyScore, sizeof(ModifyScore));

				//��Ա��Ϣ
				ModifyScore.wKindID = pModifyScore->wKindID;
				ModifyScore.wServerID = pModifyScore->wServerID;
				ModifyScore.dwUserID = pModifyScore->dwMemberID;

				//�����Ϣ
				ModifyScore.cbChangeType = SCORE_CHANGE_TYPE_MODIFY;
				ModifyScore.lChangeScore = pModifyScore->lModifyScore;

				//�����Ϣ
				ModifyScore.dwClubID = pModifyScore->dwClubID;
				StringCchCopy(ModifyScore.szClubName, CountArray(ModifyScore.szClubName), pClubInfoItem->GetClubProperty()->szClubName);

				//�㲥��Ϣ
				m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_MODIFY_SCORE, &ModifyScore, sizeof(ModifyScore),0);
			}
		}		

		return true;
	}		
	case SUB_CS_CB_UPDATE_PAYROLL:		//���´���
	{
		//��ȡ����
		ASSERT(wDataSize == sizeof(CMD_CS_CB_UpdatePayroll));
		if (wDataSize != sizeof(CMD_CS_CB_UpdatePayroll)) return false;

		//��ȡ����
		auto pUpdatePayroll = (CMD_CS_CB_UpdatePayroll*)pData;

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pUpdatePayroll->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//��������
			tagClubProperty* pClubProperty = pClubInfoItem->GetClubProperty();

			//���ô���
			pClubProperty->dwPayrollerID = pUpdatePayroll->dwPayrollerID;

			//����ṹ
			auto pClubUpdate = (CMD_CB_S_ClubUpdate*)m_cbBuffer;

			//���ñ���
			pClubUpdate->cbUpdateKind = UPCLUB_KIND_PROPERTY;
			pClubUpdate->dwClubID = pUpdatePayroll->dwClubID;

			//��������
			CopyMemory(pClubUpdate + 1, pClubProperty, sizeof(tagClubProperty));

			//��������
			WORD wPacketSize = sizeof(CMD_CB_S_ClubUpdate) + sizeof(tagClubProperty);
			SendDataToMember(pUpdatePayroll->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_UPDATE, pClubUpdate, wPacketSize, LOCATION_MASK_INCLUB);
		}

		return true;
	}
	case SUB_CS_CB_CLEANUP_SCORE:		//�������
	{
		//��ȡ����
		ASSERT(wDataSize == sizeof(CMD_CS_CB_CleanupScore));
		if (wDataSize != sizeof(CMD_CS_CB_CleanupScore)) return false;

		//��ȡ����
		auto pCleanupScore = (CMD_CS_CB_CleanupScore*)pData;

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pCleanupScore->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//һ����Ա
			if (pCleanupScore->cbCleanupObject == CLEANUP_OBJECT_ONE_MEMBER)
			{
				//��ѯ��Ա
				tagClubMemberInfo* pClubMember = pClubInfoItem->SearchMember(pCleanupScore->dwMemberID);
				if (pClubMember != NULL)
				{
					//���·���
					pClubMember->lClubScore = 0;

					//����ṹ
					CMD_CB_S_MemberUpdate MemberUpdate;
					ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));

					//���ñ���
					MemberUpdate.dwClubID = pCleanupScore->dwClubID;
					CopyMemory(&MemberUpdate.ClubMember, pClubMember, sizeof(MemberUpdate.ClubMember));

					//��������
					SendDataToMember(MemberUpdate.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), LOCATION_MASK_INCLUB);
				}

				//��Ա��ʾ
				CClubUserItem* pClubUserItem = m_ClubUserManager.SearchUserItem(pCleanupScore->dwMemberID);
				if (pClubUserItem != NULL && pClubUserItem->GetActiveClubID() == pCleanupScore->dwClubID)
				{
					//��ȡ����		
					tagSocketInfo* pIMSocketInfo = pClubUserItem->GetSocketInfo();

					//������Ϣ
					CMD_CM_SystemMessage SystemMessage;
					ZeroMemory(&SystemMessage, sizeof(SystemMessage));

					//��������						
					SystemMessage.wType = SMT_EJECT;
					StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), CWHService::FormatMessage(MIM_CLUB_0015, TEXT("���Ļ����ѱ�����")));

					//������Ϣ
					SystemMessage.wLength = CountStringBuffer(SystemMessage.szString);
					WORD wPacketSize = sizeof(SystemMessage) - sizeof(SystemMessage.szString) + SystemMessage.wLength;
					SendDataToGate(pIMSocketInfo->wGateID, SERVICE_MODULE_CLUBPLAZA, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wPacketSize, &pIMSocketInfo->dwTokenID, 1);
				}
			}

			//���г�Ա
			if (pCleanupScore->cbCleanupObject == CLEANUP_OBJECT_ALL_MEMBER)
			{
				//��������
				POSITION Position = NULL;
				tagClubMemberInfo* pClubMember = NULL;

				do
				{
					//ö�ٳ�Ա
					pClubMember = pClubInfoItem->EnumMember(Position);
					if (pClubMember == NULL) break;

					//�޸Ļ���
					pClubMember->lClubScore = 0;

				} while (Position != NULL);

				//���³�Ա
				SendClubMemberToMember(pCleanupScore->dwClubID);

				//������Ϣ
				CMD_CM_SystemMessage SystemMessage;
				ZeroMemory(&SystemMessage, sizeof(SystemMessage));

				//��������						
				SystemMessage.wType = SMT_EJECT;
				StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), CWHService::FormatMessage(MIM_CLUB_0015, TEXT("���Ļ����ѱ�����")));

				//������Ϣ
				SystemMessage.wLength = CountStringBuffer(SystemMessage.szString);
				WORD wPacketSize = sizeof(SystemMessage) - sizeof(SystemMessage.szString) + SystemMessage.wLength;
				SendDataToMember(pCleanupScore->dwClubID, SERVICE_MODULE_CLUBPLAZA, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wPacketSize, LOCATION_MASK_INCLUB);
			}
		}

		return true;
	}
	case SUB_CS_CB_CLEANUP_REVENUE:		//����˰��
	{
		//��ȡ����
		ASSERT(wDataSize == sizeof(CMD_CS_CB_CleanupRevenue));
		if (wDataSize != sizeof(CMD_CS_CB_CleanupRevenue)) return false;

		//��ȡ����
		auto pCleanupRevenue = (CMD_CS_CB_CleanupRevenue*)pData;

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pCleanupRevenue->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//һ����Ա
			if (pCleanupRevenue->cbCleanupObject == CLEANUP_OBJECT_ONE_MEMBER)
			{
				//��ѯ��Ա
				tagClubMemberInfo* pClubMember = pClubInfoItem->SearchMember(pCleanupRevenue->dwMemberID);
				if (pClubMember != NULL)
				{
					//���·���
					pClubMember->lRevenueCount = 0;

					//����ṹ
					CMD_CB_S_MemberUpdate MemberUpdate;
					ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));

					//���ñ���
					MemberUpdate.dwClubID = pCleanupRevenue->dwClubID;
					CopyMemory(&MemberUpdate.ClubMember, pClubMember, sizeof(MemberUpdate.ClubMember));

					//��������
					SendDataToMember(MemberUpdate.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), LOCATION_MASK_INCLUB);
				}
			}

			//���г�Ա
			if (pCleanupRevenue->cbCleanupObject == CLEANUP_OBJECT_ALL_MEMBER)
			{
				//��������
				POSITION Position = NULL;
				tagClubMemberInfo* pClubMember = NULL;

				do
				{
					//ö�ٳ�Ա
					pClubMember = pClubInfoItem->EnumMember(Position);
					if (pClubMember == NULL) break;

					//�޸Ļ���
					pClubMember->lRevenueCount = 0;

				} while (Position != NULL);

				//���³�Ա
				SendClubMemberToMember(pCleanupRevenue->dwClubID);
			}
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
	case SUB_CS_C_REGISTER_SERVICE:	//ע������
		{
			//Ч������
			ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterService));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterService)) return false;

			//��Ϣ����
			auto pRegisterAccess=(CMD_CS_C_RegisterService *)pData;

			//У��ģ��
			if (pRegisterAccess->wServiceModule != SERVICE_MODULE_GATE) return false;

			//��������
			if (m_GlobalItemManager.SearchGateItem(pRegisterAccess->wServiceID)!=NULL)
			{
				//��������
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//���ñ���
				RegisterFailure.lErrorCode=0;
				StringCchCopy(RegisterFailure.szDescribeString, CountArray(RegisterFailure.szDescribeString), TEXT("�Ѿ�������ͬ��ʶ�����ط������ط���ע��ʧ��"));

				//������Ϣ
				WORD wStringSize=CountStringBufferW(RegisterFailure.szDescribeString);
				WORD wSendSize=sizeof(RegisterFailure)-sizeof(RegisterFailure.szDescribeString)+wStringSize;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_FAILURE,&RegisterFailure,wSendSize,NULL,0);

				//�ж�����
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//���ð�
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->wServiceModule= SERVICE_MODULE_GATE;
			(m_pBindParameter+wBindIndex)->wServiceID=pRegisterAccess->wServiceID;

			//��������
			tagServiceItem ServiceItem;
			ZeroMemory(&ServiceItem,sizeof(ServiceItem));

			//��������
			ServiceItem.wServiceID=pRegisterAccess->wServiceID;
			ServiceItem.wServiceModule= SERVICE_MODULE_GATE;
			ServiceItem.wServicePort = pRegisterAccess->wServicePort;
			ServiceItem.wMaxConnect= pRegisterAccess->wMaxConnect;
			ServiceItem.dwServiceAddr = pRegisterAccess->dwServiceAddr;
			StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), pRegisterAccess->szServiceName);
			StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain),pRegisterAccess->szServiceDomain);

			//ע������
			(m_pBindParameter + wBindIndex)->pBindServiceItem = m_GlobalItemManager.ActiveGateItem(dwSocketID, ServiceItem);

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
		auto pGlobalGateItem = (CGlobalGateItem*)pBindParameter->pBindServiceItem;
		if (pGlobalGateItem != NULL)
		{
			//��ȡ����
			auto pTokenParameter = pGlobalGateItem->SearchTokenID(pLogoutToken->dwTokenID);
			if (pTokenParameter!=NULL)
			{
				//�����û�
				auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
				if (pClubUserItem != NULL && pClubUserItem->GetActiveClubID()>0)
				{
					//��������
					auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pClubUserItem->GetActiveClubID());
					if (pClubInfoItem != NULL)
					{
						//����״̬
						if (pClubInfoItem->GetClubState() == CLUB_STATE_ONLINE)
						{
							pClubInfoItem->DecreaseOnline();
						}						

						//����״̬
						if (pClubInfoItem->GetClubState() == CLUB_STATE_LOADING)
						{
							//�������
							tagSocketInfo SocketInfo = {};
							SocketInfo.wGateID = pBindParameter->wServiceID;
							SocketInfo.dwTokenID = pLogoutToken->dwTokenID;

							//�Ƴ�����
							pClubInfoItem->RemoveEnterRequest(SocketInfo);
						}
					}

					//���ñ���
					pClubUserItem->SetActiveClubID(0);
				}

				//�Ƴ�����
				pGlobalGateItem->RemoveTokenID(pLogoutToken->dwTokenID);
			}
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
		auto pGlobalGateItem = (CGlobalGateItem*)pBindParameter->pBindServiceItem;
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

//����״̬
bool CAttemperEngineSink::OnTCPSocketBattleMainRegister(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_SUCCESS:		//ע��ɹ�
	{
		//�¼�֪ͨ
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess = ER_SUCCESS;
		SendUIControlPacket(UI_BATTLE_RESULT, &ControlResult, sizeof(ControlResult));

		CTraceService::TraceString(TEXT("%s���ӳɹ�"), TraceLevel_Normal, TEXT("Լս������"));

		//�����������⴦��
		if (m_ClubInfoManager.GetClubItemCount()>0)
		{
			//��������
			CClubInfoItem* pClubInfoItem = NULL;

			do
			{
				//ö�ٲ���
				auto pLastClubInfoItem = pClubInfoItem;
				pClubInfoItem = m_ClubInfoManager.EnumClubItem(pLastClubInfoItem);

				//У�����
				if (pClubInfoItem != NULL)
				{		
					QueryBattleTable(pClubInfoItem);
				}

			} while (pClubInfoItem != NULL);
		}


		//����ʱ��
		m_pITimerEngine->SetTimer(IDI_DETECTION_TABLE, 5 * 1000L, -1, 0);

		return true;
	}
	case SUB_CS_S_REGISTER_FAILURE:		//ע��ʧ��
	{
		//��������
		auto pRegisterFailure = (CMD_CS_S_RegisterFailure*)pData;

		//Ч�����
		ASSERT(wDataSize >= (sizeof(CMD_CS_S_RegisterFailure) - sizeof(pRegisterFailure->szDescribeString)));
		if (wDataSize < (sizeof(CMD_CS_S_RegisterFailure) - sizeof(pRegisterFailure->szDescribeString))) return false;

		//��ʾ��Ϣ
		CW2CT strDescribeString(pRegisterFailure->szDescribeString);
		if (lstrlen(strDescribeString) > 0) CTraceService::TraceString(strDescribeString, TraceLevel_Exception);

		return true;
	}
	}

	return true;
}

//ͨ����Ϣ
bool CAttemperEngineSink::OnTCPSocketBattleMainCommonService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	return true;
}

//Լս����
bool CAttemperEngineSink::OnTCPSocketBattleMainBattleService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_BC_S_TABLE_UPDATE:			//���Ӹ���
	{
		//У������
		ASSERT (wDataSize == sizeof(tagBattleTableItem));
		if (wDataSize != sizeof(tagBattleTableItem)) return false;

		//��ȡ����
		auto pBattleTableItem = (tagBattleTableItem*)pData;

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pBattleTableItem->dwClubID);
		if (pClubInfoItem == NULL)
		{
			return true;
		}

		//��������
		auto pClubTableItem = pClubInfoItem->SearchClubTable(pBattleTableItem->dwBattleNum);
		if (pClubTableItem != NULL)
		{
			CopyMemory(&pClubTableItem->TableItem, pBattleTableItem,sizeof(tagBattleTableItem));
		}

		//��������
		SendDataToMember(pBattleTableItem->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_TABLE_UPDATE, pBattleTableItem, sizeof(tagBattleTableItem), LOCATION_MASK_INCLUB);

		return true;
	}
	case SUB_BC_S_TABLE_DISMISS:		//���ӽ�ɢ
	{
		//У������
		ASSERT(wDataSize >= sizeof(CMD_BC_S_TableDismiss));
		if (wDataSize < sizeof(CMD_BC_S_TableDismiss)) return false;

		//��ȡ����
		auto pTableDismiss = (CMD_BC_S_TableDismiss*)pData;

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pTableDismiss->dwClubID);
		if (pClubInfoItem == NULL) return false;

		//��Ϸ��ʼ
		auto pClubTableItem = pClubInfoItem->SearchClubTable(pTableDismiss->dwBattleNum);
		if (pClubTableItem->TableParam.wFinishCount==0 && !pClubTableItem->TableParam.cbGameStatus)
		{
			//�����淨
			auto pBattleConfig = pClubInfoItem->SearchConfig(pClubTableItem->TableItem.dwConfigID);
			if (pBattleConfig != NULL)
			{
				if (pBattleConfig->wTableCount > 0) pBattleConfig->wTableCount--;

				//��������
				while (pBattleConfig->wTableCount < FREE_TABLE_COUNT)
				{
					//��������
					pBattleConfig->wTableCount++;

					CTraceService::TraceString(TEXT("��������2"), TraceLevel_Debug);

					//��������
					CreateBattleTable(pClubInfoItem, pBattleConfig);
				}
			}
		}

		//�Ƴ�����
		pClubInfoItem->RemoveClubTable(pTableDismiss->dwBattleNum);

		//����ṹ
		CMD_CB_S_TableDismiss TableDismiss;
		ZeroMemory(&TableDismiss, sizeof(TableDismiss));

		//���ñ���
		TableDismiss.dwClubID = pTableDismiss->dwClubID;
		TableDismiss.dwBattleNum = pTableDismiss->dwBattleNum;

		//��������
		SendDataToMember(TableDismiss.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_TABLE_DISMISS, &TableDismiss, sizeof(TableDismiss), LOCATION_MASK_INCLUB);

		//��������
		DWORD* pdwUserID = (DWORD*)(pTableDismiss + 1);

		//ͳ��Լս����
		for (WORD i = 0; i < pTableDismiss->wUserCount; i++)
		{
			//���³�Ա
			DBR_GR_UpdateMember UpdateMember;
			ZeroMemory(&UpdateMember, sizeof(UpdateMember));

			//���ñ���
			UpdateMember.dwMemberID = *pdwUserID++;
			UpdateMember.dwClubID = pTableDismiss->dwClubID;

			//������Ϣ
			UpdateMember.wIncBattleCount = 1;
			UpdateMember.cbUpdateKind = UPMEMBER_KIND_BATTLE;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_UPDATE_MEMBER, 0, 0, 0, &UpdateMember, sizeof(UpdateMember));
		}

		return true;
	}
	case SUB_BC_S_TABLE_ITEM:			//��������
	{
		//У������
		ASSERT (wDataSize>=sizeof(tagBattleTableItem));
		if ((wDataSize<sizeof(tagBattleTableItem))) return false;

		//��ȡ����
		int nResidueSize = wDataSize;
		auto pcbByteData = (LPBYTE)pData;				

		//��������
		CClubInfoItem* pClubInfoItem = NULL;
		tagClubTableItem ClubTableItem = {};			
		
		//У������
		while (nResidueSize>0)
		{
			//��������
			ZeroMemory(&ClubTableItem,sizeof(ClubTableItem));

			//��������
			ClubTableItem.TableItem = *(tagBattleTableItem*)(pcbByteData+(wDataSize-nResidueSize));
			nResidueSize -= sizeof(tagBattleTableItem);

			//���Ӳ���
			auto pTableParam = (tagBattleTableParam*)(pcbByteData + (wDataSize-nResidueSize));
			nResidueSize -= sizeof(tagBattleTableParam)+pTableParam->wUserCount*sizeof(tagBattleTableUser);

			//���ò���
			ClubTableItem.TableParam.dwStartTime = pTableParam->dwStartTime;
			ClubTableItem.TableParam.wUserCount = pTableParam->wUserCount;
			ClubTableItem.TableParam.wFinishCount = pTableParam->wFinishCount;
			ClubTableItem.TableParam.cbGameStatus = pTableParam->cbGameStatus;

			//�����û�
			for (int i = 0; i < pTableParam->wUserCount; i++)
			{		
				auto pTableUser = &pTableParam->TableUserItem[i];
				if (pTableUser->wChairID < CountArray(ClubTableItem.TableParam.TableUserItem))
				{
					CopyMemory(&ClubTableItem.TableParam.TableUserItem[pTableUser->wChairID], pTableUser, sizeof(tagBattleTableUser));
				}
			}

			//���Ҳ���
			if (pClubInfoItem == NULL)
			{
				pClubInfoItem = m_ClubInfoManager.SearchClubItem(ClubTableItem.TableItem.dwClubID);
			}

			//�������
			if (pClubInfoItem != NULL)
			{
				if (pClubInfoItem->AppendClubTable(ClubTableItem))
				{
					//У��״̬
					if (ClubTableItem.TableItem.dwConfigID != 0 && ClubTableItem.TableParam.wFinishCount==0 && !ClubTableItem.TableParam.cbGameStatus)
					{
						auto pBattleConfig = pClubInfoItem->SearchConfig(ClubTableItem.TableItem.dwConfigID);
						if (pBattleConfig != NULL) pBattleConfig->wTableCount++;

						CTraceService::TraceString(TEXT("�淨[%d]��������)��"), TraceLevel_Exception, ClubTableItem.TableItem.dwConfigID);
					}
				}
			}			
		}

		//���䴴��ʧ��
		CTraceService::TraceString(TEXT("����[%d]�㲥���Ӹ����)��"), TraceLevel_Exception, ClubTableItem.TableItem.dwClubID);

		//���͸���Ա
		SendDataToMember(ClubTableItem.TableItem.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_TABLE_ITEM, pData, wDataSize, LOCATION_MASK_INCLUB);

		return true;
	}
	case SUB_BC_S_TABLE_PARAM:			//�������
	{
		//У������
		ASSERT (wDataSize == sizeof(CMD_BC_S_TableParam));
		if (wDataSize != sizeof(CMD_BC_S_TableParam)) return false;

		//��ȡ����
		auto pTableParam = (CMD_BC_S_TableParam*)pData;

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pTableParam->dwClubID);
		if (pClubInfoItem == NULL) return false;
		
		//���²���
		auto pClubTableItem = pClubInfoItem->SearchClubTable(pTableParam->dwBattleNum);
		if (pClubTableItem != NULL)
		{
			//Լս��ʼ
			if (pTableParam->wFinishCount == 1 && pTableParam->cbGameStatus==TRUE)
			{
				//�����淨
				auto pBattleConfig = pClubInfoItem->SearchConfig(pClubTableItem->TableItem.dwConfigID);
				if (pBattleConfig != NULL)
				{
					if (pBattleConfig->wTableCount > 0) pBattleConfig->wTableCount--;

					//��������
					while (pBattleConfig->wTableCount < FREE_TABLE_COUNT)
					{
						//��������
						pBattleConfig->wTableCount++;

						CTraceService::TraceString(TEXT("��������3"), TraceLevel_Debug);

						//��������
						CreateBattleTable(pClubInfoItem, pBattleConfig);						
					}
				}
			}						

			pClubTableItem->TableParam.dwStartTime = pTableParam->dwStartTime;
			pClubTableItem->TableParam.wFinishCount = pTableParam->wFinishCount;
			pClubTableItem->TableParam.cbGameStatus = pTableParam->cbGameStatus;
		}

		//����ṹ
		CMD_CB_S_TableParam TableParam;
		ZeroMemory(&TableParam, sizeof(TableParam));

		//���ñ���
		TableParam.dwBattleNum = pTableParam->dwBattleNum;
		TableParam.dwStartTime = pTableParam->dwStartTime;
		TableParam.wFinishCount = pTableParam->wFinishCount;
		TableParam.cbGameStatus = pTableParam->cbGameStatus;

		//��������
		SendDataToMember(pTableParam->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_TABLE_PARAM, &TableParam, sizeof(TableParam), LOCATION_MASK_INCLUB);

		return true;
	}	
	case SUB_BC_S_TABLE_CREATE:			//���Ӵ���
	{
		//У������
		ASSERT (wDataSize >= sizeof(tagBattleTableItem)+sizeof(tagBattleTableParam));
		if (wDataSize < sizeof(tagBattleTableItem) + sizeof(tagBattleTableParam)) return false;

		//��ȡ����
		auto wResidueSize = wDataSize;
		auto pcbByteData = (LPBYTE)pData;

		//��������
		tagClubTableItem ClubTableItem = {};		

		//��������
		ClubTableItem.TableItem = *(tagBattleTableItem*)(pcbByteData + (wDataSize - wResidueSize));
		wResidueSize -= sizeof(tagBattleTableItem);

		//���Ӳ���
		auto pTableParam = (tagBattleTableParam*)(pcbByteData + (wDataSize - wResidueSize));
		wResidueSize -= sizeof(tagBattleTableParam) + pTableParam->wUserCount * sizeof(tagBattleTableUser);

		//���ò���
		ClubTableItem.TableParam.dwStartTime = pTableParam->dwStartTime;
		ClubTableItem.TableParam.wUserCount = pTableParam->wUserCount;
		ClubTableItem.TableParam.wFinishCount = pTableParam->wFinishCount;
		ClubTableItem.TableParam.cbGameStatus = pTableParam->cbGameStatus;

		//�����û�
		for (int i = 0; i < pTableParam->wUserCount; i++)
		{
			auto pTableUser = &pTableParam->TableUserItem[i];
			if (pTableUser->wChairID < CountArray(ClubTableItem.TableParam.TableUserItem))
			{
				CopyMemory(&ClubTableItem.TableParam.TableUserItem[pTableUser->wChairID], pTableUser, sizeof(tagBattleTableUser));
			}
		}

		//�������
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(ClubTableItem.TableItem.dwClubID);
		if (pClubInfoItem != NULL)
		{
			pClubInfoItem->AppendClubTable(ClubTableItem);
		}		

		//���͸���Ա
		SendDataToMember(ClubTableItem.TableItem.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_TABLE_CREATE, pData, wDataSize, LOCATION_MASK_INCLUB);

		return true;
	}
	case SUB_BC_S_BATTLE_FINISH:		//Լս���
	{
		//У������
		ASSERT(wDataSize >= sizeof(CMD_BC_S_BattleFinish));
		if (wDataSize < sizeof(CMD_BC_S_BattleFinish)) return false;

		//��ȡ����
		auto pBattleFinish = (CMD_BC_S_BattleFinish*)pData;

		//��������
		DWORD* pdwUserID = (DWORD*)(pBattleFinish + 1);

		//ͳ��Լս����
		for (WORD i = 0; i < pBattleFinish->wUserCount; i++)
		{
			//���³�Ա
			DBR_GR_UpdateMember UpdateMember;
			ZeroMemory(&UpdateMember, sizeof(UpdateMember));

			//���ñ���
			UpdateMember.dwMemberID = *pdwUserID++;
			UpdateMember.dwClubID = pBattleFinish->dwClubID;

			//������Ϣ
			UpdateMember.wIncBattleCount = 1;
			UpdateMember.cbUpdateKind = UPMEMBER_KIND_BATTLE;

			//Ͷ������
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_UPDATE_MEMBER, 0, 0, 0, &UpdateMember, sizeof(UpdateMember));
		}

		return true;
	}
	case SUB_BC_S_USER_SITDOWN:			//�û�����
	{
		//У������
		ASSERT(wDataSize == sizeof(CMD_BC_S_UserSitdown));
		if (wDataSize != sizeof(CMD_BC_S_UserSitdown)) return false;

		//��ȡ����
		auto pUserSitdown = (CMD_BC_S_UserSitdown*)pData;

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pUserSitdown->dwClubID);
		if (pClubInfoItem == NULL) return false;

		//��������
		auto pClubTableItem = pClubInfoItem->SearchClubTable(pUserSitdown->dwBattleNum);
		if (pClubTableItem != NULL)
		{
			auto pTableUser = &pUserSitdown->TableUserItem;
			if (pTableUser->wChairID < CountArray(pClubTableItem->TableParam.TableUserItem))
			{
				CopyMemory(&pClubTableItem->TableParam.TableUserItem[pTableUser->wChairID], pTableUser, sizeof(tagBattleTableUser));
			}
		}

		//����ṹ
		CMD_CB_S_UserSitdown UserSitdown;
		ZeroMemory(&UserSitdown, sizeof(UserSitdown));

		//���ñ���
		UserSitdown.dwUserID = pUserSitdown->dwUserID;
		UserSitdown.dwBattleNum = pUserSitdown->dwBattleNum;
		CopyMemory(&UserSitdown.TableUserItem, &pUserSitdown->TableUserItem, sizeof(UserSitdown.TableUserItem));

		//��������
		SendDataToMember(pUserSitdown->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_USER_SITDOWN, &UserSitdown, sizeof(UserSitdown), LOCATION_MASK_INCLUB);

		return true;
	}
	case SUB_BC_S_USER_STANDUP:			//�û�����
	{
		//У������
		ASSERT(wDataSize == sizeof(CMD_BC_S_UserStandup));
		if (wDataSize != sizeof(CMD_BC_S_UserStandup)) return false;

		//��ȡ����
		auto pUserStandup = (CMD_BC_S_UserStandup*)pData;

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pUserStandup->dwClubID);
		if (pClubInfoItem == NULL) return false;

		//��������
		auto pClubTableItem = pClubInfoItem->SearchClubTable(pUserStandup->dwBattleNum);
		if (pClubTableItem != NULL)
		{
			if (pUserStandup->wChairID < CountArray(pClubTableItem->TableParam.TableUserItem))
			{
				ZeroMemory(&pClubTableItem->TableParam.TableUserItem[pUserStandup->wChairID], sizeof(tagBattleTableUser));
			}
		}

		//����ṹ
		CMD_CB_S_UserStandup UserStandup;
		ZeroMemory(&UserStandup, sizeof(UserStandup));

		//���ñ���
		UserStandup.wChairID = pUserStandup->wChairID;
		UserStandup.dwUserID = pUserStandup->dwUserID;
		UserStandup.dwBattleNum = pUserStandup->dwBattleNum;

		//��������
		SendDataToMember(pUserStandup->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_USER_STANDUP, &UserStandup, sizeof(UserStandup), LOCATION_MASK_INCLUB);

		return true;
	}
	case SUB_BC_S_USER_EFFICACY:		//�û�У��
	{
		//У������
		ASSERT(wDataSize == sizeof(CMD_BC_S_UserEfficacy));
		if (wDataSize != sizeof(CMD_BC_S_UserEfficacy)) return false;

		//��ȡ����
		auto pUserEfficacy = (CMD_BC_S_UserEfficacy*)pData;

		//����ṹ
		CMD_BC_C_EfficacyResult EfficacyResult;
		ZeroMemory(&EfficacyResult, sizeof(CMD_BC_C_EfficacyResult));

		//���ñ���
		EfficacyResult.dwUserID = pUserEfficacy->dwUserID;
		EfficacyResult.dwBattleNum = pUserEfficacy->dwBattleNum;
		CopyMemory(&EfficacyResult.UserSocketInfo, &pUserEfficacy->UserSocketInfo, sizeof(EfficacyResult.UserSocketInfo));

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pUserEfficacy->dwClubID);
		if (pClubInfoItem == NULL )
		{
			//���ý��
			EfficacyResult.bResult = false;
			StringCchCopy(EfficacyResult.szErrorMessage, CountArray(EfficacyResult.szErrorMessage), CWHService::FormatMessage(MIM_CLUB_0016, TEXT("�����Ǹò���ĳ�Ա���ܼ��뷿�䣡")));			

			//��������
			m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_EFFICACY_RESULT,&EfficacyResult,sizeof(EfficacyResult),0);

			return true;
		}

		auto pClubMember = pClubInfoItem->SearchMember(pUserEfficacy->dwUserID);
		if (pClubMember == NULL)
		{
			//���ý��
			EfficacyResult.bResult = false;
			StringCchCopy(EfficacyResult.szErrorMessage, CountArray(EfficacyResult.szErrorMessage), CWHService::FormatMessage(MIM_CLUB_0016, TEXT("�����Ǹò���ĳ�Ա���ܼ��뷿�䣡")));

			//��������
			m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_EFFICACY_RESULT, &EfficacyResult, sizeof(EfficacyResult),0);

			return true;
		}

		//������
		if (pClubMember->cbMemberStatus == MEMBER_STATUS_FREEZE)
		{
			//���ý��
			EfficacyResult.bResult = false;
			StringCchCopy(EfficacyResult.szErrorMessage, CountArray(EfficacyResult.szErrorMessage), CWHService::FormatMessage(MIM_CLUB_0016, TEXT("���ѱ������ϰ嶳����ϷȨ�ޣ�����ϵ�ϰ�����")));			

			//��������
			m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_EFFICACY_RESULT, &EfficacyResult, sizeof(EfficacyResult),0);

			return true;
		}

		//���ý��
		EfficacyResult.bResult = true;

		//��������
		m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_EFFICACY_RESULT, &EfficacyResult, sizeof(EfficacyResult),0);

		return true;
	}
	case SUB_BC_S_CLUB_SCORE:			//�������
	{
		//��ȡ����
		ASSERT(wDataSize == sizeof(CMD_BC_S_ClubScore));
		if (wDataSize != sizeof(CMD_BC_S_ClubScore)) return false;

		//��ȡ����
		auto pClubScore = (CMD_BC_S_ClubScore*)pData;		

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pClubScore->dwClubID);
		if (pClubInfoItem != NULL)
		{
			CString strMessage;
			strMessage.Format(TEXT("ClubID=%d,UserID=%d,ClubScore=%64d"), pClubScore->dwClubID, pClubScore->dwUserID, pClubScore->lClubScore);
			CTraceService::TraceString(strMessage, TraceLevel_Normal);

			//��ѯ��Ա
			auto pClubMember = pClubInfoItem->SearchMember(pClubScore->dwUserID);
			if (pClubMember != NULL && (pClubScore->lClubScore!=pClubMember->lClubScore || pClubScore->lRevenue!=0))
			{
				//���·���
				pClubMember->lClubScore = pClubScore->lClubScore;
				pClubMember->lRevenueCount += pClubScore->lRevenue;

				//����ṹ
				CMD_CB_S_MemberUpdate MemberUpdate;
				ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));

				//���ñ���
				MemberUpdate.dwClubID = pClubScore->dwClubID;
				CopyMemory(&MemberUpdate.ClubMember, pClubMember, sizeof(MemberUpdate.ClubMember));

				//�����Լ�			
				auto pClubUserItem = m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
				if (pClubUserItem->GetActiveClubID() == pClubScore->dwClubID)
				{
					CTraceService::TraceString(TEXT("���͸����%d"), TraceLevel_Normal, pClubMember->dwMemberID);

					//��������
					SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), pClubUserItem->GetSocketInfo());
				}

				//��������
				POSITION Position = NULL;
				tagClubMemberInfo* pClubMember = NULL;

				do
				{
					//ö�ٳ�Ա
					pClubMember = pClubInfoItem->EnumMember(Position);
					if (pClubMember == NULL) break;

					//���������Ա
					if (pClubMember->dwMemberID != MemberUpdate.ClubMember.dwMemberID && (pClubMember->cbMemberType == MEMBER_TYPE_EMPLOYER || pClubMember->cbMemberType == MEMBER_TYPE_MANAGER))
					{
						//�����û�
						auto pClubUserItem = m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
						if (pClubUserItem == NULL || pClubUserItem->GetActiveClubID() != pClubScore->dwClubID) continue;


						CTraceService::TraceString(TEXT("���͸�����Ա%d"), TraceLevel_Normal, pClubMember->dwMemberID);

						//��������
						SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), pClubUserItem->GetSocketInfo());
					}

				} while (Position != NULL);
			}
		}

		return true;
	}
	case SUB_BC_S_CLUB_WEALTH:			//����Ƹ�
	{
		//��ȡ����
		ASSERT(wDataSize == sizeof(CMD_BC_S_ClubWealth));
		if (wDataSize != sizeof(CMD_BC_S_ClubWealth)) return false;

		//��ȡ����
		auto pClubWealth = (CMD_BC_S_ClubWealth*)pData;

		//��ѯ����
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pClubWealth->dwClubID);
		if (pClubInfoItem != NULL)
		{
			//�����û�
			auto pClubUserItem = m_ClubUserManager.SearchUserItem(pClubWealth->dwPayUserID);
			if (pClubUserItem != NULL && pClubUserItem->GetActiveClubID() == pClubWealth->dwClubID)
			{
				//�������
				auto pWealthUpdate = (CMD_CM_WealthUpdate*)m_cbBuffer;
				ZeroMemory(pWealthUpdate, sizeof(CMD_CM_WealthUpdate));

				//���ñ���
				pWealthUpdate->wItemCount = 1;
				pWealthUpdate->WealthItem[0].wMoneyID = GOODS_ID_CARD;
				pWealthUpdate->WealthItem[0].lBalance = pClubWealth->lUserCard;

				//��������
				WORD wPacketSize = sizeof(CMD_CM_WealthUpdate) + pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]);
				SendDataToUser(SERVICE_MODULE_CLUBPLAZA, MDM_CM_SYSTEM, SUB_CM_WEALTH_UPDATE, pWealthUpdate, wPacketSize, pClubUserItem->GetSocketInfo());
			}
		}

		return true;
	}
	case SUB_BC_S_BATTLE_BASE_OPTION:	//��������
	{
		//��ȡ������Ϣ
		auto pSocketInfo = (tagSocketInfo*)pData;

		//��������
		SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_BATTLE_BASE_OPTION, pSocketInfo+1, wDataSize-sizeof(tagSocketInfo), pSocketInfo);

		return true;
	}
	case SUB_BC_S_BATTLE_GAME_OPTION:	//��Ϸ����
	{
		//��ȡ������Ϣ
		auto pSocketInfo = (tagSocketInfo*)pData;

		//��������
		SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_BATTLE_GAME_OPTION, pSocketInfo + 1, wDataSize - sizeof(tagSocketInfo), pSocketInfo);

		return true;
	}
	case SUB_BC_S_BATTLE_OPTION_FINISH:	//�������
	{
		//��ȡ������Ϣ
		auto pSocketInfo = (tagSocketInfo*)pData;

		//��������
		SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_BATTLE_OPTION_FINISH, NULL, 0, pSocketInfo);

		return true;
	}
	case SUB_BC_S_OPERATE_SUCCESS:		//�����ɹ�
	{
		//��ȡ����
		auto pOperateSuccess = (CMD_BC_S_OperateSuccess*)pData;

		//������Ϣ
		auto pSocketInfo = &pOperateSuccess->SocketInfo;
		if (pSocketInfo->wGateID != 0)
		{
			//�����ɹ�
			SendOperateSuccess(pSocketInfo->wGateID, pSocketInfo->dwTokenID, 0, pOperateSuccess->lOperateCode, pOperateSuccess->szDescribeString);
		}

		return true;
	}
	case SUB_BC_S_OPERATE_FAILURE:		//����ʧ��
	{
		//��ȡ����
		auto pOperateFailure = (CMD_BC_S_OperateFailure*)pData;

		//������Ϣ
		auto pSocketInfo = &pOperateFailure->SocketInfo;
		if (pSocketInfo->wGateID != 0)
		{
			//����ʧ��
			SendOperateFailure(pSocketInfo->wGateID, pSocketInfo->dwTokenID, 0, pOperateFailure->lErrorCode, pOperateFailure->szErrorMessage);
		}
		else
		{
			//���䴴��ʧ��
			CTraceService::TraceString(TEXT("����[%d]Լս���䴴��ʧ��(%d)(%s)��"),TraceLevel_Exception, pOperateFailure->dwClubID, pOperateFailure->lErrorCode, pOperateFailure->szErrorMessage);

			//��������ʧ��
			if (pOperateFailure->lErrorCode == BATTLE_CODE_CREATE_FAILED)
			{
				auto dwConfigID = pSocketInfo->dwTokenID;

				//���Ҿ��ֲ�
				auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pOperateFailure->dwClubID);
				if (pClubInfoItem != NULL)
				{
					auto pBattleConfig = pClubInfoItem->SearchConfig(dwConfigID);
					if (pBattleConfig && pBattleConfig->wTableCount > 0)
					{
						pBattleConfig->wTableCount--;
					}					
				}
			}
		}

		return true;
	}
	}

	return true;
}

//�������
bool CAttemperEngineSink::OnTCPNetworkMainClubService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ��Ϣ
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter * pBindParameter = (m_pBindParameter + wBindIndex);

	//���Ʋ���
	auto pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	switch (wSubCmdID)
	{
	case SUB_CB_C_ENTER_CLUB:		//�������
	{
		//����У��
		ASSERT (wDataSize==sizeof(CMD_CB_C_EnterClub));
		if (wDataSize!=sizeof(CMD_CB_C_EnterClub)) return false;

		//��ȡ����
		auto pEnterClub=(CMD_CB_C_EnterClub *)pData;			

		//����ṹ
		tagSocketInfo SocketInfo;
		ZeroMemory(&SocketInfo,sizeof(SocketInfo));

		//���ñ���
		SocketInfo.dwTokenID=dwTokenID;
		SocketInfo.wGateID=pBindParameter->wServiceID;

		//ֹͣ����
		if (m_bClubSevice==false)
		{
			//����ṹ
			CMD_CB_S_EnterFailure EnterFailure;
			ZeroMemory(&EnterFailure,sizeof(EnterFailure));

			//���ñ���
			EnterFailure.lErrorCode=LOGON_ECODE_FAILED;
			StringCchCopy(EnterFailure.szErrorDescribe,CountArray(EnterFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUB_0001, TEXT("ϵͳά���У�������ʱֹͣ�������Ժ��ٴ����ԣ�")));

			//��������
			WORD wPacketSize=CountStringBuffer(EnterFailure.szErrorDescribe);
			wPacketSize+=sizeof(EnterFailure)-sizeof(EnterFailure.szErrorDescribe);
			SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE,SUB_CB_S_ENTER_FAILURE,&EnterFailure,wPacketSize,&SocketInfo);

			return true;
		}			

		//���Ҳ���
		auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(pEnterClub->dwClubID);
		if (pClubInfoItem==NULL) 
		{
			//����ṹ
			CMD_CB_S_EnterFailure EnterFailure;
			ZeroMemory(&EnterFailure,sizeof(EnterFailure));

			//���ñ���
			EnterFailure.lErrorCode=LOGON_ECODE_FAILED;
			StringCchCopy(EnterFailure.szErrorDescribe,CountArray(EnterFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUB_0002, TEXT("ϵͳδ��ѯ���ò������Ϣ������ʧ�ܣ�")));			

			//��������
			WORD wPacketSize=CountStringBuffer(EnterFailure.szErrorDescribe);
			wPacketSize+=sizeof(EnterFailure)-sizeof(EnterFailure.szErrorDescribe);
			SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE,SUB_CB_S_ENTER_FAILURE,&EnterFailure,wPacketSize,&SocketInfo);

			return true;
		}

		//�����û�
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL || pClubUserItem->GetUserStatus()!=US_CB_ONLINE)
		{
			//����ṹ
			CMD_CB_S_EnterFailure EnterFailure;
			ZeroMemory(&EnterFailure, sizeof(EnterFailure));

			//���ñ���
			EnterFailure.lErrorCode = LOGON_ECODE_FAILED;
			StringCchCopy(EnterFailure.szErrorDescribe, CountArray(EnterFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUB_0003, TEXT("ϵͳδ��ѯ�������û���Ϣ������ʧ�ܣ�")));			

			//��������
			WORD wPacketSize = CountStringBuffer(EnterFailure.szErrorDescribe);
			wPacketSize += sizeof(EnterFailure) - sizeof(EnterFailure.szErrorDescribe);
			SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_ENTER_FAILURE, &EnterFailure, wPacketSize, &SocketInfo);

			return true;
		}

		//��������
		if (pClubInfoItem->GetClubState() == CLUB_STATE_LOADING)
		{
			//��������
			pClubInfoItem->CacheEnterRequest(SocketInfo);

			//���ò���
			pClubUserItem->SetActiveClubID(pEnterClub->dwClubID);

			return true;
		}

		//���ҳ�Ա
		auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL)
		{
			//����ṹ
			CMD_CB_S_EnterFailure EnterFailure;
			ZeroMemory(&EnterFailure,sizeof(EnterFailure));

			//���ñ���
			EnterFailure.lErrorCode=LOGON_ECODE_FAILED;
			StringCchCopy(EnterFailure.szErrorDescribe,CountArray(EnterFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUB_0004, TEXT("��Ǹ���㲻�Ǹò���ĳ�Ա������ʧ�ܣ�")));			

			//��������
			WORD wPacketSize=CountStringBuffer(EnterFailure.szErrorDescribe);
			wPacketSize+=sizeof(EnterFailure)-sizeof(EnterFailure.szErrorDescribe);
			SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE,SUB_CB_S_ENTER_FAILURE,&EnterFailure,wPacketSize,&SocketInfo);

			return true;
		}			

		//��������
		SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_ITEM, pClubInfoItem->GetClubItem(), sizeof(tagClubItem), &SocketInfo);

		//���ͳ�Ա
		SendClubMemberToUser(pEnterClub->dwClubID, &SocketInfo);

		//�����淨
		SendBattleConfigToUser(pEnterClub->dwClubID, &SocketInfo);

		//��������
		SendBattleTableToUser(pEnterClub->dwClubID, &SocketInfo);

		//��������
		SendKindSettingToUser(pEnterClub->dwClubID, &SocketInfo);

		//������Ϣ
		if (pClubMember->cbMemberType == MEMBER_TYPE_EMPLOYER || (pClubMember->cbMemberType == MEMBER_TYPE_MANAGER && (pClubMember->dwManageRight & MANAGE_RIGHT_AUDIT_APPLY) != 0))
		{
			SendApplyMessageToUser(pEnterClub->dwClubID, &SocketInfo);
		}

		//����ṹ
		CMD_CB_S_EnterSuccess EnterSuccess;
		ZeroMemory(&EnterSuccess,sizeof(EnterSuccess));

		//���ñ���
		EnterSuccess.dwClubID=pEnterClub->dwClubID;

		//����ɹ�
		SendDataToUser(SERVICE_MODULE_CLUB,MDM_CLUB_SERVICE,SUB_CB_S_ENTER_SUCCESS,&EnterSuccess,sizeof(EnterSuccess),&SocketInfo);

		//����״̬
		pClubInfoItem->IncreaseOnline();
		pClubInfoItem->SetActiveTime((DWORD)time(NULL));

		//���ò���
		pClubUserItem->SetActiveClubID(pEnterClub->dwClubID);			

		return true;
	}
	case SUB_CB_C_QUERY_OPTION:		//��ѯ����
	{
		//����У��
		ASSERT (wDataSize == sizeof(CMD_CB_C_QueryOption));
		if (wDataSize > sizeof(CMD_CB_C_QueryOption)) return false;

		//��ȡ����
		auto pQueryOption = (CMD_CB_C_QueryOption*)pData;

		//�����û�
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_CLUB_0003, TEXT("ϵͳδ��ѯ�������û���Ϣ��")));

			return true;
		}

		//���Ҳ���
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pClubUserItem->GetActiveClubID());
		if (pClubInfoItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_CLUB_0002, TEXT("ϵͳδ��ѯ���ò������Ϣ��")));

			return true;
		}

		//���ҳ�Ա
		/*auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL || pClubMember->cbMemberType != MEMBER_TYPE_EMPLOYER)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_CLUB_0006, TEXT("��Ǹ����û��Ȩ�޴������ӣ�")));

			return true;
		}*/

		//����ṹ
		CMD_BC_C_QueryOption QueryOption = {};
		
		//���ñ���
		QueryOption.wKindID = pQueryOption->wKindID;		
		QueryOption.cbEmployScene = pClubInfoItem->GetClubProperty()->wClubKind;
		CopyMemory(&QueryOption.SocketInfo, pClubUserItem->GetSocketInfo(), sizeof(QueryOption.SocketInfo));

		//��������
		m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_QUERY_OPTION, &QueryOption, sizeof(QueryOption),0);

		return true;
	}
	case SUB_CB_C_CREATE_TABLE:		//��������
	{
		//����У��
		ASSERT (wDataSize <= sizeof(CMD_CB_C_CreateTable));
		if (wDataSize > sizeof(CMD_CB_C_CreateTable)) return false;

		//��ȡ����
		auto pCreateTable = (CMD_CB_C_CreateTable *)pData;

		//�����û�
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID,dwTokenID, SUB_CB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_CLUB_0003, TEXT("ϵͳδ��ѯ�������û���Ϣ��")));

			return true;
		}

		//���Ҳ���
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pCreateTable->dwClubID);
		if (pClubInfoItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_CLUB_0002, TEXT("ϵͳδ��ѯ���ò������Ϣ��")));
								
			return true;
		}

		//���ҳ�Ա
		auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL || pClubMember->cbMemberType != MEMBER_TYPE_EMPLOYER)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_CREATE_TABLE, BATTLE_CODE_CREATE_FAILED, CWHService::FormatMessage(MIM_CLUB_0006, TEXT("��Ǹ����û��Ȩ�޴������ӣ�")));

			return true;
		}		

		//����ṹ
		CMD_BC_C_CreateTable CreateTable;
		ZeroMemory(&CreateTable, sizeof(CMD_BC_C_CreateTable));

		//�û���Ϣ
		CreateTable.dwUserID= pTokenParameter->dwUserID;		
		
		//������Ϣ
		CreateTable.dwClubID = pCreateTable->dwClubID;
		CreateTable.dwClubCreaterID = pClubInfoItem->GetClubItem()->ClubProperty.dwCreaterID;
		CreateTable.wSwitchOption = pClubInfoItem->GetClubRules()->wSwitchOption;

		//Լս����
		CreateTable.dwConfigID = pCreateTable->dwConfigID;
		CopyMemory(&CreateTable.BattleConfig, &pCreateTable->BattleConfig, sizeof(CreateTable.BattleConfig));

		//������Ϣ
		CopyMemory(&CreateTable.SocketInfo, pClubUserItem->GetSocketInfo(), sizeof(CreateTable.SocketInfo));

		//��������
		m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_CREATE_TABLE, &CreateTable, sizeof(CreateTable),0);

		return true;
	}
	case SUB_CB_C_MODIFY_TABLE:		//�޸�����
	{
		//����У��
		ASSERT (wDataSize <= sizeof(CMD_CB_C_ModifyTable));
		if (wDataSize > sizeof(CMD_CB_C_ModifyTable)) return false;

		//��ȡ����
		auto pModifyTable = (CMD_CB_C_ModifyTable*)pData;

		//�����û�
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_MODIFY_TABLE, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_CLUB_0003, TEXT("ϵͳδ��ѯ�������û���Ϣ��")));

			return true;
		}

		//���Ҳ���
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pModifyTable->dwClubID);
		if (pClubInfoItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_MODIFY_TABLE, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_CLUB_0002, TEXT("ϵͳδ��ѯ���ò������Ϣ��")));

			return true;
		}

		//���ҳ�Ա
		auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL || pClubMember->cbMemberType != MEMBER_TYPE_EMPLOYER)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_MODIFY_TABLE, BATTLE_CODE_MODIFY_FAILED, CWHService::FormatMessage(MIM_CLUB_0007, TEXT("��Ǹ����û��Ȩ���޸ķ��䣡")));

			return true;
		}

		//����ṹ
		CMD_BC_C_ModifyTable ModifyTable;
		ZeroMemory(&ModifyTable, sizeof(ModifyTable));

		//�û���Ϣ
		ModifyTable.dwUserID = pTokenParameter->dwUserID;
		ModifyTable.dwClubID = pModifyTable->dwClubID;
		ModifyTable.dwBattleNum = pModifyTable->dwBattleNum;

		//Լս����
		CopyMemory(&ModifyTable.BattleConfig, &pModifyTable->BattleConfig, sizeof(ModifyTable.BattleConfig));

		//������Ϣ
		CopyMemory(&ModifyTable.SocketInfo, pClubUserItem->GetSocketInfo(), sizeof(ModifyTable.SocketInfo));

		//��������			
		m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_MODIFY_TABLE, &ModifyTable, sizeof(ModifyTable),0);

		return true;
	}
	case SUB_CB_C_DISMISS_TABLE:	//��ɢ����
	{
		//����У��
		ASSERT (wDataSize == sizeof(CMD_CB_C_DismissTable));
		if (wDataSize != sizeof(CMD_CB_C_DismissTable)) return false;

		//��ȡ����
		auto pDismissTable = (CMD_CB_C_DismissTable*)pData;

		//�����û�
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DISMISS_TABLE, BATTLE_CODE_DISMISS_FAILED, CWHService::FormatMessage(MIM_CLUB_0003, TEXT("ϵͳδ��ѯ�������û���Ϣ��")));

			return true;
		}

		//���Ҳ���
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pDismissTable->dwClubID);
		if (pClubInfoItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DISMISS_TABLE, BATTLE_CODE_DISMISS_FAILED, CWHService::FormatMessage(MIM_CLUB_0002, TEXT("ϵͳδ��ѯ���ò������Ϣ��")));

			return true;
		}

		//���ҳ�Ա
		auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL || (pClubMember->cbMemberType!=MEMBER_TYPE_EMPLOYER && pClubMember->cbMemberType != MEMBER_TYPE_MANAGER))
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DISMISS_TABLE, BATTLE_CODE_DISMISS_FAILED, CWHService::FormatMessage(MIM_CLUB_0008, TEXT("��Ǹ����û��Ȩ�޽�ɢ���䣡")));

			return true;
		}

		//����ṹ
		CMD_BC_C_DismissTable DismissTable;
		ZeroMemory(&DismissTable, sizeof(DismissTable));

		//�û���Ϣ
		DismissTable.dwUserID = pTokenParameter->dwUserID;
		DismissTable.dwBattleNum = pDismissTable->dwBattleNum;
		
		//������Ϣ
		CopyMemory(&DismissTable.SocketInfo, pClubUserItem->GetSocketInfo(), sizeof(DismissTable.SocketInfo));

		//��������			
		m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_DISMISS_TABLE, &DismissTable, sizeof(DismissTable),0);

		return true;
	}
	case SUB_CB_C_APPEND_CONFIG:	//����淨
	{
		//����У��
		ASSERT (wDataSize == sizeof(CMD_CB_C_AppendConfig));
		if (wDataSize != sizeof(CMD_CB_C_AppendConfig)) return false;

		//��ȡ����
		auto pAppendConfig = (CMD_CB_C_AppendConfig*)pData;

		//�����û�
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_APPEND_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0003, TEXT("ϵͳδ��ѯ�������û���Ϣ��")));

			return true;
		}

		//���Ҳ���
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pAppendConfig->dwClubID);
		if (pClubInfoItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_APPEND_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0002, TEXT("ϵͳδ��ѯ���ò������Ϣ��")));

			return true;
		}

		//���ҳ�Ա
		auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL || (pClubMember->cbMemberType != MEMBER_TYPE_EMPLOYER && (pClubMember->dwManageRight & MANAGE_RIGHT_MODIFY_CONFIG) == 0))
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_APPEND_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0009, TEXT("��Ǹ����û��Ȩ�����Լս�淨��")));

			return true;
		}

		//����ṹ
		DBR_GR_AppendConfig AppendConfig;
		ZeroMemory(&AppendConfig, sizeof(AppendConfig));

		//�û���Ϣ
		AppendConfig.dwClubID = pAppendConfig->dwClubID;
		AppendConfig.dwUserID = pTokenParameter->dwUserID;		
		
		//�淨ת��
		ConvertBattleConfig(&pAppendConfig->BattleConfig, AppendConfig.BattleConfig);

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_APPEND_CONFIG, AppendConfig.dwClubID, dwSocketID, dwTokenID, &AppendConfig, sizeof(AppendConfig));

		return true;
	}
	case SUB_CB_C_MODIFY_CONFIG:	//�޸��淨
	{
		//����У��
		ASSERT(wDataSize > sizeof(CMD_CB_C_ModifyConfig));
		if (wDataSize <= sizeof(CMD_CB_C_ModifyConfig)) return false;

		//��ȡ����
		auto pModifyConfig = (CMD_CB_C_ModifyConfig*)pData;

		//�����û�
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_MODIFY_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0003, TEXT("ϵͳδ��ѯ�������û���Ϣ��")));

			return true;
		}

		//���Ҳ���
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pModifyConfig->dwClubID);
		if (pClubInfoItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_MODIFY_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0002, TEXT("ϵͳδ��ѯ���ò������Ϣ��")));

			return true;
		}

		//���ҳ�Ա
		auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL || (pClubMember->cbMemberType != MEMBER_TYPE_EMPLOYER && (pClubMember->dwManageRight & MANAGE_RIGHT_MODIFY_CONFIG) == 0))
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_MODIFY_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0010, TEXT("��Ǹ����û��Ȩ���޸�Լս�淨��")));

			return true;
		}

		//����ṹ
		DBR_GR_ModifyConfig ModifyConfig;
		ZeroMemory(&ModifyConfig, sizeof(ModifyConfig));

		//�û���Ϣ
		ModifyConfig.dwClubID = pModifyConfig->dwClubID;
		ModifyConfig.dwUserID = pTokenParameter->dwUserID;		
		ModifyConfig.cbModifyKind = pModifyConfig->cbModifyKind;
		ModifyConfig.BattleConfig.dwConfigID = pModifyConfig->dwConfigID;

		//��������
		auto pModifyData = (PBYTE)(pModifyConfig + 1);
		
		//�޸��淨
		if (ModifyConfig.cbModifyKind == CLUB_BC_MK_BATTLE_CONFIG)
		{
			auto pBattleConfig = (tagBattleConfig*)pModifyData;
			ConvertBattleConfig(pBattleConfig, ModifyConfig.BattleConfig);
		}		

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MODIFY_CONFIG, ModifyConfig.dwClubID, dwSocketID, dwTokenID, &ModifyConfig, sizeof(ModifyConfig));

		return true;
	}
	case SUB_CB_C_DELETE_CONFIG:	//ɾ���淨
	{
		//����У��
		ASSERT (wDataSize == sizeof(CMD_CB_C_DeleteConfig));
		if (wDataSize != sizeof(CMD_CB_C_DeleteConfig)) return false;

		//��ȡ����
		auto pDeleteConfig = (CMD_CB_C_DeleteConfig*)pData;

		//�����û�
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DELETE_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0003, TEXT("ϵͳδ��ѯ�������û���Ϣ��")));

			return true;
		}

		//���Ҳ���
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pDeleteConfig->dwClubID);
		if (pClubInfoItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DELETE_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0002, TEXT("ϵͳδ��ѯ���ò������Ϣ��")));

			return true;
		}

		//���ҳ�Ա
		auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL || (pClubMember->cbMemberType != MEMBER_TYPE_EMPLOYER && (pClubMember->dwManageRight & MANAGE_RIGHT_MODIFY_CONFIG) == 0))
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DELETE_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0011, TEXT("��Ǹ����û��Ȩ��ɾ��Լս�淨��")));

			return true;
		}

		//����ṹ
		DBR_GR_DeleteConfig DeleteConfig;
		ZeroMemory(&DeleteConfig, sizeof(DeleteConfig));

		//�û���Ϣ
		DeleteConfig.wKindID = pDeleteConfig->wKindID;
		DeleteConfig.dwClubID = pDeleteConfig->dwClubID;
		DeleteConfig.dwUserID = pTokenParameter->dwUserID;		
		DeleteConfig.dwConfigID= pDeleteConfig->dwConfigID;	

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_CONFIG, DeleteConfig.dwClubID, dwSocketID, dwTokenID, &DeleteConfig, sizeof(DeleteConfig));

		return true;
	}
	case SUB_CB_C_SET_WINNER_SCORE:	//����Ӯ��
	{
		//����У��
		ASSERT(wDataSize == sizeof(CMD_CB_C_SetWinnerScore));
		if (wDataSize != sizeof(CMD_CB_C_SetWinnerScore)) return false;

		//��ȡ����
		auto pSetWinnerScore = (CMD_CB_C_SetWinnerScore*)pData;

		//�����û�
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pClubUserItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DELETE_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0003, TEXT("ϵͳδ��ѯ�������û���Ϣ��")));

			return true;
		}

		//���Ҳ���
		auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pSetWinnerScore->dwClubID);
		if (pClubInfoItem == NULL)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DELETE_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0002, TEXT("ϵͳδ��ѯ���ò������Ϣ��")));

			return true;
		}

		//���ҳ�Ա
	/*	auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
		if (pClubMember == NULL || pClubMember->cbMemberType != MEMBER_TYPE_EMPLOYER)
		{
			SendOperateFailure(pBindParameter->wServiceID, dwTokenID, SUB_CB_C_DELETE_CONFIG, -1, CWHService::FormatMessage(MIM_CLUB_0018, TEXT("��Ǹ����û��Ȩ�����ô�Ӯ�����Ӯ�֣�")));

			return true;
		}*/

		//����ṹ
		DBR_GR_SetWinnerScore SetWinnerScore;
		ZeroMemory(&SetWinnerScore, sizeof(SetWinnerScore));

		//�û���Ϣ
		SetWinnerScore.wKindID = pSetWinnerScore->wKindID;
		SetWinnerScore.dwClubID = pSetWinnerScore->dwClubID;
		SetWinnerScore.dwUserID = pTokenParameter->dwUserID;
		SetWinnerScore.lMinWinnerScore = pSetWinnerScore->lMinWinnerScore;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SET_WINNER_SCORE, SetWinnerScore.dwClubID, dwSocketID, dwTokenID, &SetWinnerScore, sizeof(SetWinnerScore));

		return true;
	}
	}

	return true;
}

//��ѯ����
bool CAttemperEngineSink::QueryBattleTable(CClubInfoItem* pClubInfoItem)
{
	//�������
	CMD_BC_C_QueryTable QueryTable = {};	
	QueryTable.dwClubID = pClubInfoItem->GetClubID();

	//������Ϣ
	m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_QUERY_TABLE, &QueryTable, sizeof(QueryTable),0);

	//�������
	INT nIndex = 0;
	do
	{
		auto pBattleConfig = pClubInfoItem->EnumConfig(nIndex++);
		if (pBattleConfig == NULL) break;

		pBattleConfig->wTableCount = 0;

	} while (true);

	//�Ƴ�����
	pClubInfoItem->RemoveClubTable();

	//�㲥��Ϣ
	CMD_CB_S_TableCleanup TableCleanup = {};
	TableCleanup.dwClubID = pClubInfoItem->GetClubID();

	//�㲥��Ϣ
	SendDataToMember(TableCleanup.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_TABLE_CLEANUP, &TableCleanup, sizeof(TableCleanup), LOCATION_MASK_INCLUB);

	return true;
}

//��������
bool CAttemperEngineSink::CreateBattleTable(CClubInfoItem* pClubInfoItem, tagClubBattleConfig* pClubBattleConfig)
{
	//����ṹ
	CMD_BC_C_CreateTable CreateTable;
	ZeroMemory(&CreateTable, sizeof(CMD_BC_C_CreateTable));

	//������Ϣ
	CreateTable.dwClubID = pClubInfoItem->GetClubID();
	CreateTable.dwClubCreaterID = pClubInfoItem->GetClubItem()->ClubProperty.dwCreaterID;
	CreateTable.wSwitchOption = pClubInfoItem->GetClubRules()->wSwitchOption;

	//Լս����
	CreateTable.dwConfigID = pClubBattleConfig->dwConfigID;
	ConvertBattleConfig(pClubBattleConfig, CreateTable.BattleConfig);

	//���⸳ֵ
	CreateTable.dwUserID = CreateTable.dwClubCreaterID;
	CreateTable.SocketInfo.dwTokenID = CreateTable.dwConfigID;

	//��������
	m_pITCPSocketBattle->SendData(MDM_BC_BATTLE_SERVICE, SUB_BC_C_CREATE_TABLE, &CreateTable, sizeof(CreateTable),0);

	return true;
}

//���Ʋ���
tagTokenParameter* CAttemperEngineSink::GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ��Ϣ
	WORD wBindIndex = LOWORD(dwSocketID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);	

	//ת��ģʽ
	if (pBindParameter->wServiceModule == SERVICE_MODULE_GATE)
	{
		//��ѯ����
		auto pGlobalGateItem = (CGlobalGateItem*)pBindParameter->pBindServiceItem;
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

//����Ⱥ��Ƭ
bool CAttemperEngineSink::ConstructClubNameCard(const tagClubProperty * pClubProperty, tagClubNameCard * pClubNameCard)
{
	//����У��
	ASSERT (pClubProperty != NULL && pClubNameCard != NULL);
	if (pClubProperty == NULL || pClubNameCard == NULL) return false;

	//��������
	pClubNameCard->dwClubID = pClubProperty->dwClubID;
	pClubNameCard->wClubKind = pClubProperty->wClubKind;
	pClubNameCard->wClubIconID = pClubProperty->wClubIconID;
	pClubNameCard->wMemberCount= pClubProperty->wMemberCount;
	pClubNameCard->wMaxMemberCount = pClubProperty->wMaxMemberCount;
	StringCchCopy(pClubNameCard->szClubName, CountArray(pClubNameCard->szClubName), pClubProperty->szClubName);

	//������Ϣ
	pClubNameCard->dwCreaterID = pClubProperty->dwCreaterID;
	pClubNameCard->dwCreaterGameID = pClubProperty->dwCreaterGameID;

	//��ѯ�û�
	auto pClubUserItem = m_ClubUserManager.SearchUserItem(pClubNameCard->dwCreaterID);
	if (pClubUserItem != NULL)
	{
		pClubNameCard->wCreaterFaceID = pClubUserItem->GetUserInfo()->wFaceID;
		pClubNameCard->dwCreaterCustomID = pClubUserItem->GetUserInfo()->dwCustomID;
		StringCchCopy(pClubNameCard->szCreaterNickName, CountArray(pClubNameCard->szCreaterNickName), pClubUserItem->GetNickName());
	}

	return true;
}

//�����Ա
bool CAttemperEngineSink::OnDBClubMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У������
	ASSERT ((wDataSize-sizeof(DBO_GR_ClubMember))%sizeof(tagClubMemberUser)==0);
	if ((wDataSize-sizeof(DBO_GR_ClubMember))%sizeof(tagClubMemberUser)!=0) return false;

	//��ȡ����
	auto pClubMember=(DBO_GR_ClubMember *)pData;

	//��������
	auto pClubMemberUser = (tagClubMemberUser*)(pClubMember + 1);
	auto wMemberCount=(wDataSize-sizeof(DBO_GR_ClubMember))/sizeof(tagClubMemberUser);	

	//��ѯ����
	auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(pClubMember->dwClubID);
	if (pClubInfoItem==NULL) return true;

	//��������
	for (int i=0; i<wMemberCount; i++)
	{
		//��ӳ�Ա
		pClubInfoItem->AppendMember(pClubMemberUser->MemberInfo);

		//����û�
		auto pClubUserItem=m_ClubUserManager.SearchUserItem(pClubMemberUser->UserInfo.dwUserID);
		if (pClubUserItem!=NULL)
		{		
			//�û���Ϣ
			auto pIMUserInfo = pClubUserItem->GetUserInfo();

			//������Ϣ
			pIMUserInfo->wFaceID = pClubMemberUser->UserInfo.wFaceID;
			pIMUserInfo->cbGender = pClubMemberUser->UserInfo.cbGender;
			pIMUserInfo->dwCustomID = pClubMemberUser->UserInfo.dwCustomID;						
			StringCchCopy(pIMUserInfo->szNickName, CountArray(pIMUserInfo->szNickName), pClubMemberUser->UserInfo.szNickName);
			StringCchCopy(pIMUserInfo->szUserHeadUrl, CountArray(pIMUserInfo->szUserHeadUrl), pClubMemberUser->UserInfo.szUserHeadUrl);//kk jia
		}
		else
		{
			tagSocketInfo SocketInfo = {};
			m_ClubUserManager.InsertUserItem(pClubMemberUser->UserInfo, SocketInfo);
			/*CString s;
			s.Format(_T("�����û�4 szNickName:%s,szUserHeadUrl:%s"), pClubMemberUser->UserInfo.szNickName, pClubMemberUser->UserInfo.szUserHeadUrl);
			CTraceService::TraceString(s, TraceLevel_Exception);*/
		}

		//ָ�����
		pClubMemberUser++;
	}

	return true;
}

//��������
bool CAttemperEngineSink::OnDBKindSetting(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//У������
	ASSERT(wDataSize>=sizeof(DBO_GR_ClubKindSetting));
	if (wDataSize<sizeof(DBO_GR_ClubKindSetting)) return false;

	//��ȡ����
	auto pClubKindSetting = (DBO_GR_ClubKindSetting*)pData;

	//��ѯ����
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pClubKindSetting->dwClubID);
	if (pClubInfoItem == NULL) return true;

	//��������
	for (int i = 0; i < pClubKindSetting->wItemCount; i++)
	{
		//�������
		pClubInfoItem->AppendKindSetting(pClubKindSetting->KindSettingItem[i]);		
	}

	return true;
}

//Լս�淨
bool CAttemperEngineSink::OnDBBattleConfig(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У������
	ASSERT ((wDataSize-sizeof(DBO_GR_BattleConfig))%sizeof(tagClubBattleConfig)==0);
	if ((wDataSize-sizeof(DBO_GR_BattleConfig))%sizeof(tagClubBattleConfig)!=0) return false;

	//��ȡ����
	auto pBattleConfig=(DBO_GR_BattleConfig *)pData;

	//��������
	auto pIMBattleConfig = (tagClubBattleConfig*)(pBattleConfig + 1);
	auto wConfigCount=(wDataSize-sizeof(DBO_GR_BattleConfig))/sizeof(tagClubBattleConfig);	

	//��ѯ����
	auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(pBattleConfig->dwClubID);
	if (pClubInfoItem==NULL) return false;

	//����淨
	for (int i=0; i<wConfigCount; i++)
	{		
		pClubInfoItem->AppendConfig(*pIMBattleConfig++);		
	}

	return true;
}

//������Ϣ
bool CAttemperEngineSink::OnDBApplyMessage(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У������
	ASSERT ((wDataSize - sizeof(DBO_GR_ApplyMessage)) % sizeof(tagClubApplyMessage) == 0);
	if ((wDataSize - sizeof(DBO_GR_ApplyMessage)) % sizeof(tagClubApplyMessage) != 0) return false;

	//��ȡ����
	auto pApplyMessage = (DBO_GR_ApplyMessage *)pData;

	//��������
	auto pIMApplyMessage = (tagClubApplyMessage*)(pApplyMessage + 1);
	auto wMessageCount = (wDataSize - sizeof(DBO_GR_ApplyMessage)) / sizeof(tagClubApplyMessage);	

	//��ѯ����
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pApplyMessage->dwClubID);
	if (pClubInfoItem == NULL) return false;

	//�����Ϣ
	for (int i=0; i<wMessageCount; i++)
	{		
		pClubInfoItem->AppendMessage(*pIMApplyMessage++);
	}

	return true;
}

//���³�Ա
bool CAttemperEngineSink::OnDBUpdateMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT (wDataSize==sizeof(DBO_GR_UpdateMember));
	if (wDataSize!=sizeof(DBO_GR_UpdateMember)) return false;

	//��ȡ����
	auto pUpdateMember=(DBO_GR_UpdateMember *)pData;

	//���Ҳ���
	auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(pUpdateMember->dwClubID);
	if (pClubInfoItem!=NULL)
	{
		//��ݳ�Ա
		auto pClubMember=pClubInfoItem->SearchMember(pUpdateMember->ClubMember.dwMemberID);
		if (pClubMember!=NULL)
		{
			//����ṹ
			CMD_CB_S_MemberUpdate MemberUpdate;
			ZeroMemory(&MemberUpdate, sizeof(MemberUpdate));

			//���ñ���
			MemberUpdate.dwClubID = pUpdateMember->dwClubID;

			//������Ϣ
			if (pUpdateMember->cbUpdateKind == UPMEMBER_KIND_BATTLE)
			{
				//pClubMember->lClubScore = pUpdateMember->ClubMember.lClubScore;
				//pClubMember->lRevenueCount = pUpdateMember->ClubMember.lRevenueCount;
				pClubMember->dwBattleCount = pUpdateMember->ClubMember.dwBattleCount;
				pClubMember->dwCreateCount = pUpdateMember->ClubMember.dwCreateCount;
				pClubMember->LastBattleTime = pUpdateMember->ClubMember.LastBattleTime;				

				//���ñ���
				CopyMemory(&MemberUpdate.ClubMember, pClubMember, sizeof(MemberUpdate.ClubMember));

				//��������
				SendDataToMember(MemberUpdate.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), LOCATION_MASK_INCLUB);
			}
			else
			{
				pClubMember->lClubScore = pUpdateMember->ClubMember.lClubScore;
				pClubMember->lRevenueCount = pUpdateMember->ClubMember.lRevenueCount;
				pClubMember->cbMemberType = pUpdateMember->ClubMember.cbMemberType;
				pClubMember->dwManageRight = pUpdateMember->ClubMember.dwManageRight;
				pClubMember->cbMemberStatus = pUpdateMember->ClubMember.cbMemberStatus;				
				pClubMember->dwBattleCount = pUpdateMember->ClubMember.dwBattleCount;
				pClubMember->dwCreateCount = pUpdateMember->ClubMember.dwCreateCount;
				pClubMember->LastBattleTime = pUpdateMember->ClubMember.LastBattleTime;

				//���ñ���
				CopyMemory(&MemberUpdate.ClubMember, pClubMember, sizeof(MemberUpdate.ClubMember));

				//�����Լ�			
				auto pClubUserItem = m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
				if (pClubUserItem->GetActiveClubID() == pUpdateMember->dwClubID)
				{
					//��������
					SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), pClubUserItem->GetSocketInfo());
				}

				//��������
				POSITION Position = NULL;
				tagClubMemberInfo* pClubMember = NULL;

				do
				{
					//ö�ٳ�Ա
					pClubMember = pClubInfoItem->EnumMember(Position);
					if (pClubMember == NULL) break;

					//���������Ա
					if (pClubMember->dwMemberID != pUpdateMember->ClubMember.dwMemberID && (pClubMember->cbMemberType == MEMBER_TYPE_EMPLOYER || pClubMember->cbMemberType == MEMBER_TYPE_MANAGER))
					{
						//�����û�
						auto pClubUserItem = m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
						if (pClubUserItem == NULL || pClubUserItem->GetActiveClubID() != pUpdateMember->dwClubID) continue;

						//��������
						SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_MEMBER_UPDATE, &MemberUpdate, sizeof(MemberUpdate), pClubUserItem->GetSocketInfo());
					}

				} while (Position != NULL);
			}			
		}
	}

	return true;
}

//����淨
bool CAttemperEngineSink::OnDBAppendConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//У�����
	ASSERT(wDataSize == sizeof(DBO_GR_AppendConfig));
	if (wDataSize != sizeof(DBO_GR_AppendConfig)) return false;

	//��ȡ����
	auto pAppendConfig = (DBO_GR_AppendConfig*)pData;

	//��ѯ����
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pAppendConfig->dwClubID);
	if (pClubInfoItem != NULL)
	{
		//����淨
		pClubInfoItem->AppendConfig(pAppendConfig->BattleConfig);

		//����ṹ
		CMD_CB_S_ConfigAppend ConfigAppend;
		ZeroMemory(&ConfigAppend, sizeof(ConfigAppend));

		//���ñ���
		ConfigAppend.dwClubID = pAppendConfig->dwClubID;
		CopyMemory(&ConfigAppend.BattleConfig, &pAppendConfig->BattleConfig, sizeof(ConfigAppend.BattleConfig));

		//��������
		SendDataToMember(pAppendConfig->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CONFIG_APPEND, &ConfigAppend, sizeof(ConfigAppend), LOCATION_MASK_INCLUB);
	}

	return true;
}

//�޸��淨
bool CAttemperEngineSink::OnDBModifyConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//У�����
	ASSERT(wDataSize == sizeof(DBO_GR_ModifyConfig));
	if (wDataSize != sizeof(DBO_GR_ModifyConfig)) return false;

	//��ȡ����
	auto pModifyConfig = (DBO_GR_ModifyConfig*)pData;

	//��ѯ����
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pModifyConfig->dwClubID);
	if (pClubInfoItem != NULL)
	{
		//�޸��淨
		auto pIMBattleConfig = pClubInfoItem->SearchConfig(pModifyConfig->BattleConfig.dwConfigID);
		if (pIMBattleConfig != NULL)
		{
			CopyMemory(pIMBattleConfig, &pModifyConfig->BattleConfig, sizeof(tagClubBattleConfig));
		}

		//����ṹ
		CMD_CB_S_ConfigModify ConfigModify;
		ZeroMemory(&ConfigModify, sizeof(ConfigModify));

		//���ñ���
		ConfigModify.dwClubID = pModifyConfig->dwClubID;
		ConfigModify.cbModifyKind = pModifyConfig->cbModifyKind;
		CopyMemory(&ConfigModify.BattleConfig, &pModifyConfig->BattleConfig, sizeof(ConfigModify.BattleConfig));

		//��������
		SendDataToMember(ConfigModify.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CONFIG_MODIFY, &ConfigModify, sizeof(ConfigModify), LOCATION_MASK_INCLUB);
	}
	

	return true;
}

//ɾ���淨
bool CAttemperEngineSink::OnDBDeleteConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//У�����
	ASSERT(wDataSize == sizeof(DBO_GR_DeleteConfig));
	if (wDataSize != sizeof(DBO_GR_DeleteConfig)) return false;

	//��ȡ����
	auto pDeleteConfig = (DBO_GR_DeleteConfig*)pData;

	//��ѯ����
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pDeleteConfig->dwClubID);
	if (pClubInfoItem != NULL)
	{
		//�Ƴ��淨
		pClubInfoItem->RemoveConfig(pDeleteConfig->dwConfigID);

		//����ṹ
		CMD_CB_S_ConfigDelete ConfigDelete;
		ZeroMemory(&ConfigDelete, sizeof(ConfigDelete));

		//���ñ���
		ConfigDelete.wKindID = pDeleteConfig->wKindID;
		ConfigDelete.dwClubID = pDeleteConfig->dwClubID;
		ConfigDelete.dwConfigID = pDeleteConfig->dwConfigID;

		//��������
		SendDataToMember(ConfigDelete.dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CONFIG_DELETE, &ConfigDelete, sizeof(ConfigDelete), LOCATION_MASK_INCLUB);
	}	

	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//У�����
	ASSERT(wDataSize <= sizeof(DBO_GR_OperateFailure));
	if (wDataSize > sizeof(DBO_GR_OperateFailure)) return false;

	//��ȡ��Ϣ
	auto wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);

	//��ȡ����
	auto pOperateFailure = (DBO_GR_OperateFailure*)pData;

	//������Ϣ
	//CMD_CM_SystemMessage SystemMessage;
	//ZeroMemory(&SystemMessage, sizeof(SystemMessage));

	////��������						
	//SystemMessage.wType = SMT_EJECT;
	//StringCchCopy(SystemMessage.szString, CountArray(SystemMessage.szString), pOperateFailure->szErrorMessage);

	////������Ϣ
	//SystemMessage.wLength = CountStringBuffer(SystemMessage.szString);
	//WORD wPacketSize = sizeof(SystemMessage) - sizeof(SystemMessage.szString) + SystemMessage.wLength;
	//SendDataToGate(pBindParameter->wServiceID, SERVICE_MODULE_CLUB, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE, &SystemMessage, wPacketSize, &dwTokenID, 1);

	SendOperateFailure(pBindParameter->wServiceID, dwTokenID,0, pOperateFailure->lErrorCode, pOperateFailure->szErrorMessage);

	return true;
}

//�����޸�
bool CAttemperEngineSink::OnDBKindSettingModify(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//У�����
	ASSERT(wDataSize == sizeof(DBO_GR_KindSettingModify));
	if (wDataSize != sizeof(DBO_GR_KindSettingModify)) return false;

	//��ȡ����
	auto pKindSettingModify = (DBO_GR_KindSettingModify*)pData;

	//��ѯ����
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pKindSettingModify->dwClubID);
	if (pClubInfoItem != NULL)
	{
		//�޸�����
		pClubInfoItem->AppendKindSetting(pKindSettingModify->KindSettingItem);

		//��������
		SendDataToMember(pKindSettingModify->dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_KIND_SETTING_MODIFY, &pKindSettingModify->KindSettingItem, sizeof(tagClubKindSetting), LOCATION_MASK_INCLUB);
	}

	return true;
}

//��������
bool CAttemperEngineSink::OnDBClubList(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У������
	ASSERT (wDataSize%sizeof(tagClubItem)==0);
	if (wDataSize%sizeof(tagClubItem)!=0) return false;

	//��������
	auto pIMClubItem = (tagClubItem*)pData;
	auto wClubCount=wDataSize/sizeof(tagClubItem);	

	//��������
	for (int i=0; i<wClubCount; i++)
	{
		auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(pIMClubItem->ClubProperty.dwClubID);
		if (pClubInfoItem!=NULL)
		{
			CopyMemory(pClubInfoItem->GetClubItem(),pIMClubItem,sizeof(tagClubItem));
		}
		else
		{
			m_ClubInfoManager.ActiveClubItem(*pIMClubItem);
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//���س�Ա

		//����ṹ
		DBR_GR_LoadClubMember LoadClubMember;
		LoadClubMember.dwClubID=pIMClubItem->ClubProperty.dwClubID;

		//���س�Ա
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_CLUB_MEMBER, 0, 0, 0, &LoadClubMember, sizeof(LoadClubMember));

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//��������

		//����ṹ
		DBR_GR_LoadKindSetting LoadKindSetting;
		LoadKindSetting.dwClubID = pIMClubItem->ClubProperty.dwClubID;

		//���س�Ա
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_KIND_SETTING, 0, 0, 0, &LoadKindSetting, sizeof(LoadKindSetting));

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//�����淨

		//����ṹ
		DBR_GR_LoadBattleConfig LoadBattleConfig;
		LoadBattleConfig.dwClubID=pIMClubItem->ClubProperty.dwClubID;

		//���س�Ա
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_BATTLE_CONFIG,0,0,0,&LoadBattleConfig,sizeof(LoadBattleConfig));		

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//������Ϣ

		//����ṹ
		DBR_GR_LoadApplyMessage LoadApplyMessage;
		LoadApplyMessage.dwClubID = pIMClubItem->ClubProperty.dwClubID;

		//���س�Ա
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_APPLY_MESSAGE, 0, 0, 0, &LoadApplyMessage, sizeof(LoadApplyMessage));

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////

		//ָ�����
		pIMClubItem++;
	}

	return true;
}

//�б����
bool CAttemperEngineSink::OnDBClubListFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	return true;
}

//��������
bool CAttemperEngineSink::OnDBClubItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У������
	ASSERT (wDataSize==sizeof(tagClubItem));
	if (wDataSize!=sizeof(tagClubItem)) return false;

	//��������
	auto pIMClubItem=(tagClubItem *)pData;

	//��ѯ���
	auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(pIMClubItem->ClubProperty.dwClubID);
	if (pClubInfoItem!=NULL)
	{
		CopyMemory(pClubInfoItem->GetClubItem(),pIMClubItem,sizeof(tagClubItem));
	}
	else
	{
		m_ClubInfoManager.ActiveClubItem(*pIMClubItem);
	}

	return true;
}

//�������
bool CAttemperEngineSink::OnDBClubItemFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//У�����
	ASSERT (wDataSize==sizeof(DBO_GR_ClubItemFinish));
	if (wDataSize != sizeof(DBO_GR_ClubItemFinish)) return false;

	//��ȡ����
	auto pClubItemFinish  = (DBO_GR_ClubItemFinish*)pData;

	//��ѯ���
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(pClubItemFinish->dwClubID);
	if (pClubInfoItem != NULL)
	{
		//����ʱ��
		pClubInfoItem->SetActiveTime((DWORD)time(NULL));

		//����״̬
		pClubInfoItem->SetClubState(CLUB_STATE_ONLINE);		

		//��������
		WORD wIndex = 0;		
		tagSocketInfo* pSocketInfo = NULL;

		do
		{
			//ö������
			pSocketInfo = pClubInfoItem->EnumEnterRequest(wIndex++);
			if (pSocketInfo == NULL) break;

			//��ѯ����
			auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pSocketInfo->wGateID);
			if (pGlobalGateItem == NULL) continue;

			//���Ʋ���
			auto pTokenParameter = GetTokenParameter(pGlobalGateItem->GetSocketID(), pSocketInfo->dwTokenID);
			if (pTokenParameter != NULL)
			{
				//��Ա��Ϣ
				auto pClubMember = pClubInfoItem->SearchMember(pTokenParameter->dwUserID);
				if (pClubMember == NULL)
				{
					//����ṹ
					CMD_CB_S_EnterFailure EnterFailure;
					ZeroMemory(&EnterFailure, sizeof(EnterFailure));

					//���ñ���
					EnterFailure.lErrorCode = LOGON_ECODE_FAILED;
					StringCchCopy(EnterFailure.szErrorDescribe, CountArray(EnterFailure.szErrorDescribe), TEXT("��Ǹ���㲻�Ǹò���ĳ�Ա������ʧ�ܣ�"));

					//��������
					WORD wPacketSize = CountStringBuffer(EnterFailure.szErrorDescribe);
					wPacketSize += sizeof(EnterFailure) - sizeof(EnterFailure.szErrorDescribe);
					SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_ENTER_FAILURE, &EnterFailure, wPacketSize, pSocketInfo);
				}
				else
				{
					//�û���Ϣ
					auto pClubUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
					if (pClubUserItem != NULL && pClubUserItem->GetActiveClubID() == pClubItemFinish->dwClubID)
					{
						//��������
						SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_ITEM, pClubInfoItem->GetClubItem(), sizeof(tagClubItem), pSocketInfo);

						//���ͳ�Ա
						SendClubMemberToUser(pClubInfoItem->GetClubID(), pSocketInfo);

						//�����淨
						SendBattleConfigToUser(pClubInfoItem->GetClubID(), pSocketInfo);

						//��������
						SendKindSettingToUser(pClubInfoItem->GetClubID(), pSocketInfo);

						//������Ϣ
						if (pClubMember->cbMemberType == MEMBER_TYPE_EMPLOYER || (pClubMember->cbMemberType == MEMBER_TYPE_MANAGER && (pClubMember->dwManageRight & MANAGE_RIGHT_AUDIT_APPLY) != 0))
						{
							SendApplyMessageToUser(pClubInfoItem->GetClubID(), pSocketInfo);
						}

						//����ṹ
						CMD_CB_S_EnterSuccess EnterSuccess = {};
						
						//���ñ���
						EnterSuccess.dwClubID = pClubInfoItem->GetClubID();

						//������Ϣ
						SendDataToUser(SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_ENTER_SUCCESS, &EnterSuccess, sizeof(EnterSuccess), pSocketInfo);
					}					
				}
			}			

		} while (true);		
	}	

	//����·��
	if (pClubItemFinish->bActiveRoute == true)
	{
		//����ṹ
		CMD_CS_C_UpdateRouteItem UpdateRouteItem = {};

		//���ñ���
		UpdateRouteItem.dwRouteID = pClubItemFinish->dwClubID;
		UpdateRouteItem.cbRouteState = pClubInfoItem ? ROUTE_STATE_SERVICE : ROUTE_STATE_NULL;

		//������Ϣ
		m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_UPDATE_ROUTE_ITEM, &UpdateRouteItem, sizeof(UpdateRouteItem), 0);

		//��ѯ����
		QueryBattleTable(pClubInfoItem);
	}

	return true;
}

//ת������
bool CAttemperEngineSink::ConvertBattleConfig(tagClubBattleConfig* pBattleConfig, tagBattleConfig& BattleConfig)
{
	//У�����
	ASSERT (pBattleConfig != NULL);
	if (pBattleConfig == NULL) return false;

	//��ȡ����
	auto JsonValuePtr = CWHJson::getValue(string(CT2CA(pBattleConfig->szBattleConfig)));
	if (JsonValuePtr->getType() != eJsonTypeObj) return false;

	//ת������
	auto JsonValueObjPtr = CJsonValueObjPtr::dynamicCast(JsonValuePtr);

	//���ͱ�ʶ
	if (JsonValueObjPtr->value["KindID"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.wKindID, JsonValueObjPtr->value["KindID"], false);
	}

	//��Ϸ����
	if (JsonValueObjPtr->value["PlayerCount"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.wPlayerCount, JsonValueObjPtr->value["PlayerCount"], false);
	}

	//ʹ�ó���
	if (JsonValueObjPtr->value["EmployScene"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.cbEmployScene, JsonValueObjPtr->value["EmployScene"], false);
	}

	//��������
	if (JsonValueObjPtr->value["ScoreKind"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.cbScoreKind, JsonValueObjPtr->value["ScoreKind"], false);
	}

	//�볡��
	if (JsonValueObjPtr->value["EntryCost"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.lEntryCost, JsonValueObjPtr->value["EntryCost"], false);
	}

	//���㷽ʽ
	if (JsonValueObjPtr->value["SettleKind"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.cbSettleKind, JsonValueObjPtr->value["SettleKind"], false);
	}

	//��Ϸ����
	if (JsonValueObjPtr->value["PlayCount"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.wPlayCount, JsonValueObjPtr->value["PlayCount"], false);
	}

	//��Ϸ�׷�
	if (JsonValueObjPtr->value["BaseScore"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.lBaseScore, JsonValueObjPtr->value["BaseScore"], false);
	}

	//kk jia start
	//���
	if (JsonValueObjPtr->value["lMinWin"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.lMinWin, JsonValueObjPtr->value["lMinWin"], false);
	}

	//2
	if (JsonValueObjPtr->value["lWin2"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.lWin2, JsonValueObjPtr->value["lWin2"], false);
	}
	//"AllowPublic" : 0, "lMinWin" : 2, "lWin2" : 3, "lWin3" : 4, "lRevenueRate1" : 5, "lRevenueRate2" : 6, "lRevenueRate3" : 7,
  //3
	if (JsonValueObjPtr->value["lWin3"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.lWin3, JsonValueObjPtr->value["lWin3"], false);
	}

	//shui1
	if (JsonValueObjPtr->value["lRevenueRate1"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.lRevenueRate1, JsonValueObjPtr->value["lRevenueRate1"], false);
	}

	//shui2
	if (JsonValueObjPtr->value["lRevenueRate2"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.lRevenueRate2, JsonValueObjPtr->value["lRevenueRate2"], false);
	}

	//shui3
	if (JsonValueObjPtr->value["lRevenueRate3"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.lRevenueRate3, JsonValueObjPtr->value["lRevenueRate3"], false);
	}

	//kk jia end

	//��Ϸ�׷�
	if (JsonValueObjPtr->value["PayType"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.wPayType, JsonValueObjPtr->value["PayType"], false);
	}

	//��������
	if (JsonValueObjPtr->value["CurrencyKind"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.cbCurrencyKind, JsonValueObjPtr->value["CurrencyKind"], false);
	}

	//��������
	if (JsonValueObjPtr->value["AllowVoice"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.cbAllowVoice, JsonValueObjPtr->value["AllowVoice"], false);
	}

	//����ͬIP
	if (JsonValueObjPtr->value["AllowSameIP"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.cbAllowSameIP, JsonValueObjPtr->value["AllowSameIP"], false);
	}

	//��������ʾ
	if (JsonValueObjPtr->value["AllowPublic"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.cbAllowPublic, JsonValueObjPtr->value["AllowPublic"], false);
	}

	//�Թ�ģʽ
	if (JsonValueObjPtr->value["LookonMode"].get() != NULL)
	{
		CWHJsonInput::readJson(BattleConfig.cbLookonMode, JsonValueObjPtr->value["LookonMode"], false);
	}

	//��Ϸ����
	if (JsonValueObjPtr->value["GameOption"].get() != NULL)
	{
		string sGameOption;
		CHAR szHexValue[3] = {};
		CWHJsonInput::readJson(sGameOption, JsonValueObjPtr->value["GameOption"], false);

		for (int i = 0; i < sGameOption.size(); i += 2)
		{
			lstrcpynA(szHexValue, &sGameOption[i], CountArray(szHexValue));
			BattleConfig.cbGameOption[BattleConfig.cbOptionSize++] = (BYTE)strtol(szHexValue, NULL, 16);
		}
	}

	//�߼�����
	if (JsonValueObjPtr->value["AdvancedOption"].get() != NULL)
	{
		string sAdvancedOption;
		CHAR szHexValue[3] = {};
		CWHJsonInput::readJson(sAdvancedOption, JsonValueObjPtr->value["AdvancedOption"], false);

		for (int i = 0; i < sAdvancedOption.size(); i += 2)
		{
			lstrcpynA(szHexValue, &sAdvancedOption[i], CountArray(szHexValue));
			BattleConfig.cbAdvancedOption[BattleConfig.cbAdvancedSize++] = (BYTE)strtol(szHexValue, NULL, 16);
		}
	}

	return true;
}

//ת������
bool CAttemperEngineSink::ConvertBattleConfig(tagBattleConfig* pBattleConfig, tagClubBattleConfig& BattleConfig)
{
	//��������
	CString strOptionItem;
	CString strGameOption = TEXT("");
	CString strAdvancedOption = TEXT("");

	//��Ϸ����
	for (BYTE i = 0; i < pBattleConfig->cbOptionSize; i++)
	{
		strOptionItem.Format(TEXT("%02X"), pBattleConfig->cbGameOption[i]);
		strGameOption.Append(strOptionItem);
	}

	//���⴦��
	if (pBattleConfig->cbAdvancedSize == 0)
	{
		ZeroMemory(pBattleConfig->cbAdvancedOption, sizeof(pBattleConfig->cbAdvancedOption));		
	}	
	else
	{
		//��������
		auto pAdvancedOption = (tagAdvancedOption*)pBattleConfig->cbAdvancedOption;
		if (pBattleConfig->cbEmployScene == CLUB_KIND_NORMAL)
		{
			if (pAdvancedOption->lRevenueRate != 0) pAdvancedOption->lRevenueRate = 0;
			if (pAdvancedOption->lMinWinScore != 0) pAdvancedOption->lMinWinScore = 0;
		}
		else
		{
			if (pAdvancedOption->lMinWinScore < 0) pAdvancedOption->lMinWinScore = 0;
			if (pAdvancedOption->lRevenueRate > 1000) pAdvancedOption->lRevenueRate = 1000;
		}

		//�߼�����
		for (BYTE i = 0; i < pBattleConfig->cbAdvancedSize; i++)
		{
			strOptionItem.Format(TEXT("%02X"), pBattleConfig->cbAdvancedOption[i]);
			strAdvancedOption.Append(strOptionItem);
		}
	}

	//���ñ���
	BattleConfig.wKindID = pBattleConfig->wKindID;		

	//��������	
	//LPCTSTR pszConfigFormat = TEXT("{\"KindID\":%d,\"ScoreKind\":%d,\"PlayerCount\":%d,\"EmployScene\":%d,\"EntryCost\":%I64d,\"SettleKind\":%d,\"PlayCount\":%d, \
	//	\"BaseScore\":%I64d,\"PayType\":%d,\"CurrencyKind\":%d,\"AllowVoice\":%d,\"AllowSameIP\":%d,\"LookonMode\":%d,\"AllowPublic\":%d,\"GameOption\":\"%s\",\"AdvancedOption\":\"%s\"}");

	////��ʽ����
	//_sntprintf_s(BattleConfig.szBattleConfig, CountArray(BattleConfig.szBattleConfig), pszConfigFormat, pBattleConfig->wKindID, pBattleConfig->cbScoreKind, pBattleConfig->wPlayerCount, pBattleConfig->cbEmployScene,
	//	pBattleConfig->lEntryCost, pBattleConfig->cbSettleKind, pBattleConfig->wPlayCount, pBattleConfig->lBaseScore, pBattleConfig->wPayType, pBattleConfig->cbCurrencyKind, pBattleConfig->cbAllowVoice, pBattleConfig->cbAllowSameIP,
	//	pBattleConfig->cbLookonMode, pBattleConfig->cbAllowPublic, strGameOption,strAdvancedOption);	


	//��������	kk gai
	LPCTSTR pszConfigFormat = TEXT("{\"KindID\":%d,\"ScoreKind\":%d,\"PlayerCount\":%d,\"EmployScene\":%d,\"EntryCost\":%I64d,\"SettleKind\":%d,\"PlayCount\":%d, \
		\"BaseScore\":%I64d,\"PayType\":%d,\"CurrencyKind\":%d,\"AllowVoice\":%d,\"AllowSameIP\":%d,\"LookonMode\":%d,\"AllowPublic\":%d,\"lMinWin\":%I64d,\"lWin2\":%I64d,\"lWin3\":%I64d,\"lRevenueRate1\":%I64d,\"lRevenueRate2\":%I64d,\"lRevenueRate3\":%I64d,\"GameOption\":\"%s\",\"AdvancedOption\":\"%s\"}");

	//��ʽ���� kk gai
	_sntprintf_s(BattleConfig.szBattleConfig, CountArray(BattleConfig.szBattleConfig), pszConfigFormat, pBattleConfig->wKindID, pBattleConfig->cbScoreKind, pBattleConfig->wPlayerCount, pBattleConfig->cbEmployScene,
		pBattleConfig->lEntryCost, pBattleConfig->cbSettleKind, pBattleConfig->wPlayCount, pBattleConfig->lBaseScore, pBattleConfig->wPayType, pBattleConfig->cbCurrencyKind, pBattleConfig->cbAllowVoice, pBattleConfig->cbAllowSameIP,
		pBattleConfig->cbLookonMode, pBattleConfig->cbAllowPublic, pBattleConfig->lMinWin, pBattleConfig->lWin2, pBattleConfig->lWin3, pBattleConfig->lRevenueRate1, pBattleConfig->lRevenueRate2, pBattleConfig->lRevenueRate3, strGameOption, strAdvancedOption);


	return true;
}

//���ͳ�Ա
bool CAttemperEngineSink::SendClubMemberToMember(DWORD dwClubID)
{
	//��������
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(dwClubID);
	if (pClubInfoItem == NULL) return false;

	//��������
	tagClubMemberInfo * pClubMember = NULL;
	CClubUserItem * pClubUserItem = NULL;
	tagClubMemberUser * pClubMemberUser = NULL;

	/*CString s;
	s.Format(_T("1 sizeof(tagClubMemberUser)%d"), sizeof(tagClubMemberUser));
	CTraceService::TraceString(s, TraceLevel_Exception, dwClubID);*/


	//��������
	POSITION Position = NULL;
	WORD wPacketSize = sizeof(CMD_CB_S_ClubMember);

	//��Ա�б�
	auto pClubMemberList = (CMD_CB_S_ClubMember*)m_cbDataBuffer;
	pClubMemberList->dwClubID = dwClubID;

	do
	{
		//ö�ٳ�Ա
		pClubMember = pClubInfoItem->EnumMember(Position);
		if (pClubMember == NULL) break;

		//�����û�
		pClubUserItem = m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
		if (pClubUserItem == NULL) continue;

		//�����ж�
		if (wPacketSize + sizeof(tagClubMemberUser) > sizeof(m_cbDataBuffer))
		{
			SendDataToMember(dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_MEMBER, m_cbDataBuffer, wPacketSize, LOCATION_MASK_INCLUB);
			wPacketSize = sizeof(CMD_CB_S_ClubMember);
		}

		//��ȡ����
		pClubMemberUser = (tagClubMemberUser *)&m_cbDataBuffer[wPacketSize];

		//��������		
		CopyMemory(&pClubMemberUser->MemberInfo, pClubMember, sizeof(pClubMemberUser->MemberInfo));
		CopyMemory(&pClubMemberUser->UserInfo, pClubUserItem->GetUserInfo(), sizeof(pClubMemberUser->UserInfo));

		//���ñ���
		wPacketSize += sizeof(tagClubMemberUser);

	} while (Position!=NULL);
	//CString s;
	/*s.Format(_T("2 sizeof(tagClubMemberUser)%d"), sizeof(tagClubMemberUser));
	CTraceService::TraceString(s, TraceLevel_Exception, dwClubID);*/
	//��������
	if (wPacketSize > 0)
	{
		SendDataToMember(dwClubID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_CLUB_MEMBER, m_cbDataBuffer, wPacketSize, LOCATION_MASK_INCLUB);
	}

	return true;
}

//�����淨
bool CAttemperEngineSink::SendClubMemberToUser(DWORD dwClubID,tagSocketInfo *pUserSocketInfo)
{
	//��������
	auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(dwClubID);
	if (pClubInfoItem==NULL) return false; 

	//��������
	tagClubMemberInfo * pClubMember=NULL;
	CClubUserItem * pClubUserItem=NULL;
	tagClubMemberUser * pClubMemberUser=NULL;

	//��������
	POSITION Position = NULL;
	WORD wPacketSize=sizeof(CMD_CB_S_ClubMember);	

	//��Ա�б�
	auto pClubMemberList = (CMD_CB_S_ClubMember*)m_cbDataBuffer;
	pClubMemberList->dwClubID = dwClubID;

	do
	{
		//ö�ٳ�Ա
		pClubMember=pClubInfoItem->EnumMember(Position);
		if (pClubMember==NULL) break;

		//�����û�
		pClubUserItem=m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
		if (pClubUserItem==NULL) continue;		

		//�����ж�
		if (wPacketSize+sizeof(tagClubMemberUser)>sizeof(m_cbDataBuffer))
		{
			SendDataToGate(pUserSocketInfo->wGateID,SERVICE_MODULE_CLUB,MDM_CLUB_SERVICE,SUB_CB_S_CLUB_MEMBER,m_cbDataBuffer,wPacketSize,&pUserSocketInfo->dwTokenID,1);
			wPacketSize=sizeof(CMD_CB_S_ClubMember);
		}
		
		//��ȡ����
		pClubMemberUser=(tagClubMemberUser *)&m_cbDataBuffer[wPacketSize];

		//��������		
		CopyMemory(&pClubMemberUser->MemberInfo,pClubMember,sizeof(pClubMemberUser->MemberInfo));
		CopyMemory(&pClubMemberUser->UserInfo,pClubUserItem->GetUserInfo(),sizeof(pClubMemberUser->UserInfo));

		/*CString s;
		s.Format(_T("3 szNickName:%s,szUserHeadUrl:%s"), pClubMemberUser->UserInfo.szNickName, pClubMemberUser->UserInfo.szUserHeadUrl);
		CTraceService::TraceString(s, TraceLevel_Exception, dwClubID);*/

		//���ñ���
		wPacketSize+=sizeof(tagClubMemberUser);

	} while (Position!=NULL);

	//��������
	if (wPacketSize>0)
	{
		SendDataToGate(pUserSocketInfo->wGateID,SERVICE_MODULE_CLUB,MDM_CLUB_SERVICE,SUB_CB_S_CLUB_MEMBER,m_cbDataBuffer,wPacketSize, &pUserSocketInfo->dwTokenID, 1);
	}
	/*CString s;
	s.Format(_T("4 sizeof(tagClubMemberUser)%d"), sizeof(tagClubMemberUser));
	CTraceService::TraceString(s, TraceLevel_Exception, dwClubID);*/
	return true;
}

//������Ϣ
bool CAttemperEngineSink::SendApplyMessageToUser(DWORD dwClubID, tagSocketInfo *pUserSocketInfo)
{
	//��������
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(dwClubID);
	if (pClubInfoItem == NULL) return false;

	//��������	
	CClubUserItem * pClubUserItem = NULL;
	tagClubApplyMessage * pIMApplyMessage1 = NULL;
	tagClubApplyMessage * pIMApplyMessage2 = NULL;

	//��������
	auto pClubMessage = (CMD_CB_S_ApplyMessage *)&m_cbDataBuffer;

	//���ñ���
	pClubMessage->wMessageCount = 0;	

	//��������
	POSITION Position = NULL;
	WORD wPacketSize = sizeof(CMD_CB_S_ApplyMessage);

	do
	{
		//ö�ٳ�Ա
		pIMApplyMessage1 = pClubInfoItem->EnumMessage(Position);
		if (pIMApplyMessage1 == NULL) break;

		//�����ж�
		if (wPacketSize + sizeof(tagClubApplyMessage) > sizeof(m_cbDataBuffer))
		{
			//��������
			SendDataToGate(pUserSocketInfo->wGateID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_APPLY_MESSAGE, m_cbDataBuffer, wPacketSize, &pUserSocketInfo->dwTokenID, 1);
			
			//���ñ���
			pClubMessage->wMessageCount=0;
			wPacketSize = sizeof(CMD_CB_S_ApplyMessage);
		}

		//��ȡ����
		pIMApplyMessage2 = (tagClubApplyMessage *)&m_cbDataBuffer[wPacketSize];

		//��������		
		CopyMemory(pIMApplyMessage2, pIMApplyMessage1, sizeof(tagClubApplyMessage));

		//���ñ���
		pClubMessage->wMessageCount++;
		wPacketSize += sizeof(tagClubApplyMessage);

	} while (Position != NULL);

	//��������
	if (wPacketSize > sizeof(CMD_CB_S_ApplyMessage))
	{
		SendDataToGate(pUserSocketInfo->wGateID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_APPLY_MESSAGE, m_cbDataBuffer, wPacketSize, &pUserSocketInfo->dwTokenID, 1);
	}

	return true;
}

//���ͳ�Ա
bool CAttemperEngineSink::SendBattleConfigToUser(DWORD dwClubID,tagSocketInfo *pUserSocketInfo)
{
	//���Ҳ���
	auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(dwClubID);
	if (pClubInfoItem==NULL) return false; 

	//��������
	tagClubBattleConfig * pBattleConfig1=NULL;
	tagClubBattleConfig * pBattleConfig2=NULL;

	//��������
	auto pDBOBattleConfig=(CMD_CB_S_BattleConfig *)&m_cbDataBuffer;
	
	//���ñ���
	pDBOBattleConfig->dwClubID=dwClubID;	

	//��������
	INT nIndex=0;
	WORD wPacketSize=sizeof(CMD_CB_S_BattleConfig);

	do
	{
		//ö���淨
		pBattleConfig1=pClubInfoItem->EnumConfig(nIndex++);		
		if (pBattleConfig1==NULL) break;

		//�����ж�
		if (wPacketSize+sizeof(tagClubBattleConfig)>sizeof(m_cbDataBuffer))
		{
			SendDataToGate(pUserSocketInfo->wGateID,SERVICE_MODULE_CLUB,MDM_CLUB_SERVICE,SUB_CB_S_BATTLE_CONFIG,m_cbDataBuffer,wPacketSize,&pUserSocketInfo->dwTokenID,1);
			wPacketSize=sizeof(CMD_CB_S_BattleConfig);
		}
		
		//��ȡ����
		pBattleConfig2=(tagClubBattleConfig *)&m_cbDataBuffer[wPacketSize];

		//��������		
		CopyMemory(pBattleConfig2,pBattleConfig1,sizeof(tagClubBattleConfig));		

		//���ñ���
		wPacketSize+=sizeof(tagClubBattleConfig);

	} while (true);	

	//��������
	if (wPacketSize>sizeof(CMD_CB_S_BattleConfig))
	{
		SendDataToGate(pUserSocketInfo->wGateID,SERVICE_MODULE_CLUB,MDM_CLUB_SERVICE,SUB_CB_S_BATTLE_CONFIG,m_cbDataBuffer,wPacketSize,&pUserSocketInfo->dwTokenID,1);
	}

	return true;
}

//��������
bool CAttemperEngineSink::SendKindSettingToUser(DWORD dwClubID, tagSocketInfo* pUserSocketInfo)
{
	//���Ҳ���
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(dwClubID);
	if (pClubInfoItem == NULL) return false;

	//��������
	INT nIndex = 0;
	WORD wPacketSize = 0;

	do
	{
		//ö���淨
		auto pKindSettingItem1 = pClubInfoItem->EnumKindSetting(nIndex++);
		if (pKindSettingItem1 == NULL) break;

		//�����ж�
		if (wPacketSize + sizeof(tagClubKindSetting) > sizeof(m_cbDataBuffer))
		{
			SendDataToGate(pUserSocketInfo->wGateID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_KIND_SETTING_ITEM, m_cbDataBuffer, wPacketSize, &pUserSocketInfo->dwTokenID, 1);
			wPacketSize = 0;
		}

		//��ȡ����
		auto pKindSettingItem2 = (tagClubKindSetting*)&m_cbDataBuffer[wPacketSize];

		//��������		
		CopyMemory(pKindSettingItem2, pKindSettingItem1, sizeof(tagClubKindSetting));

		//���ñ���
		wPacketSize += sizeof(tagClubKindSetting);

	} while (true);

	//��������
	if (wPacketSize > 0)
	{
		SendDataToGate(pUserSocketInfo->wGateID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_KIND_SETTING_ITEM, m_cbDataBuffer, wPacketSize, &pUserSocketInfo->dwTokenID, 1);
	}

	return true;
}

//��������
bool CAttemperEngineSink::SendBattleTableToUser(DWORD dwClubID, tagSocketInfo* pUserSocketInfo)
{
	//��������	
	POSITION Position = NULL;
	tagClubTableItem* pClubTableItem = NULL;

	//��������
	WORD wPacketSize = 0;
	tagBattleTableItem* pTableItem = NULL;
	tagBattleTableItem* pTableItemEx = NULL;
	tagBattleTableParam* pTableParam = NULL;
	tagBattleTableParamEx* pTableParamEx = NULL;

	//���Ҳ���
	auto pClubInfoItem = m_ClubInfoManager.SearchClubItem(dwClubID);
	if (pClubInfoItem != NULL)
	{
		do
		{
			pClubTableItem = pClubInfoItem->EnumClubTable(Position);
			if (pClubTableItem == NULL) break;

			//�����ж�
			if (wPacketSize + sizeof(tagBattleTableItem) + sizeof(tagBattleTableParam) + TABLE_USER_COUNT * sizeof(tagBattleTableUser) > sizeof(m_cbDataBuffer))
			{
				SendDataToGate(pUserSocketInfo->wGateID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_TABLE_ITEM, m_cbDataBuffer, wPacketSize, &pUserSocketInfo->dwTokenID, 1);
				wPacketSize = 0;
			}

			//��ȡ����
			pTableItemEx = &pClubTableItem->TableItem;
			pTableParamEx = &pClubTableItem->TableParam;

			//��������
			pTableItem = (tagBattleTableItem*)&m_cbDataBuffer[wPacketSize];
			CopyMemory(pTableItem, pTableItemEx, sizeof(tagBattleTableItem));

			//��������			
			wPacketSize += sizeof(tagBattleTableItem);

			//���Ӳ���
			pTableParam = (tagBattleTableParam*)&m_cbDataBuffer[wPacketSize];
			ZeroMemory(pTableParam, sizeof(tagBattleTableParam));

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

		} while (Position!=NULL);
	}

	//�����ж�
	if (wPacketSize>0)
	{
		CTraceService::TraceString(TEXT("����[%d]�������Ӹ���ң�"), TraceLevel_Exception, dwClubID);

		SendDataToGate(pUserSocketInfo->wGateID, SERVICE_MODULE_CLUB, MDM_CLUB_SERVICE, SUB_CB_S_TABLE_ITEM, m_cbDataBuffer, wPacketSize, &pUserSocketInfo->dwTokenID, 1);		
	}	

	return true;
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
	SendDataToGate(wServiceID, SERVICE_MODULE_CLUBPLAZA, MDM_CM_SYSTEM, SUB_CM_OPERATE_SUCCESS, pRequestSuccess, wDataSize, &dwTokenID, 1);

	return;
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
	SendDataToGate(wServiceID, SERVICE_MODULE_CLUBPLAZA, MDM_CM_SYSTEM, SUB_CM_OPERATE_FAILURE, pRequestFailure, wDataSize, &dwTokenID, 1);

	return;
}

//��������
bool CAttemperEngineSink::SendDataToUser(WORD wServiceModule, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize,tagSocketInfo *pUserSocketInfo)
{
	//��������
	return SendDataToGate(pUserSocketInfo->wGateID,wServiceModule,wMainCmdID,wSubCmdID, pData,wDataSize,&pUserSocketInfo->dwTokenID,1);
}

//���ط���
bool CAttemperEngineSink::SendDataToGate(WORD wGateID, WORD wServiceModule, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD *pdwTokenID, WORD wTokenCount)
{
	//��������
	auto  pGlobalGateItem=m_GlobalItemManager.SearchGateItem(wGateID);
	if (pGlobalGateItem ==NULL) return false;

	//��������
	m_pITCPNetworkEngine->SendData(pGlobalGateItem->GetSocketID(), MAKECMMD(wMainCmdID, wServiceModule),wSubCmdID,pData,wDataSize,pdwTokenID,wTokenCount);

	return true;
}


//���緢��
bool CAttemperEngineSink::SendDataToMember(DWORD dwClubID, WORD wServiceModule, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, BYTE cbLocationMask)
{
	//��������
	auto pClubInfoItem=m_ClubInfoManager.SearchClubItem(dwClubID);
	if (pClubInfoItem==NULL) return false;

	//��������
	INT_PTR nIndex=0;		
	tagClubMemberInfo * pClubMember=NULL;
	CClubUserItem * pClubUserItem=NULL;

	//��������
	POSITION Position=NULL;
	CGlobalGateItem * pGlobalGateItem=NULL;

	do
	{	
		//ö�ٳ�Ա
		pClubMember=pClubInfoItem->EnumMember(Position);
		if (pClubMember==NULL) break;

		//�����û�
		pClubUserItem=m_ClubUserManager.SearchUserItem(pClubMember->dwMemberID);
		if (pClubUserItem==NULL) continue;
		if (pClubUserItem->GetUserStatus()!=US_CB_ONLINE) continue;

		//λ������
		if (cbLocationMask != (LOCATION_MASK_INCLUB | LOCATION_MASK_OUTCLUB))
		{
			if ((cbLocationMask&LOCATION_MASK_INCLUB) != 0 && pClubUserItem->GetActiveClubID() != dwClubID) continue;
			if ((cbLocationMask&LOCATION_MASK_OUTCLUB) != 0 && pClubUserItem->GetActiveClubID() == dwClubID) continue;
		}

		//��������
		pGlobalGateItem=m_GlobalItemManager.SearchGateItem(pClubUserItem->GetGateID());
		if (pGlobalGateItem!=NULL)
		{
			pGlobalGateItem->m_TokenIDArray.Add(pClubUserItem->GetTokenID());
		}		

	} while (Position!=NULL);

	//////////////////////////////////////////////////////////////////////////////////////////////

	//��������		
	DWORD * pdwTokenIDArray=NULL; 
	INT_PTR nCellCount = 0,nSendCount=0,nTotalCount=0;
	
	//���ñ���
	pGlobalGateItem = NULL;

	do
	{	
		//ö������
		pGlobalGateItem=m_GlobalItemManager.EnumGateItem(pGlobalGateItem);
		if (pGlobalGateItem==NULL) break;

		//���ñ���		
		nSendCount=0;
		nCellCount =pGlobalGateItem->m_TokenIDArray.GetCount();
		nTotalCount=pGlobalGateItem->m_TokenIDArray.GetCount();		
		pdwTokenIDArray=pGlobalGateItem->m_TokenIDArray.GetData();		

		//�з�����
		while ((sizeof(WORD)+ nCellCount *sizeof(DWORD)+wDataSize)>SOCKET_PACKET)
		{
			nCellCount = nCellCount /2;
		}	

		//ѭ������
		while (nSendCount < nTotalCount)
		{		
			//��������
			if (nTotalCount-nSendCount< nCellCount)
			{
				nCellCount =nTotalCount-nSendCount;
			}

			//��������
			if (nCellCount > TOKEN_COUNT) nCellCount = TOKEN_COUNT;

			//��������
			SendDataToGate(pGlobalGateItem->GetServiceID(),wServiceModule,wMainCmdID,wSubCmdID,pData,wDataSize,pdwTokenIDArray+nSendCount,nCellCount);

			//��������			
			nSendCount += nCellCount;
		}

		//�������
		pGlobalGateItem->m_TokenIDArray.RemoveAll();

	} while (Position!=NULL);

	return true;
}
//////////////////////////////////////////////////////////////////////////
