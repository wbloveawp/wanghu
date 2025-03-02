#ifndef DLG_SERVER_MATCH_HEAD_FILE
#define DLG_SERVER_MATCH_HEAD_FILE

#pragma once

//引入文件
#include "MatchListControl.h"
#include "MatchInfoManager.h"
#include "ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////////////////////////////

//主对话框
class MODULE_MANAGER_CLASS CDlgServerMatch : public CDialog
{
	//列表变量
protected:
	CMatchInfoBuffer				m_MatchInfoBuffer;						//比赛信息
	CMatchListControl				m_MatchListControl;						//比赛列表

	//配置变量
public:	
	tagMatchOption				m_MatchOption;						//比赛配置

	//函数定义
public:
	//构造函数
	CDlgServerMatch();
	//析构函数
	virtual ~CDlgServerMatch();

	//继承函数
protected:
	//初始窗口
	virtual BOOL OnInitDialog();
	//数据交换
	virtual void DoDataExchange(CDataExchange* pDX);

	//配置函数
public:
	//打开比赛
	bool OpenGameMatch();

	//功能函数
protected:
	//加载比赛
	bool LoadDBMatchItem();

	//按钮消息
protected:	
	//配置比赛
	VOID OnBnClickedOptionMatch();
	//删除比赛
	VOID OnBnClickedDeleteMatch();
	//创建比赛
	VOID OnBnClickedCreateMatch();

	//控件消息
protected:
	//双击列表
	VOID OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult);
	//选择改变
	VOID OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////////////////////////////
#endif