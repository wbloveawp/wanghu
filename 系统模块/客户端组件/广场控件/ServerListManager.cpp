#include "StdAfx.h"
#include "ServerListManager.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
CServerListManager * CServerListManager::m_pServerListManager=NULL;		//列表管理

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameListItem::CGameListItem(enItemGenre ItemGenre)
{
	//属性数据
	m_ItemGenre=ItemGenre;
	m_pParentListItem=NULL;

	//子项数据
	m_hTreeItemNormal=NULL;
	m_hTreeItemAttach=NULL;

	return;
}

//析构函数
CGameListItem::~CGameListItem()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameTypeItem::CGameTypeItem() : CGameListItem(ItemGenre_Type)
{
	//属性数据
	ZeroMemory(&m_GameType,sizeof(m_GameType));
}

//析构函数
CGameTypeItem::~CGameTypeItem()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameKindItem::CGameKindItem() : CGameListItem(ItemGenre_Kind)
{
	//更新变量
	m_dwUpdateTime=0;
	m_bUpdateItem=false;

	//更新句柄
	m_hItemNormalUpdate=NULL;
	m_hItemAttachUpdate=NULL;

	//扩展数据
	m_dwProcessVersion=0L;

	//属性数据
	ZeroMemory(&m_GameKind,sizeof(m_GameKind));

	return;
}

//析构函数
CGameKindItem::~CGameKindItem()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameNodeItem::CGameNodeItem() : CGameListItem(ItemGenre_Node)
{
	//属性数据
	ZeroMemory(&m_GameNode,sizeof(m_GameNode));
}

//析构函数
CGameNodeItem::~CGameNodeItem()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameServerItem::CGameServerItem() : CGameListItem(ItemGenre_Server)
{
	//辅助变量
	m_pGameKindItem=NULL;

	//扩展数据
	m_ServerStatus=ServerStatus_Normal;

	//属性数据
	ZeroMemory(&m_GameServer,sizeof(m_GameServer));
}

//析构函数
CGameServerItem::~CGameServerItem()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameCustomItem::CGameCustomItem() : CGameListItem(ItemGenre_Custom)
{
	//属性数据
	ZeroMemory(&m_GameCustom,sizeof(m_GameCustom));
}

//析构函数
CGameCustomItem::~CGameCustomItem()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameInsideItem::CGameInsideItem() : CGameListItem(ItemGenre_Inside)
{
}

//析构函数
CGameInsideItem::~CGameInsideItem()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerListManager::CServerListManager()
{
	//接口变量
	m_pIServerListViewSink=NULL;

	//设置质数
	m_GameTypeItemMap.InitHashTable(PRIME_TYPE);
	m_GameKindItemMap.InitHashTable(PRIME_KIND);
	m_GameNodeItemMap.InitHashTable(PRIME_NODE);
	m_GameServerItemMap.InitHashTable(PRIME_SERVER);
	m_GameCustomItemMap.InitHashTable(PRIME_CUSTOM);

	//设置对象
	ASSERT(m_pServerListManager==NULL);
	if (m_pServerListManager==NULL) m_pServerListManager=this;

	return;
}

//析构函数
CServerListManager::~CServerListManager()
{
	//变量定义
	WORD wKey=0;

	//删除种类
	CGameTypeItem * pGameTypeItem=NULL;
	POSITION Position=m_GameTypeItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GameTypeItemMap.GetNextAssoc(Position,wKey,pGameTypeItem);
		SafeDelete(pGameTypeItem);
	}
	m_GameTypeItemMap.RemoveAll();

	//删除类型
	CGameKindItem * pGameKindItem=NULL;
	Position=m_GameKindItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GameKindItemMap.GetNextAssoc(Position,wKey,pGameKindItem);
		SafeDelete(pGameKindItem);
	}
	m_GameKindItemMap.RemoveAll();

	//删除节点
	CGameNodeItem * pGameNodeItem=NULL;
	Position=m_GameNodeItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GameNodeItemMap.GetNextAssoc(Position,wKey,pGameNodeItem);
		SafeDelete(pGameNodeItem);
	}
	m_GameNodeItemMap.RemoveAll();

	//删除房间
	CGameServerItem * pGameServerItem=NULL;
	Position=m_GameServerItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GameServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);
		SafeDelete(pGameServerItem);
	}
	m_GameServerItemMap.RemoveAll();

	//删除定制
	CGameCustomItem * pGameCustomItem=NULL;
	Position=m_GameCustomItemMap.GetStartPosition();
	while (Position!=NULL)
	{
		m_GameCustomItemMap.GetNextAssoc(Position,wKey,pGameCustomItem);
		SafeDelete(pGameCustomItem);
	}
	m_GameCustomItemMap.RemoveAll();

	//释放对象
	ASSERT(m_pServerListManager==this);
	if (m_pServerListManager==this) m_pServerListManager=NULL;

	return;
}

//设置接口
VOID CServerListManager::SetServerListViewSink(IServerListViewSink * pIServerListViewSink)
{
	//设置变量
	m_pIServerListViewSink=pIServerListViewSink;

	return;
}

//完成通知
VOID CServerListManager::OnEventListFinish()
{
	//事件通知
	ASSERT(m_pIServerListViewSink!=NULL);
	if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameItemFinish();

	return;
}

//完成通知
VOID CServerListManager::OnEventKindFinish(WORD wKindID)
{
	//事件通知
	ASSERT(m_pIServerListViewSink!=NULL);
	if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameKindFinish(wKindID);

	return;
}

//设置人数
VOID CServerListManager::SetKindOnLineCount(WORD wKindID, DWORD dwOnLineCount)
{
	//搜索类型
	CGameKindItem * pGameKindItem=SearchGameKind(wKindID);

	//设置人数
	if (pGameKindItem!=NULL)
	{
		//设置变量
		pGameKindItem->m_GameKind.dwOnLineCount=dwOnLineCount;

		//事件通知
		ASSERT(m_pIServerListViewSink!=NULL);
		if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameItemUpdate(pGameKindItem);
	}

	return;
}

//设置人数
VOID CServerListManager::SetServerOnLineCount(WORD wServerID, DWORD dwOnLineCount)
{
	//搜索房间
	CGameServerItem * pGameServerItem=SearchGameServer(wServerID);

	//设置人数
	if (pGameServerItem!=NULL)
	{
		//设置变量
		pGameServerItem->m_GameServer.dwOnLineCount=dwOnLineCount;

		//事件通知
		ASSERT(m_pIServerListViewSink!=NULL);
		if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameItemUpdate(pGameServerItem);
	}

	return;
}

//设置人数
VOID CServerListManager::UpdateOnlineCount()
{
	//计算总人数
	DWORD dwOnlineCount = 0;
	CGameKindItem *pGameKindItem = NULL;
	POSITION pos = NULL;
	for( DWORD i = 0; i < GetGameKindCount(); i++ )
	{
		pGameKindItem = EmunGameKindItem(pos);
		if( !pGameKindItem ) break;

		dwOnlineCount += pGameKindItem->m_GameKind.dwOnLineCount;
	}

	//事件通知
	ASSERT(m_pIServerListViewSink!=NULL);
	if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnUpdateOnlineCount(dwOnlineCount);
}

//插入种类
bool CServerListManager::InsertGameType(tagGameType * pGameType)
{
	//效验参数
	ASSERT(pGameType!=NULL);
	if (pGameType==NULL) return false;

	//变量定义
	CGameTypeItem * pGameTypeItem=NULL;
	bool bInsert=(m_GameTypeItemMap.Lookup(pGameType->wTypeID,pGameTypeItem)==FALSE);

	//获取存储
	if (bInsert==true)
	{
		pGameTypeItem=new CGameTypeItem;
		if (pGameTypeItem==NULL) return false;
	}

	//设置数据
	CopyMemory(&pGameTypeItem->m_GameType,pGameType,sizeof(tagGameType));

	//寻找父项
	if (pGameType->wJoinID!=0)
	{
		//寻找父项
		pGameTypeItem->m_pParentListItem=SearchGameType(pGameType->wJoinID);

		//待决判断
		if (pGameTypeItem->m_pParentListItem==NULL) 
		{
			m_GameListItemWait.Add(pGameTypeItem);
			return true;
		}
	}

	//插入处理
	if (bInsert==true)
	{
		//设置索引
		m_GameTypeItemMap[pGameType->wTypeID]=pGameTypeItem;

		//界面更新
		m_pIServerListViewSink->OnGameItemInsert(pGameTypeItem);
	}
	else
	{
		//界面更新
		m_pIServerListViewSink->OnGameItemUpdate(pGameTypeItem);
	}

	return true;
}

//插入类型
bool CServerListManager::InsertGameKind(tagGameKind * pGameKind)
{
	//效验参数
	ASSERT(pGameKind!=NULL);
	if (pGameKind==NULL) return false;

	//变量定义
	CGameKindItem * pGameKindItem=NULL;
	bool bInsert=(m_GameKindItemMap.Lookup(pGameKind->wKindID,pGameKindItem)==FALSE);

	//获取存储
	if (bInsert==true)
	{
		pGameKindItem=new CGameKindItem;
		if (pGameKindItem==NULL) return false;
	}

	//获取版本
	if (bInsert==true)
	{
		LPCTSTR pszProcessName=CW2CT(pGameKind->szProcessName);
		CWHService::GetModuleVersion(pszProcessName,pGameKindItem->m_dwProcessVersion);
	}

	//设置数据
	CopyMemory(&pGameKindItem->m_GameKind,pGameKind,sizeof(tagGameKind));

	//寻找父项
	/*if (pGameKind->wJoinID!=0)
	{
		pGameKindItem->m_pParentListItem=SearchGameKind(pGameKind->wJoinID);
	}
	else
	{*/
		pGameKindItem->m_pParentListItem=SearchGameType(pGameKind->wTypeID);
	//}

	//待决判断
	if (pGameKindItem->m_pParentListItem==NULL) 
	{
		m_GameListItemWait.Add(pGameKindItem);
		return true;
	}

	//插入处理
	if (bInsert==true)
	{
		//设置索引
		m_GameKindItemMap[pGameKind->wKindID]=pGameKindItem;

		//插入子项
		m_pIServerListViewSink->OnGameItemInsert(pGameKindItem);
	}
	else
	{
		//更新子项
		m_pIServerListViewSink->OnGameItemUpdate(pGameKindItem);
	}

	return true;
}

//插入节点
bool CServerListManager::InsertGameNode(tagGameNode * pGameNode)
{
	//效验参数
	ASSERT(pGameNode!=NULL);
	if (pGameNode==NULL) return false;

	//变量定义
	CGameNodeItem * pGameNodeItem=NULL;
	bool bInsert=(m_GameNodeItemMap.Lookup(pGameNode->wNodeID,pGameNodeItem)==FALSE);

	//获取存储
	if (bInsert==true)
	{
		pGameNodeItem=new CGameNodeItem;
		if (pGameNodeItem==NULL) return false;
	}

	//设置数据
	CopyMemory(&pGameNodeItem->m_GameNode,pGameNode,sizeof(tagGameNode));

	//寻找父项
	if (pGameNode->wJoinID!=0)
	{
		pGameNodeItem->m_pParentListItem=SearchGameNode(pGameNode->wJoinID);
	}
	else
	{
		pGameNodeItem->m_pParentListItem=SearchGameKind(pGameNode->wKindID);
	}

	//待决判断
	if (pGameNodeItem->m_pParentListItem==NULL) 
	{
		m_GameListItemWait.Add(pGameNodeItem);
		return true;
	}

	//插入处理
	if (bInsert==true)
	{
		//设置索引
		m_GameNodeItemMap[pGameNode->wNodeID]=pGameNodeItem;

		//插入子项
		m_pIServerListViewSink->OnGameItemInsert(pGameNodeItem);
	}
	else 
	{
		//更新子项
		m_pIServerListViewSink->OnGameItemUpdate(pGameNodeItem);
	}

	return true;
}

//插入定制
bool CServerListManager::InsertGameCustom(tagGameCustom * pGameCustom)
{
	//效验参数
	ASSERT(pGameCustom!=NULL);
	if (pGameCustom==NULL) return false;

	//变量定义
	CGameCustomItem * pGameCustomItem=NULL;
	bool bInsert=(m_GameCustomItemMap.Lookup(pGameCustom->wPageID,pGameCustomItem)==FALSE);

	//获取存储
	if (bInsert==true)
	{
		pGameCustomItem=new CGameCustomItem;
		if (pGameCustomItem==NULL) return false;
	}

	//设置数据
	CopyMemory(&pGameCustomItem->m_GameCustom,pGameCustom,sizeof(tagGameCustom));

	//寻找父项
	if ((pGameCustom->wNodeID!=0)||(pGameCustom->wKindID!=0))
	{
		//寻找父项
		if (pGameCustom->wNodeID!=0) pGameCustomItem->m_pParentListItem=SearchGameNode(pGameCustom->wNodeID);
		else if (pGameCustom->wKindID!=0) pGameCustomItem->m_pParentListItem=SearchGameKind(pGameCustom->wKindID);

		//待决判断
		if (pGameCustomItem->m_pParentListItem==NULL) 
		{
			m_GameListItemWait.Add(pGameCustomItem);
			return true;
		}
	}

	//插入处理
	if (bInsert==true)
	{
		//设置索引
		m_GameCustomItemMap[pGameCustom->wPageID]=pGameCustomItem;

		//插入子项
		m_pIServerListViewSink->OnGameItemInsert(pGameCustomItem);
	}
	else 
	{
		//更新子项
		m_pIServerListViewSink->OnGameItemUpdate(pGameCustomItem);
	}

	return true;
}

//插入房间
bool CServerListManager::InsertGameServer(tagPCGameServer * pGameServer)
{
	//效验参数
	ASSERT(pGameServer!=NULL);
	if (pGameServer==NULL) return false;

	//变量定义
	CGameServerItem * pGameServerItem=NULL;
	bool bInsert=(m_GameServerItemMap.Lookup(pGameServer->wServerID,pGameServerItem)==FALSE);

	//获取存储
	if (bInsert==true)
	{
		pGameServerItem=new CGameServerItem;
		if (pGameServerItem==NULL) return false;
	}

	//设置数据
	CopyMemory(&pGameServerItem->m_GameServer,pGameServer,sizeof(tagPCGameServer));

	//寻找父项
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

	//待决判断
	if (pGameServerItem->m_pParentListItem==NULL) 
	{
		m_GameListItemWait.Add(pGameServerItem);
		return true;
	}

	//插入处理
	if (bInsert==true)
	{
		//设置索引
		m_GameServerItemMap[pGameServer->wServerID]=pGameServerItem;

		//插入子项
		m_pIServerListViewSink->OnGameItemInsert(pGameServerItem);
	}
	else 
	{
		//更新子项
		m_pIServerListViewSink->OnGameItemUpdate(pGameServerItem);
	}

	return true;
}

//删除种类
bool CServerListManager::DeleteGameType(WORD wTypeID)
{
	//查找种类
	CGameTypeItem * pGameTypeItem=NULL;
	if (m_GameTypeItemMap.Lookup(wTypeID,pGameTypeItem)==FALSE) return false;

	//删除通知
	ASSERT(m_pIServerListViewSink!=NULL);
	if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameItemDelete(pGameTypeItem);

	//重置数据
	SafeDelete(pGameTypeItem);

	//删除数据
	m_GameTypeItemMap.RemoveKey(wTypeID);

	return true;
}

//删除类型
bool CServerListManager::DeleteGameKind(WORD wKindID)
{
	//查找类型
	CGameKindItem * pGameKindItem=NULL;
	if (m_GameKindItemMap.Lookup(wKindID,pGameKindItem)==FALSE) return false;

	//删除通知
	ASSERT(m_pIServerListViewSink!=NULL);
	if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameItemDelete(pGameKindItem);

	//删除数据
	SafeDelete(pGameKindItem);

	//删除数据
	m_GameKindItemMap.RemoveKey(wKindID);

	return true;
}

//删除节点
bool CServerListManager::DeleteGameNode(WORD wNodeID)
{
	//查找节点
	CGameNodeItem * pGameNodeItem=NULL;
	if (m_GameNodeItemMap.Lookup(wNodeID,pGameNodeItem)==FALSE) return false;

	//删除通知
	ASSERT(m_pIServerListViewSink!=NULL);
	if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameItemDelete(pGameNodeItem);

	//删除数据
	SafeDelete(pGameNodeItem);

	//删除数据
	m_GameNodeItemMap.RemoveKey(wNodeID);

	return false;
}

//删除房间
bool CServerListManager::DeleteGameServer(WORD wServerID)
{
	//查找房间
	CGameServerItem * pGameServerItem=NULL;
	if (m_GameServerItemMap.Lookup(wServerID,pGameServerItem)==FALSE) return false;

	//删除通知
	ASSERT(m_pIServerListViewSink!=NULL);
	if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameItemDelete(pGameServerItem);

	//删除数据
	SafeDelete(pGameServerItem);

	//删除数据
	m_GameServerItemMap.RemoveKey(wServerID);

	return false;
}

//删除定制
bool CServerListManager::DeleteGameCustom(WORD wCustomID)
{
	//查找类型
	CGameCustomItem * pGameCustomItem=NULL;
	if (m_GameCustomItemMap.Lookup(wCustomID,pGameCustomItem)==FALSE) return false;

	//删除通知
	ASSERT(m_pIServerListViewSink!=NULL);
	if (m_pIServerListViewSink!=NULL) m_pIServerListViewSink->OnGameItemDelete(pGameCustomItem);

	//删除数据
	SafeDelete(pGameCustomItem);

	//删除数据
	m_GameCustomItemMap.RemoveKey(wCustomID);

	return true;
}

//枚举种类
CGameTypeItem * CServerListManager::EmunGameTypeItem(POSITION & Position)
{
	//调整参数
	if (Position==NULL) Position=m_GameTypeItemMap.GetStartPosition();
	if (Position==NULL) return NULL;

	//查找房间
	WORD wKey=0;
	CGameTypeItem * pGameTypeItem=NULL;
	m_GameTypeItemMap.GetNextAssoc(Position,wKey,pGameTypeItem);

	return pGameTypeItem;
}

//枚举类型
CGameKindItem * CServerListManager::EmunGameKindItem(POSITION & Position)
{
	//调整参数
	if (Position==NULL) Position=m_GameKindItemMap.GetStartPosition();
	if (Position==NULL) return NULL;

	//查找房间
	WORD wKey=0;
	CGameKindItem * pGameKindItem=NULL;
	m_GameKindItemMap.GetNextAssoc(Position,wKey,pGameKindItem);

	return pGameKindItem;
}

//枚举节点
CGameNodeItem * CServerListManager::EmunGameNodeItem(POSITION & Position)
{
	//调整参数
	if (Position==NULL) Position=m_GameNodeItemMap.GetStartPosition();
	if (Position==NULL) return NULL;

	//查找节点
	WORD wKey=0;
	CGameNodeItem * pGameNodeItem=NULL;
	m_GameNodeItemMap.GetNextAssoc(Position,wKey,pGameNodeItem);

	return pGameNodeItem;
}

//枚举定制
CGameCustomItem * CServerListManager::EmunGameCustomItem(POSITION & Position)
{
	//调整参数
	if (Position==NULL) Position=m_GameCustomItemMap.GetStartPosition();
	if (Position==NULL) return NULL;

	//查找房间
	WORD wKey=0;
	CGameCustomItem * pGameCustomItem=NULL;
	m_GameCustomItemMap.GetNextAssoc(Position,wKey,pGameCustomItem);

	return pGameCustomItem;
}

//枚举房间
CGameServerItem * CServerListManager::EmunGameServerItem(POSITION & Position)
{
	//调整参数
	if (Position==NULL) Position=m_GameServerItemMap.GetStartPosition();
	if (Position==NULL) return NULL;

	//查找房间
	WORD wKey=0;
	CGameServerItem * pGameServerItem=NULL;
	m_GameServerItemMap.GetNextAssoc(Position,wKey,pGameServerItem);

	return pGameServerItem;
}

//查找种类
CGameTypeItem * CServerListManager::SearchGameType(WORD wTypeID)
{
	CGameTypeItem * pGameTypeItem=NULL;
	m_GameTypeItemMap.Lookup(wTypeID,pGameTypeItem);
	return pGameTypeItem;
}

//查找类型
CGameKindItem * CServerListManager::SearchGameKind(WORD wKindID)
{
	CGameKindItem * pGameKindItem=NULL;
	m_GameKindItemMap.Lookup(wKindID,pGameKindItem);
	return pGameKindItem;
}

//查找节点
CGameNodeItem * CServerListManager::SearchGameNode(WORD wNodeID)
{
	CGameNodeItem * pGameNodeItem=NULL;
	m_GameNodeItemMap.Lookup(wNodeID,pGameNodeItem);
	return pGameNodeItem;
}

//查找房间
CGameServerItem * CServerListManager::SearchGameServer(WORD wServerID)
{
	CGameServerItem * pGameServerItem=NULL;
	m_GameServerItemMap.Lookup(wServerID,pGameServerItem);
	return pGameServerItem;
}

//查找定制
CGameCustomItem * CServerListManager::SearchGameCustom(WORD wCustomID)
{
	CGameCustomItem * pGameCustomItem=NULL;
	m_GameCustomItemMap.Lookup(wCustomID,pGameCustomItem);
	return pGameCustomItem;
}

//更新列表
VOID CServerListManager::UpdateGameKind(WORD wKindID)
{
	//变量定义
	CGameKindItem * pGameKindItem=NULL;
	m_GameKindItemMap.Lookup(wKindID,pGameKindItem);

	//获取版本
	if (pGameKindItem!=NULL)
	{
		LPCTSTR pszProcessName=CW2CT(pGameKindItem->m_GameKind.szProcessName);
		CWHService::GetModuleVersion(pszProcessName,pGameKindItem->m_dwProcessVersion);
	}

	//更新子项
	m_pIServerListViewSink->OnGameItemUpdate(pGameKindItem);

	return;
}

//////////////////////////////////////////////////////////////////////////
