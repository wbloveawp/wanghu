#include "StdAfx.h"
#include "DispatchManager.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
CDispatchManager * CDispatchManager::m_pDispatchManager=NULL;			//对象指针

//////////////////////////////////////////////////////////////////////////

//构造函数
CDispatchManager::CDispatchManager()
{
	//设置对象
	ASSERT(m_pDispatchManager==NULL);
	if (m_pDispatchManager==NULL) m_pDispatchManager=this;

	return;
}

//析构函数
CDispatchManager::~CDispatchManager()
{
	//释放对象
	ASSERT(m_pDispatchManager==this);
	if (m_pDispatchManager==this) m_pDispatchManager=NULL;

	return;
}

//注册窗口
bool CDispatchManager::RegisterEventWnd(HWND hEventWnd)
{
	//效验参数
	ASSERT(IsWindow(hEventWnd));
	if (IsWindow(hEventWnd)==FALSE) return false;

	//存在判断
	for (INT_PTR i=0;i<m_EventWndArray.GetCount();i++)
	{
		if (m_EventWndArray[i]==hEventWnd)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//插入数组
	m_EventWndArray.Add(hEventWnd);

	return true;
}

//注销窗口
bool CDispatchManager::UnRegisterEventWnd(HWND hEventWnd)
{
	//效验参数
	ASSERT(IsWindow(hEventWnd));
	if (IsWindow(hEventWnd)==FALSE) return false;

	//删除判断
	for (INT_PTR i=0;i<m_EventWndArray.GetCount();i++)
	{
		if (m_EventWndArray[i]==hEventWnd)
		{
			m_EventWndArray.RemoveAt(i);
			return true;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return false;
}

//发送事件
VOID CDispatchManager::SendEventInform(WORD wEventID, LPARAM lParam)
{
	//发送事件
	for (INT_PTR i=0;i<m_EventWndArray.GetCount();i++)
	{
		HWND hWnd=m_EventWndArray[i];
		if (IsWindow(hWnd)) SendMessage(hWnd,WM_EVENT_INFORM,wEventID,lParam);
	}

	return;
}

//发送事件
VOID CDispatchManager::PostEventInform(WORD wEventID, LPARAM lParam)
{
	//发送事件
	for (INT_PTR i=0;i<m_EventWndArray.GetCount();i++)
	{
		HWND hWnd=m_EventWndArray[i];
		if (IsWindow(hWnd)) PostMessage(hWnd,WM_EVENT_INFORM,wEventID,lParam);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
