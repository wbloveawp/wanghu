#ifndef CMD_COMMOM_HEAD_FILE
#define CMD_COMMOM_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////

#define MDM_CM_SYSTEM				1000								//系统命令

//消息命令
#define SUB_CM_SYSTEM_MESSAGE		1									//系统消息
#define SUB_CM_ACTION_MESSAGE		2									//动作消息

//用户命令
#define SUB_CM_PACK_GOODS			10									//背包物品
#define SUB_CM_GOODS_UPDATE			11									//物品更新
#define SUB_CM_WEALTH_UPDATE		12									//财富更新
#define SUB_CM_LOVELINESS_UPDATE	13									//魅力更新
#define SUB_CM_CONFIG_UPDATE		14									//配置更新

//操作结果
#define SUB_CM_OPERATE_SUCCESS		20									//操作成功
#define SUB_CM_OPERATE_FAILURE		31									//操作失败

//////////////////////////////////////////////////////////////////////////

//类型掩码
#define SMT_CHAT					0x0001								//聊天消息
#define SMT_EJECT					0x0002								//弹出消息
#define SMT_GLOBAL					0x0004								//全局消息
#define SMT_PROMPT					0x0008								//提示消息
#define SMT_TABLE_ROLL				0x0010								//滚动消息

//控制掩码
#define SMT_CLOSE_ROOM				0x0100								//关闭房间
#define SMT_CLOSE_GAME				0x0200								//关闭游戏
#define SMT_CLOSE_LINK				0x0400								//中断连接

//显示掩码
#define SMT_SHOW_MOBILE				0x1000								//手机显示	
#define SMT_SHOW_FLASH				0x2000								//网页显示	

//系统消息
struct CMD_CM_SystemMessage
{
	WORD							wType;								//消息类型
	WORD							wElpase;							//定时时间
	WORD							wLength;							//消息长度
	TCHAR							szString[1024];						//选项内容
};

//////////////////////////////////////////////////////////////////////////
// 
//背包物品
struct CMD_CM_PackGoods
{
	WORD							wGoodsCount;						//物品数量
	tagGoodsHoldInfo				GoodsHoldInfo[0];					//物品信息
};

//物品更新
struct CMD_CM_GoodsUpdate
{
	WORD							wGoodsCount;						//物品数量
	tagGoodsHoldInfo				GoodsHoldInfo[0];					//物品信息
};

//财富更新
struct CMD_CM_WealthUpdate
{
	WORD							wItemCount;							//子项数量
	tagWealthItem					WealthItem[0];						//财富子项
};

//魅力更新
struct CMD_CM_LoveLinessUpdate
{
	DWORD							dwLoveLiness;						//魅力值
};

//配置类型定义
#define CM_CONFIG_TYPE_NICKNAME		0x00000001							//昵称修改配置
//配置类型
struct CMD_CM_ConfigUpdate
{
	DWORD							dwType;								//配置类型
};
//昵称修改配置
struct CMD_CM_Config_NickName
{
	WORD							wFreeModifyTimes;					//免费修改次数
	WORD							wDailyModifyTimes;					//每日可修改次数
	DWORD							dwGoodsID;							//消耗物品ID
	DWORD							dwGoodsCount;						//消耗物品数量
};
//////////////////////////////////////////////////////////////////////////

//动作消息
struct CMD_CM_ActionMessage
{
	WORD							wType;								//消息类型
	WORD							wLength;							//消息长度
	UINT							nButtonType;						//按钮类型
	WCHAR							szString[1024];						//消息内容
};

//////////////////////////////////////////////////////////////////////////

//叠加信息
#define DTP_CM_DESCRIBE				1									//描述信息
#define DTP_CM_GOODSLIST			2									//物品列表

//操作代码
#define OFC_SUCCESS					0									//操作成功
#define OFC_FAILURE					1									//操作失败
#define OFC_DB_EXCEPTION			2									//数据库异常
#define OFC_MAIL_NOTEXIST			3									//邮件不存在

//操作失败
struct CMD_CM_OperateFailure
{
	LONG							lErrorCode;							//错误代码
	WORD							wRequestCmdID;						//请求命令
};

//操作成功
struct CMD_CM_OperateSuccess
{
	LONG							lErrorCode;							//操作代码
	WORD							wRequestCmdID;						//请求命令
};

//////////////////////////////////////////////////////////////////////////

//银行存取
struct CMD_CM_BankAccess
{
	DWORD							dwUserID;							//用户ID
	LONGLONG						lAmount;							//金币数量
	BYTE							cbGameAction;						//游戏操作
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码
};

//银行转帐
struct CMD_CM_BankTransfer
{
	DWORD							dwUserID;						//赠送ID
	LONGLONG						lAmount;						//金币数量
	TCHAR							szRecvUser[LEN_ACCOUNTS];		//接受用户
	TCHAR							szPassword[LEN_PASSWORD];		//用户密码
	bool							bIsAccount;						//是否帐号
};

//查询用户
struct CMD_CM_BankQuery
{
	TCHAR							szRecvUser[LEN_ACCOUNTS];		//接受用户
	bool							bIsAccount;						//是否帐号
};

//更新金币
struct CMD_CM_BankUpdate
{
	DWORD							dwUserID;						//用户ID
	TCHAR							szPassword[LEN_PASSWORD];		//用户密码
};

enum BANK_OPERATE_BYTE
{
	BANK_TYPE_STORAGE = 0,
	BANK_TYPE_DRAWOUT,
	BANK_TYPE_TRANSFER,
	BANK_TYPE_QUERY,
	BANK_TYPE_MODIFYPASS,
	BANK_TYPE_UPDATE,
	BANK_TYPE_MAX

};
#define	MIN_BANK_COUNT					10000							//银行最小值

//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif