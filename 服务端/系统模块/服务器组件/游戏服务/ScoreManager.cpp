#include "StdAfx.h"
#include "ScoreManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//常量定义
#define	AREA_MIN_PLAYER					6

//////////////////////////////////////////////////////////////////////////

//构造函数
CScoreManager::CScoreManager()
{
	//设置变量
	m_lScoreCount=0;
	m_pIServerUserManager=NULL;
	ZeroMemory(m_lScoreTemp,sizeof(m_lScoreTemp));
	ZeroMemory(m_lScoreSource,sizeof(m_lScoreSource));	
}

//析构函数
CScoreManager::~CScoreManager()
{
}

//更新数据
VOID CScoreManager::UpdateScoreData()
{
	//采集数据
	CollectScore();

	//归并排序
	MergeSort(m_lScoreSource,m_lScoreTemp,0,m_lScoreCount-1);
	
	//划分区间
	DivideScoreArea();
}

//获取分区
INT CScoreManager::GetScoreAreaIndex(SCORE lScore)
{
	//判断长度
	if (m_ScoreAreaArray.GetCount()==0) return 0;

	//匹配分数
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

//采集分数
VOID CScoreManager::CollectScore()
{
	//设置变量
	m_lScoreCount=0;

	//变量定义
	WORD wEnumIndex=0;
	IServerUserItem * pIServerUserItem=NULL;
	do
{
		//枚举用户
		pIServerUserItem=m_pIServerUserManager->EnumUserItem(wEnumIndex++);
		if (pIServerUserItem==NULL) break;

		//设置分数		
		m_lScoreSource[m_lScoreCount++]=pIServerUserItem->GetUserFreeScore();

		//溢出判断
		if (m_lScoreCount>=CountArray(m_lScoreSource)) break;

	} while (true);
}

//划分区间
VOID CScoreManager::DivideScoreArea()
{
	//移除数据
	m_ScoreAreaArray.RemoveAll();

	//变量定义
	tagScoreArea ScoreArea;
	ZeroMemory(&ScoreArea,sizeof(ScoreArea));

	//变量定义
	INT nAreaIndex=0;	
	INT nStartIndex=0;
	INT nScoreCount=0;
	SCORE lScoreArea[]={0,1000,10000,100000,500000,1000000,5000000,10000000,50000000,100000000,100000000000000};

	//变量定义
	INT nAreaScoreCount[CountArray(lScoreArea)-1];
	ZeroMemory(nAreaScoreCount,sizeof(nAreaScoreCount));

	//统计分数
	for (INT i=0; i<m_lScoreCount; i++)
{
		while (m_lScoreSource[i]>lScoreArea[nAreaIndex+1]) nAreaIndex++; 
		if (m_lScoreSource[i]>=lScoreArea[nAreaIndex] && m_lScoreSource[i]<lScoreArea[nAreaIndex+1])
	{
			nAreaScoreCount[nAreaIndex]++;
		}
	}		

	//划分区间
	for (INT i=0; i<CountArray(nAreaScoreCount); i++)
{
		nScoreCount+=nAreaScoreCount[i];
		if (nScoreCount>=AREA_MIN_PLAYER)
	{
			//构造结构
			ScoreArea.lMaxScore=lScoreArea[i+1];
			ScoreArea.lMinScore=lScoreArea[nStartIndex];				

			//添加区间
			m_ScoreAreaArray.Add(ScoreArea);

			//设置变量
			nScoreCount=0;
			nStartIndex=i+1;
		}
	}

	//剩余判断
	if (nScoreCount>0)
{
		//修改区间
		if (m_ScoreAreaArray.GetCount()>0)
	{
			tagScoreArea * pScoreArea=&m_ScoreAreaArray[m_ScoreAreaArray.GetCount()-1];
			pScoreArea->lMaxScore=lScoreArea[CountArray(lScoreArea)-1];
		}
		else
	{
			//构造结构
			ScoreArea.lMinScore=lScoreArea[0];				
			ScoreArea.lMaxScore=lScoreArea[CountArray(lScoreArea)-1];			

			//添加区间
			m_ScoreAreaArray.Add(ScoreArea);
		}
	}
}

//归并排序
VOID CScoreManager::MergeSort(SCORE nSource[], SCORE nTemp[], int nStartIndex, int nEndIndex)
{
	//参数判断
	if (nStartIndex>=nEndIndex) return;

	//计算索引
	int nMidIndex=(nStartIndex+nEndIndex)/2;

	//递归调用
	MergeSort(nSource,nTemp,nStartIndex,nMidIndex);
	MergeSort(nSource,nTemp,nMidIndex+1,nEndIndex);

	//归并数据
	MergeData(nSource,nTemp,nStartIndex,nMidIndex,nEndIndex);
}

//比较数据
VOID CScoreManager::MergeData(SCORE nSource[], SCORE nTemp[], int nStartIndex, int nMidIndex, int nEndIndex)
{
	//变量定义
	int i=nStartIndex, j=nMidIndex+1, k=nStartIndex;

	//比较数据
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

	//拷贝数据
  while (i!=nMidIndex+1) nTemp[k++]=nSource[i++];
  while (j!=nEndIndex+1) nTemp[k++]=nSource[j++];

  for (i=nStartIndex; i<=nEndIndex; i++)
{
    nSource[i]=nTemp[i];
	}
}

//////////////////////////////////////////////////////////////////////////
