#ifndef BATTLE_TABLE_FRAME_HEAD_FILE
#define BATTLE_TABLE_FRAME_HEAD_FILE

//�����ļ�
#include "BattleServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//Լս״̬
#define BATTLE_STATE_FREE			0									//����״̬
#define BATTLE_STATE_WAIT			1									//�ȴ�״̬
#define BATTLE_STATE_PLAY			2									//��Ϸ״̬

//ʱ�Ӷ���
#define IDI_TABLE_DISMISS			(TIME_TABLE_EXTEND_START+0)			//���ӽ�ɢ
#define IDI_AGREEMENT_DISMISS		(TIME_TABLE_EXTEND_START+1)			//Э���ɢ

//////////////////////////////////////////////////////////////////////////

typedef CWHArray<tagBattleUserItem> CBattleUserItemArray;				//Լս���

//////////////////////////////////////////////////////////////////////////

//Լս����
class CTableFrameBattle : public CTableFrameItem, public IEventBattleSink
{
	//Լս��־
protected:
	bool							m_bDeductCost;						//�۷ѱ�ʶ
	bool							m_bReturnCost;						//�˷ѱ�ʶ
	bool							m_bDismissNotify;					//��ɢ֪ͨ
	bool							m_bBattleService;					//����״̬
	bool							m_bBattleFinish;					//Լս����	

	//������Ϣ
public:
	DWORD							m_dwClubID;							//Ⱥ���ʶ
	WORD							m_wSwitchOption;					//Ⱥ�����
	WORD							m_wClubServiceID;					//�����ʶ
	DWORD							m_dwClubCreaterID;					//Ⱥ����ʶ

	//Լս����
protected:	
	DWORD							m_dwConfigID;						//�淨��ʶ
	DWORD							m_dwBattleRule;						//Լս����
	WORD							m_wFinishCount;						//��ɾ���
	BYTE							m_cbBattleState;					//Լս״̬
	DWORD							m_dwBattleNum;						//ӳ����
	DWORD							m_dwStartTime;						//��ʼʱ��
	DWORD							m_dwCreateTime;						//����ʱ��	
	SCORE							m_lBattleRecordID;					//��¼��ʶ
	tagBattleConfigEx				m_BattleConfig;						//Լս����	

	//Լս�ɼ�
protected:
	CUserIDArray					m_ChargeUserArray;					//�۷����
	CBattleUserItemArray			m_BattleUserItemArray;				//�������

	//Э���ɢ
protected:
	DWORD							m_dwLaunchTime;						//����ʱ��
	DWORD							m_dwLaunchUserID;					//�����û�
	BYTE							m_cbBallotResult[MAX_CHAIR];		//ͶƱ���

	//����ӿ�
protected:
	IEventBattleItem*				m_pIEventBattleItem;				//Լս����
	IEventGameOption*				m_pIEventGameOption;				//���ýӿ�
	ITCPSocketService*				m_pITCPSocketBattle;				//Լս����	

	//��������
public:
	//���캯��
	CTableFrameBattle();
	//��������
	virtual~CTableFrameBattle();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//��Ϣ����
public:	
	//��������
	virtual WORD GetRelationChairID(DWORD dwUserID);
	//��������
	virtual tagRelationParameter* GetRelationParameter();

	//ͳ�ƽӿ�
public:
	//����˰��
	virtual SCORE AccountRevenue(SCORE lGameScore);

	//���ֽӿ�
public:
	//д�����
	virtual bool WriteUserScore(WORD wChairID, tagScoreInfo& ScoreInfo);
	//д�����
	virtual bool WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount);

	//Լս�ӿ�
public:
	//Լս����
	virtual bool BattleFinish(BYTE cbReason);
	//��ȡ����
	virtual bool GetOptionValueArray(WORD wOptionIndex, COptionValueArray& OptionValueArray);	

	//Լս����
public:
	//Լս����
	virtual DWORD GetBattleRule() { return m_dwBattleRule; }
	//Լս����
	virtual VOID SetBattleRule(DWORD dwAddRules, DWORD dwRemoveRules);

	//��Ϸ�¼�
public:
	//��������
	virtual bool TryStartGame();
	//��ʼ�¼�
	virtual bool OnEventGameStart();
	//����ʱ��
	virtual bool OnEventGameConclude();

	//�û��¼�
public:
	//�����¼�
	virtual bool OnActionUserSitdown(IServerUserItem* pIServerUserItem);
	//�����¼�
	virtual bool OnActionUserStandup(IServerUserItem* pIServerUserItem);

	//У���¼�
public:
	//�Թ�У��
	virtual bool EfficacyUserLookon(WORD wChairID, IServerUserItem* pIServerUserItem);
	//����У��
	virtual bool EfficacyUserSitdown(WORD wChairID, IServerUserItem* pIServerUserItem);
	//����У��
	virtual bool EfficacyUserStandup(WORD wChairID, IServerUserItem* pIServerUserItem, bool bInitiative);

	//ϵͳ�¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//�ն�׼��
	virtual VOID OnEventClientReady(IServerUserItem* pIServerUserItem);	
	//���ӳ�ʼ��
	virtual VOID OnEventTableFrameInit(tagTableFrameParameter& TableFrameParameter);

	//ϵͳ�¼�
public:
	//��Ϸ�¼�
	virtual bool OnEventSocketGame(WORD wSubCmdID, VOID* pData, WORD wDataSize, IServerUserItem* pIServerUserItem);
	//����¼�
	virtual bool OnEventSocketFrame(WORD wSubCmdID, VOID* pData, WORD wDataSize, IServerUserItem* pIServerUserItem);

	//֪ͨ����
public:
	//���ӳɼ�
	bool NotifyTableGrade();
	//�û��б�
	bool NotifyTableUserList();
	//Լսս��
	bool NotifyTableBattle(bool bRecordBattle);
	//���Ӳ���
	bool NotifyTableParam(WORD wChairID = INVALID_CHAIR);
	//��������
	bool NotifyTableConfig(WORD wChairID = INVALID_CHAIR);	

	//���Ӳ���
public:
	//��������
	bool CreateBattleTable(tagBattleCreateParam& CreateParam);
	//�޸�����
	bool ModifyBattleTable(tagBattleModifyParam& ModifyParam);
	//������Ϣ
	void GetBattleTableItem(tagBattleTableItemEx& TableItem);	

	//���ܺ���
public:
	//����״̬
	void SetBattleService(bool bBattleService);	
	//Լս״̬
	BYTE GetBattleState() { return m_cbBattleState; }
	//Լս����
	tagBattleConfigEx* GetBattleConfig() { return &m_BattleConfig; }

	//��ɢ����
public:
	//��ɢ����
	bool DismissBattleTable(DWORD dwUserID);
	//��ɢ����
	void DismissBattleTable(bool bRecordBattle);

	//���ô���
public:
	//�۷ѽ��
	bool DeductCostResult(bool bDeductCostSuccess, DWORD dwPayUserID, DWORD dwFailUserID);

	//��������
protected:
	//����ж�
	bool IsBattleFinished();
	//��¼��ʶ
	SCORE BuildBattlleRecordID();
	//�˻�����
	bool PerformReturnCost(WORD wPayType);
	//����˰��
	SCORE CalcCustomRevenue(SCORE lGameScore);
	//��ȡ�û�
	IServerUserItem* GetFirstSitUserItem();	
	//��ȡ�û�
	tagBattleUserItem* GetBattleUserItem(DWORD dwUserID);
	//��ɢ����
	bool SendDimissNotify(IServerUserItem* pIServerUserItem);	
	//У������
	bool VerifyBattleConfig(tagBattleConfig* pBattleConfig, bool bOnlyVerify);
	//ִ��ͶƱ
	bool PerformBallotDismiss(WORD wChairID, DWORD dwUserID, bool bAgreeDismiss);
};

//////////////////////////////////////////////////////////////////////////

#endif