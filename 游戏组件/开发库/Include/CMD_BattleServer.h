#ifndef CMD_BATTLE_SERVER_HEAD_FILE
#define CMD_BATTLE_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////

//Լս����
#define MDM_AB_BATTLE_SERVICE		10									//Լս����

//////////////////////////////////////////////////////////////////////////////////
//��ѯѡ��
#define SUB_AB_C_QUERY_OPTION		1									//��ѯ����	

//���Ӳ���
#define SUB_AB_C_QUERY_TABLE		10									//��ѯ����
#define SUB_AB_C_CREATE_TABLE		11									//��������
#define SUB_AB_C_MODIFY_TABLE		12									//�޸�����
#define SUB_AB_C_DISMISS_TABLE		13									//��ɢ����
#define SUB_AB_C_REQUEST_TABLELIST	14									//�����б�
#define SUB_AB_C_REQUEST_TABLEPARAM	15									//���Ӳ���

//������Ϣ
#define SUB_AB_S_BATTLE_BASE_OPTION	100									//����ѡ��
#define SUB_AB_S_BATTLE_GAME_OPTION	101									//��Ϸѡ��	
#define SUB_AB_S_BATTLE_OPTION_FINISH 102								//ѡ�����

//�Ƹ���Ϣ
#define SUB_AB_S_WEALTH_UPDATE		110									//�Ƹ�����	

//�������
#define SUB_AB_S_TABLE_ITEM			200									//��������
#define SUB_AB_S_TABLE_LIST			201									//�����б�
#define SUB_AB_S_TABLE_PARAM		202									//���Ӳ���
#define SUB_AB_S_DISMISS_TABLE		203									//��ɢ����

//�������
#define SUB_AB_S_OPERATE_SUCCESS	210									//�����ɹ�
#define SUB_AB_S_OPERATE_FAILURE	211									//����ʧ��

//////////////////////////////////////////////////////////////////////////////////

//�������
#define BATTLE_CODE_STOP_SERVICE	1									//ֹͣ����
#define BATTLE_CODE_QUERY_FAILED	2									//��ѯʧ��
#define BATTLE_CODE_CREATE_FAILED	3									//����ʧ��
#define BATTLE_CODE_MODIFY_FAILED	4									//�޸�ʧ��
#define BATTLE_CODE_TABLE_UNFOUND	5									//���Ӳ���

//////////////////////////////////////////////////////////////////////////////////

//��ѯ����
struct CMD_AD_C_QueryOption
{
	WORD							wKindID;							//���ͱ�ʶ
	BYTE							cbEmployScene;						//ʹ�ó���
};

//��ѯ����
struct CMD_AB_C_QueryTable
{
	//��ѯ��Ϣ
	BYTE							cbQueryKind;						//��ѯ����
	DWORD							dwQueryArgKey;						//��ѯ�ؼ���		
};

//��������
struct CMD_AB_C_CreateTable
{
	//�û���Ϣ
	WCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	WCHAR							szTablePassword[LEN_PASSWORD];		//��������

	//Լս����
	tagBattleConfig					BattleConfig;						//Լս����	
};

//�޸�����
struct CMD_AB_C_ModifyTable
{
	DWORD							dwBattleNum;						//Լս���
	tagBattleConfig					BattleConfig;						//Լս����	
};

//��ɢ����
struct CMD_AB_C_DismissTable
{
	DWORD							dwBattleNum;						//���ӱ��	
};

//���������б�
struct CMD_AB_C_RequestTableList
{
	WORD							wTableCount;						//��������
};

//��������״̬
struct CMD_AB_C_RequestTableParam
{
	WORD							wTableCount;						//��������
	DWORD							dwBattleNum[0];						//���ӱ��
};

//�Ƹ�����
struct CMD_AB_S_WealthUpdate
{
	BYTE							cbMask;								//��������
	LONGLONG						lUserIngot;							//�û���ʯ		
	LONGLONG						lUserRoomCard;						//�û�����		
};

//��������
struct CMD_AB_S_TableItem
{
	//��������	
	tagBattleTableItem				TableItem;							//��������	
};

//�����б�
struct CMD_AB_S_TableList
{
	//������Ϣ
	WORD							wTableCount;						//������Ŀ
};

//�û�����
struct CMD_AB_S_UserSitdown
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwBattleNum;						//ӳ����
	tagBattleTableUser				TableUserItem;						//�û���Ϣ
};

//�û�����
struct CMD_AB_S_UserStandup
{
	WORD							wChairID;							//�û�����
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwBattleNum;						//ӳ����	
};

//���ӽ�ɢ
struct CMD_AB_S_TableDismiss
{
	DWORD							dwBattleNum;						//ӳ����
};

//��ɢ����
struct CMD_AB_S_DismissTable
{
	BYTE							cbResultCode;						//�������
	TCHAR							szDescribeString[256];				//������Ϣ	
};

//�����ɹ�
struct CMD_AB_S_OperateSuccess
{
	LONG							lOperateCode;						//��������
	TCHAR							szDescribeString[256];				//������Ϣ	
};

//����ʧ��
struct CMD_AB_S_OperateFailure
{
	//������Ϣ
	LONG							lErrorCode;							//ʧ�ܱ���
	TCHAR							szErrorMessage[128];				//������Ϣ	
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

//���Ӳ���
#define SUB_BS_S_CREATE_TABLE		401									//�������
#define SUB_BS_S_MODIFY_TABLE		402									//�޸Ľ��
#define SUB_BS_S_DISMISS_TABLE		403									//��ɢ����
#define SUB_BS_S_TABLE_PARAM		404									//���Ӳ���
#define SUB_BS_S_TABLE_STATUS		405									//����״̬
#define SUB_BS_S_TABLE_SYNCHRO		406									//����ͬ��
#define SUB_BS_S_TABLE_DISMISS		407									//���ӽ�ɢ
#define SUB_BS_S_TABLE_USERLIST		408									//�����û�
#define SUB_BS_S_UPDATE_CONFIG		409									//��������	

//֪ͨ��Ϣ
#define SUB_BS_S_USER_SITDOWN		500									//�û�����
#define SUB_BS_S_USER_STANDUP		501									//�û�����

//////////////////////////////////////////////////////////////////////////////////

//��������
struct CMD_BS_C_CreateTable
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ	
	WCHAR							szTablePassword[LEN_PASSWORD];		//��������

	//������Ϣ
	DWORD							dwBattleNum;						//ӳ����
	tagBattleConfigEx				BattleConfig;						//Լս����

	//������Ϣ
	DWORD							dwClientIP;							//�ͻ���ַ
	WCHAR							szMachineID[LEN_MACHINE_ID];		//��������		
};

//�޸�����
struct CMD_BS_C_ModifyTable
{
	//������Ϣ
	WORD							wTableID;							//���ӱ�ʶ
	DWORD							dwUserID;							//�û���ʶ	
	DWORD							dwBattleNum;						//������
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

//�������
struct CMD_BS_S_CreateTable
{
	//������Ϣ
	WORD							wTableID;							//���ӱ��	
	DWORD							dwBattleNum;						//ӳ����
	DWORD							dwCreateTime;						//����ʱ��

	//������Ϣ
	LONGLONG						lWinScore;							//Ӯȡ����

	//�����Ϣ
	BYTE							cbResultCode;						//�������
	WCHAR							szErrorMessage[128];				//������Ϣ
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
	WCHAR							szErrorMessage[128];				//������Ϣ
};

//��ɢ���
struct CMD_BS_S_DismissTable
{
	//������Ϣ
	tagSocketInfo					SocketInfo;							//������Ϣ

	//�����Ϣ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbResultCode;						//�������
	WCHAR							szDescribeString[256];				//������Ϣ	
};

//���ӽ�ɢ
struct CMD_BS_S_TableDismiss
{
	//������Ϣ
	WORD							wTableID;							//���ӱ��
	WORD							wServerID;							//�����ʶ
	DWORD							dwBattleNum;						//ӳ����
};

//���Ӳ���
struct CMD_BS_S_TableParam
{
	DWORD							dwBattleNum;						//ӳ����
	DWORD							dwStartTime;						//��ʼʱ��
	WORD							wFinishCount;						//��ɾ���		
};

//����״̬
struct CMD_BS_S_TableStatus
{
	DWORD							dwBattleNum;						//ӳ����
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
	DWORD							dwOwnerID;							//������ʶ
};

//Լս���
struct CMD_BS_S_BattleFinish
{
	//������Ϣ
	WORD							wTableID;							//���ӱ��	
	DWORD							dwBattleNum;						//ӳ����
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif