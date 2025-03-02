#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//请求数据包

//系统命令
#define DBR_LOAD_GLOBAL_CONFIG		1									//加载配置

//查询命令
#define DBR_QUERY_STOCK_ITEM		2									//查询库存
#define DBR_UPDATE_STOCK_SCORE		5									//更新库存
#define DBR_UPDATE_STOCK_STATE		6									//更新状态
#define DBR_UPDATE_JACKPOT_SCORE	7									//更新彩金
#define DBR_ADJUST_STOCK_SCORE		8									//调整库存
#define DBR_ADJUST_JACKPOT_SCORE	9									//调整彩金	

//////////////////////////////////////////////////////////////////////////
//事件标识

//加载配置
struct DBR_LoadGlobalConfig
{
	DWORD							dwConfigMask;						//配置掩码
};

//查询库存
struct DBR_QueryStockItem
{
	WORD							wStockID;							//库存标识
	WORD							wCommandID;							//命令标识
};

//调整库存
struct DBR_AdjustStockScore
{
	WORD							wStockID;							//库存标识
	SCORE							lChangeScore;						//变更分数
};

//调整彩金
struct DBR_AdjustJackpotScore
{
	WORD							wStockID;							//库存标识
	WORD							wLevelID;							//彩金等级
	SCORE							lChangeScore;						//变更分数
};

//////////////////////////////////////////////////////////////////////////
//输出信息

//配置事件
#define DBO_STOCK_LIST_ITEM			100									//库存子项
#define DBO_SUPER_PLAYER_CONTROL	101									//大R控制
#define DBO_LOAD_CONFIG_FINISH		102									//配置完成
#define DBO_QUERY_STOCK_ITEM		103									//查询库存
#define DBO_UPDATE_STOCK_SCORE		104									//更新库存
#define DBO_UPDATE_STOCK_STATE		105									//更新状态
#define DBO_UPDATE_JACKPOT_SCORE	106									//更新彩金	

//列表结果
#define DBO_GAME_KIND_ITEM			120									//类型信息

//反馈命令
#define DBO_OPERATE_SUCCESS			500									//操作成功
#define DBO_OPERATE_FAILURE			501									//操作失败

//////////////////////////////////////////////////////////////////////////////////

//查询结果
struct DBO_QueryStockItem
{
	WORD							wCommandID;							//命令标识
	tagStockItem					StockItem;							//库存子项
};

//游戏种类
struct DBO_GameKind
{
	bool							bNullity;							//禁用标识
	WORD							wModuleID;							//模块标识
	WORD							wTypeID;							//类型索引
	WORD							wJoinID;							//挂接索引
	WORD							wSortID;							//排序索引
	WORD							wKindID;							//名称索引
	WORD							wSupportMask;						//支持掩码
	TCHAR							szKindName[LEN_KIND];				//游戏名字
	TCHAR							szProcessName[LEN_PROCESS];			//进程名字
};

//操作成功
struct DBO_OperateSuccess
{
	LONG							lResultCode;						//结果代码
	WORD							wRequestCmdID;						//请求命令
	TCHAR							szDescribeString[128];				//描述消息	
};

//操作失败
struct DBO_OperateFailure
{
	LONG							lResultCode;						//结果代码
	WORD							wRequestCmdID;						//请求命令
	TCHAR							szDescribeString[128];				//描述消息	
};

//////////////////////////////////////////////////////////////////////////////////

#endif