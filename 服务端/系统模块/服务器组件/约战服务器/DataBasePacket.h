#ifndef DATA_BASE_PACKET_HEAD_FILE
#define DATA_BASE_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//�������ݰ�

//ϵͳ����
#define DBR_GR_LOAD_BATTLE_OPTION		1								//Լսѡ��	


//////////////////////////////////////////////////////////////////////////
//�����Ϣ

//���ؽ��
#define DBO_GR_BATTLE_BASE_OPTION		100								//��Ϸѡ��
#define DBO_GR_BATTLE_GAME_OPTION		101								//����ѡ��
#define DBO_GR_BATTLE_GLOBAL_OPTION		102								//ȫ��ѡ��	
#define DBO_GR_BATTLE_OPTION_FINISH		110								//�������

//�����ṹ
#define DBO_GR_OPERATE_FAILURE			300								//����ʧ��

//////////////////////////////////////////////////////////////////////////////////

//����ʧ��
struct DBO_GR_OperateFailure
{
	//������Ϣ
	WORD							wRequestCmdID;						//��������

	//������Ϣ
	LONG							lErrorCode;							//ʧ�ܱ���
	WCHAR							szErrorMessage[128];				//������Ϣ	
};

//////////////////////////////////////////////////////////////////////////////////

#endif