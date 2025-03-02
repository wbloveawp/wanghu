#include "StdAfx.h"
#include "GameGoodsManager.h"

using namespace WHJson;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CGameGoodsItem::CGameGoodsItem()
{
	m_wGoodsID = 0;
	m_wGoodsKind = 0;
	m_wCanTradeType = 0;
	m_wCanStorePlace = 0;
	m_szGoodsName[0] = 0;	
	m_LastGoodsItemPtr = NULL;
	m_NextGoodsItemPtr = NULL;	
}

//析构函数
CGameGoodsItem::~CGameGoodsItem()
{
}

//构造物品
bool CGameGoodsItem::ConstructGoods(tagGoodsInfo* pGoodsInfo)
{
	//校验参数
	ASSERT (pGoodsInfo!=NULL);
	if (pGoodsInfo == NULL) return false;

	//设置变量
	m_wGoodsID = pGoodsInfo->wGoodsID;
	m_wGoodsKind = pGoodsInfo->wGoodsKind;
	m_bHasIndate = pGoodsInfo->bHasIndate;
	m_wCanTradeType = pGoodsInfo->wCanTradeType;
	m_wCanStorePlace = pGoodsInfo->wCanStorePlace;
	StringCchCopy(m_szGoodsName, CountArray(m_szGoodsName), pGoodsInfo->szGoodsName);

	//回收价格
	if ((m_wCanTradeType&TRADE_TYPE_RECYCLE)!=0)
	{
		return false;
	}	

	return ParseGoodsOption(pGoodsInfo->szGoodsOption);
}

//解析属性
bool CGameGoodsItem::ParseGoodsOption(LPCTSTR pszGoodsOption)
{
	return true;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CGameGoodsManager::CGameGoodsManager()
{
	m_MapGoodsKind.InitHashTable(17);
	m_MapGoodsItem.InitHashTable(101);
}

//析构函数
CGameGoodsManager::~CGameGoodsManager()
{
	ResetData();
}

//重置数据
VOID CGameGoodsManager::ResetData()
{
	m_MapGoodsKind.RemoveAll();
	m_MapGoodsItem.RemoveAll();	
}

//移除物品
VOID CGameGoodsManager::RemoveGameGoods(WORD wGoodsID)
{
	//查询类型
	CGameGoodsItemPtr GameGoodsItemPtr;
	if (m_MapGoodsItem.Lookup(wGoodsID, GameGoodsItemPtr))
	{
		if (GameGoodsItemPtr->m_LastGoodsItemPtr)
		{
			GameGoodsItemPtr->m_LastGoodsItemPtr->m_NextGoodsItemPtr = GameGoodsItemPtr->m_NextGoodsItemPtr;
		}
		else
		{
			if (GameGoodsItemPtr->m_NextGoodsItemPtr)
			{
				GameGoodsItemPtr->m_NextGoodsItemPtr->m_LastGoodsItemPtr = NULL;
				m_MapGoodsKind[GameGoodsItemPtr->m_wGoodsKind] = GameGoodsItemPtr->m_NextGoodsItemPtr;
			}
			else
			{
				m_MapGoodsKind.RemoveKey(GameGoodsItemPtr->m_wGoodsKind);
			}
		}
	}	
}

//添加物品
WORD CGameGoodsManager::AppendGameGoods(tagGoodsInfo GoodsInfo[], WORD wGoodsCount)
{
	for (WORD i = 0; i < wGoodsCount; i++)
	{
		tagGoodsInfo* pGoodsInfo = &GoodsInfo[i];

		try
		{			
			auto GameGoodsItemPtr = CreateGameGoods(pGoodsInfo->wGoodsKind);
			if (GameGoodsItemPtr)
			{
				//构造物品
				GameGoodsItemPtr->ConstructGoods(pGoodsInfo);

				//构造字典
				m_MapGoodsItem[pGoodsInfo->wGoodsID] = GameGoodsItemPtr;

				//查询类型
				CGameGoodsItemPtr GameGoodsKindPtr;
				m_MapGoodsKind.Lookup(pGoodsInfo->wGoodsKind, GameGoodsKindPtr);

				//链接物品
				if (GameGoodsKindPtr)
				{
					GameGoodsItemPtr->m_LastGoodsItemPtr = NULL;
					GameGoodsItemPtr->m_NextGoodsItemPtr = GameGoodsKindPtr.get();
					GameGoodsKindPtr->m_LastGoodsItemPtr = GameGoodsItemPtr.get();
				}

				m_MapGoodsKind[pGoodsInfo->wGoodsKind] = GameGoodsItemPtr;
			}
		}
		catch (LPCTSTR pszException)
		{
			CTraceService::TraceString(TEXT("物品[%s]构造失败:%s"), TraceLevel_Exception, pGoodsInfo->szGoodsName, pszException);

			return i;
		}
		catch (...)
		{
			CTraceService::TraceString(TEXT("物品[%s]构造失败:未知异常"), TraceLevel_Exception, pGoodsInfo->szGoodsName);

			return i;
		}
	}

	return wGoodsCount;
}

//货币物品
bool CGameGoodsManager::IsMoneyGoods(WORD wGoodsID)
{
	if (wGoodsID== GOODS_ID_GOLD || 
		wGoodsID== GOODS_ID_CARD)		
	{
		return true;
	}

	return false;
}

//序列化物品
bool CGameGoodsManager::SerializeToJson(tagGoodsItem& GoodsItem, TCHAR szBuffer[], WORD wBufferCount)
{
	//格式化字符串
	CString strGoodsItem;
	LPCTSTR pszTaskItem = TEXT("{\"GoodsID\":%d,\"Level\":%d,\"Count\":%u,\"Indate\":%u}");

	//格式化物品
	strGoodsItem.Format(pszTaskItem, GoodsItem.wGoodsID, GoodsItem.wGoodsLevel,GoodsItem.dwGoodsCount, GoodsItem.dwGoodsIndate);

	//校验长度
	if (strGoodsItem.GetLength() > wBufferCount)
	{
		return false;
	}

	//拷贝数据
	StringCchCopy(szBuffer, wBufferCount, strGoodsItem.GetString());

	return true;
}

//序列化物品
bool CGameGoodsManager::SerializeToJson(tagGoodsItem GoodsItem[], WORD wItemCount, TCHAR szBuffer[], WORD wBufferCount)
{
	//格式化字符串
	LPCTSTR pszTaskItem = TEXT("{\"GoodsID\":%d,\"Level\":%d,\"Count\":%u,\"Indate\":%u}");

	//变量定义
	CString strGoodsItem;
	CString strGoodsList;

	//构造任务列表
	for (auto i = 0; i < wItemCount; i++)
	{
		//添加括号
		if (i == 0) strGoodsList.Append(TEXT("["));

		//格式化物品
		strGoodsItem.Format(pszTaskItem, GoodsItem[i].wGoodsID, GoodsItem[i].wGoodsLevel, GoodsItem[i].dwGoodsCount, GoodsItem[i].dwGoodsIndate);

		//添加任务
		strGoodsList.Append(strGoodsItem);
		strGoodsList.Append((i<wItemCount-1)?TEXT(","):TEXT("]"));
	}

	//校验长度
	if (strGoodsList.GetLength() > wBufferCount)
	{
		return false;
	}

	//拷贝数据
	StringCchCopy(szBuffer, wBufferCount, strGoodsList.GetString());

	return true;
}

//解析物品
bool CGameGoodsManager::ParseFromJson(LPCTSTR pszGoodsItem, tagGoodsItem& GoodsItem)
{
	//参数校验
	if (pszGoodsItem == NULL || lstrlen(pszGoodsItem) == 0)
	{
		return false;
	}

	try
	{
		//获取对象
		auto JsonValuePtr = CWHJson::getValue(string(CT2CA(pszGoodsItem)));
		if (JsonValuePtr->getType() != eJsonTypeObj)
		{
			return false;
		}

		//转换对象
		auto JsonValueObjPtr = CJsonValueObjPtr::dynamicCast(JsonValuePtr);

		//默认设置
		GoodsItem.wGoodsLevel = 1;

		//读取配置
		CWHJsonInput::readJson(GoodsItem.wGoodsID, JsonValueObjPtr->value["GoodsID"], true);		
		CWHJsonInput::readJson(GoodsItem.dwGoodsCount, JsonValueObjPtr->value["Count"], true);
		CWHJsonInput::readJson(GoodsItem.wGoodsLevel, JsonValueObjPtr->value["Level"], false);
		CWHJsonInput::readJson(GoodsItem.dwGoodsIndate, JsonValueObjPtr->value["Indate"], false);		

		return true;
	}
	catch (...)
	{
		ASSERT(false);
	}

	return false;
}

//解析物品
bool CGameGoodsManager::ParseFromJson(LPCTSTR pszGoodsList, CGoodsItemArray& MoneyArray, CGoodsItemArray& GoodsArray)
{
	//参数校验
	if (pszGoodsList == NULL || lstrlen(pszGoodsList) == 0)
	{
		return false;
	}

	try
	{
		//获取对象
		auto JsonValuePtr = CWHJson::getValue(string(CT2CA(pszGoodsList)));
		if (JsonValuePtr->getType() != eJsonTypeArray)
		{
			return false;
		}

		//转换对象
		auto JsonValueArrayPtr = CJsonValueArrayPtr::dynamicCast(JsonValuePtr);

		//变量定义
		tagGoodsItem GoodsItem;
		ZeroMemory(&GoodsItem,sizeof(GoodsItem));

		//转换对象	
		for (auto ItemPtr : JsonValueArrayPtr->value)
		{
			auto RewardItemPtr = CJsonValueObjPtr::dynamicCast(ItemPtr);
			if (RewardItemPtr)
			{
				//设置变量
				GoodsItem.wGoodsLevel = 1;
				GoodsItem.dwGoodsIndate = 0;

				//读取配置
				CWHJsonInput::readJson(GoodsItem.wGoodsID, RewardItemPtr->value["GoodsID"], true);
				CWHJsonInput::readJson(GoodsItem.dwGoodsCount, RewardItemPtr->value["Count"], true);
				CWHJsonInput::readJson(GoodsItem.wGoodsLevel, RewardItemPtr->value["Level"], false);
				CWHJsonInput::readJson(GoodsItem.dwGoodsIndate, RewardItemPtr->value["Indate"], false);

				//添加奖励
				if (IsMoneyGoods(GoodsItem.wGoodsID))
				{
					MoneyArray.Add(GoodsItem);
				}
				else
				{
					GoodsArray.Add(GoodsItem);
				}
			}
		}

		return true;
	}
	catch (...)
	{
		ASSERT(false);
	}

	return false;
}

//创建物品
CGameGoodsItemPtr CGameGoodsManager::CreateGameGoods(WORD wGoodsKind)
{
	try
	{
		//变量定义
		CGameGoodsItemPtr GameGoodsItemPtr(NULL);

		switch (wGoodsKind)
		{
		default:
		{
			GameGoodsItemPtr = new CGameGoodsItem();
			break;
		}
		}

		return GameGoodsItemPtr;
	}
	catch (...)
	{
		ASSERT (FALSE);
	}	

	return NULL;
}

//////////////////////////////////////////////////////////////////////////////////
