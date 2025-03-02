#ifndef GLOBAL_INFO_MANAGER_HEAD_FILE
#define GLOBAL_INFO_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"

//////////////////////////////////////////////////////////////////////////

//类说明
class CGlobalClubItem;

//仓库定义
typedef CWHStockData<WORD, CGlobalClubItem> CStockClubItem;

//////////////////////////////////////////////////////////////////////////

//茶社子项
class CGlobalClubItem : public CGlobalServiceItem
{
	//友元定义
	friend class CGlobalInfoManager;

	//函数定义
protected:
	//构造函数
	CGlobalClubItem();
	//析构函数
	virtual ~CGlobalClubItem();	
};

//////////////////////////////////////////////////////////////////////////

//全局信息
class CGlobalInfoManager : public CGlobalItemManager
{
	//索引变量
protected:
	CStockClubItem					m_StockClubItem;					//茶社仓库

	//函数定义
public:
	//构造函数
	CGlobalInfoManager();
	//析构函数
	virtual ~CGlobalInfoManager();

	//管理函数
public:
	//重置数据
	VOID ResetData();

	//茶社管理
public:
	//删除茶舍
	bool DeleteClubItem(WORD wServiceID);
	//激活茶舍
	CGlobalClubItem * ActiveClubItem(DWORD dwSocketID, tagServiceItem & ServiceItem);

	//查找函数
public:
	//寻找茶舍
	CGlobalClubItem* SearchClubItem(WORD wServiceID);

	//枚举函数
public:
	//枚举网关
	CGlobalClubItem* EnumClubItem(CGlobalClubItem* pGlobalClubItem);
};

//////////////////////////////////////////////////////////////////////////

#endif