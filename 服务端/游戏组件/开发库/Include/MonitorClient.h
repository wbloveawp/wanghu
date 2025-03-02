#ifndef MONITOR_CLIENT_HEAD_FILE
#define MONITOR_CLIENT_HEAD_FILE

#pragma once

#include "UIControlSink.h"
#include "ServiceFrameHead.h"

//////////////////////////////////////////////////////////////////////////
//接口定义
interface IMonitorClientSink
{
	//服务控制
public:
	//启动服务
	virtual bool StartService()=NULL;
	//停止服务
	virtual bool ConcludeService()=NULL;
};

//////////////////////////////////////////////////////////////////////////
//类型定义
typedef CMap<CString, LPCTSTR, bool, bool>		CMapExceptionDigest;

//////////////////////////////////////////////////////////////////////////

//监控终端
class SERVICE_FRAME_CLASS CMonitorClient : public CWnd, public IProcessTradeSink
{
	//状态变量
protected:
	bool							m_bServiceStatus;					//服务状态

	//服务信息
protected:
	WORD							m_wServiceID;						//服务标识
	WORD							m_wServiceModule;					//服务模块
	WORD							m_wServiceStatus;					//服务状态
	TCHAR							m_szServiceName[32];				//服务名称

	//异常信息
protected:
	CMapExceptionDigest				m_MapExceptionDigest;				//摘要字典

	//通讯变量
protected:
	HWND							m_hWndServer;						//窗口句柄
	CUIControlSink					m_UIControlSink;					//界面控制
	CProcessTradeHelper				m_ProcessTrade;						//通讯组件	

	//接口变量
protected:
	IMonitorClientSink*				m_pIMonitorClientSink;				//回调指针

	//静态变量
public:
	static CMonitorClient*			g_pMonitorClient;					//终端指针

	//函数定义
public:
	//构造函数
	CMonitorClient();
	//析构函数
	virtual ~CMonitorClient();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);	

	//服务控制
public:
	//启动服务
	virtual bool StartService();
	//停止服务
	virtual bool ConcludeService();	

	//进程通讯
public:
	//进程数据
	virtual bool OnProcessTradeData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize, HWND hWndSend);

	//配置函数
public:
	//设置接口
	VOID SetMonitorClientSink(IMonitorClientSink* pIMonitorClientSink);
	//设置参数
	VOID SetServiceParameter(WORD wServiceModule, WORD wServiceID, LPCTSTR pszServiceName);

	//功能函数
public:
	//初始化终端
	bool InitializeClient();
	//更新状态
	VOID UpdateServiceStatus(WORD wServiceStatus);	

	//进程数据
public:
	//进程监控
	bool OnEventProcessMonitor(WORD wSubCmdID, VOID* pData, WORD wDataSize, HWND hWndSend);		

	//消息映射
protected:
	//控制消息
	LRESULT OnUIControlRequest(WPARAM wParam, LPARAM lParam);

	//静态函数
public:	
	//异常钩子
	static VOID DBExceptionHookFun(enSQLException SQLException, CComError* pComError);

	//消息函数
public:
	//定时器消息
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif