#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//�������ݰ�

//ϵͳ����
#define DBR_LOAD_GLOBAL_CONFIG		1									//��������

//��ѯ����
#define DBR_QUERY_STOCK_ITEM		2									//��ѯ���
#define DBR_UPDATE_STOCK_SCORE		5									//���¿��
#define DBR_UPDATE_STOCK_STATE		6									//����״̬
#define DBR_UPDATE_JACKPOT_SCORE	7									//���²ʽ�
#define DBR_ADJUST_STOCK_SCORE		8									//�������
#define DBR_ADJUST_JACKPOT_SCORE	9									//�����ʽ�	

//////////////////////////////////////////////////////////////////////////
//�¼���ʶ

//��������
struct DBR_LoadGlobalConfig
{
	DWORD							dwConfigMask;						//��������
};

//��ѯ���
struct DBR_QueryStockItem
{
	WORD							wStockID;							//����ʶ
	WORD							wCommandID;							//�����ʶ
};

//�������
struct DBR_AdjustStockScore
{
	WORD							wStockID;							//����ʶ
	SCORE							lChangeScore;						//�������
};

//�����ʽ�
struct DBR_AdjustJackpotScore
{
	WORD							wStockID;							//����ʶ
	WORD							wLevelID;							//�ʽ�ȼ�
	SCORE							lChangeScore;						//�������
};

//////////////////////////////////////////////////////////////////////////
//�����Ϣ

//�����¼�
#define DBO_STOCK_LIST_ITEM			100									//�������
#define DBO_SUPER_PLAYER_CONTROL	101									//��R����
#define DBO_LOAD_CONFIG_FINISH		102									//�������
#define DBO_QUERY_STOCK_ITEM		103									//��ѯ���
#define DBO_UPDATE_STOCK_SCORE		104									//���¿��
#define DBO_UPDATE_STOCK_STATE		105									//����״̬
#define DBO_UPDATE_JACKPOT_SCORE	106									//���²ʽ�	

//�б���
#define DBO_GAME_KIND_ITEM			120									//������Ϣ

//��������
#define DBO_OPERATE_SUCCESS			500									//�����ɹ�
#define DBO_OPERATE_FAILURE			501									//����ʧ��

//////////////////////////////////////////////////////////////////////////////////

//��ѯ���
struct DBO_QueryStockItem
{
	WORD							wCommandID;							//�����ʶ
	tagStockItem					StockItem;							//�������
};

//��Ϸ����
struct DBO_GameKind
{
	bool							bNullity;							//���ñ�ʶ
	WORD							wModuleID;							//ģ���ʶ
	WORD							wTypeID;							//��������
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wKindID;							//��������
	WORD							wSupportMask;						//֧������
	TCHAR							szKindName[LEN_KIND];				//��Ϸ����
	TCHAR							szProcessName[LEN_PROCESS];			//��������
};

//�����ɹ�
struct DBO_OperateSuccess
{
	LONG							lResultCode;						//�������
	WORD							wRequestCmdID;						//��������
	TCHAR							szDescribeString[128];				//������Ϣ	
};

//����ʧ��
struct DBO_OperateFailure
{
	LONG							lResultCode;						//�������
	WORD							wRequestCmdID;						//��������
	TCHAR							szDescribeString[128];				//������Ϣ	
};

//////////////////////////////////////////////////////////////////////////////////

#endif