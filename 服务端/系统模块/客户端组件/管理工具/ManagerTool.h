#pragma once

#include "Resource.h"
#include "Other\IDispimp.h"
#include "Other\Custsite.h"

//////////////////////////////////////////////////////////////////////////

//Ӧ�ó�����
class CGamePlazaApp : public CWinApp
{
	//��������
protected:
	SOCKET									m_hSocketMutex;				//���Ӿ��
	ATOM									m_AtomPlazaClass;			//�㳡ԭ��
	
	//��������
public:
	CImpIDispatch *							m_pIDispatch;				//�Զ��ӿ�

	//��������
public:
	//���캯��
	CGamePlazaApp();

	//���غ���
public:
	//���뺯��
	virtual BOOL InitInstance();
	//�˳�����
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()	
};

//////////////////////////////////////////////////////////////////////////

//Ӧ�ó������˵��
extern CGamePlazaApp theApp;

//////////////////////////////////////////////////////////////////////////
