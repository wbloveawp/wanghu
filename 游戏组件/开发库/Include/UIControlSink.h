#ifndef UI_CONTROL_SINK_HEAD_FILE
#define UI_CONTROL_SINK_HEAD_FILE

#pragma once

#include "ServiceFrameHead.h"

//////////////////////////////////////////////////////////////////////////

#define WM_UICONTROL_REQUEST		(WM_USER+100)						//控制请求

//////////////////////////////////////////////////////////////////////////

//界面控制
class SERVICE_FRAME_CLASS CUIControlSink
{
	//辅助变量
private:
	HWND 							m_hHookWnd;							//挂钩窗口
	BYTE							m_cbBuffer[MAX_ASYNC_DATA];			//数据缓存

	//组件变量
private:
	CWHDataQueue						m_DataQueue;						//数据队列
	CCriticalSection				m_CriticalSection;					//同步对象

	//函数定义
public:
	//构造函数
	CUIControlSink();
	//析构函数
	virtual ~CUIControlSink();

	//配置函数
public:
	//配置窗口
	bool SetHookWindow(HWND hHookWnd);

	//功能函数
public:
	//获取请求
	VOID * GetControlRequest(tagDataHead & DataHead);
	//投递请求
	bool PostControlRequest(WORD dwIdentifier, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

#endif