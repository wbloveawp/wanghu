#include "stdafx.h"
#include "Logger.h"


///////////////////////////////////////////////////////////////////////////////////////
//��̬��Ա

LPCTSTR CLogByDay::FORMAT = TEXT("%Y%m%d");
LPCTSTR CLogByHour::FORMAT = TEXT("%Y%m%d%H");
LPCTSTR CLogByMinute::FORMAT = TEXT("%Y%m%d%H%M");

///////////////////////////////////////////////////////////////////////////////////////

//���캯��
CLoggerCommon::CLoggerCommon()
{
}

//��������
CLoggerCommon::~CLoggerCommon()
{
}

//�ļ�����
bool CLoggerCommon::IsFileExist(LPCTSTR pszFileName)
{
	//��������
	CFileFind FileFind;
	return FileFind.FindFile(pszFileName);
}

//Ŀ¼����
bool CLoggerCommon::IsDirectoryExist(LPCTSTR pszDirectory)
{
	//��������
	WIN32_FIND_DATA wfd;
	bool bResult = false;

	//�����ļ�
	HANDLE hFind = FindFirstFile(pszDirectory, &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		bResult = TRUE;
	}

	//�رվ��
	FindClose(hFind);

	return bResult;
}

//����Ŀ¼
bool CLoggerCommon::MakeDirectory(LPCTSTR pszFileDirectory)
{
	//��������
	TCHAR szFileDerectory[MAX_PATH]={0};
	LPCTSTR pszPathMove = pszFileDirectory;

	while (*pszPathMove != TEXT('\0'))
	{
		if ((*(pszPathMove+1) == '/' || *(pszPathMove+1) == '\\') || *(pszPathMove+1)==TEXT('\0'))
		{
			int nLength = (pszPathMove - pszFileDirectory) + 1;
			if (nLength < CountArray(szFileDerectory))
			{
				//����·��
				StringCchCopy(szFileDerectory, nLength+1, pszFileDirectory);

				//����·��
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

//��ȡĿ¼
int CLoggerCommon::ExtractFilePath(LPCTSTR pszFullFileName, TCHAR szBuffer[], WORD wBufferLen)
{
	//��������
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

//ʱ��ת��
LPCTSTR CLoggerCommon::Time2Str(time_t time, LPCTSTR pszFormat, TCHAR szBuffer[], WORD wBufferLen)
{
	//��������
	char sTimeStr[128];

	//����ʱ��
	struct tm Local;
	localtime_s(&Local, &time);

	//�����ʽ
	CT2CA szFormat(pszFormat);

	//��ʽ��ʱ��
	strftime(sTimeStr, sizeof(sTimeStr), szFormat.m_psz, &Local);

	//����ת��
	CA2CT szTransfer(sTimeStr);

	//У�黺����
	StringCchCopy(szBuffer, wBufferLen, szTransfer);

	return szBuffer;
}

///////////////////////////////////////////////////////////////////////////////////////


//���캯��
CLoggerRoll::CLoggerRoll()
{
	//���ñ���
	m_pLoggerThread = NULL;
}

//��������
CLoggerRoll::~CLoggerRoll()
{
	Flush();
}

//��װ�߳�
void CLoggerRoll::SetupThread(CLoggerThread* pLoggerThread)
{
	//����У��
	ASSERT (pLoggerThread != NULL);
	if (pLoggerThread == NULL) return;

	//ж���߳�
	UnSetupThread();
	
	//����
	std::lock_guard<std::mutex> lock(m_Mutex);

	//���ñ���
	m_pLoggerThread = pLoggerThread;

	//�������
	CLoggerRollPtr SelfRollPtr(this);

	m_pLoggerThread->RegisterLogger(SelfRollPtr);
}

//ж���߳�
void CLoggerRoll::UnSetupThread()
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	
	if (m_pLoggerThread != NULL)
	{
		m_pLoggerThread->Flush();

		//�������
		CLoggerRollPtr SelfRollPtr(this);

		m_pLoggerThread->UnRegisterLogger(SelfRollPtr);

		m_pLoggerThread = NULL;
	}

	Flush();
}

//д����־
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

//ˢ�»���
void CLoggerRoll::Flush()
{
	//���컺��
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

//���캯��
CLoggerThread::CLoggerThread()
{

}

//��������
CLoggerThread::~CLoggerThread()
{
}

//��ֹ�߳�
bool CLoggerThread::ConcludeThread(DWORD dwMillSeconds)
{
	//�����߳�
	if (IsRuning() == true)
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		m_CondVar.notify_all();
	}

	return __super::ConcludeThread(dwMillSeconds);
}

//�����¼�
bool CLoggerThread::OnEventThreadRun()
{
	//����100����
	{
		std::unique_lock<std::mutex> lock(m_Mutex);
		m_CondVar.wait_for(lock, std::chrono::milliseconds(100));
	}

	Flush();

	return true;
}
	
//��ʼ�¼�
bool CLoggerThread::OnEventThreadStrat()
{
	return true;
}

//��ֹ�¼�
bool CLoggerThread::OnEventThreadConclude()
{
	return true;
}

//ˢ����־
void CLoggerThread::Flush()
{
	CLoggerArray LoggerArray;
	
	{
		std::lock_guard<std::mutex> lock(m_Mutex);
		LoggerArray.Append(m_LoggerArray);
	}

	//ˢ����־
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

//ע�����
void CLoggerThread::RegisterLogger(CLoggerRollPtr& LoggerRollPtr)
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	//�����־
	m_LoggerArray.Add(LoggerRollPtr);
}

//ж�ض���
void CLoggerThread::UnRegisterLogger(CLoggerRollPtr& LoggerRollPtr)
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	//�����־
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

//���캯��
CLogType::CLogType() : m_tNextTime(0), m_Frequency(1)
{
	//��ʼ��
	StringCchCopy(m_szFormat, CountArray(m_szFormat), TEXT("%Y%m%d"));
	StringCchCopy(m_szDescribe, CountArray(m_szDescribe), TEXT("day"));

	//��ǰʱ���
	m_tNextTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	//ת��ʱ��
	CLoggerCommon::Time2Str(m_tNextTime, m_szFormat, m_szNextCutTime, CountArray(m_szNextCutTime));
}

//��������
CLogType::~CLogType() 
{
}

//��ʼʱ��
LPCTSTR CLogType::GetInitTime(TCHAR szBuffer[], WORD wBufferLen)
{
	time_t time = m_tNextTime - Frequence();

	return CLoggerCommon::Time2Str(time, m_szFormat, szBuffer, wBufferLen);
}

//���ؿձ�ʾû�е���һ����¼��
LPCTSTR CLogType::GetNewTime(time_t time, TCHAR szBuffer[], WORD wBufferLen)
{
	if (time >= m_tNextTime && lstrcmp(CLoggerCommon::Time2Str(time, m_szFormat, szBuffer, wBufferLen), m_szNextCutTime) != 0)
	{
		//ʱ�����			
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

//������һ��ʱ���	
void CLogType::Init(LPCTSTR pszFormat, size_t frequency)
{
	m_Frequency = (frequency == 0 ? 1 : frequency);
	StringCchCopy(m_szFormat, CountArray(m_szFormat), pszFormat);

	//��ȡʱ���
	time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

	m_tNextTime = now / Frequence() + 1;
	m_tNextTime *= Frequence();

	//ת��ʱ��
	CLoggerCommon::Time2Str(m_tNextTime, m_szFormat, m_szNextCutTime, CountArray(m_szNextCutTime));
}

///////////////////////////////////////////////////////////////////////////////////////
