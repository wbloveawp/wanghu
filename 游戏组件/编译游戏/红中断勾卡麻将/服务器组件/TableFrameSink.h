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
//枚举定义

//效验类型
enum enEstimatKind
{
	EstimatKind_OutCard,			//出牌效验
	EstimatKind_GangCard,			//杠牌效验
};


//////////////////////////////////////////////////////////////////////////

//游戏桌子类
//class CTableFrameSink : public ITableFrameSink, public ITableUserAction
class CTableFrameSink : public ITableFrameSink, public ITableUserAction, public IEventBaseScore, public IEventUserScore, public IEventBattleItem, public IEventStartEfficacy, public IEventCustomFrameTime, public ITableControlSendCard
{

	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑
	CHistoryScore					m_HistoryScore;							//历史成绩
	//ITableFrame						* m_pITableFrame;						//框架接口
	ITableFrameItem                 * m_pITableFrame;							//框架接口
	const tagGameServiceOption		* m_pGameServiceOption;					//配置参数
	//tagCustomRule *					m_pGameCustomRule;						//自定规则

	tagBattleCreateParam            m_BattleConfig;							// 约战配置

	//游戏视频
protected:
	HINSTANCE						m_hVideoInst;
	IGameVideo*						m_pGameVideo;

	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { }
	//接口查询
	virtual void * QueryInterface(const IID & Guid, DWORD dwQueryVer);

	
public://游戏事件 ITableFrameSink
	//初始化
	virtual bool InitTableFrameSink(IUnknownEx* pIUnknownEx);
	//复位桌子
	virtual VOID ResetTableFrameSink();
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏暂停
	virtual bool OnEventGamePause() { return true; }
	//游戏继续
	virtual bool OnEventGameContinue() { return true; }
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem* pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem* pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);
	//事件接口
	//定时器事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID* pData, WORD wDataSize) { return false; }
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem* pIServerUserItem, BYTE cbReason) { return false; }
	
public://ITableUserAction --用户事件
	//用户断线
	virtual bool OnActionUserOffLine(IServerUserItem* pIServerUserItem);
	//用户重入
	virtual bool OnActionUserConnect(IServerUserItem* pIServerUserItem) { return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(IServerUserItem* pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(IServerUserItem* pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserReady(IServerUserItem* pIServerUserItem, VOID* pData, WORD wDataSize);
	//查询接口
public://底分变更--IEventBaseScore
	virtual VOID  OnEventBaseScoreVariation(LONG lBaseScore);
public://底分变更--IEventUserScore
	//分数变更
	virtual VOID  OnEventScoreVariation(IServerUserItem* pIServerUserItem);

public://信息函数 --IEventBattleItem
	//赢取分数
	virtual SCORE GetBattleWinScore() { return 0; };
	//同意人数
	virtual WORD GetDismissAgreeCount() { return 0; };
	//约战摘要
	virtual LPCTSTR GetUserBattleAbstract(WORD wChairID) { return 0; };
	//事件通知



	//查询限额
	virtual SCOREEX QueryConsumeQuota(IServerUserItem * pIServerUserItem){  return 0; };
	//最少积分
	virtual SCOREEX QueryLessEnterScore(WORD wChairID, IServerUserItem * pIServerUserItem);
	//查询是否扣服务费
	virtual bool QueryBuckleServiceCharge(WORD wChairID){return true;}

	//比赛接口
public:
	//设置基数
	virtual void SetGameBaseScore(SCOREEX lBaseScore){};



	//网络接口
public:
	//游戏消息处理
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//游戏事件
protected:
	//用户托管
	bool OnUserTrustee(WORD wChairID, bool bTrustee);
	//事件接口
public:
	//管理标识
	virtual WORD GetMasterChairID() { return INVALID_CHAIR; }
	//开始人数
	virtual WORD  GetStartPlayerCount();
	//允许准备
	virtual bool  IsAllowPlayerReady(WORD wChairID);


	//接口函数
public:
	//解散时间
	virtual DWORD GetBattleDismissTime() { return 0; };
	//踢人时间
	virtual DWORD GetUnReadyKickOutTime() { return 0; };
	//解散时间
	virtual DWORD GetAgreementDismissTime() { return 0; };


	//信息函数
public:

//事件通知
public:
	//桌子解散
	virtual VOID OnEventTableDismiss();
	//桌子修改
	virtual VOID OnEventTableModify(LPVOID pModifyParam, WORD wBufferSize);
	//桌子创建
	virtual VOID OnEventTableCreated(LPVOID pCreateParam, WORD wBufferSize);

	//控制发牌
public:
	//配牌器
	virtual bool OnEventControlSendCard(IServerUserItem* pIServerUserItem, void* pData, WORD wDataSize);


protected:


	
};
//////////////////////////////////////////////////////////////////////////

#endif
