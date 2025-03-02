#ifndef CMD_LOGIC_SERVICE_HEAD_FILE
#define CMD_LOGCI_SERVICE_HEAD_FILE

#pragma pack(1)

//////////////////////////////////////////////////////////////////////////////////
//��������
#define MDM_IM_ANDROID_SERVICE		10									//��������

//////////////////////////////////////////////////////////////////////////////////

//��������
#define SUB_AS_C_LOGON_SERVICE		1									//��½����
#define SUB_AS_C_SEARCH_GROUP		2									//���Ҿ��ֲ�
#define SUB_AS_C_LOAD_GROUP_BATCH	3									//��������
#define SUB_AS_C_LOAD_GROUP_MEMBER	4									//Ⱥ���Ա
#define SUB_AS_C_LOAD_BATCH_MEMBER	5									//���γ�Ա

//��������
#define SUB_AS_C_APPEND_BATCH		10									//�������
#define SUB_AS_C_MODIFY_BATCH		11									//�޸�����
#define SUB_AS_C_DELETE_BATCH		12									//ɾ������

//��������
#define SUB_AS_C_APPEND_ANDROID		20									//��ӻ���
#define SUB_AS_C_MODIFY_ANDROID		21									//�޸Ļ���
#define SUB_AS_C_DELETE_ANDROID		22									//ɾ������

//��Ϊģ��
#define SUB_AS_C_APPEND_ATEMPLATE	30									//���ģ��
#define SUB_AS_C_MODIFY_ATEMPLATE	31									//�޸�ģ��
#define SUB_AS_C_DELETE_ATEMPLATE	32									//ɾ��ģ��

//����ģ��
#define SUB_AS_C_APPEND_STEMPLATE	40									//���ģ��
#define SUB_AS_C_MODIFY_STEMPLATE	41									//�޸�ģ��
#define SUB_AS_C_DELETE_STEMPLATE	42									//ɾ��ģ��

//////////////////////////////////////////////////////////////////////////////////

//��½���
#define SUB_AS_S_LOGON_SUCCESS		100									//��¼�ɹ�
#define SUB_AS_S_LOGON_FAILURE		101									//��¼ʧ��
#define SUB_AS_S_LOGON_FINISH		102									//��¼���

//������Ϣ
#define SUB_AS_S_GROUP_ITEM			110									//Ⱥ������
#define SUB_AS_S_BATCH_ITEM			111									//��������
#define SUB_AS_S_ATEMPLATE_ITEM		112									//ģ������
#define SUB_AS_S_STEMPLATE_ITEM		113									//ģ������
#define SUB_AS_S_GROUP_MEMBER_ITEM	114									//��Ա����
#define SUB_AS_S_BATCH_MEMBER_ITEM	115									//��Ա����

//�����б�
#define SUB_AS_S_KIND_LIST			120									//�����б�
#define SUB_AS_S_GROUP_LIST			121									//����б�
#define SUB_AS_S_BATCH_LIST			122									//��������
#define SUB_AS_S_ATEMPLATE_LIST		124									//��Ϊģ��
#define SUB_AS_S_STEMPLATE_LIST		125									//����ģ��	
#define SUB_AS_S_GROUP_MEMBER_LIST	126									//Ⱥ���Ա
#define SUB_AS_S_BATCH_MEMBER_LIST	127									//���γ�Ա

//ɾ��֪ͨ
#define SUB_AS_S_BATCH_DELETE		130									//����ɾ��
#define SUB_AS_S_ATEMPLATE_DELETE	131									//ģ��ɾ��
#define SUB_AS_S_STEMPLATE_DELETE	132									//ģ��ɾ��
#define SUB_AS_S_BATCH_MEMBER_DELETE 133								//��Աɾ��
#define SUB_AS_S_GROUP_MEMBER_DELETE 134								//��Աɾ��

//�������
#define SUB_AS_S_OPERATE_SUCCESS	200									//�����ɹ�
#define SUB_AS_S_OPERATE_FAILURE	201									//����ʧ��

//////////////////////////////////////////////////////////////////////////////////
//�����޸�����

#define BATCH_MKIND_MODIFY_CONFIG	1									//�޸�����
#define BATCH_MKIND_APPEND_MEMBER	2									//��ӳ�Ա
#define BATCH_MKIND_MODIFY_MEMBER	3									//�޸ĳ�Ա
#define BATCH_MKIND_DELETE_MEMBER	4									//ɾ����Ա

//�޸�����
struct tagBatchModifyConfig
{
	DWORD							dwBatchID;							//���α�ʶ
	DWORD							dwEnterTime;						//����ʱ��
	DWORD							dwLeaveTime;						//�뿪ʱ��	
};

//��ӻ���
struct tagBatchAppendMember
{
	DWORD							dwBatchID;							//���α�ʶ
	DWORD							dwUserID;							//������ʶ	
	DWORD							dwEnterTime;						//����ʱ��
	DWORD							dwLeaveTime;						//�뿪ʱ��
	WORD							wPlayKindID[3];						//��������
	DWORD							dwSupplyTemplateID;					//����ģ��
	DWORD							dwActionTemplateID[3];				//��Ϊģ��	
};

//�޸Ļ���
struct tagBatchModifyMember
{
	DWORD							dwBatchID;							//���α�ʶ
	DWORD							dwUserID;							//������ʶ	
	DWORD							dwEnterTime;						//����ʱ��
	DWORD							dwLeaveTime;						//�뿪ʱ��
	WORD							wPlayKindID[3];						//��������
	DWORD							dwSupplyTemplateID;					//����ģ��
	DWORD							dwActionTemplateID[3];				//��Ϊģ��	
};

//ɾ������
struct tagBatchDeleteMember
{
	DWORD							dwBatchID;							//���α�ʶ
	DWORD							dwUserID;							//������ʶ		
};

//////////////////////////////////////////////////////////////////////////////////

//��½����
struct CMD_AS_C_LogonService
{	
	//�˺���Ϣ
	WCHAR							szAccounts[LEN_ACCOUNTS];			//�û��˺�
	WCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
};

//����Ⱥ��
struct CMD_AS_C_SearchGroup
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
};

//Ⱥ������
struct CMD_AS_C_LoadGroupBatch
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
};

//����Ⱥ���Ա
struct CMD_AS_C_LoadGroupMember
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
};

//�������γ�Ա
struct CMD_AS_C_LoadBatchMember
{
	DWORD							dwBatchID;							//���α�ʶ
};

//�������
struct CMD_AS_C_AppendBatch
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwEnterTime;						//����ʱ��
	DWORD							dwLeaveTime;						//�뿪ʱ��
};

//�޸�����
struct CMD_AS_C_ModifyBatch
{
	DWORD							dwBatchID;							//���α�ʶ
	BYTE							cbModifyKind;						//�޸�����
};

//ɾ������
struct CMD_AS_C_DeleteBatch
{
	DWORD							dwBatchID;							//Ⱥ���ʶ
};

//��ӻ���
struct CMD_AS_C_AppendAndroid
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	BYTE							cbMemberType;						//��Ա����
	DWORD							dwParentGameID;						//�ϼ���ʶ
	WORD							wAndroidCount;						//��������
};

//�޸Ļ���
struct CMD_AS_C_ModifyAndroid
{
	//�˺���Ϣ
	DWORD							dwUserID;							//������ʶ	
	DWORD							dwGroupID;							//Ⱥ���ʶ
	BYTE							cbGender;							//�û��Ա�	
};

//�޸Ļ���
struct CMD_AS_C_DeleteAndroid
{
	DWORD							dwUserID;							//������ʶ
	DWORD							dwGroupID;							//Ⱥ���ʶ			
};

//ɾ��ģ��
struct  CMD_AS_C_DeleteATemplate
{
	DWORD							dwTemplateID;						//ģ���ʶ
};

//ɾ��ģ��
struct  CMD_AS_C_DeleteSTemplate
{
	DWORD							dwTemplateID;						//ģ���ʶ
};

//��½�ɹ�
struct CMD_AS_S_LogonSuccess
{	
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	DWORD							dwCustomID;							//�Զ�ͷ��
	DWORD							dwUserID;							//�û� I D
	WCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	WCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	WCHAR							szLogonPass[LEN_PASSWORD];			//��¼����	
};

//��¼ʧ��
struct CMD_AS_S_LogonFailure
{
	LONG							lErrorCode;							//�������
	WCHAR							szErrorDescribe[128];				//������Ϣ
};

//����ɾ��
struct CMD_AS_S_BatchDelete
{
	DWORD							dwBatchID;							//���α�ʶ
};

//ģ��ɾ��
struct CMD_AS_S_ATemplateDelete
{
	DWORD							dwTemplateID;						//ģ���ʶ
};

//ģ��ɾ��
struct CMD_AS_S_STemplateDelete
{
	DWORD							dwTemplateID;						//ģ���ʶ
};

//��Աɾ��
struct CMD_AS_S_BatchMemberDelete
{
	DWORD							dwBatchID;							//���α�ʶ
	DWORD							dwUserID;							//�û���ʶ
};

//��Աɾ��
struct CMD_AS_S_GroupMemberDelete
{
	DWORD							dwGroupID;							//Ⱥ���ʶ
	DWORD							dwUserID;							//�û���ʶ
};

//�����ɹ�
struct CMD_AS_S_OperateSuccess
{
	LONG							lOperateCode;						//��������
	WCHAR							szOperateNotice[128];				//������ʾ
};

//����ʧ��
struct CMD_AS_S_OperateFailure
{
	LONG							lErrorCode;							//�������
	WCHAR							szErrorDescribe[128];				//������Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
#pragma pack()

#endif