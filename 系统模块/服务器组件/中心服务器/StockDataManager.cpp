#include "Stdafx.h"
#include "StockDataManager.h"

/////////////////////////////////////////////////////////////////////////////////

//��̬����
CStockDataManager* CStockDataManager::m_pStockDataManager = NULL;	//����ָ��

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CStockDataManager::CStockDataManager()
{
	//���ö���
	ASSERT(m_pStockDataManager == NULL);
	if (m_pStockDataManager == NULL) m_pStockDataManager = this;

	return;
}

//��������
CStockDataManager::~CStockDataManager()
{
	//���ö���
	DeleteGlobalData();

	//���ö���
	ASSERT(m_pStockDataManager == this);
	if (m_pStockDataManager == this) m_pStockDataManager = NULL;

	return;
}

//��������
VOID CStockDataManager::ResetGlobalData()
{
	//��������
	m_StockItemBuffer.Append(m_StockItemActive);
	m_StockItemActive.RemoveAll();

	return;
}

//ɾ������
VOID CStockDataManager::DeleteGlobalData()
{
	//��������
	m_StockItemBuffer.Append(m_StockItemActive);
	m_StockItemActive.RemoveAll();

	//ɾ������	
	for (auto i = 0; i < m_StockItemBuffer.GetCount(); i++)
	{
		tagStockItem* pStockItem = m_StockItemBuffer[i];
		if (pStockItem)
		{
			SafeDelete(pStockItem);			
		}
	}

	//ɾ������
	m_StockItemBuffer.RemoveAll();
}

//ɾ�����
VOID CStockDataManager::DeleteStockItem(WORD wStockID)
{
	//�����Ϣ
	tagStockItem* pStockItem = NULL;

	//�Ƴ����
	for (auto i = 0; i < m_StockItemActive.GetCount(); i++)
	{
		if (m_StockItemActive[i]->wStockID == wStockID)
		{
			pStockItem = m_StockItemActive[i];
			m_StockItemActive.RemoveAt(i);
			break;
		}
	}

	//������
	if (pStockItem != NULL)
	{
		ZeroMemory(pStockItem,sizeof(tagStockItem));
		m_StockItemBuffer.Add(pStockItem);
	}
}

//��ӿ��
tagStockItem* CStockDataManager::AppendStockItem(tagStockItem& StockItem)
{
	//���ҿ��
	auto pStockItem = SearchStockItem(StockItem.wStockID);
	if (pStockItem == NULL)
	{
		//�������
		pStockItem = ActiveStockItem();
	}

	//�������
	if (pStockItem != NULL)
	{
		CopyMemory(pStockItem,&StockItem,sizeof(tagStockItem));
	}

	return pStockItem;
}

//Ѱ�ҿ��
tagStockItem* CStockDataManager::EnumStockItem(WORD wStockIndex)
{
	//У�����
	//ASSERT(wStockIndex < m_StockItemActive.GetCount());
	if (wStockIndex >= m_StockItemActive.GetCount()) return false;

	return m_StockItemActive[wStockIndex];
}

//Ѱ�ҿ��
tagStockItem * CStockDataManager::SearchStockItem(WORD wStockID)
{
	//���ҿ��
	for (auto i = 0; i < m_StockItemActive.GetCount(); i++)
	{
		tagStockItem* pStockItem = m_StockItemActive[i];
		if (pStockItem && pStockItem->wStockID==wStockID)
		{
			return pStockItem;
		}
	}

	return NULL;
}

//Ѱ�ҿ��
tagStockItem* CStockDataManager::SearchStockItem(WORD wStockKind, WORD wKindID, WORD wStockSubtype)
{
	//���ҿ��
	for (auto i = 0; i < m_StockItemActive.GetCount(); i++)
	{
		tagStockItem* pStockItem = m_StockItemActive[i];
		if (pStockItem == NULL) continue;
		/*if (pStockItem->wStockKind == wStockKind && pStockItem->wKindID == wKindID && pStockItem->wStockSubtype == wStockSubtype)
		{
			return pStockItem;
		}*/
	}

	return NULL;
}

//�������
tagStockItem* CStockDataManager::ActiveStockItem()
{
	//��������
	tagStockItem* pStockItem = NULL;

	//���һ���
	if (m_StockItemBuffer.GetCount() > 0)
	{
		auto nCount = m_StockItemBuffer.GetCount();
		pStockItem = m_StockItemBuffer[nCount-1];
		m_StockItemBuffer.RemoveAt(nCount-1);
		m_StockItemActive.Add(pStockItem);

		return pStockItem;
	}
	
	//��������
	try
	{
		pStockItem = new tagStockItem();
		if (pStockItem == NULL) throw TEXT("�ڴ治�㣬�����󴴽�ʧ�ܣ�");

		ZeroMemory(pStockItem, sizeof(tagStockItem));
		m_StockItemActive.Add(pStockItem);

		return pStockItem;
	}
	catch (...)
	{
		ASSERT (FALSE);
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
