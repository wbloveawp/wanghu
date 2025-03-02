#ifndef DLG_ROBOT_MANAGER_HEAD_FILE
#define DLG_ROBOT_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CheckComboBox.h"
#include "CustomListCtrl.h"
#include "DlgSessionItem.h"

//////////////////////////////////////////////////////////////////////////////////
//机器会话
class CDlgSessionRobot : public CDlgSessionItem
{
	//房间类型
protected:
	WORD							m_wServerType;						//房间类型

	//资源变量
protected:
	CPen							m_BoradPen;							//资源变量
	CBrush							m_FrameBrush;						//资源变量

	//变量定义
protected:
	ITCPSocket *					m_pITCPSocket;						//网络接口
	IClientUserItem *				m_pIMySelfUserItem;					//自己指针
	int								m_nSelectItem;						//选择索引

	//控件变量
protected:
	CButton							m_btQuery;							//查询按钮
	CButton							m_btAppendItem;						//添加子项
	CButton							m_btModifyItem;						//修改子项
	CButton							m_btDeleteItem;						//删除子项
	CCheckComboBox					m_ComboServiceMode;					//服务模式

	//控件变量
protected:
	CCheckComboBox					m_cbGameList;						//游戏列表
	CComboBox						m_cbServerList;						//房间列表	

	//列表配置
protected:
	CCustomListCtrl					m_ParemeterList;					//参数列表

	//房间配置
protected:
	tagPCGameServer					m_GameServer;						//房间信息


	//函数定义
public:
	//构造函数
	CDlgSessionRobot();
	//析构函数
	virtual ~CDlgSessionRobot();

	//重载函数
protected:	
	//确定函数
	virtual VOID OnOK();
	//销毁窗口
	virtual VOID OnDestory();
	//配置函数
	virtual BOOL OnInitDialog();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//控件绑定
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

	//按钮事件
protected:
	//查询参数
	VOID OnBnClickedBtQuery();
	//添加子项
	VOID OnBnClickedBtAppend();
	//编辑子项
	VOID OnBnClickedBtModify();
	//删除子项
	VOID OnBnClickedBtDelete();	

	//列表事件
protected:
	//键盘按下
	VOID OnLvnKeydownListParameter(NMHDR* pNMHDR, LRESULT* pResult);
	//单击子项
	VOID OnHdnListParameterItemclick(NMHDR *pNMHDR, LRESULT *pResult);
	//删除子项
	VOID OnLvnListParameterDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);	

	//重载函数
protected:
	//绘画消息
	virtual VOID OnDrawClientAreaBegin(CDC * pDC, INT nWidth, INT nHeight);

	//配置函数
public:
	//房间类型
	VOID SetServerType(WORD wServerType) { m_wServerType = wServerType; }
	//配置管理
	VOID Initialization(ITCPSocket * pITCPSocket, IClientUserItem * pIMySelfUserItem, tagPCGameServer GameServer);	

	//功能函数
public:
	//初始化控件
	VOID InitControls();
	//选中子项
	VOID SelectItem(int nIndex);
	//控件使能
	VOID EnableControls(BOOL bEnabled);
	//添加修改
	VOID AddModifyParameter(bool bModify);
	//设置子项
	VOID SetItemToParameterList(int nItemIndex, tagRobotParameter* pRobotParameter);

	//网络请求
public:
	//查询参数
	bool QueryRobotParameter(WORD wServerID);
	//删除参数
	bool DeleteRobotParameter(WORD wServerID, DWORD dwBatchID);
	//修改参数
	bool ModifyRobotParameter(WORD wServerID, tagRobotParameter* pRobotParameter);
	//添加参数
	bool AppendRobotParameter(WORD wServerID, tagRobotParameter* pRobotParameter);

	//消息映射
protected:
	//控件颜色
	//HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////////////

#endif