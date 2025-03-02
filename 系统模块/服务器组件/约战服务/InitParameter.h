#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

#include "MatchServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//配置参数
class CInitParameter
{
	//系统配置
public:
	WORD							m_wMaxConnect;						//最大连接

	//组件时间
public:
	WORD							m_wConnectTime;						//重连时间

	//配置信息
public:
	BYTE							m_cbNetworkKind;					//网络类型
	TCHAR							m_szValidate[LEN_VALIDATE];			//验证地址
	TCHAR							m_szAccreditID[LEN_ACCREDIT_ID];	//授权号码

	//连接信息
public:
	tagAddressInfo					m_ServiceAddress;					//服务地址
	tagAddressInfo					m_CorrespondAddress;				//协调地址
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
