#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//静态变量
CServiceUnits *	CServiceUnits::g_pServiceUnits=NULL;			//对象指针

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CServiceUnits, CWnd)
	ON_MESSAGE(WM_UICONTROL_REQUEST,OnUIControlRequest)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CServiceUnits::CServiceUnits()
{
	//设置变量
	m_ServiceStatus=SERVICE_STATUS_STOP;
	ZeroMemory(&m_ServiceOption,sizeof(m_ServiceOption));

	//设置接口
	m_pIServiceUnitsSink=NULL;

	//设置接口
	m_MonitorClient.SetMonitorClientSink(this);

	//初始化
	m_MonitorClient.InitializeClient();

	//设置对象
	ASSERT(g_pServiceUnits==NULL);
	if (g_pServiceUnits==NULL) g_pServiceUnits=this;

	return;
}

//析构函数
CServiceUnits::~CServiceUnits()
{
	//停止服务
	ConcludeService();

	return;
}

//启动服务
bool CServiceUnits::StartService()
{
	//效验状态
	ASSERT(m_ServiceStatus==SERVICE_STATUS_STOP);
	if (m_ServiceStatus!=SERVICE_STATUS_STOP) return false;

	//设置状态
	SetServiceStatus(SERVICE_STATUS_CONFIG);

	//创建窗口
	if (m_hWnd==NULL)
	{
		//创建窗口
		CRect rcCreate(0,0,0,0);
		Create(NULL,NULL,WS_CHILD,rcCreate,AfxGetMainWnd(),100);

		//设置接口
		m_UIControlSink.SetHookWindow(m_hWnd);
	}

	//监控终端
	if (m_MonitorClient.StartService() == false)
	{
		ConcludeService();
		return false;
	}

	//配置服务
	if (InitializeService()==false)
	{
		ConcludeService();
		return false;
	}

	//启动内核
	if (StartKernelService()==false)
	{
		ConcludeService();
		return false;
	}

	//授权验证
	SendControlPacket(CT_CONNECT_CENTER,NULL,0);

	return true;
}

//停止服务
bool CServiceUnits::ConcludeService()
{
	//设置变量
	SetServiceStatus(SERVICE_STATUS_STOP);

	//停止服务
	m_MonitorClient.ConcludeService();

	//停止服务
	if (m_TimerEngine.GetInterface()!=NULL) m_TimerEngine->ConcludeService();
	if (m_AttemperEngine.GetInterface()!=NULL) m_AttemperEngine->ConcludeService();
	if (m_TCPNetworkEngine.GetInterface()!=NULL) m_TCPNetworkEngine->ConcludeService();
	if (m_TCPSocketCenter.GetInterface()!=NULL) m_TCPSocketCenter->ConcludeService();

	return true;
}

//设置配置
void CServiceUnits::SetServiceOption(tagServiceOption &ServiceOption)
{
	//拷贝配置
	m_ServiceOption=ServiceOption;

	//设置参数
	m_MonitorClient.SetServiceParameter(SERVICE_MODULE_GATE, m_ServiceOption.wServiceID, m_ServiceOption.szServiceName);
}

//设置接口
bool CServiceUnits::SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink)
{
	//设置变量
	m_pIServiceUnitsSink=pIServiceUnitsSink;

	return true;
}

//配置组件
bool CServiceUnits::InitializeService()
{
	//加载参数
	m_InitParameter.LoadInitParameter();

	//创建组件
	if ((m_TimerEngine.GetInterface()==NULL)&&(m_TimerEngine.CreateInstance()==false)) return false;
	if ((m_AttemperEngine.GetInterface()==NULL)&&(m_AttemperEngine.CreateInstance()==false)) return false;
	if ((m_TCPNetworkEngine.GetInterface()==NULL)&&(m_TCPNetworkEngine.CreateInstance()==false)) return false;
	if ((m_TCPSocketCenter.GetInterface()==NULL)&&(m_TCPSocketCenter.CreateInstance()==false)) return false;

	//组件接口
	IUnknownEx * pIAttemperEngine=m_AttemperEngine.GetInterface();
	IUnknownEx * pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	IUnknownEx * pIAttemperEngineSink=QUERY_OBJECT_INTERFACE(m_AttemperEngineSink,IUnknownEx);

	//绑定接口
	if (m_AttemperEngine->SetAttemperEngineSink(pIAttemperEngineSink)==false) return false;

	//内核组件
	if (m_TimerEngine->SetTimerEngineEvent(pIAttemperEngine)==false) return false;
	if (m_AttemperEngine->SetNetworkEngine(pITCPNetworkEngine)==false) return false;
	if (m_TCPNetworkEngine->SetTCPNetworkEngineEvent(pIAttemperEngine)==false) return false;

	//网络服务
	m_TCPSocketCenter->SetServiceID(NETWORK_CENTER);
	if (m_TCPSocketCenter->SetTCPSocketEvent(pIAttemperEngine)==false) return false;

	//调度回调
	m_AttemperEngineSink.m_pServiceOption=&m_ServiceOption;
	m_AttemperEngineSink.m_pInitParameter=&m_InitParameter;
	m_AttemperEngineSink.m_pITimerEngine=m_TimerEngine.GetInterface();
	m_AttemperEngineSink.m_pIAttemperEngine=m_AttemperEngine.GetInterface();
	m_AttemperEngineSink.m_pITCPNetworkEngine=m_TCPNetworkEngine.GetInterface();
	m_AttemperEngineSink.m_TCPSocketCenter=m_TCPSocketCenter.GetInterface();

	//配置网络
	WORD wMaxConnect=m_ServiceOption.wMaxConnect;
	WORD wServicePort=m_ServiceOption.wServicePort;
	if (m_TCPNetworkEngine->SetServiceParameter(wServicePort,wMaxConnect,szCompilation)==false) return false;

	return true;
}

//启动内核
bool CServiceUnits::StartKernelService()
{
	//时间引擎
	if (m_TimerEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//调度引擎
	if (m_AttemperEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	//网络引擎
	if (m_TCPSocketCenter->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//启动网络
bool CServiceUnits::StartNetworkService()
{
	//网络引擎
	if (m_TCPNetworkEngine->StartService()==false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//设置状态
bool CServiceUnits::SetServiceStatus(WORD ServiceStatus)
{
	//状态判断
	if (m_ServiceStatus!=ServiceStatus)
	{
		//错误通知
		if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(ServiceStatus==SERVICE_STATUS_STOP))
		{
			CTraceService::TraceString(TEXT("服务启动失败"),TraceLevel_Exception);
		}

		//设置变量
		m_ServiceStatus=ServiceStatus;

		//更新状态
		m_MonitorClient.UpdateServiceStatus(ServiceStatus);

		//状态通知
		ASSERT(m_pIServiceUnitsSink!=NULL);
		if (m_pIServiceUnitsSink!=NULL) m_pIServiceUnitsSink->OnServiceUnitsStatus(m_ServiceStatus);
	}

	return true;
}

//发送控制
bool CServiceUnits::SendControlPacket(WORD wControlID, VOID * pData, WORD wDataSize)
{
	//状态效验
	ASSERT(m_AttemperEngine.GetInterface()!=NULL);
	if (m_AttemperEngine.GetInterface()==NULL) return false;

	//发送控制
	m_AttemperEngine->OnEventControl(wControlID,pData,wDataSize);

	return true;
}

//控制消息
LRESULT CServiceUnits::OnUIControlRequest(WPARAM wParam, LPARAM lParam)
{
	//获取数据
	tagDataHead DataHead;
	VOID * pData=m_UIControlSink.GetControlRequest(DataHead);

	//数据处理
	switch (DataHead.dwIdentifier)
	{
	case UI_CENTER_RESULT :		//中心成功
		{
			//效验消息
			ASSERT(DataHead.wDataSize==sizeof(CP_ControlResult));
			if (DataHead.wDataSize!=sizeof(CP_ControlResult)) return 0;

			//变量定义
			CP_ControlResult * pControlResult=(CP_ControlResult *)pData;

			//失败处理
			if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_FAILURE))
			{
				ConcludeService();
				return 0;
			}

			//成功处理
			if ((m_ServiceStatus!=SERVICE_STATUS_SERVICE)&&(pControlResult->cbSuccess==ER_SUCCESS))
			{
				//启动网络
				if (StartNetworkService()==false)
				{
					ConcludeService();
					return 0;
				}

				//设置状态
				SetServiceStatus(SERVICE_STATUS_SERVICE);
			}

			return 0;
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////
