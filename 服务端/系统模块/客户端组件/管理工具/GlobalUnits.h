#ifndef GLOBAL_UNITES_HEAD_FILE
#define GLOBAL_UNITES_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "AfxTempl.h"
#include "DlgStatus.h"
#include "DispatchManager.h"
#include "ManagerToolModule.h"

//////////////////////////////////////////////////////////////////////////

//ģ������
typedef CMap<WORD,WORD,IUnknownEx *,IUnknownEx * &> CGlobalModuleMap;

//////////////////////////////////////////////////////////////////////////

//ȫ�ֵ�Ԫ
class CGlobalUnits : public IGlobalUnits
{
	//�汾��Ϣ
protected:
	DWORD							m_dwPlazaVersion;					//�����汾
	DWORD							m_dwFrameVersion;					//��ܰ汾

	//�ؼ�����
public:
	CDlgStatus						m_DlgStatus;						//״̬����

	//ȫ�����
public:
	CGlobalStation					m_GlobalStation;					//վ����Ϣ
	CGlobalUserInfo					m_GlobalUserInfo;					//�û���Ϣ
	CParameterGlobal				m_ParameterGlobal;					//ȫ������

	//�������
public:
	CDispatchManager				m_DispatchManager;					//�¼�����

	//�������
public:
	CPlatformResourceHelper			m_PlatformResourceModule;			//��Դ���

	//��������
public:
	//���캯��
	CGlobalUnits();
	//��������
	virtual ~CGlobalUnits();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//�汾��Ϣ
public:
	//�����汾
	virtual DWORD __cdecl GetPlazaVersion() { return m_dwPlazaVersion; }
	//��ܰ汾
	virtual DWORD __cdecl GetFrameVersion() { return m_dwFrameVersion; }

	//���ú���
public:
	//�������
	bool InitGlobalUnits();
};

//////////////////////////////////////////////////////////////////////////

//����˵��
extern CGlobalUnits					g_GlobalUnits;						//ȫ�ֵ�Ԫ

//////////////////////////////////////////////////////////////////////////

#endif
