#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBaseEngineSink.h"
#include <afxinet.h>

//////////////////////////////////////////////////////////////////////////

//构造函数
CDataBaseEngineSink::CDataBaseEngineSink()
{
	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

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
	if ((m_GameBattleDBModule.GetInterface()==NULL)&&(m_GameBattleDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}		

	try
	{
		//变量定义		
		tagDataBaseParameter * pIMClubDBParameter=&m_pInitParameter->m_GameBattleDBParameter;

		//设置连接
		m_GameBattleDBModule->SetConnectionInfo(pIMClubDBParameter->dwDataBaseAddr,pIMClubDBParameter->wDataBasePort,
		CW2CT(pIMClubDBParameter->szDataBaseName),CW2CT(pIMClubDBParameter->szDataBaseUser),CW2CT(pIMClubDBParameter->szDataBasePass));

		//发起连接
		m_GameBattleDBModule->OpenConnection();
		m_GameBattleDBAide.SetDataBase(m_GameBattleDBModule.GetInterface());		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//错误信息
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//停止事件
bool CDataBaseEngineSink::OnDataBaseEngineStop(IUnknownEx * pIUnknownEx)
{
	//设置对象
	m_GameBattleDBAide.SetDataBase(NULL);

	//关闭连接
	if (m_GameBattleDBModule.GetInterface()!=NULL)
	{
		m_GameBattleDBModule->CloseConnection();
		m_GameBattleDBModule.CloseInstance();
	}

	//组件变量
	m_pInitParameter=NULL;
	m_pIDataBaseEngineEvent=NULL;

	return true;
}

//时间事件
bool CDataBaseEngineSink::OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	return false;
}

//控制事件
bool CDataBaseEngineSink::OnDataBaseEngineControl(WORD wControlID, VOID * pData, WORD wDataSize)
{
	return false;
}

//请求事件
bool CDataBaseEngineSink::OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	switch (wRequestID)
	{	
	case DBR_GR_QUERY_MEMBER:		//查询成员
	{
		return OnRequestQueryMember(dwContextID,dwTokenID,pData,wDataSize);
	}
	case DBR_GR_UPDATE_MEMBER:		//更新成员
	{
		return OnRequestUpdateMember(dwContextID,dwTokenID,pData,wDataSize);
	}
	case DBR_GR_SET_WINNER_SCORE:	//设置赢分
	{
		return OnRequestSetWinnerScore(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_LOAD_CLUB_LIST:		//加载列表
	{
		return OnRequestLoadClubList(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_LOAD_CLUB_ITEM:		//加载子项
	{
		return OnRequestLoadClubItem(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_LOAD_CLUB_MEMBER:		//茶社成员
	{
		return OnRequestLoadClubMember(dwContextID, dwTokenID, pData,wDataSize);
	}	
	case DBR_GR_LOAD_BATTLE_CONFIG:		//约战玩法
	{
		return OnRequestLoadBattleConfig(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_LOAD_APPLY_MESSAGE:		//申请消息
	{
		return OnRequestLoadApplyMessage(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_LOAD_KIND_SETTING:		//类型设置
	{
		return OnRequestLoadKindSetting(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_APPEND_CONFIG:			//添加玩法
	{
		return OnRequestAppendConfig(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_MODIFY_CONFIG:			//修改玩法
	{
		return OnRequestModifyConfig(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_DELETE_CONFIG:			//删除玩法
	{
		return OnRequestDeleteConfig(dwContextID, dwTokenID, pData, wDataSize);
	}
	}

	return false;
}

//茶社成员
bool CDataBaseEngineSink::OnRequestLoadClubMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	try
	{
		//提取数据
		DBR_GR_LoadClubMember * pLoadClubMember=(DBR_GR_LoadClubMember *)pData;

		//构造参数
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pLoadClubMember->dwClubID);
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_QueryClubMember"),true);

		//加载成功
		if (lReturnValue==DB_SUCCESS)
		{
			//变量定义			
			BYTE cbBuffer[MAX_ASYNC_DATA];
			ZeroMemory(cbBuffer,sizeof(cbBuffer));

			//变量定义
			DBO_GR_ClubMember * pGRClubMember=(DBO_GR_ClubMember *)cbBuffer;
			pGRClubMember->dwClubID=pLoadClubMember->dwClubID;

			//变量定义
			tagClubUserInfo * pUserInfo=NULL;
			tagClubMemberInfo * pMemberInfo=NULL;			
			tagClubMemberUser * pClubMemberUser=NULL;
			WORD wPacketSize=sizeof(DBO_GR_ClubMember);			

			//茶社成员
			while (m_GameBattleDBModule->IsRecordsetEnd()==false)
			{
				//发送信息
				if ((wPacketSize+sizeof(tagClubMemberUser))>sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_MEMBER,dwContextID,dwTokenID,cbBuffer,wPacketSize);
					wPacketSize=sizeof(DBO_GR_ClubMember);	
				}

				//读取信息
				pClubMemberUser=(tagClubMemberUser *)(cbBuffer+wPacketSize);

				//设置变量
				pUserInfo=&pClubMemberUser->UserInfo;
				pMemberInfo=&pClubMemberUser->MemberInfo;

				//成员信息
				pMemberInfo->dwMemberID=m_GameBattleDBAide.GetValue_DWORD(TEXT("UserID"));				
				pMemberInfo->cbMemberType=m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberType"));
				pMemberInfo->cbMemberStatus=m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberStatus"));
				pMemberInfo->dwManageRight = m_GameBattleDBAide.GetValue_DWORD(TEXT("ManageRight"));
				pMemberInfo->lClubScore=m_GameBattleDBAide.GetValue_LONGLONG(TEXT("ClubScore"));
				pMemberInfo->dwCreateCount=m_GameBattleDBAide.GetValue_DWORD(TEXT("CreateCount"));				
				pMemberInfo->dwBattleCount=m_GameBattleDBAide.GetValue_DWORD(TEXT("BattleCount"));	
				pMemberInfo->lRevenueCount = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("RevenueCount"));
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("JoinDateTime"),pMemberInfo->JoinDateTime);
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("LastBattleTime"), pMemberInfo->LastBattleTime);				

				//用户信息
				pUserInfo->cbUserStatus=US_IM_OFFLINE;
				pUserInfo->dwUserID=m_GameBattleDBAide.GetValue_DWORD(TEXT("UserID"));				
				pUserInfo->dwGameID=m_GameBattleDBAide.GetValue_DWORD(TEXT("GameID"));	
				pUserInfo->wFaceID=m_GameBattleDBAide.GetValue_WORD(TEXT("FaceID"));
				pUserInfo->cbGender=m_GameBattleDBAide.GetValue_BYTE(TEXT("Gender"));					
				pUserInfo->dwCustomID=m_GameBattleDBAide.GetValue_DWORD(TEXT("CustomID"));
				m_GameBattleDBAide.GetValue_String(TEXT("NickName"),pUserInfo->szNickName,CountArray(pUserInfo->szNickName));
				m_GameBattleDBAide.GetValue_String(TEXT("UserFacePath"), pUserInfo->szUserHeadUrl, CountArray(pUserInfo->szUserHeadUrl));//kk jia

				//设置位移
				wPacketSize+=sizeof(tagClubMemberUser);

				//移动记录
				m_GameBattleDBModule->MoveToNext();
			}

			//发送剩余
			if (wPacketSize>0) 
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_MEMBER,dwContextID,dwTokenID,cbBuffer,wPacketSize);
			}							
		}		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//约战玩法
bool CDataBaseEngineSink::OnRequestLoadBattleConfig(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	try
	{
		//提取数据
		DBR_GR_LoadBattleConfig * pLoadBattleConfig=(DBR_GR_LoadBattleConfig *)pData;

		//构造参数
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pLoadBattleConfig->dwClubID);
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadBattleConfig"),true);

		//加载成功
		if (lReturnValue==DB_SUCCESS)
		{
			//变量定义			
			BYTE cbBuffer[MAX_ASYNC_DATA];
			ZeroMemory(cbBuffer,sizeof(cbBuffer));

			//变量定义
			DBO_GR_BattleConfig * pDBOBattleConfig=(DBO_GR_BattleConfig *)cbBuffer;

			//设置变量
			pDBOBattleConfig->dwClubID=pLoadBattleConfig->dwClubID;						

			//变量定义			
			WORD wPacketSize=sizeof(DBO_GR_BattleConfig);		
			tagClubBattleConfig * pBattleConfig=(tagClubBattleConfig *)(pDBOBattleConfig+1);			

			//茶社成员
			while (m_GameBattleDBModule->IsRecordsetEnd()==false)
			{
				//发送信息
				if ((wPacketSize+sizeof(tagClubBattleConfig))>sizeof(cbBuffer))
				{
					//投递数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_CONFIG,dwContextID,dwTokenID,cbBuffer,wPacketSize);

					//设置变量
					wPacketSize=sizeof(DBO_GR_BattleConfig);
					pBattleConfig=(tagClubBattleConfig *)(pDBOBattleConfig+1);
				}

				//配置信息
				pBattleConfig->wKindID=m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
				pBattleConfig->dwConfigID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ConfigID"));
				m_GameBattleDBAide.GetValue_String(TEXT("BattleConfig"),pBattleConfig->szBattleConfig,CountArray(pBattleConfig->szBattleConfig));

				//设置位移
				pBattleConfig++;
				wPacketSize+=sizeof(tagClubBattleConfig);

				//移动记录
				m_GameBattleDBModule->MoveToNext();
			}

			//发送剩余
			if (wPacketSize>0) 
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_BATTLE_CONFIG,dwContextID,dwTokenID,cbBuffer,wPacketSize);
			}							
		}		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//游戏设置
bool CDataBaseEngineSink::OnRequestLoadKindSetting(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	try
	{
		//提取数据
		auto pLoadKindSetting = (DBR_GR_LoadKindSetting*)pData;

		//构造参数
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pLoadKindSetting->dwClubID);
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadKindSetting"), true);

		//加载成功
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义			
			BYTE cbBuffer[SOCKET_BUFFER];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//变量定义
			auto wPacketSize = sizeof(DBO_GR_ClubKindSetting);
			auto pDBOClubKindSetting = (DBO_GR_ClubKindSetting *)cbBuffer;

			//茶社标识
			pDBOClubKindSetting->dwClubID = pLoadKindSetting->dwClubID;

			//茶社成员
			while (m_GameBattleDBModule->IsRecordsetEnd() == false)
			{
				//发送信息
				if ((wPacketSize + sizeof(tagClubKindSetting)) > sizeof(cbBuffer))
				{
					//投递数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_KIND_SETTING, dwContextID, dwTokenID, cbBuffer, wPacketSize);

					//设置变量
					pDBOClubKindSetting->wItemCount = 0;
					wPacketSize = sizeof(DBO_GR_ClubKindSetting);					
				}

				auto pKindSettingItem = &pDBOClubKindSetting->KindSettingItem[pDBOClubKindSetting->wItemCount++];

				//配置信息
				pKindSettingItem->wKindID = m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
				pKindSettingItem->lMinWinnerScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("MinWinnerScore"));
				
				//设置位移				
				wPacketSize += sizeof(tagClubBattleConfig);

				//移动记录
				m_GameBattleDBModule->MoveToNext();
			}

			//发送剩余
			if (wPacketSize > 0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_KIND_SETTING, dwContextID, dwTokenID, cbBuffer, wPacketSize);
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

//申请消息
bool CDataBaseEngineSink::OnRequestLoadApplyMessage(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	try
	{
		//提取数据
		DBR_GR_LoadApplyMessage * pLoadApplyMessage = (DBR_GR_LoadApplyMessage *)pData;

		//构造参数
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), 0);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pLoadApplyMessage->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbMessageType"), APPLY_MSG_TYPE_RESPOND);
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadApplyMessage"), true);

		//加载成功
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义			
			BYTE cbBuffer[SOCKET_BUFFER];
			ZeroMemory(cbBuffer, sizeof(cbBuffer));

			//变量定义
			DBO_GR_ApplyMessage * pDBOApplyMessage = (DBO_GR_ApplyMessage *)cbBuffer;

			//设置变量
			pDBOApplyMessage->dwClubID = pLoadApplyMessage->dwClubID;

			//变量定义			
			WORD wPacketSize = sizeof(DBO_GR_ApplyMessage);
			tagClubApplyMessage * pApplyMessage = (tagClubApplyMessage *)(pDBOApplyMessage + 1);

			//茶社成员
			while (m_GameBattleDBModule->IsRecordsetEnd() == false)
			{
				//发送信息
				if ((wPacketSize + sizeof(tagClubApplyMessage)) > sizeof(cbBuffer))
				{
					//投递数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE, dwContextID, dwTokenID, cbBuffer, wPacketSize);

					//设置变量
					wPacketSize=sizeof(DBO_GR_ApplyMessage);
					pApplyMessage=(tagClubApplyMessage *)(pDBOApplyMessage+1);
				}

				//读取数据
				pApplyMessage->dwMessageID = m_GameBattleDBAide.GetValue_DWORD(TEXT("RecordID"));
				pApplyMessage->cbMessageStatus = m_GameBattleDBAide.GetValue_BYTE(TEXT("ApplyStatus"));

				//申请消息
				pApplyMessage->dwApplyerID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerID"));
				pApplyMessage->dwApplyerGameID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
				pApplyMessage->wApplyerFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("ApplyerFaceID"));
				pApplyMessage->cbApplyerGender = m_GameBattleDBAide.GetValue_BYTE(TEXT("ApplyerGender"));
				pApplyMessage->dwApplyerCustomID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("ApplyDateTime"), pApplyMessage->ApplyDateTime);
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("OperateDateTime"), pApplyMessage->OperateDateTime);
				m_GameBattleDBAide.GetValue_String(TEXT("ApplyerNickName"), pApplyMessage->szApplyerNickName, CountArray(pApplyMessage->szApplyerNickName));

				//茶社信息
				pApplyMessage->dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
				pApplyMessage->dwResponderID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ResponderID"));
				m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), pApplyMessage->szClubName, CountArray(pApplyMessage->szClubName));

				//设置位移
				pApplyMessage++;
				wPacketSize+=sizeof(tagClubApplyMessage);

				//移动记录
				m_GameBattleDBModule->MoveToNext();
			}

			//投递数据
			if (wPacketSize > sizeof(DBO_GR_ApplyMessage))
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE, dwContextID, dwTokenID, cbBuffer, wPacketSize);
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}


//添加玩法
bool CDataBaseEngineSink::OnRequestAppendConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize == sizeof(DBR_GR_AppendConfig));
	if (wDataSize != sizeof(DBR_GR_AppendConfig)) return false;

	//提取数据
	auto pAppendConfig = (DBR_GR_AppendConfig*)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");
		tagClubBattleConfig* pBattleConfig = &pAppendConfig->BattleConfig;

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pAppendConfig->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@wKindID"), pBattleConfig->wKindID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pAppendConfig->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@strBattleConfig"), pBattleConfig->szBattleConfig);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_AppendBattleConfig"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_AppendConfig AppendConfig;
			ZeroMemory(&AppendConfig, sizeof(AppendConfig));

			//读取信息
			AppendConfig.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
			AppendConfig.BattleConfig.wKindID = m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
			AppendConfig.BattleConfig.dwConfigID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ConfigID"));
			m_GameBattleDBAide.GetValue_String(TEXT("BattleConfig"), AppendConfig.BattleConfig.szBattleConfig, CountArray(AppendConfig.BattleConfig.szBattleConfig));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPEND_CONFIG, dwContextID, dwTokenID, &AppendConfig, sizeof(AppendConfig));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorMessage);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), TEXT("由于数据库操作异常，请稍后重试！"));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorMessage);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//修改玩法
bool CDataBaseEngineSink::OnRequestModifyConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize == sizeof(DBR_GR_ModifyConfig));
	if (wDataSize != sizeof(DBR_GR_ModifyConfig)) return false;

	//提取数据
	auto pModifyConfig = (DBR_GR_ModifyConfig*)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");
		tagClubBattleConfig* pBattleConfig = &pModifyConfig->BattleConfig;

		LONG lReturnValue = 0;

		//修改玩法
		if (pModifyConfig->cbModifyKind == CLUB_BC_MK_BATTLE_CONFIG)
		{
			//构造参数
			m_GameBattleDBAide.ResetParameter();

			//用户信息
			m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pModifyConfig->dwUserID);
			m_GameBattleDBAide.AddParameter(TEXT("@wKindID"), pBattleConfig->wKindID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pModifyConfig->dwClubID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwConfigID"), pBattleConfig->dwConfigID);
			m_GameBattleDBAide.AddParameter(TEXT("@strBattleConfig"), pBattleConfig->szBattleConfig);

			//输出参数
			m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

			//执行脚本
			lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_ModifyBattleConfig"), true);
		}		

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_ModifyConfig ModifyConfig;
			ZeroMemory(&ModifyConfig, sizeof(ModifyConfig));

			//修改类型
			ModifyConfig.cbModifyKind = pModifyConfig->cbModifyKind;

			//读取信息
			ModifyConfig.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
			ModifyConfig.BattleConfig.wKindID = m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
			ModifyConfig.BattleConfig.dwConfigID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ConfigID"));
			m_GameBattleDBAide.GetValue_String(TEXT("BattleConfig"), ModifyConfig.BattleConfig.szBattleConfig, CountArray(ModifyConfig.BattleConfig.szBattleConfig));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_CONFIG, dwContextID, dwTokenID, &ModifyConfig, sizeof(ModifyConfig));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorMessage);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), TEXT("由于数据库操作异常，请稍后重试！"));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorMessage);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//删除玩法
bool CDataBaseEngineSink::OnRequestDeleteConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize == sizeof(DBR_GR_DeleteConfig));
	if (wDataSize != sizeof(DBR_GR_DeleteConfig)) return false;

	//提取数据
	auto pDeleteConfig = (DBR_GR_DeleteConfig*)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pDeleteConfig->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@wKindID"), pDeleteConfig->wKindID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pDeleteConfig->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwConfigID"), pDeleteConfig->dwConfigID);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_DeleteBattleConfig"), false);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_DeleteConfig DeleteConfig;
			ZeroMemory(&DeleteConfig, sizeof(DeleteConfig));

			//设置变量
			DeleteConfig.wKindID = pDeleteConfig->wKindID;
			DeleteConfig.dwClubID = pDeleteConfig->dwClubID;
			DeleteConfig.dwConfigID = pDeleteConfig->dwConfigID;

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DELETE_CONFIG, dwContextID, dwTokenID, &DeleteConfig, sizeof(DeleteConfig));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorMessage);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), TEXT("由于数据库操作异常，请稍后重试！"));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorMessage);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//查询成员
bool CDataBaseEngineSink::OnRequestQueryMember(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	try
	{
		//提取数据
		DBR_GR_QueryMember* pQueryMember = (DBR_GR_QueryMember*)pData;

		//构造标识列表
		CString strMemberIDList = TEXT("[");

		for (int i = 0; i < pQueryMember->wMemberCount; i++)
		{
			strMemberIDList.AppendFormat(TEXT("%d"), pQueryMember->dwMemberID[i]);
			if (i < pQueryMember->wMemberCount - 1)
			{
				strMemberIDList.Append(TEXT(","));
			}
		}

		//添加回括号		
		strMemberIDList.Append(TEXT("]"));

		//构造参数
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pQueryMember->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@strMemberID"), strMemberIDList);

		//查询成员
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_QueryMemberInfo"), true);

		//加载成功
		if (lReturnValue == DB_SUCCESS)
		{
			//读取财富
			while (m_GameBattleDBModule->IsRecordsetEnd() == false)
			{
				//变量定义
				DBO_GR_UpdateMember UpdateMember;
				ZeroMemory(&UpdateMember, sizeof(UpdateMember));

				//茶社属性				
				UpdateMember.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
				UpdateMember.ClubMember.dwMemberID = m_GameBattleDBAide.GetValue_DWORD(TEXT("MemberID"));
				UpdateMember.ClubMember.cbMemberType = m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberType"));
				UpdateMember.ClubMember.cbMemberStatus= m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberStatus"));
				UpdateMember.ClubMember.dwManageRight = m_GameBattleDBAide.GetValue_DWORD(TEXT("ManageRight"));
				UpdateMember.ClubMember.lClubScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("ClubScore"));
				UpdateMember.ClubMember.lRevenueCount = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("RevenueCount"));
				UpdateMember.ClubMember.dwCreateCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreateCount"));
				UpdateMember.ClubMember.dwBattleCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("BattleCount"));
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), UpdateMember.ClubMember.JoinDateTime);
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("LastBattleTime"), UpdateMember.ClubMember.LastBattleTime);

				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_MEMBER, dwContextID, dwTokenID, &UpdateMember, sizeof(UpdateMember));

				//移动记录
				m_GameBattleDBModule->MoveToNext();
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

//更新成员
bool CDataBaseEngineSink::OnRequestUpdateMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	try
	{
		//提取数据
		DBR_GR_UpdateMember * pUpdateMember=(DBR_GR_UpdateMember *)pData;

		//更新约战
		if (pUpdateMember->cbUpdateKind == UPMEMBER_KIND_BATTLE)
		{
			//构造参数
			m_GameBattleDBAide.ResetParameter();
			m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pUpdateMember->dwClubID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pUpdateMember->dwMemberID);
			m_GameBattleDBAide.AddParameter(TEXT("@wIncCreateCount"), pUpdateMember->wIncCreateCount);
			m_GameBattleDBAide.AddParameter(TEXT("@wIncBattleCount"), pUpdateMember->wIncBattleCount);
			LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_UpdateMemberBattleCount"), true);

			//加载成功
			if (lReturnValue == DB_SUCCESS && m_GameBattleDBModule->IsRecordsetEnd() == false)
			{
				//变量定义
				DBO_GR_UpdateMember UpdateMember;
				ZeroMemory(&UpdateMember, sizeof(UpdateMember));

				//茶社属性
				UpdateMember.cbUpdateKind = pUpdateMember->cbUpdateKind;
				UpdateMember.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
				UpdateMember.ClubMember.dwMemberID = m_GameBattleDBAide.GetValue_DWORD(TEXT("MemberID"));
				UpdateMember.ClubMember.dwCreateCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreateCount"));
				UpdateMember.ClubMember.dwBattleCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("BattleCount"));
				UpdateMember.ClubMember.lClubScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("ClubScore"));
				UpdateMember.ClubMember.lRevenueCount = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("RevenueCount"));
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), UpdateMember.ClubMember.JoinDateTime);
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("LastBattleTime"), UpdateMember.ClubMember.LastBattleTime);

				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_MEMBER, dwContextID, dwTokenID, &UpdateMember, sizeof(UpdateMember));
			}
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//设置赢分 
bool CDataBaseEngineSink::OnRequestSetWinnerScore(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	try
	{
		//提取数据
		auto  pSetWinnerScore = (DBR_GR_SetWinnerScore*)pData;

		//构造参数
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pSetWinnerScore->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pSetWinnerScore->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@wKindID"), pSetWinnerScore->wKindID);
		m_GameBattleDBAide.AddParameter(TEXT("@lMinWinnerScore"), pSetWinnerScore->lMinWinnerScore);

		//输出参数
		WCHAR szErrorDescribe[128] = TEXT("");
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_SetWinnerScore"), true);

		//加载成功
		if (lReturnValue == DB_SUCCESS && m_GameBattleDBModule->IsRecordsetEnd() == false)
		{
			//变量定义
			DBO_GR_KindSettingModify KindSettingModify;
			ZeroMemory(&KindSettingModify, sizeof(KindSettingModify));

			//设置修改
			KindSettingModify.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
			KindSettingModify.KindSettingItem.wKindID = m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
			KindSettingModify.KindSettingItem.lMinWinnerScore= m_GameBattleDBAide.GetValue_LONGLONG(TEXT("MinWinnerScore"));			

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_SETTING, dwContextID, dwTokenID, &KindSettingModify, sizeof(KindSettingModify));
		}	
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = lReturnValue;
			if (DBVarValue.bstrVal != NULL)
			{
				StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), (LPCTSTR)(DBVarValue.bstrVal));
			}

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorMessage);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorMessage, CountArray(OperateFailure.szErrorMessage), TEXT("由于数据库操作异常，请稍后重试！"));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorMessage);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorMessage);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);

		return false;
	}

	return true;
}

//茶社列表
bool CDataBaseEngineSink::OnRequestLoadClubList(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	try
	{
		//提取数据
		DBR_GR_LoadClubList * pLoadClubList=(DBR_GR_LoadClubList *)pData;

		//构造参数
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubStartID"),pLoadClubList->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubEndID"),pLoadClubList->dwClubID);
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_QueryClubList"),true);

		//加载成功
		if (lReturnValue==DB_SUCCESS)
		{
			//变量定义
			WORD wPacketSize = 0;
			BYTE cbBuffer[MAX_ASYNC_DATA] = { 0 };

			//变量定义
			tagClubItem * pClubInfoItem=NULL;
			tagClubRules * pClubRules = NULL;
			tagClubNotice * pClubNotice = NULL;
			tagClubProperty * pClubProperty=NULL;			

			//开桌配置
			while (m_GameBattleDBModule->IsRecordsetEnd()==false)
			{
				//发送信息
				if ((wPacketSize+sizeof(tagClubItem))>sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_LIST,dwContextID,dwTokenID,cbBuffer,wPacketSize);
					wPacketSize=0;
				}

				//提取数据
				pClubInfoItem=(tagClubItem *)(cbBuffer+wPacketSize);

				//提取数据
				pClubRules=&pClubInfoItem->ClubRules;
				pClubNotice=&pClubInfoItem->ClubNotice;
				pClubProperty=&pClubInfoItem->ClubProperty;							

				//茶社规则
				pClubRules->cbPayType=m_GameBattleDBAide.GetValue_BYTE(TEXT("PayType"));	
				pClubRules->cbBattleMask=m_GameBattleDBAide.GetValue_BYTE(TEXT("BattleMask"));	
				pClubRules->cbScoreKind = m_GameBattleDBAide.GetValue_BYTE(TEXT("ScoreKind"));
				pClubRules->cbCurrencyKind=m_GameBattleDBAide.GetValue_BYTE(TEXT("CurrencyKind"));					
				pClubRules->wSwitchOption = m_GameBattleDBAide.GetValue_WORD(TEXT("SwitchOption"));		

				//茶社公告
				pClubNotice->cbPublishMode = m_GameBattleDBAide.GetValue_BYTE(TEXT("PublishMode"));
				pClubNotice->cbPublishState = m_GameBattleDBAide.GetValue_BYTE(TEXT("PublishState"));
				m_GameBattleDBAide.GetValue_String(TEXT("NoticeContent"), pClubNotice->szNoticeContent, CountArray(pClubNotice->szNoticeContent));

				//茶社属性
				pClubProperty->dwClubID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));				
				pClubProperty->dwCreaterID=m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterID"));				
				pClubProperty->dwCreaterGameID=m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterGameID"));
				pClubProperty->wClubKind=m_GameBattleDBAide.GetValue_WORD(TEXT("ClubKind"));
				pClubProperty->wClubIconID=m_GameBattleDBAide.GetValue_WORD(TEXT("ClubIconID"));
				pClubProperty->dwPayrollerID=m_GameBattleDBAide.GetValue_DWORD(TEXT("PayrollerID"));
				pClubProperty->wMemberCount=m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));
				pClubProperty->wMaxMemberCount=m_GameBattleDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("CreateDateTime"),pClubProperty->CreateDateTime);
				m_GameBattleDBAide.GetValue_String(TEXT("ClubName"),pClubProperty->szClubName,CountArray(pClubProperty->szClubName));
				m_GameBattleDBAide.GetValue_String(TEXT("ClubIntroduce"),pClubProperty->szClubIntroduce,CountArray(pClubProperty->szClubIntroduce));
				
				//设置位移
				wPacketSize+=sizeof(tagClubItem);

				//移动记录
				m_GameBattleDBModule->MoveToNext();
			}

			//发送剩余
			if (wPacketSize>0) 
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_LIST,dwContextID,dwTokenID,cbBuffer,wPacketSize);
			}

			//列表完成
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_LIST_FINISH,dwContextID,dwTokenID,NULL,0);
		}	

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		return false;
	}

	return true;
}

//茶社列表
bool CDataBaseEngineSink::OnRequestLoadClubItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//提取数据
	DBR_GR_LoadClubItem* pLoadClubItem = (DBR_GR_LoadClubItem*)pData;

	try
	{	
		//构造参数
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubStartID"),pLoadClubItem->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubEndID"),pLoadClubItem->dwClubID);
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_QueryClubList"),true);

		//加载成功
		if (lReturnValue==DB_SUCCESS && m_GameBattleDBModule->IsRecordsetEnd()==false)
		{
			//变量定义
			tagClubItem ClubItem;
			ZeroMemory(&ClubItem,sizeof(ClubItem));

			//变量定义			
			tagClubRules * pClubRules = &ClubItem.ClubRules;			
			tagClubNotice * pClubNotice = &ClubItem.ClubNotice;
			tagClubProperty * pClubProperty=&ClubItem.ClubProperty;						

			//茶社规则
			pClubRules->cbPayType = m_GameBattleDBAide.GetValue_BYTE(TEXT("PayType"));
			pClubRules->cbCurrencyKind = m_GameBattleDBAide.GetValue_BYTE(TEXT("CurrencyKind"));
			pClubRules->cbScoreKind = m_GameBattleDBAide.GetValue_BYTE(TEXT("ScoreKind"));
			pClubRules->cbBattleMask = m_GameBattleDBAide.GetValue_BYTE(TEXT("BattleMask"));			
			pClubRules->wSwitchOption = m_GameBattleDBAide.GetValue_WORD(TEXT("SwitchOption"));				

			//茶社公告
			pClubNotice->cbPublishMode = m_GameBattleDBAide.GetValue_BYTE(TEXT("PublishMode"));
			pClubNotice->cbPublishState = m_GameBattleDBAide.GetValue_BYTE(TEXT("PublishState"));
			m_GameBattleDBAide.GetValue_String(TEXT("NoticeContent"), pClubNotice->szNoticeContent, CountArray(pClubNotice->szNoticeContent));

			//茶社属性
			pClubProperty->dwClubID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));				
			pClubProperty->dwCreaterID=m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterID"));				
			pClubProperty->dwCreaterGameID=m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterGameID"));			
			pClubProperty->wClubKind=m_GameBattleDBAide.GetValue_WORD(TEXT("ClubKind"));
			pClubProperty->wClubIconID = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubIconID"));
			pClubProperty->dwPayrollerID = m_GameBattleDBAide.GetValue_DWORD(TEXT("PayrollerID"));
			pClubProperty->wMemberCount=m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));
			pClubProperty->wMaxMemberCount=m_GameBattleDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
			m_GameBattleDBAide.GetValue_SystemTime(TEXT("CreateDateTime"),pClubProperty->CreateDateTime);
			m_GameBattleDBAide.GetValue_String(TEXT("ClubName"),pClubProperty->szClubName,CountArray(pClubProperty->szClubName));
			m_GameBattleDBAide.GetValue_String(TEXT("ClubIntroduce"),pClubProperty->szClubIntroduce,CountArray(pClubProperty->szClubIntroduce));									

			//发送子项
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_ITEM,dwContextID,dwTokenID,&ClubItem,sizeof(ClubItem));

			//变量定义				
			DBR_GR_LoadClubMember LoadClubMember;
			ZeroMemory(&LoadClubMember,sizeof(LoadClubMember));

			//构造结构
			LoadClubMember.dwClubID=ClubItem.ClubProperty.dwClubID;

			//加载成员
			OnRequestLoadClubMember(dwContextID,dwTokenID,&LoadClubMember,sizeof(LoadClubMember));		

			//构造结构
			DBR_GR_LoadBattleConfig LoadBattleConfig;
			LoadBattleConfig.dwClubID=ClubItem.ClubProperty.dwClubID;

			//加载玩法
			OnRequestLoadBattleConfig(dwContextID, dwTokenID, &LoadBattleConfig, sizeof(LoadBattleConfig));

			//构造结构
			DBR_GR_LoadApplyMessage LoadApplyMessage;
			LoadApplyMessage.dwClubID=ClubItem.ClubProperty.dwClubID;

			//加载成员
			OnRequestLoadApplyMessage(dwContextID, dwTokenID, &LoadApplyMessage, sizeof(LoadApplyMessage));

			//子项完成
			DBO_GR_ClubItemFinish ClubItemFinish = {};

			//设置变量
			ClubItemFinish.dwClubID = pLoadClubItem->dwClubID;
			ClubItemFinish.bActiveRoute = pLoadClubItem->bActiveRoute;

			//列表完成			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_ITEM_FINISH,dwContextID,dwTokenID,&ClubItemFinish,sizeof(ClubItemFinish));
		}		

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//子项完成
		DBO_GR_ClubItemFinish ClubItemFinish = {};

		//设置变量
		ClubItemFinish.dwClubID = pLoadClubItem->dwClubID;
		ClubItemFinish.bActiveRoute = pLoadClubItem->bActiveRoute;

		//列表完成			
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_ITEM_FINISH, dwContextID, dwTokenID, &ClubItemFinish, sizeof(ClubItemFinish));

		return false;
	}

	return true;
}

//输出信息
void CDataBaseEngineSink::TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException)
{
	//错误信息
	char szDesc[512]="\0";
	_snprintf_s(szDesc,511,"%s(%d):%s",pFile,nLine,pFunction);
	CString strDescribe;
#ifdef _UNICODE
	TCHAR szWDesc[512]=TEXT("\0");
	int  nLen=(int)strlen(szDesc);
	MultiByteToWideChar(CP_ACP,0,szDesc,nLen,szWDesc,nLen*2);
	if(pIException!=NULL)
		strDescribe.Format(TEXT("%s:(%s)"),szWDesc,pIException->GetExceptionDescribe());
	else
		strDescribe.Format(TEXT("%s:(未知错误)"),szWDesc);
	CTraceService::TraceString(strDescribe,TraceLevel_Exception);
#else
	if(pIException!=NULL)
		strDescribe.Format(TEXT("%s:(%s)"),szDesc,pIException->GetExceptionDescribe());
	else
		strDescribe.Format(TEXT("%s:(未知错误)"),szDesc);
	CTraceService::TraceString(strDescribe,TraceLevel_Exception);
#endif

}
//////////////////////////////////////////////////////////////////////////

