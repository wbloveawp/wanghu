#include "StdAfx.h"
#include "ModuleDBParameter.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
CModuleDBParameter * CModuleDBParameter::m_pModuleDBParameter=NULL;		//����ָ��

//////////////////////////////////////////////////////////////////////////

//���캯��
CModuleDBParameter::CModuleDBParameter()
{
	//���ñ���
	ZeroMemory(&m_PlatformDBParameter,sizeof(m_PlatformDBParameter));

	//���ö���
	ASSERT(m_pModuleDBParameter==NULL);
	if (m_pModuleDBParameter==NULL) m_pModuleDBParameter=this;

	return;
}

//��������
CModuleDBParameter::~CModuleDBParameter()
{
	//���ö���
	ASSERT(m_pModuleDBParameter==this);
	if (m_pModuleDBParameter==this) m_pModuleDBParameter=NULL;

	return;
}

//////////////////////////////////////////////////////////////////////////
