#ifndef GAME_LEVEL_MANAGER_HEAD_FILE
#define GAME_LEVEL_MANAGER_HEAD_FILE

#pragma once

#include "ConfigServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//类型定义
typedef CWHArray<tagGameLevelItem> CGameLevelArray;
typedef CWHArray<tagUserLevelItem> CUserLevelArray;
typedef CWHArray<tagRankLevelItem> CRankLevelArray;

//类型定义
typedef CWHStockData<WORD, tagGameLevelItem> CGameLevelStock;
typedef CWHStockData<WORD, tagUserLevelItem> CUserLevelStock;
typedef CWHStockData<WORD, tagRankLevelItem> CRankLevelStock;

//////////////////////////////////////////////////////////////////////////////////

//等级管理
class CONFIG_SERVICE_CLASS CGameLevelManager : public CSingleton<CGameLevelManager>
{	
	//存储变量
protected:
	CGameLevelStock				m_StockGameLevel;						//等级仓库
	CUserLevelStock				m_StockUserLevel;						//等级仓库
	CRankLevelStock				m_StockRankLevel;						//等级仓库

	//临时存储
public:
	CGameLevelArray				m_ArrayGameLevel;						//游戏等级
	CUserLevelArray				m_ArrayUserLevel;						//用户等级
	CRankLevelArray				m_ArrayRankLevel;						//排位等级

	//函数定义
public:
	//构造函数
	CGameLevelManager();
	//析构函数
	virtual ~CGameLevelManager();

	//服务函数
public:
	//重置数据
	VOID ResetData();

	//信息函数
public:
	//等级数量
	WORD GetGameLevelCount() { return (WORD)m_StockGameLevel.GetItemCount(); }
	//等级数量
	WORD GetUserLevelCount() { return (WORD)m_StockUserLevel.GetItemCount(); }
	//等级数量
	WORD GetRankLevelCount() { return (WORD)m_StockRankLevel.GetItemCount(); }

	//功能函数
public:
	//计算等级
	WORD CalcUserOrder(LONG lMemberPoint);
	//计算等级
	WORD CalcGameOrder(LONG lExperience);
	//计算等级
	WORD CalcRankOrder(LONG lRankScore);

	//等级管理
public:
	//游戏等级
	bool AppendGameLevel(tagGameLevelItem GameLevelList[], WORD wLevelCount);	
	//用户等级
	bool AppendUserLevel(tagUserLevelItem UserLevelList[], WORD wLevelCount);	
	//排位等级
	bool AppendRankLevel(tagRankLevelItem RankLevelList[], WORD wLevelCount);

	//查找函数
public:
	//查找等级
	tagGameLevelItem* SearchGameLevel(WORD wLevelID);
	//查找等级
	tagUserLevelItem* SearchUserLevel(WORD wLevelID);
	//查找等级
	tagRankLevelItem* SearchRankLevel(WORD wLevelID);
};

//////////////////////////////////////////////////////////////////////////////////

#define g_GameLevelManager	(CGameLevelManager::getInstance())

//////////////////////////////////////////////////////////////////////////////////

#endif