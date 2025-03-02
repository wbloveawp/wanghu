#ifndef EXTEND_DATA_BASE_PACKET_HEAD_FILE
#define EXTEND_DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//�������ݰ�

//Լս�¼�
#define DBR_GR_BATTLE_LOCK			(DB_EXTEND_REQUEST_START+1)			//Լս����
#define DBR_GR_BATTLE_UNLOCK		(DB_EXTEND_REQUEST_START+2)			//Լս����
#define DBR_GR_BATTLE_RECORD		(DB_EXTEND_REQUEST_START+3)			//Լս��¼
#define DBR_GR_BATTLE_REWARD		(DB_EXTEND_REQUEST_START+4)			//Լս����
#define DBR_GR_DEDUCT_BATTLE_COST	(DB_EXTEND_REQUEST_START+6)			//�۳�����
#define DBR_GR_DEDUCT_BATTLE_COSTAA	(DB_EXTEND_REQUEST_START+7)			//�۳�����
#define DBR_GR_RETURN_BATTLE_COST	(DB_EXTEND_REQUEST_START+8)			//�˻�����
#define DBR_GR_RETURN_BATTLE_COSTAA	(DB_EXTEND_REQUEST_START+9)			//�˻�����

//////////////////////////////////////////////////////////////////////////

//�۷��û�
struct tagBattleUser
{
	DWORD							dwUserID;							//�û���ʶ	
};

//�����û�
struct DBR_GR_BattleLock
{
	//Լս��Ϣ
	BYTE							cbScoreKind;						//��������
	DWORD							dwBattleNum;						//Լս����

	//�û���Ϣ
	WORD							wUserCount;							//�û�����
	DWORD							dwUserID[TABLE_USER_COUNT];			//�û���ʶ
};

//�����û�
struct DBR_GR_BattleUnlock
{
	//�û���Ϣ
	DWORD							dwBattleNum;						//Լս����
	WORD							wUserCount;							//�û�����
	DWORD							dwUserID[TABLE_USER_COUNT];			//�û���ʶ
};

//�۳�����
struct DBR_GR_DeductBattleCost
{
	//�û���Ϣ	
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//Ⱥ���ʶ
	
	//������Ϣ
	WORD							wTableID;							//���ӱ�ʶ
	WORD							wPayType;							//֧����ʽ
	SCORE							lConsumeCount;						//��������
	BYTE							cbCurrencyKind;						//��������	
};

//�۳�����
struct DBR_GR_DeductBattleCostAA
{
	//������Ϣ
	WORD							wTableID;							//���ӱ�ʶ
	DWORD							dwClubID;							//Ⱥ���ʶ	
	SCORE							lConsumeCount;						//���ķ���	
	BYTE							cbCurrencyKind;						//��������	

	//�û���Ϣ
	WORD							wUserCount;							//�û�����
	tagBattleUser					BattleUser[TABLE_USER_COUNT];		//�û���Ϣ
};

//�˻�����
struct DBR_GR_ReturnBattleCost
{
	//�û���Ϣ	
	DWORD							dwUserID;							//�û���ʶ	
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwBattleNum;						//���ӱ��
	LONGLONG						lReturnCount;						//��������
	BYTE							cbCurrencyKind;						//��������		
};

//��ɢ����
struct DBR_GR_DismissTable
{
	//������Ϣ
	WORD							wTableID;							//���ӱ�ʶ
	DWORD							dwBattleNum;						//���ӱ��
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����	

	//������Ϣ
	tagSocketInfo					SocketInfo;							//������Ϣ
};

//Լս����
struct DBR_GR_BattleReward
{
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lRewardGold;						//��ҽ���	
};

//��¼ս��
struct DBR_GR_BattleRecord
{
	//������Ϣ
	DWORD							dwOwnerID;							//������Ϣ
	DWORD							dwBattleNum;						//ӳ����	
	bool							bBattleFinish;						//Լս���
	SCORE							lBattleRecordID;					//��¼��ʶ

	//Ⱥ����Ϣ
	DWORD							dwClubID;							//Ⱥ���ʶ
	DWORD							dwClubCreaterID;					//Ⱥ����ʶ
	
	//��������
	DWORD							dwConfigID;							//���ñ�ʶ
	WORD							wFinishCount;						//��ɾ���
	WORD							wPlayCount;							//��Ϸ����
	BYTE							cbScoreKind;						//��������
	BYTE							cbSettleKind;						//���㷽ʽ	
	DWORD							dwCreateTime;						//����ʱ��
	BYTE							cbAllowPublic;						//������
	SCORE							lBaseScore;							//�����׷�	
	SCORE							lWinnerScore;						//Ӯ�ҵ÷�
	LONGLONG						lConsumeCount;						//��ʯ����
	BYTE							cbCurrencyKind;						//��������

	//֧����Ϣ
	WORD							wPayType;							//֧����ʽ
	DWORD							dwPayUserID;						//֧���û�

	//��Ϸ����
	TCHAR							szGameOption[128];					//��Ϸ����

	//������Ϣ
	WORD							wUserCount;							//��������
	tagBattleUserItem				BattleUserItem[TABLE_USER_COUNT];	//�û�����
};

//////////////////////////////////////////////////////////////////////////
//�����Ϣ

//�߼��¼�
#define DBO_GR_DEDUCT_BATTLE_COST	(DB_EXTEND_RESPOND_START+1)			//�۷ѽ��
#define DBO_GR_RETURN_BATTLE_COST	(DB_EXTEND_RESPOND_START+2)			//�˷ѽ��
/////////////////////////////////////////////////////////////////////////////////////////

//�۷ѽ��
struct DBO_GR_DeductBattleCost
{
	WORD							wTableID;							//���ӱ�ʶ	
	DWORD							dwFailUserID;						//�û���ʶ
	
	//�Ƹ���Ϣ
	SCORE							lUserGold;							//�������	
	SCORE							lUserCard;							//��������	
	DWORD							dwPayUserID;						//�������

	//�۷ѽ��
	LONG							lResultCode;						//�������		
	WCHAR							szErrorMessage[128];				//������Ϣ
};

//�˷ѽ��
struct DBO_GR_ReturnBattleCost
{
	//������Ϣ	
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClubID;							//Ⱥ���ʶ	

	//�Ƹ���Ϣ 
	SCORE							lUserGold;							//�������
	SCORE							lUserCard;							//��������	

	//������Ϣ
	LONG							lResultCode;						//�������
	WCHAR							szErrorMessage[128];				//������Ϣ
};

//////////////////////////////////////////////////////////////////////////

#endif