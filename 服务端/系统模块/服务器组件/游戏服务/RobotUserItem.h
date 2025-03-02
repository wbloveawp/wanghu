#ifndef ROBOT_USER_TIEM_HEAD_FILE
#define ROBOT_USER_TIEM_HEAD_FILE

#pragma once

#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//时间子项
struct tagTimerItem
{
	UINT							nTimerID;							//时间标识
	UINT							nTimeLeave;							//剩余时间
};

//类说明
typedef CWHArray<tagTimerItem *> CTimerItemArray;					//时间数组

//////////////////////////////////////////////////////////////////////////

//机器人信息
class CRobotUserItem : public IRobotUserItem
{
	//友元定义
	friend class CRobotUserManager;

	//状态变量
protected:
	bool							m_bWaitLeave;						//等待离开
	bool							m_bStartClient;						//游戏标志
	BYTE							m_cbGameStatus;						//游戏状态
	tagUserStatus					m_CurrentUserStatus;				//用户状态
	WORD							m_wRobotAction;						//机器动作
	DWORD							m_dwPlayInnings;					//游戏局数

	//调试变量
protected:
	UINT							m_nCurrTimerID;						//时钟变量

	//绑定变量
protected:
	DWORD							m_dwClientVersion;					//游戏版本
	ITableFrameItem*				m_pITableFrameItem;					//桌子接口
	IServerUserItem *				m_pIServerUserItem;					//用户接口

	//状态信息
protected:
	tagRobotService					m_RobotService;						//服务信息
	tagRobotItemConfig				m_RobotItemConfig;					//机器信息

	//索引变量
protected:
	WORD							m_wRoundID;							//循环索引
	WORD							m_wRobotIndex;						//索引变量

	//时间变量
protected:
	DWORD							m_dwMinSitInterval;					//坐下间隔
	DWORD							m_dwMaxSitInterval;					//坐下间隔
	DWORD							m_dwStandupTickCount;				//起立时间
	DWORD							m_dwReposeTickCount;				//休息时间

	//时间组件
protected:
	CTimerItemArray					m_TimerItemActive;					//活动数组
	static CTimerItemArray			m_TimerItemStorage;					//库存数组

	//接口变量
protected:
	IServerUserManager *			m_pIServerUserManager;				//用户管理
	IRobotUserManager *				m_pIRobotUserManager;				//管理接口
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
	virtual VOID Release() { delete this; }
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

	//信息接口
public:
	//获取自己
	virtual IServerUserItem* GetMeUserItem() { return m_pIServerUserItem; }

	//网络接口
public:
	//发送函数
	virtual bool SendSocketData(WORD wSubCmdID);
	//发送函数
	virtual bool SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize);

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
	//设置动作
	virtual void SetRobotAction(WORD wRobotAction, bool bEnable);
	//设置版本
	virtual VOID SetAdroidVersion( DWORD dwVersion ) { m_dwClientVersion = dwVersion; }	

	//功能函数
public:
	//功能函数
	bool JudgeRobotAction(WORD wAction);
	//桌子接口
	VOID SetTableFrameItem(ITableFrameItem* pITableFrameItem);

	//同桌信息
public:
	//真人数目
	virtual WORD GetTableRealUserCount();
	//同桌用户
	virtual IServerUserItem* GetTableUserItem(WORD wChairID);	

	//配置信息
public:
	//获取状态
	tagRobotService * GetRobotService() { return &m_RobotService; }
	//获取配置
	tagRobotParameter * GetRobotParameter() { return m_RobotItemConfig.pRobotParameter; }

	//事件通知
public:
	//时间事件
	bool OnTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter);
	//网络消息
	bool OnSocketRead(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//网络事件
protected:
	//登录失败
	bool OnSocketSubLogonFailure(VOID * pData, WORD wDataSize);	
	//登录完成
	bool OnSocketSubLogonFinish(VOID * pData, WORD wDataSize);
	//用户状态
	bool OnSocketSubUserStatus(VOID * pData, WORD wDataSize);
	//请求失败
	bool OnSocketSubRequestFailure(VOID * pData, WORD wDataSize);
	//游戏消息
	bool OnSocketGameMessage(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//系统消息
	bool OnSocketSubSystemMessage(VOID * pData, WORD wDataSize);

	//控制函数
protected:
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