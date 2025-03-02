#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "ClubServiceHead.h"
#include "ClubInfoManager.h"
#include "ClubUserManager.h"
#include "GlobalServiceManager.h"

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
	PVOID							pBindServiceItem;					//�󶨷���
};

//////////////////////////////////////////////////////////////////////////

typedef CArray<DWORD, DWORD> CClubIDArray;
typedef CMap<DWORD,DWORD,ULONGLONG,ULONGLONG> CMapInvokeCount;		

//////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//״̬����
protected:
	bool							m_bClubSevice;						//�������
	bool							m_bNeedConnStatus;					//���ӱ�־

	//�������
protected:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//��������
	BYTE							m_cbDataBuffer[SOCKET_PACKET];		//��������

	//���Ա���
protected:
	CClubIDArray					m_ClubIDArray;						//��ʶ����
	CMapInvokeCount					m_MapInvokeCount1;					//����ͳ��	
	CMapInvokeCount					m_MapInvokeCount2;					//����ͳ��		

	//��������
protected:
	tagServiceItem					m_BattleItem;						//Լս����	
	tagServiceOption *				m_pClubOption;						//��������
	CInitParameter *				m_pInitParameter;					//���ò���
	tagBindParameter *				m_pBindParameter;					//��������

	//�������
protected:	
	CClubInfoManager				m_ClubInfoManager;					//�������	
	CClubUserManager				m_ClubUserManager;					//�û�����
	CGlobalItemManager				m_GlobalItemManager;				//ȫ�ֹ���
	CGlobalServiceManager			m_GlobalServiceManager;				//�������

	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IUnknownEx *					m_pIAttemperEngine;					//��������
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
	virtual bool OnEventTCPSocketRead(DWORD dwServiceID, CMD_Command Command, VOID * pData, WORD wDataSize, DWORD *pdwTokenID, WORD wTokenCount);

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
	//ͨ����Ϣ
	bool OnTCPSocketMainComonService(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//�������
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//ת����Ϣ
	bool OnTCPSocketMainForwardService(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//���Ӵ���
protected:
	//����״̬
	bool OnTCPSocketBattleMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//ͨ����Ϣ
	bool OnTCPSocketBattleMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//Լս����
	bool OnTCPSocketBattleMainBattleService(WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//�����¼�
protected:
	//ע�����
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//����״̬
	bool OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);	
	//�������
	bool OnTCPNetworkMainClubService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//�����¼�
protected:
	//�����б�
	bool OnDBClubList(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//�б����
	bool OnDBClubListFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��������
	bool OnDBClubItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//�������
	bool OnDBClubItemFinish(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//�����Ա
	bool OnDBClubMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��������
	bool OnDBKindSetting(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�����޸�
	bool OnDBKindSettingModify(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//Լս�淨
	bool OnDBBattleConfig(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnDBApplyMessage(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//���³�Ա
	bool OnDBUpdateMember(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//����淨
	bool OnDBAppendConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�޸��淨
	bool OnDBModifyConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//ɾ���淨
	bool OnDBDeleteConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//����ʧ��
	bool OnDBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);	

	//��������
protected:	
	//���Ʋ���
	tagTokenParameter* GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID);	
	//��������
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);	
	//��ѯ����
	bool QueryBattleTable(CClubInfoItem* pClubInfoItem);
	//��������
	bool CreateBattleTable(CClubInfoItem* pClubInfoItem, tagClubBattleConfig* pClubBattleConfig);
	//����Ⱥ��Ƭ
	bool ConstructClubNameCard(const tagClubProperty * pClubProperty, tagClubNameCard * pClubNameCard);	

	//��������
protected:	
	//ת������
	bool ConvertBattleConfig(tagBattleConfig* pBattleConfig, tagClubBattleConfig& BattleConfig);
	//ת������
	bool ConvertBattleConfig(tagClubBattleConfig* pBattleConfig, tagBattleConfig& BattleConfig);

	//���ݷ���
protected:	
	//���ͳ�Ա
	bool SendClubMemberToMember(DWORD dwClubID);
	//���ͳ�Ա
	bool SendClubMemberToUser(DWORD dwClubID, tagSocketInfo *pUserSocketInfo);
	//������Ϣ
	bool SendApplyMessageToUser(DWORD dwClubID, tagSocketInfo *pUserSocketInfo);
	//�����淨
	bool SendBattleConfigToUser(DWORD dwClubID,tagSocketInfo *pUserSocketInfo);	
	//��������
	bool SendBattleTableToUser(DWORD dwClubID, tagSocketInfo* pUserSocketInfo);
	//��������
	bool SendKindSettingToUser(DWORD dwClubID, tagSocketInfo* pUserSocketInfo);

	//�������
protected:
	//����ʧ��
	VOID SendOperateFailure(WORD wServiceID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe);
	//�����ɹ�
	VOID SendOperateSuccess(WORD wServiceID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe);

	//���ݷ���
protected:		
	//��������
	bool SendDataToUser(WORD wServiceModule,WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize,tagSocketInfo *pUserSocketInfo);
	//���ͳ�Ա
	bool SendDataToMember(DWORD dwClubID, WORD wServiceModule, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, BYTE cbLocationMask);
	//��������
	bool SendDataToGate(WORD wGateID, WORD wServiceModule, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD *pdwTokenID, WORD wTokenCount);	
};

//////////////////////////////////////////////////////////////////////////

#endif