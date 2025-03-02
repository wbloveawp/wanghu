#ifndef Store_MANAGER_HEAD_FILE
#define Store_MANAGER_HEAD_FILE

#pragma once

//引入文件
#include "AfxTempl.h"
#include "StoreServiceHead.h"

///////////////////////////////////////////////////////////////////////////////////
//结构定义

//房间子项
struct tagServerItem
{
	//房间信息
	WORD								wKindID;					//类型标识
	WORD								wServerID;					//房间标识
	WORD								wServerType;				//房间类型
	WORD								wTableCount;				//桌子数量
	BYTE								cbStoreType;				//存储类型

	//状态信息
	bool								bRemoved;					//移除标识
	WORD								wTableUseCount;				//使用数量
};

//房间节点
struct tagServerNode
{
	tagServerItem						ServerItem;					//房间子项	
	tagServerNode *						pNextServerNode;			//房间节点
};

///////////////////////////////////////////////////////////////////////////////////

//数组定义
typedef CWHArray<tagServerNode *> CServerNodeArray;

//索引定义
typedef CMap<WORD,WORD,tagServerNode *,tagServerNode *> CMapServerNode;

///////////////////////////////////////////////////////////////////////////////////
//房间管理
class CServerManager
{
	//房间管理
protected:
	CMapServerNode						m_MapServerNode;			//房间映射		
	CServerNodeArray					m_ServerNodeBuffer;			//缓冲子项

	//函数定义
public:
	//构造函数
	CServerManager();
	//析构函数
	virtual ~CServerManager();	

	//功能函数
public:
	//房间数量
	WORD GetServerCount(WORD wKindID);

	//功能操作
public:	
	//添加房间
	bool AppendServer(tagServerItem & ServerItem);	
	//移除房间
	bool RemoveServer(WORD wKindID,WORD wServerID);				
	//枚举房间
	tagServerItem * EnumServer(WORD wKindID,WORD wServerID=0);	
	//查找房间
	tagServerItem * SearchServer(WORD wKindID,WORD wServerID);
	//获取房间
	tagServerItem * GetServerItem(WORD wKindID,WORD wServerID,WORD wTableCount,BYTE cbStoreType);
	
	//管理函数
protected:
	//激活房间
	tagServerNode * ActiveServerNode();
	//移除房间
	void RemoveServerNode(tagServerNode * pServerNode);	

	//辅助函数
public:
	//重置数据
	void ResetData();
};

///////////////////////////////////////////////////////////////////////////////////


#endif