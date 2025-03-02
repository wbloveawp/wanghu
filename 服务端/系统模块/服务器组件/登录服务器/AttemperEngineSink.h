#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "InitParameter.h"
#include "ServerListManager.h"
#include "DataBasePacket.h"
#include "RankingListManager.h"

//////////////////////////////////////////////////////////////////////////////////
//���ݶ���

//��������
#define CLIENT_KIND_FALSH			1									//��ҳ����
#define CLIENT_KIND_MOBILE			2									//�ֻ�����
#define CLIENT_KIND_COMPUTER		3									//��������

//////////////////////////////////////////////////////////////////////////

//�󶨲���
struct tagBindParameter
{
	//������Ϣ
	DWORD							dwSocketID;							//�����ʶ	
	DWORD							dwActiveTime;						//����ʱ��

	//�����ʶ
	WORD							wServiceID;							//�����ʶ
	WORD							wServiceModule;						//����ģ��

	//������Ϣ
	CGlobalGateItem *				pGlobalGateItem;					//��������
};

//��չ����
struct tagExtendParemter
{
	WORD							wModuleID;							//ģ���ʶ
	BYTE							cbClientKind;						//��������
	DWORD							dwDeviceKind;						//�豸����
	DWORD							dwAppVersion;						//Ӧ�ð汾
	DWORD							dwPlazaVersion;						//�����汾

	//��ֹ��Ϸ
	WORD							wKindCount;							//��������
	WORD							wShieldKindList[16];				//�����б�
};

//ת�˲���
struct tagTransferParameter
{
	bool							bTransferEnabled;					//ת�˿���
	WORD 							wServiceFeeRate;					//�������
	SCORE							lReservedAmount;					//�������
	SCORE							lMinTransferAmount;					//�������
	SCORE							lMaxTransferAmount;					//�������
};

//Ӷ�����
struct tagCommissionParameter
{
	bool							bDrawEnabled;						//��ȡ����
	WORD 							wServiceFeeRate;					//�������
	SCORE							lMinDrawAmount;						//��С���	
};

//�ǳ�����
struct tagNickNameOption
{
	WORD							wFreeModifyTimes;					//����޸Ĵ���
	WORD							wDailyModifyTimes;					//ÿ�տ��޸Ĵ���
	DWORD							dwGoodsID;							//������ƷID
	DWORD							dwGoodsCount;						//������Ʒ����
};

//���Ͷ���
typedef CMap<DWORD, DWORD, WORD, WORD> CMapUserLevel;					//�û��ȼ�
typedef CWHArray<tagModuleVersion>	CModuleVersionArray;				//�汾����																		

//////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//���ñ���
protected:		
	DWORD							m_dwPCFuncption;					//����ѡ��
	DWORD							m_dwMBFuncption;					//����ѡ��
	DWORD							m_dwShowOnLineInfo;					//��ʾ����
	tagServiceOption *				m_pServiceOption;					//��������
	CInitParameter *				m_pInitParameter;					//���ò���
	tagBindParameter *				m_pBindParameter;					//������Ϣ
	tagTransferParameter			m_TransferParameter;				//ת�˲���	
	tagNickNameOption				m_NickNameOption;					//�ǳ�����

	//��������
protected:
	CModuleVersionArray				m_ModuleVersionActive;				//ģ��汾

	//�汾����
protected:
	BOOL							m_bUpdateSilent;					//��̨����

	//��������
protected:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//���ͻ���

	//�������
protected:
	CWordFilter						m_WordsFilter;						//�ʻ����
	CGlobalItemManager				m_GlobalItemManager;				//�������
	CServerListManager				m_ServerListManager;				//�б����
	CRankingListManager				m_RankingListManager;				//�񵥹���

	//����ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IDataBaseEngine *				m_pIDataBaseEngine;					//��������
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
	ITCPSocketService *				m_pITCPSocketCenter;				//���ķ���

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

	//���Ӵ���
protected:
	//ע�����
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�б��¼�
	bool OnTCPSocketMainServiceInfo(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//�û��ռ�
	bool OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//ת������
	bool OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//�������
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);	

	//�����¼�
protected:
	//ע�����
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//������Ϣ
	bool OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);

	//�����¼�
protected:
	//��¼����
	bool OnTCPNetworkMainPCLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�б���
	bool OnTCPNetworkMainPCServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//������
	bool OnTCPNetworkMainPCUserService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//���÷���
	bool OnTCPNetworkMainPCConfigService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//��������
	bool OnTCPNetworkMainPCRobotService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//������
	bool OnTCPNetworkMainPCStockService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�������
	bool OnTCPNetworkMainPCManagerService(WORD wSubCmdID, VOID *pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);	

	//�ֻ��¼�
protected:
	//��¼����
	bool OnTCPNetworkMainMBLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�б���
	bool OnTCPNetworkMainMBServerList(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�û�����
	bool OnTCPNetworkMainMBUserService(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//�����¼�
protected:
	//�ο͵�¼
	bool OnTCPNetworkSubPCLogonByGameID(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�ʺŵ�¼
	bool OnTCPNetworkSubPCLogonByAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//���ߵ�¼
	bool OnTCPNetworkSubPCLogonManageTool(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�ʺ�ע��
	bool OnTCPNetworkSubPCRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	
	//�ֻ���¼
protected:	
	//�ο͵�¼
	bool OnTCPNetworkSubMBLogonVisitor(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//��Կ��¼
	bool OnTCPNetworkSubMBLogonSecretKey(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�ʺŵ�¼
	bool OnTCPNetworkSubMBLogonAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//��֤���¼
	bool OnTCPNetworkSubMBLogonMobileCode(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//��������¼
	bool OnTCPNetworkSubMBLogonThirdParty(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�ֻ���¼
	bool OnTCPNetworkSubMBLogonMobilePhone(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//����ע��
	bool OnTCPNetworkSubMBRegisterEMail(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�ʺ�ע��
	bool OnTCPNetworkSubMBRegisterAccounts(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);	
	//�ֻ�ע��
	bool OnTCPNetworkSubMBRegisterMobilePhone(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);	
	//�����¼
	bool OnTCPNetworkSubMBLogonEMail(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//��֤���¼
	bool OnTCPNetworkSubMBLogonEMailCode(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//��Ϸ�¼�
protected:
	//��¼�ɹ�
	bool OnDBPCLogonSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��¼ʧ��
	bool OnDBPCLogonFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//�û���Ϣ
	bool OnDBPCUserIndividual(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//�����ɹ�
	bool OnDBPCOperateSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//����ʧ��
	bool OnDBPCOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��������
	bool OnDBPCGameBasicConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//Ȩ������
	bool OnDBPCGameWeightConfig(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�������
	bool OnDBPCGameConfigFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�������
	bool OnDBPCRobotOperateResult(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);		
	//�������
	bool OnDBPCStockOperateResult(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);

	//�ֻ��¼�
protected:
	//��¼�ɹ�
	bool OnDBMBLogonSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��¼ʧ��
	bool OnDBMBLogonFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//�ǳ����
	bool OnDBMBLogoutResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�û�ͷ��
	bool OnDBMBUserFaceInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�û�����
	bool OnDBMBUserCacheInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//��Ϊ����
	bool OnDBMBUserActionMask(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//��λ��Ϣ
	bool OnDBMBUserRankingInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);	
	//�󶨳ɹ�
	bool OnDBMBBindMPSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//���ɹ�
	bool OnDBMBUnbindMPSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//��ȡ���
	bool OnDBMBAcquireACResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�󶨳ɹ�
	bool OnDBMBBindEMailSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//���ɹ�
	bool OnDBMBUnbindEMailSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//������Ʒ
	bool OnDBMBUserPackGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//��Ʒ����
	bool OnDBMBUserGoodsUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�Ƹ�����
	bool OnDBMBUserWealthUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
    //��������
    bool OnDBMBUserLoveLinessUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//Ͷ����Ϣ
	bool OnDBMBPostMessage(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�����ɹ�
	bool OnDBMBOperateSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//����ʧ��
	bool OnDBMBOperateFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�һ���Ʒ
	bool OnDBMBExchangeMallGoods(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�һ������
	bool OnDBMBExchangeGiftbagCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//ǩ�����
	bool OnDBMBCheckInResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
    //��������
    bool OnDBMBBindInviteCode(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//���˺�
	bool OnDBMBBindExternalAccount(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//������Ϣ
	bool OnDBMBUserGiftInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
    //��������ɹ�
    bool OnDBMBSendGiftSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);   
    //������Ϣ���
    bool OnDBMBUserGiftInfoFinish(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//ת�˳ɹ�
	bool OnDBMBWealthTransferSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);		
	//ת�̳齱����
	bool OnDBMBTurntableLotteryData(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//ת�̳齱���
	bool OnDBMBTurntableLotteryResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//���в�ѯ���
	bool OnDBMBInsureQueryResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//���гɹ�
	bool OnDBMBInsureSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//����ʧ��
	bool OnDBMBInsureFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�����ʼ�֪ͨ
	bool OnDBMBInsureMailNotify(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�ͱ���ѯ
	bool OnDBMBSubsidyQuery(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�ǳ�����
	bool OnDBMBNickNameOption(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);

	//�б��¼�
protected:
	//��Ϸ����
	bool OnDBGameTypeItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnDBGameKindItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��Ϸ�ڵ�
	bool OnDBGameNodeItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnDBGamePageItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��Ϸ����
	bool OnDBGameFilterItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	

	//�����¼�
protected:
	//���ý��
	bool OnDBConfigResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//ȫ������
	bool OnDBGlobalOption(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//���д���Ϣ
	bool OnDBFilterWordsInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//������
	bool OnDBRankingListData(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//ת�˲���
	bool OnDBTransferParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�汾��Ϣ
	bool OnDBModuleVersionInfo(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);		
	//�󶨻���
	bool OnDBBindMachineResult(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	

	//У�麯��
protected:
	//������֤��
	VOID MakeAuthCode(WCHAR szAutoCode[], BYTE cbBufferSize);

	//�����б�
private:
	//��������
	VOID SendGameTypeInfo(DWORD dwSocketID, DWORD dwTokenID);
	//���ͽڵ�
	VOID SendGameNodeInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wKindID);
	//���Ͷ���
	VOID SendGamePageInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wKindID);
	//��������
	VOID SendGameKindInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wSupportMask);
	//���ͷ���
	VOID SendGameServerInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wKindID, WORD wClientMask);

	//�ֻ��б�
protected:		
	//��������
	VOID SendMobileKindInfo(DWORD dwSocketID, DWORD dwTokenID, tagExtendParemter* pExtendParemter);
	//���ͽ���
	VOID SendMobileJackpotInfo(DWORD dwSocketID, DWORD dwTokenID, tagExtendParemter* pExtendParemter);
	//��������
	VOID SendMobileOnlineInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wModuleID, tagExtendParemter* pExtendParemter);	
	//���ͷ���
	VOID SendMobileServerInfo(DWORD dwSocketID, DWORD dwTokenID, WORD wModuleID, tagExtendParemter* pExtendParemter);

	//��������
protected:
	//������Ʒ
	VOID SendUserPackGoods(DWORD dwSocketID, DWORD dwTokenID, tagGoodsHoldInfo GoodsHoldInfo[], WORD wGoodsCount);
	//�����ǳ�����
	VOID SendNickNameOption(DWORD dwSocketID, DWORD dwTokenID,WORD wModifyTime,WORD wDailyModifyTimes);

	//���Ӳ���
protected:
	//�󶨲���
	tagBindParameter * GetBindParameter(DWORD dwSocketID);
	//���Ʋ���
	tagTokenParameter * GetTokenParameter(DWORD dwSocketID, DWORD dwTokenID);

	//���ͺ���
protected:
	//Ⱥ������
	bool SendBatchData(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//��������
	bool SendDataToGate(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, DWORD* pdwTokenID, WORD wTokenCount);
	//��������
	bool SendDataToGate(DWORD dwSocketID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD* pdwTokenID, WORD wTokenCount);    

	//�������
protected:
	//����ʧ��
	VOID SendOperateFailure(DWORD dwSocketID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe);
	//�����ɹ�
	VOID SendOperateSuccess(DWORD dwSocketID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe, LPCTSTR pszGoodsList = NULL);

	//��������
protected:
	//�����л�
	VOID OnEventDaySwitch();
	//��������
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);
	//����ƹ���
	bool SendMarqeeNotice(BYTE cbNoticeKind, VOID* pNoticeContent, WORD wBufferSize);
	//�汾���
	bool CheckPlazaVersion(DWORD dwDeviceKind, DWORD dwPlazaVersion, DWORD dwSocketID, DWORD dwTokenID);
	//��ǰ�汾
	tagModuleVersion * GetCurrentVersion(WORD wModuleID,DWORD dwDeviceKind);
	//����汾
	tagModuleVersion * GetSpecialVersion(WORD wModuleID,DWORD dwDeviceKind,DWORD dwVersion);	
};

//////////////////////////////////////////////////////////////////////////

#endif