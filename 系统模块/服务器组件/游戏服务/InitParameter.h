#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//配置参数
class CInitParameter
{
	//系统配置
public:	
	WORD							m_wConnectTime;						//重连时间
	WORD							m_wConnectStore;					//重连时间
	WORD							m_wConnectService;					//重连时间
	WORD							m_wConnectMonitor;					//重连时间

	//配置信息
public:
	TCHAR							m_szLoggerPath[MAX_PATH];			//日志路径

	//连接信息
public:
	WORD							m_wCenterPort;						//中心端口
	tagAddressInfo					m_CenterAddress;					//中心地址
	tagAddressInfo					m_ServiceAddress;					//服务地址	
	tagDataBaseParameter			m_PlatformDBParameter;				//连接信息
	tagDataBaseParameter			m_AccountsDBParameter;				//连接信息

	//函数定义
public:
	//构造函数
	CInitParameter();
	//析构函数
	virtual ~CInitParameter();

	//功能函数
public:
	//初始化
	VOID InitParameter();
	//加载配置
	VOID LoadInitParameter();
	//日志配置
	VOID LoadLoggerParameter();
};

//////////////////////////////////////////////////////////////////////////

#endif
