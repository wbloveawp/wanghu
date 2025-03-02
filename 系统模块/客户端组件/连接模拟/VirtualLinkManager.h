#ifndef VIRTUAL_LINK_MANAGER_HEAD_FILE
#define VIRTUAL_LINK_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "SimulateLinkHead.h"

//////////////////////////////////////////////////////////////////////////

//��˵��
class CVirtualLinkItem;
class CVirtualLinkManager;

//////////////////////////////////////////////////////////////////////////
//�ص��ӿ�
interface IVirtualLinkSink
{
	//�ر��¼�
	virtual bool OnEventVirtualShut() = NULL;
	//�����¼�
	virtual bool OnEventVirtualLink(LONG lErrorCode) = NULL;	
	//��ȡ�¼�
	virtual bool OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize) = NULL;
};

//�ص��ӿ�
interface IVirtualManagerSink
{
	//����֪ͨ
	virtual bool OnEventTCPSocketLink(INT nErrorCode)=NULL;
	//�ر�֪ͨ
	virtual bool OnEventTCPSocketShut(BYTE cbShutReason)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//��������
class CVirtualLinkItem : public IVirtualLinkSink
{
	//��Ԫ����
	friend class CVirtualLinkManager;

	//״̬����
protected:
	bool						m_bLinkStatus;							//����״̬

	//ģ����Ϣ
protected:
	WORD						m_wServiceID;							//�����ʶ
	WORD						m_wServiceModule;						//����ģ��	
	
	//��������
private:
	CVirtualLinkManager *		m_pVirtualLinkManager;					//���ӹ���

	//��������
public:
	//���캯��
	CVirtualLinkItem();
	//��������
	virtual ~CVirtualLinkItem();

	//��Ϣ����
public:
	//����״̬
	bool GetLinkStatus() { return m_bLinkStatus; }
	//�����ʶ
	WORD GetServiceID() { return m_wServiceID; }
	//����ģ��
	WORD GetServiceModule() { return m_wServiceModule; }	

	//���ú���
public:
	//����ģ��
	VOID SetServiceModule(WORD wServiceModule) { m_wServiceModule=wServiceModule; }
	
	//���ͺ���
public:
	//��������
	bool SendVirtualData(WORD wMainCmdID, WORD wSubCmdID);
	//��������
	bool SendVirtualData(WORD wMainCmdID, WORD wSubCmdID, VOID* const pData, WORD wDataSize);

	//������
public:
	//�ر�����
	VOID CloseVirtualLink();
	//��������
	bool CreateVirtualLink(WORD wServiceID, DWORD dwUserID, LPCTSTR pszLogonToken, BYTE cbExtendData[], WORD wDataSize);

	//���ܺ���
public:
	//���ӹ���
	CVirtualLinkManager * GetVirtualLinkManager() { return m_pVirtualLinkManager; }
};

//////////////////////////////////////////////////////////////////////////

//����˵��
typedef CWHArray<CVirtualLinkItem *> CVirtualLinkPtrArray;

//���ӵ�ǲ
class CVirtualLinkManager : public CWnd, public ITCPSocketSink
{
	//��Ԫ����
	friend class CVirtualLinkItem;

	//������Ϣ
protected:
	WORD							m_wServerPort;						//����˿�
	DWORD							m_dwServerAddr;						//�����ַ

	//�������
protected:
	CTCPSocketHelper				m_TCPSocketModule;					//��������

	//��������
protected:
	CVirtualLinkPtrArray			m_VirtualLinkArray;					//��������

	//�ӿڱ���
protected:
	IVirtualManagerSink*			m_pIVirtualManagerSink;				//�ص��ӿ�

	//��������
public:
	//���캯��
	CVirtualLinkManager();
	//��������
	virtual ~CVirtualLinkManager();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() {}
	//�ӿڲ�ѯ
	virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
protected:
	//�����¼�
	virtual bool OnEventTCPSocketLink(WORD wSocketID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(WORD wSocketID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(WORD wSocketID, CMD_Command Command, VOID* pData, WORD wDataSize);

	//״̬����
public:
	//����״̬
	BYTE GetSocketStatus();	

	//������
public:
	//��������
	bool InsertVirtualLink(CVirtualLinkItem * pVirtualLinkItem);
	//ɾ������
	bool DeleteVirtualLink(CVirtualLinkItem * pVirtualLinkItem);
	//���ýӿ�
	VOID SetVirtualManagerSink(IVirtualManagerSink * pIVirtualManagerSink);

	//���ܺ���
public:
	//�ж�����
	bool PerformClose();
	//��������
	bool PerformConnect(LPCTSTR pszServerAddr, WORD wServerPort);

	//���ܺ���
protected:
	//���ͺ���
	bool SendSocketData(WORD wMainCmdID, WORD wSubCmdID);
	//���ͺ���
	bool SendSocketData(WORD wMainCmdID, WORD wSubCmdID, VOID * const pData, WORD wDataSize);

	//��Ϣӳ��
protected:
	//ʱ����Ϣ
	VOID OnTimer(UINT_PTR nIDEvent);

	//�ڲ�����
private:
	//��ַ����
	DWORD TranslateAddress(LPCTSTR szServerIP);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif