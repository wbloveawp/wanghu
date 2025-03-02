#include "StdAfx.h"
#include "DistributeManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////

//��������
#define DISTRIBUTE_WAIT_TIME			5							//�ȴ�ʱ��

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CDistributeManager::CDistributeManager()
{
	//���ñ���
	m_wMinPlayerCount=0;
	m_cbDistributeRule=0;
}

//��������
CDistributeManager::~CDistributeManager()
{
	//�Ƴ�Ԫ��
	RemoveAll();

	//�ͷŻ���
	for (INT_PTR i=0; i<m_SameTableBuffer.GetCount(); i++)
	{
		SafeDelete(m_SameTableBuffer[i]);
	}

	//�ͷŻ���
	for (INT_PTR i=0; i<m_DistributeNodeBuffer.GetCount(); i++)
	{
		SafeDelete(m_DistributeNodeBuffer[i]);
	}	

	//�ͷ�Ԫ��
	m_SameTableBuffer.RemoveAll();
	m_DistributeNodeBuffer.RemoveAll();
}

//������
bool CDistributeManager::InsertDistributeNode(IServerUserItem * pIServerUserItem)
{
	//����У��
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem==NULL) return false;

	//�����û�
	if (SearchNode(pIServerUserItem)!=NULL) return false;

	//������
	tagDistributeNode * pDistributeNode=ActiveDistributeNode();
	if (pDistributeNode==NULL) return false;

	//���ýڵ�
	pDistributeNode->DistributeInfo.dwInsertTime=(DWORD)time(NULL);
	pDistributeNode->DistributeInfo.pIServerUserItem=pIServerUserItem;

	//��ӽڵ�
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

//�Ƴ����
void CDistributeManager::RemoveDistributeNode(IServerUserItem * pIServerUserItem)
{
	//���ҽ��
	tagDistributeNode * pDistributeNode=SearchNode(pIServerUserItem);
	if (pDistributeNode!=NULL) RemoveDistributeNode(pDistributeNode);
}

//�Ƴ����
VOID CDistributeManager::RemoveDistributeNode(tagDistributeNode * pDistributeNode)
{
	//���뻺��
	m_DistributeNodeBuffer.Add(pDistributeNode);

	//�����ڵ�
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

//��ȡ��Ϣ
tagSameTableInfo * CDistributeManager::GetUserSameTableInfo()
{
	//�����ж�
	if ((m_cbDistributeRule&DISTRIBUTE_SAME_TABLE)!=0)
	{
		return NULL;
	}

	//�����ڵ�
	tagSameTableNode * pSameTableNode=ActiveSameTableNode();
	if (pSameTableNode!=NULL)
	{
		return &pSameTableNode->SameTableInfo;
	}

	return NULL;
}

//��ȡ��Ϣ
tagSameTableInfo * CDistributeManager::GetUserSameTableInfo(DWORD dwUserID)
{
	//��������
	tagSameTableNode * pSameTableNode;
	if (m_SameTableMap.Lookup(dwUserID,pSameTableNode))
	{
		return &pSameTableNode->SameTableInfo;
	}

	return NULL;
}

//��ȡ��Ϣ
VOID CDistributeManager::SetUserSameTableInfo(DWORD dwUserID,const tagSameTableInfo * pSameTableInfo)
{
	//��������
	tagSameTableNode * pSameTableNode=NULL;
	if (m_SameTableMap.Lookup(dwUserID,pSameTableNode))
	{
		//�Ƴ�ͬ��
		RemoveSameTableInfo(dwUserID);
	}

	//��ȡ�ڵ�
	pSameTableNode=CONTAINING_RECORD(pSameTableInfo,tagSameTableNode,SameTableInfo);

	//����ͬ��
	pSameTableNode->cbReferCount++;
	m_SameTableMap[dwUserID]=pSameTableNode;
}

//�Ƴ���Ϣ
VOID CDistributeManager::RemoveSameTableInfo(DWORD dwUserID)
{
	//��������
	tagSameTableNode * pSameTableNode=NULL;
	if (m_SameTableMap.Lookup(dwUserID,pSameTableNode)==TRUE)
	{
		//�Ƴ���Ϣ
		m_SameTableMap.RemoveKey(dwUserID);

		//���ñ���
		if (--pSameTableNode->cbReferCount==0)
		{			
			RemoveSameTableNode(pSameTableNode);		
		}		
	}
}		

//�������
tagSameTableNode * CDistributeManager::ActiveSameTableNode()
{
	//��������
	tagSameTableNode * pSameTableNode=NULL;

	//���һ���
	if (m_SameTableBuffer.GetCount()>0)
	{
		pSameTableNode=m_SameTableBuffer.GetAt(0);
		m_SameTableBuffer.RemoveAt(0);
		m_SameTableActive.Add(pSameTableNode);
	}

	//��������
	if (pSameTableNode==NULL)
	{
		try
		{
			//��������
			pSameTableNode = new tagSameTableNode;
			if(pSameTableNode==NULL) throw(TEXT("�ڴ治��,���󴴽�ʧ��!"));

			//���ö���
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

//�Ƴ�����
VOID CDistributeManager::RemoveSameTableNode(tagSameTableNode * pSameTableNode) 
{
	//�������
	m_SameTableBuffer.Add(pSameTableNode);
	ZeroMemory(pSameTableNode,sizeof(tagSameTableNode));	

	//���Ҷ���
	for (INT_PTR i=0; i<m_SameTableActive.GetCount(); i++)
	{
		if (m_SameTableActive[i]==pSameTableNode)
		{
			m_SameTableActive.RemoveAt(i);
			break;
		}
	}
}

//����ڵ�
tagDistributeNode * CDistributeManager::ActiveDistributeNode()
{
	//��������
	tagDistributeNode * pDistributeNode=NULL;

	//���һ���
	if (m_DistributeNodeBuffer.GetCount()>0)
	{
		pDistributeNode=m_DistributeNodeBuffer[0];
		m_DistributeNodeBuffer.RemoveAt(0);
	}

	//�����ڵ�
	if (pDistributeNode==NULL)
	{
		try
		{
			//�����ڴ�
			pDistributeNode=new tagDistributeNode();
			if (pDistributeNode==NULL) throw TEXT("�ڵ㴴��ʧ�ܣ��ڴ治��!");

			//��ʼ�ڴ�
			ZeroMemory(pDistributeNode,sizeof(tagDistributeNode));
		}
		catch (...)
		{
			ASSERT (FALSE);
		}
	}

	return pDistributeNode;
}

//�Ƴ����
VOID CDistributeManager::RemoveAll()
{
	//׷�ӽڵ�
	m_SameTableBuffer.Append(m_SameTableActive);
	m_DistributeNodeBuffer.Append(m_PlayerNodeActive);
	m_DistributeNodeBuffer.Append(m_RobotNodeActive);

	//�ͷ�Ԫ��
	m_SameTableMap.RemoveAll();
	m_SameTableActive.RemoveAll();
	m_PlayerNodeActive.RemoveAll();
	m_RobotNodeActive.RemoveAll();	
}

//ִ�з���
WORD CDistributeManager::PerformDistribute(CDistributeInfoArray & DistributeInfoArray, WORD wNeedCount)
{
	//�����ж�
	if (m_PlayerNodeActive.GetCount()==0) return false;

	//��������
	CDistributeNodeArray DistributeNodeArray;

	//�������
	if (m_cbDistributeRule&DISTRIBUTE_PLAYER_FIRST && GetPlayerCount()>=m_wMinPlayerCount)
	{
		//��������		
		DistributeNodeArray.Append(m_PlayerNodeActive);
	}
	else
	{
		//��������
		DistributeNodeArray.Append(m_RobotNodeActive);
		DistributeNodeArray.Append(m_PlayerNodeActive);		
	}

	//�����ж�
	if (DistributeNodeArray.GetCount()<wNeedCount) return false;

	//��ȡʱ���
	DWORD dwCurrentTime=(DWORD)time(NULL);

	//�����û�
	for (INT_PTR i=0; i< DistributeNodeArray.GetCount(); i++)
	{
		//�Ƴ�����
		DistributeInfoArray.RemoveAll();
		DistributeInfoArray.Add(&DistributeNodeArray[i]->DistributeInfo);

		//��������
		CDistributeNodeArray DistributePartNodeArray;
		DistributePartNodeArray.Append(DistributeNodeArray);
		DistributePartNodeArray.RemoveAt(i);

		//�������
		while (DistributePartNodeArray.GetCount()>0)
		{
			//��ȡ�ڵ�
			int nRandIndex=rand()%DistributePartNodeArray.GetCount();
			tagDistributeNode * pDistributeNode=DistributePartNodeArray[nRandIndex];

			//�Ƴ��ڵ�
			DistributePartNodeArray.RemoveAt(nRandIndex);

			//ʱ�����
			if ((dwCurrentTime-pDistributeNode->DistributeInfo.dwInsertTime)<DISTRIBUTE_WAIT_TIME)
			{
				continue;
			}

			//��������
			if ((DistributeInfoArray.GetCount()==wNeedCount-1) && 
				FilterRuleIsAllRobot(DistributeInfoArray,pDistributeNode->DistributeInfo.pIServerUserItem))
			{
				continue;
			}

			//��ͬ��ַ
			if ((m_cbDistributeRule&DISTRIBUTE_SAME_ADDRESS)==0 &&
				FilterRuleExitsClientAddr(DistributeInfoArray,pDistributeNode->DistributeInfo.pIServerUserItem))
			{
				continue;
			}

			//��ͬ������
			if ((m_cbDistributeRule&DISTRIBUTE_SAME_MACHINE)==0 &&			
				FilterRuleExitsMachineID(DistributeInfoArray,pDistributeNode->DistributeInfo.pIServerUserItem))
			{
				continue;
			}		

			//�Ͼ�ͬ��
			if ((m_cbDistributeRule&DISTRIBUTE_SAME_TABLE)==0 &&
				FilterRuleExitsLastTable(DistributeInfoArray,pDistributeNode->DistributeInfo.pIServerUserItem))
			{
				continue;
			}

			//��ӽڵ�
			DistributeInfoArray.Add(&pDistributeNode->DistributeInfo);

			//�ɹ��ж�
			if (DistributeInfoArray.GetCount()==wNeedCount)
			{
				return true;
			}
		}
	}

	return false;	
}

//���ҽ��
tagDistributeNode * CDistributeManager::SearchNode(IServerUserItem * pIServerUserItem)
{
	//����У��
	ASSERT (pIServerUserItem!=NULL);
	if(pIServerUserItem==NULL) return NULL;

	//�������
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

//��������
bool CDistributeManager::FilterRuleIsAllRobot(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem * const pIServerUserItem)
{
	//����У��
	if (pIServerUserItem==NULL || DistributeInfoArray.GetCount()==0) return false;

	//��������
	WORD wRobotCount=0;

	//ͳ�ƻ���
	for (INT_PTR nIndex=0;nIndex<DistributeInfoArray.GetCount();nIndex++)
	{
		if (DistributeInfoArray[nIndex]->pIServerUserItem->IsRobotUser()==true)
		{
			++wRobotCount;
		}
	}

	return (wRobotCount==DistributeInfoArray.GetCount()) && pIServerUserItem->IsRobotUser();
}

//�Ͼ�ͬ��
bool CDistributeManager::FilterRuleExitsLastTable(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem * pIServerUserItem)
{
	//����У��	
	if (DistributeInfoArray.GetCount()==0) return FALSE;

	//��������
	tagSameTableInfo * pSameTableInfo=NULL;

	//ͬ���Ƚ�
	for (INT_PTR i=0; i<DistributeInfoArray.GetCount(); i++)
	{
		//��ȡ��Ϣ
		pSameTableInfo=GetUserSameTableInfo(DistributeInfoArray[i]->pIServerUserItem->GetUserID());
		if (pSameTableInfo==NULL) continue;

		//�Ƚϱ�ʶ
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

//��ͬ��ַ
bool CDistributeManager::FilterRuleExitsClientAddr(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem * pIServerUserItem)
{
	//����ͬIP
	for (INT_PTR i=0; i<DistributeInfoArray.GetCount(); i++)
	{
		if (DistributeInfoArray[i]->pIServerUserItem->GetClientIP()==pIServerUserItem->GetClientIP())
		{
			return true;
		}
	}

	return false;
}

//��ͬ������
bool CDistributeManager::FilterRuleExitsMachineID(const CDistributeInfoArray & DistributeInfoArray,IServerUserItem * pIServerUserItem)
{
	//����ͬIP
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
