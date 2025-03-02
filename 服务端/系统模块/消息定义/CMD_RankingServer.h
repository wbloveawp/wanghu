#ifndef CMD_RANKING_SERVER_HEAD_FILE
#define CMD_RANKING_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////

//排位服务
#define MDM_AR_RANKING_SERVICE		10									//排位服务

//////////////////////////////////////////////////////////////////////////////////
//匹配操作
#define SUB_AR_C_START_MATCH		1									//开始匹配
#define SUB_AR_C_CANCEL_MATCH		2									//取消匹配

//查询操作
#define SUB_AR_C_QUERY_RANKING_CONFIG	10								//查询排位配置
#define SUB_AR_C_QUERY_RANKING_INFO		11								//查询排行榜

//匹配结果
#define SUB_AR_S_MATCH_SUCCESS		100									//匹配成功

//查询结果
#define SUB_AR_S_RANKING_CONFIG_INFO	150								//排位配置信息
#define SUB_AR_S_RANKING_INFO			151								//排行榜信息

//操作结果
#define SUB_AR_S_OPERATE_SUCCESS	200									//操作成功
#define SUB_AR_S_OPERATE_FAILURE	201									//操作失败

//////////////////////////////////////////////////////////////////////////////////

//错误代码
#define RANKING_CODE_STOP_SERVICE		1								//停止服务
#define RANKING_CODE_TABLE_UNFOUND		2								//桌子不足
#define RANKING_CODE_LOVEHEART_UNENOUGH	3								//爱心不足
#define RANKING_CODE_COST_UNENOUGH		4								//报名费不足

//////////////////////////////////////////////////////////////////////////////////

//开始匹配
struct CMD_AR_C_StartMatch
{
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
};

//取消匹配
struct CMD_AR_C_CancelMatch
{	
};

//匹配成功
struct CMD_AR_S_MatchSuccess
{
	//房间信息
	WORD							wKindID;							//类型标识
	WORD							wTableID;							//桌子标识
	WORD							wServerID;							//房间标识
	DWORD							dwRankingNum;						//排位编号	
	TCHAR							szPassword[LEN_PASSWORD];			//桌子密码

	//连接信息
	WORD							wServerPort;						//房间端口
	TCHAR							szServerDomain[LEN_DOMAIN];			//房间域名

	//用户信息
	WORD							wUserCount;							//用户数量
	tagRankingTableUser				TableUserInfo[TABLE_USER_COUNT];	//用户信息
};

//查询排位配置
struct CMD_AR_S_QueryRankingConfig
{
	tagRankingConfig				RankingConfig;						//排位配置
};

//排行榜信息
struct CMD_AN_S_RankingInfo
{
    //排名信息
	WORD							wRankID;							//自己排名
    WORD							wRankCount;							//排名数量
    tagRankingRankItem				RankingRankItem[0];					//排名子项
};

//操作成功
struct CMD_AR_S_OperateSuccess
{
	WORD							wRequestCmdID;						//操作命令
	WCHAR							szDescribeString[256];				//描述信息	
};

//操作失败
struct CMD_AR_S_OperateFailure
{
	//错误信息
	LONG							lErrorCode;							//失败编码
	WCHAR							szErrorMessage[128];				//错误消息	
};

//////////////////////////////////////////////////////////////////////////////////

//排位服务
#define MDM_RS_RANKING_SERVICE		11									//排位服务

//////////////////////////////////////////////////////////////////////////////////
//桌子操作
#define SUB_RS_C_CREATE_TABLE		301									//创建桌子
#define SUB_RS_C_DISMISS_TABLE		303									//解散桌子
#define SUB_RS_C_SYNCHRO_TABLE		304									//同步桌子
#define SUB_RS_C_RANKING_OPTION		305									//排位选项

//桌子操作
#define SUB_RS_S_CREATE_TABLE		401									//创建结果
#define SUB_RS_S_TABLE_PARAM		404									//桌子参数
#define SUB_RS_S_TABLE_STATUS		405									//桌子状态
#define SUB_RS_S_TABLE_SYNCHRO		406									//桌子同步
#define SUB_RS_S_TABLE_DISMISS		407									//桌子解散

//通知消息
#define SUB_RS_S_USER_SITDOWN		500									//用户坐下
#define SUB_RS_S_USER_STANDUP		501									//用户起立

//////////////////////////////////////////////////////////////////////////////////

//创建桌子
struct CMD_RS_C_CreateTable
{
	//桌子信息
	DWORD							dwRankingNum;						//排位编号		
	WCHAR							szTablePassword[LEN_PASSWORD];		//桌子密码

	//配置信息
	tagRankingConfig				RankingConfig;						//排位配置	
};

//解散桌子
struct CMD_RS_C_DismissTable
{
	//桌子信息
	WORD							wTableID;							//桌子标识
	DWORD							dwRankingNum;						//桌子编号	
};

//排位选项
struct CMD_RS_C_RankingOption
{
	bool							bServiceStatus;						//服务标识
};

//创建结果
struct CMD_RS_S_CreateTable
{
	//桌子信息
	WORD							wTableID;							//桌子编号	
	DWORD							dwRankingNum;						//映射编号
	DWORD							dwCreateTime;						//创建时间

	//结果信息
	BYTE							cbResultCode;						//结果代码
};

//桌子解散
struct CMD_RS_S_TableDismiss
{
	WORD							wTableID;							//桌子编号
	WORD							wServerID;							//房间标识
	DWORD							dwRankingNum;						//映射编号
};

//桌子参数
struct CMD_RS_S_TableParam
{
	DWORD							dwRankingNum;						//映射编号
	DWORD							dwStartTime;						//开始时间
	WORD							wFinishCount;						//完成局数		
};

//桌子状态
struct CMD_RS_S_TableStatus
{
	DWORD							dwRankingNum;						//排位编号
	BYTE							cbGameStatus;						//游戏状态
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif