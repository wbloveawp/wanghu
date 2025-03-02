#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#pragma once

#include "InitParameter.h"
#include "GameServiceHead.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////

//网络标示
#define NETWORK_STORE				1									//存储连接
#define NETWORK_SERVICE				2									//比赛连接
#define NETWORK_CENTER				4									//中心连接

//////////////////////////////////////////////////////////////////////////

//服务单元
class CServiceUnits : public CWnd, public IGameServiceUnits, public IMonitorClientSink
{
	//友元定义
	friend class CAttemperEngineSink;

	//状态变量
protected:
	bool							m_bForceClose;						//强制关闭
	WORD							m_ServiceStatus;					//运行状态
	tagGameServiceAttrib			m_GameServiceAttrib;				//服务属性
	tagGameServiceOption			m_GameServiceOption;				//服务配置	

	//服务组件
protected:
	CInitParameter					m_InitParameter;					//配置参数
	CMonitorClient					m_MonitorClient;					//监控终端
	CUIControlSink					m_UIControlSink;					//界面控制
	CAttemperEngineSink				m_AttemperEngineSink;				//调度钩子
	CDataBaseEngineSink				m_DataBaseEngineSink[16];			//数据钩子	

	//内核引擎
protected:
	CTimerEngineHelper				m_TimerEngine;						//时间引擎	
	CAttemperEngineHelper			m_AttemperEngine;					//调度引擎
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//网络引擎
	CGameServiceManagerHelper		m_GameServiceManager;				//游戏模块
	
	//网络引擎
protected:
	CTCPSocketServiceHelper			m_TCPSocketStore;					//存储服务
	CTCPSocketServiceHelper			m_TCPSocketCenter;					//中心服务	
	CTCPSocketServiceHelper			m_TCPSocketService;					//类型服务		

	//数据库引擎
protected:
	CDataBaseEngineHelper			m_DataBaseEngine;					//数据引擎	

	//接口变量
protected:
	IServiceUnitsSink *				m_pIServiceUnitsSink;				//状态接口

	//服务模块
protected:
#ifdef VIDEO_MODULE_HELPER	
	CGameVideoManagerHelper			m_GameVideoManager;					//录像管理	
#endif
#ifdef MATCH_MODULE_HELPER		
	CMatchServiceManagerHelper		m_MatchServiceManager;				//比赛服务
#endif	
#ifdef BATTLE_MODULE_HELPER		
	CBattleServiceManagerHelper		m_BattleServiceManager;				//约战服务
#endif	
#ifdef RANKING_MODULE_HELPER		
	CRankingServiceManagerHelper	m_RankingServiceManager;			//排位服务
#endif

	//静态变量
public:
	static CServiceUnits *			g_pServiceUnits;					//对象指针

	//函数定义
public:
	//构造函数
	CServiceUnits();
	//析构函数
	virtual ~CServiceUnits();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//信息函数
public:
	//获取状态
	virtual WORD GetServiceStatus() { return m_ServiceStatus; }

	//服务控制
public:
	//启动服务
	virtual bool StartService();
	//调试服务
	virtual bool DebugService();
	//停止服务
	virtual bool ConcludeService();	
	//设置接口
	virtual bool SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink);
	//游戏配置
	virtual bool SetServiceCollocate(LPCTSTR pszGameModule, const tagGameServiceOption & GameServiceOption);

	//辅助函数
protected:
	//创建模块
	bool CreateServiceDLL();
	//配置模块
	bool InitializeService();
	//调整参数
	bool RectifyServiceParameter();

	//启动管理
protected:
	//启动内核
	bool StartKernelService();
	//启动网络
	bool StartSocketService();
	//启动服务
	bool StartExtendService();
	//启动网络
	bool StartNetworkService();
	//启动数据库
	bool StartDataBaseService();

	//内部函数
private:
	//设置状态
	bool SetServiceStatus(WORD ServiceStatus);
	//发送控制
	bool SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize);

	//消息映射
protected:
	//控制消息
	LRESULT OnUIControlRequest(WPARAM wParam, LPARAM lParam);

	//消息函数
public:
	//定时器消息
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////

#endif