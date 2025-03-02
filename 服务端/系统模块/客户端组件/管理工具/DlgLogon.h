#ifndef DLG_LOGON_HEAD_FILE
#define DLG_LOGON_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "SessionLogon.h"

//////////////////////////////////////////////////////////////////////////

//登录 TAB 控件
class CTabCtrlLogon : public CTabCtrl
{
	//函数定义
public:
	//构造函数
	CTabCtrlLogon();
	//析够函数
	virtual ~CTabCtrlLogon();

	//消息函数
protected:
	//重画函数	
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

//登录窗口
class CDlgLogon : public CSkinDialog
{
	//登录信息
public:
	DWORD							m_dwGameID;							//游戏 I D
	TCHAR							m_szAccounts[LEN_ACCOUNTS];			//游戏帐号
	TCHAR							m_szPassword[LEN_PASSWORD];			//用户密码
	TCHAR							m_szLogonServer[LEN_SERVER];		//登录地址

	//状态变量
protected:
	bool							m_bEditPass;						//改变标志
	bool							m_bEditAccount;						//改变标志
	bool							m_bEditUserId;						//改变标志
	bool							m_bProxyMode;						//网络模式
	BYTE							m_cbVerifyMode;						//验证模式

	//模式变量
protected:
	enLogonMode						m_LogonMode;						//登录模式
	enNetworkKind					m_NetworkKind;						//地址模式

	//位置变量
protected:
	INT								m_nFullWidth;						//全部宽度
	INT								m_nFullHeight;						//全部高度
	CRect							m_rcNormalFrame;					//框架位置

	//控件变量
protected:
	CSkinButton						m_btLogon;							//登录按钮
	CSkinButton						m_btProxyTest;						//测试按钮
	CSkinButton						m_btProxyOption;					//网络按钮

	//编辑控件
protected:
	CSkinEdit						m_edPassword;						//用户密码
	CSkinEdit						m_edProxyPort;						//代理端口
	CSkinEdit						m_edProxyServer;					//代理地址
	CSkinEdit						m_edProxyUserName;					//代理用户
	CSkinEdit						m_edProxyPassword;					//代理密码
	CSkinComboBox					m_cmProxyServerType;				//代理类型

	//其他控件
protected:
	CSkinComboBox					m_cmGameID;							//游戏 I D
	CSkinComboBox					m_cmAccounts;						//登录帐号
	CSkinComboBox					m_cmLogonServer;					//登录地址

	//控件变量
protected:
	TCHAR							m_szAccountTitle[LEN_ACCOUNTS];		//帐号标题

	//函数定义
public:
	//构造函数
	CDlgLogon();
	//析构函数
	virtual ~CDlgLogon();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();
	//初始化函数
	virtual BOOL OnInitDialog();
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//功能函数
public:
	//真实密码
	LPCTSTR GetPassword() { return m_szPassword; }
	//玩家帐号
	LPCTSTR GetAccounts() { return m_szAccounts; }
	//登录模式
	enLogonMode GetLogonMode() { return m_LogonMode; }
	//构造数据
	WORD ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize);
	//设置帐号
	VOID SetAccounts(LPCTSTR lpszAccounts);
	//删除帐号
	VOID DeleteAccounts(LPCTSTR lpszAccounts);

	//模式函数
private:
	//代理模式
	VOID SwitchProxyMode(bool bProxyMode);
	//切换模式
	VOID SwitchLogonMode(enLogonMode LogonMode);

	//辅助函数
private:
	//获取信息
	bool GetInformation();
	//帐号信息
	VOID LoadAccountsInfo();
	//地址信息
	VOID LoadLogonServerInfo();
	//代理信息
	VOID LoadProxyServerInfo();
	//设置选择
	VOID SetCurrentUserID(DWORD dwUserID);
	//更新密码
	VOID UpdateUserPassWord(DWORD dwUserID);
	//查找字符
	INT ComboBoxFindString(CComboBox * pComboBox, LPCTSTR pszFindString);

	//消息函数
protected:
	//选择改变
	VOID OnSelchangeUserID();
	//选择改变
	VOID OnSelchangeAccounts();
	//密码改变
	VOID OnEnChangePassword();
	//输入改变
	VOID OnEditChangeAccounts();
	//输入改变
	VOID OnEditChangeUserID();
	//下拉列表
	VOID OnCBDrawDownAccounts();
	//下拉列表
	VOID OnCBDrawDownGameID();

	//按钮消息
protected:
	//删除用户
	VOID OnBnClickedDelete();
	//代理测试
	VOID OnBnClickedProxyTest();
	//网络设置
	VOID OnBnClickedProxyOption();
	//记住密码
	VOID OnBnClickedRemPassword();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif