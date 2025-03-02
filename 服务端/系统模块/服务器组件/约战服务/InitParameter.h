#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

#include "MatchServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//���ò���
class CInitParameter
{
	//ϵͳ����
public:
	WORD							m_wMaxConnect;						//�������

	//���ʱ��
public:
	WORD							m_wConnectTime;						//����ʱ��

	//������Ϣ
public:
	BYTE							m_cbNetworkKind;					//��������
	TCHAR							m_szValidate[LEN_VALIDATE];			//��֤��ַ
	TCHAR							m_szAccreditID[LEN_ACCREDIT_ID];	//��Ȩ����

	//������Ϣ
public:
	tagAddressInfo					m_ServiceAddress;					//�����ַ
	tagAddressInfo					m_CorrespondAddress;				//Э����ַ
	tagDataBaseParameter			m_PlatformDBParameter;				//������Ϣ

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
