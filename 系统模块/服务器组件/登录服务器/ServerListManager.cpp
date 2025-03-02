#include "StdAfx.h"
#include "ServerListManager.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameTypeItem::CGameTypeItem()
{
	//���ñ���
	ZeroMemory(&m_GameType,sizeof(m_GameType));

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameKindItem::CGameKindItem()
{
	//���ñ���
	m_dwOnLineCount=0;	
	ZeroMemory(&m_GameKind,sizeof(m_GameKind));

	//������Ϣ 
	m_lJackpotScore = 0;
	m_ulLastTickCount = 0;

	return;
}

//���ö���
VOID CGameKindItem::ResetKindItem()
{
	//���ñ���
	m_dwOnLineCount=0;	
	ZeroMemory(&m_GameKind, sizeof(m_GameKind));

	//������Ϣ 
	m_lJackpotScore = 0;
	m_ulLastTickCount = 0;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameNodeItem::CGameNodeItem()
{
	//���ñ���
	ZeroMemory(&m_GameNode,sizeof(m_GameNode));

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameServerItem::CGameServerItem()
{
	//���ñ���
	ZeroMemory(&m_GameServer,sizeof(m_GameServer));

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameCustomItem::CGameCustomItem()
{
	//���ñ���
	ZeroMemory(&m_GameCustom,sizeof(m_GameCustom));

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CGameFilterItem::CGameFilterItem()
{
	//���ñ���
	ZeroMemory(&m_GameFilter,sizeof(m_GameFilter));

	return;
}

//�����ж�
bool CGameFilterItem::InsertVerdict()
{
	//�����ж�
	if (m_GameFilter.wFilterMask&FM_DELETE)
	{
		return false;
	}

	return true;
}

//�������
WORD CGameFilterItem::FilterSortID(WORD wSortID)
{
	//�������
	if (m_GameFilter.wFilterMask&FM_SORT_ID)
	{
		return m_GameFilter.wSortID;
	}

	return wSortID;
}

//���͹���
WORD CGameFilterItem::FilterKindID(WORD wKindID)
{
	//�������
	if (m_GameFilter.wFilterMask&FM_KIND_ID)
	{
		return m_GameFilter.wKindID;
	}

	return wKindID;
}

//�ڵ����
WORD CGameFilterItem::FilterNodeID(WORD wNodeID)
{
	//�ڵ����
	if (m_GameFilter.wFilterMask&FM_NODE_ID)
	{
		return m_GameFilter.wNodeID;
	}

	return wNodeID;
}

//���ֹ���
LPCTSTR CGameFilterItem::FilterServerName(LPCTSTR pszServerName)
{
	//���ֹ���
	if (m_GameFilter.wFilterMask&FM_SERVER_NAME)
	{
		return m_GameFilter.szServerName;
	}

	return pszServerName;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CServerListManager::CServerListManager()
{
	//��������
	m_TypeItemMap.InitHashTable(PRIME_TYPE);
	m_KindItemMap.InitHashTable(PRIME_KIND);
	m_NodeItemMap.InitHashTable(PRIME_NODE);
	m_ServerItemMap.InitHashTable(PRIME_SERVER);
	m_CustomItemMap.InitHashTable(PRIME_CUSTOM);
	m_FilterItemMap.InitHashTable(PRIME_SERVER);

	return;
}

//��������
CServerListManager::~CServerListManager()
{
	//��������
	WORD wKey=0;

	//ɾ������
	CGameTypeItem * pGameTypeItem=NULL;
	POSITION Position=m_TypeItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_TypeItemMap.GetNextAssoc(Position,wKey,pGameTypeItem);
		SafeDelete(pGameTypeItem);
	}
	for (INT_PTR i=0;i<m_TypeItemBuffer.GetCount();i++)
	{
		pGameTypeItem=m_TypeItemBuffer[i];
		SafeDelete(pGameTypeItem);
	}
	m_TypeItemMap.RemoveAll();
	m_TypeItemBuffer.RemoveAll();

	//ɾ������
	CGameKindItem * pGameKindItem=NULL;
	Position=m_KindItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_KindItemMap.GetNextAssoc(Position,wKey,pGameKindItem);
		SafeDelete(pGameKindItem);
	}
	for (INT_PTR i=0;i<m_KindItemBuffer.GetCount();i++)
	{
		pGameKindItem=m_KindItemBuffer[i];
		SafeDelete(pGameKindItem);
	}
	m_KindItemMap.RemoveAll();
	m_KindItemBuffer.RemoveAll();

	//ɾ���ڵ�
	CGameNodeItem * pGameNodeItem=NULL;
	Position=m_NodeItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_NodeItemMap.GetNextAssoc(Position,wKey,pGameNodeItem);
		SafeDelete(pGameNodeItem);
	}
	for (INT_PTR i=0;i<m_NodeItemBuffer.GetCount();i++)
	{
		pGameNodeItem=m_NodeItemBuffer[i];
		SafeDelete(pGameNodeItem);
	}
	m_NodeItemMap.RemoveAll();
	m_NodeItemBuffer.RemoveAll();

	//ɾ������
	CGameServerItem * pGameServerItem=NULL;
	Position=m_ServerItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_ServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);
		SafeDelete(pGameServerItem);
	}
	for (INT_PTR i=0;i<m_ServerItemBuffer.GetCount();i++)
	{
		pGameServerItem=m_ServerItemBuffer[i];
		SafeDelete(pGameServerItem);
	}
	m_ServerItemMap.RemoveAll();
	m_ServerItemBuffer.RemoveAll();

	//ɾ������
	CGameCustomItem * pGameCustomItem=NULL;
	Position=m_CustomItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_CustomItemMap.GetNextAssoc(Position,wKey,pGameCustomItem);
		SafeDelete(pGameCustomItem);
	}
	for (INT_PTR i=0;i<m_CustomItemBuffer.GetCount();i++)
	{
		pGameCustomItem=m_CustomItemBuffer[i];
		SafeDelete(pGameCustomItem);
	}
	m_CustomItemMap.RemoveAll();
	m_CustomItemBuffer.RemoveAll();

	//ɾ������
	CGameFilterItem * pGameFilterItem=NULL;
	Position=m_FilterItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_FilterItemMap.GetNextAssoc(Position,wKey,pGameFilterItem);
		SafeDelete(pGameFilterItem);
	}
	for (INT_PTR i=0;i<m_FilterItemBuffer.GetCount();i++)
	{
		pGameFilterItem=m_FilterItemBuffer[i];
		SafeDelete(pGameFilterItem);
	}
	m_FilterItemMap.RemoveAll();
	m_FilterItemBuffer.RemoveAll();

	return;
}

//�����б�
VOID CServerListManager::ResetServerList()
{
	//�����б�
	DisuseKernelItem();
	DisuseServerItem();

	//�����б�
	CleanKernelItem();
	CleanServerItem();

	return;
}

//�����ں�
VOID CServerListManager::CleanKernelItem()
{
	//��������
	POSITION PositionType=m_TypeItemMap.GetStartPosition();
	POSITION PositionKind=m_KindItemMap.GetStartPosition();
	POSITION PositionNode=m_NodeItemMap.GetStartPosition();
	POSITION PositionCustom=m_CustomItemMap.GetStartPosition();
	POSITION PositionFilter=m_FilterItemMap.GetStartPosition();

	//��������
	while (PositionType!=NULL)
	{
		//��������
		WORD wTypeID=0;
		CGameTypeItem * pGameTypeItem=NULL;
		m_TypeItemMap.GetNextAssoc(PositionType,wTypeID,pGameTypeItem);

		//ɾ���ж�
		if (pGameTypeItem->m_bDisuse==true)
		{
			m_TypeItemMap.RemoveKey(wTypeID);
			m_TypeItemBuffer.Add(pGameTypeItem);
		}
	}

	//��������
	while (PositionKind!=NULL)
	{
		//��������
		WORD wKindID=0;
		CGameKindItem * pGameKindItem=NULL;
		m_KindItemMap.GetNextAssoc(PositionKind,wKindID,pGameKindItem);

		//ɾ���ж�
		if (pGameKindItem->m_bDisuse==true)
		{
			m_KindItemMap.RemoveKey(wKindID);
			m_KindItemBuffer.Add(pGameKindItem);
		}
	}

	//����ڵ�
	while (PositionNode!=NULL)
	{
		//��������
		WORD wNodeID=0;
		CGameNodeItem * pGameNodeItem=NULL;
		m_NodeItemMap.GetNextAssoc(PositionNode,wNodeID,pGameNodeItem);

		//ɾ���ж�
		if (pGameNodeItem->m_bDisuse==true)
		{
			m_NodeItemMap.RemoveKey(wNodeID);
			m_NodeItemBuffer.Add(pGameNodeItem);
		}
	}

	//������
	while (PositionCustom!=NULL)
	{
		//��������
		WORD wCustomID=0;
		CGameCustomItem * pGameCustomItem=NULL;
		m_CustomItemMap.GetNextAssoc(PositionCustom,wCustomID,pGameCustomItem);

		//ɾ���ж�
		if (pGameCustomItem->m_bDisuse==true)
		{
			m_CustomItemMap.RemoveKey(wCustomID);
			m_CustomItemBuffer.Add(pGameCustomItem);
		}
	}

	//�������
	while (PositionFilter!=NULL)
	{
		//��������
		WORD wFilterID=0;
		CGameFilterItem * pGameFilterItem=NULL;
		m_FilterItemMap.GetNextAssoc(PositionFilter,wFilterID,pGameFilterItem);

		//ɾ���ж�
		if (pGameFilterItem->m_bDisuse==true)
		{
			m_FilterItemMap.RemoveKey(wFilterID);
			m_FilterItemBuffer.Add(pGameFilterItem);
		}
	}

	return;
}

//������
VOID CServerListManager::CleanServerItem()
{
	//��������
	POSITION PositionServer=m_ServerItemMap.GetStartPosition();

	//ɾ������
	while (PositionServer!=NULL)
	{
		//��������
		WORD wServerID=0;
		CGameServerItem * pGameServerItem=NULL;
		m_ServerItemMap.GetNextAssoc(PositionServer,wServerID,pGameServerItem);

		//ɾ���ж�
		if (pGameServerItem->m_bDisuse==true)
		{
			m_ServerItemMap.RemoveKey(wServerID);
			m_ServerItemBuffer.Add(pGameServerItem);
		}
	}

	return;
}

//�����ں�
VOID CServerListManager::DisuseKernelItem()
{
	//��������
	WORD wKey=0;
	DWORD dwKey=0;

	//��������
	CGameTypeItem * pGameTypeItem=NULL;
	POSITION Position=m_TypeItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_TypeItemMap.GetNextAssoc(Position,wKey,pGameTypeItem);
		pGameTypeItem->m_bDisuse=true;
	}

	//��������
	CGameKindItem * pGameKindItem=NULL;
	Position=m_KindItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_KindItemMap.GetNextAssoc(Position,wKey,pGameKindItem);
		pGameKindItem->m_bDisuse=true;
	}

	//�����ڵ�
	CGameNodeItem * pGameNodeItem=NULL;
	Position=m_NodeItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_NodeItemMap.GetNextAssoc(Position,wKey,pGameNodeItem);
		pGameNodeItem->m_bDisuse=true;
	}

	//��������
	CGameCustomItem * pGameCustomItem=NULL;
	Position=m_CustomItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_CustomItemMap.GetNextAssoc(Position,wKey,pGameCustomItem);
		pGameCustomItem->m_bDisuse=true;
	}

	//��������
	CGameFilterItem * pGameFilterItem=NULL;
	Position=m_FilterItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_FilterItemMap.GetNextAssoc(Position,wKey,pGameFilterItem);
		pGameFilterItem->m_bDisuse=true;
	}

	return;
}

//��������
VOID CServerListManager::DisuseServerItem()
{
	//��������
	WORD wKey=0;
	CGameServerItem * pGameServerItem=NULL;

	//��������
	POSITION Position=m_ServerItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_ServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);
		pGameServerItem->m_bDisuse=true;
	}

	return;
}

//ͳ������
DWORD CServerListManager::CollectOnlineInfo()
{
	//��������
	DWORD dwOnLineCount=0L;
	POSITION PositionKind=m_KindItemMap.GetStartPosition();
	POSITION PositionServer=m_ServerItemMap.GetStartPosition();
	
	//��������
	while (PositionKind!=NULL)
	{
		//��������
		WORD wKindID=0;
		CGameKindItem * pGameKindItem=NULL;

		//��������
		m_KindItemMap.GetNextAssoc(PositionKind,wKindID,pGameKindItem);

		//���ñ���
		if (pGameKindItem!=NULL) pGameKindItem->m_dwOnLineCount=0L;
	}

	//ͳ������
	while (PositionServer!=NULL)
	{
		//��������
		WORD wServerID=0;
		CGameServerItem * pGameServerItem=NULL;

		//��������
		m_ServerItemMap.GetNextAssoc(PositionServer,wServerID,pGameServerItem);

		//��������
		if (pGameServerItem!=NULL)
		{
			//��ѯ����
			WORD wKindID=pGameServerItem->m_GameServer.wKindID;
			CGameKindItem * pGameKindItem=SearchGameKind(wKindID);

			//��������
			if (pGameKindItem!=NULL)
			{
				dwOnLineCount+=pGameServerItem->m_GameServer.dwOnLineCount;
				pGameKindItem->m_dwOnLineCount+=pGameServerItem->m_GameServer.dwOnLineCount;
			}
		}
	}

	return dwOnLineCount;
}

//��������
DWORD CServerListManager::CollectOnlineInfo(WORD wKindID)
{
	//��������
	DWORD dwOnLineCount=0L;
	POSITION PositionServer=m_ServerItemMap.GetStartPosition();

	//ö�ٷ���
	while (PositionServer!=NULL)
	{
		//��������
		WORD wServerID=0;
		CGameServerItem * pGameServerItem=NULL;
		m_ServerItemMap.GetNextAssoc(PositionServer,wServerID,pGameServerItem);

		//ͳ������
		if (pGameServerItem->m_GameServer.wKindID==wKindID)
		{
			dwOnLineCount+=pGameServerItem->m_GameServer.dwOnLineCount;
		}
	}

	return dwOnLineCount;
}

//��������
bool CServerListManager::InsertGameType(tagGameType * pGameType)
{
	//Ч�����
	ASSERT(pGameType!=NULL);
	if (pGameType==NULL) return false;

	//�����ִ�
	CGameTypeItem * pGameTypeItem=NULL;
	if (m_TypeItemMap.Lookup(pGameType->wTypeID,pGameTypeItem)==FALSE)
	{
		//��������
		try
		{
			INT_PTR nStroeCount=m_TypeItemBuffer.GetCount();
			if (nStroeCount>0)
			{
				pGameTypeItem=m_TypeItemBuffer[nStroeCount-1];
				m_TypeItemBuffer.RemoveAt(nStroeCount-1);
			}
			else
			{
				pGameTypeItem=new CGameTypeItem;
				if (pGameTypeItem==NULL) return false;
			}
		}
		catch (...) { return false; }

		//���ñ���
		ZeroMemory(pGameTypeItem,sizeof(CGameTypeItem));
	}

	//��������
	pGameTypeItem->m_bDisuse=false;
	CopyMemory(&pGameTypeItem->m_GameType,pGameType,sizeof(tagGameType));

	//��������
	m_TypeItemMap[pGameType->wTypeID]=pGameTypeItem;

	return true;
}

//��������
bool CServerListManager::InsertGameKind(tagGameKindEx * pGameKind)
{
	//Ч�����
	ASSERT(pGameKind!=NULL);
	if (pGameKind==NULL) return false;

	//�����ִ�
	CGameKindItem * pGameKindItem=NULL;
	if (m_KindItemMap.Lookup(pGameKind->wKindID,pGameKindItem)==FALSE)
	{
		//��������
		try
		{
			INT_PTR nStroeCount=m_KindItemBuffer.GetCount();
			if (nStroeCount>0)
			{
				pGameKindItem=m_KindItemBuffer[nStroeCount-1];
				m_KindItemBuffer.RemoveAt(nStroeCount-1);
			}
			else
			{
				pGameKindItem=new CGameKindItem;
				if (pGameKindItem==NULL) return false;
			}
		}
		catch (...) { return false; }

		//���ö���
		pGameKindItem->ResetKindItem();
	}

	//��������
	pGameKindItem->m_bDisuse=false;
	CopyMemory(&pGameKindItem->m_GameKind,pGameKind,sizeof(pGameKindItem->m_GameKind));

	//��������
	m_KindItemMap[pGameKind->wKindID]=pGameKindItem;

	return true;
}

//����ڵ�
bool CServerListManager::InsertGameNode(tagGameNode * pGameNode)
{
	//Ч�����
	ASSERT(pGameNode!=NULL);
	if (pGameNode==NULL) return false;

	//�����ִ�
	CGameNodeItem * pGameNodeItem=NULL;
	if (m_NodeItemMap.Lookup(pGameNode->wNodeID,pGameNodeItem)==FALSE) 
	{
		//��������
		try
		{
			INT_PTR nStroeCount=m_NodeItemBuffer.GetCount();
			if (nStroeCount>0)
			{
				pGameNodeItem=m_NodeItemBuffer[nStroeCount-1];
				m_NodeItemBuffer.RemoveAt(nStroeCount-1);
			}
			else
			{
				pGameNodeItem=new CGameNodeItem;
				if (pGameNodeItem==NULL) return false;
			}
		}
		catch (...) { return false; }

		//���ñ���
		ZeroMemory(pGameNodeItem,sizeof(CGameNodeItem));
	}

	//��������
	pGameNodeItem->m_bDisuse=false;
	CopyMemory(&pGameNodeItem->m_GameNode,pGameNode,sizeof(tagGameNode));

	//��������
	m_NodeItemMap[pGameNode->wNodeID]=pGameNodeItem;

	return true;
}

//���붨��
bool CServerListManager::InsertGameCustom(tagGameCustom * pGameCustom)
{
	//Ч�����
	ASSERT(pGameCustom!=NULL);
	if (pGameCustom==NULL) return false;

	//��ȡ����
	CGameCustomItem * pGameCustomItem=NULL;
	if (m_CustomItemMap.Lookup(pGameCustom->wPageID,pGameCustomItem)==FALSE) 
	{
		//��������
		try
		{
			INT_PTR nStroeCount=m_CustomItemBuffer.GetCount();
			if (nStroeCount>0)
			{
				pGameCustomItem=m_CustomItemBuffer[nStroeCount-1];
				m_CustomItemBuffer.RemoveAt(nStroeCount-1);
			}
			else
			{
				pGameCustomItem=new CGameCustomItem;
				if (pGameCustomItem==NULL) return false;
			}
		}
		catch (...) { return false; }

		//���ñ���
		ZeroMemory(pGameCustomItem,sizeof(CGameCustomItem));
	}

	//��������
	pGameCustomItem->m_bDisuse=false;
	CopyMemory(&pGameCustomItem->m_GameCustom,pGameCustom,sizeof(tagGameCustom));

	//��������
	m_CustomItemMap[pGameCustom->wPageID]=pGameCustomItem;

	return true;
}

//���뷿��
bool CServerListManager::InsertGameServer(tagGameServer * pGameServer)
{
	//Ч�����
	ASSERT(pGameServer!=NULL);
	if (pGameServer==NULL) return false;

	//���ҹ���
	WORD wServerID=pGameServer->wServerID;
	CGameFilterItem * pGameFilterItem=SearchGameFilter(wServerID);

	//��������
	if (pGameFilterItem!=NULL)
	{
		//�������
		if (pGameFilterItem->InsertVerdict()==false) return true;
	}

	//���͹���
	if (SearchGameKind(pGameServer->wKindID)==NULL) return true;

	//��ȡ����
	CGameServerItem * pGameServerItem=NULL;
	if (m_ServerItemMap.Lookup(pGameServer->wServerID,pGameServerItem)==FALSE)
	{
		//��������
		try
		{
			INT_PTR nStroeCount=m_ServerItemBuffer.GetCount();
			if (nStroeCount>0)
			{
				pGameServerItem=m_ServerItemBuffer[nStroeCount-1];
				m_ServerItemBuffer.RemoveAt(nStroeCount-1);
			}
			else
			{
				pGameServerItem=new CGameServerItem;
				if (pGameServerItem==NULL) return false;
			}
		}
		catch (...) { return false; }

		//���ñ���
		ZeroMemory(pGameServerItem,sizeof(CGameServerItem));
	}

	//��������
	pGameServerItem->m_bDisuse=false;
	CopyMemory(&pGameServerItem->m_GameServer,pGameServer,sizeof(tagGameServer));

	//��������
	if (pGameFilterItem!=NULL)
	{
		//���ñ���
		pGameServerItem->m_GameServer.wSortID=pGameFilterItem->FilterSortID(pGameServer->wSortID);
		pGameServerItem->m_GameServer.wKindID=pGameFilterItem->FilterKindID(pGameServer->wKindID);
		pGameServerItem->m_GameServer.wNodeID=pGameFilterItem->FilterNodeID(pGameServer->wNodeID);

		//��������
		LPCTSTR pszServerName=pGameFilterItem->FilterServerName(pGameServer->szServerName);
		StringCchCopy(pGameServerItem->m_GameServer.szServerName,CountArray(pGameServerItem->m_GameServer.szServerName), pszServerName);
	}

	//��������
	m_ServerItemMap[pGameServer->wServerID]=pGameServerItem;

	return true;
}

//�������
bool CServerListManager::InsertGameFilter(tagGameFilter * pGameFilter)
{
	//Ч�����
	ASSERT(pGameFilter!=NULL);
	if (pGameFilter==NULL) return false;

	//��ȡ����
	CGameFilterItem * pGameFilterItem=NULL;
	if (m_FilterItemMap.Lookup(pGameFilter->wServerID,pGameFilterItem)==FALSE)
	{
		//��������
		try
		{
			INT_PTR nStroeCount=m_FilterItemBuffer.GetCount();
			if (nStroeCount>0)
			{
				pGameFilterItem=m_FilterItemBuffer[nStroeCount-1];
				m_FilterItemBuffer.RemoveAt(nStroeCount-1);
			}
			else
			{
				pGameFilterItem=new CGameFilterItem;
				if (pGameFilterItem==NULL) return false;
			}
		}
		catch (...) { return false; }

		//���ñ���
		ZeroMemory(pGameFilterItem,sizeof(CGameFilterItem));
	}

	//��������
	pGameFilterItem->m_bDisuse=false;
	CopyMemory(&pGameFilterItem->m_GameFilter,pGameFilter,sizeof(tagGameFilter));

	//��������
	m_FilterItemMap[pGameFilter->wServerID]=pGameFilterItem;

	//���˴���
	if (pGameFilterItem->InsertVerdict()==true)
	{
		//���ҷ���
		WORD wServerID=pGameFilterItem->m_GameFilter.wServerID;
		CGameServerItem * pGameServerItem=SearchGameServer(wServerID);

		//���÷���
		if (pGameServerItem!=NULL)
		{
			//��������
			tagGameServer * pGameServer=&pGameServerItem->m_GameServer;

			//���ñ���
			pGameServer->wSortID=pGameFilterItem->FilterSortID(pGameServer->wSortID);
			pGameServer->wKindID=pGameFilterItem->FilterKindID(pGameServer->wKindID);
			pGameServer->wNodeID=pGameFilterItem->FilterNodeID(pGameServer->wNodeID);

			//��������
			LPCTSTR pszServerName=pGameFilterItem->FilterServerName(pGameServer->szServerName);
			StringCchCopy(pGameServer->szServerName,CountArray(pGameServer->szServerName), pszServerName);
		}
	}
	else
	{
		//ɾ������
		DeleteGameServer(pGameFilter->wServerID);
	}

	return true;
}

//ɾ������
bool CServerListManager::DeleteGameType(WORD wTypeID)
{
	//��������
	CGameTypeItem * pGameTypeItem=NULL;
	if (m_TypeItemMap.Lookup(wTypeID,pGameTypeItem)==FALSE) return false;

	//ɾ������
	m_TypeItemMap.RemoveKey(wTypeID);
	m_TypeItemBuffer.Add(pGameTypeItem);

	return true;
}

//ɾ������
bool CServerListManager::DeleteGameKind(WORD wKindID)
{
	//��������
	CGameKindItem * pGameKindItem=NULL;
	if (m_KindItemMap.Lookup(wKindID,pGameKindItem)==FALSE) return false;

	//ɾ������
	m_KindItemMap.RemoveKey(wKindID);
	m_KindItemBuffer.Add(pGameKindItem);

	//���ñ���
	pGameKindItem->ResetKindItem();

	return true;
}

//ɾ���ڵ�
bool CServerListManager::DeleteGameNode(WORD wNodeID)
{
	//���ҽڵ�
	CGameNodeItem * pGameNodeItem=NULL;
	if (m_NodeItemMap.Lookup(wNodeID,pGameNodeItem)==FALSE) return false;

	//ɾ������
	m_NodeItemMap.RemoveKey(wNodeID);
	m_NodeItemBuffer.Add(pGameNodeItem);

	//���ñ���
	ZeroMemory(pGameNodeItem,sizeof(CGameNodeItem));

	return false;
}

//ɾ������
bool CServerListManager::DeleteGameServer(WORD wServerID)
{
	//���ҷ���
	CGameServerItem * pGameServerItem=NULL;
	if (m_ServerItemMap.Lookup(wServerID,pGameServerItem)==FALSE) return false;

	//ɾ������
	m_ServerItemMap.RemoveKey(wServerID);
	m_ServerItemBuffer.Add(pGameServerItem);

	//���ñ���
	ZeroMemory(pGameServerItem,sizeof(CGameServerItem));

	return false;
}

//ɾ������
bool CServerListManager::DeleteGameCustom(WORD wCustomID)
{
	//��������
	CGameCustomItem * pGameCustomItem=NULL;
	if (m_CustomItemMap.Lookup(wCustomID,pGameCustomItem)==FALSE) return false;

	//ɾ������
	m_CustomItemMap.RemoveKey(wCustomID);
	m_CustomItemBuffer.Add(pGameCustomItem);

	//���ñ���
	ZeroMemory(pGameCustomItem,sizeof(CGameCustomItem));

	return true;
}

//ɾ������
bool CServerListManager::DeleteGameFilter(WORD wServerID)
{
	//��������
	CGameFilterItem * pGameFilterItem=NULL;
	if (m_FilterItemMap.Lookup(wServerID,pGameFilterItem)==FALSE) return false;

	//ɾ������
	m_FilterItemMap.RemoveKey(wServerID);
	m_FilterItemBuffer.Add(pGameFilterItem);

	//���ñ���
	ZeroMemory(pGameFilterItem,sizeof(pGameFilterItem));

	return true;
}

//ö������
CGameTypeItem * CServerListManager::EmunGameTypeItem(POSITION & Position)
{
	//��������
	WORD wKey=0;
	CGameTypeItem * pGameTypeItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_TypeItemMap.GetStartPosition();
	if (Position!=NULL) m_TypeItemMap.GetNextAssoc(Position,wKey,pGameTypeItem);

	return pGameTypeItem;
}

//ö������
CGameKindItem * CServerListManager::EmunGameKindItem(POSITION & Position)
{
	//��������
	WORD wKey=0;
	CGameKindItem * pGameKindItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_KindItemMap.GetStartPosition();
	if (Position!=NULL) m_KindItemMap.GetNextAssoc(Position,wKey,pGameKindItem);

	return pGameKindItem;
}

//ö�ٽڵ�
CGameNodeItem * CServerListManager::EmunGameNodeItem(POSITION & Position)
{
	//��������
	WORD wKey=0;
	CGameNodeItem * pGameNodeItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_NodeItemMap.GetStartPosition();
	if (Position!=NULL) m_NodeItemMap.GetNextAssoc(Position,wKey,pGameNodeItem);

	return pGameNodeItem;
}

//ö�ٷ���
CGameServerItem * CServerListManager::EmunGameServerItem(POSITION & Position)
{
	//��������
	WORD wKey=0;
	CGameServerItem * pGameServerItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_ServerItemMap.GetStartPosition();
	if (Position!=NULL) m_ServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);

	return pGameServerItem;
}

//ö�ٶ���
CGameCustomItem * CServerListManager::EmunGameCustomItem(POSITION & Position)
{
	//��������
	WORD wKey=0;
	CGameCustomItem * pGameCustomItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_CustomItemMap.GetStartPosition();
	if (Position!=NULL) m_CustomItemMap.GetNextAssoc(Position,wKey,pGameCustomItem);

	return pGameCustomItem;
}

//ö�ٹ���
CGameFilterItem * CServerListManager::EmunGameFilterItem(POSITION & Position)
{
	//��������
	WORD wKey=0;
	CGameFilterItem * pGameFilterItem=NULL;

	//��ȡ����
	if (Position==NULL) Position=m_FilterItemMap.GetStartPosition();
	if (Position!=NULL) m_FilterItemMap.GetNextAssoc(Position,wKey,pGameFilterItem);

	return pGameFilterItem;
}

//��������
CGameTypeItem * CServerListManager::SearchGameType(WORD wTypeID)
{
	CGameTypeItem * pGameTypeItem=NULL;
	m_TypeItemMap.Lookup(wTypeID,pGameTypeItem);
	return pGameTypeItem;
}

//��������
CGameKindItem * CServerListManager::SearchGameKind(WORD wKindID)
{
	CGameKindItem * pGameKindItem=NULL;
	m_KindItemMap.Lookup(wKindID,pGameKindItem);
	return pGameKindItem;
}

//���ҽڵ�
CGameNodeItem * CServerListManager::SearchGameNode(WORD wNodeID)
{
	CGameNodeItem * pGameNodeItem=NULL;
	m_NodeItemMap.Lookup(wNodeID,pGameNodeItem);
	return pGameNodeItem;
}

//���ҷ���
CGameServerItem * CServerListManager::SearchGameServer(WORD wServerID)
{
	CGameServerItem * pGameServerItem=NULL;
	m_ServerItemMap.Lookup(wServerID,pGameServerItem);
	return pGameServerItem;
}

//���Ҷ���
CGameCustomItem * CServerListManager::SearchGameCustom(WORD wCustomID)
{
	CGameCustomItem * pGameCustomItem=NULL;
	m_CustomItemMap.Lookup(wCustomID,pGameCustomItem);
	return pGameCustomItem;
}

//���ҹ���
CGameFilterItem * CServerListManager::SearchGameFilter(WORD wServerID)
{
	CGameFilterItem * pGameFilterItem=NULL;
	m_FilterItemMap.Lookup(wServerID,pGameFilterItem);
	return pGameFilterItem;
}

//////////////////////////////////////////////////////////////////////////
