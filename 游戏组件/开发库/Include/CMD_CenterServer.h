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

//���Ʒ���
#define SUB_CS_C_LOGOUT_TOKEN		10									//ע������
#define SUB_CS_C_REGISTER_TOKEN		11									//ע������

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

//����
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
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�

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
struct CMD_CS_C_EnterServer
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
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

//�������
#define SUB_CS_C_PUBLISH_MARQUEE_NOTICE 50								//��������

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
//��������

#define MDM_CS_ROBOT_SERVICE	    8									//��������

#define SUB_CS_C_APPEND_PARAMETER	100									//��Ӳ���
#define SUB_CS_C_MODIFY_PARAMETER   101									//�޸Ĳ���
#define SUB_CS_C_DELETE_PARAMETER	102									//ɾ������	

#define SUB_CS_S_APPEND_PARAMETER	200									//��Ӳ���
#define SUB_CS_S_MODIFY_PARAMETER   201									//�޸Ĳ���
#define SUB_CS_S_DELETE_PARAMETER	202									//ɾ������	

//////////////////////////////////////////////////////////////////////////////////
//��Ӳ���
struct CMD_CS_C_AppendParameter
{
	WORD							wServerID;							//�����ʶ
	tagRobotParameter				RobotParameter;						//��������
};

//�޸Ĳ���
struct CMD_CS_C_ModifyParameter
{
	WORD							wServerID;							//�����ʶ
	tagRobotParameter				RobotParameter;						//��������
};

//ɾ������
struct CMD_CS_C_DeleteParameter
{
	WORD							wServerID;							//�����ʶ
	DWORD							dwBatchID;							//���α�ʶ
};

//��Ӳ���
struct CMD_CS_S_AppendParameter
{	
	tagRobotParameter				RobotParameter;						//��������
};

//�޸Ĳ���
struct CMD_CS_S_ModifyParameter
{
	tagRobotParameter				RobotParameter;						//��������
};

//ɾ������
struct CMD_CS_S_DeleteParameter
{
	DWORD							dwBatchID;							//���α�ʶ
};


//////////////////////////////////////////////////////////////////////////////////
//������
#define MDM_CS_STOCK_SERVICE		9									//������

//////////////////////////////////////////////////////////////////////////////////
//��������
#define SUB_CS_C_UPDATE_STOCK		100									//���¿��
#define SUB_CS_C_UPDATE_FINISH		102									//�������

//��Ӧ����
#define SUB_CS_S_STOCK_INFO			200									//�����Ϣ
#define SUB_CS_S_STOCK_FINISH		201									//������
#define SUB_CS_S_UNIT_LIMIT			202									//��Ԫ����
#define SUB_CS_S_UPDATE_STOCK		203									//���¿��
#define SUB_CS_S_STOCK_REQUEST		204									//���»���
#define SUB_CS_S_FINISH_CONFIRM		205									//�������ȷ��
#define SUB_CS_S_STOCK_SCORE_LOW	206									//������

//////////////////////////////////////////////////////////////////////////////////

//���¿��
struct CMD_CS_C_UpdateStock
{
	WORD							wStockID;						//�����ʶ
	WORD							wStockKind;						//�������	
	SCORE 							lVariationScore;				//�仯���
};

//�����Ϣ
struct CMD_CS_S_StockInfo
{
	tagStockItem					StockItem;						//�����Ϣ
};

//���¿��
struct CMD_CS_S_UnitChangeLimit
{
	WORD							wKindID;						//���ͱ�ʶ
	WORD							wStockID;						//����ʶ
	WORD							wStockKind;						//�������
	WORD							wStockSubtype;					//�������
	SCORE 							lVariationLimit;				//�仯���
};

//���¿��
struct CMD_CS_S_UpdateStock
{
	WORD							wKindID;						//���ͱ�ʶ
	WORD							wStockID;						//�����ʶ
	WORD							wServiceID;						//�����ʶ
	WORD							wStockKind;						//�������	
	WORD							wStockSubtype;					//�������
	DOUBLE							lCurrentScore;					//��ǰ���
	DOUBLE							lVariationScore;				//�仯���
	CHAR							szCustomOption[256];			//�Զ�������
};

//������
struct CMD_CS_S_StockScoreLow
{
	WORD							wKindID;						//���ͱ�ʶ
	WORD							wStockID;						//����ʶ
	WORD							wStockKind;						//�������
	WORD							wStockSubtype;					//�������						
};

//////////////////////////////////////////////////////////////////////////////////
//�������
#define MDM_CS_MANAGER_SERVICE		10									//�������

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

//�û���Ϣ
#define SUB_CS_C_USER_ONLINE		1									//�û�����
#define SUB_CS_C_USER_OFFLINE		2									//�û�����
#define SUB_CS_C_USER_SHUTUP_STATUS	3									//����״̬

//��Ϣ����
#define SUB_CS_C_POST_MESSAGE		10									//Ͷ����Ϣ

//�ʼ�����
#define SUB_CS_C_MAIL_INSERT		20									//�����ʼ�
#define SUB_CS_C_MAIL_NOTIFY		21									//�ʼ�֪ͨ

//������Ϣ
#define SUB_CS_C_MATCH_REMIND		30									//��������
#define SUB_CS_C_MATCH_WINAWARD		31									//������

//�����ʼ�
#define SUB_CS_C_SEND_AUTHCODE		40									//������֤��
#define SUB_CS_C_SEND_STOCKALARM	41									//���Ϳ��Ԥ��
#define SUB_CS_C_SEND_CRASHALARM	42									//���ͱ���Ԥ��

//��������
#define SUB_CS_C_UPDATE_USER_CACHE	50									//�û�����

//֪ͨ����
#define SUB_CS_C_FILTER_WORDS_UPDATE 55									//�ʻ����
#define SUB_CS_C_GAME_CONFIG_UPDATE  56									//���ø���
#define SUB_CS_C_RANKING_LIST_UPDATE 57									//�񵥸���

//��������
#define SUB_CS_C_LOGOUT_NOTIFY		60									//�ǳ�����

//����ƹ���
#define SUB_CS_C_MARQUEE_NOTICE		70									//����ƹ���

//�ڰ�����
#define SUB_CS_C_USER_CONTROL_APPEND 80									//��ӿ���
#define SUB_CS_C_USER_CONTROL_REMOVE 81									//�Ƴ�����

//////////////////////////////////////////////////////////////////////////////////

////�û�����
//struct CMD_CS_C_UserOnline
//{
//	//������Ϣ
//	DWORD							dwUserID;							//�û���ʶ
//	tagSocketInfo					SocketInfo;							//������Ϣ
//
//	//״̬��Ϣ
//	BYTE							cbShutUpStatus;						//����״̬
//	DWORD							dwShutUpOverTime;					//���ʱ��	
//};
//
////�û�����
//struct CMD_CS_C_UserOffline
//{
//	DWORD							dwUserID;							//�û���ʶ	
//};

//����״̬
struct CMD_CS_C_UserShutUpStatus
{
	DWORD							dwUserID;							//�û���ʶ	
	BYTE							cbShutUpStatus;						//����״̬
	DWORD							dwShutUpOverTime;					//���ʱ��	
};

//Ͷ����Ϣ
struct CMD_CS_C_PostMessage
{
	DWORD							dwHashKeyID;						//ɢ�б�ʶ	
};

//�����ʼ�
struct CMD_CS_C_MailInsert
{
	DWORD							dwMailID;							//�ʼ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	SYSTEMTIME						LastLogonTime;						//��¼ʱ��
};

//�ʼ�֪ͨ
struct CMD_CS_C_MailNotify
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwMailID;							//�ʼ���ʶ
};

//��������
struct CMD_CS_C_MatchRemind
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ

	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������
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
struct CMD_CS_C_MatchWinAward
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
struct CMD_CS_C_SendAuthCode
{
	BYTE						    cbCodeKind;							//��֤������
	BYTE						    cbAuthKind;							//��֤����
	WCHAR							szAuthCode[LEN_AUTHCODE];			//�ֻ���֤��
	WCHAR							szMobileEmail[LEN_EMAIL];			//�ֻ�����
};

//���Ԥ��
struct CMD_CS_C_SendStockAlarm
{
	LONGLONG						dAlarmScore;						//Ԥ�����	
	LONGLONG						dCurrentScore;						//��ǰ���
	TCHAR							szStockName[32];					//�������
};

//��������
struct CMD_CS_C_SendCrashAlarm
{
	WORD							wServiceID;							//�����ʶ
	WORD							wServiceModule;						//����ģ��
	TCHAR							szServiceName[32];					//��������
};

//���»���
struct CMD_CS_C_UpdateUserCache
{
	//������Ϣ
	DWORD							dwUserID;							//�û���ʶ
	WORD							wInfoMask;							//��Ϣ����

	//��Ϸ��Ϣ
	SCORE							lRankScore;							//��λ����
	DWORD							dwLoveliness;						//����ֵ
	WORD							wMasterOrder;						//����ȼ�
	DWORD							dwExperience;						//�û�����
	DWORD							dwMemberPoint;						//��Ա����		
};

//�ʻ����
struct CMD_CS_C_FilterWordsUpdate
{
	bool							bDeleted;							//��ӱ�ʶ
	TCHAR							szFilterWords[128];					//���˴ʻ�
};

//���ø���
struct CMD_CS_C_GameConfigUpdate
{
	DWORD							dwConfigMask;						//��������	
};

//�ǳ�����
struct CMD_CS_C_LogoutNotify
{
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szNotifyContent[128];				//��ʾ����
};

//����ƹ���
struct CMD_CS_C_MarqueeNotice
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
struct CMD_CS_C_UserControlAppend
{
	DWORD							dwUserID;							//�û���ʶ
};

//�����Ƴ�
struct CMD_CS_C_UserControlRemove
{
	DWORD							dwUserID;							//�û���ʶ
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif