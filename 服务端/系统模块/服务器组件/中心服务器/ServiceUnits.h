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
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"


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
	CInitParameter					m_InitParameter;					//���ò���	

	//�������
protected:
	CUIControlSink					m_UIControlSink;					//�������
	CAttemperEngineSink				m_AttemperEngineSink;				//���ȹ���
	CDataBaseEngineSink				m_DataBaseEngineSink;				//���ݹ���

	//�ں����
protected:
	CTimerEngineHelper				m_TimerEngine;						//ʱ������
	CAttemperEngineHelper			m_AttemperEngine;					//��������
	CDataBaseEngineHelper			m_DataBaseEngine;					//��������
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//��������

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
	//���Է���
	virtual bool DebugService();
	//ֹͣ����
	virtual bool ConcludeService(bool bWillStop=false);
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