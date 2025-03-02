#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

//�����ļ�
#include "ClubPlazaHead.h"
#include "ClubUserManager.h"

//////////////////////////////////////////////////////////////////////////
//���ݶ���

//�󶨲���
struct tagBindParameter
{
	//��������
	DWORD							dwSocketID;							//�����ʶ
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwActiveTime;						//����ʱ��

	//��������
	WORD							wServiceID;							//�����ʶ
	WORD							wServiceModule;						//����ģ��
};

//////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//״̬����
protected:
	bool							m_bLogicSevice;						//�����ʶ
	bool							m_bNeedConnStatus;					//���ӱ�־

	//���ݻ���
protected:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//��������

	//��������
protected:
	tagClubOption					m_ClubOption;						//��������
	tagServiceOption *				m_pLogicOption;						//��������	
	CInitParameter *				m_pInitParameter;					//���ò���
	tagBindParameter *				m_pBindParameter;					//��������

	//�������
protected:	
	CWordFilter						m_WordFilter;						//�ʻ����	
	CClubUserManager				m_ClubUserManager;					//�û�����
	CGlobalItemManager				m_GlobalItemManager;				//ȫ�ֹ���			

	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IDataBaseEngine *				m_pIDataBaseEngine;					//��������
	ITCPSocketService *				m_pITCPSocketCenter;				//�������
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������			

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
	virtual bool OnEventControl(WORD wIdentifier, VOID * pData, WORD wDataSize);
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

	//���Ӵ���
protected:
	//ע���¼�
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�û�����
	bool OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//�б��¼�
	bool OnTCPSocketMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize);		
	//�������
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�߼�ת��
	bool OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//�����¼�
protected:
	//ע�����
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����״̬
	bool OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�߼�����
	bool OnTCPNetworkMainClubPlazaService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//�����¼�
protected:	
	//��¼�ɹ�
	bool OnDBLogonSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��¼ʧ��
	bool OnDBLogonFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//��¼���
	bool OnDBLogonFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//����Ⱥ��
	bool OnDBCreateClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//����Ⱥ��
	bool OnDBUpdateClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//ɾ��Ⱥ��
	bool OnDBDeleteClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//����Ⱥ��
	bool OnDBSearchClub(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//��ӳ�Ա
	bool OnDBInsertMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//���³�Ա
	bool OnDBUpdateMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//ɾ����Ա
	bool OnDBDeleteMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//������Ƭ
	bool OnDBClubNameCard(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//������Ϣ
	bool OnDBApplyMessage(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//�޸Ļ���
	bool OnDBModifyScore(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//Ⱥ������
	bool OnDBClubOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//�������
	bool OnDBOptionFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��ֹ�ʻ�
	bool OnDBForbidWords(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//�Ƹ�����
	bool OnDBWealthUpdate(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//ɾ�����
	bool OnDBApplyDelete(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//�������
	bool OnDBDetectClubName(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�����ɹ�
	bool OnDBOperateSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//����ʧ��
	bool OnDBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);

	//��������
protected:	
	//��ȡ�ṹ
	tagForwardHead* GetForwardHead();	
	//��������
	bool SendUIControlPacket(WORD wRequestID, VOID* pData, WORD wDataSize);
	//���Ʋ���
	tagTokenParameter* GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID);

	//���ݷ���
protected:
	//���ط���
	bool SendDataToGate(WORD wGateID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenID);
};

//////////////////////////////////////////////////////////////////////////

#endif