#ifndef CMD_VALIDATE_HEAD_FILE
#define CMD_VALIDATE_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//验证命令

#define MDM_VS_VALIDATE				1									//验证命令

//////////////////////////////////////////////////////////////////////////

//验证模式
#define SUB_VS_C_VALIDATE_LOGON		1									//登录验证

//////////////////////////////////////////////////////////////////////////

//验证结果
#define SUB_GP_S_VALIDATE_SUCCESS	100									//验证成功
#define SUB_GP_S_VALIDATE_FAILURE	101									//验证失败

//////////////////////////////////////////////////////////////////////////

//登录验证
struct CMD_VS_C_ValidateLogon
{
	TCHAR							szAccreditID[LEN_ACCREDIT_ID];		//授权号码
	TCHAR							szMachineID[LEN_MACHINE_ID];		//机器序列
};

//////////////////////////////////////////////////////////////////////////

//验证成功
struct CMD_VS_S_ValidateSuccess
{
	//协调地址
	DWORD							dwCenterServerAddr;					//协调地址

	//连接信息
	tagDataBaseParameter			MessageDBParameter;					//连接信息
	tagDataBaseParameter			PlatformDBParameter;				//连接信息
	tagDataBaseParameter			AccountsDBParameter;				//连接信息	
	tagDataBaseParameter			GameGoldDBParameter;				//连接信息
	tagDataBaseParameter			GameMatchDBParameter;				//连接信息
	tagDataBaseParameter			GameBattleDBParameter;				//连接信息
	tagDataBaseParameter			GameRankingDBParameter;				//连接信息
};

//验证失败
struct CMD_VS_S_ValidateFailure
{
	LONG							lErrorCode;							//错误代码
	TCHAR							szDescribeString[128];				//错误消息
};

//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif