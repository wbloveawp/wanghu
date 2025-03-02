#ifndef MATCH_LIST_CONTROL_HEAD_FILE
#define MATCH_LIST_CONTROL_HEAD_FILE

#pragma once

#include "ListControl.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////

//比赛列表
class MODULE_MANAGER_CLASS CMatchListControl : public CListControl
{
	//函数定义
public:
	//构造函数
	CMatchListControl();
	//析构函数
	virtual ~CMatchListControl();

	//重载函数
public:
	//配置列表
	virtual VOID InitListControl();
	//子项排序
	virtual INT SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex);
	//获取颜色
	virtual VOID GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor);

	//功能函数
public:
	//插入列表
	bool InsertMatchOption(tagMatchOption * pMatchOption);
	//更新列表
	bool UpdateMatchOption(tagMatchOption * pMatchOption);

	//辅助函数
private:
	//插入索引
	WORD GetInsertIndex(tagMatchOption * pMatchOption);
	//描述字符
	LPCTSTR GetDescribeString(tagMatchOption * pMatchOption, WORD wColumnIndex);
};

//////////////////////////////////////////////////////////////////////////////////

//奖励列表
class MODULE_MANAGER_CLASS CRewardListControl : public CListControl
{
	//函数定义
public:
	//构造函数
	CRewardListControl();
	//析构函数
	virtual ~CRewardListControl();

	//重载函数
public:
	//配置列表
	virtual VOID InitListControl();
	//子项排序
	virtual INT SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex);
	//获取颜色
	virtual VOID GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor);

	//功能函数
public:
	//插入列表
	bool InsertMatchReward(tagMatchRewardItem * pMatchRewardItem);
	//更新列表
	bool UpdateMatchReward(tagMatchRewardItem * pMatchRewardItem);
	//删除列表
	bool DeleteMatchReward(tagMatchRewardItem * pMatchRewardItem);

	//辅助函数
private:
	//插入索引
	WORD GetInsertIndex(tagMatchRewardItem * pMatchRewardItem);
	//描述字符
	LPCTSTR GetDescribeString(tagMatchRewardItem * pMatchRewardItem, WORD wColumnIndex);
};


//////////////////////////////////////////////////////////////////////////////////
//轮次列表
class MODULE_MANAGER_CLASS CRoundListControl : public CListControl
{
	//函数定义
public:
	//构造函数
	CRoundListControl();
	//析构函数
	virtual ~CRoundListControl();

	//重载函数
public:
	//配置列表
	virtual VOID InitListControl();
	//子项排序
	virtual INT SortListItemData(LPARAM lParam1, LPARAM lParam2, WORD wColumnIndex);
	//获取颜色
	virtual VOID GetListItemColor(INT nItemID,LPARAM lItemParam, UINT uItemStatus, tagListItemColor & ListItemColor);

	//功能函数
public:
	//插入列表
	bool InsertMatchRound(tagMatchRoundItem * pMatchRoundItem);
	//更新列表
	bool UpdateMatchRound(tagMatchRoundItem * pMatchRoundItem);
	//删除列表
	bool DeleteMatchRound(tagMatchRoundItem * pMatchRoundItem);

	//辅助函数
private:
	//插入索引
	WORD GetInsertIndex(tagMatchRoundItem * pMatchRoundItem);
	//描述字符
	LPCTSTR GetDescribeString(tagMatchRoundItem * pMatchRoundItem, WORD wColumnIndex);
};

///////////////////////////////////////////////////////////////////////////////////////////

#endif