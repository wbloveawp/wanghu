#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#pragma once

//�����ļ�
#include "InitParameter.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//�����ʾ
#define NETWORK_CENTER				1									//��������

//////////////////////////////////////////////////////////////////////////

//����Ԫ
class CServiceUnits : public CWnd, public IMonitorClientSink
{
	//��Ԫ����
	friend class CAttemperEngineSink;
	friend class CDataBaseEngineSink;

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
	CDataBaseEngineSink				m_DataBaseEngineSink[16];			//���ݹ���

	//�ں����
protected:
	CTimerEngineHelper				m_TimerEngine;						//ʱ������
	CAttemperEngineHelper			m_AttemperEngine;					//��������
	CDataBaseEngineHelper			m_DataBaseEngine;					//��������
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
	virtual void SetServiceOption(tagServiceOption &ServiceOption);
	//���ýӿ�
	virtual bool SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink);

	//��������
protected:
	//�������
	bool InitializeService();
	//�����ں�
	bool StartKernelService();
	//��������
	bool StartNetworkService();
	//�������ݿ�
	bool StartDataBaseService();

	//�ڲ�����
private:
	//����״̬
	bool SetServiceStatus(WORD ServiceStatus);
	//���Ϳ���
	bool SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize);

	//���ܺ���
public:
	//��������
	const tagServiceOption * GeServiceOption() const { return &m_ServiceOption; }
	//���ò���
	const CInitParameter * GetInitParameter() const { return &m_InitParameter; }

	//��Ϣӳ��
protected:
	//������Ϣ
	LRESULT OnUIControlRequest(WPARAM wParam, LPARAM lParam);	

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif