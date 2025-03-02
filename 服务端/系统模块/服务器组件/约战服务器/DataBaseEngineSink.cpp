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
		tagDataBaseParameter * pGameBattleDBParameter = &m_pInitParameter->m_GameBattleDBParameter;

		//��������
		m_GameBattleDBModule->SetConnectionInfo(pGameBattleDBParameter->dwDataBaseAddr, pGameBattleDBParameter->wDataBasePort,
		CW2CT(pGameBattleDBParameter->szDataBaseName),CW2CT(pGameBattleDBParameter->szDataBaseUser),CW2CT(pGameBattleDBParameter->szDataBasePass));
	
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
	if (m_GameBattleDBModule.GetInterface() != NULL)
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
	case DBR_GR_LOAD_BATTLE_OPTION:		//����ѡ��
		{
			return OnRequestLoadBattleOption(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
	}

	return false;
}

//��������
bool CDataBaseEngineSink::OnRequestLoadBattleOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//�������
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@wKindID"),0);				
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadBattleBaseOption"),true);

		//���سɹ�
		if (lReturnValue==DB_SUCCESS)
		{
			//��������
			WORD wPacketSize=0;
			BYTE cbBuffer[MAX_ASYNC_DATA];

			//��������						
			tagBattleBaseOptionEx * pBattleBaseOption=NULL;

			//��������
			while (m_GameBattleDBModule->IsRecordsetEnd()==false)
			{
				//������Ϣ
				if ((wPacketSize+sizeof(tagBattleBaseOption))>sizeof(cbBuffer))
				{
					break;
				}

				//��ȡ��Ϣ
				pBattleBaseOption=(tagBattleBaseOptionEx *)(cbBuffer+wPacketSize);

				//��ȡ��Ϣ
				pBattleBaseOption->wOptionID = m_GameBattleDBAide.GetValue_WORD(TEXT("OptionID"));
				
				//��ȡ����
				pBattleBaseOption->BattleOption.wKindID = m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
				pBattleBaseOption->BattleOption.cbScoreKind = m_GameBattleDBAide.GetValue_BYTE(TEXT("ScoreKind"));
				pBattleBaseOption->BattleOption.wPlayerCount = m_GameBattleDBAide.GetValue_WORD(TEXT("PlayerCount"));
				pBattleBaseOption->BattleOption.cbEmployScene = m_GameBattleDBAide.GetValue_BYTE(TEXT("EmployScene"));
				pBattleBaseOption->BattleOption.wOptionMask = m_GameBattleDBAide.GetValue_WORD(TEXT("OptionMask"));
				pBattleBaseOption->BattleOption.cbItemCount = m_GameBattleDBAide.GetValue_BYTE(TEXT("ItemCount"));
				pBattleBaseOption->BattleOption.wSuportPayType = m_GameBattleDBAide.GetValue_WORD(TEXT("SuportPayType"));
				pBattleBaseOption->BattleOption.wWinnerCount = m_GameBattleDBAide.GetValue_WORD(TEXT("WinnerCount"));
				pBattleBaseOption->BattleOption.lWinnerRevenue = m_GameBattleDBAide.GetValue_LONG(TEXT("WinnerRevenue"));
				pBattleBaseOption->BattleOption.lMinBaseScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("MinBaseScore"));
				pBattleBaseOption->BattleOption.lMaxBaseScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("MaxBaseScore"));
				pBattleBaseOption->BattleOption.lMinEntryCost = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("MinEntryCost"));
				pBattleBaseOption->BattleOption.lMaxEntryCost = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("MaxEntryCost"));				
	

				//����λ��
				wPacketSize+=sizeof(tagBattleBaseOptionEx);

				//�ƶ���¼
				m_GameBattleDBModule->MoveToNext();
			}	

			//��������			
			WORD wOptionCount = wPacketSize / sizeof(tagBattleBaseOptionEx);

			//���ñ���
			WORD wIndex = 0;
			auto pBattleBaseOptionEx = (tagBattleBaseOptionEx*)cbBuffer;

			for (WORD i = 0; i < wOptionCount; i++)
			{
				//�������
				m_GameBattleDBAide.ResetParameter();
				m_GameBattleDBAide.AddParameter(TEXT("@wOptionID"), pBattleBaseOptionEx->wOptionID);
				m_GameBattleDBAide.AddParameter(TEXT("@wItemCount"), pBattleBaseOptionEx->BattleOption.cbItemCount);
				LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadBattleOptionItem"), true);

				//���سɹ�
				if (lReturnValue == DB_SUCCESS)
				{
					//��������
					while (m_GameBattleDBModule->IsRecordsetEnd() == false)
					{
						//��ȡ����
						wIndex = m_GameBattleDBAide.GetValue_WORD(TEXT("ItemIndex"));
						if (wIndex >= CountArray(pBattleBaseOptionEx->BattleOption.OptionItem)) break;

						//��ȡ����
						pBattleBaseOptionEx->BattleOption.OptionItem[wIndex].wPlayCount = m_GameBattleDBAide.GetValue_WORD(TEXT("PlayCount"));
						pBattleBaseOptionEx->BattleOption.OptionItem[wIndex].lRoomCard = m_GameBattleDBAide.GetValue_LONG(TEXT("RoomCard"));
						pBattleBaseOptionEx->BattleOption.OptionItem[wIndex].lRoomCardAA = m_GameBattleDBAide.GetValue_LONG(TEXT("RoomCardAA"));						

						//�ƶ���¼
						m_GameBattleDBModule->MoveToNext();
					}
				}

				pBattleBaseOptionEx++;
			}

			//��������
			if (wOptionCount > 0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_BASE_OPTION, dwContextID, dwTokenID, cbBuffer, wPacketSize);
			}			

			//�������
			m_GameBattleDBAide.ResetParameter();
			m_GameBattleDBAide.AddParameter(TEXT("@wKindID"), 0);
			lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadBattleGameOption"), true);

			//���سɹ�
			if (lReturnValue == DB_SUCCESS)
			{
				//���ñ���
				wPacketSize = 0;

				//��������				
				tagBattleGameOptionEx * pBattleGameOption = NULL;

				//��Ϸ����
				while (m_GameBattleDBModule->IsRecordsetEnd() == false)
				{
					//������Ϣ
					if ((wPacketSize + sizeof(tagBattleGameOptionEx)) > sizeof(cbBuffer))
					{
						m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_GAME_OPTION, dwContextID, dwTokenID, cbBuffer, wPacketSize);
						wPacketSize = 0;
					}

					//��ȡ��Ϣ
					pBattleGameOption = (tagBattleGameOptionEx *)(cbBuffer + wPacketSize);

					//��ȡ����
					pBattleGameOption->BattleGameOption.wKindID = m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
					pBattleGameOption->BattleGameOption.cbEmployScene= m_GameBattleDBAide.GetValue_BYTE(TEXT("EmployScene"));
					m_GameBattleDBAide.GetValue_String(TEXT("CustomOption"), pBattleGameOption->BattleGameOption.szOptionJson, CountArray(pBattleGameOption->BattleGameOption.szOptionJson));

					//����λ��
					wPacketSize += sizeof(tagBattleGameOptionEx);

					//�ƶ���¼
					m_GameBattleDBModule->MoveToNext();
				}

				//����ʣ��
				if (wPacketSize > 0)
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_GAME_OPTION, dwContextID, dwTokenID, cbBuffer, wPacketSize);
				}
			}	


			//��������
			tagBattleGlobalOption BattleGlobalOption;
			ZeroMemory(&BattleGlobalOption,sizeof(BattleGlobalOption));

			//�������
			m_GameBattleDBAide.ResetParameter();
			
			//ִ�в�ѯ
			lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadBattleOption"), true);

			//ִ�гɹ�
			if (lReturnValue == DB_SUCCESS)
			{
				//��ݿ���������
				BattleGlobalOption.wClubMaxTableCount = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubMaxTableCount"));
				BattleGlobalOption.wNormalMaxTableCount = m_GameBattleDBAide.GetValue_WORD(TEXT("NormalMaxTableCount"));	

				//Ͷ������
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_GLOBAL_OPTION, dwContextID, dwTokenID, &BattleGlobalOption, sizeof(BattleGlobalOption));
			}			

			//���֪ͨ
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_OPTION_FINISH,dwContextID, dwTokenID,NULL,0);
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

//�����Ϣ
void CDataBaseEngineSink::TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException)
{
	//������Ϣ
	char szDesc[512]="\0";
	_snprintf_s(szDesc,511,"%s(%d):%s",pFile,nLine,pFunction);
	CString strDescribe;
#ifdef _UNICODE
	TCHAR szWDesc[512]=TEXT("\0");
	int nLen=(int)strlen(szDesc);
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

