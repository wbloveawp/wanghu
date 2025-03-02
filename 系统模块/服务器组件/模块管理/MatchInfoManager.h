#ifndef MATCH_INFO_MANAGER_HEAD_FILE
#define MATCH_INFO_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////

//���鶨��
typedef CWHArray<tagMatchOption *> CMatchOptionArray;
typedef CWHArray<tagMatchRoundItem *> CMatchRoundItemArray;
typedef CWHArray<tagMatchRewardItem *> CMatchRewardItemArray;

//��������
typedef CMap<DWORD,DWORD,tagMatchOption *,tagMatchOption *> CMatchOptionMap;
typedef CMap<DWORD,DWORD,tagMatchRoundItem *,tagMatchRoundItem *> CMatchRoundItemMap;
typedef CMap<DWORD,DWORD,tagMatchRewardItem *,tagMatchRewardItem *> CMatchRewardItemMap;

//////////////////////////////////////////////////////////////////////////
//��������
class MODULE_MANAGER_CLASS CMatchInfoBuffer
{
	//��������
public:
	CMatchOptionArray		m_MatchOptionArray;			  //ģ������
	CMatchOptionArray		m_MatchOptionBuffer;			//ģ������	

	//��������
public:
	//���캯��
	CMatchInfoBuffer();
	//��������
	virtual ~CMatchInfoBuffer();

	//������
public:
	//��������
	bool ResetMatchOption();
	//ɾ������
	bool DeleteMatchOption(DWORD dwMatchID);
	//��������
	tagMatchOption * InsertMatchOption(tagMatchOption * pMatchOption);

	//��Ϣ����
public:
	//��ȡ��Ŀ
	DWORD GetMatchOptionCount();
	//��������
	tagMatchOption * SearchMatchOption(DWORD dwMatchID);

	//�ڲ�����
private:
	//��������
	tagMatchOption * CreateMatchOption();
};

//////////////////////////////////////////////////////////////////////////

//��������
class MODULE_MANAGER_CLASS CRewardInfoBuffer
{
	//��������
public:
	CMatchRewardItemArray		m_MatchRewardItemArray;			  //��������
	CMatchRewardItemArray		m_MatchRewardItemBuffer;			//��������	

	//��������
public:
	//���캯��
	CRewardInfoBuffer();
	//��������
	virtual ~CRewardInfoBuffer();

	//������
public:
	//��������
	bool ResetMatchRewardItem();
	//ɾ������
	bool DeleteMatchRewardItem(WORD wRankID);
	//��������
	tagMatchRewardItem * InsertMatchRewardItem(tagMatchRewardItem & MatchRewardItem);

	//��Ϣ����
public:
	//��ȡ��Ŀ
	WORD GetMatchRewardCount();
	//��������
	tagMatchRewardItem * SearchMatchReward(WORD wRankID);

	//�ڲ�����
private:
	//��������
	tagMatchRewardItem * CreateMatchRewardItem();
};

//////////////////////////////////////////////////////////////////////////

//�ִ�����
class MODULE_MANAGER_CLASS CRoundInfoBuffer
{
	//��������
public:
	CMatchRoundItemArray		m_MatchRoundItemArray;			  //�ִ�����
	CMatchRoundItemArray		m_MatchRoundItemBuffer;				//�ִ�����	

	//��������
public:
	//���캯��
	CRoundInfoBuffer();
	//��������
	virtual ~CRoundInfoBuffer();

	//������
public:
	//��������
	bool ResetMatchRoundItem();
	//ɾ������
	bool DeleteMatchRoundItem(WORD wRoundID);
	//��������
	tagMatchRoundItem * InsertMatchRoundItem(tagMatchRoundItem & MatchRoundItem);

	//��Ϣ����
public:
	//��ȡ��Ŀ
	WORD GetMatchRoundCount();
	//��������
	tagMatchRoundItem * SearchMatchRound(WORD wRoundID);

	//�ڲ�����
private:
	//��������
	tagMatchRoundItem * CreateMatchRoundItem();
};

//////////////////////////////////////////////////////////////////////////

//ģ����Ϣ
class MODULE_MANAGER_CLASS CMatchInfoManager
{
	//��������
public:
	//���캯��
	CMatchInfoManager();
	//��������
	virtual ~CMatchInfoManager();

	//��������
public:		
	//���ر���
	bool LoadMatchOption(WORD wKindID,TCHAR szMachineID[LEN_MACHINE_ID],CMatchInfoBuffer & MatchInfoBuffer);

	//��������
public:
	//ɾ������
	bool DeleteMatchOption(DWORD dwMatchID);
	//���뷿��
	bool InsertMatchOption(tagCreateMatchInfo * pCreateMatchInfo,tagMatchOption & GameMatchResult);
	//�޸ķ���
	bool ModifyMatchOption(tagCreateMatchInfo * pCreateMatchInfo,tagMatchOption & GameMatchResult);

	//�ڲ�����
private:
	//��������
	bool ConnectPlatformDB(CDataBaseHelper & PlatformDBModule);
	//��������
	bool ConnectGameMatchDB(CDataBaseHelper & GameMatchDBModule);	
	//��ȡ����
	bool ReadMatchOption(CDataBaseAide & GameMatchDBAide, tagMatchOption & GameMatchResult);
};

//////////////////////////////////////////////////////////////////////////

#endif