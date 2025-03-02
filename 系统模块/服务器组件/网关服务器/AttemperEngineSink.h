#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "InitParameter.h"
#include "GlobalInfoManager.h"

//////////////////////////////////////////////////////////////////////////
//����״̬
enum emLinkState
{
	LS_NULL=0,					//����״̬
	LS_SHAKEHAND,				//����״̬
	LS_LINKING					//����״̬
};

//���Ӽ�¼
struct tagLinkRecord
{
	WORD							wTimes;								//���Ӵ���
	WORD							wServiceID[5];						//�����ʶ
	WORD							wReqServiceID;						//�����ʶ
	BYTE							cbLinkExtend[LEN_LINK_EXTEND];		//��չ��Ϣ
};


//�󶨲���
struct tagBindParameter
{	
	//������Ϣ	
	DWORD							dwSocketID;							//�����ʶ	
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwActiveTime;						//����ʱ��

	//������Ϣ	
	WORD							wServiceID[SERVICE_COUNT];			//�����ʶ
	emLinkState						LinkState[SERVICE_COUNT];			//����״̬	
	tagLinkRecord					LinkRecord[SERVICE_COUNT];			//���Ӽ�¼

	//�û���Ϣ	
	DWORD							dwUserID;							//�û���ʶ
	TCHAR							szLogonToken[LEN_LOGON_TOKEN];		//��¼����
};

//////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//״̬����
protected:
	bool							m_bNeedBattle;						//��֤Լս

	//�������
protected:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//��������

	//���ñ���
protected:	
	tagServiceOption *				m_pServiceOption;					//��������
	CInitParameter *				m_pInitParameter;					//���ò���
	tagBindParameter *				m_pBindParameter;					//������Ϣ
	tagServiceAttrib				m_ServiceAttribute[SERVICE_COUNT];	//�������

	//�������
protected:
	CGlobalInfoManager				m_GlobalInfoManager;				//�������

	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IUnknownEx*						m_pIAttemperEngine;					//��������	
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
	ITCPSocketService *				m_TCPSocketCenter;					//���ķ���

	//��������
public:
	//���캯��
	CAttemperEngineSink();
	//��������
	virtual ~CAttemperEngineSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { return; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//�첽�ӿ�
public:
	//�����¼�
	virtual bool OnAttemperEngineStart(IUnknownEx * pIUnknownEx);
	//ֹͣ�¼�
	virtual bool OnAttemperEngineStop(IUnknownEx * pIUnknownEx);

	//�¼��ӿ�
public:
	//�����¼�
	virtual bool OnEventControl(WORD dwIdentifier, VOID * pData, WORD wDataSize);
	//�Զ��¼�
	virtual bool OnEventAttemperData(WORD wRequestID, VOID * pData, WORD wDataSize);

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM wBindParam);
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount);

	//�����¼�
public:
	//Ӧ���¼�
	virtual bool OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID);
	//�ر��¼�
	virtual bool OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID);
	//��ȡ�¼�
	virtual bool OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//��������
protected:
	//ע�����
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�б��¼�
	bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);		

	//�߼�����
protected:
	//ע�����
	bool OnTCPSocketMainRegister(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize);
	//ͨ�÷���
	bool OnTCPSocketMainCommonService(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount);
	//�ڲ�����
	bool OnTCPSocketMainInsideService(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount);

	//�����¼�
protected:	
	//���ط���
	bool OnTCPNetworkMainGateService(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�ڲ�����
	bool OnTCPNetworkMainInsideService(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);	

	//���ݷ���
protected:
	//������Ϣ
	bool SendDataToService(WORD wServiceIndex, WORD wServiceID, DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//�������
protected:
	//�������
	WORD AssignInsideService(WORD wServiceModule, DWORD dwSocketID);

	//��������
protected:	
	//ע������
	VOID LogoutToken(WORD wServiceIndex, WORD wServiceID, DWORD dwTokenID);
	//�ر�����
	void ShutdownServiceLink(WORD wServiceModule, WORD wServiceID);
	//�ر�����
	void ShutdownServiceLink(WORD wServiceModule, WORD wServiceID, DWORD dwSoceketID);

	//��������
protected:
	//ʱ�ӱ�ʶ
	inline DWORD GetTimerID(WORD wServiceIndex, WORD wServiceID);
	//�����ʶ
	inline DWORD GetServiceID(WORD wServiceIndex, WORD wServiceID);

	//��������
protected:	
	//��������
	WORD GetOnlineCount();
	//�󶨲���
	tagBindParameter * GetBindParameter(WORD wBindIndex);		
	//��������
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);
};

//////////////////////////////////////////////////////////////////////////

#endif

