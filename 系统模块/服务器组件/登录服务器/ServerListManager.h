#ifndef SERVER_LIST_MANAGER_HEAD_FILE
#define SERVER_LIST_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//过滤状态
#define FM_DELETE					0x0001								//删除过滤
#define FM_SORT_ID					0x0002								//排序过滤
#define FM_KIND_ID					0x0004								//类型过滤
#define FM_NODE_ID					0x0008								//节点过滤
#define FM_SERVER_NAME				0x0010								//名字过滤

//////////////////////////////////////////////////////////////////////////

//游戏子项
class CGameItem
{
	//变量定义
public:
	bool							m_bDisuse;							//废弃标志

	//函数定义
protected:
	//构造函数
	CGameItem() { m_bDisuse=false; }
};

//////////////////////////////////////////////////////////////////////////

//种类结构
class CGameTypeItem : public CGameItem
{
	//变量定义
public:
	tagGameType						m_GameType;							//种类信息

	//函数定义
public:
	//构造函数
	CGameTypeItem();
};

//////////////////////////////////////////////////////////////////////////

//类型定义
typedef CWHArray<tagGameKindEx>	CGameKindArray;					//类型数组

//类型结构
class CGameKindItem : public CGameItem
{
	//变量定义
public:	
	tagGameKindEx					m_GameKind;							//游戏类型
	DWORD							m_dwOnLineCount;					//在线人数	

	//奖池信息
public:
	SCORE							m_lJackpotScore;					//奖池积分
	ULONGLONG						m_ulLastTickCount;					//更新时间

	//函数定义
public:
	//构造函数
	CGameKindItem();

	//功能函数
public:
	//重置对象
	VOID ResetKindItem();

	//信息函数
public:
	//类型标识
	WORD GetKindID() { return m_GameKind.wKindID; }
};

//////////////////////////////////////////////////////////////////////////

//节点结构
class CGameNodeItem : public CGameItem
{
	//变量定义
public:
	tagGameNode						m_GameNode;							//节点信息

	//函数定义
public:
	//构造函数
	CGameNodeItem();
};

//////////////////////////////////////////////////////////////////////////

//房间结构
class CGameServerItem : public CGameItem
{
	//变量定义
public:
	tagGameServer					m_GameServer;						//房间信息

	//函数定义
public:
	//构造函数
	CGameServerItem();
};

//////////////////////////////////////////////////////////////////////////

//定制结构
class CGameCustomItem : public CGameItem
{
	//变量定义
public:
	tagGameCustom					m_GameCustom;						//定制信息

	//函数定义
public:
	//构造函数
	CGameCustomItem();
};

//////////////////////////////////////////////////////////////////////////

//房间过滤
class CGameFilterItem : public CGameItem
{
	//变量定义
public:
	tagGameFilter					m_GameFilter;						//过滤信息

	//函数定义
public:
	//构造函数
	CGameFilterItem();

	//功能函数
public:
	//插入判断
	bool InsertVerdict();
	//排序过滤
	WORD FilterSortID(WORD wSortID);
	//类型过滤
	WORD FilterKindID(WORD wKindID);
	//节点过滤
	WORD FilterNodeID(WORD wNodeID);
	//名字过滤
	LPCTSTR FilterServerName(LPCTSTR pszServerName);
};

//////////////////////////////////////////////////////////////////////////

//数组说明
typedef CWHArray<CGameTypeItem *>	CGameTypeItemArray;
typedef CWHArray<CGameKindItem *>	CGameKindItemArray;
typedef CWHArray<CGameNodeItem *>	CGameNodeItemArray;
typedef CWHArray<CGameCustomItem *> CGameCustomItemArray;
typedef CWHArray<CGameServerItem *> CGameServerItemArray;
typedef CWHArray<CGameFilterItem *> CGameFilterItemArray;

//索引说明
typedef CMap<WORD,WORD,CGameTypeItem *,CGameTypeItem * &> CTypeItemMap;
typedef CMap<WORD,WORD,CGameKindItem *,CGameKindItem * &> CKindItemMap;
typedef CMap<WORD,WORD,CGameNodeItem *,CGameNodeItem * &> CNodeItemMap;
typedef CMap<WORD,WORD,CGameCustomItem *,CGameCustomItem * &> CCustomItemMap;
typedef CMap<WORD,WORD,CGameServerItem *,CGameServerItem * &> CServerItemMap;
typedef CMap<WORD,WORD,CGameFilterItem *,CGameFilterItem * &> CFilterItemMap;

//////////////////////////////////////////////////////////////////////////

//列表服务
class CServerListManager
{
	//索引变量
protected:
	CTypeItemMap					m_TypeItemMap;						//种类索引
	CKindItemMap					m_KindItemMap;						//类型索引
	CNodeItemMap					m_NodeItemMap;						//节点索引
	CCustomItemMap					m_CustomItemMap;					//定制索引
	CServerItemMap					m_ServerItemMap;					//房间索引
	CFilterItemMap					m_FilterItemMap;					//过滤索引

	//存储变量
protected:
	CGameTypeItemArray				m_TypeItemBuffer;					//种类数组
	CGameKindItemArray				m_KindItemBuffer;					//类型数组
	CGameNodeItemArray				m_NodeItemBuffer;					//节点数组
	CGameCustomItemArray			m_CustomItemBuffer;					//定制数组
	CGameServerItemArray			m_ServerItemBuffer;					//房间数组
	CGameFilterItemArray			m_FilterItemBuffer;					//过滤数组

	//函数定义
public:
	//构造函数
	CServerListManager();
	//析构函数
	virtual ~CServerListManager();

	//管理接口
public:
	//重置列表
	VOID ResetServerList();
	//清理内核
	VOID CleanKernelItem();
	//清理房间
	VOID CleanServerItem();
	//废弃内核
	VOID DisuseKernelItem();
	//废弃房间
	VOID DisuseServerItem();

	//统计接口
public:
	//统计人数
	DWORD CollectOnlineInfo();
	//类型在线
	DWORD CollectOnlineInfo(WORD wKindID);

	//插入接口
public:
	//插入种类
	bool InsertGameType(tagGameType * pGameType);
	//插入类型
	bool InsertGameKind(tagGameKindEx * pGameKind);
	//插入节点
	bool InsertGameNode(tagGameNode * pGameNode);
	//插入定制
	bool InsertGameCustom(tagGameCustom * pGameCustom);
	//插入房间
	bool InsertGameServer(tagGameServer * pGameServer);
	//插入过滤
	bool InsertGameFilter(tagGameFilter * pGameFilter);

	//删除接口
public:
	//删除种类
	bool DeleteGameType(WORD wTypeID);
	//删除类型
	bool DeleteGameKind(WORD wKindID);
	//删除节点
	bool DeleteGameNode(WORD wNodeID);
	//删除房间
	bool DeleteGameServer(WORD wServerID);
	//删除定制
	bool DeleteGameCustom(WORD wCustomID);
	//删除过滤
	bool DeleteGameFilter(WORD wServerID);

	//枚举接口
public:
	//枚举种类
	CGameTypeItem * EmunGameTypeItem(POSITION & Position);
	//枚举类型
	CGameKindItem * EmunGameKindItem(POSITION & Position);
	//枚举节点
	CGameNodeItem * EmunGameNodeItem(POSITION & Position);
	//枚举房间
	CGameServerItem * EmunGameServerItem(POSITION & Position);
	//枚举定制
	CGameCustomItem * EmunGameCustomItem(POSITION & Position);
	//枚举过滤
	CGameFilterItem * EmunGameFilterItem(POSITION & Position);

	//查找接口
public:
	//查找种类
	CGameTypeItem * SearchGameType(WORD wTypeID);
	//查找类型
	CGameKindItem * SearchGameKind(WORD wKindID);
	//查找节点
	CGameNodeItem * SearchGameNode(WORD wNodeID);
	//查找房间
	CGameServerItem * SearchGameServer(WORD wServerID);
	//查找定制
	CGameCustomItem * SearchGameCustom(WORD wCustomID);
	//查找过滤
	CGameFilterItem * SearchGameFilter(WORD wServerID);

	//数目接口
public:
	//种类数目
	DWORD GetGameTypeCount() { return (DWORD)m_TypeItemMap.GetCount(); }
	//类型数目
	DWORD GetGameKindCount() { return (DWORD)m_KindItemMap.GetCount(); }
	//节点数目
	DWORD GetGameNodeCount() { return (DWORD)m_NodeItemMap.GetCount(); }
	//定制数目
	DWORD GetGameCustomCount() { return (DWORD)m_CustomItemMap.GetCount(); }
	//房间数目
	DWORD GetGameServerCount() { return (DWORD)m_ServerItemMap.GetCount(); }
	//过滤数目
	DWORD GetGameFilterCount() { return (DWORD)m_FilterItemMap.GetCount(); }
};

//////////////////////////////////////////////////////////////////////////

#endif