#include "StdAfx.h"
#include "GlobalItemManager.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalItemManager::CGlobalItemManager()
{
	//设置索引
	m_StockGateItem.InitHashTable(PRIME_GATE);
	m_StockUserItem.InitHashTable(PRIME_PLATFORM_USER);	
	m_StockServerItem.InitHashTable(PRIME_SERVER);

	return;
}

//析构函数
CGlobalItemManager::~CGlobalItemManager()
{
	//删除子项
	m_StockGateItem.DeleteAllItem();
	m_StockUserItem.DeleteAllItem();	
	m_StockServerItem.DeleteAllItem();
	
	return;
}

//重置数据
VOID CGlobalItemManager::ResetData()
{
	//移除子项
	m_StockGateItem.RemoveAllItem();
	m_StockUserItem.RemoveAllItem();	
	m_StockServerItem.RemoveAllItem();

	return;
}

//激活用户
CGlobalUserItem * CGlobalItemManager::ActiveUserItem(tagGlobalUserInfo& GlobalUserInfo)
{
	//查找用户
	auto pGlobalUserItem = m_StockUserItem.SearchStockItem(GlobalUserInfo.dwUserID);
	if (pGlobalUserItem == NULL)
	{
		pGlobalUserItem = m_StockUserItem.CreateStockItem(GlobalUserInfo.dwUserID);
	}

	//设置用户
	if (pGlobalUserItem!=NULL)
	{
		//用户属性
		pGlobalUserItem->m_cbGender = GlobalUserInfo.cbGender;
		pGlobalUserItem->m_dwUserID= GlobalUserInfo.dwUserID;
		pGlobalUserItem->m_dwGameID = GlobalUserInfo.dwGameID;
		StringCchCopy(pGlobalUserItem->m_szNickName, CountArray(pGlobalUserItem->m_szNickName), GlobalUserInfo.szNickName);

		//头像信息
		pGlobalUserItem->m_wFaceID = GlobalUserInfo.wFaceID;
		pGlobalUserItem->m_dwCustomID = GlobalUserInfo.dwCustomID;

		//网络信息
		pGlobalUserItem->m_wGateID = GlobalUserInfo.SocketInfo.wGateID;
		pGlobalUserItem->m_dwTokenID= GlobalUserInfo.SocketInfo.dwTokenID;		

		//状态信息
		pGlobalUserItem->m_cbShutUpStatus = GlobalUserInfo.cbShutUpStatus;
		pGlobalUserItem->m_dwShutUpOverTime= GlobalUserInfo.dwShutUpOverTime;
	}

	return pGlobalUserItem;
}


//删除用户
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

//激活网关
CGlobalGateItem * CGlobalItemManager::ActiveGateItem(DWORD dwSocketID, tagServiceItem & GateItem)
{
	//查找网关
	auto pGlobalGateItem = m_StockGateItem.SearchStockItem(GateItem.wServiceID);
	if (pGlobalGateItem == NULL)
	{
		pGlobalGateItem = m_StockGateItem.CreateStockItem(GateItem.wServiceID);
	}

	//设置子项
	if (pGlobalGateItem != NULL)
	{
		pGlobalGateItem->m_dwSocketID = dwSocketID;
		pGlobalGateItem->m_ServiceItem = GateItem;
	}

	return pGlobalGateItem;
}

//删除网关
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


//激活房间
CGlobalServerItem* CGlobalItemManager::ActiveServerItem(DWORD dwSocketID, tagGameServer& GameServer)
{
	//寻找房间
	auto pGlobalServerItem = m_StockServerItem.SearchStockItem(GameServer.wServerID);
	if (pGlobalServerItem == NULL)
	{
		pGlobalServerItem = m_StockServerItem.CreateStockItem(GameServer.wServerID);
	}

	//设置房间
	if (pGlobalServerItem != NULL)
	{
		//设置变量
		pGlobalServerItem->m_dwSocketID = dwSocketID;
		pGlobalServerItem->m_GameServer = GameServer;		
	}

	return pGlobalServerItem;
}

//删除房间
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

//寻找用户
CGlobalUserItem * CGlobalItemManager::SearchUserItem(DWORD dwUserID)
{
	return m_StockUserItem.SearchStockItem(dwUserID);
}

//寻找网关
CGlobalGateItem * CGlobalItemManager::SearchGateItem(WORD wGateID)
{
	return m_StockGateItem.SearchStockItem(wGateID);
}

//寻找房间
CGlobalServerItem* CGlobalItemManager::SearchServerItem(WORD wServerID)
{
	return m_StockServerItem.SearchStockItem(wServerID);
}

//枚举用户
CGlobalUserItem * CGlobalItemManager::EnumUserItem(CGlobalUserItem* pGlobalUserItem)
{
	if (pGlobalUserItem == NULL)
	{
		return m_StockUserItem.GetHeadStockItem();
	}

	return m_StockUserItem.GetNextStockItem(pGlobalUserItem);
}

//枚举网关
CGlobalGateItem * CGlobalItemManager::EnumGateItem(CGlobalGateItem* pGlobalGateItem)
{
	if (pGlobalGateItem == NULL)
	{
		return m_StockGateItem.GetHeadStockItem();
	}

	return m_StockGateItem.GetNextStockItem(pGlobalGateItem);
}

//枚举房间
CGlobalServerItem* CGlobalItemManager::EnumServerItem(CGlobalServerItem* pGlobalServerItem)
{
	if (pGlobalServerItem == NULL)
	{
		return m_StockServerItem.GetHeadStockItem();
	}

	return m_StockServerItem.GetNextStockItem(pGlobalServerItem);
}

//////////////////////////////////////////////////////////////////////////
