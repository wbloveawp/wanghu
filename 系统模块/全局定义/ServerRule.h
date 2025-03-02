#ifndef SERVER_RULE_HEAD_FILE
#define SERVER_RULE_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//房间规则
#define SR_RECORD_GAME_SCORE		0x00000001							//记录积分
#define SR_IMMEDIATE_WRITE_SCORE	0x00000002							//即时写分
#define SR_RECORD_GAME_VIDEO		0x00000004							//记录游戏过程 
#define SR_FORBID_ROOM_CHAT			0x00000008							//房间禁止公聊
#define SR_FORBID_ROOM_WISPER		0x00000010							//房间禁止私聊
#define SR_FORBIT_GAME_CHAT			0x00000020							//游戏禁止公聊
#define SR_ALLOW_VOICE_CHAT			0x00000040							//允许语音聊天
#define SR_TABLE_END_IMMEDIATE		0x00000080							//桌子随游戏状态改变
#define SR_DYNAMIC_JOIN_GAME		0x00000100							//动态加入游戏
#define SR_ALLOW_LARGESS_SUBSIDY	0x00000200							//允许赠送低保
#define SR_CHARGE_MODE_FEE			0x00000400							//固定服务费模式
#define SR_FORBID_PLAY_CHAT			0x00000800							//同桌游戏禁止私聊
#define SR_SIT_FOR_LOOKON			0x00001000							//坐下即旁观
#define SR_FORBID_QUERY_USER		0x00002000							//禁止远程搜索
#define SR_MEMBER_CAN_CHAT			0x00004000							//会员可聊天
#define SR_FORBID_LOOKON			0x00008000							//禁止旁观
#define SR_HIDE_USER_INFO			0x00010000							//隐藏用户信息
#define SR_MATCH_USER_BY_SCORE		0x00020000							//根据分数匹配用户
#define SR_FORBID_BUGLE				0x00040000							//禁止发喇叭
#define SR_ALLOW_AGREEMENT_DISMISS	0x00080000							//允许协议解散
#define SR_FORBIT_OFFLINE_STANDUP   0x00100000							//断线超时禁止起立
#define SR_ALLOW_ROBOT_ATTEND		0x00200000							//允许陪玩
#define SR_ALLOW_ROBOT_SIMULATE		0x00400000							//允许占位
#define SR_FORBIT_FORCE_STANDUP		0x00800000							//强退禁止起立

//桌子规则
#define TR_FORBID_WINRATE_SET		0x01000000							//禁止胜率限制
#define	TR_FORBID_FLEERATE_SET		0x02000000							//禁止逃率限制
#define	TR_FORBID_SCORE_LIMIT		0x04000000							//禁止成绩限制
#define TR_FORBID_BLACKLIST_SET		0x08000000							//禁止不跟不受欢迎玩家游戏的设置
#define TR_FORBID_IP_LIMIT			0x10000000							//禁止IP限制
#define TR_FORBID_PASSWORD_SET		0x20000000							//禁止密码设置
#define TR_FORBID_SHOW_BASESCORE	0x40000000							//禁止显示底分

//管理规则
#define MR_FORBID_ENTER_ROOM		0x00000001							//禁止进房间

//////////////////////////////////////////////////////////////////////////

//房间规则
class CServerRule
{
	//查询函数
public:
	//记录积分
	static bool IsRecordGameScore(DWORD dwServerRule) { return (dwServerRule&SR_RECORD_GAME_SCORE)!=0; }
	//即时写分
	static bool IsImmediateWriteScore(DWORD dwServerRule) { return (dwServerRule&SR_IMMEDIATE_WRITE_SCORE)!=0; }
	//记录游戏录像
	static bool IsRecordGameVideo( DWORD dwServerRule ) { return (dwServerRule&SR_RECORD_GAME_VIDEO) != 0; }
	//房间禁止公聊
	static bool IsForbidRoomChat( DWORD dwServerRule ) { return (dwServerRule&SR_FORBID_ROOM_CHAT) != 0; }
	//房间禁止私聊
	static bool IsForbidRoomWisper( DWORD dwServerRule ) { return (dwServerRule&SR_FORBID_ROOM_WISPER) != 0; }
	//游戏禁止公聊
	static bool IsForbidGameChat( DWORD dwServerRule ) { return (dwServerRule&SR_FORBIT_GAME_CHAT) != 0; }
	//允许陪玩
	static bool IsAllowRobotAttend(DWORD dwServerRule) { return (dwServerRule&SR_ALLOW_ROBOT_ATTEND)!=0; }
	//允许占位
	static bool IsAllowRobotSimulate(DWORD dwServerRule) { return (dwServerRule&SR_ALLOW_ROBOT_SIMULATE)!=0; }
	//桌子随游戏状态改变
	static bool IsTableEndImmediate( DWORD dwServerRule ) { return (dwServerRule&SR_TABLE_END_IMMEDIATE) != 0; }
	//动态加入游戏
	static bool IsDynamicJoinGame( DWORD dwServerRule ) { return (dwServerRule&SR_DYNAMIC_JOIN_GAME) != 0; }
	//固定服务费模式
	static bool IsChargeModeFee( DWORD dwServerRule ) { return (dwServerRule&SR_CHARGE_MODE_FEE) != 0; }
	//同桌禁止私聊
	static bool IsForbidPlayChat( DWORD dwServerRule ) { return (dwServerRule&SR_FORBID_PLAY_CHAT) != 0; }
	//坐下即旁观
	static bool IsSitForLookon( DWORD dwServerRule ) { return (dwServerRule&SR_SIT_FOR_LOOKON) != 0; }
	//禁止远程搜索
	static bool IsForbidQueryUser( DWORD dwServerRule ) { return (dwServerRule&SR_FORBID_QUERY_USER) != 0; }
	//会员可聊天
	static bool IsMemberCanChat( DWORD dwServerRule ) { return (dwServerRule&SR_MEMBER_CAN_CHAT) != 0; }
	//禁止旁观
	static bool IsForbidLookon( DWORD dwServerRule ) { return (dwServerRule&SR_FORBID_LOOKON) != 0; }
	//隐藏用户信息
	static bool IsHideUserInfo(DWORD dwServerRule) {return (dwServerRule&SR_HIDE_USER_INFO) != 0;}
	//根据分数匹配用户
	static bool IsMatchUserByScore(DWORD dwServerRule) {return (dwServerRule&SR_MATCH_USER_BY_SCORE) != 0;}
	//禁止发喇叭
	static bool IsForbidBugle(DWORD dwServerRule) {return (dwServerRule&SR_FORBID_BUGLE) != 0;}
	//允许协议解散
	static bool IsAllowAgreementDismiss(DWORD dwServerRule) {return (dwServerRule&SR_ALLOW_AGREEMENT_DISMISS) != 0;}
	//强退禁止起立
	static bool IsForbidForceStandup(DWORD dwServerRule) {return (dwServerRule&SR_FORBIT_FORCE_STANDUP)!=0; }
	//断线超时禁止起立
	static bool IsForbidOfflineStandup(DWORD dwServerRule) {return (dwServerRule&SR_FORBIT_OFFLINE_STANDUP)!=0; }
	//允许语音聊天
	static bool IsAllowVoiceChat(DWORD dwServerRule) {return (dwServerRule&SR_ALLOW_VOICE_CHAT)!=0; }
	//允许赠送低保
	static bool IsAllowLargessSubsidy(DWORD dwServerRule) {return (dwServerRule&SR_ALLOW_LARGESS_SUBSIDY)!=0; }

	//设置函数
public:
	//记录积分
	static VOID SetRecordGameScore(DWORD & dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=SR_RECORD_GAME_SCORE:dwServerRule&=~SR_RECORD_GAME_SCORE; }
	//即时写分
	static VOID SetImmediateWriteScore(DWORD & dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=SR_IMMEDIATE_WRITE_SCORE:dwServerRule&=~SR_IMMEDIATE_WRITE_SCORE; }
	//记录游戏录像
	static VOID SetRecordGameVideo( DWORD &dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_RECORD_GAME_VIDEO:dwServerRule&=~SR_RECORD_GAME_VIDEO;
	}
	//房间禁止公聊
	static VOID SetForbidRoomChat( DWORD &dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_FORBID_ROOM_CHAT:dwServerRule&=~SR_FORBID_ROOM_CHAT;
	}
	//房间禁止私聊
	static VOID SetForbidRoomWisper( DWORD &dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_FORBID_ROOM_WISPER:dwServerRule&=~SR_FORBID_ROOM_WISPER;
	}
	//游戏禁止公聊
	static VOID SetForbidGameChat( DWORD &dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_FORBIT_GAME_CHAT:dwServerRule&=~SR_FORBIT_GAME_CHAT;
	}
	//允许陪玩
	static VOID SetAllowRobotAttend(DWORD & dwServerRule, bool bEnable) 
	{ 
		(bEnable==true)?dwServerRule|=SR_ALLOW_ROBOT_ATTEND:dwServerRule&=~SR_ALLOW_ROBOT_ATTEND; 
	}	
	//允许占位
	static VOID SetAllowRobotSimulate(DWORD & dwServerRule, bool bEnable) 
	{ 
		(bEnable==true)?dwServerRule|=SR_ALLOW_ROBOT_SIMULATE:dwServerRule&=~SR_ALLOW_ROBOT_SIMULATE;
	}
	//桌子随游戏状态改变
	static VOID SetTableEndImmediate( DWORD & dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_TABLE_END_IMMEDIATE:dwServerRule&=~SR_TABLE_END_IMMEDIATE;
	}
	//动态加入游戏
	static VOID SetDynamicJoinGame( DWORD & dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_DYNAMIC_JOIN_GAME:dwServerRule&=~SR_DYNAMIC_JOIN_GAME;
	}
	//固定服务费模式
	static VOID SetChargeModeFee( DWORD & dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_CHARGE_MODE_FEE:dwServerRule&=~SR_CHARGE_MODE_FEE;
	}
	//同桌禁止私聊
	static VOID SetForbidPlayChat( DWORD & dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_FORBID_PLAY_CHAT:dwServerRule&=~SR_FORBID_PLAY_CHAT;
	}
	//坐下即旁观
	static VOID SetSitForLookon( DWORD & dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_SIT_FOR_LOOKON:dwServerRule&=~SR_SIT_FOR_LOOKON;
	}
	//禁止远程搜索
	static VOID SetForbidQueryUser( DWORD & dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_FORBID_QUERY_USER:dwServerRule&=~SR_FORBID_QUERY_USER;
	}
	//会员可聊天
	static VOID SetMemberCanChat( DWORD & dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_MEMBER_CAN_CHAT:dwServerRule&=~SR_MEMBER_CAN_CHAT;
	}
	//禁止旁观
	static VOID SetForbidLookon( DWORD & dwServerRule, bool bEnable )
	{
		bEnable?dwServerRule|=SR_FORBID_LOOKON:dwServerRule&=~SR_FORBID_LOOKON;
	}
	//隐藏用户信息
	static VOID SetHideUserInfo(DWORD & dwServerRule, bool bEnable)
	{
		bEnable?dwServerRule|=SR_HIDE_USER_INFO:dwServerRule&=~SR_HIDE_USER_INFO;
	}
	//设置根据分数匹配用户
	static VOID SetMatchUserByScore(DWORD & dwServerRule, bool bEnable)
	{
		bEnable?dwServerRule|=SR_MATCH_USER_BY_SCORE:dwServerRule&=~SR_MATCH_USER_BY_SCORE;
	}
	//禁止发喇叭
	static VOID SetForbidBugle(DWORD &	dwServerRule, bool bEnable)
	{
		bEnable?dwServerRule|=SR_FORBID_BUGLE:dwServerRule&=~SR_FORBID_BUGLE;
	}

	//允许协议解散
	static VOID SetAllowAgreementDismiss(DWORD & dwServerRule, bool bEnable)
	{
		bEnable?dwServerRule|=SR_ALLOW_AGREEMENT_DISMISS:dwServerRule&=~SR_ALLOW_AGREEMENT_DISMISS;
	}

	//强退禁止起立
	static VOID SetForbidForceStandup(DWORD & dwServerRule,bool bEnable)
	{ 
		bEnable?dwServerRule|=SR_FORBIT_FORCE_STANDUP:dwServerRule&=~SR_FORBIT_FORCE_STANDUP;
	}

	//断线超时禁止起立
	static VOID SetForbidOfflineStandup(DWORD & dwServerRule,bool bEnable)
	{ 
		bEnable?dwServerRule|=SR_FORBIT_OFFLINE_STANDUP:dwServerRule&=~SR_FORBIT_OFFLINE_STANDUP;
	}

	//设置语音聊天
	static VOID SetAllowVoiceChat(DWORD & dwServerRule,bool bEnable)
	{ 
		bEnable?dwServerRule|=SR_ALLOW_VOICE_CHAT:dwServerRule&=~SR_ALLOW_VOICE_CHAT;
	}

	//设置赠送低保
	static VOID SetAllowLargessSubsidy(DWORD & dwServerRule,bool bEnable)
	{ 
		bEnable?dwServerRule|=SR_ALLOW_LARGESS_SUBSIDY:dwServerRule&=~SR_ALLOW_LARGESS_SUBSIDY;
	}
	
	//查询函数
public:
	//禁止胜率限制
	static bool IsForbidWinRateSet(DWORD dwServerRule) { return (dwServerRule&TR_FORBID_WINRATE_SET)!=0; }
	//禁止逃率限制
	static bool IsForbidFleeRateSet(DWORD dwServerRule) { return (dwServerRule&TR_FORBID_FLEERATE_SET)!=0; }
	//禁止成绩限制
	static bool IsForbidScoreLimit(DWORD dwServerRule) { return (dwServerRule&TR_FORBID_SCORE_LIMIT)!=0; }
	//禁止不跟不受欢迎玩家游戏的设置
	static bool IsForbidBlackListSet(DWORD dwServerRule) { return (dwServerRule&TR_FORBID_BLACKLIST_SET)!=0; }
	//禁止IP限制
	static bool IsForbidIPLimit(DWORD dwServerRule) { return (dwServerRule&TR_FORBID_IP_LIMIT)!=0; }
	//禁止密码设置
	static bool IsForbidPasswordSet(DWORD dwServerRule) { return (dwServerRule&TR_FORBID_PASSWORD_SET)!=0; }
	//禁止显示底分
	static bool IsForbidShowBaseScore(DWORD dwServerRule) { return (dwServerRule&TR_FORBID_SHOW_BASESCORE)!=0; }

	//设置函数
public:
	//禁止胜率限制
	static VOID SetForbidWinRateSet(DWORD & dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=TR_FORBID_WINRATE_SET:dwServerRule&=~TR_FORBID_WINRATE_SET; }
	//禁止逃率限制
	static VOID SetForbidFleeRateSet(DWORD &dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=TR_FORBID_FLEERATE_SET:dwServerRule&=~TR_FORBID_FLEERATE_SET; }
	//禁止成绩限制
	static VOID SetForbidScoreLimit(DWORD &dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=TR_FORBID_SCORE_LIMIT:dwServerRule&=~TR_FORBID_SCORE_LIMIT; }
	//禁止不跟不受欢迎玩家游戏的设置
	static VOID SetForbidBlackListSet(DWORD &dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=TR_FORBID_BLACKLIST_SET:dwServerRule&=~TR_FORBID_BLACKLIST_SET; }
	//禁止IP限制
	static VOID SetForbidIPLimit(DWORD &dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=TR_FORBID_IP_LIMIT:dwServerRule&=~TR_FORBID_IP_LIMIT; }
	//禁止密码设置
	static VOID SetForbidPasswordSet(DWORD &dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=TR_FORBID_PASSWORD_SET:dwServerRule&=~TR_FORBID_PASSWORD_SET; }
	//禁止显示底分
	static VOID SetForbidShowBaseScore(DWORD &dwServerRule, bool bEnable) { (bEnable==true)?dwServerRule|=TR_FORBID_SHOW_BASESCORE:dwServerRule&=~TR_FORBID_SHOW_BASESCORE; }

	//信息函数
public:
	//支持类型
	static WORD GetSupportType()
	{
		WORD wSupportType = 0xFFFF;

		//移除积分
		wSupportType &= ~GAME_GENRE_SCORE;

#ifndef MATCH_SERVICE_MODULE		
		wSupportType &= ~GAME_GENRE_MATCH;
#endif	
#ifndef BATTLE_SERVICE_MODULE
		wSupportType &= ~GAME_GENRE_BATTLE;
#endif	
#ifndef RANKING_SERVICE_MODULE
		wSupportType &= ~GAME_GENRE_RANKING;
#endif

		return wSupportType;
	};
};

//////////////////////////////////////////////////////////////////////////


#endif