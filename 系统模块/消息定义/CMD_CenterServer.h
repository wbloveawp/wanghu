#ifndef CMD_CENTER_HEAD_FILE
#define CMD_CENTER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//注册命令

#define MDM_CS_REGISTER				1									//服务注册

//////////////////////////////////////////////////////////////////////////

#define SUB_CS_C_REGISTER_AGENT		1									//注册代理
#define SUB_CS_C_REGISTER_SERVER	2									//注册游戏
#define SUB_CS_C_REGISTER_MASTER	3									//注册后台
#define SUB_CS_C_REGISTER_SERVICE	10									//注册服务		

//////////////////////////////////////////////////////////////////////////

#define SUB_CS_S_REGISTER_SUCCESS	99									//注册成功
#define SUB_CS_S_REGISTER_FAILURE	100									//注册失败

//////////////////////////////////////////////////////////////////////////
//注册服务
struct CMD_CS_C_RegisterService
{
	WORD							wServiceID;							//服务标识
	WORD							wServiceModule;						//服务模块
	WORD							wServiceMask;						//服务掩码
	WORD							wServicePort;						//服务端口
	WORD							wMaxConnect;						//连接数量
	WORD							wConnectCount;						//当前连接
	WORD							wServiceKindID;						//服务类型
	DWORD							dwServiceAddr;						//服务地址
	TCHAR							szServiceName[LEN_SERVER];			//服务名称
	TCHAR							szServiceDomain[LEN_DOMAIN];		//服务域名
};

//注册代理
struct CMD_CS_C_RegisterAgent
{
	WORD							wAgentID;							//代理标识
	WORD							wAgentKind;							//代理类型
	WORD							wMaxConnect;						//连接数量
	DWORD							dwServiceAddr;						//服务地址
	TCHAR							szServiceDomain[LEN_DOMAIN];		//服务域名		
};

//注册后台
struct CMD_CS_C_RegisterMaster
{
	WORD							wServiceID;							//服务标识		
};

//注册游戏
struct CMD_CS_C_RegisterServer
{
	WORD							wModuleID;							//模块ID
	WORD							wKindID;							//名称索引
	WORD							wNodeID;							//节点索引
	WORD							wSortID;							//排序索引
	WORD							wVisibleMask;						//可视规则
	WORD							wServerID;							//房间索引
	WORD							wServerPort;						//房间端口
	WORD							wServerType;						//房间类型
	WORD							wServerLevel;						//房间等级
	WORD							wChairCount;						//椅子数量		
	BYTE							cbMatchType;						//比赛类型
	WORD							wTableCount;						//桌子数目
	DWORD							dwMaxPlayer;						//最大人数
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwServerRule;						//房间规则
	DWORD							dwServerAddr;						//服务地址
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	TCHAR							szServerDomain[LEN_DOMAIN];			//服务域名

	//分数配置
	SCORE 							lCellScore;							//单位积分	
	SCORE 							lMinEnterScore;						//最低进入
	SCORE 							lMaxEnterScore;						//最高进入
};

//注册失败
struct CMD_CS_S_RegisterFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//错误消息
};

//////////////////////////////////////////////////////////////////////////
//服务信息

#define MDM_CS_COMMON_SERVICE		2									//通用服务

//////////////////////////////////////////////////////////////////////////

//房间信息
#define SUB_CS_C_SERVER_ONLINE		1									//房间人数
#define SUB_CS_C_SERVER_MODIFY		2									//房间修改

//服务命令
#define SUB_CS_C_USER_BUGLE			5									//用户喇叭
#define SUB_CS_C_SERVICE_ONLINE		6									//服务在线
#define SUB_CS_C_REPORT_JACKPOT		7									//上报彩金

//令牌服务
#define SUB_CS_C_LOGOUT_TOKEN		10									//注销令牌
#define SUB_CS_C_REGISTER_TOKEN		11									//注册令牌

//路由命令
#define SUB_CS_C_QUERY_ROUTE_ITEM	20									//查询路由
#define SUB_CS_C_ACTIVE_ROUTE_ITEM	21									//激活路由
#define SUB_CS_C_UPDATE_ROUTE_ITEM	22									//更新路由
#define SUB_CS_C_REMOVE_ROUTE_ITEM	23									//移除路由
#define SUB_CS_C_APPEND_ROUTE_ITEM	24									//添加路由

//////////////////////////////////////////////////////////////////////////

//房间命令
#define SUB_CS_S_SERVER_INFO		110									//房间信息
#define SUB_CS_S_SERVER_INSERT		111									//房间列表
#define SUB_CS_S_SERVER_MODIFY		112									//房间修改
#define SUB_CS_S_SERVER_REMOVE		113									//房间删除
#define SUB_CS_S_SERVER_FINISH		114									//房间完成

//服务命令
#define SUB_CS_S_SERVICE_INFO		200									//服务信息
#define SUB_CS_S_SERVICE_INSERT		201									//服务插入
#define SUB_CS_S_SERVICE_REMOVE		202									//服务删除
#define SUB_CS_S_SERVICE_FINISH		203									//服务完成
#define SUB_CS_S_SERVICE_ONLINE		204									//服务在线
#define SUB_CS_S_SERVICE_ATTRIB		205									//服务属性

//路由命令
#define SUB_CS_S_APPEND_ROUTE_ITEM	210									//添加路由
#define SUB_CS_S_REMOVE_ROUTE_ITEM	211									//移除路由
#define SUB_CS_S_ACTIVE_ROUTE_ITEM	212									//激活路由
#define SUB_CS_S_QUERY_ROUTE_RESULT	216									//查询结果

//服务
#define SUB_CS_S_GAME_JACKPOT		301									//游戏彩池
#define SUB_CS_S_SERVICE_CONFIG		300									//服务配置

//令牌服务
#define SUB_CS_S_REGISTER_TOKEN		500									//注册结果
#define SUB_CS_S_TOKEN_PARAMETER	501									//令牌参数

//应答命令
#define SUB_CS_S_SHUTDOWN_SOCKET	1000								//关闭网络

//////////////////////////////////////////////////////////////////////////
//注册结果
#define REGISTER_RESULT_SUCCESS		0									//注册成功	
#define REGISTER_RESULT_FAILURE		1									//注册失败	
#define REGISTER_RESULT_LINKFULL	2									//链接已满

//////////////////////////////////////////////////////////////////////////

//房间数据
struct CMD_CS_C_ServerModify
{
	WORD							wSortID;							//排序索引
	WORD							wKindID;							//类型索引
	WORD							wNodeID;							//节点索引
	WORD							wVisibleMask;						//可视规则
	WORD							wServerPort;						//房间端口
	WORD							wServerType;						//房间类型
	WORD							wServerLevel;						//房间等级
	DWORD							dwMaxPlayer;						//最大人数
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwServerAddr;						//服务地址
	DWORD							dwServerRule;						//房间规则
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	TCHAR							szServerDomain[LEN_DOMAIN];			//服务域名

	//分数配置
	SCORE 							lCellScore;							//单位积分	
	SCORE 							lMinEnterScore;						//最低进入
	SCORE 							lMaxEnterScore;						//最高进入
};

//服务在线人数
struct CMD_CS_C_ServiceOnline
{
	WORD							wOnlineCount;						//在线人数	
};

//奖池彩金
struct CMD_CS_C_ReportJackpot
{
	WORD							wKindID;							//类型标识
	SCORE							lJackpotScore;						//奖池分数
};

//查询路由
struct CMD_CS_C_QueryRouteItem
{
	DWORD							dwTokenID;							//令牌标识
	DWORD							dwRouteID;							//路由标识
	WORD							wServiceKind;						//服务类型
	WORD							wServiceModule;						//服务模块
};

//激活路由
struct CMD_CS_C_ActiveRouteItem
{
	DWORD							dwRouteID;							//路由标识
	WORD							wServiceKind;						//服务类型
	WORD							wServiceModule;						//服务模块
};

//添加路由
struct CMD_CS_C_AppendRouteItem
{
	bool							bTheLast;							//结束标识
	WORD							wServiceKind;						//服务类型
	WORD							wServiceModule;						//服务模块
};

//更新路由
struct CMD_CS_C_UpdateRouteItem
{
	DWORD							dwRouteID;							//路由标识
	BYTE							cbRouteState;						//路由状态
};

//移除路由
struct CMD_CS_C_RemoveRouteItem
{
	DWORD							dwRouteID;							//路由标识	
};

//注销令牌
struct CMD_CS_C_LogoutToken
{
	DWORD							dwTokenID;							//令牌标识	
};

//注册令牌
struct CMD_CS_C_RegisterToken
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwTokenID;							//令牌标识
	DWORD							dwClientIP;							//终端地址
};

//关闭网络
struct CMD_CS_C_ShutdownSocket
{
	bool							bForceClose;						//强制关闭
};

//////////////////////////////////////////////////////////////////////////

//房间人数
struct CMD_CS_S_ServiceOnline
{
	WORD							wServiceID;							//房间标识
	WORD							wServiceModule;						//服务模块
	WORD							wOnlineCount;						//在线人数
};

//游戏奖池
struct CMD_CS_S_GameJackpot
{
	WORD							wKindID;							//类型标识
	SCORE							lJackpotScore;						//奖池分数
};

//房间修改
struct CMD_CS_S_ServerModify
{
	WORD							wSortID;							//排序索引
	WORD							wKindID;							//类型标识
	WORD							wNodeID;							//节点索引
	WORD							wVisibleMask;						//可视规则
	WORD							wServerID;							//房间标识
	WORD							wServerPort;						//房间端口
	WORD							wServerType;						//房间类型
	WORD							wServerLevel;						//房间等级
	DWORD							dwServerRule;						//房间规则
	DWORD							dwMasterRule;						//管理规则
	DWORD							dwMaxPlayer;						//最大人数
	DWORD							dwOnLineCount;						//在线人数
	DWORD							dwServerAddr;						//服务地址
	TCHAR							szServerName[LEN_SERVER];			//房间名称
	TCHAR							szServerDomain[LEN_DOMAIN];			//房间域名

	//分数配置
	SCORE 							lCellScore;							//单位积分	
	SCORE 							lMinEnterScore;						//最低进入
	SCORE 							lMaxEnterScore;						//最高进入
};

//服务删除
struct CMD_CS_S_ServiceRemove
{
	WORD							wServiceID;							//服务标识
	WORD							wServicePort;						//服务端口
	WORD							wServiceModule;						//服务模块	
};

//激活路由
struct CMD_CS_S_ActiveRouteItem
{
	DWORD							dwRouteID;							//路由标识
};

//添加路由
struct CMD_CS_S_AppendRouteItem
{
	bool							bTheFirst;							//开始标识
	WORD							wServiceID;							//服务标识
	WORD							wServiceModule;						//服务模块		
};

//移除路由
struct CMD_CS_S_RemoveRouteItem
{
	DWORD 							dwRouteID;							//路由标识
	WORD							wServiceID;							//服务标识
	WORD							wServiceModule;						//服务模块
};

//查询结果
struct CMD_CS_S_QueryRouteResult
{
	DWORD							dwTokenID;							//令牌标识
	DWORD							dwRouteID;							//路由标识
	WORD							wServiceID;							//服务标识
	WORD							wServiceModule;						//服务模块
};

//注册结果
struct CMD_CS_S_RegisterToken
{
	LONG							lResultCode;						//结果代码
}; 

//令牌参数
struct CMD_CS_S_TokenParameter
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szLogonToken[LEN_LOGON_TOKEN];		//登录令牌
};

//////////////////////////////////////////////////////////////////////////
//用户汇总

#define MDM_CS_USER_COLLECT			3									//用户汇总

//////////////////////////////////////////////////////////////////////////

//请求命令
#define SUB_CS_C_USER_ONLINE		1									//用户上线
#define SUB_CS_C_USER_OFFLINE		2									//用户离线
#define SUB_CS_C_ENTER_SERVER		3									//用户进房
#define SUB_CS_C_LEAVE_SERVER		4									//用户退房
#define SUB_CS_C_ENTER_MESSAGE		5									//用户消息
#define SUB_CS_C_LEAVE_MESSAGE		6									//用户消息
#define SUB_CS_C_ENTER_CLUBPLAZA	7									//用户消息
#define SUB_CS_C_LEAVE_CLUBPLAZA	8									//用户消息
#define SUB_CS_C_USER_UPDATE		9									//用户更新
#define SUB_CS_C_COLLECT_USER		20									//汇总用户
#define SUB_CS_C_COLLECT_FINISH		21									//汇总完成

//应答命令
#define SUB_CS_S_USER_LIST			100									//用户列表
#define SUB_CS_S_COLLECT_USER		100									//汇总用户
#define SUB_CS_S_COLLECT_FINISH		101									//汇总完成

//////////////////////////////////////////////////////////////////////////

//用户上线
struct CMD_CS_C_UserOnline
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	BYTE							cbGender;							//用户性别
	TCHAR							szNickName[LEN_NICKNAME];			//登录帐号
	TCHAR							szUserHeadUrl[256];			//玩家昵称	kk jia

	//用户头像
	WORD							wFaceID;							//头像标识
	DWORD							dwCustomID;							//头像标识 

	//状态信息
	BYTE							cbShutUpStatus;						//禁言状态
	DWORD							dwShutUpOverTime;					//解禁时间	

	//网络信息
	WORD							wGateID;							//网关标识
	DWORD							dwTokenID;							//令牌标识
};

//用户离线
struct CMD_CS_C_UserOffline
{
	DWORD							dwUserID;							//用户标识
};

//用户更新
struct CMD_CS_C_UserUpdate
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwInfoMask;							//信息掩码
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
	TCHAR							szUserHeadUrl[256];			//玩家昵称	kk jia

	//头像信息
	WORD							wFaceID;							//头像标识
	DWORD							dwCustomID;							//头像标识
};

//用户进入
struct CMD_CS_C_EnterServer
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	BYTE							cbGender;							//用户性别
	WORD							wFaceID;							//头像标识
	DWORD							dwCustomID;							//头像标识
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称
};

//用户离开
struct CMD_CS_C_LeaveServer
{
	DWORD							dwUserID;							//用户标识
};

//用户进入
struct CMD_CS_C_EnterMessage
{
	//用户信息
	DWORD							dwUserID;							//用户标识	
};

//用户离开
struct CMD_CS_C_LeaveMessage
{
	DWORD							dwUserID;							//用户标识
};

//用户进入
struct CMD_CS_C_EnterClubPlaza
{
	//用户信息
	DWORD							dwUserID;							//用户标识	
};

//用户离开
struct CMD_CS_C_LeaveClubPlaza
{
	DWORD							dwUserID;							//用户标识
};

//////////////////////////////////////////////////////////////////////////
//传输命令

#define MDM_CS_TRANSFERS			4									//传输命令

//////////////////////////////////////////////////////////////////////////
//查询命令

#define MDM_CS_QUERY_SERVICE		5									//查询命令

#define SUB_CS_C_QUERY_BY_GAMEID	1									//查询用户
#define SUB_CS_C_QUERY_BY_ACCOUNTS	2									//查询用户

#define SUB_CS_S_QUERY_USER_RESULT	200									//查询结果
#define SUB_CS_S_QUERY_NOT_FOUND	201									//查询不到

//////////////////////////////////////////////////////////////////////////
//查询用户
struct CMD_CS_C_QueryByGameID
{
	DWORD							dwUserID;							//用户ID
	DWORD							dwQueryGameID;						//游戏ID
};

//查询用户
struct CMD_CS_C_QueryByAccounts
{
	DWORD							dwUserID;							//用户ID
	TCHAR							szQueryAccounts[LEN_ACCOUNTS];		//用户帐户
};

//查询结果
struct CMD_CS_S_QueryUserResult
{
	DWORD							dwSendUserID;						//查询用户
	DWORD							dwUserID;							//用户标识
	DWORD							dwGameID;							//游戏标识
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
};

//查询不到
struct CMD_CS_S_QueryNotFound
{
	DWORD							dwSendUserID;						//查询用户
};

//////////////////////////////////////////////////////////////////////////
//广播命令

#define MDM_CS_BROADCAST			6									//广播消息

#define SUB_CS_C_GAME_BROADCAST		1									//游戏房间

//////////////////////////////////////////////////////////////////////////

//后台命令
#define MDM_CS_MASTER_SERVICE		7									//后台服务

//房间管理
#define SUB_CS_C_MODIFY_SERVER		1									//修改房间
#define SUB_CS_C_COLLECT_SERVER		2									//收集房间

//用户管理
#define SUB_CS_C_UNLOCK_PLAYER		10									//解锁玩家
#define SUB_CS_C_KICKOUT_PLAYER		11									//踢出玩家
#define SUB_CS_C_USER_MAIL_NOTIFY	12									//邮件通知
#define SUB_CS_C_USER_MAIL_INSERT	13									//邮件插入
#define SUB_CS_C_SHUTDOWN_ACCOUNT	14									//关闭账号
#define SUB_CS_C_USER_INFO_UPDATE	16									//信息更新
#define SUB_CS_C_MODIFY_SHUTUP_STATUS 15								//更新状态

//配置管理
#define SUB_CS_C_RELOAD_GAME_CONFIG  30									//重载配置
#define SUB_CS_C_APPEND_FILTER_WORDS 35									//添加词汇
#define SUB_CS_C_DELETE_FILTER_WORDS 36									//删除词汇

//黑白名单
#define SUB_CS_C_APPEND_USER_CONTROL 40									//添加控制
#define SUB_CS_C_REMOVE_USER_CONTROL 41									//移除控制

//公告命令
#define SUB_CS_C_PUBLISH_MARQUEE_NOTICE 50								//发布公告

//财富通知
#define SUB_CS_C_WEALTH_UPDATE      60									//财富更新
#define SUB_CS_C_RECHARGE_NOTICE	61									//充值通知

//库存命令
#define SUB_CS_C_APPEND_STOCK_ITEM	70									//添加库存
#define SUB_CS_C_DELETE_STOCK_ITEM	71									//删除库存
#define SUB_CS_C_MODIFY_STOCK_ITEM	72									//修改库存
#define SUB_CS_C_ADJUST_STOCK_SCORE 73									//调整库存值
#define SUB_CS_C_ADJUST_JACKPOT_SCORE 74								//调整彩金
#define SUB_CS_C_UPDATE_SPLAYER_CONTROL 75								//大R控制

//房间信息
#define SUB_CS_S_SERVER_ITEM_LIST	100									//房间列表
#define SUB_CS_S_SERVER_ITEM_FINISH	101									//房间完成

//操作结果
#define SUB_CS_S_OPERATE_RESULT		1000								//操作结果

//////////////////////////////////////////////////////////////////////////

//修改房间
struct CMD_CS_C_ModifyServer
{
	WORD							wServerID;							//房间标识
	DWORD							dwAppendRule;						//添加规则
	DWORD							dwRemoveRule;						//移除规则

	//操作信息
	TCHAR							szOperateID[33];					//操作标识
};

//解锁玩家
struct CMD_CS_C_UnlockPlayer
{
	DWORD							dwUserID;							//用户标识	
};

//踢出玩家
struct CMD_CS_C_KickoutPlayer
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szNotifyContent[128];				//提示内容
};

//插入邮件
struct CMD_CS_C_UserMailInsert
{
	//邮件信息
	DWORD							dwMailID;							//邮件标识
	DWORD							dwUserID;							//用户标识

	//筛选条件
	SYSTEMTIME						LastLogonTime;						//登录时间	
};

//关闭账号
struct CMD_CS_C_ShutDownAccount
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szNotifyContent[128];				//提示内容
};

//修改状态
struct CMD_CS_C_ModifyShutUpStatus
{
	DWORD							dwUserID;							//用户标识
	BYTE							cbShutUpStatus;						//禁言状态
	DWORD							dwShutUpOverTime;					//解禁时间
};

//信息更新
struct CMD_CS_C_UserInfoUpdate
{
	DWORD							dwUserID;							//用户标识
	WORD							wInfoMask;							//信息掩码
	DWORD							dwInfoValue;						//信息数值
};

//重载配置
struct CMD_CS_C_ReloadGameConfig
{
	DWORD							dwConfigMask;						//配置掩码
	DWORD							dwServiceMask;						//配置掩码
};

//添加词汇
struct CMD_CS_C_AppendFilterWords
{
	TCHAR							szFilterWords[128];					//过滤词汇
};

//删除词汇
struct CMD_CS_C_DeleteFilterWords
{
	TCHAR							szFilterWords[128];					//过滤词汇
};

//添加控制
struct CMD_CS_C_AppendUserControl
{
	DWORD							dwUserID;							//用户标识
};

//移除控制
struct CMD_CS_C_RemoveUserControl
{
	DWORD							dwUserID;							//用户标识
};

//发布跑马灯公告
struct CMD_CS_C_PublishMarqueeNotice
{
	DWORD							dwNoticeID;							//公告标识
	bool							bExclusive;							//独占标识
	WORD							wViewPlace;							//显示位置	
	DWORD							dwEndRollTime;						//结束时间	
	DWORD							dwStartRollTime;					//开始时间	
	TCHAR							szNoticeContent[256];				//公告内容
};

//财富更新
struct CMD_CS_C_WealthUpdate
{
	DWORD							dwUserID;							//用户标识	
};

//充值通知
struct CMD_CS_C_RechargeNotice
{
	//行为掩码
	bool							bUpdateMask;						//更新标识
	DWORD							dwActionMaskEver;					//行为掩码
	DWORD							dwActionMaskPerDay;					//行为掩码
	DWORD							dwActionMaskPerWeek;				//行为掩码

	//物品信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szGoodsList[1024];					//物品列表
};

//提现通知 
struct CMD_CS_C_WithdrawNotice
{
	DWORD							dwUserID;							//提现通知
	SCORE							lWithdrawAmount;					//提现金额
	TCHAR							szNickName[LEN_NICKNAME];			//玩家昵称
};

//添加库存
struct CMD_CS_C_AppendStockItem
{
	WORD							wStockID;							//库存标识
	TCHAR							szOperateID[33];					//操作标识
};

//删除库存
struct CMD_CS_C_DeleteStockItem
{
	WORD							wStockID;							//库存标识
	TCHAR							szOperateID[33];					//操作标识
};

//修改库存值
struct CMD_CS_C_ModifyStockItem
{
	WORD							wStockID;							//库存标识
	TCHAR							szOperateID[33];					//操作标识
};

//调整库存值
struct CMD_CS_C_AdjustStockScore
{
	WORD							wStockID;							//库存标识
	SCORE							lChangeScore;						//修改金额
	TCHAR							szOperateID[33];					//操作标识
};

//调整彩金
struct CMD_CS_C_AdjustJackpotScore
{
	WORD							wStockID;							//库存标识	
	WORD							wLevelID;							//彩池级别
	SCORE							lChangeScore;						//修改金额
	TCHAR							szOperateID[33];					//操作标识
};

//调整彩金
struct CMD_CS_C_UpdateSPlayerControl
{
	TCHAR							szOperateID[33];					//操作标识
};

//操作结果
struct CMD_CS_S_OperateResult
{
	//请求信息
	WORD							wRequestCmdID;						//请求命令
	TCHAR							szOperateID[33];					//操作标识


	//结果信息
	LONG							lResultCode;						//结果代码
	TCHAR							szErrorDescribe[128];				//错误描述
};

//////////////////////////////////////////////////////////////////////////////////
//管理服务
#define MDM_CS_MANAGER_SERVICE		10									//管理服务

#define SUB_CS_C_MODIFY_OPTION		1									//修改选项
#define SUB_CS_C_EXECUTE_COMMAND	2									//执行命令

#define SUB_CS_S_MODIFY_OPTION		100									//修改选项
#define SUB_CS_S_EXECUTE_COMMAND	101									//执行命令

//////////////////////////////////////////////////////////////////////////////////
// 
//修改规则
struct CMD_CS_C_ModifyOption
{
	tagModifyOption					ModifyOption;						//编辑选项
	WORD							wServerCount;						//房间数量
	WORD							wServerIDList[512];					//房间列表
};

//////////////////////////////////////////////////////////////////////////////////
//转发服务
#define MDM_CS_FORWARD_SERVICE		12									//转发服务

//////////////////////////////////////////////////////////////////////////////////

//对象定义
#define FORWARD_TARGET_USER			1									//转发对象
#define FORWARD_TARGET_SERVICE		2									//服务对象

//服务子项
struct tagForwardItem
{
	WORD							wServiceID;							//服务标识
	WORD							wExcludeID;							//排除标识
	DWORD							dwHashKeyID;						//散列标识
	WORD							wServiceKind;						//服务类型	
	WORD							wServiceModule;						//服务模块
};

//转发头部
struct tagForwardHead
{
	WORD							wTarget;							//转发目标
	DWORD							dwUserID;							//用户标识
	WORD							wItemCount;							//服务数量
	tagForwardItem					ForwardItem[3];						//转发子项
};

//////////////////////////////////////////////////////////////////////////////////

//禁言消息
#define SUB_CS_USER_SHUTUP_STATUS   1									//禁言状态

//消息命令
#define SUB_CS_POST_MESSAGE			10									//投递消息

//邮件命令
#define SUB_CS_MAIL_INSERT			20									//插入邮件
#define SUB_CS_MAIL_NOTIFY			21									//邮件通知

//比赛消息
#define SUB_CS_MATCH_REMIND			30									//比赛提醒
#define SUB_CS_MATCH_WINAWARD		31									//比赛获奖

//短信邮件
#define SUB_CS_SEND_AUTHCODE		40									//发送验证码
#define SUB_CS_SEND_STOCK_ALARM		41									//发送库存预警
#define SUB_CS_SEND_EXCEPTION_ALARM 42									//发送异常预警

//更新命令
#define SUB_CS_UPDATE_ACTION_MASK	51									//更新掩码

//通知命令
#define SUB_CS_FILTER_WORDS_UPDATE	55									//词汇更新
#define SUB_CS_RANKING_LIST_UPDATE	57									//榜单更新

//管理命令
#define SUB_CS_LOGOUT_NOTIFY		60									//登出提醒

//跑马灯公告
#define SUB_CS_MARQUEE_NOTICE		70									//跑马灯公告

//黑白名单
#define SUB_CS_USER_CONTROL_APPEND	80									//添加控制
#define SUB_CS_USER_CONTROL_REMOVE	81									//移除控制

//财富信息
#define SUB_CS_USER_WEALTH_UPDATE   90									//财富更新
#define SUB_CS_USER_RECHARGE_NOTICE 91									//充值通知

//库存命令
#define SUB_CS_APPEND_STOCK_ITEM	100									//添加库存
#define SUB_CS_DELETE_STOCK_ITEM	101									//删除库存
#define SUB_CS_MODIFY_STOCK_ITEM	102									//修改库存
#define SUB_CS_ADJUST_STOCK_SCORE	104									//修改库存值
#define SUB_CS_ADJUST_JACKPOT_SCORE 105									//修改彩金值
#define SUB_CS_UPDATE_SPLAYER_CONTROL 106								//更新大R控制

//机器命令
#define SUB_CS_APPEND_ROBOT_PARAMETER 110								//添加参数
#define SUB_CS_MODIFY_ROBOT_PARAMETER 111								//修改参数
#define SUB_CS_DELETE_ROBOT_PARAMETER 112								//删除参数	

///////////////////////////////////////////////////////////////////////////////////
// 
//聊天消息

//用户状态
#define SUB_CS_IM_USER_ONLINE		200									//用户上线
#define SUB_CS_IM_USER_OFFLINE		201									//用户下线

///////////////////////////////////////////////////////////////////////////////////
//茶社消息
 
//用户状态
#define SUB_CS_CB_USER_ONLINE		300									//用户上线
#define SUB_CS_CB_USER_OFFLINE		301									//用户状态
#define SUB_CS_CB_USER_UPDATE		302									//用户更新

//茶社命令
#define SUB_CS_CB_UPDATE_CLUB		310									//修改茶社
#define SUB_CS_CB_DELETE_CLUB		311									//删除茶社

//清理命令
#define SUB_CS_CB_CLEANUP_SCORE		320									//清理命令
#define SUB_CS_CB_CLEANUP_REVENUE	321									//清理税收

//申请命令
#define SUB_CS_CB_APPLY_MESSAGE		330									//申请消息
#define SUB_CS_CB_APPLY_DELETE		331									//删除消息

//成员命令
#define SUB_CS_CB_INSERT_MEMBER		340									//添加成员
#define SUB_CS_CB_UPDATE_MEMBER		341									//更新成员
#define SUB_CS_CB_DELETE_MEMBER		342									//删除成员

//茶馆积分
#define SUB_CS_CB_MODIFY_SCORE		350									//修改积分

//茶馆代付
#define SUB_CS_CB_UPDATE_PAYROLL	360									//更新代付

//////////////////////////////////////////////////////////////////////////////////

//禁言状态
struct CMD_CS_UserShutUpStatus
{
	DWORD							dwUserID;							//用户标识	
	BYTE							cbShutUpStatus;						//禁言状态
	DWORD							dwShutUpOverTime;					//解禁时间	
};

//投递消息
struct CMD_CS_PostMessage
{
	DWORD							dwHashKeyID;						//散列标识	
};

//插入邮件
struct CMD_CS_MailInsert
{
	DWORD							dwMailID;							//邮件标识
	DWORD							dwUserID;							//用户标识
	SYSTEMTIME						LastLogonTime;						//登录时间
};

//邮件通知
struct CMD_CS_MailNotify
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwMailID;							//邮件标识
};

//比赛提醒
struct CMD_CS_MatchRemind
{
	//用户信息
	DWORD							dwUserID;							//用户标识

	//比赛信息
	DWORD							dwMatchID;							//比赛标识
	SCORE						lMatchNo;							//比赛场次
	DWORD							dwSurplusTime;						//剩余时间
	WCHAR							szMatchName[32];					//比赛名称	
	SYSTEMTIME						MatchStartTime;						//比赛时间

	//房间信息
	WORD							wKindID;							//类型标识
	WORD							wServerID;							//房间标识
	WORD							wServerPort;						//房间端口
	DWORD							dwServerAddr;						//房间地址	
	WCHAR							szServerDomain[LEN_DOMAIN];			//房间域名

};

//比赛获奖
struct CMD_CS_MatchWinAward
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	WCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称

	//比赛信息
	WCHAR							szMatchName[32];					//比赛名称

	//奖励信息
	WORD							wRankID;							//名次标识	
	WCHAR							szRewardEntity[128];				//实物奖励
	WCHAR							szRewardGoods[256];					//物品奖励
};

//发送验证码
struct CMD_CS_SendAuthCode
{
	BYTE						    cbCodeKind;							//验证码类型
	BYTE						    cbAuthKind;							//验证类型
	WCHAR							szAuthCode[LEN_AUTHCODE];			//手机验证码
	WCHAR							szMobileEmail[LEN_EMAIL];			//手机邮箱
};

//库存预警
struct CMD_CS_SendStockAlarm
{
	SCORE						dAlarmScore;						//预警库存	
	SCORE						dCurrentScore;						//当前库存
	TCHAR							szStockName[32];					//库存名称
};

//崩溃警告
struct CMD_CS_SendCrashAlarm
{
	WORD							wServiceID;							//服务标识
	WORD							wServiceModule;						//服务模块
	TCHAR							szServiceName[32];					//服务名称
};

//异常警告
struct CMD_CS_SendExceptionAlarm
{
	CHAR							szAlarmTitle[64];					//预警标题
	TCHAR							szServiceName[32];					//服务名称
	TCHAR							szExceptionInfo[512];				//异常信息
};

////更新缓存
//struct CMD_CS_UpdateUserCache
//{
//	//基础信息
//	DWORD							dwUserID;							//用户标识
//	WORD							wInfoMask;							//信息掩码
//
//	//游戏信息
//	SCORE							lRankScore;							//排位分数
//	DWORD							dwLoveliness;						//魅力值
//	WORD							wMasterOrder;						//管理等级
//	DWORD							dwExperience;						//用户经验
//	DWORD							dwMemberPoint;						//会员点数		
//};

//更新掩码
struct CMD_CS_UpdateActionMask
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwActionMaskEver;					//行为掩码
	DWORD							dwActionMaskPerDay;					//行为掩码
	DWORD							dwActionMaskPerWeek;				//行为掩码
};

//词汇更新
struct CMD_CS_FilterWordsUpdate
{
	bool							bDeleted;							//添加标识
	TCHAR							szFilterWords[128];					//过滤词汇
};

//登出提醒
struct CMD_CS_LogoutNotify
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szNotifyContent[128];				//提示内容
};

//跑马灯公告
struct CMD_CS_MarqueeNotice
{
	DWORD							dwNoticeID;							//公告标识
	BYTE							cbNoticeKind;						//公告类型
	bool							bExclusive;							//独占标识
	BYTE							cbPriority;							//优先级别		
	WORD							wViewPlace;							//显示位置
	WORD							wRollTimes;							//滚动次数
	DWORD							dwEndRollTime;						//结束时间	
	DWORD							dwStartRollTime;					//开始时间	
	WORD							wNoticeSize;						//公告大小
	BYTE							cbNoticeContent[512];				//公告内容
};

//控制添加
struct CMD_CS_UserControlAppend
{
	DWORD							dwUserID;							//用户标识
};

//控制移除
struct CMD_CS_UserControlRemove
{
	DWORD							dwUserID;							//用户标识
};


//财富更新
struct CMD_CS_UserWealthUpdate
{
	DWORD							dwUserID;							//用户标识
};

//充值通知
struct CMD_CS_UserRechargeNotice
{
	DWORD							dwUserID;							//用户标识
	TCHAR							szGoodsList[1024];					//物品iebook
};

//提现通知
struct CMD_CS_UserWithdrawNotice
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwWithdrawTime;						//提现时间
	SCORE							lWithdrawAmount;					//提现金额
	TCHAR							szNickName[LEN_NICKNAME];			//玩家昵称
	
};

//添加库存
struct CMD_CS_AppendStockItem
{
	WORD							wStockID;							//库存标识
	TCHAR							szOperateID[33];					//操作标识
};

//删除库存
struct CMD_CS_DeleteStockItem
{
	WORD							wStockID;							//库存标识
	TCHAR							szOperateID[33];					//操作标识
};

//修改库存
struct CMD_CS_ModifyStockItem
{
	WORD							wStockID;							//库存标识
	TCHAR							szOperateID[33];					//操作标识
};

//调整库存值
struct CMD_CS_AdjustStockScore
{
	WORD							wStockID;							//库存标识
	SCORE							lChangeScore;						//变化分数
	TCHAR							szOperateID[33];					//操作标识
};

//调整彩金
struct CMD_CS_AdjustJackpotScore
{
	WORD							wStockID;							//库存标识	
	WORD							wLevelID;							//彩池级别
	SCORE							lChangeScore;						//变化金额
	TCHAR							szOperateID[33];					//操作标识
};

//添加参数
struct CMD_CS_AppendRobotParameter
{
	tagRobotParameter				RobotParameter;						//机器参数
};

//修改参数
struct CMD_CS_ModifyRobotParameter
{
	tagRobotParameter				RobotParameter;						//机器参数
};

//删除参数
struct CMD_CS_DeleteRobotParameter
{
	DWORD							dwBatchID;							//批次标识
};

//////////////////////////////////////////////////////////////////////////////////
// 
//用户下线
struct CMD_CS_IM_UserOffline
{
	DWORD							dwUserID;							//用户标识	
};

//////////////////////////////////////////////////////////////////////////////////

//用户下线
struct CMD_CS_CB_UserOffline
{
	DWORD							dwUserID;							//用户标识	
};

//更新茶社
struct CMD_CS_CB_UpdateClub
{
	//茶社信息
	DWORD							dwClubID;							//茶社标识	
	BYTE							cbUpdateKind;						//更新类型	
};

//删除茶社
struct CMD_CS_CB_DeleteClub
{
	DWORD							dwClubID;							//茶社标识
};

//清理积分
struct CMD_CS_CB_CleanupScore
{
	DWORD							dwClubID;							//馆主标识
	DWORD							dwMemberID;							//馆员标识
	BYTE							cbCleanupObject;					//清除对象
};

//清理税收
struct CMD_CS_CB_CleanupRevenue
{
	DWORD							dwClubID;							//馆主标识
	DWORD							dwMemberID;							//馆员标识
	BYTE							cbCleanupObject;					//清除对象
};

//申请消息
struct CMD_CS_CB_ApplyMessage
{
	DWORD							dwClubID;							//茶社标识
	WORD							wMessageCount;						//消息数量
};

//申请删除
struct CMD_CS_CB_ApplyDelete
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//茶社标识	
	WORD							wMessageCount;						//消息数量
};

//添加成员
struct CMD_CS_CB_InsertMember
{
	DWORD							dwClubID;							//茶社标识
	WORD							wMemberCount;						//成员数量
	tagClubMemberUser				MemberUser;							//茶社成员
};

//更新成员
struct CMD_CS_CB_UpdateMember
{
	DWORD							dwClubID;							//茶社标识
	WORD							wUpdateKind;						//更新类型
	tagClubMemberInfo				ClubMember;							//茶社成员
};

//删除成员
struct CMD_CS_CB_DeleteMember
{
	DWORD							dwClubID;							//茶社标识
	DWORD							dwMemberID;							//成员标识
	WORD							wMemberCount;						//成员数量
};

//修改分数
struct CMD_CS_CB_ModifyScore
{
	//游戏信息
	WORD							wKindID;							//类型标识
	WORD							wServerID;							//房间标识

	//成员信息
	DWORD							dwMemberID;							//馆员标识
	SCORE							lMemberScore;						//馆员分数
	SCORE							lModifyScore;						//修改积分

	//用户信息
	DWORD							dwClubID;							//茶社标识	
	DWORD							dwPayUserID;						//支付者标识		
};

//更新代付
struct CMD_CS_CB_UpdatePayroll
{
	DWORD							dwClubID;							//茶馆标识
	DWORD							dwPayrollerID;						//代付者标识
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif