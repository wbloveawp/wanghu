#include "StdAfx.h"
#include "ModuleDBParameter.h"
#include "ModuleInfoManager.h"

//////////////////////////////////////////////////////////////////////////

//构造函数
CModuleInfoBuffer::CModuleInfoBuffer()
{
}

//析构函数
CModuleInfoBuffer::~CModuleInfoBuffer()
{
	//变量定义
	WORD wModuleID=0;
	tagGameModuleInfo * pGameModuleInfo=NULL;
	POSITION Position=m_GameModuleInfoMap.GetStartPosition();

	//删除索引
	while (Position!=NULL)
	{
		m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameModuleInfo);
		SafeDelete(pGameModuleInfo);
	}

	//删除数组
	for (INT_PTR i=0;i<m_GameModuleInfoArray.GetCount();i++)
	{
		pGameModuleInfo=m_GameModuleInfoArray[i];
		SafeDelete(pGameModuleInfo);
	}

	//删除引用
	m_GameModuleInfoMap.RemoveAll();
	m_GameModuleInfoArray.RemoveAll();

	return;
}

//重置数据
bool CModuleInfoBuffer::ResetModuleInfo()
{
	//变量定义
	WORD wModuleID=0;
	tagGameModuleInfo * pGameModuleInfo=NULL;
	POSITION Position=m_GameModuleInfoMap.GetStartPosition();

	//删除对象
	while (Position!=NULL)
	{
		m_GameModuleInfoMap.GetNextAssoc(Position,wModuleID,pGameModuleInfo);
		m_GameModuleInfoArray.Add(pGameModuleInfo);
	}

	//删除索引
	m_GameModuleInfoMap.RemoveAll();

	return true;
}

//删除数据
bool CModuleInfoBuffer::DeleteModuleInfo(WORD wModuleID)
{
	//查找类型
	tagGameModuleInfo * pGameModuleInfo=NULL;
	if (m_GameModuleInfoMap.Lookup(wModuleID,pGameModuleInfo)==FALSE) return false;

	//删除数据
	m_GameModuleInfoMap.RemoveKey(wModuleID);
	m_GameModuleInfoArray.Add(pGameModuleInfo);

	//设置变量
	ZeroMemory(pGameModuleInfo,sizeof(tagGameModuleInfo));

	return true;
}

//插入数据
bool CModuleInfoBuffer::InsertModuleInfo(tagGameModuleInfo * pGameModuleInfo)
{
	//效验参数
	ASSERT(pGameModuleInfo!=NULL);
	if (pGameModuleInfo==NULL) return false;

	//查找现存
	WORD wModuleID=pGameModuleInfo->wModuleID;
	tagGameModuleInfo * pGameModuleInsert=SearchModuleInfo(wModuleID);

	//创建判断
	if (pGameModuleInsert==NULL)
	{
		//创建对象
		pGameModuleInsert=CreateModuleInfo();

		//结果判断
		if (pGameModuleInsert==NULL)
		{
			ASSERT(FALSE);
			return false;
		}
	}

	//设置数据
	m_GameModuleInfoMap[wModuleID]=pGameModuleInsert;
	CopyMemory(pGameModuleInsert,pGameModuleInfo,sizeof(tagGameModuleInfo));

	return true;
}

//获取数目
DWORD CModuleInfoBuffer::GetModuleInfoCount()
{
	return (DWORD)(m_GameModuleInfoMap.GetCount());
}

//查找数据
tagGameModuleInfo * CModuleInfoBuffer::SearchModuleInfo(WORD wModuleID)
{
	tagGameModuleInfo * pGameModuleInfo=NULL;
	m_GameModuleInfoMap.Lookup(wModuleID,pGameModuleInfo);
	return pGameModuleInfo;
}

//创建对象
tagGameModuleInfo * CModuleInfoBuffer::CreateModuleInfo()
{
	//变量定义
	tagGameModuleInfo * pGameModuleInfo=NULL;

	//创建对象
	try
	{
		INT_PTR nArrayCount=m_GameModuleInfoArray.GetCount();
		if (nArrayCount>0)
		{
			pGameModuleInfo=m_GameModuleInfoArray[nArrayCount-1];
			m_GameModuleInfoArray.RemoveAt(nArrayCount-1);
		}
		else
		{
			pGameModuleInfo=new tagGameModuleInfo;
			if (pGameModuleInfo==NULL) return NULL;
		}
	}
	catch (...) { return false; }

	//设置变量
	ZeroMemory(pGameModuleInfo,sizeof(tagGameModuleInfo));

	return pGameModuleInfo;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CModuleInfoManager::CModuleInfoManager()
{
}

//析构函数
CModuleInfoManager::~CModuleInfoManager()
{
}

//注册模块
bool CModuleInfoManager::RegisterGameModule(LPCTSTR pszModuleName)
{
	return true;
}

//注销模块
bool CModuleInfoManager::UnRegisterGameModule(LPCTSTR pszModuleName)
{
	return true;
}

//加载模块
bool CModuleInfoManager::LoadGameModuleInfo(CModuleInfoBuffer & ModuleInfoBuffer)
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//创建对象
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("LoadGameModuleInfo 创建 PlatformDBModule 对象失败"),MB_ICONERROR);
		return false;
	}

	//连接数据库
	try
	{
		//变量定义
		CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
		tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

		//设置连接
		PlatformDBModule->SetConnectionInfo(pDataBaseParameter->dwDataBaseAddr,pDataBaseParameter->wDataBasePort,
		CW2CT(pDataBaseParameter->szDataBaseName),CW2CT(pDataBaseParameter->szDataBaseUser),CW2CT(pDataBaseParameter->szDataBasePass));

		//发起连接
		PlatformDBModule->OpenConnection();
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//读取列表
		PlatformDBAide.ResetParameter();
		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_LoadGameModuleInfo"),true)==DB_SUCCESS)
		{
			//清空列表
			ModuleInfoBuffer.ResetModuleInfo();

			//读取列表
			while (PlatformDBModule->IsRecordsetEnd()==false)
			{
				//变量定义
				tagGameModuleInfo GameModuleInfo;
				ZeroMemory(&GameModuleInfo,sizeof(GameModuleInfo));

				//读取信息
				GameModuleInfo.wModuleID=PlatformDBAide.GetValue_WORD(TEXT("ModuleID"));
				GameModuleInfo.wDBListID = PlatformDBAide.GetValue_WORD(TEXT("DBListID"));
				GameModuleInfo.wSupportType = PlatformDBAide.GetValue_WORD(TEXT("SupportType"));
				GameModuleInfo.dwModuleVersion=PlatformDBAide.GetValue_DWORD(TEXT("ServerVersion"));
				GameModuleInfo.dwClientVersion = PlatformDBAide.GetValue_DWORD(TEXT("ClientVersion"));
				PlatformDBAide.GetValue_String(TEXT("ModuleName"),GameModuleInfo.szModuleName,CountArray(GameModuleInfo.szModuleName));
				PlatformDBAide.GetValue_String(TEXT("ServerDLLName"),GameModuleInfo.szServerDLLName,CountArray(GameModuleInfo.szServerDLLName));
				PlatformDBAide.GetValue_String(TEXT("ClientExeName"),GameModuleInfo.szClientEXEName,CountArray(GameModuleInfo.szClientEXEName));

				//本地版本
				LPCTSTR pszServerDLLName=GameModuleInfo.szServerDLLName;
				CWHService::GetModuleVersion(pszServerDLLName,GameModuleInfo.dwNativeVersion);

				//列表处理
				ModuleInfoBuffer.InsertModuleInfo(&GameModuleInfo);

				//移动记录
				PlatformDBModule->MoveToNext();
			}
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

//修改模块
bool CModuleInfoManager::UpdateGameModuleInfo( tagGameModuleInfo &GameModuleInfo )
{
	//变量定义
	CDataBaseAide PlatformDBAide;
	CDataBaseHelper PlatformDBModule;

	//创建对象
	if ((PlatformDBModule.GetInterface()==NULL)&&(PlatformDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("UpdateGameModuleInfo 创建 PlatformDBModule 对象失败"),MB_ICONERROR);
		return false;
	}

	//连接数据库
	try
	{
		//变量定义
		CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
		tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetPlatformDBParameter();

		//设置连接
		PlatformDBModule->SetConnectionInfo(pDataBaseParameter->dwDataBaseAddr,pDataBaseParameter->wDataBasePort,
		CW2CT(pDataBaseParameter->szDataBaseName),CW2CT(pDataBaseParameter->szDataBaseUser),CW2CT(pDataBaseParameter->szDataBasePass));

		//发起连接
		PlatformDBModule->OpenConnection();
		PlatformDBAide.SetDataBase(PlatformDBModule.GetInterface());

		//读取列表
		PlatformDBAide.ResetParameter();
		PlatformDBAide.AddParameter(TEXT("@wModuleID"),GameModuleInfo.wModuleID);
		PlatformDBAide.AddParameter(TEXT("@wDBListID"),GameModuleInfo.wDBListID);
		PlatformDBAide.AddParameter(TEXT("@wSupportType"),GameModuleInfo.wSupportType);
		PlatformDBAide.AddParameter(TEXT("@szModuleName"),GameModuleInfo.szModuleName);
		PlatformDBAide.AddParameter(TEXT("@dwModuleVerion"),GameModuleInfo.dwNativeVersion);
		PlatformDBAide.AddParameter(TEXT("@dwClientVerion"),GameModuleInfo.dwClientVersion);
		PlatformDBAide.AddParameter(TEXT("@szServerDLLName"),GameModuleInfo.szServerDLLName);
		PlatformDBAide.AddParameter(TEXT("@szClientExeName"),GameModuleInfo.szClientEXEName);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		PlatformDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		if (PlatformDBAide.ExecuteProcess(TEXT("GSP_UpdateGameModuleInfo"),true)!=DB_SUCCESS)
		{
			//获取信息
			PlatformDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取信息
		GameModuleInfo.wModuleID=PlatformDBAide.GetValue_WORD(TEXT("ModuleID"));
		GameModuleInfo.wDBListID = PlatformDBAide.GetValue_WORD(TEXT("DBListID"));
		GameModuleInfo.wSupportType = PlatformDBAide.GetValue_WORD(TEXT("SupportType"));
		GameModuleInfo.dwModuleVersion=PlatformDBAide.GetValue_DWORD(TEXT("ServerVersion"));
		GameModuleInfo.dwClientVersion=PlatformDBAide.GetValue_DWORD(TEXT("ClientVersion"));
		PlatformDBAide.GetValue_String(TEXT("ModuleName"),GameModuleInfo.szModuleName,CountArray(GameModuleInfo.szModuleName));
		PlatformDBAide.GetValue_String(TEXT("ServerDLLName"),GameModuleInfo.szServerDLLName,CountArray(GameModuleInfo.szServerDLLName));
		PlatformDBAide.GetValue_String(TEXT("ClientExeName"),GameModuleInfo.szClientEXEName,CountArray(GameModuleInfo.szClientEXEName));

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

//修改模块
bool CModuleInfoManager::UpdateGameModuleInfo( WORD wModuleID, DWORD dwModuleVersion)
{
	//变量定义
	CModuleInfoBuffer ModuleInfoBuffer;
	
	//加载模块
	if( LoadGameModuleInfo(ModuleInfoBuffer) )
	{
		//搜索模块
		tagGameModuleInfo *pGameModuleInfo = ModuleInfoBuffer.SearchModuleInfo(wModuleID);
		if( pGameModuleInfo == NULL )
		{
			TCHAR szDescribeString[]=TEXT("相应的模块信息不存在，模块修改失败！");

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		pGameModuleInfo->dwNativeVersion = dwModuleVersion;
		return UpdateGameModuleInfo(*pGameModuleInfo);
	}

	return false;
}

//模块属性
bool CModuleInfoManager::GetGameServiceAttrib(LPCTSTR pszModuleName, tagGameServiceAttrib & GameServiceAttrib)
{
	//设置变量
	ZeroMemory(&GameServiceAttrib,sizeof(GameServiceAttrib));

	//游戏模块
	CGameServiceManagerHelper GameServiceManager;
	GameServiceManager.SetModuleCreateInfo(pszModuleName,GAME_SERVICE_CREATE_NAME);

	//加载模块
	if (GameServiceManager.CreateInstance()==true)
	{
		GameServiceManager->GetGameServiceAttrib(GameServiceAttrib);
		return true;
	}

	return false;
}

//////////////////////////////////////////////////////////////////////////
