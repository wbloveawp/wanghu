#ifndef SIMULATE_LINK_HEAD_HEAD_FILE
#define SIMULATE_LINK_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//平台定义
#include "..\..\全局定义\Platform.h"

//消息定义
#include "..\..\消息定义\CMD_Commom.h"
#include "..\..\消息定义\CMD_GateServer.h"
#include "..\..\消息定义\CMD_GameServer.h"
#include "..\..\消息定义\CMD_LogonServer.h"
#include "..\..\消息定义\CMD_BattleServer.h"
#include "..\..\消息定义\CMD_RankingServer.h"
#include "..\..\消息定义\CMD_MatchServer.h"
#include "..\..\消息定义\CMD_ClubServer.h"
#include "..\..\消息定义\CMD_ClubPlaza.h"
#include "..\..\消息定义\CMD_MessageServer.h"

//平台文件
#include "..\..\服务器组件\游戏服务\GameServiceHead.h"
#include "..\..\公共组件\公共服务\CommonServiceHead.h"
#include "..\..\客户端组件\网络服务\NetworkServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//时间标识

//初始标识
#define IDI_MODULE_START			100   								//起始标识

//登录模块
#define IDI_MODULE_LOGON_START		IDI_MODULE_START+1   				//起始标识
#define IDI_MODULE_LOGON_RANGE		IDI_MODULE_START+99					//标识范围

//房间模块
#define IDI_MODULE_SERVER_START		IDI_MODULE_START+100   				//起始标识
#define IDI_MODULE_SERVER_RANGE		IDI_MODULE_START+199				//标识范围

//约战模块
#define IDI_MODULE_BATTLE_START		IDI_MODULE_START+200   				//起始标识
#define IDI_MODULE_BATTLE_RANGE		IDI_MODULE_START+299				//标识范围

//比赛模块
#define IDI_MODULE_MATCH_START		IDI_MODULE_START+300   				//起始标识
#define IDI_MODULE_MATCH_RANGE		IDI_MODULE_START+399				//标识范围

//排位模块
#define IDI_MODULE_RANKING_START	IDI_MODULE_START+400   				//起始标识
#define IDI_MODULE_RANKING_RANGE	IDI_MODULE_START+499				//标识范围

//茶社模块
#define IDI_MODULE_CLUB_START		IDI_MODULE_START+500   				//起始标识
#define IDI_MODULE_CLUB_RANGE		IDI_MODULE_START+599				//标识范围

//广场模块
#define IDI_MODULE_CLUBPLAZA_START	IDI_MODULE_START+600   				//起始标识
#define IDI_MODULE_CLUBPLAZA_RANGE	IDI_MODULE_START+699				//标识范围

//////////////////////////////////////////////////////////////////////////////////
//结构定义

//数据包
struct tagDataPacket
{
	WORD							wMainCmdID;							//主命令码
	WORD							wSubCmdID;							//子命令码
	WORD							wDataSize;							//数据大小
	BYTE							cbDataBuffer[SOCKET_BUFFER-12];		//数据缓冲
};

//用户信息
struct tagGlobalUserData
{
	WORD							wFaceID;							//头像标识
	BYTE							cbGender;							//用户性别
	DWORD							dwCustomID;							//自定头像
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	TCHAR							szNickName[LEN_ACCOUNTS];			//用户昵称
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szLogonPass[LEN_PASSWORD];			//登录密码
	TCHAR							szInsurePass[LEN_PASSWORD];			//银行密码

	//经验等级
	DWORD							dwExperience;						//经验数值

	//会员资料
	WORD							wMasterOrder;						//管理等级
	DWORD							dwMemberPoint;						//会员点数
	DWORD							dwMemberOverTime;					//到期时间

	//财富信息
	LONGLONG						lGameScore;							//游戏积分
	LONGLONG						lUserGold;							//用户黄金
	LONGLONG						lUserCard;							//用户钻石	
	LONGLONG						lUserInsure;						//用户银行	

	//行为掩码
	DWORD							dwActionMaskEver;					//行为掩码
	DWORD							dwActionMaskPerDay;					//行为掩码
	DWORD							dwActionMaskPerWeek;				//行为掩码

	//上级信息
	DWORD							dwParentGameID;						//用户 I D
	WORD							wParentFaceID;						//头像ID
	TCHAR							szParentNickName[LEN_ACCOUNTS];		//用户昵称

	//附加信息	
	DWORD							dwLikedNumber;						//被赞数目	

	//锁定信息
	WORD							wLockServerID;						//锁定房间

	//扩展信息
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//手机号码
	TCHAR							szLogonToken[LEN_LOGON_TOKEN];		//登录令牌

	//状态信息
	WORD							wTableID;							//桌子索引
	WORD							wChairID;							//椅子位置
	BYTE							cbUserStatus;						//用户状态

	WORD GetChairID() { return wChairID; }
	SCORE GetUserScore() { return lGameScore; }
};

//////////////////////////////////////////////////////////////////////////////////
//接口定义

//模块子项
interface IModuleItem
{
	//信息函数
public:
	//时钟标识
	virtual DWORD GetTimerStart() = NULL;
	//时钟范围
	virtual DWORD GetTimerRange() = NULL;

	//功能函数
public:
	//关闭时钟
	virtual bool KillModuleTimer(UINT uTimerID)=NULL;
	//设置时钟
	virtual bool SetModuleTimer(UINT uTimerID, UINT nElapse)=NULL;

	//时钟事件
public:
	//结束时钟
	virtual bool OnEventTimer(UINT uTimerID, WPARAM wBindParam) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////
//接口定义

//模块回调接口
interface IModuleItemSink
{
	//时钟接口
public:
	//结束时钟
	virtual VOID KillGameTimer(UINT uTimerID)=NULL;
	//设置时钟
	virtual VOID SetGameTimer(UINT uTimerID, DWORD dwElapse)=NULL;	

	//信息接口
public:
	//用户数据
	virtual tagGlobalUserData* GetGlobalUserData() = NULL;

	//模块接口
public:
	//茶社模块
	virtual VOID * GetModuleClub() = NULL;
	//房间模块
	virtual VOID * GetModuleServer() = NULL;
	//约战模块
	virtual VOID * GetModuleBattle() = NULL;
	//茶社模块
	virtual VOID * GetModuleClubPlaza() = NULL;

	//功能接口
public:
	//插入消息
	virtual VOID InsertString(LPCTSTR pszString, COLORREF crColor)=NULL;
	//投递消息
	virtual VOID PostWindowMessage(UINT MsgID, WPARAM wParam, LPARAM lParam) = NULL;	
};

//////////////////////////////////////////////////////////////////////////////////

#endif