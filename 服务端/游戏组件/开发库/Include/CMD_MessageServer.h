#ifndef CMD_MESSAGE_SERVER_HEAD_FILE
#define CMD_MESSAGE_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//命令定义

//消息命令
#define MDM_AN_MESSAGE_SERVICE		10									//消息命令

//////////////////////////////////////////////////////////////////////////
//请求命令
#define SUB_AN_C_ENTER_MESSAGE      1									//进入服务
#define SUB_AN_C_LEAVE_MESSAGE	 	2									//离开服务

//邮件请求
#define SUB_AN_C_USER_QUERY_MAIL	10									//查询邮件
#define SUB_AN_C_USER_VIEW_MAIL		11									//查看邮件
#define SUB_AN_C_USER_DELETE_MAIL	13									//删除邮件
#define SUB_AN_C_CLAIM_MAIL_ATTACH	12									//领取附件								

//服务命令
#define SUB_AN_S_ENTER_SUCCESS		90									//进入成功

//应答命令
#define SUB_AN_S_MATCH_REMIND		100									//比赛提醒
#define SUB_AN_S_MARQUEE_NOTICE		101									//公告内容

//邮件应答
#define SUB_AN_S_USER_MAIL_INFO		110									//邮件信息
#define SUB_AN_S_USER_MAIL_FINISH	111									//邮件完成
#define SUB_AN_S_USER_MAIL_APPEND	112									//邮件添加
#define SUB_AN_S_USER_MAIL_UPDATE	113									//邮件更新
#define SUB_AN_S_USER_MAIL_NOTIFY	114									//邮件通知

//////////////////////////////////////////////////////////////////////////

//查询邮件
struct CMD_AN_C_UserQueryMail
{
	DWORD							dwLatestMailID;						//最新邮件
};

//查看邮件
struct CMD_AN_C_UserViewMail
{
	DWORD							dwMailID;							//邮件标识
};

//删除邮件
struct CMD_AN_C_UserDeleteMail
{
	DWORD							dwMailID;							//邮件标识
};

//领取邮件
struct CMD_AN_C_ClaimMailAttach
{
	DWORD							dwMailID;							//邮件标识
};

//比赛提醒
struct CMD_AN_S_MatchRemind
{
	//比赛信息	
	DWORD							dwMatchID;							//比赛标识
	LONGLONG						lMatchNo;							//比赛场次
	DWORD							dwSurplusTime;						//剩余时间
	TCHAR							szMatchName[32];					//比赛名称
	SYSTEMTIME						MatchStartTime;						//比赛时间

	//房间信息
	WORD							wKindID;							//类型标识
	WORD							wServerID;							//房间标识
	WORD							wServerPort;						//房间端口
	DWORD							dwServerAddr;						//房间地址	
	TCHAR							szServerDomain[LEN_DOMAIN];			//房间域名
};

//邮件信息
struct CMD_AN_S_UserMailInfo
{
	WORD							wMailCount;							//邮件数量
	tagMailInfo						MailInfo[0];						//邮件信息	
};

//邮件添加
struct CMD_AN_S_UserMailAppend
{
	tagMailInfo						MailInfo;							//邮件信息	
};

//邮件更新
struct CMD_AN_S_UserMailUpdate
{
	DWORD							dwMailID;							//邮件标识
	BYTE							cbMailState;						//邮件状态	
};

//邮件通知
struct CMD_AN_S_UserMailNotify
{
	DWORD							dwMailID;							//邮件标识	
};

////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif