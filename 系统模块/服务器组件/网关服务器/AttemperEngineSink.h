#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "InitParameter.h"
#include "GlobalInfoManager.h"

//////////////////////////////////////////////////////////////////////////
//链接状态
enum emLinkState
{
	LS_NULL=0,					//空闲状态
	LS_SHAKEHAND,				//握手状态
	LS_LINKING					//链接状态
};

//链接记录
struct tagLinkRecord
{
	WORD							wTimes;								//链接次数
	WORD							wServiceID[5];						//服务标识
	WORD							wReqServiceID;						//服务标识
	BYTE							cbLinkExtend[LEN_LINK_EXTEND];		//扩展信息
};


//绑定参数
struct tagBindParameter
{	
	//网络信息	
	DWORD							dwSocketID;							//网络标识	
	DWORD							dwClientIP;							//连接地址
	DWORD							dwActiveTime;						//激活时间

	//链接信息	
	WORD							wServiceID[SERVICE_COUNT];			//服务标识
	emLinkState						LinkState[SERVICE_COUNT];			//链接状态	
	tagLinkRecord					LinkRecord[SERVICE_COUNT];			//链接记录

	//用户信息	
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonToken[LEN_LOGON_TOKEN];		//登录令牌
};

//////////////////////////////////////////////////////////////////////////

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//状态变量
protected:
	bool							m_bNeedBattle;						//验证约战

	//缓冲变量
protected:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//缓冲数据

	//配置变量
protected:	
	tagServiceOption *				m_pServiceOption;					//网关配置
	CInitParameter *				m_pInitParameter;					//配置参数
	tagBindParameter *				m_pBindParameter;					//连接信息
	tagServiceAttrib				m_ServiceAttribute[SERVICE_COUNT];	//服务参数

	//管理变量
protected:
	CGlobalInfoManager				m_GlobalInfoManager;				//管理对象

	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IUnknownEx*						m_pIAttemperEngine;					//调度引擎	
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎
	ITCPSocketService *				m_TCPSocketCenter;					//中心服务

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

	//中心连接
protected:
	//注册服务
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//列表事件
	bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);		

	//逻辑连接
protected:
	//注册服务
	bool OnTCPSocketMainRegister(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize);
	//通用服务
	bool OnTCPSocketMainCommonService(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount);
	//内部服务
	bool OnTCPSocketMainInsideService(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount);

	//网络事件
protected:	
	//网关服务
	bool OnTCPNetworkMainGateService(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//内部服务
	bool OnTCPNetworkMainInsideService(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);	

	//数据发送
protected:
	//发送消息
	bool SendDataToService(WORD wServiceIndex, WORD wServiceID, DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//分配服务
protected:
	//分配服务
	WORD AssignInsideService(WORD wServiceModule, DWORD dwSocketID);

	//辅助函数
protected:	
	//注销令牌
	VOID LogoutToken(WORD wServiceIndex, WORD wServiceID, DWORD dwTokenID);
	//关闭链接
	void ShutdownServiceLink(WORD wServiceModule, WORD wServiceID);
	//关闭链接
	void ShutdownServiceLink(WORD wServiceModule, WORD wServiceID, DWORD dwSoceketID);

	//辅助函数
protected:
	//时钟标识
	inline DWORD GetTimerID(WORD wServiceIndex, WORD wServiceID);
	//服务标识
	inline DWORD GetServiceID(WORD wServiceIndex, WORD wServiceID);

	//辅助函数
protected:	
	//在线人数
	WORD GetOnlineCount();
	//绑定参数
	tagBindParameter * GetBindParameter(WORD wBindIndex);		
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

#endif

