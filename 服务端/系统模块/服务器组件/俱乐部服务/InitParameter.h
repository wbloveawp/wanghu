#ifndef INI_PARAMETER_HEAD_FILE
#define INI_PARAMETER_HEAD_FILE

#pragma once


//////////////////////////////////////////////////////////////////////////

//���ò���
class CInitParameter
{
	//ϵͳ����
public:
	WORD							m_wConnectCenter;					//����ʱ��		
	WORD							m_wConnectBattle;					//����ʱ��		

	//��������
public:	
	WORD							m_wCenterPort;						//���Ķ˿�
	tagAddressInfo					m_CenterAddress;					//���ĵ�ַ				

	//��������
public:
	tagDataBaseParameter			m_GameBattleDBParameter;			//��������

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
