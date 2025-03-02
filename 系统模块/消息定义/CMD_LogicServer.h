#ifndef CMD_LOGIC_SERVICE_HEAD_FILE
#define CMD_LOGCI_SERVICE_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//逻辑服务
#define MDM_LOGIC_SERVICE			10									//逻辑服务

//请求命令
#define SUB_LS_C_ENTER_LOGIC		1									//进入服务
#define SUB_LS_C_LEAVE_LOGIC		2									//离开服务
#define SUB_LS_C_SEARCH_CLUB		3									//搜索群组
#define SUB_LS_C_CREATE_CLUB		4									//创建群组
#define SUB_LS_C_UPDATE_CLUB		5									//更新群组
#define SUB_LS_C_DELETE_CLUB		6									//删除群组
#define SUB_LS_C_UPGRADE_CLUB      7									//升级群组
#define SUB_LS_C_TRANSFER_CLUB		8									//转让群组 
#define SUB_LS_C_INSERT_MEMBER		9									//添加成员
#define SUB_LS_C_UPDATE_MEMBER		10									//修改成员
#define SUB_LS_C_DELETE_MEMBER		11									//删除成员
#define SUB_LS_C_REMOVE_MEMBER		12									//移除成员
#define SUB_LS_C_APPLY_REQUEST		13									//申请请求
#define SUB_LS_C_APPLY_RESPOND		14									//申请响应	
#define SUB_LS_C_APPLY_DELETE		15									//申请删除	
#define SUB_LS_C_APPLY_REPEAL		16									//申请撤销
#define SUB_LS_C_SETTLE_BATTLE		17									//约战结算
#define SUB_LS_C_REQUEST_MERGE		18									//请求合并
#define SUB_LS_C_REQUEST_INVITE		19									//请求邀请
#define SUB_LS_C_REQUEST_PAYROLL	20									//请求代付
#define SUB_LS_C_CANCEL_PAYROLL		21									//取消代付
#define SUB_LS_C_MODIFY_SCORE		22									//修改馆分
#define SUB_LS_C_MODIFY_COWRY		23									//修改贝壳
#define SUB_LS_C_MARKER_RECORD		24									//标记记录
#define SUB_LS_C_PRESENT_SCORE		25									//赠送馆分

//清理命令
#define SUB_LS_C_CLEANUP_SCORE		30									//清除馆分
#define SUB_LS_C_CLEANUP_REVENUE	31									//清除税收
#define SUB_LS_C_CLEANUP_SETUP		32									//清理设置
#define SUB_LS_C_CLEANUP_RANKLIST	33									//清除排行
#define SUB_LS_C_CLEANUP_COOPERATE	34									//清除合作

//应答命令
#define SUB_LS_S_LOGON_SUCCESS		100									//登录成功
#define SUB_LS_S_LOGON_FAILURE		101									//登录失败
#define SUB_LS_S_CLUB_OPTION		102									//群组配置
#define SUB_LS_S_CLUB_VIEWLIST		103									//群组列表
#define SUB_LS_S_CLUB_DYNAMIC		104									//群组动态
#define SUB_LS_S_CLUB_NAMECARD		105									//群组名片
#define SUB_LS_S_CLUB_DELETE		106									//群组移除
#define SUB_LS_S_CLUB_MEMBER		107									//群组成员
#define SUB_LS_S_APPLY_MESSAGE		108									//申请消息
#define SUB_LS_S_WEALTH_UPDATE		109	    							//财富更新
#define SUB_LS_S_SEARCH_RESULT		110									//搜索结果
#define SUB_LS_S_APPLY_DELETE		112									//申请删除

//清理命令
#define SUB_LS_S_CLEANUP_SCORE		120									//清理馆分
#define SUB_LS_S_CLEANUP_REVENUE	121									//清理税收
#define SUB_LS_S_CLEANUP_SETUP		122									//清理设置
#define SUB_LS_S_CLEANUP_RANKLIST	123									//清理排行
#define SUB_LS_S_CLEANUP_COOPERATE	124									//清除合作

//操作结果
#define SUB_LS_S_OPERATE_SUCCESS	200									//操作成功
#define SUB_LS_S_OPERATE_FAILURE	201									//操作失败	

//////////////////////////////////////////////////////////////////////////////////

//进入服务
struct CMD_LS_C_EnterLogic
{	
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//离开服务
struct CMD_LS_C_LeaveLogic
{	
	DWORD							dwUserID;							//用户标识	
};

//搜索群组
struct CMD_LS_C_SearchClub
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//群组标识
};

//创建群组
struct CMD_LS_C_CreateClub
{
	////规则信息
	//BYTE							cbPayType;							//支付类型	
	//BYTE							cbCurrencyKind;						//货币类型	
	//BYTE							cbBattleMask;						//约战掩码
	//WORD							wRevenueRate;						//税收比例

	//用户信息
	DWORD							dwCreateID;							//群主标识	
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码

	//群组信息
	WORD							wClubLevel;							//茶馆级别
	TCHAR							szClubName[LEN_CLUB_NAME];			//群组名称
	TCHAR							szClubIntroduce[LEN_CLUB_INTRODUCE];//群组介绍
};

//更新群组
struct CMD_LS_C_UpdateClub
{	
	//群组信息
	DWORD							dwClubID;							//群组标识
	BYTE							cbUpdateKind;						//更新类型

	//用户信息
	DWORD							dwCreaterID;						//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//删除群组
struct CMD_LS_C_DeleteClub
{
	//群组信息
	DWORD							dwClubID;							//群组标识

	//用户信息
	DWORD							dwCreaterID;						//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//升级群组
struct CMD_LS_C_UpgradeClub
{
	//群组信息
	DWORD							dwClubID;							//群组标识	
	WORD							wTargetLevel;						//目标级别

	//用户信息
	DWORD							dwCreaterID;						//用户标识
	DWORD							dwCreaterGameID;					//游戏标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//转让群组
struct CMD_LS_C_TransferClub
{
	//用户信息
	DWORD							dwClubID;							//群组标识		
	DWORD							dwCreaterID;						//馆主标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码

	//转让信息
	DWORD							dwNewCreaterID;						//新馆主标识
	TCHAR							szAuthCodeMD5[LEN_MD5];				//验证码
};

//删除成员
struct CMD_LS_C_DeleteMember
{
	//群组信息
	DWORD							dwClubID;							//群组标识
	DWORD							dwMemberID;							//成员标识
	
	//操作信息
	BYTE							cbDeleteKind;						//删除类型
	
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//移除成员
struct CMD_LS_C_RemoveMember
{
	//群组信息
	DWORD							dwClubID;							//群组标识
	DWORD							dwMemberID;							//成员标识

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//修改馆员
struct CMD_LS_C_UpdateMember
{
	//群组信息
	DWORD							dwClubID;							//群组标识
	DWORD							dwMemberID;							//成员标识

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码

	//更新类型
	WORD							wUpdateKind;						//修改类型
};

//申请请求
struct CMD_LS_C_ApplyRequest
{
	DWORD							dwClubID;							//群组标识
	DWORD							dwApplyerID;						//用户标识		
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//申请响应
struct CMD_LS_C_ApplyRespond
{
	DWORD							dwMessageID;						//消息标识

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码

	//申请信息
	DWORD							dwClubID;							//群组标识
	DWORD							dwApplyerID;						//用户标识	
	BYTE							cbApplyStatus;						//申请状态
};

//申请删除
struct CMD_LS_C_ApplyDelete
{
	DWORD							dwMessageID;						//消息标识
	BYTE							cbMessageType;						//消息类型

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//申请撤销
struct CMD_LS_C_ApplyRepeal
{
	DWORD							dwMessageID;						//消息标识

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//申请删除
struct CMD_LS_C_SettleBattle
{
	DWORD							dwDateID;							//时间标识
	DWORD							dwClubID;							//群组标识
	DWORD							dwMemberID;							//成员标识
	WORD							wSettleCount;						//结算次数

	//用户信息
	DWORD							dwCreaterID;						//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码		
};

//修改馆分
struct CMD_LS_C_ModifyScore
{
	//馆员信息
	DWORD							dwClubID;							//馆主标识
	DWORD							dwMemberID;							//馆员标识
	LONGLONG						lModifyScore;						//变更分数

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码		
};

//修改贝壳
struct CMD_LS_C_ModifyCowry
{
	//馆员信息
	DWORD							dwClubID;							//馆主标识
	DWORD							dwMemberID;							//馆员标识
	LONGLONG						lModifyCowry;						//变更贝壳

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码		
};

//赠送馆分
struct CMD_LS_C_PresentScore
{
	//馆员信息
	DWORD							dwClubID;							//茶馆标识
	DWORD							dwMemberID;							//目标馆员
	LONGLONG						lPresentScore;						//赠送积分

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码		
};

//标记记录
struct CMD_LS_C_MarkerRecord
{
	//标记信息
	BYTE							cbRecordKind;						//记录类型
	DWORD							dwRecordID;							//记录标识
	BYTE							cbMarkValue;						//标记数值
	DWORD							dwClubID;							//茶馆标识

	//用户信息
	DWORD							dwCreaterID;						//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//清除馆分
struct CMD_LS_C_CleanupScore
{
	//馆员信息
	DWORD							dwClubID;							//馆主标识
	DWORD							dwMemberID;							//馆员标识
	BYTE							cbCleanupObject;					//清除对象
	DWORD							dwCreaterGameID;					//游戏标识

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码		
};

//清除税收
struct CMD_LS_C_CleanupRevenue
{
	//馆员信息
	DWORD							dwClubID;							//茶馆标识
	DWORD							dwMemberID;							//成员标识
	BYTE							cbCleanupObject;					//清除对象
	DWORD							dwCreaterGameID;					//游戏标识

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码		
};

//清理设置
struct CMD_LS_C_CleanupSetup
{
	//配置信息
	bool							bAutoCleanup;						//自动清理	

	//用户信息	
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//茶馆标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//清除排行榜
struct CMD_LS_C_CleanupRankList
{
	//馆员信息
	DWORD							dwClubID;							//茶馆标识
	DWORD							dwMemberID;							//成员标识
	BYTE							cbListKind;							//榜单类型	
	BYTE							cbCleanupObject;					//清除对象
	DWORD							dwCreaterGameID;					//游戏标识

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//清除合作数据
struct CMD_LS_C_CleanupCooperate
{
	//馆员信息
	DWORD							dwClubID;							//茶馆标识
	DWORD							dwParentID;							//上级标识
	WORD							wMemberType;						//成员类型	
	DWORD							dwCreaterGameID;					//游戏标识

	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//添加玩法
struct CMD_LS_C_AppendConfig
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码

	//约战配置
	DWORD							dwClubID;							//群组标识
	tagIMBattleConfig				BattleConfig;						//约战玩法
};

//修改玩法
struct CMD_LS_C_ModifyConfig
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码

	//约战配置	
	DWORD							dwClubID;							//群组标识
	tagIMBattleConfig				BattleConfig;						//约战玩法
};

//删除玩法
struct CMD_LS_C_DeleteConfig
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码

	//群组信息
	WORD							wKindID;							//类型标识
	DWORD							dwClubID;							//群组标识
};

//请求合并
struct CMD_LS_C_RequestMerge
{
	//用户信息
	DWORD							dwUserID;							//用户标识	
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码

	//茶馆信息
	DWORD							dwRequestClubID;					//茶馆标识

	//茶馆信息
	DWORD							dwRespondClubID;					//茶馆标识
	DWORD							dwCreaterGameID;					//馆主标识
};

//请求邀请
struct CMD_LS_C_RequestInvite
{
	//用户信息
	DWORD							dwUserID;							//用户标识	
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码

	//茶馆信息
	DWORD							dwRequestClubID;					//茶馆标识

	//邀请信息	
	DWORD							dwParentID;							//上级标识
	DWORD							dwRespondGameID;					//游戏标识
};

//请求代付
struct CMD_LS_C_RequestPayroll
{
	//用户信息
	DWORD							dwUserID;							//用户标识	
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码

	//茶馆信息
	DWORD							dwClubID;							//茶馆标识
	DWORD							dwMemberID;							//成员标识	
};

//取消代付
struct CMD_LS_C_CancelPayroll
{
	//用户信息
	DWORD							dwUserID;							//用户标识	
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码

	//茶馆信息
	DWORD							dwClubID;							//茶馆标识
	DWORD							dwMemberID;							//成员标识	
};

//登陆成功
struct CMD_LS_S_LogonSuccess
{
	//分数信息
	LONGLONG						lClubScore;							//茶馆分数
};

//登录失败
struct CMD_LS_S_LogonFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};

//群组配置
struct CMD_LS_S_ClubOption
{
	tagClubOption					ClubOption;							//群组配置
};

//群组更新
struct CMD_LS_S_ClubUpdate
{
	DWORD							dwClubID;							//群组标识
	BYTE							cbUpdateKind;						//更新类型
};

//群组移除
struct CMD_LS_S_ClubDelete
{
	DWORD							dwClubID;							//群组标识
	TCHAR							szNotifyContent[128];				//提示信息
};

//成员更新
struct CMD_LS_S_MemberUpdate
{
	DWORD							dwClubID;							//群组标识
	tagClubMember					ClubMember;							//成员信息
};

//搜索结果
struct CMD_LS_S_SearchResult
{
	WORD							wClubCount;							//群组数量
};

//申请消息
struct CMD_LS_S_ApplyMessage
{
	BYTE							cbMessageType;						//消息类型
	WORD							wMessageCount;						//消息数量
};

//用户财富
struct CMD_LS_S_WealthUpdate
{
	BYTE							cbWealthMask;						//财富掩码
	LONGLONG						lUserIngot;							//用户钻石
	LONGLONG						lUserRoomCard;						//用户房卡
	LONGLONG						lUserClubScore;						//用户茶馆分
};

//申请删除
struct CMD_LS_S_ApplyDelete
{
	DWORD							dwMessageID;						//消息标识
	BYTE							cbMessageType;						//消息类型
	bool							bDelResult;							//删除结果
	TCHAR							szErrorDescribe[128];				//错误消息
};


//清除馆分
struct CMD_LS_S_CleanupScore
{
	//馆员信息
	DWORD							dwClubID;							//馆主标识
	DWORD							dwMemberID;							//馆员标识
	BYTE							cbCleanupObject;					//清除对象
};

//清除税收
struct CMD_LS_S_CleanupRevenue
{
	//馆员信息
	DWORD							dwClubID;							//茶馆标识
	DWORD							dwMemberID;							//成员标识	
	BYTE							cbCleanupObject;					//清除对象
};

//清理设置
struct CMD_LS_S_CleanupSetup
{
	DWORD							dwClubID;							//茶馆标识
	bool							bAutoCleanup;						//自动清理	
};

//清除排行榜
struct CMD_LS_S_CleanupRankList
{
	DWORD							dwClubID;							//茶馆标识
	DWORD							dwMemberID;							//成员标识
	BYTE							cbListKind;							//榜单类型	
	BYTE							cbCleanupObject;					//清除对象		
};

//清除合作数据
struct CMD_LS_S_CleanupCooperate
{
	DWORD							dwClubID;							//茶馆标识
	DWORD							dwParentID;							//上级标识
	WORD							wMemberType;						//成员类型		
};

//群组动态
struct CMD_LS_S_ClubDynamic
{
	DWORD							dwClubID;							//群组标识
	tagClubDynamic					ClubDynamic;						//动态信息
};

//操作成功
struct CMD_LS_S_OperateSuccess
{
	LONG							lOperateCode;						//操作代码
	TCHAR							szOperateNotice[128];				//操作提示
};

//操作失败
struct CMD_LS_S_OperateFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};

//////////////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif