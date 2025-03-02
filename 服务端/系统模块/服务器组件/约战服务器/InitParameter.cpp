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
	ZeroMemory(&m_CenterAddress,sizeof(m_CenterAddress));	
	ZeroMemory(&m_GameBattleDBParameter, sizeof(m_GameBattleDBParameter));
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
	ServiceParameter.GetGameBattleDBInfo(m_GameBattleDBParameter);
	
	return;
}

//////////////////////////////////////////////////////////////////////////
