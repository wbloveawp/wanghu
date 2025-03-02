#include "StdAfx.h"
#include "OptionParameter.h"

//////////////////////////////////////////////////////////////////////////

//�ؼ���ɫ
#define PARAMETER_CHAT_TX			RGB(0,0,0)							//������ɫ

//////////////////////////////////////////////////////////////////////////

//��̬����
CParameterGlobal * CParameterGlobal::m_pParameterGlobal=NULL;			//ȫ������

//////////////////////////////////////////////////////////////////////////

//���캯��
CParameterGame::CParameterGame()
{
	//Ĭ�ϲ���
	DefaultParameter();

	//���Ա���
	ZeroMemory(m_szRegKeyName,sizeof(m_szRegKeyName));

	return;
}

//��������
CParameterGame::~CParameterGame()
{
}

//���ز���
VOID CParameterGame::LoadParameter()
{
	//��������
	CRegKeyAide RegOptionItem;

	//���ñ���
	if (RegOptionItem.OpenRegKey(m_szRegKeyName,REG_GAME_OPTION,false)==true)
	{
		//ʤ������
		m_wLessWinRate=(WORD)RegOptionItem.GetValue(TEXT("LessWinRate"),m_wLessWinRate);
		m_bLimitWinRate=RegOptionItem.GetValue(TEXT("LimitWinRate"),m_bLimitWinRate)?true:false;

		//��������
		m_wMaxFleeRate=(WORD)RegOptionItem.GetValue(TEXT("MaxFleeRate"),m_wMaxFleeRate);
		m_bLimitFleeRate=RegOptionItem.GetValue(TEXT("LimitFleeRate"),m_bLimitFleeRate)?true:false;
		
		//��������
		m_lMaxGameScore=RegOptionItem.GetValue(TEXT("MaxGameScore"),m_lMaxGameScore);
		m_lLessGameScore=RegOptionItem.GetValue(TEXT("LessGameScore"),m_lLessGameScore);
		m_bLimitGameScore=RegOptionItem.GetValue(TEXT("LimitGameScore"),m_bLimitGameScore)?true:false;
	}

	return;
}

//�������
VOID CParameterGame::SaveParameter()
{
	//��������
	CRegKeyAide RegOptionItem;

	//���ñ���
	if (RegOptionItem.OpenRegKey(m_szRegKeyName,REG_GAME_OPTION,true)==true)
	{
		//ʤ������
		RegOptionItem.WriteValue(TEXT("LessWinRate"),m_wLessWinRate);
		RegOptionItem.WriteValue(TEXT("LimitWinRate"),m_bLimitWinRate);

		//��������
		RegOptionItem.WriteValue(TEXT("MaxFleeRate"),m_wMaxFleeRate);
		RegOptionItem.WriteValue(TEXT("LimitFleeRate"),m_bLimitFleeRate);

		//��������
		RegOptionItem.WriteValue(TEXT("MaxGameScore"),m_lMaxGameScore);
		RegOptionItem.WriteValue(TEXT("LessGameScore"),m_lLessGameScore);
		RegOptionItem.WriteValue(TEXT("LimitGameScore"),m_bLimitGameScore);
	}

	return;
}

//Ĭ�ϲ���
VOID CParameterGame::DefaultParameter()
{
	//ʤ������
	m_wLessWinRate=0;
	m_bLimitWinRate=false;

	//��������
	m_wMaxFleeRate=5000;
	m_bLimitFleeRate=false;

	//��������
	m_bLimitGameScore=false;
	m_lMaxGameScore=2147483647L;
	m_lLessGameScore=-2147483646L;

	return;
}

//���ò���
bool CParameterGame::InitParameter(LPCTSTR pszProcessName)
{
	//�������
	WORD wNameIndex=0;
	while (wNameIndex<(CountArray(m_szRegKeyName)-1))
	{
		//��ֹ�ж�
		if (pszProcessName[wNameIndex]==0) break;
		if (pszProcessName[wNameIndex]==TEXT('.')) break;

		//�����ַ�
		WORD wCurrentIndex=wNameIndex++;
		m_szRegKeyName[wCurrentIndex]=pszProcessName[wCurrentIndex];
	}

	//���ñ���
	m_szRegKeyName[wNameIndex]=0;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CParameterServer::CParameterServer()
{
	//Ĭ�ϲ���
	DefaultParameter();

	return;
}

//��������
CParameterServer::~CParameterServer()
{
}

//Ĭ�ϲ���
VOID CParameterServer::DefaultParameter()
{
	//���ñ���
	m_bTakePassword=false;
	ZeroMemory(m_szPassword,sizeof(m_szPassword));

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CParameterGlobal::CParameterGlobal()
{
	//Ĭ�ϲ���
	DefaultParameter();

	//���ö���
	ASSERT(m_pParameterGlobal==NULL);
	if (m_pParameterGlobal==NULL) m_pParameterGlobal=this;

	return;
}

//��������
CParameterGlobal::~CParameterGlobal()
{
	//��������
	POSITION PositionGame=m_ParameterGameMap.GetStartPosition();
	POSITION PositionServer=m_ParameterGameMap.GetStartPosition();

	//ɾ������
	while (PositionGame!=NULL)
	{
		//��ȡ����
		WORD wKindID=0;
		CParameterGame * pParameterGame=NULL;
		m_ParameterGameMap.GetNextAssoc(PositionGame,wKindID,pParameterGame);

		//ɾ������
		SafeDelete(pParameterGame);
	}

	//ɾ������
	while (PositionServer!=NULL)
	{
		//��ȡ����
		WORD wServerID=0;
		CParameterServer * pParameterServer=NULL;
		m_ParameterServerMap.GetNextAssoc(PositionServer,wServerID,pParameterServer);

		//ɾ������
		SafeDelete(pParameterServer);
	}

	//ɾ������
	m_ParameterGameMap.RemoveAll();
	m_ParameterServerMap.RemoveAll();

	//�ͷŶ���
	ASSERT(m_pParameterGlobal==this);
	if (m_pParameterGlobal==this) m_pParameterGlobal=NULL;

	return;
}

//���ز���
VOID CParameterGlobal::LoadParameter()
{
	//��������
	CRegKeyAide RegOptionItem;

	//������Ϣ
	if (RegOptionItem.OpenRegKey(REG_INTERFACE_INFO,false)==true)
	{
		//��������
		m_bSutraView=RegOptionItem.GetValue(TEXT("SutraView"),m_bSutraView)?true:false;
		m_bSalienceTable=RegOptionItem.GetValue(TEXT("SalienceTable"),m_bSalienceTable)?true:false;
		m_bFullScreenBrowse=RegOptionItem.GetValue(TEXT("FullScreenBrowse"),m_bFullScreenBrowse)?true:false;

		//������Դ
#ifndef _DEBUG
		RegOptionItem.GetString(TEXT("SkinDirectory"),m_szSkinDirectory,CountArray(m_szSkinDirectory));
		RegOptionItem.GetString(TEXT("ResourceModule"),m_szResourceModule,CountArray(m_szResourceModule));
#else
 		RegOptionItem.GetString(TEXT("SkinDirectory"),m_szSkinDirectory,CountArray(m_szSkinDirectory));
 		RegOptionItem.GetString(TEXT("ResourceModule_Debug"),m_szResourceModule,CountArray(m_szResourceModule));
#endif

		//��������
		if (m_szSkinDirectory[0]==0) lstrcpyn(m_szSkinDirectory,SKIN_RESOURCE,CountArray(m_szSkinDirectory));
		if (m_szResourceModule[0]==0) lstrcpyn(m_szResourceModule,RESOURCE_MODELE,CountArray(m_szResourceModule));
	}

	//���Ʊ���
	if (RegOptionItem.OpenRegKey(REG_GOBAL_OPTION,false)==true)
	{
		//��¼ģʽ
		m_LogonMode=(enLogonMode)RegOptionItem.GetValue(TEXT("LogonMode"),m_LogonMode);
		switch (m_LogonMode)
		{
		case LogonMode_GameID:
		case LogonMode_Accounts: 
		case LogonMode_BBS_Accounts:{ break; }
		default: { m_LogonMode=LogonMode_Accounts; }
		}

		//�ʺŹ���
		m_AcountsRule=(enAcountsRule)RegOptionItem.GetValue(TEXT("AcountsRule"),m_AcountsRule);
		switch (m_AcountsRule)
		{
		case AcountsRule_None:
		case AcountsRule_Accounts:
		case AcountsRule_AccountsAndPass: break;
		default: { m_AcountsRule=AcountsRule_Accounts; break; }
		}

		//��·����
		m_NetworkKind=(enNetworkKind)RegOptionItem.GetValue(TEXT("NetworkKind"),m_NetworkKind);
		switch (m_NetworkKind)
		{
		case NetworkKind_DianXin:
		case NetworkKind_WangTong: { break; }
		default: { m_NetworkKind=NetworkKind_Default; }
		}

		//����ģʽ
		m_InviteMode=(enInviteMode)RegOptionItem.GetValue(TEXT("InviteMode"),m_InviteMode);
		switch (m_InviteMode)
		{
		case InviteMode_None:
		case InviteMode_Friend: { break; }
		default: { m_InviteMode=InviteMode_All; }
		}

		//��Ϣģʽ
		m_MessageMode=(enMessageMode)RegOptionItem.GetValue(TEXT("MessageMode"),m_MessageMode);
		switch (m_MessageMode)
		{
		case MessageMode_ShowAll:
		case MessageMode_OnlyFriend: { break; }
		default: { m_MessageMode=MessageMode_HideDetest; }
		}

		//�Զ�����
		m_ActionHitAutoJoin=(enInterfaceAction)RegOptionItem.GetValue(TEXT("ActionHitAutoJoin"),m_ActionHitAutoJoin);
		switch (m_ActionHitAutoJoin)
		{
		case InterfaceAction_ShowSearchDlg: { break; }
		default: { m_ActionHitAutoJoin=InterfaceAction_SearchTable; }
		}

		//˫���б�
		m_ActionLeftDoubleList=(enInterfaceAction)RegOptionItem.GetValue(TEXT("ActionLeftDoubleList"),m_ActionLeftDoubleList);
		switch (m_ActionLeftDoubleList)
		{
		case InterfaceAction_Orientation:
		case InterfaceAction_ShowUserInfo: { break; }
		default: { m_ActionLeftDoubleList=InterfaceAction_SendWhisper; }
		}

		//��ɫ����
		m_crChatTX=RegOptionItem.GetValue(TEXT("ChatTX"),m_crChatTX);

		//��������
		m_bLimitDetest=RegOptionItem.GetValue(TEXT("LimitDetest"),m_bLimitDetest)?true:false;
		m_bLimitSameIP=RegOptionItem.GetValue(TEXT("LimitSameIP"),m_bLimitSameIP)?true:false;
		m_bNotifyUserInOut=RegOptionItem.GetValue(TEXT("NotifyUserInOut"),m_bNotifyUserInOut)?true:false;
		m_bNotifyFriendCome=RegOptionItem.GetValue(TEXT("NotifyFriendCome"),m_bNotifyFriendCome)?true:false;

		//ϵͳ����
		m_bAllowSound=RegOptionItem.GetValue(TEXT("AllowSound"),m_bAllowSound)?true:false;
		m_bAutoSitDown=RegOptionItem.GetValue(TEXT("AutoSitDown"),m_bAutoSitDown)?true:false;
		m_bAutoShowWhisper=RegOptionItem.GetValue(TEXT("AutoShowWhisper"),m_bAutoShowWhisper)?true:false;
		m_bSaveWhisperChat=RegOptionItem.GetValue(TEXT("SaveWhisperChat"),m_bSaveWhisperChat)?true:false;
		m_bSendWhisperByEnter=RegOptionItem.GetValue(TEXT("SendWhisperByEnter"),m_bSendWhisperByEnter)?true:false;
		m_bShowSafePrompt=RegOptionItem.GetValue(TEXT("ShowSafePrompt"),m_bShowSafePrompt)?true:false;

		//������Ϣ
		RegOptionItem.GetString(TEXT("SrMachineID"),m_szMachineID,CountArray(m_szMachineID));
		RegOptionItem.GetString(TEXT("XorMachineID"),m_szXorMachineID,CountArray(m_szXorMachineID));

		//�ȼ�����
		m_wBossHotKey=(WORD)RegOptionItem.GetValue(TEXT("BossHotKey"),m_wBossHotKey);
		m_wWhisperHotKey=(WORD)RegOptionItem.GetValue(TEXT("WhisperHotKey"),m_wWhisperHotKey);
	}

	//������Ϣ
	if (RegOptionItem.OpenRegKey(REG_PROXY_SERVER_INFO,false)==true)
	{
		//���ò���
		m_ProxyServerInfo.szUserName[0]=0;
		m_ProxyServerInfo.szPassword[0]=0;
		m_ProxyServerInfo.szHttpDomain[0]=0;
		m_ProxyServerInfo.szProxyServer[0]=0;

		//��ȡ����
		m_ProxyServerInfo.wProxyPort=(WORD)RegOptionItem.GetValue(TEXT("ProxyPort"),0);
		m_ProxyServerType=(enProxyServerType)RegOptionItem.GetValue(TEXT("ProxyType"),ProxyType_None);
		RegOptionItem.GetString(TEXT("UserName"),m_ProxyServerInfo.szUserName,CountArray(m_ProxyServerInfo.szUserName));
		RegOptionItem.GetString(TEXT("Password"),m_ProxyServerInfo.szPassword,CountArray(m_ProxyServerInfo.szPassword));
		RegOptionItem.GetString(TEXT("HttpDomain"),m_ProxyServerInfo.szHttpDomain,CountArray(m_ProxyServerInfo.szHttpDomain));
		RegOptionItem.GetString(TEXT("ProxyServer"),m_ProxyServerInfo.szProxyServer,CountArray(m_ProxyServerInfo.szProxyServer));

		//��������
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

//�������
VOID CParameterGlobal::SaveParameter()
{
	//��������
	CRegKeyAide RegOptionItem;
	POSITION PositionGame=m_ParameterGameMap.GetStartPosition();

	//������Ϣ
	if (RegOptionItem.OpenRegKey(REG_INTERFACE_INFO,true)==true)
	{
		//��������
		RegOptionItem.WriteValue(TEXT("SutraView"),m_bSutraView);
		RegOptionItem.WriteValue(TEXT("SalienceTable"),m_bSalienceTable);
		RegOptionItem.WriteValue(TEXT("FullScreenBrowse"),m_bFullScreenBrowse);

		//������Դ
#ifndef _DEBUG
		RegOptionItem.WriteString(TEXT("SkinDirectory"),m_szSkinDirectory);
		RegOptionItem.WriteString(TEXT("ResourceModule"),m_szResourceModule);
#else
		RegOptionItem.WriteString(TEXT("SkinDirectory"),m_szSkinDirectory);
		RegOptionItem.WriteString(TEXT("ResourceModule_Debug"),m_szResourceModule);
#endif

	}

	//���Ʊ���
	if (RegOptionItem.OpenRegKey(REG_GOBAL_OPTION,true)==true)
	{
		//��ɫ����
		RegOptionItem.WriteValue(TEXT("ChatTX"),m_crChatTX);

		//��¼����
		RegOptionItem.WriteValue(TEXT("LogonMode"),m_LogonMode);
		RegOptionItem.WriteValue(TEXT("AcountsRule"),m_AcountsRule);
		RegOptionItem.WriteValue(TEXT("NetworkKind"),m_NetworkKind);

		//��������
		RegOptionItem.WriteValue(TEXT("InviteMode"),m_InviteMode);
		RegOptionItem.WriteValue(TEXT("MessageMode"),m_MessageMode);
		RegOptionItem.WriteValue(TEXT("ActionHitAutoJoin"),m_ActionHitAutoJoin);
		RegOptionItem.WriteValue(TEXT("ActionLeftDoubleList"),m_ActionLeftDoubleList);

		//��������
		RegOptionItem.WriteValue(TEXT("LimitDetest"),m_bLimitDetest);
		RegOptionItem.WriteValue(TEXT("LimitSameIP"),m_bLimitSameIP);
		RegOptionItem.WriteValue(TEXT("NotifyUserInOut"),m_bNotifyUserInOut);
		RegOptionItem.WriteValue(TEXT("NotifyFriendCome"),m_bNotifyFriendCome);

		//ϵͳ����
		RegOptionItem.WriteValue(TEXT("AllowSound"),m_bAllowSound);
		RegOptionItem.WriteValue(TEXT("AutoSitDown"),m_bAutoSitDown);
		RegOptionItem.WriteValue(TEXT("AutoShowWhisper"),m_bAutoShowWhisper);
		RegOptionItem.WriteValue(TEXT("SaveWhisperChat"),m_bSaveWhisperChat);
		RegOptionItem.WriteValue(TEXT("SendWhisperByEnter"),m_bSendWhisperByEnter);
		RegOptionItem.WriteValue(TEXT("ShowSafePrompt"),m_bShowSafePrompt);

		//������Ϣ
		if (m_szMachineID[0]==0 || m_szXorMachineID[0]==0)
		{
			//��������
			WCHAR szMachineID[LEN_MACHINE_ID];
			TCHAR szXorMachineID[LEN_MACHINE_ID *XOR_TIMES];

			//��ȡ����
			CWHService::GetMachineID(szMachineID);
			CXOREncrypt::EncryptData((CW2CT)szMachineID,szXorMachineID,CountArray(szXorMachineID));

			//д�����
			RegOptionItem.WriteString(TEXT("SrMachineID"),(CW2CT)szMachineID);
			RegOptionItem.WriteString(TEXT("XorMachineID"),szXorMachineID);
		}
		else
		{
			//��������
			TCHAR szMachineID[LEN_MACHINE_ID];

			//��������
			CXOREncrypt::CrevasseData(m_szXorMachineID,szMachineID,CountArray(szMachineID));

			//У�����
			if(lstrcmp(szMachineID,m_szMachineID)!=0)
			{
				RegOptionItem.WriteString(TEXT("SrMachineID"),szMachineID);
			}
		}

		//�ȼ�����
		RegOptionItem.WriteValue(TEXT("BossHotKey"),m_wBossHotKey);
		RegOptionItem.WriteValue(TEXT("WhisperHotKey"),m_wWhisperHotKey);
	}

	//������Ϣ
	if (RegOptionItem.OpenRegKey(REG_PROXY_SERVER_INFO,true)==true)
	{
		RegOptionItem.WriteValue(TEXT("ProxyType"),m_ProxyServerType);
		RegOptionItem.WriteValue(TEXT("ProxyPort"),m_ProxyServerInfo.wProxyPort);
		RegOptionItem.WriteString(TEXT("UserName"),m_ProxyServerInfo.szUserName);
		RegOptionItem.WriteString(TEXT("Password"),m_ProxyServerInfo.szPassword);
		RegOptionItem.WriteString(TEXT("HttpDomain"),m_ProxyServerInfo.szHttpDomain);
		RegOptionItem.WriteString(TEXT("ProxyServer"),m_ProxyServerInfo.szProxyServer);
	}

	//��Ϸ����
	while (PositionGame!=NULL)
	{
		//��ȡ����
		WORD wKindID=0;
		CParameterGame * pParameterGame=NULL;
		m_ParameterGameMap.GetNextAssoc(PositionGame,wKindID,pParameterGame);

		//��������
		ASSERT(pParameterGame!=NULL);
		if (pParameterGame!=NULL) pParameterGame->SaveParameter();
	}

	return;
}

//Ĭ�ϲ���
VOID CParameterGlobal::DefaultParameter()
{
	//��¼����
	m_LogonMode=LogonMode_Accounts;
	m_NetworkKind=NetworkKind_Default;
	m_AcountsRule=AcountsRule_Accounts;

	//ʱ������
	m_wIntermitTime=0L;
	m_wOnLineCountTime=0L;

	//��������
	m_InviteMode=InviteMode_All;
	m_MessageMode=MessageMode_HideDetest;
	m_ActionHitAutoJoin=InterfaceAction_SearchTable;
	m_ActionLeftDoubleList=InterfaceAction_SendWhisper;

	//��������
	m_bLimitDetest=true;
	m_bLimitSameIP=true;
	m_bNotifyUserInOut=false;
	m_bNotifyFriendCome=true;

	//��������
	m_bSutraView=false;
	m_bSalienceTable=true;
	m_bFullScreenBrowse=true;

	//ϵͳ����
	m_bAllowSound=true;
	m_bAutoSitDown=true;
	m_bSaveWhisperChat=true;
	m_bAutoShowWhisper=false;
	m_bSendWhisperByEnter=true;
	m_bShowSafePrompt=true;

	//������Ϣ
	m_szMachineID[0]=0;
	m_szXorMachineID[0]=0;

	//��ɫ����
	m_crChatTX=PARAMETER_CHAT_TX;

	//�ȼ�����
	m_wWhisperHotKey=MAKEWORD(VK_F11,0);
	m_wBossHotKey=MAKEWORD(VK_F12,HOTKEYF_CONTROL);

	//��������
	lstrcpyn(m_szSkinDirectory,SKIN_RESOURCE,CountArray(m_szSkinDirectory));
	lstrcpyn(m_szResourceModule,RESOURCE_MODELE,CountArray(m_szResourceModule));

	//��������
	m_ProxyServerType=ProxyType_None;
	ZeroMemory(&m_ProxyServerInfo,sizeof(m_ProxyServerInfo));

	return;
}

//��Ϸ����
CParameterGame * CParameterGlobal::GetParameterGame(tagGameKind * pGameKind)
{
	//Ѱ���ִ�
	CParameterGame * pParameterGame=NULL;
	if (m_ParameterGameMap.Lookup(pGameKind->wKindID,pParameterGame)==TRUE) return pParameterGame;

	//��������
	try
	{
		//��������
		pParameterGame=new CParameterGame;
		if (pParameterGame==NULL) return NULL;

		//���ö���
		CW2CT strProcessName(pGameKind->szProcessName);
		pParameterGame->InitParameter(strProcessName);

		//���ز���
		pParameterGame->LoadParameter();

		//��������
		m_ParameterGameMap[pGameKind->wKindID]=pParameterGame;

		return pParameterGame;
	}
	catch (...) {}

	return NULL;
}

//��������
CParameterServer * CParameterGlobal::GetParameterServer(tagPCGameServer * pGameServer)
{
	//Ѱ���ִ�
	CParameterServer * pParameterServer=NULL;
	if (m_ParameterServerMap.Lookup(pGameServer->wServerID,pParameterServer)==TRUE) return pParameterServer;

	//��������
	try
	{
		//��������
		pParameterServer=new CParameterServer;
		if (pParameterServer==NULL) return NULL;

		//��������
		m_ParameterServerMap[pGameServer->wServerID]=pParameterServer;

		return pParameterServer;
	}
	catch (...) {}

	return NULL;
}

//////////////////////////////////////////////////////////////////////////
