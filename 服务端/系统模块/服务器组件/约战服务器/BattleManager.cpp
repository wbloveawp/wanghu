#include "stdafx.h"
#include "BattleManager.h"

///////////////////////////////////////////////////////////////////////////////////
//��������
#define MAPPED_NUM_LEN						6							//��ų���
#define MIN_PASSWORD_LEN					6							//���볤��

//�ڵ�����
#define NODE_TYPE_USER						0							//�û��ڵ�
#define NODE_TYPE_KIND						1							//���ͽڵ�
#define NODE_TYPE_CLUB						2							//����ڵ�

///////////////////////////////////////////////////////////////////////////////////

//���캯��
CBattleManager::CBattleManager()
{
	//��������
	m_MapServerNode.InitHashTable(103);
	m_MapTableUserID.InitHashTable(PRIME_PLATFORM_USER);	
	m_MapTableKindID.InitHashTable(PRIME_PLATFORM_USER);

	//��ʼ��
	m_StockTableNode.InitHashTable(PRIME_PLATFORM_USER);
	m_StockTableOwner.InitHashTable(PRIME_PLATFORM_USER);	
}

//��������
CBattleManager::~CBattleManager()
{
	//��������
	ResetData();

	//ɾ������
	for (INT_PTR i=0; i<m_ServerNodeBuffer.GetCount(); i++)
	{
		SafeDelete(m_ServerNodeBuffer[i]);
	}

	//�Ƴ�Ԫ��
	m_ServerNodeBuffer.RemoveAll();	
	m_StockTableNode.DeleteAllItem();
	m_StockTableOwner.DeleteAllItem();	
}

//�Ƴ�����
bool CBattleManager::RemoveServer(WORD wKindID,WORD wServerID)
{
	//Ѱ�ҷ���
	tagServerNode * pServerNodeHead=NULL;
	if (m_MapServerNode.Lookup(wKindID,pServerNodeHead)==FALSE)
	{
		return false;
	}

	//��������
	tagServerNode * pServerNodeMove=pServerNodeHead;

	//���ҷ���
	while (pServerNodeMove!=NULL)
	{	
		//�жϱ�ʶ
		if (pServerNodeMove->ServerItem.wServerID==wServerID)
		{		
			//����״̬
			pServerNodeMove->ServerStatus.bRemoved=true;
			break;
		}

		//�ƶ��ڵ�
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}		

	return true;
}

//��ӷ���
bool CBattleManager::AppendServer(const tagServerItem & ServerItem)
{
	//Ѱ�ҷ���
	tagServerNode * pServerNodeHead=NULL;
	m_MapServerNode.Lookup(ServerItem.wKindID,pServerNodeHead);

	//���ҽڵ�	
	if (pServerNodeHead!=NULL)
	{
    //��������
		tagServerNode * pServerNodeLast=NULL;
		tagServerNode * pServerNodeNext=pServerNodeHead;

		//���ҷ���
		do
		{	
			//�жϱ�ʶ
			if (pServerNodeNext->ServerItem.wServerID==ServerItem.wServerID)
			{		
				//����״̬
				if (pServerNodeNext->ServerStatus.bRemoved==true)
				{					
					pServerNodeNext->ServerStatus.bRemoved=false;					
					pServerNodeNext->ServerItem.wTableCount=ServerItem.wTableCount;
				}

				//���ӷ���
				if (pServerNodeLast!=NULL)
				{
					pServerNodeLast->pNextServerNode=pServerNodeNext->pNextServerNode;
					pServerNodeNext->pNextServerNode=pServerNodeHead;

					//���ñ�ͷ
					m_MapServerNode[ServerItem.wKindID]=pServerNodeNext;
				}

				return false;
			}

			//�ƶ��ڵ�
			pServerNodeLast=pServerNodeNext;
			pServerNodeNext=pServerNodeNext->pNextServerNode;

		} while (pServerNodeNext!=NULL);		
	}	

	//����ڵ�
	tagServerNode * pServerNodeNew=ActiveServerNode();
	if (pServerNodeNew==NULL) return true;

	//����״̬
	pServerNodeNew->ServerStatus.bRemoved=false;
	pServerNodeNew->ServerStatus.wTableUseCount=0;

	//������Ϣ
	CopyMemory(&pServerNodeNew->ServerItem,&ServerItem,sizeof(pServerNodeNew->ServerItem));
  
	//���ӽڵ�
	pServerNodeNew->pNextServerNode=pServerNodeHead;
	m_MapServerNode[ServerItem.wKindID]=pServerNodeNew;
	
	return true;
}

//�Ƴ�����
bool CBattleManager::RemoveTable(DWORD dwBattleNum)
{
	//Ѱ������
	tagTableNode* pTableNode=m_StockTableNode.SearchStockItem(dwBattleNum);
	if (pTableNode == NULL) return true;

	//��������
	tagBattleTableItemEx * pTableItem=&pTableNode->TableItem;

	//�Ƴ��ڵ�
	RemoveUserTableNode(pTableItem->dwOwnerID,pTableNode);

	//�Ƴ��ڵ�
	if (pTableItem->dwClubID) RemoveClubTableNode(pTableItem->dwClubID, pTableNode);

	//���ҷ���			
	tagServerNode * pServerNodeMove=SearchServerNode(pTableItem->BattleConfig.wKindID,pTableItem->wServerID);
	if (pServerNodeMove!=NULL) pServerNodeMove->ServerStatus.wTableUseCount--;

	//�Ƴ��ڵ�
	m_StockTableNode.RemoveStockItem(dwBattleNum);

	return true;
}

//��������
tagBattleTableItemEx * CBattleManager::CreateTable(DWORD dwUserID, DWORD dwClubID, tagBattleConfigEx & BattleConfig,WORD wServerID)
{
	//��������
	tagServerNode *	pServerNodeFind=NULL;	
	tagServerNode * pServerNodeMove=SearchServerNode(BattleConfig.wKindID);	

	//���ҷ���
	while (pServerNodeMove!=NULL && wServerID!=0)
	{
		if (pServerNodeMove->ServerItem.wServerID==wServerID)
		{
			pServerNodeMove=pServerNodeMove->pNextServerNode;
			break;
		}
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}

	//���ҷ���
	while (pServerNodeMove!=NULL)
	{
		//�ж�״̬
		if (pServerNodeMove->ServerStatus.bRemoved==false)
		{				
			//��������
			if (pServerNodeMove->ServerStatus.wTableUseCount<pServerNodeMove->ServerItem.wTableCount/10*9)
			{
				pServerNodeFind=pServerNodeMove;
				break;
			}			
		}
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}

	//�����þ�
	if (pServerNodeFind==NULL) return NULL;

	//��������
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

	//ѭ���ж�
	if (nLoopCount <= 0) return NULL;

	//�����ڵ�
	auto pTableNodeNew=m_StockTableNode.CreateStockItem(dwBattleNum);
	if (pTableNodeNew == NULL) return NULL;

	//��ʼ��
	ZeroMemory(pTableNodeNew,sizeof(tagTableNode));

	//��������	
	pTableNodeNew->TableItem.dwOwnerID=dwUserID;
	pTableNodeNew->TableItem.wTableID=INVALID_TABLE;	
	pTableNodeNew->TableItem.dwClubID = dwClubID;
	pTableNodeNew->TableItem.dwBattleNum=dwBattleNum;
	pTableNodeNew->TableItem.dwCreateTime = time(NULL);
	pTableNodeNew->TableItem.wServerID=pServerNodeFind->ServerItem.wServerID;
	CopyMemory(&pTableNodeNew->TableItem.BattleConfig,&BattleConfig,sizeof(pTableNodeNew->TableItem.BattleConfig));	

	//����״̬
	pServerNodeFind->ServerStatus.wTableUseCount++;

	//��ӽڵ�
	InsertUserTableNode(dwUserID,pTableNodeNew);

	//����ڵ�
	if (dwClubID != 0) InsertClubTableNode(dwClubID, pTableNodeNew);

	return &pTableNodeNew->TableItem;
}

//��������
VOID CBattleManager::ResetData()
{
	//�Ƴ�����
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

	//�Ƴ�Ԫ��		
	m_MapServerNode.RemoveAll();
	m_MapTableKindID.RemoveAll();	
	m_MapTableUserID.RemoveAll();	
	m_MapTableClubID.RemoveAll();
	m_StockTableNode.RemoveAllItem();
	m_StockTableOwner.RemoveAllItem();
}

//��������
WORD CBattleManager::GetTableCountByKindID(DWORD dwUserID,WORD wKindID, DWORD dwClubID)
{
	//���ҽڵ�
	tagTableNodeHead * pTableNodeHead=SearchTableByUserID(dwUserID);
	if (pTableNodeHead==NULL) return 0;
	
	//���з���
	//if (wKindID==0) return pTableNodeHead->nNodeCount;

	//��������
	WORD wTableCount=0;
	tagTableNode * pTableNodeMove=pTableNodeHead->pTableNodeHead;

	//�����ڵ�
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

//ö����Ϸ
WORD CBattleManager::EnumTableKindID(POSITION& Position)
{
	WORD wKindID = 0L;
	tagTableNodeHead TableNodeHead;

	//��ȡ����
	if (Position == NULL) Position = m_MapTableKindID.GetStartPosition();
	if (Position != NULL) m_MapTableKindID.GetNextAssoc(Position, wKindID, TableNodeHead);

	return wKindID;
}

//ö���û�
DWORD CBattleManager::EnumTableUserID(POSITION& Position)
{
	DWORD dwUserID = 0L;
	tagTableNodeHead TableNodeHead;

	//��ȡ����
	if (Position == NULL) Position = m_MapTableUserID.GetStartPosition();
	if (Position != NULL) m_MapTableUserID.GetNextAssoc(Position, dwUserID, TableNodeHead);

	return dwUserID;
}

//ö�ٲ���
DWORD CBattleManager::EnumTableClubID(POSITION& Position)
{
	DWORD dwClubID = 0L;
	tagTableNodeHead TableNodeHead;

	//��ȡ����
	if (Position == NULL) Position = m_MapTableClubID.GetStartPosition();
	if (Position != NULL) m_MapTableClubID.GetNextAssoc(Position, dwClubID, TableNodeHead);

	return dwClubID;
}

//��������
WORD CBattleManager::GetTableCountByClubID(DWORD dwClubID)
{
	//���ҽڵ�
	tagTableNodeHead* pTableNodeHead = SearchTableByClubID(dwClubID);
	if (pTableNodeHead == NULL)  return 0;

	return pTableNodeHead->nNodeCount;
}

//��������
WORD CBattleManager::GetTableCountByClubID(DWORD dwUserID, DWORD dwClubID)
{
	//���ҽڵ�
	tagTableNodeHead* pTableNodeHead = SearchTableByUserID(dwUserID);
	if (pTableNodeHead == NULL)  return 0;

	//��������
	WORD wTableCount = 0;
	tagTableNode* pTableNodeMove = pTableNodeHead->pTableNodeHead;

	//�����ڵ�
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

//��������
tagBattleTableItemEx * CBattleManager::GetTableItem(DWORD dwBattleNum)
{
	//Ѱ������
	auto pTableNode= m_StockTableNode.SearchStockItem(dwBattleNum);
	
	return pTableNode?&pTableNode->TableItem:NULL;
}

//����״̬
tagBattleTableParamEx * CBattleManager::GetTableParam(DWORD dwBattleNum)
{
	//Ѱ������
	auto pTableNode = m_StockTableNode.SearchStockItem(dwBattleNum);

	return pTableNode?&pTableNode->TableParam:NULL;
}

//�������
bool CBattleManager::AddTableItem(const tagBattleTableItemEx & TableItem)
{
	//Ѱ������
	tagTableNode * pTableNode= m_StockTableNode.SearchStockItem(TableItem.dwBattleNum);
	if (pTableNode != NULL) return true;

	//���ҷ���	
	tagServerNode * pServerNodeMove=SearchServerNode(TableItem.BattleConfig.wKindID,TableItem.wServerID);
	if (pServerNodeMove==NULL) return false;

	//�����ڵ�
	tagTableNode* pTableNodeNew=m_StockTableNode.CreateStockItem(TableItem.dwBattleNum);
	if (pTableNodeNew == NULL) return false;

	//��ʼ��
	ZeroMemory(pTableNodeNew, sizeof(tagTableNode));

	//��������	
	CopyMemory(&pTableNodeNew->TableItem,&TableItem,sizeof(pTableNodeNew->TableItem));

	//����״̬
	pServerNodeMove->ServerStatus.wTableUseCount++;

	//���ӽڵ�
	InsertUserTableNode(TableItem.dwOwnerID,pTableNodeNew);

	//���ӽڵ�
	if (TableItem.dwClubID != 0) InsertClubTableNode(TableItem.dwClubID, pTableNodeNew);

	return true;
}

//�����б�
bool CBattleManager::GetRandTableList(WORD wTableCount, CTableItemArray& TableItemArray, CTableParamArray& TableParamArray)
{
	//У������
	auto nItemCount = m_StockTableNode.GetItemCount();
	if (nItemCount == 0) return true;

	//��������
	INT_PTR nIndex = 0;

	//У������
	if (nItemCount <= wTableCount)
	{
		//���ô�С
		TableItemArray.SetSize(nItemCount);
		TableParamArray.SetSize(nItemCount);

		//�������
		tagTableNode* pTableNodeMove = m_StockTableNode.GetHeadStockItem();
		while (pTableNodeMove != NULL)
		{
			auto pTableNodeCurrent = pTableNodeMove;
			pTableNodeMove = m_StockTableNode.GetNextStockItem(pTableNodeCurrent);

			//��������
			if (pTableNodeCurrent->TableItem.BattleConfig.cbAllowPublic)
			{
				TableItemArray.SetAtGrow(nIndex, &pTableNodeCurrent->TableItem);
				TableParamArray.SetAtGrow(nIndex, &pTableNodeCurrent->TableParam);
				nIndex++;
			}			
		}

		//������С
		if (nIndex < nItemCount)
		{
			TableItemArray.SetSize(nIndex);
			TableParamArray.SetSize(nIndex);
		}
	}
	else
	{
		//���ô�С
		TableItemArray.SetSize(wTableCount);
		TableParamArray.SetSize(wTableCount);

		//��������
		INT nLoopIndex = 0;
		INT nRandIndex = rand() % nItemCount;

		//�������
		tagTableNode* pTableNodeMove = m_StockTableNode.GetHeadStockItem();
		while (pTableNodeMove != NULL)
		{
			auto pTableNodeCurrent = pTableNodeMove;
			pTableNodeMove = m_StockTableNode.GetNextStockItem(pTableNodeCurrent);

			if (nLoopIndex++>= nRandIndex && pTableNodeCurrent->TableItem.BattleConfig.cbAllowPublic)
			{
				//��������
				TableItemArray.SetAtGrow(nIndex, &pTableNodeCurrent->TableItem);
				TableParamArray.SetAtGrow(nIndex, &pTableNodeCurrent->TableParam);

				//�˳��ж�
				if (++nIndex >= wTableCount) break;
			}

			//�˳��ж�
			if (nLoopIndex == nRandIndex+nItemCount) break;

			//βͷ�ν�
			if (pTableNodeMove == NULL)
			{				
				pTableNodeMove = m_StockTableNode.GetHeadStockItem();
			}
		}

		//������С
		if (nIndex < wTableCount)
		{
			TableItemArray.SetSize(nIndex);
			TableParamArray.SetSize(nIndex);
		}
	}

	return true;
}

//�����б�
bool CBattleManager::GetKindTableList(WORD wKindID,CTableItemArray & TableItemArray,CTableParamArray & TableParamArray)
{
	//��������
	tagTableNodeHead * pTableNodeHead=SearchTableByKindID(wKindID);
	if (pTableNodeHead==NULL) return true;

	//��������
	tagTableNode * pTableNodeMove=pTableNodeHead->pTableNodeHead;

	//�����ڵ�
	while (pTableNodeMove!=NULL)
	{
		TableItemArray.Add(&pTableNodeMove->TableItem);
		TableParamArray.Add(&pTableNodeMove->TableParam);
		pTableNodeMove=pTableNodeMove->pNextKindNode;
	}

	return true;
}

//�����б�
bool CBattleManager::GetUserTableList(DWORD dwUserID,CTableItemArray & TableItemArray,CTableParamArray & TableParamArray)
{
	//��������
	tagTableNodeHead * pTableNodeHead=SearchTableByUserID(dwUserID);
	if (pTableNodeHead==NULL) return false;

	//��������
	tagTableNode * pTableNodeMove=pTableNodeHead->pTableNodeHead;

	//�����ڵ�
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

//�����б�
bool CBattleManager::GetClubTableList(DWORD dwClubID, CTableItemArray& TableItemArray, CTableParamArray& TableParamArray)
{
	//��������
	tagTableNodeHead* pTableNodeHead = SearchTableByClubID(dwClubID);
	if (pTableNodeHead == NULL) return false;

	//��������
	tagTableNode* pTableNodeMove = pTableNodeHead->pTableNodeHead;

	//�����ڵ�
	while (pTableNodeMove != NULL)
	{
		TableItemArray.Add(&pTableNodeMove->TableItem);
		TableParamArray.Add(&pTableNodeMove->TableParam);
		pTableNodeMove = pTableNodeMove->pNextClubNode;
	}

	return true;
}

//�޸�����
bool CBattleManager::ModifyTableOwner(DWORD dwBattleNum, DWORD dwNewOwnerID)
{
	//��������
	auto pTableNode = m_StockTableNode.SearchStockItem(dwBattleNum);
	if (pTableNode == NULL) return false;

	//�Ƴ�����
	RemoveUserTableNode(pTableNode->TableItem.dwOwnerID, pTableNode);

	//��������
	pTableNode->TableItem.dwOwnerID = dwNewOwnerID;

	//�������
	InsertUserTableNode(pTableNode->TableItem.dwOwnerID, pTableNode);

	return true;
}

//ö������
tagBattleTableItemEx * CBattleManager::EnumTableItem(tagBattleTableItemEx* pTableItem)
{
	//��������
	tagTableNode * pTableNode=NULL;

	//��ȡ�ڵ�
	if (pTableItem == NULL)
	{
		pTableNode= m_StockTableNode.GetHeadStockItem();
	}
	else
	{
		//��ȡ�ڵ�
		pTableNode = m_StockTableNode.GetNextStockItem(CONTAINING_RECORD(pTableItem, tagTableNode, TableItem));
	}

	return (pTableNode==NULL)?NULL:&pTableNode->TableItem;		
}

//�����
tagServerNode * CBattleManager::ActiveServerNode()
{
	//��������
	tagServerNode * pServerNode=NULL;

	//���һ���
	if (m_ServerNodeBuffer.GetCount()>0)
	{
		INT_PTR nIndex=m_ServerNodeBuffer.GetCount()-1;
		pServerNode=m_ServerNodeBuffer[nIndex];
		m_ServerNodeBuffer.RemoveAt(nIndex);		
	}

	//ָ���ж�
	if (pServerNode==NULL)
	{
		try
		{
			//�����ڵ�
			pServerNode=new tagServerNode;
			if (pServerNode==NULL) throw TEXT("�ڴ治��, ����ڵ㴴��ʧ�ܣ�");			
		}
		catch (...)
		{
			ASSERT (FALSE);
			return NULL;
		}
	}

	//��ʼ����
	ZeroMemory(pServerNode,sizeof(tagServerNode));

	return pServerNode;
}

//�Ƴ�����
void CBattleManager::RemoveServerNode(tagServerNode * pServerNode)
{
	//���뻺��	
	m_ServerNodeBuffer.Add(pServerNode);
	ZeroMemory(pServerNode,sizeof(tagServerNode));
}

//�Ƴ�����
void CBattleManager::RemoveTableOwner(DWORD dwBattleNum)
{
	m_StockTableOwner.RemoveStockItem(dwBattleNum);
}

//��������
tagTableOwner * CBattleManager::CreateTableOwner(DWORD dwBattleNum)
{
	return m_StockTableOwner.CreateStockItem(dwBattleNum);
}

//��������
tagTableOwner * CBattleManager::SearchTableOwner(DWORD dwBattleNum)
{
	return m_StockTableOwner.SearchStockItem(dwBattleNum);
}

//��������
tagTableNodeHead * CBattleManager::SearchTableByKindID(WORD wKindID)
{
	//Ѱ������
	tagTableNodeHead TableNodeHead;
	if (m_MapTableKindID.Lookup(wKindID,TableNodeHead)==FALSE)
	{
		return NULL;
	}

	return &m_MapTableKindID[wKindID];
}

//��������
tagTableNodeHead * CBattleManager::SearchTableByUserID(DWORD dwUserID)
{
	//Ѱ������
	tagTableNodeHead TableNodeHead;
	if (m_MapTableUserID.Lookup(dwUserID,TableNodeHead)==FALSE)
	{
		return NULL;
	}

	return &m_MapTableUserID[dwUserID];	
}

//��������
tagTableNodeHead* CBattleManager::SearchTableByClubID(DWORD dwClubID)
{
	//Ѱ������
	tagTableNodeHead TableNodeHead;
	if (m_MapTableClubID.Lookup(dwClubID, TableNodeHead) == FALSE)
	{
		return NULL;
	}

	return &m_MapTableClubID[dwClubID];
}

//��ӽڵ�
bool CBattleManager::InsertKindTableNode(WORD wKindID, tagTableNode * pTableNode)
{
	//���ҽڵ�
	tagTableNodeHead * pTableUserNodeHead=SearchTableByKindID(wKindID);
	if (pTableUserNodeHead!=NULL) 
	{
		pTableNode->pNextKindNode=pTableUserNodeHead->pTableNodeHead;	
		pTableUserNodeHead->pTableNodeHead=pTableNode;
		pTableUserNodeHead->nNodeCount++;
	}
	else
	{
		//����ṹ
		tagTableNodeHead TableUserNodeHead;
		TableUserNodeHead.nNodeCount=1;
		TableUserNodeHead.pTableNodeHead=pTableNode;

		//����ӳ��
		m_MapTableKindID[wKindID]=TableUserNodeHead;
	}

	return true;
}


//��ӽڵ�
bool CBattleManager::InsertUserTableNode(DWORD dwUserID, tagTableNode * pTableNode)
{
	//���ҽڵ�
	tagTableNodeHead * pTableUserNodeHead=SearchTableByUserID(dwUserID);
	if (pTableUserNodeHead!=NULL) 
	{
		pTableNode->pNextUserNode=pTableUserNodeHead->pTableNodeHead;	
		pTableUserNodeHead->pTableNodeHead=pTableNode;
		pTableUserNodeHead->nNodeCount++;
	}
	else
	{
		//����ṹ
		tagTableNodeHead TableUserNodeHead;
		TableUserNodeHead.nNodeCount=1;
		TableUserNodeHead.pTableNodeHead=pTableNode;

		//����ӳ��
		m_MapTableUserID[dwUserID]=TableUserNodeHead;
	}

	return true;
}

//��ӽڵ�
bool CBattleManager::InsertClubTableNode(DWORD dwClubID, tagTableNode* pTableNode)
{
	//���ҽڵ�
	tagTableNodeHead* pTableUserNodeHead = SearchTableByClubID(dwClubID);
	if (pTableUserNodeHead != NULL)
	{
		pTableNode->pNextClubNode = pTableUserNodeHead->pTableNodeHead;
		pTableUserNodeHead->pTableNodeHead = pTableNode;
		pTableUserNodeHead->nNodeCount++;
	}
	else
	{
		//����ṹ
		tagTableNodeHead TableUserNodeHead;
		TableUserNodeHead.nNodeCount = 1;
		TableUserNodeHead.pTableNodeHead = pTableNode;

		//����ӳ��
		m_MapTableClubID[dwClubID] = TableUserNodeHead;
	}

	return true;
}

//�Ƴ��ڵ�
bool CBattleManager::RemoveKindTableNode(WORD wKindID, tagTableNode * pTableNode)
{
	//���ҽڵ�
	tagTableNodeHead * pTableUserNodeHead=SearchTableByKindID(wKindID);
	if (pTableUserNodeHead!=NULL)
	{
		//��������
		tagTableNode * pTableNodeHead=pTableUserNodeHead->pTableNodeHead;

		//��������
		if (pTableNodeHead==pTableNode)
		{
			if (pTableNodeHead->pNextKindNode==NULL)
			{
				m_MapTableKindID.RemoveKey(wKindID);					
			}
			else
			{
				//���ýڵ�
				pTableUserNodeHead->nNodeCount--;
				pTableUserNodeHead->pTableNodeHead=pTableNodeHead->pNextKindNode;				
			}
		}
		else
		{	
			//�Ƴ��ڵ�
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

//�Ƴ��ڵ�
bool CBattleManager::RemoveUserTableNode(DWORD dwUserID, tagTableNode * pTableNode)
{
	//���ҽڵ�
	tagTableNodeHead * pTableUserNodeHead=SearchTableByUserID(dwUserID);
	if (pTableUserNodeHead!=NULL)
	{
		//��������
		tagTableNode * pTableNodeHead=pTableUserNodeHead->pTableNodeHead;

		//��������
		if (pTableNodeHead==pTableNode)
		{
			if (pTableNodeHead->pNextUserNode==NULL)
			{
				m_MapTableUserID.RemoveKey(dwUserID);					
			}
			else
			{
				//���ýڵ�
				pTableUserNodeHead->nNodeCount--;
				pTableUserNodeHead->pTableNodeHead=pTableNodeHead->pNextUserNode;				
			}
		}
		else
		{	
			//�Ƴ��ڵ�
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


//�Ƴ��ڵ�
bool CBattleManager::RemoveClubTableNode(DWORD dwClubID, tagTableNode* pTableNode)
{
	//���ҽڵ�
	tagTableNodeHead* pTableUserNodeHead = SearchTableByClubID(dwClubID);
	if (pTableUserNodeHead != NULL)
	{
		//��������
		tagTableNode* pTableNodeHead = pTableUserNodeHead->pTableNodeHead;

		//��������
		if (pTableNodeHead == pTableNode)
		{
			if (pTableNodeHead->pNextClubNode == NULL)
			{
				m_MapTableClubID.RemoveKey(dwClubID);
			}
			else
			{
				//���ýڵ�
				pTableUserNodeHead->nNodeCount--;
				pTableUserNodeHead->pTableNodeHead = pTableNodeHead->pNextClubNode;
			}
		}
		else
		{
			//�Ƴ��ڵ�
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

//���ҷ���
tagServerNode * CBattleManager::SearchServerNode(WORD wKindID)
{
	//���ҷ���
	tagServerNode * pServerNodeHead=NULL;
	m_MapServerNode.Lookup(wKindID,pServerNodeHead);

	return pServerNodeHead;
}

//���ҷ���
tagServerNode * CBattleManager::SearchServerNode(WORD wKindID,WORD wServerID)
{
	//���ҷ���	
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

//���ɱ��
DWORD CBattleManager::BuildNumber(int nNumberLen)
{
	//��������
	DWORD dwMultiple[]={1,10,100,1000,10000,100000,1000000,1000000,10000000,1000000000};

	//��������
	DWORD dwBattleNum=0;
	int nLen=__min(nNumberLen,CountArray(dwMultiple));

	//���ɱ��
	for (int i=nLen-1; i>=0; i--)
	{
		dwBattleNum+=(i==(nLen-1)?(rand()%9+1):(rand()%10))*dwMultiple[i];
	}

	return dwBattleNum;
}

//��������
void CBattleManager::BuildPassword(TCHAR szPassword[LEN_PASSWORD],WORD wBufferCount)
{
	//��������
	LPCTSTR pszWords=TEXT("123456789abcdefghijklmnopqrstuvwxyz");

	//���볤��
	int nWordsLen=lstrlen(pszWords);
	int nPasswordLen=__min(MIN_PASSWORD_LEN+rand()%(LEN_PASSWORD-MIN_PASSWORD_LEN),wBufferCount);

	//�������
	for (int i=0; i<nPasswordLen; i++)
	{
		szPassword[i]=pszWords[rand()%nWordsLen];
	}
}

///////////////////////////////////////////////////////////////////////////////////