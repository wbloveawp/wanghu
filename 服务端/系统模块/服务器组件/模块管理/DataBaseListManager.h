#ifndef DATABASE_LIST_MANAGER_HEAD_FILE
#define DATABASE_LIST_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////

//数组定义
typedef CWHArray<tagDataBaseList *> CDatabaseListArray;

//索引定义
typedef CMap<WORD,WORD,tagDataBaseList *,tagDataBaseList *> CDatabaseListMap;

//////////////////////////////////////////////////////////////////////////

//数据库列表类
class MODULE_MANAGER_CLASS CDatabaseListManager
{
	//变量定义
public:
	CDatabaseListMap				m_ListMap;					//数据库索引
	CDatabaseListArray				m_ListArray;				//数据库数组

	//静态变量
protected:
	static bool						m_bLoad;					//加载标志
	static CDatabaseListManager		*m_spDBListMgr;				//数据库列表类
	
	//函数定义
public:
	//构造函数
	CDatabaseListManager();
	//析构函数
	virtual ~CDatabaseListManager();

	//管理函数
public:
	//加载数据库
	bool Load();
	//重置数据
	bool Reset();
	//删除数据
	bool Delete(WORD wDBListID);
	//插入数据
	bool Insert(tagDataBaseList * pItem);

	//信息函数
public:
	//获取数目
	INT_PTR GetCount();
	//查找数据
	tagDataBaseList * SearchItem(WORD wDBListID);
	//遍历数据
	POSITION GetStartPosition();
	//遍历数据
	tagDataBaseList * GetNext( POSITION &pos );

	//内部函数
private:
	//创建对象
	tagDataBaseList * CreateItem();

	//静态函数
public:
	//获取实例
	static CDatabaseListManager * GetInstance() { return m_spDBListMgr; }
};

//////////////////////////////////////////////////////////////////////////

#endif