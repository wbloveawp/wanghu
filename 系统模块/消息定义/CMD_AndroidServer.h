#ifndef CMD_LOGIC_SERVICE_HEAD_FILE
#define CMD_LOGCI_SERVICE_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//机器服务
#define MDM_IM_ANDROID_SERVICE		10									//机器服务

//////////////////////////////////////////////////////////////////////////////////

//请求命令
#define SUB_AS_C_LOGON_SERVICE		1									//登陆服务
#define SUB_AS_C_SEARCH_GROUP		2									//查找俱乐部
#define SUB_AS_C_LOAD_GROUP_BATCH	3									//加载批次
#define SUB_AS_C_LOAD_GROUP_MEMBER	4									//群组成员
#define SUB_AS_C_LOAD_BATCH_MEMBER	5									//批次成员

//批次命令
#define SUB_AS_C_APPEND_BATCH		10									//添加批次
#define SUB_AS_C_MODIFY_BATCH		11									//修改批次
#define SUB_AS_C_DELETE_BATCH		12									//删除批次

//机器命令
#define SUB_AS_C_APPEND_ANDROID		20									//添加机器
#define SUB_AS_C_MODIFY_ANDROID		21									//修改机器
#define SUB_AS_C_DELETE_ANDROID		22									//删除机器

//行为模板
#define SUB_AS_C_APPEND_ATEMPLATE	30									//添加模板
#define SUB_AS_C_MODIFY_ATEMPLATE	31									//修改模板
#define SUB_AS_C_DELETE_ATEMPLATE	32									//删除模板

//补给模板
#define SUB_AS_C_APPEND_STEMPLATE	40									//添加模板
#define SUB_AS_C_MODIFY_STEMPLATE	41									//修改模板
#define SUB_AS_C_DELETE_STEMPLATE	42									//删除模板

//////////////////////////////////////////////////////////////////////////////////

//登陆结果
#define SUB_AS_S_LOGON_SUCCESS		100									//登录成功
#define SUB_AS_S_LOGON_FAILURE		101									//登录失败
#define SUB_AS_S_LOGON_FINISH		102									//登录完成

//子项信息
#define SUB_AS_S_GROUP_ITEM			110									//群组子项
#define SUB_AS_S_BATCH_ITEM			111									//批次子项
#define SUB_AS_S_ATEMPLATE_ITEM		112									//模板子项
#define SUB_AS_S_STEMPLATE_ITEM		113									//模板子项
#define SUB_AS_S_GROUP_MEMBER_ITEM	114									//成员子项
#define SUB_AS_S_BATCH_MEMBER_ITEM	115									//成员子项

//子项列表
#define SUB_AS_S_KIND_LIST			120									//类型列表
#define SUB_AS_S_GROUP_LIST			121									//茶馆列表
#define SUB_AS_S_BATCH_LIST			122									//机器批次
#define SUB_AS_S_ATEMPLATE_LIST		124									//行为模板
#define SUB_AS_S_STEMPLATE_LIST		125									//补给模板	
#define SUB_AS_S_GROUP_MEMBER_LIST	126									//群组成员
#define SUB_AS_S_BATCH_MEMBER_LIST	127									//批次成员

//删除通知
#define SUB_AS_S_BATCH_DELETE		130									//批次删除
#define SUB_AS_S_ATEMPLATE_DELETE	131									//模板删除
#define SUB_AS_S_STEMPLATE_DELETE	132									//模板删除
#define SUB_AS_S_BATCH_MEMBER_DELETE 133								//成员删除
#define SUB_AS_S_GROUP_MEMBER_DELETE 134								//成员删除

//操作结果
#define SUB_AS_S_OPERATE_SUCCESS	200									//操作成功
#define SUB_AS_S_OPERATE_FAILURE	201									//操作失败

//////////////////////////////////////////////////////////////////////////////////
//批次修改类型

#define BATCH_MKIND_MODIFY_CONFIG	1									//修改配置
#define BATCH_MKIND_APPEND_MEMBER	2									//添加成员
#define BATCH_MKIND_MODIFY_MEMBER	3									//修改成员
#define BATCH_MKIND_DELETE_MEMBER	4									//删除成员

//修改属性
struct tagBatchModifyConfig
{
	DWORD							dwBatchID;							//批次标识
	DWORD							dwEnterTime;						//进入时间
	DWORD							dwLeaveTime;						//离开时间	
};

//添加机器
struct tagBatchAppendMember
{
	DWORD							dwBatchID;							//批次标识
	DWORD							dwUserID;							//机器标识	
	DWORD							dwEnterTime;						//进入时间
	DWORD							dwLeaveTime;						//离开时间
	WORD							wPlayKindID[3];						//陪玩类型
	DWORD							dwSupplyTemplateID;					//补给模板
	DWORD							dwActionTemplateID[3];				//行为模板	
};

//修改机器
struct tagBatchModifyMember
{
	DWORD							dwBatchID;							//批次标识
	DWORD							dwUserID;							//机器标识	
	DWORD							dwEnterTime;						//进入时间
	DWORD							dwLeaveTime;						//离开时间
	WORD							wPlayKindID[3];						//陪玩类型
	DWORD							dwSupplyTemplateID;					//补给模板
	DWORD							dwActionTemplateID[3];				//行为模板	
};

//删除机器
struct tagBatchDeleteMember
{
	DWORD							dwBatchID;							//批次标识
	DWORD							dwUserID;							//机器标识		
};

//////////////////////////////////////////////////////////////////////////////////

//登陆服务
struct CMD_AS_C_LogonService
{	
	//账号信息
	WCHAR							szAccounts[LEN_ACCOUNTS];			//用户账号
	WCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
};

//查找群组
struct CMD_AS_C_SearchGroup
{
	DWORD							dwGroupID;							//群组标识
};

//群组批次
struct CMD_AS_C_LoadGroupBatch
{
	DWORD							dwGroupID;							//群组标识
};

//加载群组成员
struct CMD_AS_C_LoadGroupMember
{
	DWORD							dwGroupID;							//群组标识
};

//加载批次成员
struct CMD_AS_C_LoadBatchMember
{
	DWORD							dwBatchID;							//批次标识
};

//添加批次
struct CMD_AS_C_AppendBatch
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwEnterTime;						//进入时间
	DWORD							dwLeaveTime;						//离开时间
};

//修改批次
struct CMD_AS_C_ModifyBatch
{
	DWORD							dwBatchID;							//批次标识
	BYTE							cbModifyKind;						//修改类型
};

//删除批次
struct CMD_AS_C_DeleteBatch
{
	DWORD							dwBatchID;							//群组标识
};

//添加机器
struct CMD_AS_C_AppendAndroid
{
	DWORD							dwGroupID;							//群组标识
	BYTE							cbMemberType;						//成员类型
	DWORD							dwParentGameID;						//上级标识
	WORD							wAndroidCount;						//机器数量
};

//修改机器
struct CMD_AS_C_ModifyAndroid
{
	//账号信息
	DWORD							dwUserID;							//机器标识	
	DWORD							dwGroupID;							//群组标识
	BYTE							cbGender;							//用户性别	
};

//修改机器
struct CMD_AS_C_DeleteAndroid
{
	DWORD							dwUserID;							//机器标识
	DWORD							dwGroupID;							//群组标识			
};

//删除模板
struct  CMD_AS_C_DeleteATemplate
{
	DWORD							dwTemplateID;						//模板标识
};

//删除模板
struct  CMD_AS_C_DeleteSTemplate
{
	DWORD							dwTemplateID;						//模板标识
};

//登陆成功
struct CMD_AS_S_LogonSuccess
{	
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	DWORD							dwCustomID;							//自定头像
	DWORD							dwUserID;							//用户 I D
	WCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	WCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	WCHAR							szLogonPass[LEN_PASSWORD];			//登录密码	
};

//登录失败
struct CMD_AS_S_LogonFailure
{
	LONG							lErrorCode;							//错误代码
	WCHAR							szErrorDescribe[128];				//错误消息
};

//批次删除
struct CMD_AS_S_BatchDelete
{
	DWORD							dwBatchID;							//批次标识
};

//模板删除
struct CMD_AS_S_ATemplateDelete
{
	DWORD							dwTemplateID;						//模板标识
};

//模板删除
struct CMD_AS_S_STemplateDelete
{
	DWORD							dwTemplateID;						//模板标识
};

//成员删除
struct CMD_AS_S_BatchMemberDelete
{
	DWORD							dwBatchID;							//批次标识
	DWORD							dwUserID;							//用户标识
};

//成员删除
struct CMD_AS_S_GroupMemberDelete
{
	DWORD							dwGroupID;							//群组标识
	DWORD							dwUserID;							//用户标识
};

//操作成功
struct CMD_AS_S_OperateSuccess
{
	LONG							lOperateCode;						//操作代码
	WCHAR							szOperateNotice[128];				//操作提示
};

//操作失败
struct CMD_AS_S_OperateFailure
{
	LONG							lErrorCode;							//错误代码
	WCHAR							szErrorDescribe[128];				//错误消息
};

//////////////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif