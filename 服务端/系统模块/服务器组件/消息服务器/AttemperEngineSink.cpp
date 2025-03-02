#include "StdAfx.h"
#include "ServiceUnits.h"
#include "ControlPacket.h"
#include "DataBasePacket.h"
#include "AttemperEngineSink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_CONNECT_VALIDATE		2									//重连标识
#define IDI_CONNECT_CENTER			3									//重连标识

//////////////////////////////////////////////////////////////////////////
//颜色定义
#define COLOR_TEXT					0xFFBF7B							//文本颜色
#define COLOR_SYSTEM				0xFFBF7B							//系统颜色
#define COLOR_NICKNAME				0xFFFFFF							//昵称颜色
#define COLOR_MATCHNAME				0xFFBF7B							//比赛颜色
#define COLOR_NORMAL_NUMBER			0xFF00FF							//普通数字
#define COLOR_WEALTH_NUMBER			0xFFFFFF							//财富数字

//常量定义
#define	NOTICE_CACHE_COUNT			10									//缓存数量
#define MESSAGE_SERVER_LOG_FILE		L"Message"
//////////////////////////////////////////////////////////////////////////
//消息格式
LPCTSTR g_pszMsgFormat1=L"#%d#%s#%d#在%s中获得第%d名,赢得";

//////////////////////////////////////////////////////////////////////////

//构造函数
CAttemperEngineSink::CAttemperEngineSink()
{
	//状态变量	
	m_bCollectUser=false;

	//设置变量	
	m_pServiceOption=NULL;
	m_pInitParameter=NULL;
	m_pBindParameter=NULL;		

	//组件变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;	
	m_pITCPSocketCenter=NULL;

	//初始化
	m_StockMarqueeNoticeInfo.InitHashTable(101);

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

	//配置日志
	if (InitLoggerService() == false)
	{
		ASSERT(FALSE);
		return false;
	}

	return true;
}

//停止事件
bool CAttemperEngineSink::OnAttemperEngineStop(IUnknownEx * pIUnknownEx)
{
	//状态变量		
	m_bCollectUser=false;

	//停止服务
	g_TimeLoggerService->ConcludeService();

	//设置变量
	m_pITimerEngine=NULL;
	m_pIDataBaseEngine=NULL;
	m_pITCPNetworkEngine=NULL;
	m_pITCPSocketCenter=NULL;

	//删除数据
	SafeDeleteArray(m_pBindParameter);

	//设置组件
	m_GlobalItemManager.ResetData();		

	return true;
}

//控制事件
bool CAttemperEngineSink::OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize)
{
	switch (dwIdentifier)
	{
	case CT_CONNECT_CENTER:		//连接中心
	{
		//汇总标志
		m_bCollectUser = false;

		//设置地址
		DWORD dwCenterServer=m_pInitParameter->m_CenterAddress.dwServiceAddr;

		//发起连接
		m_pITCPSocketCenter->Connect(htonl(dwCenterServer), m_pInitParameter->m_wCenterPort);

		//提示消息
		CTraceService::TraceString(TEXT("正在尝试连接中心服务器..."),TraceLevel_Normal);

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
	case IDI_CONNECT_CENTER:	//连接中心
		{
			//变量定义
			tagAddressInfo * pAddressInfo=&m_pInitParameter->m_CenterAddress;

			//设置地址
			DWORD dwCenterServer=pAddressInfo->dwServiceAddr; 

			//汇总标志
			m_bCollectUser = false;

			//发起连接
			m_pITCPSocketCenter->Connect(htonl(dwCenterServer), m_pInitParameter->m_wCenterPort);

			//提示消息
			CTraceService::TraceString(TEXT("正在尝试连接中心服务器..."),TraceLevel_Normal);

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
	//变量定义
	bool bResult = true;
	WORD wMainCmdID = LOCMMD(Command.wMainCmdID);

	{
		CString str;
		str.Format(L"[LH][MessageServer] M:%d,S:%d", wMainCmdID, Command.wSubCmdID);
		FTLOG(MESSAGE_SERVER_LOG_FILE)->LogInfo(str);
	}

	//注册服务
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
	case MDM_AN_MESSAGE_SERVICE:	//消息服务
		{
			bResult = OnTCPNetWorkMainMessageService(Command.wSubCmdID,pData,wDataSize,dwSocketID,dwTokenID);
			break;
		}
	}

	//断开链接
	if (bResult == false)
	{
		//构造结构
		CMD_CS_C_ShutdownSocket ShutdownSocket;
		ShutdownSocket.bForceClose = false;

		//发送数据
		SendDataToGate(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_SHUTDOWN_SOCKET, &ShutdownSocket, sizeof(ShutdownSocket), &dwTokenID, 1);
	}

	return true;
}

//关闭事件
bool CAttemperEngineSink::OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID)
{
	//获取信息
	WORD wBindIndex=LOWORD(dwSocketID);
	tagBindParameter * pBindParameter=(m_pBindParameter+wBindIndex);

	//网关服务
	if (pBindParameter->wServiceModule==SERVICE_MODULE_GATE)
	{
		//变量定义
		WORD wBindIndex=LOWORD(dwSocketID);

		//查找网关
		CGlobalGateItem * pGlobalGateItem=m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
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
	switch (wRequestID)
	{
	case DBO_GR_USER_MAIL_INFO:		//邮件信息
	{
		return OnDBUserMailInfo(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_USER_MAIL_FINISH:	//邮件完成
	{
		return OnDBUserMailFinish(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_USER_MAIL_UPDATE:	//邮件更新
	{
		return OnDBUserMailUpdate(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_USER_MAIL_INSERT:	//邮件插入
	{
		return OnDBUserMailInsert(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_CLAIM_MAIL_ATTACH:	//领取附件
	{
		return OnDBClaimMailAttach(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_USER_GOODS_UPDATE:	//物品更新
	{
		return OnDBUserGoodsUpdate(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_USER_WEALTH_UPDATE:	//财富更新
	{
		return OnDBUserWealthUpdate(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_OPERATE_SUCCESS:	//操作结果
	{
		return OnDBOperateSuccess(dwContextID, dwTokenID, pData, wDataSize);
	}
	case DBO_GR_OPERATE_FAILURE:	//操作失败
	{
		return OnDBOperateFailure(dwContextID, dwTokenID, pData, wDataSize);
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
			m_pITimerEngine->SetTimer(IDI_CONNECT_CENTER,m_pInitParameter->m_wConnectTime*1000L,1,0);

			return false;
		}

		//变量定义
		CMD_CS_C_RegisterService RegisterService;
		ZeroMemory(&RegisterService,sizeof(RegisterService));

		//构造结构
		RegisterService.wServiceID= m_pServiceOption->wServiceID;
		RegisterService.wServicePort=m_pServiceOption->wServicePort;
		RegisterService.wServiceModule= SERVICE_MODULE_MESSAGE;
		RegisterService.wServiceKindID = m_pServiceOption->wServiceKindID;
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
	{
		CString str;
		str.Format(L"[LH][MessageServer][Server] M:%d,S:%d", Command.wMainCmdID, Command.wSubCmdID);
		FTLOG(MESSAGE_SERVER_LOG_FILE)->LogInfo(str);
	}

	//中心连接
	if (dwServiceID==NETWORK_CENTER)
	{
		switch (Command.wMainCmdID)
		{
		case MDM_CS_REGISTER:			//注册服务
		{
			return OnTCPSocketMainRegister(Command.wSubCmdID,pData,wDataSize);
		}
		case MDM_CS_USER_COLLECT:		//用户汇总
		{
			return OnTCPSocketMainCollectService(Command.wSubCmdID,pData,wDataSize);
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
			return OnTCPSocketMainForwardService(Command.wSubCmdID,pData,wDataSize);
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
		case SUB_CS_S_SERVICE_INSERT:	//服务插入
		{
			//参数校验
			ASSERT(wDataSize % sizeof(tagServiceItem) == 0);
			if (wDataSize % sizeof(tagServiceItem) != 0) return false;

			//提取数据
			tagServiceItem * pServiceItem = (tagServiceItem *)pData;

			//日志服务
			if (pServiceItem->wServiceModule == SERVICE_MODULE_LOGGER)
			{
				//日志服务
				g_TimeLoggerService->SetLoggerServer(pServiceItem->szServiceDomain, pServiceItem->wServicePort);

				FTLOG(MESSAGE_SERVER_LOG_FILE)->LogInfo(L"connect log server ...");
			}

			return true;
		}
		case SUB_CS_S_SERVICE_REMOVE:	//服务移除
		{
			//参数校验
			ASSERT(wDataSize == sizeof(CMD_CS_S_ServiceRemove));
			if (wDataSize != sizeof(CMD_CS_S_ServiceRemove)) return false;

			//提取数据
			CMD_CS_S_ServiceRemove * pServiceRemove = (CMD_CS_S_ServiceRemove *)pData;


			return true;
		}
	}

	return true;
}

//管理服务
bool CAttemperEngineSink::OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	return true;
}

//汇总服务
bool CAttemperEngineSink::OnTCPSocketMainCollectService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_CS_S_COLLECT_USER:			//用户汇总
	{
		//变量定义
		CMD_CS_C_EnterMessage EnterMessage;
		ZeroMemory(&EnterMessage, sizeof(EnterMessage));

		//发送用户
		POSITION Position = NULL;
		CGlobalUserItem * pGlobalUserItem = NULL;

		do
		{
			//获取用户
			pGlobalUserItem = m_GlobalItemManager.EnumUserItem(pGlobalUserItem);
			if (pGlobalUserItem == NULL) break;

			//设置变量
			EnterMessage.dwUserID = pGlobalUserItem->GetUserID();
			
			//发送数据
			ASSERT(m_pITCPSocketCenter != NULL);
			m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_ENTER_MESSAGE, &EnterMessage, sizeof(EnterMessage), 0);

		} while (true);

		//汇报完成
		m_bCollectUser = true;
		m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_COLLECT_FINISH, 0);

		return true;
	}
	}

	return true;
}

//转发服务
bool CAttemperEngineSink::OnTCPSocketMainForwardService(WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	FTLOG(MESSAGE_SERVER_LOG_FILE)->LogInfo(L"[LH][MessageServer][Forward] S:%d", wSubCmdID);	

	switch (wSubCmdID)
	{
	case SUB_CS_MAIL_INSERT:		//邮件插入
	{
		//参数校验
		ASSERT(wDataSize == sizeof(CMD_CS_MailInsert));
		if (wDataSize != sizeof(CMD_CS_MailInsert)) return true;

		//提取数据
		auto pMailInsert = (CMD_CS_MailInsert*)pData;

		//构造结构
		DBR_GR_UserInsertMail UserInsertMail;
		ZeroMemory(&UserInsertMail,sizeof(UserInsertMail));

		//设置变量
		UserInsertMail.dwMailID = pMailInsert->dwMailID;
		UserInsertMail.dwUserID = pMailInsert->dwUserID;
		UserInsertMail.LastLogonTime = pMailInsert->LastLogonTime;

		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_INSERT_MAIL, UserInsertMail.dwMailID, 0, 0, &UserInsertMail, sizeof(UserInsertMail));

		return true;
	}
	case SUB_CS_MAIL_NOTIFY:		//邮件通知
	{
		//参数校验
		ASSERT (wDataSize%sizeof(CMD_CS_MailNotify)==0);
		if (wDataSize % sizeof(CMD_CS_MailNotify) != 0) return true;

		//提取数据
		WORD wItemCount = wDataSize / sizeof(CMD_CS_MailNotify);
		CMD_CS_MailNotify * pMailNotify = (CMD_CS_MailNotify*)pData;

		for (int i = 0; i < wItemCount; i++)
		{
			//群发邮件
			if (pMailNotify->dwUserID == 0)
			{
                //构造结构
                CMD_AN_S_UserMailNotify UserMailNotify;
                ZeroMemory(&UserMailNotify, sizeof(UserMailNotify));

                //邮件信息
                UserMailNotify.dwMailID = pMailNotify->dwMailID;

                //发送数据
                SendBatchData(MDM_AN_MESSAGE_SERVICE, SUB_AN_S_USER_MAIL_NOTIFY, &UserMailNotify, sizeof(UserMailNotify));
			}
			else
			{
                //查找玩家
                CGlobalUserItem* pClobalUserItem = m_GlobalItemManager.SearchUserItem(pMailNotify->dwUserID);
                if (pClobalUserItem != NULL)
                {
                    //构造结构
                    CMD_AN_S_UserMailNotify UserMailNotify;
                    ZeroMemory(&UserMailNotify, sizeof(UserMailNotify));

                    //比赛信息
                    UserMailNotify.dwMailID = pMailNotify->dwMailID;

					//查找网关
					auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pClobalUserItem->GetGateID());
					if (pGlobalGateItem != NULL)
					{
						//发送数据
						SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_AN_MESSAGE_SERVICE, SUB_AN_S_USER_MAIL_NOTIFY, &UserMailNotify, sizeof(UserMailNotify), &pClobalUserItem->m_dwTokenID, 1);
					}                    
                }
			}

			pMailNotify++;
		}

		return true;
	}
	case SUB_CS_MATCH_REMIND:		//比赛提醒
	{
		//参数校验
		ASSERT (sizeof(CMD_CS_MatchRemind)==wDataSize);
		if (sizeof(CMD_CS_MatchRemind)!=wDataSize) return false;

		//提取数据
		CMD_CS_MatchRemind* pMatchRemind=(CMD_CS_MatchRemind*)pData;

		//查找玩家
		CGlobalUserItem * pClobalUserItem=m_GlobalItemManager.SearchUserItem(pMatchRemind->dwUserID);
		if (pClobalUserItem!=NULL)
		{
			//构造结构
			CMD_AN_S_MatchRemind ANMatchRemind;
			ZeroMemory(&ANMatchRemind,sizeof(ANMatchRemind));
				
			//比赛信息
			ANMatchRemind.lMatchNo=pMatchRemind->lMatchNo;
			ANMatchRemind.dwMatchID=pMatchRemind->dwMatchID;
			ANMatchRemind.dwSurplusTime=pMatchRemind->dwSurplusTime;
			StringCchCopy(ANMatchRemind.szMatchName,CountArray(ANMatchRemind.szMatchName), pMatchRemind->szMatchName);
			CopyMemory(&ANMatchRemind.MatchStartTime,&pMatchRemind->MatchStartTime,sizeof(ANMatchRemind.MatchStartTime));

			//房间信息
			ANMatchRemind.wKindID=pMatchRemind->wKindID;
			ANMatchRemind.wServerID=pMatchRemind->wServerID;
			ANMatchRemind.wServerPort=pMatchRemind->wServerPort;
			ANMatchRemind.dwServerAddr=pMatchRemind->dwServerAddr;
			StringCchCopy(ANMatchRemind.szServerDomain,CountArray(ANMatchRemind.szServerDomain), pMatchRemind->szServerDomain);

			//查找网关
			auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pClobalUserItem->GetGateID());
			if (pGlobalGateItem != NULL)
			{
				//发送数据
				SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_AN_MESSAGE_SERVICE, SUB_AN_S_MATCH_REMIND, &ANMatchRemind, sizeof(ANMatchRemind), &pClobalUserItem->m_dwTokenID, 1);
			}
		}

		return true;
	}
	case SUB_CS_MATCH_WINAWARD:		//比赛获奖
	{
		//参数校验
		//ASSERT (sizeof(CMD_CS_C_MatchWinAward)==wDataSize);
		//if (sizeof(CMD_CS_C_MatchWinAward)!=wDataSize) return false;

		////提取数据
		//CMD_CS_C_MatchWinAward* pMatchWinAward=(CMD_CS_C_MatchWinAward*)pData;

		////变量定义
		//CString strMessage;
		//CString strMessageItem;

		////格式化消息
		//strMessage.Format(g_pszMsgFormat1,COLOR_NICKNAME,pMatchWinAward->szNickName,COLOR_TEXT,pMatchWinAward->szMatchName,pMatchWinAward->wRankID);

		//////实物奖励
		////if (pMatchWinAward->szRewardEntity[0]!=0)
		////{
		////	strMessageItem.Format(TEXT("#%d#%s"),COLOR_WEALTH_NUMBER,pMatchWinAward->szRewardEntity);
		////	strMessage.Append(strMessageItem);
		////}

		////构造消息
		//tagScrollMessageItem ScrollMessageItem;
		//ZeroMemory(&ScrollMessageItem,sizeof(ScrollMessageItem));

		////设置变量
		//ScrollMessageItem.cbType=MSG_TYPE_NORMAL;
		//ScrollMessageItem.wLength=strMessage.GetLength();
		//StringCchCopy(ScrollMessageItem.szContent,ScrollMessageItem.wLength, strMessage.GetString());

		////消息大小
		//WORD wMessageSize=CountStringBuffer(ScrollMessageItem.szContent);
		//wMessageSize+=sizeof(ScrollMessageItem)-sizeof(ScrollMessageItem.szContent);

		////构造结构
		//tagScrollMessageItem * pScrollMessageItem=(tagScrollMessageItem *)m_cbBuffer;		

		////设置变量			
		//CopyMemory(pScrollMessageItem,&ScrollMessageItem,wMessageSize);

		////添加消息
		//m_ScrollMessageArray.Add(ScrollMessageItem);

		////移除消息
		//if (m_ScrollMessageArray.GetCount() > MESSAGE_CACHE_COUNT)
		//{
		//	m_ScrollMessageArray.RemoveAt(0);
		//}

		////推送消息
		//SendBatchData(MDM_AN_MESSAGE_SERVICE, SUB_AN_S_MESSAGE_LIST, pScrollMessageItem, wMessageSize);
			
		return true;
	}
	case SUB_CS_MARQUEE_NOTICE:		//跑马灯公告
	{
		//参数校验
		ASSERT (sizeof(CMD_CS_MarqueeNotice)>=wDataSize);
		if (sizeof(CMD_CS_MarqueeNotice) < wDataSize) return false;

		//提取数据
		auto pMarqueeNotice =(CMD_CS_MarqueeNotice*)pData;

		//清理超时
		DWORD dwCurrentTime = time(NULL);
		static DWORD dwLastCleanTime = dwCurrentTime;
		if (dwCurrentTime - dwLastCleanTime >= 300)
		{
			CleanExpireNotice();
			dwLastCleanTime = dwCurrentTime;
		}

		//查找公告
		auto pMarqueeNoticeInfo = m_StockMarqueeNoticeInfo.SearchStockItem(pMarqueeNotice->dwNoticeID);
		if (pMarqueeNoticeInfo == NULL)
		{
			//构造结构
			pMarqueeNoticeInfo = m_StockMarqueeNoticeInfo.CreateStockItem(pMarqueeNotice->dwNoticeID);

			//设置变量
			pMarqueeNoticeInfo->dwNoticeID = pMarqueeNotice->dwNoticeID;
			pMarqueeNoticeInfo->cbNoticeKind = pMarqueeNotice->cbNoticeKind;
			pMarqueeNoticeInfo->bExclusive = pMarqueeNotice->bExclusive;
			pMarqueeNoticeInfo->cbPriority = pMarqueeNotice->cbPriority;
			pMarqueeNoticeInfo->wRollTimes = pMarqueeNotice->wRollTimes;
			pMarqueeNoticeInfo->wViewPlace = pMarqueeNotice->wViewPlace;
			pMarqueeNoticeInfo->dwEndRollTime = pMarqueeNotice->dwEndRollTime;
			pMarqueeNoticeInfo->dwStartRollTime = pMarqueeNotice->dwStartRollTime;
			pMarqueeNoticeInfo->wNoticeSize = pMarqueeNotice->wNoticeSize;
			CopyMemory(pMarqueeNoticeInfo->cbNoticeContent, pMarqueeNotice->cbNoticeContent, pMarqueeNoticeInfo->wNoticeSize);			
		}
		else
		{
			pMarqueeNoticeInfo->bExclusive = pMarqueeNotice->bExclusive;
			pMarqueeNoticeInfo->cbPriority = pMarqueeNotice->cbPriority;
			pMarqueeNoticeInfo->wRollTimes = pMarqueeNotice->wRollTimes;
			pMarqueeNoticeInfo->wViewPlace = pMarqueeNotice->wViewPlace;
			pMarqueeNoticeInfo->dwEndRollTime = pMarqueeNotice->dwEndRollTime;
			pMarqueeNoticeInfo->dwStartRollTime = pMarqueeNotice->dwStartRollTime;
			pMarqueeNoticeInfo->wNoticeSize = pMarqueeNotice->wNoticeSize;
			CopyMemory(pMarqueeNoticeInfo->cbNoticeContent, pMarqueeNotice->cbNoticeContent, pMarqueeNoticeInfo->wNoticeSize);
		}		

		//批量发送
		SendBatchData(MDM_AN_MESSAGE_SERVICE, SUB_AN_S_MARQUEE_NOTICE, pMarqueeNoticeInfo, sizeof(tagMarqueeNoticeInfo));

		return true;
	}
	case SUB_CS_SEND_AUTHCODE:		//发送验证码
	{
		//参数校验
		ASSERT(sizeof(CMD_CS_SendAuthCode) == wDataSize);
		if (sizeof(CMD_CS_SendAuthCode) != wDataSize) return false;

		//提取数据
		CMD_CS_SendAuthCode* pSendAuthCode = (CMD_CS_SendAuthCode*)pData;

		//变量定义
		DWORD dwTimeStamp = (DWORD)time(NULL);
		CStringW strAutoCode(pSendAuthCode->szAuthCode);

		//短信验证码
		if (pSendAuthCode->cbCodeKind == CODE_KIND_MP)
		{
			CStringW strMobilePhone(pSendAuthCode->szMobileEmail);

			//调整号码
			auto nIndex = strMobilePhone.Find('+');
			if (nIndex >= 0) strMobilePhone.Delete(nIndex);

			//格式化令牌
			TCHAR szSign[256];
			_sntprintf_s(szSign, CountArray(szSign), TEXT("phonecode=%ls&phonenumber=%ls&servertime=%d&key=%s"), strAutoCode.GetString(), strMobilePhone.GetString(), dwTimeStamp, szCompilation);

			//令牌加密
			TCHAR szSignMD5[LEN_MD5];
			CMD5Encrypt::EncryptData(szSign, szSignMD5);

			//构造结构
			DBR_GR_RequestWebPage RequestWebPage;
			ZeroMemory(&RequestWebPage, sizeof(RequestWebPage));

			//请求类型
			RequestWebPage.hrType = Hr_Post;

			//设置变量			
			_snprintf_s(RequestWebPage.szWebUrl, CountArray(RequestWebPage.szWebUrl), "%s/api/Sms/SendInter", CT2CA(m_pInitParameter->m_szPaltformDomain).m_psz);

			//设置头部
			StringCchCopyA(RequestWebPage.szHeader, CountArray(RequestWebPage.szHeader), "content-type:application/json\r\n\r\n");

			//构造内容
			LPCSTR pszPostFormat = "{\"phonenumber\":\"%ls\",\"phonecode\":\"%ls\",\"servertime\":%d,\"sign\":\"%s\"}";
			_snprintf_s(RequestWebPage.szPostData, CountArray(RequestWebPage.szPostData), pszPostFormat, strMobilePhone.GetString(), strAutoCode.GetString(), dwTimeStamp, CT2CA(szSignMD5).m_psz);

			//投递数据
			WORD wDataSize = CountStringBufferA(RequestWebPage.szPostData);
			wDataSize += sizeof(RequestWebPage) - sizeof(RequestWebPage.szPostData);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_REQUEST_WEBPAGE, 0, 0, 0, &RequestWebPage, wDataSize);
		}
		else
		{
			//变量定义
			DWORD dwTimeStamp = (DWORD)time(NULL);
			CStringW strAutoCode(pSendAuthCode->szAuthCode);
			CStringW strEmail(pSendAuthCode->szMobileEmail);

			//格式化令牌
			TCHAR szToken[256];
			_sntprintf_s(szToken, CountArray(szToken), TEXT("email=%ls&mailtitle=%s&servertime=%d&templatename=sendcode&key=%s"), strEmail.GetString(), TEXT("Email verification code"), dwTimeStamp, szCompilation);

			//令牌加密
			TCHAR szTokenMD5[LEN_MD5];
			CMD5Encrypt::EncryptData(szToken, szTokenMD5);

			//构造结构
			DBR_GR_RequestWebPage RequestWebPage;
			ZeroMemory(&RequestWebPage, sizeof(RequestWebPage));

			//请求类型
			RequestWebPage.hrType = Hr_Post;

			//设置变量			
			_snprintf_s(RequestWebPage.szWebUrl, CountArray(RequestWebPage.szWebUrl), "%s/api/Mail/SendMail", CT2A(m_pInitParameter->m_szPaltformDomain).m_psz);

			//设置头部
			StringCchCopyA(RequestWebPage.szHeader, CountArray(RequestWebPage.szHeader), "content-type:application/json\r\n\r\n");

			//构造内容
			LPCSTR pszPostFormat = "{\"email\":\"%ls\",\"mailtitle\":\"%s\",\"templateName\":\"sendcode\",\"templateParam\":{\"code\":\"%ls\"},\"serverTime\":%d,\"sign\":\"%s\"}";
			_snprintf_s(RequestWebPage.szPostData, CountArray(RequestWebPage.szPostData), pszPostFormat, strEmail.GetString(), "Email verification code", strAutoCode.GetString(), dwTimeStamp, CT2CA(szTokenMD5).m_psz);

			//投递数据
			WORD wDataSize = CountStringBufferA(RequestWebPage.szPostData);
			wDataSize += sizeof(RequestWebPage) - sizeof(RequestWebPage.szPostData);
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_REQUEST_WEBPAGE, 0, 0, 0, &RequestWebPage, wDataSize);
		}

		return true;
	}		
	case SUB_CS_SEND_EXCEPTION_ALARM:	//异常预警
	{
		//参数校验
		ASSERT(sizeof(CMD_CS_SendExceptionAlarm) == wDataSize);
		if (sizeof(CMD_CS_SendExceptionAlarm) != wDataSize) return false;

		//提取数据
		auto pSendExceptionAlarm = (CMD_CS_SendExceptionAlarm*)pData;

		//变量定义
		DWORD dwTimeStamp = (DWORD)time(NULL);

		//邮件标题
		//LPCSTR pszMailTitle = "DataBase Exception Notification";

		//构造内容
		WCHAR szMailContent[256];
		_snwprintf_s(szMailContent, CountArray(szMailContent), L"请注意：系统检测到[%ls]%ls", CT2CW(pSendExceptionAlarm->szServiceName).m_psz, CT2CW(pSendExceptionAlarm->szExceptionInfo).m_psz);

		//格式化令牌
		WCHAR szSignUnic[256];
		_snwprintf_s(szSignUnic, CountArray(szSignUnic), L"mailcontent=%s&mailtitle=%hs&servertime=%d&key=%s", szMailContent, pSendExceptionAlarm->szAlarmTitle, dwTimeStamp, szCompilation);

		//转换编码
		CHAR szSignUtf8[512];
		CWHService::Unicode2Utf8(szSignUnic, szSignUtf8, CountArray(szSignUtf8));

		//签名加密
		CHAR szSignMD5[LEN_MD5];
		CMD5Encrypt::EncryptData(szSignUtf8, szSignMD5);

		//构造结构
		DBR_GR_RequestWebPage RequestWebPage;
		ZeroMemory(&RequestWebPage, sizeof(RequestWebPage));

		//请求类型
		RequestWebPage.hrType = Hr_Post;

		//设置变量			
		_snprintf_s(RequestWebPage.szWebUrl, CountArray(RequestWebPage.szWebUrl), "%s/api/Mail/SendWarningMail", CT2A(m_pInitParameter->m_szPaltformDomain).m_psz);

		//设置头部
		StringCchCopyA(RequestWebPage.szHeader, CountArray(RequestWebPage.szHeader), "content-type:application/json\r\n\r\n");

		//转换编码
		CHAR szMailContentUtf8[512];
		CWHService::Unicode2Utf8(szMailContent, szMailContentUtf8, sizeof(szMailContentUtf8));

		//构造内容			
		LPCSTR pszPostFormat = "{\"mailtitle\":\"%hs\",\"mailcontent\":\"%hs\",\"serverTime\":%d,\"sign\":\"%s\"}";
		_snprintf_s(RequestWebPage.szPostData, CountArray(RequestWebPage.szPostData), pszPostFormat, pSendExceptionAlarm->szAlarmTitle, szMailContentUtf8, dwTimeStamp, szSignMD5);

		//投递数据
		WORD wDataSize = CountStringBufferA(RequestWebPage.szPostData);
		wDataSize += sizeof(RequestWebPage) - sizeof(RequestWebPage.szPostData);
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_REQUEST_WEBPAGE, 0, 0, 0, &RequestWebPage, wDataSize);

		return true;
	}
	}

	return true;
}

//注册服务
bool CAttemperEngineSink::OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID)
{
	switch (wSubCmdID)
	{	
	case SUB_CS_C_REGISTER_SERVICE:		//注册网关
		{
			//效验数据
			ASSERT (wDataSize==sizeof(CMD_CS_C_RegisterService));
			if (wDataSize!=sizeof(CMD_CS_C_RegisterService)) return false;

			//消息定义
			CMD_CS_C_RegisterService * pRegisterService =(CMD_CS_C_RegisterService *)pData;

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
				StringCchCopy(RegisterFailure.szDescribeString,CountArray(RegisterFailure.szDescribeString), TEXT("已经存在相同标识的网关服务，网关服务注册失败"));

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
			(m_pBindParameter + wBindIndex)->wServiceID = pRegisterService->wServiceID;
			(m_pBindParameter+wBindIndex)->wServiceModule= pRegisterService->wServiceModule;			

			//变量定义
			tagServiceItem GateItem;
			ZeroMemory(&GateItem,sizeof(GateItem));

			//构造数据
			GateItem.wServiceID= pRegisterService->wServiceID;
			GateItem.wServicePort= pRegisterService->wServicePort;
			GateItem.wServiceModule = pRegisterService->wServiceModule;
			GateItem.dwServiceAddr= pRegisterService->dwServiceAddr;
			StringCchCopy(GateItem.szServiceName, CountArray(GateItem.szServiceName), pRegisterService->szServiceName);
			StringCchCopy(GateItem.szServiceDomain, CountArray(GateItem.szServiceDomain), pRegisterService->szServiceDomain);

			//注册房间
			auto pGlobalGateItem=m_GlobalItemManager.ActiveGateItem(dwSocketID,GateItem);

			//注册成功
			m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_REGISTER,SUB_CS_S_REGISTER_SUCCESS,NULL,0);	

			//群发设置
			m_pITCPNetworkEngine->AllowBatchSend(dwSocketID,true,SERVICE_MODULE_GATE);

			//判断用户数
			if (m_GlobalItemManager.GetUserItemCount()>0)
			{
				//变量定义
				POSITION Position=NULL;
				CGlobalUserItem * pGlobalUserItem=NULL;

				do
				{
					//枚举用户
					pGlobalUserItem=m_GlobalItemManager.EnumUserItem(pGlobalUserItem);
					if (pGlobalUserItem==NULL) break;

					//校验标识
					if (pGlobalUserItem->m_wGateID== pRegisterService->wServiceID)
					{
						//删除用户
						if (pRegisterService->wConnectCount==0)
						{
							m_GlobalItemManager.DeleteUserItem(pGlobalUserItem->m_dwUserID);
						}
						else
						{
							//添加令牌
							pGlobalGateItem->ActiveTokenID(pGlobalUserItem->m_dwTokenID);
						}
					}

				} while (true);
			}
			else
			{
				//收集用户
				if (pRegisterService->wConnectCount>0)
				{
					m_pITCPNetworkEngine->SendData(dwSocketID,MDM_CS_COMMON_SERVICE,SUB_CS_S_COLLECT_USER,NULL,0);
				}
			}

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
			tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

			//校验服务
			if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return true;

			//提取数据
			CMD_CS_C_LogoutToken* pLogoutToken = (CMD_CS_C_LogoutToken*)pData;

			//查找网关
			CGlobalGateItem* pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
			if (pGlobalGateItem != NULL)
			{
				//查找参数
				auto pTokenParameter = pGlobalGateItem->SearchTokenID(pLogoutToken->dwTokenID);

				//汇总用户
				if (pTokenParameter!=NULL && pTokenParameter->pBindUserItem!=NULL && m_bCollectUser == true)
				{
					auto pGlobalUserItem = (CGlobalUserItem *)pTokenParameter->pBindUserItem;

					//变量定义
					CMD_CS_C_LeaveMessage LeaveMessage;
					ZeroMemory(&LeaveMessage, sizeof(LeaveMessage));

					//设置变量
					LeaveMessage.dwUserID = pGlobalUserItem->GetUserID();

					//发送消息
					ASSERT(m_pITCPSocketCenter != NULL);
					m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_LEAVE_MESSAGE, &LeaveMessage, sizeof(LeaveMessage), 0);
				}

				//删除令牌
				pGlobalGateItem->RemoveTokenID(pLogoutToken->dwTokenID);
			}

			return true;
		}
	case SUB_CS_C_REGISTER_TOKEN:
		{
			//设置绑定
			WORD wBindIndex = LOWORD(dwSocketID);
			tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

			//移除令牌
			if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return true;

			//提取数据
			CMD_CS_C_RegisterToken* pRegisterToken = (CMD_CS_C_RegisterToken*)pData;

			//构造结构
			CMD_CS_S_RegisterToken RegisterToken;
			ZeroMemory(&RegisterToken, sizeof(RegisterToken));

			//默认成功
			RegisterToken.lResultCode = REGISTER_RESULT_FAILURE;

			//查找网关
			CGlobalGateItem* pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
			if (pGlobalGateItem != NULL)
			{
				//查询用户
				auto pTokenParameter = pGlobalGateItem->ActiveTokenID(pRegisterToken->dwTokenID);
				if (pTokenParameter != NULL)
				{
					//注册成功
					RegisterToken.lResultCode = REGISTER_RESULT_SUCCESS;

					//设置令牌		
					pTokenParameter->pBindUserItem = NULL;
					pTokenParameter->dwUserID = pRegisterToken->dwUserID;
					pTokenParameter->dwClientIP = pRegisterToken->dwClientIP;					
				}
			}

			//发送数据
			m_pITCPNetworkEngine->SendData(dwSocketID, MDM_CS_COMMON_SERVICE, SUB_CS_S_REGISTER_TOKEN, &RegisterToken, sizeof(RegisterToken), &pRegisterToken->dwTokenID, 1);

			return true;
		}
	}

	return false;
}

//邮件信息
bool CAttemperEngineSink::OnDBUserMailInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pDBUserMailInfo = (DBO_GR_UserMailInfo*)pData;

	//获取参数
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL || pTokenParameter->pBindUserItem == NULL) return true;	

	//定义变量
	CMD_AN_S_UserMailInfo* pUserMailInfo = (CMD_AN_S_UserMailInfo*)m_cbBuffer;
	ZeroMemory(pUserMailInfo, sizeof(CMD_AN_S_UserMailInfo));

	//构造数据		
	pUserMailInfo->wMailCount = pDBUserMailInfo->wMailCount;
	if (pUserMailInfo->wMailCount > 0)
	{
		CopyMemory(pUserMailInfo->MailInfo, pDBUserMailInfo->MailInfo, pUserMailInfo->wMailCount * sizeof(pUserMailInfo->MailInfo[0]));
	}

	//发送数据
	WORD wPacketSize = sizeof(CMD_AN_S_UserMailInfo)+ pUserMailInfo->wMailCount * sizeof(pUserMailInfo->MailInfo[0]);	
	SendDataToGate(dwContextID, MDM_AN_MESSAGE_SERVICE, SUB_AN_S_USER_MAIL_INFO, pUserMailInfo, wPacketSize, &dwTokenID, 1);

	return true;
}

//邮件完成
bool CAttemperEngineSink::OnDBUserMailFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//获取参数
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL || pTokenParameter->pBindUserItem == NULL) return true;

	//发送数据
	SendDataToGate(dwContextID, MDM_AN_MESSAGE_SERVICE, SUB_AN_S_USER_MAIL_FINISH, NULL, 0, &dwTokenID, 1);

	return true;
}

//邮件更新
bool CAttemperEngineSink::OnDBUserMailUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pUserMailUpdate = (DBO_GR_UserMailUpdate*)pData;

	//获取参数
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL || pTokenParameter->pBindUserItem == NULL) return true;

	//定义变量
	CMD_AN_S_UserMailUpdate UserMailUpdate;
	ZeroMemory(&UserMailUpdate, sizeof(UserMailUpdate));

	//设置变量
	UserMailUpdate.dwMailID = pUserMailUpdate->dwMailID;
	UserMailUpdate.cbMailState = pUserMailUpdate->cbMailState;

	//发送数据
	SendDataToGate(dwContextID, MDM_AN_MESSAGE_SERVICE, SUB_AN_S_USER_MAIL_UPDATE, &UserMailUpdate, sizeof(UserMailUpdate), &dwTokenID, 1);

	return true;
}

//插入邮件
bool CAttemperEngineSink::OnDBUserMailInsert(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pUserMailInsert = (DBO_GR_UserMailInsert*)pData;

	//定义变量
	BYTE cbBuffer[SOCKET_PACKET];
	ZeroMemory(cbBuffer, sizeof(cbBuffer));

	//转换对象
	tagForwardHead* pForwardHead = (tagForwardHead*)cbBuffer;
	CMD_CS_MailNotify* pMailNotify = (CMD_CS_MailNotify*)(pForwardHead + 1);

	//设置头部
	if (pUserMailInsert->dwUserID==0)
	{
		//设置转发头
		pForwardHead->wTarget = FORWARD_TARGET_SERVICE;
		pForwardHead->wItemCount = 1;
		pForwardHead->ForwardItem[0].wServiceID = 0;
		pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;
	}	
	else
	{
		//设置转发头
		pForwardHead->wTarget = FORWARD_TARGET_USER;
		pForwardHead->dwUserID = pUserMailInsert->dwUserID;
		pForwardHead->wItemCount = 1;
		pForwardHead->ForwardItem[0].wServiceID = 0;
		pForwardHead->ForwardItem[0].wServiceModule = SERVICE_MODULE_MESSAGE;
	}

	//设置变量
	pMailNotify->dwUserID = pUserMailInsert->dwUserID;
	pMailNotify->dwMailID = pUserMailInsert->dwMailID;

	//发送数据
	WORD wPacketSize = sizeof(tagForwardHead) + sizeof(CMD_CS_MailNotify);	
	m_pITCPSocketCenter->SendData(MDM_CS_FORWARD_SERVICE, SUB_CS_MAIL_NOTIFY, cbBuffer, wPacketSize, 0);

	return true;
}

//领取附件
bool CAttemperEngineSink::OnDBClaimMailAttach(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pClaimMailAttach = (DBO_GR_ClaimMailAttach*)pData;

	//获取参数
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL || pTokenParameter->pBindUserItem == NULL) return true;

	//用户信息
	auto pGlobalUserItem = (CGlobalUserItem *)pTokenParameter->pBindUserItem;

	//操作成功
	SendOperateSuccess(pGlobalUserItem, SUB_AN_C_CLAIM_MAIL_ATTACH, pClaimMailAttach->lResultCode, NULL, pClaimMailAttach->szAttachGoods);

	return true;
}

//物品信息
bool CAttemperEngineSink::OnDBUserGoodsUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pDBGoodsUpdate = (DBO_GR_GoodsUpdate *)pData;

	//获取参数
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL || pTokenParameter->pBindUserItem == NULL) return true;

	//定义变量
	auto pGoodsUpdate=(CMD_CM_GoodsUpdate*)m_cbBuffer;
	ZeroMemory(pGoodsUpdate, sizeof(CMD_CM_GoodsUpdate));

	//设置变量
	pGoodsUpdate->wGoodsCount = pDBGoodsUpdate->wGoodsCount;
	CopyMemory(pGoodsUpdate->GoodsHoldInfo, pDBGoodsUpdate->GoodsHoldInfo, pGoodsUpdate->wGoodsCount*sizeof(pGoodsUpdate->GoodsHoldInfo[0]));

	//发送数据
	WORD wPacketSize = sizeof(CMD_CM_GoodsUpdate) + pGoodsUpdate->wGoodsCount * sizeof(pGoodsUpdate->GoodsHoldInfo[0]);
	SendDataToGate(dwContextID, MDM_CM_SYSTEM, SUB_CM_GOODS_UPDATE, pGoodsUpdate, wPacketSize, &dwTokenID, 1);

	return true;
}

//财富信息
bool CAttemperEngineSink::OnDBUserWealthUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pDBWealthUpdate = (DBO_GR_WealthUpdate*)pData;

	//获取参数
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL || pTokenParameter->pBindUserItem == NULL) return true;

	//定义变量
	auto pWealthUpdate = (CMD_CM_WealthUpdate*)m_cbBuffer;
	ZeroMemory(pWealthUpdate, sizeof(CMD_CM_WealthUpdate));

	//设置变量
	pWealthUpdate->wItemCount = pDBWealthUpdate->wItemCount;
	CopyMemory(pWealthUpdate->WealthItem, pDBWealthUpdate->WealthItem, pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]));

	//发送数据
	WORD wPacketSize = sizeof(CMD_CM_WealthUpdate) + pWealthUpdate->wItemCount * sizeof(pWealthUpdate->WealthItem[0]);
	SendDataToGate(dwContextID, MDM_CM_SYSTEM, SUB_CM_WEALTH_UPDATE, pWealthUpdate, wPacketSize, &dwTokenID, 1);

	return true;
}

//操作成功
bool CAttemperEngineSink::OnDBOperateSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pOperateSuccess = (DBO_GR_OperateSuccess*)pData;

	//获取参数
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL || pTokenParameter->pBindUserItem == NULL) return true;

	//用户信息
	auto pGlobalUserItem = (CGlobalUserItem*)pTokenParameter->pBindUserItem;

	//操作成功
	SendOperateSuccess(pGlobalUserItem, pOperateSuccess->wRequestCmdID, pOperateSuccess->lErrorCode, pOperateSuccess->szDescribeString, NULL);

	return true;
}

//操作失败
bool CAttemperEngineSink::OnDBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pOperateFailure = (DBO_GR_OperateFailure*)pData;

	//获取参数
	auto pTokenParameter = GetTokenParameter(dwContextID, dwTokenID);
	if (pTokenParameter == NULL || pTokenParameter->pBindUserItem == NULL) return true;

	//用户信息
	auto pGlobalUserItem = (CGlobalUserItem*)pTokenParameter->pBindUserItem;

	//操作失败
	SendOperateFailure(pGlobalUserItem, pOperateFailure->wRequestCmdID, pOperateFailure->lErrorCode, pOperateFailure->szDescribeString);

	return true;
}

//发送请求
bool CAttemperEngineSink::SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize)
{
	//发送数据
	CServiceUnits * pServiceUnits=CServiceUnits::g_pServiceUnits;
	pServiceUnits->m_UIControlSink.PostControlRequest(wRequestID,pData,wDataSize);

	return true;
}

//发送数据
bool CAttemperEngineSink::SendBatchData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)
{
	//变量定义
	CGlobalGateItem * pGlobalGateItem=NULL;

	//变量定义
	DWORD * pdwTokenIDArray=NULL; 
	WORD wCellCount=0,wSendCount=0,wTotalCount=0;

	//查找房间
	do
	{
		//枚举网关 
		pGlobalGateItem=m_GlobalItemManager.EnumGateItem(pGlobalGateItem);
		if (pGlobalGateItem==NULL) break;

		//查找容器
		if (pGlobalGateItem->GetTokenCount()==0) continue;

		//设置大小
		pGlobalGateItem->m_TokenIDArray.SetSize(pGlobalGateItem->GetTokenCount());

		//收集令牌
		WORD wIndex = 0;
		tagTokenParameter* pTokenParameter = NULL;
		do
		{
			auto pTokenCurrent = pTokenParameter;
			pTokenParameter = pGlobalGateItem->EnumTokenID(pTokenCurrent);
			if (pTokenParameter != NULL)
			{
				pGlobalGateItem->m_TokenIDArray.SetAt(wIndex++, pTokenParameter->dwTokenID);
			}

		} while (pTokenParameter!=NULL);

		//设置变量		
		wSendCount=0;
		wCellCount = pGlobalGateItem->m_TokenIDArray.GetCount();
		wTotalCount = pGlobalGateItem->m_TokenIDArray.GetCount();
		pdwTokenIDArray= pGlobalGateItem->m_TokenIDArray.GetData();

		//切分数量
		while ((sizeof(WORD)+ wCellCount *sizeof(DWORD)+wDataSize)>SOCKET_PACKET)
		{
			wCellCount = wCellCount /2;
		}

		//调整数量
		if (wCellCount > TOKEN_COUNT) wCellCount = TOKEN_COUNT;

		//循环发送
		while (wSendCount< wTotalCount)
		{		
			//调整变量
			if (wTotalCount - wSendCount < wCellCount)
			{
				wCellCount= wTotalCount - wSendCount;
			}								

			//发送数据
			SendDataToGate(pGlobalGateItem->GetSocketID(), MAKECMMD(wMainCmdID, SERVICE_MODULE_MESSAGE),wSubCmdID, pData,wDataSize, pdwTokenIDArray + wSendCount, wCellCount);

			//设置数据			
			wSendCount += wCellCount;
		}

		//清除令牌
		pGlobalGateItem->m_TokenIDArray.RemoveAll();

	} while (true);

	return true;
}

//网关发送
bool CAttemperEngineSink::SendDataToGate(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount)
{
	//发送数据
	m_pITCPNetworkEngine->SendData(dwSocketID, MAKECMMD(wMainCmdID,SERVICE_MODULE_MESSAGE),wSubCmdID,pData,wDataSize, pdwTokenID, wTokenCount);

	return true;
}


//清除公告
VOID CAttemperEngineSink::CleanExpireNotice()
{
	//变量定义
	DWORD  dwCurrentTime = (DWORD)time(NULL);

	//删除过期
	auto pMarqueeNoticeItem = m_StockMarqueeNoticeInfo.GetHeadStockItem();
	while (pMarqueeNoticeItem != NULL)
	{
		auto pLastMarqueeNoticeItem = pMarqueeNoticeItem;
		pMarqueeNoticeItem = m_StockMarqueeNoticeInfo.GetNextStockItem(pLastMarqueeNoticeItem);

		//校验时间
		if (pLastMarqueeNoticeItem->dwEndRollTime!=0 && pLastMarqueeNoticeItem->dwEndRollTime <= dwCurrentTime)
		{
			m_StockMarqueeNoticeInfo.RemoveStockItem(pLastMarqueeNoticeItem->dwNoticeID);
		}
	}
}

//令牌参数
tagTokenParameter* CAttemperEngineSink::GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID)
{
	//获取信息
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

	//转发模式
	if (pBindParameter->wServiceModule == SERVICE_MODULE_GATE)
	{
		//查询网关
		CGlobalGateItem* pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem == NULL) return NULL;

		//查询用户
		auto pTokenParameter = pGlobalGateItem->SearchTokenID(dwTokenID);
		if (pTokenParameter == NULL || pTokenParameter->dwTokenID != dwTokenID) return NULL;

		return pTokenParameter;
	}

	return NULL;
}

//操作成功
VOID CAttemperEngineSink::SendOperateSuccess(CGlobalUserItem* pGLobalUserItem, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe, LPCTSTR pszGoodsList)
{

	//变量定义
	CMD_CM_OperateSuccess* pRequestSuccess = (CMD_CM_OperateSuccess*)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer + sizeof(CMD_CM_OperateSuccess), sizeof(m_cbBuffer) - sizeof(CMD_CM_OperateSuccess));

	//设置变量
	pRequestSuccess->lErrorCode = lErrorCode;
	pRequestSuccess->wRequestCmdID = wRequestCmdID;

	//叠加信息
	if (pszDescribe != NULL)  SendPacket.AddPacket(pszDescribe, DTP_CM_DESCRIBE);
	if (pszGoodsList != NULL)  SendPacket.AddPacket(pszGoodsList, DTP_CM_GOODSLIST);

	//查找网关
	auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGLobalUserItem->GetGateID());
	if (pGlobalGateItem != NULL)
	{
		//变量定义
		DWORD dwTokenID = pGLobalUserItem->GetTokenID();
		WORD wDataSize = sizeof(CMD_CM_OperateSuccess) + SendPacket.GetDataSize();

		//发送数据
		SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_CM_SYSTEM, SUB_CM_OPERATE_SUCCESS, pRequestSuccess, wDataSize, &dwTokenID, 1);
	}	

	return;
}

//操作失败
VOID CAttemperEngineSink::SendOperateFailure(CGlobalUserItem* pGLobalUserItem, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe)
{
	//变量定义
	auto pOperateFailure = (CMD_CM_OperateFailure*)m_cbBuffer;
	CSendPacketHelper SendPacket(m_cbBuffer + sizeof(CMD_CM_OperateFailure), sizeof(m_cbBuffer) - sizeof(CMD_CM_OperateFailure));

	//设置变量
	pOperateFailure->lErrorCode = lErrorCode;
	pOperateFailure->wRequestCmdID = wRequestCmdID;

	//叠加信息
	if (pszDescribe != NULL)  SendPacket.AddPacket(pszDescribe, DTP_CM_DESCRIBE);

	//查找网关
	auto pGlobalGateItem = m_GlobalItemManager.SearchGateItem(pGLobalUserItem->GetGateID());
	if (pGlobalGateItem != NULL)
	{
		//变量定义
		DWORD dwTokenID = pGLobalUserItem->GetTokenID();
		WORD wDataSize = sizeof(CMD_CM_OperateSuccess) + SendPacket.GetDataSize();

		//发送数据
		SendDataToGate(pGlobalGateItem->GetSocketID(), MDM_CM_SYSTEM, SUB_CM_OPERATE_FAILURE, pOperateFailure, wDataSize, &dwTokenID, 1);
	}

	return;
}

//邮件服务
bool CAttemperEngineSink::OnTCPNetWorkMainMessageService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID)
{
	//获取参数
	WORD wBindIndex = LOWORD(dwSocketID);
	tagBindParameter* pBindParameter = (m_pBindParameter + wBindIndex);

	switch (wSubCmdID)
	{
	case SUB_AN_C_ENTER_MESSAGE:		//进入服务
	{
		//获取参数
		auto pTokenParameter = GetTokenParameter(dwSocketID,dwTokenID);
		if (pTokenParameter == NULL) return true;

		//连接效验
		ASSERT (pBindParameter->wServiceModule==SERVICE_MODULE_GATE);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return false;

		//查询网关
		CGlobalGateItem * pGlobalGateItem=m_GlobalItemManager.SearchGateItem(pBindParameter->wServiceID);
		if (pGlobalGateItem==NULL) return false;

		//查询用户
		CGlobalUserItem * pGlobalUserItem=m_GlobalItemManager.SearchUserItem(pTokenParameter->dwUserID);
		if (pGlobalUserItem!=NULL)
		{
			//设置变量
			pGlobalUserItem->m_dwTokenID= dwTokenID;
			pGlobalUserItem->m_wGateID=pBindParameter->wServiceID;
		}
		else
		{
			//构造结构
			tagGlobalUserInfo GlobalUserInfo;
			ZeroMemory(&GlobalUserInfo,sizeof(GlobalUserInfo));

			//设置变量
			GlobalUserInfo.dwUserID = pTokenParameter->dwUserID;
			GlobalUserInfo.SocketInfo.dwTokenID = dwTokenID;
			GlobalUserInfo.SocketInfo.wGateID = pBindParameter->wServiceID;			

			//激活用户
			pGlobalUserItem=m_GlobalItemManager.ActiveUserItem(GlobalUserInfo);

		}

		//绑定用户		
		pTokenParameter->pBindUserItem = pGlobalUserItem;

        //进入成功
        SendDataToGate(dwSocketID, MDM_AN_MESSAGE_SERVICE, SUB_AN_S_ENTER_SUCCESS, NULL, 0, &dwTokenID, 1);

		//变量定义
		WORD wPacketSize = 0;

		//清除过期
		CleanExpireNotice();
		
		//遍历公告
		auto pMarqueeNoticeItem = m_StockMarqueeNoticeInfo.GetHeadStockItem();
		while (pMarqueeNoticeItem != NULL)
		{
			auto pLastMarqueeNoticeItem = pMarqueeNoticeItem;
			pMarqueeNoticeItem = m_StockMarqueeNoticeInfo.GetNextStockItem(pLastMarqueeNoticeItem);

			//设置变量				
			CopyMemory(&m_cbBuffer[wPacketSize], pLastMarqueeNoticeItem, sizeof(tagMarqueeNoticeInfo));

			//设置大小
			wPacketSize += sizeof(tagMarqueeNoticeInfo);

			//退出判断
			if (wPacketSize + sizeof(tagMarqueeNoticeInfo) >= sizeof(m_cbBuffer)) break;
		}

		//推送消息
		if (wPacketSize > 0)
		{
			SendDataToGate(dwSocketID, MDM_AN_MESSAGE_SERVICE, SUB_AN_S_MARQUEE_NOTICE, m_cbBuffer, wPacketSize, &dwTokenID, 1);
		}

		//汇总用户
		if (m_bCollectUser == true)
		{
			//变量定义
			CMD_CS_C_EnterMessage EnterMessage;
			ZeroMemory(&EnterMessage, sizeof(EnterMessage));

			//设置变量
			EnterMessage.dwUserID = pTokenParameter->dwUserID;

			//发送消息
			ASSERT(m_pITCPSocketCenter != NULL);
			m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_ENTER_MESSAGE, &EnterMessage, sizeof(EnterMessage), 0);
		}

		return true;
	}
	case SUB_AN_C_LEAVE_MESSAGE:		//离开服务
	{
		//获取参数
		auto pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return true;

		//连接效验
		ASSERT (pBindParameter->wServiceModule==SERVICE_MODULE_GATE);
		if (pBindParameter->wServiceModule != SERVICE_MODULE_GATE) return false;

		//汇总用户
		if (m_bCollectUser == true)
		{
			auto pGlobalUserItem = (CGlobalUserItem *)pTokenParameter->pBindUserItem;

			//变量定义
			CMD_CS_C_LeaveMessage LeaveMessage;
			ZeroMemory(&LeaveMessage, sizeof(LeaveMessage));

			//设置变量
			LeaveMessage.dwUserID = pTokenParameter->dwUserID;

			//发送消息
			ASSERT(m_pITCPSocketCenter != NULL);
			m_pITCPSocketCenter->SendData(MDM_CS_USER_COLLECT, SUB_CS_C_LEAVE_MESSAGE, &LeaveMessage, sizeof(LeaveMessage), 0);
		}

		//解绑用户
		pTokenParameter->pBindUserItem = NULL;

		//删除用户
		m_GlobalItemManager.DeleteUserItem(pTokenParameter->dwUserID);

		return true;
	}
	case SUB_AN_C_USER_QUERY_MAIL:		//查询邮件
	{
		//参数校验
		ASSERT (wDataSize == sizeof(CMD_AN_C_UserQueryMail));
		if (wDataSize != sizeof(CMD_AN_C_UserQueryMail)) return true;

		//获取参数
		auto pTokenParameter = GetTokenParameter(dwSocketID,dwTokenID);
		if (pTokenParameter == NULL) return true;

		//提取数据
		CMD_AN_C_UserQueryMail* pUserQueryMail = (CMD_AN_C_UserQueryMail*)pData;

		//构造数据
		DBR_GR_UserQueryMail UserQueryMail;
		ZeroMemory(&UserQueryMail, sizeof(UserQueryMail));

		//设置变量
		UserQueryMail.dwUserID = pTokenParameter->dwUserID;
		UserQueryMail.dwLatestMailID = pUserQueryMail->dwLatestMailID;			

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_QUERY_MAIL, UserQueryMail.dwUserID, dwSocketID, dwTokenID, &UserQueryMail, sizeof(UserQueryMail));

		return true;
	}
	case SUB_AN_C_USER_VIEW_MAIL:		//查看邮件
	{
		//参数校验
		ASSERT(wDataSize == sizeof(CMD_AN_C_UserViewMail));
		if (wDataSize != sizeof(CMD_AN_C_UserViewMail)) return true;

		//获取参数
		auto pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return true;

		//提取数据
		CMD_AN_C_UserViewMail* pUserQueryMail = (CMD_AN_C_UserViewMail*)pData;

		//构造数据
		DBR_GR_UserModifyMail UserModifyMail;
		ZeroMemory(&UserModifyMail, sizeof(UserModifyMail));

		//设置变量
		UserModifyMail.dwUserID = pTokenParameter->dwUserID;
		UserModifyMail.dwMailID = pUserQueryMail->dwMailID;
		UserModifyMail.cbMailState = MAIL_STATE_HAVETOSEE;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_MODIFY_MAIL, UserModifyMail.dwUserID, dwSocketID, dwTokenID, &UserModifyMail, sizeof(UserModifyMail));

		return true;
	}
	case SUB_AN_C_CLAIM_MAIL_ATTACH:	//领取附件
	{
		//参数校验
		ASSERT(wDataSize == sizeof(CMD_AN_C_ClaimMailAttach));
		if (wDataSize != sizeof(CMD_AN_C_ClaimMailAttach)) return true;

		//获取参数
		auto pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return true;

		//提取数据
		CMD_AN_C_ClaimMailAttach* pClaimMailAttach = (CMD_AN_C_ClaimMailAttach*)pData;

		//构造数据
		DBR_GR_ClaimMailAttach ClaimMailAttach;
		ZeroMemory(&ClaimMailAttach, sizeof(ClaimMailAttach));

		//设置变量
		ClaimMailAttach.dwUserID = pTokenParameter->dwUserID;
		ClaimMailAttach.dwMailID = pClaimMailAttach->dwMailID;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_CLAIM_MAIL_ATTACH, ClaimMailAttach.dwUserID, dwSocketID, dwTokenID, &ClaimMailAttach, sizeof(ClaimMailAttach));

		return true;
	}
	case SUB_AN_C_USER_DELETE_MAIL:		//删除邮件
	{
		//参数校验
		ASSERT(wDataSize == sizeof(CMD_AN_C_UserDeleteMail));
		if (wDataSize != sizeof(CMD_AN_C_UserDeleteMail)) return true;

		//获取参数
		auto pTokenParameter = GetTokenParameter(dwSocketID, dwTokenID);
		if (pTokenParameter == NULL) return true;

		//提取数据
		CMD_AN_C_UserDeleteMail* pUserDeleteMail = (CMD_AN_C_UserDeleteMail*)pData;

		//构造数据
		DBR_GR_UserDeleteMail DeleteMail;
		ZeroMemory(&DeleteMail, sizeof(DeleteMail));

		//设置变量
		DeleteMail.dwUserID = pTokenParameter->dwUserID;
		DeleteMail.dwMailID = pUserDeleteMail->dwMailID;

		//投递请求
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_USER_DELETE_MAIL, DeleteMail.dwUserID, dwSocketID, dwTokenID, &DeleteMail, sizeof(DeleteMail));

		return true;
	}
	}

	return false;
}

//配置日志
bool CAttemperEngineSink::InitLoggerService()
{
	//使能设置
	g_TimeLoggerService->EnableLocal(false);
	g_TimeLoggerService->EnableRemote(true);
	g_TimeLoggerService->EnablePrefix(false);
	g_TimeLoggerService->EnableSufix(true);

	//配置信息
	g_TimeLoggerService->SetFormat(TEXT("%Y%m%d"));
	g_TimeLoggerService->SetConcatStr(TEXT("_"));
	g_TimeLoggerService->SetSeparator(TEXT("|"));
	g_TimeLoggerService->SetLoggerInfo(TEXT("ServiceLog"), MESSAGE_SERVER_LOG_FILE, m_pInitParameter->m_szLoggerPath);

	//启动服务
	if (g_TimeLoggerService->StartService() == false)
	{
		ASSERT(FALSE);

		return false;
	}

	//创建日志
	g_TimeLoggerService->InitFormat(MESSAGE_SERVER_LOG_FILE, TEXT("%Y%m%d"), new CLogByDay());

	return true;
}
//////////////////////////////////////////////////////////////////////////
