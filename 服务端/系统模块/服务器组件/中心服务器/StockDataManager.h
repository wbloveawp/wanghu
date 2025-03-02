#ifndef STOCK_DATA_MANAGER_HEAD_FILE
#define STOCK_DATA_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DataBasePacket.h"

//////////////////////////////////////////////////////////////////////////////////

//信息索引
typedef CWHArray<tagStockItem*> CStockItemArray;

//////////////////////////////////////////////////////////////////////////////////

//全局数据
class CStockDataManager
{
	//数据信息
public:
	CStockItemArray					m_StockItemActive;					//库存数据
	CStockItemArray					m_StockItemBuffer;					//库存数据

	//静态变量
protected:
	static CStockDataManager*		m_pStockDataManager;				//对象指针

	//函数定义
public:
	//构造函数
	CStockDataManager();
	//析构函数
	virtual ~CStockDataManager();

	//管理函数
public:
	//重置数据
	VOID ResetGlobalData();
	//删除数据
	VOID DeleteGlobalData();

	//库存信息
public:
	//库存数目
	WORD GetStockCount() { return (WORD)m_StockItemActive.GetCount(); }

	//服务管理
public:
	//删除库存
	VOID DeleteStockItem(WORD wStockID);
	//添加库存
	tagStockItem* AppendStockItem(tagStockItem& StockItem);	

	//服务查找
public:
	//枚举库存
	tagStockItem * EnumStockItem(WORD wStockIndex);
	//寻找库存
	tagStockItem * SearchStockItem(WORD wStockID);
	//寻找库存
	tagStockItem * SearchStockItem(WORD wStockKind, WORD wKindID, WORD wStockSubtype);	

	//辅助函数
private:
	//创建库存
	tagStockItem* ActiveStockItem();	

	//静态函数
public:
	//获取对象
	static CStockDataManager* GetInstance() { return m_pStockDataManager; }
};

//////////////////////////////////////////////////////////////////////////////////

#endif