#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

//引入文件
#include "ClubPlazaHead.h"
#include "ClubUserManager.h"

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

	//状态变量
protected:
	bool							m_bLogicSevice;						//服务标识
	bool							m_bNeedConnStatus;					//连接标志

	//数据缓冲
protected:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//缓冲数据

	//变量定义
protected:
	tagClubOption					m_ClubOption;						//茶社配置
	tagServiceOption *				m_pLogicOption;						//服务配置	
	CInitParameter *				m_pInitParameter;					//配置参数
	tagBindParameter *				m_pBindParameter;					//辅助数组

	//组件变量
protected:	
	CWordFilter						m_WordFilter;						//词汇过滤	
	CClubUserManager				m_ClubUserManager;					//用户管理
	CGlobalItemManager				m_GlobalItemManager;				//全局管理			

	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IDataBaseEngine *				m_pIDataBaseEngine;					//数据引擎
	ITCPSocketService *				m_pITCPSocketCenter;				//网络服务
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
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);
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

	//连接处理
protected:
	//注册事件
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//用户汇总
	bool OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//列表事件
	bool OnTCPSocketMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize);		
	//管理服务
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//逻辑转发
	bool OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//网络事件
protected:
	//注册服务
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//服务状态
	bool OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//逻辑服务
	bool OnTCPNetworkMainClubPlazaService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//其他事件
protected:	
	//登录成功
	bool OnDBLogonSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBLogonFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//登录完成
	bool OnDBLogonFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//创建群组
	bool OnDBCreateClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//更新群组
	bool OnDBUpdateClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//删除群组
	bool OnDBDeleteClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//查找群组
	bool OnDBSearchClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//添加成员
	bool OnDBInsertMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//更新成员
	bool OnDBUpdateMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//删除成员
	bool OnDBDeleteMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//茶社名片
	bool OnDBClubNameCard(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//申请消息
	bool OnDBApplyMessage(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//修改积分
	bool OnDBModifyScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//群组配置
	bool OnDBClubOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//配置完成
	bool OnDBOptionFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//禁止词汇
	bool OnDBForbidWords(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//财富更新
	bool OnDBWealthUpdate(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//删除结果
	bool OnDBApplyDelete(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//检测名称
	bool OnDBDetectClubName(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//操作成功
	bool OnDBOperateSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//操作失败
	bool OnDBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);

	//辅助函数
protected:	
	//获取结构
	tagForwardHead* GetForwardHead();	
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID* pData, WORD wDataSize);
	//令牌参数
	tagTokenParameter* GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID);

	//数据发送
protected:
	//网关发送
	bool SendDataToGate(WORD wGateID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenID);
};

//////////////////////////////////////////////////////////////////////////

#endif