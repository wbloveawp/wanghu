#ifndef DISPATCH_MANAGER_HEAD_FILE
#define DISPATCH_MANAGER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////
//事件定义

//登录事件
#define EVENT_LOGON_SUCCESS			1									//登录成功
#define EVENT_LOGOUT_NOTIFY			2									//登出提醒
#define EVENT_COMMIT_MODIFY			3									//提交修改
#define EVENT_USER_INFO_UPDATE		4									//资料更新


//系统消息
#define EVENT_DOWN_LOAD_FINISH		20									//下载成功

//////////////////////////////////////////////////////////////////////////

//通知消息
#define WM_EVENT_INFORM				(WM_USER+1000)						//事件通知

//////////////////////////////////////////////////////////////////////////

//数组定义
typedef CWHArray<HWND> CEventWndArray;

//事件管理
class CDispatchManager
{
	//变量定义
protected:
	CEventWndArray					m_EventWndArray;					//事件窗口

	//静态变量
protected:
	static CDispatchManager *		m_pDispatchManager;					//对象指针

	//函数定义
public:
	//构造函数
	CDispatchManager();
	//析构函数
	virtual ~CDispatchManager();

	//注册窗口
public:
	//注册窗口
	bool RegisterEventWnd(HWND hEventWnd);
	//注销窗口
	bool UnRegisterEventWnd(HWND hEventWnd);

	//发送事件
public:
	//发送事件
	VOID SendEventInform(WORD wEventID, LPARAM lParam);
	//发送事件
	VOID PostEventInform(WORD wEventID, LPARAM lParam);

	//静态函数
public:
	//获取对象
	static CDispatchManager * GetInstance() { return m_pDispatchManager; }
};

//////////////////////////////////////////////////////////////////////////

#endif