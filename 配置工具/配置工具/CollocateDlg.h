#ifndef COLLOCATE_DLG_HEAD_FILE
#define COLLOCATE_DLG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgTabViewItem1.h"

//////////////////////////////////////////////////////////////////////////////////

//配置窗口
class CCollocateDlg : public CDialog
{
	//组件变量
protected:
	CInsideThread					m_InsideThread;						//内部线程

	//控件变量
protected:
	CTabCtrl						m_TabControl;						//页面控件
	CDlgTabViewItem1				m_DlgTabViewItem1;					//页面窗口

	//函数定义
public:
	//构造函数
	CCollocateDlg();
	//析构函数
	virtual ~CCollocateDlg();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//进入函数
	virtual BOOL OnInitDialog();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//消息映射
public:
	//时间消息
	VOID OnTimer(UINT_PTR nIDEvent);
	//类型改变
	VOID OnTcnSelchangeTabControl(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif