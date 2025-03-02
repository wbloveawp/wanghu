#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "InitParameter.h"
#include "ServerListManager.h"
#include "DataBasePacket.h"
#include "RankingListManager.h"

//////////////////////////////////////////////////////////////////////////////////
//数据定义

//连接类型
#define CLIENT_KIND_FALSH			1									//网页类型
#define CLIENT_KIND_MOBILE			2									//手机类型
#define CLIENT_KIND_COMPUTER		3									//电脑类型

//////////////////////////////////////////////////////////////////////////

//绑定参数
struct tagBindParameter
{
	//连接信息
	DWORD							dwSocketID;							//网络标识	
	DWORD							dwActiveTime;						//激活时间

	//服务标识
	WORD							wServiceID;							//服务标识
	WORD							wServiceModule;						//服务模块

	//网关信息
	CGlobalGateItem *				pGlobalGateItem;					//网关子项
};

//扩展参数
struct tagExtendParemter
{
	WORD							wModuleID;							//模块标识
	BYTE							cbClientKind;						//连接类型
	DWORD							dwDeviceKind;						//设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//大厅版本

	//禁止游戏
	WORD							wKindCount;							//类型数量
	WORD							wShieldKindList[16];				//类型列表
};

//转账参数
struct tagTransferParameter
{
	bool							bTransferEnabled;					//转账开关
	WORD 							wServiceFeeRate;					//服务费率
	SCORE							lReservedAmount;					//保留金额
	SCORE							lMinTransferAmount;					//保留金额
	SCORE							lMaxTransferAmount;					//保留金额
};

//佣金参数
struct tagCommissionParameter
{
	bool							bDrawEnabled;						//提取开关
	WORD 							wServiceFeeRate;					//服务费率
	SCORE							lMinDrawAmount;						//最小金额	
};

//昵称配置
struct tagNickNameOption
{
	WORD							wFreeModifyTimes;					//免费修改次数
	WORD							wDailyModifyTimes;					//每日可修改次数
	DWORD							dwGoodsID;							//消耗物品ID
	DWORD							dwGoodsCount;						//消耗物品数量
};

//类型定义
typedef CMap<DWORD, DWORD, WORD, WORD> CMapUserLevel;					//用户等级
typedef CWHArray<tagModuleVersion>	CModuleVersionArray;				//版本数组																		

//////////////////////////////////////////////////////////////////////////

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink
{
	//友元定义
	friend class CServiceUnits;

	//配置变量
protected:		
	DWORD							m_dwPCFuncption;					//功能选项
	DWORD							m_dwMBFuncption;					//功能选项
	DWORD							m_dwShowOnLineInfo;					//显示配置
	tagServiceOption *				m_pServiceOption;					//服务配置
	CInitParameter *				m_pInitParameter;					//配置参数
	tagBindParameter *				m_pBindParameter;					//连接信息
	tagTransferParameter			m_TransferParameter;				//转账参数	
	tagNickNameOption				m_NickNameOption;					//昵称配置

	//开桌配置
protected:
	CModuleVersionArray				m_ModuleVersionActive;				//模块版本

	//版本配置
protected:
	BOOL							m_bUpdateSilent;					//后台升级

	//辅助变量
protected:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//发送缓冲

	//组件变量
protected:
	CWordFilter						m_WordsFilter;						//词汇过滤
	CGlobalItemManager				m_GlobalItemManager;				//服务管理
	CServerListManager				m_ServerListManager;				//列表管理
	CRankingListManager				m_RankingListManager;				//榜单管理

	//组件接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IDataBaseEngine *				m_pIDataBaseEngine;					//数据引擎
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎
	ITCPSocketService *				m_pITCPSocketCenter;				//中心服务

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

	//连接处理
protected:
	//注册服务
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//列表事件
	bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//用户收集
	bool OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//转发服务
	bool OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//管理服务
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);	

	//网络事件
protected:
	//注册服务
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//服务信息
	bool OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//网络事件
protected:
	//登录处理
	bool OnTCPNetworkMainPCLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//列表处理
	bool OnTCPNetworkMainPCServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//服务处理
	bool OnTCPNetworkMainPCUserService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//配置服务
	bool OnTCPNetworkMainPCConfigService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//机器服务
	bool OnTCPNetworkMainPCRobotService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//库存服务
	bool OnTCPNetworkMainPCStockService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//管理服务
	bool OnTCPNetworkMainPCManagerService(WORD wSubCmdID, VOID *pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);	

	//手机事件
protected:
	//登录处理
	bool OnTCPNetworkMainMBLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//列表处理
	bool OnTCPNetworkMainMBServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//用户服务
	bool OnTCPNetworkMainMBUserService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//网络事件
protected:
	//游客登录
	bool OnTCPNetworkSubPCLogonByGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//帐号登录
	bool OnTCPNetworkSubPCLogonByAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//工具登录
	bool OnTCPNetworkSubPCLogonManageTool(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//帐号注册
	bool OnTCPNetworkSubPCRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	
	//手机登录
protected:	
	//游客登录
	bool OnTCPNetworkSubMBLogonVisitor(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//秘钥登录
	bool OnTCPNetworkSubMBLogonSecretKey(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//帐号登录
	bool OnTCPNetworkSubMBLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//验证码登录
	bool OnTCPNetworkSubMBLogonMobileCode(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//第三方登录
	bool OnTCPNetworkSubMBLogonThirdParty(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//手机登录
	bool OnTCPNetworkSubMBLogonMobilePhone(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//邮箱注册
	bool OnTCPNetworkSubMBRegisterEMail(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//帐号注册
	bool OnTCPNetworkSubMBRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);	
	//手机注册
	bool OnTCPNetworkSubMBRegisterMobilePhone(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);	
	//邮箱登录
	bool OnTCPNetworkSubMBLogonEMail(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//验证码登录
	bool OnTCPNetworkSubMBLogonEMailCode(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//游戏事件
protected:
	//登录成功
	bool OnDBPCLogonSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBPCLogonFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//用户信息
	bool OnDBPCUserIndividual(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//操作成功
	bool OnDBPCOperateSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//操作失败
	bool OnDBPCOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//基础配置
	bool OnDBPCGameBasicConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//权重配置
	bool OnDBPCGameWeightConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//配置完成
	bool OnDBPCGameConfigFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//操作结果
	bool OnDBPCRobotOperateResult(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);		
	//操作结果
	bool OnDBPCStockOperateResult(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);

	//手机事件
protected:
	//登录成功
	bool OnDBMBLogonSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//登录失败
	bool OnDBMBLogonFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//登出结果
	bool OnDBMBLogoutResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//用户头像
	bool OnDBMBUserFaceInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//用户缓冲
	bool OnDBMBUserCacheInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//行为掩码
	bool OnDBMBUserActionMask(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//排位信息
	bool OnDBMBUserRankingInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);	
	//绑定成功
	bool OnDBMBBindMPSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//解绑成功
	bool OnDBMBUnbindMPSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//获取结果
	bool OnDBMBAcquireACResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//绑定成功
	bool OnDBMBBindEMailSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//解绑成功
	bool OnDBMBUnbindEMailSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//背包物品
	bool OnDBMBUserPackGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//物品更新
	bool OnDBMBUserGoodsUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//财富更新
	bool OnDBMBUserWealthUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
    //魅力更新
    bool OnDBMBUserLoveLinessUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//投递消息
	bool OnDBMBPostMessage(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//操作成功
	bool OnDBMBOperateSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//操作失败
	bool OnDBMBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//兑换商品
	bool OnDBMBExchangeMallGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//兑换礼包码
	bool OnDBMBExchangeGiftbagCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//签到结果
	bool OnDBMBCheckInResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
    //绑定邀请码
    bool OnDBMBBindInviteCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//绑定账号
	bool OnDBMBBindExternalAccount(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//礼物信息
	bool OnDBMBUserGiftInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
    //赠送礼物成功
    bool OnDBMBSendGiftSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);   
    //礼物信息完成
    bool OnDBMBUserGiftInfoFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//转账成功
	bool OnDBMBWealthTransferSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);		
	//转盘抽奖数据
	bool OnDBMBTurntableLotteryData(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//转盘抽奖结果
	bool OnDBMBTurntableLotteryResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//银行查询结果
	bool OnDBMBInsureQueryResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//银行成功
	bool OnDBMBInsureSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//银行失败
	bool OnDBMBInsureFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//银行邮件通知
	bool OnDBMBInsureMailNotify(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//低保查询
	bool OnDBMBSubsidyQuery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//昵称配置
	bool OnDBMBNickNameOption(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);

	//列表事件
protected:
	//游戏种类
	bool OnDBGameTypeItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//游戏类型
	bool OnDBGameKindItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//游戏节点
	bool OnDBGameNodeItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//游戏定制
	bool OnDBGamePageItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//游戏过滤
	bool OnDBGameFilterItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	

	//其他事件
protected:
	//配置结果
	bool OnDBConfigResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//全局配置
	bool OnDBGlobalOption(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//敏感词信息
	bool OnDBFilterWordsInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//榜单数据
	bool OnDBRankingListData(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//转账参数
	bool OnDBTransferParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//版本信息
	bool OnDBModuleVersionInfo(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);		
	//绑定机器
	bool OnDBBindMachineResult(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	

	//校验函数
protected:
	//生成验证码
	VOID MakeAuthCode(WCHAR szAutoCode[], BYTE cbBufferSize);

	//发送列表
private:
	//发送类型
	VOID SendGameTypeInfo(DWORD dwSocketID, DWORD dwTokenID);
	//发送节点
	VOID SendGameNodeInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wKindID);
	//发送定制
	VOID SendGamePageInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wKindID);
	//发送种类
	VOID SendGameKindInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wSupportMask);
	//发送房间
	VOID SendGameServerInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wKindID, WORD wClientMask);

	//手机列表
protected:		
	//发送类型
	VOID SendMobileKindInfo(DWORD dwSocketID, DWORD dwTokenID, tagExtendParemter* pExtendParemter);
	//发送奖金
	VOID SendMobileJackpotInfo(DWORD dwSocketID, DWORD dwTokenID, tagExtendParemter* pExtendParemter);
	//发送在线
	VOID SendMobileOnlineInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wModuleID, tagExtendParemter* pExtendParemter);	
	//发送房间
	VOID SendMobileServerInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wModuleID, tagExtendParemter* pExtendParemter);

	//发送数据
protected:
	//发送物品
	VOID SendUserPackGoods(DWORD dwSocketID, DWORD dwTokenID, tagGoodsHoldInfo GoodsHoldInfo[], WORD wGoodsCount);
	//发送昵称配置
	VOID SendNickNameOption(DWORD dwSocketID, DWORD dwTokenID,WORD wModifyTime,WORD wDailyModifyTimes);

	//连接参数
protected:
	//绑定参数
	tagBindParameter * GetBindParameter(DWORD dwSocketID);
	//令牌参数
	tagTokenParameter * GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID);

	//发送函数
protected:
	//群发数据
	bool SendBatchData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//发送数据
	bool SendDataToGate(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, DWORD* pdwTokenID, WORD wTokenCount);
	//发送数据
	bool SendDataToGate(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD* pdwTokenID, WORD wTokenCount);    

	//操作结果
protected:
	//操作失败
	VOID SendOperateFailure(DWORD dwSocketID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe);
	//操作成功
	VOID SendOperateSuccess(DWORD dwSocketID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe, LPCTSTR pszGoodsList = NULL);

	//辅助函数
protected:
	//日期切换
	VOID OnEventDaySwitch();
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);
	//跑马灯公告
	bool SendMarqeeNotice(BYTE cbNoticeKind, VOID* pNoticeContent, WORD wBufferSize);
	//版本检测
	bool CheckPlazaVersion(DWORD dwDeviceKind, DWORD dwPlazaVersion, DWORD dwSocketID, DWORD dwTokenID);
	//当前版本
	tagModuleVersion * GetCurrentVersion(WORD wModuleID,DWORD dwDeviceKind);
	//特殊版本
	tagModuleVersion * GetSpecialVersion(WORD wModuleID,DWORD dwDeviceKind,DWORD dwVersion);	
};

//////////////////////////////////////////////////////////////////////////

#endif