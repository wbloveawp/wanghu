#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////

//ʱ�䶨��
#define TIME_UPDATE					10									//����ʱ��
#define TIME_UPLOAD					15									//�ϴ�ʱ��
#define TIME_CONNECT				30									//����ʱ��
#define TIME_COLLECT				60									//ͳ��ʱ��
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
	m_wUpdateTime=TIME_UPDATE;
	m_wUploadTime=TIME_UPLOAD;
	m_wMaxConnect=MAX_CONTENT;
	m_wServicePort=PORT_CENTER;
	m_wConnectTime=TIME_CONNECT;
	m_wCollectTime=TIME_COLLECT;	

	//������Ϣ
	ZeroMemory(m_szUploadDomain,sizeof(m_szUploadDomain));

	//������Ϣ
	ZeroMemory(&m_PlatformDBParameter,sizeof(m_PlatformDBParameter));
	ZeroMemory(&m_AccountsDBParameter,sizeof(m_AccountsDBParameter));

	//������Ϣ
	ZeroMemory(&m_RedisConnOption,sizeof(m_RedisConnOption));

	return;
}

//��������
VOID CInitParameter::LoadInitParameter()
{
	//���ò���
	InitParameter();

	//������Ϣ
	CServiceParameter ServiceParameter;
	m_wServicePort =ServiceParameter.GetCenterPort();

	//������Ϣ
	ServiceParameter.GetPlatformDBInfo(m_PlatformDBParameter);
	ServiceParameter.GetAccountsDBInfo(m_AccountsDBParameter);

	//��������
	LoadRedisParameter();

	//����Ŀ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//�ļ�Ŀ¼
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\ServerInfo.ini"), szDirectory);

	//ϵͳ����
	GetPrivateProfileString(TEXT("Information"), TEXT("UploadDomain"), TEXT(""), m_szUploadDomain, CountArray(m_szUploadDomain), szFileName);

	return;
}


//��¼����
VOID CInitParameter::LoadRedisParameter()
{
	//����Ŀ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//�ļ�Ŀ¼
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\RedisParameter.ini"), szDirectory);

	//��������
	m_RedisConnOption.wDB = GetPrivateProfileInt(TEXT("Master_Write"), TEXT("DB"), RKEY_DDI_MASTER, szFileName);
	m_RedisConnOption.wPort = GetPrivateProfileInt(TEXT("Master_Write"), TEXT("Port"), 0, szFileName);
	GetPrivateProfileString(TEXT("Master_Write"), TEXT("Host"), 0, m_RedisConnOption.szHost, CountArray(m_RedisConnOption.szHost), szFileName);
	GetPrivateProfileString(TEXT("Master_Write"), TEXT("User"), 0, m_RedisConnOption.szUser, CountArray(m_RedisConnOption.szUser), szFileName);
	GetPrivateProfileString(TEXT("Master_Write"), TEXT("Password"), 0, m_RedisConnOption.szPassword, CountArray(m_RedisConnOption.szPassword), szFileName);

	return;
}

//////////////////////////////////////////////////////////////////////////
