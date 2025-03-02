#include "stdafx.h"
#include "Logger.h"


///////////////////////////////////////////////////////////////////////////////////////
//静态成员

LPCTSTR CLogByDay::FORMAT = TEXT("%Y%m%d");
LPCTSTR CLogByHour::FORMAT = TEXT("%Y%m%d%H");
LPCTSTR CLogByMinute::FORMAT = TEXT("%Y%m%d%H%M");

///////////////////////////////////////////////////////////////////////////////////////

//构造函数
CLoggerCommon::CLoggerCommon()
{
}

//析构函数
CLoggerCommon::~CLoggerCommon()
{
}

//文件存在
bool CLoggerCommon::IsFileExist(LPCTSTR pszFileName)
{
	//变量定义
	CFileFind FileFind;
	return FileFind.FindFile(pszFileName);
}

//目录存在
bool CLoggerCommon::IsDirectoryExist(LPCTSTR pszDirectory)
{
	//变量定义
	WIN32_FIND_DATA wfd;
	bool bResult = false;

	//查找文件
	HANDLE hFind = FindFirstFile(pszDirectory, &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		bResult = TRUE;
	}

	//关闭句柄
	FindClose(hFind);

	return bResult;
}

//创建目录
bool CLoggerCommon::MakeDirectory(LPCTSTR pszFileDirectory)
{
	//变量定义
	TCHAR szFileDerectory[MAX_PATH]={0};
	LPCTSTR pszPathMove = pszFileDirectory;

	while (*pszPathMove != TEXT('\0'))
	{
		if ((*(pszPathMove+1) == '/' || *(pszPathMove+1) == '\\') || *(pszPathMove+1)==TEXT('\0'))
		{
			int nLength = (pszPathMove - pszFileDirectory) + 1;
			if (nLength < CountArray(szFileDerectory))
			{
				//拷贝路径
				StringCchCopy(szFileDerectory, nLength+1, pszFileDirectory);

				//创建路径
				if (IsDirectoryExist(szFileDerectory) == false)
				{
					::CreateDirectory(szFileDerectory, NULL);
				}				
			}		
		}

		pszPathMove++;
		while (*pszPathMove == '/' || *pszPathMove == '\\') pszPathMove++;
	}

	return 0;

}

//提取目录
int CLoggerCommon::ExtractFilePath(LPCTSTR pszFullFileName, TCHAR szBuffer[], WORD wBufferLen)
{
	//变量定义
	LPCTSTR pszPathReverse = pszFullFileName + lstrlen(pszFullFileName);

	while (*(--pszPathReverse) != TEXT('\0'))
	{
		if (pszPathReverse == pszFullFileName) break;
		if (*pszPathReverse == '/' || *pszPathReverse == '\\')
		{
			int nLength = (pszPathReverse - pszFullFileName)+1;
			if (nLength < wBufferLen)
			{
				StringCchCopy(szBuffer, nLength, pszFullFileName);
				return nLength;
			}
		}
	}

	return 0;
}

//时间转换
LPCTSTR CLoggerCommon::Time2Str(time_t time, LPCTSTR pszFormat, TCHAR szBuffer[], WORD wBufferLen)
{
	//变量定义
	char sTimeStr[128];

	//本地时间
	struct tm Local;
	localtime_s(&Local, &time);

	//构造格式
	CT2CA szFormat(pszFormat);

	//格式化时间
	strftime(sTimeStr, sizeof(sTimeStr), szFormat.m_psz, &Local);

	//编码转换
	CA2CT szTransfer(sTimeStr);

	//校验缓冲区
	StringCchCopy(szBuffer, wBufferLen, szTransfer);

	return szBuffer;
}

///////////////////////////////////////////////////////////////////////////////////////


//构造函数
CLoggerRoll::CLoggerRoll()
{
	//设置变量
	m_pLoggerThread = NULL;
}

//析构函数
CLoggerRoll::~CLoggerRoll()
{
	Flush();
}

//安装线程
void CLoggerRoll::SetupThread(CLoggerThread* pLoggerThread)
{
	//参数校验
	ASSERT (pLoggerThread != NULL);
	if (pLoggerThread == NULL) return;

	//卸载线程
	UnSetupThread();
	
	//加锁
	std::lock_guard<std::mutex> lock(m_Mutex);

	//设置变量
	m_pLoggerThread = pLoggerThread;

	//构造对象
	CLoggerRollPtr SelfRollPtr(this);

	m_pLoggerThread->RegisterLogger(SelfRollPtr);
}

//卸载线程
void CLoggerRoll::UnSetupThread()
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	
	if (m_pLoggerThread != NULL)
	{
		m_pLoggerThread->Flush();

		//构造对象
		CLoggerRollPtr SelfRollPtr(this);

		m_pLoggerThread->UnRegisterLogger(SelfRollPtr);

		m_pLoggerThread = NULL;
	}

	Flush();
}

//写入日志
void CLoggerRoll::Write(const CString& strContent)
{
	if (m_pLoggerThread)
	{
		std::lock_guard<std::mutex> lock(m_BufMutex);
		m_strBuffer.Add(strContent);		
	}
	else
	{
		CStringArray strBuffer;
		strBuffer.Add(strContent);

		Roll(strBuffer);
	}
}

//刷新缓存
void CLoggerRoll::Flush()
{
	//构造缓存
	CStringArray strBuffer;	

	{
		std::lock_guard<std::mutex> lock(m_BufMutex);
		strBuffer.Append(m_strBuffer);		
		m_strBuffer.RemoveAll();
	}

	if (strBuffer.IsEmpty()==false)
	{
		Roll(strBuffer);
	}
}

///////////////////////////////////////////////////////////////////////////////////////

//构造函数
CLoggerThread::CLoggerThread()
{

}

//析构函数
CLoggerThread::~CLoggerThread()
{
}

//终止线程
bool CLoggerThread::ConcludeThread(DWORD dwMillSeconds)
{
	//唤醒线程
	if (IsRuning() == true)
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		m_CondVar.notify_all();
	}

	return __super::ConcludeThread(dwMillSeconds);
}

//运行事件
bool CLoggerThread::OnEventThreadRun()
{
	//休眠100毫秒
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		m_CondVar.wait_for(lock, std::chrono::milliseconds(100));
	}

	Flush();

	return true;
}
	
//开始事件
bool CLoggerThread::OnEventThreadStrat()
{
	return true;
}

//终止事件
bool CLoggerThread::OnEventThreadConclude()
{
	return true;
}

//刷新日志
void CLoggerThread::Flush()
{
	CLoggerArray LoggerArray;
	
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		LoggerArray.Append(m_LoggerArray);
	}

	//刷新日志
	for (auto i=0; i< LoggerArray.GetCount(); i++)
	{
		try
		{
			LoggerArray[i]->Flush();
		}
		catch (...)
		{
			ASSERT(FALSE);
		}		
	}

	LoggerArray.RemoveAll();
}

//注册对象
void CLoggerThread::RegisterLogger(CLoggerRollPtr& LoggerRollPtr)
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	//添加日志
	m_LoggerArray.Add(LoggerRollPtr);
}

//卸载对象
void CLoggerThread::UnRegisterLogger(CLoggerRollPtr& LoggerRollPtr)
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	//添加日志
	for (auto i = 0; i < m_LoggerArray.GetCount(); i++)
	{
		if (m_LoggerArray[i].get() == LoggerRollPtr.get())
		{
			m_LoggerArray.RemoveAt(i);
			break;
		}
	}	
}

///////////////////////////////////////////////////////////////////////////////////////

//构造函数
CLogType::CLogType() : m_tNextTime(0), m_Frequency(1)
{
	//初始化
	StringCchCopy(m_szFormat, CountArray(m_szFormat), TEXT("%Y%m%d"));
	StringCchCopy(m_szDescribe, CountArray(m_szDescribe), TEXT("day"));

	//当前时间戳
	m_tNextTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	//转换时间
	CLoggerCommon::Time2Str(m_tNextTime, m_szFormat, m_szNextCutTime, CountArray(m_szNextCutTime));
}

//析构函数
CLogType::~CLogType() 
{
}

//初始时间
LPCTSTR CLogType::GetInitTime(TCHAR szBuffer[], WORD wBufferLen)
{
	time_t time = m_tNextTime - Frequence();

	return CLoggerCommon::Time2Str(time, m_szFormat, szBuffer, wBufferLen);
}

//返回空表示没有到下一个记录点
LPCTSTR CLogType::GetNewTime(time_t time, TCHAR szBuffer[], WORD wBufferLen)
{
	if (time >= m_tNextTime && lstrcmp(CLoggerCommon::Time2Str(time, m_szFormat, szBuffer, wBufferLen), m_szNextCutTime) != 0)
	{
		//时间规整			
		time_t tNextTime = m_tNextTime + ((time - m_tNextTime) / Frequence()) * Frequence();
		m_tNextTime = tNextTime + Frequence();
		CLoggerCommon::Time2Str(m_tNextTime, m_szFormat, m_szNextCutTime, CountArray(m_szNextCutTime));

		return CLoggerCommon::Time2Str(tNextTime, m_szFormat, szBuffer, wBufferLen);
	}
	else
	{
		return NULL;
	}
}

//计算下一个时间点	
void CLogType::Init(LPCTSTR pszFormat, size_t frequency)
{
	m_Frequency = (frequency == 0 ? 1 : frequency);
	StringCchCopy(m_szFormat, CountArray(m_szFormat), pszFormat);

	//获取时间戳
	time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	m_tNextTime = now / Frequence() + 1;
	m_tNextTime *= Frequence();

	//转换时间
	CLoggerCommon::Time2Str(m_tNextTime, m_szFormat, m_szNextCutTime, CountArray(m_szNextCutTime));
}

///////////////////////////////////////////////////////////////////////////////////////
