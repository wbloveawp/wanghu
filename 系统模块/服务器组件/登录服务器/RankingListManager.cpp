#include "RankingListManager.h"

//////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CRankingListItem::CRankingListItem()
{
	m_cbListType = 0;
	m_szRankingList[0] = 0;
	m_StockListUserItem.InitHashTable(11);
}

//析构函数
CRankingListItem::~CRankingListItem()
{
	m_StockListUserItem.DeleteAllItem();
}

//构造榜单
bool CRankingListItem::ConstructRankingList(BYTE cbListType, LPCTSTR pszRankingList)
{
	//设置变量
	m_cbListType = cbListType;
	StringCchCopy(m_szRankingList,CountArray(m_szRankingList),pszRankingList);

	//清除仓库
	m_StockListUserItem.RemoveAllItem();

	return ParseRankingList();
}

//查找用户
tagListUserItem* CRankingListItem::SearchListUser(DWORD dwUserID)
{
	return m_StockListUserItem.SearchStockItem(dwUserID);
}

//解析榜单
bool CRankingListItem::ParseRankingList()
{
	//获取对象
	auto JsonValuePtr = CWHJson::getValue(string(CT2A(m_szRankingList).m_psz));
	if (JsonValuePtr->getType() != eJsonTypeObj)
	{
		throw L"榜单列表类型错误,Json解析失败！";
	}

	//转换对象
	auto JsonValueObjPtr = CJsonValueObjPtr::dynamicCast(JsonValuePtr);

	//用户列表
	auto UserListPtr = JsonValueObjPtr->value["UserList_Total"];
	if (UserListPtr && UserListPtr->getType() == eJsonTypeArray)
	{
		//获取对象
		auto UserListArrayPtr = CJsonValueArrayPtr::dynamicCast(UserListPtr);

		//读取奖励
		for (auto UserItemPtr : UserListArrayPtr->value)
		{
			if (UserItemPtr->getType() == eJsonTypeObj)
			{
				//转换对象
				auto UserItemObjPtr = CJsonValueObjPtr::dynamicCast(UserItemPtr);

				//奖励信息
				tagListUserItem ListUserItem;
				ZeroMemory(&ListUserItem, sizeof(ListUserItem));

				//读取配置
				CWHJsonInput::readJson(ListUserItem.wRankID, UserItemObjPtr->value["RankID"], true);
				CWHJsonInput::readJson(ListUserItem.dwUserID, UserItemObjPtr->value["UserID"], true);

				//创建对象
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
		throw L"用户列表解析失败！";
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CRankingListManager::CRankingListManager()
{
	m_StockRankingList.InitHashTable(5);
}
	
//析构函数
CRankingListManager::~CRankingListManager()
{
	m_StockRankingList.DeleteAllItem();
}

//删除榜单
VOID CRankingListManager::DeleteRankingList(BYTE cbListType)
{
	m_StockRankingList.RemoveStockItem(cbListType);
}

//添加榜单
bool CRankingListManager::AppendRankingList(BYTE cbListType, LPCTSTR pszRankingList)
{
	try
	{
		//查找榜单
		auto pRankListItem = m_StockRankingList.SearchStockItem(cbListType);
		if (pRankListItem == NULL)
		{
			pRankListItem = m_StockRankingList.CreateStockItem(cbListType);
		}

		//构造榜单
		if (pRankListItem != NULL)
		{
			return pRankListItem->ConstructRankingList(cbListType, pszRankingList);
		}		
	}
	catch (LPCTSTR pszException)
	{
		ASSERT(FALSE);
		CTraceService::TraceString(TEXT("榜单[%d]构造失败: %s"), TraceLevel_Exception, cbListType, pszException);
	}
	catch (...)
	{
		ASSERT (FALSE);
		CTraceService::TraceString(TEXT("榜单[%d]构造失败: 未知错误"),TraceLevel_Exception, cbListType);
	}

	return false;
}

//查找榜单
CRankingListItem* CRankingListManager::SearchRankingList(BYTE cbListType)
{
	return m_StockRankingList.SearchStockItem(cbListType);
}

//重置数据
VOID CRankingListManager::ResetData()
{
	m_StockRankingList.RemoveAllItem();
}

//////////////////////////////////////////////////////////////////////////////////////////
