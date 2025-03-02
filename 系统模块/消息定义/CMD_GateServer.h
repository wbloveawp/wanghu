#ifndef CMD_GATE_HEAD_FILE
#define CMD_GATE_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//�����

//���ط���
#define MDM_GG_GATE_SERVICE				1								//���ط���

//���ӷ���
#define SUB_GG_C_LINK_SERVICE			1								//���ӷ���
#define SUB_GG_C_DISLINK_SERVICE		2								//�Ͽ�����

//���ӽ��
#define SUB_GG_S_LINK_SUCCESS			100								//���ӳɹ�
#define SUB_GG_S_LINK_FAILURE			101								//����ʧ��
#define SUB_GG_S_LINK_SHUTDOWN			102								//���ӹر�

//////////////////////////////////////////////////////////////////////////////////
//��չ����

//�����ϣ
struct tagServiceHash
{
	DWORD							dwHashKeyID;						//ɢ�б�ʶ
	DWORD							dwHashParam;						//ɢ�в���	
	BYTE							cbExtendData[8];					//��չ����
};

//���������չ
struct tagLinkExtend_Server
{
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wServerLevel;						//���伶��
	WORD							wServerType;						//�������
};

//����·����չ
struct tagLinkExtend_Route
{
	DWORD							dwRouteKeyID;						//·�ɱ�ʶ	
};

//////////////////////////////////////////////////////////////////////////////////
//���ӷ���
struct CMD_GG_C_LinkService
{
	//������Ϣ
	WORD						    wServiceID;							//�����ʶ
	WORD							wServiceModule;						//����ģ��	

	//�û���Ϣ
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonToken[LEN_LOGON_TOKEN];		//��¼����

	//��չ��Ϣ
	BYTE							cbLinkExtend[LEN_LINK_EXTEND];		//������չ
};

//�Ͽ�����
struct CMD_GG_C_DislinkService
{
	//������Ϣ
	WORD							wServiceModule;						//����ģ��	
};

//���ӳɹ�
struct CMD_GG_S_LinkSuccess
{
	//������Ϣ
	WORD						    wServiceID;							//�����ʶ
	WORD							wServiceModule;						//����ģ��
};

//����ʧ��
struct CMD_GG_S_LinkFailure
{
	//������Ϣ
	WORD						    wServiceID;							//�����ʶ
	WORD							wServiceModule;						//����ģ��

	//������Ϣ
	LONG							lErrorCode;							//�������
	TCHAR							szErrorDescribe[128];				//��������
};

//���ӹر�
struct CMD_GG_S_LinkShutdown
{
	//������Ϣ
	WORD						    wServiceID;							//�����ʶ
	WORD							wServiceModule;						//����ģ��
};

////////////////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif