#ifndef CMD_BATTLE_SERVER_HEAD_FILE
#define CMD_BATTLE_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////

//约战服务
#define MDM_GB_BATTLE_SERVICE		10									//约战服务

//////////////////////////////////////////////////////////////////////////////////
//查询选项
#define SUB_GB_C_QUERY_OPTION		1									//查询配置	

//桌子操作
#define SUB_GB_C_QUERY_TABLE		10									//查询桌子
#define SUB_GB_C_SEARCH_TABLE		11									//查找桌子
#define SUB_GB_C_CREATE_TABLE		12									//创建桌子
#define SUB_GB_C_MODIFY_TABLE		13									//修改桌子
#define SUB_GB_C_DISMISS_TABLE		14									//解散桌子
#define SUB_GB_C_REQUEST_TABLELIST	15									//桌子列表
#define SUB_GB_C_REQUEST_TABLEPARAM	16									//桌子参数

//配置信息
#define SUB_GB_S_BATTLE_BASE_OPTION	  100								//基础选项
#define SUB_GB_S_BATTLE_GAME_OPTION	  101								//游戏选项	
#define SUB_GB_S_BATTLE_OPTION_FINISH 102								//选项完成

//财富信息
#define SUB_GB_S_WEALTH_UPDATE		110									//财富更新

//操作结果
#define SUB_GB_S_SEARCH_RESULT		120									//查找结果

//桌子列表
#define SUB_GB_S_TABLE_ITEM			200									//桌子子项
#define SUB_GB_S_TABLE_PARAM		201									//桌子参数
#define SUB_GB_S_TABLE_FINISH		202									//桌子完成

//桌子通知
#define SUB_GB_S_TABLE_CREATE		203									//桌子创建
#define SUB_GB_S_TABLE_UPDATE		204									//桌子更新
#define SUB_GB_S_TABLE_DISMISS		205									//桌子解散

//////////////////////////////////////////////////////////////////////////////////

//错误代码
#define BATTLE_CODE_STOP_SERVICE	1									//停止服务
#define BATTLE_CODE_QUERY_FAILED	2									//查询失败
#define BATTLE_CODE_SEARCH_FAILED	3									//查找失败
#define BATTLE_CODE_CREATE_FAILED	4									//创建失败
#define BATTLE_CODE_MODIFY_FAILED	5									//修改失败
#define BATTLE_CODE_DISMISS_FAILED	6									//解散失败
#define BATTLE_CODE_TABLE_UNFOUND	7									//桌子不足

//////////////////////////////////////////////////////////////////////////////////

//查询配置
struct CMD_GB_C_QueryOption
{
	WORD							wKindID;							//类型标识	
};

//查找桌子
struct CMD_GB_C_SearchTable
{
	DWORD							dwBattleNum;						//约战编号
};

//创建桌子
struct CMD_GB_C_CreateTable
{
	//用户信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szTablePassword[LEN_PASSWORD];		//桌子密码

	//约战配置
	tagBattleConfig					BattleConfig;						//约战配置	
};

//修改桌子
struct CMD_GB_C_ModifyTable
{
	DWORD							dwBattleNum;						//约战编号
	tagBattleConfig					BattleConfig;						//约战配置	
};

//解散桌子
struct CMD_GB_C_DismissTable
{
	DWORD							dwBattleNum;						//桌子编号	
};

//请求桌子列表
struct CMD_GB_C_RequestTableList
{
	WORD							wTableCount;						//桌子数量
};

//请求桌子状态
struct CMD_GB_C_RequestTableParam
{
	WORD							wTableCount;						//桌子数量
	DWORD							dwBattleNum[0];						//桌子编号
};

//财富更新
struct CMD_GB_S_WealthUpdate
{
	BYTE							cbMask;								//更新掩码
	LONGLONG						lUserIngot;							//用户钻石		
	LONGLONG						lUserRoomCard;						//用户房卡		
};

//桌子解散
struct CMD_GB_S_TableDismiss
{
	DWORD							dwBattleNum;						//约战编号
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

//茶馆命令
#define SUB_BS_C_MODIFY_CLUB_SCORE	310									//更新馆分
#define SUB_BS_C_UPDATE_CLUB_INFO	312									//更新桌子

//桌子操作
#define SUB_BS_S_CREATE_TABLE		401									//创建结果
#define SUB_BS_S_MODIFY_TABLE		402									//修改结果
#define SUB_BS_S_DISMISS_TABLE		403									//解散桌子
#define SUB_BS_S_TABLE_PARAM		404									//桌子参数
#define SUB_BS_S_TABLE_SYNCHRO		405									//桌子同步
#define SUB_BS_S_TABLE_DISMISS		406									//桌子解散
#define SUB_BS_S_TABLE_USERLIST		407									//桌子用户
#define SUB_BS_S_SYNCHRO_FINISH		408									//同步完成
#define SUB_BS_S_UPDATE_CONFIG		409									//更新配置	
#define SUB_BS_S_BATTLE_FINISH		410									//约战完成

//茶馆命令
#define SUB_BS_S_CLUB_SCORE			450									//茶馆分数	
#define SUB_BS_S_CLUB_WEALTH		453									//茶馆财富

//用户动态
#define SUB_BS_S_USER_SITDOWN		500									//用户坐下
#define SUB_BS_S_USER_STANDUP		501									//用户起立

//////////////////////////////////////////////////////////////////////////////////

//创建桌子
struct CMD_BS_C_CreateTable
{
	//用户信息
	DWORD							dwUserID;							//用户标识	
	TCHAR							szPassword[LEN_PASSWORD];			//桌子密码

	//茶社信息
	DWORD							dwClubID;							//茶社标识
	WORD							wSwitchOption;						//开关选项
	WORD							wClubServiceID;						//服务标识
	DWORD							dwClubCreaterID;					//群主标识	

	//创建信息
	DWORD							dwBattleNum;						//映射编号
	DWORD							dwConfigID;							//玩法标识
	tagBattleConfigEx				BattleConfig;						//约战配置	
};

//修改桌子
struct CMD_BS_C_ModifyTable
{
	//桌子信息
	WORD							wTableID;							//桌子标识
	DWORD							dwUserID;							//用户标识	
	DWORD							dwBattleNum;						//桌子编号
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


//更新茶馆
struct CMD_BS_C_UpdateClubInfo
{
	//桌子信息
	WORD							wTableID;							//桌子标识
	DWORD							dwClubID;							//茶馆标识
	DWORD							dwBattleNum;						//桌子编号

	//变更信息
	WORD							wSwitchOption;						//茶馆规则
	DWORD							dwClubCreaterID;					//馆主标识
};

//查询馆分
struct CMD_BS_C_ModifyClubScore
{
	DWORD							dwUserID;							//用户标识
	BYTE							cbChangeType;						//变更类型
	LONGLONG						lChangeScore;						//变更分数

	//茶馆信息
	DWORD							dwClubID;							//茶馆标识
	TCHAR							szClubName[LEN_CLUB_NAME];			//茶馆名称
	TCHAR							szOperaterNickName[LEN_ACCOUNTS];	//用户昵称
};

//创建结果
struct CMD_BS_S_CreateTable
{
	//桌子信息
	WORD							wTableID;							//桌子编号	
	DWORD							dwBattleNum;						//映射编号
	DWORD							dwCreateTime;						//创建时间

	//财富信息
	SCORE							lUserGold;							//用户金币
	SCORE							lUserCard;							//用户房卡
	DWORD							dwPayUserID;						//玩家标识

	//结果信息
	LONG							lResultCode;						//结果代码
	TCHAR							szErrorMessage[128];				//错误消息
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
	TCHAR							szErrorMessage[128];				//错误消息
};

//解散结果
struct CMD_BS_S_DismissTable
{
	//桌子信息
	DWORD							dwBattleNum;						//约战编号
	tagSocketInfo					SocketInfo;							//网络信息

	//结果信息
	BYTE							cbResultCode;						//结果代码
	TCHAR							szDescribeString[256];				//描述信息	
};

//桌子解散
struct CMD_BS_S_TableDismiss
{
	//桌子信息
	WORD							wTableID;							//桌子编号
	WORD							wServerID;							//桌子标识
	DWORD							dwBattleNum;						//映射编号
};

//桌子参数
struct CMD_BS_S_TableParam
{
	DWORD							dwBattleNum;						//映射编号
	DWORD							dwStartTime;						//开始时间
	WORD							wFinishCount;						//完成局数
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
	DWORD							dwPayUserID;						//支付玩家
};

//茶馆分数
struct CMD_BS_S_ClubScore
{
	DWORD							dwClubID;							//群组标识
	DWORD							dwUserID;							//用户标识
	SCORE							lRevenue;							//游戏税收
	SCORE							lClubScore;							//茶馆分数
	DWORD							dwBattleNum;						//映射编号
};

//群组财富
struct CMD_BS_S_ClubWealth
{
	DWORD							dwClubID;							//群组标识
	DWORD							dwBattleNum;						//映射编号

	//财富信息
	SCORE							lUserGold;							//用户金币
	SCORE							lUserCard;							//房卡数量
	DWORD							dwPayUserID;						//支付玩家
};

//同步完成
struct CMD_BS_S_SynchroFinish
{
	WORD							wServerID;							//房间标识
};

//约战完成
struct CMD_BS_S_BattleFinish
{
	//桌子信息
	WORD							wTableID;							//桌子编号	
	DWORD							dwBattleNum;						//映射编号
};


//////////////////////////////////////////////////////////////////////////////////

//约战服务
#define MDM_BC_BATTLE_SERVICE		12									//约战服务

//////////////////////////////////////////////////////////////////////////////////
//查询命令
#define SUB_BC_C_QUERY_OPTION		100									//查询配置
#define SUB_BC_C_MODIFY_SCORE		101									//修改积分
 
//茶社命令
#define SUB_BC_C_CLUB_UPDATE		110									//修改茶社
#define SUB_BC_C_CLUB_DELETE		111									//删除茶社
#define SUB_BC_C_CLUB_TRANSFER		112									//转让茶社

//约战桌子
#define SUB_BC_C_QUERY_TABLE		120									//查询桌子
#define SUB_BC_C_CREATE_TABLE		121									//创建桌子
#define SUB_BC_C_MODIFY_TABLE		122									//修改桌子
#define SUB_BC_C_DISMISS_TABLE		123									//解散桌子
#define SUB_BC_C_EFFICACY_RESULT	125									//验证结果

//约战通知
#define SUB_BC_S_TABLE_CREATE		500									//桌子创建
#define SUB_BC_S_TABLE_UPDATE		501									//桌子更新
#define SUB_BC_S_TABLE_DISMISS		502									//桌子解散
#define SUB_BC_S_TABLE_ITEM			503									//桌子子项
#define SUB_BC_S_TABLE_PARAM		504									//桌子参数
#define SUB_BC_S_BATTLE_FINISH		505									//约战完成
#define SUB_BC_S_USER_SITDOWN		506									//用户坐下
#define SUB_BC_S_USER_STANDUP		507									//用户起立
#define SUB_BC_S_USER_EFFICACY		508									//用户验证

//茶舍命令
#define SUB_BC_S_CLUB_SCORE			510									//茶馆积分
#define SUB_BC_S_CLUB_WEALTH		511									//茶馆财富

//配置信息
#define SUB_BC_S_BATTLE_BASE_OPTION	  520								//基础选项
#define SUB_BC_S_BATTLE_GAME_OPTION	  521								//游戏选项	
#define SUB_BC_S_BATTLE_OPTION_FINISH 522								//选项完成

//操作结果
#define SUB_BC_S_OPERATE_SUCCESS	800									//操作成功
#define SUB_BC_S_OPERATE_FAILURE	801									//操作失败

//////////////////////////////////////////////////////////////////////////////////

//查询配置
struct CMD_BC_C_QueryOption
{
	WORD							wKindID;							//类型标识	
	BYTE							cbEmployScene;						//使用场景
	tagSocketInfo					SocketInfo;							//网络信息
};

//更新茶社
struct CMD_BC_C_ClubUpdate
{
	//茶社信息
	DWORD							dwClubID;							//茶社标识	
	BYTE							cbUpdateKind;						//更新类型	
};

//删除茶社
struct CMD_BC_C_ClubDelete
{
	DWORD							dwClubID;							//茶社标识
};

//升级茶社
struct CMD_BC_C_ClubUpgrade
{
	//茶社标识
	DWORD							dwClubID;							//茶社标识	
};

//转让茶社
struct CMD_BC_C_ClubTransfer
{
	//茶社标识
	DWORD							dwClubID;							//茶社标识
	DWORD							dwNewCreaterID;						//新馆主标识	
};

//查询桌子
struct CMD_BC_C_QueryTable
{
	DWORD							dwClubID;							//茶社标识
};

//创建桌子
struct CMD_BC_C_CreateTable
{
	//用户信息
	DWORD							dwUserID;							//用户标识	

	//茶社信息
	DWORD							dwClubID;							//茶社标识
	DWORD							dwClubCreaterID;					//群主标识
	DWORD							wSwitchOption;						//开关选项

	//约战配置
	DWORD							dwConfigID;							//玩法标识	
	tagBattleConfig					BattleConfig;						//约战配置

	//网络信息
	tagSocketInfo					SocketInfo;							//网络信息
};

//修改桌子
struct CMD_BC_C_ModifyTable
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//茶社标识
	DWORD							dwBattleNum;						//桌子编号

	//约战配置
	tagBattleConfig					BattleConfig;						//约战配置	

	//网络信息
	tagSocketInfo					SocketInfo;							//网络信息
};

//解散桌子
struct CMD_BC_C_DismissTable
{
	//桌子信息
	DWORD							dwBattleNum;						//桌子编号

	//用户信息
	DWORD							dwUserID;							//用户标识	
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码	

	//网络信息
	tagSocketInfo					SocketInfo;							//网络信息
};

//查询积分
struct CMD_BC_C_ModifyScore
{
	//用户信息
	DWORD							dwUserID;							//用户标识	
	WORD							wKindID;							//类型标识
	WORD							wServerID;							//桌子标识

	//变更信息
	BYTE							cbChangeType;						//变更类型
	SCORE							lChangeScore;						//变更分数

	//茶馆信息
	DWORD							dwClubID;							//茶馆标识
	TCHAR							szClubName[LEN_CLUB_NAME];			//茶馆名称
	TCHAR							szOperaterNickName[LEN_ACCOUNTS];	//用户昵称
};

//验证结果
struct CMD_BC_C_EfficacyResult
{
	bool							bResult;							//验证结果
	DWORD							dwUserID;							//用户标识
	DWORD							dwBattleNum;						//映射编号
	tagSocketInfo					UserSocketInfo;						//网络信息
	TCHAR							szErrorMessage[64];					//错误消息
};

//添加玩法
struct CMD_BC_C_AppendConfig
{
	DWORD							dwClubID;							//茶舍标识
	DWORD							dwUserID;							//用户标识
	tagBattleConfig					BattleConfig;						//约战配置
};

//修改玩法
struct CMD_BC_C_ModifyConfig
{
	//用户信息
	DWORD							dwClubID;							//群组标识
	DWORD							dwUserID;							//用户标识
	DWORD							dwConfigID;							//玩法标识
	tagBattleConfig					BattleConfig;						//约战配置		
};

//删除玩法
struct CMD_BC_C_DeleteConfig
{
	WORD							wKindID;							//类型标识
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//群组标识
	DWORD							dwConfigID;							//玩法标识	
};

//桌子解散
struct CMD_BC_S_TableDismiss
{
	DWORD							dwClubID;							//茶社标识
	DWORD							dwBattleNum;						//约战编号
	WORD							wUserCount;							//用户数量
};

//玩法添加
struct CMD_BC_S_ConfigAppend
{
	DWORD							dwClubID;							//茶社标识
	tagClubBattleConfig				BattleConfig;						//约战玩法
};

//玩法修改
struct CMD_BC_S_ConfigModify
{
	DWORD							dwClubID;							//茶社标识
	tagClubBattleConfig				BattleConfig;						//约战玩法
};

//玩法删除
struct CMD_BC_S_ConfigDelete
{
	WORD							wKindID;							//类型标识	
	DWORD							dwClubID;							//茶社标识	
	DWORD							dwConfigID;							//玩法标识
};

//桌子参数
struct CMD_BC_S_TableParam
{
	DWORD							dwClubID;							//茶社标识
	DWORD							dwBattleNum;						//映射编号
	DWORD							dwStartTime;						//开始时间
	WORD							wFinishCount;						//完成局数
	BYTE							cbGameStatus;						//游戏状态
};

//约战完成
struct CMD_BC_S_BattleFinish
{
	DWORD							dwClubID;							//茶社标识
	DWORD							dwBattleNum;						//桌子编号
	WORD							wUserCount;							//用户数量
};

//用户坐下
struct CMD_BC_S_UserSitdown
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//茶社标识
	DWORD							dwBattleNum;						//映射编号
	tagBattleTableUser				TableUserItem;						//用户信息
};

//用户起立
struct CMD_BC_S_UserStandup
{
	WORD							wChairID;							//用户椅子
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//茶社标识
	DWORD							dwBattleNum;						//映射编号	
};

//用户验证
struct CMD_BC_S_UserEfficacy
{
	WORD							wBattleID;							//约战标识		
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//茶社标识
	DWORD							dwBattleNum;						//映射编号	
	tagSocketInfo					UserSocketInfo;						//网络信息
};

//茶舍分数
struct CMD_BC_S_ClubScore
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//茶舍标识
	SCORE							lRevenue;							//游戏税收
	SCORE							lClubScore;							//茶舍分数
};

//茶社财富
struct CMD_BC_S_ClubWealth
{
	DWORD							dwClubID;							//茶社标识
	DWORD							dwPayUserID;						//代付标识
	SCORE							lUserCard;							//房卡数量
	BYTE							cbWealthMask;						//财富掩码
};

//操作成功
struct CMD_BC_S_OperateSuccess
{
	//茶社信息
	DWORD							dwClubID;							//茶社标识

	//网络信息
	tagSocketInfo					SocketInfo;							//网络信息

	//结果信息
	LONG							lOperateCode;						//操作代码
	TCHAR							szDescribeString[256];				//描述信息	
};

//操作失败
struct CMD_BC_S_OperateFailure
{
	//茶社信息
	DWORD							dwClubID;							//茶社标识

	//网络信息
	tagSocketInfo					SocketInfo;							//网络信息

	//错误信息
	LONG							lErrorCode;							//失败编码
	TCHAR							szErrorMessage[128];				//错误消息	
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif