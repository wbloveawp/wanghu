#ifndef SCORE_MANAGER_HEAD_FILE
#define SCORE_MANAGER_HEAD_FILE

#pragma once

#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////////
//��������
#define MAX_SCORE_COUNT					512									//�����Ŀ

//��������
struct tagScoreArea
{
	SCORE							lMinScore;							//��С����				
	SCORE							lMaxScore;							//������
};


//���鶨��
typedef CWHArray<tagScoreArea> CScoreAreaArray;

//////////////////////////////////////////////////////////////////////////////////////

//��������
class CScoreManager
{	
	//�洢����
protected:
	INT									m_lScoreCount;						//������Ŀ
	SCORE							m_lScoreTemp[MAX_SCORE_COUNT];		//��������
	SCORE							m_lScoreSource[MAX_SCORE_COUNT];	//��������
	CScoreAreaArray						m_ScoreAreaArray;					//��������

	//�ӿڱ���
public:
	IServerUserManager *				m_pIServerUserManager;				//�û�����

	//��������
public:
	//���캯��
	CScoreManager();
	//��������
	virtual ~CScoreManager();

	//���ܺ���
public:
	//��������
	VOID UpdateScoreData();
	//��ȡ����
	INT GetScoreAreaIndex(SCORE lScore);

	//��������
protected:
	//�ɼ�����
	VOID CollectScore();
	//��������
	VOID DivideScoreArea();

	//������
private:
	//�鲢����
	VOID MergeSort(SCORE nSource[], SCORE nTemp[], int nStartIndex, int nEndIndex);
	//�Ƚ�����
	VOID MergeData(SCORE nSource[], SCORE nTemp[], int nStartIndex, int nMidIndex, int nEndIndex);	
};

//////////////////////////////////////////////////////////////////////////

#endif
