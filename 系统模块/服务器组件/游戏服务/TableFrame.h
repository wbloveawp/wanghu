#ifndef TABLE_FRAME_HEAD_FILE
#define TABLE_FRAME_HEAD_FILE

#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//ʱ�Ӷ���
#define IDI_OFF_LINE				(TIME_TABLE_KERNEL_START+0)			//���߱�ʶ
#define IDI_KICKOUT_PLAYER			(TIME_TABLE_KERNEL_START+1)			//�߳���ʶ
#define IDI_KICKOUT_LOOKON			(TIME_TABLE_KERNEL_START+2)			//�߳���ʶ

//ʱ�䶨��
#define TIME_KICKOUT_LOOKON			10L									//����Թ�

//////////////////////////////////////////////////////////////////////////

//���鶨��
typedef IServerUserItem * CTableUserItemArray[MAX_CHAIR];				//��Ϸ����

typedef CWHArray<DWORD> CUserIDArray;									//��ʶ����
typedef CWHArray<IServerUserItem *> CLookonUserItemArray;				//�Թ�����


//////////////////////////////////////////////////////////////////////////////////
//��������
struct tagRelationParameter
{	
	DWORD			dwRelationNum;				//�������
	SCORE			lRelationRecordID;			//��¼��ʶ
};

//��������-Լս
struct tagRelationParameter_Battle : public tagRelationParameter
{
	DWORD			dwClubID;					//�����ʶ	
	BYTE			cbScoreKind;				//��������	
};

//��������-��λ
struct tagRelationParameter_Ranking : public tagRelationParameter
{	
};

//////////////////////////////////////////////////////////////////////////

//���ӿ��
class GAME_SERVICE_CLASS CTableFrameItem : public ITableFrameItem, public IGameBalanceControl
{
	//��������
protected:
	WORD							m_wTableID;							//���Ӻ���
	WORD							m_wChairCount;						//������Ŀ
	enStartMode						m_GameStartMode;					//��ʼģʽ

	//�������
protected:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//��������

	//��������
protected:
	DWORD							m_dwLastEnterTime;					//����ʱ��
	DWORD							m_dwLastLeaveTime;					//�뿪ʱ��

	//�������
public:
	WORD							m_wMaxPlayerCount;					//�������
	WORD							m_wSitdownMultiple;					//���±���
	WORD							m_wStandupMultiple;					//��������

	//��־����
protected:
	bool							m_bVideoData;						//¼������
	bool							m_bWriteScore;						//д�ֱ�־
	bool							m_bGamePaused;						//��ͣ��־	
	bool							m_bGameStarted;						//��ʼ��־
	bool							m_bTableStarted;					//��ʼ��־
	bool							m_bAllowLookon[MAX_CHAIR];			//�Թ۱�־

	//���ܱ�ʶ
protected:
	bool							m_bSameIpRemind;					//ͬIP����

	//��Ϸ����
protected:
	LONG							m_lBaseScore;						//��Ϸ�׷�
	DWORD							m_dwPlayCount;						//��Ϸ����
	BYTE							m_cbGameStatus;						//��Ϸ״̬
	DWORD							m_dwGameStartTime;					//��ʼʱ��
	DWORD							m_dwPlayerID[MAX_CHAIR];			//��ұ�ʶ
	LONGLONG                        m_lMinEnterScore;                   //��ͽ��� KK JIA

	//������Ϣ
protected:
	DWORD							m_dwTableOwnerID;					//�����û�
	TCHAR							m_szTablePassword[LEN_PASSWORD];	//��������	

	//ƽ�����
public:
	bool							m_bControlCard;						//���Ʊ�ʶ
	BYTE							m_cbUserCardLevel[TABLE_USER_COUNT];//�˿˼���	

	//���ּ�¼
protected:
	DWORD							m_dwPlayTimeCount;					//��Ϸʱ��
	tagScoreInfoEx					m_ScoreInfoArray[MAX_CHAIR];		//��Ϸ����

	//����Ѽ�¼
protected:
	bool							m_bCostGame[MAX_CHAIR];				//�۷ѱ�ʶ

	//���߱���
protected:
	WORD							m_wOffLineCount[MAX_CHAIR];			//���ߴ���
	DWORD							m_dwOffLineTime[MAX_CHAIR];			//����ʱ��

	//���˱���
protected:
	DWORD							m_dwUserUnReadyTime[MAX_CHAIR];		//δ׼��ʱ��

	//�������ּ�¼
protected:
	tagOfflineScore					m_OfflineScore[MAX_CHAIR];			//�뿪����

	//�û�����
protected:
	CTableUserItemArray				m_TableUserItemArray;				//��Ϸ�û�
	CLookonUserItemArray			m_LookonUserItemArray;				//�Թ��û�

	//�ں����
protected:
	ITimerEngine *					m_pITimerEngine;					//ʱ������
	IDataBaseEngine *				m_pIDataBaseEngine;					//�ں����ݿ�
	IDataBaseEngine *				m_pIAssistDataBaseEngine;			//��¼���ݿ�

	//¼��ӿ�
protected:
	IGameVideoItem *				m_pIGameVideoItem;					//¼��ӿ�
	IGameVideoManager *				m_pIGameVideoManager;				//¼�����

	//����ӿ�
protected:
	ITableFrameSink	*				m_pITableFrameSink;					//���ӽӿ�
	ITableUserAction *				m_pITableUserAction;				//�����ӿ�
	IEventUserScore *				m_pIEventUserScore;					//���ֽӿ�
	IEventBaseScore *				m_pIEventBaseScore;					//�׷ֽӿ�	
	ITableUserRequest*				m_pITableUserRequest;				//�����¼�
	IGameStockQuery*				m_pIGameStockQuery;					//����ѯ
	IServiceFunction *				m_pIServiceFunction;				//���ܽӿ�
	IEventStartEfficacy *			m_pIEventStartEfficacy;				//��ʼУ��
	IEventServiceCost *				m_pIEventServiceCost;				//��˰�¼�
	IEventCustomFrameTime *			m_pIEventCustomFrameTime;			//���ʱ��
	ITableEventSink *				m_pITableEventSink;					//�����¼�		
	IRobotUserManager *				m_pIRobotUserManager;				//��������	
	ITableControlSendCard *			m_pITableControlSendCard;			//�������ӿ�

	//������Ϣ
protected:	
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//��������
	tagGameServiceOption *			m_pGameServiceOption;				//��������

	//��������
public:
	//���캯��
	CTableFrameItem();
	//��������
	virtual ~CTableFrameItem();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//���ýӿ�
public:
	//��ʼģʽ
	virtual enStartMode GetStartMode() { return m_GameStartMode; }
	//��ʼģʽ
	virtual VOID SetStartMode(enStartMode StartMode) { m_GameStartMode=StartMode; }

	//��Ԫ����
public:
	//��Ԫ����
	virtual LONG GetCellScore() { return m_lBaseScore; }
	//��Ԫ����
	virtual VOID SetCellScore(LONG lCellScore);

	//��ͽ��� KK JIA
	virtual VOID SetMinEnterScore(LONGLONG lMinEnterScore);

	//��Ϣ�ӿ�
public:
	//��Ϸ״̬
	virtual bool IsGameStarted() { return m_bGameStarted; }
	//��Ϸ״̬
	virtual bool IsTableStarted() { return m_bTableStarted; }
	//����״̬
	virtual bool IsTableLocked() { return ((m_szTablePassword[0]!=0) && (!CServerRule::IsForbidPasswordSet(m_pGameServiceOption->dwServerRule))); }

	//״̬�ӿ�
public:
	//��ȡ״̬
	virtual BYTE GetGameStatus() { return m_cbGameStatus; }
	//����״̬
	virtual VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus=cbGameStatus; }

	//���Խӿ�
public:
	//��λ����Ŀ
	virtual WORD GetNullChairCount();
	//���Ӻ���
	virtual WORD GetTableID() { return m_wTableID; }
	//��Ϸ����
	virtual WORD GetChairCount() { return m_wChairCount; }	
	//��Ϸʱ��
	virtual DWORD GetPlayTimeCount() { return (DWORD)time(NULL)-m_dwGameStartTime; }

	//���ò���
public:
	//��ȡ����
	virtual tagGameServiceAttrib * GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//��ȡ����
	virtual tagGameServiceOption * GetGameServiceOption() { return m_pGameServiceOption; }

	//���ƽӿ�
public:
	//��ʼ��Ϸ
	virtual bool StartGame();
	//��ͣ��Ϸ
	virtual bool PauseGame();
	//������Ϸ
	virtual bool ContinueGame();
	//��ɢ��Ϸ
	virtual bool DismissGame();
	//������Ϸ
	virtual bool ConcludeGame(LPCTSTR pszGameCourse);

	//ͳ�ƽӿ�
public:
	//����˰��
	virtual SCORE AccountRevenue(SCORE lGameScore);

	//���ֽӿ�
public:
	//�۷����
	virtual VOID CostForGameStart();
	//д�����
	virtual bool WriteUserScore(WORD wChairID, tagScoreInfo& ScoreInfo);
	//д�����
	virtual bool WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount);	
	//kk jjia
	//qu zhuo zi canshu
	virtual CMD_GF_TableParam GetTableParam(tagScoreInfo ScoreInfoArray[], WORD wScoreCount);

	//��ؽӿ�
public:
	//�����˿�
	virtual bool IsControlCard();
	//�˿˵ȼ�
	virtual BYTE GetUserCardLevel(WORD wChairID);

  //�û��ӿ�
public:
	//Ѱ���û�
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID);
	//��Ϸ�û�
	virtual IServerUserItem * GetTableUserItem(WORD wChairID);
	//�Թ��û�
	virtual IServerUserItem * EnumLookonUserItem(WORD wEnumIndex);

	//ʱ��ӿ�
public:
	//����ʱ��
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);
	//ɾ��ʱ��
	virtual bool KillGameTimer(DWORD dwTimerID);

	//����ӿ�
public:
	//��������
	virtual bool SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID);
	//��������
	virtual bool SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//��������
	virtual bool SendDebugData(IServerUserItem* pIServerUserItem, WORD wDebugKind, WORD wSubCmdID);
	//��������
	virtual bool SendDebugData(IServerUserItem* pIServerUserItem, WORD wDebugKind, WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//����ӿ�
public:
	//��������
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID, WORD wMainCmdID = MDM_GF_GAME);
	//��������
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID, VOID* pData, WORD wDataSize, WORD wMainCmdID = MDM_GF_GAME);
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, WORD wMainCmdID = MDM_GF_GAME, BYTE cbDataMask = 0xFF);
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, VOID* pData, WORD wDataSize, WORD wMainCmdID = MDM_GF_GAME, BYTE cbDataMask = 0xFF);

	//���ܽӿ�
public:
	//������Ϣ
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//����ƹ���
	virtual bool SendMarqeeNotice(BYTE cbNoticeKind, VOID* pNoticeContent, WORD wNoticeSize);
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//������Ϣ
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);	

	//���ݽӿ�
public:
	//�����¼�
	virtual bool PostDataBaseRequest(WORD wRequestID, DWORD dwEngineKeyID, DWORD dwTokenID);
	//�����¼�
	virtual bool PostDataBaseRequest(WORD wRequestID, DWORD dwEngineKeyID, DWORD dwTokenID, VOID* pData, WORD wDataSize);

	//��������
public:
	//��������
	virtual bool PerformStandUpAction(IServerUserItem * pIServerUserItem, bool bInitiative=false);
	//�Թ۶���
	virtual bool PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem);
	//���¶���
	virtual bool PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem);

	//���ܽӿ�
public:
	//�۳������
	virtual VOID ChargeGameAndReset();
	//��������
	virtual VOID FreezeGameScore(IServerUserItem* pIServerUserItem);
	//ģ������
	virtual bool ImitateGameOption(IServerUserItem* pIServerUserItem);
	//���ͳ���
	virtual bool OnEventSendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);		

	//�����ӿ�
public:
	//�������
	virtual VOID OnEventUserScoreChange(IServerUserItem *pIServerUserItem);

	//ͳ�ƽӿ�
public:
	//�û���Ŀ
	virtual WORD GetSitUserCount();
	//������Ŀ
	virtual WORD GetRealUserCount();
	//������Ŀ
	virtual WORD GetRobotUserCount();
	//�Թ���Ŀ
	virtual WORD GetLookonUserCount();
	//������Ŀ
	virtual WORD GetOffLineUserCount();

	//�������
public:
	//��ȡ����
	virtual WORD GetMaxPlayerCount() { return m_wMaxPlayerCount; }
	//��������
	virtual VOID SetMaxPlayerCount(WORD wPlayerCount) { m_wMaxPlayerCount = wPlayerCount; }

	//��Ϸ����
public:
	//���±���
	virtual VOID SetSitdownGameMultiple(WORD wGameMultiple);
	//��������
	virtual VOID SetStandupGameMultiple(WORD wGameMultiple);

	//������Ϣ
public:
	//��������
	virtual WORD GetRelationChairID(DWORD dwUserID);
	//��������
	virtual tagRelationParameter * GetRelationParameter();

	//��Ϣ����
public:
	//��������
	LPCTSTR GetTablePassword() { return m_szTablePassword; }

	//��¼����
public:
	//��¼¼��
	bool RecordGameVideo();
	//��¼����
	bool RecordGameScore(LPCTSTR pszGameCourse);	

	//���ܺ���
public:
	//��ȡ��λ
	WORD GetNullChairID();
	//�����λ
	WORD GetRandNullChairID();				
	//δ׼����Ŀ
	WORD GetUnReadyUserCount();	
	//��������
	bool InitTableFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter);	

	//���ܺ���
public:		
	//У������
	bool ContrastPassword(LPCTSTR pszPassword);	
	//�û�״��
	WORD GetTableUserInfo(tagTableUserInfo & TableUserInfo);		
	//��������
	bool PerformConnectAction(IServerUserItem *pIServerUserItem);

	//�û��¼�
public:
	//�û������¼�
	bool OnEventUserRule(IServerUserItem *pIServerUserItem);
	//�����¼�
	bool OnEventUserOffLine(IServerUserItem * pIServerUserItem);
	//�ؽ��¼�
	bool OnEventUserReConnect(IServerUserItem * pIServerUserItem);

	//��Ϸ�¼�
public:
	//���Կ�ʼ
	virtual bool TryStartGame();
	//��ʼ�¼�
	virtual bool OnEventGameStart();
	//����ʱ��
	virtual bool OnEventGameConclude();

	//�û��¼�
public:
	//�û�����
	virtual bool OnActionUserSitdown(IServerUserItem* pIServerUserItem);
	//�û�����
	virtual bool OnActionUserStandup(IServerUserItem* pIServerUserItem);
	
	//�û��¼�
public:
	//�Թ�У��
	virtual bool EfficacyUserLookon(WORD wChairID, IServerUserItem* pIServerUserItem);
	//����У��
	virtual bool EfficacyUserSitdown(WORD wChairID, IServerUserItem* pIServerUserItem);	
	//����У��
	virtual bool EfficacyUserStandup(WORD wChairID, IServerUserItem* pIServerUserItem, bool bInitiative);

	//�¼�����
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);	
	//��Ϸѡ��
	virtual VOID OnEventClientReady(IServerUserItem* pIServerUserItem);	
	//���ӳ�ʼ��
	virtual VOID OnEventTableFrameInit(tagTableFrameParameter& TableFrameParameter);	

	//ϵͳ�¼�
public:
	//��Ϸ�¼�
	virtual bool OnEventSocketGame(WORD wSubCmdID, VOID* pData, WORD wDataSize, IServerUserItem* pIServerUserItem);
	//����¼�
	virtual bool OnEventSocketFrame(WORD wSubCmdID, VOID* pData, WORD wDataSize, IServerUserItem* pIServerUserItem);

	//���ƽӿ�
public:
	//������
	virtual bool ControlSendCard(IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize);

	//��������
protected:
	//����״̬
	bool SendTableStatus();	
	//��ʼ״̬
	bool SendStartStatus();			
	//����ʧ��
	bool SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, BYTE cbFailureCode);
	//�汾���
	bool PerformCheckVersion(DWORD dwFrameVersion, DWORD dwClientVersion, IServerUserItem * pIServerUserItem);

	//Ч�麯��
public:
	//��ʼЧ��
	bool EfficacyStartGame(WORD wReadyChairID);
	//��ַЧ��
	bool EfficacyAddrRule(IServerUserItem * pIServerUserItem);
	//����Ч��
	bool EfficacyScoreRule(IServerUserItem * pIServerUserItem);
	//����Ч��
	bool EfficacyTableRule(IServerUserItem * pIServerUserItem);

	//��������
protected:	
	//����ȼ�
	VOID CalcUserCardLevel();
	//��Ϸ��¼��ʶ
	SCORE BuildGameRecordID();				
	//¼��Ƶ��
	VOID BuildVideoChannel(TCHAR szBuffer[], WORD wBufferLen);	
};

//////////////////////////////////////////////////////////////////////////

#endif