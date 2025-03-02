#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "QueryServiceHead.h"
#include "DataCacheEngineSink.h"

//////////////////////////////////////////////////////////////////////////
//数据定义

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

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//缓冲变量
protected:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//数据缓冲			

	//变量定义
protected:	
	tagServiceOption *				m_pServiceOption;					//服务配置
	CInitParameter *				m_pInitParameter;					//配置参数
	tagBindParameter *				m_pBindParameter;					//辅助数组	

	//组件变量
protected:
	CGlobalItemManager				m_GlobalItemManager;				//全局管理	

	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IDataBaseEngine *				m_pIDataCacheEngine;				//数据引擎
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

	//网络事件
public:
	//应答事件
	virtual bool OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID);
	//关闭事件
	virtual bool OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID);
	//读取事件
	virtual bool OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//连接事件
public:
	//连接事件
	virtual bool OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode);
	//关闭事件
	virtual bool OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason);
	//读取事件
	virtual bool OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD *pdwTokenID, WORD wTokenCount);

	//连接处理
protected:
	//注册事件
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//服务信息
	bool OnTCPSocketMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//管理服务
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//转发服务
	bool OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//网络事件
protected:
	//注册服务
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//服务状态
	bool OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//聊天服务
	bool OnTCPNetWorkMainQueryService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID,DWORD dwTokenID);	

	//数据库事件
protected:
	//用户列表
	bool OnDBUserInfoList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//好友列表
	bool OnDBFriendInfoList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//附加列表
	bool OnDBFriendExtrasList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//同步结果
	bool OnDBSyncUserInfoResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//查询结果
	bool OnDBQueryUserInfoResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//查询结果
	bool OnDBQueryBlackListResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//查询结果
	bool OnDBQueryFriendListResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//查询结果
	bool OnDBQueryFriendInfoResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//查询结果
	bool OnDBQueryFriendExtrasResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//令牌结果
	bool OnDBBuildAccessTokenResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	
	//辅助函数
protected:
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);

	//数据发送
protected:
	//网关发送
	bool SendDataToGate(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount);

	//辅助函数
protected:
	//令牌参数
	inline tagBindParameter* GetBindParameter(DWORD dwSocketID);
};

//////////////////////////////////////////////////////////////////////////

#endif