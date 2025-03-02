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
	ZeroMemory(&m_szServerName,sizeof(m_szServerName));
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
	ServiceParameter.GetServerName(m_szServerName);

	//���ĵ�ַ
	m_wCenterPort = ServiceParameter.GetCenterPort();
	ServiceParameter.GetCenterAddress(m_CenterAddress);

	//���ݿ���Ϣ
	ServiceParameter.GetPlatformDBInfo(m_PlatformDBParameter);
	
	return;
}

//////////////////////////////////////////////////////////////////////////
