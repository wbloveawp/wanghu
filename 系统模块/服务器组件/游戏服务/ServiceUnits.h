#ifndef SERVICE_UNITS_HEAD_FILE
#define SERVICE_UNITS_HEAD_FILE

#pragma once

#include "InitParameter.h"
#include "GameServiceHead.h"
#include "AttemperEngineSink.h"
#include "DataBaseEngineSink.h"
#include "GlobalUnits.h"

//////////////////////////////////////////////////////////////////////////

//�����ʾ
#define NETWORK_STORE				1									//�洢����
#define NETWORK_SERVICE				2									//��������
#define NETWORK_CENTER				4									//��������

//////////////////////////////////////////////////////////////////////////

//����Ԫ
class CServiceUnits : public CWnd, public IGameServiceUnits, public IMonitorClientSink
{
	//��Ԫ����
	friend class CAttemperEngineSink;

	//״̬����
protected:
	bool							m_bForceClose;						//ǿ�ƹر�
	WORD							m_ServiceStatus;					//����״̬
	tagGameServiceAttrib			m_GameServiceAttrib;				//��������
	tagGameServiceOption			m_GameServiceOption;				//��������	

	//�������
protected:
	CInitParameter					m_InitParameter;					//���ò���
	CMonitorClient					m_MonitorClient;					//����ն�
	CUIControlSink					m_UIControlSink;					//�������
	CAttemperEngineSink				m_AttemperEngineSink;				//���ȹ���
	CDataBaseEngineSink				m_DataBaseEngineSink[16];			//���ݹ���	

	//�ں�����
protected:
	CTimerEngineHelper				m_TimerEngine;						//ʱ������	
	CAttemperEngineHelper			m_AttemperEngine;					//��������
	CTCPNetworkEngineHelper			m_TCPNetworkEngine;					//��������
	CGameServiceManagerHelper		m_GameServiceManager;				//��Ϸģ��
	
	//��������
protected:
	CTCPSocketServiceHelper			m_TCPSocketStore;					//�洢����
	CTCPSocketServiceHelper			m_TCPSocketCenter;					//���ķ���	
	CTCPSocketServiceHelper			m_TCPSocketService;					//���ͷ���		

	//���ݿ�����
protected:
	CDataBaseEngineHelper			m_DataBaseEngine;					//��������	

	//�ӿڱ���
protected:
	IServiceUnitsSink *				m_pIServiceUnitsSink;				//״̬�ӿ�

	//����ģ��
protected:
#ifdef VIDEO_MODULE_HELPER	
	CGameVideoManagerHelper			m_GameVideoManager;					//¼�����	
#endif
#ifdef MATCH_MODULE_HELPER		
	CMatchServiceManagerHelper		m_MatchServiceManager;				//��������
#endif	
#ifdef BATTLE_MODULE_HELPER		
	CBattleServiceManagerHelper		m_BattleServiceManager;				//Լս����
#endif	
#ifdef RANKING_MODULE_HELPER		
	CRankingServiceManagerHelper	m_RankingServiceManager;			//��λ����
#endif

	//��̬����
public:
	static CServiceUnits *			g_pServiceUnits;					//����ָ��

	//��������
public:
	//���캯��
	CServiceUnits();
	//��������
	virtual ~CServiceUnits();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

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
	virtual bool ConcludeService();	
	//���ýӿ�
	virtual bool SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink);
	//��Ϸ����
	virtual bool SetServiceCollocate(LPCTSTR pszGameModule, const tagGameServiceOption & GameServiceOption);

	//��������
protected:
	//����ģ��
	bool CreateServiceDLL();
	//����ģ��
	bool InitializeService();
	//��������
	bool RectifyServiceParameter();

	//��������
protected:
	//�����ں�
	bool StartKernelService();
	//��������
	bool StartSocketService();
	//��������
	bool StartExtendService();
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

	//��Ϣӳ��
protected:
	//������Ϣ
	LRESULT OnUIControlRequest(WPARAM wParam, LPARAM lParam);

	//��Ϣ����
public:
	//��ʱ����Ϣ
	afx_msg void OnTimer(UINT_PTR nIDEvent);

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////

#endif