#include "StdAfx.h"
#include "DistributeManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////

//常量定义
#define DISTRIBUTE_WAIT_TIME			5							//等待时长

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CDistributeManager::CDistributeManager()
{
	//设置变量
	m_wMinPlayerCount=0;
	m_cbDistributeRule=0;
}

//析构函数
CDistributeManager::~CDistributeManager()
{
	//移除元素
	RemoveAll();

	//释放缓冲
	for (INT_PTR i=0; i<m_SameTableBuffer.GetCount(); i++)
	{
		SafeDelete(m_SameTableBuffer[i]);
	}

	//释放缓冲
	for (INT_PTR i=0; i<m_DistributeNodeBuffer.GetCount(); i++)
	{
		SafeDelete(m_DistributeNodeBuffer[i]);
	}	

	//释放元素
	m_SameTableBuffer.RemoveAll();
	m_DistributeNodeBuffer.RemoveAll();
}

//插入结点
bool CDistributeManager::InsertDistributeNode(IServerUserItem * pIServerUserItem)
{
	//参数校验
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//查找用户
	if (SearchNode(pIServerUserItem)!=NULL) return false;

	//分配结点
	tagDistributeNode * pDistributeNode=ActiveDistributeNode();
	if (pDistributeNode==NULL) return false;

	//设置节点
	pDistributeNode->DistributeInfo.dwInsertTime=(DWORD)time(NULL);
	pDistributeNode->DistributeInfo.pIServerUserItem=pIServerUserItem;

	//添加节点
	if (pIServerUserItem->IsRobotUser()==false)
	{
		m_PlayerNodeActive.Add(pDistributeNode);		
	}
	else
	{
		m_RobotNodeActive.Add(pDistributeNode);		
	}

	return true;
}

//移除结点
void CDistributeManager::RemoveDistributeNode(IServerUserItem * pIServerUserItem)
{
	//查找结点
	tagDistributeNode * pDistributeNode=SearchNode(pIServerUserItem);
	if (pDistributeNode!=NULL) RemoveDistributeNode(pDistributeNode);
}

//移除结点
VOID CDistributeManager::RemoveDistributeNode(tagDistributeNode * pDistributeNode)
{
	//加入缓冲
	m_DistributeNodeBuffer.Add(pDistributeNode);

	//机器节点
	if (pDistributeNode->DistributeInfo.pIServerUserItem->IsRobotUser())
	{
		for (INT_PTR i=0; i<m_RobotNodeActive.GetCount(); i++)
		{
			if (m_RobotNodeActive[i]==pDistributeNode)
			{
				m_RobotNodeActive.RemoveAt(i);
				break;
			}
		}
	}
	else
	{
		for (INT_PTR i=0; i<m_PlayerNodeActive.GetCount(); i++)
		{
			if (m_PlayerNodeActive[i]==pDistributeNode)
			{
				m_PlayerNodeActive.RemoveAt(i);
				break;
			}
		}
	}
}

//获取信息
tagSameTableInfo * CDistributeManager::GetUserSameTableInfo()
{
	//规则判断
	if ((m_cbDistributeRule&DISTRIBUTE_SAME_TABLE)!=0)
	{
		return NULL;
	}

	//创建节点
	tagSameTableNode * pSameTableNode=ActiveSameTableNode();
	if (pSameTableNode!=NULL)
	{
		return &pSameTableNode->SameTableInfo;
	}

	return NULL;
}

//获取信息
tagSameTableInfo * CDistributeManager::GetUserSameTableInfo(DWORD dwUserID)
{
	//变量定义
	tagSameTableNode * pSameTableNode;
	if (m_SameTableMap.Lookup(dwUserID,pSameTableNode))
	{
		return &pSameTableNode->SameTableInfo;
	}

	return NULL;
}

//获取信息
VOID CDistributeManager::SetUserSameTableInfo(DWORD dwUserID,const tagSameTableInfo * pSameTableInfo)
{
	//变量定义
	tagSameTableNode * pSameTableNode=NULL;
	if (m_SameTableMap.Lookup(dwUserID,pSameTableNode))
	{
		//移除同桌
		RemoveSameTableInfo(dwUserID);
	}

	//获取节点
	pSameTableNode=CONTAINING_RECORD(pSameTableInfo,tagSameTableNode,SameTableInfo);

	//设置同桌
	pSameTableNode->cbReferCount++;
	m_SameTableMap[dwUserID]=pSameTableNode;
}

//移除信息
VOID CDistributeManager::RemoveSameTableInfo(DWORD dwUserID)
{
	//变量定义
	tagSameTableNode * pSameTableNode=NULL;
	if (m_SameTableMap.Lookup(dwUserID,pSameTableNode)==TRUE)
	{
		//移除信息
		m_SameTableMap.RemoveKey(dwUserID);

		//设置变量
		if (--pSameTableNode->cbReferCount==0)
		{			
			RemoveSameTableNode(pSameTableNode);		
		}		
	}
}		

//激活对象
tagSameTableNode * CDistributeManager::ActiveSameTableNode()
{
	//变量定义
	tagSameTableNode * pSameTableNode=NULL;

	//查找缓冲
	if (m_SameTableBuffer.GetCount()>0)
	{
		pSameTableNode=m_SameTableBuffer.GetAt(0);
		m_SameTableBuffer.RemoveAt(0);
		m_SameTableActive.Add(pSameTableNode);
	}

	//创建对象
	if (pSameTableNode==NULL)
	{
		try
		{
			//创建对象
			pSameTableNode = new tagSameTableNode;
			if(pSameTableNode==NULL) throw(TEXT("内存不足,对象创建失败!"));

			//设置对象
			m_SameTableActive.Add(pSameTableNode);			
			ZeroMemory(pSameTableNode,sizeof(tagSameTableNode));			
		}
		catch(...)
		{
			ASSERT(FALSE);
		}
	}

	return pSameTableNode;
}

//移除对象
VOID CDistributeManager::RemoveSameTableNode(tagSameTableNode * pSameTableNode) 
{
	//清除数据
	m_SameTableBuffer.Add(pSameTableNode);
	ZeroMemory(pSameTableNode,sizeof(tagSameTableNode));	

	//查找对象
	for (INT_PTR i=0; i<m_SameTableActive.GetCount(); i++)
	{
		if (m_SameTableActive[i]==pSameTableNode)
		{
			m_SameTableActive.RemoveAt(i);
			break;
		}
	}
}

//激活节点
tagDistributeNode * CDistributeManager::ActiveDistributeNode()
{
	//变量定义
	tagDistributeNode * pDistributeNode=NULL;

	//查找缓存
	if (m_DistributeNodeBuffer.GetCount()>0)
	{
		pDistributeNode=m_DistributeNodeBuffer[0];
		m_DistributeNodeBuffer.RemoveAt(0);
	}

	//创建节点
	if (pDistributeNode==NULL)
	{
		try
		{
			//分配内存
			pDistributeNode=new tagDistributeNode();
			if (pDistributeNode==NULL) throw TEXT("节点创建失败，内存不足!");

			//初始内存
			ZeroMemory(pDistributeNode,sizeof(tagDistributeNode));
		}
		catch (...)
		{
			ASSERT (FALSE);
		}
	}

	return pDistributeNode;
}

//移除结点
VOID CDistributeManager::RemoveAll()
{
	//追加节点
	m_SameTableBuffer.Append(m_SameTableActive);
	m_DistributeNodeBuffer.Append(m_PlayerNodeActive);
	m_DistributeNodeBuffer.Append(m_RobotNodeActive);

	//释放元素
	m_SameTableMap.RemoveAll();
	m_SameTableActive.RemoveAll();
	m_PlayerNodeActive.RemoveAll();
	m_RobotNodeActive.RemoveAll();	
}

//执行分组
WORD CDistributeManager::PerformDistribute(CDistributeInfoArray & DistributeInfoArray, WORD wNeedCount)
{
	//真人判断
	if (m_PlayerNodeActive.GetCount()==0) return false;

	//变量定义
	CDistributeNodeArray DistributeNodeArray;

	//玩家优先
	if (m_cbDistributeRule&DISTRIBUTE_PLAYER_FIRST && GetPlayerCount()>=m_wMinPlayerCount)
	{
		//构造数组		
		DistributeNodeArray.Append(m_PlayerNodeActive);
	}
	else
	{
		//构造数组
		DistributeNodeArray.Append(m_RobotNodeActive);
		DistributeNodeArray.Append(m_PlayerNodeActive);		
	}

	//人数判断
	if (DistributeNodeArray.GetCount()<wNeedCount) return false;

	//获取时间戳
	DWORD dwCurrentTime=(DWORD)time(NULL);

	//分配用户
	for (INT_PTR i=0; i< DistributeNodeArray.GetCount(); i++)
	{
		//移除所有
		DistributeInfoArray.RemoveAll();
		DistributeInfoArray.Add(&DistributeNodeArray[i]->DistributeInfo);

		//构造数组
		CDistributeNodeArray DistributePartNodeArray;
		DistributePartNodeArray.Append(DistributeNodeArray);
		DistributePartNodeArray.RemoveAt(i);

		//随机分配
		while (DistributePartNodeArray.GetCount()>0)
		{
			//获取节点
			int nRandIndex=rand()%DistributePartNodeArray.GetCount();
			tagDistributeNode * pDistributeNode=DistributePartNodeArray[nRandIndex];

			//移除节点
			DistributePartNodeArray.RemoveAt(nRandIndex);

			//时间过滤
			if ((dwCurrentTime-pDistributeNode->DistributeInfo.dwInsertTime)<DISTRIBUTE_WAIT_TIME)
			{
				continue;
			}

			//机器过滤
			if ((DistributeInfoArray.GetCount()==wNeedCount-1) && 
				FilterRuleIsAllRobot(DistributeInfoArray,pDistributeNode->DistributeInfo.pIServerUserItem))
			{
				continue;
			}

			//相同地址
			if ((m_cbDistributeRule&DISTRIBUTE_SAME_ADDRESS)==0 &&
				FilterRuleExitsClientAddr(DistributeInfoArray,pDistributeNode->DistributeInfo.pIServerUserItem))
			{
				continue;
			}

			//相同机器码
			if ((m_cbDistributeRule&DISTRIBUTE_SAME_MACHINE)==0 &&			
				FilterRuleExitsMachineID(DistributeInfoArray,pDistributeNode->DistributeInfo.pIServerUserItem))
			{
				continue;
			}		

			//上局同桌
			if ((m_cbDistributeRule&DISTRIBUTE_SAME_TABLE)==0 &&
				FilterRuleExitsLastTable(DistributeInfoArray,pDistributeNode->DistributeInfo.pIServerUserItem))
			{
				continue;
			}

			//添加节点
			DistributeInfoArray.Add(&pDistributeNode->DistributeInfo);

			//成功判断
			if (DistributeInfoArray.GetCount()==wNeedCount)
			{
				return true;
			}
		}
	}

	return false;	
}

//查找结点
tagDistributeNode * CDistributeManager::SearchNode(IServerUserItem * pIServerUserItem)
{
	//参数校验
	ASSERT (pIServerUserItem!=NULL);
	if(pIServerUserItem==NULL) return NULL;

	//机器玩家
	if (pIServerUserItem->IsRobotUser())
	{
		for (INT_PTR i=0; i<m_RobotNodeActive.GetCount(); i++)
		{
			if (m_RobotNodeActive[i]->DistributeInfo.pIServerUserItem==pIServerUserItem)
			{
				return m_RobotNodeActive[i];
			}
		}
	}
	else
	{
		for (INT_PTR i=0; i<m_PlayerNodeActive.GetCount(); i++)
		{
			if (m_PlayerNodeActive[i]->DistributeInfo.pIServerUserItem==pIServerUserItem)
			{
				return m_PlayerNodeActive[i];
			}
		}
	}

	return NULL;
}

//机器过滤
bool CDistributeManager::FilterRuleIsAllRobot(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem * const pIServerUserItem)
{
	//参数校验
	if (pIServerUserItem==NULL || DistributeInfoArray.GetCount()==0) return false;

	//变量定义
	WORD wRobotCount=0;

	//统计机器
	for (INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
	{
		if (DistributeInfoArray[nIndex]->pIServerUserItem->IsRobotUser()==true)
		{
			++wRobotCount;
		}
	}

	return (wRobotCount==DistributeInfoArray.GetCount()) && pIServerUserItem->IsRobotUser();
}

//上局同桌
bool CDistributeManager::FilterRuleExitsLastTable(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem * pIServerUserItem)
{
	//参数校验	
	if (DistributeInfoArray.GetCount()==0) return FALSE;

	//变量定义
	tagSameTableInfo * pSameTableInfo=NULL;

	//同桌比较
	for (INT_PTR i=0; i<DistributeInfoArray.GetCount(); i++)
	{
		//获取信息
		pSameTableInfo=GetUserSameTableInfo(DistributeInfoArray[i]->pIServerUserItem->GetUserID());
		if (pSameTableInfo==NULL) continue;

		//比较标识
		for (WORD j=0; j<pSameTableInfo->wPlayerCount; j++)
		{
			if (pSameTableInfo->wPlayerIDSet[j]==pIServerUserItem->GetUserID())
			{
				return TRUE;				
			}
		}
	}
	
	return FALSE;
}

//相同地址
bool CDistributeManager::FilterRuleExitsClientAddr(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem * pIServerUserItem)
{
	//查找同IP
	for (INT_PTR i=0; i<DistributeInfoArray.GetCount(); i++)
	{
		if (DistributeInfoArray[i]->pIServerUserItem->GetClientIP()==pIServerUserItem->GetClientIP())
		{
			return true;
		}
	}

	return false;
}

//相同机器码
bool CDistributeManager::FilterRuleExitsMachineID(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem * pIServerUserItem)
{
	//查找同IP
	for (INT_PTR i=0; i<DistributeInfoArray.GetCount(); i++)
	{
		if (lstrcmp(DistributeInfoArray[i]->pIServerUserItem->GetMachineID(),pIServerUserItem->GetMachineID()))
		{
			return true;
		}
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////////////
