#ifndef RANKING_LIST_MANAGER_HEAD
#define RANKING_LIST_MANAGER_HEAD
#pragma once

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////////////////
//类型声明
class CRankingListItem;
class CRankingListManager;

//////////////////////////////////////////////////////////////////////////////////////////
//结构定义
struct tagListUserItem
{
	DWORD							dwUserID;								//用户标识
	WORD							wRankID;								//排名标识
};

//////////////////////////////////////////////////////////////////////////////////////////
//类型定义
typedef CWHStockData<DWORD, tagListUserItem>	CStockListUserItem;
typedef CWHStockData<BYTE, CRankingListItem>	CStockRankingListItem;

//////////////////////////////////////////////////////////////////////////////////////////

//榜单子项
class CRankingListItem
{
	friend class CRankingListManager;

	//变量定义
protected:
	BYTE							m_cbListType;							//榜单类型
	TCHAR							m_szRankingList[8192];					//榜单数据
	CStockListUserItem				m_StockListUserItem;					//用户仓库

	//函数定义
public:
	//构造函数
	CRankingListItem();
	//析构函数
	virtual~CRankingListItem();

	//信息函数
public:
	//榜单数据
	LPCTSTR GetRankingList() { return m_szRankingList; }

	//管理函数
public:
	//构造榜单
	bool ConstructRankingList(BYTE cbListType, LPCTSTR pszRankingList);

	//功能函数
public:	
	//查找用户
	tagListUserItem* SearchListUser(DWORD dwUserID);

	//辅助函数
private:
	//解析榜单
	bool ParseRankingList();
};

//////////////////////////////////////////////////////////////////////////////////////////

//榜单管理
class CRankingListManager
{
	//管理变量
protected:
	CStockRankingListItem			m_StockRankingList;						//榜单仓库

	//函数定义
public:
	//构造函数
	CRankingListManager();
	//析构函数
	virtual~CRankingListManager();

	//榜单管理
public:
	//删除榜单
	VOID DeleteRankingList(BYTE cbListType);
	//添加榜单
	bool AppendRankingList(BYTE cbListType, LPCTSTR pszRankingList);	

	//功能函数
public:
	//查找榜单
	CRankingListItem* SearchRankingList(BYTE cbListType);

	//辅助函数
public:
	//重置数据
	VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////////////////////

#endif

