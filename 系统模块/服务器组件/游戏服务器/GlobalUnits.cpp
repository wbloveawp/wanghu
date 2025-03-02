#include "StdAfx.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalUnits::CGlobalUnits()
{
	//���ñ���
	m_bAutoStart=false;
	m_bFailureClose=false;

	return;
}

//��������
CGlobalUnits::~CGlobalUnits()
{
}

//���ز���
bool CGlobalUnits::LoadServiceParameter()
{
	//��������
	CServiceParameter ServiceParameter;
	tagDataBaseParameter PlatformDBParameter;
	tagDataBaseParameter GameMatchDBParameter;

	//������Ϣ
	ServiceParameter.GetPlatformDBInfo(PlatformDBParameter);
	ServiceParameter.GetGameMatchDBInfo(GameMatchDBParameter);

	//���ò���
	m_ModuleDBParameter.SetMatchDBParameter(GameMatchDBParameter);
	m_ModuleDBParameter.SetPlatformDBParameter(PlatformDBParameter);	

	//�������ݿ��б�
	m_DBLisMgr.Load();

	return true;
}

//////////////////////////////////////////////////////////////////////////

//ȫ�ֶ���
CGlobalUnits						g_GlobalUnits;						//ȫ�ֵ�Ԫ

//////////////////////////////////////////////////////////////////////////
