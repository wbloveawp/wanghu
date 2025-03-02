#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//�������ݰ�

//��¼����
#define	DBR_GP_LOGON_GAMEID			1									//I D ��¼
#define	DBR_GP_LOGON_ACCOUNTS		2									//�ʺŵ�¼
#define DBR_GP_REGISTER_ACCOUNTS	3									//ע���ʺ�

//�˺ŷ���
#define DBR_GP_MODIFY_ACCOUNTS		10									//�޸��ʺ�
#define DBR_GP_MODIFY_LOGON_PASS	11									//�޸�����
#define DBR_GP_MODIFY_INSURE_PASS	12									//�޸�����
#define DBR_GP_MODIFY_INDIVIDUAL	13									//�޸�����
#define DBR_GP_BIND_MACHINE			15									//��������
#define DBR_GP_UN_BIND_MACHINE		16									//��������

//��ѯ����
#define DBR_GP_QUERY_INDIVIDUAL		32									//��ѯ����

//ϵͳ����
#define DBR_GP_LOAD_CONFIG			50									//��������
#define DBR_GP_LOAD_RANKING_LIST	51									//���ذ�
#define DBR_GP_RESET_USER_DATA		52									//��������
#define DBR_GP_ONLINE_COUNT_INFO	53									//������Ϣ

//��Ϸ����
#define DBR_GP_QUERY_GAME_CONFIG	60									//��ѯ����
#define DBR_GP_UPDATE_BASIC_CONFIG	61									//��������
#define DBR_GP_UPDATE_WEIGHT_CONFIG	62									//��������

//��������
#define DBR_GP_QUERY_PARAMETER		70									//��ȡ����
#define DBR_GP_APPEND_PARAMETER		71									//��Ӳ���
#define DBR_GP_MODIFY_PARAMETER		72									//�޸Ĳ���
#define DBR_GP_DELETE_PARAMETER		73									//ɾ������

//�������
#define DBR_GP_QUERY_STOCK_INFO		80									//��ѯ���
#define DBR_GP_APPEND_STOCK_INFO	81									//��ӿ��
#define DBR_GP_MODIFY_STOCK_INFO	82									//�޸Ŀ��
#define DBR_GP_DELETE_STOCK_INFO	83									//ɾ�����
#define DBR_GP_ADJUCT_STOCK_SCORE	84									//�������
#define DBR_GP_ADJUCT_JACKPOT_SCORE	85									//�����ʽ�

//////////////////////////////////////////////////////////////////////////

//ID ��¼
struct DBR_GP_LogonByGameID
{
	//��¼��Ϣ
	DWORD							dwGameID;							//�û� I D
	TCHAR							szPassword[LEN_MD5];				//��¼����

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	//������Ϣ
	DWORD							dwPostTime;
	LPVOID							pTokenParameter;					//�󶨲���

	//��չ��Ϣ
	DWORD							dwCheckUserRight;					//���Ȩ��
};

//�ʺŵ�¼
struct DBR_GP_LogonByAccounts
{
	//��¼��Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwLogonServerID;					//��¼ I D
	DWORD							dwPlazaVersion;						//�����汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szNewComputerID[LEN_MACHINE_ID];	//�������� 

	//������Ϣ
	WORD							wGateID;							//���ر�ʶ
	WORD							wLogonID;							//��¼��ʶ
	DWORD							dwPostTime;							//Ͷ��ʱ��
	LPVOID							pTokenParameter;					//�󶨲���	

	//��չ��Ϣ
	DWORD							dwCheckUserRight;					//���Ȩ��
};

//�ʺ�ע��
struct DBR_GP_RegisterAccounts
{
	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ʺ�
	TCHAR							szBankPass[LEN_MD5];				//��������

	//�û���Ϣ
	BYTE							cbAge;								//�û�����
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szArea[LEN_AREA];					//������Ϣ
	TCHAR							szCity[LEN_CITY];					//������Ϣ
	TCHAR							szProvince[LEN_PROVINCE];			//ʡ����Ϣ

	//������Ϣ
	WORD							wClientMask;						//�ն�����
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwLogonServerID;					//��¼ I D
	TCHAR							szMachineID[LEN_MACHINE_ID];	//��������
	TCHAR							szValidComputerID[LEN_MACHINE_ID];//��������
	DWORD							dwPlazaVersion;						//�����汾

	//�����Ϣ
	BYTE							cbPassportType;						//�������
	TCHAR							szPassportID[LEN_PASSPORT_ID];		//�����Ϣ

	//�Ƽ���Ϣ
	TCHAR							szSpreader[LEN_ACCOUNTS];			//�Ƽ����

	//������Ϣ
	DWORD							dwPostTime;
	LPVOID							pTokenParameter;					//�󶨲���
};

//�ʺż���
struct DBR_GP_ActivateAccounts
{
	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	TCHAR							szOrgPassword[LEN_MD5];				//ԭʼ����
	TCHAR							szOrgAccounts[LEN_ACCOUNTS];		//ԭʼ�ʺ�
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ʺ�
	TCHAR							szBankPass[LEN_MD5];				//��������

	//�û���Ϣ
	BYTE							cbAge;								//�û�����
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szArea[LEN_AREA];					//������Ϣ
	TCHAR							szCity[LEN_CITY];					//������Ϣ
	TCHAR							szProvince[LEN_PROVINCE];			//ʡ����Ϣ

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwLogonServerID;					//��¼ I D
	DWORD							dwPlazaVersion;						//�����汾
	TCHAR							szMachineID[LEN_MACHINE_ID];	//��������

	//�����Ϣ
	BYTE							cbPassportType;						//�������
	TCHAR							szPassportID[LEN_PASSPORT_ID];		//�����Ϣ

	//�Ƽ���Ϣ
	TCHAR							szSpreader[LEN_ACCOUNTS];			//�Ƽ����

	//������Ϣ
	DWORD							dwPostTime;
	LPVOID							pTokenParameter;					//�󶨲���
};

//�޸��ʺ�
struct DBR_GP_ModifyAccounts
{
	//��֤����
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//�ʺ�����
	WORD							wFaceID;							//ͷ���ʶ
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�û��ʺ�
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��

	//�����Ա�
	BYTE							cbAge;								//�û�����
	BYTE							cbGender;							//�û��Ա�

	//��������
	TCHAR							szArea[LEN_AREA];					//������Ϣ
	TCHAR							szCity[LEN_CITY];					//������Ϣ
	TCHAR							szProvince[LEN_PROVINCE];			//ʡ����Ϣ
	bool							bOldVersion;						//�Ƿ�ɰ�
};

//�޸�����
struct DBR_GP_ModifyLogonPass
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szDesPassword[LEN_PASSWORD];		//�û�����
	TCHAR							szScrPassword[LEN_PASSWORD];		//�û�����
};

//�޸�����
struct DBR_GP_ModifyInsurePass
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szDesPassword[LEN_PASSWORD];		//�û�����
	TCHAR							szScrPassword[LEN_PASSWORD];		//�û�����
};

//�޸�����
struct DBR_GP_ModifyIndividual
{
	//��֤����
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//�û���Ϣ
	TCHAR							szUserNote[LEN_USER_NOTE];			//�û�˵��
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����

	//�绰����
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//�̶��绰
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ƶ��绰

	//��ϵ����
	TCHAR							szQQ[LEN_QQ];						//Q Q ����
	TCHAR							szEMail[LEN_EMAIL];					//�����ʼ�
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//��ϵ��ַ
};

//��ѯ����
struct DBR_GP_QueryIndividual
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientIP;							//���ӵ�ַ
};

//��ѯ��¼
struct DBR_GP_QueryBattleRecord
{
	//��֤����
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//�汾��Ϣ
	BYTE							cbFuncVersion;						//���ܰ汾
};

//��������
struct DBR_GP_BindMachine
{
	//��֤����
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��������
struct DBR_GP_UnbindMachine
{
	//��֤����
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�����ܱ�
struct DBR_GP_AccountSecurity
{
	//��֤����
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//��������
	TCHAR							szQuestion[LEN_PROTECT_QUESTION];	//����һ
	TCHAR							szResponse[LEN_PROTECT_QUESTION];	//��һ
	TCHAR							szQuestion2[LEN_PROTECT_QUESTION];	//�����
	TCHAR							szResponse2[LEN_PROTECT_QUESTION];	//�𰸶�
	TCHAR							szQuestion3[LEN_PROTECT_QUESTION];	//������
	TCHAR							szResponse3[LEN_PROTECT_QUESTION];	//����
	BYTE							cbPassportType;						//֤������
	TCHAR							szPassportID[LEN_PASSPORT_ID];		//֤������
	TCHAR							szSafeEmail[LEN_EMAIL];				//��ȫ����

	DWORD							dwClientIP;							//���ӵ�ַ
};

//��������
struct DBR_GP_ResetUserData
{
	WORD							wDayOfWeek;							//���ڱ�ʶ
	TCHAR							szUserIDList[2048];					//�û��б�	
};

//����ʺ�
struct DBR_GP_TestAccounts
{
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�ʺ�
	TCHAR							szMechine[LEN_MACHINE_ID];			//������
	BYTE							cbCheckType;						//�������
	DWORD							dwClientIP;							//���ӵ�ַ
};

//������Ϣ
struct DBR_GP_OnLineCountInfo
{
	WORD							wKindCount;							//������Ŀ
	DWORD							dwOnLineCountSum;					//��������
	tagOnLineInfoKind				OnLineCountKind[MAX_KIND];			//��������
};

//���д�ȡ
struct DBR_GP_BankAccess
{
	DWORD							dwUserID;							//�û�ID
	TCHAR							szBankPass[LEN_PASSWORD];			//��������
	LONGLONG						lAmount;							//��ȡ����
	DWORD							dwClientIP;							//���ӵ�ַ
};

//����ת��
struct DBR_GP_BankTransfer
{
	DWORD							dwUserID;							//�û�ID
	DWORD							dwRecvGameID;						//�����û�
	TCHAR							szBankPass[LEN_PASSWORD];			//��������
	LONGLONG						lAmount;							//ת������
	DWORD							dwClientIP;							//���ӵ�ַ
};

//��ѯ�û�
struct DBR_GP_BankQuery
{
	TCHAR							szUser[LEN_ACCOUNTS];				//�û��ʺ�
	bool							bAccount;							//�Ƿ��ʺ�
};

struct DBR_GP_BankUpdate
{
	DWORD							dwUserID;							//�û�ID
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
};

//��ѯ�ͱ�
struct DBR_GP_QuerySubsidy
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û�ID	

	//������Ϣ	
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ȡ�ͱ�
struct DBR_GP_LargessSubsidy
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û�ID
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ѯ����
struct DBR_GP_QueryGameConfig
{
	WORD							wKindID;							//���ͱ�ʶ��ʶ
};

//��������
struct DBR_GP_UpdateBasicConfig
{
	WORD							wKindID;							//���ͱ�ʶ
	TCHAR							szBasicConfig[4000];				//��������
};

//��������
struct DBR_GP_UpdateWeightConfig
{
	WORD							wKindID;							//���ͱ�ʶ
	BYTE							cbTypeID;							//Ȩ������
	WORD							wItemCount;							//������Ŀ
	tagTimesWeightItem				TimesWeightItem[128];				//Ȩ������
};

//��ѯ����
struct DBR_GP_QueryParameter
{
	WORD							wServerID;							//�����ʶ
};

//��Ӳ���
struct DBR_GP_AppendParameter
{
	WORD							wServerID;							//�����ʶ
	tagRobotParameter				RobotParameter;						//��������
};

//�޸Ĳ���
struct DBR_GP_ModifyParameter
{
	WORD							wServerID;							//�����ʶ
	tagRobotParameter				RobotParameter;						//��������
};

//ɾ������
struct DBR_GP_DeleteParameter
{
	WORD							wServerID;							//�����ʶ
	DWORD							dwBatchID;							//���α�ʶ
};

//��ѯ���
struct DBR_GP_QueryStockInfo
{
	TCHAR							szKindID[128];						//�����б�
};

//��Ӳ���
struct DBR_GP_AppendStockInfo
{
	tagStockInfo					StockInfo;							//�������
};

//�޸Ĳ���
struct DBR_GP_ModifyStockInfo
{
	tagStockInfo					StockInfo;							//�������
};

//ɾ�����
struct DBR_GP_DeleteStockInfo
{
	WORD							wStockID;							//����ʶ	
};

//�������
struct DBR_GP_AdjustStockScore
{
	WORD							wStockID;							//����ʶ
	SCORE							lChangeScore;						//�������
};

//�����ʽ�
struct DBR_GP_AdjustJackpotScore
{
	WORD							wStockID;							//����ʶ
	WORD							wLevelID;							//�ʳصȼ�
	SCORE							lChangeScore;						//�������
};

//��¼�û�
struct DBR_GP_RecordUpdateUser
{
	DWORD							dwUserID;							//�û���ʶ
};

//��������
struct DBR_GP_LoadConfig
{
	bool							bInitService;						//��ʼ��ʶ
	DWORD							dwConfigMask;						//��������
};

//////////////////////////////////////////////////////////////////////////
//PC�������

//��¼���
#define DBO_GP_LOGON_SUCCESS		100									//��¼�ɹ�
#define DBO_GP_LOGON_FAILURE		101									//��¼ʧ��

//�û���Ϣ
#define DBO_GP_USER_INDIVIDUAL		110									//�û�����

//������
#define DBO_GP_OPERATE_SUCCESS		120									//�����ɹ�
#define DBO_GP_OPERATE_FAILURE		121									//����ʧ��
#define DBO_GP_BIND_MANCHINE_RESULT 122									//�󶨽��

//�б���
#define DBO_GP_GAME_TYPE_ITEM		130									//������Ϣ
#define DBO_GP_GAME_KIND_ITEM		131									//������Ϣ
#define DBO_GP_GAME_NODE_ITEM		132									//�ڵ���Ϣ
#define DBO_GP_GAME_PAGE_ITEM		133									//������Ϣ
#define DBO_GP_GAME_FILTER_ITEM		134									//������Ϣ

//��Ϸ����
#define DBO_GP_GAME_BASIC_CONFIG	140									//��������
#define DBO_GP_GAME_WEIGHT_CONFIG	141									//Ȩ������
#define DBO_GP_GAME_CONFIG_FINISH	142									//�������

//�������
#define DBO_GP_CONFIG_RESULT		150									//���ý��

//�������
#define DBO_GP_VERSION_INFO			160									//�汾��Ϣ	
#define DBO_GP_FILTER_WORDS			161									//���˴ʻ�
#define DBO_GP_GLOBAL_OPTION		162									//ȫ������
#define DBO_GP_RANKING_LIST_DATA	163									//������									
#define DBO_GP_TRANSFER_PARAMETER	164									//ת�˲���
#define DBO_GP_STOCK_OPERATE_RESULT 165									//�������
#define DBO_GP_ROBOT_OPERATE_RESULT 166									//�������

//////////////////////////////////////////////////////////////////////////

//��¼�ɹ�
struct DBO_GP_LogonSuccess
{
	//��������
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwGameID;							//��Ϸ��ʶ
	DWORD							dwGroupID;							//���ű�ʶ
	DWORD							dwCustomID;							//�Զ�����
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	TCHAR							szGroupName[LEN_GROUP_NAME];		//��������
	BYTE							cbMasterOrder;						//����Ա�ȼ�
	DWORD							dwProtectID;						//�ܱ���ʶ
	BYTE							cbMoorMachine;						//�󶨻�����ʶ	

	//�û�����
	BYTE							cbAge;								//�û�����
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szArea[LEN_AREA];					//������Ϣ
	TCHAR							szCity[LEN_CITY];					//������Ϣ
	TCHAR							szProvince[LEN_PROVINCE];			//ʡ����Ϣ
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��

	//����ȼ�
	DWORD							dwExperience;						//������ֵ

	//��Ա����
	DWORD							dwMemberPoint;						//��Ա����
	DWORD							dwMemberOverTime;					//����ʱ��

	//�Ƹ���Ϣ
	LONGLONG						lUserGold;							//�û��ƽ�
	LONGLONG						lInsureGold;						//���н��

	//������Ϣ
	WORD							wLockGateID;						//���ر�ʶ
	WORD							wLockLogonID;						//��¼��ʶ
	DWORD							dwLockSocketID;						//�����ʶ
	
	//��չ��Ϣ 
	DWORD							dwCheckUserRight;					//���Ȩ��

	//������Ϣ
	DWORD							dwPostTime;
	TCHAR							szDescribeString[128];				//������Ϣ
};


//��¼ʧ��
struct DBO_GP_LogonFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��������
struct DBO_GP_UserIndividual
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û� I D
	TCHAR							szUserNote[LEN_USER_NOTE];			//�û�˵��
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����

	//�绰����
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//�̶��绰
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ƶ��绰

	//��ϵ����
	TCHAR							szQQ[LEN_QQ];						//Q Q ����
	TCHAR							szEMail[LEN_EMAIL];					//�����ʼ�
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//��ϵ��ַ
};

//��������
struct DBO_GP_GameBasicConfig
{
	TCHAR							szBasicConfig[4000];				//��������
};

//Ȩ������
struct DBO_GP_GameWeightConfig
{
	WORD							wItemCount;							//������Ŀ
	tagTimesWeightItem				TimesWeightItem[128];				//Ȩ������
};

//����ʧ��
struct DBO_GP_OperateFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�����ɹ�
struct DBO_GP_OperateSuccess
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//�ɹ���Ϣ
};

//�󶨻���
struct DBO_GP_BindMachineResult
{
	LONG							lResultCode;						//��������
	WCHAR							szDescribeString[128];				//�ɹ���Ϣ
};

//�����
struct DBO_GP_StockOperateResult
{
	WORD							wPacketIdx;							//���ݰ�ID
	WORD							wSubCommdID;						//��������
	WORD							wStockCount;						//������Ŀ
	tagStockInfo					StockList[24];						//����б�
};

//��������
struct DBO_GP_RobotOperateResult
{
	WORD							wSubCommdID;						//��������
	WORD							wServerID;							//�����ʶ
	WORD							wParameterCount;					//������Ŀ
	tagRobotParameter				RobotParameter[MAX_BATCH];			//��������
};

//�������
struct DBO_GP_UpdateConfigResult
{
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwConfigMask;						//��������
	LONG							lResultCode;						//��������
	WCHAR							szDescribeString[128];				//�ɹ���Ϣ
};

//���ý��
struct DBO_GP_ConfigResult
{
	bool							bResultCode;						//�����ʶ
	bool							bInitService;						//��ʼ��ʶ		
	DWORD							dwConfigMask;						//��������
};

//������
struct DBO_GP_RankingListData
{
	BYTE							cbListType;							//������
	CHAR							szRankingList[8192];				//������
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ����
struct DBO_GP_GameType
{
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wTypeID;							//��������
	TCHAR							szTypeName[LEN_TYPE];				//��������
};

//��Ϸ����
struct DBO_GP_GameKind
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

//��Ϸ�ڵ�
struct DBO_GP_GameNode
{
	WORD							wKindID;							//��������
	WORD							wJoinID;							//�ҽ�����
	WORD							wSortID;							//��������
	WORD							wNodeID;							//�ڵ�����
	TCHAR							szNodeName[LEN_NODE];				//�ڵ�����
};

//��������
struct DBO_GP_GameCustom
{
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wSortID;							//��������
	WORD							wPageID;							//��������
	WORD							wOperateType;						//��������
	TCHAR							szDisplayName[LEN_CUSTOM];			//��������
	TCHAR							szReponseUrl[256];					//��ת��ַ
};

//�������
struct DBO_GP_GameFilter
{
	WORD							wSortID;							//��������
	WORD							wKindID;							//��������
	WORD							wNodeID;							//�ڵ�����
	WORD							wServerID;							//��������
	WORD							wFilterMask;						//����״̬
	TCHAR							szServerName[LEN_SERVER];			//��������
};

//////////////////////////////////////////////////////////////////////////
//����������

#define DBC_REMODIFY_PASSWD			1									//�����޸�����
#define DBC_REMODIFY_ACCOUNTS		2									//�����޸��ʺ�

//////////////////////////////////////////////////////////////////////////

//�ֻ�����

//��¼����
#define	DBR_MB_LOGON_EMAIL			500									//�����¼
#define	DBR_MB_LOGON_EMAILCODE		501									//������֤��
#define	DBR_MB_LOGON_VISITOR		502									//�ο͵�¼
#define	DBR_MB_LOGON_SECRETKEY		503									//��Կ��¼
#define	DBR_MB_LOGON_ACCOUNTS		504									//�ʺŵ�¼
#define	DBR_MB_LOGON_THIRDPARTY		505									//��������¼
#define	DBR_MB_LOGON_MOBILECODE		506									//�ֻ���֤��
#define	DBR_MB_LOGON_MOBILEPHONE	507									//�ֻ���¼
#define DBR_MB_REGISTER_EMAIL		508									//����ע��
#define DBR_MB_REGISTER_ACCOUNTS	509									//�ʺ�ע��
#define DBR_MB_REGISTER_MOBILEPHONE	510									//�ֻ��ʺ�

//�ǳ�����
#define DBR_MB_LOGOUT_ACCOUNTS		511									//�ǳ��˺�

//�˺ŷ���
#define DBR_MB_LIKE_PLAYER			520									//�������
#define DBR_MB_MODIFY_LOGON_PASS	522									//�޸�����
#define DBR_MB_MODIFY_INDIVIDUAL	523									//�޸�����
#define DBR_MB_MODIFY_SYSTEM_FACE	524									//�޸�ͷ��
#define DBR_MB_MODIFY_CUSTOM_FACE	525									//�޸�ͷ��
#define DBR_MB_REAL_AUTH			526									//ʵ����֤

//��ѯ����
#define DBR_MB_QUERY_WEALTH			530									//��ѯ�Ƹ�
#define DBR_MB_QUERY_RANKINGINFO	531									//��ѯ��λ
#define DBR_MB_QUERY_LOVELINESS		532									//��ѯ����
#define DBR_MB_QUERY_SUBSIDY		533									//��ѯ�ͱ�

//�һ�����
#define DBR_MB_PURCHASE_MALLGOODS	540									//������Ʒ
#define DBR_MB_EXCHANGE_MALLGOODS	541									//�һ���Ʒ
#define DBR_MB_EXCHANGE_GIFTBAGCODE 542									//�һ������

//������֤
#define DBR_MB_BIND_EMAIL			550									//������
#define DBR_MB_UNBIND_EMAIL			551									//�������
#define DBR_MB_BIND_MOBILEPHONE		552									//���ֻ�
#define DBR_MB_UNBIND_MOBILEPHONE	553									//����ֻ�
#define DBR_MB_RESET_LOGON_PASS		554									//��������
#define DBR_MB_ACQUIRE_AUTHCODE		555									//��ȡ��֤��

//��������
#define DBR_MB_REQUEST_CHECKIN		570									//����ǩ��
#define DBR_MB_BIND_INVITE_CODE		571									//��������
#define DBR_MB_SUBMIT_INFORM_INFO	572									//�ύ����
#define DBR_MB_BIND_EXTERNAL_ACCOUNT 573								//���˺�
#define DBR_MB_UPDATE_USER_CACHE	574									//���»���	

//��������
#define DBR_MB_SEND_GIFT			580									//��������
#define DBR_MB_LOAD_GIFT			581									//��������
#define DBR_MB_GIFT_STATUS_UPDATE	582									//״̬����

//ת������
#define DBR_MB_WEALTH_TRANSFER		590									//����ת��

//Ӷ������
#define DBR_MB_DRAW_COMMISSION		600									//ȡ��Ӷ��

//ת������
#define DBR_MB_QUERY_TURNTABLE_LOTTERY  610								//��ѯת��
#define DBR_MB_REQUEST_TURNTABLE_LOTTERY 611							//����齱

//��������
#define DBR_MB_INSURE_QUERY			621									//��ѯ����
#define DBR_MB_INSURE_ENABLE		622									//���п�ͨ
#define DBR_MB_INSURE_SAVE			624									//���д洢
#define	DBR_MB_INSURE_TAKE			625									//������ȡ
#define DBR_MB_INSURE_TRANSFER		626									//����ת��
#define DBR_MB_INSURE_RESET_PASSWORD		627							//������������

//�ǳ�����
#define DBR_MB_NICKNAME_OPTION		628									//�ǳ�����
//////////////////////////////////////////////////////////////////////////////////

//�ο͵�¼
struct DBR_MB_LogonVisitor
{
	//��¼��Ϣ
	WORD							wMarketID;							//������ʶ
    DWORD							dwSpreaderID;						//����ID
    DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwDeviceKind;						//�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�����汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szMachineName[LEN_ACCOUNTS];		//��������
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾
	TCHAR							szPackageName[LEN_PACKAGE_NAME];	//Ӧ�ð���
	
	//������Ϣ
	WORD							wGateID;							//���ر�ʶ
	WORD							wLogonID;							//��¼��ʶ	
	LPVOID							pTokenParameter;					//�󶨲���
};

//��Կ��¼
struct DBR_MB_LogonSecretKey
{
	//��¼��Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szSecretKey[LEN_LOGON_TOKEN];		//��¼��Կ

	//������Ϣ
	WORD							wMarketID;							//������ʶ	
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwDeviceKind;						//�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�����汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾

	//������Ϣ
	WORD							wGateID;							//���ر�ʶ
	WORD							wLogonID;							//��¼��ʶ	
	LPVOID							pTokenParameter;					//�󶨲���
};

//�ʺŵ�¼
struct DBR_MB_LogonAccounts
{
	//��¼��Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�

	//������Ϣ
	WORD							wMarketID;							//������ʶ	
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwDeviceKind;						//�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�����汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾

	//������Ϣ
	WORD							wGateID;							//���ر�ʶ
	WORD							wLogonID;							//��¼��ʶ	
	LPVOID							pTokenParameter;					//�󶨲���
};

//��֤���¼
struct DBR_MB_LogonMobileCode
{
	//��¼��Ϣ
	TCHAR							szAuthCode[LEN_MD5];				//��½��֤��
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����

	//������Ϣ
	WORD							wMarketID;							//������ʶ	
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwSpreaderID;						//�ƹ�ԱID
	DWORD							dwDeviceKind;						//�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�����汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������	
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾

	//������Ϣ
	WORD							wGateID;							//���ر�ʶ
	WORD							wLogonID;							//��¼��ʶ	
	LPVOID							pTokenParameter;					//�󶨲���
};

//�ʺŵ�¼
struct DBR_MB_LogonThirdParty
{
	//��¼��Ϣ
	BYTE							cbGender;							//�û��Ա�
    DWORD							dwSpreaderID;						//�ƹ�ԱID
    DWORD							dwExternalID;						//������ʶ	
	TCHAR							szUniqueID[LEN_UNIQUEID];			//�û�UID
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	TCHAR							szUserDatum[LEN_USER_DATUM];		//�û�����
	TCHAR							szUserFaceUrl[LEN_USER_FACE];		//�û�ͷ��
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����	

	//������Ϣ
	WORD							wMarketID;							//������ʶ	
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwDeviceKind;						//�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�����汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾
	TCHAR							szPackageName[LEN_PACKAGE_NAME];	//Ӧ�ð���

	//������Ϣ
	WORD							wGateID;							//���ر�ʶ
	WORD							wLogonID;							//��¼��ʶ	
	LPVOID							pTokenParameter;					//�󶨲���
};

//�ֻ���¼
struct DBR_MB_LogonMobilePhone
{
	//��¼��Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//��¼�ʺ�

	//������Ϣ
	WORD							wMarketID;							//������ʶ	
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwDeviceKind;						//�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�����汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾

	//������Ϣ
	WORD							wGateID;							//���ر�ʶ
	WORD							wLogonID;							//��¼��ʶ	
	LPVOID							pTokenParameter;					//�󶨲���
};

//����ע��
struct DBR_MB_RegisterEMail
{
	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szAuthCode[LEN_MD5];				//��½��֤��
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	TCHAR							szEMail[LEN_EMAIL];					//����	
	DWORD							dwSpreaderID;						//����ID

	//�������
	TCHAR							szLogonPass[LEN_MD5];				//��¼����	

	//������Ϣ
	WORD							wMarketID;							//������ʶ
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwDeviceKind;						//�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�����汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������	
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾
	TCHAR							szPackageName[LEN_PACKAGE_NAME];	//Ӧ�ð���

	//������Ϣ
	WORD							wGateID;							//���ر�ʶ
	WORD							wLogonID;							//��¼��ʶ	
	LPVOID							pTokenParameter;					//�󶨲���
};

//�ʺ�ע��
struct DBR_MB_RegisterAccounts
{
	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�û��˺�
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�
	DWORD							dwSpreaderID;						//����ID

	//�������
	TCHAR							szLogonPass[LEN_MD5];				//��¼����	

	//������Ϣ
	WORD							wMarketID;							//������ʶ
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwDeviceKind;						//�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�����汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������	
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾
	TCHAR							szPackageName[LEN_PACKAGE_NAME];	//Ӧ�ð���

	//������Ϣ
	WORD							wGateID;							//���ر�ʶ
	WORD							wLogonID;							//��¼��ʶ	
	LPVOID							pTokenParameter;					//�󶨲���
};

//�ֻ�ע��
struct DBR_MB_RegisterMobilePhone
{
	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szAuthCode[LEN_MD5];				//��½��֤��
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����	
	DWORD							dwSpreaderID;						//����ID

	//�������
	TCHAR							szLogonPass[LEN_MD5];				//��¼����	

	//������Ϣ
	WORD							wMarketID;							//������ʶ
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwDeviceKind;						//�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�����汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������	
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾
	TCHAR							szPackageName[LEN_PACKAGE_NAME];	//Ӧ�ð���

	//������Ϣ
	WORD							wGateID;							//���ر�ʶ
	WORD							wLogonID;							//��¼��ʶ	
	LPVOID							pTokenParameter;					//�󶨲���
};


//�����¼
struct DBR_MB_LogonEMail
{
	//��¼��Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szEMail[LEN_EMAIL];					//����

	//������Ϣ
	WORD							wMarketID;							//������ʶ	
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwDeviceKind;						//�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�����汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾

	//������Ϣ
	WORD							wGateID;							//���ر�ʶ
	WORD							wLogonID;							//��¼��ʶ	
	LPVOID							pTokenParameter;					//�󶨲���
};

//��֤���¼
struct DBR_MB_LogonEMailCode
{
	//��¼��Ϣ
	TCHAR							szAuthCode[LEN_MD5];				//��½��֤��
	TCHAR							szEMail[LEN_EMAIL];					//����

	//������Ϣ
	WORD							wMarketID;							//������ʶ	
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwSpreaderID;						//�ƹ�ԱID
	DWORD							dwDeviceKind;						//�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�����汾
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������	
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾

	//������Ϣ
	WORD							wGateID;							//���ر�ʶ
	WORD							wLogonID;							//��¼��ʶ	
	LPVOID							pTokenParameter;					//�󶨲���
};

//�ǳ��˺�
struct DBR_MB_LogoutAccounts
{
	WORD							wGateID;							//���ر�ʶ
	WORD							wLogonID;							//��¼��ʶ
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwUserID;							//�û���ʶ
};

//�������
struct DBR_MB_LikePlayer
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientIP;							//���ӵ�ַ	
	DWORD							dwDstUserID;						//Ŀ���û�	
};

//�޸�����
struct DBR_MB_ModifyLogonPass
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szDesPassword[LEN_PASSWORD];		//�û�����
	TCHAR							szScrPassword[LEN_PASSWORD];		//�û�����
};


//�޸�����
struct DBR_MB_ModifyIndividual
{
	//��֤����
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientIP;							//���ӵ�ַ	

	//�û���Ϣ
	BYTE							cbGender;							//�û��Ա�
	WORD							wInfoMask;							//��Ϣ����
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�	
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��
};

//�޸�ͷ��
struct DBR_MB_ModifySystemFace
{
	//�û���Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwUserID;							//�û� I D
	
	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�޸�ͷ��
struct DBR_MB_ModifyAccessor
{
	//�����Ϣ
	DWORD							dwUserID;							//�û���ʶ
	WORD							wAccessorMask;						//�������
	WORD							wAccessorValue;						//�����ֵ

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�޸����
struct DBR_MB_ModifyFaceFrame
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û� I D	
	WORD							wFaceFrameID;						//����ʶ	

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�޸������
struct DBR_MB_ModifyChatFrame
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û� I D	
	WORD							wChatFrameID;						//����ʶ	

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�޸Ľ�ɫ
struct DBR_MB_ModifyUserActor
{
	//�û���Ϣ
	WORD							wActorID;							//��ɫ��ʶ
	DWORD							dwUserID;							//�û� I D	

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ѯ�Ƹ�
struct DBR_MB_QueryWealth
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientIP;							//���ӵ�ַ
};

//��ѯ��λ
struct DBR_MB_QueryRankingInfo
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientIP;							//���ӵ�ַ
};

//��ѯ����
struct DBR_MB_QueryLoveLiness
{
    DWORD							dwUserID;							//�û� I D
};

//��ѯ����
struct DBR_MB_QueryPayOrders
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientIP;							//���ӵ�ַ
};

//��ѯ����
struct DBR_MB_QueryWithdrawOrders
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientIP;							//���ӵ�ַ
};

//��ȡ��֤��
struct DBR_MB_AcquireAuthCode
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//��֤��Ϣ
	BYTE							cbCodeKind;							//��֤������
	BYTE							cbAuthKind;							//��֤����
	DWORD							dwBindUserID;						//�󶨱�ʶ
	TCHAR							szMobileEmail[LEN_EMAIL];			//�ֻ��ʼ�

	//��֤����Ϣ
	TCHAR							szAuthCode[LEN_AUTHCODE];			//��֤����
	TCHAR							szAuthCodeMD5[LEN_MD5];				//��֤����

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//���ֻ���
struct DBR_MB_BindMobilePhone
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szNewPassword[LEN_PASSWORD];		//�û�����

	//��֤��Ϣ
	DWORD							dwUnbindUserID;						//����ʶ
	TCHAR							szAuthCode[LEN_MD5];				//��֤��
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ�����

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//����ֻ���
struct DBR_MB_UnbindMobilePhone
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//��֤��Ϣ
	TCHAR							szAuthCode[LEN_MD5];				//��֤����
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ�����

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������
struct DBR_MB_BindEMail
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szNewPassword[LEN_PASSWORD];		//�û�����

	//��֤��Ϣ
	DWORD							dwUnbindUserID;						//����ʶ
	TCHAR							szAuthCode[LEN_MD5];				//��֤��
	TCHAR							szEMail[LEN_EMAIL];					//����

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�������
struct DBR_MB_UnbindEMail
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//��֤��Ϣ
	TCHAR							szAuthCode[LEN_MD5];				//��֤��
	TCHAR							szEMail[LEN_EMAIL];					//����

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��������
struct DBR_MB_ResetLogonPasswd
{
	//��֤��Ϣ
	DWORD							dwBindUserID;						//�󶨱�ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//��֤��Ϣ
	BYTE							cbCodeKind;							//��֤������
	TCHAR							szAuthCode[LEN_MD5];				//��֤��
	TCHAR							szMobileEmail[LEN_EMAIL];			//�ֻ�����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�տ��˺�
struct DBR_MB_ReceiptAccounts
{
	//�û���Ϣ
	DWORD							dwUserID;							//�󶨱�ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//�˺�����
	TCHAR							szAccountsName[LEN_ACCOUNTS];		//�˺�����

	//��֤��Ϣ
	TCHAR							szAuthCodeMD5[LEN_MD5];				//��֤����
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ�����	

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�һ���Ʒ
struct DBR_MB_ExchangeMallGoods
{
	DWORD							dwUserID;							//�󶨱�ʶ
	DWORD							dwMallGoodsID;						//��Ʒ��ʶ
};

//�һ������
struct DBR_MB_ExchangeGiftbagCode
{
	DWORD							dwUserID;							//�󶨱�ʶ
	TCHAR 							szGiftbagCode[16];					//�����

	DWORD							dwClientIP;							//���ӵ�ַ
};

//ɾ������
struct DBR_MB_DeleteFriend
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwFriendID;							//���ѱ�ʶ
};

//��Ӻ���
struct DBR_MB_ApplyAddFriend
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwObjectID;							//Ŀ���ʶ
	TCHAR							szApplyDesc[128];					//��������
};

//ͬ�����
struct DBR_MB_AgreeAddFriend
{
	DWORD							dwUserID;							//�û���ʶ	
	DWORD							dwApplyID;							//�����ʶ
};

//�ܾ����
struct DBR_MB_RefuseAddFriend
{
	DWORD							dwUserID;							//�û���ʶ	
	DWORD							dwApplyID;							//�����ʶ
	TCHAR							szRefuseDesc[128];					//�ܾ�����
};

//��Ϣ����
struct DBR_MB_SetupInfoPublic
{
	DWORD							dwUserID;							//�û���ʶ	
	bool							bInfoPublic;						//������־
};

//����ת��
struct DBR_MB_WealthTransfer
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	BYTE							cbCodeKind;							//��֤������
	TCHAR							szAuthCode[LEN_MD5];				//��֤����

	//ת����Ϣ
	DWORD							dwTargetGameID;						//��Ϸ��ʶ
	SCORE							lTransferAmount;					//ת�����

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ȡӶ��
struct DBR_MB_DrawCommission
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	SCORE							lDrawAmount;						//ȡ�����

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//���»���
struct DBR_MB_UpdateUserCache
{
	//������Ϣ
	DWORD							dwUserID;							//�û���ʶ
	WORD							wInfoMask;							//��Ϣ����

	//��Ϸ��Ϣ
	SCORE							lRankScore;							//��λ����
	DWORD							dwLoveliness;						//����ֵ
	WORD							wMasterOrder;						//����ȼ�
	DWORD							dwExperience;						//�û�����
	DWORD							dwMemberPoint;						//��Ա����		
};

//��������
struct DBR_MB_BindInviteCode
{
    //�û���Ϣ
    DWORD							dwUserID;							//�û���ʶ
    TCHAR							szPassword[LEN_PASSWORD];			//�û�����

    //������Ϣ
    DWORD							dwInviteCode;						//������
  
    //������Ϣ
    DWORD							dwClientIP;							//���ӵ�ַ
    TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//����ǩ��
struct DBR_MB_RequestCheckIn
{
	DWORD							dwUserID;							//�û���ʶ
	BOOL							bQuery;								//��ѯ��ʶ
	DWORD							dwClientIP;							//���ӵ�ַ
};

//���˺�
struct DBR_MB_BindExternalAccount
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	
	//������Ϣ
	DWORD							dwExternalID;						//������ʶ
	TCHAR							szUniqueID[LEN_UNIQUEID];			//�û�UUID
	TCHAR							szUserDatum[LEN_USER_DATUM];		//�û�����
	TCHAR							szUserFaceUrl[LEN_USER_FACE];		//ͷ��·��

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ	
};

//�ύͶ��
struct DBR_MB_SubmitInformInfo
{
    BYTE							cbInformKind;						//Ͷ������
    BYTE							cbInformSource;						//Ͷ����Դ
	DWORD							dwInformUserID;						//��ұ�ʶ	
	DWORD							dwBeInformUserID;					//��Ͷ�����
	TCHAR							szAttachmentUrl[128];				//������ַ
    TCHAR							szInformMessage[512];				//Ͷ����Ϣ	
    TCHAR							szInformContent[512];				//Ͷ������	

	//������Ϣ
	DWORD							dwClientIP;							//���ӵ�ַ	
};

//��������
struct DBR_MB_SendGift
{
    //�û���Ϣ
    DWORD							dwUserID;							//�û���ʶ
    DWORD							dwTargetUserID;						//�û���ʶ

    //��Ʒ��Ϣ
    DWORD							dwGoodsID;							//��Ʒ��ʶ
    DWORD							dwGoodsCount;						//��Ʒ����

    BYTE							cbSendType;							//��������(1:���� 2:��Ϸ)

    //������Ϣ
    DWORD							dwClientIP;							//���ӵ�ַ	

};

//��������
struct DBR_MB_LoadGift
{
    DWORD							dwUserID;							//�û���ʶ
    //DWORD							dwRecordID;							//��¼��ʶ
};

//����״̬����
struct DBR_MB_GiftStatusUpdata
{
    DWORD							dwUserID;							//�û���ʶ
    DWORD							dwRecordID;							//��¼��ʶ
};

//��ѯת�̳齱����
struct DBR_MB_QueryTurntableLottery
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwRecordID;							//��¼��ʶ
	DWORD							dwClientIP;							//���ӵ�ַ	
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������	
};

//����齱
struct DBR_MB_RequestTurntableLottery
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������	
};
//ʵ����֤
struct DBR_MB_RealAuth
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//֤������
};
//���в�ѯ
struct DBR_MB_InsureQuery
{
	DWORD							dwUserID;							//�û� I D
};
//���п�ͨ
struct DBR_MB_InsureEnable
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
};
//������
struct DBR_MB_InsureSave
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lSaveScore;							//������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	DWORD							dwClientIP;							//���ӵ�ַ
};

//��ȡ���
struct DBR_MB_InsureTake
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lTakeScore;							//��ȡ���
	TCHAR							szPassword[LEN_MD5];				//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	DWORD							dwClientIP;							//���ӵ�ַ
};

//ת�˽��
struct DBR_MB_InsureTransfer
{
	DWORD							dwUserID;							//�û� I D
	SCORE							lTransferScore;						//ת�˽��
	TCHAR							szPassword[LEN_MD5];				//��������
	DWORD							dwGameID;							//Ŀ��GameID
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
	DWORD							dwClientIP;							//���ӵ�ַ
};
//������������
struct DBR_MB_InsureResetPassword
{
	DWORD							dwUserID;							//�û� I D
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szSrcPassword[LEN_PASSWORD];		//ԭ����
	TCHAR							szDstPassword[LEN_PASSWORD];		//������
};
//�ͱ���ѯ
struct DBR_MB_SubsidyQuery
{
	DWORD							dwUserID;							//�û�ID
	DWORD							dwClientIP;							//���ӵ�ַ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};
//////////////////////////////////////////////////////////////////////////////////

//��¼���
#define DBO_MB_LOGON_SUCCESS		500									//��¼�ɹ�
#define DBO_MB_LOGON_FAILURE		501									//��¼ʧ��
#define DBO_MB_LOGOUT_RESULT		502									//�ǳ����

//�û�����
#define DBO_MB_USER_FACE_INFO		510									//ͷ����Ϣ
#define DBO_MB_USER_RANKING_INFO	511									//��λ��Ϣ
#define DBO_MB_USER_ACTION_MASK		512									//��Ϊ����
#define DBO_MB_USER_CACHE_INFO		514									//������Ϣ

//�����¼�
#define DBO_MB_USER_PACK_GOODS		520									//������Ʒ
#define DBO_MB_USER_GOODS_UPDATE	521									//��Ʒ����
#define DBO_MB_USER_WEALTH_UPDATE	522									//�Ƹ�����	
#define DBO_MB_USER_LOVELINESS_UPDATE	523								//�Ƹ�����

//��֤���
#define DBO_MB_BINDMP_SUCCESS		530									//�󶨳ɹ�	
#define DBO_MB_UNBINDMP_SUCCESS		531									//���ɹ�
#define DBO_MB_BIND_EMAIL_SUCCESS	532									//�󶨳ɹ�	
#define DBO_MB_UNBIND_EMAIL_SUCCESS	533									//���ɹ�	
#define DBO_MB_ACQUIREAC_RESULT		534									//��ȡ���

//�һ�����
#define DBO_MB_EXCHANGE_MALLGOODS	540									//�һ���Ʒ
#define DBO_MB_EXCHANGE_GIFTBAGCODE 541									//�һ�����

//��������
#define DBO_MB_SEND_GIFT_SUCCESS	550									//���ͳɹ�
#define DBO_MB_USER_GIFT_INFO		551									//������Ϣ
#define DBO_MB_USER_GIFT_INFO_FINISH 552								//������Ϣ���

//��������
#define DBO_MB_POST_MESSAGE			560									//
#define DBO_MB_CHECKIN_RESULT		561									//ǩ�����
#define DBO_MB_BIND_INVITE_CODE		562									//�󶨳ɹ�
#define DBO_MB_BIND_EXTERNAL_ACCOUNT 563								//�󶨳ɹ� 

//ת������
#define DBO_MB_TURNTABLE_LOTTERY_DATA 570								//ת������
#define DBO_MB_TURNTABLE_LOTTERY_RESULT 571								//�齱���

//ת������
#define DBO_MB_WEALTH_TRANSFER_SUCCESS 590								//ת�˳ɹ�

//��������
#define DBR_MB_INSURE_QUERY_RESULT	600									//���в�ѯ���
#define DBR_MB_INSURE_SUCCESS		601									//���в����ɹ�
#define DBR_MB_INSURE_FAILURE		602									//���в���ʧ��
#define DBR_MB_INSURE_MAIL_NOTIFY	603									//�����ʼ�֪ͨ

//��ѯ����
#define DBR_MB_RESULT_SUBSIDY		610									//�ͱ����

//�������
#define DBO_MB_OPERATE_SUCCESS		800									//�����ɹ�
#define DBO_MB_OPERATE_FAILURE		801									//����ʧ��

//////////////////////////////////////////////////////////////////////////////////

//��¼�ɹ�
struct DBO_MB_LogonSuccess
{
	//�û�����
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	DWORD							dwCustomID;							//�Զ�ͷ��
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	BYTE							cbInfoPublic;						//��Ϣ����
	DWORD 							dwUserRight;						//�û�Ȩ��
	TCHAR							szUniqueID[LEN_UNIQUEID];			//�û�UID
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
	TCHAR							szInsurePass[LEN_PASSWORD];			//��������	
	TCHAR							szLogonToken[LEN_LOGON_TOKEN];		//��¼����
	

	//�û�����
	TCHAR							szEmail[LEN_EMAIL];					//��������
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//����ǩ��
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ�����

	//����ȼ�
	DWORD							dwExperience;						//������ֵ

    //����ֵ
    DWORD							dwLoveLiness;						//����ֵ

	//��Ա��Ϣ
	WORD							wMasterOrder;						//����ȼ�
	DWORD							dwMemberPoint;						//��Ա����
	DWORD							dwMemberOverTime;					//����ʱ��

	//��λ��Ϣ
	tagUserRankingInfo				UserRankingInfo;					//��λ��Ϣ

	//�û��Ƹ�
	LONGLONG						lUserGold;							//�û����
	LONGLONG						lUserCard;							//�û�����
	LONGLONG						lUserInsure;						//�û�����	
	LONGLONG						lCommission;						//�û�Ӷ��

	//��Ϊ����
	DWORD							dwActionMaskEver;					//��Ϊ����
	DWORD							dwActionMaskPerDay;					//��Ϊ����
	DWORD							dwActionMaskPerWeek;				//��Ϊ����

	//������Ϣ
	WORD							wLockGateID;						//���ر�ʶ
	WORD							wLockLogonID;						//��¼��ʶ
	DWORD							dwLockSocketID;						//�����ʶ
	WORD							wLockKindID;						//���ͱ�ʶ	
	WORD							wLockServerID;						//��������
	WORD							wLockServerKind;					//��������	

	//�ϼ���Ϣ
	DWORD							dwParentGameID;						//�û� I D
	WORD							wParentFaceID;						//ͷ��ID
	TCHAR							szParentNickName[LEN_ACCOUNTS];		//�û��ǳ�

	//��Ʒ��Ϣ
	WORD							wGoodsCount;						//��Ʒ����
	tagGoodsHoldInfo				GoodsHoldInfo[128];					//������Ϣ

	//״̬��Ϣ
	BYTE							cbShutUpStatus;						//����״̬
	DWORD							dwShutUpOverTime;					//��ֹʱ��

	//ǩ����Ϣ
	WORD							wCheckInDays;						//ǩ������
	BYTE							cbCheckInState;						//ǩ��״̬

	//ʵ����Ϣ
	tagUserRealAuth					RealAuth;							//ʵ����Ϣ

	//�ǳ��޸���Ϣ
	WORD							wNickNameModifyTimes;				//�ǳ����޸Ĵ���
	WORD							wNickNameDailyModifyTimes;			//�ǳƽ����޸Ĵ���

	//������Ϣ				
	BYTE							cbRegisterMode;						//ע��ģʽ
	DWORD							dwRegisterTime;						//ע��ʱ��
	DWORD							dwLikedNumber;						//������Ŀ	
	DWORD							dwUIShieldMask;						//��������	
	TCHAR							szShieldKindList[256];				//�����б�
	TCHAR							szPresentGoodsList[512];			//ע������	
	TCHAR                           szUserFaceUrl[LEN_LOGON_TOKEN];     //touxiangdizhi kk jia

	//������Ϣ
	TCHAR							szDescribeString[128];				//������Ϣ
};


//��¼ʧ��
struct DBO_MB_LogonFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�ǳ����
struct DBO_MB_LogoutResult
{
	DWORD							dwUserID;							//�û���ʶ
	LONG							lResultCode;						//�������
};

//�û��Ƹ�
struct DBO_MB_UserWealth
{
	//�Ƹ���Ϣ
	WORD							wItemCount;							//��������
	tagWealthItem					WealthItem[6];						//�Ƹ�����
};

//�󶨳ɹ�
struct DBO_MB_BindMPSuccess
{
	//�Ƹ���Ϣ
	SCORE							lUserCard;							//��������

	//��ʾ��Ϣ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�󶨺���
	TCHAR							szNoticeString[128];				//��ʾ��Ϣ
};

//���ɹ�
struct DBO_MB_UnbindMPSuccess
{
	//�Ƹ���Ϣ
	SCORE							lUserCard;							//��������	

	//��ʾ��Ϣ
	TCHAR							szNoticeString[128];				//��ʾ��Ϣ
};

//�󶨳ɹ�
struct DBO_MB_BindEMailSuccess
{
	//�Ƹ���Ϣ
	SCORE							lUserCard;							//��������

	//��ʾ��Ϣ
	TCHAR							szEMail[LEN_EMAIL];					//������
	TCHAR							szNoticeString[128];				//��ʾ��Ϣ
};

//���ɹ�
struct DBO_MB_UnbindEMailSuccess
{
	//�Ƹ���Ϣ
	LONGLONG						lUserCard;							//��������

	//��ʾ��Ϣ
	TCHAR							szNoticeString[128];				//��ʾ��Ϣ
};

//ͷ����Ϣ
struct DBO_MB_UserFaceInfo
{
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwCustomID;							//�Զ�����
};

//�޸�ͷ��
struct DBR_MB_ModifyCustomFace
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û� I D
	DWORD							dwCustomID;							//�Զ����ʶ	
	TCHAR							szFacePath[LEN_USER_FACE];			//ͷ��·��
};

//�������
struct DBO_MB_UserAccessorUpdate
{
	WORD							wAccessorMask;						//�������
	WORD							wAccessorValue;						//�����ֵ
};

//��Ϊ����
struct DBO_MB_UserActionMask
{
	DWORD							dwActionMaskEver;					//��Ϊ����
	DWORD							dwActionMaskPerDay;					//��Ϊ����
	DWORD							dwActionMaskPerWeek;				//��Ϊ����
};

//��ȡ���
struct DBO_MB_AcquireACResult
{
	//��֤��Ϣ
	BYTE							cbCodeKind;							//��֤������
	BYTE							cbAuthKind;							//��֤����
	DWORD							dwBindUserID;						//�󶨱�ʶ
	TCHAR							szAuthCode[LEN_AUTHCODE];			//��֤��
	TCHAR							szMobileEmail[LEN_MOBILE_PHONE];	//�ֻ�����

	//�����Ϣ
	LONG							lResultCode;						//�������
	TCHAR							szNoticeString[128];				//��ʾ��Ϣ
};

//��ȡ���
struct DBO_MB_AcquireEMailACResult
{
	//��֤��Ϣ
	BYTE							cbAuthKind;							//��֤����
	DWORD							dwBindUserID;						//�󶨱�ʶ
	TCHAR							szAuthCode[LEN_AUTHCODE];			//��֤��
	TCHAR							szEMail[LEN_EMAIL];					//����

	//�����Ϣ
	LONG							lResultCode;						//�������
	TCHAR							szNoticeString[128];				//��ʾ��Ϣ
};

//�Ƹ�����
struct DBO_MB_PackGoods
{
	WORD							wGoodsCount;						//��������
	tagGoodsHoldInfo				GoodsHoldInfo[128];					//��Ʒ��Ϣ
};

//��Ʒ����
struct DBO_MB_GoodsUpdate
{
	WORD							wGoodsCount;						//��Ʒ����	
	tagGoodsHoldInfo				GoodsHoldInfo[128];					//��Ʒ��Ϣ
};

//�Ƹ�����
struct DBO_MB_WealthUpdate
{
	WORD							wItemCount;							//��������
	tagWealthItem					WealthItem[6];						//�Ƹ�����
};

//��������
struct DBO_MB_LoveLinessUpdate
{
	DWORD							dwLoveLiness;						//����ֵ
};

//�һ���Ʒ
struct DBO_MB_ExchangeMallGoods
{
	WORD							wIncGoodsID;						//��Ʒ��ʶ
	DWORD							dwIncGoodsCount;					//��������
	DWORD							dwIncGoodsIndate;					//��Ʒ��Ч��
	WORD							wExpendMoneyID;						//���ұ�ʶ
	SCORE							lMoneyBalance;						//�������			
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�һ���Ʒ
struct DBO_MB_ExchangeGiftbagCode
{
	LONG							lResultCode;						//�������
	TCHAR							szGiftBagGoodsList[128];			//��Ʒ�б�		
	TCHAR							szDescribeString[128];				//������Ϣ
};

//ǩ�����
struct DBO_MB_CheckInResult
{
	DWORD							dwUserID;							//�û� I D
	WORD							wCheckInDays;						//ǩ������
	BYTE							cbCheckInState;						//ǩ��״̬	
	TCHAR							szRewardGoodsList[128];				//��Ʒ�б�		
};

//��������ɹ�
struct DBO_MB_BindInviteCode
{
	//�ϼ���Ϣ
	DWORD							dwUserID;							//�û� I D
	DWORD							dwParentGameID;						//�û� I D
    WORD							wParentFaceID;						//ͷ��ID
    WORD							wParentFaceFrameID;					//ͷ���ID
    TCHAR							szParentNickName[LEN_ACCOUNTS];		//�û��ǳ�

	//�ʼ���Ϣ
	DWORD							dwMailID;							//�ʼ�ID

	//��ʾ��Ϣ
	TCHAR							szNoticeString[128];				//��ʾ��Ϣ
};

//���˺ųɹ�
struct DBO_MB_BindExternalAccount
{
	//�ⲿ��Ϣ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwExternalID;						//������ʶ
	TCHAR							szUniqueID[LEN_UNIQUEID];			//�û�UUID

	//��ʾ��Ϣ
	TCHAR							szNoticeString[128];				//��ʾ��Ϣ
};

//Ͷ����Ϣ
struct DBO_MB_PostMessage
{
	//ת����Ϣ
	DWORD							dwHashKeyID;						//ɢ�б�ʶ
	WORD							wServiceKind;						//��������	

	//��Ϣ����
	WORD							wMessageSize;						//���ݴ�С
	BYTE							cbMessageData[1024];				//��������
};

//���ͳɹ�
struct DBO_MB_SendGiftSuccess
{
    DWORD							dwRecordID;							//��¼ID
    DWORD							dwUserID;							//�û���ʶ
    DWORD							dwTargetUserID;						//�û���ʶ
    DWORD							dwLoveLiness;						//��������
    DWORD							dwMyLoveLiness;						//��������ֵ
    DWORD							dwTargetLoveLiness;					//�Է�����ֵ

    //��Ʒ��Ϣ
    DWORD							dwGoodsID;							//��Ʒ��ʶ
    DWORD							dwGoodsCount;						//��Ʒ����

	BYTE							cbSendType;							//��������(0:ȫ������ 1:˽������)
};

//ת�˳ɹ�
struct DBO_MB_WealthTransferSuccess
{
	DWORD							dwMailID;							//�ʼ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwTargetUserID;						//Ŀ���û�
};

//������Ϣ
struct DBO_MB_UserGiftInfo
{
    WORD							wItemCount;							//��������
	tagGiftInfo						GiftInfo[3];						//��������
};

//ת�̳齱����
struct DBO_MB_TurntableLotteryData
{
	//�齱����
	WORD							wPayLotteryTimes;					//��ֵ�齱����
	WORD							wFreeLotteryTimes;					//��ѳ齱����
	WORD							wGameTimeLotteryTimes;				//ʱ���齱����
	WORD							wGameCountLotteryTimes;				//�����齱����
	WORD							wGameExpendLotteryTimes;			//���ĳ齱����
	WORD							wUseLotteryTimes;					//���ó齱����
	WORD							wResidueLotteryTimes;				//ʣ��齱����
	WORD							wDailyMayLotteryTimes;				//ÿ�ճ齱����

	//��������
	DWORD							dwGameTimeProgress[2];				//��Ϸʱ������
	DWORD							dwGameCountProgress[2];				//��Ϸ��������
	DWORD							dwGameExpendProgress[2];			//��Ϸ���Ľ���

	//��¼����
	TCHAR							szTurntableRecord[4096];			//ת�̼�¼
};

//ת�̳齱���
struct DBO_MB_TurntableLotteryResult
{
	WORD							wHitIndex;							//��������
	WORD							wUseLotteryTimes;					//���ó齱����
	WORD							wResidueLotteryTimes;				//ʣ��齱����
};

//����ʧ��
struct DBO_MB_OperateFailure
{
	LONG							lErrorCode;							//�������
	WORD							wRequestCmdID;						//��������	
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�����ɹ�
struct DBO_MB_OperateSuccess
{
	LONG							lResultCode;						//��������
	WORD							wRequestCmdID;						//��������	
	TCHAR							szDescribeString[128];				//�ɹ���Ϣ
};
//���в�ѯ���
struct DBR_MB_InsureQueryResult
{
	BYTE							cbEnable;							//���п���
	SCORE							lScore;								//�û����
	SCORE							lInsureScore;						//���н��
	tagInsureConfig					SaveAndTake;						//��ȡ����
	tagInsureConfig					Transfer;							//ת������
};
//���гɹ�
struct DBR_MB_InsureSuccess
{
	SCORE							lScore;								//�û����
	SCORE							lInsureScore;						//���н��
};

//����ʧ��
struct DBR_MB_InsureFailure
{
	DWORD							dwResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};
//�����ʼ�֪ͨ
struct DBR_MB_InsureMailNotify
{
	DWORD							dwUserID;							//�û�ID
	DWORD							dwMailID;							//�ʼ�ID
};
//�ͱ�����
struct DBR_MB_Subsidy
{
	BYTE							cbDailyTimes;						//ÿ�մ���
	BYTE							cbResidualTimes;					//ʣ�����
	SCORE							lConsumeScore;						//�������
};
//////////////////////////////////////////////////////////////////////////////////


#endif