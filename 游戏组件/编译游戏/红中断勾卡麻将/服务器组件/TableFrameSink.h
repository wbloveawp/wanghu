#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "HistoryScore.h"
#include "DlgCustomRule.h"
#include "GameVideo.h"

#define SCOREEX SCORE
#pragma warning(disable : 4244)//////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//Ч������
enum enEstimatKind
{
	EstimatKind_OutCard,			//����Ч��
	EstimatKind_GangCard,			//����Ч��
};


//////////////////////////////////////////////////////////////////////////

//��Ϸ������
//class CTableFrameSink : public ITableFrameSink, public ITableUserAction
class CTableFrameSink : public ITableFrameSink, public ITableUserAction, public IEventBaseScore, public IEventUserScore, public IEventBattleItem, public IEventStartEfficacy, public IEventCustomFrameTime, public ITableControlSendCard
{

	//�������
protected:
	CGameLogic						m_GameLogic;							//��Ϸ�߼�
	CHistoryScore					m_HistoryScore;							//��ʷ�ɼ�
	//ITableFrame						* m_pITableFrame;						//��ܽӿ�
	ITableFrameItem                 * m_pITableFrame;							//��ܽӿ�
	const tagGameServiceOption		* m_pGameServiceOption;					//���ò���
	//tagCustomRule *					m_pGameCustomRule;						//�Զ�����

	tagBattleCreateParam            m_BattleConfig;							// Լս����

	//��Ϸ��Ƶ
protected:
	HINSTANCE						m_hVideoInst;
	IGameVideo*						m_pGameVideo;

	//��������
public:
	//���캯��
	CTableFrameSink();
	//��������
	virtual ~CTableFrameSink();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { }
	//�ӿڲ�ѯ
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	
public://��Ϸ�¼� ITableFrameSink
	//��ʼ��
	virtual bool InitTableFrameSink(IUnknownEx* pIUnknownEx);
	//��λ����
	virtual VOID ResetTableFrameSink();
	//��Ϸ��ʼ
	virtual bool OnEventGameStart();
	//��Ϸ��ͣ
	virtual bool OnEventGamePause() { return true; }
	//��Ϸ����
	virtual bool OnEventGameContinue() { return true; }
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem* pIServerUserItem, BYTE cbReason);
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem* pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);
	//�¼��ӿ�
	//��ʱ���¼�
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//�����¼�
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID* pData, WORD wDataSize) { return false; }
	//�����¼�
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem* pIServerUserItem, BYTE cbReason) { return false; }
	
public://ITableUserAction --�û��¼�
	//�û�����
	virtual bool OnActionUserOffLine(IServerUserItem* pIServerUserItem);
	//�û�����
	virtual bool OnActionUserConnect(IServerUserItem* pIServerUserItem) { return true; }
	//�û�����
	virtual bool OnActionUserSitDown(IServerUserItem* pIServerUserItem, bool bLookonUser);
	//�û�����
	virtual bool OnActionUserStandUp(IServerUserItem* pIServerUserItem, bool bLookonUser);
	//�û�ͬ��
	virtual bool OnActionUserReady(IServerUserItem* pIServerUserItem, VOID* pData, WORD wDataSize);
	//��ѯ�ӿ�
public://�׷ֱ��--IEventBaseScore
	virtual VOID  OnEventBaseScoreVariation(LONG lBaseScore);
public://�׷ֱ��--IEventUserScore
	//�������
	virtual VOID  OnEventScoreVariation(IServerUserItem* pIServerUserItem);

public://��Ϣ���� --IEventBattleItem
	//Ӯȡ����
	virtual SCORE GetBattleWinScore() { return 0; };
	//ͬ������
	virtual WORD GetDismissAgreeCount() { return 0; };
	//ԼսժҪ
	virtual LPCTSTR GetUserBattleAbstract(WORD wChairID) { return 0; };
	//�¼�֪ͨ



	//��ѯ�޶�
	virtual SCOREEX QueryConsumeQuota(IServerUserItem * pIServerUserItem){  return 0; };
	//���ٻ���
	virtual SCOREEX QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//��ѯ�Ƿ�۷����
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

	//�����ӿ�
public:
	//���û���
	virtual void SetGameBaseScore(SCOREEX lBaseScore){};



	//����ӿ�
public:
	//��Ϸ��Ϣ����
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//�����Ϣ����
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//��Ϸ�¼�
protected:
	//�û��й�
	bool OnUserTrustee(WORD wChairID, bool bTrustee);
	//�¼��ӿ�
public:
	//�����ʶ
	virtual WORD GetMasterChairID() { return INVALID_CHAIR; }
	//��ʼ����
	virtual WORD  GetStartPlayerCount();
	//����׼��
	virtual bool  IsAllowPlayerReady(WORD wChairID);


	//�ӿں���
public:
	//��ɢʱ��
	virtual DWORD GetBattleDismissTime() { return 0; };
	//����ʱ��
	virtual DWORD GetUnReadyKickOutTime() { return 0; };
	//��ɢʱ��
	virtual DWORD GetAgreementDismissTime() { return 0; };


	//��Ϣ����
public:

//�¼�֪ͨ
public:
	//���ӽ�ɢ
	virtual VOID OnEventTableDismiss();
	//�����޸�
	virtual VOID OnEventTableModify(LPVOID pModifyParam, WORD wBufferSize);
	//���Ӵ���
	virtual VOID OnEventTableCreated(LPVOID pCreateParam, WORD wBufferSize);

	//���Ʒ���
public:
	//������
	virtual bool OnEventControlSendCard(IServerUserItem* pIServerUserItem, void* pData, WORD wDataSize);


protected:


	
};
//////////////////////////////////////////////////////////////////////////

#endif
