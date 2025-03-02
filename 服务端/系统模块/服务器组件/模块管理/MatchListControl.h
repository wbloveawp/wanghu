#ifndef MATCH_LIST_CONTROL_HEAD_FILE
#define MATCH_LIST_CONTROL_HEAD_FILE

#pragma once

#include "ListControl.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////

//�����б�
class MODULE_MANAGER_CLASS CMatchListControl : public CListControl
{
	//��������
public:
	//���캯��
	CMatchListControl();
	//��������
	virtual ~CMatchListControl();

	//���غ���
public:
	//�����б�
	virtual VOID InitListControl();
	//��������
	virtual INT SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex);
	//��ȡ��ɫ
	virtual VOID GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor);

	//���ܺ���
public:
	//�����б�
	bool InsertMatchOption(tagMatchOption * pMatchOption);
	//�����б�
	bool UpdateMatchOption(tagMatchOption * pMatchOption);

	//��������
private:
	//��������
	WORD GetInsertIndex(tagMatchOption * pMatchOption);
	//�����ַ�
	LPCTSTR GetDescribeString(tagMatchOption * pMatchOption, WORD wColumnIndex);
};

//////////////////////////////////////////////////////////////////////////////////

//�����б�
class MODULE_MANAGER_CLASS CRewardListControl : public CListControl
{
	//��������
public:
	//���캯��
	CRewardListControl();
	//��������
	virtual ~CRewardListControl();

	//���غ���
public:
	//�����б�
	virtual VOID InitListControl();
	//��������
	virtual INT SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex);
	//��ȡ��ɫ
	virtual VOID GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor);

	//���ܺ���
public:
	//�����б�
	bool InsertMatchReward(tagMatchRewardItem * pMatchRewardItem);
	//�����б�
	bool UpdateMatchReward(tagMatchRewardItem * pMatchRewardItem);
	//ɾ���б�
	bool DeleteMatchReward(tagMatchRewardItem * pMatchRewardItem);

	//��������
private:
	//��������
	WORD GetInsertIndex(tagMatchRewardItem * pMatchRewardItem);
	//�����ַ�
	LPCTSTR GetDescribeString(tagMatchRewardItem * pMatchRewardItem, WORD wColumnIndex);
};


//////////////////////////////////////////////////////////////////////////////////
//�ִ��б�
class MODULE_MANAGER_CLASS CRoundListControl : public CListControl
{
	//��������
public:
	//���캯��
	CRoundListControl();
	//��������
	virtual ~CRoundListControl();

	//���غ���
public:
	//�����б�
	virtual VOID InitListControl();
	//��������
	virtual INT SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex);
	//��ȡ��ɫ
	virtual VOID GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor);

	//���ܺ���
public:
	//�����б�
	bool InsertMatchRound(tagMatchRoundItem * pMatchRoundItem);
	//�����б�
	bool UpdateMatchRound(tagMatchRoundItem * pMatchRoundItem);
	//ɾ���б�
	bool DeleteMatchRound(tagMatchRoundItem * pMatchRoundItem);

	//��������
private:
	//��������
	WORD GetInsertIndex(tagMatchRoundItem * pMatchRoundItem);
	//�����ַ�
	LPCTSTR GetDescribeString(tagMatchRoundItem * pMatchRoundItem, WORD wColumnIndex);
};

///////////////////////////////////////////////////////////////////////////////////////////

#endif