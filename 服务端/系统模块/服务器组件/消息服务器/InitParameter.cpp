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

	//������Ϣ	
	ZeroMemory(m_szPaltformDomain, sizeof(m_szPaltformDomain));
	ZeroMemory(&m_CenterAddress,sizeof(m_CenterAddress));
	ZeroMemory(&m_AccountsDBParameter,sizeof(m_AccountsDBParameter));
	ZeroMemory(&m_PlatformDBParameter, sizeof(m_PlatformDBParameter));
	ZeroMemory(m_szLoggerPath, sizeof(m_szLoggerPath));

	return;
}

//��������
VOID CInitParameter::LoadInitParameter()
{
	//���ò���
	InitParameter();

	//������Ϣ
	CServiceParameter ServiceParameter;

	//���Ķ˿�
	m_wCenterPort=ServiceParameter.GetCenterPort();

	//��֤��Ϣ
	ServiceParameter.GetCenterAddress(m_CenterAddress);
	ServiceParameter.GetAccountsDBInfo(m_AccountsDBParameter);
	ServiceParameter.GetPlatformDBInfo(m_PlatformDBParameter);

	//����Ŀ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//�ļ�Ŀ¼
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\ServerInfo.ini"), szDirectory);

	//ϵͳ����
	GetPrivateProfileString(TEXT("Information"), TEXT("PlatformDomain"), TEXT(""), m_szPaltformDomain, CountArray(m_szPaltformDomain), szFileName);

	//��ȡ·��
	GetPrivateProfileString(TEXT("LoggerConfig"), TEXT("LocalLogPath"), TEXT("D:\\LocalLogs"), m_szLoggerPath, CountArray(m_szLoggerPath), szFileName);

	return;
}

//////////////////////////////////////////////////////////////////////////
