#include "stdafx.h"
#include "LoggerService.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//常量定义

#define LOGGER_SERVICE							100										//日志服务

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CTimeWriteT::CTimeWriteT()
{
	//标志变量
	m_bLocal = true;
	m_bRemote = false;
	m_bHasSufix = false;
	m_bHasPrefix = false;

	//设置变量
	m_szFormat[0] = 0;
	m_szLogType[0] = 0;
	m_szAppName[0] = 0;
	m_szLogPath[0] = 0;
	m_szFileName[0] = 0;
	m_szSeparator[0] = 0;
	m_szConcatStr[0] = 0;
	m_szServiceName[0] = 0;

	//通信接口
	m_pILoggerSocket = NULL;
}

//析构函数
CTimeWriteT::~CTimeWriteT()
{

}

//通信接口
void CTimeWriteT::SetLoggerSocket(ILoggerSocket* pILoggerSocket)
{
	m_pILoggerSocket = pILoggerSocket;
}

//日志信息
void CTimeWriteT::SetLoggerInfo(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFile, LPCTSTR pszLogPath, LPCTSTR pszFormat, LPCTSTR pszLogType)
{
	//服务信息
	StringCchCopy(m_szAppName, CountArray(m_szAppName), pszAppName);
	StringCchCopy(m_szServiceName,CountArray(m_szServiceName), pszServiceName);

	//文件信息
	StringCchCopy(m_szFileName, CountArray(m_szFileName), pszFile);	

	//配置变量
	StringCchCopy(m_szFormat, CountArray(m_szFormat), pszFormat);
	StringCchCopy(m_szLogType, CountArray(m_szLogType), pszLogType);

	//变量定义
	CString strLogPath = pszLogPath; 

	//补充路径
	strLogPath.AppendFormat(TEXT("\\%s\\%s\\"), m_szAppName, m_szServiceName);

	//前缀标识
	if (m_bHasPrefix) strLogPath.AppendFormat(TEXT("%s.%s"), m_szAppName, m_szServiceName);

	if (m_szFileName[0]!=0)
	{
		strLogPath.AppendFormat(TEXT("%s%s"), m_bHasPrefix? m_szConcatStr:TEXT(""), m_szFileName);
	}

	//日志路径
	StringCchCopy(m_szLogPath,CountArray(m_szLogPath), strLogPath.GetString());
}

//设置时间格式
void CTimeWriteT::SetFormat(LPCTSTR pszFormat) 
{
	StringCchCopy(m_szFormat, CountArray(m_szFormat), pszFormat);
}

//设置分隔符
void CTimeWriteT::SetSeparator(LPCTSTR pszSeparator) 
{
	StringCchCopy(m_szSeparator, CountArray(m_szSeparator), pszSeparator);
}

//连接符
void CTimeWriteT::SetConcatStr(LPCTSTR pszConcatStr) 
{ 
	StringCchCopy(m_szConcatStr, CountArray(m_szConcatStr), pszConcatStr);
}

//调用函数
void CTimeWriteT::operator()(CLoggerFile& LoggerFile, const CStringArray& strBuffer)
{
	//写本地
	if (m_bLocal && strBuffer.GetCount()>0)
	{
		try
		{
			for (auto i = 0; i < strBuffer.GetCount(); i++)
			{
				CT2CA szBuffer(strBuffer[i].GetString());

				//写入文件
				LoggerFile.SeekToEnd();
				LoggerFile.Write(szBuffer.m_psz, (DWORD)strlen(szBuffer.m_psz));
			}			
		}
		catch (...)
		{
			ASSERT(FALSE);
		}
	}

	//写远程
	if (m_bRemote && m_pILoggerSocket && strBuffer.GetCount() > 0)
	{
		try
		{
			//缓冲定义
			BYTE cbBuffer[SOCKET_PACKET] = { 0 };

			//构造结构
			WORD wDataSize = sizeof(CMD_LC_WriteTimeLogger);
			CMD_LC_WriteTimeLogger* pWriteLogger = (CMD_LC_WriteTimeLogger*)cbBuffer;

			//标志变量
			pWriteLogger->bHasSufix = m_bHasSufix;
			pWriteLogger->bHasPrefix = m_bHasPrefix;			

			//配置变量
			StringCchCopy(pWriteLogger->szFormat,CountArray(pWriteLogger->szFormat),m_szFormat);
			StringCchCopy(pWriteLogger->szAppName, CountArray(pWriteLogger->szAppName), m_szAppName);
			StringCchCopy(pWriteLogger->szFileName, CountArray(pWriteLogger->szFileName), m_szFileName);
			StringCchCopy(pWriteLogger->szLogType, CountArray(pWriteLogger->szLogType), m_szLogType);
			StringCchCopy(pWriteLogger->szConcatStr, CountArray(pWriteLogger->szConcatStr), m_szConcatStr);
			StringCchCopy(pWriteLogger->szSeparator, CountArray(pWriteLogger->szSeparator), m_szSeparator);			
			StringCchCopy(pWriteLogger->szServiceName, CountArray(pWriteLogger->szServiceName), m_szServiceName);

			//加入缓冲
			m_strBuffer.Append(strBuffer);

			for (auto i = 0; i < m_strBuffer.GetCount(); i++)
			{
				//计算大小
				WORD wStringSize = CountStringBuffer(m_strBuffer[i].GetString());

				//发送判断
				if (wDataSize + sizeof(WORD) + wStringSize > sizeof(cbBuffer))
				{
					//发送数据
					if (!m_pILoggerSocket->SendSocketData(MDM_LS_LOGGER_SERVICE, SUB_LC_WRITE_LOGGER, cbBuffer, wDataSize))
					{
						return;
					}

					//重置变量
					wDataSize = sizeof(CMD_LC_WriteTimeLogger);

					//移除数组
					m_strBuffer.RemoveAt(0,i); i = 0;
				}

				//设置大小
				*(WORD*)&cbBuffer[wDataSize] = wStringSize;
				wDataSize += sizeof(WORD);

				//拷贝字符串
				StringCchCopy((LPTSTR)&cbBuffer[wDataSize], (sizeof(cbBuffer)-wDataSize)/sizeof(TCHAR), m_strBuffer[i].GetString());
				wDataSize += wStringSize;
			}

			//发送数据
			if (wDataSize > sizeof(CMD_LC_WriteTimeLogger))
			{
				if (m_pILoggerSocket->SendSocketData(MDM_LS_LOGGER_SERVICE, SUB_LC_WRITE_LOGGER, cbBuffer, wDataSize))
				{
					m_strBuffer.RemoveAll();
				}
			}

			//清理缓存
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
//构造函数
CLoggerSocket::CLoggerSocket()
{
	//状态变量
	m_bService = false;
	m_bConnected = false;
	
	//设置变量		
	m_wServicePort = 0;
	m_szServiceHost[0] = 0;	
}

//析构函数
CLoggerSocket::~CLoggerSocket()
{
	//停止服务
	ConcludeService();
}

//接口查询
VOID* CLoggerSocket::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITCPSocketEvent, Guid, dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITCPSocketEvent, Guid, dwQueryVer);
	return NULL;
}

//连接事件
bool CLoggerSocket::OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode)
{
	//连接成功
	if (nErrorCode == 0)
	{
		m_bConnected = true;

		//发送消息
		SendSocketData(MDM_KN_COMMAND, SUB_KN_DETECT_SOCKET,0,0);		
	}

	return true;
}

//关闭事件
bool CLoggerSocket::OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason)
{
	//重置数据
	m_bConnected = false;

	//服务信息
	m_wServicePort = 0;
	m_szServiceHost[0] = 0;

	return true;
}

//读取事件
bool CLoggerSocket::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID* pData, WORD wDataSize, DWORD* pdwTokenID, WORD wTokenCount)
{
	return true;
}

//启动服务
bool CLoggerSocket::StartService()
{
	//设置标志
	ASSERT (m_bService==false);
	if (m_bService == true) return true;

	//设置标识
	m_bService = false;
	m_bConnected = false;

	try
	{
		//创建组件
		if (m_TCPSocketService.GetInterface() == NULL && m_TCPSocketService.CreateInstance() == false)
		{
			throw TEXT("组件创建失败");
		}

		//设置组件
		if (m_TCPSocketService->SetTCPSocketEvent(this) == false)
		{
			throw TEXT("组件设置失败");
		}

		//设置组件
		if (m_TCPSocketService->SetServiceID(LOGGER_SERVICE) == false)
		{
			throw TEXT("组件设置失败");
		}

		//启动线程
		if (m_TCPSocketService->StartService() == false)
		{
			throw TEXT("线程启动失败");
		}

		//设置标志
		m_bService = true;

		//连接服务
		if (m_szServiceHost[0] != 0 && m_wServicePort != 0)
		{
			m_TCPSocketService->Connect(m_szServiceHost, m_wServicePort);
		}

		return true;
	}
	catch (...)
	{
		//错误断言
		ASSERT(FALSE);

		//关闭服务
		m_TCPSocketService.CloseInstance();
	}

	return false;
}

//终止服务
bool CLoggerSocket::ConcludeService()
{
	//设置标志
	if (m_bService == false) return true;

	//停止服务
	if (m_TCPSocketService.GetInterface() != NULL) m_TCPSocketService->ConcludeService();

	//设置标志
	m_bService = false;	
	m_bConnected = false;

	//服务信息
	m_wServicePort = 0;
	m_szServiceHost[0] = 0;

	return true;
}

//连接服务
void CLoggerSocket::SetLoggerServer(LPCTSTR pszServiceHost, WORD wServicePort)
{
	//校验连接
	if (m_bConnected == true || m_szServiceHost[0] != 0)
	{
		return;
	}

	//服务信息
	m_wServicePort = wServicePort;
	StringCchCopy(m_szServiceHost,CountArray(m_szServiceHost), pszServiceHost);

	//连接服务
	if (m_bService == true && m_bConnected ==false)
	{
		m_TCPSocketService->Connect(m_szServiceHost, m_wServicePort);
	}	
}

//异步发送
bool CLoggerSocket::SendSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	if (m_bConnected == true)
	{
		return m_TCPSocketService->SendData(wMainCmdID, wSubCmdID, pData, wDataSize,0);
	}	

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

 //构造函数
CTimeLoggerService::CTimeLoggerService()
{
	//状态变量
	m_bService = false;

	//标志变量
	m_bLocal = true;
	m_bRemote = true;
	m_bHasSufix = true;
	m_bHasPrefix = true;

	//配置变量
	m_szAppName[0] = 0;
	m_szServiceName[0] = 0;
	m_szFileName[0] = 0;
	m_szFormat[0] = 0;
	m_szLogPath[0] = 0;
	m_szLogType[0] = 0;
	StringCchCopy(m_szConcatStr,CountArray(m_szConcatStr),TEXT("_"));
	StringCchCopy(m_szSeparator, CountArray(m_szSeparator), TEXT("|"));	

	//日志变量
	m_DefaultLogger = NULL;
	m_MapLoggers.InitHashTable(103);
}

//析构函数
CTimeLoggerService::~CTimeLoggerService()
{
	ConcludeService();
}

//本地开关
void CTimeLoggerService::EnableLocalEx(LPCTSTR pszFileName, bool bEnable)
{
	auto pLogger = Logger(pszFileName);
	if (pLogger != NULL)
	{
		pLogger->GetWriteT().EnableLocal(bEnable);
		pLogger->SetRemote(!bEnable);
	}
}

//本地开关
void CTimeLoggerService::EnableLocalEx(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, bool bEnable)
{
	auto pLogger = Logger(pszAppName, pszServiceName, pszFileName);
	if (pLogger != NULL)
	{
		pLogger->GetWriteT().EnableLocal(bEnable);
		pLogger->SetRemote(!bEnable);
	}
}

//远程开关
void CTimeLoggerService::EnableRemoteEx(LPCTSTR pszFileName, bool bEnable)
{
	auto pLogger = Logger(pszFileName);
	if (pLogger != NULL) pLogger->GetWriteT().EnableRemote(bEnable);
}

//远程开关
void CTimeLoggerService::EnableRemoteEx(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, bool bEnable)
{
	auto pLogger = Logger(pszAppName, pszServiceName, pszFileName);
	if (pLogger != NULL) pLogger->GetWriteT().EnableRemote(bEnable);
}

//设置时间格式
void CTimeLoggerService::SetFormat(LPCTSTR pszFormat)
{
	StringCchCopy(m_szFormat, CountArray(m_szFormat), pszFormat);
}

//设置分隔符
void CTimeLoggerService::SetSeparator(LPCTSTR pszSeparator)
{
	StringCchCopy(m_szSeparator, CountArray(m_szSeparator), pszSeparator);
}

//连接符
void CTimeLoggerService::SetConcatStr(LPCTSTR pszConcatStr)
{
	StringCchCopy(m_szConcatStr, CountArray(m_szConcatStr), pszConcatStr);
}

//启动服务
bool CTimeLoggerService::StartService()
{
	//校验状态
	ASSERT (m_bService==false);
	if (m_bService == true) return true;

	//启动线程
	m_LocalThread.StartThread();
	m_RemoteThread.StartThread();

	//启动服务
	m_LoggerSocket.StartService();

	//设置状态
	m_bService = true;

	return true;
}

//停止服务
bool CTimeLoggerService::ConcludeService()
{
	//校验状态
	if (m_bService == false) return false;

	//停止线程
	m_LocalThread.ConcludeThread(INFINITE);
	m_RemoteThread.ConcludeThread(INFINITE);	

	//清理日志
	CleanLoggers();

	//停止服务
	m_LoggerSocket.ConcludeService();

	//设置状态
	m_bService = false;

	return true;
}

//日志服务
void CTimeLoggerService::SetLoggerServer(LPCTSTR pszServiceHost, WORD wServicePort)
{
	return m_LoggerSocket.SetLoggerServer(pszServiceHost, wServicePort);
}

//日志信息
void CTimeLoggerService::SetLoggerInfo(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszLogPath)
{
	//拷贝数据
	StringCchCopy(m_szLogPath, CountArray(m_szLogPath), pszLogPath);
	StringCchCopy(m_szAppName,CountArray(m_szAppName), pszAppName);
	StringCchCopy(m_szServiceName, CountArray(m_szServiceName), pszServiceName);	

	//构造路径
	CString strFilePath = m_szLogPath;
	strFilePath.AppendFormat(TEXT("\\%s\\%s"), m_szAppName, m_szServiceName);

	//创建目录
	if (m_bLocal)
	{
		CLoggerCommon::MakeDirectory(strFilePath);
	}
	
}

//初始化格式
void CTimeLoggerService::InitFormat(LPCTSTR pszFileName, LPCTSTR pszFormat, const CLogTypePtr& logTypePtr)
{
	try
	{
		//锁定缓冲
		std::lock_guard<std::mutex> lock(m_Mutex);

		//创建对象
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
			//构造主键
			CString strKey;
			strKey.AppendFormat(TEXT("%s\\%s\\%s"),m_szAppName,m_szServiceName,pszFileName);

			//查找日志
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
	
//初始化格式
void CTimeLoggerService::InitFormat(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, LPCTSTR pszFormat, const CLogTypePtr& logTypePtr)
{
	try
	{
		//构造主键
		CString strKey;
		strKey.AppendFormat(TEXT("%s\\%s\\%s"), pszAppName, pszServiceName, pszFileName);

		//锁定缓冲
		std::lock_guard<std::mutex> lock(m_Mutex);

		//查找日志
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

//创建时间日志
CTimeLoggerService::CTimeLogger* CTimeLoggerService::Logger(LPCTSTR pszFileName)
{
	//锁定缓冲
	std::lock_guard<std::mutex> lock(m_Mutex);

	//创建对象
	if (lstrlen(pszFileName) == 0)
	{
		if (!m_DefaultLogger)
		{
			m_DefaultLogger = new CTimeLogger();
			InitTimeLogger(m_DefaultLogger, TEXT(""), pszFileName);
		}		

		return m_DefaultLogger;
	}
	
	//构造主键
	CString strKey;
	strKey.AppendFormat(TEXT("%s\\%s\\%s"), m_szAppName, m_szServiceName, pszFileName);

	//查找日志
	CTimeLogger* pTimeLogger = NULL;
	if (m_MapLoggers.Lookup(strKey, pTimeLogger) == FALSE)
	{
		pTimeLogger = new CTimeLogger();
		InitTimeLogger(pTimeLogger, pszFileName, TEXT("%Y%m%d"));
		m_MapLoggers[strKey] = pTimeLogger;		
	}

	return pTimeLogger;	
}
	
//创建时间日志
CTimeLoggerService::CTimeLogger* CTimeLoggerService::Logger(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName)
{
	//构造主键
	CString strKey;
	strKey.AppendFormat(TEXT("%s\\%s\\%s"), pszAppName, pszServiceName, pszFileName);

	//锁定缓冲
	std::lock_guard<std::mutex> lock(m_Mutex);

	//查找日志
	CTimeLogger* pTimeLogger = NULL;
	if (m_MapLoggers.Lookup(strKey, pTimeLogger) == FALSE)
	{
		pTimeLogger = new CTimeLogger();
		InitTimeLogger(pTimeLogger, pszFileName, TEXT("%Y%m%d"));
		m_MapLoggers[strKey] = pTimeLogger;
	}

	return pTimeLogger;
}

//存在判断
bool CTimeLoggerService::LoggerExists(LPCTSTR pszFileName)
{
	//锁定缓冲
	std::lock_guard<std::mutex> lock(m_Mutex);

	//构造主键
	CString strKey;
	strKey.AppendFormat(TEXT("%s\\%s\\%s"), m_szAppName, m_szServiceName, pszFileName);

	//查找日志
	CTimeLogger* pTimeLogger = NULL;
	m_MapLoggers.Lookup(strKey, pTimeLogger);

	return pTimeLogger!=NULL;
}

//存在判断
bool CTimeLoggerService::LoggerExists(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName)
{
	//构造主键
	CString strKey;
	strKey.AppendFormat(TEXT("%s\\%s\\%s"), pszAppName, pszServiceName, pszFileName);

	//锁定缓冲
	std::lock_guard<std::mutex> lock(m_Mutex);

	//查找日志
	CTimeLogger* pTimeLogger = NULL;
	m_MapLoggers.Lookup(strKey, pTimeLogger);
	
	return pTimeLogger!=NULL;
}

//初始化时间日志
void CTimeLoggerService::InitTimeLogger(CTimeLogger* pTimeLogger, LPCTSTR pszFileName, LPCTSTR pszFormat, const CLogTypePtr& logTypePtr)
{
	return InitTimeLogger(pTimeLogger, m_szAppName, m_szServiceName, pszFileName, pszFormat, logTypePtr);	
}

//初始化时间日志
void CTimeLoggerService::InitTimeLogger(CTimeLogger* pTimeLogger, LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, LPCTSTR pszFormat, const CLogTypePtr& logTypePtr)
{
	//文件路径
	CString strFilePath = m_szLogPath;

	//补充路径
	strFilePath.AppendFormat(TEXT("\\%s\\%s\\"), pszAppName, pszServiceName);

	//添加前缀
	if (m_bHasPrefix)
	{
		strFilePath.AppendFormat(TEXT("%s.%s"), pszAppName, pszServiceName);
	}

	//文件名称
	if (pszFileName != NULL)
	{
		strFilePath.AppendFormat(TEXT("%s%s"), m_bHasPrefix ? m_szConcatStr : TEXT(""), pszFileName);
	}

	//本地日志格式
	pTimeLogger->Init(strFilePath, pszFormat, m_bHasSufix, m_szConcatStr, logTypePtr, !m_bLocal);
	pTimeLogger->ModHeadMask(0xffff, false);
	pTimeLogger->ModHeadMask(CTimeLogger::LH_MASK_TIME, true);
	pTimeLogger->SetSeparator(m_szSeparator);

	//卸载线程
	pTimeLogger->UnSetupThread();

	//设置线程
	if (m_bLocal)
	{
		pTimeLogger->SetupThread(&m_LocalThread);
	}	
	else
	{
		pTimeLogger->SetupThread(&m_RemoteThread);
	}

	//日志类型
	LPCTSTR pszLogType = TEXT("");
	if (logTypePtr) pszLogType = logTypePtr->ToString();

	//日志信息
	pTimeLogger->GetWriteT().EnableLocal(m_bLocal);
	pTimeLogger->GetWriteT().EnableRemote(m_bRemote);
	pTimeLogger->GetWriteT().EnableSufix(m_bHasSufix);
	pTimeLogger->GetWriteT().EnablePrefix(m_bHasPrefix);
	pTimeLogger->GetWriteT().SetConcatStr(m_szConcatStr);
	pTimeLogger->GetWriteT().SetSeparator(m_szSeparator);
	pTimeLogger->GetWriteT().SetLoggerSocket(&m_LoggerSocket);
	pTimeLogger->GetWriteT().SetLoggerInfo(pszAppName, pszServiceName, pszFileName, m_szLogPath, pszFormat, pszLogType);
}

//清理日志
void CTimeLoggerService::CleanLoggers()
{
	//释放对象
	if (m_DefaultLogger != NULL) SafeDelete(m_DefaultLogger);

	//变量定义
	CString strKey;
	CTimeLogger* pTimeLogger = NULL;

	//遍历字典
	POSITION Postion = m_MapLoggers.GetStartPosition();
	while (Postion != NULL)
	{
		m_MapLoggers.GetNextAssoc(Postion, strKey, pTimeLogger);
		if (pTimeLogger == NULL) break;

		//释放对象
		SafeDelete(pTimeLogger);
	}

	//移除元素
	m_MapLoggers.RemoveAll();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CRollLoggerService::CRollLoggerService()
{
	//状态变量
	m_bService = false;

	//配置变量
	m_wMaxNum=10;
	m_dwMaxSize=1024*1024*10;              
	m_szAppName[0]=0;
	m_szLogPath[0] = 0;
	m_szServiceName[0]=0;	

	//初始化
	m_DefaultLogger = NULL;
	m_MapLoggers.InitHashTable(13);
}

//析构函数
CRollLoggerService::~CRollLoggerService()
{
	ConcludeService();
}

//启动服务
bool CRollLoggerService::StartService()
{
	//校验状态
	ASSERT(m_bService==false);
	if ((m_bService == true)) return false;

	//启动线程
	m_LocalThread.StartThread();

	//设置状态
	m_bService = true;

	return true;
}

//停止服务
bool CRollLoggerService::ConcludeService()
{
	//校验状态
	if ((m_bService == false)) return false;

	//启动线程
	m_LocalThread.ConcludeThread(INFINITE);

	//释放对象
	if (m_DefaultLogger != NULL) SafeDelete(m_DefaultLogger);

	//变量定义
	CString strKey;
	CRollLogger* pRollLogger = NULL;

	//遍历字典
	POSITION Postion = m_MapLoggers.GetStartPosition();
	while (Postion != NULL)
	{
		m_MapLoggers.GetNextAssoc(Postion, strKey, pRollLogger);
		if (pRollLogger == NULL) break;

		//释放对象
		SafeDelete(pRollLogger);
	}

	//移除元素
	m_MapLoggers.RemoveAll();

	//设置状态
	m_bService = false;

	return true;
}

//日志信息
void CRollLoggerService::SetLoggerInfo(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszLogPath, int dwMaxSize, int wMaxNum)
{
	//配置变量
	m_wMaxNum = wMaxNum;
	m_dwMaxSize = dwMaxSize;
	StringCchCopy(m_szAppName,CountArray(m_szAppName), pszAppName);
	StringCchCopy(m_szLogPath, CountArray(m_szLogPath), pszLogPath);
	StringCchCopy(m_szServiceName, CountArray(m_szServiceName), pszServiceName);

	//构造路径
	CString strFilePath = m_szLogPath;
	strFilePath.AppendFormat(TEXT("\\%s\\%s"), m_szAppName, m_szServiceName);

	//创建目录
	CLoggerCommon::MakeDirectory(strFilePath);
}

//获取滚动日志
CRollLoggerService::CRollLogger* CRollLoggerService::Logger(LPCTSTR pszFileName)
{
	try
	{
		//锁定缓冲
		std::lock_guard<std::mutex> lock(m_Mutex);

		//创建对象
		if (lstrlen(pszFileName) == 0)
		{
			if (!m_DefaultLogger)
			{
				m_DefaultLogger = new CRollLogger();
				InitRollLogger(m_DefaultLogger, pszFileName, m_dwMaxSize, m_wMaxNum);
			}

			return m_DefaultLogger;
		}

		//构造主键
		CString strKey;
		strKey.AppendFormat(TEXT("%s\\%s\\%s"), m_szAppName, m_szServiceName, pszFileName);
		
		//查找日志
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

//获取滚动日志
CRollLoggerService::CRollLogger* CRollLoggerService::Logger(LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName)
{
	try
	{
		//构造主键
		CString strKey;
		strKey.AppendFormat(TEXT("%s\\%s\\%s"), pszAppName, pszServiceName, pszFileName);	

		//锁定缓冲
		std::lock_guard<std::mutex> lock(m_Mutex);

		//查找日志
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

//初始化时间日志
void CRollLoggerService::InitRollLogger(CRollLogger* pRollLogger, LPCTSTR pszFileName, int dwMaxSize, int wMaxNum)
{
	return InitRollLogger(pRollLogger,m_szAppName,m_szServiceName, pszFileName, dwMaxSize, wMaxNum);
}

//初始化时间日志
void CRollLoggerService::InitRollLogger(CRollLogger* pRollLogger, LPCTSTR pszAppName, LPCTSTR pszServiceName, LPCTSTR pszFileName, int dwMaxSize, int wMaxNum)
{
	//文件路径
	CString strFilePath = m_szLogPath;

	//拼接路径
	if (lstrlen(pszFileName) == 0)
	{
		strFilePath.AppendFormat(TEXT("\\%s\\%s\\%s"), pszAppName, pszServiceName, pszServiceName);
	}
	else
	{
		strFilePath.AppendFormat(TEXT("\\%s\\%s\\%s"), pszAppName, pszServiceName, pszFileName);
	}		

	//本地日志格式
	pRollLogger->Init(strFilePath, m_dwMaxSize, m_wMaxNum);
	pRollLogger->ModHeadMask(0xffff, false);
	pRollLogger->ModHeadMask(CRollLogger::LH_MASK_TIME, true);
	pRollLogger->SetupThread(&m_LocalThread);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////