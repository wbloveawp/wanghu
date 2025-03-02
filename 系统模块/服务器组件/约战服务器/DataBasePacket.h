#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//请求数据包

//系统命令
#define DBR_GR_LOAD_BATTLE_OPTION		1								//约战选项	


//////////////////////////////////////////////////////////////////////////
//输出信息

//加载结果
#define DBO_GR_BATTLE_BASE_OPTION		100								//游戏选项
#define DBO_GR_BATTLE_GAME_OPTION		101								//创建选项
#define DBO_GR_BATTLE_GLOBAL_OPTION		102								//全局选项	
#define DBO_GR_BATTLE_OPTION_FINISH		110								//配置完成

//操作结构
#define DBO_GR_OPERATE_FAILURE			300								//操作失败

//////////////////////////////////////////////////////////////////////////////////

//操作失败
struct DBO_GR_OperateFailure
{
	//请求信息
	WORD							wRequestCmdID;						//请求命令

	//错误信息
	LONG							lErrorCode;							//失败编码
	WCHAR							szErrorMessage[128];				//错误消息	
};

//////////////////////////////////////////////////////////////////////////////////

#endif