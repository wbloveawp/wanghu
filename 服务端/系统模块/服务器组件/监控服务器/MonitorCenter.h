#ifndef MONITOR_CENTER_HEAD_FILE
#define MONITOR_CENTER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "ForwardPacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////
//类型定义

//进程统计信息
struct tagProcessStatInfo
{
	//进程参数
	int							nCPUUsage;						//CPU使用
	UINT64						uReadBytes;						//读字节数	 
	UINT64						uWriteBytes;					//写字节数
	UINT64						uMemoryUsage;					//内存使用
	UINT64						uVMemoryUsage;					//内存使用
	DWORD						dwLastTimeStamp;				//时间戳

	//预警参数
	DWORD						dwCpuTestTime;					//测试时间	
	DWORD						dwCpuAlarmTime;					//预警时间		
	UINT64						uInitMemoryUsage;				//内存使用
	UINT64						uInitVMemoryUsage;				//内存使用
	UINT64						uAlarmMemoryUsage;				//内存使用
	UINT64						uAlarmVMemoryUsage;				//内存使用

	//辅助参数
	int64_t						CpuLastTime;					//本地时间
	int64_t						CpuLastSystemTime;				//系统时间
};

//服务进程
struct tagServiceProcessItem
{	
	//进程信息
	HWND						hWndRecv;						//窗口句柄
	DWORD						dwProcessID;					//进程标识

	//服务信息
	WORD						wServiceID;						//服务标识
	WORD						wServiceModule;					//服务模块
	WORD						wServiceStatus;					//服务状态
	TCHAR						szServiceName[32];				//服务名称
	TCHAR						szProcessPath[MAX_PATH];		//进程路径

	//统计信息
	tagProcessStatInfo			ProcessStatInfo;				//统计信息	
};

//服务崩溃信息
struct tagServiceCrashInfo
{
	WORD						wServiceID;						//服务标识
	WORD						wServiceModule;					//服务模块	
	DWORD						dwCrashTime;					//崩溃时间
};


//////////////////////////////////////////////////////////////////////////
//类型定义	
typedef CWHStockData<DWORD, tagServiceCrashInfo>		CStockCrashInfo;
typedef CWHStockData<DWORD, tagServiceProcessItem>		CStockProcessItem;

//////////////////////////////////////////////////////////////////////////

//监控中心
class CMonitorCenter : public CWnd, public IProcessTradeSink
{
	//变量定义
protected:
	bool							m_bServiceStatus;					//服务状态
	IAttemperEngine*				m_pIAttemperEngine;					//调度引擎	

	//环境变量
protected:
	SYSTEM_INFO						m_SystemInfo;						//系统信息

	//服务信息
protected:
	TCHAR							m_szServerName[32];					//服务名称
	tagServiceOption*				m_pServiceOption;					//服务选项

	//服务组件
protected:
	CUIControlSink					m_UIControlSink;					//界面控制	

	//进程管理
protected:
	CStockCrashInfo					m_StockCrashInfo;					//崩溃信息
	CStockProcessItem				m_StockProcessItem;					//进程仓库

	//进程通讯
protected:
	CProcessTradeHelper				m_ProcessTrade;						//通讯组件

	//静态变量
public:
	static CMonitorCenter*			g_pMonitorCenter;					//对象指针

	//函数定义
public:
	//构造函数
	CMonitorCenter();
	//析构函数
	virtual ~CMonitorCenter();

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
	//服务名称
	VOID SetServerName(LPCTSTR pszServerName);
	//服务信息
	VOID SetServiceOption(tagServiceOption* pServiceOption);
	//设置接口
	VOID SetAttemperEngine(IAttemperEngine* pIAttemperEngine);	

	//进程数据
public:
	//进程监控
	bool OnEventProcessMonitor(WORD wSubCmdID, VOID* pData, WORD wDataSize, HWND hWndSend);

	//服务控制
private:
	//启动服务
	bool StartService(tagServiceProcessItem* pServiceProcessItem);
	//停止服务
	bool ConcludeService(tagServiceProcessItem* pServiceProcessItem);

	//内部函数
private:
	//关闭进程
	VOID NotifyCloseProcess(HWND hWndClient);
	//发送控制
	bool SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize);

	//检测函数
private:
	//分析CPU占用
	VOID AnalyseCpuUsage(tagServiceProcessItem* pServiceProcessItem);
	//分析内存占用
	VOID AnalyseMemoryUsage(tagServiceProcessItem* pServiceProcessItem);	

	//消息映射
protected:
	//控制消息
	LRESULT OnUIControlRequest(WPARAM wParam, LPARAM lParam);

	//辅助函数
protected:
	//发送预警
	VOID SendExceptionAlarmNotice(DWORD dwProcessID,CMD_CS_SendExceptionAlarm* pSendExceptionAlarm);

	//消息事件
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif