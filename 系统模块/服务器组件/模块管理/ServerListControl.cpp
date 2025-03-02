#include "StdAfx.h"
#include "ServerListControl.h"
#include "DataBaseListManager.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CServerListControl::CServerListControl()
{
}

//��������
CServerListControl::~CServerListControl()
{
}

//�����б�
VOID CServerListControl::InitListControl()
{
	//��������
	INT nColIndex=0;

	//�����б�
	InsertColumn(nColIndex++,TEXT("��������"),LVCFMT_LEFT,80);
	InsertColumn(nColIndex++,TEXT("��Ϸ����"),LVCFMT_CENTER,80);
	InsertColumn(nColIndex++,TEXT("��������"),LVCFMT_CENTER,60);
	InsertColumn(nColIndex++,TEXT("������Ŀ"),LVCFMT_CENTER,60);
	InsertColumn(nColIndex++,TEXT("�����˿�"),LVCFMT_CENTER,60);
	InsertColumn(nColIndex++,TEXT("�����ʶ"),LVCFMT_CENTER,60);
	InsertColumn(nColIndex++,TEXT("���ͱ�ʶ"),LVCFMT_CENTER,60);
	InsertColumn(nColIndex++,TEXT("���ݿ���"),LVCFMT_LEFT,100);
	InsertColumn(nColIndex++,TEXT("��������"),LVCFMT_LEFT,140);
	InsertColumn(nColIndex++,TEXT("�ͻ�����"),LVCFMT_LEFT,120);

	return;
}

//��������
INT CServerListControl::SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex)
{
	//��������
	tagGameServerInfo * pGameServerInfo1=(tagGameServerInfo *)lParam1;
	tagGameServerInfo * pGameServerInfo2=(tagGameServerInfo *)lParam2;

	//��װ����
	bool bInstall1=(pGameServerInfo1->dwNativeVersion!=0L);
	bool bInstall2=(pGameServerInfo2->dwNativeVersion!=0L);
	if (bInstall1!=bInstall2) return (bInstall1==true)?1:-1;

	//��������
	switch (wColumnIndex)
	{
	case 0:		//��������
		{
			return lstrcmp(pGameServerInfo1->szServerName,pGameServerInfo2->szServerName);
		}
	case 1:		//��Ϸ����
		{
			return lstrcmp(pGameServerInfo1->szModuleName,pGameServerInfo2->szModuleName);
		}
	case 2:		//��������
		{
			return (pGameServerInfo1->wServerType>pGameServerInfo2->wServerType)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 3:		//������Ŀ
		{
			return (pGameServerInfo1->wTableCount>pGameServerInfo2->wTableCount)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 4:		//�����˿�
		{
			return (pGameServerInfo1->wServerPort>pGameServerInfo2->wServerPort)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 5:		//�����ʶ
		{
			return (pGameServerInfo1->wServerID>pGameServerInfo2->wServerID)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 6:		//���ͱ�ʶ
		{
			//�����ʶ
			WORD wKindID1=pGameServerInfo1->wKindID;
			if (wKindID1==0) wKindID1=pGameServerInfo1->wModuleID;

			//�����ʶ
			WORD wKindID2=pGameServerInfo2->wKindID;
			if (wKindID2==0) wKindID2=pGameServerInfo2->wModuleID;

			return (wKindID1>wKindID2)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 7:		//���ݿ���
		{
			return (pGameServerInfo1->wDBListID>pGameServerInfo2->wDBListID)?SORT_POSITION_AFTER:SORT_POSITION_FRONT;
		}
	case 8:		//��������
		{
			return lstrcmp(pGameServerInfo1->szServerDLLName,pGameServerInfo2->szServerDLLName);
		}
	case 9:		//�ͻ�����
		{
			return lstrcmp(pGameServerInfo1->szClientEXEName,pGameServerInfo2->szClientEXEName);
		}
	}

	return 0;
}

//��ȡ��ɫ
VOID CServerListControl::GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor)
{
	//��������
	ASSERT(lItemParam!=NULL);
	tagGameServerInfo * pGameServerInfo=(tagGameServerInfo *)lItemParam;

	//û�а�װ
	if (pGameServerInfo->dwNativeVersion==0L)
	{
		//������ɫ
		ListItemColor.rcTextColor=RGB(125,125,125);
		ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(50,50,50):CR_NORMAL_BK;

		return;
	}

	//���ڸ���
	if (pGameServerInfo->dwNativeVersion!=pGameServerInfo->dwModuleVersion)
	{
		//������ɫ
		ListItemColor.rcTextColor=RGB(128,0,0);
		ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(230,230,0):CR_NORMAL_BK;

		return;
	}

	//�汾һ��
	if (pGameServerInfo->dwNativeVersion==pGameServerInfo->dwModuleVersion)
	{
		//������ɫ
		ListItemColor.rcBackColor=(uItemStatus&ODS_SELECTED)?RGB(0,0,128):CR_NORMAL_BK;
		ListItemColor.rcTextColor=(uItemStatus&ODS_SELECTED)?RGB(255,255,255):RGB(0,0,0);

		return;
	}

	return;
}

//�����б�
bool CServerListControl::InsertServerInfo(tagGameServerInfo * pGameServerInfo)
{
	//��������
	LVFINDINFO FindInfo;
	ZeroMemory(&FindInfo,sizeof(FindInfo));

	//���ñ���
	FindInfo.flags=LVFI_PARAM;
	FindInfo.lParam=(LPARAM)pGameServerInfo;

	//�����ж�
	INT nInsertItem=FindItem(&FindInfo);
	if (nInsertItem!=LB_ERR) return true;

	//�����б�
	for (WORD i=0;i<m_ListHeaderCtrl.GetItemCount();i++)
	{
		if (i==0)
		{
			//��������
			INT nIndex=GetInsertIndex(pGameServerInfo);
			LPCTSTR pszDescribe=GetDescribeString(pGameServerInfo,i);
			nInsertItem=InsertItem(LVIF_TEXT|LVIF_PARAM,nIndex,pszDescribe,0,0,0,(LPARAM)pGameServerInfo);
		}
		else
		{
			//�ַ�����
			SetItem(nInsertItem,i,LVIF_TEXT,GetDescribeString(pGameServerInfo,i),0,0,0,0);
		}
	}

	return true;
}

//��������
WORD CServerListControl::GetInsertIndex(tagGameServerInfo * pGameServerInfo)
{
	//��������
	INT nItemCount=GetItemCount();
	tagGameServerInfo * pGameServerTemp=NULL;

	//��ȡλ��
	for (INT i=0;i<nItemCount;i++)
	{
		//��ȡ����
		pGameServerTemp=(tagGameServerInfo *)GetItemData(i);

		//��װ�ж�
		if ((pGameServerInfo->dwNativeVersion==0)&&(pGameServerTemp->dwNativeVersion!=0))
		{
			continue;
		}
		
		//��Ϸ����
		if (lstrcmp(pGameServerInfo->szModuleName,pGameServerTemp->szModuleName)<0)
		{
			return i;
		}

		//��������
		if (lstrcmp(pGameServerInfo->szServerName,pGameServerTemp->szServerName)<0)
		{
			return i;
		}
	}

	return nItemCount;
}

//�����ַ�
LPCTSTR CServerListControl::GetDescribeString(tagGameServerInfo * pGameServerInfo, WORD wColumnIndex)
{
	//��������
	static TCHAR szDescribe[128]=TEXT("");

	//�����ַ�
	switch (wColumnIndex)
	{
	case 0:		//��������
		{
			return pGameServerInfo->szServerName;
		}
	case 1:		//��Ϸ����
		{
			return pGameServerInfo->szModuleName;
		}
	case 2:		//��������
		{
			//�Ƹ�����
			if (pGameServerInfo->wServerType&GAME_GENRE_GOLD)
			{
				StringCchCopy(szDescribe,CountArray(szDescribe), TEXT("�Ƹ�����"));
				return szDescribe;
			}

			//��������
			if (pGameServerInfo->wServerType&GAME_GENRE_SCORE)
			{
				StringCchCopy(szDescribe,CountArray(szDescribe), TEXT("��������"));
				return szDescribe;
			}

			//��������
			if (pGameServerInfo->wServerType&GAME_GENRE_MATCH)
			{
				StringCchCopy(szDescribe,CountArray(szDescribe), TEXT("��������"));
				return szDescribe;
			}

			//Լս����
			if (pGameServerInfo->wServerType&GAME_GENRE_BATTLE)
			{
				StringCchCopy(szDescribe,CountArray(szDescribe), TEXT("Լս����"));
				return szDescribe;
			}

			//��λ����
			if (pGameServerInfo->wServerType & GAME_GENRE_RANKING)
			{
				StringCchCopy(szDescribe, CountArray(szDescribe), TEXT("��λ����"));
				return szDescribe;
			}

			return TEXT("δ֪����");
		}
	case 3:		//������Ŀ
		{
			_itot_s(pGameServerInfo->wTableCount, szDescribe, 10);			
			return szDescribe;
		}
	case 4:		//�����˿�
		{
			_itot_s(pGameServerInfo->wServerPort, szDescribe, 10);
			return szDescribe;
		}
	case 5:		//�����ʶ
		{
			_itot_s(pGameServerInfo->wServerID, szDescribe, 10);
			return szDescribe;
		}
	case 6:		//���ͱ�ʶ
		{
			_itot_s((pGameServerInfo->wKindID == 0) ? pGameServerInfo->wModuleID : pGameServerInfo->wKindID, szDescribe, 10);
			return szDescribe;
		}
	case 7:		//���ݿ���
		{
			CDatabaseListManager *pDBListMgr = CDatabaseListManager::GetInstance();
			if( pDBListMgr == NULL ) return TEXT("NULL");

			tagDataBaseList *pDBList = pDBListMgr->SearchItem(pGameServerInfo->wDBListID);
			if( pDBList == NULL ) return TEXT("NULL");

			static TCHAR szDBName[128] = {0};
			szDBName[0] = 0;
			_sntprintf_s( szDBName,CountArray(szDBName),TEXT("%s(%s)"),pDBList->szDBAliasName,pDBList->szDBName );
			return szDBName;
		}
	case 8:		//��������
		{
			return pGameServerInfo->szServerDLLName;
		}
	case 9:	//�ͻ�����
		{
			return pGameServerInfo->szClientEXEName;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
