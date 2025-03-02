#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "BattleManager.h"
#include "GlobalInfoManager.h"
#include "BattleServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//数据定义

//服务类型
enum enServiceKind
{
	ServiceKind_None,				//无效服务
	ServiceKind_Game,				//游戏服务	
	ServiceKind_Gate,				//网关服务
};

//全局选项
struct tagBattleGlobalOption
{
	WORD							wClubMaxTableCount;					//最大桌数
	WORD							wNormalMaxTableCount;				//最大桌数	
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

//数组定义
typedef CWHArray<tagBattleGameOptionEx> CBattleGameOptionArray;			//选项数组
typedef CWHArray<tagBattleBaseOptionEx> CBattleBaseOptionArray;			//选项数组
typedef CWHArray<tagBattleGlobalOption> CBattleGlobalOptionArray;		//选项数组

//////////////////////////////////////////////////////////////////////////

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//状态变量
protected:
	bool							m_bBattleService;					//服务标识
	CWordArray						m_SynchroItemArray;					//同步数组

	//缓冲变量
protected:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//数据缓冲

	//变量定义
protected:
	tagServiceOption *				m_pServiceOption;					//约战配置
	CInitParameter *				m_pInitParameter;					//配置参数
	tagBindParameter *				m_pBindParameter;					//辅助数组
	tagBattleGlobalOption			m_GlobalOption;						//全局配置

	//存储变量
protected:
	CBattleGameOptionArray			m_BattleGameOptionActive;			//自定选项		
	CBattleGameOptionArray			m_BattleGameOptionBuffer;			//自定选项		
	CBattleBaseOptionArray			m_BattleBaseOptionActive;			//创建选项		
	CBattleBaseOptionArray			m_BattleBaseOptionBuffer;			//创建选项		

	//组件变量
protected:
	CBattleManager					m_BattleManager;					//约战管理
	CGlobalInfoManager				m_GlobalInfoManager;				//全局管理	

	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IDataBaseEngine *				m_pIDataBaseEngine;					//数据引擎
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎
	ITCPSocketService *				m_pITCPSocketCenter;				//网络服务	
	ITCPSocketService *				m_pITCPSocketBattle;				//网络服务	

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

	//连接处理
protected:
	//注册事件
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//服务信息
	bool OnTCPSocketMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//管理服务
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//转发消息
	bool OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//网络事件
protected:
	//注册服务
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//服务状态
	bool OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//约战服务
	bool OnTCPNetWorkMainGBBattleService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//约战服务
	bool OnTCPNetWorkMainBSBattleService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//约战服务
	bool OnTCPNetWorkMainBCBattleService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	

	//数据库事件
protected:	
	//操作失败
	bool OnDBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//约战选项
	bool OnDBBattleBaseOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//游戏选项
	bool OnDBBattleGameOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//全局选项
	bool OnDBBattleGlobalOption(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//选项完成
	bool OnDBBattleOptionFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);			

	//约战玩法
protected:		
	//校验玩法
	bool VerifyBattleConfig(tagBattleConfig * pBattleConfig);
	//校验配置
	bool VerifyBattleConfig(tagBattleConfig * pBattleConfig, tagBattleConfigEx & BattleConfig);	

	//辅助函数
protected:
	//添加同步
	VOID AppendSynchroItem(WORD wServerID);
	//移除同步
	VOID RemoveSynchroItem(WORD wServerID);
	// 检验桌数
	bool VerifyTableCount(DWORD dwClubID, WORD wTableCount);
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);		
	//令牌参数
	tagTokenParameter* GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID);

	//打包函数
protected:
	//构造桌子
	VOID ConstructTableItem(tagBattleTableItemEx * pTableItemEx, tagBattleTableItem * pTableItem);
	//游戏选项
	WORD ConstructGameOption(INT& nStartIndex, WORD wModuleID, BYTE cbEmployScene, BYTE cbBuffer[], WORD wBufferSize);
	//约战选项
	WORD ConstructBaseOption(INT& nStartIndex, WORD wModuleID, BYTE cbEmployScene, BYTE cbBuffer[], WORD wBufferSize);
	//构造桌子
	WORD ConstructTableList(CTableItemArray& TableItemArray, CTableParamArray& TableParamArray, INT &nStartIndex, BYTE cbBuffer[], WORD wBufferSize);

	//操作结果
protected:
	//操作失败
	VOID SendOperateFailure(WORD wServiceID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe);
	//操作失败
	VOID SendOperateFailure(WORD wServiceID, DWORD dwClubID, tagSocketInfo& SocketInfo, LONG lErrorCode, LPCTSTR pszDescribe);
	//操作成功
	VOID SendOperateSuccess(WORD wServiceID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe);
	//操作成功
	VOID SendOperateSuccess(WORD wServiceID, DWORD dwClubID, tagSocketInfo& SocketInfo, LONG lErrorCode, LPCTSTR pszDescribe);

	//数据发送
protected:
	//房间发送
	bool SendDataToGame(WORD wServerID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//茶馆发送
	bool SendDataToClub(WORD wServiceID, WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//网关发送
	bool SendDataToGate(WORD wServiceID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD * pdwTokenID=NULL, WORD wTokenCount=0);
};

//////////////////////////////////////////////////////////////////////////

#endif