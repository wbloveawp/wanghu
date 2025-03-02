#ifndef DLG_ADD_MATCH_DATE_HEAD_FILE
#define DLG_ADD_MATCH_DATE_HEAD_FILE

#pragma once

#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////

//添加比赛日期类
class MODULE_MANAGER_CLASS CDlgAddMatchDate : public CDialog
{
	//定义变量
public:
	CTime					m_TimeBegin;				//开始时间
	CTime					m_TimeEnd;					//结束时间

	//控件变量
protected:
	CDateTimeCtrl			m_CtrlDateBegin;			//开始日期
	CDateTimeCtrl			m_CtrlTimeBegin;			//开始时间
	CDateTimeCtrl			m_CtrlDateEnd;				//结束日期
	CDateTimeCtrl			m_CtrlTimeEnd;				//结束时间

	//函数定义
public:
	//构造函数
	CDlgAddMatchDate();   
	//析构函数
	virtual ~CDlgAddMatchDate();

	//重载函数
protected:
	//数据交换
	virtual void DoDataExchange(CDataExchange* pDX); 
	//初始化
	virtual BOOL OnInitDialog();
	//确定函数
	VOID OnOK();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif