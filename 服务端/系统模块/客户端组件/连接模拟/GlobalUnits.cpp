#include "StdAfx.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalUnits::CGlobalUnits()
{
	//���ñ���
	m_cbTestMode = 0;
	m_cbConnectMode = 0;
	m_bConnectStatus=true;
	

	//������Ϣ
	m_wKindID=0;
	m_wServerType=0;
	m_wServerPort=0;
	m_wServerLevel=0;
	m_szServerAddr[0]=0;
	m_dwProcessVersion=0;

	return;
}

//��������
CGlobalUnits::~CGlobalUnits()
{
}

//////////////////////////////////////////////////////////////////////////

//ȫ�ֵ�Ԫ
CGlobalUnits g_GlobalUnits;

//////////////////////////////////////////////////////////////////////////
