#ifndef OPTION_PARAMETER_HEAD_FILE
#define OPTION_PARAMETER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "GlobalServer.h"
#include "PlatformDataHead.h"

//////////////////////////////////////////////////////////////////////////
//注册表项

//系统信息
#define REG_USER_INFO				TEXT("UserInfo")					//用户信息
#define REG_GAME_OPTION				TEXT("ServerOption")				//游戏信息

//配置信息
#define REG_GOBAL_OPTION			TEXT("Option\\Golbal")				//全局信息
#define REG_INTERFACE_INFO			TEXT("Option\\Interface")			//界面信息
#define REG_PROXY_SERVER_INFO		TEXT("Option\\ProxyServer")			//代理信息

//////////////////////////////////////////////////////////////////////////
//枚举定义

//邀请模式
enum enInviteMode
{
	InviteMode_All,					//全部显示
	InviteMode_None,				//全不显示
	InviteMode_Friend,				//好友显示
};

//登录模式
enum enLogonMode
{
	LogonMode_GameID,				//I D 登录
	LogonMode_Accounts,				//帐号登录
	LogonMode_BBS_Accounts,			//论坛帐号
};

//消息模式
enum enMessageMode
{
	MessageMode_ShowAll,			//全部显示
	MessageMode_HideDetest,			//屏蔽厌恶
	MessageMode_OnlyFriend,			//社团好友
	MessageMode_HideAll,			//屏蔽所有
};

//帐户规则
enum enAcountsRule
{
	AcountsRule_None,				//不作记录
	AcountsRule_Accounts,			//记住帐户
	AcountsRule_AccountsAndPass,	//帐户密码
};

//界面动作
enum enInterfaceAction
{
	//列表动作
	InterfaceAction_SendWhisper,	//发送私聊
	InterfaceAction_Orientation,	//定位用户
	InterfaceAction_ShowUserInfo,	//用户信息

	//加入动作
	InterfaceAction_SearchTable,	//寻找位置
	InterfaceAction_ShowSearchDlg,	//查找界面
};

//////////////////////////////////////////////////////////////////////////

//游戏参数
class PLATFORM_DATA_CLASS CParameterGame
{
	//友元定义
	friend class CParameterGlobal;

	//胜率限制
public:
	WORD							m_wLessWinRate;						//最低胜率
	bool							m_bLimitWinRate;					//限制胜率

	//逃率限制
public:
	WORD							m_wMaxFleeRate;						//最高逃跑
	bool							m_bLimitFleeRate;					//限制断线

	//积分限制
public:
	LONG							m_lMaxGameScore;					//最高分数 
	LONG							m_lLessGameScore;					//最低分数
	bool							m_bLimitGameScore;					//限制分数
	DWORD							m_dwServerRule;						//桌子规则

	//属性变量
protected:
	TCHAR							m_szRegKeyName[16];					//注册项名

	//函数定义
public:
	//构造函数
	CParameterGame();
	//析构函数
	virtual ~CParameterGame();

	//功能函数
public:
	//加载参数
	VOID LoadParameter();
	//保存参数
	VOID SaveParameter();
	//默认参数
	VOID DefaultParameter();

	//配置函数
protected:
	//配置参数
	bool InitParameter(LPCTSTR pszProcessName);
};

//////////////////////////////////////////////////////////////////////////

//房间参数
class PLATFORM_DATA_CLASS CParameterServer
{
	//配置变量
public:
	bool							m_bTakePassword;					//密码标志
	TCHAR							m_szPassword[LEN_PASSWORD];			//桌子密码

	//函数定义
public:
	//构造函数
	CParameterServer();
	//析构函数
	virtual ~CParameterServer();

	//配置函数
public:
	//默认参数
	VOID DefaultParameter();
};

//////////////////////////////////////////////////////////////////////////

//数组定义
typedef CMap<WORD,WORD,CParameterGame *,CParameterGame *> CParameterGameMap;
typedef CMap<WORD,WORD,CParameterServer *,CParameterServer *> CParameterServerMap;

//全局参数
class PLATFORM_DATA_CLASS CParameterGlobal
{
	//登录配置
public:
	enLogonMode						m_LogonMode;						//登录模式
	enAcountsRule					m_AcountsRule;						//帐户规则
	enNetworkKind					m_NetworkKind;						//网络类型

	//房间配置
public:
	enInviteMode					m_InviteMode;						//邀请模式
	enMessageMode					m_MessageMode;						//消息模式
	enInterfaceAction				m_ActionHitAutoJoin;				//自动加入
	enInterfaceAction				m_ActionLeftDoubleList;				//双击列表

	//时间配置
public:
	WORD							m_wIntermitTime;					//中断时间
	WORD							m_wOnLineCountTime;					//人数时间

	//房间配置
public:
	bool							m_bLimitDetest;						//限制厌恶
	bool							m_bLimitSameIP;						//限制地址
	bool							m_bNotifyUserInOut;					//进出消息
	bool							m_bNotifyFriendCome;				//好友提示

	//界面配置
public:
	bool							m_bSutraView;						//经典视图
	bool							m_bSalienceTable;					//突出桌子
	bool							m_bFullScreenBrowse;				//全屏浏览

	//系统配置
public:
	bool							m_bAllowSound;						//允许声音
	bool							m_bAutoSitDown;						//自动坐下
	bool							m_bAutoShowWhisper;					//显示私聊
	bool							m_bSaveWhisperChat;					//保存私聊
	bool							m_bSendWhisperByEnter;				//回车发送
	bool							m_bShowSafePrompt;					//安全设置

	//热键配置
public:
	WORD							m_wBossHotKey;						//老板热键
	WORD							m_wWhisperHotKey;					//私聊热键

	//颜色配置
public:
	COLORREF						m_crChatTX;							//聊天颜色

	//界面模块
public:
	TCHAR							m_szResourceModule[32];				//资源模块
	TCHAR							m_szSkinDirectory[MAX_PATH];		//资源目录

	//代理配置
public:
	enProxyServerType				m_ProxyServerType;					//代理类型
	tagProxyServerInfo				m_ProxyServerInfo;					//代理信息

	//机器信息
public:
	//变量定义
	TCHAR							m_szMachineID[LEN_MACHINE_ID];  //原始机器
	TCHAR							m_szXorMachineID[LEN_MACHINE_ID *XOR_TIMES]; //异或机器

	//配置组件
protected:
	CParameterGameMap				m_ParameterGameMap;					//游戏配置
	CParameterServerMap				m_ParameterServerMap;				//房间配置

	//静态变量
protected:
	static CParameterGlobal *		m_pParameterGlobal;					//全局配置

	//函数定义
public:
	//构造函数
	CParameterGlobal();
	//析构函数
	virtual ~CParameterGlobal();

	//配置函数
public:
	//加载参数
	VOID LoadParameter();
	//保存参数
	VOID SaveParameter();
	//默认参数
	VOID DefaultParameter();

	//游戏配置
public:
	//游戏配置
	CParameterGame * GetParameterGame(tagGameKind * pGameKind);
	//房间配置
	CParameterServer * GetParameterServer(tagPCGameServer * pGameServer);

	//静态函数
public:
	//获取对象
	static CParameterGlobal * GetInstance() { return m_pParameterGlobal; }
};

//////////////////////////////////////////////////////////////////////////

#endif