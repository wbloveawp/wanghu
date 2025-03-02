#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////

//ʱ�䶨��
#define TIME_CONNECT				3									//����ʱ��
#define TIME_REPORT_ONLINE			600									//�ϱ�ʱ��

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
	//ʱ�䶨��
	m_wCenterPort=PORT_CENTER;
	m_wConnectTime=TIME_CONNECT;
	m_wReportTime=TIME_REPORT_ONLINE;

	//վ����Ϣ
	m_wStationID=0;
	ZeroMemory(m_szStationName,sizeof(m_szStationName));

	//��������
	ZeroMemory(&m_CenterAddress,sizeof(m_CenterAddress));

	return;
}

//��������
VOID CInitParameter::LoadInitParameter()
{
	//���ò���
	InitParameter();

	//������Ϣ
	CServiceParameter ServiceParameter;		
	m_wCenterPort=ServiceParameter.GetCenterPort();
	ServiceParameter.GetCenterAddress(m_CenterAddress);

	return;
}

//////////////////////////////////////////////////////////////////////////
