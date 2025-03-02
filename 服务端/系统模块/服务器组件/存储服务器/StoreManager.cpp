#include "stdafx.h"
#include "StoreManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////////

//���캯��
CServerManager::CServerManager()
{
	//��������
	m_MapServerNode.InitHashTable(103);
}

//��������
CServerManager::~CServerManager()
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
}

//��������
WORD CServerManager::GetServerCount(WORD wKindID)
{
	//Ѱ�ҷ���
	tagServerNode * pServerNodeHead=NULL;
	if (m_MapServerNode.Lookup(wKindID,pServerNodeHead)==FALSE)
	{
		return 0;
	}

	//��������
	WORD wServerCount=0;
	tagServerNode * pServerNodeMove=pServerNodeHead;

	//���ҷ���
	while (pServerNodeMove!=NULL)
	{	
		//ͳ������
		wServerCount++;

		//�ƶ��ڵ�
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}

	return wServerCount;
}

//�Ƴ�����
bool CServerManager::RemoveServer(WORD wKindID,WORD wServerID)
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
			pServerNodeMove->ServerItem.bRemoved=true;
			break;
		}

		//�ƶ��ڵ�
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}		

	return true;
}

//��ӷ���
bool CServerManager::AppendServer(tagServerItem & ServerItem)
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
				if (pServerNodeNext->ServerItem.bRemoved==true)
				{					
					pServerNodeNext->ServerItem.bRemoved=false;					
					pServerNodeNext->ServerItem.wTableUseCount=0;
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

	//������Ϣ
	CopyMemory(&pServerNodeNew->ServerItem,&ServerItem,sizeof(pServerNodeNew->ServerItem));

	//����״̬
	pServerNodeNew->ServerItem.bRemoved=false;
	pServerNodeNew->ServerItem.wTableUseCount=0;
  
	//���ӽڵ�
	pServerNodeNew->pNextServerNode=pServerNodeHead;
	m_MapServerNode[ServerItem.wKindID]=pServerNodeNew;
	
	return true;
}

//ö�ٷ���
tagServerItem * CServerManager::EnumServer(WORD wKindID,WORD wServerID)
{
	//Ѱ�ҷ���
	tagServerNode * pServerNodeHead=NULL;
	if (m_MapServerNode.Lookup(wKindID,pServerNodeHead)==FALSE)
	{
		return NULL;
	}

	//��������
	tagServerNode * pServerNodeMove=pServerNodeHead;

	//���ҷ���
	while (wServerID!=0 && pServerNodeMove!=NULL)
	{	
		//�жϱ�ʶ
		if (pServerNodeMove->ServerItem.bRemoved==false && pServerNodeMove->ServerItem.wServerID==wServerID)
		{		
			//�ƶ��ڵ�
			pServerNodeMove=pServerNodeMove->pNextServerNode;
			break;
		}

		//�ƶ��ڵ�
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}

	//���ҷ���
	while (pServerNodeMove!=NULL)
	{	
		//�жϱ�ʶ
		if (pServerNodeMove->ServerItem.bRemoved==false)
		{		
			return &pServerNodeMove->ServerItem;
		}

		//�ƶ��ڵ�
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}

	return NULL;
}

//���ҷ���
tagServerItem * CServerManager::SearchServer(WORD wKindID,WORD wServerID)
{
	//Ѱ�ҷ���
	tagServerNode * pServerNodeHead=NULL;
	if (m_MapServerNode.Lookup(wKindID,pServerNodeHead)==FALSE)
	{
		return NULL;
	}

	//��������
	tagServerNode * pServerNodeMove=pServerNodeHead;

	//���ҷ���
	while (wServerID!=0 && pServerNodeMove!=NULL)
	{	
		//�жϱ�ʶ
		if (pServerNodeMove->ServerItem.wServerID==wServerID)
		{						
			break;
		}

		//�ƶ��ڵ�
		pServerNodeMove=pServerNodeMove->pNextServerNode;
	}

	return &pServerNodeMove->ServerItem;
}

//��ȡ����
tagServerItem * CServerManager::GetServerItem(WORD wKindID,WORD wServerID,WORD wTableCount,BYTE cbStoreType)
{
	//��������
	tagServerItem * pServerItem=NULL;

	do
	{
		//ö�ٷ���
		pServerItem=EnumServer(wKindID,wServerID);
		if (pServerItem==NULL) break;

		//У������
		if (pServerItem->cbStoreType==cbStoreType && pServerItem->wTableCount-pServerItem->wTableUseCount>=wTableCount)
		{
			return pServerItem;
		}

		//���ñ�ʶ
		wServerID=pServerItem->wServerID;

	} while (true);

	return NULL;
}

//�����
tagServerNode * CServerManager::ActiveServerNode()
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
void CServerManager::RemoveServerNode(tagServerNode * pServerNode)
{
	//���뻺��	
	m_ServerNodeBuffer.Add(pServerNode);
	ZeroMemory(pServerNode,sizeof(tagServerNode));
}

//��������
void CServerManager::ResetData()
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
}

///////////////////////////////////////////////////////////////////////////////////