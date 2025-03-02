#include "StdAfx.h"
#include "ScoreManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//��������
#define	AREA_MIN_PLAYER					6

//////////////////////////////////////////////////////////////////////////

//���캯��
CScoreManager::CScoreManager()
{
	//���ñ���
	m_lScoreCount=0;
	m_pIServerUserManager=NULL;
	ZeroMemory(m_lScoreTemp,sizeof(m_lScoreTemp));
	ZeroMemory(m_lScoreSource,sizeof(m_lScoreSource));	
}

//��������
CScoreManager::~CScoreManager()
{
}

//��������
VOID CScoreManager::UpdateScoreData()
{
	//�ɼ�����
	CollectScore();

	//�鲢����
	MergeSort(m_lScoreSource,m_lScoreTemp,0,m_lScoreCount-1);
	
	//��������
	DivideScoreArea();
}

//��ȡ����
INT CScoreManager::GetScoreAreaIndex(SCORE lScore)
{
	//�жϳ���
	if (m_ScoreAreaArray.GetCount()==0) return 0;

	//ƥ�����
	for (INT_PTR i=0; i<m_ScoreAreaArray.GetCount(); i++)
{
		tagScoreArea * pScoreArea=&m_ScoreAreaArray[i];
		if (lScore>=pScoreArea->lMinScore && lScore<pScoreArea->lMaxScore)
	{
			return i;
		}
	}

	return 0;
}

//�ɼ�����
VOID CScoreManager::CollectScore()
{
	//���ñ���
	m_lScoreCount=0;

	//��������
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;
	do
{
		//ö���û�
		pIServerUserItem=m_pIServerUserManager->EnumUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//���÷���		
		m_lScoreSource[m_lScoreCount++]=pIServerUserItem->GetUserFreeScore();

		//����ж�
		if (m_lScoreCount>=CountArray(m_lScoreSource)) break;

	} while (true);
}

//��������
VOID CScoreManager::DivideScoreArea()
{
	//�Ƴ�����
	m_ScoreAreaArray.RemoveAll();

	//��������
	tagScoreArea ScoreArea;
	ZeroMemory(&ScoreArea,sizeof(ScoreArea));

	//��������
	INT nAreaIndex=0;	
	INT nStartIndex=0;
	INT nScoreCount=0;
	SCORE lScoreArea[]={0,1000,10000,100000,500000,1000000,5000000,10000000,50000000,100000000,100000000000000};

	//��������
	INT nAreaScoreCount[CountArray(lScoreArea)-1];
	ZeroMemory(nAreaScoreCount,sizeof(nAreaScoreCount));

	//ͳ�Ʒ���
	for (INT i=0; i<m_lScoreCount; i++)
{
		while (m_lScoreSource[i]>lScoreArea[nAreaIndex+1]) nAreaIndex++; 
		if (m_lScoreSource[i]>=lScoreArea[nAreaIndex] && m_lScoreSource[i]<lScoreArea[nAreaIndex+1])
	{
			nAreaScoreCount[nAreaIndex]++;
		}
	}		

	//��������
	for (INT i=0; i<CountArray(nAreaScoreCount); i++)
{
		nScoreCount+=nAreaScoreCount[i];
		if (nScoreCount>=AREA_MIN_PLAYER)
	{
			//����ṹ
			ScoreArea.lMaxScore=lScoreArea[i+1];
			ScoreArea.lMinScore=lScoreArea[nStartIndex];				

			//�������
			m_ScoreAreaArray.Add(ScoreArea);

			//���ñ���
			nScoreCount=0;
			nStartIndex=i+1;
		}
	}

	//ʣ���ж�
	if (nScoreCount>0)
{
		//�޸�����
		if (m_ScoreAreaArray.GetCount()>0)
	{
			tagScoreArea * pScoreArea=&m_ScoreAreaArray[m_ScoreAreaArray.GetCount()-1];
			pScoreArea->lMaxScore=lScoreArea[CountArray(lScoreArea)-1];
		}
		else
	{
			//����ṹ
			ScoreArea.lMinScore=lScoreArea[0];				
			ScoreArea.lMaxScore=lScoreArea[CountArray(lScoreArea)-1];			

			//�������
			m_ScoreAreaArray.Add(ScoreArea);
		}
	}
}

//�鲢����
VOID CScoreManager::MergeSort(SCORE nSource[], SCORE nTemp[], int nStartIndex, int nEndIndex)
{
	//�����ж�
	if (nStartIndex>=nEndIndex) return;

	//��������
	int nMidIndex=(nStartIndex+nEndIndex)/2;

	//�ݹ����
	MergeSort(nSource,nTemp,nStartIndex,nMidIndex);
	MergeSort(nSource,nTemp,nMidIndex+1,nEndIndex);

	//�鲢����
	MergeData(nSource,nTemp,nStartIndex,nMidIndex,nEndIndex);
}

//�Ƚ�����
VOID CScoreManager::MergeData(SCORE nSource[], SCORE nTemp[], int nStartIndex, int nMidIndex, int nEndIndex)
{
	//��������
	int i=nStartIndex, j=nMidIndex+1, k=nStartIndex;

	//�Ƚ�����
  while (i!=nMidIndex+1 && j!=nEndIndex+1)
  {
    if (nSource[i]>nSource[j])
	{
      nTemp[k++]=nSource[j++];
		}
    else
	{
      nTemp[k++]=nSource[i++];
		}
  }

	//��������
  while (i!=nMidIndex+1) nTemp[k++]=nSource[i++];
  while (j!=nEndIndex+1) nTemp[k++]=nSource[j++];

  for (i=nStartIndex; i<=nEndIndex; i++)
{
    nSource[i]=nTemp[i];
	}
}

//////////////////////////////////////////////////////////////////////////
