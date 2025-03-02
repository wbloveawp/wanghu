#pragma once

#include "Stdafx.h"
#include <random>
#include "ModuleLogon.h"
#include "ModuleMatch.h"
#include "ModuleServer.h"
#include "ModuleBattle.h"
#include "ModuleClub.h"
#include "ModuleClubPlaza.h"
#include "SimulateLinkHead.h"
#include "VirtualLinkManager.h"

//////////////////////////////////////////////////////////////////////////

//�ͻ�������
class CSimulateClient : public CWnd, public IModuleItemSink, public IVirtualManagerSink
{
	//��¼��Ϣ
protected:
	TCHAR							m_szAccounts[LEN_ACCOUNTS];			//�û��˺�
	TCHAR							m_szPassword[LEN_PASSWORD];			//�û�����	

	//�û�����
protected:
	tagGlobalUserData				m_GlobalUserData;					//�û�����	

	//�������
public:
	CModuleClub						m_ModuleClub;						//����ģ��
	CModuleLogon					m_ModuleLogon;						//��¼ģ��
	CModuleMatch					m_ModuleMatch;						//����ģ��
	CModuleServer					m_ModuleServer;						//����ģ��
	CModuleBattle					m_ModuleBattle;						//Լսģ��
	CModuleClubPlaza				m_ModuleClubPlaza;					//����ģ��
	CVirtualLinkManager				m_VirtualLinkManager;				//���ӹ���	

	//��������
public:
	//���캯��
	CSimulateClient();
	//��������
	virtual ~CSimulateClient();

	//�����¼�
public:
	//����֪ͨ
	virtual bool OnEventTCPSocketLink(INT nErrorCode);
	//�ر�֪ͨ
	virtual bool OnEventTCPSocketShut(BYTE cbShutReason);

	//ʱ�ӽӿ�
public:
	//����ʱ��
	virtual VOID KillGameTimer(UINT uTimerID);
	//����ʱ��
	virtual VOID SetGameTimer(UINT uTimerID, DWORD dwElapse);

	//��Ϣ�ӿ�
public:
	//�û�����
	virtual tagGlobalUserData* GetGlobalUserData() { return &m_GlobalUserData; }

	//ģ��ӿ�
public:
	//����ģ��
	virtual VOID * GetModuleClub() { return &m_ModuleClub; }
	//����ģ��
	virtual VOID * GetModuleServer() { return &m_ModuleServer; }
	//Լսģ��
	virtual VOID * GetModuleBattle() { return &m_ModuleBattle; }
	//����ģ��
	virtual VOID * GetModuleClubPlaza() { return &m_ModuleClubPlaza; }	

	//���ܽӿ�
public:
	//������Ϣ
	virtual VOID InsertString(LPCTSTR pszString, COLORREF crColor);
	//Ͷ����Ϣ
	virtual VOID PostWindowMessage(UINT MsgID, WPARAM wParam, LPARAM lParam);

	//��Ϣ����
public:
	//����״̬
	BYTE GetConnectStatus() { return m_VirtualLinkManager.GetSocketStatus(); }

	//���ú���
public:
	//�����˺�
	VOID SetAccountInfo(LPCTSTR pszAccounts, LPCTSTR pszPassword);

	//������
public:
	//�رտͻ���
	VOID CloseCleint();
	//�����ͻ���
	bool StartClient(LPCTSTR pszServerAddr, WORD wServerPort);	

	//��Ϣ����
protected:
	//ʱ����Ϣ
	VOID OnTimer(UINT_PTR nIDEvent);

	//��¼��Ϣ
protected:
	//���ӹر�
	LRESULT OnLogonLinkShut(WPARAM wParam, LPARAM lParam);
	//����ʧ��
	LRESULT OnLogonLinkFailure(WPARAM wParam, LPARAM lParam);
	//��¼���
	LRESULT OnLogonLogonFinish(WPARAM wParam, LPARAM lParam);
	//��¼ʧ��
	LRESULT OnLogonLogonFailure(WPARAM wParam, LPARAM lParam);

	//������Ϣ
protected:
	//���ӹر�
	LRESULT OnMatchLinkShut(WPARAM wParam, LPARAM lParam);
	//����ʧ��
	LRESULT OnMatchLinkFailure(WPARAM wParam, LPARAM lParam);
	//��¼���
	LRESULT OnMatchLogonFinish(WPARAM wParam, LPARAM lParam);
	//��¼ʧ��
	LRESULT OnMatchLogonFailure(WPARAM wParam, LPARAM lParam);

	//������Ϣ
protected:
	//���ӹر�
	LRESULT OnClubLinkShut(WPARAM wParam, LPARAM lParam);
	//����ʧ��
	LRESULT OnClubLinkFailure(WPARAM wParam, LPARAM lParam);
	//��¼���
	LRESULT OnClubLogonFinish(WPARAM wParam, LPARAM lParam);
	//��¼ʧ��
	LRESULT OnClubLogonFailure(WPARAM wParam, LPARAM lParam);

	//����㳡
protected:
	//���ӹر�
	LRESULT OnClubPlazaLinkShut(WPARAM wParam, LPARAM lParam);
	//����ʧ��
	LRESULT OnClubPlazaLinkFailure(WPARAM wParam, LPARAM lParam);
	//��¼���
	LRESULT OnClubPlazaLogonFinish(WPARAM wParam, LPARAM lParam);
	//��¼ʧ��
	LRESULT OnClubPlazaLogonFailure(WPARAM wParam, LPARAM lParam);

	//Լս��Ϣ
protected:
	//���ӹر�
	LRESULT OnBattleLinkShut(WPARAM wParam, LPARAM lParam);
	//����ʧ��
	LRESULT OnBattleLinkFailure(WPARAM wParam, LPARAM lParam);
	//��¼���
	LRESULT OnBattleLogonFinish(WPARAM wParam, LPARAM lParam);
	//��¼ʧ��
	LRESULT OnBattleLogonFailure(WPARAM wParam, LPARAM lParam);

	//������Ϣ
protected:
	//���ӹر�
	LRESULT OnServerLinkShut(WPARAM wParam, LPARAM lParam);
	//����ʧ��
	LRESULT OnServerLinkFailure(WPARAM wParam, LPARAM lParam);
	//��¼���
	LRESULT OnServerLogonFinish(WPARAM wParam, LPARAM lParam);
	//��¼ʧ��
	LRESULT OnServerLogonFailure(WPARAM wParam, LPARAM lParam);
	//�������
	LRESULT OnServerMatchCompleted(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////
