#ifndef CMD_LOGIC_SERVICE_HEAD_FILE
#define CMD_LOGCI_SERVICE_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//�߼�����
#define MDM_LOGIC_SERVICE			10									//�߼�����

//��������
#define SUB_LS_C_ENTER_LOGIC		1									//�������
#define SUB_LS_C_LEAVE_LOGIC		2									//�뿪����
#define SUB_LS_C_SEARCH_CLUB		3									//����Ⱥ��
#define SUB_LS_C_CREATE_CLUB		4									//����Ⱥ��
#define SUB_LS_C_UPDATE_CLUB		5									//����Ⱥ��
#define SUB_LS_C_DELETE_CLUB		6									//ɾ��Ⱥ��
#define SUB_LS_C_UPGRADE_CLUB      7									//����Ⱥ��
#define SUB_LS_C_TRANSFER_CLUB		8									//ת��Ⱥ�� 
#define SUB_LS_C_INSERT_MEMBER		9									//��ӳ�Ա
#define SUB_LS_C_UPDATE_MEMBER		10									//�޸ĳ�Ա
#define SUB_LS_C_DELETE_MEMBER		11									//ɾ����Ա
#define SUB_LS_C_REMOVE_MEMBER		12									//�Ƴ���Ա
#define SUB_LS_C_APPLY_REQUEST		13									//��������
#define SUB_LS_C_APPLY_RESPOND		14									//������Ӧ	
#define SUB_LS_C_APPLY_DELETE		15									//����ɾ��	
#define SUB_LS_C_APPLY_REPEAL		16									//���볷��
#define SUB_LS_C_SETTLE_BATTLE		17									//Լս����
#define SUB_LS_C_REQUEST_MERGE		18									//����ϲ�
#define SUB_LS_C_REQUEST_INVITE		19									//��������
#define SUB_LS_C_REQUEST_PAYROLL	20									//�������
#define SUB_LS_C_CANCEL_PAYROLL		21									//ȡ������
#define SUB_LS_C_MODIFY_SCORE		22									//�޸Ĺݷ�
#define SUB_LS_C_MODIFY_COWRY		23									//�޸ı���
#define SUB_LS_C_MARKER_RECORD		24									//��Ǽ�¼
#define SUB_LS_C_PRESENT_SCORE		25									//���͹ݷ�

//��������
#define SUB_LS_C_CLEANUP_SCORE		30									//����ݷ�
#define SUB_LS_C_CLEANUP_REVENUE	31									//���˰��
#define SUB_LS_C_CLEANUP_SETUP		32									//��������
#define SUB_LS_C_CLEANUP_RANKLIST	33									//�������
#define SUB_LS_C_CLEANUP_COOPERATE	34									//�������

//Ӧ������
#define SUB_LS_S_LOGON_SUCCESS		100									//��¼�ɹ�
#define SUB_LS_S_LOGON_FAILURE		101									//��¼ʧ��
#define SUB_LS_S_CLUB_OPTION		102									//Ⱥ������
#define SUB_LS_S_CLUB_VIEWLIST		103									//Ⱥ���б�
#define SUB_LS_S_CLUB_DYNAMIC		104									//Ⱥ�鶯̬
#define SUB_LS_S_CLUB_NAMECARD		105									//Ⱥ����Ƭ
#define SUB_LS_S_CLUB_DELETE		106									//Ⱥ���Ƴ�
#define SUB_LS_S_CLUB_MEMBER		107									//Ⱥ���Ա
#define SUB_LS_S_APPLY_MESSAGE		108									//������Ϣ
#define SUB_LS_S_WEALTH_UPDATE		109	    							//�Ƹ�����
#define SUB_LS_S_SEARCH_RESULT		110									//�������
#define SUB_LS_S_APPLY_DELETE		112									//����ɾ��

//��������
#define SUB_LS_S_CLEANUP_SCORE		120									//����ݷ�
#define SUB_LS_S_CLEANUP_REVENUE	121									//����˰��
#define SUB_LS_S_CLEANUP_SETUP		122									//��������
#define SUB_LS_S_CLEANUP_RANKLIST	123									//��������
#define SUB_LS_S_CLEANUP_COOPERATE	124									//�������

//�������
#define SUB_LS_S_OPERATE_SUCCESS	200									//�����ɹ�
#define SUB_LS_S_OPERATE_FAILURE	201									//����ʧ��	

//////////////////////////////////////////////////////////////////////////////////

//�������
struct CMD_LS_C_EnterLogic
{	
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//�뿪����
struct CMD_LS_C_LeaveLogic
{	
	DWORD							dwUserID;							//�û���ʶ	
};

//����Ⱥ��
struct CMD_LS_C_SearchClub
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//Ⱥ���ʶ
};

//����Ⱥ��
struct CMD_LS_C_CreateClub
{
	////������Ϣ
	//BYTE							cbPayType;							//֧������	
	//BYTE							cbCurrencyKind;						//��������	
	//BYTE							cbBattleMask;						//Լս����
	//WORD							wRevenueRate;						//˰�ձ���

	//�û���Ϣ
	DWORD							dwCreateID;							//Ⱥ����ʶ	
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����

	//Ⱥ����Ϣ
	WORD							wClubLevel;							//��ݼ���
	TCHAR							szClubName[LEN_CLUB_NAME];			//Ⱥ������
	TCHAR							szClubIntroduce[LEN_CLUB_INTRODUCE];//Ⱥ�����
};

//����Ⱥ��
struct CMD_LS_C_UpdateClub
{	
	//Ⱥ����Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ
	BYTE							cbUpdateKind;						//��������

	//�û���Ϣ
	DWORD							dwCreaterID;						//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//ɾ��Ⱥ��
struct CMD_LS_C_DeleteClub
{
	//Ⱥ����Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ

	//�û���Ϣ
	DWORD							dwCreaterID;						//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//����Ⱥ��
struct CMD_LS_C_UpgradeClub
{
	//Ⱥ����Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ	
	WORD							wTargetLevel;						//Ŀ�꼶��

	//�û���Ϣ
	DWORD							dwCreaterID;						//�û���ʶ
	DWORD							dwCreaterGameID;					//��Ϸ��ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//ת��Ⱥ��
struct CMD_LS_C_TransferClub
{
	//�û���Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ		
	DWORD							dwCreaterID;						//������ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����

	//ת����Ϣ
	DWORD							dwNewCreaterID;						//�¹�����ʶ
	TCHAR							szAuthCodeMD5[LEN_MD5];				//��֤��
};

//ɾ����Ա
struct CMD_LS_C_DeleteMember
{
	//Ⱥ����Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	
	//������Ϣ
	BYTE							cbDeleteKind;						//ɾ������
	
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//�Ƴ���Ա
struct CMD_LS_C_RemoveMember
{
	//Ⱥ����Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//�޸Ĺ�Ա
struct CMD_LS_C_UpdateMember
{
	//Ⱥ����Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����

	//��������
	WORD							wUpdateKind;						//�޸�����
};

//��������
struct CMD_LS_C_ApplyRequest
{
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwApplyerID;						//�û���ʶ		
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//������Ӧ
struct CMD_LS_C_ApplyRespond
{
	DWORD							dwMessageID;						//��Ϣ��ʶ

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����

	//������Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwApplyerID;						//�û���ʶ	
	BYTE							cbApplyStatus;						//����״̬
};

//����ɾ��
struct CMD_LS_C_ApplyDelete
{
	DWORD							dwMessageID;						//��Ϣ��ʶ
	BYTE							cbMessageType;						//��Ϣ����

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//���볷��
struct CMD_LS_C_ApplyRepeal
{
	DWORD							dwMessageID;						//��Ϣ��ʶ

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//����ɾ��
struct CMD_LS_C_SettleBattle
{
	DWORD							dwDateID;							//ʱ���ʶ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	WORD							wSettleCount;						//�������

	//�û���Ϣ
	DWORD							dwCreaterID;						//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����		
};

//�޸Ĺݷ�
struct CMD_LS_C_ModifyScore
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//������ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	LONGLONG						lModifyScore;						//�������

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����		
};

//�޸ı���
struct CMD_LS_C_ModifyCowry
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//������ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	LONGLONG						lModifyCowry;						//�������

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����		
};

//���͹ݷ�
struct CMD_LS_C_PresentScore
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//��ݱ�ʶ
	DWORD							dwMemberID;							//Ŀ���Ա
	LONGLONG						lPresentScore;						//���ͻ���

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����		
};

//��Ǽ�¼
struct CMD_LS_C_MarkerRecord
{
	//�����Ϣ
	BYTE							cbRecordKind;						//��¼����
	DWORD							dwRecordID;							//��¼��ʶ
	BYTE							cbMarkValue;						//�����ֵ
	DWORD							dwClubID;							//��ݱ�ʶ

	//�û���Ϣ
	DWORD							dwCreaterID;						//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//����ݷ�
struct CMD_LS_C_CleanupScore
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//������ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	BYTE							cbCleanupObject;					//�������
	DWORD							dwCreaterGameID;					//��Ϸ��ʶ

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����		
};

//���˰��
struct CMD_LS_C_CleanupRevenue
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//��ݱ�ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	BYTE							cbCleanupObject;					//�������
	DWORD							dwCreaterGameID;					//��Ϸ��ʶ

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����		
};

//��������
struct CMD_LS_C_CleanupSetup
{
	//������Ϣ
	bool							bAutoCleanup;						//�Զ�����	

	//�û���Ϣ	
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//��ݱ�ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//������а�
struct CMD_LS_C_CleanupRankList
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//��ݱ�ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	BYTE							cbListKind;							//������	
	BYTE							cbCleanupObject;					//�������
	DWORD							dwCreaterGameID;					//��Ϸ��ʶ

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//�����������
struct CMD_LS_C_CleanupCooperate
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//��ݱ�ʶ
	DWORD							dwParentID;							//�ϼ���ʶ
	WORD							wMemberType;						//��Ա����	
	DWORD							dwCreaterGameID;					//��Ϸ��ʶ

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//����淨
struct CMD_LS_C_AppendConfig
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����

	//Լս����
	DWORD							dwClubID;							//Ⱥ���ʶ
	tagIMBattleConfig				BattleConfig;						//Լս�淨
};

//�޸��淨
struct CMD_LS_C_ModifyConfig
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����

	//Լս����	
	DWORD							dwClubID;							//Ⱥ���ʶ
	tagIMBattleConfig				BattleConfig;						//Լս�淨
};

//ɾ���淨
struct CMD_LS_C_DeleteConfig
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����

	//Ⱥ����Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwClubID;							//Ⱥ���ʶ
};

//����ϲ�
struct CMD_LS_C_RequestMerge
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����

	//�����Ϣ
	DWORD							dwRequestClubID;					//��ݱ�ʶ

	//�����Ϣ
	DWORD							dwRespondClubID;					//��ݱ�ʶ
	DWORD							dwCreaterGameID;					//������ʶ
};

//��������
struct CMD_LS_C_RequestInvite
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����

	//�����Ϣ
	DWORD							dwRequestClubID;					//��ݱ�ʶ

	//������Ϣ	
	DWORD							dwParentID;							//�ϼ���ʶ
	DWORD							dwRespondGameID;					//��Ϸ��ʶ
};

//�������
struct CMD_LS_C_RequestPayroll
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����

	//�����Ϣ
	DWORD							dwClubID;							//��ݱ�ʶ
	DWORD							dwMemberID;							//��Ա��ʶ	
};

//ȡ������
struct CMD_LS_C_CancelPayroll
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����

	//�����Ϣ
	DWORD							dwClubID;							//��ݱ�ʶ
	DWORD							dwMemberID;							//��Ա��ʶ	
};

//��½�ɹ�
struct CMD_LS_S_LogonSuccess
{
	//������Ϣ
	LONGLONG						lClubScore;							//��ݷ���
};

//��¼ʧ��
struct CMD_LS_S_LogonFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szErrorDescribe[128];				//������Ϣ
};

//Ⱥ������
struct CMD_LS_S_ClubOption
{
	tagClubOption					ClubOption;							//Ⱥ������
};

//Ⱥ�����
struct CMD_LS_S_ClubUpdate
{
	DWORD							dwClubID;							//Ⱥ���ʶ
	BYTE							cbUpdateKind;						//��������
};

//Ⱥ���Ƴ�
struct CMD_LS_S_ClubDelete
{
	DWORD							dwClubID;							//Ⱥ���ʶ
	TCHAR							szNotifyContent[128];				//��ʾ��Ϣ
};

//��Ա����
struct CMD_LS_S_MemberUpdate
{
	DWORD							dwClubID;							//Ⱥ���ʶ
	tagClubMember					ClubMember;							//��Ա��Ϣ
};

//�������
struct CMD_LS_S_SearchResult
{
	WORD							wClubCount;							//Ⱥ������
};

//������Ϣ
struct CMD_LS_S_ApplyMessage
{
	BYTE							cbMessageType;						//��Ϣ����
	WORD							wMessageCount;						//��Ϣ����
};

//�û��Ƹ�
struct CMD_LS_S_WealthUpdate
{
	BYTE							cbWealthMask;						//�Ƹ�����
	LONGLONG						lUserIngot;							//�û���ʯ
	LONGLONG						lUserRoomCard;						//�û�����
	LONGLONG						lUserClubScore;						//�û���ݷ�
};

//����ɾ��
struct CMD_LS_S_ApplyDelete
{
	DWORD							dwMessageID;						//��Ϣ��ʶ
	BYTE							cbMessageType;						//��Ϣ����
	bool							bDelResult;							//ɾ�����
	TCHAR							szErrorDescribe[128];				//������Ϣ
};


//����ݷ�
struct CMD_LS_S_CleanupScore
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//������ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	BYTE							cbCleanupObject;					//�������
};

//���˰��
struct CMD_LS_S_CleanupRevenue
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//��ݱ�ʶ
	DWORD							dwMemberID;							//��Ա��ʶ	
	BYTE							cbCleanupObject;					//�������
};

//��������
struct CMD_LS_S_CleanupSetup
{
	DWORD							dwClubID;							//��ݱ�ʶ
	bool							bAutoCleanup;						//�Զ�����	
};

//������а�
struct CMD_LS_S_CleanupRankList
{
	DWORD							dwClubID;							//��ݱ�ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	BYTE							cbListKind;							//������	
	BYTE							cbCleanupObject;					//�������		
};

//�����������
struct CMD_LS_S_CleanupCooperate
{
	DWORD							dwClubID;							//��ݱ�ʶ
	DWORD							dwParentID;							//�ϼ���ʶ
	WORD							wMemberType;						//��Ա����		
};

//Ⱥ�鶯̬
struct CMD_LS_S_ClubDynamic
{
	DWORD							dwClubID;							//Ⱥ���ʶ
	tagClubDynamic					ClubDynamic;						//��̬��Ϣ
};

//�����ɹ�
struct CMD_LS_S_OperateSuccess
{
	LONG							lOperateCode;						//��������
	TCHAR							szOperateNotice[128];				//������ʾ
};

//����ʧ��
struct CMD_LS_S_OperateFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szErrorDescribe[128];				//������Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif