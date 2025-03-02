#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////
//数据定义

//服务类型
enum enServiceKind
{
	ServiceKind_None,				//无效服务
	ServiceKind_Game,				//游戏服务	
	ServiceKind_Gate,				//网关服务
};

//绑定参数
struct tagBindParameter
{
	//网络数据
	DWORD							dwSocketID;							//网络标识
	DWORD							dwClientIP;							//连接地址
	DWORD							dwActiveTime;						//激活时间

	//附加数据
	WORD							wServiceID;							//服务标识
	WORD							wServiceModule;						//服务模块
};

//////////////////////////////////////////////////////////////////////////
//类型定义
typedef CWHStockData<WORD, tagGameKindEx>  CStockGameKind;				//游戏类型

//////////////////////////////////////////////////////////////////////////

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink, IStockManagerSink
{
	//友元定义
	friend class CServiceUnits;

	//状态变量
protected:
	bool							m_bMonitorService;					//服务标识	
	bool							m_bWillConcludeService;				//预停止状态
	DWORD							m_StockUpdateID;					//更新标识
	CWordArray						m_StockUpdateArray;					//更新数组

	//变量定义
protected:
	tagServiceItem					m_MonitorItem;						//状态子项
	tagServiceOption *				m_pServiceOption;					//约战配置
	CInitParameter *				m_pInitParameter;					//配置参数
	tagBindParameter *				m_pBindParameter;					//辅助数组		

	//组件变量
protected:
	CStockGameKind					m_StockGameKind;					//游戏类型
	CShareStockManager				m_ShareStockManager;				//库存管理	
	
	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IDataBaseEngine *				m_pIDataBaseEngine;					//数据引擎
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎
	ITCPSocketService *				m_pITCPSocketCenter;				//网络服务	

	//函数定义
public:
	//构造函数
	CAttemperEngineSink();
	//析构函数
	virtual ~CAttemperEngineSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//异步接口
public:
	//启动事件
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//停止事件
	virtual bool OnAttemperEngineStop(IUnknownEx * pIUnknownEx);

	//事件接口
public:
	//控制事件
	virtual bool OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize);
	//自定事件
	virtual bool OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize);

	//内核事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);

	//连接事件
public:
	//连接事件
	virtual bool OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode);
	//关闭事件
	virtual bool OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason);
	//读取事件
	virtual bool OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize,DWORD * pdwTokenID,WORD wTokenCount);

	//网络事件
public:
	//应答事件
	virtual bool OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID);
	//关闭事件
	virtual bool OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID);
	//读取事件
	virtual bool OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//预警接口
public:
	//库存预警
	virtual VOID OnEventStockScoreAlarm(tagStockAlarmItem StockAlarmList[], WORD wItemCount);
	//彩池预警
	virtual VOID OnEventJackpotScoreAlarm(tagJackpotAlarmItem JackpotAlarmList[], WORD wItemCount);

	//更新接口
public:
	//库存更新
	virtual VOID OnEventStockScoreUpdate(tagStockVariationItem StockVariationList[], WORD wItemCount);
	//状态更新
	virtual VOID OnEventStockStateUpdate(tagStateVariationItem StateVariationList[], WORD wItemCount);
	//彩池更新
	virtual VOID OnEventJackpotScoreUpdate(tagJackpotVariationItem JackpotVariationList[], WORD wItemCount);

	//连接处理
protected:
	//注册事件
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//服务信息
	bool OnTCPSocketMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//转发服务
	bool OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//管理服务
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);	

	//数据事件
protected:
	//游戏类型
	bool OnDBResultGameKindItem(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//库存子项
	bool OnDBResultStockListItem(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//大R控制
	bool OnDBResultSuperPlayerControl(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//加载完成
	bool OnDBResultLoadConfigFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//查询结果
	bool OnDBResultQueryStockItem(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//更新库存
	bool OnDBResultUpdateStockScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//更新状态
	bool OnDBResultUpdateStockState(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//更新彩金
	bool OnDBResultUpdateJackpotScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);

	//辅助函数
protected:
	//上报彩金
	VOID ReportGameJackpot();
	//移除标识
	VOID RemoveStockUpdateID(DWORD dwUpdateID);
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);	
};

//////////////////////////////////////////////////////////////////////////

#endif