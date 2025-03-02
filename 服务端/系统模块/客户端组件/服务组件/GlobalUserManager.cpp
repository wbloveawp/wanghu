#include "StdAfx.h"
#include "GlobalUserManager.h"

//////////////////////////////////////////////////////////////////////////
//静态变量

CGlobalUserItemArray CGlobalUserManager::m_UserItemStorage;				//存储数组

CExtraItemPtrArray CGlobalUserItem::m_ExtraItemStorge;					//信息库存

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalUserItem::CGlobalUserItem()
{
	//设置变量
	ZeroMemory(&m_GlobalUserInfo,sizeof(m_GlobalUserInfo));

	return;
}

//析构函数
CGlobalUserItem::~CGlobalUserItem()
{
	//删除信息数组
	for( INT_PTR i = 0; i < m_ExtraItemPtrArry.GetCount(); i++ )
	{
		tagExtraItem *pExtraItem = m_ExtraItemPtrArry[i];

		SafeDeleteArray(pExtraItem->pBuffer);

		ZeroMemory(&pExtraItem->DataDescribe,sizeof(pExtraItem->DataDescribe));

		m_ExtraItemStorge.Add(pExtraItem);
	}

	m_ExtraItemPtrArry.RemoveAll();
}

//接口查询
VOID * __cdecl CGlobalUserItem::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGlobalUserItem,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGlobalUserItem,Guid,dwQueryVer);
	return NULL;
}

//额外属性
VOID * __cdecl CGlobalUserItem::GetExtraInfo(WORD &wDataSize,WORD wDataType)
{
	//遍历数组
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

//添加信息
bool __cdecl CGlobalUserItem::AddExtraInfo(VOID * pData, WORD wDataSize, WORD wDataType)
{
	//变量定义
	tagExtraItem *pExtraItem = NULL;

	//寻找现有
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
			//新创建
			try{ pExtraItem = new tagExtraItem; }
			catch(...) { ASSERT(FALSE); }
			if( pExtraItem == NULL ) return false;

			ZeroMemory( pExtraItem,sizeof(tagExtraItem) );
		}

		pExtraItem->DataDescribe.wDataDescribe = wDataType;
	}

	//分配新内存
	BYTE *pBuffer = NULL;
	try{ pBuffer = new BYTE[pExtraItem->DataDescribe.wDataSize+wDataSize]; }
	catch(...) { ASSERT(FALSE); }
	if( pBuffer == NULL ) return false;

	//复制数据
	if( pExtraItem->DataDescribe.wDataSize != 0 )
	{
		CopyMemory( pBuffer,pExtraItem->pBuffer,pExtraItem->DataDescribe.wDataSize );
		CopyMemory( &pBuffer[pExtraItem->DataDescribe.wDataSize],pData,wDataSize );

		//删除原有
		SafeDeleteArray(pExtraItem->pBuffer);
	}
	else
	{
		CopyMemory( pBuffer,pData,wDataSize );
	}

	pExtraItem->DataDescribe.wDataSize += wDataSize;
	pExtraItem->pBuffer = pBuffer;

	//添加
	if( !bExist )
		m_ExtraItemPtrArry.Add(pExtraItem);

	return true;
}

//删除信息
bool __cdecl CGlobalUserItem::RemoveExtraInfo(WORD wDataType)
{
	//遍历信息
	for( INT_PTR i = 0; i < m_ExtraItemPtrArry.GetCount(); i++ )
	{
		tagExtraItem *pExtraItem = m_ExtraItemPtrArry[i];

		if( pExtraItem->DataDescribe.wDataDescribe == wDataType )
		{
			//删除
			SafeDeleteArray(pExtraItem->pBuffer);

			ZeroMemory( &pExtraItem->DataDescribe,sizeof(pExtraItem->DataDescribe) );

			m_ExtraItemPtrArry.RemoveAt(i);
			m_ExtraItemStorge.Add(pExtraItem);

			return true;
		}
	}

	return false;
}

//重置用户
bool __cdecl CGlobalUserItem::ResetUserItem()
{
	//清0
	ZeroMemory( &m_GlobalUserInfo,sizeof(m_GlobalUserInfo) );

	//遍历信息
	for( INT_PTR i = 0; i < m_ExtraItemPtrArry.GetCount(); i++ )
	{
		tagExtraItem *pExtraItem = m_ExtraItemPtrArry[i];

		//删除
		SafeDeleteArray(pExtraItem->pBuffer);

		ZeroMemory( &pExtraItem->DataDescribe,sizeof(pExtraItem->DataDescribe) );

		m_ExtraItemStorge.Add(pExtraItem);
	}
	m_ExtraItemPtrArry.RemoveAll();

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalUserManager::CGlobalUserManager()
{
}

//析构函数
CGlobalUserManager::~CGlobalUserManager()
{
	//存储用户
	m_UserItemStorage.Append(m_UserItemActive);

	//释放资源
	for (INT_PTR i=0;i<m_UserItemStorage.GetCount();i++)
	{
		if (m_UserItemStorage[i]!=NULL) m_UserItemStorage[i]->Release();
	}

	//清理用户
	m_UserItemActive.RemoveAll();

	//删除回调
	m_UserManagerSinkArry.RemoveAll();

	return;
}

//接口查询
VOID * __cdecl CGlobalUserManager::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IGlobalUserManager,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IGlobalUserManager,Guid,dwQueryVer);
	return NULL;
}

//设置接口
bool __cdecl CGlobalUserManager::AddUserManagerSink(IUnknownEx * pIUnknownEx)
{
	//效验
	ASSERT(pIUnknownEx);
	if( !pIUnknownEx ) return false;

	//定义接口
	IGlobalUserManagerSink *pIUserManagerSink = QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IGlobalUserManagerSink);
	ASSERT(pIUserManagerSink);
	if( !pIUserManagerSink ) return false;

	//寻找现有
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

//删除接口
bool __cdecl CGlobalUserManager::RemoveUserManagerSink(IUnknownEx *pIUnkownEx)
{
	//效验
	ASSERT(pIUnkownEx);
	if( !pIUnkownEx ) return false;

	//定义接口
	IGlobalUserManagerSink *pIUserManagerSink = QUERY_OBJECT_PTR_INTERFACE(pIUnkownEx,IGlobalUserManagerSink);
	ASSERT(pIUserManagerSink);
	if( !pIUserManagerSink ) return false;

	//寻找
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

//删除接口
bool __cdecl CGlobalUserManager::RemoveAllUserManagerSink()
{
	m_UserManagerSinkArry.RemoveAll();
	return true;
}

//增加用户
IGlobalUserItem * __cdecl CGlobalUserManager::ActiveUserItem(const tagGlobalUserInfo & GlobalUserInfo)
{
	//变量定义
	CGlobalUserItem *pGlobalUserItem = NULL;

	//搜索库存
	if( m_UserItemStorage.GetCount() > 0 )
	{
		INT_PTR nIndex = m_UserItemStorage.GetCount()-1;
		pGlobalUserItem = m_UserItemStorage[nIndex];

		m_UserItemStorage.RemoveAt(nIndex);
	}
	if( pGlobalUserItem == NULL )
	{
		//创建对象
		try{ pGlobalUserItem = new CGlobalUserItem; }
		catch(...) { ASSERT(FALSE); }

		if( !pGlobalUserItem ) return NULL;
	}

	//设置变量
	CopyMemory( &pGlobalUserItem->m_GlobalUserInfo,&GlobalUserInfo,sizeof(GlobalUserInfo) );

	//添加对象
	m_UserItemActive.Add(pGlobalUserItem);

	//通知回调
	for( INT_PTR i = 0; i < m_UserManagerSinkArry.GetCount(); i++ )
	{
		IGlobalUserManagerSink *pIUserManagerSink = m_UserManagerSinkArry[i];

		pIUserManagerSink->OnUserItemAcitve(QUERY_OBJECT_PTR_INTERFACE(pGlobalUserItem,IGlobalUserItem));
	}

	return QUERY_OBJECT_PTR_INTERFACE(pGlobalUserItem,IGlobalUserItem);
}

//删除用户
bool __cdecl CGlobalUserManager::DeleteUserItem(IGlobalUserItem * pIGlobalUserItem)
{
	//变量定义
	IGlobalUserItem *pITargetUserItem = NULL;

	//查找用户
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

	//找不到
	if( !pITargetUserItem ) return false;

	//通知回调
	for( INT_PTR i = 0; i < m_UserManagerSinkArry.GetCount(); i++ )
	{
		IGlobalUserManagerSink *pIUserManagerSink = m_UserManagerSinkArry[i];

		pIUserManagerSink->OnUserItemDelete(pITargetUserItem);
	}

	//重置用户
	pITargetUserItem->ResetUserItem();

	return true;
}

//更新基本信息
bool __cdecl CGlobalUserManager::UpdateUserItem(IGlobalUserItem * pIGlobalUserItem, tagGlobalUserInfo * const pGlobalUserInfo)
{
	//效验
	ASSERT(pIGlobalUserItem&&pGlobalUserInfo);
	if( !pIGlobalUserItem || !pGlobalUserInfo ) 
		return false;

	//变量定义
	CGlobalUserItem *pTargetUserItem = NULL;

	//查找用户
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

	//设置变量
	CopyMemory( &pTargetUserItem->m_GlobalUserInfo,pGlobalUserInfo,sizeof(tagGlobalUserInfo) );

	//通知回调
	for( INT_PTR i = 0; i < m_UserManagerSinkArry.GetCount(); i++ )
	{
		IGlobalUserManagerSink *pUserManagerSink = m_UserManagerSinkArry[i];

		pUserManagerSink->OnUserItemUpdate(pIGlobalUserItem);
	}

	return true;
}

//添加附加信息
bool __cdecl CGlobalUserManager::AddUserExtraInfo(IGlobalUserItem * pIGlobalUserItem, VOID * pData, WORD wDataSize, WORD wDataType)
{
	//效验
	ASSERT(pIGlobalUserItem);
	if( !pIGlobalUserItem ) return false;

	//添加信息
	if( pIGlobalUserItem->AddExtraInfo(pData,wDataSize,wDataType) )
	{
		//通知回调
		for( INT_PTR i = 0; i < m_UserManagerSinkArry.GetCount(); i++ )
		{
			IGlobalUserManagerSink *pUserManagerSink = m_UserManagerSinkArry[i];

			pUserManagerSink->OnUserExtraInfoUpdate(pIGlobalUserItem);
		}
	}

	return false;
}

//删除附加信息
bool __cdecl CGlobalUserManager::RemoveUserExtraInfo(IGlobalUserItem * pIGlobalUserItem, WORD wDataType)
{
	//效验
	ASSERT(pIGlobalUserItem);
	if( !pIGlobalUserItem ) return false;

	//添加信息
	if( pIGlobalUserItem->RemoveExtraInfo(wDataType) )
	{
		//通知回调
		for( INT_PTR i = 0; i < m_UserManagerSinkArry.GetCount(); i++ )
		{
			IGlobalUserManagerSink *pUserManagerSink = m_UserManagerSinkArry[i];

			pUserManagerSink->OnUserExtraInfoUpdate(pIGlobalUserItem);
		}
	}

	return false;
}

//枚举用户
IGlobalUserItem * __cdecl CGlobalUserManager::EnumUserItem(WORD wEnumIndex)
{
	if (wEnumIndex>=m_UserItemActive.GetCount()) return NULL;
	return m_UserItemActive[wEnumIndex];
}

//查找用户
IGlobalUserItem * __cdecl CGlobalUserManager::SearchUserByUserID(DWORD dwUserID)
{
	//用户搜索
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		CGlobalUserItem * pUserItem=m_UserItemActive[i];
		if (pUserItem->GetUserID()==dwUserID) return pUserItem;
	}

	return NULL;
}

//查找用户
IGlobalUserItem * __cdecl CGlobalUserManager::SearchUserByGameID(DWORD dwGameID)
{
	//用户搜索
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		CGlobalUserItem * pUserItem=m_UserItemActive[i];
		if (pUserItem->GetGameID()==dwGameID) return pUserItem;
	}

	return NULL;
}

//查找用户
IGlobalUserItem * __cdecl CGlobalUserManager::SearchUserByAccounts(LPCTSTR pszAccounts)
{
	//变量定义
	CT2CW strAccounts(pszAccounts);

	//用户搜索
	for (INT_PTR i=0;i<m_UserItemActive.GetCount();i++)
	{
		CGlobalUserItem * pUserItem=m_UserItemActive[i];
		LPCWSTR pszTempAccounts=pUserItem->GetAccounts();
		if (wcscmp((LPCWSTR)strAccounts,pszTempAccounts)==0) return pUserItem;
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(GlobalUserManager);

//////////////////////////////////////////////////////////////////////////
