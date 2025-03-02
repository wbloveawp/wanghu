#ifndef CMD_CLUB_PLAZA_HEAD_FILE
#define CMD_CLUB_PLAZA_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//�㳡����
#define MDM_CLUBPLAZA_SERVICE		10									//�㳡����

//��������
#define SUB_CP_C_ENTER_SERVICE		1									//�������
#define SUB_CP_C_SEARCH_CLUB		3									//��������
#define SUB_CP_C_CREATE_CLUB		4									//��������
#define SUB_CP_C_UPDATE_CLUB		5									//���²���
#define SUB_CP_C_DELETE_CLUB		6									//ɾ������
#define SUB_CP_C_INSERT_MEMBER		7									//��ӳ�Ա
#define SUB_CP_C_UPDATE_MEMBER		8									//�޸ĳ�Ա
#define SUB_CP_C_DELETE_MEMBER		9									//ɾ����Ա
#define SUB_CP_C_APPLY_REQUEST		10									//��������
#define SUB_CP_C_APPLY_RESPOND		11									//������Ӧ	
#define SUB_CP_C_APPLY_DELETE		12									//����ɾ��	
#define SUB_CP_C_APPLY_REPEAL		13									//���볷��
#define SUB_CP_C_SETTLE_BATTLE		14									//Լս����
#define SUB_CP_C_MODIFY_SCORE		15									//�޸Ĺݷ�
#define SUB_CP_C_DETECT_CLUBNAME	17									//�������
#define SUB_CP_C_DELETE_RECORD		18									//ɾ����¼

//Ӧ������
#define SUB_CP_S_LOGON_SUCCESS		100									//��¼�ɹ�
#define SUB_CP_S_LOGON_FAILURE		101									//��¼ʧ��
#define SUB_CP_S_LOGON_FINISH		102									//��¼���
#define SUB_CP_S_CLUB_OPTION		104									//����ѡ��
#define SUB_CP_S_CLUB_MEMBER		105									//�����Ա
#define SUB_CP_S_CLUB_NAMECARD		106									//������Ƭ
#define SUB_CP_S_CLUB_DELETE		107									//�����Ƴ�
#define SUB_CP_S_APPLY_MESSAGE		109									//������Ϣ
#define SUB_CP_S_APPLY_DELETE		111									//����ɾ��
#define SUB_CP_S_SEARCH_RESULT		112									//�������
#define SUB_CP_S_DETECT_RESULT		113									//�����

//�������
#define SUB_CP_S_OPERATE_SUCCESS	200									//�����ɹ�
#define SUB_CP_S_OPERATE_FAILURE	201									//����ʧ��	

//////////////////////////////////////////////////////////////////////////////////

//��������
struct CMD_CP_C_SearchClub
{
	DWORD							dwClubID;							//�����ʶ
};

//��������
struct CMD_CP_C_CreateClub
{
	//�������
	BYTE							cbPayType;							//֧������	
	BYTE							cbCurrencyKind;						//��������	
	DWORD							dwSwitchOption;						//����ѡ��	

	//��������
	WORD							wClubKind;							//�������
	WORD							wClubIconID;						//ͼ���ʶ
	TCHAR							szClubName[LEN_CLUB_NAME];			//��������
	TCHAR							szClubIntroduce[LEN_CLUB_INTRODUCE];//�������
};

//���²���
struct CMD_CP_C_UpdateClub
{	
	//������Ϣ
	DWORD							dwClubID;							//�����ʶ
	BYTE							cbUpdateKind;						//��������
};

//ɾ������
struct CMD_CP_C_DeleteClub
{
	//������Ϣ
	DWORD							dwClubID;							//�����ʶ
};

//ɾ����Ա
struct CMD_CP_C_DeleteMember
{
	//������Ϣ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	BYTE							cbDeleteKind;						//ɾ������
};

//�޸Ĺ�Ա
struct CMD_CP_C_UpdateMember
{
	//������Ϣ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	WORD							wUpdateKind;						//�޸�����
};

//��������
struct CMD_CP_C_ApplyRequest
{
	DWORD							dwClubID;							//�����ʶ
};

//������Ӧ
struct CMD_CP_C_ApplyRespond
{
	//������Ϣ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwApplyerID;						//�û���ʶ	
	BYTE							cbApplyStatus;						//����״̬
	DWORD							dwMessageID;						//��Ϣ��ʶ
};

//����ɾ��
struct CMD_CP_C_ApplyDelete
{
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwMessageID;						//��Ϣ��ʶ
	BYTE							cbMessageType;						//��Ϣ����
};

//���볷��
struct CMD_CP_C_ApplyRepeal
{
	DWORD							dwMessageID;						//��Ϣ��ʶ
};

//����ɾ��
struct CMD_CP_C_SettleBattle
{
	DWORD							dwDateID;							//ʱ���ʶ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	WORD							wSettleCount;						//�������
};

//�޸Ĺݷ�
struct CMD_CP_C_ModifyScore
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//������ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	LONGLONG						lModifyScore;						//�������

};

//�������
struct CMD_CP_C_DetectClubName
{
	TCHAR							szClubName[LEN_CLUB_NAME];			//��������
};

//ɾ����¼
struct CMD_CP_C_DeleteRecord
{
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwRecordID;							//��¼��ʶ
	BYTE							cbRecordKind;						//��¼����	
};

////����ݷ�
//struct CMD_CP_C_CleanupScore
//{
//	//��Ա��Ϣ
//	DWORD							dwClubID;							//������ʶ
//	DWORD							dwMemberID;							//��Ա��ʶ
//	BYTE							cbCleanupObject;					//�������
//	DWORD							dwCreaterGameID;					//��Ϸ��ʶ
//};
//
////���˰��
//struct CMD_CP_C_CleanupRevenue
//{
//	//��Ա��Ϣ
//	DWORD							dwClubID;							//��ݱ�ʶ
//	DWORD							dwMemberID;							//��Ա��ʶ
//	BYTE							cbCleanupObject;					//�������
//	DWORD							dwCreaterGameID;					//��Ϸ��ʶ
//
//};
//
////��������
//struct CMD_CP_C_CleanupSetup
//{
//	DWORD							dwClubID;							//��ݱ�ʶ
//	bool							bAutoCleanup;						//�Զ�����		
//};
//
////������а�
//struct CMD_CP_C_CleanupRankList
//{
//	//��Ա��Ϣ
//	DWORD							dwClubID;							//��ݱ�ʶ
//	DWORD							dwMemberID;							//��Ա��ʶ
//	BYTE							cbListKind;							//������	
//	BYTE							cbCleanupObject;					//�������
//	DWORD							dwCreaterGameID;					//��Ϸ��ʶ
//};

//��½�ɹ�
struct CMD_CP_S_LogonSuccess
{	
};

//��¼ʧ��
struct CMD_CP_S_LogonFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szErrorDescribe[128];				//������Ϣ
};

//��������
struct CMD_CP_S_ClubOption
{
	tagClubOption					ClubOption;							//��������
};

//�������
struct CMD_CP_S_ClubUpdate
{
	DWORD							dwClubID;							//�����ʶ
	BYTE							cbUpdateKind;						//��������
};

//�����Ƴ�
struct CMD_CP_S_ClubDelete
{
	DWORD							dwClubID;							//�����ʶ
	TCHAR							szNotifyContent[128];				//��ʾ��Ϣ
};

//��Ա����
struct CMD_CP_S_MemberUpdate
{
	DWORD							dwClubID;							//�����ʶ
	tagClubMemberInfo				ClubMember;							//��Ա��Ϣ
};

//�������
struct CMD_CP_S_SearchResult
{
	tagClubNameCard					ClubNameCard;						//������Ƭ
};

//������Ϣ
struct CMD_CP_S_ApplyMessage
{
	BYTE							cbMessageType;						//��Ϣ����
	WORD							wMessageCount;						//��Ϣ����
};

//����ɾ��
struct CMD_CP_S_ApplyDelete
{
	WORD							wMessageCount;						//��Ϣ����
};

//�û��Ƹ�
struct CMD_CP_S_WealthUpdate
{
	BYTE							cbWealthMask;						//�Ƹ�����
	SCORE							lUserCard;							//�û�����
	SCORE							lUserClubScore;						//�û���ݷ�
};

//�����
struct CMD_CP_S_DetectResult
{
	bool							bUsable;							//���ñ�־
	TCHAR							szErrorDescribe[128];				//��������
};

//
////����ݷ�
//struct CMD_CP_S_CleanupScore
//{
//	//��Ա��Ϣ
//	DWORD							dwClubID;							//������ʶ
//	DWORD							dwMemberID;							//��Ա��ʶ
//	BYTE							cbCleanupObject;					//�������
//};
//
////���˰��
//struct CMD_CP_S_CleanupRevenue
//{
//	//��Ա��Ϣ
//	DWORD							dwClubID;							//��ݱ�ʶ
//	DWORD							dwMemberID;							//��Ա��ʶ	
//	BYTE							cbCleanupObject;					//�������
//};
//
////��������
//struct CMD_CP_S_CleanupSetup
//{
//	DWORD							dwClubID;							//��ݱ�ʶ
//	bool							bAutoCleanup;						//�Զ�����	
//};
//
////������а�
//struct CMD_CP_S_CleanupRankList
//{
//	DWORD							dwClubID;							//��ݱ�ʶ
//	DWORD							dwMemberID;							//��Ա��ʶ
//	BYTE							cbListKind;							//������	
//	BYTE							cbCleanupObject;					//�������		
//};
//
////�����������
//struct CMD_CP_S_CleanupCooperate
//{
//	DWORD							dwClubID;							//��ݱ�ʶ
//	DWORD							dwParentID;							//�ϼ���ʶ
//	WORD							wMemberType;						//��Ա����		
//};

//�����ɹ�
struct CMD_CP_S_OperateSuccess
{
	LONG							lOperateCode;						//��������
	TCHAR							szOperateNotice[128];				//������ʾ
};

//����ʧ��
struct CMD_CP_S_OperateFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szErrorDescribe[128];				//������Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif