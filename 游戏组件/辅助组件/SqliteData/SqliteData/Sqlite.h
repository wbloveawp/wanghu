
#ifndef SQLITE_HEAD_FILE
#define SQLITE_HEAD_FILE

#pragma pack(1)
//////////////////////////////////////////////////////////////////////////////////

#define	CONTROL_CNT						200					//控制局数
//新手信息
struct tagNewUserControl
{
	DWORD							dwUserId;							 //用户ID
	DWORD							dwGameId;							 //用户ID
	WORD							wKindID;						//游戏ID
	BYTE							cbIndex;						//样本进行到的位置
	WORD							wRebateRatio;					//返奖率（%）
	DWORD							wSelectSampleIndex;				//选择样本索引
	WORD							wTimesCount;					//倍数数目
	WORD							wTimesList[CONTROL_CNT];		//倍数列表
};

//样本存储
struct tagSampleData
{
	int								dwID;							//标识ID
	WORD							wKindID;						//游戏ID
	CHAR							cbBuffer[512];					//样本数据
};
//////////////////////////////////////////////////////////////////////////////////

static int  CB_sqlite_count(void* data, int argc, char** argv, char** pszColName)
{
	for (int i = 0; i < argc; i++)
	{
		if (argv == NULL)
		{
			return 0;
		}
		*((int*)data) = atoi(argv[i]);
	}
	return 0;
}

//库存
struct tagInventoryInfo
{
	INT								nKindID;							//游戏ID
	INT								nDebugType;							//控制类型
	INT								nGameID;							//用户ID/索引

	SCORE							lConfigSysStorage;					//配置系统库存(系统要赢的钱)
	SCORE							lConfigPlayerStorage;				//配置玩家库存(玩家要赢的钱)
	INT								lConfigParameterK;					//配置调节系数(千分比)
	INT								lConfigResetSection;				//强制重置区间(千分比)

	SCORE							lCurSysStorage;						//当前系统库存(系统要赢的钱)
	SCORE							lCurPlayerStorage;					//当前玩家库存(玩家要赢的钱)
	INT								lCurParameterK;						//当前调节系数(百分比):
	INT								lCurResetSection;					//强制重置区间(百分比)

	SCORE							lSysDbgSysWin;						//系统输赢总数
	SCORE							lSysDbgPlayerWin;					//玩家输赢总数
	INT								lWinRatio;							//当前输赢概率
	INT								lResetTimes;						//重置次数
};

#pragma pack()

#endif