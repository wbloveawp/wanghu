#include "StdAfx.h"
#include "MatchInfoManager.h"
#include "ModuleDBParameter.h"
#include "ModuleInfoManager.h"

//////////////////////////////////////////////////////////////////////////
//构造函数
CMatchInfoBuffer::CMatchInfoBuffer()
{
}

//析构函数
CMatchInfoBuffer::~CMatchInfoBuffer()
{
	//变量定义
	tagMatchOption * pMatchOption=NULL;

	//删除数组
	m_MatchOptionBuffer.Append(m_MatchOptionArray);
	for (INT_PTR i=0;i<m_MatchOptionBuffer.GetCount();i++)
	{
		pMatchOption=m_MatchOptionBuffer[i];
		SafeDelete(pMatchOption);
	}

	//删除引用
	m_MatchOptionArray.RemoveAll();
	m_MatchOptionBuffer.RemoveAll();

	return;
}

//重置数据
bool CMatchInfoBuffer::ResetMatchOption()
{
	//添加对象
	m_MatchOptionBuffer.Append(m_MatchOptionArray);

	//删除元素
	m_MatchOptionArray.RemoveAll();

	return true;
}

//删除数据
bool CMatchInfoBuffer::DeleteMatchOption(DWORD dwMatchID)
{
	//变量定义
	tagMatchOption * pMatchOption=NULL;

	//查找对象
	for(INT_PTR i=0;i<m_MatchOptionArray.GetCount();i++)
	{
		pMatchOption=m_MatchOptionArray[i];
		if(pMatchOption->dwMatchID==dwMatchID)
		{
			//删除数据
			m_MatchOptionArray.RemoveAt(i);
			m_MatchOptionBuffer.Add(pMatchOption);

			//设置变量
			ZeroMemory(pMatchOption,sizeof(tagMatchOption));

			break;
		}
	}	

	return true;
}

//插入数据
tagMatchOption * CMatchInfoBuffer::InsertMatchOption(tagMatchOption * pMatchOption)
{
	//效验参数
	ASSERT(pMatchOption!=NULL);
	if (pMatchOption==NULL) return NULL;

	//查找现存
	tagMatchOption * pGameMatchInsert=SearchMatchOption(pMatchOption->dwMatchID);

	//创建判断
	if (pGameMatchInsert==NULL)
	{
		//创建对象
		pGameMatchInsert=CreateMatchOption();

		//结果判断
		if (pGameMatchInsert==NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		//添加数据
		m_MatchOptionArray.Add(pGameMatchInsert);
	}

	//拷贝数据	
	CopyMemory(pGameMatchInsert,pMatchOption,sizeof(tagMatchOption));

	return pGameMatchInsert;
}

//获取数目
DWORD CMatchInfoBuffer::GetMatchOptionCount()
{
	return (DWORD)(m_MatchOptionArray.GetCount());
}

//查找数据
tagMatchOption * CMatchInfoBuffer::SearchMatchOption(DWORD dwMatchID)
{
	//变量定义
	tagMatchOption * pMatchOption=NULL;

	//查找对象
	for(INT_PTR i=0;i<m_MatchOptionArray.GetCount();i++)
	{
		pMatchOption=m_MatchOptionArray[i];
		if(pMatchOption->dwMatchID==dwMatchID)
		{
			return pMatchOption;
		}
	}

	return NULL;
}

//创建对象
tagMatchOption * CMatchInfoBuffer::CreateMatchOption()
{
	//变量定义
	tagMatchOption * pMatchOption=NULL;

	//创建对象
	try
	{
		INT_PTR nArrayCount=m_MatchOptionBuffer.GetCount();
		if (nArrayCount>0)
		{
			pMatchOption=m_MatchOptionBuffer[nArrayCount-1];
			m_MatchOptionBuffer.RemoveAt(nArrayCount-1);
		}
		else
		{
			pMatchOption=new tagMatchOption;
			if (pMatchOption==NULL) return NULL;
		}
	}
	catch (...) 
	{ 
		return NULL; 
	}

	//设置变量
	ZeroMemory(pMatchOption,sizeof(tagMatchOption));

	return pMatchOption;
}

//////////////////////////////////////////////////////////////////////////
//构造函数
CRewardInfoBuffer::CRewardInfoBuffer()
{
}

//析构函数
CRewardInfoBuffer::~CRewardInfoBuffer()
{
	//变量定义
	tagMatchRewardItem * pMatchRewardItem=NULL;

	//删除数组
	m_MatchRewardItemBuffer.Append(m_MatchRewardItemArray);
	for (INT_PTR i=0;i<m_MatchRewardItemBuffer.GetCount();i++)
	{
		pMatchRewardItem=m_MatchRewardItemBuffer[i];
		SafeDelete(pMatchRewardItem);
	}

	//删除引用
	m_MatchRewardItemArray.RemoveAll();
	m_MatchRewardItemBuffer.RemoveAll();

	return;
}

//重置数据
bool CRewardInfoBuffer::ResetMatchRewardItem()
{
	//添加对象
	m_MatchRewardItemBuffer.Append(m_MatchRewardItemArray);

	//删除元素
	m_MatchRewardItemArray.RemoveAll();

	return true;
}

//删除数据
bool CRewardInfoBuffer::DeleteMatchRewardItem(WORD wRankID)
{
	//变量定义
	tagMatchRewardItem * pMatchRewardItem=NULL;

	//查找对象
	for (INT_PTR i=0;i<m_MatchRewardItemArray.GetCount();i++)
	{
		pMatchRewardItem=m_MatchRewardItemArray[i];
		if (pMatchRewardItem->wStartRankID==wRankID)
		{
			//删除数据
			m_MatchRewardItemArray.RemoveAt(i);
			m_MatchRewardItemBuffer.Add(pMatchRewardItem);

			//设置变量
			ZeroMemory(pMatchRewardItem,sizeof(tagMatchRewardItem));

			break;
		}
	}	

	return true;
}

//插入数据
tagMatchRewardItem * CRewardInfoBuffer::InsertMatchRewardItem(tagMatchRewardItem & MatchRewardItem)
{
	//查找现存
	tagMatchRewardItem * pMatchRewardInsert=SearchMatchReward(MatchRewardItem.wStartRankID);

	//创建判断
	if (pMatchRewardInsert==NULL)
	{
		//创建对象
		pMatchRewardInsert=CreateMatchRewardItem();

		//结果判断
		if (pMatchRewardInsert==NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		//添加数据
		m_MatchRewardItemArray.Add(pMatchRewardInsert);		
	}

	//拷贝数据
	CopyMemory(pMatchRewardInsert,&MatchRewardItem,sizeof(tagMatchRewardItem));

	return pMatchRewardInsert;
}

//获取数目
WORD CRewardInfoBuffer::GetMatchRewardCount()
{
	return (WORD)(m_MatchRewardItemArray.GetCount());
}
	
//查找数据
tagMatchRewardItem * CRewardInfoBuffer::SearchMatchReward(WORD wRankID)
{
	//变量定义
	tagMatchRewardItem * pMatchRewardItem=NULL;

	//查找对象
	for (INT_PTR i=0;i<m_MatchRewardItemArray.GetCount();i++)
	{
		pMatchRewardItem=m_MatchRewardItemArray[i];
		if (pMatchRewardItem->wStartRankID<=wRankID && wRankID<=pMatchRewardItem->wEndRankID)
		{
			return pMatchRewardItem;
		}
	}

	return NULL;
}

//创建对象
tagMatchRewardItem * CRewardInfoBuffer::CreateMatchRewardItem()
{
	//变量定义
	tagMatchRewardItem * pMatchRewardItem=NULL;

	//创建对象
	try
	{
		INT_PTR nArrayCount=m_MatchRewardItemBuffer.GetCount();
		if (nArrayCount>0)
		{
			pMatchRewardItem=m_MatchRewardItemBuffer[nArrayCount-1];
			m_MatchRewardItemBuffer.RemoveAt(nArrayCount-1);
		}
		else
		{
			pMatchRewardItem=new tagMatchRewardItem;
			if (pMatchRewardItem==NULL) return NULL;
		}
	}
	catch (...) 
	{ 
		return NULL; 
	}

	//设置变量
	ZeroMemory(pMatchRewardItem,sizeof(tagMatchRewardItem));

	return pMatchRewardItem;
}

//////////////////////////////////////////////////////////////////////////
//构造函数
CRoundInfoBuffer::CRoundInfoBuffer()
{
}

//析构函数
CRoundInfoBuffer::~CRoundInfoBuffer()
{
	//变量定义
	tagMatchRoundItem * pMatchRoundItem=NULL;

	//删除数组
	m_MatchRoundItemBuffer.Append(m_MatchRoundItemArray);
	for (INT_PTR i=0;i<m_MatchRoundItemBuffer.GetCount();i++)
	{
		pMatchRoundItem=m_MatchRoundItemBuffer[i];
		SafeDelete(pMatchRoundItem);
	}

	//删除引用
	m_MatchRoundItemArray.RemoveAll();
	m_MatchRoundItemBuffer.RemoveAll();

	return;
}

//重置数据
bool CRoundInfoBuffer::ResetMatchRoundItem()
{
	//添加对象
	m_MatchRoundItemBuffer.Append(m_MatchRoundItemArray);

	//删除元素
	m_MatchRoundItemArray.RemoveAll();

	return true;
}

//删除数据
bool CRoundInfoBuffer::DeleteMatchRoundItem(WORD wRoundID)
{
	//变量定义
	tagMatchRoundItem * pMatchRoundItem=NULL;

	//查找对象
	for (INT_PTR i=0;i<m_MatchRoundItemArray.GetCount();i++)
	{
		pMatchRoundItem=m_MatchRoundItemArray[i];
		if (pMatchRoundItem->wRoundID==wRoundID)
		{
			//删除数据
			m_MatchRoundItemArray.RemoveAt(i);
			m_MatchRoundItemBuffer.Add(pMatchRoundItem);

			//设置变量
			ZeroMemory(pMatchRoundItem,sizeof(tagMatchRoundItem));

			break;
		}
	}	

	return true;
}

//插入数据
tagMatchRoundItem * CRoundInfoBuffer::InsertMatchRoundItem(tagMatchRoundItem & MatchRoundItem)
{
	//查找现存
	tagMatchRoundItem * pMatchRoundInsert=SearchMatchRound(MatchRoundItem.wRoundID);

	//创建判断
	if (pMatchRoundInsert==NULL)
	{
		//创建对象
		pMatchRoundInsert=CreateMatchRoundItem();

		//结果判断
		if (pMatchRoundInsert==NULL)
		{
			ASSERT(FALSE);
			return NULL;
		}

		//添加数据
		m_MatchRoundItemArray.Add(pMatchRoundInsert);		
	}

	//拷贝数据
	CopyMemory(pMatchRoundInsert,&MatchRoundItem,sizeof(tagMatchRoundItem));

	return pMatchRoundInsert;
}

//获取数目
WORD CRoundInfoBuffer::GetMatchRoundCount()
{
	return (WORD)(m_MatchRoundItemArray.GetCount());
}
	
//查找数据
tagMatchRoundItem * CRoundInfoBuffer::SearchMatchRound(WORD wRoundID)
{
	//变量定义
	tagMatchRoundItem * pMatchRoundItem=NULL;

	//查找对象
	for (INT_PTR i=0;i<m_MatchRoundItemArray.GetCount();i++)
	{
		pMatchRoundItem=m_MatchRoundItemArray[i];
		if (pMatchRoundItem->wRoundID==wRoundID)
		{
			return pMatchRoundItem;
		}
	}

	return NULL;
}

//创建对象
tagMatchRoundItem * CRoundInfoBuffer::CreateMatchRoundItem()
{
	//变量定义
	tagMatchRoundItem * pMatchRoundItem=NULL;

	//创建对象
	try
	{
		INT_PTR nArrayCount=m_MatchRoundItemBuffer.GetCount();
		if (nArrayCount>0)
		{
			pMatchRoundItem=m_MatchRoundItemBuffer[nArrayCount-1];
			m_MatchRoundItemBuffer.RemoveAt(nArrayCount-1);
		}
		else
		{
			pMatchRoundItem=new tagMatchRoundItem;
			if (pMatchRoundItem==NULL) return NULL;
		}
	}
	catch (...) 
	{ 
		return NULL; 
	}

	//设置变量
	ZeroMemory(pMatchRoundItem,sizeof(tagMatchRoundItem));

	return pMatchRoundItem;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CMatchInfoManager::CMatchInfoManager()
{
}

//析构函数
CMatchInfoManager::~CMatchInfoManager()
{
}

//加载比赛
bool CMatchInfoManager::LoadMatchOption(WORD wKindID,TCHAR szMachineID[LEN_MACHINE_ID],CMatchInfoBuffer & MatchInfoBuffer)
{
	//变量定义
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());

		//插入参数
		GameMatchDBAide.ResetParameter();
		GameMatchDBAide.AddParameter(TEXT("@wKindID"),wKindID);
		GameMatchDBAide.AddParameter(TEXT("@strServiceMachine"),szMachineID);

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		GameMatchDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//读取列表
		if (GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_LoadMatchOption"),true)!=DB_SUCCESS)
		{
			//获取信息
			GameMatchDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//清空列表
		MatchInfoBuffer.ResetMatchOption();

		//读取列表
		while (GameMatchDBModule->IsRecordsetEnd()==false)
		{
			//读取数据
			tagMatchOption GameMatchResult;
			ReadMatchOption(GameMatchDBAide,GameMatchResult);

			//插入列表
			MatchInfoBuffer.InsertMatchOption(&GameMatchResult);

			//移动记录
			GameMatchDBModule->MoveToNext();
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
bool CMatchInfoManager::DeleteMatchOption(DWORD dwMatchID)
{
	//变量定义
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());

		//插入参数
		GameMatchDBAide.ResetParameter();		
		GameMatchDBAide.AddParameter(TEXT("@dwMatchID"),dwMatchID);

		//读取列表
		LONG lResultCode=GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_DeleteMatchOption"),false);
		
		return lResultCode==DB_SUCCESS;
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

//插入房间
bool CMatchInfoManager::InsertMatchOption(tagCreateMatchInfo * pCreateMatchInfo,tagMatchOption & GameMatchResult)
{
	//变量定义
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());		

		//插入参数
		GameMatchDBAide.ResetParameter();		
 		GameMatchDBAide.AddParameter(TEXT("@wKindID"),pCreateMatchInfo->wKindID);		
		GameMatchDBAide.AddParameter(TEXT("@dwClubID"),pCreateMatchInfo->dwClubID);		
		GameMatchDBAide.AddParameter(TEXT("@strModuleName"),pCreateMatchInfo->szModuleName);
		GameMatchDBAide.AddParameter(TEXT("@strMatchName"),pCreateMatchInfo->szMatchName);
		GameMatchDBAide.AddParameter(TEXT("@strMatchDate"),pCreateMatchInfo->szMatchDate);
		GameMatchDBAide.AddParameter(TEXT("@cbShowType"), pCreateMatchInfo->cbShowType);
		GameMatchDBAide.AddParameter(TEXT("@cbMatchType"),pCreateMatchInfo->cbMatchType);

		//报名参数
		GameMatchDBAide.AddParameter(TEXT("@wFeeMoneyID"),pCreateMatchInfo->wFeeMoneyID);			
		GameMatchDBAide.AddParameter(TEXT("@lFeeAmount"),pCreateMatchInfo->lFeeAmount);
		GameMatchDBAide.AddParameter(TEXT("@wUseTicketCount"), pCreateMatchInfo->wUseTicketCount);
		GameMatchDBAide.AddParameter(TEXT("@wBuyTicketCount"), pCreateMatchInfo->wBuyTicketCount);
		GameMatchDBAide.AddParameter(TEXT("@lBuyTicketOption"), pCreateMatchInfo->wBuyTicketOption);
		GameMatchDBAide.AddParameter(TEXT("@lBuyTicketAmount"), pCreateMatchInfo->lBuyTicketAmount);
		GameMatchDBAide.AddParameter(TEXT("@wBuyTicketMoneyID"), pCreateMatchInfo->wBuyTicketMoneyID);
		GameMatchDBAide.AddParameter(TEXT("@wSignupCount"), pCreateMatchInfo->wSignupCount);
		GameMatchDBAide.AddParameter(TEXT("@cbSignupMode"),pCreateMatchInfo->cbSignupMode);		
		GameMatchDBAide.AddParameter(TEXT("@cbJoinCondition"),pCreateMatchInfo->cbJoinCondition);
		GameMatchDBAide.AddParameter(TEXT("@cbMemberOrder"),pCreateMatchInfo->cbMemberOrder);
		GameMatchDBAide.AddParameter(TEXT("@lExperience"),pCreateMatchInfo->lExperience);	

		//比赛配桌
		GameMatchDBAide.AddParameter(TEXT("@cbMatchDistribute"), pCreateMatchInfo->cbMatchDistribute, sizeof(pCreateMatchInfo->cbMatchDistribute));

		//比赛奖励
		GameMatchDBAide.AddParameter(TEXT("@wRewardCount"),pCreateMatchInfo->wRewardCount);
		GameMatchDBAide.AddParameter(TEXT("@cbMatchReward"),(LPBYTE)pCreateMatchInfo->MatchRewardItem,pCreateMatchInfo->wRewardCount*sizeof(pCreateMatchInfo->MatchRewardItem[0]));

		//比赛轮次
		GameMatchDBAide.AddParameter(TEXT("@wRoundCount"),pCreateMatchInfo->wRoundCount);
		GameMatchDBAide.AddParameter(TEXT("@cbMatchRound"),(LPBYTE)pCreateMatchInfo->MatchRoundItem,__max(pCreateMatchInfo->wRoundCount*sizeof(pCreateMatchInfo->MatchRoundItem[0]),1));

		//扩展配置
		GameMatchDBAide.AddParameter(TEXT("@cbMatchRule"),pCreateMatchInfo->cbMatchRule,sizeof(pCreateMatchInfo->cbMatchRule));
		GameMatchDBAide.AddParameter(TEXT("@strServiceMachine"),pCreateMatchInfo->szServiceMachine);	

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		GameMatchDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		if (GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_InsertMatchOption"),true)!=DB_SUCCESS)
		{
			//获取信息
			GameMatchDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取配置
		ReadMatchOption(GameMatchDBAide,GameMatchResult);

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

//修改房间
bool CMatchInfoManager::ModifyMatchOption(tagCreateMatchInfo * pCreateMatchInfo,tagMatchOption & GameMatchResult)
{
	//变量定义
	CDataBaseAide GameMatchDBAide;
	CDataBaseHelper GameMatchDBModule;

	//执行逻辑
	try
	{
		//连接数据库
		if (ConnectGameMatchDB(GameMatchDBModule)==false)
		{
			ASSERT(FALSE);
			return false;
		}

		//设置对象
		GameMatchDBAide.SetDataBase(GameMatchDBModule.GetInterface());

		//基础参数
		GameMatchDBAide.ResetParameter();
		GameMatchDBAide.AddParameter(TEXT("@wKindID"),pCreateMatchInfo->wKindID);	
		GameMatchDBAide.AddParameter(TEXT("@dwMatchID"),pCreateMatchInfo->dwMatchID);	
		GameMatchDBAide.AddParameter(TEXT("@dwClubID"),pCreateMatchInfo->dwClubID);
		GameMatchDBAide.AddParameter(TEXT("@strModuleName"),pCreateMatchInfo->szModuleName);
		GameMatchDBAide.AddParameter(TEXT("@strMatchName"),pCreateMatchInfo->szMatchName);
		GameMatchDBAide.AddParameter(TEXT("@strMatchDate"),pCreateMatchInfo->szMatchDate);
		GameMatchDBAide.AddParameter(TEXT("@cbShowType"), pCreateMatchInfo->cbShowType);
		GameMatchDBAide.AddParameter(TEXT("@cbMatchType"),pCreateMatchInfo->cbMatchType);

		//报名参数
		GameMatchDBAide.AddParameter(TEXT("@wFeeMoneyID"), pCreateMatchInfo->wFeeMoneyID);
		GameMatchDBAide.AddParameter(TEXT("@lFeeAmount"), pCreateMatchInfo->lFeeAmount);
		GameMatchDBAide.AddParameter(TEXT("@wUseTicketCount"), pCreateMatchInfo->wUseTicketCount);
		GameMatchDBAide.AddParameter(TEXT("@wBuyTicketCount"), pCreateMatchInfo->wBuyTicketCount);
		GameMatchDBAide.AddParameter(TEXT("@wBuyTicketOption"), pCreateMatchInfo->wBuyTicketOption);
		GameMatchDBAide.AddParameter(TEXT("@lBuyTicketAmount"), pCreateMatchInfo->lBuyTicketAmount);
		GameMatchDBAide.AddParameter(TEXT("@wBuyTicketMoneyID"), pCreateMatchInfo->wBuyTicketMoneyID);
		GameMatchDBAide.AddParameter(TEXT("@wSignupCount"), pCreateMatchInfo->wSignupCount);
		GameMatchDBAide.AddParameter(TEXT("@cbSignupMode"),pCreateMatchInfo->cbSignupMode);		
		GameMatchDBAide.AddParameter(TEXT("@cbJoinCondition"),pCreateMatchInfo->cbJoinCondition);
		GameMatchDBAide.AddParameter(TEXT("@cbMemberOrder"),pCreateMatchInfo->cbMemberOrder);
		GameMatchDBAide.AddParameter(TEXT("@lExperience"),pCreateMatchInfo->lExperience);

		//比赛配桌
		GameMatchDBAide.AddParameter(TEXT("@cbMatchDistribute"), pCreateMatchInfo->cbMatchDistribute, sizeof(pCreateMatchInfo->cbMatchDistribute));

		//比赛奖励
		GameMatchDBAide.AddParameter(TEXT("@wRewardCount"),pCreateMatchInfo->wRewardCount);
		GameMatchDBAide.AddParameter(TEXT("@cbMatchReward"),(LPBYTE)&pCreateMatchInfo->MatchRewardItem,pCreateMatchInfo->wRewardCount*sizeof(pCreateMatchInfo->MatchRewardItem[0]));	

		//比赛轮次
		GameMatchDBAide.AddParameter(TEXT("@wRoundCount"),pCreateMatchInfo->wRoundCount);
		GameMatchDBAide.AddParameter(TEXT("@cbMatchRound"),(LPBYTE)&pCreateMatchInfo->MatchRoundItem, __max(pCreateMatchInfo->wRoundCount*sizeof(pCreateMatchInfo->MatchRoundItem[0]),1));

		//比赛规则
		GameMatchDBAide.AddParameter(TEXT("@cbMatchRule"),pCreateMatchInfo->cbMatchRule,sizeof(pCreateMatchInfo->cbMatchRule));		

		//输出信息
		TCHAR szDescribeString[128]=TEXT("");
		GameMatchDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szDescribeString,sizeof(szDescribeString),adParamOutput);

		//执行脚本
		if (GameMatchDBAide.ExecuteProcess(TEXT("GSP_GS_ModifyMatchOption"),true)!=DB_SUCCESS)
		{
			//获取信息
			GameMatchDBAide.GetParameter(TEXT("@strErrorDescribe"),szDescribeString,CountArray(szDescribeString));

			//错误提示
			CTraceService::TraceString(szDescribeString,TraceLevel_Exception);

			//错误提示
			AfxMessageBox(szDescribeString,MB_ICONERROR);

			return false;
		}

		//读取配置
		ReadMatchOption(GameMatchDBAide,GameMatchResult);

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
bool CMatchInfoManager::ConnectPlatformDB(CDataBaseHelper & PlatformDBModule)
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

//连接数据
bool CMatchInfoManager::ConnectGameMatchDB(CDataBaseHelper & GameMatchDBModule)
{
	//获取参数
	CModuleDBParameter * pModuleDBParameter=CModuleDBParameter::GetModuleDBParameter();
	tagDataBaseParameter * pDataBaseParameter=pModuleDBParameter->GetMatchDBParameter();

	//创建对象
	if ((GameMatchDBModule.GetInterface()==NULL)&&(GameMatchDBModule.CreateInstance()==false))
	{
		AfxMessageBox(TEXT("ConnectGameMatchDB 创建 GameMatchDBModule 对象失败"),MB_ICONERROR);
		return false;
	}

	//设置连接
	GameMatchDBModule->SetConnectionInfo(pDataBaseParameter->dwDataBaseAddr,pDataBaseParameter->wDataBasePort,
	CW2CT(pDataBaseParameter->szDataBaseName),CW2CT(pDataBaseParameter->szDataBaseUser),CW2CT(pDataBaseParameter->szDataBasePass));

	//发起连接
	GameMatchDBModule->OpenConnection();

	return true;
}

//读取比赛
bool CMatchInfoManager::ReadMatchOption(CDataBaseAide & GameMatchDBAide, tagMatchOption & GameMatchResult)
{
	//设置变量
	ZeroMemory(&GameMatchResult,sizeof(GameMatchResult));

	//索引变量	
	GameMatchResult.wKindID=GameMatchDBAide.GetValue_WORD(TEXT("KindID"));	
	GameMatchResult.dwClubID=GameMatchDBAide.GetValue_WORD(TEXT("ClubID"));	
	GameMatchResult.dwMatchID=GameMatchDBAide.GetValue_DWORD(TEXT("MatchID"));	
	GameMatchResult.cbMatchStatus=GameMatchDBAide.GetValue_BYTE(TEXT("MatchStatus"));

	//比赛信息
	GameMatchResult.cbShowType = GameMatchDBAide.GetValue_BYTE(TEXT("ShowType"));
	GameMatchResult.cbMatchType = GameMatchDBAide.GetValue_BYTE(TEXT("MatchType"));		
	GameMatchDBAide.GetValue_String(TEXT("ModuleName"),GameMatchResult.szModuleName,CountArray(GameMatchResult.szModuleName));
	GameMatchDBAide.GetValue_String(TEXT("MatchName"),GameMatchResult.szMatchName,CountArray(GameMatchResult.szMatchName));

	//报名信息
	GameMatchResult.wFeeMoneyID= GameMatchDBAide.GetValue_WORD(TEXT("FeeMoneyID"));	
	GameMatchResult.lFeeAmount= GameMatchDBAide.GetValue_LONGLONG(TEXT("FeeAmount"));
	GameMatchResult.wUseTicketCount = GameMatchDBAide.GetValue_WORD(TEXT("UseTicketCount"));
	GameMatchResult.wBuyTicketCount = GameMatchDBAide.GetValue_WORD(TEXT("BuyTicketCount"));
	GameMatchResult.wBuyTicketOption= GameMatchDBAide.GetValue_WORD(TEXT("BuyTicketOption"));
	GameMatchResult.wBuyTicketMoneyID = GameMatchDBAide.GetValue_WORD(TEXT("BuyTicketMoneyID"));
	GameMatchResult.lBuyTicketAmount = GameMatchDBAide.GetValue_LONGLONG(TEXT("BuyTicketAmount"));	
	GameMatchResult.wSignupCount = GameMatchDBAide.GetValue_WORD(TEXT("SignupCount"));
	GameMatchResult.cbSignupMode = GameMatchDBAide.GetValue_BYTE(TEXT("SignupMode"));	
	GameMatchResult.cbJoinCondition = GameMatchDBAide.GetValue_BYTE(TEXT("JoinCondition"));
	GameMatchResult.cbMemberOrder = GameMatchDBAide.GetValue_BYTE(TEXT("MemberOrder"));
	GameMatchResult.lExperience = GameMatchDBAide.GetValue_LONG(TEXT("Experience"));
	
	//比赛奖励	
	GameMatchResult.wRewardCount=GameMatchDBAide.GetValue_WORD(TEXT("RewardCount"));
	GameMatchDBAide.GetValue_Binary(TEXT("MatchReward"),(LPBYTE)&GameMatchResult.MatchRewardItem,sizeof(GameMatchResult.MatchRewardItem));	

	//比赛轮次
	GameMatchResult.wRoundCount=GameMatchDBAide.GetValue_WORD(TEXT("RoundCount"));
	GameMatchDBAide.GetValue_Binary(TEXT("MatchRound"),(LPBYTE)&GameMatchResult.MatchRoundItem,sizeof(GameMatchResult.MatchRoundItem));

	//比赛规则	
	GameMatchDBAide.GetValue_Binary(TEXT("MatchRule"),GameMatchResult.cbMatchRule,sizeof(GameMatchResult.cbMatchRule));

	//比赛配桌
	GameMatchDBAide.GetValue_Binary(TEXT("MatchDistribute"), GameMatchResult.cbMatchDistribute, sizeof(GameMatchResult.cbMatchDistribute));
	
	return true;
}

//////////////////////////////////////////////////////////////////////////
