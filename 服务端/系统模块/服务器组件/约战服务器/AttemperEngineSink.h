#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "BattleManager.h"
#include "GlobalInfoManager.h"
#include "BattleServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//���ݶ���

//��������
enum enServiceKind
{
	ServiceKind_None,				//��Ч����
	ServiceKind_Game,				//��Ϸ����	
	ServiceKind_Gate,				//���ط���
};

//ȫ��ѡ��
struct tagBattleGlobalOption
{
	WORD							wClubMaxTableCount;					//�������
	WORD							wNormalMaxTableCount;				//�������	
};

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

//���鶨��
typedef CWHArray<tagBattleGameOptionEx> CBattleGameOptionArray;			//ѡ������
typedef CWHArray<tagBattleBaseOptionEx> CBattleBaseOptionArray;			//ѡ������
typedef CWHArray<tagBattleGlobalOption> CBattleGlobalOptionArray;		//ѡ������

//////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//״̬����
protected:
	bool							m_bBattleService;					//�����ʶ
	CWordArray						m_SynchroItemArray;					//ͬ������

	//�������
protected:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//���ݻ���

	//��������
protected:
	tagServiceOption *				m_pServiceOption;					//Լս����
	CInitParameter *				m_pInitParameter;					//���ò���
	tagBindParameter *				m_pBindParameter;					//��������
	tagBattleGlobalOption			m_GlobalOption;						//ȫ������

	//�洢����
protected:
	CBattleGameOptionArray			m_BattleGameOptionActive;			//�Զ�ѡ��		
	CBattleGameOptionArray			m_BattleGameOptionBuffer;			//�Զ�ѡ��		
	CBattleBaseOptionArray			m_BattleBaseOptionActive;			//����ѡ��		
	CBattleBaseOptionArray			m_BattleBaseOptionBuffer;			//����ѡ��		

	//�������
protected:
	CBattleManager					m_BattleManager;					//Լս����
	CGlobalInfoManager				m_GlobalInfoManager;				//ȫ�ֹ���	

	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IDataBaseEngine *				m_pIDataBaseEngine;					//��������
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
	ITCPSocketService *				m_pITCPSocketCenter;				//�������	
	ITCPSocketService *				m_pITCPSocketBattle;				//�������	

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
	virtual bool OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize,DWORD * pdwTokenID,WORD wTokenCount);

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
	//������Ϣ
	bool OnTCPSocketMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//�������
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//ת����Ϣ
	bool OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//�����¼�
protected:
	//ע�����
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����״̬
	bool OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//Լս����
	bool OnTCPNetWorkMainGBBattleService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//Լս����
	bool OnTCPNetWorkMainBSBattleService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//Լս����
	bool OnTCPNetWorkMainBCBattleService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	

	//���ݿ��¼�
protected:	
	//����ʧ��
	bool OnDBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//Լսѡ��
	bool OnDBBattleBaseOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��Ϸѡ��
	bool OnDBBattleGameOption(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//ȫ��ѡ��
	bool OnDBBattleGlobalOption(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//ѡ�����
	bool OnDBBattleOptionFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);			

	//Լս�淨
protected:		
	//У���淨
	bool VerifyBattleConfig(tagBattleConfig * pBattleConfig);
	//У������
	bool VerifyBattleConfig(tagBattleConfig * pBattleConfig, tagBattleConfigEx & BattleConfig);	

	//��������
protected:
	//���ͬ��
	VOID AppendSynchroItem(WORD wServerID);
	//�Ƴ�ͬ��
	VOID RemoveSynchroItem(WORD wServerID);
	// ��������
	bool VerifyTableCount(DWORD dwClubID, WORD wTableCount);
	//��������
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);		
	//���Ʋ���
	tagTokenParameter* GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID);

	//�������
protected:
	//��������
	VOID ConstructTableItem(tagBattleTableItemEx * pTableItemEx, tagBattleTableItem * pTableItem);
	//��Ϸѡ��
	WORD ConstructGameOption(INT& nStartIndex, WORD wModuleID, BYTE cbEmployScene, BYTE cbBuffer[], WORD wBufferSize);
	//Լսѡ��
	WORD ConstructBaseOption(INT& nStartIndex, WORD wModuleID, BYTE cbEmployScene, BYTE cbBuffer[], WORD wBufferSize);
	//��������
	WORD ConstructTableList(CTableItemArray& TableItemArray, CTableParamArray& TableParamArray, INT &nStartIndex, BYTE cbBuffer[], WORD wBufferSize);

	//�������
protected:
	//����ʧ��
	VOID SendOperateFailure(WORD wServiceID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe);
	//����ʧ��
	VOID SendOperateFailure(WORD wServiceID, DWORD dwClubID, tagSocketInfo& SocketInfo, LONG lErrorCode, LPCTSTR pszDescribe);
	//�����ɹ�
	VOID SendOperateSuccess(WORD wServiceID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe);
	//�����ɹ�
	VOID SendOperateSuccess(WORD wServiceID, DWORD dwClubID, tagSocketInfo& SocketInfo, LONG lErrorCode, LPCTSTR pszDescribe);

	//���ݷ���
protected:
	//���䷢��
	bool SendDataToGame(WORD wServerID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��ݷ���
	bool SendDataToClub(WORD wServiceID, WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//���ط���
	bool SendDataToGate(WORD wServiceID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD * pdwTokenID=NULL, WORD wTokenCount=0);
};

//////////////////////////////////////////////////////////////////////////

#endif