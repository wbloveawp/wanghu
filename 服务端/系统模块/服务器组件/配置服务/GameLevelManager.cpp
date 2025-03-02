#include "StdAfx.h"
#include "GameLevelManager.h"

//////////////////////////////////////////////////////////////////////////////////
//构造函数
CGameLevelManager::CGameLevelManager()
{
	//初始化
	m_StockUserLevel.InitHashTable(11);
	m_StockGameLevel.InitHashTable(43);	
	m_StockRankLevel.InitHashTable(103);
}

//析构函数
CGameLevelManager::~CGameLevelManager()
{
	ResetData();

	//删除子项
	m_StockGameLevel.DeleteAllItem();
	m_StockUserLevel.DeleteAllItem();
	m_StockRankLevel.DeleteAllItem();
}

//重置数据
VOID CGameLevelManager::ResetData()
{
	m_StockGameLevel.RemoveAllItem();
	m_StockUserLevel.RemoveAllItem();
	m_StockRankLevel.RemoveAllItem();
}

//计算等级
WORD CGameLevelManager::CalcUserOrder(LONG lMemberPoint)
{
	//变量定义
	tagUserLevelItem* pNextUserLevelItem = NULL;
	tagUserLevelItem* pCurrUserLevelItem = m_StockUserLevel.GetHeadStockItem();

	while (pCurrUserLevelItem)
	{
		pNextUserLevelItem = m_StockUserLevel.GetNextStockItem(pCurrUserLevelItem);
		if (pNextUserLevelItem != NULL)
		{
			if (lMemberPoint >= pCurrUserLevelItem->lNeedPoint && lMemberPoint < pNextUserLevelItem->lNeedPoint)
			{
				return pCurrUserLevelItem->wLevelID;
			}
		}
		else
		{
			return pCurrUserLevelItem->wLevelID;
		}

		pCurrUserLevelItem = pNextUserLevelItem;
	}

	return 1;
}

//计算等级
WORD CGameLevelManager::CalcGameOrder(LONG lExperience)
{
	//变量定义
	tagGameLevelItem* pNextGameLevelItem = NULL;
	tagGameLevelItem* pCurrGameLevelItem = m_StockGameLevel.GetTailStockItem();

	while (pCurrGameLevelItem)
	{
		pNextGameLevelItem = m_StockGameLevel.GetLastStockItem(pCurrGameLevelItem);
		if (pNextGameLevelItem != NULL)
		{
			if (lExperience >= pCurrGameLevelItem->lNeedPoint && lExperience < pNextGameLevelItem->lNeedPoint)
			{
				return pCurrGameLevelItem->wLevelID;
			}
		}
		else
		{
			return pCurrGameLevelItem->wLevelID;
		}

		pCurrGameLevelItem = pNextGameLevelItem;
	}

	return 1;
}

//计算等级
WORD CGameLevelManager::CalcRankOrder(LONG lRankScore)
{
	//变量定义
	tagRankLevelItem* pNextRankLevelItem = NULL;
	tagRankLevelItem* pCurrRankLevelItem = m_StockRankLevel.GetTailStockItem();
	
	while (pCurrRankLevelItem)
	{
		pNextRankLevelItem = m_StockRankLevel.GetLastStockItem(pCurrRankLevelItem);		
		if (pNextRankLevelItem != NULL)
		{
			if (lRankScore >= pCurrRankLevelItem->lNeedScore && lRankScore < pNextRankLevelItem->lNeedScore)
			{
				return pCurrRankLevelItem->wLevelID;
			}
		}
		else
		{
			return pCurrRankLevelItem->wLevelID;
		}

		pCurrRankLevelItem = pNextRankLevelItem;		
	}

	return 1;
}

//游戏等级
bool CGameLevelManager::AppendGameLevel(tagGameLevelItem GameLevelList[], WORD wLevelCount)
{
	for (WORD i = 0; i < wLevelCount; i++)
	{
		auto pGameLevelItem = &GameLevelList[i];
		auto pLevelStockItem = m_StockGameLevel.CreateStockItem(pGameLevelItem->wLevelID);
		if (pLevelStockItem)
		{
			CopyMemory(pLevelStockItem, pGameLevelItem, sizeof(tagGameLevelItem));
		}
	}

	return true;
}

//用户等级
bool CGameLevelManager::AppendUserLevel(tagUserLevelItem UserLevelList[], WORD wLevelCount)
{
	for (WORD i = 0; i < wLevelCount; i++)
	{
		auto pUserLevelItem = &UserLevelList[i];
		auto pLevelStockItem = m_StockUserLevel.CreateStockItem(pUserLevelItem->wLevelID);
		if (pLevelStockItem)
		{
			CopyMemory(pLevelStockItem, pUserLevelItem, sizeof(tagUserLevelItem));
		}
	}

	return true;
}

//排位等级
bool CGameLevelManager::AppendRankLevel(tagRankLevelItem RankLevelList[], WORD wLevelCount)
{
	for (WORD i = 0; i < wLevelCount; i++)
	{
		auto pRankLevelItem = &RankLevelList[i];
		auto pLevelStockItem = m_StockRankLevel.CreateStockItem(pRankLevelItem->wLevelID);
		if (pLevelStockItem)
		{
			CopyMemory(pLevelStockItem, pRankLevelItem, sizeof(tagRankLevelItem));
		}
	}

	return true;
}

//查找等级
tagGameLevelItem* CGameLevelManager::SearchGameLevel(WORD wLevelID)
{
	return m_StockGameLevel.SearchStockItem(wLevelID);
}

//查找等级
tagUserLevelItem* CGameLevelManager::SearchUserLevel(WORD wLevelID)
{
	return m_StockUserLevel.SearchStockItem(wLevelID);
}

//排位等级
tagRankLevelItem* CGameLevelManager::SearchRankLevel(WORD wLevelID)
{
	return m_StockRankLevel.SearchStockItem(wLevelID);
}

//////////////////////////////////////////////////////////////////////////////////
