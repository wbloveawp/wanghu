#ifndef TABLE_FRAME_HEAD_FILE
#define TABLE_FRAME_HEAD_FILE

#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//时钟定义
#define IDI_OFF_LINE				(TIME_TABLE_KERNEL_START+0)			//断线标识
#define IDI_KICKOUT_PLAYER			(TIME_TABLE_KERNEL_START+1)			//踢出标识
#define IDI_KICKOUT_LOOKON			(TIME_TABLE_KERNEL_START+2)			//踢出标识

//时间定义
#define TIME_KICKOUT_LOOKON			10L									//提出旁观

//////////////////////////////////////////////////////////////////////////

//数组定义
typedef IServerUserItem * CTableUserItemArray[MAX_CHAIR];				//游戏数组

typedef CWHArray<DWORD> CUserIDArray;									//标识数组
typedef CWHArray<IServerUserItem *> CLookonUserItemArray;				//旁观数组


//////////////////////////////////////////////////////////////////////////////////
//关联参数
struct tagRelationParameter
{	
	DWORD			dwRelationNum;				//关联编号
	SCORE			lRelationRecordID;			//记录标识
};

//关联参数-约战
struct tagRelationParameter_Battle : public tagRelationParameter
{
	DWORD			dwClubID;					//茶社标识	
	BYTE			cbScoreKind;				//积分类型	
};

//关联参数-排位
struct tagRelationParameter_Ranking : public tagRelationParameter
{	
};

//////////////////////////////////////////////////////////////////////////

//桌子框架
class GAME_SERVICE_CLASS CTableFrameItem : public ITableFrameItem, public IGameBalanceControl
{
	//固有属性
protected:
	WORD							m_wTableID;							//桌子号码
	WORD							m_wChairCount;						//椅子数目
	enStartMode						m_GameStartMode;					//开始模式

	//缓冲变量
protected:
	BYTE							m_cbBuffer[SOCKET_PACKET];			//缓冲数组

	//辅助变量
protected:
	DWORD							m_dwLastEnterTime;					//进入时间
	DWORD							m_dwLastLeaveTime;					//离开时间

	//规则变量
public:
	WORD							m_wMaxPlayerCount;					//最大人数
	WORD							m_wSitdownMultiple;					//坐下倍数
	WORD							m_wStandupMultiple;					//起立倍数

	//标志变量
protected:
	bool							m_bVideoData;						//录像数据
	bool							m_bWriteScore;						//写分标志
	bool							m_bGamePaused;						//暂停标志	
	bool							m_bGameStarted;						//开始标志
	bool							m_bTableStarted;					//开始标志
	bool							m_bAllowLookon[MAX_CHAIR];			//旁观标志

	//功能标识
protected:
	bool							m_bSameIpRemind;					//同IP提醒

	//游戏变量
protected:
	LONG							m_lBaseScore;						//游戏底分
	DWORD							m_dwPlayCount;						//游戏局数
	BYTE							m_cbGameStatus;						//游戏状态
	DWORD							m_dwGameStartTime;					//开始时间
	DWORD							m_dwPlayerID[MAX_CHAIR];			//玩家标识
	LONGLONG                        m_lMinEnterScore;                   //最低进入 KK JIA

	//桌子信息
protected:
	DWORD							m_dwTableOwnerID;					//桌主用户
	TCHAR							m_szTablePassword[LEN_PASSWORD];	//桌子密码	

	//平衡控制
public:
	bool							m_bControlCard;						//控制标识
	BYTE							m_cbUserCardLevel[TABLE_USER_COUNT];//扑克级别	

	//积分记录
protected:
	DWORD							m_dwPlayTimeCount;					//游戏时间
	tagScoreInfoEx					m_ScoreInfoArray[MAX_CHAIR];		//游戏积分

	//服务费记录
protected:
	bool							m_bCostGame[MAX_CHAIR];				//扣费标识

	//断线变量
protected:
	WORD							m_wOffLineCount[MAX_CHAIR];			//断线次数
	DWORD							m_dwOffLineTime[MAX_CHAIR];			//断线时间

	//踢人变量
protected:
	DWORD							m_dwUserUnReadyTime[MAX_CHAIR];		//未准备时间

	//离线锁分记录
protected:
	tagOfflineScore					m_OfflineScore[MAX_CHAIR];			//离开积分

	//用户数组
protected:
	CTableUserItemArray				m_TableUserItemArray;				//游戏用户
	CLookonUserItemArray			m_LookonUserItemArray;				//旁观用户

	//内核组件
protected:
	ITimerEngine *					m_pITimerEngine;					//时间引擎
	IDataBaseEngine *				m_pIDataBaseEngine;					//内核数据库
	IDataBaseEngine *				m_pIAssistDataBaseEngine;			//记录数据库

	//录像接口
protected:
	IGameVideoItem *				m_pIGameVideoItem;					//录像接口
	IGameVideoManager *				m_pIGameVideoManager;				//录像管理

	//组件接口
protected:
	ITableFrameSink	*				m_pITableFrameSink;					//桌子接口
	ITableUserAction *				m_pITableUserAction;				//动作接口
	IEventUserScore *				m_pIEventUserScore;					//积分接口
	IEventBaseScore *				m_pIEventBaseScore;					//底分接口	
	ITableUserRequest*				m_pITableUserRequest;				//请求事件
	IGameStockQuery*				m_pIGameStockQuery;					//库存查询
	IServiceFunction *				m_pIServiceFunction;				//功能接口
	IEventStartEfficacy *			m_pIEventStartEfficacy;				//开始校验
	IEventServiceCost *				m_pIEventServiceCost;				//扣税事件
	IEventCustomFrameTime *			m_pIEventCustomFrameTime;			//框架时间
	ITableEventSink *				m_pITableEventSink;					//桌子事件		
	IRobotUserManager *				m_pIRobotUserManager;				//机器管理	
	ITableControlSendCard *			m_pITableControlSendCard;			//配牌器接口

	//配置信息
protected:	
	tagGameServiceAttrib *			m_pGameServiceAttrib;				//服务属性
	tagGameServiceOption *			m_pGameServiceOption;				//服务配置

	//函数定义
public:
	//构造函数
	CTableFrameItem();
	//析构函数
	virtual ~CTableFrameItem();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//配置接口
public:
	//开始模式
	virtual enStartMode GetStartMode() { return m_GameStartMode; }
	//开始模式
	virtual VOID SetStartMode(enStartMode StartMode) { m_GameStartMode=StartMode; }

	//单元积分
public:
	//单元积分
	virtual LONG GetCellScore() { return m_lBaseScore; }
	//单元积分
	virtual VOID SetCellScore(LONG lCellScore);

	//最低进入 KK JIA
	virtual VOID SetMinEnterScore(LONGLONG lMinEnterScore);

	//信息接口
public:
	//游戏状态
	virtual bool IsGameStarted() { return m_bGameStarted; }
	//游戏状态
	virtual bool IsTableStarted() { return m_bTableStarted; }
	//锁定状态
	virtual bool IsTableLocked() { return ((m_szTablePassword[0]!=0) && (!CServerRule::IsForbidPasswordSet(m_pGameServiceOption->dwServerRule))); }

	//状态接口
public:
	//获取状态
	virtual BYTE GetGameStatus() { return m_cbGameStatus; }
	//设置状态
	virtual VOID SetGameStatus(BYTE cbGameStatus) { m_cbGameStatus=cbGameStatus; }

	//属性接口
public:
	//空位置数目
	virtual WORD GetNullChairCount();
	//桌子号码
	virtual WORD GetTableID() { return m_wTableID; }
	//游戏人数
	virtual WORD GetChairCount() { return m_wChairCount; }	
	//游戏时间
	virtual DWORD GetPlayTimeCount() { return (DWORD)time(NULL)-m_dwGameStartTime; }

	//配置参数
public:
	//获取属性
	virtual tagGameServiceAttrib * GetGameServiceAttrib() { return m_pGameServiceAttrib; }
	//获取配置
	virtual tagGameServiceOption * GetGameServiceOption() { return m_pGameServiceOption; }

	//控制接口
public:
	//开始游戏
	virtual bool StartGame();
	//暂停游戏
	virtual bool PauseGame();
	//继续游戏
	virtual bool ContinueGame();
	//解散游戏
	virtual bool DismissGame();
	//结束游戏
	virtual bool ConcludeGame(LPCTSTR pszGameCourse);

	//统计接口
public:
	//计算税收
	virtual SCORE AccountRevenue(SCORE lGameScore);

	//积分接口
public:
	//扣服务费
	virtual VOID CostForGameStart();
	//写入积分
	virtual bool WriteUserScore(WORD wChairID, tagScoreInfo& ScoreInfo);
	//写入积分
	virtual bool WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount);	
	//kk jjia
	//qu zhuo zi canshu
	virtual CMD_GF_TableParam GetTableParam(tagScoreInfo ScoreInfoArray[], WORD wScoreCount);

	//点控接口
public:
	//控制扑克
	virtual bool IsControlCard();
	//扑克等级
	virtual BYTE GetUserCardLevel(WORD wChairID);

  //用户接口
public:
	//寻找用户
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID);
	//游戏用户
	virtual IServerUserItem * GetTableUserItem(WORD wChairID);
	//旁观用户
	virtual IServerUserItem * EnumLookonUserItem(WORD wEnumIndex);

	//时间接口
public:
	//设置时间
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter);
	//删除时间
	virtual bool KillGameTimer(DWORD dwTimerID);

	//网络接口
public:
	//发送数据
	virtual bool SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID);
	//发送数据
	virtual bool SendUserData(IServerUserItem * pIServerUserItem, WORD wSubCmdID, VOID * pData, WORD wDataSize);	
	//发送数据
	virtual bool SendDebugData(IServerUserItem* pIServerUserItem, WORD wDebugKind, WORD wSubCmdID);
	//发送数据
	virtual bool SendDebugData(IServerUserItem* pIServerUserItem, WORD wDebugKind, WORD wSubCmdID, VOID* pData, WORD wDataSize);

	//网络接口
public:
	//发送数据
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID, WORD wMainCmdID = MDM_GF_GAME);
	//发送数据
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID, VOID* pData, WORD wDataSize, WORD wMainCmdID = MDM_GF_GAME);
	//发送数据
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, WORD wMainCmdID = MDM_GF_GAME, BYTE cbDataMask = 0xFF);
	//发送数据
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, VOID* pData, WORD wDataSize, WORD wMainCmdID = MDM_GF_GAME, BYTE cbDataMask = 0xFF);

	//功能接口
public:
	//发送消息
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType);
	//跑马灯公告
	virtual bool SendMarqeeNotice(BYTE cbNoticeKind, VOID* pNoticeContent, WORD wNoticeSize);
	//游戏消息
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);
	//房间消息
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType);	

	//数据接口
public:
	//请求事件
	virtual bool PostDataBaseRequest(WORD wRequestID, DWORD dwEngineKeyID, DWORD dwTokenID);
	//请求事件
	virtual bool PostDataBaseRequest(WORD wRequestID, DWORD dwEngineKeyID, DWORD dwTokenID, VOID* pData, WORD wDataSize);

	//动作处理
public:
	//起立动作
	virtual bool PerformStandUpAction(IServerUserItem * pIServerUserItem, bool bInitiative=false);
	//旁观动作
	virtual bool PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem);
	//坐下动作
	virtual bool PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem);

	//功能接口
public:
	//扣除服务费
	virtual VOID ChargeGameAndReset();
	//离线锁分
	virtual VOID FreezeGameScore(IServerUserItem* pIServerUserItem);
	//模拟配置
	virtual bool ImitateGameOption(IServerUserItem* pIServerUserItem);
	//发送场景
	virtual bool OnEventSendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize);		

	//分数接口
public:
	//分数变更
	virtual VOID OnEventUserScoreChange(IServerUserItem *pIServerUserItem);

	//统计接口
public:
	//用户数目
	virtual WORD GetSitUserCount();
	//真人数目
	virtual WORD GetRealUserCount();
	//机器数目
	virtual WORD GetRobotUserCount();
	//旁观数目
	virtual WORD GetLookonUserCount();
	//断线数目
	virtual WORD GetOffLineUserCount();

	//最大人数
public:
	//获取人数
	virtual WORD GetMaxPlayerCount() { return m_wMaxPlayerCount; }
	//设置人数
	virtual VOID SetMaxPlayerCount(WORD wPlayerCount) { m_wMaxPlayerCount = wPlayerCount; }

	//游戏倍数
public:
	//坐下倍数
	virtual VOID SetSitdownGameMultiple(WORD wGameMultiple);
	//起立倍数
	virtual VOID SetStandupGameMultiple(WORD wGameMultiple);

	//关联信息
public:
	//关联椅子
	virtual WORD GetRelationChairID(DWORD dwUserID);
	//关联参数
	virtual tagRelationParameter * GetRelationParameter();

	//信息函数
public:
	//桌子密码
	LPCTSTR GetTablePassword() { return m_szTablePassword; }

	//记录函数
public:
	//记录录像
	bool RecordGameVideo();
	//记录分数
	bool RecordGameScore(LPCTSTR pszGameCourse);	

	//功能函数
public:
	//获取空位
	WORD GetNullChairID();
	//随机空位
	WORD GetRandNullChairID();				
	//未准备数目
	WORD GetUnReadyUserCount();	
	//配置桌子
	bool InitTableFrame(WORD wTableID, tagTableFrameParameter & TableFrameParameter);	

	//功能函数
public:		
	//校验密码
	bool ContrastPassword(LPCTSTR pszPassword);	
	//用户状况
	WORD GetTableUserInfo(tagTableUserInfo & TableUserInfo);		
	//重连动作
	bool PerformConnectAction(IServerUserItem *pIServerUserItem);

	//用户事件
public:
	//用户规则事件
	bool OnEventUserRule(IServerUserItem *pIServerUserItem);
	//断线事件
	bool OnEventUserOffLine(IServerUserItem * pIServerUserItem);
	//重进事件
	bool OnEventUserReConnect(IServerUserItem * pIServerUserItem);

	//游戏事件
public:
	//尝试开始
	virtual bool TryStartGame();
	//开始事件
	virtual bool OnEventGameStart();
	//结束时间
	virtual bool OnEventGameConclude();

	//用户事件
public:
	//用户坐下
	virtual bool OnActionUserSitdown(IServerUserItem* pIServerUserItem);
	//用户起立
	virtual bool OnActionUserStandup(IServerUserItem* pIServerUserItem);
	
	//用户事件
public:
	//旁观校验
	virtual bool EfficacyUserLookon(WORD wChairID, IServerUserItem* pIServerUserItem);
	//坐下校验
	virtual bool EfficacyUserSitdown(WORD wChairID, IServerUserItem* pIServerUserItem);	
	//起立校验
	virtual bool EfficacyUserStandup(WORD wChairID, IServerUserItem* pIServerUserItem, bool bInitiative);

	//事件函数
public:
	//时间事件
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter);	
	//游戏选项
	virtual VOID OnEventClientReady(IServerUserItem* pIServerUserItem);	
	//桌子初始化
	virtual VOID OnEventTableFrameInit(tagTableFrameParameter& TableFrameParameter);	

	//系统事件
public:
	//游戏事件
	virtual bool OnEventSocketGame(WORD wSubCmdID, VOID* pData, WORD wDataSize, IServerUserItem* pIServerUserItem);
	//框架事件
	virtual bool OnEventSocketFrame(WORD wSubCmdID, VOID* pData, WORD wDataSize, IServerUserItem* pIServerUserItem);

	//配牌接口
public:
	//配牌器
	virtual bool ControlSendCard(IServerUserItem * pIServerUserItem, void * pData, WORD wDataSize);

	//辅助函数
protected:
	//桌子状态
	bool SendTableStatus();	
	//开始状态
	bool SendStartStatus();			
	//请求失败
	bool SendRequestFailure(IServerUserItem * pIServerUserItem, LPCTSTR pszDescribe, BYTE cbFailureCode);
	//版本检查
	bool PerformCheckVersion(DWORD dwFrameVersion, DWORD dwClientVersion, IServerUserItem * pIServerUserItem);

	//效验函数
public:
	//开始效验
	bool EfficacyStartGame(WORD wReadyChairID);
	//地址效验
	bool EfficacyAddrRule(IServerUserItem * pIServerUserItem);
	//积分效验
	bool EfficacyScoreRule(IServerUserItem * pIServerUserItem);
	//桌子效验
	bool EfficacyTableRule(IServerUserItem * pIServerUserItem);

	//辅助函数
protected:	
	//计算等级
	VOID CalcUserCardLevel();
	//游戏记录标识
	SCORE BuildGameRecordID();				
	//录像频道
	VOID BuildVideoChannel(TCHAR szBuffer[], WORD wBufferLen);	
};

//////////////////////////////////////////////////////////////////////////

#endif