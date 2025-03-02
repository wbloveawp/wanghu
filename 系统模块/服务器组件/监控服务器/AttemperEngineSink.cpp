#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "MonitorCenter.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_CONNECT_CENTER			1									//重连标识
#define IDI_CONNECT_VALIDATE		2									//重连标识
#define IDI_MONITOR_SERVICE_PULSE	4									//服务脉冲
#define IDI_REPORT_JACKPOT_SCORE	5									//上报彩金

//时间定义
#define TIME_MONITOR_SERVICE_PULSE	1L									//服务脉冲
#define TIME_REPORT_JACKPOT_SCORE	15L									//上报彩金

//////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//状态变量		
	m_StockUpdateID = 0;
	m_bMonitorService=false;	
	m_bWillConcludeService=false;

	//设置变量
	m_pServiceOption=NULL;
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;	
	ZeroMemory(&m_MonitorItem,sizeof(m_MonitorItem));

	//组件变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;

	//初始化
	m_StockGameKind.InitHashTable(31);

	return;
}

//析构函数
CAttemperEngineSink::~CAttemperEngineSink()
{
}

//接口查询
VOID * CAttemperEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IAttemperEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IAttemperEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CAttemperEngineSink::OnAttemperEngineStart(IUnknownEx * pIUnknownEx)
{
	//绑定参数
	m_pBindParameter=new tagBindParameter[m_pServiceOption->wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pServiceOption->wMaxConnect);

	//设置标识
	m_StockUpdateID = 0;
	m_bMonitorService=true;
	m_bWillConcludeService=false;

	//初始化
	m_ShareStockManager.StartStockManager(this);

	//设置时钟
	ASSERT(m_pITimerEngine != NULL);
	m_pITimerEngine->SetTimer(IDI_MONITOR_SERVICE_PULSE, TIME_MONITOR_SERVICE_PULSE*1000L, TIMES_INFINITY, 0);

	//设置时钟
	ASSERT(m_pITimerEngine != NULL);
	m_pITimerEngine->SetTimer(IDI_REPORT_JACKPOT_SCORE, TIME_REPORT_JACKPOT_SCORE * 1000L, TIMES_INFINITY, 0);	

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//重置对象
	if (!m_ShareStockManager.CloseStockManager())
	{
		return false;
	}

	//移除子项
	m_StockGameKind.RemoveAllItem();

	//状态变量		
	m_StockUpdateID = 0;
	m_bMonitorService=false;
	m_bWillConcludeService=false;
	m_StockUpdateArray.RemoveAll();

	//设置变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;

	//删除数据
	SafeDeleteArray(m_pBindParameter);

	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	switch (dwIdentifier)
	{
	case CT_LOAD_GLOBAL_CONFIG:			//加载配置
	{
		//提示消息
		CTraceService::TraceString(TEXT("正在加载系统配置..."),TraceLevel_Normal);

		//构造结构
		DBR_LoadGlobalConfig LoadGlobalConfig;
		ZeroMemory(&LoadGlobalConfig,sizeof(LoadGlobalConfig));

		//设置掩码
		LoadGlobalConfig.dwConfigMask = 0;

		//加载配置
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_LOAD_GLOBAL_CONFIG, 0, 0, 0, &LoadGlobalConfig, sizeof(LoadGlobalConfig));

		return true;
	}
	case CT_CONNECT_CENTER:				//连接中心
	{
		//设置地址
		DWORD dwCenterServer=m_pInitParameter->m_CenterAddress.dwServiceAddr;

		//发起连接
		m_pITCPSocketCenter->Connect(htonl(dwCenterServer), m_pInitParameter->m_wCenterPort);

		//提示消息
		CTraceService::TraceString(TEXT("正在尝试连接中心服务器..."),TraceLevel_Normal);

		return true;
	}
	case CT_SERVICE_WILLSTOP:			//即将停止
	{
		//状态判断
		if (m_bWillConcludeService) return true;

		//关闭判断
		if (!m_ShareStockManager.CloseStockManager())
		{
			//提示信息
			CTraceService::TraceString(TEXT("请先关闭本地其他游戏服务器再停止服务！"), TraceLevel_Warning);

			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess = ER_FAILURE;

			//发送数据
			CServiceUnits* pServiceUnits = CServiceUnits::g_pServiceUnits;
			pServiceUnits->SendControlPacket(UI_SERVICE_WILLSTOP_RESULT, &ControlResult, sizeof(ControlResult));

			return true;
		}

		//设置状态
		m_bWillConcludeService = true;

		//判断状态
		if (m_StockUpdateArray.GetCount() == 0)
		{
			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess = ER_SUCCESS;
			SendUIControlPacket(UI_SERVICE_WILLSTOP_RESULT, &ControlResult, sizeof(ControlResult));
		}

		return true;
	}
	case FT_FORWARD_TO_CENTER:			//转发数据
	{
		//提取数据
		auto pForwardToCenter = (FP_ForwardToCenter*)pData;

		//发送数据
		m_pITCPSocketCenter->SendData(pForwardToCenter->wMainCmdID, pForwardToCenter->wSubCmdID, pForwardToCenter->cbDataBuffer, pForwardToCenter->wDataSize,0);

		return true;
	}
	}

	return false;
}

//调度事件
bool CAttemperEngineSink::OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	return false;
}

//时间事件
bool CAttemperEngineSink::OnEventTimer(DWORD dwTimerID, WPARAM wBindParam)
{
	switch (dwTimerID)
	{
	case IDI_CONNECT_CENTER:		//连接中心
	{
		//设置地址
		DWORD dwCenterServer = m_pInitParameter->m_CenterAddress.dwServiceAddr;

		//发起连接
		m_pITCPSocketCenter->Connect(htonl(dwCenterServer), m_pInitParameter->m_wCenterPort);

		//提示消息
		CTraceService::TraceString(TEXT("正在尝试连接中心服务器..."),TraceLevel_Normal);

		return true;
	}
	case IDI_MONITOR_SERVICE_PULSE:	 //服务脉冲
	{
		//时钟调度
		m_ShareStockManager.OnEventTimerPulse();			

		return true;
	}
	case IDI_REPORT_JACKPOT_SCORE:	//上报彩金
	{
		//上报彩金
		//ReportGameJackpot();		

		return true;
	}
	}

	return false;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//获取索引
	ASSERT(LOWORD(dwSocketID)< m_pServiceOption->wMaxConnect);
	if (LOWORD(dwSocketID)>= m_pServiceOption->wMaxConnect) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//设置变量
	pBindParameter->dwSocketID=dwSocketID;
	pBindParameter->dwClientIP=dwClientIP;
	pBindParameter->dwActiveTime=(DWORD)time(NULL);	

	return true;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//提取命令码
	WORD wMainCmdID = LOCMMD(Command.wMainCmdID);

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//清除信息
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//库存预警
VOID CAttemperEngineSink::OnEventStockScoreAlarm(tagStockAlarmItem StockAlarmList[], WORD wItemCount)
{
	//缓存定义
	BYTE cbBuffer[2048] = {0};

	//获取对象
	auto pForwardHead = (tagForwardHead*)cbBuffer;
	ZeroMemory(pForwardHead, sizeof(tagForwardHead));

	//转发对象
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;

	//转发子项
	pForwardHead->wItemCount = 1;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;

	//构造对象
	auto pSendExceptionAlarm = (CMD_CS_SendExceptionAlarm*)(pForwardHead + 1);

	//预警标题
	StringCchCopyA(pSendExceptionAlarm->szAlarmTitle, CountArray(pSendExceptionAlarm->szAlarmTitle), "Game Stock Alarm");

	for (int i = 0; i < wItemCount; i++)
	{
		//查找类型
		auto pGameKind = m_StockGameKind.SearchStockItem(StockAlarmList[i].wKindID);
		if (pGameKind != NULL)
		{
			StringCchCopy(pSendExceptionAlarm->szServiceName,CountArray(pSendExceptionAlarm->szServiceName), pGameKind->szKindName);
		}

		//构造预警信息		
		_sntprintf_s(pSendExceptionAlarm->szExceptionInfo, CountArray(pSendExceptionAlarm->szExceptionInfo), CountArray(pSendExceptionAlarm->szExceptionInfo), TEXT("游戏库存出现负分：StockID=%d,BetAmount=%0.2f,StockScore=%0.2f 服务器IP: %s, 平台名称: %s"),
			StockAlarmList[i].wStockID, StockAlarmList[i].lBetAmount / (RATIO_VALUE * 1.f), StockAlarmList[i].lStockScore / (RATIO_VALUE * 1.f), m_pServiceOption->szServiceDomain, m_pInitParameter->m_szServerName);

		//发送数据				
		WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_SendExceptionAlarm);
		m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_SEND_EXCEPTION_ALARM, cbBuffer, wPacketSize, 0);		
	}
}

//彩池预警
VOID CAttemperEngineSink::OnEventJackpotScoreAlarm(tagJackpotAlarmItem JackpotAlarmList[], WORD wItemCount)
{
	//缓存定义
	BYTE cbBuffer[2048] = { 0 };

	//获取对象
	auto pForwardHead = (tagForwardHead*)cbBuffer;
	ZeroMemory(pForwardHead, sizeof(tagForwardHead));

	//转发对象
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;

	//转发子项
	pForwardHead->wItemCount = 1;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;

	//构造对象
	auto pSendExceptionAlarm = (CMD_CS_SendExceptionAlarm*)(pForwardHead + 1);

	//预警标题
	StringCchCopyA(pSendExceptionAlarm->szAlarmTitle, CountArray(pSendExceptionAlarm->szAlarmTitle), "Game Jackpot Alarm");

	for (int i = 0; i < wItemCount; i++)
	{	
		//查找类型
		auto pGameKind = m_StockGameKind.SearchStockItem(JackpotAlarmList[i].wKindID);
		if (pGameKind != NULL)
		{
			StringCchCopy(pSendExceptionAlarm->szServiceName, CountArray(pSendExceptionAlarm->szServiceName), pGameKind->szKindName);
		}

		//构造预警信息
		_sntprintf_s(pSendExceptionAlarm->szExceptionInfo, CountArray(pSendExceptionAlarm->szExceptionInfo), CountArray(pSendExceptionAlarm->szExceptionInfo), TEXT("游戏彩池出现负分：StockID=%d,BetAmount=%0.2f,LevelID=%d,StockScore=%0.3f 服务器IP: %s, 平台名称: %s"),
			JackpotAlarmList[i].wStockID, JackpotAlarmList[i].lBetAmount/ (RATIO_VALUE * 1.f), JackpotAlarmList[i].wLevelID, JackpotAlarmList[i].lStockScore / (RATIO_VALUE * 1.f), m_pServiceOption->szServiceDomain,m_pInitParameter->m_szServerName);

		//发送数据				
		WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_SendExceptionAlarm);
		m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_SEND_EXCEPTION_ALARM, cbBuffer, wPacketSize, 0);
	}
}

//库存更新
VOID CAttemperEngineSink::OnEventStockScoreUpdate(tagStockVariationItem StockVariationList[], WORD wItemCount)
{
	//变量定义
	int nStepSize = 60;

	//分包处理
	for (int i = 0; i < wItemCount; i += nStepSize)
	{
		//记录标识
		if (m_bWillConcludeService) m_StockUpdateArray.Add(m_StockUpdateID);

		//计算数量
		auto nItemCount = (i + nStepSize) <= wItemCount ? nStepSize : wItemCount - i;

		//投递数据
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_UPDATE_STOCK_SCORE, 0L, 0L, m_StockUpdateID++, &StockVariationList[i], nItemCount * sizeof(StockVariationList[0]));
	}	
}

//状态更新
VOID CAttemperEngineSink::OnEventStockStateUpdate(tagStateVariationItem StateVariationList[], WORD wItemCount)
{
	//变量定义
	int nStepSize = 70;

	//分包处理
	for (int i = 0; i < wItemCount; i += nStepSize)
	{
		//记录标识
		if (m_bWillConcludeService) m_StockUpdateArray.Add(m_StockUpdateID);

		//计算数量
		auto nItemCount = (i + nStepSize) <= wItemCount ? nStepSize : wItemCount - i;

		//投递数据
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_UPDATE_STOCK_STATE, 0L, 0L, m_StockUpdateID++, &StateVariationList[i], nItemCount * sizeof(StateVariationList[0]));
	}
}

//彩池更新
VOID CAttemperEngineSink::OnEventJackpotScoreUpdate(tagJackpotVariationItem JackpotVariationList[], WORD wItemCount)
{
	//变量定义
	int nStepSize = 60;

	//分包处理
	for (int i = 0; i < wItemCount; i += nStepSize)
	{
		//记录标识
		if (m_bWillConcludeService) m_StockUpdateArray.Add(m_StockUpdateID);

		//计算数量
		auto nItemCount = (i + nStepSize) <= wItemCount ? nStepSize : wItemCount - i;

		//投递数据
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_UPDATE_JACKPOT_SCORE, 0L, 0L, m_StockUpdateID++, &JackpotVariationList[i], nItemCount * sizeof(JackpotVariationList[0]));
	}
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	switch (wRequestID)
	{
	case DBO_GAME_KIND_ITEM:		//游戏类型
	{
		return OnDBResultGameKindItem(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_STOCK_LIST_ITEM:		//库存子项
	{
		return OnDBResultStockListItem(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_SUPER_PLAYER_CONTROL:	//大R控制
	{
		return OnDBResultSuperPlayerControl(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_LOAD_CONFIG_FINISH:	//加载完成
	{
		return OnDBResultLoadConfigFinish(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_QUERY_STOCK_ITEM:		//查询库存
	{
		return OnDBResultQueryStockItem(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_UPDATE_STOCK_SCORE:	//更新库存
	{
		return OnDBResultUpdateStockScore(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_UPDATE_STOCK_STATE:	//更新状态
	{
		return OnDBResultUpdateStockState(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_UPDATE_JACKPOT_SCORE:	//更新彩金
	{
		return OnDBResultUpdateJackpotScore(dwContextID, dwTokenID, pData, wDataSize);
	}

	}
	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason)
{
	switch (dwServiceID)
	{
	case NETWORK_CENTER:	//中心连接
		{
			//提示消息
			CTraceService::TraceString(TEXT("与中心服务器的连接关闭了，%ld 秒后将重新连接"),TraceLevel_Warning, m_pInitParameter->m_wConnectTime);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return true;
		}
	}

	return false;
}

//连接事件
bool CAttemperEngineSink::OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode)
{
	//中心连接
	if (dwServiceID==NETWORK_CENTER)
	{
		//错误判断
		if (nErrorCode!=0)
		{
			//提示消息
			CTraceService::TraceString(TEXT("中心服务器连接失败 [ %ld ]，%ld 秒后将重新连接"),TraceLevel_Warning, nErrorCode, m_pInitParameter->m_wConnectTime);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L, TIMES_INFINITY, 0);

			return false;
		}

		//变量定义
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService,sizeof(RegisterService));

		//构造结构
		RegisterService.wServiceMask = SERVICE_MASK_GAME;
		RegisterService.wServiceID= m_pServiceOption->wServiceID;
		RegisterService.wServiceModule = SERVICE_MODULE_MONITOR;		
		RegisterService.wServicePort= m_pServiceOption->wServicePort;	
		RegisterService.wServiceKindID = m_pServiceOption->wServiceKindID;
		StringCchCopy(RegisterService.szServiceName, CountArray(RegisterService.szServiceName), TEXT("监控服务器"));
		StringCchCopy(RegisterService.szServiceDomain, CountArray(RegisterService.szServiceDomain), m_pServiceOption->szServiceDomain);

		//发送数据
		m_pITCPSocketCenter->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_SERVICE,&RegisterService,sizeof(RegisterService),0);

		return true;
	}	

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	//中心连接
	if (dwServiceID ==NETWORK_CENTER)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:			//注册服务
			{
				return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_COMMON_SERVICE:		//服务信息
			{
				return OnTCPSocketMainCommonService(Command.wSubCmdID,pData,wDataSize);
			}
		case MDM_CS_FORWARD_SERVICE:	//转发服务
			{
				return OnTCPSocketMainForwardService(Command.wSubCmdID, pData, wDataSize);
			}
		case MDM_CS_MANAGER_SERVICE:	//管理服务
			{
				return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
			}
		}
	}

	//错误断言
	ASSERT(FALSE);

	return true;
}

//注册事件
bool CAttemperEngineSink::OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_REGISTER_SUCCESS:		//注册成功
		{
			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_SUCCESS;
			SendUIControlPacket(UI_CENTER_RESULT,&ControlResult,sizeof(ControlResult));	

			//时钟调度
			m_ShareStockManager.OnEventTimerPulse();

			//上报彩金
			//ReportGameJackpot();

			return true;
		}
	case SUB_CS_S_REGISTER_FAILURE:		//注册失败
		{
			//效验参数
			CMD_CS_S_RegisterFailure * pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;
			ASSERT(wDataSize>=(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString)));
			if (wDataSize<(sizeof(CMD_CS_S_RegisterFailure)-sizeof(pRegisterFailure->szDescribeString))) return false;

			//关闭处理
			m_pITCPSocketCenter->CloseSocket();

			//显示消息
			CW2CT strDescribeString(pRegisterFailure->szDescribeString);
			if (lstrlen(strDescribeString)>0) CTraceService::TraceString(strDescribeString,TraceLevel_Exception);

			//事件通知
			CP_ControlResult ControlResult;
			ControlResult.cbSuccess=ER_FAILURE;
			SendUIControlPacket(UI_CENTER_RESULT,&ControlResult,sizeof(ControlResult));

			return true;
		}
	}

	return true;
}


//列表事件
bool CAttemperEngineSink::OnTCPSocketMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{	
	case SUB_CS_S_SERVER_INSERT:	//房间插入
	{
		//效验参数
		ASSERT(wDataSize % sizeof(tagGameServer) == 0);
		if (wDataSize % sizeof(tagGameServer) != 0) return false;

		//变量定义
		WORD wItemCount = wDataSize / sizeof(tagGameServer);
		tagGameServer* pGameServer = (tagGameServer*)pData;

		return true;
	}
	case SUB_CS_S_SERVER_REMOVE:	//房间移除
	{
		//效验参数
		ASSERT(wDataSize == sizeof(CMD_CS_S_ServiceRemove));
		if (wDataSize != sizeof(CMD_CS_S_ServiceRemove)) return false;

		//变量定义
		CMD_CS_S_ServiceRemove* pServiceRemove = (CMD_CS_S_ServiceRemove*)pData;
		
		return true;
	}
	}

	return true;
}
//转发服务
bool CAttemperEngineSink::OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_APPEND_STOCK_ITEM:	  //添加库存
	{
		auto pAppendStockItem = (CMD_CS_AppendStockItem*)pData;

		//构造对象
		DBR_QueryStockItem QueryStockItem = {};
		QueryStockItem.wStockID = pAppendStockItem->wStockID;
		QueryStockItem.wCommandID = SUB_CS_APPEND_STOCK_ITEM;

		//投递数据
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_QUERY_STOCK_ITEM, 0, 0, 0, &QueryStockItem, sizeof(QueryStockItem));

		//输出日志
		CTraceService::TraceString(TEXT("添加库存：StockID=%d"), TraceLevel_Exception ,pAppendStockItem->wStockID);

		return true;
	}
	case SUB_CS_DELETE_STOCK_ITEM:	  //删除库存
	{
		auto pDeleteStockItem = (CMD_CS_DeleteStockItem*)pData;

		//删除库存
		m_ShareStockManager.DeleteStockItem(pDeleteStockItem->wStockID);

		//输出日志
		CTraceService::TraceString(TEXT("删除库存：StockID=%d"), TraceLevel_Exception, pDeleteStockItem->wStockID);

		return true;
	}
	case SUB_CS_MODIFY_STOCK_ITEM:	  //修改库存
	{
		auto pModifyStockItem = (CMD_CS_ModifyStockItem*)pData;

		//构造对象
		DBR_QueryStockItem QueryStockItem = {};
		QueryStockItem.wStockID = pModifyStockItem->wStockID;
		QueryStockItem.wCommandID = SUB_CS_MODIFY_STOCK_ITEM;

		//投递数据
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_QUERY_STOCK_ITEM, 0, 0, 0, &QueryStockItem, sizeof(QueryStockItem));

		//输出日志
		CTraceService::TraceString(TEXT("修改库存：StockID=%d"), TraceLevel_Exception, pModifyStockItem->wStockID);

		return true;
	}
	case SUB_CS_ADJUST_STOCK_SCORE:	  //调整库存值
	{
		auto pModifyStockScore= (CMD_CS_AdjustStockScore*)pData;

		//调整库存值
		if (m_ShareStockManager.AdjustStockScore(pModifyStockScore->wStockID, pModifyStockScore->lChangeScore))
		{
			//构造对象
			DBR_AdjustStockScore AdjustStockScore = {};
			AdjustStockScore.wStockID = pModifyStockScore->wStockID;
			AdjustStockScore.lChangeScore = pModifyStockScore->lChangeScore;

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_ADJUST_STOCK_SCORE, 0, 0, 0, &AdjustStockScore, sizeof(AdjustStockScore));

			//输出日志
			CTraceService::TraceString(TEXT("调整库存值：StockID=%d,ChangeScore=%I64d"), TraceLevel_Exception, pModifyStockScore->wStockID, pModifyStockScore->lChangeScore);
		}

		return true;
	}
	case SUB_CS_ADJUST_JACKPOT_SCORE: //调整彩金值
	{
		auto pModifyJackpotScore = (CMD_CS_AdjustJackpotScore*)pData;

		//调整彩金
		if (m_ShareStockManager.AdjustJackpotScore(pModifyJackpotScore->wStockID, pModifyJackpotScore->wLevelID, pModifyJackpotScore->lChangeScore))
		{
			//构造对象
			DBR_AdjustJackpotScore AdjustJackpotScore = {};
			AdjustJackpotScore.wStockID = pModifyJackpotScore->wStockID;
			AdjustJackpotScore.wLevelID = pModifyJackpotScore->wLevelID;
			AdjustJackpotScore.lChangeScore = pModifyJackpotScore->lChangeScore;

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_ADJUST_JACKPOT_SCORE, 0, 0, 0, &AdjustJackpotScore, sizeof(AdjustJackpotScore));

			//输出日志
			CTraceService::TraceString(TEXT("调整彩金值：StockID=%d,LevelID=%d,ChangeScore=%I64d"), TraceLevel_Exception, pModifyJackpotScore->wStockID, pModifyJackpotScore->wLevelID, pModifyJackpotScore->lChangeScore);
		}

		return true;
	}	
	case SUB_CS_UPDATE_SPLAYER_CONTROL: //更新大R控制
	{
		//构造结构
		DBR_LoadGlobalConfig LoadGlobalConfig;
		ZeroMemory(&LoadGlobalConfig, sizeof(LoadGlobalConfig));

		//设置掩码
		LoadGlobalConfig.dwConfigMask = CONFIG_MASK_SPLAYER_CONTROL;

		//加载配置
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_LOAD_GLOBAL_CONFIG, 0, 0, 0, &LoadGlobalConfig, sizeof(LoadGlobalConfig));

		return true;
	}	
	}

	return false;
}

//管理服务
bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_EXECUTE_COMMAND:			//执行命令
	{
		//参数校验
		ASSERT(sizeof(tagExecuteCommand) == wDataSize);
		if (sizeof(tagExecuteCommand) != wDataSize) return false;

		//提取数据
		auto pCommandCode = (tagExecuteCommand*)pData;

		//加载配置
		if (pCommandCode->dwCommandCode & CMD_LOAD_CONFIG_DATA)
		{
			//重载配置
			if (pCommandCode->dwCommandParam != 0 )
			{
				//构造结构
				DBR_LoadGlobalConfig LoadGlobalConfig;
				ZeroMemory(&LoadGlobalConfig, sizeof(LoadGlobalConfig));

				//设置掩码
				LoadGlobalConfig.dwConfigMask = pCommandCode->dwCommandParam;

				//加载配置
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_LOAD_GLOBAL_CONFIG, 0, 0, 0, &LoadGlobalConfig, sizeof(LoadGlobalConfig));
			}
		}

		return true;
	}
	}

	return true;
}

//游戏类型
bool CAttemperEngineSink::OnDBResultGameKindItem(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//效验参数
	ASSERT(wDataSize % sizeof(DBO_GameKind) == 0);
	if (wDataSize % sizeof(DBO_GameKind) != 0) return false;

	//变量定义
	auto pGameKind = (DBO_GameKind*)pData;
	WORD wItemCount = wDataSize / sizeof(DBO_GameKind);	

	//更新数据
	for (WORD i = 0; i < wItemCount; i++)
	{
		//变量定义
		tagGameKindEx GameKind;
		ZeroMemory(&GameKind, sizeof(GameKind));

		//构造数据
		GameKind.wModuleID = (pGameKind + i)->wModuleID;
		GameKind.wTypeID = (pGameKind + i)->wTypeID;
		GameKind.wJoinID = (pGameKind + i)->wJoinID;
		GameKind.wSortID = (pGameKind + i)->wSortID;
		GameKind.wKindID = (pGameKind + i)->wKindID;
		StringCchCopy(GameKind.szKindName, CountArray(GameKind.szKindName), (pGameKind + i)->szKindName);
		StringCchCopy(GameKind.szProcessName, CountArray(GameKind.szProcessName), (pGameKind + i)->szProcessName);

		//支持掩码
		GameKind.wSupportMask = (pGameKind + i)->wSupportMask;

		//插入列表
		if ((pGameKind + i)->bNullity == false)
		{
			//查找类型 
			auto pGameKind = m_StockGameKind.SearchStockItem(GameKind.wKindID);
			if (pGameKind == NULL)		
			{
				pGameKind = m_StockGameKind.CreateStockItem(GameKind.wKindID);
			}

			//拷贝数据
			if (pGameKind != NULL)
			{
				CopyMemory(pGameKind,&GameKind,sizeof(GameKind));
			}
		}
		else
		{
			m_StockGameKind.RemoveStockItem(GameKind.wKindID);
		}
	}

	return true;
}

//库存子项
bool CAttemperEngineSink::OnDBResultStockListItem(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//校验参数
	ASSERT(wDataSize%sizeof(tagStockItem)==0);
	if (wDataSize % sizeof(tagStockItem) != 0) return false;

	//提取数据
	auto pStockItem = (tagStockItem*)pData;
	auto wItemCount = wDataSize / sizeof(tagStockItem);	

	//添加库存
	m_ShareStockManager.AppendStockItem(pStockItem, wItemCount);	

	return true;
}

//大R控制
bool CAttemperEngineSink::OnDBResultSuperPlayerControl(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//校验参数
	ASSERT(wDataSize==sizeof(tagSuperPlayerControl));
	if (wDataSize != sizeof(tagSuperPlayerControl)) return false;

	//提取数据
	auto pSuperPlayerControl = (tagSuperPlayerControl*)pData;

	//应用大R控制
	m_ShareStockManager.ApplySuperPlayerControl(pSuperPlayerControl);

	return true;
}

//列表完成
bool CAttemperEngineSink::OnDBResultLoadConfigFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//事件通知
	CP_ControlResult ControlResult;
	ControlResult.cbSuccess = ER_SUCCESS;
	SendUIControlPacket(UI_LOAD_CONFIG_RESULT, &ControlResult, sizeof(ControlResult));

	return true;
}

//查询结果
bool CAttemperEngineSink::OnDBResultQueryStockItem(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize == sizeof(DBO_QueryStockItem));
	if (wDataSize != sizeof(DBO_QueryStockItem)) return false;

	//提取数据
	auto pQueryStockItem = (DBO_QueryStockItem*)pData;

	//添加库存
	if (pQueryStockItem->wCommandID == SUB_CS_APPEND_STOCK_ITEM)
	{
		m_ShareStockManager.AppendStockItem(&pQueryStockItem->StockItem, 1);
	}

	//编辑库存
	if (pQueryStockItem->wCommandID == SUB_CS_MODIFY_STOCK_ITEM)
	{
		//修改库存
		m_ShareStockManager.ModifyStockItem(&pQueryStockItem->StockItem);
	}	

	return true;
}

//更新库存
bool CAttemperEngineSink::OnDBResultUpdateStockScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//移除标识
	RemoveStockUpdateID(dwTokenID);

	//判断状态
	if (m_bWillConcludeService && m_StockUpdateArray.GetCount()==0)
	{
		//事件通知
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess = ER_SUCCESS;
		SendUIControlPacket(UI_SERVICE_WILLSTOP_RESULT, &ControlResult, sizeof(ControlResult));
	}

	return true;
}

//更新状态
bool CAttemperEngineSink::OnDBResultUpdateStockState(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//移除标识
	RemoveStockUpdateID(dwTokenID);

	//判断状态
	if (m_bWillConcludeService && m_StockUpdateArray.GetCount() == 0)
	{
		//事件通知
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess = ER_SUCCESS;
		SendUIControlPacket(UI_SERVICE_WILLSTOP_RESULT, &ControlResult, sizeof(ControlResult));
	}

	return true;
}

//更新彩金
bool CAttemperEngineSink::OnDBResultUpdateJackpotScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//移除标识
	RemoveStockUpdateID(dwTokenID);

	//判断状态
	if (m_bWillConcludeService && m_StockUpdateArray.GetCount() == 0)
	{
		//事件通知
		CP_ControlResult ControlResult;
		ControlResult.cbSuccess = ER_SUCCESS;
		SendUIControlPacket(UI_SERVICE_WILLSTOP_RESULT, &ControlResult, sizeof(ControlResult));
	}

	return true;
}

//上报彩金
VOID CAttemperEngineSink::ReportGameJackpot()
{
	//变量定义
	CMD_CS_C_ReportJackpot ReportJackpot;
	ZeroMemory(&ReportJackpot, sizeof(ReportJackpot));

	//变量定义
	auto pGameKind = m_StockGameKind.GetHeadStockItem();
	while (pGameKind != NULL)
	{
		auto pCurrentGameKind = pGameKind;
		pGameKind = m_StockGameKind.GetNextStockItem(pCurrentGameKind);

		//设置变量
		ReportJackpot.lJackpotScore = 0;
		ReportJackpot.wKindID = pCurrentGameKind->wKindID;

		//统计彩金
		if (m_ShareStockManager.CollectJackpotScore(STOCK_KIND_GOLD, ReportJackpot.wKindID, ReportJackpot.lJackpotScore))
		{
			//发送信息
			ASSERT(m_pITCPSocketCenter != NULL);
			m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE, SUB_CS_C_REPORT_JACKPOT, &ReportJackpot, sizeof(ReportJackpot), 0);
		}
	}
}

//移除标识
VOID CAttemperEngineSink::RemoveStockUpdateID(DWORD dwUpdateID)
{
	for (int i = 0; i < m_StockUpdateArray.GetCount(); i++)
	{
		if (m_StockUpdateArray[i] == dwUpdateID)
		{
			m_StockUpdateArray.RemoveAt(i);
		}
	}
}

//发送请求
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//发送数据
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->m_UIControlSink.PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//////////////////////////////////////////////////////////////////////////
