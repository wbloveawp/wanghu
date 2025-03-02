#ifndef OPTION_PARAMETER_HEAD_FILE
#define OPTION_PARAMETER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "GlobalServer.h"
#include "PlatformDataHead.h"

//////////////////////////////////////////////////////////////////////////
//ע�����

//ϵͳ��Ϣ
#define REG_USER_INFO				TEXT("UserInfo")					//�û���Ϣ
#define REG_GAME_OPTION				TEXT("ServerOption")				//��Ϸ��Ϣ

//������Ϣ
#define REG_GOBAL_OPTION			TEXT("Option\\Golbal")				//ȫ����Ϣ
#define REG_INTERFACE_INFO			TEXT("Option\\Interface")			//������Ϣ
#define REG_PROXY_SERVER_INFO		TEXT("Option\\ProxyServer")			//������Ϣ

//////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//����ģʽ
enum enInviteMode
{
	InviteMode_All,					//ȫ����ʾ
	InviteMode_None,				//ȫ����ʾ
	InviteMode_Friend,				//������ʾ
};

//��¼ģʽ
enum enLogonMode
{
	LogonMode_GameID,				//I D ��¼
	LogonMode_Accounts,				//�ʺŵ�¼
	LogonMode_BBS_Accounts,			//��̳�ʺ�
};

//��Ϣģʽ
enum enMessageMode
{
	MessageMode_ShowAll,			//ȫ����ʾ
	MessageMode_HideDetest,			//�������
	MessageMode_OnlyFriend,			//���ź���
	MessageMode_HideAll,			//��������
};

//�ʻ�����
enum enAcountsRule
{
	AcountsRule_None,				//������¼
	AcountsRule_Accounts,			//��ס�ʻ�
	AcountsRule_AccountsAndPass,	//�ʻ�����
};

//���涯��
enum enInterfaceAction
{
	//�б���
	InterfaceAction_SendWhisper,	//����˽��
	InterfaceAction_Orientation,	//��λ�û�
	InterfaceAction_ShowUserInfo,	//�û���Ϣ

	//���붯��
	InterfaceAction_SearchTable,	//Ѱ��λ��
	InterfaceAction_ShowSearchDlg,	//���ҽ���
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ����
class PLATFORM_DATA_CLASS CParameterGame
{
	//��Ԫ����
	friend class CParameterGlobal;

	//ʤ������
public:
	WORD							m_wLessWinRate;						//���ʤ��
	bool							m_bLimitWinRate;					//����ʤ��

	//��������
public:
	WORD							m_wMaxFleeRate;						//�������
	bool							m_bLimitFleeRate;					//���ƶ���

	//��������
public:
	LONG							m_lMaxGameScore;					//��߷��� 
	LONG							m_lLessGameScore;					//��ͷ���
	bool							m_bLimitGameScore;					//���Ʒ���
	DWORD							m_dwServerRule;						//���ӹ���

	//���Ա���
protected:
	TCHAR							m_szRegKeyName[16];					//ע������

	//��������
public:
	//���캯��
	CParameterGame();
	//��������
	virtual ~CParameterGame();

	//���ܺ���
public:
	//���ز���
	VOID LoadParameter();
	//�������
	VOID SaveParameter();
	//Ĭ�ϲ���
	VOID DefaultParameter();

	//���ú���
protected:
	//���ò���
	bool InitParameter(LPCTSTR pszProcessName);
};

//////////////////////////////////////////////////////////////////////////

//�������
class PLATFORM_DATA_CLASS CParameterServer
{
	//���ñ���
public:
	bool							m_bTakePassword;					//�����־
	TCHAR							m_szPassword[LEN_PASSWORD];			//��������

	//��������
public:
	//���캯��
	CParameterServer();
	//��������
	virtual ~CParameterServer();

	//���ú���
public:
	//Ĭ�ϲ���
	VOID DefaultParameter();
};

//////////////////////////////////////////////////////////////////////////

//���鶨��
typedef CMap<WORD,WORD,CParameterGame *,CParameterGame *> CParameterGameMap;
typedef CMap<WORD,WORD,CParameterServer *,CParameterServer *> CParameterServerMap;

//ȫ�ֲ���
class PLATFORM_DATA_CLASS CParameterGlobal
{
	//��¼����
public:
	enLogonMode						m_LogonMode;						//��¼ģʽ
	enAcountsRule					m_AcountsRule;						//�ʻ�����
	enNetworkKind					m_NetworkKind;						//��������

	//��������
public:
	enInviteMode					m_InviteMode;						//����ģʽ
	enMessageMode					m_MessageMode;						//��Ϣģʽ
	enInterfaceAction				m_ActionHitAutoJoin;				//�Զ�����
	enInterfaceAction				m_ActionLeftDoubleList;				//˫���б�

	//ʱ������
public:
	WORD							m_wIntermitTime;					//�ж�ʱ��
	WORD							m_wOnLineCountTime;					//����ʱ��

	//��������
public:
	bool							m_bLimitDetest;						//�������
	bool							m_bLimitSameIP;						//���Ƶ�ַ
	bool							m_bNotifyUserInOut;					//������Ϣ
	bool							m_bNotifyFriendCome;				//������ʾ

	//��������
public:
	bool							m_bSutraView;						//������ͼ
	bool							m_bSalienceTable;					//ͻ������
	bool							m_bFullScreenBrowse;				//ȫ�����

	//ϵͳ����
public:
	bool							m_bAllowSound;						//��������
	bool							m_bAutoSitDown;						//�Զ�����
	bool							m_bAutoShowWhisper;					//��ʾ˽��
	bool							m_bSaveWhisperChat;					//����˽��
	bool							m_bSendWhisperByEnter;				//�س�����
	bool							m_bShowSafePrompt;					//��ȫ����

	//�ȼ�����
public:
	WORD							m_wBossHotKey;						//�ϰ��ȼ�
	WORD							m_wWhisperHotKey;					//˽���ȼ�

	//��ɫ����
public:
	COLORREF						m_crChatTX;							//������ɫ

	//����ģ��
public:
	TCHAR							m_szResourceModule[32];				//��Դģ��
	TCHAR							m_szSkinDirectory[MAX_PATH];		//��ԴĿ¼

	//��������
public:
	enProxyServerType				m_ProxyServerType;					//��������
	tagProxyServerInfo				m_ProxyServerInfo;					//������Ϣ

	//������Ϣ
public:
	//��������
	TCHAR							m_szMachineID[LEN_MACHINE_ID];  //ԭʼ����
	TCHAR							m_szXorMachineID[LEN_MACHINE_ID *XOR_TIMES]; //������

	//�������
protected:
	CParameterGameMap				m_ParameterGameMap;					//��Ϸ����
	CParameterServerMap				m_ParameterServerMap;				//��������

	//��̬����
protected:
	static CParameterGlobal *		m_pParameterGlobal;					//ȫ������

	//��������
public:
	//���캯��
	CParameterGlobal();
	//��������
	virtual ~CParameterGlobal();

	//���ú���
public:
	//���ز���
	VOID LoadParameter();
	//�������
	VOID SaveParameter();
	//Ĭ�ϲ���
	VOID DefaultParameter();

	//��Ϸ����
public:
	//��Ϸ����
	CParameterGame * GetParameterGame(tagGameKind * pGameKind);
	//��������
	CParameterServer * GetParameterServer(tagPCGameServer * pGameServer);

	//��̬����
public:
	//��ȡ����
	static CParameterGlobal * GetInstance() { return m_pParameterGlobal; }
};

//////////////////////////////////////////////////////////////////////////

#endif