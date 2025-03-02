#ifndef CMD_MONITOR_SERVER_HEAD_FILE
#define CMD_MONITOR_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//ע�����
#define MDM_MS_REGISTER				10									//�û�����

//////////////////////////////////////////////////////////////////////////////////

//����ע��
#define SUB_MS_C_REGISTER_WORLD		100									//ע�������
#define SUB_MS_C_REGISTER_SERVER	101									//ע�᷿��

//////////////////////////////////////////////////////////////////////////////////

//ע����
#define SUB_MS_S_REGISTER_SUCCESS	300									//ע��ɹ�
#define SUB_MS_S_REGISTER_FAILURE	301									//ע��ʧ��

//////////////////////////////////////////////////////////////////////////////////

//ע�������
struct CMD_MS_C_RegisterWorld
{
	WORD							wGameID;							//��������
	WORD							wKindID;							//��������
	WORD							wServiceID;							//��������
	WORD							wServicePort;						//����˿�
	TCHAR							szServerName[LEN_SERVER];			//��������
	TCHAR							szServerHost[LEN_DOMAIN];			//�����ַ
};

//ע�᷿��
struct CMD_MS_C_RegisterServer
{
	WORD							wGameID;							//��������
	WORD							wKindID;							//��������
	WORD							wServerID;							//��������
	WORD							wServerPort;						//����˿�
	TCHAR							szServerName[LEN_SERVER];			//��������
	TCHAR							szServerHost[LEN_DOMAIN];			//�����ַ
};

//ע��ɹ�
struct CMD_MS_S_RegisterSuccess
{
	WORD							wServiceID;							//�����ʶ
};

//ע��ʧ��
struct CMD_MS_S_RegisterFailure
{
	DWORD							dwErrorCode;						//�������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//////////////////////////////////////////////////////////////////////////////////

//��������
#define MDM_MS_MANAGE_SERVICE		11									//��������

//////////////////////////////////////////////////////////////////////////////////

#define SUB_MS_DISMISS_GAME			100									//��ɢ��Ϸ

//////////////////////////////////////////////////////////////////////////////////
//��ɢԭ��
#define DISMISS_REASON_STOCKLOW		1									//���ֵ��

//////////////////////////////////////////////////////////////////////////////////
//��ɢ��Ϸ
struct CMD_MS_S_DismissGame
{
	LONG							lReason;							//��ɢԭ��
	WORD							wTableID;							//���ӱ�ʶ
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif