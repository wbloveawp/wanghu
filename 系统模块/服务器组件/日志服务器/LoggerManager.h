#ifndef LOGGER_MANAGER_HEAD_FILE
#define LOGGER_MANAGER_HEAD_FILE
#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////////////
//类型定义
typedef CMap<CString, LPCTSTR, CString, CString> CMapLogFormat;
typedef CMap<CString, LPCTSTR, CDayLogger*, CDayLogger*> CMapDayLogger;

/////////////////////////////////////////////////////////////////////////////////////
//日志管理
class CLoggerManager
{
	//状态变量
protected:
	bool								m_bService;							//服务状态

	//配置变量
protected:
	TCHAR								m_szLogPath[MAX_PATH];				//日志路径

	//组件变量
protected:
	CLoggerThread						m_LoggerThread;						//日志线程

	//管理变量
protected:
	CMapDayLogger						m_MapDayLogger;						//日志字典
	CMapLogFormat						m_MapLogFormat;						//格式字典

	//函数定义
public:
	//构造函数
	CLoggerManager();
	//析构函数
	virtual~CLoggerManager();

	//管理函数
public:
	//启动服务
	bool StartService();
	//停止服务
	bool ConcludeService();

	//配置函数
public:
	//设置路径
	void SetLogPath(LPCTSTR pszLogPath);

	//服务函数
public:
	//获取日志
	CDayLogger& GetDayLogger(const tagTimeLogInfo& LogInfo);

	//辅助函数
protected:
	//日志名称
	CString GetLogName(const tagTimeLogInfo& LogInfo);
	//日志路径
	CString GetLogPath(const tagTimeLogInfo& LogInfo);
	//日志类型
	CLogTypePtr GetLogType(const CString& strFormat, const CString& strLogType);
	//创建日志
	CDayLogger& MakeDayLogger(const tagTimeLogInfo& LogInfo, const CString& strLogName);

	//辅助函数
private:
	//清理日志
	void CleanLoggers();
};

/////////////////////////////////////////////////////////////////////////////////////
#endif

