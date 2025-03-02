#ifndef GLOBAL_UNITES_HEAD_FILE
#define GLOBAL_UNITES_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//ȫ�ֵ�Ԫ
class CGlobalUnits
{
	//�������
public:
	bool							m_bAutoStart;						//�Զ�����
	bool							m_bFailureClose;					//����ر�
	bool							m_bValidateSuccess;					//��֤��־

	//�������
public:	
	CDatabaseListManager			m_DBLisMgr;							//���ݿ��б�
	CModuleDBParameter				m_ModuleDBParameter;				//ģ�����

	//��������
public:
	//���캯��
	CGlobalUnits();
	//��������
	virtual ~CGlobalUnits();

	//��������
public:
	//�������
	bool LoadServiceParameter();
};

//////////////////////////////////////////////////////////////////////////

//����˵��
extern CGlobalUnits					g_GlobalUnits;						//ȫ�ֵ�Ԫ

//////////////////////////////////////////////////////////////////////////

#endif