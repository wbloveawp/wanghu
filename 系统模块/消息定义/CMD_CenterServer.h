#ifndef CMD_CENTER_HEAD_FILE
#define CMD_CENTER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//ע������

#define MDM_CS_REGISTER				1									//����ע��

//////////////////////////////////////////////////////////////////////////

#define SUB_CS_C_REGISTER_AGENT		1									//ע�����
#define SUB_CS_C_REGISTER_SERVER	2									//ע����Ϸ
#define SUB_CS_C_REGISTER_MASTER	3									//ע���̨
#define SUB_CS_C_REGISTER_SERVICE	10									//ע�����		

//////////////////////////////////////////////////////////////////////////

#define SUB_CS_S_REGISTER_SUCCESS	99									//ע��ɹ�
#define SUB_CS_S_REGISTER_FAILURE	100									//ע��ʧ��

//////////////////////////////////////////////////////////////////////////
//ע�����
struct CMD_CS_C_RegisterService
{
	WORD							wServiceID;							//�����ʶ
	WORD							wServiceModule;						//����ģ��
	WORD							wServiceMask;						//��������
	WORD							wServicePort;						//����˿�
	WORD							wMaxConnect;						//��������
	WORD							wConnectCount;						//��ǰ����
	WORD							wServiceKindID;						//��������
	DWORD							dwServiceAddr;						//�����ַ
	TCHAR							szServiceName[LEN_SERVER];			//��������
	TCHAR							szServiceDomain[LEN_DOMAIN];		//��������
};

//ע�����
struct CMD_CS_C_RegisterAgent
{
	WORD							wAgentID;							//�����ʶ
	WORD							wAgentKind;							//��������
	WORD							wMaxConnect;						//��������
	DWORD							dwServiceAddr;						//�����ַ
	TCHAR							szServiceDomain[LEN_DOMAIN];		//��������		
};

//ע���̨
struct CMD_CS_C_RegisterMaster
{
	WORD							wServiceID;							//�����ʶ		
};

//ע����Ϸ
struct CMD_CS_C_RegisterServer
{
	WORD							wModuleID;							//ģ��ID
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wSortID;							//��������
	WORD							wVisibleMask;						//���ӹ���
	WORD							wServerID;							//��������
	WORD							wServerPort;						//����˿�
	WORD							wServerType;						//��������
	WORD							wServerLevel;						//����ȼ�
	WORD							wChairCount;						//��������		
	BYTE							cbMatchType;						//��������
	WORD							wTableCount;						//������Ŀ
	DWORD							dwMaxPlayer;						//�������
	DWORD							dwOnLineCount;						//��������
	DWORD							dwServerRule;						//�������
	DWORD							dwServerAddr;						//�����ַ
	TCHAR							szServerName[LEN_SERVER];			//��������
	TCHAR							szServerDomain[LEN_DOMAIN];			//��������

	//��������
	SCORE 							lCellScore;							//��λ����	
	SCORE 							lMinEnterScore;						//��ͽ���
	SCORE 							lMaxEnterScore;						//��߽���
};

//ע��ʧ��
struct CMD_CS_S_RegisterFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//////////////////////////////////////////////////////////////////////////
//������Ϣ

#define MDM_CS_COMMON_SERVICE		2									//ͨ�÷���

//////////////////////////////////////////////////////////////////////////

//������Ϣ
#define SUB_CS_C_SERVER_ONLINE		1									//��������
#define SUB_CS_C_SERVER_MODIFY		2									//�����޸�

//��������
#define SUB_CS_C_USER_BUGLE			5									//�û�����
#define SUB_CS_C_SERVICE_ONLINE		6									//��������
#define SUB_CS_C_REPORT_JACKPOT		7									//�ϱ��ʽ�

//���Ʒ���
#define SUB_CS_C_LOGOUT_TOKEN		10									//ע������
#define SUB_CS_C_REGISTER_TOKEN		11									//ע������

//·������
#define SUB_CS_C_QUERY_ROUTE_ITEM	20									//��ѯ·��
#define SUB_CS_C_ACTIVE_ROUTE_ITEM	21									//����·��
#define SUB_CS_C_UPDATE_ROUTE_ITEM	22									//����·��
#define SUB_CS_C_REMOVE_ROUTE_ITEM	23									//�Ƴ�·��
#define SUB_CS_C_APPEND_ROUTE_ITEM	24									//���·��

//////////////////////////////////////////////////////////////////////////

//��������
#define SUB_CS_S_SERVER_INFO		110									//������Ϣ
#define SUB_CS_S_SERVER_INSERT		111									//�����б�
#define SUB_CS_S_SERVER_MODIFY		112									//�����޸�
#define SUB_CS_S_SERVER_REMOVE		113									//����ɾ��
#define SUB_CS_S_SERVER_FINISH		114									//�������

//��������
#define SUB_CS_S_SERVICE_INFO		200									//������Ϣ
#define SUB_CS_S_SERVICE_INSERT		201									//�������
#define SUB_CS_S_SERVICE_REMOVE		202									//����ɾ��
#define SUB_CS_S_SERVICE_FINISH		203									//�������
#define SUB_CS_S_SERVICE_ONLINE		204									//��������
#define SUB_CS_S_SERVICE_ATTRIB		205									//��������

//·������
#define SUB_CS_S_APPEND_ROUTE_ITEM	210									//���·��
#define SUB_CS_S_REMOVE_ROUTE_ITEM	211									//�Ƴ�·��
#define SUB_CS_S_ACTIVE_ROUTE_ITEM	212									//����·��
#define SUB_CS_S_QUERY_ROUTE_RESULT	216									//��ѯ���

//����
#define SUB_CS_S_GAME_JACKPOT		301									//��Ϸ�ʳ�
#define SUB_CS_S_SERVICE_CONFIG		300									//��������

//���Ʒ���
#define SUB_CS_S_REGISTER_TOKEN		500									//ע����
#define SUB_CS_S_TOKEN_PARAMETER	501									//���Ʋ���

//Ӧ������
#define SUB_CS_S_SHUTDOWN_SOCKET	1000								//�ر�����

//////////////////////////////////////////////////////////////////////////
//ע����
#define REGISTER_RESULT_SUCCESS		0									//ע��ɹ�	
#define REGISTER_RESULT_FAILURE		1									//ע��ʧ��	
#define REGISTER_RESULT_LINKFULL	2									//��������

//////////////////////////////////////////////////////////////////////////

//��������
struct CMD_CS_C_ServerModify
{
	WORD							wSortID;							//��������
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wVisibleMask;						//���ӹ���
	WORD							wServerPort;						//����˿�
	WORD							wServerType;						//��������
	WORD							wServerLevel;						//����ȼ�
	DWORD							dwMaxPlayer;						//�������
	DWORD							dwOnLineCount;						//��������
	DWORD							dwServerAddr;						//�����ַ
	DWORD							dwServerRule;						//�������
	TCHAR							szServerName[LEN_SERVER];			//��������
	TCHAR							szServerDomain[LEN_DOMAIN];			//��������

	//��������
	SCORE 							lCellScore;							//��λ����	
	SCORE 							lMinEnterScore;						//��ͽ���
	SCORE 							lMaxEnterScore;						//��߽���
};

//������������
struct CMD_CS_C_ServiceOnline
{
	WORD							wOnlineCount;						//��������	
};

//���زʽ�
struct CMD_CS_C_ReportJackpot
{
	WORD							wKindID;							//���ͱ�ʶ
	SCORE							lJackpotScore;						//���ط���
};

//��ѯ·��
struct CMD_CS_C_QueryRouteItem
{
	DWORD							dwTokenID;							//���Ʊ�ʶ
	DWORD							dwRouteID;							//·�ɱ�ʶ
	WORD							wServiceKind;						//��������
	WORD							wServiceModule;						//����ģ��
};

//����·��
struct CMD_CS_C_ActiveRouteItem
{
	DWORD							dwRouteID;							//·�ɱ�ʶ
	WORD							wServiceKind;						//��������
	WORD							wServiceModule;						//����ģ��
};

//���·��
struct CMD_CS_C_AppendRouteItem
{
	bool							bTheLast;							//������ʶ
	WORD							wServiceKind;						//��������
	WORD							wServiceModule;						//����ģ��
};

//����·��
struct CMD_CS_C_UpdateRouteItem
{
	DWORD							dwRouteID;							//·�ɱ�ʶ
	BYTE							cbRouteState;						//·��״̬
};

//�Ƴ�·��
struct CMD_CS_C_RemoveRouteItem
{
	DWORD							dwRouteID;							//·�ɱ�ʶ	
};

//ע������
struct CMD_CS_C_LogoutToken
{
	DWORD							dwTokenID;							//���Ʊ�ʶ	
};

//ע������
struct CMD_CS_C_RegisterToken
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwTokenID;							//���Ʊ�ʶ
	DWORD							dwClientIP;							//�ն˵�ַ
};

//�ر�����
struct CMD_CS_C_ShutdownSocket
{
	bool							bForceClose;						//ǿ�ƹر�
};

//////////////////////////////////////////////////////////////////////////

//��������
struct CMD_CS_S_ServiceOnline
{
	WORD							wServiceID;							//�����ʶ
	WORD							wServiceModule;						//����ģ��
	WORD							wOnlineCount;						//��������
};

//��Ϸ����
struct CMD_CS_S_GameJackpot
{
	WORD							wKindID;							//���ͱ�ʶ
	SCORE							lJackpotScore;						//���ط���
};

//�����޸�
struct CMD_CS_S_ServerModify
{
	WORD							wSortID;							//��������
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wNodeID;							//�ڵ�����
	WORD							wVisibleMask;						//���ӹ���
	WORD							wServerID;							//�����ʶ
	WORD							wServerPort;						//����˿�
	WORD							wServerType;						//��������
	WORD							wServerLevel;						//����ȼ�
	DWORD							dwServerRule;						//�������
	DWORD							dwMasterRule;						//�������
	DWORD							dwMaxPlayer;						//�������
	DWORD							dwOnLineCount;						//��������
	DWORD							dwServerAddr;						//�����ַ
	TCHAR							szServerName[LEN_SERVER];			//��������
	TCHAR							szServerDomain[LEN_DOMAIN];			//��������

	//��������
	SCORE 							lCellScore;							//��λ����	
	SCORE 							lMinEnterScore;						//��ͽ���
	SCORE 							lMaxEnterScore;						//��߽���
};

//����ɾ��
struct CMD_CS_S_ServiceRemove
{
	WORD							wServiceID;							//�����ʶ
	WORD							wServicePort;						//����˿�
	WORD							wServiceModule;						//����ģ��	
};

//����·��
struct CMD_CS_S_ActiveRouteItem
{
	DWORD							dwRouteID;							//·�ɱ�ʶ
};

//���·��
struct CMD_CS_S_AppendRouteItem
{
	bool							bTheFirst;							//��ʼ��ʶ
	WORD							wServiceID;							//�����ʶ
	WORD							wServiceModule;						//����ģ��		
};

//�Ƴ�·��
struct CMD_CS_S_RemoveRouteItem
{
	DWORD 							dwRouteID;							//·�ɱ�ʶ
	WORD							wServiceID;							//�����ʶ
	WORD							wServiceModule;						//����ģ��
};

//��ѯ���
struct CMD_CS_S_QueryRouteResult
{
	DWORD							dwTokenID;							//���Ʊ�ʶ
	DWORD							dwRouteID;							//·�ɱ�ʶ
	WORD							wServiceID;							//�����ʶ
	WORD							wServiceModule;						//����ģ��
};

//ע����
struct CMD_CS_S_RegisterToken
{
	LONG							lResultCode;						//�������
}; 

//���Ʋ���
struct CMD_CS_S_TokenParameter
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonToken[LEN_LOGON_TOKEN];		//��¼����
};

//////////////////////////////////////////////////////////////////////////
//�û�����

#define MDM_CS_USER_COLLECT			3									//�û�����

//////////////////////////////////////////////////////////////////////////

//��������
#define SUB_CS_C_USER_ONLINE		1									//�û�����
#define SUB_CS_C_USER_OFFLINE		2									//�û�����
#define SUB_CS_C_ENTER_SERVER		3									//�û�����
#define SUB_CS_C_LEAVE_SERVER		4									//�û��˷�
#define SUB_CS_C_ENTER_MESSAGE		5									//�û���Ϣ
#define SUB_CS_C_LEAVE_MESSAGE		6									//�û���Ϣ
#define SUB_CS_C_ENTER_CLUBPLAZA	7									//�û���Ϣ
#define SUB_CS_C_LEAVE_CLUBPLAZA	8									//�û���Ϣ
#define SUB_CS_C_USER_UPDATE		9									//�û�����
#define SUB_CS_C_COLLECT_USER		20									//�����û�
#define SUB_CS_C_COLLECT_FINISH		21									//�������

//Ӧ������
#define SUB_CS_S_USER_LIST			100									//�û��б�
#define SUB_CS_S_COLLECT_USER		100									//�����û�
#define SUB_CS_S_COLLECT_FINISH		101									//�������

//////////////////////////////////////////////////////////////////////////

//�û�����
struct CMD_CS_C_UserOnline
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szNickName[LEN_NICKNAME];			//��¼�ʺ�
	TCHAR							szUserHeadUrl[256];			//����ǳ�	kk jia

	//�û�ͷ��
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwCustomID;							//ͷ���ʶ 

	//״̬��Ϣ
	BYTE							cbShutUpStatus;						//����״̬
	DWORD							dwShutUpOverTime;					//���ʱ��	

	//������Ϣ
	WORD							wGateID;							//���ر�ʶ
	DWORD							dwTokenID;							//���Ʊ�ʶ
};

//�û�����
struct CMD_CS_C_UserOffline
{
	DWORD							dwUserID;							//�û���ʶ
};

//�û�����
struct CMD_CS_C_UserUpdate
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwInfoMask;							//��Ϣ����
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	TCHAR							szUserHeadUrl[256];			//����ǳ�	kk jia

	//ͷ����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwCustomID;							//ͷ���ʶ
};

//�û�����
struct CMD_CS_C_EnterServer
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	BYTE							cbGender;							//�û��Ա�
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwCustomID;							//ͷ���ʶ
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
};

//�û��뿪
struct CMD_CS_C_LeaveServer
{
	DWORD							dwUserID;							//�û���ʶ
};

//�û�����
struct CMD_CS_C_EnterMessage
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
};

//�û��뿪
struct CMD_CS_C_LeaveMessage
{
	DWORD							dwUserID;							//�û���ʶ
};

//�û�����
struct CMD_CS_C_EnterClubPlaza
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
};

//�û��뿪
struct CMD_CS_C_LeaveClubPlaza
{
	DWORD							dwUserID;							//�û���ʶ
};

//////////////////////////////////////////////////////////////////////////
//��������

#define MDM_CS_TRANSFERS			4									//��������

//////////////////////////////////////////////////////////////////////////
//��ѯ����

#define MDM_CS_QUERY_SERVICE		5									//��ѯ����

#define SUB_CS_C_QUERY_BY_GAMEID	1									//��ѯ�û�
#define SUB_CS_C_QUERY_BY_ACCOUNTS	2									//��ѯ�û�

#define SUB_CS_S_QUERY_USER_RESULT	200									//��ѯ���
#define SUB_CS_S_QUERY_NOT_FOUND	201									//��ѯ����

//////////////////////////////////////////////////////////////////////////
//��ѯ�û�
struct CMD_CS_C_QueryByGameID
{
	DWORD							dwUserID;							//�û�ID
	DWORD							dwQueryGameID;						//��ϷID
};

//��ѯ�û�
struct CMD_CS_C_QueryByAccounts
{
	DWORD							dwUserID;							//�û�ID
	TCHAR							szQueryAccounts[LEN_ACCOUNTS];		//�û��ʻ�
};

//��ѯ���
struct CMD_CS_S_QueryUserResult
{
	DWORD							dwSendUserID;						//��ѯ�û�
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
};

//��ѯ����
struct CMD_CS_S_QueryNotFound
{
	DWORD							dwSendUserID;						//��ѯ�û�
};

//////////////////////////////////////////////////////////////////////////
//�㲥����

#define MDM_CS_BROADCAST			6									//�㲥��Ϣ

#define SUB_CS_C_GAME_BROADCAST		1									//��Ϸ����

//////////////////////////////////////////////////////////////////////////

//��̨����
#define MDM_CS_MASTER_SERVICE		7									//��̨����

//�������
#define SUB_CS_C_MODIFY_SERVER		1									//�޸ķ���
#define SUB_CS_C_COLLECT_SERVER		2									//�ռ�����

//�û�����
#define SUB_CS_C_UNLOCK_PLAYER		10									//�������
#define SUB_CS_C_KICKOUT_PLAYER		11									//�߳����
#define SUB_CS_C_USER_MAIL_NOTIFY	12									//�ʼ�֪ͨ
#define SUB_CS_C_USER_MAIL_INSERT	13									//�ʼ�����
#define SUB_CS_C_SHUTDOWN_ACCOUNT	14									//�ر��˺�
#define SUB_CS_C_USER_INFO_UPDATE	16									//��Ϣ����
#define SUB_CS_C_MODIFY_SHUTUP_STATUS 15								//����״̬

//���ù���
#define SUB_CS_C_RELOAD_GAME_CONFIG  30									//��������
#define SUB_CS_C_APPEND_FILTER_WORDS 35									//��Ӵʻ�
#define SUB_CS_C_DELETE_FILTER_WORDS 36									//ɾ���ʻ�

//�ڰ�����
#define SUB_CS_C_APPEND_USER_CONTROL 40									//��ӿ���
#define SUB_CS_C_REMOVE_USER_CONTROL 41									//�Ƴ�����

//��������
#define SUB_CS_C_PUBLISH_MARQUEE_NOTICE 50								//��������

//�Ƹ�֪ͨ
#define SUB_CS_C_WEALTH_UPDATE      60									//�Ƹ�����
#define SUB_CS_C_RECHARGE_NOTICE	61									//��ֵ֪ͨ

//�������
#define SUB_CS_C_APPEND_STOCK_ITEM	70									//��ӿ��
#define SUB_CS_C_DELETE_STOCK_ITEM	71									//ɾ�����
#define SUB_CS_C_MODIFY_STOCK_ITEM	72									//�޸Ŀ��
#define SUB_CS_C_ADJUST_STOCK_SCORE 73									//�������ֵ
#define SUB_CS_C_ADJUST_JACKPOT_SCORE 74								//�����ʽ�
#define SUB_CS_C_UPDATE_SPLAYER_CONTROL 75								//��R����

//������Ϣ
#define SUB_CS_S_SERVER_ITEM_LIST	100									//�����б�
#define SUB_CS_S_SERVER_ITEM_FINISH	101									//�������

//�������
#define SUB_CS_S_OPERATE_RESULT		1000								//�������

//////////////////////////////////////////////////////////////////////////

//�޸ķ���
struct CMD_CS_C_ModifyServer
{
	WORD							wServerID;							//�����ʶ
	DWORD							dwAppendRule;						//��ӹ���
	DWORD							dwRemoveRule;						//�Ƴ�����

	//������Ϣ
	TCHAR							szOperateID[33];					//������ʶ
};

//�������
struct CMD_CS_C_UnlockPlayer
{
	DWORD							dwUserID;							//�û���ʶ	
};

//�߳����
struct CMD_CS_C_KickoutPlayer
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//�����ʼ�
struct CMD_CS_C_UserMailInsert
{
	//�ʼ���Ϣ
	DWORD							dwMailID;							//�ʼ���ʶ
	DWORD							dwUserID;							//�û���ʶ

	//ɸѡ����
	SYSTEMTIME						LastLogonTime;						//��¼ʱ��	
};

//�ر��˺�
struct CMD_CS_C_ShutDownAccount
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//�޸�״̬
struct CMD_CS_C_ModifyShutUpStatus
{
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbShutUpStatus;						//����״̬
	DWORD							dwShutUpOverTime;					//���ʱ��
};

//��Ϣ����
struct CMD_CS_C_UserInfoUpdate
{
	DWORD							dwUserID;							//�û���ʶ
	WORD							wInfoMask;							//��Ϣ����
	DWORD							dwInfoValue;						//��Ϣ��ֵ
};

//��������
struct CMD_CS_C_ReloadGameConfig
{
	DWORD							dwConfigMask;						//��������
	DWORD							dwServiceMask;						//��������
};

//��Ӵʻ�
struct CMD_CS_C_AppendFilterWords
{
	TCHAR							szFilterWords[128];					//���˴ʻ�
};

//ɾ���ʻ�
struct CMD_CS_C_DeleteFilterWords
{
	TCHAR							szFilterWords[128];					//���˴ʻ�
};

//��ӿ���
struct CMD_CS_C_AppendUserControl
{
	DWORD							dwUserID;							//�û���ʶ
};

//�Ƴ�����
struct CMD_CS_C_RemoveUserControl
{
	DWORD							dwUserID;							//�û���ʶ
};

//��������ƹ���
struct CMD_CS_C_PublishMarqueeNotice
{
	DWORD							dwNoticeID;							//�����ʶ
	bool							bExclusive;							//��ռ��ʶ
	WORD							wViewPlace;							//��ʾλ��	
	DWORD							dwEndRollTime;						//����ʱ��	
	DWORD							dwStartRollTime;					//��ʼʱ��	
	TCHAR							szNoticeContent[256];				//��������
};

//�Ƹ�����
struct CMD_CS_C_WealthUpdate
{
	DWORD							dwUserID;							//�û���ʶ	
};

//��ֵ֪ͨ
struct CMD_CS_C_RechargeNotice
{
	//��Ϊ����
	bool							bUpdateMask;						//���±�ʶ
	DWORD							dwActionMaskEver;					//��Ϊ����
	DWORD							dwActionMaskPerDay;					//��Ϊ����
	DWORD							dwActionMaskPerWeek;				//��Ϊ����

	//��Ʒ��Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szGoodsList[1024];					//��Ʒ�б�
};

//����֪ͨ 
struct CMD_CS_C_WithdrawNotice
{
	DWORD							dwUserID;							//����֪ͨ
	SCORE							lWithdrawAmount;					//���ֽ��
	TCHAR							szNickName[LEN_NICKNAME];			//����ǳ�
};

//��ӿ��
struct CMD_CS_C_AppendStockItem
{
	WORD							wStockID;							//����ʶ
	TCHAR							szOperateID[33];					//������ʶ
};

//ɾ�����
struct CMD_CS_C_DeleteStockItem
{
	WORD							wStockID;							//����ʶ
	TCHAR							szOperateID[33];					//������ʶ
};

//�޸Ŀ��ֵ
struct CMD_CS_C_ModifyStockItem
{
	WORD							wStockID;							//����ʶ
	TCHAR							szOperateID[33];					//������ʶ
};

//�������ֵ
struct CMD_CS_C_AdjustStockScore
{
	WORD							wStockID;							//����ʶ
	SCORE							lChangeScore;						//�޸Ľ��
	TCHAR							szOperateID[33];					//������ʶ
};

//�����ʽ�
struct CMD_CS_C_AdjustJackpotScore
{
	WORD							wStockID;							//����ʶ	
	WORD							wLevelID;							//�ʳؼ���
	SCORE							lChangeScore;						//�޸Ľ��
	TCHAR							szOperateID[33];					//������ʶ
};

//�����ʽ�
struct CMD_CS_C_UpdateSPlayerControl
{
	TCHAR							szOperateID[33];					//������ʶ
};

//�������
struct CMD_CS_S_OperateResult
{
	//������Ϣ
	WORD							wRequestCmdID;						//��������
	TCHAR							szOperateID[33];					//������ʶ


	//�����Ϣ
	LONG							lResultCode;						//�������
	TCHAR							szErrorDescribe[128];				//��������
};

//////////////////////////////////////////////////////////////////////////////////
//�������
#define MDM_CS_MANAGER_SERVICE		10									//�������

#define SUB_CS_C_MODIFY_OPTION		1									//�޸�ѡ��
#define SUB_CS_C_EXECUTE_COMMAND	2									//ִ������

#define SUB_CS_S_MODIFY_OPTION		100									//�޸�ѡ��
#define SUB_CS_S_EXECUTE_COMMAND	101									//ִ������

//////////////////////////////////////////////////////////////////////////////////
// 
//�޸Ĺ���
struct CMD_CS_C_ModifyOption
{
	tagModifyOption					ModifyOption;						//�༭ѡ��
	WORD							wServerCount;						//��������
	WORD							wServerIDList[512];					//�����б�
};

//////////////////////////////////////////////////////////////////////////////////
//ת������
#define MDM_CS_FORWARD_SERVICE		12									//ת������

//////////////////////////////////////////////////////////////////////////////////

//������
#define FORWARD_TARGET_USER			1									//ת������
#define FORWARD_TARGET_SERVICE		2									//�������

//��������
struct tagForwardItem
{
	WORD							wServiceID;							//�����ʶ
	WORD							wExcludeID;							//�ų���ʶ
	DWORD							dwHashKeyID;						//ɢ�б�ʶ
	WORD							wServiceKind;						//��������	
	WORD							wServiceModule;						//����ģ��
};

//ת��ͷ��
struct tagForwardHead
{
	WORD							wTarget;							//ת��Ŀ��
	DWORD							dwUserID;							//�û���ʶ
	WORD							wItemCount;							//��������
	tagForwardItem					ForwardItem[3];						//ת������
};

//////////////////////////////////////////////////////////////////////////////////

//������Ϣ
#define SUB_CS_USER_SHUTUP_STATUS   1									//����״̬

//��Ϣ����
#define SUB_CS_POST_MESSAGE			10									//Ͷ����Ϣ

//�ʼ�����
#define SUB_CS_MAIL_INSERT			20									//�����ʼ�
#define SUB_CS_MAIL_NOTIFY			21									//�ʼ�֪ͨ

//������Ϣ
#define SUB_CS_MATCH_REMIND			30									//��������
#define SUB_CS_MATCH_WINAWARD		31									//������

//�����ʼ�
#define SUB_CS_SEND_AUTHCODE		40									//������֤��
#define SUB_CS_SEND_STOCK_ALARM		41									//���Ϳ��Ԥ��
#define SUB_CS_SEND_EXCEPTION_ALARM 42									//�����쳣Ԥ��

//��������
#define SUB_CS_UPDATE_ACTION_MASK	51									//��������

//֪ͨ����
#define SUB_CS_FILTER_WORDS_UPDATE	55									//�ʻ����
#define SUB_CS_RANKING_LIST_UPDATE	57									//�񵥸���

//��������
#define SUB_CS_LOGOUT_NOTIFY		60									//�ǳ�����

//����ƹ���
#define SUB_CS_MARQUEE_NOTICE		70									//����ƹ���

//�ڰ�����
#define SUB_CS_USER_CONTROL_APPEND	80									//��ӿ���
#define SUB_CS_USER_CONTROL_REMOVE	81									//�Ƴ�����

//�Ƹ���Ϣ
#define SUB_CS_USER_WEALTH_UPDATE   90									//�Ƹ�����
#define SUB_CS_USER_RECHARGE_NOTICE 91									//��ֵ֪ͨ

//�������
#define SUB_CS_APPEND_STOCK_ITEM	100									//��ӿ��
#define SUB_CS_DELETE_STOCK_ITEM	101									//ɾ�����
#define SUB_CS_MODIFY_STOCK_ITEM	102									//�޸Ŀ��
#define SUB_CS_ADJUST_STOCK_SCORE	104									//�޸Ŀ��ֵ
#define SUB_CS_ADJUST_JACKPOT_SCORE 105									//�޸Ĳʽ�ֵ
#define SUB_CS_UPDATE_SPLAYER_CONTROL 106								//���´�R����

//��������
#define SUB_CS_APPEND_ROBOT_PARAMETER 110								//��Ӳ���
#define SUB_CS_MODIFY_ROBOT_PARAMETER 111								//�޸Ĳ���
#define SUB_CS_DELETE_ROBOT_PARAMETER 112								//ɾ������	

///////////////////////////////////////////////////////////////////////////////////
// 
//������Ϣ

//�û�״̬
#define SUB_CS_IM_USER_ONLINE		200									//�û�����
#define SUB_CS_IM_USER_OFFLINE		201									//�û�����

///////////////////////////////////////////////////////////////////////////////////
//������Ϣ
 
//�û�״̬
#define SUB_CS_CB_USER_ONLINE		300									//�û�����
#define SUB_CS_CB_USER_OFFLINE		301									//�û�״̬
#define SUB_CS_CB_USER_UPDATE		302									//�û�����

//��������
#define SUB_CS_CB_UPDATE_CLUB		310									//�޸Ĳ���
#define SUB_CS_CB_DELETE_CLUB		311									//ɾ������

//��������
#define SUB_CS_CB_CLEANUP_SCORE		320									//��������
#define SUB_CS_CB_CLEANUP_REVENUE	321									//����˰��

//��������
#define SUB_CS_CB_APPLY_MESSAGE		330									//������Ϣ
#define SUB_CS_CB_APPLY_DELETE		331									//ɾ����Ϣ

//��Ա����
#define SUB_CS_CB_INSERT_MEMBER		340									//��ӳ�Ա
#define SUB_CS_CB_UPDATE_MEMBER		341									//���³�Ա
#define SUB_CS_CB_DELETE_MEMBER		342									//ɾ����Ա

//��ݻ���
#define SUB_CS_CB_MODIFY_SCORE		350									//�޸Ļ���

//��ݴ���
#define SUB_CS_CB_UPDATE_PAYROLL	360									//���´���

//////////////////////////////////////////////////////////////////////////////////

//����״̬
struct CMD_CS_UserShutUpStatus
{
	DWORD							dwUserID;							//�û���ʶ	
	BYTE							cbShutUpStatus;						//����״̬
	DWORD							dwShutUpOverTime;					//���ʱ��	
};

//Ͷ����Ϣ
struct CMD_CS_PostMessage
{
	DWORD							dwHashKeyID;						//ɢ�б�ʶ	
};

//�����ʼ�
struct CMD_CS_MailInsert
{
	DWORD							dwMailID;							//�ʼ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	SYSTEMTIME						LastLogonTime;						//��¼ʱ��
};

//�ʼ�֪ͨ
struct CMD_CS_MailNotify
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwMailID;							//�ʼ���ʶ
};

//��������
struct CMD_CS_MatchRemind
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ

	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	SCORE						lMatchNo;							//��������
	DWORD							dwSurplusTime;						//ʣ��ʱ��
	WCHAR							szMatchName[32];					//��������	
	SYSTEMTIME						MatchStartTime;						//����ʱ��

	//������Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wServerID;							//�����ʶ
	WORD							wServerPort;						//����˿�
	DWORD							dwServerAddr;						//�����ַ	
	WCHAR							szServerDomain[LEN_DOMAIN];			//��������

};

//������
struct CMD_CS_MatchWinAward
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	WCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�

	//������Ϣ
	WCHAR							szMatchName[32];					//��������

	//������Ϣ
	WORD							wRankID;							//���α�ʶ	
	WCHAR							szRewardEntity[128];				//ʵ�ｱ��
	WCHAR							szRewardGoods[256];					//��Ʒ����
};

//������֤��
struct CMD_CS_SendAuthCode
{
	BYTE						    cbCodeKind;							//��֤������
	BYTE						    cbAuthKind;							//��֤����
	WCHAR							szAuthCode[LEN_AUTHCODE];			//�ֻ���֤��
	WCHAR							szMobileEmail[LEN_EMAIL];			//�ֻ�����
};

//���Ԥ��
struct CMD_CS_SendStockAlarm
{
	SCORE						dAlarmScore;						//Ԥ�����	
	SCORE						dCurrentScore;						//��ǰ���
	TCHAR							szStockName[32];					//�������
};

//��������
struct CMD_CS_SendCrashAlarm
{
	WORD							wServiceID;							//�����ʶ
	WORD							wServiceModule;						//����ģ��
	TCHAR							szServiceName[32];					//��������
};

//�쳣����
struct CMD_CS_SendExceptionAlarm
{
	CHAR							szAlarmTitle[64];					//Ԥ������
	TCHAR							szServiceName[32];					//��������
	TCHAR							szExceptionInfo[512];				//�쳣��Ϣ
};

////���»���
//struct CMD_CS_UpdateUserCache
//{
//	//������Ϣ
//	DWORD							dwUserID;							//�û���ʶ
//	WORD							wInfoMask;							//��Ϣ����
//
//	//��Ϸ��Ϣ
//	SCORE							lRankScore;							//��λ����
//	DWORD							dwLoveliness;						//����ֵ
//	WORD							wMasterOrder;						//����ȼ�
//	DWORD							dwExperience;						//�û�����
//	DWORD							dwMemberPoint;						//��Ա����		
//};

//��������
struct CMD_CS_UpdateActionMask
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwActionMaskEver;					//��Ϊ����
	DWORD							dwActionMaskPerDay;					//��Ϊ����
	DWORD							dwActionMaskPerWeek;				//��Ϊ����
};

//�ʻ����
struct CMD_CS_FilterWordsUpdate
{
	bool							bDeleted;							//��ӱ�ʶ
	TCHAR							szFilterWords[128];					//���˴ʻ�
};

//�ǳ�����
struct CMD_CS_LogoutNotify
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//����ƹ���
struct CMD_CS_MarqueeNotice
{
	DWORD							dwNoticeID;							//�����ʶ
	BYTE							cbNoticeKind;						//��������
	bool							bExclusive;							//��ռ��ʶ
	BYTE							cbPriority;							//���ȼ���		
	WORD							wViewPlace;							//��ʾλ��
	WORD							wRollTimes;							//��������
	DWORD							dwEndRollTime;						//����ʱ��	
	DWORD							dwStartRollTime;					//��ʼʱ��	
	WORD							wNoticeSize;						//�����С
	BYTE							cbNoticeContent[512];				//��������
};

//�������
struct CMD_CS_UserControlAppend
{
	DWORD							dwUserID;							//�û���ʶ
};

//�����Ƴ�
struct CMD_CS_UserControlRemove
{
	DWORD							dwUserID;							//�û���ʶ
};


//�Ƹ�����
struct CMD_CS_UserWealthUpdate
{
	DWORD							dwUserID;							//�û���ʶ
};

//��ֵ֪ͨ
struct CMD_CS_UserRechargeNotice
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szGoodsList[1024];					//��Ʒiebook
};

//����֪ͨ
struct CMD_CS_UserWithdrawNotice
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwWithdrawTime;						//����ʱ��
	SCORE							lWithdrawAmount;					//���ֽ��
	TCHAR							szNickName[LEN_NICKNAME];			//����ǳ�
	
};

//��ӿ��
struct CMD_CS_AppendStockItem
{
	WORD							wStockID;							//����ʶ
	TCHAR							szOperateID[33];					//������ʶ
};

//ɾ�����
struct CMD_CS_DeleteStockItem
{
	WORD							wStockID;							//����ʶ
	TCHAR							szOperateID[33];					//������ʶ
};

//�޸Ŀ��
struct CMD_CS_ModifyStockItem
{
	WORD							wStockID;							//����ʶ
	TCHAR							szOperateID[33];					//������ʶ
};

//�������ֵ
struct CMD_CS_AdjustStockScore
{
	WORD							wStockID;							//����ʶ
	SCORE							lChangeScore;						//�仯����
	TCHAR							szOperateID[33];					//������ʶ
};

//�����ʽ�
struct CMD_CS_AdjustJackpotScore
{
	WORD							wStockID;							//����ʶ	
	WORD							wLevelID;							//�ʳؼ���
	SCORE							lChangeScore;						//�仯���
	TCHAR							szOperateID[33];					//������ʶ
};

//��Ӳ���
struct CMD_CS_AppendRobotParameter
{
	tagRobotParameter				RobotParameter;						//��������
};

//�޸Ĳ���
struct CMD_CS_ModifyRobotParameter
{
	tagRobotParameter				RobotParameter;						//��������
};

//ɾ������
struct CMD_CS_DeleteRobotParameter
{
	DWORD							dwBatchID;							//���α�ʶ
};

//////////////////////////////////////////////////////////////////////////////////
// 
//�û�����
struct CMD_CS_IM_UserOffline
{
	DWORD							dwUserID;							//�û���ʶ	
};

//////////////////////////////////////////////////////////////////////////////////

//�û�����
struct CMD_CS_CB_UserOffline
{
	DWORD							dwUserID;							//�û���ʶ	
};

//���²���
struct CMD_CS_CB_UpdateClub
{
	//������Ϣ
	DWORD							dwClubID;							//�����ʶ	
	BYTE							cbUpdateKind;						//��������	
};

//ɾ������
struct CMD_CS_CB_DeleteClub
{
	DWORD							dwClubID;							//�����ʶ
};

//�������
struct CMD_CS_CB_CleanupScore
{
	DWORD							dwClubID;							//������ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	BYTE							cbCleanupObject;					//�������
};

//����˰��
struct CMD_CS_CB_CleanupRevenue
{
	DWORD							dwClubID;							//������ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	BYTE							cbCleanupObject;					//�������
};

//������Ϣ
struct CMD_CS_CB_ApplyMessage
{
	DWORD							dwClubID;							//�����ʶ
	WORD							wMessageCount;						//��Ϣ����
};

//����ɾ��
struct CMD_CS_CB_ApplyDelete
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//�����ʶ	
	WORD							wMessageCount;						//��Ϣ����
};

//��ӳ�Ա
struct CMD_CS_CB_InsertMember
{
	DWORD							dwClubID;							//�����ʶ
	WORD							wMemberCount;						//��Ա����
	tagClubMemberUser				MemberUser;							//�����Ա
};

//���³�Ա
struct CMD_CS_CB_UpdateMember
{
	DWORD							dwClubID;							//�����ʶ
	WORD							wUpdateKind;						//��������
	tagClubMemberInfo				ClubMember;							//�����Ա
};

//ɾ����Ա
struct CMD_CS_CB_DeleteMember
{
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	WORD							wMemberCount;						//��Ա����
};

//�޸ķ���
struct CMD_CS_CB_ModifyScore
{
	//��Ϸ��Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wServerID;							//�����ʶ

	//��Ա��Ϣ
	DWORD							dwMemberID;							//��Ա��ʶ
	SCORE							lMemberScore;						//��Ա����
	SCORE							lModifyScore;						//�޸Ļ���

	//�û���Ϣ
	DWORD							dwClubID;							//�����ʶ	
	DWORD							dwPayUserID;						//֧���߱�ʶ		
};

//���´���
struct CMD_CS_CB_UpdatePayroll
{
	DWORD							dwClubID;							//��ݱ�ʶ
	DWORD							dwPayrollerID;						//�����߱�ʶ
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif