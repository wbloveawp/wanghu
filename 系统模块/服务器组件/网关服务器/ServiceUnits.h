#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#pragma once

//平台定义
#include "InitParameter.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//网络标识
#define NETWORK_CENTER			1										//中心连接

//////////////////////////////////////////////////////////////////////////
//类型定义
typedef CWHArray<CTCPSocketServiceHelper *>		CSocketServiceArray; //服务数组

//////////////////////////////////////////////////////////////////////////

//服务单元
class CServiceUnits : public CWnd, public IMonitorClientSink
{
	//友元定义
	friend class CAttemperEngineSink;
	friend class CDataBaseEngineSink;

	//状态变量
protected:
	WORD							m_ServiceStatus;					//运行状态
	tagServiceOption				m_ServiceOption;					//服务配置
	CInitParameter					m_InitParameter;					//配置参数	

	//服务组件
protected:
	CUIControlSink					m_UIControlSink;					//界面控制
	CMonitorClient					m_MonitorClient;					//监控终端
	CAttemperEngineSink				m_AttemperEngineSink;				//调度钩子

	//内核组件
protected:
	CTimerEngineHelper				m_TimerEngine;						//时间引擎
	CAttemperEngineHelper			m_AttemperEngine;					//调度引擎
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//网络引擎
	CTCPSocketServiceHelper			m_TCPSocketCenter;					//中心服务

	//接口变量
protected:
	IServiceUnitsSink *				m_pIServiceUnitsSink;				//状态接口

	//静态变量
public:
	static CServiceUnits *			g_pServiceUnits;					//对象指针

	//函数定义
public:
	//构造函数
	CServiceUnits();
	//析构函数
	virtual ~CServiceUnits();

	//信息函数
public:
	//获取状态
	virtual WORD GetServiceStatus() { return m_ServiceStatus; }

	//服务控制
public:
	//启动服务
	virtual bool StartService();
	//停止服务
	virtual bool ConcludeService();
	//设置配置
	virtual void SetServiceOption(tagServiceOption &ServiceOption);
	//设置接口
	virtual bool SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink);

	//辅助函数
protected:
	//配置组件
	bool InitializeService();
	//启动内核
	bool StartKernelService();
	//启动网络
	bool StartNetworkService();

	//内部函数
private:
	//设置状态
	bool SetServiceStatus(WORD ServiceStatus);
	//发送控制
	bool SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize);

	//功能函数
public:
	//服务配置
	const tagServiceOption * GeServiceOption() const { return &m_ServiceOption; }
	//配置参数
	const CInitParameter * GetInitParameter() const { return &m_InitParameter; }

	//消息映射
protected:
	//控制消息
	LRESULT OnUIControlRequest(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif