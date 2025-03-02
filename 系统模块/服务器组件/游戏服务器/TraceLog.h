#ifndef TRACE_LOG_HEAD_FILE
#define TRACE_LOG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include <fstream>

//////////////////////////////////////////////////////////////////////////

//输出日志
class CTraceLog: public ITraceService
{
	//服务参数
public:
	bool							m_bLoging;							//启动标志
	std::ofstream					m_LogFile;							//输出日志

	//组件变量
protected:
	ITraceService*					m_pITraceService;					//内部输出

	//函数定义
public:
	//构造函数
	CTraceLog();
	//析构函数
	virtual ~CTraceLog();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//信息接口
public:
	//追踪信息
	virtual bool TraceString(LPCTSTR pszString, enTraceLevel TraceLevel);

	//功能函数
public:
	//初始化
	void InitLog(IUnknownEx *pITraceService);
	//创建日志
	bool OpenLog(LPCTSTR lpszFile);
	//关闭日志
	bool CloseLog();
	//查询状态
	bool IsLoging() { return m_bLoging; }
};

//////////////////////////////////////////////////////////////////////////

#endif