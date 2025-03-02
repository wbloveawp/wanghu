#ifndef MODULE_SERVER_HEAD_FILE
#define MODULE_SERVER_HEAD_FILE
#pragma once

//引入文件
#include "RobotUserItem.h"
#include "SimulateLinkHead.h"
#include "VirtualLinkManager.h"

/////////////////////////////////////////////////////////////////////////////////////
//消息定义

#define WM_SERVER_LINK_SHUT			(WM_USER+1100)					//链接关闭
#define WM_SERVER_LINK_FAILURE		(WM_USER+1101)					//链接关闭
#define WM_SERVER_LOGON_FINISH		(WM_USER+1102)					//登录完成	
#define WM_SERVER_LOGON_FAILURE		(WM_USER+1103)					//登录失败
#define WM_SERVER_MATCH_COMPLETED	(WM_USER+1104)					//比赛完成

/////////////////////////////////////////////////////////////////////////////////////
 
//房间模块
class CModuleServer : public IModuleItem, public CVirtualLinkItem
{
	//配置变量
protected:
	WORD						m_wKindID;							//类型索引
	WORD						m_wTableCount;						//桌子数目
	WORD						m_wChairCount;						//椅子数目
	WORD						m_wServerType;						//房间类型
	DWORD						m_dwServerRule;						//房间规则	

	//组件变量
protected:
	CRobotUserItem*				m_pRobotUserItem;					//机器子项
	tagGlobalUserData*			m_pGlobalUserData;					//用户指针

	//缓冲变量
protected:
	tagDataPacket				m_BufferPacket;						//缓冲数据

	//比赛变量
protected:
	DWORD						m_dwMatchID;						//比赛标识
	SCORE						m_lMatchNo;							//比赛场次
	BYTE						m_cbMatchUStatus;					//比赛状态

	//接口变量
protected:
	IModuleItemSink*			m_pIModuleItemSink;					//回调接口

	//函数定义
public:
	//析构函数
	virtual ~CModuleServer();
	//构造函数
	CModuleServer(IModuleItemSink* pIModuleItemSink);

	//回调函数
public:
	//关闭事件
	virtual bool OnEventVirtualShut();
	//连接事件
	virtual bool OnEventVirtualLink(LONG lErrorCode);
	//读取事件
	virtual bool OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize);

	//信息函数
public:
	//时钟标识
	virtual DWORD GetTimerStart() { return IDI_MODULE_SERVER_START; }
	//时钟范围
	virtual DWORD GetTimerRange() { return IDI_MODULE_SERVER_RANGE; }

	//功能函数
public:
	//关闭时钟
	virtual bool KillModuleTimer(UINT uTimerID);
	//设置时钟
	virtual bool SetModuleTimer(UINT uTimerID, UINT nElapse);

	//时钟事件
public:
	//结束时钟
	virtual bool OnEventTimer(UINT uTimerID, WPARAM wBindParam);

	//网络函数
public:
	//登录处理
	bool OnSocketMainLogon(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//配置处理
	bool OnSocketMainConfig(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//用户处理
	bool OnSocketMainUser(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//状态处理
	bool OnSocketMainStatus(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//系统处理
	bool OnSocketMainSystem(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//比赛处理
	bool OnSocketMainMatch(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//网络处理
public:
	//登录失败
	bool OnSocketSubLogonFailure(VOID* pData, WORD wDataSize);
	//登录成功
	bool OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize);
	//登录完成
	bool OnSocketSubLogonFinish(VOID* pData, WORD wDataSize);
	//更新提示
	bool OnSocketSubUpdateNotify(VOID* pData, WORD wDataSize);
	//配置规则
	bool OnSocketSubConfigRule(VOID* pData, WORD wDataSize);
	//配置房间
	bool OnSocketSubConfigServer(VOID* pData, WORD wDataSize);	
	//用户进入
	bool OnSocketSubUserEnter(VOID* pData, WORD wDataSize);
	//用户积分
	bool OnSocketSubUserScore(VOID* pData, WORD wDataSize);
	//用户状态
	bool OnSocketSubUserStatus(VOID* pData, WORD wDataSize);
	//系统消息
	bool OnSocketSubSystemMessage(VOID* pData, WORD wDataSize);
	//请求失败
	bool OnSocketSubRequestFailure(VOID* pData, WORD wDataSize);
	//比赛状态
	bool OnSocketSubMatchStatus(VOID* pData, WORD wDataSize);
	//比赛状态
	bool OnSocketSubMatchUStatus(VOID* pData, WORD wDataSize);	
	//报名成功
	bool OnSocketSubSignupSuccess(VOID* pData, WORD wDataSize);

	//功能函数
public:
	//重置模块
	VOID ResetModule();
	//进入房间
	bool EnterServer(WORD wKindID, WORD wServerID, WORD wServerLevel);
	//进入房间
	bool EnterMatchServer(WORD wKindID, WORD wServerID, DWORD dwMatchID, SCORE lMatchNo);		

	//控制函数
protected:
	//登录房间
	bool SendLogonPacket(WORD wKindID, WORD wServerLevel, WORD wServerID, VOID* pExtendData, WORD wDataSize);
};

/////////////////////////////////////////////////////////////////////////////////////

#endif

