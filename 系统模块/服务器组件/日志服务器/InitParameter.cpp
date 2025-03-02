#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////

//ʱ�䶨��
#define TIME_CONNECT				3									//����ʱ��
//////////////////////////////////////////////////////////////////////////

//���캯��
CInitParameter::CInitParameter() 
{ 
	InitParameter();
}

//��������
CInitParameter::~CInitParameter()
{
}

//��ʼ��
VOID CInitParameter::InitParameter()
{
	//ϵͳ����
	m_wCenterPort=PORT_CENTER;
	m_wConnectTime=TIME_CONNECT;

	//ϵͳ����
	m_szLoggerPath[0] = 0;

	//������Ϣ	
	ZeroMemory(&m_CenterAddress,sizeof(m_CenterAddress));	
	ZeroMemory(&m_PlatformDBParameter,sizeof(m_PlatformDBParameter));	

	return;
}

//��������
VOID CInitParameter::LoadInitParameter()
{
	//������Ϣ
	CServiceParameter ServiceParameter;

	//���ĵ�ַ
	m_wCenterPort=ServiceParameter.GetCenterPort();
	ServiceParameter.GetCenterAddress(m_CenterAddress);

	//���ݿ���Ϣ
	ServiceParameter.GetPlatformDBInfo(m_PlatformDBParameter);

	//����Ŀ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//�ļ�Ŀ¼
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\ServerInfo.ini"), szDirectory);

	//ϵͳ����
	GetPrivateProfileString(TEXT("LoggerConfig"), TEXT("RemoteLogPath"), TEXT("D:\\"), m_szLoggerPath, CountArray(m_szLoggerPath), szFileName);
	
	return;
}

//////////////////////////////////////////////////////////////////////////
