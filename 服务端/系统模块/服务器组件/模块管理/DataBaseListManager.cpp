#include "StdAfx.h"
#include "DataBaseListManager.h"
#include "ModuleDBParameter.h"

//////////////////////////////////////////////////////////////////////////

bool CDatabaseListManager::m_bLoad = false;
CDatabaseListManager * CDatabaseListManager::m_spDBListMgr = NULL;

//////////////////////////////////////////////////////////////////////////

//���캯��
CDatabaseListManager::CDatabaseListManager()
{
	if( m_spDBListMgr == NULL )
	{
		m_spDBListMgr = this;
	}
}

//��������
CDatabaseListManager::~CDatabaseListManager()
{
	if( m_spDBListMgr == this )
	{
		m_spDBListMgr = NULL;
	}

	//��������
	WORD wDBListID=0;
	tagDataBaseList * pItem=NULL;
	POSITION Position=m_ListMap.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_ListMap.GetNextAssoc(Position,wDBListID,pItem);
		SafeDelete(pItem);
	}

	//ɾ������
	for (INT_PTR i=0;i<m_ListArray.GetCount();i++)
	{
		pItem=m_ListArray[i];
		SafeDelete(pItem);
	}

	//ɾ������
	m_ListMap.RemoveAll();
	m_ListArray.RemoveAll();

	return;
}

//�������ݿ�
bool CDatabaseListManager::Load()
{
	//�����ж�
	if( !m_bLoad )
	{
		//���ñ�־
		m_bLoad = true;

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
			if (PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadDatabaseList"),true)==DB_SUCCESS)
			{
				//����б�
				Reset();

				//��ȡ�б�
				while (PlatformDBModule->IsRecordsetEnd()==false)
				{
					//��������
					tagDataBaseList item = {0};

					//��ȡ��Ϣ
					item.wDBListID = PlatformDBAide.GetValue_WORD(TEXT("DBListID"));
					item.wDBInfoID = PlatformDBAide.GetValue_WORD(TEXT("DBInfoID"));
					item.dwDBGenre = PlatformDBAide.GetValue_DWORD(TEXT("DBGenre"));
					PlatformDBAide.GetValue_String(TEXT("DBName"),item.szDBName,CountArray(item.szDBName));
					PlatformDBAide.GetValue_String(TEXT("DBAliasName"),item.szDBAliasName,CountArray(item.szDBAliasName));

					//�б���
					Insert(&item);

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
	}

	return true;
}

//��������
bool CDatabaseListManager::Reset()
{
	//��������
	WORD wDBListID=0;
	tagDataBaseList * pItem=NULL;
	POSITION Position=m_ListMap.GetStartPosition();

	//ɾ������
	while (Position!=NULL)
	{
		m_ListMap.GetNextAssoc(Position,wDBListID,pItem);
		m_ListArray.Add(pItem);
	}

	//ɾ������
	m_ListMap.RemoveAll();

	return true;
}

//ɾ������
bool CDatabaseListManager::Delete(WORD wDBListID)
{
	//��������
	tagDataBaseList * pItem=NULL;
	if (m_ListMap.Lookup(wDBListID,pItem)==FALSE) return false;

	//ɾ������
	m_ListMap.RemoveKey(wDBListID);
	m_ListArray.Add(pItem);

	//���ñ���
	ZeroMemory(pItem,sizeof(tagDataBaseList));

	return true;
}

//��������
bool CDatabaseListManager::Insert(tagDataBaseList * pItem)
{
	//Ч�����
	ASSERT(pItem!=NULL);
	if (pItem==NULL) return false;

	//�����ִ�
	WORD wDBListID=pItem->wDBListID;
	tagDataBaseList * pItemInsert=SearchItem(wDBListID);

	//�����ж�
	if (pItemInsert==NULL)
	{
		//��������
		pItemInsert=CreateItem();

		//����ж�
		if (pItemInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//��������
	m_ListMap[wDBListID] = pItemInsert;
	CopyMemory(pItemInsert,pItem,sizeof(tagDataBaseList));

	return true;
}

//��ȡ��Ŀ
INT_PTR CDatabaseListManager::GetCount()
{
	return m_ListMap.GetCount();
}

//��������
tagDataBaseList * CDatabaseListManager::SearchItem(WORD wDBListID)
{
	tagDataBaseList * pItem=NULL;
	m_ListMap.Lookup(wDBListID,pItem);
	return pItem;
}

//��������
POSITION CDatabaseListManager::GetStartPosition()
{
	return m_ListMap.GetStartPosition();
}

//��������
tagDataBaseList * CDatabaseListManager::GetNext( POSITION &pos )
{
	//��������
	WORD wDBListID = 0;
	tagDataBaseList *pItem = NULL;

	m_ListMap.GetNextAssoc(pos,wDBListID,pItem);

	return pItem;
}

//��������
tagDataBaseList * CDatabaseListManager::CreateItem()
{
	//��������
	tagDataBaseList * pItem=NULL;

	//��������
	try
	{
		INT_PTR nArrayCount=m_ListArray.GetCount();
		if (nArrayCount>0)
		{
			pItem=m_ListArray[nArrayCount-1];
			m_ListArray.RemoveAt(nArrayCount-1);
		}
		else
		{
			pItem=new tagDataBaseList;
			if (pItem==NULL) return NULL;
		}
	}
	catch (...) { return false; }

	//���ñ���
	ZeroMemory(pItem,sizeof(tagDataBaseList));

	return pItem;
}

//////////////////////////////////////////////////////////////////////////
