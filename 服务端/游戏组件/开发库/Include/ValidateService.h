#ifndef VALIDATE_SERVICE_HEAD_FILE
#define VALIDATE_SERVICE_HEAD_FILE

#pragma once

#include "ServiceFrameHead.h"

//////////////////////////////////////////////////////////////////////////

//类说明
class CValidateService;

//////////////////////////////////////////////////////////////////////////

//验证模块
class CValidateServiceSink : public ITCPSocketEvent
{
	//友元定义
	friend class CValidateService;

	//组件变量
protected:
	CValidateService *				m_pValidateService;					//验证服务

	//函数定义
protected:
	//构造函数
	CValidateServiceSink();
	//析构函数
	virtual ~CValidateServiceSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//网络接口
protected:
	//连接事件
	virtual bool OnEventTCPSocketLink(DWORD wServiceID, INT nErrorCode);
	//关闭事件
	virtual bool OnEventTCPSocketShut(DWORD wServiceID, BYTE cbShutReason);
	//读取事件
	virtual bool OnEventTCPSocketRead(DWORD wServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount);
};

//////////////////////////////////////////////////////////////////////////

//验证服务
class SERVICE_FRAME_CLASS CValidateService
{
	//友元定义
	friend class CValidateServiceSink;

	//状态变量
protected:
	bool							m_bValidate;						//验证标志
	HANDLE							m_hResultEvent;						//事件句柄

	//读取信息
protected:
	tagAddressInfo					m_CorrespondAddress;				//协调地址
	tagDataBaseParameter			m_PlatformDBParameter;				//连接信息
	tagDataBaseParameter			m_AccountsDBParameter;				//连接信息
	tagDataBaseParameter			m_GameMatchDBParameter;				//连接信息
	tagDataBaseParameter			m_GameBattleDBParameter;			//连接信息	

	//组件变量
protected:
	CValidateServiceSink			m_ValidateServiceSink;				//验证模块
	CTCPSocketServiceHelper			m_TCPSocketServiceModule;			//网络服务

	//函数定义
public:
	//构造函数
	CValidateService();
	//析构函数
	virtual ~CValidateService();

	//功能函数
public:	 
	//连接信息
	bool GetMatchDBParameter(tagDataBaseParameter & DataBaseParameter);
	//连接信息
	bool GetBattleDBParameter(tagDataBaseParameter & DataBaseParameter);
	//连接信息
	bool GetPlatformDBParameter(tagDataBaseParameter & DataBaseParameter);
	//连接信息
	bool GetAccountsDBParameter(tagDataBaseParameter & DataBaseParameter);

	//内部函数
protected:
	//执行验证
	bool PerformValidate();
};

//////////////////////////////////////////////////////////////////////////

#endif