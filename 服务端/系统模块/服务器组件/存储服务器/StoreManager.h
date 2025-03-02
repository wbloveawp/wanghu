#ifndef Store_MANAGER_HEAD_FILE
#define Store_MANAGER_HEAD_FILE

#pragma once

//�����ļ�
#include "AfxTempl.h"
#include "StoreServiceHead.h"

///////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//��������
struct tagServerItem
{
	//������Ϣ
	WORD								wKindID;					//���ͱ�ʶ
	WORD								wServerID;					//�����ʶ
	WORD								wServerType;				//��������
	WORD								wTableCount;				//��������
	BYTE								cbStoreType;				//�洢����

	//״̬��Ϣ
	bool								bRemoved;					//�Ƴ���ʶ
	WORD								wTableUseCount;				//ʹ������
};

//����ڵ�
struct tagServerNode
{
	tagServerItem						ServerItem;					//��������	
	tagServerNode *						pNextServerNode;			//����ڵ�
};

///////////////////////////////////////////////////////////////////////////////////

//���鶨��
typedef CWHArray<tagServerNode *> CServerNodeArray;

//��������
typedef CMap<WORD,WORD,tagServerNode *,tagServerNode *> CMapServerNode;

///////////////////////////////////////////////////////////////////////////////////
//�������
class CServerManager
{
	//�������
protected:
	CMapServerNode						m_MapServerNode;			//����ӳ��		
	CServerNodeArray					m_ServerNodeBuffer;			//��������

	//��������
public:
	//���캯��
	CServerManager();
	//��������
	virtual ~CServerManager();	

	//���ܺ���
public:
	//��������
	WORD GetServerCount(WORD wKindID);

	//���ܲ���
public:	
	//��ӷ���
	bool AppendServer(tagServerItem & ServerItem);	
	//�Ƴ�����
	bool RemoveServer(WORD wKindID,WORD wServerID);				
	//ö�ٷ���
	tagServerItem * EnumServer(WORD wKindID,WORD wServerID=0);	
	//���ҷ���
	tagServerItem * SearchServer(WORD wKindID,WORD wServerID);
	//��ȡ����
	tagServerItem * GetServerItem(WORD wKindID,WORD wServerID,WORD wTableCount,BYTE cbStoreType);
	
	//������
protected:
	//�����
	tagServerNode * ActiveServerNode();
	//�Ƴ�����
	void RemoveServerNode(tagServerNode * pServerNode);	

	//��������
public:
	//��������
	void ResetData();
};

///////////////////////////////////////////////////////////////////////////////////


#endif