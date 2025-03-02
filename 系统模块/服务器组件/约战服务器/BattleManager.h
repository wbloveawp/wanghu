#ifndef PRIVATE_TABLE_MANAGER_HEAD_FILE
#define PRIVATE_TABLE_MANAGER_HEAD_FILE

#pragma once

//�����ļ�
#include "AfxTempl.h"
#include "BattleServiceHead.h"

///////////////////////////////////////////////////////////////////////////////////
//��������
//#ifdef _DEBUG
//	#define MAX_TABLE_COUNT_IN_CLUB	20								//�������	
//	#define MAX_TABLE_COUNT_OUT_CLUB	4								//�������	
//#else
//	#define MAX_TABLE_COUNT_IN_CLUB	20								//�������	
//	#define MAX_TABLE_COUNT_OUT_CLUB	4								//�������	
//#endif

///////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//��������
struct tagServerItem
{
	WORD								wKindID;					//���ͱ�ʶ
	WORD								wServerID;					//�����ʶ	
	WORD								wServerType;				//��������
	WORD								wTableCount;				//��������	
};

//����״̬
struct tagServerStatus
{
	bool								bRemoved;					//�Ƴ���ʶ
	WORD								wTableUseCount;				//ʹ������
};

//����ڵ�
struct tagServerNode
{
	tagServerItem						ServerItem;					//��������
	tagServerStatus						ServerStatus;				//����״̬
	tagServerNode *						pNextServerNode;			//����ڵ�
};

//���ӽڵ�
struct tagTableNode
{
	//������Ϣ
	tagBattleTableItemEx				TableItem;					//������Ϣ		
	tagBattleTableParamEx 				TableParam;					//���Ӳ���

	//������Ϣ
	tagTableNode *						pNextUserNode;				//���ӽڵ�
	tagTableNode *						pNextKindNode;				//���ӽڵ�	
	tagTableNode *						pNextClubNode;				//���ӽڵ�	
};

//ͷ���ڵ�
struct tagTableNodeHead
{
	INT									nNodeCount;					//�ڵ�����
	tagTableNode *						pTableNodeHead;				//���ӽڵ�
};

//������Ϣ
struct tagTableOwner
{
	//�û���Ϣ
	DWORD								dwUserID;					//�û���ʶ	
	
	//������Ϣ
	tagSocketInfo						SocketInfo;					//������Ϣ	
};

///////////////////////////////////////////////////////////////////////////////////

//���鶨��
typedef CWHArray<tagTableNode *> CTableNodeArray;
typedef CWHArray<tagServerNode *> CServerNodeArray;
typedef CWHArray<tagTableOwner *> CTableOwnerArray;
typedef CWHArray<tagBattleConfigEx*> CBattleConfigArray;
typedef CWHArray<tagBattleTableItemEx*> CTableItemArray;
typedef CWHArray<tagBattleTableParamEx*> CTableParamArray;

//���Ͷ���
typedef CWHStockData<DWORD, tagTableNode> CStockTableNode;
typedef CWHStockData<DWORD, tagTableOwner> CStockTableOwner;
typedef CWHStockData<DWORD, tagBattleConfigEx> CStockBattleConfig;

//��������
typedef CMap<WORD,WORD,tagServerNode*,tagServerNode*> CMapServerNode;
typedef CMap<WORD,WORD,tagTableNodeHead,tagTableNodeHead> CMapTableKindID;
typedef CMap<DWORD,DWORD,tagTableNodeHead,tagTableNodeHead> CMapTableUserID;
typedef CMap<DWORD,DWORD, tagTableNodeHead, tagTableNodeHead> CMapTableClubID;

///////////////////////////////////////////////////////////////////////////////////
//Լս����
class CBattleManager
{
	//���ӹ���
protected:	
	CStockTableNode						m_StockTableNode;			//���Ӳֿ�
	CMapTableUserID						m_MapTableUserID;			//�û�ӳ��
	CMapTableKindID						m_MapTableKindID;			//����ӳ��	
	CMapTableClubID						m_MapTableClubID;			//����ӳ��

	//�������
protected:
	CMapServerNode						m_MapServerNode;			//�����ֵ�
	CServerNodeArray					m_ServerNodeBuffer;			//���仺��

	//���ݲֿ�
protected:
	CStockTableOwner					m_StockTableOwner;			//�����ֿ�	

	//��������
public:
	//���캯��
	CBattleManager();
	//��������
	virtual ~CBattleManager();

	//���ܺ���
public:
	//��������
	VOID ResetData();

	//�������
public:
	//�Ƴ�����
	bool RemoveServer(WORD wKindID,WORD wServerID);	
	//��ӷ���
	bool AppendServer(const tagServerItem & ServerItem);	

	//��������
public:		
	//�Ƴ�����
	void RemoveTableOwner(DWORD dwBattleNum);
	//��������
	tagTableOwner * SearchTableOwner(DWORD dwBattleNum);	
	//��������
	tagTableOwner * CreateTableOwner(DWORD dwBattleNum);

	//���Ӳ���
public:
	//�Ƴ�����
	bool RemoveTable(DWORD dwBattleNum);	
	//�޸�����
	bool ModifyTableOwner(DWORD dwBattleNum, DWORD dwNewOwnerID);
	//ö������
	tagBattleTableItemEx * EnumTableItem(tagBattleTableItemEx* pTableItem);
	//��������
	tagBattleTableItemEx * CreateTable(DWORD dwUserID, DWORD dwClubID, tagBattleConfigEx & BattleConfig,WORD wServerID=0);

	//���ܺ���
public:		
	//��������
	tagBattleTableItemEx * GetTableItem(DWORD dwBattleNum);			
	//���Ӳ���
	tagBattleTableParamEx * GetTableParam(DWORD dwBattleNum);	
	//�������
	bool AddTableItem(const tagBattleTableItemEx & TableItem);	

	//����ö��
public:
	//ö����Ϸ
	WORD EnumTableKindID(POSITION& Position);
	//ö���û�
	DWORD EnumTableUserID(POSITION& Position);	
	//ö�ٲ���
	DWORD EnumTableClubID(POSITION& Position);

	//����ͳ��
public:
	//��������
	WORD GetTableCountByClubID(DWORD dwClubID);
	//��������
	WORD GetTableCountByClubID(DWORD dwUserID, DWORD dwClubID);	
	//��������
	WORD GetTableCountByKindID(DWORD dwUserID, WORD wKindID, DWORD dwClubID);

	//�б����
public:
	//�����б�
	bool GetRandTableList(WORD wTableCount, CTableItemArray& TableItemArray, CTableParamArray& TableParamArray);
	//�����б�
	bool GetKindTableList(WORD wKindID,CTableItemArray & TableItemArray,CTableParamArray & TableParamArray);
	//�����б�
	bool GetUserTableList(DWORD dwUserID,CTableItemArray & TableItemArray,CTableParamArray & TableParamArray);	
	//�����б�
	bool GetClubTableList(DWORD dwClubID, CTableItemArray& TableItemArray, CTableParamArray& TableParamArray);

	//������
protected:
	//�����
	tagServerNode * ActiveServerNode();
	//�Ƴ�����
	void RemoveServerNode(tagServerNode * pServerNode);	

	//���ҷ���
protected:	
	//���ҷ���
	tagServerNode * SearchServerNode(WORD wKindID);	
	//���ҷ���
	tagServerNode * SearchServerNode(WORD wKindID,WORD wServerID);	

	//���Һ���
protected:
	//��������
	tagTableNodeHead * SearchTableByKindID(WORD wKindID);
	//��������
	tagTableNodeHead * SearchTableByUserID(DWORD dwUserID);	
	//��������
	tagTableNodeHead * SearchTableByClubID(DWORD dwClubID);

	//��Ӻ���
private:
	//��ӽڵ�
	bool InsertKindTableNode(WORD wKindID, tagTableNode * pTableNode);
	//��ӽڵ�
	bool InsertUserTableNode(DWORD dwUserID, tagTableNode * pTableNode);
	//��ӽڵ�
	bool InsertClubTableNode(DWORD dwClubID, tagTableNode* pTableNode);

	//�Ƴ�����
private:
	//�Ƴ��ڵ�
	bool RemoveKindTableNode(WORD wKindID, tagTableNode * pTableNode);
	//�Ƴ��ڵ�
	bool RemoveUserTableNode(DWORD dwUserID, tagTableNode * pTableNode);	
	//�Ƴ��ڵ�
	bool RemoveClubTableNode(DWORD dwClubID, tagTableNode* pTableNode);

	//��������
private:
	//���ɱ��
	DWORD BuildNumber(int nNumberLen);	
	//��������
	void BuildPassword(TCHAR szPassword[LEN_PASSWORD],WORD wBufferCount);	
};

///////////////////////////////////////////////////////////////////////////////////


#endif