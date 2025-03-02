#ifndef BATTLE_TABLE_FRAME_HEAD_FILE
#define BATTLE_TABLE_FRAME_HEAD_FILE

//引入文件
#include "BattleServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//约战状态
#define BATTLE_STATE_FREE			0									//空闲状态
#define BATTLE_STATE_WAIT			1									//等待状态
#define BATTLE_STATE_PLAY			2									//游戏状态

//时钟定义
#define IDI_TABLE_DISMISS			(TIME_TABLE_EXTEND_START+0)			//桌子解散
#define IDI_AGREEMENT_DISMISS		(TIME_TABLE_EXTEND_START+1)			//协议解散

//////////////////////////////////////////////////////////////////////////

typedef CWHArray<tagBattleUserItem> CBattleUserItemArray;				//约战玩家

//////////////////////////////////////////////////////////////////////////

//约战桌子
class CTableFrameBattle : public CTableFrameItem, public IEventBattleSink
{
	//约战标志
protected:
	bool							m_bDeductCost;						//扣费标识
	bool							m_bReturnCost;						//退费标识
	bool							m_bDismissNotify;					//解散通知
	bool							m_bBattleService;					//服务状态
	bool							m_bBattleFinish;					//约战结束	

	//茶舍信息
public:
	DWORD							m_dwClubID;							//群组标识
	WORD							m_wSwitchOption;					//群组规则
	WORD							m_wClubServiceID;					//服务标识
	DWORD							m_dwClubCreaterID;					//群主标识

	//约战属性
protected:	
	DWORD							m_dwConfigID;						//玩法标识
	DWORD							m_dwBattleRule;						//约战规则
	WORD							m_wFinishCount;						//完成局数
	BYTE							m_cbBattleState;					//约战状态
	DWORD							m_dwBattleNum;						//映射编号
	DWORD							m_dwStartTime;						//开始时间
	DWORD							m_dwCreateTime;						//创建时间	
	SCORE							m_lBattleRecordID;					//记录标识
	tagBattleConfigEx				m_BattleConfig;						//约战配置	

	//约战成绩
protected:
	CUserIDArray					m_ChargeUserArray;					//扣费玩家
	CBattleUserItemArray			m_BattleUserItemArray;				//玩家数组

	//协议解散
protected:
	DWORD							m_dwLaunchTime;						//发起时间
	DWORD							m_dwLaunchUserID;					//发起用户
	BYTE							m_cbBallotResult[MAX_CHAIR];		//投票结果

	//组件接口
protected:
	IEventBattleItem*				m_pIEventBattleItem;				//约战子项
	IEventGameOption*				m_pIEventGameOption;				//配置接口
	ITCPSocketService*				m_pITCPSocketBattle;				//约战服务	

	//函数定义
public:
	//构造函数
	CTableFrameBattle();
	//析构函数
	virtual~CTableFrameBattle();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//信息函数
public:	
	//关联椅子
	virtual WORD GetRelationChairID(DWORD dwUserID);
	//关联参数
	virtual tagRelationParameter* GetRelationParameter();

	//统计接口
public:
	//计算税收
	virtual SCORE AccountRevenue(SCORE lGameScore);

	//积分接口
public:
	//写入积分
	virtual bool WriteUserScore(WORD wChairID, tagScoreInfo& ScoreInfo);
	//写入积分
	virtual bool WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount);

	//约战接口
public:
	//约战结束
	virtual bool BattleFinish(BYTE cbReason);
	//获取数组
	virtual bool GetOptionValueArray(WORD wOptionIndex, COptionValueArray& OptionValueArray);	

	//约战规则
public:
	//约战规则
	virtual DWORD GetBattleRule() { return m_dwBattleRule; }
	//约战规则
	virtual VOID SetBattleRule(DWORD dwAddRules, DWORD dwRemoveRules);

	//游戏事件
public:
	//尝试启动
	virtual bool TryStartGame();
	//开始事件
	virtual bool OnEventGameStart();
	//结束时间
	virtual bool OnEventGameConclude();

	//用户事件
public:
	//坐下事件
	virtual bool OnActionUserSitdown(IServerUserItem* pIServerUserItem);
	//起立事件
	virtual bool OnActionUserStandup(IServerUserItem* pIServerUserItem);

	//校验事件
public:
	//旁观校验
	virtual bool EfficacyUserLookon(WORD wChairID, IServerUserItem* pIServerUserItem);
	//坐下校验
	virtual bool EfficacyUserSitdown(WORD wChairID, IServerUserItem* pIServerUserItem);
	//起立校验
	virtual bool EfficacyUserStandup(WORD wChairID, IServerUserItem* pIServerUserItem, bool bInitiative);

	//系统事件
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);
	//终端准备
	virtual VOID OnEventClientReady(IServerUserItem* pIServerUserItem);	
	//桌子初始化
	virtual VOID OnEventTableFrameInit(tagTableFrameParameter& TableFrameParameter);

	//系统事件
public:
	//游戏事件
	virtual bool OnEventSocketGame(WORD wSubCmdID, VOID* pData, WORD wDataSize, IServerUserItem* pIServerUserItem);
	//框架事件
	virtual bool OnEventSocketFrame(WORD wSubCmdID, VOID* pData, WORD wDataSize, IServerUserItem* pIServerUserItem);

	//通知函数
public:
	//桌子成绩
	bool NotifyTableGrade();
	//用户列表
	bool NotifyTableUserList();
	//约战战绩
	bool NotifyTableBattle(bool bRecordBattle);
	//桌子参数
	bool NotifyTableParam(WORD wChairID = INVALID_CHAIR);
	//桌子配置
	bool NotifyTableConfig(WORD wChairID = INVALID_CHAIR);	

	//桌子操作
public:
	//创建桌子
	bool CreateBattleTable(tagBattleCreateParam& CreateParam);
	//修改桌子
	bool ModifyBattleTable(tagBattleModifyParam& ModifyParam);
	//桌子信息
	void GetBattleTableItem(tagBattleTableItemEx& TableItem);	

	//功能函数
public:
	//设置状态
	void SetBattleService(bool bBattleService);	
	//约战状态
	BYTE GetBattleState() { return m_cbBattleState; }
	//约战配置
	tagBattleConfigEx* GetBattleConfig() { return &m_BattleConfig; }

	//解散桌子
public:
	//解散桌子
	bool DismissBattleTable(DWORD dwUserID);
	//解散桌子
	void DismissBattleTable(bool bRecordBattle);

	//费用处理
public:
	//扣费结果
	bool DeductCostResult(bool bDeductCostSuccess, DWORD dwPayUserID, DWORD dwFailUserID);

	//辅助函数
protected:
	//完成判断
	bool IsBattleFinished();
	//记录标识
	SCORE BuildBattlleRecordID();
	//退还费用
	bool PerformReturnCost(WORD wPayType);
	//计算税收
	SCORE CalcCustomRevenue(SCORE lGameScore);
	//获取用户
	IServerUserItem* GetFirstSitUserItem();	
	//获取用户
	tagBattleUserItem* GetBattleUserItem(DWORD dwUserID);
	//解散提醒
	bool SendDimissNotify(IServerUserItem* pIServerUserItem);	
	//校验配置
	bool VerifyBattleConfig(tagBattleConfig* pBattleConfig, bool bOnlyVerify);
	//执行投票
	bool PerformBallotDismiss(WORD wChairID, DWORD dwUserID, bool bAgreeDismiss);
};

//////////////////////////////////////////////////////////////////////////

#endif