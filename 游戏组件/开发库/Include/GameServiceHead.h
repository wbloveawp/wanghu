#ifndef GAME_SERVICE_HEAD_HEAD_FILE
#define GAME_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台定义
#include "Platform.h"
#include "BattleRule.h"
#include "RightDefine.h"
#include "BrowserDef.h"
#include "ServerRule.h"

//消息定义
#include "CMD_Commom.h"
#include "CMD_Validate.h"
#include "CMD_CenterServer.h"
#include "CMD_GameServer.h"
#include "CMD_StoreServer.h"
#include "CMD_MessageServer.h"

//平台文件
#include "CommonServiceHead.h"
#include "ServiceFrameHead.h"
#include "KernelEngineHead.h"
#include "MonitorServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef GAME_SERVICE_CLASS
#ifdef  GAME_SERVICE_DLL
#define GAME_SERVICE_CLASS _declspec(dllexport)
#else
#define GAME_SERVICE_CLASS _declspec(dllimport)
#endif
#endif

//模块定义
#ifndef _DEBUG
	#define GAME_SERVICE_DLL_NAME	TEXT("GameService.dll")				//组件 DLL 名字
#else
	#define GAME_SERVICE_DLL_NAME	TEXT("GameServiceD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////
//类型说明

//用户背包
class CServerUserPack;

//////////////////////////////////////////////////////////////////////////////////
//用户接口

//用户接口
interface IServerUserItem;
interface IServerUserManager;
interface IServerUserItemSink;

//机器人接口
interface IRobotUserItem;
interface IRobotUserItemSink;
interface IRobotUserManager;

//桌子接口
interface ITableFrameItem;
interface ITableFrameSink;
interface ITableUserAction;
interface ITableEventSink;

//录像接口
interface IGameVideoItem;
interface IGameVideoManager;

//比赛接口
interface IMatchEventSink;

//服务接口
interface IServiceUnits;
interface IServiceFunction;
interface IGameServiceManager;
interface IGameServiceCustomRule;
interface IGameServiceCustomTime;
interface IGameServiceDynamicRule;

//其他接口
interface IGameBalanceControl;

//////////////////////////////////////////////////////////////////////////
//常量定义

//机器动作
#define ROBOT_WAITSTANDUP			0x01								//等待起立
#define ROBOT_WAITLEAVE				0x02								//等待离开	

//游戏行为
#define ROBOT_BEBANKER				0x0100								//机器坐庄	
#define ROBOT_OCCUPYSEAT			0x0200								//机器占位

//结束原因
#define GER_NORMAL					0x00								//常规结束
#define GER_DISMISS					0x01								//游戏解散
#define GER_USER_LEAVE				0x02								//用户离开
#define GER_NETWORK_ERROR			0x03								//网络错误

//结束原因
#define BFR_NORMAL					0									//常规结束
#define BFR_SYSTEM					1									//系统原因
#define BFR_OFFLINE_EXPIRE			2									//掉线超时
#define BFR_MANAGER_DISMISS			3									//管理员解散
#define BFR_AGREEMENT_DISMISS		4									//协议解散
#define BFR_DEDUCT_COST_FAILED		5									//扣费失败

//同步目标
#define SYNC_TARGET_CLIENT			0x01								//同步客户端
#define SYNC_TARGET_DATABASE		0x02								//同步数据库
#define SYNC_TARGET_GAMESERVER		0x04								//同步游戏

//群发掩码
#define BG_MOBILE					(BYTE)(0x01)						//手机群发
#define BG_COMPUTER					(BYTE)(0x02)						//电脑群发
#define BG_ALL_CLIENT				(BYTE)(0xFF)						//全体群发

//索引定义
#define INDEX_SOCKET				(WORD)(0x0000)						//网络索引
#define INDEX_ROBOT					(WORD)(0x2000)						//机器索引
#define INVALID_WORD_INDEX			(WORD)(0xFFFF)						//无效索引

#define MAX_MATCH_COUNT				(1024/(sizeof(ULARGE_INTEGER)*2))	//比赛场次

//创建函数
#define GAME_SERVICE_CREATE_NAME	"CreateGameServiceManager"			//创建函数

//////////////////////////////////////////////////////////////////////////
//时间标识

//调度时间
#define IDI_ATTEMPER_SINK_START		1									//起始标识
#define IDI_ATTEMPER_SINK_RANGE		99									//标识范围

//数据库数据
#define IDI_DATA_BASE_SINK_START	100									//起始标识
#define IDI_DATA_BASE_SINK_RANGE	199									//标识范围

//机器时间
#define IDI_ROBOT_MANAGER_START		200									//起始标识
#define IDI_ROBOT_MANAGER_RANGE		299									//标识范围

//扩展范围
#define IDI_EXTEND_MODULE_START		300									//起始标识
#define IDI_EXTEND_MODULE_RANGE		5999								//结束标识

//桌子时间
#define IDI_TABLE_FRAME_START		20000								//起始标识
#define IDI_TABLE_FRAME_RANGE		50000								//标识范围

//////////////////////////////////////////////////////////////////////////
//时间标识

//桌子时间
#define TIME_TABLE_FRAME_RANGE		100									//标识范围

//游戏时间
#define TIME_TABLE_SINK_RANGE		50									//标识范围
#define TIME_TABLE_KERNEL_RANGE		30									//标识范围
#define TIME_TABLE_EXTEND_RANGE		20									//标识范围

//比赛时间
#define TIME_MATCH_FRAME_RANGE		160									//标识范围
#define TIME_MATCH_GROUP_RANGE		10									//标识范围

//机器时间
#define IDI_ROBOT_USER_TIEM			1									//起始标识
#define IDI_ROBOT_ITEM_SINK			1									//起始标识

//////////////////////////////////////////////////////////////////////////
//数据标识
#define DB_BASICS_REQUEST_START		0									//起始标识
#define DB_BASICS_RESPOND_START		1000								//起始标识
#define DB_EXTEND_REQUEST_START		2000								//起始标识
#define DB_EXTEND_RESPOND_START		3000								//起始标识

//////////////////////////////////////////////////////////////////////////
//时间常量

//系统时间
#define TIME_AMORTIZE_DISPOSE		30L									//缓冲时间
#define TIME_CONNECT_STORE			3L									//重连时间
#define TIME_CONNECT_SERVICE		3L									//重连时间
#define TIME_CONNECT_MONITOR		5L									//重连时间			
#define TIME_CONNECT_VALIDATE		5L									//重连时间
#define TIME_CONNECT_CENTER			5L									//重连时间
#define TIME_REPORT_SERVER_INFO		30L									//上报时间

//辅助时间
#ifdef _DEBUG

#define TIME_LOAD_ROBOT_INFO		15L									//加载用户
#define TIME_DISTRIBUTE_ROBOT		15L									//分配用户
#define TIME_LOAD_SYSTEM_MESSAGE	900L								//系统消息
#define TIME_SEND_SYSTEM_MESSAGE	480L								//系统消息
#define TIME_WAIT_USER_SCORE		30L									//写分等待

#else

#define TIME_LOAD_ROBOT_INFO		15L									//加载用户
#define TIME_DISTRIBUTE_ROBOT		15L									//分配用户
#define TIME_LOAD_SYSTEM_MESSAGE	900L								//系统消息
#define TIME_SEND_SYSTEM_MESSAGE	480L								//系统消息
#define TIME_WAIT_USER_SCORE		30L									//写分等待

#endif

//离开时间
#define TIME_ROBOT_LEAVE_MIN		2									//离开时间
#define TIME_ROBOT_LEAVE_MAX		10									//离开时间

//起立时间
#define TIME_ROBOT_STANDUP_MIN		2									//起立时间
#define TIME_ROBOT_STANDUP_MAX		5									//起立时间

//取款时间
#define TIME_ROBOT_INSURE_MIN		10									//取款时间
#define TIME_ROBOT_INSURE_MAX		20									//取款时间

//开始游戏
#define TIME_ROBOT_STARTGAME_MIN	3									//开始时间
#define TIME_ROBOT_STARTGAME_MAX	6									//开始时间

//分组模式
#define DISTRIBUTE_MODE_IMMEDIATELY 0									//立即模式
#define DISTRIBUTE_MODE_DELAYWAIT	1									//延时模式

//分组选项
#define DISTRIBUTE_SAME_TABLE		0x01								//同桌选项
#define DISTRIBUTE_SAME_ADDRESS		0x02								//地址选项
#define DISTRIBUTE_SAME_MACHINE		0x04								//机器选项
#define DISTRIBUTE_PLAYER_FIRST		0x08								//玩家优先

//机器动作
#define ROBOT_ACTION_LEAVE			1									//离开房间
#define ROBOT_ACTION_STANDUP		2									//起立动作
#define ROBOT_ACTION_INSURE			3									//银行取款
#define ROBOT_ACTION_DISMISS		4									//投票解散
#define ROBOT_ACTION_STARTGAME		5									//开始游戏

//////////////////////////////////////////////////////////////////////////
//枚举定义

//分数类型
enum enScoreType
{
	ScoreType_Null,					//无效
	ScoreType_Win,					//胜局
	ScoreType_Lose,					//输局
	ScoreType_Draw,					//和局
	ScoreType_Flee,					//逃局
	ScoreType_Present,				//赠送
	ScoreType_Service				//服务费
};

//开始模式
enum enStartMode
{
	StartMode_AllReady,				//所有准备
	StartMode_Symmetry,				//对称开始
	StartMode_FullReady,			//满人开始
	StartMode_TimeControl,			//时间控制
	StartMode_MasterControl,		//管理控制
};

//////////////////////////////////////////////////////////////////////////
//结构定义

//服务属性
struct tagGameServiceAttrib
{
	//内核属性
	WORD							wKindID;							//名称号码
	WORD							wChairCount;						//椅子数目
	WORD							wSupporType;						//支持类型
	BYTE							cbDynamicJoin;						//动态加入
	BYTE							cbSegmentScore;						//段位积分
	TCHAR							szGameName[LEN_KIND];				//游戏名字

	//服务属性
	DWORD							dwServerVersion;					//游戏版本
	DWORD							dwClientVersion;					//游戏版本
	TCHAR							szDataBaseName[32];					//游戏库名
	TCHAR							szServerDLLName[LEN_PROCESS];		//进程名字
	TCHAR							szClientEXEName[LEN_PROCESS];		//进程名字
};

//服务配置
struct tagGameServiceOption
{
	//列表属性
	WORD							wKindID;							//挂接类型
	WORD							wNodeID;							//挂接节点
	WORD							wSortID;							//排列标识
	WORD							wVisibleMask;						//可视掩码

	//房间属性
	WORD							wServerID;							//房间号码
	WORD							wTableCount;						//桌子数目
	WORD							wServerType;						//房间类型
	WORD							wServerPort;						//房间端口
	WORD							wServerLevel;						//房间等级
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	BYTE							cbMatchType;						//比赛类型
	WORD							wDBListID;							//数据库索引	

	//积分配置
	LONG							lRevenue;							//税收比例
	LONG							lExpGainRate;						//经验增益
	SCORE							lCellScore;							//单位积分
	SCORE							lLessScore;							//最低积分
	SCORE							lRestrictScore;						//限制积分
	SCORE							lMinEnterScore;						//最低进入
	SCORE							lMaxEnterScore;						//最高进入
	LONG							lMinEnterMember;					//最低进入会员
	LONG							lMaxEnterMember;					//最高进入会员	

	//锁定配置
	SCORE							lMaxLockScore;						//最大锁定
	SCORE							lLessLockScore;						//最少锁定

	//房间配置
	WORD							wMaxPlayer;							//最大数目
	DWORD							dwServerRule;						//房间规则
	DWORD							dwAttachUserRight;					//附加权限

	//扩展配置
	BYTE							cbCustomRule[1024];					//自定规则
	WORD							wLessStartPlayer;					//最少人数 	

	//约战规则
	DWORD							dwBattleRule;						//约战规则

	//配桌设置
	BYTE							cbDistributeMode;					//分组模式
	BYTE							cbDistributeRule;					//配桌规则
	WORD							wMinPlayerCount;					//玩家人数
	WORD							wMinDistributeUser;					//最少人数
	WORD							wDistributeTimeSpace;				//分组间隔	
	WORD							wMinPartakeGameUser;				//最少人数
	WORD							wMaxPartakeGameUser;				//最多人数

	//服务信息
	TCHAR							szServiceAddr[LEN_ADDRESS];			//服务地址
	TCHAR							szServiceDomain[LEN_DOMAIN];		//服务域名
};

//服务参数
struct tagExtendServiceParameter
{
	//配置参数
	tagGameServiceOption*			pGameServiceOption;					//服务配置
	tagGameServiceAttrib*			pGameServiceAttrib;					//服务属性

	//内核组件
	ITimerEngine*					pITimerEngine;						//时间引擎
	IDataBaseEngine*				pIDataBaseEngine;					//数据引擎	
	ITCPNetworkEngine*				pTCPNetworkEngine;					//网络引擎
	ITCPSocketService*				pITCPSocketService;					//比赛服务
	ITCPSocketService*				pITCPSocketCenterServer;			//中心服务	

	//服务组件
	IRobotUserManager*				pIRobotUserManager;					//机器管理	
	IServiceFunction*				pIMainServiceFrame;					//服务框架
	IServerUserManager*				pIServerUserManager;				//用户管理	
};

//桌子参数
struct tagTableFrameParameter
{
	//内核组件
	ITimerEngine *					pITimerEngine;						//时间引擎
	ITCPSocketService *				pITCPSocketService;					//网络服务
	IDataBaseEngine *				pIAssistDataBaseEngine;				//数据引擎	
	IDataBaseEngine*				pIDataBaseEngineEngine;				//数据引擎	

	//服务组件
	IServiceFunction *				pIServiceFunction;					//服务框架
	IGameVideoManager *				pIGameVideoManager;					//录像管理
	IGameServiceManager *			pIGameServiceManager;				//服务管理
	IRobotUserManager *				pIRobotUserManager;					//机器管理	

	//回调组件
	ITableEventSink *				pITableEventSink;					//桌子事件

	//配置变量
	tagGameServiceAttrib *			pGameServiceAttrib;					//桌子属性
	tagGameServiceOption *			pGameServiceOption;					//配置参数
};

//扩展调度参数
struct tagExtendDBSinkParameter
{
	//游戏数据库
	CDataBaseAide *					pGameDBAide;						//游戏数据库
	IDataBase*						pIGameDBModule;						//游戏数据库

	//账号数据库
	CDataBaseAide *					pAccountsDBAide;					//账号数据库
	IDataBase*						pIAccountsDBModule;					//账号数据库

	//平台数据库
	CDataBaseAide *					pPlatformDBAide;					//游戏数据库
	IDataBase*						pIPlatformDBModule;					//游戏数据库

	//配置变量
	tagGameServiceAttrib*			pGameServiceAttrib;					//桌子属性
	tagGameServiceOption*			pGameServiceOption;					//配置参数

	//组件变量
	ITimerEngine*					pITimerEngine;						//定时引擎
	IDataBaseEngine*				pIDataBaseEngine;					//数据引擎
	IDataBaseEngineEvent*			pIDataBaseEngineEvent;				//数据事件
};

//控制参数
struct tagControlParameter
{
	bool							bControlEnabled;					//控制开关
	LONG							lBenchmarkValue;					//基准分值	
	WORD							wWLControlRate;						//控制胜率
	WORD							wBLControlRate;						//控制胜率
	WORD							wInWhiteListRate;					//进入胜率
	WORD							wInBlackListRate;					//进入胜率	
	WORD							wOutWhiteListRate;					//退出胜率
	WORD							wOutBlackListRate;					//退出胜率
	WORD							wStartPlayCount;					//豁免局数
	WORD							wSupportControlMode;				//控制模式
};

//控制子项
struct tagUserControlItem
{
	BYTE							cbControlMode;						//控制模式
	BYTE							cbControlKind;						//控制类型
	BYTE							cbControlStatus;					//控制状态
	WORD							wTargetWinRate;						//目标胜率
	WORD							wControlWinRate;					//控制胜率	
	LONG							lTotalWinCount;						//合计赢局
	LONG							lTotalLoseCount;					//合计输局
	SCORE							lTotalWinScore;						//合计赢分
	SCORE							lTotalLoseScore;					//合计输分
	WORD							wControlPlayCount;					//游戏局数
	DWORD							dwControlDuration;					//持续时长	
	DWORD							dwControlStartTime;					//起始时间	
};

//财富变更
struct tagWealthVariation
{
	WORD							wReason;							//变更原因
	WORD							wMoneyID;							//货币标识
	SCORE							lBefore;							//货币余额
	SCORE							lBalance;							//货币余额
	BYTE							cbSyncTarget;						//同步目标
	SCORE							lVariationScore;					//变更分数	
	SYSTEMTIME						VariationTime;						//变更时间		
};

//物品变更
struct tagGoodsVariation
{
	WORD							wReason;							//变更原因
	WORD							wGoodsID;							//物品标识
	WORD							wGoodsKind;							//物品类型
	DWORD							dwBeforeCount;						//物品数量	
	LONG							lVariationCount;					//变更数量
	SYSTEMTIME						VariationTime;						//变更时间	
};

//////////////////////////////////////////////////////////////////////////
//辅助结构

//积分信息
struct tagScoreInfo
{
	SCORE							lScore;								//游戏积分
	SCORE							lRevenue;							//游戏税收
	LONG							lExperience;						//游戏经验
	enScoreType						ScoreType;							//分数类型		
	SCORE							lBetAmount;							//下注分数
};

//锁定信息
struct tagLockScoreInfo
{
	SCORE							lMaxLockScore;						//最大锁定
	SCORE							lLessLockScore;						//最少锁定
	SCORE							lRestrictScore;						//限制积分
};

//积分信息
struct tagVariationScore
{
	SCORE							lRevenue;							//游戏税收	
	SCORE							lUnLockScore;						//释放积分
	DWORD							dwPlayTimeCount;					//游戏时长
	tagServerUserScore				VariationScore;						//变更积分
};

//积分子项
struct tagScoreRecordItem
{	
	DWORD							dwUserID;							//用户标识
	WORD							wChairID;							//用户方位	
	SCORE							lScore;								//用户分数
	SCORE							lRevenue;							//用户税收
	SCORE							lBetAmount;							//下注金额
	SCORE							lBeforScore;						//用户分数
};

//用户规则
struct tagServerUserRule
{
	//规则标志
	bool							bLimitSameIP;						//效验地址
	bool							bLimitWinRate;						//限制胜率
	bool							bLimitFleeRate;						//限制断线
	bool							bLimitGameScore;					//限制分数

	//规则属性
	WORD							wMaxFleeRate;						//最高逃率
	WORD							wLessWinRate;						//最低胜率
	LONG							lMaxGameScore;						//最高分数 
	LONG							lLessGameScore;						//最低分数
	TCHAR							szPassword[LEN_PASSWORD];			//桌子密码
};

//用户信息
struct tagUserInfoPlus
{
	//属性变量
	WORD							wBindIndex;							//绑定索引
	DWORD							dwTokenID;							//令牌数量
	DWORD							dwClientIP;							//连接地址

	//积分信息
	SCORE							lFreeScore;							//自由积分
	LONG							lLockServiceScore;					//锁定服务费
	SCORE							lFreezeScore;						//冻结积分
	SCORE							lRestrictScore;						//限制积分

	//辅助变量
	bool							bRobotUser;							//机器用户
	bool							bMobileUser;						//手机用户	

	//权限变量
	DWORD							dwUserRight;						//用户权限
	DWORD							dwMasterRight;						//管理权限

	//机器码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//比赛信息
	WORD							wMacthGameCount;					//比赛局数

	//进出索引
	DWORD							dwInoutIndex;						//进出标识

	//位置信息
	double							dGlobalPosLat;						//定位精度
	double							dGlobalPosLng;						//定位纬度

	//状态信息
	BYTE							cbShutUpStatus;						//禁言状态
	DWORD							dwShutUpOverTime;					//解禁时间

	//黑白名单
	WORD							wAddupWinRate;						//累计胜率
	SCORE							lAddupWinScore;						//累计赢分
	SCORE							lAddupLoseScore;					//累计输分
	tagUserControlItem				UserControlItem;					//控制子项
};

//离线积分
struct tagOfflineScore
{
	DWORD							dwUserID;							//玩家ID
	DWORD							dwUserRight;						//玩家权限
	DWORD							dwClientIP;							//玩家IP
	DWORD							dwPlayTimeCount;					//游戏时间

	SCORE							lScore;								//游戏积分
	SCORE							lWinScore;							//胜利积分
	enScoreType						ScoreType;							//分数类型
	SCORE							lRevenue;							//游戏税收
	SCORE							lUnLockScore;						//解锁积分
	SCORE							lBetAmount;							//下注金额
	LONG							lWinCount;							//胜利数目
	LONG							lLostCount;							//失败数目
	LONG							lDrawCount;							//平局数目
	LONG							lFleeCount;							//逃跑数目
	LONG							lExperience;						//用户经验
};

//桌子状况
struct tagTableUserInfo
{
	WORD							wMinUserCount;						//最少人数	
	WORD							wTableUserCount;					//用户数目
	WORD							wTableReadyCount;					//准备数目
	WORD							wClientReadyCount;					//准备数目
	WORD							wTableRobotCount;					//机器数目	
};

//机器状况
struct tagRobotUserInfo
{
	WORD							wFreeUserCount;						//用户数目
	WORD							wPlayUserCount;						//用户数目
	WORD							wSitdownUserCount;					//用户数目
	IRobotUserItem *				pIRobotUserFree[MAX_ROBOT];		//机器接口
	IRobotUserItem *				pIRobotUserPlay[MAX_ROBOT];		//机器接口
	IRobotUserItem *				pIRobotUserSitdown[MAX_ROBOT];	//机器接口
};

//服务信息
struct tagRobotService
{
	WORD							wInsureDrawCount;					//取款次数
	DWORD							dwSwitchTableInnings;				//游戏局数
};

//帐号信息
struct tagRobotAccountsInfo
{
	DWORD							dwUserID;							//机器标识
	TCHAR							szPassword[LEN_PASSWORD];			//机器密码
};

//机器配置
struct tagRobotItemConfig
{
	tagRobotAccountsInfo			RobotAccountsInfo;					//帐号信息		
	tagRobotParameter *				pRobotParameter;					//机器参数
};

//机器动作
struct tagRobotAction
{
	WORD							wActionCode;						//动作代码
	DWORD							dwUserID;							//机器标识
	DWORD							dwDelayTime;						//延迟时间
	DWORD							dwGenerateTime;						//生成时间
	BYTE							cbActionData[16];					//动作数据
};

//配牌器
struct tagCardDispatcher
{
	//控制数据
	BYTE							cbControlGameCount;					//控制局数
	BYTE							cbMagicIndex;						//财神索引
	BYTE							cbMagicOpenPos;						//财神位置
	BYTE							cbMagicOpenCard;					//财神牌
	BYTE							cbCardCount;						//发牌数目
	BYTE							cbCardData[MAX_CARD_REPERTORY];		//发牌数据
};

//////////////////////////////////////////////////////////////////////////
//类型定义
typedef CWHArray<WORD, WORD> COptionValueArray;

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameServiceManager INTERFACE_VERSION(1,1)
	static const GUID IID_IGameServiceManager={0xffc1663d,0x26a8,0x488e,0x009b,0x43,0x31,0x33,0xef,0xb4,0x64,0xf2};
#else
	#define VER_IGameServiceManager INTERFACE_VERSION(1,1)
	static const GUID IID_IGameServiceManager={0xbf32a738,0x1380,0x4182,0x008b,0x7f,0xb8,0x97,0x7d,0xf3,0x26,0xd5};
#endif

//游戏接口
interface IGameServiceManager : public IUnknownEx
{
	//参数接口
public:
	//组件属性
	virtual bool GetGameServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)=NULL;
	//调整参数
	virtual bool RectifyOptionParameter(tagGameServiceOption & GameServiceOption)=NULL;

	//创建接口
public:
	//创建桌子
	virtual VOID * CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)=NULL;
	//创建机器
	virtual VOID * CreateRobotUserItemSink(REFGUID Guid, DWORD dwQueryVer)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameServiceCustomRule INTERFACE_VERSION(1,1)
	static const GUID IID_IGameServiceCustomRule={0x76b5f361,0x2989,0x40da,0x00b6,0x34,0x2e,0xca,0x20,0x62,0xa2,0x2a};
#else
	#define VER_IGameServiceCustomRule INTERFACE_VERSION(1,1)
	static const GUID IID_IGameServiceCustomRule={0x639781cc,0x799f,0x4f99,0x008e,0x48,0x46,0xfa,0x58,0xde,0x9d,0xa6};
#endif

//配置接口
interface IGameServiceCustomRule : public IUnknownEx
{
	//配置接口
public:
	//获取配置
	virtual bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)=NULL;
	//默认配置
	virtual bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)=NULL;
	//创建窗口
	virtual HWND CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameVideoPrivateSink INTERFACE_VERSION(1,1)
	static const GUID IID_IGameVideoPrivateSink={0x7ca0d424,0x9d7b,0x4b32,0xb4,0x50,0xea,0x69,0xc4,0x5c,0x0,0x1e};
#else
	#define VER_IGameVideoPrivateSink INTERFACE_VERSION(1,1)
	static const GUID IID_IGameVideoPrivateSink={0x1ee7b273,0xcbed,0x40a0,0xa9,0xe8,0x75,0x98,0x16,0xda,0x2,0x29};
#endif

//录像接口
interface IGameVideoPrivateSink : public IUnknownEx
{
	//发送接口
public:
	//私密数据
	virtual bool SendPrivateData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IServerUserItem INTERFACE_VERSION(1,2)
	static const GUID IID_IServerUserItem={0xd5f0b821,0x8c38,0x478b,0x0095,0x73,0x55,0x89,0x23,0xf1,0xda,0xf2};
#else
	#define VER_IServerUserItem INTERFACE_VERSION(1,2)
	static const GUID IID_IServerUserItem={0x0b8f593a,0x1a9d,0x4085,0x0088,0x93,0x62,0x01,0xd7,0x50,0x09,0xdd};
#endif

//用户接口
interface IServerUserItem : public IUnknownEx
{
	//属性信息
public:	
	//网络令牌
	virtual DWORD GetTokenID()=NULL;	
	//绑定索引
	virtual WORD GetBindIndex() = NULL;
	//用户地址
	virtual DWORD GetClientIP()=NULL;
	//用户信息
	virtual tagServerUserInfo * GetUserInfo()=NULL;
	//用户规则
	virtual tagServerUserRule * GetUserRule()=NULL;

	//属性信息
public:
	//用户标识
	virtual DWORD GetUserID()=NULL;
	//游戏标识
	virtual DWORD GetGameID()=NULL;
	//用户名字
	virtual LPCTSTR GetAccounts()=NULL;

	//状态接口
public:
	//桌子号码
	virtual WORD GetTableID()=NULL;
	//椅子号码
	virtual WORD GetChairID()=NULL;
	//用户状态
	virtual BYTE GetUserStatus()=NULL;

	//权限信息
public:
	//用户权限
	virtual DWORD GetUserRight()=NULL;
	//管理权限
	virtual DWORD GetMasterRight()=NULL;

	//游戏等级
public:
	//游戏等级
	virtual BYTE GetGameOrder()=NULL;
	//游戏经验
	virtual DWORD GetExperience()=NULL;	

	//会员等级
public:
	//会员等级
	virtual BYTE GetMemberOrder() = NULL;
	//管理等级
	virtual BYTE GetMasterOrder() = NULL;
	//会员点数
	virtual DWORD GetMemberPoint() = NULL;
	//更新点数
	virtual VOID UpdateMemberPoint(DWORD dwIncPoint) = NULL;

	//时间信息
public:
	//登录时间
	virtual DWORD GetLogonTime()=NULL;
	//游戏时间
	virtual DWORD GetPlayTimeCount()=NULL;
	//总游戏时间
	virtual DWORD GetTotalPlayTimeCount()=NULL;

	//积分信息
public:
	//用户分数
	virtual SCORE GetUserScore()=NULL;	
	//自由积分
	virtual SCORE GetUserFreeScore()=NULL;
	//锁定积分
	virtual SCORE GetUserLockScore()=NULL;	

	//积分信息
public:
	//用户胜率
	virtual WORD GetUserWinRate()=NULL;
	//用户输率
	virtual WORD GetUserLostRate()=NULL;
	//用户和率
	virtual WORD GetUserDrawRate()=NULL;
	//用户逃率
	virtual WORD GetUserFleeRate()=NULL;
	//游戏局数
	virtual LONG GetUserPlayCount()=NULL;

	//排位信息
public:
	//爱心数量
	virtual LONG GetLoveHeart()=NULL;	

	//效验接口
public:
	//对比帐号
	virtual bool ContrastAccounts(LPCTSTR pszAccounts)=NULL;
	//对比密码
	virtual bool ContrastPassword(LPCTSTR pszPassword)=NULL;
	//对比临时密码
	virtual bool ContrastTempPassword(LPCTSTR pszPassword)=NULL;
	//对比桌子密码
	virtual bool ContrastTablePassword(LPCTSTR pszPassword)=NULL;

	//托管状态
public:
	//判断状态
	virtual bool IsTrusteeUser()=NULL;
	//设置状态
	virtual VOID SetTrusteeUser(bool bTrusteeUser)=NULL;

	//游戏状态
public:
	//连接状态
	virtual bool IsClientReady()=NULL;
	//设置连接
	virtual VOID SetClientReady(bool bClientReady)=NULL;

	//控制用户
public:
	//控制状态
	virtual bool IsRobotUser()=NULL;
	//设置控制
	virtual VOID SetRobotUser(bool bbMachineUser)=NULL;

	//手机用户
public:
	//控制状态
	virtual bool IsMobileUser()=NULL;
	//设置控制
	virtual VOID SetMobileUser(bool bMobileUser)=NULL;

	//账号状态
public:
	//禁言状态
	virtual BYTE GetShutUpStatus() = NULL;
	//设置状态
	virtual VOID SetShutUpStatus(BYTE cbShutUpStatus) = NULL;
	//解禁时间
	virtual DWORD GetShutUpOverTime() = NULL;
	//设置时间
	virtual VOID SetShutUpOverTime(DWORD dwShutUpOverTime) = NULL;

	//黑白名单
public:
	//当前胜率
	virtual WORD GetAddupWinRate() = NULL;
	//累计赢分
	virtual SCORE GetAddupWinScore() = NULL;
	//累计输分
	virtual SCORE GetAddupLoseScore() = NULL;
	//控制状态
	virtual BYTE GetControlStatus() = NULL;	
	//调控状态
	virtual BYTE GetRegulateStatus(SCORE lScore) = NULL;	
	//预期分数
	virtual bool IsExpectScoreChange(SCORE lScore) = NULL;	
	//控制子项
	virtual tagUserControlItem* GetUserControlItem() = NULL;

	//比赛接口
public:	
	//比赛标识
	virtual DWORD GetMatchID()=NULL;
	//比赛标识
	virtual VOID SetMatchID(DWORD dwMatchID)=NULL;
	//比赛场次
	virtual SCORE GetMatchNo()=NULL;
	//比赛标识
	virtual VOID SetMatchNo(SCORE lMatchNo)=NULL;	
	//报名时间
	virtual DWORD GetSignUpTime() = NULL;
	//报名时间
	virtual VOID SetSignUpTime(DWORD dwSignUpTime) = NULL;
	//报名次数
	virtual WORD GetSignupCount() = NULL;
	//报名次数
	virtual VOID SetSignupCount(WORD wSignupCount) = NULL;	
	//赛券数量
	virtual DWORD GetTicketCount() = NULL;
	//赛券数量
	virtual VOID SetTicketCount(DWORD dwTicketCount) = NULL;
	//报名状态
	virtual BYTE GetUserMatchStatus()=NULL;
	//报名状态
	virtual VOID SetUserMatchStatus(BYTE cbMatchStatus)=NULL;
	//比赛分数
	virtual VOID InitMatchScore(SCORE lMatchScore)=NULL;	

	//手机定义
public:
	//手机规则
	virtual WORD GetMobileUserRule()=NULL;
	//设置定义
	virtual VOID SetMobileUserRule(WORD wMobileUserRule)=NULL;
	//当前分页
	virtual WORD GetMobileUserDeskPos()=NULL;
	//当前分页
	virtual VOID SetMobileUserDeskPos(WORD wMobileUserDeskPos)=NULL;
	//分页桌数
	virtual WORD GetMobileUserDeskCount()=NULL;
	//分页桌数
	virtual VOID SetMobileUserDeskCount(WORD wMobileUserDeskCount)=NULL;	

	//管理接口
public:
	//设置状态
	virtual bool SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID)=NULL;
	//写入财富
	virtual bool WriteUserWealth(WORD wMoneyID, SCORE lScore, WORD wReason)=NULL;
	//写入积分
	virtual bool WriteUserScore(SCORE lGameScore, SCORE lRevenue, SCORE lBetAmount, LONG lExperience, enScoreType ScoreType)=NULL;
	//设置临时密码
	virtual VOID SetTmpPassword( LPCTSTR lpszPassword )=NULL;
	//修改权限
	virtual VOID ModifyUserRight( DWORD dwAddRight, DWORD dwRemoveRight )=NULL;
	//查询临时密码
	virtual bool IsTmpPasswdEmpty()=NULL;

	//辅助接口
public:
	//用户背包
	virtual CServerUserPack* GetUserPack() = NULL;	

	//高级接口
public:
	//变更判断
	virtual bool IsVariation()=NULL;
	//设置参数
	virtual bool SetUserParameter(DWORD dwClientIP, WORD wIndex, DWORD dwTokenID, bool bRobotUser, bool bClientReady)=NULL;
	//提取变更
	virtual bool DistillVariation(tagVariationScore & VariationScore, const tagLockScoreInfo & LockScoreInfo)=NULL;
	//冻结积分
	virtual bool FreezeUserScore( SCORE lScore, bool bNotify )=NULL;

	//新增接口
public:
	//机器标识
	virtual LPCTSTR GetMachineID()=NULL;
	//获取密码
	virtual LPCTSTR GetPassword()=NULL;

	//低保接口
public:
	//低保次数
	virtual BYTE GetResidualTimes()=NULL;	
	//赠送标识
	virtual bool IsLargessSubsidy()=NULL;
	//低保次数
	virtual void SetResidualTimes(BYTE cbTimes)=NULL;	
	//赠送标识
	virtual void SetLargessSubsidy(bool bLargessSubsidy)=NULL;	

	//高级接口
public:
	//解除绑定
	virtual bool DetachBindStatus()=NULL;
	//记录索引
	virtual DWORD GetInoutIndex()=NULL;
	//查询记录
	virtual bool QueryRecordInfo(tagVariationScore & UserRecordInfo)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IServerUserItemSink INTERFACE_VERSION(1,1)
	static const GUID IID_IServerUserItemSink={0x07311553,0x207d,0x4c0b,0x0095,0x65,0xf3,0xc2,0xcc,0xbd,0x7e,0x69};
#else
	#define VER_IServerUserItemSink INTERFACE_VERSION(1,1)
	static const GUID IID_IServerUserItemSink={0xb5823d39,0xdf93,0x4106,0x0086,0x3f,0x85,0x4b,0xa3,0x6a,0xfb,0x42};
#endif

//状态接口
interface IServerUserItemSink : public IUnknownEx
{
	//控制变更
public:
	//控制开始
	virtual bool OnEventUserControlStart(IServerUserItem * pIServerUserItem)=NULL;
	//控制结束
	virtual bool OnEventUserControlFinish(IServerUserItem * pIServerUserItem)=NULL;

	//信息变更
public:
	//用户积分
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem)=NULL;	
	//用户排位
	virtual bool OnEventUserItemRanking(IServerUserItem* pIServerUserItem)=NULL;
	//用户状态
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID)=NULL;
	//用户权限
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight)=NULL;
	//用户财富
	virtual VOID OnEventUserItemWealth(IServerUserItem* pIServerUserItem, tagWealthVariation& WealthVariation) = NULL;
	//用户等级
	virtual VOID OnEventUserUserLevel(IServerUserItem* pIServerUserItem, BYTE cbLastLevelID, BYTE cbCurrLevelID) = NULL;
	//游戏等级
	virtual VOID OnEventUserGameLevel(IServerUserItem* pIServerUserItem, BYTE cbLastLevelID, BYTE cbCurrLevelID) = NULL;
	//用户背包
	virtual VOID OnEventUserItemPack(IServerUserItem* pIServerUserItem, WORD wGoodsIDList[], WORD wGoodsCount) = NULL;
	//用户冻结
	virtual bool OnEventUserFreezeScore(IServerUserItem *pIServerUserItem, SCORE lFreezeScore)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IServerUserManager INTERFACE_VERSION(1,1)
	static const GUID IID_IServerUserManager={0x2760f435,0xf2b3,0x432b,0x009d,0x14,0x52,0xa2,0x45,0x01,0xcc,0xf7};
#else
	#define VER_IServerUserManager INTERFACE_VERSION(1,1)
	static const GUID IID_IServerUserManager={0xaf50cbe9,0x8c88,0x4d21,0x0089,0x05,0x0c,0x5c,0x14,0x70,0x7a,0xae};
#endif

//用户管理
interface IServerUserManager : public IUnknownEx
{
	//配置接口
public:
	//设置接口
	virtual bool SetServerUserItemSink(IUnknownEx * pIUnknownEx)=NULL;
	//控制参数
	virtual VOID SetControlParamter(tagControlParameter* pControlParameter)=NULL;
	//服务参数
	virtual VOID SetServiceParameter(tagGameServiceOption* pGameServiceOption)=NULL;

	//查找接口
public:
	//枚举用户
	virtual IServerUserItem * EnumUserItem(WORD wEnumIndex)=NULL;
	//查找用户
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID)=NULL;
	//查找用户
	virtual IServerUserItem * SearchUserItem(LPCTSTR pszAccounts)=NULL;

	//统计接口
public:
	//在线人数
	virtual DWORD GetUserItemCount()=NULL;

	//管理接口
public:
	//删除用户
	virtual bool DeleteUserItem() = NULL;
	//删除用户
	virtual bool DeleteUserItem(IServerUserItem* pIServerUserItem) = NULL;
	//激活用户
	virtual IServerUserItem * ActiveUserItem(tagServerUserInfo& UserInfo, tagUserInfoPlus & UserInfoPlus, LPCTSTR pszPassword)=NULL;	
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableFrameItem INTERFACE_VERSION(1,2)
	static const GUID IID_ITableFrameItem={0x51a16304,0x0417,0x4ee6,0x00b8,0x31,0x7e,0x56,0x09,0x78,0xa2,0x95};
#else
	#define VER_ITableFrameItem INTERFACE_VERSION(1,2)
	static const GUID IID_ITableFrameItem={0x1655fd14,0x6dc0,0x4198,0x00ae,0x77,0x2c,0x63,0x15,0xb2,0x97,0xc1};
#endif

//桌子接口
interface ITableFrameItem : public IUnknownEx
{
	//属性接口
public:
	//桌子号码
	virtual WORD GetTableID()=NULL;
	//游戏人数
	virtual WORD GetChairCount()=NULL;
	//空位置数目
	virtual WORD GetNullChairCount()=NULL;
	//游戏时间
	virtual DWORD GetPlayTimeCount()=NULL;	

	//配置参数
public:
	//获取属性
	virtual tagGameServiceAttrib * GetGameServiceAttrib()=NULL;
	//获取配置
	virtual tagGameServiceOption * GetGameServiceOption()=NULL;

	//配置接口
public:
	//开始模式
	virtual enStartMode GetStartMode()=NULL;
	//开始模式
	virtual VOID SetStartMode(enStartMode StartMode)=NULL;

	//单元积分
public:
	//单元积分
	virtual LONG GetCellScore()=NULL;
	//单元积分
	virtual VOID SetCellScore(LONG lCellScore)=NULL;
	
	//最低进入 KK JIA
	virtual VOID SetMinEnterScore(LONGLONG lMinEnterScore) = NULL;

	//信息接口
public:
	//锁定状态
	virtual bool IsTableLocked()=NULL;
	//游戏状态
	virtual bool IsGameStarted()=NULL;
	//游戏状态
	virtual bool IsTableStarted()=NULL;

	//状态接口
public:
	//获取状态
	virtual BYTE GetGameStatus()=NULL;
	//设置状态
	virtual VOID SetGameStatus(BYTE bGameStatus)=NULL;

	//控制接口
public:
	//开始游戏
	virtual bool StartGame()=NULL;
	//暂停游戏
	virtual bool PauseGame()=NULL;
	//继续游戏
	virtual bool ContinueGame()=NULL;
	//解散游戏
	virtual bool DismissGame()=NULL;	
	//结束游戏
	virtual bool ConcludeGame(LPCTSTR pszGameCourse)=NULL;

	//统计接口
public:
	//计算税收
	virtual SCORE AccountRevenue(SCORE lGameScore)=NULL;

	//写分接口
public:	
	//扣服务费
	virtual VOID CostForGameStart() = NULL;
	//写入积分
	virtual bool WriteUserScore(WORD wChairID, tagScoreInfo& ScoreInfo)=NULL;
	//写入积分
	virtual bool WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount)=NULL;	

	//用户接口
public:
	//寻找用户
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID)=NULL;
	//游戏用户
	virtual IServerUserItem * GetTableUserItem(WORD wChairID)=NULL;
	//旁观用户
	virtual IServerUserItem * EnumLookonUserItem(WORD wEnumIndex)=NULL;

	//时间接口
public:
	//删除时间
	virtual bool KillGameTimer(DWORD dwTimerID) = NULL;
	//设置时间
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)=NULL;	

	//网络接口
public:
	//发送数据
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID)=NULL;
	//发送数据
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID=MDM_GF_GAME)=NULL;
	//发送数据
	virtual bool SendUserData(IServerUserItem* pIServerUserItem, WORD wSubCmdID) = NULL;
	//发送数据
	virtual bool SendUserData(IServerUserItem* pIServerUserItem, WORD wSubCmdID, VOID* pData, WORD wDataSize) = NULL;
	//发送数据
	virtual bool SendDebugData(IServerUserItem* pIServerUserItem, WORD wDebugKind, WORD wSubCmdID) = NULL;
	//发送数据
	virtual bool SendDebugData(IServerUserItem* pIServerUserItem, WORD wDebugKind, WORD wSubCmdID, VOID* pData, WORD wDataSize) = NULL;
	//发送数据
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID)=NULL;
	//发送数据
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wMainCmdID=MDM_GF_GAME)=NULL;	

	//功能接口
public:
	//发送消息
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType)=NULL;
	//跑马灯公告
	virtual bool SendMarqeeNotice(BYTE cbNoticeKind, VOID* pNoticeContent, WORD wBufferSize) = NULL;
	//游戏消息
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)=NULL;
	//房间消息
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)=NULL;	

	//动作处理
public:
	//旁观动作
	virtual bool PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//坐下动作
	virtual bool PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//起立动作
	virtual bool PerformStandUpAction(IServerUserItem* pIServerUserItem, bool bInitiative = false) = NULL;

	//功能接口
public:
	//扣除服务费
	virtual VOID ChargeGameAndReset() = NULL;
	//模拟配置
	virtual	bool ImitateGameOption(IServerUserItem* pIServerUserItem)=NULL;
	//发送场景
	virtual bool OnEventSendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)=NULL;
	//离线锁分
	virtual VOID FreezeGameScore(IServerUserItem *pIServerUserItem)=NULL;	

	//分数接口
public:
	//分数变更
	virtual VOID OnEventUserScoreChange(IServerUserItem *pIServerUserItem)=NULL;

	//排位接口
public:
	//获取转换比例
	virtual WORD GetWinConvertRate() = NULL;
	//获取转换比例
	virtual WORD GetLoseConvertRate() = NULL;

	//跑马灯接口
public:
	//获取赢取分数
	virtual SCORE GetReachWinScore() = NULL;
	//获取奖励金额
	virtual SCORE GetReachAwardAmount() = NULL;

	//统计接口
public:
	//用户数目
	virtual WORD GetSitUserCount() = NULL;
	//真人数目
	virtual WORD GetRealUserCount() = NULL;
	//机器数目
	virtual WORD GetRobotUserCount() = NULL;
	//旁观数目
	virtual WORD GetLookonUserCount() = NULL;
	//断线数目
	virtual WORD GetOffLineUserCount() = NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableFrameSink INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrameSink={0xeb3f34a6,0x0d9e,0x4fb6,0x0084,0xc8,0x8a,0x3e,0xae,0x70,0x10,0x60};
#else
	#define VER_ITableFrameSink INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrameSink={0xfa7f6f71,0xe2f8,0x43bf,0x008c,0xa5,0xe1,0x30,0x13,0x23,0xf3,0x81};
#endif

//回调接口
interface ITableFrameSink : public IUnknownEx
{
	//管理接口
public:
	//复位桌子
	virtual VOID ResetTableFrameSink() = NULL;
	//配置桌子
	virtual bool InitTableFrameSink(IUnknownEx * pIUnknownEx)=NULL;	

	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart()=NULL;
	//游戏暂停
	virtual bool OnEventGamePause()=NULL;
	//游戏继续
	virtual bool OnEventGameContinue()=NULL;
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)=NULL;

	//事件接口
public:
	//定时器事件
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//游戏消息
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
	//框架消息
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_ITableControlSendCard INTERFACE_VERSION(1,1)
static const GUID IID_ITableControlSendCard = { 0xf1926b0d, 0x3990, 0x4a18, 0x9e, 0xdc, 0x2f, 0x1c, 0x44, 0xb9, 0xd9, 0xa8 };
#else
#define VER_ITableControlSendCard INTERFACE_VERSION(1,1)
static const GUID IID_ITableControlSendCard = { 0x88b657fb, 0x13e8, 0x434f, 0xb8, 0x33, 0x4, 0xf6, 0x6f, 0xba, 0xe4, 0xcd };
#endif

//控制发牌
interface ITableControlSendCard : public IUnknownEx
{
	//配牌器
	virtual bool OnEventControlSendCard(IServerUserItem* pIServerUserItem, tagCardDispatcher* SendCardData) = NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableUserAction INTERFACE_VERSION(1,1)
	static const GUID IID_ITableUserAction={0xfaa4ff2e,0x6347,0x4de2,0x009d,0x19,0x3d,0x35,0x1c,0xd7,0xa3,0x28};
#else
	#define VER_ITableUserAction INTERFACE_VERSION(1,1)
	static const GUID IID_ITableUserAction={0x7e42fb7f,0x0ffc,0x46fd,0x0097,0x0b,0x3d,0xe9,0x13,0x39,0x1c,0x06};
#endif

//动作事件
interface ITableUserAction : public IUnknownEx
{
	//用户断线
	virtual bool OnActionUserOffLine(IServerUserItem * pIServerUserItem)=NULL;
	//用户重入
	virtual bool OnActionUserConnect(IServerUserItem * pIServerUserItem)=NULL;
	//用户坐下
	virtual bool OnActionUserSitDown(IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//用户起来
	virtual bool OnActionUserStandUp(IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//用户同意
	virtual bool OnActionUserReady(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_ITableUserRequest INTERFACE_VERSION(1,1)
static const GUID IID_ITableUserRequest={0xffa8f2ee,0x1080,0x4f7c,0x9f,0x38,0xbd,0xdd,0x64,0x87,0x99,0x40};
#else
#define VER_ITableUserRequest INTERFACE_VERSION(1,1)
static const GUID IID_ITableUserRequest={0x4de032b5,0x6482,0x4d49,0xbb,0xa0,0xb1,0xc4,0xd3,0x81,0x81,0x7b};
#endif

//动作事件
interface ITableUserRequest : public IUnknownEx
{
	//请求事件
public:
	//请求重入
	virtual bool OnTableUserReqConnect(IServerUserItem *pIServerUserItem)=NULL;
	//请求坐下
	virtual bool OnTableUserReqSitDown(IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//请求起来
	virtual bool OnTableUserReqStandUp(IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//请求准备
	virtual bool OnTableUserReqReady(IServerUserItem* pIServerUserItem, VOID* pData, WORD wDataSize) = NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IEventServiceCost INTERFACE_VERSION(1,1)
static const GUID IID_IEventServiceCost={0xc2cc9870,0xc124,0x4fc5,0x0084,0x4e,0x47,0x2a,0x21,0x42,0x35,0x9b};
#else
#define VER_IEventServiceCost INTERFACE_VERSION(1,1)
static const GUID IID_IEventServiceCost={0x9e989e0e,0xea3c,0x4179,0x009d,0x56,0x84,0xad,0x8c,0x66,0x94,0x60};
#endif

//框架事件
interface IEventServiceCost : public IUnknownEx
{
	//咨询玩家
	virtual bool OnEventQueryCostable(IServerUserItem *pIServerUserItem, bool bLookonUser)=NULL;
	//咨询服务费
	virtual SCORE OnEventQueryCost(IServerUserItem *pIServerUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IEventUserScore INTERFACE_VERSION(1,1)
static const GUID IID_IEventUserScore={0x3e507657,0xbad1,0x4231,0xbe,0x2,0xb2,0x80,0x1d,0xad,0x23,0x20};
#else
#define VER_IEventUserScore INTERFACE_VERSION(1,1)
static const GUID IID_IEventUserScore={0x94a82fba,0x1703,0x45c4,0xb7,0x61,0xbc,0x57,0xf3,0x21,0x2d,0xc2};
#endif

//框架事件
interface IEventUserScore : public IUnknownEx
{
	//分数变更
	virtual VOID OnEventScoreVariation(IServerUserItem *pIServerUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IEventBaseScore INTERFACE_VERSION(1,1)
static const GUID IID_IEventBaseScore={0x7614a289,0x8714,0x43b8,0xa0,0x4e,0xcb,0x8b,0x13,0x95,0xe1,0x1a};
#else
#define VER_IEventBaseScore INTERFACE_VERSION(1,1)
static const GUID IID_IEventBaseScore={0x9be405de,0x8fc3,0x4a74,0xb2,0x8c,0x79,0x67,0x5f,0x2c,0xaa,0xe0};
#endif

//框架事件
interface IEventBaseScore : public IUnknownEx
{
	//底分变更
	virtual VOID OnEventBaseScoreVariation(LONG lBaseScore)=NULL;
};


//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IEventStartEfficacy INTERFACE_VERSION(1,1)
static const GUID IID_IEventStartEfficacy={0xa3cd8cc1,0x7c3a,0x4052,0x92,0x24,0x44,0x89,0x8c,0x69,0x10,0xde};
#else
#define VER_IEventStartEfficacy INTERFACE_VERSION(1,1)
static const GUID IID_IEventStartEfficacy={0xddb4d2a0,0xe032,0x4d50,0xbb,0x56,0x6,0x38,0x32,0xa4,0xde,0x33};
#endif

//开始校验
interface IEventStartEfficacy : public IUnknownEx
{	
	//管理标识
	virtual WORD GetMasterChairID()=NULL;
	//开始人数
	virtual WORD GetStartPlayerCount()=NULL;	
	//允许准备
	virtual bool IsAllowPlayerReady(WORD wChairID)=NULL;
};


//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IEventCustomFrameTime INTERFACE_VERSION(1,1)
static const GUID IID_IEventCustomFrameTime={0xfeab5761, 0x6dae, 0x4e61, 0xb9, 0x5a, 0x24, 0x84, 0x13, 0x7b, 0xa8, 0x3f};
#else
#define VER_IEventCustomFrameTime INTERFACE_VERSION(1,1)
static const GUID IID_IEventCustomFrameTime={0x846580ea, 0x704f, 0x4277, 0xb5, 0x8e, 0xfd, 0x75, 0x38, 0x9c, 0xea, 0x1e};
#endif

//自定义时间
interface IEventCustomFrameTime : public IUnknownEx
{	
	//解散时间
	virtual DWORD GetBattleDismissTime()=NULL;
	//踢人时间
	virtual DWORD GetUnReadyKickOutTime()=NULL;
	//解散时间
	virtual DWORD GetAgreementDismissTime()=NULL;
};

//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IEventGameOption INTERFACE_VERSION(1,1)
static const GUID IID_IEventGameOption={0xf750da53,0xdf05,0x4f9b,0xa4,0x6c,0x5d,0x1c,0x3c,0xe3,0xc4,0x5a};
#else
#define VER_IEventGameOption INTERFACE_VERSION(1,1)
static const GUID IID_IEventGameOption={0xa0dba87c,0x9d1b,0x4c53,0x9a,0x49,0x25,0xf5,0x5,0xed,0xe7,0x12};
#endif

//游戏配置
interface IEventGameOption : public IUnknownEx
{	
	//配置详情
	virtual VOID FormatOptionDetail(CString & strOptionDetail) = NULL;
	//游戏配置
	virtual bool SetNormalOption(bool bOnlyVerify, BYTE cbNormalOption[],WORD wBufferSize)=NULL;
	//游戏配置
	virtual bool SetAdvancedOption(bool bOnlyVerify, BYTE cbAdvancedOption[], WORD wBufferSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IEventBattleSink INTERFACE_VERSION(1,1)
static const GUID IID_IEventBattleSink={0xd88282c4,0xf92e,0x4fc2,0x9d,0xe2,0x53,0x21,0xeb,0x6a,0x60,0x11};
#else
#define VER_IEventBattleSink INTERFACE_VERSION(1,1)
static const GUID IID_IEventBattleSink={0x6e73b365,0xc988,0x45dd,0x8b,0x3f,0x69,0x4a,0xe7,0x64,0x8,0xc0};
#endif

//约战接口
interface IEventBattleSink : public IUnknownEx
{	
	//约战服务
public:
	//约战结束
	virtual bool BattleFinish(BYTE cbReason)=NULL;
	//获取数组
	virtual bool GetOptionValueArray(WORD wOptionIndex, COptionValueArray & OptionValueArray)=NULL;

	//约战规则
public:
	//约战规则
	virtual DWORD GetBattleRule()=NULL;
	//约战规则
	virtual VOID SetBattleRule(DWORD dwAddRules, DWORD dwRemoveRules)=NULL;
};

//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IEventBattleItem INTERFACE_VERSION(1,1)
static const GUID IID_IEventBattleItem={0x76618df0,0xdc52,0x40aa,0x95,0x81,0x48,0x7c,0x74,0x48,0x1d,0x91};
#else
#define VER_IEventBattleItem INTERFACE_VERSION(1,1)
static const GUID IID_IEventBattleItem={0x7e4557f1,0x20aa,0x4937,0xb0,0x47,0x89,0xf7,0x1e,0x48,0xc4,0x6d};
#endif

//约战子项
interface IEventBattleItem : public IUnknownEx
{	
	//信息函数
public:
	//赢取分数
	virtual SCORE GetBattleWinScore()=NULL;
	//同意人数
	virtual WORD GetDismissAgreeCount()=NULL;		
	//约战摘要
	virtual LPCTSTR GetUserBattleAbstract(WORD wChairID)=NULL;

	//事件通知
public:
	//桌子解散
	virtual VOID OnEventTableDismiss()=NULL;	
	//桌子修改
	virtual VOID OnEventTableModify(LPVOID pModifyParam, WORD wBufferSize)=NULL;
	//桌子创建
	virtual VOID OnEventTableCreated(LPVOID pCreateParam, WORD wBufferSize)=NULL;	
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_ITableEventSink INTERFACE_VERSION(1,1)
static const GUID IID_ITableEventSink={0x1fdc6b5b,0x5f1d,0x4761,0x0089,0x61,0x5c,0x2f,0x1c,0x85,0xf7,0xe};
#else
#define VER_ITableEventSink INTERFACE_VERSION(1,1)
static const GUID IID_ITableEventSink={0xb5bfe81e,0x95f,0x494b,0x00b1,0xb9,0x5a,0xb1,0x23,0x46,0xf9,0x63};
#endif

//桌子事件
interface ITableEventSink : public IUnknownEx
{
	//桌子事件
public:
	//桌子锁事件
	virtual VOID OnEventTableLocked(ITableFrameItem *pITableFrame, BOOL bTableLocked )=NULL;
	//桌子状态事件
	virtual VOID OnEventTableStatus(ITableFrameItem *pITableFrame, BOOL bPlayStatus )=NULL;

	//请求事件
public:
	//离线写分请求
	virtual VOID OnTableReqOfflineScore( tagOfflineScore const &OfflineScore )=NULL;
	//解冻锁分请求
	virtual VOID OnTableReqUnfreezeScore( DWORD dwUserID, SCORE lUnfreezeScore )=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IServiceFunction INTERFACE_VERSION(1,1)
	static const GUID IID_IServiceFunction={0x4888f916,0x12c9,0x4a5c,0x00a9,0x4d,0x6c,0xaf,0x1f,0xd0,0x48,0xd0};
#else
	#define VER_IServiceFunction INTERFACE_VERSION(1,1)
	static const GUID IID_IServiceFunction={0x20bb62da,0x77d8,0x4d54,0x008a,0x6f,0xeb,0x7e,0x8d,0xe8,0x15,0x6c};
#endif

//服务功能
interface IServiceFunction : public IUnknownEx
{
	//信息接口
public:
	//获取状态
	virtual WORD GetServiceStatus()=NULL;

	//网络接口
public:
	//发送数据
	virtual bool SendBatchData(BYTE cbSendMask,WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//群发数据
	virtual bool SendMobileData(WORD wCmdTable, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize) = NULL;	
	//发送数据
	virtual bool SendDataToGate(DWORD dwContextID, DWORD dwTokenID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//发送数据
	virtual bool SendDataToUser(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;	
	//发送数据
	virtual bool SendDebugData(IServerUserItem* pIServerUserItem, WORD wTableID, WORD wDebugKind, WORD wSubCmdID, VOID* pData, WORD wDataSize) = NULL;

	//服务接口
public:
	//发送数据
	virtual bool SendDataToCenter(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)=NULL;	

	//消息接口
public:
	//房间消息
	virtual bool SendRoomMessage(LPCTSTR lpszMessage, WORD wType)=NULL;
	//游戏消息
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType)=NULL;
	//跑马灯公告
	virtual bool SendMarqeeNotice(BYTE cbNoticeKind, VOID* pNoticeContent, WORD wNoticeSize) = NULL;
	//房间消息
	virtual bool SendRoomMessage(DWORD dwSocketID, DWORD dwTokenID, LPCTSTR lpszMessage, WORD wType)=NULL;
	//房间消息
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)=NULL;
	//游戏消息
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)=NULL;	

	//库存接口
public:
	//库存分数
	virtual CShareStockItemPtr GetStockItem(WORD wStockKind, WORD wStockSubtype) = NULL;
	//修改库存
	virtual bool ModifyStockScore(WORD wStockKind, WORD wStockSubtype, SCORE lModifyScore) = NULL;

	//通知接口
public:	
	//游戏开始
	virtual bool OnEventGameStart(ITableFrameItem* pITableFrame) = NULL;
	//游戏结束
	virtual bool OnEventGameConclude(ITableFrameItem * pITableFrame)=NULL;		

	//分组接口
public:
	//插入分组
	virtual bool InsertDistribute(IServerUserItem * pIServerUserItem)=NULL;
	//移除分组
	virtual bool RemoveDistribute(IServerUserItem * pIServerUserItem)=NULL;		

	//约战接口
public:
	//数值数组
	virtual bool GetOptionValueArray(WORD wEmployScene, WORD wOptionIndex, COptionValueArray & OptionValueArray)=NULL;	

	//存储接口
public:
	//存储录像
	virtual bool StoreVideoData(VOID * pData,WORD wDataSize)=NULL;

	//功能接口
public:
	//敏感词过滤
	virtual bool FilterWords(LPCTSTR szFiltered)=NULL;
	//敏感词过滤
	virtual void FilterWords(LPWSTR szFiltered, int nMsgLen)=NULL;
	//发送分数
	virtual bool SendUserScore(IServerUserItem *pIServerUserItem)=NULL;	
};


//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IRobotService INTERFACE_VERSION(1,1)
static const GUID IID_IRobotService = { 0x5468bfb,0xeb56,0x480c,0xa9,0x82,0x62,0x1a,0xc2,0x20,0xf9,0xa2 };
#else
#define VER_IRobotService INTERFACE_VERSION(1,1)
static const GUID IID_IRobotService = { 0xcf77eef2,0xbe9,0x49d8,0xb4,0x50,0xcb,0x27,0xe5,0x5f,0xc8,0x44 };
#endif

//机器人接口
interface IRobotService : public IUnknownEx
{
	//功能接口
public:
	//创建机器人
	virtual VOID* CreateRobotUserItemSink(REFGUID Guid, DWORD dwQueryVer) = NULL;
};

DECLARE_MODULE_DYNAMIC(RobotService, RobotService);

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IRobotUserItem INTERFACE_VERSION(1,1)
static const GUID IID_IRobotUserItem = { 0x014b4ef4,0xa9d5,0x45cb,0x0092,0xa1,0x01,0xcf,0xe9,0xe7,0x0f,0x3c };
#else
#define VER_IRobotUserItem INTERFACE_VERSION(1,1)
static const GUID IID_IRobotUserItem = { 0xebb22c87,0x7612,0x4707,0x00b3,0xf8,0x95,0xdd,0x51,0xad,0x93,0x98 };
#endif

//机器人接口
interface IRobotUserItem : public IUnknownEx
{
	//状态函数
public:
	//获取状态
	virtual BYTE GetGameStatus() = NULL;
	//设置状态
	virtual VOID SetGameStatus(BYTE cbGameStatus) = NULL;

	//信息接口
public:
	//获取 I D
	virtual DWORD GetUserID() = NULL;
	//桌子号码
	virtual WORD GetTableID() = NULL;
	//椅子号码
	virtual WORD GetChairID() = NULL;
	//用户状态
	virtual BYTE GetUserStatus() = NULL;
	//用户名字
	virtual LPCTSTR GetAccounts() = NULL;

	//积分信息
public:
	//用户积分
	virtual SCORE GetUserScore() = NULL;

	//网络接口
public:
	//发送函数
	virtual bool SendSocketData(WORD wSubCmdID) = NULL;
	//发送函数
	virtual bool SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize) = NULL;

	//信息接口
public:
	//获取自己
	virtual IServerUserItem* GetMeUserItem() = NULL;	

	//同桌信息
public:
	//真人数目
	virtual WORD GetTableRealUserCount() = NULL;
	//同桌用户
	virtual IServerUserItem* GetTableUserItem(WORD wChairID) = NULL;

	//功能接口
public:
	//删除时间
	virtual bool KillGameTimer(UINT nTimerID) = NULL;
	//设置时间
	virtual bool SetGameTimer(UINT nTimerID, UINT nElapse) = NULL;
	//发送准备
	virtual bool SendUserReady(VOID * pData, WORD wDataSize) = NULL;	
	//发送聊天
	virtual bool SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor) = NULL;

	//配置函数
public:
	//设置版本
	virtual VOID SetAdroidVersion(DWORD dwVersion) = NULL;
	//设置动作
	virtual void SetRobotAction(WORD wTestAction, bool bEnable) = NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IRobotUserItemSink INTERFACE_VERSION(1,1)
static const GUID IID_IRobotUserItemSink = { 0x366aba89,0xcca2,0x40d6,0x0087,0x95,0xb5,0xb2,0xb6,0x1d,0x29,0x80 };
#else
#define VER_IRobotUserItemSink INTERFACE_VERSION(1,1)
static const GUID IID_IRobotUserItemSink = { 0x4cbe7840,0x5068,0x42fb,0x0083,0x38,0x8a,0xb6,0x58,0x65,0xa5,0xdd };
#endif

//机器人接口
interface IRobotUserItemSink : public IUnknownEx
{
	//控制接口
public:
	//重置接口
	virtual bool ResetUserItemSink() = NULL;
	//初始接口
	virtual bool InitUserItemSink(IUnknownEx * pIUnknownEx) = NULL;	

	//参数接口
public:
	//行为模板
	virtual bool SetActionTemplate(tagActionTemplateData * pActionTemplateData)=NULL;

	//游戏事件
public:
	//时间消息
	virtual bool OnEventTimer(UINT nTimerID) = NULL;
	//游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize) = NULL;
	//游戏消息
	virtual bool OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize) = NULL;
	//场景消息
	virtual bool OnEventSendGameScene(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize) = NULL;

	//用户事件
public:
	//用户进入
	virtual VOID OnEventUserEnter(IRobotUserItem * pIRobotUserItem, bool bLookonUser) = NULL;
	//用户离开
	virtual VOID OnEventUserLeave(IRobotUserItem * pIRobotUserItem, bool bLookonUser) = NULL;
	//用户积分
	virtual VOID OnEventUserScore(IRobotUserItem * pIRobotUserItem, bool bLookonUser) = NULL;
	//用户状态
	virtual VOID OnEventUserStatus(IRobotUserItem * pIRobotUserItem, bool bLookonUser) = NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IRobotUserManager INTERFACE_VERSION(1,1)
	static const GUID IID_IRobotUserManager={0x847b487d,0x2a16,0x4de9,0x008b,0xba,0x54,0x55,0x8f,0x47,0xd4,0x6c};
#else
	#define VER_IRobotUserManager INTERFACE_VERSION(1,1)
	static const GUID IID_IRobotUserManager={0xded38804,0x8413,0x4f56,0x0089,0x9b,0xcd,0x11,0x7a,0x19,0x37,0x51};
#endif

//机器人接口
interface IRobotUserManager : public IUnknownEx
{
	//控制接口
public:
	//启动服务
	virtual bool StartService()=NULL;
	//停止服务
	virtual bool ConcludeService()=NULL;

	//配置接口
public:	
	//移除参数
	virtual bool RemoveRobotParameter(DWORD dwBatchID)=NULL;
	//添加参数
	virtual bool AddRobotParameter(tagRobotParameter RobotParameter[], WORD wParemeterCount)=NULL;
	//插入机器
	virtual bool InsertRobotInfo(tagRobotAccountsInfo RobotAccountsInfo[],WORD wRobotCount,DWORD dwBatchID)=NULL;

	//管理接口
public:
	//投递动作
	virtual bool PostponeRobotAction(DWORD dwUserID,WORD wActionCode)=NULL;
	//删除机器
	virtual bool DeleteRobotUserItem(DWORD dwRobotID,bool bStockRetrieve)=NULL;
	//查找机器
	virtual IRobotUserItem * SearchRobotUserItem(DWORD dwUserID)=NULL;
	//创建机器
	virtual IRobotUserItem * CreateRobotUserItem(tagRobotItemConfig & RobotItemConfig)=NULL;	

	//事件接口
public:
	//脉冲事件
	virtual bool OnEventTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;

	//状态接口
public:
	//机器数目
	virtual WORD GetRobotCount()=NULL;
	//加载机器
	virtual bool GetRobotLoadInfo(DWORD & dwBatchID,DWORD & dwLoadCount)=NULL;
	//用户状况
	virtual WORD GetRobotUserInfo(tagRobotUserInfo & RobotUserInfo, DWORD dwServiceMode)=NULL;	

	//网络接口
public:
	//发送数据
	virtual bool SendDataToClient(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//发送数据
	virtual bool SendDataToClient(DWORD dwRobotID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//发送数据
	virtual bool SendDataToServer(DWORD dwRobotID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGameServiceCustomTime INTERFACE_VERSION(1,1)
static const GUID IID_IGameServiceCustomTime={0xf57573b0,0x63c3,0x43a9,0x8c,0xb1,0xe2,0x22,0xd3,0x93,0xe2,0xb};
#else
#define VER_IGameServiceCustomTime INTERFACE_VERSION(1,1)
static const GUID IID_IGameServiceCustomTime={0xf57573b0,0x63c3,0x43a9,0x8c,0xb1,0xe2,0x22,0xd3,0x93,0xe2,0xb};
#endif

//时间配置
interface IGameServiceCustomTime : public IUnknownEx
{	
	//机器脉冲
	virtual DWORD GetRobotTimerPulse()=NULL;
	//时间范围
	virtual DWORD GetTableSinkTimeRange()=NULL;
	//时间单元
	virtual DWORD GetTimerEngineTimeCell()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameServiceUnits INTERFACE_VERSION(1,1)
	static const GUID IID_IGameServiceUnits={0x9385d3a0,0x448a,0x4ad8,0xbc,0x6f,0x06,0x8f,0xb0,0xb0,0xe1,0x8f};
#else
	#define VER_IGameServiceUnits INTERFACE_VERSION(1,1)
	static const GUID IID_IGameServiceUnits={0xc5950f08,0x41f2,0x4c18,0xaa,0x2b,0x48,0xd2,0x83,0x28,0x61,0x38};
#endif

//服务单元
interface IGameServiceUnits : public IUnknownEx
{
	//信息函数
public:
	//获取状态
	virtual WORD GetServiceStatus()=NULL;

	//服务控制
public:
	//启动服务
	virtual bool StartService()=NULL;
	//停止服务
	virtual bool ConcludeService()=NULL;
	//设置接口
	virtual bool SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink)=NULL;
	//游戏配置
	virtual bool SetServiceCollocate(LPCTSTR pszGameModule, const tagGameServiceOption & GameServiceOption)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGameBalanceControl INTERFACE_VERSION(1,1)
static const GUID IID_IGameBalanceControl = { 0xce9d1609, 0x9873, 0x4fa6, 0xba, 0x95, 0x5b, 0x36, 0xc4, 0x1, 0x47, 0xee };
#else
#define VER_IGameBalanceControl INTERFACE_VERSION(1,1)
static const GUID IID_IGameBalanceControl = { 0xdf9be98, 0xd15c, 0x4bc4, 0xa1, 0x75, 0xa7, 0x9f, 0xb1, 0x3d, 0xb1, 0x6c };
#endif

//游戏平衡控制
interface IGameBalanceControl : public IUnknownEx
{
	//对战类点控
public:
	//控制扑克
	virtual bool IsControlCard()=NULL;
	//扑克等级
	virtual BYTE GetUserCardLevel(WORD wChairID)=NULL;	

	//群控接口
public:
	//库存分数
	virtual CShareStockItemPtr GetStockItem(WORD wStockKind, WORD wStockSubtype)=NULL;
	//修改库存
	virtual bool ModifyStockScore(WORD wStockKind, WORD wStockSubtype, SCORE lModifyScore)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IExtendDataBaseSink INTERFACE_VERSION(1, 1)
static const GUID IID_IExtendDataBaseSink = { 0x336e9f83,0x5688,0x4957,0xa7,0x4b,0x77,0xff,0x98,0xb7,0xef,0x51 };
#else
#define VER_IExtendDataBaseSink INTERFACE_VERSION(1, 1)
static const GUID IID_IExtendDataBaseSink = { 0x2ca4a68f,0x3c0a,0x4ee4,0xbb,0x3d,0x98,0xdd,0xf8,0xe7,0xc8,0xb3 };
#endif

//扩展钩子子项
interface IExtendDataBaseSink : public IUnknownEx
{
	//配置接口
public:
	//初始化
	virtual bool InitExtendDataBaseSink(tagExtendDBSinkParameter& ExtendDBSinkParameter) = NULL;

	//内核事件
public:
	//时间事件
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter) = NULL;
	//控制事件
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID* pData, WORD wDataSize) = NULL;
	//请求事件
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IExtendServiceManager INTERFACE_VERSION(1,1)
static const GUID IID_IExtendServiceManager = { 0xd513eace,0xb67d,0x43d9,0x0097,0xfa,0xd8,0xa7,0x9d,0x31,0x39,0x9b };
#else
#define VER_IExtendServiceManager INTERFACE_VERSION(1,1)
static const GUID IID_IExtendServiceManager = { 0x7d381e8a,0x8c10,0x4443,0x00b5,0xad,0x75,0x4b,0xe5,0x50,0xc5,0x53 };
#endif

//扩展服务管理接口
interface IExtendServiceManager : public IUnknownEx
{
	//控制接口
public:
	//启动服务
	virtual bool StartService() = NULL;
	//停止服务
	virtual bool ConcludeService() = NULL;	

	//管理接口
public:
	//创建桌子
	virtual VOID * CreateExtendTableFrame() = NULL;
	//创建子项
	virtual VOID * CreateExtendDataBaseSink(REFGUID Guid, DWORD dwQueryVer) = NULL;
	//初始化
	virtual bool InitServiceManager(tagExtendServiceParameter& ExtendServiceParameter) = NULL;

	//系统事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter) = NULL;
	//数据库事件
	virtual bool OnEventDataBase(WORD wRequestID, VOID* pData, WORD wDataSize, DWORD dwContextID, DWORD dwTokenID) = NULL;

	//网络事件
public:
	//网络事件
	virtual bool OnEventSocketRead(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize) = NULL;
	//网络事件
	virtual bool OnEventNetworkRead(WORD wSubCmdID, VOID* pData, WORD wDataSize, IServerUserItem* pIServerUserItem, DWORD dwSocketID, DWORD dwTokenID) = NULL;

	//用户接口
public:
	//用户登录
	virtual bool OnEventUserLogon(IServerUserItem* pIServerUserItem) = NULL;
	//用户登出
	virtual bool OnEventUserLogout(IServerUserItem* pIServerUserItem) = NULL;
};

//////////////////////////////////////////////////////////////////////////

//游戏服务
DECLARE_MODULE_DYNAMIC(GameServiceManager, GameServiceManager)

//服务单元
DECLARE_MODULE_HELPER(GameServiceUnits, GameServiceUnits, GAME_SERVICE_DLL_NAME, "CreateServiceUnits")


//////////////////////////////////////////////////////////////////////////
//导出文件

#ifndef GAME_SERVICE_DLL
	#include "TableFrame.h"
	#include "ServerUserManager.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif