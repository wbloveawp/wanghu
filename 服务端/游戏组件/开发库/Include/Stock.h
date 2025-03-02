#ifndef STOCK_HEAD_FILE
#define STOCK_HEAD_FILE

#pragma once

#include "Array.h"

//////////////////////////////////////////////////////////////////////////////////

//类型说明
template <class TYPE> class CWHStock;
template <class TYPE> class CWHStockList;
template <class KEY, class TYPE> class CWHStockData;

//////////////////////////////////////////////////////////////////////////////////

//仓库数据
template <class TYPE> class CWHStock : public TYPE
{
	//链表指针
public:
	CWHStock *						m_pStockItemLast;					//链表指针
	CWHStock *						m_pStockItemNext;					//链表指针

	//函数定义
public:
	//构造函数
	CWHStock();
	//析构函数
	virtual ~CWHStock();

	//对象管理
public:
	//获取对象
	TYPE * GetLastStockItem();
	//获取对象
	TYPE * GetNextStockItem();
};

//////////////////////////////////////////////////////////////////////////////////

//仓库管理
template <class TYPE> class CWHStockList
{
	//类型说明
	typedef CWHStock<TYPE> CWHStockItem;

	//链表数据
protected:
	DWORD							m_dwItemCount;						//子项数目
	CWHStockItem *					m_pStockItemHead;					//链表数据
	CWHStockItem *					m_pStockItemTail;					//链表数据
	CWHStockItem *					m_pStockItemIdle;					//链表数据

	//函数定义
public:
	//构造函数
	CWHStockList();
	//析构函数
	virtual ~CWHStockList();

	//辅助函数
public:
	//获取数目
	DWORD GetItemCount();

	//数据函数
public:
	//获取对象
	TYPE * GetHeadStockItem();
	//获取对象
	TYPE * GetTailStockItem();
	//获取对象
	TYPE * GetLastStockItem(TYPE * pCurrentItem);
	//获取对象
	TYPE * GetNextStockItem(TYPE * pCurrentItem);

	//管理函数
public:
	//清理对象
	VOID DeleteAllItem();
	//删除对象
	VOID RemoveAllItem();
	//创建对象
	TYPE * CreateStockItem();
	//附加对象
	TYPE * AttachStockItem(TYPE * pCurrentItem);
	//分离对象
	TYPE * DetachStockItem(TYPE * pCurrentItem);
	//删除对象
	TYPE * RemoveStockItem(TYPE * pCurrentItem);
};

//////////////////////////////////////////////////////////////////////////////////

//仓库管理
template <class KEY, class TYPE> class CWHStockData
{
	//类型说明
	typedef CWHStock<TYPE> CWHStockItem;
	typedef CMap<KEY, KEY, CWHStockItem *, CWHStockItem * &> CWHStockItemMap;

	//链表数据
protected:
	bool							m_bInitHashTable;					//配置标志
	CWHStockItem *					m_pStockItemHead;					//链表数据
	CWHStockItem *					m_pStockItemTail;					//链表数据
	CWHStockItem *					m_pStockItemIdle;					//链表数据

	//仓库数据
protected:
	CWHStockItemMap					m_StockItemAssist;					//数据索引

	//函数定义
public:
	//构造函数
	CWHStockData();
	//析构函数
	virtual ~CWHStockData();

	//配置函数
public:
	//设置因子
	VOID InitHashTable(UINT nHashCount);

	//信息函数
public:
	//获取数目
	INT_PTR GetItemCount();
	//查找对象
	TYPE * SearchStockItem(KEY rItemKey);

	//数据函数
public:
	//获取对象
	TYPE * GetHeadStockItem();
	//获取对象
	TYPE * GetTailStockItem();
	//获取对象
	TYPE * GetLastStockItem(TYPE * pCurrentItem);
	//获取对象
	TYPE * GetNextStockItem(TYPE * pCurrentItem);

	//管理函数
public:
	//清理对象
	VOID DeleteAllItem();
	//删除对象
	VOID RemoveAllItem();
	//创建对象
	TYPE * CreateStockItem(KEY rItemKey);
	//删除对象
	TYPE * RemoveStockItem(KEY rItemKey);
};

//////////////////////////////////////////////////////////////////////////////////

//构造函数
template<class TYPE> CWHStock<TYPE>::CWHStock()
{
	//设置变量
	m_pStockItemLast = NULL;
	m_pStockItemNext = NULL;

	return;
}

//析构函数
template<class TYPE> CWHStock<TYPE>::~CWHStock()
{
}

//获取对象
template<class TYPE> AFX_INLINE TYPE * CWHStock<TYPE>::GetLastStockItem()
{
	return m_pStockItemLast;
}

//获取对象
template<class TYPE> AFX_INLINE TYPE * CWHStock<TYPE>::GetNextStockItem()
{
	return m_pStockItemNext;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
template<class TYPE> CWHStockList<TYPE>::CWHStockList()
{
	//设置变量
	m_dwItemCount = 0L;
	m_pStockItemHead = NULL;
	m_pStockItemTail = NULL;
	m_pStockItemIdle = NULL;

	return;
}

//析构函数
template<class TYPE> CWHStockList<TYPE>::~CWHStockList()
{
	//清空数据
	DeleteAllItem();

	return;
}

//获取数目
template<class TYPE> AFX_INLINE DWORD CWHStockList<TYPE>::GetItemCount()
{
	return m_dwItemCount;
}

//获取数据
template<class TYPE> AFX_INLINE TYPE * CWHStockList<TYPE>::GetHeadStockItem()
{
	return m_pStockItemHead;
}

//获取数据
template<class TYPE> AFX_INLINE TYPE * CWHStockList<TYPE>::GetTailStockItem()
{
	return m_pStockItemTail;
}

//查找数据
template<class TYPE> TYPE * CWHStockList<TYPE>::GetLastStockItem(TYPE * pCurrentItem)
{
	//效验参数
	ASSERT(pCurrentItem != NULL);
	if (pCurrentItem != NULL) return ((CWHStockItem *)pCurrentItem)->GetLastStockItem();

	return NULL;
}

//查找数据
template<class TYPE> TYPE * CWHStockList<TYPE>::GetNextStockItem(TYPE * pCurrentItem)
{
	//效验参数
	ASSERT(pCurrentItem != NULL);
	if (pCurrentItem != NULL) return ((CWHStockItem *)pCurrentItem)->GetNextStockItem();

	return NULL;
}

//清空对象
template<class TYPE> VOID CWHStockList<TYPE>::DeleteAllItem()
{
	//设置变量
	m_dwItemCount = 0L;
	m_pStockItemTail = NULL;

	//删除链表
	while (m_pStockItemIdle != NULL)
	{
		//获取对象
		CWHStockItem * pStockItemDelete = m_pStockItemIdle;

		//设置链接
		m_pStockItemIdle = m_pStockItemIdle->m_pStockItemNext;

		//删除对象
		SafeDelete(pStockItemDelete);
	}

	//删除链表
	while (m_pStockItemHead != NULL)
	{
		//获取对象
		CWHStockItem * pStockItemDelete = m_pStockItemHead;

		//设置链接
		m_pStockItemHead = m_pStockItemHead->m_pStockItemNext;

		//删除对象
		SafeDelete(pStockItemDelete);
	}

	return;
}

//删除对象
template<class TYPE> VOID CWHStockList<TYPE>::RemoveAllItem()
{
	//设置变量
	m_dwItemCount = 0L;
	m_pStockItemTail = NULL;

	//删除链表
	while (m_pStockItemHead != NULL)
	{
		//获取对象
		CWHStockItem * pStockItemBuffer = m_pStockItemIdle;
		CWHStockItem * pStockItemRemove = m_pStockItemHead;

		//设置链接
		m_pStockItemIdle = pStockItemRemove;
		m_pStockItemHead = m_pStockItemHead->m_pStockItemNext;

		//重置对象
		pStockItemRemove->m_pStockItemLast = NULL;
		pStockItemRemove->m_pStockItemNext = pStockItemBuffer;
	}

	return;
}

//附加对象
template<class TYPE> TYPE * CWHStockList<TYPE>::AttachStockItem(TYPE * pCurrentItem)
{
	//效验参数
	ASSERT(pCurrentItem != NULL);
	if (pCurrentItem == NULL) return NULL;

	//变量定义
	CWHStockItem * pStockItemAttach = (CWHStockItem *)pCurrentItem;

	//设置对象
	pStockItemAttach->m_pStockItemLast = NULL;
	pStockItemAttach->m_pStockItemNext = m_pStockItemHead;

	//设置链表
	if (m_pStockItemTail == NULL)
	{
		m_pStockItemTail = pStockItemAttach;
	}

	//设置链表
	if (m_pStockItemHead != NULL)
	{
		m_pStockItemHead->m_pStockItemLast = pStockItemAttach;
	}

	//设置链表
	m_dwItemCount++;
	m_pStockItemHead = pStockItemAttach;

	return pStockItemAttach;
}

//分离对象
template<class TYPE> TYPE * CWHStockList<TYPE>::DetachStockItem(TYPE * pCurrentItem)
{
	//效验参数
	ASSERT(pCurrentItem != NULL);
	if (pCurrentItem == NULL) return NULL;

	//变量定义
	CWHStockItem * pStockItemDetach = (CWHStockItem *)pCurrentItem;

	//变量定义
	CWHStockItem * pStockItemLast = pStockItemDetach->m_pStockItemLast;
	CWHStockItem * pStockItemNext = pStockItemDetach->m_pStockItemNext;

	//设置数目
	ASSERT(m_dwItemCount > 0L);
	if (m_dwItemCount > 0L) m_dwItemCount--;

	//重置对象
	pStockItemDetach->m_pStockItemLast = NULL;
	pStockItemDetach->m_pStockItemNext = NULL;

	//设置链表
	if (pStockItemLast != NULL)
	{
		pStockItemLast->m_pStockItemNext = pStockItemNext;
	}

	//设置链表
	if (pStockItemNext != NULL)
	{
		pStockItemNext->m_pStockItemLast = pStockItemLast;
	}

	//设置链表
	if (m_pStockItemHead == pStockItemDetach)
	{
		m_pStockItemHead = pStockItemNext;
	}

	//设置链表
	if (m_pStockItemTail == pStockItemDetach)
	{
		m_pStockItemTail = pStockItemLast;
	}

	return pStockItemDetach;
}

//创建对象
template<class TYPE> TYPE * CWHStockList<TYPE>::CreateStockItem()
{
	//变量定义
	CWHStockItem * pStockItem = NULL;

	//获取指针
	if (m_pStockItemIdle == NULL)
	{
		//创建对象
		try
		{
			pStockItem = new CWHStockItem;
			if (pStockItem == NULL) throw TEXT("创建对象失败");
		}
		catch (...)
		{
			ASSERT(FALSE);
			return NULL;
		}
	}
	else
	{
		//获取对象
		pStockItem = m_pStockItemIdle;

		//设置链表
		m_pStockItemIdle = m_pStockItemIdle->m_pStockItemNext;
	}

	//结果判断
	if (pStockItem != NULL)
	{
		//设置链表
		pStockItem->m_pStockItemLast = NULL;
		pStockItem->m_pStockItemNext = m_pStockItemHead;

		//设置链表
		if (m_pStockItemTail == NULL)
		{
			m_pStockItemTail = pStockItem;
		}

		//设置链表
		if (m_pStockItemHead != NULL)
		{
			m_pStockItemHead->m_pStockItemLast = pStockItem;
		}

		//设置链表
		m_dwItemCount++;
		m_pStockItemHead = pStockItem;

		return pStockItem;
	}

	//错误断言
	ASSERT(FALSE);

	return NULL;
}

//删除对象
template<class TYPE> TYPE * CWHStockList<TYPE>::RemoveStockItem(TYPE * pCurrentItem)
{
	//效验参数
	ASSERT(pCurrentItem != NULL);
	if (pCurrentItem == NULL) return NULL;

	//变量定义
	CWHStockItem * pStockItemIdle = m_pStockItemIdle;
	CWHStockItem * pStockItemRemove = (CWHStockItem *)pCurrentItem;

	//变量定义
	CWHStockItem * pStockItemLast = pStockItemRemove->m_pStockItemLast;
	CWHStockItem * pStockItemNext = pStockItemRemove->m_pStockItemNext;

	//设置链表
	m_pStockItemIdle = pStockItemRemove;

	//设置数目
	ASSERT(m_dwItemCount > 0L);
	if (m_dwItemCount > 0L) m_dwItemCount--;

	//重置对象
	pStockItemRemove->m_pStockItemLast = NULL;
	pStockItemRemove->m_pStockItemNext = pStockItemIdle;

	//设置链表
	if (pStockItemLast != NULL)
	{
		pStockItemLast->m_pStockItemNext = pStockItemNext;
	}

	//设置链表
	if (pStockItemNext != NULL)
	{
		pStockItemNext->m_pStockItemLast = pStockItemLast;
	}

	//设置链表
	if (m_pStockItemHead == pStockItemRemove)
	{
		m_pStockItemHead = pStockItemNext;
	}

	//设置链表
	if (m_pStockItemTail == pStockItemRemove)
	{
		m_pStockItemTail = pStockItemLast;
	}

	return pStockItemRemove;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
template<class KEY, class TYPE> CWHStockData<KEY, TYPE>::CWHStockData()
{
	//设置变量
	m_pStockItemHead = NULL;
	m_pStockItemTail = NULL;
	m_pStockItemIdle = NULL;

	//设置变量
	m_bInitHashTable = false;

	return;
}

//析构函数
template<class KEY, class TYPE> CWHStockData<KEY, TYPE>::~CWHStockData()
{
	//清空数据
	DeleteAllItem();

	return;
}

//设置因子
template<class KEY, class TYPE> VOID CWHStockData<KEY, TYPE>::InitHashTable(UINT nHashCount)
{
	//设置变量
	m_bInitHashTable = true;

	//设置因子
	m_StockItemAssist.InitHashTable(nHashCount);

	return;
}

//获取数目
template<class KEY, class TYPE> AFX_INLINE INT_PTR CWHStockData<KEY, TYPE>::GetItemCount()
{
	return m_StockItemAssist.GetCount();
}

//查找数据
template<class KEY, class TYPE> TYPE * CWHStockData<KEY, TYPE>::SearchStockItem(KEY rItemKey)
{
	//查找数据
	CWHStockItem * pStockItem = NULL;
	m_StockItemAssist.Lookup(rItemKey, pStockItem);

	return pStockItem;
}

//获取数据
template<class KEY, class TYPE> AFX_INLINE TYPE * CWHStockData<KEY, TYPE>::GetHeadStockItem()
{
	return m_pStockItemHead;
}

//获取数据
template<class KEY, class TYPE> AFX_INLINE TYPE * CWHStockData<KEY, TYPE>::GetTailStockItem()
{
	return m_pStockItemTail;
}

//查找数据
template<class KEY, class TYPE> TYPE * CWHStockData<KEY, TYPE>::GetLastStockItem(TYPE * pCurrentItem)
{
	//效验参数
	ASSERT(pCurrentItem != NULL);
	if (pCurrentItem != NULL) return ((CWHStockItem *)pCurrentItem)->GetLastStockItem();

	return NULL;
}

//查找数据
template<class KEY, class TYPE> TYPE * CWHStockData<KEY, TYPE>::GetNextStockItem(TYPE * pCurrentItem)
{
	//效验参数
	ASSERT(pCurrentItem != NULL);
	if (pCurrentItem != NULL) return ((CWHStockItem *)pCurrentItem)->GetNextStockItem();

	return NULL;
}

//清空对象
template<class KEY, class TYPE> VOID CWHStockData<KEY, TYPE>::DeleteAllItem()
{
	//设置变量
	m_pStockItemTail = NULL;

	//删除索引
	m_StockItemAssist.RemoveAll();

	//删除链表
	while (m_pStockItemIdle != NULL)
	{
		//获取对象
		CWHStockItem * pStockItemDelete = m_pStockItemIdle;

		//设置链接
		m_pStockItemIdle = m_pStockItemIdle->m_pStockItemNext;

		//删除对象
		SafeDelete(pStockItemDelete);
	}

	//删除链表
	while (m_pStockItemHead != NULL)
	{
		//获取对象
		CWHStockItem * pStockItemDelete = m_pStockItemHead;

		//设置链接
		m_pStockItemHead = m_pStockItemHead->m_pStockItemNext;

		//删除对象
		SafeDelete(pStockItemDelete);
	}

	return;
}

//删除对象
template<class KEY, class TYPE> VOID CWHStockData<KEY, TYPE>::RemoveAllItem()
{
	//设置变量
	m_pStockItemTail = NULL;

	//删除索引
	m_StockItemAssist.RemoveAll();

	//删除链表
	while (m_pStockItemHead != NULL)
	{
		//获取对象
		CWHStockItem * pStockItemBuffer = m_pStockItemIdle;
		CWHStockItem * pStockItemRemove = m_pStockItemHead;

		//设置链接
		m_pStockItemIdle = pStockItemRemove;
		m_pStockItemHead = m_pStockItemHead->m_pStockItemNext;

		//重置对象
		pStockItemRemove->m_pStockItemLast = NULL;
		pStockItemRemove->m_pStockItemNext = pStockItemBuffer;
	}

	return;
}

//创建对象
template<class KEY, class TYPE> TYPE * CWHStockData<KEY, TYPE>::CreateStockItem(KEY rItemKey)
{
	//效验状态
	ASSERT(m_bInitHashTable == true);
	if (m_bInitHashTable == false) return NULL;

	//查找对象
	CWHStockItem * pStockItem = NULL;
	m_StockItemAssist.Lookup(rItemKey, pStockItem);

	//创建对象
	if (pStockItem == NULL)
	{
		//获取指针
		if (m_pStockItemIdle == NULL)
		{
			//创建对象
			try
			{
				pStockItem = new CWHStockItem;
				if (pStockItem == NULL) throw TEXT("创建对象失败");
			}
			catch (...)
			{
				ASSERT(FALSE);
				return NULL;
			}
		}
		else
		{
			//获取对象
			pStockItem = m_pStockItemIdle;

			//设置链表
			m_pStockItemIdle = m_pStockItemIdle->m_pStockItemNext;
		}

		//结果判断
		if (pStockItem != NULL)
		{
			//设置链表
			pStockItem->m_pStockItemLast = NULL;
			pStockItem->m_pStockItemNext = m_pStockItemHead;

			//设置链表
			if (m_pStockItemTail == NULL)
			{
				m_pStockItemTail = pStockItem;
			}

			//设置链表
			if (m_pStockItemHead != NULL)
			{
				m_pStockItemHead->m_pStockItemLast = pStockItem;
			}

			//设置链表
			m_pStockItemHead = pStockItem;

			//设置索引
			m_StockItemAssist[rItemKey] = pStockItem;

			return pStockItem;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return NULL;
}

//删除对象
template<class KEY, class TYPE> TYPE * CWHStockData<KEY, TYPE>::RemoveStockItem(KEY rItemKey)
{
	//查找对象
	CWHStockItem * pStockItem = NULL;
	m_StockItemAssist.Lookup(rItemKey, pStockItem);

	//对象判断
	if (pStockItem != NULL)
	{
		//变量定义
		CWHStockItem * pStockItemIdle = m_pStockItemIdle;
		CWHStockItem * pStockItemLast = pStockItem->m_pStockItemLast;
		CWHStockItem * pStockItemNext = pStockItem->m_pStockItemNext;

		//删除索引
		if (m_StockItemAssist.RemoveKey(rItemKey) == TRUE)
		{
			//设置链表
			m_pStockItemIdle = pStockItem;

			//重置对象
			pStockItem->m_pStockItemLast = NULL;
			pStockItem->m_pStockItemNext = pStockItemIdle;

			//设置链表
			if (pStockItemLast != NULL)
			{
				pStockItemLast->m_pStockItemNext = pStockItemNext;
			}

			//设置链表
			if (pStockItemNext != NULL)
			{
				pStockItemNext->m_pStockItemLast = pStockItemLast;
			}

			//设置链表
			if (m_pStockItemHead == pStockItem)
			{
				m_pStockItemHead = pStockItemNext;
			}

			//设置链表
			if (m_pStockItemTail == pStockItem)
			{
				m_pStockItemTail = pStockItemLast;
			}

			return pStockItem;
		}
	}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////

#endif