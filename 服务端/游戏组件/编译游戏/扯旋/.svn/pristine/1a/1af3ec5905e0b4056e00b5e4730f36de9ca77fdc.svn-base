#ifndef ANDROID_USER_ITEM_SINK_HEAD_FILE
#define ANDROID_USER_ITEM_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////

//机器人类
class CRobotUserItemSink : public IRobotUserItemSink
{
	//游戏变量
protected:
	WORD							m_wBankerUser;						//庄家用户
	LONGLONG						m_lMinAddScore;						//最小加注
	LONGLONG						m_lTurnMaxScore;					//最大下注
	LONGLONG						m_lTurnMinScore;					//最小下注
	LONGLONG						m_lUserInvest;						//用户下本
	LONGLONG						m_lUserScore;						//用户分数
	LONGLONG						m_lCellScore;						//单元下注
	LONGLONG						m_lTotalAddBet;						//总下注
	BYTE							m_cbUserStatus[GAME_PLAYER];		//用户状态
	LONGLONG						m_lMaxBoboMultiple;					//簸簸倍数
	bool							m_bGiveUp;							//放弃

	//扑克变量
protected:
	BYTE							m_cbHandCardCount[GAME_PLAYER];			//手牌数量
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//桌面扑克

	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	IRobotUserItem *				m_pITestUserItem;					//用户接口

	//函数定义
public:
	//构造函数
	CRobotUserItemSink();
	//析构函数
	virtual ~CRobotUserItemSink();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//控制接口
public:
	//初始接口
	virtual bool InitUserItemSink(IUnknownEx * pIUnknownEx);
	//重置接口
	virtual bool ResetUserItemSink();

	//游戏事件
public:
	//时间消息
	virtual bool __cdecl OnEventTimer(UINT nTimerID);
	//游戏消息
	virtual bool __cdecl OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//游戏消息
	virtual bool __cdecl OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//场景消息
	virtual bool __cdecl OnEventSendGameScene(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize);

	//用户事件
public:
	//用户进入
	virtual VOID __cdecl OnEventUserEnter(IRobotUserItem * pITestUserItem, bool bLookonUser);
	//用户离开
	virtual VOID __cdecl OnEventUserLeave(IRobotUserItem * pITestUserItem, bool bLookonUser);
	//用户积分
	virtual VOID __cdecl OnEventUserScore(IRobotUserItem * pITestUserItem, bool bLookonUser);
	//用户状态
	virtual VOID __cdecl OnEventUserStatus(IRobotUserItem * pITestUserItem, bool bLookonUser);
	//用户段位
	virtual VOID __cdecl OnEventUserSegment(IRobotUserItem * pITestUserItem, bool bLookonUser);

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(VOID * pData, WORD wDataSize);
	//用户出牌
	bool OnUserInvest(VOID * pData, WORD wDataSize);
	//用户加注
	bool OnAddScore(VOID * pData, WORD wDataSize);
	//游戏结束
	bool OnSubSendCard(VOID * pData, WORD wDataSize);


//辅助函数
protected:
	//测试写日志
	VOID WriteControlLog(string & str);
};

//////////////////////////////////////////////////////////////////////////

#endif