#include "StdAfx.h"
#include "MatchInfoManager.h"
#include "ModuleDBParameter.h"
#include "ModuleInfoManager.h"

//////////////////////////////////////////////////////////////////////////
//���캯��
CMatchInfoBuffer::CMatchInfoBuffer()
{
}

//��������
CMatchInfoBuffer::~CMatchInfoBuffer()
{
	//��������
	tagMatchOption * pMatchOption=NULL;

	//ɾ������
	m_MatchOptionBuffer.Append(m_MatchOptionArray);
	for (INT_PTR i=0;i<m_MatchOptionBuffer.GetCount();i++)
	{
		pMatchOption=m_MatchOptionBuffer[i];
		SafeDelete(pMatchOption);
	}

	//ɾ������
	m_MatchOptionArray.RemoveAll();
	m_MatchOptionBuffer.RemoveAll();

	return;
}

//��������
bool CMatchInfoBuffer::ResetMatchOption()
{
	//��Ӷ���
	m_MatchOptionBuffer.Append(m_MatchOptionArray);

	//ɾ��Ԫ��
	m_MatchOptionArray.RemoveAll();

	return true;
}

//ɾ������
bool CMatchInfoBuffer::DeleteMatchOption(DWORD dwMatchID)
{
	//��������
	tagMatchOption * pMatchOption=NULL;

	//���Ҷ���
	for(INT_PTR i=0;i<m_MatchOptionArray.GetCount();i++)
	{
		pMatchOption=m_MatchOptionArray[i];
		if(pMatchOption->dwMatchID==dwMatchID)
		{
			//ɾ������
			m_MatchOptionArray.RemoveAt(i);
			m_MatchOptionBuffer.Add(pMatchOption);

			//���ñ���
			ZeroMemory(pMatchOption,sizeof(tagMatchOption));

			break;
		}
	}	

	return true;
}

//��������
tagMatchOption * CMatchInfoBuffer::InsertMatchOption(tagMatchOption * pMatchOption)
{
	//Ч�����
	ASSERT(pMatchOption!=NULL);
	if (pMatchOption==NULL) return NULL;

	//�����ִ�
	tagMatchOption * pGameMatchInsert=SearchMatchOption(pMatchOption->dwMatchID);

	//�����ж�
	if (pGameMatchInsert==NULL)
	{
		//��������
		pGameMatchInsert=CreateMatchOption();

		//����ж�
		if (pGameMatchInsert==NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		//�������
		m_MatchOptionArray.Add(pGameMatchInsert);
	}

	//��������	
	CopyMemory(pGameMatchInsert,pMatchOption,sizeof(tagMatchOption));

	return pGameMatchInsert;
}

//��ȡ��Ŀ
DWORD CMatchInfoBuffer::GetMatchOptionCount()
{
	return (DWORD)(m_MatchOptionArray.GetCount());
}

//��������
tagMatchOption * CMatchInfoBuffer::SearchMatchOption(DWORD dwMatchID)
{
	//��������
	tagMatchOption * pMatchOption=NULL;

	//���Ҷ���
	for(INT_PTR i=0;i<m_MatchOptionArray.GetCount();i++)
	{
		pMatchOption=m_MatchOptionArray[i];
		if(pMatchOption->dwMatchID==dwMatchID)
		{
			return pMatchOption;
		}
	}

	return NULL;
}

//��������
tagMatchOption * CMatchInfoBuffer::CreateMatchOption()
{
	//��������
	tagMatchOption * pMatchOption=NULL;

	//��������
	try
	{
		INT_PTR nArrayCount=m_MatchOptionBuffer.GetCount();
		if (nArrayCount>0)
		{
			pMatchOption=m_MatchOptionBuffer[nArrayCount-1];
			m_MatchOptionBuffer.RemoveAt(nArrayCount-1);
		}
		else
		{
			pMatchOption=new tagMatchOption;
			if (pMatchOption==NULL) return NULL;
		}
	}
	catch (...) 
	{ 
		return NULL; 
	}

	//���ñ���
	ZeroMemory(pMatchOption,sizeof(tagMatchOption));

	return pMatchOption;
}

//////////////////////////////////////////////////////////////////////////
//���캯��
CRewardInfoBuffer::CRewardInfoBuffer()
{
}

//��������
CRewardInfoBuffer::~CRewardInfoBuffer()
{
	//��������
	tagMatchRewardItem * pMatchRewardItem=NULL;

	//ɾ������
	m_MatchRewardItemBuffer.Append(m_MatchRewardItemArray);
	for (INT_PTR i=0;i<m_MatchRewardItemBuffer.GetCount();i++)
	{
		pMatchRewardItem=m_MatchRewardItemBuffer[i];
		SafeDelete(pMatchRewardItem);
	}

	//ɾ������
	m_MatchRewardItemArray.RemoveAll();
	m_MatchRewardItemBuffer.RemoveAll();

	return;
}

//��������
bool CRewardInfoBuffer::ResetMatchRewardItem()
{
	//��Ӷ���
	m_MatchRewardItemBuffer.Append(m_MatchRewardItemArray);

	//ɾ��Ԫ��
	m_MatchRewardItemArray.RemoveAll();

	return true;
}

//ɾ������
bool CRewardInfoBuffer::DeleteMatchRewardItem(WORD wRankID)
{
	//��������
	tagMatchRewardItem * pMatchRewardItem=NULL;

	//���Ҷ���
	for (INT_PTR i=0;i<m_MatchRewardItemArray.GetCount();i++)
	{
		pMatchRewardItem=m_MatchRewardItemArray[i];
		if (pMatchRewardItem->wStartRankID==wRankID)
		{
			//ɾ������
			m_MatchRewardItemArray.RemoveAt(i);
			m_MatchRewardItemBuffer.Add(pMatchRewardItem);

			//���ñ���
			ZeroMemory(pMatchRewardItem,sizeof(tagMatchRewardItem));

			break;
		}
	}	

	return true;
}

//��������
tagMatchRewardItem * CRewardInfoBuffer::InsertMatchRewardItem(tagMatchRewardItem & MatchRewardItem)
{
	//�����ִ�
	tagMatchRewardItem * pMatchRewardInsert=SearchMatchReward(MatchRewardItem.wStartRankID);

	//�����ж�
	if (pMatchRewardInsert==NULL)
	{
		//��������
		pMatchRewardInsert=CreateMatchRewardItem();

		//����ж�
		if (pMatchRewardInsert==NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		//�������
		m_MatchRewardItemArray.Add(pMatchRewardInsert);		
	}

	//��������
	CopyMemory(pMatchRewardInsert,&MatchRewardItem,sizeof(tagMatchRewardItem));

	return pMatchRewardInsert;
}

//��ȡ��Ŀ
WORD CRewardInfoBuffer::GetMatchRewardCount()
{
	return (WORD)(m_MatchRewardItemArray.GetCount());
}
	
//��������
tagMatchRewardItem * CRewardInfoBuffer::SearchMatchReward(WORD wRankID)
{
	//��������
	tagMatchRewardItem * pMatchRewardItem=NULL;

	//���Ҷ���
	for (INT_PTR i=0;i<m_MatchRewardItemArray.GetCount();i++)
	{
		pMatchRewardItem=m_MatchRewardItemArray[i];
		if (pMatchRewardItem->wStartRankID<=wRankID && wRankID<=pMatchRewardItem->wEndRankID)
		{
			return pMatchRewardItem;
		}
	}

	return NULL;
}

//��������
tagMatchRewardItem * CRewardInfoBuffer::CreateMatchRewardItem()
{
	//��������
	tagMatchRewardItem * pMatchRewardItem=NULL;

	//��������
	try
	{
		INT_PTR nArrayCount=m_MatchRewardItemBuffer.GetCount();
		if (nArrayCount>0)
		{
			pMatchRewardItem=m_MatchRewardItemBuffer[nArrayCount-1];
			m_MatchRewardItemBuffer.RemoveAt(nArrayCount-1);
		}
		else
		{
			pMatchRewardItem=new tagMatchRewardItem;
			if (pMatchRewardItem==NULL) return NULL;
		}
	}
	catch (...) 
	{ 
		return NULL; 
	}

	//���ñ���
	ZeroMemory(pMatchRewardItem,sizeof(tagMatchRewardItem));

	return pMatchRewardItem;
}

//////////////////////////////////////////////////////////////////////////
//���캯��
CRoundInfoBuffer::CRoundInfoBuffer()
{
}

//��������
CRoundInfoBuffer::~CRoundInfoBuffer()
{
	//��������
	tagMatchRoundItem * pMatchRoundItem=NULL;

	//ɾ������
	m_MatchRoundItemBuffer.Append(m_MatchRoundItemArray);
	for (INT_PTR i=0;i<m_MatchRoundItemBuffer.GetCount();i++)
	{
		pMatchRoundItem=m_MatchRoundItemBuffer[i];
		SafeDelete(pMatchRoundItem);
	}

	//ɾ������
	m_MatchRoundItemArray.RemoveAll();
	m_MatchRoundItemBuffer.RemoveAll();

	return;
}

//��������
bool CRoundInfoBuffer::ResetMatchRoundItem()
{
	//��Ӷ���
	m_MatchRoundItemBuffer.Append(m_MatchRoundItemArray);

	//ɾ��Ԫ��
	m_MatchRoundItemArray.RemoveAll();

	return true;
}

//ɾ������
bool CRoundInfoBuffer::DeleteMatchRoundItem(WORD wRoundID)
{
	//��������
	tagMatchRoundItem * pMatchRoundItem=NULL;

	//���Ҷ���
	for (INT_PTR i=0;i<m_MatchRoundItemArray.GetCount();i++)
	{
		pMatchRoundItem=m_MatchRoundItemArray[i];
		if (pMatchRoundItem->wRoundID==wRoundID)
		{
			//ɾ������
			m_MatchRoundItemArray.RemoveAt(i);
			m_MatchRoundItemBuffer.Add(pMatchRoundItem);

			//���ñ���
			ZeroMemory(pMatchRoundItem,sizeof(tagMatchRoundItem));

			break;
		}
	}	

	return true;
}

//��������
tagMatchRoundItem * CRoundInfoBuffer::InsertMatchRoundItem(tagMatchRoundItem & MatchRoundItem)
{
	//�����ִ�
	tagMatchRoundItem * pMatchRoundInsert=SearchMatchRound(MatchRoundItem.wRoundID);

	//�����ж�
	if (pMatchRoundInsert==NULL)
	{
		//��������
		pMatchRoundInsert=CreateMatchRoundItem();

		//����ж�
		if (pMatchRoundInsert==NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		//�������
		m_MatchRoundItemArray.Add(pMatchRoundInsert);		
	}

	//��������
	CopyMemory(pMatchRoundInsert,&MatchRoundItem,sizeof(tagMatchRoundItem));

	return pMatchRoundInsert;
}

//��ȡ��Ŀ
WORD CRoundInfoBuffer::GetMatchRoundCount()
{
	return (WORD)(m_MatchRoundItemArray.GetCount());
}
	
//��������
tagMatchRoundItem * CRoundInfoBuffer::SearchMatchRound(WORD wRoundID)
{
	//��������
	tagMatchRoundItem * pMatchRoundItem=NULL;

	//���Ҷ���
	for (INT_PTR i=0;i<m_MatchRoundItemArray.GetCount();i++)
	{
		pMatchRoundItem=m_MatchRoundItemArray[i];
		if (pMatchRoundItem->wRoundID==wRoundID)
		{
			return pMatchRoundItem;
		}
	}

	return NULL;
}

//��������
tagMatchRoundItem * CRoundInfoBuffer::CreateMatchRoundItem()
{
	//��������
	tagMatchRoundItem * pMatchRoundItem=NULL;

	//��������
	try
	{
		INT_PTR nArrayCount=m_MatchRoundItemBuffer.GetCount();
		if (nArrayCount>0)
		{
			pMatchRoundItem=m_MatchRoundItemBuffer[nArrayCount-1];
			m_MatchRoundItemBuffer.RemoveAt(nArrayCount-1);
		}
		else
		{
			pMatchRoundItem=new tagMatchRoundItem;
			if (pMatchRoundItem==NULL) return NULL;
		}
	}
	catch (...) 
	{ 
		return NULL; 
	}

	//���ñ���
	ZeroMemory(pMatchRoundItem,sizeof(tagMatchRoundItem));

	return pMatchRoundItem;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CMatchInfoManager::CMatchInfoManager()
{
}

//��������
CMatchInfoManager::~CMatchInfoManager()
{
}

//���ر���
bool CMatchInfoManager::LoadMatchOption(WORD wKindID,TCHAR szMachineID[LEN_MACHINE_ID],CMatchInfoBuffer & MatchInfoBuffer)
{
	//��������
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());

		//�������
		GameMatchDBAide.ResetParameter();
		GameMatchDBAide.AddParameter(TEXT("@wKindID"),wKindID);
		GameMatchDBAide.AddParameter(TEXT("@strServiceMachine"),szMachineID);

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		GameMatchDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//��ȡ�б�
		if (GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_LoadMatchOption"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			GameMatchDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//����б�
		MatchInfoBuffer.ResetMatchOption();

		//��ȡ�б�
		while (GameMatchDBModule->IsRecordsetEnd()==false)
		{
			//��ȡ����
			tagMatchOption GameMatchResult;
			ReadMatchOption(GameMatchDBAide,GameMatchResult);

			//�����б�
			MatchInfoBuffer.InsertMatchOption(&GameMatchResult);

			//�ƶ���¼
			GameMatchDBModule->MoveToNext();
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//ɾ������
bool CMatchInfoManager::DeleteMatchOption(DWORD dwMatchID)
{
	//��������
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());

		//�������
		GameMatchDBAide.ResetParameter();		
		GameMatchDBAide.AddParameter(TEXT("@dwMatchID"),dwMatchID);

		//��ȡ�б�
		LONG lResultCode=GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_DeleteMatchOption"),false);
		
		return lResultCode==DB_SUCCESS;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//���뷿��
bool CMatchInfoManager::InsertMatchOption(tagCreateMatchInfo * pCreateMatchInfo,tagMatchOption & GameMatchResult)
{
	//��������
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());		

		//�������
		GameMatchDBAide.ResetParameter();		
 		GameMatchDBAide.AddParameter(TEXT("@wKindID"),pCreateMatchInfo->wKindID);		
		GameMatchDBAide.AddParameter(TEXT("@dwClubID"),pCreateMatchInfo->dwClubID);		
		GameMatchDBAide.AddParameter(TEXT("@strModuleName"),pCreateMatchInfo->szModuleName);
		GameMatchDBAide.AddParameter(TEXT("@strMatchName"),pCreateMatchInfo->szMatchName);
		GameMatchDBAide.AddParameter(TEXT("@strMatchDate"),pCreateMatchInfo->szMatchDate);
		GameMatchDBAide.AddParameter(TEXT("@cbShowType"), pCreateMatchInfo->cbShowType);
		GameMatchDBAide.AddParameter(TEXT("@cbMatchType"),pCreateMatchInfo->cbMatchType);

		//��������
		GameMatchDBAide.AddParameter(TEXT("@wFeeMoneyID"),pCreateMatchInfo->wFeeMoneyID);			
		GameMatchDBAide.AddParameter(TEXT("@lFeeAmount"),pCreateMatchInfo->lFeeAmount);
		GameMatchDBAide.AddParameter(TEXT("@wUseTicketCount"), pCreateMatchInfo->wUseTicketCount);
		GameMatchDBAide.AddParameter(TEXT("@wBuyTicketCount"), pCreateMatchInfo->wBuyTicketCount);
		GameMatchDBAide.AddParameter(TEXT("@lBuyTicketOption"), pCreateMatchInfo->wBuyTicketOption);
		GameMatchDBAide.AddParameter(TEXT("@lBuyTicketAmount"), pCreateMatchInfo->lBuyTicketAmount);
		GameMatchDBAide.AddParameter(TEXT("@wBuyTicketMoneyID"), pCreateMatchInfo->wBuyTicketMoneyID);
		GameMatchDBAide.AddParameter(TEXT("@wSignupCount"), pCreateMatchInfo->wSignupCount);
		GameMatchDBAide.AddParameter(TEXT("@cbSignupMode"),pCreateMatchInfo->cbSignupMode);		
		GameMatchDBAide.AddParameter(TEXT("@cbJoinCondition"),pCreateMatchInfo->cbJoinCondition);
		GameMatchDBAide.AddParameter(TEXT("@cbMemberOrder"),pCreateMatchInfo->cbMemberOrder);
		GameMatchDBAide.AddParameter(TEXT("@lExperience"),pCreateMatchInfo->lExperience);	

		//��������
		GameMatchDBAide.AddParameter(TEXT("@cbMatchDistribute"), pCreateMatchInfo->cbMatchDistribute, sizeof(pCreateMatchInfo->cbMatchDistribute));

		//��������
		GameMatchDBAide.AddParameter(TEXT("@wRewardCount"),pCreateMatchInfo->wRewardCount);
		GameMatchDBAide.AddParameter(TEXT("@cbMatchReward"),(LPBYTE)pCreateMatchInfo->MatchRewardItem,pCreateMatchInfo->wRewardCount*sizeof(pCreateMatchInfo->MatchRewardItem[0]));

		//�����ִ�
		GameMatchDBAide.AddParameter(TEXT("@wRoundCount"),pCreateMatchInfo->wRoundCount);
		GameMatchDBAide.AddParameter(TEXT("@cbMatchRound"),(LPBYTE)pCreateMatchInfo->MatchRoundItem,__max(pCreateMatchInfo->wRoundCount*sizeof(pCreateMatchInfo->MatchRoundItem[0]),1));

		//��չ����
		GameMatchDBAide.AddParameter(TEXT("@cbMatchRule"),pCreateMatchInfo->cbMatchRule,sizeof(pCreateMatchInfo->cbMatchRule));
		GameMatchDBAide.AddParameter(TEXT("@strServiceMachine"),pCreateMatchInfo->szServiceMachine);	

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		GameMatchDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		if (GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_InsertMatchOption"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			GameMatchDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//��ȡ����
		ReadMatchOption(GameMatchDBAide,GameMatchResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//�޸ķ���
bool CMatchInfoManager::ModifyMatchOption(tagCreateMatchInfo * pCreateMatchInfo,tagMatchOption & GameMatchResult)
{
	//��������
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());

		//��������
		GameMatchDBAide.ResetParameter();
		GameMatchDBAide.AddParameter(TEXT("@wKindID"),pCreateMatchInfo->wKindID);	
		GameMatchDBAide.AddParameter(TEXT("@dwMatchID"),pCreateMatchInfo->dwMatchID);	
		GameMatchDBAide.AddParameter(TEXT("@dwClubID"),pCreateMatchInfo->dwClubID);
		GameMatchDBAide.AddParameter(TEXT("@strModuleName"),pCreateMatchInfo->szModuleName);
		GameMatchDBAide.AddParameter(TEXT("@strMatchName"),pCreateMatchInfo->szMatchName);
		GameMatchDBAide.AddParameter(TEXT("@strMatchDate"),pCreateMatchInfo->szMatchDate);
		GameMatchDBAide.AddParameter(TEXT("@cbShowType"), pCreateMatchInfo->cbShowType);
		GameMatchDBAide.AddParameter(TEXT("@cbMatchType"),pCreateMatchInfo->cbMatchType);

		//��������
		GameMatchDBAide.AddParameter(TEXT("@wFeeMoneyID"), pCreateMatchInfo->wFeeMoneyID);
		GameMatchDBAide.AddParameter(TEXT("@lFeeAmount"), pCreateMatchInfo->lFeeAmount);
		GameMatchDBAide.AddParameter(TEXT("@wUseTicketCount"), pCreateMatchInfo->wUseTicketCount);
		GameMatchDBAide.AddParameter(TEXT("@wBuyTicketCount"), pCreateMatchInfo->wBuyTicketCount);
		GameMatchDBAide.AddParameter(TEXT("@wBuyTicketOption"), pCreateMatchInfo->wBuyTicketOption);
		GameMatchDBAide.AddParameter(TEXT("@lBuyTicketAmount"), pCreateMatchInfo->lBuyTicketAmount);
		GameMatchDBAide.AddParameter(TEXT("@wBuyTicketMoneyID"), pCreateMatchInfo->wBuyTicketMoneyID);
		GameMatchDBAide.AddParameter(TEXT("@wSignupCount"), pCreateMatchInfo->wSignupCount);
		GameMatchDBAide.AddParameter(TEXT("@cbSignupMode"),pCreateMatchInfo->cbSignupMode);		
		GameMatchDBAide.AddParameter(TEXT("@cbJoinCondition"),pCreateMatchInfo->cbJoinCondition);
		GameMatchDBAide.AddParameter(TEXT("@cbMemberOrder"),pCreateMatchInfo->cbMemberOrder);
		GameMatchDBAide.AddParameter(TEXT("@lExperience"),pCreateMatchInfo->lExperience);

		//��������
		GameMatchDBAide.AddParameter(TEXT("@cbMatchDistribute"), pCreateMatchInfo->cbMatchDistribute, sizeof(pCreateMatchInfo->cbMatchDistribute));

		//��������
		GameMatchDBAide.AddParameter(TEXT("@wRewardCount"),pCreateMatchInfo->wRewardCount);
		GameMatchDBAide.AddParameter(TEXT("@cbMatchReward"),(LPBYTE)&pCreateMatchInfo->MatchRewardItem,pCreateMatchInfo->wRewardCount*sizeof(pCreateMatchInfo->MatchRewardItem[0]));	

		//�����ִ�
		GameMatchDBAide.AddParameter(TEXT("@wRoundCount"),pCreateMatchInfo->wRoundCount);
		GameMatchDBAide.AddParameter(TEXT("@cbMatchRound"),(LPBYTE)&pCreateMatchInfo->MatchRoundItem, __max(pCreateMatchInfo->wRoundCount*sizeof(pCreateMatchInfo->MatchRoundItem[0]),1));

		//��������
		GameMatchDBAide.AddParameter(TEXT("@cbMatchRule"),pCreateMatchInfo->cbMatchRule,sizeof(pCreateMatchInfo->cbMatchRule));		

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		GameMatchDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ�нű�
		if (GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_ModifyMatchOption"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			GameMatchDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//��ȡ����
		ReadMatchOption(GameMatchDBAide,GameMatchResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}



//��������
bool CMatchInfoManager::ConnectPlatformDB(CDataBaseHelper & PlatformDBModule)
{
	//��ȡ����
	CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

	//��������
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("CreateGameServer ���� PlatformDBModule ����ʧ��"),MB_ICONERROR);
		return false;
	}

	//��������
	PlatformDBModule->SetConnectionInfo(pDataBaseParameter->dwDataBaseAddr,pDataBaseParameter->wDataBasePort,
	CW2CT(pDataBaseParameter->szDataBaseName),CW2CT(pDataBaseParameter->szDataBaseUser),CW2CT(pDataBaseParameter->szDataBasePass));

	//��������
	PlatformDBModule->OpenConnection();

	return true;
}

//��������
bool CMatchInfoManager::ConnectGameMatchDB(CDataBaseHelper & GameMatchDBModule)
{
	//��ȡ����
	CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetMatchDBParameter();

	//��������
	if ((GameMatchDBModule.GetInterface()==NULL)&&(GameMatchDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("ConnectGameMatchDB ���� GameMatchDBModule ����ʧ��"),MB_ICONERROR);
		return false;
	}

	//��������
	GameMatchDBModule->SetConnectionInfo(pDataBaseParameter->dwDataBaseAddr,pDataBaseParameter->wDataBasePort,
	CW2CT(pDataBaseParameter->szDataBaseName),CW2CT(pDataBaseParameter->szDataBaseUser),CW2CT(pDataBaseParameter->szDataBasePass));

	//��������
	GameMatchDBModule->OpenConnection();

	return true;
}

//��ȡ����
bool CMatchInfoManager::ReadMatchOption(CDataBaseAide & GameMatchDBAide, tagMatchOption & GameMatchResult)
{
	//���ñ���
	ZeroMemory(&GameMatchResult,sizeof(GameMatchResult));

	//��������	
	GameMatchResult.wKindID=GameMatchDBAide.GetValue_WORD(TEXT("KindID"));	
	GameMatchResult.dwClubID=GameMatchDBAide.GetValue_WORD(TEXT("ClubID"));	
	GameMatchResult.dwMatchID=GameMatchDBAide.GetValue_DWORD(TEXT("MatchID"));	
	GameMatchResult.cbMatchStatus=GameMatchDBAide.GetValue_BYTE(TEXT("MatchStatus"));

	//������Ϣ
	GameMatchResult.cbShowType = GameMatchDBAide.GetValue_BYTE(TEXT("ShowType"));
	GameMatchResult.cbMatchType = GameMatchDBAide.GetValue_BYTE(TEXT("MatchType"));		
	GameMatchDBAide.GetValue_String(TEXT("ModuleName"),GameMatchResult.szModuleName,CountArray(GameMatchResult.szModuleName));
	GameMatchDBAide.GetValue_String(TEXT("MatchName"),GameMatchResult.szMatchName,CountArray(GameMatchResult.szMatchName));

	//������Ϣ
	GameMatchResult.wFeeMoneyID= GameMatchDBAide.GetValue_WORD(TEXT("FeeMoneyID"));	
	GameMatchResult.lFeeAmount= GameMatchDBAide.GetValue_LONGLONG(TEXT("FeeAmount"));
	GameMatchResult.wUseTicketCount = GameMatchDBAide.GetValue_WORD(TEXT("UseTicketCount"));
	GameMatchResult.wBuyTicketCount = GameMatchDBAide.GetValue_WORD(TEXT("BuyTicketCount"));
	GameMatchResult.wBuyTicketOption= GameMatchDBAide.GetValue_WORD(TEXT("BuyTicketOption"));
	GameMatchResult.wBuyTicketMoneyID = GameMatchDBAide.GetValue_WORD(TEXT("BuyTicketMoneyID"));
	GameMatchResult.lBuyTicketAmount = GameMatchDBAide.GetValue_LONGLONG(TEXT("BuyTicketAmount"));	
	GameMatchResult.wSignupCount = GameMatchDBAide.GetValue_WORD(TEXT("SignupCount"));
	GameMatchResult.cbSignupMode = GameMatchDBAide.GetValue_BYTE(TEXT("SignupMode"));	
	GameMatchResult.cbJoinCondition = GameMatchDBAide.GetValue_BYTE(TEXT("JoinCondition"));
	GameMatchResult.cbMemberOrder = GameMatchDBAide.GetValue_BYTE(TEXT("MemberOrder"));
	GameMatchResult.lExperience = GameMatchDBAide.GetValue_LONG(TEXT("Experience"));
	
	//��������	
	GameMatchResult.wRewardCount=GameMatchDBAide.GetValue_WORD(TEXT("RewardCount"));
	GameMatchDBAide.GetValue_Binary(TEXT("MatchReward"),(LPBYTE)&GameMatchResult.MatchRewardItem,sizeof(GameMatchResult.MatchRewardItem));	

	//�����ִ�
	GameMatchResult.wRoundCount=GameMatchDBAide.GetValue_WORD(TEXT("RoundCount"));
	GameMatchDBAide.GetValue_Binary(TEXT("MatchRound"),(LPBYTE)&GameMatchResult.MatchRoundItem,sizeof(GameMatchResult.MatchRoundItem));

	//��������	
	GameMatchDBAide.GetValue_Binary(TEXT("MatchRule"),GameMatchResult.cbMatchRule,sizeof(GameMatchResult.cbMatchRule));

	//��������
	GameMatchDBAide.GetValue_Binary(TEXT("MatchDistribute"), GameMatchResult.cbMatchDistribute, sizeof(GameMatchResult.cbMatchDistribute));
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
