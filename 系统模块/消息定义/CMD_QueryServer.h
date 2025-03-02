#ifndef CMD_SYNC_SERVER_HEAD_FILE
#define CMD_SYNC_SERVER_HEAD_FILE

#pragma pack(1)


//////////////////////////////////////////////////////////////////////////
//�����

//��Ϣ����
#define MDM_AQ_QUERY_SERVICE			10									//��ѯ����

//////////////////////////////////////////////////////////////////////////

//��������
#define SUB_AQ_C_SYNC_USERINFO			10									//ͬ����Ϣ
#define SUB_AQ_C_QUERY_USERINFO			11									//��ѯ��Ϣ
#define SUB_AQ_C_QUERY_BLACKLIST		12									//��ѯ������
#define SUB_AQ_C_QUERY_FRIENDLIST		13									//�����б�
#define SUB_AQ_C_QUERY_FRIENDINFO		14									//������Ϣ
#define SUB_AQ_C_QUERY_FRIENDEXTRAS		15									//������Ϣ

//��������
#define SUB_AQ_C_BUILD_ACCESSTOKEN		20									//��������

//Ӧ������
#define SUB_AQ_S_USERINFO_LIST			100									//��Ϣ�б�
#define SUB_AQ_S_FRIENDINFO_LIST		101									//�����б�
#define SUB_AQ_S_FRIENDEXTRAS_LIST		102									//�����б�

//Ӧ������
#define SUB_AQ_S_SYNC_USERINFO_RESULT	110									//ͬ�����
#define SUB_AQ_S_QUERY_USERINFO_RESULT	111									//��ѯ���
#define SUB_AQ_S_QUERY_BLACKLIST_RESULT 112									//��ѯ���
#define SUB_AQ_S_QUERY_FRIENDLIST_RESULT 113								//��ѯ���
#define SUB_AQ_S_QUERY_FRIENDINFO_RESULT 114								//��ѯ���
#define SUB_AQ_S_QUERY_FRIENDEXTRAS_RESULT 115								//��ѯ���

//Ӧ������
#define SUB_AQ_S_BUILD_ACCESSTOKEN_RESULT 120									//���ƽ��	

/////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif