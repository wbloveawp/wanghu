#ifndef DLG_STATUS_HEAD_FILE
#define DLG_STATUS_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//状态窗口
class CDlgStatus : public CDialog
{
	//变量定义
protected:
	UINT							m_uCommomID;						//命令标识
	HWND 							m_hNotifyWnd;						//处理窗口

	//控件变量
protected:
	CSkinButton						m_btGiveUp;							//取消按钮
	CProgressCtrl					m_ProgressCtrl;						//进度控件

	//函数定义
public:
	//构造函数
	CDlgStatus();
	//析构函数
	virtual ~CDlgStatus();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK() { return; };
	//取消函数
	virtual VOID OnCancel() { return; };

	//功能函数
public:
	//通知窗口
	HWND GetNotifyWnd() { return m_hNotifyWnd; }
	//设置消息
	VOID ShowStatusWnd(LPCTSTR pszDescribe, HWND hNotifyWnd, UINT uCommomID);

	//按钮消息
protected:
	//取消按钮
	VOID OnBnClickedGiveUp();

	//消息映射
protected:
	//时间消息
	VOID OnTimer(UINT_PTR nIDEvent);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint Point);
	//控件颜色
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif