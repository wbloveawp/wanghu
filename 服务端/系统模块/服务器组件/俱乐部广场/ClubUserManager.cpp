#include "StdAfx.h"
#include "ClubUserManager.h"

//////////////////////////////////////////////////////////////////////////////////
//构造函数
CClubUserItem::CClubUserItem()
{
	//设置变量	
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_SocketInfo,sizeof(m_SocketInfo));
}

//析构函数
CClubUserItem::~CClubUserItem()
{
}

//重置数据
VOID CClubUserItem::ResetUserItem()
{
	//设置变量	
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_SocketInfo,sizeof(m_SocketInfo));
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CClubUserManager::CClubUserManager()
{
	//设置质数
	m_StockClubUser.InitHashTable(PRIME_PLATFORM_USER);

	return;
}

//析构函数
CClubUserManager::~CClubUserManager()
{
	//删除用户
	m_StockClubUser.DeleteAllItem();

	return;
}

//枚举用户
CClubUserItem * CClubUserManager::EnumUserItem(CClubUserItem* pClubUserItem)
{
	if (pClubUserItem == NULL)
	{
		return m_StockClubUser.GetHeadStockItem();
	}

	return m_StockClubUser.GetNextStockItem(pClubUserItem);
}

//查找用户
CClubUserItem * CClubUserManager::SearchUserItem(DWORD dwUserID)
{
	return m_StockClubUser.SearchStockItem(dwUserID);	
}

//删除用户
VOID CClubUserManager::DeleteUserItem()
{
	m_StockClubUser.RemoveAllItem();
}

//删除用户
bool CClubUserManager::DeleteUserItem(DWORD dwUserID)
{
	return m_StockClubUser.RemoveStockItem(dwUserID)!=NULL;
}

//插入用户
CClubUserItem* CClubUserManager::InsertUserItem(tagClubUserInfo & UserInfo, tagSocketInfo &SocketInfo)
{
	//变量定义
	auto pClubUserItem = m_StockClubUser.SearchStockItem(UserInfo.dwUserID);
	if (pClubUserItem == NULL)
	{
		pClubUserItem = m_StockClubUser.CreateStockItem(UserInfo.dwUserID);		
	}

	//拷贝数据
	if (pClubUserItem != NULL)
	{
		//属性变量
		CopyMemory(&pClubUserItem->m_UserInfo, &UserInfo, sizeof(UserInfo));
		CopyMemory(&pClubUserItem->m_SocketInfo, &SocketInfo, sizeof(SocketInfo));
	}

	return pClubUserItem;
}

//////////////////////////////////////////////////////////////////////////////////
