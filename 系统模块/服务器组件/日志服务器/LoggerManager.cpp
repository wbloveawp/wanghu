#include "stdafx.h"
#include "LoggerManager.h"

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CLoggerManager::CLoggerManager()
{
	//设置变量
	m_bService = false;
	m_szLogPath[0] = 0;

	//初始化字典
	m_MapDayLogger.InitHashTable(103);
	m_MapLogFormat.InitHashTable(103);
}

//析构函数
CLoggerManager::~CLoggerManager()
{
	//启动线程
	if (m_LoggerThread.IsRuning())
	{
		m_LoggerThread.ConcludeThread(INFINITE);
	}

	//清理日志
	CleanLoggers();
}

//启动服务
bool CLoggerManager::StartService()
{
	//校验状态
	ASSERT (m_bService==false);
	if (m_bService == true) return false;

	//启动线程
	m_LoggerThread.StartThread();

	//设置状态
	m_bService = true;

	return true;
}

//停止服务
bool CLoggerManager::ConcludeService()
{
	//校验状态
	if (m_bService == false) return true;

	//启动线程
	if (m_LoggerThread.IsRuning())
	{
		m_LoggerThread.ConcludeThread(INFINITE);
	}

	//清理日志
	CleanLoggers();

	//设置状态
	m_bService = false;
	
	return true;
}

//设置路径
void CLoggerManager::SetLogPath(LPCTSTR pszLogPath)
{
	StringCchCopy(m_szLogPath,CountArray(m_szLogPath),pszLogPath);
}

//获取日志
CDayLogger& CLoggerManager::GetDayLogger(const tagTimeLogInfo& LogInfo)
{
	//日志名称
	CString strFormat = LogInfo.szFormat;
	CString strLogName = GetLogName(LogInfo);

	//查找日志
	CDayLogger* pDayLogger = NULL;
	if (m_MapDayLogger.Lookup(strLogName, pDayLogger) == FALSE)
	{
		return MakeDayLogger(LogInfo, strLogName);
	}

	//查找格式
	CString strLogForamt;
	if (m_MapLogFormat.Lookup(strLogName, strLogForamt) == TRUE)
	{
		//校验格式
		if (strLogForamt != strFormat)
		{
			pDayLogger->UnSetupThread();
			SafeDelete(pDayLogger);
			pDayLogger = NULL;

			return MakeDayLogger(LogInfo, strLogName);
		}
	}

	return (*(m_MapDayLogger[strLogName]));
}


//创建日志
CDayLogger& CLoggerManager::MakeDayLogger(const tagTimeLogInfo& LogInfo, const CString& strLogName)
{
	//获取信息
	CString strFormat = LogInfo.szFormat;
	CString strLogType = LogInfo.szLogType;
	CString strLogPath = GetLogPath(LogInfo);

	//创建日志
	auto pDayLogger = new CDayLogger();

	pDayLogger->Init(strLogPath, strFormat, LogInfo.bHasSufix, LogInfo.szConcatStr, GetLogType(strFormat, strLogType));
	pDayLogger->SetSeparator(LogInfo.szSeparator);
	pDayLogger->SetupThread(&m_LoggerThread);

	//所有标识都不要
	pDayLogger->ModHeadMask(0xffff, false);

	//映射日志
	m_MapDayLogger[strLogName] = pDayLogger;
	m_MapLogFormat[strLogName] = strFormat;

	return (*pDayLogger);
}

//日志名称
CString CLoggerManager::GetLogName(const tagTimeLogInfo& LogInfo)
{
	CString strLogName;
	strLogName.Format(TEXT("%s%s%s%s%s%s%s%d%d"), LogInfo.szAppName, LogInfo.szServiceName, LogInfo.szFileName, LogInfo.szFormat, LogInfo.szConcatStr, LogInfo.szSeparator, LogInfo.szLogType, LogInfo.bHasSufix, LogInfo.bHasPrefix);

	return strLogName;
}

//日志路径
CString CLoggerManager::GetLogPath(const tagTimeLogInfo& LogInfo)
{
	CString strLogName;
	CString strLogPath;

	if (LogInfo.bHasPrefix)
	{
		if (LogInfo.szFileName[0] == 0)
		{
			strLogName.Format(TEXT("%s.%s"), LogInfo.szAppName, LogInfo.szServiceName);
		}
		else
		{
			strLogName.Format(TEXT("%s.%s%s%s"), LogInfo.szAppName, LogInfo.szServiceName, LogInfo.szConcatStr, LogInfo.szFileName);
		}
	}
	else
	{
		strLogName = LogInfo.szFileName;
	}

	strLogPath.Format(TEXT("%s\\%s\\%s\\%s"), m_szLogPath, LogInfo.szAppName, LogInfo.szServiceName, strLogName.GetString());

	return strLogPath;
}

//日志类型
CLogTypePtr CLoggerManager::GetLogType(const CString& strFormat, const CString& strLogType)
{
	//变量定义
	CLogTypePtr LogTypePtr = NULL;

	//日志类型
	CString sLogType = strLogType;

	if (sLogType.IsEmpty() == false)
	{
		int nPos = -1;
		sLogType = sLogType.MakeLower();

		if ((nPos = sLogType.Find(TEXT("day"), 0)) != -1)
		{
			int nValue = 1;
			CString strValue = sLogType.Left(nPos);
			if (strValue.IsEmpty() == false && strValue.SpanIncluding(TEXT("0123456789")) == strValue)
			{
				nValue = _ttoi(strValue.GetString());
			}

			LPCTSTR pszFormat = strFormat.IsEmpty() ? CLogByDay::FORMAT : strFormat.GetString();
			LogTypePtr = new CLogByDay(pszFormat, nValue);
		}
		else if ((nPos = sLogType.Find(TEXT("hour"), 0)) != -1)
		{
			int nValue = 1;
			CString strValue = sLogType.Left(nPos);
			if (strValue.IsEmpty() == false && strValue.SpanIncluding(TEXT("0123456789")) == strValue)
			{
				nValue = _ttoi(strValue.GetString());
			}

			LPCTSTR pszFormat = strFormat.IsEmpty() ? CLogByHour::FORMAT : strFormat.GetString();
			LogTypePtr = new CLogByHour(pszFormat, nValue);
		}
		else if ((nPos = sLogType.Find(TEXT("minute"), 0)) != -1)
		{
			int nValue = 1;
			CString strValue = sLogType.Left(nPos);
			if (strValue.IsEmpty() == false && strValue.SpanIncluding(TEXT("0123456789")) == strValue)
			{
				nValue = _ttoi(strValue.GetString());
			}

			LPCTSTR pszFormat = strFormat.IsEmpty() ? CLogByMinute::FORMAT : strFormat.GetString();
			LogTypePtr = new CLogByMinute(pszFormat, nValue);
		}
	}

	return LogTypePtr;
}

//清理日志
void CLoggerManager::CleanLoggers()
{
	//清理日志
	CString strLogName;
	CDayLogger* pDayLogger;

	//查找日志
	POSITION Position = m_MapDayLogger.GetStartPosition();
	while (Position!=NULL)
	{
		m_MapDayLogger.GetNextAssoc(Position, strLogName, pDayLogger);
		if (pDayLogger == NULL) break;

		//释放日志
		SafeDelete(pDayLogger);

	};

	//清除元素
	m_MapDayLogger.RemoveAll();
	m_MapLogFormat.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////////////