#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
CServiceUnits *			CServiceUnits::g_pServiceUnits=NULL;			//����ָ��

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServiceUnits, CWnd)
	ON_MESSAGE(WM_UICONTROL_REQUEST,OnUIControlRequest)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CServiceUnits::CServiceUnits()
{
	//���ñ���
	m_ServiceStatus=SERVICE_STATUS_STOP;
	ZeroMemory(&m_ServiceOption,sizeof(m_ServiceOption));

	//���ýӿ�
	m_pIServiceUnitsSink=NULL;	

	//���ýӿ�
	m_MonitorClient.SetMonitorClientSink(this);

	//��ʼ��
	m_MonitorClient.InitializeClient();

	//���ö���
	ASSERT(g_pServiceUnits==NULL);
	if (g_pServiceUnits==NULL) g_pServiceUnits=this;

	return;
}

//��������
CServiceUnits::~CServiceUnits()
{
	ConcludeService();
}

//��������
bool CServiceUnits::StartService()
{
	//Ч��״̬
	ASSERT(m_ServiceStatus==SERVICE_STATUS_STOP);
	if (m_ServiceStatus!=SERVICE_STATUS_STOP) return false;

	//����״̬
	SetServiceStatus(SERVICE_STATUS_CONFIG);

	//��������
	if (m_hWnd==NULL)
	{
		//��������
		CRect rcCreate(0,0,0,0);
		Create(NULL,NULL,WS_CHILD,rcCreate,AfxGetMainWnd(),100);

		//���ýӿ�
		m_UIControlSink.SetHookWindow(m_hWnd);
	}

	//����ն�
	if (m_MonitorClient.StartService() == false)
	{
		ConcludeService();
		return false;
	}

	//���÷���
	if (InitializeService()==false)
	{
		ConcludeService();
		return false;
	}

	//�����ں�
	if (StartKernelService()==false)
	{
		ConcludeService();
		return false;
	}

	//��Ȩ��֤
	SendControlPacket(CT_CONNECT_CENTER,NULL,0);

	return true;
}

//ֹͣ����
bool CServiceUnits::ConcludeService()
{
	//���ñ���
	SetServiceStatus(SERVICE_STATUS_STOP);

	//ֹͣ����
	m_MonitorClient.ConcludeService();

	//ֹͣ����
	if (m_TimerEngine.GetInterface()!=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_DataBaseEngine.GetInterface()!=NULL) m_DataBaseEngine->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();	
	if (m_TCPSocketCenter.GetInterface()!=NULL) m_TCPSocketCenter->ConcludeService();	

	return true;
}

//���ýӿ�
bool CServiceUnits::SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink)
{
	//���ñ���
	m_pIServiceUnitsSink=pIServiceUnitsSink;

	return true;
}

//�洢����
bool _cdecl CServiceUnits::SetServiceOption(tagServiceOption& ServiceOption)
{
	m_ServiceOption = ServiceOption;

	//���ò���
	m_MonitorClient.SetServiceParameter(SERVICE_MODULE_STORE, m_ServiceOption.wServiceID, m_ServiceOption.szServiceName);

	return true;
}

//�������
bool CServiceUnits::InitializeService()
{
	//���ز���
	m_InitParameter.LoadInitParameter();

	//�������
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false)) return false;
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false)) return false;
	if ((m_DataBaseEngine.GetInterface()==NULL)&&(m_DataBaseEngine.CreateInstance()==false)) return false;
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false)) return false;		
	if ((m_TCPSocketCenter.GetInterface()==NULL)&&(m_TCPSocketCenter.CreateInstance()==false)) return false;

	//����ӿ�
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();

	//���ݽӿ�	
	IUnknownEx * pDataBaseEngineSink=QUERY_OBJECT_INTERFACE(m_DataBaseEngineSink,IUnknownEx);

	//�ص��ӿ�
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//�󶨽ӿ�
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false;
	if (m_DataBaseEngine->SetDataBaseEngineSink(&pDataBaseEngineSink,1)==false) return false;

	//�ں����
	if (m_TimerEngine->SetTimerEngineEvent(pIAttemperEngine)==false) return false;
	if (m_AttemperEngine->SetNetworkEngine(pITCPNetworkEngine)==false) return false;
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return false;

	//�������
	m_TCPSocketCenter->SetServiceID(NETWORK_CENTER);
	if (m_TCPSocketCenter->SetTCPSocketEvent(pIAttemperEngine)==false) return false;	

	//���Ȼص�
	m_AttemperEngineSink.m_pServiceOption=&m_ServiceOption;
	m_AttemperEngineSink.m_pInitParameter=&m_InitParameter;
	m_AttemperEngineSink.m_pITimerEngine=m_TimerEngine.GetInterface();
	m_AttemperEngineSink.m_pIDataBaseEngine=m_DataBaseEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();	
	m_AttemperEngineSink.m_pITCPSocketCenter=m_TCPSocketCenter.GetInterface();

	//���ݿ�ص�
	m_DataBaseEngineSink.m_pInitParameter=&m_InitParameter;
	m_DataBaseEngineSink.m_pIDataBaseEngineEvent=QUERY_OBJECT_PTR_INTERFACE(pIAttemperEngine,IDataBaseEngineEvent);

	//��������
	WORD wMaxConnect= m_ServiceOption.wMaxConnect;
	WORD wServicePort= m_ServiceOption.wServicePort;
	if (m_TCPNetworkEngine->SetServiceParameter(m_ServiceOption.wServicePort,wMaxConnect, szCompilation)==false) return false;

	return true;
}

//�����ں�
bool CServiceUnits::StartKernelService()
{
	//ʱ������
	if (m_TimerEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_AttemperEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//��������
	if (m_TCPSocketCenter->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}	

	return true;
}

//�����ܱ�
bool CServiceUnits::StartPeripheryService()
{
	//��������
	if (m_TCPNetworkEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//�������ݿ�
	if (m_DataBaseEngine->StartService() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//����״̬
bool CServiceUnits::SetServiceStatus(WORD ServiceStatus)
{
	//״̬�ж�
	if (m_ServiceStatus!=ServiceStatus)
	{
		//����֪ͨ
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(ServiceStatus==SERVICE_STATUS_STOP))
		{
			CTraceService::TraceString(TEXT("��������ʧ��"),TraceLevel_Exception);
		}

		//���ñ���
		m_ServiceStatus=ServiceStatus;

		//����״̬
		m_MonitorClient.UpdateServiceStatus(ServiceStatus);

		//״̬֪ͨ
		ASSERT(m_pIServiceUnitsSink!=NULL);
		if (m_pIServiceUnitsSink!=NULL) m_pIServiceUnitsSink->OnServiceUnitsStatus(m_ServiceStatus);
	}

	return true;
}

//���Ϳ���
bool CServiceUnits::SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize)
{
	//״̬Ч��
	ASSERT(m_AttemperEngine.GetInterface()!=NULL);
	if (m_AttemperEngine.GetInterface()==NULL) return false;

	//���Ϳ���
	m_AttemperEngine->OnEventControl(wControlID,pData,wDataSize);

	return true;
}

//������Ϣ
LRESULT CServiceUnits::OnUIControlRequest(WPARAM wParam, LPARAM lParam)
{
	//��ȡ����
	tagDataHead DataHead;
	VOID * pData=m_UIControlSink.GetControlRequest(DataHead);

	//���ݴ���
	switch (DataHead.dwIdentifier)
	{	
	case UI_CENTER_RESULT:		//���ĳɹ�
		{
			//Ч����Ϣ
			ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
			if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

			//��������
			CP_ControlResult * pControlResult=(CP_ControlResult *)pData;

			//ʧ�ܴ���
			if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_FAILURE))
			{
				ConcludeService();
				return 0;
			}

			//�ɹ�����
			if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_SUCCESS))
			{				
				//��������
				if (StartPeripheryService()==false)
				{
					ConcludeService();
					return 0;
				}

				//����״̬
				SetServiceStatus(SERVICE_STATUS_SERVICE);
			}

			return 0;
		}		
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
