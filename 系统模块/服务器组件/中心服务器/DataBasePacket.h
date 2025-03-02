#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//ϵͳ����
#define DBR_GP_LOAD_CONFIG			1									//��������	
#define DBR_GP_UNLOCK_PLAYER		3									//�������
#define DBR_GP_REQUEST_WEBPAGE		4									//������ҳ
#define DBR_GP_UPDATE_GATE_LIST		8									//��������
#define DBR_GP_ONLINE_COUNT_INFO	9									//������Ϣ
#define DBR_GP_UPDATE_SERVER_LIST	10									//���·���
#define DBR_GP_UPDATE_RANKING_LIST  11									//��������

//////////////////////////////////////////////////////////////////////////////////
//��������

#define UPDATE_KIND_MODIFY			1									//�޸����� 
#define UPDATE_KIND_DELETE			2									//ɾ������

//////////////////////////////////////////////////////////////////////////

//�������
struct DBR_GP_UnlockPlayer
{
	DWORD							dwUserID;							//�û���ʶ
};

//��������
struct DBR_UpdateGateList
{
	CHAR							szGateList[8192];					//�����б�
};

//������ҳ
struct DBR_GP_RequestWebPage
{
	CHAR							szWebUrl[128];						//��ҳ����	
	CHAR							szPostData[2048];					//Ͷ������
};

//�ϱ�����
struct DBR_GP_UpdateServerList
{
	BYTE							cbUpdateKind;						//��������	
	WCHAR							szServerList[4096];					//�����б�
};

//////////////////////////////////////////////////////////////////////////
//ϵͳ����
#define DBO_GP_CONFIG_FINISH		101									//�������
#define DBO_GP_UNLOCK_PLAYER		103									//�������
#define DBO_GP_UPDATA_RANKING_LIST	106									//���°�

//////////////////////////////////////////////////////////////////////////

//�������
struct DBO_GP_UnlockPlayer
{
	DWORD							dwUserID;							//�û���ʶ
};

//////////////////////////////////////////////////////////////////////////

#endif