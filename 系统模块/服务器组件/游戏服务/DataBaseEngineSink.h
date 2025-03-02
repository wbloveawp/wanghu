#ifndef DATABASE_ENGINE_SINK_HEAD_FILE
#define DATABASE_ENGINE_SINK_HEAD_FILE

#pragma once

#include "DataBasePacket.h"
#include "GameServiceHead.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//数据库钩子
class CDataBaseEngineSink : public IDataBaseEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//变量定义
protected:
	INT_PTR							m_nSyncIndex;						//同步ID

	//系统配置
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置 

	//辅助变量
protected:
	DBO_GR_LogonFailure				m_LogonFailure;						//登录失败
	DBO_GR_LogonSuccess				m_LogonSuccess;						//登录成功

	//游戏数据库
protected:
	CDataBaseAide					m_GameDBAide;						//游戏数据库
	CDataBaseHelper					m_GameDBModule;						//游戏数据库

	//游戏数据库
protected:
	CDataBaseAide					m_AccountsDBAide;					//账号数据库
	CDataBaseHelper					m_AccountsDBModule;					//账号数据库

	//平台数据库
protected:
	CDataBaseAide					m_PlatformDBAide;					//平台数据库
	CDataBaseHelper					m_PlatformDBModule;					//平台数据库

	//扩展接口
protected:
	IGameDataBaseSink *				m_pIGameDataBaseSink;				//扩展钩子
	IExtendDataBaseSink *			m_pIExtendDataBaseSink;				//扩展钩子

	//组件变量
protected:
	ITimerEngine *					m_pITimerEngine;					//定时引擎
	IDataBaseEngine *				m_pIDataBaseEngine;					//数据引擎
	IDataBaseEngineEvent *			m_pIDataBaseEngineEvent;			//数据事件		
	CAttemperEngineSink *			m_pAttemperEngineSink;				//调度勾子
	
	//函数定义
public:
	//构造函数
	CDataBaseEngineSink();
	//析构函数
	virtual ~CDataBaseEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//系统事件
public:
	//启动事件
	virtual bool OnDataBaseEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnDataBaseEngineStop(IUnknownEx * pIUnknownEx);

	//内核事件
public:
	//时间事件
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//控制事件
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize);
	//请求事件
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//加载请求
private:
	//加载机器
	bool OnRequestLoadRobots(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//加载配置
	bool OnRequestLoadConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//登录请求
private:
	//机器登录
	bool OnRequestLogonRobot(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//手机登录
	bool OnRequestLogonMobile(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//电脑登录
	bool OnRequestLogonComputer(DWORD dwContextID, DWORD dwTokenID,VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//游戏请求
private:
	//录像记录
	bool OnRequestRecordGameVideo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//游戏记录
	bool OnRequestRecordDrawInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//写分记录
	bool OnRequestRecordDrawScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//用户权限
	bool OnRequestManageUserRight(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//赠送礼物
	bool OnRequestSendGift(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//更新魅力
	bool OnRequestUpdateLoveLiness(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);	

	//系统请求
private:
	//离开房间
	bool OnRequestLeaveService(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//游戏写分
	bool OnRequestWriteGameScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//用户财富
	bool OnRequestWriteUserWealth(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//赠送低保
	bool OnRequestLargessSubsidy(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);

	//背包请求
private:
	//查询物品
	bool OnRequestUserQueryGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//更新物品	
	bool OnRequestUserUpdateGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//记录物品	
	bool OnRequestUserRecordGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);

	//控制请求
protected:
	//控制查询
	bool OnRequestControlQuery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode);
	//控制开始
	bool OnRequestControlStart(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//控制结束
	bool OnRequestControlFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);
	//控制更新
	bool OnRequestControlUpdate(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode);	

	//辅助函数
private:	
	//解锁机器
	bool UnlockRobotUser();
	//加载背包
	bool LoadUserPack(DWORD dwUserID);
	//查询低保
	bool LoadUserSubsidy(DWORD dwUserID);
	//加载控制
	bool LoadControlInfo(DWORD dwUserID,tagUserControlItem& UserControlItem);
	//更新魅力
	bool UpdateUserLoveLiness(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID);
	//登录结果
	VOID OnLogonDisposeResult(DWORD dwContextID, DWORD dwTokenID, LONG lResultCode, bool bMobileClient);
	//登录失败
	VOID OnLogonDisposeDBError(DWORD dwContextID, DWORD dwTokenID, IDataBaseException * pIException);		

	//加载函数
private:	
	//敏感词汇
	bool LoadFilterWords(DWORD dwContextID, DWORD dwTokenID);	
	//约战配置
	bool LoadBattleOption(DWORD dwContextID, DWORD dwTokenID);
	//机器参数
	bool LoadRobotParameter(DWORD dwContextID, DWORD dwTokenID);
	//低保参数
	bool LoadSubsidyParameter(DWORD dwContextID, DWORD dwTokenID);
	//控制参数
	bool LoadControlParameter(DWORD dwContextID, DWORD dwTokenID);
	//游戏等级
	bool LoadGameLevelOption(DWORD dwContextID, DWORD dwTokenID);
	//游戏物品
	bool LoadGameGoodsOption(DWORD dwContextID, DWORD dwTokenID);
	//版本信息
	bool LoadGameVersionInfo(DWORD dwContextID, DWORD dwTokenID);	
	//全局配置
	bool LoadGlobalOption(DWORD dwContextID, DWORD dwTokenID, DWORD dwOptionMask);
};

//////////////////////////////////////////////////////////////////////////

#endif