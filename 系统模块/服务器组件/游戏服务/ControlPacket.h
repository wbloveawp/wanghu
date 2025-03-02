#ifndef CONTROL_PACKET_HEAD_FILE
#define CONTROL_PACKET_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//�¼����
#define ER_FAILURE					0									//ʧ�ܱ�־
#define ER_SUCCESS					1									//�ɹ���־

//////////////////////////////////////////////////////////////////////////
//�������ݰ�

#define UI_STORE_RESULT				1									//�洢���
#define UI_CENTER_RESULT			2									//���Ľ��
#define UI_SERVICE_RESULT			3									//������
#define UI_SERVICE_CONFIG_RESULT	5									//���ý��
#define UI_SERVICE_WILLSTOP_RESULT	6									//Ԥֹͣ���

//////////////////////////////////////////////////////////////////////////

//���ƽ��
struct CP_ControlResult
{
	BYTE							cbSuccess;							//�ɹ���־
};

//////////////////////////////////////////////////////////////////////////
//�������ݰ�

#define CT_CONNECT_STORE			100									//���Ӵ洢
#define CT_CONNECT_CENTER			101									//��������
#define CT_CONNECT_SERVICE			102									//����Լս
#define CT_DEBUG_SERVICE			103									//���Է���
#define CT_SERVICE_WILLSTOP			104									//����Ԥֹͣ
#define CT_LOAD_SERVICE_CONFIG		105									//��������
	

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

#endif