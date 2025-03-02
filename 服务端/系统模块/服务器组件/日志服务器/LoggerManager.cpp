#include "stdafx.h"
#include "LoggerManager.h"

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CLoggerManager::CLoggerManager()
{
	//���ñ���
	m_bService = false;
	m_szLogPath[0] = 0;

	//��ʼ���ֵ�
	m_MapDayLogger.InitHashTable(103);
	m_MapLogFormat.InitHashTable(103);
}

//��������
CLoggerManager::~CLoggerManager()
{
	//�����߳�
	if (m_LoggerThread.IsRuning())
	{
		m_LoggerThread.ConcludeThread(INFINITE);
	}

	//������־
	CleanLoggers();
}

//��������
bool CLoggerManager::StartService()
{
	//У��״̬
	ASSERT (m_bService==false);
	if (m_bService == true) return false;

	//�����߳�
	m_LoggerThread.StartThread();

	//����״̬
	m_bService = true;

	return true;
}

//ֹͣ����
bool CLoggerManager::ConcludeService()
{
	//У��״̬
	if (m_bService == false) return true;

	//�����߳�
	if (m_LoggerThread.IsRuning())
	{
		m_LoggerThread.ConcludeThread(INFINITE);
	}

	//������־
	CleanLoggers();

	//����״̬
	m_bService = false;
	
	return true;
}

//����·��
void CLoggerManager::SetLogPath(LPCTSTR pszLogPath)
{
	StringCchCopy(m_szLogPath,CountArray(m_szLogPath),pszLogPath);
}

//��ȡ��־
CDayLogger& CLoggerManager::GetDayLogger(const tagTimeLogInfo& LogInfo)
{
	//��־����
	CString strFormat = LogInfo.szFormat;
	CString strLogName = GetLogName(LogInfo);

	//������־
	CDayLogger* pDayLogger = NULL;
	if (m_MapDayLogger.Lookup(strLogName, pDayLogger) == FALSE)
	{
		return MakeDayLogger(LogInfo, strLogName);
	}

	//���Ҹ�ʽ
	CString strLogForamt;
	if (m_MapLogFormat.Lookup(strLogName, strLogForamt) == TRUE)
	{
		//У���ʽ
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


//������־
CDayLogger& CLoggerManager::MakeDayLogger(const tagTimeLogInfo& LogInfo, const CString& strLogName)
{
	//��ȡ��Ϣ
	CString strFormat = LogInfo.szFormat;
	CString strLogType = LogInfo.szLogType;
	CString strLogPath = GetLogPath(LogInfo);

	//������־
	auto pDayLogger = new CDayLogger();

	pDayLogger->Init(strLogPath, strFormat, LogInfo.bHasSufix, LogInfo.szConcatStr, GetLogType(strFormat, strLogType));
	pDayLogger->SetSeparator(LogInfo.szSeparator);
	pDayLogger->SetupThread(&m_LoggerThread);

	//���б�ʶ����Ҫ
	pDayLogger->ModHeadMask(0xffff, false);

	//ӳ����־
	m_MapDayLogger[strLogName] = pDayLogger;
	m_MapLogFormat[strLogName] = strFormat;

	return (*pDayLogger);
}

//��־����
CString CLoggerManager::GetLogName(const tagTimeLogInfo& LogInfo)
{
	CString strLogName;
	strLogName.Format(TEXT("%s%s%s%s%s%s%s%d%d"), LogInfo.szAppName, LogInfo.szServiceName, LogInfo.szFileName, LogInfo.szFormat, LogInfo.szConcatStr, LogInfo.szSeparator, LogInfo.szLogType, LogInfo.bHasSufix, LogInfo.bHasPrefix);

	return strLogName;
}

//��־·��
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

//��־����
CLogTypePtr CLoggerManager::GetLogType(const CString& strFormat, const CString& strLogType)
{
	//��������
	CLogTypePtr LogTypePtr = NULL;

	//��־����
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

//������־
void CLoggerManager::CleanLoggers()
{
	//������־
	CString strLogName;
	CDayLogger* pDayLogger;

	//������־
	POSITION Position = m_MapDayLogger.GetStartPosition();
	while (Position!=NULL)
	{
		m_MapDayLogger.GetNextAssoc(Position, strLogName, pDayLogger);
		if (pDayLogger == NULL) break;

		//�ͷ���־
		SafeDelete(pDayLogger);

	};

	//���Ԫ��
	m_MapDayLogger.RemoveAll();
	m_MapLogFormat.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////////////