#include "StdAfx.h"
#include "TraceLog.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CTraceLog::CTraceLog()
{
	//设置变量
	m_bLoging = false;

	return;
}

//析构函数
CTraceLog::~CTraceLog()
{
	CloseLog();
}

//接口查询
VOID * CTraceLog::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITraceService,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITraceService,Guid,dwQueryVer);
	return NULL;
}

//追踪信息
bool CTraceLog::TraceString(LPCTSTR pszString, enTraceLevel TraceLevel)
{
	//内部TRACE
	if( m_pITraceService )
	{
		m_pITraceService->TraceString(pszString,TraceLevel);
	}

	//输出日志
	if( m_bLoging && m_LogFile.is_open() )
	{
		if( TraceLevel == TraceLevel_Exception )
		{
			//获取时间
			SYSTEMTIME SystemTime;
			CHAR szTimeBuffer[64];
			GetLocalTime(&SystemTime);
			_snprintf_s(szTimeBuffer,CountArray(szTimeBuffer),"【 %04d-%02d-%02d %02d:%02d:%02d 】",
				SystemTime.wYear,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);

			m_LogFile << szTimeBuffer << (LPCSTR)CT2A(pszString) << "\n";

			m_LogFile.flush();
		}
	}

	return true;
}

//初始化
void CTraceLog::InitLog(IUnknownEx *pITraceService)
{
	//设置变量
	m_pITraceService = QUERY_OBJECT_PTR_INTERFACE(pITraceService,ITraceService);

	//信息组件
	if (CTraceService::SetTraceService(QUERY_ME_INTERFACE(IUnknownEx))==false)
	{
		TraceString(TEXT("信息输出接口设置失败，系统运行信息将无法显示"),TraceLevel_Exception);
	}
}

//创建日志
bool CTraceLog::OpenLog(LPCTSTR lpszFile)
{
	//效验
	ASSERT(!m_bLoging);
	if( m_bLoging ) return false;

	ASSERT( !m_LogFile.is_open() );
	if( m_LogFile.is_open() )
		m_LogFile.close();

	m_bLoging = true;

	m_LogFile.open( (LPCSTR)CT2A(lpszFile),std::ios::out|std::ios::app );

	return m_bLoging;
}

//关闭日志
bool CTraceLog::CloseLog()
{
	if( m_LogFile.is_open() )
		m_LogFile.close();
	m_bLoging = false;
	return true;
}

//////////////////////////////////////////////////////////////////////////
