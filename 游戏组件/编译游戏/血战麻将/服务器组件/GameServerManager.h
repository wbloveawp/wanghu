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
	HINSTANCE							m_hDllInstance;						//DLL ���
	CDlgCustomRule *					m_pDlgCustomRule;					//�Զ�����

	//��������
protected:
	tagGameServiceAttrib				m_GameServiceAttrib;				//��������

	//�������
	CGameServiceManagerHelper			m_AndroidServiceHelper;				//�����˷���

	//��������
public:
	//���캯��
	CGameServiceManager(void);
	//��������
	virtual ~CGameServiceManager(void);

	//����ӿ�
public:
	//��ʼ��
	virtual bool StartService() { return true; }
	//ֹͣ����
	virtual bool ConcludeService() { return true; }

	

	//���ݽӿ�
public:
	//��������
	virtual VOID* CreateDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer) { return NULL; }
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, VOID* pData, WORD wDataSize, DWORD dwContextID, DWORD dwTokenID) { return true; }


	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { }
	//�Ƿ���Ч
	virtual bool IsValid() { return AfxIsValidAddress(this,sizeof(CGameServiceManager))?true:false; }
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//�ӿں���
public:
	//��������
	virtual void * CreateTableFrameSink(const IID & Guid, DWORD dwQueryVer);
	//��������
	virtual VOID* CreateRobotUserItemSink(REFGUID Guid, DWORD dwQueryVer);
	//��������
	virtual VOID * CreateGameDataBaseEngineSink(REFGUID Guid, DWORD dwQueryVer) { return NULL; }
	

	//���ýӿ�
public:
	//��ȡ����
	virtual bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
	//Ĭ������
	virtual bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize);
	//��������
	virtual HWND CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize);
	//Ĭ�ϵȼ�����
	//virtual bool DefaultServerLevel(LPBYTE pcbCustomRule, WORD wCustonSize, tagGameServerLevelInfo & GameServerLevelInfo, WORD wCurSelServerLevel) { return true; }
   
	//��ȡ����
	virtual bool  GetGameServiceAttrib(tagGameServiceAttrib & GameServiceAttrib);
	//�޸Ĳ���
	virtual bool  RectifyOptionParameter(tagGameServiceOption & GameServiceOption);

};

//////////////////////////////////////////////////////////////////////////

#endif
