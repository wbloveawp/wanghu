#ifndef CMD_BATTLE_SERVER_HEAD_FILE
#define CMD_BATTLE_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////

//Լս����
#define MDM_GB_BATTLE_SERVICE		10									//Լս����

//////////////////////////////////////////////////////////////////////////////////
//��ѯѡ��
#define SUB_GB_C_QUERY_OPTION		1									//��ѯ����	

//���Ӳ���
#define SUB_GB_C_QUERY_TABLE		10									//��ѯ����
#define SUB_GB_C_SEARCH_TABLE		11									//��������
#define SUB_GB_C_CREATE_TABLE		12									//��������
#define SUB_GB_C_MODIFY_TABLE		13									//�޸�����
#define SUB_GB_C_DISMISS_TABLE		14									//��ɢ����
#define SUB_GB_C_REQUEST_TABLELIST	15									//�����б�
#define SUB_GB_C_REQUEST_TABLEPARAM	16									//���Ӳ���

//������Ϣ
#define SUB_GB_S_BATTLE_BASE_OPTION	  100								//����ѡ��
#define SUB_GB_S_BATTLE_GAME_OPTION	  101								//��Ϸѡ��	
#define SUB_GB_S_BATTLE_OPTION_FINISH 102								//ѡ�����

//�Ƹ���Ϣ
#define SUB_GB_S_WEALTH_UPDATE		110									//�Ƹ�����

//�������
#define SUB_GB_S_SEARCH_RESULT		120									//���ҽ��

//�����б�
#define SUB_GB_S_TABLE_ITEM			200									//��������
#define SUB_GB_S_TABLE_PARAM		201									//���Ӳ���
#define SUB_GB_S_TABLE_FINISH		202									//�������

//����֪ͨ
#define SUB_GB_S_TABLE_CREATE		203									//���Ӵ���
#define SUB_GB_S_TABLE_UPDATE		204									//���Ӹ���
#define SUB_GB_S_TABLE_DISMISS		205									//���ӽ�ɢ

//////////////////////////////////////////////////////////////////////////////////

//�������
#define BATTLE_CODE_STOP_SERVICE	1									//ֹͣ����
#define BATTLE_CODE_QUERY_FAILED	2									//��ѯʧ��
#define BATTLE_CODE_SEARCH_FAILED	3									//����ʧ��
#define BATTLE_CODE_CREATE_FAILED	4									//����ʧ��
#define BATTLE_CODE_MODIFY_FAILED	5									//�޸�ʧ��
#define BATTLE_CODE_DISMISS_FAILED	6									//��ɢʧ��
#define BATTLE_CODE_TABLE_UNFOUND	7									//���Ӳ���

//////////////////////////////////////////////////////////////////////////////////

//��ѯ����
struct CMD_GB_C_QueryOption
{
	WORD							wKindID;							//���ͱ�ʶ	
};

//��������
struct CMD_GB_C_SearchTable
{
	DWORD							dwBattleNum;						//Լս���
};

//��������
struct CMD_GB_C_CreateTable
{
	//�û���Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szTablePassword[LEN_PASSWORD];		//��������

	//Լս����
	tagBattleConfig					BattleConfig;						//Լս����	
};

//�޸�����
struct CMD_GB_C_ModifyTable
{
	DWORD							dwBattleNum;						//Լս���
	tagBattleConfig					BattleConfig;						//Լս����	
};

//��ɢ����
struct CMD_GB_C_DismissTable
{
	DWORD							dwBattleNum;						//���ӱ��	
};

//���������б�
struct CMD_GB_C_RequestTableList
{
	WORD							wTableCount;						//��������
};

//��������״̬
struct CMD_GB_C_RequestTableParam
{
	WORD							wTableCount;						//��������
	DWORD							dwBattleNum[0];						//���ӱ��
};

//�Ƹ�����
struct CMD_GB_S_WealthUpdate
{
	BYTE							cbMask;								//��������
	LONGLONG						lUserIngot;							//�û���ʯ		
	LONGLONG						lUserRoomCard;						//�û�����		
};

//���ӽ�ɢ
struct CMD_GB_S_TableDismiss
{
	DWORD							dwBattleNum;						//Լս���
};

//////////////////////////////////////////////////////////////////////////////////

//Լս����
#define MDM_BS_BATTLE_SERVICE		11									//Լս����

//////////////////////////////////////////////////////////////////////////////////
//���Ӳ���
#define SUB_BS_C_CREATE_TABLE		301									//��������
#define SUB_BS_C_MODIFY_TABLE		302									//�޸�����
#define SUB_BS_C_DISMISS_TABLE		303									//��ɢ����
#define SUB_BS_C_SYNCHRO_TABLE		304									//ͬ������
#define SUB_BS_C_BATTLE_OPTION		305									//Լսѡ��

//�������
#define SUB_BS_C_MODIFY_CLUB_SCORE	310									//���¹ݷ�
#define SUB_BS_C_UPDATE_CLUB_INFO	312									//��������

//���Ӳ���
#define SUB_BS_S_CREATE_TABLE		401									//�������
#define SUB_BS_S_MODIFY_TABLE		402									//�޸Ľ��
#define SUB_BS_S_DISMISS_TABLE		403									//��ɢ����
#define SUB_BS_S_TABLE_PARAM		404									//���Ӳ���
#define SUB_BS_S_TABLE_SYNCHRO		405									//����ͬ��
#define SUB_BS_S_TABLE_DISMISS		406									//���ӽ�ɢ
#define SUB_BS_S_TABLE_USERLIST		407									//�����û�
#define SUB_BS_S_SYNCHRO_FINISH		408									//ͬ�����
#define SUB_BS_S_UPDATE_CONFIG		409									//��������	
#define SUB_BS_S_BATTLE_FINISH		410									//Լս���

//�������
#define SUB_BS_S_CLUB_SCORE			450									//��ݷ���	
#define SUB_BS_S_CLUB_WEALTH		453									//��ݲƸ�

//�û���̬
#define SUB_BS_S_USER_SITDOWN		500									//�û�����
#define SUB_BS_S_USER_STANDUP		501									//�û�����

//////////////////////////////////////////////////////////////////////////////////

//��������
struct CMD_BS_C_CreateTable
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szPassword[LEN_PASSWORD];			//��������

	//������Ϣ
	DWORD							dwClubID;							//�����ʶ
	WORD							wSwitchOption;						//����ѡ��
	WORD							wClubServiceID;						//�����ʶ
	DWORD							dwClubCreaterID;					//Ⱥ����ʶ	

	//������Ϣ
	DWORD							dwBattleNum;						//ӳ����
	DWORD							dwConfigID;							//�淨��ʶ
	tagBattleConfigEx				BattleConfig;						//Լս����	
};

//�޸�����
struct CMD_BS_C_ModifyTable
{
	//������Ϣ
	WORD							wTableID;							//���ӱ�ʶ
	DWORD							dwUserID;							//�û���ʶ	
	DWORD							dwBattleNum;						//���ӱ��
	tagBattleConfigEx				BattleConfig;						//Լս����	

	//������Ϣ
	tagSocketInfo					SocketInfo;							//������Ϣ
};

//��ɢ����
struct CMD_BS_C_DismissTable
{
	//������Ϣ
	tagSocketInfo					SocketInfo;							//������Ϣ

	//������Ϣ
	WORD							wTableID;							//���ӱ�ʶ
	DWORD							dwUserID;							//�û���ʶ	
	DWORD							dwBattleNum;						//���ӱ��
};

//ͬ������
struct CMD_BS_C_BattleOption
{
	bool							bBattleService;						//�����ʶ
};


//���²��
struct CMD_BS_C_UpdateClubInfo
{
	//������Ϣ
	WORD							wTableID;							//���ӱ�ʶ
	DWORD							dwClubID;							//��ݱ�ʶ
	DWORD							dwBattleNum;						//���ӱ��

	//�����Ϣ
	WORD							wSwitchOption;						//��ݹ���
	DWORD							dwClubCreaterID;					//������ʶ
};

//��ѯ�ݷ�
struct CMD_BS_C_ModifyClubScore
{
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbChangeType;						//�������
	LONGLONG						lChangeScore;						//�������

	//�����Ϣ
	DWORD							dwClubID;							//��ݱ�ʶ
	TCHAR							szClubName[LEN_CLUB_NAME];			//�������
	TCHAR							szOperaterNickName[LEN_ACCOUNTS];	//�û��ǳ�
};

//�������
struct CMD_BS_S_CreateTable
{
	//������Ϣ
	WORD							wTableID;							//���ӱ��	
	DWORD							dwBattleNum;						//ӳ����
	DWORD							dwCreateTime;						//����ʱ��

	//�Ƹ���Ϣ
	SCORE							lUserGold;							//�û����
	SCORE							lUserCard;							//�û�����
	DWORD							dwPayUserID;						//��ұ�ʶ

	//�����Ϣ
	LONG							lResultCode;						//�������
	TCHAR							szErrorMessage[128];				//������Ϣ
};

//�޸Ľ��
struct CMD_BS_S_ModifyTable
{
	//������Ϣ
	DWORD							dwUserID;							//�û���ʶ	
	DWORD							dwBattleNum;						//���ӱ��
	tagBattleConfigEx				BattleConfig;						//Լս����	

	//������Ϣ
	tagSocketInfo					SocketInfo;							//������Ϣ

	//�����Ϣ
	BYTE							cbResultCode;						//�������
	TCHAR							szErrorMessage[128];				//������Ϣ
};

//��ɢ���
struct CMD_BS_S_DismissTable
{
	//������Ϣ
	DWORD							dwBattleNum;						//Լս���
	tagSocketInfo					SocketInfo;							//������Ϣ

	//�����Ϣ
	BYTE							cbResultCode;						//�������
	TCHAR							szDescribeString[256];				//������Ϣ	
};

//���ӽ�ɢ
struct CMD_BS_S_TableDismiss
{
	//������Ϣ
	WORD							wTableID;							//���ӱ��
	WORD							wServerID;							//���ӱ�ʶ
	DWORD							dwBattleNum;						//ӳ����
};

//���Ӳ���
struct CMD_BS_S_TableParam
{
	DWORD							dwBattleNum;						//ӳ����
	DWORD							dwStartTime;						//��ʼʱ��
	WORD							wFinishCount;						//��ɾ���
	BYTE							cbGameStatus;						//��Ϸ״̬
};


//�����û�
struct CMD_BS_S_TableUserList
{
	DWORD							dwBattleNum;						//ӳ����
	WORD							wUserCount;							//��ɾ���	
	tagBattleTableUser				TableUserItem[TABLE_USER_COUNT];	//�û��б�
};

//�û�����
struct CMD_BS_S_UserSitdown
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwBattleNum;						//ӳ����
	tagBattleTableUser				TableUserItem;						//�û���Ϣ
};

//�û�����
struct CMD_BS_S_UserStandup
{
	WORD							wChairID;							//�û�����
	DWORD							dwUserID;							//�û���ʶ	
	DWORD							dwBattleNum;						//ӳ����	
};

//��������
struct CMD_BS_S_UpdateConfig
{
	DWORD							dwBattleNum;						//ӳ����	
	DWORD							dwPayUserID;						//֧�����
};

//��ݷ���
struct CMD_BS_S_ClubScore
{
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lRevenue;							//��Ϸ˰��
	SCORE							lClubScore;							//��ݷ���
	DWORD							dwBattleNum;						//ӳ����
};

//Ⱥ��Ƹ�
struct CMD_BS_S_ClubWealth
{
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwBattleNum;						//ӳ����

	//�Ƹ���Ϣ
	SCORE							lUserGold;							//�û����
	SCORE							lUserCard;							//��������
	DWORD							dwPayUserID;						//֧�����
};

//ͬ�����
struct CMD_BS_S_SynchroFinish
{
	WORD							wServerID;							//�����ʶ
};

//Լս���
struct CMD_BS_S_BattleFinish
{
	//������Ϣ
	WORD							wTableID;							//���ӱ��	
	DWORD							dwBattleNum;						//ӳ����
};


//////////////////////////////////////////////////////////////////////////////////

//Լս����
#define MDM_BC_BATTLE_SERVICE		12									//Լս����

//////////////////////////////////////////////////////////////////////////////////
//��ѯ����
#define SUB_BC_C_QUERY_OPTION		100									//��ѯ����
#define SUB_BC_C_MODIFY_SCORE		101									//�޸Ļ���
 
//��������
#define SUB_BC_C_CLUB_UPDATE		110									//�޸Ĳ���
#define SUB_BC_C_CLUB_DELETE		111									//ɾ������
#define SUB_BC_C_CLUB_TRANSFER		112									//ת�ò���

//Լս����
#define SUB_BC_C_QUERY_TABLE		120									//��ѯ����
#define SUB_BC_C_CREATE_TABLE		121									//��������
#define SUB_BC_C_MODIFY_TABLE		122									//�޸�����
#define SUB_BC_C_DISMISS_TABLE		123									//��ɢ����
#define SUB_BC_C_EFFICACY_RESULT	125									//��֤���

//Լս֪ͨ
#define SUB_BC_S_TABLE_CREATE		500									//���Ӵ���
#define SUB_BC_S_TABLE_UPDATE		501									//���Ӹ���
#define SUB_BC_S_TABLE_DISMISS		502									//���ӽ�ɢ
#define SUB_BC_S_TABLE_ITEM			503									//��������
#define SUB_BC_S_TABLE_PARAM		504									//���Ӳ���
#define SUB_BC_S_BATTLE_FINISH		505									//Լս���
#define SUB_BC_S_USER_SITDOWN		506									//�û�����
#define SUB_BC_S_USER_STANDUP		507									//�û�����
#define SUB_BC_S_USER_EFFICACY		508									//�û���֤

//��������
#define SUB_BC_S_CLUB_SCORE			510									//��ݻ���
#define SUB_BC_S_CLUB_WEALTH		511									//��ݲƸ�

//������Ϣ
#define SUB_BC_S_BATTLE_BASE_OPTION	  520								//����ѡ��
#define SUB_BC_S_BATTLE_GAME_OPTION	  521								//��Ϸѡ��	
#define SUB_BC_S_BATTLE_OPTION_FINISH 522								//ѡ�����

//�������
#define SUB_BC_S_OPERATE_SUCCESS	800									//�����ɹ�
#define SUB_BC_S_OPERATE_FAILURE	801									//����ʧ��

//////////////////////////////////////////////////////////////////////////////////

//��ѯ����
struct CMD_BC_C_QueryOption
{
	WORD							wKindID;							//���ͱ�ʶ	
	BYTE							cbEmployScene;						//ʹ�ó���
	tagSocketInfo					SocketInfo;							//������Ϣ
};

//���²���
struct CMD_BC_C_ClubUpdate
{
	//������Ϣ
	DWORD							dwClubID;							//�����ʶ	
	BYTE							cbUpdateKind;						//��������	
};

//ɾ������
struct CMD_BC_C_ClubDelete
{
	DWORD							dwClubID;							//�����ʶ
};

//��������
struct CMD_BC_C_ClubUpgrade
{
	//�����ʶ
	DWORD							dwClubID;							//�����ʶ	
};

//ת�ò���
struct CMD_BC_C_ClubTransfer
{
	//�����ʶ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwNewCreaterID;						//�¹�����ʶ	
};

//��ѯ����
struct CMD_BC_C_QueryTable
{
	DWORD							dwClubID;							//�����ʶ
};

//��������
struct CMD_BC_C_CreateTable
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	

	//������Ϣ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwClubCreaterID;					//Ⱥ����ʶ
	DWORD							wSwitchOption;						//����ѡ��

	//Լս����
	DWORD							dwConfigID;							//�淨��ʶ	
	tagBattleConfig					BattleConfig;						//Լս����

	//������Ϣ
	tagSocketInfo					SocketInfo;							//������Ϣ
};

//�޸�����
struct CMD_BC_C_ModifyTable
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwBattleNum;						//���ӱ��

	//Լս����
	tagBattleConfig					BattleConfig;						//Լս����	

	//������Ϣ
	tagSocketInfo					SocketInfo;							//������Ϣ
};

//��ɢ����
struct CMD_BC_C_DismissTable
{
	//������Ϣ
	DWORD							dwBattleNum;						//���ӱ��

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����	

	//������Ϣ
	tagSocketInfo					SocketInfo;							//������Ϣ
};

//��ѯ����
struct CMD_BC_C_ModifyScore
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wServerID;							//���ӱ�ʶ

	//�����Ϣ
	BYTE							cbChangeType;						//�������
	SCORE							lChangeScore;						//�������

	//�����Ϣ
	DWORD							dwClubID;							//��ݱ�ʶ
	TCHAR							szClubName[LEN_CLUB_NAME];			//�������
	TCHAR							szOperaterNickName[LEN_ACCOUNTS];	//�û��ǳ�
};

//��֤���
struct CMD_BC_C_EfficacyResult
{
	bool							bResult;							//��֤���
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwBattleNum;						//ӳ����
	tagSocketInfo					UserSocketInfo;						//������Ϣ
	TCHAR							szErrorMessage[64];					//������Ϣ
};

//����淨
struct CMD_BC_C_AppendConfig
{
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ
	tagBattleConfig					BattleConfig;						//Լս����
};

//�޸��淨
struct CMD_BC_C_ModifyConfig
{
	//�û���Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwConfigID;							//�淨��ʶ
	tagBattleConfig					BattleConfig;						//Լս����		
};

//ɾ���淨
struct CMD_BC_C_DeleteConfig
{
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwConfigID;							//�淨��ʶ	
};

//���ӽ�ɢ
struct CMD_BC_S_TableDismiss
{
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwBattleNum;						//Լս���
	WORD							wUserCount;							//�û�����
};

//�淨���
struct CMD_BC_S_ConfigAppend
{
	DWORD							dwClubID;							//�����ʶ
	tagClubBattleConfig				BattleConfig;						//Լս�淨
};

//�淨�޸�
struct CMD_BC_S_ConfigModify
{
	DWORD							dwClubID;							//�����ʶ
	tagClubBattleConfig				BattleConfig;						//Լս�淨
};

//�淨ɾ��
struct CMD_BC_S_ConfigDelete
{
	WORD							wKindID;							//���ͱ�ʶ	
	DWORD							dwClubID;							//�����ʶ	
	DWORD							dwConfigID;							//�淨��ʶ
};

//���Ӳ���
struct CMD_BC_S_TableParam
{
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwBattleNum;						//ӳ����
	DWORD							dwStartTime;						//��ʼʱ��
	WORD							wFinishCount;						//��ɾ���
	BYTE							cbGameStatus;						//��Ϸ״̬
};

//Լս���
struct CMD_BC_S_BattleFinish
{
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwBattleNum;						//���ӱ��
	WORD							wUserCount;							//�û�����
};

//�û�����
struct CMD_BC_S_UserSitdown
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwBattleNum;						//ӳ����
	tagBattleTableUser				TableUserItem;						//�û���Ϣ
};

//�û�����
struct CMD_BC_S_UserStandup
{
	WORD							wChairID;							//�û�����
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwBattleNum;						//ӳ����	
};

//�û���֤
struct CMD_BC_S_UserEfficacy
{
	WORD							wBattleID;							//Լս��ʶ		
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwBattleNum;						//ӳ����	
	tagSocketInfo					UserSocketInfo;						//������Ϣ
};

//�������
struct CMD_BC_S_ClubScore
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//�����ʶ
	SCORE							lRevenue;							//��Ϸ˰��
	SCORE							lClubScore;							//�������
};

//����Ƹ�
struct CMD_BC_S_ClubWealth
{
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwPayUserID;						//������ʶ
	SCORE							lUserCard;							//��������
	BYTE							cbWealthMask;						//�Ƹ�����
};

//�����ɹ�
struct CMD_BC_S_OperateSuccess
{
	//������Ϣ
	DWORD							dwClubID;							//�����ʶ

	//������Ϣ
	tagSocketInfo					SocketInfo;							//������Ϣ

	//�����Ϣ
	LONG							lOperateCode;						//��������
	TCHAR							szDescribeString[256];				//������Ϣ	
};

//����ʧ��
struct CMD_BC_S_OperateFailure
{
	//������Ϣ
	DWORD							dwClubID;							//�����ʶ

	//������Ϣ
	tagSocketInfo					SocketInfo;							//������Ϣ

	//������Ϣ
	LONG							lErrorCode;							//ʧ�ܱ���
	TCHAR							szErrorMessage[128];				//������Ϣ	
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif