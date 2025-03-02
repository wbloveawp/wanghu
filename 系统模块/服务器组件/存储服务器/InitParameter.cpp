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
	ZeroMemory(m_szVideoPath,sizeof(m_szVideoPath));
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
	ServiceParameter.GetCenterAddress(m_CenterAddress);

	//���Ķ˿�
	m_wCenterPort = ServiceParameter.GetCenterPort();

	//����Ŀ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szDirectory, CountArray(szDirectory));

	//�ļ�Ŀ¼
	TCHAR szFileName[MAX_PATH] = TEXT("");
	_sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s\\StoreServer.ini"), szDirectory);

	//·������
	GetPrivateProfileString(TEXT("PathConfig"), TEXT("VideoPath"), TEXT("D:"), m_szVideoPath, CountArray(m_szVideoPath), szFileName);

	return;
}

//////////////////////////////////////////////////////////////////////////
