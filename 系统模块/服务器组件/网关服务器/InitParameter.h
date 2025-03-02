#ifndef INIR_PARAMETER_HEAD_FILE
#define INIR_PARAMETER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//���ò���
class CInitParameter
{
	//���ʱ��
public:
	WORD							m_wReportTime;						//�ϱ�ʱ��
	WORD							m_wConnectTime;						//����ʱ��

	//վ����Ϣ
public:
	WORD							m_wStationID;						//վ���ʶ
	TCHAR							m_szStationName[LEN_SERVER];		//վ������

	//��������
public:	
	WORD							m_wCenterPort;						//���Ķ˿�
	tagAddressInfo					m_CenterAddress;					//���ĵ�ַ	

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
