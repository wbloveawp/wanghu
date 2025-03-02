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
	if ((m_GameBattleDBModule.GetInterface()==NULL)&&(m_GameBattleDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}		

	try
	{
		//��������		
		tagDataBaseParameter * pIMClubDBParameter=&m_pInitParameter->m_GameBattleDBParameter;

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
	case DBR_GR_QUERY_MEMBER:		//��ѯ��Ա
	{
		return OnRequestQueryMember(dwContextID,dwTokenID,pData,wDataSize);
	}
	case DBR_GR_UPDATE_MEMBER:		//���³�Ա
	{
		return OnRequestUpdateMember(dwContextID,dwTokenID,pData,wDataSize);
	}
	case DBR_GR_SET_WINNER_SCORE:	//����Ӯ��
	{
		return OnRequestSetWinnerScore(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_LOAD_CLUB_LIST:		//�����б�
	{
		return OnRequestLoadClubList(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_LOAD_CLUB_ITEM:		//��������
	{
		return OnRequestLoadClubItem(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_LOAD_CLUB_MEMBER:		//�����Ա
	{
		return OnRequestLoadClubMember(dwContextID, dwTokenID, pData,wDataSize);
	}	
	case DBR_GR_LOAD_BATTLE_CONFIG:		//Լս�淨
	{
		return OnRequestLoadBattleConfig(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_LOAD_APPLY_MESSAGE:		//������Ϣ
	{
		return OnRequestLoadApplyMessage(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_LOAD_KIND_SETTING:		//��������
	{
		return OnRequestLoadKindSetting(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_APPEND_CONFIG:			//����淨
	{
		return OnRequestAppendConfig(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_MODIFY_CONFIG:			//�޸��淨
	{
		return OnRequestModifyConfig(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_DELETE_CONFIG:			//ɾ���淨
	{
		return OnRequestDeleteConfig(dwContextID, dwTokenID, pData, wDataSize);
	}
	}

	return false;
}

//�����Ա
bool CDataBaseEngineSink::OnRequestLoadClubMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��ȡ����
		DBR_GR_LoadClubMember * pLoadClubMember=(DBR_GR_LoadClubMember *)pData;

		//�������
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pLoadClubMember->dwClubID);
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_QueryClubMember"),true);

		//���سɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//��������			
			BYTE cbBuffer[MAX_ASYNC_DATA];
			ZeroMemory(cbBuffer,sizeof(cbBuffer));

			//��������
			DBO_GR_ClubMember * pGRClubMember=(DBO_GR_ClubMember *)cbBuffer;
			pGRClubMember->dwClubID=pLoadClubMember->dwClubID;

			//��������
			tagClubUserInfo * pUserInfo=NULL;
			tagClubMemberInfo * pMemberInfo=NULL;			
			tagClubMemberUser * pClubMemberUser=NULL;
			WORD wPacketSize=sizeof(DBO_GR_ClubMember);			

			//�����Ա
			while (m_GameBattleDBModule->IsRecordsetEnd()==false)
			{
				//������Ϣ
				if ((wPacketSize+sizeof(tagClubMemberUser))>sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_MEMBER,dwContextID,dwTokenID,cbBuffer,wPacketSize);
					wPacketSize=sizeof(DBO_GR_ClubMember);	
				}

				//��ȡ��Ϣ
				pClubMemberUser=(tagClubMemberUser *)(cbBuffer+wPacketSize);

				//���ñ���
				pUserInfo=&pClubMemberUser->UserInfo;
				pMemberInfo=&pClubMemberUser->MemberInfo;

				//��Ա��Ϣ
				pMemberInfo->dwMemberID=m_GameBattleDBAide.GetValue_DWORD(TEXT("UserID"));				
				pMemberInfo->cbMemberType=m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberType"));
				pMemberInfo->cbMemberStatus=m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberStatus"));
				pMemberInfo->dwManageRight = m_GameBattleDBAide.GetValue_DWORD(TEXT("ManageRight"));
				pMemberInfo->lClubScore=m_GameBattleDBAide.GetValue_LONGLONG(TEXT("ClubScore"));
				pMemberInfo->dwCreateCount=m_GameBattleDBAide.GetValue_DWORD(TEXT("CreateCount"));				
				pMemberInfo->dwBattleCount=m_GameBattleDBAide.GetValue_DWORD(TEXT("BattleCount"));	
				pMemberInfo->lRevenueCount = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("RevenueCount"));
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("JoinDateTime"),pMemberInfo->JoinDateTime);
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("LastBattleTime"), pMemberInfo->LastBattleTime);				

				//�û���Ϣ
				pUserInfo->cbUserStatus=US_IM_OFFLINE;
				pUserInfo->dwUserID=m_GameBattleDBAide.GetValue_DWORD(TEXT("UserID"));				
				pUserInfo->dwGameID=m_GameBattleDBAide.GetValue_DWORD(TEXT("GameID"));	
				pUserInfo->wFaceID=m_GameBattleDBAide.GetValue_WORD(TEXT("FaceID"));
				pUserInfo->cbGender=m_GameBattleDBAide.GetValue_BYTE(TEXT("Gender"));					
				pUserInfo->dwCustomID=m_GameBattleDBAide.GetValue_DWORD(TEXT("CustomID"));
				m_GameBattleDBAide.GetValue_String(TEXT("NickName"),pUserInfo->szNickName,CountArray(pUserInfo->szNickName));
				m_GameBattleDBAide.GetValue_String(TEXT("UserFacePath"), pUserInfo->szUserHeadUrl, CountArray(pUserInfo->szUserHeadUrl));//kk jia

				//����λ��
				wPacketSize+=sizeof(tagClubMemberUser);

				//�ƶ���¼
				m_GameBattleDBModule->MoveToNext();
			}

			//����ʣ��
			if (wPacketSize>0) 
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_MEMBER,dwContextID,dwTokenID,cbBuffer,wPacketSize);
			}							
		}		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//Լս�淨
bool CDataBaseEngineSink::OnRequestLoadBattleConfig(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��ȡ����
		DBR_GR_LoadBattleConfig * pLoadBattleConfig=(DBR_GR_LoadBattleConfig *)pData;

		//�������
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pLoadBattleConfig->dwClubID);
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadBattleConfig"),true);

		//���سɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//��������			
			BYTE cbBuffer[MAX_ASYNC_DATA];
			ZeroMemory(cbBuffer,sizeof(cbBuffer));

			//��������
			DBO_GR_BattleConfig * pDBOBattleConfig=(DBO_GR_BattleConfig *)cbBuffer;

			//���ñ���
			pDBOBattleConfig->dwClubID=pLoadBattleConfig->dwClubID;						

			//��������			
			WORD wPacketSize=sizeof(DBO_GR_BattleConfig);		
			tagClubBattleConfig * pBattleConfig=(tagClubBattleConfig *)(pDBOBattleConfig+1);			

			//�����Ա
			while (m_GameBattleDBModule->IsRecordsetEnd()==false)
			{
				//������Ϣ
				if ((wPacketSize+sizeof(tagClubBattleConfig))>sizeof(cbBuffer))
				{
					//Ͷ������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_CONFIG,dwContextID,dwTokenID,cbBuffer,wPacketSize);

					//���ñ���
					wPacketSize=sizeof(DBO_GR_BattleConfig);
					pBattleConfig=(tagClubBattleConfig *)(pDBOBattleConfig+1);
				}

				//������Ϣ
				pBattleConfig->wKindID=m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
				pBattleConfig->dwConfigID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ConfigID"));
				m_GameBattleDBAide.GetValue_String(TEXT("BattleConfig"),pBattleConfig->szBattleConfig,CountArray(pBattleConfig->szBattleConfig));

				//����λ��
				pBattleConfig++;
				wPacketSize+=sizeof(tagClubBattleConfig);

				//�ƶ���¼
				m_GameBattleDBModule->MoveToNext();
			}

			//����ʣ��
			if (wPacketSize>0) 
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_CONFIG,dwContextID,dwTokenID,cbBuffer,wPacketSize);
			}							
		}		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//��Ϸ����
bool CDataBaseEngineSink::OnRequestLoadKindSetting(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	try
	{
		//��ȡ����
		auto pLoadKindSetting = (DBR_GR_LoadKindSetting*)pData;

		//�������
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pLoadKindSetting->dwClubID);
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadKindSetting"), true);

		//���سɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��������			
			BYTE cbBuffer[SOCKET_BUFFER];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//��������
			auto wPacketSize = sizeof(DBO_GR_ClubKindSetting);
			auto pDBOClubKindSetting = (DBO_GR_ClubKindSetting *)cbBuffer;

			//�����ʶ
			pDBOClubKindSetting->dwClubID = pLoadKindSetting->dwClubID;

			//�����Ա
			while (m_GameBattleDBModule->IsRecordsetEnd() == false)
			{
				//������Ϣ
				if ((wPacketSize + sizeof(tagClubKindSetting)) > sizeof(cbBuffer))
				{
					//Ͷ������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_KIND_SETTING, dwContextID, dwTokenID, cbBuffer, wPacketSize);

					//���ñ���
					pDBOClubKindSetting->wItemCount = 0;
					wPacketSize = sizeof(DBO_GR_ClubKindSetting);					
				}

				auto pKindSettingItem = &pDBOClubKindSetting->KindSettingItem[pDBOClubKindSetting->wItemCount++];

				//������Ϣ
				pKindSettingItem->wKindID = m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
				pKindSettingItem->lMinWinnerScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("MinWinnerScore"));
				
				//����λ��				
				wPacketSize += sizeof(tagClubBattleConfig);

				//�ƶ���¼
				m_GameBattleDBModule->MoveToNext();
			}

			//����ʣ��
			if (wPacketSize > 0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_KIND_SETTING, dwContextID, dwTokenID, cbBuffer, wPacketSize);
			}
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//������Ϣ
bool CDataBaseEngineSink::OnRequestLoadApplyMessage(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��ȡ����
		DBR_GR_LoadApplyMessage * pLoadApplyMessage = (DBR_GR_LoadApplyMessage *)pData;

		//�������
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), 0);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pLoadApplyMessage->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbMessageType"), APPLY_MSG_TYPE_RESPOND);
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadApplyMessage"), true);

		//���سɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��������			
			BYTE cbBuffer[SOCKET_BUFFER];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//��������
			DBO_GR_ApplyMessage * pDBOApplyMessage = (DBO_GR_ApplyMessage *)cbBuffer;

			//���ñ���
			pDBOApplyMessage->dwClubID = pLoadApplyMessage->dwClubID;

			//��������			
			WORD wPacketSize = sizeof(DBO_GR_ApplyMessage);
			tagClubApplyMessage * pApplyMessage = (tagClubApplyMessage *)(pDBOApplyMessage + 1);

			//�����Ա
			while (m_GameBattleDBModule->IsRecordsetEnd() == false)
			{
				//������Ϣ
				if ((wPacketSize + sizeof(tagClubApplyMessage)) > sizeof(cbBuffer))
				{
					//Ͷ������
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE, dwContextID, dwTokenID, cbBuffer, wPacketSize);

					//���ñ���
					wPacketSize=sizeof(DBO_GR_ApplyMessage);
					pApplyMessage=(tagClubApplyMessage *)(pDBOApplyMessage+1);
				}

				//��ȡ����
				pApplyMessage->dwMessageID = m_GameBattleDBAide.GetValue_DWORD(TEXT("RecordID"));
				pApplyMessage->cbMessageStatus = m_GameBattleDBAide.GetValue_BYTE(TEXT("ApplyStatus"));

				//������Ϣ
				pApplyMessage->dwApplyerID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerID"));
				pApplyMessage->dwApplyerGameID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
				pApplyMessage->wApplyerFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("ApplyerFaceID"));
				pApplyMessage->cbApplyerGender = m_GameBattleDBAide.GetValue_BYTE(TEXT("ApplyerGender"));
				pApplyMessage->dwApplyerCustomID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("ApplyDateTime"), pApplyMessage->ApplyDateTime);
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("OperateDateTime"), pApplyMessage->OperateDateTime);
				m_GameBattleDBAide.GetValue_String(TEXT("ApplyerNickName"), pApplyMessage->szApplyerNickName, CountArray(pApplyMessage->szApplyerNickName));

				//������Ϣ
				pApplyMessage->dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
				pApplyMessage->dwResponderID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ResponderID"));
				m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), pApplyMessage->szClubName, CountArray(pApplyMessage->szClubName));

				//����λ��
				pApplyMessage++;
				wPacketSize+=sizeof(tagClubApplyMessage);

				//�ƶ���¼
				m_GameBattleDBModule->MoveToNext();
			}

			//Ͷ������
			if (wPacketSize > sizeof(DBO_GR_ApplyMessage))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE, dwContextID, dwTokenID, cbBuffer, wPacketSize);
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}


//����淨
bool CDataBaseEngineSink::OnRequestAppendConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize == sizeof(DBR_GR_AppendConfig));
	if (wDataSize != sizeof(DBR_GR_AppendConfig)) return false;

	//��ȡ����
	auto pAppendConfig = (DBR_GR_AppendConfig*)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");
		tagClubBattleConfig* pBattleConfig = &pAppendConfig->BattleConfig;

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pAppendConfig->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@wKindID"), pBattleConfig->wKindID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pAppendConfig->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@strBattleConfig"), pBattleConfig->szBattleConfig);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_AppendBattleConfig"), true);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_AppendConfig AppendConfig;
			ZeroMemory(&AppendConfig, sizeof(AppendConfig));

			//��ȡ��Ϣ
			AppendConfig.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
			AppendConfig.BattleConfig.wKindID = m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
			AppendConfig.BattleConfig.dwConfigID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ConfigID"));
			m_GameBattleDBAide.GetValue_String(TEXT("BattleConfig"), AppendConfig.BattleConfig.szBattleConfig, CountArray(AppendConfig.BattleConfig.szBattleConfig));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPEND_CONFIG, dwContextID, dwTokenID, &AppendConfig, sizeof(AppendConfig));
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
			StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorMessage);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), TEXT("�������ݿ�����쳣�����Ժ����ԣ�"));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorMessage);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//�޸��淨
bool CDataBaseEngineSink::OnRequestModifyConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize == sizeof(DBR_GR_ModifyConfig));
	if (wDataSize != sizeof(DBR_GR_ModifyConfig)) return false;

	//��ȡ����
	auto pModifyConfig = (DBR_GR_ModifyConfig*)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");
		tagClubBattleConfig* pBattleConfig = &pModifyConfig->BattleConfig;

		LONG lReturnValue = 0;

		//�޸��淨
		if (pModifyConfig->cbModifyKind == CLUB_BC_MK_BATTLE_CONFIG)
		{
			//�������
			m_GameBattleDBAide.ResetParameter();

			//�û���Ϣ
			m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pModifyConfig->dwUserID);
			m_GameBattleDBAide.AddParameter(TEXT("@wKindID"), pBattleConfig->wKindID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pModifyConfig->dwClubID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwConfigID"), pBattleConfig->dwConfigID);
			m_GameBattleDBAide.AddParameter(TEXT("@strBattleConfig"), pBattleConfig->szBattleConfig);

			//�������
			m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

			//ִ�нű�
			lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_ModifyBattleConfig"), true);
		}		

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_ModifyConfig ModifyConfig;
			ZeroMemory(&ModifyConfig, sizeof(ModifyConfig));

			//�޸�����
			ModifyConfig.cbModifyKind = pModifyConfig->cbModifyKind;

			//��ȡ��Ϣ
			ModifyConfig.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
			ModifyConfig.BattleConfig.wKindID = m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
			ModifyConfig.BattleConfig.dwConfigID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ConfigID"));
			m_GameBattleDBAide.GetValue_String(TEXT("BattleConfig"), ModifyConfig.BattleConfig.szBattleConfig, CountArray(ModifyConfig.BattleConfig.szBattleConfig));

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_CONFIG, dwContextID, dwTokenID, &ModifyConfig, sizeof(ModifyConfig));
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
			StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorMessage);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//���ñ���
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), TEXT("�������ݿ�����쳣�����Ժ����ԣ�"));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorMessage);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//ɾ���淨
bool CDataBaseEngineSink::OnRequestDeleteConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//����У��
	ASSERT(wDataSize == sizeof(DBR_GR_DeleteConfig));
	if (wDataSize != sizeof(DBR_GR_DeleteConfig)) return false;

	//��ȡ����
	auto pDeleteConfig = (DBR_GR_DeleteConfig*)pData;

	try
	{
		//��������
		WCHAR szErrorDescribe[128] = TEXT("");

		//�������
		m_GameBattleDBAide.ResetParameter();

		//�û���Ϣ
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pDeleteConfig->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@wKindID"), pDeleteConfig->wKindID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pDeleteConfig->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwConfigID"), pDeleteConfig->dwConfigID);

		//�������
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//ִ�нű�
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_DeleteBattleConfig"), false);

		//ִ�гɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//����ṹ
			DBO_GR_DeleteConfig DeleteConfig;
			ZeroMemory(&DeleteConfig, sizeof(DeleteConfig));

			//���ñ���
			DeleteConfig.wKindID = pDeleteConfig->wKindID;
			DeleteConfig.dwClubID = pDeleteConfig->dwClubID;
			DeleteConfig.dwConfigID = pDeleteConfig->dwConfigID;

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DELETE_CONFIG, dwContextID, dwTokenID, &DeleteConfig, sizeof(DeleteConfig));
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
			StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), (LPCTSTR)(DBVarValue.bstrVal));

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorMessage);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

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
		StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), TEXT("�������ݿ�����쳣�����Ժ����ԣ�"));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorMessage);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//��ѯ��Ա
bool CDataBaseEngineSink::OnRequestQueryMember(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	try
	{
		//��ȡ����
		DBR_GR_QueryMember* pQueryMember = (DBR_GR_QueryMember*)pData;

		//�����ʶ�б�
		CString strMemberIDList = TEXT("[");

		for (int i = 0; i < pQueryMember->wMemberCount; i++)
		{
			strMemberIDList.AppendFormat(TEXT("%d"), pQueryMember->dwMemberID[i]);
			if (i < pQueryMember->wMemberCount - 1)
			{
				strMemberIDList.Append(TEXT(","));
			}
		}

		//��ӻ�����		
		strMemberIDList.Append(TEXT("]"));

		//�������
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pQueryMember->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@strMemberID"), strMemberIDList);

		//��ѯ��Ա
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_QueryMemberInfo"), true);

		//���سɹ�
		if (lReturnValue == DB_SUCCESS)
		{
			//��ȡ�Ƹ�
			while (m_GameBattleDBModule->IsRecordsetEnd() == false)
			{
				//��������
				DBO_GR_UpdateMember UpdateMember;
				ZeroMemory(&UpdateMember, sizeof(UpdateMember));

				//��������				
				UpdateMember.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
				UpdateMember.ClubMember.dwMemberID = m_GameBattleDBAide.GetValue_DWORD(TEXT("MemberID"));
				UpdateMember.ClubMember.cbMemberType = m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberType"));
				UpdateMember.ClubMember.cbMemberStatus= m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberStatus"));
				UpdateMember.ClubMember.dwManageRight = m_GameBattleDBAide.GetValue_DWORD(TEXT("ManageRight"));
				UpdateMember.ClubMember.lClubScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("ClubScore"));
				UpdateMember.ClubMember.lRevenueCount = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("RevenueCount"));
				UpdateMember.ClubMember.dwCreateCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreateCount"));
				UpdateMember.ClubMember.dwBattleCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("BattleCount"));
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), UpdateMember.ClubMember.JoinDateTime);
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("LastBattleTime"), UpdateMember.ClubMember.LastBattleTime);

				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_MEMBER, dwContextID, dwTokenID, &UpdateMember, sizeof(UpdateMember));

				//�ƶ���¼
				m_GameBattleDBModule->MoveToNext();
			}			
		}	

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//�������
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//���³�Ա
bool CDataBaseEngineSink::OnRequestUpdateMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��ȡ����
		DBR_GR_UpdateMember * pUpdateMember=(DBR_GR_UpdateMember *)pData;

		//����Լս
		if (pUpdateMember->cbUpdateKind == UPMEMBER_KIND_BATTLE)
		{
			//�������
			m_GameBattleDBAide.ResetParameter();
			m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pUpdateMember->dwClubID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pUpdateMember->dwMemberID);
			m_GameBattleDBAide.AddParameter(TEXT("@wIncCreateCount"), pUpdateMember->wIncCreateCount);
			m_GameBattleDBAide.AddParameter(TEXT("@wIncBattleCount"), pUpdateMember->wIncBattleCount);
			LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_UpdateMemberBattleCount"), true);

			//���سɹ�
			if (lReturnValue == DB_SUCCESS && m_GameBattleDBModule->IsRecordsetEnd() == false)
			{
				//��������
				DBO_GR_UpdateMember UpdateMember;
				ZeroMemory(&UpdateMember, sizeof(UpdateMember));

				//��������
				UpdateMember.cbUpdateKind = pUpdateMember->cbUpdateKind;
				UpdateMember.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
				UpdateMember.ClubMember.dwMemberID = m_GameBattleDBAide.GetValue_DWORD(TEXT("MemberID"));
				UpdateMember.ClubMember.dwCreateCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreateCount"));
				UpdateMember.ClubMember.dwBattleCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("BattleCount"));
				UpdateMember.ClubMember.lClubScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("ClubScore"));
				UpdateMember.ClubMember.lRevenueCount = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("RevenueCount"));
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), UpdateMember.ClubMember.JoinDateTime);
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("LastBattleTime"), UpdateMember.ClubMember.LastBattleTime);

				//��������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_MEMBER, dwContextID, dwTokenID, &UpdateMember, sizeof(UpdateMember));
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//����Ӯ�� 
bool CDataBaseEngineSink::OnRequestSetWinnerScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	try
	{
		//��ȡ����
		auto  pSetWinnerScore = (DBR_GR_SetWinnerScore*)pData;

		//�������
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pSetWinnerScore->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pSetWinnerScore->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@wKindID"), pSetWinnerScore->wKindID);
		m_GameBattleDBAide.AddParameter(TEXT("@lMinWinnerScore"), pSetWinnerScore->lMinWinnerScore);

		//�������
		WCHAR szErrorDescribe[128] = TEXT("");
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_SetWinnerScore"), true);

		//���سɹ�
		if (lReturnValue == DB_SUCCESS && m_GameBattleDBModule->IsRecordsetEnd() == false)
		{
			//��������
			DBO_GR_KindSettingModify KindSettingModify;
			ZeroMemory(&KindSettingModify, sizeof(KindSettingModify));

			//�����޸�
			KindSettingModify.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
			KindSettingModify.KindSettingItem.wKindID = m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
			KindSettingModify.KindSettingItem.lMinWinnerScore= m_GameBattleDBAide.GetValue_LONGLONG(TEXT("MinWinnerScore"));			

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_SETTING, dwContextID, dwTokenID, &KindSettingModify, sizeof(KindSettingModify));
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
			if (DBVarValue.bstrVal != NULL)
			{
				StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), (LPCTSTR)(DBVarValue.bstrVal));
			}

			//��������
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorMessage);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

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
		StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), TEXT("�������ݿ�����쳣�����Ժ����ԣ�"));

		//��������
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorMessage);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);

		return false;
	}

	return true;
}

//�����б�
bool CDataBaseEngineSink::OnRequestLoadClubList(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	try
	{
		//��ȡ����
		DBR_GR_LoadClubList * pLoadClubList=(DBR_GR_LoadClubList *)pData;

		//�������
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubStartID"),pLoadClubList->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubEndID"),pLoadClubList->dwClubID);
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_QueryClubList"),true);

		//���سɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//��������
			WORD wPacketSize = 0;
			BYTE cbBuffer[MAX_ASYNC_DATA] = { 0 };

			//��������
			tagClubItem * pClubInfoItem=NULL;
			tagClubRules * pClubRules = NULL;
			tagClubNotice * pClubNotice = NULL;
			tagClubProperty * pClubProperty=NULL;			

			//��������
			while (m_GameBattleDBModule->IsRecordsetEnd()==false)
			{
				//������Ϣ
				if ((wPacketSize+sizeof(tagClubItem))>sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_LIST,dwContextID,dwTokenID,cbBuffer,wPacketSize);
					wPacketSize=0;
				}

				//��ȡ����
				pClubInfoItem=(tagClubItem *)(cbBuffer+wPacketSize);

				//��ȡ����
				pClubRules=&pClubInfoItem->ClubRules;
				pClubNotice=&pClubInfoItem->ClubNotice;
				pClubProperty=&pClubInfoItem->ClubProperty;							

				//�������
				pClubRules->cbPayType=m_GameBattleDBAide.GetValue_BYTE(TEXT("PayType"));	
				pClubRules->cbBattleMask=m_GameBattleDBAide.GetValue_BYTE(TEXT("BattleMask"));	
				pClubRules->cbScoreKind = m_GameBattleDBAide.GetValue_BYTE(TEXT("ScoreKind"));
				pClubRules->cbCurrencyKind=m_GameBattleDBAide.GetValue_BYTE(TEXT("CurrencyKind"));					
				pClubRules->wSwitchOption = m_GameBattleDBAide.GetValue_WORD(TEXT("SwitchOption"));		

				//���繫��
				pClubNotice->cbPublishMode = m_GameBattleDBAide.GetValue_BYTE(TEXT("PublishMode"));
				pClubNotice->cbPublishState = m_GameBattleDBAide.GetValue_BYTE(TEXT("PublishState"));
				m_GameBattleDBAide.GetValue_String(TEXT("NoticeContent"), pClubNotice->szNoticeContent, CountArray(pClubNotice->szNoticeContent));

				//��������
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
				
				//����λ��
				wPacketSize+=sizeof(tagClubItem);

				//�ƶ���¼
				m_GameBattleDBModule->MoveToNext();
			}

			//����ʣ��
			if (wPacketSize>0) 
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_LIST,dwContextID,dwTokenID,cbBuffer,wPacketSize);
			}

			//�б����
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_LIST_FINISH,dwContextID,dwTokenID,NULL,0);
		}	

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//�����б�
bool CDataBaseEngineSink::OnRequestLoadClubItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//��ȡ����
	DBR_GR_LoadClubItem* pLoadClubItem = (DBR_GR_LoadClubItem*)pData;

	try
	{	
		//�������
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubStartID"),pLoadClubItem->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubEndID"),pLoadClubItem->dwClubID);
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_QueryClubList"),true);

		//���سɹ�
		if (lReturnValue==DB_SUCCESS && m_GameBattleDBModule->IsRecordsetEnd()==false)
		{
			//��������
			tagClubItem ClubItem;
			ZeroMemory(&ClubItem,sizeof(ClubItem));

			//��������			
			tagClubRules * pClubRules = &ClubItem.ClubRules;			
			tagClubNotice * pClubNotice = &ClubItem.ClubNotice;
			tagClubProperty * pClubProperty=&ClubItem.ClubProperty;						

			//�������
			pClubRules->cbPayType = m_GameBattleDBAide.GetValue_BYTE(TEXT("PayType"));
			pClubRules->cbCurrencyKind = m_GameBattleDBAide.GetValue_BYTE(TEXT("CurrencyKind"));
			pClubRules->cbScoreKind = m_GameBattleDBAide.GetValue_BYTE(TEXT("ScoreKind"));
			pClubRules->cbBattleMask = m_GameBattleDBAide.GetValue_BYTE(TEXT("BattleMask"));			
			pClubRules->wSwitchOption = m_GameBattleDBAide.GetValue_WORD(TEXT("SwitchOption"));				

			//���繫��
			pClubNotice->cbPublishMode = m_GameBattleDBAide.GetValue_BYTE(TEXT("PublishMode"));
			pClubNotice->cbPublishState = m_GameBattleDBAide.GetValue_BYTE(TEXT("PublishState"));
			m_GameBattleDBAide.GetValue_String(TEXT("NoticeContent"), pClubNotice->szNoticeContent, CountArray(pClubNotice->szNoticeContent));

			//��������
			pClubProperty->dwClubID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));				
			pClubProperty->dwCreaterID=m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterID"));				
			pClubProperty->dwCreaterGameID=m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterGameID"));			
			pClubProperty->wClubKind=m_GameBattleDBAide.GetValue_WORD(TEXT("ClubKind"));
			pClubProperty->wClubIconID = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubIconID"));
			pClubProperty->dwPayrollerID = m_GameBattleDBAide.GetValue_DWORD(TEXT("PayrollerID"));
			pClubProperty->wMemberCount=m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));
			pClubProperty->wMaxMemberCount=m_GameBattleDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
			m_GameBattleDBAide.GetValue_SystemTime(TEXT("CreateDateTime"),pClubProperty->CreateDateTime);
			m_GameBattleDBAide.GetValue_String(TEXT("ClubName"),pClubProperty->szClubName,CountArray(pClubProperty->szClubName));
			m_GameBattleDBAide.GetValue_String(TEXT("ClubIntroduce"),pClubProperty->szClubIntroduce,CountArray(pClubProperty->szClubIntroduce));									

			//��������
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_ITEM,dwContextID,dwTokenID,&ClubItem,sizeof(ClubItem));

			//��������				
			DBR_GR_LoadClubMember LoadClubMember;
			ZeroMemory(&LoadClubMember,sizeof(LoadClubMember));

			//����ṹ
			LoadClubMember.dwClubID=ClubItem.ClubProperty.dwClubID;

			//���س�Ա
			OnRequestLoadClubMember(dwContextID,dwTokenID,&LoadClubMember,sizeof(LoadClubMember));		

			//����ṹ
			DBR_GR_LoadBattleConfig LoadBattleConfig;
			LoadBattleConfig.dwClubID=ClubItem.ClubProperty.dwClubID;

			//�����淨
			OnRequestLoadBattleConfig(dwContextID, dwTokenID, &LoadBattleConfig, sizeof(LoadBattleConfig));

			//����ṹ
			DBR_GR_LoadApplyMessage LoadApplyMessage;
			LoadApplyMessage.dwClubID=ClubItem.ClubProperty.dwClubID;

			//���س�Ա
			OnRequestLoadApplyMessage(dwContextID, dwTokenID, &LoadApplyMessage, sizeof(LoadApplyMessage));

			//�������
			DBO_GR_ClubItemFinish ClubItemFinish = {};

			//���ñ���
			ClubItemFinish.dwClubID = pLoadClubItem->dwClubID;
			ClubItemFinish.bActiveRoute = pLoadClubItem->bActiveRoute;

			//�б����			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_ITEM_FINISH,dwContextID,dwTokenID,&ClubItemFinish,sizeof(ClubItemFinish));
		}		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//�������
		DBO_GR_ClubItemFinish ClubItemFinish = {};

		//���ñ���
		ClubItemFinish.dwClubID = pLoadClubItem->dwClubID;
		ClubItemFinish.bActiveRoute = pLoadClubItem->bActiveRoute;

		//�б����			
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_ITEM_FINISH, dwContextID, dwTokenID, &ClubItemFinish, sizeof(ClubItemFinish));

		return false;
	}

	return true;
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

