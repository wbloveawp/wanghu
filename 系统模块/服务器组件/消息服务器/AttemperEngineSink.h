#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "MessageServiceHead.h"

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

//////////////////////////////////////////////////////////////////////////////////
//���Ͷ���
typedef CWHStockData<DWORD, tagMarqueeNoticeInfo> CStockMarqueeNoticeInfo;

//////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//״̬����
protected:
	bool							m_bCollectUser;						//���ܱ�־

	//�������
protected:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//���ݻ���			

	//��������
protected:
	tagServiceOption *				m_pServiceOption;					//��������
	CInitParameter *				m_pInitParameter;					//���ò���
	tagBindParameter *				m_pBindParameter;					//��������	

	//�������
protected:
	CGlobalItemManager				m_GlobalItemManager;				//ȫ�ֹ���	
	CStockMarqueeNoticeInfo			m_StockMarqueeNoticeInfo;			//����ֿ�

	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IDataBaseEngine *				m_pIDataBaseEngine;					//��������
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
	ITCPSocketService*				m_pITCPSocketCenter;				//�������	

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
	//Ӧ���¼�
	virtual bool OnEventTCPNetworkBind(DWORD dwClientIP, DWORD dwSocketID);
	//�ر��¼�
	virtual bool OnEventTCPNetworkShut(DWORD dwClientIP, DWORD dwActiveTime, DWORD dwSocketID);
	//��ȡ�¼�
	virtual bool OnEventTCPNetworkRead(CMD_Command Command, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventTCPSocketLink(DWORD dwServiceID, INT nErrorCode);
	//�ر��¼�
	virtual bool OnEventTCPSocketShut(DWORD dwServiceID, BYTE cbShutReason);
	//��ȡ�¼�
	virtual bool OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD *pdwTokenID, WORD wTokenCount);

	//���Ӵ���
protected:
	//ע���¼�
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnTCPSocketMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//�������
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//ת������
	bool OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//���ܷ���
	bool OnTCPSocketMainCollectService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//�����¼�
protected:
	//ע�����
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����״̬
	bool OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��Ϣ����
	bool OnTCPNetWorkMainMessageService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID,DWORD dwTokenID);	

	//���ݿ��¼�
protected:
	//�ʼ���Ϣ
	bool OnDBUserMailInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�ʼ����
	bool OnDBUserMailFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�ʼ�����
	bool OnDBUserMailUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�����ʼ�
	bool OnDBUserMailInsert(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//��ȡ����
	bool OnDBClaimMailAttach(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);	
	//��Ʒ��Ϣ
	bool OnDBUserGoodsUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�Ƹ���Ϣ
	bool OnDBUserWealthUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�����ɹ�
	bool OnDBOperateSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//����ʧ��
	bool OnDBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);

	//��������
protected:
	//��������
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);

	//���ݷ���
protected:
	//��������
	bool SendBatchData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//���ݷ���
protected:
	//���ط���
	bool SendDataToGate(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD * pdwTokenID, WORD wTokenCount);

	//��������
private:
	//�������
	VOID CleanExpireNotice();
	//���Ʋ���
	tagTokenParameter* GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID);
	//������־
	bool InitLoggerService();

	//�������
protected:
	//����ʧ��
	VOID SendOperateFailure(CGlobalUserItem * pGLobalUserItem, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe);
	//�����ɹ�
	VOID SendOperateSuccess(CGlobalUserItem* pGLobalUserItem, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe, LPCTSTR pszGoodsList = NULL);
};

//////////////////////////////////////////////////////////////////////////

#endif