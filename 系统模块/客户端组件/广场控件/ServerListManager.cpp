#include "StdAfx.h"
#include "ServerListManager.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
CServerListManager * CServerListManager::m_pServerListManager=NULL;		//�б����

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameListItem::CGameListItem(enItemGenre ItemGenre)
{
	//��������
	m_ItemGenre=ItemGenre;
	m_pParentListItem=NULL;

	//��������
	m_hTreeItemNormal=NULL;
	m_hTreeItemAttach=NULL;

	return;
}

//��������
CGameListItem::~CGameListItem()
{
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameTypeItem::CGameTypeItem() : CGameListItem(ItemGenre_Type)
{
	//��������
	ZeroMemory(&m_GameType,sizeof(m_GameType));
}

//��������
CGameTypeItem::~CGameTypeItem()
{
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameKindItem::CGameKindItem() : CGameListItem(ItemGenre_Kind)
{
	//���±���
	m_dwUpdateTime=0;
	m_bUpdateItem=false;

	//���¾��
	m_hItemNormalUpdate=NULL;
	m_hItemAttachUpdate=NULL;

	//��չ����
	m_dwProcessVersion=0L;

	//��������
	ZeroMemory(&m_GameKind,sizeof(m_GameKind));

	return;
}

//��������
CGameKindItem::~CGameKindItem()
{
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameNodeItem::CGameNodeItem() : CGameListItem(ItemGenre_Node)
{
	//��������
	ZeroMemory(&m_GameNode,sizeof(m_GameNode));
}

//��������
CGameNodeItem::~CGameNodeItem()
{
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameServerItem::CGameServerItem() : CGameListItem(ItemGenre_Server)
{
	//��������
	m_pGameKindItem=NULL;

	//��չ����
	m_ServerStatus=ServerStatus_Normal;

	//��������
	ZeroMemory(&m_GameServer,sizeof(m_GameServer));
}

//��������
CGameServerItem::~CGameServerItem()
{
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameCustomItem::CGameCustomItem() : CGameListItem(ItemGenre_Custom)
{
	//��������
	ZeroMemory(&m_GameCustom,sizeof(m_GameCustom));
}

//��������
CGameCustomItem::~CGameCustomItem()
{
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameInsideItem::CGameInsideItem() : CGameListItem(ItemGenre_Inside)
{
}

//��������
CGameInsideItem::~CGameInsideItem()
{
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CServerListManager::CServerListManager()
{
	//�ӿڱ���
	m_pIServerListViewSink=NULL;

	//��������
	m_GameTypeItemMap.InitHashTable(PRIME_TYPE);
	m_GameKindItemMap.InitHashTable(PRIME_KIND);
	m_GameNodeItemMap.InitHashTable(PRIME_NODE);
	m_GameServerItemMap.InitHashTable(PRIME_SERVER);
	m_GameCustomItemMap.InitHashTable(PRIME_CUSTOM);

	//���ö���
	ASSERT(m_pServerListManager==NULL);
	if (m_pServerListManager==NULL) m_pServerListManager=this;

	return;
}

//��������
CServerListManager::~CServerListManager()
{
	//��������
	WORD wKey=0;

	//ɾ������
	CGameTypeItem * pGameTypeItem=NULL;
	POSITION Position=m_GameTypeItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GameTypeItemMap.GetNextAssoc(Position,wKey,pGameTypeItem);
		SafeDelete(pGameTypeItem);
	}
	m_GameTypeItemMap.RemoveAll();

	//ɾ������
	CGameKindItem * pGameKindItem=NULL;
	Position=m_GameKindItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GameKindItemMap.GetNextAssoc(Position,wKey,pGameKindItem);
		SafeDelete(pGameKindItem);
	}
	m_GameKindItemMap.RemoveAll();

	//ɾ���ڵ�
	CGameNodeItem * pGameNodeItem=NULL;
	Position=m_GameNodeItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GameNodeItemMap.GetNextAssoc(Position,wKey,pGameNodeItem);
		SafeDelete(pGameNodeItem);
	}
	m_GameNodeItemMap.RemoveAll();

	//ɾ������
	CGameServerItem * pGameServerItem=NULL;
	Position=m_GameServerItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GameServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);
		SafeDelete(pGameServerItem);
	}
	m_GameServerItemMap.RemoveAll();

	//ɾ������
	CGameCustomItem * pGameCustomItem=NULL;
	Position=m_GameCustomItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GameCustomItemMap.GetNextAssoc(Position,wKey,pGameCustomItem);
		SafeDelete(pGameCustomItem);
	}
	m_GameCustomItemMap.RemoveAll();

	//�ͷŶ���
	ASSERT(m_pServerListManager==this);
	if (m_pServerListManager==this) m_pServerListManager=NULL;

	return;
}

//���ýӿ�
VOID CServerListManager::SetServerListViewSink(IServerListViewSink * pIServerListViewSink)
{
	//���ñ���
	m_pIServerListViewSink=pIServerListViewSink;

	return;
}

//���֪ͨ
VOID CServerListManager::OnEventListFinish()
{
	//�¼�֪ͨ
	ASSERT(m_pIServerListViewSink!=NULL);
	if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameItemFinish();

	return;
}

//���֪ͨ
VOID CServerListManager::OnEventKindFinish(WORD wKindID)
{
	//�¼�֪ͨ
	ASSERT(m_pIServerListViewSink!=NULL);
	if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameKindFinish(wKindID);

	return;
}

//��������
VOID CServerListManager::SetKindOnLineCount(WORD wKindID, DWORD dwOnLineCount)
{
	//��������
	CGameKindItem * pGameKindItem=SearchGameKind(wKindID);

	//��������
	if (pGameKindItem!=NULL)
	{
		//���ñ���
		pGameKindItem->m_GameKind.dwOnLineCount=dwOnLineCount;

		//�¼�֪ͨ
		ASSERT(m_pIServerListViewSink!=NULL);
		if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameItemUpdate(pGameKindItem);
	}

	return;
}

//��������
VOID CServerListManager::SetServerOnLineCount(WORD wServerID, DWORD dwOnLineCount)
{
	//��������
	CGameServerItem * pGameServerItem=SearchGameServer(wServerID);

	//��������
	if (pGameServerItem!=NULL)
	{
		//���ñ���
		pGameServerItem->m_GameServer.dwOnLineCount=dwOnLineCount;

		//�¼�֪ͨ
		ASSERT(m_pIServerListViewSink!=NULL);
		if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameItemUpdate(pGameServerItem);
	}

	return;
}

//��������
VOID CServerListManager::UpdateOnlineCount()
{
	//����������
	DWORD dwOnlineCount = 0;
	CGameKindItem *pGameKindItem = NULL;
	POSITION pos = NULL;
	for( DWORD i = 0; i < GetGameKindCount(); i++ )
	{
		pGameKindItem = EmunGameKindItem(pos);
		if( !pGameKindItem ) break;

		dwOnlineCount += pGameKindItem->m_GameKind.dwOnLineCount;
	}

	//�¼�֪ͨ
	ASSERT(m_pIServerListViewSink!=NULL);
	if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnUpdateOnlineCount(dwOnlineCount);
}

//��������
bool CServerListManager::InsertGameType(tagGameType * pGameType)
{
	//Ч�����
	ASSERT(pGameType!=NULL);
	if (pGameType==NULL) return false;

	//��������
	CGameTypeItem * pGameTypeItem=NULL;
	bool bInsert=(m_GameTypeItemMap.Lookup(pGameType->wTypeID,pGameTypeItem)==FALSE);

	//��ȡ�洢
	if (bInsert==true)
	{
		pGameTypeItem=new CGameTypeItem;
		if (pGameTypeItem==NULL) return false;
	}

	//��������
	CopyMemory(&pGameTypeItem->m_GameType,pGameType,sizeof(tagGameType));

	//Ѱ�Ҹ���
	if (pGameType->wJoinID!=0)
	{
		//Ѱ�Ҹ���
		pGameTypeItem->m_pParentListItem=SearchGameType(pGameType->wJoinID);

		//�����ж�
		if (pGameTypeItem->m_pParentListItem==NULL) 
		{
			m_GameListItemWait.Add(pGameTypeItem);
			return true;
		}
	}

	//���봦��
	if (bInsert==true)
	{
		//��������
		m_GameTypeItemMap[pGameType->wTypeID]=pGameTypeItem;

		//�������
		m_pIServerListViewSink->OnGameItemInsert(pGameTypeItem);
	}
	else
	{
		//�������
		m_pIServerListViewSink->OnGameItemUpdate(pGameTypeItem);
	}

	return true;
}

//��������
bool CServerListManager::InsertGameKind(tagGameKind * pGameKind)
{
	//Ч�����
	ASSERT(pGameKind!=NULL);
	if (pGameKind==NULL) return false;

	//��������
	CGameKindItem * pGameKindItem=NULL;
	bool bInsert=(m_GameKindItemMap.Lookup(pGameKind->wKindID,pGameKindItem)==FALSE);

	//��ȡ�洢
	if (bInsert==true)
	{
		pGameKindItem=new CGameKindItem;
		if (pGameKindItem==NULL) return false;
	}

	//��ȡ�汾
	if (bInsert==true)
	{
		LPCTSTR pszProcessName=CW2CT(pGameKind->szProcessName);
		CWHService::GetModuleVersion(pszProcessName,pGameKindItem->m_dwProcessVersion);
	}

	//��������
	CopyMemory(&pGameKindItem->m_GameKind,pGameKind,sizeof(tagGameKind));

	//Ѱ�Ҹ���
	/*if (pGameKind->wJoinID!=0)
	{
		pGameKindItem->m_pParentListItem=SearchGameKind(pGameKind->wJoinID);
	}
	else
	{*/
		pGameKindItem->m_pParentListItem=SearchGameType(pGameKind->wTypeID);
	//}

	//�����ж�
	if (pGameKindItem->m_pParentListItem==NULL) 
	{
		m_GameListItemWait.Add(pGameKindItem);
		return true;
	}

	//���봦��
	if (bInsert==true)
	{
		//��������
		m_GameKindItemMap[pGameKind->wKindID]=pGameKindItem;

		//��������
		m_pIServerListViewSink->OnGameItemInsert(pGameKindItem);
	}
	else
	{
		//��������
		m_pIServerListViewSink->OnGameItemUpdate(pGameKindItem);
	}

	return true;
}

//����ڵ�
bool CServerListManager::InsertGameNode(tagGameNode * pGameNode)
{
	//Ч�����
	ASSERT(pGameNode!=NULL);
	if (pGameNode==NULL) return false;

	//��������
	CGameNodeItem * pGameNodeItem=NULL;
	bool bInsert=(m_GameNodeItemMap.Lookup(pGameNode->wNodeID,pGameNodeItem)==FALSE);

	//��ȡ�洢
	if (bInsert==true)
	{
		pGameNodeItem=new CGameNodeItem;
		if (pGameNodeItem==NULL) return false;
	}

	//��������
	CopyMemory(&pGameNodeItem->m_GameNode,pGameNode,sizeof(tagGameNode));

	//Ѱ�Ҹ���
	if (pGameNode->wJoinID!=0)
	{
		pGameNodeItem->m_pParentListItem=SearchGameNode(pGameNode->wJoinID);
	}
	else
	{
		pGameNodeItem->m_pParentListItem=SearchGameKind(pGameNode->wKindID);
	}

	//�����ж�
	if (pGameNodeItem->m_pParentListItem==NULL) 
	{
		m_GameListItemWait.Add(pGameNodeItem);
		return true;
	}

	//���봦��
	if (bInsert==true)
	{
		//��������
		m_GameNodeItemMap[pGameNode->wNodeID]=pGameNodeItem;

		//��������
		m_pIServerListViewSink->OnGameItemInsert(pGameNodeItem);
	}
	else 
	{
		//��������
		m_pIServerListViewSink->OnGameItemUpdate(pGameNodeItem);
	}

	return true;
}

//���붨��
bool CServerListManager::InsertGameCustom(tagGameCustom * pGameCustom)
{
	//Ч�����
	ASSERT(pGameCustom!=NULL);
	if (pGameCustom==NULL) return false;

	//��������
	CGameCustomItem * pGameCustomItem=NULL;
	bool bInsert=(m_GameCustomItemMap.Lookup(pGameCustom->wPageID,pGameCustomItem)==FALSE);

	//��ȡ�洢
	if (bInsert==true)
	{
		pGameCustomItem=new CGameCustomItem;
		if (pGameCustomItem==NULL) return false;
	}

	//��������
	CopyMemory(&pGameCustomItem->m_GameCustom,pGameCustom,sizeof(tagGameCustom));

	//Ѱ�Ҹ���
	if ((pGameCustom->wNodeID!=0)||(pGameCustom->wKindID!=0))
	{
		//Ѱ�Ҹ���
		if (pGameCustom->wNodeID!=0) pGameCustomItem->m_pParentListItem=SearchGameNode(pGameCustom->wNodeID);
		else if (pGameCustom->wKindID!=0) pGameCustomItem->m_pParentListItem=SearchGameKind(pGameCustom->wKindID);

		//�����ж�
		if (pGameCustomItem->m_pParentListItem==NULL) 
		{
			m_GameListItemWait.Add(pGameCustomItem);
			return true;
		}
	}

	//���봦��
	if (bInsert==true)
	{
		//��������
		m_GameCustomItemMap[pGameCustom->wPageID]=pGameCustomItem;

		//��������
		m_pIServerListViewSink->OnGameItemInsert(pGameCustomItem);
	}
	else 
	{
		//��������
		m_pIServerListViewSink->OnGameItemUpdate(pGameCustomItem);
	}

	return true;
}

//���뷿��
bool CServerListManager::InsertGameServer(tagPCGameServer * pGameServer)
{
	//Ч�����
	ASSERT(pGameServer!=NULL);
	if (pGameServer==NULL) return false;

	//��������
	CGameServerItem * pGameServerItem=NULL;
	bool bInsert=(m_GameServerItemMap.Lookup(pGameServer->wServerID,pGameServerItem)==FALSE);

	//��ȡ�洢
	if (bInsert==true)
	{
		pGameServerItem=new CGameServerItem;
		if (pGameServerItem==NULL) return false;
	}

	//��������
	CopyMemory(&pGameServerItem->m_GameServer,pGameServer,sizeof(tagPCGameServer));

	//Ѱ�Ҹ���
	if (pGameServer->wNodeID!=0)
	{
		pGameServerItem->m_pGameKindItem=SearchGameKind(pGameServer->wKindID);
		pGameServerItem->m_pParentListItem=SearchGameNode(pGameServer->wNodeID);
	}
	else
	{
		pGameServerItem->m_pGameKindItem=SearchGameKind(pGameServer->wKindID);
		pGameServerItem->m_pParentListItem=SearchGameKind(pGameServer->wKindID);
	}

	//�����ж�
	if (pGameServerItem->m_pParentListItem==NULL) 
	{
		m_GameListItemWait.Add(pGameServerItem);
		return true;
	}

	//���봦��
	if (bInsert==true)
	{
		//��������
		m_GameServerItemMap[pGameServer->wServerID]=pGameServerItem;

		//��������
		m_pIServerListViewSink->OnGameItemInsert(pGameServerItem);
	}
	else 
	{
		//��������
		m_pIServerListViewSink->OnGameItemUpdate(pGameServerItem);
	}

	return true;
}

//ɾ������
bool CServerListManager::DeleteGameType(WORD wTypeID)
{
	//��������
	CGameTypeItem * pGameTypeItem=NULL;
	if (m_GameTypeItemMap.Lookup(wTypeID,pGameTypeItem)==FALSE) return false;

	//ɾ��֪ͨ
	ASSERT(m_pIServerListViewSink!=NULL);
	if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameItemDelete(pGameTypeItem);

	//��������
	SafeDelete(pGameTypeItem);

	//ɾ������
	m_GameTypeItemMap.RemoveKey(wTypeID);

	return true;
}

//ɾ������
bool CServerListManager::DeleteGameKind(WORD wKindID)
{
	//��������
	CGameKindItem * pGameKindItem=NULL;
	if (m_GameKindItemMap.Lookup(wKindID,pGameKindItem)==FALSE) return false;

	//ɾ��֪ͨ
	ASSERT(m_pIServerListViewSink!=NULL);
	if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameItemDelete(pGameKindItem);

	//ɾ������
	SafeDelete(pGameKindItem);

	//ɾ������
	m_GameKindItemMap.RemoveKey(wKindID);

	return true;
}

//ɾ���ڵ�
bool CServerListManager::DeleteGameNode(WORD wNodeID)
{
	//���ҽڵ�
	CGameNodeItem * pGameNodeItem=NULL;
	if (m_GameNodeItemMap.Lookup(wNodeID,pGameNodeItem)==FALSE) return false;

	//ɾ��֪ͨ
	ASSERT(m_pIServerListViewSink!=NULL);
	if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameItemDelete(pGameNodeItem);

	//ɾ������
	SafeDelete(pGameNodeItem);

	//ɾ������
	m_GameNodeItemMap.RemoveKey(wNodeID);

	return false;
}

//ɾ������
bool CServerListManager::DeleteGameServer(WORD wServerID)
{
	//���ҷ���
	CGameServerItem * pGameServerItem=NULL;
	if (m_GameServerItemMap.Lookup(wServerID,pGameServerItem)==FALSE) return false;

	//ɾ��֪ͨ
	ASSERT(m_pIServerListViewSink!=NULL);
	if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameItemDelete(pGameServerItem);

	//ɾ������
	SafeDelete(pGameServerItem);

	//ɾ������
	m_GameServerItemMap.RemoveKey(wServerID);

	return false;
}

//ɾ������
bool CServerListManager::DeleteGameCustom(WORD wCustomID)
{
	//��������
	CGameCustomItem * pGameCustomItem=NULL;
	if (m_GameCustomItemMap.Lookup(wCustomID,pGameCustomItem)==FALSE) return false;

	//ɾ��֪ͨ
	ASSERT(m_pIServerListViewSink!=NULL);
	if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameItemDelete(pGameCustomItem);

	//ɾ������
	SafeDelete(pGameCustomItem);

	//ɾ������
	m_GameCustomItemMap.RemoveKey(wCustomID);

	return true;
}

//ö������
CGameTypeItem * CServerListManager::EmunGameTypeItem(POSITION & Position)
{
	//��������
	if (Position==NULL) Position=m_GameTypeItemMap.GetStartPosition();
	if (Position==NULL) return NULL;

	//���ҷ���
	WORD wKey=0;
	CGameTypeItem * pGameTypeItem=NULL;
	m_GameTypeItemMap.GetNextAssoc(Position,wKey,pGameTypeItem);

	return pGameTypeItem;
}

//ö������
CGameKindItem * CServerListManager::EmunGameKindItem(POSITION & Position)
{
	//��������
	if (Position==NULL) Position=m_GameKindItemMap.GetStartPosition();
	if (Position==NULL) return NULL;

	//���ҷ���
	WORD wKey=0;
	CGameKindItem * pGameKindItem=NULL;
	m_GameKindItemMap.GetNextAssoc(Position,wKey,pGameKindItem);

	return pGameKindItem;
}

//ö�ٽڵ�
CGameNodeItem * CServerListManager::EmunGameNodeItem(POSITION & Position)
{
	//��������
	if (Position==NULL) Position=m_GameNodeItemMap.GetStartPosition();
	if (Position==NULL) return NULL;

	//���ҽڵ�
	WORD wKey=0;
	CGameNodeItem * pGameNodeItem=NULL;
	m_GameNodeItemMap.GetNextAssoc(Position,wKey,pGameNodeItem);

	return pGameNodeItem;
}

//ö�ٶ���
CGameCustomItem * CServerListManager::EmunGameCustomItem(POSITION & Position)
{
	//��������
	if (Position==NULL) Position=m_GameCustomItemMap.GetStartPosition();
	if (Position==NULL) return NULL;

	//���ҷ���
	WORD wKey=0;
	CGameCustomItem * pGameCustomItem=NULL;
	m_GameCustomItemMap.GetNextAssoc(Position,wKey,pGameCustomItem);

	return pGameCustomItem;
}

//ö�ٷ���
CGameServerItem * CServerListManager::EmunGameServerItem(POSITION & Position)
{
	//��������
	if (Position==NULL) Position=m_GameServerItemMap.GetStartPosition();
	if (Position==NULL) return NULL;

	//���ҷ���
	WORD wKey=0;
	CGameServerItem * pGameServerItem=NULL;
	m_GameServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);

	return pGameServerItem;
}

//��������
CGameTypeItem * CServerListManager::SearchGameType(WORD wTypeID)
{
	CGameTypeItem * pGameTypeItem=NULL;
	m_GameTypeItemMap.Lookup(wTypeID,pGameTypeItem);
	return pGameTypeItem;
}

//��������
CGameKindItem * CServerListManager::SearchGameKind(WORD wKindID)
{
	CGameKindItem * pGameKindItem=NULL;
	m_GameKindItemMap.Lookup(wKindID,pGameKindItem);
	return pGameKindItem;
}

//���ҽڵ�
CGameNodeItem * CServerListManager::SearchGameNode(WORD wNodeID)
{
	CGameNodeItem * pGameNodeItem=NULL;
	m_GameNodeItemMap.Lookup(wNodeID,pGameNodeItem);
	return pGameNodeItem;
}

//���ҷ���
CGameServerItem * CServerListManager::SearchGameServer(WORD wServerID)
{
	CGameServerItem * pGameServerItem=NULL;
	m_GameServerItemMap.Lookup(wServerID,pGameServerItem);
	return pGameServerItem;
}

//���Ҷ���
CGameCustomItem * CServerListManager::SearchGameCustom(WORD wCustomID)
{
	CGameCustomItem * pGameCustomItem=NULL;
	m_GameCustomItemMap.Lookup(wCustomID,pGameCustomItem);
	return pGameCustomItem;
}

//�����б�
VOID CServerListManager::UpdateGameKind(WORD wKindID)
{
	//��������
	CGameKindItem * pGameKindItem=NULL;
	m_GameKindItemMap.Lookup(wKindID,pGameKindItem);

	//��ȡ�汾
	if (pGameKindItem!=NULL)
	{
		LPCTSTR pszProcessName=CW2CT(pGameKindItem->m_GameKind.szProcessName);
		CWHService::GetModuleVersion(pszProcessName,pGameKindItem->m_dwProcessVersion);
	}

	//��������
	m_pIServerListViewSink->OnGameItemUpdate(pGameKindItem);

	return;
}

//////////////////////////////////////////////////////////////////////////
