#ifndef CMD_RANKING_SERVER_HEAD_FILE
#define CMD_RANKING_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////

//��λ����
#define MDM_AR_RANKING_SERVICE		10									//��λ����

//////////////////////////////////////////////////////////////////////////////////
//ƥ�����
#define SUB_AR_C_START_MATCH		1									//��ʼƥ��
#define SUB_AR_C_CANCEL_MATCH		2									//ȡ��ƥ��

//��ѯ����
#define SUB_AR_C_QUERY_RANKING_CONFIG	10								//��ѯ��λ����
#define SUB_AR_C_QUERY_RANKING_INFO		11								//��ѯ���а�

//ƥ����
#define SUB_AR_S_MATCH_SUCCESS		100									//ƥ��ɹ�

//��ѯ���
#define SUB_AR_S_RANKING_CONFIG_INFO	150								//��λ������Ϣ
#define SUB_AR_S_RANKING_INFO			151								//���а���Ϣ

//�������
#define SUB_AR_S_OPERATE_SUCCESS	200									//�����ɹ�
#define SUB_AR_S_OPERATE_FAILURE	201									//����ʧ��

//////////////////////////////////////////////////////////////////////////////////

//�������
#define RANKING_CODE_STOP_SERVICE		1								//ֹͣ����
#define RANKING_CODE_TABLE_UNFOUND		2								//���Ӳ���
#define RANKING_CODE_LOVEHEART_UNENOUGH	3								//���Ĳ���
#define RANKING_CODE_COST_UNENOUGH		4								//�����Ѳ���

//////////////////////////////////////////////////////////////////////////////////

//��ʼƥ��
struct CMD_AR_C_StartMatch
{
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

//ȡ��ƥ��
struct CMD_AR_C_CancelMatch
{	
};

//ƥ��ɹ�
struct CMD_AR_S_MatchSuccess
{
	//������Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wTableID;							//���ӱ�ʶ
	WORD							wServerID;							//�����ʶ
	DWORD							dwRankingNum;						//��λ���	
	TCHAR							szPassword[LEN_PASSWORD];			//��������

	//������Ϣ
	WORD							wServerPort;						//����˿�
	TCHAR							szServerDomain[LEN_DOMAIN];			//��������

	//�û���Ϣ
	WORD							wUserCount;							//�û�����
	tagRankingTableUser				TableUserInfo[TABLE_USER_COUNT];	//�û���Ϣ
};

//��ѯ��λ����
struct CMD_AR_S_QueryRankingConfig
{
	tagRankingConfig				RankingConfig;						//��λ����
};

//���а���Ϣ
struct CMD_AN_S_RankingInfo
{
    //������Ϣ
	WORD							wRankID;							//�Լ�����
    WORD							wRankCount;							//��������
    tagRankingRankItem				RankingRankItem[0];					//��������
};

//�����ɹ�
struct CMD_AR_S_OperateSuccess
{
	WORD							wRequestCmdID;						//��������
	WCHAR							szDescribeString[256];				//������Ϣ	
};

//����ʧ��
struct CMD_AR_S_OperateFailure
{
	//������Ϣ
	LONG							lErrorCode;							//ʧ�ܱ���
	WCHAR							szErrorMessage[128];				//������Ϣ	
};

//////////////////////////////////////////////////////////////////////////////////

//��λ����
#define MDM_RS_RANKING_SERVICE		11									//��λ����

//////////////////////////////////////////////////////////////////////////////////
//���Ӳ���
#define SUB_RS_C_CREATE_TABLE		301									//��������
#define SUB_RS_C_DISMISS_TABLE		303									//��ɢ����
#define SUB_RS_C_SYNCHRO_TABLE		304									//ͬ������
#define SUB_RS_C_RANKING_OPTION		305									//��λѡ��

//���Ӳ���
#define SUB_RS_S_CREATE_TABLE		401									//�������
#define SUB_RS_S_TABLE_PARAM		404									//���Ӳ���
#define SUB_RS_S_TABLE_STATUS		405									//����״̬
#define SUB_RS_S_TABLE_SYNCHRO		406									//����ͬ��
#define SUB_RS_S_TABLE_DISMISS		407									//���ӽ�ɢ

//֪ͨ��Ϣ
#define SUB_RS_S_USER_SITDOWN		500									//�û�����
#define SUB_RS_S_USER_STANDUP		501									//�û�����

//////////////////////////////////////////////////////////////////////////////////

//��������
struct CMD_RS_C_CreateTable
{
	//������Ϣ
	DWORD							dwRankingNum;						//��λ���		
	WCHAR							szTablePassword[LEN_PASSWORD];		//��������

	//������Ϣ
	tagRankingConfig				RankingConfig;						//��λ����	
};

//��ɢ����
struct CMD_RS_C_DismissTable
{
	//������Ϣ
	WORD							wTableID;							//���ӱ�ʶ
	DWORD							dwRankingNum;						//���ӱ��	
};

//��λѡ��
struct CMD_RS_C_RankingOption
{
	bool							bServiceStatus;						//�����ʶ
};

//�������
struct CMD_RS_S_CreateTable
{
	//������Ϣ
	WORD							wTableID;							//���ӱ��	
	DWORD							dwRankingNum;						//ӳ����
	DWORD							dwCreateTime;						//����ʱ��

	//�����Ϣ
	BYTE							cbResultCode;						//�������
};

//���ӽ�ɢ
struct CMD_RS_S_TableDismiss
{
	WORD							wTableID;							//���ӱ��
	WORD							wServerID;							//�����ʶ
	DWORD							dwRankingNum;						//ӳ����
};

//���Ӳ���
struct CMD_RS_S_TableParam
{
	DWORD							dwRankingNum;						//ӳ����
	DWORD							dwStartTime;						//��ʼʱ��
	WORD							wFinishCount;						//��ɾ���		
};

//����״̬
struct CMD_RS_S_TableStatus
{
	DWORD							dwRankingNum;						//��λ���
	BYTE							cbGameStatus;						//��Ϸ״̬
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif