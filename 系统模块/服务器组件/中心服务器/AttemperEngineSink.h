#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "StockDataManager.h"
#include "GlobalInfoManager.h"
#include "ServiceAttribManager.h"

//////////////////////////////////////////////////////////////////////////
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

//�ֵ䶨��
typedef CMap<WORD, WORD, WORD, WORD> CMapPortOnline;

//////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//״̬����
protected:
	bool							m_bWillConcludeService;				//Ԥֹͣ״̬
	WORD							m_wCollectItem;						//��������	
	DWORD							m_dwCollectItem;					//��������
	CWordArray						m_StockUpdateArray;					//��������
	CWordArray						m_WaitCollectItemArray;				//���ܵȴ�

	//��������
protected:
	CInitParameter *				m_pInitParameter;					//���ò���
	tagBindParameter *				m_pBindParameter;					//��������

	//�������
protected:
	CGlobalInfoManager				m_GlobalInfoManager;				//ȫ�ֹ���	
	CServiceAttribManager			m_ServiceAttribManager;				//�������

	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IDataBaseEngine *				m_pIDataBaseEngine;					//��������
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

	//�����¼�
protected:
	//ע�����
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����״̬
	bool OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�û�����
	bool OnTCPNetworkMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��Ϣ��ѯ
	bool OnTCPNetworkMainQueryService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�㲥��Ϣ
	bool OnTCPNetworkMainBroadcast(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//��վ֪ͨ
	bool OnTCPNetworkMasterService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//�������
	bool OnTCPNetWorkMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//ת������
	bool OnTCPNetworkMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID);

	//�¼�����
protected:
	//��ѯ�û�
	bool OnTCPNetwordSubQueryByGameID(VOID *pData, WORD wDataSize, DWORD dwSocketID);
	//��ѯ�û�
	bool OnTCPNetwordSubQueryByAccounts(VOID *pData, WORD wDataSize, DWORD dwSocketID);

	//���ݿ��¼�
protected:
	//�������
	bool OnDBConfigFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�������
	bool OnDBUnlockPlayer(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//���°�
	bool OnDBUpdateRankingList(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);

	//��������
protected:
	//���Ͳʳ�
	bool SendGameJackpot(DWORD dwSocketID);
	//���ͷ���
	bool SendServiceAttrib(DWORD dwSocketID);	
	//�����б�
	bool SendServerListItem(DWORD dwSocketID);		
	//���ͷ���
	bool SendServiceListItem(WORD wServiceModule,DWORD dwSocketID, bool bPushRouteTable);
	//���ͷ���
	bool PushServiceListItem(WORD wServiceMask, DWORD dwSocketID, bool bPushRouteTable);
	//��������
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);

	//���ݷ���
protected:
	//�û�����
	bool SendDataToUser(WORD wServerID, DWORD dwUserID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//���ݷ���
protected:	
	//��������
	bool SendDataToService(WORD wServiceModule, WORD wServiceID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	bool SendDataToService(WORD wServiceModule, tagForwardItem& ForwardItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);		
	//��������
	bool SendDataToAllService(WORD wServiceModule, WORD wExcludeID, WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//��������
protected:
	//���´���
	VOID UploadAgentList();
	//��������
	VOID UpdateGateList();

	//�������
protected:
	//���·���
	VOID UpdateServerItem(CGlobalServerInfo* pGlobalServerInfo, BYTE cbUpdateKind);
	//��ʽ������
	VOID FormatServerItem(CGlobalServerInfo* pGlobalServerInfo, CStringW& strServerItem);

};

//////////////////////////////////////////////////////////////////////////

#endif