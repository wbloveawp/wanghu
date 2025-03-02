#include "Stdafx.h"
#include "StockDataManager.h"

/////////////////////////////////////////////////////////////////////////////////

//静态变量
CStockDataManager* CStockDataManager::m_pStockDataManager = NULL;	//对象指针

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CStockDataManager::CStockDataManager()
{
	//设置对象
	ASSERT(m_pStockDataManager == NULL);
	if (m_pStockDataManager == NULL) m_pStockDataManager = this;

	return;
}

//析构函数
CStockDataManager::~CStockDataManager()
{
	//重置对象
	DeleteGlobalData();

	//设置对象
	ASSERT(m_pStockDataManager == this);
	if (m_pStockDataManager == this) m_pStockDataManager = NULL;

	return;
}

//重置数据
VOID CStockDataManager::ResetGlobalData()
{
	//回收数据
	m_StockItemBuffer.Append(m_StockItemActive);
	m_StockItemActive.RemoveAll();

	return;
}

//删除数据
VOID CStockDataManager::DeleteGlobalData()
{
	//回收数据
	m_StockItemBuffer.Append(m_StockItemActive);
	m_StockItemActive.RemoveAll();

	//删除数据	
	for (auto i = 0; i < m_StockItemBuffer.GetCount(); i++)
	{
		tagStockItem* pStockItem = m_StockItemBuffer[i];
		if (pStockItem)
		{
			SafeDelete(pStockItem);			
		}
	}

	//删除数据
	m_StockItemBuffer.RemoveAll();
}

//删除库存
VOID CStockDataManager::DeleteStockItem(WORD wStockID)
{
	//库存信息
	tagStockItem* pStockItem = NULL;

	//移除库存
	for (auto i = 0; i < m_StockItemActive.GetCount(); i++)
	{
		if (m_StockItemActive[i]->wStockID == wStockID)
		{
			pStockItem = m_StockItemActive[i];
			m_StockItemActive.RemoveAt(i);
			break;
		}
	}

	//缓冲库存
	if (pStockItem != NULL)
	{
		ZeroMemory(pStockItem,sizeof(tagStockItem));
		m_StockItemBuffer.Add(pStockItem);
	}
}

//添加库存
tagStockItem* CStockDataManager::AppendStockItem(tagStockItem& StockItem)
{
	//查找库存
	auto pStockItem = SearchStockItem(StockItem.wStockID);
	if (pStockItem == NULL)
	{
		//创建库存
		pStockItem = ActiveStockItem();
	}

	//拷贝库存
	if (pStockItem != NULL)
	{
		CopyMemory(pStockItem,&StockItem,sizeof(tagStockItem));
	}

	return pStockItem;
}

//寻找库存
tagStockItem* CStockDataManager::EnumStockItem(WORD wStockIndex)
{
	//校验参数
	//ASSERT(wStockIndex < m_StockItemActive.GetCount());
	if (wStockIndex >= m_StockItemActive.GetCount()) return false;

	return m_StockItemActive[wStockIndex];
}

//寻找库存
tagStockItem * CStockDataManager::SearchStockItem(WORD wStockID)
{
	//查找库存
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

//寻找库存
tagStockItem* CStockDataManager::SearchStockItem(WORD wStockKind, WORD wKindID, WORD wStockSubtype)
{
	//查找库存
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

//创建库存
tagStockItem* CStockDataManager::ActiveStockItem()
{
	//变量定义
	tagStockItem* pStockItem = NULL;

	//查找缓存
	if (m_StockItemBuffer.GetCount() > 0)
	{
		auto nCount = m_StockItemBuffer.GetCount();
		pStockItem = m_StockItemBuffer[nCount-1];
		m_StockItemBuffer.RemoveAt(nCount-1);
		m_StockItemActive.Add(pStockItem);

		return pStockItem;
	}
	
	//创建对象
	try
	{
		pStockItem = new tagStockItem();
		if (pStockItem == NULL) throw TEXT("内存不足，库存对象创建失败！");

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
