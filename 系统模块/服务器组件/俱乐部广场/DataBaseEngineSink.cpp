#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBaseEngineSink.h"
#include <afxinet.h>

//////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//��������
CDataBaseEngineSink::~CDataBaseEngineSink()
{
}

//�ӿڲ�ѯ
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	try
	{
		//��������
		if ((m_GameBattleDBModule.GetInterface() == NULL) && (m_GameBattleDBModule.CreateInstance() == false))
		{
			ASSERT(FALSE);
			return false;
		}

		//��������		
		auto pIMClubDBParameter=&m_pInitParameter->m_GameBattleDBParameter;

		//��������
		m_GameBattleDBModule->SetConnectionInfo(pIMClubDBParameter->dwDataBaseAddr,pIMClubDBParameter->wDataBasePort,
		CW2CT(pIMClubDBParameter->szDataBaseName),CW2CT(pIMClubDBParameter->szDataBaseUser),CW2CT(pIMClubDBParameter->szDataBasePass));
	
		//��������
		m_GameBattleDBModule->OpenConnection();
		m_GameBattleDBAide.SetDataBase(m_GameBattleDBModule.GetInterface());		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//ֹͣ�¼�
bool CDataBaseEngineSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	//���ö���
	m_GameBattleDBAide.SetDataBase(NULL);

	//�ر�����
	if (m_GameBattleDBModule.GetInterface()!=NULL)
	{
		m_GameBattleDBModule->CloseConnection();
		m_GameBattleDBModule.CloseInstance();
	}

	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//ʱ���¼�
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	switch (wRequestID)
	{
	case DBR_GR_LOGON_SERVICE:		//�û���¼
	{
		return OnRequestLogonService(dwContextID, dwTokenID,pData,wDataSize);
	}
	case DBR_GR_CREATE_CLUB:		//����Ⱥ��
	{
		return OnRequestCreateClub(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_UPDATE_CLUB:		//����Ⱥ��
	{
		return OnRequestUpdateClub(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_DELETE_CLUB:		//ɾ��Ⱥ��
	{
		return OnRequestDeleteClub(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_SEARCH_CLUB:		//����Ⱥ��
	{
		return OnRequestSearchClub(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_TRANSFER_CLUB:		//ת��Ⱥ��
	{
		return OnRequestTransferClub(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_UPDATE_MEMBER:		//���³�Ա
	{
		return OnRequestUpdateMember(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_DELETE_MEMBER:		//ɾ����Ա
	{
		return OnRequestDeleteMember(dwContextID, dwTokenID, pData,wDataSize);
	}	
	case DBR_GR_APPLY_REQUEST:		//��������
	{
		return OnRequestApplyRequest(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_APPLY_RESPOND:		//������Ӧ
	{
		return OnRequestApplyRespond(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_APPLY_DELETE:		//����ɾ��
	{
	return OnRequestApplyDelete(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_APPLY_REPEAL:		//����ظ�
	{
	return OnRequestApplyRepeal(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_SETTLE_BATTLE:		//Լս����
	{
	return OnRequestSettleBattle(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_MODIFY_SCORE:		//�޸Ĳ����
	{
	return OnRequestModifyScore(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_PRESENT_SCORE:		//���͹ݷ�
	{
	return OnRequestPresentScore(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_DELETE_RECORD:		//ɾ����¼
	{
	return OnRequestDeleteRecord(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_DETECT_CLUBNAME:	//�������
	{
	return OnRequestDetectClubName(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_CLEANUP_SCORE:		//�������
	{
		return OnRequestCleanupScore(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_CLEANUP_REVENUE:	//���˰��
	{
		return OnRequestCleanupRevenue(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_CLEANUP_SETUP:		//��������
	{
		return OnRequestCleanupSetup(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_CLEANUP_RANKLIST:	//�����
	{
		return OnRequestCleanupRankList(dwContextID, dwTokenID, pData, wDataSize);
	}	
	case DBR_GR_REQUEST_MERGE:		//����ϲ�
	{
		return OnRequestRequestMerge(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_RESPOND_MERGE:		//��Ӧ�ϲ�
	{
		return OnRequestRespondMerge(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_CANCEL_PAYROLL:		//ȡ������
	{
		return OnRequestCancelPayroll(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_REQUEST_PAYROLL:	//�������
	{
		return OnRequestRequestPayroll(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_RESPOND_PAYROLL:	//��Ӧ����
	{
		return OnRequestRespondPayroll(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_LOAD_CLUB_OPTION:	//��������
	{
		return OnRequestLoadClubOption(dwContextID, dwTokenID, pData,wDataSize);
	}		
	case DBR_GR_LOAD_FORBID_WORDS:  //��ֹ�ʻ�
	{
		return OnRequestLoadForbidWords(dwContextID, dwTokenID, pData,wDataSize);
	}
	}

	return false;
}

//�û���¼
bool CDataBaseEngineSink::OnRequestLogonService(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��ȡ����
	auto pLogonService=(DBR_GR_LogonService *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128]=TEXT("");		

		//�������
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"),pLogonService->dwUserID);
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//ִ�нű�
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyUserID"),true);

		//���سɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_LogonSuccess LogonSuccess = {};

			//���ñ���
			LogonSuccess.dwUserID = pLogonService->dwUserID;

			//������Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_SUCCESS, dwContextID, dwTokenID, &LogonSuccess, sizeof(LogonSuccess));

			//��������
			WORD wPacketSize = 0;
			tagClubNameCard* pClubNameCard = NULL;

			//��ȡ����
			while (m_GameBattleDBModule->IsRecordsetEnd() == false)
			{
				//��ȡ����
				pClubNameCard = (tagClubNameCard*)(m_cbBuffer + wPacketSize);

				//��ȡ����
				pClubNameCard->dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
				pClubNameCard->wClubKind = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubKind"));
				pClubNameCard->wClubIconID = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubIconID"));
				pClubNameCard->dwCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterID"));
				pClubNameCard->dwCreaterGameID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterGameID"));
				pClubNameCard->wMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));
				pClubNameCard->wMaxMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
				pClubNameCard->wCreaterFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("CreaterFaceID"));
				pClubNameCard->dwCreaterCustomID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterCustomID"));
				m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), pClubNameCard->szClubName, CountArray(pClubNameCard->szClubName));				
				m_GameBattleDBAide.GetValue_String(TEXT("CreaterNickName"), pClubNameCard->szCreaterNickName, CountArray(pClubNameCard->szCreaterNickName));


				wPacketSize += sizeof(tagClubNameCard);

				//�ƶ���¼
				m_GameBattleDBModule->MoveToNext();
			}

			//������Ϣ
			if (wPacketSize > 0)
			{
				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_NAMECARD, dwContextID, dwTokenID, m_cbBuffer, wPacketSize);
			}
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_LogonFailure LogonFailure;
			ZeroMemory(&LogonFailure,sizeof(LogonFailure));

			//���ñ���
			LogonFailure.lErrorCode=lReturnValue;
			StringCchCopy(LogonFailure.szErrorDescribe,CountArray(LogonFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize=CountStringBuffer(LogonFailure.szErrorDescribe);
			wPacketSize+=sizeof(LogonFailure)-sizeof(LogonFailure.szErrorDescribe);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE,dwContextID,dwTokenID,&LogonFailure,wPacketSize);

			return true;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////
		//��ȡ������Ϣ

		//�������
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"),pLogonService->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), 0);
		m_GameBattleDBAide.AddParameter(TEXT("@cbMsgType"),APPLY_MSG_TYPE_REQUEST);

		//ִ�нű�
		lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadApplyMessage"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS && m_GameBattleDBModule->GetRecordCount()>0)
		{
			//��ȡ����
			WORD wPacketSize=sizeof(DBO_GR_ApplyMessage);
			auto pDBApplyMessage=(DBO_GR_ApplyMessage *)m_cbBuffer;

			//���ñ���
			pDBApplyMessage->wMessageCount=0;
			pDBApplyMessage->dwUserID=pLogonService->dwUserID;
			pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_REQUEST;

			//��������
			tagClubApplyMessage * pApplyMessage=NULL;

			//��ȡ����
			while (m_GameBattleDBModule->IsRecordsetEnd()==false)
			{
				if ((wPacketSize+sizeof(tagClubApplyMessage))>sizeof(m_cbBuffer))
				{
					//��������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,dwTokenID,m_cbBuffer,wPacketSize);

					//���ñ���
					pDBApplyMessage->wMessageCount=0;
					wPacketSize=sizeof(DBO_GR_ApplyMessage);
				}

				//��ȡ����
				pApplyMessage=(tagClubApplyMessage *)(m_cbBuffer+wPacketSize);

				//��ȡ����
				pApplyMessage->dwMessageID=m_GameBattleDBAide.GetValue_DWORD(TEXT("RecordID"));
				pApplyMessage->cbMessageStatus=m_GameBattleDBAide.GetValue_BYTE(TEXT("ApplyStatus"));

				//������Ϣ
				pApplyMessage->dwApplyerID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerID"));
				pApplyMessage->dwApplyerGameID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
				pApplyMessage->wApplyerFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("ApplyerFaceID"));
				pApplyMessage->cbApplyerGender = m_GameBattleDBAide.GetValue_BYTE(TEXT("ApplyerGender"));
				pApplyMessage->dwApplyerCustomID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("ApplyDateTime"), pApplyMessage->ApplyDateTime);
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("OperateDateTime"), pApplyMessage->OperateDateTime);
				m_GameBattleDBAide.GetValue_String(TEXT("ApplyerNickName"),pApplyMessage->szApplyerNickName,CountArray(pApplyMessage->szApplyerNickName));

				//Ⱥ����Ϣ
				pApplyMessage->dwClubID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
				pApplyMessage->dwResponderID =m_GameBattleDBAide.GetValue_DWORD(TEXT("ResponderID"));
				m_GameBattleDBAide.GetValue_String(TEXT("ClubName"),pApplyMessage->szClubName,CountArray(pApplyMessage->szClubName));

				//���ñ���
				pDBApplyMessage->wMessageCount++;
				wPacketSize+=sizeof(tagClubApplyMessage);

				//�ƶ���¼
				m_GameBattleDBModule->MoveToNext();
			}

			//��������
			if (wPacketSize>sizeof(DBO_GR_ApplyMessage))
			{					
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,dwTokenID,m_cbBuffer,wPacketSize);
			}
		}	

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FINISH, dwContextID, dwTokenID, NULL, 0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_LogonFailure LogonFailure;
		ZeroMemory(&LogonFailure,sizeof(LogonFailure));

		//���ñ���
		LogonFailure.lErrorCode=1;
		StringCchCopy(LogonFailure.szErrorDescribe,CountArray(LogonFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize=CountStringBuffer(LogonFailure.szErrorDescribe);
		wPacketSize+=sizeof(LogonFailure)-sizeof(LogonFailure.szErrorDescribe);			
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE,dwContextID,dwTokenID,&LogonFailure,wPacketSize);		
	}

	return true;
}

//����Ⱥ��
bool CDataBaseEngineSink::OnRequestCreateClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize==sizeof(DBR_GR_CreateClub));
	if (wDataSize!=sizeof(DBR_GR_CreateClub)) return false;

	//��ȡ����
	auto pCreateClub=(DBR_GR_CreateClub *)pData;

	try
	{
		//��������						
		bool bCreateSuccess=true;
		WCHAR szErrorDescribe[128]=TEXT("");			

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwCreateID"),pCreateClub->dwCreateID);

		//Ⱥ������
		m_GameBattleDBAide.AddParameter(TEXT("@wClubKind"), pCreateClub->wClubKind);
		m_GameBattleDBAide.AddParameter(TEXT("@wClubIconID"), pCreateClub->wClubIconID);
		m_GameBattleDBAide.AddParameter(TEXT("@strClubName"),pCreateClub->szClubName);
		m_GameBattleDBAide.AddParameter(TEXT("@strClubIntroduce"),pCreateClub->szClubIntroduce);		

		//Ⱥ�����
		m_GameBattleDBAide.AddParameter(TEXT("@cbPayType"), pCreateClub->cbPayType);
		m_GameBattleDBAide.AddParameter(TEXT("@cbCurrencyKind"), pCreateClub->cbCurrencyKind);
		m_GameBattleDBAide.AddParameter(TEXT("@dwSwitchOption"), pCreateClub->dwSwitchOption);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);		

		//ִ�нű�
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_CreateClub"),true);		

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS && m_GameBattleDBModule->IsRecordsetEnd()==false)
		{
			//����ṹ
			DBO_GR_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate,sizeof(WealthUpdate));					

			//��������
			DBO_GR_CreateClub CreateClub;
			ZeroMemory(&CreateClub,sizeof(CreateClub));							

			//��ȡ�Ƹ�
			WealthUpdate.wItemCount = 1;
			WealthUpdate.WealthItem[0].wMoneyID = GOODS_ID_CARD;
			WealthUpdate.WealthItem[0].lBalance = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("UserCard"));			

			//��ȡ����
			auto pClubNameCard = &CreateClub.ClubNameCard;

			//��ȡ����
			pClubNameCard->dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
			pClubNameCard->wClubKind = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubKind"));
			pClubNameCard->wClubIconID = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubIconID"));
			pClubNameCard->dwCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterID"));
			pClubNameCard->dwCreaterGameID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterGameID"));
			pClubNameCard->wMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));
			pClubNameCard->wMaxMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
			pClubNameCard->wCreaterFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("CreaterFaceID"));
			pClubNameCard->dwCreaterCustomID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterCustomID"));
			m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), pClubNameCard->szClubName, CountArray(pClubNameCard->szClubName));
			m_GameBattleDBAide.GetValue_String(TEXT("CreaterNickName"), pClubNameCard->szCreaterNickName, CountArray(pClubNameCard->szCreaterNickName));
				
			////////////////////////////////////////////////////��ӳ�Ա///////////////////////////////////////////////////

			//�������
			m_GameBattleDBAide.ResetParameter();

			//�û���Ϣ
			m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pClubNameCard->dwClubID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pClubNameCard->dwCreaterID);
			m_GameBattleDBAide.AddParameter(TEXT("@cbMemberType"), MEMBER_TYPE_EMPLOYER);
			m_GameBattleDBAide.AddParameter(TEXT("@dwOperaterID"), 0);

			//�������
			m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

			//ִ�нű�
			lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_AddMember"),true);
			if (lReturnValue!=DB_SUCCESS)
			{
				//�ɹ���ʶ
				bCreateSuccess = false;

				//�������
				m_GameBattleDBAide.ResetParameter();

				//�û���Ϣ
				m_GameBattleDBAide.AddParameter(TEXT("@dwCreaterID"), pClubNameCard->dwCreaterID);
				m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pClubNameCard->dwClubID);

				//�������
				m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

				//ִ�нű�
				m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_DeleteClub"),false);

				//���ñ���			
				StringCchCopy(szErrorDescribe,CountArray(szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0012, TEXT("����ϵͳԭ�򣬲��紴��ʧ�ܣ�")));
			}
			else
			{
				//��Ա����
				pClubNameCard->wMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));
			}

			//��������
			if (bCreateSuccess==true)
			{
				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CREATE_CLUB,dwContextID,dwTokenID,&CreateClub,sizeof(CreateClub));

				//���²Ƹ�			
				WORD wHeadSize = sizeof(WealthUpdate) - sizeof(WealthUpdate.WealthItem);
				WORD wPacketSize = WealthUpdate.wItemCount * sizeof(WealthUpdate.WealthItem[0]) + wHeadSize;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE, dwContextID, dwTokenID, &WealthUpdate, wPacketSize);
			}
		}
		else
		{
			//�ɹ���ʶ
			bCreateSuccess = false;

			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//���ñ���			
			StringCchCopy(szErrorDescribe,CountArray(szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));
		}

		//�����ɹ�
		if (bCreateSuccess==true)
		{
			//����ṹ
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//���ñ���
			OperateSuccess.lOperateCode=0;
			StringCchCopy(OperateSuccess.szOperateNotice,CountArray(OperateSuccess.szOperateNotice), CWHService::FormatMessage(MIM_CLUBPLAZA_0013, TEXT("��ϲ�������紴���ɹ���")));

			//��������
			WORD wPacketSize=CountStringBuffer(OperateSuccess.szOperateNotice);
			wPacketSize+=sizeof(OperateSuccess)-sizeof(OperateSuccess.szOperateNotice);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS,dwContextID,dwTokenID,&OperateSuccess,wPacketSize);			
		}
		else
		{
			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode=lReturnValue;						
			StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), szErrorDescribe);

			//��������
			WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;
}

//����Ⱥ��
bool CDataBaseEngineSink::OnRequestUpdateClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize>=sizeof(DBR_GR_UpdateClub));
	if (wDataSize<sizeof(DBR_GR_UpdateClub)) return false;

	//��ȡ����
	auto pUpdateClub=(DBR_GR_UpdateClub *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128]=TEXT("");

		//��������
		LPBYTE pcbBuffer=(LPBYTE)pData;
		WORD wOffsetPos=sizeof(DBR_GR_UpdateClub);

		//��������		
		WORD wHeadSize=sizeof(DBO_GR_UpdateClub);
		auto pUpdateClubResult=(DBO_GR_UpdateClub*)m_cbBuffer;

		//Ⱥ���ʶ
		pUpdateClubResult->dwClubID=pUpdateClub->dwClubID;		

		//���¹���
		if ((pUpdateClub->cbUpdateKind&UPCLUB_KIND_RULES) != 0)
		{
			//��ȡ����
			auto pClubRules = (tagClubUpdate_Rules *)(pcbBuffer + wOffsetPos);

			//����ƫ��
			wOffsetPos += sizeof(tagClubUpdate_Rules);

			//�������
			m_GameBattleDBAide.ResetParameter();

			//�û���Ϣ
			m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pUpdateClub->dwUserID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pUpdateClub->dwClubID);

			//������Ϣ
			m_GameBattleDBAide.AddParameter(TEXT("@cbBattleMask"), pClubRules->cbBattleMask);			
			m_GameBattleDBAide.AddParameter(TEXT("@cbSwitchOption"), pClubRules->wSwitchOption);			

			//�������
			m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

			//ִ�нű�
			LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_UpdateClubRules"), true);

			//ִ�гɹ�
			if (lReturnValue == DB_SUCCESS)
			{
				//��������
				pUpdateClubResult->cbUpdateKind = UPCLUB_KIND_RULES;

				//��������
				auto pClubRules = (tagClubRules *)&m_cbBuffer[wHeadSize];

				//Ⱥ�����
				pClubRules->cbPayType = m_GameBattleDBAide.GetValue_BYTE(TEXT("PayType"));
				pClubRules->cbBattleMask = m_GameBattleDBAide.GetValue_BYTE(TEXT("BattleMask"));
				pClubRules->cbScoreKind = m_GameBattleDBAide.GetValue_BYTE(TEXT("ScoreKind"));				
				pClubRules->wSwitchOption = m_GameBattleDBAide.GetValue_WORD(TEXT("SwitchOption"));
				pClubRules->cbCurrencyKind = m_GameBattleDBAide.GetValue_BYTE(TEXT("CurrencyKind"));							

				//��������
				WORD wPacketSize = wHeadSize + sizeof(tagClubRules);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_CLUB, dwContextID, dwTokenID, m_cbBuffer, wPacketSize);
			}
			else
			{
				//��ȡ����
				CDBVarValue DBVarValue;
				m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

				//����ṹ
				DBO_GR_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));

				//���ñ���
				OperateFailure.lErrorCode = lReturnValue;
				StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

				//��������
				WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
				wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);

				return true;
			}
		}

		//���¹���
		if ((pUpdateClub->cbUpdateKind & UPCLUB_KIND_NOTICE) != 0)
		{
			//��ȡ����
			auto pClubNotice = (tagClubUpdate_Notice*)(pcbBuffer + wOffsetPos);

			//����ƫ��
			wOffsetPos += sizeof(tagClubUpdate_Notice);

			//�������
			m_GameBattleDBAide.ResetParameter();

			//�û���Ϣ
			m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pUpdateClub->dwUserID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pUpdateClub->dwClubID);

			//������Ϣ
			m_GameBattleDBAide.AddParameter(TEXT("@cbPublishMode"), pClubNotice->cbPublishMode);
			m_GameBattleDBAide.AddParameter(TEXT("@cbPublishState"), pClubNotice->cbPublishState);
			m_GameBattleDBAide.AddParameter(TEXT("@strNoticeContent"), pClubNotice->szNoticeContent);

			//�������
			m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

			//ִ�нű�
			LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_UpdateClubNotice"), true);

			//ִ�гɹ�
			if (lReturnValue == DB_SUCCESS)
			{
				//��������
				pUpdateClubResult->cbUpdateKind = UPCLUB_KIND_NOTICE;

				//��������
				auto pClubRules = (tagClubNotice*)&m_cbBuffer[wHeadSize];

				//������Ϣ
				pClubRules->cbPublishMode = m_GameBattleDBAide.GetValue_BYTE(TEXT("PublishMode"));
				pClubRules->cbPublishState = m_GameBattleDBAide.GetValue_BYTE(TEXT("PublishState"));
				m_GameBattleDBAide.GetValue_String(TEXT("NoticeContent"), pClubRules->szNoticeContent, CountArray(pClubRules->szNoticeContent));

				//��������
				WORD wPacketSize = wHeadSize + sizeof(tagClubNotice);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_CLUB, dwContextID, dwTokenID, m_cbBuffer, wPacketSize);
			}
			else
			{
				//��ȡ����
				CDBVarValue DBVarValue;
				m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

				//����ṹ
				DBO_GR_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure, sizeof(OperateFailure));

				//���ñ���
				OperateFailure.lErrorCode = lReturnValue;
				StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

				//��������
				WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
				wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);

				return true;
			}
		}

		//���²Ƹ�
		//if ((pUpdateClub->cbUpdateKind&UPCLUB_KIND_WEALTH)!=0)
		//{
		//	//��ȡ����
		//	tagClubWealth * pClubWealth=(tagClubWealth *)(pcbBuffer+wOffsetPos);

		//	//����ƫ��
		//	wOffsetPos+=sizeof(tagClubWealth);

		//	//�������
		//	m_GameBattleDBAide.ResetParameter();

		//	//�û���Ϣ
		//	m_GameBattleDBAide.AddParameter(TEXT("@dwCreateID"),pUpdateClub->dwCreaterID);
		//	m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pUpdateClub->dwClubID);

		//	//�Ƹ���Ϣ
		//	m_GameBattleDBAide.AddParameter(TEXT("@lRoomCard"),pClubWealth->lRoomCard);

		//	//�������
		//	m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//	//ִ�нű�
		//	LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_UpdateClubWealth"),true);

		//	//ִ�гɹ�
		//	if (lReturnValue==DB_SUCCESS)
		//	{
		//		//��������
		//		pUpdateClubResult->cbUpdateKind=UPCLUB_KIND_WEALTH;

		//		//��������
		//		tagClubWealth * pClubWealth=(tagClubWealth *)&m_cbBuffer[wHeadSize];								

		//		//Ⱥ��Ƹ�
		//		pClubWealth->lRoomCard=m_GameBattleDBAide.GetValue_LONGLONG(TEXT("RoomCard"));					

		//		//��������
		//		WORD wPacketSize=wHeadSize+sizeof(tagClubWealth);
		//		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_CLUB,dwContextID,dwTokenID,m_cbBuffer,wPacketSize);

		//		//����ṹ
		//		DBO_GR_WealthUpdate WealthUpdate;
		//		ZeroMemory(&WealthUpdate,sizeof(WealthUpdate));

		//		//�û��Ƹ�
		//		WealthUpdate.dwUserID=pUpdateClub->dwCreaterID;
		//		WealthUpdate.cbWealthMask = WEALTH_MASK_CARD;
		//		WealthUpdate.lUserCard=m_GameBattleDBAide.GetValue_LONGLONG(TEXT("UserCard"));					

		//		//��������
		//		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE,dwContextID,dwTokenID,&WealthUpdate,sizeof(WealthUpdate));
		//	}
		//	else
		//	{
		//		//��ȡ����
		//		CDBVarValue DBVarValue;
		//		m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//		//����ṹ
		//		DBO_GR_OperateFailure OperateFailure;
		//		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//		//���ñ���
		//		OperateFailure.lErrorCode=lReturnValue;
		//		OperateFailure.dwUserID=pUpdateClub->dwCreaterID;			
		//		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

		//		//��������
		//		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		//		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
		//		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);

		//		return true;
		//	}
		//}

		//��������
		if ((pUpdateClub->cbUpdateKind&UPCLUB_KIND_PROPERTY)!=0)
		{
			//��ȡ����
			auto pClubProperty=(tagClubUpdate_Property *)(pcbBuffer+wOffsetPos);

			//����ƫ��
			wOffsetPos+=sizeof(tagClubUpdate_Property);

			//�������
			m_GameBattleDBAide.ResetParameter();

			//�û���Ϣ
			m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"),pUpdateClub->dwUserID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pUpdateClub->dwClubID);

			//������Ϣ
			m_GameBattleDBAide.AddParameter(TEXT("@wClubIconID"), pClubProperty->wClubIconID);
			m_GameBattleDBAide.AddParameter(TEXT("@strClubName"), pClubProperty->szClubName);
			m_GameBattleDBAide.AddParameter(TEXT("@strClubIntroduce"),pClubProperty->szClubIntroduce);			

			//�������
			m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

			//ִ�нű�
			LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_UpdateClubProperty"),true);

			//ִ�гɹ�
			if (lReturnValue==DB_SUCCESS)
			{
				//��������
				pUpdateClubResult->cbUpdateKind=UPCLUB_KIND_PROPERTY;

				//��������
				auto pClubProperty=(tagClubProperty *)&m_cbBuffer[wHeadSize];

				//Ⱥ������
				pClubProperty->dwClubID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));				
				pClubProperty->dwCreaterID=m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterID"));				
				pClubProperty->dwCreaterGameID=m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterGameID"));				
				pClubProperty->wClubKind=m_GameBattleDBAide.GetValue_WORD(TEXT("ClubKind"));
				pClubProperty->wClubIconID=m_GameBattleDBAide.GetValue_WORD(TEXT("ClubIconID"));
				pClubProperty->dwPayrollerID=m_GameBattleDBAide.GetValue_DWORD(TEXT("PayrollerID"));
				pClubProperty->wMemberCount=m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));
				pClubProperty->wMaxMemberCount=m_GameBattleDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("CreateDateTime"),pClubProperty->CreateDateTime);
				m_GameBattleDBAide.GetValue_String(TEXT("ClubName"),pClubProperty->szClubName,CountArray(pClubProperty->szClubName));
				m_GameBattleDBAide.GetValue_String(TEXT("ClubIntroduce"),pClubProperty->szClubIntroduce,CountArray(pClubProperty->szClubIntroduce));		
				
				//��������
				WORD wPacketSize=wHeadSize+sizeof(tagClubProperty);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_CLUB,dwContextID,dwTokenID,m_cbBuffer,wPacketSize);
			}
			else
			{
				//��ȡ����
				CDBVarValue DBVarValue;
				m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

				//����ṹ
				DBO_GR_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure,sizeof(OperateFailure));

				//���ñ���
				OperateFailure.lErrorCode=lReturnValue;				
				StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

				//��������
				WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
				wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);

				return true;
			}
		}		

		//����ṹ
		DBO_GR_OperateSuccess OperateSuccess;
		ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

		//���ñ���
		OperateSuccess.lOperateCode=0;	

		//��������
		WORD wPacketSize=CountStringBuffer(OperateSuccess.szOperateNotice);
		wPacketSize+=sizeof(OperateSuccess)-sizeof(OperateSuccess.szOperateNotice);			
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS,dwContextID,dwTokenID,&OperateSuccess,wPacketSize);	

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;

	return true;
}

//ɾ��Ⱥ��
bool CDataBaseEngineSink::OnRequestDeleteClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize==sizeof(DBR_GR_DeleteClub));
	if (wDataSize!=sizeof(DBR_GR_DeleteClub)) return false;

	//��ȡ����
	auto pDeleteClub=(DBR_GR_DeleteClub *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128]=TEXT("");			

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwCreaterID"),pDeleteClub->dwCreaterID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pDeleteClub->dwClubID);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//ִ�нű�
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_DeleteClub"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS)
		{           
			//����ṹ
			DBO_GR_DeleteClub DeleteClub;
			ZeroMemory(&DeleteClub,sizeof(DeleteClub));

			//���ñ���
			DeleteClub.dwUserID=pDeleteClub->dwCreaterID;
			DeleteClub.dwClubID=pDeleteClub->dwClubID;
			m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), DeleteClub.szClubName,CountArray(DeleteClub.szClubName));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DELETE_CLUB,dwContextID,dwTokenID,&DeleteClub,sizeof(DeleteClub));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode=lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;
}

//����Ⱥ��
bool CDataBaseEngineSink::OnRequestSearchClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize == sizeof(DBR_GR_SearchClub));
	if (wDataSize != sizeof(DBR_GR_SearchClub)) return false;

	//��ȡ����
	auto pSearchClub = (DBR_GR_SearchClub *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pSearchClub->dwClubID);
		
		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_SearchClub"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_SearchClub SearchClub;
			ZeroMemory(&SearchClub, sizeof(SearchClub));

			//���ñ���
			SearchClub.dwUserID = pSearchClub->dwUserID;
			
			//��ȡ����
			auto pClubNameCard = &SearchClub.ClubNameCard;

			//��ȡ����
			pClubNameCard->dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
			pClubNameCard->wClubKind = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubKind"));
			pClubNameCard->wClubIconID = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubIconID"));
			pClubNameCard->dwCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterID"));
			pClubNameCard->dwCreaterGameID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterGameID"));
			pClubNameCard->wMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));
			pClubNameCard->wMaxMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
			pClubNameCard->wCreaterFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("CreaterFaceID"));
			pClubNameCard->dwCreaterCustomID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterCustomID"));
			m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), pClubNameCard->szClubName, CountArray(pClubNameCard->szClubName));
			m_GameBattleDBAide.GetValue_String(TEXT("CreaterNickName"), pClubNameCard->szCreaterNickName, CountArray(pClubNameCard->szCreaterNickName));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SEARCH_CLUB, dwContextID, dwTokenID, &SearchClub, sizeof(SearchClub));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//ת��Ⱥ��
bool CDataBaseEngineSink::OnRequestTransferClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize == sizeof(DBR_GR_TransferClub));
	if (wDataSize != sizeof(DBR_GR_TransferClub)) return false;

	//��ȡ����
	auto pTransferClub = (DBR_GR_TransferClub *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pTransferClub->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwCreaterID"), pTransferClub->dwCreaterID);		
		m_GameBattleDBAide.AddParameter(TEXT("@dwNewCreaterID"), pTransferClub->dwNewCreaterID);		
		m_GameBattleDBAide.AddParameter(TEXT("@szAuthCodeMD5"), pTransferClub->szAuthCodeMD5);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_TransferClub"), false);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_TransferClub TransferClub;
			ZeroMemory(&TransferClub, sizeof(TransferClub));

			//���ñ���
			TransferClub.dwClubID = pTransferClub->dwClubID;
			TransferClub.dwUserID = pTransferClub->dwCreaterID;			
			TransferClub.dwNewCreaterID = pTransferClub->dwNewCreaterID;

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TRANSFER_CLUB, dwContextID, dwTokenID, &TransferClub, sizeof(TransferClub));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;			
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;		
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//���³�Ա
bool CDataBaseEngineSink::OnRequestUpdateMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize >= sizeof(DBR_GR_UpdateMember));
	if (wDataSize < sizeof(DBR_GR_UpdateMember)) return false;

	//��ȡ����
	auto pUpdateMember = (DBR_GR_UpdateMember *)pData;

	try
	{
		//��������
		LONG lReturnValue = DB_SUCCESS;
		WCHAR szErrorDescribe[128] = TEXT("");

		//���³�Ա����
		if (pUpdateMember->wUpdateKind == UPMEMBER_KIND_TYPE)
		{
			//��Ա����
			BYTE cbMemberType = *(BYTE *)(pUpdateMember + 1);
			DWORD dwManageRight = *(DWORD *)((BYTE*)(pUpdateMember + 1)+1);

			//�������
			m_GameBattleDBAide.ResetParameter();

			//�û���Ϣ
			m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pUpdateMember->dwUserID);			
			m_GameBattleDBAide.AddParameter(TEXT("dwClubID"), pUpdateMember->dwClubID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pUpdateMember->dwMemberID);
			m_GameBattleDBAide.AddParameter(TEXT("@cbMemberType"), cbMemberType);
			m_GameBattleDBAide.AddParameter(TEXT("@dwManageRight"), dwManageRight);

			//�������
			m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

			//ִ�нű�
			lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_UpdateMemberType"), true);
		}		

		//���³�Ա״̬
		if (pUpdateMember->wUpdateKind == UPMEMBER_KIND_STATUS)
		{
			//��Ա״̬
			BYTE cbMemberStatus= *(BYTE *)(pUpdateMember + 1);

			//�������
			m_GameBattleDBAide.ResetParameter();

			//�û���Ϣ
			m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pUpdateMember->dwUserID);
			
			//Ⱥ����Ϣ
			m_GameBattleDBAide.AddParameter(TEXT("dwClubID"), pUpdateMember->dwClubID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pUpdateMember->dwMemberID);
			m_GameBattleDBAide.AddParameter(TEXT("@cbMemberStatus"), cbMemberStatus);

			//�������
			m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

			//ִ�нű�
			lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_UpdateMemberStatus"), true);
		}

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��������
			DBO_GR_UpdateMember UpdateMember;
			ZeroMemory(&UpdateMember, sizeof(UpdateMember));

			//��������
			UpdateMember.dwClubID = pUpdateMember->dwClubID;
			UpdateMember.wUpdateKind = pUpdateMember->wUpdateKind;

			//��������
			tagClubMemberInfo * pClubMember = &UpdateMember.ClubMember;

			//��Ա��Ϣ
			pClubMember->dwMemberID = m_GameBattleDBAide.GetValue_DWORD(TEXT("MemberID"));
			pClubMember->cbMemberType = m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberType"));
			pClubMember->cbMemberStatus = m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberStatus"));
			pClubMember->dwManageRight = m_GameBattleDBAide.GetValue_DWORD(TEXT("ManageRight"));
			pClubMember->lClubScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("ClubScore"));
			pClubMember->dwCreateCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreateCount"));
			pClubMember->dwBattleCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("BattleCount"));
			pClubMember->lRevenueCount = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("RevenueCount"));
			m_GameBattleDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pClubMember->JoinDateTime);
			m_GameBattleDBAide.GetValue_SystemTime(TEXT("LastBattleTime"), pClubMember->LastBattleTime);			

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_MEMBER, dwContextID, dwTokenID, &UpdateMember, sizeof(UpdateMember));

			//����ṹ
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//���ñ���
			OperateSuccess.lOperateCode = 0;
			StringCchCopy(OperateSuccess.szOperateNotice, CountArray(OperateSuccess.szOperateNotice), CWHService::FormatMessage(MIM_CLUBPLAZA_0014, TEXT("�����ɹ���")));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateSuccess.szOperateNotice);
			wPacketSize += sizeof(OperateSuccess) - sizeof(OperateSuccess.szOperateNotice);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wPacketSize);
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			if (DBVarValue.bstrVal)
			{
				StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));
			}

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//ɾ����Ա
bool CDataBaseEngineSink::OnRequestDeleteMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize==sizeof(DBR_GR_DeleteMember));
	if (wDataSize!=sizeof(DBR_GR_DeleteMember)) return false;

	//��ȡ����
	auto pDeleteMember=(DBR_GR_DeleteMember *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128]=TEXT("");			

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"),pDeleteMember->dwUserID);		
		m_GameBattleDBAide.AddParameter(TEXT("@cbDeleteKind"), pDeleteMember->cbDeleteKind);

		//Ⱥ����Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pDeleteMember->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"),pDeleteMember->dwMemberID);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//ִ�нű�
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_DeleteMember"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//��������
			DBO_GR_DeleteMember DeleteMember;
			ZeroMemory(&DeleteMember,sizeof(DeleteMember));

			//Ⱥ����Ϣ
			DeleteMember.dwUserID=pDeleteMember->dwUserID;
			DeleteMember.dwClubID=pDeleteMember->dwClubID;
			DeleteMember.dwMemberID=pDeleteMember->dwMemberID;
			DeleteMember.cbDeleteKind=pDeleteMember->cbDeleteKind;
			DeleteMember.wMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));

			//Ⱥ������
			m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), DeleteMember.szClubName, CountArray(DeleteMember.szClubName));
			
			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DELETE_MEMBER,dwContextID,dwTokenID,&DeleteMember,sizeof(DeleteMember));

			//����ṹ
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//���ñ���
			OperateSuccess.lOperateCode = 0;
			StringCchCopy(OperateSuccess.szOperateNotice, CountArray(OperateSuccess.szOperateNotice), CWHService::FormatMessage(MIM_CLUBPLAZA_0014, TEXT("�����ɹ���")));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateSuccess.szOperateNotice);
			wPacketSize += sizeof(OperateSuccess) - sizeof(OperateSuccess.szOperateNotice);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wPacketSize);			
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode=lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestApplyRequest(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize==sizeof(DBR_GR_ApplyRequest));
	if (wDataSize!=sizeof(DBR_GR_ApplyRequest)) return false;

	//��ȡ����
	auto pApplyRequest=(DBR_GR_ApplyRequest *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128]=TEXT("");			

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pApplyRequest->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwApplyerID"),pApplyRequest->dwApplyerID);							
		
		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//ִ�нű�
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_ApplyRequest"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//��ȡ����
			tagClubApplyMessage ApplyMessage;
			ZeroMemory(&ApplyMessage,sizeof(ApplyMessage));			

			//��ȡ����
			ApplyMessage.dwMessageID=m_GameBattleDBAide.GetValue_DWORD(TEXT("RecordID"));
			ApplyMessage.cbMessageStatus=m_GameBattleDBAide.GetValue_BYTE(TEXT("ApplyStatus"));

			//������Ϣ
			ApplyMessage.dwApplyerID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerID"));
			ApplyMessage.dwApplyerGameID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
			ApplyMessage.wApplyerFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("ApplyerFaceID"));
			ApplyMessage.cbApplyerGender = m_GameBattleDBAide.GetValue_BYTE(TEXT("ApplyerGender"));
			ApplyMessage.dwApplyerCustomID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
			m_GameBattleDBAide.GetValue_SystemTime(TEXT("ApplyDateTime"),ApplyMessage.ApplyDateTime);
			m_GameBattleDBAide.GetValue_SystemTime(TEXT("OperateDateTime"), ApplyMessage.OperateDateTime);
			m_GameBattleDBAide.GetValue_String(TEXT("ApplyerNickName"),ApplyMessage.szApplyerNickName,CountArray(ApplyMessage.szApplyerNickName));

			//Ⱥ����Ϣ
			ApplyMessage.dwClubID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
			ApplyMessage.dwResponderID =m_GameBattleDBAide.GetValue_DWORD(TEXT("ResponderID"));
			m_GameBattleDBAide.GetValue_String(TEXT("ClubName"),ApplyMessage.szClubName,CountArray(ApplyMessage.szClubName));

			//��ȡ����			
			auto pDBApplyMessage=(DBO_GR_ApplyMessage *)m_cbBuffer;

			//���ñ���
			pDBApplyMessage->wMessageCount=1;
			CopyMemory(pDBApplyMessage+1,&ApplyMessage,sizeof(ApplyMessage));

			///////////////////////////////////////////////////////////////////////////////////////////////////////			

			//���ñ���			
			pDBApplyMessage->dwUserID=ApplyMessage.dwApplyerID;
			pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_REQUEST;		
			
			//��������
			WORD wPacketSize=sizeof(DBO_GR_ApplyMessage)+sizeof(ApplyMessage);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,dwTokenID,pDBApplyMessage,wPacketSize);

			///////////////////////////////////////////////////////////////////////////////////////////////////////

			//���ñ���
			pDBApplyMessage->dwClubID=ApplyMessage.dwClubID;
			pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_RESPOND;		
			
			//��������
			wPacketSize=sizeof(DBO_GR_ApplyMessage)+sizeof(ApplyMessage);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,dwTokenID,pDBApplyMessage,wPacketSize);

			///////////////////////////////////////////////////////////////////////////////////////////////////////

			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//ͬ��״̬
			if (ApplyMessage.cbMessageStatus == APPLY_STATUS_AGREE)
			{
				//Ⱥ����Ƭ
				tagClubNameCard ClubNameCard = {};

				//��ȡ����
				ClubNameCard.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
				ClubNameCard.wClubKind = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubKind"));
				ClubNameCard.wClubIconID = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubIconID"));
				ClubNameCard.dwCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterID"));
				ClubNameCard.dwCreaterGameID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterGameID"));
				ClubNameCard.wMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));
				ClubNameCard.wMaxMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
				ClubNameCard.wCreaterFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("CreaterFaceID"));
				ClubNameCard.dwCreaterCustomID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterCustomID"));
				m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), ClubNameCard.szClubName, CountArray(ClubNameCard.szClubName));
				m_GameBattleDBAide.GetValue_String(TEXT("CreaterNickName"), ClubNameCard.szCreaterNickName, CountArray(ClubNameCard.szCreaterNickName));

				//�������
				m_GameBattleDBAide.ResetParameter();

				//�û���Ϣ
				m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pApplyRequest->dwClubID);
				m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pApplyRequest->dwApplyerID);
				m_GameBattleDBAide.AddParameter(TEXT("@cbMemberType"), MEMBER_TYPE_MEMBER);
				m_GameBattleDBAide.AddParameter(TEXT("@dwOperaterID"), 0);

				//�������
				m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

				//ִ�нű�
				lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_AddMember"), true);

				if (lReturnValue == DB_SUCCESS)
				{
					//��������
					DBO_GR_InsertMember InsertMember;
					ZeroMemory(&InsertMember, sizeof(InsertMember));

					//���ñ���
					InsertMember.dwClubID = pApplyRequest->dwClubID;
					CopyMemory(&InsertMember.ClubNameCard,&ClubNameCard,sizeof(InsertMember.ClubNameCard));

					//��������
					tagClubUserInfo * pUserInfo = &InsertMember.ClubMemberUser.UserInfo;
					tagClubMemberInfo * pClubMember = &InsertMember.ClubMemberUser.MemberInfo;

					//��Ա����
					InsertMember.ClubNameCard.wMemberCount= m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));

					//��Ա��Ϣ
					pClubMember->dwMemberID = m_GameBattleDBAide.GetValue_DWORD(TEXT("UserID"));
					pClubMember->cbMemberType = m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberType"));
					pClubMember->cbMemberStatus = m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberStatus"));
					pClubMember->dwManageRight = m_GameBattleDBAide.GetValue_DWORD(TEXT("ManageRight"));
					pClubMember->lClubScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("ClubScore"));
					pClubMember->dwCreateCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreateCount"));
					pClubMember->dwBattleCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("BattleCount"));					
					pClubMember->lRevenueCount = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("RevenueCount"));
					m_GameBattleDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pClubMember->JoinDateTime);
					m_GameBattleDBAide.GetValue_SystemTime(TEXT("LastBattleTime"), pClubMember->LastBattleTime);

					//�û���Ϣ
					pUserInfo->dwUserID = m_GameBattleDBAide.GetValue_DWORD(TEXT("UserID"));
					pUserInfo->dwGameID = m_GameBattleDBAide.GetValue_DWORD(TEXT("GameID"));
					pUserInfo->cbGender = m_GameBattleDBAide.GetValue_BYTE(TEXT("Gender"));
					pUserInfo->wFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("FaceID"));
					pUserInfo->dwCustomID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CustomID"));
					m_GameBattleDBAide.GetValue_String(TEXT("NickName"), pUserInfo->szNickName, CountArray(pUserInfo->szNickName));

					//Ͷ������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_INSERT_MEMBER, dwContextID, dwTokenID, &InsertMember, sizeof(InsertMember));					
				}
				else
				{
					//����ṹ
					DBO_GR_OperateFailure OperateFailure;
					ZeroMemory(&OperateFailure, sizeof(OperateFailure));

					//���ñ���
					OperateFailure.lErrorCode = lReturnValue;
					StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

					//��������
					WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
					wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);

					return true;
				}
			}	

			//����ṹ
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//���ñ���
			OperateSuccess.lOperateCode = 0;
			StringCchCopy(OperateSuccess.szOperateNotice, CountArray(OperateSuccess.szOperateNotice),(LPCTSTR)(DBVarValue.bstrVal));

			//��������
			wPacketSize = CountStringBuffer(OperateSuccess.szOperateNotice);
			wPacketSize += sizeof(OperateSuccess) - sizeof(OperateSuccess.szOperateNotice);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wPacketSize);
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode=lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe),CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;
}

//������Ӧ
bool CDataBaseEngineSink::OnRequestApplyRespond(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize==sizeof(DBR_GR_ApplyRespond));
	if (wDataSize!=sizeof(DBR_GR_ApplyRespond)) return false;

	//��ȡ����
	auto pApplyRespond=(DBR_GR_ApplyRespond *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128]=TEXT("");			

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwRecordID"),pApplyRespond->dwMessageID);		
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"),pApplyRespond->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwApplyerID"),pApplyRespond->dwApplyerID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pApplyRespond->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbApplyStatus"),pApplyRespond->cbApplyStatus);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//ִ�нű�
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_ApplyRespond"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//��ȡ����
			tagClubApplyMessage ApplyMessage;
			ZeroMemory(&ApplyMessage,sizeof(ApplyMessage));

			//��������
			CDWordArray ApplyerArray;
			WCHAR szClubName[LEN_CLUB_NAME] = { 0 };

			//��ȡ����
			while (m_GameBattleDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ����				
				ApplyMessage.dwMessageID=m_GameBattleDBAide.GetValue_DWORD(TEXT("RecordID"));
				ApplyMessage.cbMessageStatus=pApplyRespond->cbApplyStatus;

				//������Ϣ
				ApplyMessage.dwApplyerID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerID"));
				ApplyMessage.dwApplyerGameID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
				ApplyMessage.wApplyerFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("ApplyerFaceID"));
				ApplyMessage.cbApplyerGender = m_GameBattleDBAide.GetValue_BYTE(TEXT("ApplyerGender"));
				ApplyMessage.dwApplyerCustomID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("ApplyDateTime"), ApplyMessage.ApplyDateTime);
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("OperateDateTime"), ApplyMessage.OperateDateTime);
				m_GameBattleDBAide.GetValue_String(TEXT("ApplyerNickName"),ApplyMessage.szApplyerNickName,CountArray(ApplyMessage.szApplyerNickName));

				//Ⱥ����Ϣ
				ApplyMessage.dwClubID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
				ApplyMessage.dwResponderID =m_GameBattleDBAide.GetValue_DWORD(TEXT("ResponderID"));
				m_GameBattleDBAide.GetValue_String(TEXT("ClubName"),ApplyMessage.szClubName,CountArray(ApplyMessage.szClubName));

				//�������
				if (szClubName[0] == 0)
				{
					StringCchCopy(szClubName, CountArray(szClubName), ApplyMessage.szClubName);
				}

				//��ȡ����			
				auto pDBApplyMessage=(DBO_GR_ApplyMessage *)m_cbBuffer;

				//��������
				pDBApplyMessage->wMessageCount=1;
				CopyMemory(pDBApplyMessage+1,&ApplyMessage,sizeof(ApplyMessage));

				///////////////////////////////////////////////////////////////////////////////////////////////////////			

				//���ñ���
				pDBApplyMessage->dwUserID=ApplyMessage.dwApplyerID;
				pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_REQUEST;		
				
				//��������
				WORD wPacketSize=sizeof(DBO_GR_ApplyMessage)+sizeof(ApplyMessage);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,dwTokenID,pDBApplyMessage,wPacketSize);

				///////////////////////////////////////////////////////////////////////////////////////////////////////

				//���ñ���
				pDBApplyMessage->dwClubID=ApplyMessage.dwClubID;
				pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_RESPOND;		
				
				//��������
				wPacketSize=sizeof(DBO_GR_ApplyMessage)+sizeof(ApplyMessage);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,dwTokenID,pDBApplyMessage,wPacketSize);

				//////////////////////////////////////////////////////////////////////////////////////////////////////

				//����û�
				ApplyerArray.Add(ApplyMessage.dwApplyerID);

				//�ƶ���¼
				m_GameBattleDBModule->MoveToNext();
			}			

			////��ȡ����
			//CDBVarValue DBVarValue;
			//m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//���ñ���
			OperateSuccess.lOperateCode=0;
			//StringCchCopy(OperateSuccess.szOperateNotice,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateSuccess.szOperateNotice));	

			//��������
			WORD wPacketSize=CountStringBuffer(OperateSuccess.szOperateNotice);
			wPacketSize+=sizeof(OperateSuccess)-sizeof(OperateSuccess.szOperateNotice);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS,dwContextID,dwTokenID,&OperateSuccess,wPacketSize);

			//��ӳ�Ա
			if (pApplyRespond->cbApplyStatus==APPLY_STATUS_AGREE)
			{
				//�������
				m_GameBattleDBAide.ResetParameter();				
				m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pApplyRespond->dwClubID);
				
				//ִ�нű�
				LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_QueryClubNameCard"), true);
				if (lReturnValue == DB_SUCCESS)
				{
					//Ⱥ����Ƭ
					tagClubNameCard ClubNameCard = {};

					//��ȡ����
					ClubNameCard.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
					ClubNameCard.wClubKind = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubKind"));
					ClubNameCard.wClubIconID = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubIconID"));
					ClubNameCard.dwCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterID"));
					ClubNameCard.dwCreaterGameID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterGameID"));
					ClubNameCard.wMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));
					ClubNameCard.wMaxMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
					ClubNameCard.wCreaterFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("CreaterFaceID"));
					ClubNameCard.dwCreaterCustomID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterCustomID"));
					m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), ClubNameCard.szClubName, CountArray(ClubNameCard.szClubName));
					m_GameBattleDBAide.GetValue_String(TEXT("CreaterNickName"), ClubNameCard.szCreaterNickName, CountArray(ClubNameCard.szCreaterNickName));

					for (INT_PTR i = 0; i < ApplyerArray.GetCount(); i++)
					{
						//�������
						m_GameBattleDBAide.ResetParameter();

						//�û���Ϣ
						m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pApplyRespond->dwClubID);
						m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), ApplyerArray[i]);
						m_GameBattleDBAide.AddParameter(TEXT("@cbMemberType"), MEMBER_TYPE_MEMBER);
						m_GameBattleDBAide.AddParameter(TEXT("@dwOperaterID"), pApplyRespond->dwUserID);

						//�������
						m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

						//ִ�нű�
						lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_AddMember"), true);

						if (lReturnValue == DB_SUCCESS)
						{
							//��������
							DBO_GR_InsertMember InsertMember;
							ZeroMemory(&InsertMember, sizeof(InsertMember));

							//���ñ���
							InsertMember.dwClubID = pApplyRespond->dwClubID;
							CopyMemory(&InsertMember.ClubNameCard, &ClubNameCard, sizeof(InsertMember.ClubNameCard));

							//��������
							tagClubUserInfo* pUserInfo = &InsertMember.ClubMemberUser.UserInfo;
							tagClubMemberInfo* pClubMember = &InsertMember.ClubMemberUser.MemberInfo;

							//��Ա����
							InsertMember.ClubNameCard.wMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));

							//��Ա��Ϣ
							pClubMember->dwMemberID = m_GameBattleDBAide.GetValue_DWORD(TEXT("MemberID"));
							pClubMember->cbMemberType = m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberType"));
							pClubMember->cbMemberStatus = m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberStatus"));
							pClubMember->dwManageRight = m_GameBattleDBAide.GetValue_DWORD(TEXT("ManageRight"));
							pClubMember->lClubScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("ClubScore"));
							pClubMember->dwCreateCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreateCount"));
							pClubMember->dwBattleCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("BattleCount"));
							pClubMember->lRevenueCount = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("RevenueCount"));
							m_GameBattleDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pClubMember->JoinDateTime);
							m_GameBattleDBAide.GetValue_SystemTime(TEXT("LastBattleTime"), pClubMember->LastBattleTime);

							//�û���Ϣ
							pUserInfo->dwUserID = m_GameBattleDBAide.GetValue_DWORD(TEXT("UserID"));
							pUserInfo->dwGameID = m_GameBattleDBAide.GetValue_DWORD(TEXT("GameID"));
							pUserInfo->cbGender = m_GameBattleDBAide.GetValue_BYTE(TEXT("Gender"));
							pUserInfo->wFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("FaceID"));
							pUserInfo->dwCustomID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CustomID"));
							m_GameBattleDBAide.GetValue_String(TEXT("NickName"), pUserInfo->szNickName, CountArray(pUserInfo->szNickName));

							//Ͷ������
							m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_INSERT_MEMBER, dwContextID, dwTokenID, &InsertMember, sizeof(InsertMember));
						}
					}
				}				
			}
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode=lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);

			return true;
		}				

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;
}

//����ɾ��
bool CDataBaseEngineSink::OnRequestApplyDelete(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize==sizeof(DBR_GR_ApplyDelete));
	if (wDataSize!=sizeof(DBR_GR_ApplyDelete)) return false;

	//��ȡ����
	auto pApplyDelete=(DBR_GR_ApplyDelete *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128]=TEXT("");			

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pApplyDelete->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pApplyDelete->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMessageID"),pApplyDelete->dwMessageID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbMessageType"),pApplyDelete->cbMessageType);									
		
		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//ִ�нű�
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_ApplyDelete"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//����ṹ
			auto pDBOApplyDelete=(DBO_GR_ApplyDelete*)m_cbBuffer;
			ZeroMemory(pDBOApplyDelete,sizeof(DBO_GR_ApplyDelete));

			//���ñ���
			pDBOApplyDelete->dwUserID = pApplyDelete->dwUserID;
			pDBOApplyDelete->dwClubID = pApplyDelete->dwClubID;
			pDBOApplyDelete->dwMessageID=pApplyDelete->dwMessageID;
			pDBOApplyDelete->cbMessageType=pApplyDelete->cbMessageType;

			//�������
			WORD wPacketSize=sizeof(DBO_GR_ApplyDelete);
			auto pdwMessageID = (DWORD*)(pDBOApplyDelete + 1);

			//��ȡ����
			while (m_GameBattleDBModule->IsRecordsetEnd() == false)
			{
				//��ȡ����				
				*pdwMessageID++ = m_GameBattleDBAide.GetValue_DWORD(TEXT("RecordID"));
				wPacketSize += sizeof(DWORD);

				//�ƶ���¼
				m_GameBattleDBModule->MoveToNext();
			}

			//��������					
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_DELETE,dwContextID,dwTokenID,m_cbBuffer,wPacketSize);
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = 1L;
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);			
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;
}

//���볷��
bool CDataBaseEngineSink::OnRequestApplyRepeal(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize==sizeof(DBR_GR_ApplyRepeal));
	if (wDataSize!=sizeof(DBR_GR_ApplyRepeal)) return false;

	//��ȡ����
	auto pApplyRepeal=(DBR_GR_ApplyRepeal *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128]=TEXT("");			

		//�������
		m_GameBattleDBAide.ResetParameter();

		//���ò���
		m_GameBattleDBAide.AddParameter(TEXT("@dwMessageID"),pApplyRepeal->dwMessageID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbMessageStatus"),APPLY_STATUS_REPEAL);
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"),pApplyRepeal->dwUserID);
		
		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//ִ�нű�
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_ApplyRepeal"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//��ȡ����
			tagClubApplyMessage ApplyMessage;
			ZeroMemory(&ApplyMessage,sizeof(ApplyMessage));	

			//��ȡ����
			while (m_GameBattleDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ����				
				ApplyMessage.cbMessageStatus=APPLY_STATUS_REPEAL;
				ApplyMessage.dwMessageID=m_GameBattleDBAide.GetValue_DWORD(TEXT("RecordID"));								

				//������Ϣ
				ApplyMessage.dwApplyerID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerID"));
				ApplyMessage.dwApplyerGameID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
				ApplyMessage.wApplyerFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("ApplyerFaceID"));
				ApplyMessage.cbApplyerGender= m_GameBattleDBAide.GetValue_BYTE(TEXT("ApplyerGender"));
				ApplyMessage.dwApplyerCustomID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("ApplyDateTime"), ApplyMessage.ApplyDateTime);
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("OperateDateTime"), ApplyMessage.OperateDateTime);
				m_GameBattleDBAide.GetValue_String(TEXT("ApplyerNickName"),ApplyMessage.szApplyerNickName,CountArray(ApplyMessage.szApplyerNickName));

				//Ⱥ����Ϣ
				ApplyMessage.dwClubID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
				ApplyMessage.dwResponderID =m_GameBattleDBAide.GetValue_DWORD(TEXT("ResponderID"));
				m_GameBattleDBAide.GetValue_String(TEXT("ClubName"),ApplyMessage.szClubName,CountArray(ApplyMessage.szClubName));

				//��ȡ����			
				auto pDBApplyMessage=(DBO_GR_ApplyMessage *)m_cbBuffer;

				//��������
				pDBApplyMessage->wMessageCount=1;
				CopyMemory(pDBApplyMessage+1,&ApplyMessage,sizeof(ApplyMessage));

				///////////////////////////////////////////////////////////////////////////////////////////////////////			

				//���ñ���
				pDBApplyMessage->dwUserID=ApplyMessage.dwApplyerID;
				pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_REQUEST;		
				
				//��������
				WORD wPacketSize=sizeof(DBO_GR_ApplyMessage)+sizeof(ApplyMessage);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,dwTokenID,pDBApplyMessage,wPacketSize);

				///////////////////////////////////////////////////////////////////////////////////////////////////////

				//���ñ���
				pDBApplyMessage->dwClubID=ApplyMessage.dwClubID;
				pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_RESPOND;		
				
				//��������
				wPacketSize=sizeof(DBO_GR_ApplyMessage)+sizeof(ApplyMessage);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,dwTokenID,pDBApplyMessage,wPacketSize);

				///////////////////////////////////////////////////////////////////////////////////////////////////////

				//�ƶ���¼
				m_GameBattleDBModule->MoveToNext();
			}

			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//���ñ���
			OperateSuccess.lOperateCode=0;
			if (DBVarValue.bstrVal)
			{
				StringCchCopy(OperateSuccess.szOperateNotice, CountArray(OperateSuccess.szOperateNotice), (LPCTSTR)(DBVarValue.bstrVal));
			}

			//��������
			WORD wPacketSize=CountStringBuffer(OperateSuccess.szOperateNotice);
			wPacketSize+=sizeof(OperateSuccess)-sizeof(OperateSuccess.szOperateNotice);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS,dwContextID,dwTokenID,&OperateSuccess,wPacketSize);
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode=lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;				
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;
}

//Լս����
bool CDataBaseEngineSink::OnRequestSettleBattle(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize==sizeof(DBR_GR_SettleBattle));
	if (wDataSize!=sizeof(DBR_GR_SettleBattle)) return false;

	//��ȡ����
	auto pSettleBattle=(DBR_GR_SettleBattle *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128]=TEXT("");			

		//�������
		m_GameBattleDBAide.ResetParameter();
			
		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwDateID"),pSettleBattle->dwDateID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pSettleBattle->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"),pSettleBattle->dwMemberID);							
		m_GameBattleDBAide.AddParameter(TEXT("@wSettleCount"),pSettleBattle->wSettleCount);
		m_GameBattleDBAide.AddParameter(TEXT("@dwCreaterID"),pSettleBattle->dwCreaterID);
		
		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//ִ�нű�
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_SettleBattle"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//���ñ���
			OperateSuccess.lOperateCode=0;
			
			//��������
			WORD wPacketSize=CountStringBuffer(OperateSuccess.szOperateNotice);
			wPacketSize+=sizeof(OperateSuccess)-sizeof(OperateSuccess.szOperateNotice);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS,dwContextID,dwTokenID,&OperateSuccess,wPacketSize);

		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode=lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode=1L;
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;
}

//�޸ķ���
bool CDataBaseEngineSink::OnRequestModifyScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize == sizeof(DBR_GR_ModifyScore));
	if (wDataSize != sizeof(DBR_GR_ModifyScore)) return false;

	//��ȡ����
	auto pModifyScore = (DBR_GR_ModifyScore *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pModifyScore->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pModifyScore->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pModifyScore->dwMemberID);
		m_GameBattleDBAide.AddParameter(TEXT("@lModifyScore"), pModifyScore->lModifyScore);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_ModifyClubScore"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_ModifyScore ModifyScore;
			ZeroMemory(&ModifyScore,sizeof(ModifyScore));

			//�޸���Ϣ
			ModifyScore.lModifyScore = pModifyScore->lModifyScore;

			//��ȡ����
			ModifyScore.wKindID = m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
			ModifyScore.wServerID = m_GameBattleDBAide.GetValue_WORD(TEXT("ServerID"));
			ModifyScore.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
			ModifyScore.dwMemberID = m_GameBattleDBAide.GetValue_DWORD(TEXT("MemberID"));
			ModifyScore.dwPayUserID = m_GameBattleDBAide.GetValue_DWORD(TEXT("PayUserID"));			
			ModifyScore.wPayUserType= m_GameBattleDBAide.GetValue_WORD(TEXT("PayUserType"));
			ModifyScore.lMemberScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("MemberScore"));
			m_GameBattleDBAide.GetValue_String(TEXT("MemberNickName"), ModifyScore.szNickName, CountArray(ModifyScore.szNickName));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_SCORE, dwContextID, dwTokenID, &ModifyScore, sizeof(ModifyScore));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//���ͻ���
bool CDataBaseEngineSink::OnRequestPresentScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize == sizeof(DBR_GR_PresentScore));
	if (wDataSize != sizeof(DBR_GR_PresentScore)) return false;

	//��ȡ����
	auto pPresentScore = (DBR_GR_PresentScore *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pPresentScore->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pPresentScore->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pPresentScore->dwMemberID);
		m_GameBattleDBAide.AddParameter(TEXT("@lPresentScore"), pPresentScore->lPresentScore);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_PresentClubScore"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_PresentScore PresentScore;
			ZeroMemory(&PresentScore, sizeof(PresentScore));

			//�����Ϣ
			PresentScore.dwClubID = pPresentScore->dwClubID;
			m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), PresentScore.szClubName, CountArray(PresentScore.szClubName));

			//������Ա
			PresentScore.dwSrcMemberID = pPresentScore->dwUserID;
			PresentScore.lPresentScore = pPresentScore->lPresentScore;
			PresentScore.lSrcMemberScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("SrcMemberScore"));
			m_GameBattleDBAide.GetValue_String(TEXT("SrcNickName"), PresentScore.szSrcNickName, CountArray(PresentScore.szSrcNickName));

			//Ŀ���Ա
			PresentScore.dwDstMemberID = pPresentScore->dwMemberID;
			PresentScore.wKindID = m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
			PresentScore.wServerID = m_GameBattleDBAide.GetValue_WORD(TEXT("ServerID"));			
			PresentScore.lDstMemberScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("DstMemberScore"));			
			m_GameBattleDBAide.GetValue_String(TEXT("DstNickName"), PresentScore.szDstNickName, CountArray(PresentScore.szDstNickName));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_PRESENT_SCORE, dwContextID, dwTokenID, &PresentScore, sizeof(PresentScore));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//��Ǽ�¼
bool CDataBaseEngineSink::OnRequestDeleteRecord(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize == sizeof(DBR_GR_DeleteRecord));
	if (wDataSize != sizeof(DBR_GR_DeleteRecord)) return false;

	//��ȡ����
	auto pDeleteRecord = (DBR_GR_DeleteRecord*)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pDeleteRecord->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pDeleteRecord->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwRecordID"), pDeleteRecord->dwRecordID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbRecordKind"), pDeleteRecord->cbRecordKind);				

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_DeleteRecord"), false);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//���ñ���
			OperateSuccess.lOperateCode = 0;
			
			//��������
			WORD wPacketSize = CountStringBuffer(OperateSuccess.szOperateNotice);
			wPacketSize += sizeof(OperateSuccess) - sizeof(OperateSuccess.szOperateNotice);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wPacketSize);
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//�������
bool CDataBaseEngineSink::OnRequestDetectClubName(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize == sizeof(DBR_GR_DetectClubName));
	if (wDataSize != sizeof(DBR_GR_DetectClubName)) return false;

	//��ȡ����
	auto pDetectClubName = (DBR_GR_DetectClubName*)pData;

	try
	{
		//�������
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@strClubName"), pDetectClubName->szClubName);		

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_DetectClubName"), false);

		//����ṹ
		DBO_GR_DetectClubName DetectClubName = {};

		//���ñ�־
		DetectClubName.bUsable=(lReturnValue==DB_SUCCESS);

		//������Ϣ
		if (DetectClubName.bUsable == false)
		{
			StringCchCopy(DetectClubName.szErrorDescribe,CountArray(DetectClubName.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0015, TEXT("��ǰ���������ѱ�ռ�ã�")));
		}

		//��������			
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DETECT_CLUBNAME, dwContextID, dwTokenID, &DetectClubName, sizeof(DetectClubName));		

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//�������
bool CDataBaseEngineSink::OnRequestCleanupScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize == sizeof(DBR_GR_CleanupScore));
	if (wDataSize != sizeof(DBR_GR_CleanupScore)) return false;

	//��ȡ����
	auto pCleanupScore = (DBR_GR_CleanupScore *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pCleanupScore->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pCleanupScore->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pCleanupScore->dwMemberID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbCleanupObject"), pCleanupScore->cbCleanupObject);
		m_GameBattleDBAide.AddParameter(TEXT("@dwCreaterGameID"), pCleanupScore->dwCreaterGameID);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_CleanupScoreRecord"), false);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_CleanupScore CleanupScore;
			ZeroMemory(&CleanupScore, sizeof(CleanupScore));

			//���ñ���
			CleanupScore.dwUserID = pCleanupScore->dwUserID;
			CleanupScore.dwClubID = pCleanupScore->dwClubID;
			CleanupScore.dwMemberID = pCleanupScore->dwMemberID;
			CleanupScore.cbCleanupObject = pCleanupScore->cbCleanupObject;

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLEANUP_SCORE, dwContextID, dwTokenID, &CleanupScore, sizeof(CleanupScore));				
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//���˰��
bool CDataBaseEngineSink::OnRequestCleanupRevenue(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize == sizeof(DBR_GR_CleanupRevenue));
	if (wDataSize != sizeof(DBR_GR_CleanupRevenue)) return false;

	//��ȡ����
	auto pCleanupRevenue = (DBR_GR_CleanupRevenue *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pCleanupRevenue->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pCleanupRevenue->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pCleanupRevenue->dwMemberID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbCleanupObject"), pCleanupRevenue->cbCleanupObject);
		m_GameBattleDBAide.AddParameter(TEXT("@dwCreaterGameID"), pCleanupRevenue->dwCreaterGameID);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_CleanupRevenueRecord"), false);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_CleanupRevenue CleanupRevenue;
			ZeroMemory(&CleanupRevenue, sizeof(CleanupRevenue));

			//���ñ���
			CleanupRevenue.dwUserID = pCleanupRevenue->dwUserID;
			CleanupRevenue.dwClubID = pCleanupRevenue->dwClubID;
			CleanupRevenue.dwMemberID = pCleanupRevenue->dwMemberID;
			CleanupRevenue.cbCleanupObject = pCleanupRevenue->cbCleanupObject;

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLEANUP_REVENUE, dwContextID, dwTokenID, &CleanupRevenue, sizeof(CleanupRevenue));	
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestCleanupSetup(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize == sizeof(DBR_GR_CleanupSetup));
	if (wDataSize != sizeof(DBR_GR_CleanupSetup)) return false;

	//��ȡ����
	auto pCleanupSetup = (DBR_GR_CleanupSetup *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pCleanupSetup->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pCleanupSetup->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@bAutoCleanup"), pCleanupSetup->bAutoCleanup);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_CleanupSetup"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��������		
			WORD wHeadSize = sizeof(DBO_GR_UpdateClub);
			DBO_GR_UpdateClub * pUpdateClub = (DBO_GR_UpdateClub*)m_cbBuffer;

			//Ⱥ���ʶ
			pUpdateClub->cbUpdateKind = UPCLUB_KIND_RULES;
			pUpdateClub->dwClubID = pCleanupSetup->dwClubID;			

			//��������
			tagClubRules * pClubRules = (tagClubRules *)&m_cbBuffer[wHeadSize];

			//Ⱥ�����
			pClubRules->cbPayType = m_GameBattleDBAide.GetValue_BYTE(TEXT("PayType"));
			pClubRules->cbBattleMask = m_GameBattleDBAide.GetValue_BYTE(TEXT("BattleMask"));
			pClubRules->cbScoreKind = m_GameBattleDBAide.GetValue_BYTE(TEXT("ScoreKind"));
			pClubRules->wSwitchOption = m_GameBattleDBAide.GetValue_WORD(TEXT("SwitchOption"));
			pClubRules->cbCurrencyKind = m_GameBattleDBAide.GetValue_BYTE(TEXT("CurrencyKind"));			

			//��������
			WORD wPacketSize = wHeadSize + sizeof(tagClubRules);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_CLUB, dwContextID, dwTokenID, m_cbBuffer, wPacketSize);

			//����ṹ
			DBO_GR_CleanupSetup CleanupSetup;
			ZeroMemory(&CleanupSetup, sizeof(CleanupSetup));

			//���ñ���
			CleanupSetup.dwUserID = pCleanupSetup->dwUserID;
			CleanupSetup.dwClubID = pCleanupSetup->dwClubID;
			CleanupSetup.bAutoCleanup = pCleanupSetup->bAutoCleanup;

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLEANUP_SETUP, dwContextID, dwTokenID, &CleanupSetup, sizeof(CleanupSetup));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//�����
bool CDataBaseEngineSink::OnRequestCleanupRankList(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize == sizeof(DBR_GR_CleanupRankList));
	if (wDataSize != sizeof(DBR_GR_CleanupRankList)) return false;

	//��ȡ����
	auto pCleanupRankList = (DBR_GR_CleanupRankList *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pCleanupRankList->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pCleanupRankList->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pCleanupRankList->dwMemberID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbListKind"), pCleanupRankList->cbListKind);
		m_GameBattleDBAide.AddParameter(TEXT("@cbCleanupObject"), pCleanupRankList->cbCleanupObject);
		m_GameBattleDBAide.AddParameter(TEXT("@dwCreaterGameID"), pCleanupRankList->dwCreaterGameID);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_CleanupRankListRecord"), false);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_CleanupRankList CleanupRankList;
			ZeroMemory(&CleanupRankList, sizeof(CleanupRankList));

			//���ñ���
			CleanupRankList.dwUserID = pCleanupRankList->dwUserID;
			CleanupRankList.dwClubID = pCleanupRankList->dwClubID;
			CleanupRankList.dwMemberID = pCleanupRankList->dwMemberID;
			CleanupRankList.cbListKind = pCleanupRankList->cbListKind;
			CleanupRankList.cbCleanupObject = pCleanupRankList->cbCleanupObject;

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLEANUP_RANKLIST, dwContextID, dwTokenID, &CleanupRankList, sizeof(CleanupRankList));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//����ϲ�
bool CDataBaseEngineSink::OnRequestRequestMerge(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize == sizeof(DBR_GR_RequestMerge));
	if (wDataSize != sizeof(DBR_GR_RequestMerge)) return false;

	//��ȡ����
	auto pRequestMerge = (DBR_GR_RequestMerge *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pRequestMerge->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwRequestClubID"), pRequestMerge->dwRequestClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwRespondClubID"), pRequestMerge->dwRespondClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwCreaterGameID"), pRequestMerge->dwCreaterGameID);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_RequestMerge"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_RequestMerge RequestMerge;
			ZeroMemory(&RequestMerge, sizeof(RequestMerge));

			//�û���Ϣ
			RequestMerge.dwUserID = pRequestMerge->dwUserID;
			RequestMerge.dwRequestClubID = pRequestMerge->dwRequestClubID;			
			RequestMerge.dwRespondClubID = pRequestMerge->dwRespondClubID;

			//��ȡ��Ϣ			
			RequestMerge.dwRecordID = m_GameBattleDBAide.GetValue_DWORD(TEXT("RecordID"));
			RequestMerge.dwRespondCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("RespondCreaterID"));			
			m_GameBattleDBAide.GetValue_String(TEXT("RequestNickName"), RequestMerge.szRequestNickName, CountArray(RequestMerge.szRequestNickName));
			m_GameBattleDBAide.GetValue_String(TEXT("RequestClubName"), RequestMerge.szRequestClubName, CountArray(RequestMerge.szRequestClubName));
			m_GameBattleDBAide.GetValue_String(TEXT("RespondClubName"), RequestMerge.szRespondClubName, CountArray(RequestMerge.szRespondClubName));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_REQUEST_MERGE, dwContextID, dwTokenID, &RequestMerge, sizeof(RequestMerge));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe),(LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//��Ӧ�ϲ�
bool CDataBaseEngineSink::OnRequestRespondMerge(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize == sizeof(DBR_GR_RespondMerge));
	if (wDataSize != sizeof(DBR_GR_RespondMerge)) return false;

	//��ȡ����
	auto pRespondMerge = (DBR_GR_RespondMerge *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pRespondMerge->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwRecordID"), pRespondMerge->dwRecordID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbRespondCode"), pRespondMerge->cbRespondCode);		
		m_GameBattleDBAide.AddParameter(TEXT("@strAuthCodeMD5"), pRespondMerge->szAuthCodeMD5);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_RespondMerge"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_RespondMerge RespondMerge;
			ZeroMemory(&RespondMerge, sizeof(RespondMerge));

			//���ñ���
			RespondMerge.dwUserID = pRespondMerge->dwUserID;
			RespondMerge.dwRecordID = pRespondMerge->dwRecordID;
			RespondMerge.cbRespondCode = pRespondMerge->cbRespondCode;

			//���ñ���			
			RespondMerge.cbRespondCode = m_GameBattleDBAide.GetValue_BYTE(TEXT("RespondCode"));
			RespondMerge.dwRequestClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("RequestClubID"));
			RespondMerge.dwRequestCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("RequestCreaterID"));
			RespondMerge.dwRespondClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("RespondClubID"));
			RespondMerge.dwRespondCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("RespondCreaterID"));
			m_GameBattleDBAide.GetValue_String(TEXT("RespondClubName"), RespondMerge.szRespondClubName, CountArray(RespondMerge.szRespondClubName));

			//ͬ��ϲ�
			if (RespondMerge.cbRespondCode == RESPOND_CODE_AGREE)
			{
				//�������
				m_GameBattleDBAide.ResetParameter();

				//�û���Ϣ
				m_GameBattleDBAide.AddParameter(TEXT("@dwRecordID"), pRespondMerge->dwRecordID);

				//�������
				m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

				//ִ�нű�
				lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_MergeClub"), false);

				//ִ�гɹ�
				if (lReturnValue == DB_SUCCESS)
				{
					//��������		
					WORD wPacketSize = CountStringBuffer(RespondMerge.szErrorDescribe);
					wPacketSize += sizeof(RespondMerge) - sizeof(RespondMerge.szErrorDescribe);
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RESPOND_MERGE, dwContextID, dwTokenID, &RespondMerge, wPacketSize);
				}
				else
				{
					//��ȡ����
					CDBVarValue DBVarValue;
					m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

					//���ñ���
					RespondMerge.lErrorCode = lReturnValue;
					StringCchCopy(RespondMerge.szErrorDescribe, CountArray(RespondMerge.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

					//��������		
					WORD wPacketSize = CountStringBuffer(RespondMerge.szErrorDescribe);
					wPacketSize += sizeof(RespondMerge) - sizeof(RespondMerge.szErrorDescribe);
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RESPOND_MERGE, dwContextID, dwTokenID, &RespondMerge, wPacketSize);
				}
			}
			else
			{
				//��������		
				WORD wPacketSize = CountStringBuffer(RespondMerge.szErrorDescribe);
				wPacketSize += sizeof(RespondMerge) - sizeof(RespondMerge.szErrorDescribe);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RESPOND_MERGE, dwContextID, dwTokenID, &RespondMerge, wPacketSize);
			}
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_RespondMerge RespondMerge;
			ZeroMemory(&RespondMerge, sizeof(RespondMerge));

			//���ñ���	
			RespondMerge.dwUserID = pRespondMerge->dwUserID;
			RespondMerge.dwRecordID = pRespondMerge->dwRecordID;
			RespondMerge.cbRespondCode = pRespondMerge->cbRespondCode;

			//���ñ���
			RespondMerge.lErrorCode = lReturnValue;
			StringCchCopy(RespondMerge.szErrorDescribe, CountArray(RespondMerge.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������		
			WORD wPacketSize = CountStringBuffer(RespondMerge.szErrorDescribe);
			wPacketSize += sizeof(RespondMerge) - sizeof(RespondMerge.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RESPOND_MERGE, dwContextID, dwTokenID, &RespondMerge, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_RespondMerge RespondMerge;
		ZeroMemory(&RespondMerge, sizeof(RespondMerge));

		//���ñ���	
		RespondMerge.dwUserID = pRespondMerge->dwUserID;
		RespondMerge.dwRecordID = pRespondMerge->dwRecordID;
		RespondMerge.cbRespondCode = pRespondMerge->cbRespondCode;

		//���ñ���
		RespondMerge.lErrorCode = 1;
		StringCchCopy(RespondMerge.szErrorDescribe, CountArray(RespondMerge.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������		
		WORD wPacketSize = CountStringBuffer(RespondMerge.szErrorDescribe);
		wPacketSize += sizeof(RespondMerge) - sizeof(RespondMerge.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RESPOND_MERGE, dwContextID, dwTokenID, &RespondMerge, wPacketSize);
	}

	return true;
}

//ȡ������
bool CDataBaseEngineSink::OnRequestCancelPayroll(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize == sizeof(DBR_GR_CancelPayroll));
	if (wDataSize != sizeof(DBR_GR_CancelPayroll)) return false;

	//��ȡ����
	auto pCancelPayroll = (DBR_GR_CancelPayroll *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pCancelPayroll->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pCancelPayroll->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pCancelPayroll->dwMemberID);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_CancelPayroll"), false);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_CancelPayroll CancelPayroll;
			ZeroMemory(&CancelPayroll, sizeof(CancelPayroll));

			//�û���Ϣ
			CancelPayroll.dwUserID = pCancelPayroll->dwUserID;
			CancelPayroll.dwClubID = pCancelPayroll->dwClubID;
			CancelPayroll.dwMemberID = pCancelPayroll->dwMemberID;			

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CANCEL_PAYROLL, dwContextID, dwTokenID, &CancelPayroll, sizeof(CancelPayroll));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//�������
bool CDataBaseEngineSink::OnRequestRequestPayroll(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize == sizeof(DBR_GR_RequestPayroll));
	if (wDataSize != sizeof(DBR_GR_RequestPayroll)) return false;

	//��ȡ����
	auto pRequestPayroll = (DBR_GR_RequestPayroll *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pRequestPayroll->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pRequestPayroll->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pRequestPayroll->dwMemberID);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_RequestPayroll"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_RequestPayroll RequestPayroll;
			ZeroMemory(&RequestPayroll, sizeof(RequestPayroll));

			//���ñ���
			RequestPayroll.dwUserID = pRequestPayroll->dwUserID;
			RequestPayroll.dwClubID = pRequestPayroll->dwClubID;
			RequestPayroll.dwMemberID = pRequestPayroll->dwMemberID;
			RequestPayroll.dwRecordID = m_GameBattleDBAide.GetValue_DWORD(TEXT("RecordID"));
			m_GameBattleDBAide.GetValue_String(TEXT("NickName"), RequestPayroll.szNickName, CountArray(RequestPayroll.szNickName));
			m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), RequestPayroll.szClubName, CountArray(RequestPayroll.szClubName));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_REQUEST_PAYROLL, dwContextID, dwTokenID, &RequestPayroll, sizeof(RequestPayroll));
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//���ñ���
			OperateFailure.lErrorCode = lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//��Ӧ����
bool CDataBaseEngineSink::OnRequestRespondPayroll(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//����У��
	ASSERT (wDataSize == sizeof(DBR_GR_RespondPayroll));
	if (wDataSize != sizeof(DBR_GR_RespondPayroll)) return false;

	//��ȡ����
	auto pRespondPayroll = (DBR_GR_RespondPayroll *)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pRespondPayroll->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwRecordID"), pRespondPayroll->dwRecordID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbRespondCode"), pRespondPayroll->cbRespondCode);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_RespondPayroll"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_RespondPayroll RespondPayroll;
			ZeroMemory(&RespondPayroll, sizeof(RespondPayroll));

			//���ñ���
			RespondPayroll.dwUserID = pRespondPayroll->dwUserID;
			RespondPayroll.dwRecordID = pRespondPayroll->dwRecordID;
			RespondPayroll.cbRespondCode = pRespondPayroll->cbRespondCode;

			//���ñ���			
			RespondPayroll.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
			RespondPayroll.dwCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterID"));

			//��������		
			WORD wPacketSize = CountStringBuffer(RespondPayroll.szErrorDescribe);
			wPacketSize += sizeof(RespondPayroll) - sizeof(RespondPayroll.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RESPOND_PAYROLL, dwContextID, dwTokenID, &RespondPayroll, wPacketSize);
		}
		else
		{
			//��ȡ����
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//����ṹ
			DBO_GR_RespondPayroll RespondPayroll;
			ZeroMemory(&RespondPayroll, sizeof(RespondPayroll));

			//���ñ���
			RespondPayroll.dwUserID = pRespondPayroll->dwUserID;
			RespondPayroll.dwRecordID = pRespondPayroll->dwRecordID;
			RespondPayroll.cbRespondCode = pRespondPayroll->cbRespondCode;

			//���ñ���
			RespondPayroll.lErrorCode = lReturnValue;
			StringCchCopy(RespondPayroll.szErrorDescribe, CountArray(RespondPayroll.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//��������		
			WORD wPacketSize = CountStringBuffer(RespondPayroll.szErrorDescribe);
			wPacketSize += sizeof(RespondPayroll) - sizeof(RespondPayroll.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RESPOND_PAYROLL, dwContextID, dwTokenID, &RespondPayroll, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//����ṹ
		DBO_GR_RespondPayroll RespondPayroll;
		ZeroMemory(&RespondPayroll, sizeof(RespondPayroll));

		//���ñ���
		RespondPayroll.dwUserID = pRespondPayroll->dwUserID;
		RespondPayroll.dwRecordID = pRespondPayroll->dwRecordID;
		RespondPayroll.cbRespondCode = pRespondPayroll->cbRespondCode;

		//���ñ���
		RespondPayroll.lErrorCode = 1;
		StringCchCopy(RespondPayroll.szErrorDescribe, CountArray(RespondPayroll.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("�������ݿ�����쳣�����Ժ����ԣ�")));

		//��������		
		WORD wPacketSize = CountStringBuffer(RespondPayroll.szErrorDescribe);
		wPacketSize += sizeof(RespondPayroll) - sizeof(RespondPayroll.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RESPOND_PAYROLL, dwContextID, dwTokenID, &RespondPayroll, wPacketSize);
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestLoadClubOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	try
	{
		//�������
		m_GameBattleDBAide.ResetParameter();

		//ִ�нű�
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadClubOption"),true);

		//ִ�гɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//��ȡ����								
			tagClubOption ClubOption = {};

			//��ȡ����
			ClubOption.wSupportClubKind = m_GameBattleDBAide.GetValue_WORD(TEXT("SupportClubKind"));
			ClubOption.wMaxJoinClubCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxJoinClubCount"));
			ClubOption.wMaxCreateClubCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxCreateClubCount"));
			ClubOption.wMaxBattleConfigCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxBattleConfigCount"));			

			//��������ѡ��
			for (BYTE cbClubKind = 0; cbClubKind < CountArray(ClubOption.ClubKindOption); cbClubKind++)
			{

				//�������
				m_GameBattleDBAide.ResetParameter();
				m_GameBattleDBAide.AddParameter(TEXT("@wClubKind"), cbClubKind);
				lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadClubKindOption"), true);

				//ִ�гɹ�
				if (lReturnValue == DB_SUCCESS)
				{
					//��ȡ����								
					auto pClubKindOption = &ClubOption.ClubKindOption[cbClubKind];

					//��ȡ����		
					pClubKindOption->cbClubKind = cbClubKind;
					pClubKindOption->cbPayTypeMask = m_GameBattleDBAide.GetValue_BYTE(TEXT("PayTypeMask"));
					pClubKindOption->cbCurrencyKindMask = m_GameBattleDBAide.GetValue_BYTE(TEXT("CurrencyKindMask"));
					pClubKindOption->wMinRevenueRate = m_GameBattleDBAide.GetValue_WORD(TEXT("MinRevenueRate"));
					pClubKindOption->wMaxRevenueRate = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxRevenueRate"));
					pClubKindOption->dwNeedUserRight = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreateNeedUserRight"));
					pClubKindOption->lCreateClubTakeCard = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("CreateClubTakeCard"));
					pClubKindOption->lCreateClubDeductCard = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("CreateClubDeductCard"));					
					pClubKindOption->lModifyClubNameDeductCard = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("ModifyClubNameDeductCard"));					
				}
			}

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_OPTION, dwContextID, dwTokenID, &ClubOption, sizeof(ClubOption));			
		}		

		//��������
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPTION_FINISH, dwContextID, dwTokenID, NULL, 0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);		
	}

	return true;
}

//��������
bool CDataBaseEngineSink::OnRequestLoadForbidWords(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	try
	{
		//�������
		m_GameBattleDBAide.ResetParameter();

		//ִ�в�ѯ
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadClubForbidWords"),true);

		//��ȡ��Ϣ
		if (lReturnValue==DB_SUCCESS)
		{
			//���建��
			WORD wPacketSize=0;
			WORD wStringCount=0;
			BYTE cbBuffer[SOCKET_PACKET];
			TCHAR szForbidWords[32]=TEXT("");

			//��ʼ��Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FIRBID_WORDS,0xFFFE,dwTokenID,NULL,0);

			//��ȡ��Ϣ
			while (m_GameBattleDBModule->IsRecordsetEnd()==false)
			{
				//��ȡ��Ϣ
				m_GameBattleDBAide.GetValue_String(TEXT("ForbidWords"),szForbidWords,32);

				//���ó���
				wStringCount=CountStringBuffer(szForbidWords);

				//��������
				if (wPacketSize+wStringCount>=sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FIRBID_WORDS,0,dwTokenID,cbBuffer,wPacketSize);
					wPacketSize=0;
				}

				//���ó���
				*(BYTE *)&cbBuffer[wPacketSize]=wStringCount;				

				//��������
				wPacketSize++;
				CopyMemory(&cbBuffer[wPacketSize],szForbidWords,CountStringBuffer(szForbidWords));

				//���ó���
				wPacketSize+=wStringCount;				

				//�ƶ���¼
				m_GameBattleDBModule->MoveToNext();
			};

			//������Ϣ
			if (wPacketSize>0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FIRBID_WORDS,0,dwTokenID,cbBuffer,wPacketSize);
			}

			//������Ϣ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FIRBID_WORDS,0xFFFF,dwTokenID,NULL,0);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//��ȡȺ��
WORD CDataBaseEngineSink::ExtractClubID(LPCTSTR pszClubIDArray, CDWordArray & ClubIDArray)
{
	//���ò���
	ClubIDArray.RemoveAll();

	//��������	
	CString strClubIDArray(pszClubIDArray);

	//��������
	INT nFirstPos=0;
	INT nSecondPos=0;
	DWORD dwClubID=0;
	CString strClubID;

	do
	{
		//��ȡλ��
		nFirstPos=strClubIDArray.Find(';',nSecondPos);
		if (nFirstPos==-1) break;

		//��ȡλ��
		nSecondPos=strClubIDArray.Find(';',nFirstPos+1);
		if (nSecondPos==-1) break;

		//��������
		strClubID=strClubIDArray.Mid(nFirstPos+1,nSecondPos-nFirstPos-1);
		dwClubID=_tcstoul(strClubID,NULL,0);

		//���Ⱥ��
		ClubIDArray.Add(dwClubID);

	} while (true);

	return (WORD)ClubIDArray.GetCount();
}

//�����Ϣ
void CDataBaseEngineSink::TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException)
{
	//������Ϣ
	char szDesc[512]="\0";
	_snprintf_s(szDesc,511,"%s(%d):%s",pFile,nLine,pFunction);
	CString strDescribe;
#ifdef _UNICODE
	TCHAR szWDesc[512]=TEXT("\0");
	int  nLen=(int)strlen(szDesc);
	MultiByteToWideChar(CP_ACP,0,szDesc,nLen,szWDesc,nLen*2);
	if(pIException!=NULL)
		strDescribe.Format(TEXT("%s:(%s)"),szWDesc,pIException->GetExceptionDescribe());
	else
		strDescribe.Format(TEXT("%s:(δ֪����)"),szWDesc);
	CTraceService::TraceString(strDescribe,TraceLevel_Exception);
#else
	if(pIException!=NULL)
		strDescribe.Format(TEXT("%s:(%s)"),szDesc,pIException->GetExceptionDescribe());
	else
		strDescribe.Format(TEXT("%s:(δ֪����)"),szDesc);
	CTraceService::TraceString(strDescribe,TraceLevel_Exception);
#endif

}
//////////////////////////////////////////////////////////////////////////

