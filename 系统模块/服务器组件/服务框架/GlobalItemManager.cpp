#include "StdAfx.h"
#include "GlobalItemManager.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalItemManager::CGlobalItemManager()
{
	//��������
	m_StockGateItem.InitHashTable(PRIME_GATE);
	m_StockUserItem.InitHashTable(PRIME_PLATFORM_USER);	
	m_StockServerItem.InitHashTable(PRIME_SERVER);

	return;
}

//��������
CGlobalItemManager::~CGlobalItemManager()
{
	//ɾ������
	m_StockGateItem.DeleteAllItem();
	m_StockUserItem.DeleteAllItem();	
	m_StockServerItem.DeleteAllItem();
	
	return;
}

//��������
VOID CGlobalItemManager::ResetData()
{
	//�Ƴ�����
	m_StockGateItem.RemoveAllItem();
	m_StockUserItem.RemoveAllItem();	
	m_StockServerItem.RemoveAllItem();

	return;
}

//�����û�
CGlobalUserItem * CGlobalItemManager::ActiveUserItem(tagGlobalUserInfo& GlobalUserInfo)
{
	//�����û�
	auto pGlobalUserItem = m_StockUserItem.SearchStockItem(GlobalUserInfo.dwUserID);
	if (pGlobalUserItem == NULL)
	{
		pGlobalUserItem = m_StockUserItem.CreateStockItem(GlobalUserInfo.dwUserID);
	}

	//�����û�
	if (pGlobalUserItem!=NULL)
	{
		//�û�����
		pGlobalUserItem->m_cbGender = GlobalUserInfo.cbGender;
		pGlobalUserItem->m_dwUserID= GlobalUserInfo.dwUserID;
		pGlobalUserItem->m_dwGameID = GlobalUserInfo.dwGameID;
		StringCchCopy(pGlobalUserItem->m_szNickName, CountArray(pGlobalUserItem->m_szNickName), GlobalUserInfo.szNickName);

		//ͷ����Ϣ
		pGlobalUserItem->m_wFaceID = GlobalUserInfo.wFaceID;
		pGlobalUserItem->m_dwCustomID = GlobalUserInfo.dwCustomID;

		//������Ϣ
		pGlobalUserItem->m_wGateID = GlobalUserInfo.SocketInfo.wGateID;
		pGlobalUserItem->m_dwTokenID= GlobalUserInfo.SocketInfo.dwTokenID;		

		//״̬��Ϣ
		pGlobalUserItem->m_cbShutUpStatus = GlobalUserInfo.cbShutUpStatus;
		pGlobalUserItem->m_dwShutUpOverTime= GlobalUserInfo.dwShutUpOverTime;
	}

	return pGlobalUserItem;
}


//ɾ���û�
bool CGlobalItemManager::DeleteUserItem(DWORD dwUserID)
{
	auto pGlobalUserItem = m_StockUserItem.SearchStockItem(dwUserID);
	if (pGlobalUserItem != NULL)
	{
		pGlobalUserItem->ResetData();
		m_StockUserItem.RemoveStockItem(dwUserID);

		return true;
	}

	return false;
}

//��������
CGlobalGateItem * CGlobalItemManager::ActiveGateItem(DWORD dwSocketID, tagServiceItem & GateItem)
{
	//��������
	auto pGlobalGateItem = m_StockGateItem.SearchStockItem(GateItem.wServiceID);
	if (pGlobalGateItem == NULL)
	{
		pGlobalGateItem = m_StockGateItem.CreateStockItem(GateItem.wServiceID);
	}

	//��������
	if (pGlobalGateItem != NULL)
	{
		pGlobalGateItem->m_dwSocketID = dwSocketID;
		pGlobalGateItem->m_ServiceItem = GateItem;
	}

	return pGlobalGateItem;
}

//ɾ������
bool CGlobalItemManager::DeleteGateItem(WORD wGateID)
{
	auto pGlobalGateItem = m_StockGateItem.SearchStockItem(wGateID);
	if (pGlobalGateItem != NULL)
	{
		pGlobalGateItem->ResetData();
		m_StockGateItem.RemoveStockItem(wGateID);

		return true;
	}

	return false;
}


//�����
CGlobalServerItem* CGlobalItemManager::ActiveServerItem(DWORD dwSocketID, tagGameServer& GameServer)
{
	//Ѱ�ҷ���
	auto pGlobalServerItem = m_StockServerItem.SearchStockItem(GameServer.wServerID);
	if (pGlobalServerItem == NULL)
	{
		pGlobalServerItem = m_StockServerItem.CreateStockItem(GameServer.wServerID);
	}

	//���÷���
	if (pGlobalServerItem != NULL)
	{
		//���ñ���
		pGlobalServerItem->m_dwSocketID = dwSocketID;
		pGlobalServerItem->m_GameServer = GameServer;		
	}

	return pGlobalServerItem;
}

//ɾ������
bool CGlobalItemManager::DeleteServerItem(WORD wServerID)
{
	auto pGlobalServerItem = m_StockServerItem.SearchStockItem(wServerID);
	if (pGlobalServerItem != NULL)
	{
		pGlobalServerItem->ResetData();
		m_StockServerItem.RemoveStockItem(wServerID);

		return true;
	}

	return false;
}

//Ѱ���û�
CGlobalUserItem * CGlobalItemManager::SearchUserItem(DWORD dwUserID)
{
	return m_StockUserItem.SearchStockItem(dwUserID);
}

//Ѱ������
CGlobalGateItem * CGlobalItemManager::SearchGateItem(WORD wGateID)
{
	return m_StockGateItem.SearchStockItem(wGateID);
}

//Ѱ�ҷ���
CGlobalServerItem* CGlobalItemManager::SearchServerItem(WORD wServerID)
{
	return m_StockServerItem.SearchStockItem(wServerID);
}

//ö���û�
CGlobalUserItem * CGlobalItemManager::EnumUserItem(CGlobalUserItem* pGlobalUserItem)
{
	if (pGlobalUserItem == NULL)
	{
		return m_StockUserItem.GetHeadStockItem();
	}

	return m_StockUserItem.GetNextStockItem(pGlobalUserItem);
}

//ö������
CGlobalGateItem * CGlobalItemManager::EnumGateItem(CGlobalGateItem* pGlobalGateItem)
{
	if (pGlobalGateItem == NULL)
	{
		return m_StockGateItem.GetHeadStockItem();
	}

	return m_StockGateItem.GetNextStockItem(pGlobalGateItem);
}

//ö�ٷ���
CGlobalServerItem* CGlobalItemManager::EnumServerItem(CGlobalServerItem* pGlobalServerItem)
{
	if (pGlobalServerItem == NULL)
	{
		return m_StockServerItem.GetHeadStockItem();
	}

	return m_StockServerItem.GetNextStockItem(pGlobalServerItem);
}

//////////////////////////////////////////////////////////////////////////
