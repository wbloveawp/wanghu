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
	tagDataBaseParameter			m_AccountsDBParameter;				//������Ϣ
	tagDataBaseParameter			m_PlatformDBParameter;				//������Ϣ	
	TCHAR							m_szPaltformDomain[LEN_DOMAIN];		//ƽ̨����
	TCHAR							m_szLoggerPath[MAX_PATH];			//��־·��

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
