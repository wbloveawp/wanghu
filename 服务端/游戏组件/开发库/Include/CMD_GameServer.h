#ifndef CMD_GAME_SERVER_HEAD_FILE
#define CMD_GAME_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////

//服务配置
struct tagServerConfig
{
	WORD							wServerID;							//房间标识
	WORD							wServerType;						//房间类型	
	DWORD							dwServerRule;						//房间规则
};

//////////////////////////////////////////////////////////////////////////
//登录命令

#define MDM_GR_LOGON				10									//登录信息

//登录模式
#define SUB_GR_LOGON_ROBOT			1									//机器登录
#define SUB_GR_LOGON_MOBILE			2									//手机登录
#define SUB_GR_LOGON_COMPUTER		3									//电脑登录

//登出命令
#define SUB_GR_LOGON_LOGOUT			10									//用户登出

//登录结果
#define SUB_GR_LOGON_SUCCESS		100									//登录成功
#define SUB_GR_LOGON_FAILURE		101									//登录失败
#define SUB_GR_LOGON_FINISH			102									//登录完成

//升级提示
#define SUB_GR_UPDATE_NOTIFY		200									//升级提示

//////////////////////////////////////////////////////////////////////////

//比赛详情
struct tagLogonMatchDetails
{
	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次
};

//约战详情
struct tagLogonBattleDetails
{
	bool							bLookon;							//旁观标志	
	WORD							wTableID;							//桌子标识
	WORD							wChairID;							//椅子标识	
	TCHAR                           szTablePasswd[LEN_PASSWORD];        //桌子密码
};

//排位详情
struct tagLogonRankingDetails
{
	//桌子信息
	bool							bLookon;							//旁观标志	
	WORD							wTableID;							//桌子标识
	WORD							wChairID;							//椅子标识	
	TCHAR                           szTablePasswd[LEN_PASSWORD];        //桌子密码
};

//手机登录
struct CMD_GR_LogonMobile
{
	//版本信息
	WORD							wGameID;							//游戏标识
	DWORD							dwPlazaVersion;						//大厅版本
	DWORD							dwProcessVersion;					//进程版本

	//登录信息
	TCHAR                           szServerPasswd[LEN_PASSWORD];       //房间密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识

	//定位信息
	double							dGlobalPosLng;						//定位经度
	double							dGlobalPosLat;						//定位纬度
	TCHAR							szPlaceName[LEN_PLACE_NAME];		//地址信息

	//设备信息
	DWORD                           dwDeviceKind;                       //设备类型
};

//机器登录
struct CMD_GR_LogonRobot
{
	//版本信息
	DWORD							dwPlazaVersion;						//广场版本
	DWORD							dwFrameVersion;						//框架版本
	DWORD							dwProcessVersion;					//进程版本

	//登录信息
	DWORD							dwUserID;							//用户 I D		
	TCHAR							szPassword[LEN_PASSWORD];			//登陆密码
};

//电脑登录
struct CMD_GR_LogonComputer
{
	//版本信息
	DWORD							dwPlazaVersion;						//广场版本
	DWORD							dwFrameVersion;						//框架版本
	DWORD							dwProcessVersion;					//进程版本

	//登录信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码
};


//登录成功
struct CMD_GR_LogonSuccess
{
	DWORD							dwUserID;							//登陆成功
	DWORD							dwUserRight;						//用户权限
	DWORD							dwMasterRight;						//管理权限	
};

//登录失败
struct CMD_GR_LogonFailure
{
	//锁定信息
	WORD							wLockKindID;						//类型标识
	WORD							wLockServerID;						//房间标识
	WORD							wLockServerKind;					//房间类型

	//错误信息
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//错误消息
};

//升级提示
struct CMD_GR_UpdateNotify
{
	//升级标志
	BYTE							cbMustUpdatePlaza;					//强行升级
	BYTE							cbMustUpdateClient;					//强行升级
	BYTE							cbAdviceUpdateClient;				//建议升级

	//当前版本
	DWORD							dwCurrentPlazaVersion;				//当前版本
	DWORD							dwCurrentFrameVersion;				//当前版本
	DWORD							dwCurrentClientVersion;				//当前版本
};

//////////////////////////////////////////////////////////////////////////
//配置命令

#define MDM_GR_CONFIG				11									//配置信息

#define SUB_GR_CONFIG_COLUMN		100									//列表配置
#define SUB_GR_CONFIG_SERVER		101									//房间配置
#define SUB_GR_CONFIG_FINISH		102									//配置完成
#define SUB_GR_CONFIG_RULE			103									//房间规则
#define SUB_GR_CONFIG_USER_RIGHT	104									//玩家权限

//////////////////////////////////////////////////////////////////////////

//列表配置
struct CMD_GR_ConfigColumn
{
	BYTE							cbColumnCount;						//列表数目
	tagColumnItem					ColumnItem[32];						//列表描述
};

//房间配置
struct CMD_GR_ConfigServer
{
	WORD							wKindID;							//类型索引
	WORD							wTableCount;						//桌子数目
	WORD							wChairCount;						//椅子数目
	WORD							wServerType;						//房间类型
	DWORD							dwServerRule;						//房间规则	
};

//房间规则
struct CMD_GR_ConfigRule
{
	DWORD							dwServerRule;						//房间规则
};

//玩家权限
struct CMD_GR_ConfigUserRight
{
	DWORD							dwUserRight;						//玩家权限
};

//////////////////////////////////////////////////////////////////////////
//携带信息

#define DTP_GR_SERVER_TITLE			16									//房间标题

//////////////////////////////////////////////////////////////////////////
//用户命令

#define MDM_GR_USER					12									//用户信息

//////////////////////////////////////////////////////////////////////////
//用户命令
#define SUB_GR_USER_RULE			1									//用户规则
#define SUB_GR_USER_LOOKON			2									//旁观请求
#define SUB_GR_USER_SITDOWN			3									//坐下请求
#define SUB_GR_USER_STANDUP			4									//起立请求
#define SUB_GR_USER_APPLY			5									//报名请求
#define SUB_GR_USER_FEE_QUERY		6									//费用查询
#define SUB_GR_USER_REPULSE_SIT  	7									//拒绝玩家
#define SUB_GR_USER_REQUEST_INFO	8                                   //获取信息
#define SUB_GR_USER_SWITCH_TABLE	9                                   //请求换桌
#define SUB_GR_USER_DESKMATE_INFO	10                                  //同桌信息

//用户命令
#define SUB_GR_USER_ENTER			100									//用户进入
#define SUB_GR_USER_SCORE			101									//用户分数
#define SUB_GR_USER_STATUS			102									//用户状态
#define SUB_GR_REQUEST_FAILURE		104									//请求失败
#define SUB_GR_USER_WAIT_DISTRIBUTE 105									//等待分组
#define SUB_GR_USERLEVEL_UPGRADE	108									//用户等级
#define SUB_GR_GAMELEVEL_UPGRADE	109									//游戏等级
#define SUB_GR_RECHARGE_NOTICE		110									//充值通知

//礼物命令
#define SUB_GR_SEND_GIFT			120									//赠送礼物
#define SUB_GR_SEND_GIFT_SUCCESS	121									//赠送成功
#define SUB_GR_RECV_GIFT			122									//收到礼物

//聊天命令
#define SUB_GR_USER_CHAT			200									//聊天消息
#define SUB_GR_USER_WISPER			201									//私语消息
#define SUB_GR_WHSPER_REPLY			204									//自动回复

#define SUB_GR_INVITE_USER			300									//邀请用户

//请求失败代码
#define RFC_NULL					0									//无错误
#define RFC_PASSWORD_INCORRECT		1									//密码错误
#define RFC_TMP_PASSWD_INCORRECT	2									//临时密码错误
#define RFC_SCORE_UNENOUGH			3									//游戏币不足

//////////////////////////////////////////////////////////////////////////

//旁观请求
struct CMD_GR_UserLookon
{
	WORD							wTableID;							//桌子位置
	WORD							wChairID;							//椅子位置
	TCHAR							szPassword[LEN_PASSWORD];			//桌子密码
};

//坐下请求
struct CMD_GR_UserSitDown
{
	WORD							wTableID;							//桌子位置
	WORD							wChairID;							//椅子位置
	TCHAR							szPassword[LEN_PASSWORD];			//桌子密码
};

//起立请求
struct CMD_GR_UserStandUp
{
	WORD							wTableID;							//桌子位置
	WORD							wChairID;							//椅子位置
	BYTE							cbForceLeave;						//强行离开
};

//用户分数
struct CMD_GR_UserScore
{
	DWORD							dwUserID;							//用户标识
	tagClientUserScore				UserScore;							//积分信息
};

//用户状态
struct CMD_GR_UserStatus
{
	DWORD							dwUserID;							//用户标识
	tagUserStatus					UserStatus;							//用户状态
};

//等级升级
struct CMD_GR_GameLevelUpgrade
{
	DWORD							dwUserID;							//用户标识
	BYTE							cbGameOrder;						//当前等级	
	LONG							lExperience;						//经验值	
};

//等级升级
struct CMD_GR_UserLevelUpgrade
{
	DWORD							dwUserID;							//用户标识
	BYTE							cbMemberOrder;						//会员等级
	DWORD							dwMemberPoint;						//会员点数	
};

//充值通知
struct CMD_GR_RechargeNotice
{
	DWORD							dwUserID;							//用户标识
	SCORE							lPresentScore;						//赠送积分
};

//请求失败
struct CMD_GR_RequestFailure
{
	BYTE							cbFailureCode;						//错误代码
	TCHAR							szDescribeString[256];				//描述信息
};

//用户聊天
struct CMD_GR_S_UserChat
{
	WORD							wChatLength;						//信息长度
	DWORD							dwChatColor;						//信息颜色
	DWORD							dwTargetUserID;						//目标用户
	TCHAR							szChatString[LEN_USER_CHAT];		//聊天信息
};

//用户聊天
struct CMD_GR_R_UserChat
{
	WORD							wChatLength;						//信息长度
	DWORD							dwChatColor;						//信息颜色
	DWORD							dwSendUserID;						//发送用户
	DWORD							dwTargetUserID;						//目标用户
	TCHAR							szChatString[LEN_USER_CHAT];		//聊天信息
};

//用户私聊
struct CMD_GR_S_UserWisper
{
	WORD							wChatLength;						//信息长度
	DWORD							dwChatColor;						//信息颜色
	DWORD							dwTargetUserID;						//目标用户
	TCHAR							szChatString[LEN_USER_CHAT];		//聊天信息
};

//用户私聊
struct CMD_GR_R_UserWisper
{
	WORD							wChatLength;						//信息长度
	DWORD							dwChatColor;						//信息颜色
	DWORD							dwSendUserID;						//发送用户
	DWORD							dwTargetUserID;						//目标用户
	TCHAR							szChatString[LEN_USER_CHAT];		//聊天信息
};

//用户会话
struct CMD_GR_UserConversation
{
	WORD							wChatLength;						//信息长度
	DWORD							dwChatColor;						//信息颜色
	DWORD							dwSendUserID;						//发送用户
	DWORD							dwConversationID;					//会话标识
	DWORD							dwTargetUserID[MAX_CONVERSATION_USER];	//目标用户
	TCHAR							szChatString[LEN_USER_CHAT];		//聊天信息
};

//拒绝玩家
struct CMD_GR_UserRepulseSit
{
	WORD							wTableID;							//桌子号码
	WORD							wChairID;							//椅子位置
	DWORD							dwUserID;							//用户 I D
	DWORD							dwRepulseUserID;					//用户 I D
};

//请求用户信息
struct CMD_GR_UserRequestInfo
{
	DWORD                           dwUserID;							//请求用户
	WORD							wTablePos;							//桌子位置
};

//同桌信息
struct CMD_GR_UserDeskmateInfo
{
	WORD							wTableID;							//桌子号码
	WORD							wChairID;							//椅子位置
};


//背包信息
struct CMD_GR_UserPackInfo
{
	WORD							wGoodsCount;						//物品数量
	tagGoodsHoldInfo				GoodsHoldInfo[0];					//物品信息
};

//物品更新
struct CMD_GR_UserGoodsUpdate
{
	WORD							wGoodsCount;						//物品数量
	tagGoodsHoldInfo				GoodsHoldInfo[0];					//物品信息
};

//邀请用户
struct CMD_GR_R_InviteUser
{
	WORD							wTableID;							//桌子号码
	DWORD							dwSendUserID;						//发送用户
};

//邀请用户
struct CMD_GR_S_InviteUser
{
	DWORD							dwTargetUserID;						//目标用户
};

//喇叭消息
struct CMD_GR_UserBugle 
{
	DWORD							dwUserID;							//用户ID
	TCHAR							szAccounts[LEN_ACCOUNTS];			//用户帐户
	WORD							wChatLength;						//信息长度
	DWORD							dwChatColor;						//信息颜色
	BYTE							cbBugleType;						//喇叭类型
	TCHAR							szChatString[MAX_BUGLE_LEN];		//聊天信息
};

//赠送礼物
struct CMD_GR_SendGift
{
    DWORD							dwUserID;							//用户ID

    DWORD							dwGoodsID;							//商品标识
    DWORD							dwGoodsCount;						//物品数量
};

//赠送成功
struct CMD_GR_SendGiftSuccess
{
    DWORD							dwLoveLiness;						//魅力值
};

//收到礼物
struct CMD_GR_RecvGift
{
    //用户信息
    DWORD							dwUserID;							//用户标识
    DWORD							dwTargetUserID;						//目标用户

    //物品信息
    DWORD							dwGoodsID;							//商品标识
    DWORD							dwGoodsCount;						//物品数量

	DWORD							dwLoveLiness;						//附赠魅力
};

//////////////////////////////////////////////////////////////////////////

//规则标志
#define UR_LIMIT_SAME_IP			0x01								//限制地址
#define UR_LIMIT_WIN_RATE			0x02								//限制胜率
#define UR_LIMIT_FLEE_RATE			0x04								//限制逃率
#define UR_LIMIT_GAME_SCORE			0x08								//限制积分

//用户规则
struct CMD_GR_UserRule
{
	BYTE							cbRuleMask;							//规则掩码
	WORD							wMaxFleeRate;						//最高逃率
	WORD							wLessWinRate;						//最低胜率
	LONG							lMaxGameScore;						//最高分数 
	LONG							lLessGameScore;						//最低分数
};

//////////////////////////////////////////////////////////////////////////
//状态命令

#define MDM_GR_STATUS				13									//状态信息

#define SUB_GR_TABLE_INFO			100									//桌子信息
#define SUB_GR_TABLE_STATUS			101									//桌子状态
#define SUB_GR_TABLE_SCORE			102									//桌子底分
#define SUB_GR_SCORE_VARIATION		103									//底分变更

//////////////////////////////////////////////////////////////////////////

//桌子信息
struct CMD_GR_TableInfo
{
	WORD							wTableCount;						//桌子数目
	tagTableStatus					TableStatusArray[512];				//桌子状态
};

//桌子状态
struct CMD_GR_TableStatus
{
	WORD							wTableID;							//桌子号码
	tagTableStatus					TableStatus;						//桌子状态
};

//桌子分数
struct CMD_GR_TableScore
{
	WORD							wTableCount;						//桌子数目
	LONGLONG						lTableScoreArray[512];				//桌子底分
};

//底分变更
struct CMD_GR_ScoreVariation
{
	WORD							wTableID;							//桌子号码
	LONGLONG						lBaseScore;							//桌子底分
};

//////////////////////////////////////////////////////////////////////////
//管理命令

#define MDM_GR_MANAGE				14									//管理命令

#define SUB_GR_SEND_WARNING			1									//发送警告
#define SUB_GR_SEND_MESSAGE			2									//发送消息
#define SUB_GR_LOOK_USER_IP			3									//查看地址
#define SUB_GR_KILL_USER			4									//踢出用户
#define SUB_GR_LIMIT_ACCOUNS		5									//禁用帐户
#define SUB_GR_SET_USER_RIGHT		6									//权限设置
#define SUB_GR_OPTION_SERVER		7									//房间设置
#define SUB_GR_KICK_ALL_USER		8									//踢出用户
#define SUB_GR_LIMIT_USER_CHAT		9									//限制聊天
#define SUB_GR_TABLE_RULE			10									//桌子规则
#define SUB_GR_SERVER_OPTION		11									//房间选项
#define SUB_GR_DISMISSGAME			12									//解散桌子
#define SUB_GR_CARDDISPATCHERDEBUG	14									//配牌器信息

//////////////////////////////////////////////////////////////////////////
//最大库存
#define MAX_REPERTORY_CNT			144									//最大库存

//设置标志
#define OSW_ROOM					1									//大厅消息
#define OSW_GAME					2									//游戏消息

//发送警告
struct CMD_GR_SendWarning
{
	WORD							wChatLength;						//信息长度
	DWORD							dwTargetUserID;						//目标用户
	TCHAR							szWarningMessage[LEN_USER_CHAT];	//警告消息
};

//系统消息
struct CMD_GR_SendMessage
{
	WORD							wType;								//消息类型
	DWORD							dwOptionFlags;						//消息范围
	WORD							wLength;							//消息长度
	TCHAR							szString[1024];						//消息内容
};

//查看地址
struct CMD_GR_LookUserIP
{
	DWORD							dwTargetUserID;						//目标用户
};

//踢出用户
struct CMD_GR_KillUser
{
	DWORD							dwTargetUserID;						//目标用户
};

//解散游戏
struct CMD_GR_DismissGame
{
	WORD							wDismissTableNum;		            //解散桌号
};

//禁用帐户
struct CMD_GR_LimitAccounts
{
	DWORD							dwTargetUserID;						//目标用户
};

//权限设置
struct CMD_GR_SetUserRight
{
	//目标用户
	DWORD							dwTargetUserID;						//目标用户

	//绑定变量
	BYTE							cbGameRight;						//帐号权限
	BYTE							cbAccountsRight;					//帐号权限

	//权限变化
	BYTE							cbLimitRoomChat;					//大厅聊天
	BYTE							cbLimitGameChat;					//游戏聊天
	BYTE							cbLimitPlayGame;					//游戏权限
	BYTE							cbLimitSendWisper;					//发送消息
	BYTE							cbLimitLookonGame;					//旁观权限
};

//房间设置
struct CMD_GR_OptionServer
{
	BYTE							cbOptionFlags;						//设置标志
	BYTE							cbOptionValue;						//设置标志
};

//踢出所有用户
struct CMD_GR_KickAllUser
{
	TCHAR							szKickMessage[LEN_USER_CHAT];		//踢出提示
};

//限制聊天
struct CMD_GR_LimitUserChat
{
	DWORD							dwTargetUserID;						//目标用户
	BYTE							cbLimitFlags;						//限制标志
	BYTE							cbLimitValue;						//限制与否
};

//房间选项
struct CMD_GR_ManageServerOption
{
	DWORD							dwRevenueType;						//税收类型
	LONG							lRevenue;							//税收比例
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	TCHAR							szServerName[LEN_SERVER];			//房间名称
};

//配牌器
struct CMD_GR_CardDispatcherDebug
{
	WORD							wTableID;							//目标桌子
};

// 发送扑克
struct CMD_GR_SendCard
{
	//控制数据
	WORD                            wBankerUser;                       // 控制庄家
	BYTE							cbControlGameCount;					//控制局数
	BYTE							cbMagicIndex[4];					//财神索引
	BYTE							cbMagicOpenPos;						//财神位置
	BYTE							cbMagicOpenCard;					//财神牌
	BYTE							cbCardCount;						//发牌数目
	BYTE							cbCardData[MAX_CARD_REPERTORY];		//发牌数据
};

//////////////////////////////////////////////////////////////////////////
//会员命令

#define MDM_GR_MEMBER				15									//会员命令

#define SUB_GR_KICK_USER			1									//踢走用户

//////////////////////////////////////////////////////////////////////////

//踢走用户
struct CMD_GR_KickUser
{
	DWORD							dwTargetUserID;						//目标用户
};

//////////////////////////////////////////////////////////////////////////

//设置标志
#define OSF_ROOM_CHAT				1									//大厅聊天
#define OSF_GAME_CHAT				2									//游戏聊天
#define OSF_ROOM_WISPER				3									//大厅私聊
#define OSF_ENTER_ROOM				4									//进入房间
#define OSF_ENTER_GAME				5									//进入游戏
#define OSF_PLAY_CHAT				6									//同桌游戏私聊
#define OSF_REMOTE_SEARCH			7									//远程搜索
#define OSF_ROBOT_ATTEND			8									//机器人陪打
#define OSF_ROBOT_SIMULATE			9									//机器人占座
#define OSF_CLOSE_NOPLAYER			10									//自动关闭房间
#define OSF_MEMBER_CAN_CHAT			11									//会员可聊天
#define OSF_LOOK_ON					12									//禁止旁观
#define OSF_SEND_BUGLE				13									//发送喇叭
#define OSF_LOOK_ON_CHAT			14									//旁观聊天

//////////////////////////////////////////////////////////////////////////
//查询命令

#define MDM_GR_QUERY				16									//查询命令

#define SUB_GR_QUERY_BY_GAMEID		1									//查询用户
#define SUB_GR_QUERY_BY_ACCOUNTS	2									//查询用户

#define SUB_GR_QUERY_USER_RESULT	200									//查询结果
#define SUB_GR_QUERY_NOT_FOUND		201									//查询不到

//////////////////////////////////////////////////////////////////////////

//查询用户
struct CMD_GR_QueryByGameID
{
	DWORD							dwGameID;							//游戏ID
};

//查询用户
struct CMD_GR_QueryByAccounts
{
	TCHAR							szAccounts[LEN_ACCOUNTS];			//用户帐户
};

//查找不到
struct CMD_GR_QueryNotFound
{
	INT								nResultCode;						//返回代码
	TCHAR							szDescribeString[128];				//返回消息
};

//返回代码
#define QUERY_NOT_FOUND				1									//未找到
#define QUERY_FORBID_USE			2									//禁止使用查找


//////////////////////////////////////////////////////////////////////////
//比赛命令

#define MDM_GR_MATCH				17									//比赛命令

//请求命令
#define SUB_GR_MATCH_SIGNUP			1									//比赛报名
#define SUB_GR_MATCH_UNSIGNUP		2									//取消报名
#define SUB_GR_MATCH_REVIVE			3									//复活比赛
#define SUB_GR_DISMISS_MATCH		4									//解散比赛

//应答命令
#define SUB_GR_MATCH_NUMBER			10									//等待人数
#define SUB_GR_MATCH_STATUS			11									//比赛状态
#define SUB_GR_MATCH_USTATUS		12									//比赛状态
#define SUB_GR_SIGNUP_SUCCESS		13									//报名成功
#define SUB_GR_TICKET_UPDATE		14									//赛券更新

//比赛信息
struct CMD_GR_MatchRequest
{
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次
};

//比赛退出
struct CMD_GR_MatchSignup : public CMD_GR_MatchRequest
{
};

//比赛报名
struct CMD_GR_MatchUnSignup : public CMD_GR_MatchRequest
{	
};

//比赛复活
struct CMD_GR_MatchRevive : public CMD_GR_MatchRequest
{	
};

//分享比赛
struct CMD_GR_ShareMatch : public CMD_GR_MatchRequest
{	
};

//解散比赛
struct CMD_GR_DismissMatch : public CMD_GR_MatchRequest
{	
};

//
//定时赛数据
struct tagLockTimeMatchData
{
	//复活配置	
	WORD							wFeeMoneyID;						//费用类型
	LONG							lFeeAmount;							//费用金额
	BYTE							cbReviveTimes;						//复活次数	
};

//即时赛数据
struct tagImmediateMatchData
{		
};

//比赛人数
struct CMD_GR_MatchNumber
{	
	WORD							wTotalCount;						//开赛人数
	WORD							wSignupCount;						//报名人数
};

//赛券更新
struct SUB_GR_TicketUpdate
{
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次
	DWORD							dwTicketCount;						//比赛券数量
};

//////////////////////////////////////////////////////////////////////////
// 
//调试命令
#define MDM_GR_DEBUG				18									//调试命令

//////////////////////////////////////////////////////////////////////////
//调试命令

#define SUB_GR_DEBUG_PACKET			1									//调试数据包

//////////////////////////////////////////////////////////////////////////
//调试类型
#define DEBUG_KIND_GAME_CONTROL		1									//游戏控制
#define DEBUG_KIND_ROBOT_CONFIG		2									//机器配置

//////////////////////////////////////////////////////////////////////////
// 
//调试数据包
struct CMD_GR_DebugPacket
{
	WORD							wTableID;							//桌子标识
	WORD							wDebugKind;							//调试类型
	WORD							wGameCmdID;							//游戏命令
};


//////////////////////////////////////////////////////////////////////////
// 
//约战命令
#define MDM_GR_BATTLE				19									//约战命令

//////////////////////////////////////////////////////////////////////////
//请求命令
#define SUB_GR_DISMISS_TABLE		1									//解散桌子


//响应命令
#define SUB_GR_DISMISS_RESULT		100									//解散结果

//////////////////////////////////////////////////////////////////////////
//消息定义

//解散桌子
struct CMD_GR_DismissTable
{
	WORD							wTableID;							//桌子编号	
	DWORD							dwBattleNum;						//房间编号
};

//解散结果
struct CMD_GR_DismissResult
{
	BYTE							cbResultCode;						//结果代码
	TCHAR							szDescribeString[256];				//描述信息
};

//////////////////////////////////////////////////////////////////////////
//框架命令

#define MDM_GF_FRAME				100									//框架命令

//////////////////////////////////////////////////////////////////////////
//框架命令

//用户命令
#define SUB_GF_GAME_OPTION			1									//游戏配置
#define SUB_GF_USER_READY			2									//用户准备
#define SUB_GF_USER_CHAT			3									//用户聊天
#define SUB_GF_LOOKON_CONFIG		4									//旁观配置
#define SUB_GF_USER_LOOK2SIT		5									//旁观坐下
#define SUB_GF_USER_SIT2LOOK		6									//坐下旁观
#define SUB_GF_USER_BUGLE			8									//喇叭消息
#define SUB_GF_USER_VOICE			9									//用户语音
#define SUB_GF_LAUNCH_DISMISS		10									//发起解散
#define SUB_GF_BALLOT_DISMISS		11									//投票解散

//开始模式
#define SUB_GF_START_GAME			20									//开始游戏
#define SUB_GF_START_STATUS			21									//开始状态

//比赛信息
#define SUB_GF_MATCH_DATA			50									//比赛数据
#define SUB_GF_MATCH_RANK			51									//比赛排名
#define SUB_GF_MATCH_SCORE			52									//比赛分数	
#define SUB_GF_MATCH_WAIT			53									//比赛等待
#define SUB_GF_MATCH_PROMOTE		54									//比赛晋级
#define SUB_GF_MATCH_ELIMINATE		55									//比赛淘汰
#define SUB_GF_MATCH_ROUND_SWITCH	56									//比赛轮次
#define SUB_GF_MATCH_RESULT			50									//比赛结果

//游戏信息
#define SUB_GF_GAME_STATUS			100									//游戏状态
#define SUB_GF_GAME_SCENE			101									//游戏场景
#define SUB_GF_LOOKON_STATUS		102									//旁观状态

//约战信息
#define SUB_GF_TABLE_PARAM			150									//桌子参数	
#define SUB_GF_TABLE_BATTLE			151									//桌子战绩
#define SUB_GF_TABLE_GRADE			152									//桌子成绩
#define SUB_GF_VIDEO_PARAM			154									//视频参数	
#define SUB_GF_TABLE_CONFIG			155									//桌子配置
#define SUB_GF_MODIFY_CONFIG		156									//修改配置

//协议解散
#define SUB_GF_DISMISS_NOTIFY		160									//解散提醒
#define SUB_GF_DISMISS_BALLOT		161									//解散投票
#define SUB_GF_DISMISS_SUCCESS		162									//解散成功	

//排位信息
#define SUB_GF_RANKORDER_UPDATE		170									//等级更新

//系统消息
#define SUB_GF_SYSTEM_MESSAGE		200									//系统消息
#define SUB_GF_ACTION_MESSAGE		201									//动作消息

//获取用户
#define SUB_GF_GET_SERVER_USER		210									//获取房间用户
#define	SUB_GF_SERVER_USER_LIST		211									//用户列表

//游戏框架
#define SUB_GF_GAME_FRAME_MIN		500									//框架命令
#define SUB_GF_GAME_FRAME_MAX		600									//框架命令

//////////////////////////////////////////////////////////////////////////

//游戏配置
struct CMD_GF_GameOption
{
	BYTE							cbAllowLookon;						//旁观标志
	DWORD							dwFrameVersion;						//框架版本
	DWORD							dwClientVersion;					//游戏版本
};

//游戏状态
struct CMD_GF_GameStatus
{
	bool							bGamePaused;						//暂停标志
	BYTE							cbGameStatus;						//游戏状态
	BYTE							cbAllowLookon;						//旁观标志
};

//用户聊天
struct CMD_GF_UserChat
{
	WORD							wChatLength;						//信息长度
	COLORREF						crStringColor;						//信息颜色
	DWORD							dwSendUserID;						//发送用户
	DWORD							dwTargetUserID;						//目标用户
	TCHAR							szChatString[LEN_USER_CHAT];		//聊天信息
};

//发送语音
struct CMD_GF_SendVoice
{
	DWORD							dwTargetUserID;						//目标用户
};

//用户语音
struct CMD_GF_UserVoice
{
	DWORD							dwSendUserID;						//发送用户
};

//旁观配置
struct CMD_GF_LookonConfig
{
	DWORD							dwUserID;							//用户标识
	BYTE							bAllowLookon;						//允许旁观
};

//旁观状态
struct CMD_GF_LookonStatus
{
	BYTE							bAllowLookon;						//允许旁观
};

//旁观坐下
struct CMD_GF_Look2Sit
{
	WORD							wChairID;							//椅子位置
};

//坐下旁观
struct CMD_GF_Sit2Look
{
	WORD							wChairID;							//椅子位置
};

//开始状态
struct CMD_GF_StartStatus
{
	bool							bEnabled;							//使用标识
	bool							bVisibled;							//可视标识
};

//比赛分数
struct CMD_GF_MatchScore
{
	WORD							wPlayCount;							//游戏局数
	LONG							lBaseScore;							//游戏底分
	LONG							lEliminateScore;					//淘汰分数
};

//比赛数据
struct CMD_GF_MatchData
{
	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次
	BYTE							cbMatchType;						//比赛类型	

	//比赛轮次
	WORD							wRoundID;							//轮次标识
	WORD							wRoundCount;						//轮次数目
	tagMatchRoundItem				MatchRoundItem[5];					//轮次信息

	//比赛数据
	BYTE							cbMatchData[256];					//比赛数据
};

//比赛等待
struct CMD_GF_MatchWait
{
	WORD							wWaitMask;						//等待掩码		
	WORD							wTableRankID;						//本桌名次
	WORD							wTableUserCount;					//桌子人数
	WORD							wWaitTableCount;					//等待桌数
	TCHAR							szWaitMessage[128];				//等待消息
};

//比赛晋级
struct CMD_GF_MatchPromote
{
	WORD							wRankID;							//当前名词
	WORD							wPromoteCount;						//晋级人数
	LONGLONG						lMatchScore;						//比赛分数
	TCHAR							szNotifyContent[128];				//提示内容
};

//比赛淘汰
struct CMD_GF_MatchEliminate
{
	//名次信息
	WORD							wRankID;							//当前名次	
	WORD							wMatchUserCount;					//用户人数

	//复活信息
	bool							bEnableRevive;						//复活标识
	BYTE							cbReviveTimes;						//复活次数

	//提示内容
	TCHAR							szNotifyContent[128];				//提示内容
};

//轮次切换
struct CMD_GF_MatchRoundSwitch
{
	WORD							wCurrRoundID;						//轮次标识
};

//比赛结果
struct CMD_GF_MatchResult
{	
	//用户信息
	WORD							wRankID;							//比赛名次	
	TCHAR							szNickName[LEN_ACCOUNTS];			//玩家昵称

	//奖励信息
	TCHAR							szRewardEntity[128];				//实物奖励	
	TCHAR							szRewardGoods[256];					//物品奖励

	//比赛信息
	TCHAR							szMatchName[64];					//比赛名称	
	SYSTEMTIME						MatchFinishTime;					//结束时间	
};

//修改配置
struct CMD_GF_ModifyConfig
{
	DWORD							dwBattleNum;						//约战编号
	WORD							wOptionMask;						//选项掩码
	BYTE							cbOptionValue;						//选项数值
};

//桌子参数
struct CMD_GF_TableParam
{
	//桌子信息	
	LONG							lBaseScore;							//游戏底分
	DWORD							dwOwnerID;							//桌主标识
	DWORD							dwBattleNum;						//映射编号	

	//结算信息
	WORD							wPlayCount;							//游戏局数
	WORD							wFinishCount;						//完成局数	

	//记录信息
	LONGLONG						lBattleRecordID;					//记录标识
};

//视频参数
struct CMD_GF_VideoParam
{
	TCHAR							szVideoKey[LEN_VIDEO_KEY];			//视频秘钥
	TCHAR							szVideoChannel[LEN_VIDEO_CHANNEL];	//视频频道
};

//约战配置
struct CMD_GF_TableConfig
{
	DWORD							dwBattleNum;						//约战编号
	SCORE							lWinnerScore;						//赢家奖励		
	LONGLONG						lConsumeCount;						//消耗数量
	tagBattleConfig					BattleConfig;						//约战配置
};

//投票解散
struct CMD_GF_BallotDismiss
{
	bool							bAgreeDismiss;						//同意解散
};

//解散提醒
struct CMD_GF_DismissNotify
{
	DWORD							dwUserID;							//发起用户
	WORD							wResidueTime;						//剩余时间
	WORD							wNeedAgreeCount;					//同意人数	
	WORD							wUserCount;							//用户数量
	DWORD							dwUserIDList[TABLE_USER_COUNT];		//用户列表
};

//解散投票
struct CMD_GF_DismissBallot
{
	DWORD							dwUserID;							//用户标识
	bool							bAgreeDismiss;						//同意解散
};

//等级更新
struct CMD_GF_RankOrderUpdate
{
	SCORE							lRankScore;							//排位分数
	WORD							wRankOrder;							//排位等级	
};


//////////////////////////////////////////////////////////////////////////
//游戏命令

#define MDM_GF_GAME					200									//游戏命令

//////////////////////////////////////////////////////////////////////////
//携带信息

//其他信息
#define DTP_GR_MACHINE_ID			1									//机器标识
#define DTP_GR_TABLE_PASSWORD		2									//桌子密码

//用户属性
#define DTP_GR_USER_NICKNAME		10									//用户昵称
#define DTP_GR_USER_GROUP_NAME		11									//社团名字
#define DTP_GR_USER_UNDER_WRITE		12									//个性签名

//地区信息
#define DTP_GR_AREA					20									//地区信息
#define DTP_GR_CITY					21									//城市信息
#define DTP_GR_PROVINCE				22									//省份信息
#define DTP_GR_CUSTOM_FACE			23									//自定头像

//附加信息
#define DTP_GR_USER_NOTE			30									//用户备注

//玩家位置
#define DTP_GR_SERVER_INFO			40									//玩家位置

//提示信息
#define DTP_GR_DESCRIBE				50									//描述信息
#define DTP_GR_GOODSLIST			51									//物品列表

//////////////////////////////////////////////////////////////////////////

//请求错误
#define REQUEST_FAILURE_NORMAL		0									//常规原因
#define REQUEST_FAILURE_NOGOLD		1									//金币不足
#define REQUEST_FAILURE_NOSCORE		2									//积分不足
#define REQUEST_FAILURE_PASSWORD	3									//密码错误

//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif