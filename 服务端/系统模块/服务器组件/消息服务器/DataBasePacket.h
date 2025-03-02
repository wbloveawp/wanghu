#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//�������ݰ�

//�û�����
#define DBR_GR_REQUEST_WEBPAGE		 1 										//������ҳ

//�ʼ�����
#define DBR_GR_USER_QUERY_MAIL		 10										//��ѯ�ʼ�
#define DBR_GR_USER_MODIFY_MAIL		 11										//�޸��ʼ�
#define DBR_GR_USER_INSERT_MAIL		 12										//�����ʼ�
#define DBR_GR_USER_DELETE_MAIL		 13										//ɾ���ʼ�
#define DBR_GR_CLAIM_MAIL_ATTACH	 14										//��ȡ����

//////////////////////////////////////////////////////////////////////////

//������ҳ
struct DBR_GR_RequestWebPage
{
	emHttpRequest					hrType;									//��������
	CHAR							szHeader[128];							//��ҳͷ��
	CHAR							szWebUrl[256];							//��ҳ����	
	CHAR							szPostData[2048];						//Ͷ������
};


//��ѯ�ʼ�
struct DBR_GR_UserQueryMail
{
	DWORD							dwUserID;							//�û���ʶ	
	DWORD							dwLatestMailID;						//�ʼ���ʶ
};

//�޸��ʼ�
struct DBR_GR_UserModifyMail
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwMailID;							//�ʼ���ʶ
	BYTE							cbMailState;						//�ʼ�״̬
};

//����ʼ�
struct DBR_GR_UserInsertMail
{
	DWORD							dwMailID;							//�ʼ���ʶ
	DWORD							dwUserID;							//�û���ʶ
	SYSTEMTIME						LastLogonTime;						//��¼ʱ��
};

//��ȡ����
struct DBR_GR_ClaimMailAttach
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwMailID;							//�ʼ���ʶ	
};

//ɾ���ʼ�
struct DBR_GR_UserDeleteMail
{
	DWORD							dwUserID;							//�û���ʶ
	DWORD							dwMailID;							//�ʼ���ʶ	
};

//////////////////////////////////////////////////////////////////////////
//�����Ϣ

//�ʼ��¼�
#define DBO_GR_USER_MAIL_INFO		110									//�ʼ���Ϣ
#define DBO_GR_USER_MAIL_FINISH		111									//�ʼ����
#define DBO_GR_USER_MAIL_UPDATE		112									//�ʼ�����
#define DBO_GR_USER_MAIL_INSERT		113									//�����ʼ�
#define DBO_GR_CLAIM_MAIL_ATTACH	114									//��ȡ����

//�����¼�
#define DBO_GR_USER_GOODS_UPDATE	120									//��Ʒ����
#define DBO_GR_USER_WEALTH_UPDATE	121									//�Ƹ�����	

//�������
#define DBO_GR_OPERATE_SUCCESS		1000								//�����ɹ�
#define DBO_GR_OPERATE_FAILURE		1001								//����ʧ��

//////////////////////////////////////////////////////////////////////////////////
//�ʼ��б�
struct DBO_GR_UserMailList
{
	DWORD							dwUserID;								//�û���ʶ
};

//�ʼ���Ϣ
struct DBO_GR_UserMailInfo
{
	WORD							wMailCount;							//�ʼ�����
	tagMailInfo						MailInfo[3];						//�ʼ���Ϣ	
};

//����ʼ�
struct DBO_GR_UserMailInsert
{
	DWORD							dwMailID;							//�ʼ���ʶ
	DWORD							dwUserID;							//�û���ʶ	
};

//�ʼ�����
struct DBO_GR_UserMailUpdate
{
	DWORD							dwMailID;							//�ʼ���ʶ
	BYTE							cbMailState;						//�ʼ�״̬
};

//��ȡ����
struct DBO_GR_ClaimMailAttach
{
	DWORD							dwMailID;							//�ʼ���ʶ		
	TCHAR							szAttachGoods[LEN_MAIL_ATTACH];		//�ʼ�����

	//�����Ϣ
	LONG							lResultCode;						//�������	
};

//�Ƹ�����
struct DBO_GR_WealthUpdate
{
	WORD							wItemCount;							//��������
	tagWealthItem					WealthItem[6];						//�Ƹ�����
};

//��Ʒ����
struct DBO_GR_GoodsUpdate
{
	WORD							wGoodsCount;						//��Ʒ����	
	tagGoodsHoldInfo				GoodsHoldInfo[128];					//��Ʒ��Ϣ
};

//����ʧ��
struct DBO_GR_OperateFailure
{
	LONG							lErrorCode;							//�������
	WORD							wRequestCmdID;						//��������
	TCHAR							szDescribeString[128];				//������Ϣ
};

//�����ɹ�
struct DBO_GR_OperateSuccess
{
	LONG							lErrorCode;							//��������
	WORD							wRequestCmdID;						//��������
	TCHAR							szDescribeString[128];				//�ɹ���Ϣ
};

//////////////////////////////////////////////////////////////////////////////////

#endif