#ifndef TABLE_FRAME_SINK_HEAD_FILE
#define TABLE_FRAME_SINK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "log_writer.h"

//////////////////////////////////////////////////////////////////////////

////发送配牌
//struct tagConfigCard
//{
//	BOOL									bAllotFlag;									//配牌标识
//	BYTE									cbCardData[54];//扑克数据
//};

//比牌结果
struct tagCompareResult
{
	int							nResult;					//最终结果
	int							nHeadResult;				//头牌结果
	int							nTailResult;				//尾牌结果
};

//////////////////////////////////////////////////////////////////////////

//游戏桌子类
class CTableFrameSink : public ITableFrameSink, public ITableUserAction, public IEventBaseScore, public IEventUserScore, public IEventBattleItem, public IEventStartEfficacy, public IEventCustomFrameTime, public ITableControlSendCard
{
	//游戏变量
protected:
	BYTE							m_cbPlayerStatus[GAME_PLAYER];			//游戏状态
	BYTE							m_cbShowHand[GAME_PLAYER];				//全压状态
	BYTE							m_cbOpenFinish[GAME_PLAYER];			//开牌状态
	bool							m_bInvestFinish[GAME_PLAYER];			//完成标志
	bool							m_bUserReConnect[GAME_PLAYER];			//断线重连
	WORD							m_wFirstUser;							//首叫用户
	WORD							m_wBankerUser;							//庄家用户
	WORD							m_wCurrentUser;							//当前用户
	WORD                           m_wSetRuleUser;							//设置用户
	bool							m_bGameRule;							//规则标志
	LONGLONG						m_lCellScore;							//单元下注
	WORD							m_wOperaCount;							//操作次数
	BYTE							m_cbBalanceCount;						//下注轮数
	LONGLONG						m_lBalanceScore;						//平衡数目
	LONGLONG						m_lUserScore[GAME_PLAYER];				//用户金币
	LONGLONG						m_lTableScore[GAME_PLAYER];				//下注数目
	LONGLONG						m_lTotalScore[GAME_PLAYER];				//总注数目
	LONGLONG						m_lLoseScore[GAME_PLAYER];				//输分数目
	LONGLONG						m_lTurnMaxScore;						//最大下注
	LONGLONG						m_lTurnMinScore;						//最小下注
	LONGLONG						m_lMinAddScore;							//最下加注
	LONGLONG                        m_lUserBeginScore[GAME_PLAYER];			//开始分数(测试用)
	LONGLONG                        m_lUserFreeScore[GAME_PLAYER];			//用户积分
	WORD							m_wBattleCount;							//约战局数

		//税收配置 kk jia 低于1时不收税，1-2之间收 lWinnerRevenue1，2-3之间收 lWinnerRevenue2，高于2 收lWinnerRevenue3税
	SCORE							m_lWinnerScore1;						//赢家分数
	SCORE							m_lWinnerScore2;						//赢家分数
	SCORE							m_lWinnerRevenue1;					//赢家税收	

	SCORE							m_lWinnerScore3;						//赢家分数

	SCORE							m_lWinnerRevenue2;					//赢家税收	

	SCORE							m_lWinnerRevenue3;					//赢家税收

	SCORE                           m_lMinBetValue;//最低下注kk jia

//控制变量
protected:
	bool							m_bAllotCard;									//配牌状态
	WORD							m_wControBankerUser;							//控制庄家
	BYTE							m_cbControlCardData[GAME_PLAYER][MAX_COUNT];	//字牌数据

	//变量定义
protected:
	DWORD							m_dwDelayTime;							//延时时间

	//配置变量
protected:
	tagCustomConfig					m_CustomConfig;							//自定义配置
	tagBattleCreateParam            m_BattleConfig;							// 约战配置

	//扑克变量
protected:
	BYTE							m_cbHandCardCount[GAME_PLAYER];			//手牌数量
	BYTE							m_cbHandCardData[GAME_PLAYER][MAX_COUNT];//桌面扑克
	BYTE							m_cbZhuoMianCardData[28];//桌面扑克

	//组件变量
protected:
	CGameLogic						m_GameLogic;							//游戏逻辑	
	ITableFrameItem *				m_pITableFrame;							//框架接口
	IEventBattleSink *				m_pIEventBattleSink;					//约战接口	
	tagGameServiceOption			* m_pGameServiceOption;					//配置参数	
	IGameServiceDynamicRule			* m_pIGameServiceDynamicRule;			//规则接口

	//属性变量
protected:
	static const WORD				m_wPlayerCount;							//游戏人数
	
	//函数定义
public:
	//构造函数
	CTableFrameSink();
	//析构函数
	virtual ~CTableFrameSink();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//是否有效
	virtual bool __cdecl IsValid() { return AfxIsValidAddress(this,sizeof(CTableFrameSink))?true:false; }
	//接口查询
	virtual void *  __cdecl QueryInterface(const IID & Guid, DWORD dwQueryVer);

	//管理接口
public:
	//复位桌子
	virtual VOID ResetTableFrameSink();
	//配置桌子
	virtual bool InitTableFrameSink(IUnknownEx * pIUnknownEx);

	//游戏状态
	virtual bool  __cdecl IsUserPlaying(WORD wChairID);

	//游戏事件
public:
	//游戏开始
	virtual bool OnEventGameStart() ;
	//游戏暂停
	virtual bool OnEventGamePause() { return true; }
	//游戏继续
	virtual bool OnEventGameContinue() { return true; }
	//游戏结束
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason) ;
	//发送场景
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret) ;


	//事件接口
public:
	//定时器事件
	virtual bool  __cdecl OnTimerMessage(DWORD wTimerID, WPARAM wBindParam);
	//游戏消息处理
	virtual bool  __cdecl OnGameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	//框架消息处理
	virtual bool  __cdecl OnFrameMessage(WORD wSubCmdID, VOID * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem);
	

	//用户事件
public:
	//用户断线
	virtual bool __cdecl OnActionUserOffLine(IServerUserItem * pIServerUserItem);
	//用户重入
	virtual bool __cdecl OnActionUserConnect(IServerUserItem * pIServerUserItem);
	//用户坐下
	virtual bool __cdecl OnActionUserSitDown(IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户起立
	virtual bool __cdecl OnActionUserStandUp(IServerUserItem * pIServerUserItem, bool bLookonUser);
	//用户同意
	virtual bool __cdecl OnActionUserReady(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);

	//事件接口
public:		
	//管理标识
	virtual WORD GetMasterChairID() { return INVALID_CHAIR; }
	//开始人数
	virtual WORD __cdecl GetStartPlayerCount();
	//允许准备
	virtual bool __cdecl IsAllowPlayerReady(WORD wChairID);		
	//底分变更
	virtual VOID __cdecl OnEventBaseScoreVariation(LONG lBaseScore);
	//分数变更
	virtual VOID __cdecl OnEventScoreVariation(IServerUserItem *pIServerUserItem);

	//接口函数
public:
	//解散时间
	virtual DWORD GetBattleDismissTime() ;
	//踢人时间
	virtual DWORD GetUnReadyKickOutTime() ;
	//解散时间
	virtual DWORD GetAgreementDismissTime() ;


	//信息函数
public:
	//赢取分数
	virtual SCORE GetBattleWinScore() { return 0; }
	//同意人数
	virtual WORD GetDismissAgreeCount() ;
	//约战摘要
	virtual LPCTSTR GetUserBattleAbstract(WORD wChairID) { return L""; }

	//事件通知
public:
	//桌子解散
	virtual VOID OnEventTableDismiss() ;
	//桌子修改
	virtual VOID OnEventTableModify(LPVOID pModifyParam, WORD wBufferSize) ;
	//桌子创建
	virtual VOID OnEventTableCreated(LPVOID pCreateParam, WORD wBufferSize) ;

	//控制发牌
public:
	//配牌器
	virtual bool OnEventControlSendCard(IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize);

	//辅助函数
public:
	//加注信息
	void SendUserAddInfo(WORD wAddUser,LONGLONG lScore,WORD wNextUser,LONGLONG lMinScore,LONGLONG lMaxScore);
	//发牌信息
	void SendSendCardInfo(BYTE cbCardCount,WORD wCurrentUser);
	//最大下注
	LONGLONG GetMaxUserCanLostScore( WORD wChairID);

	//辅助函数
protected:
	//关闭定时器
	void KillUserGameTimer(WORD wChairID);
	//设置定时器
	void SetUserGameTimer(WORD wChairID,DWORD dwTimeOut,DWORD dwDelayTime);

	//游戏事件
protected:	
	//放弃事件
	bool OnUserGiveUp(WORD wChairID);
	//开牌事件
	bool OnUserOpenCard(WORD wChairID, BYTE cbCardData[]);
	//下本事件
	bool OnUserInvest(WORD wChairID, LONGLONG lInvestCount);
	//加注事件
	bool OnUserAddScore(WORD wChairID, LONGLONG lScore, BYTE cbCode, bool bGiveup=false);	

	//辅助函数
protected:	
	//尝试解散
	void TryDismissGame();
	//更新分数
	void UpdateUserScore(WORD wChairID);
	//获取分数
	LONGLONG GetUserScore(WORD wChairID);	
	//获取加注
	LONGLONG GetMinAddScore(WORD wChairID);
	//比较扑克
	int CompareHandCard(WORD wChairID1,WORD wChairID2,tagCompareResult & CompareResult);
	//记录异常
	void WriteErrorLog(tagScoreInfo ScoreInfoArray[], WORD wScoreCount,int nType);
	//提示信息
	void PopBrowseAction(IServerUserItem *pIUserItem, LPCWSTR szTips, LPCWSTR szUrl,WORD wType);
	
	//辅助函数
protected:
	//测试写日志
	VOID WriteControlLog(string & str);
};

//////////////////////////////////////////////////////////////////////////

#endif
