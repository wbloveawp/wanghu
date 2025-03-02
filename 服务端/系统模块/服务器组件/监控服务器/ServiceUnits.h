#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#pragma once

//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"
#include "..\..\ȫ�ֶ���\RightDefine.h"

//��Ϣ����
#include "..\..\��Ϣ����\CMD_Commom.h"
#include "..\..\��Ϣ����\CMD_CenterServer.h"

//ƽ̨�ļ�
#include "..\..\�������\��������\CommonServiceHead.h"
#include "..\..\���������\������\ServiceFrameHead.h"
#include "..\..\���������\�ں�����\KernelEngineHead.h"

#include "InitParameter.h"
#include "MonitorCenter.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//�����ʾ
#define NETWORK_STATUS				1									//״̬����
#define NETWORK_CENTER				2									//��������

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
	CMonitorCenter					m_MonitorCenter;					//�������
	CMonitorClient					m_MonitorClient;					//����ն�
	CAttemperEngineSink				m_AttemperEngineSink;				//���ȹ���	
	CDataBaseEngineSink				m_DataBaseEngineSink[2];			//���ݹ���

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
	//ֹͣ����
	virtual bool ConcludeService(bool bWillStop);	
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
	//�������ݿ�
	bool StartDataBaseService();
	//�����ܱ�
	bool StartPeripheryService();

	//���ܺ���
public:
	//��ʼ����
	CInitParameter * GetInitParameter() { return &m_InitParameter; }
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