#ifndef CMD_MESSAGE_SERVER_HEAD_FILE
#define CMD_MESSAGE_SERVER_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////
//�����

//��Ϣ����
#define MDM_AN_MESSAGE_SERVICE		10									//��Ϣ����

//////////////////////////////////////////////////////////////////////////
//��������
#define SUB_AN_C_ENTER_MESSAGE      1									//�������
#define SUB_AN_C_LEAVE_MESSAGE	 	2									//�뿪����

//�ʼ�����
#define SUB_AN_C_USER_QUERY_MAIL	10									//��ѯ�ʼ�
#define SUB_AN_C_USER_VIEW_MAIL		11									//�鿴�ʼ�
#define SUB_AN_C_USER_DELETE_MAIL	13									//ɾ���ʼ�
#define SUB_AN_C_CLAIM_MAIL_ATTACH	12									//��ȡ����								

//��������
#define SUB_AN_S_ENTER_SUCCESS		90									//����ɹ�

//Ӧ������
#define SUB_AN_S_MATCH_REMIND		100									//��������
#define SUB_AN_S_MARQUEE_NOTICE		101									//��������

//�ʼ�Ӧ��
#define SUB_AN_S_USER_MAIL_INFO		110									//�ʼ���Ϣ
#define SUB_AN_S_USER_MAIL_FINISH	111									//�ʼ����
#define SUB_AN_S_USER_MAIL_APPEND	112									//�ʼ����
#define SUB_AN_S_USER_MAIL_UPDATE	113									//�ʼ�����
#define SUB_AN_S_USER_MAIL_NOTIFY	114									//�ʼ�֪ͨ

//////////////////////////////////////////////////////////////////////////

//��ѯ�ʼ�
struct CMD_AN_C_UserQueryMail
{
	DWORD							dwLatestMailID;						//�����ʼ�
};

//�鿴�ʼ�
struct CMD_AN_C_UserViewMail
{
	DWORD							dwMailID;							//�ʼ���ʶ
};

//ɾ���ʼ�
struct CMD_AN_C_UserDeleteMail
{
	DWORD							dwMailID;							//�ʼ���ʶ
};

//��ȡ�ʼ�
struct CMD_AN_C_ClaimMailAttach
{
	DWORD							dwMailID;							//�ʼ���ʶ
};

//��������
struct CMD_AN_S_MatchRemind
{
	//������Ϣ	
	DWORD							dwMatchID;							//������ʶ
	LONGLONG						lMatchNo;							//��������
	DWORD							dwSurplusTime;						//ʣ��ʱ��
	TCHAR							szMatchName[32];					//��������
	SYSTEMTIME						MatchStartTime;						//����ʱ��

	//������Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wServerID;							//�����ʶ
	WORD							wServerPort;						//����˿�
	DWORD							dwServerAddr;						//�����ַ	
	TCHAR							szServerDomain[LEN_DOMAIN];			//��������
};

//�ʼ���Ϣ
struct CMD_AN_S_UserMailInfo
{
	WORD							wMailCount;							//�ʼ�����
	tagMailInfo						MailInfo[0];						//�ʼ���Ϣ	
};

//�ʼ����
struct CMD_AN_S_UserMailAppend
{
	tagMailInfo						MailInfo;							//�ʼ���Ϣ	
};

//�ʼ�����
struct CMD_AN_S_UserMailUpdate
{
	DWORD							dwMailID;							//�ʼ���ʶ
	BYTE							cbMailState;						//�ʼ�״̬	
};

//�ʼ�֪ͨ
struct CMD_AN_S_UserMailNotify
{
	DWORD							dwMailID;							//�ʼ���ʶ	
};

////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif