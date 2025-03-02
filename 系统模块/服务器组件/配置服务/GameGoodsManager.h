#ifndef GAME_GOODS_MANAGER_HEAD_FILE
#define GAME_GOODS_MANAGER_HEAD_FILE

#pragma once

#include "ConfigServiceHead.h"


//////////////////////////////////////////////////////////////////////////////////
//类型声明
class CGameGoodsItem;
class CGameGoodsManager;

//类型声明
class CGameGoodsMatchProps;

//////////////////////////////////////////////////////////////////////////////////

//类型定义
typedef CWHAutoPtr<CGameGoodsItem> CGameGoodsItemPtr;

//类型定义
typedef CWHArray<tagGoodsItem> CGoodsItemArray;
typedef CWHArray<tagGoodsInfo> CGoodsInfoArray;

//类型定义
typedef CMap<WORD, WORD, CGameGoodsItemPtr, CGameGoodsItemPtr>	CGoodsItemMap;
typedef CMap<WORD, WORD, CGameGoodsItemPtr, CGameGoodsItemPtr>	CGoodsKindMap;

//////////////////////////////////////////////////////////////////////////////////

//物品子项
class CONFIG_SERVICE_CLASS CGameGoodsItem : public CHandleBase
{
	friend class CGameGoodsManager;

	//基础属性
protected:
	WORD						m_wGoodsID;								//物品标识
	WORD						m_wGoodsKind;							//物品类型
	bool						m_bHasIndate;							//有效期标识	
	WORD						m_wCanTradeType;						//交易类型
	WORD						m_wCanStorePlace;						//存放位置
	TCHAR						m_szGoodsName[LEN_GOODS_NAME];			//物品名称		


	//指针变量
protected:
	CGameGoodsItem*				m_LastGoodsItemPtr;						//物品指针
	CGameGoodsItem*				m_NextGoodsItemPtr;						//物品指针

	//函数定义
public:
	//构造函数
	CGameGoodsItem();
	//析构函数
	virtual ~CGameGoodsItem();

	//基础信息
public:
	//物品标识
	WORD GetGoodsID() { return m_wGoodsID; }
	//物品类型
	WORD GetGoodsKind() { return m_wGoodsKind; }
	//有效期
	bool GetHasIndate() { return m_bHasIndate; }
	//物品名称
	LPCTSTR GetGoodsName() { return m_szGoodsName; }
	//交易类型
	WORD GetCanTradeType() { return m_wCanTradeType; }
	//存放位置
	WORD GetCanStorePlace() { return m_wCanStorePlace; }

	//链接信息
public:
	//物品指针	
	template <class TYPE> TYPE GetLastGoodsItemPtr();
	//物品指针	
	template <class TYPE> TYPE GetNextGoodsItemPtr();

	//功能函数
public:
	//构造物品
	bool ConstructGoods(tagGoodsInfo* GoodsInfo);

	//继承函数
public:
	//解析属性
	virtual bool ParseGoodsOption(LPCTSTR pszGoodsOption);
};

//////////////////////////////////////////////////////////////////////////////////

//物品管理
class CONFIG_SERVICE_CLASS CGameGoodsManager : public CSingleton<CGameGoodsManager>
{	
	//存储变量
protected:
	CGoodsItemMap				m_MapGoodsItem;						//物品子项
	CGoodsKindMap				m_MapGoodsKind;						//物品类型

	//临时存储
public:
	CGoodsInfoArray				m_ArrayGoodsInfo;					//物品信息

	//函数定义
public:
	//构造函数
	CGameGoodsManager();
	//析构函数
	virtual ~CGameGoodsManager();

	//服务函数
public:
	//重置数据
	VOID ResetData();

	//物品管理
public:
	//移除物品
	VOID RemoveGameGoods(WORD wGoodsID);
	//添加物品
	WORD AppendGameGoods(tagGoodsInfo GoodsInfo[], WORD wGoodsCount);

	//静态函数
public:
	//货币物品
	static bool IsMoneyGoods(WORD wGoodsID);
	//解析物品
	static bool ParseFromJson(LPCTSTR pszGoodsItem, tagGoodsItem& GoodsItem);
	//解析物品
	static bool ParseFromJson(LPCTSTR pszGoodsJson, CGoodsItemArray & MoneyArray, CGoodsItemArray& GoodsArray);	
	//序列化物品
	static bool SerializeToJson(tagGoodsItem & GoodsItem, TCHAR szBuffer[], WORD wBufferCount);
	//序列化物品
	static bool SerializeToJson(tagGoodsItem GoodsItem[], WORD wItemCount, TCHAR szBuffer[], WORD wBufferCount);
	

	//模板函数
public:	
	//查找物品
	template <class TYPE> TYPE SearchGoodsItem(WORD wGoodsID);
	//查找物品
	template <class TYPE> TYPE GetGoodsKindHead(WORD wGoodsKind);

	//辅助函数
private:
	//创建物品
	CGameGoodsItemPtr CreateGameGoods(WORD wGoodsKind);
};

//////////////////////////////////////////////////////////////////////////////////

//物品指针	
template <class TYPE> TYPE CGameGoodsItem::GetLastGoodsItemPtr()
{ 
	return (typename TYPE::element_type*)m_LastGoodsItemPtr;
}

//查找物品
template <class TYPE> TYPE CGameGoodsItem::GetNextGoodsItemPtr()
{ 
	return (typename TYPE::element_type*)m_NextGoodsItemPtr;
}

//查找物品
template <class TYPE> TYPE CGameGoodsManager::SearchGoodsItem(WORD wGoodsID)
{
	//查找物品
	CGameGoodsItemPtr GameGoodsItemPtr;
	if (m_MapGoodsItem.Lookup(wGoodsID, GameGoodsItemPtr) == FALSE)
	{
		return NULL;
	}

	return (typename TYPE::element_type*)GameGoodsItemPtr.get();
}

//枚举物品
template <class TYPE> TYPE CGameGoodsManager::GetGoodsKindHead(WORD wGoodsKind)
{
	//查找物品
	CGameGoodsItemPtr GameGoodsItemPtr;
	if (m_MapGoodsKind.Lookup(wGoodsKind, GameGoodsItemPtr) == FALSE)
	{
		return NULL;
	}

	return (typename TYPE::element_type*)GameGoodsItemPtr.get();
}

//////////////////////////////////////////////////////////////////////////////////

#define g_GameGoodsManager	(CGameGoodsManager::getInstance())

//////////////////////////////////////////////////////////////////////////////////

#endif