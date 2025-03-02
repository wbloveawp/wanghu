#ifndef EXTEND_DATA_BASE_PACKET_HEAD_FILE
#define EXTEND_DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//请求数据包

//约战事件
#define DBR_GR_BATTLE_LOCK			(DB_EXTEND_REQUEST_START+1)			//约战锁定
#define DBR_GR_BATTLE_UNLOCK		(DB_EXTEND_REQUEST_START+2)			//约战解锁
#define DBR_GR_BATTLE_RECORD		(DB_EXTEND_REQUEST_START+3)			//约战记录
#define DBR_GR_BATTLE_REWARD		(DB_EXTEND_REQUEST_START+4)			//约战奖励
#define DBR_GR_DEDUCT_BATTLE_COST	(DB_EXTEND_REQUEST_START+6)			//扣除费用
#define DBR_GR_DEDUCT_BATTLE_COSTAA	(DB_EXTEND_REQUEST_START+7)			//扣除费用
#define DBR_GR_RETURN_BATTLE_COST	(DB_EXTEND_REQUEST_START+8)			//退还费用
#define DBR_GR_RETURN_BATTLE_COSTAA	(DB_EXTEND_REQUEST_START+9)			//退还费用

//////////////////////////////////////////////////////////////////////////

//扣费用户
struct tagBattleUser
{
	DWORD							dwUserID;							//用户标识	
};

//锁定用户
struct DBR_GR_BattleLock
{
	//约战信息
	BYTE							cbScoreKind;						//分数类型
	DWORD							dwBattleNum;						//约战房号

	//用户信息
	WORD							wUserCount;							//用户数量
	DWORD							dwUserID[TABLE_USER_COUNT];			//用户标识
};

//锁定用户
struct DBR_GR_BattleUnlock
{
	//用户信息
	DWORD							dwBattleNum;						//约战房号
	WORD							wUserCount;							//用户数量
	DWORD							dwUserID[TABLE_USER_COUNT];			//用户标识
};

//扣除费用
struct DBR_GR_DeductBattleCost
{
	//用户信息	
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//群组标识
	
	//开桌信息
	WORD							wTableID;							//桌子标识
	WORD							wPayType;							//支付方式
	SCORE							lConsumeCount;						//消耗数量
	BYTE							cbCurrencyKind;						//货币类型	
};

//扣除费用
struct DBR_GR_DeductBattleCostAA
{
	//桌子信息
	WORD							wTableID;							//桌子标识
	DWORD							dwClubID;							//群组标识	
	SCORE							lConsumeCount;						//消耗费用	
	BYTE							cbCurrencyKind;						//货币类型	

	//用户信息
	WORD							wUserCount;							//用户数量
	tagBattleUser					BattleUser[TABLE_USER_COUNT];		//用户信息
};

//退还费用
struct DBR_GR_ReturnBattleCost
{
	//用户信息	
	DWORD							dwUserID;							//用户标识	
	DWORD							dwClubID;							//群组标识
	DWORD							dwBattleNum;						//桌子编号
	LONGLONG						lReturnCount;						//返还数量
	BYTE							cbCurrencyKind;						//货币类型		
};

//解散桌子
struct DBR_GR_DismissTable
{
	//桌子信息
	WORD							wTableID;							//桌子标识
	DWORD							dwBattleNum;						//桌子编号
	DWORD							dwUserID;							//用户标识	
	TCHAR							szPassword[LEN_PASSWORD];			//用户密码	

	//网络信息
	tagSocketInfo					SocketInfo;							//网络信息
};

//约战奖励
struct DBR_GR_BattleReward
{
	DWORD							dwUserID;							//用户标识
	SCORE							lRewardGold;						//金币奖励	
};

//记录战绩
struct DBR_GR_BattleRecord
{
	//桌子信息
	DWORD							dwOwnerID;							//桌主信息
	DWORD							dwBattleNum;						//映射编号	
	bool							bBattleFinish;						//约战完成
	SCORE							lBattleRecordID;					//记录标识

	//群组信息
	DWORD							dwClubID;							//群组标识
	DWORD							dwClubCreaterID;					//群主标识
	
	//创建配置
	DWORD							dwConfigID;							//配置标识
	WORD							wFinishCount;						//完成局数
	WORD							wPlayCount;							//游戏局数
	BYTE							cbScoreKind;						//分数类型
	BYTE							cbSettleKind;						//结算方式	
	DWORD							dwCreateTime;						//创建时间
	BYTE							cbAllowPublic;						//允许公开
	SCORE							lBaseScore;							//基础底分	
	SCORE							lWinnerScore;						//赢家得分
	LONGLONG						lConsumeCount;						//钻石消耗
	BYTE							cbCurrencyKind;						//货币类型

	//支付信息
	WORD							wPayType;							//支付方式
	DWORD							dwPayUserID;						//支付用户

	//游戏配置
	TCHAR							szGameOption[128];					//游戏配置

	//分数信息
	WORD							wUserCount;							//子项数量
	tagBattleUserItem				BattleUserItem[TABLE_USER_COUNT];	//用户子项
};

//////////////////////////////////////////////////////////////////////////
//输出信息

//逻辑事件
#define DBO_GR_DEDUCT_BATTLE_COST	(DB_EXTEND_RESPOND_START+1)			//扣费结果
#define DBO_GR_RETURN_BATTLE_COST	(DB_EXTEND_RESPOND_START+2)			//退费结果
/////////////////////////////////////////////////////////////////////////////////////////

//扣费结果
struct DBO_GR_DeductBattleCost
{
	WORD							wTableID;							//桌子标识	
	DWORD							dwFailUserID;						//用户标识
	
	//财富信息
	SCORE							lUserGold;							//金币数量	
	SCORE							lUserCard;							//房卡数量	
	DWORD							dwPayUserID;						//付费玩家

	//扣费结果
	LONG							lResultCode;						//结果代码		
	WCHAR							szErrorMessage[128];				//错误消息
};

//退费结果
struct DBO_GR_ReturnBattleCost
{
	//创建信息	
	DWORD							dwUserID;							//用户标识
	DWORD							dwClubID;							//群组标识	

	//财富信息 
	SCORE							lUserGold;							//金币数量
	SCORE							lUserCard;							//房卡数量	

	//描述信息
	LONG							lResultCode;						//结果代码
	WCHAR							szErrorMessage[128];				//错误消息
};

//////////////////////////////////////////////////////////////////////////

#endif