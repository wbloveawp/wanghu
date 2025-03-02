#ifndef SERVER_LIST_MANAGER_HEAD_FILE
#define SERVER_LIST_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"

//////////////////////////////////////////////////////////////////////////
//�궨��

//����״̬
#define FM_DELETE					0x0001								//ɾ������
#define FM_SORT_ID					0x0002								//�������
#define FM_KIND_ID					0x0004								//���͹���
#define FM_NODE_ID					0x0008								//�ڵ����
#define FM_SERVER_NAME				0x0010								//���ֹ���

//////////////////////////////////////////////////////////////////////////

//��Ϸ����
class CGameItem
{
	//��������
public:
	bool							m_bDisuse;							//������־

	//��������
protected:
	//���캯��
	CGameItem() { m_bDisuse=false; }
};

//////////////////////////////////////////////////////////////////////////

//����ṹ
class CGameTypeItem : public CGameItem
{
	//��������
public:
	tagGameType						m_GameType;							//������Ϣ

	//��������
public:
	//���캯��
	CGameTypeItem();
};

//////////////////////////////////////////////////////////////////////////

//���Ͷ���
typedef CWHArray<tagGameKindEx>	CGameKindArray;					//��������

//���ͽṹ
class CGameKindItem : public CGameItem
{
	//��������
public:	
	tagGameKindEx					m_GameKind;							//��Ϸ����
	DWORD							m_dwOnLineCount;					//��������	

	//������Ϣ
public:
	SCORE							m_lJackpotScore;					//���ػ���
	ULONGLONG						m_ulLastTickCount;					//����ʱ��

	//��������
public:
	//���캯��
	CGameKindItem();

	//���ܺ���
public:
	//���ö���
	VOID ResetKindItem();

	//��Ϣ����
public:
	//���ͱ�ʶ
	WORD GetKindID() { return m_GameKind.wKindID; }
};

//////////////////////////////////////////////////////////////////////////

//�ڵ�ṹ
class CGameNodeItem : public CGameItem
{
	//��������
public:
	tagGameNode						m_GameNode;							//�ڵ���Ϣ

	//��������
public:
	//���캯��
	CGameNodeItem();
};

//////////////////////////////////////////////////////////////////////////

//����ṹ
class CGameServerItem : public CGameItem
{
	//��������
public:
	tagGameServer					m_GameServer;						//������Ϣ

	//��������
public:
	//���캯��
	CGameServerItem();
};

//////////////////////////////////////////////////////////////////////////

//���ƽṹ
class CGameCustomItem : public CGameItem
{
	//��������
public:
	tagGameCustom					m_GameCustom;						//������Ϣ

	//��������
public:
	//���캯��
	CGameCustomItem();
};

//////////////////////////////////////////////////////////////////////////

//�������
class CGameFilterItem : public CGameItem
{
	//��������
public:
	tagGameFilter					m_GameFilter;						//������Ϣ

	//��������
public:
	//���캯��
	CGameFilterItem();

	//���ܺ���
public:
	//�����ж�
	bool InsertVerdict();
	//�������
	WORD FilterSortID(WORD wSortID);
	//���͹���
	WORD FilterKindID(WORD wKindID);
	//�ڵ����
	WORD FilterNodeID(WORD wNodeID);
	//���ֹ���
	LPCTSTR FilterServerName(LPCTSTR pszServerName);
};

//////////////////////////////////////////////////////////////////////////

//����˵��
typedef CWHArray<CGameTypeItem *>	CGameTypeItemArray;
typedef CWHArray<CGameKindItem *>	CGameKindItemArray;
typedef CWHArray<CGameNodeItem *>	CGameNodeItemArray;
typedef CWHArray<CGameCustomItem *> CGameCustomItemArray;
typedef CWHArray<CGameServerItem *> CGameServerItemArray;
typedef CWHArray<CGameFilterItem *> CGameFilterItemArray;

//����˵��
typedef CMap<WORD,WORD,CGameTypeItem *,CGameTypeItem * &> CTypeItemMap;
typedef CMap<WORD,WORD,CGameKindItem *,CGameKindItem * &> CKindItemMap;
typedef CMap<WORD,WORD,CGameNodeItem *,CGameNodeItem * &> CNodeItemMap;
typedef CMap<WORD,WORD,CGameCustomItem *,CGameCustomItem * &> CCustomItemMap;
typedef CMap<WORD,WORD,CGameServerItem *,CGameServerItem * &> CServerItemMap;
typedef CMap<WORD,WORD,CGameFilterItem *,CGameFilterItem * &> CFilterItemMap;

//////////////////////////////////////////////////////////////////////////

//�б����
class CServerListManager
{
	//��������
protected:
	CTypeItemMap					m_TypeItemMap;						//��������
	CKindItemMap					m_KindItemMap;						//��������
	CNodeItemMap					m_NodeItemMap;						//�ڵ�����
	CCustomItemMap					m_CustomItemMap;					//��������
	CServerItemMap					m_ServerItemMap;					//��������
	CFilterItemMap					m_FilterItemMap;					//��������

	//�洢����
protected:
	CGameTypeItemArray				m_TypeItemBuffer;					//��������
	CGameKindItemArray				m_KindItemBuffer;					//��������
	CGameNodeItemArray				m_NodeItemBuffer;					//�ڵ�����
	CGameCustomItemArray			m_CustomItemBuffer;					//��������
	CGameServerItemArray			m_ServerItemBuffer;					//��������
	CGameFilterItemArray			m_FilterItemBuffer;					//��������

	//��������
public:
	//���캯��
	CServerListManager();
	//��������
	virtual ~CServerListManager();

	//����ӿ�
public:
	//�����б�
	VOID ResetServerList();
	//�����ں�
	VOID CleanKernelItem();
	//������
	VOID CleanServerItem();
	//�����ں�
	VOID DisuseKernelItem();
	//��������
	VOID DisuseServerItem();

	//ͳ�ƽӿ�
public:
	//ͳ������
	DWORD CollectOnlineInfo();
	//��������
	DWORD CollectOnlineInfo(WORD wKindID);

	//����ӿ�
public:
	//��������
	bool InsertGameType(tagGameType * pGameType);
	//��������
	bool InsertGameKind(tagGameKindEx * pGameKind);
	//����ڵ�
	bool InsertGameNode(tagGameNode * pGameNode);
	//���붨��
	bool InsertGameCustom(tagGameCustom * pGameCustom);
	//���뷿��
	bool InsertGameServer(tagGameServer * pGameServer);
	//�������
	bool InsertGameFilter(tagGameFilter * pGameFilter);

	//ɾ���ӿ�
public:
	//ɾ������
	bool DeleteGameType(WORD wTypeID);
	//ɾ������
	bool DeleteGameKind(WORD wKindID);
	//ɾ���ڵ�
	bool DeleteGameNode(WORD wNodeID);
	//ɾ������
	bool DeleteGameServer(WORD wServerID);
	//ɾ������
	bool DeleteGameCustom(WORD wCustomID);
	//ɾ������
	bool DeleteGameFilter(WORD wServerID);

	//ö�ٽӿ�
public:
	//ö������
	CGameTypeItem * EmunGameTypeItem(POSITION & Position);
	//ö������
	CGameKindItem * EmunGameKindItem(POSITION & Position);
	//ö�ٽڵ�
	CGameNodeItem * EmunGameNodeItem(POSITION & Position);
	//ö�ٷ���
	CGameServerItem * EmunGameServerItem(POSITION & Position);
	//ö�ٶ���
	CGameCustomItem * EmunGameCustomItem(POSITION & Position);
	//ö�ٹ���
	CGameFilterItem * EmunGameFilterItem(POSITION & Position);

	//���ҽӿ�
public:
	//��������
	CGameTypeItem * SearchGameType(WORD wTypeID);
	//��������
	CGameKindItem * SearchGameKind(WORD wKindID);
	//���ҽڵ�
	CGameNodeItem * SearchGameNode(WORD wNodeID);
	//���ҷ���
	CGameServerItem * SearchGameServer(WORD wServerID);
	//���Ҷ���
	CGameCustomItem * SearchGameCustom(WORD wCustomID);
	//���ҹ���
	CGameFilterItem * SearchGameFilter(WORD wServerID);

	//��Ŀ�ӿ�
public:
	//������Ŀ
	DWORD GetGameTypeCount() { return (DWORD)m_TypeItemMap.GetCount(); }
	//������Ŀ
	DWORD GetGameKindCount() { return (DWORD)m_KindItemMap.GetCount(); }
	//�ڵ���Ŀ
	DWORD GetGameNodeCount() { return (DWORD)m_NodeItemMap.GetCount(); }
	//������Ŀ
	DWORD GetGameCustomCount() { return (DWORD)m_CustomItemMap.GetCount(); }
	//������Ŀ
	DWORD GetGameServerCount() { return (DWORD)m_ServerItemMap.GetCount(); }
	//������Ŀ
	DWORD GetGameFilterCount() { return (DWORD)m_FilterItemMap.GetCount(); }
};

//////////////////////////////////////////////////////////////////////////

#endif