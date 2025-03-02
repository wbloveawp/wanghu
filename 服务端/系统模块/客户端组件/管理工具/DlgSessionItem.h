#ifndef DLG_MANAGER_OPTION_HEAD_FILE
#define DLG_MANAGER_OPTION_HEAD_FILE

#pragma once

#include "CheckComboBox.h"
#include "VirtualLinkManager.h"

//////////////////////////////////////////////////////////////////////////
//颜色定义
#define CR_NORMAL_TX				RGB(0,0,0)
#define CR_SELECT_TX				RGB(128,64,0)	
#define CR_CONTROL_BACK				RGB(240,240,240)		
#define CR_CONTROL_BORAD			RGB(0,0,0)

//////////////////////////////////////////////////////////////////////////

//类型声明
class CDlgSessionItem;
class CDlgSessionOption;
class CDlgSessionCommand;

//////////////////////////////////////////////////////////////////////////

//绘画子项
class CDlgSessionItem : public CDialog, public CSessionItem
{
	//变量定义
protected:
	CServerListManager*				m_pServerListManager;				//列表管理

	//函数定义
protected:
	//构造函数
	CDlgSessionItem(UINT nIDTemplate);
	//析构函数
	virtual ~CDlgSessionItem();

	//重载函数
protected:
	//确定函数
	virtual VOID OnOK() { return; }
	//取消消息
	virtual VOID OnCancel() { return; }

	//配置函数
public:
	//列表管理
	VOID SetServerListManager(CServerListManager* pServerListManager) { m_pServerListManager=pServerListManager;}

	//通知事件
public:
	//列表完成
	virtual VOID OnEventListFinish() {};

	//功能函数
public:
	//切换子项
	VOID SwitchViewItem(bool bActive);
	//创建子项
	VOID CreateViewItem(const CRect & rcRect, CWnd * pParentWnd);	

	//回调接口
public:
	//连接事件
	virtual bool OnEventMissionLink(LONG lErrorCode) { return true; }
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason) { return true; }
	//读取事件
	virtual bool OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize) { return true; }	

	//消息函数
private:
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);
	//创建颜色
	HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//选项会话
class CDlgSessionOption : public CDlgSessionItem
{
	//变量定义
protected:
	tagModifyOption					m_ModifyOption;						//编辑选项

	//控件变量
protected:
	CCheckComboBox					m_cbGameList;						//游戏列表
	CCheckComboBox					m_cbServerList;						//房间列表

	//函数定义
public:
	//构造函数
	CDlgSessionOption();
	//析构函数
	virtual ~CDlgSessionOption();

	//重载函数
protected:
	//初始窗口
	virtual BOOL OnInitDialog();
	//交换数据
	virtual VOID DoDataExchange(CDataExchange* pDX);	
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//通知事件
public:
	//列表完成
	virtual VOID OnEventListFinish();

	//回调接口
public:
	//连接事件
	virtual bool OnEventMissionLink(LONG lErrorCode) { return true; }
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason) { return true; }
	//读取事件
	virtual bool OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize);

	//辅助函数
private:
	//控件使能
	VOID EnableControls(BOOL bEnabled);

	//事件函数
protected:
	//编辑子项
	VOID OnBnClickedBtModify();

	//消息函数
protected:
	//销毁窗口
	VOID OnDestroy();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//命令会话
class CDlgSessionCommand : public CDlgSessionItem
{
	//变量定义
protected:
	tagExecuteCommand					m_CommandCode;						//命令编码

	//函数定义
public:
	//构造函数
	CDlgSessionCommand();
	//析构函数
	virtual ~CDlgSessionCommand();

	//重载函数
protected:
	//初始窗口
	virtual BOOL OnInitDialog();
	//交换数据
	virtual VOID DoDataExchange(CDataExchange* pDX);

	//回调接口
public:
	//连接事件
	virtual bool OnEventMissionLink(LONG lErrorCode) { return true; }
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason) { return true; }
	//读取事件
	virtual bool OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize);

	//辅助函数
private:
	//控件使能
	VOID EnableControls(BOOL bEnabled);

	//事件函数
protected:
	//执行命令
	VOID OnBnClickedBtExecuteCmd();
	//重载配置
	VOID OnBnClickedBtReloadConfig();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
#endif