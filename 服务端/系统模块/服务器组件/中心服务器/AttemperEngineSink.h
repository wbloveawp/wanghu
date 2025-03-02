#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "StockDataManager.h"
#include "GlobalInfoManager.h"
#include "ServiceAttribManager.h"

//////////////////////////////////////////////////////////////////////////
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

//字典定义
typedef CMap<WORD, WORD, WORD, WORD> CMapPortOnline;

//////////////////////////////////////////////////////////////////////////

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//状态变量
protected:
	bool							m_bWillConcludeService;				//预停止状态
	WORD							m_wCollectItem;						//汇总连接	
	DWORD							m_dwCollectItem;					//汇总连接
	CWordArray						m_StockUpdateArray;					//更新数组
	CWordArray						m_WaitCollectItemArray;				//汇总等待

	//变量定义
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	tagBindParameter *				m_pBindParameter;					//辅助数组

	//组件变量
protected:
	CGlobalInfoManager				m_GlobalInfoManager;				//全局管理	
	CServiceAttribManager			m_ServiceAttribManager;				//服务管理

	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IDataBaseEngine *				m_pIDataBaseEngine;					//数据引擎
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎

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
	virtual bool OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount);

	//网络事件
public:
	//应答事件
	virtual bool OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID);
	//关闭事件
	virtual bool OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID);
	//读取事件
	virtual bool OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//网络事件
protected:
	//注册服务
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//服务状态
	bool OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//用户汇总
	bool OnTCPNetworkMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//信息查询
	bool OnTCPNetworkMainQueryService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//广播消息
	bool OnTCPNetworkMainBroadcast(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//网站通知
	bool OnTCPNetworkMasterService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//管理服务
	bool OnTCPNetWorkMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//转发服务
	bool OnTCPNetworkMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID);

	//事件处理
protected:
	//查询用户
	bool OnTCPNetwordSubQueryByGameID(VOID *pData, WORD wDataSize, DWORD dwSocketID);
	//查询用户
	bool OnTCPNetwordSubQueryByAccounts(VOID *pData, WORD wDataSize, DWORD dwSocketID);

	//数据库事件
protected:
	//配置完成
	bool OnDBConfigFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//解锁玩家
	bool OnDBUnlockPlayer(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//更新榜单
	bool OnDBUpdateRankingList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);

	//辅助函数
protected:
	//发送彩池
	bool SendGameJackpot(DWORD dwSocketID);
	//发送服务
	bool SendServiceAttrib(DWORD dwSocketID);	
	//发送列表
	bool SendServerListItem(DWORD dwSocketID);		
	//发送服务
	bool SendServiceListItem(WORD wServiceModule,DWORD dwSocketID, bool bPushRouteTable);
	//推送服务
	bool PushServiceListItem(WORD wServiceMask, DWORD dwSocketID, bool bPushRouteTable);
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);

	//数据发送
protected:
	//用户发送
	bool SendDataToUser(WORD wServerID, DWORD dwUserID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//数据发送
protected:	
	//服务数据
	bool SendDataToService(WORD wServiceModule, WORD wServiceID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//服务数据
	bool SendDataToService(WORD wServiceModule, tagForwardItem& ForwardItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);		
	//发送数据
	bool SendDataToAllService(WORD wServiceModule, WORD wExcludeID, WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//辅助函数
protected:
	//更新代理
	VOID UploadAgentList();
	//更新网关
	VOID UpdateGateList();

	//房间管理
protected:
	//更新房间
	VOID UpdateServerItem(CGlobalServerInfo* pGlobalServerInfo, BYTE cbUpdateKind);
	//格式化房间
	VOID FormatServerItem(CGlobalServerInfo* pGlobalServerInfo, CStringW& strServerItem);

};

//////////////////////////////////////////////////////////////////////////

#endif