#ifndef CMD_BATTLE_SERVER_HEAD_FILE
#define CMD_BATTLE_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////

//约战服务
#define MDM_AB_BATTLE_SERVICE		10									//约战服务

//////////////////////////////////////////////////////////////////////////////////
//查询选项
#define SUB_AB_C_QUERY_OPTION		1									//查询配置	

//桌子操作
#define SUB_AB_C_QUERY_TABLE		10									//查询桌子
#define SUB_AB_C_CREATE_TABLE		11									//创建桌子
#define SUB_AB_C_MODIFY_TABLE		12									//修改桌子
#define SUB_AB_C_DISMISS_TABLE		13									//解散桌子
#define SUB_AB_C_REQUEST_TABLELIST	14									//桌子列表
#define SUB_AB_C_REQUEST_TABLEPARAM	15									//桌子参数

//配置信息
#define SUB_AB_S_BATTLE_BASE_OPTION	100									//基础选项
#define SUB_AB_S_BATTLE_GAME_OPTION	101									//游戏选项	
#define SUB_AB_S_BATTLE_OPTION_FINISH 102								//选项完成

//财富信息
#define SUB_AB_S_WEALTH_UPDATE		110									//财富更新	

//操作结果
#define SUB_AB_S_TABLE_ITEM			200									//桌子子项
#define SUB_AB_S_TABLE_LIST			201									//桌子列表
#define SUB_AB_S_TABLE_PARAM		202									//桌子参数
#define SUB_AB_S_DISMISS_TABLE		203									//解散桌子

//操作结果
#define SUB_AB_S_OPERATE_SUCCESS	210									//操作成功
#define SUB_AB_S_OPERATE_FAILURE	211									//操作失败

//////////////////////////////////////////////////////////////////////////////////

//错误代码
#define BATTLE_CODE_STOP_SERVICE	1									//停止服务
#define BATTLE_CODE_QUERY_FAILED	2									//查询失败
#define BATTLE_CODE_CREATE_FAILED	3									//创建失败
#define BATTLE_CODE_MODIFY_FAILED	4									//修改失败
#define BATTLE_CODE_TABLE_UNFOUND	5									//桌子不足

//////////////////////////////////////////////////////////////////////////////////

//查询配置
struct CMD_AD_C_QueryOption
{
	WORD							wKindID;							//类型标识
	BYTE							cbEmployScene;						//使用场景
};

//查询桌子
struct CMD_AB_C_QueryTable
{
	//查询信息
	BYTE							cbQueryKind;						//查询类型
	DWORD							dwQueryArgKey;						//查询关键字		
};

//创建桌子
struct CMD_AB_C_CreateTable
{
	//用户信息
	WCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	WCHAR							szTablePassword[LEN_PASSWORD];		//桌子密码

	//约战配置
	tagBattleConfig					BattleConfig;						//约战配置	
};

//修改桌子
struct CMD_AB_C_ModifyTable
{
	DWORD							dwBattleNum;						//约战编号
	tagBattleConfig					BattleConfig;						//约战配置	
};

//解散桌子
struct CMD_AB_C_DismissTable
{
	DWORD							dwBattleNum;						//桌子编号	
};

//请求桌子列表
struct CMD_AB_C_RequestTableList
{
	WORD							wTableCount;						//桌子数量
};

//请求桌子状态
struct CMD_AB_C_RequestTableParam
{
	WORD							wTableCount;						//桌子数量
	DWORD							dwBattleNum[0];						//桌子编号
};

//财富更新
struct CMD_AB_S_WealthUpdate
{
	BYTE							cbMask;								//更新掩码
	LONGLONG						lUserIngot;							//用户钻石		
	LONGLONG						lUserRoomCard;						//用户房卡		
};

//桌子子项
struct CMD_AB_S_TableItem
{
	//桌子子项	
	tagBattleTableItem				TableItem;							//桌子子项	
};

//桌子列表
struct CMD_AB_S_TableList
{
	//桌子信息
	WORD							wTableCount;						//桌子数目
};

//用户坐下
struct CMD_AB_S_UserSitdown
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwBattleNum;						//映射编号
	tagBattleTableUser				TableUserItem;						//用户信息
};

//用户起立
struct CMD_AB_S_UserStandup
{
	WORD							wChairID;							//用户椅子
	DWORD							dwUserID;							//用户标识
	DWORD							dwBattleNum;						//映射编号	
};

//桌子解散
struct CMD_AB_S_TableDismiss
{
	DWORD							dwBattleNum;						//映射编号
};

//解散桌子
struct CMD_AB_S_DismissTable
{
	BYTE							cbResultCode;						//结果代码
	TCHAR							szDescribeString[256];				//描述信息	
};

//操作成功
struct CMD_AB_S_OperateSuccess
{
	LONG							lOperateCode;						//操作代码
	TCHAR							szDescribeString[256];				//描述信息	
};

//操作失败
struct CMD_AB_S_OperateFailure
{
	//错误信息
	LONG							lErrorCode;							//失败编码
	TCHAR							szErrorMessage[128];				//错误消息	
};

//////////////////////////////////////////////////////////////////////////////////

//约战服务
#define MDM_BS_BATTLE_SERVICE		11									//约战服务

//////////////////////////////////////////////////////////////////////////////////
//桌子操作
#define SUB_BS_C_CREATE_TABLE		301									//创建桌子
#define SUB_BS_C_MODIFY_TABLE		302									//修改桌子
#define SUB_BS_C_DISMISS_TABLE		303									//解散桌子
#define SUB_BS_C_SYNCHRO_TABLE		304									//同步桌子
#define SUB_BS_C_BATTLE_OPTION		305									//约战选项

//桌子操作
#define SUB_BS_S_CREATE_TABLE		401									//创建结果
#define SUB_BS_S_MODIFY_TABLE		402									//修改结果
#define SUB_BS_S_DISMISS_TABLE		403									//解散桌子
#define SUB_BS_S_TABLE_PARAM		404									//桌子参数
#define SUB_BS_S_TABLE_STATUS		405									//桌子状态
#define SUB_BS_S_TABLE_SYNCHRO		406									//桌子同步
#define SUB_BS_S_TABLE_DISMISS		407									//桌子解散
#define SUB_BS_S_TABLE_USERLIST		408									//桌子用户
#define SUB_BS_S_UPDATE_CONFIG		409									//更新配置	

//通知消息
#define SUB_BS_S_USER_SITDOWN		500									//用户坐下
#define SUB_BS_S_USER_STANDUP		501									//用户起立

//////////////////////////////////////////////////////////////////////////////////

//创建桌子
struct CMD_BS_C_CreateTable
{
	//用户信息
	DWORD							dwUserID;							//用户标识	
	WCHAR							szTablePassword[LEN_PASSWORD];		//桌子密码

	//创建信息
	DWORD							dwBattleNum;						//映射编号
	tagBattleConfigEx				BattleConfig;						//约战配置

	//连接信息
	DWORD							dwClientIP;							//客户地址
	WCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列		
};

//修改桌子
struct CMD_BS_C_ModifyTable
{
	//桌子信息
	WORD							wTableID;							//桌子标识
	DWORD							dwUserID;							//用户标识	
	DWORD							dwBattleNum;						//房间编号
	tagBattleConfigEx				BattleConfig;						//约战配置	

	//网络信息
	tagSocketInfo					SocketInfo;							//网络信息
};

//解散桌子
struct CMD_BS_C_DismissTable
{
	//网络信息
	tagSocketInfo					SocketInfo;							//网络信息

	//桌子信息
	WORD							wTableID;							//桌子标识
	DWORD							dwUserID;							//用户标识	
	DWORD							dwBattleNum;						//桌子编号
};

//同步桌子
struct CMD_BS_C_BattleOption
{
	bool							bBattleService;						//服务标识
};

//创建结果
struct CMD_BS_S_CreateTable
{
	//桌子信息
	WORD							wTableID;							//桌子编号	
	DWORD							dwBattleNum;						//映射编号
	DWORD							dwCreateTime;						//创建时间

	//积分信息
	LONGLONG						lWinScore;							//赢取分数

	//结果信息
	BYTE							cbResultCode;						//结果代码
	WCHAR							szErrorMessage[128];				//错误消息
};

//修改结果
struct CMD_BS_S_ModifyTable
{
	//桌子信息
	DWORD							dwUserID;							//用户标识	
	DWORD							dwBattleNum;						//桌子编号
	tagBattleConfigEx				BattleConfig;						//约战配置	

	//网络信息
	tagSocketInfo					SocketInfo;							//网络信息

	//结果信息
	BYTE							cbResultCode;						//结果代码
	WCHAR							szErrorMessage[128];				//错误消息
};

//解散结果
struct CMD_BS_S_DismissTable
{
	//网络信息
	tagSocketInfo					SocketInfo;							//网络信息

	//结果信息
	DWORD							dwUserID;							//用户标识
	BYTE							cbResultCode;						//结果代码
	WCHAR							szDescribeString[256];				//描述信息	
};

//桌子解散
struct CMD_BS_S_TableDismiss
{
	//桌子信息
	WORD							wTableID;							//桌子编号
	WORD							wServerID;							//房间标识
	DWORD							dwBattleNum;						//映射编号
};

//桌子参数
struct CMD_BS_S_TableParam
{
	DWORD							dwBattleNum;						//映射编号
	DWORD							dwStartTime;						//开始时间
	WORD							wFinishCount;						//完成局数		
};

//桌子状态
struct CMD_BS_S_TableStatus
{
	DWORD							dwBattleNum;						//映射编号
	BYTE							cbGameStatus;						//游戏状态
};

//桌子用户
struct CMD_BS_S_TableUserList
{
	DWORD							dwBattleNum;						//映射编号
	WORD							wUserCount;							//完成局数	
	tagBattleTableUser				TableUserItem[TABLE_USER_COUNT];	//用户列表
};

//用户坐下
struct CMD_BS_S_UserSitdown
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwBattleNum;						//映射编号
	tagBattleTableUser				TableUserItem;						//用户信息
};

//用户起立
struct CMD_BS_S_UserStandup
{
	WORD							wChairID;							//用户椅子
	DWORD							dwUserID;							//用户标识	
	DWORD							dwBattleNum;						//映射编号	
};

//更新配置
struct CMD_BS_S_UpdateConfig
{
	DWORD							dwBattleNum;						//映射编号	
	DWORD							dwOwnerID;							//房主标识
};

//约战完成
struct CMD_BS_S_BattleFinish
{
	//桌子信息
	WORD							wTableID;							//桌子编号	
	DWORD							dwBattleNum;						//映射编号
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif