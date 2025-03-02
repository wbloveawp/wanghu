#ifndef MONITOR_CLIENT_HEAD_FILE
#define MONITOR_CLIENT_HEAD_FILE

#pragma once

#include "ServiceFrameHead.h"

//////////////////////////////////////////////////////////////////////////
//�ӿڶ���
interface IMonitorClientSink
{
	//�������
public:
	//��������
	virtual bool StartService()=NULL;
	//ֹͣ����
	virtual bool ConcludeService()=NULL;
};

//////////////////////////////////////////////////////////////////////////

//����ն�
class SERVICE_FRAME_CLASS CMonitorClient : public CWnd, public IProcessTradeSink
{
	//������Ϣ
protected:
	WORD							m_wServiceID;						//�����ʶ
	WORD							m_wServiceModule;					//����ģ��
	WORD							m_wServiceStatus;					//����״̬
	TCHAR							m_szServiceName[32];				//��������	

	//ͨѶ����
protected:
	HWND							m_hWndServer;						//���ھ��
	CProcessTradeHelper				m_ProcessTrade;						//ͨѶ���

	//�ӿڱ���
protected:
	IMonitorClientSink*				m_pIMonitorClientSink;				//�ص�ָ��

	//��������
public:
	//���캯��
	CMonitorClient();
	//��������
	virtual ~CMonitorClient();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);	

	//�������
public:
	//��������
	virtual bool StartService();
	//ֹͣ����
	virtual bool ConcludeService();	

	//����ͨѶ
public:
	//��������
	virtual bool OnProcessTradeData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize, HWND hWndSend);

	//���ú���
public:
	//���ýӿ�
	VOID SetMonitorClientSink(IMonitorClientSink* pIMonitorClientSink);
	//���ò���
	VOID SetServiceParameter(WORD wServiceModule, WORD wServiceID, LPCTSTR pszServiceName);

	//���ܺ���
public:
	//��ʼ���ն�
	bool InitializeClient();
	//����״̬
	VOID UpdateServiceStatus(WORD wServiceStatus);	

	//��������
public:
	//���̼��
	bool OnEventProcessMonitor(WORD wSubCmdID, VOID* pData, WORD wDataSize, HWND hWndSend);		

	//��Ϣ�¼�
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif