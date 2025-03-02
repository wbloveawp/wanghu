#ifndef MODULE_MANAGER_HEAD_HEAD_FILE
#define MODULE_MANAGER_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台定义
#include "..\..\全局定义\Platform.h"
#include "..\..\全局定义\RightDefine.h"
#include "..\..\全局定义\ServerRule.h"

//平台文件
#include "..\..\公共组件\公共服务\CommonServiceHead.h"
#include "..\..\服务器组件\游戏服务\GameServiceHead.h"
#include "..\..\服务器组件\内核引擎\KernelEngineHead.h"
#include "..\..\服务器组件\服务框架\ServiceFrameHead.h"
#include "..\..\服务器组件\配置服务\ConfigServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef MODULE_MANAGER_CLASS
	#ifdef MODULE_MANAGER_DLL
		#define MODULE_MANAGER_CLASS _declspec(dllexport)
	#else
		#define MODULE_MANAGER_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define MODULE_MANAGER_DLL_NAME	TEXT("ModuleManager.dll")			//组件 DLL 名字
#else
	#define MODULE_MANAGER_DLL_NAME	TEXT("ModuleManagerD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////
//结构定义

//模块信息
struct tagGameModuleInfo
{
	//模块属性
	WORD							wModuleID;							//模块标识
	WORD							wDBListID;							//数据库索引
	WORD							wSupportType;						//支持类型
	DWORD							dwModuleVersion;					//模块版本
	DWORD							dwNativeVersion;					//本地版本
	DWORD							dwClientVersion;					//客户端版本

	//游戏属性
	TCHAR							szModuleName[LEN_KIND];				//游戏名字

	//文件名字
	TCHAR							szServerDLLName[LEN_PROCESS];		//进程名字
	TCHAR							szClientEXEName[LEN_PROCESS];		//进程名字
};

//////////////////////////////////////////////////////////////////////////

//房间信息
struct tagGameServerInfo
{
	//索引配置
	WORD							wServerID;							//房间标识
	WORD							wModuleID;							//模块标识

	//挂接配置
	WORD							wKindID;							//类型标识
	WORD							wNodeID;							//节点标识
	WORD							wSortID;							//排序标识
	WORD							wVisibleMask;						//可视规则

	//积分配置
	LONG							lRevenue;							//税收比例
	LONG							lExpGainRate;						//经验增益
	SCORE							lCellScore;							//单位积分
	SCORE							lLessScore;							//最低积分
	SCORE							lRestrictScore;						//限制积分
	SCORE							lMinEnterScore;						//最低进入
	SCORE							lMaxEnterScore;						//最高进入
	LONG							lMinEnterMember;					//最低进入会员
	LONG							lMaxEnterMember;					//最高进入会员

	//房间信息
	WORD							wMaxPlayer;							//最大数目
	WORD							wTableCount;						//桌子数目
	WORD							wServerType;						//房间类型
	WORD							wServerPort;						//房间端口
	WORD							wServerLevel;						//房间等级
	DWORD							dwServerRule;						//房间规则
	DWORD							dwAttachUserRight;					//附加权限
	TCHAR							szServerName[LEN_SERVER];			//房间名字
	BYTE							cbMatchType;						//比赛类型
	WORD							wDBListID;							//数据库索引

	//模块属性
	DWORD							dwModuleVersion;					//模块版本
	DWORD							dwNativeVersion;					//本地版本
	DWORD							dwClientVersion;					//客户端版本
	TCHAR							szModuleName[LEN_KIND];				//游戏名字
	TCHAR							szServerDLLName[LEN_PROCESS];		//进程名字
	TCHAR							szClientEXEName[LEN_PROCESS];		//进程名字

	//扩展配置
	BYTE							cbCustomRule[1024];					//自定规则
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//机器序列	

	//约战规则
	DWORD							dwBattleRule;						//约战规则

	//配桌设置
	BYTE							cbDistributeMode;					//分组模式
	BYTE							cbDistributeRule;					//配桌规则
	WORD							wMinPlayerCount;					//玩家人数
	WORD							wMinDistributeUser;					//最少人数
	WORD							wDistributeTimeSpace;				//分组间隔	
	WORD							wMinPartakeGameUser;				//最少人数
	WORD							wMaxPartakeGameUser;				//最多人数

	//服务信息
	TCHAR							szServiceAddr[LEN_ADDRESS];			//服务地址
	TCHAR							szServiceDomain[LEN_DOMAIN];		//服务域名
};

//////////////////////////////////////////////////////////////////////////

//创建信息
struct tagCreateServerInfo
{
	//索引配置
	WORD							wServerID;							//房间标识
	WORD							wModuleID;							//模块标识

	//挂接配置
	WORD							wKindID;							//类型标识
	WORD							wNodeID;							//节点标识
	WORD							wSortID;							//排序标识
	WORD							wVisibleMask;						//可视规则

	//积分配置
	LONG							lRevenue;							//税收比例
	LONG							lExpGainRate;						//经验增益
	SCORE							lCellScore;							//单位积分
	SCORE							lLessScore;							//最低积分
	SCORE							lRestrictScore;						//限制积分
	SCORE							lMinEnterScore;						//最低进入
	SCORE							lMaxEnterScore;						//最高进入
	LONG							lMinEnterMember;					//最低进入会员
	LONG							lMaxEnterMember;					//最高进入会员	

	//房间信息
	WORD							wMaxPlayer;							//最大数目
	WORD							wTableCount;						//桌子数目
	WORD							wServerType;						//房间类型
	WORD							wServerPort;						//房间端口
	WORD							wServerLevel;						//房间等级
	DWORD							dwServerRule;						//房间规则
	DWORD							dwAttachUserRight;					//附加权限
	TCHAR							szServerName[LEN_SERVER];			//房间名字
	BYTE							cbMatchType;						//比赛类型
	WORD							wDBListID;							//数据库索引

	//扩展配置
	BYTE							cbCustomRule[1024];					//自定规则
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//机器序列

	//约战规则
	DWORD							dwBattleRule;						//约战规则

	//配桌设置
	BYTE							cbDistributeMode;					//分组模式
	BYTE							cbDistributeRule;					//配桌规则
	WORD							wMinPlayerCount;					//玩家人数
	WORD							wMinDistributeUser;					//最少人数
	WORD							wDistributeTimeSpace;				//分组间隔	
	WORD							wMinPartakeGameUser;				//最少人数
	WORD							wMaxPartakeGameUser;				//最多人数

	//服务信息
	TCHAR							szServiceAddr[LEN_ADDRESS];			//服务地址
	TCHAR							szServiceDomain[LEN_DOMAIN];		//服务域名
};

//创建信息
struct tagCreateMatchInfo
{
	//标识信息
	WORD							wKindID;							//类型标识
	DWORD							dwClubID;							//茶社标识
	DWORD							dwMatchID;							//比赛标识

	//基本信息	
	BYTE							cbShowType;							//展示类型	
	BYTE							cbMatchType;						//比赛类型
	TCHAR							szMatchName[32];					//比赛名称
	TCHAR							szMatchDate[64];					//比赛时间
	TCHAR							szModuleName[LEN_KIND];				//模块名称

	//报名信息
	SCORE							lFeeAmount;							//费用金额
	WORD							wFeeMoneyID;						//货币标识
	WORD							wUseTicketCount;					//赛券数量
	WORD							wBuyTicketCount;					//赛券数量
	WORD							wBuyTicketOption;					//购票选项
	SCORE							lBuyTicketAmount;					//费用金额
	WORD							wBuyTicketMoneyID;					//货币标识
	WORD							wSignupCount;						//报名次数	
	BYTE							cbSignupMode;						//报名方式
	BYTE							cbJoinCondition;					//参赛条件
	BYTE							cbMemberOrder;						//会员等级
	LONG							lExperience;						//玩家经验

	//配桌配置
	BYTE							cbMatchDistribute[64];				//比赛配桌
	
	//比赛奖励
	WORD							wRewardCount;						//奖励数目
	tagMatchRewardItem				MatchRewardItem[16];				//奖励信息

	//比赛轮次
	WORD							wRoundCount;						//轮次数目
	tagMatchRoundItem				MatchRoundItem[5];					//轮次信息

	//扩展配置
	BYTE							cbMatchRule[256];					//比赛规则	
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//机器序列
};


//////////////////////////////////////////////////////////////////////////

//配置参数
struct tagModuleInitParameter
{
	tagGameServiceAttrib			GameServiceAttrib;					//服务属性
	tagGameServiceOption			GameServiceOption;					//服务配置
};

//////////////////////////////////////////////////////////////////////////

//数据库信息
struct tagDataBaseList
{
	WORD							wDBListID;							//数据列表索引
	WORD							wDBInfoID;							//数据库信息索引
	DWORD							dwDBGenre;							//数据库类型
	TCHAR							szDBName[32];						//数据库名称
	TCHAR							szDBAliasName[32];					//数据库别名
};

//////////////////////////////////////////////////////////////////////////

//导出文件
#ifndef MODULE_MANAGER_DLL 
	#include "ListControl.h"	
	#include "DlgServerWizard.h"
	#include "DlgServerMatch.h"
	#include "DlgMatchWizard.h"
	#include "DlgServiceOption.h"
	#include "ModuleListControl.h"
	#include "ServerListControl.h"
	#include "ServerInfoManager.h"
	#include "MatchInfoManager.h"
	#include "MatchListControl.h"
	#include "ModuleDBParameter.h"
	#include "ModuleInfoManager.h"	
	#include "DataBaseListManager.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif