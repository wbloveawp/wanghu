#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_CONNECT_CENTER			1									//重连标识
#define IDI_LOAD_BATTLE_OPTION		2									//逻辑配置
#define IDI_LOAD_FORBID_WORDS		3									//加载禁词

//时长定义
#define TIME_LOAD_BATTLE_OPTION		60									//加载配置

	
//////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//设置变量		
	m_bLogicSevice=false;
	m_bNeedConnStatus=true;
	m_pLogicOption=NULL;
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;			

	//组件变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPSocketCenter = NULL;
	m_pITCPNetworkEngine=NULL;	

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
	m_pBindParameter=new tagBindParameter[m_pLogicOption->wMaxConnect];
	ZeroMemory(m_pBindParameter,sizeof(tagBindParameter)*m_pLogicOption->wMaxConnect);

	//设置状态
	m_bLogicSevice=true;

	//加载禁词
	m_pITimerEngine->SetTimer(IDI_LOAD_FORBID_WORDS,1000L,1,0);

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//设置变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;	
	m_pITCPSocketCenter=NULL;

	//设置状态
	m_bLogicSevice=false;
	m_bNeedConnStatus=true;

	//删除数据
	SafeDeleteArray(m_pBindParameter);

	//重置数据
	m_GlobalItemManager.ResetData();		

	//删除用户
	m_ClubUserManager.DeleteUserItem();	

	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize)
{
	switch (wIdentifier)
	{	
	case CT_CONNECT_CENTER:		//连接中心
		{
			//获取地址					
			auto pAddressInfo=&m_pInitParameter->m_CenterAddress;

			//发起连接
			m_pITCPSocketCenter->Connect(htonl(pAddressInfo->dwServiceAddr), m_pInitParameter->m_wCenterPort);

			//提示消息
			CTraceService::TraceString(TEXT("正在尝试连接中心服务器..."),TraceLevel_Normal);

			return true;
		}
	case CT_LOAD_CLUB_OPTION:		//加载配置
		{
			//加载配置
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_CLUB_OPTION,0, 0, 0,NULL,0);

			//提示消息
			CTraceService::TraceString(TEXT("正在尝试加载群组配置信息..."),TraceLevel_Normal);

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
			//获取地址					
			auto pAddressInfo=&m_pInitParameter->m_CenterAddress;

			//发起连接
			m_pITCPSocketCenter->Connect(htonl(pAddressInfo->dwServiceAddr), m_pInitParameter->m_wCenterPort);

			//提示消息
			CTraceService::TraceString(TEXT("正在尝试连接中心服务器..."),TraceLevel_Normal);

			return true;
		}
	case IDI_LOAD_FORBID_WORDS:		//加载禁词
		{
			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_FORBID_WORDS,0,0,0,NULL,0);

			return true;
		}
	}

	return false;
}

//应答事件
bool CAttemperEngineSink::OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID)
{
	//获取索引
	ASSERT(LOWORD(dwSocketID)<m_pLogicOption->wMaxConnect);
	if (LOWORD(dwSocketID)>=m_pLogicOption->wMaxConnect) return false;

	//变量定义
	WORD wBindIndex=LOWORD(dwSocketID);
	auto pBindParameter=(m_pBindParameter+wBindIndex);

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

	//注册命令
	if (wMainCmdID == MDM_CS_REGISTER)
	{
		return OnTCPNetworkMainRegister(Command.wSubCmdID, pData, wDataSize, dwSocketID);
	}

	//通用服务
	if (wMainCmdID == MDM_CS_COMMON_SERVICE)
	{
		return OnTCPNetworkMainCommonService(Command.wSubCmdID, pData, wDataSize, dwSocketID);
	}

	switch (wMainCmdID)
	{
	case MDM_CLUBPLAZA_SERVICE:		//逻辑服务
		{
			return OnTCPNetworkMainClubPlazaService(Command.wSubCmdID,pData,wDataSize,dwSocketID,dwTokenID);
		}
	}

	return false;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	auto pBindParameter=(m_pBindParameter+wBindIndex);

	//网关服务
	if (pBindParameter->wServiceModule==SERVICE_MODULE_GATE)
	{
		//查找网关
		auto pGlobalGateItem=m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem!=NULL)
		{
			//注销网关
			m_GlobalItemManager.DeleteGateItem(pBindParameter->wServiceID);					
		}
	}

	//清除信息
	ZeroMemory(pBindParameter,sizeof(tagBindParameter));

	return false;
}

//数据库事件
bool CAttemperEngineSink::OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	try
	{
		switch (wRequestID)
		{
		case DBO_GR_LOGON_SUCCESS:		//登录成功
		{
			return OnDBLogonSuccess(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_LOGON_FAILURE:		//登录失败
		{
			return OnDBLogonFailure(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_LOGON_FINISH:		//登录完成
		{
			return OnDBLogonFinish(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_CLUB_NAMECARD:		//茶社名片
		{
			return OnDBClubNameCard(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_SEARCH_CLUB:		//查找群组
		{
			return OnDBSearchClub(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_CREATE_CLUB:		//创建群组
		{
			return OnDBCreateClub(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_UPDATE_CLUB:		//更新群组
		{
			return OnDBUpdateClub(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_DELETE_CLUB:		//删除群组
		{
			return OnDBDeleteClub(dwContextID, dwTokenID, pData, wDataSize);
		}		
		case DBO_GR_INSERT_MEMBER:		//添加成员
		{
			return OnDBInsertMember(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_UPDATE_MEMBER:		//更新成员
		{
			return OnDBUpdateMember(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_DELETE_MEMBER:		//删除成员
		{
			return OnDBDeleteMember(dwContextID, dwTokenID, pData, wDataSize);
		}		
		case DBO_GR_MODIFY_SCORE:		//修改分数
		{
			return OnDBModifyScore(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_APPLY_MESSAGE:		//申请消息
		{
			return OnDBApplyMessage(dwContextID, dwTokenID, pData, wDataSize);
		}		
		case DBO_GR_CLUB_OPTION:		//群组配置
		{
			return OnDBClubOption(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_OPTION_FINISH:		//配置完成
		{
			return OnDBOptionFinish(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_FIRBID_WORDS:		//禁止词汇
		{
			return OnDBForbidWords(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_WEALTH_UPDATE:		//财富更新
		{
			return OnDBWealthUpdate(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_APPLY_DELETE:		//申请删除
		{
			return OnDBApplyDelete(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_DETECT_CLUBNAME:	//检测名称
		{
			return OnDBDetectClubName(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_OPERATE_SUCCESS:	//操作成功
		{
			return OnDBOperateSuccess(dwContextID, dwTokenID, pData, wDataSize);
		}
		case DBO_GR_OPERATE_FAILURE:	//操作失败
		{
			return OnDBOperateFailure(dwContextID, dwTokenID, pData, wDataSize);
		}	
		}
	}
	catch (...)
	{
		//构造提示
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("OnEventDataBase(SubCmdID=%d)"),wRequestID);

		//提示消息
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);
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
			//构造提示
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("与中心服务器的连接关闭了，%ld 秒后将重新连接"),m_pInitParameter->m_wConnectTime);

			//提示消息
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

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
			//构造提示
			TCHAR szDescribe[128]=TEXT("");
			_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("中心服务器连接失败 [ %ld ]，%ld 秒后将重新连接"),
				nErrorCode,m_pInitParameter->m_wConnectTime);

			//提示消息
			CTraceService::TraceString(szDescribe,TraceLevel_Warning);

			//设置时间
			ASSERT(m_pITimerEngine!=NULL);
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}

		//变量定义
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService,sizeof(RegisterService));

		//构造结构
		RegisterService.wServiceID=m_pLogicOption->wServiceID;
		RegisterService.wServiceModule=SERVICE_MODULE_CLUBPLAZA;
		RegisterService.wMaxConnect = m_pLogicOption->wMaxConnect;
		RegisterService.wServicePort=m_pLogicOption->wServicePort;
		lstrcpyn(RegisterService.szServiceName, TEXT("茶社广场"), CountArray(RegisterService.szServiceName));
		lstrcpyn(RegisterService.szServiceDomain,m_pLogicOption->szServiceDomain,CountArray(RegisterService.szServiceDomain));

		//发送数据
		m_pITCPSocketCenter->SendData(MDM_CS_REGISTER,SUB_CS_C_REGISTER_SERVICE,&RegisterService,sizeof(RegisterService),0);

		return true;
	}	

	return false;
}

//读取事件
bool CAttemperEngineSink::OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD *pdwTokenID, WORD wTokenCount)
{
	//中心连接
	if (dwServiceID==NETWORK_CENTER)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:			//注册服务
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
		}
		case MDM_CS_USER_COLLECT:		//用户收集
		{
			return OnTCPSocketMainUserCollect(Command.wSubCmdID, pData, wDataSize);
		}
		case MDM_CS_COMMON_SERVICE:		//服务信息
		{
			return OnTCPSocketMainCommonService(Command.wSubCmdID,pData,wDataSize);
		}
		case MDM_CS_MANAGER_SERVICE:	//管理服务
		{
			return OnTCPSocketMainManagerService(Command.wSubCmdID,pData,wDataSize);
		}
		case MDM_CS_FORWARD_SERVICE:	//转发服务
		{
			return OnTCPSocketMainForwardService(Command.wSubCmdID, pData, wDataSize);
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

			//汇总用户
			m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_COLLECT_USER, NULL, 0, 0);

			return true;
		}
	case SUB_CS_S_REGISTER_FAILURE:		//注册失败
		{
			//效验参数
			auto pRegisterFailure=(CMD_CS_S_RegisterFailure *)pData;
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

//收集事件
bool CAttemperEngineSink::OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_USER_LIST:		//用户列表
	{
		//校验参数
		ASSERT (wDataSize % sizeof(tagGlobalUserInfo) == 0);
		if (wDataSize % sizeof(tagGlobalUserInfo) != 0) return true;

		//提取数据
		auto pUserItem = (tagGlobalUserInfo*)pData;
		WORD wUserCount = wDataSize / sizeof(tagGlobalUserInfo);		

		//变量定义
		tagClubUserInfo UserInfo;
		ZeroMemory(&UserInfo, sizeof(UserInfo));

		//变量定义
		CClubUserItem* pServerUserItem = NULL;

		//插入用户
		for (WORD i = 0; i < wUserCount; i++)
		{
			//校验状态
			if (pUserItem->cbCBUserStatus == US_CB_OFFLINE) continue;

			//查找用户
			pServerUserItem = m_ClubUserManager.SearchUserItem(pUserItem->dwUserID);
			if (pServerUserItem != NULL)
			{
				pServerUserItem->SetSocketInfo(pUserItem->SocketInfo);
				pServerUserItem->SetUserStatus(pUserItem->cbCBUserStatus);
			}
			else
			{
				//构造用户
				UserInfo.dwUserID = pUserItem->dwUserID;
				UserInfo.cbGender = pUserItem->cbGender;
				UserInfo.cbUserStatus = pUserItem->cbCBUserStatus;
				StringCchCopy(UserInfo.szNickName, CountArray(UserInfo.szNickName), pUserItem->szNickName);

				//头像信息
				UserInfo.wFaceID = pUserItem->wFaceID;
				UserInfo.dwCustomID = pUserItem->dwCustomID;
				StringCchCopy(UserInfo.szUserHeadUrl, CountArray(UserInfo.szUserHeadUrl), pUserItem->szUserHeadUrl);//kk jia
				

				//插入用户
				m_ClubUserManager.InsertUserItem(UserInfo, pUserItem->SocketInfo);
			}

			pUserItem++;
		}

		return true;
	}
	case SUB_CS_S_COLLECT_FINISH:	//列表完成
	{
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
	case SUB_CS_S_SERVICE_INSERT:	//状态插入
	{
		//参数校验
		ASSERT(wDataSize % sizeof(tagServiceItem) == 0);
		if (wDataSize % sizeof(tagServiceItem) != 0) return false;

		//提取数据
		auto pServiceItem = (tagServiceItem *)pData;

		return true;
	}
	case SUB_CS_S_SERVICE_REMOVE:	//状态移除
	{
		//参数校验
		ASSERT(wDataSize == sizeof(CMD_CS_S_ServiceRemove));
		if (wDataSize != sizeof(CMD_CS_S_ServiceRemove)) return false;

		//提取数据
		auto pServiceRemove = (CMD_CS_S_ServiceRemove *)pData;

		return true;
	}	
	}

	return true;
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
				//敏感词汇
				if (pCommandCode->dwCommandParam & CONFIG_MASK_FILTER_WORDS)
				{
					//加载配置
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_FORBID_WORDS, 0, 0, 0, 0, 0);
				}

				//茶社配置
				if (pCommandCode->dwCommandParam & CONFIG_MASK_CLUB_OPTION)
				{
					//加载配置
					m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOAD_CLUB_OPTION, 0, 0, 0, 0, 0);
				}
			}

			//停止服务
			if (pCommandCode->dwCommandCode&CMD_STOP_CLUB_SERVICE)
			{
				//设置标识
				m_bLogicSevice=false;
			}

			//启动服务
			if (pCommandCode->dwCommandCode&CMD_START_CLUB_SERVICE)
			{
				//设置标识
				m_bLogicSevice=true;
			}

			return true;
		}
	}

	return true;
}

//逻辑转发
bool CAttemperEngineSink::OnTCPSocketMainForwardService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_CB_USER_ONLINE:			//用户上线
	{
		//参数校验
		ASSERT (wDataSize == sizeof(tagGlobalUserInfo));
		if (wDataSize != sizeof(tagGlobalUserInfo)) return false;

		//提取数据
		auto pGlobalUserInfo = (tagGlobalUserInfo*)pData;

		//查找对象
		auto pClubUserItem = m_ClubUserManager.SearchUserItem(pGlobalUserInfo->dwUserID);
		if (pClubUserItem != NULL)
		{
			//用户信息
			auto pClubUserInfo = pClubUserItem->GetUserInfo(); 

			//设置变量
			pClubUserInfo->wFaceID = pGlobalUserInfo->wFaceID;
			pClubUserInfo->dwCustomID = pGlobalUserInfo->dwCustomID;
			pClubUserInfo->cbUserStatus = pGlobalUserInfo->cbCBUserStatus;
			StringCchCopy(pClubUserInfo->szNickName,CountArray(pClubUserInfo->szNickName),pGlobalUserInfo->szNickName);

			//设置网络				
			pClubUserItem->SetSocketInfo(pGlobalUserInfo->SocketInfo);
		}
		else
		{
			//构造结构
			tagClubUserInfo ClubUserInfo = {};

			//设置变量
			ClubUserInfo.dwUserID = pGlobalUserInfo->dwUserID;
			ClubUserInfo.dwGameID = pGlobalUserInfo->dwGameID;
			ClubUserInfo.cbGender = pGlobalUserInfo->cbGender;
			ClubUserInfo.wFaceID = pGlobalUserInfo->wFaceID;
			ClubUserInfo.dwCustomID = pGlobalUserInfo->dwCustomID;
			ClubUserInfo.cbUserStatus = pGlobalUserInfo->cbCBUserStatus;
			StringCchCopy(ClubUserInfo.szNickName, CountArray(ClubUserInfo.szNickName), pGlobalUserInfo->szNickName);
			StringCchCopy(ClubUserInfo.szUserHeadUrl, CountArray(ClubUserInfo.szUserHeadUrl), pGlobalUserInfo->szUserHeadUrl);


			//插入用户
			m_ClubUserManager.InsertUserItem(ClubUserInfo, pGlobalUserInfo->SocketInfo);
		}

		return true;
	}
	case SUB_CS_CB_USER_OFFLINE:			//用户下线
	{
		//参数校验
		ASSERT (wDataSize == sizeof(CMD_CS_CB_UserOffline));
		if (wDataSize != sizeof(CMD_CS_CB_UserOffline)) return false;

		//提取数据
		auto pUserOffline = (CMD_CS_CB_UserOffline*)pData;

		//删除用户
		m_ClubUserManager.DeleteUserItem(pUserOffline->dwUserID);

		return true;
	}			
	}

	return true;
}

//逻辑服务
bool CAttemperEngineSink::OnTCPNetworkMainClubPlazaService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID,DWORD dwTokenID)
{
	//获取参数
	WORD wBindIndex=LOWORD(dwSocketID);
	auto pBindParameter=(m_pBindParameter+wBindIndex);

	//令牌参数
	auto pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
	if (pTokenParameter == NULL) return false;

	//停止服务
	if (m_bLogicSevice==false)
	{
		//构造消息
		CMD_CM_SystemMessage SystemMessage;
		ZeroMemory(&SystemMessage,sizeof(SystemMessage));

		//变量定义						
		SystemMessage.wType=SMT_EJECT|SMT_CLOSE_LINK;
		StringCchCopy(SystemMessage.szString,CountArray(SystemMessage.szString), CWHService::FormatMessage(MIM_CLUBPLAZA_0001,TEXT("系统维护中，茶社暂时停止服务。请稍后再次重试！")));

		//发送消息
		SystemMessage.wLength=CountStringBuffer(SystemMessage.szString);
		WORD wPacketSize=sizeof(SystemMessage)-sizeof(SystemMessage.szString)+ SystemMessage.wLength;
		SendDataToGate(pBindParameter->wServiceID, MDM_CM_SYSTEM, SUB_CM_SYSTEM_MESSAGE,&SystemMessage,wPacketSize,&dwTokenID,1);

		return true;
	}	

	try
	{
		switch (wSubCmdID)
		{		
		case SUB_CP_C_ENTER_SERVICE:	//进入服务
		{
			//构造结构
			DBR_GR_LogonService LogonService;
			ZeroMemory(&LogonService,sizeof(LogonService));

			//设置变量
			LogonService.dwUserID=pTokenParameter->dwUserID;			

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_LOGON_SERVICE, LogonService.dwUserID,dwSocketID,dwTokenID,&LogonService,sizeof(LogonService));

			return true;
		}		
		case SUB_CP_C_SEARCH_CLUB:		//搜索群组
		{
			//参数校验
			ASSERT (wDataSize==sizeof(CMD_CP_C_SearchClub));
			if (wDataSize!=sizeof(CMD_CP_C_SearchClub)) return true;

			//提取数据
			auto pSearchClub=(CMD_CP_C_SearchClub *)pData;				

			//构造结构
			DBR_GR_SearchClub SearchClub;
			ZeroMemory(&SearchClub, sizeof(SearchClub));

			//设置变量
			SearchClub.dwClubID = pSearchClub->dwClubID;
			SearchClub.dwUserID = pTokenParameter->dwUserID;				

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SEARCH_CLUB, SearchClub.dwUserID, dwSocketID, dwTokenID, &SearchClub, sizeof(SearchClub));

			return true;
		}
		case SUB_CP_C_CREATE_CLUB:		//创建群组
		{
			//参数校验
			ASSERT (wDataSize==sizeof(CMD_CP_C_CreateClub));
			if (wDataSize!=sizeof(CMD_CP_C_CreateClub)) return true;

			//提取数据
			auto pCreateClub=(CMD_CP_C_CreateClub *)pData;

			//调整数据
			pCreateClub->szClubName[CountArray(pCreateClub->szClubName)-1]=0;
			pCreateClub->szClubIntroduce[CountArray(pCreateClub->szClubIntroduce)-1]=0;

			//查询用户
			auto pServerUserItem=m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
			if (pServerUserItem==NULL) return true;

			//获取对象
			auto pSocketInfo=pServerUserItem->GetSocketInfo();

			//校验茶社名
			if (m_WordFilter.FilterWords(pCreateClub->szClubName)==true)
			{
				//获取对象
				CMD_CP_S_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure,sizeof(OperateFailure));

				//设置变量		
				StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0002, TEXT("抱歉, 茶社名称中包含敏感词汇, 茶社创建失败！")));

				//发送数据
				WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
				wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
				SendDataToGate(pSocketInfo->wGateID,MDM_CLUBPLAZA_SERVICE,SUB_CP_S_OPERATE_FAILURE,&OperateFailure,wPacketSize,&dwTokenID,1);				

				return true;
			}

			//校验茶社介绍
			if (m_WordFilter.FilterWords(pCreateClub->szClubIntroduce)==true)
			{
				//获取对象
				CMD_CP_S_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure,sizeof(OperateFailure));

				//设置变量		
				StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0003, TEXT("抱歉,茶社介绍中包含敏感词汇,茶社创建失败！")));

				//发送数据
				WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
				wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
				SendDataToGate(pSocketInfo->wGateID,MDM_CLUBPLAZA_SERVICE,SUB_CP_S_OPERATE_FAILURE,&OperateFailure,wPacketSize,&dwTokenID,1);				

				return true;
			}

			//构造结构
			DBR_GR_CreateClub CreateClub;
			ZeroMemory(&CreateClub,sizeof(CreateClub));

			//群组信息
			CreateClub.dwCreateID= pTokenParameter->dwUserID;

			//群组属性
			CreateClub.wClubKind= pCreateClub->wClubKind;
			CreateClub.wClubIconID = pCreateClub->wClubIconID;
			StringCchCopy(CreateClub.szClubName,CountArray(CreateClub.szClubName), pCreateClub->szClubName);
			StringCchCopy(CreateClub.szClubIntroduce,CountArray(CreateClub.szClubIntroduce), pCreateClub->szClubIntroduce);

			//群组规则
			CreateClub.cbPayType = pCreateClub->cbPayType;
			CreateClub.dwSwitchOption = pCreateClub->dwSwitchOption;
			CreateClub.cbCurrencyKind = pCreateClub->cbCurrencyKind;		

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_CREATE_CLUB, CreateClub.dwCreateID,dwSocketID,dwTokenID,&CreateClub,sizeof(CreateClub));

			return true;
		}
		case SUB_CP_C_UPDATE_CLUB:		//更新群组
		{
			//参数校验
			ASSERT (wDataSize>=sizeof(CMD_CP_C_UpdateClub));
			if (wDataSize<sizeof(CMD_CP_C_UpdateClub)) return true;

			//提取数据
			auto pUpdateClub=(CMD_CP_C_UpdateClub *)pData;

			//查询用户
			auto pServerUserItem=m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
			if (pServerUserItem==NULL) return true;

			//变量定义
			LPBYTE pcbBuffer=(LPBYTE)pData;
			WORD wOffsetPos=sizeof(CMD_CP_C_UpdateClub);

			//更新规则
			if ((pUpdateClub->cbUpdateKind&UPCLUB_KIND_RULES) != 0)
			{
				//设置偏移
				wOffsetPos+=sizeof(tagClubUpdate_Rules);
			}

			//更新公告
			if ((pUpdateClub->cbUpdateKind & UPCLUB_KIND_NOTICE) != 0)
			{
				//获取对象
				auto pClubNotice = (tagClubUpdate_Notice*)(pcbBuffer + wOffsetPos);
				pClubNotice->szNoticeContent[CountArray(pClubNotice->szNoticeContent) - 1] = 0;
				
				//校验公告内容
				if (m_WordFilter.FilterWords(pClubNotice->szNoticeContent) == true)
				{
					//获取对象
					tagSocketInfo* pSocketInfo = pServerUserItem->GetSocketInfo();

					//获取对象
					CMD_CP_S_OperateFailure OperateFailure;
					ZeroMemory(&OperateFailure, sizeof(OperateFailure));

					//设置变量		
					StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0016, TEXT("抱歉,公告内容中包含敏感词汇,修改失败！")));

					//发送数据
					WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
					wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
					SendDataToGate(pSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_OPERATE_FAILURE, &OperateFailure, wPacketSize, &dwTokenID, 1);

					return true;
				}

				//设置偏移
				wOffsetPos += sizeof(tagClubUpdate_Notice);
			}

			//更新属性
			if ((pUpdateClub->cbUpdateKind&UPCLUB_KIND_PROPERTY)!=0)
			{
				//获取对象
				auto pClubProperty=(tagClubUpdate_Property *)(pcbBuffer+wOffsetPos);
				pClubProperty->szClubName[CountArray(pClubProperty->szClubName)-1] = 0;
				pClubProperty->szClubIntroduce[CountArray(pClubProperty->szClubIntroduce)-1]=0;

				//校验茶社名称
				if (m_WordFilter.FilterWords(pClubProperty->szClubName) == true)
				{
					//获取对象
					tagSocketInfo * pSocketInfo = pServerUserItem->GetSocketInfo();

					//获取对象
					CMD_CP_S_OperateFailure OperateFailure;
					ZeroMemory(&OperateFailure, sizeof(OperateFailure));

					//设置变量		
					StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0004, TEXT("抱歉,茶社名称中包含敏感词汇,修改失败！")));

					//发送数据
					WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
					wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
					SendDataToGate(pSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_OPERATE_FAILURE, &OperateFailure, wPacketSize, &dwTokenID, 1);

					return true;
				}

				//校验茶社介绍
				if (m_WordFilter.FilterWords(pClubProperty->szClubIntroduce)==true)
				{
					//获取对象
					tagSocketInfo * pSocketInfo=pServerUserItem->GetSocketInfo();

					//获取对象
					CMD_CP_S_OperateFailure OperateFailure;
					ZeroMemory(&OperateFailure,sizeof(OperateFailure)); 

					//设置变量		
					StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0005, TEXT("抱歉,茶社介绍中包含敏感词汇,修改失败！")));

					//发送数据
					WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
					wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
					SendDataToGate(pSocketInfo->wGateID,MDM_CLUBPLAZA_SERVICE,SUB_CP_S_OPERATE_FAILURE,&OperateFailure,wPacketSize,&dwTokenID,1);				

					return true;
				}
			}

			//构造结构
			auto pDBUpdateClub=(DBR_GR_UpdateClub *)m_cbBuffer;

			//群组信息
			pDBUpdateClub->dwClubID=pUpdateClub->dwClubID;
			pDBUpdateClub->dwUserID = pTokenParameter->dwUserID;
			pDBUpdateClub->cbUpdateKind=pUpdateClub->cbUpdateKind;			

			//拷贝数据
			CopyMemory(pDBUpdateClub+1,pUpdateClub+1,wDataSize-sizeof(CMD_CP_C_UpdateClub));

			//投递数据
			WORD wPacketSize=sizeof(DBR_GR_UpdateClub)+wDataSize-sizeof(CMD_CP_C_UpdateClub);		
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_UPDATE_CLUB, pDBUpdateClub->dwUserID, dwSocketID,dwTokenID,m_cbBuffer,wPacketSize);

			return true;
		}
		case SUB_CP_C_DELETE_CLUB:		//删除群组
		{
			//参数校验
			ASSERT (wDataSize==sizeof(CMD_CP_C_DeleteClub));
			if (wDataSize!=sizeof(CMD_CP_C_DeleteClub)) return true;

			//提取数据
			auto  pDeleteClub=(CMD_CP_C_DeleteClub *)pData;

			//构造结构
			DBR_GR_DeleteClub DeleteClub;
			ZeroMemory(&DeleteClub,sizeof(DeleteClub));

			//群组标识
			DeleteClub.dwClubID=pDeleteClub->dwClubID;				
			DeleteClub.dwCreaterID= pTokenParameter->dwUserID;				

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_CLUB, DeleteClub.dwCreaterID,dwSocketID,dwTokenID,&DeleteClub,sizeof(DeleteClub));

			return true;
		}				
		case SUB_CP_C_UPDATE_MEMBER:	//更新成员
		{
			//参数校验
			ASSERT (wDataSize >= sizeof(CMD_CP_C_UpdateMember));
			if (wDataSize < sizeof(CMD_CP_C_UpdateMember)) return true;

			//提取数据
			auto pIMUpdateMember = (CMD_CP_C_UpdateMember *)pData;

			//查询用户
			auto pServerUserItem = m_ClubUserManager.SearchUserItem(pTokenParameter->dwUserID);
			if (pServerUserItem == NULL) return true;

			//获取对象
			auto pIMSocketInfo = pServerUserItem->GetSocketInfo();

			//更新成员类型
			if (pIMUpdateMember->wUpdateKind == UPMEMBER_KIND_TYPE)
			{
				//成员类型
				BYTE cbMemberType = *(BYTE *)(pIMUpdateMember + 1);

				//校验类型
				if (cbMemberType > MEMBER_TYPE_EMPLOYER)
				{
					//获取对象
					CMD_CP_S_OperateFailure OperateFailure;
					ZeroMemory(&OperateFailure, sizeof(OperateFailure));

					//设置变量		
					StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0006, TEXT("抱歉,成员类型异常,操作失败！")));

					//发送数据
					WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
					wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
					SendDataToGate(pIMSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_OPERATE_FAILURE, &OperateFailure, wPacketSize, &pIMSocketInfo->dwTokenID, 1);

					return true;
				}
			}

			//更新成员状态
			if (pIMUpdateMember->wUpdateKind == UPMEMBER_KIND_STATUS)
			{
				//成员类型
				BYTE cbMemberStatus = *(BYTE *)(pIMUpdateMember + 1);

				//校验状态
				if (cbMemberStatus > MEMBER_STATUS_FREEZE)
				{
					//获取对象
					CMD_CP_S_OperateFailure OperateFailure;
					ZeroMemory(&OperateFailure, sizeof(OperateFailure));

					//设置变量		
					StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0007, TEXT("抱歉,成员状态异常,操作失败！")));

					//发送数据
					WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
					wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
					SendDataToGate(pIMSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_OPERATE_FAILURE, &OperateFailure, wPacketSize, &pIMSocketInfo->dwTokenID, 1);

					return true;
				}
			}

			//构造结构
			auto pUpdateMember = (DBR_GR_UpdateMember *)m_cbBuffer;

			//群组信息
			pUpdateMember->dwClubID = pIMUpdateMember->dwClubID;
			pUpdateMember->dwMemberID = pIMUpdateMember->dwMemberID;

			//用户信息
			pUpdateMember->dwUserID = pTokenParameter->dwUserID;
			pUpdateMember->wUpdateKind = pIMUpdateMember->wUpdateKind;

			//拷贝数据
			CopyMemory(pUpdateMember+1, pIMUpdateMember+1, wDataSize-sizeof(CMD_CP_C_UpdateMember));

			//投递数据
			WORD wPacketSize = sizeof(DBR_GR_UpdateMember)+wDataSize-sizeof(CMD_CP_C_UpdateMember);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_UPDATE_MEMBER, pUpdateMember->dwUserID, dwSocketID, dwTokenID, m_cbBuffer, wPacketSize);

			return true;
		}
		case SUB_CP_C_DELETE_MEMBER:	//删除成员
		{
			//参数校验
			ASSERT (wDataSize==sizeof(CMD_CP_C_DeleteMember));
			if (wDataSize!=sizeof(CMD_CP_C_DeleteMember)) return true;

			//提取数据
			auto pDeleteMember=(CMD_CP_C_DeleteMember *)pData;

			//构造结构
			DBR_GR_DeleteMember DeleteMember;
			ZeroMemory(&DeleteMember,sizeof(DeleteMember));

			//群组信息
			DeleteMember.dwClubID=pDeleteMember->dwClubID;
			DeleteMember.dwMemberID=pDeleteMember->dwMemberID;

			//操作信息
			DeleteMember.cbDeleteKind=pDeleteMember->cbDeleteKind;

			//用户信息
			DeleteMember.dwUserID= pTokenParameter->dwUserID;
				
			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_MEMBER, DeleteMember.dwUserID,dwSocketID,dwTokenID,&DeleteMember,sizeof(DeleteMember));

			return true;
		}		
		case SUB_CP_C_APPLY_REQUEST:	//申请请求
		{
			//参数校验
			ASSERT (wDataSize==sizeof(CMD_CP_C_ApplyRequest));
			if (wDataSize!=sizeof(CMD_CP_C_ApplyRequest)) return true;

			//提取数据
			auto pApplyRequest=(CMD_CP_C_ApplyRequest *)pData;

			//构造结构
			DBR_GR_ApplyRequest ApplyRequest;
			ZeroMemory(&ApplyRequest,sizeof(ApplyRequest));

			//群组信息			
			ApplyRequest.dwClubID=pApplyRequest->dwClubID;
			ApplyRequest.dwApplyerID= pTokenParameter->dwUserID;
				
			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_APPLY_REQUEST, ApplyRequest.dwApplyerID,dwSocketID,dwTokenID,&ApplyRequest,sizeof(ApplyRequest));

			return true;
		}
		case SUB_CP_C_APPLY_RESPOND:	//申请响应
		{
			//参数校验
			ASSERT (wDataSize==sizeof(CMD_CP_C_ApplyRespond));
			if (wDataSize!=sizeof(CMD_CP_C_ApplyRespond)) return true;

			//提取数据
			auto pApplyRespond=(CMD_CP_C_ApplyRespond *)pData;

			//构造结构
			DBR_GR_ApplyRespond ApplyRespond;
			ZeroMemory(&ApplyRespond,sizeof(ApplyRespond));

			//设置变量
			ApplyRespond.dwMessageID=pApplyRespond->dwMessageID;

			//群组信息
			ApplyRespond.dwClubID=pApplyRespond->dwClubID;
			ApplyRespond.dwApplyerID=pApplyRespond->dwApplyerID;
			ApplyRespond.cbApplyStatus=pApplyRespond->cbApplyStatus;			

			//用户信息
			ApplyRespond.dwUserID= pTokenParameter->dwUserID;
				
			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_APPLY_RESPOND, ApplyRespond.dwUserID,dwSocketID,dwTokenID,&ApplyRespond,sizeof(ApplyRespond));

			return true;
		}
		case SUB_CP_C_APPLY_DELETE:		//申请删除
		{
			//参数校验
			ASSERT (wDataSize==sizeof(CMD_CP_C_ApplyDelete));
			if (wDataSize!=sizeof(CMD_CP_C_ApplyDelete)) return true;

			//提取数据
			auto pApplyDelete=(CMD_CP_C_ApplyDelete *)pData;			

			//构造结构
			DBR_GR_ApplyDelete ApplyDelete;
			ZeroMemory(&ApplyDelete,sizeof(ApplyDelete));

			//设置变量
			ApplyDelete.dwClubID = pApplyDelete->dwClubID;
			ApplyDelete.dwUserID = pTokenParameter->dwUserID;			

			//设置变量
			ApplyDelete.dwMessageID=pApplyDelete->dwMessageID;
			ApplyDelete.cbMessageType=pApplyDelete->cbMessageType;
			
			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_APPLY_DELETE, ApplyDelete.dwUserID,dwSocketID,dwTokenID,&ApplyDelete,sizeof(ApplyDelete));

			return true;
		}
		case SUB_CP_C_APPLY_REPEAL:		//申请撤销
		{
			//参数校验
			ASSERT (wDataSize==sizeof(CMD_CP_C_ApplyRepeal));
			if (wDataSize!=sizeof(CMD_CP_C_ApplyRepeal)) return true;

			//提取数据
			auto pApplyRepeal=(CMD_CP_C_ApplyRepeal *)pData;			

			//构造结构
			DBR_GR_ApplyRepeal ApplyRepeal;
			ZeroMemory(&ApplyRepeal,sizeof(ApplyRepeal));

			//设置变量
			ApplyRepeal.dwMessageID=pApplyRepeal->dwMessageID;

			//设置变量
			ApplyRepeal.dwUserID= pTokenParameter->dwUserID;
				
			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_APPLY_REPEAL, ApplyRepeal.dwUserID,dwSocketID,dwTokenID,&ApplyRepeal,sizeof(ApplyRepeal));

			return true;
		}
		case SUB_CP_C_SETTLE_BATTLE:	//约战结算
		{
			//参数校验
			ASSERT (wDataSize==sizeof(CMD_CP_C_SettleBattle));
			if (wDataSize!=sizeof(CMD_CP_C_SettleBattle)) return true;

			//提取数据
			auto pSettleBattle=(CMD_CP_C_SettleBattle *)pData;

			//构造结构
			DBR_GR_SettleBattle SettleBattle;
			ZeroMemory(&SettleBattle,sizeof(SettleBattle));

			//设置变量
			SettleBattle.dwDateID=pSettleBattle->dwDateID;
			SettleBattle.dwClubID=pSettleBattle->dwClubID;
			SettleBattle.dwMemberID=pSettleBattle->dwMemberID;
			SettleBattle.wSettleCount=pSettleBattle->wSettleCount;

			//用户信息
			SettleBattle.dwCreaterID= pTokenParameter->dwUserID;
				
			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_SETTLE_BATTLE, SettleBattle.dwCreaterID,dwSocketID,dwTokenID,&SettleBattle,sizeof(SettleBattle));

			return true;
		}	
		case SUB_CP_C_MODIFY_SCORE:		//修改积分
		{
			//参数校验
			ASSERT(wDataSize == sizeof(CMD_CP_C_ModifyScore));
			if (wDataSize != sizeof(CMD_CP_C_ModifyScore)) return true;

			//提取数据
			auto pModifyScore = (CMD_CP_C_ModifyScore *)pData;

			//构造结构
			DBR_GR_ModifyScore ModifyScore;
			ZeroMemory(&ModifyScore, sizeof(ModifyScore));

			//设置变量				
			ModifyScore.dwUserID = pTokenParameter->dwUserID;
			ModifyScore.dwClubID = pModifyScore->dwClubID;
			ModifyScore.dwMemberID = pModifyScore->dwMemberID;
			ModifyScore.lModifyScore= pModifyScore->lModifyScore;				
				
			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_MODIFY_SCORE, ModifyScore.dwUserID,dwSocketID, dwTokenID, &ModifyScore, sizeof(ModifyScore));

			return true;
		}
		case SUB_CP_C_DELETE_RECORD:	//删除记录
		{
			//参数校验
			ASSERT(wDataSize == sizeof(CMD_CP_C_DeleteRecord));
			if (wDataSize != sizeof(CMD_CP_C_DeleteRecord)) return true;

			//提取数据
			auto pDeleteRecord = (CMD_CP_C_DeleteRecord*)pData;

			//构造结构
			DBR_GR_DeleteRecord DeleteRecord = {};

			//设置变量
			DeleteRecord.dwClubID = pDeleteRecord->dwClubID;
			DeleteRecord.dwUserID = pTokenParameter->dwUserID;
			DeleteRecord.dwRecordID = pDeleteRecord->dwRecordID;
			DeleteRecord.cbRecordKind = pDeleteRecord->cbRecordKind;

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DELETE_RECORD, pTokenParameter->dwUserID, dwSocketID, dwTokenID, &DeleteRecord, sizeof(DeleteRecord));

			return true;
		}
		case SUB_CP_C_DETECT_CLUBNAME:	//检测名称
		{
			//参数校验
			ASSERT(wDataSize == sizeof(CMD_CP_C_DetectClubName));
			if (wDataSize != sizeof(CMD_CP_C_DetectClubName)) return true;

			//提取数据
			auto pDetectClubName = (CMD_CP_C_DetectClubName*)pData;

			//校验茶社名
			if (m_WordFilter.FilterWords(pDetectClubName->szClubName) == true)
			{
				//获取对象
				CMD_CP_S_DetectResult DetectResult = {};

				//设置变量		
				DetectResult.bUsable = false;
				StringCchCopy(DetectResult.szErrorDescribe, CountArray(DetectResult.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0001, TEXT("茶社名称包含敏感词汇！")));

				//发送数据
				SendDataToGate(pBindParameter->wServiceID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_DETECT_RESULT, &DetectResult, sizeof(DetectResult), &dwTokenID, 1);

				return true;
			}

			//构造结构
			DBR_GR_DetectClubName DetectClubName = {};
			
			//设置变量
			StringCchCopy(DetectClubName.szClubName, CountArray(DetectClubName.szClubName) ,pDetectClubName->szClubName);

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DETECT_CLUBNAME, pTokenParameter->dwUserID, dwSocketID, dwTokenID, &DetectClubName, sizeof(DetectClubName));

			return true;
		}		
		}
	}
	catch (...)
	{
		//构造提示
		TCHAR szDescribe[128]=TEXT("");
		_sntprintf_s(szDescribe,CountArray(szDescribe),TEXT("OnTCPNetworkMainClubPlazaService(SubCmdID=%d)"),wSubCmdID);

		//提示消息
		CTraceService::TraceString(szDescribe,TraceLevel_Warning);
	}

	return true;
}

//注册服务
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{		
	case SUB_CS_C_REGISTER_SERVICE:	//注册网关
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_CS_C_RegisterService));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterService)) return false;

			//消息定义
			auto pRegisterService =(CMD_CS_C_RegisterService *)pData;

			//校验模块
			if (pRegisterService->wServiceModule != SERVICE_MODULE_GATE) return false;

			//查找网关
			if (m_GlobalItemManager.SearchGateItem(pRegisterService->wServiceID)!=NULL)
			{
				//变量定义
				CMD_CS_S_RegisterFailure RegisterFailure;
				ZeroMemory(&RegisterFailure,sizeof(RegisterFailure));

				//设置变量
				RegisterFailure.lErrorCode=0;
				StringCchCopy(RegisterFailure.szDescribeString,CountArray(RegisterFailure.szDescribeString),TEXT("已经存在相同标识的网关服务，网关服务注册失败"));

				//发送消息
				WORD wStringSize=CountStringBuffer(RegisterFailure.szDescribeString);
				WORD wSendSize=sizeof(RegisterFailure)-sizeof(RegisterFailure.szDescribeString)+wStringSize;
				m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_FAILURE,&RegisterFailure,wSendSize,NULL,0);

				//中断网络
				m_pITCPNetworkEngine->ShutDownSocket(dwSocketID);

				return true;
			}

			//设置绑定
			WORD wBindIndex=LOWORD(dwSocketID);
			(m_pBindParameter+wBindIndex)->wServiceModule= SERVICE_MODULE_GATE;
			(m_pBindParameter+wBindIndex)->wServiceID= pRegisterService->wServiceID;

			//变量定义
			tagServiceItem ServiceItem;
			ZeroMemory(&ServiceItem,sizeof(ServiceItem));

			//构造数据
			ServiceItem.wServiceID= pRegisterService->wServiceID;
			ServiceItem.wServiceModule = SERVICE_MODULE_GATE;
			ServiceItem.wServicePort= pRegisterService->wServicePort;
			ServiceItem.wMaxConnect= pRegisterService->wMaxConnect;
			ServiceItem.dwServiceAddr= pRegisterService->dwServiceAddr;
			StringCchCopy(ServiceItem.szServiceName, CountArray(ServiceItem.szServiceName), pRegisterService->szServiceName);
			StringCchCopy(ServiceItem.szServiceDomain, CountArray(ServiceItem.szServiceDomain), pRegisterService->szServiceDomain);

			//注册房间
			m_GlobalItemManager.ActiveGateItem(dwSocketID, ServiceItem);

			//注册成功
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_SUCCESS,NULL,0);			

			//群发设置
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,0L);			

			return true;
		}
	}

	return false;
}

//服务状态
bool CAttemperEngineSink::OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{
	case SUB_CS_C_LOGOUT_TOKEN:
	{
		//设置绑定
		WORD wBindIndex = LOWORD(dwSocketID);
		auto pBindParameter = (m_pBindParameter + wBindIndex);

		//校验服务
		if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return true;

		//提取数据
		auto pLogoutToken = (CMD_CS_C_LogoutToken*)pData;

		//查找网关
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem != NULL)
		{
			//绑定参数
			auto pTokenParameter = pGlobalGateItem->SearchTokenID(pLogoutToken->dwTokenID);
			if (pTokenParameter != NULL)
			{
				//移除用户
				m_ClubUserManager.DeleteUserItem(pTokenParameter->dwUserID);				

				//构造结构
				CMD_CS_C_LeaveClubPlaza LeaveClubPlaza = {};

				//设置变量
				LeaveClubPlaza.dwUserID = pTokenParameter->dwUserID;

				//广播消息
				m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_LEAVE_CLUBPLAZA, &LeaveClubPlaza, sizeof(LeaveClubPlaza), 0);
			}			

			//移除令牌
			pGlobalGateItem->RemoveTokenID(pLogoutToken->dwTokenID);
		}

		return true;
	}
	case SUB_CS_C_REGISTER_TOKEN:
	{
		//设置绑定
		WORD wBindIndex = LOWORD(dwSocketID);
		auto pBindParameter = (m_pBindParameter + wBindIndex);

		//移除令牌
		if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return true;

		//提取数据
		auto pRegisterToken = (CMD_CS_C_RegisterToken*)pData;

		//构造结构
		CMD_CS_S_RegisterToken RegisterToken;
		ZeroMemory(&RegisterToken, sizeof(RegisterToken));

		//默认成功
		RegisterToken.lResultCode = REGISTER_RESULT_FAILURE;

		//查找网关
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem != NULL)
		{
			//查询用户
			auto pTokenParameter = pGlobalGateItem->ActiveTokenID(pRegisterToken->dwTokenID);
			if (pTokenParameter != NULL)
			{
				//注册成功
				RegisterToken.lResultCode = REGISTER_RESULT_SUCCESS;

				//设置令牌		
				pTokenParameter->dwUserID = pRegisterToken->dwUserID;
				pTokenParameter->dwClientIP = pRegisterToken->dwClientIP;
			}
		}

		//发送数据
		m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_REGISTER_TOKEN, &RegisterToken, sizeof(RegisterToken), &pRegisterToken->dwTokenID, 1);

		return true;
	}
	}
	return true;
}

//获取结构
tagForwardHead* CAttemperEngineSink::GetForwardHead()
{
	auto pForwardHead=(tagForwardHead*)m_cbBuffer;
	ZeroMemory(pForwardHead,sizeof(tagForwardHead));

	return pForwardHead;
}

//发送请求
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//发送数据
	auto pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->m_UIControlSink.PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//令牌参数
tagTokenParameter* CAttemperEngineSink::GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID)
{
	//获取信息
	WORD wBindIndex = LOWORD(dwSocketID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);
	if (pBindParameter == NULL) return NULL;

	//转发模式
	if (pBindParameter->wServiceModule == SERVICE_MODULE_GATE)
	{
		//查询网关
		auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem == NULL) return NULL;

		//查询用户
		auto pTokenParameter = pGlobalGateItem->SearchTokenID(dwTokenID);
		if (pTokenParameter == NULL || pTokenParameter->dwTokenID != dwTokenID) return NULL;

		return pTokenParameter;
	}

	return NULL;
}

//网关发送
bool CAttemperEngineSink::SendDataToGate(WORD wServerID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD *pdwTokenID, WORD wTokenCount)
{
	//查找网关
	auto pGlobalGateItem=m_GlobalItemManager.SearchGateItem(wServerID);
	if (pGlobalGateItem==NULL) return false;

	//发送数据
	m_pITCPNetworkEngine->SendData(pGlobalGateItem->GetSocketID(),MAKECMMD(wMainCmdID,SERVICE_MODULE_CLUBPLAZA),wSubCmdID,pData,wDataSize,pdwTokenID,wTokenCount);

	return true;
}

//登录成功
bool CAttemperEngineSink::OnDBLogonSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT (wDataSize>=sizeof(DBO_GR_LogonSuccess));
	if (wDataSize<sizeof(DBO_GR_LogonSuccess)) return false;

	//提取数据
	auto pLogonSuccess=(DBO_GR_LogonSuccess *)pData;

	//变量定义
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);

	//构造结构
	CMD_CS_C_EnterClubPlaza EnterClubPlaza = {};
	
	//设置变量
	EnterClubPlaza.dwUserID = pLogonSuccess->dwUserID;

	//广播消息
	m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_ENTER_CLUBPLAZA, &EnterClubPlaza, sizeof(EnterClubPlaza), 0);

	//登录成功
	SendDataToGate(pBindParameter->wServiceID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_LOGON_SUCCESS, NULL, 0, &dwTokenID, 1);

	//构造结构
	CMD_CP_S_ClubOption ClubOption;
	ZeroMemory(&ClubOption, sizeof(ClubOption));

	//拷贝数据
	CopyMemory(&ClubOption.ClubOption, &m_ClubOption, sizeof(tagClubOption));

	//发送数据
	SendDataToGate(pBindParameter->wServiceID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_OPTION, &ClubOption, sizeof(ClubOption), &dwTokenID, 1);	

	return true;
}

//登录失败
bool CAttemperEngineSink::OnDBLogonFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT (wDataSize<=sizeof(DBO_GR_LogonFailure));
	if (wDataSize>sizeof(DBO_GR_LogonFailure)) return false;

	//提取数据
	auto pLogonFailure=(DBO_GR_LogonFailure *)pData;

	//变量定义
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);

	//获取对象
	CMD_CP_S_LogonFailure LogonFailure;
	ZeroMemory(&LogonFailure,sizeof(LogonFailure));

	//设置变量			
	LogonFailure.lErrorCode=pLogonFailure->lErrorCode;			
	StringCchCopy(LogonFailure.szErrorDescribe,CountArray(LogonFailure.szErrorDescribe), pLogonFailure->szErrorDescribe);

	//发送数据
	WORD wPacketSize=CountStringBuffer(LogonFailure.szErrorDescribe);
	wPacketSize+=sizeof(LogonFailure)-sizeof(LogonFailure.szErrorDescribe);
	SendDataToGate(pBindParameter->wServiceID,MDM_CLUBPLAZA_SERVICE,SUB_CP_S_LOGON_FAILURE,&LogonFailure,wPacketSize,&dwTokenID,1);
	
	return true;
}

//登录完成
bool CAttemperEngineSink::OnDBLogonFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//变量定义
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);

	//发送数据
	SendDataToGate(pBindParameter->wServiceID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_LOGON_FINISH, NULL, 0, &dwTokenID, 1);

	return true;
}

//茶社列表
bool CAttemperEngineSink::OnDBClubNameCard(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//变量定义
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);

	//发送数据
	SendDataToGate(pBindParameter->wServiceID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_NAMECARD, pData, wDataSize, &dwTokenID, 1);

	return true;
}

//创建群组
bool CAttemperEngineSink::OnDBCreateClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT (wDataSize==sizeof(DBO_GR_CreateClub));
	if (wDataSize!=sizeof(DBO_GR_CreateClub)) return false;

	//提取数据
	auto pCreateClub=(DBO_GR_CreateClub *)pData;

	//激活路由
	CMD_CS_C_ActiveRouteItem ActiveRouteItem = {};

	//设置变量
	ActiveRouteItem.dwRouteID = pCreateClub->ClubNameCard.dwClubID;
	ActiveRouteItem.wServiceKind = 0;
	ActiveRouteItem.wServiceModule = SERVICE_MODULE_CLUB;

	//发送消息
	m_pITCPSocketCenter->SendData(MDM_CS_COMMON_SERVICE,SUB_CS_C_ACTIVE_ROUTE_ITEM,&ActiveRouteItem,sizeof(ActiveRouteItem),0);

	//查询用户
	auto pClubUserItem=m_ClubUserManager.SearchUserItem(pCreateClub->ClubNameCard.dwCreaterID);
	if (pClubUserItem !=NULL)
	{
		//网络信息
		auto pSocketInfo = pClubUserItem->GetSocketInfo();

		//发送数据
		SendDataToGate(pSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_NAMECARD, &pCreateClub->ClubNameCard, sizeof(pCreateClub->ClubNameCard), &pSocketInfo->dwTokenID,1);
	}

	return true;
}

//更新群组
bool CAttemperEngineSink::OnDBUpdateClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT(wDataSize >= sizeof(DBO_GR_UpdateClub));
	if (wDataSize < sizeof(DBO_GR_UpdateClub)) return false;

	//提取数据
	auto pDBUpdateClub = (DBO_GR_UpdateClub *)pData;

	//构造结构
	auto pForwardHead = GetForwardHead();

	//设置变量	
	pForwardHead->wItemCount = 1;
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CLUB;
	pForwardHead->ForwardItem[0].dwHashKeyID = pDBUpdateClub->dwClubID;

	//获取对象
	auto pUpdateClub = (CMD_CS_CB_UpdateClub *)(pForwardHead + 1);

	//设置变量
	pUpdateClub->dwClubID = pDBUpdateClub->dwClubID;
	pUpdateClub->cbUpdateKind = pDBUpdateClub->cbUpdateKind;
	CopyMemory(pUpdateClub + 1, pDBUpdateClub + 1, wDataSize - sizeof(DBO_GR_UpdateClub));

	//广播消息
	WORD wPacketSize = sizeof(tagForwardHead);
	wPacketSize += sizeof(CMD_CS_CB_UpdateClub) + wDataSize - sizeof(DBO_GR_UpdateClub);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_CB_UPDATE_CLUB, pForwardHead, wPacketSize, 0);

	return true;
}

//删除群组
bool CAttemperEngineSink::OnDBDeleteClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT (wDataSize == sizeof(DBO_GR_DeleteClub));
	if (wDataSize != sizeof(DBO_GR_DeleteClub)) return false;

	//提取数据
	auto pDBDeleteClub = (DBO_GR_DeleteClub *)pData;

	//构造结构
	auto pForwardHead = GetForwardHead();

	//设置变量	
	pForwardHead->wItemCount = 1;
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CLUB;
	pForwardHead->ForwardItem[0].dwHashKeyID = pDBDeleteClub->dwClubID;

	//获取对象
	auto pDeleteClub = (CMD_CS_CB_DeleteClub *)(pForwardHead + 1);
	ZeroMemory(pDeleteClub, sizeof(CMD_CS_CB_DeleteClub));

	//设置变量
	pDeleteClub->dwClubID = pDBDeleteClub->dwClubID;

	//广播消息
	WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_CB_DeleteClub);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_CB_DELETE_CLUB, pForwardHead, wPacketSize, 0);	

	return true;
}

//查找群组
bool CAttemperEngineSink::OnDBSearchClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT (wDataSize == sizeof(DBO_GR_SearchClub));
	if (wDataSize != sizeof(DBO_GR_SearchClub)) return false;

	//提取数据
	auto pSearchClub = (DBO_GR_SearchClub *)pData;
	
	//查询用户
	auto pClubUserItem = m_ClubUserManager.SearchUserItem(pSearchClub->dwUserID);
	if (pClubUserItem != NULL)
	{
		//构造结构
		CMD_CP_S_SearchResult SearchResult = {};		
		CopyMemory(&SearchResult.ClubNameCard, &pSearchClub->ClubNameCard, sizeof(SearchResult.ClubNameCard));

		//网络信息
		auto pSocketInfo = pClubUserItem->GetSocketInfo();

		//发送消息
		SendDataToGate(pSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_SEARCH_RESULT,&SearchResult,sizeof(SearchResult),&pSocketInfo->dwTokenID,1);
	}	

	return true;
}

//添加成员
bool CAttemperEngineSink::OnDBInsertMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT (wDataSize==sizeof(DBO_GR_InsertMember));
	if (wDataSize!=sizeof(DBO_GR_InsertMember)) return false;

	//提取数据
	auto pInsertMember=(DBO_GR_InsertMember *)pData;

	//构造结构
	auto pForwardHead=GetForwardHead();

	//构造头部
	pForwardHead->wItemCount = 1;
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CLUB;
	pForwardHead->ForwardItem[0].dwHashKeyID = pInsertMember->dwClubID;

	//获取对象
	auto pIMInsertMember=(CMD_CS_CB_InsertMember *)(pForwardHead+1);
	ZeroMemory(pIMInsertMember,sizeof(CMD_CS_CB_InsertMember));

	//设置变量
	pIMInsertMember->dwClubID=pInsertMember->dwClubID;
	pIMInsertMember->wMemberCount = pInsertMember->ClubNameCard.wMemberCount;
	CopyMemory(&pIMInsertMember->MemberUser,&pInsertMember->ClubMemberUser,sizeof(pIMInsertMember->MemberUser));

	//广播消息
	WORD wPacketSize=sizeof(tagForwardHead)+sizeof(CMD_CS_CB_InsertMember);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE,SUB_CS_CB_INSERT_MEMBER,pForwardHead,wPacketSize,0);

	//查询用户
	auto pClubUserItem=m_ClubUserManager.SearchUserItem(pInsertMember->ClubMemberUser.UserInfo.dwUserID);
	if (pClubUserItem !=NULL)
	{
		//网络信息
		auto pSocketInfo = pClubUserItem->GetSocketInfo();

		//发送数据
		SendDataToGate(pSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_NAMECARD, &pInsertMember->ClubNameCard, sizeof(pInsertMember->ClubNameCard), &pSocketInfo->dwTokenID, 1);
	}

	return true;
}

//更新成员
bool CAttemperEngineSink::OnDBUpdateMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT (wDataSize == sizeof(DBO_GR_UpdateMember));
	if (wDataSize != sizeof(DBO_GR_UpdateMember)) return false;

	//提取数据
	auto pUpdateMember = (DBO_GR_UpdateMember *)pData;

	//构造结构
	auto pForwardHead = GetForwardHead();

	//构造头部
	pForwardHead->wItemCount = 1;
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CLUB;
	pForwardHead->ForwardItem[0].dwHashKeyID = pUpdateMember->dwClubID;

	//获取对象
	auto pIMUpdateMember = (CMD_CS_CB_UpdateMember *)(pForwardHead + 1);
	ZeroMemory(pIMUpdateMember, sizeof(CMD_CS_CB_UpdateMember));

	//设置变量
	pIMUpdateMember->dwClubID = pUpdateMember->dwClubID;
	pIMUpdateMember->wUpdateKind = pUpdateMember->wUpdateKind;
	CopyMemory(&pIMUpdateMember->ClubMember, &pUpdateMember->ClubMember, sizeof(pIMUpdateMember->ClubMember));

	//广播消息
	WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_CB_UpdateMember);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_CB_UPDATE_MEMBER, pForwardHead, wPacketSize, 0);	

	return true;
}

//删除成员
bool CAttemperEngineSink::OnDBDeleteMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT (wDataSize==sizeof(DBO_GR_DeleteMember));
	if (wDataSize!=sizeof(DBO_GR_DeleteMember)) return false;

	//提取数据
	auto pDeleteMember=(DBO_GR_DeleteMember *)pData;

	//构造结构
	auto pForwardHead=GetForwardHead();

	//构造头部
	pForwardHead->wItemCount = 1;
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CLUB;
	pForwardHead->ForwardItem[0].dwHashKeyID = pDeleteMember->dwClubID;	

	//获取对象
	auto pIMDeleteMember=(CMD_CS_CB_DeleteMember *)(pForwardHead+1);
	ZeroMemory(pIMDeleteMember,sizeof(CMD_CS_CB_DeleteMember));

	//设置变量
	pIMDeleteMember->dwClubID=pDeleteMember->dwClubID;
	pIMDeleteMember->dwMemberID=pDeleteMember->dwMemberID;
	pIMDeleteMember->wMemberCount = pDeleteMember->wMemberCount;

	//广播消息
	WORD wPacketSize=sizeof(tagForwardHead)+sizeof(CMD_CS_CB_DeleteMember);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE,SUB_CS_CB_DELETE_MEMBER,pForwardHead,wPacketSize,0);

	//移除群组
	auto pServerUserItem = m_ClubUserManager.SearchUserItem(pDeleteMember->dwMemberID);
	if (pServerUserItem != NULL)
	{
		//获取对象		
		auto pSocketInfo = pServerUserItem->GetSocketInfo();

		//构造结构
		CMD_CP_S_ClubDelete  ClubDelete;
		ZeroMemory(&ClubDelete,sizeof(ClubDelete));

		//设置变量
		ClubDelete.dwClubID = pDeleteMember->dwClubID;

		//老板踢人
		if (pDeleteMember->cbDeleteKind == 1)
		{
			StringCchCopy(ClubDelete.szNotifyContent, CountArray(ClubDelete.szNotifyContent), CWHService::FormatMessageV(MIM_CLUBPLAZA_0010, TEXT("抱歉，您已被移出[%s]的茶社。"), pDeleteMember->szClubName));			
		}
		else
		{
			StringCchCopy(ClubDelete.szNotifyContent, CountArray(ClubDelete.szNotifyContent), CWHService::FormatMessageV(MIM_CLUBPLAZA_0011, TEXT("您已退出[%s]的茶社。"), pDeleteMember->szClubName));			
		}

		//发送数据
		WORD wPacketSize = CountStringBuffer(ClubDelete.szNotifyContent);
		wPacketSize += sizeof(ClubDelete) - sizeof(ClubDelete.szNotifyContent);
		SendDataToGate(pSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_CLUB_DELETE, &ClubDelete, wPacketSize, &pSocketInfo->dwTokenID, 1);						
	}

	return true;
}

//申请消息
bool CAttemperEngineSink::OnDBApplyMessage(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT (wDataSize>=sizeof(DBO_GR_ApplyMessage));
	if (wDataSize<sizeof(DBO_GR_ApplyMessage)) return false;

	//提取数据
	auto pDBApplyMessage=(DBO_GR_ApplyMessage *)pData;

	//申请者消息
	if (pDBApplyMessage->cbMessageType== APPLY_MSG_TYPE_REQUEST)
	{
		//申请用户
		auto pServerUserItem = m_ClubUserManager.SearchUserItem(pDBApplyMessage->dwUserID);
		if (pServerUserItem != NULL && pServerUserItem->GetUserStatus() == US_CB_ONLINE)
		{
			//转换对象		
			auto pSocketInfo = pServerUserItem->GetSocketInfo();

			//获取对象
			auto pApplyMessage = (CMD_CP_S_ApplyMessage *)m_cbBuffer;

			//设置变量
			pApplyMessage->cbMessageType = pDBApplyMessage->cbMessageType;
			pApplyMessage->wMessageCount = pDBApplyMessage->wMessageCount;

			//设置变量	
			CopyMemory(pApplyMessage + 1, pDBApplyMessage + 1, wDataSize - sizeof(DBO_GR_ApplyMessage));

			//发送数据
			WORD wPacketSize = sizeof(CMD_CP_S_ApplyMessage) + wDataSize - sizeof(DBO_GR_ApplyMessage);
			SendDataToGate(pSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_APPLY_MESSAGE, pApplyMessage, wPacketSize, &pSocketInfo->dwTokenID, 1);
		}
	}

	//响应者消息
	if (pDBApplyMessage->cbMessageType == APPLY_MSG_TYPE_RESPOND)
	{
		//构造结构
		auto pForwardHead = GetForwardHead();

		//设置变量	
		pForwardHead->wItemCount = 1;
		pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
		pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
		pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CLUB;
		pForwardHead->ForwardItem[0].dwHashKeyID = pDBApplyMessage->dwClubID;

		//获取对象
		auto pApplyMessage = (CMD_CS_CB_ApplyMessage *)(pForwardHead + 1);
		ZeroMemory(pApplyMessage, sizeof(CMD_CS_CB_ApplyMessage));

		//设置变量		
		pApplyMessage->dwClubID = pDBApplyMessage->dwClubID;		
		pApplyMessage->wMessageCount = pDBApplyMessage->wMessageCount;

		//设置变量	
		CopyMemory(pApplyMessage + 1, pDBApplyMessage + 1, wDataSize - sizeof(DBO_GR_ApplyMessage));

		//发送数据
		WORD wPacketSize = sizeof(tagForwardHead)+sizeof(CMD_CS_CB_ApplyMessage)+wDataSize-sizeof(DBO_GR_ApplyMessage);
		m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_CB_APPLY_MESSAGE, pForwardHead, wPacketSize, 0);
	}	

	return true;
}

//修改分数
bool CAttemperEngineSink::OnDBModifyScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT(wDataSize == sizeof(DBO_GR_ModifyScore));
	if (wDataSize != sizeof(DBO_GR_ModifyScore)) return false;

	//提取数据
	auto pDBOModifyScore = (DBO_GR_ModifyScore *)pData;

	//构造结构
	auto pForwardHead = GetForwardHead();

	//设置变量	
	pForwardHead->wItemCount = 1;
	pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
	pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
	pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CLUB;
	pForwardHead->ForwardItem[0].dwHashKeyID = pDBOModifyScore->dwClubID;

	//获取对象
	CMD_CS_CB_ModifyScore * pModifyScore = (CMD_CS_CB_ModifyScore *)(pForwardHead + 1);
	ZeroMemory(pModifyScore, sizeof(CMD_CS_CB_ModifyScore));

	//设置变量
	pModifyScore->wKindID = pDBOModifyScore->wKindID;
	pModifyScore->wServerID= pDBOModifyScore->wServerID;
	pModifyScore->dwClubID = pDBOModifyScore->dwClubID;
	pModifyScore->dwMemberID = pDBOModifyScore->dwMemberID;
	pModifyScore->dwPayUserID = pDBOModifyScore->dwPayUserID;
	pModifyScore->lMemberScore = pDBOModifyScore->lMemberScore;
	pModifyScore->lModifyScore = pDBOModifyScore->lModifyScore;		

	//广播消息
	WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_CB_ModifyScore);
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_CB_MODIFY_SCORE, pForwardHead, wPacketSize, 0);
	
	//更新财富
	auto pServerUserItem = m_ClubUserManager.SearchUserItem(pDBOModifyScore->dwPayUserID);
	if (pServerUserItem != NULL)
	{
		//转换对象		
		auto pSocketInfo = pServerUserItem->GetSocketInfo();

		//获取对象
		CMD_CP_S_OperateSuccess OperateSuccess;
		ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

		//设置变量		
		OperateSuccess.lOperateCode = 0;
		_snwprintf_s(OperateSuccess.szOperateNotice, CountArray(OperateSuccess.szOperateNotice), TEXT("%s: %s%I64d"), pDBOModifyScore->szNickName, pDBOModifyScore->lModifyScore >= 0 ? L"+" : L"", pDBOModifyScore->lModifyScore / SCORE_MULTIPLE);

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateSuccess.szOperateNotice);
		wPacketSize += sizeof(OperateSuccess) - sizeof(OperateSuccess.szOperateNotice);
		SendDataToGate(pSocketInfo->wGateID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_OPERATE_SUCCESS, &OperateSuccess, wPacketSize, &pSocketInfo->dwTokenID, 1);
	}	

	return true;
}

//群组配置
bool CAttemperEngineSink::OnDBClubOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT (wDataSize % sizeof(tagClubOption) == 0);
	if (wDataSize % sizeof(tagClubOption) != 0) return false;

	//变量定义
	auto pClubOption = (tagClubOption*)pData;
	WORD wOptionCount = wDataSize / sizeof(tagClubOption);	

	//移除配置
	m_ClubOption= *pClubOption;	

	return true;
}

//配置完成
bool CAttemperEngineSink::OnDBOptionFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//事件通知
	CP_ControlResult ControlResult;
	ControlResult.cbSuccess= ER_SUCCESS/*m_ClubOptionArray.GetCount()>0?ER_SUCCESS:ER_FAILURE*/;
	SendUIControlPacket(UI_LOAD_CLUB_OPTION,&ControlResult,sizeof(ControlResult));

	return true;
}

//禁止词汇
bool CAttemperEngineSink::OnDBForbidWords(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//开始加载
	if(dwContextID==0xFFFE)
	{
		//重置对象
		m_WordFilter.ResetFilter();

		//关闭定时器
		m_pITimerEngine->KillTimer(IDI_LOAD_FORBID_WORDS);

		return true;			
	}

	//变量定义
	WORD wBufferPos=0;
	BYTE cbWordsCount=0;
	BYTE * pBuffer=(LPBYTE)pData;
	TCHAR szKeyWords[32]=TEXT("");

	//提取数据
	while (wBufferPos<wDataSize)
	{
		cbWordsCount=*(BYTE *)&pBuffer[wBufferPos++];
		CopyMemory(szKeyWords,&pBuffer[wBufferPos],cbWordsCount);
		wBufferPos+=cbWordsCount;

		//添加关键词
		m_WordFilter.InsertKeyword(szKeyWords);
	}   

	return true;
}

//财富更新
bool CAttemperEngineSink::OnDBWealthUpdate(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//校验参数
	ASSERT (wDataSize<=sizeof(DBO_GR_WealthUpdate));
	if (wDataSize>sizeof(DBO_GR_WealthUpdate)) return false;

	//变量定义
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);
	if (pBindParameter->dwSocketID != dwContextID) return true;

	//提取数据
	auto pUserWealthUpdate=(DBO_GR_WealthUpdate *)pData;

	//定义变量
	auto pWealthUpdate = (CMD_CM_WealthUpdate*)m_cbBuffer;
	ZeroMemory(pWealthUpdate, sizeof(CMD_CM_WealthUpdate));

	//设置变量
	pWealthUpdate->wItemCount = pUserWealthUpdate->wItemCount;
	CopyMemory(pWealthUpdate->WealthItem, pUserWealthUpdate->WealthItem, pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]));

	//发送数据
	WORD wPacketSize = sizeof(CMD_CM_WealthUpdate) + pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]);
	SendDataToGate(pBindParameter->wServiceID, MDM_CM_SYSTEM, SUB_CM_WEALTH_UPDATE, pWealthUpdate, wPacketSize, &dwTokenID, 1);

	return true;
}

//删除结果
bool CAttemperEngineSink::OnDBApplyDelete(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//变量定义
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);
	if (pBindParameter->dwSocketID != dwContextID) return true;

	//校验参数
	ASSERT (wDataSize>=sizeof(DBO_GR_ApplyDelete));
	if (wDataSize<sizeof(DBO_GR_ApplyDelete)) return false;

	//提取数据
	auto pApplyDelete =(DBO_GR_ApplyDelete *)pData;

	//构造结果
	auto pCPApplyDelete = (CMD_CP_S_ApplyDelete*)m_cbBuffer;
	WORD wMessageSize = wDataSize - sizeof(DBO_GR_ApplyDelete);

	//消息数量
	pCPApplyDelete->wMessageCount = wMessageSize / sizeof(DWORD);

	//拷贝标识
	CopyMemory(pCPApplyDelete+1, pApplyDelete+1, wMessageSize);

	//发送数据	
	SendDataToGate(pBindParameter->wServiceID,MDM_CLUBPLAZA_SERVICE,SUB_CP_S_APPLY_DELETE, pCPApplyDelete, sizeof(CMD_CP_S_ApplyDelete)+wMessageSize,&dwTokenID,1);

	//响应类型
	if (pApplyDelete->cbMessageType == APPLY_MSG_TYPE_RESPOND)
	{
		//构造结构
		auto pForwardHead = GetForwardHead();

		//设置变量	
		pForwardHead->wItemCount = 1;
		pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
		pForwardHead->ForwardItem[0].wServiceID = INVALID_WORD;
		pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_CLUB;
		pForwardHead->ForwardItem[0].dwHashKeyID = pApplyDelete->dwClubID;

		//获取对象
		auto pFDApplyDelete = (CMD_CS_CB_ApplyDelete*)(pForwardHead + 1);

		//设置变量
		pFDApplyDelete->dwUserID = pApplyDelete->dwUserID;
		pFDApplyDelete->dwClubID = pApplyDelete->dwClubID;
		pFDApplyDelete->wMessageCount = wMessageSize /sizeof(DWORD);

		//拷贝标识
		CopyMemory(pFDApplyDelete+1, pApplyDelete+1, wMessageSize);

		//发送数据
		WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_CB_ApplyDelete) + wMessageSize;
		m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_CB_APPLY_DELETE, pForwardHead, wPacketSize, 0);
	}

	return true;
}

//检测名称
bool CAttemperEngineSink::OnDBDetectClubName(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//变量定义
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);
	if (pBindParameter->dwSocketID != dwContextID) return true;

	//提取数据
	auto pDetectClubName = (DBO_GR_DetectClubName*)pData;

	//获取对象
	CMD_CP_S_DetectResult DetectResult = {};
	
	//设置变量		
	DetectResult.bUsable = pDetectClubName->bUsable;
	StringCchCopy(DetectResult.szErrorDescribe,CountArray(DetectResult.szErrorDescribe), pDetectClubName->szErrorDescribe);

	//发送数据
	SendDataToGate(pBindParameter->wServiceID, MDM_CLUBPLAZA_SERVICE, SUB_CP_S_DETECT_RESULT, &DetectResult, sizeof(DetectResult), &dwTokenID, 1);

	return true;
}

//操作成功
bool CAttemperEngineSink::OnDBOperateSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//变量定义
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);
	if (pBindParameter->dwSocketID != dwContextID) return true;

	//校验参数
	ASSERT (wDataSize<=sizeof(DBO_GR_OperateSuccess));
	if (wDataSize>sizeof(DBO_GR_OperateSuccess)) return false;	

	//提取数据
	auto pOperateSuccess=(DBO_GR_OperateSuccess *)pData;

	//获取对象
	//CMD_CP_S_OperateSuccess OperateSuccess;
	//ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

	////设置变量		
	//OperateSuccess.lOperateCode=pOperateSuccess->lOperateCode;		
	//StringCchCopy(OperateSuccess.szOperateNotice,CountArray(OperateSuccess.szOperateNotice), pOperateSuccess->szOperateNotice);

	////发送数据
	//WORD wPacketSize=CountStringBuffer(OperateSuccess.szOperateNotice);
	//wPacketSize+=sizeof(OperateSuccess)-sizeof(OperateSuccess.szOperateNotice);
	//SendDataToGate(pBindParameter->wServiceID,MDM_CLUBPLAZA_SERVICE,SUB_CP_S_OPERATE_SUCCESS,&OperateSuccess,wPacketSize,&dwTokenID,1);

	//变量定义
	auto pRequestSuccess = (CMD_CM_OperateSuccess*)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer + sizeof(CMD_CM_OperateSuccess), sizeof(m_cbBuffer) - sizeof(CMD_CM_OperateSuccess));

	//设置变量
	pRequestSuccess->wRequestCmdID = 0;
	pRequestSuccess->lErrorCode = pOperateSuccess->lOperateCode;	

	//叠加信息
	if (pOperateSuccess->szOperateNotice[0] != 0)  SendPacket.AddPacket(pOperateSuccess->szOperateNotice, DTP_CM_DESCRIBE);

	//发送完成
	WORD wPacketSize = sizeof(CMD_CM_OperateSuccess) + SendPacket.GetDataSize();
	SendDataToGate(pBindParameter->wServiceID, MDM_CM_SYSTEM, SUB_CM_OPERATE_SUCCESS, pRequestSuccess, wPacketSize, &dwTokenID, 1);
	
	return true;
}

//操作失败
bool CAttemperEngineSink::OnDBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//变量定义
	WORD wBindIndex = LOWORD(dwContextID);
	auto pBindParameter = (m_pBindParameter + wBindIndex);
	if (pBindParameter->dwSocketID != dwContextID) return true;

	//校验参数
	ASSERT (wDataSize<=sizeof(DBO_GR_OperateFailure));
	if (wDataSize>sizeof(DBO_GR_OperateFailure)) return false;	

	//提取数据
	auto pOperateFailure=(DBO_GR_OperateFailure *)pData;

	//获取对象
	//CMD_CP_S_OperateFailure OperateFailure;
	//ZeroMemory(&OperateFailure,sizeof(OperateFailure));

	////设置变量		
	//OperateFailure.lErrorCode=pOperateFailure->lErrorCode;		
	//StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), pOperateFailure->szErrorDescribe);

	////发送数据
	//WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
	//wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
	//SendDataToGate(pBindParameter->wServiceID,MDM_CLUBPLAZA_SERVICE,SUB_CP_S_OPERATE_FAILURE,&OperateFailure,wPacketSize,&dwTokenID,1);

	//变量定义
	auto pRequestFailure = (CMD_CM_OperateFailure*)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer + sizeof(CMD_CM_OperateFailure), sizeof(m_cbBuffer) - sizeof(CMD_CM_OperateFailure));

	//设置变量
	pRequestFailure->wRequestCmdID = 0;
	pRequestFailure->lErrorCode = pOperateFailure->lErrorCode;	

	//叠加信息
	if (pOperateFailure->szErrorDescribe[0] != 0)  SendPacket.AddPacket(pOperateFailure->szErrorDescribe, DTP_CM_DESCRIBE);

	//发送完成
	WORD wPacketSize = sizeof(CMD_CM_OperateFailure) + SendPacket.GetDataSize();
	SendDataToGate(pBindParameter->wServiceID, MDM_CM_SYSTEM, SUB_CM_OPERATE_FAILURE, pRequestFailure, wPacketSize, &dwTokenID, 1);
		
	return true;
}

//////////////////////////////////////////////////////////////////////////

