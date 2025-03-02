#ifndef BATTLE_SERVER_DLG_HEAD_FILE
#define BATTLE_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServiceUnits.h"

//////////////////////////////////////////////////////////////////////////

//主对话框
class CClubServerDlg : public CDialog, public IServiceUnitsSink
{
	//组件变量
protected:
	CRichEditTrace					m_RichEditTrace;			//追踪窗口

	//服务组件
protected:
	CServiceUnits					m_ServiceUnits;				//服务单元	

	//函数定义
public:
	//构造函数
	CClubServerDlg();
	//析构函数
	virtual ~CClubServerDlg();

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

	//辅助函数
protected:
	//设置标题
	VOID SetWindowStatusText(LPCTSTR pszServiceStatus);

	//按钮消息
protected:
	//启动服务
	VOID OnBnClickedStartService();
	//停止服务
	VOID OnBnClickedStopService();
	//配置服务
	VOID OnBnClickedOptionService();

	//消息映射
public:
	//关闭询问
	BOOL OnQueryEndSession();	
	//命令处理
	LRESULT OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif