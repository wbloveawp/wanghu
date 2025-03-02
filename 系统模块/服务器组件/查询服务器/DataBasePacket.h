#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//��������
#define DBR_GR_SYNC_USERINFO			10									//ͬ����Ϣ
#define DBR_GR_QUERY_USERINFO			11									//��ѯ��Ϣ
#define DBR_GR_QUERY_BLACKLIST			12									//��ѯ������
#define DBR_GR_QUERY_FRIENDLIST			13									//�����б�
#define DBR_GR_QUERY_FRIENDINFO			14									//������Ϣ
#define DBR_GR_QUERY_FRIENDEXTRAS		15									//������Ϣ

//��������
#define DBR_GR_BUILD_ACCESSTOKEN		20									//��������

//////////////////////////////////////////////////////////////////////////
//�����Ϣ

//Ӧ������
#define DBO_GR_USERINFO_LIST			100									//��Ϣ�б�
#define DBO_GR_FRIENDINFO_LIST			101									//�����б�
#define DBO_GR_FRIENDEXTRAS_LIST		102									//�����б�

//Ӧ������
#define DBO_GR_SYNC_USERINFO_RESULT		110									//ͬ�����
#define DBO_GR_QUERY_USERINFO_RESULT	111									//��ѯ���
#define DBO_GR_QUERY_BLACKLIST_RESULT	112									//��ѯ���
#define DBO_GR_QUERY_FRIENDLIST_RESULT	113									//��ѯ���
#define DBO_GR_QUERY_FRIENDINFO_RESULT  114									//��ѯ���
#define DBO_GR_QUERY_FRIENDEXTRAS_RESULT 115								//��ѯ���

//Ӧ������
#define DBO_GR_BUILD_ACCESSTOKEN_RESULT	120									//���ƽ��

//////////////////////////////////////////////////////////////////////////////////

#endif