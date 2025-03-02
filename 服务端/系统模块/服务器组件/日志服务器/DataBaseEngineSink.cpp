#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//析构函数
CDataBaseEngineSink::~CDataBaseEngineSink()
{
}

//接口查询
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{

	try
	{
		//创建对象
		if ((m_PlatformDBModule.GetInterface() == NULL) && (m_PlatformDBModule.CreateInstance() == false))
		{
			ASSERT(FALSE);
			return false;
		}

		//变量定义
		tagDataBaseParameter * pPlatformDBParameter = &m_pInitParameter->m_PlatformDBParameter;

		//设置连接
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->dwDataBaseAddr, pPlatformDBParameter->wDataBasePort,
		CW2CT(pPlatformDBParameter->szDataBaseName),CW2CT(pPlatformDBParameter->szDataBaseUser),CW2CT(pPlatformDBParameter->szDataBasePass));
	
		//发起连接
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	//设置对象
	m_PlatformDBAide.SetDataBase(NULL);

	//关闭连接
	if (m_PlatformDBModule.GetInterface() != NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}

	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//时间事件
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//控制事件
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//请求事件
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	switch (wRequestID)
	{		
	case DBR_GR_LOAD_LOGGER_OPTION:		//加载选项
		{
			return OnRequestLoadLoggerOption(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
	}

	return false;
}

//加载配置
bool CDataBaseEngineSink::OnRequestLoadLoggerOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//构造参数
		//m_PlatformDBAide.ResetParameter();
		//m_PlatformDBAide.AddParameter(TEXT("@wKindID"),0);				
		//LONG lReturnValue=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_LoadLoggerBaseOption"),true);		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}


//输出信息
void CDataBaseEngineSink::TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException)
{
	CString strMessage;
	strMessage.Format(TEXT("%hs(%d):%hs(%s)"), pFile, nLine, pFunction, pIException ? pIException->GetExceptionDescribe() : TEXT(""));
	CTraceService::TraceString(strMessage, TraceLevel_Exception);
}
//////////////////////////////////////////////////////////////////////////

