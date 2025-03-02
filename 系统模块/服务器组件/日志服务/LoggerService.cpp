#include "stdafx.h"
#include "LoggerService.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//��������

#define LOGGER_SERVICE							100										//��־����

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CTimeWriteT::CTimeWriteT()
{
	//��־����
	m_bLocal = true;
	m_bRemote = false;
	m_bHasSufix = false;
	m_bHasPrefix = false;

	//���ñ���
	m_szFormat[0] = 0;
	m_szLogType[0] = 0;
	m_szAppName[0] = 0;
	m_szLogPath[0] = 0;
	m_szFileName[0] = 0;
	m_szSeparator[0] = 0;
	m_szConcatStr[0] = 0;
	m_szServiceName[0] = 0;

	//ͨ�Žӿ�
	m_pILoggerSocket = NULL;
}

//��������
CTimeWriteT::~CTimeWriteT()
{

}

//ͨ�Žӿ�
void CTimeWriteT::SetLoggerSocket(ILoggerSocket* pILoggerSocket)
{
	m_pILoggerSocket = pILoggerSocket;
}

//��־��Ϣ
void CTimeWriteT::SetLoggerInfo(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFile, LPCTSTR pszLogPath, LPCTSTR pszFormat, LPCTSTR pszLogType)
{
	//������Ϣ
	StringCchCopy(m_szAppName, CountArray(m_szAppName), pszAppName);
	StringCchCopy(m_szServiceName,CountArray(m_szServiceName), pszServiceName);

	//�ļ���Ϣ
	StringCchCopy(m_szFileName, CountArray(m_szFileName), pszFile);	

	//���ñ���
	StringCchCopy(m_szFormat, CountArray(m_szFormat), pszFormat);
	StringCchCopy(m_szLogType, CountArray(m_szLogType), pszLogType);

	//��������
	CString strLogPath = pszLogPath; 

	//����·��
	strLogPath.AppendFormat(TEXT("\\%s\\%s\\"), m_szAppName, m_szServiceName);

	//ǰ׺��ʶ
	if (m_bHasPrefix) strLogPath.AppendFormat(TEXT("%s.%s"), m_szAppName, m_szServiceName);

	if (m_szFileName[0]!=0)
	{
		strLogPath.AppendFormat(TEXT("%s%s"), m_bHasPrefix? m_szConcatStr:TEXT(""), m_szFileName);
	}

	//��־·��
	StringCchCopy(m_szLogPath,CountArray(m_szLogPath), strLogPath.GetString());
}

//����ʱ���ʽ
void CTimeWriteT::SetFormat(LPCTSTR pszFormat) 
{
	StringCchCopy(m_szFormat, CountArray(m_szFormat), pszFormat);
}

//���÷ָ���
void CTimeWriteT::SetSeparator(LPCTSTR pszSeparator) 
{
	StringCchCopy(m_szSeparator, CountArray(m_szSeparator), pszSeparator);
}

//���ӷ�
void CTimeWriteT::SetConcatStr(LPCTSTR pszConcatStr) 
{ 
	StringCchCopy(m_szConcatStr, CountArray(m_szConcatStr), pszConcatStr);
}

//���ú���
void CTimeWriteT::operator()(CLoggerFile& LoggerFile, const CStringArray& strBuffer)
{
	//д����
	if (m_bLocal && strBuffer.GetCount()>0)
	{
		try
		{
			for (auto i = 0; i < strBuffer.GetCount(); i++)
			{
				CT2CA szBuffer(strBuffer[i].GetString());

				//д���ļ�
				LoggerFile.SeekToEnd();
				LoggerFile.Write(szBuffer.m_psz, (DWORD)strlen(szBuffer.m_psz));
			}			
		}
		catch (...)
		{
			ASSERT(FALSE);
		}
	}

	//дԶ��
	if (m_bRemote && m_pILoggerSocket && strBuffer.GetCount() > 0)
	{
		try
		{
			//���嶨��
			BYTE cbBuffer[SOCKET_PACKET] = { 0 };

			//����ṹ
			WORD wDataSize = sizeof(CMD_LC_WriteTimeLogger);
			CMD_LC_WriteTimeLogger* pWriteLogger = (CMD_LC_WriteTimeLogger*)cbBuffer;

			//��־����
			pWriteLogger->bHasSufix = m_bHasSufix;
			pWriteLogger->bHasPrefix = m_bHasPrefix;			

			//���ñ���
			StringCchCopy(pWriteLogger->szFormat,CountArray(pWriteLogger->szFormat),m_szFormat);
			StringCchCopy(pWriteLogger->szAppName, CountArray(pWriteLogger->szAppName), m_szAppName);
			StringCchCopy(pWriteLogger->szFileName, CountArray(pWriteLogger->szFileName), m_szFileName);
			StringCchCopy(pWriteLogger->szLogType, CountArray(pWriteLogger->szLogType), m_szLogType);
			StringCchCopy(pWriteLogger->szConcatStr, CountArray(pWriteLogger->szConcatStr), m_szConcatStr);
			StringCchCopy(pWriteLogger->szSeparator, CountArray(pWriteLogger->szSeparator), m_szSeparator);			
			StringCchCopy(pWriteLogger->szServiceName, CountArray(pWriteLogger->szServiceName), m_szServiceName);

			//���뻺��
			m_strBuffer.Append(strBuffer);

			for (auto i = 0; i < m_strBuffer.GetCount(); i++)
			{
				//�����С
				WORD wStringSize = CountStringBuffer(m_strBuffer[i].GetString());

				//�����ж�
				if (wDataSize + sizeof(WORD) + wStringSize > sizeof(cbBuffer))
				{
					//��������
					if (!m_pILoggerSocket->SendSocketData(MDM_LS_LOGGER_SERVICE, SUB_LC_WRITE_LOGGER, cbBuffer, wDataSize))
					{
						return;
					}

					//���ñ���
					wDataSize = sizeof(CMD_LC_WriteTimeLogger);

					//�Ƴ�����
					m_strBuffer.RemoveAt(0,i); i = 0;
				}

				//���ô�С
				*(WORD*)&cbBuffer[wDataSize] = wStringSize;
				wDataSize += sizeof(WORD);

				//�����ַ���
				StringCchCopy((LPTSTR)&cbBuffer[wDataSize], (sizeof(cbBuffer)-wDataSize)/sizeof(TCHAR), m_strBuffer[i].GetString());
				wDataSize += wStringSize;
			}

			//��������
			if (wDataSize > sizeof(CMD_LC_WriteTimeLogger))
			{
				if (m_pILoggerSocket->SendSocketData(MDM_LS_LOGGER_SERVICE, SUB_LC_WRITE_LOGGER, cbBuffer, wDataSize))
				{
					m_strBuffer.RemoveAll();
				}
			}

			//������
			if (m_strBuffer.GetCount() > 500)
			{
				m_strBuffer.RemoveAt(0, m_strBuffer.GetCount() - 500);
			}
		}
		catch (...)
		{
			ASSERT (FALSE);
		}
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CLoggerSocket::CLoggerSocket()
{
	//״̬����
	m_bService = false;
	m_bConnected = false;
	
	//���ñ���		
	m_wServicePort = 0;
	m_szServiceHost[0] = 0;	
}

//��������
CLoggerSocket::~CLoggerSocket()
{
	//ֹͣ����
	ConcludeService();
}

//�ӿڲ�ѯ
VOID* CLoggerSocket::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPSocketEvent, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPSocketEvent, Guid, dwQueryVer);
	return NULL;
}

//�����¼�
bool CLoggerSocket::OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode)
{
	//���ӳɹ�
	if (nErrorCode == 0)
	{
		m_bConnected = true;

		//������Ϣ
		SendSocketData(MDM_KN_COMMAND, SUB_KN_DETECT_SOCKET,0,0);		
	}

	return true;
}

//�ر��¼�
bool CLoggerSocket::OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason)
{
	//��������
	m_bConnected = false;

	//������Ϣ
	m_wServicePort = 0;
	m_szServiceHost[0] = 0;

	return true;
}

//��ȡ�¼�
bool CLoggerSocket::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID* pData, WORD wDataSize, DWORD* pdwTokenID, WORD wTokenCount)
{
	return true;
}

//��������
bool CLoggerSocket::StartService()
{
	//���ñ�־
	ASSERT (m_bService==false);
	if (m_bService == true) return true;

	//���ñ�ʶ
	m_bService = false;
	m_bConnected = false;

	try
	{
		//�������
		if (m_TCPSocketService.GetInterface() == NULL && m_TCPSocketService.CreateInstance() == false)
		{
			throw TEXT("�������ʧ��");
		}

		//�������
		if (m_TCPSocketService->SetTCPSocketEvent(this) == false)
		{
			throw TEXT("�������ʧ��");
		}

		//�������
		if (m_TCPSocketService->SetServiceID(LOGGER_SERVICE) == false)
		{
			throw TEXT("�������ʧ��");
		}

		//�����߳�
		if (m_TCPSocketService->StartService() == false)
		{
			throw TEXT("�߳�����ʧ��");
		}

		//���ñ�־
		m_bService = true;

		//���ӷ���
		if (m_szServiceHost[0] != 0 && m_wServicePort != 0)
		{
			m_TCPSocketService->Connect(m_szServiceHost, m_wServicePort);
		}

		return true;
	}
	catch (...)
	{
		//�������
		ASSERT(FALSE);

		//�رշ���
		m_TCPSocketService.CloseInstance();
	}

	return false;
}

//��ֹ����
bool CLoggerSocket::ConcludeService()
{
	//���ñ�־
	if (m_bService == false) return true;

	//ֹͣ����
	if (m_TCPSocketService.GetInterface() != NULL) m_TCPSocketService->ConcludeService();

	//���ñ�־
	m_bService = false;	
	m_bConnected = false;

	//������Ϣ
	m_wServicePort = 0;
	m_szServiceHost[0] = 0;

	return true;
}

//���ӷ���
void CLoggerSocket::SetLoggerServer(LPCTSTR pszServiceHost, WORD wServicePort)
{
	//У������
	if (m_bConnected == true || m_szServiceHost[0] != 0)
	{
		return;
	}

	//������Ϣ
	m_wServicePort = wServicePort;
	StringCchCopy(m_szServiceHost,CountArray(m_szServiceHost), pszServiceHost);

	//���ӷ���
	if (m_bService == true && m_bConnected ==false)
	{
		m_TCPSocketService->Connect(m_szServiceHost, m_wServicePort);
	}	
}

//�첽����
bool CLoggerSocket::SendSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	if (m_bConnected == true)
	{
		return m_TCPSocketService->SendData(wMainCmdID, wSubCmdID, pData, wDataSize,0);
	}	

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

 //���캯��
CTimeLoggerService::CTimeLoggerService()
{
	//״̬����
	m_bService = false;

	//��־����
	m_bLocal = true;
	m_bRemote = true;
	m_bHasSufix = true;
	m_bHasPrefix = true;

	//���ñ���
	m_szAppName[0] = 0;
	m_szServiceName[0] = 0;
	m_szFileName[0] = 0;
	m_szFormat[0] = 0;
	m_szLogPath[0] = 0;
	m_szLogType[0] = 0;
	StringCchCopy(m_szConcatStr,CountArray(m_szConcatStr),TEXT("_"));
	StringCchCopy(m_szSeparator, CountArray(m_szSeparator), TEXT("|"));	

	//��־����
	m_DefaultLogger = NULL;
	m_MapLoggers.InitHashTable(103);
}

//��������
CTimeLoggerService::~CTimeLoggerService()
{
	ConcludeService();
}

//���ؿ���
void CTimeLoggerService::EnableLocalEx(LPCTSTR pszFileName, bool bEnable)
{
	auto pLogger = Logger(pszFileName);
	if (pLogger != NULL)
	{
		pLogger->GetWriteT().EnableLocal(bEnable);
		pLogger->SetRemote(!bEnable);
	}
}

//���ؿ���
void CTimeLoggerService::EnableLocalEx(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, bool bEnable)
{
	auto pLogger = Logger(pszAppName, pszServiceName, pszFileName);
	if (pLogger != NULL)
	{
		pLogger->GetWriteT().EnableLocal(bEnable);
		pLogger->SetRemote(!bEnable);
	}
}

//Զ�̿���
void CTimeLoggerService::EnableRemoteEx(LPCTSTR pszFileName, bool bEnable)
{
	auto pLogger = Logger(pszFileName);
	if (pLogger != NULL) pLogger->GetWriteT().EnableRemote(bEnable);
}

//Զ�̿���
void CTimeLoggerService::EnableRemoteEx(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, bool bEnable)
{
	auto pLogger = Logger(pszAppName, pszServiceName, pszFileName);
	if (pLogger != NULL) pLogger->GetWriteT().EnableRemote(bEnable);
}

//����ʱ���ʽ
void CTimeLoggerService::SetFormat(LPCTSTR pszFormat)
{
	StringCchCopy(m_szFormat, CountArray(m_szFormat), pszFormat);
}

//���÷ָ���
void CTimeLoggerService::SetSeparator(LPCTSTR pszSeparator)
{
	StringCchCopy(m_szSeparator, CountArray(m_szSeparator), pszSeparator);
}

//���ӷ�
void CTimeLoggerService::SetConcatStr(LPCTSTR pszConcatStr)
{
	StringCchCopy(m_szConcatStr, CountArray(m_szConcatStr), pszConcatStr);
}

//��������
bool CTimeLoggerService::StartService()
{
	//У��״̬
	ASSERT (m_bService==false);
	if (m_bService == true) return true;

	//�����߳�
	m_LocalThread.StartThread();
	m_RemoteThread.StartThread();

	//��������
	m_LoggerSocket.StartService();

	//����״̬
	m_bService = true;

	return true;
}

//ֹͣ����
bool CTimeLoggerService::ConcludeService()
{
	//У��״̬
	if (m_bService == false) return false;

	//ֹͣ�߳�
	m_LocalThread.ConcludeThread(INFINITE);
	m_RemoteThread.ConcludeThread(INFINITE);	

	//������־
	CleanLoggers();

	//ֹͣ����
	m_LoggerSocket.ConcludeService();

	//����״̬
	m_bService = false;

	return true;
}

//��־����
void CTimeLoggerService::SetLoggerServer(LPCTSTR pszServiceHost, WORD wServicePort)
{
	return m_LoggerSocket.SetLoggerServer(pszServiceHost, wServicePort);
}

//��־��Ϣ
void CTimeLoggerService::SetLoggerInfo(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszLogPath)
{
	//��������
	StringCchCopy(m_szLogPath, CountArray(m_szLogPath), pszLogPath);
	StringCchCopy(m_szAppName,CountArray(m_szAppName), pszAppName);
	StringCchCopy(m_szServiceName, CountArray(m_szServiceName), pszServiceName);	

	//����·��
	CString strFilePath = m_szLogPath;
	strFilePath.AppendFormat(TEXT("\\%s\\%s"), m_szAppName, m_szServiceName);

	//����Ŀ¼
	if (m_bLocal)
	{
		CLoggerCommon::MakeDirectory(strFilePath);
	}
	
}

//��ʼ����ʽ
void CTimeLoggerService::InitFormat(LPCTSTR pszFileName, LPCTSTR pszFormat, const CLogTypePtr& logTypePtr)
{
	try
	{
		//��������
		std::lock_guard<std::mutex> lock(m_Mutex);

		//��������
		if (lstrlen(pszFileName) == 0)
		{
			if (!m_DefaultLogger)
			{
				m_DefaultLogger = new CTimeLogger();
			}

			InitTimeLogger(m_DefaultLogger, TEXT(""), pszFormat, logTypePtr);
		}
		else
		{
			//��������
			CString strKey;
			strKey.AppendFormat(TEXT("%s\\%s\\%s"),m_szAppName,m_szServiceName,pszFileName);

			//������־
			CTimeLogger* pTimeLogger = NULL;
			if (m_MapLoggers.Lookup(strKey, pTimeLogger)==FALSE)			
			{
				pTimeLogger = new CTimeLogger();				
				InitTimeLogger(pTimeLogger, pszFileName, pszFormat, logTypePtr);
				m_MapLoggers[strKey] = pTimeLogger;				

				return;
			}

			InitTimeLogger(pTimeLogger, pszFileName, pszFormat, logTypePtr);
		}
	}
	catch (...)
	{
		ASSERT (FALSE);
	}	
}
	
//��ʼ����ʽ
void CTimeLoggerService::InitFormat(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, LPCTSTR pszFormat, const CLogTypePtr& logTypePtr)
{
	try
	{
		//��������
		CString strKey;
		strKey.AppendFormat(TEXT("%s\\%s\\%s"), pszAppName, pszServiceName, pszFileName);

		//��������
		std::lock_guard<std::mutex> lock(m_Mutex);

		//������־
		CTimeLogger* pTimeLogger = NULL;
		if (m_MapLoggers.Lookup(strKey, pTimeLogger) == FALSE)
		{
			pTimeLogger = new CTimeLogger();
			InitTimeLogger(pTimeLogger, pszFileName, pszFormat, logTypePtr);
			m_MapLoggers[strKey] = pTimeLogger;

			return;
		}

		InitTimeLogger(pTimeLogger, pszFileName, pszFormat, logTypePtr);
	}
	catch (...)
	{
		ASSERT (FALSE);
	}
}

//����ʱ����־
CTimeLoggerService::CTimeLogger* CTimeLoggerService::Logger(LPCTSTR pszFileName)
{
	//��������
	std::lock_guard<std::mutex> lock(m_Mutex);

	//��������
	if (lstrlen(pszFileName) == 0)
	{
		if (!m_DefaultLogger)
		{
			m_DefaultLogger = new CTimeLogger();
			InitTimeLogger(m_DefaultLogger, TEXT(""), pszFileName);
		}		

		return m_DefaultLogger;
	}
	
	//��������
	CString strKey;
	strKey.AppendFormat(TEXT("%s\\%s\\%s"), m_szAppName, m_szServiceName, pszFileName);

	//������־
	CTimeLogger* pTimeLogger = NULL;
	if (m_MapLoggers.Lookup(strKey, pTimeLogger) == FALSE)
	{
		pTimeLogger = new CTimeLogger();
		InitTimeLogger(pTimeLogger, pszFileName, TEXT("%Y%m%d"));
		m_MapLoggers[strKey] = pTimeLogger;		
	}

	return pTimeLogger;	
}
	
//����ʱ����־
CTimeLoggerService::CTimeLogger* CTimeLoggerService::Logger(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName)
{
	//��������
	CString strKey;
	strKey.AppendFormat(TEXT("%s\\%s\\%s"), pszAppName, pszServiceName, pszFileName);

	//��������
	std::lock_guard<std::mutex> lock(m_Mutex);

	//������־
	CTimeLogger* pTimeLogger = NULL;
	if (m_MapLoggers.Lookup(strKey, pTimeLogger) == FALSE)
	{
		pTimeLogger = new CTimeLogger();
		InitTimeLogger(pTimeLogger, pszFileName, TEXT("%Y%m%d"));
		m_MapLoggers[strKey] = pTimeLogger;
	}

	return pTimeLogger;
}

//�����ж�
bool CTimeLoggerService::LoggerExists(LPCTSTR pszFileName)
{
	//��������
	std::lock_guard<std::mutex> lock(m_Mutex);

	//��������
	CString strKey;
	strKey.AppendFormat(TEXT("%s\\%s\\%s"), m_szAppName, m_szServiceName, pszFileName);

	//������־
	CTimeLogger* pTimeLogger = NULL;
	m_MapLoggers.Lookup(strKey, pTimeLogger);

	return pTimeLogger!=NULL;
}

//�����ж�
bool CTimeLoggerService::LoggerExists(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName)
{
	//��������
	CString strKey;
	strKey.AppendFormat(TEXT("%s\\%s\\%s"), pszAppName, pszServiceName, pszFileName);

	//��������
	std::lock_guard<std::mutex> lock(m_Mutex);

	//������־
	CTimeLogger* pTimeLogger = NULL;
	m_MapLoggers.Lookup(strKey, pTimeLogger);
	
	return pTimeLogger!=NULL;
}

//��ʼ��ʱ����־
void CTimeLoggerService::InitTimeLogger(CTimeLogger* pTimeLogger, LPCTSTR pszFileName, LPCTSTR pszFormat, const CLogTypePtr& logTypePtr)
{
	return InitTimeLogger(pTimeLogger, m_szAppName, m_szServiceName, pszFileName, pszFormat, logTypePtr);	
}

//��ʼ��ʱ����־
void CTimeLoggerService::InitTimeLogger(CTimeLogger* pTimeLogger, LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, LPCTSTR pszFormat, const CLogTypePtr& logTypePtr)
{
	//�ļ�·��
	CString strFilePath = m_szLogPath;

	//����·��
	strFilePath.AppendFormat(TEXT("\\%s\\%s\\"), pszAppName, pszServiceName);

	//���ǰ׺
	if (m_bHasPrefix)
	{
		strFilePath.AppendFormat(TEXT("%s.%s"), pszAppName, pszServiceName);
	}

	//�ļ�����
	if (pszFileName != NULL)
	{
		strFilePath.AppendFormat(TEXT("%s%s"), m_bHasPrefix ? m_szConcatStr : TEXT(""), pszFileName);
	}

	//������־��ʽ
	pTimeLogger->Init(strFilePath, pszFormat, m_bHasSufix, m_szConcatStr, logTypePtr, !m_bLocal);
	pTimeLogger->ModHeadMask(0xffff, false);
	pTimeLogger->ModHeadMask(CTimeLogger::LH_MASK_TIME, true);
	pTimeLogger->SetSeparator(m_szSeparator);

	//ж���߳�
	pTimeLogger->UnSetupThread();

	//�����߳�
	if (m_bLocal)
	{
		pTimeLogger->SetupThread(&m_LocalThread);
	}	
	else
	{
		pTimeLogger->SetupThread(&m_RemoteThread);
	}

	//��־����
	LPCTSTR pszLogType = TEXT("");
	if (logTypePtr) pszLogType = logTypePtr->ToString();

	//��־��Ϣ
	pTimeLogger->GetWriteT().EnableLocal(m_bLocal);
	pTimeLogger->GetWriteT().EnableRemote(m_bRemote);
	pTimeLogger->GetWriteT().EnableSufix(m_bHasSufix);
	pTimeLogger->GetWriteT().EnablePrefix(m_bHasPrefix);
	pTimeLogger->GetWriteT().SetConcatStr(m_szConcatStr);
	pTimeLogger->GetWriteT().SetSeparator(m_szSeparator);
	pTimeLogger->GetWriteT().SetLoggerSocket(&m_LoggerSocket);
	pTimeLogger->GetWriteT().SetLoggerInfo(pszAppName, pszServiceName, pszFileName, m_szLogPath, pszFormat, pszLogType);
}

//������־
void CTimeLoggerService::CleanLoggers()
{
	//�ͷŶ���
	if (m_DefaultLogger != NULL) SafeDelete(m_DefaultLogger);

	//��������
	CString strKey;
	CTimeLogger* pTimeLogger = NULL;

	//�����ֵ�
	POSITION Postion = m_MapLoggers.GetStartPosition();
	while (Postion != NULL)
	{
		m_MapLoggers.GetNextAssoc(Postion, strKey, pTimeLogger);
		if (pTimeLogger == NULL) break;

		//�ͷŶ���
		SafeDelete(pTimeLogger);
	}

	//�Ƴ�Ԫ��
	m_MapLoggers.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//���캯��
CRollLoggerService::CRollLoggerService()
{
	//״̬����
	m_bService = false;

	//���ñ���
	m_wMaxNum=10;
	m_dwMaxSize=1024*1024*10;              
	m_szAppName[0]=0;
	m_szLogPath[0] = 0;
	m_szServiceName[0]=0;	

	//��ʼ��
	m_DefaultLogger = NULL;
	m_MapLoggers.InitHashTable(13);
}

//��������
CRollLoggerService::~CRollLoggerService()
{
	ConcludeService();
}

//��������
bool CRollLoggerService::StartService()
{
	//У��״̬
	ASSERT(m_bService==false);
	if ((m_bService == true)) return false;

	//�����߳�
	m_LocalThread.StartThread();

	//����״̬
	m_bService = true;

	return true;
}

//ֹͣ����
bool CRollLoggerService::ConcludeService()
{
	//У��״̬
	if ((m_bService == false)) return false;

	//�����߳�
	m_LocalThread.ConcludeThread(INFINITE);

	//�ͷŶ���
	if (m_DefaultLogger != NULL) SafeDelete(m_DefaultLogger);

	//��������
	CString strKey;
	CRollLogger* pRollLogger = NULL;

	//�����ֵ�
	POSITION Postion = m_MapLoggers.GetStartPosition();
	while (Postion != NULL)
	{
		m_MapLoggers.GetNextAssoc(Postion, strKey, pRollLogger);
		if (pRollLogger == NULL) break;

		//�ͷŶ���
		SafeDelete(pRollLogger);
	}

	//�Ƴ�Ԫ��
	m_MapLoggers.RemoveAll();

	//����״̬
	m_bService = false;

	return true;
}

//��־��Ϣ
void CRollLoggerService::SetLoggerInfo(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszLogPath, int dwMaxSize, int wMaxNum)
{
	//���ñ���
	m_wMaxNum = wMaxNum;
	m_dwMaxSize = dwMaxSize;
	StringCchCopy(m_szAppName,CountArray(m_szAppName), pszAppName);
	StringCchCopy(m_szLogPath, CountArray(m_szLogPath), pszLogPath);
	StringCchCopy(m_szServiceName, CountArray(m_szServiceName), pszServiceName);

	//����·��
	CString strFilePath = m_szLogPath;
	strFilePath.AppendFormat(TEXT("\\%s\\%s"), m_szAppName, m_szServiceName);

	//����Ŀ¼
	CLoggerCommon::MakeDirectory(strFilePath);
}

//��ȡ������־
CRollLoggerService::CRollLogger* CRollLoggerService::Logger(LPCTSTR pszFileName)
{
	try
	{
		//��������
		std::lock_guard<std::mutex> lock(m_Mutex);

		//��������
		if (lstrlen(pszFileName) == 0)
		{
			if (!m_DefaultLogger)
			{
				m_DefaultLogger = new CRollLogger();
				InitRollLogger(m_DefaultLogger, pszFileName, m_dwMaxSize, m_wMaxNum);
			}

			return m_DefaultLogger;
		}

		//��������
		CString strKey;
		strKey.AppendFormat(TEXT("%s\\%s\\%s"), m_szAppName, m_szServiceName, pszFileName);
		
		//������־
		CRollLogger* pRollLogger = NULL;
		if (m_MapLoggers.Lookup(strKey, pRollLogger) == FALSE)
		{
			pRollLogger = new CRollLogger();
			InitRollLogger(pRollLogger, pszFileName, m_dwMaxSize, m_wMaxNum);
			m_MapLoggers[strKey] = pRollLogger;
		}

		return pRollLogger;
	}
	catch(...)
	{
		ASSERT(FALSE);
	}

	return NULL;
}

//��ȡ������־
CRollLoggerService::CRollLogger* CRollLoggerService::Logger(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName)
{
	try
	{
		//��������
		CString strKey;
		strKey.AppendFormat(TEXT("%s\\%s\\%s"), pszAppName, pszServiceName, pszFileName);	

		//��������
		std::lock_guard<std::mutex> lock(m_Mutex);

		//������־
		CRollLogger* pRollLogger = NULL;
		if (m_MapLoggers.Lookup(strKey, pRollLogger) == FALSE)
		{
			pRollLogger = new CRollLogger();
			InitRollLogger(pRollLogger, pszFileName, m_dwMaxSize, m_wMaxNum);
			m_MapLoggers[strKey] = pRollLogger;
		}

		return pRollLogger;
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return NULL;
}

//��ʼ��ʱ����־
void CRollLoggerService::InitRollLogger(CRollLogger* pRollLogger, LPCTSTR pszFileName, int dwMaxSize, int wMaxNum)
{
	return InitRollLogger(pRollLogger,m_szAppName,m_szServiceName, pszFileName, dwMaxSize, wMaxNum);
}

//��ʼ��ʱ����־
void CRollLoggerService::InitRollLogger(CRollLogger* pRollLogger, LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, int dwMaxSize, int wMaxNum)
{
	//�ļ�·��
	CString strFilePath = m_szLogPath;

	//ƴ��·��
	if (lstrlen(pszFileName) == 0)
	{
		strFilePath.AppendFormat(TEXT("\\%s\\%s\\%s"), pszAppName, pszServiceName, pszServiceName);
	}
	else
	{
		strFilePath.AppendFormat(TEXT("\\%s\\%s\\%s"), pszAppName, pszServiceName, pszFileName);
	}		

	//������־��ʽ
	pRollLogger->Init(strFilePath, m_dwMaxSize, m_wMaxNum);
	pRollLogger->ModHeadMask(0xffff, false);
	pRollLogger->ModHeadMask(CRollLogger::LH_MASK_TIME, true);
	pRollLogger->SetupThread(&m_LocalThread);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////