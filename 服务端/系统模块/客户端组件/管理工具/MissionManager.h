#ifndef MISSION_MANAGER_HEAD_FILE
#define MISSION_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

//��˵��
class CMissionManager;

//////////////////////////////////////////////////////////////////////////

//����ص�
interface IMissionSocketSink
{
	//�����¼�
	virtual bool OnEventMissionLink(INT nErrorCode)=NULL;
	//�ر��¼�
	virtual bool OnEventMissionShut(BYTE cbShutReason)=NULL;
	//��ȡ�¼�
	virtual bool OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//��������
class CMissionSocket : public ITCPSocketSink
{
	//������Ϣ
protected:
	WORD							m_wCurrentPort;						//��ǰ�˿�
	DWORD							m_dwCurrentServer;					//��ǰ��ַ

	//�������
protected:
	CTCPSocketHelper				m_TCPSocketModule;					//��������

	//�ӿڱ���
protected:
	IMissionSocketSink *			m_pIMissionSocketSink;				//�ص��ӿ�

	//��������
public:
	//���캯��
	CMissionSocket();
	//��������
	virtual ~CMissionSocket();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
protected:
	//�����¼�
	virtual bool __cdecl OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode);
	//�ر��¼�
	virtual bool __cdecl OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool __cdecl OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID * pData, WORD wDataSize);

	//״̬����
public:
	//����״̬
	BYTE GetSocketStatus();

	//���ú���
public:
	//���ýӿ�
	VOID SetMissionSocketSink(IMissionSocketSink * pIMissionSocketSink) { m_pIMissionSocketSink=pIMissionSocketSink; }
	//����������Ϣ
	VOID SetConnectInfo( DWORD dwServerIP, WORD wPort );

	//���ܺ���
public:
	//�ж�����
	bool PerformClose();
	//��������
	bool PerformConnect();
	//���ͺ���
	bool PerformSendData(WORD wMainCmdID, WORD wSubCmdID);
	//���ͺ���
	bool PerformSendData(WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

//��������
class CMissionItem : public IMissionSocketSink
{
	//��Ԫ����
	friend class CMissionManager;

	//��������
private:
	bool						m_bActiveStatus;						//����״̬
	CMissionManager *			m_pMissionManager;						//�������

	//��������
public:
	//���캯��
	CMissionItem();
	//��������
	virtual ~CMissionItem();

	//���ܺ���
public:
	//����״̬
	bool GetActiveStatus() { return m_bActiveStatus; }
	//�������
	CMissionManager * GetMissionManager() { return m_pMissionManager; }
};

//////////////////////////////////////////////////////////////////////////

//����˵��
typedef CWHArray<CMissionItem *> CMissionItemPtrArray;

//�����ǲ
class CMissionManager : public CWnd, public IMissionSocketSink, public IRedirectServiceSink
{
	//״̬����
protected:
	bool							m_bSwitchDns;						//DNS ת��
	bool							m_bTryDefault;						//����Ĭ��
	bool							m_bTryRedirect;						//���Զ���
	WORD							m_wSwitchIndex;						//�л�����

	//������Ϣ
protected:
	WORD							m_wCurrentPort;						//��ǰ�˿�
	DWORD							m_dwCurrentServer;					//��ǰ��ַ
	TCHAR							m_szCurrentServer[LEN_SERVER];		//��ǰ��ַ

	//�������
protected:
	CMissionSocket					m_MissionSocket;					//��������
	CRedirectServiceHelper			m_RedirectServiceHelper;			//�������

	//��������
protected:
	CMissionItemPtrArray			m_MissionItemArray;					//��������

	//��������
public:
	//���캯��
	CMissionManager();
	//��������
	virtual ~CMissionManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
protected:
	//�����¼�
	virtual bool OnEventMissionLink(INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize);

	//����ӿ�
protected:
	//�������
	//virtual bool __cdecl OnRedirectComplete(LPCTSTR pszServerIP, int nParam);
	////�����쳣
	//virtual bool __cdecl OnRedirectException(enDownLoadError DownLoadError);

	//������
public:
	//���õ�ַ
	VOID SetCurrentServer(LPCTSTR pszCurrentServer);
	//��������
	bool InsertMissionItem(CMissionItem * pMissionItem);
	//ɾ������
	bool DeleteMissionItem(CMissionItem * pMissionItem);

	//���ܺ���
public:
	//��������
	bool AvtiveMissionItem(CMissionItem * pMissionItem, bool bContinue);
	//��ֹ����
	bool ConcludeMissionItem(CMissionItem * pMissionItem);

	//���ܺ���
public:
	//���ͺ���
	bool SendData(WORD wMainCmdID, WORD wSubCmdID);
	//���ͺ���
	bool SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize);

	//��Ϣӳ��
protected:
	//ʱ����Ϣ
	VOID OnTimer(UINT_PTR nIDEvent);

	//�ڲ�����
private:
	//��ַ��Ϣ
	bool GetServerInfo();
	//�л�����
	bool SwitchServerItem();
	//���¶���
	bool RedirectionServer();
	//������������
	void CreateVirtualLink();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif