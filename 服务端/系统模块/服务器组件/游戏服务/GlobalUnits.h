#ifndef GLOBAL_UNITES_HEAD_FILE
#define GLOBAL_UNITES_HEAD_FILE

#pragma once

#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//全局单元
class CGlobalUnits
{
	//聊天控制
protected:
	bool							m_bAllowWisper;						//私聊标志
	bool							m_bAllowRoomChat;					//聊天标志
	bool							m_bAllowGameChat;					//聊天标志
	bool							m_bForbidPlayChat;					//同桌游戏聊天
	bool							m_bMemberCanChat;					//会员可以聊天

	//房间控制
protected:
	bool							m_bAllowEnterRoom;					//进入标志
	bool							m_bAllowEnterGame;					//进入标志
	bool							m_bForbidQueryUser;					//查询标志			
	bool							m_bCloseWhilePlayer;				//无人时关闭房间
	bool							m_bForbidLookon;					//禁止旁观
	BYTE							m_cbAllowLookOnChat;				//允许旁观聊天0禁止聊天1允许会员聊天2允许聊天
	bool							m_bAllowRobotAttend;				//机器人陪打	
	bool							m_bAllowRobotSimulate;				//机器人占座
		
	//积分控制
protected:
	SCORE							m_lMaxJoinScore;					//最高进入积分

	//静态变量
protected:
	static CGlobalUnits *			m_pGlobalUnits;						//对象指针


	//函数定义
public:
	//构造函数
	CGlobalUnits();
	//析构函数
	virtual ~CGlobalUnits();

	//聊天控制
public:
	//私聊状态
	bool IsAllowWisper() { return m_bAllowWisper; }
	//聊天状态
	bool IsAllowRoomChat() { return m_bAllowRoomChat; }
	//聊天状态
	bool IsAllowGameChat() { return m_bAllowGameChat; }
	//聊天状态
	bool IsForbidPlayChat() { return m_bForbidPlayChat; }
	//会员聊天
	bool IsMemberCanChat() { return m_bMemberCanChat; }
	//私聊状态
	VOID SetAllowWisper(bool bAllowWisper) { m_bAllowWisper=bAllowWisper; }
	//聊天状态
	VOID SetAllowRoomChat(bool bAllowRoomChat) { m_bAllowRoomChat=bAllowRoomChat; }
	//聊天状态
	VOID SetAllowGameChat(bool bAllowGameChat) { m_bAllowGameChat=bAllowGameChat; }
	//聊天状态
	VOID SetForbidPlayChat(bool bForbidPlayChat) { m_bForbidPlayChat=bForbidPlayChat; }
	//会员聊天
	VOID SetMemberCanChat(bool bMemCanChat) { m_bMemberCanChat = bMemCanChat; }
	//旁观聊天
	BYTE GetAllowLookOnChat(){ return m_cbAllowLookOnChat;}

	//房间控制
public:
	//进入状态
	bool IsAllowEnterRoom() { return m_bAllowEnterRoom; }
	//进入状态
	bool IsAllowEnterGame() { return m_bAllowEnterGame; }
	//是否禁止查询
	bool IsForbidQueryUser() { return m_bForbidQueryUser; }
	//是否允许机器人
	bool IsAllowRobotAttend() { return m_bAllowRobotAttend; }
	//是否允许机器人
	bool IsAllowRobotSimulate() { return m_bAllowRobotSimulate; }
	//是否自动关闭房间
	bool IsCloseWhileNoplayer() { return m_bCloseWhilePlayer; }
	//禁止旁观
	bool IsForbidLookon() { return m_bForbidLookon; }
	//获取最高进入积分
	SCORE GetMaxJoinScore() { return m_lMaxJoinScore; }
	//进入状态
	VOID SetAllowEnterRoom(bool bAllowEnterRoom) { m_bAllowEnterRoom=bAllowEnterRoom; }
	//进入状态
	VOID SetAllowEnterGame(bool bAllowEnterGame) { m_bAllowEnterGame=bAllowEnterGame; }
	//设置最高进入积分
	VOID SetMaxJoinScore( SCORE lMaxJoinScore ) { m_lMaxJoinScore = lMaxJoinScore; }
	//设置查询
	VOID SetForbidQueryUser( bool bForbidQuery ) { m_bForbidQueryUser = bForbidQuery; }
	//机器人陪打
	VOID SetAllowRobotAttend(bool bAllowRobotAttend) { m_bAllowRobotAttend=bAllowRobotAttend; }	
	//机器人占座
	VOID SetAllowRobotSimulate(bool bAllowRobotSimulate) { m_bAllowRobotSimulate=bAllowRobotSimulate; }	
	//设置房间自动关闭
	VOID SetCloseWhileNoplayer(bool bCloseWhileNoplayer) { m_bCloseWhilePlayer = bCloseWhileNoplayer; }
	//禁止旁观
	VOID SetForbidLookon(bool bForbidLookon) { m_bForbidLookon = bForbidLookon; }
	//允许旁观聊天
	VOID SetAllowLookOnChat(BYTE cbAllowLookOnChat) {m_cbAllowLookOnChat=cbAllowLookOnChat;}
};

//////////////////////////////////////////////////////////////////////////

//对象说明
extern CGlobalUnits					g_GlobalUnits;						//全局单元

//////////////////////////////////////////////////////////////////////////

#endif