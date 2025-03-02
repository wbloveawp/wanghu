#include "Stdafx.h"
#include "ModuleLogon.h"
#include "ModuleServer.h"

//////////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CModuleLogon::CModuleLogon(IModuleItemSink* pIModuleItemSink):
	m_pIModuleItemSink(pIModuleItemSink)
{
	//设置模块
	SetServiceModule(SERVICE_MODULE_LOGON);

	//设置变量
	ZeroMemory(&m_BufferPacket,sizeof(m_BufferPacket));	
}

//析构函数
CModuleLogon::~CModuleLogon()
{
}

//关闭事件
bool CModuleLogon::OnEventVirtualShut()
{
	//设置变量
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));

	//投递消息	
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_LOGON_LINK_SHUT, 0, 0);

	//构造消息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 与登录服务器的虚拟连接断开了"), m_pIModuleItemSink->GetGlobalUserData()->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(255, 0, 0));

	return true;
}

//连接事件
bool CModuleLogon::OnEventVirtualLink(LONG lErrorCode)
{
	//连接失败
	if (lErrorCode != 0)
	{
		//投递消息	
		ASSERT(m_pIModuleItemSink != NULL);
		m_pIModuleItemSink->PostWindowMessage(WM_LOGON_LINK_FAILURE, 0, 0);

		return true;
	}

	//发送数据
	if (m_BufferPacket.wMainCmdID != 0 && m_BufferPacket.wSubCmdID != 0)
	{
		SendVirtualData(m_BufferPacket.wMainCmdID, m_BufferPacket.wSubCmdID, m_BufferPacket.cbDataBuffer, m_BufferPacket.wDataSize);
	}	

	return true;
}

//删除时间
bool CModuleLogon::KillModuleTimer(UINT nTimerID)
{
	//投递消息
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->KillGameTimer(GetTimerStart() + nTimerID);

	return true;
}

//设置时间
bool CModuleLogon::SetModuleTimer(UINT nTimerID, UINT nElapse)
{
	//投递消息
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->SetGameTimer(GetTimerStart() + nTimerID, nElapse);

	return true;
}

//时钟事件
bool CModuleLogon::OnEventTimer(UINT uTimerID, WPARAM wBindParam)
{
	return true;
}

//读取事件
bool CModuleLogon::OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_MB_LOGON:			//登录处理
	{
		return OnSocketMainLogon(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_MB_SERVER_LIST:	//房间列表
	{
		return OnSocketMainServerList(Command.wSubCmdID, pData, wDataSize);
	}
	case MDM_MB_USER_SERVICE:	//用户服务
	{
		return OnSocketMainUserService(Command.wSubCmdID, pData, wDataSize);
	}
	}

	return true;
}

//登录处理
bool CModuleLogon::OnSocketMainLogon(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_MB_LOGON_SUCCESS:	//登录成功
	{
		return OnSocketSubLogonSuccess(pData, wDataSize);
	}
	case SUB_MB_LOGON_FAILURE:	//登录失败
	{
		return OnSocketSubLogonFailure(pData, wDataSize);
	}
	case SUB_MB_LOGON_FINISH:	//登录完成
	{
		return OnSocketSubLogonFinish(pData, wDataSize);
	}
	case SUB_MB_UPDATE_NOTIFY:	//升级提示
	{
		return OnSocketSubUpdateNotify(pData, wDataSize);
	}
	}

	return true;
}

//列表处理
bool CModuleLogon::OnSocketMainServerList(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_MB_LIST_SERVER:	//房间列表
	{
		//效验参数
		ASSERT(wDataSize % sizeof(tagPCGameServer) == 0);
		if (wDataSize % sizeof(tagPCGameServer) != 0) return false;

		//变量定义
		WORD wItemCount = wDataSize / sizeof(tagPCGameServer);
		auto  pGameServer = (tagPCGameServer*)(pData);

		return true;
	}
	}

	return true;
}

//用户服务
bool CModuleLogon::OnSocketMainUserService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	return true;
}

//登录失败
bool CModuleLogon::OnSocketSubLogonFailure(VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pLogonFailure = (CMD_MB_LogonFailure*)pData;

	ASSERT(m_pIModuleItemSink);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 登录失败,%s!"), pGlobalUserData->szAccounts, pLogonFailure->szDescribeString);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	//投递消息		
	m_pIModuleItemSink->PostWindowMessage(WM_LOGON_LOGON_FAILURE, 0, 0);

	return true;
}

//登录成功
bool CModuleLogon::OnSocketSubLogonSuccess(VOID* pData, WORD wDataSize)
{
	//提取数据
	auto pLogonSuccess = (CMD_MB_LogonSuccess*)pData;

	//用户数据
	ASSERT (m_pIModuleItemSink!=NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//基础信息
	pGlobalUserData->dwUserID = pLogonSuccess->dwUserID;
	pGlobalUserData->dwGameID = pLogonSuccess->dwGameID;
	pGlobalUserData->wFaceID = pLogonSuccess->wFaceID;
	pGlobalUserData->cbGender = pLogonSuccess->cbGender;
	pGlobalUserData->dwCustomID = pLogonSuccess->dwCustomID;	
	StringCchCopy(pGlobalUserData->szNickName,CountArray(pGlobalUserData->szNickName), pLogonSuccess->szNickName);
	StringCchCopy(pGlobalUserData->szAccounts, CountArray(pGlobalUserData->szAccounts), pLogonSuccess->szAccounts);
	StringCchCopy(pGlobalUserData->szLogonPass, CountArray(pGlobalUserData->szLogonPass), pLogonSuccess->szLogonPass);
	StringCchCopy(pGlobalUserData->szInsurePass, CountArray(pGlobalUserData->szInsurePass), pLogonSuccess->szInsurePass);
	
	//经验等级
	pGlobalUserData->dwExperience = pLogonSuccess->dwExperience;

	//会员资料
	pGlobalUserData->wMasterOrder = pLogonSuccess->wMasterOrder;
	pGlobalUserData->dwMemberPoint = pLogonSuccess->dwMemberPoint;
	pGlobalUserData->dwMemberOverTime = pLogonSuccess->dwMemberOverTime;

	//财富信息
	pGlobalUserData->lUserGold = pLogonSuccess->lUserGold;
	pGlobalUserData->lUserCard = pLogonSuccess->lUserCard;
	pGlobalUserData->lUserInsure = pLogonSuccess->lUserInsure;

	//行为掩码
	pGlobalUserData->dwActionMaskEver = pLogonSuccess->dwActionMaskEver;
	pGlobalUserData->dwActionMaskPerDay = pLogonSuccess->dwActionMaskPerDay;
	pGlobalUserData->dwActionMaskPerWeek = pLogonSuccess->dwActionMaskPerWeek;

	//锁定信息
	pGlobalUserData->wLockServerID = pLogonSuccess->wLockServerID;

	//上级信息
	pGlobalUserData->wParentFaceID = pLogonSuccess->wParentFaceID;
	pGlobalUserData->dwParentGameID = pLogonSuccess->dwParentGameID;
	StringCchCopy(pGlobalUserData->szParentNickName, CountArray(pGlobalUserData->szParentNickName), pLogonSuccess->szParentNickName);

	//扩展信息
	if (wDataSize > sizeof(CMD_MB_LogonSuccess))
	{
		//变量定义
		VOID* pDataBuffer = NULL;
		tagDataDescribe DataDescribe;
		CRecvPacketHelper RecvPacket(pLogonSuccess + 1, wDataSize - sizeof(CMD_MB_LogonSuccess));

		//读取信息
		while (true)
		{
			//提取数据
			pDataBuffer = RecvPacket.GetData(DataDescribe);
			if (DataDescribe.wDataDescribe == DTP_NULL) break;

			//提取数据
			switch (DataDescribe.wDataDescribe)
			{
			case DTP_MB_MOBILE_PHONE:		//手机号码
			{
				ASSERT(DataDescribe.wDataSize <= sizeof(pGlobalUserData->szMobilePhone));
				if (DataDescribe.wDataSize <= sizeof(pGlobalUserData->szMobilePhone))
				{
					pGlobalUserData->szMobilePhone[CountArray(pGlobalUserData->szMobilePhone) - 1L] = 0;
					StringCchCopy(pGlobalUserData->szMobilePhone, CountArray(pGlobalUserData->szMobilePhone), (LPCTSTR)pDataBuffer);
				}

				break;
			}
			case DTP_MB_LOGON_TOKEN:		//登录令牌
			{
				ASSERT(DataDescribe.wDataSize <= sizeof(pGlobalUserData->szLogonToken));
				if (DataDescribe.wDataSize <= sizeof(pGlobalUserData->szLogonToken))
				{
					pGlobalUserData->szLogonToken[CountArray(pGlobalUserData->szLogonToken) - 1L] = 0;
					StringCchCopy(pGlobalUserData->szLogonToken, CountArray(pGlobalUserData->szLogonToken), (LPCTSTR)pDataBuffer);
				}

				break;
			}
			}
		}
	}	

	return true;
}

//登录完成
bool CModuleLogon::OnSocketSubLogonFinish(VOID* pData, WORD wDataSize)
{
	ASSERT(m_pIModuleItemSink);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 平台登录成功!"),pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	//投递消息	
	m_pIModuleItemSink->PostWindowMessage(WM_LOGON_LOGON_FINISH, 0, 0);

	return true;
}

//更新提示
bool CModuleLogon::OnSocketSubUpdateNotify(VOID* pData, WORD wDataSize)
{
	ASSERT(m_pIModuleItemSink);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 登录失败,版本不一致!"), pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	//投递消息	
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_LOGON_LOGON_FAILURE, 0, 0);

	return true;
}

//游客登录
bool CModuleLogon::LogonByVisitor(LPCTSTR pszMachineID)
{
	//构造结构
	CMD_MB_LogonVisitor LogonVisitor;
	ZeroMemory(&LogonVisitor, sizeof(LogonVisitor));

	//设置变量
	LogonVisitor.wMarketID = 1;
	LogonVisitor.dwAppVersion = 16842752;
	LogonVisitor.dwPlazaVersion = 16842752;
	LogonVisitor.dwDeviceKind = DEVICE_KIND_ANDROID;
	StringCchCopy(LogonVisitor.szMachineID, CountArray(LogonVisitor.szMachineID) ,pszMachineID);

	//获取机器码
	CWHService::GetMachineID(LogonVisitor.szMachineID);

	//校验状态
	if (GetLinkStatus() == true)
	{
		SendVirtualData(MDM_MB_LOGON, SUB_MB_LOGON_VISITOR, &LogonVisitor, sizeof(LogonVisitor));
	}
	else
	{
		//缓冲数据
		m_BufferPacket.wMainCmdID = MDM_MB_LOGON;
		m_BufferPacket.wSubCmdID = SUB_MB_LOGON_VISITOR;
		m_BufferPacket.wDataSize = sizeof(LogonVisitor);
		CopyMemory(m_BufferPacket.cbDataBuffer, &LogonVisitor, sizeof(LogonVisitor));

		//创建连接
		CreateVirtualLink(INVALID_WORD, 0, NULL, NULL, 0);
	}

	return true;
}

//账号登录
bool CModuleLogon::LogonByAccounts(LPCTSTR pszAccounts, LPCTSTR pszLogonPass)
{
	//构造结构
	CMD_MB_LogonAccounts LogonAccounts;
	ZeroMemory(&LogonAccounts,sizeof(LogonAccounts));

	//设置变量
	LogonAccounts.wMarketID = 1;
	LogonAccounts.dwAppVersion = 16842752;
	LogonAccounts.dwPlazaVersion = 16842752;
	LogonAccounts.dwDeviceKind = DEVICE_KIND_ANDROID;	
	StringCchCopy(LogonAccounts.szAccounts,CountArray(LogonAccounts.szAccounts), pszAccounts);
	
	//MD5加密
	CMD5Encrypt::EncryptData(pszLogonPass, LogonAccounts.szPassword);

	//获取机器码
	CWHService::GetMachineID(LogonAccounts.szMachineID);

	//获取指针
	ASSERT(m_pIModuleItemSink);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//拷贝账号
	StringCchCopy(pGlobalUserData->szAccounts, CountArray(pGlobalUserData->szAccounts), pszAccounts);

	//校验状态
	if (GetLinkStatus() == true)
	{
		SendVirtualData(MDM_MB_LOGON, SUB_MB_LOGON_ACCOUNTS, &LogonAccounts, sizeof(LogonAccounts));
	}
	else
	{
		//缓冲数据
		m_BufferPacket.wMainCmdID = MDM_MB_LOGON;
		m_BufferPacket.wSubCmdID = SUB_MB_LOGON_ACCOUNTS;		
		m_BufferPacket.wDataSize = sizeof(LogonAccounts);
		CopyMemory(m_BufferPacket.cbDataBuffer, &LogonAccounts, sizeof(LogonAccounts));

		//创建连接
		CreateVirtualLink(INVALID_WORD,0,NULL,NULL,0);
	}

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 正在登录平台..."), pszAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

//重置模块
VOID CModuleLogon::ResetModule()
{
	//设置变量
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));
}
//////////////////////////////////////////////////////////////////////////////////////////////
