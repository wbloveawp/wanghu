#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return;
}

//��������
CDataBaseEngineSink::~CDataBaseEngineSink()
{
}

//�ӿڲ�ѯ
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{

	try
	{
		//��������
		if ((m_PlatformDBModule.GetInterface() == NULL) && (m_PlatformDBModule.CreateInstance() == false))
		{
			ASSERT(FALSE);
			return false;
		}

		//��������
		tagDataBaseParameter * pPlatformDBParameter = &m_pInitParameter->m_PlatformDBParameter;

		//��������
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->dwDataBaseAddr, pPlatformDBParameter->wDataBasePort,
		CW2CT(pPlatformDBParameter->szDataBaseName),CW2CT(pPlatformDBParameter->szDataBaseUser),CW2CT(pPlatformDBParameter->szDataBasePass));
	
		//��������
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//������Ϣ
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//ֹͣ�¼�
bool CDataBaseEngineSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	//���ö���
	m_PlatformDBAide.SetDataBase(NULL);

	//�ر�����
	if (m_PlatformDBModule.GetInterface() != NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}

	//�������
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//ʱ���¼�
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//�����¼�
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	switch (wRequestID)
	{		
	case DBR_GR_LOAD_LOGGER_OPTION:		//����ѡ��
		{
			return OnRequestLoadLoggerOption(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
	}

	return false;
}

//��������
bool CDataBaseEngineSink::OnRequestLoadLoggerOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//�������
		//m_PlatformDBAide.ResetParameter();
		//m_PlatformDBAide.AddParameter(TEXT("@wKindID"),0);				
		//LONG lReturnValue=m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GR_LoadLoggerBaseOption"),true);		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//�������
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}


//�����Ϣ
void CDataBaseEngineSink::TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException)
{
	CString strMessage;
	strMessage.Format(TEXT("%hs(%d):%hs(%s)"), pFile, nLine, pFunction, pIException ? pIException->GetExceptionDescribe() : TEXT(""));
	CTraceService::TraceString(strMessage, TraceLevel_Exception);
}
//////////////////////////////////////////////////////////////////////////

