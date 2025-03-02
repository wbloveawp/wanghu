#ifndef CMD_GAME_SERVER_HEAD_FILE
#define CMD_GAME_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////

//��������
struct tagServerConfig
{
	WORD							wServerID;							//�����ʶ
	WORD							wServerType;						//��������	
	DWORD							dwServerRule;						//�������
};

//////////////////////////////////////////////////////////////////////////
//��¼����

#define MDM_GR_LOGON				10									//��¼��Ϣ

//��¼ģʽ
#define SUB_GR_LOGON_ROBOT			1									//������¼
#define SUB_GR_LOGON_MOBILE			2									//�ֻ���¼
#define SUB_GR_LOGON_COMPUTER		3									//���Ե�¼

//�ǳ�����
#define SUB_GR_LOGON_LOGOUT			10									//�û��ǳ�

//��¼���
#define SUB_GR_LOGON_SUCCESS		100									//��¼�ɹ�
#define SUB_GR_LOGON_FAILURE		101									//��¼ʧ��
#define SUB_GR_LOGON_FINISH			102									//��¼���

//������ʾ
#define SUB_GR_UPDATE_NOTIFY		200									//������ʾ

//////////////////////////////////////////////////////////////////////////

//��������
struct tagLogonMatchDetails
{
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������
};

//Լս����
struct tagLogonBattleDetails
{
	bool							bLookon;							//�Թ۱�־	
	WORD							wTableID;							//���ӱ�ʶ
	WORD							wChairID;							//���ӱ�ʶ	
	TCHAR                           szTablePasswd[LEN_PASSWORD];        //��������
};

//��λ����
struct tagLogonRankingDetails
{
	//������Ϣ
	bool							bLookon;							//�Թ۱�־	
	WORD							wTableID;							//���ӱ�ʶ
	WORD							wChairID;							//���ӱ�ʶ	
	TCHAR                           szTablePasswd[LEN_PASSWORD];        //��������
};

//�ֻ���¼
struct CMD_GR_LogonMobile
{
	//�汾��Ϣ
	WORD							wGameID;							//��Ϸ��ʶ
	DWORD							dwPlazaVersion;						//�����汾
	DWORD							dwProcessVersion;					//���̰汾

	//��¼��Ϣ
	TCHAR                           szServerPasswd[LEN_PASSWORD];       //��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ

	//��λ��Ϣ
	double							dGlobalPosLng;						//��λ����
	double							dGlobalPosLat;						//��λγ��
	TCHAR							szPlaceName[LEN_PLACE_NAME];		//��ַ��Ϣ

	//�豸��Ϣ
	DWORD                           dwDeviceKind;                       //�豸����
};

//������¼
struct CMD_GR_LogonRobot
{
	//�汾��Ϣ
	DWORD							dwPlazaVersion;						//�㳡�汾
	DWORD							dwFrameVersion;						//��ܰ汾
	DWORD							dwProcessVersion;					//���̰汾

	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D		
	TCHAR							szPassword[LEN_PASSWORD];			//��½����
};

//���Ե�¼
struct CMD_GR_LogonComputer
{
	//�汾��Ϣ
	DWORD							dwPlazaVersion;						//�㳡�汾
	DWORD							dwFrameVersion;						//��ܰ汾
	DWORD							dwProcessVersion;					//���̰汾

	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����
};


//��¼�ɹ�
struct CMD_GR_LogonSuccess
{
	DWORD							dwUserID;							//��½�ɹ�
	DWORD							dwUserRight;						//�û�Ȩ��
	DWORD							dwMasterRight;						//����Ȩ��	
};

//��¼ʧ��
struct CMD_GR_LogonFailure
{
	//������Ϣ
	WORD							wLockKindID;						//���ͱ�ʶ
	WORD							wLockServerID;						//�����ʶ
	WORD							wLockServerKind;					//��������

	//������Ϣ
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//������ʾ
struct CMD_GR_UpdateNotify
{
	//������־
	BYTE							cbMustUpdatePlaza;					//ǿ������
	BYTE							cbMustUpdateClient;					//ǿ������
	BYTE							cbAdviceUpdateClient;				//��������

	//��ǰ�汾
	DWORD							dwCurrentPlazaVersion;				//��ǰ�汾
	DWORD							dwCurrentFrameVersion;				//��ǰ�汾
	DWORD							dwCurrentClientVersion;				//��ǰ�汾
};

//////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GR_CONFIG				11									//������Ϣ

#define SUB_GR_CONFIG_COLUMN		100									//�б�����
#define SUB_GR_CONFIG_SERVER		101									//��������
#define SUB_GR_CONFIG_FINISH		102									//�������
#define SUB_GR_CONFIG_RULE			103									//�������
#define SUB_GR_CONFIG_USER_RIGHT	104									//���Ȩ��

//////////////////////////////////////////////////////////////////////////

//�б�����
struct CMD_GR_ConfigColumn
{
	BYTE							cbColumnCount;						//�б���Ŀ
	tagColumnItem					ColumnItem[32];						//�б�����
};

//��������
struct CMD_GR_ConfigServer
{
	WORD							wKindID;							//��������
	WORD							wTableCount;						//������Ŀ
	WORD							wChairCount;						//������Ŀ
	WORD							wServerType;						//��������
	DWORD							dwServerRule;						//�������	
};

//�������
struct CMD_GR_ConfigRule
{
	DWORD							dwServerRule;						//�������
};

//���Ȩ��
struct CMD_GR_ConfigUserRight
{
	DWORD							dwUserRight;						//���Ȩ��
};

//////////////////////////////////////////////////////////////////////////
//Я����Ϣ

#define DTP_GR_SERVER_TITLE			16									//�������

//////////////////////////////////////////////////////////////////////////
//�û�����

#define MDM_GR_USER					12									//�û���Ϣ

//////////////////////////////////////////////////////////////////////////
//�û�����
#define SUB_GR_USER_RULE			1									//�û�����
#define SUB_GR_USER_LOOKON			2									//�Թ�����
#define SUB_GR_USER_SITDOWN			3									//��������
#define SUB_GR_USER_STANDUP			4									//��������
#define SUB_GR_USER_APPLY			5									//��������
#define SUB_GR_USER_FEE_QUERY		6									//���ò�ѯ
#define SUB_GR_USER_REPULSE_SIT  	7									//�ܾ����
#define SUB_GR_USER_REQUEST_INFO	8                                   //��ȡ��Ϣ
#define SUB_GR_USER_SWITCH_TABLE	9                                   //������
#define SUB_GR_USER_DESKMATE_INFO	10                                  //ͬ����Ϣ

//�û�����
#define SUB_GR_USER_ENTER			100									//�û�����
#define SUB_GR_USER_SCORE			101									//�û�����
#define SUB_GR_USER_STATUS			102									//�û�״̬
#define SUB_GR_REQUEST_FAILURE		104									//����ʧ��
#define SUB_GR_USER_WAIT_DISTRIBUTE 105									//�ȴ�����
#define SUB_GR_USERLEVEL_UPGRADE	108									//�û��ȼ�
#define SUB_GR_GAMELEVEL_UPGRADE	109									//��Ϸ�ȼ�
#define SUB_GR_RECHARGE_NOTICE		110									//��ֵ֪ͨ

//��������
#define SUB_GR_SEND_GIFT			120									//��������
#define SUB_GR_SEND_GIFT_SUCCESS	121									//���ͳɹ�
#define SUB_GR_RECV_GIFT			122									//�յ�����

//��������
#define SUB_GR_USER_CHAT			200									//������Ϣ
#define SUB_GR_USER_WISPER			201									//˽����Ϣ
#define SUB_GR_WHSPER_REPLY			204									//�Զ��ظ�

#define SUB_GR_INVITE_USER			300									//�����û�

//����ʧ�ܴ���
#define RFC_NULL					0									//�޴���
#define RFC_PASSWORD_INCORRECT		1									//�������
#define RFC_TMP_PASSWD_INCORRECT	2									//��ʱ�������
#define RFC_SCORE_UNENOUGH			3									//��Ϸ�Ҳ���

//////////////////////////////////////////////////////////////////////////

//�Թ�����
struct CMD_GR_UserLookon
{
	WORD							wTableID;							//����λ��
	WORD							wChairID;							//����λ��
	TCHAR							szPassword[LEN_PASSWORD];			//��������
};

//��������
struct CMD_GR_UserSitDown
{
	WORD							wTableID;							//����λ��
	WORD							wChairID;							//����λ��
	TCHAR							szPassword[LEN_PASSWORD];			//��������
};

//��������
struct CMD_GR_UserStandUp
{
	WORD							wTableID;							//����λ��
	WORD							wChairID;							//����λ��
	BYTE							cbForceLeave;						//ǿ���뿪
};

//�û�����
struct CMD_GR_UserScore
{
	DWORD							dwUserID;							//�û���ʶ
	tagClientUserScore				UserScore;							//������Ϣ
};

//�û�״̬
struct CMD_GR_UserStatus
{
	DWORD							dwUserID;							//�û���ʶ
	tagUserStatus					UserStatus;							//�û�״̬
};

//�ȼ�����
struct CMD_GR_GameLevelUpgrade
{
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbGameOrder;						//��ǰ�ȼ�	
	LONG							lExperience;						//����ֵ	
};

//�ȼ�����
struct CMD_GR_UserLevelUpgrade
{
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	DWORD							dwMemberPoint;						//��Ա����	
};

//��ֵ֪ͨ
struct CMD_GR_RechargeNotice
{
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lPresentScore;						//���ͻ���
};

//����ʧ��
struct CMD_GR_RequestFailure
{
	BYTE							cbFailureCode;						//�������
	TCHAR							szDescribeString[256];				//������Ϣ
};

//�û�����
struct CMD_GR_S_UserChat
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�û�����
struct CMD_GR_R_UserChat
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�û�˽��
struct CMD_GR_S_UserWisper
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�û�˽��
struct CMD_GR_R_UserWisper
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�û��Ự
struct CMD_GR_UserConversation
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwConversationID;					//�Ự��ʶ
	DWORD							dwTargetUserID[MAX_CONVERSATION_USER];	//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//�ܾ����
struct CMD_GR_UserRepulseSit
{
	WORD							wTableID;							//���Ӻ���
	WORD							wChairID;							//����λ��
	DWORD							dwUserID;							//�û� I D
	DWORD							dwRepulseUserID;					//�û� I D
};

//�����û���Ϣ
struct CMD_GR_UserRequestInfo
{
	DWORD                           dwUserID;							//�����û�
	WORD							wTablePos;							//����λ��
};

//ͬ����Ϣ
struct CMD_GR_UserDeskmateInfo
{
	WORD							wTableID;							//���Ӻ���
	WORD							wChairID;							//����λ��
};


//������Ϣ
struct CMD_GR_UserPackInfo
{
	WORD							wGoodsCount;						//��Ʒ����
	tagGoodsHoldInfo				GoodsHoldInfo[0];					//��Ʒ��Ϣ
};

//��Ʒ����
struct CMD_GR_UserGoodsUpdate
{
	WORD							wGoodsCount;						//��Ʒ����
	tagGoodsHoldInfo				GoodsHoldInfo[0];					//��Ʒ��Ϣ
};

//�����û�
struct CMD_GR_R_InviteUser
{
	WORD							wTableID;							//���Ӻ���
	DWORD							dwSendUserID;						//�����û�
};

//�����û�
struct CMD_GR_S_InviteUser
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//������Ϣ
struct CMD_GR_UserBugle 
{
	DWORD							dwUserID;							//�û�ID
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�û��ʻ�
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwChatColor;						//��Ϣ��ɫ
	BYTE							cbBugleType;						//��������
	TCHAR							szChatString[MAX_BUGLE_LEN];		//������Ϣ
};

//��������
struct CMD_GR_SendGift
{
    DWORD							dwUserID;							//�û�ID

    DWORD							dwGoodsID;							//��Ʒ��ʶ
    DWORD							dwGoodsCount;						//��Ʒ����
};

//���ͳɹ�
struct CMD_GR_SendGiftSuccess
{
    DWORD							dwLoveLiness;						//����ֵ
};

//�յ�����
struct CMD_GR_RecvGift
{
    //�û���Ϣ
    DWORD							dwUserID;							//�û���ʶ
    DWORD							dwTargetUserID;						//Ŀ���û�

    //��Ʒ��Ϣ
    DWORD							dwGoodsID;							//��Ʒ��ʶ
    DWORD							dwGoodsCount;						//��Ʒ����

	DWORD							dwLoveLiness;						//��������
};

//////////////////////////////////////////////////////////////////////////

//�����־
#define UR_LIMIT_SAME_IP			0x01								//���Ƶ�ַ
#define UR_LIMIT_WIN_RATE			0x02								//����ʤ��
#define UR_LIMIT_FLEE_RATE			0x04								//��������
#define UR_LIMIT_GAME_SCORE			0x08								//���ƻ���

//�û�����
struct CMD_GR_UserRule
{
	BYTE							cbRuleMask;							//��������
	WORD							wMaxFleeRate;						//�������
	WORD							wLessWinRate;						//���ʤ��
	LONG							lMaxGameScore;						//��߷��� 
	LONG							lLessGameScore;						//��ͷ���
};

//////////////////////////////////////////////////////////////////////////
//״̬����

#define MDM_GR_STATUS				13									//״̬��Ϣ

#define SUB_GR_TABLE_INFO			100									//������Ϣ
#define SUB_GR_TABLE_STATUS			101									//����״̬
#define SUB_GR_TABLE_SCORE			102									//���ӵ׷�
#define SUB_GR_SCORE_VARIATION		103									//�׷ֱ��

//////////////////////////////////////////////////////////////////////////

//������Ϣ
struct CMD_GR_TableInfo
{
	WORD							wTableCount;						//������Ŀ
	tagTableStatus					TableStatusArray[512];				//����״̬
};

//����״̬
struct CMD_GR_TableStatus
{
	WORD							wTableID;							//���Ӻ���
	tagTableStatus					TableStatus;						//����״̬
};

//���ӷ���
struct CMD_GR_TableScore
{
	WORD							wTableCount;						//������Ŀ
	LONGLONG						lTableScoreArray[512];				//���ӵ׷�
};

//�׷ֱ��
struct CMD_GR_ScoreVariation
{
	WORD							wTableID;							//���Ӻ���
	LONGLONG						lBaseScore;							//���ӵ׷�
};

//////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GR_MANAGE				14									//��������

#define SUB_GR_SEND_WARNING			1									//���;���
#define SUB_GR_SEND_MESSAGE			2									//������Ϣ
#define SUB_GR_LOOK_USER_IP			3									//�鿴��ַ
#define SUB_GR_KILL_USER			4									//�߳��û�
#define SUB_GR_LIMIT_ACCOUNS		5									//�����ʻ�
#define SUB_GR_SET_USER_RIGHT		6									//Ȩ������
#define SUB_GR_OPTION_SERVER		7									//��������
#define SUB_GR_KICK_ALL_USER		8									//�߳��û�
#define SUB_GR_LIMIT_USER_CHAT		9									//��������
#define SUB_GR_TABLE_RULE			10									//���ӹ���
#define SUB_GR_SERVER_OPTION		11									//����ѡ��
#define SUB_GR_DISMISSGAME			12									//��ɢ����
#define SUB_GR_CARDDISPATCHERDEBUG	14									//��������Ϣ

//////////////////////////////////////////////////////////////////////////
//�����
#define MAX_REPERTORY_CNT			144									//�����

//���ñ�־
#define OSW_ROOM					1									//������Ϣ
#define OSW_GAME					2									//��Ϸ��Ϣ

//���;���
struct CMD_GR_SendWarning
{
	WORD							wChatLength;						//��Ϣ����
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szWarningMessage[LEN_USER_CHAT];	//������Ϣ
};

//ϵͳ��Ϣ
struct CMD_GR_SendMessage
{
	WORD							wType;								//��Ϣ����
	DWORD							dwOptionFlags;						//��Ϣ��Χ
	WORD							wLength;							//��Ϣ����
	TCHAR							szString[1024];						//��Ϣ����
};

//�鿴��ַ
struct CMD_GR_LookUserIP
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�߳��û�
struct CMD_GR_KillUser
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//��ɢ��Ϸ
struct CMD_GR_DismissGame
{
	WORD							wDismissTableNum;		            //��ɢ����
};

//�����ʻ�
struct CMD_GR_LimitAccounts
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//Ȩ������
struct CMD_GR_SetUserRight
{
	//Ŀ���û�
	DWORD							dwTargetUserID;						//Ŀ���û�

	//�󶨱���
	BYTE							cbGameRight;						//�ʺ�Ȩ��
	BYTE							cbAccountsRight;					//�ʺ�Ȩ��

	//Ȩ�ޱ仯
	BYTE							cbLimitRoomChat;					//��������
	BYTE							cbLimitGameChat;					//��Ϸ����
	BYTE							cbLimitPlayGame;					//��ϷȨ��
	BYTE							cbLimitSendWisper;					//������Ϣ
	BYTE							cbLimitLookonGame;					//�Թ�Ȩ��
};

//��������
struct CMD_GR_OptionServer
{
	BYTE							cbOptionFlags;						//���ñ�־
	BYTE							cbOptionValue;						//���ñ�־
};

//�߳������û�
struct CMD_GR_KickAllUser
{
	TCHAR							szKickMessage[LEN_USER_CHAT];		//�߳���ʾ
};

//��������
struct CMD_GR_LimitUserChat
{
	DWORD							dwTargetUserID;						//Ŀ���û�
	BYTE							cbLimitFlags;						//���Ʊ�־
	BYTE							cbLimitValue;						//�������
};

//����ѡ��
struct CMD_GR_ManageServerOption
{
	DWORD							dwRevenueType;						//˰������
	LONG							lRevenue;							//˰�ձ���
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wSortID;							//��������
	TCHAR							szServerName[LEN_SERVER];			//��������
};

//������
struct CMD_GR_CardDispatcherDebug
{
	WORD							wTableID;							//Ŀ������
};

// �����˿�
struct CMD_GR_SendCard
{
	//��������
	WORD                            wBankerUser;                       // ����ׯ��
	BYTE							cbControlGameCount;					//���ƾ���
	BYTE							cbMagicIndex[4];					//��������
	BYTE							cbMagicOpenPos;						//����λ��
	BYTE							cbMagicOpenCard;					//������
	BYTE							cbCardCount;						//������Ŀ
	BYTE							cbCardData[MAX_CARD_REPERTORY];		//��������
};

//////////////////////////////////////////////////////////////////////////
//��Ա����

#define MDM_GR_MEMBER				15									//��Ա����

#define SUB_GR_KICK_USER			1									//�����û�

//////////////////////////////////////////////////////////////////////////

//�����û�
struct CMD_GR_KickUser
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//////////////////////////////////////////////////////////////////////////

//���ñ�־
#define OSF_ROOM_CHAT				1									//��������
#define OSF_GAME_CHAT				2									//��Ϸ����
#define OSF_ROOM_WISPER				3									//����˽��
#define OSF_ENTER_ROOM				4									//���뷿��
#define OSF_ENTER_GAME				5									//������Ϸ
#define OSF_PLAY_CHAT				6									//ͬ����Ϸ˽��
#define OSF_REMOTE_SEARCH			7									//Զ������
#define OSF_ROBOT_ATTEND			8									//���������
#define OSF_ROBOT_SIMULATE			9									//������ռ��
#define OSF_CLOSE_NOPLAYER			10									//�Զ��رշ���
#define OSF_MEMBER_CAN_CHAT			11									//��Ա������
#define OSF_LOOK_ON					12									//��ֹ�Թ�
#define OSF_SEND_BUGLE				13									//��������
#define OSF_LOOK_ON_CHAT			14									//�Թ�����

//////////////////////////////////////////////////////////////////////////
//��ѯ����

#define MDM_GR_QUERY				16									//��ѯ����

#define SUB_GR_QUERY_BY_GAMEID		1									//��ѯ�û�
#define SUB_GR_QUERY_BY_ACCOUNTS	2									//��ѯ�û�

#define SUB_GR_QUERY_USER_RESULT	200									//��ѯ���
#define SUB_GR_QUERY_NOT_FOUND		201									//��ѯ����

//////////////////////////////////////////////////////////////////////////

//��ѯ�û�
struct CMD_GR_QueryByGameID
{
	DWORD							dwGameID;							//��ϷID
};

//��ѯ�û�
struct CMD_GR_QueryByAccounts
{
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�û��ʻ�
};

//���Ҳ���
struct CMD_GR_QueryNotFound
{
	INT								nResultCode;						//���ش���
	TCHAR							szDescribeString[128];				//������Ϣ
};

//���ش���
#define QUERY_NOT_FOUND				1									//δ�ҵ�
#define QUERY_FORBID_USE			2									//��ֹʹ�ò���


//////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GR_MATCH				17									//��������

//��������
#define SUB_GR_MATCH_SIGNUP			1									//��������
#define SUB_GR_MATCH_UNSIGNUP		2									//ȡ������
#define SUB_GR_MATCH_REVIVE			3									//�������
#define SUB_GR_DISMISS_MATCH		4									//��ɢ����

//Ӧ������
#define SUB_GR_MATCH_NUMBER			10									//�ȴ�����
#define SUB_GR_MATCH_STATUS			11									//����״̬
#define SUB_GR_MATCH_USTATUS		12									//����״̬
#define SUB_GR_SIGNUP_SUCCESS		13									//�����ɹ�
#define SUB_GR_TICKET_UPDATE		14									//��ȯ����

//������Ϣ
struct CMD_GR_MatchRequest
{
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������
};

//�����˳�
struct CMD_GR_MatchSignup : public CMD_GR_MatchRequest
{
};

//��������
struct CMD_GR_MatchUnSignup : public CMD_GR_MatchRequest
{	
};

//��������
struct CMD_GR_MatchRevive : public CMD_GR_MatchRequest
{	
};

//�������
struct CMD_GR_ShareMatch : public CMD_GR_MatchRequest
{	
};

//��ɢ����
struct CMD_GR_DismissMatch : public CMD_GR_MatchRequest
{	
};

//
//��ʱ������
struct tagLockTimeMatchData
{
	//��������	
	WORD							wFeeMoneyID;						//��������
	LONG							lFeeAmount;							//���ý��
	BYTE							cbReviveTimes;						//�������	
};

//��ʱ������
struct tagImmediateMatchData
{		
};

//��������
struct CMD_GR_MatchNumber
{	
	WORD							wTotalCount;						//��������
	WORD							wSignupCount;						//��������
};

//��ȯ����
struct SUB_GR_TicketUpdate
{
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������
	DWORD							dwTicketCount;						//����ȯ����
};

//////////////////////////////////////////////////////////////////////////
// 
//��������
#define MDM_GR_DEBUG				18									//��������

//////////////////////////////////////////////////////////////////////////
//��������

#define SUB_GR_DEBUG_PACKET			1									//�������ݰ�

//////////////////////////////////////////////////////////////////////////
//��������
#define DEBUG_KIND_GAME_CONTROL		1									//��Ϸ����
#define DEBUG_KIND_ROBOT_CONFIG		2									//��������

//////////////////////////////////////////////////////////////////////////
// 
//�������ݰ�
struct CMD_GR_DebugPacket
{
	WORD							wTableID;							//���ӱ�ʶ
	WORD							wDebugKind;							//��������
	WORD							wGameCmdID;							//��Ϸ����
};


//////////////////////////////////////////////////////////////////////////
// 
//Լս����
#define MDM_GR_BATTLE				19									//Լս����

//////////////////////////////////////////////////////////////////////////
//��������
#define SUB_GR_DISMISS_TABLE		1									//��ɢ����


//��Ӧ����
#define SUB_GR_DISMISS_RESULT		100									//��ɢ���

//////////////////////////////////////////////////////////////////////////
//��Ϣ����

//��ɢ����
struct CMD_GR_DismissTable
{
	WORD							wTableID;							//���ӱ��	
	DWORD							dwBattleNum;						//������
};

//��ɢ���
struct CMD_GR_DismissResult
{
	BYTE							cbResultCode;						//�������
	TCHAR							szDescribeString[256];				//������Ϣ
};

//////////////////////////////////////////////////////////////////////////
//�������

#define MDM_GF_FRAME				100									//�������

//////////////////////////////////////////////////////////////////////////
//�������

//�û�����
#define SUB_GF_GAME_OPTION			1									//��Ϸ����
#define SUB_GF_USER_READY			2									//�û�׼��
#define SUB_GF_USER_CHAT			3									//�û�����
#define SUB_GF_LOOKON_CONFIG		4									//�Թ�����
#define SUB_GF_USER_LOOK2SIT		5									//�Թ�����
#define SUB_GF_USER_SIT2LOOK		6									//�����Թ�
#define SUB_GF_USER_BUGLE			8									//������Ϣ
#define SUB_GF_USER_VOICE			9									//�û�����
#define SUB_GF_LAUNCH_DISMISS		10									//�����ɢ
#define SUB_GF_BALLOT_DISMISS		11									//ͶƱ��ɢ

//��ʼģʽ
#define SUB_GF_START_GAME			20									//��ʼ��Ϸ
#define SUB_GF_START_STATUS			21									//��ʼ״̬

//������Ϣ
#define SUB_GF_MATCH_DATA			50									//��������
#define SUB_GF_MATCH_RANK			51									//��������
#define SUB_GF_MATCH_SCORE			52									//��������	
#define SUB_GF_MATCH_WAIT			53									//�����ȴ�
#define SUB_GF_MATCH_PROMOTE		54									//��������
#define SUB_GF_MATCH_ELIMINATE		55									//������̭
#define SUB_GF_MATCH_ROUND_SWITCH	56									//�����ִ�
#define SUB_GF_MATCH_RESULT			50									//�������

//��Ϸ��Ϣ
#define SUB_GF_GAME_STATUS			100									//��Ϸ״̬
#define SUB_GF_GAME_SCENE			101									//��Ϸ����
#define SUB_GF_LOOKON_STATUS		102									//�Թ�״̬

//Լս��Ϣ
#define SUB_GF_TABLE_PARAM			150									//���Ӳ���	
#define SUB_GF_TABLE_BATTLE			151									//����ս��
#define SUB_GF_TABLE_GRADE			152									//���ӳɼ�
#define SUB_GF_VIDEO_PARAM			154									//��Ƶ����	
#define SUB_GF_TABLE_CONFIG			155									//��������
#define SUB_GF_MODIFY_CONFIG		156									//�޸�����

//Э���ɢ
#define SUB_GF_DISMISS_NOTIFY		160									//��ɢ����
#define SUB_GF_DISMISS_BALLOT		161									//��ɢͶƱ
#define SUB_GF_DISMISS_SUCCESS		162									//��ɢ�ɹ�	

//��λ��Ϣ
#define SUB_GF_RANKORDER_UPDATE		170									//�ȼ�����

//ϵͳ��Ϣ
#define SUB_GF_SYSTEM_MESSAGE		200									//ϵͳ��Ϣ
#define SUB_GF_ACTION_MESSAGE		201									//������Ϣ

//��ȡ�û�
#define SUB_GF_GET_SERVER_USER		210									//��ȡ�����û�
#define	SUB_GF_SERVER_USER_LIST		211									//�û��б�

//��Ϸ���
#define SUB_GF_GAME_FRAME_MIN		500									//�������
#define SUB_GF_GAME_FRAME_MAX		600									//�������

//////////////////////////////////////////////////////////////////////////

//��Ϸ����
struct CMD_GF_GameOption
{
	BYTE							cbAllowLookon;						//�Թ۱�־
	DWORD							dwFrameVersion;						//��ܰ汾
	DWORD							dwClientVersion;					//��Ϸ�汾
};

//��Ϸ״̬
struct CMD_GF_GameStatus
{
	bool							bGamePaused;						//��ͣ��־
	BYTE							cbGameStatus;						//��Ϸ״̬
	BYTE							cbAllowLookon;						//�Թ۱�־
};

//�û�����
struct CMD_GF_UserChat
{
	WORD							wChatLength;						//��Ϣ����
	COLORREF						crStringColor;						//��Ϣ��ɫ
	DWORD							dwSendUserID;						//�����û�
	DWORD							dwTargetUserID;						//Ŀ���û�
	TCHAR							szChatString[LEN_USER_CHAT];		//������Ϣ
};

//��������
struct CMD_GF_SendVoice
{
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//�û�����
struct CMD_GF_UserVoice
{
	DWORD							dwSendUserID;						//�����û�
};

//�Թ�����
struct CMD_GF_LookonConfig
{
	DWORD							dwUserID;							//�û���ʶ
	BYTE							bAllowLookon;						//�����Թ�
};

//�Թ�״̬
struct CMD_GF_LookonStatus
{
	BYTE							bAllowLookon;						//�����Թ�
};

//�Թ�����
struct CMD_GF_Look2Sit
{
	WORD							wChairID;							//����λ��
};

//�����Թ�
struct CMD_GF_Sit2Look
{
	WORD							wChairID;							//����λ��
};

//��ʼ״̬
struct CMD_GF_StartStatus
{
	bool							bEnabled;							//ʹ�ñ�ʶ
	bool							bVisibled;							//���ӱ�ʶ
};

//��������
struct CMD_GF_MatchScore
{
	WORD							wPlayCount;							//��Ϸ����
	LONG							lBaseScore;							//��Ϸ�׷�
	LONG							lEliminateScore;					//��̭����
};

//��������
struct CMD_GF_MatchData
{
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������
	BYTE							cbMatchType;						//��������	

	//�����ִ�
	WORD							wRoundID;							//�ִα�ʶ
	WORD							wRoundCount;						//�ִ���Ŀ
	tagMatchRoundItem				MatchRoundItem[5];					//�ִ���Ϣ

	//��������
	BYTE							cbMatchData[256];					//��������
};

//�����ȴ�
struct CMD_GF_MatchWait
{
	WORD							wWaitMask;						//�ȴ�����		
	WORD							wTableRankID;						//��������
	WORD							wTableUserCount;					//��������
	WORD							wWaitTableCount;					//�ȴ�����
	TCHAR							szWaitMessage[128];				//�ȴ���Ϣ
};

//��������
struct CMD_GF_MatchPromote
{
	WORD							wRankID;							//��ǰ����
	WORD							wPromoteCount;						//��������
	LONGLONG						lMatchScore;						//��������
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//������̭
struct CMD_GF_MatchEliminate
{
	//������Ϣ
	WORD							wRankID;							//��ǰ����	
	WORD							wMatchUserCount;					//�û�����

	//������Ϣ
	bool							bEnableRevive;						//�����ʶ
	BYTE							cbReviveTimes;						//�������

	//��ʾ����
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//�ִ��л�
struct CMD_GF_MatchRoundSwitch
{
	WORD							wCurrRoundID;						//�ִα�ʶ
};

//�������
struct CMD_GF_MatchResult
{	
	//�û���Ϣ
	WORD							wRankID;							//��������	
	TCHAR							szNickName[LEN_ACCOUNTS];			//����ǳ�

	//������Ϣ
	TCHAR							szRewardEntity[128];				//ʵ�ｱ��	
	TCHAR							szRewardGoods[256];					//��Ʒ����

	//������Ϣ
	TCHAR							szMatchName[64];					//��������	
	SYSTEMTIME						MatchFinishTime;					//����ʱ��	
};

//�޸�����
struct CMD_GF_ModifyConfig
{
	DWORD							dwBattleNum;						//Լս���
	WORD							wOptionMask;						//ѡ������
	BYTE							cbOptionValue;						//ѡ����ֵ
};

//���Ӳ���
struct CMD_GF_TableParam
{
	//������Ϣ	
	LONG							lBaseScore;							//��Ϸ�׷�
	DWORD							dwOwnerID;							//������ʶ
	DWORD							dwBattleNum;						//ӳ����	

	//������Ϣ
	WORD							wPlayCount;							//��Ϸ����
	WORD							wFinishCount;						//��ɾ���	

	//��¼��Ϣ
	LONGLONG						lBattleRecordID;					//��¼��ʶ
};

//��Ƶ����
struct CMD_GF_VideoParam
{
	TCHAR							szVideoKey[LEN_VIDEO_KEY];			//��Ƶ��Կ
	TCHAR							szVideoChannel[LEN_VIDEO_CHANNEL];	//��ƵƵ��
};

//Լս����
struct CMD_GF_TableConfig
{
	DWORD							dwBattleNum;						//Լս���
	SCORE							lWinnerScore;						//Ӯ�ҽ���		
	LONGLONG						lConsumeCount;						//��������
	tagBattleConfig					BattleConfig;						//Լս����
};

//ͶƱ��ɢ
struct CMD_GF_BallotDismiss
{
	bool							bAgreeDismiss;						//ͬ���ɢ
};

//��ɢ����
struct CMD_GF_DismissNotify
{
	DWORD							dwUserID;							//�����û�
	WORD							wResidueTime;						//ʣ��ʱ��
	WORD							wNeedAgreeCount;					//ͬ������	
	WORD							wUserCount;							//�û�����
	DWORD							dwUserIDList[TABLE_USER_COUNT];		//�û��б�
};

//��ɢͶƱ
struct CMD_GF_DismissBallot
{
	DWORD							dwUserID;							//�û���ʶ
	bool							bAgreeDismiss;						//ͬ���ɢ
};

//�ȼ�����
struct CMD_GF_RankOrderUpdate
{
	SCORE							lRankScore;							//��λ����
	WORD							wRankOrder;							//��λ�ȼ�	
};


//////////////////////////////////////////////////////////////////////////
//��Ϸ����

#define MDM_GF_GAME					200									//��Ϸ����

//////////////////////////////////////////////////////////////////////////
//Я����Ϣ

//������Ϣ
#define DTP_GR_MACHINE_ID			1									//������ʶ
#define DTP_GR_TABLE_PASSWORD		2									//��������

//�û�����
#define DTP_GR_USER_NICKNAME		10									//�û��ǳ�
#define DTP_GR_USER_GROUP_NAME		11									//��������
#define DTP_GR_USER_UNDER_WRITE		12									//����ǩ��

//������Ϣ
#define DTP_GR_AREA					20									//������Ϣ
#define DTP_GR_CITY					21									//������Ϣ
#define DTP_GR_PROVINCE				22									//ʡ����Ϣ
#define DTP_GR_CUSTOM_FACE			23									//�Զ�ͷ��

//������Ϣ
#define DTP_GR_USER_NOTE			30									//�û���ע

//���λ��
#define DTP_GR_SERVER_INFO			40									//���λ��

//��ʾ��Ϣ
#define DTP_GR_DESCRIBE				50									//������Ϣ
#define DTP_GR_GOODSLIST			51									//��Ʒ�б�

//////////////////////////////////////////////////////////////////////////

//�������
#define REQUEST_FAILURE_NORMAL		0									//����ԭ��
#define REQUEST_FAILURE_NOGOLD		1									//��Ҳ���
#define REQUEST_FAILURE_NOSCORE		2									//���ֲ���
#define REQUEST_FAILURE_PASSWORD	3									//�������

//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif