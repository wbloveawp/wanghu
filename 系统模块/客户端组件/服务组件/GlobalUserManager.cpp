#include "StdAfx.h"
#include "GlobalUserManager.h"

//////////////////////////////////////////////////////////////////////////
//��̬����

CGlobalUserItemArray CGlobalUserManager::m_UserItemStorage;				//�洢����

CExtraItemPtrArray CGlobalUserItem::m_ExtraItemStorge;					//��Ϣ���

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalUserItem::CGlobalUserItem()
{
	//���ñ���
	ZeroMemory(&m_GlobalUserInfo,sizeof(m_GlobalUserInfo));

	return;
}

//��������
CGlobalUserItem::~CGlobalUserItem()
{
	//ɾ����Ϣ����
	for( INT_PTR i = 0; i < m_ExtraItemPtrArry.GetCount(); i++ )
	{
		tagExtraItem *pExtraItem = m_ExtraItemPtrArry[i];

		SafeDeleteArray(pExtraItem->pBuffer);

		ZeroMemory(&pExtraItem->DataDescribe,sizeof(pExtraItem->DataDescribe));

		m_ExtraItemStorge.Add(pExtraItem);
	}

	m_ExtraItemPtrArry.RemoveAll();
}

//�ӿڲ�ѯ
VOID * __cdecl CGlobalUserItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGlobalUserItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGlobalUserItem,Guid,dwQueryVer);
	return NULL;
}

//��������
VOID * __cdecl CGlobalUserItem::GetExtraInfo(WORD &wDataSize,WORD wDataType)
{
	//��������
	for( INT_PTR i = 0; i < m_ExtraItemPtrArry.GetCount(); i++ )
	{
		tagExtraItem *pExtraItem = m_ExtraItemPtrArry[i];

		if( pExtraItem->DataDescribe.wDataDescribe == wDataType )
		{
			wDataSize = pExtraItem->DataDescribe.wDataSize;

			return (VOID *)pExtraItem->pBuffer;
		}
	}

	wDataSize = 0;
	return NULL;
}

//�����Ϣ
bool __cdecl CGlobalUserItem::AddExtraInfo(VOID * pData, WORD wDataSize, WORD wDataType)
{
	//��������
	tagExtraItem *pExtraItem = NULL;

	//Ѱ������
	bool bExist = false;
	for( INT_PTR i = 0; i < m_ExtraItemPtrArry.GetCount(); i++ )
	{
		tagExtraItem *pTmpItem = m_ExtraItemPtrArry[i];

		if( pTmpItem->DataDescribe.wDataDescribe == wDataType )
		{
			pExtraItem = pTmpItem;
			bExist = true;
			break;
		}
	}
	if( pExtraItem == NULL )
	{
		if( m_ExtraItemStorge.GetCount() > 0 )
		{
			INT_PTR nIndex = m_ExtraItemStorge.GetCount()-1;
			pExtraItem = m_ExtraItemStorge[nIndex];
			m_ExtraItemStorge.RemoveAt(nIndex);
		}
		else
		{
			//�´���
			try{ pExtraItem = new tagExtraItem; }
			catch(...) { ASSERT(FALSE); }
			if( pExtraItem == NULL ) return false;

			ZeroMemory( pExtraItem,sizeof(tagExtraItem) );
		}

		pExtraItem->DataDescribe.wDataDescribe = wDataType;
	}

	//�������ڴ�
	BYTE *pBuffer = NULL;
	try{ pBuffer = new BYTE[pExtraItem->DataDescribe.wDataSize+wDataSize]; }
	catch(...) { ASSERT(FALSE); }
	if( pBuffer == NULL ) return false;

	//��������
	if( pExtraItem->DataDescribe.wDataSize != 0 )
	{
		CopyMemory( pBuffer,pExtraItem->pBuffer,pExtraItem->DataDescribe.wDataSize );
		CopyMemory( &pBuffer[pExtraItem->DataDescribe.wDataSize],pData,wDataSize );

		//ɾ��ԭ��
		SafeDeleteArray(pExtraItem->pBuffer);
	}
	else
	{
		CopyMemory( pBuffer,pData,wDataSize );
	}

	pExtraItem->DataDescribe.wDataSize += wDataSize;
	pExtraItem->pBuffer = pBuffer;

	//���
	if( !bExist )
		m_ExtraItemPtrArry.Add(pExtraItem);

	return true;
}

//ɾ����Ϣ
bool __cdecl CGlobalUserItem::RemoveExtraInfo(WORD wDataType)
{
	//������Ϣ
	for( INT_PTR i = 0; i < m_ExtraItemPtrArry.GetCount(); i++ )
	{
		tagExtraItem *pExtraItem = m_ExtraItemPtrArry[i];

		if( pExtraItem->DataDescribe.wDataDescribe == wDataType )
		{
			//ɾ��
			SafeDeleteArray(pExtraItem->pBuffer);

			ZeroMemory( &pExtraItem->DataDescribe,sizeof(pExtraItem->DataDescribe) );

			m_ExtraItemPtrArry.RemoveAt(i);
			m_ExtraItemStorge.Add(pExtraItem);

			return true;
		}
	}

	return false;
}

//�����û�
bool __cdecl CGlobalUserItem::ResetUserItem()
{
	//��0
	ZeroMemory( &m_GlobalUserInfo,sizeof(m_GlobalUserInfo) );

	//������Ϣ
	for( INT_PTR i = 0; i < m_ExtraItemPtrArry.GetCount(); i++ )
	{
		tagExtraItem *pExtraItem = m_ExtraItemPtrArry[i];

		//ɾ��
		SafeDeleteArray(pExtraItem->pBuffer);

		ZeroMemory( &pExtraItem->DataDescribe,sizeof(pExtraItem->DataDescribe) );

		m_ExtraItemStorge.Add(pExtraItem);
	}
	m_ExtraItemPtrArry.RemoveAll();

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalUserManager::CGlobalUserManager()
{
}

//��������
CGlobalUserManager::~CGlobalUserManager()
{
	//�洢�û�
	m_UserItemStorage.Append(m_UserItemActive);

	//�ͷ���Դ
	for (INT_PTR i=0;i<m_UserItemStorage.GetCount();i++)
	{
		if (m_UserItemStorage[i]!=NULL) m_UserItemStorage[i]->Release();
	}

	//�����û�
	m_UserItemActive.RemoveAll();

	//ɾ���ص�
	m_UserManagerSinkArry.RemoveAll();

	return;
}

//�ӿڲ�ѯ
VOID * __cdecl CGlobalUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGlobalUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGlobalUserManager,Guid,dwQueryVer);
	return NULL;
}

//���ýӿ�
bool __cdecl CGlobalUserManager::AddUserManagerSink(IUnknownEx * pIUnknownEx)
{
	//Ч��
	ASSERT(pIUnknownEx);
	if( !pIUnknownEx ) return false;

	//����ӿ�
	IGlobalUserManagerSink *pIUserManagerSink = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IGlobalUserManagerSink);
	ASSERT(pIUserManagerSink);
	if( !pIUserManagerSink ) return false;

	//Ѱ������
	for( INT_PTR i = 0; i < m_UserManagerSinkArry.GetCount(); i++ )
	{
		if( pIUserManagerSink == m_UserManagerSinkArry[i] ) 
		{
			ASSERT(FALSE);
			return true;
		}
	}

	m_UserManagerSinkArry.Add(pIUserManagerSink);

	return true;
}

//ɾ���ӿ�
bool __cdecl CGlobalUserManager::RemoveUserManagerSink(IUnknownEx *pIUnkownEx)
{
	//Ч��
	ASSERT(pIUnkownEx);
	if( !pIUnkownEx ) return false;

	//����ӿ�
	IGlobalUserManagerSink *pIUserManagerSink = QUERY_OBJECT_PTR_INTERFACE(pIUnkownEx,IGlobalUserManagerSink);
	ASSERT(pIUserManagerSink);
	if( !pIUserManagerSink ) return false;

	//Ѱ��
	for( INT_PTR i = 0; i < m_UserManagerSinkArry.GetCount(); i++ )
	{
		if( pIUserManagerSink == m_UserManagerSinkArry[i] ) 
		{
			m_UserManagerSinkArry.RemoveAt(i);

			return true;
		}
	}

	return false;
}

//ɾ���ӿ�
bool __cdecl CGlobalUserManager::RemoveAllUserManagerSink()
{
	m_UserManagerSinkArry.RemoveAll();
	return true;
}

//�����û�
IGlobalUserItem * __cdecl CGlobalUserManager::ActiveUserItem(const tagGlobalUserInfo & GlobalUserInfo)
{
	//��������
	CGlobalUserItem *pGlobalUserItem = NULL;

	//�������
	if( m_UserItemStorage.GetCount() > 0 )
	{
		INT_PTR nIndex = m_UserItemStorage.GetCount()-1;
		pGlobalUserItem = m_UserItemStorage[nIndex];

		m_UserItemStorage.RemoveAt(nIndex);
	}
	if( pGlobalUserItem == NULL )
	{
		//��������
		try{ pGlobalUserItem = new CGlobalUserItem; }
		catch(...) { ASSERT(FALSE); }

		if( !pGlobalUserItem ) return NULL;
	}

	//���ñ���
	CopyMemory( &pGlobalUserItem->m_GlobalUserInfo,&GlobalUserInfo,sizeof(GlobalUserInfo) );

	//��Ӷ���
	m_UserItemActive.Add(pGlobalUserItem);

	//֪ͨ�ص�
	for( INT_PTR i = 0; i < m_UserManagerSinkArry.GetCount(); i++ )
	{
		IGlobalUserManagerSink *pIUserManagerSink = m_UserManagerSinkArry[i];

		pIUserManagerSink->OnUserItemAcitve(QUERY_OBJECT_PTR_INTERFACE(pGlobalUserItem,IGlobalUserItem));
	}

	return QUERY_OBJECT_PTR_INTERFACE(pGlobalUserItem,IGlobalUserItem);
}

//ɾ���û�
bool __cdecl CGlobalUserManager::DeleteUserItem(IGlobalUserItem * pIGlobalUserItem)
{
	//��������
	IGlobalUserItem *pITargetUserItem = NULL;

	//�����û�
	for( INT_PTR i = 0; i < m_UserItemActive.GetCount(); i++ )
	{
		CGlobalUserItem *pTmpItem = m_UserItemActive[i];

		if( pTmpItem == pIGlobalUserItem )
		{
			pITargetUserItem = pTmpItem;

			m_UserItemActive.RemoveAt(i);
			m_UserItemStorage.Add(pTmpItem);

			break;
		}
	}

	//�Ҳ���
	if( !pITargetUserItem ) return false;

	//֪ͨ�ص�
	for( INT_PTR i = 0; i < m_UserManagerSinkArry.GetCount(); i++ )
	{
		IGlobalUserManagerSink *pIUserManagerSink = m_UserManagerSinkArry[i];

		pIUserManagerSink->OnUserItemDelete(pITargetUserItem);
	}

	//�����û�
	pITargetUserItem->ResetUserItem();

	return true;
}

//���»�����Ϣ
bool __cdecl CGlobalUserManager::UpdateUserItem(IGlobalUserItem * pIGlobalUserItem, tagGlobalUserInfo * const pGlobalUserInfo)
{
	//Ч��
	ASSERT(pIGlobalUserItem&&pGlobalUserInfo);
	if( !pIGlobalUserItem || !pGlobalUserInfo ) 
		return false;

	//��������
	CGlobalUserItem *pTargetUserItem = NULL;

	//�����û�
	for( INT_PTR i = 0; i < m_UserItemActive.GetCount(); i++ )
	{
		CGlobalUserItem *pTmpItem = m_UserItemActive[i];

		if( pTmpItem == pIGlobalUserItem )
		{
			pTargetUserItem = pTmpItem;

			break;
		}
	}

	if( !pTargetUserItem ) return false;

	//���ñ���
	CopyMemory( &pTargetUserItem->m_GlobalUserInfo,pGlobalUserInfo,sizeof(tagGlobalUserInfo) );

	//֪ͨ�ص�
	for( INT_PTR i = 0; i < m_UserManagerSinkArry.GetCount(); i++ )
	{
		IGlobalUserManagerSink *pUserManagerSink = m_UserManagerSinkArry[i];

		pUserManagerSink->OnUserItemUpdate(pIGlobalUserItem);
	}

	return true;
}

//��Ӹ�����Ϣ
bool __cdecl CGlobalUserManager::AddUserExtraInfo(IGlobalUserItem * pIGlobalUserItem, VOID * pData, WORD wDataSize, WORD wDataType)
{
	//Ч��
	ASSERT(pIGlobalUserItem);
	if( !pIGlobalUserItem ) return false;

	//�����Ϣ
	if( pIGlobalUserItem->AddExtraInfo(pData,wDataSize,wDataType) )
	{
		//֪ͨ�ص�
		for( INT_PTR i = 0; i < m_UserManagerSinkArry.GetCount(); i++ )
		{
			IGlobalUserManagerSink *pUserManagerSink = m_UserManagerSinkArry[i];

			pUserManagerSink->OnUserExtraInfoUpdate(pIGlobalUserItem);
		}
	}

	return false;
}

//ɾ��������Ϣ
bool __cdecl CGlobalUserManager::RemoveUserExtraInfo(IGlobalUserItem * pIGlobalUserItem, WORD wDataType)
{
	//Ч��
	ASSERT(pIGlobalUserItem);
	if( !pIGlobalUserItem ) return false;

	//�����Ϣ
	if( pIGlobalUserItem->RemoveExtraInfo(wDataType) )
	{
		//֪ͨ�ص�
		for( INT_PTR i = 0; i < m_UserManagerSinkArry.GetCount(); i++ )
		{
			IGlobalUserManagerSink *pUserManagerSink = m_UserManagerSinkArry[i];

			pUserManagerSink->OnUserExtraInfoUpdate(pIGlobalUserItem);
		}
	}

	return false;
}

//ö���û�
IGlobalUserItem * __cdecl CGlobalUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemActive.GetCount()) return NULL;
	return m_UserItemActive[wEnumIndex];
}

//�����û�
IGlobalUserItem * __cdecl CGlobalUserManager::SearchUserByUserID(DWORD dwUserID)
{
	//�û�����
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		CGlobalUserItem * pUserItem=m_UserItemActive[i];
		if (pUserItem->GetUserID()==dwUserID) return pUserItem;
	}

	return NULL;
}

//�����û�
IGlobalUserItem * __cdecl CGlobalUserManager::SearchUserByGameID(DWORD dwGameID)
{
	//�û�����
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		CGlobalUserItem * pUserItem=m_UserItemActive[i];
		if (pUserItem->GetGameID()==dwGameID) return pUserItem;
	}

	return NULL;
}

//�����û�
IGlobalUserItem * __cdecl CGlobalUserManager::SearchUserByAccounts(LPCTSTR pszAccounts)
{
	//��������
	CT2CW strAccounts(pszAccounts);

	//�û�����
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		CGlobalUserItem * pUserItem=m_UserItemActive[i];
		LPCWSTR pszTempAccounts=pUserItem->GetAccounts();
		if (wcscmp((LPCWSTR)strAccounts,pszTempAccounts)==0) return pUserItem;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(GlobalUserManager);

//////////////////////////////////////////////////////////////////////////
