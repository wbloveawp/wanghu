#include "StdAfx.h"
#include "UIControlSink.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CUIControlSink::CUIControlSink()
{
	//设置变量
	m_hHookWnd=NULL;
	ZeroMemory(m_cbBuffer,sizeof(m_cbBuffer));

	return;
}

//析构函数
CUIControlSink::~CUIControlSink()
{
}

//设置窗口
bool CUIControlSink::SetHookWindow(HWND hHookWnd)
{
	//效验参数
	ASSERT(IsWindow(hHookWnd));
	if (IsWindow(hHookWnd)==FALSE) return false;

	//设置变量
	m_hHookWnd=hHookWnd;

	return true;
}

//获取请求
VOID * CUIControlSink::GetControlRequest(tagDataHead & DataHead)
{
	//获取数据
	CThreadLock ThreadLock(m_CriticalSection);
	if (m_DataQueue.DistillData(DataHead,m_cbBuffer,sizeof(m_cbBuffer),true)==false) return NULL;

	return m_cbBuffer;
}

//投递请求
bool CUIControlSink::PostControlRequest(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	//状态判断
	ASSERT(IsWindow(m_hHookWnd));
	if (IsWindow(m_hHookWnd)==FALSE) return false;

	//插入队列
	CThreadLock ThreadLock(m_CriticalSection);
	if (m_DataQueue.InsertData(dwIdentifier,pData,wDataSize)==false) return false;

	//发送消息
	::PostMessage(m_hHookWnd,WM_UICONTROL_REQUEST,dwIdentifier,wDataSize);

	return true;
}

//////////////////////////////////////////////////////////////////////////
