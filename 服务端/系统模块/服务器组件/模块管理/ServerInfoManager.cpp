#include "StdAfx.h"
#include "ModuleDBParameter.h"
#include "ServerInfoManager.h"
#include "ModuleInfoManager.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerInfoBuffer::CServerInfoBuffer()
{
}

//析构函数
CServerInfoBuffer::~CServerInfoBuffer()
{
	//变量定义
	WORD wServerID=0;
	tagGameServerInfo * pGameServerInfo=NULL;
	POSITION Position=m_GameServerInfoMap.GetStartPosition();

	//删除索引
	while (Position!=NULL)
	{
		m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);
		SafeDelete(pGameServerInfo);
	}

	//删除数组
	for (INT_PTR i=0;i<m_GameServerInfoArray.GetCount();i++)
	{
		pGameServerInfo=m_GameServerInfoArray[i];
		SafeDelete(pGameServerInfo);
	}

	//删除引用
	m_GameServerInfoMap.RemoveAll();
	m_GameServerInfoArray.RemoveAll();

	return;
}

//重置数据
bool CServerInfoBuffer::ResetServerInfo()
{
	//变量定义
	WORD wServerID=0;
	tagGameServerInfo * pGameServerInfo=NULL;
	POSITION Position=m_GameServerInfoMap.GetStartPosition();

	//删除对象
	while (Position!=NULL)
	{
		m_GameServerInfoMap.GetNextAssoc(Position,wServerID,pGameServerInfo);
		m_GameServerInfoArray.Add(pGameServerInfo);
	}

	//删除索引
	m_GameServerInfoMap.RemoveAll();

	return true;
}

//删除数据
bool CServerInfoBuffer::DeleteServerInfo(WORD wServerID)
{
	//查找类型
	tagGameServerInfo * pGameServerInfo=NULL;
	if (m_GameServerInfoMap.Lookup(wServerID,pGameServerInfo)==FALSE) return false;

	//删除数据
	m_GameServerInfoMap.RemoveKey(wServerID);
	m_GameServerInfoArray.Add(pGameServerInfo);

	//设置变量
	ZeroMemory(pGameServerInfo,sizeof(tagGameServerInfo));

	return true;
}

//插入数据
bool CServerInfoBuffer::InsertServerInfo(tagGameServerInfo * pGameServerInfo)
{
	//效验参数
	ASSERT(pGameServerInfo!=NULL);
	if (pGameServerInfo==NULL) return false;

	//查找现存
	WORD wServerID=pGameServerInfo->wServerID;
	tagGameServerInfo * pGameServerInsert=SearchServerInfo(wServerID);

	//创建判断
	if (pGameServerInsert==NULL)
	{
		//创建对象
		pGameServerInsert=CreateServerInfo();

		//结果判断
		if (pGameServerInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//设置数据
	m_GameServerInfoMap[wServerID]=pGameServerInsert;
	CopyMemory(pGameServerInsert,pGameServerInfo,sizeof(tagGameServerInfo));

	return true;
}

//获取数目
DWORD CServerInfoBuffer::GetServerInfoCount()
{
	return (DWORD)(m_GameServerInfoMap.GetCount());
}

//查找数据
tagGameServerInfo * CServerInfoBuffer::SearchServerInfo(WORD wServerID)
{
	tagGameServerInfo * pGameServerInfo=NULL;
	m_GameServerInfoMap.Lookup(wServerID,pGameServerInfo);
	return pGameServerInfo;
}

//创建对象
tagGameServerInfo * CServerInfoBuffer::CreateServerInfo()
{
	//变量定义
	tagGameServerInfo * pGameServerInfo=NULL;

	//创建对象
	try
	{
		INT_PTR nArrayCount=m_GameServerInfoArray.GetCount();
		if (nArrayCount>0)
		{
			pGameServerInfo=m_GameServerInfoArray[nArrayCount-1];
			m_GameServerInfoArray.RemoveAt(nArrayCount-1);
		}
		else
		{
			pGameServerInfo=new tagGameServerInfo;
			if (pGameServerInfo==NULL) return NULL;
		}
	}
	catch (...) { return false; }

	//设置变量
	ZeroMemory(pGameServerInfo,sizeof(tagGameServerInfo));

	return pGameServerInfo;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CServerInfoManager::CServerInfoManager()
{
}

//析构函数
CServerInfoManager::~CServerInfoManager()
{
}

//加载房间
bool CServerInfoManager::LoadGameServerInfo(WORD wServerID, tagGameServerInfo & GameServerResult)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//读取列表
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),TEXT(""));

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//读取列表
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameServerInfo"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取数据
		if (PlatformDBModule->IsRecordsetEnd()==false)
		{
			ReadGameServerInfo(PlatformDBAide,GameServerResult);
			return true;
		}

		return false;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//加载房间
bool CServerInfoManager::LoadGameServerInfo(TCHAR szMachineID[LEN_MACHINE_ID], CServerInfoBuffer & ServerInfoBuffer)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//插入参数
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),0);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),szMachineID);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//读取列表
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameServerInfo"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//清空列表
		ServerInfoBuffer.ResetServerInfo();

		//读取列表
		while (PlatformDBModule->IsRecordsetEnd()==false)
		{
			//读取数据
			tagGameServerInfo GameServerResult;
			ReadGameServerInfo(PlatformDBAide,GameServerResult);

			//插入列表
			ServerInfoBuffer.InsertServerInfo(&GameServerResult);

			//移动记录
			PlatformDBModule->MoveToNext();
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//删除房间
bool CServerInfoManager::DeleteGameServer(WORD wServerID)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//插入参数
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),wServerID);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_DeleteGameServer"),false)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//插入房间
bool CServerInfoManager::InsertGameServer(tagCreateServerInfo * pCreateServerInfo, tagGameServerInfo & GameServerResult)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//插入参数
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wModuleID"),pCreateServerInfo->wModuleID);
		PlatformDBAide.AddParameter(TEXT("@wKindID"),pCreateServerInfo->wKindID);
		PlatformDBAide.AddParameter(TEXT("@wNodeID"),pCreateServerInfo->wNodeID);
		PlatformDBAide.AddParameter(TEXT("@wSortID"),pCreateServerInfo->wSortID);
		PlatformDBAide.AddParameter(TEXT("@wVisibleMask"),pCreateServerInfo->wVisibleMask);
		PlatformDBAide.AddParameter(TEXT("@strServerName"),pCreateServerInfo->szServerName);
		PlatformDBAide.AddParameter(TEXT("@wTableCount"),pCreateServerInfo->wTableCount);
		PlatformDBAide.AddParameter(TEXT("@wServerType"),pCreateServerInfo->wServerType);
		PlatformDBAide.AddParameter(TEXT("@wServerPort"),pCreateServerInfo->wServerPort);
		PlatformDBAide.AddParameter(TEXT("@wServerLevel"), pCreateServerInfo->wServerLevel);
		PlatformDBAide.AddParameter(TEXT("@cbMatchType"), pCreateServerInfo->cbMatchType);
		PlatformDBAide.AddParameter(TEXT("@lCellScore"),pCreateServerInfo->lCellScore);
		PlatformDBAide.AddParameter(TEXT("@lLessScore"),pCreateServerInfo->lLessScore);
		PlatformDBAide.AddParameter(TEXT("@lRestrictScore"),pCreateServerInfo->lRestrictScore);
		PlatformDBAide.AddParameter(TEXT("@lRevenue"),pCreateServerInfo->lRevenue);
		PlatformDBAide.AddParameter(TEXT("@lExpGainRate"), pCreateServerInfo->lExpGainRate);
		PlatformDBAide.AddParameter(TEXT("@wDBListID"),pCreateServerInfo->wDBListID);
		PlatformDBAide.AddParameter(TEXT("@lMinEnterScore"),pCreateServerInfo->lMinEnterScore);
		PlatformDBAide.AddParameter(TEXT("@lMaxEnterScore"),pCreateServerInfo->lMaxEnterScore);
		PlatformDBAide.AddParameter(TEXT("@lMinEnterMember"),pCreateServerInfo->lMinEnterMember);
		PlatformDBAide.AddParameter(TEXT("@lMaxEnterMember"),pCreateServerInfo->lMaxEnterMember);		
		
		PlatformDBAide.AddParameter(TEXT("@cbDistributeMode"),pCreateServerInfo->cbDistributeMode);
		PlatformDBAide.AddParameter(TEXT("@cbDistributeRule"),pCreateServerInfo->cbDistributeRule);
		PlatformDBAide.AddParameter(TEXT("@wMinPlayerCount"),pCreateServerInfo->wMinPlayerCount);
		PlatformDBAide.AddParameter(TEXT("@wMinDistributeUser"),pCreateServerInfo->wMinDistributeUser);
		PlatformDBAide.AddParameter(TEXT("@wMinPartakeGameUser"),pCreateServerInfo->wMinPartakeGameUser);
		PlatformDBAide.AddParameter(TEXT("@wMaxPartakeGameUser"),pCreateServerInfo->wMaxPartakeGameUser);
		PlatformDBAide.AddParameter(TEXT("@wDistributeTimeSpace"),pCreateServerInfo->wDistributeTimeSpace);

		PlatformDBAide.AddParameter(TEXT("@dwBattleRule"),pCreateServerInfo->dwBattleRule);
		PlatformDBAide.AddParameter(TEXT("@wMaxPlayer"),pCreateServerInfo->wMaxPlayer);
		PlatformDBAide.AddParameter(TEXT("@dwServerRule"),pCreateServerInfo->dwServerRule);
		PlatformDBAide.AddParameter(TEXT("@dwAttachUserRight"),pCreateServerInfo->dwAttachUserRight);
		PlatformDBAide.AddParameter(TEXT("@strServiceAddr"),pCreateServerInfo->szServiceAddr);
		PlatformDBAide.AddParameter(TEXT("@strServiceDomain"),pCreateServerInfo->szServiceDomain);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),pCreateServerInfo->szServiceMachine);		
		PlatformDBAide.AddParameter(TEXT("@cbCustomRule"), pCreateServerInfo->cbCustomRule, sizeof(pCreateServerInfo->cbCustomRule));

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_InsertGameServer"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取资料
		ReadGameServerInfo(PlatformDBAide,GameServerResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);
	}

	return false;
}

//修改房间
bool CServerInfoManager::ModifyGameServer(tagCreateServerInfo * pCreateServerInfo, tagGameServerInfo & GameServerResult)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectPlatformDB(PlatformDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//插入参数
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wServerID"),pCreateServerInfo->wServerID);
		PlatformDBAide.AddParameter(TEXT("@wModuleID"),pCreateServerInfo->wModuleID);
		PlatformDBAide.AddParameter(TEXT("@wKindID"),pCreateServerInfo->wKindID);
		PlatformDBAide.AddParameter(TEXT("@wNodeID"),pCreateServerInfo->wNodeID);
		PlatformDBAide.AddParameter(TEXT("@wSortID"),pCreateServerInfo->wSortID);
		PlatformDBAide.AddParameter(TEXT("@wVisibleMask"),pCreateServerInfo->wVisibleMask);
		PlatformDBAide.AddParameter(TEXT("@strServerName"),pCreateServerInfo->szServerName);
		PlatformDBAide.AddParameter(TEXT("@wTableCount"),pCreateServerInfo->wTableCount);
		PlatformDBAide.AddParameter(TEXT("@wServerType"),pCreateServerInfo->wServerType);
		PlatformDBAide.AddParameter(TEXT("@wServerPort"),pCreateServerInfo->wServerPort);
		PlatformDBAide.AddParameter(TEXT("@wServerLevel"), pCreateServerInfo->wServerLevel);
		PlatformDBAide.AddParameter(TEXT("@cbMatchType"), pCreateServerInfo->cbMatchType);
		PlatformDBAide.AddParameter(TEXT("@lCellScore"),pCreateServerInfo->lCellScore);
		PlatformDBAide.AddParameter(TEXT("@lLessScore"),pCreateServerInfo->lLessScore);
		PlatformDBAide.AddParameter(TEXT("@lRestrictScore"),pCreateServerInfo->lRestrictScore);
		PlatformDBAide.AddParameter(TEXT("@lRevenue"),pCreateServerInfo->lRevenue);
		PlatformDBAide.AddParameter(TEXT("@lExpGainRate"), pCreateServerInfo->lExpGainRate);
		PlatformDBAide.AddParameter(TEXT("@wDBListID"),pCreateServerInfo->wDBListID);
		PlatformDBAide.AddParameter(TEXT("@lMinEnterScore"),pCreateServerInfo->lMinEnterScore);
		PlatformDBAide.AddParameter(TEXT("@lMaxEnterScore"),pCreateServerInfo->lMaxEnterScore);
		PlatformDBAide.AddParameter(TEXT("@lMinEnterMember"),pCreateServerInfo->lMinEnterMember);
		PlatformDBAide.AddParameter(TEXT("@lMaxEnterMember"),pCreateServerInfo->lMaxEnterMember);		

		PlatformDBAide.AddParameter(TEXT("@cbDistributeMode"),pCreateServerInfo->cbDistributeMode);
		PlatformDBAide.AddParameter(TEXT("@cbDistributeRule"),pCreateServerInfo->cbDistributeRule);
		PlatformDBAide.AddParameter(TEXT("@wMinPlayerCount"),pCreateServerInfo->wMinPlayerCount);
		PlatformDBAide.AddParameter(TEXT("@wMinDistributeUser"),pCreateServerInfo->wMinDistributeUser);
		PlatformDBAide.AddParameter(TEXT("@wMinPartakeGameUser"),pCreateServerInfo->wMinPartakeGameUser);
		PlatformDBAide.AddParameter(TEXT("@wMaxPartakeGameUser"),pCreateServerInfo->wMaxPartakeGameUser);
		PlatformDBAide.AddParameter(TEXT("@wDistributeTimeSpace"),pCreateServerInfo->wDistributeTimeSpace);

		PlatformDBAide.AddParameter(TEXT("@dwBattleRule"),pCreateServerInfo->dwBattleRule);
		PlatformDBAide.AddParameter(TEXT("@wMaxPlayer"),pCreateServerInfo->wMaxPlayer);
		PlatformDBAide.AddParameter(TEXT("@dwServerRule"),pCreateServerInfo->dwServerRule);
		PlatformDBAide.AddParameter(TEXT("@dwAttachUserRight"),pCreateServerInfo->dwAttachUserRight);
		PlatformDBAide.AddParameter(TEXT("@strServiceAddr"),pCreateServerInfo->szServiceAddr);
		PlatformDBAide.AddParameter(TEXT("@strServiceDomain"),pCreateServerInfo->szServiceDomain);
		PlatformDBAide.AddParameter(TEXT("@strServiceMachine"),pCreateServerInfo->szServiceMachine);
		PlatformDBAide.AddParameter(TEXT("@cbCustomRule"), pCreateServerInfo->cbCustomRule, sizeof(pCreateServerInfo->cbCustomRule));
		
		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行命令
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_ModifyGameServer"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取资料
		ReadGameServerInfo(PlatformDBAide,GameServerResult);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		CTraceService::TraceString(pszDescribe,TraceLevel_Exception);

		//错误提示
		AfxMessageBox(pszDescribe,MB_ICONERROR);

		return false;
	}

	return false;
}

//连接数据
bool CServerInfoManager::ConnectPlatformDB(CDataBaseHelper & PlatformDBModule)
{
	//获取参数
	CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

	//创建对象
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("CreateGameServer 创建 PlatformDBModule 对象失败"),MB_ICONERROR);
		return false;
	}

	//设置连接
	PlatformDBModule->SetConnectionInfo(pDataBaseParameter->dwDataBaseAddr,pDataBaseParameter->wDataBasePort,
	CW2CT(pDataBaseParameter->szDataBaseName),CW2CT(pDataBaseParameter->szDataBaseUser),CW2CT(pDataBaseParameter->szDataBasePass));

	//发起连接
	PlatformDBModule->OpenConnection();

	return true;
}

//读取房间
bool CServerInfoManager::ReadGameServerInfo(CDataBaseAide & PlatformDBAide, tagGameServerInfo & GameServerResult)
{
	//设置变量
	ZeroMemory(&GameServerResult,sizeof(GameServerResult));

	//索引配置
	GameServerResult.wServerID=PlatformDBAide.GetValue_WORD(TEXT("ServerID"));
	GameServerResult.wModuleID=PlatformDBAide.GetValue_WORD(TEXT("ModuleID"));
	GameServerResult.wDBListID = PlatformDBAide.GetValue_WORD(TEXT("DBListID"));

	//挂接配置
	GameServerResult.wKindID=PlatformDBAide.GetValue_WORD(TEXT("KindID"));
	GameServerResult.wNodeID=PlatformDBAide.GetValue_WORD(TEXT("NodeID"));
	GameServerResult.wSortID=PlatformDBAide.GetValue_WORD(TEXT("SortID"));
	GameServerResult.wVisibleMask=PlatformDBAide.GetValue_WORD(TEXT("VisibleMask"));

	//积分配置
	GameServerResult.lRevenue=PlatformDBAide.GetValue_LONG(TEXT("ServiceScore"));
	GameServerResult.lCellScore=PlatformDBAide.GetValue_LONG(TEXT("CellScore"));
	GameServerResult.lExpGainRate = PlatformDBAide.GetValue_LONG(TEXT("ExpGainRate"));
	GameServerResult.lLessScore=PlatformDBAide.GetValue_LONGLONG(TEXT("MinTableScore"));
	GameServerResult.lRestrictScore=PlatformDBAide.GetValue_LONGLONG(TEXT("RestrictScore"));
	GameServerResult.lMinEnterScore=PlatformDBAide.GetValue_LONGLONG(TEXT("MinEnterScore"));
	GameServerResult.lMaxEnterScore=PlatformDBAide.GetValue_LONGLONG(TEXT("MaxEnterScore"));
	GameServerResult.lMinEnterMember=PlatformDBAide.GetValue_LONG(TEXT("MinEnterMember"));
	GameServerResult.lMaxEnterMember=PlatformDBAide.GetValue_LONG(TEXT("MaxEnterMember"));

	//约战规则
	GameServerResult.dwBattleRule=PlatformDBAide.GetValue_DWORD(TEXT("BattleRule"));

	//分组参数
	GameServerResult.cbDistributeMode=PlatformDBAide.GetValue_BYTE(TEXT("DistributeMode"));
	GameServerResult.cbDistributeRule=PlatformDBAide.GetValue_BYTE(TEXT("DistributeRule"));
	GameServerResult.wMinPlayerCount=PlatformDBAide.GetValue_WORD(TEXT("MinPlayerCount"));
	GameServerResult.wMinDistributeUser=PlatformDBAide.GetValue_WORD(TEXT("MinDistributeUser"));
	GameServerResult.wMinPartakeGameUser=PlatformDBAide.GetValue_WORD(TEXT("MinPartakeGameUser"));
	GameServerResult.wMaxPartakeGameUser=PlatformDBAide.GetValue_WORD(TEXT("MaxPartakeGameUser"));
	GameServerResult.wDistributeTimeSpace=PlatformDBAide.GetValue_WORD(TEXT("DistributeTimeSpace"));

	//房间信息
	GameServerResult.wMaxPlayer=PlatformDBAide.GetValue_WORD(TEXT("MaxPlayer"));
	GameServerResult.wTableCount=PlatformDBAide.GetValue_WORD(TEXT("TableCount"));
	GameServerResult.wServerType=PlatformDBAide.GetValue_WORD(TEXT("ServerType"));
	GameServerResult.wServerPort=PlatformDBAide.GetValue_WORD(TEXT("ServerPort"));
	GameServerResult.wServerLevel = PlatformDBAide.GetValue_WORD(TEXT("ServerLevel"));
	GameServerResult.cbMatchType = PlatformDBAide.GetValue_BYTE(TEXT("MatchType"));
	GameServerResult.dwServerRule=PlatformDBAide.GetValue_DWORD(TEXT("ServerRule"));
	GameServerResult.dwAttachUserRight=PlatformDBAide.GetValue_DWORD(TEXT("AttachUserRight"));

	PlatformDBAide.GetValue_String(TEXT("ServerName"),GameServerResult.szServerName,CountArray(GameServerResult.szServerName));

	//组件属性
	GameServerResult.dwModuleVersion=PlatformDBAide.GetValue_DWORD(TEXT("ServerVersion"));
	GameServerResult.dwClientVersion=PlatformDBAide.GetValue_DWORD(TEXT("ClientVersion"));
	PlatformDBAide.GetValue_String(TEXT("ModuleName"),GameServerResult.szModuleName,CountArray(GameServerResult.szModuleName));
	PlatformDBAide.GetValue_String(TEXT("ServerDLLName"),GameServerResult.szServerDLLName,CountArray(GameServerResult.szServerDLLName));
	PlatformDBAide.GetValue_String(TEXT("ClientExeName"),GameServerResult.szClientEXEName,CountArray(GameServerResult.szClientEXEName));

	//扩展配置	
	PlatformDBAide.GetValue_Binary(TEXT("CustomRule"), GameServerResult.cbCustomRule,sizeof(GameServerResult.cbCustomRule));
	PlatformDBAide.GetValue_String(TEXT("ServiceMachine"),GameServerResult.szServiceMachine,CountArray(GameServerResult.szServiceMachine));
	
	//服务信息
	PlatformDBAide.GetValue_String(TEXT("ServiceAddr"),GameServerResult.szServiceAddr,CountArray(GameServerResult.szServiceAddr));
	PlatformDBAide.GetValue_String(TEXT("ServiceDomain"),GameServerResult.szServiceDomain,CountArray(GameServerResult.szServiceDomain));

	//本地版本
	LPCTSTR pszServerDLLName=GameServerResult.szServerDLLName;
	CWHService::GetModuleVersion(pszServerDLLName,GameServerResult.dwNativeVersion);

	//如果本地版本有更新
	if( GameServerResult.dwModuleVersion < GameServerResult.dwNativeVersion )
	{
		CModuleInfoManager ModuleInfoManager;
		if( ModuleInfoManager.UpdateGameModuleInfo(GameServerResult.wModuleID,GameServerResult.dwNativeVersion) )
		{
			GameServerResult.dwModuleVersion = GameServerResult.dwNativeVersion;
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
