#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once


//////////////////////////////////////////////////////////////////////////

//���ò���
class CInitParameter
{
	//ϵͳ����
public:
	WORD							m_wConnectTime;						//����ʱ��

	//������Ϣ
public:
	WORD							m_wCenterPort;						//���Ķ˿�
	tagAddressInfo					m_CenterAddress;					//���ĵ�ַ

	//������Ϣ
public:
	tagDataBaseParameter			m_AccountsDBParameter;				//������Ϣ
	tagDataBaseParameter			m_PlatformDBParameter;				//������Ϣ	

	//������Ϣ
public:
	tagRedisConnOption				m_UserRConnOption;					//�û�����
	tagRedisConnOption				m_UserWConnOption;					//�û�����
	tagRedisConnOption				m_FriendConnOption;					//��������

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
