#ifndef PLATFORM_FRAME_HEAD_FILE
#define PLATFORM_FRAME_HEAD_FILE

#pragma once

//ϵͳ�ļ�
#include "Stdafx.h"
#include "CardDispatcher.h"
#include "DlgSessionItem.h"
#include "DlgSessionRobot.h"
#include "DlgSessionStock.h"
#include "DlgSessionConfig.h"
#include "DlgSessionServer.h"

//�����ļ�
#include "SessionLogon.h"
#include "VirtualLinkManager.h"

//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IPlatformFrame INTERFACE_VERSION(1,1)
static const GUID IID_IPlatformFrame = { 0xa2565921, 0x7434, 0x4a0d, 0x9f, 0x83, 0xd5, 0x35, 0xd1, 0x97, 0x90, 0xb8 };
#else
#define VER_IPlatformFrame INTERFACE_VERSION(1,1)
static const GUID IID_IPlatformFrame = { 0xc70a9f11, 0xfa16, 0x4a91, 0x86, 0xb2, 0x84, 0xc9, 0x24, 0xa2, 0x1b, 0xca };
#endif

//�����ӿ�
interface IPlatformFrame : public IUnknownEx
{
};

//ƽ̨���
class CPlatformFrame : public CDialog, public IPlatformFrame, public IServerListViewSink
{
	//��������
protected:
	int								m_nSelectItem;						//ѡ������	
	bool							m_bLogonSuccess;					//��¼�ɹ�	

	//��Ϸ�б�
protected:
	CServerListManager				m_ServerListManager;				//��Ϸ����	

	//�ؼ�����
protected:
	BYTE							m_cbItemIndex;						//��������
	CDlgSessionItem *				m_pDlgSessionItem[6];				//��������
	CDlgSessionRobot				m_DlgSessionRobot;					//��������
	CDlgSessionStock				m_DlgSessionStock;					//������
	CDlgSessionConfig				m_DlgSessionConfig;					//���ù���
	CDlgSessionOption				m_DlgSessionOption;					//��������
	CDlgSessionServer				m_pDlgSessionServer;				//����Ự
	CDlgSessionCommand				m_DlgSessionCommand;				//��������	

	//�ؼ�����
protected:		
	CSystemTrayIcon					m_SystemTrayIcon;					//����ͼ��

	//�������
protected:
	CSessionLogon					m_MissionLogon;						//��¼����

	//�������
protected:
	CVirtualLinkItem				m_VirtualLinkLogon;					//��¼����
	CVirtualLinkItem				m_VirtualLinkServer;				//��������
	CVirtualLinkManager				m_VirtualLinkManager;				//���ӹ���

	//��������
protected:
	WORD							m_wServerType;						//��������
	WORD							m_wTableID;							//����ID
	DWORD							m_dwServerRule;						//�������
	tagGameKind						m_GameKind;							//������Ϣ
	tagPCGameServer					m_GameServer;						//������Ϣ
	enServiceStatus					m_ServiceStatus;					//����״̬

	//�û�����
protected:
	DWORD							m_dwUserRight;						//�û�Ȩ��
	DWORD							m_dwMasterRight;					//����Ȩ��
	IClientUserItem*				m_pIMySelfUserItem;					//�Լ�ָ��
	IClientUserItem*				m_pISelectedUserItem;				//ѡ��ָ��

	//�������
protected:
	CTCPSocketHelper				m_TCPSocketModule;					//��������	
	CUserListControl				m_UserListControl;					//�û��б�
	CPlazaUserManagerHelper			m_PlazaUserManagerModule;			//�û�����

	//��̬����
protected:
	static CPlatformFrame *			m_pPlatformFrame;					//���ָ��

	//��������
public:
	//���캯��
	CPlatformFrame();
	//��������
	virtual ~CPlatformFrame();

	//���غ���
protected:
	//��ʼ����
	virtual BOOL OnInitDialog();
	//��Ϣ����
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//�����
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//��������
	virtual VOID DoDataExchange(CDataExchange* pDX);

	//״̬֪ͨ
public:
	//���֪ͨ
	virtual VOID OnGameItemFinish();
	//���֪ͨ
	virtual VOID OnGameKindFinish(WORD wKindID);

	//����֪ͨ
public:
	//����֪ͨ
	virtual VOID OnGameItemInsert(CGameListItem * pGameListItem);
	//����֪ͨ
	virtual VOID OnGameItemUpdate(CGameListItem * pGameListItem);
	//ɾ��֪ͨ
	virtual VOID OnGameItemDelete(CGameListItem * pGameListItem);

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//��Ϣ����
protected:
	//ѡ����
	VOID OnTcnSelchangeManagerOption(NMHDR* pNMHDR, LRESULT* pResult);

	//����֪ͨ
public:
	//����֪ͨ
	virtual VOID OnUpdateOnlineCount(DWORD dwOnlineCount);

	//��������
private:
	//��ʼ���ؼ�
	VOID InitControls();
	//��������
	VOID ActiveViewItem(BYTE cbItemIndex);
	//�����ؼ�
	VOID RectifyControl(INT nWidth, INT nHeight);

	//��Ϣ����
protected:
	//�ر���Ϣ
	VOID OnClose();	
	//������Ϣ
	VOID OnDestroy();
	//�滭����
	BOOL OnEraseBkgnd(CDC * pDC);
	//λ����Ϣ
	VOID OnSize(UINT nType, INT cx, INT cy);

	//�û��˵�
protected:
	//��ʾ�˵�
	virtual VOID TrackUserItemMenu(IClientUserItem* pIClientUserItem);

	//�Ҽ��б�
	VOID OnNMRclickUserList(NMHDR* pNMHDR, LRESULT* pResult);

	//�Զ���Ϣ
protected:
	//�¼���Ϣ
	LRESULT OnEventInformMessage(WPARAM wParam, LPARAM lParam);

	//���ܺ���
public:
	//��ȡʵ��
	static CPlatformFrame * GetInstance() { return m_pPlatformFrame; }

	DECLARE_MESSAGE_MAP()				
};

//////////////////////////////////////////////////////////////////////////

#endif