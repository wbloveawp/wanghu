#ifndef ATTEMPER_ENGINE_SINK_HEAD_FILE
#define ATTEMPER_ENGINE_SINK_HEAD_FILE

#pragma once

#include "TableFrame.h"
#include "InitParameter.h"
#include "ScoreManager.h"
#include "ServerUserManager.h"
#include "RobotUserManager.h"
#include "GameServiceHead.h"
#include "DistributeManager.h"

//////////////////////////////////////////////////////////////////////////

//��������
#define CLIENT_KIND_FALSH			0x01									//��ҳ����
#define CLIENT_KIND_MOBILE			0x02									//�ֻ�����
#define CLIENT_KIND_ROBOT			0x04									//��������
#define CLIENT_KIND_COMPUTER		0x08									//��������

//////////////////////////////////////////////////////////////////////////

//��������
enum enServiceKind
{
	ServiceKind_None,				//��������
	ServiceKind_Gate				//���ط���	
};


//�󶨲���
struct tagBindParameter
{
	//��������
	DWORD							dwClientIP;							//���ӵ�ַ
	DWORD							dwContextID;						//�󶨱�ʶ
	DWORD							dwActiveTime;						//����ʱ��

	//�����ʶ
	WORD							dwServiceID;						//�����ʶ
	WORD							wServiceModule;						//����ģ��

	//������Ϣ
	tagTokenParameter 				TokenParameter;						//���Ʋ���		
	CGlobalGateItem *				pGlobalGateItem;					//��������
};

//���Ʋ���
struct tagExtendParameter
{
	BYTE							cbClientKind;						//�ն�����
	DWORD							dwPlazaVersion;						//�㳡�汾
	DWORD							dwFrameVersion;						//��ܰ汾
	DWORD							dwProcessVersion;					//��Ϸ�汾
	DWORD							dwPlazaStationID;					//�㳡վ��
};

//ϵͳ����
struct tagSystemParameter
{
	bool							bLoadFlag;							//���ر�ʶ	

	//�ͱ�����
	SCORE							lTakeSubsidyGold;					//���ͽ��
	SCORE							lTakeSubsidyCondition;				//��ȡ����	
};

//����״��
struct tagTableUserStatus
{
	WORD							wLessUserCount;						//��������
	WORD							wTableUserCount;					//�û���Ŀ
	WORD							wTableRobotCount;					//������Ŀ
};

//�û�״��
struct tagRobotUserStatus
{
	WORD							wFreeUserCount;						//�û���Ŀ
	WORD							wPlayUserCount;						//�û���Ŀ
	WORD							wSitdownUserCount;					//�û���Ŀ
	IServerUserItem *				pServerUserFree[MAX_ROBOT];			//�û��ӿ�
	IServerUserItem *				pServerUserPlay[MAX_ROBOT];			//�û��ӿ�
	IServerUserItem *				pServerUserSitdown[MAX_ROBOT];		//�û��ӿ�
};

//�ֵ䶨��
typedef CMap<WORD,WORD,DWORD,DWORD>	 CTableTimerMap;					//��������
typedef CMap<DWORD,DWORD,SCORE,SCORE> CFreezeScoreMap;					//��������
typedef CMap<WORD,WORD,DWORD,DWORD> CUserBindIDMap;						//�û�����

//���鶨��
typedef CWHArray<CTableFrameItem *> CTableFrameItemArray;				//��������
typedef CWHArray<tagModuleVersion> CModuleVersionArray;					//�汾����
typedef CWHArray<tagBattleGameOptionEx> CBattleGameOptionArray;			//��Ϸѡ��

//////////////////////////////////////////////////////////////////////////

//���ȹ���
class CAttemperEngineSink : public IAttemperEngineSink, public IServiceFunction, public IServerUserItemSink,
	public ITableEventSink, public IGameVideoItemSink
{
	//��Ԫ����
	friend class CServiceUnits;

	//״̬����
protected:
	bool							m_bCollectUser;						//���ܱ�־
	bool							m_bNeedStore;						//�洢��־
	bool							m_bNeekCenter;						//���ı�־
	bool							m_bNeedService;						//��־����
	bool							m_bWillConcludeService;				//����Ԥֹͣ
	
	//���Ʊ���
protected:
	DWORD							m_dwIntervalTime;					//���ʱ��
	DWORD							m_dwLastDisposeTime;				//����ʱ��
	
	//ʱ�����
protected:
	DWORD							m_dwRegularTimes;					//����ʱ��
	DWORD							m_dwLockTableTime;					//����ʱ��

	//����Ϣ
protected:
	tagBindParameter *				m_pNormalParameter;					//����Ϣ
	tagBindParameter *				m_pRobotParameter;					//����Ϣ

	//��������
protected:
	CMD_GR_ConfigColumn				CMD_ConfigColumn;					//�б�����
	CMD_GR_ConfigServer				CMD_ConfigServer;					//��������	

	//Լս����
protected:	
	tagServiceItem					m_StoreServer;						//�洢����
	tagServiceItem					m_ServiceItem;						//��������
	CBattleGameOptionArray			m_BattleGameOptionArray;			//��Ϸѡ��	

	//¼�񻺴�
protected:
	bool							m_bCacheVideo;						//�����ʶ
	CWHDataQueue					m_VideoDataQueue;					//�������

	//ϵͳ����
protected:
	CInitParameter *				m_pInitParameter;					//���ò���
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������
	CScoreManager					m_ScoreManager;						//��������		
	tagSystemParameter				m_SystemParameter;					//ϵͳ����	
	tagControlParameter				m_ControlParameter;					//���Ʋ���
	CDistributeManager				m_DistributeManager;				//�������	

	//�ں˽ӿ�
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IAttemperEngine *				m_pIAttemperEngine;					//��������
	ITCPNetworkEngine *				m_pITCPNetworkEngine;				//��������
	IGameServiceManager *			m_pIGameServiceManager;				//�������
	IGameServiceCustomTime *		m_pIGameServiceSustomTime;			//ʱ������	

	//����ӿ�
protected:
	ITCPSocketService *				m_pITCPSocketStore;					//�洢����
	ITCPSocketService*				m_pITCPSocketCenter;				//���ķ���
	ITCPSocketService *				m_pITCPSocketService;				//���ͷ���

	//���ݿ�ӿ�
protected:
	IDataBaseEngine *				m_pIDataBaseEngine;					//�ں�����

	//��������
public:	
	IGameVideoManager *				m_pIGameVideoManager;				//¼�����
	IExtendServiceManager *			m_pIExtendServiceManager;			//�������

	//�������
public:	
	CWordFilter						m_WordsFilter;						//�ַ�����
	
	//�������
protected:
	CTableFrameItemArray			m_TableFrameArray;					//��������	
	CGlobalItemManager				m_GlobalItemManager;				//ȫ����Ϣ
	CServerUserManager				m_ServerUserManager;				//�û�����
	CRobotUserManager				m_RobotUserManager;					//�û�����
	CTableTimerMap					m_LockedTableTimes;					//����ʱ��
	CFreezeScoreMap					m_FreezeScoreMap;					//�������
	CModuleVersionArray				m_ModuleVersionActive;				//ģ��汾

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

	//�û�����
public:
	//���ƿ�ʼ
	virtual bool OnEventUserControlStart(IServerUserItem * pIServerUserItem);
	//���Ƹ���
	virtual bool OnEventUserControlUpdate(IServerUserItem* pIServerUserItem);
	//���ƽ���
	virtual bool OnEventUserControlFinish(IServerUserItem * pIServerUserItem);

	//�û�״̬
public:
	//�û�����
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem);	
	//�û���λ
	virtual bool OnEventUserItemRanking(IServerUserItem * pIServerUserItem);
	//�û�״̬
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID);
	//�û�Ȩ��
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight);
	//�û��Ƹ�
	virtual VOID OnEventUserItemWealth(IServerUserItem* pIServerUserItem, tagWealthVariation& WealthVariation);
	//�û��ȼ�
	virtual VOID OnEventUserUserLevel(IServerUserItem* pIServerUserItem, BYTE cbLastLevelID, BYTE cbCurrLevelID);
	//��Ϸ�ȼ�
	virtual VOID OnEventUserGameLevel(IServerUserItem* pIServerUserItem, BYTE cbLastLevelID, BYTE cbCurrLevelID);
	//�û�����
	virtual VOID OnEventUserItemPack(IServerUserItem* pIServerUserItem, WORD wGoodsIDList[], WORD wGoodsCount);
	//�û�����
	virtual bool OnEventUserFreezeScore(IServerUserItem *pIServerUserItem, SCORE lFreezeScore);

	//��Ϣ�ӿ�
public:
	//��ȡ״̬
	virtual WORD GetServiceStatus();

	//����ӿ�
public:
	//��������
	virtual bool SendBatchData(BYTE cbSendMask, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendDataToTable(WORD wCmdTable, BYTE cbClientKind, WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//��������
	virtual bool SendDataToGate(DWORD dwSocketID, DWORD dwTokenID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//��������
	virtual bool SendDataToUser(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��������
	virtual bool SendDebugData(IServerUserItem* pIServerUserItem, WORD wTableID, WORD wDebugKind, WORD wSubCmdID, VOID* pData, WORD wDataSize);	

	//����ӿ�
public:
	//��������
	virtual bool SendDataToCenter(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//�����¼�
public:
	//�������¼�
	virtual VOID OnEventTableLocked( ITableFrameItem *pITableFrame, BOOL bTableLocked );
	//����״̬�¼�
	virtual VOID OnEventTableStatus( ITableFrameItem *pITableFrame, BOOL bPlayStatus );

	//�����¼�
public:
	//����д������
	virtual VOID OnTableReqOfflineScore( tagOfflineScore const &OfflineScore );
	//�ⶳ��������
	virtual VOID OnTableReqUnfreezeScore( DWORD dwUserID, SCORE lUnfreezeScore );

	//��Ϣ�ӿ�
public:
	//������Ϣ
	virtual bool SendRoomMessage(LPCTSTR lpszMessage, WORD wType);
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//����ƹ���
	virtual bool SendMarqeeNotice(BYTE cbNoticeKind, VOID* pNoticeContent, WORD wBufferSize);
	//������Ϣ
	virtual bool SendRoomMessage(DWORD dwSocketID, DWORD dwTokenID, LPCTSTR lpszMessage, WORD wType);
	//������Ϣ
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);	

	//֪ͨ�ӿ�
public:	
	//��Ϸ��ʼ
	virtual bool OnEventGameStart(ITableFrameItem* pITableFrame);
	//��Ϸ����
	virtual bool OnEventGameConclude(ITableFrameItem * pITableFrame);		

	//����ӿ�
public:
	//�������
	virtual bool InsertDistribute(IServerUserItem * pIServerUserItem);
	//�Ƴ�����
	virtual bool RemoveDistribute(IServerUserItem * pIServerUserItem);

	//¼��ӿ�
public:
	//¼��ʼ
	virtual void OnEventVideoStart(IGameVideoItem * pIGameVideoItem);
	//¼��ֹͣ
	virtual void OnEventVideoConclude(IGameVideoItem * pIGameVideoItem);

	//�洢�ӿ�
public:
	//�洢¼��
	virtual bool StoreVideoData(VOID * pData,WORD wDataSize);

	//Լս�ӿ�
public:
	//��ֵ����
	virtual bool GetOptionValueArray(WORD wEmployScene, WORD wOptionIndex, COptionValueArray & OptionValueArray);

	//���ܽӿ�
public:
	//���дʹ���
	virtual bool FilterWords(LPCTSTR szFiltered);
	//���дʹ���
	virtual void FilterWords(LPWSTR szFiltered, int nMsgLen);
	//���ͷ���
	virtual bool SendUserScore(IServerUserItem *pIServerUserItem);
	
	
	//���ܺ���
public:
	//Ч������
	bool EfficacySocketContext(DWORD dwSocketID,DWORD dwTokenID);

	//�����¼�
protected:
	//ע�����
	bool OnTCPNetworkMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);
	//������Ϣ
	bool OnTCPNetworkMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID);	

	//�����¼�
protected:	
	//��Ϸ����
	bool OnScoketMainGame(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//��ܴ���
	bool OnScoketMainFrame(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);	
	//���Դ���
	bool OnTCPNetworkMainDebug(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//��¼����
	bool OnTCPNetworkMainLogon(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�û�����
	bool OnTCPNetworkMainUser(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//������
	bool OnTCPNetworkMainManage(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);	
	//��Ա����
	bool OnTCPNetworkMainMember(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//��ѯ����
	bool OnTCPNetworkMainQuery(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//��������
	bool OnTCPNetworkMainMatch(WORD wSubCmdID, VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//Լս����
	bool OnTCPNetwrokMainBattle(WORD wSubCmdID, VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//�����¼�
protected:	
	//������¼
	bool OnTCPNetworkSubLogonRobot(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�ֻ���¼
	bool OnTCPNetworkSubLogonMobile(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);	
	//���Ե�¼
	bool OnTCPNetworkSubLogonComputer(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);		
	//�û��ǳ�
	bool OnTCPNetworkSubLogonLogout(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�û�����
	bool OnTCPNetworkSubUserRule(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�û��Թ�
	bool OnTCPNetworkSubUserLookon(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�û�����
	bool OnTCPNetworkSubUserSitDown(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�û�����
	bool OnTCPNetworkSubUserStandUp(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�û�����
	bool OnTCPNetworkSubUserChat(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//��������
	bool OnTCPNetworkSubDebugPacket(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�û�˽��
	bool OnTCPNetworkSubUserWisper(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID, WORD wSubCmdID);
	//�����û�
	bool OnSocketSubInviteUser(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//�ܾ�����
	bool OnTCPNetworkSubUserRepulseSit(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);	
	//�û���Ϣ
	bool OnTCPNetworkSubUserRequestInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//��������
	bool OnTCPNetworkSubUserSwitchTable(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
	//ͬ����Ϣ
	bool OnTCPNetworkSubUserDeskmateInfo(VOID * pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);
    //��������
    bool OnTCPNetworkSubUserSendGift(VOID* pData, WORD wDataSize, DWORD dwSocketID, DWORD dwTokenID);

	//���ݿ��¼�
protected:	
	//������Ϣ
	bool OnDBRobotUser(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//�б�����
	bool OnDBColumnItem(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//���ý��
	bool OnDBConfigResult(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//��Ϸѡ��
	bool OnDBBattleOption(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);	
	//��������
	bool OnDBRobotParameter(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//�ͱ�����
	bool OnDBSubsidyParameter(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//���Ʋ���
	bool OnDBControlParameter(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//��¼ʧ��
	bool OnDBLogonFailure(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//��¼�ɹ�
	bool OnDBLogonSuccess(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//�û��Ƹ�
	bool OnDBWealthUpdate(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//д�ֽ��
	bool OnDBWriteScoreResult(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//�������д�
	bool OnDBFilterWords(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//���½��
	bool OnDBUpdateGold(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//д�����
	bool OnDBWriteUserProp(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//�汾��Ϣ
	bool OnDBGameVersionInfo(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);			
	//��Ʒ����
	bool OnDBUserGoodsUpdate(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);
	//�ͱ����
	bool OnDBLargessSubsidy(DWORD dwContextID, DWORD dwTokenID, VOID * pData, WORD wDataSize);	
	//������Ϣ
	bool OnDBUserControlInfo(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	//���Ƹ���
	bool OnDBUserControlUpdate(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
    //���ͳɹ�
    bool OnDBSendGiftSuccess(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
    //����ʧ��
    bool OnDBSendGiftFailure(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
    //��������
    bool OnDBUpdateLoveLiness(DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize);
	

    //�������
protected:
    //����ʧ��
    VOID SendOperateFailure(DWORD dwSocketID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe);
    //�����ɹ�
    VOID SendOperateSuccess(DWORD dwSocketID, DWORD dwTokenID, WORD wRequestCmdID, LONG lErrorCode, LPCTSTR pszDescribe, LPCTSTR pszGoodsList = NULL);
	
	//���ķ���
protected:
	//ע���¼�
	bool OnTCPSocketMainRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//�б��¼�
	bool OnTCPSocketMainCommonService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//������Ϣ
	bool OnTCPSocketMainForwardService(WORD wSubCmdID, VOID* pData, WORD wDataSize);	

	//ע����
protected:
	//ע���¼�
	bool OnTCPSocketMainStoreRegister(WORD wSubCmdID, VOID* pData, WORD wDataSize);
	//ע���¼�
	bool OnTCPSocketMainServiceRegister(WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//���Ӵ���
protected:			
	//�����¼�
	bool OnTCPSocketMainUserCollect(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//��ѯ�¼�
	bool OnTCPSocketMainQueryService(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�㲥�¼�
	bool OnTCPSocketMainBroadcast(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//�������
	bool OnTCPSocketMainManagerService(WORD wSubCmdID, VOID * pData, WORD wDataSize);		

	//�ڲ��¼�
protected:
	//�û���¼
	VOID OnEventUserLogon(IServerUserItem * pIServerUserItem);
	//�û��ǳ�
	VOID OnEventUserLogout(IServerUserItem * pIServerUserItem);

	//ִ�й���
protected:
	//�л�����
	VOID SwitchUserItemConnect(IServerUserItem * pIServerUserItem, WORD wTargetIndex, DWORD dwTokenID, bool bMobileUser);
	//�汾���
	bool PerformCheckVersion(DWORD dwDeviceKind, DWORD dwPlazaVersion, DWORD dwClientVersion, DWORD dwSocketID, DWORD dwTokenID);

	//��������
protected:
	//��¼ʧ��
	bool SendLogonFailure(LPCTSTR pszString, LONG lErrorCode, DWORD dwSocketID, DWORD dwTokenID);
	//����ʧ��
	bool SendRequestFailure(IServerUserItem* pIServerUserItem, LPCTSTR pszDescribe, BYTE cbFailureCode);
	//�û�����
	bool SendUserEnterPacket(IServerUserItem * pIServerUserItem, DWORD dwSocketID, DWORD dwTokenID,bool bAllLinks, bool bNotifySink=false);	

	//�󶨺���
public:	;
	//�󶨲���
	tagBindParameter * GetBindParameter(WORD wBindIndex);	
	//��ȡ�û�
	IServerUserItem * GetServerUserItem(WORD wBindIndex,DWORD dwTokenID);	
	//���Ʋ���
	tagTokenParameter * GetTokenParameter(WORD wBindIndex, DWORD dwTokenID);
	//��չ����
	tagExtendParameter* GetExtendParameter(WORD wBindIndex, DWORD dwTokenID);

	//�汾����
protected:
	//��ǰ�汾
	tagModuleVersion * GetCurrentVersion(WORD wModuleID,DWORD dwDeviceKind);
	//����汾
	tagModuleVersion * GetSpecialVersion(WORD wModuleID, DWORD dwDeviceKind,DWORD dwVersion);

	//��������
public:
	//�û�״��
	WORD GetRobotUserStatus(tagRobotUserStatus & RobotUserStatus);
	//����״��
	WORD GetTableUserStatus(CTableFrameItem * pTableFrame, tagTableUserStatus & TableUserStatus);

	//�ֻ�����
public:
	//�ֻ�������¼
	bool MobileUserImmediately(IServerUserItem * pIServerUserItem);
	//������Ϣ
	bool SendTableUserToUser(IServerUserItem* pITargetUserItem, BYTE cbClientKind, DWORD dwTableUserID);
	//������Ϣ
	bool SendUserToTableUser(IServerUserItem* pIServerUserItem, BYTE cbClientKind, DWORD dwTableUserID);
	//���ò���
	//void SetMobileUserParameter(IServerUserItem * pIServerUserItem,BYTE dwDeviceKind,WORD wBehaviorFlags,WORD wPageTableCount);	

	//��������
protected:
	//��������
	WORD SearchSuitableTable(BYTE cbSearchMode,IServerUserItem * pIServerUserItem, WORD wLastTableID=INVALID_WORD);
	//��������
	bool SearchTableAndSitdown(BYTE cbSearchMode,IServerUserItem * pIServerUserItem, WORD wLastTableID=INVALID_WORD);

	//��������
protected:	
	//�û�����
	WORD ConstructScorePacket(IServerUserItem* pIServerUserItem, VOID* pBuffer, WORD wBufferSize);
	//�û�״̬
	WORD ConstructStatusPacket(IServerUserItem * pIServerUserItem,VOID * pBuffer,WORD wBufferSize);	
	//�û�����
	WORD ConstructEnterPacket(IServerUserItem* pIServerUserItem, VOID* pBuffer, WORD wBufferSize, BYTE cbClientKind, bool bHideUserInfo = false);
	
	//��������
protected:
	//�޸�����
	bool ModifyGameServer();
	//ִ�з���
	bool PerformDistribute();
	//ͳ������
	DWORD CollectTokenCount();
	//��������
	INT GetTableScoreAreaIndex(CTableFrameItem * pTableFrame);
	//ʱ����
	SCORE GetIntervalSeconds(SYSTEMTIME stStart, SYSTEMTIME stEnd);
	//��������
	bool SendUIControlPacket(WORD wRequestID, VOID * pData, WORD wDataSize);		
	//Ч��˽��Ȩ��
	bool EfficacyUserWhisper(IServerUserItem *pISendUserItem, IServerUserItem *pIRecvUserItem);	

	//��������
protected:	
	//��������
	bool InitTableFrame();
	//��չ����
	bool InitExtendService();
	//���û���
	bool InitRobotService();
	//¼�����
	bool InitVideoService();
	//���÷���
	bool InitServerConfig();
	//������־
	bool InitLoggerService();
};

//////////////////////////////////////////////////////////////////////////

#endif