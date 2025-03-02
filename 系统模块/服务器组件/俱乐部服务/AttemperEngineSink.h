#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "ClubServiceHead.h"
#include "ClubInfoManager.h"
#include "ClubUserManager.h"
#include "GlobalServiceManager.h"

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
	PVOID							pBindServiceItem;					//绑定服务
};

//////////////////////////////////////////////////////////////////////////

typedef CArray<DWORD, DWORD> CClubIDArray;
typedef CMap<DWORD,DWORD,ULONGLONG,ULONGLONG> CMapInvokeCount;		

//////////////////////////////////////////////////////////////////////////

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//状态变量
protected:
	bool							m_bClubSevice;						//茶社服务
	bool							m_bNeedConnStatus;					//连接标志

	//缓冲变量
protected:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//缓冲数据
	BYTE							m_cbDataBuffer[SOCKET_PACKET];		//缓冲数据

	//调试变量
protected:
	CClubIDArray					m_ClubIDArray;						//标识数组
	CMapInvokeCount					m_MapInvokeCount1;					//调用统计	
	CMapInvokeCount					m_MapInvokeCount2;					//调用统计		

	//变量定义
protected:
	tagServiceItem					m_BattleItem;						//约战子项	
	tagServiceOption *				m_pClubOption;						//茶社配置
	CInitParameter *				m_pInitParameter;					//配置参数
	tagBindParameter *				m_pBindParameter;					//辅助数组

	//组件变量
protected:	
	CClubInfoManager				m_ClubInfoManager;					//茶社管理	
	CClubUserManager				m_ClubUserManager;					//用户管理
	CGlobalItemManager				m_GlobalItemManager;				//全局管理
	CGlobalServiceManager			m_GlobalServiceManager;				//服务管理

	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IUnknownEx *					m_pIAttemperEngine;					//调度引擎
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
	virtual bool OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD *pdwTokenID, WORD wTokenCount);

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
	//通用消息
	bool OnTCPSocketMainComonService(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//管理服务
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//转发消息
	bool OnTCPSocketMainForwardService(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//连接处理
protected:
	//服务状态
	bool OnTCPSocketBattleMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//通用消息
	bool OnTCPSocketBattleMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//约战服务
	bool OnTCPSocketBattleMainBattleService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//网络事件
protected:
	//注册服务
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//服务状态
	bool OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);	
	//茶社服务
	bool OnTCPNetworkMainClubService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//其他事件
protected:
	//茶社列表
	bool OnDBClubList(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//列表完成
	bool OnDBClubListFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//茶社子项
	bool OnDBClubItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//子项完成
	bool OnDBClubItemFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//茶社成员
	bool OnDBClubMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//类型设置
	bool OnDBKindSetting(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//设置修改
	bool OnDBKindSettingModify(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//约战玩法
	bool OnDBBattleConfig(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//申请消息
	bool OnDBApplyMessage(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//更新成员
	bool OnDBUpdateMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//添加玩法
	bool OnDBAppendConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//修改玩法
	bool OnDBModifyConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//删除玩法
	bool OnDBDeleteConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//操作失败
	bool OnDBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);	

	//辅助函数
protected:	
	//令牌参数
	tagTokenParameter* GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID);	
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);	
	//查询桌子
	bool QueryBattleTable(CClubInfoItem* pClubInfoItem);
	//创建桌子
	bool CreateBattleTable(CClubInfoItem* pClubInfoItem, tagClubBattleConfig* pClubBattleConfig);
	//构造群名片
	bool ConstructClubNameCard(const tagClubProperty * pClubProperty, tagClubNameCard * pClubNameCard);	

	//辅助函数
protected:	
	//转换配置
	bool ConvertBattleConfig(tagBattleConfig* pBattleConfig, tagClubBattleConfig& BattleConfig);
	//转换配置
	bool ConvertBattleConfig(tagClubBattleConfig* pBattleConfig, tagBattleConfig& BattleConfig);

	//数据发送
protected:	
	//发送成员
	bool SendClubMemberToMember(DWORD dwClubID);
	//发送成员
	bool SendClubMemberToUser(DWORD dwClubID, tagSocketInfo *pUserSocketInfo);
	//发送消息
	bool SendApplyMessageToUser(DWORD dwClubID, tagSocketInfo *pUserSocketInfo);
	//发送玩法
	bool SendBattleConfigToUser(DWORD dwClubID,tagSocketInfo *pUserSocketInfo);	
	//发送桌子
	bool SendBattleTableToUser(DWORD dwClubID, tagSocketInfo* pUserSocketInfo);
	//发送设置
	bool SendKindSettingToUser(DWORD dwClubID, tagSocketInfo* pUserSocketInfo);

	//操作结果
protected:
	//操作失败
	VOID SendOperateFailure(WORD wServiceID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe);
	//操作成功
	VOID SendOperateSuccess(WORD wServiceID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe);

	//数据发送
protected:		
	//发送数据
	bool SendDataToUser(WORD wServiceModule,WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize,tagSocketInfo *pUserSocketInfo);
	//发送成员
	bool SendDataToMember(DWORD dwClubID, WORD wServiceModule, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, BYTE cbLocationMask);
	//发送网关
	bool SendDataToGate(WORD wGateID, WORD wServiceModule, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD *pdwTokenID, WORD wTokenCount);	
};

//////////////////////////////////////////////////////////////////////////

#endif