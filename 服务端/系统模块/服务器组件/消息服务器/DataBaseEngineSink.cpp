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
	//��������
	if ((m_AccountsDBModule.GetInterface()==NULL)&&(m_AccountsDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if ((m_PlatformDBModule.GetInterface() == NULL) && (m_PlatformDBModule.CreateInstance() == false))
	{
		ASSERT(FALSE);
		return false;
	}


	try
	{
		//��������
		tagDataBaseParameter * pAccountsDBParameter =&m_pInitParameter->m_AccountsDBParameter;
		tagDataBaseParameter * pPlatformDBParameter = &m_pInitParameter->m_PlatformDBParameter;

		//��������
		m_AccountsDBModule->SetConnectionInfo(pAccountsDBParameter->dwDataBaseAddr, pAccountsDBParameter->wDataBasePort,
		CW2CT(pAccountsDBParameter->szDataBaseName),CW2CT(pAccountsDBParameter->szDataBaseUser),CW2CT(pAccountsDBParameter->szDataBasePass));

		//��������
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->dwDataBaseAddr, pPlatformDBParameter->wDataBasePort,
			CW2CT(pPlatformDBParameter->szDataBaseName), CW2CT(pPlatformDBParameter->szDataBaseUser), CW2CT(pPlatformDBParameter->szDataBasePass));
	
		//��������
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());	

		//��������
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

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
	m_AccountsDBAide.SetDataBase(NULL);
	m_PlatformDBAide.SetDataBase(NULL);

	//�ر�����
	if (m_AccountsDBModule.GetInterface()!=NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
	}

	//�ر�����
	if (m_PlatformDBModule.GetInterface() != NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
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
	case DBR_GR_REQUEST_WEBPAGE:		//������ҳ
	{
		return OnRequestRequestWebPage(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_USER_QUERY_MAIL:		//��ѯ�ʼ�
	{
		return OnRequestUserQueryMail(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_USER_INSERT_MAIL:		//����ʼ�
	{
		return OnRequestUserInsertMail(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_USER_MODIFY_MAIL:		//�޸��ʼ�
	{
		return OnRequestUserModifyMail(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_CLAIM_MAIL_ATTACH:		//��ȡ����
	{
		return OnRequestClaimMailAttach(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_USER_DELETE_MAIL:		//ɾ���ʼ�
	{
		return OnRequestUserDeleteMail(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	}

	return false;
}

//������ҳ
bool CDataBaseEngineSink::OnRequestRequestWebPage(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//��ȡ����
	ASSERT (wDataSize <= sizeof(DBR_GR_RequestWebPage));
	if (wDataSize > sizeof(DBR_GR_RequestWebPage)) return false;

	//��ȡ����
	DBR_GR_RequestWebPage * pRequestWebPage = (DBR_GR_RequestWebPage *)pData;

	//������ҳ
	CStringA strResult = m_WininetHttp.RequestWebPage(pRequestWebPage->szWebUrl, pRequestWebPage->hrType, pRequestWebPage->szHeader, pRequestWebPage->szPostData);

	//����ɹ�
	if (m_WininetHttp.GetLastError() == Hi_Success)
	{
		//ת������
		TCHAR szResult[256] = { 0 };
		CWHService::Utf82Unicode(strResult, szResult, CountArray(szResult));		

		//��ȡ��Ϣ
		auto JsonValuePtr = CWHJson::getValue(string(CT2CA(szResult)));
		if (JsonValuePtr->getType() != eJsonTypeObj)
		{
			//��������
			TCHAR szMessage[256];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��ҳ�������[δ֪����]��%hs"), pRequestWebPage->szWebUrl);

			//�������
			CTraceService::TraceString(szMessage, TraceLevel_Exception);

			return true;
		}

		//ת������
		LONG lResultCode = 0;
		auto JsonValueObjPtr = CJsonValueObjPtr::dynamicCast(JsonValuePtr);

		//��ȡ���		
		CWHJsonInput::readJson(lResultCode, JsonValueObjPtr->value["code"], false);

		//�������
		CTraceService::TraceString(szResult, TraceLevel_Exception);

		//У����
		if (lResultCode != 200)
		{
			//��������
			TCHAR szMessage[512];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��ҳ�������[%s]��%hs"), szResult, pRequestWebPage->szWebUrl);

			//�������
			CTraceService::TraceString(szMessage, TraceLevel_Exception);

			return true;
		}
	}
	else
	{
		//��������
		TCHAR szMessage[256];
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("��ҳ�������[%d]��%hs"), m_WininetHttp.GetLastError(), pRequestWebPage->szWebUrl);

		//�������
		CTraceService::TraceString(szMessage, TraceLevel_Exception);
	}

	return true;
}

//��ѯ�ʼ�
bool CDataBaseEngineSink::OnRequestUserQueryMail(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//Ч�����
		ASSERT(wDataSize == sizeof(DBR_GR_UserQueryMail));
		if (wDataSize != sizeof(DBR_GR_UserQueryMail)) return false;

		//������
		DBR_GR_UserQueryMail* pUserQueryMail = (DBR_GR_UserQueryMail*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUserQueryMail->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwMailID"), pUserQueryMail->dwLatestMailID);
		m_AccountsDBAide.AddParameter(TEXT("@@dwCurrentTime"), (DWORD)time(NULL));

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserQueryMail"), true);

		//�û���Ϣ
		if (lResultCode == DB_SUCCESS)
		{
			//�������
			DBO_GR_UserMailInfo UserMailInfo;
			ZeroMemory(&UserMailInfo, sizeof(UserMailInfo));

			//��������
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				if (UserMailInfo.wMailCount >= CountArray(UserMailInfo.MailInfo))
				{
					//��������
					WORD wHeadSize = sizeof(UserMailInfo) - sizeof(UserMailInfo.MailInfo);
					WORD wDataSize = UserMailInfo.wMailCount * sizeof(UserMailInfo.MailInfo[0]);
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_MAIL_INFO, dwContextID, dwTokenID, &UserMailInfo, wHeadSize + wDataSize);

					//��������
					UserMailInfo.wMailCount = 0;
				}

				//�ʼ���Ϣ
				tagMailInfo* pMailInfo = &UserMailInfo.MailInfo[UserMailInfo.wMailCount++];

				//��ȡ����
				pMailInfo->dwMailID = m_AccountsDBAide.GetValue_DWORD(TEXT("MailID"));
				pMailInfo->cbMailKind = m_AccountsDBAide.GetValue_BYTE(TEXT("MailKind"));
				pMailInfo->cbMailType = m_AccountsDBAide.GetValue_BYTE(TEXT("MailType"));
				pMailInfo->cbMailState = m_AccountsDBAide.GetValue_BYTE(TEXT("MailState"));
				pMailInfo->dwSendTime = m_AccountsDBAide.GetValue_DWORD(TEXT("SendTime"));
				pMailInfo->dwExpireTime = m_AccountsDBAide.GetValue_DWORD(TEXT("ExpireTime"));
				m_AccountsDBAide.GetValue_String(TEXT("MailTitle"), pMailInfo->szMailTitle, CountArray(pMailInfo->szMailTitle));
				m_AccountsDBAide.GetValue_String(TEXT("MailContent"), pMailInfo->szMailContent, CountArray(pMailInfo->szMailContent));
				m_AccountsDBAide.GetValue_String(TEXT("AttachGoods"), pMailInfo->szAttachGoods, CountArray(pMailInfo->szAttachGoods));

				//�ƶ���¼
				m_AccountsDBModule->MoveToNext();
			}

			//��������
			WORD wHeadSize = sizeof(UserMailInfo) - sizeof(UserMailInfo.MailInfo);
			WORD wDataSize = UserMailInfo.wMailCount * sizeof(UserMailInfo.MailInfo[0]);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_MAIL_INFO, dwContextID, dwTokenID, &UserMailInfo, wHeadSize + wDataSize);
		}

		//�ʼ����
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_MAIL_FINISH, dwContextID, dwTokenID, NULL, NULL);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		//OperateFailure.wRequestCmdID = SUB_MP_USER_QUERY_MAIL;
		OperateFailure.lErrorCode = DB_ERROR;
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("�������ݿ�����쳣�������Ժ����ԣ�"));

		//��������
		WORD wDataSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wHeadSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wHeadSize + wDataSize);

		return false;
	}

	return true;
}

//�޸��ʼ�
bool CDataBaseEngineSink::OnRequestUserModifyMail(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//У�����
		ASSERT(wDataSize == sizeof(DBR_GR_UserModifyMail));
		if (wDataSize != sizeof(DBR_GR_UserModifyMail)) return false;

		//��������
		DBR_GR_UserModifyMail* pUserModifyMail = (DBR_GR_UserModifyMail*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUserModifyMail->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwMailID"), pUserModifyMail->dwMailID);
		m_AccountsDBAide.AddParameter(TEXT("@cbMailState"), pUserModifyMail->cbMailState);

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserModifyMail"), false);

		//�ʼ���Ϣ
		if (lResultCode == DB_SUCCESS)
		{
			//�������
			DBO_GR_UserMailUpdate UserMailUpdate;
			ZeroMemory(&UserMailUpdate, sizeof(UserMailUpdate));

			//���ñ���
			UserMailUpdate.dwMailID = pUserModifyMail->dwMailID;
			UserMailUpdate.cbMailState = MAIL_STATE_HAVETOSEE;

			//��������			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_MAIL_UPDATE, dwContextID, dwTokenID, &UserMailUpdate, sizeof(UserMailUpdate));
		}

		return true;

	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		//OperateFailure.wRequestCmdID = SUB_MR_USER_VIEW_MAIL;
		OperateFailure.lErrorCode = DB_ERROR;
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("�������ݿ�����쳣�������Ժ����ԣ�"));

		//��������
		WORD wDataSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wHeadSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wHeadSize + wDataSize);
	}

	return true;
}

//�����ʼ�
bool CDataBaseEngineSink::OnRequestUserInsertMail(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//����У��
		ASSERT(wDataSize <= sizeof(DBR_GR_UserInsertMail));
		if (wDataSize > sizeof(DBR_GR_UserInsertMail)) return false;

		//��������
		DBR_GR_UserInsertMail* pUserInsertMail = (DBR_GR_UserInsertMail*)pData;

		//У�����
		if (pUserInsertMail->dwUserID != 0)
		{
			CString strReceiverID;
			strReceiverID.Format(TEXT("[%d]"), pUserInsertMail->dwUserID);

			//�������
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@dwMailID"), pUserInsertMail->dwMailID);
			m_AccountsDBAide.AddParameter(TEXT("@cbMailState"), MAIL_STATE_UNTREATED);
			m_AccountsDBAide.AddParameter(TEXT("@dwExpireTime"), time(NULL)+ ONE_MONTH);
			m_AccountsDBAide.AddParameter(TEXT("@szReceiverID"), strReceiverID);

			//ִ�в�ѯ
			LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_InsertMailBox"), false);
		}
		else
		{
			//�������
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@dwMailID"), pUserInsertMail->dwMailID);
			m_AccountsDBAide.AddParameter(TEXT("@LastLogonTime"), pUserInsertMail->LastLogonTime);			

			//ִ�в�ѯ
			LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CollectMailUser"), false);

			//ִ�гɹ�
			if (lResultCode == DB_SUCCESS)
			{
				do
				{
					//�������
					m_AccountsDBAide.ResetParameter();
					m_AccountsDBAide.AddParameter(TEXT("@dwMailID"), pUserInsertMail->dwMailID);
					m_AccountsDBAide.AddParameter(TEXT("@cbMailState"), MAIL_STATE_UNTREATED);
					m_AccountsDBAide.AddParameter(TEXT("@dwExpireTime"), time(NULL) + ONE_MONTH);
					m_AccountsDBAide.AddParameter(TEXT("@wUserCount"), 5000);

					//ִ�в�ѯ
					lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_BatchInsertMail"), true);
					if (lResultCode == DB_SUCCESS)
					{
						//ʣ������
						DWORD dwResidueCount = m_AccountsDBAide.GetValue_DWORD(TEXT("ResidueCount"));
						if (dwResidueCount == 0) break;
					}

					//�߳�˯��
					Sleep(32);

				} while (true);
			}
		}

		//������
		DBO_GR_UserMailInsert  UserInsertMail;
		ZeroMemory(&UserInsertMail, sizeof(UserInsertMail));

		//���ñ���
		UserInsertMail.dwMailID = pUserInsertMail->dwMailID;
		UserInsertMail.dwUserID = pUserInsertMail->dwUserID;

		//��������			
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_MAIL_INSERT, dwContextID, dwTokenID, &UserInsertMail, sizeof(UserInsertMail));
		
		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);	

		//���û���
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//��ȡ����
bool CDataBaseEngineSink::OnRequestClaimMailAttach(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//У�����
		ASSERT(wDataSize == sizeof(DBR_GR_ClaimMailAttach));
		if (wDataSize != sizeof(DBR_GR_ClaimMailAttach)) return false;

		//��������
		DBR_GR_ClaimMailAttach* pClaimMailAttach = (DBR_GR_ClaimMailAttach*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pClaimMailAttach->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwMailID"), pClaimMailAttach->dwMailID);
		m_AccountsDBAide.AddParameter(TEXT("@dwTimeStamp"), (DWORD)time(NULL));
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_CLAIM_MAIL_ATTACH);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString));

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ClaimMailAttach"), true);

		//�ʼ���Ϣ
		if (lResultCode == DB_SUCCESS)
		{
			//�������
			DBO_GR_UserMailUpdate UserMailUpdate;
			ZeroMemory(&UserMailUpdate, sizeof(UserMailUpdate));

			//���ñ���
			UserMailUpdate.cbMailState = MAIL_STATE_HAVECLAIM;
			UserMailUpdate.dwMailID = pClaimMailAttach->dwMailID;

			//��������			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_MAIL_UPDATE, dwContextID, dwTokenID, &UserMailUpdate, sizeof(UserMailUpdate));

			//�������
			DBO_GR_ClaimMailAttach ClaimMailAttach;
			ZeroMemory(&ClaimMailAttach, sizeof(ClaimMailAttach));

			//���ñ���
			ClaimMailAttach.lResultCode = lResultCode;
			ClaimMailAttach.dwMailID = pClaimMailAttach->dwMailID;
			m_AccountsDBAide.GetValue_String(TEXT("AttachGoods"), ClaimMailAttach.szAttachGoods, CountArray(ClaimMailAttach.szAttachGoods));

			//������Ʒ
			CGoodsItemArray MoneyGoodsList;
			CGoodsItemArray NormalGoodsList;

			//������Ʒ
			if (g_GameGoodsManager->ParseFromJson(ClaimMailAttach.szAttachGoods, MoneyGoodsList, NormalGoodsList))
			{
				//������Ʒ
				if (MoneyGoodsList.GetCount() != 0)
				{
					//���²Ƹ�
					UpdateUserWealth(pClaimMailAttach->dwUserID, dwContextID, dwTokenID);
				}

				//��ѯ��Ʒ
				if (NormalGoodsList.GetCount() != 0)
				{
					UpdateUserGoods(pClaimMailAttach->dwUserID, dwContextID, dwTokenID, NormalGoodsList.GetData(), (WORD)NormalGoodsList.GetCount());
				}
			}

			//��������			
			WORD wHeadSize = sizeof(ClaimMailAttach) - sizeof(ClaimMailAttach.szAttachGoods);
			WORD wDataSize = CountStringBuffer(ClaimMailAttach.szAttachGoods);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLAIM_MAIL_ATTACH, dwContextID, dwTokenID, &ClaimMailAttach, wHeadSize + wDataSize);
		}
		else
		{
			//��������
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.lErrorCode = lResultCode;
			OperateFailure.wRequestCmdID = SUB_AN_C_CLAIM_MAIL_ATTACH;
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), (CW2CT)DBVarValue.bstrVal);

			//����������
			if (lResultCode == 4)
			{
				OperateFailure.lErrorCode = OFC_MAIL_NOTEXIST;
			}

			//���ͽ��
			WORD wStringSize = CountStringBufferW(OperateFailure.szDescribeString);
			WORD wPacketSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = OFC_DB_EXCEPTION;
		OperateFailure.wRequestCmdID = SUB_AN_C_CLAIM_MAIL_ATTACH;
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("�������ݿ�����쳣�������Ժ����ԣ�"));

		//��������
		WORD wDataSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wHeadSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wHeadSize + wDataSize);
	}

	return true;
}

//ɾ���ʼ�
bool CDataBaseEngineSink::OnRequestUserDeleteMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//У�����
		ASSERT(wDataSize == sizeof(DBR_GR_UserDeleteMail));
		if (wDataSize != sizeof(DBR_GR_UserDeleteMail)) return false;

		//��������
		DBR_GR_UserDeleteMail* pDeleteMail = (DBR_GR_UserDeleteMail*)pData;

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pDeleteMail->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwMailID"), pDeleteMail->dwMailID);

		//�������
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString));

		//ִ�в�ѯ
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserDeleteMail"), true);

		//�ʼ���Ϣ
		if (lResultCode == DB_SUCCESS)
		{

			//�������
			DBO_GR_UserMailUpdate UserMailUpdate;
			ZeroMemory(&UserMailUpdate, sizeof(UserMailUpdate));

			//���ñ���
			UserMailUpdate.cbMailState = MAIL_STATE_ONDELETED;
			UserMailUpdate.dwMailID = pDeleteMail->dwMailID;

			//��������			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_MAIL_UPDATE, dwContextID, dwTokenID, &UserMailUpdate, sizeof(UserMailUpdate));

			//�������
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//�������
			OperateSuccess.lErrorCode = 0;
			OperateSuccess.wRequestCmdID = SUB_AN_C_USER_DELETE_MAIL;

			//���ͽ��
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wPacketSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wPacketSize);
		}
		else
		{
			//��������
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//��ȡ��Ϣ
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//�������
			OperateFailure.lErrorCode = lResultCode;
			OperateFailure.wRequestCmdID = SUB_AN_C_USER_DELETE_MAIL;
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), (CW2CT)DBVarValue.bstrVal);

			//���ͽ��
			WORD wStringSize = CountStringBufferW(OperateFailure.szDescribeString);
			WORD wPacketSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//������
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//�������
		OperateFailure.lErrorCode = OFC_DB_EXCEPTION;
		OperateFailure.wRequestCmdID = SUB_AN_C_USER_DELETE_MAIL;
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("�������ݿ�����쳣�������Ժ����ԣ�"));

		//��������
		WORD wDataSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wHeadSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wHeadSize + wDataSize);
	}

	return true;
}

//���²Ƹ�
bool CDataBaseEngineSink::UpdateUserWealth(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryGameWealth"), true);

		//���سɹ�
		if (lResultCode == DB_SUCCESS)
		{
			//���²Ƹ�
			DBO_GR_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

			//���ñ���
			WealthUpdate.wItemCount = 2;
			WealthUpdate.WealthItem[0].wMoneyID = GOODS_ID_GOLD;
			WealthUpdate.WealthItem[0].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));
			WealthUpdate.WealthItem[1].wMoneyID = GOODS_ID_CARD;
			WealthUpdate.WealthItem[1].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserCard"));			
			
			//���ͽ��			
			WORD wHeadSize = sizeof(WealthUpdate) - sizeof(WealthUpdate.WealthItem);
			WORD wPacketSize = WealthUpdate.wItemCount * sizeof(WealthUpdate.WealthItem[0]) + wHeadSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_WEALTH_UPDATE, dwContextID, dwTokenID ,&WealthUpdate, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

//������Ʒ
bool CDataBaseEngineSink::UpdateUserGoods(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID, tagGoodsItem GoodsItemList[], WORD wGoodsCount)
{
	try
	{
		//��������
		CString strGoodsID;
		CString strGoodsIDList;

		//������Ʒ�б�
		for (auto i = 0; i < wGoodsCount; i++)
		{
			//�������
			if (i == 0) strGoodsIDList.Append(TEXT("["));

			//��ʽ��
			strGoodsID.Format(TEXT("%d"), GoodsItemList[i].wGoodsID);

			//׷������
			strGoodsIDList.Append(strGoodsID);
			strGoodsIDList.Append((i < wGoodsCount - 1) ? TEXT(",") : TEXT("]"));
		}

		//�������
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@szGoodsIDList"), strGoodsIDList);
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserQueryGoods"), true);

		//���سɹ�
		if (lResultCode == DB_SUCCESS)
		{
			//��������
			DBO_GR_GoodsUpdate UserGoodsUpdate;
			ZeroMemory(&UserGoodsUpdate, sizeof(UserGoodsUpdate));

			//��ȡ��Ϣ
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				//��ȡ����
				auto pGoodsHoldInfo = &UserGoodsUpdate.GoodsHoldInfo[UserGoodsUpdate.wGoodsCount++];

				//��Ʒ��Ϣ
				pGoodsHoldInfo->wGoodsID = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsID"));
				pGoodsHoldInfo->wGoodsKind = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsKind"));
				pGoodsHoldInfo->wGoodsLevel = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsLevel"));				
				pGoodsHoldInfo->dwGoodsCount = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsCount"));
				pGoodsHoldInfo->dwGoodsIndate = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsIndate"));
				pGoodsHoldInfo->dwExpireTime = m_AccountsDBAide.GetValue_DWORD(TEXT("ExpireTime"));

				//�ƶ���¼
				m_AccountsDBModule->MoveToNext();
			}

			//��������
			WORD wHeadSize = sizeof(UserGoodsUpdate) - sizeof(UserGoodsUpdate.GoodsHoldInfo);
			WORD wPacketSize = UserGoodsUpdate.wGoodsCount * sizeof(UserGoodsUpdate.GoodsHoldInfo[0]) + wHeadSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_GOODS_UPDATE, dwContextID, dwTokenID, &UserGoodsUpdate, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

//�����Ϣ
void CDataBaseEngineSink::TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException)
{
	CString strMessage;
	strMessage.Format(TEXT("%s(%d):%hs(%s)"), CA2CT(pFile).m_psz, nLine, pFunction, pIException ? pIException->GetExceptionDescribe() : TEXT(""));
	CTraceService::TraceString(strMessage, TraceLevel_Exception);

}
//////////////////////////////////////////////////////////////////////////

