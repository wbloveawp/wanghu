#ifndef WH_CONHASE_FILE_HEAD
#define WH_CONHASE_FILE_HEAD
#pragma once

#include "map"
#include "CommonServiceHead.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////
//哈希节点
class COMMON_SERVICE_CLASS CWHHashNode
{
	//变量定义
protected:
	string							m_sIdentify;							//对象标识
	int								m_nReplicas;							//复制数量

	//函数定义
public:
	//构造函数
	CWHHashNode();
	//构造函数
	CWHHashNode(string sIdentify, int nReplicas);

	//信息函数
public:
	//复制数量
	int GetReplicas() const { return m_nReplicas; }
	//对象标识
	string GetIdentify() const { return m_sIdentify; }	
};

/////////////////////////////////////////////////////////////////////////////////////////////
//一致性哈希
class COMMON_SERVICE_CLASS CWHConHash
{
	//变量定义
protected:
	map<string, CWHHashNode>			m_MapNodes;								//节点字典
	map<uint32_t, CWHHashNode*>			m_MapNodeRing;							//节点字典	

	//函数定义
public:
	//构造函数
	CWHConHash();
	//构造函数
	virtual ~CWHConHash();

	//函数定义
private:
	//复制构造
	CWHConHash(const CWHConHash&) {};
	//复制构造
	void operator=(const CWHConHash&) {};

	//信息函数
public:
	//节点数量
	int GetNodeCount() { return (int)m_MapNodeRing.size(); }

	//操作函数
public:
	//清除节点
	void CleanNode();
	//添加节点
	bool AppendNode(const CWHHashNode& Node);
	//移除节点
	void RemoveNode(const CWHHashNode& Node);
	//移除节点
	void RemoveNode(const string& sIdentify);
	//查找节点
	bool Lookup(const string& sObject, CWHHashNode* Node);

	//复制函数
protected:
	//散列函数
	uint32_t Hash(const string& sIdentify, int nIndex);
};

/////////////////////////////////////////////////////////////////////////////////////////////


#endif