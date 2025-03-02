#ifndef PLATFORM_FRAME_HEAD_FILE
#define PLATFORM_FRAME_HEAD_FILE

#pragma once

//系统文件
#include "Stdafx.h"
#include "CardDispatcher.h"
#include "DlgSessionItem.h"
#include "DlgSessionRobot.h"
#include "DlgSessionStock.h"
#include "DlgSessionConfig.h"
#include "DlgSessionServer.h"

//任务文件
#include "SessionLogon.h"
#include "VirtualLinkManager.h"

//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IPlatformFrame INTERFACE_VERSION(1,1)
static const GUID IID_IPlatformFrame = { 0xa2565921, 0x7434, 0x4a0d, 0x9f, 0x83, 0xd5, 0x35, 0xd1, 0x97, 0x90, 0xb8 };
#else
#define VER_IPlatformFrame INTERFACE_VERSION(1,1)
static const GUID IID_IPlatformFrame = { 0xc70a9f11, 0xfa16, 0x4a91, 0x86, 0xb2, 0x84, 0xc9, 0x24, 0xa2, 0x1b, 0xca };
#endif

//大厅接口
interface IPlatformFrame : public IUnknownEx
{
};

//平台框架
class CPlatformFrame : public CDialog, public IPlatformFrame, public IServerListViewSink
{
	//变量定义
protected:
	int								m_nSelectItem;						//选择索引	
	bool							m_bLogonSuccess;					//登录成功	

	//游戏列表
protected:
	CServerListManager				m_ServerListManager;				//游戏数据	

	//控件变量
protected:
	BYTE							m_cbItemIndex;						//子项索引
	CDlgSessionItem *				m_pDlgSessionItem[6];				//子项数组
	CDlgSessionRobot				m_DlgSessionRobot;					//机器管理
	CDlgSessionStock				m_DlgSessionStock;					//库存管理
	CDlgSessionConfig				m_DlgSessionConfig;					//配置管理
	CDlgSessionOption				m_DlgSessionOption;					//房间配置
	CDlgSessionServer				m_pDlgSessionServer;				//房间会话
	CDlgSessionCommand				m_DlgSessionCommand;				//管理命令	

	//控件变量
protected:		
	CSystemTrayIcon					m_SystemTrayIcon;					//任务图标

	//任务组件
protected:
	CSessionLogon					m_MissionLogon;						//登录任务

	//链接组件
protected:
	CVirtualLinkItem				m_VirtualLinkLogon;					//登录链接
	CVirtualLinkItem				m_VirtualLinkServer;				//房间链接
	CVirtualLinkManager				m_VirtualLinkManager;				//链接管理

	//房间属性
protected:
	WORD							m_wServerType;						//房间类型
	WORD							m_wTableID;							//桌子ID
	DWORD							m_dwServerRule;						//房间规则
	tagGameKind						m_GameKind;							//类型信息
	tagPCGameServer					m_GameServer;						//房间信息
	enServiceStatus					m_ServiceStatus;					//房间状态

	//用户属性
protected:
	DWORD							m_dwUserRight;						//用户权限
	DWORD							m_dwMasterRight;					//管理权限
	IClientUserItem*				m_pIMySelfUserItem;					//自己指针
	IClientUserItem*				m_pISelectedUserItem;				//选择指针

	//组件变量
protected:
	CTCPSocketHelper				m_TCPSocketModule;					//网络连接	
	CUserListControl				m_UserListControl;					//用户列表
	CPlazaUserManagerHelper			m_PlazaUserManagerModule;			//用户管理

	//静态变量
protected:
	static CPlatformFrame *			m_pPlatformFrame;					//框架指针

	//函数定义
public:
	//构造函数
	CPlatformFrame();
	//析构函数
	virtual ~CPlatformFrame();

	//重载函数
protected:
	//初始窗口
	virtual BOOL OnInitDialog();
	//消息过虑
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//交换数据
	virtual VOID DoDataExchange(CDataExchange* pDX);

	//状态通知
public:
	//完成通知
	virtual VOID OnGameItemFinish();
	//完成通知
	virtual VOID OnGameKindFinish(WORD wKindID);

	//更新通知
public:
	//插入通知
	virtual VOID OnGameItemInsert(CGameListItem * pGameListItem);
	//更新通知
	virtual VOID OnGameItemUpdate(CGameListItem * pGameListItem);
	//删除通知
	virtual VOID OnGameItemDelete(CGameListItem * pGameListItem);

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//消息函数
protected:
	//选择变更
	VOID OnTcnSelchangeManagerOption(NMHDR* pNMHDR, LRESULT* pResult);

	//人数通知
public:
	//总数通知
	virtual VOID OnUpdateOnlineCount(DWORD dwOnlineCount);

	//辅助函数
private:
	//初始化控件
	VOID InitControls();
	//激活子项
	VOID ActiveViewItem(BYTE cbItemIndex);
	//调整控件
	VOID RectifyControl(INT nWidth, INT nHeight);

	//消息函数
protected:
	//关闭消息
	VOID OnClose();	
	//销毁消息
	VOID OnDestroy();
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);

	//用户菜单
protected:
	//显示菜单
	virtual VOID TrackUserItemMenu(IClientUserItem* pIClientUserItem);

	//右键列表
	VOID OnNMRclickUserList(NMHDR* pNMHDR, LRESULT* pResult);

	//自定消息
protected:
	//事件消息
	LRESULT OnEventInformMessage(WPARAM wParam, LPARAM lParam);

	//功能函数
public:
	//获取实例
	static CPlatformFrame * GetInstance() { return m_pPlatformFrame; }

	DECLARE_MESSAGE_MAP()				
};

//////////////////////////////////////////////////////////////////////////

#endif