#ifndef DLG_ROOM_CONFIG_HEAD_FILE
#define DLG_ROOM_CONFIG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CheckComboBox.h"
#include "CardDispatcher.h"
#include "DlgSessionItem.h"

//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IRoomConfig INTERFACE_VERSION(1,1)
static const GUID IID_IRoomConfig = { 0x24d6aae1, 0x8f5a, 0x4b67, 0x95, 0x52, 0xfb, 0xa3, 0xe5, 0x6a, 0xc8, 0x29 };
#else
#define VER_IRoomConfig INTERFACE_VERSION(1,1)
static const GUID IID_IRoomConfig = { 0x3ba8a607, 0xa696, 0x4cfa, 0x92, 0xcc, 0xa4, 0x7b, 0x46, 0xa4, 0xd7, 0x71 };
#endif

//大厅接口
interface IRoomConfig : public IUnknownEx
{
};

//服务状态
enum enServiceStatus
{
	ServiceStatus_Unknow,			//未知状态
	ServiceStatus_Entering,			//进入状态
	ServiceStatus_Validate,			//验证状态
	ServiceStatus_RecvInfo,			//读取状态
	ServiceStatus_ServiceIng,		//服务状态
	ServiceStatus_NetworkDown,		//中断状态
};
//////////////////////////////////////////////////////////////////////////

//房间会话
class CDlgSessionServer : public CDlgSessionItem, public IRoomConfig, public IUserManagerSink, public ICardDispatcherCallback, public IClientDebugCallback
{
	//用户属性
protected:
	DWORD							m_dwUserRight;						//用户权限
	DWORD							m_dwMasterRight;					//管理权限
	IClientUserItem*				m_pIMySelfUserItem;					//自己指针
	IClientUserItem*				m_pISelectedUserItem;				//选择指针

	//控件变量
protected:
	CCheckComboBox					m_cbGameList;						//游戏列表
	CComboBox						m_cbServerList;						//房间列表

	//房间属性
protected:
	WORD							m_wServerType;						//房间类型
	WORD							m_wTableID;							//桌子ID
	DWORD							m_dwServerRule;						//房间规则
	tagGameKind						m_GameKind;							//类型信息
	tagPCGameServer					m_GameServer;						//房间信息
	enServiceStatus					m_ServiceStatus;					//房间状态

	//连接信息
protected:
	WORD							m_wCurrentPort;						//当前端口
	DWORD							m_dwCurrentServer;					//当前地址

	//组件变量
protected:
	CUserListControl				m_UserListControl;					//用户列表
	CPlazaUserManagerHelper			m_PlazaUserManagerModule;			//用户管理

	//配牌器
protected:
	HINSTANCE						m_hCardDispatcherInst;				//配牌器句柄
	ICardDispatcher*				m_pICardDispatcher;					//配牌器接口

	//调试变量
protected:
	HINSTANCE						m_hClientDebugInst;					//调试句柄
	IClientDebug*					m_pIClientDebug;					//调试接口

	//机器变量
protected:
	HINSTANCE						m_hRobotDebugInst;					//机器句柄
	IClientDebug*					m_pIRobotDebug;						//机器接口

	//函数定义
public:
	//构造函数
	CDlgSessionServer();
	//析构函数
	virtual ~CDlgSessionServer();

	//重载函数
protected:
	//初始窗口
	virtual BOOL OnInitDialog();
	//交换数据
	virtual VOID DoDataExchange(CDataExchange* pDX);
	//命令函数
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//通知事件
public:
	//列表完成
	virtual VOID OnEventListFinish();

	//网络接口
protected:
	//连接事件
	virtual bool OnEventMissionLink(LONG nErrorCode);
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//读取事件
	virtual bool OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize);

	//配牌器接口
public:
	// 配牌器信息
	virtual bool OnCardDispatcherDebugInfo(WORD wTableID, void* pData, WORD nSize);

	//用户菜单
protected:
	//显示菜单
	virtual VOID TrackUserItemMenu(IClientUserItem* pIClientUserItem);	

	//用户接口
public:
	//用户激活
	virtual VOID OnUserItemAcitve(IClientUserItem* pIClientUserItem);
	//用户删除
	virtual VOID OnUserItemDelete(IClientUserItem* pIClientUserItem);
	//用户更新
	virtual VOID OnUserFaceUpdate(IClientUserItem* pIClientUserItem);
	//用户更新
	virtual VOID OnUserItemUpdate(IClientUserItem* pIClientUserItem, const tagUserStatus& LastStatus);
	//用户更新
	virtual VOID OnUserItemUpdate(IClientUserItem* pIClientUserItem, const tagClientUserScore& LastScore);	

	//调试接口
public:
	//发送数据
	virtual bool SendDebugData(WORD nMessageID, WORD wTableID, void* pData, WORD wDataSize);	

	//事件函数
protected:
	//进入房间
	VOID OnBnClickedBtEnterRoom();
	//退出房间
	VOID OnBnClickedBtExitRoom();
	//游戏调试
	VOID OnBnClickedBtGameDebug();
	//机器调试
	VOID OnBnClickedBtRobotDebug();
	//配牌器
	VOID OnBnClickedCardDispatcher();
	//右键列表
	VOID OnNMRclickUserList(NMHDR* pNMHDR, LRESULT* pResult);

	//网络事件
protected:
	//用户处理
	bool OnSocketMainUser(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//登录处理
	bool OnSocketMainLogon(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//调试处理
	bool OnSocketMainDebug(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//管理处理
	bool OnSocketMainManager(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//游戏处理
	bool OnSocketMainGameFrame(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//网络事件
protected:
	//登录完成
	bool OnSocketSubLogonFinish(VOID* pData, WORD wDataSize);
	//登录成功
	bool OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize);
	//登录失败
	bool OnSocketSubLogonFailure(VOID* pData, WORD wDataSize);
	//升级提示
	bool OnSocketSubUpdateNotify(VOID* pData, WORD wDataSize);
	//系统消息
	bool OnSocketSubSystemMessage(VOID* pData, WORD wDataSize);
	//用户进入
	bool OnSocketSubUserEnter(VOID* pData, WORD wDataSize);
	//用户积分
	bool OnSocketSubUserScore(VOID* pData, WORD wDataSize);
	//用户状态
	bool OnSocketSubUserStatus(VOID* pData, WORD wDataSize);
	//调试数据包
	bool OnSocketSubDebugPacket(VOID* pData, WORD wDataSize);	

	//内部函数
private:
	// 发送登录
	bool SendLogonPacket();
	// 使能按钮
	VOID EnableButton(bool bEnabled);
	//用户进入
	VOID OnEventUserEnter(IClientUserItem* pIClientUserItem);
	//用户离开
	VOID OnEventUserLeave(IClientUserItem* pIClientUserItem);
	//设置状态
	VOID SetServiceStatus(enServiceStatus ServiceStatus);
	//获取游戏版本
	VOID GetProcessVersion(WORD wKindID, DWORD& dwProcessVersion);
	//清理列表数据
	VOID CleanListData();
	//获取连接信息
	void GetConnectInfo();

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////////////

#endif