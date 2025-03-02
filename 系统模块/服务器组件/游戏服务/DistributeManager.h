#ifndef DISTRIBUTE_MANAGER_HEAD_FILE
#define DISTRIBUTE_MANAGER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////////////

//������Ϣ
struct tagDistributeInfo
{
	DWORD							dwInsertTime;						//����ʱ��
	IServerUserItem *				pIServerUserItem;					//�û��ӿ�
};

//����ڵ�
struct tagDistributeNode
{
	tagDistributeInfo				DistributeInfo;						//������Ϣ
};

//ͬ����Ϣ
struct tagSameTableInfo
{			
	WORD							wPlayerCount;					   //�����Ŀ
	DWORD							wPlayerIDSet[8];				   //��Ҽ���
};

//ͬ���ڵ�
struct tagSameTableNode
{
	BYTE							cbReferCount;						//���ô���		
	tagSameTableInfo				SameTableInfo;						//ͬ����Ϣ	
};
//////////////////////////////////////////////////////////////////////////////////

//���Ͷ���
typedef CWHArray<tagSameTableNode *> CSameTableArray;				//ͬ������
typedef CWHArray<tagDistributeInfo *> CDistributeInfoArray;		//�ڵ�����
typedef CWHArray<tagDistributeNode *> CDistributeNodeArray;		//�ڵ�����
typedef CMap<DWORD,DWORD,tagSameTableNode *, tagSameTableNode*>	CSameTableMap;	//ͬ����Ϣ

//////////////////////////////////////////////////////////////////////////////////

//�������
class CDistributeManager
{
	//�������
protected:
	WORD							m_wMinPlayerCount;				 //�������	
	BYTE							m_cbDistributeRule;				 //�������

	//�洢����
protected:
	CSameTableMap					m_SameTableMap;					 //ͬ���ֵ�
	CSameTableArray					m_SameTableBuffer;				 //ͬ������
	CSameTableArray					m_SameTableActive;				 //ͬ������	

	//�������
protected:		
	CDistributeNodeArray			m_PlayerNodeActive;				 //��ҽڵ�
	CDistributeNodeArray			m_RobotNodeActive;			 //�����ڵ�
	CDistributeNodeArray			m_DistributeNodeBuffer;			 //�ڵ㻺��		

	//��������
public:
	//���캯��
	CDistributeManager();
	//��������
	virtual ~CDistributeManager();

	//��������
public:
	//�����Ŀ
	inline int GetNodeCount() { return GetPlayerCount()+GetRobotCount(); }	
	//�����Ŀ
	inline int GetPlayerCount() { return (int)m_PlayerNodeActive.GetCount(); }
	//������Ŀ
	inline int GetRobotCount() { return (int)m_RobotNodeActive.GetCount(); }
	//������Ŀ
	inline void	SetMinPlayerCount(WORD wPlayerCount) { m_wMinPlayerCount=wPlayerCount; }
	//���ù���
	inline void	SetDistributeRule(BYTE cbDistributeRule) { m_cbDistributeRule=cbDistributeRule; }

	//���ܺ���
public:	
	//�Ƴ����
	void RemoveAll();
	//�Ƴ����
	void RemoveDistributeNode(IServerUserItem * pIServerUserItem);	
	//������
	bool InsertDistributeNode(IServerUserItem * pIServerUserItem);			
	//ִ�з���
	WORD PerformDistribute(CDistributeInfoArray & DistributeInfoArray, WORD wNeedCount);

	//���ܺ���
public:	
	//�Ƴ���Ϣ
	VOID RemoveSameTableInfo(DWORD dwUserID);
	//��ȡ��Ϣ
	tagSameTableInfo * GetUserSameTableInfo();
	//��ȡ��Ϣ
	tagSameTableInfo * GetUserSameTableInfo(DWORD dwUserID);	
	//��ȡ��Ϣ
	VOID SetUserSameTableInfo(DWORD dwUserID,const tagSameTableInfo * pSameTableInfo);	

	//ͬ������
protected:
	//�������
	tagSameTableNode * ActiveSameTableNode();
	//�Ƴ�����
	VOID RemoveSameTableNode(tagSameTableNode * pSameTableNode);

	//�ڵ����
protected:	
	//����ڵ�
	tagDistributeNode * ActiveDistributeNode();
	//�Ƴ����
	VOID RemoveDistributeNode(tagDistributeNode * pDistributeNode);	
	//���ҽ��
	tagDistributeNode* SearchNode(IServerUserItem * pIServerUserItem);

	//���˺���
protected:
	//��������
	bool FilterRuleIsAllRobot(const CDistributeInfoArray & DistributeInfoArray, IServerUserItem * pIServerUserItem);
	//�Ͼ�ͬ��
	bool FilterRuleExitsLastTable(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem * pIServerUserItem);
	//��ͬ��ַ
	bool FilterRuleExitsClientAddr(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem * pIServerUserItem);	
	//��ͬ������
	bool FilterRuleExitsMachineID(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem * pIServerUserItem);		
};

//////////////////////////////////////////////////////////////////////////////////

#endif