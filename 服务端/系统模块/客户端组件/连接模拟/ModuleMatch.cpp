#include "Stdafx.h"
#include "ModuleMatch.h"
#include "ModuleServer.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//ʱ�Ӷ���

#define	IDI_TIME_PULSE				1								//ʱ������

//////////////////////////////////////////////////////////////////////////////////////////////

//���캯��
CModuleMatch::CModuleMatch(IModuleItemSink* pIModuleItemSink):
	m_pIModuleItemSink(pIModuleItemSink)
{
	//����ģ��
	SetServiceModule(SERVICE_MODULE_MATCH);

	//���ñ���
	ZeroMemory(&m_BufferPacket,sizeof(m_BufferPacket));	
	ZeroMemory(&m_MatchGroupInfo,sizeof(m_MatchGroupInfo));
	ZeroMemory(&m_MatchViewOption, sizeof(m_MatchViewOption));
}

//��������
CModuleMatch::~CModuleMatch()
{
}

//�ر��¼�
bool CModuleMatch::OnEventVirtualShut()
{
	//���ñ���
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));

	//Ͷ����Ϣ	
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_MATCH_LINK_SHUT, 0, 0);

	//������Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ��������������������ӶϿ���"), m_pIModuleItemSink->GetGlobalUserData()->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(255, 0, 0));

	return true;
}

//�����¼�
bool CModuleMatch::OnEventVirtualLink(LONG lErrorCode)
{
	//����ʧ��
	if (lErrorCode != 0)
	{
		//Ͷ����Ϣ	
		ASSERT(m_pIModuleItemSink != NULL);
		m_pIModuleItemSink->PostWindowMessage(WM_MATCH_LINK_FAILURE, 0, 0);

		return true;
	}

	//��������
	if (m_BufferPacket.wMainCmdID != 0 && m_BufferPacket.wSubCmdID != 0)
	{
		SendVirtualData(m_BufferPacket.wMainCmdID, m_BufferPacket.wSubCmdID, m_BufferPacket.cbDataBuffer, m_BufferPacket.wDataSize);
	}	

	return true;
}

//ɾ��ʱ��
bool CModuleMatch::KillModuleTimer(UINT nTimerID)
{
	//Ͷ����Ϣ
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->KillGameTimer(GetTimerStart() + nTimerID);

	return true;
}

//����ʱ��
bool CModuleMatch::SetModuleTimer(UINT nTimerID, UINT nElapse)
{
	//Ͷ����Ϣ
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->SetGameTimer(GetTimerStart() + nTimerID, nElapse);

	return true;
}

//ʱ���¼�
bool CModuleMatch::OnEventTimer(UINT uTimerID, WPARAM wBindParam)
{
	//ʱ������
	if (uTimerID == IDI_TIME_PULSE)
	{
		//��������
		DWORD dwTimeStamp = (DWORD)time(NULL);

		//����ģ��
		auto pModuleServer = (CModuleServer *)m_pIModuleItemSink->GetModuleServer();

		//���뷿��
		if (m_MatchGroupInfo.dwMatchID != 0 && pModuleServer->GetLinkStatus()==false)
		{
			//������
			if (m_MatchGroupInfo.cbMatchType == MATCH_TYPE_TOURNAMENT)
			{
				//����ṹ
				auto pTournamentGroupDetail = (tagTournamentGroupDetail*)(m_MatchGroupInfo.cbGroupDetail);

				//��ȡʱ��
				CTime TimeCurrent = CTime::GetCurrentTime();
				CTime TimeMatchStart(pTournamentGroupDetail->MatchStartTime);

				//У��ʱ��
				if (TimeCurrent > pTournamentGroupDetail->MatchStartTime && m_MatchGroupInfo.dwMatchTicket>0)
				{
					//����ṹ
					CMD_AM_C_QueryMatchSession QueryMatchSession;
					ZeroMemory(&QueryMatchSession,sizeof(QueryMatchSession));

					//���ñ���
					QueryMatchSession.dwMatchID = m_MatchGroupInfo.dwMatchID;

					//��������
					SendVirtualData(MDM_AM_MATCH_SERVICE, SUB_AM_C_QUERY_MATCH_SESSION, &QueryMatchSession, sizeof(QueryMatchSession));

					//�رն�ʱ��
					KillModuleTimer(IDI_TIME_PULSE);
				}
			}
		}

		//�Ѿ����뷿��
		if (pModuleServer->GetLinkStatus() == true && GetLinkStatus()==true)
		{
			//3���ѯһ���û�
			if (dwTimeStamp % 1 == 0)
			{
				//����ṹ
				CMD_AM_C_QuerySignupUser QuerySignupUser;
				ZeroMemory(&QuerySignupUser, sizeof(QuerySignupUser));

				//���ñ���
				QuerySignupUser.dwMatchID = m_MatchGroupInfo.dwMatchID;
				QuerySignupUser.lMatchNo = m_MatchGroupInfo.lMatchNo;

				//��������
				SendVirtualData(MDM_AM_MATCH_SERVICE, SUB_AM_C_QUERY_SIGNUP_USER, &QuerySignupUser, sizeof(QuerySignupUser));
			}			

			//5���ѯһ������
			if (dwTimeStamp % 2 == 0)
			{
				//����ṹ
				CMD_AM_C_QueryMatchTable QueryMatchTable;
				ZeroMemory(&QueryMatchTable, sizeof(QueryMatchTable));

				//���ñ���
				QueryMatchTable.dwMatchID = m_MatchGroupInfo.dwMatchID;
				QueryMatchTable.lMatchNo = m_MatchGroupInfo.lMatchNo;

				//��������
				SendVirtualData(MDM_AM_MATCH_SERVICE, SUB_AM_C_QUERY_MATCH_TABLE, &QueryMatchTable, sizeof(QueryMatchTable));
			}
		}
	}

	return true;
}

//��ȡ�¼�
bool CModuleMatch::OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_CM_SYSTEM:					//ϵͳ����
	{
		return OnSocketMainSystem(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_AM_MATCH_SERVICE:			//��������
	{
		return OnSocketMainMatchService(Command.wSubCmdID, pData, wDataSize);
	}		
	}

	return true;
}

//ϵͳ����
bool CModuleMatch::OnSocketMainSystem(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CM_SYSTEM_MESSAGE:		//ϵͳ��Ϣ
	{
		return OnSocketSubSystemMessage(pData, wDataSize);
	}
	case SUB_CM_OPERATE_FAILURE:	//����ʧ��
	{
		return OnSocketSubOperateFailure(pData, wDataSize);
	}
	}

	return true;
}

//��¼����
bool CModuleMatch::OnSocketMainMatchService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_AM_S_MATCH_OPTION_LIST:	//�����б�
	{
		return OnSocketSubMatchOptionList(pData, wDataSize);
	}
	case SUB_AM_S_MATCH_OPTION_STATUS:	//����״̬
	{
		return OnSocketSubMatchOptionStatus(pData, wDataSize);
	}
	case SUB_AM_S_MATCH_SESSION_ITEM:		//��������
	{
		return OnSocketSubMatchSessionItem(pData, wDataSize);
	}
	case SUB_AM_S_MATCH_SESSION_LIST:		//�����б�
	{
		return OnSocketSubMatchSessionList(pData, wDataSize);
	}
	case SUB_AM_S_MATCH_SESSION_DETAIL:	//��������
	{
		return OnSocketSubMatchSessionDetail(pData, wDataSize);
	}
	case SUB_AM_S_MATCH_SESSION_STATUS:	//����״̬
	{
		return OnSocketSubMatchSessionStatus(pData, wDataSize);
	}
	case SUB_AM_S_MATCH_ENTER_FINISH:	//�������
	{
		return OnSocketSubEnterFinish(pData, wDataSize);
	}
	case SUB_AM_S_BUY_TCIKET_RESULT:	//������
	{
		return OnSocketSubBuyTickResult(pData, wDataSize);
	}
	case SUB_AM_S_QUERY_TCIKET_RESULT:	//��ѯ���
	{
		return OnSocketSubQueryTickResult(pData, wDataSize);
	}
	}


	return true;
}

//�������
bool CModuleMatch::OnSocketSubEnterFinish(VOID* pData, WORD wDataSize)
{
	//���ö�ʱ��
	SetModuleTimer(IDI_TIME_PULSE, 1000);

	//�û��ӿ�
	ASSERT(m_pIModuleItemSink != NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ��������������ɹ�..."), pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

//������
bool CModuleMatch::OnSocketSubBuyTickResult(VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pBuyTicketResult = (CMD_AM_S_BuyTicketResult*)pData;

	//У�鳡��
	if (pBuyTicketResult->dwMatchID == m_MatchGroupInfo.dwMatchID && pBuyTicketResult->lMatchNo == m_MatchGroupInfo.lMatchNo)
	{
		m_MatchGroupInfo.dwMatchTicket = pBuyTicketResult->dwTicketCount;		
	}

	return true;
}

//��ѯ���
bool CModuleMatch::OnSocketSubQueryTickResult(VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pQueryTicketResult = (CMD_AM_S_QueryTicketResult*)pData;

	//У�鳡��
	if (pQueryTicketResult->dwMatchID == m_MatchGroupInfo.dwMatchID && pQueryTicketResult->lMatchNo == m_MatchGroupInfo.lMatchNo)
	{
		m_MatchGroupInfo.dwMatchTicket = pQueryTicketResult->dwTicketCount;
	}

	//������ȯ
	if (m_MatchGroupInfo.dwMatchTicket == 0)
	{
		//����ṹ
		CMD_AM_C_BuyMatchTicket BuyMatchTicket;
		ZeroMemory(&BuyMatchTicket, sizeof(BuyMatchTicket));

		//���ñ���
		BuyMatchTicket.lMatchNo = m_MatchGroupInfo.lMatchNo;
		BuyMatchTicket.dwMatchID = m_MatchGroupInfo.dwMatchID;

		//��ѯ����ȯ
		SendVirtualData(MDM_AM_MATCH_SERVICE, SUB_AM_C_BUY_MATCH_TICKET, &BuyMatchTicket, sizeof(BuyMatchTicket));
	}	

	return true;
}

//��������
bool CModuleMatch::OnSocketSubMatchSessionItem(VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pMatchSessionItem = (CMD_AM_S_MatchSessionItem*)pData;	

	//��ȡģ��
	ASSERT(m_pIModuleItemSink != NULL);	
	auto pModuleServer = (CModuleServer*)m_pIModuleItemSink->GetModuleServer();	

	//������Ϣ
	SCORE lMatchNo = pMatchSessionItem->MatchSessionItem.lMatchNo;
	DWORD dwMatchID = pMatchSessionItem->MatchSessionItem.dwMatchID;

	//���뷿��
	pModuleServer->EnterMatchServer(m_MatchViewOption.wKindID, pMatchSessionItem->MatchSessionItem.wServerID, dwMatchID, lMatchNo);

	//�ر�����
	CloseVirtualLink();

	return true;
}

//�����б�
bool CModuleMatch::OnSocketSubMatchSessionList(VOID* pData, WORD wDataSize)
{
	return true;
}

//��������
bool CModuleMatch::OnSocketSubMatchSessionDetail(VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pMatchSessionDetail = (CMD_AM_S_MatchSessionDetail*)pData;

	//���ñ���
	m_MatchGroupInfo.dwMatchID = pMatchSessionDetail->dwMatchID;
	m_MatchGroupInfo.lMatchNo = pMatchSessionDetail->lMatchNo;
	m_MatchGroupInfo.cbMatchType = pMatchSessionDetail->cbMatchType;
	m_MatchGroupInfo.wStartCount = pMatchSessionDetail->wStartCount;
	m_MatchGroupInfo.wSignupCount = pMatchSessionDetail->wSignupCount;

	//��չ����
	CopyMemory(m_MatchGroupInfo.cbGroupDetail, pMatchSessionDetail + 1, wDataSize-sizeof(CMD_AM_S_MatchSessionDetail));

	//����ṹ
	CMD_AM_C_QueryMatchTicket QueryMatchTicket;
	ZeroMemory(&QueryMatchTicket,sizeof(QueryMatchTicket));

	//���ñ���
	QueryMatchTicket.lMatchNo = m_MatchGroupInfo.lMatchNo;
	QueryMatchTicket.dwMatchID = m_MatchGroupInfo.dwMatchID;	

	//��ѯ����ȯ
	SendVirtualData(MDM_AM_MATCH_SERVICE,SUB_AM_C_QUERY_MATCH_TICKET,&QueryMatchTicket,sizeof(QueryMatchTicket));

	return true;
}

//����״̬
bool CModuleMatch::OnSocketSubMatchSessionStatus(VOID* pData, WORD wDataSize)
{
	return true;
}

//�����б�
bool CModuleMatch::OnSocketSubMatchOptionList(VOID* pData, WORD wDataSize)
{
	return true;
}

//����״̬
bool CModuleMatch::OnSocketSubMatchOptionStatus(VOID* pData, WORD wDataSize)
{
	return true;
}

//ϵͳ��Ϣ
bool CModuleMatch::OnSocketSubSystemMessage(VOID* pData, WORD wDataSize)
{
	//��������
	CMD_CM_SystemMessage* pSystemMessage = (CMD_CM_SystemMessage*)pData;
	WORD wHeadSize = sizeof(CMD_CM_SystemMessage) - sizeof(pSystemMessage->szString);

	//Ч�����
	ASSERT((wDataSize > wHeadSize) && (wDataSize == (wHeadSize + pSystemMessage->wLength * sizeof(TCHAR))));
	if ((wDataSize <= wHeadSize) || (wDataSize != (wHeadSize + pSystemMessage->wLength * sizeof(TCHAR)))) return false;

	//�رմ���
	if ((pSystemMessage->wType & (SMT_CLOSE_LINK)) != 0)
	{
		//�ر�����
		CloseVirtualLink();
	}

	return true;
}

//����ʧ��
bool CModuleMatch::OnSocketSubOperateFailure(VOID* pData, WORD wDataSize)
{
	//��ȡ����
	auto pOperateFailure = (CMD_CM_OperateFailure*)pData;

	//�û��ӿ�
	ASSERT(m_pIModuleItemSink != NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//��ӡ��Ϣ
	/*CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ����ʧ��[%s]..."), pGlobalUserData->szAccounts, pOperateFailure->szDescribeString);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));*/

	//�ر�����
	CloseVirtualLink();

	return true;
}

//�������
bool CModuleMatch::EnterService()
{
	//�û��ӿ�
	ASSERT(m_pIModuleItemSink != NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//У��״̬
	if (GetLinkStatus() == true)
	{
		SendVirtualData(MDM_AM_MATCH_SERVICE, SUB_AM_C_ENTER_MATCH, NULL, 0);
	}
	else
	{
		//��������
		m_BufferPacket.wMainCmdID = MDM_AM_MATCH_SERVICE;
		m_BufferPacket.wSubCmdID = SUB_AM_C_ENTER_MATCH;
		m_BufferPacket.wDataSize = 0;		

		//��������
		CreateVirtualLink(INVALID_WORD, pGlobalUserData->dwUserID, pGlobalUserData->szLogonToken, NULL, 0);
	}

	//��ӡ��Ϣ
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: ���ڵ�¼����������..."), pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

//����ģ��
VOID CModuleMatch::ResetModule()
{
	//���ñ���
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));
	ZeroMemory(&m_MatchGroupInfo, sizeof(m_MatchGroupInfo));
	ZeroMemory(&m_MatchViewOption, sizeof(m_MatchViewOption));
}
//////////////////////////////////////////////////////////////////////////////////////////////
