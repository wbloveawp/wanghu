#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//���ò���
class CInitParameter
{
	//���ʱ��
public:
	WORD							m_wConnectTime;						//����ʱ��
	WORD							m_wCollectTime;						//ͳ��ʱ��
	WORD							m_wLoadListTime;					//�б�ʱ��
	WORD							m_wReLoadListTime;					//�б�ʱ��

	//�ͻ�ʱ��
public:
	WORD							m_wIntermitTime;					//�ж�ʱ��
	WORD							m_wOnLineCountTime;					//����ʱ��

	//������Ϣ
public:
	TCHAR							m_szServerName[LEN_SERVER];			//��������

	//������Ϣ
public:
	tagRedisConnOption				m_UserConnOption;					//�û�����	
	tagRedisConnOption				m_TokenConnOption;					//��������	
	tagRedisConnOption				m_FriendConnOption;					//��������
	tagRedisConnOption				m_MasterConnOption;					//��������

	//������Ϣ
public:
	WORD							m_wCenterPort;						//���Ķ˿�
	tagAddressInfo					m_CenterAddress;					//���ĵ�ַ
	tagAddressInfo					m_ServiceAddress;					//�����ַ	
	tagDataBaseParameter			m_PlatformDBParameter;				//������Ϣ
	tagDataBaseParameter			m_AccountsDBParameter;				//������Ϣ

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
