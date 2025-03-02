#ifndef MONITOR_SERVICE_HEAD_FILE
#define MONITOR_SERVICE_HEAD_FILE

/////////////////////////////////////////////////////////////////////////////////
//MFC文件
#include <afxtempl.h>

//平台定义
#include "..\..\全局定义\Platform.h"

//平台文件
#include "..\..\公共组件\公共服务\CommonServiceHead.h"
#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef MONITOR_SERVICE_CLASS
#ifdef MONITOR_SERVICE_DLL
#define MONITOR_SERVICE_CLASS _declspec(dllexport)
#else
#define MONITOR_SERVICE_CLASS _declspec(dllimport)
#endif
#endif

//模块定义
#ifndef _DEBUG
#define MONITOR_SERVICE_DLL_NAME	TEXT("MonitorService.dll")		//组件名字
#else
#define MONITOR_SERVICE_DLL_NAME	TEXT("MonitorServiceD.dll")		//组件名字
#endif

//////////////////////////////////////////////////////////////////////////////////
//结构定义

//状态变更子项
struct tagStateVariationItem
{
	WORD								wStockID;					//库存标识
	BYTE								cbStockState;				//库存状态
	LONGLONG							lStockControlID;			//控制编号
};

//库存变更子项
struct tagStockVariationItem
{
	WORD								wStockID;					//库存标识
	SCORE								lVariationScore;			//变更积分
	SCORE								lVariationProfit;			//变更利润
};

//库存警报子项
struct tagStockAlarmItem
{
	WORD								wStockID;					//库存标识
	WORD								wKindID;					//类型标识
	SCORE								lBetAmount;					//下注金额
	SCORE								lStockScore;				//当前库存
};

//彩池变更子项
struct tagJackpotVariationItem
{
	WORD								wStockID;					//库存标识
	WORD								wLevelID;					//级别标识
	SCORE								lVariationScore;			//变更积分	
};

//彩池警报子项
struct tagJackpotAlarmItem
{
	WORD								wStockID;					//库存标识
	WORD								wKindID;					//类型标识
	WORD								wLevelID;					//级别标识
	SCORE								lBetAmount;					//下注金额
	SCORE								lStockScore;				//当前库存
};

//////////////////////////////////////////////////////////////////////////////////

//包含文件
#ifndef MONITOR_SERVICE_DLL
	#include "ShareStockManager.h"
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif