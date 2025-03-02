#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//�������ݰ�

//�ں��¼�
#define DBR_GR_LOAD_CONFIG			(DB_FRAME_REQUEST_START+1)			//��������
#define DBR_GR_LOAD_ROBOTS			(DB_FRAME_REQUEST_START+2)			//�����û�

//��½�¼�
#define	DBR_GR_LOGON_ROBOT			(DB_FRAME_REQUEST_START+100)		//������¼
#define	DBR_GR_LOGON_MOBILE			(DB_FRAME_REQUEST_START+101)		//�ֻ���¼
#define	DBR_GR_LOGON_COMPUTER		(DB_FRAME_REQUEST_START+102)		//���Ե�¼

//ϵͳ�¼�
#define DBR_GR_WRITE_GAME_SCORE		(DB_FRAME_REQUEST_START+200)		//��Ϸ����
#define DBR_GR_WRITE_USER_WEALTH	(DB_FRAME_REQUEST_START+201)		//��Ϸ�Ƹ�
#define DBR_GR_LEAVE_SERVICE		(DB_FRAME_REQUEST_START+202)		//�뿪����
#define DBR_GR_RECORD_DRAW_INFO		(DB_FRAME_REQUEST_START+203)		//���ּ�¼
#define DBR_GR_RECORD_DRAW_SCORE	(DB_FRAME_REQUEST_START+204)		//���ּ�¼
#define DBR_GR_RECORD_GAME_VIDEO	(DB_FRAME_REQUEST_START+205)		//���ּ�¼
#define DBR_GR_MANAGE_USER_RIGHT	(DB_FRAME_REQUEST_START+206)		//Ȩ�޹���
#define DBR_GR_RELOCK_SCORE			(DB_FRAME_REQUEST_START+207)		//��������
#define DBR_GR_LARGESS_SUBSIDY		(DB_FRAME_REQUEST_START+210)		//���͵ͱ�

//�����¼�
#define DBR_GR_USER_QUERY_GOODS		(DB_FRAME_REQUEST_START+270)		//��ѯ��Ʒ
#define DBR_GR_USER_UPDATE_GOODS	(DB_FRAME_REQUEST_START+271)		//������Ʒ
#define DBR_GR_USER_RECORD_GOODS	(DB_FRAME_REQUEST_START+272)		//��¼��Ʒ

//��Ӯ����
#define DBR_GR_CONTROL_START		(DB_FRAME_REQUEST_START+280)		//���ƿ�ʼ
#define DBR_GR_CONTROL_FINISH		(DB_FRAME_REQUEST_START+281)		//���ƽ���
#define DBR_GR_CONTROL_UPDATE		(DB_FRAME_REQUEST_START+282)		//���Ƹ���
#define DBR_GR_CONTROL_QUERY		(DB_FRAME_REQUEST_START+283)		//���Ʋ�ѯ

//��������
#define DBR_GR_SEND_GIFT			(DB_FRAME_REQUEST_START+290)		//��������
#define DBR_GR_UPDATE_LOVELINESS	(DB_FRAME_REQUEST_START+291)		//��������

//�뿪����ԭ��
#define LEAVE_GAME_NORMAL			0									//�����˳�
#define LEAVE_GAME_USER_CANCEL		1									//�û�ȡ����¼
#define LEAVE_GAME_THAN_MAX_SCORE	2									//���ֹ���
#define LEAVE_GAME_FULL				3									//��������
#define LEAVE_GAME_MANAGEMENT		4									//����ά��
#define LEAVE_GAME_ACTIVE_FAILURE	5									//�����û�ʧ��
#define LEAVE_GAME_BIND_ERROR		6									//�󶨴���
#define LEAVE_GAME_TWICE_LOGIN		7

//////////////////////////////////////////////////////////////////////////

//��������
struct DBR_GR_LoadConfig
{
	bool							bInitService;						//��ʼ��ʶ
	DWORD							dwConfigMask;						//��������
};

//���ػ���
struct DBR_GR_LoadRobotUser
{
	DWORD							dwBatchID;							//���α�ʶ
	DWORD							dwRobotCount;						//������Ŀ
};

//���Ե�¼
struct DBR_GR_LogonComputer
{		
	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//��½����	
	
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������

	//��������
	SCORE							lRequestScore;						//��������

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������¼
struct DBR_GR_LogonRobot
{
	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D	

	//Я����Ϣ
	BYTE							cbClientKind;						//�ն�����	

	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������

	//��������
	SCORE							lRequestScore;						//��������

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//ID ��¼
struct DBR_GR_LogonMobile
{		
	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D
	DWORD						    dwDeviceKind;						//�豸����
	
	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������

	//��������
	SCORE							lRequestScore;						//��������

	//λ����Ϣ
	double							dGlobalPosLng;						//��λ����
	double							dGlobalPosLat;						//��λγ��
	TCHAR							szPlaceName[LEN_PLACE_NAME];		//��ַ��Ϣ

	//Լս��λ
	bool							bLookon;							//�Թ۱�ʶ
	WORD							wTableID;							//���ӱ�ʶ
	WORD							wChairID;							//���ӱ�ʶ	
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwBattleNum;						//Լս����
	BYTE							cbScoreKind;						//��������
	DWORD							dwRankingNum;						//��λ����	
	TCHAR							szTablePasswd[LEN_PASSWORD];		//��������

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
};

struct DBR_GR_Update_Gold
{		
	//��¼��Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
};

//��Ϸ����
struct DBR_GR_WriteGameScore
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwPlayTimeCount;					//��Ϸʱ��

	//������Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lRevenue;							//��Ϸ˰��
	SCORE							lUnLockScore;						//�ͷŻ���

	//��Ϸ��Ϣ
	LONG							lWinCount;							//ʤ������
	LONG							lLostCount;							//ʧ������
	LONG							lDrawCount;							//�;�����
	LONG							lFleeCount;							//������Ŀ
	LONG							lExperience;						//�û�����
	LONG							lWinStreak;							//��ʤ����
	LONG							lMaxWinStreak;						//��ʤ����	
	SCORE							lBetAmount;							//��ע���

	//�ۼ���Ӯ
	WORD							wAddupWinRate;						//�ۼ�ʤ��
	SCORE							lAddupWinScore;						//�ۼ�Ӯ��
	SCORE							lAddupLoseScore;					//�ۼ����

	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������

	//������Ϣ
	DWORD							dwInoutIndex;						//��������
};

//д�븣��
struct DBR_GR_WriteUserWelfare
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ

	//������Ϣ
	SCORE							lPresentGold;						//���ͽ��	

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//���͵ͱ�
struct DBR_GR_LargessSubsidy
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//���¿��
struct DBR_GR_UpdateStock
{
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wStockID;							//����ʶ
	WORD							wStockKind;							//�������
	DOUBLE							lVariationScore;					//�仯���
};

//�������
struct DBR_GR_PurcaseProperty
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwPropKind;							//��������	
	LONG							lPropPrice;							//���߼۸�	
};

//��ѯ�ݷ�
struct DBR_GR_QueryGroupScore
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbChangeType;						//�������	
	SCORE							lChangeScore;						//�������

	//�����Ϣ
	DWORD							dwGroupID;							//��ݱ�ʶ
	TCHAR							szGroupName[LEN_GROUP_NAME];		//�������
	TCHAR							szOperaterNickName[LEN_ACCOUNTS];	//�û��ǳ�
};

//��ѯ����
struct DBR_GR_QueryGroupCowry
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbChangeType;						//�������	
	SCORE							lChangeCowry;						//�����ʯ

	//�����Ϣ
	DWORD							dwGroupID;							//��ݱ�ʶ
	TCHAR							szGroupName[LEN_GROUP_NAME];		//�������
	TCHAR							szOperaterNickName[LEN_ACCOUNTS];	//�û��ǳ�
};

//�۳���ʯ
struct DBR_GR_DeductGroupCowry
{
	DWORD							dwGroupID;							//��ݱ�ʶ
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lDeductCount;						//�۳�����
};

//���ȵ���
struct DBR_GR_WriteUserProperty 
{
	DWORD							dwUserID;							//�û���ʶ
	int								nPropertyID;						//����ID
	SCORE							lPropertyPrice;						//���߼۸�
	DWORD							dwPropCount;						//������Ŀ
	DWORD							dwClientIP;							//���ӵ�ַ

	//������Ϣ
	DWORD							dwRequestID;						//�����ʶ
};

//��������
struct DBR_GR_SendGift
{
    //�û���Ϣ
    DWORD							dwUserID;							//�û���ʶ
    DWORD							dwTargetUserID;						//�û���ʶ

    //��Ʒ��Ϣ
    DWORD							dwGoodsID;							//��Ʒ��ʶ
    DWORD							dwGoodsCount;						//��Ʒ����

	//������Ϣ
	SCORE							lMinTableScore;						//������ͷ���
	SCORE							lCellScore;							//���ӵ׷�
	BYTE							cbIsGoldRoom;						//�Ƿ��ҷ�

    //������Ϣ
    DWORD							dwClientIP;							//���ӵ�ַ	
};

//��������
struct DBR_GR_UpdateLoveLiness
{
    DWORD							dwUserID;							//�û���ʶ
};


//�뿪����
struct DBR_GR_LeaveGameService
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwPlayTimeCount;					//��Ϸʱ��
	DWORD							dwOnLineTimeCount;					//����ʱ��

	//������Ϣ
	SCORE							lScore;								//�û�����
	SCORE							lRevenue;							//��Ϸ˰��
	SCORE							lUnLockScore;						//�ͷŻ���

	//��Ϸ��Ϣ
	LONG							lWinCount;							//ʤ������
	LONG							lLostCount;							//ʧ������
	LONG							lDrawCount;							//�;�����
	LONG							lFleeCount;							//������Ŀ
	LONG							lExperience;						//�û�����
	LONG							lWinStreak;							//��ʤ����
	LONG							lMaxWinStreak;						//��ʤ����

	//������Ϣ
	DWORD							dwRequestID;						//�����ʶ
	BYTE							cbLeaveReason;						//�뿪ԭ��
	DWORD							dwInoutIndex;						//��¼����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	//�����Ϣ
	tagVariationScore				RecordInfo;							//��¼��Ϣ
};

//��Ϸ��¼
struct DBR_GR_RecordDrawInfo
{
	//��¼��ʶ
	SCORE							lRecordID;							//��¼��ʶ

	//��Ϸ��Ϣ
	WORD							wTableID;							//���Ӻ���	
	DWORD							dwPlayTimeCount;					//��Ϸʱ��	

	//������Ϣ
	BYTE							cbScoreKind;						//��������
	SCORE							lWasteCount;						//�����Ŀ
	DWORD							dwWasteUserID;						//������
	SCORE							lRevenueCount;						//˰����Ŀ		

	//������Ϣ	
	SCORE							lRelationRecordID;					//��¼��ʶ
	TCHAR							szVideoNumber[LEN_VIDEO_NUMBER];	//¼����

	//��¼ʱ��
	SYSTEMTIME						RecordTime;							//��¼ʱ��

	//��¼��Ϣ
	WORD							wUserCount;							//�û�����
	TCHAR							szGameSourse[1024];					//��Ϸ����
};

//д�ּ�¼
struct DBR_GR_RecordDrawScore
{
	//��¼��ʶ
	SCORE							lRecordID;							//��¼��ʶ

	//������Ϣ	
	SCORE							lRelationRecordID;					//��¼��ʶ
	TCHAR							szVideoNumber[LEN_VIDEO_NUMBER];	//¼����

	//��¼����
	WORD							wUserCount;							//�û�����	
	tagScoreRecordItem				ScoreRecordItem[MAX_CHAIR];			//��������
};

//��Ϸ¼��
struct DBR_GR_RecordGameVideo
{
	//Լս��Ϣ
	DWORD							dwRelationNum;						//�������
	SCORE							lRelationRecordID;					//��¼��ʶ

	//�û���Ϣ
	WORD							wUserCount;							//�û�����
	DWORD							dwUserID[8];						//��ʶ�б�

	//¼������
	SYSTEMTIME						VideoCreateTime;					//����ʱ��
	TCHAR							szVideoNumber[LEN_VIDEO_NUMBER];	//¼����	
};

//Ȩ�޹���
struct DBR_GR_ManageUserRight
{
	DWORD							dwUserID;							//Ŀ���û�
	DWORD							dwAddRight;							//���Ȩ��
	DWORD							dwRemoveRight;						//ɾ��Ȩ��
};

//��ѯ��Ʒ
struct DBR_GR_UserQueryGoods
{
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szGoodsIDList[256];					//��Ʒ�б�
};

//��Ϸ�Ƹ�
struct DBR_GR_WriteGameWealth
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	WORD							wReason;							//���ԭ��	
	WORD							wMoneyID;							//���ұ�ʶ
	SCORE							lVariationScore;					//�������	
};

//������Ʒ
struct DBR_GR_UserUpdateGoods
{
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szGoodsList[2000];					//��Ʒ�б�						
};

//��¼��Ʒ
struct DBR_GR_UserRecordGoods
{
	DWORD							dwUserID;							//�û���ʶ	
	TCHAR							szRecordList[2000];					//��¼�б�						
};

//���ƿ�ʼ
struct DBR_GR_ControlStart
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ

	//��Ӯ�ۼ�
	WORD							wAddupWinRate;						//�ۼ�ʤ��
	SCORE							lAddupWinScore;						//�ۼ�Ӯ��
	SCORE							lAddupLoseScore;					//�ۼ����
	
	//������Ϣ
	tagUserControlItem				UserControlItem;					//��������
};

//���ƽ���
struct DBR_GR_ControlFinish
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ

	//��Ӯ�ۼ�
	WORD							wAddupWinRate;						//�ۼ�ʤ��
	SCORE							lAddupWinScore;						//�ۼ�Ӯ��
	SCORE							lAddupLoseScore;					//�ۼ����

	//��Ϸ��Ϣ
	LONG							lWinCount;							//Ӯ����Ŀ
	LONG							lLoseCount;							//�����Ŀ
	SCORE							lWinScore;							//Ӯ����Ŀ
	SCORE							lLoseScore;							//�����Ŀ
	SCORE							lBetAmount;							//������Ŀ
};

//���Ƹ���
struct DBR_GR_ControlUpdate
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ

	//��Ϸ��Ϣ
	LONG							lWinCount;							//Ӯ����Ŀ
	LONG							lLoseCount;							//�����Ŀ
	SCORE							lWinScore;							//Ӯ����Ŀ
	SCORE							lLoseScore;							//�����Ŀ
	SCORE							lBetAmount;							//��������
};

//���Ʋ�ѯ
struct DBR_GR_ControlQuery
{
	DWORD							dwUserID;							//�û���ʶ
};


//////////////////////////////////////////////////////////////////////////
//�����Ϣ

//������Ϣ
#define DBO_GR_GAME_LEVEL			(DB_FRAME_RESPOND_START+500)		//��Ϸ�ȼ�
#define DBO_GR_USER_LEVEL			(DB_FRAME_RESPOND_START+501)		//�û��ȼ�
#define DBO_GR_GAME_GOODS			(DB_FRAME_RESPOND_START+502)		//��Ϸ��Ʒ
#define DBO_GR_VERSION_INFO			(DB_FRAME_RESPOND_START+503)		//�汾��Ϣ	
#define DBO_GR_FILTER_WORDS			(DB_FRAME_RESPOND_START+504)		//���˴ʻ�
#define DBO_GR_ROBOT_PARAMETER		(DB_FRAME_RESPOND_START+505)		//��������
#define DBO_GR_SUBSIDY_PARAMETER	(DB_FRAME_RESPOND_START+506)		//�ͱ�����
#define DBO_GR_CONTROL_PARAMETER	(DB_FRAME_RESPOND_START+507)		//���Ʋ���
#define DBO_GR_BATTLE_GAME_OPTION	(DB_FRAME_RESPOND_START+508)		//��Ϸѡ��

//�ں��¼�
#define DBO_GR_ROBOT_USER			(DB_FRAME_RESPOND_START+520)		//������Ϣ
#define DBO_GR_CONFIG_RESULT		(DB_FRAME_RESPOND_START+521)		//���ý��

//�߼��¼�
#define DBO_GR_LOGON_SUCCESS		(DB_FRAME_RESPOND_START+600)		//��¼�ɹ�
#define DBO_GR_LOGON_FAILURE		(DB_FRAME_RESPOND_START+601)		//��¼ʧ��
#define DBO_GR_WEALTH_UPDATE		(DB_FRAME_RESPOND_START+602)		//�Ƹ�����
#define DBO_GR_WRITE_GAME_SCORE		(DB_FRAME_RESPOND_START+603)		//д�ֽ��
#define DBO_GR_UPDATE_GOLD			(DB_FRAME_RESPOND_START+604)		//���½��
#define DBO_GR_WRITE_USER_PROP		(DB_FRAME_RESPOND_START+605)		//д�����
#define DBO_GR_LARGESS_SUBSIDY		(DB_FRAME_RESPOND_START+606)		//��ȡ�ͱ�
#define DBO_GR_USER_GOODS_UPDATE	(DB_FRAME_RESPOND_START+607)		//��Ʒ����

//��������
#define DBO_GR_SEND_GIFT_SUCCESS	(DB_FRAME_RESPOND_START+620)		//���ͳɹ�
#define DBO_GR_SEND_GIFT_FAILURE	(DB_FRAME_RESPOND_START+621)		//����ʧ��
#define DBO_GR_UPDATE_LOVELINESS	(DB_FRAME_RESPOND_START+622)		//��������

//��Ӯ����
#define DBO_GR_USER_CONTROL_INFO	(DB_FRAME_RESPOND_START+630)		//������Ϣ
#define DBO_GR_USER_CONTROL_UPDATE  (DB_FRAME_RESPOND_START+631)		//���Ƹ���

/////////////////////////////////////////////////////////////////////////////////////////

//���ý��
struct DBO_GR_ConfigResult
{
	bool							bResultCode;						//�����ʶ
	bool							bInitService;						//��ʼ��ʶ		
	DWORD							dwConfigMask;						//��������
};

//������Ϣ
struct DBO_GR_GameRobotInfo
{
	LONG							lResultCode;						//�������
	DWORD							dwBatchID;							//���α�ʶ
	WORD							wRobotCount;						//�û���Ŀ
	tagRobotAccountsInfo			RobotAccountsInfo[MAX_ROBOT];		//�����ʺ�
};

//������Ϣ
struct DBO_GR_RobotParameter
{
	LONG							lResultCode;						//�������
	WORD							wParameterCount;					//������Ŀ
	tagRobotParameter				RobotParameter[MAX_BATCH];			//������Ϣ
};

//ϵͳ��Ϣ
struct DBO_GR_MessageInfo
{
	DWORD							dwMessageID;						//��ϢID
	BYTE							cbStatindIDCount;					//վ������
	DWORD							dwOptionFlags;						//��Ϣѡ��
	SYSTEMTIME						stStartTime;						//��ʼʱ��
	SYSTEMTIME						stEndTime;							//����ʱ��
	DWORD							dwTimeRate;							//����Ƶ��
	TCHAR							szMessageString[1024];				//ϵͳ��Ϣ
};

//��¼�ɹ�
struct DBO_GR_LogonSuccess
{
	//��������
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	DWORD							dwCustomID;							//�Զ�ͷ��
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	
	//�û�����
	BYTE							cbAge;								//�û�����
	BYTE							cbGender;							//�û��Ա�	
	TCHAR							szArea[LEN_AREA];					//������Ϣ
	TCHAR							szCity[LEN_CITY];					//������Ϣ
	TCHAR							szProvince[LEN_PROVINCE];			//ʡ����Ϣ
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��
	TCHAR							szUserHeadUrl[256];		//kk jia
	

	//�Ƹ���Ϣ
	SCORE							lGold;								//�û����
	SCORE							lScore;								//��Ϸ����
	SCORE							lUserCard;							//�û�����
	SCORE							lFreeScore;							//���ɻ���	
	SCORE							lInsureGold;						//���н��			
	BYTE							cbScoreKind;						//��������

	//��Ϸ��Ϣ
	LONG							lWinCount;							//ʤ������
	LONG							lLostCount;							//ʧ������
	LONG							lDrawCount;							//�;�����
	LONG							lFleeCount;							//������Ŀ
	LONG							lExperience;						//�û�����
	LONG							lWinStreak;							//��ʤ����
	LONG							lMaxWinStreak;						//��ʤ����

	//�ȼ���Ϣ
	BYTE							cbMasterOrder;						//����ȼ�
	DWORD							dwMemberPoint;						//��Ա����

	//�û�Ȩ��
	DWORD							dwUserRight;						//�û�Ȩ��
	DWORD							dwMasterRight;						//����Ȩ��

	//�ͱ���Ϣ
	BYTE							cbResidualTimes;					//ʣ�����
	SCORE							lConsumeScore;						//���Ľ��(�ʼ������ת�˵�)

	//�ֻ�����
	BYTE							dwDeviceKind;						//�豸����
	WORD							wBehaviorFlags;						//��Ϊ��ʶ
	WORD							wPageTableCount;					//��ҳ����

	//λ����Ϣ	
	double							dGlobalPosLng;						//��λ����
	double							dGlobalPosLat;						//��λγ��
	TCHAR							szPlaceName[LEN_PLACE_NAME];		//��ַ��Ϣ

	//������Ϣ
	DWORD							dwMatchID;							//������ʶ
	SCORE							lMatchNo;							//��������
	WORD							wSignupCount;						//��������
	DWORD							dwTicketCount;						//��ȯ����

	//״̬��Ϣ
	BYTE							cbShutUpStatus;						//����״̬
	DWORD							dwShutUpOverTime;					//���ʱ��

	//�ڰ�����
	WORD							wAddupWinRate;						//�ۼ�ʤ��	
	SCORE							lAddupWinScore;						//�ۼ�Ӯ��
	SCORE							lAddupLoseScore;					//�ۼ����	
	tagUserControlItem				UserControlItem;					//��������

	//������Ϣ
	bool							bLookon;							//�Թ۱�־
	WORD							wTableID;							//���ӱ�ʶ
	WORD							wChairID;							//���ӱ�ʶ
	TCHAR							szTablePasswd[LEN_PASSWORD];		//��������

	//��Ʒ��Ϣ
	WORD							wGoodsCount;						//��Ʒ����
	tagGoodsHoldInfo				GoodsHoldInfo[32];					//������Ϣ

	//��������
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	DWORD							dwInoutIndex;						//��¼����
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��¼ʧ��
struct DBO_GR_LogonFailure
{
	//������Ϣ
	WORD							wLockKindID;						//���ͱ�ʶ
	WORD							wLockServerID;						//�����ʶ
	WORD							wLockServerKind;					//��������

	//������Ϣ
	LONG							lErrorCode;							//�������	
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��������ID
struct DBO_GR_ResetRequestID
{
	LONG							lErrorCode;							//�������
};

//��������
struct DBO_GR_RelockScore
{
	SCORE							lGold;								//�û����
	SCORE							lInsureGold;						//���н��
	SCORE							lFreeScore;							//���ɻ��� 
};

//�������
struct DBO_GR_PurcaseProperty
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lCurrIngot;							//��ʯ���

	//������Ϣ
	DWORD							dwPropKind;							//��������		
};

//��Ϸ���� �走ר��
struct DBO_GR_GameRoundFinish
{
	WORD							wGameRound;							//��ɾ���
	WORD							wRoundCount;						//�ܹ�����
};

//�ͱ�����
struct DBO_GR_SubsidyParameter
{
	//�ͱ�����
	SCORE							lTakeSubsidyGold;					//���ͽ��
	SCORE							lTakeSubsidyCondition;				//��ȡ����			
	};

//���Ʋ���
struct DBO_GR_ControlParameter
{

	//���Ʋ���
	bool							bControlEnabled;					//���ƿ���
	LONG							lBenchmarkValue;					//��׼��ֵ
	WORD							wWLControlRate;						//����ʤ��
	WORD							wBLControlRate;						//����ʤ��
	WORD							wInWhiteListRate;					//������ֵ
	WORD							wInBlackListRate;					//������ֵ
	WORD							wOutWhiteListRate;					//�˳���ֵ
	WORD							wOutBlackListRate;					//�˳���ֵ
	WORD							wStartPlayCount;					//�������
	WORD							wSupportControlMode;				//����ģʽ
};

//�������
struct DBO_GR_WriteWelfareResult
{
	bool							bResultCode;						//�������
	BYTE							cbResidualTimes;					//ʣ�����
	BYTE							cbRechargeStatus;					//�׳�״̬
	SCORE							lCurrGold;							//��ǰ���
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�ͱ����
struct DBO_GR_LargessSubsidy
{
	bool							bResultCode;						//�������
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbResidualTimes;					//ʣ�����
	SCORE							lCurrGold;							//��ǰ���
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�Ƹ�����
struct DBO_GR_UserWealthUpdate
{
	WORD							wMoneyID;							//���ұ�ʶ
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lChangeAmount;						//������
	SCORE							lBalanceAmount;						//������
};

//��Ʒ��Ϣ
struct DBO_GR_UserGoodsUpdate
{
	WORD							wGoodsCount;						//��Ʒ����	
	tagGoodsHoldInfo				GoodsHoldInfo[128];					//��Ʒ��Ϣ
};

//���ͳɹ�
struct DBO_GR_SendGiftSuccess
{
	DWORD							dwUserID;							//�����û�
    DWORD							dwTargetUserID;						//Ŀ���û�

    //��Ʒ��Ϣ
    DWORD							dwGoodsID;							//��Ʒ��ʶ
    DWORD							dwGoodsCount;						//��Ʒ����

    DWORD							dwLoveLiness;						//��������
    DWORD							dwMyLoveLiness;						//��������
    DWORD							dwTargetLoveLiness;					//�Է�����

    WORD							wMoneyID;							//��������
	SCORE							lGiftPrice;							//���ü۸�
};

//����ʧ��
struct DBO_GR_SendGiftFailure
{
    LONG							lErrorCode;							//�������
    TCHAR							szDescribeString[128];				//������Ϣ
};

//��������
struct DBO_GR_UpdateLoveLiness
{
    DWORD							dwLoveLiness;						//����ֵ
};

//������Ϣ
struct DBO_GR_UserControlInfo
{
	DWORD							dwUserID;							//�û���ʶ
	tagUserControlItem				UserControlItem;					//��������
};

//���Ƹ���
struct DBO_GR_UserControlUpdate
{
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbControlStatus;					//����״̬
};

//////////////////////////////////////////////////////////////////////////

#endif