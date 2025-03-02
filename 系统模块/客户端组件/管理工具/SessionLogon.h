#ifndef MISSION_LOGON_HEAD_FILE
#define MISSION_LOGON_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgLogon.h"
#include "DlgStatus.h"
#include "VirtualLinkManager.h"

//////////////////////////////////////////////////////////////////////////
//�궨��

#define MT_LOGON					0									//��¼����

//////////////////////////////////////////////////////////////////////////

//��¼����
class CSessionLogon : public CSessionItem, public IHttpServiceSink
{
	//��Ԫ����
	friend class CDlgLogon;
	friend class CPlatformFrame;

	//��������
protected:
	BYTE							m_cbMissionType;					//�����ʶ
	CHAR							m_chBuffer[1024];					//���ݻ���

	//�ؼ�ָ��
protected:
	CDlgLogon *						m_pDlgLogon;						//��¼����
	CDlgStatus *					m_pDlgStatus;						//״̬����

	//�������
protected:
	CHttpServiceHelper				m_HttpServiceHelper;				//HTTP����

	//��֤����
protected:
	MSXML2::IXMLDOMDocumentPtr		m_pXmlDoc;							//XML���

	//��̬����
protected:
	static CSessionLogon *			m_pMissionLogon;					//����ָ��

	//��������
public:
	//���캯��
	CSessionLogon();
	//��������
	virtual ~CSessionLogon();

	//���غ���
public:
	//�����¼�
	virtual bool OnEventMissionLink(LONG lErrorCode);
	//�ر��¼�
	virtual bool OnEventMissionShut(BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize);

	//���ܺ���
public:
	//��ʾ��¼
	VOID ShowLogon();

	//���̺���
public:
	//ִ�е�¼
	bool PerformLogonMission(BYTE cbMissonType);

	//��������
protected:
	//�����û�
	VOID UpdateUserInfo();
	//��ʾ״̬
	VOID ShowStatusInfo(LPCTSTR pszStatusInfo);
	//�ر�״̬
	VOID CloseStatusInfo();
	//�ָ�����
	VOID RestoreShow();

	//�����¼�
protected:
	//��¼����
	bool OnSocketMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//ϵͳ����
	bool OnSocketMainSystem(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��ҷ���
	bool OnSocketMainService(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//�����б�
	bool OnSocketMainServerList(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//�����¼�
protected:
	//��¼�ɹ�
	bool OnSocketSubLogonSuccess(VOID * pData, WORD wDataSize);
	//��¼ʧ��
	bool OnSocketSubLogonFailure(VOID * pData, WORD wDataSize);
	//��¼���
	bool OnSocketSubLogonFinish(VOID * pData, WORD wDataSize);
	//��¼�ǳ�
	bool OnSocketSubLogoutNotify(VOID* pData, WORD wDataSize);
	//������ʾ
	bool OnSocketSubUpdateNotify(VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnSocketSubActionMessage(VOID * pData, WORD wDataSize);
	//��ȫ����
	bool OnSocketSubSafePrompt(VOID * pData, WORD wDataSize);
	//�����ʺ�
	bool OnSocketSubActivateAccounts(VOID * pData, WORD wDataSize);
	//������ʾ
	bool OnSocketSubShowOnLine(VOID * pData, WORD wDataSize);
	//��̨����
	bool OnSocketSubUpdateSilent(VOID * pData, WORD wDataSize);

	//��̬����
public:
	//��ȡ����
	static CSessionLogon * GetInstance() { return m_pMissionLogon; }
};

//////////////////////////////////////////////////////////////////////////

#endif