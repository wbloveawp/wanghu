#include "StdAfx.h"
#include "GlobalInfoManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalClubItem::CGlobalClubItem()
{
}

//析构函数
CGlobalClubItem::~CGlobalClubItem()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalInfoManager::CGlobalInfoManager()
{
	m_StockClubItem.InitHashTable(11);
}
	
//析构函数
CGlobalInfoManager::~CGlobalInfoManager()
{
	m_StockClubItem.DeleteAllItem();
}

//重置数据
VOID CGlobalInfoManager::ResetData()
{
	m_StockClubItem.RemoveAllItem();
}

//删除茶舍
bool CGlobalInfoManager::DeleteClubItem(WORD wServiceID)
{
	return m_StockClubItem.RemoveStockItem(wServiceID)!=NULL;
}
	
//激活茶舍
CGlobalClubItem* CGlobalInfoManager::ActiveClubItem(DWORD dwSocketID, tagServiceItem& ServiceItem)
{
	auto pGloablClubItem = m_StockClubItem.CreateStockItem(ServiceItem.wServiceID);
	if (pGloablClubItem != NULL)
	{
		pGloablClubItem->m_dwSocketID = dwSocketID;
		CopyMemory(&pGloablClubItem->m_ServiceItem, &ServiceItem,sizeof(ServiceItem));
	}

	return pGloablClubItem;
}

//寻找茶舍
CGlobalClubItem* CGlobalInfoManager::SearchClubItem(WORD wServiceID)
{
	return m_StockClubItem.SearchStockItem(wServiceID);
}

//枚举网关
CGlobalClubItem* CGlobalInfoManager::EnumClubItem(CGlobalClubItem* pGlobalClubItem)
{
	if (pGlobalClubItem == NULL)
	{
		return m_StockClubItem.GetHeadStockItem();
	}
	
	return m_StockClubItem.GetNextStockItem(pGlobalClubItem);	
}

//////////////////////////////////////////////////////////////////////////
