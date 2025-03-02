#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//���ò���
class CInitParameter
{
	//ϵͳ����
public:	
	WORD							m_wConnectTime;						//����ʱ��
	WORD							m_wConnectStore;					//����ʱ��
	WORD							m_wConnectService;					//����ʱ��
	WORD							m_wConnectMonitor;					//����ʱ��

	//������Ϣ
public:
	TCHAR							m_szLoggerPath[MAX_PATH];			//��־·��

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
	//��־����
	VOID LoadLoggerParameter();
};

//////////////////////////////////////////////////////////////////////////

#endif
