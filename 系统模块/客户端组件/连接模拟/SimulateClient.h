#pragma once

#include "Stdafx.h"
#include <random>
#include "ModuleLogon.h"
#include "ModuleMatch.h"
#include "ModuleServer.h"
#include "ModuleBattle.h"
#include "ModuleClub.h"
#include "ModuleClubPlaza.h"
#include "SimulateLinkHead.h"
#include "VirtualLinkManager.h"

//////////////////////////////////////////////////////////////////////////

//客户端子项
class CSimulateClient : public CWnd, public IModuleItemSink, public IVirtualManagerSink
{
	//登录信息
protected:
	TCHAR							m_szAccounts[LEN_ACCOUNTS];			//用户账号
	TCHAR							m_szPassword[LEN_PASSWORD];			//用户密码	

	//用户数据
protected:
	tagGlobalUserData				m_GlobalUserData;					//用户资料	

	//组件变量
public:
	CModuleClub						m_ModuleClub;						//茶社模块
	CModuleLogon					m_ModuleLogon;						//登录模块
	CModuleMatch					m_ModuleMatch;						//比赛模块
	CModuleServer					m_ModuleServer;						//房间模块
	CModuleBattle					m_ModuleBattle;						//约战模块
	CModuleClubPlaza				m_ModuleClubPlaza;					//茶社模块
	CVirtualLinkManager				m_VirtualLinkManager;				//链接管理	

	//函数定义
public:
	//构造函数
	CSimulateClient();
	//析构函数
	virtual ~CSimulateClient();

	//网络事件
public:
	//连接通知
	virtual bool OnEventTCPSocketLink(INT nErrorCode);
	//关闭通知
	virtual bool OnEventTCPSocketShut(BYTE cbShutReason);

	//时钟接口
public:
	//结束时钟
	virtual VOID KillGameTimer(UINT uTimerID);
	//设置时钟
	virtual VOID SetGameTimer(UINT uTimerID, DWORD dwElapse);

	//信息接口
public:
	//用户数据
	virtual tagGlobalUserData* GetGlobalUserData() { return &m_GlobalUserData; }

	//模块接口
public:
	//茶社模块
	virtual VOID * GetModuleClub() { return &m_ModuleClub; }
	//房间模块
	virtual VOID * GetModuleServer() { return &m_ModuleServer; }
	//约战模块
	virtual VOID * GetModuleBattle() { return &m_ModuleBattle; }
	//茶社模块
	virtual VOID * GetModuleClubPlaza() { return &m_ModuleClubPlaza; }	

	//功能接口
public:
	//插入消息
	virtual VOID InsertString(LPCTSTR pszString, COLORREF crColor);
	//投递消息
	virtual VOID PostWindowMessage(UINT MsgID, WPARAM wParam, LPARAM lParam);

	//信息函数
public:
	//连接状态
	BYTE GetConnectStatus() { return m_VirtualLinkManager.GetSocketStatus(); }

	//配置函数
public:
	//设置账号
	VOID SetAccountInfo(LPCTSTR pszAccounts, LPCTSTR pszPassword);

	//管理函数
public:
	//关闭客户端
	VOID CloseCleint();
	//启动客户端
	bool StartClient(LPCTSTR pszServerAddr, WORD wServerPort);	

	//消息函数
protected:
	//时间消息
	VOID OnTimer(UINT_PTR nIDEvent);

	//登录消息
protected:
	//链接关闭
	LRESULT OnLogonLinkShut(WPARAM wParam, LPARAM lParam);
	//链接失败
	LRESULT OnLogonLinkFailure(WPARAM wParam, LPARAM lParam);
	//登录完成
	LRESULT OnLogonLogonFinish(WPARAM wParam, LPARAM lParam);
	//登录失败
	LRESULT OnLogonLogonFailure(WPARAM wParam, LPARAM lParam);

	//比赛消息
protected:
	//链接关闭
	LRESULT OnMatchLinkShut(WPARAM wParam, LPARAM lParam);
	//链接失败
	LRESULT OnMatchLinkFailure(WPARAM wParam, LPARAM lParam);
	//登录完成
	LRESULT OnMatchLogonFinish(WPARAM wParam, LPARAM lParam);
	//登录失败
	LRESULT OnMatchLogonFailure(WPARAM wParam, LPARAM lParam);

	//茶社消息
protected:
	//链接关闭
	LRESULT OnClubLinkShut(WPARAM wParam, LPARAM lParam);
	//链接失败
	LRESULT OnClubLinkFailure(WPARAM wParam, LPARAM lParam);
	//登录完成
	LRESULT OnClubLogonFinish(WPARAM wParam, LPARAM lParam);
	//登录失败
	LRESULT OnClubLogonFailure(WPARAM wParam, LPARAM lParam);

	//茶社广场
protected:
	//链接关闭
	LRESULT OnClubPlazaLinkShut(WPARAM wParam, LPARAM lParam);
	//链接失败
	LRESULT OnClubPlazaLinkFailure(WPARAM wParam, LPARAM lParam);
	//登录完成
	LRESULT OnClubPlazaLogonFinish(WPARAM wParam, LPARAM lParam);
	//登录失败
	LRESULT OnClubPlazaLogonFailure(WPARAM wParam, LPARAM lParam);

	//约战消息
protected:
	//链接关闭
	LRESULT OnBattleLinkShut(WPARAM wParam, LPARAM lParam);
	//链接失败
	LRESULT OnBattleLinkFailure(WPARAM wParam, LPARAM lParam);
	//登录完成
	LRESULT OnBattleLogonFinish(WPARAM wParam, LPARAM lParam);
	//登录失败
	LRESULT OnBattleLogonFailure(WPARAM wParam, LPARAM lParam);

	//房间消息
protected:
	//链接关闭
	LRESULT OnServerLinkShut(WPARAM wParam, LPARAM lParam);
	//链接失败
	LRESULT OnServerLinkFailure(WPARAM wParam, LPARAM lParam);
	//登录完成
	LRESULT OnServerLogonFinish(WPARAM wParam, LPARAM lParam);
	//登录失败
	LRESULT OnServerLogonFailure(WPARAM wParam, LPARAM lParam);
	//比赛完成
	LRESULT OnServerMatchCompleted(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
