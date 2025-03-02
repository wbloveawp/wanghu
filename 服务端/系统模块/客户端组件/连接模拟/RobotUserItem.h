#ifndef ROBOT_USER_TIEM_HEAD_FILE
#define ROBOT_USER_TIEM_HEAD_FILE

#pragma once

#include "VirtualLinkManager.h"
#include "SimulateLinkHead.h"

//////////////////////////////////////////////////////////////////////////

//时间子项
struct tagTimerItem
{
	UINT							nTimerID;							//时间标识
	UINT							nTimeLeave;							//剩余时间
};

//类说明
typedef CWHArray<tagTimerItem *> CTimerItemArray;						//时间数组

//////////////////////////////////////////////////////////////////////////

//机器人信息
class CRobotUserItem : public IRobotUserItem
{
	//友元定义
	friend class CRobotUserManager;

	//状态变量
protected:
	bool							m_bStartClient;						//游戏标志
	BYTE							m_cbGameStatus;						//游戏状态	
	
	//绑定变量
protected:
	DWORD							m_dwClientVersion;					//游戏版本
	tagGlobalUserData *				m_pGlobalUserData;					//用户结构

	//时间组件
protected:
	CTimerItemArray					m_TimerItemActive;					//活动数组
	static CTimerItemArray			m_TimerItemStorage;					//库存数组

	//接口变量
protected:
	CVirtualLinkItem*				m_pVirtualLinkItem;					//虚拟链接
	IRobotUserItemSink *			m_pIRobotUserItemSink;				//回调接口

	//函数定义
public:
	//构造函数
	CRobotUserItem();
	//析构函数
	virtual ~CRobotUserItem();

	//基础接口
public:
	//释放对象
	virtual VOID Release() {}
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//状态接口
public:
	//获取状态
	virtual BYTE GetGameStatus() { return m_cbGameStatus; }
	//设置状态
	virtual VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus=cbGameStatus; }

	//用户信息
public:
	//获取 I D
	virtual DWORD GetUserID();
	//桌子号码
	virtual WORD GetTableID();
	//椅子号码
	virtual WORD GetChairID();
	//用户状态
	virtual BYTE GetUserStatus();
	//用户名字
	virtual LPCTSTR GetAccounts();

	//积分信息
public:
	//用户积分
	virtual SCORE GetUserScore();

	//网络接口
public:
	//发送函数
	virtual bool SendSocketData(WORD wSubCmdID);
	//发送函数
	virtual bool SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//信息接口
public:
	//获取自己
	virtual IServerUserItem* GetMeUserItem() { return NULL; }

	//同桌信息
public:
	//真人数目
	virtual WORD GetTableRealUserCount() { return 0; }
	//同桌用户
	virtual IServerUserItem* GetTableUserItem(WORD wChairID) { return NULL; }

	//功能接口
public:
	//删除时间
	virtual bool KillGameTimer(UINT nTimerID);
	//设置时间
	virtual bool SetGameTimer(UINT nTimerID, UINT nElapse);
	//发送准备
	virtual bool SendUserReady(VOID * pData, WORD wDataSize);
	//发送聊天
	virtual bool SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor);

	//配置函数
public:
	//设置版本
	virtual VOID SetAdroidVersion( DWORD dwVersion ) { m_dwClientVersion = dwVersion; }
	//设置动作
	virtual void SetRobotAction(WORD wTestAction, bool bEnable) {}

	//配置函数
public:
	//用户数据
	VOID SetGlobalUserData(tagGlobalUserData* pGlobalUserData);
	//虚拟链接
	VOID SetVirtualLinkItem(CVirtualLinkItem* pVirtualLinkItem);

	//信息函数
public:
	//启动标志
	bool IsStartClient() { return m_bStartClient; }

	//事件通知
public:
	//时间事件
	bool OnTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter);

	//网络事件
public:
	//游戏消息
	bool OnSocketGameMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//控制函数
public:
	//启动游戏
	VOID StartGameClient();
	//关闭游戏
	VOID CloseGameClient();

	//辅助函数
public:
	//复位数据
	VOID RepositUserItem();
};
//////////////////////////////////////////////////////////////////////////

#endif