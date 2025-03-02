#ifndef MISSION_SERVER_LIST_HEAD_FILE
#define MISSION_SERVER_LIST_HEAD_FILE

#pragma once

#include "StdAfx.h"
#include "MissionManager.h"

//////////////////////////////////////////////////////////////////////////

//数组说明
typedef CMap<WORD,WORD,WORD,WORD>	CGameKindIDMap;						//类型标识

//////////////////////////////////////////////////////////////////////////

//列表任务
class CMissionList : public CWnd, public CMissionItem
{
	//变量定义
protected:
	bool							m_bOnLineMission;					//更新人数
	CGameKindIDMap					m_GameKindIDMission;				//游戏标识
	CGameKindIDMap					m_GameKindIDPending;				//待决标识

	//静态变量
protected:
	static CMissionList *		m_pMissionServerList;				//对象指针

	//函数定义
public:
	//构造函数
	CMissionList();
	//析构函数
	virtual ~CMissionList();

	//重载函数
public:
	//连接事件
	virtual bool OnEventMissionLink(INT nErrorCode);
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//读取事件
	virtual bool OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize);

	//功能函数
public:
	//更新人数
	VOID UpdateOnLineInfo();
	//更新类型
	VOID UpdateServerInfo(WORD wKindID);
	//加载服务
	VOID LoadServerInfo();

	//静态函数
public:
	//获取对象
	static CMissionList * GetInstance() { return m_pMissionServerList; }

	//消息映射
protected:
	//时间消息
	VOID OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif