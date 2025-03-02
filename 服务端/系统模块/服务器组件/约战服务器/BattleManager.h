#ifndef PRIVATE_TABLE_MANAGER_HEAD_FILE
#define PRIVATE_TABLE_MANAGER_HEAD_FILE

#pragma once

//引入文件
#include "AfxTempl.h"
#include "BattleServiceHead.h"

///////////////////////////////////////////////////////////////////////////////////
//常量定义
//#ifdef _DEBUG
//	#define MAX_TABLE_COUNT_IN_CLUB	20								//最大桌数	
//	#define MAX_TABLE_COUNT_OUT_CLUB	4								//最大桌数	
//#else
//	#define MAX_TABLE_COUNT_IN_CLUB	20								//最大桌数	
//	#define MAX_TABLE_COUNT_OUT_CLUB	4								//最大桌数	
//#endif

///////////////////////////////////////////////////////////////////////////////////
//结构定义

//房间子项
struct tagServerItem
{
	WORD								wKindID;					//类型标识
	WORD								wServerID;					//房间标识	
	WORD								wServerType;				//房间类型
	WORD								wTableCount;				//桌子总量	
};

//房间状态
struct tagServerStatus
{
	bool								bRemoved;					//移除标识
	WORD								wTableUseCount;				//使用数量
};

//房间节点
struct tagServerNode
{
	tagServerItem						ServerItem;					//房间子项
	tagServerStatus						ServerStatus;				//房间状态
	tagServerNode *						pNextServerNode;			//房间节点
};

//桌子节点
struct tagTableNode
{
	//桌子信息
	tagBattleTableItemEx				TableItem;					//桌子信息		
	tagBattleTableParamEx 				TableParam;					//桌子参数

	//链接信息
	tagTableNode *						pNextUserNode;				//桌子节点
	tagTableNode *						pNextKindNode;				//桌子节点	
	tagTableNode *						pNextClubNode;				//桌子节点	
};

//头部节点
struct tagTableNodeHead
{
	INT									nNodeCount;					//节点数量
	tagTableNode *						pTableNodeHead;				//桌子节点
};

//桌主信息
struct tagTableOwner
{
	//用户信息
	DWORD								dwUserID;					//用户标识	
	
	//网络信息
	tagSocketInfo						SocketInfo;					//网络信息	
};

///////////////////////////////////////////////////////////////////////////////////

//数组定义
typedef CWHArray<tagTableNode *> CTableNodeArray;
typedef CWHArray<tagServerNode *> CServerNodeArray;
typedef CWHArray<tagTableOwner *> CTableOwnerArray;
typedef CWHArray<tagBattleConfigEx*> CBattleConfigArray;
typedef CWHArray<tagBattleTableItemEx*> CTableItemArray;
typedef CWHArray<tagBattleTableParamEx*> CTableParamArray;

//类型定义
typedef CWHStockData<DWORD, tagTableNode> CStockTableNode;
typedef CWHStockData<DWORD, tagTableOwner> CStockTableOwner;
typedef CWHStockData<DWORD, tagBattleConfigEx> CStockBattleConfig;

//索引定义
typedef CMap<WORD,WORD,tagServerNode*,tagServerNode*> CMapServerNode;
typedef CMap<WORD,WORD,tagTableNodeHead,tagTableNodeHead> CMapTableKindID;
typedef CMap<DWORD,DWORD,tagTableNodeHead,tagTableNodeHead> CMapTableUserID;
typedef CMap<DWORD,DWORD, tagTableNodeHead, tagTableNodeHead> CMapTableClubID;

///////////////////////////////////////////////////////////////////////////////////
//约战管理
class CBattleManager
{
	//桌子管理
protected:	
	CStockTableNode						m_StockTableNode;			//桌子仓库
	CMapTableUserID						m_MapTableUserID;			//用户映射
	CMapTableKindID						m_MapTableKindID;			//类型映射	
	CMapTableClubID						m_MapTableClubID;			//茶社映射

	//房间管理
protected:
	CMapServerNode						m_MapServerNode;			//房间字典
	CServerNodeArray					m_ServerNodeBuffer;			//房间缓存

	//数据仓库
protected:
	CStockTableOwner					m_StockTableOwner;			//桌主仓库	

	//函数定义
public:
	//构造函数
	CBattleManager();
	//析构函数
	virtual ~CBattleManager();

	//功能函数
public:
	//重置数据
	VOID ResetData();

	//房间操作
public:
	//移除房间
	bool RemoveServer(WORD wKindID,WORD wServerID);	
	//添加房间
	bool AppendServer(const tagServerItem & ServerItem);	

	//桌主操作
public:		
	//移除桌主
	void RemoveTableOwner(DWORD dwBattleNum);
	//查找桌主
	tagTableOwner * SearchTableOwner(DWORD dwBattleNum);	
	//创建桌主
	tagTableOwner * CreateTableOwner(DWORD dwBattleNum);

	//桌子操作
public:
	//移除桌子
	bool RemoveTable(DWORD dwBattleNum);	
	//修改桌主
	bool ModifyTableOwner(DWORD dwBattleNum, DWORD dwNewOwnerID);
	//枚举桌子
	tagBattleTableItemEx * EnumTableItem(tagBattleTableItemEx* pTableItem);
	//创建桌子
	tagBattleTableItemEx * CreateTable(DWORD dwUserID, DWORD dwClubID, tagBattleConfigEx & BattleConfig,WORD wServerID=0);

	//功能函数
public:		
	//桌子子项
	tagBattleTableItemEx * GetTableItem(DWORD dwBattleNum);			
	//桌子参数
	tagBattleTableParamEx * GetTableParam(DWORD dwBattleNum);	
	//添加桌子
	bool AddTableItem(const tagBattleTableItemEx & TableItem);	

	//数据枚举
public:
	//枚举游戏
	WORD EnumTableKindID(POSITION& Position);
	//枚举用户
	DWORD EnumTableUserID(POSITION& Position);	
	//枚举茶社
	DWORD EnumTableClubID(POSITION& Position);

	//数量统计
public:
	//桌子数量
	WORD GetTableCountByClubID(DWORD dwClubID);
	//桌子数量
	WORD GetTableCountByClubID(DWORD dwUserID, DWORD dwClubID);	
	//桌子数量
	WORD GetTableCountByKindID(DWORD dwUserID, WORD wKindID, DWORD dwClubID);

	//列表操作
public:
	//桌子列表
	bool GetRandTableList(WORD wTableCount, CTableItemArray& TableItemArray, CTableParamArray& TableParamArray);
	//桌子列表
	bool GetKindTableList(WORD wKindID,CTableItemArray & TableItemArray,CTableParamArray & TableParamArray);
	//桌子列表
	bool GetUserTableList(DWORD dwUserID,CTableItemArray & TableItemArray,CTableParamArray & TableParamArray);	
	//桌子列表
	bool GetClubTableList(DWORD dwClubID, CTableItemArray& TableItemArray, CTableParamArray& TableParamArray);

	//管理函数
protected:
	//激活房间
	tagServerNode * ActiveServerNode();
	//移除房间
	void RemoveServerNode(tagServerNode * pServerNode);	

	//查找房间
protected:	
	//查找房间
	tagServerNode * SearchServerNode(WORD wKindID);	
	//查找房间
	tagServerNode * SearchServerNode(WORD wKindID,WORD wServerID);	

	//查找函数
protected:
	//查找桌子
	tagTableNodeHead * SearchTableByKindID(WORD wKindID);
	//查找桌子
	tagTableNodeHead * SearchTableByUserID(DWORD dwUserID);	
	//查找桌子
	tagTableNodeHead * SearchTableByClubID(DWORD dwClubID);

	//添加函数
private:
	//添加节点
	bool InsertKindTableNode(WORD wKindID, tagTableNode * pTableNode);
	//添加节点
	bool InsertUserTableNode(DWORD dwUserID, tagTableNode * pTableNode);
	//添加节点
	bool InsertClubTableNode(DWORD dwClubID, tagTableNode* pTableNode);

	//移除函数
private:
	//移除节点
	bool RemoveKindTableNode(WORD wKindID, tagTableNode * pTableNode);
	//移除节点
	bool RemoveUserTableNode(DWORD dwUserID, tagTableNode * pTableNode);	
	//移除节点
	bool RemoveClubTableNode(DWORD dwClubID, tagTableNode* pTableNode);

	//辅助函数
private:
	//生成编号
	DWORD BuildNumber(int nNumberLen);	
	//生成密码
	void BuildPassword(TCHAR szPassword[LEN_PASSWORD],WORD wBufferCount);	
};

///////////////////////////////////////////////////////////////////////////////////


#endif