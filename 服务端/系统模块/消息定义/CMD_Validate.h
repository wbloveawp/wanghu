#ifndef CMD_VALIDATE_HEAD_FILE
#define CMD_VALIDATE_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//��֤����

#define MDM_VS_VALIDATE				1									//��֤����

//////////////////////////////////////////////////////////////////////////

//��֤ģʽ
#define SUB_VS_C_VALIDATE_LOGON		1									//��¼��֤

//////////////////////////////////////////////////////////////////////////

//��֤���
#define SUB_GP_S_VALIDATE_SUCCESS	100									//��֤�ɹ�
#define SUB_GP_S_VALIDATE_FAILURE	101									//��֤ʧ��

//////////////////////////////////////////////////////////////////////////

//��¼��֤
struct CMD_VS_C_ValidateLogon
{
	TCHAR							szAccreditID[LEN_ACCREDIT_ID];		//��Ȩ����
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������
};

//////////////////////////////////////////////////////////////////////////

//��֤�ɹ�
struct CMD_VS_S_ValidateSuccess
{
	//Э����ַ
	DWORD							dwCenterServerAddr;					//Э����ַ

	//������Ϣ
	tagDataBaseParameter			MessageDBParameter;					//������Ϣ
	tagDataBaseParameter			PlatformDBParameter;				//������Ϣ
	tagDataBaseParameter			AccountsDBParameter;				//������Ϣ	
	tagDataBaseParameter			GameGoldDBParameter;				//������Ϣ
	tagDataBaseParameter			GameMatchDBParameter;				//������Ϣ
	tagDataBaseParameter			GameBattleDBParameter;				//������Ϣ
	tagDataBaseParameter			GameRankingDBParameter;				//������Ϣ
};

//��֤ʧ��
struct CMD_VS_S_ValidateFailure
{
	LONG							lErrorCode;							//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif