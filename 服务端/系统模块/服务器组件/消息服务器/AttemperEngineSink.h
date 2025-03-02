#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "MessageServiceHead.h"

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

//////////////////////////////////////////////////////////////////////////////////
//类型定义
typedef CWHStockData<DWORD, tagMarqueeNoticeInfo> CStockMarqueeNoticeInfo;

//////////////////////////////////////////////////////////////////////////

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//状态变量
protected:
	bool							m_bCollectUser;						//汇总标志

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
	CStockMarqueeNoticeInfo			m_StockMarqueeNoticeInfo;			//公告仓库

	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IDataBaseEngine *				m_pIDataBaseEngine;					//数据引擎
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎
	ITCPSocketService*				m_pITCPSocketCenter;				//网络服务	

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
	//汇总服务
	bool OnTCPSocketMainCollectService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//网络事件
protected:
	//注册服务
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//服务状态
	bool OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//消息服务
	bool OnTCPNetWorkMainMessageService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID,DWORD dwTokenID);	

	//数据库事件
protected:
	//邮件信息
	bool OnDBUserMailInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//邮件完成
	bool OnDBUserMailFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//邮件更新
	bool OnDBUserMailUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//插入邮件
	bool OnDBUserMailInsert(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//领取附件
	bool OnDBClaimMailAttach(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);	
	//物品信息
	bool OnDBUserGoodsUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//财富信息
	bool OnDBUserWealthUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//操作成功
	bool OnDBOperateSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//操作失败
	bool OnDBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);

	//辅助函数
protected:
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);

	//数据发送
protected:
	//发送数据
	bool SendBatchData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//数据发送
protected:
	//网关发送
	bool SendDataToGate(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount);

	//辅助函数
private:
	//清除公告
	VOID CleanExpireNotice();
	//令牌参数
	tagTokenParameter* GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID);
	//配置日志
	bool InitLoggerService();

	//操作结果
protected:
	//操作失败
	VOID SendOperateFailure(CGlobalUserItem * pGLobalUserItem, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe);
	//操作成功
	VOID SendOperateSuccess(CGlobalUserItem* pGLobalUserItem, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe, LPCTSTR pszGoodsList = NULL);
};

//////////////////////////////////////////////////////////////////////////

#endif