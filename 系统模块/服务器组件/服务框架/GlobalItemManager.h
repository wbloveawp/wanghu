#ifndef GLOBAL_ITEM_MANAGER_HEAD_FILE
#define GLOBAL_ITEM_MANAGER_HEAD_FILE

#pragma once

#include "GlobalInfoItem.h"
#include "ServiceFrameHead.h"

//////////////////////////////////////////////////////////////////////////

//信息汇总
class SERVICE_FRAME_CLASS CGlobalItemManager
{
	//索引变量
protected:
	CStockUserItem					m_StockUserItem;					//用户子项
	CStockGateItem					m_StockGateItem;					//网关子项
	CStockServerItem				m_StockServerItem;					//房间子项

	//函数定义
public:
	//构造函数
	CGlobalItemManager();
	//析构函数
	virtual ~CGlobalItemManager();

	//管理函数
public:
	//用户数目
	DWORD GetUserItemCount() { return (DWORD)m_StockUserItem.GetItemCount(); }
	//网关数目
	DWORD GetGateItemCount() { return (DWORD)m_StockGateItem.GetItemCount(); }
	//房间数目
	DWORD GetServerItemCount() { return (DWORD)m_StockServerItem.GetItemCount(); }

	//用户管理
public:
	//删除用户
	bool DeleteUserItem(DWORD dwUserID);
	//激活用户
	CGlobalUserItem * ActiveUserItem(tagGlobalUserInfo & GlobalUserInfo);

	//网关管理
public:
	//删除网关
	bool DeleteGateItem(WORD wGateID);
	//激活网关
	CGlobalGateItem * ActiveGateItem(DWORD dwSocketID, tagServiceItem & GateItem);	

	//房间管理
public:
	//删除房间
	bool DeleteServerItem(WORD wServerID);
	//激活房间
	CGlobalServerItem* ActiveServerItem(DWORD dwSocketID, tagGameServer& GameServer);	

	//查找函数
public:
	//寻找网关
	CGlobalGateItem* SearchGateItem(WORD wGateID);
	//寻找用户
	CGlobalUserItem * SearchUserItem(DWORD dwUserID);	
	//寻找房间
	CGlobalServerItem* SearchServerItem(WORD wServerID);

	//枚举函数
public:
	//枚举用户
	CGlobalUserItem * EnumUserItem(CGlobalUserItem* pGlobalUserItem);
	//枚举网关
	CGlobalGateItem * EnumGateItem(CGlobalGateItem* pGlobalGateItem);
	//枚举房间
	CGlobalServerItem * EnumServerItem(CGlobalServerItem* pGlobalServerItem);

	//辅助函数
public:
	//重置数据
	virtual VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

#endif