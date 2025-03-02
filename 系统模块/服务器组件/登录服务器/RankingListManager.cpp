#include "RankingListManager.h"

//////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CRankingListItem::CRankingListItem()
{
	m_cbListType = 0;
	m_szRankingList[0] = 0;
	m_StockListUserItem.InitHashTable(11);
}

//��������
CRankingListItem::~CRankingListItem()
{
	m_StockListUserItem.DeleteAllItem();
}

//�����
bool CRankingListItem::ConstructRankingList(BYTE cbListType, LPCTSTR pszRankingList)
{
	//���ñ���
	m_cbListType = cbListType;
	StringCchCopy(m_szRankingList,CountArray(m_szRankingList),pszRankingList);

	//����ֿ�
	m_StockListUserItem.RemoveAllItem();

	return ParseRankingList();
}

//�����û�
tagListUserItem* CRankingListItem::SearchListUser(DWORD dwUserID)
{
	return m_StockListUserItem.SearchStockItem(dwUserID);
}

//������
bool CRankingListItem::ParseRankingList()
{
	//��ȡ����
	auto JsonValuePtr = CWHJson::getValue(string(CT2A(m_szRankingList).m_psz));
	if (JsonValuePtr->getType() != eJsonTypeObj)
	{
		throw L"���б����ʹ���,Json����ʧ�ܣ�";
	}

	//ת������
	auto JsonValueObjPtr = CJsonValueObjPtr::dynamicCast(JsonValuePtr);

	//�û��б�
	auto UserListPtr = JsonValueObjPtr->value["UserList_Total"];
	if (UserListPtr && UserListPtr->getType() == eJsonTypeArray)
	{
		//��ȡ����
		auto UserListArrayPtr = CJsonValueArrayPtr::dynamicCast(UserListPtr);

		//��ȡ����
		for (auto UserItemPtr : UserListArrayPtr->value)
		{
			if (UserItemPtr->getType() == eJsonTypeObj)
			{
				//ת������
				auto UserItemObjPtr = CJsonValueObjPtr::dynamicCast(UserItemPtr);

				//������Ϣ
				tagListUserItem ListUserItem;
				ZeroMemory(&ListUserItem, sizeof(ListUserItem));

				//��ȡ����
				CWHJsonInput::readJson(ListUserItem.wRankID, UserItemObjPtr->value["RankID"], true);
				CWHJsonInput::readJson(ListUserItem.dwUserID, UserItemObjPtr->value["UserID"], true);

				//��������
				auto pListUserItem = m_StockListUserItem.CreateStockItem(ListUserItem.dwUserID);
				if (pListUserItem != NULL)
				{
					pListUserItem->wRankID = ListUserItem.wRankID;
					pListUserItem->dwUserID = ListUserItem.dwUserID;
				}
			}
		}
	}
	else
	{
		throw L"�û��б����ʧ�ܣ�";
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////

//���캯��
CRankingListManager::CRankingListManager()
{
	m_StockRankingList.InitHashTable(5);
}
	
//��������
CRankingListManager::~CRankingListManager()
{
	m_StockRankingList.DeleteAllItem();
}

//ɾ����
VOID CRankingListManager::DeleteRankingList(BYTE cbListType)
{
	m_StockRankingList.RemoveStockItem(cbListType);
}

//��Ӱ�
bool CRankingListManager::AppendRankingList(BYTE cbListType, LPCTSTR pszRankingList)
{
	try
	{
		//���Ұ�
		auto pRankListItem = m_StockRankingList.SearchStockItem(cbListType);
		if (pRankListItem == NULL)
		{
			pRankListItem = m_StockRankingList.CreateStockItem(cbListType);
		}

		//�����
		if (pRankListItem != NULL)
		{
			return pRankListItem->ConstructRankingList(cbListType, pszRankingList);
		}		
	}
	catch (LPCTSTR pszException)
	{
		ASSERT(FALSE);
		CTraceService::TraceString(TEXT("��[%d]����ʧ��: %s"), TraceLevel_Exception, cbListType, pszException);
	}
	catch (...)
	{
		ASSERT (FALSE);
		CTraceService::TraceString(TEXT("��[%d]����ʧ��: δ֪����"),TraceLevel_Exception, cbListType);
	}

	return false;
}

//���Ұ�
CRankingListItem* CRankingListManager::SearchRankingList(BYTE cbListType)
{
	return m_StockRankingList.SearchStockItem(cbListType);
}

//��������
VOID CRankingListManager::ResetData()
{
	m_StockRankingList.RemoveAllItem();
}

//////////////////////////////////////////////////////////////////////////////////////////
