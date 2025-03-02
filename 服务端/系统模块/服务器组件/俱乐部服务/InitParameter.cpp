#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////

//ʱ�䶨��
#define TIME_CONNECT				3									//����ʱ��
#define TIME_BATTLE					1									//����ʱ��

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
	m_wConnectCenter=TIME_CONNECT;		
	m_wConnectBattle=TIME_BATTLE;

	//���ñ���
	ZeroMemory(&m_CenterAddress,sizeof(m_CenterAddress));
	ZeroMemory(&m_GameBattleDBParameter,sizeof(m_GameBattleDBParameter));

	return;
}

//��������
VOID CInitParameter::LoadInitParameter()
{
	//���ò���
	InitParameter();

	//������Ϣ
	CServiceParameter ServiceParameter;	

	//���ĵ�ַ
	m_wCenterPort = ServiceParameter.GetCenterPort();
	ServiceParameter.GetCenterAddress(m_CenterAddress);

	//������Ϣ
	ServiceParameter.GetGameBattleDBInfo(m_GameBattleDBParameter);	

	return;
}
//////////////////////////////////////////////////////////////////////////
