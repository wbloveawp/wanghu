#ifndef MISSION_LOGON_HEAD_FILE
#define MISSION_LOGON_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "DlgLogon.h"
#include "DlgStatus.h"
#include "MissionManager.h"

//////////////////////////////////////////////////////////////////////////
//�궨��

#define MT_LOGON					0									//��¼����

//////////////////////////////////////////////////////////////////////////

//��¼����
class CMissionLogon : public CMissionItem, public IHttpServiceSink
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
	static CMissionLogon *			m_pMissionLogon;					//����ָ��

	//��������
public:
	//���캯��
	CMissionLogon();
	//��������
	virtual ~CMissionLogon();

	//HTTP����ص�
public:
	////���󻺳�
	//virtual DWORD __cdecl OnHttpRequestBuffer(DWORD dwRequestSize, LPVOID *ppvBuffer);
	////�������
	//virtual bool __cdecl OnHttpDataComplete(DWORD dwDataSize);
	////�����쳣
	//virtual bool __cdecl OnHttpDataException(enDownLoadError DownLoadError);
	////���ӹر�
	//virtual bool __cdecl OnHttpSocketClose(bool bCloseByServer);

	//���غ���
public:
	//�����¼�
	virtual bool OnEventMissionLink(INT nErrorCode);
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
	static CMissionLogon * GetInstance() { return m_pMissionLogon; }
};

//////////////////////////////////////////////////////////////////////////

#endif