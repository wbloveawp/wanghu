#ifndef CMD_CLUB_SERVICE_HEAD_FILE
#define CMD_CLUB_SERVICE_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////

//�������
#define MDM_CLUB_SERVICE			10									//���ֲ�����

//��������
#define SUB_CB_C_ENTER_CLUB			1									//�������

//Լս����
#define SUB_CB_C_QUERY_OPTION		10									//��ѯ����
#define SUB_CB_C_CREATE_TABLE		11									//����Լս
#define SUB_CB_C_MODIFY_TABLE		12									//�޸�Լս
#define SUB_CB_C_DISMISS_TABLE		13									//��ɢԼս

//Լս�淨
#define SUB_CB_C_APPEND_CONFIG		20									//����淨
#define SUB_CB_C_MODIFY_CONFIG		21									//�޸��淨
#define SUB_CB_C_DELETE_CONFIG		22									//ɾ���淨

//��Ϸ����
#define SUB_CB_C_SET_WINNER_SCORE	30									//����Ӯ��

//��������
#define SUB_CB_S_CLUB_ITEM			100									//������Ϣ
#define SUB_CB_S_CLUB_MEMBER		101									//�����Ա
#define SUB_CB_S_CLUB_UPDATE		102									//�������
#define SUB_CB_S_MEMBER_INSERT		103									//��Ա���
#define SUB_CB_S_MEMBER_UPDATE		104									//��Ա����
#define SUB_CB_S_MEMBER_DELETE		105									//��Աɾ��
#define SUB_CB_S_APPLY_MESSAGE		106									//������Ϣ
#define SUB_CB_S_APPLY_DELETE		107									//����ɾ��

//Լս�淨
#define SUB_CB_S_BATTLE_CONFIG		120									//Լս�淨
#define SUB_CB_S_CONFIG_APPEND		121									//�淨���
#define SUB_CB_S_CONFIG_MODIFY		122									//�淨�޸�
#define SUB_CB_S_CONFIG_DELETE		123									//�淨ɾ��

//��Ϸ����
#define SUB_CB_S_KIND_SETTING_ITEM	 130								//��Ϸ����
#define SUB_CB_S_KIND_SETTING_MODIFY 131								//���ø���

//Լս����
#define SUB_CB_S_TABLE_ITEM			150									//��������
#define SUB_CB_S_TABLE_PARAM		151									//���Ӳ���
#define SUB_CB_S_TABLE_CREATE		152									//���ӽ�ɢ
#define SUB_CB_S_TABLE_UPDATE		153									//���Ӹ���
#define SUB_CB_S_TABLE_DISMISS		154									//���ӽ�ɢ
#define SUB_CB_S_USER_SITDOWN		155									//�û�����
#define SUB_CB_S_USER_STANDUP		156									//�û�����
#define SUB_CB_S_TABLE_CLEANUP		157									//�������

//Լս����
#define SUB_CB_S_BATTLE_BASE_OPTION	  160								//����ѡ��
#define SUB_CB_S_BATTLE_GAME_OPTION	  161								//��Ϸѡ��	
#define SUB_CB_S_BATTLE_OPTION_FINISH 162								//ѡ�����

//������
#define SUB_CB_S_ENTER_SUCCESS		500									//����ɹ�
#define SUB_CB_S_ENTER_FAILURE		501									//����ʧ��

//////////////////////////////////////////////////////////////////////////////////
//�޸�����
#define CLUB_BC_MK_WINNER_SCORE		0x01								//Ӯ�ҷ���
#define CLUB_BC_MK_BATTLE_CONFIG	0x02								//Լս�淨

//////////////////////////////////////////////////////////////////////////////////

//�������
struct CMD_CB_C_EnterClub
{
	DWORD							dwClubID;							//�����ʶ
};

//��ѯ����
struct CMD_CB_C_QueryOption
{
	WORD							wKindID;							//���ͱ�ʶ	
};

//��������
struct CMD_CB_C_CreateTable
{
	DWORD							dwClubID;							//�����ʶ	
	DWORD							dwConfigID;							//���ñ�ʶ
	tagBattleConfig					BattleConfig;						//Լս����
};

//�޸�����
struct CMD_CB_C_ModifyTable
{
	//�û���Ϣ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwBattleNum;						//���ӱ��
	tagBattleConfig					BattleConfig;						//Լս����		
};

//��ɢ����
struct CMD_CB_C_DismissTable
{
	//������Ϣ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwBattleNum;						//���ӱ��
};

//����淨
struct CMD_CB_C_AppendConfig
{
	DWORD							dwClubID;							//�����ʶ	
	tagBattleConfig					BattleConfig;						//Լս����
};

//�޸��淨
struct CMD_CB_C_ModifyConfig
{
	//�û���Ϣ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwConfigID;							//�淨��ʶ
	BYTE							cbModifyKind;						//�޸�����	
};

//ɾ���淨
struct CMD_CB_C_DeleteConfig
{
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwConfigID;							//�淨��ʶ	
};

//����Ӯ��
struct CMD_CB_C_SetWinnerScore
{
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwClubID;							//�����ʶ
	SCORE							lMinWinnerScore;					//���Ӯ��
};

//�����Ա
struct CMD_CB_S_ClubMember
{
	DWORD							dwClubID;							//�����ʶ
};

//�������
struct CMD_CB_S_ClubUpdate
{
	DWORD							dwClubID;							//�����ʶ
	BYTE							cbUpdateKind;						//��������
};

//��Ա����
struct CMD_CB_S_MemberInsert
{
	DWORD							dwClubID;							//�����ʶ
	WORD							wMemberCount;						//��Ա����
	tagClubMemberUser				MemberUser;							//��Ա�û�	
};

//��Ա����
struct CMD_CB_S_MemberUpdate
{
	DWORD							dwClubID;							//�����ʶ
	tagClubMemberInfo				ClubMember;							//��Ա��Ϣ
};

//��Աɾ��
struct CMD_CB_S_MemberDelete
{
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwMemberID;							//��Ա��ʶ
	WORD							wMemberCount;						//��Ա����
};

//������Ϣ
struct CMD_CB_S_ApplyMessage
{
	WORD							wMessageCount;						//��Ϣ����
};

//����ɾ��
struct CMD_CB_S_ApplyDelete
{
	WORD							wMessageCount;						//��Ϣ����
};

//Լս�淨
struct CMD_CB_S_BattleConfig
{
	DWORD							dwClubID;							//�����ʶ
};

//�淨���
struct CMD_CB_S_ConfigAppend
{
	DWORD							dwClubID;							//�����ʶ
	tagClubBattleConfig				BattleConfig;						//Լս����
};

//�淨�޸�
struct CMD_CB_S_ConfigModify
{
	DWORD							dwClubID;							//�����ʶ
	BYTE							cbModifyKind;						//�޸�����
	tagClubBattleConfig				BattleConfig;						//Լս����
};

//�淨ɾ��
struct CMD_CB_S_ConfigDelete
{
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwConfigID;							//�淨��ʶ
};

//���ӽ�ɢ
struct CMD_CB_S_TableDismiss
{
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwBattleNum;						//ӳ����
};

//���Ӳ���
struct CMD_CB_S_TableParam
{
	DWORD							dwBattleNum;						//ӳ����
	DWORD							dwStartTime;						//��ʼʱ��
	WORD							wFinishCount;						//��ɾ���
	BYTE							cbGameStatus;						//��Ϸ״̬
};

//�û�����
struct CMD_CB_S_UserSitdown
{
	DWORD							dwUserID;							//�û���ʶ	
	DWORD							dwBattleNum;						//ӳ����	
	tagBattleTableUser				TableUserItem;						//�û���Ϣ
};

//�û�����
struct CMD_CB_S_UserStandup
{
	WORD							wChairID;							//�û�����
	DWORD							dwUserID;							//�û���ʶ	
	DWORD							dwBattleNum;						//ӳ����	
};

//�������
struct CMD_CB_S_TableCleanup
{
	DWORD							dwClubID;							//�����ʶ
};

//����ɹ�
struct CMD_CB_S_EnterSuccess
{
	DWORD							dwClubID;							//�����ʶ
};

//����ʧ��
struct CMD_CB_S_EnterFailure
{
	LONG							lErrorCode;							//�������
	WCHAR							szErrorDescribe[128];				//������Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif