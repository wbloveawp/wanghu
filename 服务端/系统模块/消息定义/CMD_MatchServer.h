#ifndef CMD_MATCH_SERVER_HEAD_FILE
#define CMD_MATCH_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//命令定义

//比赛命令
#define MDM_AM_MATCH_SERVICE		10									//比赛服务

//////////////////////////////////////////////////////////////////////////

//比赛请求
#define SUB_AM_C_ENTER_MATCH		   1							 	//进入比赛
#define SUB_AM_C_LEAVE_MATCH		   2								//离开比赛
#define SUB_AM_C_QUERY_MATCH_SESSION   3								//查询场次
#define SUB_AM_C_QUERY_MATCH_RANKING   4								//查询排行
#define SUB_AM_C_QUERY_MATCH_TABLE	   5								//请求桌子
#define SUB_AM_C_QUERY_SIGNUP_USER	   6								//请求用户

//赛券命令
#define SUB_AM_C_BUY_MATCH_TICKET	   10								//购买赛券
#define SUB_AM_C_QUERY_MATCH_TICKET	   11								//查询赛券

//比赛记录
#define SUB_AM_C_QUERY_MATCH_RECORD	   20								//查询记录
#define SUB_AM_C_QUERY_MATCH_RECORD_BOARD 21							//记录看板

//比赛配置
#define SUB_AM_S_MATCH_OPTION_LIST	   100								//配置列表
#define SUB_AM_S_MATCH_OPTION_STATUS   101								//配置状态

//比赛场次
#define SUB_AM_S_MATCH_SESSION_ITEM	   102								//比赛场次
#define SUB_AM_S_MATCH_SESSION_LIST	   103								//场次列表
#define SUB_AM_S_MATCH_SESSION_DETAIL  104								//场次详情
#define SUB_AM_S_MATCH_SESSION_SIGNUP  105								//场次报名
#define SUB_AM_S_MATCH_SESSION_STATUS  106								//场次状态
#define SUB_AM_S_MATCH_SESSION_RANKING 107								//场次排行
#define SUB_AM_S_MATCH_ENTER_FINISH	   109								//进入完成

//赛券结果
#define SUB_AM_S_BUY_TCIKET_RESULT	   110								//买券结果
#define SUB_AM_S_QUERY_TCIKET_RESULT   111								//赛券信息

//比赛记录
#define SUB_AM_S_MATCH_RECORD_LIST	   120								//记录列表
#define SUB_AM_S_MATCH_RECORD_BOARD	   121								//记录看板

//比赛桌子
#define SUB_AM_S_MATCH_TABLE_LIST	   130								//桌子列表

//报名用户
#define SUB_AM_S_SIGNUP_USER_LIST	   140								//用户列表	

//////////////////////////////////////////////////////////////////////////
//结构定义

//查询比赛
struct CMD_AM_C_QueryMatchSession
{
	DWORD							dwMatchID;							//比赛标识	
};

//查询排行
struct CMD_AM_C_QueryMatchRanking
{
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次
};

//购买结果
struct CMD_AM_C_BuyMatchTicket
{
	DWORD							dwMatchID;							//比赛标识	
	SCORE							lMatchNo;							//比赛场次	
};

//查询赛券
struct CMD_AM_C_QueryMatchTicket
{
	DWORD							dwMatchID;							//比赛标识	
	SCORE							lMatchNo;							//比赛场次
};

//查询记录看板
struct CMD_AM_C_QueryMatchRecordBoard 
{
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次
};

//查找桌子列表
struct CMD_AM_C_QueryMatchTable
{
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次
};

//查找报名用户
struct CMD_AM_C_QuerySignupUser
{
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次
};

//配置状态
struct CMD_AM_S_MatchOptionStatus
{
	DWORD							dwMatchID;							//比赛标识
	BYTE							cbOptionStatus;						//配置状态
};

//场次子项
struct CMD_AM_S_MatchSessionItem
{
	tagMatchSessionItem				MatchSessionItem;					//比赛场次
};

//场次详情
struct CMD_AM_S_MatchSessionDetail
{
	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次	
	BYTE							cbMatchType;						//比赛类型
	WORD							wStartCount;						//开始人数
	WORD							wSignupCount;						//报名人数
};

//报名信息
struct CMD_AM_S_MatchOptionSignup
{
	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次
	WORD							wStartCount;						//开始人数
	WORD							wSignupCount;						//报名人数		
};

//比赛状态
struct CMD_AM_S_MatchSessionStatus
{
	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次
	BYTE							cbMatchStatus;						//比赛状态
};

//比赛排行
struct CMD_AM_S_MatchSessionRanking
{
	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次
	SCORE							lMatchScore;						//比赛积分
};

//购买结果
struct CMD_AM_S_BuyTicketResult
{
	DWORD							dwMatchID;							//比赛标识	
	LONGLONG						lMatchNo;							//比赛场次
	DWORD							dwTicketCount;						//赛券数量
};

//查询赛券
struct CMD_AM_S_QueryTicketResult
{
	DWORD							dwMatchID;							//比赛标识	
	LONGLONG						lMatchNo;							//比赛场次
	DWORD							dwTicketCount;						//赛券数量
};


//记录看板
struct CMD_AM_S_MatchRecordBoard
{
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次
	tagMatchRecordUserItem			MatchRecordUserItem[0];				//用户子项
};

//桌子列表
struct CMD_AM_S_MatchTableList
{
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次
	WORD							wTableCount;						//桌子数量
	tagMatchTableInfo				MatchTableInfo[0];					//桌子信息
};

//用户列表
struct CMD_AM_S_SignupUserList
{
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次
	WORD							wSignupCount;						//报名数量
	WORD							wUserCount;							//用户数量
	tagSignupUserItem				SignupUserItem[0];					//用户列表
};

//////////////////////////////////////////////////////////////////////////

//比赛命令
#define MDM_MS_MATCH_SERVICE		11									//比赛服务

//////////////////////////////////////////////////////////////////////////

//比赛命令
#define SUB_MS_C_CREATE_MATCH		300									//创建比赛
#define SUB_MS_C_REMOVE_MATCH		301									//移除比赛
#define SUB_MS_C_DISMISS_MATCH		302									//解散比赛
#define SUB_MS_C_COLLECT_MATCH		303									//收集比赛

//比赛同步
#define SUB_MS_S_COLLECT_MATCH		410									//比赛收集
#define SUB_MS_S_COLLECT_FINISH		411									//收集完成

//比赛命令
#define SUB_MS_S_MATCH_GROUP		420									//比赛分组
#define SUB_MS_S_MATCH_START		421									//比赛开始
#define SUB_MS_S_MATCH_CANCEL		422									//比赛取消
#define SUB_MS_S_MATCH_FINISH		423									//比赛结束
#define SUB_MS_S_MATCH_COMPLETE		424									//比赛完成
#define SUB_MS_S_MATCH_SIGNUP		425									//比赛报名
#define SUB_MS_S_MATCH_UNSIGNUP		426									//取消报名
#define SUB_MS_S_MATCH_GIVEUP		427									//比赛放弃
#define SUB_MS_S_MATCH_REVIVE		428									//比赛复活
#define SUB_MS_S_MATCH_ELIMINATE	429									//比赛淘汰
#define SUB_MS_S_CREATE_FAILED		430									//创建失败

//比赛桌子
#define SUB_MS_S_MATCH_TABLE_APPEND 450									//添加桌子
#define SUB_MS_S_MATCH_TABLE_UPDATE	451									//更新桌子
#define SUB_MS_S_MATCH_TABLE_REMOVE	452									//移除桌子		
#define SUB_MS_S_MATCH_TABLE_RECYCLE 453								//回收桌子

//////////////////////////////////////////////////////////////////////////
//结构定义

//创建比赛
struct CMD_MS_C_CreateMatch
{
	DWORD							dwTimeStamp;						//时间戳
	WORD							wTableCount;						//桌子数量	
	tagMatchOption					MatchOption;						//比赛配置
};

//移除比赛
struct CMD_MS_C_RemoveMatch
{
	DWORD							dwMatchID;							//比赛标识
};

//解散比赛
struct CMD_MS_C_DismissMatch
{
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次
};

//收集比赛
struct CMD_MS_S_CollectMatch
{
	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次	
	WORD							wTableCount;						//桌子数量
	bool							bActiveGroup;						//当前分组
	BYTE							cbMatchStatus;						//比赛状态	
	SYSTEMTIME						MatchStartTime;						//开赛时间
	SYSTEMTIME						MatchSignupStart;					//报名开始
	SYSTEMTIME						MatchSignupCutOff;					//报名截止
	SYSTEMTIME						MatchSaleTicketTime;				//售票时间

	//玩家信息
	WORD							wStartCount;						//开赛人数
	WORD							wPlayerCount;						//游戏人数
	WORD							wMaxPlayerCount;					//最大人数

};

//比赛场次
struct CMD_MS_S_MatchGroup
{
	WORD							wServerID;							//房间标识
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次
	WORD							wTableCount;						//桌子数量
	WORD							wStartCount;						//开赛人数
	BYTE							cbMatchStatus;						//比赛状态
	WORD							wMaxPlayerCount;					//最大人数
	SYSTEMTIME						MatchStartTime;						//开赛时间
	SYSTEMTIME						MatchSignupStart;					//报名开始
	SYSTEMTIME						MatchSignupCutOff;					//报名截止
	SYSTEMTIME						MatchSaleTicketTime;				//售票时间
};

//比赛开始
struct CMD_MS_S_MatchStart
{
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次	
};

//比赛取消
struct CMD_MS_S_MatchCancel
{
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次	
};

//比赛开始
struct CMD_MS_S_MatchFinish
{
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次	
};

//比赛完成
struct CMD_MS_S_MatchComplete
{
	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次	

	//用户信息
	WORD							wUserCount;							//用户数量
	tagMatchUserItem				MatchUserItem[16];					//比赛用户
};

//比赛报名
struct CMD_MS_S_MatchSignup
{
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次

	WORD							wSignupCount;						//报名人数
	tagMatchUserItem				MatchUserItem;						//比赛用户	
};

//取消报名
struct CMD_MS_S_MatchUnSignup
{
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次

	WORD							wSignupCount;						//报名人数
	tagMatchUserItem				MatchUserItem;						//比赛用户	
};

//放弃比赛
struct CMD_MS_S_MatchGiveup
{
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次

	tagMatchUserItem				MatchUserItem;						//比赛用户
};

//比赛复活
struct CMD_MS_S_MatchRevive
{
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次

	WORD							wSignupCount;						//报名人数
	tagMatchUserItem				MatchUserItem;						//比赛用户
};

//比赛淘汰
struct CMD_MS_S_MatchEliminate
{
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次
	tagMatchUserItem				MatchUserItem;						//比赛用户
};

//创建失败
struct SUB_MS_S_CreateFailed
{
	DWORD							dwMatchID;							//比赛标识
	WORD							wTableCount;						//桌子数量
	WORD							wErrorCode;							//错误代码
};

//桌子激活
struct SUB_MS_S_MatchTableAppend
{
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次
	WORD							wTableCount;						//桌子数量
	tagMatchTableInfo				MatchTableInfo[0];					//桌子信息
};

//桌子更新
struct SUB_MS_S_MatchTableUpdate
{
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次
	WORD							wTableID;							//桌子标识
	BYTE							cbGameStatus;						//游戏状态
	BYTE							cbTeamCount;						//团队数量
	SCORE							lTeamScore[4];						//团队积分
};

//桌子删除
struct SUB_MS_S_MatchTableRemove
{
	DWORD							dwMatchID;							//比赛标识
	SCORE							lMatchNo;							//比赛场次
	WORD							wTableID;							//桌子标识	
};

//释放桌子
struct CMD_MS_S_MatchTableRecycle
{
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次
	WORD							wTableCount;						//桌子数量
};

//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif
