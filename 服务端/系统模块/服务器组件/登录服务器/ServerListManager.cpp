#include "StdAfx.h"
#include "ServerListManager.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameTypeItem::CGameTypeItem()
{
	//设置变量
	ZeroMemory(&m_GameType,sizeof(m_GameType));

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameKindItem::CGameKindItem()
{
	//设置变量
	m_dwOnLineCount=0;	
	ZeroMemory(&m_GameKind,sizeof(m_GameKind));

	//奖池信息 
	m_lJackpotScore = 0;
	m_ulLastTickCount = 0;

	return;
}

//重置对象
VOID CGameKindItem::ResetKindItem()
{
	//设置变量
	m_dwOnLineCount=0;	
	ZeroMemory(&m_GameKind, sizeof(m_GameKind));

	//奖池信息 
	m_lJackpotScore = 0;
	m_ulLastTickCount = 0;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameNodeItem::CGameNodeItem()
{
	//设置变量
	ZeroMemory(&m_GameNode,sizeof(m_GameNode));

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameServerItem::CGameServerItem()
{
	//设置变量
	ZeroMemory(&m_GameServer,sizeof(m_GameServer));

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameCustomItem::CGameCustomItem()
{
	//设置变量
	ZeroMemory(&m_GameCustom,sizeof(m_GameCustom));

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameFilterItem::CGameFilterItem()
{
	//设置变量
	ZeroMemory(&m_GameFilter,sizeof(m_GameFilter));

	return;
}

//插入判断
bool CGameFilterItem::InsertVerdict()
{
	//插入判断
	if (m_GameFilter.wFilterMask&FM_DELETE)
	{
		return false;
	}

	return true;
}

//排序过滤
WORD CGameFilterItem::FilterSortID(WORD wSortID)
{
	//排序过滤
	if (m_GameFilter.wFilterMask&FM_SORT_ID)
	{
		return m_GameFilter.wSortID;
	}

	return wSortID;
}

//类型过滤
WORD CGameFilterItem::FilterKindID(WORD wKindID)
{
	//排序过滤
	if (m_GameFilter.wFilterMask&FM_KIND_ID)
	{
		return m_GameFilter.wKindID;
	}

	return wKindID;
}

//节点过滤
WORD CGameFilterItem::FilterNodeID(WORD wNodeID)
{
	//节点过滤
	if (m_GameFilter.wFilterMask&FM_NODE_ID)
	{
		return m_GameFilter.wNodeID;
	}

	return wNodeID;
}

//名字过滤
LPCTSTR CGameFilterItem::FilterServerName(LPCTSTR pszServerName)
{
	//名字过滤
	if (m_GameFilter.wFilterMask&FM_SERVER_NAME)
	{
		return m_GameFilter.szServerName;
	}

	return pszServerName;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerListManager::CServerListManager()
{
	//设置质数
	m_TypeItemMap.InitHashTable(PRIME_TYPE);
	m_KindItemMap.InitHashTable(PRIME_KIND);
	m_NodeItemMap.InitHashTable(PRIME_NODE);
	m_ServerItemMap.InitHashTable(PRIME_SERVER);
	m_CustomItemMap.InitHashTable(PRIME_CUSTOM);
	m_FilterItemMap.InitHashTable(PRIME_SERVER);

	return;
}

//析构函数
CServerListManager::~CServerListManager()
{
	//变量定义
	WORD wKey=0;

	//删除种类
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

	//删除类型
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

	//删除节点
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

	//删除房间
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

	//删除定制
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

	//删除过滤
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

//重置列表
VOID CServerListManager::ResetServerList()
{
	//废弃列表
	DisuseKernelItem();
	DisuseServerItem();

	//清理列表
	CleanKernelItem();
	CleanServerItem();

	return;
}

//清理内核
VOID CServerListManager::CleanKernelItem()
{
	//变量定义
	POSITION PositionType=m_TypeItemMap.GetStartPosition();
	POSITION PositionKind=m_KindItemMap.GetStartPosition();
	POSITION PositionNode=m_NodeItemMap.GetStartPosition();
	POSITION PositionCustom=m_CustomItemMap.GetStartPosition();
	POSITION PositionFilter=m_FilterItemMap.GetStartPosition();

	//清理种类
	while (PositionType!=NULL)
	{
		//索引房间
		WORD wTypeID=0;
		CGameTypeItem * pGameTypeItem=NULL;
		m_TypeItemMap.GetNextAssoc(PositionType,wTypeID,pGameTypeItem);

		//删除判断
		if (pGameTypeItem->m_bDisuse==true)
		{
			m_TypeItemMap.RemoveKey(wTypeID);
			m_TypeItemBuffer.Add(pGameTypeItem);
		}
	}

	//清理类型
	while (PositionKind!=NULL)
	{
		//索引房间
		WORD wKindID=0;
		CGameKindItem * pGameKindItem=NULL;
		m_KindItemMap.GetNextAssoc(PositionKind,wKindID,pGameKindItem);

		//删除判断
		if (pGameKindItem->m_bDisuse==true)
		{
			m_KindItemMap.RemoveKey(wKindID);
			m_KindItemBuffer.Add(pGameKindItem);
		}
	}

	//清理节点
	while (PositionNode!=NULL)
	{
		//索引房间
		WORD wNodeID=0;
		CGameNodeItem * pGameNodeItem=NULL;
		m_NodeItemMap.GetNextAssoc(PositionNode,wNodeID,pGameNodeItem);

		//删除判断
		if (pGameNodeItem->m_bDisuse==true)
		{
			m_NodeItemMap.RemoveKey(wNodeID);
			m_NodeItemBuffer.Add(pGameNodeItem);
		}
	}

	//清理定制
	while (PositionCustom!=NULL)
	{
		//索引房间
		WORD wCustomID=0;
		CGameCustomItem * pGameCustomItem=NULL;
		m_CustomItemMap.GetNextAssoc(PositionCustom,wCustomID,pGameCustomItem);

		//删除判断
		if (pGameCustomItem->m_bDisuse==true)
		{
			m_CustomItemMap.RemoveKey(wCustomID);
			m_CustomItemBuffer.Add(pGameCustomItem);
		}
	}

	//清理过滤
	while (PositionFilter!=NULL)
	{
		//索引房间
		WORD wFilterID=0;
		CGameFilterItem * pGameFilterItem=NULL;
		m_FilterItemMap.GetNextAssoc(PositionFilter,wFilterID,pGameFilterItem);

		//删除判断
		if (pGameFilterItem->m_bDisuse==true)
		{
			m_FilterItemMap.RemoveKey(wFilterID);
			m_FilterItemBuffer.Add(pGameFilterItem);
		}
	}

	return;
}

//清理房间
VOID CServerListManager::CleanServerItem()
{
	//变量定义
	POSITION PositionServer=m_ServerItemMap.GetStartPosition();

	//删除房间
	while (PositionServer!=NULL)
	{
		//索引房间
		WORD wServerID=0;
		CGameServerItem * pGameServerItem=NULL;
		m_ServerItemMap.GetNextAssoc(PositionServer,wServerID,pGameServerItem);

		//删除判断
		if (pGameServerItem->m_bDisuse==true)
		{
			m_ServerItemMap.RemoveKey(wServerID);
			m_ServerItemBuffer.Add(pGameServerItem);
		}
	}

	return;
}

//废弃内核
VOID CServerListManager::DisuseKernelItem()
{
	//变量定义
	WORD wKey=0;
	DWORD dwKey=0;

	//废弃种类
	CGameTypeItem * pGameTypeItem=NULL;
	POSITION Position=m_TypeItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_TypeItemMap.GetNextAssoc(Position,wKey,pGameTypeItem);
		pGameTypeItem->m_bDisuse=true;
	}

	//废弃类型
	CGameKindItem * pGameKindItem=NULL;
	Position=m_KindItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_KindItemMap.GetNextAssoc(Position,wKey,pGameKindItem);
		pGameKindItem->m_bDisuse=true;
	}

	//废弃节点
	CGameNodeItem * pGameNodeItem=NULL;
	Position=m_NodeItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_NodeItemMap.GetNextAssoc(Position,wKey,pGameNodeItem);
		pGameNodeItem->m_bDisuse=true;
	}

	//废弃定制
	CGameCustomItem * pGameCustomItem=NULL;
	Position=m_CustomItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_CustomItemMap.GetNextAssoc(Position,wKey,pGameCustomItem);
		pGameCustomItem->m_bDisuse=true;
	}

	//废弃过滤
	CGameFilterItem * pGameFilterItem=NULL;
	Position=m_FilterItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_FilterItemMap.GetNextAssoc(Position,wKey,pGameFilterItem);
		pGameFilterItem->m_bDisuse=true;
	}

	return;
}

//废弃房间
VOID CServerListManager::DisuseServerItem()
{
	//变量定义
	WORD wKey=0;
	CGameServerItem * pGameServerItem=NULL;

	//废弃房间
	POSITION Position=m_ServerItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_ServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);
		pGameServerItem->m_bDisuse=true;
	}

	return;
}

//统计人数
DWORD CServerListManager::CollectOnlineInfo()
{
	//变量定义
	DWORD dwOnLineCount=0L;
	POSITION PositionKind=m_KindItemMap.GetStartPosition();
	POSITION PositionServer=m_ServerItemMap.GetStartPosition();
	
	//人数设零
	while (PositionKind!=NULL)
	{
		//变量定义
		WORD wKindID=0;
		CGameKindItem * pGameKindItem=NULL;

		//设置索引
		m_KindItemMap.GetNextAssoc(PositionKind,wKindID,pGameKindItem);

		//设置变量
		if (pGameKindItem!=NULL) pGameKindItem->m_dwOnLineCount=0L;
	}

	//统计人数
	while (PositionServer!=NULL)
	{
		//变量定义
		WORD wServerID=0;
		CGameServerItem * pGameServerItem=NULL;

		//设置索引
		m_ServerItemMap.GetNextAssoc(PositionServer,wServerID,pGameServerItem);

		//设置人数
		if (pGameServerItem!=NULL)
		{
			//查询房间
			WORD wKindID=pGameServerItem->m_GameServer.wKindID;
			CGameKindItem * pGameKindItem=SearchGameKind(wKindID);

			//设置人数
			if (pGameKindItem!=NULL)
			{
				dwOnLineCount+=pGameServerItem->m_GameServer.dwOnLineCount;
				pGameKindItem->m_dwOnLineCount+=pGameServerItem->m_GameServer.dwOnLineCount;
			}
		}
	}

	return dwOnLineCount;
}

//类型在线
DWORD CServerListManager::CollectOnlineInfo(WORD wKindID)
{
	//变量定义
	DWORD dwOnLineCount=0L;
	POSITION PositionServer=m_ServerItemMap.GetStartPosition();

	//枚举房间
	while (PositionServer!=NULL)
	{
		//索引房间
		WORD wServerID=0;
		CGameServerItem * pGameServerItem=NULL;
		m_ServerItemMap.GetNextAssoc(PositionServer,wServerID,pGameServerItem);

		//统计人数
		if (pGameServerItem->m_GameServer.wKindID==wKindID)
		{
			dwOnLineCount+=pGameServerItem->m_GameServer.dwOnLineCount;
		}
	}

	return dwOnLineCount;
}

//插入种类
bool CServerListManager::InsertGameType(tagGameType * pGameType)
{
	//效验参数
	ASSERT(pGameType!=NULL);
	if (pGameType==NULL) return false;

	//查找现存
	CGameTypeItem * pGameTypeItem=NULL;
	if (m_TypeItemMap.Lookup(pGameType->wTypeID,pGameTypeItem)==FALSE)
	{
		//创建对象
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

		//设置变量
		ZeroMemory(pGameTypeItem,sizeof(CGameTypeItem));
	}

	//设置数据
	pGameTypeItem->m_bDisuse=false;
	CopyMemory(&pGameTypeItem->m_GameType,pGameType,sizeof(tagGameType));

	//设置索引
	m_TypeItemMap[pGameType->wTypeID]=pGameTypeItem;

	return true;
}

//插入类型
bool CServerListManager::InsertGameKind(tagGameKindEx * pGameKind)
{
	//效验参数
	ASSERT(pGameKind!=NULL);
	if (pGameKind==NULL) return false;

	//查找现存
	CGameKindItem * pGameKindItem=NULL;
	if (m_KindItemMap.Lookup(pGameKind->wKindID,pGameKindItem)==FALSE)
	{
		//创建对象
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

		//重置对象
		pGameKindItem->ResetKindItem();
	}

	//设置数据
	pGameKindItem->m_bDisuse=false;
	CopyMemory(&pGameKindItem->m_GameKind,pGameKind,sizeof(pGameKindItem->m_GameKind));

	//设置索引
	m_KindItemMap[pGameKind->wKindID]=pGameKindItem;

	return true;
}

//插入节点
bool CServerListManager::InsertGameNode(tagGameNode * pGameNode)
{
	//效验参数
	ASSERT(pGameNode!=NULL);
	if (pGameNode==NULL) return false;

	//查找现存
	CGameNodeItem * pGameNodeItem=NULL;
	if (m_NodeItemMap.Lookup(pGameNode->wNodeID,pGameNodeItem)==FALSE) 
	{
		//创建对象
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

		//设置变量
		ZeroMemory(pGameNodeItem,sizeof(CGameNodeItem));
	}

	//插入数据
	pGameNodeItem->m_bDisuse=false;
	CopyMemory(&pGameNodeItem->m_GameNode,pGameNode,sizeof(tagGameNode));

	//设置索引
	m_NodeItemMap[pGameNode->wNodeID]=pGameNodeItem;

	return true;
}

//插入定制
bool CServerListManager::InsertGameCustom(tagGameCustom * pGameCustom)
{
	//效验参数
	ASSERT(pGameCustom!=NULL);
	if (pGameCustom==NULL) return false;

	//获取子项
	CGameCustomItem * pGameCustomItem=NULL;
	if (m_CustomItemMap.Lookup(pGameCustom->wPageID,pGameCustomItem)==FALSE) 
	{
		//创建对象
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

		//设置变量
		ZeroMemory(pGameCustomItem,sizeof(CGameCustomItem));
	}

	//设置数据
	pGameCustomItem->m_bDisuse=false;
	CopyMemory(&pGameCustomItem->m_GameCustom,pGameCustom,sizeof(tagGameCustom));

	//设置索引
	m_CustomItemMap[pGameCustom->wPageID]=pGameCustomItem;

	return true;
}

//插入房间
bool CServerListManager::InsertGameServer(tagGameServer * pGameServer)
{
	//效验参数
	ASSERT(pGameServer!=NULL);
	if (pGameServer==NULL) return false;

	//查找过滤
	WORD wServerID=pGameServer->wServerID;
	CGameFilterItem * pGameFilterItem=SearchGameFilter(wServerID);

	//过滤数据
	if (pGameFilterItem!=NULL)
	{
		//插入过滤
		if (pGameFilterItem->InsertVerdict()==false) return true;
	}

	//类型过滤
	if (SearchGameKind(pGameServer->wKindID)==NULL) return true;

	//获取子项
	CGameServerItem * pGameServerItem=NULL;
	if (m_ServerItemMap.Lookup(pGameServer->wServerID,pGameServerItem)==FALSE)
	{
		//创建对象
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

		//设置变量
		ZeroMemory(pGameServerItem,sizeof(CGameServerItem));
	}

	//设置数据
	pGameServerItem->m_bDisuse=false;
	CopyMemory(&pGameServerItem->m_GameServer,pGameServer,sizeof(tagGameServer));

	//过滤数据
	if (pGameFilterItem!=NULL)
	{
		//设置变量
		pGameServerItem->m_GameServer.wSortID=pGameFilterItem->FilterSortID(pGameServer->wSortID);
		pGameServerItem->m_GameServer.wKindID=pGameFilterItem->FilterKindID(pGameServer->wKindID);
		pGameServerItem->m_GameServer.wNodeID=pGameFilterItem->FilterNodeID(pGameServer->wNodeID);

		//房间名字
		LPCTSTR pszServerName=pGameFilterItem->FilterServerName(pGameServer->szServerName);
		StringCchCopy(pGameServerItem->m_GameServer.szServerName,CountArray(pGameServerItem->m_GameServer.szServerName), pszServerName);
	}

	//设置索引
	m_ServerItemMap[pGameServer->wServerID]=pGameServerItem;

	return true;
}

//插入过滤
bool CServerListManager::InsertGameFilter(tagGameFilter * pGameFilter)
{
	//效验参数
	ASSERT(pGameFilter!=NULL);
	if (pGameFilter==NULL) return false;

	//获取子项
	CGameFilterItem * pGameFilterItem=NULL;
	if (m_FilterItemMap.Lookup(pGameFilter->wServerID,pGameFilterItem)==FALSE)
	{
		//创建对象
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

		//设置变量
		ZeroMemory(pGameFilterItem,sizeof(CGameFilterItem));
	}

	//设置数据
	pGameFilterItem->m_bDisuse=false;
	CopyMemory(&pGameFilterItem->m_GameFilter,pGameFilter,sizeof(tagGameFilter));

	//设置索引
	m_FilterItemMap[pGameFilter->wServerID]=pGameFilterItem;

	//过滤处理
	if (pGameFilterItem->InsertVerdict()==true)
	{
		//查找房间
		WORD wServerID=pGameFilterItem->m_GameFilter.wServerID;
		CGameServerItem * pGameServerItem=SearchGameServer(wServerID);

		//设置房间
		if (pGameServerItem!=NULL)
		{
			//变量定义
			tagGameServer * pGameServer=&pGameServerItem->m_GameServer;

			//设置变量
			pGameServer->wSortID=pGameFilterItem->FilterSortID(pGameServer->wSortID);
			pGameServer->wKindID=pGameFilterItem->FilterKindID(pGameServer->wKindID);
			pGameServer->wNodeID=pGameFilterItem->FilterNodeID(pGameServer->wNodeID);

			//房间名字
			LPCTSTR pszServerName=pGameFilterItem->FilterServerName(pGameServer->szServerName);
			StringCchCopy(pGameServer->szServerName,CountArray(pGameServer->szServerName), pszServerName);
		}
	}
	else
	{
		//删除房间
		DeleteGameServer(pGameFilter->wServerID);
	}

	return true;
}

//删除种类
bool CServerListManager::DeleteGameType(WORD wTypeID)
{
	//查找种类
	CGameTypeItem * pGameTypeItem=NULL;
	if (m_TypeItemMap.Lookup(wTypeID,pGameTypeItem)==FALSE) return false;

	//删除数据
	m_TypeItemMap.RemoveKey(wTypeID);
	m_TypeItemBuffer.Add(pGameTypeItem);

	return true;
}

//删除类型
bool CServerListManager::DeleteGameKind(WORD wKindID)
{
	//查找类型
	CGameKindItem * pGameKindItem=NULL;
	if (m_KindItemMap.Lookup(wKindID,pGameKindItem)==FALSE) return false;

	//删除数据
	m_KindItemMap.RemoveKey(wKindID);
	m_KindItemBuffer.Add(pGameKindItem);

	//设置变量
	pGameKindItem->ResetKindItem();

	return true;
}

//删除节点
bool CServerListManager::DeleteGameNode(WORD wNodeID)
{
	//查找节点
	CGameNodeItem * pGameNodeItem=NULL;
	if (m_NodeItemMap.Lookup(wNodeID,pGameNodeItem)==FALSE) return false;

	//删除数据
	m_NodeItemMap.RemoveKey(wNodeID);
	m_NodeItemBuffer.Add(pGameNodeItem);

	//设置变量
	ZeroMemory(pGameNodeItem,sizeof(CGameNodeItem));

	return false;
}

//删除房间
bool CServerListManager::DeleteGameServer(WORD wServerID)
{
	//查找房间
	CGameServerItem * pGameServerItem=NULL;
	if (m_ServerItemMap.Lookup(wServerID,pGameServerItem)==FALSE) return false;

	//删除数据
	m_ServerItemMap.RemoveKey(wServerID);
	m_ServerItemBuffer.Add(pGameServerItem);

	//设置变量
	ZeroMemory(pGameServerItem,sizeof(CGameServerItem));

	return false;
}

//删除定制
bool CServerListManager::DeleteGameCustom(WORD wCustomID)
{
	//查找类型
	CGameCustomItem * pGameCustomItem=NULL;
	if (m_CustomItemMap.Lookup(wCustomID,pGameCustomItem)==FALSE) return false;

	//删除数据
	m_CustomItemMap.RemoveKey(wCustomID);
	m_CustomItemBuffer.Add(pGameCustomItem);

	//设置变量
	ZeroMemory(pGameCustomItem,sizeof(CGameCustomItem));

	return true;
}

//删除过滤
bool CServerListManager::DeleteGameFilter(WORD wServerID)
{
	//查找类型
	CGameFilterItem * pGameFilterItem=NULL;
	if (m_FilterItemMap.Lookup(wServerID,pGameFilterItem)==FALSE) return false;

	//删除数据
	m_FilterItemMap.RemoveKey(wServerID);
	m_FilterItemBuffer.Add(pGameFilterItem);

	//设置变量
	ZeroMemory(pGameFilterItem,sizeof(pGameFilterItem));

	return true;
}

//枚举种类
CGameTypeItem * CServerListManager::EmunGameTypeItem(POSITION & Position)
{
	//变量定义
	WORD wKey=0;
	CGameTypeItem * pGameTypeItem=NULL;

	//获取对象
	if (Position==NULL) Position=m_TypeItemMap.GetStartPosition();
	if (Position!=NULL) m_TypeItemMap.GetNextAssoc(Position,wKey,pGameTypeItem);

	return pGameTypeItem;
}

//枚举类型
CGameKindItem * CServerListManager::EmunGameKindItem(POSITION & Position)
{
	//变量定义
	WORD wKey=0;
	CGameKindItem * pGameKindItem=NULL;

	//获取对象
	if (Position==NULL) Position=m_KindItemMap.GetStartPosition();
	if (Position!=NULL) m_KindItemMap.GetNextAssoc(Position,wKey,pGameKindItem);

	return pGameKindItem;
}

//枚举节点
CGameNodeItem * CServerListManager::EmunGameNodeItem(POSITION & Position)
{
	//变量定义
	WORD wKey=0;
	CGameNodeItem * pGameNodeItem=NULL;

	//获取对象
	if (Position==NULL) Position=m_NodeItemMap.GetStartPosition();
	if (Position!=NULL) m_NodeItemMap.GetNextAssoc(Position,wKey,pGameNodeItem);

	return pGameNodeItem;
}

//枚举房间
CGameServerItem * CServerListManager::EmunGameServerItem(POSITION & Position)
{
	//变量定义
	WORD wKey=0;
	CGameServerItem * pGameServerItem=NULL;

	//获取对象
	if (Position==NULL) Position=m_ServerItemMap.GetStartPosition();
	if (Position!=NULL) m_ServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);

	return pGameServerItem;
}

//枚举定制
CGameCustomItem * CServerListManager::EmunGameCustomItem(POSITION & Position)
{
	//变量定义
	WORD wKey=0;
	CGameCustomItem * pGameCustomItem=NULL;

	//获取对象
	if (Position==NULL) Position=m_CustomItemMap.GetStartPosition();
	if (Position!=NULL) m_CustomItemMap.GetNextAssoc(Position,wKey,pGameCustomItem);

	return pGameCustomItem;
}

//枚举过滤
CGameFilterItem * CServerListManager::EmunGameFilterItem(POSITION & Position)
{
	//变量定义
	WORD wKey=0;
	CGameFilterItem * pGameFilterItem=NULL;

	//获取对象
	if (Position==NULL) Position=m_FilterItemMap.GetStartPosition();
	if (Position!=NULL) m_FilterItemMap.GetNextAssoc(Position,wKey,pGameFilterItem);

	return pGameFilterItem;
}

//查找种类
CGameTypeItem * CServerListManager::SearchGameType(WORD wTypeID)
{
	CGameTypeItem * pGameTypeItem=NULL;
	m_TypeItemMap.Lookup(wTypeID,pGameTypeItem);
	return pGameTypeItem;
}

//查找类型
CGameKindItem * CServerListManager::SearchGameKind(WORD wKindID)
{
	CGameKindItem * pGameKindItem=NULL;
	m_KindItemMap.Lookup(wKindID,pGameKindItem);
	return pGameKindItem;
}

//查找节点
CGameNodeItem * CServerListManager::SearchGameNode(WORD wNodeID)
{
	CGameNodeItem * pGameNodeItem=NULL;
	m_NodeItemMap.Lookup(wNodeID,pGameNodeItem);
	return pGameNodeItem;
}

//查找房间
CGameServerItem * CServerListManager::SearchGameServer(WORD wServerID)
{
	CGameServerItem * pGameServerItem=NULL;
	m_ServerItemMap.Lookup(wServerID,pGameServerItem);
	return pGameServerItem;
}

//查找定制
CGameCustomItem * CServerListManager::SearchGameCustom(WORD wCustomID)
{
	CGameCustomItem * pGameCustomItem=NULL;
	m_CustomItemMap.Lookup(wCustomID,pGameCustomItem);
	return pGameCustomItem;
}

//查找过滤
CGameFilterItem * CServerListManager::SearchGameFilter(WORD wServerID)
{
	CGameFilterItem * pGameFilterItem=NULL;
	m_FilterItemMap.Lookup(wServerID,pGameFilterItem);
	return pGameFilterItem;
}

//////////////////////////////////////////////////////////////////////////
