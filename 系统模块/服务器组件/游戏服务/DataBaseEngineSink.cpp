#include "StdAfx.h"
#include "ServiceUnits.h"
#include "DataBaseEngineSink.h"

//////////////////////////////////////////////////////////////////////////

//输出信息
void TraceDatabaseException(char* pFile, char* pFunction, int nLine, IDataBaseException* pIException)
{
	CString strMessage;
	strMessage.Format(TEXT("%s(%d):%hs(%s)"), CA2CT(pFile).m_psz, nLine, pFunction, pIException ? pIException->GetExceptionDescribe() : TEXT(""));
	CTraceService::TraceString(strMessage, TraceLevel_Exception);
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//变量定义
	m_nSyncIndex = -1;

	//系统配置
	m_pInitParameter=NULL;
	m_pGameServiceAttrib=NULL;
	m_pGameServiceOption=NULL;

	//扩展接口
	m_pIExtendDataBaseSink = NULL;	

	//组件变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pIDataBaseEngineEvent=NULL;	
	m_pAttemperEngineSink = NULL;	
	
	//辅助变量
	ZeroMemory(&m_LogonFailure,sizeof(m_LogonFailure));
	ZeroMemory(&m_LogonSuccess,sizeof(m_LogonSuccess));

	return;
}

//析构函数
CDataBaseEngineSink::~CDataBaseEngineSink()
{
}

//接口查询
VOID * CDataBaseEngineSink::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IDataBaseEngineSink,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IDataBaseEngineSink,Guid,dwQueryVer);
	return NULL;
}

//启动事件
bool CDataBaseEngineSink::OnDataBaseEngineStart(IUnknownEx * pIUnknownEx)
{
	//创建对象
	if ((m_PlatformDBModule.GetInterface()==NULL)&&(m_PlatformDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_AccountsDBModule.GetInterface() == NULL) && (m_AccountsDBModule.CreateInstance() == false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_GameDBModule.GetInterface()==NULL)&&(m_GameDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	try
	{
		//变量定义
		tagDataBaseParameter * pPlatformDBParameter=&m_pInitParameter->m_PlatformDBParameter;
		tagDataBaseParameter * pAccountsDBParameter=&m_pInitParameter->m_AccountsDBParameter;

		//连接信息
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->dwDataBaseAddr,pPlatformDBParameter->wDataBasePort,
		pPlatformDBParameter->szDataBaseName,pPlatformDBParameter->szDataBaseUser,pPlatformDBParameter->szDataBasePass);	

		//连接信息
		m_AccountsDBModule->SetConnectionInfo(pAccountsDBParameter->dwDataBaseAddr, pAccountsDBParameter->wDataBasePort,
			pAccountsDBParameter->szDataBaseName, pAccountsDBParameter->szDataBaseUser, pAccountsDBParameter->szDataBasePass);

		//发起连接
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

		//发起连接
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());

		//连接信息
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wDBListID"),m_pGameServiceOption->wDBListID);
		if (m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadDataBaseInfo"),true)==DB_SUCCESS)
		{
			//变量定义
			TCHAR szDBUser[32]=TEXT(""),szDBPassword[32]=TEXT("");
			TCHAR szDBUserRead[512]=TEXT(""),szDBPassRead[512]=TEXT("");
			TCHAR szDBName[32] = TEXT("");
			TCHAR szDBAddr[16] = TEXT("");

			//获取信息
			WORD wDBPort=m_PlatformDBAide.GetValue_WORD(TEXT("DBPort"));
			m_PlatformDBAide.GetValue_String(TEXT("DBUser"),szDBUserRead,CountArray(szDBUserRead));
			m_PlatformDBAide.GetValue_String(TEXT("DBPassword"),szDBPassRead,CountArray(szDBPassRead));
			m_PlatformDBAide.GetValue_String(TEXT("DataBaseName"),szDBName,CountArray(szDBName));
			m_PlatformDBAide.GetValue_String(TEXT("DataBaseAddr"),szDBAddr,CountArray(szDBAddr));
					
			//解密密文
			CXOREncrypt::CrevasseData(szDBUserRead,szDBUser,CountArray(szDBUser));
			CXOREncrypt::CrevasseData(szDBPassRead,szDBPassword,CountArray(szDBPassword));

			//连接数据库
			m_GameDBModule->SetConnectionInfo(szDBAddr,wDBPort,szDBName,szDBUser,szDBPassword);

			//发起连接
			m_GameDBModule->OpenConnection();
			m_GameDBAide.SetDataBase(m_GameDBModule.GetInterface());
		}
		else
		{
			//构造信息
			TCHAR szErrorDescribe[128]=TEXT("");
			m_PlatformDBAide.GetValue_String(TEXT("ErrorDescribe"),szErrorDescribe,CountArray(szErrorDescribe));

			//提示消息
			CTraceService::TraceString(szErrorDescribe,TraceLevel_Exception);

			return false;
		}

		//游戏钩子
		if (m_pIGameDataBaseSink != NULL)
		{
			//变量定义
			tagGameDBSinkParameter GameDBSinkParameter;
			ZeroMemory(&GameDBSinkParameter, sizeof(GameDBSinkParameter));

			//平台数据库
			GameDBSinkParameter.pPlatformDBAide = &m_PlatformDBAide;
			GameDBSinkParameter.pIPlatformDBModule = m_PlatformDBModule.GetInterface();

			//配置变量
			GameDBSinkParameter.pGameServiceAttrib = m_pGameServiceAttrib;
			GameDBSinkParameter.pGameServiceOption = m_pGameServiceOption;

			//组件变量
			GameDBSinkParameter.pITimerEngine = m_pITimerEngine;
			GameDBSinkParameter.pIDataBaseEngine = m_pIDataBaseEngine;
			GameDBSinkParameter.pIDataBaseEngineEvent = m_pIDataBaseEngineEvent;

			//初始化
			m_pIGameDataBaseSink->InitGameDataBaseSink(GameDBSinkParameter);
		}

		//扩展钩子
		if (m_pIExtendDataBaseSink != NULL)
		{
			//变量定义
			tagExtendDBSinkParameter ExtendDBSinkParameter;
			ZeroMemory(&ExtendDBSinkParameter, sizeof(ExtendDBSinkParameter));

			//游戏数据库
			ExtendDBSinkParameter.pGameDBAide = &m_GameDBAide;
			ExtendDBSinkParameter.pIGameDBModule = m_GameDBModule.GetInterface();

			//账号数据库
			ExtendDBSinkParameter.pAccountsDBAide = &m_AccountsDBAide;
			ExtendDBSinkParameter.pIAccountsDBModule = m_AccountsDBModule.GetInterface();

			//平台数据库
			ExtendDBSinkParameter.pPlatformDBAide = &m_PlatformDBAide;
			ExtendDBSinkParameter.pIPlatformDBModule = m_PlatformDBModule.GetInterface();

			//配置变量
			ExtendDBSinkParameter.pGameServiceAttrib = m_pGameServiceAttrib;
			ExtendDBSinkParameter.pGameServiceOption = m_pGameServiceOption;

			//组件变量
			ExtendDBSinkParameter.pITimerEngine = m_pITimerEngine;
			ExtendDBSinkParameter.pIDataBaseEngine = m_pIDataBaseEngine;
			ExtendDBSinkParameter.pIDataBaseEngineEvent = m_pIDataBaseEngineEvent;

			//初始化
			m_pIExtendDataBaseSink->InitExtendDataBaseSink(ExtendDBSinkParameter);
		}		
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
		return false;
	}

	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	//解锁机器
	UnlockRobotUser();

	//组件变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pIDataBaseEngineEvent=NULL;
	m_pAttemperEngineSink = NULL;
	
	//关闭连接
	if (m_GameDBModule.GetInterface()!=NULL)
	{
		m_GameDBModule->CloseConnection();
		m_GameDBModule.CloseInstance();
	}

	//关闭连接
	if (m_AccountsDBModule.GetInterface() != NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
	}

	//关闭连接
	if (m_PlatformDBModule.GetInterface()!=NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
	}

	//释放接口
	if (m_pIGameDataBaseSink != NULL)
	{
		SafeRelease(m_pIGameDataBaseSink);
	}

	//释放接口
	if (m_pIExtendDataBaseSink != NULL)
	{
		SafeRelease(m_pIExtendDataBaseSink);
	}		

	return true;
}

//时间事件
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//游戏钩子
	if (m_pIGameDataBaseSink != NULL)
	{
		m_pIGameDataBaseSink->OnDataBaseEngineTimer(dwTimerID, dwBindParameter);
	}
	

	//扩展钩子
	if (m_pIExtendDataBaseSink != NULL)
	{
		m_pIExtendDataBaseSink->OnDataBaseEngineTimer(dwTimerID, dwBindParameter);
	}

	return true;
}

//控制事件
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	//扩展处理
	if (m_pIGameDataBaseSink != NULL)
	{
		m_pIGameDataBaseSink->OnDataBaseEngineControl(wControlID, pData, wDataSize);
	}

	//扩展处理
	if (m_pIExtendDataBaseSink != NULL)
	{
		m_pIExtendDataBaseSink->OnDataBaseEngineControl(wControlID, pData, wDataSize);
	}

	return true;
}

//请求事件
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//游戏请求
	if (wRequestID > DB_GAME_REQUEST_START && wRequestID <= DB_GAME_REQUEST_START + DB_GAME_IDENTIFY_RANGE)
	{
		if (m_pIGameDataBaseSink)
		{
			return m_pIGameDataBaseSink->OnDataBaseEngineRequest(wRequestID, dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
	}

	//扩展请求
	if (wRequestID > DB_EXTEND_REQUEST_START && wRequestID <= DB_EXTEND_REQUEST_START + DB_EXTEND_IDENTIFY_RANGE)
	{
		if (m_pIExtendDataBaseSink)
		{
			return m_pIExtendDataBaseSink->OnDataBaseEngineRequest(wRequestID, dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
	}

	//框架请求
	if (wRequestID > DB_FRAME_REQUEST_START && wRequestID <= DB_FRAME_REQUEST_START + DB_FRAME_IDENTIFY_RANGE)
	{
		switch (wRequestID)
		{
		case DBR_GR_LOAD_ROBOTS:			//加载机器
		{
			return OnRequestLoadRobots(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_LOAD_CONFIG:			//加载配置
		{
			return OnRequestLoadConfig(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_RECORD_DRAW_INFO:		//游戏记录
		{
			return OnRequestRecordDrawInfo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_RECORD_DRAW_SCORE:		//游戏记录
		{
			return OnRequestRecordDrawScore(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_RECORD_GAME_VIDEO:		//游戏录像
		{
			return OnRequestRecordGameVideo(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_MANAGE_USER_RIGHT:		//用户权限管理
		{
			return OnRequestManageUserRight(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_SEND_GIFT:				//赠送礼物
		{
			return OnRequestSendGift(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_UPDATE_LOVELINESS:		//更新魅力
		{
			return OnRequestUpdateLoveLiness(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_LOGON_COMPUTER:			//电脑登录
		{
			return OnRequestLogonComputer(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_LOGON_MOBILE:			//手机登录
		{
			return OnRequestLogonMobile(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_LOGON_ROBOT:			//机器登录
		{
			return OnRequestLogonRobot(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_WRITE_GAME_SCORE:		//游戏写分
		{
			return OnRequestWriteGameScore(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_WRITE_USER_WEALTH:		//用户财富
		{
			return OnRequestWriteUserWealth(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_LARGESS_SUBSIDY:		//赠送低保
		{
			return OnRequestLargessSubsidy(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_LEAVE_SERVICE:			//离开房间
		{
			return OnRequestLeaveService(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_CONTROL_QUERY:			//控制查询
		{
			return OnRequestControlQuery(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_CONTROL_START:			//控制开始
		{
			return OnRequestControlStart(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_CONTROL_FINISH:			//控制结束
		{
			return OnRequestControlFinish(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_CONTROL_UPDATE:			//控制更新
		{
			return OnRequestControlUpdate(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_USER_QUERY_GOODS:		//查询物品
		{
			return OnRequestUserQueryGoods(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_USER_UPDATE_GOODS:		//更新物品
		{
			return OnRequestUserUpdateGoods(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		case DBR_GR_USER_RECORD_GOODS:		//记录物品
		{
			return OnRequestUserRecordGoods(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
		}
		}
	}	

	return true;
}

//电脑登录
bool CDataBaseEngineSink::OnRequestLogonComputer(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize>=sizeof(DBR_GR_LogonComputer));
		if (wDataSize<sizeof(DBR_GR_LogonComputer)) return false;

		//效验连接
		ASSERT(m_pAttemperEngineSink);
		if ((m_pAttemperEngineSink!=NULL)&&(m_pAttemperEngineSink->EfficacySocketContext(dwContextID,dwTokenID)==false))
		{
			return true;
		}

		//执行查询
		DBR_GR_LogonComputer * pLogonComputer =(DBR_GR_LogonComputer *)pData;
		ZeroMemory(&m_LogonSuccess,sizeof(m_LogonSuccess));

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
 		BYTE * pClientIP=(BYTE *)&pLogonComputer->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"), pLogonComputer->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@szPassword"), pLogonComputer->szPassword);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@dwRequestScore"), pLogonComputer->lRequestScore);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"), pLogonComputer->szMachineID);

		//比赛参数
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		{
			//设置参数
			m_GameDBAide.AddParameter(TEXT("@MatchID"), pLogonComputer->dwMatchID);
			m_GameDBAide.AddParameter(TEXT("@MatchNo"), pLogonComputer->lMatchNo);
		}

		//输出参数
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);

		//执行查询
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyComputer"),true);

		//比赛信息
		m_LogonSuccess.lMatchNo= pLogonComputer->lMatchNo;
		m_LogonSuccess.dwMatchID= pLogonComputer->dwMatchID;

		//机器序列
		StringCchCopy(m_LogonSuccess.szMachineID, LEN_MACHINE_ID, pLogonComputer->szMachineID);
	
		//结果处理
		OnLogonDisposeResult(dwContextID,dwTokenID,lReturnValue,false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//错误处理
		OnLogonDisposeDBError(dwContextID,dwTokenID,pIException);

		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}
	catch(...)
	{
		//错误处理
		OnLogonDisposeDBError(dwContextID,dwTokenID,NULL);

		return false;
	}

	return true;
}

//I D 登录
bool CDataBaseEngineSink::OnRequestLogonMobile(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize>=sizeof(DBR_GR_LogonMobile));
		if (wDataSize<sizeof(DBR_GR_LogonMobile)) return false;

		//效验连接
		ASSERT(m_pAttemperEngineSink);
		if ((m_pAttemperEngineSink != NULL) && (m_pAttemperEngineSink->EfficacySocketContext(dwContextID, dwTokenID) == false))
		{
			return true;
		}

		//提取数据
		DBR_GR_LogonMobile * pLogonMobile=(DBR_GR_LogonMobile *)pData;		
		ZeroMemory(&m_LogonSuccess, sizeof(m_LogonSuccess));

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientAddr=(BYTE *)&pLogonMobile->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientAddr[0],pClientAddr[1],pClientAddr[2],pClientAddr[3]);

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pLogonMobile->dwUserID);		
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@dwRequestScore"),pLogonMobile->lRequestScore);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"), pLogonMobile->szMachineID);
		m_GameDBAide.AddParameter(TEXT("@dwDeviceKind"), pLogonMobile->dwDeviceKind);

		//比赛参数
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		{
			//设置参数
			m_GameDBAide.AddParameter(TEXT("@MatchID"),pLogonMobile->dwMatchID);
			m_GameDBAide.AddParameter(TEXT("@MatchNo"),pLogonMobile->lMatchNo);
		}

		//约战参数
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE) != 0)
		{
			m_GameDBAide.AddParameter(TEXT("@dwClubID"), pLogonMobile->dwClubID);
			m_GameDBAide.AddParameter(TEXT("@dwBattleNum"), pLogonMobile->dwBattleNum);			
			m_GameDBAide.AddParameter(TEXT("@cbScoreKind"), pLogonMobile->cbScoreKind);
		}

		//排位参数
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_RANKING) != 0)
		{
			m_GameDBAide.AddParameter(TEXT("@dwRankingNum"), pLogonMobile->dwRankingNum);
		}

		//输出参数
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,sizeof(m_LogonFailure.szDescribeString),adParamOutput);	

		//执行查询
		LONG lResultCode=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyMobile"),true);

		//手机定义
		m_LogonSuccess.dwDeviceKind=pLogonMobile->dwDeviceKind;		

		//位置信息
		m_LogonSuccess.dGlobalPosLat=pLogonMobile->dGlobalPosLat;
		m_LogonSuccess.dGlobalPosLng=pLogonMobile->dGlobalPosLng;
		CopyMemory(m_LogonSuccess.szPlaceName, pLogonMobile->szPlaceName, sizeof(m_LogonSuccess.szPlaceName));

		//比赛信息
		m_LogonSuccess.lMatchNo=pLogonMobile->lMatchNo;
		m_LogonSuccess.dwMatchID=pLogonMobile->dwMatchID;	

		//桌子信息
		m_LogonSuccess.bLookon = pLogonMobile->bLookon;
		m_LogonSuccess.wTableID= pLogonMobile->wTableID;
		m_LogonSuccess.wChairID = pLogonMobile->wChairID;
		StringCchCopy(m_LogonSuccess.szTablePasswd, CountArray(m_LogonSuccess.szTablePasswd), pLogonMobile->szTablePasswd);

		//用户信息
		StringCchCopy(m_LogonSuccess.szMachineID,CountArray(m_LogonSuccess.szMachineID), pLogonMobile->szMachineID);
	
		//结果处理
		OnLogonDisposeResult(dwContextID,dwTokenID,lResultCode,true);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//错误处理
		OnLogonDisposeDBError(dwContextID,dwTokenID,pIException);

		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(),TraceLevel_Exception);

		return false;
	}
	catch(...)
	{
		//错误处理
		OnLogonDisposeDBError(dwContextID,dwTokenID,NULL);

		return false;
	}

	return true;
}

//机器登录
bool CDataBaseEngineSink::OnRequestLogonRobot(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize >= sizeof(DBR_GR_LogonRobot));
		if (wDataSize < sizeof(DBR_GR_LogonRobot)) return false;

		//效验连接
		ASSERT (m_pAttemperEngineSink);
		if ((m_pAttemperEngineSink != NULL) && (m_pAttemperEngineSink->EfficacySocketContext(dwContextID,dwTokenID) == false))
		{
			return true;
		}

		//执行查询
		DBR_GR_LogonRobot * pLogonRobot = (DBR_GR_LogonRobot *)pData;
		ZeroMemory(&m_LogonSuccess, sizeof(m_LogonSuccess));

		//转化地址
		TCHAR szClientIP[16] = TEXT("");
		BYTE * pClientIP = (BYTE *)&pLogonRobot->dwClientIP;
		_sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"), pLogonRobot->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@dwRequestScore"), pLogonRobot->lRequestScore);
		m_GameDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);
		m_GameDBAide.AddParameter(TEXT("@strMachineID"), pLogonRobot->szMachineID);

		//比赛参数
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH) != 0)
		{
			//设置参数
			m_GameDBAide.AddParameter(TEXT("@MatchID"), pLogonRobot->dwMatchID);
			m_GameDBAide.AddParameter(TEXT("@MatchNo"), pLogonRobot->lMatchNo);
		}

		//输出参数
		m_GameDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), m_LogonFailure.szDescribeString, sizeof(m_LogonFailure.szDescribeString), adParamOutput);

		//执行查询
		LONG lReturnValue = m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyRobot"), true);

		//比赛信息
		m_LogonSuccess.lMatchNo = pLogonRobot->lMatchNo;
		m_LogonSuccess.dwMatchID = pLogonRobot->dwMatchID;		

		//机器序列
		StringCchCopy(m_LogonSuccess.szMachineID, LEN_MACHINE_ID, pLogonRobot->szMachineID);

		//结果处理
		OnLogonDisposeResult(dwContextID, dwTokenID, lReturnValue, false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//错误处理
		OnLogonDisposeDBError(dwContextID, dwTokenID, pIException);

		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		return false;
	}
	catch (...)
	{
		//错误处理
		OnLogonDisposeDBError(dwContextID, dwTokenID, NULL);

		return false;
	}

	return true;
}

//游戏写分
bool CDataBaseEngineSink::OnRequestWriteGameScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_WriteGameScore));
		if (wDataSize!=sizeof(DBR_GR_WriteGameScore)) return false;

		//请求处理
		DBR_GR_WriteGameScore * pWriteGameScore=(DBR_GR_WriteGameScore *)pData;

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pWriteGameScore->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pWriteGameScore->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@lScore"),pWriteGameScore->lScore);
		m_GameDBAide.AddParameter(TEXT("@lRevenue"),pWriteGameScore->lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lWinCount"),pWriteGameScore->lWinCount);
		m_GameDBAide.AddParameter(TEXT("@lLostCount"),pWriteGameScore->lLostCount);
		m_GameDBAide.AddParameter(TEXT("@lDrawCount"),pWriteGameScore->lDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lFleeCount"),pWriteGameScore->lFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lWinStreak"), pWriteGameScore->lWinStreak);
		m_GameDBAide.AddParameter(TEXT("@lMaxWinStreak"), pWriteGameScore->lMaxWinStreak);
		m_GameDBAide.AddParameter(TEXT("@lExperience"),pWriteGameScore->lExperience);
		m_GameDBAide.AddParameter(TEXT("@lUnLockScore"),pWriteGameScore->lUnLockScore);
		m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"),pWriteGameScore->dwPlayTimeCount);

		//其他参数
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);

		//约战游戏
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
		{
			m_GameDBAide.AddParameter(TEXT("@dwInoutIndex"), pWriteGameScore->dwInoutIndex);			
		}

		//比赛游戏
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH)!=0)
		{
			m_GameDBAide.AddParameter(TEXT("@dwMatchID"),pWriteGameScore->dwMatchID);
			m_GameDBAide.AddParameter(TEXT("@lMatchNo"),pWriteGameScore->lMatchNo);
		}

		//执行查询
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_WriteGameScore"),false);

		return lReturnValue==DB_SUCCESS;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//用户财富
bool CDataBaseEngineSink::OnRequestWriteUserWealth(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//执行查询
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GR_WriteGameWealth));
		if (wDataSize != sizeof(DBR_GR_WriteGameWealth)) return false;

		//请求处理
		DBR_GR_WriteGameWealth* pWriteGameWealth = (DBR_GR_WriteGameWealth*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();

		//用户信息
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pWriteGameWealth->dwUserID);

		//变更信息
		m_AccountsDBAide.AddParameter(TEXT("@wMoneyID"), pWriteGameWealth->wMoneyID);
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), pWriteGameWealth->wReason);
		m_AccountsDBAide.AddParameter(TEXT("@lVariationScore"), pWriteGameWealth->lVariationScore);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_WriteGameWealth"), true);

		//执行成功
		if (lResultCode == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_UserWealthUpdate UserWealthUpdate;
			ZeroMemory(&UserWealthUpdate, sizeof(UserWealthUpdate));

			//设置变量
			UserWealthUpdate.wMoneyID = pWriteGameWealth->wMoneyID;
			UserWealthUpdate.dwUserID = pWriteGameWealth->dwUserID;
			UserWealthUpdate.lBalanceAmount = m_AccountsDBAide.GetValue_LONGLONG(TEXT("BalanceAmount"));

			//发送数据			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE, dwContextID, dwTokenID, &UserWealthUpdate, sizeof(UserWealthUpdate));
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__,__FUNCTION__, __LINE__, pIException);	

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return false;
}

//赠送低保
bool CDataBaseEngineSink::OnRequestLargessSubsidy(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_LargessSubsidy));
		if (wDataSize!=sizeof(DBR_GR_LargessSubsidy)) return false;

		//请求处理
		DBR_GR_LargessSubsidy * pLargessSubsidy=(DBR_GR_LargessSubsidy *)pData;

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pLargessSubsidy->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pLargessSubsidy->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwClientIP"),pLargessSubsidy->dwClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@szClientIP"),szClientIP);
		m_AccountsDBAide.AddParameter(TEXT("@szMachine"),pLargessSubsidy->szMachineID);

		//输出参数
		TCHAR szDescribeString[128]=TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行查询
		LONG lReturnValue= m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_LargessSubsidy"),true);

		//变量定义
		DBO_GR_LargessSubsidy LargessSubsidy;
		ZeroMemory(&LargessSubsidy,sizeof(LargessSubsidy));

		//设置变量
		LargessSubsidy.bResultCode=lReturnValue==DB_SUCCESS;
		LargessSubsidy.dwUserID=pLargessSubsidy->dwUserID;

		//执行成功
		if (LargessSubsidy.bResultCode==true)
		{
			LargessSubsidy.lCurrGold= m_AccountsDBAide.GetValue_LONGLONG(TEXT("CurrGold"));
			LargessSubsidy.cbResidualTimes= m_AccountsDBAide.GetValue_BYTE(TEXT("ResidualTimes"));
		}
		else
		{
			//错误描述
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);
			StringCchCopy(LargessSubsidy.szDescribeString,CountArray(LargessSubsidy.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);
		}

		//发送结果
		WORD wStringSize=CountStringBuffer(LargessSubsidy.szDescribeString);
		WORD wSendDataSize=sizeof(LargessSubsidy)-sizeof(LargessSubsidy.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LARGESS_SUBSIDY,dwContextID,dwTokenID,&LargessSubsidy,wSendDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//查询物品
bool CDataBaseEngineSink::OnRequestUserQueryGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//提取数据
		DBR_GR_UserQueryGoods* pUserQueryGoods = (DBR_GR_UserQueryGoods*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUserQueryGoods->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@szGoodsIDList"), pUserQueryGoods->szGoodsIDList);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserQueryGoods"), true);

		//执行成功
		if (lResultCode == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_UserGoodsUpdate UserGoodsUpdate;
			ZeroMemory(&UserGoodsUpdate, sizeof(UserGoodsUpdate));

			//循环读取
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				//物品信息
				UserGoodsUpdate.GoodsHoldInfo[UserGoodsUpdate.wGoodsCount].wGoodsID = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsID"));
				UserGoodsUpdate.GoodsHoldInfo[UserGoodsUpdate.wGoodsCount].wGoodsKind = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsKind"));
				UserGoodsUpdate.GoodsHoldInfo[UserGoodsUpdate.wGoodsCount].wGoodsLevel = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsLevel"));
				UserGoodsUpdate.GoodsHoldInfo[UserGoodsUpdate.wGoodsCount].dwGoodsCount = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsCount"));
				UserGoodsUpdate.GoodsHoldInfo[UserGoodsUpdate.wGoodsCount].dwGoodsIndate = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsIndate"));
				UserGoodsUpdate.GoodsHoldInfo[UserGoodsUpdate.wGoodsCount].dwExpireTime = m_AccountsDBAide.GetValue_DWORD(TEXT("ExpireTime"));

				//物品数量
				UserGoodsUpdate.wGoodsCount++;

				//下条记录
				m_AccountsDBModule->MoveToNext();
			}

			//发送数据
			WORD wHeadSize = sizeof(UserGoodsUpdate) - sizeof(UserGoodsUpdate.GoodsHoldInfo);
			WORD wDataSize = UserGoodsUpdate.wGoodsCount * sizeof(UserGoodsUpdate.GoodsHoldInfo[0]);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_GOODS_UPDATE, dwContextID, dwTokenID, &UserGoodsUpdate, wHeadSize + wDataSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);		

		return false;
	}

	return true;
}

//更新物品	
bool CDataBaseEngineSink::OnRequestUserUpdateGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//提取变量
		DBR_GR_UserUpdateGoods* pUserUpdateGoods = (DBR_GR_UserUpdateGoods*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUserUpdateGoods->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strGoodsList"), pUserUpdateGoods->szGoodsList);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserUpdateGoods"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__,__FUNCTION__, __LINE__, pIException);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//记录物品	
bool CDataBaseEngineSink::OnRequestUserRecordGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//提取变量
		DBR_GR_UserRecordGoods* pUserRecordGoods = (DBR_GR_UserRecordGoods*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUserRecordGoods->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@strRecordList"), pUserRecordGoods->szRecordList);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserRecordGoods"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__,__FUNCTION__, __LINE__, pIException);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//离开房间
bool CDataBaseEngineSink::OnRequestLeaveService(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_LeaveGameService));
		if (wDataSize!=sizeof(DBR_GR_LeaveGameService)) return false;

		//请求处理
		DBR_GR_LeaveGameService * pLeaveGameService=(DBR_GR_LeaveGameService *)pData;

		//转化地址
		TCHAR szClientIP[16]=TEXT("");
		BYTE * pClientIP=(BYTE *)&pLeaveGameService->dwClientIP;
		_sntprintf_s(szClientIP,CountArray(szClientIP),TEXT("%d.%d.%d.%d"),pClientIP[0],pClientIP[1],pClientIP[2],pClientIP[3]);

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"),pLeaveGameService->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@lScore"),pLeaveGameService->lScore);		
		m_GameDBAide.AddParameter(TEXT("@lRevenue"),pLeaveGameService->lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lWinCount"),pLeaveGameService->lWinCount);
		m_GameDBAide.AddParameter(TEXT("@lLostCount"),pLeaveGameService->lLostCount);
		m_GameDBAide.AddParameter(TEXT("@lDrawCount"),pLeaveGameService->lDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lFleeCount"),pLeaveGameService->lFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lWinStreak"), pLeaveGameService->lWinStreak);
		m_GameDBAide.AddParameter(TEXT("@lMaxWinStreak"), pLeaveGameService->lMaxWinStreak);
		m_GameDBAide.AddParameter(TEXT("@lExperience"),pLeaveGameService->lExperience);
		m_GameDBAide.AddParameter(TEXT("@lUnLockScore"),pLeaveGameService->lUnLockScore);
		m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"),pLeaveGameService->dwPlayTimeCount);
		m_GameDBAide.AddParameter(TEXT("@dwOnLineTimeCount"),pLeaveGameService->dwOnLineTimeCount);
		
		//记录成绩
		m_GameDBAide.AddParameter(TEXT("@lRecordScore"),pLeaveGameService->RecordInfo.VariationScore.lScore);
		m_GameDBAide.AddParameter(TEXT("@lRecordRevenue"),pLeaveGameService->RecordInfo.lRevenue);
		m_GameDBAide.AddParameter(TEXT("@lRecordWinCount"),pLeaveGameService->RecordInfo.VariationScore.lWinCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordLostCount"),pLeaveGameService->RecordInfo.VariationScore.lLostCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordDrawCount"),pLeaveGameService->RecordInfo.VariationScore.lDrawCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordFleeCount"),pLeaveGameService->RecordInfo.VariationScore.lFleeCount);
		m_GameDBAide.AddParameter(TEXT("@lRecordExperience"),pLeaveGameService->RecordInfo.VariationScore.lExperience);
		m_GameDBAide.AddParameter(TEXT("@dwRecordPlayTimeCount"),pLeaveGameService->RecordInfo.dwPlayTimeCount);

		//其他参数
		m_GameDBAide.AddParameter(TEXT("@strClientIP"),szClientIP);
		m_GameDBAide.AddParameter(TEXT("@wKindID"),m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@szMachineID"),pLeaveGameService->szMachineID);
		m_GameDBAide.AddParameter(TEXT("@cbLeaveReason"),pLeaveGameService->cbLeaveReason);
		m_GameDBAide.AddParameter(TEXT("@dwInoutIndex"),pLeaveGameService->dwInoutIndex);		
		
		//执行查询
		LONG lReturnValue=m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LeaveGameServer"),false);

		return lReturnValue==DB_SUCCESS;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//控制查询
bool CDataBaseEngineSink::OnRequestControlQuery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//效验参数
	ASSERT(wDataSize == sizeof(DBR_GR_ControlQuery));
	if (wDataSize != sizeof(DBR_GR_ControlQuery)) return false;

	//提取数据
	auto pControlQuery = (DBR_GR_ControlQuery*)pData;

	//构造结果
	DBO_GR_UserControlInfo UserControlInfo;
	ZeroMemory(&UserControlInfo,sizeof(UserControlInfo));

	//加载控制
	if (LoadControlInfo(pControlQuery->dwUserID, UserControlInfo.UserControlItem))
	{
		//用户标识
		UserControlInfo.dwUserID = pControlQuery->dwUserID;

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_CONTROL_INFO, dwContextID, dwTokenID, &UserControlInfo, sizeof(UserControlInfo));
	}
	else
	{
		*pdwControlCode |= DB_RESULT_DEFER_DATA;
	}

	return true;
}

//控制开始
bool CDataBaseEngineSink::OnRequestControlStart(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//效验参数
	ASSERT (wDataSize == sizeof(DBR_GR_ControlStart));
	if (wDataSize != sizeof(DBR_GR_ControlStart)) return false;

	//提取数据
	DBR_GR_ControlStart * pControlStart = (DBR_GR_ControlStart *)pData;

	try
	{
		//重置参数
		m_GameDBAide.ResetParameter();		
		m_GameDBAide.AddParameter(TEXT("@dwUserID"), pControlStart->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@cbControlMode"), pControlStart->UserControlItem.cbControlMode);
		m_GameDBAide.AddParameter(TEXT("@cbControlKind"), pControlStart->UserControlItem.cbControlKind);
		m_GameDBAide.AddParameter(TEXT("@cbControlStatus"), pControlStart->UserControlItem.cbControlStatus);
		m_GameDBAide.AddParameter(TEXT("@wTargetWinRate"), pControlStart->UserControlItem.wTargetWinRate);
		m_GameDBAide.AddParameter(TEXT("@wControlWinRate"), pControlStart->UserControlItem.wControlWinRate);
		m_GameDBAide.AddParameter(TEXT("@wInitailWinRate"), pControlStart->wAddupWinRate);		
		m_GameDBAide.AddParameter(TEXT("@lInitailWinScore"), pControlStart->lAddupWinScore);
		m_GameDBAide.AddParameter(TEXT("@lInitailLoseScore"), pControlStart->lAddupLoseScore);

		//执行锁定
		m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_UserControlStart"), false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return true;
	}

	return true;
}

//控制结束
bool CDataBaseEngineSink::OnRequestControlFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//效验参数
	ASSERT (wDataSize == sizeof(DBR_GR_ControlFinish));
	if (wDataSize != sizeof(DBR_GR_ControlFinish)) return false;

	//提取数据
	DBR_GR_ControlFinish * pControlFinish = (DBR_GR_ControlFinish *)pData;

	try
	{
		//重置参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"), pControlFinish->dwUserID);		
		m_GameDBAide.AddParameter(TEXT("@lTotalWinCount"), pControlFinish->lWinCount);
		m_GameDBAide.AddParameter(TEXT("@lTotalLoseCount"), pControlFinish->lLoseCount);
		m_GameDBAide.AddParameter(TEXT("@lTotalWinScore"), pControlFinish->lWinScore);
		m_GameDBAide.AddParameter(TEXT("@lTotalLoseScore"), pControlFinish->lLoseScore);
		m_GameDBAide.AddParameter(TEXT("@lTotalBetAmount"), pControlFinish->lBetAmount);

		//执行锁定
		m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_UserControlFinish"), false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return true;
	}

	return true;
}

//控制更新
bool CDataBaseEngineSink::OnRequestControlUpdate(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//效验参数
	ASSERT (wDataSize == sizeof(DBR_GR_ControlUpdate));
	if (wDataSize != sizeof(DBR_GR_ControlUpdate)) return false;

	//提取数据
	DBR_GR_ControlUpdate * pControlUpdate = (DBR_GR_ControlUpdate *)pData;

	try
	{
		//重置参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"), pControlUpdate->dwUserID);
		m_GameDBAide.AddParameter(TEXT("@lTotalWinCount"), pControlUpdate->lWinCount);
		m_GameDBAide.AddParameter(TEXT("@lTotalLoseCount"), pControlUpdate->lLoseCount);
		m_GameDBAide.AddParameter(TEXT("@lTotalWinScore"), pControlUpdate->lWinScore);
		m_GameDBAide.AddParameter(TEXT("@lTotalLoseScore"), pControlUpdate->lLoseScore);
		m_GameDBAide.AddParameter(TEXT("@lTotalBetAmount"), pControlUpdate->lBetAmount);

		//执行锁定
		LONG lResultCode = m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_UserControlUpdate"), true);

		//执行成功
		if (lResultCode == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_UserControlUpdate UserControlUpdate;
			ZeroMemory(&UserControlUpdate,sizeof(UserControlUpdate));

			//物品信息
			UserControlUpdate.dwUserID = pControlUpdate->dwUserID;
			UserControlUpdate.cbControlStatus = m_GameDBAide.GetValue_BYTE(TEXT("ControlStatus"));

			//投递消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_CONTROL_UPDATE, dwContextID, dwTokenID, &UserControlUpdate, sizeof(UserControlUpdate));
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return true;
	}

	return true;
}

//全局配置
bool CDataBaseEngineSink::LoadGlobalOption(DWORD dwContextID, DWORD dwTokenID, DWORD dwOptionMask)
{
	//变量定义
	TCHAR szOptionData[2048] = { 0 };

	try
	{
		//跑马灯配置
		if (dwOptionMask & OPTION_MASK_MARQUEE_NOTICE)
		{
			//构造参数
			m_PlatformDBAide.ResetParameter();
			LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadMarqueeNoticeOption"), true);

			//加载成功
			if (lReturnValue == DB_SUCCESS)
			{
				//读取配置
				m_PlatformDBAide.GetValue_String(TEXT("MarqueeNoticeOption"), szOptionData, CountArray(szOptionData));

				//添加配置
				if (!g_GlobalOptionManager->AppendOptionItem(OPTION_MASK_MARQUEE_NOTICE, szOptionData))
				{
					return false;
				}
			}
		}
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//约战配置
bool CDataBaseEngineSink::LoadBattleOption(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		LONG lReturnValue = m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LoadBattleGameOption"), true);

		//加载成功
		if (lReturnValue == DB_SUCCESS && m_GameDBModule->IsRecordsetEnd() == false)
		{
			//变量定义
			WORD wPacketSize = 0;
			BYTE cbBuffer[MAX_ASYNC_DATA];

			//版本配置
			while (m_GameDBModule->IsRecordsetEnd() == false)
			{
				//变量定义
				tagBattleGameOptionEx* pBattleGameOption = (tagBattleGameOptionEx*)&cbBuffer[wPacketSize];

				//读取数据
				pBattleGameOption->BattleGameOption.wKindID = m_GameDBAide.GetValue_WORD(TEXT("KindID"));
				pBattleGameOption->BattleGameOption.cbEmployScene = m_GameDBAide.GetValue_BYTE(TEXT("EmployScene"));
				m_GameDBAide.GetValue_String(TEXT("CustomOption"), pBattleGameOption->BattleGameOption.szOptionJson, CountArray(pBattleGameOption->BattleGameOption.szOptionJson));

				//设置大小
				wPacketSize += sizeof(tagBattleGameOptionEx);

				//移动记录
				m_GameDBModule->MoveToNext();
			}

			//发送配置
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_GAME_OPTION, dwContextID, dwTokenID, cbBuffer, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//机器参数
bool CDataBaseEngineSink::LoadRobotParameter(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//变量定义
		DBO_GR_RobotParameter GameRobotParameter;
		ZeroMemory(&GameRobotParameter, sizeof(GameRobotParameter));

		//用户帐户
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wKindID"), m_pGameServiceAttrib->wKindID);
		m_AccountsDBAide.AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);

		//执行查询
		GameRobotParameter.lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_LoadRobotConfigure"), true);

		//读取信息
		for (WORD i = 0; i < CountArray(GameRobotParameter.RobotParameter); i++)
		{
			//结束判断
			if (m_AccountsDBModule->IsRecordsetEnd() == true) break;

			//溢出效验
			ASSERT(GameRobotParameter.wParameterCount < CountArray(GameRobotParameter.RobotParameter));
			if (GameRobotParameter.wParameterCount >= CountArray(GameRobotParameter.RobotParameter)) break;

			//读取数据
			GameRobotParameter.wParameterCount++;
			GameRobotParameter.RobotParameter[i].dwBatchID = m_AccountsDBAide.GetValue_DWORD(TEXT("BatchID"));
			GameRobotParameter.RobotParameter[i].dwServiceMode = m_AccountsDBAide.GetValue_DWORD(TEXT("ServiceMode"));
			GameRobotParameter.RobotParameter[i].dwRobotCount = m_AccountsDBAide.GetValue_DWORD(TEXT("RobotCount"));
			GameRobotParameter.RobotParameter[i].dwEnterTime = m_AccountsDBAide.GetValue_DWORD(TEXT("EnterTime"));
			GameRobotParameter.RobotParameter[i].dwLeaveTime = m_AccountsDBAide.GetValue_DWORD(TEXT("LeaveTime"));
			GameRobotParameter.RobotParameter[i].lTakeMinScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("TakeMinScore"));
			GameRobotParameter.RobotParameter[i].lTakeMaxScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("TakeMaxScore"));
			GameRobotParameter.RobotParameter[i].dwEnterMinInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("EnterMinInterval"));
			GameRobotParameter.RobotParameter[i].dwEnterMaxInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("EnterMaxInterval"));
			GameRobotParameter.RobotParameter[i].dwLeaveMinInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("LeaveMinInterval"));
			GameRobotParameter.RobotParameter[i].dwLeaveMaxInterval = m_AccountsDBAide.GetValue_DWORD(TEXT("LeaveMaxInterval"));
			GameRobotParameter.RobotParameter[i].dwSwitchMinInnings = m_AccountsDBAide.GetValue_DWORD(TEXT("SwitchMinInnings"));
			GameRobotParameter.RobotParameter[i].dwSwitchMaxInnings = m_AccountsDBAide.GetValue_DWORD(TEXT("SwitchMaxInnings"));

			//移动记录
			m_AccountsDBModule->MoveToNext();
		}

		//发送信息
		WORD wHeadSize = sizeof(GameRobotParameter) - sizeof(GameRobotParameter.RobotParameter);
		WORD wDataSize = GameRobotParameter.wParameterCount * sizeof(GameRobotParameter.RobotParameter[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ROBOT_PARAMETER, dwContextID, dwTokenID, &GameRobotParameter, wHeadSize + wDataSize);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);

		//变量定义
		DBO_GR_RobotParameter GameRobotParameter;
		ZeroMemory(&GameRobotParameter, sizeof(GameRobotParameter));

		//构造变量
		GameRobotParameter.wParameterCount = 0L;
		GameRobotParameter.lResultCode = DB_ERROR;

		//发送信息
		WORD wHeadSize = sizeof(GameRobotParameter) - sizeof(GameRobotParameter.RobotParameter);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ROBOT_PARAMETER, dwContextID, dwTokenID, &GameRobotParameter, wHeadSize);
	}

	return false;
}

//低保参数
bool CDataBaseEngineSink::LoadSubsidyParameter(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//变量定义
		DBO_GR_SubsidyParameter SubsidyParameter;
		ZeroMemory(&SubsidyParameter, sizeof(SubsidyParameter));

		//构造参数
		m_PlatformDBAide.ResetParameter();

		//执行查询
		LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadSubsidyParameter"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//读取数据
			SubsidyParameter.lTakeSubsidyGold = m_PlatformDBAide.GetValue_LONGLONG(TEXT("TakeSubsidyGold"));
			SubsidyParameter.lTakeSubsidyCondition = m_PlatformDBAide.GetValue_LONGLONG(TEXT("TakeSubsidyCondition"));
		}

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SUBSIDY_PARAMETER, 0, 0, &SubsidyParameter, sizeof(SubsidyParameter));

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//控制参数
bool CDataBaseEngineSink::LoadControlParameter(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//变量定义
		DBO_GR_ControlParameter ControlParameter;
		ZeroMemory(&ControlParameter, sizeof(ControlParameter));

		//构造参数
		m_PlatformDBAide.ResetParameter();

		//执行查询
		LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadControlParameter"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//控制参数
			ControlParameter.bControlEnabled = m_PlatformDBAide.GetValue_BYTE(TEXT("ControlEnabled"));
			ControlParameter.lBenchmarkValue = m_PlatformDBAide.GetValue_LONGLONG(TEXT("BenchmarkValue"));
			ControlParameter.wWLControlRate = m_PlatformDBAide.GetValue_WORD(TEXT("WLControlRate"));
			ControlParameter.wBLControlRate = m_PlatformDBAide.GetValue_WORD(TEXT("BLControlRate"));
			ControlParameter.wInWhiteListRate = m_PlatformDBAide.GetValue_WORD(TEXT("InWhiteListRate"));
			ControlParameter.wInBlackListRate = m_PlatformDBAide.GetValue_WORD(TEXT("InBlackListRate"));
			ControlParameter.wOutWhiteListRate = m_PlatformDBAide.GetValue_WORD(TEXT("OutWhiteListRate"));
			ControlParameter.wOutBlackListRate = m_PlatformDBAide.GetValue_WORD(TEXT("OutBlackListRate"));
			ControlParameter.wStartPlayCount = m_PlatformDBAide.GetValue_WORD(TEXT("StartPlayCount"));
			ControlParameter.wSupportControlMode = m_PlatformDBAide.GetValue_WORD(TEXT("SupportControlMode"));
		}

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CONTROL_PARAMETER, 0, 0, &ControlParameter, sizeof(ControlParameter));

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//游戏等级
bool CDataBaseEngineSink::LoadGameLevelOption(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//构造参数
		m_PlatformDBAide.ResetParameter();
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameLevelInfo"), true);

		//加载成功
		if (lResultCode == DB_SUCCESS)
		{
			//清理数据
			g_GameLevelManager->m_ArrayGameLevel.RemoveAll();
			g_GameLevelManager->m_ArrayUserLevel.RemoveAll();
			g_GameLevelManager->m_ArrayRankLevel.RemoveAll();

			//等级信息
			tagGameLevelItem GameLevelItem;

			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				//读取数据					
				GameLevelItem.wLevelID = m_PlatformDBAide.GetValue_WORD(TEXT("LevelID"));
				GameLevelItem.lNeedPoint = m_PlatformDBAide.GetValue_LONG(TEXT("NeedPoint"));
				m_PlatformDBAide.GetValue_String(TEXT("Rewards"), GameLevelItem.szRewards, CountArray(GameLevelItem.szRewards));

				//添加物品
				g_GameLevelManager->m_ArrayGameLevel.Add(GameLevelItem);

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}

			//构造参数
			m_PlatformDBAide.ResetParameter();
			lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadUserLevelInfo"), true);

			//加载成功
			if (lResultCode == DB_SUCCESS)
			{
				//等级信息
				tagUserLevelItem UserLevelItem;

				while (m_PlatformDBModule->IsRecordsetEnd() == false)
				{
					//读取数据					
					UserLevelItem.wLevelID = m_PlatformDBAide.GetValue_WORD(TEXT("LevelID"));
					UserLevelItem.lNeedPoint = m_PlatformDBAide.GetValue_LONG(TEXT("NeedPoint"));
					m_PlatformDBAide.GetValue_String(TEXT("Rewards"), UserLevelItem.szRewards, CountArray(UserLevelItem.szRewards));
					m_PlatformDBAide.GetValue_String(TEXT("LevelName"), UserLevelItem.szLevelName, CountArray(UserLevelItem.szLevelName));

					//添加物品
					g_GameLevelManager->m_ArrayUserLevel.Add(UserLevelItem);

					//移动记录
					m_PlatformDBModule->MoveToNext();
				}
			}

			//构造参数
			m_PlatformDBAide.ResetParameter();
			lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadRankLevelInfo"), true);

			//加载成功
			if (lResultCode == DB_SUCCESS)
			{
				//等级信息
				tagRankLevelItem RankLevelItem;

				while (m_PlatformDBModule->IsRecordsetEnd() == false)
				{
					//读取数据					
					RankLevelItem.wLevelID = m_PlatformDBAide.GetValue_WORD(TEXT("LevelID"));
					RankLevelItem.lNeedScore = m_PlatformDBAide.GetValue_LONG(TEXT("NeedScore"));
					RankLevelItem.cbMainOrder = m_PlatformDBAide.GetValue_BYTE(TEXT("MainOrder"));
					RankLevelItem.cbSubOrder = m_PlatformDBAide.GetValue_BYTE(TEXT("SubOrder"));
					RankLevelItem.cbStarOrder = m_PlatformDBAide.GetValue_BYTE(TEXT("StarOrder"));
					m_PlatformDBAide.GetValue_String(TEXT("LevelName"), RankLevelItem.szLevelName, CountArray(RankLevelItem.szLevelName));

					//添加等级
					g_GameLevelManager->m_ArrayRankLevel.Add(RankLevelItem);

					//移动记录
					m_PlatformDBModule->MoveToNext();
				}
			}
		}

		return lResultCode == DB_SUCCESS;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;

}

//游戏物品
bool CDataBaseEngineSink::LoadGameGoodsOption(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//构造参数
		m_PlatformDBAide.ResetParameter();
		LONG lResultCode = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_GS_LoadGameGoodsInfo"), true);

		//加载成功
		if (lResultCode == DB_SUCCESS)
		{
			//清理数据
			g_GameGoodsManager->m_ArrayGoodsInfo.RemoveAll();

			//物品信息
			tagGoodsInfo GoodsInfo;

			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				//读取数据					
				GoodsInfo.wGoodsID = m_PlatformDBAide.GetValue_WORD(TEXT("GoodsID"));
				GoodsInfo.wGoodsKind = m_PlatformDBAide.GetValue_WORD(TEXT("GoodsKind"));
				GoodsInfo.bHasIndate = m_PlatformDBAide.GetValue_BYTE(TEXT("HasIndate"));
				GoodsInfo.wCanTradeType = m_PlatformDBAide.GetValue_WORD(TEXT("CanTradeType"));
				GoodsInfo.wCanStorePlace = m_PlatformDBAide.GetValue_WORD(TEXT("CanStorePlace"));
				m_PlatformDBAide.GetValue_String(TEXT("GoodsName"), GoodsInfo.szGoodsName, CountArray(GoodsInfo.szGoodsName));
				m_PlatformDBAide.GetValue_String(TEXT("GoodsOption"), GoodsInfo.szGoodsOption, CountArray(GoodsInfo.szGoodsOption));

				//添加物品
				g_GameGoodsManager->m_ArrayGoodsInfo.Add(GoodsInfo);

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}
		}

		return lResultCode == DB_SUCCESS;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//版本信息
bool CDataBaseEngineSink::LoadGameVersionInfo(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//构造参数
		m_PlatformDBAide.ResetParameter();
		m_PlatformDBAide.AddParameter(TEXT("@wModuleID"), m_pGameServiceOption->wKindID);
		LONG lReturnValue = m_PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameVersionInfo"), true);

		//加载成功
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			WORD wPacketSize = 0;
			BYTE cbBuffer[SOCKET_PACKET];

			//变量定义
			tagModuleVersion* pModuleVersion = NULL;

			//版本配置
			while (m_PlatformDBModule->IsRecordsetEnd() == false)
			{
				//发送信息
				if ((wPacketSize + sizeof(tagModuleVersion)) > sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_VERSION_INFO, dwContextID, dwTokenID, cbBuffer, wPacketSize);
					wPacketSize = 0;
				}

				//读取信息
				pModuleVersion = (tagModuleVersion*)(cbBuffer + wPacketSize);

				//读取数据
				pModuleVersion->IsCurrent = m_PlatformDBAide.GetValue_BYTE(TEXT("IsCurrent"));
				pModuleVersion->wModuleID = m_PlatformDBAide.GetValue_WORD(TEXT("ModuleID"));
				pModuleVersion->dwDeviceKind = m_PlatformDBAide.GetValue_BYTE(TEXT("DeviceKind"));
				pModuleVersion->dwVersion = m_PlatformDBAide.GetValue_DWORD(TEXT("Version"));

				//设置位移
				wPacketSize += sizeof(tagModuleVersion);

				//移动记录
				m_PlatformDBModule->MoveToNext();
			}

			//发送剩余
			if (wPacketSize > 0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_VERSION_INFO, dwContextID, dwTokenID, cbBuffer, wPacketSize);
			}
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//敏感词汇
bool CDataBaseEngineSink::LoadFilterWords(DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_LoadConfineContent"), true);

		//读取信息
		if (lReturnValue == DB_SUCCESS)
		{
			//起始消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FILTER_WORDS, 0xfffe, dwTokenID, NULL, 0);

			//读取消息
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				TCHAR szSensitiveWords[32] = TEXT("");

				//读取消息
				m_AccountsDBAide.GetValue_String(TEXT("ForbidWords"), szSensitiveWords, 32);

				//发送消息
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FILTER_WORDS, 0, dwTokenID, szSensitiveWords, 32);

				//移动记录
				m_AccountsDBModule->MoveToNext();
			};

			//结束消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FILTER_WORDS, 0xffff, 0, NULL, 0);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//解锁机器
bool CDataBaseEngineSink::UnlockRobotUser()
{
	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_UnlockRobotUser"), false);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		CTraceService::TraceString(pIException->GetExceptionDescribe(), TraceLevel_Exception);
	}

	return false;
}

//加载背包
bool CDataBaseEngineSink::LoadUserPack(DWORD dwUserID)
{
	try
	{
		//设置变量
		m_LogonSuccess.wGoodsCount = 0;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserQueryGoods"), true);

		//执行成功
		if (lResultCode == DB_SUCCESS)
		{
			//读取信息
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				//读取数据
				m_LogonSuccess.GoodsHoldInfo[m_LogonSuccess.wGoodsCount].wGoodsID = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsID"));
				m_LogonSuccess.GoodsHoldInfo[m_LogonSuccess.wGoodsCount].wGoodsKind = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsKind"));
				m_LogonSuccess.GoodsHoldInfo[m_LogonSuccess.wGoodsCount].wGoodsLevel = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsLevel"));
				m_LogonSuccess.GoodsHoldInfo[m_LogonSuccess.wGoodsCount].dwGoodsCount = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsCount"));
				m_LogonSuccess.GoodsHoldInfo[m_LogonSuccess.wGoodsCount].dwGoodsIndate = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsIndate"));
				m_LogonSuccess.GoodsHoldInfo[m_LogonSuccess.wGoodsCount].dwExpireTime = m_AccountsDBAide.GetValue_DWORD(TEXT("ExpireTime"));

				//校验数量
				if (++m_LogonSuccess.wGoodsCount == CountArray(m_LogonSuccess.GoodsHoldInfo))
				{
					break;
				}				

				//移动记录
				m_AccountsDBModule->MoveToNext();
			}			
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

//查询低保
bool CDataBaseEngineSink::LoadUserSubsidy(DWORD dwUserID)
{
	try
	{
		//设置变量
		m_LogonSuccess.cbResidualTimes = 0;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@szMachine"), m_LogonSuccess.szMachineID);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString), adParamOutput);

		//执行查询
		LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_QuerySubsidy"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//剩余次数
			m_LogonSuccess.cbResidualTimes = m_AccountsDBAide.GetValue_BYTE(TEXT("ResidualTimes"));
			m_LogonSuccess.lConsumeScore = m_AccountsDBAide.GetValue_LONGLONG(TEXT("ConsumeScore"));
		}

		return true;
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return false;
}

//加载控制
bool CDataBaseEngineSink::LoadControlInfo(DWORD dwUserID, tagUserControlItem& UserControlItem)
{
	try
	{
		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);		

		//执行查询
		LONG lReturnValue = m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_LoadUserControl"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//控制信息
			UserControlItem.cbControlMode = m_GameDBAide.GetValue_BYTE(TEXT("ControlMode"));
			UserControlItem.cbControlKind = m_GameDBAide.GetValue_BYTE(TEXT("ControlKind"));
			UserControlItem.cbControlStatus = m_GameDBAide.GetValue_BYTE(TEXT("ControlStatus"));
			UserControlItem.wTargetWinRate = m_GameDBAide.GetValue_WORD(TEXT("TargetWinRate"));
			UserControlItem.wControlWinRate = m_GameDBAide.GetValue_WORD(TEXT("ControlWinRate"));
			UserControlItem.lTotalWinCount = m_GameDBAide.GetValue_LONG(TEXT("TotalWinCount"));
			UserControlItem.lTotalLoseCount = m_GameDBAide.GetValue_LONG(TEXT("TotalLoseCount"));
			UserControlItem.lTotalWinScore = m_GameDBAide.GetValue_LONGLONG(TEXT("TotalWinScore"));
			UserControlItem.lTotalLoseScore = m_GameDBAide.GetValue_LONGLONG(TEXT("TotalLoseScore"));
			UserControlItem.lTotalBetAmount = m_GameDBAide.GetValue_LONGLONG(TEXT("TotalBetAmount"));
			UserControlItem.wControlPlayCount = m_GameDBAide.GetValue_WORD(TEXT("ControlPlayCount"));
			UserControlItem.dwControlDuration = m_GameDBAide.GetValue_DWORD(TEXT("ControlDuration"));

			//起始时间
			SYSTEMTIME stControlStartTime;
			m_GameDBAide.GetValue_SystemTime(TEXT("ControlStartTime"), stControlStartTime);

			//转换时间
			CTime TimeControlStart(stControlStartTime);
			UserControlItem.dwControlStartTime = TimeControlStart.GetTime();
		}
	}
	catch (...)
	{
		ASSERT(FALSE);

		return false;
	}

	return true;
}

//登录成功
VOID CDataBaseEngineSink::OnLogonDisposeResult(DWORD dwContextID, DWORD dwTokenID, LONG lResultCode, bool bMobileClient)
{
	if (lResultCode==DB_SUCCESS)
	{
		//属性资料
		m_LogonSuccess.wFaceID=m_GameDBAide.GetValue_WORD(TEXT("FaceID"));
		m_LogonSuccess.dwUserID=m_GameDBAide.GetValue_DWORD(TEXT("UserID"));
		m_LogonSuccess.dwGameID=m_GameDBAide.GetValue_DWORD(TEXT("GameID"));
		m_LogonSuccess.dwCustomID=m_GameDBAide.GetValue_DWORD(TEXT("CustomID"));
		m_GameDBAide.GetValue_String(TEXT("Accounts"), m_LogonSuccess.szAccounts, CountArray(m_LogonSuccess.szAccounts));
		m_GameDBAide.GetValue_String(TEXT("LogonPass"), m_LogonSuccess.szPassword, CountArray(m_LogonSuccess.szPassword));

		//用户资料
		m_LogonSuccess.cbAge=m_GameDBAide.GetValue_BYTE(TEXT("Age"));
		m_LogonSuccess.cbGender=m_GameDBAide.GetValue_BYTE(TEXT("Gender"));
		m_GameDBAide.GetValue_String(TEXT("Area"),m_LogonSuccess.szArea,CountArray(m_LogonSuccess.szArea));
		m_GameDBAide.GetValue_String(TEXT("City"),m_LogonSuccess.szCity,CountArray(m_LogonSuccess.szCity));
		m_GameDBAide.GetValue_String(TEXT("Province"),m_LogonSuccess.szProvince,CountArray(m_LogonSuccess.szProvince));
		m_GameDBAide.GetValue_String(TEXT("UnderWrite"),m_LogonSuccess.szUnderWrite,CountArray(m_LogonSuccess.szUnderWrite));
		m_GameDBAide.GetValue_String(TEXT("userHeadUrl"), m_LogonSuccess.szUserHeadUrl, CountArray(m_LogonSuccess.szUserHeadUrl));//kk jia
				
		//积分信息
		m_LogonSuccess.lGold=m_GameDBAide.GetValue_LONGLONG(TEXT("Gold"));
		m_LogonSuccess.lScore=m_GameDBAide.GetValue_LONGLONG(TEXT("Score"));
		m_LogonSuccess.cbScoreKind=m_GameDBAide.GetValue_BYTE(TEXT("ScoreKind"));
		m_LogonSuccess.lUserCard =m_GameDBAide.GetValue_LONGLONG(TEXT("UserCard"));
		m_LogonSuccess.lFreeScore=m_GameDBAide.GetValue_LONGLONG(TEXT("FreeScore"));		
		m_LogonSuccess.lInsureGold=m_GameDBAide.GetValue_LONGLONG(TEXT("InsureScore"));		

		//游戏信息
		m_LogonSuccess.lWinCount=m_GameDBAide.GetValue_LONG(TEXT("WinCount"));
		m_LogonSuccess.lLostCount=m_GameDBAide.GetValue_LONG(TEXT("LostCount"));
		m_LogonSuccess.lDrawCount=m_GameDBAide.GetValue_LONG(TEXT("DrawCount"));
		m_LogonSuccess.lFleeCount=m_GameDBAide.GetValue_LONG(TEXT("FleeCount"));
		m_LogonSuccess.lExperience=m_GameDBAide.GetValue_LONG(TEXT("Experience"));
		m_LogonSuccess.lWinStreak=m_GameDBAide.GetValue_LONG(TEXT("WinStreak"));
		m_LogonSuccess.lMaxWinStreak=m_GameDBAide.GetValue_LONG(TEXT("MaxWinStreak"));

		//等级信息
		m_LogonSuccess.cbMasterOrder = m_GameDBAide.GetValue_BYTE(TEXT("MasterOrder"));
		m_LogonSuccess.dwMemberPoint = m_GameDBAide.GetValue_DWORD(TEXT("MemberPoint"));

		//状态信息
		m_LogonSuccess.cbShutUpStatus = m_GameDBAide.GetValue_BYTE(TEXT("ShutUpStatus"));
		m_LogonSuccess.dwShutUpOverTime = m_GameDBAide.GetValue_DWORD(TEXT("ShutUpOverTime"));

		//附加信息
		m_LogonSuccess.dwUserRight=m_GameDBAide.GetValue_DWORD(TEXT("UserRight"));
		m_LogonSuccess.dwMasterRight=m_GameDBAide.GetValue_DWORD(TEXT("MasterRight"));

		//索引变量
		m_LogonSuccess.dwInoutIndex=m_GameDBAide.GetValue_DWORD(TEXT("InoutIndex"));

		//金币房间
		if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD && bMobileClient == true)
		{
			m_LogonSuccess.wAddupWinRate = m_GameDBAide.GetValue_WORD(TEXT("AddupWinRate"));
			m_LogonSuccess.lAddupWinScore = m_GameDBAide.GetValue_LONGLONG(TEXT("AddupWinScore"));
			m_LogonSuccess.lAddupLoseScore = m_GameDBAide.GetValue_LONGLONG(TEXT("AddupLoseScore"));			
		}

		//比赛房间
		if (m_pGameServiceOption->wServerType&GAME_GENRE_MATCH && bMobileClient==true) 
		{
			m_LogonSuccess.wSignupCount=m_GameDBAide.GetValue_WORD(TEXT("SignupCount"));
			m_LogonSuccess.dwTicketCount=m_GameDBAide.GetValue_DWORD(TEXT("TicketCount"));
		}

		//错误信息
		m_GameDBAide.GetParameter(TEXT("@strErrorDescribe"),m_LogonSuccess.szDescribeString,CountArray(m_LogonSuccess.szDescribeString));

		//加载背包
		LoadUserPack(m_LogonSuccess.dwUserID);

		//低保信息
		if (CServerRule::IsAllowLargessSubsidy(m_pGameServiceOption->dwServerRule))
		{
			LoadUserSubsidy(m_LogonSuccess.dwUserID);
		}		

		//控制信息
		if (m_pGameServiceOption->wServerType&GAME_GENRE_GOLD)
		{
			LoadControlInfo(m_LogonSuccess.dwUserID, m_LogonSuccess.UserControlItem);
		}

		//发送结果
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_SUCCESS,dwContextID,dwTokenID,&m_LogonSuccess,sizeof(m_LogonSuccess));
	}
	else
	{
		//获取信息		
		m_LogonFailure.lErrorCode=lResultCode;
		m_LogonFailure.wLockServerID=INVALID_WORD;
		m_GameDBAide.GetParameter(TEXT("@strErrorDescribe"),m_LogonFailure.szDescribeString,CountArray(m_LogonFailure.szDescribeString));

		//用户锁定
		if (m_LogonFailure.lErrorCode==100)
		{
			try
			{
				m_LogonFailure.wLockKindID=m_GameDBAide.GetValue_WORD(TEXT("LockKindID"));
				m_LogonFailure.wLockServerID=m_GameDBAide.GetValue_WORD(TEXT("LockServerID"));
				m_LogonFailure.wLockServerKind=m_GameDBAide.GetValue_WORD(TEXT("LockServerKind"));
			}
			catch (...){}
		}

		//发送结果
		WORD wStringSize=CountStringBuffer(m_LogonFailure.szDescribeString);
		WORD wSendSize=sizeof(m_LogonFailure)-sizeof(m_LogonFailure.szDescribeString)+wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE,dwContextID,dwTokenID,&m_LogonFailure,wSendSize);
	}

	return;
}

//登录失败
VOID CDataBaseEngineSink::OnLogonDisposeDBError(DWORD dwContextID, DWORD dwTokenID, IDataBaseException * pIException)
{
	//构造错误
	m_LogonFailure.lErrorCode=DB_ERROR;
	m_LogonFailure.wLockServerID=INVALID_WORD;
	LPCTSTR pszErrorString= CWHService::FormatMessage(MIM_SERVER_0111,TEXT("很抱歉，由于数据库处理繁忙，请您稍后再次尝试或者进去其他游戏房间！"));
	StringCchCopy(m_LogonFailure.szDescribeString,CountArray(m_LogonFailure.szDescribeString), pszErrorString);

	//发送结果
	WORD wStringSize=CountStringBuffer(m_LogonFailure.szDescribeString);
	WORD wSendSize=sizeof(m_LogonFailure)-sizeof(m_LogonFailure.szDescribeString)+wStringSize;
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE,dwContextID,dwTokenID,&m_LogonFailure,wSendSize);

	return;
}

//加载机器
bool CDataBaseEngineSink::OnRequestLoadRobots(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//参数校验
	ASSERT(sizeof(DBR_GR_LoadRobotUser)==wDataSize);
	if(sizeof(DBR_GR_LoadRobotUser)!=wDataSize) return false;

	try
	{		
		//提取变量
		DBR_GR_LoadRobotUser * pLoadRobotUser = (DBR_GR_LoadRobotUser *)pData;
		if(pLoadRobotUser==NULL) return false;

		//变量定义
		DBO_GR_GameRobotInfo GameRobotInfo;
		ZeroMemory(&GameRobotInfo,sizeof(GameRobotInfo));

		//设置批次
		GameRobotInfo.dwBatchID=pLoadRobotUser->dwBatchID;

		//用户帐户
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@wServerID"),m_pGameServiceOption->wServerID);
		m_AccountsDBAide.AddParameter(TEXT("@dwBatchID"),pLoadRobotUser->dwBatchID);
		m_AccountsDBAide.AddParameter(TEXT("@dwRobotCount"),pLoadRobotUser->dwRobotCount);

		//执行查询
		GameRobotInfo.lResultCode=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_LoadRobotUser"),true);

		//读取信息
		for (WORD i=0;i<CountArray(GameRobotInfo.RobotAccountsInfo);i++)
		{
			//结束判断
			if (m_AccountsDBModule->IsRecordsetEnd()==true) break;

			//溢出效验
			ASSERT(GameRobotInfo.wRobotCount<CountArray(GameRobotInfo.RobotAccountsInfo));
			if (GameRobotInfo.wRobotCount>=CountArray(GameRobotInfo.RobotAccountsInfo)) break;

			//读取数据
			GameRobotInfo.wRobotCount++;
			GameRobotInfo.RobotAccountsInfo[i].dwUserID=m_AccountsDBAide.GetValue_DWORD(TEXT("UserID"));
			m_AccountsDBAide.GetValue_String(TEXT("LogonPass"),GameRobotInfo.RobotAccountsInfo[i].szPassword,CountArray(GameRobotInfo.RobotAccountsInfo[i].szPassword));

			//移动记录
			m_AccountsDBModule->MoveToNext();
		}

		//发送信息
		WORD wHeadSize=sizeof(GameRobotInfo)-sizeof(GameRobotInfo.RobotAccountsInfo);
		WORD wDataSize=GameRobotInfo.wRobotCount*sizeof(GameRobotInfo.RobotAccountsInfo[0]);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ROBOT_USER,dwContextID,dwTokenID,&GameRobotInfo,wHeadSize+wDataSize);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GR_GameRobotInfo GameRobotInfo;
		ZeroMemory(&GameRobotInfo,sizeof(GameRobotInfo));

		//构造变量
		GameRobotInfo.wRobotCount=0L;
		GameRobotInfo.lResultCode=DB_ERROR;

		//发送信息
		WORD wHeadSize=sizeof(GameRobotInfo)-sizeof(GameRobotInfo.RobotAccountsInfo);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_ROBOT_USER,dwContextID,dwTokenID,&GameRobotInfo,wHeadSize);
	}

	return true;
}

//加载配置
bool CDataBaseEngineSink::OnRequestLoadConfig(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//提取数据
	auto pLoadConfig = (DBR_GR_LoadConfig*)pData;

	//构造结果
	DBO_GR_ConfigResult ConfigResult;
	ZeroMemory(&ConfigResult,sizeof(ConfigResult));

	//设置掩码
	ConfigResult.bInitService = pLoadConfig->bInitService;
	ConfigResult.dwConfigMask = pLoadConfig->dwConfigMask;

	do
	{
		//游戏等级
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_GAME_LEVEL)
		{
			ConfigResult.bResultCode = LoadGameLevelOption(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//游戏物品
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_GAME_GOODS)
		{
			ConfigResult.bResultCode = LoadGameGoodsOption(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//机器参数
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_ROBOT_PARAM)
		{
			ConfigResult.bResultCode = LoadRobotParameter(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//低保参数
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_SUBSIDY_PARAM)
		{
			ConfigResult.bResultCode = LoadSubsidyParameter(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//控制参数
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_CONTROL_PARAM)
		{
			ConfigResult.bResultCode = LoadControlParameter(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//约战配置
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_BATTLE_OPTION)
		{
			ConfigResult.bResultCode = LoadBattleOption(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//版本信息
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_GAME_VERSION)
		{
			ConfigResult.bResultCode = LoadGameVersionInfo(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//敏感词汇
		if (pLoadConfig->dwConfigMask & CONFIG_MASK_FILTER_WORDS)
		{
			ConfigResult.bResultCode = LoadFilterWords(dwContextID, dwTokenID);
			if (!ConfigResult.bResultCode) break;
		}

		//全局配置
		if (pLoadConfig->dwConfigMask & GLOBAL_OPTION_MASK)
		{
			ConfigResult.bResultCode = LoadGlobalOption(dwContextID, dwTokenID, pLoadConfig->dwConfigMask);
			if (!ConfigResult.bResultCode) break;
		}		

		//游戏配置
		if (pLoadConfig->dwConfigMask & GAME_CONFIG_MASK)
		{
			if (m_pIGameDataBaseSink != NULL)
			{
				ConfigResult.bResultCode = m_pIGameDataBaseSink->LoadGameConfig(pLoadConfig->dwConfigMask,0);
				if (!ConfigResult.bResultCode) break;
			}
		}

	} while (false);
	
	//发送结果
	m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CONFIG_RESULT, dwContextID, dwTokenID, &ConfigResult, sizeof(ConfigResult));
	
	return true;
}

//录像记录
bool CDataBaseEngineSink::OnRequestRecordGameVideo(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize <= sizeof(DBR_GR_RecordGameVideo));
		if (wDataSize > sizeof(DBR_GR_RecordGameVideo)) return false;

		//请求处理
		DBR_GR_RecordGameVideo * pRecordGameVideo = (DBR_GR_RecordGameVideo *)pData;

		//变量定义
		CString strUserID, strUserIDList;

		strUserIDList.AppendChar('[');

		//构造数据		
		for (int i = 0; i < pRecordGameVideo->wUserCount; i++)
		{
			//用户标识
			strUserID.Format(TEXT("%d"), pRecordGameVideo->dwUserID[i]);

			if (i > 0)
			{
				strUserIDList.AppendChar(',');
			}

			strUserIDList.Append(strUserID);
		}

		strUserIDList.AppendChar(']');

		//构造参数
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@dwBattleNum"), pRecordGameVideo->dwRelationNum);
		m_GameDBAide.AddParameter(TEXT("@lBattleRecordID"), pRecordGameVideo->lRelationRecordID);
		m_GameDBAide.AddParameter(TEXT("@strVideoNumber"), pRecordGameVideo->szVideoNumber);
		m_GameDBAide.AddParameter(TEXT("@VideoCreateTime"), pRecordGameVideo->VideoCreateTime);

		//用户信息
		m_GameDBAide.AddParameter(TEXT("@wUserCount"), pRecordGameVideo->wUserCount);
		m_GameDBAide.AddParameter(TEXT("@strUserIDList"), strUserIDList);

		//执行查询
		LONG lReturnValue = m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_RecordGameVideo"), false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//游戏记录
bool CDataBaseEngineSink::OnRequestRecordDrawInfo(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize <= sizeof(DBR_GR_RecordDrawInfo));
		if (wDataSize > sizeof(DBR_GR_RecordDrawInfo)) return false;

		//请求处理
		DBR_GR_RecordDrawInfo * pRecordDrawInfo = (DBR_GR_RecordDrawInfo *)pData;

		//构造参数		
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@lRecordID"), pRecordDrawInfo->lRecordID);
		m_GameDBAide.AddParameter(TEXT("@wKindID"), m_pGameServiceOption->wKindID);
		m_GameDBAide.AddParameter(TEXT("@wServerID"), m_pGameServiceOption->wServerID);
		m_GameDBAide.AddParameter(TEXT("@wTableID"), pRecordDrawInfo->wTableID);
		m_GameDBAide.AddParameter(TEXT("@wUserCount"), pRecordDrawInfo->wUserCount);
		m_GameDBAide.AddParameter(TEXT("@lWasteCount"), pRecordDrawInfo->lWasteCount);
		m_GameDBAide.AddParameter(TEXT("@dwWasterUserID"), pRecordDrawInfo->dwWasteUserID);
		m_GameDBAide.AddParameter(TEXT("@lRevenueCount"), pRecordDrawInfo->lRevenueCount);
		m_GameDBAide.AddParameter(TEXT("@dwPlayTimeCount"), pRecordDrawInfo->dwPlayTimeCount);
		m_GameDBAide.AddParameter(TEXT("@ExecuteDate"), pRecordDrawInfo->RecordTime);
		m_GameDBAide.AddParameter(TEXT("@strVideoNumber"), pRecordDrawInfo->szVideoNumber);

		//约战房间
		if ((m_pGameServiceOption->wServerType&GAME_GENRE_BATTLE) != 0)
		{
			m_GameDBAide.AddParameter(TEXT("@cbSocreKind"), pRecordDrawInfo->cbScoreKind);
			m_GameDBAide.AddParameter(TEXT("@lBattleRecordID"), pRecordDrawInfo->lRelationRecordID);
			m_GameDBAide.AddParameter(TEXT("@strGameCourse"), pRecordDrawInfo->szGameSourse);
		}

		//比赛房间
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_MATCH) != 0)
		{
			m_GameDBAide.AddParameter(TEXT("@lRelationMatchNo"), pRecordDrawInfo->lRelationRecordID);			
		}

		//执行查询
		m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_RecordDrawInfo"), false);		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//写分记录
bool CDataBaseEngineSink::OnRequestRecordDrawScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize <= sizeof(DBR_GR_RecordDrawScore));
		if (wDataSize > sizeof(DBR_GR_RecordDrawScore)) return false;

		//请求处理
		DBR_GR_RecordDrawScore* pRecordDrawScore = (DBR_GR_RecordDrawScore*)pData;

		//构造字符串
		CString strDrawScore, strScoreItem;
		LPCTSTR pszScoreFormat = TEXT("{\"UserID\":%d,\"ChairID\":%d,\"Score\":%I64d,\"Revenue\":%I64d,\"BetAmount\":%I64d,\"BeforeScore\":%I64d}");
		
		//添加括号
		strDrawScore.AppendChar('[');

		//构造参数
		for (WORD i = 0; i < pRecordDrawScore->wUserCount; i++)
		{
			//获取对象
			auto pScoreRecordItem = &pRecordDrawScore->ScoreRecordItem[i];

			//格式化分数
			strScoreItem.Format(pszScoreFormat, pScoreRecordItem->dwUserID, pScoreRecordItem->wChairID, pScoreRecordItem->lScore, pScoreRecordItem->lRevenue, pScoreRecordItem->lBetAmount, pScoreRecordItem->lBeforScore);
			
			//添加字符串			
			if (i>0) strDrawScore.AppendChar(',');
			strDrawScore.Append(strScoreItem);			
		}

		//添加括号
		strDrawScore.AppendChar(']');
		
		//构造请求
		m_GameDBAide.ResetParameter();
		m_GameDBAide.AddParameter(TEXT("@lRecordID"), pRecordDrawScore->lRecordID);
		m_GameDBAide.AddParameter(TEXT("@strVideoNumber"), pRecordDrawScore->szVideoNumber);
		m_GameDBAide.AddParameter(TEXT("@strScoreItemList"), strDrawScore.GetString());

		//约战房间
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_BATTLE) != 0)
		{
			m_GameDBAide.AddParameter(TEXT("@lBattleRecordID"), pRecordDrawScore->lRelationRecordID);
		}	

		//比赛房间
		if ((m_pGameServiceOption->wServerType & GAME_GENRE_MATCH) != 0)
		{
			m_GameDBAide.AddParameter(TEXT("@lRelationMatchNo"), pRecordDrawScore->lRelationRecordID);
		}

		//执行查询
		m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_RecordDrawScore"), false);		

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//设置缓冲
		*pdwControlCode |= DB_RESULT_DEFER_DATA;

		return false;
	}

	return true;
}

//用户权限
bool CDataBaseEngineSink::OnRequestManageUserRight(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize==sizeof(DBR_GR_ManageUserRight));
		if (wDataSize!=sizeof(DBR_GR_ManageUserRight)) return false;

		//请求处理
		DBR_GR_ManageUserRight * pManageUserRight=(DBR_GR_ManageUserRight *)pData;

		if((pManageUserRight->dwAddRight&UR_MATCH_USER)!=0 || (pManageUserRight->dwRemoveRight&UR_MATCH_USER)!=0)
		{
			//构造参数
			m_GameDBAide.ResetParameter();
			m_GameDBAide.AddParameter(TEXT("@dwUserID"),pManageUserRight->dwUserID);
			m_GameDBAide.AddParameter(TEXT("@dwAddRight"),pManageUserRight->dwAddRight&UR_MATCH_USER);
			m_GameDBAide.AddParameter(TEXT("@dwRemoveRight"),pManageUserRight->dwRemoveRight&UR_MATCH_USER);

			m_GameDBAide.ExecuteProcess(TEXT("GSP_GR_ManageUserRight"),false);

			if((pManageUserRight->dwAddRight&~UR_MATCH_USER)==0 && (pManageUserRight->dwRemoveRight&~UR_MATCH_USER)==0)
			{
				return true;
			}
			else
			{
				pManageUserRight->dwAddRight=(pManageUserRight->dwAddRight&~UR_MATCH_USER);
				pManageUserRight->dwRemoveRight=(pManageUserRight->dwRemoveRight&~UR_MATCH_USER);
			}
		}

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"),pManageUserRight->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwAddRight"),pManageUserRight->dwAddRight);
		m_AccountsDBAide.AddParameter(TEXT("@dwRemoveRight"),pManageUserRight->dwRemoveRight);

		//执行查询
		LONG lReturnValue=m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_ManageUserRight"),false);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//赠送礼物
bool CDataBaseEngineSink::OnRequestSendGift(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
    try
    {
        //效验参数
        ASSERT(wDataSize == sizeof(DBR_GR_SendGift));
        if (wDataSize != sizeof(DBR_GR_SendGift)) return false;

        //请求处理
		DBR_GR_SendGift* pSendGift = (DBR_GR_SendGift*)pData;

        //转化地址
        TCHAR szClientIP[16] = TEXT("");
        BYTE* pClientIP = (BYTE*)&pSendGift->dwClientIP;
        _sntprintf_s(szClientIP, CountArray(szClientIP), TEXT("%d.%d.%d.%d"), pClientIP[0], pClientIP[1], pClientIP[2], pClientIP[3]);

        //构造参数
        m_AccountsDBAide.ResetParameter();
        m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pSendGift->dwUserID);
        m_AccountsDBAide.AddParameter(TEXT("@dwTargetUserID"), pSendGift->dwTargetUserID);
        m_AccountsDBAide.AddParameter(TEXT("@cbIsGoldRoom"), pSendGift->cbIsGoldRoom);
        m_AccountsDBAide.AddParameter(TEXT("@lCellScore"), pSendGift->lCellScore);
        m_AccountsDBAide.AddParameter(TEXT("@lMinTableScore"), pSendGift->lMinTableScore);
        m_AccountsDBAide.AddParameter(TEXT("@dwGoodsID"), pSendGift->dwGoodsID);
        m_AccountsDBAide.AddParameter(TEXT("@dwGoodsCount"), pSendGift->dwGoodsCount);
        m_AccountsDBAide.AddParameter(TEXT("@strClientIP"), szClientIP);

        //输出变量
        TCHAR szDescribe[128] = TEXT("");
        m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribe, sizeof(szDescribe), adParamOutput);

        //执行查询
        LONG lReturnValue = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_SendGift"), true);

		//执行成
		if (lReturnValue == DB_SUCCESS)
		{
            //变量定义
			DBO_GR_SendGiftSuccess OperateSuccess;
            ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

            //构造变量
			OperateSuccess.dwUserID = pSendGift->dwUserID;
            OperateSuccess.dwTargetUserID = pSendGift->dwTargetUserID;
            OperateSuccess.wMoneyID = m_AccountsDBAide.GetValue_WORD(TEXT("MoneyID"));
            OperateSuccess.lGiftPrice = m_AccountsDBAide.GetValue_LONGLONG(TEXT("Price"));
            OperateSuccess.dwLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));
            OperateSuccess.dwMyLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("MyLoveLiness"));
            OperateSuccess.dwTargetLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("TargetLoveLiness"));
            OperateSuccess.dwGoodsID = pSendGift->dwGoodsID;
            OperateSuccess.dwGoodsCount = pSendGift->dwGoodsCount;

            //发送结果
            m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SEND_GIFT_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, sizeof(OperateSuccess));
		}
		else
		{
            //变量定义
			DBO_GR_SendGiftFailure OperateFailure;
            ZeroMemory(&OperateFailure, sizeof(OperateFailure));

            //获取信息
            CDBVarValue DBVarValue;
            m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

            //构造变量
            OperateFailure.lErrorCode = m_AccountsDBModule->GetReturnValue();
            StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CW2CT(DBVarValue.bstrVal).m_psz);

            //发送结果
            WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
            WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SEND_GIFT_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);
		}

        return true;
    }
    catch (IDataBaseException* pIException)
    {
        TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//变量定义
		DBO_GR_SendGiftFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//构造变量
		OperateFailure.lErrorCode = -1;
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), CWHService::FormatMessage(MIM_SERVER_0113, TEXT("由于数据库操作异常，请您稍后重试！")));

		//发送结果
		WORD wStringSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wSendSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SEND_GIFT_FAILURE, dwContextID, dwTokenID, &OperateFailure, wSendSize);

        return false;
    }

    return true;
}

//更新魅力
bool CDataBaseEngineSink::UpdateUserLoveLiness(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID)
{
    try
    {
        //构造参数
        m_AccountsDBAide.ResetParameter();
        m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
        LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GR_QueryUserLoveLiness"), true);

        //加载成功
        if (lResultCode == DB_SUCCESS)
        {
            //更新财富
            DBO_GR_UpdateLoveLiness LoveLinessUpdate;
            ZeroMemory(&LoveLinessUpdate, sizeof(LoveLinessUpdate));

            //设置变量
            LoveLinessUpdate.dwLoveLiness = m_AccountsDBAide.GetValue_DWORD(TEXT("LoveLiness"));

            //发送结果			
            m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_LOVELINESS, dwContextID, dwTokenID, &LoveLinessUpdate, sizeof(LoveLinessUpdate));
        }

        return true;
    }
    catch (IDataBaseException* pIException)
    {
        TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
    }

    return false;
}

//更新魅力
bool CDataBaseEngineSink::OnRequestUpdateLoveLiness(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
    //效验参数
    ASSERT(wDataSize == sizeof(DBR_GR_UpdateLoveLiness));
    if (wDataSize != sizeof(DBR_GR_UpdateLoveLiness)) return false;

    //请求处理
    DBR_GR_UpdateLoveLiness* pUpdateLoveLiness = (DBR_GR_UpdateLoveLiness*)pData;

    //更新魅力
    UpdateUserLoveLiness(pUpdateLoveLiness->dwUserID, dwContextID, dwTokenID);

	return  true;
}

//////////////////////////////////////////////////////////////////////////

