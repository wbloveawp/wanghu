#ifndef GAME_DLG_CUSTOM_RULE_HEAD_FILE
#define GAME_DLG_CUSTOM_RULE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "TableFrameSink.h"

//////////////////////////////////////////////////////////////////////////

//配置窗口
class CDlgCustomRule : public CDialog
{
	//配置变量
protected:
	WORD							m_wCustomSize;						//配置大小

	//配置结构
protected:
	tagCustomConfig					m_CustomConfig;						//自定配置

	//函数定义
public:
	//构造函数
	CDlgCustomRule();
	//析构函数
	virtual ~CDlgCustomRule();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//功能函数
public:
	//设置配置
	bool SetCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);
	//保存数据
	bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);

	//默认配置
	bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustomSize);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif