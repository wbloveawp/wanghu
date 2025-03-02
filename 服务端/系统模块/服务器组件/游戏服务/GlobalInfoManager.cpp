#include "StdAfx.h"
#include "GlobalInfoManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalClubItem::CGlobalClubItem()
{
}

//��������
CGlobalClubItem::~CGlobalClubItem()
{
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalInfoManager::CGlobalInfoManager()
{
	m_StockClubItem.InitHashTable(11);
}
	
//��������
CGlobalInfoManager::~CGlobalInfoManager()
{
	m_StockClubItem.DeleteAllItem();
}

//��������
VOID CGlobalInfoManager::ResetData()
{
	m_StockClubItem.RemoveAllItem();
}

//ɾ������
bool CGlobalInfoManager::DeleteClubItem(WORD wServiceID)
{
	return m_StockClubItem.RemoveStockItem(wServiceID)!=NULL;
}
	
//�������
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

//Ѱ�Ҳ���
CGlobalClubItem* CGlobalInfoManager::SearchClubItem(WORD wServiceID)
{
	return m_StockClubItem.SearchStockItem(wServiceID);
}

//ö������
CGlobalClubItem* CGlobalInfoManager::EnumClubItem(CGlobalClubItem* pGlobalClubItem)
{
	if (pGlobalClubItem == NULL)
	{
		return m_StockClubItem.GetHeadStockItem();
	}
	
	return m_StockClubItem.GetNextStockItem(pGlobalClubItem);	
}

//////////////////////////////////////////////////////////////////////////
