#ifndef CMD_LONGON_HEAD_FILE
#define CMD_LONGON_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//命令定义

//登录命令
#define MDM_GP_LOGON				10									//广场登录

//////////////////////////////////////////////////////////////////////////

//登录模式
#define SUB_GP_LOGON_GAMEID			1									//I D 登录
#define SUB_GP_LOGON_ACCOUNTS		2									//帐号登录
#define SUB_GP_LOGON_MANAGETOOL		3									//管理工具
#define SUB_GP_REGISTER_ACCOUNTS	4									//注册帐号

//////////////////////////////////////////////////////////////////////////

//登录结果
#define SUB_GP_LOGON_SUCCESS		100									//登录成功
#define SUB_GP_LOGON_FAILURE		101									//登录失败
#define SUB_GP_LOGON_FINISH			102									//登录完成
#define SUB_GP_LOGON_ACTIVE			103									//激活帐号
#define SUB_GP_SHOW_ONLINE			104									//显示在线

//命令提示
#define SUB_GP_UPDATE_NOTIFY		200									//升级提示
#define SUB_GP_UPDATE_SILENT		201									//后台升级

//////////////////////////////////////////////////////////////////////////

//I D 登录
struct CMD_GP_LogonByGameID
{	
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwPlazaVersion;						//广场版本	
	TCHAR							szPassword[LEN_MD5];				//登录密码
};

//帐号登录
struct CMD_GP_LogonByAccounts
{
	DWORD							dwPlazaVersion;						//广场版本
	WORD							wMaskServerVisible;					//可视掩码
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
};

//注册帐号
struct CMD_GP_RegisterAccounts
{	
	WORD							wFaceID;							//头像标识
	DWORD							dwPlazaVersion;						//广场版本
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szBankPass[LEN_MD5];				//银行密码
};

//登录成功
struct CMD_GP_LogonSuccess
{
	//属性资料
	WORD							wFaceID;							//头像标识
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwGroupID;							//社团标识
	DWORD							dwCustomID;							//自定标识
	BYTE							cbMasterOrder;						//管理员级别
	DWORD							dwProtectID;						//密保ID
	BYTE							cbMoorMachine;						//绑定机器标识

	//经验等级
	DWORD							dwExperience;						//经验数值

	//会员等级
	DWORD							dwMemberPoint;						//会员点数
	DWORD							dwMemberOverTime;					//到期时间

	//财富信息
	LONGLONG						lUserGold;							//用户金币
	LONGLONG						lInsureGold;						//银行金币

	//用户信息
	BYTE							cbAge;								//用户年龄
	BYTE							cbGender;							//用户性别
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字
};

//登录失败标识
#define LGF_NEED_ACTIVATE			8									//需要激活
#define LGF_CHANGE_ACCOUNT			9									//更换帐号

//登录失败
struct CMD_GP_LogonFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//错误消息
};

//登陆完成
struct CMD_GP_LogonFinish
{
	WORD							wIntermitTime;						//中断时间
	WORD							wOnLineCountTime;					//更新时间
	DWORD							dwFunctionOption;					//功能选项
};

//升级提示
struct CMD_GP_UpdateNotify
{
	BYTE							cbMustUpdate;						//强行升级
	BYTE							cbAdviceUpdate;						//建议升级
	DWORD							dwCurrentVersion;					//当前版本
};

//激活帐号
struct CMD_GP_NotifyActivateAccounts
{
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szTips[128];						//提示信息
	TCHAR							szAdTitle[128];						//广告标题
	TCHAR							szAdUrl[128];						//广告地址
};

//////////////////////////////////////////////////////////////////////////
//携带信息 CMD_GP_RegisterAccounts

#define DTP_GP_USER_INFO			1									//用户信息
#define DTP_GP_MACHINE_ID			2									//机器标识
#define DTP_GP_PASSPORT				3									//用户身份
#define DTP_GP_SPREADER				4									//推荐玩家
#define DTP_GP_BANK_PASS			5									//银行密码
#define DTP_GP_NICKNAME				6									//用户昵称
#define DTP_GP_TRUENAME				7									//真实姓名
#define DTP_GP_NEW_COMPUTER_ID      8									//机器标识
#define DTP_GP_VERIFY_COMPUTER_ID   9									//机器标识
#define DTP_GP_UNIQUEID				10									//用户UID
#define DTP_GP_MOBILE_PHONE			11									//手机号码

//用户信息
struct DTP_GP_UserInfo
{
	BYTE							cbAge;								//用户年龄
	BYTE							cbGender;							//用户性别
	TCHAR							szArea[LEN_AREA];					//地区信息
	TCHAR							szCity[LEN_CITY];					//城市信息
	TCHAR							szProvince[LEN_PROVINCE];			//省份信息
};

//用户身份
struct DTP_GP_Passport
{
	BYTE							cbPassportType;						//身份类型
	TCHAR							szPassportID[LEN_PASSPORT_ID];		//用户身份
};

//////////////////////////////////////////////////////////////////////////
//携带信息 CMD_GP_LogonSuccess

#define DTP_GP_AREA_INFO			1									//区域信息
#define DTP_GP_GROUP_INFO			2									//社团信息
#define DTP_GP_MEMBER_INFO			3									//会员信息
#define	DTP_GP_UNDER_WRITE			4									//个性签名


//区域信息
struct DTP_GP_AreaInfo
{
	TCHAR							szArea[LEN_AREA];					//地区信息
	TCHAR							szCity[LEN_CITY];					//城市信息
	TCHAR							szProvince[LEN_PROVINCE];			//省份信息
};

//社团信息
struct DTP_GP_GroupInfo
{
	DWORD							dwGroupID;							//社团索引
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字
};

//会员信息
struct DTP_GP_MemberInfo
{
	BYTE							cbMemberOrder;						//会员等级
	DWORD							dwMemberPoint;						//会员点数
	DWORD							dwMemberOverTime;					//到期时间
};

//////////////////////////////////////////////////////////////////////////
//列表命令

#define MDM_GP_SERVER_LIST			11									//列表信息

//////////////////////////////////////////////////////////////////////////

//获取命令
#define SUB_GP_GET_LIST				1									//获取列表
#define SUB_GP_GET_SERVER			2									//获取房间
#define SUB_GP_GET_ONLINE			3									//获取在线
#define SUB_GP_GET_COLLECTION		4									//获取收藏

//////////////////////////////////////////////////////////////////////////

//列表信息
#define SUB_GP_LIST_TYPE			100									//类型列表
#define SUB_GP_LIST_KIND			101									//种类列表
#define SUB_GP_LIST_NODE			102									//节点列表
#define SUB_GP_LIST_SERVER			103									//房间列表
#define SUB_GP_LIST_CUSTOM			104									//定制列表

//完成信息
#define SUB_GP_LIST_FINISH			200									//发送完成
#define SUB_GP_SERVER_FINISH		201									//房间完成

//在线信息
#define SUB_GR_KINE_ONLINE			300									//类型在线
#define SUB_GR_SERVER_ONLINE		301									//房间在线

//////////////////////////////////////////////////////////////////////////

//获取在线
struct CMD_GP_GetOnline
{
	WORD							wServerCount;						//房间数目
	WORD							wOnLineServerID[MAX_SERVER];		//房间标识
};

//类型在线
struct CMD_GP_KindOnline
{
	WORD							wKindCount;							//类型数目
	tagOnLineInfoKind				OnLineInfoKind[MAX_KIND];			//类型在线
};

//房间在线
struct CMD_GP_ServerOnline
{
	WORD							wServerCount;						//房间数目
	tagOnLineInfoServer				OnLineInfoServer[MAX_SERVER];		//房间在线
};

//////////////////////////////////////////////////////////////////////////
//服务命令

#define MDM_GP_USER_SERVICE			12									//用户服务

//////////////////////////////////////////////////////////////////////////

//账号服务
#define SUB_GP_MODIFY_ACCOUNTS		1									//修改帐号
#define SUB_GP_MODIFY_LOGON_PASS	2									//修改密码
#define SUB_GP_MODIFY_INSURE_PASS	3									//修改密码
#define SUB_GP_MODIFY_INDIVIDUAL	4									//修改资料
#define SUB_GP_BIND_MACHINE			6									//锁定机器
#define SUB_GP_UN_BIND_MACHINE		7									//解锁机器

//查询命令
#define	SUB_GP_QUERY_INDIVIDUAL		20									//查询信息

//////////////////////////////////////////////////////////////////////////

//操作结果
#define SUB_GP_OPERATE_SUCCESS		100									//操作成功
#define SUB_GP_OPERATE_FAILURE		101									//操作失败

//用户信息
#define SUB_GP_USER_INDIVIDUAL		203									//个人资料

//////////////////////////////////////////////////////////////////////////

//修改帐号
struct CMD_GP_ModifyAccounts
{
	//验证资料
	DWORD							dwUserID;							//用户 I D
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
};

//修改密码
struct CMD_GP_ModifyLogonPass
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szDesPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
};

//修改密码
struct CMD_GP_ModifyInsurePass
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szDesPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码
};

//修改资料
struct CMD_GP_ModifyIndividual
{
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
};

//查询信息
struct CMD_GP_QueryIndividual
{
	DWORD							dwUserID;							//用户 I D
};

//操作失败
struct CMD_GP_OperateFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//错误消息
};

//操作成功
struct CMD_GP_OperateSuccess
{
	LONG							lResultCode;						//操作代码
	TCHAR							szDescribeString[128];				//成功消息
};

//个人资料
struct CMD_GP_UserIndividual
{
	DWORD							dwUserID;							//用户 I D
};

//锁定机器
struct CMD_GP_BindMachine
{
	//验证资料
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//机器码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//解锁机器
struct CMD_GP_UnbindMachine
{
	//验证资料
	DWORD							dwUserID;							//用户 I D
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//机器码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//////////////////////////////////////////////////////////////////////////
//携带信息 CMD_GP_UserIndividual

#define DTP_GP_UI_USER_NOTE			1									//用户说明
#define DTP_GP_UI_COMPELLATION		2									//真实名字
#define DTP_GP_UI_SEAT_PHONE		3									//固定电话
#define DTP_GP_UI_MOBILE_PHONE		4									//移动电话
#define DTP_GP_UI_QQ				5									//Q Q 号码
#define DTP_GP_UI_EMAIL				6									//电子邮件
#define DTP_GP_UI_DWELLING_PLACE	7									//联系地址
#define DTP_GP_UI_NICKNAME			8									//用户昵称

//显示在线信息参数
#define SHOW_TOTAL_USER_COUNT		0x00000001							//显示总在线
#define SHOW_KIND_USER_COUNT		0x00000002							//显示类型在线
#define SHOW_SERVER_USER_COUNT		0x00000004							//显示房间在线


//////////////////////////////////////////////////////////////////////////////////
//机器服务

#define MDM_GP_ANDROID_SERVICE		14									//机器服务

//参数操作
#define SUB_GP_GET_PARAMETER		100									//获取参数
#define SUB_GP_ADD_PARAMETER		101									//添加参数
#define SUB_GP_MODIFY_PARAMETER		102									//修改参数
#define SUB_GP_DELETE_PARAMETER		103									//删除参数

//参数信息
#define SUB_GP_ANDROID_PARAMETER	200									//机器参数		

//////////////////////////////////////////////////////////////////////////////////
//获取参数
struct CMD_GP_GetParameter
{
	WORD							wServerID;							//房间标识
};

//添加参数
struct CMD_GP_AddParameter
{
	WORD							wServerID;							//房间标识
	tagRobotParameter				RobotParameter;					//机器参数
};

//修改参数
struct CMD_GP_ModifyParameter
{
	WORD							wServerID;							//房间标识
	tagRobotParameter				RobotParameter;					//机器参数
};

//删除参数
struct CMD_GP_DeleteParameter
{
	WORD							wServerID;							//房间标识
	DWORD							dwBatchID;							//批次标识
};

//////////////////////////////////////////////////////////////////////////////////
//机器参数
struct CMD_GP_RobotParameter
{
	WORD							wSubCommdID;						//子命令码
	WORD							wParameterCount;					//参数数目
	tagRobotParameter				RobotParameter[MAX_BATCH];		//机器参数
};

//////////////////////////////////////////////////////////////////////////////////
//管理服务
#define MDM_GP_MANAGER_SERVICE		16									//管理服务


#define SUB_GP_MANAGE_SUCCESS		100									//修改配置

//////////////////////////////////////////////////////////////////////////////////

//登录命令
#define MDM_MB_LOGON				100									//手机登陆

//登录模式
#define SUB_MB_LOGON_EMAIL			1									//邮箱登陆
#define SUB_MB_LOGON_EMAILCODE		2									//邮箱验证码
#define SUB_MB_LOGON_MOBILECODE		3									//手机验证码
#define SUB_MB_LOGON_MOBILEPHONE	4									//手机登陆
#define SUB_MB_LOGON_ACCOUNTS	    5									//帐号登录
#define SUB_MB_LOGON_VISITOR		6									//游客登录
#define SUB_MB_LOGON_SECRETKEY		7									//秘钥登录
#define SUB_MB_LOGON_THIRDPARTY		8									//他方登录
#define SUB_MB_REGISTER_EMAIL		9									//邮箱注册
#define SUB_MB_REGISTER_ACCOUNTS	10									//帐号注册
#define SUB_MB_REGISTER_MOBILEPHONE	11									//手机注册

//登出命令
#define SUB_MB_LOGOUT_NOTIFY		20									//登出提醒

//登录结果
#define SUB_MB_LOGON_SUCCESS		100									//登录成功
#define SUB_MB_LOGON_FAILURE		101									//登录失败
#define SUB_MB_LOGON_FINISH			102									//登录完成

//升级提示
#define SUB_MB_UPDATE_NOTIFY		200									//升级提示

//////////////////////////////////////////////////////////////////////////////////
//功能选项
#define FO_FORBID_RECHARGE			0x00000001							//禁止充值


//////////////////////////////////////////////////////////////////////////////////

//游客登录
struct CMD_MB_LogonVisitor
{
	//设备信息
	WORD							wModuleID;							//模块标识
	WORD							wMarketID;							//渠道标识	
    DWORD                           dwSpreaderID;						//代理ID
    DWORD                           dwDeviceKind;                       //设备类型
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本
	
	//系统信息
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//广场版本		
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识	
	TCHAR							szMachineName[LEN_ACCOUNTS];		//机器名字
};

//秘钥登录
struct CMD_MB_LogonSecretKey
{
	//系统信息
	WORD							wModuleID;							//模块标识
	WORD							wMarketID;							//渠道标识
	DWORD                           dwDeviceKind;                       //设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//广场版本		
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本

	//登录信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szSecretKey[LEN_LOGON_TOKEN];		//登录令牌

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码
};

//帐号登录
struct CMD_MB_LogonAccounts
{
	//系统信息
	WORD							wModuleID;							//模块标识
	WORD							wMarketID;							//渠道标识
	DWORD                           dwDeviceKind;                       //设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//广场版本		
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本

	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码
};

//手机验证码
struct CMD_MB_LogonMobileCode
{
	//系统信息
	WORD							wModuleID;							//模块标识
	WORD							wMarketID;							//渠道标识
	DWORD                           dwSpreaderID;						//推广员ID
	DWORD                           dwDeviceKind;                       //设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//广场版本	
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本

	//登录信息
	TCHAR							szAuthCode[LEN_MD5];				//验证码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机号码

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识	
};

//手机登录
struct CMD_MB_LogonMobilePhone
{
	//系统信息
	WORD							wModuleID;							//模块标识
	WORD							wMarketID;							//渠道标识
	DWORD                           dwDeviceKind;                       //设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//广场版本		
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本

	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机号码

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识	
};

//第三方登录
struct CMD_MB_LogonThirdParty
{
	//系统信息
	WORD							wModuleID;							//模块标识
	WORD							wMarketID;							//渠道标识
    DWORD                           dwSpreaderID;						//推广员ID
    DWORD                           dwDeviceKind;                       //设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//广场版本	
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本

	//登录信息
	BYTE							cbGender;							//用户性别
	DWORD							dwExternalID;						//他方标识	
	TCHAR							szUniqueID[LEN_UNIQUEID];			//用户UID
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码
};

//注册帐号
struct CMD_MB_RegisterAccounts
{
	//系统信息
	WORD							wModuleID;							//模块标识
	WORD							wMarketID;							//渠道标识
    DWORD                           dwSpreaderID;						//代理ID
    DWORD                           dwDeviceKind;                       //设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//广场版本
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本

	//注册信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	TCHAR							szAccounts[LEN_ACCOUNTS];			//用户账号
	TCHAR							szNickName[LEN_NICKNAME];			//用户昵称		
	TCHAR							szLogonPass[LEN_MD5];				//登录密码	

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识	
};


//邮箱注册
struct CMD_MB_RegisterEMail
{
	//系统信息
	WORD							wModuleID;							//模块标识
	WORD							wMarketID;							//渠道标识
	DWORD                           dwSpreaderID;						//代理ID
	DWORD                           dwDeviceKind;                       //设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//广场版本
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本

	//注册信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	TCHAR							szAuthCode[LEN_MD5];				//验证码
	TCHAR							szLogonPass[LEN_MD5];				//登录密码
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称	
	TCHAR							szEMail[LEN_EMAIL];					//邮箱

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识	
};

//手机注册
struct CMD_MB_RegisterMobilePhone
{
	//系统信息
	WORD							wModuleID;							//模块标识
	WORD							wMarketID;							//渠道标识
	DWORD                           dwSpreaderID;						//代理ID
	DWORD                           dwDeviceKind;                       //设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//广场版本
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本

	//注册信息
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	TCHAR							szAuthCode[LEN_MD5];				//验证码
	TCHAR							szLogonPass[LEN_MD5];				//登录密码
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称	
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//电话号码

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识	
};


//邮箱登录
struct CMD_MB_LogonEMail
{
	//系统信息
	WORD							wModuleID;							//模块标识
	WORD							wMarketID;							//渠道标识
	DWORD                           dwDeviceKind;                       //设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//广场版本		
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本

	//登录信息
	TCHAR							szPassword[LEN_MD5];				//登录密码
	TCHAR							szEMail[LEN_EMAIL];					//邮箱

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识	
};

//邮箱验证码
struct CMD_MB_LogonEMailCode
{
	//系统信息
	WORD							wModuleID;							//模块标识
	WORD							wMarketID;							//渠道标识
	DWORD                           dwSpreaderID;						//推广员ID
	DWORD                           dwDeviceKind;                       //设备类型
	DWORD							dwAppVersion;						//应用版本
	DWORD							dwPlazaVersion;						//广场版本	
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//系统版本

	//登录信息
	TCHAR							szAuthCode[LEN_MD5];				//验证码
	TCHAR							szEMail[LEN_EMAIL];					//邮箱

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器标识	
};

//登陆完成
struct CMD_MB_LogonFinish
{
	DWORD							dwFuncOption;						//功能选项	
};

//登出提醒
struct CMD_MB_LogoutNotify
{
	TCHAR							szDescribeString[128];				//描述信息
};

///////////////////////////////////////////////////////////////////////////////////////////

//游客验证
struct CMD_MB_OnlineCheck
{
	DWORD							dwUserID;							//用户 I D
};

//登录成功
struct CMD_MB_LogonSuccess
{
	WORD							wFaceID;							//头像标识
	WORD							wActorID;							//角色标识
	BYTE							cbGender;							//用户性别
	DWORD							dwCustomID;							//自定头像
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	WORD							wFaceFrameID;						//像框标识
	WORD							wChatFrameID;						//聊天框标识
	WORD							wCardBackID;						//牌背标识
	WORD							wTableBackID;						//桌背标识
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
	TCHAR							szInsurePass[LEN_PASSWORD];			//银行密码

	//经验等级
	DWORD							dwExperience;						//经验数值

	//魅力值
	DWORD							dwLoveLiness;						//魅力值

	//会员资料
	WORD							wMasterOrder;						//管理等级
	DWORD							dwMemberPoint;						//会员点数
	DWORD							dwMemberOverTime;					//到期时间

	//财富信息
	SCORE							lUserGold;							//用户黄金
	SCORE							lUserIngot;							//用户钻石	
	SCORE							lUserInsure;						//用户银行	
	SCORE							lCommission;						//用户佣金


	//行为掩码
	DWORD							dwActionMaskEver;					//行为掩码
	DWORD							dwActionMaskPerDay;					//行为掩码
	DWORD							dwActionMaskPerWeek;				//行为掩码

	//上级信息
	DWORD							dwParentGameID;						//用户 I D
	WORD							wParentFaceID;						//头像ID
	TCHAR							szParentNickName[LEN_ACCOUNTS];		//用户昵称

	//锁定信息
	WORD							wLockKindID;						//类型标识
	WORD							wLockServerID;						//锁定房间
	WORD							wLockServerKind;					//房间类型

	//状态信息	
	BYTE							cbShutUpStatus;						//禁言状态
	DWORD							dwShutUpOverTime;					//解禁时间

	//附加信息	
	DWORD							dwServerTime;						//服务器时间	
	DWORD							dwLikedNumber;						//被赞数目	
	BYTE							cbRegisterMode;						//注册模式
};

//登录失败
struct CMD_MB_LogonFailure
{
	LONG							lResultCode;						//错误代码
	TCHAR							szDescribeString[128];				//描述消息
};

//升级提示
struct CMD_MB_UpdateNotify
{
	BYTE							cbMustUpdate;						//强行升级
	BYTE							cbAdviceUpdate;						//建议升级
	DWORD							dwCurrentVersion;					//当前版本
};

//////////////////////////////////////////////////////////////////////////////////
//列表命令

#define MDM_MB_SERVER_LIST			101									//列表信息

//获取命令
#define SUB_MB_GET_LIST				1									//获取列表
#define SUB_MB_GET_SERVER			2									//获取房间
#define SUB_MB_GET_ONLINE			3									//获取在线

//列表信息
#define SUB_MB_LIST_KIND			100									//种类列表
#define SUB_MB_LIST_SERVER			101									//房间列表
#define SUB_MB_LIST_MATCH			102									//比赛列表
#define SUB_MB_GAME_OPTION			104									//游戏选项
#define SUB_MB_CREATE_OPTION		103									//开房选项
#define SUB_MB_LIST_LOGON			105									//登录列表
#define SUB_MB_LIST_AGENT			106									//代理列表
#define SUB_MB_SERVER_AGENT			107									//房间代理
#define SUB_MB_LIST_ACCESS			108									//网关服务
#define SUB_MB_LIST_ONLINE			109									//在线信息
#define SUB_MB_LIST_FINISH			200									//列表完成
#define SUB_MB_SERVER_FINISH		201									//房间完成

//////////////////////////////////////////////////////////////////////////////////

//在线信息
struct tagOnlineInfo
{
	WORD							wServerID;							//房间标识
	DWORD							dwOnlineCount;						//在线人数
};

//获取在线
struct tagGetOnline
{
	WORD							wKindID;							//类型标识
};

//获取房间
struct tagGetServer
{
	WORD							wKindID;							//类型标识
	DWORD							dwUserID;							//用户标志
};

//获取配置
struct tagGetOption
{
	WORD							wModuleID;							//类型标识
};


//////////////////////////////////////////////////////////////////////////
//服务命令

#define MDM_MB_USER_SERVICE			102									//用户服务

//////////////////////////////////////////////////////////////////////////

//用户中心
#define SUB_MB_LIKE_PLAYER			1									//点赞玩家
#define SUB_MB_MODIFY_LOGON_PASS	2									//修改密码
#define SUB_MB_MODIFY_INDIVIDUAL	5									//修改资料

//查询命令
#define SUB_MB_QUERY_WEALTH			20									//查询财富
#define	SUB_MB_QUERY_INDIVIDUAL		21									//查询信息
#define	SUB_MB_QUERY_RANKINGINFO	22									//查询排位
#define SUB_MB_QUERY_RANKING_LIST	23									//查询榜单	

//修改信息
#define SUB_MB_MODIFY_ACCESSOR		30									//修改配件
#define SUB_MB_MODIFY_SYSTEM_FACE	31									//修改头像
#define SUB_MB_MODIFY_CUSTOM_FACE	32									//修改头像

//验证码验证
#define SUB_MB_BIND_EMAIL			40									//绑定邮箱
#define SUB_MB_UNBIND_EMAIL			41									//解绑邮箱
#define SUB_MB_BIND_MOBILEPHONE		42									//绑定手机
#define SUB_MB_UNBIND_MOBILEPHONE	43									//解绑手机
#define SUB_MB_RESET_LOGON_PASS		44									//重置密码
#define SUB_MB_ACQUIRE_AUTHCODE		49									//获取验证码

//好友命令
#define SUB_MB_DELETE_FRIEND		50									//删除好友
#define SUB_MB_APPLY_ADD_FRIEND		51									//申请加好友
#define SUB_MB_AGREE_ADD_FRIEND		52									//同意加好友
#define SUB_MB_REFUSE_ADD_FRIEND	53									//拒绝加好友
#define SUB_MB_SETUP_INFO_PUBLIC	54									//信息公开

//兑换命令
#define SUB_MB_EXCHANGE_MALLGOODS	60									//兑换商品
#define SUB_MB_EXCHANGE_GIFTBAGCODE 61									//兑换礼包码

//请求命令
#define SUB_MB_BIND_INVITE_CODE		70									//绑定邀请码
#define SUB_MB_SUBMIT_INFORM_INFO	71									//提交投诉
#define SUB_MB_BIND_EXTERNAL_ACCOUNT 75									//绑定账号

//赠送礼物
#define SUB_MB_SEND_GIFT			80									//赠送礼物
#define SUB_MB_LOAD_GIFT			81									//加载礼物
#define SUB_MB_GIFT_STATUS_UPDATE	82									//礼物状态更新
#define SUB_MB_SEND_GIFT_SUCCESS	83									//赠送成功
#define SUB_MB_RECV_GIFT_NOTIFY		84									//礼物通知
#define SUB_MB_GIFT_INFO			85									//礼物信息
#define SUB_MB_GIFT_INFO_FINISH		86									//礼物信息完成
#define SUB_MB_GLOBAL_GIFT_NOTIFY	87									//全局礼物通知

//转盘命令
#define SUB_MB_QUERY_TURNTABLE_LOTTERY 90								//转盘数据
#define SUB_MB_REQUEST_TURNTABLE_LOTTERY 91								//请求抽奖

//转账命令
#define SUB_MB_WEALTH_TRANSFER		110									//货币转账

//佣金命令
#define SUB_MB_DRAW_COMMISSION		120									//提取佣金

//////////////////////////////////////////////////////////////////////////

//查询信息
#define SUB_MB_USER_INDIVIDUAL		200									//个人资料
#define SUB_MB_USER_FACE_INFO		201									//头像信息
#define SUB_MB_USER_ACTION_MASK		202									//掩码信息
#define SUB_MB_USER_RANKING_INFO	203									//排位信息
#define SUB_MB_USER_ACCESSOR_UPDATE	204									//配件更新
#define SUB_MB_USER_CACHE_INFO		205									//缓存信息
#define SUB_MB_RANKING_LIST_DATA	206									//榜单数据

//账号状态
#define SUB_MB_SHUTUP_STATUS_UPDATE	210									//状态更新

//转盘命令
#define SUB_MB_TURNTABLE_OPTION		220									//转盘配置
#define SUB_MB_TURNTABLE_LOTTERY_DATA	221								//转盘数据
#define SUB_MB_TURNTABLE_LOTTERY_RESULT 222								//抽奖结果

//验证码验证
#define SUB_MB_BINDMP_SUCCESS		350									//绑定成功
#define SUB_MB_UNBINDMP_SUCCESS		351									//解绑成功
#define SUB_MB_BIND_EMAIL_SUCCESS	352									//绑定成功			
#define SUB_MB_UNBIND_EMAIL_SUCCESS	353									//解绑成功			
#define SUB_MB_ACQUIREAC_RESULT		355									//获取结果

//应答命令
#define SUB_MB_BIND_INVITE_CODE_SUCCESS	 360							//绑定成功
#define SUB_MB_BIND_EXTERNAL_ACCOUNT_SUCCESS 361						//绑定成功

//////////////////////////////////////////////////////////////////////////
//配件掩码
#define ACCESSOR_MASK_ACTOR			0x01								//配件掩码
#define ACCESSOR_MASK_FACE_FRAME	0x02								//配件掩码
#define ACCESSOR_MASK_CHAT_FRAME	0x04								//配件掩码
#define ACCESSOR_MASK_CARD_BACK		0x08								//配件掩码
#define ACCESSOR_MASK_TABLE_BACK	0x10								//配件掩码

//////////////////////////////////////////////////////////////////////////
//点赞玩家
struct CMD_MB_LikePlayer
{
	DWORD							dwDstUserID;						//目标用户
};

//修改密码
struct CMD_MB_ModifyLogonPass
{
	TCHAR							szDesPassword[LEN_PASSWORD];		//用户密码
	TCHAR							szScrPassword[LEN_PASSWORD];		//用户密码	
};

//修改资料
struct CMD_MB_ModifyIndividual
{
	BYTE							cbGender;							//用户性别	
};

//绑定账号
struct CMD_MB_BindExternalAccount
{
	DWORD							dwExternalID;						//他方标识	
	TCHAR							szUniqueID[LEN_UNIQUEID];			//用户UID
};

//提交投诉
struct CMD_MB_SubmitInformInfo
{
	BYTE							cbInformKind;						//投诉类型
	BYTE							cbInformSource;						//投诉来源
	DWORD							dwBeInformUserID;					//被投诉玩家
};

//个人资料
struct CMD_MB_UserIndividual
{
	DWORD							dwUserID;							//用户 I D
};

//用户头像
struct CMD_MB_UserFaceInfo
{
	WORD							wFaceID;							//头像标识
	DWORD							dwCustomID;							//自定标识
};

//修改头像
struct CMD_MB_ModifySystemFace
{
	WORD							wFaceID;							//头像标识
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//修改头像
struct CMD_MB_ModifyCustomFace
{
	DWORD							dwCustomID;							//自定义标识
	TCHAR							szFacePath[LEN_USER_FACE];			//头像路径
};

//修改配件
struct CMD_MB_ModifyAccessor
{
	WORD							wAccessorMask;						//配件掩码
	WORD							wAccessorValue;						//配件数值
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//配件更新
struct CMD_MB_UserAccessorUpdate
{
	WORD							wAccessorMask;						//配件掩码
	WORD							wAccessorValue;						//配件数值
};

//行为掩码
struct CMD_MB_UserActionMask
{
	DWORD							dwActionMaskEver;					//行为掩码
	DWORD							dwActionMaskPerDay;					//行为掩码
	DWORD							dwActionMaskPerWeek;				//行为掩码
};

//银行访问结果
struct CMD_MB_BankAccessResult
{
	LONG							lReturnCode;						//返回代码
	LONGLONG						lGold;								//返回金币
	LONGLONG						lInsureGold;						//银行金币
	TCHAR							szDescribeString[128];				//返回消息
};

//查询结果
struct CMD_MB_BankQueryResult
{
	LONG							lResultCode;						//返回代码
	DWORD							dwUserID;							//用户ID
	DWORD							dwGameID;							//游戏ID
	TCHAR							szAccounts[LEN_ACCOUNTS];			//用户帐号
	TCHAR							szDescribeString[128];				//返回消息
};

//查询排位结果
struct CMD_MB_UserRankingInfo
{
	tagUserRankingInfo				UserRankingInfo;					//玩家排位信息
};

//状态更新
struct CMD_MB_ShutUpStatusUpdate
{
	BYTE							cbShutUpStatus;						//禁言状态
	DWORD							dwShutUpOverTime;					//解禁时间
};

//获取验证码
struct CMD_MB_AcquireAuthCode
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//验证信息
	BYTE							cbCodeKind;							//验证码类型
	BYTE							cbAuthKind;							//验证类型
	DWORD							dwBindUserID;						//绑定标识
	TCHAR							szMobileEmail[LEN_EMAIL];			//手机邮件

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//绑定手机号
struct CMD_MB_BindMobilePhone
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
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//解绑定手机号
struct CMD_MB_UnbindMobilePhone
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//验证信息
	TCHAR							szAuthCode[LEN_MD5];				//验证码
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机邮箱

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//重置密码
struct CMD_MB_ResetLogonPasswd
{
	//验证信息
	DWORD							dwBindUserID;						//绑定标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//验证信息
	BYTE							cbCodeKind;							//验证码类型
	TCHAR							szAuthCode[LEN_MD5];				//验证密码
	TCHAR							szMobileEmail[LEN_EMAIL];			//手机邮箱

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//绑定邮箱
struct CMD_MB_BindEMail
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							szNewPassword[LEN_PASSWORD];		//用户密码

	//验证信息
	DWORD							dwUnbindUserID;						//解绑标识
	TCHAR							szEMail[LEN_EMAIL];					//邮箱
	TCHAR							szAuthCode[LEN_MD5];				//验证码	

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//解绑定邮箱
struct CMD_MB_UnbindEMail
{
	//用户信息
	DWORD							dwUserID;							//用户标识
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码

	//验证信息
	TCHAR							szEMail[LEN_EMAIL];					//邮箱
	TCHAR							szAuthCode[LEN_MD5];				//验证码	

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//绑定成功
struct CMD_MB_BindMPSuccess
{
	//提示信息
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//绑定号码
	TCHAR							szNoticeString[128];				//提示信息
};

//解绑成功
struct CMD_MB_UnbindMPSuccess
{
	//提示信息
	TCHAR							szNoticeString[128];				//提示信息
};

//获取结果
struct CMD_MB_AcquireACResult
{
	//验证信息
	BYTE							cbAuthKind;							//验证类型
	DWORD							dwBindUserID;						//绑定标识	

	//结果信息
	LONG							lResultCode;						//结果代码
	TCHAR							szNoticeString[128];				//提示信息
};

//绑定成功
struct CMD_MB_BindEMailSuccess
{
	//提示信息
	TCHAR							szEMail[LEN_EMAIL];					//绑定邮箱
	TCHAR							szNoticeString[128];				//提示信息
};

//解绑成功
struct CMD_MB_UnbindEMailSuccess
{
	//提示信息
	TCHAR							szNoticeString[128];				//提示信息
};

//删除好友
struct CMD_MB_DeleteFriend
{
	DWORD							dwFriendID;							//好友标识
};

//添加好友
struct CMD_MB_ApplyAddFriend
{
	DWORD							dwObjectID;							//目标标识
	TCHAR							szApplyDesc[128];					//申请描述
};

//同意添加
struct CMD_MB_AgreeAddFriend
{
	DWORD							dwApplyID;							//申请标识
};

//拒绝添加
struct CMD_MB_RefuseAddFriend
{
	DWORD							dwApplyID;							//申请标识
	TCHAR							szRefuseDesc[128];					//拒绝描述
};

//信息公开
struct CMD_MB_SetupInfoPublic
{
	bool							bInfoPublic;						//公开标志
};

//货币转账
struct CMD_MB_WealthTransfer
{
	//验证信息
	BYTE							cbCodeKind;							//验证码类型
	TCHAR							szAuthCode[LEN_MD5];				//验证密码

	//转账信息
	DWORD							dwTargetGameID;						//游戏标识
	SCORE							lTransferAmount;					//转出金额

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//取出佣金
struct CMD_MB_DrawCommission
{
	//提取信息
	SCORE							lDarwinAmount;						//提取金额

	//连接信息
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//兑换商品
struct CMD_MB_ExchangeMallGoods
{
	DWORD							dwMallGoodsID;						//商品标识
};

//兑换礼包码
struct CMD_MB_ExchangeGiftbagCode
{
	TCHAR							szGiftbagCode[16];					//礼包码
};

//查询榜单
struct CMD_MB_QueryRankingList
{
	BYTE							cbListType;							//榜单类型
};

//绑定邀请码
struct CMD_MB_BindInviteCode
{
    //用户信息
    DWORD							dwUserID;							//用户标识
    TCHAR							szPassword[LEN_PASSWORD];			//用户密码

    //邀请信息
    DWORD							dwInviteCode;						//邀请码

    //机器信息
    TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//绑定邀请码成功
struct CMD_MB_BindInviteCodeSuccess
{
    //上级信息
    DWORD							dwParentGameID;						//用户 I D
    WORD							wParentFaceID;						//头像ID
    TCHAR							szParentNickName[LEN_ACCOUNTS];		//用户昵称

    //提示信息
	TCHAR							szNoticeString[128];				//提示信息
};

//绑定成功 
struct CMD_MB_BindExternalAccountSuccess
{
	DWORD							dwExternalID;						//他方标识	
	TCHAR							szUniqueID[LEN_UNIQUEID];			//用户UID

	//提示信息
	TCHAR							szNoticeString[128];				//提示信息
};

//赠送礼物
struct CMD_MB_SendGift
{
    //用户信息
    DWORD							dwUserID;							//用户标识

	//物品信息
    DWORD							dwGoodsID;							//商品标识
	DWORD							dwGoodsCount;						//物品数量
};

//赠送成功
struct CMD_MB_SendGiftSuccess
{
    DWORD							dwTargetUserID;						//目标用户
    DWORD							dwLoveLiness;						//魅力值

    //物品信息
    DWORD							dwGoodsID;							//商品标识
    DWORD							dwGoodsCount;						//物品数量

    BYTE							cbSendType;							//赠送类型(0:全局赠送 1:私人赠送)
};

//加载礼物
struct CMD_MB_LoadGift
{
    DWORD							dwUserID;							//用户标识
};

//礼物信息
struct CMD_MB_UserGiftInfo
{
    WORD							wItemCount;							//礼物数量
    tagGiftInfo						GiftInfo[0];						//礼物信息	
};

//全局礼物通知
struct CMD_MB_GlobalGiftNotify
{
    DWORD							dwUserID;							//用户标识
    DWORD							dwTargetUserID;						//目标用户

    //物品信息
    DWORD							dwGoodsID;							//商品标识
    DWORD							dwGoodsCount;						//物品数量
    DWORD							dwLoveLiness;						//魅力值

    BYTE							cbSendType;							//赠送类型(0:全局赠送 1:私人赠送)
};

//礼物状态更新
struct CMD_MB_GiftStatusUpdate
{
    DWORD							dwUserID;							//用户标识
    DWORD							dwRecordID;							//记录标识
};

//查询转盘抽奖数据
struct CMD_MB_QueryTurntableLottery
{	
	DWORD							dwRecordID;							//记录标识
	TCHAR							szOptionDigest[LEN_MD5];			//配置摘要
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器码
};

//请求抽奖
struct CMD_MB_RequestTurntableLottery
{
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器码
	TCHAR							szOptionDigest[LEN_MD5];			//配置摘要
};

//转盘配置
struct CMD_MB_TurntableOption
{
	TCHAR							szOptionDigest[LEN_MD5];			//配置摘要
	TCHAR							szTurantableAward[2048];			//转盘奖励
};

//转盘抽奖数据
struct CMD_MB_TurntableLotteryData
{
	//抽奖次数
	WORD							wPayLotteryTimes;					//充值抽奖次数
	WORD							wFreeLotteryTimes;					//免费抽奖次数
	WORD							wGameTimeLotteryTimes;				//时长抽奖次数
	WORD							wGameCountLotteryTimes;				//局数抽奖次数
	WORD							wGameExpendLotteryTimes;			//消耗抽奖次数
	WORD							wResidueLotteryTimes;				//剩余抽奖次数

	//进度数据
	DWORD							dwGameTimeProgress[2];				//游戏时长进度
	DWORD							dwGameCountProgress[2];				//游戏局数进度
	DWORD							dwGameExpendProgress[2];			//游戏消耗进度

	//转盘记录
	TCHAR							szTurantableRecord[4096];			//转盘记录
};

//转盘抽奖结果
struct CMD_MB_TurntableLotteryResult
{
	WORD							wHitIndex;							//命中索引
	WORD							wResidueLotteryTimes;				//剩余抽奖次数
};

//榜单数据
struct CMD_MB_RankingListData
{
	BYTE							cbListType;							//榜单类型
	TCHAR							szRankingList[8196];				//榜单数据
};

//////////////////////////////////////////////////////////////////////////

//携带信息
#define DTP_MB_USER_INFO			0x0001								//用户信息
#define DTP_MB_MACHINE_ID			0x0002								//机器标识
#define DTP_MB_NICKNAME				0x0004								//用户昵称
#define DTP_MB_UNIQUEID				0x0008								//用户UID
#define DTP_MB_COMPELLATION			0x0010								//真实姓名
#define DTP_MB_MOBILE_PHONE			0x0020								//手机号码
#define DTP_MB_EXTERNAL_DATUM		0x0040								//外部资料
#define DTP_MB_UNDER_WRITE			0x0080								//个性签名
#define DTP_MB_LOGON_TOKEN			0x0100								//登录令牌
#define DTP_MB_USERFACE_URL			0x0200								//头像地址
#define DTP_MB_RANKING_INFO			0x0400								//头像地址
#define DTP_MB_USER_EMAIL			0x0800								//电子邮箱
#define DTP_MB_TRANSFER_PARAM		0x1000								//转盘参数
#define DTP_MB_REGISTER_PRESENT		0x2000								//注册赠送
#define DTP_MB_COMMISSION_PARAM		0x4000								//佣金参数

//投诉携带信息
#define DTP_MB_INFORM_ATTACHMENTURL	0x0001								//附件地址
#define DTP_MB_INFORM_MESSAGE		0x0002								//投诉消息
#define DTP_MB_INFORM_CONTENT		0x0004								//投诉内容


//////////////////////////////////////////////////////////////////////////

//转账配置
struct DTP_GP_TransferParam
{
	bool							bTransferEnabled;					//转账开关
	WORD 							wServiceFeeRate;					//服务费率
	SCORE							lReservedAmount;					//保留金额
	SCORE							lMinTransferAmount;					//保留金额
	SCORE							lMaxTransferAmount;					//保留金额
};

//佣金配置
struct DTP_GP_CommissionParam
{
	bool							bDrawEnabled;						//取出开关
	WORD 							wServiceFeeRate;					//服务费率
	SCORE							lMinDrawAmount;						//最少金额	
};

//排位信息
struct DTP_GP_RankingInfo
{
	DWORD							dwRankID;							//排名标识	
	WORD							wSeasonID;							//赛季标识
	LONG							lWinCount;							//赢局数目
	LONG							lLostCount;							//数局数目
	LONG							lWinStreakCount;					//连胜局数
	LONG							lLoveHeart;							//爱心数量
	SCORE							lRankScore;							//排位积分	
};

//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif