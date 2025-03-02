#include "StdAfx.h"
#include "ClubUserManager.h"

//////////////////////////////////////////////////////////////////////////////////
//���캯��
CClubUserItem::CClubUserItem()
{
	//���ñ���	
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_SocketInfo,sizeof(m_SocketInfo));
}

//��������
CClubUserItem::~CClubUserItem()
{
}

//��������
VOID CClubUserItem::ResetUserItem()
{
	//���ñ���	
	ZeroMemory(&m_UserInfo,sizeof(m_UserInfo));
	ZeroMemory(&m_SocketInfo,sizeof(m_SocketInfo));
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CClubUserManager::CClubUserManager()
{
	//��������
	m_StockClubUser.InitHashTable(PRIME_PLATFORM_USER);

	return;
}

//��������
CClubUserManager::~CClubUserManager()
{
	//ɾ���û�
	m_StockClubUser.DeleteAllItem();

	return;
}

//ö���û�
CClubUserItem * CClubUserManager::EnumUserItem(CClubUserItem* pClubUserItem)
{
	if (pClubUserItem == NULL)
	{
		return m_StockClubUser.GetHeadStockItem();
	}

	return m_StockClubUser.GetNextStockItem(pClubUserItem);
}

//�����û�
CClubUserItem * CClubUserManager::SearchUserItem(DWORD dwUserID)
{
	return m_StockClubUser.SearchStockItem(dwUserID);	
}

//ɾ���û�
VOID CClubUserManager::DeleteUserItem()
{
	m_StockClubUser.RemoveAllItem();
}

//ɾ���û�
bool CClubUserManager::DeleteUserItem(DWORD dwUserID)
{
	return m_StockClubUser.RemoveStockItem(dwUserID)!=NULL;
}

//�����û�
CClubUserItem* CClubUserManager::InsertUserItem(tagClubUserInfo & UserInfo, tagSocketInfo &SocketInfo)
{
	//��������
	auto pClubUserItem = m_StockClubUser.SearchStockItem(UserInfo.dwUserID);
	if (pClubUserItem == NULL)
	{
		pClubUserItem = m_StockClubUser.CreateStockItem(UserInfo.dwUserID);		
	}

	//��������
	if (pClubUserItem != NULL)
	{
		//���Ա���
		CopyMemory(&pClubUserItem->m_UserInfo, &UserInfo, sizeof(UserInfo));
		CopyMemory(&pClubUserItem->m_SocketInfo, &SocketInfo, sizeof(SocketInfo));
	}

	return pClubUserItem;
}

//////////////////////////////////////////////////////////////////////////////////
