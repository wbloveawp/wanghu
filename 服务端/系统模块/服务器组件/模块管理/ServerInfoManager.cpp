#include "StdAfx.h"
#include "ModuleDBParameter.h"
#include "ServerInfoManager.h"
#include "ModuleInfoManager.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CServerInfoBuffer::CServerInfoBuffer()
{
}

//��������
CServerInfoBuffer::~CServerInfoBuffer()
{
	//��������
	WORD wServerID=0;
	tagGameServerInfo * pGameServerInfo=NULL;
	POSITION Position=m_GameServerInfoMap.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);
		SafeDelete(pGameServerInfo);
	}

	//ɾ������
	for (INT_PTR i=0;i<m_GameServerInfoArray.GetCount();i++)
	{
		pGameServerInfo=m_GameServerInfoArray[i];
		SafeDelete(pGameServerInfo);
	}

	//ɾ������
	m_GameServerInfoMap.RemoveAll();
	m_GameServerInfoArray.RemoveAll();

	return;
}

//��������
bool CServerInfoBuffer::ResetServerInfo()
{
	//��������
	WORD wServerID=0;
	tagGameServerInfo * pGameServerInfo=NULL;
	POSITION Position=m_GameServerInfoMap.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);
		m_GameServerInfoArray.Add(pGameServerInfo);
	}

	//ɾ������
	m_GameServerInfoMap.RemoveAll();

	return true;
}

//ɾ������
bool CServerInfoBuffer::DeleteServerInfo(WORD wServerID)
{
	//��������
	tagGameServerInfo * pGameServerInfo=NULL;
	if (m_GameServerInfoMap.Lookup(wServerID,pGameServerInfo)==FALSE) return false;

	//ɾ������
	m_GameServerInfoMap.RemoveKey(wServerID);
	m_GameServerInfoArray.Add(pGameServerInfo);

	//���ñ���
	ZeroMemory(pGameServerInfo,sizeof(tagGameServerInfo));

	return true;
}

//��������
bool CServerInfoBuffer::InsertServerInfo(tagGameServerInfo * pGameServerInfo)
{
	//Ч�����
	ASSERT(pGameServerInfo!=NULL);
	if (pGameServerInfo==NULL) return false;

	//�����ִ�
	WORD wServerID=pGameServerInfo->wServerID;
	tagGameServerInfo * pGameServerInsert=SearchServerInfo(wServerID);

	//�����ж�
	if (pGameServerInsert==NULL)
	{
		//��������
		pGameServerInsert=CreateServerInfo();

		//����ж�
		if (pGameServerInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//��������
	m_GameServerInfoMap[wServerID]=pGameServerInsert;
	CopyMemory(pGameServerInsert,pGameServerInfo,sizeof(tagGameServerInfo));

	return true;
}

//��ȡ��Ŀ
DWORD CServerInfoBuffer::GetServerInfoCount()
{
	return (DWORD)(m_GameServerInfoMap.GetCount());
}

//��������
tagGameServerInfo * CServerInfoBuffer::SearchServerInfo(WORD wServerID)
{
	tagGameServerInfo * pGameServerInfo=NULL;
	m_GameServerInfoMap.Lookup(wServerID,pGameServerInfo);
	return pGameServerInfo;
}

//��������
tagGameServerInfo * CServerInfoBuffer::CreateServerInfo()
{
	//��������
	tagGameServerInfo * pGameServerInfo=NULL;

	//��������
	try
	{
		INT_PTR nArrayCount=m_GameServerInfoArray.GetCount();
		if (nArrayCount>0)
		{
			pGameServerInfo=m_GameServerInfoArray[nArrayCount-1];
			m_GameServerInfoArray.RemoveAt(nArrayCount-1);
		}
		else
		{
			pGameServerInfo=new tagGameServerInfo;
			if (pGameServerInfo==NULL) return NULL;
		}
	}
	catch (...) { return false; }

	//���ñ���
	ZeroMemory(pGameServerInfo,sizeof(tagGameServerInfo));

	return pGameServerInfo;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CServerInfoManager::CServerInfoManager()
{
}

//��������
CServerInfoManager::~CServerInfoManager()
{
}

//���ط���
bool CServerInfoManager::LoadGameServerInfo(WORD wServerID, tagGameServerInfo & GameServerResult)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//��ȡ�б�
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),TEXT(""));

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//��ȡ�б�
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameServerInfo"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//��ȡ����
		if (PlatformDBModule->IsRecordsetEnd()==false)
		{
			ReadGameServerInfo(PlatformDBAide,GameServerResult);
			return true;
		}

		return false;
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

//���ط���
bool CServerInfoManager::LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], CServerInfoBuffer & ServerInfoBuffer)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//�������
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),0);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),szMachineID);

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//��ȡ�б�
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameServerInfo"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//����б�
		ServerInfoBuffer.ResetServerInfo();

		//��ȡ�б�
		while (PlatformDBModule->IsRecordsetEnd()==false)
		{
			//��ȡ����
			tagGameServerInfo GameServerResult;
			ReadGameServerInfo(PlatformDBAide,GameServerResult);

			//�����б�
			ServerInfoBuffer.InsertServerInfo(&GameServerResult);

			//�ƶ���¼
			PlatformDBModule->MoveToNext();
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
bool CServerInfoManager::DeleteGameServer(WORD wServerID)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//�������
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ������
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_DeleteGameServer"),false)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
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
	}

	return false;
}

//���뷿��
bool CServerInfoManager::InsertGameServer(tagCreateServerInfo * pCreateServerInfo, tagGameServerInfo & GameServerResult)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//�������
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wModuleID"),pCreateServerInfo->wModuleID);
		PlatformDBAide.AddParameter(TEXT("@wKindID"),pCreateServerInfo->wKindID);
		PlatformDBAide.AddParameter(TEXT("@wNodeID"),pCreateServerInfo->wNodeID);
		PlatformDBAide.AddParameter(TEXT("@wSortID"),pCreateServerInfo->wSortID);
		PlatformDBAide.AddParameter(TEXT("@wVisibleMask"),pCreateServerInfo->wVisibleMask);
		PlatformDBAide.AddParameter(TEXT("@strServerName"),pCreateServerInfo->szServerName);
		PlatformDBAide.AddParameter(TEXT("@wTableCount"),pCreateServerInfo->wTableCount);
		PlatformDBAide.AddParameter(TEXT("@wServerType"),pCreateServerInfo->wServerType);
		PlatformDBAide.AddParameter(TEXT("@wServerPort"),pCreateServerInfo->wServerPort);
		PlatformDBAide.AddParameter(TEXT("@wServerLevel"), pCreateServerInfo->wServerLevel);
		PlatformDBAide.AddParameter(TEXT("@cbMatchType"), pCreateServerInfo->cbMatchType);
		PlatformDBAide.AddParameter(TEXT("@lCellScore"),pCreateServerInfo->lCellScore);
		PlatformDBAide.AddParameter(TEXT("@lLessScore"),pCreateServerInfo->lLessScore);
		PlatformDBAide.AddParameter(TEXT("@lRestrictScore"),pCreateServerInfo->lRestrictScore);
		PlatformDBAide.AddParameter(TEXT("@lRevenue"),pCreateServerInfo->lRevenue);
		PlatformDBAide.AddParameter(TEXT("@lExpGainRate"), pCreateServerInfo->lExpGainRate);
		PlatformDBAide.AddParameter(TEXT("@wDBListID"),pCreateServerInfo->wDBListID);
		PlatformDBAide.AddParameter(TEXT("@lMinEnterScore"),pCreateServerInfo->lMinEnterScore);
		PlatformDBAide.AddParameter(TEXT("@lMaxEnterScore"),pCreateServerInfo->lMaxEnterScore);
		PlatformDBAide.AddParameter(TEXT("@lMinEnterMember"),pCreateServerInfo->lMinEnterMember);
		PlatformDBAide.AddParameter(TEXT("@lMaxEnterMember"),pCreateServerInfo->lMaxEnterMember);		
		
		PlatformDBAide.AddParameter(TEXT("@cbDistributeMode"),pCreateServerInfo->cbDistributeMode);
		PlatformDBAide.AddParameter(TEXT("@cbDistributeRule"),pCreateServerInfo->cbDistributeRule);
		PlatformDBAide.AddParameter(TEXT("@wMinPlayerCount"),pCreateServerInfo->wMinPlayerCount);
		PlatformDBAide.AddParameter(TEXT("@wMinDistributeUser"),pCreateServerInfo->wMinDistributeUser);
		PlatformDBAide.AddParameter(TEXT("@wMinPartakeGameUser"),pCreateServerInfo->wMinPartakeGameUser);
		PlatformDBAide.AddParameter(TEXT("@wMaxPartakeGameUser"),pCreateServerInfo->wMaxPartakeGameUser);
		PlatformDBAide.AddParameter(TEXT("@wDistributeTimeSpace"),pCreateServerInfo->wDistributeTimeSpace);

		PlatformDBAide.AddParameter(TEXT("@dwBattleRule"),pCreateServerInfo->dwBattleRule);
		PlatformDBAide.AddParameter(TEXT("@wMaxPlayer"),pCreateServerInfo->wMaxPlayer);
		PlatformDBAide.AddParameter(TEXT("@dwServerRule"),pCreateServerInfo->dwServerRule);
		PlatformDBAide.AddParameter(TEXT("@dwAttachUserRight"),pCreateServerInfo->dwAttachUserRight);
		PlatformDBAide.AddParameter(TEXT("@strServiceAddr"),pCreateServerInfo->szServiceAddr);
		PlatformDBAide.AddParameter(TEXT("@strServiceDomain"),pCreateServerInfo->szServiceDomain);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),pCreateServerInfo->szServiceMachine);		
		PlatformDBAide.AddParameter(TEXT("@cbCustomRule"), pCreateServerInfo->cbCustomRule, sizeof(pCreateServerInfo->cbCustomRule));

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ������
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_InsertGameServer"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//��ȡ����
		ReadGameServerInfo(PlatformDBAide,GameServerResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//������ʾ
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//�޸ķ���
bool CServerInfoManager::ModifyGameServer(tagCreateServerInfo * pCreateServerInfo, tagGameServerInfo & GameServerResult)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//ִ���߼�
	try
	{
		//�������ݿ�
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���ö���
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//�������
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),pCreateServerInfo->wServerID);
		PlatformDBAide.AddParameter(TEXT("@wModuleID"),pCreateServerInfo->wModuleID);
		PlatformDBAide.AddParameter(TEXT("@wKindID"),pCreateServerInfo->wKindID);
		PlatformDBAide.AddParameter(TEXT("@wNodeID"),pCreateServerInfo->wNodeID);
		PlatformDBAide.AddParameter(TEXT("@wSortID"),pCreateServerInfo->wSortID);
		PlatformDBAide.AddParameter(TEXT("@wVisibleMask"),pCreateServerInfo->wVisibleMask);
		PlatformDBAide.AddParameter(TEXT("@strServerName"),pCreateServerInfo->szServerName);
		PlatformDBAide.AddParameter(TEXT("@wTableCount"),pCreateServerInfo->wTableCount);
		PlatformDBAide.AddParameter(TEXT("@wServerType"),pCreateServerInfo->wServerType);
		PlatformDBAide.AddParameter(TEXT("@wServerPort"),pCreateServerInfo->wServerPort);
		PlatformDBAide.AddParameter(TEXT("@wServerLevel"), pCreateServerInfo->wServerLevel);
		PlatformDBAide.AddParameter(TEXT("@cbMatchType"), pCreateServerInfo->cbMatchType);
		PlatformDBAide.AddParameter(TEXT("@lCellScore"),pCreateServerInfo->lCellScore);
		PlatformDBAide.AddParameter(TEXT("@lLessScore"),pCreateServerInfo->lLessScore);
		PlatformDBAide.AddParameter(TEXT("@lRestrictScore"),pCreateServerInfo->lRestrictScore);
		PlatformDBAide.AddParameter(TEXT("@lRevenue"),pCreateServerInfo->lRevenue);
		PlatformDBAide.AddParameter(TEXT("@lExpGainRate"), pCreateServerInfo->lExpGainRate);
		PlatformDBAide.AddParameter(TEXT("@wDBListID"),pCreateServerInfo->wDBListID);
		PlatformDBAide.AddParameter(TEXT("@lMinEnterScore"),pCreateServerInfo->lMinEnterScore);
		PlatformDBAide.AddParameter(TEXT("@lMaxEnterScore"),pCreateServerInfo->lMaxEnterScore);
		PlatformDBAide.AddParameter(TEXT("@lMinEnterMember"),pCreateServerInfo->lMinEnterMember);
		PlatformDBAide.AddParameter(TEXT("@lMaxEnterMember"),pCreateServerInfo->lMaxEnterMember);		

		PlatformDBAide.AddParameter(TEXT("@cbDistributeMode"),pCreateServerInfo->cbDistributeMode);
		PlatformDBAide.AddParameter(TEXT("@cbDistributeRule"),pCreateServerInfo->cbDistributeRule);
		PlatformDBAide.AddParameter(TEXT("@wMinPlayerCount"),pCreateServerInfo->wMinPlayerCount);
		PlatformDBAide.AddParameter(TEXT("@wMinDistributeUser"),pCreateServerInfo->wMinDistributeUser);
		PlatformDBAide.AddParameter(TEXT("@wMinPartakeGameUser"),pCreateServerInfo->wMinPartakeGameUser);
		PlatformDBAide.AddParameter(TEXT("@wMaxPartakeGameUser"),pCreateServerInfo->wMaxPartakeGameUser);
		PlatformDBAide.AddParameter(TEXT("@wDistributeTimeSpace"),pCreateServerInfo->wDistributeTimeSpace);

		PlatformDBAide.AddParameter(TEXT("@dwBattleRule"),pCreateServerInfo->dwBattleRule);
		PlatformDBAide.AddParameter(TEXT("@wMaxPlayer"),pCreateServerInfo->wMaxPlayer);
		PlatformDBAide.AddParameter(TEXT("@dwServerRule"),pCreateServerInfo->dwServerRule);
		PlatformDBAide.AddParameter(TEXT("@dwAttachUserRight"),pCreateServerInfo->dwAttachUserRight);
		PlatformDBAide.AddParameter(TEXT("@strServiceAddr"),pCreateServerInfo->szServiceAddr);
		PlatformDBAide.AddParameter(TEXT("@strServiceDomain"),pCreateServerInfo->szServiceDomain);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),pCreateServerInfo->szServiceMachine);
		PlatformDBAide.AddParameter(TEXT("@cbCustomRule"), pCreateServerInfo->cbCustomRule, sizeof(pCreateServerInfo->cbCustomRule));
		
		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//ִ������
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_ModifyGameServer"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//��ȡ����
		ReadGameServerInfo(PlatformDBAide,GameServerResult);

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
bool CServerInfoManager::ConnectPlatformDB(CDataBaseHelper & PlatformDBModule)
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

//��ȡ����
bool CServerInfoManager::ReadGameServerInfo(CDataBaseAide & PlatformDBAide, tagGameServerInfo & GameServerResult)
{
	//���ñ���
	ZeroMemory(&GameServerResult,sizeof(GameServerResult));

	//��������
	GameServerResult.wServerID=PlatformDBAide.GetValue_WORD(TEXT("ServerID"));
	GameServerResult.wModuleID=PlatformDBAide.GetValue_WORD(TEXT("ModuleID"));
	GameServerResult.wDBListID = PlatformDBAide.GetValue_WORD(TEXT("DBListID"));

	//�ҽ�����
	GameServerResult.wKindID=PlatformDBAide.GetValue_WORD(TEXT("KindID"));
	GameServerResult.wNodeID=PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
	GameServerResult.wSortID=PlatformDBAide.GetValue_WORD(TEXT("SortID"));
	GameServerResult.wVisibleMask=PlatformDBAide.GetValue_WORD(TEXT("VisibleMask"));

	//��������
	GameServerResult.lRevenue=PlatformDBAide.GetValue_LONG(TEXT("ServiceScore"));
	GameServerResult.lCellScore=PlatformDBAide.GetValue_LONG(TEXT("CellScore"));
	GameServerResult.lExpGainRate = PlatformDBAide.GetValue_LONG(TEXT("ExpGainRate"));
	GameServerResult.lLessScore=PlatformDBAide.GetValue_LONGLONG(TEXT("MinTableScore"));
	GameServerResult.lRestrictScore=PlatformDBAide.GetValue_LONGLONG(TEXT("RestrictScore"));
	GameServerResult.lMinEnterScore=PlatformDBAide.GetValue_LONGLONG(TEXT("MinEnterScore"));
	GameServerResult.lMaxEnterScore=PlatformDBAide.GetValue_LONGLONG(TEXT("MaxEnterScore"));
	GameServerResult.lMinEnterMember=PlatformDBAide.GetValue_LONG(TEXT("MinEnterMember"));
	GameServerResult.lMaxEnterMember=PlatformDBAide.GetValue_LONG(TEXT("MaxEnterMember"));

	//Լս����
	GameServerResult.dwBattleRule=PlatformDBAide.GetValue_DWORD(TEXT("BattleRule"));

	//�������
	GameServerResult.cbDistributeMode=PlatformDBAide.GetValue_BYTE(TEXT("DistributeMode"));
	GameServerResult.cbDistributeRule=PlatformDBAide.GetValue_BYTE(TEXT("DistributeRule"));
	GameServerResult.wMinPlayerCount=PlatformDBAide.GetValue_WORD(TEXT("MinPlayerCount"));
	GameServerResult.wMinDistributeUser=PlatformDBAide.GetValue_WORD(TEXT("MinDistributeUser"));
	GameServerResult.wMinPartakeGameUser=PlatformDBAide.GetValue_WORD(TEXT("MinPartakeGameUser"));
	GameServerResult.wMaxPartakeGameUser=PlatformDBAide.GetValue_WORD(TEXT("MaxPartakeGameUser"));
	GameServerResult.wDistributeTimeSpace=PlatformDBAide.GetValue_WORD(TEXT("DistributeTimeSpace"));

	//������Ϣ
	GameServerResult.wMaxPlayer=PlatformDBAide.GetValue_WORD(TEXT("MaxPlayer"));
	GameServerResult.wTableCount=PlatformDBAide.GetValue_WORD(TEXT("TableCount"));
	GameServerResult.wServerType=PlatformDBAide.GetValue_WORD(TEXT("ServerType"));
	GameServerResult.wServerPort=PlatformDBAide.GetValue_WORD(TEXT("ServerPort"));
	GameServerResult.wServerLevel = PlatformDBAide.GetValue_WORD(TEXT("ServerLevel"));
	GameServerResult.cbMatchType = PlatformDBAide.GetValue_BYTE(TEXT("MatchType"));
	GameServerResult.dwServerRule=PlatformDBAide.GetValue_DWORD(TEXT("ServerRule"));
	GameServerResult.dwAttachUserRight=PlatformDBAide.GetValue_DWORD(TEXT("AttachUserRight"));

	PlatformDBAide.GetValue_String(TEXT("ServerName"),GameServerResult.szServerName,CountArray(GameServerResult.szServerName));

	//�������
	GameServerResult.dwModuleVersion=PlatformDBAide.GetValue_DWORD(TEXT("ServerVersion"));
	GameServerResult.dwClientVersion=PlatformDBAide.GetValue_DWORD(TEXT("ClientVersion"));
	PlatformDBAide.GetValue_String(TEXT("ModuleName"),GameServerResult.szModuleName,CountArray(GameServerResult.szModuleName));
	PlatformDBAide.GetValue_String(TEXT("ServerDLLName"),GameServerResult.szServerDLLName,CountArray(GameServerResult.szServerDLLName));
	PlatformDBAide.GetValue_String(TEXT("ClientExeName"),GameServerResult.szClientEXEName,CountArray(GameServerResult.szClientEXEName));

	//��չ����	
	PlatformDBAide.GetValue_Binary(TEXT("CustomRule"), GameServerResult.cbCustomRule,sizeof(GameServerResult.cbCustomRule));
	PlatformDBAide.GetValue_String(TEXT("ServiceMachine"),GameServerResult.szServiceMachine,CountArray(GameServerResult.szServiceMachine));
	
	//������Ϣ
	PlatformDBAide.GetValue_String(TEXT("ServiceAddr"),GameServerResult.szServiceAddr,CountArray(GameServerResult.szServiceAddr));
	PlatformDBAide.GetValue_String(TEXT("ServiceDomain"),GameServerResult.szServiceDomain,CountArray(GameServerResult.szServiceDomain));

	//���ذ汾
	LPCTSTR pszServerDLLName=GameServerResult.szServerDLLName;
	CWHService::GetModuleVersion(pszServerDLLName,GameServerResult.dwNativeVersion);

	//������ذ汾�и���
	if( GameServerResult.dwModuleVersion < GameServerResult.dwNativeVersion )
	{
		CModuleInfoManager ModuleInfoManager;
		if( ModuleInfoManager.UpdateGameModuleInfo(GameServerResult.wModuleID,GameServerResult.dwNativeVersion) )
		{
			GameServerResult.dwModuleVersion = GameServerResult.dwNativeVersion;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
