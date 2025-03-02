#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#pragma once

//ƽ̨����
#include "InitParameter.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//�����ʾ
#define NETWORK_CENTER				1									//��������
#define NETWORK_BATTLE				2									//Լս����

//////////////////////////////////////////////////////////////////////////

//����Ԫ
class CServiceUnits : public CWnd
{
	//��Ԫ����
	friend class CAttemperEngineSink;
	friend class CDataBaseEngineSink;

	//״̬����
protected:	
	WORD							m_ServiceStatus;					//����״̬
	tagServiceOption				m_ClubOption;						//�߼�����
	CInitParameter					m_InitParameter;					//���ò���	

	//�������
protected:
	CUIControlSink					m_UIControlSink;					//�������
	CAttemperEngineSink				m_AttemperEngineSink;				//���ȹ���	
	CDataBaseEngineSink				m_DataBaseEngineSink[4];			//���ݹ���

	//�ں����
protected:
	CTimerEngineHelper				m_TimerEngine;						//ʱ������
	CAttemperEngineHelper			m_AttemperEngine;					//��������	
	CDataBaseEngineHelper			m_DataBaseEngine;					//��������
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//��������
	CTCPSocketServiceHelper			m_TCPSocketCenter;					//���ķ���
	CTCPSocketServiceHelper			m_TCPSocketBattle;					//Լս����

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
	virtual void SetServiceOption(tagServiceOption &ClubOption);
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
	//��������
	tagServiceOption * GetServiceOption() { return &m_ClubOption; }
	//��ʼ����
	CInitParameter * GetInitParameter() { return &m_InitParameter; }

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