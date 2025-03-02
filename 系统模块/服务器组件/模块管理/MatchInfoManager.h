#ifndef MATCH_INFO_MANAGER_HEAD_FILE
#define MATCH_INFO_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////

//数组定义
typedef CWHArray<tagMatchOption *> CMatchOptionArray;
typedef CWHArray<tagMatchRoundItem *> CMatchRoundItemArray;
typedef CWHArray<tagMatchRewardItem *> CMatchRewardItemArray;

//索引定义
typedef CMap<DWORD,DWORD,tagMatchOption *,tagMatchOption *> CMatchOptionMap;
typedef CMap<DWORD,DWORD,tagMatchRoundItem *,tagMatchRoundItem *> CMatchRoundItemMap;
typedef CMap<DWORD,DWORD,tagMatchRewardItem *,tagMatchRewardItem *> CMatchRewardItemMap;

//////////////////////////////////////////////////////////////////////////
//比赛数据
class MODULE_MANAGER_CLASS CMatchInfoBuffer
{
	//变量定义
public:
	CMatchOptionArray		m_MatchOptionArray;			  //模块数组
	CMatchOptionArray		m_MatchOptionBuffer;			//模块数组	

	//函数定义
public:
	//构造函数
	CMatchInfoBuffer();
	//析构函数
	virtual ~CMatchInfoBuffer();

	//管理函数
public:
	//重置数据
	bool ResetMatchOption();
	//删除数据
	bool DeleteMatchOption(DWORD dwMatchID);
	//插入数据
	tagMatchOption * InsertMatchOption(tagMatchOption * pMatchOption);

	//信息函数
public:
	//获取数目
	DWORD GetMatchOptionCount();
	//查找数据
	tagMatchOption * SearchMatchOption(DWORD dwMatchID);

	//内部函数
private:
	//创建对象
	tagMatchOption * CreateMatchOption();
};

//////////////////////////////////////////////////////////////////////////

//奖励数据
class MODULE_MANAGER_CLASS CRewardInfoBuffer
{
	//变量定义
public:
	CMatchRewardItemArray		m_MatchRewardItemArray;			  //奖励数组
	CMatchRewardItemArray		m_MatchRewardItemBuffer;			//奖励数组	

	//函数定义
public:
	//构造函数
	CRewardInfoBuffer();
	//析构函数
	virtual ~CRewardInfoBuffer();

	//管理函数
public:
	//重置数据
	bool ResetMatchRewardItem();
	//删除数据
	bool DeleteMatchRewardItem(WORD wRankID);
	//插入数据
	tagMatchRewardItem * InsertMatchRewardItem(tagMatchRewardItem & MatchRewardItem);

	//信息函数
public:
	//获取数目
	WORD GetMatchRewardCount();
	//查找数据
	tagMatchRewardItem * SearchMatchReward(WORD wRankID);

	//内部函数
private:
	//创建对象
	tagMatchRewardItem * CreateMatchRewardItem();
};

//////////////////////////////////////////////////////////////////////////

//轮次数据
class MODULE_MANAGER_CLASS CRoundInfoBuffer
{
	//变量定义
public:
	CMatchRoundItemArray		m_MatchRoundItemArray;			  //轮次数组
	CMatchRoundItemArray		m_MatchRoundItemBuffer;				//轮次数组	

	//函数定义
public:
	//构造函数
	CRoundInfoBuffer();
	//析构函数
	virtual ~CRoundInfoBuffer();

	//管理函数
public:
	//重置数据
	bool ResetMatchRoundItem();
	//删除数据
	bool DeleteMatchRoundItem(WORD wRoundID);
	//插入数据
	tagMatchRoundItem * InsertMatchRoundItem(tagMatchRoundItem & MatchRoundItem);

	//信息函数
public:
	//获取数目
	WORD GetMatchRoundCount();
	//查找数据
	tagMatchRoundItem * SearchMatchRound(WORD wRoundID);

	//内部函数
private:
	//创建对象
	tagMatchRoundItem * CreateMatchRoundItem();
};

//////////////////////////////////////////////////////////////////////////

//模块信息
class MODULE_MANAGER_CLASS CMatchInfoManager
{
	//函数定义
public:
	//构造函数
	CMatchInfoManager();
	//析构函数
	virtual ~CMatchInfoManager();

	//比赛管理
public:		
	//加载比赛
	bool LoadMatchOption(WORD wKindID,TCHAR szMachineID[LEN_MACHINE_ID],CMatchInfoBuffer & MatchInfoBuffer);

	//比赛管理
public:
	//删除房间
	bool DeleteMatchOption(DWORD dwMatchID);
	//插入房间
	bool InsertMatchOption(tagCreateMatchInfo * pCreateMatchInfo,tagMatchOption & GameMatchResult);
	//修改房间
	bool ModifyMatchOption(tagCreateMatchInfo * pCreateMatchInfo,tagMatchOption & GameMatchResult);

	//内部函数
private:
	//连接数据
	bool ConnectPlatformDB(CDataBaseHelper & PlatformDBModule);
	//连接数据
	bool ConnectGameMatchDB(CDataBaseHelper & GameMatchDBModule);	
	//读取比赛
	bool ReadMatchOption(CDataBaseAide & GameMatchDBAide, tagMatchOption & GameMatchResult);
};

//////////////////////////////////////////////////////////////////////////

#endif