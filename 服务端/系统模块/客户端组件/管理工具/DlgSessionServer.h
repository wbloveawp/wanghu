#ifndef DLG_ROOM_CONFIG_HEAD_FILE
#define DLG_ROOM_CONFIG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CheckComboBox.h"
#include "CardDispatcher.h"
#include "DlgSessionItem.h"

//////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IRoomConfig INTERFACE_VERSION(1,1)
static const GUID IID_IRoomConfig = { 0x24d6aae1, 0x8f5a, 0x4b67, 0x95, 0x52, 0xfb, 0xa3, 0xe5, 0x6a, 0xc8, 0x29 };
#else
#define VER_IRoomConfig INTERFACE_VERSION(1,1)
static const GUID IID_IRoomConfig = { 0x3ba8a607, 0xa696, 0x4cfa, 0x92, 0xcc, 0xa4, 0x7b, 0x46, 0xa4, 0xd7, 0x71 };
#endif

//�����ӿ�
interface IRoomConfig : public IUnknownEx
{
};

//����״̬
enum enServiceStatus
{
	ServiceStatus_Unknow,			//δ֪״̬
	ServiceStatus_Entering,			//����״̬
	ServiceStatus_Validate,			//��֤״̬
	ServiceStatus_RecvInfo,			//��ȡ״̬
	ServiceStatus_ServiceIng,		//����״̬
	ServiceStatus_NetworkDown,		//�ж�״̬
};
//////////////////////////////////////////////////////////////////////////

//����Ự
class CDlgSessionServer : public CDlgSessionItem, public IRoomConfig, public IUserManagerSink, public ICardDispatcherCallback, public IClientDebugCallback
{
	//�û�����
protected:
	DWORD							m_dwUserRight;						//�û�Ȩ��
	DWORD							m_dwMasterRight;					//����Ȩ��
	IClientUserItem*				m_pIMySelfUserItem;					//�Լ�ָ��
	IClientUserItem*				m_pISelectedUserItem;				//ѡ��ָ��

	//�ؼ�����
protected:
	CCheckComboBox					m_cbGameList;						//��Ϸ�б�
	CComboBox						m_cbServerList;						//�����б�

	//��������
protected:
	WORD							m_wServerType;						//��������
	WORD							m_wTableID;							//����ID
	DWORD							m_dwServerRule;						//�������
	tagGameKind						m_GameKind;							//������Ϣ
	tagPCGameServer					m_GameServer;						//������Ϣ
	enServiceStatus					m_ServiceStatus;					//����״̬

	//������Ϣ
protected:
	WORD							m_wCurrentPort;						//��ǰ�˿�
	DWORD							m_dwCurrentServer;					//��ǰ��ַ

	//�������
protected:
	CUserListControl				m_UserListControl;					//�û��б�
	CPlazaUserManagerHelper			m_PlazaUserManagerModule;			//�û�����

	//������
protected:
	HINSTANCE						m_hCardDispatcherInst;				//���������
	ICardDispatcher*				m_pICardDispatcher;					//�������ӿ�

	//���Ա���
protected:
	HINSTANCE						m_hClientDebugInst;					//���Ծ��
	IClientDebug*					m_pIClientDebug;					//���Խӿ�

	//��������
protected:
	HINSTANCE						m_hRobotDebugInst;					//�������
	IClientDebug*					m_pIRobotDebug;						//�����ӿ�

	//��������
public:
	//���캯��
	CDlgSessionServer();
	//��������
	virtual ~CDlgSessionServer();

	//���غ���
protected:
	//��ʼ����
	virtual BOOL OnInitDialog();
	//��������
	virtual VOID DoDataExchange(CDataExchange* pDX);
	//�����
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//֪ͨ�¼�
public:
	//�б����
	virtual VOID OnEventListFinish();

	//����ӿ�
protected:
	//�����¼�
	virtual bool OnEventMissionLink(LONG nErrorCode);
	//�ر��¼�
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize);

	//�������ӿ�
public:
	// ��������Ϣ
	virtual bool OnCardDispatcherDebugInfo(WORD wTableID, void* pData, WORD nSize);

	//�û��˵�
protected:
	//��ʾ�˵�
	virtual VOID TrackUserItemMenu(IClientUserItem* pIClientUserItem);	

	//�û��ӿ�
public:
	//�û�����
	virtual VOID OnUserItemAcitve(IClientUserItem* pIClientUserItem);
	//�û�ɾ��
	virtual VOID OnUserItemDelete(IClientUserItem* pIClientUserItem);
	//�û�����
	virtual VOID OnUserFaceUpdate(IClientUserItem* pIClientUserItem);
	//�û�����
	virtual VOID OnUserItemUpdate(IClientUserItem* pIClientUserItem, const tagUserStatus& LastStatus);
	//�û�����
	virtual VOID OnUserItemUpdate(IClientUserItem* pIClientUserItem, const tagClientUserScore& LastScore);	

	//���Խӿ�
public:
	//��������
	virtual bool SendDebugData(WORD nMessageID, WORD wTableID, void* pData, WORD wDataSize);	

	//�¼�����
protected:
	//���뷿��
	VOID OnBnClickedBtEnterRoom();
	//�˳�����
	VOID OnBnClickedBtExitRoom();
	//��Ϸ����
	VOID OnBnClickedBtGameDebug();
	//��������
	VOID OnBnClickedBtRobotDebug();
	//������
	VOID OnBnClickedCardDispatcher();
	//�Ҽ��б�
	VOID OnNMRclickUserList(NMHDR* pNMHDR, LRESULT* pResult);

	//�����¼�
protected:
	//�û�����
	bool OnSocketMainUser(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//��¼����
	bool OnSocketMainLogon(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//���Դ���
	bool OnSocketMainDebug(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//������
	bool OnSocketMainManager(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//��Ϸ����
	bool OnSocketMainGameFrame(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//�����¼�
protected:
	//��¼���
	bool OnSocketSubLogonFinish(VOID* pData, WORD wDataSize);
	//��¼�ɹ�
	bool OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize);
	//��¼ʧ��
	bool OnSocketSubLogonFailure(VOID* pData, WORD wDataSize);
	//������ʾ
	bool OnSocketSubUpdateNotify(VOID* pData, WORD wDataSize);
	//ϵͳ��Ϣ
	bool OnSocketSubSystemMessage(VOID* pData, WORD wDataSize);
	//�û�����
	bool OnSocketSubUserEnter(VOID* pData, WORD wDataSize);
	//�û�����
	bool OnSocketSubUserScore(VOID* pData, WORD wDataSize);
	//�û�״̬
	bool OnSocketSubUserStatus(VOID* pData, WORD wDataSize);
	//�������ݰ�
	bool OnSocketSubDebugPacket(VOID* pData, WORD wDataSize);	

	//�ڲ�����
private:
	// ���͵�¼
	bool SendLogonPacket();
	// ʹ�ܰ�ť
	VOID EnableButton(bool bEnabled);
	//�û�����
	VOID OnEventUserEnter(IClientUserItem* pIClientUserItem);
	//�û��뿪
	VOID OnEventUserLeave(IClientUserItem* pIClientUserItem);
	//����״̬
	VOID SetServiceStatus(enServiceStatus ServiceStatus);
	//��ȡ��Ϸ�汾
	VOID GetProcessVersion(WORD wKindID, DWORD& dwProcessVersion);
	//�����б�����
	VOID CleanListData();
	//��ȡ������Ϣ
	void GetConnectInfo();

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////////////

#endif