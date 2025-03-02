#ifndef WH_CONHASE_FILE_HEAD
#define WH_CONHASE_FILE_HEAD
#pragma once

#include "map"
#include "CommonServiceHead.h"

using namespace std;

/////////////////////////////////////////////////////////////////////////////////////////////
//��ϣ�ڵ�
class COMMON_SERVICE_CLASS CWHHashNode
{
	//��������
protected:
	string							m_sIdentify;							//�����ʶ
	int								m_nReplicas;							//��������

	//��������
public:
	//���캯��
	CWHHashNode();
	//���캯��
	CWHHashNode(string sIdentify, int nReplicas);

	//��Ϣ����
public:
	//��������
	int GetReplicas() const { return m_nReplicas; }
	//�����ʶ
	string GetIdentify() const { return m_sIdentify; }	
};

/////////////////////////////////////////////////////////////////////////////////////////////
//һ���Թ�ϣ
class COMMON_SERVICE_CLASS CWHConHash
{
	//��������
protected:
	map<string, CWHHashNode>			m_MapNodes;								//�ڵ��ֵ�
	map<uint32_t, CWHHashNode*>			m_MapNodeRing;							//�ڵ��ֵ�	

	//��������
public:
	//���캯��
	CWHConHash();
	//���캯��
	virtual ~CWHConHash();

	//��������
private:
	//���ƹ���
	CWHConHash(const CWHConHash&) {};
	//���ƹ���
	void operator=(const CWHConHash&) {};

	//��Ϣ����
public:
	//�ڵ�����
	int GetNodeCount() { return (int)m_MapNodeRing.size(); }

	//��������
public:
	//����ڵ�
	void CleanNode();
	//��ӽڵ�
	bool AppendNode(const CWHHashNode& Node);
	//�Ƴ��ڵ�
	void RemoveNode(const CWHHashNode& Node);
	//�Ƴ��ڵ�
	void RemoveNode(const string& sIdentify);
	//���ҽڵ�
	bool Lookup(const string& sObject, CWHHashNode* Node);

	//���ƺ���
protected:
	//ɢ�к���
	uint32_t Hash(const string& sIdentify, int nIndex);
};

/////////////////////////////////////////////////////////////////////////////////////////////


#endif