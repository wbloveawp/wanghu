#ifndef DLG_TABVIEW_ITEM2_HEAD_FILE
#define DLG_TABVIEW_ITEM2_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////

//页面窗口
class CDlgTabViewItem1 : public CDialog
{
	//组件变量
protected:
	CRichEditTrace					m_TraceServiceControl;				//追踪窗口

	//验证变量
protected:
	CValidateDataHelper				m_ValidateDataModule;				//验证数据
	CValidateModuleHelper			m_ValidateServiceModule;			//验证服务

	//函数定义
public:
	//构造函数
	CDlgTabViewItem1();
	//析构函数
	virtual ~CDlgTabViewItem1();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//配置函数
	virtual BOOL OnInitDialog();

	//功能函数
protected:
	//更新证书
	VOID SetControlStatus(tagValidateInfo & ValidateInfo);
	//获取信息
	bool GetClientAccounts(tagClientAccounts & ClientAccounts);

	//按钮消息
protected:
	//创建授权
	VOID OnBnClickedCreateAccredit();
	//注销授权
	VOID OnBnClickedDeleteAccredit();
	//授权验证
	VOID OnBnClickedValidateAccredit();
	//加载证书
	VOID OnBnClickedReLoadValidateInfo();
	//进入系统
	VOID OnBnClickedEnterProductManager();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////

#endif