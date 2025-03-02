#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once


//////////////////////////////////////////////////////////////////////////

//配置参数
class CInitParameter
{
	//系统配置
public:
	WORD							m_wConnectTime;						//重连时间

	//配置信息
public:
	WORD							m_wCenterPort;						//中心端口
	tagAddressInfo					m_CenterAddress;					//中心地址

	//系统信息
public:
	TCHAR							m_szLoggerPath[MAX_PATH];			//日志路径

	//连接配置
public:
	tagDataBaseParameter			m_PlatformDBParameter;				//连接信息	

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
};

//////////////////////////////////////////////////////////////////////////

#endif
