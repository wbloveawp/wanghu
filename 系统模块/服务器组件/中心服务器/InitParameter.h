#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once


//////////////////////////////////////////////////////////////////////////

//���ò���
class CInitParameter
{
	//ϵͳ����
public:
	WORD							m_wMaxConnect;						//�������
	WORD							m_wServicePort;						//����˿�
	WORD							m_wConnectTime;						//����ʱ��
	WORD							m_wCollectTime;						//ͳ��ʱ��
	WORD							m_wUploadTime;						//�ϴ�ʱ��
	WORD							m_wUpdateTime;						//����ʱ��

	//������Ϣ
public:
	TCHAR							m_szUploadDomain[128];				//�ϴ�����	

	//������Ϣ
public:
	tagDataBaseParameter			m_AccountsDBParameter;				//������Ϣ
	tagDataBaseParameter			m_PlatformDBParameter;				//������Ϣ

	//������Ϣ
public:
	tagRedisConnOption				m_RedisConnOption;					//����ѡ��

	//��������
public:
	//���캯��
	CInitParameter();
	//��������
	virtual ~CInitParameter();

	//���ܺ���
public:
	//��ʼ��
	VOID InitParameter();
	//��������
	VOID LoadInitParameter();
	//��������
	VOID LoadRedisParameter();
};

//////////////////////////////////////////////////////////////////////////

#endif
