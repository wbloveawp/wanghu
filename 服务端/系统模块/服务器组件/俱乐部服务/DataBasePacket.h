#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//�������ݰ�

//ϵͳ����
#define DBR_GR_LOAD_CLUB_LIST		1									//���ز���
#define DBR_GR_LOAD_CLUB_ITEM		2									//���Ҳ���
#define DBR_GR_LOAD_CLUB_MEMBER		3									//�����Ա
#define DBR_GR_LOAD_KIND_SETTING	4									//��������
#define DBR_GR_LOAD_BATTLE_CONFIG	5									//�����淨
#define DBR_GR_LOAD_APPLY_MESSAGE	6									//������Ϣ	

//��Ա����
#define DBR_GR_QUERY_MEMBER			10									//��ѯ��Ա
#define DBR_GR_UPDATE_MEMBER		11									//���³�Ա

//Լս�淨
#define DBR_GR_APPEND_CONFIG		20									//����淨
#define DBR_GR_MODIFY_CONFIG		21									//�޸��淨
#define DBR_GR_DELETE_CONFIG		22									//ɾ���淨

//��Ϸ����
#define DBR_GR_SET_WINNER_SCORE		30									//����Ӯ��

//////////////////////////////////////////////////////////////////////////
//�����Ϣ

//�����б�
#define DBO_GR_CLUB_LIST			100									//��������
#define DBO_GR_CLUB_LIST_FINISH		101									//�б����

//��������
#define DBO_GR_CLUB_ITEM			110									//��������
#define DBO_GR_CLUB_ITEM_FINISH		111									//�������	

//��������
#define DBO_GR_CLUB_MEMBER			120									//�����Ա
#define DBO_GR_CLUB_CREATER			121									//����Ⱥ��
#define DBO_GR_UPDATE_MEMBER		122									//���³�Ա

//Լս�淨
#define DBO_GR_BATTLE_CONFIG		130									//Լս�淨	
#define DBO_GR_APPEND_CONFIG		131									//����淨
#define DBO_GR_MODIFY_CONFIG		132									//�޸��淨
#define DBO_GR_DELETE_CONFIG		133									//ɾ���淨

//��Ϸ����
#define DBO_GR_KIND_SETTING			140									//��Ϸ����
#define DBO_GR_MODIFY_SETTING		141									//�����޸�

//������Ϣ
#define DBO_GR_APPLY_MESSAGE		150									//������Ϣ	

//�����ṹ
#define DBO_GR_OPERATE_FAILURE		300									//����ʧ��

//////////////////////////////////////////////////////////////////////////////////
//������Ϣ

//���ز����б�
struct DBR_GR_LoadClubList
{
	DWORD							dwClubID;							//�����ʶ	
};

//���ز�����Ϣ
struct DBR_GR_LoadClubItem
{
	DWORD							dwClubID;							//�����ʶ
	bool							bActiveRoute;						//����·��
};

//�����Ա
struct DBR_GR_LoadClubMember
{
	DWORD							dwClubID;							//�����ʶ
};

//Լս�淨
struct DBR_GR_LoadBattleConfig
{
	DWORD							dwClubID;							//�����ʶ
};

//������Ϣ
struct DBR_GR_LoadApplyMessage
{
	DWORD							dwClubID;							//�����ʶ
};

//��������  
struct DBR_GR_LoadKindSetting
{
	DWORD							dwClubID;							//�����ʶ
};

//���³�Ա
struct DBR_GR_UpdateMember
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwMemberID;							//��Ա��ʶ

	//��������							
	BYTE							cbUpdateKind;						//��������

	//Լս��Ϣ
	WORD							wIncCreateCount;					//��������
	WORD							wIncBattleCount;					//�������
};

//��ѯ��Ա
struct DBR_GR_QueryMember
{
	//��Ա��Ϣ
	DWORD							dwClubID;							//�����ʶ
	WORD							wMemberCount;						//��Ա����
	DWORD							dwMemberID[16];						//��Ա��ʶ	
};

//����淨
struct DBR_GR_AppendConfig
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ

	//Լս����
	DWORD							dwClubID;							//�����ʶ
	tagClubBattleConfig				BattleConfig;						//Լս�淨
};

//�޸��淨
struct DBR_GR_ModifyConfig
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbModifyKind;						//�޸�����

	//Լս����	
	DWORD							dwClubID;							//�����ʶ
	tagClubBattleConfig				BattleConfig;						//Լս�淨
};

//ɾ���淨
struct DBR_GR_DeleteConfig
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ

	//������Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwConfigID;							//�淨��ʶ
};

//����Ӯ��
struct DBR_GR_SetWinnerScore
{
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lMinWinnerScore;					//���Ӯ��
};

//////////////////////////////////////////////////////////////////////////////////
//�����Ա
struct DBO_GR_ClubMember
{
	DWORD							dwClubID;							//�����ʶ
};

//�������
struct DBO_GR_ClubItemFinish
{
	DWORD							dwClubID;							//�����ʶ
	bool							bActiveRoute;						//����·��
};

//Լս�淨
struct DBO_GR_BattleConfig
{
	DWORD							dwClubID;							//�����ʶ
};

//����淨
struct DBO_GR_AppendConfig
{
	//Լս����
	DWORD							dwClubID;							//�����ʶ
	tagClubBattleConfig				BattleConfig;						//Լս�淨
};

//�޸��淨
struct DBO_GR_ModifyConfig
{
	//Լս����	
	DWORD							dwClubID;							//�����ʶ
	BYTE							cbModifyKind;						//�޸�����
	tagClubBattleConfig				BattleConfig;						//Լս�淨
};

//ɾ���淨
struct DBO_GR_DeleteConfig
{
	//������Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwConfigID;							//�淨��ʶ
};

//������Ϣ
struct DBO_GR_ApplyMessage
{
	DWORD							dwClubID;							//�����ʶ
};

//���³�Ա
struct DBO_GR_UpdateMember
{
	DWORD							dwClubID;							//�����ʶ
	BYTE							cbUpdateKind;						//��������
	tagClubMemberInfo				ClubMember;							//�����Ա
};

//��Ϸ����
struct DBO_GR_ClubKindSetting
{
	DWORD							dwClubID;							//�����ʶ	
	WORD							wItemCount;							//������Ŀ
	tagClubKindSetting				KindSettingItem[0];					//��������
};

//��Ϸ����
struct DBO_GR_KindSettingModify
{
	DWORD							dwClubID;							//�����ʶ	
	tagClubKindSetting				KindSettingItem;					//��������
};

//����ʧ��
struct DBO_GR_OperateFailure
{
	//������Ϣ
	LONG							lErrorCode;							//ʧ�ܱ���
	WCHAR							szErrorMessage[128];				//������Ϣ	
};

//////////////////////////////////////////////////////////////////////////////////

#endif