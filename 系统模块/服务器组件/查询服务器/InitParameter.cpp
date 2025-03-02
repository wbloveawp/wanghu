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
	m_wConnectTime=TIME_CONNECT;
	m_wCenterPort = PORT_CENTER;

	//������Ϣ	
	ZeroMemory(&m_CenterAddress,sizeof(m_CenterAddress));
	ZeroMemory(&m_AccountsDBParameter, sizeof(m_AccountsDBParameter));
	ZeroMemory(&m_PlatformDBParameter,sizeof(m_PlatformDBParameter));

	return;
}

//��������
VOID CInitParameter::LoadInitParameter()
{
	//���ò���
	InitParameter();

	//������Ϣ
	CServiceParameter ServiceParameter;
	m_wCenterPort = ServiceParameter.GetCenterPort();

	//������Ϣ
	ServiceParameter.GetCenterAddress(m_CenterAddress);
	ServiceParameter.GetAccountsDBInfo(m_AccountsDBParameter);
	ServiceParameter.GetPlatformDBInfo(m_PlatformDBParameter);

	//���ز���
	LoadRedisParameter();

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

	//�û�������
	m_UserRConnOption.wDB = GetPrivateProfileInt(TEXT("User_Read"), TEXT("DB"), RKEY_DDI_USER, szFileName);
	m_UserRConnOption.wPort = GetPrivateProfileInt(TEXT("User_Read"), TEXT("Port"), 0, szFileName);
	GetPrivateProfileString(TEXT("User_Read"), TEXT("Host"), 0, m_UserRConnOption.szHost, CountArray(m_UserRConnOption.szHost), szFileName);
	GetPrivateProfileString(TEXT("User_Read"), TEXT("User"), 0, m_UserRConnOption.szUser, CountArray(m_UserRConnOption.szUser), szFileName);
	GetPrivateProfileString(TEXT("User_Read"), TEXT("Password"), 0, m_UserRConnOption.szPassword, CountArray(m_UserRConnOption.szPassword), szFileName);

	//�û�������
	m_UserWConnOption.wDB = GetPrivateProfileInt(TEXT("User_Write"), TEXT("DB"), RKEY_DDI_USER, szFileName);
	m_UserWConnOption.wPort = GetPrivateProfileInt(TEXT("User_Write"), TEXT("Port"), 0, szFileName);
	GetPrivateProfileString(TEXT("User_Write"), TEXT("Host"), 0, m_UserWConnOption.szHost, CountArray(m_UserWConnOption.szHost), szFileName);
	GetPrivateProfileString(TEXT("User_Write"), TEXT("User"), 0, m_UserWConnOption.szUser, CountArray(m_UserWConnOption.szUser), szFileName);
	GetPrivateProfileString(TEXT("User_Write"), TEXT("Password"), 0, m_UserWConnOption.szPassword, CountArray(m_UserWConnOption.szPassword), szFileName);

	//���ƿ�����
	m_FriendConnOption.wDB = GetPrivateProfileInt(TEXT("UFriend_Read"), TEXT("DB"), RKEY_DDI_UFRIEND, szFileName);
	m_FriendConnOption.wPort = GetPrivateProfileInt(TEXT("UFriend_Read"), TEXT("Port"), 0, szFileName);
	GetPrivateProfileString(TEXT("UFriend_Read"), TEXT("Host"), 0, m_FriendConnOption.szHost, CountArray(m_FriendConnOption.szHost), szFileName);
	GetPrivateProfileString(TEXT("UFriend_Read"), TEXT("User"), 0, m_FriendConnOption.szUser, CountArray(m_FriendConnOption.szUser), szFileName);
	GetPrivateProfileString(TEXT("UFriend_Read"), TEXT("Password"), 0, m_FriendConnOption.szPassword, CountArray(m_FriendConnOption.szPassword), szFileName);

	return;
}

//////////////////////////////////////////////////////////////////////////
