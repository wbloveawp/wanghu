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
	if ((m_AccountsDBModule.GetInterface()==NULL)&&(m_AccountsDBModule.CreateInstance()==false))
	{
		ASSERT(FALSE);
		return false;
	}

	//创建对象
	if ((m_PlatformDBModule.GetInterface() == NULL) && (m_PlatformDBModule.CreateInstance() == false))
	{
		ASSERT(FALSE);
		return false;
	}


	try
	{
		//变量定义
		tagDataBaseParameter * pAccountsDBParameter =&m_pInitParameter->m_AccountsDBParameter;
		tagDataBaseParameter * pPlatformDBParameter = &m_pInitParameter->m_PlatformDBParameter;

		//设置连接
		m_AccountsDBModule->SetConnectionInfo(pAccountsDBParameter->dwDataBaseAddr, pAccountsDBParameter->wDataBasePort,
		CW2CT(pAccountsDBParameter->szDataBaseName),CW2CT(pAccountsDBParameter->szDataBaseUser),CW2CT(pAccountsDBParameter->szDataBasePass));

		//设置连接
		m_PlatformDBModule->SetConnectionInfo(pPlatformDBParameter->dwDataBaseAddr, pPlatformDBParameter->wDataBasePort,
			CW2CT(pPlatformDBParameter->szDataBaseName), CW2CT(pPlatformDBParameter->szDataBaseUser), CW2CT(pPlatformDBParameter->szDataBasePass));
	
		//发起连接
		m_AccountsDBModule->OpenConnection();
		m_AccountsDBAide.SetDataBase(m_AccountsDBModule.GetInterface());	

		//发起连接
		m_PlatformDBModule->OpenConnection();
		m_PlatformDBAide.SetDataBase(m_PlatformDBModule.GetInterface());

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
	m_AccountsDBAide.SetDataBase(NULL);
	m_PlatformDBAide.SetDataBase(NULL);

	//关闭连接
	if (m_AccountsDBModule.GetInterface()!=NULL)
	{
		m_AccountsDBModule->CloseConnection();
		m_AccountsDBModule.CloseInstance();
	}

	//关闭连接
	if (m_PlatformDBModule.GetInterface() != NULL)
	{
		m_PlatformDBModule->CloseConnection();
		m_PlatformDBModule.CloseInstance();
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
	case DBR_GR_REQUEST_WEBPAGE:		//请求网页
	{
		return OnRequestRequestWebPage(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_USER_QUERY_MAIL:		//查询邮件
	{
		return OnRequestUserQueryMail(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_USER_INSERT_MAIL:		//添加邮件
	{
		return OnRequestUserInsertMail(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_USER_MODIFY_MAIL:		//修改邮件
	{
		return OnRequestUserModifyMail(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_CLAIM_MAIL_ATTACH:		//领取附件
	{
		return OnRequestClaimMailAttach(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	case DBR_GR_USER_DELETE_MAIL:		//删除邮件
	{
		return OnRequestUserDeleteMail(dwContextID, dwTokenID, pData, wDataSize, pdwControlCode);
	}
	}

	return false;
}

//请求网页
bool CDataBaseEngineSink::OnRequestRequestWebPage(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	//提取数据
	ASSERT (wDataSize <= sizeof(DBR_GR_RequestWebPage));
	if (wDataSize > sizeof(DBR_GR_RequestWebPage)) return false;

	//提取数据
	DBR_GR_RequestWebPage * pRequestWebPage = (DBR_GR_RequestWebPage *)pData;

	//请求网页
	CStringA strResult = m_WininetHttp.RequestWebPage(pRequestWebPage->szWebUrl, pRequestWebPage->hrType, pRequestWebPage->szHeader, pRequestWebPage->szPostData);

	//请求成功
	if (m_WininetHttp.GetLastError() == Hi_Success)
	{
		//转换编码
		TCHAR szResult[256] = { 0 };
		CWHService::Utf82Unicode(strResult, szResult, CountArray(szResult));		

		//读取信息
		auto JsonValuePtr = CWHJson::getValue(string(CT2CA(szResult)));
		if (JsonValuePtr->getType() != eJsonTypeObj)
		{
			//变量定义
			TCHAR szMessage[256];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("网页请求错误[未知错误]：%hs"), pRequestWebPage->szWebUrl);

			//输出错误
			CTraceService::TraceString(szMessage, TraceLevel_Exception);

			return true;
		}

		//转换对象
		LONG lResultCode = 0;
		auto JsonValueObjPtr = CJsonValueObjPtr::dynamicCast(JsonValuePtr);

		//读取结果		
		CWHJsonInput::readJson(lResultCode, JsonValueObjPtr->value["code"], false);

		//输出错误
		CTraceService::TraceString(szResult, TraceLevel_Exception);

		//校验结果
		if (lResultCode != 200)
		{
			//变量定义
			TCHAR szMessage[512];
			_sntprintf_s(szMessage, CountArray(szMessage), TEXT("网页请求错误[%s]：%hs"), szResult, pRequestWebPage->szWebUrl);

			//输出错误
			CTraceService::TraceString(szMessage, TraceLevel_Exception);

			return true;
		}
	}
	else
	{
		//变量定义
		TCHAR szMessage[256];
		_sntprintf_s(szMessage, CountArray(szMessage), TEXT("网页请求错误[%d]：%hs"), m_WininetHttp.GetLastError(), pRequestWebPage->szWebUrl);

		//输出错误
		CTraceService::TraceString(szMessage, TraceLevel_Exception);
	}

	return true;
}

//查询邮件
bool CDataBaseEngineSink::OnRequestUserQueryMail(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//效验参数
		ASSERT(wDataSize == sizeof(DBR_GR_UserQueryMail));
		if (wDataSize != sizeof(DBR_GR_UserQueryMail)) return false;

		//请求处理
		DBR_GR_UserQueryMail* pUserQueryMail = (DBR_GR_UserQueryMail*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUserQueryMail->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwMailID"), pUserQueryMail->dwLatestMailID);
		m_AccountsDBAide.AddParameter(TEXT("@@dwCurrentTime"), (DWORD)time(NULL));

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserQueryMail"), true);

		//用户信息
		if (lResultCode == DB_SUCCESS)
		{
			//定义变量
			DBO_GR_UserMailInfo UserMailInfo;
			ZeroMemory(&UserMailInfo, sizeof(UserMailInfo));

			//遍历数据
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				if (UserMailInfo.wMailCount >= CountArray(UserMailInfo.MailInfo))
				{
					//发送数据
					WORD wHeadSize = sizeof(UserMailInfo) - sizeof(UserMailInfo.MailInfo);
					WORD wDataSize = UserMailInfo.wMailCount * sizeof(UserMailInfo.MailInfo[0]);
					m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_MAIL_INFO, dwContextID, dwTokenID, &UserMailInfo, wHeadSize + wDataSize);

					//重置数量
					UserMailInfo.wMailCount = 0;
				}

				//邮件信息
				tagMailInfo* pMailInfo = &UserMailInfo.MailInfo[UserMailInfo.wMailCount++];

				//读取数据
				pMailInfo->dwMailID = m_AccountsDBAide.GetValue_DWORD(TEXT("MailID"));
				pMailInfo->cbMailKind = m_AccountsDBAide.GetValue_BYTE(TEXT("MailKind"));
				pMailInfo->cbMailType = m_AccountsDBAide.GetValue_BYTE(TEXT("MailType"));
				pMailInfo->cbMailState = m_AccountsDBAide.GetValue_BYTE(TEXT("MailState"));
				pMailInfo->dwSendTime = m_AccountsDBAide.GetValue_DWORD(TEXT("SendTime"));
				pMailInfo->dwExpireTime = m_AccountsDBAide.GetValue_DWORD(TEXT("ExpireTime"));
				m_AccountsDBAide.GetValue_String(TEXT("MailTitle"), pMailInfo->szMailTitle, CountArray(pMailInfo->szMailTitle));
				m_AccountsDBAide.GetValue_String(TEXT("MailContent"), pMailInfo->szMailContent, CountArray(pMailInfo->szMailContent));
				m_AccountsDBAide.GetValue_String(TEXT("AttachGoods"), pMailInfo->szAttachGoods, CountArray(pMailInfo->szAttachGoods));

				//移动记录
				m_AccountsDBModule->MoveToNext();
			}

			//发送数据
			WORD wHeadSize = sizeof(UserMailInfo) - sizeof(UserMailInfo.MailInfo);
			WORD wDataSize = UserMailInfo.wMailCount * sizeof(UserMailInfo.MailInfo[0]);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_MAIL_INFO, dwContextID, dwTokenID, &UserMailInfo, wHeadSize + wDataSize);
		}

		//邮件完成
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_MAIL_FINISH, dwContextID, dwTokenID, NULL, NULL);

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结果
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//结果代码
		//OperateFailure.wRequestCmdID = SUB_MP_USER_QUERY_MAIL;
		OperateFailure.lErrorCode = DB_ERROR;
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("由于数据库操作异常，请您稍后重试！"));

		//发送数据
		WORD wDataSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wHeadSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wHeadSize + wDataSize);

		return false;
	}

	return true;
}

//修改邮件
bool CDataBaseEngineSink::OnRequestUserModifyMail(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//校验参数
		ASSERT(wDataSize == sizeof(DBR_GR_UserModifyMail));
		if (wDataSize != sizeof(DBR_GR_UserModifyMail)) return false;

		//变量定义
		DBR_GR_UserModifyMail* pUserModifyMail = (DBR_GR_UserModifyMail*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pUserModifyMail->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwMailID"), pUserModifyMail->dwMailID);
		m_AccountsDBAide.AddParameter(TEXT("@cbMailState"), pUserModifyMail->cbMailState);

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserModifyMail"), false);

		//邮件信息
		if (lResultCode == DB_SUCCESS)
		{
			//定义变量
			DBO_GR_UserMailUpdate UserMailUpdate;
			ZeroMemory(&UserMailUpdate, sizeof(UserMailUpdate));

			//设置变量
			UserMailUpdate.dwMailID = pUserModifyMail->dwMailID;
			UserMailUpdate.cbMailState = MAIL_STATE_HAVETOSEE;

			//发送数据			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_MAIL_UPDATE, dwContextID, dwTokenID, &UserMailUpdate, sizeof(UserMailUpdate));
		}

		return true;

	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结果
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//结果代码
		//OperateFailure.wRequestCmdID = SUB_MR_USER_VIEW_MAIL;
		OperateFailure.lErrorCode = DB_ERROR;
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("由于数据库操作异常，请您稍后重试！"));

		//发送数据
		WORD wDataSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wHeadSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wHeadSize + wDataSize);
	}

	return true;
}

//插入邮件
bool CDataBaseEngineSink::OnRequestUserInsertMail(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//参数校验
		ASSERT(wDataSize <= sizeof(DBR_GR_UserInsertMail));
		if (wDataSize > sizeof(DBR_GR_UserInsertMail)) return false;

		//变量定义
		DBR_GR_UserInsertMail* pUserInsertMail = (DBR_GR_UserInsertMail*)pData;

		//校验玩家
		if (pUserInsertMail->dwUserID != 0)
		{
			CString strReceiverID;
			strReceiverID.Format(TEXT("[%d]"), pUserInsertMail->dwUserID);

			//构造参数
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@dwMailID"), pUserInsertMail->dwMailID);
			m_AccountsDBAide.AddParameter(TEXT("@cbMailState"), MAIL_STATE_UNTREATED);
			m_AccountsDBAide.AddParameter(TEXT("@dwExpireTime"), time(NULL)+ ONE_MONTH);
			m_AccountsDBAide.AddParameter(TEXT("@szReceiverID"), strReceiverID);

			//执行查询
			LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_InsertMailBox"), false);
		}
		else
		{
			//构造参数
			m_AccountsDBAide.ResetParameter();
			m_AccountsDBAide.AddParameter(TEXT("@dwMailID"), pUserInsertMail->dwMailID);
			m_AccountsDBAide.AddParameter(TEXT("@LastLogonTime"), pUserInsertMail->LastLogonTime);			

			//执行查询
			LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_CollectMailUser"), false);

			//执行成功
			if (lResultCode == DB_SUCCESS)
			{
				do
				{
					//构造参数
					m_AccountsDBAide.ResetParameter();
					m_AccountsDBAide.AddParameter(TEXT("@dwMailID"), pUserInsertMail->dwMailID);
					m_AccountsDBAide.AddParameter(TEXT("@cbMailState"), MAIL_STATE_UNTREATED);
					m_AccountsDBAide.AddParameter(TEXT("@dwExpireTime"), time(NULL) + ONE_MONTH);
					m_AccountsDBAide.AddParameter(TEXT("@wUserCount"), 5000);

					//执行查询
					lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_BatchInsertMail"), true);
					if (lResultCode == DB_SUCCESS)
					{
						//剩余人数
						DWORD dwResidueCount = m_AccountsDBAide.GetValue_DWORD(TEXT("ResidueCount"));
						if (dwResidueCount == 0) break;
					}

					//线程睡眠
					Sleep(32);

				} while (true);
			}
		}

		//构造结果
		DBO_GR_UserMailInsert  UserInsertMail;
		ZeroMemory(&UserInsertMail, sizeof(UserInsertMail));

		//设置变量
		UserInsertMail.dwMailID = pUserInsertMail->dwMailID;
		UserInsertMail.dwUserID = pUserInsertMail->dwUserID;

		//发送数据			
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_MAIL_INSERT, dwContextID, dwTokenID, &UserInsertMail, sizeof(UserInsertMail));
		
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

//领取附件
bool CDataBaseEngineSink::OnRequestClaimMailAttach(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//校验参数
		ASSERT(wDataSize == sizeof(DBR_GR_ClaimMailAttach));
		if (wDataSize != sizeof(DBR_GR_ClaimMailAttach)) return false;

		//变量定义
		DBR_GR_ClaimMailAttach* pClaimMailAttach = (DBR_GR_ClaimMailAttach*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pClaimMailAttach->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwMailID"), pClaimMailAttach->dwMailID);
		m_AccountsDBAide.AddParameter(TEXT("@dwTimeStamp"), (DWORD)time(NULL));
		m_AccountsDBAide.AddParameter(TEXT("@wReason"), GOODS_REASON_CLAIM_MAIL_ATTACH);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString));

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_ClaimMailAttach"), true);

		//邮件信息
		if (lResultCode == DB_SUCCESS)
		{
			//定义变量
			DBO_GR_UserMailUpdate UserMailUpdate;
			ZeroMemory(&UserMailUpdate, sizeof(UserMailUpdate));

			//设置变量
			UserMailUpdate.cbMailState = MAIL_STATE_HAVECLAIM;
			UserMailUpdate.dwMailID = pClaimMailAttach->dwMailID;

			//发送数据			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_MAIL_UPDATE, dwContextID, dwTokenID, &UserMailUpdate, sizeof(UserMailUpdate));

			//定义变量
			DBO_GR_ClaimMailAttach ClaimMailAttach;
			ZeroMemory(&ClaimMailAttach, sizeof(ClaimMailAttach));

			//设置变量
			ClaimMailAttach.lResultCode = lResultCode;
			ClaimMailAttach.dwMailID = pClaimMailAttach->dwMailID;
			m_AccountsDBAide.GetValue_String(TEXT("AttachGoods"), ClaimMailAttach.szAttachGoods, CountArray(ClaimMailAttach.szAttachGoods));

			//解析物品
			CGoodsItemArray MoneyGoodsList;
			CGoodsItemArray NormalGoodsList;

			//解析物品
			if (g_GameGoodsManager->ParseFromJson(ClaimMailAttach.szAttachGoods, MoneyGoodsList, NormalGoodsList))
			{
				//货币物品
				if (MoneyGoodsList.GetCount() != 0)
				{
					//更新财富
					UpdateUserWealth(pClaimMailAttach->dwUserID, dwContextID, dwTokenID);
				}

				//查询物品
				if (NormalGoodsList.GetCount() != 0)
				{
					UpdateUserGoods(pClaimMailAttach->dwUserID, dwContextID, dwTokenID, NormalGoodsList.GetData(), (WORD)NormalGoodsList.GetCount());
				}
			}

			//发送数据			
			WORD wHeadSize = sizeof(ClaimMailAttach) - sizeof(ClaimMailAttach.szAttachGoods);
			WORD wDataSize = CountStringBuffer(ClaimMailAttach.szAttachGoods);
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_CLAIM_MAIL_ATTACH, dwContextID, dwTokenID, &ClaimMailAttach, wHeadSize + wDataSize);
		}
		else
		{
			//变量定义
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.lErrorCode = lResultCode;
			OperateFailure.wRequestCmdID = SUB_AN_C_CLAIM_MAIL_ATTACH;
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), (CW2CT)DBVarValue.bstrVal);

			//调整错误码
			if (lResultCode == 4)
			{
				OperateFailure.lErrorCode = OFC_MAIL_NOTEXIST;
			}

			//发送结果
			WORD wStringSize = CountStringBufferW(OperateFailure.szDescribeString);
			WORD wPacketSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结果
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//结果代码
		OperateFailure.lErrorCode = OFC_DB_EXCEPTION;
		OperateFailure.wRequestCmdID = SUB_AN_C_CLAIM_MAIL_ATTACH;
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("由于数据库操作异常，请您稍后重试！"));

		//发送数据
		WORD wDataSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wHeadSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wHeadSize + wDataSize);
	}

	return true;
}

//删除邮件
bool CDataBaseEngineSink::OnRequestUserDeleteMail(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode)
{
	try
	{
		//校验参数
		ASSERT(wDataSize == sizeof(DBR_GR_UserDeleteMail));
		if (wDataSize != sizeof(DBR_GR_UserDeleteMail)) return false;

		//变量定义
		DBR_GR_UserDeleteMail* pDeleteMail = (DBR_GR_UserDeleteMail*)pData;

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), pDeleteMail->dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@dwMailID"), pDeleteMail->dwMailID);

		//输出参数
		TCHAR szDescribeString[128] = TEXT("");
		m_AccountsDBAide.AddParameterOutput(TEXT("@strErrorDescribe"), szDescribeString, sizeof(szDescribeString));

		//执行查询
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserDeleteMail"), true);

		//邮件信息
		if (lResultCode == DB_SUCCESS)
		{

			//定义变量
			DBO_GR_UserMailUpdate UserMailUpdate;
			ZeroMemory(&UserMailUpdate, sizeof(UserMailUpdate));

			//设置变量
			UserMailUpdate.cbMailState = MAIL_STATE_ONDELETED;
			UserMailUpdate.dwMailID = pDeleteMail->dwMailID;

			//发送数据			
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_MAIL_UPDATE, dwContextID, dwTokenID, &UserMailUpdate, sizeof(UserMailUpdate));

			//定义变量
			DBO_GR_OperateSuccess OperateSuccess;
			ZeroMemory(&OperateSuccess, sizeof(OperateSuccess));

			//构造变量
			OperateSuccess.lErrorCode = 0;
			OperateSuccess.wRequestCmdID = SUB_AN_C_USER_DELETE_MAIL;

			//发送结果
			WORD wStringSize = CountStringBuffer(OperateSuccess.szDescribeString);
			WORD wPacketSize = sizeof(OperateSuccess) - sizeof(OperateSuccess.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_SUCCESS, dwContextID, dwTokenID, &OperateSuccess, wPacketSize);
		}
		else
		{
			//变量定义
			DBO_GR_OperateFailure OperateFailure;
			ZeroMemory(&OperateFailure, sizeof(OperateFailure));

			//获取信息
			CDBVarValue DBVarValue;
			m_AccountsDBModule->GetParameter(TEXT("@strErrorDescribe"), DBVarValue);

			//构造变量
			OperateFailure.lErrorCode = lResultCode;
			OperateFailure.wRequestCmdID = SUB_AN_C_USER_DELETE_MAIL;
			StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), (CW2CT)DBVarValue.bstrVal);

			//发送结果
			WORD wStringSize = CountStringBufferW(OperateFailure.szDescribeString);
			WORD wPacketSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString) + wStringSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);

		//构造结果
		DBO_GR_OperateFailure OperateFailure;
		ZeroMemory(&OperateFailure, sizeof(OperateFailure));

		//结果代码
		OperateFailure.lErrorCode = OFC_DB_EXCEPTION;
		OperateFailure.wRequestCmdID = SUB_AN_C_USER_DELETE_MAIL;
		StringCchCopy(OperateFailure.szDescribeString, CountArray(OperateFailure.szDescribeString), TEXT("由于数据库操作异常，请您稍后重试！"));

		//发送数据
		WORD wDataSize = CountStringBuffer(OperateFailure.szDescribeString);
		WORD wHeadSize = sizeof(OperateFailure) - sizeof(OperateFailure.szDescribeString);
		m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_OPERATE_FAILURE, dwContextID, dwTokenID, &OperateFailure, wHeadSize + wDataSize);
	}

	return true;
}

//更新财富
bool CDataBaseEngineSink::UpdateUserWealth(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID)
{
	try
	{
		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_QueryGameWealth"), true);

		//加载成功
		if (lResultCode == DB_SUCCESS)
		{
			//更新财富
			DBO_GR_WealthUpdate WealthUpdate;
			ZeroMemory(&WealthUpdate, sizeof(WealthUpdate));

			//设置变量
			WealthUpdate.wItemCount = 2;
			WealthUpdate.WealthItem[0].wMoneyID = GOODS_ID_GOLD;
			WealthUpdate.WealthItem[0].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserGold"));
			WealthUpdate.WealthItem[1].wMoneyID = GOODS_ID_CARD;
			WealthUpdate.WealthItem[1].lBalance = m_AccountsDBAide.GetValue_LONGLONG(TEXT("UserCard"));			
			
			//发送结果			
			WORD wHeadSize = sizeof(WealthUpdate) - sizeof(WealthUpdate.WealthItem);
			WORD wPacketSize = WealthUpdate.wItemCount * sizeof(WealthUpdate.WealthItem[0]) + wHeadSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_WEALTH_UPDATE, dwContextID, dwTokenID ,&WealthUpdate, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

//更新物品
bool CDataBaseEngineSink::UpdateUserGoods(DWORD dwUserID, DWORD dwContextID, DWORD dwTokenID, tagGoodsItem GoodsItemList[], WORD wGoodsCount)
{
	try
	{
		//变量定义
		CString strGoodsID;
		CString strGoodsIDList;

		//构造物品列表
		for (auto i = 0; i < wGoodsCount; i++)
		{
			//添加括号
			if (i == 0) strGoodsIDList.Append(TEXT("["));

			//格式化
			strGoodsID.Format(TEXT("%d"), GoodsItemList[i].wGoodsID);

			//追加数据
			strGoodsIDList.Append(strGoodsID);
			strGoodsIDList.Append((i < wGoodsCount - 1) ? TEXT(",") : TEXT("]"));
		}

		//构造参数
		m_AccountsDBAide.ResetParameter();
		m_AccountsDBAide.AddParameter(TEXT("@dwUserID"), dwUserID);
		m_AccountsDBAide.AddParameter(TEXT("@szGoodsIDList"), strGoodsIDList);
		LONG lResultCode = m_AccountsDBAide.ExecuteProcess(TEXT("GSP_GP_UserQueryGoods"), true);

		//加载成功
		if (lResultCode == DB_SUCCESS)
		{
			//变量定义
			DBO_GR_GoodsUpdate UserGoodsUpdate;
			ZeroMemory(&UserGoodsUpdate, sizeof(UserGoodsUpdate));

			//读取信息
			while (m_AccountsDBModule->IsRecordsetEnd() == false)
			{
				//获取对象
				auto pGoodsHoldInfo = &UserGoodsUpdate.GoodsHoldInfo[UserGoodsUpdate.wGoodsCount++];

				//物品信息
				pGoodsHoldInfo->wGoodsID = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsID"));
				pGoodsHoldInfo->wGoodsKind = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsKind"));
				pGoodsHoldInfo->wGoodsLevel = m_AccountsDBAide.GetValue_WORD(TEXT("GoodsLevel"));				
				pGoodsHoldInfo->dwGoodsCount = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsCount"));
				pGoodsHoldInfo->dwGoodsIndate = m_AccountsDBAide.GetValue_DWORD(TEXT("GoodsIndate"));
				pGoodsHoldInfo->dwExpireTime = m_AccountsDBAide.GetValue_DWORD(TEXT("ExpireTime"));

				//移动记录
				m_AccountsDBModule->MoveToNext();
			}

			//发送数据
			WORD wHeadSize = sizeof(UserGoodsUpdate) - sizeof(UserGoodsUpdate.GoodsHoldInfo);
			WORD wPacketSize = UserGoodsUpdate.wGoodsCount * sizeof(UserGoodsUpdate.GoodsHoldInfo[0]) + wHeadSize;
			m_pIDataBaseEngineEvent->OnEventDataBaseResult(DBO_GR_USER_GOODS_UPDATE, dwContextID, dwTokenID, &UserGoodsUpdate, wPacketSize);
		}

		return true;
	}
	catch (IDataBaseException* pIException)
	{
		TraceDatabaseException(__FILE__, __FUNCTION__, __LINE__, pIException);
	}

	return false;
}

//输出信息
void CDataBaseEngineSink::TraceDatabaseException(char *pFile, char *pFunction, int nLine,IDataBaseException * pIException)
{
	CString strMessage;
	strMessage.Format(TEXT("%s(%d):%hs(%s)"), CA2CT(pFile).m_psz, nLine, pFunction, pIException ? pIException->GetExceptionDescribe() : TEXT(""));
	CTraceService::TraceString(strMessage, TraceLevel_Exception);

}
//////////////////////////////////////////////////////////////////////////

