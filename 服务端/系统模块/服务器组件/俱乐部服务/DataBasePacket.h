#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//请求数据包

//系统命令
#define DBR_GR_LOAD_CLUB_LIST		1									//加载茶社
#define DBR_GR_LOAD_CLUB_ITEM		2									//查找茶社
#define DBR_GR_LOAD_CLUB_MEMBER		3									//茶社成员
#define DBR_GR_LOAD_KIND_SETTING	4									//类型设置
#define DBR_GR_LOAD_BATTLE_CONFIG	5									//加载玩法
#define DBR_GR_LOAD_APPLY_MESSAGE	6									//加载消息	

//成员命令
#define DBR_GR_QUERY_MEMBER			10									//查询成员
#define DBR_GR_UPDATE_MEMBER		11									//更新成员

//约战玩法
#define DBR_GR_APPEND_CONFIG		20									//添加玩法
#define DBR_GR_MODIFY_CONFIG		21									//修改玩法
#define DBR_GR_DELETE_CONFIG		22									//删除玩法

//游戏设置
#define DBR_GR_SET_WINNER_SCORE		30									//设置赢分

//////////////////////////////////////////////////////////////////////////
//输出信息

//茶社列表
#define DBO_GR_CLUB_LIST			100									//茶社属性
#define DBO_GR_CLUB_LIST_FINISH		101									//列表完成

//茶社子项
#define DBO_GR_CLUB_ITEM			110									//茶社属性
#define DBO_GR_CLUB_ITEM_FINISH		111									//子项完成	

//茶社命令
#define DBO_GR_CLUB_MEMBER			120									//茶社成员
#define DBO_GR_CLUB_CREATER			121									//茶社群主
#define DBO_GR_UPDATE_MEMBER		122									//更新成员

//约战玩法
#define DBO_GR_BATTLE_CONFIG		130									//约战玩法	
#define DBO_GR_APPEND_CONFIG		131									//添加玩法
#define DBO_GR_MODIFY_CONFIG		132									//修改玩法
#define DBO_GR_DELETE_CONFIG		133									//删除玩法

//游戏设置
#define DBO_GR_KIND_SETTING			140									//游戏设置
#define DBO_GR_MODIFY_SETTING		141									//设置修改

//申请消息
#define DBO_GR_APPLY_MESSAGE		150									//申请消息	

//操作结构
#define DBO_GR_OPERATE_FAILURE		300									//操作失败

//////////////////////////////////////////////////////////////////////////////////
//茶社信息

//加载茶社列表
struct DBR_GR_LoadClubList
{
	DWORD							dwClubID;							//茶社标识	
};

//加载茶社信息
struct DBR_GR_LoadClubItem
{
	DWORD							dwClubID;							//茶社标识
	bool							bActiveRoute;						//激活路由
};

//茶社成员
struct DBR_GR_LoadClubMember
{
	DWORD							dwClubID;							//茶社标识
};

//约战玩法
struct DBR_GR_LoadBattleConfig
{
	DWORD							dwClubID;							//茶社标识
};

//申请消息
struct DBR_GR_LoadApplyMessage
{
	DWORD							dwClubID;							//茶社标识
};

//类型设置  
struct DBR_GR_LoadKindSetting
{
	DWORD							dwClubID;							//茶社标识
};

//更新成员
struct DBR_GR_UpdateMember
{
	//成员信息
	DWORD							dwClubID;							//茶社标识
	DWORD							dwMemberID;							//成员标识

	//更新类型							
	BYTE							cbUpdateKind;						//更新类型

	//约战信息
	WORD							wIncCreateCount;					//创建次数
	WORD							wIncBattleCount;					//参与次数
};

//查询成员
struct DBR_GR_QueryMember
{
	//成员信息
	DWORD							dwClubID;							//茶社标识
	WORD							wMemberCount;						//成员数量
	DWORD							dwMemberID[16];						//成员标识	
};

//添加玩法
struct DBR_GR_AppendConfig
{
	//用户信息
	DWORD							dwUserID;							//用户标识

	//约战配置
	DWORD							dwClubID;							//茶社标识
	tagClubBattleConfig				BattleConfig;						//约战玩法
};

//修改玩法
struct DBR_GR_ModifyConfig
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	BYTE							cbModifyKind;						//修改类型

	//约战配置	
	DWORD							dwClubID;							//茶社标识
	tagClubBattleConfig				BattleConfig;						//约战玩法
};

//删除玩法
struct DBR_GR_DeleteConfig
{
	//用户信息
	DWORD							dwUserID;							//用户标识

	//茶社信息
	WORD							wKindID;							//类型标识
	DWORD							dwClubID;							//茶社标识
	DWORD							dwConfigID;							//玩法标识
};

//设置赢分
struct DBR_GR_SetWinnerScore
{
	WORD							wKindID;							//类型标识
	DWORD							dwClubID;							//茶社标识
	DWORD							dwUserID;							//用户标识
	SCORE							lMinWinnerScore;					//最低赢分
};

//////////////////////////////////////////////////////////////////////////////////
//茶社成员
struct DBO_GR_ClubMember
{
	DWORD							dwClubID;							//茶社标识
};

//子项完成
struct DBO_GR_ClubItemFinish
{
	DWORD							dwClubID;							//茶社标识
	bool							bActiveRoute;						//激活路由
};

//约战玩法
struct DBO_GR_BattleConfig
{
	DWORD							dwClubID;							//茶社标识
};

//添加玩法
struct DBO_GR_AppendConfig
{
	//约战配置
	DWORD							dwClubID;							//茶社标识
	tagClubBattleConfig				BattleConfig;						//约战玩法
};

//修改玩法
struct DBO_GR_ModifyConfig
{
	//约战配置	
	DWORD							dwClubID;							//茶社标识
	BYTE							cbModifyKind;						//修改类型
	tagClubBattleConfig				BattleConfig;						//约战玩法
};

//删除玩法
struct DBO_GR_DeleteConfig
{
	//茶社信息
	WORD							wKindID;							//类型标识
	DWORD							dwClubID;							//茶社标识
	DWORD							dwConfigID;							//玩法标识
};

//申请消息
struct DBO_GR_ApplyMessage
{
	DWORD							dwClubID;							//茶社标识
};

//更新成员
struct DBO_GR_UpdateMember
{
	DWORD							dwClubID;							//茶社标识
	BYTE							cbUpdateKind;						//更新类型
	tagClubMemberInfo				ClubMember;							//茶社成员
};

//游戏设置
struct DBO_GR_ClubKindSetting
{
	DWORD							dwClubID;							//茶社标识	
	WORD							wItemCount;							//子项数目
	tagClubKindSetting				KindSettingItem[0];					//设置子项
};

//游戏设置
struct DBO_GR_KindSettingModify
{
	DWORD							dwClubID;							//茶社标识	
	tagClubKindSetting				KindSettingItem;					//设置子项
};

//操作失败
struct DBO_GR_OperateFailure
{
	//错误信息
	LONG							lErrorCode;							//失败编码
	WCHAR							szErrorMessage[128];				//错误消息	
};

//////////////////////////////////////////////////////////////////////////////////

#endif