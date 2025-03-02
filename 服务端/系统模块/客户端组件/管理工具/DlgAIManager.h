#ifndef DLG_AI_MANAGER_HEAD_FILE
#define DLG_AI_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CheckComboBox.h"
#include "MissionAndroid.h"
#include "DlgManagerOption.h"

//////////////////////////////////////////////////////////////////////////////////
//房间管理
class CDlgAIManager : public CDlgOptionItem, public IAndroidOperateCenter
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
	CButton							m_btAddItem;						//添加子项
	CButton							m_btModifyItem;						//修改子项
	CButton							m_btDeleteItem;						//删除子项
	CCheckComboBox					m_ComboServiceMode;					//服务模式

	//任务组件
protected:
	CMissionAndroid					m_MissionAndroid;					//机器任务
	CMissionManager					m_MissionManager;					//任务管理

	//控件变量
protected:
	CCheckComboBox *				m_pServerList;						//房间列表

	//列表配置
protected:
	CListCtrl						m_ParemeterList;					//参数列表

	//房间配置
protected:
	tagPCGameServer					m_GameServer;						//房间信息


	//函数定义
public:
	//构造函数
	CDlgAIManager();
	//析构函数
	virtual ~CDlgAIManager();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//配置函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();
	//销毁窗口
	virtual VOID OnDestory();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);

	//事件函数
protected:
	//添加子项
	VOID OnBnClickedBtAdd();
	//编辑子项
	VOID OnBnClickedBtModify();
	//删除子项
	VOID OnBnClickedBtDelete();
	//查询参数
	VOID OnBnClickedBtQuery();
	//单击子项
	VOID OnHdnListParameterItemclick(NMHDR *pNMHDR, LRESULT *pResult);
	//删除子项
	VOID OnLvnListParameterDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	//键盘按下
	VOID OnLvnKeydownListParameter(NMHDR *pNMHDR, LRESULT *pResult);

	//重载函数
protected:
	//绘画消息
	virtual VOID OnDrawClientAreaBegin(CDC * pDC, INT nWidth, INT nHeight);

	//机器接口
public:
	//控件使能
	virtual VOID OnEventEnableControls(BOOL bEnabled);
	//机器参数
	virtual VOID OnEventAndroidParenter(WORD wSubCommdID, WORD wParameterCount, tagRobotParameter* pAndroidParameter);

	//配置函数
public:
	//控件使能
	virtual VOID EnableControls(BOOL bEnabled);
	//构造数据
	virtual WORD ConstructLogonPacket(BYTE cbBuffer[], WORD wBufferSize);

	//功能函数
public:
	//配置管理
	VOID Initialization(ITCPSocket * pITCPSocket, IClientUserItem * pIMySelfUserItem, CCheckComboBox * pCheckComboBox, tagPCGameServer GameServer);
	//房间类型
	VOID SetServerType(WORD wServerType){m_wServerType=wServerType;}
	//初始化控件
	VOID InitControls();
	//添加修改
	VOID AddModifyParameter(bool bModify);
	//选中子项
	VOID SelectItem(int nIndex);
	//设置子项
	VOID SetItemToParameterList(int nItemIndex, tagRobotParameter* pAndroidParameter);

	//消息映射
protected:
	//控件颜色
	//HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////////////

#endif