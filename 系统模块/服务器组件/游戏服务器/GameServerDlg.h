#ifndef GAME_SERVER_DLG_HEAD_FILE
#define GAME_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//主对话框
class CGameServerDlg : public CDialog, public IServiceUnitsSink
{
	//模块参数
public:
	bool							m_bOptionSuccess;					//配置标志	
	tagModuleInitParameter			m_ModuleInitParameter;				//配置参数

	//服务组件
protected:
	CRichEditTrace					m_RichEditTrace;					//追踪窗口
	CGameServiceUnitsHelper			m_ServiceUnitsModule;				//服务单元

	//函数定义
public:
	//构造函数
	CGameServerDlg();
	//析构函数
	virtual ~CGameServerDlg();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual VOID OnOK();
	//取消函数
	virtual VOID OnCancel();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//服务接口
public:
	//服务状态
	virtual VOID OnServiceUnitsStatus(WORD ServiceStatus);

	//按钮消息
protected:
	//启动服务
	VOID OnBnClickedStartService();
	//停止服务
	VOID OnBnClickedConcludeService();
	//打开房间
	VOID OnBnClickedOpenServer();
	//创建房间
	VOID OnBnClickedCreateServer();
	//配置房间
	VOID OnBnClickedOptionServer();
	//服务调试
	VOID OnBnClickedDebugService();

	//辅助函数
protected:
	//更新标题
	VOID UpdateServerTitle();
	//配置成功
	VOID OnLoadModuleParameter();

	//消息映射
protected:
	//关闭询问
	BOOL OnQueryEndSession();
	//命令处理
	LRESULT OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif