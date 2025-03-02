#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//配置参数
class CInitParameter
{
	//组件时间
public:
	WORD							m_wReportTime;						//上报时间
	WORD							m_wConnectTime;						//重连时间

	//站点信息
public:
	WORD							m_wStationID;						//站点标识
	TCHAR							m_szStationName[LEN_SERVER];		//站点名称

	//中心配置
public:	
	WORD							m_wCenterPort;						//中心端口
	tagAddressInfo					m_CenterAddress;					//中心地址	

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
