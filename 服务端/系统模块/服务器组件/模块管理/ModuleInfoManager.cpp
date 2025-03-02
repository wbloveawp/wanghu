#include "StdAfx.h"
#include "ModuleDBParameter.h"
#include "ModuleInfoManager.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CModuleInfoBuffer::CModuleInfoBuffer()
{
}

//��������
CModuleInfoBuffer::~CModuleInfoBuffer()
{
	//��������
	WORD wModuleID=0;
	tagGameModuleInfo * pGameModuleInfo=NULL;
	POSITION Position=m_GameModuleInfoMap.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameModuleInfo);
		SafeDelete(pGameModuleInfo);
	}

	//ɾ������
	for (INT_PTR i=0;i<m_GameModuleInfoArray.GetCount();i++)
	{
		pGameModuleInfo=m_GameModuleInfoArray[i];
		SafeDelete(pGameModuleInfo);
	}

	//ɾ������
	m_GameModuleInfoMap.RemoveAll();
	m_GameModuleInfoArray.RemoveAll();

	return;
}

//��������
bool CModuleInfoBuffer::ResetModuleInfo()
{
	//��������
	WORD wModuleID=0;
	tagGameModuleInfo * pGameModuleInfo=NULL;
	POSITION Position=m_GameModuleInfoMap.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameModuleInfo);
		m_GameModuleInfoArray.Add(pGameModuleInfo);
	}

	//ɾ������
	m_GameModuleInfoMap.RemoveAll();

	return true;
}

//ɾ������
bool CModuleInfoBuffer::DeleteModuleInfo(WORD wModuleID)
{
	//��������
	tagGameModuleInfo * pGameModuleInfo=NULL;
	if (m_GameModuleInfoMap.Lookup(wModuleID,pGameModuleInfo)==FALSE) return false;

	//ɾ������
	m_GameModuleInfoMap.RemoveKey(wModuleID);
	m_GameModuleInfoArray.Add(pGameModuleInfo);

	//���ñ���
	ZeroMemory(pGameModuleInfo,sizeof(tagGameModuleInfo));

	return true;
}

//��������
bool CModuleInfoBuffer::InsertModuleInfo(tagGameModuleInfo * pGameModuleInfo)
{
	//Ч�����
	ASSERT(pGameModuleInfo!=NULL);
	if (pGameModuleInfo==NULL) return false;

	//�����ִ�
	WORD wModuleID=pGameModuleInfo->wModuleID;
	tagGameModuleInfo * pGameModuleInsert=SearchModuleInfo(wModuleID);

	//�����ж�
	if (pGameModuleInsert==NULL)
	{
		//��������
		pGameModuleInsert=CreateModuleInfo();

		//����ж�
		if (pGameModuleInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//��������
	m_GameModuleInfoMap[wModuleID]=pGameModuleInsert;
	CopyMemory(pGameModuleInsert,pGameModuleInfo,sizeof(tagGameModuleInfo));

	return true;
}

//��ȡ��Ŀ
DWORD CModuleInfoBuffer::GetModuleInfoCount()
{
	return (DWORD)(m_GameModuleInfoMap.GetCount());
}

//��������
tagGameModuleInfo * CModuleInfoBuffer::SearchModuleInfo(WORD wModuleID)
{
	tagGameModuleInfo * pGameModuleInfo=NULL;
	m_GameModuleInfoMap.Lookup(wModuleID,pGameModuleInfo);
	return pGameModuleInfo;
}

//��������
tagGameModuleInfo * CModuleInfoBuffer::CreateModuleInfo()
{
	//��������
	tagGameModuleInfo * pGameModuleInfo=NULL;

	//��������
	try
	{
		INT_PTR nArrayCount=m_GameModuleInfoArray.GetCount();
		if (nArrayCount>0)
		{
			pGameModuleInfo=m_GameModuleInfoArray[nArrayCount-1];
			m_GameModuleInfoArray.RemoveAt(nArrayCount-1);
		}
		else
		{
			pGameModuleInfo=new tagGameModuleInfo;
			if (pGameModuleInfo==NULL) return NULL;
		}
	}
	catch (...) { return false; }

	//���ñ���
	ZeroMemory(pGameModuleInfo,sizeof(tagGameModuleInfo));

	return pGameModuleInfo;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CModuleInfoManager::CModuleInfoManager()
{
}

//��������
CModuleInfoManager::~CModuleInfoManager()
{
}

//ע��ģ��
bool CModuleInfoManager::RegisterGameModule(LPCTSTR pszModuleName)
{
	return true;
}

//ע��ģ��
bool CModuleInfoManager::UnRegisterGameModule(LPCTSTR pszModuleName)
{
	return true;
}

//����ģ��
bool CModuleInfoManager::LoadGameModuleInfo(CModuleInfoBuffer & ModuleInfoBuffer)
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//��������
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("LoadGameModuleInfo ���� PlatformDBModule ����ʧ��"),MB_ICONERROR);
		return false;
	}

	//�������ݿ�
	try
	{
		//��������
		CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
		tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

		//��������
		PlatformDBModule->SetConnectionInfo(pDataBaseParameter->dwDataBaseAddr,pDataBaseParameter->wDataBasePort,
		CW2CT(pDataBaseParameter->szDataBaseName),CW2CT(pDataBaseParameter->szDataBaseUser),CW2CT(pDataBaseParameter->szDataBasePass));

		//��������
		PlatformDBModule->OpenConnection();
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//��ȡ�б�
		PlatformDBAide.ResetParameter();
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameModuleInfo"),true)==DB_SUCCESS)
		{
			//����б�
			ModuleInfoBuffer.ResetModuleInfo();

			//��ȡ�б�
			while (PlatformDBModule->IsRecordsetEnd()==false)
			{
				//��������
				tagGameModuleInfo GameModuleInfo;
				ZeroMemory(&GameModuleInfo,sizeof(GameModuleInfo));

				//��ȡ��Ϣ
				GameModuleInfo.wModuleID=PlatformDBAide.GetValue_WORD(TEXT("ModuleID"));
				GameModuleInfo.wDBListID = PlatformDBAide.GetValue_WORD(TEXT("DBListID"));
				GameModuleInfo.wSupportType = PlatformDBAide.GetValue_WORD(TEXT("SupportType"));
				GameModuleInfo.dwModuleVersion=PlatformDBAide.GetValue_DWORD(TEXT("ServerVersion"));
				GameModuleInfo.dwClientVersion = PlatformDBAide.GetValue_DWORD(TEXT("ClientVersion"));
				PlatformDBAide.GetValue_String(TEXT("ModuleName"),GameModuleInfo.szModuleName,CountArray(GameModuleInfo.szModuleName));
				PlatformDBAide.GetValue_String(TEXT("ServerDLLName"),GameModuleInfo.szServerDLLName,CountArray(GameModuleInfo.szServerDLLName));
				PlatformDBAide.GetValue_String(TEXT("ClientExeName"),GameModuleInfo.szClientEXEName,CountArray(GameModuleInfo.szClientEXEName));

				//���ذ汾
				LPCTSTR pszServerDLLName=GameModuleInfo.szServerDLLName;
				CWHService::GetModuleVersion(pszServerDLLName,GameModuleInfo.dwNativeVersion);

				//�б���
				ModuleInfoBuffer.InsertModuleInfo(&GameModuleInfo);

				//�ƶ���¼
				PlatformDBModule->MoveToNext();
			}
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

//�޸�ģ��
bool CModuleInfoManager::UpdateGameModuleInfo( tagGameModuleInfo &GameModuleInfo )
{
	//��������
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//��������
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("UpdateGameModuleInfo ���� PlatformDBModule ����ʧ��"),MB_ICONERROR);
		return false;
	}

	//�������ݿ�
	try
	{
		//��������
		CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
		tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

		//��������
		PlatformDBModule->SetConnectionInfo(pDataBaseParameter->dwDataBaseAddr,pDataBaseParameter->wDataBasePort,
		CW2CT(pDataBaseParameter->szDataBaseName),CW2CT(pDataBaseParameter->szDataBaseUser),CW2CT(pDataBaseParameter->szDataBasePass));

		//��������
		PlatformDBModule->OpenConnection();
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//��ȡ�б�
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wModuleID"),GameModuleInfo.wModuleID);
		PlatformDBAide.AddParameter(TEXT("@wDBListID"),GameModuleInfo.wDBListID);
		PlatformDBAide.AddParameter(TEXT("@wSupportType"),GameModuleInfo.wSupportType);
		PlatformDBAide.AddParameter(TEXT("@szModuleName"),GameModuleInfo.szModuleName);
		PlatformDBAide.AddParameter(TEXT("@dwModuleVerion"),GameModuleInfo.dwNativeVersion);
		PlatformDBAide.AddParameter(TEXT("@dwClientVerion"),GameModuleInfo.dwClientVersion);
		PlatformDBAide.AddParameter(TEXT("@szServerDLLName"),GameModuleInfo.szServerDLLName);
		PlatformDBAide.AddParameter(TEXT("@szClientExeName"),GameModuleInfo.szClientEXEName);

		//�����Ϣ
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_UpdateGameModuleInfo"),true)!=DB_SUCCESS)
		{
			//��ȡ��Ϣ
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//��ȡ��Ϣ
		GameModuleInfo.wModuleID=PlatformDBAide.GetValue_WORD(TEXT("ModuleID"));
		GameModuleInfo.wDBListID = PlatformDBAide.GetValue_WORD(TEXT("DBListID"));
		GameModuleInfo.wSupportType = PlatformDBAide.GetValue_WORD(TEXT("SupportType"));
		GameModuleInfo.dwModuleVersion=PlatformDBAide.GetValue_DWORD(TEXT("ServerVersion"));
		GameModuleInfo.dwClientVersion=PlatformDBAide.GetValue_DWORD(TEXT("ClientVersion"));
		PlatformDBAide.GetValue_String(TEXT("ModuleName"),GameModuleInfo.szModuleName,CountArray(GameModuleInfo.szModuleName));
		PlatformDBAide.GetValue_String(TEXT("ServerDLLName"),GameModuleInfo.szServerDLLName,CountArray(GameModuleInfo.szServerDLLName));
		PlatformDBAide.GetValue_String(TEXT("ClientExeName"),GameModuleInfo.szClientEXEName,CountArray(GameModuleInfo.szClientEXEName));

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

//�޸�ģ��
bool CModuleInfoManager::UpdateGameModuleInfo( WORD wModuleID, DWORD dwModuleVersion)
{
	//��������
	CModuleInfoBuffer ModuleInfoBuffer;
	
	//����ģ��
	if( LoadGameModuleInfo(ModuleInfoBuffer) )
	{
		//����ģ��
		tagGameModuleInfo *pGameModuleInfo = ModuleInfoBuffer.SearchModuleInfo(wModuleID);
		if( pGameModuleInfo == NULL )
		{
			TCHAR szDescribeString[]=TEXT("��Ӧ��ģ����Ϣ�����ڣ�ģ���޸�ʧ�ܣ�");

			//������ʾ
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//������ʾ
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		pGameModuleInfo->dwNativeVersion = dwModuleVersion;
		return UpdateGameModuleInfo(*pGameModuleInfo);
	}

	return false;
}

//ģ������
bool CModuleInfoManager::GetGameServiceAttrib(LPCTSTR pszModuleName, tagGameServiceAttrib & GameServiceAttrib)
{
	//���ñ���
	ZeroMemory(&GameServiceAttrib,sizeof(GameServiceAttrib));

	//��Ϸģ��
	CGameServiceManagerHelper GameServiceManager;
	GameServiceManager.SetModuleCreateInfo(pszModuleName,GAME_SERVICE_CREATE_NAME);

	//����ģ��
	if (GameServiceManager.CreateInstance()==true)
	{
		GameServiceManager->GetGameServiceAttrib(GameServiceAttrib);
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
