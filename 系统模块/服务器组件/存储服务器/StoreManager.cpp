#include "stdafx.h"
#include "StoreManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////////

//构造函数
CServerManager::CServerManager()
{
	//设置索引
	m_MapServerNode.InitHashTable(103);
}

//析构函数
CServerManager::~CServerManager()
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
}

//房间数量
WORD CServerManager::GetServerCount(WORD wKindID)
{
	//寻找房间
	tagServerNode * pServerNodeHead=NULL;
	if (m_MapServerNode.Lookup(wKindID,pServerNodeHead)==FALSE)
	{
		return 0;
	}

	//变量定义
	WORD wServerCount=0;
	tagServerNode * pServerNodeMove=pServerNodeHead;

	//查找房间
	while (pServerNodeMove!=NULL)
	{	
		//统计数量
		wServerCount++;

		//移动节点
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}

	return wServerCount;
}

//移除房间
bool CServerManager::RemoveServer(WORD wKindID,WORD wServerID)
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
			pServerNodeMove->ServerItem.bRemoved=true;
			break;
		}

		//移动节点
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}		

	return true;
}

//添加房间
bool CServerManager::AppendServer(tagServerItem & ServerItem)
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
				if (pServerNodeNext->ServerItem.bRemoved==true)
				{					
					pServerNodeNext->ServerItem.bRemoved=false;					
					pServerNodeNext->ServerItem.wTableUseCount=0;
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

	//拷贝信息
	CopyMemory(&pServerNodeNew->ServerItem,&ServerItem,sizeof(pServerNodeNew->ServerItem));

	//设置状态
	pServerNodeNew->ServerItem.bRemoved=false;
	pServerNodeNew->ServerItem.wTableUseCount=0;
  
	//链接节点
	pServerNodeNew->pNextServerNode=pServerNodeHead;
	m_MapServerNode[ServerItem.wKindID]=pServerNodeNew;
	
	return true;
}

//枚举房间
tagServerItem * CServerManager::EnumServer(WORD wKindID,WORD wServerID)
{
	//寻找房间
	tagServerNode * pServerNodeHead=NULL;
	if (m_MapServerNode.Lookup(wKindID,pServerNodeHead)==FALSE)
	{
		return NULL;
	}

	//变量定义
	tagServerNode * pServerNodeMove=pServerNodeHead;

	//查找房间
	while (wServerID!=0 && pServerNodeMove!=NULL)
	{	
		//判断标识
		if (pServerNodeMove->ServerItem.bRemoved==false && pServerNodeMove->ServerItem.wServerID==wServerID)
		{		
			//移动节点
			pServerNodeMove=pServerNodeMove->pNextServerNode;
			break;
		}

		//移动节点
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}

	//查找房间
	while (pServerNodeMove!=NULL)
	{	
		//判断标识
		if (pServerNodeMove->ServerItem.bRemoved==false)
		{		
			return &pServerNodeMove->ServerItem;
		}

		//移动节点
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}

	return NULL;
}

//查找房间
tagServerItem * CServerManager::SearchServer(WORD wKindID,WORD wServerID)
{
	//寻找房间
	tagServerNode * pServerNodeHead=NULL;
	if (m_MapServerNode.Lookup(wKindID,pServerNodeHead)==FALSE)
	{
		return NULL;
	}

	//变量定义
	tagServerNode * pServerNodeMove=pServerNodeHead;

	//查找房间
	while (wServerID!=0 && pServerNodeMove!=NULL)
	{	
		//判断标识
		if (pServerNodeMove->ServerItem.wServerID==wServerID)
		{						
			break;
		}

		//移动节点
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}

	return &pServerNodeMove->ServerItem;
}

//获取房间
tagServerItem * CServerManager::GetServerItem(WORD wKindID,WORD wServerID,WORD wTableCount,BYTE cbStoreType)
{
	//变量定义
	tagServerItem * pServerItem=NULL;

	do
	{
		//枚举房间
		pServerItem=EnumServer(wKindID,wServerID);
		if (pServerItem==NULL) break;

		//校验桌数
		if (pServerItem->cbStoreType==cbStoreType && pServerItem->wTableCount-pServerItem->wTableUseCount>=wTableCount)
		{
			return pServerItem;
		}

		//设置标识
		wServerID=pServerItem->wServerID;

	} while (true);

	return NULL;
}

//激活房间
tagServerNode * CServerManager::ActiveServerNode()
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
void CServerManager::RemoveServerNode(tagServerNode * pServerNode)
{
	//加入缓冲	
	m_ServerNodeBuffer.Add(pServerNode);
	ZeroMemory(pServerNode,sizeof(tagServerNode));
}

//重置数据
void CServerManager::ResetData()
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
}

///////////////////////////////////////////////////////////////////////////////////