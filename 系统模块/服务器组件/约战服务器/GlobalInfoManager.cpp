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
	//重置数据
	ResetData();

	//删除子项
	m_StockClubItem.DeleteAllItem();
	
	return;
}

//重置数据
VOID CGlobalInfoManager::ResetData()
{
	__super::ResetData();

	//移除子项
	m_StockClubItem.RemoveAllItem();

	return;
}
//删除茶舍
bool CGlobalInfoManager::DeleteClubItem(WORD wServiceID)
{
	return m_StockClubItem.RemoveStockItem(wServiceID)!=NULL;
}

//激活茶舍
CGlobalClubItem* CGlobalInfoManager::ActiveClubItem(DWORD dwSocketID, tagServiceItem& ServiceItem)
{
	auto pGlobalClubItem = m_StockClubItem.CreateStockItem(ServiceItem.wServiceID);
	if (pGlobalClubItem != NULL)
	{
		//设置变量
		pGlobalClubItem->m_dwSocketID = dwSocketID;
		pGlobalClubItem->m_ServiceItem = ServiceItem;
	}

	return pGlobalClubItem;
}

//寻找茶舍
CGlobalClubItem* CGlobalInfoManager::SearchClubItem(WORD wServiceID)
{
	return m_StockClubItem.SearchStockItem(wServiceID);
}

//枚举茶社
CGlobalClubItem* CGlobalInfoManager::EnumClubItem(CGlobalClubItem* pGlobalClubItem)
{
	if (pGlobalClubItem == NULL)
	{
		return m_StockClubItem.GetHeadStockItem();
	}

	return m_StockClubItem.GetNextStockItem(pGlobalClubItem);
}
//////////////////////////////////////////////////////////////////////////
