#ifndef CMD_CLUB_PLAZA_HEAD_FILE
#define CMD_CLUB_PLAZA_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//广场服务
#define MDM_CLUBPLAZA_SERVICE		10									//广场服务

//请求命令
#define SUB_CP_C_ENTER_SERVICE		1									//进入服务
#define SUB_CP_C_SEARCH_CLUB		3									//搜索茶社
#define SUB_CP_C_CREATE_CLUB		4									//创建茶社
#define SUB_CP_C_UPDATE_CLUB		5									//更新茶社
#define SUB_CP_C_DELETE_CLUB		6									//删除茶社
#define SUB_CP_C_INSERT_MEMBER		7									//添加成员
#define SUB_CP_C_UPDATE_MEMBER		8									//修改成员
#define SUB_CP_C_DELETE_MEMBER		9									//删除成员
#define SUB_CP_C_APPLY_REQUEST		10									//申请请求
#define SUB_CP_C_APPLY_RESPOND		11									//申请响应	
#define SUB_CP_C_APPLY_DELETE		12									//申请删除	
#define SUB_CP_C_APPLY_REPEAL		13									//申请撤销
#define SUB_CP_C_SETTLE_BATTLE		14									//约战结算
#define SUB_CP_C_MODIFY_SCORE		15									//修改馆分
#define SUB_CP_C_DETECT_CLUBNAME	17									//检测名称
#define SUB_CP_C_DELETE_RECORD		18									//删除记录

//应答命令
#define SUB_CP_S_LOGON_SUCCESS		100									//登录成功
#define SUB_CP_S_LOGON_FAILURE		101									//登录失败
#define SUB_CP_S_LOGON_FINISH		102									//登录完成
#define SUB_CP_S_CLUB_OPTION		104									//茶社选项
#define SUB_CP_S_CLUB_MEMBER		105									//茶社成员
#define SUB_CP_S_CLUB_NAMECARD		106									//茶社名片
#define SUB_CP_S_CLUB_DELETE		107									//茶社移除
#define SUB_CP_S_APPLY_MESSAGE		109									//申请消息
#define SUB_CP_S_APPLY_DELETE		111									//申请删除
#define SUB_CP_S_SEARCH_RESULT		112									//搜索结果
#define SUB_CP_S_DETECT_RESULT		113									//检测结果

//操作结果
#define SUB_CP_S_OPERATE_SUCCESS	200									//操作成功
#define SUB_CP_S_OPERATE_FAILURE	201									//操作失败	

//////////////////////////////////////////////////////////////////////////////////

//搜索茶社
struct CMD_CP_C_SearchClub
{
	DWORD							dwClubID;							//茶社标识
};

//创建茶社
struct CMD_CP_C_CreateClub
{
	//茶社规则
	BYTE							cbPayType;							//支付类型	
	BYTE							cbCurrencyKind;						//货币类型	
	DWORD							dwSwitchOption;						//开关选项	

	//茶社属性
	WORD							wClubKind;							//茶馆类型
	WORD							wClubIconID;						//图标标识
	TCHAR							szClubName[LEN_CLUB_NAME];			//茶社名称
	TCHAR							szClubIntroduce[LEN_CLUB_INTRODUCE];//茶社介绍
};

//更新茶社
struct CMD_CP_C_UpdateClub
{	
	//茶社信息
	DWORD							dwClubID;							//茶社标识
	BYTE							cbUpdateKind;						//更新类型
};

//删除茶社
struct CMD_CP_C_DeleteClub
{
	//茶社信息
	DWORD							dwClubID;							//茶社标识
};

//删除成员
struct CMD_CP_C_DeleteMember
{
	//茶社信息
	DWORD							dwClubID;							//茶社标识
	DWORD							dwMemberID;							//成员标识
	BYTE							cbDeleteKind;						//删除类型
};

//修改馆员
struct CMD_CP_C_UpdateMember
{
	//茶社信息
	DWORD							dwClubID;							//茶社标识
	DWORD							dwMemberID;							//成员标识
	WORD							wUpdateKind;						//修改类型
};

//申请请求
struct CMD_CP_C_ApplyRequest
{
	DWORD							dwClubID;							//茶社标识
};

//申请响应
struct CMD_CP_C_ApplyRespond
{
	//申请信息
	DWORD							dwClubID;							//茶社标识
	DWORD							dwApplyerID;						//用户标识	
	BYTE							cbApplyStatus;						//申请状态
	DWORD							dwMessageID;						//消息标识
};

//申请删除
struct CMD_CP_C_ApplyDelete
{
	DWORD							dwClubID;							//茶社标识
	DWORD							dwMessageID;						//消息标识
	BYTE							cbMessageType;						//消息类型
};

//申请撤销
struct CMD_CP_C_ApplyRepeal
{
	DWORD							dwMessageID;						//消息标识
};

//申请删除
struct CMD_CP_C_SettleBattle
{
	DWORD							dwDateID;							//时间标识
	DWORD							dwClubID;							//茶社标识
	DWORD							dwMemberID;							//成员标识
	WORD							wSettleCount;						//结算次数
};

//修改馆分
struct CMD_CP_C_ModifyScore
{
	//馆员信息
	DWORD							dwClubID;							//馆主标识
	DWORD							dwMemberID;							//馆员标识
	LONGLONG						lModifyScore;						//变更分数

};

//检测名称
struct CMD_CP_C_DetectClubName
{
	TCHAR							szClubName[LEN_CLUB_NAME];			//茶社名称
};

//删除记录
struct CMD_CP_C_DeleteRecord
{
	DWORD							dwClubID;							//茶社标识
	DWORD							dwRecordID;							//记录标识
	BYTE							cbRecordKind;						//记录类型	
};

////清除馆分
//struct CMD_CP_C_CleanupScore
//{
//	//馆员信息
//	DWORD							dwClubID;							//馆主标识
//	DWORD							dwMemberID;							//馆员标识
//	BYTE							cbCleanupObject;					//清除对象
//	DWORD							dwCreaterGameID;					//游戏标识
//};
//
////清除税收
//struct CMD_CP_C_CleanupRevenue
//{
//	//馆员信息
//	DWORD							dwClubID;							//茶馆标识
//	DWORD							dwMemberID;							//成员标识
//	BYTE							cbCleanupObject;					//清除对象
//	DWORD							dwCreaterGameID;					//游戏标识
//
//};
//
////清理设置
//struct CMD_CP_C_CleanupSetup
//{
//	DWORD							dwClubID;							//茶馆标识
//	bool							bAutoCleanup;						//自动清理		
//};
//
////清除排行榜
//struct CMD_CP_C_CleanupRankList
//{
//	//馆员信息
//	DWORD							dwClubID;							//茶馆标识
//	DWORD							dwMemberID;							//成员标识
//	BYTE							cbListKind;							//榜单类型	
//	BYTE							cbCleanupObject;					//清除对象
//	DWORD							dwCreaterGameID;					//游戏标识
//};

//登陆成功
struct CMD_CP_S_LogonSuccess
{	
};

//登录失败
struct CMD_CP_S_LogonFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};

//茶社配置
struct CMD_CP_S_ClubOption
{
	tagClubOption					ClubOption;							//茶社配置
};

//茶社更新
struct CMD_CP_S_ClubUpdate
{
	DWORD							dwClubID;							//茶社标识
	BYTE							cbUpdateKind;						//更新类型
};

//茶社移除
struct CMD_CP_S_ClubDelete
{
	DWORD							dwClubID;							//茶社标识
	TCHAR							szNotifyContent[128];				//提示信息
};

//成员更新
struct CMD_CP_S_MemberUpdate
{
	DWORD							dwClubID;							//茶社标识
	tagClubMemberInfo				ClubMember;							//成员信息
};

//搜索结果
struct CMD_CP_S_SearchResult
{
	tagClubNameCard					ClubNameCard;						//茶社名片
};

//申请消息
struct CMD_CP_S_ApplyMessage
{
	BYTE							cbMessageType;						//消息类型
	WORD							wMessageCount;						//消息数量
};

//申请删除
struct CMD_CP_S_ApplyDelete
{
	WORD							wMessageCount;						//消息数量
};

//用户财富
struct CMD_CP_S_WealthUpdate
{
	BYTE							cbWealthMask;						//财富掩码
	SCORE							lUserCard;							//用户房卡
	SCORE							lUserClubScore;						//用户茶馆分
};

//检测结果
struct CMD_CP_S_DetectResult
{
	bool							bUsable;							//可用标志
	TCHAR							szErrorDescribe[128];				//错误描述
};

//
////清除馆分
//struct CMD_CP_S_CleanupScore
//{
//	//馆员信息
//	DWORD							dwClubID;							//馆主标识
//	DWORD							dwMemberID;							//馆员标识
//	BYTE							cbCleanupObject;					//清除对象
//};
//
////清除税收
//struct CMD_CP_S_CleanupRevenue
//{
//	//馆员信息
//	DWORD							dwClubID;							//茶馆标识
//	DWORD							dwMemberID;							//成员标识	
//	BYTE							cbCleanupObject;					//清除对象
//};
//
////清理设置
//struct CMD_CP_S_CleanupSetup
//{
//	DWORD							dwClubID;							//茶馆标识
//	bool							bAutoCleanup;						//自动清理	
//};
//
////清除排行榜
//struct CMD_CP_S_CleanupRankList
//{
//	DWORD							dwClubID;							//茶馆标识
//	DWORD							dwMemberID;							//成员标识
//	BYTE							cbListKind;							//榜单类型	
//	BYTE							cbCleanupObject;					//清除对象		
//};
//
////清除合作数据
//struct CMD_CP_S_CleanupCooperate
//{
//	DWORD							dwClubID;							//茶馆标识
//	DWORD							dwParentID;							//上级标识
//	WORD							wMemberType;						//成员类型		
//};

//操作成功
struct CMD_CP_S_OperateSuccess
{
	LONG							lOperateCode;						//操作代码
	TCHAR							szOperateNotice[128];				//操作提示
};

//操作失败
struct CMD_CP_S_OperateFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szErrorDescribe[128];				//错误消息
};

//////////////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif