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
	WORD							m_wCenterPort;						//���ĶϿ�
	tagAddressInfo					m_CenterAddress;					//���ĵ�ַ

	//��������
public:
	tagDataBaseParameter			m_PlatformDBParameter;				//������Ϣ	
	tagDataBaseParameter			m_GameBattleDBParameter;			//������Ϣ	

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
};

//////////////////////////////////////////////////////////////////////////

#endif
