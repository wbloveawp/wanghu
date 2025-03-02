#ifndef SERVER_LIST_MANAGER_HEAD_FILE
#define SERVER_LIST_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "PlazaControlsHead.h"

//////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//��������
enum enItemGenre
{
	ItemGenre_Type,					//��Ϸ����
	ItemGenre_Kind,					//��Ϸ����
	ItemGenre_Node,					//��Ϸ�ڵ�
	ItemGenre_Server,				//��Ϸ����
	ItemGenre_Custom,				//��Ϸ�ƶ�
	ItemGenre_Inside,				//��Ϸ�ڲ�
};

//����״̬
enum enServerStatus
{
	ServerStatus_Normal,			//����״̬
	ServerStatus_Entrance,			//����ʹ��
	ServerStatus_EverEntrance,		//��������
};

//////////////////////////////////////////////////////////////////////////

//��˵��
class CGameListItem;
class CGameTypeItem;
class CGameKindItem;
class CGameNodeItem;
class CGameCustomItem;
class CGameServerItem;
class CGameInsideItem;

//����˵��
typedef CWHArray<CGameListItem *>	CGameListItemArray;

//����˵��
typedef CMap<WORD,WORD,CGameTypeItem *,CGameTypeItem * &>		CGameTypeItemMap;
typedef CMap<WORD,WORD,CGameKindItem *,CGameKindItem * &>		CGameKindItemMap;
typedef CMap<WORD,WORD,CGameNodeItem *,CGameNodeItem * &>		CGameNodeItemMap;
typedef CMap<WORD,WORD,CGameCustomItem *,CGameCustomItem * &>	CGameCustomItemMap;
typedef CMap<WORD,WORD,CGameServerItem *,CGameServerItem * &>	CGameServerItemMap;

//////////////////////////////////////////////////////////////////////////

//�б�ӿ�
interface IServerListViewSink
{
	//״̬֪ͨ
public:
	//���֪ͨ
	virtual VOID OnGameItemFinish()=NULL;
	//���֪ͨ
	virtual VOID OnGameKindFinish(WORD wKindID)=NULL;

	//����֪ͨ
public:
	//����֪ͨ
	virtual VOID OnGameItemInsert(CGameListItem * pGameListItem)=NULL;
	//����֪ͨ
	virtual VOID OnGameItemUpdate(CGameListItem * pGameListItem)=NULL;
	//ɾ��֪ͨ
	virtual VOID OnGameItemDelete(CGameListItem * pGameListItem)=NULL;

	//����֪ͨ
public:
	//����֪ͨ
	virtual VOID OnUpdateOnlineCount(DWORD dwOnlineCount)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//�б�����
class PLAZA_CONTROL_CLASS CGameListItem
{
	//��Ԫ����
	friend class CServerListManager;

	//��������
protected:
	enItemGenre						m_ItemGenre;						//��������
	CGameListItem *					m_pParentListItem;					//��������

	//��������
public:
	HTREEITEM						m_hTreeItemNormal;					//������
	HTREEITEM						m_hTreeItemAttach;					//�󶨾��

	//��������
protected:
	//���캯��
	CGameListItem(enItemGenre ItemGenre);
	//��������
	virtual ~CGameListItem();

	//���ܺ���
public:
	//��ȡ����
	enItemGenre GetItemGenre() { return m_ItemGenre; }
	//��ȡ����
	CGameListItem * GetParentListItem() { return m_pParentListItem; }

	//���غ���
public:
	//��������
	virtual WORD GetSortID()=NULL;
};

//////////////////////////////////////////////////////////////////////////

//����ṹ
class PLAZA_CONTROL_CLASS CGameTypeItem : public CGameListItem
{
	//��������
public:
	tagGameType						m_GameType;							//������Ϣ

	//��������
public:
	//���캯��
	CGameTypeItem();
	//��������
	virtual ~CGameTypeItem();

	//���غ���
public:
	//��������
	virtual WORD GetSortID() { return m_GameType.wSortID; }
};

//////////////////////////////////////////////////////////////////////////

//���ͽṹ
class PLAZA_CONTROL_CLASS CGameKindItem : public CGameListItem
{
	//��������
public:
	tagGameKind						m_GameKind;							//������Ϣ

	//���±���
public:
	bool							m_bUpdateItem;						//���±�־
	DWORD							m_dwUpdateTime;						//����ʱ��

	//���¾��
public:
	HTREEITEM						m_hItemNormalUpdate;				//���¾��
	HTREEITEM						m_hItemAttachUpdate;				//���¾��

	//��չ����
public:
	DWORD							m_dwProcessVersion;					//��Ϸ�汾

	//��������
public:
	//���캯��
	CGameKindItem();
	//��������
	virtual ~CGameKindItem();

	//���غ���
public:
	//��������
	virtual WORD GetSortID() { return m_GameKind.wSortID; }
};

//////////////////////////////////////////////////////////////////////////

//�ڵ�ṹ
class PLAZA_CONTROL_CLASS CGameNodeItem : public CGameListItem
{
	//��������
public:
	tagGameNode						m_GameNode;							//�ڵ���Ϣ

	//��������
public:
	//���캯��
	CGameNodeItem();
	//��������
	virtual ~CGameNodeItem();

	//���غ���
public:
	//��������
	virtual WORD GetSortID() { return m_GameNode.wSortID; }
};

//////////////////////////////////////////////////////////////////////////

//����ṹ
class PLAZA_CONTROL_CLASS CGameServerItem : public CGameListItem
{
	//��������
public:
	tagPCGameServer					m_GameServer;						//������Ϣ

	//��չ����
public:
	enServerStatus					m_ServerStatus;						//����״̬

	//��������
public:
	CGameKindItem *					m_pGameKindItem;					//��Ϸ����

	//��������
public:
	//���캯��
	CGameServerItem();
	//��������
	virtual ~CGameServerItem();

	//���غ���
public:
	//��������
	virtual WORD GetSortID() { return m_GameServer.wSortID; }
};

//////////////////////////////////////////////////////////////////////////

//���ƽṹ
class PLAZA_CONTROL_CLASS CGameCustomItem : public CGameListItem
{
	//��������
public:
	tagGameCustom					m_GameCustom;						//������Ϣ

	//��������
public:
	//���캯��
	CGameCustomItem();
	//��������
	virtual ~CGameCustomItem();

	//���غ���
public:
	//��������
	virtual WORD GetSortID() { return m_GameCustom.wSortID; }
};

//////////////////////////////////////////////////////////////////////////

//�ڲ��ṹ
class PLAZA_CONTROL_CLASS CGameInsideItem : public CGameListItem
{
	//��������
public:
	DWORD							m_dwInsideID;						//�ڲ� ID

	//��������
public:
	//���캯��
	CGameInsideItem();
	//��������
	virtual ~CGameInsideItem();

	//���غ���
public:
	//��������
	virtual WORD GetSortID() { return 0L; }
};

//////////////////////////////////////////////////////////////////////////

//�б����
class PLAZA_CONTROL_CLASS CServerListManager
{
	//��������
protected:
	CGameTypeItemMap				m_GameTypeItemMap;					//��������
	CGameKindItemMap				m_GameKindItemMap;					//��������
	CGameNodeItemMap				m_GameNodeItemMap;					//�ڵ�����
	CGameCustomItemMap				m_GameCustomItemMap;				//��������
	CGameServerItemMap				m_GameServerItemMap;				//��������

	//�ں˱���
protected:
	CGameListItemArray				m_GameListItemWait;					//�ȴ�����
	IServerListViewSink *			m_pIServerListViewSink;				//�ص��ӿ�

	//��̬����
protected:
	static CServerListManager *		m_pServerListManager;				//�б����

	//��������
public:
	//���캯��
	CServerListManager();
	//��������
	virtual ~CServerListManager();

	//���ú���
public:
	//���ýӿ�
	VOID SetServerListViewSink(IServerListViewSink * pIServerListViewSink);

	//״̬֪ͨ
public:
	//���֪ͨ
	VOID OnEventListFinish();
	//���֪ͨ
	VOID OnEventKindFinish(WORD wKindID);

	//��������
public:
	//��������
	VOID SetKindOnLineCount(WORD wKindID, DWORD dwOnLineCount);
	//��������
	VOID SetServerOnLineCount(WORD wServerID, DWORD dwOnLineCount);
	//��������
	VOID UpdateOnlineCount();

	//���뺯��
public:
	//��������
	bool InsertGameType(tagGameType * pGameType);
	//��������
	bool InsertGameKind(tagGameKind * pGameKind);
	//����ڵ�
	bool InsertGameNode(tagGameNode * pGameNode);
	//���붨��
	bool InsertGameCustom(tagGameCustom * pGameCustom);
	//���뷿��
	bool InsertGameServer(tagPCGameServer * pGameServer);

	//ɾ������
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

	//ö�ٺ���
public:
	//ö������
	CGameTypeItem * EmunGameTypeItem(POSITION & Pos);
	//ö������
	CGameKindItem * EmunGameKindItem(POSITION & Pos);
	//ö�ٽڵ�
	CGameNodeItem * EmunGameNodeItem(POSITION & Pos);
	//ö�ٶ���
	CGameCustomItem * EmunGameCustomItem(POSITION & Pos);
	//ö�ٷ���
	CGameServerItem * EmunGameServerItem(POSITION & Pos);

	//���Һ���
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

	//��Ŀ����
public:
	//������Ŀ
	DWORD GetGameTypeCount() { return (DWORD)m_GameTypeItemMap.GetCount(); }
	//������Ŀ
	DWORD GetGameKindCount() { return (DWORD)m_GameKindItemMap.GetCount(); }
	//�ڵ���Ŀ
	DWORD GetGameNodeCount() { return (DWORD)m_GameNodeItemMap.GetCount(); }
	//������Ŀ
	DWORD GetGameCustomCount() { return (DWORD)m_GameCustomItemMap.GetCount(); }
	//������Ŀ
	DWORD GetGameServerCount() { return (DWORD)m_GameServerItemMap.GetCount(); }

	//�����б�
public:
	//�����б�
	VOID UpdateGameKind(WORD wKindID);

	//��̬����
public:
	//��ȡ����
	static CServerListManager * GetInstance() { return m_pServerListManager; }
};

//////////////////////////////////////////////////////////////////////////

#endif
