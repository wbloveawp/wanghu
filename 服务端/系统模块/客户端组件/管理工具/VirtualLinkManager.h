#ifndef VIRTUAL_LINK_MANAGER_HEAD_FILE
#define VIRTUAL_LINK_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"
//#include "SimulateLinkHead.h"

//////////////////////////////////////////////////////////////////////////

//����״̬
#define LINK_STATUS_IDLE				0								//����״̬
#define LINK_STATUS_WAIT				1								//�ȴ�״̬
#define LINK_STATUS_LINKED				2								//����״̬

//////////////////////////////////////////////////////////////////////////

//��˵��
class CVirtualLinkItem;
class CVirtualLinkManager;

//////////////////////////////////////////////////////////////////////////
//�ص��ӿ�
interface IVirtualLinkSink
{
	//׼������
	virtual bool OnEventSocketReady() = NULL;
	//�����¼�
	virtual bool OnEventVirtualLink(LONG lErrorCode) = NULL;
	//�ر��¼�
	virtual bool OnEventVirtualShut(BYTE cbShutReason) = NULL;			
	//��ȡ�¼�
	virtual bool OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize) = NULL;
};

//�ص��ӿ�
interface ISessionItemSink
{
	//�����¼�
	virtual bool OnEventMissionLink(LONG lErrorCode) = NULL;
	//�ر��¼�
	virtual bool OnEventMissionShut(BYTE cbShutReason) = NULL;	
	//��ȡ�¼�
	virtual bool OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize) = NULL;
};

//////////////////////////////////////////////////////////////////////////
// 
//��������
class CSessionItem : public ISessionItemSink
{
	//��Ԫ����
	friend class CVirtualLinkItem;

	//��������
protected:
	bool						m_bActiveStatus;						//����״̬
	CVirtualLinkItem *			m_pVirtualLinkItem;						//��������

	//��������
public:
	//���캯��
	CSessionItem();
	//��������
	virtual ~CSessionItem();

	//���ܺ���
public:
	//����״̬
	bool GetActiveStatus() { return m_bActiveStatus; }
	//��������
	CVirtualLinkItem* GetVirtualLinkItem() { return m_pVirtualLinkItem; }
};

//////////////////////////////////////////////////////////////////////////

//����˵��
typedef CWHArray<CSessionItem*> CSessionItemPtrArray;

//��������
class CVirtualLinkItem : public IVirtualLinkSink
{
	//��Ԫ����
	friend class CVirtualLinkManager;

	//״̬����
protected:
	BYTE						m_cbLinkStatus;							//����״̬	

	//ģ����Ϣ
protected:
	WORD						m_wServiceID;							//�����ʶ
	WORD						m_wServiceModule;						//����ģ��	

	//���Ӳ���
protected:
	CMD_GG_C_LinkService		m_LinkService;							//���Ӳ���

	//�������
protected:
	CSessionItemPtrArray		m_SessionItemArray;						//��������
	
	//��������
private:
	CVirtualLinkManager *		m_pVirtualLinkManager;					//���ӹ���

	//��������
public:
	//���캯��
	CVirtualLinkItem();
	//��������
	virtual ~CVirtualLinkItem();

	//�ص��ӿ�
public:
	//׼������
	virtual bool OnEventSocketReady();
	//�����¼�
	virtual bool OnEventVirtualLink(LONG lErrorCode);
	//�ر��¼�
	virtual bool OnEventVirtualShut(BYTE cbShutReason);	
	//��ȡ�¼�
	virtual bool OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize);

	//��Ϣ����
public:
	//�����ʶ
	WORD GetServiceID() { return m_wServiceID; }
	//����״̬
	BYTE GetLinkStatus() { return m_cbLinkStatus; }	
	//����ģ��
	WORD GetServiceModule() { return m_wServiceModule; }	

	//���ú���
public:
	//����ģ��
	VOID SetServiceModule(WORD wServiceModule) { m_wServiceModule=wServiceModule; }

	//�������
public:		
	//ɾ������
	VOID DeleteSessionItem();
	//ɾ������
	bool DeleteSessionItem(CSessionItem* pSessionItem);
	//��������
	bool InsertSessionItem(CSessionItem* pSessionItem);	
	
	//���ͺ���
public:
	//��������
	bool SendVirtualData(WORD wMainCmdID, WORD wSubCmdID);
	//��������
	bool SendVirtualData(WORD wMainCmdID, WORD wSubCmdID, VOID* const pData, WORD wDataSize);

	//���ӹ���
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
	CTCPSocketHelper				m_TCPSocketModule;					//��������

	//��������
protected:
	CVirtualLinkPtrArray			m_VirtualLinkArray;					//��������

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
	//���õ�ַ
	VOID SetCurrentServer(LPCTSTR pszCurrentServer);
	//��������
	bool InsertVirtualLink(CVirtualLinkItem * pVirtualLinkItem);
	//ɾ������
	bool DeleteVirtualLink(CVirtualLinkItem * pVirtualLinkItem);

	//���ܺ���
public:
	//�ر�����
	bool ConcludeVirtualLink();
	//��������
	bool ActiveVirtualLink(CVirtualLinkItem* pVirtualLinkItem);	

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
	//��ַ��Ϣ
	bool GetServerInfo();
	//�л�����
	bool SwitchServerItem();
	//��ַ����
	DWORD TranslateAddress(LPCTSTR szServerIP);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif