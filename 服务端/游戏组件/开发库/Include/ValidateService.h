#ifndef VALIDATE_SERVICE_HEAD_FILE
#define VALIDATE_SERVICE_HEAD_FILE

#pragma once

#include "ServiceFrameHead.h"

//////////////////////////////////////////////////////////////////////////

//��˵��
class CValidateService;

//////////////////////////////////////////////////////////////////////////

//��֤ģ��
class CValidateServiceSink : public ITCPSocketEvent
{
	//��Ԫ����
	friend class CValidateService;

	//�������
protected:
	CValidateService *				m_pValidateService;					//��֤����

	//��������
protected:
	//���캯��
	CValidateServiceSink();
	//��������
	virtual ~CValidateServiceSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//����ӿ�
protected:
	//�����¼�
	virtual bool OnEventTCPSocketLink(DWORD wServiceID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(DWORD wServiceID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(DWORD wServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount);
};

//////////////////////////////////////////////////////////////////////////

//��֤����
class SERVICE_FRAME_CLASS CValidateService
{
	//��Ԫ����
	friend class CValidateServiceSink;

	//״̬����
protected:
	bool							m_bValidate;						//��֤��־
	HANDLE							m_hResultEvent;						//�¼����

	//��ȡ��Ϣ
protected:
	tagAddressInfo					m_CorrespondAddress;				//Э����ַ
	tagDataBaseParameter			m_PlatformDBParameter;				//������Ϣ
	tagDataBaseParameter			m_AccountsDBParameter;				//������Ϣ
	tagDataBaseParameter			m_GameMatchDBParameter;				//������Ϣ
	tagDataBaseParameter			m_GameBattleDBParameter;			//������Ϣ	

	//�������
protected:
	CValidateServiceSink			m_ValidateServiceSink;				//��֤ģ��
	CTCPSocketServiceHelper			m_TCPSocketServiceModule;			//�������

	//��������
public:
	//���캯��
	CValidateService();
	//��������
	virtual ~CValidateService();

	//���ܺ���
public:	 
	//������Ϣ
	bool GetMatchDBParameter(tagDataBaseParameter & DataBaseParameter);
	//������Ϣ
	bool GetBattleDBParameter(tagDataBaseParameter & DataBaseParameter);
	//������Ϣ
	bool GetPlatformDBParameter(tagDataBaseParameter & DataBaseParameter);
	//������Ϣ
	bool GetAccountsDBParameter(tagDataBaseParameter & DataBaseParameter);

	//�ڲ�����
protected:
	//ִ����֤
	bool PerformValidate();
};

//////////////////////////////////////////////////////////////////////////

#endif