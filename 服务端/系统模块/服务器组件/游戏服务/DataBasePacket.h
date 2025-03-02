#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//请求数据包

//内核事件
#define DBR_GR_LOAD_CONFIG			(DB_FRAME_REQUEST_START+1)			//加载配置
#define DBR_GR_LOAD_ROBOTS			(DB_FRAME_REQUEST_START+2)			//加载用户

//登陆事件
#define	DBR_GR_LOGON_ROBOT			(DB_FRAME_REQUEST_START+100)		//机器登录
#define	DBR_GR_LOGON_MOBILE			(DB_FRAME_REQUEST_START+101)		//手机登录
#define	DBR_GR_LOGON_COMPUTER		(DB_FRAME_REQUEST_START+102)		//电脑登录

//系统事件
#define DBR_GR_WRITE_GAME_SCORE		(DB_FRAME_REQUEST_START+200)		//游戏积分
#define DBR_GR_WRITE_USER_WEALTH	(DB_FRAME_REQUEST_START+201)		//游戏财富
#define DBR_GR_LEAVE_SERVICE		(DB_FRAME_REQUEST_START+202)		//离开房间
#define DBR_GR_RECORD_DRAW_INFO		(DB_FRAME_REQUEST_START+203)		//积分记录
#define DBR_GR_RECORD_DRAW_SCORE	(DB_FRAME_REQUEST_START+204)		//积分记录
#define DBR_GR_RECORD_GAME_VIDEO	(DB_FRAME_REQUEST_START+205)		//积分记录
#define DBR_GR_MANAGE_USER_RIGHT	(DB_FRAME_REQUEST_START+206)		//权限管理
#define DBR_GR_RELOCK_SCORE			(DB_FRAME_REQUEST_START+207)		//重新锁分
#define DBR_GR_LARGESS_SUBSIDY		(DB_FRAME_REQUEST_START+210)		//赠送低保

//背包事件
#define DBR_GR_USER_QUERY_GOODS		(DB_FRAME_REQUEST_START+270)		//查询物品
#define DBR_GR_USER_UPDATE_GOODS	(DB_FRAME_REQUEST_START+271)		//更新物品
#define DBR_GR_USER_RECORD_GOODS	(DB_FRAME_REQUEST_START+272)		//记录物品

//输赢控制
#define DBR_GR_CONTROL_START		(DB_FRAME_REQUEST_START+280)		//控制开始
#define DBR_GR_CONTROL_FINISH		(DB_FRAME_REQUEST_START+281)		//控制结束
#define DBR_GR_CONTROL_UPDATE		(DB_FRAME_REQUEST_START+282)		//控制更新
#define DBR_GR_CONTROL_QUERY		(DB_FRAME_REQUEST_START+283)		//控制查询

//赠送礼物
#define DBR_GR_SEND_GIFT			(DB_FRAME_REQUEST_START+290)		//赠送礼物
#define DBR_GR_UPDATE_LOVELINESS	(DB_FRAME_REQUEST_START+291)		//更新魅力

//离开房间原因
#define LEAVE_GAME_NORMAL			0									//正常退出
#define LEAVE_GAME_USER_CANCEL		1									//用户取消登录
#define LEAVE_GAME_THAN_MAX_SCORE	2									//积分过高
#define LEAVE_GAME_FULL				3									//房间满人
#define LEAVE_GAME_MANAGEMENT		4									//房间维护
#define LEAVE_GAME_ACTIVE_FAILURE	5									//激活用户失败
#define LEAVE_GAME_BIND_ERROR		6									//绑定错误
#define LEAVE_GAME_TWICE_LOGIN		7

//////////////////////////////////////////////////////////////////////////

//加载配置
struct DBR_GR_LoadConfig
{
	bool							bInitService;						//初始标识
	DWORD							dwConfigMask;						//配置掩码
};

//加载机器
struct DBR_GR_LoadRobotUser
{
	DWORD							dwBatchID;							//批次标识
	DWORD							dwRobotCount;						//机器数目
};

//电脑登录
struct DBR_GR_LogonComputer
{		
	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//登陆密码	
	
	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次

	//积分锁定
	SCORE							lRequestScore;						//锁定积分

	//附加信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//机器登录
struct DBR_GR_LogonRobot
{
	//登录信息
	DWORD							dwUserID;							//用户 I D	

	//携带信息
	BYTE							cbClientKind;						//终端类型	

	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次

	//积分锁定
	SCORE							lRequestScore;						//锁定积分

	//附加信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//ID 登录
struct DBR_GR_LogonMobile
{		
	//登录信息
	DWORD							dwUserID;							//用户 I D
	DWORD						    dwDeviceKind;						//设备类型
	
	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次

	//积分锁定
	SCORE							lRequestScore;						//锁定积分

	//位置信息
	double							dGlobalPosLng;						//定位经度
	double							dGlobalPosLat;						//定位纬度
	TCHAR							szPlaceName[LEN_PLACE_NAME];		//地址信息

	//约战排位
	bool							bLookon;							//旁观标识
	WORD							wTableID;							//桌子标识
	WORD							wChairID;							//椅子标识	
	DWORD							dwClubID;							//茶社标识
	DWORD							dwBattleNum;						//约战房号
	BYTE							cbScoreKind;						//分数类型
	DWORD							dwRankingNum;						//排位房号	
	TCHAR							szTablePasswd[LEN_PASSWORD];		//桌子密码

	//附加信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

struct DBR_GR_Update_Gold
{		
	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//附加信息
	DWORD							dwClientIP;							//连接地址
};

//游戏积分
struct DBR_GR_WriteGameScore
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwClientIP;							//连接地址
	DWORD							dwPlayTimeCount;					//游戏时间

	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lRevenue;							//游戏税收
	SCORE							lUnLockScore;						//释放积分

	//游戏信息
	LONG							lWinCount;							//胜利盘数
	LONG							lLostCount;							//失败盘数
	LONG							lDrawCount;							//和局盘数
	LONG							lFleeCount;							//断线数目
	LONG							lExperience;						//用户经验
	LONG							lWinStreak;							//连胜局数
	LONG							lMaxWinStreak;						//连胜局数	
	SCORE							lBetAmount;							//下注金额

	//累计输赢
	WORD							wAddupWinRate;						//累计胜率
	SCORE							lAddupWinScore;						//累计赢分
	SCORE							lAddupLoseScore;					//累计输分

	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次

	//附加信息
	DWORD							dwInoutIndex;						//进出索引
};

//写入福利
struct DBR_GR_WriteUserWelfare
{
	//用户信息
	DWORD							dwUserID;							//用户标识

	//积分信息
	SCORE							lPresentGold;						//赠送金币	

	//附加信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//赠送低保
struct DBR_GR_LargessSubsidy
{
	//用户信息
	DWORD							dwUserID;							//用户标识

	//附加信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//更新库存
struct DBR_GR_UpdateStock
{
	WORD							wKindID;							//类型标识
	WORD							wStockID;							//库存标识
	WORD							wStockKind;							//库存类型
	DOUBLE							lVariationScore;					//变化库存
};

//购买道具
struct DBR_GR_PurcaseProperty
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwPropKind;							//道具类型	
	LONG							lPropPrice;							//道具价格	
};

//查询馆分
struct DBR_GR_QueryGroupScore
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	BYTE							cbChangeType;						//变更类型	
	SCORE							lChangeScore;						//变更分数

	//茶馆信息
	DWORD							dwGroupID;							//茶馆标识
	TCHAR							szGroupName[LEN_GROUP_NAME];		//茶馆名称
	TCHAR							szOperaterNickName[LEN_ACCOUNTS];	//用户昵称
};

//查询馆钻
struct DBR_GR_QueryGroupCowry
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	BYTE							cbChangeType;						//变更类型	
	SCORE							lChangeCowry;						//变更钻石

	//茶馆信息
	DWORD							dwGroupID;							//茶馆标识
	TCHAR							szGroupName[LEN_GROUP_NAME];		//茶馆名称
	TCHAR							szOperaterNickName[LEN_ACCOUNTS];	//用户昵称
};

//扣除钻石
struct DBR_GR_DeductGroupCowry
{
	DWORD							dwGroupID;							//茶馆标识
	DWORD							dwUserID;							//用户标识
	SCORE							lDeductCount;						//扣除数量
};

//喇叭道具
struct DBR_GR_WriteUserProperty 
{
	DWORD							dwUserID;							//用户标识
	int								nPropertyID;						//道具ID
	SCORE							lPropertyPrice;						//道具价格
	DWORD							dwPropCount;						//购买数目
	DWORD							dwClientIP;							//连接地址

	//请求信息
	DWORD							dwRequestID;						//请求标识
};

//赠送礼物
struct DBR_GR_SendGift
{
    //用户信息
    DWORD							dwUserID;							//用户标识
    DWORD							dwTargetUserID;						//用户标识

    //物品信息
    DWORD							dwGoodsID;							//商品标识
    DWORD							dwGoodsCount;						//物品数量

	//桌子信息
	SCORE							lMinTableScore;						//桌子最低分数
	SCORE							lCellScore;							//桌子底分
	BYTE							cbIsGoldRoom;						//是否金币房

    //连接信息
    DWORD							dwClientIP;							//连接地址	
};

//更新魅力
struct DBR_GR_UpdateLoveLiness
{
    DWORD							dwUserID;							//用户标识
};


//离开房间
struct DBR_GR_LeaveGameService
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwClientIP;							//连接地址
	DWORD							dwPlayTimeCount;					//游戏时间
	DWORD							dwOnLineTimeCount;					//在线时长

	//积分信息
	SCORE							lScore;								//用户分数
	SCORE							lRevenue;							//游戏税收
	SCORE							lUnLockScore;						//释放积分

	//游戏信息
	LONG							lWinCount;							//胜利盘数
	LONG							lLostCount;							//失败盘数
	LONG							lDrawCount;							//和局盘数
	LONG							lFleeCount;							//断线数目
	LONG							lExperience;						//用户经验
	LONG							lWinStreak;							//连胜局数
	LONG							lMaxWinStreak;						//连胜局数

	//请求信息
	DWORD							dwRequestID;						//请求标识
	BYTE							cbLeaveReason;						//离开原因
	DWORD							dwInoutIndex;						//记录索引
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//变更信息
	tagVariationScore				RecordInfo;							//记录信息
};

//游戏记录
struct DBR_GR_RecordDrawInfo
{
	//记录标识
	SCORE							lRecordID;							//记录标识

	//游戏信息
	WORD							wTableID;							//桌子号码	
	DWORD							dwPlayTimeCount;					//游戏时间	

	//积分信息
	BYTE							cbScoreKind;						//分数类型
	SCORE							lWasteCount;						//损耗数目
	DWORD							dwWasteUserID;						//损耗玩家
	SCORE							lRevenueCount;						//税收数目		

	//关联信息	
	SCORE							lRelationRecordID;					//记录标识
	TCHAR							szVideoNumber[LEN_VIDEO_NUMBER];	//录像编号

	//记录时间
	SYSTEMTIME						RecordTime;							//记录时间

	//记录信息
	WORD							wUserCount;							//用户数量
	TCHAR							szGameSourse[1024];					//游戏过程
};

//写分记录
struct DBR_GR_RecordDrawScore
{
	//记录标识
	SCORE							lRecordID;							//记录标识

	//关联信息	
	SCORE							lRelationRecordID;					//记录标识
	TCHAR							szVideoNumber[LEN_VIDEO_NUMBER];	//录像编号

	//记录子项
	WORD							wUserCount;							//用户数量	
	tagScoreRecordItem				ScoreRecordItem[MAX_CHAIR];			//子项数组
};

//游戏录像
struct DBR_GR_RecordGameVideo
{
	//约战信息
	DWORD							dwRelationNum;						//关联编号
	SCORE							lRelationRecordID;					//记录标识

	//用户信息
	WORD							wUserCount;							//用户数量
	DWORD							dwUserID[8];						//标识列表

	//录像数据
	SYSTEMTIME						VideoCreateTime;					//生成时间
	TCHAR							szVideoNumber[LEN_VIDEO_NUMBER];	//录像编号	
};

//权限管理
struct DBR_GR_ManageUserRight
{
	DWORD							dwUserID;							//目标用户
	DWORD							dwAddRight;							//添加权限
	DWORD							dwRemoveRight;						//删除权限
};

//查询物品
struct DBR_GR_UserQueryGoods
{
	DWORD							dwUserID;							//用户标识	
	TCHAR							szGoodsIDList[256];					//物品列表
};

//游戏财富
struct DBR_GR_WriteGameWealth
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	WORD							wReason;							//变更原因	
	WORD							wMoneyID;							//货币标识
	SCORE							lVariationScore;					//变更积分	
};

//更新物品
struct DBR_GR_UserUpdateGoods
{
	DWORD							dwUserID;							//用户标识	
	TCHAR							szGoodsList[2000];					//物品列表						
};

//记录物品
struct DBR_GR_UserRecordGoods
{
	DWORD							dwUserID;							//用户标识	
	TCHAR							szRecordList[2000];					//记录列表						
};

//控制开始
struct DBR_GR_ControlStart
{
	//用户信息
	DWORD							dwUserID;							//用户标识

	//输赢累计
	WORD							wAddupWinRate;						//累计胜率
	SCORE							lAddupWinScore;						//累计赢分
	SCORE							lAddupLoseScore;					//累计输分
	
	//控制信息
	tagUserControlItem				UserControlItem;					//控制子项
};

//控制结束
struct DBR_GR_ControlFinish
{
	//用户信息
	DWORD							dwUserID;							//用户标识

	//输赢累计
	WORD							wAddupWinRate;						//累计胜率
	SCORE							lAddupWinScore;						//累计赢分
	SCORE							lAddupLoseScore;					//累计输分

	//游戏信息
	LONG							lWinCount;							//赢局数目
	LONG							lLoseCount;							//输局数目
	SCORE							lWinScore;							//赢分数目
	SCORE							lLoseScore;							//输分数目
	SCORE							lBetAmount;							//打码数目
};

//控制更新
struct DBR_GR_ControlUpdate
{
	//用户信息
	DWORD							dwUserID;							//用户标识

	//游戏信息
	LONG							lWinCount;							//赢局数目
	LONG							lLoseCount;							//输局数目
	SCORE							lWinScore;							//赢分数目
	SCORE							lLoseScore;							//输分数目
	SCORE							lBetAmount;							//打码数额
};

//控制查询
struct DBR_GR_ControlQuery
{
	DWORD							dwUserID;							//用户标识
};


//////////////////////////////////////////////////////////////////////////
//输出信息

//配置信息
#define DBO_GR_GAME_LEVEL			(DB_FRAME_RESPOND_START+500)		//游戏等级
#define DBO_GR_USER_LEVEL			(DB_FRAME_RESPOND_START+501)		//用户等级
#define DBO_GR_GAME_GOODS			(DB_FRAME_RESPOND_START+502)		//游戏物品
#define DBO_GR_VERSION_INFO			(DB_FRAME_RESPOND_START+503)		//版本信息	
#define DBO_GR_FILTER_WORDS			(DB_FRAME_RESPOND_START+504)		//过滤词汇
#define DBO_GR_ROBOT_PARAMETER		(DB_FRAME_RESPOND_START+505)		//机器参数
#define DBO_GR_SUBSIDY_PARAMETER	(DB_FRAME_RESPOND_START+506)		//低保参数
#define DBO_GR_CONTROL_PARAMETER	(DB_FRAME_RESPOND_START+507)		//控制参数
#define DBO_GR_BATTLE_GAME_OPTION	(DB_FRAME_RESPOND_START+508)		//游戏选项

//内核事件
#define DBO_GR_ROBOT_USER			(DB_FRAME_RESPOND_START+520)		//机器信息
#define DBO_GR_CONFIG_RESULT		(DB_FRAME_RESPOND_START+521)		//配置结果

//逻辑事件
#define DBO_GR_LOGON_SUCCESS		(DB_FRAME_RESPOND_START+600)		//登录成功
#define DBO_GR_LOGON_FAILURE		(DB_FRAME_RESPOND_START+601)		//登录失败
#define DBO_GR_WEALTH_UPDATE		(DB_FRAME_RESPOND_START+602)		//财富更新
#define DBO_GR_WRITE_GAME_SCORE		(DB_FRAME_RESPOND_START+603)		//写分结果
#define DBO_GR_UPDATE_GOLD			(DB_FRAME_RESPOND_START+604)		//更新金币
#define DBO_GR_WRITE_USER_PROP		(DB_FRAME_RESPOND_START+605)		//写入道具
#define DBO_GR_LARGESS_SUBSIDY		(DB_FRAME_RESPOND_START+606)		//领取低保
#define DBO_GR_USER_GOODS_UPDATE	(DB_FRAME_RESPOND_START+607)		//物品更新

//赠送礼物
#define DBO_GR_SEND_GIFT_SUCCESS	(DB_FRAME_RESPOND_START+620)		//赠送成功
#define DBO_GR_SEND_GIFT_FAILURE	(DB_FRAME_RESPOND_START+621)		//赠送失败
#define DBO_GR_UPDATE_LOVELINESS	(DB_FRAME_RESPOND_START+622)		//更新魅力

//输赢控制
#define DBO_GR_USER_CONTROL_INFO	(DB_FRAME_RESPOND_START+630)		//控制信息
#define DBO_GR_USER_CONTROL_UPDATE  (DB_FRAME_RESPOND_START+631)		//控制更新

/////////////////////////////////////////////////////////////////////////////////////////

//配置结果
struct DBO_GR_ConfigResult
{
	bool							bResultCode;						//结果标识
	bool							bInitService;						//初始标识		
	DWORD							dwConfigMask;						//配置掩码
};

//机器信息
struct DBO_GR_GameRobotInfo
{
	LONG							lResultCode;						//结果代码
	DWORD							dwBatchID;							//批次标识
	WORD							wRobotCount;						//用户数目
	tagRobotAccountsInfo			RobotAccountsInfo[MAX_ROBOT];		//机器帐号
};

//机器信息
struct DBO_GR_RobotParameter
{
	LONG							lResultCode;						//结果代码
	WORD							wParameterCount;					//参数数目
	tagRobotParameter				RobotParameter[MAX_BATCH];			//机器信息
};

//系统消息
struct DBO_GR_MessageInfo
{
	DWORD							dwMessageID;						//消息ID
	BYTE							cbStatindIDCount;					//站点数量
	DWORD							dwOptionFlags;						//消息选项
	SYSTEMTIME						stStartTime;						//开始时间
	SYSTEMTIME						stEndTime;							//结束时间
	DWORD							dwTimeRate;							//发送频率
	TCHAR							szMessageString[1024];				//系统消息
};

//登录成功
struct DBO_GR_LogonSuccess
{
	//属性资料
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwCustomID;							//自定头像
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	
	//用户资料
	BYTE							cbAge;								//用户年龄
	BYTE							cbGender;							//用户性别	
	TCHAR							szArea[LEN_AREA];					//地区信息
	TCHAR							szCity[LEN_CITY];					//城市信息
	TCHAR							szProvince[LEN_PROVINCE];			//省份信息
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//更新签名
	TCHAR							szUserHeadUrl[256];		//kk jia
	

	//财富信息
	SCORE							lGold;								//用户金币
	SCORE							lScore;								//游戏积分
	SCORE							lUserCard;							//用户房卡
	SCORE							lFreeScore;							//自由积分	
	SCORE							lInsureGold;						//银行金币			
	BYTE							cbScoreKind;						//分数类型

	//游戏信息
	LONG							lWinCount;							//胜利盘数
	LONG							lLostCount;							//失败盘数
	LONG							lDrawCount;							//和局盘数
	LONG							lFleeCount;							//断线数目
	LONG							lExperience;						//用户经验
	LONG							lWinStreak;							//连胜局数
	LONG							lMaxWinStreak;						//连胜局数

	//等级信息
	BYTE							cbMasterOrder;						//管理等级
	DWORD							dwMemberPoint;						//会员点数

	//用户权限
	DWORD							dwUserRight;						//用户权限
	DWORD							dwMasterRight;						//管理权限

	//低保信息
	BYTE							cbResidualTimes;					//剩余次数
	SCORE							lConsumeScore;						//消耗金币(邮件、礼物、转账等)

	//手机定义
	BYTE							dwDeviceKind;						//设备类型
	WORD							wBehaviorFlags;						//行为标识
	WORD							wPageTableCount;					//分页桌数

	//位置信息	
	double							dGlobalPosLng;						//定位经度
	double							dGlobalPosLat;						//定位纬度
	TCHAR							szPlaceName[LEN_PLACE_NAME];		//地址信息

	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次
	WORD							wSignupCount;						//报名次数
	DWORD							dwTicketCount;						//赛券数量

	//状态信息
	BYTE							cbShutUpStatus;						//禁言状态
	DWORD							dwShutUpOverTime;					//解禁时间

	//黑白名单
	WORD							wAddupWinRate;						//累计胜率	
	SCORE							lAddupWinScore;						//累计赢分
	SCORE							lAddupLoseScore;					//累计输分	
	tagUserControlItem				UserControlItem;					//控制子项

	//桌椅信息
	bool							bLookon;							//旁观标志
	WORD							wTableID;							//桌子标识
	WORD							wChairID;							//椅子标识
	TCHAR							szTablePasswd[LEN_PASSWORD];		//桌子密码

	//物品信息
	WORD							wGoodsCount;						//物品数量
	tagGoodsHoldInfo				GoodsHoldInfo[32];					//持有信息

	//机器序列
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	DWORD							dwInoutIndex;						//记录索引
	TCHAR							szDescribeString[128];				//错误消息
};

//登录失败
struct DBO_GR_LogonFailure
{
	//锁定信息
	WORD							wLockKindID;						//类型标识
	WORD							wLockServerID;						//房间标识
	WORD							wLockServerKind;					//房间类型

	//错误信息
	LONG							lErrorCode;							//错误代码	
	TCHAR							szDescribeString[128];				//错误消息
};

//重置请求ID
struct DBO_GR_ResetRequestID
{
	LONG							lErrorCode;							//错误代码
};

//重新锁分
struct DBO_GR_RelockScore
{
	SCORE							lGold;								//用户金币
	SCORE							lInsureGold;						//银行金币
	SCORE							lFreeScore;							//自由积分 
};

//购买道具
struct DBO_GR_PurcaseProperty
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	SCORE							lCurrIngot;							//钻石余额

	//道具信息
	DWORD							dwPropKind;							//道具类型		
};

//游戏轮数 掼蛋专用
struct DBO_GR_GameRoundFinish
{
	WORD							wGameRound;							//完成局数
	WORD							wRoundCount;						//总共局数
};

//低保参数
struct DBO_GR_SubsidyParameter
{
	//低保参数
	SCORE							lTakeSubsidyGold;					//赠送金币
	SCORE							lTakeSubsidyCondition;				//领取福利			
	};

//控制参数
struct DBO_GR_ControlParameter
{

	//控制参数
	bool							bControlEnabled;					//控制开关
	LONG							lBenchmarkValue;					//基准分值
	WORD							wWLControlRate;						//控制胜率
	WORD							wBLControlRate;						//控制胜率
	WORD							wInWhiteListRate;					//进入阈值
	WORD							wInBlackListRate;					//进入阈值
	WORD							wOutWhiteListRate;					//退出阈值
	WORD							wOutBlackListRate;					//退出阈值
	WORD							wStartPlayCount;					//豁免局数
	WORD							wSupportControlMode;				//控制模式
};

//福利结果
struct DBO_GR_WriteWelfareResult
{
	bool							bResultCode;						//结果代码
	BYTE							cbResidualTimes;					//剩余次数
	BYTE							cbRechargeStatus;					//首充状态
	SCORE							lCurrGold;							//当前金币
	TCHAR							szDescribeString[128];				//描述消息
};

//低保结果
struct DBO_GR_LargessSubsidy
{
	bool							bResultCode;						//结果代码
	DWORD							dwUserID;							//用户标识
	BYTE							cbResidualTimes;					//剩余次数
	SCORE							lCurrGold;							//当前金币
	TCHAR							szDescribeString[128];				//描述消息
};

//财富更新
struct DBO_GR_UserWealthUpdate
{
	WORD							wMoneyID;							//货币标识
	DWORD							dwUserID;							//用户标识
	SCORE							lChangeAmount;						//变更金额
	SCORE							lBalanceAmount;						//结余金额
};

//物品信息
struct DBO_GR_UserGoodsUpdate
{
	WORD							wGoodsCount;						//物品数量	
	tagGoodsHoldInfo				GoodsHoldInfo[128];					//物品信息
};

//赠送成功
struct DBO_GR_SendGiftSuccess
{
	DWORD							dwUserID;							//赠送用户
    DWORD							dwTargetUserID;						//目标用户

    //物品信息
    DWORD							dwGoodsID;							//商品标识
    DWORD							dwGoodsCount;						//物品数量

    DWORD							dwLoveLiness;						//附赠魅力
    DWORD							dwMyLoveLiness;						//自身魅力
    DWORD							dwTargetLoveLiness;					//对方魅力

    WORD							wMoneyID;							//费用类型
	SCORE							lGiftPrice;							//费用价格
};

//赠送失败
struct DBO_GR_SendGiftFailure
{
    LONG							lErrorCode;							//错误代码
    TCHAR							szDescribeString[128];				//错误消息
};

//更新魅力
struct DBO_GR_UpdateLoveLiness
{
    DWORD							dwLoveLiness;						//魅力值
};

//控制信息
struct DBO_GR_UserControlInfo
{
	DWORD							dwUserID;							//用户标识
	tagUserControlItem				UserControlItem;					//控制子项
};

//控制更新
struct DBO_GR_UserControlUpdate
{
	DWORD							dwUserID;							//用户标识
	BYTE							cbControlStatus;					//控制状态
};

//////////////////////////////////////////////////////////////////////////

#endif