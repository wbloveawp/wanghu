#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "log_writer.h"

//////////////////////////////////////////////////////////////////////////

////��������
//struct tagConfigCard
//{
//	BOOL									bAllotFlag;									//���Ʊ�ʶ
//	BYTE									cbCardData[54];//�˿�����
//};

//���ƽ��
struct tagCompareResult
{
	int							nResult;					//���ս��
	int							nHeadResult;				//ͷ�ƽ��
	int							nTailResult;				//β�ƽ��
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ������
class CTableFrameSink : public ITableFrameSink, public ITableUserAction, public IEventBaseScore, public IEventUserScore, public IEventBattleItem, public IEventStartEfficacy, public IEventCustomFrameTime, public ITableControlSendCard
{
	//��Ϸ����
protected:
	BYTE							m_cbPlayerStatus[GAME_PLAYER];			//��Ϸ״̬
	BYTE							m_cbShowHand[GAME_PLAYER];				//ȫѹ״̬
	BYTE							m_cbOpenFinish[GAME_PLAYER];			//����״̬
	bool							m_bInvestFinish[GAME_PLAYER];			//��ɱ�־
	bool							m_bUserReConnect[GAME_PLAYER];			//��������
	WORD							m_wFirstUser;							//�׽��û�
	WORD							m_wBankerUser;							//ׯ���û�
	WORD							m_wCurrentUser;							//��ǰ�û�
	WORD                           m_wSetRuleUser;							//�����û�
	bool							m_bGameRule;							//�����־
	LONGLONG						m_lCellScore;							//��Ԫ��ע
	WORD							m_wOperaCount;							//��������
	BYTE							m_cbBalanceCount;						//��ע����
	LONGLONG						m_lBalanceScore;						//ƽ����Ŀ
	LONGLONG						m_lUserScore[GAME_PLAYER];				//�û����
	LONGLONG						m_lTableScore[GAME_PLAYER];				//��ע��Ŀ
	LONGLONG						m_lTotalScore[GAME_PLAYER];				//��ע��Ŀ
	LONGLONG						m_lLoseScore[GAME_PLAYER];				//�����Ŀ
	LONGLONG						m_lTurnMaxScore;						//�����ע
	LONGLONG						m_lTurnMinScore;						//��С��ע
	LONGLONG						m_lMinAddScore;							//���¼�ע
	LONGLONG                        m_lUserBeginScore[GAME_PLAYER];			//��ʼ����(������)
	LONGLONG                        m_lUserFreeScore[GAME_PLAYER];			//�û�����
	WORD							m_wBattleCount;							//Լս����

		//˰������ kk jia ����1ʱ����˰��1-2֮���� lWinnerRevenue1��2-3֮���� lWinnerRevenue2������2 ��lWinnerRevenue3˰
	SCORE							m_lWinnerScore1;						//Ӯ�ҷ���
	SCORE							m_lWinnerScore2;						//Ӯ�ҷ���
	SCORE							m_lWinnerRevenue1;					//Ӯ��˰��	

	SCORE							m_lWinnerScore3;						//Ӯ�ҷ���

	SCORE							m_lWinnerRevenue2;					//Ӯ��˰��	

	SCORE							m_lWinnerRevenue3;					//Ӯ��˰��

	SCORE                           m_lMinBetValue;//�����עkk jia

//���Ʊ���
protected:
	bool							m_bAllotCard;									//����״̬
	WORD							m_wControBankerUser;							//����ׯ��
	BYTE							m_cbControlCardData[GAME_PLAYER][MAX_COUNT];	//��������

	//��������
protected:
	DWORD							m_dwDelayTime;							//��ʱʱ��

	//���ñ���
protected:
	tagCustomConfig					m_CustomConfig;							//�Զ�������
	tagBattleCreateParam            m_BattleConfig;							// Լս����

	//�˿˱���
protected:
	BYTE							m_cbHandCardCount[GAME_PLAYER];			//��������
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//�����˿�
	BYTE							m_cbZhuoMianCardData[28];//�����˿�

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�	
	ITableFrameItem *				m_pITableFrame;							//��ܽӿ�
	IEventBattleSink *				m_pIEventBattleSink;					//Լս�ӿ�	
	tagGameServiceOption			* m_pGameServiceOption;					//���ò���	
	IGameServiceDynamicRule			* m_pIGameServiceDynamicRule;			//����ӿ�

	//���Ա���
protected:
	static const WORD				m_wPlayerCount;							//��Ϸ����
	
	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID __cdecl Release() { delete this; }
	//�Ƿ���Ч
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//�ӿڲ�ѯ
	virtual void *  __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//����ӿ�
public:
	//��λ����
	virtual VOID ResetTableFrameSink();
	//��������
	virtual bool InitTableFrameSink(IUnknownEx * pIUnknownEx);

	//��Ϸ״̬
	virtual bool  __cdecl IsUserPlaying(WORD wChairID);

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart() ;
	//��Ϸ��ͣ
	virtual bool OnEventGamePause() { return true; }
	//��Ϸ����
	virtual bool OnEventGameContinue() { return true; }
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason) ;
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret) ;


	//�¼��ӿ�
public:
	//��ʱ���¼�
	virtual bool  __cdecl OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//��Ϸ��Ϣ����
	virtual bool  __cdecl OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool  __cdecl OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	

	//�û��¼�
public:
	//�û�����
	virtual bool __cdecl OnActionUserOffLine(IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool __cdecl OnActionUserConnect(IServerUserItem * pIServerUserItem);
	//�û�����
	virtual bool __cdecl OnActionUserSitDown(IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool __cdecl OnActionUserStandUp(IServerUserItem * pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool __cdecl OnActionUserReady(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//�¼��ӿ�
public:		
	//�����ʶ
	virtual WORD GetMasterChairID() { return INVALID_CHAIR; }
	//��ʼ����
	virtual WORD __cdecl GetStartPlayerCount();
	//����׼��
	virtual bool __cdecl IsAllowPlayerReady(WORD wChairID);		
	//�׷ֱ��
	virtual VOID __cdecl OnEventBaseScoreVariation(LONG lBaseScore);
	//�������
	virtual VOID __cdecl OnEventScoreVariation(IServerUserItem *pIServerUserItem);

	//�ӿں���
public:
	//��ɢʱ��
	virtual DWORD GetBattleDismissTime() ;
	//����ʱ��
	virtual DWORD GetUnReadyKickOutTime() ;
	//��ɢʱ��
	virtual DWORD GetAgreementDismissTime() ;


	//��Ϣ����
public:
	//Ӯȡ����
	virtual SCORE GetBattleWinScore() { return 0; }
	//ͬ������
	virtual WORD GetDismissAgreeCount() ;
	//ԼսժҪ
	virtual LPCTSTR GetUserBattleAbstract(WORD wChairID) { return L""; }

	//�¼�֪ͨ
public:
	//���ӽ�ɢ
	virtual VOID OnEventTableDismiss() ;
	//�����޸�
	virtual VOID OnEventTableModify(LPVOID pModifyParam, WORD wBufferSize) ;
	//���Ӵ���
	virtual VOID OnEventTableCreated(LPVOID pCreateParam, WORD wBufferSize) ;

	//���Ʒ���
public:
	//������
	virtual bool OnEventControlSendCard(IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize);

	//��������
public:
	//��ע��Ϣ
	void SendUserAddInfo(WORD wAddUser,LONGLONG lScore,WORD wNextUser,LONGLONG lMinScore,LONGLONG lMaxScore);
	//������Ϣ
	void SendSendCardInfo(BYTE cbCardCount,WORD wCurrentUser);
	//�����ע
	LONGLONG GetMaxUserCanLostScore( WORD wChairID);

	//��������
protected:
	//�رն�ʱ��
	void KillUserGameTimer(WORD wChairID);
	//���ö�ʱ��
	void SetUserGameTimer(WORD wChairID,DWORD dwTimeOut,DWORD dwDelayTime);

	//��Ϸ�¼�
protected:	
	//�����¼�
	bool OnUserGiveUp(WORD wChairID);
	//�����¼�
	bool OnUserOpenCard(WORD wChairID, BYTE cbCardData[]);
	//�±��¼�
	bool OnUserInvest(WORD wChairID, LONGLONG lInvestCount);
	//��ע�¼�
	bool OnUserAddScore(WORD wChairID, LONGLONG lScore, BYTE cbCode, bool bGiveup=false);	

	//��������
protected:	
	//���Խ�ɢ
	void TryDismissGame();
	//���·���
	void UpdateUserScore(WORD wChairID);
	//��ȡ����
	LONGLONG GetUserScore(WORD wChairID);	
	//��ȡ��ע
	LONGLONG GetMinAddScore(WORD wChairID);
	//�Ƚ��˿�
	int CompareHandCard(WORD wChairID1,WORD wChairID2,tagCompareResult & CompareResult);
	//��¼�쳣
	void WriteErrorLog(tagScoreInfo ScoreInfoArray[], WORD wScoreCount,int nType);
	//��ʾ��Ϣ
	void PopBrowseAction(IServerUserItem *pIUserItem, LPCWSTR szTips, LPCWSTR szUrl,WORD wType);
	
	//��������
protected:
	//����д��־
	VOID WriteControlLog(string & str);
};

//////////////////////////////////////////////////////////////////////////

#endif
