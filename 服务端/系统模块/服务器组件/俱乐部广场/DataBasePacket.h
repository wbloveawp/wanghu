#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//请求数据包

//系统命令
#define DBR_GR_LOAD_CLUB_OPTION		1									//加载配置
#define DBR_GR_LOAD_FORBID_WORDS	2									//禁止词汇

//群组命令
#define DBR_GR_LOGON_SERVICE		10									//登录服务
#define DBR_GR_CREATE_CLUB			11									//创建群组
#define DBR_GR_UPDATE_CLUB			12									//更新群组
#define DBR_GR_DELETE_CLUB			13									//删除群组
#define DBR_GR_SEARCH_CLUB			14									//查找群组
#define DBR_GR_TRANSFER_CLUB		15									//转让群组	
#define DBR_GR_UPDATE_MEMBER		16									//更新成员
#define DBR_GR_DELETE_MEMBER		17									//删除成员
#define DBR_GR_SETTLE_BATTLE		20									//约战结算
#define DBR_GR_MODIFY_SCORE			21									//修改积分
#define DBR_GR_PRESENT_SCORE		22									//赠送积分
#define DBR_GR_DELETE_RECORD		23									//删除记录
#define DBR_GR_DETECT_CLUBNAME		24									//检测名称
#define DBR_GR_CLEANUP_SCORE		25									//清理馆分
#define DBR_GR_CLEANUP_REVENUE		26									//清理税收
#define DBR_GR_CLEANUP_SETUP		27									//清理设置
#define DBR_GR_CLEANUP_RANKLIST		28									//清理税收

//合并命令
#define DBR_GR_REQUEST_MERGE		30									//请求合并
#define DBR_GR_RESPOND_MERGE		31									//响应合并

//代付命令
#define DBR_GR_CANCEL_PAYROLL		50									//取消代付
#define DBR_GR_REQUEST_PAYROLL		51									//请求代付
#define DBR_GR_RESPOND_PAYROLL		52									//响应代付

//申请命令
#define DBR_GR_APPLY_REQUEST		60									//申请请求
#define DBR_GR_APPLY_RESPOND		61									//申请响应
#define DBR_GR_APPLY_DELETE			62									//申请删除
#define DBR_GR_APPLY_REPEAL			63									//申请撤销

//////////////////////////////////////////////////////////////////////////

//用户登录
struct DBR_GR_LogonService
{
	//用户信息
	DWORD							dwUserID;							//用户标识	
};

//创建群组
struct DBR_GR_CreateClub
{
	//用户信息
	DWORD							dwCreateID;							//群主标识	
	
	//群组属性
	WORD							wClubKind;							//群组级别
	WORD							wClubIconID;						//图标标识
	TCHAR							szClubName[LEN_CLUB_NAME];			//群组名称
	TCHAR							szClubIntroduce[LEN_CLUB_INTRODUCE];//群组介绍	

	//群组规则
	BYTE							cbPayType;							//支付类型
	BYTE							cbCurrencyKind;						//货币类型
	DWORD							dwSwitchOption;						//开关选项
};

//更新群组
struct DBR_GR_UpdateClub
{
	//群组信息
	DWORD							dwClubID;							//群组标识
	DWORD							dwUserID;							//用户标识		
	BYTE							cbUpdateKind;						//更新类型	
};

//删除群组
struct DBR_GR_DeleteClub
{
	//群组信息
	DWORD							dwClubID;							//群组标识
	DWORD							dwCreaterID;						//群主标识		
};

//查找群组
struct DBR_GR_SearchClub
{
	//群组信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//群组标识	
};

//检测名称
struct DBR_GR_DetectClubName
{
	TCHAR							szClubName[LEN_CLUB_NAME];			//茶社名称
};

//转让俱乐部
struct DBR_GR_TransferClub
{
	//群组信息
	DWORD							dwClubID;							//群组标识
	DWORD							dwCreaterID;						//群主标识		

	//转让信息
	DWORD							dwNewCreaterID;						//新老板标识
	TCHAR							szAuthCodeMD5[LEN_MD5];				//验证码
};

//更新成员
struct DBR_GR_UpdateMember
{
	//群组信息
	DWORD							dwClubID;							//群组标识
	DWORD							dwMemberID;							//成员标识

	//用户信息
	DWORD							dwUserID;							//用户标识

	//更新类型
	WORD							wUpdateKind;						//更新类型
};

//删除成员
struct DBR_GR_DeleteMember
{
	//群组信息
	DWORD							dwClubID;							//群组标识
	DWORD							dwMemberID;							//成员标识

	//操作信息
	BYTE							cbDeleteKind;						//删除类型

	//用户信息
	DWORD							dwUserID;							//用户标识	
};

//移除成员
struct DBR_GR_RemoveMember
{
	//群组信息
	DWORD							dwClubID;							//群组标识
	DWORD							dwMemberID;							//成员标识

	//用户信息
	DWORD							dwUserID;							//用户标识	
};

//申请请求
struct DBR_GR_ApplyRequest
{
	DWORD							dwClubID;							//群组标识
	DWORD							dwApplyerID;						//用户标识			
};

//申请响应
struct DBR_GR_ApplyRespond
{
	DWORD							dwMessageID;						//消息标识

	//用户信息
	DWORD							dwUserID;							//用户标识
	
	//申请信息
	DWORD							dwClubID;							//群组标识
	DWORD							dwApplyerID;						//用户标识	
	BYTE							cbApplyStatus;						//申请状态
};

//申请删除
struct DBR_GR_ApplyDelete
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//茶社标识

	DWORD							dwMessageID;						//消息标识
	BYTE							cbMessageType;						//消息类型	
};

//申请撤销
struct DBR_GR_ApplyRepeal
{
	DWORD							dwMessageID;						//消息标识	

	//用户信息
	DWORD							dwUserID;							//用户标识
};

//约战结算
struct DBR_GR_SettleBattle
{		
	DWORD							dwDateID;							//时间标识
	DWORD							dwClubID;							//群组标识
	DWORD							dwMemberID;							//成员标识
	WORD							wSettleCount;						//结算次数

	//用户信息
	DWORD							dwCreaterID;						//用户标识
};

//修改馆分
struct DBR_GR_ModifyScore
{
	//成员信息
	DWORD							dwClubID;							//俱乐部标识
	DWORD							dwMemberID;							//成员标识	
	LONGLONG						lModifyScore;						//变更分数

	//用户信息
	DWORD							dwUserID;							//用户标识
};

//赠送馆分
struct DBR_GR_PresentScore
{
	//成员信息
	DWORD							dwClubID;							//俱乐部标识
	DWORD							dwMemberID;							//目标成员
	LONGLONG						lPresentScore;						//赠送分数

	//用户信息
	DWORD							dwUserID;							//用户标识
};

//删除记录
struct DBR_GR_DeleteRecord
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//茶社标识
	DWORD							dwRecordID;							//记录标识
	BYTE							cbRecordKind;						//记录类型		
};

//清除馆分
struct DBR_GR_CleanupScore
{
	//成员信息
	DWORD							dwClubID;							//俱乐部标识
	DWORD							dwMemberID;							//成员标识
	BYTE							cbCleanupObject;					//清除对象
	DWORD							dwCreaterGameID;					//老板标识	

	//用户信息
	DWORD							dwUserID;							//用户标识
};

//清除税收
struct DBR_GR_CleanupRevenue
{
	//成员信息
	DWORD							dwClubID;							//俱乐部标识
	DWORD							dwMemberID;							//成员标识
	BYTE							cbCleanupObject;					//清除对象
	DWORD							dwCreaterGameID;					//老板标识	

	//用户信息
	DWORD							dwUserID;							//用户标识
};

//清除设置
struct DBR_GR_CleanupSetup
{
	//配置信息
	bool							bAutoCleanup;						//自动清理	

	//用户信息	
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//茶馆标识
};

//清除排行榜
struct DBR_GR_CleanupRankList
{
	//成员信息
	DWORD							dwClubID;							//俱乐部标识
	DWORD							dwMemberID;							//成员标识
	BYTE							cbListKind;							//榜单类型
	BYTE							cbCleanupObject;					//清除对象
	DWORD							dwCreaterGameID;					//游戏标识

	//用户信息
	DWORD							dwUserID;							//用户标识
};

//请求合并
struct DBR_GR_RequestMerge
{
	//用户信息
	DWORD							dwUserID;							//用户标识		
	DWORD							dwRequestClubID;					//俱乐部标识

	//俱乐部信息
	DWORD							dwRespondClubID;					//俱乐部标识
	DWORD							dwCreaterGameID;					//老板标识
};

//响应合并
struct DBR_GR_RespondMerge
{
	//用户信息
	DWORD							dwUserID;							//用户标识		

	//响应信息
	DWORD							dwRecordID;							//记录标识		
	BYTE							cbRespondCode;						//响应代码
	TCHAR							szAuthCodeMD5[LEN_MD5];				//验证码
};

//取消代付
struct DBR_GR_CancelPayroll
{
	//用户信息
	DWORD							dwUserID;							//用户标识		

	//俱乐部信息
	DWORD							dwClubID;							//俱乐部标识
	DWORD							dwMemberID;							//成员标识
};

//请求代付
struct DBR_GR_RequestPayroll
{
	//用户信息
	DWORD							dwUserID;							//用户标识		
	
	//俱乐部信息
	DWORD							dwClubID;							//俱乐部标识
	DWORD							dwMemberID;							//成员标识
};

//响应代付
struct DBR_GR_RespondPayroll
{
	//记录信息
	DWORD							dwRecordID;							//记录标识
	BYTE							cbRespondCode;						//响应代码

	//用户信息
	DWORD							dwUserID;							//用户标识		
};

//处理邮件
struct DBR_GR_HandNoticeMail
{
	//响应信息
	DWORD							dwRecordID;							//记录标识
	BYTE							cbRespondCode;						//响应代码	

	//用户信息
	DWORD							dwUserID;							//用户标识			
};

//////////////////////////////////////////////////////////////////////////

//输出信息

//系统命令
#define DBO_GR_CLUB_OPTION			100									//茶社配置
#define DBO_GR_FIRBID_WORDS			101									//禁止词汇
#define DBO_GR_OPTION_FINISH		102									//配置完成	

//群组命令
#define DBO_GR_LOGON_SUCCESS		110									//登录成功
#define DBO_GR_LOGON_FAILURE		111									//登录失败
#define DBO_GR_LOGON_FINISH			112									//登录完成
#define DBO_GR_CREATE_CLUB			113									//创建群组
#define DBO_GR_UPDATE_CLUB			114									//更新群组
#define DBO_GR_DELETE_CLUB			115									//删除群组
#define DBO_GR_SEARCH_CLUB			116									//查找群组
#define DBO_GR_TRANSFER_CLUB		117									//转让群组	
#define DBO_GR_CLUB_NAMECARD		118									//茶社名片
#define DBO_GR_INSERT_MEMBER		119									//添加成员
#define DBO_GR_UPDATE_MEMBER		120									//添加成员
#define DBO_GR_DELETE_MEMBER		121									//删除成员
#define DBO_GR_MODIFY_SCORE			123									//变更分数
#define DBO_GR_PRESENT_SCORE		124									//赠送分数
#define DBO_GR_DETECT_CLUBNAME		125									//检测名称

//清理命令
#define DBO_GR_CLEANUP_SCORE		130									//清理馆分
#define DBO_GR_CLEANUP_REVENUE		131									//清理税收
#define DBO_GR_CLEANUP_SETUP		132									//清理设置
#define DBO_GR_CLEANUP_RANKLIST		133									//清理税收

//合并命令
#define DBO_GR_REQUEST_MERGE		140									//请求合并	
#define DBO_GR_RESPOND_MERGE		141									//响应合并

//代付命令
#define DBO_GR_CANCEL_PAYROLL		160									//取消代付	
#define DBO_GR_REQUEST_PAYROLL		161									//请求代付
#define DBO_GR_RESPOND_PAYROLL		162									//响应代付

//申请命令
#define DBO_GR_WEALTH_UPDATE		170									//财富更新
#define DBO_GR_APPLY_MESSAGE		171									//申请消息
#define DBO_GR_APPLY_DELETE			172									//删除结果

//操作结果
#define DBO_GR_OPERATE_SUCCESS		200									//操作成功
#define DBO_GR_OPERATE_FAILURE		201									//操作失败	

//////////////////////////////////////////////////////////////////////////////////
//登录成功
struct DBO_GR_LogonSuccess
{
	//用户信息
	DWORD							dwUserID;							//用户标识
};

//登录失败
struct DBO_GR_LogonFailure
{
	//用户信息	
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};

//创建群组
struct DBO_GR_CreateClub
{
	tagClubNameCard					ClubNameCard;						//群组属性	
};

//更新群组
struct DBO_GR_UpdateClub
{
	//群组信息
	DWORD							dwClubID;							//群组标识	
	BYTE							cbUpdateKind;						//更新类型
};

//删除群组
struct DBO_GR_DeleteClub
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//群组标识
	TCHAR							szClubName[LEN_CLUB_NAME];			//群组名次
};

//查找群组
struct DBO_GR_SearchClub
{
	DWORD							dwUserID;							//用户标识
	tagClubNameCard					ClubNameCard;						//茶社名片
};

//升级群组
struct DBO_GR_UpgradeClub
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//群组标识	
};

//转让群组
struct DBO_GR_TransferClub
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//群组标识
	DWORD							dwNewCreaterID;						//新老板标识
};

//添加成员
struct DBO_GR_InsertMember
{
	DWORD							dwClubID;							//群组标识
	tagClubNameCard					ClubNameCard;						//茶社名片
	tagClubMemberUser				ClubMemberUser;						//成员用户	
};

//更新成员
struct DBO_GR_UpdateMember
{
	DWORD							dwClubID;							//群组标识
	WORD							wUpdateKind;						//更新类型
	tagClubMemberInfo				ClubMember;							//群组成员
};

//删除成员
struct DBO_GR_DeleteMember
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//群组标识
	DWORD							dwMemberID;							//成员标识	
	WORD							wMemberCount;						//成员数量
	BYTE							cbDeleteKind;						//删除类型
	TCHAR							szClubName[LEN_CLUB_NAME];			//群组名称
};

//财富更新
struct DBO_GR_WealthUpdate
{
	WORD							wItemCount;							//子项数量
	tagWealthItem					WealthItem[5];						//财富子项
};

//变更分数
struct DBO_GR_ModifyScore
{
	//成员信息
	WORD							wKindID;							//类型标识
	WORD							wServerID;							//房间标识
	DWORD							dwMemberID;							//成员标识
	SCORE							lMemberScore;						//成员分数	
	SCORE							lModifyScore;						//修改分数
	TCHAR							szNickName[LEN_ACCOUNTS];			//成员昵称

	//馆主信息	
	DWORD							dwClubID;							//群组标识	
	DWORD							dwPayUserID;						//支付者标识
	WORD							wPayUserType;						//支付者类型	
	TCHAR							szClubName[LEN_CLUB_NAME];			//茶馆名称
};

//赠送分数
struct DBO_GR_PresentScore
{
	//俱乐部信息
	DWORD							dwClubID;							//群组标识
	TCHAR							szClubName[LEN_CLUB_NAME];			//茶馆名称

	//操作成员		
	DWORD							dwSrcMemberID;						//成员标识
	LONGLONG						lPresentScore;						//赠送积分
	LONGLONG						lSrcMemberScore;					//成员积分		
	TCHAR							szSrcNickName[LEN_ACCOUNTS];		//成员昵称	

	//目标成员
	WORD							wKindID;							//类型标识
	WORD							wServerID;							//房间标识
	DWORD							dwDstMemberID;						//成员标识
	LONGLONG						lDstMemberScore;					//成员积分		
	TCHAR							szDstNickName[LEN_ACCOUNTS];		//成员昵称	
};

//检测结果
struct DBO_GR_DetectClubName
{
	bool							bUsable;							//可用标志
	TCHAR							szErrorDescribe[128];				//错误描述
};

//清除馆分
struct DBO_GR_CleanupScore
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//馆主标识
	DWORD							dwMemberID;							//成员标识
	BYTE							cbCleanupObject;					//清除对象
};

//清除税收
struct DBO_GR_CleanupRevenue
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//茶馆标识
	DWORD							dwMemberID;							//成员标识	
	BYTE							cbCleanupObject;					//清除对象
};

//清理设置
struct DBO_GR_CleanupSetup
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//茶馆标识
	bool							bAutoCleanup;						//自动清理	
};

//清除排行榜
struct DBO_GR_CleanupRankList
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//茶馆标识
	DWORD							dwMemberID;							//成员标识
	BYTE							cbListKind;							//榜单类型	
	BYTE							cbCleanupObject;					//清除对象
};

//清除合作数据
struct DBO_GR_CleanupCooperate
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//茶馆标识
	DWORD							dwParentID;							//上级标识
	WORD							wMemberType;						//成员类型		
};

//请求合并
struct DBO_GR_RequestMerge
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwRequestClubID;					//俱乐部标识	
	TCHAR							szRequestNickName[LEN_ACCOUNTS];	//用户昵称
	TCHAR							szRequestClubName[LEN_CLUB_NAME];	//群组名称	

	//合并信息
	DWORD							dwRecordID;							//记录标识
	DWORD							dwRespondClubID;					//俱乐部标识
	DWORD							dwRespondCreaterID;					//老板标识	
	TCHAR							szRespondClubName[LEN_CLUB_NAME];	//群组名称
};

//响应合并
struct DBO_GR_RespondMerge
{
	//记录信息
	DWORD							dwRecordID;							//记录标识
	BYTE							cbRespondCode;						//响应代码

	//请求信息
	DWORD							dwRequestClubID;					//俱乐部标识
	DWORD							dwRequestCreaterID;					//老板标识	
	
	//响应信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwRespondClubID;					//俱乐部标识
	DWORD							dwRespondCreaterID;					//老板标识
	TCHAR							szRespondClubName[LEN_CLUB_NAME];	//群组名称

	//错误信息
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};

//请求邀请
struct DBO_GR_RequestInvite
{
	//用户信息
	DWORD							dwRequestUserID;					//用户标识
	DWORD							dwRequestClubID;					//俱乐部标识	
	TCHAR							szRequestNickName[LEN_ACCOUNTS];	//用户昵称
	TCHAR							szRequestClubName[LEN_CLUB_NAME];	//群组名称	

	//邀请信息
	DWORD							dwRecordID;							//记录标识
	DWORD							dwRespondUserID;					//用户标识		
};

//响应邀请
struct DBO_GR_RespondInvite
{
	//记录信息
	DWORD							dwRecordID;							//记录标识
	BYTE							cbRespondCode;						//响应代码

	//请求信息
	DWORD							dwRequestUserID;					//用户标识
	DWORD							dwRequestClubID;					//俱乐部标识	

	//响应信息
	DWORD							dwRespondUserID;					//响应标识	
	TCHAR							szRespondNickName[LEN_ACCOUNTS];	//玩家昵称	

	//错误信息
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};


//请求代付
struct DBO_GR_RequestPayroll
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//俱乐部标识	
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szClubName[LEN_CLUB_NAME];			//群组名称	

	//成员信息	
	DWORD							dwMemberID;							//成员标识	

	//记录信息
	DWORD							dwRecordID;							//记录标识
};

//响应代付
struct DBO_GR_RespondPayroll
{
	//记录信息
	DWORD							dwRecordID;							//记录标识
	BYTE							cbRespondCode;						//响应代码

	//请求信息
	DWORD							dwClubID;							//俱乐部标识
	DWORD							dwCreaterID;						//老板标识	
	
	//成员信息
	DWORD							dwUserID;							//用户标识

	//错误信息
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};

//取消代付
struct DBO_GR_CancelPayroll
{
	//用户信息
	DWORD							dwUserID;							//用户标识	
	DWORD							dwClubID;							//俱乐部标识	
	DWORD							dwMemberID;							//成员标识		
};

//申请消息
struct DBO_GR_ApplyMessage
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//俱乐部标识
	BYTE							cbMessageType;						//消息类型
	WORD							wMessageCount;						//消息数量	
};

//合并消息
struct DBO_GR_MergeMessage
{
	DWORD							dwUserID;							//用户标识	
	WORD							wMessageCount;						//消息数量	
};

//删除结果
struct DBO_GR_ApplyDelete
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//茶社标识
	DWORD							dwMessageID;						//消息标识
	BYTE							cbMessageType;						//消息类型	
};

//操作成功
struct DBO_GR_OperateSuccess
{
	//用户信息
	LONG							lOperateCode;						//操作代码
	TCHAR							szOperateNotice[128];				//操作提示
};

//操作失败
struct DBO_GR_OperateFailure
{
	//用户信息
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};

//////////////////////////////////////////////////////////////////////////////////

#endif