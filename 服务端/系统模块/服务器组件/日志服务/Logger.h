#ifndef LOGGER_HEAD_FILE
#define LOGGER_HEAD_FILE
#pragma once

//����ͷ�ļ�
#include <mutex>
#include "LoggerFile.h"
#include "LoggerServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//��������
class CLoggerRoll;
class CLoggerThread;

//���Ͷ���
typedef CWHAutoPtr<CLoggerRoll> CLoggerRollPtr;

//���Ͷ���
typedef CWHArray<CLoggerRollPtr> CLoggerArray;

//////////////////////////////////////////////////////////////////////////////////////////////

//�ӿڶ���
class CRollWrapperInterface
{
public:
	virtual ~CRollWrapperInterface() {}

	//��־����
public:
	//ˢ�»���
	virtual void Flush() = 0;

	//������־
	virtual void LogAny(LPCTSTR pszFormat, ...) = NULL;

	//��Ϣ��־
	virtual void LogInfo(LPCTSTR pszFormat, ...)=NULL;

	//������־
	virtual void LogDebug(LPCTSTR pszFormat, ...) = NULL;

	//������־
	virtual void LogWarn(LPCTSTR pszFormat, ...) = NULL;

	//������־
	virtual void LogError(LPCTSTR pszFormat, ...) = NULL;

	//��������
protected:
	//�����־
	virtual void LogString(LPCTSTR pszFormat, va_list args) = NULL;
	
};

//////////////////////////////////////////////////////////////////////////////////////////////
//��־����
class LOGGER_SERVICE_CLASS CLoggerCommon
{
	//��������
public:
	//���캯��
	CLoggerCommon();
	//��������
	virtual ~CLoggerCommon();

	//�ļ�����
public:
	//�ļ�����
	static bool IsFileExist(LPCTSTR pszFileName);
	//Ŀ¼����
	static bool IsDirectoryExist(LPCTSTR pszDirectory);
	//����Ŀ¼
	static bool MakeDirectory(LPCTSTR pszFileDirectory);
	//��ȡĿ¼
	static int ExtractFilePath(LPCTSTR pszFullFileName, TCHAR szBuffer[], WORD wBufferLen);

	//ʱ�亯��
public:
	//ʱ��ת��
	static LPCTSTR Time2Str(time_t time, LPCTSTR pszFormat, TCHAR szBuffer[], WORD wBufferLen);
};

//////////////////////////////////////////////////////////////////////////////////////////////
//��־�߳�
class LOGGER_SERVICE_CLASS CLoggerThread : public CServiceThread
{
	//��������
protected:
	std::mutex					m_Mutex;						//�������	
	std::condition_variable		m_CondVar;						//��������

	//�洢����
protected:
	CLoggerArray				m_LoggerArray;					//��������	

	//��������
public:
	//���캯��
	CLoggerThread();
	//��������
	virtual ~CLoggerThread();

	//�ӿں���
public:
	//��ֹ�߳�
	virtual bool ConcludeThread(DWORD dwMillSeconds);

	//�¼�����
protected:
	//�����¼�
	virtual bool OnEventThreadRun();
	//��ʼ�¼�
	virtual bool OnEventThreadStrat();
	//��ֹ�¼�
	virtual bool OnEventThreadConclude();

	//���ܺ���
public:
	//ˢ����־
	void Flush();
	//ע�����
	void RegisterLogger(CLoggerRollPtr& LoggerRollPtr);
	//ж�ض���
	void UnRegisterLogger(CLoggerRollPtr& LoggerRollPtr);
};


//////////////////////////////////////////////////////////////////////////////////////////////

//��־������
class LOGGER_SERVICE_CLASS CLoggerRoll : public CHandleBase
{
	//��������
protected:
	std::mutex							m_Mutex;							//�������
	std::mutex							m_BufMutex;							//�������
	CStringArray						m_strBuffer;						//��־����		
	CLoggerThread*						m_pLoggerThread;					//��־�߳�

	//��������
public:
	//���캯��
	CLoggerRoll();
	//��������
	virtual ~CLoggerRoll();

	//���ܺ���
public:
	//ˢ�»���
	void Flush();
	//д����־
	void Write(const CString& strContent);
	//ж���߳�
	void UnSetupThread();
	//��װ�߳�
	void SetupThread(CLoggerThread* pLoggerThread);	

	//���⺯��
public:
	//������־
	virtual void Roll(const CStringArray& strBuffer) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////////////////

//д������
class CWriteT
{
public:
	void operator()(CLoggerFile& LoggerFile, const CStringArray& strBuffer)
	{
		for (auto i = 0; i < strBuffer.GetCount(); i++)
		{
			CT2CA szBuffer(strBuffer[i].GetString());			

			//д���ļ�
			LoggerFile.SeekToEnd();
			LoggerFile.Write(szBuffer.m_psz,(DWORD)strlen(szBuffer.m_psz));
		}
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////

//��־��
template <class WriteT, template <class> class RollPolicy>
class CLogger : public RollPolicy<WriteT>::CRollWrapperI
{
public:
	enum
	{
		MAX_BUFFER_LENGTH = 1024 * 1,
	};

	//��־ͷ����
	enum
	{
		LH_MASK_TIME = 0x01,		//�뼶��ʱ��		
		LH_MASK_PID = 0x02,			//����ID
		LH_MASK_LEVEL = 0x04		//��־�ȼ�	
	};

	//��־����
	enum
	{
		LOG_LEVEL_ANY = 1,			//
		LOG_LEVEL_ERROR = 2,		//������־
		LOG_LEVEL_WARN = 3,			//������־
		LOG_LEVEL_INFO = 4,			//��Ϣ��־
		LOG_LEVEL_DEBUG = 5			//������־
	};	

	//���ñ���
protected:
	int							m_nHeadMask;					//ͷ������
	int							m_nLogLevel;					//��־�ȼ�
	TCHAR						m_szSeparator[16];				//�ָ���	
	LPCTSTR						m_szLevelName[5];				//��������

	//��־����
protected:
	CString						m_LogBuffer;					//��־����
	std::mutex					m_BufMutex;						//�������

	//��������
public:
	//���캯��		
	CLogger()
	{
		//��������
		m_szLevelName[0] = TEXT("NONE_LOG");
		m_szLevelName[1] = TEXT("ERROR");
		m_szLevelName[2] = TEXT("WARN");
		m_szLevelName[3] = TEXT("INFO");
		m_szLevelName[4] = TEXT("DEBUG");

		//���ñ���
		m_nHeadMask = LH_MASK_TIME;
		m_nLogLevel = LOG_LEVEL_DEBUG;
		StringCchCopy(m_szSeparator, CountArray(m_szSeparator), TEXT("|"));		
	}

	//��������
	virtual ~CLogger()
	{
		//ˢ�»���
		if (!m_LogBuffer.IsEmpty())
		{
			m_RollPtr->Write(m_LogBuffer);
		}
	}

	//���ܺ���
public:

	//�޸�����
	void ModHeadMask(int nMask, bool bAdd = true)
	{
		if (bAdd)
		{
			m_nHeadMask |= nMask;
		}
		else
		{
			m_nHeadMask &= ~nMask;
		}
	}

	//��ȡ����
	int GetHeadMask() const { return m_nHeadMask; }

	//�Ƿ���ĳ����
	bool HasHeadMask(int nMask) const { return (m_nHeadMask & nMask) != 0; }

	//��ȡ��־�ȼ�
	int GetLogLevel() const { return m_nLogLevel; }

	//������־�ȼ�
	bool SetLogLevel(int nLevel)
	{
		//У��ȼ�
		if (!IsLogLevelValid(nLevel))
		{
			return false;
		}

		//���õȼ�
		m_nLogLevel = nLevel;

		return true;
	}

	//�ȼ�У��
	bool IsLogLevelValid(int level)
	{
		switch (level)
		{
		case LOG_LEVEL_ANY:
		case LOG_LEVEL_ERROR:
		case LOG_LEVEL_WARN:
		case LOG_LEVEL_INFO:
		case LOG_LEVEL_DEBUG:
		{
			return true;
			break;
		}
		}

		return false;
	}

	//���õȼ�
	int SetLogLevel(LPCTSTR pszLevel)
	{
		if (lstrcmp(pszLevel, TEXT("ERROR")) == 0)
		{
			return SetLogLevel(LOG_LEVEL_ERROR);
		}
		else if (lstrcmp(pszLevel, TEXT("WARN")) == 0)
		{
			return setLogLevel(LOG_LEVEL_WARN);
		}
		else if (lstrcmp(pszLevel, TEXT("INFO")) == 0)
		{
			return setLogLevel(LOG_LEVEL_INFO);
		}
		else if (lstrcmp(pszLevel, TEXT("DEBUG")) == 0)
		{
			return setLogLevel(LOG_LEVEL_DEBUG);
		}
		else if (lstrcmp(pszLevel, TEXT("NONE")) == 0)
		{
			return setLogLevel(LOG_LEVEL_ANY);
		}

		return -1;
	}

	//�Ƿ���Ҫ����־
	bool IsNeedLog(int nLevel) { return nLevel <= m_nLogLevel; }

	//�Ƿ���Ҫ����־
	bool IsNeedLog(LPCTSTR pszLevel)
	{
		if (lstrcmp(pszLevel, TEXT("ERROR")) == 0)
		{
			return IsNeedLog(LOG_LEVEL_ERROR);
		}
		else if (lstrcmp(pszLevel, TEXT("WARN")) == 0)
		{
			return IsNeedLog(LOG_LEVEL_WARN);
		}
		else if (lstrcmp(pszLevel, TEXT("INFO")) == 0)
		{
			return IsNeedLog(LOG_LEVEL_INFO);
		}
		else if (lstrcmp(pszLevel, TEXT("DEBUG")) == 0)
		{
			return IsNeedLog(LOG_LEVEL_DEBUG);
		}
		else if (lstrcmp(pszLevel, TEXT("NONE")) == 0)
		{
			return IsNeedLog(LOG_LEVEL_ANY);
		}
		else
		{
			return true;
		}
	}

	//��־�ָ����
	void SetSeparator(LPCTSTR pszSeparator)
	{
		StringCchCopy(m_szSeparator, CountArray(m_szSeparator), pszSeparator);
	}

	//������־
	virtual void LogAny(LPCTSTR pszFormat, ...)
	{
		//У�鼶��
		if (IsNeedLog(LOG_LEVEL_ANY) == true)
		{
			//��ȡ����	
			va_list args;
			va_start(args, pszFormat);
			LogString(pszFormat, args);
			va_end(args);
		}
	}

	//��Ϣ��־
	virtual void LogInfo(LPCTSTR pszFormat, ...)
	{
		//У�鼶��
		if (IsNeedLog(LOG_LEVEL_INFO) == true)
		{
			//��ȡ����	
			va_list args;
			va_start(args, pszFormat);
			LogString(pszFormat, args);
			va_end(args);
		}
	}

	//������־
	virtual void LogDebug(LPCTSTR pszFormat, ...)
	{
		//У�鼶��
		if (IsNeedLog(LOG_LEVEL_DEBUG) == true)
		{
			//��ȡ����	
			va_list args;
			va_start(args, pszFormat);
			LogString(pszFormat, args);
			va_end(args);
		}
	}

	//������־
	virtual void LogWarn(LPCTSTR pszFormat, ...)
	{
		//У�鼶��
		if (IsNeedLog(LOG_LEVEL_WARN) == true)
		{
			//��ȡ����	
			va_list args;
			va_start(args, pszFormat);
			LogString(pszFormat, args);
			va_end(args);
		}
	}

	//������־
	virtual void LogError(LPCTSTR pszFormat, ...)
	{
		//У�鼶��
		if (IsNeedLog(LOG_LEVEL_ERROR) == true)
		{
			//��ȡ����	
			va_list args;
			va_start(args, pszFormat);
			LogString(pszFormat, args);
			va_end(args);
		}		
	}	

//��������	
protected:
	//�����־
	virtual void LogString(LPCTSTR pszFormat, va_list args)
	{
		//��������
		CString strContent;

		//ͷ����Ϣ
		GetHeader(strContent, m_nLogLevel);

		//��־��Ϣ		
		strContent.AppendFormatV(pszFormat, args);

		//��ӻ��з�
		if (HasHeadMask(0xffff)!=0) strContent.Append(TEXT("\r\n"));

		//д�뻺��
		m_RollPtr->Write(strContent);

		//��������
		//std::lock_guard<std::mutex> lock(m_BufMutex);

		////����ж�
		//if ((CountStringBuffer(m_LogBuffer.GetString()) + CountStringBuffer(strContent.GetString())) > MAX_BUFFER_LENGTH)
		//{
		//	//ˢ�»���
		//	m_RollPtr->Write(m_LogBuffer);

		//	//�����־
		//	m_LogBuffer.Empty();
		//}

		////�����־
		//m_LogBuffer.Append(strContent);
	}

	//��ȡͷ����Ϣ
	void GetHeader(CString & strBuffer, int nLevel)
	{
		//ʱ����Ϣ
		if (HasHeadMask(CLogger::LH_MASK_TIME))
		{
			//��ǰʱ��
			auto clock = time(NULL);

			//����ʱ��
			tm tt;
			::localtime_s(&tt, &clock);

			//��ʽ�ַ���
			LPCTSTR szFormat = TEXT("%04d-%02d-%02d %02d:%02d:%02d%s");

			strBuffer.Format(szFormat, tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, m_szSeparator);
		}

		//������Ϣ
		if (HasHeadMask(CLogger::LH_MASK_PID))
		{
			strBuffer.AppendFormat(TEXT("%d%s"), ::GetCurrentThreadId(), m_szSeparator);
		}

		//��־����
		if (HasHeadMask(CLogger::LH_MASK_LEVEL))
		{
			strBuffer.AppendFormat(TEXT("%s%s"), m_szLevelName[nLevel], m_szSeparator);
		}
	}	
};

//////////////////////////////////////////////////////////////////////////////////////////////

template <typename RollPolicy>
class CRollWrapperBase : public CRollWrapperInterface
{
	typedef CWHAutoPtr<RollPolicy> CRollPllicyPtr;

	//��������
protected:
	CRollPllicyPtr							m_RollPtr;

	//��������
public:	
	//���캯��
	CRollWrapperBase()
	{
		m_RollPtr = new RollPolicy;
	}
	//������������
	~CRollWrapperBase()
	{
		m_RollPtr->UnSetupThread();
	}

	//��װ�߳�
	void SetupThread(CLoggerThread * pLoggerThread) 
	{
		m_RollPtr->SetupThread(pLoggerThread); 
	}

	//ȡ���߳�
	void UnSetupThread() 
	{
		m_RollPtr->UnSetupThread(); 
	}

	//��ȡдʵ��
	typename RollPolicy::T& GetWriteT() 
	{
		return m_RollPtr->GetWriteT();
	}

	//д����־
	void Roll(const CString & strContent) 
	{
		m_RollPtr->Write(strContent); 
	}

	//��ȡRollʵ��
	CRollPllicyPtr& GetRoll() 
	{
		return m_RollPtr; 
	}

	//�첽ˢ��
	void Flush() 
	{
		m_RollPtr->Flush(); 
	}
};

////////////////////////////////////////////////////////////////////////////////

//������־��С����
template <typename WriteT>
class CRollBySize : public CLoggerRoll, public std::mutex
{
public:
	typedef WriteT T;

	//�������
protected:
	WriteT						m_WriteT;							//д����
	CLoggerFile					m_LogFile;							//��־�ļ�	

	//���ñ���
protected:
	TCHAR						m_szPath[256];						//�ļ�·��
	WORD						m_wMaxNum;							//�������
	DWORD						m_dwMaxSize;						//����С
	time_t						m_tUpdateTime;						//���ʱ��
	WORD						m_wCheckInterval;					//�����		

public:
	
	//��װ��(�ṩ�ӿ�)	
	class CRollWrapperI : public CRollWrapperBase<CRollBySize<WriteT>>
	{
	public:
		//��ʼ��
		void Init(LPCTSTR pszPath, int dwMaxSize = 5000000, int wMaxNum = 10)
		{
			this->m_RollPtr->Init(pszPath, dwMaxSize, wMaxNum);
		}

		//��ȡ·��
		LPCTSTR GetPath() { return this->m_RollPtr->GetPath(); }

		//��ȡ������
		int GetMaxNum() { return this->m_RollPtr->GetMaxNum(); }

		//��ȡ����С
		int GetMaxSize() { return this->m_RollPtr->GetMaxSize(); }

		//����������
		void SetMaxNum(int wMaxNum) { this->m_RollPtr->SetMaxNum(wMaxNum); }

		//��������С
		void SetMaxSize(int dwMaxSize) { this->m_RollPtr->SetMaxSize(dwMaxSize); }

		//����·��
		void SetPath(LPCTSTR pszPath) { this->m_RollPtr->SetPath(pszPath); }		
	};

	//��������
public:
	//���캯��
	CRollBySize()
	{
		//���ñ���
		m_szPath[0] = 0;
		m_wMaxNum = 10;
		m_dwMaxSize = 5000000;
		m_wCheckInterval = 0;
		m_tUpdateTime = time(NULL);
	}

	//��������
	virtual ~CRollBySize()
	{
		if (m_LogFile.IsValid()) m_LogFile.Close();
	}

	//���ú���
public:
	//��ȡ����
	int GetMaxNum()
	{
		std::lock_guard<std::mutex> lock(*this);
		return m_wMaxNum;
	}

	//���ø���
	void SetMaxNum(int wMaxNum)
	{
		std::lock_guard<std::mutex> lock(*this);
		m_wMaxNum = wMaxNum;
	}

	//��ȡ·��
	LPCTSTR GetPath()
	{
		std::lock_guard<std::mutex> lock(*this);
		return m_szPath;
	}

	//����·��
	void SetPath(LPCTSTR pszPath)
	{
		std::lock_guard<std::mutex> lock(*this);
		StringCchCopy(m_szPath, CountArray(m_szPath), pszPath);
	}

	//��ȡ��С
	int GetMaxSize()
	{
		std::lock_guard<std::mutex> lock(*this);
		return m_dwMaxSize;
	}

	//���ô�С
	void SetMaxSize(int dwMaxSize)
	{
		std::lock_guard<std::mutex> lock(*this);
		m_dwMaxSize = dwMaxSize;
	}

	//��Ϣ����
public:
	//��ȡдʵ��
	WriteT& GetWriteT() { return m_WriteT; }

	//���ܺ���
public:
	//��ʼ��
	void Init(LPCTSTR pszPath, int dwMaxSize = 5000000, int wMaxNum = 10)
	{
		std::lock_guard<std::mutex> lock(*this);

		//���ñ���		
		m_dwMaxSize = dwMaxSize;
		m_wMaxNum = wMaxNum;
		StringCchCopy(m_szPath, CountArray(m_szPath), pszPath);
	}

	//������־
	void Roll(const CStringArray& strBuffer)
	{
		std::lock_guard<std::mutex> lock(*this);

		//У��·��
		if (m_szPath[0] == 0)
		{
			return;
		}

		//������
		time_t now = time(NULL);
		time_t interval = now - m_tUpdateTime;

		//У��ʱ��
		if (interval > 5 || interval < 0)
		{
			m_tUpdateTime = now;
			m_LogFile.Close();
		}

		//�����־�ļ��Ƿ��		
		if (!m_LogFile.IsValid())
		{
			//����·��			
			TCHAR szLogFileName[256] = { 0 };
			_sntprintf_s(szLogFileName, CountArray(szLogFileName), TEXT("%s.log"), m_szPath);

			//��ȡ·��
			TCHAR szLogFilePath[256] = { 0 };
			CLoggerCommon::ExtractFilePath(m_szPath, szLogFilePath, CountArray(szLogFilePath));

			//����Ŀ¼
			if (!CLoggerCommon::IsDirectoryExist(szLogFilePath))
			{
				CLoggerCommon::MakeDirectory(szLogFilePath);
			}

			//���ļ�
			m_LogFile.Create(szLogFileName, GENERIC_READ | GENERIC_WRITE | OPEN_ALWAYS);
		}

		//д���ļ�
		if (m_LogFile.IsValid())
		{
			m_WriteT(m_LogFile, strBuffer);			
		}

		//У����
		if (interval <= 5)
		{
			return;
		}

		//У���ļ���С
		if (m_LogFile.GetFileLength() < m_dwMaxSize)
		{
			return;
		}

		//�ļ���С��������,ɾ�����һ���ļ�		
		TCHAR szLogFileName[256] = { 0 };
		_sntprintf_s(szLogFileName, CountArray(szLogFileName), TEXT("%s%d.log"), m_szPath, m_wMaxNum - 1);

		if (CLoggerCommon::IsFileExist(szLogFileName) && !DeleteFile(szLogFileName))
		{
			return;
		}

		//��log�ļ�����shift, xxx1.log=>xxx2.log,��һ�ļ�����Ϊxxx.log
		for (int i = m_wMaxNum - 2; i >= 0; i--)
		{
			if (i == 0)
			{
				_sntprintf_s(szLogFileName, CountArray(szLogFileName), TEXT("%s.log"), m_szPath);
			}
			else
			{
				_sntprintf_s(szLogFileName, CountArray(szLogFileName), TEXT("%s%d.log"), m_szPath, i);
			}

			if (CLoggerCommon::IsFileExist(szLogFileName))
			{
				//����·��
				TCHAR szNewLogFileName[256] = { 0 };
				_sntprintf_s(szNewLogFileName, CountArray(szNewLogFileName), TEXT("%s%d.log"), m_szPath, i + 1);

				//�ļ�������
				rename(CT2CA(szLogFileName), CT2CA(szNewLogFileName));
			}
		}

		//�رմ�
		m_LogFile.Close();
		m_LogFile.Create(szLogFileName, GENERIC_READ | GENERIC_WRITE | OPEN_ALWAYS);
		if (!m_LogFile.IsValid())
		{
		}
	}	
};

typedef CLogger<CWriteT, CRollBySize> CRollLogger;

//////////////////////////////////////////////////////////////////////////////////////////////////////

//��־����
class LOGGER_SERVICE_CLASS CLogType : public CHandleBase
{
	//��������
protected:
	time_t						m_tNextTime;							//ʱ��ڵ�
	size_t						m_Frequency;							//Ƶ��ֵ
	TCHAR						m_szFormat[64];							//ʱ���ʽ
	TCHAR						m_szDescribe[64];						//��������	
	TCHAR						m_szNextCutTime[128];					//ʱ��ڵ�	

	//��������
public:
	//���캯��
	CLogType();
	//��������
	virtual ~CLogType();

	//�麯��
public:
	//Ƶ��ֵ	
	virtual size_t Frequence() = 0;
	//��ʼʱ��
	virtual LPCTSTR GetInitTime(TCHAR szBuffer[], WORD wBufferLen);
	//���ؼ�¼ʱ���
	virtual LPCTSTR GetNewTime(time_t time, TCHAR szBuffer[], WORD wBufferLen);

	//��Ϣ����
public:
	//ת�����ַ�����ʽ
	inline LPCTSTR ToString() { return m_szDescribe; }	

	//��������
protected:
	//������һ��ʱ���	
	void Init(LPCTSTR pszFormat = TEXT("%Y%m%d"), size_t frequency = 1);
};

typedef CWHAutoPtr<CLogType> CLogTypePtr;

/////////////////////////////////////////////////////////////////////////////////////////////////////

class LOGGER_SERVICE_CLASS CLogByDay : public CLogType
{
public:
	static LPCTSTR FORMAT;

public:
	explicit CLogByDay(LPCTSTR pszFormat = TEXT("%Y%m%d"), size_t frequency = 1)
	{
		Init(pszFormat, frequency);		
		_sntprintf_s(m_szDescribe, CountArray(m_szDescribe), TEXT("%zdday"), m_Frequency);
	}
	virtual size_t Frequence()
	{
		const size_t DAY_IN_SECOND = 60 * 60 * 24;
		return m_Frequency * DAY_IN_SECOND;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

class LOGGER_SERVICE_CLASS CLogByHour : public CLogType
{
public:
	static LPCTSTR FORMAT;

public:
	explicit CLogByHour(LPCTSTR pszFormat = TEXT("%Y%m%d%H"), size_t frequency = 1)
	{
		Init(pszFormat, frequency);
		_sntprintf_s(m_szDescribe, CountArray(m_szDescribe), TEXT("%zdhour"), m_Frequency);
	}

	virtual size_t Frequence()
	{
		const size_t HOUR_IN_SECOND = 60 * 60;
		return m_Frequency * HOUR_IN_SECOND;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

class LOGGER_SERVICE_CLASS CLogByMinute : public CLogType
{
public:
	static LPCTSTR FORMAT;

public:
	explicit CLogByMinute(LPCTSTR pszFormat = TEXT("%Y%m%d%H%M"), size_t frequency = 1)
	{
		Init(pszFormat, frequency);
		_sntprintf_s(m_szDescribe, CountArray(m_szDescribe), TEXT("%zdminute"), m_Frequency);
	}
	virtual size_t Frequence()
	{
		const size_t MINUTE_IN_SECOND = 60;
		return m_Frequency * MINUTE_IN_SECOND;
	}
};

/////////////////////////////////////////////////////////////////////////////////////////////////////

// ����ʱ�������־
template <typename WriteT>
class CRollByTime : public CLoggerRoll, public std::mutex
{
public:
	typedef WriteT T;

	//�������
protected:
	WriteT					m_WriteT;							//д����
	CLoggerFile				m_LogFile;							//��־�ļ�
	CLogTypePtr				m_LogTypePtr;						//��־����

	//��־����
protected:
	bool					m_bHasSufix;						//��׺��ʶ
	bool					m_bRemoteType;						//Զ������

	//���ñ���
protected:	
	TCHAR					m_szPath[256];						//�ļ�·��
	TCHAR					m_szFormat[64];						//�����ʽ
	TCHAR					m_szCurrentTime[128];				//��ǰʱ��
	TCHAR					m_szConcatStr[16];					//�����ַ�
	time_t					m_tCheckInterval;					//�����			

public:
	
	// ��װ��(�ӿ���)	 
	class CRollWrapperI : public CRollWrapperBase<CRollByTime<WriteT>>
	{
	public:

		//��ʼ��.		
		void Init(LPCTSTR pszPath, LPCTSTR pszFormat = TEXT("%Y%m%d"), bool bHasSufix = true, LPCTSTR pszConcatstr = TEXT("_"), const CLogTypePtr& logTypePtr = NULL, bool bRemoteType=false)
		{
			this->m_RollPtr->Init(pszPath, pszFormat, bHasSufix, pszConcatstr, logTypePtr, bRemoteType);
		}

		//Զ�̱�־
		void SetRemote(bool bEnabled) { this->m_RollPtr->SetRemote(bEnabled); }

		//��ȡ��־·��.		
		LPCTSTR GetPath() { return this->m_RollPtr->GetPath(); }

		//�����ļ�·��		 
		void SetPath(LPCTSTR pszFath) { this->m_RollPtr->SetPath(pszFath); }

		//��ȡ��ʽ
		LPCTSTR GetFormat() { return this->m_RollPtr->GetFormat(); }

		//���ø�ʽ
		void SetFormat(LPCTSTR pszFormat) { this->m_RollPtr->SetFormat(pszFormat); }
	};

	//��������
public:
	//���캯��
	CRollByTime() : m_tCheckInterval(0), m_LogTypePtr(NULL)
	{
		//��־����
		m_bHasSufix = true;
		m_bRemoteType = false;

		//���ñ���
		m_szPath[0] = 0;
		m_szFormat[0] = 0;
		m_szCurrentTime[0] = 0;
		StringCchCopy(m_szConcatStr, CountArray(m_szConcatStr), TEXT("_"));
		m_tCheckInterval = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	}

	//��������
	virtual ~CRollByTime()
	{
		if (m_LogFile.IsValid()) m_LogFile.Close();
	}

	//���ú���
public:
	//��ȡ·��
	LPCTSTR GetPath()
	{
		std::lock_guard<std::mutex> lock(*this);
		return m_szPath;
	}

	//����·��
	void SetPath(LPCTSTR pszPath)
	{
		std::lock_guard<std::mutex> lock(*this);
		StringCchCopy(m_szPath, CountArray(m_szPath), pszPath);
	}

	//��ȡ��ʽ
	LPCTSTR GetFormat()
	{
		std::lock_guard<std::mutex> lock(*this);
		return m_szFormat;
	}

	//���ø�ʽ
	void SetFormat(LPCTSTR pszFormat)
	{
		std::lock_guard<std::mutex> lock(*this);
		StringCchCopy(m_szFormat, CountArray(m_szFormat), pszFormat);
	}

	//Զ�̱�־
	void SetRemote(bool bEnabled) 
	{
		std::lock_guard<std::mutex> lock(*this);
		m_bRemoteType = bEnabled;
	}

	//��Ϣ����
public:
	//��ȡдʵ��
	WriteT& GetWriteT() { return m_WriteT; }

	//���ܺ���
public:	
	//��־����	
	void Roll(const CStringArray& strBuffer)
	{
		std::lock_guard<std::mutex> lock(*this);

		//У��·��
		if (m_szPath[0] == 0) return;

		//Զ����־
		if (m_bRemoteType)
		{
			m_WriteT(m_LogFile, strBuffer);

			return;
		}

		TCHAR szNowTime[128] = { 0 };
		time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

		//��������˼�¼����,��ʹ�ü�¼����������ʱ��		
		if (m_LogTypePtr)
		{
			//��ȡʱ��
			m_LogTypePtr->GetNewTime(now, szNowTime, CountArray(szNowTime));

			//ʱ���Խ��			
			if (szNowTime[0] != 0)
			{
				StringCchCopy(m_szCurrentTime, CountArray(m_szCurrentTime), szNowTime);
				m_LogFile.Close();
			}
			else
			{
				StringCchCopy(szNowTime, CountArray(szNowTime), m_szCurrentTime);
			}
		}
		else
		{
			CLoggerCommon::Time2Str(now, m_szFormat, szNowTime, CountArray(szNowTime));

			//���ʱ���Ƿ��ʱ����, ��ʱ���ر��ļ�
			if (lstrcmp(m_szCurrentTime, szNowTime) != 0)
			{
				StringCchCopy(m_szCurrentTime, CountArray(m_szCurrentTime), szNowTime);
				m_LogFile.Close();
			}
		}

		//ÿ��10s, ���´�һ���ļ�, �����ļ���ɾ���������ͷ�		
		if (now - m_tCheckInterval > 10 || now - m_tCheckInterval < 0)
		{
			m_tCheckInterval = now;
			m_LogFile.Close();
		}

		if (!m_LogFile.IsValid())
		{
			//��������
			TCHAR szLogFileName[256] = TEXT("");
			TCHAR szLogFilePath[256] = TEXT("");
			_sntprintf_s(szLogFileName, CountArray(szLogFileName), TEXT("%s%s%s%s"), m_szPath, m_szConcatStr, szNowTime, m_bHasSufix ? TEXT(".log") : TEXT(""));

			//��ȡ·��
			CLoggerCommon::ExtractFilePath(m_szPath, szLogFilePath, CountArray(szLogFilePath));

			//����Ŀ¼
			if (!CLoggerCommon::IsDirectoryExist(szLogFilePath))
			{
				CLoggerCommon::MakeDirectory(szLogFilePath);
			}

			m_LogFile.Create(szLogFileName, GENERIC_READ | GENERIC_WRITE, OPEN_ALWAYS);

			if (!m_LogFile.IsValid())
			{
				ASSERT(FALSE);
			}
		}

		//д��־
		if (m_LogFile.IsValid())
		{
			m_WriteT(m_LogFile, strBuffer);		
			m_LogFile.Close();
		}
	}

	//��ʼ��
	void Init(LPCTSTR pszPath, LPCTSTR pszFormat = TEXT("%Y%m%d"), bool bHasSufix = true, LPCTSTR pszConcatstr = TEXT("_"), const CLogTypePtr& logTypePtr = NULL, bool bRemoteType = false)
	{
		std::lock_guard<std::mutex> lock(*this);

		//���ñ���
		m_bHasSufix = bHasSufix;
		m_LogTypePtr = logTypePtr;

		//��־����
		m_bRemoteType = bRemoteType;

		//�����ַ�
		StringCchCopy(m_szPath, CountArray(m_szPath), pszPath);
		StringCchCopy(m_szFormat, CountArray(m_szFormat), pszFormat);
		StringCchCopy(m_szConcatStr, CountArray(m_szConcatStr), pszConcatstr);

		if (m_LogTypePtr)
		{
			m_LogTypePtr->GetInitTime(m_szCurrentTime, CountArray(m_szCurrentTime));
		}
		else
		{
			CLoggerCommon::Time2Str(time(NULL), m_szFormat, m_szCurrentTime, CountArray(m_szCurrentTime));
		}
	}
};

typedef CLogger<CWriteT, CRollByTime> CDayLogger;

//////////////////////////////////////////////////////////////////////////////////////////////

#endif

