#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//请求数据包

//用户命令
#define DBR_GR_REQUEST_WEBPAGE		 1 										//请求网页

//邮件命令
#define DBR_GR_USER_QUERY_MAIL		 10										//查询邮件
#define DBR_GR_USER_MODIFY_MAIL		 11										//修改邮件
#define DBR_GR_USER_INSERT_MAIL		 12										//插入邮件
#define DBR_GR_USER_DELETE_MAIL		 13										//删除邮件
#define DBR_GR_CLAIM_MAIL_ATTACH	 14										//领取附件

//////////////////////////////////////////////////////////////////////////

//请求网页
struct DBR_GR_RequestWebPage
{
	emHttpRequest					hrType;									//请求类型
	CHAR							szHeader[128];							//网页头部
	CHAR							szWebUrl[256];							//网页链接	
	CHAR							szPostData[2048];						//投递数据
};


//查询邮件
struct DBR_GR_UserQueryMail
{
	DWORD							dwUserID;							//用户标识	
	DWORD							dwLatestMailID;						//邮件标识
};

//修改邮件
struct DBR_GR_UserModifyMail
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwMailID;							//邮件标识
	BYTE							cbMailState;						//邮件状态
};

//添加邮件
struct DBR_GR_UserInsertMail
{
	DWORD							dwMailID;							//邮件标识
	DWORD							dwUserID;							//用户标识
	SYSTEMTIME						LastLogonTime;						//登录时间
};

//领取附件
struct DBR_GR_ClaimMailAttach
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwMailID;							//邮件标识	
};

//删除邮件
struct DBR_GR_UserDeleteMail
{
	DWORD							dwUserID;							//用户标识
	DWORD							dwMailID;							//邮件标识	
};

//////////////////////////////////////////////////////////////////////////
//输出信息

//邮件事件
#define DBO_GR_USER_MAIL_INFO		110									//邮件信息
#define DBO_GR_USER_MAIL_FINISH		111									//邮件完成
#define DBO_GR_USER_MAIL_UPDATE		112									//邮件更新
#define DBO_GR_USER_MAIL_INSERT		113									//插入邮件
#define DBO_GR_CLAIM_MAIL_ATTACH	114									//领取附件

//更新事件
#define DBO_GR_USER_GOODS_UPDATE	120									//物品更新
#define DBO_GR_USER_WEALTH_UPDATE	121									//财富更新	

//操作结果
#define DBO_GR_OPERATE_SUCCESS		1000								//操作成功
#define DBO_GR_OPERATE_FAILURE		1001								//操作失败

//////////////////////////////////////////////////////////////////////////////////
//邮件列表
struct DBO_GR_UserMailList
{
	DWORD							dwUserID;								//用户标识
};

//邮件信息
struct DBO_GR_UserMailInfo
{
	WORD							wMailCount;							//邮件数量
	tagMailInfo						MailInfo[3];						//邮件信息	
};

//添加邮件
struct DBO_GR_UserMailInsert
{
	DWORD							dwMailID;							//邮件标识
	DWORD							dwUserID;							//用户标识	
};

//邮件更新
struct DBO_GR_UserMailUpdate
{
	DWORD							dwMailID;							//邮件标识
	BYTE							cbMailState;						//邮件状态
};

//领取附件
struct DBO_GR_ClaimMailAttach
{
	DWORD							dwMailID;							//邮件标识		
	TCHAR							szAttachGoods[LEN_MAIL_ATTACH];		//邮件附件

	//结果信息
	LONG							lResultCode;						//结果代码	
};

//财富更新
struct DBO_GR_WealthUpdate
{
	WORD							wItemCount;							//子项数量
	tagWealthItem					WealthItem[6];						//财富子项
};

//物品更新
struct DBO_GR_GoodsUpdate
{
	WORD							wGoodsCount;						//物品数量	
	tagGoodsHoldInfo				GoodsHoldInfo[128];					//物品信息
};

//操作失败
struct DBO_GR_OperateFailure
{
	LONG							lErrorCode;							//错误代码
	WORD							wRequestCmdID;						//请求命令
	TCHAR							szDescribeString[128];				//描述消息
};

//操作成功
struct DBO_GR_OperateSuccess
{
	LONG							lErrorCode;							//操作代码
	WORD							wRequestCmdID;						//请求命令
	TCHAR							szDescribeString[128];				//成功消息
};

//////////////////////////////////////////////////////////////////////////////////

#endif