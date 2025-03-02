#include "StdAfx.h"
#include "InitParameter.h"

//////////////////////////////////////////////////////////////////////////

//ʱ�䶨��
#define TIME_CONNECT				30									//����ʱ��

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
	m_wMaxConnect=MAX_CONTENT;

	//ʱ�䶨��
	m_wConnectTime=TIME_CONNECT;

	//������Ϣ
	m_cbNetworkKind=NETWORK_KIND_DX;
	ZeroMemory(m_szValidate,sizeof(m_szValidate));
	ZeroMemory(m_szAccreditID,sizeof(m_szAccreditID));

	//������Ϣ
	ZeroMemory(&m_ServiceAddress,sizeof(m_ServiceAddress));
	ZeroMemory(&m_CorrespondAddress,sizeof(m_CorrespondAddress));
	ZeroMemory(&m_PlatformDBParameter,sizeof(m_PlatformDBParameter));

	return;
}

//��������
VOID CInitParameter::LoadInitParameter()
{
	//���ò���
	InitParameter();

	//��ȡ����
	CServiceParameter ServiceParameter;
	ServiceParameter.GetAccreditID(m_szAccreditID);
	ServiceParameter.GetValidateLink(m_szValidate);
	ServiceParameter.GetServiceAddress(m_ServiceAddress);

	//��������
	m_cbNetworkKind=ServiceParameter.GetNetworkKind();

	return;
}

//////////////////////////////////////////////////////////////////////////
