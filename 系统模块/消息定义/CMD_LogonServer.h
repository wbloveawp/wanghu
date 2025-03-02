#ifndef CMD_LONGON_HEAD_FILE
#define CMD_LONGON_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�����

//��¼����
#define MDM_GP_LOGON				10									//�㳡��¼

//////////////////////////////////////////////////////////////////////////

//��¼ģʽ
#define SUB_GP_LOGON_GAMEID			1									//I D ��¼
#define SUB_GP_LOGON_ACCOUNTS		2									//�ʺŵ�¼
#define SUB_GP_LOGON_MANAGETOOL		3									//������
#define SUB_GP_REGISTER_ACCOUNTS	4									//ע���ʺ�

//�ǳ�����
#define SUB_GP_LOGOUT_NOTIFY		10									//�ǳ�����

//////////////////////////////////////////////////////////////////////////

//��¼���
#define SUB_GP_LOGON_SUCCESS		100									//��¼�ɹ�
#define SUB_GP_LOGON_FAILURE		101									//��¼ʧ��
#define SUB_GP_LOGON_FINISH			102									//��¼���
#define SUB_GP_LOGON_ACTIVE			103									//�����ʺ�
#define SUB_GP_SHOW_ONLINE			104									//��ʾ����

//������ʾ
#define SUB_GP_UPDATE_NOTIFY		200									//������ʾ
#define SUB_GP_UPDATE_SILENT		201									//��̨����

//////////////////////////////////////////////////////////////////////////

//I D ��¼
struct CMD_GP_LogonByGameID
{	
	DWORD							dwGameID;							//��Ϸ I D
	DWORD							dwPlazaVersion;						//�㳡�汾	
	TCHAR							szPassword[LEN_MD5];				//��¼����
};

//�ʺŵ�¼
struct CMD_GP_LogonByAccounts
{
	DWORD							dwPlazaVersion;						//�㳡�汾
	WORD							wMaskServerVisible;					//��������
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
};

//ע���ʺ�
struct CMD_GP_RegisterAccounts
{	
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwPlazaVersion;						//�㳡�汾
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szBankPass[LEN_MD5];				//��������
};

//��¼�ɹ�
struct CMD_GP_LogonSuccess
{
	//��������
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	DWORD							dwGroupID;							//���ű�ʶ
	DWORD							dwCustomID;							//�Զ���ʶ
	BYTE							cbMasterOrder;						//����Ա����
	DWORD							dwProtectID;						//�ܱ�ID
	BYTE							cbMoorMachine;						//�󶨻�����ʶ

	//����ȼ�
	DWORD							dwExperience;						//������ֵ

	//��Ա�ȼ�
	DWORD							dwMemberPoint;						//��Ա����
	DWORD							dwMemberOverTime;					//����ʱ��

	//�Ƹ���Ϣ
	LONGLONG						lUserGold;							//�û����
	LONGLONG						lInsureGold;						//���н��

	//�û���Ϣ
	BYTE							cbAge;								//�û�����
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szGroupName[LEN_GROUP_NAME];		//��������
};

//��¼ʧ�ܱ�ʶ
#define LGF_NEED_ACTIVATE			8									//��Ҫ����
#define LGF_CHANGE_ACCOUNT			9									//�����ʺ�

//��¼ʧ��
struct CMD_GP_LogonFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��½���
struct CMD_GP_LogonFinish
{
	WORD							wIntermitTime;						//�ж�ʱ��
	WORD							wOnLineCountTime;					//����ʱ��
	DWORD							dwFunctionOption;					//����ѡ��
};

//�ǳ�����
struct CMD_GP_LogoutNotify
{
	TCHAR							szDescribeString[128];				//������Ϣ
};

//������ʾ
struct CMD_GP_UpdateNotify
{
	BYTE							cbMustUpdate;						//ǿ������
	BYTE							cbAdviceUpdate;						//��������
	DWORD							dwCurrentVersion;					//��ǰ�汾
};

//�����ʺ�
struct CMD_GP_NotifyActivateAccounts
{
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szTips[128];						//��ʾ��Ϣ
	TCHAR							szAdTitle[128];						//������
	TCHAR							szAdUrl[128];						//����ַ
};

//////////////////////////////////////////////////////////////////////////
//Я����Ϣ CMD_GP_RegisterAccounts

#define DTP_GP_USER_INFO			1									//�û���Ϣ
#define DTP_GP_MACHINE_ID			2									//������ʶ
#define DTP_GP_PASSPORT				3									//�û����
#define DTP_GP_SPREADER				4									//�Ƽ����
#define DTP_GP_BANK_PASS			5									//��������
#define DTP_GP_NICKNAME				6									//�û��ǳ�
#define DTP_GP_TRUENAME				7									//��ʵ����
#define DTP_GP_NEW_COMPUTER_ID      8									//������ʶ
#define DTP_GP_VERIFY_COMPUTER_ID   9									//������ʶ
#define DTP_GP_UNIQUEID				10									//�û�UID
#define DTP_GP_MOBILE_PHONE			11									//�ֻ�����
#define DTP_GP_PACKAGE_NAME			12									//Ӧ�ð���

//�û���Ϣ
struct DTP_GP_UserInfo
{
	BYTE							cbAge;								//�û�����
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szArea[LEN_AREA];					//������Ϣ
	TCHAR							szCity[LEN_CITY];					//������Ϣ
	TCHAR							szProvince[LEN_PROVINCE];			//ʡ����Ϣ
};

//�û����
struct DTP_GP_Passport
{
	BYTE							cbPassportType;						//�������
	TCHAR							szPassportID[LEN_PASSPORT_ID];		//�û����
};

//////////////////////////////////////////////////////////////////////////
//Я����Ϣ CMD_GP_LogonSuccess

#define DTP_GP_AREA_INFO			1									//������Ϣ
#define DTP_GP_GROUP_INFO			2									//������Ϣ
#define DTP_GP_MEMBER_INFO			3									//��Ա��Ϣ
#define	DTP_GP_UNDER_WRITE			4									//����ǩ��


//������Ϣ
struct DTP_GP_AreaInfo
{
	TCHAR							szArea[LEN_AREA];					//������Ϣ
	TCHAR							szCity[LEN_CITY];					//������Ϣ
	TCHAR							szProvince[LEN_PROVINCE];			//ʡ����Ϣ
};

//������Ϣ
struct DTP_GP_GroupInfo
{
	DWORD							dwGroupID;							//��������
	TCHAR							szGroupName[LEN_GROUP_NAME];		//��������
};

//��Ա��Ϣ
struct DTP_GP_MemberInfo
{
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	DWORD							dwMemberPoint;						//��Ա����
	DWORD							dwMemberOverTime;					//����ʱ��
};

//////////////////////////////////////////////////////////////////////////
//�б�����

#define MDM_GP_SERVER_LIST			11									//�б���Ϣ

//////////////////////////////////////////////////////////////////////////

//��ȡ����
#define SUB_GP_GET_LIST				1									//��ȡ�б�
#define SUB_GP_GET_SERVER			2									//��ȡ����
#define SUB_GP_GET_ONLINE			3									//��ȡ����
#define SUB_GP_GET_COLLECTION		4									//��ȡ�ղ�

//////////////////////////////////////////////////////////////////////////

//�б���Ϣ
#define SUB_GP_LIST_TYPE			100									//�����б�
#define SUB_GP_LIST_KIND			101									//�����б�
#define SUB_GP_LIST_NODE			102									//�ڵ��б�
#define SUB_GP_LIST_SERVER			103									//�����б�
#define SUB_GP_LIST_CUSTOM			104									//�����б�

//�����Ϣ
#define SUB_GP_LIST_FINISH			200									//�������
#define SUB_GP_SERVER_FINISH		201									//�������

//������Ϣ
#define SUB_GR_KINE_ONLINE			300									//��������
#define SUB_GR_SERVER_ONLINE		301									//��������

//////////////////////////////////////////////////////////////////////////

//��ȡ����
struct CMD_GP_GetOnline
{
	WORD							wServerCount;						//������Ŀ
	WORD							wOnLineServerID[MAX_SERVER];		//�����ʶ
};

//��������
struct CMD_GP_KindOnline
{
	WORD							wKindCount;							//������Ŀ
	tagOnLineInfoKind				OnLineInfoKind[MAX_KIND];			//��������
};

//��������
struct CMD_GP_ServerOnline
{
	WORD							wServerCount;						//������Ŀ
	tagOnLineInfoServer				OnLineInfoServer[MAX_SERVER];		//��������
};

//////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GP_USER_SERVICE			12									//�û�����

//////////////////////////////////////////////////////////////////////////

//�˺ŷ���
#define SUB_GP_MODIFY_ACCOUNTS		1									//�޸��ʺ�
#define SUB_GP_MODIFY_LOGON_PASS	2									//�޸�����
#define SUB_GP_MODIFY_INSURE_PASS	3									//�޸�����
#define SUB_GP_MODIFY_INDIVIDUAL	4									//�޸�����
#define SUB_GP_BIND_MACHINE			6									//��������
#define SUB_GP_UN_BIND_MACHINE		7									//��������

//��ѯ����
#define	SUB_GP_QUERY_INDIVIDUAL		20									//��ѯ��Ϣ

//////////////////////////////////////////////////////////////////////////

//�������
#define SUB_GP_OPERATE_SUCCESS		100									//�����ɹ�
#define SUB_GP_OPERATE_FAILURE		101									//����ʧ��

//�û���Ϣ
#define SUB_GP_USER_INDIVIDUAL		203									//��������

//////////////////////////////////////////////////////////////////////////

//�޸��ʺ�
struct CMD_GP_ModifyAccounts
{
	//��֤����
	DWORD							dwUserID;							//�û� I D
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
};

//�޸�����
struct CMD_GP_ModifyLogonPass
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szDesPassword[LEN_PASSWORD];		//�û�����
	TCHAR							szScrPassword[LEN_PASSWORD];		//�û�����
};

//�޸�����
struct CMD_GP_ModifyInsurePass
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szDesPassword[LEN_PASSWORD];		//�û�����
	TCHAR							szScrPassword[LEN_PASSWORD];		//�û�����
};

//�޸�����
struct CMD_GP_ModifyIndividual
{
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
};

//��ѯ��Ϣ
struct CMD_GP_QueryIndividual
{
	DWORD							dwUserID;							//�û� I D
};

//����ʧ��
struct CMD_GP_OperateFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�����ɹ�
struct CMD_GP_OperateSuccess
{
	LONG							lResultCode;						//��������
	TCHAR							szDescribeString[128];				//�ɹ���Ϣ
};

//��������
struct CMD_GP_UserIndividual
{
	DWORD							dwUserID;							//�û� I D
};

//��������
struct CMD_GP_BindMachine
{
	//��֤����
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��������
struct CMD_GP_UnbindMachine
{
	//��֤����
	DWORD							dwUserID;							//�û� I D
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//////////////////////////////////////////////////////////////////////////
//Я����Ϣ CMD_GP_UserIndividual

#define DTP_GP_UI_USER_NOTE			1									//�û�˵��
#define DTP_GP_UI_COMPELLATION		2									//��ʵ����
#define DTP_GP_UI_SEAT_PHONE		3									//�̶��绰
#define DTP_GP_UI_MOBILE_PHONE		4									//�ƶ��绰
#define DTP_GP_UI_QQ				5									//Q Q ����
#define DTP_GP_UI_EMAIL				6									//�����ʼ�
#define DTP_GP_UI_DWELLING_PLACE	7									//��ϵ��ַ
#define DTP_GP_UI_NICKNAME			8									//�û��ǳ�

//��ʾ������Ϣ����
#define SHOW_TOTAL_USER_COUNT		0x00000001							//��ʾ������
#define SHOW_KIND_USER_COUNT		0x00000002							//��ʾ��������
#define SHOW_SERVER_USER_COUNT		0x00000004							//��ʾ��������



//////////////////////////////////////////////////////////////////////////////////
//���÷���

#define MDM_GP_CONFIG_SERVICE		13									//���÷���

//��������
#define SUB_GP_QUERY_GAME_CONFIG	100									//��ѯ����
#define SUB_GP_UPDATE_BASIC_CONFIG	101									//��������
#define SUB_GP_UPDATE_WEIGHT_CONFIG	102									//��������

//������Ϣ
#define SUB_GP_GAME_BASIC_CONFIG	200									//��������
#define SUB_GP_GAME_WEIGHT_CONFIG	201									//Ȩ������
#define SUB_GP_GAME_CONFIG_FINISH	202									//�������

//////////////////////////////////////////////////////////////////////////////////
//��ѯ����
struct CMD_GP_QueryGameConfig
{
	WORD							wKindID;							//���ͱ�ʶ��ʶ
};

//��������
struct CMD_GP_UpdateBasicConfig
{
	WORD							wKindID;							//���ͱ�ʶ
	TCHAR							szBasicConfig[4000];				//��������
};

//��������
struct CMD_GP_UpdateWeightConfig
{
	WORD							wKindID;							//���ͱ�ʶ
	BYTE							cbTypeID;							//Ȩ������
	WORD							wItemCount;							//������Ŀ
	tagTimesWeightItem				TimesWeightItem[128];				//Ȩ������
};

//��������
struct CMD_GP_GameBasicConfig
{
	TCHAR							szBasicConfig[4000];				//��������
};

//Ȩ������
struct CMD_GP_GameWeightConfig
{
	WORD							wItemCount;							//������Ŀ
	tagTimesWeightItem				TimesWeightItem[128];				//Ȩ������
};

//////////////////////////////////////////////////////////////////////////////////
//��������

#define MDM_GP_ROBOT_SERVICE		14									//��������

//��������
#define SUB_GP_QUERY_PARAMETER		100									//��ѯ����
#define SUB_GP_APPEND_PARAMETER		101									//��Ӳ���
#define SUB_GP_MODIFY_PARAMETER		102									//�޸Ĳ���
#define SUB_GP_DELETE_PARAMETER		103									//ɾ������

//������Ϣ
#define SUB_GP_ROBOT_OPERATE_RESULT	200									//�������

//////////////////////////////////////////////////////////////////////////////////
//��ȡ����
struct CMD_GP_QueryParameter
{
	WORD							wServerID;							//�����ʶ
};

//��Ӳ���
struct CMD_GP_AppendParameter
{
	WORD							wServerID;							//�����ʶ
	tagRobotParameter				RobotParameter;						//��������
};

//�޸Ĳ���
struct CMD_GP_ModifyParameter
{
	WORD							wServerID;							//�����ʶ
	tagRobotParameter				RobotParameter;						//��������
};

//ɾ������
struct CMD_GP_DeleteParameter
{
	WORD							wServerID;							//�����ʶ
	DWORD							dwBatchID;							//���α�ʶ
};

//////////////////////////////////////////////////////////////////////////////////
//��������
struct CMD_GP_RobotOperateResult
{
	WORD							wSubCommdID;						//��������
	WORD							wParameterCount;					//������Ŀ
	tagRobotParameter				RobotParameter[MAX_BATCH];		//��������
};

//////////////////////////////////////////////////////////////////////////////////
//������

#define MDM_GP_STOCK_SERVICE		15									//������

//������
#define SUB_GP_QUERY_STOCK_INFO		100									//��ѯ���
#define SUB_GP_APPEND_STOCK_INFO	101									//��ӿ��
#define SUB_GP_MODIFY_STOCK_INFO	102									//�޸Ŀ��
#define SUB_GP_DELETE_STOCK_INFO	103									//ɾ�����
#define SUB_GP_ADJUST_STOCK_SCORE	104									//�������
#define SUB_GP_ADJUST_JACKPOT_SCORE	105									//�����ʽ�

//�����Ϣ
#define SUB_GP_STOCK_OPERATE_RESULT	200									//�������

//////////////////////////////////////////////////////////////////////////////////
//��ѯ���
struct CMD_GP_QueryStockInfo
{
	TCHAR							szKindID[128];						//�����б�
};

//��Ӳ���
struct CMD_GP_AppendStockInfo
{	
	tagStockInfo					StockInfo;							//�����Ϣ
};

//�޸Ĳ���
struct CMD_GP_ModifyStockInfo
{	
	tagStockInfo					StockInfo;							//�����Ϣ
};

//ɾ�����
struct CMD_GP_DeleteStockInfo
{
	WORD							wStockID;							//����ʶ	
};

//�������
struct CMD_GP_AdjustStockScore
{
	WORD							wStockID;							//����ʶ
	SCORE							lChangeScore;						//�������
};

//�����ʽ�
struct CMD_GP_AdjustJackpotScore
{
	WORD							wStockID;							//����ʶ
	WORD							wLevelID;							//�ʳصȼ�
	SCORE							lChangeScore;						//�������
};

//////////////////////////////////////////////////////////////////////////////////
//�������
struct CMD_GP_StockOperateResult
{
	WORD							wPacketIdx;							//��������
	WORD							wSubCommdID;						//��������
	WORD							wStockCount;						//�����Ŀ
	tagStockInfo					StockList[24];						//����б�
};

//////////////////////////////////////////////////////////////////////////////////
//�������
#define MDM_GP_MANAGER_SERVICE		16									//�������

#define SUB_GP_MODIFY_OPTION		1									//�޸�ѡ��
#define SUB_GP_EXECUTE_COMMAND		2									//ִ������

#define SUB_GP_MANAGE_SUCCESS		100									//����ɹ�

//////////////////////////////////////////////////////////////////////////////////
//�޸Ĺ���
struct CMD_GP_ModifyOption
{
	tagModifyOption					ModifyOption;						//�༭ѡ��
	WORD							wServerCount;						//��������
	WORD							wServerIDList[512];					//�����б�
};

//////////////////////////////////////////////////////////////////////////////////

//��¼����
#define MDM_MB_LOGON				100									//�ֻ���½

//��¼ģʽ
#define SUB_MB_LOGON_EMAIL			1									//�����½
#define SUB_MB_LOGON_EMAILCODE		2									//������֤��
#define SUB_MB_LOGON_MOBILECODE		3									//�ֻ���֤��
#define SUB_MB_LOGON_MOBILEPHONE	4									//�ֻ���½
#define SUB_MB_LOGON_ACCOUNTS	    5									//�ʺŵ�¼
#define SUB_MB_LOGON_VISITOR		6									//�ο͵�¼
#define SUB_MB_LOGON_SECRETKEY		7									//��Կ��¼
#define SUB_MB_LOGON_THIRDPARTY		8									//������¼
#define SUB_MB_REGISTER_EMAIL		9									//����ע��
#define SUB_MB_REGISTER_ACCOUNTS	10									//�ʺ�ע��
#define SUB_MB_REGISTER_MOBILEPHONE	11									//�ֻ�ע��

//�ǳ�����
#define SUB_MB_LOGOUT_NOTIFY		20									//�ǳ�����

//��¼���
#define SUB_MB_LOGON_SUCCESS		100									//��¼�ɹ�
#define SUB_MB_LOGON_FAILURE		101									//��¼ʧ��
#define SUB_MB_LOGON_FINISH			102									//��¼���

//������ʾ
#define SUB_MB_UPDATE_NOTIFY		200									//������ʾ

//////////////////////////////////////////////////////////////////////////////////
//����ѡ��
#define FO_FORBID_RECHARGE			0x00000001							//��ֹ��ֵ


//////////////////////////////////////////////////////////////////////////////////

//�ο͵�¼
struct CMD_MB_LogonVisitor
{
	//�豸��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	WORD							wMarketID;							//������ʶ	
    DWORD                           dwSpreaderID;						//����ID
    DWORD                           dwDeviceKind;                       //�豸����
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾
	
	//ϵͳ��Ϣ
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�㳡�汾		
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ	
	TCHAR							szMachineName[LEN_ACCOUNTS];		//��������		
};

//��Կ��¼
struct CMD_MB_LogonSecretKey
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	WORD							wMarketID;							//������ʶ
	DWORD                           dwDeviceKind;                       //�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�㳡�汾		
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾

	//��¼��Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szSecretKey[LEN_LOGON_TOKEN];		//��¼����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
};

//�ʺŵ�¼
struct CMD_MB_LogonAccounts
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	WORD							wMarketID;							//������ʶ
	DWORD                           dwDeviceKind;                       //�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�㳡�汾		
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾

	//��¼��Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
};

//�ֻ���֤��
struct CMD_MB_LogonMobileCode
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	WORD							wMarketID;							//������ʶ
	DWORD                           dwSpreaderID;						//�ƹ�ԱID
	DWORD                           dwDeviceKind;                       //�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�㳡�汾	
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾

	//��¼��Ϣ
	TCHAR							szAuthCode[LEN_MD5];				//��֤��
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ�����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ	
};

//�ֻ���¼
struct CMD_MB_LogonMobilePhone
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	WORD							wMarketID;							//������ʶ
	DWORD                           dwDeviceKind;                       //�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�㳡�汾		
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾

	//��¼��Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ�����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ	
};

//��������¼
struct CMD_MB_LogonThirdParty
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	WORD							wMarketID;							//������ʶ
    DWORD                           dwSpreaderID;						//�ƹ�ԱID
    DWORD                           dwDeviceKind;                       //�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�㳡�汾	
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾

	//��¼��Ϣ
	BYTE							cbGender;							//�û��Ա�
	DWORD							dwExternalID;						//������ʶ	
	TCHAR							szUniqueID[LEN_UNIQUEID];			//�û�UID
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����
};

//ע���ʺ�
struct CMD_MB_RegisterAccounts
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	WORD							wMarketID;							//������ʶ
    DWORD                           dwSpreaderID;						//����ID
    DWORD                           dwDeviceKind;                       //�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�㳡�汾
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾

	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�û��˺�
	TCHAR							szNickName[LEN_NICKNAME];			//�û��ǳ�		
	TCHAR							szLogonPass[LEN_MD5];				//��¼����	

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ	
};


//����ע��
struct CMD_MB_RegisterEMail
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	WORD							wMarketID;							//������ʶ
	DWORD                           dwSpreaderID;						//����ID
	DWORD                           dwDeviceKind;                       //�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�㳡�汾
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾

	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szAuthCode[LEN_MD5];				//��֤��
	TCHAR							szLogonPass[LEN_MD5];				//��¼����
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�	
	TCHAR							szEMail[LEN_EMAIL];					//����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ	
};

//�ֻ�ע��
struct CMD_MB_RegisterMobilePhone
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	WORD							wMarketID;							//������ʶ
	DWORD                           dwSpreaderID;						//����ID
	DWORD                           dwDeviceKind;                       //�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�㳡�汾
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾

	//ע����Ϣ
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	TCHAR							szAuthCode[LEN_MD5];				//��֤��
	TCHAR							szLogonPass[LEN_MD5];				//��¼����
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�	
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�绰����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ	
};


//�����¼
struct CMD_MB_LogonEMail
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	WORD							wMarketID;							//������ʶ
	DWORD                           dwDeviceKind;                       //�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�㳡�汾		
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾

	//��¼��Ϣ
	TCHAR							szPassword[LEN_MD5];				//��¼����
	TCHAR							szEMail[LEN_EMAIL];					//����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ	
};

//������֤��
struct CMD_MB_LogonEMailCode
{
	//ϵͳ��Ϣ
	WORD							wModuleID;							//ģ���ʶ
	WORD							wMarketID;							//������ʶ
	DWORD                           dwSpreaderID;						//�ƹ�ԱID
	DWORD                           dwDeviceKind;                       //�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�㳡�汾	
	TCHAR							szMachineOSID[LEN_MACHINE_OSID];	//ϵͳ�汾

	//��¼��Ϣ
	TCHAR							szAuthCode[LEN_MD5];				//��֤��
	TCHAR							szEMail[LEN_EMAIL];					//����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������ʶ	
};

//��½���
struct CMD_MB_LogonFinish
{
	DWORD							dwFuncOption;						//����ѡ��	
};

//�ǳ�����
struct CMD_MB_LogoutNotify
{
	TCHAR							szDescribeString[128];				//������Ϣ
};

///////////////////////////////////////////////////////////////////////////////////////////

//�ο���֤
struct CMD_MB_OnlineCheck
{
	DWORD							dwUserID;							//�û� I D
};

//��¼�ɹ�
struct CMD_MB_LogonSuccess
{
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	DWORD							dwCustomID;							//�Զ�ͷ��
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
	TCHAR							szInsurePass[LEN_PASSWORD];			//��������

	//����ȼ�
	DWORD							dwExperience;						//������ֵ

	//����ֵ
	DWORD							dwLoveLiness;						//����ֵ

	//�û�Ȩ��
	DWORD							dwUserRight;						//�û�Ȩ��

	//��Ա����
	WORD							wMasterOrder;						//����ȼ�
	DWORD							dwMemberPoint;						//��Ա����
	DWORD							dwMemberOverTime;					//����ʱ��

	//�Ƹ���Ϣ
	SCORE							lUserGold;							//�û����
	SCORE							lUserCard;							//�û�����
	SCORE							lUserInsure;						//�û�����	
	SCORE							lCommission;						//�û�Ӷ��

	//��Ϊ����
	DWORD							dwActionMaskEver;					//��Ϊ����
	DWORD							dwActionMaskPerDay;					//��Ϊ����
	DWORD							dwActionMaskPerWeek;				//��Ϊ����

	//�ϼ���Ϣ
	DWORD							dwParentGameID;						//�û� I D
	WORD							wParentFaceID;						//ͷ��ID
	TCHAR							szParentNickName[LEN_ACCOUNTS];		//�û��ǳ�

	//������Ϣ
	WORD							wLockKindID;						//���ͱ�ʶ
	WORD							wLockServerID;						//��������
	WORD							wLockServerKind;					//��������

	//״̬��Ϣ	
	BYTE							cbShutUpStatus;						//����״̬
	DWORD							dwShutUpOverTime;					//���ʱ��

	//������Ϣ	
	DWORD							dwServerTime;						//������ʱ��	
	DWORD							dwLikedNumber;						//������Ŀ	
	BYTE							cbRegisterMode;						//ע��ģʽ
	DWORD							dwRegisterTime;						//ע��ʱ��
	DWORD							dwUIShieldMask;						//��������	
};

//��¼ʧ��
struct CMD_MB_LogonFailure
{
	LONG							lResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//������ʾ
struct CMD_MB_UpdateNotify
{
	BYTE							cbMustUpdate;						//ǿ������
	BYTE							cbAdviceUpdate;						//��������
	DWORD							dwCurrentVersion;					//��ǰ�汾
};

//////////////////////////////////////////////////////////////////////////////////
//�б�����

#define MDM_MB_SERVER_LIST			101									//�б���Ϣ

//��ȡ����
#define SUB_MB_GET_LIST				1									//��ȡ�б�
#define SUB_MB_GET_SERVER			2									//��ȡ����
#define SUB_MB_GET_ONLINE			3									//��ȡ����
#define SUB_MB_GET_JACKPOT			4									//��ȡ����

//�б���Ϣ
#define SUB_MB_LIST_KIND			100									//�����б�
#define SUB_MB_LIST_SERVER			101									//�����б�
#define SUB_MB_LIST_MATCH			102									//�����б�
#define SUB_MB_LIST_LOGON			105									//��¼�б�
#define SUB_MB_LIST_AGENT			106									//�����б�
#define SUB_MB_SERVER_AGENT			107									//�������
#define SUB_MB_LIST_ACCESS			108									//���ط���
#define SUB_MB_LIST_ONLINE			109									//������Ϣ
#define SUB_MB_LIST_JACKPOT			110									//������Ϣ
#define SUB_MB_LIST_FINISH			200									//�б����
#define SUB_MB_SERVER_FINISH		201									//�������

//////////////////////////////////////////////////////////////////////////////////

//������Ϣ
struct tagOnlineInfo
{
	WORD							wServerID;							//�����ʶ
	DWORD							dwOnlineCount;						//��������
};

//������Ϣ
struct tagJackpotInfo
{
	WORD							wKindID;							//���ͱ�ʶ
	SCORE							lJackpotScore;						//���ػ���
};

//��ȡ����
struct tagGetOnline
{
	WORD							wKindID;							//���ͱ�ʶ
};

//��ȡ����
struct tagGetServer
{
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwUserID;							//�û���־
};

//��ȡ����
struct tagGetOption
{
	WORD							wModuleID;							//���ͱ�ʶ
};


//////////////////////////////////////////////////////////////////////////
//��������

#define MDM_MB_USER_SERVICE			102									//�û�����

//////////////////////////////////////////////////////////////////////////

//�û�����
#define SUB_MB_LIKE_PLAYER			1									//�������
#define SUB_MB_MODIFY_LOGON_PASS	2									//�޸�����
#define SUB_MB_MODIFY_INDIVIDUAL	5									//�޸�����
#define SUB_MB_REAL_AUTH			6									//ʵ����֤

//��ѯ����
#define SUB_MB_QUERY_WEALTH			20									//��ѯ�Ƹ�
#define	SUB_MB_QUERY_INDIVIDUAL		21									//��ѯ��Ϣ
#define	SUB_MB_QUERY_RANKINGINFO	22									//��ѯ��λ
#define SUB_MB_QUERY_RANKING_LIST	23									//��ѯ��	
#define SUB_MB_QUERY_SUBSIDY		24									//��ѯ�ͱ�

//�޸���Ϣ
#define SUB_MB_MODIFY_SYSTEM_FACE	31									//�޸�ͷ��
#define SUB_MB_MODIFY_CUSTOM_FACE	32									//�޸�ͷ��

//��֤����֤
#define SUB_MB_BIND_EMAIL			40									//������
#define SUB_MB_UNBIND_EMAIL			41									//�������
#define SUB_MB_BIND_MOBILEPHONE		42									//���ֻ�
#define SUB_MB_UNBIND_MOBILEPHONE	43									//����ֻ�
#define SUB_MB_RESET_LOGON_PASS		44									//��������
#define SUB_MB_ACQUIRE_AUTHCODE		49									//��ȡ��֤��

//�һ�����
#define SUB_MB_EXCHANGE_MALLGOODS	60									//�һ���Ʒ
#define SUB_MB_EXCHANGE_GIFTBAGCODE 61									//�һ������

//��������
#define SUB_MB_REQUEST_CHECKIN		70									//����ǩ��
#define SUB_MB_BIND_INVITE_CODE		71									//��������
#define SUB_MB_SUBMIT_INFORM_INFO	72									//�ύͶ��
#define SUB_MB_BIND_EXTERNAL_ACCOUNT 73									//���˺�

//��������
#define SUB_MB_SEND_GIFT			80									//��������
#define SUB_MB_LOAD_GIFT			81									//��������
#define SUB_MB_GIFT_STATUS_UPDATE	82									//����״̬����
#define SUB_MB_SEND_GIFT_SUCCESS	83									//���ͳɹ�
#define SUB_MB_RECV_GIFT_NOTIFY		84									//����֪ͨ
#define SUB_MB_GIFT_INFO			85									//������Ϣ
#define SUB_MB_GIFT_INFO_FINISH		86									//������Ϣ���
#define SUB_MB_GLOBAL_GIFT_NOTIFY	87									//ȫ������֪ͨ

//ת������
#define SUB_MB_QUERY_TURNTABLE_LOTTERY 90								//ת������
#define SUB_MB_REQUEST_TURNTABLE_LOTTERY 91								//����齱

//ת������
#define SUB_MB_WEALTH_TRANSFER		110									//����ת��

//Ӷ������
#define SUB_MB_DRAW_COMMISSION		120									//��ȡӶ��
//��������
#define SUB_MB_INSURE_QUERY			130									//��ѯ����
#define SUB_MB_INSURE_ENABLE		131									//���п�ͨ
#define SUB_MB_INSURE_SAVE			132									//���д洢
#define	SUB_MB_INSURE_TAKE			133									//������ȡ
#define SUB_MB_INSURE_TRANSFER		134									//����ת��
#define SUB_MB_INSURE_RESET_PASSWORD	135								//������������
//////////////////////////////////////////////////////////////////////////

//��ѯ��Ϣ
#define SUB_MB_USER_INDIVIDUAL		200									//��������
#define SUB_MB_USER_FACE_INFO		201									//ͷ����Ϣ
#define SUB_MB_USER_ACTION_MASK		202									//������Ϣ
#define SUB_MB_USER_RANKING_INFO	203									//��λ��Ϣ
#define SUB_MB_USER_ACCESSOR_UPDATE	204									//�������
#define SUB_MB_USER_CACHE_INFO		205									//������Ϣ
#define SUB_MB_RANKING_LIST_DATA	206									//������
#define SUB_MB_USER_SUBSIDY			207									//�ͱ�����

//ϵͳ����
#define SUB_MB_SHUTUP_STATUS_UPDATE	210									//״̬����
#define SUB_MB_RESET_USERDATA		211									//��������

//ת������
#define SUB_MB_TURNTABLE_OPTION		220									//ת������
#define SUB_MB_TURNTABLE_LOTTERY_DATA	221								//ת������
#define SUB_MB_TURNTABLE_LOTTERY_RESULT 222								//�齱���

//ǩ������
#define SUB_MB_CHECKIN_OPTION		230									//ǩ������
#define SUB_MB_CHECKIN_RESULT		231									//ǩ�����

//��֤����֤
#define SUB_MB_BINDMP_SUCCESS		350									//�󶨳ɹ�
#define SUB_MB_UNBINDMP_SUCCESS		351									//���ɹ�
#define SUB_MB_BIND_EMAIL_SUCCESS	352									//�󶨳ɹ�			
#define SUB_MB_UNBIND_EMAIL_SUCCESS	353									//���ɹ�			
#define SUB_MB_ACQUIREAC_RESULT		355									//��ȡ���

//Ӧ������
#define SUB_MB_BIND_INVITE_CODE_SUCCESS	 360							//�󶨳ɹ�
#define SUB_MB_BIND_EXTERNAL_ACCOUNT_SUCCESS 361						//�󶨳ɹ�

//��������
#define SUB_MB_INSURE_QUERY_RESULT		370								//���в�ѯ���
#define SUB_MB_INSURE_SUCCESS			371								//���в����ɹ�
#define SUB_MB_INSURE_FAILURE			372								//���в���ʧ��

//////////////////////////////////////////////////////////////////////////
//�������
#define ACCESSOR_MASK_ACTOR			0x01								//�������
#define ACCESSOR_MASK_FACE_FRAME	0x02								//�������
#define ACCESSOR_MASK_CHAT_FRAME	0x04								//�������
#define ACCESSOR_MASK_CARD_BACK		0x08								//�������
#define ACCESSOR_MASK_TABLE_BACK	0x10								//�������

//////////////////////////////////////////////////////////////////////////
//�������
struct CMD_MB_LikePlayer
{
	DWORD							dwDstUserID;						//Ŀ���û�
};

//��������
struct CMD_MB_ResetUserData
{
	WORD							wDayID;								//���ڱ�ʶ
	WORD							wDayOfWeek;							//���ڱ�ʶ
};

//�޸�����
struct CMD_MB_ModifyLogonPass
{
	TCHAR							szDesPassword[LEN_PASSWORD];		//�û�����
	TCHAR							szScrPassword[LEN_PASSWORD];		//�û�����	
};

//�޸�����
struct CMD_MB_ModifyIndividual
{
	BYTE							cbGender;							//�û��Ա�	
};

//����ǩ��
struct CMD_MB_RequestCheckIn
{
	TCHAR							szOptionDigest[LEN_MD5];			//����ժҪ
};

//���˺�
struct CMD_MB_BindExternalAccount
{
	DWORD							dwExternalID;						//������ʶ	
	TCHAR							szUniqueID[LEN_UNIQUEID];			//�û�UID
};

//�ύͶ��
struct CMD_MB_SubmitInformInfo
{
	BYTE							cbInformKind;						//Ͷ������
	BYTE							cbInformSource;						//Ͷ����Դ
	DWORD							dwBeInformUserID;					//��Ͷ�����
};

//��������
struct CMD_MB_UserIndividual
{
	DWORD							dwUserID;							//�û� I D
};

//�û�ͷ��
struct CMD_MB_UserFaceInfo
{
	WORD							wFaceID;							//ͷ���ʶ
	DWORD							dwCustomID;							//�Զ���ʶ
};

//�޸�ͷ��
struct CMD_MB_ModifySystemFace
{
	WORD							wFaceID;							//ͷ���ʶ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�޸�ͷ��
struct CMD_MB_ModifyCustomFace
{
	DWORD							dwCustomID;							//�Զ����ʶ
	TCHAR							szFacePath[LEN_USER_FACE];			//ͷ��·��
};

//��Ϊ����
struct CMD_MB_UserActionMask
{
	DWORD							dwActionMaskEver;					//��Ϊ����
	DWORD							dwActionMaskPerDay;					//��Ϊ����
	DWORD							dwActionMaskPerWeek;				//��Ϊ����
};

//���з��ʽ��
struct CMD_MB_BankAccessResult
{
	LONG							lReturnCode;						//���ش���
	LONGLONG						lGold;								//���ؽ��
	LONGLONG						lInsureGold;						//���н��
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��ѯ���
struct CMD_MB_BankQueryResult
{
	LONG							lResultCode;						//���ش���
	DWORD							dwUserID;							//�û�ID
	DWORD							dwGameID;							//��ϷID
	TCHAR							szAccounts[LEN_ACCOUNTS];			//�û��ʺ�
	TCHAR							szDescribeString[128];				//������Ϣ
};

//��ѯ��λ���
struct CMD_MB_UserRankingInfo
{
	tagUserRankingInfo				UserRankingInfo;					//�����λ��Ϣ
};

//״̬����
struct CMD_MB_ShutUpStatusUpdate
{
	BYTE							cbShutUpStatus;						//����״̬
	DWORD							dwShutUpOverTime;					//���ʱ��
};

//��ȡ��֤��
struct CMD_MB_AcquireAuthCode
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//��֤��Ϣ
	BYTE							cbCodeKind;							//��֤������
	BYTE							cbAuthKind;							//��֤����
	DWORD							dwBindUserID;						//�󶨱�ʶ
	TCHAR							szMobileEmail[LEN_EMAIL];			//�ֻ��ʼ�

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//���ֻ���
struct CMD_MB_BindMobilePhone
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
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//����ֻ���
struct CMD_MB_UnbindMobilePhone
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//��֤��Ϣ
	TCHAR							szAuthCode[LEN_MD5];				//��֤��
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ�����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��������
struct CMD_MB_ResetLogonPasswd
{
	//��֤��Ϣ
	DWORD							dwBindUserID;						//�󶨱�ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//��֤��Ϣ
	BYTE							cbCodeKind;							//��֤������
	TCHAR							szAuthCode[LEN_MD5];				//��֤����
	TCHAR							szMobileEmail[LEN_EMAIL];			//�ֻ�����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//������
struct CMD_MB_BindEMail
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szNewPassword[LEN_PASSWORD];		//�û�����

	//��֤��Ϣ
	DWORD							dwUnbindUserID;						//����ʶ
	TCHAR							szEMail[LEN_EMAIL];					//����
	TCHAR							szAuthCode[LEN_MD5];				//��֤��	

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�������
struct CMD_MB_UnbindEMail
{
	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����

	//��֤��Ϣ
	TCHAR							szEMail[LEN_EMAIL];					//����
	TCHAR							szAuthCode[LEN_MD5];				//��֤��	

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�󶨳ɹ�
struct CMD_MB_BindMPSuccess
{
	//��ʾ��Ϣ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�󶨺���
	TCHAR							szNoticeString[128];				//��ʾ��Ϣ
};

//���ɹ�
struct CMD_MB_UnbindMPSuccess
{
	//��ʾ��Ϣ
	TCHAR							szNoticeString[128];				//��ʾ��Ϣ
};

//��ȡ���
struct CMD_MB_AcquireACResult
{
	//��֤��Ϣ
	BYTE							cbAuthKind;							//��֤����
	DWORD							dwBindUserID;						//�󶨱�ʶ	

	//�����Ϣ
	LONG							lResultCode;						//�������
	TCHAR							szNoticeString[128];				//��ʾ��Ϣ
};

//�󶨳ɹ�
struct CMD_MB_BindEMailSuccess
{
	//��ʾ��Ϣ
	TCHAR							szEMail[LEN_EMAIL];					//������
	TCHAR							szNoticeString[128];				//��ʾ��Ϣ
};

//���ɹ�
struct CMD_MB_UnbindEMailSuccess
{
	//��ʾ��Ϣ
	TCHAR							szNoticeString[128];				//��ʾ��Ϣ
};

//ɾ������
struct CMD_MB_DeleteFriend
{
	DWORD							dwFriendID;							//���ѱ�ʶ
};

//��Ӻ���
struct CMD_MB_ApplyAddFriend
{
	DWORD							dwObjectID;							//Ŀ���ʶ
	TCHAR							szApplyDesc[128];					//��������
};

//ͬ�����
struct CMD_MB_AgreeAddFriend
{
	DWORD							dwApplyID;							//�����ʶ
};

//�ܾ����
struct CMD_MB_RefuseAddFriend
{
	DWORD							dwApplyID;							//�����ʶ
	TCHAR							szRefuseDesc[128];					//�ܾ�����
};

//��Ϣ����
struct CMD_MB_SetupInfoPublic
{
	bool							bInfoPublic;						//������־
};

//����ת��
struct CMD_MB_WealthTransfer
{
	//��֤��Ϣ
	BYTE							cbCodeKind;							//��֤������
	TCHAR							szAuthCode[LEN_MD5];				//��֤����

	//ת����Ϣ
	DWORD							dwTargetGameID;						//��Ϸ��ʶ
	SCORE							lTransferAmount;					//ת�����

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//ȡ��Ӷ��
struct CMD_MB_DrawCommission
{
	//��ȡ��Ϣ
	SCORE							lDarwinAmount;						//��ȡ���

	//������Ϣ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//�һ���Ʒ
struct CMD_MB_ExchangeMallGoods
{
	DWORD							dwMallGoodsID;						//��Ʒ��ʶ
};

//�һ������
struct CMD_MB_ExchangeGiftbagCode
{
	TCHAR							szGiftbagCode[16];					//�����
};

//��ѯ��
struct CMD_MB_QueryRankingList
{
	BYTE							cbListType;							//������
};

//��������
struct CMD_MB_BindInviteCode
{
    //�û���Ϣ
    DWORD							dwUserID;							//�û���ʶ
    TCHAR							szPassword[LEN_PASSWORD];			//�û�����

    //������Ϣ
    DWORD							dwInviteCode;						//������

    //������Ϣ
    TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��������ɹ�
struct CMD_MB_BindInviteCodeSuccess
{
    //�ϼ���Ϣ
    DWORD							dwParentGameID;						//�û� I D
    WORD							wParentFaceID;						//ͷ��ID
    TCHAR							szParentNickName[LEN_ACCOUNTS];		//�û��ǳ�

    //��ʾ��Ϣ
	TCHAR							szNoticeString[128];				//��ʾ��Ϣ
};

//�󶨳ɹ� 
struct CMD_MB_BindExternalAccountSuccess
{
	DWORD							dwExternalID;						//������ʶ	
	TCHAR							szUniqueID[LEN_UNIQUEID];			//�û�UID

	//��ʾ��Ϣ
	TCHAR							szNoticeString[128];				//��ʾ��Ϣ
};

//��������
struct CMD_MB_SendGift
{
    //�û���Ϣ
    DWORD							dwUserID;							//�û���ʶ

	//��Ʒ��Ϣ
    DWORD							dwGoodsID;							//��Ʒ��ʶ
	DWORD							dwGoodsCount;						//��Ʒ����
};

//���ͳɹ�
struct CMD_MB_SendGiftSuccess
{
    DWORD							dwTargetUserID;						//Ŀ���û�
    DWORD							dwLoveLiness;						//����ֵ

    //��Ʒ��Ϣ
    DWORD							dwGoodsID;							//��Ʒ��ʶ
    DWORD							dwGoodsCount;						//��Ʒ����

    BYTE							cbSendType;							//��������(0:ȫ������ 1:˽������)
};

//��������
struct CMD_MB_LoadGift
{
    DWORD							dwUserID;							//�û���ʶ
};

//������Ϣ
struct CMD_MB_UserGiftInfo
{
    WORD							wItemCount;							//��������
    tagGiftInfo						GiftInfo[0];						//������Ϣ	
};

//ȫ������֪ͨ
struct CMD_MB_GlobalGiftNotify
{
    DWORD							dwUserID;							//�û���ʶ
    DWORD							dwTargetUserID;						//Ŀ���û�

    //��Ʒ��Ϣ
    DWORD							dwGoodsID;							//��Ʒ��ʶ
    DWORD							dwGoodsCount;						//��Ʒ����
    DWORD							dwLoveLiness;						//����ֵ

    BYTE							cbSendType;							//��������(0:ȫ������ 1:˽������)
};

//����״̬����
struct CMD_MB_GiftStatusUpdate
{
    DWORD							dwUserID;							//�û���ʶ
    DWORD							dwRecordID;							//��¼��ʶ
};

//��ѯת�̳齱����
struct CMD_MB_QueryTurntableLottery
{	
	DWORD							dwRecordID;							//��¼��ʶ
	TCHAR							szOptionDigest[LEN_MD5];			//����ժҪ
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������
};

//����齱
struct CMD_MB_RequestTurntableLottery
{
	TCHAR							szMachineID[LEN_MACHINE_ID];		//������
	TCHAR							szOptionDigest[LEN_MD5];			//����ժҪ
};

//ת������
struct CMD_MB_TurntableOption
{
	TCHAR							szOptionDigest[LEN_MD5];			//����ժҪ
	TCHAR							szTurantableAward[2048];			//ת�̽���
};

//ת�̳齱����
struct CMD_MB_TurntableLotteryData
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

	//ת�̼�¼
	TCHAR							szTurantableRecord[4096];			//ת�̼�¼
};

//ת�̳齱���
struct CMD_MB_TurntableLotteryResult
{
	WORD							wHitIndex;							//��������
	WORD							wUseLotteryTimes;					//���ó齱����
	WORD							wResidueLotteryTimes;				//ʣ��齱����
};

//������
struct CMD_MB_RankingListData
{
	BYTE							cbListType;							//������
	TCHAR							szRankingList[8196];				//������
};
//ʵ����֤
struct CMD_MB_RealAuth
{
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//֤������
};
//���п�ͨ
struct CMD_MB_InsureEnable
{
	TCHAR							szPassword[LEN_PASSWORD];			//�û�����
};
//������
struct CMD_MB_InsureSave
{
	SCORE							lSaveScore;							//������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//��ȡ���
struct CMD_MB_InsureTake
{
	SCORE							lTakeScore;							//��ȡ���
	TCHAR							szPassword[LEN_MD5];				//��������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//ת�˽��
struct CMD_MB_InsureTransfer
{
	SCORE							lTransferScore;						//ת�˽��
	TCHAR							szPassword[LEN_MD5];				//��������
	DWORD							dwGameID;							//Ŀ��GameID
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//���в�ѯ���
struct CMD_MB_InsureQueryResult
{
	BYTE							cbEnable;							//���ر�ʶ 0-�� 1-��
	SCORE							lScore;								//�û����
	SCORE							lInsureScore;						//�������
	tagInsureConfig					SaveAndTake;						//��ȡ����
	tagInsureConfig					Transfer;							//ת������
};
//������������
struct CMD_MB_InsureResetPassword
{
	TCHAR							szSrcPassword[LEN_PASSWORD];		//ԭ����
	TCHAR							szDstPassword[LEN_PASSWORD];		//������
};
//ǩ������
struct CMD_MB_CheckInOption
{
	bool							bCheckInEnabled;					//ǩ������
	TCHAR							szOptionDigest[LEN_MD5];			//����ժҪ
	TCHAR							szCheckInRewards[1024];				//ǩ������
};

//ǩ�����
struct CMD_MB_CheckInResult
{
	WORD							wCheckInDays;						//ǩ������
	BYTE							cbCheckInState;						//ǩ��״̬		
};

//���гɹ�
struct CMD_MB_InsureSuccess
{
	SCORE							lScore;								//�û����
	SCORE							lInsureScore;						//�������
};

//����ʧ��
struct CMD_MB_InsureFailure
{
	DWORD							dwResultCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};
//��ѯ�ͱ�
struct CMD_MB_SubsidyQuery
{
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};
//�ͱ�����
struct CMD_MB_Subsidy
{
	BYTE							cbDailyTimes;						//ÿ�մ���
	BYTE							cbResidualTimes;					//ʣ�����
	SCORE							lConsumeScore;						//�������
};
//////////////////////////////////////////////////////////////////////////

//Я����Ϣ
#define DTP_MB_USER_INFO			0x0001								//�û���Ϣ
#define DTP_MB_MACHINE_ID			0x0002								//������ʶ
#define DTP_MB_NICKNAME				0x0004								//�û��ǳ�
#define DTP_MB_UNIQUEID				0x0008								//�û�UID
#define DTP_MB_COMPELLATION			0x0010								//��ʵ����
#define DTP_MB_MOBILE_PHONE			0x0020								//�ֻ�����
#define DTP_MB_EXTERNAL_DATUM		0x0040								//�ⲿ����
#define DTP_MB_UNDER_WRITE			0x0080								//����ǩ��
#define DTP_MB_LOGON_TOKEN			0x0100								//��¼����
#define DTP_MB_USERFACE_URL			0x0200								//ͷ���ַ
#define DTP_MB_RANKING_INFO			0x0400								//��λ��Ϣ
#define DTP_MB_USER_EMAIL			0x0800								//��������
#define DTP_MB_TRANSFER_PARAM		0x1000								//ת�˲���
#define DTP_MB_REGISTER_PRESENT		0x2000								//ע������
#define DTP_MB_PACKAGE_NAME			0x4000								//Ӧ�ð���
#define DTP_MB_REAL_AUTH			0x8000								//ʵ����Ϣ
//wwrq#define DTP_MB_LOGON_FACEURL        0x10000	                            //ͷ���ַKK JIA

//Ͷ��Я����Ϣ
#define DTP_MB_INFORM_ATTACHMENTURL	0x0001								//������ַ
#define DTP_MB_INFORM_MESSAGE		0x0002								//Ͷ����Ϣ
#define DTP_MB_INFORM_CONTENT		0x0004								//Ͷ������


//////////////////////////////////////////////////////////////////////////

//ת������
struct DTP_GP_TransferParam
{
	bool							bTransferEnabled;					//ת�˿���
	WORD 							wServiceFeeRate;					//�������
	SCORE							lReservedAmount;					//�������
	SCORE							lMinTransferAmount;					//ת�˽��
	SCORE							lMaxTransferAmount;					//ת�˽��
};

//Ӷ������
struct DTP_GP_CommissionParam
{
	bool							bDrawEnabled;						//ȡ������
	WORD 							wServiceFeeRate;					//�������
	SCORE							lMinDrawAmount;						//���ٽ��	
};

//��λ��Ϣ
struct DTP_GP_RankingInfo
{
	DWORD							dwRankID;							//������ʶ	
	WORD							wSeasonID;							//������ʶ
	LONG							lWinCount;							//Ӯ����Ŀ
	LONG							lLostCount;							//������Ŀ
	LONG							lWinStreakCount;					//��ʤ����
	LONG							lLoveHeart;							//��������
	SCORE							lRankScore;							//��λ����	
};

//ʵ����Ϣ
struct DTP_MB_RealAuth
{
	TCHAR							szCompellation[LEN_COMPELLATION];	//��ʵ����
	TCHAR							szPassPortID[LEN_PASS_PORT_ID];		//֤������
};
//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif