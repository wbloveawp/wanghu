#ifndef STOCK_HEAD_FILE
#define STOCK_HEAD_FILE

#pragma once

#include "Array.h"

//////////////////////////////////////////////////////////////////////////////////

//����˵��
template <class TYPE> class CWHStock;
template <class TYPE> class CWHStockList;
template <class KEY, class TYPE> class CWHStockData;

//////////////////////////////////////////////////////////////////////////////////

//�ֿ�����
template <class TYPE> class CWHStock : public TYPE
{
	//����ָ��
public:
	CWHStock *						m_pStockItemLast;					//����ָ��
	CWHStock *						m_pStockItemNext;					//����ָ��

	//��������
public:
	//���캯��
	CWHStock();
	//��������
	virtual ~CWHStock();

	//�������
public:
	//��ȡ����
	TYPE * GetLastStockItem();
	//��ȡ����
	TYPE * GetNextStockItem();
};

//////////////////////////////////////////////////////////////////////////////////

//�ֿ����
template <class TYPE> class CWHStockList
{
	//����˵��
	typedef CWHStock<TYPE> CWHStockItem;

	//��������
protected:
	DWORD							m_dwItemCount;						//������Ŀ
	CWHStockItem *					m_pStockItemHead;					//��������
	CWHStockItem *					m_pStockItemTail;					//��������
	CWHStockItem *					m_pStockItemIdle;					//��������

	//��������
public:
	//���캯��
	CWHStockList();
	//��������
	virtual ~CWHStockList();

	//��������
public:
	//��ȡ��Ŀ
	DWORD GetItemCount();

	//���ݺ���
public:
	//��ȡ����
	TYPE * GetHeadStockItem();
	//��ȡ����
	TYPE * GetTailStockItem();
	//��ȡ����
	TYPE * GetLastStockItem(TYPE * pCurrentItem);
	//��ȡ����
	TYPE * GetNextStockItem(TYPE * pCurrentItem);

	//������
public:
	//�������
	VOID DeleteAllItem();
	//ɾ������
	VOID RemoveAllItem();
	//��������
	TYPE * CreateStockItem();
	//���Ӷ���
	TYPE * AttachStockItem(TYPE * pCurrentItem);
	//�������
	TYPE * DetachStockItem(TYPE * pCurrentItem);
	//ɾ������
	TYPE * RemoveStockItem(TYPE * pCurrentItem);
};

//////////////////////////////////////////////////////////////////////////////////

//�ֿ����
template <class KEY, class TYPE> class CWHStockData
{
	//����˵��
	typedef CWHStock<TYPE> CWHStockItem;
	typedef CMap<KEY, KEY, CWHStockItem *, CWHStockItem * &> CWHStockItemMap;

	//��������
protected:
	bool							m_bInitHashTable;					//���ñ�־
	CWHStockItem *					m_pStockItemHead;					//��������
	CWHStockItem *					m_pStockItemTail;					//��������
	CWHStockItem *					m_pStockItemIdle;					//��������

	//�ֿ�����
protected:
	CWHStockItemMap					m_StockItemAssist;					//��������

	//��������
public:
	//���캯��
	CWHStockData();
	//��������
	virtual ~CWHStockData();

	//���ú���
public:
	//��������
	VOID InitHashTable(UINT nHashCount);

	//��Ϣ����
public:
	//��ȡ��Ŀ
	INT_PTR GetItemCount();
	//���Ҷ���
	TYPE * SearchStockItem(KEY rItemKey);

	//���ݺ���
public:
	//��ȡ����
	TYPE * GetHeadStockItem();
	//��ȡ����
	TYPE * GetTailStockItem();
	//��ȡ����
	TYPE * GetLastStockItem(TYPE * pCurrentItem);
	//��ȡ����
	TYPE * GetNextStockItem(TYPE * pCurrentItem);

	//������
public:
	//�������
	VOID DeleteAllItem();
	//ɾ������
	VOID RemoveAllItem();
	//��������
	TYPE * CreateStockItem(KEY rItemKey);
	//ɾ������
	TYPE * RemoveStockItem(KEY rItemKey);
};

//////////////////////////////////////////////////////////////////////////////////

//���캯��
template<class TYPE> CWHStock<TYPE>::CWHStock()
{
	//���ñ���
	m_pStockItemLast = NULL;
	m_pStockItemNext = NULL;

	return;
}

//��������
template<class TYPE> CWHStock<TYPE>::~CWHStock()
{
}

//��ȡ����
template<class TYPE> AFX_INLINE TYPE * CWHStock<TYPE>::GetLastStockItem()
{
	return m_pStockItemLast;
}

//��ȡ����
template<class TYPE> AFX_INLINE TYPE * CWHStock<TYPE>::GetNextStockItem()
{
	return m_pStockItemNext;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
template<class TYPE> CWHStockList<TYPE>::CWHStockList()
{
	//���ñ���
	m_dwItemCount = 0L;
	m_pStockItemHead = NULL;
	m_pStockItemTail = NULL;
	m_pStockItemIdle = NULL;

	return;
}

//��������
template<class TYPE> CWHStockList<TYPE>::~CWHStockList()
{
	//�������
	DeleteAllItem();

	return;
}

//��ȡ��Ŀ
template<class TYPE> AFX_INLINE DWORD CWHStockList<TYPE>::GetItemCount()
{
	return m_dwItemCount;
}

//��ȡ����
template<class TYPE> AFX_INLINE TYPE * CWHStockList<TYPE>::GetHeadStockItem()
{
	return m_pStockItemHead;
}

//��ȡ����
template<class TYPE> AFX_INLINE TYPE * CWHStockList<TYPE>::GetTailStockItem()
{
	return m_pStockItemTail;
}

//��������
template<class TYPE> TYPE * CWHStockList<TYPE>::GetLastStockItem(TYPE * pCurrentItem)
{
	//Ч�����
	ASSERT(pCurrentItem != NULL);
	if (pCurrentItem != NULL) return ((CWHStockItem *)pCurrentItem)->GetLastStockItem();

	return NULL;
}

//��������
template<class TYPE> TYPE * CWHStockList<TYPE>::GetNextStockItem(TYPE * pCurrentItem)
{
	//Ч�����
	ASSERT(pCurrentItem != NULL);
	if (pCurrentItem != NULL) return ((CWHStockItem *)pCurrentItem)->GetNextStockItem();

	return NULL;
}

//��ն���
template<class TYPE> VOID CWHStockList<TYPE>::DeleteAllItem()
{
	//���ñ���
	m_dwItemCount = 0L;
	m_pStockItemTail = NULL;

	//ɾ������
	while (m_pStockItemIdle != NULL)
	{
		//��ȡ����
		CWHStockItem * pStockItemDelete = m_pStockItemIdle;

		//��������
		m_pStockItemIdle = m_pStockItemIdle->m_pStockItemNext;

		//ɾ������
		SafeDelete(pStockItemDelete);
	}

	//ɾ������
	while (m_pStockItemHead != NULL)
	{
		//��ȡ����
		CWHStockItem * pStockItemDelete = m_pStockItemHead;

		//��������
		m_pStockItemHead = m_pStockItemHead->m_pStockItemNext;

		//ɾ������
		SafeDelete(pStockItemDelete);
	}

	return;
}

//ɾ������
template<class TYPE> VOID CWHStockList<TYPE>::RemoveAllItem()
{
	//���ñ���
	m_dwItemCount = 0L;
	m_pStockItemTail = NULL;

	//ɾ������
	while (m_pStockItemHead != NULL)
	{
		//��ȡ����
		CWHStockItem * pStockItemBuffer = m_pStockItemIdle;
		CWHStockItem * pStockItemRemove = m_pStockItemHead;

		//��������
		m_pStockItemIdle = pStockItemRemove;
		m_pStockItemHead = m_pStockItemHead->m_pStockItemNext;

		//���ö���
		pStockItemRemove->m_pStockItemLast = NULL;
		pStockItemRemove->m_pStockItemNext = pStockItemBuffer;
	}

	return;
}

//���Ӷ���
template<class TYPE> TYPE * CWHStockList<TYPE>::AttachStockItem(TYPE * pCurrentItem)
{
	//Ч�����
	ASSERT(pCurrentItem != NULL);
	if (pCurrentItem == NULL) return NULL;

	//��������
	CWHStockItem * pStockItemAttach = (CWHStockItem *)pCurrentItem;

	//���ö���
	pStockItemAttach->m_pStockItemLast = NULL;
	pStockItemAttach->m_pStockItemNext = m_pStockItemHead;

	//��������
	if (m_pStockItemTail == NULL)
	{
		m_pStockItemTail = pStockItemAttach;
	}

	//��������
	if (m_pStockItemHead != NULL)
	{
		m_pStockItemHead->m_pStockItemLast = pStockItemAttach;
	}

	//��������
	m_dwItemCount++;
	m_pStockItemHead = pStockItemAttach;

	return pStockItemAttach;
}

//�������
template<class TYPE> TYPE * CWHStockList<TYPE>::DetachStockItem(TYPE * pCurrentItem)
{
	//Ч�����
	ASSERT(pCurrentItem != NULL);
	if (pCurrentItem == NULL) return NULL;

	//��������
	CWHStockItem * pStockItemDetach = (CWHStockItem *)pCurrentItem;

	//��������
	CWHStockItem * pStockItemLast = pStockItemDetach->m_pStockItemLast;
	CWHStockItem * pStockItemNext = pStockItemDetach->m_pStockItemNext;

	//������Ŀ
	ASSERT(m_dwItemCount > 0L);
	if (m_dwItemCount > 0L) m_dwItemCount--;

	//���ö���
	pStockItemDetach->m_pStockItemLast = NULL;
	pStockItemDetach->m_pStockItemNext = NULL;

	//��������
	if (pStockItemLast != NULL)
	{
		pStockItemLast->m_pStockItemNext = pStockItemNext;
	}

	//��������
	if (pStockItemNext != NULL)
	{
		pStockItemNext->m_pStockItemLast = pStockItemLast;
	}

	//��������
	if (m_pStockItemHead == pStockItemDetach)
	{
		m_pStockItemHead = pStockItemNext;
	}

	//��������
	if (m_pStockItemTail == pStockItemDetach)
	{
		m_pStockItemTail = pStockItemLast;
	}

	return pStockItemDetach;
}

//��������
template<class TYPE> TYPE * CWHStockList<TYPE>::CreateStockItem()
{
	//��������
	CWHStockItem * pStockItem = NULL;

	//��ȡָ��
	if (m_pStockItemIdle == NULL)
	{
		//��������
		try
		{
			pStockItem = new CWHStockItem;
			if (pStockItem == NULL) throw TEXT("��������ʧ��");
		}
		catch (...)
		{
			ASSERT(FALSE);
			return NULL;
		}
	}
	else
	{
		//��ȡ����
		pStockItem = m_pStockItemIdle;

		//��������
		m_pStockItemIdle = m_pStockItemIdle->m_pStockItemNext;
	}

	//����ж�
	if (pStockItem != NULL)
	{
		//��������
		pStockItem->m_pStockItemLast = NULL;
		pStockItem->m_pStockItemNext = m_pStockItemHead;

		//��������
		if (m_pStockItemTail == NULL)
		{
			m_pStockItemTail = pStockItem;
		}

		//��������
		if (m_pStockItemHead != NULL)
		{
			m_pStockItemHead->m_pStockItemLast = pStockItem;
		}

		//��������
		m_dwItemCount++;
		m_pStockItemHead = pStockItem;

		return pStockItem;
	}

	//�������
	ASSERT(FALSE);

	return NULL;
}

//ɾ������
template<class TYPE> TYPE * CWHStockList<TYPE>::RemoveStockItem(TYPE * pCurrentItem)
{
	//Ч�����
	ASSERT(pCurrentItem != NULL);
	if (pCurrentItem == NULL) return NULL;

	//��������
	CWHStockItem * pStockItemIdle = m_pStockItemIdle;
	CWHStockItem * pStockItemRemove = (CWHStockItem *)pCurrentItem;

	//��������
	CWHStockItem * pStockItemLast = pStockItemRemove->m_pStockItemLast;
	CWHStockItem * pStockItemNext = pStockItemRemove->m_pStockItemNext;

	//��������
	m_pStockItemIdle = pStockItemRemove;

	//������Ŀ
	ASSERT(m_dwItemCount > 0L);
	if (m_dwItemCount > 0L) m_dwItemCount--;

	//���ö���
	pStockItemRemove->m_pStockItemLast = NULL;
	pStockItemRemove->m_pStockItemNext = pStockItemIdle;

	//��������
	if (pStockItemLast != NULL)
	{
		pStockItemLast->m_pStockItemNext = pStockItemNext;
	}

	//��������
	if (pStockItemNext != NULL)
	{
		pStockItemNext->m_pStockItemLast = pStockItemLast;
	}

	//��������
	if (m_pStockItemHead == pStockItemRemove)
	{
		m_pStockItemHead = pStockItemNext;
	}

	//��������
	if (m_pStockItemTail == pStockItemRemove)
	{
		m_pStockItemTail = pStockItemLast;
	}

	return pStockItemRemove;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
template<class KEY, class TYPE> CWHStockData<KEY, TYPE>::CWHStockData()
{
	//���ñ���
	m_pStockItemHead = NULL;
	m_pStockItemTail = NULL;
	m_pStockItemIdle = NULL;

	//���ñ���
	m_bInitHashTable = false;

	return;
}

//��������
template<class KEY, class TYPE> CWHStockData<KEY, TYPE>::~CWHStockData()
{
	//�������
	DeleteAllItem();

	return;
}

//��������
template<class KEY, class TYPE> VOID CWHStockData<KEY, TYPE>::InitHashTable(UINT nHashCount)
{
	//���ñ���
	m_bInitHashTable = true;

	//��������
	m_StockItemAssist.InitHashTable(nHashCount);

	return;
}

//��ȡ��Ŀ
template<class KEY, class TYPE> AFX_INLINE INT_PTR CWHStockData<KEY, TYPE>::GetItemCount()
{
	return m_StockItemAssist.GetCount();
}

//��������
template<class KEY, class TYPE> TYPE * CWHStockData<KEY, TYPE>::SearchStockItem(KEY rItemKey)
{
	//��������
	CWHStockItem * pStockItem = NULL;
	m_StockItemAssist.Lookup(rItemKey, pStockItem);

	return pStockItem;
}

//��ȡ����
template<class KEY, class TYPE> AFX_INLINE TYPE * CWHStockData<KEY, TYPE>::GetHeadStockItem()
{
	return m_pStockItemHead;
}

//��ȡ����
template<class KEY, class TYPE> AFX_INLINE TYPE * CWHStockData<KEY, TYPE>::GetTailStockItem()
{
	return m_pStockItemTail;
}

//��������
template<class KEY, class TYPE> TYPE * CWHStockData<KEY, TYPE>::GetLastStockItem(TYPE * pCurrentItem)
{
	//Ч�����
	ASSERT(pCurrentItem != NULL);
	if (pCurrentItem != NULL) return ((CWHStockItem *)pCurrentItem)->GetLastStockItem();

	return NULL;
}

//��������
template<class KEY, class TYPE> TYPE * CWHStockData<KEY, TYPE>::GetNextStockItem(TYPE * pCurrentItem)
{
	//Ч�����
	ASSERT(pCurrentItem != NULL);
	if (pCurrentItem != NULL) return ((CWHStockItem *)pCurrentItem)->GetNextStockItem();

	return NULL;
}

//��ն���
template<class KEY, class TYPE> VOID CWHStockData<KEY, TYPE>::DeleteAllItem()
{
	//���ñ���
	m_pStockItemTail = NULL;

	//ɾ������
	m_StockItemAssist.RemoveAll();

	//ɾ������
	while (m_pStockItemIdle != NULL)
	{
		//��ȡ����
		CWHStockItem * pStockItemDelete = m_pStockItemIdle;

		//��������
		m_pStockItemIdle = m_pStockItemIdle->m_pStockItemNext;

		//ɾ������
		SafeDelete(pStockItemDelete);
	}

	//ɾ������
	while (m_pStockItemHead != NULL)
	{
		//��ȡ����
		CWHStockItem * pStockItemDelete = m_pStockItemHead;

		//��������
		m_pStockItemHead = m_pStockItemHead->m_pStockItemNext;

		//ɾ������
		SafeDelete(pStockItemDelete);
	}

	return;
}

//ɾ������
template<class KEY, class TYPE> VOID CWHStockData<KEY, TYPE>::RemoveAllItem()
{
	//���ñ���
	m_pStockItemTail = NULL;

	//ɾ������
	m_StockItemAssist.RemoveAll();

	//ɾ������
	while (m_pStockItemHead != NULL)
	{
		//��ȡ����
		CWHStockItem * pStockItemBuffer = m_pStockItemIdle;
		CWHStockItem * pStockItemRemove = m_pStockItemHead;

		//��������
		m_pStockItemIdle = pStockItemRemove;
		m_pStockItemHead = m_pStockItemHead->m_pStockItemNext;

		//���ö���
		pStockItemRemove->m_pStockItemLast = NULL;
		pStockItemRemove->m_pStockItemNext = pStockItemBuffer;
	}

	return;
}

//��������
template<class KEY, class TYPE> TYPE * CWHStockData<KEY, TYPE>::CreateStockItem(KEY rItemKey)
{
	//Ч��״̬
	ASSERT(m_bInitHashTable == true);
	if (m_bInitHashTable == false) return NULL;

	//���Ҷ���
	CWHStockItem * pStockItem = NULL;
	m_StockItemAssist.Lookup(rItemKey, pStockItem);

	//��������
	if (pStockItem == NULL)
	{
		//��ȡָ��
		if (m_pStockItemIdle == NULL)
		{
			//��������
			try
			{
				pStockItem = new CWHStockItem;
				if (pStockItem == NULL) throw TEXT("��������ʧ��");
			}
			catch (...)
			{
				ASSERT(FALSE);
				return NULL;
			}
		}
		else
		{
			//��ȡ����
			pStockItem = m_pStockItemIdle;

			//��������
			m_pStockItemIdle = m_pStockItemIdle->m_pStockItemNext;
		}

		//����ж�
		if (pStockItem != NULL)
		{
			//��������
			pStockItem->m_pStockItemLast = NULL;
			pStockItem->m_pStockItemNext = m_pStockItemHead;

			//��������
			if (m_pStockItemTail == NULL)
			{
				m_pStockItemTail = pStockItem;
			}

			//��������
			if (m_pStockItemHead != NULL)
			{
				m_pStockItemHead->m_pStockItemLast = pStockItem;
			}

			//��������
			m_pStockItemHead = pStockItem;

			//��������
			m_StockItemAssist[rItemKey] = pStockItem;

			return pStockItem;
		}
	}

	//�������
	ASSERT(FALSE);

	return NULL;
}

//ɾ������
template<class KEY, class TYPE> TYPE * CWHStockData<KEY, TYPE>::RemoveStockItem(KEY rItemKey)
{
	//���Ҷ���
	CWHStockItem * pStockItem = NULL;
	m_StockItemAssist.Lookup(rItemKey, pStockItem);

	//�����ж�
	if (pStockItem != NULL)
	{
		//��������
		CWHStockItem * pStockItemIdle = m_pStockItemIdle;
		CWHStockItem * pStockItemLast = pStockItem->m_pStockItemLast;
		CWHStockItem * pStockItemNext = pStockItem->m_pStockItemNext;

		//ɾ������
		if (m_StockItemAssist.RemoveKey(rItemKey) == TRUE)
		{
			//��������
			m_pStockItemIdle = pStockItem;

			//���ö���
			pStockItem->m_pStockItemLast = NULL;
			pStockItem->m_pStockItemNext = pStockItemIdle;

			//��������
			if (pStockItemLast != NULL)
			{
				pStockItemLast->m_pStockItemNext = pStockItemNext;
			}

			//��������
			if (pStockItemNext != NULL)
			{
				pStockItemNext->m_pStockItemLast = pStockItemLast;
			}

			//��������
			if (m_pStockItemHead == pStockItem)
			{
				m_pStockItemHead = pStockItemNext;
			}

			//��������
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