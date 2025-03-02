#ifndef ROBOT_USER_MANAGER_HEAD_FILE
#define ROBOT_USER_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "RobotUserItem.h"
#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////////
//参数扩展
struct tagRobotParameterEx
{
	//参数变量
	tagRobotParameter					RobotParameter;						//机器参数

	//标识变量
	bool								bIsValided;							//有效标识
	bool								bIsLoadRobot;						//加载机器	
};

///////////////////////////////////////////////////////////////////////////////////////

//数组数组
typedef CWHArray<tagRobotAction> CRobotActionArray;
typedef CWHArray<CRobotUserItem *> CRobotUserItemArray;
typedef CWHArray<tagRobotParameterEx *> CRobotParameterArray;	
typedef CMap<DWORD,DWORD,CRobotUserItem *, CRobotUserItem *> CRobotUserItemMap;

//////////////////////////////////////////////////////////////////////////////////

//机器人管理
class CRobotUserManager : public IRobotUserManager
{
	//配置变量
protected:
	WORD							m_wAutoRobotCount;					//自动数目

	//机器动作
protected:
	CRobotActionArray				m_RobotActionArray;					//动作数组

	//库存变量
protected:
	WORD							m_wStockCount;						//库存数目
	tagRobotItemConfig				m_RobotItemConfig[MAX_ROBOT];		//机器配置

	//组件接口
public:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IServiceFunction *				m_pIServiceFunction;				//服务接口
	IServerUserManager *			m_pIServerUserManager;				//用户管理		
	IGameServiceManager *			m_pIGameServiceManager;				//服务管理
	IGameServiceCustomTime *		m_pIGameServiceSustomTime;			//时间配置
	ITCPNetworkEngineEvent *		m_pITCPNetworkEngineEvent;			//事件接口

	//服务配置
protected:	
	bool							m_bServiceContinue;					//服务延续
	DWORD							m_dwMinSitInterval;					//休息时间
	DWORD							m_dwMaxSitInterval;					//休息时间	

	//系统配置
public:
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置

	//用户数组
protected:
	CRobotUserItemMap				m_RobotUserItemMap;					//用户索引
	CRobotParameterArray			m_RobotParameterArray;				//参数数组
	CRobotUserItemArray				m_RobotUserItemArray;				//存储对象
	CRobotUserItemArray				m_RobotUserItemBuffer;				//空闲对象

	//函数定义
public:
	//构造函数
	CRobotUserManager();
	//析构函数
	virtual ~CRobotUserManager();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//控制接口
public:
	//启动服务
	virtual bool StartService();
	//停止服务
	virtual bool ConcludeService();

	//配置接口
public:	
	//移除参数
	virtual bool RemoveRobotParameter(DWORD dwBatchID);
	//设置参数
	virtual bool AddRobotParameter(tagRobotParameter RobotParameter[], WORD wParameterCount);
	//插入机器
	virtual bool InsertRobotInfo(tagRobotAccountsInfo RobotAccountsInfo[],WORD wRobotCount,DWORD dwBatchID);

	//管理接口
public:
	//延缓动作
	virtual bool PostponeRobotAction(DWORD dwUserID,WORD wActionCode);
	//删除机器
	virtual bool DeleteRobotUserItem(DWORD dwRobotID,bool bStockRetrieve);
	//查找机器
	virtual IRobotUserItem * SearchRobotUserItem(DWORD dwUserID);
	//创建机器
	virtual IRobotUserItem * CreateRobotUserItem(tagRobotItemConfig & RobotItemConfig);

	//系统事件
public:
	//脉冲事件
	virtual bool OnEventTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter);

	//状态接口
public:
	//机器数目
	virtual WORD GetRobotCount() { return (WORD)m_RobotUserItemMap.GetCount(); }
	//库存数目
	virtual WORD GetRobotStock() { return m_wStockCount; }
	//加载机器
	virtual bool GetRobotLoadInfo(DWORD & dwBatchID,DWORD & dwLoadCount);
	//用户状况
	virtual WORD GetRobotUserInfo(tagRobotUserInfo & RobotUserInfo, DWORD dwServiceMode);

	//网络接口
public:
	//发送数据
	virtual bool SendDataToClient(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据
	virtual bool SendDataToClient(DWORD dwRobotID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据
	virtual bool SendDataToServer(DWORD dwRobotID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//对象管理
protected:
	//释放对象
	VOID FreeRobotUserItem(DWORD dwUserID);	
	//获取对象
	CRobotUserItem * GetRobotUserItem(WORD wIndex);
	//激活对象
	CRobotUserItem * ActiveRobotUserItem(tagRobotItemConfig & RobotItemConfig);
	//强制起立
	VOID ForceRobotUserItemStandup(CRobotUserItem * pRobotUserItem);

	//辅助函数
protected:	 	
	//转换时间
	DWORD GetTodayTickCount();
	//获取参数
	tagRobotParameterEx * GetRobotParameter(DWORD dwBatchID);
	//获取索引
	WORD GetRobotParameterIndex(tagRobotParameterEx * pRobotParameter);	
	//等待时间
	DWORD BatchServiceWaitTime(tagRobotParameter * pRobotParameter,DWORD dwTodayTickCount);
	//服务时间
	DWORD BatchServiceRemaindTime(tagRobotParameter * pRobotParameter,DWORD dwTodayTickCount);	
};

//////////////////////////////////////////////////////////////////////////////////

#endif