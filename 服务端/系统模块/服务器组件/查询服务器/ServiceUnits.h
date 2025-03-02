#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#pragma once

//ƽ̨����
#include "InitParameter.h"
#include "AttemperEngineSink.h"
#include "DataCacheEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//�����ʾ
#define NETWORK_CENTER				1									//��������

//////////////////////////////////////////////////////////////////////////

//����Ԫ
class CServiceUnits : public CWnd, public IMonitorClientSink
{
	//��Ԫ����
	friend class CAttemperEngineSink;
	friend class CDataCacheEngineSink;

	//�������
private:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//��������	

	//״̬����
protected:
	WORD							m_ServiceStatus;					//����״̬
	tagServiceOption				m_ServiceOption;					//��������
	CInitParameter					m_InitParameter;					//���ò���	

	//�������
protected:
	CUIControlSink					m_UIControlSink;					//�������
	CMonitorClient					m_MonitorClient;					//����ն�
	CAttemperEngineSink				m_AttemperEngineSink;				//���ȹ���		
	CDataCacheEngineSink			m_DataCacheEngineSink[12];			//���ݹ���

	//�ں����
protected:
	CTimerEngineHelper				m_TimerEngine;						//ʱ������
	CAttemperEngineHelper			m_AttemperEngine;					//��������	
	CDataBaseEngineHelper			m_DataCacheEngine;					//��������
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//��������
	CTCPSocketServiceHelper			m_TCPSocketCenter;					//���ķ���	

	//�ӿڱ���
protected:
	IServiceUnitsSink *				m_pIServiceUnitsSink;				//״̬�ӿ�

	//��̬����
public:
	static CServiceUnits *			g_pServiceUnits;					//����ָ��

	//��������
public:
	//���캯��
	CServiceUnits();
	//��������
	virtual ~CServiceUnits();

	//��Ϣ����
public:
	//��ȡ״̬
	virtual WORD GetServiceStatus() { return m_ServiceStatus; }

	//�������
public:
	//��������
	virtual bool StartService();
	//ֹͣ����
	virtual bool ConcludeService();
	//��������
	virtual void SetServiceOption(tagServiceOption& ServiceOption);
	//���ýӿ�
	virtual bool SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink);

	//��������
protected:
	//�������
	bool InitializeService();
	//�����ں�
	bool StartKernelService();
	//�������ݿ�
	bool StartDataBaseService();
	//�����ܱ�
	bool StartPeripheryService();

	//���ܺ���
public:
	//��ʼ����
	const CInitParameter * GetInitParameter() const { return &m_InitParameter; }
	//��������
	const tagServiceOption * GetServiceOption() const { return &m_ServiceOption; }	

	//�ڲ�����
private:
	//����״̬
	bool SetServiceStatus(WORD ServiceStatus);
	//���Ϳ���
	bool SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize);

	//��Ϣӳ��
protected:
	//������Ϣ
	LRESULT OnUIControlRequest(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif