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
	//游戏规则（房卡和服务端配置）
protected:
	bool							m_bHuanSanZhang;						//true为允许换三张
	bool							m_bHuJiaoZhuanYi;						//true允许呼叫转移
	bool							m_bZiMoAddTimes;						//true允许自摸加倍
	bool							m_bTianDiHu;							//true为允许天地胡加倍
	bool							m_bHaiDiLaoYue;							//true允许海底捞月加倍
	bool							m_bMenQing;								//true允许门清加倍
	CMD_S_RECORD					m_stRecord;								//游戏记录(房卡结算显示)
	tagChart						m_tagHistoryChart[GAME_PLAYER][MAX_CHART_COUNT];//对局流水清单
	BYTE							m_cbHistoryChartCount[GAME_PLAYER];		//对局流水清单数目
	CMD_S_RoomCardRecord			m_RoomCardRecord;						//房卡游戏记录
	std::vector<SCOREEX>			m_vecRecord[GAME_PLAYER];
	SCOREEX							m_lCurTotalWinloseScore[GAME_PLAYER];	//当局游戏总输赢记录
	//
	CMD_S_GameEnd					m_stGameEnd;							//结算数据缓存

		//属性变量
protected:
	static const WORD				m_wPlayerCount;							//游戏人数

	//游戏变量
protected:
	SCOREEX							m_lCellScore;							//游戏底分
	LONG							m_lSiceCount;							//骰子点数
	WORD							m_wBankerUser;							//庄家用户
	WORD							m_wFirstWinUser;						//首赢用户
	SCOREEX							m_lGameScore[GAME_PLAYER];				//游戏得分
	BYTE							m_cbCardIndex[GAME_PLAYER][MAX_INDEX];	//用户扑克
	bool							m_bTrustee[GAME_PLAYER];				//是否托管
	bool							m_bPlayStatus[GAME_PLAYER];				//玩家状态
	BYTE							m_cbWinCount;							//赢的倍率
	WORD							m_wWinOrder[GAME_PLAYER];				//胡牌顺序
	SCOREEX							m_lGameTax[GAME_PLAYER];				//游戏税率
	tagGangScore					m_GangScore[GAME_PLAYER];				//杠牌分数
	BYTE							m_cbTrusteeDelayTime;					//托管延迟时间
	CTime							m_ctChangeTime;							//切换时间
	CTime							m_ctStartTime;							//服务端开始时间

	//换三张
protected:
	BYTE							m_cbChangeType;
	BYTE							m_cbChangeCardDataBefore[GAME_PLAYER][MAX_CHANGE_CARDCOUNT];	//换三张扑克(换之前)
	BYTE							m_cbChangeCardDataAfter[GAME_PLAYER][MAX_CHANGE_CARDCOUNT];		//换三张扑克(换之后)
	bool							m_bChangeCard[GAME_PLAYER];				//换三张标识


	//缺门信息
protected:
	bool							m_bCallCard[GAME_PLAYER];				//用户状态
	BYTE							m_cbCallCard[GAME_PLAYER];				//用户缺门

	//出牌信息
protected:
	WORD							m_wOutCardUser;							//出牌用户
	BYTE							m_cbOutCardData;						//出牌扑克
	BYTE							m_cbOutCardCount;						//出牌数目
	BYTE							m_cbDiscardCount[GAME_PLAYER];			//丢弃数目
	BYTE							m_cbDiscardCard[GAME_PLAYER][60];		//丢弃记录

	//发牌信息
protected:
	BYTE							m_cbSendCardData;						//发牌扑克
	BYTE							m_cbSendCardCount;						//发牌数目
	BYTE							m_cbLeftCardCount;						//剩余数目
	BYTE							m_cbRepertoryCard[MAX_REPERTORY];		//库存扑克

#ifdef CARD_CONFIG
	BYTE							m_cbconfigCard[MAX_REPERTORY];			//配牌扑克
#endif

	//堆立变量
protected:
	WORD							m_wHeapHead;							//堆立头部
	WORD							m_wHeapTail;							//堆立尾部
	BYTE							m_cbMinusHeadCount;						//头部空缺
	BYTE							m_cbMinusLastCount;						//尾部空缺
	BYTE							m_cbHeapCardInfo[GAME_PLAYER][2];		//堆牌信息

	//运行变量
protected:
	WORD							m_wResumeUser;							//还原用户
	WORD							m_wCurrentUser;							//当前用户
	WORD							m_wProvideUser;							//供应用户
	BYTE							m_cbProvideCard;						//供应扑克

	//状态变量
protected:
	bool							m_bSendStatus;							//发牌状态
	bool							m_bGangStatus;							//抢杆状态
	WORD							m_wProvideGang;							//引杠用户
	bool							m_bGangOutStatus;						//杠牌状态
	bool							m_bEnjoinChiHu[GAME_PLAYER];			//禁止吃胡
	bool							m_bEnjoinChiPeng[GAME_PLAYER];			//禁止吃碰

	//用户状态
public:
	bool							m_bResponse[GAME_PLAYER];				//响应标志
	BYTE							m_cbUserAction[GAME_PLAYER];			//用户动作
	BYTE							m_cbOperateCard[GAME_PLAYER];			//操作扑克
	BYTE							m_cbPerformAction[GAME_PLAYER];			//执行动作

	//组合扑克
protected:
	BYTE							m_cbWeaveItemCount[GAME_PLAYER];		//组合数目
	tagWeaveItem					m_WeaveItemArray[GAME_PLAYER][MAX_WEAVE];//组合扑克

	//结束信息
protected:
	BYTE							m_cbChiHuCard;							//吃胡扑克
	BYTE							m_cbChiHuCardArray[GAME_PLAYER];		//吃胡扑克数组
	DWORD							m_dwChiHuKind[GAME_PLAYER];				//吃胡结果
	CChiHuRight						m_ChiHuRight[GAME_PLAYER];				//吃胡权位
	WORD							m_wProvider[GAME_PLAYER];				//引胡用户
	bool							m_bWaitWriteScore[GAME_PLAYER];			//等待写分标志

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

	//管理接口
public:
	//初始化
	virtual bool InitTableFrameSink(IUnknownEx* pIUnknownEx);
	//复位桌子
	virtual VOID ResetTableFrameSink();

	//查询接口
public:
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


	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart();
	//游戏暂停
	virtual bool OnEventGamePause() { return true; }
	//游戏继续
	virtual bool OnEventGameContinue() { return true; }

	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason);
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret);

	//事件接口
public:
	//定时器事件
	virtual bool OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//数据事件
	virtual bool OnDataBaseMessage(WORD wRequestID, VOID * pData, WORD wDataSize) { return false; }
	//积分事件
	virtual bool OnUserScroeNotify(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason) { return false; }

	//网络接口
public:
	//游戏消息处理
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);

	//用户事件
public:
	//用户断线
	virtual bool OnActionUserOffLine(IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool OnActionUserConnect(IServerUserItem* pIServerUserItem) { return true; }
	//用户坐下
	virtual bool OnActionUserSitDown(IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool OnActionUserStandUp(IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool OnActionUserReady(IServerUserItem* pIServerUserItem, VOID* pData, WORD wDataSize);


	//游戏事件
protected:
	//用户选缺
	bool OnUserCallCard(WORD wChairID, BYTE cbCallCard);
	//用户出牌
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData, bool bClient = false);
	//用户操作
	bool OnUserOperateCard(WORD wChairID, BYTE cbOperateCode, BYTE cbOperateCard);
	//用户换三张
	bool OnUserChangeCard(WORD wChairID, BYTE cbChangeCardData[MAX_CHANGE_CARDCOUNT], bool bClient = false);
	//用户托管
	bool OnUserTrustee(WORD wChairID, bool bTrustee);
	//辅助函数
protected:
	//发送操作
	bool SendOperateNotify();
	//派发扑克
	bool DispatchCardData(WORD wCurrentUser, bool bTail = false);
	//响应判断
	bool EstimateUserRespond(WORD wCenterUser, BYTE cbCenterCard, enEstimatKind EstimatKind);
	//吃胡用户
	SCOREEX ProcessChiHuUser(WORD wChairId, WORD &wOutFanShu);
	//过虑权位
	void FiltrateRight(WORD wChairId, CChiHuRight &chr);
	//获取吃胡番数
	WORD GetChiHuFanCount(const CChiHuRight & ChiHuRight);
	//写日志文件
	void WriteInfo(LPCTSTR pszString, const TCHAR szServerName[LEN_SERVER]);

protected:
	//发送听牌数据，打哪些牌胡哪些牌 
	void SendTingData(WORD wChairID, const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount);
	//获取指定玩家某张牌未出数量
	BYTE GetRemainingCount(WORD wChairID, BYTE cbCardData);

protected:
	//一对一赔付计算
	//一对多赔付计算
	//多对一赔付计算
	void ChangeUserPaidScore(SCOREEX lUserGameScore[GAME_PLAYER]);
	//流局判定
	bool IsLiuju();
	//更新玩家状态
	void UpdateUserStatus();


	//事件接口
public:
	//管理标识
	virtual WORD GetMasterChairID() { return INVALID_CHAIR; }
	//开始人数
	virtual WORD  GetStartPlayerCount();
	//允许准备
	virtual bool  IsAllowPlayerReady(WORD wChairID);
	//底分变更
	virtual VOID  OnEventBaseScoreVariation(LONG lBaseScore);
	//分数变更
	virtual VOID  OnEventScoreVariation(IServerUserItem* pIServerUserItem);

	//接口函数
public:
	//解散时间
	virtual DWORD GetBattleDismissTime();
	//踢人时间
	virtual DWORD GetUnReadyKickOutTime();
	//解散时间
	virtual DWORD GetAgreementDismissTime();


	//信息函数
public:
	//赢取分数
	virtual SCORE GetBattleWinScore() { return 0; }
	//同意人数
	virtual WORD GetDismissAgreeCount();
	//约战摘要
	virtual LPCTSTR GetUserBattleAbstract(WORD wChairID) { return L""; }

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

	};
//////////////////////////////////////////////////////////////////////////

#endif
