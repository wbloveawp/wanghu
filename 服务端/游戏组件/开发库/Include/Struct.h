#ifndef STRUCT_HEAD_FILE
#define STRUCT_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////

//游戏类型
struct tagGameType
{
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wTypeID;							//类型索引
	TCHAR							szTypeName[LEN_TYPE];				//种类名字
};

//游戏种类
struct tagGameKind
{
	WORD							wModuleID;							//类型标识
	WORD							wTypeID;							//类型索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wKindID;							//名称索引
	DWORD							dwOnLineCount;						//在线人数
	TCHAR							szKindName[LEN_KIND];				//游戏名字
	TCHAR							szProcessName[LEN_PROCESS];			//进程名字
};

//服务属性
struct tagServiceAttrib
{
	WORD							wServiceModule;						//服务模块
	TCHAR							szServiceName[32];					//服务名称
	bool							bIsStateful;						//状态标志
	bool							bIsExternal;						//对外标志
	WORD							wServiceMapKind;					//映射方式
	WORD							dwMaxInstanceNum;					//实例数目
};

//服务子项
struct tagServiceItem
{
	WORD							wServiceID;							//服务标识
	WORD							wServiceModule;						//服务模块
	WORD							wServicePort;						//服务端口
	WORD							wMaxConnect;						//最大连接
	DWORD							dwServiceAddr;						//服务地址
	WORD							wServiceKindID;						//类型标识
	TCHAR							szServiceName[LEN_SERVER];			//服务名称
	TCHAR							szServiceDomain[LEN_DOMAIN];		//服务地址		
};

//服务配置
struct tagServiceOption
{
	//基础信息
	WORD							wServiceID;							//服务标识
	WORD							wServicePort;						//服务端口
	WORD							wMaxConnect;						//最大连接
	TCHAR							szServiceName[LEN_SERVER];			//服务名称
	TCHAR							szServiceDomain[LEN_DOMAIN];		//服务地址	

	//扩展信息
	WORD							wServiceKindID;						//类型标识
	TCHAR							szServiceKindName[LEN_SERVER];		//类型名称
};

//服务类型
struct tagServiceKind
{
	//扩展信息
	WORD							wServiceKindID;						//类型标识
	TCHAR							szServiceKindName[LEN_SERVER];		//类型名称
};

//路由子项
struct tagRouteItem
{
	DWORD							dwRouteID;							//路由标识
	BYTE							cbRouteState;						//路由状态
};

//游戏种类
struct tagGameKindEx : public tagGameKind
{
	WORD							wSupportMask;						//支持掩码	
};

//游戏节点
struct tagGameNode
{
	WORD							wKindID;							//名称索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wNodeID;							//节点索引
	TCHAR							szNodeName[LEN_NODE];				//节点名称
};

//网络信息
struct tagSocketInfo
{
	WORD							wGateID;							//网关标识
	DWORD							dwTokenID;							//令牌标识
};

//游戏房间
struct tagGameServer
{
	WORD							wModuleID;							//类型标识
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wVisibleMask;						//可视掩码
	WORD							wServerID;							//房间索引
	WORD							wServerPort;						//房间端口
	WORD							wServerType;						//房间类型
	WORD							wServerLevel;						//房间等级
	WORD							wChairCount;						//椅子数量
	DWORD							dwMaxPlayer;						//最大人数
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwServerRule;						//房间规则
	DWORD							dwServerAddr;						//服务地址
	DWORD							dwMasterRule;						//管理规则
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	TCHAR							szServerDomain[LEN_DOMAIN];			//服务域名

	//分数配置
	SCORE							lCellScore;							//单位积分	
	SCORE							lMinEnterScore;						//最低进入
	SCORE							lMaxEnterScore;						//最高进入	
};


//结构定义
struct tagPCGameServer
{
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wServerID;							//房间索引
	WORD							wServerPort;						//房间端口
	WORD							wServerType;						//房间类型
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwServerAddr;						//电信地址	
	TCHAR							szServerName[LEN_SERVER];			//房间名称

	//分数配置
	SCORE							lCellScore;							//单位积分	
	SCORE							lMinEnterScore;						//最低进入
	SCORE							lMaxEnterScore;						//最高进入
};

//结构定义
struct tagMBGameServer
{
	WORD							wKindID;							//名称索引
	WORD							wSortID;							//排序索引
	WORD							wServerID;							//房间索引
	WORD							wServerPort;						//房间端口
	WORD							wServerType;						//房间类型
	WORD							wServerLevel;						//房间等级
	WORD							wChairCount;						//椅子数量
	DWORD							dwMaxPlayer;						//最大人数
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwServerAddr;						//电信地址	
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	TCHAR							szServerDomain[LEN_DOMAIN];			//房间域名

	//分数配置
	SCORE							lCellScore;							//单位积分	
	SCORE							lMinEnterScore;						//最低进入
	SCORE							lMaxEnterScore;						//最高进入
};

//模块版本
struct tagModuleVersion
{
	bool							IsCurrent;							//当前版本
	DWORD							dwDeviceKind;						//设备类型
	WORD							wModuleID;							//模块标识	
	DWORD							dwVersion;							//终端版本
};


//定制类型
struct tagGameCustom
{
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wPageID;							//定制索引
	WORD							wOperateType;						//控制类型
	TCHAR							szDisplayName[LEN_CUSTOM];			//定制名称
	TCHAR							szReponseUrl[256];					//跳转地址
};

//过滤类型
struct tagGameFilter
{
	WORD							wSortID;							//排序索引
	WORD							wKindID;							//类型索引
	WORD							wNodeID;							//节点索引
	WORD							wServerID;							//房间索引
	WORD							wFilterMask;						//过滤掩码
	TCHAR							szServerName[LEN_SERVER];			//房间名称
};

//在线信息
struct tagOnLineInfoKind
{
	WORD							wKindID;							//类型标识
	DWORD							dwOnLineCount;						//在线人数
};

//在线信息
struct tagOnLineInfoServer
{
	WORD							wServerID;							//房间标识
	DWORD							dwOnLineCount;						//在线人数
};

//用户信息
struct tagGlobalUserInfo
{
	//用户属性
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	BYTE							cbGender;							//用户性别
	TCHAR							szNickName[LEN_NICKNAME];			//登录昵称

	//头像信息
	WORD							wFaceID;							//系统头像
	DWORD							dwCustomID;							//自定标识

	//用户状态
	BYTE							cbIMUserStatus;						//用户状态
	BYTE							cbCBUserStatus;						//用户状态

	//状态信息
	BYTE							cbShutUpStatus;						//禁言状态
	DWORD							dwShutUpOverTime;					//解禁时间

	//网络信息	
	tagSocketInfo					SocketInfo;							//网络信息
};

///////////////////////////////////////////////////////////////////////////////////////////////
//消息定义

//消息子项
struct tagMessageItem
{
	WORD							wMessageID;							//消息标识
	WORD							wServicePort;						//服务端口
	TCHAR							szServiceAddr[LEN_DOMAIN];			//服务地址	
};

//滚动配置
struct tagNoticeRollOption
{
	bool							bExclusive;						//独占标识
	BYTE							cbPriority;						//优先级别
	WORD							wRollTimes;						//滚动次数	
	WORD							wViewPlace;						//显示位置
	BYTE							cbNoticeKind;					//公告类型
	DWORD							dwDurationTime;					//持续时间
};

//公告信息
struct tagMarqueeNoticeInfo
{
	//公告信息
	DWORD							dwNoticeID;						//公告标识
	BYTE							cbNoticeKind;					//公告类型

	//滚动选项
	bool							bExclusive;						//独占标识
	BYTE							cbPriority;						//优先级别		
	WORD							wViewPlace;						//显示位置
	WORD							wRollTimes;						//滚动次数	
	DWORD							dwEndRollTime;					//结束时间
	DWORD							dwStartRollTime;				//开始时间			

	//公告内容
	WORD							wNoticeSize;					//公告大小
	BYTE							cbNoticeContent[512];			//公告内容	
};

//系统类公告
struct tagNoticeKind_System
{
	TCHAR							szNoticeContent[256];			//公告内容
};

//登录类公告
struct tagNoticeKind_UserLogon
{
	WORD							wRankID;						//排名标识
	BYTE							cbListType;						//榜单类型
	TCHAR							szNickName[LEN_NICKNAME];		//用户昵称
};

//游戏获胜类公告
struct tagNoticeKind_BigWinner
{
	SCORE							lWinScore;						//赢取分数
	TCHAR							szGameKind[LEN_KIND];			//游戏类型	
	TCHAR							szNickName[LEN_NICKNAME];		//用户昵称	
};

//奖池中奖类公告
struct tagNoticeKind_PrizePool
{
	SCORE							lAwardAmount;					//奖励金额	
	TCHAR							szGameKind[LEN_KIND];			//游戏类型	
	TCHAR							szNickName[LEN_NICKNAME];		//用户昵称	
};

///////////////////////////////////////////////////////////////////////////////////////////////
//比赛定义

//排名子项
struct tagMatchRankItem
{
	WORD							wRankID;							//名次标识
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	SCORE							lMatchScore;						//比赛分数
	TCHAR							szNickName[LEN_ACCOUNTS];			//玩家昵称
};

//比赛用户
struct tagMatchUserItem
{
	DWORD							dwUserID;							//用户标识
	WORD							wServerID;							//房间标识
	LONG							lWinCount;							//胜利局数
	LONG							lLostCount;							//失败次数
	LONG							lMaxWinStreak;						//连胜局数
	SCORE							lMatchScore;						//比赛积分
	WORD							wSignupCount;						//报名次数
	DWORD							dwSignupTime;						//报名时间
};

//报名用户
struct tagSignupUserItem
{
	DWORD							dwUserID;							//报名用户
	DWORD							dwSignupTime;						//报名时间
};

//轮次子项
struct tagMatchRoundItem
{
	WORD							wRoundID;							//轮次标识
	WORD							wPlayCount;							//游戏局数
	WORD							wStartCount;						//开始人数		
	WORD							wStartMinCount;						//开始人数		
	WORD							wStartMaxCount;						//开始人数		

	//比赛分数
	BYTE							cbMScoreSetType;					//赛分方式
	LONG							lInitMatchScore;					//初始比分
	WORD							wReserveRatio;						//保留比例

	//游戏底分
	LONG							lMaxBaseScore;						//游戏底分
	LONG							lInitBaseScore;						//游戏底分
	BYTE							cbIncreaseMode;						//增长模式
	WORD							wStartInning;						//起始局数
	WORD							wIntervalInning;					//间隔局数
	WORD							wIntervalSecond;					//间隔秒数
	WORD							wIncreaseRatio;						//增长比例		
	
	//晋级信息
	WORD							wPromoteCount;						//晋级人数
	BYTE							cbPromoteMode;						//晋级模式
	BYTE							cbPromoteRule[16];					//晋级规则
};

//定时赛
struct tagLockTimeMatch
{
	//比赛周期
	SYSTEMTIME						MatchStartTime;						//开始时间
	SYSTEMTIME						MatchCutOffTime;					//截止时间

	//开赛方式
	BYTE							cbMatchMode;						//开赛模式	
	BYTE							cbMatchCycle;						//开赛周期
	BYTE							cbDayFlagList[MAX_DAY];				//标识列表	
	SYSTEMTIME						MatchLockTime;						//开赛时间
	SYSTEMTIME						MatchInterval;						//比赛间隔
	SYSTEMTIME						MatchSignupTime;					//报名时间

	//游戏人数
	WORD							wMinPlayerCount;					//游戏人数
	WORD							wMaxPlayerCount;					//游戏人数

	//复活配置	
	WORD							wFeeMoneyID;						//费用类型
	LONG							lFeeAmount;							//费用金额
	LONG							lReviveMulti;						//复活倍数
	BYTE							cbReviveTimes;						//复活次数	
};

//即时赛
struct tagImmediateMatch
{
	//基础配置
	WORD							wMinPlayerCount;					//游戏人数
	WORD							wMaxPlayerCount;					//游戏人数

	//比赛时间
	SYSTEMTIME						MatchStartTime;						//开始时间
	SYSTEMTIME						MatchCutOffTime;					//截止时间
};

//锦标赛
struct tagTournamentMatch
{
	//比赛周期
	SYSTEMTIME						MatchStartTime;						//开始时间
	SYSTEMTIME						MatchCutOffTime;					//截止时间

	//开赛方式
	BYTE							cbMatchMode;						//开赛模式
	BYTE							cbMatchCycle;						//开赛周期
	BYTE							cbDayFlagList[MAX_DAY];				//标识列表		
	SYSTEMTIME						MatchLockTime;						//开赛时间
	SYSTEMTIME						MatchInterval;						//比赛间隔
	SYSTEMTIME						MatchDurationTime;					//持续时间
	SYSTEMTIME						MatchSaleTicketTime;				//售票时间
	SYSTEMTIME						MatchSignupStart;					//报名开始
	SYSTEMTIME						MatchSignupCutOff;					//报名截止

	//基础配置
	WORD							wPlayCount;							//游戏局数
	WORD							wMaxTableCount;						//最大桌数
	SCORE							lInitBaseScore;						//初始底分
	SCORE							lInitMatchScore;					//初始积分
	WORD							wMinPlayerCount;					//游戏人数
	WORD							wMaxPlayerCount;					//游戏人数

	//开赛方式
	BYTE							cbMonthFlagList[MAX_MONTH];			//标识列表
};

//定时赛详情
struct tagLockTimeGroupDetail
{
	//时间定义	
	SYSTEMTIME						MatchStartTime;						//开始时间
	SYSTEMTIME						MatchSignupStart;					//报名开始
	SYSTEMTIME						MatchSignupCutOff;					//报名截止
	SYSTEMTIME						ServerLocalTime;					//本地时间
};

//锦标赛详情
struct tagTournamentGroupDetail
{
	//时间定义	
	SYSTEMTIME						MatchStartTime;						//开始时间
	SYSTEMTIME						MatchSignupStart;					//报名开始
	SYSTEMTIME						MatchSignupCutOff;					//报名截止
	SYSTEMTIME						ServerLocalTime;					//本地时间
	SYSTEMTIME						MatchSaleTicketTime;				//售票时间
};

//即时赛详情
struct tagImmediateGroupDetail
{

};

//奖励子项
struct tagMatchRewardItem
{
	WORD							wEndRankID;							//奖励名次
	WORD							wStartRankID;						//奖励名次	
	//SCORE							lRewardGold;						//奖励金币
	//SCORE							lRewardIngot;						//奖励钻石
	//TCHAR							szRewardEntity[16];					//奖励实物

	TCHAR							szRewardEntity[128];				//实物奖励
	TCHAR							szRewardGoods[256];					//物品奖励
};

//比赛配置
struct tagMatchOption
{
	//基本信息
	WORD							wKindID;							//类型标识
	DWORD							dwMatchID;							//比赛标识	
	DWORD							dwClubID;							//茶社标识
	BYTE							cbShowType;							//展示类型
	BYTE							cbMatchStatus;						//比赛状态	
	BYTE							cbMatchType;						//比赛类型		
	TCHAR							szMatchName[32];					//比赛名称
	TCHAR							szModuleName[LEN_KIND];				//模块名称	
	
	//报名信息
	SCORE							lFeeAmount;							//费用金额
	WORD							wFeeMoneyID;						//货币标识
	WORD							wUseTicketCount;					//赛券数量
	WORD							wBuyTicketCount;					//赛券数量
	SCORE							lBuyTicketAmount;					//费用金额
	WORD							wBuyTicketMoneyID;					//货币标识
	WORD							wBuyTicketOption;					//购票选项
	WORD							wSignupCount;						//报名次数
	BYTE							cbSignupMode;						//报名方式
	BYTE							cbJoinCondition;					//参赛条件
	BYTE							cbMemberOrder;						//会员等级
	LONG							lExperience;						//玩家经验
	
	//比赛奖励
	WORD							wRewardCount;						//奖励数目
	tagMatchRewardItem				MatchRewardItem[8];					//奖励信息

	//比赛轮次
	WORD							wRoundCount;						//轮次数目
	tagMatchRoundItem				MatchRoundItem[5];					//轮次信息

	//比赛规则
	BYTE							cbMatchRule[256];					//比赛规则

	//比赛配桌
	BYTE							cbMatchDistribute[64];				//比赛配桌
};

//比赛配桌
struct tagMatchDistribute
{
	WORD							wDistributeRule;					//分配规则
	WORD							wDistributeMinUser;					//配桌用户
	WORD							wDistributeInterval;				//配桌间隔		
	WORD							wMinPartakeGameUser;				//最少人数
	WORD							wMaxPartakeGameUser;				//最大人数
};

//比赛分组
struct tagMatchSessionItem
{
	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次
	BYTE							cbMatchStatus;						//比赛状态

	//房间信息	
	WORD							wServerID;							//房间标识	
};

//比赛信息
struct tagMatchViewOption
{
	//基本信息
	WORD							wKindID;							//类型标识
	DWORD							dwMatchID;							//比赛标识
	DWORD							dwClubID;							//俱乐部标识
	BYTE							cbShowType;							//展示类型	
	BYTE							cbMatchType;						//比赛类型	
	TCHAR							szMatchName[32];					//比赛名称
	BYTE							cbOptionStatus;						//配置状态

	//报名信息
	SCORE							lFeeAmount;							//费用金额
	WORD							wFeeMoneyID;						//货币标识
	WORD							wUseTicketCount;					//赛券数量
	WORD							wBuyTicketCount;					//赛券数量
	SCORE							lBuyTicketAmount;					//费用金额
	WORD							wBuyTicketMoneyID;					//货币标识
	WORD							wSignupCount;						//报名次数
	BYTE							cbSignupMode;						//报名方式
	BYTE							cbJoinCondition;					//参赛条件
	BYTE							cbMemberOrder;						//会员等级
	LONG							lExperience;						//玩家经验

	//比赛时间
	SYSTEMTIME						MatchStartTime;						//开始时间
	SYSTEMTIME						MatchCutOffTime;					//截止时间

	//奖励信息
	WORD							wRewardCount;						//奖励数目
	tagMatchRewardItem				MatchRewardItem[8];					//比赛奖励
};

//打立出局
struct tagPromoteOutGame
{
	//WORD							wEliminateCount;					//淘汰人数	
	WORD							wEliminateMulti;					//淘汰倍数
	WORD							wDistributeCount;					//搓桌人数
};

//定局积分
struct tagPromoteFixedGame
{
	WORD							wSwitchCount;						//换桌局数
};

//比赛记录用户子项
struct tagMatchRecordUserItem
{
	DWORD							dwUserID;							//用户标识
	WORD							wRankID;							//比赛排名
	SCORE							lMatchScore;						//比赛积分
	TCHAR							szRewardEntity[64];					//比赛奖励
	TCHAR							szRewardGoods[128];					//比赛奖励
};

//比赛记录子项
struct tagMatchRecordItem
{
	SCORE							lMatchNo;							//比赛场次
	DWORD							dwMatchID;							//比赛标识
	BYTE							cbShowType;							//展示类型
	TCHAR							szMatchName[32];					//比赛名称	
	SYSTEMTIME						MatchStartTime;						//开始时间
	SYSTEMTIME						MatchFinishTime;					//结束时间
	tagMatchRecordUserItem			MatchRecordUserItem;				//自己记录
};


//比赛桌子信息
struct tagMatchTableInfo
{
	WORD							wServerID;							//房间标识	
	WORD							wTableID;							//桌子标识
	BYTE							cbGameStatus;						//游戏状态
	BYTE							cbTeamCount;						//团队数量
	SCORE							lTeamScore[4];						//团队积分
	WORD							wUserCount;							//用户数量
	DWORD							dwUserIDList[4];					//标识数组					
};

//////////////////////////////////////////////////////////////////////////////////////

//机器参数
struct tagRobotParameter
{
	//属性变量
	DWORD							dwBatchID;							//批次标识
	DWORD							dwServiceMode;						//服务模式
	DWORD							dwRobotCount;						//机器数目
	DWORD							dwEnterTime;						//进入时间
	DWORD							dwLeaveTime;						//离开时间
	SCORE							lTakeMinScore;						//携带分数
	SCORE							lTakeMaxScore;						//携带分数
	DWORD							dwEnterMinInterval;					//进入间隔
	DWORD							dwEnterMaxInterval;					//进入间隔
	DWORD							dwLeaveMinInterval;					//离开间隔
	DWORD							dwLeaveMaxInterval;					//离开间隔	
	DWORD							dwSwitchMinInnings;					//换桌局数
	DWORD							dwSwitchMaxInnings;					//换桌局数		
};

//////////////////////////////////////////////////////////////////////////
//排位信息

//桌子子项
struct tagRankingTableItem
{
	//房间信息
	WORD							wKindID;							//类型标识
	WORD							wTableID;							//桌子标识
	WORD							wServerID;							//房间标识
	DWORD							dwRankingNum;						//排位编号		
	TCHAR							szPassword[LEN_PASSWORD];			//桌子密码

	//创建信息	
	DWORD							dwCreateTime;						//创建时间	
};

//桌子子项
struct tagRankingTableItemEx
{
	//房间信息
	WORD							wKindID;							//类型标识
	WORD							wTableID;							//桌子标识
	WORD							wServerID;							//房间标识
	DWORD							dwRankingNum;						//排位编号	
	TCHAR							szPassword[LEN_PASSWORD];			//桌子密码

	//创建信息	
	DWORD							dwCreateTime;						//创建时间	
};

//桌子用户
struct tagRankingTableUser
{
	//用户信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	DWORD							dwUserID;							//用户标识
	DWORD							dwCustomID;							//头像标识
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称

	//段位信息
	WORD							wRankOrder;							//排位等级	
};

//排位配置
struct tagRankingConfig
{
	//基础配置
	WORD							wPlayCount;							//游戏局数
	WORD							wNotifyMask;						//通知掩码		
	LONG							lRankingCost;						//排位费用

	//转换比例
	WORD							wWinConvertRate;					//转换比例
	WORD							wLoseConvertRate;					//转换比例

	//爱心配置
	LONG							lRiseLoveHeartCount;				//升级数量
	LONG							lLostLoveHeartCount;				//失败数量	
};

//创建参数
struct tagRankingCreateParam
{
	//桌子信息
	DWORD							dwRankingNum;						//约战编号
	DWORD							dwCreateTime;						//创建时间
	TCHAR							szTablePassword[LEN_PASSWORD];		//桌子密码

	//配置信息
	tagRankingConfig				RankingConfig;						//排位配置
};

//排名子项
struct tagRankingRankItem
{
    WORD							wRankID;							//名次标识
    DWORD							dwUserID;							//用户标识
    //DWORD							dwGameID;							//游戏标识
    SCORE							lScore;								//排位分数
    //TCHAR							szNickName[LEN_ACCOUNTS];			//玩家昵称
};

//////////////////////////////////////////////////////////////////////////
//约战信息

//约战游戏选项
struct tagBattleGameOption
{
	WORD							wKindID;							//类型标识	
	BYTE							cbEmployScene;						//使用场景
	TCHAR							szOptionJson[4000];					//配置内容													
};

//自定义选项
struct tagBattleGameOptionEx
{
	tagBattleGameOption				BattleGameOption;					//游戏配置
};

//配置子项
struct tagBaseOptionItem
{
	WORD							wPlayCount;							//游戏局数
	LONG							lRoomCard;							//房卡消耗
	LONG							lRoomCardAA;						//房卡消耗
};

//约战基础配置
struct tagBattleBaseOption
{
	WORD							wKindID;							//类型标识
	BYTE							cbScoreKind;						//分数类型
	WORD							wPlayerCount;						//游戏人数
	BYTE							cbEmployScene;						//使用场景
	WORD							wOptionMask;						//配置掩码
	SCORE							lMinEntryCost;						//入场费用
	SCORE							lMaxEntryCost;		 				//入场费用	
	WORD							wWinnerCount;						//赢家数量
	LONG							lWinnerRevenue;						//税收比例	
	WORD							wSuportPayType;						//支付类型
	BYTE							cbItemCount;						//子项数目		
	SCORE							lMinBaseScore;						//最少底分
	SCORE							lMaxBaseScore;		 				//最高底分
	tagBaseOptionItem				OptionItem[8];						//选项子项
};

//约战选项
struct tagBattleBaseOptionEx
{
	WORD							wOptionID;							//选项标识
	tagBattleBaseOption				BattleOption;						//约战选项
};

//高级配置
struct tagAdvancedOption
{
	LONG							lRevenueRate;						//税收比例
	LONG							lMinWinScore;						//最低赢分	
};

//约战配置
struct tagBattleConfig
{
	//基础配置
	WORD							wKindID;							//类型标识
	SCORE							lEntryCost;							//入场费用
	BYTE							cbScoreKind;						//分数类型
	WORD							wPlayerCount;						//游戏人数	
	BYTE							cbEmployScene;						//使用场景	
	BYTE							cbSettleKind;						//结算类型
	WORD							wPlayCount;							//游戏局数	
	LONGLONG						lBaseScore;							//游戏底分

	//支付信息
	WORD							wPayType;							//支付方式
	BYTE							cbCurrencyKind;						//支付货币

	//选择配置
	BYTE							cbLookonMode;						//旁观模式
	BYTE							cbAllowVoice;						//允许语音
	BYTE							cbAllowSameIP;						//允许同IP	
	BYTE							cbAllowPublic;						//允许公开	

	//高级配置
	BYTE							cbAdvancedSize;						//配置大小	
	BYTE							cbAdvancedOption[64];				//高级配置

	//游戏配置
	BYTE							cbOptionSize;						//配置大小
	BYTE							cbGameOption[64];					//游戏配置	
};

//创建配置
struct tagBattleConfigEx : public tagBattleConfig
{	
	//创建信息	
	TCHAR							szPassword[LEN_PASSWORD];			//桌子密码

	//费用信息
	DWORD							dwPayUserID;						//付费玩家
	LONGLONG						lConsumeCount;						//消耗数量
	BYTE							cbConsumeIndex;						//消耗索引

	//系统配置		
	LONG							lGameRevenue;						//游戏税收
	WORD							wWinnerCount;						//赢家数量
	SCORE							lWinnerScore;						//赢家分数
	LONG							lWinnerRevenue;						//赢家税收			

	//附加信息	
	WORD							wNotifyMask;						//通知掩码	
	WORD							wBattleOptionID;					//配置标识	
};

//创建参数
struct tagBattleCreateParam
{
	//基础信息	
	DWORD							dwOwnerID;							//用户标识
	DWORD							dwBattleNum;						//约战编号
	DWORD							dwCreateTime;						//创建时间
	TCHAR							szPassword[LEN_PASSWORD];			//桌子密码

	//群组信息
	DWORD							dwClubID;							//群组标识
	WORD							wSwitchOption;						//群组规则
	WORD							wClubServiceID;						//服务标识
	DWORD							dwClubCreaterID;					//群主标识	

	//创建信息	
	DWORD							dwConfigID;							//玩法标识
	tagBattleConfigEx				BattleConfig;						//创建选项
};

//修改参数
struct tagBattleModifyParam
{
	//桌子信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwBattleNum;						//约战编号

	//配置信息	
	tagBattleConfigEx				BattleConfig;						//创建配置
};

//桌子用户
struct tagBattleTableUser
{
	//用户信息
	WORD							wFaceID;							//头像标识
	WORD							wChairID;							//用户方位
	BYTE							cbGender;							//用户性别
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	DWORD							dwCustomID;							//头像标识
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称

	//等级信息
	WORD							wGameOrder;							//游戏等级
	WORD							wMemberOrder;						//会员等级	
};

//桌子参数
struct tagBattleTableParam
{
	//状态信息
	DWORD							dwBattleNum;						//约战编号
	DWORD							dwStartTime;						//开始时间
	WORD							wFinishCount;						//完成局数
	BYTE							cbGameStatus;						//游戏状态

	//用户信息
	WORD							wUserCount;							//用户数量
	tagBattleTableUser				TableUserItem[0];					//用户子项
};

//桌子参数
struct tagBattleTableParamEx
{
	//状态信息		
	DWORD							dwStartTime;						//开始时间
	WORD							wFinishCount;						//完成局数	
	BYTE							cbGameStatus;						//游戏状态

	//用户信息
	WORD							wUserCount;							//用户数量
	tagBattleTableUser				TableUserItem[TABLE_USER_COUNT];	//用户信息
};

//桌子子项
struct tagBattleTableItem
{
	//房间信息
	WORD							wKindID;							//类型标识
	WORD							wTableID;							//桌子标识
	WORD							wServerID;							//房间标识
	DWORD							dwOwnerID;							//用户标识	
	DWORD							dwClubID;							//茶社标识
	DWORD							dwBattleNum;						//约战编号		

	//创建信息	
	SCORE							lWinnerScore;						//赢家奖励	
	bool							bLockState;							//锁定状态			
	DWORD							dwCreateTime;						//创建时间

	//配置信息
	DWORD							dwConfigID;							//玩法标识
	tagBattleConfig					BattleConfig;						//约战配置
};

//桌子子项
struct tagBattleTableItemEx
{
	//基础信息
	WORD							wTableID;							//桌子标识
	WORD							wServerID;							//房间标识
	DWORD							dwOwnerID;							//用户标识
	DWORD							dwBattleNum;						//约战编号
	TCHAR							szPassword[LEN_PASSWORD];			//桌子密码

	//茶社信息
	DWORD							dwClubID;							//茶社标识
	DWORD							dwClubCreaterID;					//群主标识
	WORD							wClubServiceID;						//服务标识
	WORD							wSwitchOption;						//茶社规则

	//创建信息	
	bool							bLockState;							//锁定状态	
	DWORD							dwCreateTime;						//创建时间

	//配置信息
	DWORD							dwConfigID;							//玩法标识
	tagBattleConfigEx				BattleConfig;						//约战配置
};

//玩家成绩
struct tagUserGradeItem
{		
	WORD							wChairID;							//椅子标识
	DWORD							dwUserID;							//用户标识
	SCORE							lAddupScore;						//用户成绩
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
};

//分数子项
struct tagBattleScoreItem
{
	//用户信息
	WORD							wChairID;							//用户方位
	DWORD							dwUserID;							//用户标识	
	DWORD							dwGameID;							//游戏标识	
	BYTE							cbUserStatus;						//用户状态
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称

	//用户属性
	WORD							wFaceID;							//头像标识	
	DWORD							dwCustomID;							//头像标识
	BYTE							cbGender;							//用户性别

	//税收信息
	SCORE							lSysRevenue;						//系统税收
	SCORE							lClubRevenue;						//茶社税收	

	//成绩信息
	SCORE							lScore;								//用户积分	
	LONG							lWinCount;							//胜利数目
	LONG							lLostCount;							//失败数目
	LONG							lDrawCount;							//平局数目	

	//摘要信息
	WORD							wRankID;							//名次标识
	SCORE							lRewardScore;						//奖励分数
	TCHAR							szBattleAbstract[128];				//约战摘要
};

//用户子项
struct tagBattleUserItem : public tagBattleScoreItem
{	
};

//用户子项
struct tagRankingUserItem
{
	//用户信息
	WORD							wChairID;							//用户方位
	DWORD							dwUserID;							//用户标识	
	DWORD							dwGameID;							//游戏标识	
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称

	//用户属性
	WORD							wFaceID;							//头像标识	
	DWORD							dwCustomID;							//头像标识
	BYTE							cbGender;							//用户性别

	//排位成绩
	SCORE							lInitScore;							//初始积分
	SCORE							lOverScore;							//结束积分	
};

//////////////////////////////////////////////////////////////////////////
//用户信息

//桌子状态
struct tagTableStatus
{
	BYTE							cbTableLock;						//锁定标志
	BYTE							cbPlayStatus;						//游戏标志
};

//用户状态
struct tagUserStatus
{
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子位置
	BYTE							cbUserStatus;						//用户状态
};

//用户积分
struct tagServerUserScore
{
	//积分信息
	SCORE							lScore;								//用户分数	
	BYTE							cbScoreKind;						//分数类型
	
	//游戏信息
	LONG							lWinCount;							//胜利盘数
	LONG							lLostCount;							//失败盘数
	LONG							lDrawCount;							//和局盘数
	LONG							lFleeCount;							//逃跑盘数
	LONG							lExperience;						//用户经验
	LONG							lWinStreak;							//连胜局数
	LONG							lMaxWinStreak;						//最大连胜
	SCORE							lBetAmount;							//下注金额
};

//用户积分
struct tagClientUserScore
{
	//积分信息
	SCORE							lScore;								//用户分数
	BYTE							cbScoreKind;						//分数类型
	
	//输赢信息
	LONG							lWinCount;							//胜利盘数
	LONG							lLostCount;							//失败盘数
	LONG							lDrawCount;							//和局盘数
	LONG							lFleeCount;							//逃跑盘数
	LONG							lExperience;						//用户经验
	LONG							lWinStreak;							//连胜局数
};

//用户信息
struct tagServerUserInfo
{
	//基本属性
	WORD							wFaceID;							//头像索引
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
	
	//用户资料
	BYTE							cbAge;								//用户年龄
	BYTE							cbGender;							//用户性别
	BYTE							cbMasterOrder;						//管理等级
	DWORD							dwMemberPoint;						//会员点数
	TCHAR							szArea[LEN_AREA];					//地区信息
	TCHAR							szCity[LEN_CITY];					//城市信息
	TCHAR							szProvince[LEN_PROVINCE];			//省份信息
	
	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态	

	//积分信息
	SCORE							lGold;								//用户金币	
	SCORE							lScore;								//用户分数
	SCORE							lUserCard;							//用户房卡
	SCORE							lInsureGold;						//银行金币	
	BYTE							cbScoreKind;						//分数类型

	//游戏信息
	LONG							lWinCount;							//胜利盘数
	LONG							lLostCount;							//失败盘数
	LONG							lDrawCount;							//和局盘数
	LONG							lFleeCount;							//逃跑盘数
	LONG							lExperience;						//用户经验
	LONG							lWinStreak;							//连胜局数
	LONG							lMaxWinStreak;						//连胜局数		

	//自定头像
	DWORD							dwCustomID;							//自定标识
};

//用户信息
struct tagClientUserInfo
{
	//基本属性	
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	bool							bIsRobot;							//机器标识
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名	

	//用户资料
	BYTE							cbAge;								//用户年龄
	BYTE							cbGender;							//用户性别
	BYTE							cbMasterOrder;						//管理等级
	DWORD							dwMemberPoint;						//会员点数
	TCHAR							szArea[LEN_AREA];					//地区信息
	TCHAR							szCity[LEN_CITY];					//城市信息
	TCHAR							szProvince[LEN_PROVINCE];			//省份信息

	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态

	//积分信息
	SCORE							lGold;								//用户金币	
	SCORE							lScore;								//用户分数
	SCORE							lInsureGold;						//银行金币
	BYTE							cbScoreKind;						//分数类型

	//游戏信息
	LONG							lWinCount;							//胜利盘数
	LONG							lLostCount;							//失败盘数
	LONG							lDrawCount;							//和局盘数
	LONG							lFleeCount;							//逃跑盘数
	LONG							lExperience;						//用户经验
	LONG							lWinStreak;							//连胜局数
	LONG							lMaxWinStreak;						//连胜局数

	//自定头像
	WORD							wFaceID;							//头像索引
	DWORD							dwCustomID;							//自定标识
};

//用户信息
struct tagPCUserInfoHead
{
	//用户属性
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwUserID;							//用户 I D
	bool							bIsRobot;							//机器标识

	//头像信息
	WORD							wFaceID;							//头像索引
	DWORD							dwCustomID;							//自定标识

	//用户属性
	BYTE							cbAge;								//用户年龄
	BYTE							cbGender;							//用户性别
	BYTE							cbMasterOrder;						//管理等级

	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态

	//积分信息
	SCORE							lGold;								//用户黄金
	SCORE							lScore;								//用户分数
	SCORE							lInsureGold;						//银行金币
	BYTE							cbScoreKind;						//分数类型

	//游戏信息
	LONG							lWinCount;							//胜利盘数
	LONG							lLostCount;							//失败盘数
	LONG							lDrawCount;							//和局盘数
	LONG							lFleeCount;							//逃跑盘数
	LONG							lWinStreak;							//连胜局数
	LONG							lExperience;						//用户经验	
};

//用户信息
struct tagMBUserInfoHead
{
	//用户属性
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwUserID;							//用户 I D

	//头像信息
	WORD							wFaceID;							//头像索引
	DWORD							dwCustomID;							//自定标识
	
	//用户属性
	BYTE							cbAge;								//用户年龄
	BYTE							cbGender;							//用户性别
	BYTE							cbMasterOrder;						//管理等级

	//用户状态
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态

	//积分信息
	SCORE							lGold;								//用户黄金
	SCORE							lScore;								//游戏分数
	BYTE							cbScoreKind;						//分数类型
																		
	//游戏信息
	LONG							lWinCount;							//胜利盘数
	LONG							lLostCount;							//失败盘数
	LONG							lDrawCount;							//和局盘数
	LONG							lFleeCount;							//逃跑盘数
	LONG							lWinStreak;							//连胜局数
	LONG							lExperience;						//用户经验	

	//位置信息
	DOUBLE							dGlobalPosLat;						//纬度信息
	DOUBLE							dGlobalPosLng;						//经度信息	
	TCHAR							szPlaceName[LEN_PLACE_NAME];		//地址信息
};

//排位信息
struct tagUserRankingInfo
{
	DWORD							dwRankID;							//排名标识	
	WORD							wSeasonID;							//赛季标识
	LONG							lWinCount;							//赢局数目
	LONG							lLostCount;							//数局数目
	LONG							lWinStreakCount;					//连胜局数
	LONG							lLoveHeart;							//爱心数量
	SCORE							lRankScore;							//排位积分	
};

//头像信息
struct tagCustomFaceInfo
{
	DWORD							dwDataSize;							//数据大小
	DWORD							dwCustomFace[FACE_CX*FACE_CY];		//图片信息
};

//用户信息
struct tagDebugUserInfo
{
	//基础信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	bool							bIsRobot;							//机器标识
	SCORE							lHistoryKindIDWin;					//该游戏历史总输赢
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称

	//财富信息
	SCORE							lUserScore;							//用户分数
	SCORE							lInsureScore;						//银行分数	

	//状态信息
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子索引
	BYTE							cbUserStatus;						//用户状态
};

//实名信息
struct tagUserRealAuth
{
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//证件号码
};

//银行配置参数
struct tagInsureConfig
{
	SCORE							lReserveAmount;						//保留额度
	SCORE							lDealMin;							//单笔最低额度
	SCORE							lDealMax;							//单笔最高额度
	SCORE							lDailyMax;							//每日限额
	DWORD							dwFeeRate;							//手续费率(千分比)
};
//////////////////////////////////////////////////////////////////////////
//IM结构定义

//公开信息
struct tagIMUserPublicInfo
{
	//基础信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	BYTE							cbGender;							//用户性别
	WORD							wMasterOrder;						//管理等级	
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称

	//形象信息
	WORD							wFaceID;							//头像标识
	DWORD							dwCustomID;							//头像标识	

	//游戏信息
	SCORE							lRankScore;							//排位分数
	DWORD							dwExperience;						//用户经验
	DWORD							dwMemberPoint;						//会员点数	
	DWORD							dwLoveliness;						//魅力值
	
	//其他信息
	BYTE							cbInfoPublic;						//公开标志
	DWORD							dwLikedNumber;						//点赞数量
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
};

//////////////////////////////////////////////////////////////////////////
//茶社定义

//茶社配置
struct tagClubKindOption
{
	BYTE							cbClubKind;							//茶社类型
	BYTE							cbPayTypeMask;						//支付掩码
	BYTE							cbCurrencyKindMask;					//货币掩码
	DWORD							dwNeedUserRight;					//用户权限
	WORD							wMinRevenueRate;					//最低税收
	WORD							wMaxRevenueRate;					//最高税收
	SCORE							lCreateClubTakeCard;				//携带房卡
	SCORE							lCreateClubDeductCard;				//扣除房卡
	SCORE							lModifyClubNameDeductCard;			//扣除房卡
};

//茶社配置
struct tagClubOption
{
	//基础选项 
	WORD							wSupportClubKind;					//支持类型
	WORD							wMaxJoinClubCount;					//加入数量
	WORD							wMaxCreateClubCount;				//创建数量	
	WORD							wMaxBattleConfigCount;				//玩法数量

	//类型选项
	tagClubKindOption				ClubKindOption[CLUB_KIND_COUNT];	//类型选项
};

//茶社属性
struct tagClubProperty
{
	//基础信息
	DWORD							dwClubID;							//茶社标识	
	DWORD							dwCreaterID;						//创建者标识
	DWORD							dwCreaterGameID;					//创建者标识	
	WORD							wClubKind;							//茶社级别
	WORD							wClubIconID;						//图标标识
	TCHAR							szClubName[LEN_CLUB_NAME];			//茶社名称	

	//成员信息
	WORD							wMemberCount;						//成员数量
	WORD							wMaxMemberCount;					//成员数量

	//代付信息
	DWORD							dwPayrollerID;						//代付者标识

	//创建信息
	SYSTEMTIME						CreateDateTime;						//创建时间
	TCHAR							szClubIntroduce[LEN_CLUB_INTRODUCE];//茶社介绍	
};

//茶社名片
struct tagClubNameCard
{
	//茶社信息
	DWORD							dwClubID;							//茶社标识		
	WORD							wClubKind;							//茶社级别
	WORD							wClubIconID;						//茶社图标
	WORD							wMemberCount;						//成员数量	
	WORD							wMaxMemberCount;					//最大数量	
	TCHAR							szClubName[LEN_CLUB_NAME];			//茶社名称	

	//创建者信息
	DWORD							dwCreaterID;						//群主标识
	DWORD							dwCreaterGameID;					//群主标识
	WORD							wCreaterFaceID;						//群组头像
	DWORD							dwCreaterCustomID;					//群组头像
	TCHAR							szCreaterNickName[LEN_ACCOUNTS];	//群主昵称

};

//约战玩法
struct tagClubBattleConfig
{
	WORD							wKindID;							//类型标识	
	DWORD							dwConfigID;							//玩法标识
	TCHAR							szBattleConfig[512];				//约战配置
};

//游戏设置
struct tagClubKindSetting
{
	WORD							wKindID;							//类型标识
	SCORE							lMinWinnerScore;					//最低赢分
};

//茶社规则
struct tagClubRules
{	
	BYTE							cbPayType;							//支付类型
	BYTE							cbCurrencyKind;						//货币类型
	BYTE							cbScoreKind;						//游戏分类型
	BYTE							cbBattleMask;						//约战掩码	
	WORD							wSwitchOption;						//开关选项	
};

//茶社公告
struct tagClubNotice
{
	BYTE							cbPublishMode;						//发布模式
	BYTE							cbPublishState;						//发布状态	
	TCHAR							szNoticeContent[LEN_CLUB_NOTICE];	//公告内容
};

//茶社子项
struct tagClubItem	
{	
	tagClubRules					ClubRules;							//茶社规则
	tagClubNotice					ClubNotice;							//茶社公告
	tagClubProperty					ClubProperty;						//茶社属性	
};

//用户信息
struct tagClubUserInfo
{
	//基本属性	
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D	
	BYTE							cbGender;							//用户性别
	BYTE							cbUserStatus;						//用户状态
	TCHAR							szNickName[LEN_ACCOUNTS];			//玩家昵称	

	//自定头像
	WORD							wFaceID;							//系统头像
	DWORD							dwCustomID;							//自定标识
};

//茶社成员
struct tagClubMemberInfo
{	
	//成员信息
	DWORD							dwMemberID;							//成员标识
	BYTE							cbMemberType;						//成员类型
	BYTE							cbMemberStatus;						//成员状态	
	DWORD							dwManageRight;						//管理权限
	SYSTEMTIME						JoinDateTime;						//加入时间
	SYSTEMTIME						LastBattleTime;						//约战时间

	//约战信息
	SCORE							lClubScore;							//茶社分数
	SCORE							lRevenueCount;						//累计税收
	DWORD							dwCreateCount;						//创建次数
	DWORD							dwBattleCount;						//参与次数
};

//茶社成员
struct tagClubMemberUser
{	
	tagClubUserInfo					UserInfo;							//用户信息	
	tagClubMemberInfo				MemberInfo;							//成员信息		
};

//用户子项
struct tagClubUserItem
{
	//基础信息
	DWORD							dwUserID;							//用户 I D
	tagSocketInfo					SocketInfo;							//网络信息

	//状态信息
	BYTE							cbShutUpStatus;						//禁言状态
	DWORD							dwShutUpOverTime;					//解禁时间
};

//申请消息
struct tagClubApplyMessage
{
	DWORD							dwMessageID;						//消息标识
	BYTE							cbMessageStatus;					//消息状态

	//申请信息
	DWORD							dwApplyerID;						//用户标识	
	DWORD							dwApplyerGameID;					//游戏标识
	BYTE							cbApplyerGender;					//用户性别
	WORD							wApplyerFaceID;						//系统头像
	DWORD							dwApplyerCustomID;					//自定头像
	TCHAR							szApplyerNickName[LEN_ACCOUNTS];	//用户昵称
	SYSTEMTIME						ApplyDateTime;						//申请时间
	SYSTEMTIME						OperateDateTime;					//操作时间

	//茶社信息
	DWORD							dwClubID;							//茶社标识	
	DWORD							dwResponderID;						//操作者标识	
	TCHAR							szClubName[LEN_CLUB_NAME];			//茶社名称	
};

//规则更新
struct tagClubUpdate_Rules
{
	BYTE							cbBattleMask;						//约战掩码	
	WORD							wSwitchOption;						//开关选项	
};

//公告更新
struct tagClubUpdate_Notice
{
	BYTE							cbPublishMode;						//发布模式
	BYTE							cbPublishState;						//发布状态	
	TCHAR							szNoticeContent[LEN_CLUB_NOTICE];	//公告内容	
};

//属性更新
struct tagClubUpdate_Property
{
	WORD							wClubIconID;						//图标标识
	TCHAR							szClubName[LEN_CLUB_NAME];			//茶社名称
	TCHAR							szClubIntroduce[LEN_CLUB_INTRODUCE];//茶社介绍	
};

//////////////////////////////////////////////////////////////////////////
//公共结构

//级别子项
struct tagLevelItem
{
	LONG							lLevelScore;						//级别积分
	TCHAR							szLevelName[16];					//级别描述
};

//会员子项
struct tagMemberItem
{
	BYTE							cbMemberOrder;						//等级积分
	TCHAR							szMemberName[16];					//等级名字
};

//管理子项
struct tagMasterItem
{
	BYTE							cbMasterOrder;						//等级积分
	TCHAR							szMasterName[16];					//等级名字
};

//列表子项
struct tagColumnItem
{
	BYTE							cbColumnWidth;						//列表宽度
	BYTE							cbDataDescribe;						//字段类型
	TCHAR							szColumnName[16];					//列表名字
};

//机器标识
struct tagComputerID
{
	DWORD							dwSystemVer;						//系统版本
	DWORD							dwComputerID[3];					//机器序列
};

//代理子项
struct tagAgentItem
{
	WORD							wAgentKind;							//代理类型
};

//地址信息
struct tagAddressInfo
{
	DWORD							dwServiceAddr;						//电信地址
	TCHAR							szServiceDomain[LEN_DOMAIN];		//服务域名
};

//数据库信息
struct tagDataBaseParameter
{
	WORD							wDataBasePort;						//数据库端口
	DWORD							dwDataBaseAddr;						//数据库地址
	TCHAR							szDataBaseUser[32];					//数据库用户
	TCHAR							szDataBasePass[32];					//数据库密码
	TCHAR							szDataBaseName[32];					//数据库名字
};

//连接选项
struct tagRedisConnOption
{
	WORD							wDB;								//数据索引	
	WORD							wPort;								//连接端口	
	TCHAR							szHost[32];							//连接地址
	TCHAR							szUser[32];							//连接用户
	TCHAR							szPassword[32];						//连接密码	
};

//游戏房间
struct tagGameServerItem
{
	WORD							wServerID;							//房间索引
	WORD							wServerPort;						//房间端口
	DWORD							dwServerAddr;						//服务地址
	TCHAR							szServerName[LEN_SERVER];			//房间名称
};

//财富子项
struct tagWealthItem
{
	WORD							wMoneyID;							//货币标识
	SCORE							lBalance;							//货币余额
};

//用户等级
struct tagUserLevelItem
{
	WORD							wLevelID;							//等级标识
	LONG							lNeedPoint;							//所需点数
	TCHAR							szLevelName[32];					//等级名称
	TCHAR							szRewards[256];						//升级奖励
};

//游戏等级
struct tagGameLevelItem
{
	WORD							wLevelID;							//等级标识	
	LONG							lNeedPoint;							//所需点数
	TCHAR							szRewards[256];						//升级奖励
};

//排位等级
struct tagRankLevelItem
{
	//等级信息
	WORD							wLevelID;							//等级标识	
	LONG							lNeedScore;							//所需积分
	TCHAR							szLevelName[32];					//等级名称

	//段位信息
	BYTE							cbMainOrder;						//段位顺序
	BYTE							cbSubOrder;							//级别顺序
	BYTE							cbStarOrder;						//星级顺序
};

//////////////////////////////////////////////////////////////////////////

//修改选项
struct tagModifyOption
{
	DWORD							dwTableRuleAdd;						//桌子规则	
	DWORD							dwTableRuleDel;						//桌子规则	
	DWORD							dwServerRuleAdd;					//房间规则		
	DWORD							dwServerRuleDel;					//房间规则
};

//执行命令
struct tagExecuteCommand
{
	DWORD							dwCommandCode;						//命令编码
	DWORD							dwCommandParam;						//命令参数
};

//////////////////////////////////////////////////////////////////////////
//录像定义

//录像数据头
struct tagVideoHead
{
	DWORD							dwStartTime;						//开始时间
	DWORD							dwDataSize;							//数据大小	
	WORD							wPacketCount;						//数据数目
	BYTE							cbUserCount;						//用户数量
	BYTE							cbCompressKind;						//压缩类型	
};

//录像用户
struct tagVideoUser
{
	DWORD							dwUserID;							//用户标识
	WORD							wChairID;							//椅子方位
	TCHAR							szNickName[LEN_ACCOUNTS];			//玩家昵称
};

//录像数据包
struct tagVideoPacket
{	
	//控制信息
	WORD							wPacketSize;						//数据大小
	DWORD							dwPacketPos;						//数据位置
	DWORD							dwInsertTime;						//插入时间	

	//位置信息
	WORD							wChairID;							//玩家方位

	//数据信息
	WORD							wMainCmdID;							//主命令码
	WORD							wSubCmdID;							//子命令码
	WORD							wDataSize;							//数据大小
};

//结构定义
struct tagVideoServer
{
	//类型信息
	WORD							wKindID;							//名称号码
	WORD							wChairCount;						//椅子数目
	TCHAR							szProcessName[LEN_PROCESS];			//进程名字

	//房间信息
	WORD							wServerID;							//房间号码
	WORD							wServerType;						//房间类型		
	DWORD							dwServerRule;						//房间规则
	TCHAR							szServerName[LEN_SERVER];			//房间名称	
};

//////////////////////////////////////////////////////////////////////////

//物品子项
struct tagGoodsItem
{
	WORD							wGoodsID;							//物品标识
	WORD							wGoodsLevel;						//物品等级
	DWORD							dwGoodsCount;						//物品数量
	DWORD							dwGoodsIndate;						//物品有效期
};

//物品数量
struct tagGoodsCountItem
{
	WORD							wGoodsID;							//物品标识
	DWORD							dwGoodsCount;						//物品数量
};

//物品信息
struct tagGoodsInfo
{
	WORD							wGoodsID;							//物品标识
	WORD							wGoodsKind;							//物品类型
	bool							bHasIndate;							//有效期
	WORD							wCanTradeType;						//交易类型
	WORD							wCanStorePlace;						//存放位置	
	TCHAR							szGoodsName[LEN_GOODS_NAME];		//物品名称
	TCHAR							szGoodsOption[LEN_GOODS_OPTION];	//物品选项
};

//物品持有信息
struct tagGoodsHoldInfo
{
	WORD							wGoodsID;							//物品标识
	WORD							wGoodsKind;							//物品类型
	WORD							wGoodsLevel;						//物品等级
	DWORD							dwGoodsCount;						//物品数量
	DWORD							dwGoodsIndate;						//物品有效期	
	DWORD							dwExpireTime;						//过期时间
};

//赠送礼物信息
struct tagGiftInfo
{
    DWORD							dwRecordID;							//记录ID
    DWORD							dwUserID;							//用户标识
    DWORD							dwGoodsID;							//商品标识
    DWORD							dwGoodsCount;						//物品数量
	DWORD							dwLoveLiness;						//附赠魅力
    SYSTEMTIME						SendDate;							//赠送时间	
    TCHAR							szNickName[LEN_ACCOUNTS];			//用户帐号
};

//////////////////////////////////////////////////////////////////////////
//代理定义

//映射子项
struct tagMappedItem
{
	WORD							wServicePort;						//监听端口
	WORD							wRemotePort;						//远程端口
	DWORD							dwRemoteAddr;						//远程地址
};

//端口对
struct tagPortPair
{
	WORD							wServicePort;						//监听端口
	WORD							wForwardPort;						//转发端口
};

//代理选项
struct tagAgentOption
{
	WORD							wAgentID;							//代理标识	
	TCHAR							szAgentName[32];					//代理名称
	WORD							wAgentKind;							//代理类型
	WORD							wMaxConnect;						//最大连接	
	WORD							wMappedCount;						//映射数目
	tagMappedItem					MappedItem[16];						//映射子项目
};
//////////////////////////////////////////////////////////////////////////
//邮件定义

//邮件子项
struct tagUserMailItem
{
	//邮件头部
	DWORD							dwToUserID;							//接收标识
	WORD							wFromModule;						//发送模块	
	TCHAR							szMailTitle[32];					//邮件标题		

	//邮件内容
	WORD							wMailKind;							//邮件类型
	WORD							wDataSize;							//数据大小
	BYTE							cbDataBuffer[256];					//数据缓冲
};

//邮件信息
struct tagMailInfo
{
	DWORD							dwMailID;						//邮件标识
	BYTE							cbMailKind;						//邮件种类	
	BYTE							cbMailType;						//邮件类型
	BYTE							cbMailState;					//邮件状态
	DWORD							dwSendTime;						//发送时间
	DWORD							dwExpireTime;					//过期时间	
	TCHAR							szMailTitle[LEN_MAIL_TITLE];	//邮件标题
	TCHAR							szMailContent[LEN_MAIL_CONTENT];//邮件内容
	TCHAR							szAttachGoods[LEN_MAIL_ATTACH];	//附带信息	
};

//////////////////////////////////////////////////////////////////////////

//抽水配置
struct tagShrinkItem
{
	WORD							wShrinkRatio;					//抽水比例
	SCORE							lShrinkProfit;					//抽水利润	
};

//彩池信息
struct tagJackpotItem
{
	WORD							wLevelID;						//彩池等级
	SCORE							lStockScore;					//彩金积分
	SCORE							lExtraScore;					//额外积分
	SCORE							lInitialScore;					//初始彩金
	SCORE							lVirtualScore;					//虚拟积分
	WORD							wShrinkRatio;					//抽取比例
	WORD							wPayoutProb;					//派彩概率
	SCORE							lPayoutLimit;					//派彩限制
	SCORE							lPayoutOrigin;					//派彩起点
};

//库存基础信息
struct tagStockBase
{
	//基本信息
	WORD							wKindID;						//游戏类型
	WORD							wStockID;						//排序标识
	WORD							wStockKind;						//库存类型
	WORD							wStockLevel;					//库存级别
	BYTE							cbStockState;					//库存状态
	LONG							lBetAmount;						//下注金额

	//启用配置
	BYTE							cbEnableMode;					//启用模式
	DWORD							dwEnableEndTime;				//结束时间
	DWORD							dwEnableStartTime;				//开始时间

	//积分信息
	SCORE							lStockScore;					//库存积分
	SCORE							lExtraScore;					//额外配额

	//抽水配置
	SCORE							lShrinkProfit;					//抽水利润	
};

//库存子项
struct tagStockItem : public tagStockBase
{
	//关联信息
	WORD							wRelationCount;					//关联数量
	WORD							RelationStockID[20];			//关联标识	

	//抽水配置
	WORD							wShrinkCount;					//配置数量
	tagShrinkItem					ShrinkItem[2];					//抽水配置

	//奖池配置
	WORD							wJackpotCount;					//彩池数量
	tagJackpotItem					GameJackpotItem[MAX_JACKPOT_COUNT];//彩池配置
};

//库存信息
struct tagStockInfo : public tagStockBase
{
	//库存名称
	TCHAR							szStockName[32];				//库存名称

	//关联信息
	CHAR							szRelationStockID[82];			//关联标识	

	//抽水配置
	CHAR							szShrinkOption[128];			//配置数量
	
	//奖池配置
	WORD							wJackpotCount;					//彩池数量
	tagJackpotItem					GameJackpotItem[MAX_JACKPOT_COUNT];//彩池配置
};

//大R控制
struct tagSuperPlayerControl
{
	bool							bEanbleState;					//启用状态
	WORD							wShrinkCount;					//配置数量
	tagShrinkItem					ShrinkItem[2];					//抽水配置
};

//库存控制
struct tagStockControl
{
	tagSuperPlayerControl			SuperPlayerControl;				//大R控制
};

//倍数权重
struct tagTimesWeightItem
{
	DWORD							dwTimesRange[2];				//倍数区间
	BYTE							cbTimesKind;					//倍数类型
	BYTE							cbWeightType;					//权重类型	
	WORD							wTimesWeight;					//倍数权重
	float							fWightRatio;					//权重比例
};

//低保信息
struct tagSubsidyInfo
{
	BYTE							cbResidualTimes;				//剩余次数
	bool							bLargessSubsidy;				//赠送标识
	SCORE							lConsumeScore;					//消费金币(邮件、礼物、转账等)
};
//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif