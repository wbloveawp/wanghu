#ifndef CMD_CLUB_SERVICE_HEAD_FILE
#define CMD_CLUB_SERVICE_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////

//茶社服务
#define MDM_CLUB_SERVICE			10									//俱乐部服务

//请求命令
#define SUB_CB_C_ENTER_CLUB			1									//进入茶社

//约战命令
#define SUB_CB_C_QUERY_OPTION		10									//查询配置
#define SUB_CB_C_CREATE_TABLE		11									//创建约战
#define SUB_CB_C_MODIFY_TABLE		12									//修改约战
#define SUB_CB_C_DISMISS_TABLE		13									//解散约战

//约战玩法
#define SUB_CB_C_APPEND_CONFIG		20									//添加玩法
#define SUB_CB_C_MODIFY_CONFIG		21									//修改玩法
#define SUB_CB_C_DELETE_CONFIG		22									//删除玩法

//游戏配置
#define SUB_CB_C_SET_WINNER_SCORE	30									//设置赢分

//茶社命令
#define SUB_CB_S_CLUB_ITEM			100									//茶社信息
#define SUB_CB_S_CLUB_MEMBER		101									//茶社成员
#define SUB_CB_S_CLUB_UPDATE		102									//茶社更新
#define SUB_CB_S_MEMBER_INSERT		103									//成员添加
#define SUB_CB_S_MEMBER_UPDATE		104									//成员更新
#define SUB_CB_S_MEMBER_DELETE		105									//成员删除
#define SUB_CB_S_APPLY_MESSAGE		106									//申请消息
#define SUB_CB_S_APPLY_DELETE		107									//申请删除

//约战玩法
#define SUB_CB_S_BATTLE_CONFIG		120									//约战玩法
#define SUB_CB_S_CONFIG_APPEND		121									//玩法添加
#define SUB_CB_S_CONFIG_MODIFY		122									//玩法修改
#define SUB_CB_S_CONFIG_DELETE		123									//玩法删除

//游戏配置
#define SUB_CB_S_KIND_SETTING_ITEM	 130								//游戏设置
#define SUB_CB_S_KIND_SETTING_MODIFY 131								//设置更新

//约战命令
#define SUB_CB_S_TABLE_ITEM			150									//桌子子项
#define SUB_CB_S_TABLE_PARAM		151									//桌子参数
#define SUB_CB_S_TABLE_CREATE		152									//桌子解散
#define SUB_CB_S_TABLE_UPDATE		153									//桌子更新
#define SUB_CB_S_TABLE_DISMISS		154									//桌子解散
#define SUB_CB_S_USER_SITDOWN		155									//用户坐下
#define SUB_CB_S_USER_STANDUP		156									//用户起立
#define SUB_CB_S_TABLE_CLEANUP		157									//桌子清除

//约战配置
#define SUB_CB_S_BATTLE_BASE_OPTION	  160								//基础选项
#define SUB_CB_S_BATTLE_GAME_OPTION	  161								//游戏选项	
#define SUB_CB_S_BATTLE_OPTION_FINISH 162								//选项完成

//进入结果
#define SUB_CB_S_ENTER_SUCCESS		500									//进入成功
#define SUB_CB_S_ENTER_FAILURE		501									//进入失败

//////////////////////////////////////////////////////////////////////////////////
//修改类型
#define CLUB_BC_MK_WINNER_SCORE		0x01								//赢家分数
#define CLUB_BC_MK_BATTLE_CONFIG	0x02								//约战玩法

//////////////////////////////////////////////////////////////////////////////////

//进入茶社
struct CMD_CB_C_EnterClub
{
	DWORD							dwClubID;							//茶社标识
};

//查询配置
struct CMD_CB_C_QueryOption
{
	WORD							wKindID;							//类型标识	
};

//创建桌子
struct CMD_CB_C_CreateTable
{
	DWORD							dwClubID;							//茶社标识	
	DWORD							dwConfigID;							//配置标识
	tagBattleConfig					BattleConfig;						//约战配置
};

//修改桌子
struct CMD_CB_C_ModifyTable
{
	//用户信息
	DWORD							dwClubID;							//茶社标识
	DWORD							dwBattleNum;						//桌子编号
	tagBattleConfig					BattleConfig;						//约战配置		
};

//解散桌子
struct CMD_CB_C_DismissTable
{
	//桌子信息
	DWORD							dwClubID;							//茶社标识
	DWORD							dwBattleNum;						//桌子编号
};

//添加玩法
struct CMD_CB_C_AppendConfig
{
	DWORD							dwClubID;							//茶舍标识	
	tagBattleConfig					BattleConfig;						//约战配置
};

//修改玩法
struct CMD_CB_C_ModifyConfig
{
	//用户信息
	DWORD							dwClubID;							//茶社标识
	DWORD							dwConfigID;							//玩法标识
	BYTE							cbModifyKind;						//修改类型	
};

//删除玩法
struct CMD_CB_C_DeleteConfig
{
	WORD							wKindID;							//类型标识
	DWORD							dwClubID;							//茶社标识
	DWORD							dwConfigID;							//玩法标识	
};

//设置赢分
struct CMD_CB_C_SetWinnerScore
{
	WORD							wKindID;							//类型标识
	DWORD							dwClubID;							//茶社标识
	SCORE							lMinWinnerScore;					//最低赢分
};

//茶社成员
struct CMD_CB_S_ClubMember
{
	DWORD							dwClubID;							//茶社标识
};

//茶社更新
struct CMD_CB_S_ClubUpdate
{
	DWORD							dwClubID;							//茶社标识
	BYTE							cbUpdateKind;						//更新类型
};

//成员加入
struct CMD_CB_S_MemberInsert
{
	DWORD							dwClubID;							//茶社标识
	WORD							wMemberCount;						//成员数量
	tagClubMemberUser				MemberUser;							//成员用户	
};

//成员更新
struct CMD_CB_S_MemberUpdate
{
	DWORD							dwClubID;							//茶社标识
	tagClubMemberInfo				ClubMember;							//成员信息
};

//成员删除
struct CMD_CB_S_MemberDelete
{
	DWORD							dwClubID;							//茶社标识
	DWORD							dwMemberID;							//成员标识
	WORD							wMemberCount;						//成员数量
};

//申请消息
struct CMD_CB_S_ApplyMessage
{
	WORD							wMessageCount;						//消息数量
};

//申请删除
struct CMD_CB_S_ApplyDelete
{
	WORD							wMessageCount;						//消息数量
};

//约战玩法
struct CMD_CB_S_BattleConfig
{
	DWORD							dwClubID;							//茶社标识
};

//玩法添加
struct CMD_CB_S_ConfigAppend
{
	DWORD							dwClubID;							//茶社标识
	tagClubBattleConfig				BattleConfig;						//约战配置
};

//玩法修改
struct CMD_CB_S_ConfigModify
{
	DWORD							dwClubID;							//茶社标识
	BYTE							cbModifyKind;						//修改类型
	tagClubBattleConfig				BattleConfig;						//约战配置
};

//玩法删除
struct CMD_CB_S_ConfigDelete
{
	WORD							wKindID;							//类型标识
	DWORD							dwClubID;							//茶社标识
	DWORD							dwConfigID;							//玩法标识
};

//桌子解散
struct CMD_CB_S_TableDismiss
{
	DWORD							dwClubID;							//茶社标识
	DWORD							dwBattleNum;						//映射编号
};

//桌子参数
struct CMD_CB_S_TableParam
{
	DWORD							dwBattleNum;						//映射编号
	DWORD							dwStartTime;						//开始时间
	WORD							wFinishCount;						//完成局数
	BYTE							cbGameStatus;						//游戏状态
};

//用户坐下
struct CMD_CB_S_UserSitdown
{
	DWORD							dwUserID;							//用户标识	
	DWORD							dwBattleNum;						//映射编号	
	tagBattleTableUser				TableUserItem;						//用户信息
};

//用户起立
struct CMD_CB_S_UserStandup
{
	WORD							wChairID;							//用户椅子
	DWORD							dwUserID;							//用户标识	
	DWORD							dwBattleNum;						//映射编号	
};

//桌子清除
struct CMD_CB_S_TableCleanup
{
	DWORD							dwClubID;							//茶社标识
};

//进入成功
struct CMD_CB_S_EnterSuccess
{
	DWORD							dwClubID;							//茶社标识
};

//进入失败
struct CMD_CB_S_EnterFailure
{
	LONG							lErrorCode;							//错误代码
	WCHAR							szErrorDescribe[128];				//错误消息
};

//////////////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif