#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "TableFrame.h"
#include "InitParameter.h"
#include "ScoreManager.h"
#include "ServerUserManager.h"
#include "RobotUserManager.h"
#include "GameServiceHead.h"
#include "DistributeManager.h"

//////////////////////////////////////////////////////////////////////////

//连接类型
#define CLIENT_KIND_FALSH			0x01									//网页类型
#define CLIENT_KIND_MOBILE			0x02									//手机类型
#define CLIENT_KIND_ROBOT			0x04									//机器类型
#define CLIENT_KIND_COMPUTER		0x08									//电脑类型

//////////////////////////////////////////////////////////////////////////

//服务类型
enum enServiceKind
{
	ServiceKind_None,				//服务类型
	ServiceKind_Gate				//网关服务	
};


//绑定参数
struct tagBindParameter
{
	//连接属性
	DWORD							dwClientIP;							//连接地址
	DWORD							dwContextID;						//绑定标识
	DWORD							dwActiveTime;						//激活时间

	//服务标识
	WORD							dwServiceID;						//服务标识
	WORD							wServiceModule;						//服务模块

	//关联信息
	tagTokenParameter 				TokenParameter;						//令牌参数		
	CGlobalGateItem *				pGlobalGateItem;					//服务子项
};

//令牌参数
struct tagExtendParameter
{
	BYTE							cbClientKind;						//终端类型
	DWORD							dwPlazaVersion;						//广场版本
	DWORD							dwFrameVersion;						//框架版本
	DWORD							dwProcessVersion;					//游戏版本
	DWORD							dwPlazaStationID;					//广场站点
};

//系统参数
struct tagSystemParameter
{
	bool							bLoadFlag;							//加载标识	

	//低保参数
	SCORE							lTakeSubsidyGold;					//赠送金币
	SCORE							lTakeSubsidyCondition;				//领取条件	
};

//桌子状况
struct tagTableUserStatus
{
	WORD							wLessUserCount;						//最少人数
	WORD							wTableUserCount;					//用户数目
	WORD							wTableRobotCount;					//机器数目
};

//用户状况
struct tagRobotUserStatus
{
	WORD							wFreeUserCount;						//用户数目
	WORD							wPlayUserCount;						//用户数目
	WORD							wSitdownUserCount;					//用户数目
	IServerUserItem *				pServerUserFree[MAX_ROBOT];			//用户接口
	IServerUserItem *				pServerUserPlay[MAX_ROBOT];			//用户接口
	IServerUserItem *				pServerUserSitdown[MAX_ROBOT];		//用户接口
};

//字典定义
typedef CMap<WORD,WORD,DWORD,DWORD>	 CTableTimerMap;					//桌子索引
typedef CMap<DWORD,DWORD,SCORE,SCORE> CFreezeScoreMap;					//冻结索引
typedef CMap<WORD,WORD,DWORD,DWORD> CUserBindIDMap;						//用户索引

//数组定义
typedef CWHArray<CTableFrameItem *> CTableFrameItemArray;				//桌子数组
typedef CWHArray<tagModuleVersion> CModuleVersionArray;					//版本数组
typedef CWHArray<tagBattleGameOptionEx> CBattleGameOptionArray;			//游戏选项

//////////////////////////////////////////////////////////////////////////

//调度钩子
class CAttemperEngineSink : public IAttemperEngineSink, public IServiceFunction, public IServerUserItemSink,
	public ITableEventSink, public IGameVideoItemSink
{
	//友元定义
	friend class CServiceUnits;

	//状态变量
protected:
	bool							m_bCollectUser;						//汇总标志
	bool							m_bNeedStore;						//存储标志
	bool							m_bNeekCenter;						//中心标志
	bool							m_bNeedService;						//标志变量
	bool							m_bWillConcludeService;				//服务预停止
	
	//控制变量
protected:
	DWORD							m_dwIntervalTime;					//间隔时间
	DWORD							m_dwLastDisposeTime;				//处理时间
	
	//时间变量
protected:
	DWORD							m_dwRegularTimes;					//常规时间
	DWORD							m_dwLockTableTime;					//锁桌时间

	//绑定信息
protected:
	tagBindParameter *				m_pNormalParameter;					//绑定信息
	tagBindParameter *				m_pRobotParameter;					//绑定信息

	//配置数据
protected:
	CMD_GR_ConfigColumn				CMD_ConfigColumn;					//列表配置
	CMD_GR_ConfigServer				CMD_ConfigServer;					//房间配置	

	//约战配置
protected:	
	tagServiceItem					m_StoreServer;						//存储服务
	tagServiceItem					m_ServiceItem;						//服务子项
	CBattleGameOptionArray			m_BattleGameOptionArray;			//游戏选项	

	//录像缓存
protected:
	bool							m_bCacheVideo;						//缓存标识
	CWHDataQueue					m_VideoDataQueue;					//缓存队列

	//系统配置
protected:
	CInitParameter *				m_pInitParameter;					//配置参数
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置
	CScoreManager					m_ScoreManager;						//分数管理		
	tagSystemParameter				m_SystemParameter;					//系统参数	
	tagControlParameter				m_ControlParameter;					//控制参数
	CDistributeManager				m_DistributeManager;				//分组管理	

	//内核接口
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IAttemperEngine *				m_pIAttemperEngine;					//调度引擎
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//网络引擎
	IGameServiceManager *			m_pIGameServiceManager;				//服务管理
	IGameServiceCustomTime *		m_pIGameServiceSustomTime;			//时间配置	

	//网络接口
protected:
	ITCPSocketService *				m_pITCPSocketStore;					//存储服务
	ITCPSocketService*				m_pITCPSocketCenter;				//中心服务
	ITCPSocketService *				m_pITCPSocketService;				//类型服务

	//数据库接口
protected:
	IDataBaseEngine *				m_pIDataBaseEngine;					//内核数据

	//比赛服务
public:	
	IGameVideoManager *				m_pIGameVideoManager;				//录像管理
	IExtendServiceManager *			m_pIExtendServiceManager;			//服务管理

	//组件变量
public:	
	CWordFilter						m_WordsFilter;						//字符过滤
	
	//组件变量
protected:
	CTableFrameItemArray			m_TableFrameArray;					//桌子数组	
	CGlobalItemManager				m_GlobalItemManager;				//全局信息
	CServerUserManager				m_ServerUserManager;				//用户管理
	CRobotUserManager				m_RobotUserManager;					//用户管理
	CTableTimerMap					m_LockedTableTimes;					//锁桌时间
	CFreezeScoreMap					m_FreezeScoreMap;					//冻结积分
	CModuleVersionArray				m_ModuleVersionActive;				//模块版本

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

	//用户控制
public:
	//控制开始
	virtual bool OnEventUserControlStart(IServerUserItem * pIServerUserItem);
	//控制更新
	virtual bool OnEventUserControlUpdate(IServerUserItem* pIServerUserItem);
	//控制结束
	virtual bool OnEventUserControlFinish(IServerUserItem * pIServerUserItem);

	//用户状态
public:
	//用户积分
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem);	
	//用户排位
	virtual bool OnEventUserItemRanking(IServerUserItem * pIServerUserItem);
	//用户状态
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID);
	//用户权限
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight);
	//用户财富
	virtual VOID OnEventUserItemWealth(IServerUserItem* pIServerUserItem, tagWealthVariation& WealthVariation);
	//用户等级
	virtual VOID OnEventUserUserLevel(IServerUserItem* pIServerUserItem, BYTE cbLastLevelID, BYTE cbCurrLevelID);
	//游戏等级
	virtual VOID OnEventUserGameLevel(IServerUserItem* pIServerUserItem, BYTE cbLastLevelID, BYTE cbCurrLevelID);
	//用户背包
	virtual VOID OnEventUserItemPack(IServerUserItem* pIServerUserItem, WORD wGoodsIDList[], WORD wGoodsCount);
	//用户冻结
	virtual bool OnEventUserFreezeScore(IServerUserItem *pIServerUserItem, SCORE lFreezeScore);

	//信息接口
public:
	//获取状态
	virtual WORD GetServiceStatus();

	//网络接口
public:
	//发送数据
	virtual bool SendBatchData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据
	virtual bool SendDataToTable(WORD wCmdTable, BYTE cbClientKind, WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//发送数据
	virtual bool SendDataToGate(DWORD dwSocketID, DWORD dwTokenID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//发送数据
	virtual bool SendDataToUser(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//发送数据
	virtual bool SendDebugData(IServerUserItem* pIServerUserItem, WORD wTableID, WORD wDebugKind, WORD wSubCmdID, VOID* pData, WORD wDataSize);	

	//服务接口
public:
	//发送数据
	virtual bool SendDataToCenter(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//桌子事件
public:
	//桌子锁事件
	virtual VOID OnEventTableLocked( ITableFrameItem *pITableFrame, BOOL bTableLocked );
	//桌子状态事件
	virtual VOID OnEventTableStatus( ITableFrameItem *pITableFrame, BOOL bPlayStatus );

	//请求事件
public:
	//离线写分请求
	virtual VOID OnTableReqOfflineScore( tagOfflineScore const &OfflineScore );
	//解冻锁分请求
	virtual VOID OnTableReqUnfreezeScore( DWORD dwUserID, SCORE lUnfreezeScore );

	//消息接口
public:
	//房间消息
	virtual bool SendRoomMessage(LPCTSTR lpszMessage, WORD wType);
	//游戏消息
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//跑马灯公告
	virtual bool SendMarqeeNotice(BYTE cbNoticeKind, VOID* pNoticeContent, WORD wBufferSize);
	//房间消息
	virtual bool SendRoomMessage(DWORD dwSocketID, DWORD dwTokenID, LPCTSTR lpszMessage, WORD wType);
	//房间消息
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//游戏消息
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);	

	//通知接口
public:	
	//游戏开始
	virtual bool OnEventGameStart(ITableFrameItem* pITableFrame);
	//游戏结束
	virtual bool OnEventGameConclude(ITableFrameItem * pITableFrame);		

	//分组接口
public:
	//插入分组
	virtual bool InsertDistribute(IServerUserItem * pIServerUserItem);
	//移除分组
	virtual bool RemoveDistribute(IServerUserItem * pIServerUserItem);

	//录像接口
public:
	//录像开始
	virtual void OnEventVideoStart(IGameVideoItem * pIGameVideoItem);
	//录像停止
	virtual void OnEventVideoConclude(IGameVideoItem * pIGameVideoItem);

	//存储接口
public:
	//存储录像
	virtual bool StoreVideoData(VOID * pData,WORD wDataSize);

	//约战接口
public:
	//数值数组
	virtual bool GetOptionValueArray(WORD wEmployScene, WORD wOptionIndex, COptionValueArray & OptionValueArray);

	//功能接口
public:
	//敏感词过滤
	virtual bool FilterWords(LPCTSTR szFiltered);
	//敏感词过滤
	virtual void FilterWords(LPWSTR szFiltered, int nMsgLen);
	//发送分数
	virtual bool SendUserScore(IServerUserItem *pIServerUserItem);
	
	
	//功能函数
public:
	//效验连接
	bool EfficacySocketContext(DWORD dwSocketID,DWORD dwTokenID);

	//网络事件
protected:
	//注册服务
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//服务信息
	bool OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);	

	//网络事件
protected:	
	//游戏处理
	bool OnScoketMainGame(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//框架处理
	bool OnScoketMainFrame(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);	
	//调试处理
	bool OnTCPNetworkMainDebug(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//登录处理
	bool OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//用户处理
	bool OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//管理处理
	bool OnTCPNetworkMainManage(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);	
	//会员处理
	bool OnTCPNetworkMainMember(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//查询处理
	bool OnTCPNetworkMainQuery(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//比赛命令
	bool OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//约战命令
	bool OnTCPNetwrokMainBattle(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//网络事件
protected:	
	//机器登录
	bool OnTCPNetworkSubLogonRobot(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//手机登录
	bool OnTCPNetworkSubLogonMobile(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);	
	//电脑登录
	bool OnTCPNetworkSubLogonComputer(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);		
	//用户登出
	bool OnTCPNetworkSubLogonLogout(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//用户规则
	bool OnTCPNetworkSubUserRule(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//用户旁观
	bool OnTCPNetworkSubUserLookon(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//用户坐下
	bool OnTCPNetworkSubUserSitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//用户起立
	bool OnTCPNetworkSubUserStandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//用户聊天
	bool OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//调试数据
	bool OnTCPNetworkSubDebugPacket(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//用户私聊
	bool OnTCPNetworkSubUserWisper(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID, WORD wSubCmdID);
	//邀请用户
	bool OnSocketSubInviteUser(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//拒绝厌友
	bool OnTCPNetworkSubUserRepulseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);	
	//用户信息
	bool OnTCPNetworkSubUserRequestInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//更换桌子
	bool OnTCPNetworkSubUserSwitchTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//同桌信息
	bool OnTCPNetworkSubUserDeskmateInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
    //赠送礼物
    bool OnTCPNetworkSubUserSendGift(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//数据库事件
protected:	
	//机器信息
	bool OnDBRobotUser(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//列表子项
	bool OnDBColumnItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//配置结果
	bool OnDBConfigResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//游戏选项
	bool OnDBBattleOption(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);	
	//机器参数
	bool OnDBRobotParameter(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//低保参数
	bool OnDBSubsidyParameter(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//控制参数
	bool OnDBControlParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//登录失败
	bool OnDBLogonFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//登录成功
	bool OnDBLogonSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//用户财富
	bool OnDBWealthUpdate(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//写分结果
	bool OnDBWriteScoreResult(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//加载敏感词
	bool OnDBFilterWords(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//更新金币
	bool OnDBUpdateGold(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//写入道具
	bool OnDBWriteUserProp(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//版本信息
	bool OnDBGameVersionInfo(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);			
	//物品更新
	bool OnDBUserGoodsUpdate(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//低保结果
	bool OnDBLargessSubsidy(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//控制信息
	bool OnDBUserControlInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//控制更新
	bool OnDBUserControlUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
    //赠送成功
    bool OnDBSendGiftSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
    //赠送失败
    bool OnDBSendGiftFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
    //更新魅力
    bool OnDBUpdateLoveLiness(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	

    //操作结果
protected:
    //操作失败
    VOID SendOperateFailure(DWORD dwSocketID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe);
    //操作成功
    VOID SendOperateSuccess(DWORD dwSocketID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe, LPCTSTR pszGoodsList = NULL);
	
	//中心服务
protected:
	//注册事件
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//列表事件
	bool OnTCPSocketMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//服务信息
	bool OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize);	

	//注册结果
protected:
	//注册事件
	bool OnTCPSocketMainStoreRegister(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//注册事件
	bool OnTCPSocketMainServiceRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//连接处理
protected:			
	//汇总事件
	bool OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//查询事件
	bool OnTCPSocketMainQueryService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//广播事件
	bool OnTCPSocketMainBroadcast(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//管理服务
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);		

	//内部事件
protected:
	//用户登录
	VOID OnEventUserLogon(IServerUserItem * pIServerUserItem);
	//用户登出
	VOID OnEventUserLogout(IServerUserItem * pIServerUserItem);

	//执行功能
protected:
	//切换连接
	VOID SwitchUserItemConnect(IServerUserItem * pIServerUserItem, WORD wTargetIndex, DWORD dwTokenID, bool bMobileUser);
	//版本检查
	bool PerformCheckVersion(DWORD dwDeviceKind, DWORD dwPlazaVersion, DWORD dwClientVersion, DWORD dwSocketID, DWORD dwTokenID);

	//辅助函数
protected:
	//登录失败
	bool SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID, DWORD dwTokenID);
	//请求失败
	bool SendRequestFailure(IServerUserItem* pIServerUserItem, LPCTSTR pszDescribe, BYTE cbFailureCode);
	//用户进入
	bool SendUserEnterPacket(IServerUserItem * pIServerUserItem, DWORD dwSocketID, DWORD dwTokenID,bool bAllLinks, bool bNotifySink=false);	

	//绑定函数
public:	;
	//绑定参数
	tagBindParameter * GetBindParameter(WORD wBindIndex);	
	//获取用户
	IServerUserItem * GetServerUserItem(WORD wBindIndex,DWORD dwTokenID);	
	//令牌参数
	tagTokenParameter * GetTokenParameter(WORD wBindIndex, DWORD dwTokenID);
	//扩展参数
	tagExtendParameter* GetExtendParameter(WORD wBindIndex, DWORD dwTokenID);

	//版本函数
protected:
	//当前版本
	tagModuleVersion * GetCurrentVersion(WORD wModuleID,DWORD dwDeviceKind);
	//特殊版本
	tagModuleVersion * GetSpecialVersion(WORD wModuleID, DWORD dwDeviceKind,DWORD dwVersion);

	//辅助函数
public:
	//用户状况
	WORD GetRobotUserStatus(tagRobotUserStatus & RobotUserStatus);
	//桌子状况
	WORD GetTableUserStatus(CTableFrameItem * pTableFrame, tagTableUserStatus & TableUserStatus);

	//手机辅助
public:
	//手机立即登录
	bool MobileUserImmediately(IServerUserItem * pIServerUserItem);
	//发送信息
	bool SendTableUserToUser(IServerUserItem* pITargetUserItem, BYTE cbClientKind, DWORD dwTableUserID);
	//发送信息
	bool SendUserToTableUser(IServerUserItem* pIServerUserItem, BYTE cbClientKind, DWORD dwTableUserID);
	//设置参数
	//void SetMobileUserParameter(IServerUserItem * pIServerUserItem,BYTE dwDeviceKind,WORD wBehaviorFlags,WORD wPageTableCount);	

	//辅助函数
protected:
	//查找桌子
	WORD SearchSuitableTable(BYTE cbSearchMode,IServerUserItem * pIServerUserItem, WORD wLastTableID=INVALID_WORD);
	//查找坐下
	bool SearchTableAndSitdown(BYTE cbSearchMode,IServerUserItem * pIServerUserItem, WORD wLastTableID=INVALID_WORD);

	//构造数据
protected:	
	//用户分数
	WORD ConstructScorePacket(IServerUserItem* pIServerUserItem, VOID* pBuffer, WORD wBufferSize);
	//用户状态
	WORD ConstructStatusPacket(IServerUserItem * pIServerUserItem,VOID * pBuffer,WORD wBufferSize);	
	//用户进入
	WORD ConstructEnterPacket(IServerUserItem* pIServerUserItem, VOID* pBuffer, WORD wBufferSize, BYTE cbClientKind, bool bHideUserInfo = false);
	
	//辅助函数
protected:
	//修改配置
	bool ModifyGameServer();
	//执行分组
	bool PerformDistribute();
	//统计令牌
	DWORD CollectTokenCount();
	//分数区间
	INT GetTableScoreAreaIndex(CTableFrameItem * pTableFrame);
	//时间间隔
	SCORE GetIntervalSeconds(SYSTEMTIME stStart, SYSTEMTIME stEnd);
	//发送请求
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);		
	//效验私聊权限
	bool EfficacyUserWhisper(IServerUserItem *pISendUserItem, IServerUserItem *pIRecvUserItem);	

	//辅助函数
protected:	
	//配置桌子
	bool InitTableFrame();
	//扩展服务
	bool InitExtendService();
	//配置机器
	bool InitRobotService();
	//录像服务
	bool InitVideoService();
	//配置房间
	bool InitServerConfig();
	//配置日志
	bool InitLoggerService();
};

//////////////////////////////////////////////////////////////////////////

#endif