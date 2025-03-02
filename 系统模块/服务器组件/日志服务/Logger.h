#ifndef LOGGER_HEAD_FILE
#define LOGGER_HEAD_FILE
#pragma once

//引入头文件
#include <mutex>
#include "LoggerFile.h"
#include "LoggerServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//类型声明
class CLoggerRoll;
class CLoggerThread;

//类型定义
typedef CWHAutoPtr<CLoggerRoll> CLoggerRollPtr;

//类型定义
typedef CWHArray<CLoggerRollPtr> CLoggerArray;

//////////////////////////////////////////////////////////////////////////////////////////////

//接口定义
class CRollWrapperInterface
{
public:
	virtual ~CRollWrapperInterface() {}

	//日志函数
public:
	//刷新缓存
	virtual void Flush() = 0;

	//所有日志
	virtual void LogAny(LPCTSTR pszFormat, ...) = NULL;

	//信息日志
	virtual void LogInfo(LPCTSTR pszFormat, ...)=NULL;

	//调试日志
	virtual void LogDebug(LPCTSTR pszFormat, ...) = NULL;

	//警告日志
	virtual void LogWarn(LPCTSTR pszFormat, ...) = NULL;

	//错误日志
	virtual void LogError(LPCTSTR pszFormat, ...) = NULL;

	//包含函数
protected:
	//输出日志
	virtual void LogString(LPCTSTR pszFormat, va_list args) = NULL;
	
};

//////////////////////////////////////////////////////////////////////////////////////////////
//日志助手
class LOGGER_SERVICE_CLASS CLoggerCommon
{
	//函数定义
public:
	//构造函数
	CLoggerCommon();
	//析构函数
	virtual ~CLoggerCommon();

	//文件函数
public:
	//文件存在
	static bool IsFileExist(LPCTSTR pszFileName);
	//目录存在
	static bool IsDirectoryExist(LPCTSTR pszDirectory);
	//创建目录
	static bool MakeDirectory(LPCTSTR pszFileDirectory);
	//提取目录
	static int ExtractFilePath(LPCTSTR pszFullFileName, TCHAR szBuffer[], WORD wBufferLen);

	//时间函数
public:
	//时间转换
	static LPCTSTR Time2Str(time_t time, LPCTSTR pszFormat, TCHAR szBuffer[], WORD wBufferLen);
};

//////////////////////////////////////////////////////////////////////////////////////////////
//日志线程
class LOGGER_SERVICE_CLASS CLoggerThread : public CServiceThread
{
	//变量定义
protected:
	std::mutex					m_Mutex;						//互斥对象	
	std::condition_variable		m_CondVar;						//条件变量

	//存储变量
protected:
	CLoggerArray				m_LoggerArray;					//对象数组	

	//函数定义
public:
	//构造函数
	CLoggerThread();
	//析构函数
	virtual ~CLoggerThread();

	//接口函数
public:
	//终止线程
	virtual bool ConcludeThread(DWORD dwMillSeconds);

	//事件函数
protected:
	//运行事件
	virtual bool OnEventThreadRun();
	//开始事件
	virtual bool OnEventThreadStrat();
	//终止事件
	virtual bool OnEventThreadConclude();

	//功能函数
public:
	//刷新日志
	void Flush();
	//注册对象
	void RegisterLogger(CLoggerRollPtr& LoggerRollPtr);
	//卸载对象
	void UnRegisterLogger(CLoggerRollPtr& LoggerRollPtr);
};


//////////////////////////////////////////////////////////////////////////////////////////////

//日志滚动类
class LOGGER_SERVICE_CLASS CLoggerRoll : public CHandleBase
{
	//变量定义
protected:
	std::mutex							m_Mutex;							//互斥对象
	std::mutex							m_BufMutex;							//互斥对象
	CStringArray						m_strBuffer;						//日志缓存		
	CLoggerThread*						m_pLoggerThread;					//日志线程

	//函数定义
public:
	//构造函数
	CLoggerRoll();
	//析构函数
	virtual ~CLoggerRoll();

	//功能函数
public:
	//刷新缓存
	void Flush();
	//写入日志
	void Write(const CString& strContent);
	//卸载线程
	void UnSetupThread();
	//安装线程
	void SetupThread(CLoggerThread* pLoggerThread);	

	//虚拟函数
public:
	//滚动日志
	virtual void Roll(const CStringArray& strBuffer) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////////////////

//写操作类
class CWriteT
{
public:
	void operator()(CLoggerFile& LoggerFile, const CStringArray& strBuffer)
	{
		for (auto i = 0; i < strBuffer.GetCount(); i++)
		{
			CT2CA szBuffer(strBuffer[i].GetString());			

			//写入文件
			LoggerFile.SeekToEnd();
			LoggerFile.Write(szBuffer.m_psz,(DWORD)strlen(szBuffer.m_psz));
		}
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////

//日志类
template <class WriteT, template <class> class RollPolicy>
class CLogger : public RollPolicy<WriteT>::CRollWrapperI
{
public:
	enum
	{
		MAX_BUFFER_LENGTH = 1024 * 1,
	};

	//日志头掩码
	enum
	{
		LH_MASK_TIME = 0x01,		//秒级别时间		
		LH_MASK_PID = 0x02,			//进程ID
		LH_MASK_LEVEL = 0x04		//日志等级	
	};

	//日志级别
	enum
	{
		LOG_LEVEL_ANY = 1,			//
		LOG_LEVEL_ERROR = 2,		//错误日志
		LOG_LEVEL_WARN = 3,			//警告日志
		LOG_LEVEL_INFO = 4,			//信息日志
		LOG_LEVEL_DEBUG = 5			//调试日志
	};	

	//配置变量
protected:
	int							m_nHeadMask;					//头部掩码
	int							m_nLogLevel;					//日志等级
	TCHAR						m_szSeparator[16];				//分隔符	
	LPCTSTR						m_szLevelName[5];				//级别名称

	//日志缓冲
protected:
	CString						m_LogBuffer;					//日志缓冲
	std::mutex					m_BufMutex;						//互斥对象

	//函数定义
public:
	//构造函数		
	CLogger()
	{
		//级别名称
		m_szLevelName[0] = TEXT("NONE_LOG");
		m_szLevelName[1] = TEXT("ERROR");
		m_szLevelName[2] = TEXT("WARN");
		m_szLevelName[3] = TEXT("INFO");
		m_szLevelName[4] = TEXT("DEBUG");

		//设置变量
		m_nHeadMask = LH_MASK_TIME;
		m_nLogLevel = LOG_LEVEL_DEBUG;
		StringCchCopy(m_szSeparator, CountArray(m_szSeparator), TEXT("|"));		
	}

	//析构函数
	virtual ~CLogger()
	{
		//刷新缓冲
		if (!m_LogBuffer.IsEmpty())
		{
			m_RollPtr->Write(m_LogBuffer);
		}
	}

	//功能函数
public:

	//修改掩码
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

	//获取掩码
	int GetHeadMask() const { return m_nHeadMask; }

	//是否有某掩码
	bool HasHeadMask(int nMask) const { return (m_nHeadMask & nMask) != 0; }

	//获取日志等级
	int GetLogLevel() const { return m_nLogLevel; }

	//设置日志等级
	bool SetLogLevel(int nLevel)
	{
		//校验等级
		if (!IsLogLevelValid(nLevel))
		{
			return false;
		}

		//设置等级
		m_nLogLevel = nLevel;

		return true;
	}

	//等级校验
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

	//设置等级
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

	//是否需要打日志
	bool IsNeedLog(int nLevel) { return nLevel <= m_nLogLevel; }

	//是否需要打日志
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

	//日志分割符号
	void SetSeparator(LPCTSTR pszSeparator)
	{
		StringCchCopy(m_szSeparator, CountArray(m_szSeparator), pszSeparator);
	}

	//所有日志
	virtual void LogAny(LPCTSTR pszFormat, ...)
	{
		//校验级别
		if (IsNeedLog(LOG_LEVEL_ANY) == true)
		{
			//获取参数	
			va_list args;
			va_start(args, pszFormat);
			LogString(pszFormat, args);
			va_end(args);
		}
	}

	//信息日志
	virtual void LogInfo(LPCTSTR pszFormat, ...)
	{
		//校验级别
		if (IsNeedLog(LOG_LEVEL_INFO) == true)
		{
			//获取参数	
			va_list args;
			va_start(args, pszFormat);
			LogString(pszFormat, args);
			va_end(args);
		}
	}

	//调试日志
	virtual void LogDebug(LPCTSTR pszFormat, ...)
	{
		//校验级别
		if (IsNeedLog(LOG_LEVEL_DEBUG) == true)
		{
			//获取参数	
			va_list args;
			va_start(args, pszFormat);
			LogString(pszFormat, args);
			va_end(args);
		}
	}

	//警告日志
	virtual void LogWarn(LPCTSTR pszFormat, ...)
	{
		//校验级别
		if (IsNeedLog(LOG_LEVEL_WARN) == true)
		{
			//获取参数	
			va_list args;
			va_start(args, pszFormat);
			LogString(pszFormat, args);
			va_end(args);
		}
	}

	//错误日志
	virtual void LogError(LPCTSTR pszFormat, ...)
	{
		//校验级别
		if (IsNeedLog(LOG_LEVEL_ERROR) == true)
		{
			//获取参数	
			va_list args;
			va_start(args, pszFormat);
			LogString(pszFormat, args);
			va_end(args);
		}		
	}	

//辅助函数	
protected:
	//输出日志
	virtual void LogString(LPCTSTR pszFormat, va_list args)
	{
		//变量定义
		CString strContent;

		//头部信息
		GetHeader(strContent, m_nLogLevel);

		//日志信息		
		strContent.AppendFormatV(pszFormat, args);

		//添加换行符
		if (HasHeadMask(0xffff)!=0) strContent.Append(TEXT("\r\n"));

		//写入缓冲
		m_RollPtr->Write(strContent);

		//锁定缓冲
		//std::lock_guard<std::mutex> lock(m_BufMutex);

		////溢出判断
		//if ((CountStringBuffer(m_LogBuffer.GetString()) + CountStringBuffer(strContent.GetString())) > MAX_BUFFER_LENGTH)
		//{
		//	//刷新缓冲
		//	m_RollPtr->Write(m_LogBuffer);

		//	//清除日志
		//	m_LogBuffer.Empty();
		//}

		////添加日志
		//m_LogBuffer.Append(strContent);
	}

	//获取头部信息
	void GetHeader(CString & strBuffer, int nLevel)
	{
		//时间信息
		if (HasHeadMask(CLogger::LH_MASK_TIME))
		{
			//当前时间
			auto clock = time(NULL);

			//本地时间
			tm tt;
			::localtime_s(&tt, &clock);

			//格式字符串
			LPCTSTR szFormat = TEXT("%04d-%02d-%02d %02d:%02d:%02d%s");

			strBuffer.Format(szFormat, tt.tm_year + 1900, tt.tm_mon + 1, tt.tm_mday, tt.tm_hour, tt.tm_min, tt.tm_sec, m_szSeparator);
		}

		//进程信息
		if (HasHeadMask(CLogger::LH_MASK_PID))
		{
			strBuffer.AppendFormat(TEXT("%d%s"), ::GetCurrentThreadId(), m_szSeparator);
		}

		//日志级别
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

	//操作对象
protected:
	CRollPllicyPtr							m_RollPtr;

	//函数定义
public:	
	//构造函数
	CRollWrapperBase()
	{
		m_RollPtr = new RollPolicy;
	}
	//析构函数函数
	~CRollWrapperBase()
	{
		m_RollPtr->UnSetupThread();
	}

	//安装线程
	void SetupThread(CLoggerThread * pLoggerThread) 
	{
		m_RollPtr->SetupThread(pLoggerThread); 
	}

	//取消线程
	void UnSetupThread() 
	{
		m_RollPtr->UnSetupThread(); 
	}

	//获取写实例
	typename RollPolicy::T& GetWriteT() 
	{
		return m_RollPtr->GetWriteT();
	}

	//写入日志
	void Roll(const CString & strContent) 
	{
		m_RollPtr->Write(strContent); 
	}

	//获取Roll实例
	CRollPllicyPtr& GetRoll() 
	{
		return m_RollPtr; 
	}

	//异步刷新
	void Flush() 
	{
		m_RollPtr->Flush(); 
	}
};

////////////////////////////////////////////////////////////////////////////////

//根据日志大小滚动
template <typename WriteT>
class CRollBySize : public CLoggerRoll, public std::mutex
{
public:
	typedef WriteT T;

	//对象变量
protected:
	WriteT						m_WriteT;							//写对象
	CLoggerFile					m_LogFile;							//日志文件	

	//配置变量
protected:
	TCHAR						m_szPath[256];						//文件路径
	WORD						m_wMaxNum;							//最大数量
	DWORD						m_dwMaxSize;						//最大大小
	time_t						m_tUpdateTime;						//检查时间
	WORD						m_wCheckInterval;					//检查间隔		

public:
	
	//封装类(提供接口)	
	class CRollWrapperI : public CRollWrapperBase<CRollBySize<WriteT>>
	{
	public:
		//初始化
		void Init(LPCTSTR pszPath, int dwMaxSize = 5000000, int wMaxNum = 10)
		{
			this->m_RollPtr->Init(pszPath, dwMaxSize, wMaxNum);
		}

		//获取路径
		LPCTSTR GetPath() { return this->m_RollPtr->GetPath(); }

		//获取最大个数
		int GetMaxNum() { return this->m_RollPtr->GetMaxNum(); }

		//获取最大大小
		int GetMaxSize() { return this->m_RollPtr->GetMaxSize(); }

		//设置最大个数
		void SetMaxNum(int wMaxNum) { this->m_RollPtr->SetMaxNum(wMaxNum); }

		//设置最大大小
		void SetMaxSize(int dwMaxSize) { this->m_RollPtr->SetMaxSize(dwMaxSize); }

		//设置路径
		void SetPath(LPCTSTR pszPath) { this->m_RollPtr->SetPath(pszPath); }		
	};

	//函数定义
public:
	//构造函数
	CRollBySize()
	{
		//设置变量
		m_szPath[0] = 0;
		m_wMaxNum = 10;
		m_dwMaxSize = 5000000;
		m_wCheckInterval = 0;
		m_tUpdateTime = time(NULL);
	}

	//析构函数
	virtual ~CRollBySize()
	{
		if (m_LogFile.IsValid()) m_LogFile.Close();
	}

	//配置函数
public:
	//获取个数
	int GetMaxNum()
	{
		std::lock_guard<std::mutex> lock(*this);
		return m_wMaxNum;
	}

	//设置个数
	void SetMaxNum(int wMaxNum)
	{
		std::lock_guard<std::mutex> lock(*this);
		m_wMaxNum = wMaxNum;
	}

	//获取路径
	LPCTSTR GetPath()
	{
		std::lock_guard<std::mutex> lock(*this);
		return m_szPath;
	}

	//设置路径
	void SetPath(LPCTSTR pszPath)
	{
		std::lock_guard<std::mutex> lock(*this);
		StringCchCopy(m_szPath, CountArray(m_szPath), pszPath);
	}

	//获取大小
	int GetMaxSize()
	{
		std::lock_guard<std::mutex> lock(*this);
		return m_dwMaxSize;
	}

	//设置大小
	void SetMaxSize(int dwMaxSize)
	{
		std::lock_guard<std::mutex> lock(*this);
		m_dwMaxSize = dwMaxSize;
	}

	//信息函数
public:
	//获取写实例
	WriteT& GetWriteT() { return m_WriteT; }

	//功能函数
public:
	//初始化
	void Init(LPCTSTR pszPath, int dwMaxSize = 5000000, int wMaxNum = 10)
	{
		std::lock_guard<std::mutex> lock(*this);

		//设置变量		
		m_dwMaxSize = dwMaxSize;
		m_wMaxNum = wMaxNum;
		StringCchCopy(m_szPath, CountArray(m_szPath), pszPath);
	}

	//滚动日志
	void Roll(const CStringArray& strBuffer)
	{
		std::lock_guard<std::mutex> lock(*this);

		//校验路径
		if (m_szPath[0] == 0)
		{
			return;
		}

		//计算间隔
		time_t now = time(NULL);
		time_t interval = now - m_tUpdateTime;

		//校验时间
		if (interval > 5 || interval < 0)
		{
			m_tUpdateTime = now;
			m_LogFile.Close();
		}

		//检查日志文件是否打开		
		if (!m_LogFile.IsValid())
		{
			//构造路径			
			TCHAR szLogFileName[256] = { 0 };
			_sntprintf_s(szLogFileName, CountArray(szLogFileName), TEXT("%s.log"), m_szPath);

			//提取路径
			TCHAR szLogFilePath[256] = { 0 };
			CLoggerCommon::ExtractFilePath(m_szPath, szLogFilePath, CountArray(szLogFilePath));

			//创建目录
			if (!CLoggerCommon::IsDirectoryExist(szLogFilePath))
			{
				CLoggerCommon::MakeDirectory(szLogFilePath);
			}

			//打开文件
			m_LogFile.Create(szLogFileName, GENERIC_READ | GENERIC_WRITE | OPEN_ALWAYS);
		}

		//写入文件
		if (m_LogFile.IsValid())
		{
			m_WriteT(m_LogFile, strBuffer);			
		}

		//校验间隔
		if (interval <= 5)
		{
			return;
		}

		//校验文件大小
		if (m_LogFile.GetFileLength() < m_dwMaxSize)
		{
			return;
		}

		//文件大小超出限制,删除最后一个文件		
		TCHAR szLogFileName[256] = { 0 };
		_sntprintf_s(szLogFileName, CountArray(szLogFileName), TEXT("%s%d.log"), m_szPath, m_wMaxNum - 1);

		if (CLoggerCommon::IsFileExist(szLogFileName) && !DeleteFile(szLogFileName))
		{
			return;
		}

		//将log文件命名shift, xxx1.log=>xxx2.log,第一文件还是为xxx.log
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
				//构造路径
				TCHAR szNewLogFileName[256] = { 0 };
				_sntprintf_s(szNewLogFileName, CountArray(szNewLogFileName), TEXT("%s%d.log"), m_szPath, i + 1);

				//文件重命名
				rename(CT2CA(szLogFileName), CT2CA(szNewLogFileName));
			}
		}

		//关闭打开
		m_LogFile.Close();
		m_LogFile.Create(szLogFileName, GENERIC_READ | GENERIC_WRITE | OPEN_ALWAYS);
		if (!m_LogFile.IsValid())
		{
		}
	}	
};

typedef CLogger<CWriteT, CRollBySize> CRollLogger;

//////////////////////////////////////////////////////////////////////////////////////////////////////

//日志类型
class LOGGER_SERVICE_CLASS CLogType : public CHandleBase
{
	//变量定义
protected:
	time_t						m_tNextTime;							//时间节点
	size_t						m_Frequency;							//频率值
	TCHAR						m_szFormat[64];							//时间格式
	TCHAR						m_szDescribe[64];						//类型描述	
	TCHAR						m_szNextCutTime[128];					//时间节点	

	//函数定义
public:
	//构造函数
	CLogType();
	//析构函数
	virtual ~CLogType();

	//虚函数
public:
	//频率值	
	virtual size_t Frequence() = 0;
	//初始时间
	virtual LPCTSTR GetInitTime(TCHAR szBuffer[], WORD wBufferLen);
	//返回记录时间点
	virtual LPCTSTR GetNewTime(time_t time, TCHAR szBuffer[], WORD wBufferLen);

	//信息函数
public:
	//转换成字符串形式
	inline LPCTSTR ToString() { return m_szDescribe; }	

	//辅助函数
protected:
	//计算下一个时间点	
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

// 根据时间滚动日志
template <typename WriteT>
class CRollByTime : public CLoggerRoll, public std::mutex
{
public:
	typedef WriteT T;

	//对象变量
protected:
	WriteT					m_WriteT;							//写对象
	CLoggerFile				m_LogFile;							//日志文件
	CLogTypePtr				m_LogTypePtr;						//日志类型

	//标志变量
protected:
	bool					m_bHasSufix;						//后缀标识
	bool					m_bRemoteType;						//远程类型

	//配置变量
protected:	
	TCHAR					m_szPath[256];						//文件路径
	TCHAR					m_szFormat[64];						//输出格式
	TCHAR					m_szCurrentTime[128];				//当前时间
	TCHAR					m_szConcatStr[16];					//连接字符
	time_t					m_tCheckInterval;					//检查间隔			

public:
	
	// 封装类(接口类)	 
	class CRollWrapperI : public CRollWrapperBase<CRollByTime<WriteT>>
	{
	public:

		//初始化.		
		void Init(LPCTSTR pszPath, LPCTSTR pszFormat = TEXT("%Y%m%d"), bool bHasSufix = true, LPCTSTR pszConcatstr = TEXT("_"), const CLogTypePtr& logTypePtr = NULL, bool bRemoteType=false)
		{
			this->m_RollPtr->Init(pszPath, pszFormat, bHasSufix, pszConcatstr, logTypePtr, bRemoteType);
		}

		//远程标志
		void SetRemote(bool bEnabled) { this->m_RollPtr->SetRemote(bEnabled); }

		//获取日志路径.		
		LPCTSTR GetPath() { return this->m_RollPtr->GetPath(); }

		//设置文件路径		 
		void SetPath(LPCTSTR pszFath) { this->m_RollPtr->SetPath(pszFath); }

		//获取格式
		LPCTSTR GetFormat() { return this->m_RollPtr->GetFormat(); }

		//设置格式
		void SetFormat(LPCTSTR pszFormat) { this->m_RollPtr->SetFormat(pszFormat); }
	};

	//函数定义
public:
	//构造函数
	CRollByTime() : m_tCheckInterval(0), m_LogTypePtr(NULL)
	{
		//标志变量
		m_bHasSufix = true;
		m_bRemoteType = false;

		//配置变量
		m_szPath[0] = 0;
		m_szFormat[0] = 0;
		m_szCurrentTime[0] = 0;
		StringCchCopy(m_szConcatStr, CountArray(m_szConcatStr), TEXT("_"));
		m_tCheckInterval = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	}

	//析构函数
	virtual ~CRollByTime()
	{
		if (m_LogFile.IsValid()) m_LogFile.Close();
	}

	//配置函数
public:
	//获取路径
	LPCTSTR GetPath()
	{
		std::lock_guard<std::mutex> lock(*this);
		return m_szPath;
	}

	//设置路径
	void SetPath(LPCTSTR pszPath)
	{
		std::lock_guard<std::mutex> lock(*this);
		StringCchCopy(m_szPath, CountArray(m_szPath), pszPath);
	}

	//获取格式
	LPCTSTR GetFormat()
	{
		std::lock_guard<std::mutex> lock(*this);
		return m_szFormat;
	}

	//设置格式
	void SetFormat(LPCTSTR pszFormat)
	{
		std::lock_guard<std::mutex> lock(*this);
		StringCchCopy(m_szFormat, CountArray(m_szFormat), pszFormat);
	}

	//远程标志
	void SetRemote(bool bEnabled) 
	{
		std::lock_guard<std::mutex> lock(*this);
		m_bRemoteType = bEnabled;
	}

	//信息函数
public:
	//获取写实例
	WriteT& GetWriteT() { return m_WriteT; }

	//功能函数
public:	
	//日志滚动	
	void Roll(const CStringArray& strBuffer)
	{
		std::lock_guard<std::mutex> lock(*this);

		//校验路径
		if (m_szPath[0] == 0) return;

		//远程日志
		if (m_bRemoteType)
		{
			m_WriteT(m_LogFile, strBuffer);

			return;
		}

		TCHAR szNowTime[128] = { 0 };
		time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

		//如果设置了记录类型,则使用记录类型来计算时间		
		if (m_LogTypePtr)
		{
			//获取时间
			m_LogTypePtr->GetNewTime(now, szNowTime, CountArray(szNowTime));

			//时间跨越了			
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

			//检查时间是否跨时间了, 跨时间后关闭文件
			if (lstrcmp(m_szCurrentTime, szNowTime) != 0)
			{
				StringCchCopy(m_szCurrentTime, CountArray(m_szCurrentTime), szNowTime);
				m_LogFile.Close();
			}
		}

		//每隔10s, 重新打开一次文件, 避免文件被删除后句柄不释放		
		if (now - m_tCheckInterval > 10 || now - m_tCheckInterval < 0)
		{
			m_tCheckInterval = now;
			m_LogFile.Close();
		}

		if (!m_LogFile.IsValid())
		{
			//变量定义
			TCHAR szLogFileName[256] = TEXT("");
			TCHAR szLogFilePath[256] = TEXT("");
			_sntprintf_s(szLogFileName, CountArray(szLogFileName), TEXT("%s%s%s%s"), m_szPath, m_szConcatStr, szNowTime, m_bHasSufix ? TEXT(".log") : TEXT(""));

			//提取路径
			CLoggerCommon::ExtractFilePath(m_szPath, szLogFilePath, CountArray(szLogFilePath));

			//创建目录
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

		//写日志
		if (m_LogFile.IsValid())
		{
			m_WriteT(m_LogFile, strBuffer);		
			m_LogFile.Close();
		}
	}

	//初始化
	void Init(LPCTSTR pszPath, LPCTSTR pszFormat = TEXT("%Y%m%d"), bool bHasSufix = true, LPCTSTR pszConcatstr = TEXT("_"), const CLogTypePtr& logTypePtr = NULL, bool bRemoteType = false)
	{
		std::lock_guard<std::mutex> lock(*this);

		//设置变量
		m_bHasSufix = bHasSufix;
		m_LogTypePtr = logTypePtr;

		//标志变量
		m_bRemoteType = bRemoteType;

		//拷贝字符
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

