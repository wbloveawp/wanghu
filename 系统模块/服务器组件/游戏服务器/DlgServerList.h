#ifndef DLG_SERVER_LIST_HEAD_FILE
#define DLG_SERVER_LIST_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameServerDlg.h"

//////////////////////////////////////////////////////////////////////////

//主对话框
class CDlgServerList : public CDialog
{
	//列表变量
protected:
	CServerInfoBuffer				m_ServerInfoBuffer;					//模块信息
	CServerListControl				m_ServerListControl;				//房间列表

	//配置变量
public:
	bool							m_bOptionSuccess;					//配置标志
	tagModuleInitParameter			m_ModuleInitParameter;				//配置参数

	//函数定义
public:
	//构造函数
	CDlgServerList();
	//析构函数
	virtual ~CDlgServerList();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定消息
	virtual VOID OnOK();

	//功能函数
protected:
	//加载房间
	bool LoadDBServerItem();
	//初始化房间
	bool InitGameServer(tagGameServerInfo *pGameServerInfo);

	//控制函数
public:
	//加载指定房间
	bool LoadGameServer(WORD wServerID);

	//按钮消息
protected:
	//加载房间
	VOID OnBnClickedLoadServer();
	//刷新列表
	VOID OnBnClickedReLoadList();
	//删除房间
	VOID OnBnClickedDeleteServer();
	//组件属性
	VOID OnBnClickedServiceAttrib();

	//控件消息
protected:
	//双击列表
	VOID OnNMDblclkServerList(NMHDR * pNMHDR, LRESULT * pResult);
	//选择改变
	VOID OnLvnItemChangedServerList(NMHDR * pNMHDR, LRESULT * pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif