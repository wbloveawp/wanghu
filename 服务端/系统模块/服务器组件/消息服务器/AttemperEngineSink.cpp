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
#define IDI_CONNECT_VALIDATE		2									//������ʶ
#define IDI_CONNECT_CENTER			3									//������ʶ

//////////////////////////////////////////////////////////////////////////
//��ɫ����
#define COLOR_TEXT					0xFFBF7B							//�ı���ɫ
#define COLOR_SYSTEM				0xFFBF7B							//ϵͳ��ɫ
#define COLOR_NICKNAME				0xFFFFFF							//�ǳ���ɫ
#define COLOR_MATCHNAME				0xFFBF7B							//������ɫ
#define COLOR_NORMAL_NUMBER			0xFF00FF							//��ͨ����
#define COLOR_WEALTH_NUMBER			0xFFFFFF							//�Ƹ�����

//��������
#define	NOTICE_CACHE_COUNT			10									//��������
#define MESSAGE_SERVER_LOG_FILE		L"Message"
//////////////////////////////////////////////////////////////////////////
//��Ϣ��ʽ
LPCTSTR g_pszMsgFormat1=L"#%d#%s#%d#��%s�л�õ�%d��,Ӯ��";

//////////////////////////////////////////////////////////////////////////

//���캯��
CAttemperEngineSink::CAttemperEngineSink()
{
	//״̬����	
	m_bCollectUser=false;

	//���ñ���	
	m_pServiceOption=NULL;
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;		

	//�������
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;	
	m_pITCPSocketCenter=NULL;

	//��ʼ��
	m_StockMarqueeNoticeInfo.InitHashTable(101);

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

	//������־
	if (InitLoggerService() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//ֹͣ�¼�
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//״̬����		
	m_bCollectUser=false;

	//ֹͣ����
	g_TimeLoggerService->ConcludeService();

	//���ñ���
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;

	//ɾ������
	SafeDeleteArray(m_pBindParameter);

	//�������
	m_GlobalItemManager.ResetData();		

	return true;
}

//�����¼�
bool CAttemperEngineSink::OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	switch (dwIdentifier)
	{
	case CT_CONNECT_CENTER:		//��������
	{
		//���ܱ�־
		m_bCollectUser = false;

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

			//���ܱ�־
			m_bCollectUser = false;

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
	//��������
	bool bResult = true;
	WORD wMainCmdID = LOCMMD(Command.wMainCmdID);

	{
		CString str;
		str.Format(L"[LH][MessageServer] M:%d,S:%d", wMainCmdID, Command.wSubCmdID);
		FTLOG(MESSAGE_SERVER_LOG_FILE)->LogInfo(str);
	}

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
	case MDM_AN_MESSAGE_SERVICE:	//��Ϣ����
		{
			bResult = OnTCPNetWorkMainMessageService(Command.wSubCmdID,pData,wDataSize,dwSocketID,dwTokenID);
			break;
		}
	}

	//�Ͽ�����
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
	//��ȡ��Ϣ
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//���ط���
	if (pBindParameter->wServiceModule==SERVICE_MODULE_GATE)
	{
		//��������
		WORD wBindIndex=LOWORD(dwSocketID);

		//��������
		CGlobalGateItem * pGlobalGateItem=m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
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
	switch (wRequestID)
	{
	case DBO_GR_USER_MAIL_INFO:		//�ʼ���Ϣ
	{
		return OnDBUserMailInfo(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_USER_MAIL_FINISH:	//�ʼ����
	{
		return OnDBUserMailFinish(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_USER_MAIL_UPDATE:	//�ʼ�����
	{
		return OnDBUserMailUpdate(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_USER_MAIL_INSERT:	//�ʼ�����
	{
		return OnDBUserMailInsert(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_CLAIM_MAIL_ATTACH:	//��ȡ����
	{
		return OnDBClaimMailAttach(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_USER_GOODS_UPDATE:	//��Ʒ����
	{
		return OnDBUserGoodsUpdate(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_USER_WEALTH_UPDATE:	//�Ƹ�����
	{
		return OnDBUserWealthUpdate(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_OPERATE_SUCCESS:	//�������
	{
		return OnDBOperateSuccess(dwContextID, dwTokenID, pData, wDataSize);
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
		RegisterService.wServicePort=m_pServiceOption->wServicePort;
		RegisterService.wServiceModule= SERVICE_MODULE_MESSAGE;
		RegisterService.wServiceKindID = m_pServiceOption->wServiceKindID;
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
	{
		CString str;
		str.Format(L"[LH][MessageServer][Server] M:%d,S:%d", Command.wMainCmdID, Command.wSubCmdID);
		FTLOG(MESSAGE_SERVER_LOG_FILE)->LogInfo(str);
	}

	//��������
	if (dwServiceID==NETWORK_CENTER)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:			//ע�����
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
		}
		case MDM_CS_USER_COLLECT:		//�û�����
		{
			return OnTCPSocketMainCollectService(Command.wSubCmdID,pData,wDataSize);
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
			return OnTCPSocketMainForwardService(Command.wSubCmdID,pData,wDataSize);
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
			ASSERT(wDataSize % sizeof(tagServiceItem) == 0);
			if (wDataSize % sizeof(tagServiceItem) != 0) return false;

			//��ȡ����
			tagServiceItem * pServiceItem = (tagServiceItem *)pData;

			//��־����
			if (pServiceItem->wServiceModule == SERVICE_MODULE_LOGGER)
			{
				//��־����
				g_TimeLoggerService->SetLoggerServer(pServiceItem->szServiceDomain, pServiceItem->wServicePort);

				FTLOG(MESSAGE_SERVER_LOG_FILE)->LogInfo(L"connect log server ...");
			}

			return true;
		}
		case SUB_CS_S_SERVICE_REMOVE:	//�����Ƴ�
		{
			//����У��
			ASSERT(wDataSize == sizeof(CMD_CS_S_ServiceRemove));
			if (wDataSize != sizeof(CMD_CS_S_ServiceRemove)) return false;

			//��ȡ����
			CMD_CS_S_ServiceRemove * pServiceRemove = (CMD_CS_S_ServiceRemove *)pData;


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

//���ܷ���
bool CAttemperEngineSink::OnTCPSocketMainCollectService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_COLLECT_USER:			//�û�����
	{
		//��������
		CMD_CS_C_EnterMessage EnterMessage;
		ZeroMemory(&EnterMessage, sizeof(EnterMessage));

		//�����û�
		POSITION Position = NULL;
		CGlobalUserItem * pGlobalUserItem = NULL;

		do
		{
			//��ȡ�û�
			pGlobalUserItem = m_GlobalItemManager.EnumUserItem(pGlobalUserItem);
			if (pGlobalUserItem == NULL) break;

			//���ñ���
			EnterMessage.dwUserID = pGlobalUserItem->GetUserID();
			
			//��������
			ASSERT(m_pITCPSocketCenter != NULL);
			m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_ENTER_MESSAGE, &EnterMessage, sizeof(EnterMessage), 0);

		} while (true);

		//�㱨���
		m_bCollectUser = true;
		m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_COLLECT_FINISH, 0);

		return true;
	}
	}

	return true;
}

//ת������
bool CAttemperEngineSink::OnTCPSocketMainForwardService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	FTLOG(MESSAGE_SERVER_LOG_FILE)->LogInfo(L"[LH][MessageServer][Forward] S:%d", wSubCmdID);	

	switch (wSubCmdID)
	{
	case SUB_CS_MAIL_INSERT:		//�ʼ�����
	{
		//����У��
		ASSERT(wDataSize == sizeof(CMD_CS_MailInsert));
		if (wDataSize != sizeof(CMD_CS_MailInsert)) return true;

		//��ȡ����
		auto pMailInsert = (CMD_CS_MailInsert*)pData;

		//����ṹ
		DBR_GR_UserInsertMail UserInsertMail;
		ZeroMemory(&UserInsertMail,sizeof(UserInsertMail));

		//���ñ���
		UserInsertMail.dwMailID = pMailInsert->dwMailID;
		UserInsertMail.dwUserID = pMailInsert->dwUserID;
		UserInsertMail.LastLogonTime = pMailInsert->LastLogonTime;

		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_INSERT_MAIL, UserInsertMail.dwMailID, 0, 0, &UserInsertMail, sizeof(UserInsertMail));

		return true;
	}
	case SUB_CS_MAIL_NOTIFY:		//�ʼ�֪ͨ
	{
		//����У��
		ASSERT (wDataSize%sizeof(CMD_CS_MailNotify)==0);
		if (wDataSize % sizeof(CMD_CS_MailNotify) != 0) return true;

		//��ȡ����
		WORD wItemCount = wDataSize / sizeof(CMD_CS_MailNotify);
		CMD_CS_MailNotify * pMailNotify = (CMD_CS_MailNotify*)pData;

		for (int i = 0; i < wItemCount; i++)
		{
			//Ⱥ���ʼ�
			if (pMailNotify->dwUserID == 0)
			{
                //����ṹ
                CMD_AN_S_UserMailNotify UserMailNotify;
                ZeroMemory(&UserMailNotify, sizeof(UserMailNotify));

                //�ʼ���Ϣ
                UserMailNotify.dwMailID = pMailNotify->dwMailID;

                //��������
                SendBatchData(MDM_AN_MESSAGE_SERVICE, SUB_AN_S_USER_MAIL_NOTIFY, &UserMailNotify, sizeof(UserMailNotify));
			}
			else
			{
                //�������
                CGlobalUserItem* pClobalUserItem = m_GlobalItemManager.SearchUserItem(pMailNotify->dwUserID);
                if (pClobalUserItem != NULL)
                {
                    //����ṹ
                    CMD_AN_S_UserMailNotify UserMailNotify;
                    ZeroMemory(&UserMailNotify, sizeof(UserMailNotify));

                    //������Ϣ
                    UserMailNotify.dwMailID = pMailNotify->dwMailID;

					//��������
					auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pClobalUserItem->GetGateID());
					if (pGlobalGateItem != NULL)
					{
						//��������
						SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_AN_MESSAGE_SERVICE, SUB_AN_S_USER_MAIL_NOTIFY, &UserMailNotify, sizeof(UserMailNotify), &pClobalUserItem->m_dwTokenID, 1);
					}                    
                }
			}

			pMailNotify++;
		}

		return true;
	}
	case SUB_CS_MATCH_REMIND:		//��������
	{
		//����У��
		ASSERT (sizeof(CMD_CS_MatchRemind)==wDataSize);
		if (sizeof(CMD_CS_MatchRemind)!=wDataSize) return false;

		//��ȡ����
		CMD_CS_MatchRemind* pMatchRemind=(CMD_CS_MatchRemind*)pData;

		//�������
		CGlobalUserItem * pClobalUserItem=m_GlobalItemManager.SearchUserItem(pMatchRemind->dwUserID);
		if (pClobalUserItem!=NULL)
		{
			//����ṹ
			CMD_AN_S_MatchRemind ANMatchRemind;
			ZeroMemory(&ANMatchRemind,sizeof(ANMatchRemind));
				
			//������Ϣ
			ANMatchRemind.lMatchNo=pMatchRemind->lMatchNo;
			ANMatchRemind.dwMatchID=pMatchRemind->dwMatchID;
			ANMatchRemind.dwSurplusTime=pMatchRemind->dwSurplusTime;
			StringCchCopy(ANMatchRemind.szMatchName,CountArray(ANMatchRemind.szMatchName), pMatchRemind->szMatchName);
			CopyMemory(&ANMatchRemind.MatchStartTime,&pMatchRemind->MatchStartTime,sizeof(ANMatchRemind.MatchStartTime));

			//������Ϣ
			ANMatchRemind.wKindID=pMatchRemind->wKindID;
			ANMatchRemind.wServerID=pMatchRemind->wServerID;
			ANMatchRemind.wServerPort=pMatchRemind->wServerPort;
			ANMatchRemind.dwServerAddr=pMatchRemind->dwServerAddr;
			StringCchCopy(ANMatchRemind.szServerDomain,CountArray(ANMatchRemind.szServerDomain), pMatchRemind->szServerDomain);

			//��������
			auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pClobalUserItem->GetGateID());
			if (pGlobalGateItem != NULL)
			{
				//��������
				SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_AN_MESSAGE_SERVICE, SUB_AN_S_MATCH_REMIND, &ANMatchRemind, sizeof(ANMatchRemind), &pClobalUserItem->m_dwTokenID, 1);
			}
		}

		return true;
	}
	case SUB_CS_MATCH_WINAWARD:		//������
	{
		//����У��
		//ASSERT (sizeof(CMD_CS_C_MatchWinAward)==wDataSize);
		//if (sizeof(CMD_CS_C_MatchWinAward)!=wDataSize) return false;

		////��ȡ����
		//CMD_CS_C_MatchWinAward* pMatchWinAward=(CMD_CS_C_MatchWinAward*)pData;

		////��������
		//CString strMessage;
		//CString strMessageItem;

		////��ʽ����Ϣ
		//strMessage.Format(g_pszMsgFormat1,COLOR_NICKNAME,pMatchWinAward->szNickName,COLOR_TEXT,pMatchWinAward->szMatchName,pMatchWinAward->wRankID);

		//////ʵ�ｱ��
		////if (pMatchWinAward->szRewardEntity[0]!=0)
		////{
		////	strMessageItem.Format(TEXT("#%d#%s"),COLOR_WEALTH_NUMBER,pMatchWinAward->szRewardEntity);
		////	strMessage.Append(strMessageItem);
		////}

		////������Ϣ
		//tagScrollMessageItem ScrollMessageItem;
		//ZeroMemory(&ScrollMessageItem,sizeof(ScrollMessageItem));

		////���ñ���
		//ScrollMessageItem.cbType=MSG_TYPE_NORMAL;
		//ScrollMessageItem.wLength=strMessage.GetLength();
		//StringCchCopy(ScrollMessageItem.szContent,ScrollMessageItem.wLength, strMessage.GetString());

		////��Ϣ��С
		//WORD wMessageSize=CountStringBuffer(ScrollMessageItem.szContent);
		//wMessageSize+=sizeof(ScrollMessageItem)-sizeof(ScrollMessageItem.szContent);

		////����ṹ
		//tagScrollMessageItem * pScrollMessageItem=(tagScrollMessageItem *)m_cbBuffer;		

		////���ñ���			
		//CopyMemory(pScrollMessageItem,&ScrollMessageItem,wMessageSize);

		////�����Ϣ
		//m_ScrollMessageArray.Add(ScrollMessageItem);

		////�Ƴ���Ϣ
		//if (m_ScrollMessageArray.GetCount() > MESSAGE_CACHE_COUNT)
		//{
		//	m_ScrollMessageArray.RemoveAt(0);
		//}

		////������Ϣ
		//SendBatchData(MDM_AN_MESSAGE_SERVICE, SUB_AN_S_MESSAGE_LIST, pScrollMessageItem, wMessageSize);
			
		return true;
	}
	case SUB_CS_MARQUEE_NOTICE:		//����ƹ���
	{
		//����У��
		ASSERT (sizeof(CMD_CS_MarqueeNotice)>=wDataSize);
		if (sizeof(CMD_CS_MarqueeNotice) < wDataSize) return false;

		//��ȡ����
		auto pMarqueeNotice =(CMD_CS_MarqueeNotice*)pData;

		//����ʱ
		DWORD dwCurrentTime = time(NULL);
		static DWORD dwLastCleanTime = dwCurrentTime;
		if (dwCurrentTime - dwLastCleanTime >= 300)
		{
			CleanExpireNotice();
			dwLastCleanTime = dwCurrentTime;
		}

		//���ҹ���
		auto pMarqueeNoticeInfo = m_StockMarqueeNoticeInfo.SearchStockItem(pMarqueeNotice->dwNoticeID);
		if (pMarqueeNoticeInfo == NULL)
		{
			//����ṹ
			pMarqueeNoticeInfo = m_StockMarqueeNoticeInfo.CreateStockItem(pMarqueeNotice->dwNoticeID);

			//���ñ���
			pMarqueeNoticeInfo->dwNoticeID = pMarqueeNotice->dwNoticeID;
			pMarqueeNoticeInfo->cbNoticeKind = pMarqueeNotice->cbNoticeKind;
			pMarqueeNoticeInfo->bExclusive = pMarqueeNotice->bExclusive;
			pMarqueeNoticeInfo->cbPriority = pMarqueeNotice->cbPriority;
			pMarqueeNoticeInfo->wRollTimes = pMarqueeNotice->wRollTimes;
			pMarqueeNoticeInfo->wViewPlace = pMarqueeNotice->wViewPlace;
			pMarqueeNoticeInfo->dwEndRollTime = pMarqueeNotice->dwEndRollTime;
			pMarqueeNoticeInfo->dwStartRollTime = pMarqueeNotice->dwStartRollTime;
			pMarqueeNoticeInfo->wNoticeSize = pMarqueeNotice->wNoticeSize;
			CopyMemory(pMarqueeNoticeInfo->cbNoticeContent, pMarqueeNotice->cbNoticeContent, pMarqueeNoticeInfo->wNoticeSize);			
		}
		else
		{
			pMarqueeNoticeInfo->bExclusive = pMarqueeNotice->bExclusive;
			pMarqueeNoticeInfo->cbPriority = pMarqueeNotice->cbPriority;
			pMarqueeNoticeInfo->wRollTimes = pMarqueeNotice->wRollTimes;
			pMarqueeNoticeInfo->wViewPlace = pMarqueeNotice->wViewPlace;
			pMarqueeNoticeInfo->dwEndRollTime = pMarqueeNotice->dwEndRollTime;
			pMarqueeNoticeInfo->dwStartRollTime = pMarqueeNotice->dwStartRollTime;
			pMarqueeNoticeInfo->wNoticeSize = pMarqueeNotice->wNoticeSize;
			CopyMemory(pMarqueeNoticeInfo->cbNoticeContent, pMarqueeNotice->cbNoticeContent, pMarqueeNoticeInfo->wNoticeSize);
		}		

		//��������
		SendBatchData(MDM_AN_MESSAGE_SERVICE, SUB_AN_S_MARQUEE_NOTICE, pMarqueeNoticeInfo, sizeof(tagMarqueeNoticeInfo));

		return true;
	}
	case SUB_CS_SEND_AUTHCODE:		//������֤��
	{
		//����У��
		ASSERT(sizeof(CMD_CS_SendAuthCode) == wDataSize);
		if (sizeof(CMD_CS_SendAuthCode) != wDataSize) return false;

		//��ȡ����
		CMD_CS_SendAuthCode* pSendAuthCode = (CMD_CS_SendAuthCode*)pData;

		//��������
		DWORD dwTimeStamp = (DWORD)time(NULL);
		CStringW strAutoCode(pSendAuthCode->szAuthCode);

		//������֤��
		if (pSendAuthCode->cbCodeKind == CODE_KIND_MP)
		{
			CStringW strMobilePhone(pSendAuthCode->szMobileEmail);

			//��������
			auto nIndex = strMobilePhone.Find('+');
			if (nIndex >= 0) strMobilePhone.Delete(nIndex);

			//��ʽ������
			TCHAR szSign[256];
			_sntprintf_s(szSign, CountArray(szSign), TEXT("phonecode=%ls&phonenumber=%ls&servertime=%d&key=%s"), strAutoCode.GetString(), strMobilePhone.GetString(), dwTimeStamp, szCompilation);

			//���Ƽ���
			TCHAR szSignMD5[LEN_MD5];
			CMD5Encrypt::EncryptData(szSign, szSignMD5);

			//����ṹ
			DBR_GR_RequestWebPage RequestWebPage;
			ZeroMemory(&RequestWebPage, sizeof(RequestWebPage));

			//��������
			RequestWebPage.hrType = Hr_Post;

			//���ñ���			
			_snprintf_s(RequestWebPage.szWebUrl, CountArray(RequestWebPage.szWebUrl), "%s/api/Sms/SendInter", CT2CA(m_pInitParameter->m_szPaltformDomain).m_psz);

			//����ͷ��
			StringCchCopyA(RequestWebPage.szHeader, CountArray(RequestWebPage.szHeader), "content-type:application/json\r\n\r\n");

			//��������
			LPCSTR pszPostFormat = "{\"phonenumber\":\"%ls\",\"phonecode\":\"%ls\",\"servertime\":%d,\"sign\":\"%s\"}";
			_snprintf_s(RequestWebPage.szPostData, CountArray(RequestWebPage.szPostData), pszPostFormat, strMobilePhone.GetString(), strAutoCode.GetString(), dwTimeStamp, CT2CA(szSignMD5).m_psz);

			//Ͷ������
			WORD wDataSize = CountStringBufferA(RequestWebPage.szPostData);
			wDataSize += sizeof(RequestWebPage) - sizeof(RequestWebPage.szPostData);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_REQUEST_WEBPAGE, 0, 0, 0, &RequestWebPage, wDataSize);
		}
		else
		{
			//��������
			DWORD dwTimeStamp = (DWORD)time(NULL);
			CStringW strAutoCode(pSendAuthCode->szAuthCode);
			CStringW strEmail(pSendAuthCode->szMobileEmail);

			//��ʽ������
			TCHAR szToken[256];
			_sntprintf_s(szToken, CountArray(szToken), TEXT("email=%ls&mailtitle=%s&servertime=%d&templatename=sendcode&key=%s"), strEmail.GetString(), TEXT("Email verification code"), dwTimeStamp, szCompilation);

			//���Ƽ���
			TCHAR szTokenMD5[LEN_MD5];
			CMD5Encrypt::EncryptData(szToken, szTokenMD5);

			//����ṹ
			DBR_GR_RequestWebPage RequestWebPage;
			ZeroMemory(&RequestWebPage, sizeof(RequestWebPage));

			//��������
			RequestWebPage.hrType = Hr_Post;

			//���ñ���			
			_snprintf_s(RequestWebPage.szWebUrl, CountArray(RequestWebPage.szWebUrl), "%s/api/Mail/SendMail", CT2A(m_pInitParameter->m_szPaltformDomain).m_psz);

			//����ͷ��
			StringCchCopyA(RequestWebPage.szHeader, CountArray(RequestWebPage.szHeader), "content-type:application/json\r\n\r\n");

			//��������
			LPCSTR pszPostFormat = "{\"email\":\"%ls\",\"mailtitle\":\"%s\",\"templateName\":\"sendcode\",\"templateParam\":{\"code\":\"%ls\"},\"serverTime\":%d,\"sign\":\"%s\"}";
			_snprintf_s(RequestWebPage.szPostData, CountArray(RequestWebPage.szPostData), pszPostFormat, strEmail.GetString(), "Email verification code", strAutoCode.GetString(), dwTimeStamp, CT2CA(szTokenMD5).m_psz);

			//Ͷ������
			WORD wDataSize = CountStringBufferA(RequestWebPage.szPostData);
			wDataSize += sizeof(RequestWebPage) - sizeof(RequestWebPage.szPostData);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_REQUEST_WEBPAGE, 0, 0, 0, &RequestWebPage, wDataSize);
		}

		return true;
	}		
	case SUB_CS_SEND_EXCEPTION_ALARM:	//�쳣Ԥ��
	{
		//����У��
		ASSERT(sizeof(CMD_CS_SendExceptionAlarm) == wDataSize);
		if (sizeof(CMD_CS_SendExceptionAlarm) != wDataSize) return false;

		//��ȡ����
		auto pSendExceptionAlarm = (CMD_CS_SendExceptionAlarm*)pData;

		//��������
		DWORD dwTimeStamp = (DWORD)time(NULL);

		//�ʼ�����
		//LPCSTR pszMailTitle = "DataBase Exception Notification";

		//��������
		WCHAR szMailContent[256];
		_snwprintf_s(szMailContent, CountArray(szMailContent), L"��ע�⣺ϵͳ��⵽[%ls]%ls", CT2CW(pSendExceptionAlarm->szServiceName).m_psz, CT2CW(pSendExceptionAlarm->szExceptionInfo).m_psz);

		//��ʽ������
		WCHAR szSignUnic[256];
		_snwprintf_s(szSignUnic, CountArray(szSignUnic), L"mailcontent=%s&mailtitle=%hs&servertime=%d&key=%s", szMailContent, pSendExceptionAlarm->szAlarmTitle, dwTimeStamp, szCompilation);

		//ת������
		CHAR szSignUtf8[512];
		CWHService::Unicode2Utf8(szSignUnic, szSignUtf8, CountArray(szSignUtf8));

		//ǩ������
		CHAR szSignMD5[LEN_MD5];
		CMD5Encrypt::EncryptData(szSignUtf8, szSignMD5);

		//����ṹ
		DBR_GR_RequestWebPage RequestWebPage;
		ZeroMemory(&RequestWebPage, sizeof(RequestWebPage));

		//��������
		RequestWebPage.hrType = Hr_Post;

		//���ñ���			
		_snprintf_s(RequestWebPage.szWebUrl, CountArray(RequestWebPage.szWebUrl), "%s/api/Mail/SendWarningMail", CT2A(m_pInitParameter->m_szPaltformDomain).m_psz);

		//����ͷ��
		StringCchCopyA(RequestWebPage.szHeader, CountArray(RequestWebPage.szHeader), "content-type:application/json\r\n\r\n");

		//ת������
		CHAR szMailContentUtf8[512];
		CWHService::Unicode2Utf8(szMailContent, szMailContentUtf8, sizeof(szMailContentUtf8));

		//��������			
		LPCSTR pszPostFormat = "{\"mailtitle\":\"%hs\",\"mailcontent\":\"%hs\",\"serverTime\":%d,\"sign\":\"%s\"}";
		_snprintf_s(RequestWebPage.szPostData, CountArray(RequestWebPage.szPostData), pszPostFormat, pSendExceptionAlarm->szAlarmTitle, szMailContentUtf8, dwTimeStamp, szSignMD5);

		//Ͷ������
		WORD wDataSize = CountStringBufferA(RequestWebPage.szPostData);
		wDataSize += sizeof(RequestWebPage) - sizeof(RequestWebPage.szPostData);
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_REQUEST_WEBPAGE, 0, 0, 0, &RequestWebPage, wDataSize);

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
	case SUB_CS_C_REGISTER_SERVICE:		//ע������
		{
			//Ч������
			ASSERT (wDataSize==sizeof(CMD_CS_C_RegisterService));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterService)) return false;

			//��Ϣ����
			CMD_CS_C_RegisterService * pRegisterService =(CMD_CS_C_RegisterService *)pData;

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
				StringCchCopy(RegisterFailure.szDescribeString,CountArray(RegisterFailure.szDescribeString), TEXT("�Ѿ�������ͬ��ʶ�����ط������ط���ע��ʧ��"));

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
			tagServiceItem GateItem;
			ZeroMemory(&GateItem,sizeof(GateItem));

			//��������
			GateItem.wServiceID= pRegisterService->wServiceID;
			GateItem.wServicePort= pRegisterService->wServicePort;
			GateItem.wServiceModule = pRegisterService->wServiceModule;
			GateItem.dwServiceAddr= pRegisterService->dwServiceAddr;
			StringCchCopy(GateItem.szServiceName, CountArray(GateItem.szServiceName), pRegisterService->szServiceName);
			StringCchCopy(GateItem.szServiceDomain, CountArray(GateItem.szServiceDomain), pRegisterService->szServiceDomain);

			//ע�᷿��
			auto pGlobalGateItem=m_GlobalItemManager.ActiveGateItem(dwSocketID,GateItem);

			//ע��ɹ�
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_SUCCESS,NULL,0);	

			//Ⱥ������
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,SERVICE_MODULE_GATE);

			//�ж��û���
			if (m_GlobalItemManager.GetUserItemCount()>0)
			{
				//��������
				POSITION Position=NULL;
				CGlobalUserItem * pGlobalUserItem=NULL;

				do
				{
					//ö���û�
					pGlobalUserItem=m_GlobalItemManager.EnumUserItem(pGlobalUserItem);
					if (pGlobalUserItem==NULL) break;

					//У���ʶ
					if (pGlobalUserItem->m_wGateID== pRegisterService->wServiceID)
					{
						//ɾ���û�
						if (pRegisterService->wConnectCount==0)
						{
							m_GlobalItemManager.DeleteUserItem(pGlobalUserItem->m_dwUserID);
						}
						else
						{
							//�������
							pGlobalGateItem->ActiveTokenID(pGlobalUserItem->m_dwTokenID);
						}
					}

				} while (true);
			}
			else
			{
				//�ռ��û�
				if (pRegisterService->wConnectCount>0)
				{
					m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_COMMON_SERVICE,SUB_CS_S_COLLECT_USER,NULL,0);
				}
			}

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
			tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

			//У�����
			if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return true;

			//��ȡ����
			CMD_CS_C_LogoutToken* pLogoutToken = (CMD_CS_C_LogoutToken*)pData;

			//��������
			CGlobalGateItem* pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
			if (pGlobalGateItem != NULL)
			{
				//���Ҳ���
				auto pTokenParameter = pGlobalGateItem->SearchTokenID(pLogoutToken->dwTokenID);

				//�����û�
				if (pTokenParameter!=NULL && pTokenParameter->pBindUserItem!=NULL && m_bCollectUser == true)
				{
					auto pGlobalUserItem = (CGlobalUserItem *)pTokenParameter->pBindUserItem;

					//��������
					CMD_CS_C_LeaveMessage LeaveMessage;
					ZeroMemory(&LeaveMessage, sizeof(LeaveMessage));

					//���ñ���
					LeaveMessage.dwUserID = pGlobalUserItem->GetUserID();

					//������Ϣ
					ASSERT(m_pITCPSocketCenter != NULL);
					m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_LEAVE_MESSAGE, &LeaveMessage, sizeof(LeaveMessage), 0);
				}

				//ɾ������
				pGlobalGateItem->RemoveTokenID(pLogoutToken->dwTokenID);
			}

			return true;
		}
	case SUB_CS_C_REGISTER_TOKEN:
		{
			//���ð�
			WORD wBindIndex = LOWORD(dwSocketID);
			tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

			//�Ƴ�����
			if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return true;

			//��ȡ����
			CMD_CS_C_RegisterToken* pRegisterToken = (CMD_CS_C_RegisterToken*)pData;

			//����ṹ
			CMD_CS_S_RegisterToken RegisterToken;
			ZeroMemory(&RegisterToken, sizeof(RegisterToken));

			//Ĭ�ϳɹ�
			RegisterToken.lResultCode = REGISTER_RESULT_FAILURE;

			//��������
			CGlobalGateItem* pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
			if (pGlobalGateItem != NULL)
			{
				//��ѯ�û�
				auto pTokenParameter = pGlobalGateItem->ActiveTokenID(pRegisterToken->dwTokenID);
				if (pTokenParameter != NULL)
				{
					//ע��ɹ�
					RegisterToken.lResultCode = REGISTER_RESULT_SUCCESS;

					//��������		
					pTokenParameter->pBindUserItem = NULL;
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

//�ʼ���Ϣ
bool CAttemperEngineSink::OnDBUserMailInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pDBUserMailInfo = (DBO_GR_UserMailInfo*)pData;

	//��ȡ����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL || pTokenParameter->pBindUserItem == NULL) return true;	

	//�������
	CMD_AN_S_UserMailInfo* pUserMailInfo = (CMD_AN_S_UserMailInfo*)m_cbBuffer;
	ZeroMemory(pUserMailInfo, sizeof(CMD_AN_S_UserMailInfo));

	//��������		
	pUserMailInfo->wMailCount = pDBUserMailInfo->wMailCount;
	if (pUserMailInfo->wMailCount > 0)
	{
		CopyMemory(pUserMailInfo->MailInfo, pDBUserMailInfo->MailInfo, pUserMailInfo->wMailCount * sizeof(pUserMailInfo->MailInfo[0]));
	}

	//��������
	WORD wPacketSize = sizeof(CMD_AN_S_UserMailInfo)+ pUserMailInfo->wMailCount * sizeof(pUserMailInfo->MailInfo[0]);	
	SendDataToGate(dwContextID, MDM_AN_MESSAGE_SERVICE, SUB_AN_S_USER_MAIL_INFO, pUserMailInfo, wPacketSize, &dwTokenID, 1);

	return true;
}

//�ʼ����
bool CAttemperEngineSink::OnDBUserMailFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL || pTokenParameter->pBindUserItem == NULL) return true;

	//��������
	SendDataToGate(dwContextID, MDM_AN_MESSAGE_SERVICE, SUB_AN_S_USER_MAIL_FINISH, NULL, 0, &dwTokenID, 1);

	return true;
}

//�ʼ�����
bool CAttemperEngineSink::OnDBUserMailUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pUserMailUpdate = (DBO_GR_UserMailUpdate*)pData;

	//��ȡ����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL || pTokenParameter->pBindUserItem == NULL) return true;

	//�������
	CMD_AN_S_UserMailUpdate UserMailUpdate;
	ZeroMemory(&UserMailUpdate, sizeof(UserMailUpdate));

	//���ñ���
	UserMailUpdate.dwMailID = pUserMailUpdate->dwMailID;
	UserMailUpdate.cbMailState = pUserMailUpdate->cbMailState;

	//��������
	SendDataToGate(dwContextID, MDM_AN_MESSAGE_SERVICE, SUB_AN_S_USER_MAIL_UPDATE, &UserMailUpdate, sizeof(UserMailUpdate), &dwTokenID, 1);

	return true;
}

//�����ʼ�
bool CAttemperEngineSink::OnDBUserMailInsert(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pUserMailInsert = (DBO_GR_UserMailInsert*)pData;

	//�������
	BYTE cbBuffer[SOCKET_PACKET];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//ת������
	tagForwardHead* pForwardHead = (tagForwardHead*)cbBuffer;
	CMD_CS_MailNotify* pMailNotify = (CMD_CS_MailNotify*)(pForwardHead + 1);

	//����ͷ��
	if (pUserMailInsert->dwUserID==0)
	{
		//����ת��ͷ
		pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
		pForwardHead->wItemCount = 1;
		pForwardHead->ForwardItem[0].wServiceID = 0;
		pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;
	}	
	else
	{
		//����ת��ͷ
		pForwardHead->wTarget = FORWARD_TARGET_USER;
		pForwardHead->dwUserID = pUserMailInsert->dwUserID;
		pForwardHead->wItemCount = 1;
		pForwardHead->ForwardItem[0].wServiceID = 0;
		pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;
	}

	//���ñ���
	pMailNotify->dwUserID = pUserMailInsert->dwUserID;
	pMailNotify->dwMailID = pUserMailInsert->dwMailID;

	//��������
	WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_MailNotify);	
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_MAIL_NOTIFY, cbBuffer, wPacketSize, 0);

	return true;
}

//��ȡ����
bool CAttemperEngineSink::OnDBClaimMailAttach(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pClaimMailAttach = (DBO_GR_ClaimMailAttach*)pData;

	//��ȡ����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL || pTokenParameter->pBindUserItem == NULL) return true;

	//�û���Ϣ
	auto pGlobalUserItem = (CGlobalUserItem *)pTokenParameter->pBindUserItem;

	//�����ɹ�
	SendOperateSuccess(pGlobalUserItem, SUB_AN_C_CLAIM_MAIL_ATTACH, pClaimMailAttach->lResultCode, NULL, pClaimMailAttach->szAttachGoods);

	return true;
}

//��Ʒ��Ϣ
bool CAttemperEngineSink::OnDBUserGoodsUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pDBGoodsUpdate = (DBO_GR_GoodsUpdate *)pData;

	//��ȡ����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL || pTokenParameter->pBindUserItem == NULL) return true;

	//�������
	auto pGoodsUpdate=(CMD_CM_GoodsUpdate*)m_cbBuffer;
	ZeroMemory(pGoodsUpdate, sizeof(CMD_CM_GoodsUpdate));

	//���ñ���
	pGoodsUpdate->wGoodsCount = pDBGoodsUpdate->wGoodsCount;
	CopyMemory(pGoodsUpdate->GoodsHoldInfo, pDBGoodsUpdate->GoodsHoldInfo, pGoodsUpdate->wGoodsCount*sizeof(pGoodsUpdate->GoodsHoldInfo[0]));

	//��������
	WORD wPacketSize = sizeof(CMD_CM_GoodsUpdate) + pGoodsUpdate->wGoodsCount * sizeof(pGoodsUpdate->GoodsHoldInfo[0]);
	SendDataToGate(dwContextID, MDM_CM_SYSTEM, SUB_CM_GOODS_UPDATE, pGoodsUpdate, wPacketSize, &dwTokenID, 1);

	return true;
}

//�Ƹ���Ϣ
bool CAttemperEngineSink::OnDBUserWealthUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pDBWealthUpdate = (DBO_GR_WealthUpdate*)pData;

	//��ȡ����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL || pTokenParameter->pBindUserItem == NULL) return true;

	//�������
	auto pWealthUpdate = (CMD_CM_WealthUpdate*)m_cbBuffer;
	ZeroMemory(pWealthUpdate, sizeof(CMD_CM_WealthUpdate));

	//���ñ���
	pWealthUpdate->wItemCount = pDBWealthUpdate->wItemCount;
	CopyMemory(pWealthUpdate->WealthItem, pDBWealthUpdate->WealthItem, pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]));

	//��������
	WORD wPacketSize = sizeof(CMD_CM_WealthUpdate) + pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]);
	SendDataToGate(dwContextID, MDM_CM_SYSTEM, SUB_CM_WEALTH_UPDATE, pWealthUpdate, wPacketSize, &dwTokenID, 1);

	return true;
}

//�����ɹ�
bool CAttemperEngineSink::OnDBOperateSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pOperateSuccess = (DBO_GR_OperateSuccess*)pData;

	//��ȡ����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL || pTokenParameter->pBindUserItem == NULL) return true;

	//�û���Ϣ
	auto pGlobalUserItem = (CGlobalUserItem*)pTokenParameter->pBindUserItem;

	//�����ɹ�
	SendOperateSuccess(pGlobalUserItem, pOperateSuccess->wRequestCmdID, pOperateSuccess->lErrorCode, pOperateSuccess->szDescribeString, NULL);

	return true;
}

//����ʧ��
bool CAttemperEngineSink::OnDBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pOperateFailure = (DBO_GR_OperateFailure*)pData;

	//��ȡ����
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL || pTokenParameter->pBindUserItem == NULL) return true;

	//�û���Ϣ
	auto pGlobalUserItem = (CGlobalUserItem*)pTokenParameter->pBindUserItem;

	//����ʧ��
	SendOperateFailure(pGlobalUserItem, pOperateFailure->wRequestCmdID, pOperateFailure->lErrorCode, pOperateFailure->szDescribeString);

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
bool CAttemperEngineSink::SendBatchData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//��������
	CGlobalGateItem * pGlobalGateItem=NULL;

	//��������
	DWORD * pdwTokenIDArray=NULL; 
	WORD wCellCount=0,wSendCount=0,wTotalCount=0;

	//���ҷ���
	do
	{
		//ö������ 
		pGlobalGateItem=m_GlobalItemManager.EnumGateItem(pGlobalGateItem);
		if (pGlobalGateItem==NULL) break;

		//��������
		if (pGlobalGateItem->GetTokenCount()==0) continue;

		//���ô�С
		pGlobalGateItem->m_TokenIDArray.SetSize(pGlobalGateItem->GetTokenCount());

		//�ռ�����
		WORD wIndex = 0;
		tagTokenParameter* pTokenParameter = NULL;
		do
		{
			auto pTokenCurrent = pTokenParameter;
			pTokenParameter = pGlobalGateItem->EnumTokenID(pTokenCurrent);
			if (pTokenParameter != NULL)
			{
				pGlobalGateItem->m_TokenIDArray.SetAt(wIndex++, pTokenParameter->dwTokenID);
			}

		} while (pTokenParameter!=NULL);

		//���ñ���		
		wSendCount=0;
		wCellCount = pGlobalGateItem->m_TokenIDArray.GetCount();
		wTotalCount = pGlobalGateItem->m_TokenIDArray.GetCount();
		pdwTokenIDArray= pGlobalGateItem->m_TokenIDArray.GetData();

		//�з�����
		while ((sizeof(WORD)+ wCellCount *sizeof(DWORD)+wDataSize)>SOCKET_PACKET)
		{
			wCellCount = wCellCount /2;
		}

		//��������
		if (wCellCount > TOKEN_COUNT) wCellCount = TOKEN_COUNT;

		//ѭ������
		while (wSendCount< wTotalCount)
		{		
			//��������
			if (wTotalCount - wSendCount < wCellCount)
			{
				wCellCount= wTotalCount - wSendCount;
			}								

			//��������
			SendDataToGate(pGlobalGateItem->GetSocketID(), MAKECMMD(wMainCmdID, SERVICE_MODULE_MESSAGE),wSubCmdID, pData,wDataSize, pdwTokenIDArray + wSendCount, wCellCount);

			//��������			
			wSendCount += wCellCount;
		}

		//�������
		pGlobalGateItem->m_TokenIDArray.RemoveAll();

	} while (true);

	return true;
}

//���ط���
bool CAttemperEngineSink::SendDataToGate(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	//��������
	m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(wMainCmdID,SERVICE_MODULE_MESSAGE),wSubCmdID,pData,wDataSize, pdwTokenID, wTokenCount);

	return true;
}


//�������
VOID CAttemperEngineSink::CleanExpireNotice()
{
	//��������
	DWORD  dwCurrentTime = (DWORD)time(NULL);

	//ɾ������
	auto pMarqueeNoticeItem = m_StockMarqueeNoticeInfo.GetHeadStockItem();
	while (pMarqueeNoticeItem != NULL)
	{
		auto pLastMarqueeNoticeItem = pMarqueeNoticeItem;
		pMarqueeNoticeItem = m_StockMarqueeNoticeInfo.GetNextStockItem(pLastMarqueeNoticeItem);

		//У��ʱ��
		if (pLastMarqueeNoticeItem->dwEndRollTime!=0 && pLastMarqueeNoticeItem->dwEndRollTime <= dwCurrentTime)
		{
			m_StockMarqueeNoticeInfo.RemoveStockItem(pLastMarqueeNoticeItem->dwNoticeID);
		}
	}
}

//���Ʋ���
tagTokenParameter* CAttemperEngineSink::GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ��Ϣ
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

	//ת��ģʽ
	if (pBindParameter->wServiceModule == SERVICE_MODULE_GATE)
	{
		//��ѯ����
		CGlobalGateItem* pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem == NULL) return NULL;

		//��ѯ�û�
		auto pTokenParameter = pGlobalGateItem->SearchTokenID(dwTokenID);
		if (pTokenParameter == NULL || pTokenParameter->dwTokenID != dwTokenID) return NULL;

		return pTokenParameter;
	}

	return NULL;
}

//�����ɹ�
VOID CAttemperEngineSink::SendOperateSuccess(CGlobalUserItem* pGLobalUserItem, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe, LPCTSTR pszGoodsList)
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

	//��������
	auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGLobalUserItem->GetGateID());
	if (pGlobalGateItem != NULL)
	{
		//��������
		DWORD dwTokenID = pGLobalUserItem->GetTokenID();
		WORD wDataSize = sizeof(CMD_CM_OperateSuccess) + SendPacket.GetDataSize();

		//��������
		SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_CM_SYSTEM, SUB_CM_OPERATE_SUCCESS, pRequestSuccess, wDataSize, &dwTokenID, 1);
	}	

	return;
}

//����ʧ��
VOID CAttemperEngineSink::SendOperateFailure(CGlobalUserItem* pGLobalUserItem, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe)
{
	//��������
	auto pOperateFailure = (CMD_CM_OperateFailure*)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer + sizeof(CMD_CM_OperateFailure), sizeof(m_cbBuffer) - sizeof(CMD_CM_OperateFailure));

	//���ñ���
	pOperateFailure->lErrorCode = lErrorCode;
	pOperateFailure->wRequestCmdID = wRequestCmdID;

	//������Ϣ
	if (pszDescribe != NULL)  SendPacket.AddPacket(pszDescribe, DTP_CM_DESCRIBE);

	//��������
	auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGLobalUserItem->GetGateID());
	if (pGlobalGateItem != NULL)
	{
		//��������
		DWORD dwTokenID = pGLobalUserItem->GetTokenID();
		WORD wDataSize = sizeof(CMD_CM_OperateSuccess) + SendPacket.GetDataSize();

		//��������
		SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_CM_SYSTEM, SUB_CM_OPERATE_FAILURE, pOperateFailure, wDataSize, &dwTokenID, 1);
	}

	return;
}

//�ʼ�����
bool CAttemperEngineSink::OnTCPNetWorkMainMessageService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//��ȡ����
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

	switch (wSubCmdID)
	{
	case SUB_AN_C_ENTER_MESSAGE:		//�������
	{
		//��ȡ����
		auto pTokenParameter = GetTokenParameter(dwSocketID,dwTokenID);
		if (pTokenParameter == NULL) return true;

		//����Ч��
		ASSERT (pBindParameter->wServiceModule==SERVICE_MODULE_GATE);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return false;

		//��ѯ����
		CGlobalGateItem * pGlobalGateItem=m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem==NULL) return false;

		//��ѯ�û�
		CGlobalUserItem * pGlobalUserItem=m_GlobalItemManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pGlobalUserItem!=NULL)
		{
			//���ñ���
			pGlobalUserItem->m_dwTokenID= dwTokenID;
			pGlobalUserItem->m_wGateID=pBindParameter->wServiceID;
		}
		else
		{
			//����ṹ
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo,sizeof(GlobalUserInfo));

			//���ñ���
			GlobalUserInfo.dwUserID = pTokenParameter->dwUserID;
			GlobalUserInfo.SocketInfo.dwTokenID = dwTokenID;
			GlobalUserInfo.SocketInfo.wGateID = pBindParameter->wServiceID;			

			//�����û�
			pGlobalUserItem=m_GlobalItemManager.ActiveUserItem(GlobalUserInfo);

		}

		//���û�		
		pTokenParameter->pBindUserItem = pGlobalUserItem;

        //����ɹ�
        SendDataToGate(dwSocketID, MDM_AN_MESSAGE_SERVICE, SUB_AN_S_ENTER_SUCCESS, NULL, 0, &dwTokenID, 1);

		//��������
		WORD wPacketSize = 0;

		//�������
		CleanExpireNotice();
		
		//��������
		auto pMarqueeNoticeItem = m_StockMarqueeNoticeInfo.GetHeadStockItem();
		while (pMarqueeNoticeItem != NULL)
		{
			auto pLastMarqueeNoticeItem = pMarqueeNoticeItem;
			pMarqueeNoticeItem = m_StockMarqueeNoticeInfo.GetNextStockItem(pLastMarqueeNoticeItem);

			//���ñ���				
			CopyMemory(&m_cbBuffer[wPacketSize], pLastMarqueeNoticeItem, sizeof(tagMarqueeNoticeInfo));

			//���ô�С
			wPacketSize += sizeof(tagMarqueeNoticeInfo);

			//�˳��ж�
			if (wPacketSize + sizeof(tagMarqueeNoticeInfo) >= sizeof(m_cbBuffer)) break;
		}

		//������Ϣ
		if (wPacketSize > 0)
		{
			SendDataToGate(dwSocketID, MDM_AN_MESSAGE_SERVICE, SUB_AN_S_MARQUEE_NOTICE, m_cbBuffer, wPacketSize, &dwTokenID, 1);
		}

		//�����û�
		if (m_bCollectUser == true)
		{
			//��������
			CMD_CS_C_EnterMessage EnterMessage;
			ZeroMemory(&EnterMessage, sizeof(EnterMessage));

			//���ñ���
			EnterMessage.dwUserID = pTokenParameter->dwUserID;

			//������Ϣ
			ASSERT(m_pITCPSocketCenter != NULL);
			m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_ENTER_MESSAGE, &EnterMessage, sizeof(EnterMessage), 0);
		}

		return true;
	}
	case SUB_AN_C_LEAVE_MESSAGE:		//�뿪����
	{
		//��ȡ����
		auto pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return true;

		//����Ч��
		ASSERT (pBindParameter->wServiceModule==SERVICE_MODULE_GATE);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return false;

		//�����û�
		if (m_bCollectUser == true)
		{
			auto pGlobalUserItem = (CGlobalUserItem *)pTokenParameter->pBindUserItem;

			//��������
			CMD_CS_C_LeaveMessage LeaveMessage;
			ZeroMemory(&LeaveMessage, sizeof(LeaveMessage));

			//���ñ���
			LeaveMessage.dwUserID = pTokenParameter->dwUserID;

			//������Ϣ
			ASSERT(m_pITCPSocketCenter != NULL);
			m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_LEAVE_MESSAGE, &LeaveMessage, sizeof(LeaveMessage), 0);
		}

		//����û�
		pTokenParameter->pBindUserItem = NULL;

		//ɾ���û�
		m_GlobalItemManager.DeleteUserItem(pTokenParameter->dwUserID);

		return true;
	}
	case SUB_AN_C_USER_QUERY_MAIL:		//��ѯ�ʼ�
	{
		//����У��
		ASSERT (wDataSize == sizeof(CMD_AN_C_UserQueryMail));
		if (wDataSize != sizeof(CMD_AN_C_UserQueryMail)) return true;

		//��ȡ����
		auto pTokenParameter = GetTokenParameter(dwSocketID,dwTokenID);
		if (pTokenParameter == NULL) return true;

		//��ȡ����
		CMD_AN_C_UserQueryMail* pUserQueryMail = (CMD_AN_C_UserQueryMail*)pData;

		//��������
		DBR_GR_UserQueryMail UserQueryMail;
		ZeroMemory(&UserQueryMail, sizeof(UserQueryMail));

		//���ñ���
		UserQueryMail.dwUserID = pTokenParameter->dwUserID;
		UserQueryMail.dwLatestMailID = pUserQueryMail->dwLatestMailID;			

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_QUERY_MAIL, UserQueryMail.dwUserID, dwSocketID, dwTokenID, &UserQueryMail, sizeof(UserQueryMail));

		return true;
	}
	case SUB_AN_C_USER_VIEW_MAIL:		//�鿴�ʼ�
	{
		//����У��
		ASSERT(wDataSize == sizeof(CMD_AN_C_UserViewMail));
		if (wDataSize != sizeof(CMD_AN_C_UserViewMail)) return true;

		//��ȡ����
		auto pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return true;

		//��ȡ����
		CMD_AN_C_UserViewMail* pUserQueryMail = (CMD_AN_C_UserViewMail*)pData;

		//��������
		DBR_GR_UserModifyMail UserModifyMail;
		ZeroMemory(&UserModifyMail, sizeof(UserModifyMail));

		//���ñ���
		UserModifyMail.dwUserID = pTokenParameter->dwUserID;
		UserModifyMail.dwMailID = pUserQueryMail->dwMailID;
		UserModifyMail.cbMailState = MAIL_STATE_HAVETOSEE;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_MODIFY_MAIL, UserModifyMail.dwUserID, dwSocketID, dwTokenID, &UserModifyMail, sizeof(UserModifyMail));

		return true;
	}
	case SUB_AN_C_CLAIM_MAIL_ATTACH:	//��ȡ����
	{
		//����У��
		ASSERT(wDataSize == sizeof(CMD_AN_C_ClaimMailAttach));
		if (wDataSize != sizeof(CMD_AN_C_ClaimMailAttach)) return true;

		//��ȡ����
		auto pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return true;

		//��ȡ����
		CMD_AN_C_ClaimMailAttach* pClaimMailAttach = (CMD_AN_C_ClaimMailAttach*)pData;

		//��������
		DBR_GR_ClaimMailAttach ClaimMailAttach;
		ZeroMemory(&ClaimMailAttach, sizeof(ClaimMailAttach));

		//���ñ���
		ClaimMailAttach.dwUserID = pTokenParameter->dwUserID;
		ClaimMailAttach.dwMailID = pClaimMailAttach->dwMailID;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_CLAIM_MAIL_ATTACH, ClaimMailAttach.dwUserID, dwSocketID, dwTokenID, &ClaimMailAttach, sizeof(ClaimMailAttach));

		return true;
	}
	case SUB_AN_C_USER_DELETE_MAIL:		//ɾ���ʼ�
	{
		//����У��
		ASSERT(wDataSize == sizeof(CMD_AN_C_UserDeleteMail));
		if (wDataSize != sizeof(CMD_AN_C_UserDeleteMail)) return true;

		//��ȡ����
		auto pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return true;

		//��ȡ����
		CMD_AN_C_UserDeleteMail* pUserDeleteMail = (CMD_AN_C_UserDeleteMail*)pData;

		//��������
		DBR_GR_UserDeleteMail DeleteMail;
		ZeroMemory(&DeleteMail, sizeof(DeleteMail));

		//���ñ���
		DeleteMail.dwUserID = pTokenParameter->dwUserID;
		DeleteMail.dwMailID = pUserDeleteMail->dwMailID;

		//Ͷ������
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_DELETE_MAIL, DeleteMail.dwUserID, dwSocketID, dwTokenID, &DeleteMail, sizeof(DeleteMail));

		return true;
	}
	}

	return false;
}

//������־
bool CAttemperEngineSink::InitLoggerService()
{
	//ʹ������
	g_TimeLoggerService->EnableLocal(false);
	g_TimeLoggerService->EnableRemote(true);
	g_TimeLoggerService->EnablePrefix(false);
	g_TimeLoggerService->EnableSufix(true);

	//������Ϣ
	g_TimeLoggerService->SetFormat(TEXT("%Y%m%d"));
	g_TimeLoggerService->SetConcatStr(TEXT("_"));
	g_TimeLoggerService->SetSeparator(TEXT("|"));
	g_TimeLoggerService->SetLoggerInfo(TEXT("ServiceLog"), MESSAGE_SERVER_LOG_FILE, m_pInitParameter->m_szLoggerPath);

	//��������
	if (g_TimeLoggerService->StartService() == false)
	{
		ASSERT(FALSE);

		return false;
	}

	//������־
	g_TimeLoggerService->InitFormat(MESSAGE_SERVER_LOG_FILE, TEXT("%Y%m%d"), new CLogByDay());

	return true;
}
//////////////////////////////////////////////////////////////////////////
