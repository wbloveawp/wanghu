#include "StdAfx.h"
#include "OptionParameter.h"

//////////////////////////////////////////////////////////////////////////

//控件颜色
#define PARAMETER_CHAT_TX			RGB(0,0,0)							//聊天颜色

//////////////////////////////////////////////////////////////////////////

//静态变量
CParameterGlobal * CParameterGlobal::m_pParameterGlobal=NULL;			//全局配置

//////////////////////////////////////////////////////////////////////////

//构造函数
CParameterGame::CParameterGame()
{
	//默认参数
	DefaultParameter();

	//属性变量
	ZeroMemory(m_szRegKeyName,sizeof(m_szRegKeyName));

	return;
}

//析构函数
CParameterGame::~CParameterGame()
{
}

//加载参数
VOID CParameterGame::LoadParameter()
{
	//变量定义
	CRegKeyAide RegOptionItem;

	//配置表项
	if (RegOptionItem.OpenRegKey(m_szRegKeyName,REG_GAME_OPTION,false)==true)
	{
		//胜率限制
		m_wLessWinRate=(WORD)RegOptionItem.GetValue(TEXT("LessWinRate"),m_wLessWinRate);
		m_bLimitWinRate=RegOptionItem.GetValue(TEXT("LimitWinRate"),m_bLimitWinRate)?true:false;

		//逃率限制
		m_wMaxFleeRate=(WORD)RegOptionItem.GetValue(TEXT("MaxFleeRate"),m_wMaxFleeRate);
		m_bLimitFleeRate=RegOptionItem.GetValue(TEXT("LimitFleeRate"),m_bLimitFleeRate)?true:false;
		
		//积分限制
		m_lMaxGameScore=RegOptionItem.GetValue(TEXT("MaxGameScore"),m_lMaxGameScore);
		m_lLessGameScore=RegOptionItem.GetValue(TEXT("LessGameScore"),m_lLessGameScore);
		m_bLimitGameScore=RegOptionItem.GetValue(TEXT("LimitGameScore"),m_bLimitGameScore)?true:false;
	}

	return;
}

//保存参数
VOID CParameterGame::SaveParameter()
{
	//变量定义
	CRegKeyAide RegOptionItem;

	//配置表项
	if (RegOptionItem.OpenRegKey(m_szRegKeyName,REG_GAME_OPTION,true)==true)
	{
		//胜率限制
		RegOptionItem.WriteValue(TEXT("LessWinRate"),m_wLessWinRate);
		RegOptionItem.WriteValue(TEXT("LimitWinRate"),m_bLimitWinRate);

		//逃率限制
		RegOptionItem.WriteValue(TEXT("MaxFleeRate"),m_wMaxFleeRate);
		RegOptionItem.WriteValue(TEXT("LimitFleeRate"),m_bLimitFleeRate);

		//积分限制
		RegOptionItem.WriteValue(TEXT("MaxGameScore"),m_lMaxGameScore);
		RegOptionItem.WriteValue(TEXT("LessGameScore"),m_lLessGameScore);
		RegOptionItem.WriteValue(TEXT("LimitGameScore"),m_bLimitGameScore);
	}

	return;
}

//默认参数
VOID CParameterGame::DefaultParameter()
{
	//胜率限制
	m_wLessWinRate=0;
	m_bLimitWinRate=false;

	//逃率限制
	m_wMaxFleeRate=5000;
	m_bLimitFleeRate=false;

	//积分限制
	m_bLimitGameScore=false;
	m_lMaxGameScore=2147483647L;
	m_lLessGameScore=-2147483646L;

	return;
}

//配置参数
bool CParameterGame::InitParameter(LPCTSTR pszProcessName)
{
	//构造键名
	WORD wNameIndex=0;
	while (wNameIndex<(CountArray(m_szRegKeyName)-1))
	{
		//终止判断
		if (pszProcessName[wNameIndex]==0) break;
		if (pszProcessName[wNameIndex]==TEXT('.')) break;

		//设置字符
		WORD wCurrentIndex=wNameIndex++;
		m_szRegKeyName[wCurrentIndex]=pszProcessName[wCurrentIndex];
	}

	//设置变量
	m_szRegKeyName[wNameIndex]=0;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CParameterServer::CParameterServer()
{
	//默认参数
	DefaultParameter();

	return;
}

//析构函数
CParameterServer::~CParameterServer()
{
}

//默认参数
VOID CParameterServer::DefaultParameter()
{
	//配置变量
	m_bTakePassword=false;
	ZeroMemory(m_szPassword,sizeof(m_szPassword));

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CParameterGlobal::CParameterGlobal()
{
	//默认参数
	DefaultParameter();

	//设置对象
	ASSERT(m_pParameterGlobal==NULL);
	if (m_pParameterGlobal==NULL) m_pParameterGlobal=this;

	return;
}

//析构函数
CParameterGlobal::~CParameterGlobal()
{
	//变量定义
	POSITION PositionGame=m_ParameterGameMap.GetStartPosition();
	POSITION PositionServer=m_ParameterGameMap.GetStartPosition();

	//删除配置
	while (PositionGame!=NULL)
	{
		//获取子项
		WORD wKindID=0;
		CParameterGame * pParameterGame=NULL;
		m_ParameterGameMap.GetNextAssoc(PositionGame,wKindID,pParameterGame);

		//删除配置
		SafeDelete(pParameterGame);
	}

	//删除配置
	while (PositionServer!=NULL)
	{
		//获取子项
		WORD wServerID=0;
		CParameterServer * pParameterServer=NULL;
		m_ParameterServerMap.GetNextAssoc(PositionServer,wServerID,pParameterServer);

		//删除配置
		SafeDelete(pParameterServer);
	}

	//删除索引
	m_ParameterGameMap.RemoveAll();
	m_ParameterServerMap.RemoveAll();

	//释放对象
	ASSERT(m_pParameterGlobal==this);
	if (m_pParameterGlobal==this) m_pParameterGlobal=NULL;

	return;
}

//加载参数
VOID CParameterGlobal::LoadParameter()
{
	//变量定义
	CRegKeyAide RegOptionItem;

	//界面信息
	if (RegOptionItem.OpenRegKey(REG_INTERFACE_INFO,false)==true)
	{
		//界面配置
		m_bSutraView=RegOptionItem.GetValue(TEXT("SutraView"),m_bSutraView)?true:false;
		m_bSalienceTable=RegOptionItem.GetValue(TEXT("SalienceTable"),m_bSalienceTable)?true:false;
		m_bFullScreenBrowse=RegOptionItem.GetValue(TEXT("FullScreenBrowse"),m_bFullScreenBrowse)?true:false;

		//界面资源
#ifndef _DEBUG
		RegOptionItem.GetString(TEXT("SkinDirectory"),m_szSkinDirectory,CountArray(m_szSkinDirectory));
		RegOptionItem.GetString(TEXT("ResourceModule"),m_szResourceModule,CountArray(m_szResourceModule));
#else
 		RegOptionItem.GetString(TEXT("SkinDirectory"),m_szSkinDirectory,CountArray(m_szSkinDirectory));
 		RegOptionItem.GetString(TEXT("ResourceModule_Debug"),m_szResourceModule,CountArray(m_szResourceModule));
#endif

		//参数调整
		if (m_szSkinDirectory[0]==0) lstrcpyn(m_szSkinDirectory,SKIN_RESOURCE,CountArray(m_szSkinDirectory));
		if (m_szResourceModule[0]==0) lstrcpyn(m_szResourceModule,RESOURCE_MODELE,CountArray(m_szResourceModule));
	}

	//控制变量
	if (RegOptionItem.OpenRegKey(REG_GOBAL_OPTION,false)==true)
	{
		//登录模式
		m_LogonMode=(enLogonMode)RegOptionItem.GetValue(TEXT("LogonMode"),m_LogonMode);
		switch (m_LogonMode)
		{
		case LogonMode_GameID:
		case LogonMode_Accounts: 
		case LogonMode_BBS_Accounts:{ break; }
		default: { m_LogonMode=LogonMode_Accounts; }
		}

		//帐号规则
		m_AcountsRule=(enAcountsRule)RegOptionItem.GetValue(TEXT("AcountsRule"),m_AcountsRule);
		switch (m_AcountsRule)
		{
		case AcountsRule_None:
		case AcountsRule_Accounts:
		case AcountsRule_AccountsAndPass: break;
		default: { m_AcountsRule=AcountsRule_Accounts; break; }
		}

		//线路类型
		m_NetworkKind=(enNetworkKind)RegOptionItem.GetValue(TEXT("NetworkKind"),m_NetworkKind);
		switch (m_NetworkKind)
		{
		case NetworkKind_DianXin:
		case NetworkKind_WangTong: { break; }
		default: { m_NetworkKind=NetworkKind_Default; }
		}

		//邀请模式
		m_InviteMode=(enInviteMode)RegOptionItem.GetValue(TEXT("InviteMode"),m_InviteMode);
		switch (m_InviteMode)
		{
		case InviteMode_None:
		case InviteMode_Friend: { break; }
		default: { m_InviteMode=InviteMode_All; }
		}

		//消息模式
		m_MessageMode=(enMessageMode)RegOptionItem.GetValue(TEXT("MessageMode"),m_MessageMode);
		switch (m_MessageMode)
		{
		case MessageMode_ShowAll:
		case MessageMode_OnlyFriend: { break; }
		default: { m_MessageMode=MessageMode_HideDetest; }
		}

		//自动加入
		m_ActionHitAutoJoin=(enInterfaceAction)RegOptionItem.GetValue(TEXT("ActionHitAutoJoin"),m_ActionHitAutoJoin);
		switch (m_ActionHitAutoJoin)
		{
		case InterfaceAction_ShowSearchDlg: { break; }
		default: { m_ActionHitAutoJoin=InterfaceAction_SearchTable; }
		}

		//双击列表
		m_ActionLeftDoubleList=(enInterfaceAction)RegOptionItem.GetValue(TEXT("ActionLeftDoubleList"),m_ActionLeftDoubleList);
		switch (m_ActionLeftDoubleList)
		{
		case InterfaceAction_Orientation:
		case InterfaceAction_ShowUserInfo: { break; }
		default: { m_ActionLeftDoubleList=InterfaceAction_SendWhisper; }
		}

		//颜色设置
		m_crChatTX=RegOptionItem.GetValue(TEXT("ChatTX"),m_crChatTX);

		//房间配置
		m_bLimitDetest=RegOptionItem.GetValue(TEXT("LimitDetest"),m_bLimitDetest)?true:false;
		m_bLimitSameIP=RegOptionItem.GetValue(TEXT("LimitSameIP"),m_bLimitSameIP)?true:false;
		m_bNotifyUserInOut=RegOptionItem.GetValue(TEXT("NotifyUserInOut"),m_bNotifyUserInOut)?true:false;
		m_bNotifyFriendCome=RegOptionItem.GetValue(TEXT("NotifyFriendCome"),m_bNotifyFriendCome)?true:false;

		//系统配置
		m_bAllowSound=RegOptionItem.GetValue(TEXT("AllowSound"),m_bAllowSound)?true:false;
		m_bAutoSitDown=RegOptionItem.GetValue(TEXT("AutoSitDown"),m_bAutoSitDown)?true:false;
		m_bAutoShowWhisper=RegOptionItem.GetValue(TEXT("AutoShowWhisper"),m_bAutoShowWhisper)?true:false;
		m_bSaveWhisperChat=RegOptionItem.GetValue(TEXT("SaveWhisperChat"),m_bSaveWhisperChat)?true:false;
		m_bSendWhisperByEnter=RegOptionItem.GetValue(TEXT("SendWhisperByEnter"),m_bSendWhisperByEnter)?true:false;
		m_bShowSafePrompt=RegOptionItem.GetValue(TEXT("ShowSafePrompt"),m_bShowSafePrompt)?true:false;

		//机器信息
		RegOptionItem.GetString(TEXT("SrMachineID"),m_szMachineID,CountArray(m_szMachineID));
		RegOptionItem.GetString(TEXT("XorMachineID"),m_szXorMachineID,CountArray(m_szXorMachineID));

		//热键设置
		m_wBossHotKey=(WORD)RegOptionItem.GetValue(TEXT("BossHotKey"),m_wBossHotKey);
		m_wWhisperHotKey=(WORD)RegOptionItem.GetValue(TEXT("WhisperHotKey"),m_wWhisperHotKey);
	}

	//代理信息
	if (RegOptionItem.OpenRegKey(REG_PROXY_SERVER_INFO,false)==true)
	{
		//重置参数
		m_ProxyServerInfo.szUserName[0]=0;
		m_ProxyServerInfo.szPassword[0]=0;
		m_ProxyServerInfo.szHttpDomain[0]=0;
		m_ProxyServerInfo.szProxyServer[0]=0;

		//读取参数
		m_ProxyServerInfo.wProxyPort=(WORD)RegOptionItem.GetValue(TEXT("ProxyPort"),0);
		m_ProxyServerType=(enProxyServerType)RegOptionItem.GetValue(TEXT("ProxyType"),ProxyType_None);
		RegOptionItem.GetString(TEXT("UserName"),m_ProxyServerInfo.szUserName,CountArray(m_ProxyServerInfo.szUserName));
		RegOptionItem.GetString(TEXT("Password"),m_ProxyServerInfo.szPassword,CountArray(m_ProxyServerInfo.szPassword));
		RegOptionItem.GetString(TEXT("HttpDomain"),m_ProxyServerInfo.szHttpDomain,CountArray(m_ProxyServerInfo.szHttpDomain));
		RegOptionItem.GetString(TEXT("ProxyServer"),m_ProxyServerInfo.szProxyServer,CountArray(m_ProxyServerInfo.szProxyServer));

		//代理类型
		switch (m_ProxyServerType)
		{
		case ProxyType_None:
		case ProxyType_Http:
		case ProxyType_Socks4:
		case ProxyType_Socks5: { break; }
		default: { m_ProxyServerType=ProxyType_None; break; }
		}
	}

	return;
}

//保存参数
VOID CParameterGlobal::SaveParameter()
{
	//变量定义
	CRegKeyAide RegOptionItem;
	POSITION PositionGame=m_ParameterGameMap.GetStartPosition();

	//界面信息
	if (RegOptionItem.OpenRegKey(REG_INTERFACE_INFO,true)==true)
	{
		//界面配置
		RegOptionItem.WriteValue(TEXT("SutraView"),m_bSutraView);
		RegOptionItem.WriteValue(TEXT("SalienceTable"),m_bSalienceTable);
		RegOptionItem.WriteValue(TEXT("FullScreenBrowse"),m_bFullScreenBrowse);

		//界面资源
#ifndef _DEBUG
		RegOptionItem.WriteString(TEXT("SkinDirectory"),m_szSkinDirectory);
		RegOptionItem.WriteString(TEXT("ResourceModule"),m_szResourceModule);
#else
		RegOptionItem.WriteString(TEXT("SkinDirectory"),m_szSkinDirectory);
		RegOptionItem.WriteString(TEXT("ResourceModule_Debug"),m_szResourceModule);
#endif

	}

	//控制变量
	if (RegOptionItem.OpenRegKey(REG_GOBAL_OPTION,true)==true)
	{
		//颜色设置
		RegOptionItem.WriteValue(TEXT("ChatTX"),m_crChatTX);

		//登录配置
		RegOptionItem.WriteValue(TEXT("LogonMode"),m_LogonMode);
		RegOptionItem.WriteValue(TEXT("AcountsRule"),m_AcountsRule);
		RegOptionItem.WriteValue(TEXT("NetworkKind"),m_NetworkKind);

		//房间配置
		RegOptionItem.WriteValue(TEXT("InviteMode"),m_InviteMode);
		RegOptionItem.WriteValue(TEXT("MessageMode"),m_MessageMode);
		RegOptionItem.WriteValue(TEXT("ActionHitAutoJoin"),m_ActionHitAutoJoin);
		RegOptionItem.WriteValue(TEXT("ActionLeftDoubleList"),m_ActionLeftDoubleList);

		//房间配置
		RegOptionItem.WriteValue(TEXT("LimitDetest"),m_bLimitDetest);
		RegOptionItem.WriteValue(TEXT("LimitSameIP"),m_bLimitSameIP);
		RegOptionItem.WriteValue(TEXT("NotifyUserInOut"),m_bNotifyUserInOut);
		RegOptionItem.WriteValue(TEXT("NotifyFriendCome"),m_bNotifyFriendCome);

		//系统配置
		RegOptionItem.WriteValue(TEXT("AllowSound"),m_bAllowSound);
		RegOptionItem.WriteValue(TEXT("AutoSitDown"),m_bAutoSitDown);
		RegOptionItem.WriteValue(TEXT("AutoShowWhisper"),m_bAutoShowWhisper);
		RegOptionItem.WriteValue(TEXT("SaveWhisperChat"),m_bSaveWhisperChat);
		RegOptionItem.WriteValue(TEXT("SendWhisperByEnter"),m_bSendWhisperByEnter);
		RegOptionItem.WriteValue(TEXT("ShowSafePrompt"),m_bShowSafePrompt);

		//机器信息
		if (m_szMachineID[0]==0 || m_szXorMachineID[0]==0)
		{
			//变量定义
			WCHAR szMachineID[LEN_MACHINE_ID];
			TCHAR szXorMachineID[LEN_MACHINE_ID *XOR_TIMES];

			//获取机器
			CWHService::GetMachineID(szMachineID);
			CXOREncrypt::EncryptData((CW2CT)szMachineID,szXorMachineID,CountArray(szXorMachineID));

			//写入机器
			RegOptionItem.WriteString(TEXT("SrMachineID"),(CW2CT)szMachineID);
			RegOptionItem.WriteString(TEXT("XorMachineID"),szXorMachineID);
		}
		else
		{
			//变量定义
			TCHAR szMachineID[LEN_MACHINE_ID];

			//生成明文
			CXOREncrypt::CrevasseData(m_szXorMachineID,szMachineID,CountArray(szMachineID));

			//校验机器
			if(lstrcmp(szMachineID,m_szMachineID)!=0)
			{
				RegOptionItem.WriteString(TEXT("SrMachineID"),szMachineID);
			}
		}

		//热键设置
		RegOptionItem.WriteValue(TEXT("BossHotKey"),m_wBossHotKey);
		RegOptionItem.WriteValue(TEXT("WhisperHotKey"),m_wWhisperHotKey);
	}

	//代理信息
	if (RegOptionItem.OpenRegKey(REG_PROXY_SERVER_INFO,true)==true)
	{
		RegOptionItem.WriteValue(TEXT("ProxyType"),m_ProxyServerType);
		RegOptionItem.WriteValue(TEXT("ProxyPort"),m_ProxyServerInfo.wProxyPort);
		RegOptionItem.WriteString(TEXT("UserName"),m_ProxyServerInfo.szUserName);
		RegOptionItem.WriteString(TEXT("Password"),m_ProxyServerInfo.szPassword);
		RegOptionItem.WriteString(TEXT("HttpDomain"),m_ProxyServerInfo.szHttpDomain);
		RegOptionItem.WriteString(TEXT("ProxyServer"),m_ProxyServerInfo.szProxyServer);
	}

	//游戏配置
	while (PositionGame!=NULL)
	{
		//获取子项
		WORD wKindID=0;
		CParameterGame * pParameterGame=NULL;
		m_ParameterGameMap.GetNextAssoc(PositionGame,wKindID,pParameterGame);

		//保存配置
		ASSERT(pParameterGame!=NULL);
		if (pParameterGame!=NULL) pParameterGame->SaveParameter();
	}

	return;
}

//默认参数
VOID CParameterGlobal::DefaultParameter()
{
	//登录配置
	m_LogonMode=LogonMode_Accounts;
	m_NetworkKind=NetworkKind_Default;
	m_AcountsRule=AcountsRule_Accounts;

	//时间配置
	m_wIntermitTime=0L;
	m_wOnLineCountTime=0L;

	//房间配置
	m_InviteMode=InviteMode_All;
	m_MessageMode=MessageMode_HideDetest;
	m_ActionHitAutoJoin=InterfaceAction_SearchTable;
	m_ActionLeftDoubleList=InterfaceAction_SendWhisper;

	//房间配置
	m_bLimitDetest=true;
	m_bLimitSameIP=true;
	m_bNotifyUserInOut=false;
	m_bNotifyFriendCome=true;

	//界面配置
	m_bSutraView=false;
	m_bSalienceTable=true;
	m_bFullScreenBrowse=true;

	//系统配置
	m_bAllowSound=true;
	m_bAutoSitDown=true;
	m_bSaveWhisperChat=true;
	m_bAutoShowWhisper=false;
	m_bSendWhisperByEnter=true;
	m_bShowSafePrompt=true;

	//机器信息
	m_szMachineID[0]=0;
	m_szXorMachineID[0]=0;

	//颜色设置
	m_crChatTX=PARAMETER_CHAT_TX;

	//热键配置
	m_wWhisperHotKey=MAKEWORD(VK_F11,0);
	m_wBossHotKey=MAKEWORD(VK_F12,HOTKEYF_CONTROL);

	//界面配置
	lstrcpyn(m_szSkinDirectory,SKIN_RESOURCE,CountArray(m_szSkinDirectory));
	lstrcpyn(m_szResourceModule,RESOURCE_MODELE,CountArray(m_szResourceModule));

	//代理配置
	m_ProxyServerType=ProxyType_None;
	ZeroMemory(&m_ProxyServerInfo,sizeof(m_ProxyServerInfo));

	return;
}

//游戏配置
CParameterGame * CParameterGlobal::GetParameterGame(tagGameKind * pGameKind)
{
	//寻找现存
	CParameterGame * pParameterGame=NULL;
	if (m_ParameterGameMap.Lookup(pGameKind->wKindID,pParameterGame)==TRUE) return pParameterGame;

	//创建对象
	try
	{
		//创建对象
		pParameterGame=new CParameterGame;
		if (pParameterGame==NULL) return NULL;

		//配置对象
		CW2CT strProcessName(pGameKind->szProcessName);
		pParameterGame->InitParameter(strProcessName);

		//加载参数
		pParameterGame->LoadParameter();

		//设置数组
		m_ParameterGameMap[pGameKind->wKindID]=pParameterGame;

		return pParameterGame;
	}
	catch (...) {}

	return NULL;
}

//房间配置
CParameterServer * CParameterGlobal::GetParameterServer(tagPCGameServer * pGameServer)
{
	//寻找现存
	CParameterServer * pParameterServer=NULL;
	if (m_ParameterServerMap.Lookup(pGameServer->wServerID,pParameterServer)==TRUE) return pParameterServer;

	//创建对象
	try
	{
		//创建对象
		pParameterServer=new CParameterServer;
		if (pParameterServer==NULL) return NULL;

		//设置数组
		m_ParameterServerMap[pGameServer->wServerID]=pParameterServer;

		return pParameterServer;
	}
	catch (...) {}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
