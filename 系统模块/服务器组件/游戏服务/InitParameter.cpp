#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////

//ʱ�䶨��
#define TIME_STORE				1									//�洢ʱ��		
#define TIME_SERVICE			1									//����ʱ��		
#define TIME_MONITOR			1									//���ʱ��		
#define TIME_CONNECT			1									//����ʱ��

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
	m_wConnectStore=TIME_STORE;
	m_wConnectService=TIME_SERVICE;
	m_wConnectMonitor=TIME_MONITOR;

	//������Ϣ
	ZeroMemory(m_szLoggerPath,sizeof(m_szLoggerPath));

	//������Ϣ
	ZeroMemory(&m_CenterAddress, sizeof(m_CenterAddress));
	ZeroMemory(&m_ServiceAddress,sizeof(m_ServiceAddress));	
	ZeroMemory(&m_PlatformDBParameter,sizeof(m_PlatformDBParameter));
	ZeroMemory(&m_AccountsDBParameter,sizeof(m_AccountsDBParameter));
	

	return;
}

//��������
VOID CInitParameter::LoadInitParameter()
{
	//���ò���
	InitParameter();

	//��������
	CServiceParameter ServiceParameter;

	//���Ķ˿�
	m_wCenterPort=ServiceParameter.GetCenterPort();

	//������Ϣ
	ServiceParameter.GetCenterAddress(m_CenterAddress);
	ServiceParameter.GetPlatformDBInfo(m_PlatformDBParameter);
	ServiceParameter.GetAccountsDBInfo(m_AccountsDBParameter);	

	//������Ϣ
	ServiceParameter.GetServiceAddress(m_ServiceAddress);	

	//��־����
	LoadLoggerParameter();

	return;
}

//��־����
VOID CInitParameter::LoadLoggerParameter()
{
	//����Ŀ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//�ļ�Ŀ¼
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\ServerInfo.ini"), szDirectory);

	//��ȡ·��
	GetPrivateProfileString(TEXT("LoggerConfig"), TEXT("LocalLogPath"), TEXT("D:\\LocalLogs"), m_szLoggerPath, CountArray(m_szLoggerPath), szFileName);
}

//////////////////////////////////////////////////////////////////////////
