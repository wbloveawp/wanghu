#ifndef GATE_SERVER_DLG_HEAD_FILE
#define GATE_SERVER_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ServiceUnits.h"

//////////////////////////////////////////////////////////////////////////

//主对话框
class CGateServerDlg : public CDialog, public IServiceUnitsSink
{
	//组件变量
protected:
	CRichEditTrace					m_RichEditTrace;					//追踪窗口

	//服务组件
protected:	
	CServiceUnits *					m_pServiceUnits;					//服务单元 

	//函数定义
public:
	//构造函数
	CGateServerDlg();
	//析构函数
	virtual ~CGateServerDlg();

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
	//加载配置
	VOID OnBnClickedLoadOption();
	//设置标题
	VOID SetWindowStatusText(LPCTSTR pszServiceStatus);

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