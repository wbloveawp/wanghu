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
	try
	{
		//创建对象
		if ((m_GameBattleDBModule.GetInterface() == NULL) && (m_GameBattleDBModule.CreateInstance() == false))
		{
			ASSERT(FALSE);
			return false;
		}

		//变量定义		
		auto pIMClubDBParameter=&m_pInitParameter->m_GameBattleDBParameter;

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
	case DBR_GR_LOGON_SERVICE:		//用户登录
	{
		return OnRequestLogonService(dwContextID, dwTokenID,pData,wDataSize);
	}
	case DBR_GR_CREATE_CLUB:		//创建群组
	{
		return OnRequestCreateClub(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_UPDATE_CLUB:		//更新群组
	{
		return OnRequestUpdateClub(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_DELETE_CLUB:		//删除群组
	{
		return OnRequestDeleteClub(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_SEARCH_CLUB:		//查找群组
	{
		return OnRequestSearchClub(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_TRANSFER_CLUB:		//转让群组
	{
		return OnRequestTransferClub(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_UPDATE_MEMBER:		//更新成员
	{
		return OnRequestUpdateMember(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_DELETE_MEMBER:		//删除成员
	{
		return OnRequestDeleteMember(dwContextID, dwTokenID, pData,wDataSize);
	}	
	case DBR_GR_APPLY_REQUEST:		//申请请求
	{
		return OnRequestApplyRequest(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_APPLY_RESPOND:		//申请响应
	{
		return OnRequestApplyRespond(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_APPLY_DELETE:		//申请删除
	{
	return OnRequestApplyDelete(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_APPLY_REPEAL:		//申请回复
	{
	return OnRequestApplyRepeal(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_SETTLE_BATTLE:		//约战结算
	{
	return OnRequestSettleBattle(dwContextID, dwTokenID, pData,wDataSize);
	}
	case DBR_GR_MODIFY_SCORE:		//修改茶社分
	{
	return OnRequestModifyScore(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_PRESENT_SCORE:		//赠送馆分
	{
	return OnRequestPresentScore(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_DELETE_RECORD:		//删除记录
	{
	return OnRequestDeleteRecord(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_DETECT_CLUBNAME:	//检测名称
	{
	return OnRequestDetectClubName(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_CLEANUP_SCORE:		//清除积分
	{
		return OnRequestCleanupScore(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_CLEANUP_REVENUE:	//清除税收
	{
		return OnRequestCleanupRevenue(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_CLEANUP_SETUP:		//清理设置
	{
		return OnRequestCleanupSetup(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_CLEANUP_RANKLIST:	//清除榜单
	{
		return OnRequestCleanupRankList(dwContextID, dwTokenID, pData, wDataSize);
	}	
	case DBR_GR_REQUEST_MERGE:		//请求合并
	{
		return OnRequestRequestMerge(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_RESPOND_MERGE:		//响应合并
	{
		return OnRequestRespondMerge(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_CANCEL_PAYROLL:		//取消代付
	{
		return OnRequestCancelPayroll(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_REQUEST_PAYROLL:	//请求代付
	{
		return OnRequestRequestPayroll(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_RESPOND_PAYROLL:	//响应代付
	{
		return OnRequestRespondPayroll(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBR_GR_LOAD_CLUB_OPTION:	//加载配置
	{
		return OnRequestLoadClubOption(dwContextID, dwTokenID, pData,wDataSize);
	}		
	case DBR_GR_LOAD_FORBID_WORDS:  //禁止词汇
	{
		return OnRequestLoadForbidWords(dwContextID, dwTokenID, pData,wDataSize);
	}
	}

	return false;
}

//用户登录
bool CDataBaseEngineSink::OnRequestLogonService(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//提取数据
	auto pLogonService=(DBR_GR_LogonService *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128]=TEXT("");		

		//构造参数
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"),pLogonService->dwUserID);
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//执行脚本
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_EfficacyUserID"),true);

		//加载成功
		if (lReturnValue==DB_SUCCESS)
		{
			//构造结构
			DBO_GR_LogonSuccess LogonSuccess = {};

			//设置变量
			LogonSuccess.dwUserID = pLogonService->dwUserID;

			//发送消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_SUCCESS, dwContextID, dwTokenID, &LogonSuccess, sizeof(LogonSuccess));

			//变量定义
			WORD wPacketSize = 0;
			tagClubNameCard* pClubNameCard = NULL;

			//读取数据
			while (m_GameBattleDBModule->IsRecordsetEnd() == false)
			{
				//获取对象
				pClubNameCard = (tagClubNameCard*)(m_cbBuffer + wPacketSize);

				//读取数据
				pClubNameCard->dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
				pClubNameCard->wClubKind = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubKind"));
				pClubNameCard->wClubIconID = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubIconID"));
				pClubNameCard->dwCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterID"));
				pClubNameCard->dwCreaterGameID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterGameID"));
				pClubNameCard->wMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));
				pClubNameCard->wMaxMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
				pClubNameCard->wCreaterFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("CreaterFaceID"));
				pClubNameCard->dwCreaterCustomID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterCustomID"));
				m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), pClubNameCard->szClubName, CountArray(pClubNameCard->szClubName));				
				m_GameBattleDBAide.GetValue_String(TEXT("CreaterNickName"), pClubNameCard->szCreaterNickName, CountArray(pClubNameCard->szCreaterNickName));


				wPacketSize += sizeof(tagClubNameCard);

				//移动记录
				m_GameBattleDBModule->MoveToNext();
			}

			//发送消息
			if (wPacketSize > 0)
			{
				//发送数据
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_NAMECARD, dwContextID, dwTokenID, m_cbBuffer, wPacketSize);
			}
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_LogonFailure LogonFailure;
			ZeroMemory(&LogonFailure,sizeof(LogonFailure));

			//设置变量
			LogonFailure.lErrorCode=lReturnValue;
			StringCchCopy(LogonFailure.szErrorDescribe,CountArray(LogonFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize=CountStringBuffer(LogonFailure.szErrorDescribe);
			wPacketSize+=sizeof(LogonFailure)-sizeof(LogonFailure.szErrorDescribe);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE,dwContextID,dwTokenID,&LogonFailure,wPacketSize);

			return true;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////
		//读取申请消息

		//构造参数
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"),pLogonService->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), 0);
		m_GameBattleDBAide.AddParameter(TEXT("@cbMsgType"),APPLY_MSG_TYPE_REQUEST);

		//执行脚本
		lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadApplyMessage"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS && m_GameBattleDBModule->GetRecordCount()>0)
		{
			//获取对象
			WORD wPacketSize=sizeof(DBO_GR_ApplyMessage);
			auto pDBApplyMessage=(DBO_GR_ApplyMessage *)m_cbBuffer;

			//设置变量
			pDBApplyMessage->wMessageCount=0;
			pDBApplyMessage->dwUserID=pLogonService->dwUserID;
			pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_REQUEST;

			//变量定义
			tagClubApplyMessage * pApplyMessage=NULL;

			//读取数据
			while (m_GameBattleDBModule->IsRecordsetEnd()==false)
			{
				if ((wPacketSize+sizeof(tagClubApplyMessage))>sizeof(m_cbBuffer))
				{
					//发送数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,dwTokenID,m_cbBuffer,wPacketSize);

					//设置变量
					pDBApplyMessage->wMessageCount=0;
					wPacketSize=sizeof(DBO_GR_ApplyMessage);
				}

				//获取对象
				pApplyMessage=(tagClubApplyMessage *)(m_cbBuffer+wPacketSize);

				//读取数据
				pApplyMessage->dwMessageID=m_GameBattleDBAide.GetValue_DWORD(TEXT("RecordID"));
				pApplyMessage->cbMessageStatus=m_GameBattleDBAide.GetValue_BYTE(TEXT("ApplyStatus"));

				//申请消息
				pApplyMessage->dwApplyerID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerID"));
				pApplyMessage->dwApplyerGameID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
				pApplyMessage->wApplyerFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("ApplyerFaceID"));
				pApplyMessage->cbApplyerGender = m_GameBattleDBAide.GetValue_BYTE(TEXT("ApplyerGender"));
				pApplyMessage->dwApplyerCustomID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("ApplyDateTime"), pApplyMessage->ApplyDateTime);
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("OperateDateTime"), pApplyMessage->OperateDateTime);
				m_GameBattleDBAide.GetValue_String(TEXT("ApplyerNickName"),pApplyMessage->szApplyerNickName,CountArray(pApplyMessage->szApplyerNickName));

				//群组信息
				pApplyMessage->dwClubID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
				pApplyMessage->dwResponderID =m_GameBattleDBAide.GetValue_DWORD(TEXT("ResponderID"));
				m_GameBattleDBAide.GetValue_String(TEXT("ClubName"),pApplyMessage->szClubName,CountArray(pApplyMessage->szClubName));

				//设置变量
				pDBApplyMessage->wMessageCount++;
				wPacketSize+=sizeof(tagClubApplyMessage);

				//移动记录
				m_GameBattleDBModule->MoveToNext();
			}

			//发送数据
			if (wPacketSize>sizeof(DBO_GR_ApplyMessage))
			{					
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,dwTokenID,m_cbBuffer,wPacketSize);
			}
		}	

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FINISH, dwContextID, dwTokenID, NULL, 0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_LogonFailure LogonFailure;
		ZeroMemory(&LogonFailure,sizeof(LogonFailure));

		//设置变量
		LogonFailure.lErrorCode=1;
		StringCchCopy(LogonFailure.szErrorDescribe,CountArray(LogonFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize=CountStringBuffer(LogonFailure.szErrorDescribe);
		wPacketSize+=sizeof(LogonFailure)-sizeof(LogonFailure.szErrorDescribe);			
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_LOGON_FAILURE,dwContextID,dwTokenID,&LogonFailure,wPacketSize);		
	}

	return true;
}

//创建群组
bool CDataBaseEngineSink::OnRequestCreateClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize==sizeof(DBR_GR_CreateClub));
	if (wDataSize!=sizeof(DBR_GR_CreateClub)) return false;

	//提取数据
	auto pCreateClub=(DBR_GR_CreateClub *)pData;

	try
	{
		//变量定义						
		bool bCreateSuccess=true;
		WCHAR szErrorDescribe[128]=TEXT("");			

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwCreateID"),pCreateClub->dwCreateID);

		//群组属性
		m_GameBattleDBAide.AddParameter(TEXT("@wClubKind"), pCreateClub->wClubKind);
		m_GameBattleDBAide.AddParameter(TEXT("@wClubIconID"), pCreateClub->wClubIconID);
		m_GameBattleDBAide.AddParameter(TEXT("@strClubName"),pCreateClub->szClubName);
		m_GameBattleDBAide.AddParameter(TEXT("@strClubIntroduce"),pCreateClub->szClubIntroduce);		

		//群组规则
		m_GameBattleDBAide.AddParameter(TEXT("@cbPayType"), pCreateClub->cbPayType);
		m_GameBattleDBAide.AddParameter(TEXT("@cbCurrencyKind"), pCreateClub->cbCurrencyKind);
		m_GameBattleDBAide.AddParameter(TEXT("@dwSwitchOption"), pCreateClub->dwSwitchOption);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);		

		//执行脚本
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_CreateClub"),true);		

		//执行成功
		if (lReturnValue==DB_SUCCESS && m_GameBattleDBModule->IsRecordsetEnd()==false)
		{
			//构造结构
			DBO_GR_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate,sizeof(WealthUpdate));					

			//变量定义
			DBO_GR_CreateClub CreateClub;
			ZeroMemory(&CreateClub,sizeof(CreateClub));							

			//读取财富
			WealthUpdate.wItemCount = 1;
			WealthUpdate.WealthItem[0].wMoneyID = GOODS_ID_CARD;
			WealthUpdate.WealthItem[0].lBalance = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("UserCard"));			

			//获取对象
			auto pClubNameCard = &CreateClub.ClubNameCard;

			//读取数据
			pClubNameCard->dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
			pClubNameCard->wClubKind = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubKind"));
			pClubNameCard->wClubIconID = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubIconID"));
			pClubNameCard->dwCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterID"));
			pClubNameCard->dwCreaterGameID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterGameID"));
			pClubNameCard->wMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));
			pClubNameCard->wMaxMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
			pClubNameCard->wCreaterFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("CreaterFaceID"));
			pClubNameCard->dwCreaterCustomID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterCustomID"));
			m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), pClubNameCard->szClubName, CountArray(pClubNameCard->szClubName));
			m_GameBattleDBAide.GetValue_String(TEXT("CreaterNickName"), pClubNameCard->szCreaterNickName, CountArray(pClubNameCard->szCreaterNickName));
				
			////////////////////////////////////////////////////添加成员///////////////////////////////////////////////////

			//构造参数
			m_GameBattleDBAide.ResetParameter();

			//用户信息
			m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pClubNameCard->dwClubID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pClubNameCard->dwCreaterID);
			m_GameBattleDBAide.AddParameter(TEXT("@cbMemberType"), MEMBER_TYPE_EMPLOYER);
			m_GameBattleDBAide.AddParameter(TEXT("@dwOperaterID"), 0);

			//输出参数
			m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

			//执行脚本
			lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_AddMember"),true);
			if (lReturnValue!=DB_SUCCESS)
			{
				//成功标识
				bCreateSuccess = false;

				//构造参数
				m_GameBattleDBAide.ResetParameter();

				//用户信息
				m_GameBattleDBAide.AddParameter(TEXT("@dwCreaterID"), pClubNameCard->dwCreaterID);
				m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pClubNameCard->dwClubID);

				//输出参数
				m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

				//执行脚本
				m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_DeleteClub"),false);

				//设置变量			
				StringCchCopy(szErrorDescribe,CountArray(szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0012, TEXT("由于系统原因，茶社创建失败！")));
			}
			else
			{
				//成员数量
				pClubNameCard->wMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));
			}

			//发送数据
			if (bCreateSuccess==true)
			{
				//创建茶社
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CREATE_CLUB,dwContextID,dwTokenID,&CreateClub,sizeof(CreateClub));

				//更新财富			
				WORD wHeadSize = sizeof(WealthUpdate) - sizeof(WealthUpdate.WealthItem);
				WORD wPacketSize = WealthUpdate.wItemCount * sizeof(WealthUpdate.WealthItem[0]) + wHeadSize;
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE, dwContextID, dwTokenID, &WealthUpdate, wPacketSize);
			}
		}
		else
		{
			//成功标识
			bCreateSuccess = false;

			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//设置变量			
			StringCchCopy(szErrorDescribe,CountArray(szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));
		}

		//创建成功
		if (bCreateSuccess==true)
		{
			//构造结构
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//设置变量
			OperateSuccess.lOperateCode=0;
			StringCchCopy(OperateSuccess.szOperateNotice,CountArray(OperateSuccess.szOperateNotice), CWHService::FormatMessage(MIM_CLUBPLAZA_0013, TEXT("恭喜您，茶社创建成功！")));

			//发送数据
			WORD wPacketSize=CountStringBuffer(OperateSuccess.szOperateNotice);
			wPacketSize+=sizeof(OperateSuccess)-sizeof(OperateSuccess.szOperateNotice);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS,dwContextID,dwTokenID,&OperateSuccess,wPacketSize);			
		}
		else
		{
			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode=lReturnValue;						
			StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), szErrorDescribe);

			//发送数据
			WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;
}

//更新群组
bool CDataBaseEngineSink::OnRequestUpdateClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize>=sizeof(DBR_GR_UpdateClub));
	if (wDataSize<sizeof(DBR_GR_UpdateClub)) return false;

	//提取数据
	auto pUpdateClub=(DBR_GR_UpdateClub *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128]=TEXT("");

		//变量定义
		LPBYTE pcbBuffer=(LPBYTE)pData;
		WORD wOffsetPos=sizeof(DBR_GR_UpdateClub);

		//变量定义		
		WORD wHeadSize=sizeof(DBO_GR_UpdateClub);
		auto pUpdateClubResult=(DBO_GR_UpdateClub*)m_cbBuffer;

		//群组标识
		pUpdateClubResult->dwClubID=pUpdateClub->dwClubID;		

		//更新规则
		if ((pUpdateClub->cbUpdateKind&UPCLUB_KIND_RULES) != 0)
		{
			//获取对象
			auto pClubRules = (tagClubUpdate_Rules *)(pcbBuffer + wOffsetPos);

			//设置偏移
			wOffsetPos += sizeof(tagClubUpdate_Rules);

			//构造参数
			m_GameBattleDBAide.ResetParameter();

			//用户信息
			m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pUpdateClub->dwUserID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pUpdateClub->dwClubID);

			//规则信息
			m_GameBattleDBAide.AddParameter(TEXT("@cbBattleMask"), pClubRules->cbBattleMask);			
			m_GameBattleDBAide.AddParameter(TEXT("@cbSwitchOption"), pClubRules->wSwitchOption);			

			//输出参数
			m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

			//执行脚本
			LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_UpdateClubRules"), true);

			//执行成功
			if (lReturnValue == DB_SUCCESS)
			{
				//设置类型
				pUpdateClubResult->cbUpdateKind = UPCLUB_KIND_RULES;

				//变量定义
				auto pClubRules = (tagClubRules *)&m_cbBuffer[wHeadSize];

				//群组规则
				pClubRules->cbPayType = m_GameBattleDBAide.GetValue_BYTE(TEXT("PayType"));
				pClubRules->cbBattleMask = m_GameBattleDBAide.GetValue_BYTE(TEXT("BattleMask"));
				pClubRules->cbScoreKind = m_GameBattleDBAide.GetValue_BYTE(TEXT("ScoreKind"));				
				pClubRules->wSwitchOption = m_GameBattleDBAide.GetValue_WORD(TEXT("SwitchOption"));
				pClubRules->cbCurrencyKind = m_GameBattleDBAide.GetValue_BYTE(TEXT("CurrencyKind"));							

				//发送数据
				WORD wPacketSize = wHeadSize + sizeof(tagClubRules);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_CLUB, dwContextID, dwTokenID, m_cbBuffer, wPacketSize);
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
				StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

				//发送数据
				WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
				wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);

				return true;
			}
		}

		//更新公告
		if ((pUpdateClub->cbUpdateKind & UPCLUB_KIND_NOTICE) != 0)
		{
			//获取对象
			auto pClubNotice = (tagClubUpdate_Notice*)(pcbBuffer + wOffsetPos);

			//设置偏移
			wOffsetPos += sizeof(tagClubUpdate_Notice);

			//构造参数
			m_GameBattleDBAide.ResetParameter();

			//用户信息
			m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pUpdateClub->dwUserID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pUpdateClub->dwClubID);

			//规则信息
			m_GameBattleDBAide.AddParameter(TEXT("@cbPublishMode"), pClubNotice->cbPublishMode);
			m_GameBattleDBAide.AddParameter(TEXT("@cbPublishState"), pClubNotice->cbPublishState);
			m_GameBattleDBAide.AddParameter(TEXT("@strNoticeContent"), pClubNotice->szNoticeContent);

			//输出参数
			m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

			//执行脚本
			LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_UpdateClubNotice"), true);

			//执行成功
			if (lReturnValue == DB_SUCCESS)
			{
				//设置类型
				pUpdateClubResult->cbUpdateKind = UPCLUB_KIND_NOTICE;

				//变量定义
				auto pClubRules = (tagClubNotice*)&m_cbBuffer[wHeadSize];

				//公告信息
				pClubRules->cbPublishMode = m_GameBattleDBAide.GetValue_BYTE(TEXT("PublishMode"));
				pClubRules->cbPublishState = m_GameBattleDBAide.GetValue_BYTE(TEXT("PublishState"));
				m_GameBattleDBAide.GetValue_String(TEXT("NoticeContent"), pClubRules->szNoticeContent, CountArray(pClubRules->szNoticeContent));

				//发送数据
				WORD wPacketSize = wHeadSize + sizeof(tagClubNotice);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_CLUB, dwContextID, dwTokenID, m_cbBuffer, wPacketSize);
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
				StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

				//发送数据
				WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
				wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);

				return true;
			}
		}

		//更新财富
		//if ((pUpdateClub->cbUpdateKind&UPCLUB_KIND_WEALTH)!=0)
		//{
		//	//获取对象
		//	tagClubWealth * pClubWealth=(tagClubWealth *)(pcbBuffer+wOffsetPos);

		//	//设置偏移
		//	wOffsetPos+=sizeof(tagClubWealth);

		//	//构造参数
		//	m_GameBattleDBAide.ResetParameter();

		//	//用户信息
		//	m_GameBattleDBAide.AddParameter(TEXT("@dwCreateID"),pUpdateClub->dwCreaterID);
		//	m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pUpdateClub->dwClubID);

		//	//财富信息
		//	m_GameBattleDBAide.AddParameter(TEXT("@lRoomCard"),pClubWealth->lRoomCard);

		//	//输出参数
		//	m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//	//执行脚本
		//	LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_UpdateClubWealth"),true);

		//	//执行成功
		//	if (lReturnValue==DB_SUCCESS)
		//	{
		//		//设置类型
		//		pUpdateClubResult->cbUpdateKind=UPCLUB_KIND_WEALTH;

		//		//变量定义
		//		tagClubWealth * pClubWealth=(tagClubWealth *)&m_cbBuffer[wHeadSize];								

		//		//群组财富
		//		pClubWealth->lRoomCard=m_GameBattleDBAide.GetValue_LONGLONG(TEXT("RoomCard"));					

		//		//发送数据
		//		WORD wPacketSize=wHeadSize+sizeof(tagClubWealth);
		//		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_CLUB,dwContextID,dwTokenID,m_cbBuffer,wPacketSize);

		//		//构造结构
		//		DBO_GR_WealthUpdate WealthUpdate;
		//		ZeroMemory(&WealthUpdate,sizeof(WealthUpdate));

		//		//用户财富
		//		WealthUpdate.dwUserID=pUpdateClub->dwCreaterID;
		//		WealthUpdate.cbWealthMask = WEALTH_MASK_CARD;
		//		WealthUpdate.lUserCard=m_GameBattleDBAide.GetValue_LONGLONG(TEXT("UserCard"));					

		//		//发送数据
		//		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_WEALTH_UPDATE,dwContextID,dwTokenID,&WealthUpdate,sizeof(WealthUpdate));
		//	}
		//	else
		//	{
		//		//获取参数
		//		CDBVarValue DBVarValue;
		//		m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

		//		//构造结构
		//		DBO_GR_OperateFailure OperateFailure;
		//		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//		//设置变量
		//		OperateFailure.lErrorCode=lReturnValue;
		//		OperateFailure.dwUserID=pUpdateClub->dwCreaterID;			
		//		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

		//		//发送数据
		//		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		//		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
		//		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);

		//		return true;
		//	}
		//}

		//更新属性
		if ((pUpdateClub->cbUpdateKind&UPCLUB_KIND_PROPERTY)!=0)
		{
			//获取对象
			auto pClubProperty=(tagClubUpdate_Property *)(pcbBuffer+wOffsetPos);

			//设置偏移
			wOffsetPos+=sizeof(tagClubUpdate_Property);

			//构造参数
			m_GameBattleDBAide.ResetParameter();

			//用户信息
			m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"),pUpdateClub->dwUserID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pUpdateClub->dwClubID);

			//属性信息
			m_GameBattleDBAide.AddParameter(TEXT("@wClubIconID"), pClubProperty->wClubIconID);
			m_GameBattleDBAide.AddParameter(TEXT("@strClubName"), pClubProperty->szClubName);
			m_GameBattleDBAide.AddParameter(TEXT("@strClubIntroduce"),pClubProperty->szClubIntroduce);			

			//输出参数
			m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

			//执行脚本
			LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_UpdateClubProperty"),true);

			//执行成功
			if (lReturnValue==DB_SUCCESS)
			{
				//设置类型
				pUpdateClubResult->cbUpdateKind=UPCLUB_KIND_PROPERTY;

				//变量定义
				auto pClubProperty=(tagClubProperty *)&m_cbBuffer[wHeadSize];

				//群组属性
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
				
				//发送数据
				WORD wPacketSize=wHeadSize+sizeof(tagClubProperty);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_CLUB,dwContextID,dwTokenID,m_cbBuffer,wPacketSize);
			}
			else
			{
				//获取参数
				CDBVarValue DBVarValue;
				m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

				//构造结构
				DBO_GR_OperateFailure OperateFailure;
				ZeroMemory(&OperateFailure,sizeof(OperateFailure));

				//设置变量
				OperateFailure.lErrorCode=lReturnValue;				
				StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

				//发送数据
				WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
				wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);

				return true;
			}
		}		

		//构造结构
		DBO_GR_OperateSuccess OperateSuccess;
		ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

		//设置变量
		OperateSuccess.lOperateCode=0;	

		//发送数据
		WORD wPacketSize=CountStringBuffer(OperateSuccess.szOperateNotice);
		wPacketSize+=sizeof(OperateSuccess)-sizeof(OperateSuccess.szOperateNotice);			
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS,dwContextID,dwTokenID,&OperateSuccess,wPacketSize);	

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;

	return true;
}

//删除群组
bool CDataBaseEngineSink::OnRequestDeleteClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize==sizeof(DBR_GR_DeleteClub));
	if (wDataSize!=sizeof(DBR_GR_DeleteClub)) return false;

	//提取数据
	auto pDeleteClub=(DBR_GR_DeleteClub *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128]=TEXT("");			

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwCreaterID"),pDeleteClub->dwCreaterID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pDeleteClub->dwClubID);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//执行脚本
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_DeleteClub"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS)
		{           
			//构造结构
			DBO_GR_DeleteClub DeleteClub;
			ZeroMemory(&DeleteClub,sizeof(DeleteClub));

			//设置变量
			DeleteClub.dwUserID=pDeleteClub->dwCreaterID;
			DeleteClub.dwClubID=pDeleteClub->dwClubID;
			m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), DeleteClub.szClubName,CountArray(DeleteClub.szClubName));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DELETE_CLUB,dwContextID,dwTokenID,&DeleteClub,sizeof(DeleteClub));
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode=lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;
}

//查找群组
bool CDataBaseEngineSink::OnRequestSearchClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize == sizeof(DBR_GR_SearchClub));
	if (wDataSize != sizeof(DBR_GR_SearchClub)) return false;

	//提取数据
	auto pSearchClub = (DBR_GR_SearchClub *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pSearchClub->dwClubID);
		
		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_SearchClub"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_SearchClub SearchClub;
			ZeroMemory(&SearchClub, sizeof(SearchClub));

			//设置变量
			SearchClub.dwUserID = pSearchClub->dwUserID;
			
			//获取对象
			auto pClubNameCard = &SearchClub.ClubNameCard;

			//读取数据
			pClubNameCard->dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
			pClubNameCard->wClubKind = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubKind"));
			pClubNameCard->wClubIconID = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubIconID"));
			pClubNameCard->dwCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterID"));
			pClubNameCard->dwCreaterGameID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterGameID"));
			pClubNameCard->wMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));
			pClubNameCard->wMaxMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
			pClubNameCard->wCreaterFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("CreaterFaceID"));
			pClubNameCard->dwCreaterCustomID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterCustomID"));
			m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), pClubNameCard->szClubName, CountArray(pClubNameCard->szClubName));
			m_GameBattleDBAide.GetValue_String(TEXT("CreaterNickName"), pClubNameCard->szCreaterNickName, CountArray(pClubNameCard->szCreaterNickName));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_SEARCH_CLUB, dwContextID, dwTokenID, &SearchClub, sizeof(SearchClub));
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
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//转让群组
bool CDataBaseEngineSink::OnRequestTransferClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize == sizeof(DBR_GR_TransferClub));
	if (wDataSize != sizeof(DBR_GR_TransferClub)) return false;

	//提取数据
	auto pTransferClub = (DBR_GR_TransferClub *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pTransferClub->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwCreaterID"), pTransferClub->dwCreaterID);		
		m_GameBattleDBAide.AddParameter(TEXT("@dwNewCreaterID"), pTransferClub->dwNewCreaterID);		
		m_GameBattleDBAide.AddParameter(TEXT("@szAuthCodeMD5"), pTransferClub->szAuthCodeMD5);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_TransferClub"), false);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_TransferClub TransferClub;
			ZeroMemory(&TransferClub, sizeof(TransferClub));

			//设置变量
			TransferClub.dwClubID = pTransferClub->dwClubID;
			TransferClub.dwUserID = pTransferClub->dwCreaterID;			
			TransferClub.dwNewCreaterID = pTransferClub->dwNewCreaterID;

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_TRANSFER_CLUB, dwContextID, dwTokenID, &TransferClub, sizeof(TransferClub));
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
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;		
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//更新成员
bool CDataBaseEngineSink::OnRequestUpdateMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize >= sizeof(DBR_GR_UpdateMember));
	if (wDataSize < sizeof(DBR_GR_UpdateMember)) return false;

	//提取数据
	auto pUpdateMember = (DBR_GR_UpdateMember *)pData;

	try
	{
		//变量定义
		LONG lReturnValue = DB_SUCCESS;
		WCHAR szErrorDescribe[128] = TEXT("");

		//更新成员类型
		if (pUpdateMember->wUpdateKind == UPMEMBER_KIND_TYPE)
		{
			//成员类型
			BYTE cbMemberType = *(BYTE *)(pUpdateMember + 1);
			DWORD dwManageRight = *(DWORD *)((BYTE*)(pUpdateMember + 1)+1);

			//构造参数
			m_GameBattleDBAide.ResetParameter();

			//用户信息
			m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pUpdateMember->dwUserID);			
			m_GameBattleDBAide.AddParameter(TEXT("dwClubID"), pUpdateMember->dwClubID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pUpdateMember->dwMemberID);
			m_GameBattleDBAide.AddParameter(TEXT("@cbMemberType"), cbMemberType);
			m_GameBattleDBAide.AddParameter(TEXT("@dwManageRight"), dwManageRight);

			//输出参数
			m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

			//执行脚本
			lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_UpdateMemberType"), true);
		}		

		//更新成员状态
		if (pUpdateMember->wUpdateKind == UPMEMBER_KIND_STATUS)
		{
			//成员状态
			BYTE cbMemberStatus= *(BYTE *)(pUpdateMember + 1);

			//构造参数
			m_GameBattleDBAide.ResetParameter();

			//用户信息
			m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pUpdateMember->dwUserID);
			
			//群组信息
			m_GameBattleDBAide.AddParameter(TEXT("dwClubID"), pUpdateMember->dwClubID);
			m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pUpdateMember->dwMemberID);
			m_GameBattleDBAide.AddParameter(TEXT("@cbMemberStatus"), cbMemberStatus);

			//输出参数
			m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

			//执行脚本
			lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_UpdateMemberStatus"), true);
		}

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义
			DBO_GR_UpdateMember UpdateMember;
			ZeroMemory(&UpdateMember, sizeof(UpdateMember));

			//更新类型
			UpdateMember.dwClubID = pUpdateMember->dwClubID;
			UpdateMember.wUpdateKind = pUpdateMember->wUpdateKind;

			//变量定义
			tagClubMemberInfo * pClubMember = &UpdateMember.ClubMember;

			//成员信息
			pClubMember->dwMemberID = m_GameBattleDBAide.GetValue_DWORD(TEXT("MemberID"));
			pClubMember->cbMemberType = m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberType"));
			pClubMember->cbMemberStatus = m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberStatus"));
			pClubMember->dwManageRight = m_GameBattleDBAide.GetValue_DWORD(TEXT("ManageRight"));
			pClubMember->lClubScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("ClubScore"));
			pClubMember->dwCreateCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreateCount"));
			pClubMember->dwBattleCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("BattleCount"));
			pClubMember->lRevenueCount = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("RevenueCount"));
			m_GameBattleDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pClubMember->JoinDateTime);
			m_GameBattleDBAide.GetValue_SystemTime(TEXT("LastBattleTime"), pClubMember->LastBattleTime);			

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_MEMBER, dwContextID, dwTokenID, &UpdateMember, sizeof(UpdateMember));

			//构造结构
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//设置变量
			OperateSuccess.lOperateCode = 0;
			StringCchCopy(OperateSuccess.szOperateNotice, CountArray(OperateSuccess.szOperateNotice), CWHService::FormatMessage(MIM_CLUBPLAZA_0014, TEXT("操作成功！")));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateSuccess.szOperateNotice);
			wPacketSize += sizeof(OperateSuccess) - sizeof(OperateSuccess.szOperateNotice);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wPacketSize);
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
			if (DBVarValue.bstrVal)
			{
				StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));
			}

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//删除成员
bool CDataBaseEngineSink::OnRequestDeleteMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize==sizeof(DBR_GR_DeleteMember));
	if (wDataSize!=sizeof(DBR_GR_DeleteMember)) return false;

	//提取数据
	auto pDeleteMember=(DBR_GR_DeleteMember *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128]=TEXT("");			

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"),pDeleteMember->dwUserID);		
		m_GameBattleDBAide.AddParameter(TEXT("@cbDeleteKind"), pDeleteMember->cbDeleteKind);

		//群组信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pDeleteMember->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"),pDeleteMember->dwMemberID);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//执行脚本
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_DeleteMember"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS)
		{
			//变量定义
			DBO_GR_DeleteMember DeleteMember;
			ZeroMemory(&DeleteMember,sizeof(DeleteMember));

			//群组信息
			DeleteMember.dwUserID=pDeleteMember->dwUserID;
			DeleteMember.dwClubID=pDeleteMember->dwClubID;
			DeleteMember.dwMemberID=pDeleteMember->dwMemberID;
			DeleteMember.cbDeleteKind=pDeleteMember->cbDeleteKind;
			DeleteMember.wMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));

			//群组名称
			m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), DeleteMember.szClubName, CountArray(DeleteMember.szClubName));
			
			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DELETE_MEMBER,dwContextID,dwTokenID,&DeleteMember,sizeof(DeleteMember));

			//构造结构
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//设置变量
			OperateSuccess.lOperateCode = 0;
			StringCchCopy(OperateSuccess.szOperateNotice, CountArray(OperateSuccess.szOperateNotice), CWHService::FormatMessage(MIM_CLUBPLAZA_0014, TEXT("操作成功！")));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateSuccess.szOperateNotice);
			wPacketSize += sizeof(OperateSuccess) - sizeof(OperateSuccess.szOperateNotice);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wPacketSize);			
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode=lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;
}

//申请请求
bool CDataBaseEngineSink::OnRequestApplyRequest(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize==sizeof(DBR_GR_ApplyRequest));
	if (wDataSize!=sizeof(DBR_GR_ApplyRequest)) return false;

	//提取数据
	auto pApplyRequest=(DBR_GR_ApplyRequest *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128]=TEXT("");			

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pApplyRequest->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwApplyerID"),pApplyRequest->dwApplyerID);							
		
		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//执行脚本
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_ApplyRequest"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS)
		{
			//获取对象
			tagClubApplyMessage ApplyMessage;
			ZeroMemory(&ApplyMessage,sizeof(ApplyMessage));			

			//读取数据
			ApplyMessage.dwMessageID=m_GameBattleDBAide.GetValue_DWORD(TEXT("RecordID"));
			ApplyMessage.cbMessageStatus=m_GameBattleDBAide.GetValue_BYTE(TEXT("ApplyStatus"));

			//申请消息
			ApplyMessage.dwApplyerID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerID"));
			ApplyMessage.dwApplyerGameID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
			ApplyMessage.wApplyerFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("ApplyerFaceID"));
			ApplyMessage.cbApplyerGender = m_GameBattleDBAide.GetValue_BYTE(TEXT("ApplyerGender"));
			ApplyMessage.dwApplyerCustomID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
			m_GameBattleDBAide.GetValue_SystemTime(TEXT("ApplyDateTime"),ApplyMessage.ApplyDateTime);
			m_GameBattleDBAide.GetValue_SystemTime(TEXT("OperateDateTime"), ApplyMessage.OperateDateTime);
			m_GameBattleDBAide.GetValue_String(TEXT("ApplyerNickName"),ApplyMessage.szApplyerNickName,CountArray(ApplyMessage.szApplyerNickName));

			//群组信息
			ApplyMessage.dwClubID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
			ApplyMessage.dwResponderID =m_GameBattleDBAide.GetValue_DWORD(TEXT("ResponderID"));
			m_GameBattleDBAide.GetValue_String(TEXT("ClubName"),ApplyMessage.szClubName,CountArray(ApplyMessage.szClubName));

			//获取对象			
			auto pDBApplyMessage=(DBO_GR_ApplyMessage *)m_cbBuffer;

			//设置变量
			pDBApplyMessage->wMessageCount=1;
			CopyMemory(pDBApplyMessage+1,&ApplyMessage,sizeof(ApplyMessage));

			///////////////////////////////////////////////////////////////////////////////////////////////////////			

			//设置变量			
			pDBApplyMessage->dwUserID=ApplyMessage.dwApplyerID;
			pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_REQUEST;		
			
			//发送数据
			WORD wPacketSize=sizeof(DBO_GR_ApplyMessage)+sizeof(ApplyMessage);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,dwTokenID,pDBApplyMessage,wPacketSize);

			///////////////////////////////////////////////////////////////////////////////////////////////////////

			//设置变量
			pDBApplyMessage->dwClubID=ApplyMessage.dwClubID;
			pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_RESPOND;		
			
			//发送数据
			wPacketSize=sizeof(DBO_GR_ApplyMessage)+sizeof(ApplyMessage);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,dwTokenID,pDBApplyMessage,wPacketSize);

			///////////////////////////////////////////////////////////////////////////////////////////////////////

			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//同意状态
			if (ApplyMessage.cbMessageStatus == APPLY_STATUS_AGREE)
			{
				//群组名片
				tagClubNameCard ClubNameCard = {};

				//读取数据
				ClubNameCard.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
				ClubNameCard.wClubKind = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubKind"));
				ClubNameCard.wClubIconID = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubIconID"));
				ClubNameCard.dwCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterID"));
				ClubNameCard.dwCreaterGameID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterGameID"));
				ClubNameCard.wMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));
				ClubNameCard.wMaxMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
				ClubNameCard.wCreaterFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("CreaterFaceID"));
				ClubNameCard.dwCreaterCustomID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterCustomID"));
				m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), ClubNameCard.szClubName, CountArray(ClubNameCard.szClubName));
				m_GameBattleDBAide.GetValue_String(TEXT("CreaterNickName"), ClubNameCard.szCreaterNickName, CountArray(ClubNameCard.szCreaterNickName));

				//构造参数
				m_GameBattleDBAide.ResetParameter();

				//用户信息
				m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pApplyRequest->dwClubID);
				m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pApplyRequest->dwApplyerID);
				m_GameBattleDBAide.AddParameter(TEXT("@cbMemberType"), MEMBER_TYPE_MEMBER);
				m_GameBattleDBAide.AddParameter(TEXT("@dwOperaterID"), 0);

				//输出参数
				m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

				//执行脚本
				lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_AddMember"), true);

				if (lReturnValue == DB_SUCCESS)
				{
					//变量定义
					DBO_GR_InsertMember InsertMember;
					ZeroMemory(&InsertMember, sizeof(InsertMember));

					//设置变量
					InsertMember.dwClubID = pApplyRequest->dwClubID;
					CopyMemory(&InsertMember.ClubNameCard,&ClubNameCard,sizeof(InsertMember.ClubNameCard));

					//变量定义
					tagClubUserInfo * pUserInfo = &InsertMember.ClubMemberUser.UserInfo;
					tagClubMemberInfo * pClubMember = &InsertMember.ClubMemberUser.MemberInfo;

					//成员数量
					InsertMember.ClubNameCard.wMemberCount= m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));

					//成员信息
					pClubMember->dwMemberID = m_GameBattleDBAide.GetValue_DWORD(TEXT("UserID"));
					pClubMember->cbMemberType = m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberType"));
					pClubMember->cbMemberStatus = m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberStatus"));
					pClubMember->dwManageRight = m_GameBattleDBAide.GetValue_DWORD(TEXT("ManageRight"));
					pClubMember->lClubScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("ClubScore"));
					pClubMember->dwCreateCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreateCount"));
					pClubMember->dwBattleCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("BattleCount"));					
					pClubMember->lRevenueCount = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("RevenueCount"));
					m_GameBattleDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pClubMember->JoinDateTime);
					m_GameBattleDBAide.GetValue_SystemTime(TEXT("LastBattleTime"), pClubMember->LastBattleTime);

					//用户信息
					pUserInfo->dwUserID = m_GameBattleDBAide.GetValue_DWORD(TEXT("UserID"));
					pUserInfo->dwGameID = m_GameBattleDBAide.GetValue_DWORD(TEXT("GameID"));
					pUserInfo->cbGender = m_GameBattleDBAide.GetValue_BYTE(TEXT("Gender"));
					pUserInfo->wFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("FaceID"));
					pUserInfo->dwCustomID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CustomID"));
					m_GameBattleDBAide.GetValue_String(TEXT("NickName"), pUserInfo->szNickName, CountArray(pUserInfo->szNickName));

					//投递数据
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_INSERT_MEMBER, dwContextID, dwTokenID, &InsertMember, sizeof(InsertMember));					
				}
				else
				{
					//构造结构
					DBO_GR_OperateFailure OperateFailure;
					ZeroMemory(&OperateFailure, sizeof(OperateFailure));

					//设置变量
					OperateFailure.lErrorCode = lReturnValue;
					StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

					//发送数据
					WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
					wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);

					return true;
				}
			}	

			//构造结构
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//设置变量
			OperateSuccess.lOperateCode = 0;
			StringCchCopy(OperateSuccess.szOperateNotice, CountArray(OperateSuccess.szOperateNotice),(LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			wPacketSize = CountStringBuffer(OperateSuccess.szOperateNotice);
			wPacketSize += sizeof(OperateSuccess) - sizeof(OperateSuccess.szOperateNotice);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wPacketSize);
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode=lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe),CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;
}

//申请响应
bool CDataBaseEngineSink::OnRequestApplyRespond(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize==sizeof(DBR_GR_ApplyRespond));
	if (wDataSize!=sizeof(DBR_GR_ApplyRespond)) return false;

	//提取数据
	auto pApplyRespond=(DBR_GR_ApplyRespond *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128]=TEXT("");			

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwRecordID"),pApplyRespond->dwMessageID);		
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"),pApplyRespond->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwApplyerID"),pApplyRespond->dwApplyerID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pApplyRespond->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbApplyStatus"),pApplyRespond->cbApplyStatus);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//执行脚本
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_ApplyRespond"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS)
		{
			//获取对象
			tagClubApplyMessage ApplyMessage;
			ZeroMemory(&ApplyMessage,sizeof(ApplyMessage));

			//变量定义
			CDWordArray ApplyerArray;
			WCHAR szClubName[LEN_CLUB_NAME] = { 0 };

			//读取数据
			while (m_GameBattleDBModule->IsRecordsetEnd()==false)
			{
				//读取数据				
				ApplyMessage.dwMessageID=m_GameBattleDBAide.GetValue_DWORD(TEXT("RecordID"));
				ApplyMessage.cbMessageStatus=pApplyRespond->cbApplyStatus;

				//申请消息
				ApplyMessage.dwApplyerID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerID"));
				ApplyMessage.dwApplyerGameID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
				ApplyMessage.wApplyerFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("ApplyerFaceID"));
				ApplyMessage.cbApplyerGender = m_GameBattleDBAide.GetValue_BYTE(TEXT("ApplyerGender"));
				ApplyMessage.dwApplyerCustomID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("ApplyDateTime"), ApplyMessage.ApplyDateTime);
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("OperateDateTime"), ApplyMessage.OperateDateTime);
				m_GameBattleDBAide.GetValue_String(TEXT("ApplyerNickName"),ApplyMessage.szApplyerNickName,CountArray(ApplyMessage.szApplyerNickName));

				//群组信息
				ApplyMessage.dwClubID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
				ApplyMessage.dwResponderID =m_GameBattleDBAide.GetValue_DWORD(TEXT("ResponderID"));
				m_GameBattleDBAide.GetValue_String(TEXT("ClubName"),ApplyMessage.szClubName,CountArray(ApplyMessage.szClubName));

				//茶馆名称
				if (szClubName[0] == 0)
				{
					StringCchCopy(szClubName, CountArray(szClubName), ApplyMessage.szClubName);
				}

				//获取对象			
				auto pDBApplyMessage=(DBO_GR_ApplyMessage *)m_cbBuffer;

				//拷贝数据
				pDBApplyMessage->wMessageCount=1;
				CopyMemory(pDBApplyMessage+1,&ApplyMessage,sizeof(ApplyMessage));

				///////////////////////////////////////////////////////////////////////////////////////////////////////			

				//设置变量
				pDBApplyMessage->dwUserID=ApplyMessage.dwApplyerID;
				pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_REQUEST;		
				
				//发送数据
				WORD wPacketSize=sizeof(DBO_GR_ApplyMessage)+sizeof(ApplyMessage);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,dwTokenID,pDBApplyMessage,wPacketSize);

				///////////////////////////////////////////////////////////////////////////////////////////////////////

				//设置变量
				pDBApplyMessage->dwClubID=ApplyMessage.dwClubID;
				pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_RESPOND;		
				
				//发送数据
				wPacketSize=sizeof(DBO_GR_ApplyMessage)+sizeof(ApplyMessage);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,dwTokenID,pDBApplyMessage,wPacketSize);

				//////////////////////////////////////////////////////////////////////////////////////////////////////

				//添加用户
				ApplyerArray.Add(ApplyMessage.dwApplyerID);

				//移动记录
				m_GameBattleDBModule->MoveToNext();
			}			

			////获取参数
			//CDBVarValue DBVarValue;
			//m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//设置变量
			OperateSuccess.lOperateCode=0;
			//StringCchCopy(OperateSuccess.szOperateNotice,(LPCWSTR)(DBVarValue.bstrVal),CountArray(OperateSuccess.szOperateNotice));	

			//发送数据
			WORD wPacketSize=CountStringBuffer(OperateSuccess.szOperateNotice);
			wPacketSize+=sizeof(OperateSuccess)-sizeof(OperateSuccess.szOperateNotice);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS,dwContextID,dwTokenID,&OperateSuccess,wPacketSize);

			//添加成员
			if (pApplyRespond->cbApplyStatus==APPLY_STATUS_AGREE)
			{
				//构造参数
				m_GameBattleDBAide.ResetParameter();				
				m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pApplyRespond->dwClubID);
				
				//执行脚本
				LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_QueryClubNameCard"), true);
				if (lReturnValue == DB_SUCCESS)
				{
					//群组名片
					tagClubNameCard ClubNameCard = {};

					//读取数据
					ClubNameCard.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
					ClubNameCard.wClubKind = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubKind"));
					ClubNameCard.wClubIconID = m_GameBattleDBAide.GetValue_WORD(TEXT("ClubIconID"));
					ClubNameCard.dwCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterID"));
					ClubNameCard.dwCreaterGameID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterGameID"));
					ClubNameCard.wMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));
					ClubNameCard.wMaxMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxMemberCount"));
					ClubNameCard.wCreaterFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("CreaterFaceID"));
					ClubNameCard.dwCreaterCustomID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterCustomID"));
					m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), ClubNameCard.szClubName, CountArray(ClubNameCard.szClubName));
					m_GameBattleDBAide.GetValue_String(TEXT("CreaterNickName"), ClubNameCard.szCreaterNickName, CountArray(ClubNameCard.szCreaterNickName));

					for (INT_PTR i = 0; i < ApplyerArray.GetCount(); i++)
					{
						//构造参数
						m_GameBattleDBAide.ResetParameter();

						//用户信息
						m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pApplyRespond->dwClubID);
						m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), ApplyerArray[i]);
						m_GameBattleDBAide.AddParameter(TEXT("@cbMemberType"), MEMBER_TYPE_MEMBER);
						m_GameBattleDBAide.AddParameter(TEXT("@dwOperaterID"), pApplyRespond->dwUserID);

						//输出参数
						m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

						//执行脚本
						lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_AddMember"), true);

						if (lReturnValue == DB_SUCCESS)
						{
							//变量定义
							DBO_GR_InsertMember InsertMember;
							ZeroMemory(&InsertMember, sizeof(InsertMember));

							//设置变量
							InsertMember.dwClubID = pApplyRespond->dwClubID;
							CopyMemory(&InsertMember.ClubNameCard, &ClubNameCard, sizeof(InsertMember.ClubNameCard));

							//变量定义
							tagClubUserInfo* pUserInfo = &InsertMember.ClubMemberUser.UserInfo;
							tagClubMemberInfo* pClubMember = &InsertMember.ClubMemberUser.MemberInfo;

							//成员数量
							InsertMember.ClubNameCard.wMemberCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MemberCount"));

							//成员信息
							pClubMember->dwMemberID = m_GameBattleDBAide.GetValue_DWORD(TEXT("MemberID"));
							pClubMember->cbMemberType = m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberType"));
							pClubMember->cbMemberStatus = m_GameBattleDBAide.GetValue_BYTE(TEXT("MemberStatus"));
							pClubMember->dwManageRight = m_GameBattleDBAide.GetValue_DWORD(TEXT("ManageRight"));
							pClubMember->lClubScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("ClubScore"));
							pClubMember->dwCreateCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreateCount"));
							pClubMember->dwBattleCount = m_GameBattleDBAide.GetValue_DWORD(TEXT("BattleCount"));
							pClubMember->lRevenueCount = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("RevenueCount"));
							m_GameBattleDBAide.GetValue_SystemTime(TEXT("JoinDateTime"), pClubMember->JoinDateTime);
							m_GameBattleDBAide.GetValue_SystemTime(TEXT("LastBattleTime"), pClubMember->LastBattleTime);

							//用户信息
							pUserInfo->dwUserID = m_GameBattleDBAide.GetValue_DWORD(TEXT("UserID"));
							pUserInfo->dwGameID = m_GameBattleDBAide.GetValue_DWORD(TEXT("GameID"));
							pUserInfo->cbGender = m_GameBattleDBAide.GetValue_BYTE(TEXT("Gender"));
							pUserInfo->wFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("FaceID"));
							pUserInfo->dwCustomID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CustomID"));
							m_GameBattleDBAide.GetValue_String(TEXT("NickName"), pUserInfo->szNickName, CountArray(pUserInfo->szNickName));

							//投递数据
							m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_INSERT_MEMBER, dwContextID, dwTokenID, &InsertMember, sizeof(InsertMember));
						}
					}
				}				
			}
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode=lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);

			return true;
		}				

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;
}

//申请删除
bool CDataBaseEngineSink::OnRequestApplyDelete(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize==sizeof(DBR_GR_ApplyDelete));
	if (wDataSize!=sizeof(DBR_GR_ApplyDelete)) return false;

	//提取数据
	auto pApplyDelete=(DBR_GR_ApplyDelete *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128]=TEXT("");			

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pApplyDelete->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pApplyDelete->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMessageID"),pApplyDelete->dwMessageID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbMessageType"),pApplyDelete->cbMessageType);									
		
		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//执行脚本
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_ApplyDelete"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS)
		{
			//构造结构
			auto pDBOApplyDelete=(DBO_GR_ApplyDelete*)m_cbBuffer;
			ZeroMemory(pDBOApplyDelete,sizeof(DBO_GR_ApplyDelete));

			//设置变量
			pDBOApplyDelete->dwUserID = pApplyDelete->dwUserID;
			pDBOApplyDelete->dwClubID = pApplyDelete->dwClubID;
			pDBOApplyDelete->dwMessageID=pApplyDelete->dwMessageID;
			pDBOApplyDelete->cbMessageType=pApplyDelete->cbMessageType;

			//构造对象
			WORD wPacketSize=sizeof(DBO_GR_ApplyDelete);
			auto pdwMessageID = (DWORD*)(pDBOApplyDelete + 1);

			//读取数据
			while (m_GameBattleDBModule->IsRecordsetEnd() == false)
			{
				//读取数据				
				*pdwMessageID++ = m_GameBattleDBAide.GetValue_DWORD(TEXT("RecordID"));
				wPacketSize += sizeof(DWORD);

				//移动记录
				m_GameBattleDBModule->MoveToNext();
			}

			//发送数据					
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_DELETE,dwContextID,dwTokenID,m_cbBuffer,wPacketSize);
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode = 1L;
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);			
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;
}

//申请撤销
bool CDataBaseEngineSink::OnRequestApplyRepeal(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize==sizeof(DBR_GR_ApplyRepeal));
	if (wDataSize!=sizeof(DBR_GR_ApplyRepeal)) return false;

	//提取数据
	auto pApplyRepeal=(DBR_GR_ApplyRepeal *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128]=TEXT("");			

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//设置参数
		m_GameBattleDBAide.AddParameter(TEXT("@dwMessageID"),pApplyRepeal->dwMessageID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbMessageStatus"),APPLY_STATUS_REPEAL);
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"),pApplyRepeal->dwUserID);
		
		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//执行脚本
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_ApplyRepeal"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS)
		{
			//获取对象
			tagClubApplyMessage ApplyMessage;
			ZeroMemory(&ApplyMessage,sizeof(ApplyMessage));	

			//读取数据
			while (m_GameBattleDBModule->IsRecordsetEnd()==false)
			{
				//读取数据				
				ApplyMessage.cbMessageStatus=APPLY_STATUS_REPEAL;
				ApplyMessage.dwMessageID=m_GameBattleDBAide.GetValue_DWORD(TEXT("RecordID"));								

				//申请消息
				ApplyMessage.dwApplyerID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerID"));
				ApplyMessage.dwApplyerGameID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerGameID"));
				ApplyMessage.wApplyerFaceID = m_GameBattleDBAide.GetValue_WORD(TEXT("ApplyerFaceID"));
				ApplyMessage.cbApplyerGender= m_GameBattleDBAide.GetValue_BYTE(TEXT("ApplyerGender"));
				ApplyMessage.dwApplyerCustomID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ApplyerCustomID"));
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("ApplyDateTime"), ApplyMessage.ApplyDateTime);
				m_GameBattleDBAide.GetValue_SystemTime(TEXT("OperateDateTime"), ApplyMessage.OperateDateTime);
				m_GameBattleDBAide.GetValue_String(TEXT("ApplyerNickName"),ApplyMessage.szApplyerNickName,CountArray(ApplyMessage.szApplyerNickName));

				//群组信息
				ApplyMessage.dwClubID=m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
				ApplyMessage.dwResponderID =m_GameBattleDBAide.GetValue_DWORD(TEXT("ResponderID"));
				m_GameBattleDBAide.GetValue_String(TEXT("ClubName"),ApplyMessage.szClubName,CountArray(ApplyMessage.szClubName));

				//获取对象			
				auto pDBApplyMessage=(DBO_GR_ApplyMessage *)m_cbBuffer;

				//拷贝数据
				pDBApplyMessage->wMessageCount=1;
				CopyMemory(pDBApplyMessage+1,&ApplyMessage,sizeof(ApplyMessage));

				///////////////////////////////////////////////////////////////////////////////////////////////////////			

				//设置变量
				pDBApplyMessage->dwUserID=ApplyMessage.dwApplyerID;
				pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_REQUEST;		
				
				//发送数据
				WORD wPacketSize=sizeof(DBO_GR_ApplyMessage)+sizeof(ApplyMessage);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,dwTokenID,pDBApplyMessage,wPacketSize);

				///////////////////////////////////////////////////////////////////////////////////////////////////////

				//设置变量
				pDBApplyMessage->dwClubID=ApplyMessage.dwClubID;
				pDBApplyMessage->cbMessageType=APPLY_MSG_TYPE_RESPOND;		
				
				//发送数据
				wPacketSize=sizeof(DBO_GR_ApplyMessage)+sizeof(ApplyMessage);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_APPLY_MESSAGE,dwContextID,dwTokenID,pDBApplyMessage,wPacketSize);

				///////////////////////////////////////////////////////////////////////////////////////////////////////

				//移动记录
				m_GameBattleDBModule->MoveToNext();
			}

			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//设置变量
			OperateSuccess.lOperateCode=0;
			if (DBVarValue.bstrVal)
			{
				StringCchCopy(OperateSuccess.szOperateNotice, CountArray(OperateSuccess.szOperateNotice), (LPCTSTR)(DBVarValue.bstrVal));
			}

			//发送数据
			WORD wPacketSize=CountStringBuffer(OperateSuccess.szOperateNotice);
			wPacketSize+=sizeof(OperateSuccess)-sizeof(OperateSuccess.szOperateNotice);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS,dwContextID,dwTokenID,&OperateSuccess,wPacketSize);
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode=lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;				
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;
}

//约战结算
bool CDataBaseEngineSink::OnRequestSettleBattle(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize==sizeof(DBR_GR_SettleBattle));
	if (wDataSize!=sizeof(DBR_GR_SettleBattle)) return false;

	//提取数据
	auto pSettleBattle=(DBR_GR_SettleBattle *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128]=TEXT("");			

		//构造参数
		m_GameBattleDBAide.ResetParameter();
			
		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwDateID"),pSettleBattle->dwDateID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"),pSettleBattle->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"),pSettleBattle->dwMemberID);							
		m_GameBattleDBAide.AddParameter(TEXT("@wSettleCount"),pSettleBattle->wSettleCount);
		m_GameBattleDBAide.AddParameter(TEXT("@dwCreaterID"),pSettleBattle->dwCreaterID);
		
		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"),szErrorDescribe,sizeof(szErrorDescribe),adParamOutput);

		//执行脚本
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_SettleBattle"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS)
		{
			//构造结构
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess,sizeof(OperateSuccess));

			//设置变量
			OperateSuccess.lOperateCode=0;
			
			//发送数据
			WORD wPacketSize=CountStringBuffer(OperateSuccess.szOperateNotice);
			wPacketSize+=sizeof(OperateSuccess)-sizeof(OperateSuccess.szOperateNotice);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS,dwContextID,dwTokenID,&OperateSuccess,wPacketSize);

		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"),DBVarValue);

			//构造结构
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure,sizeof(OperateFailure));

			//设置变量
			OperateFailure.lErrorCode=lReturnValue;
			StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure,sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode=1L;
		StringCchCopy(OperateFailure.szErrorDescribe,CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize=CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize+=sizeof(OperateFailure)-sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE,dwContextID,dwTokenID,&OperateFailure,wPacketSize);
	}

	return true;
}

//修改分数
bool CDataBaseEngineSink::OnRequestModifyScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize == sizeof(DBR_GR_ModifyScore));
	if (wDataSize != sizeof(DBR_GR_ModifyScore)) return false;

	//提取数据
	auto pModifyScore = (DBR_GR_ModifyScore *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pModifyScore->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pModifyScore->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pModifyScore->dwMemberID);
		m_GameBattleDBAide.AddParameter(TEXT("@lModifyScore"), pModifyScore->lModifyScore);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_ModifyClubScore"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_ModifyScore ModifyScore;
			ZeroMemory(&ModifyScore,sizeof(ModifyScore));

			//修改信息
			ModifyScore.lModifyScore = pModifyScore->lModifyScore;

			//读取数据
			ModifyScore.wKindID = m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
			ModifyScore.wServerID = m_GameBattleDBAide.GetValue_WORD(TEXT("ServerID"));
			ModifyScore.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
			ModifyScore.dwMemberID = m_GameBattleDBAide.GetValue_DWORD(TEXT("MemberID"));
			ModifyScore.dwPayUserID = m_GameBattleDBAide.GetValue_DWORD(TEXT("PayUserID"));			
			ModifyScore.wPayUserType= m_GameBattleDBAide.GetValue_WORD(TEXT("PayUserType"));
			ModifyScore.lMemberScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("MemberScore"));
			m_GameBattleDBAide.GetValue_String(TEXT("MemberNickName"), ModifyScore.szNickName, CountArray(ModifyScore.szNickName));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_MODIFY_SCORE, dwContextID, dwTokenID, &ModifyScore, sizeof(ModifyScore));
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
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//赠送积分
bool CDataBaseEngineSink::OnRequestPresentScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize == sizeof(DBR_GR_PresentScore));
	if (wDataSize != sizeof(DBR_GR_PresentScore)) return false;

	//提取数据
	auto pPresentScore = (DBR_GR_PresentScore *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pPresentScore->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pPresentScore->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pPresentScore->dwMemberID);
		m_GameBattleDBAide.AddParameter(TEXT("@lPresentScore"), pPresentScore->lPresentScore);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_PresentClubScore"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_PresentScore PresentScore;
			ZeroMemory(&PresentScore, sizeof(PresentScore));

			//茶馆信息
			PresentScore.dwClubID = pPresentScore->dwClubID;
			m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), PresentScore.szClubName, CountArray(PresentScore.szClubName));

			//操作成员
			PresentScore.dwSrcMemberID = pPresentScore->dwUserID;
			PresentScore.lPresentScore = pPresentScore->lPresentScore;
			PresentScore.lSrcMemberScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("SrcMemberScore"));
			m_GameBattleDBAide.GetValue_String(TEXT("SrcNickName"), PresentScore.szSrcNickName, CountArray(PresentScore.szSrcNickName));

			//目标成员
			PresentScore.dwDstMemberID = pPresentScore->dwMemberID;
			PresentScore.wKindID = m_GameBattleDBAide.GetValue_WORD(TEXT("KindID"));
			PresentScore.wServerID = m_GameBattleDBAide.GetValue_WORD(TEXT("ServerID"));			
			PresentScore.lDstMemberScore = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("DstMemberScore"));			
			m_GameBattleDBAide.GetValue_String(TEXT("DstNickName"), PresentScore.szDstNickName, CountArray(PresentScore.szDstNickName));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_PRESENT_SCORE, dwContextID, dwTokenID, &PresentScore, sizeof(PresentScore));
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
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//标记记录
bool CDataBaseEngineSink::OnRequestDeleteRecord(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize == sizeof(DBR_GR_DeleteRecord));
	if (wDataSize != sizeof(DBR_GR_DeleteRecord)) return false;

	//提取数据
	auto pDeleteRecord = (DBR_GR_DeleteRecord*)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pDeleteRecord->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pDeleteRecord->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwRecordID"), pDeleteRecord->dwRecordID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbRecordKind"), pDeleteRecord->cbRecordKind);				

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_DeleteRecord"), false);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//设置变量
			OperateSuccess.lOperateCode = 0;
			
			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateSuccess.szOperateNotice);
			wPacketSize += sizeof(OperateSuccess) - sizeof(OperateSuccess.szOperateNotice);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wPacketSize);
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
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//检测名称
bool CDataBaseEngineSink::OnRequestDetectClubName(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize == sizeof(DBR_GR_DetectClubName));
	if (wDataSize != sizeof(DBR_GR_DetectClubName)) return false;

	//提取数据
	auto pDetectClubName = (DBR_GR_DetectClubName*)pData;

	try
	{
		//构造参数
		m_GameBattleDBAide.ResetParameter();
		m_GameBattleDBAide.AddParameter(TEXT("@strClubName"), pDetectClubName->szClubName);		

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_DetectClubName"), false);

		//构造结构
		DBO_GR_DetectClubName DetectClubName = {};

		//可用标志
		DetectClubName.bUsable=(lReturnValue==DB_SUCCESS);

		//错误信息
		if (DetectClubName.bUsable == false)
		{
			StringCchCopy(DetectClubName.szErrorDescribe,CountArray(DetectClubName.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0015, TEXT("当前茶社名称已被占用！")));
		}

		//发送数据			
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_DETECT_CLUBNAME, dwContextID, dwTokenID, &DetectClubName, sizeof(DetectClubName));		

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
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//清除积分
bool CDataBaseEngineSink::OnRequestCleanupScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize == sizeof(DBR_GR_CleanupScore));
	if (wDataSize != sizeof(DBR_GR_CleanupScore)) return false;

	//提取数据
	auto pCleanupScore = (DBR_GR_CleanupScore *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pCleanupScore->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pCleanupScore->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pCleanupScore->dwMemberID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbCleanupObject"), pCleanupScore->cbCleanupObject);
		m_GameBattleDBAide.AddParameter(TEXT("@dwCreaterGameID"), pCleanupScore->dwCreaterGameID);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_CleanupScoreRecord"), false);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_CleanupScore CleanupScore;
			ZeroMemory(&CleanupScore, sizeof(CleanupScore));

			//设置变量
			CleanupScore.dwUserID = pCleanupScore->dwUserID;
			CleanupScore.dwClubID = pCleanupScore->dwClubID;
			CleanupScore.dwMemberID = pCleanupScore->dwMemberID;
			CleanupScore.cbCleanupObject = pCleanupScore->cbCleanupObject;

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLEANUP_SCORE, dwContextID, dwTokenID, &CleanupScore, sizeof(CleanupScore));				
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
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//清除税收
bool CDataBaseEngineSink::OnRequestCleanupRevenue(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize == sizeof(DBR_GR_CleanupRevenue));
	if (wDataSize != sizeof(DBR_GR_CleanupRevenue)) return false;

	//提取数据
	auto pCleanupRevenue = (DBR_GR_CleanupRevenue *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pCleanupRevenue->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pCleanupRevenue->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pCleanupRevenue->dwMemberID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbCleanupObject"), pCleanupRevenue->cbCleanupObject);
		m_GameBattleDBAide.AddParameter(TEXT("@dwCreaterGameID"), pCleanupRevenue->dwCreaterGameID);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_CleanupRevenueRecord"), false);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_CleanupRevenue CleanupRevenue;
			ZeroMemory(&CleanupRevenue, sizeof(CleanupRevenue));

			//设置变量
			CleanupRevenue.dwUserID = pCleanupRevenue->dwUserID;
			CleanupRevenue.dwClubID = pCleanupRevenue->dwClubID;
			CleanupRevenue.dwMemberID = pCleanupRevenue->dwMemberID;
			CleanupRevenue.cbCleanupObject = pCleanupRevenue->cbCleanupObject;

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLEANUP_REVENUE, dwContextID, dwTokenID, &CleanupRevenue, sizeof(CleanupRevenue));	
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
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//清理设置
bool CDataBaseEngineSink::OnRequestCleanupSetup(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize == sizeof(DBR_GR_CleanupSetup));
	if (wDataSize != sizeof(DBR_GR_CleanupSetup)) return false;

	//提取数据
	auto pCleanupSetup = (DBR_GR_CleanupSetup *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pCleanupSetup->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pCleanupSetup->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@bAutoCleanup"), pCleanupSetup->bAutoCleanup);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_CleanupSetup"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//变量定义		
			WORD wHeadSize = sizeof(DBO_GR_UpdateClub);
			DBO_GR_UpdateClub * pUpdateClub = (DBO_GR_UpdateClub*)m_cbBuffer;

			//群组标识
			pUpdateClub->cbUpdateKind = UPCLUB_KIND_RULES;
			pUpdateClub->dwClubID = pCleanupSetup->dwClubID;			

			//变量定义
			tagClubRules * pClubRules = (tagClubRules *)&m_cbBuffer[wHeadSize];

			//群组规则
			pClubRules->cbPayType = m_GameBattleDBAide.GetValue_BYTE(TEXT("PayType"));
			pClubRules->cbBattleMask = m_GameBattleDBAide.GetValue_BYTE(TEXT("BattleMask"));
			pClubRules->cbScoreKind = m_GameBattleDBAide.GetValue_BYTE(TEXT("ScoreKind"));
			pClubRules->wSwitchOption = m_GameBattleDBAide.GetValue_WORD(TEXT("SwitchOption"));
			pClubRules->cbCurrencyKind = m_GameBattleDBAide.GetValue_BYTE(TEXT("CurrencyKind"));			

			//发送数据
			WORD wPacketSize = wHeadSize + sizeof(tagClubRules);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_UPDATE_CLUB, dwContextID, dwTokenID, m_cbBuffer, wPacketSize);

			//构造结构
			DBO_GR_CleanupSetup CleanupSetup;
			ZeroMemory(&CleanupSetup, sizeof(CleanupSetup));

			//设置变量
			CleanupSetup.dwUserID = pCleanupSetup->dwUserID;
			CleanupSetup.dwClubID = pCleanupSetup->dwClubID;
			CleanupSetup.bAutoCleanup = pCleanupSetup->bAutoCleanup;

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLEANUP_SETUP, dwContextID, dwTokenID, &CleanupSetup, sizeof(CleanupSetup));
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
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//清除榜单
bool CDataBaseEngineSink::OnRequestCleanupRankList(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize == sizeof(DBR_GR_CleanupRankList));
	if (wDataSize != sizeof(DBR_GR_CleanupRankList)) return false;

	//提取数据
	auto pCleanupRankList = (DBR_GR_CleanupRankList *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pCleanupRankList->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pCleanupRankList->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pCleanupRankList->dwMemberID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbListKind"), pCleanupRankList->cbListKind);
		m_GameBattleDBAide.AddParameter(TEXT("@cbCleanupObject"), pCleanupRankList->cbCleanupObject);
		m_GameBattleDBAide.AddParameter(TEXT("@dwCreaterGameID"), pCleanupRankList->dwCreaterGameID);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_CleanupRankListRecord"), false);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_CleanupRankList CleanupRankList;
			ZeroMemory(&CleanupRankList, sizeof(CleanupRankList));

			//设置变量
			CleanupRankList.dwUserID = pCleanupRankList->dwUserID;
			CleanupRankList.dwClubID = pCleanupRankList->dwClubID;
			CleanupRankList.dwMemberID = pCleanupRankList->dwMemberID;
			CleanupRankList.cbListKind = pCleanupRankList->cbListKind;
			CleanupRankList.cbCleanupObject = pCleanupRankList->cbCleanupObject;

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLEANUP_RANKLIST, dwContextID, dwTokenID, &CleanupRankList, sizeof(CleanupRankList));
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
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//请求合并
bool CDataBaseEngineSink::OnRequestRequestMerge(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize == sizeof(DBR_GR_RequestMerge));
	if (wDataSize != sizeof(DBR_GR_RequestMerge)) return false;

	//提取数据
	auto pRequestMerge = (DBR_GR_RequestMerge *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pRequestMerge->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwRequestClubID"), pRequestMerge->dwRequestClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwRespondClubID"), pRequestMerge->dwRespondClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwCreaterGameID"), pRequestMerge->dwCreaterGameID);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_RequestMerge"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_RequestMerge RequestMerge;
			ZeroMemory(&RequestMerge, sizeof(RequestMerge));

			//用户信息
			RequestMerge.dwUserID = pRequestMerge->dwUserID;
			RequestMerge.dwRequestClubID = pRequestMerge->dwRequestClubID;			
			RequestMerge.dwRespondClubID = pRequestMerge->dwRespondClubID;

			//读取信息			
			RequestMerge.dwRecordID = m_GameBattleDBAide.GetValue_DWORD(TEXT("RecordID"));
			RequestMerge.dwRespondCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("RespondCreaterID"));			
			m_GameBattleDBAide.GetValue_String(TEXT("RequestNickName"), RequestMerge.szRequestNickName, CountArray(RequestMerge.szRequestNickName));
			m_GameBattleDBAide.GetValue_String(TEXT("RequestClubName"), RequestMerge.szRequestClubName, CountArray(RequestMerge.szRequestClubName));
			m_GameBattleDBAide.GetValue_String(TEXT("RespondClubName"), RequestMerge.szRespondClubName, CountArray(RequestMerge.szRespondClubName));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_REQUEST_MERGE, dwContextID, dwTokenID, &RequestMerge, sizeof(RequestMerge));
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
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe),(LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//响应合并
bool CDataBaseEngineSink::OnRequestRespondMerge(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize == sizeof(DBR_GR_RespondMerge));
	if (wDataSize != sizeof(DBR_GR_RespondMerge)) return false;

	//提取数据
	auto pRespondMerge = (DBR_GR_RespondMerge *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pRespondMerge->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwRecordID"), pRespondMerge->dwRecordID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbRespondCode"), pRespondMerge->cbRespondCode);		
		m_GameBattleDBAide.AddParameter(TEXT("@strAuthCodeMD5"), pRespondMerge->szAuthCodeMD5);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_RespondMerge"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_RespondMerge RespondMerge;
			ZeroMemory(&RespondMerge, sizeof(RespondMerge));

			//设置变量
			RespondMerge.dwUserID = pRespondMerge->dwUserID;
			RespondMerge.dwRecordID = pRespondMerge->dwRecordID;
			RespondMerge.cbRespondCode = pRespondMerge->cbRespondCode;

			//设置变量			
			RespondMerge.cbRespondCode = m_GameBattleDBAide.GetValue_BYTE(TEXT("RespondCode"));
			RespondMerge.dwRequestClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("RequestClubID"));
			RespondMerge.dwRequestCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("RequestCreaterID"));
			RespondMerge.dwRespondClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("RespondClubID"));
			RespondMerge.dwRespondCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("RespondCreaterID"));
			m_GameBattleDBAide.GetValue_String(TEXT("RespondClubName"), RespondMerge.szRespondClubName, CountArray(RespondMerge.szRespondClubName));

			//同意合并
			if (RespondMerge.cbRespondCode == RESPOND_CODE_AGREE)
			{
				//构造参数
				m_GameBattleDBAide.ResetParameter();

				//用户信息
				m_GameBattleDBAide.AddParameter(TEXT("@dwRecordID"), pRespondMerge->dwRecordID);

				//输出参数
				m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

				//执行脚本
				lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_MergeClub"), false);

				//执行成功
				if (lReturnValue == DB_SUCCESS)
				{
					//发送数据		
					WORD wPacketSize = CountStringBuffer(RespondMerge.szErrorDescribe);
					wPacketSize += sizeof(RespondMerge) - sizeof(RespondMerge.szErrorDescribe);
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RESPOND_MERGE, dwContextID, dwTokenID, &RespondMerge, wPacketSize);
				}
				else
				{
					//获取参数
					CDBVarValue DBVarValue;
					m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

					//设置变量
					RespondMerge.lErrorCode = lReturnValue;
					StringCchCopy(RespondMerge.szErrorDescribe, CountArray(RespondMerge.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

					//发送数据		
					WORD wPacketSize = CountStringBuffer(RespondMerge.szErrorDescribe);
					wPacketSize += sizeof(RespondMerge) - sizeof(RespondMerge.szErrorDescribe);
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RESPOND_MERGE, dwContextID, dwTokenID, &RespondMerge, wPacketSize);
				}
			}
			else
			{
				//发送数据		
				WORD wPacketSize = CountStringBuffer(RespondMerge.szErrorDescribe);
				wPacketSize += sizeof(RespondMerge) - sizeof(RespondMerge.szErrorDescribe);
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RESPOND_MERGE, dwContextID, dwTokenID, &RespondMerge, wPacketSize);
			}
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_RespondMerge RespondMerge;
			ZeroMemory(&RespondMerge, sizeof(RespondMerge));

			//设置变量	
			RespondMerge.dwUserID = pRespondMerge->dwUserID;
			RespondMerge.dwRecordID = pRespondMerge->dwRecordID;
			RespondMerge.cbRespondCode = pRespondMerge->cbRespondCode;

			//设置变量
			RespondMerge.lErrorCode = lReturnValue;
			StringCchCopy(RespondMerge.szErrorDescribe, CountArray(RespondMerge.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据		
			WORD wPacketSize = CountStringBuffer(RespondMerge.szErrorDescribe);
			wPacketSize += sizeof(RespondMerge) - sizeof(RespondMerge.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RESPOND_MERGE, dwContextID, dwTokenID, &RespondMerge, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_RespondMerge RespondMerge;
		ZeroMemory(&RespondMerge, sizeof(RespondMerge));

		//设置变量	
		RespondMerge.dwUserID = pRespondMerge->dwUserID;
		RespondMerge.dwRecordID = pRespondMerge->dwRecordID;
		RespondMerge.cbRespondCode = pRespondMerge->cbRespondCode;

		//设置变量
		RespondMerge.lErrorCode = 1;
		StringCchCopy(RespondMerge.szErrorDescribe, CountArray(RespondMerge.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据		
		WORD wPacketSize = CountStringBuffer(RespondMerge.szErrorDescribe);
		wPacketSize += sizeof(RespondMerge) - sizeof(RespondMerge.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RESPOND_MERGE, dwContextID, dwTokenID, &RespondMerge, wPacketSize);
	}

	return true;
}

//取消代付
bool CDataBaseEngineSink::OnRequestCancelPayroll(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT(wDataSize == sizeof(DBR_GR_CancelPayroll));
	if (wDataSize != sizeof(DBR_GR_CancelPayroll)) return false;

	//提取数据
	auto pCancelPayroll = (DBR_GR_CancelPayroll *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pCancelPayroll->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pCancelPayroll->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pCancelPayroll->dwMemberID);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_CancelPayroll"), false);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_CancelPayroll CancelPayroll;
			ZeroMemory(&CancelPayroll, sizeof(CancelPayroll));

			//用户信息
			CancelPayroll.dwUserID = pCancelPayroll->dwUserID;
			CancelPayroll.dwClubID = pCancelPayroll->dwClubID;
			CancelPayroll.dwMemberID = pCancelPayroll->dwMemberID;			

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CANCEL_PAYROLL, dwContextID, dwTokenID, &CancelPayroll, sizeof(CancelPayroll));
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
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//请求代付
bool CDataBaseEngineSink::OnRequestRequestPayroll(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize == sizeof(DBR_GR_RequestPayroll));
	if (wDataSize != sizeof(DBR_GR_RequestPayroll)) return false;

	//提取数据
	auto pRequestPayroll = (DBR_GR_RequestPayroll *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pRequestPayroll->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwClubID"), pRequestPayroll->dwClubID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwMemberID"), pRequestPayroll->dwMemberID);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_RequestPayroll"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_RequestPayroll RequestPayroll;
			ZeroMemory(&RequestPayroll, sizeof(RequestPayroll));

			//设置变量
			RequestPayroll.dwUserID = pRequestPayroll->dwUserID;
			RequestPayroll.dwClubID = pRequestPayroll->dwClubID;
			RequestPayroll.dwMemberID = pRequestPayroll->dwMemberID;
			RequestPayroll.dwRecordID = m_GameBattleDBAide.GetValue_DWORD(TEXT("RecordID"));
			m_GameBattleDBAide.GetValue_String(TEXT("NickName"), RequestPayroll.szNickName, CountArray(RequestPayroll.szNickName));
			m_GameBattleDBAide.GetValue_String(TEXT("ClubName"), RequestPayroll.szClubName, CountArray(RequestPayroll.szClubName));

			//发送数据
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_REQUEST_PAYROLL, dwContextID, dwTokenID, &RequestPayroll, sizeof(RequestPayroll));
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
			StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据
			WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
			wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//设置变量
		OperateFailure.lErrorCode = 1L;
		StringCchCopy(OperateFailure.szErrorDescribe, CountArray(OperateFailure.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据
		WORD wPacketSize = CountStringBuffer(OperateFailure.szErrorDescribe);
		wPacketSize += sizeof(OperateFailure) - sizeof(OperateFailure.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
	}

	return true;
}

//响应代付
bool CDataBaseEngineSink::OnRequestRespondPayroll(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	//参数校验
	ASSERT (wDataSize == sizeof(DBR_GR_RespondPayroll));
	if (wDataSize != sizeof(DBR_GR_RespondPayroll)) return false;

	//提取数据
	auto pRespondPayroll = (DBR_GR_RespondPayroll *)pData;

	try
	{
		//变量定义
		WCHAR szErrorDescribe[128] = TEXT("");

		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//用户信息
		m_GameBattleDBAide.AddParameter(TEXT("@dwUserID"), pRespondPayroll->dwUserID);
		m_GameBattleDBAide.AddParameter(TEXT("@dwRecordID"), pRespondPayroll->dwRecordID);
		m_GameBattleDBAide.AddParameter(TEXT("@cbRespondCode"), pRespondPayroll->cbRespondCode);

		//输出参数
		m_GameBattleDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szErrorDescribe, sizeof(szErrorDescribe), adParamOutput);

		//执行脚本
		LONG lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_RespondPayroll"), true);

		//执行成功
		if (lReturnValue == DB_SUCCESS)
		{
			//构造结构
			DBO_GR_RespondPayroll RespondPayroll;
			ZeroMemory(&RespondPayroll, sizeof(RespondPayroll));

			//设置变量
			RespondPayroll.dwUserID = pRespondPayroll->dwUserID;
			RespondPayroll.dwRecordID = pRespondPayroll->dwRecordID;
			RespondPayroll.cbRespondCode = pRespondPayroll->cbRespondCode;

			//设置变量			
			RespondPayroll.dwClubID = m_GameBattleDBAide.GetValue_DWORD(TEXT("ClubID"));
			RespondPayroll.dwCreaterID = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreaterID"));

			//发送数据		
			WORD wPacketSize = CountStringBuffer(RespondPayroll.szErrorDescribe);
			wPacketSize += sizeof(RespondPayroll) - sizeof(RespondPayroll.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RESPOND_PAYROLL, dwContextID, dwTokenID, &RespondPayroll, wPacketSize);
		}
		else
		{
			//获取参数
			CDBVarValue DBVarValue;
			m_GameBattleDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造结构
			DBO_GR_RespondPayroll RespondPayroll;
			ZeroMemory(&RespondPayroll, sizeof(RespondPayroll));

			//设置变量
			RespondPayroll.dwUserID = pRespondPayroll->dwUserID;
			RespondPayroll.dwRecordID = pRespondPayroll->dwRecordID;
			RespondPayroll.cbRespondCode = pRespondPayroll->cbRespondCode;

			//设置变量
			RespondPayroll.lErrorCode = lReturnValue;
			StringCchCopy(RespondPayroll.szErrorDescribe, CountArray(RespondPayroll.szErrorDescribe), (LPCTSTR)(DBVarValue.bstrVal));

			//发送数据		
			WORD wPacketSize = CountStringBuffer(RespondPayroll.szErrorDescribe);
			wPacketSize += sizeof(RespondPayroll) - sizeof(RespondPayroll.szErrorDescribe);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RESPOND_PAYROLL, dwContextID, dwTokenID, &RespondPayroll, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe = pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结构
		DBO_GR_RespondPayroll RespondPayroll;
		ZeroMemory(&RespondPayroll, sizeof(RespondPayroll));

		//设置变量
		RespondPayroll.dwUserID = pRespondPayroll->dwUserID;
		RespondPayroll.dwRecordID = pRespondPayroll->dwRecordID;
		RespondPayroll.cbRespondCode = pRespondPayroll->cbRespondCode;

		//设置变量
		RespondPayroll.lErrorCode = 1;
		StringCchCopy(RespondPayroll.szErrorDescribe, CountArray(RespondPayroll.szErrorDescribe), CWHService::FormatMessage(MIM_CLUBPLAZA_0009, TEXT("由于数据库操作异常，请稍后重试！")));

		//发送数据		
		WORD wPacketSize = CountStringBuffer(RespondPayroll.szErrorDescribe);
		wPacketSize += sizeof(RespondPayroll) - sizeof(RespondPayroll.szErrorDescribe);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_RESPOND_PAYROLL, dwContextID, dwTokenID, &RespondPayroll, wPacketSize);
	}

	return true;
}

//加载配置
bool CDataBaseEngineSink::OnRequestLoadClubOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	try
	{
		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//执行脚本
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadClubOption"),true);

		//执行成功
		if (lReturnValue==DB_SUCCESS)
		{
			//获取对象								
			tagClubOption ClubOption = {};

			//读取配置
			ClubOption.wSupportClubKind = m_GameBattleDBAide.GetValue_WORD(TEXT("SupportClubKind"));
			ClubOption.wMaxJoinClubCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxJoinClubCount"));
			ClubOption.wMaxCreateClubCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxCreateClubCount"));
			ClubOption.wMaxBattleConfigCount = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxBattleConfigCount"));			

			//加载类型选项
			for (BYTE cbClubKind = 0; cbClubKind < CountArray(ClubOption.ClubKindOption); cbClubKind++)
			{

				//构造参数
				m_GameBattleDBAide.ResetParameter();
				m_GameBattleDBAide.AddParameter(TEXT("@wClubKind"), cbClubKind);
				lReturnValue = m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadClubKindOption"), true);

				//执行成功
				if (lReturnValue == DB_SUCCESS)
				{
					//获取对象								
					auto pClubKindOption = &ClubOption.ClubKindOption[cbClubKind];

					//读取数据		
					pClubKindOption->cbClubKind = cbClubKind;
					pClubKindOption->cbPayTypeMask = m_GameBattleDBAide.GetValue_BYTE(TEXT("PayTypeMask"));
					pClubKindOption->cbCurrencyKindMask = m_GameBattleDBAide.GetValue_BYTE(TEXT("CurrencyKindMask"));
					pClubKindOption->wMinRevenueRate = m_GameBattleDBAide.GetValue_WORD(TEXT("MinRevenueRate"));
					pClubKindOption->wMaxRevenueRate = m_GameBattleDBAide.GetValue_WORD(TEXT("MaxRevenueRate"));
					pClubKindOption->dwNeedUserRight = m_GameBattleDBAide.GetValue_DWORD(TEXT("CreateNeedUserRight"));
					pClubKindOption->lCreateClubTakeCard = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("CreateClubTakeCard"));
					pClubKindOption->lCreateClubDeductCard = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("CreateClubDeductCard"));					
					pClubKindOption->lModifyClubNameDeductCard = m_GameBattleDBAide.GetValue_LONGLONG(TEXT("ModifyClubNameDeductCard"));					
				}
			}

			//茶社配置
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLUB_OPTION, dwContextID, dwTokenID, &ClubOption, sizeof(ClubOption));			
		}		

		//发送数据
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPTION_FINISH, dwContextID, dwTokenID, NULL, 0);

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		//输出错误
		LPCTSTR pszDescribe=pIException->GetExceptionDescribe();
		TraceDatabaseException(__FILE__,__FUNCTION__,__LINE__,pIException);		
	}

	return true;
}

//加载配置
bool CDataBaseEngineSink::OnRequestLoadForbidWords(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize)
{
	try
	{
		//构造参数
		m_GameBattleDBAide.ResetParameter();

		//执行查询
		LONG lReturnValue=m_GameBattleDBAide.ExecuteProcess(TEXT("GSP_GR_LoadClubForbidWords"),true);

		//读取信息
		if (lReturnValue==DB_SUCCESS)
		{
			//定义缓冲
			WORD wPacketSize=0;
			WORD wStringCount=0;
			BYTE cbBuffer[SOCKET_PACKET];
			TCHAR szForbidWords[32]=TEXT("");

			//起始消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FIRBID_WORDS,0xFFFE,dwTokenID,NULL,0);

			//读取消息
			while (m_GameBattleDBModule->IsRecordsetEnd()==false)
			{
				//读取消息
				m_GameBattleDBAide.GetValue_String(TEXT("ForbidWords"),szForbidWords,32);

				//设置长度
				wStringCount=CountStringBuffer(szForbidWords);

				//发送数据
				if (wPacketSize+wStringCount>=sizeof(cbBuffer))
				{
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FIRBID_WORDS,0,dwTokenID,cbBuffer,wPacketSize);
					wPacketSize=0;
				}

				//设置长度
				*(BYTE *)&cbBuffer[wPacketSize]=wStringCount;				

				//拷贝数据
				wPacketSize++;
				CopyMemory(&cbBuffer[wPacketSize],szForbidWords,CountStringBuffer(szForbidWords));

				//设置长度
				wPacketSize+=wStringCount;				

				//移动记录
				m_GameBattleDBModule->MoveToNext();
			};

			//发送消息
			if (wPacketSize>0)
			{
				m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FIRBID_WORDS,0,dwTokenID,cbBuffer,wPacketSize);
			}

			//结束消息
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_FIRBID_WORDS,0xFFFF,dwTokenID,NULL,0);
		}

		return true;
	}
	catch (IDataBaseException * pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		return false;
	}

	return true;
}

//提取群组
WORD CDataBaseEngineSink::ExtractClubID(LPCTSTR pszClubIDArray, CDWordArray & ClubIDArray)
{
	//重置参数
	ClubIDArray.RemoveAll();

	//变量定义	
	CString strClubIDArray(pszClubIDArray);

	//变量定义
	INT nFirstPos=0;
	INT nSecondPos=0;
	DWORD dwClubID=0;
	CString strClubID;

	do
	{
		//获取位置
		nFirstPos=strClubIDArray.Find(';',nSecondPos);
		if (nFirstPos==-1) break;

		//获取位置
		nSecondPos=strClubIDArray.Find(';',nFirstPos+1);
		if (nSecondPos==-1) break;

		//变量定义
		strClubID=strClubIDArray.Mid(nFirstPos+1,nSecondPos-nFirstPos-1);
		dwClubID=_tcstoul(strClubID,NULL,0);

		//添加群组
		ClubIDArray.Add(dwClubID);

	} while (true);

	return (WORD)ClubIDArray.GetCount();
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

