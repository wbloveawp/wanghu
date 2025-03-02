#ifndef SCORE_MANAGER_HEAD_FILE
#define SCORE_MANAGER_HEAD_FILE

#pragma once

#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////////
//常量定义
#define MAX_SCORE_COUNT					512									//最大数目

//分数区间
struct tagScoreArea
{
	SCORE							lMinScore;							//最小分数				
	SCORE							lMaxScore;							//最大分数
};


//数组定义
typedef CWHArray<tagScoreArea> CScoreAreaArray;

//////////////////////////////////////////////////////////////////////////////////////

//分数管理
class CScoreManager
{	
	//存储变量
protected:
	INT									m_lScoreCount;						//分数数目
	SCORE							m_lScoreTemp[MAX_SCORE_COUNT];		//分数数组
	SCORE							m_lScoreSource[MAX_SCORE_COUNT];	//分数数组
	CScoreAreaArray						m_ScoreAreaArray;					//区间数组

	//接口变量
public:
	IServerUserManager *				m_pIServerUserManager;				//用户管理

	//函数定义
public:
	//构造函数
	CScoreManager();
	//析构函数
	virtual ~CScoreManager();

	//功能函数
public:
	//更新数据
	VOID UpdateScoreData();
	//获取分区
	INT GetScoreAreaIndex(SCORE lScore);

	//辅助函数
protected:
	//采集分数
	VOID CollectScore();
	//划分区间
	VOID DivideScoreArea();

	//排序函数
private:
	//归并排序
	VOID MergeSort(SCORE nSource[], SCORE nTemp[], int nStartIndex, int nEndIndex);
	//比较数据
	VOID MergeData(SCORE nSource[], SCORE nTemp[], int nStartIndex, int nMidIndex, int nEndIndex);	
};

//////////////////////////////////////////////////////////////////////////

#endif
