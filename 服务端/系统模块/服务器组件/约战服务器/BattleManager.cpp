#include "stdafx.h"
#include "BattleManager.h"

///////////////////////////////////////////////////////////////////////////////////
//常亮定义
#define MAPPED_NUM_LEN						6							//编号长度
#define MIN_PASSWORD_LEN					6							//密码长度

//节点类型
#define NODE_TYPE_USER						0							//用户节点
#define NODE_TYPE_KIND						1							//类型节点
#define NODE_TYPE_CLUB						2							//茶社节点

///////////////////////////////////////////////////////////////////////////////////

//构造函数
CBattleManager::CBattleManager()
{
	//设置索引
	m_MapServerNode.InitHashTable(103);
	m_MapTableUserID.InitHashTable(PRIME_PLATFORM_USER);	
	m_MapTableKindID.InitHashTable(PRIME_PLATFORM_USER);

	//初始化
	m_StockTableNode.InitHashTable(PRIME_PLATFORM_USER);
	m_StockTableOwner.InitHashTable(PRIME_PLATFORM_USER);	
}

//析构函数
CBattleManager::~CBattleManager()
{
	//重置数据
	ResetData();

	//删除房间
	for (INT_PTR i=0; i<m_ServerNodeBuffer.GetCount(); i++)
	{
		SafeDelete(m_ServerNodeBuffer[i]);
	}

	//移除元素
	m_ServerNodeBuffer.RemoveAll();	
	m_StockTableNode.DeleteAllItem();
	m_StockTableOwner.DeleteAllItem();	
}

//移除房间
bool CBattleManager::RemoveServer(WORD wKindID,WORD wServerID)
{
	//寻找房间
	tagServerNode * pServerNodeHead=NULL;
	if (m_MapServerNode.Lookup(wKindID,pServerNodeHead)==FALSE)
	{
		return false;
	}

	//变量定义
	tagServerNode * pServerNodeMove=pServerNodeHead;

	//查找房间
	while (pServerNodeMove!=NULL)
	{	
		//判断标识
		if (pServerNodeMove->ServerItem.wServerID==wServerID)
		{		
			//设置状态
			pServerNodeMove->ServerStatus.bRemoved=true;
			break;
		}

		//移动节点
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}		

	return true;
}

//添加房间
bool CBattleManager::AppendServer(const tagServerItem & ServerItem)
{
	//寻找房间
	tagServerNode * pServerNodeHead=NULL;
	m_MapServerNode.Lookup(ServerItem.wKindID,pServerNodeHead);

	//查找节点	
	if (pServerNodeHead!=NULL)
	{
    //变量定义
		tagServerNode * pServerNodeLast=NULL;
		tagServerNode * pServerNodeNext=pServerNodeHead;

		//查找房间
		do
		{	
			//判断标识
			if (pServerNodeNext->ServerItem.wServerID==ServerItem.wServerID)
			{		
				//设置状态
				if (pServerNodeNext->ServerStatus.bRemoved==true)
				{					
					pServerNodeNext->ServerStatus.bRemoved=false;					
					pServerNodeNext->ServerItem.wTableCount=ServerItem.wTableCount;
				}

				//链接房间
				if (pServerNodeLast!=NULL)
				{
					pServerNodeLast->pNextServerNode=pServerNodeNext->pNextServerNode;
					pServerNodeNext->pNextServerNode=pServerNodeHead;

					//设置表头
					m_MapServerNode[ServerItem.wKindID]=pServerNodeNext;
				}

				return false;
			}

			//移动节点
			pServerNodeLast=pServerNodeNext;
			pServerNodeNext=pServerNodeNext->pNextServerNode;

		} while (pServerNodeNext!=NULL);		
	}	

	//激活节点
	tagServerNode * pServerNodeNew=ActiveServerNode();
	if (pServerNodeNew==NULL) return true;

	//设置状态
	pServerNodeNew->ServerStatus.bRemoved=false;
	pServerNodeNew->ServerStatus.wTableUseCount=0;

	//拷贝信息
	CopyMemory(&pServerNodeNew->ServerItem,&ServerItem,sizeof(pServerNodeNew->ServerItem));
  
	//链接节点
	pServerNodeNew->pNextServerNode=pServerNodeHead;
	m_MapServerNode[ServerItem.wKindID]=pServerNodeNew;
	
	return true;
}

//移除桌子
bool CBattleManager::RemoveTable(DWORD dwBattleNum)
{
	//寻找桌子
	tagTableNode* pTableNode=m_StockTableNode.SearchStockItem(dwBattleNum);
	if (pTableNode == NULL) return true;

	//变量定义
	tagBattleTableItemEx * pTableItem=&pTableNode->TableItem;

	//移除节点
	RemoveUserTableNode(pTableItem->dwOwnerID,pTableNode);

	//移除节点
	if (pTableItem->dwClubID) RemoveClubTableNode(pTableItem->dwClubID, pTableNode);

	//查找房间			
	tagServerNode * pServerNodeMove=SearchServerNode(pTableItem->BattleConfig.wKindID,pTableItem->wServerID);
	if (pServerNodeMove!=NULL) pServerNodeMove->ServerStatus.wTableUseCount--;

	//移除节点
	m_StockTableNode.RemoveStockItem(dwBattleNum);

	return true;
}

//创建桌子
tagBattleTableItemEx * CBattleManager::CreateTable(DWORD dwUserID, DWORD dwClubID, tagBattleConfigEx & BattleConfig,WORD wServerID)
{
	//变量定义
	tagServerNode *	pServerNodeFind=NULL;	
	tagServerNode * pServerNodeMove=SearchServerNode(BattleConfig.wKindID);	

	//查找房间
	while (pServerNodeMove!=NULL && wServerID!=0)
	{
		if (pServerNodeMove->ServerItem.wServerID==wServerID)
		{
			pServerNodeMove=pServerNodeMove->pNextServerNode;
			break;
		}
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}

	//查找房间
	while (pServerNodeMove!=NULL)
	{
		//判断状态
		if (pServerNodeMove->ServerStatus.bRemoved==false)
		{				
			//桌子数量
			if (pServerNodeMove->ServerStatus.wTableUseCount<pServerNodeMove->ServerItem.wTableCount/10*9)
			{
				pServerNodeFind=pServerNodeMove;
				break;
			}			
		}
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}

	//桌子用尽
	if (pServerNodeFind==NULL) return NULL;

	//变量定义
	int nLoopCount=10000;
	DWORD dwBattleNum = 0;	

	do
	{
		dwBattleNum = BuildNumber(MAPPED_NUM_LEN);
		auto pTableNodeTemp = m_StockTableNode.SearchStockItem(dwBattleNum);
		if (pTableNodeTemp == NULL)
		{
			break;
		}
	} while (--nLoopCount >0);

	//循环判断
	if (nLoopCount <= 0) return NULL;

	//创建节点
	auto pTableNodeNew=m_StockTableNode.CreateStockItem(dwBattleNum);
	if (pTableNodeNew == NULL) return NULL;

	//初始化
	ZeroMemory(pTableNodeNew,sizeof(tagTableNode));

	//设置桌子	
	pTableNodeNew->TableItem.dwOwnerID=dwUserID;
	pTableNodeNew->TableItem.wTableID=INVALID_TABLE;	
	pTableNodeNew->TableItem.dwClubID = dwClubID;
	pTableNodeNew->TableItem.dwBattleNum=dwBattleNum;
	pTableNodeNew->TableItem.dwCreateTime = time(NULL);
	pTableNodeNew->TableItem.wServerID=pServerNodeFind->ServerItem.wServerID;
	CopyMemory(&pTableNodeNew->TableItem.BattleConfig,&BattleConfig,sizeof(pTableNodeNew->TableItem.BattleConfig));	

	//更新状态
	pServerNodeFind->ServerStatus.wTableUseCount++;

	//添加节点
	InsertUserTableNode(dwUserID,pTableNodeNew);

	//茶舍节点
	if (dwClubID != 0) InsertClubTableNode(dwClubID, pTableNodeNew);

	return &pTableNodeNew->TableItem;
}

//重置数据
VOID CBattleManager::ResetData()
{
	//移除房间
	WORD wKindID=0L;
	tagServerNode * pServerNode=NULL;
	POSITION Position=m_MapServerNode.GetStartPosition();
	while (Position!=NULL)
	{
		m_MapServerNode.GetNextAssoc(Position,wKindID,pServerNode);
		while (pServerNode!=NULL)
		{
			m_ServerNodeBuffer.Add(pServerNode);
			pServerNode=pServerNode->pNextServerNode;
		}		
	}	

	//移除元素		
	m_MapServerNode.RemoveAll();
	m_MapTableKindID.RemoveAll();	
	m_MapTableUserID.RemoveAll();	
	m_MapTableClubID.RemoveAll();
	m_StockTableNode.RemoveAllItem();
	m_StockTableOwner.RemoveAllItem();
}

//桌子数量
WORD CBattleManager::GetTableCountByKindID(DWORD dwUserID,WORD wKindID, DWORD dwClubID)
{
	//查找节点
	tagTableNodeHead * pTableNodeHead=SearchTableByUserID(dwUserID);
	if (pTableNodeHead==NULL) return 0;
	
	//所有房间
	//if (wKindID==0) return pTableNodeHead->nNodeCount;

	//变量定义
	WORD wTableCount=0;
	tagTableNode * pTableNodeMove=pTableNodeHead->pTableNodeHead;

	//遍历节点
	while (pTableNodeMove!=NULL)
	{
		if ((pTableNodeMove->TableItem.BattleConfig.wKindID==wKindID || wKindID==0) && pTableNodeMove->TableItem.dwClubID==dwClubID)
		{
			++wTableCount;
		}
		pTableNodeMove=pTableNodeMove->pNextUserNode;
	}

	return wTableCount;
}

//枚举游戏
WORD CBattleManager::EnumTableKindID(POSITION& Position)
{
	WORD wKindID = 0L;
	tagTableNodeHead TableNodeHead;

	//获取对象
	if (Position == NULL) Position = m_MapTableKindID.GetStartPosition();
	if (Position != NULL) m_MapTableKindID.GetNextAssoc(Position, wKindID, TableNodeHead);

	return wKindID;
}

//枚举用户
DWORD CBattleManager::EnumTableUserID(POSITION& Position)
{
	DWORD dwUserID = 0L;
	tagTableNodeHead TableNodeHead;

	//获取对象
	if (Position == NULL) Position = m_MapTableUserID.GetStartPosition();
	if (Position != NULL) m_MapTableUserID.GetNextAssoc(Position, dwUserID, TableNodeHead);

	return dwUserID;
}

//枚举茶社
DWORD CBattleManager::EnumTableClubID(POSITION& Position)
{
	DWORD dwClubID = 0L;
	tagTableNodeHead TableNodeHead;

	//获取对象
	if (Position == NULL) Position = m_MapTableClubID.GetStartPosition();
	if (Position != NULL) m_MapTableClubID.GetNextAssoc(Position, dwClubID, TableNodeHead);

	return dwClubID;
}

//桌子数量
WORD CBattleManager::GetTableCountByClubID(DWORD dwClubID)
{
	//查找节点
	tagTableNodeHead* pTableNodeHead = SearchTableByClubID(dwClubID);
	if (pTableNodeHead == NULL)  return 0;

	return pTableNodeHead->nNodeCount;
}

//桌子数量
WORD CBattleManager::GetTableCountByClubID(DWORD dwUserID, DWORD dwClubID)
{
	//查找节点
	tagTableNodeHead* pTableNodeHead = SearchTableByUserID(dwUserID);
	if (pTableNodeHead == NULL)  return 0;

	//变量定义
	WORD wTableCount = 0;
	tagTableNode* pTableNodeMove = pTableNodeHead->pTableNodeHead;

	//遍历节点
	while (pTableNodeMove != NULL)
	{
		if (pTableNodeMove->TableItem.dwClubID == dwClubID)
		{
			++wTableCount;
		}
		pTableNodeMove = pTableNodeMove->pNextUserNode;
	}

	return wTableCount;
}

//桌子子项
tagBattleTableItemEx * CBattleManager::GetTableItem(DWORD dwBattleNum)
{
	//寻找桌子
	auto pTableNode= m_StockTableNode.SearchStockItem(dwBattleNum);
	
	return pTableNode?&pTableNode->TableItem:NULL;
}

//桌子状态
tagBattleTableParamEx * CBattleManager::GetTableParam(DWORD dwBattleNum)
{
	//寻找桌子
	auto pTableNode = m_StockTableNode.SearchStockItem(dwBattleNum);

	return pTableNode?&pTableNode->TableParam:NULL;
}

//添加桌子
bool CBattleManager::AddTableItem(const tagBattleTableItemEx & TableItem)
{
	//寻找桌子
	tagTableNode * pTableNode= m_StockTableNode.SearchStockItem(TableItem.dwBattleNum);
	if (pTableNode != NULL) return true;

	//查找房间	
	tagServerNode * pServerNodeMove=SearchServerNode(TableItem.BattleConfig.wKindID,TableItem.wServerID);
	if (pServerNodeMove==NULL) return false;

	//创建节点
	tagTableNode* pTableNodeNew=m_StockTableNode.CreateStockItem(TableItem.dwBattleNum);
	if (pTableNodeNew == NULL) return false;

	//初始化
	ZeroMemory(pTableNodeNew, sizeof(tagTableNode));

	//拷贝数据	
	CopyMemory(&pTableNodeNew->TableItem,&TableItem,sizeof(pTableNodeNew->TableItem));

	//更新状态
	pServerNodeMove->ServerStatus.wTableUseCount++;

	//链接节点
	InsertUserTableNode(TableItem.dwOwnerID,pTableNodeNew);

	//链接节点
	if (TableItem.dwClubID != 0) InsertClubTableNode(TableItem.dwClubID, pTableNodeNew);

	return true;
}

//桌子列表
bool CBattleManager::GetRandTableList(WORD wTableCount, CTableItemArray& TableItemArray, CTableParamArray& TableParamArray)
{
	//校验数量
	auto nItemCount = m_StockTableNode.GetItemCount();
	if (nItemCount == 0) return true;

	//索引定义
	INT_PTR nIndex = 0;

	//校验数量
	if (nItemCount <= wTableCount)
	{
		//设置大小
		TableItemArray.SetSize(nItemCount);
		TableParamArray.SetSize(nItemCount);

		//添加数据
		tagTableNode* pTableNodeMove = m_StockTableNode.GetHeadStockItem();
		while (pTableNodeMove != NULL)
		{
			auto pTableNodeCurrent = pTableNodeMove;
			pTableNodeMove = m_StockTableNode.GetNextStockItem(pTableNodeCurrent);

			//公开房间
			if (pTableNodeCurrent->TableItem.BattleConfig.cbAllowPublic)
			{
				TableItemArray.SetAtGrow(nIndex, &pTableNodeCurrent->TableItem);
				TableParamArray.SetAtGrow(nIndex, &pTableNodeCurrent->TableParam);
				nIndex++;
			}			
		}

		//调整大小
		if (nIndex < nItemCount)
		{
			TableItemArray.SetSize(nIndex);
			TableParamArray.SetSize(nIndex);
		}
	}
	else
	{
		//设置大小
		TableItemArray.SetSize(wTableCount);
		TableParamArray.SetSize(wTableCount);

		//变量定义
		INT nLoopIndex = 0;
		INT nRandIndex = rand() % nItemCount;

		//添加数据
		tagTableNode* pTableNodeMove = m_StockTableNode.GetHeadStockItem();
		while (pTableNodeMove != NULL)
		{
			auto pTableNodeCurrent = pTableNodeMove;
			pTableNodeMove = m_StockTableNode.GetNextStockItem(pTableNodeCurrent);

			if (nLoopIndex++>= nRandIndex && pTableNodeCurrent->TableItem.BattleConfig.cbAllowPublic)
			{
				//加入数组
				TableItemArray.SetAtGrow(nIndex, &pTableNodeCurrent->TableItem);
				TableParamArray.SetAtGrow(nIndex, &pTableNodeCurrent->TableParam);

				//退出判断
				if (++nIndex >= wTableCount) break;
			}

			//退出判断
			if (nLoopIndex == nRandIndex+nItemCount) break;

			//尾头衔接
			if (pTableNodeMove == NULL)
			{				
				pTableNodeMove = m_StockTableNode.GetHeadStockItem();
			}
		}

		//调整大小
		if (nIndex < wTableCount)
		{
			TableItemArray.SetSize(nIndex);
			TableParamArray.SetSize(nIndex);
		}
	}

	return true;
}

//桌子列表
bool CBattleManager::GetKindTableList(WORD wKindID,CTableItemArray & TableItemArray,CTableParamArray & TableParamArray)
{
	//变量定义
	tagTableNodeHead * pTableNodeHead=SearchTableByKindID(wKindID);
	if (pTableNodeHead==NULL) return true;

	//变量定义
	tagTableNode * pTableNodeMove=pTableNodeHead->pTableNodeHead;

	//遍历节点
	while (pTableNodeMove!=NULL)
	{
		TableItemArray.Add(&pTableNodeMove->TableItem);
		TableParamArray.Add(&pTableNodeMove->TableParam);
		pTableNodeMove=pTableNodeMove->pNextKindNode;
	}

	return true;
}

//桌子列表
bool CBattleManager::GetUserTableList(DWORD dwUserID,CTableItemArray & TableItemArray,CTableParamArray & TableParamArray)
{
	//变量定义
	tagTableNodeHead * pTableNodeHead=SearchTableByUserID(dwUserID);
	if (pTableNodeHead==NULL) return false;

	//变量定义
	tagTableNode * pTableNodeMove=pTableNodeHead->pTableNodeHead;

	//遍历节点
	while (pTableNodeMove!=NULL)
	{
		if (pTableNodeMove->TableItem.dwClubID == 0)
		{
			TableItemArray.Add(&pTableNodeMove->TableItem);
			TableParamArray.Add(&pTableNodeMove->TableParam);
		}
		pTableNodeMove=pTableNodeMove->pNextUserNode;
	}

	return true;
}

//桌子列表
bool CBattleManager::GetClubTableList(DWORD dwClubID, CTableItemArray& TableItemArray, CTableParamArray& TableParamArray)
{
	//变量定义
	tagTableNodeHead* pTableNodeHead = SearchTableByClubID(dwClubID);
	if (pTableNodeHead == NULL) return false;

	//变量定义
	tagTableNode* pTableNodeMove = pTableNodeHead->pTableNodeHead;

	//遍历节点
	while (pTableNodeMove != NULL)
	{
		TableItemArray.Add(&pTableNodeMove->TableItem);
		TableParamArray.Add(&pTableNodeMove->TableParam);
		pTableNodeMove = pTableNodeMove->pNextClubNode;
	}

	return true;
}

//修改桌主
bool CBattleManager::ModifyTableOwner(DWORD dwBattleNum, DWORD dwNewOwnerID)
{
	//查找桌子
	auto pTableNode = m_StockTableNode.SearchStockItem(dwBattleNum);
	if (pTableNode == NULL) return false;

	//移除桌子
	RemoveUserTableNode(pTableNode->TableItem.dwOwnerID, pTableNode);

	//更新桌主
	pTableNode->TableItem.dwOwnerID = dwNewOwnerID;

	//添加桌子
	InsertUserTableNode(pTableNode->TableItem.dwOwnerID, pTableNode);

	return true;
}

//枚举桌子
tagBattleTableItemEx * CBattleManager::EnumTableItem(tagBattleTableItemEx* pTableItem)
{
	//变量定义
	tagTableNode * pTableNode=NULL;

	//获取节点
	if (pTableItem == NULL)
	{
		pTableNode= m_StockTableNode.GetHeadStockItem();
	}
	else
	{
		//获取节点
		pTableNode = m_StockTableNode.GetNextStockItem(CONTAINING_RECORD(pTableItem, tagTableNode, TableItem));
	}

	return (pTableNode==NULL)?NULL:&pTableNode->TableItem;		
}

//激活房间
tagServerNode * CBattleManager::ActiveServerNode()
{
	//变量定义
	tagServerNode * pServerNode=NULL;

	//查找缓存
	if (m_ServerNodeBuffer.GetCount()>0)
	{
		INT_PTR nIndex=m_ServerNodeBuffer.GetCount()-1;
		pServerNode=m_ServerNodeBuffer[nIndex];
		m_ServerNodeBuffer.RemoveAt(nIndex);		
	}

	//指针判断
	if (pServerNode==NULL)
	{
		try
		{
			//创建节点
			pServerNode=new tagServerNode;
			if (pServerNode==NULL) throw TEXT("内存不足, 房间节点创建失败！");			
		}
		catch (...)
		{
			ASSERT (FALSE);
			return NULL;
		}
	}

	//初始数据
	ZeroMemory(pServerNode,sizeof(tagServerNode));

	return pServerNode;
}

//移除房间
void CBattleManager::RemoveServerNode(tagServerNode * pServerNode)
{
	//加入缓冲	
	m_ServerNodeBuffer.Add(pServerNode);
	ZeroMemory(pServerNode,sizeof(tagServerNode));
}

//移除桌主
void CBattleManager::RemoveTableOwner(DWORD dwBattleNum)
{
	m_StockTableOwner.RemoveStockItem(dwBattleNum);
}

//创建桌主
tagTableOwner * CBattleManager::CreateTableOwner(DWORD dwBattleNum)
{
	return m_StockTableOwner.CreateStockItem(dwBattleNum);
}

//查找桌主
tagTableOwner * CBattleManager::SearchTableOwner(DWORD dwBattleNum)
{
	return m_StockTableOwner.SearchStockItem(dwBattleNum);
}

//查找桌子
tagTableNodeHead * CBattleManager::SearchTableByKindID(WORD wKindID)
{
	//寻找桌子
	tagTableNodeHead TableNodeHead;
	if (m_MapTableKindID.Lookup(wKindID,TableNodeHead)==FALSE)
	{
		return NULL;
	}

	return &m_MapTableKindID[wKindID];
}

//查找桌子
tagTableNodeHead * CBattleManager::SearchTableByUserID(DWORD dwUserID)
{
	//寻找桌子
	tagTableNodeHead TableNodeHead;
	if (m_MapTableUserID.Lookup(dwUserID,TableNodeHead)==FALSE)
	{
		return NULL;
	}

	return &m_MapTableUserID[dwUserID];	
}

//查找桌子
tagTableNodeHead* CBattleManager::SearchTableByClubID(DWORD dwClubID)
{
	//寻找桌子
	tagTableNodeHead TableNodeHead;
	if (m_MapTableClubID.Lookup(dwClubID, TableNodeHead) == FALSE)
	{
		return NULL;
	}

	return &m_MapTableClubID[dwClubID];
}

//添加节点
bool CBattleManager::InsertKindTableNode(WORD wKindID, tagTableNode * pTableNode)
{
	//查找节点
	tagTableNodeHead * pTableUserNodeHead=SearchTableByKindID(wKindID);
	if (pTableUserNodeHead!=NULL) 
	{
		pTableNode->pNextKindNode=pTableUserNodeHead->pTableNodeHead;	
		pTableUserNodeHead->pTableNodeHead=pTableNode;
		pTableUserNodeHead->nNodeCount++;
	}
	else
	{
		//构造结构
		tagTableNodeHead TableUserNodeHead;
		TableUserNodeHead.nNodeCount=1;
		TableUserNodeHead.pTableNodeHead=pTableNode;

		//设置映射
		m_MapTableKindID[wKindID]=TableUserNodeHead;
	}

	return true;
}


//添加节点
bool CBattleManager::InsertUserTableNode(DWORD dwUserID, tagTableNode * pTableNode)
{
	//查找节点
	tagTableNodeHead * pTableUserNodeHead=SearchTableByUserID(dwUserID);
	if (pTableUserNodeHead!=NULL) 
	{
		pTableNode->pNextUserNode=pTableUserNodeHead->pTableNodeHead;	
		pTableUserNodeHead->pTableNodeHead=pTableNode;
		pTableUserNodeHead->nNodeCount++;
	}
	else
	{
		//构造结构
		tagTableNodeHead TableUserNodeHead;
		TableUserNodeHead.nNodeCount=1;
		TableUserNodeHead.pTableNodeHead=pTableNode;

		//设置映射
		m_MapTableUserID[dwUserID]=TableUserNodeHead;
	}

	return true;
}

//添加节点
bool CBattleManager::InsertClubTableNode(DWORD dwClubID, tagTableNode* pTableNode)
{
	//查找节点
	tagTableNodeHead* pTableUserNodeHead = SearchTableByClubID(dwClubID);
	if (pTableUserNodeHead != NULL)
	{
		pTableNode->pNextClubNode = pTableUserNodeHead->pTableNodeHead;
		pTableUserNodeHead->pTableNodeHead = pTableNode;
		pTableUserNodeHead->nNodeCount++;
	}
	else
	{
		//构造结构
		tagTableNodeHead TableUserNodeHead;
		TableUserNodeHead.nNodeCount = 1;
		TableUserNodeHead.pTableNodeHead = pTableNode;

		//设置映射
		m_MapTableClubID[dwClubID] = TableUserNodeHead;
	}

	return true;
}

//移除节点
bool CBattleManager::RemoveKindTableNode(WORD wKindID, tagTableNode * pTableNode)
{
	//查找节点
	tagTableNodeHead * pTableUserNodeHead=SearchTableByKindID(wKindID);
	if (pTableUserNodeHead!=NULL)
	{
		//变量定义
		tagTableNode * pTableNodeHead=pTableUserNodeHead->pTableNodeHead;

		//整理链表
		if (pTableNodeHead==pTableNode)
		{
			if (pTableNodeHead->pNextKindNode==NULL)
			{
				m_MapTableKindID.RemoveKey(wKindID);					
			}
			else
			{
				//设置节点
				pTableUserNodeHead->nNodeCount--;
				pTableUserNodeHead->pTableNodeHead=pTableNodeHead->pNextKindNode;				
			}
		}
		else
		{	
			//移除节点
			tagTableNode * pLastTableNode=pTableNodeHead;
			do
			{
				if (pLastTableNode->pNextKindNode==pTableNode)
				{		
					pTableUserNodeHead->nNodeCount--;
					pLastTableNode->pNextKindNode=pTableNode->pNextKindNode;
					break;										
				}
				pLastTableNode=pLastTableNode->pNextKindNode;
			}
			while (pLastTableNode!=NULL);
		}
	}

	return true;
}

//移除节点
bool CBattleManager::RemoveUserTableNode(DWORD dwUserID, tagTableNode * pTableNode)
{
	//查找节点
	tagTableNodeHead * pTableUserNodeHead=SearchTableByUserID(dwUserID);
	if (pTableUserNodeHead!=NULL)
	{
		//变量定义
		tagTableNode * pTableNodeHead=pTableUserNodeHead->pTableNodeHead;

		//整理链表
		if (pTableNodeHead==pTableNode)
		{
			if (pTableNodeHead->pNextUserNode==NULL)
			{
				m_MapTableUserID.RemoveKey(dwUserID);					
			}
			else
			{
				//设置节点
				pTableUserNodeHead->nNodeCount--;
				pTableUserNodeHead->pTableNodeHead=pTableNodeHead->pNextUserNode;				
			}
		}
		else
		{	
			//移除节点
			tagTableNode * pLastTableNode=pTableNodeHead;
			do
			{
				if (pLastTableNode->pNextUserNode==pTableNode)
				{		
					pTableUserNodeHead->nNodeCount--;
					pLastTableNode->pNextUserNode=pTableNode->pNextUserNode;
					break;										
				}
				pLastTableNode=pLastTableNode->pNextUserNode;
			}
			while (pLastTableNode!=NULL);
		}
	}

	return true;
}


//移除节点
bool CBattleManager::RemoveClubTableNode(DWORD dwClubID, tagTableNode* pTableNode)
{
	//查找节点
	tagTableNodeHead* pTableUserNodeHead = SearchTableByClubID(dwClubID);
	if (pTableUserNodeHead != NULL)
	{
		//变量定义
		tagTableNode* pTableNodeHead = pTableUserNodeHead->pTableNodeHead;

		//整理链表
		if (pTableNodeHead == pTableNode)
		{
			if (pTableNodeHead->pNextClubNode == NULL)
			{
				m_MapTableClubID.RemoveKey(dwClubID);
			}
			else
			{
				//设置节点
				pTableUserNodeHead->nNodeCount--;
				pTableUserNodeHead->pTableNodeHead = pTableNodeHead->pNextClubNode;
			}
		}
		else
		{
			//移除节点
			tagTableNode* pLastTableNode = pTableNodeHead;
			do
			{
				if (pLastTableNode->pNextClubNode == pTableNode)
				{
					pTableUserNodeHead->nNodeCount--;
					pLastTableNode->pNextClubNode = pTableNode->pNextClubNode;
					break;
				}
				pLastTableNode = pLastTableNode->pNextClubNode;
			} while (pLastTableNode != NULL);
		}
	}

	return true;
}

//查找房间
tagServerNode * CBattleManager::SearchServerNode(WORD wKindID)
{
	//查找房间
	tagServerNode * pServerNodeHead=NULL;
	m_MapServerNode.Lookup(wKindID,pServerNodeHead);

	return pServerNodeHead;
}

//查找房间
tagServerNode * CBattleManager::SearchServerNode(WORD wKindID,WORD wServerID)
{
	//查找房间	
	tagServerNode * pServerNodeMove=SearchServerNode(wKindID);
	while (pServerNodeMove!=NULL)
	{
		if (pServerNodeMove->ServerItem.wServerID==wServerID)
		{
			break;
		}
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}

	return pServerNodeMove;
}

//生成编号
DWORD CBattleManager::BuildNumber(int nNumberLen)
{
	//变量定义
	DWORD dwMultiple[]={1,10,100,1000,10000,100000,1000000,1000000,10000000,1000000000};

	//变量定义
	DWORD dwBattleNum=0;
	int nLen=__min(nNumberLen,CountArray(dwMultiple));

	//生成编号
	for (int i=nLen-1; i>=0; i--)
	{
		dwBattleNum+=(i==(nLen-1)?(rand()%9+1):(rand()%10))*dwMultiple[i];
	}

	return dwBattleNum;
}

//生成密码
void CBattleManager::BuildPassword(TCHAR szPassword[LEN_PASSWORD],WORD wBufferCount)
{
	//变量定义
	LPCTSTR pszWords=TEXT("123456789abcdefghijklmnopqrstuvwxyz");

	//密码长度
	int nWordsLen=lstrlen(pszWords);
	int nPasswordLen=__min(MIN_PASSWORD_LEN+rand()%(LEN_PASSWORD-MIN_PASSWORD_LEN),wBufferCount);

	//随机密码
	for (int i=0; i<nPasswordLen; i++)
	{
		szPassword[i]=pszWords[rand()%nWordsLen];
	}
}

///////////////////////////////////////////////////////////////////////////////////