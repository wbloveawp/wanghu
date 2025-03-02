#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//�������ݰ�

//ϵͳ����
#define DBR_GR_LOAD_CLUB_OPTION		1									//��������
#define DBR_GR_LOAD_FORBID_WORDS	2									//��ֹ�ʻ�

//Ⱥ������
#define DBR_GR_LOGON_SERVICE		10									//��¼����
#define DBR_GR_CREATE_CLUB			11									//����Ⱥ��
#define DBR_GR_UPDATE_CLUB			12									//����Ⱥ��
#define DBR_GR_DELETE_CLUB			13									//ɾ��Ⱥ��
#define DBR_GR_SEARCH_CLUB			14									//����Ⱥ��
#define DBR_GR_TRANSFER_CLUB		15									//ת��Ⱥ��	
#define DBR_GR_UPDATE_MEMBER		16									//���³�Ա
#define DBR_GR_DELETE_MEMBER		17									//ɾ����Ա
#define DBR_GR_SETTLE_BATTLE		20									//Լս����
#define DBR_GR_MODIFY_SCORE			21									//�޸Ļ���
#define DBR_GR_PRESENT_SCORE		22									//���ͻ���
#define DBR_GR_DELETE_RECORD		23									//ɾ����¼
#define DBR_GR_DETECT_CLUBNAME		24									//�������
#define DBR_GR_CLEANUP_SCORE		25									//����ݷ�
#define DBR_GR_CLEANUP_REVENUE		26									//����˰��
#define DBR_GR_CLEANUP_SETUP		27									//��������
#define DBR_GR_CLEANUP_RANKLIST		28									//����˰��

//�ϲ�����
#define DBR_GR_REQUEST_MERGE		30									//����ϲ�
#define DBR_GR_RESPOND_MERGE		31									//��Ӧ�ϲ�

//��������
#define DBR_GR_CANCEL_PAYROLL		50									//ȡ������
#define DBR_GR_REQUEST_PAYROLL		51									//�������
#define DBR_GR_RESPOND_PAYROLL		52									//��Ӧ����

//��������
#define DBR_GR_APPLY_REQUEST		60									//��������
#define DBR_GR_APPLY_RESPOND		61									//������Ӧ
#define DBR_GR_APPLY_DELETE			62									//����ɾ��
#define DBR_GR_APPLY_REPEAL			63									//���볷��

//////////////////////////////////////////////////////////////////////////

//�û���¼
struct DBR_GR_LogonService
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
};

//����Ⱥ��
struct DBR_GR_CreateClub
{
	//�û���Ϣ
	DWORD							dwCreateID;							//Ⱥ����ʶ	
	
	//Ⱥ������
	WORD							wClubKind;							//Ⱥ�鼶��
	WORD							wClubIconID;						//ͼ���ʶ
	TCHAR							szClubName[LEN_CLUB_NAME];			//Ⱥ������
	TCHAR							szClubIntroduce[LEN_CLUB_INTRODUCE];//Ⱥ�����	

	//Ⱥ�����
	BYTE							cbPayType;							//֧������
	BYTE							cbCurrencyKind;						//��������
	DWORD							dwSwitchOption;						//����ѡ��
};

//����Ⱥ��
struct DBR_GR_UpdateClub
{
	//Ⱥ����Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ		
	BYTE							cbUpdateKind;						//��������	
};

//ɾ��Ⱥ��
struct DBR_GR_DeleteClub
{
	//Ⱥ����Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwCreaterID;						//Ⱥ����ʶ		
};

//����Ⱥ��
struct DBR_GR_SearchClub
{
	//Ⱥ����Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//Ⱥ���ʶ	
};

//�������
struct DBR_GR_DetectClubName
{
	TCHAR							szClubName[LEN_CLUB_NAME];			//��������
};

//ת�þ��ֲ�
struct DBR_GR_TransferClub
{
	//Ⱥ����Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwCreaterID;						//Ⱥ����ʶ		

	//ת����Ϣ
	DWORD							dwNewCreaterID;						//���ϰ��ʶ
	TCHAR							szAuthCodeMD5[LEN_MD5];				//��֤��
};

//���³�Ա
struct DBR_GR_UpdateMember
{
	//Ⱥ����Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ

	//��������
	WORD							wUpdateKind;						//��������
};

//ɾ����Ա
struct DBR_GR_DeleteMember
{
	//Ⱥ����Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ

	//������Ϣ
	BYTE							cbDeleteKind;						//ɾ������

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
};

//�Ƴ���Ա
struct DBR_GR_RemoveMember
{
	//Ⱥ����Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
};

//��������
struct DBR_GR_ApplyRequest
{
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwApplyerID;						//�û���ʶ			
};

//������Ӧ
struct DBR_GR_ApplyRespond
{
	DWORD							dwMessageID;						//��Ϣ��ʶ

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	
	//������Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwApplyerID;						//�û���ʶ	
	BYTE							cbApplyStatus;						//����״̬
};

//����ɾ��
struct DBR_GR_ApplyDelete
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//�����ʶ

	DWORD							dwMessageID;						//��Ϣ��ʶ
	BYTE							cbMessageType;						//��Ϣ����	
};

//���볷��
struct DBR_GR_ApplyRepeal
{
	DWORD							dwMessageID;						//��Ϣ��ʶ	

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
};

//Լս����
struct DBR_GR_SettleBattle
{		
	DWORD							dwDateID;							//ʱ���ʶ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	WORD							wSettleCount;						//�������

	//�û���Ϣ
	DWORD							dwCreaterID;						//�û���ʶ
};

//�޸Ĺݷ�
struct DBR_GR_ModifyScore
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//���ֲ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ	
	LONGLONG						lModifyScore;						//�������

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
};

//���͹ݷ�
struct DBR_GR_PresentScore
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//���ֲ���ʶ
	DWORD							dwMemberID;							//Ŀ���Ա
	LONGLONG						lPresentScore;						//���ͷ���

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
};

//ɾ����¼
struct DBR_GR_DeleteRecord
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwRecordID;							//��¼��ʶ
	BYTE							cbRecordKind;						//��¼����		
};

//����ݷ�
struct DBR_GR_CleanupScore
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//���ֲ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	BYTE							cbCleanupObject;					//�������
	DWORD							dwCreaterGameID;					//�ϰ��ʶ	

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
};

//���˰��
struct DBR_GR_CleanupRevenue
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//���ֲ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	BYTE							cbCleanupObject;					//�������
	DWORD							dwCreaterGameID;					//�ϰ��ʶ	

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
};

//�������
struct DBR_GR_CleanupSetup
{
	//������Ϣ
	bool							bAutoCleanup;						//�Զ�����	

	//�û���Ϣ	
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//��ݱ�ʶ
};

//������а�
struct DBR_GR_CleanupRankList
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//���ֲ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	BYTE							cbListKind;							//������
	BYTE							cbCleanupObject;					//�������
	DWORD							dwCreaterGameID;					//��Ϸ��ʶ

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
};

//����ϲ�
struct DBR_GR_RequestMerge
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ		
	DWORD							dwRequestClubID;					//���ֲ���ʶ

	//���ֲ���Ϣ
	DWORD							dwRespondClubID;					//���ֲ���ʶ
	DWORD							dwCreaterGameID;					//�ϰ��ʶ
};

//��Ӧ�ϲ�
struct DBR_GR_RespondMerge
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ		

	//��Ӧ��Ϣ
	DWORD							dwRecordID;							//��¼��ʶ		
	BYTE							cbRespondCode;						//��Ӧ����
	TCHAR							szAuthCodeMD5[LEN_MD5];				//��֤��
};

//ȡ������
struct DBR_GR_CancelPayroll
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ		

	//���ֲ���Ϣ
	DWORD							dwClubID;							//���ֲ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
};

//�������
struct DBR_GR_RequestPayroll
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ		
	
	//���ֲ���Ϣ
	DWORD							dwClubID;							//���ֲ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
};

//��Ӧ����
struct DBR_GR_RespondPayroll
{
	//��¼��Ϣ
	DWORD							dwRecordID;							//��¼��ʶ
	BYTE							cbRespondCode;						//��Ӧ����

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ		
};

//�����ʼ�
struct DBR_GR_HandNoticeMail
{
	//��Ӧ��Ϣ
	DWORD							dwRecordID;							//��¼��ʶ
	BYTE							cbRespondCode;						//��Ӧ����	

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ			
};

//////////////////////////////////////////////////////////////////////////

//�����Ϣ

//ϵͳ����
#define DBO_GR_CLUB_OPTION			100									//��������
#define DBO_GR_FIRBID_WORDS			101									//��ֹ�ʻ�
#define DBO_GR_OPTION_FINISH		102									//�������	

//Ⱥ������
#define DBO_GR_LOGON_SUCCESS		110									//��¼�ɹ�
#define DBO_GR_LOGON_FAILURE		111									//��¼ʧ��
#define DBO_GR_LOGON_FINISH			112									//��¼���
#define DBO_GR_CREATE_CLUB			113									//����Ⱥ��
#define DBO_GR_UPDATE_CLUB			114									//����Ⱥ��
#define DBO_GR_DELETE_CLUB			115									//ɾ��Ⱥ��
#define DBO_GR_SEARCH_CLUB			116									//����Ⱥ��
#define DBO_GR_TRANSFER_CLUB		117									//ת��Ⱥ��	
#define DBO_GR_CLUB_NAMECARD		118									//������Ƭ
#define DBO_GR_INSERT_MEMBER		119									//��ӳ�Ա
#define DBO_GR_UPDATE_MEMBER		120									//��ӳ�Ա
#define DBO_GR_DELETE_MEMBER		121									//ɾ����Ա
#define DBO_GR_MODIFY_SCORE			123									//�������
#define DBO_GR_PRESENT_SCORE		124									//���ͷ���
#define DBO_GR_DETECT_CLUBNAME		125									//�������

//��������
#define DBO_GR_CLEANUP_SCORE		130									//����ݷ�
#define DBO_GR_CLEANUP_REVENUE		131									//����˰��
#define DBO_GR_CLEANUP_SETUP		132									//��������
#define DBO_GR_CLEANUP_RANKLIST		133									//����˰��

//�ϲ�����
#define DBO_GR_REQUEST_MERGE		140									//����ϲ�	
#define DBO_GR_RESPOND_MERGE		141									//��Ӧ�ϲ�

//��������
#define DBO_GR_CANCEL_PAYROLL		160									//ȡ������	
#define DBO_GR_REQUEST_PAYROLL		161									//�������
#define DBO_GR_RESPOND_PAYROLL		162									//��Ӧ����

//��������
#define DBO_GR_WEALTH_UPDATE		170									//�Ƹ�����
#define DBO_GR_APPLY_MESSAGE		171									//������Ϣ
#define DBO_GR_APPLY_DELETE			172									//ɾ�����

//�������
#define DBO_GR_OPERATE_SUCCESS		200									//�����ɹ�
#define DBO_GR_OPERATE_FAILURE		201									//����ʧ��	

//////////////////////////////////////////////////////////////////////////////////
//��¼�ɹ�
struct DBO_GR_LogonSuccess
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
};

//��¼ʧ��
struct DBO_GR_LogonFailure
{
	//�û���Ϣ	
	LONG							lErrorCode;							//�������
	TCHAR							szErrorDescribe[128];				//������Ϣ
};

//����Ⱥ��
struct DBO_GR_CreateClub
{
	tagClubNameCard					ClubNameCard;						//Ⱥ������	
};

//����Ⱥ��
struct DBO_GR_UpdateClub
{
	//Ⱥ����Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ	
	BYTE							cbUpdateKind;						//��������
};

//ɾ��Ⱥ��
struct DBO_GR_DeleteClub
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//Ⱥ���ʶ
	TCHAR							szClubName[LEN_CLUB_NAME];			//Ⱥ������
};

//����Ⱥ��
struct DBO_GR_SearchClub
{
	DWORD							dwUserID;							//�û���ʶ
	tagClubNameCard					ClubNameCard;						//������Ƭ
};

//����Ⱥ��
struct DBO_GR_UpgradeClub
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//Ⱥ���ʶ	
};

//ת��Ⱥ��
struct DBO_GR_TransferClub
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwNewCreaterID;						//���ϰ��ʶ
};

//��ӳ�Ա
struct DBO_GR_InsertMember
{
	DWORD							dwClubID;							//Ⱥ���ʶ
	tagClubNameCard					ClubNameCard;						//������Ƭ
	tagClubMemberUser				ClubMemberUser;						//��Ա�û�	
};

//���³�Ա
struct DBO_GR_UpdateMember
{
	DWORD							dwClubID;							//Ⱥ���ʶ
	WORD							wUpdateKind;						//��������
	tagClubMemberInfo				ClubMember;							//Ⱥ���Ա
};

//ɾ����Ա
struct DBO_GR_DeleteMember
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwMemberID;							//��Ա��ʶ	
	WORD							wMemberCount;						//��Ա����
	BYTE							cbDeleteKind;						//ɾ������
	TCHAR							szClubName[LEN_CLUB_NAME];			//Ⱥ������
};

//�Ƹ�����
struct DBO_GR_WealthUpdate
{
	WORD							wItemCount;							//��������
	tagWealthItem					WealthItem[5];						//�Ƹ�����
};

//�������
struct DBO_GR_ModifyScore
{
	//��Ա��Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wServerID;							//�����ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	SCORE							lMemberScore;						//��Ա����	
	SCORE							lModifyScore;						//�޸ķ���
	TCHAR							szNickName[LEN_ACCOUNTS];			//��Ա�ǳ�

	//������Ϣ	
	DWORD							dwClubID;							//Ⱥ���ʶ	
	DWORD							dwPayUserID;						//֧���߱�ʶ
	WORD							wPayUserType;						//֧��������	
	TCHAR							szClubName[LEN_CLUB_NAME];			//�������
};

//���ͷ���
struct DBO_GR_PresentScore
{
	//���ֲ���Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ
	TCHAR							szClubName[LEN_CLUB_NAME];			//�������

	//������Ա		
	DWORD							dwSrcMemberID;						//��Ա��ʶ
	LONGLONG						lPresentScore;						//���ͻ���
	LONGLONG						lSrcMemberScore;					//��Ա����		
	TCHAR							szSrcNickName[LEN_ACCOUNTS];		//��Ա�ǳ�	

	//Ŀ���Ա
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wServerID;							//�����ʶ
	DWORD							dwDstMemberID;						//��Ա��ʶ
	LONGLONG						lDstMemberScore;					//��Ա����		
	TCHAR							szDstNickName[LEN_ACCOUNTS];		//��Ա�ǳ�	
};

//�����
struct DBO_GR_DetectClubName
{
	bool							bUsable;							//���ñ�־
	TCHAR							szErrorDescribe[128];				//��������
};

//����ݷ�
struct DBO_GR_CleanupScore
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//������ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	BYTE							cbCleanupObject;					//�������
};

//���˰��
struct DBO_GR_CleanupRevenue
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//��ݱ�ʶ
	DWORD							dwMemberID;							//��Ա��ʶ	
	BYTE							cbCleanupObject;					//�������
};

//��������
struct DBO_GR_CleanupSetup
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//��ݱ�ʶ
	bool							bAutoCleanup;						//�Զ�����	
};

//������а�
struct DBO_GR_CleanupRankList
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//��ݱ�ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	BYTE							cbListKind;							//������	
	BYTE							cbCleanupObject;					//�������
};

//�����������
struct DBO_GR_CleanupCooperate
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//��ݱ�ʶ
	DWORD							dwParentID;							//�ϼ���ʶ
	WORD							wMemberType;						//��Ա����		
};

//����ϲ�
struct DBO_GR_RequestMerge
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwRequestClubID;					//���ֲ���ʶ	
	TCHAR							szRequestNickName[LEN_ACCOUNTS];	//�û��ǳ�
	TCHAR							szRequestClubName[LEN_CLUB_NAME];	//Ⱥ������	

	//�ϲ���Ϣ
	DWORD							dwRecordID;							//��¼��ʶ
	DWORD							dwRespondClubID;					//���ֲ���ʶ
	DWORD							dwRespondCreaterID;					//�ϰ��ʶ	
	TCHAR							szRespondClubName[LEN_CLUB_NAME];	//Ⱥ������
};

//��Ӧ�ϲ�
struct DBO_GR_RespondMerge
{
	//��¼��Ϣ
	DWORD							dwRecordID;							//��¼��ʶ
	BYTE							cbRespondCode;						//��Ӧ����

	//������Ϣ
	DWORD							dwRequestClubID;					//���ֲ���ʶ
	DWORD							dwRequestCreaterID;					//�ϰ��ʶ	
	
	//��Ӧ��Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwRespondClubID;					//���ֲ���ʶ
	DWORD							dwRespondCreaterID;					//�ϰ��ʶ
	TCHAR							szRespondClubName[LEN_CLUB_NAME];	//Ⱥ������

	//������Ϣ
	LONG							lErrorCode;							//�������
	TCHAR							szErrorDescribe[128];				//������Ϣ
};

//��������
struct DBO_GR_RequestInvite
{
	//�û���Ϣ
	DWORD							dwRequestUserID;					//�û���ʶ
	DWORD							dwRequestClubID;					//���ֲ���ʶ	
	TCHAR							szRequestNickName[LEN_ACCOUNTS];	//�û��ǳ�
	TCHAR							szRequestClubName[LEN_CLUB_NAME];	//Ⱥ������	

	//������Ϣ
	DWORD							dwRecordID;							//��¼��ʶ
	DWORD							dwRespondUserID;					//�û���ʶ		
};

//��Ӧ����
struct DBO_GR_RespondInvite
{
	//��¼��Ϣ
	DWORD							dwRecordID;							//��¼��ʶ
	BYTE							cbRespondCode;						//��Ӧ����

	//������Ϣ
	DWORD							dwRequestUserID;					//�û���ʶ
	DWORD							dwRequestClubID;					//���ֲ���ʶ	

	//��Ӧ��Ϣ
	DWORD							dwRespondUserID;					//��Ӧ��ʶ	
	TCHAR							szRespondNickName[LEN_ACCOUNTS];	//����ǳ�	

	//������Ϣ
	LONG							lErrorCode;							//�������
	TCHAR							szErrorDescribe[128];				//������Ϣ
};


//�������
struct DBO_GR_RequestPayroll
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//���ֲ���ʶ	
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	TCHAR							szClubName[LEN_CLUB_NAME];			//Ⱥ������	

	//��Ա��Ϣ	
	DWORD							dwMemberID;							//��Ա��ʶ	

	//��¼��Ϣ
	DWORD							dwRecordID;							//��¼��ʶ
};

//��Ӧ����
struct DBO_GR_RespondPayroll
{
	//��¼��Ϣ
	DWORD							dwRecordID;							//��¼��ʶ
	BYTE							cbRespondCode;						//��Ӧ����

	//������Ϣ
	DWORD							dwClubID;							//���ֲ���ʶ
	DWORD							dwCreaterID;						//�ϰ��ʶ	
	
	//��Ա��Ϣ
	DWORD							dwUserID;							//�û���ʶ

	//������Ϣ
	LONG							lErrorCode;							//�������
	TCHAR							szErrorDescribe[128];				//������Ϣ
};

//ȡ������
struct DBO_GR_CancelPayroll
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
	DWORD							dwClubID;							//���ֲ���ʶ	
	DWORD							dwMemberID;							//��Ա��ʶ		
};

//������Ϣ
struct DBO_GR_ApplyMessage
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//���ֲ���ʶ
	BYTE							cbMessageType;						//��Ϣ����
	WORD							wMessageCount;						//��Ϣ����	
};

//�ϲ���Ϣ
struct DBO_GR_MergeMessage
{
	DWORD							dwUserID;							//�û���ʶ	
	WORD							wMessageCount;						//��Ϣ����	
};

//ɾ�����
struct DBO_GR_ApplyDelete
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwMessageID;						//��Ϣ��ʶ
	BYTE							cbMessageType;						//��Ϣ����	
};

//�����ɹ�
struct DBO_GR_OperateSuccess
{
	//�û���Ϣ
	LONG							lOperateCode;						//��������
	TCHAR							szOperateNotice[128];				//������ʾ
};

//����ʧ��
struct DBO_GR_OperateFailure
{
	//�û���Ϣ
	LONG							lErrorCode;							//�������
	TCHAR							szErrorDescribe[128];				//������Ϣ
};

//////////////////////////////////////////////////////////////////////////////////

#endif