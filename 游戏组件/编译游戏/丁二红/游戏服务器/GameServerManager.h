#ifndef GAME_SERVER_MANAGER_HEAD_FILE
#define GAME_SERVER_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgCustomRule.h"

//////////////////////////////////////////////////////////////////////////

//��Ϸ������������

class CGameServiceManager : public IGameServiceManager, public IGameServiceCustomRule
{
	//�ؼ�����
protected:
	HINSTANCE							m_hDllInstance;						//DLL
	CDlgCustomRule 					m_pDlgCustomRule;				//�Զ�����

	//��������
protected:
	tagGameServiceAttrib				m_GameServiceAttrib;			//��������

	//��������
public:
	//���캯��
	CGameServiceManager(void);
	//��������
	virtual ~CGameServiceManager(void);

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { }
	//�Ƿ���Ч
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CGameServiceManager))?true:false; }
	//�ӿڲ�ѯ
	virtual void * __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��ʼ��
	virtual bool StartService() { return true; }
	//ֹͣ����
	virtual bool ConcludeService() { return true; }

	//���ݽӿ�
public:
	//��������
	virtual VOID * CreateDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer) { return NULL; }
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, VOID * pData, WORD wDataSize, DWORD dwContextID, DWORD dwTokenID) { return true; }

	//���ýӿ�
public:
	//��ȡ����
	virtual bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
	//Ĭ������
	virtual bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
	//��������
	virtual HWND CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize);

	//�����ӿ�
public:
	//��������
	virtual VOID * CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer) ;
	//��������
	virtual VOID * CreateRobotUserItemSink(REFGUID Guid, DWORD dwQueryVer) ;
	//��ȡ����
	virtual bool __cdecl GetGameServiceAttrib(tagGameServiceAttrib & GameServiceAttrib);
	//�޸Ĳ���
	virtual bool __cdecl RectifyOptionParameter(tagGameServiceOption & GameServiceOption);
};

//////////////////////////////////////////////////////////////////////////

#endif
