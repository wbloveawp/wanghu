#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//请求数据包

//登录命令
#define	DBR_GP_LOGON_GAMEID			1									//I D 登录
#define	DBR_GP_LOGON_ACCOUNTS		2									//帐号登录
#define DBR_GP_REGISTER_ACCOUNTS	3									//注册帐号

//账号服务
#define DBR_GP_MODIFY_ACCOUNTS		10									//修改帐号
#define DBR_GP_MODIFY_LOGON_PASS	11									//修改密码
#define DBR_GP_MODIFY_INSURE_PASS	12									//修改密码
#define DBR_GP_MODIFY_INDIVIDUAL	13									//修改资料
#define DBR_GP_BIND_MACHINE			15									//锁定机器
#define DBR_GP_UN_BIND_MACHINE		16									//解锁机器

//查询命令
#define DBR_GP_QUERY_INDIVIDUAL		32									//查询资料

//系统命令
#define DBR_GP_LOAD_CONFIG			50									//加载配置
#define DBR_GP_LOAD_RANKING_LIST	51									//加载榜单
#define DBR_GP_RESET_USER_DATA		52									//重置数据
#define DBR_GP_ONLINE_COUNT_INFO	53									//在线信息

//游戏配置
#define DBR_GP_QUERY_GAME_CONFIG	60									//查询配置
#define DBR_GP_UPDATE_BASIC_CONFIG	61									//更新配置
#define DBR_GP_UPDATE_WEIGHT_CONFIG	62									//更新配置

//机器命令
#define DBR_GP_QUERY_PARAMETER		70									//获取参数
#define DBR_GP_APPEND_PARAMETER		71									//添加参数
#define DBR_GP_MODIFY_PARAMETER		72									//修改参数
#define DBR_GP_DELETE_PARAMETER		73									//删除参数

//库存命令
#define DBR_GP_QUERY_STOCK_INFO		80									//查询库存
#define DBR_GP_APPEND_STOCK_INFO	81									//添加库存
#define DBR_GP_MODIFY_STOCK_INFO	82									//修改库存
#define DBR_GP_DELETE_STOCK_INFO	83									//删除库存
#define DBR_GP_ADJUCT_STOCK_SCORE	84									//调整库存
#define DBR_GP_ADJUCT_JACKPOT_SCORE	85									//调整彩金

//////////////////////////////////////////////////////////////////////////

//ID 登录
struct DBR_GP_LogonByGameID
{
	//登录信息
	DWORD							dwGameID;							//用户 I D
	TCHAR							szPassword[LEN_MD5];				//登录密码

	//附加信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列

	//连接信息
	DWORD							dwPostTime;
	LPVOID							pTokenParameter;					//绑定参数

	//扩展信息
	DWORD							dwCheckUserRight;					//检查权限
};

//帐号登录
struct DBR_GP_LogonByAccounts
{
	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号

	//附加信息
	DWORD							dwClientIP;							//连接地址
	DWORD							dwLogonServerID;					//登录 I D
	DWORD							dwPlazaVersion;						//大厅版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szNewComputerID[LEN_MACHINE_ID];	//机器序列 

	//连接信息
	WORD							wGateID;							//网关标识
	WORD							wLogonID;							//登录标识
	DWORD							dwPostTime;							//投递时间
	LPVOID							pTokenParameter;					//绑定参数	

	//扩展信息
	DWORD							dwCheckUserRight;					//检查权限
};

//帐号注册
struct DBR_GP_RegisterAccounts
{
	//注册信息
	WORD							wFaceID;							//头像标识
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户帐号
	TCHAR							szBankPass[LEN_MD5];				//银行密码

	//用户信息
	BYTE							cbAge;								//用户年龄
	BYTE							cbGender;							//用户性别
	TCHAR							szArea[LEN_AREA];					//地区信息
	TCHAR							szCity[LEN_CITY];					//城市信息
	TCHAR							szProvince[LEN_PROVINCE];			//省份信息

	//附加信息
	WORD							wClientMask;						//终端掩码
	DWORD							dwClientIP;							//连接地址
	DWORD							dwLogonServerID;					//登录 I D
	TCHAR							szMachineID[LEN_MACHINE_ID];	//机器序列
	TCHAR							szValidComputerID[LEN_MACHINE_ID];//机器序列
	DWORD							dwPlazaVersion;						//大厅版本

	//身份信息
	BYTE							cbPassportType;						//身份类型
	TCHAR							szPassportID[LEN_PASSPORT_ID];		//身份信息

	//推荐信息
	TCHAR							szSpreader[LEN_ACCOUNTS];			//推荐玩家

	//连接信息
	DWORD							dwPostTime;
	LPVOID							pTokenParameter;					//绑定参数
};

//帐号激活
struct DBR_GP_ActivateAccounts
{
	//注册信息
	WORD							wFaceID;							//头像标识
	TCHAR							szOrgPassword[LEN_MD5];				//原始密码
	TCHAR							szOrgAccounts[LEN_ACCOUNTS];		//原始帐号
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户帐号
	TCHAR							szBankPass[LEN_MD5];				//银行密码

	//用户信息
	BYTE							cbAge;								//用户年龄
	BYTE							cbGender;							//用户性别
	TCHAR							szArea[LEN_AREA];					//地区信息
	TCHAR							szCity[LEN_CITY];					//城市信息
	TCHAR							szProvince[LEN_PROVINCE];			//省份信息

	//附加信息
	DWORD							dwClientIP;							//连接地址
	DWORD							dwLogonServerID;					//登录 I D
	DWORD							dwPlazaVersion;						//大厅版本
	TCHAR							szMachineID[LEN_MACHINE_ID];	//机器序列

	//身份信息
	BYTE							cbPassportType;						//身份类型
	TCHAR							szPassportID[LEN_PASSPORT_ID];		//身份信息

	//推荐信息
	TCHAR							szSpreader[LEN_ACCOUNTS];			//推荐玩家

	//连接信息
	DWORD							dwPostTime;
	LPVOID							pTokenParameter;					//绑定参数
};

//修改帐号
struct DBR_GP_ModifyAccounts
{
	//验证资料
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientIP;							//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//帐号资料
	WORD							wFaceID;							//头像标识
	TCHAR							szAccounts[LEN_ACCOUNTS];			//用户帐号
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//年龄性别
	BYTE							cbAge;								//用户年龄
	BYTE							cbGender;							//用户性别

	//地区资料
	TCHAR							szArea[LEN_AREA];					//地区信息
	TCHAR							szCity[LEN_CITY];					//城市信息
	TCHAR							szProvince[LEN_PROVINCE];			//省份信息
	bool							bOldVersion;						//是否旧版
};

//修改密码
struct DBR_GP_ModifyLogonPass
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientIP;							//连接地址
	TCHAR							szDesPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
};

//修改密码
struct DBR_GP_ModifyInsurePass
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientIP;							//连接地址
	TCHAR							szDesPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
};

//修改资料
struct DBR_GP_ModifyIndividual
{
	//验证资料
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientIP;							//连接地址
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//用户信息
	TCHAR							szUserNote[LEN_USER_NOTE];			//用户说明
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字

	//电话号码
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//固定电话
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//移动电话

	//联系资料
	TCHAR							szQQ[LEN_QQ];						//Q Q 号码
	TCHAR							szEMail[LEN_EMAIL];					//电子邮件
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//联系地址
};

//查询资料
struct DBR_GP_QueryIndividual
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientIP;							//连接地址
};

//查询记录
struct DBR_GP_QueryBattleRecord
{
	//验证资料
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//版本信息
	BYTE							cbFuncVersion;						//功能版本
};

//锁定机器
struct DBR_GP_BindMachine
{
	//验证资料
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//机器码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//解锁机器
struct DBR_GP_UnbindMachine
{
	//验证资料
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//机器码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//申请密保
struct DBR_GP_AccountSecurity
{
	//验证资料
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//保护资料
	TCHAR							szQuestion[LEN_PROTECT_QUESTION];	//问题一
	TCHAR							szResponse[LEN_PROTECT_QUESTION];	//答案一
	TCHAR							szQuestion2[LEN_PROTECT_QUESTION];	//问题二
	TCHAR							szResponse2[LEN_PROTECT_QUESTION];	//答案二
	TCHAR							szQuestion3[LEN_PROTECT_QUESTION];	//问题三
	TCHAR							szResponse3[LEN_PROTECT_QUESTION];	//答案三
	BYTE							cbPassportType;						//证件类型
	TCHAR							szPassportID[LEN_PASSPORT_ID];		//证件号码
	TCHAR							szSafeEmail[LEN_EMAIL];				//安全邮箱

	DWORD							dwClientIP;							//连接地址
};

//重置数据
struct DBR_GP_ResetUserData
{
	WORD							wDayOfWeek;							//日期标识
	TCHAR							szUserIDList[2048];					//用户列表	
};

//检测帐号
struct DBR_GP_TestAccounts
{
	TCHAR							szAccounts[LEN_ACCOUNTS];			//帐号
	TCHAR							szMechine[LEN_MACHINE_ID];			//机器码
	BYTE							cbCheckType;						//检查类型
	DWORD							dwClientIP;							//连接地址
};

//在线信息
struct DBR_GP_OnLineCountInfo
{
	WORD							wKindCount;							//类型数目
	DWORD							dwOnLineCountSum;					//总在线数
	tagOnLineInfoKind				OnLineCountKind[MAX_KIND];			//类型在线
};

//银行存取
struct DBR_GP_BankAccess
{
	DWORD							dwUserID;							//用户ID
	TCHAR							szBankPass[LEN_PASSWORD];			//银行密码
	LONGLONG						lAmount;							//存取数量
	DWORD							dwClientIP;							//连接地址
};

//银行转帐
struct DBR_GP_BankTransfer
{
	DWORD							dwUserID;							//用户ID
	DWORD							dwRecvGameID;						//接收用户
	TCHAR							szBankPass[LEN_PASSWORD];			//银行密码
	LONGLONG						lAmount;							//转帐数量
	DWORD							dwClientIP;							//连接地址
};

//查询用户
struct DBR_GP_BankQuery
{
	TCHAR							szUser[LEN_ACCOUNTS];				//用户帐号
	bool							bAccount;							//是否帐号
};

struct DBR_GP_BankUpdate
{
	DWORD							dwUserID;							//用户ID
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
};

//查询低保
struct DBR_GP_QuerySubsidy
{
	//用户信息
	DWORD							dwUserID;							//用户ID	

	//连接信息	
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//领取低保
struct DBR_GP_LargessSubsidy
{
	//用户信息
	DWORD							dwUserID;							//用户ID
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//连接信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//查询配置
struct DBR_GP_QueryGameConfig
{
	WORD							wKindID;							//类型标识标识
};

//保存配置
struct DBR_GP_UpdateBasicConfig
{
	WORD							wKindID;							//类型标识
	TCHAR							szBasicConfig[4000];				//基础配置
};

//保存配置
struct DBR_GP_UpdateWeightConfig
{
	WORD							wKindID;							//类型标识
	BYTE							cbTypeID;							//权重类型
	WORD							wItemCount;							//子项数目
	tagTimesWeightItem				TimesWeightItem[128];				//权重数组
};

//查询参数
struct DBR_GP_QueryParameter
{
	WORD							wServerID;							//房间标识
};

//添加参数
struct DBR_GP_AppendParameter
{
	WORD							wServerID;							//房间标识
	tagRobotParameter				RobotParameter;						//机器参数
};

//修改参数
struct DBR_GP_ModifyParameter
{
	WORD							wServerID;							//房间标识
	tagRobotParameter				RobotParameter;						//机器参数
};

//删除参数
struct DBR_GP_DeleteParameter
{
	WORD							wServerID;							//房间标识
	DWORD							dwBatchID;							//批次标识
};

//查询库存
struct DBR_GP_QueryStockInfo
{
	TCHAR							szKindID[128];						//类型列表
};

//添加参数
struct DBR_GP_AppendStockInfo
{
	tagStockInfo					StockInfo;							//库存子项
};

//修改参数
struct DBR_GP_ModifyStockInfo
{
	tagStockInfo					StockInfo;							//库存子项
};

//删除库存
struct DBR_GP_DeleteStockInfo
{
	WORD							wStockID;							//库存标识	
};

//调整库存
struct DBR_GP_AdjustStockScore
{
	WORD							wStockID;							//库存标识
	SCORE							lChangeScore;						//变更分数
};

//调整彩金
struct DBR_GP_AdjustJackpotScore
{
	WORD							wStockID;							//库存标识
	WORD							wLevelID;							//彩池等级
	SCORE							lChangeScore;						//变更积分
};

//记录用户
struct DBR_GP_RecordUpdateUser
{
	DWORD							dwUserID;							//用户标识
};

//加载配置
struct DBR_GP_LoadConfig
{
	bool							bInitService;						//初始标识
	DWORD							dwConfigMask;						//配置掩码
};

//////////////////////////////////////////////////////////////////////////
//PC输出命令

//登录结果
#define DBO_GP_LOGON_SUCCESS		100									//登录成功
#define DBO_GP_LOGON_FAILURE		101									//登录失败

//用户信息
#define DBO_GP_USER_INDIVIDUAL		110									//用户资料

//服务结果
#define DBO_GP_OPERATE_SUCCESS		120									//操作成功
#define DBO_GP_OPERATE_FAILURE		121									//操作失败
#define DBO_GP_BIND_MANCHINE_RESULT 122									//绑定结果

//列表结果
#define DBO_GP_GAME_TYPE_ITEM		130									//种类信息
#define DBO_GP_GAME_KIND_ITEM		131									//类型信息
#define DBO_GP_GAME_NODE_ITEM		132									//节点信息
#define DBO_GP_GAME_PAGE_ITEM		133									//定制信息
#define DBO_GP_GAME_FILTER_ITEM		134									//过滤信息

//游戏配置
#define DBO_GP_GAME_BASIC_CONFIG	140									//基础配置
#define DBO_GP_GAME_WEIGHT_CONFIG	141									//权重配置
#define DBO_GP_GAME_CONFIG_FINISH	142									//配置完成

//操作结果
#define DBO_GP_CONFIG_RESULT		150									//配置结果

//结果数据
#define DBO_GP_VERSION_INFO			160									//版本信息	
#define DBO_GP_FILTER_WORDS			161									//过滤词汇
#define DBO_GP_GLOBAL_OPTION		162									//全局配置
#define DBO_GP_RANKING_LIST_DATA	163									//榜单数据									
#define DBO_GP_TRANSFER_PARAMETER	164									//转账参数
#define DBO_GP_STOCK_OPERATE_RESULT 165									//操作结果
#define DBO_GP_ROBOT_OPERATE_RESULT 166									//操作结果

//////////////////////////////////////////////////////////////////////////

//登录成功
struct DBO_GP_LogonSuccess
{
	//属性资料
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	DWORD							dwGroupID;							//社团标识
	DWORD							dwCustomID;							//自定索引
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字
	BYTE							cbMasterOrder;						//管理员等级
	DWORD							dwProtectID;						//密保标识
	BYTE							cbMoorMachine;						//绑定机器标识	

	//用户资料
	BYTE							cbAge;								//用户年龄
	BYTE							cbGender;							//用户性别
	TCHAR							szArea[LEN_AREA];					//地区信息
	TCHAR							szCity[LEN_CITY];					//城市信息
	TCHAR							szProvince[LEN_PROVINCE];			//省份信息
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//经验等级
	DWORD							dwExperience;						//经验数值

	//会员资料
	DWORD							dwMemberPoint;						//会员点数
	DWORD							dwMemberOverTime;					//到期时间

	//财富信息
	LONGLONG						lUserGold;							//用户黄金
	LONGLONG						lInsureGold;						//银行金币

	//锁定信息
	WORD							wLockGateID;						//网关标识
	WORD							wLockLogonID;						//登录标识
	DWORD							dwLockSocketID;						//网络标识
	
	//扩展信息 
	DWORD							dwCheckUserRight;					//检查权限

	//描述信息
	DWORD							dwPostTime;
	TCHAR							szDescribeString[128];				//描述消息
};


//登录失败
struct DBO_GP_LogonFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//错误消息
};

//个人资料
struct DBO_GP_UserIndividual
{
	//用户信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szUserNote[LEN_USER_NOTE];			//用户说明
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字

	//电话号码
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//固定电话
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//移动电话

	//联系资料
	TCHAR							szQQ[LEN_QQ];						//Q Q 号码
	TCHAR							szEMail[LEN_EMAIL];					//电子邮件
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//联系地址
};

//基础配置
struct DBO_GP_GameBasicConfig
{
	TCHAR							szBasicConfig[4000];				//基础配置
};

//权重配置
struct DBO_GP_GameWeightConfig
{
	WORD							wItemCount;							//子项数目
	tagTimesWeightItem				TimesWeightItem[128];				//权重数组
};

//操作失败
struct DBO_GP_OperateFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//错误消息
};

//操作成功
struct DBO_GP_OperateSuccess
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//成功消息
};

//绑定机器
struct DBO_GP_BindMachineResult
{
	LONG							lResultCode;						//操作代码
	WCHAR							szDescribeString[128];				//成功消息
};

//库存结果
struct DBO_GP_StockOperateResult
{
	WORD							wPacketIdx;							//数据包ID
	WORD							wSubCommdID;						//子命令码
	WORD							wStockCount;						//参数数目
	tagStockInfo					StockList[24];						//库存列表
};

//机器参数
struct DBO_GP_RobotOperateResult
{
	WORD							wSubCommdID;						//子命令码
	WORD							wServerID;							//房间标识
	WORD							wParameterCount;					//参数数目
	tagRobotParameter				RobotParameter[MAX_BATCH];			//机器参数
};

//操作结果
struct DBO_GP_UpdateConfigResult
{
	WORD							wKindID;							//类型标识
	DWORD							dwConfigMask;						//配置掩码
	LONG							lResultCode;						//操作代码
	WCHAR							szDescribeString[128];				//成功消息
};

//配置结果
struct DBO_GP_ConfigResult
{
	bool							bResultCode;						//结果标识
	bool							bInitService;						//初始标识		
	DWORD							dwConfigMask;						//配置掩码
};

//榜单数据
struct DBO_GP_RankingListData
{
	BYTE							cbListType;							//榜单类型
	CHAR							szRankingList[8192];				//榜单数据
};

//////////////////////////////////////////////////////////////////////////

//游戏类型
struct DBO_GP_GameType
{
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wTypeID;							//类型索引
	TCHAR							szTypeName[LEN_TYPE];				//种类名字
};

//游戏种类
struct DBO_GP_GameKind
{
	bool							bNullity;							//禁用标识
	WORD							wModuleID;							//模块标识
	WORD							wTypeID;							//类型索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wKindID;							//名称索引
	WORD							wSupportMask;						//支持掩码
	TCHAR							szKindName[LEN_KIND];				//游戏名字
	TCHAR							szProcessName[LEN_PROCESS];			//进程名字
};

//游戏节点
struct DBO_GP_GameNode
{
	WORD							wKindID;							//名称索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wNodeID;							//节点索引
	TCHAR							szNodeName[LEN_NODE];				//节点名称
};

//定制类型
struct DBO_GP_GameCustom
{
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wPageID;							//定制索引
	WORD							wOperateType;						//控制类型
	TCHAR							szDisplayName[LEN_CUSTOM];			//定制名称
	TCHAR							szReponseUrl[256];					//跳转地址
};

//房间过滤
struct DBO_GP_GameFilter
{
	WORD							wSortID;							//排序索引
	WORD							wKindID;							//类型索引
	WORD							wNodeID;							//节点索引
	WORD							wServerID;							//房间索引
	WORD							wFilterMask;						//房间状态
	TCHAR							szServerName[LEN_SERVER];			//房间名称
};

//////////////////////////////////////////////////////////////////////////
//返回命令码

#define DBC_REMODIFY_PASSWD			1									//重新修改密码
#define DBC_REMODIFY_ACCOUNTS		2									//重新修改帐号

//////////////////////////////////////////////////////////////////////////

//手机命令

//登录命令
#define	DBR_MB_LOGON_EMAIL			500									//邮箱登录
#define	DBR_MB_LOGON_EMAILCODE		501									//邮箱验证码
#define	DBR_MB_LOGON_VISITOR		502									//游客登录
#define	DBR_MB_LOGON_SECRETKEY		503									//秘钥登录
#define	DBR_MB_LOGON_ACCOUNTS		504									//帐号登录
#define	DBR_MB_LOGON_THIRDPARTY		505									//第三方登录
#define	DBR_MB_LOGON_MOBILECODE		506									//手机验证码
#define	DBR_MB_LOGON_MOBILEPHONE	507									//手机登录
#define DBR_MB_REGISTER_EMAIL		508									//邮箱注册
#define DBR_MB_REGISTER_ACCOUNTS	509									//帐号注册
#define DBR_MB_REGISTER_MOBILEPHONE	510									//手机帐号

//登出命令
#define DBR_MB_LOGOUT_ACCOUNTS		511									//登出账号

//账号服务
#define DBR_MB_LIKE_PLAYER			520									//点赞玩家
#define DBR_MB_MODIFY_LOGON_PASS	522									//修改密码
#define DBR_MB_MODIFY_INDIVIDUAL	523									//修改资料
#define DBR_MB_MODIFY_SYSTEM_FACE	524									//修改头像
#define DBR_MB_MODIFY_CUSTOM_FACE	525									//修改头像
#define DBR_MB_REAL_AUTH			526									//实名认证

//查询命令
#define DBR_MB_QUERY_WEALTH			530									//查询财富
#define DBR_MB_QUERY_RANKINGINFO	531									//查询排位
#define DBR_MB_QUERY_LOVELINESS		532									//查询魅力
#define DBR_MB_QUERY_SUBSIDY		533									//查询低保

//兑换命令
#define DBR_MB_PURCHASE_MALLGOODS	540									//购买商品
#define DBR_MB_EXCHANGE_MALLGOODS	541									//兑换商品
#define DBR_MB_EXCHANGE_GIFTBAGCODE 542									//兑换礼包码

//短信验证
#define DBR_MB_BIND_EMAIL			550									//绑定邮箱
#define DBR_MB_UNBIND_EMAIL			551									//解绑邮箱
#define DBR_MB_BIND_MOBILEPHONE		552									//绑定手机
#define DBR_MB_UNBIND_MOBILEPHONE	553									//解绑手机
#define DBR_MB_RESET_LOGON_PASS		554									//重置密码
#define DBR_MB_ACQUIRE_AUTHCODE		555									//获取验证码

//其他命令
#define DBR_MB_REQUEST_CHECKIN		570									//请求签到
#define DBR_MB_BIND_INVITE_CODE		571									//绑定邀请码
#define DBR_MB_SUBMIT_INFORM_INFO	572									//提交反馈
#define DBR_MB_BIND_EXTERNAL_ACCOUNT 573								//绑定账号
#define DBR_MB_UPDATE_USER_CACHE	574									//更新缓存	

//赠送命令
#define DBR_MB_SEND_GIFT			580									//赠送礼物
#define DBR_MB_LOAD_GIFT			581									//加载礼物
#define DBR_MB_GIFT_STATUS_UPDATE	582									//状态更新

//转账命令
#define DBR_MB_WEALTH_TRANSFER		590									//货币转账

//佣金命令
#define DBR_MB_DRAW_COMMISSION		600									//取出佣金

//转盘命令
#define DBR_MB_QUERY_TURNTABLE_LOTTERY  610								//查询转盘
#define DBR_MB_REQUEST_TURNTABLE_LOTTERY 611							//请求抽奖

//银行命令
#define DBR_MB_INSURE_QUERY			621									//查询银行
#define DBR_MB_INSURE_ENABLE		622									//银行开通
#define DBR_MB_INSURE_SAVE			624									//银行存储
#define	DBR_MB_INSURE_TAKE			625									//银行提取
#define DBR_MB_INSURE_TRANSFER		626									//银行转账
#define DBR_MB_INSURE_RESET_PASSWORD		627							//银行重置密码

//昵称配置
#define DBR_MB_NICKNAME_OPTION		628									//昵称配置
//////////////////////////////////////////////////////////////////////////////////

//游客登录
struct DBR_MB_LogonVisitor
{
	//登录信息
	WORD							wMarketID;							//渠道标识
    DWORD							dwSpreaderID;						//代理ID
    DWORD							dwClientIP;							//连接地址
	DWORD							dwDeviceKind;						//设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//大厅版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szMachineName[LEN_ACCOUNTS];		//机器名字
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本
	TCHAR							szPackageName[LEN_PACKAGE_NAME];	//应用包名
	
	//连接信息
	WORD							wGateID;							//网关标识
	WORD							wLogonID;							//登录标识	
	LPVOID							pTokenParameter;					//绑定参数
};

//秘钥登录
struct DBR_MB_LogonSecretKey
{
	//登录信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szSecretKey[LEN_LOGON_TOKEN];		//登录秘钥

	//附加信息
	WORD							wMarketID;							//渠道标识	
	DWORD							dwClientIP;							//连接地址
	DWORD							dwDeviceKind;						//设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//大厅版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本

	//连接信息
	WORD							wGateID;							//网关标识
	WORD							wLogonID;							//登录标识	
	LPVOID							pTokenParameter;					//绑定参数
};

//帐号登录
struct DBR_MB_LogonAccounts
{
	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号

	//附加信息
	WORD							wMarketID;							//渠道标识	
	DWORD							dwClientIP;							//连接地址
	DWORD							dwDeviceKind;						//设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//大厅版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本

	//连接信息
	WORD							wGateID;							//网关标识
	WORD							wLogonID;							//登录标识	
	LPVOID							pTokenParameter;					//绑定参数
};

//验证码登录
struct DBR_MB_LogonMobileCode
{
	//登录信息
	TCHAR							szAuthCode[LEN_MD5];				//登陆验证码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码

	//附加信息
	WORD							wMarketID;							//渠道标识	
	DWORD							dwClientIP;							//连接地址
	DWORD							dwSpreaderID;						//推广员ID
	DWORD							dwDeviceKind;						//设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//大厅版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列	
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本

	//连接信息
	WORD							wGateID;							//网关标识
	WORD							wLogonID;							//登录标识	
	LPVOID							pTokenParameter;					//绑定参数
};

//帐号登录
struct DBR_MB_LogonThirdParty
{
	//登录信息
	BYTE							cbGender;							//用户性别
    DWORD							dwSpreaderID;						//推广员ID
    DWORD							dwExternalID;						//他方标识	
	TCHAR							szUniqueID[LEN_UNIQUEID];			//用户UID
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szUserDatum[LEN_USER_DATUM];		//用户资料
	TCHAR							szUserFaceUrl[LEN_USER_FACE];		//用户头像
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字	

	//附加信息
	WORD							wMarketID;							//渠道标识	
	DWORD							dwClientIP;							//连接地址
	DWORD							dwDeviceKind;						//设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//大厅版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本
	TCHAR							szPackageName[LEN_PACKAGE_NAME];	//应用包名

	//连接信息
	WORD							wGateID;							//网关标识
	WORD							wLogonID;							//登录标识	
	LPVOID							pTokenParameter;					//绑定参数
};

//手机登录
struct DBR_MB_LogonMobilePhone
{
	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//登录帐号

	//附加信息
	WORD							wMarketID;							//渠道标识	
	DWORD							dwClientIP;							//连接地址
	DWORD							dwDeviceKind;						//设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//大厅版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本

	//连接信息
	WORD							wGateID;							//网关标识
	WORD							wLogonID;							//登录标识	
	LPVOID							pTokenParameter;					//绑定参数
};

//邮箱注册
struct DBR_MB_RegisterEMail
{
	//注册信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	TCHAR							szAuthCode[LEN_MD5];				//登陆验证码
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szEMail[LEN_EMAIL];					//邮箱	
	DWORD							dwSpreaderID;						//代理ID

	//密码变量
	TCHAR							szLogonPass[LEN_MD5];				//登录密码	

	//附加信息
	WORD							wMarketID;							//渠道标识
	DWORD							dwClientIP;							//连接地址
	DWORD							dwDeviceKind;						//设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//大厅版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列	
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本
	TCHAR							szPackageName[LEN_PACKAGE_NAME];	//应用包名

	//连接信息
	WORD							wGateID;							//网关标识
	WORD							wLogonID;							//登录标识	
	LPVOID							pTokenParameter;					//绑定参数
};

//帐号注册
struct DBR_MB_RegisterAccounts
{
	//注册信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	TCHAR							szAccounts[LEN_ACCOUNTS];			//用户账号
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	DWORD							dwSpreaderID;						//代理ID

	//密码变量
	TCHAR							szLogonPass[LEN_MD5];				//登录密码	

	//附加信息
	WORD							wMarketID;							//渠道标识
	DWORD							dwClientIP;							//连接地址
	DWORD							dwDeviceKind;						//设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//大厅版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列	
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本
	TCHAR							szPackageName[LEN_PACKAGE_NAME];	//应用包名

	//连接信息
	WORD							wGateID;							//网关标识
	WORD							wLogonID;							//登录标识	
	LPVOID							pTokenParameter;					//绑定参数
};

//手机注册
struct DBR_MB_RegisterMobilePhone
{
	//注册信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	TCHAR							szAuthCode[LEN_MD5];				//登陆验证码
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码	
	DWORD							dwSpreaderID;						//代理ID

	//密码变量
	TCHAR							szLogonPass[LEN_MD5];				//登录密码	

	//附加信息
	WORD							wMarketID;							//渠道标识
	DWORD							dwClientIP;							//连接地址
	DWORD							dwDeviceKind;						//设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//大厅版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列	
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本
	TCHAR							szPackageName[LEN_PACKAGE_NAME];	//应用包名

	//连接信息
	WORD							wGateID;							//网关标识
	WORD							wLogonID;							//登录标识	
	LPVOID							pTokenParameter;					//绑定参数
};


//邮箱登录
struct DBR_MB_LogonEMail
{
	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szEMail[LEN_EMAIL];					//邮箱

	//附加信息
	WORD							wMarketID;							//渠道标识	
	DWORD							dwClientIP;							//连接地址
	DWORD							dwDeviceKind;						//设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//大厅版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本

	//连接信息
	WORD							wGateID;							//网关标识
	WORD							wLogonID;							//登录标识	
	LPVOID							pTokenParameter;					//绑定参数
};

//验证码登录
struct DBR_MB_LogonEMailCode
{
	//登录信息
	TCHAR							szAuthCode[LEN_MD5];				//登陆验证码
	TCHAR							szEMail[LEN_EMAIL];					//邮箱

	//附加信息
	WORD							wMarketID;							//渠道标识	
	DWORD							dwClientIP;							//连接地址
	DWORD							dwSpreaderID;						//推广员ID
	DWORD							dwDeviceKind;						//设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//大厅版本
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列	
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本

	//连接信息
	WORD							wGateID;							//网关标识
	WORD							wLogonID;							//登录标识	
	LPVOID							pTokenParameter;					//绑定参数
};

//登出账号
struct DBR_MB_LogoutAccounts
{
	WORD							wGateID;							//网关标识
	WORD							wLogonID;							//登录标识
	DWORD							dwSocketID;							//网络标识
	DWORD							dwUserID;							//用户标识
};

//点赞玩家
struct DBR_MB_LikePlayer
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientIP;							//连接地址	
	DWORD							dwDstUserID;						//目标用户	
};

//修改密码
struct DBR_MB_ModifyLogonPass
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientIP;							//连接地址
	TCHAR							szDesPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
};


//修改资料
struct DBR_MB_ModifyIndividual
{
	//验证资料
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientIP;							//连接地址	

	//用户信息
	BYTE							cbGender;							//用户性别
	WORD							wInfoMask;							//信息掩码
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称	
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
};

//修改头像
struct DBR_MB_ModifySystemFace
{
	//用户信息
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户 I D
	
	//机器信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//修改头像
struct DBR_MB_ModifyAccessor
{
	//配件信息
	DWORD							dwUserID;							//用户标识
	WORD							wAccessorMask;						//配件掩码
	WORD							wAccessorValue;						//配件数值

	//机器信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//修改像框
struct DBR_MB_ModifyFaceFrame
{
	//用户信息
	DWORD							dwUserID;							//用户 I D	
	WORD							wFaceFrameID;						//像框标识	

	//机器信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//修改聊天框
struct DBR_MB_ModifyChatFrame
{
	//用户信息
	DWORD							dwUserID;							//用户 I D	
	WORD							wChatFrameID;						//像框标识	

	//机器信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//修改角色
struct DBR_MB_ModifyUserActor
{
	//用户信息
	WORD							wActorID;							//角色标识
	DWORD							dwUserID;							//用户 I D	

	//机器信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//查询财富
struct DBR_MB_QueryWealth
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientIP;							//连接地址
};

//查询排位
struct DBR_MB_QueryRankingInfo
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientIP;							//连接地址
};

//查询魅力
struct DBR_MB_QueryLoveLiness
{
    DWORD							dwUserID;							//用户 I D
};

//查询订单
struct DBR_MB_QueryPayOrders
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientIP;							//连接地址
};

//查询订单
struct DBR_MB_QueryWithdrawOrders
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientIP;							//连接地址
};

//获取验证码
struct DBR_MB_AcquireAuthCode
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//验证信息
	BYTE							cbCodeKind;							//验证码类型
	BYTE							cbAuthKind;							//验证类型
	DWORD							dwBindUserID;						//绑定标识
	TCHAR							szMobileEmail[LEN_EMAIL];			//手机邮件

	//验证码信息
	TCHAR							szAuthCode[LEN_AUTHCODE];			//验证明码
	TCHAR							szAuthCodeMD5[LEN_MD5];				//验证密码

	//连接信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//绑定手机号
struct DBR_MB_BindMobilePhone
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szNewPassword[LEN_PASSWORD];		//用户密码

	//验证信息
	DWORD							dwUnbindUserID;						//解绑标识
	TCHAR							szAuthCode[LEN_MD5];				//验证码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机号码

	//连接信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//解绑定手机号
struct DBR_MB_UnbindMobilePhone
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//验证信息
	TCHAR							szAuthCode[LEN_MD5];				//验证密码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机号码

	//连接信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//绑定邮箱
struct DBR_MB_BindEMail
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szNewPassword[LEN_PASSWORD];		//用户密码

	//验证信息
	DWORD							dwUnbindUserID;						//解绑标识
	TCHAR							szAuthCode[LEN_MD5];				//验证码
	TCHAR							szEMail[LEN_EMAIL];					//邮箱

	//连接信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//解绑定邮箱
struct DBR_MB_UnbindEMail
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//验证信息
	TCHAR							szAuthCode[LEN_MD5];				//验证码
	TCHAR							szEMail[LEN_EMAIL];					//邮箱

	//连接信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//重置密码
struct DBR_MB_ResetLogonPasswd
{
	//验证信息
	DWORD							dwBindUserID;						//绑定标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//验证信息
	BYTE							cbCodeKind;							//验证码类型
	TCHAR							szAuthCode[LEN_MD5];				//验证码
	TCHAR							szMobileEmail[LEN_EMAIL];			//手机邮箱

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//收款账号
struct DBR_MB_ReceiptAccounts
{
	//用户信息
	DWORD							dwUserID;							//绑定标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//账号名称
	TCHAR							szAccountsName[LEN_ACCOUNTS];		//账号名称

	//验证信息
	TCHAR							szAuthCodeMD5[LEN_MD5];				//验证密码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机号码	

	//连接信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//兑换商品
struct DBR_MB_ExchangeMallGoods
{
	DWORD							dwUserID;							//绑定标识
	DWORD							dwMallGoodsID;						//商品标识
};

//兑换礼包码
struct DBR_MB_ExchangeGiftbagCode
{
	DWORD							dwUserID;							//绑定标识
	TCHAR 							szGiftbagCode[16];					//礼包码

	DWORD							dwClientIP;							//连接地址
};

//删除好友
struct DBR_MB_DeleteFriend
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwFriendID;							//好友标识
};

//添加好友
struct DBR_MB_ApplyAddFriend
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwObjectID;							//目标标识
	TCHAR							szApplyDesc[128];					//申请描述
};

//同意添加
struct DBR_MB_AgreeAddFriend
{
	DWORD							dwUserID;							//用户标识	
	DWORD							dwApplyID;							//申请标识
};

//拒绝添加
struct DBR_MB_RefuseAddFriend
{
	DWORD							dwUserID;							//用户标识	
	DWORD							dwApplyID;							//申请标识
	TCHAR							szRefuseDesc[128];					//拒绝描述
};

//信息公开
struct DBR_MB_SetupInfoPublic
{
	DWORD							dwUserID;							//用户标识	
	bool							bInfoPublic;						//公开标志
};

//货币转账
struct DBR_MB_WealthTransfer
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	BYTE							cbCodeKind;							//验证码类型
	TCHAR							szAuthCode[LEN_MD5];				//验证密码

	//转账信息
	DWORD							dwTargetGameID;						//游戏标识
	SCORE							lTransferAmount;					//转出金额

	//连接信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//提取佣金
struct DBR_MB_DrawCommission
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	SCORE							lDrawAmount;						//取出金额

	//连接信息
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//更新缓存
struct DBR_MB_UpdateUserCache
{
	//基础信息
	DWORD							dwUserID;							//用户标识
	WORD							wInfoMask;							//信息掩码

	//游戏信息
	SCORE							lRankScore;							//排位分数
	DWORD							dwLoveliness;						//魅力值
	WORD							wMasterOrder;						//管理等级
	DWORD							dwExperience;						//用户经验
	DWORD							dwMemberPoint;						//会员点数		
};

//绑定邀请码
struct DBR_MB_BindInviteCode
{
    //用户信息
    DWORD							dwUserID;							//用户标识
    TCHAR							szPassword[LEN_PASSWORD];			//用户密码

    //邀请信息
    DWORD							dwInviteCode;						//邀请码
  
    //连接信息
    DWORD							dwClientIP;							//连接地址
    TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//请求签到
struct DBR_MB_RequestCheckIn
{
	DWORD							dwUserID;							//用户标识
	BOOL							bQuery;								//查询标识
	DWORD							dwClientIP;							//连接地址
};

//绑定账号
struct DBR_MB_BindExternalAccount
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	
	//他方信息
	DWORD							dwExternalID;						//他方标识
	TCHAR							szUniqueID[LEN_UNIQUEID];			//用户UUID
	TCHAR							szUserDatum[LEN_USER_DATUM];		//用户资料
	TCHAR							szUserFaceUrl[LEN_USER_FACE];		//头像路径

	//连接信息
	DWORD							dwClientIP;							//连接地址	
};

//提交投诉
struct DBR_MB_SubmitInformInfo
{
    BYTE							cbInformKind;						//投诉类型
    BYTE							cbInformSource;						//投诉来源
	DWORD							dwInformUserID;						//玩家标识	
	DWORD							dwBeInformUserID;					//被投诉玩家
	TCHAR							szAttachmentUrl[128];				//附件地址
    TCHAR							szInformMessage[512];				//投诉消息	
    TCHAR							szInformContent[512];				//投诉内容	

	//连接信息
	DWORD							dwClientIP;							//连接地址	
};

//赠送礼物
struct DBR_MB_SendGift
{
    //用户信息
    DWORD							dwUserID;							//用户标识
    DWORD							dwTargetUserID;						//用户标识

    //物品信息
    DWORD							dwGoodsID;							//商品标识
    DWORD							dwGoodsCount;						//物品数量

    BYTE							cbSendType;							//赠送类型(1:大厅 2:游戏)

    //连接信息
    DWORD							dwClientIP;							//连接地址	

};

//加载礼物
struct DBR_MB_LoadGift
{
    DWORD							dwUserID;							//用户标识
    //DWORD							dwRecordID;							//记录标识
};

//礼物状态更新
struct DBR_MB_GiftStatusUpdata
{
    DWORD							dwUserID;							//用户标识
    DWORD							dwRecordID;							//记录标识
};

//查询转盘抽奖数据
struct DBR_MB_QueryTurntableLottery
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwRecordID;							//记录标识
	DWORD							dwClientIP;							//连接地址	
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器码	
};

//请求抽奖
struct DBR_MB_RequestTurntableLottery
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器码	
};
//实名认证
struct DBR_MB_RealAuth
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//证件号码
};
//银行查询
struct DBR_MB_InsureQuery
{
	DWORD							dwUserID;							//用户 I D
};
//银行开通
struct DBR_MB_InsureEnable
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
};
//存入金币
struct DBR_MB_InsureSave
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lSaveScore;							//存入金币
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	DWORD							dwClientIP;							//连接地址
};

//提取金币
struct DBR_MB_InsureTake
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lTakeScore;							//提取金币
	TCHAR							szPassword[LEN_MD5];				//银行密码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	DWORD							dwClientIP;							//连接地址
};

//转账金币
struct DBR_MB_InsureTransfer
{
	DWORD							dwUserID;							//用户 I D
	SCORE							lTransferScore;						//转账金币
	TCHAR							szPassword[LEN_MD5];				//银行密码
	DWORD							dwGameID;							//目标GameID
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
	DWORD							dwClientIP;							//连接地址
};
//银行密码重置
struct DBR_MB_InsureResetPassword
{
	DWORD							dwUserID;							//用户 I D
	DWORD							dwClientIP;							//连接地址
	TCHAR							szSrcPassword[LEN_PASSWORD];		//原密码
	TCHAR							szDstPassword[LEN_PASSWORD];		//新密码
};
//低保查询
struct DBR_MB_SubsidyQuery
{
	DWORD							dwUserID;							//用户ID
	DWORD							dwClientIP;							//连接地址
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};
//////////////////////////////////////////////////////////////////////////////////

//登录结果
#define DBO_MB_LOGON_SUCCESS		500									//登录成功
#define DBO_MB_LOGON_FAILURE		501									//登录失败
#define DBO_MB_LOGOUT_RESULT		502									//登出结果

//用户服务
#define DBO_MB_USER_FACE_INFO		510									//头像信息
#define DBO_MB_USER_RANKING_INFO	511									//排位信息
#define DBO_MB_USER_ACTION_MASK		512									//行为掩码
#define DBO_MB_USER_CACHE_INFO		514									//缓存信息

//更新事件
#define DBO_MB_USER_PACK_GOODS		520									//背包物品
#define DBO_MB_USER_GOODS_UPDATE	521									//物品更新
#define DBO_MB_USER_WEALTH_UPDATE	522									//财富更新	
#define DBO_MB_USER_LOVELINESS_UPDATE	523								//财富魅力

//验证结果
#define DBO_MB_BINDMP_SUCCESS		530									//绑定成功	
#define DBO_MB_UNBINDMP_SUCCESS		531									//解绑成功
#define DBO_MB_BIND_EMAIL_SUCCESS	532									//绑定成功	
#define DBO_MB_UNBIND_EMAIL_SUCCESS	533									//解绑成功	
#define DBO_MB_ACQUIREAC_RESULT		534									//获取结果

//兑换命令
#define DBO_MB_EXCHANGE_MALLGOODS	540									//兑换商品
#define DBO_MB_EXCHANGE_GIFTBAGCODE 541									//兑换命令

//赠送礼物
#define DBO_MB_SEND_GIFT_SUCCESS	550									//赠送成功
#define DBO_MB_USER_GIFT_INFO		551									//礼物信息
#define DBO_MB_USER_GIFT_INFO_FINISH 552								//礼物信息完成

//其他命令
#define DBO_MB_POST_MESSAGE			560									//
#define DBO_MB_CHECKIN_RESULT		561									//签到结果
#define DBO_MB_BIND_INVITE_CODE		562									//绑定成功
#define DBO_MB_BIND_EXTERNAL_ACCOUNT 563								//绑定成功 

//转盘命令
#define DBO_MB_TURNTABLE_LOTTERY_DATA 570								//转盘数据
#define DBO_MB_TURNTABLE_LOTTERY_RESULT 571								//抽奖结果

//转账命令
#define DBO_MB_WEALTH_TRANSFER_SUCCESS 590								//转账成功

//银行命令
#define DBR_MB_INSURE_QUERY_RESULT	600									//银行查询结果
#define DBR_MB_INSURE_SUCCESS		601									//银行操作成功
#define DBR_MB_INSURE_FAILURE		602									//银行操作失败
#define DBR_MB_INSURE_MAIL_NOTIFY	603									//银行邮件通知

//查询数据
#define DBR_MB_RESULT_SUBSIDY		610									//低保结果

//操作结果
#define DBO_MB_OPERATE_SUCCESS		800									//操作成功
#define DBO_MB_OPERATE_FAILURE		801									//操作失败

//////////////////////////////////////////////////////////////////////////////////

//登录成功
struct DBO_MB_LogonSuccess
{
	//用户属性
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	DWORD							dwCustomID;							//自定头像
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	BYTE							cbInfoPublic;						//信息公开
	DWORD 							dwUserRight;						//用户权限
	TCHAR							szUniqueID[LEN_UNIQUEID];			//用户UID
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
	TCHAR							szInsurePass[LEN_PASSWORD];			//银行密码	
	TCHAR							szLogonToken[LEN_LOGON_TOKEN];		//登录令牌
	

	//用户资料
	TCHAR							szEmail[LEN_EMAIL];					//电子邮箱
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机号码

	//经验等级
	DWORD							dwExperience;						//经验数值

    //魅力值
    DWORD							dwLoveLiness;						//魅力值

	//会员信息
	WORD							wMasterOrder;						//管理等级
	DWORD							dwMemberPoint;						//会员点数
	DWORD							dwMemberOverTime;					//到期时间

	//排位信息
	tagUserRankingInfo				UserRankingInfo;					//排位信息

	//用户财富
	LONGLONG						lUserGold;							//用户金币
	LONGLONG						lUserCard;							//用户房卡
	LONGLONG						lUserInsure;						//用户银行	
	LONGLONG						lCommission;						//用户佣金

	//行为掩码
	DWORD							dwActionMaskEver;					//行为掩码
	DWORD							dwActionMaskPerDay;					//行为掩码
	DWORD							dwActionMaskPerWeek;				//行为掩码

	//锁定信息
	WORD							wLockGateID;						//网关标识
	WORD							wLockLogonID;						//登录标识
	DWORD							dwLockSocketID;						//网络标识
	WORD							wLockKindID;						//类型标识	
	WORD							wLockServerID;						//锁定房间
	WORD							wLockServerKind;					//房间类型	

	//上级信息
	DWORD							dwParentGameID;						//用户 I D
	WORD							wParentFaceID;						//头像ID
	TCHAR							szParentNickName[LEN_ACCOUNTS];		//用户昵称

	//物品信息
	WORD							wGoodsCount;						//物品数量
	tagGoodsHoldInfo				GoodsHoldInfo[128];					//持有信息

	//状态信息
	BYTE							cbShutUpStatus;						//禁言状态
	DWORD							dwShutUpOverTime;					//截止时间

	//签到信息
	WORD							wCheckInDays;						//签到天数
	BYTE							cbCheckInState;						//签到状态

	//实名信息
	tagUserRealAuth					RealAuth;							//实名信息

	//昵称修改信息
	WORD							wNickNameModifyTimes;				//昵称已修改次数
	WORD							wNickNameDailyModifyTimes;			//昵称今日修改次数

	//附加信息				
	BYTE							cbRegisterMode;						//注册模式
	DWORD							dwRegisterTime;						//注册时间
	DWORD							dwLikedNumber;						//被赞数目	
	DWORD							dwUIShieldMask;						//屏蔽掩码	
	TCHAR							szShieldKindList[256];				//类型列表
	TCHAR							szPresentGoodsList[512];			//注册赠送	
	TCHAR                           szUserFaceUrl[LEN_LOGON_TOKEN];     //touxiangdizhi kk jia

	//描述信息
	TCHAR							szDescribeString[128];				//描述消息
};


//登录失败
struct DBO_MB_LogonFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//登出结果
struct DBO_MB_LogoutResult
{
	DWORD							dwUserID;							//用户标识
	LONG							lResultCode;						//结果代码
};

//用户财富
struct DBO_MB_UserWealth
{
	//财富信息
	WORD							wItemCount;							//子项数量
	tagWealthItem					WealthItem[6];						//财富子项
};

//绑定成功
struct DBO_MB_BindMPSuccess
{
	//财富信息
	SCORE							lUserCard;							//房卡数量

	//提示信息
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//绑定号码
	TCHAR							szNoticeString[128];				//提示信息
};

//解绑成功
struct DBO_MB_UnbindMPSuccess
{
	//财富信息
	SCORE							lUserCard;							//房卡数量	

	//提示信息
	TCHAR							szNoticeString[128];				//提示信息
};

//绑定成功
struct DBO_MB_BindEMailSuccess
{
	//财富信息
	SCORE							lUserCard;							//房卡数量

	//提示信息
	TCHAR							szEMail[LEN_EMAIL];					//绑定邮箱
	TCHAR							szNoticeString[128];				//提示信息
};

//解绑成功
struct DBO_MB_UnbindEMailSuccess
{
	//财富信息
	LONGLONG						lUserCard;							//房卡数量

	//提示信息
	TCHAR							szNoticeString[128];				//提示信息
};

//头像信息
struct DBO_MB_UserFaceInfo
{
	WORD							wFaceID;							//头像标识
	DWORD							dwCustomID;							//自定索引
};

//修改头像
struct DBR_MB_ModifyCustomFace
{
	//用户信息
	DWORD							dwUserID;							//用户 I D
	DWORD							dwCustomID;							//自定义标识	
	TCHAR							szFacePath[LEN_USER_FACE];			//头像路径
};

//配件更新
struct DBO_MB_UserAccessorUpdate
{
	WORD							wAccessorMask;						//配件掩码
	WORD							wAccessorValue;						//配件数值
};

//行为掩码
struct DBO_MB_UserActionMask
{
	DWORD							dwActionMaskEver;					//行为掩码
	DWORD							dwActionMaskPerDay;					//行为掩码
	DWORD							dwActionMaskPerWeek;				//行为掩码
};

//获取结果
struct DBO_MB_AcquireACResult
{
	//验证信息
	BYTE							cbCodeKind;							//验证码类型
	BYTE							cbAuthKind;							//验证类型
	DWORD							dwBindUserID;						//绑定标识
	TCHAR							szAuthCode[LEN_AUTHCODE];			//验证码
	TCHAR							szMobileEmail[LEN_MOBILE_PHONE];	//手机邮箱

	//结果信息
	LONG							lResultCode;						//结果代码
	TCHAR							szNoticeString[128];				//提示信息
};

//获取结果
struct DBO_MB_AcquireEMailACResult
{
	//验证信息
	BYTE							cbAuthKind;							//验证类型
	DWORD							dwBindUserID;						//绑定标识
	TCHAR							szAuthCode[LEN_AUTHCODE];			//验证码
	TCHAR							szEMail[LEN_EMAIL];					//邮箱

	//结果信息
	LONG							lResultCode;						//结果代码
	TCHAR							szNoticeString[128];				//提示信息
};

//财富更新
struct DBO_MB_PackGoods
{
	WORD							wGoodsCount;						//子项数量
	tagGoodsHoldInfo				GoodsHoldInfo[128];					//物品信息
};

//物品更新
struct DBO_MB_GoodsUpdate
{
	WORD							wGoodsCount;						//物品数量	
	tagGoodsHoldInfo				GoodsHoldInfo[128];					//物品信息
};

//财富更新
struct DBO_MB_WealthUpdate
{
	WORD							wItemCount;							//子项数量
	tagWealthItem					WealthItem[6];						//财富子项
};

//魅力更新
struct DBO_MB_LoveLinessUpdate
{
	DWORD							dwLoveLiness;						//魅力值
};

//兑换商品
struct DBO_MB_ExchangeMallGoods
{
	WORD							wIncGoodsID;						//物品标识
	DWORD							dwIncGoodsCount;					//增加数量
	DWORD							dwIncGoodsIndate;					//物品有效期
	WORD							wExpendMoneyID;						//货币标识
	SCORE							lMoneyBalance;						//货币余额			
	LONG							lResultCode;						//结果代码
	TCHAR							szDescribeString[128];				//描述消息
};

//兑换商品
struct DBO_MB_ExchangeGiftbagCode
{
	LONG							lResultCode;						//结果代码
	TCHAR							szGiftBagGoodsList[128];			//物品列表		
	TCHAR							szDescribeString[128];				//描述消息
};

//签到结果
struct DBO_MB_CheckInResult
{
	DWORD							dwUserID;							//用户 I D
	WORD							wCheckInDays;						//签到天数
	BYTE							cbCheckInState;						//签到状态	
	TCHAR							szRewardGoodsList[128];				//物品列表		
};

//绑定邀请码成功
struct DBO_MB_BindInviteCode
{
	//上级信息
	DWORD							dwUserID;							//用户 I D
	DWORD							dwParentGameID;						//用户 I D
    WORD							wParentFaceID;						//头像ID
    WORD							wParentFaceFrameID;					//头像框ID
    TCHAR							szParentNickName[LEN_ACCOUNTS];		//用户昵称

	//邮件信息
	DWORD							dwMailID;							//邮件ID

	//提示信息
	TCHAR							szNoticeString[128];				//提示信息
};

//绑定账号成功
struct DBO_MB_BindExternalAccount
{
	//外部信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwExternalID;						//他方标识
	TCHAR							szUniqueID[LEN_UNIQUEID];			//用户UUID

	//提示信息
	TCHAR							szNoticeString[128];				//提示信息
};

//投递消息
struct DBO_MB_PostMessage
{
	//转发信息
	DWORD							dwHashKeyID;						//散列标识
	WORD							wServiceKind;						//服务类型	

	//消息数据
	WORD							wMessageSize;						//数据大小
	BYTE							cbMessageData[1024];				//数据内容
};

//赠送成功
struct DBO_MB_SendGiftSuccess
{
    DWORD							dwRecordID;							//记录ID
    DWORD							dwUserID;							//用户标识
    DWORD							dwTargetUserID;						//用户标识
    DWORD							dwLoveLiness;						//附赠魅力
    DWORD							dwMyLoveLiness;						//自身魅力值
    DWORD							dwTargetLoveLiness;					//对方魅力值

    //物品信息
    DWORD							dwGoodsID;							//商品标识
    DWORD							dwGoodsCount;						//物品数量

	BYTE							cbSendType;							//赠送类型(0:全局赠送 1:私人赠送)
};

//转账成功
struct DBO_MB_WealthTransferSuccess
{
	DWORD							dwMailID;							//邮件标识
	DWORD							dwUserID;							//用户标识
	DWORD							dwTargetUserID;						//目标用户
};

//礼物信息
struct DBO_MB_UserGiftInfo
{
    WORD							wItemCount;							//子项数量
	tagGiftInfo						GiftInfo[3];						//礼物子项
};

//转盘抽奖数据
struct DBO_MB_TurntableLotteryData
{
	//抽奖次数
	WORD							wPayLotteryTimes;					//充值抽奖次数
	WORD							wFreeLotteryTimes;					//免费抽奖次数
	WORD							wGameTimeLotteryTimes;				//时长抽奖次数
	WORD							wGameCountLotteryTimes;				//局数抽奖次数
	WORD							wGameExpendLotteryTimes;			//消耗抽奖次数
	WORD							wUseLotteryTimes;					//已用抽奖次数
	WORD							wResidueLotteryTimes;				//剩余抽奖次数
	WORD							wDailyMayLotteryTimes;				//每日抽奖次数

	//进度数据
	DWORD							dwGameTimeProgress[2];				//游戏时长进度
	DWORD							dwGameCountProgress[2];				//游戏局数进度
	DWORD							dwGameExpendProgress[2];			//游戏消耗进度

	//记录数据
	TCHAR							szTurntableRecord[4096];			//转盘记录
};

//转盘抽奖结果
struct DBO_MB_TurntableLotteryResult
{
	WORD							wHitIndex;							//命中索引
	WORD							wUseLotteryTimes;					//已用抽奖次数
	WORD							wResidueLotteryTimes;				//剩余抽奖次数
};

//操作失败
struct DBO_MB_OperateFailure
{
	LONG							lErrorCode;							//错误代码
	WORD							wRequestCmdID;						//请求命令	
	TCHAR							szDescribeString[128];				//错误消息
};

//操作成功
struct DBO_MB_OperateSuccess
{
	LONG							lResultCode;						//操作代码
	WORD							wRequestCmdID;						//请求命令	
	TCHAR							szDescribeString[128];				//成功消息
};
//银行查询结果
struct DBR_MB_InsureQueryResult
{
	BYTE							cbEnable;							//银行开关
	SCORE							lScore;								//用户金额
	SCORE							lInsureScore;						//银行金额
	tagInsureConfig					SaveAndTake;						//存取配置
	tagInsureConfig					Transfer;							//转账配置
};
//银行成功
struct DBR_MB_InsureSuccess
{
	SCORE							lScore;								//用户金币
	SCORE							lInsureScore;						//银行金币
};

//银行失败
struct DBR_MB_InsureFailure
{
	DWORD							dwResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//错误消息
};
//银行邮件通知
struct DBR_MB_InsureMailNotify
{
	DWORD							dwUserID;							//用户ID
	DWORD							dwMailID;							//邮件ID
};
//低保配置
struct DBR_MB_Subsidy
{
	BYTE							cbDailyTimes;						//每日次数
	BYTE							cbResidualTimes;					//剩余次数
	SCORE							lConsumeScore;						//补助起点
};
//////////////////////////////////////////////////////////////////////////////////


#endif