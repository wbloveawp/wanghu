#ifndef DISTRIBUTE_MANAGER_HEAD_FILE
#define DISTRIBUTE_MANAGER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////////////

//分组信息
struct tagDistributeInfo
{
	DWORD							dwInsertTime;						//插入时间
	IServerUserItem *				pIServerUserItem;					//用户接口
};

//分组节点
struct tagDistributeNode
{
	tagDistributeInfo				DistributeInfo;						//分组信息
};

//同桌信息
struct tagSameTableInfo
{			
	WORD							wPlayerCount;					   //玩家数目
	DWORD							wPlayerIDSet[8];				   //玩家集合
};

//同桌节点
struct tagSameTableNode
{
	BYTE							cbReferCount;						//引用次数		
	tagSameTableInfo				SameTableInfo;						//同桌信息	
};
//////////////////////////////////////////////////////////////////////////////////

//类型定义
typedef CWHArray<tagSameTableNode *> CSameTableArray;				//同坐数组
typedef CWHArray<tagDistributeInfo *> CDistributeInfoArray;		//节点数组
typedef CWHArray<tagDistributeNode *> CDistributeNodeArray;		//节点数组
typedef CMap<DWORD,DWORD,tagSameTableNode *, tagSameTableNode*>	CSameTableMap;	//同桌信息

//////////////////////////////////////////////////////////////////////////////////

//分组管理
class CDistributeManager
{
	//规则变量
protected:
	WORD							m_wMinPlayerCount;				 //玩家数量	
	BYTE							m_cbDistributeRule;				 //分组规则

	//存储变量
protected:
	CSameTableMap					m_SameTableMap;					 //同桌字典
	CSameTableArray					m_SameTableBuffer;				 //同桌缓冲
	CSameTableArray					m_SameTableActive;				 //同桌数组	

	//管理变量
protected:		
	CDistributeNodeArray			m_PlayerNodeActive;				 //玩家节点
	CDistributeNodeArray			m_RobotNodeActive;			 //机器节点
	CDistributeNodeArray			m_DistributeNodeBuffer;			 //节点缓冲		

	//函数定义
public:
	//构造函数
	CDistributeManager();
	//析构函数
	virtual ~CDistributeManager();

	//内联函数
public:
	//结点数目
	inline int GetNodeCount() { return GetPlayerCount()+GetRobotCount(); }	
	//玩家数目
	inline int GetPlayerCount() { return (int)m_PlayerNodeActive.GetCount(); }
	//机器数目
	inline int GetRobotCount() { return (int)m_RobotNodeActive.GetCount(); }
	//设置数目
	inline void	SetMinPlayerCount(WORD wPlayerCount) { m_wMinPlayerCount=wPlayerCount; }
	//设置规则
	inline void	SetDistributeRule(BYTE cbDistributeRule) { m_cbDistributeRule=cbDistributeRule; }

	//功能函数
public:	
	//移除结点
	void RemoveAll();
	//移除结点
	void RemoveDistributeNode(IServerUserItem * pIServerUserItem);	
	//插入结点
	bool InsertDistributeNode(IServerUserItem * pIServerUserItem);			
	//执行分组
	WORD PerformDistribute(CDistributeInfoArray & DistributeInfoArray, WORD wNeedCount);

	//功能函数
public:	
	//移除信息
	VOID RemoveSameTableInfo(DWORD dwUserID);
	//获取信息
	tagSameTableInfo * GetUserSameTableInfo();
	//获取信息
	tagSameTableInfo * GetUserSameTableInfo(DWORD dwUserID);	
	//获取信息
	VOID SetUserSameTableInfo(DWORD dwUserID,const tagSameTableInfo * pSameTableInfo);	

	//同桌管理
protected:
	//激活对象
	tagSameTableNode * ActiveSameTableNode();
	//移除对象
	VOID RemoveSameTableNode(tagSameTableNode * pSameTableNode);

	//节点管理
protected:	
	//激活节点
	tagDistributeNode * ActiveDistributeNode();
	//移除结点
	VOID RemoveDistributeNode(tagDistributeNode * pDistributeNode);	
	//查找结点
	tagDistributeNode* SearchNode(IServerUserItem * pIServerUserItem);

	//过滤函数
protected:
	//机器过滤
	bool FilterRuleIsAllRobot(const CDistributeInfoArray & DistributeInfoArray, IServerUserItem * pIServerUserItem);
	//上局同桌
	bool FilterRuleExitsLastTable(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem * pIServerUserItem);
	//相同地址
	bool FilterRuleExitsClientAddr(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem * pIServerUserItem);	
	//相同机器码
	bool FilterRuleExitsMachineID(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem * pIServerUserItem);		
};

//////////////////////////////////////////////////////////////////////////////////

#endif