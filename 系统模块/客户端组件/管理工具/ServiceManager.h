#ifndef SERVICE_MANAGER_HEAD_FILE
#define SERVICE_MANAGER_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////

class CServiceManager;

//����ص�
interface IServiceSocketSink
{
	//���Ӻ���
public:
	//�����¼�
	virtual bool OnEventMissionLink(INT nErrorCode)=NULL;
	//�ر��¼�
	virtual bool OnEventMissionShut(BYTE cbShutReason)=NULL;
	//��ȡ�¼�
	virtual bool OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)=NULL;

	//���ú���
public:
	//���ù�����
	virtual VOID SetServiceManager( CServiceManager *pServiceManager )=NULL;
	//���û״̬
	virtual VOID SetActiveStatus(bool bActiveStatus)=NULL;

	//״̬����
public:
	//��ȡ������
	virtual CServiceManager *GetServiceManager()=NULL;
	//�Ƿ�״̬
	virtual bool IsActiveStatus()=NULL;
};

//////////////////////////////////////////////////////////////////////////

//����˵��
typedef CWHArray<IServiceSocketSink *> CServiceItemPtrArray;

//�����ǲ
class CServiceManager : public CWnd, public IMissionSocketSink, public IRedirectServiceSink
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
	CServiceItemPtrArray			m_MissionItemArray;					//��������

	//��̬����
public:
	static CServiceManager			*m_pServiceManager;					//�������

	//��������
public:
	//���캯��
	CServiceManager();
	//��������
	virtual ~CServiceManager();
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
	////�������
	//virtual bool __cdecl OnRedirectComplete(LPCTSTR pszServerIP, int nParam);
	////�����쳣
	//virtual bool __cdecl OnRedirectException(enDownLoadError DownLoadError);

	//������
public:
	//���õ�ַ
	VOID SetCurrentServer(LPCTSTR pszCurrentServer);
	//��������
	bool InsertMissionItem(IServiceSocketSink * pMissionItem);
	//ɾ������
	bool DeleteMissionItem(IServiceSocketSink * pMissionItem);

	//���ܺ���
public:
	//��������
	bool AvtiveMissionItem(IServiceSocketSink * pMissionItem, bool bContinue);
	//��ֹ����
	bool ConcludeMissionItem(IServiceSocketSink * pMissionItem);

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

	//��̬����
public:
	static CServiceManager *GetServiceManager() { return m_pServiceManager; }

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif