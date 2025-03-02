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
	//��������
	ResetData();

	//ɾ������
	m_StockClubItem.DeleteAllItem();
	
	return;
}

//��������
VOID CGlobalInfoManager::ResetData()
{
	__super::ResetData();

	//�Ƴ�����
	m_StockClubItem.RemoveAllItem();

	return;
}
//ɾ������
bool CGlobalInfoManager::DeleteClubItem(WORD wServiceID)
{
	return m_StockClubItem.RemoveStockItem(wServiceID)!=NULL;
}

//�������
CGlobalClubItem* CGlobalInfoManager::ActiveClubItem(DWORD dwSocketID, tagServiceItem& ServiceItem)
{
	auto pGlobalClubItem = m_StockClubItem.CreateStockItem(ServiceItem.wServiceID);
	if (pGlobalClubItem != NULL)
	{
		//���ñ���
		pGlobalClubItem->m_dwSocketID = dwSocketID;
		pGlobalClubItem->m_ServiceItem = ServiceItem;
	}

	return pGlobalClubItem;
}

//Ѱ�Ҳ���
CGlobalClubItem* CGlobalInfoManager::SearchClubItem(WORD wServiceID)
{
	return m_StockClubItem.SearchStockItem(wServiceID);
}

//ö�ٲ���
CGlobalClubItem* CGlobalInfoManager::EnumClubItem(CGlobalClubItem* pGlobalClubItem)
{
	if (pGlobalClubItem == NULL)
	{
		return m_StockClubItem.GetHeadStockItem();
	}

	return m_StockClubItem.GetNextStockItem(pGlobalClubItem);
}
//////////////////////////////////////////////////////////////////////////
