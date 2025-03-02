#ifndef SHARE_STOCK_MANAGER_HEAD_FILE
#define SHARE_STOCK_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "MonitorServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//类型声明
class CShareMutex;

//////////////////////////////////////////////////////////////////////////////////
//常量定义
#define MAX_STOCK_GAME					50							//游戏数量	
#define MAX_STOCK_KIND					1							//类型数量
#define MAX_STOCK_ITEM					12							//子类数量

//库存常量
#define ACTIVITY_STOCK_COUNT			(MAX_STOCK_GAME * 2)
#define ROUTINE_STOCK_COUNT				(MAX_STOCK_GAME * MAX_STOCK_ITEM)
#define SHARE_STOCK_COUNT				(ROUTINE_STOCK_COUNT+ACTIVITY_STOCK_COUNT)	

//////////////////////////////////////////////////////////////////////////////////

//库存数据
struct tagShareStock
{
	DWORD						dwMutexNo;							//互斥编号
	tagStockItem				StockItem;							//库存子项	
	tagStockControl				StockControl;						//库存控制	
};

//库存列表数据
struct tagStockList
{
	WORD						wKindID;							//类型标识
	DWORD						dwMutexNo;							//互斥编号
	tagShareStock*				pRelationStock;						//关联库存

	//库存列表
	tagShareStock				ShareStock[MAX_STOCK_ITEM];			//库存数组	
};

//共享数据
struct tagShareData
{
	tagStockList				RoutineStock[MAX_STOCK_GAME];		//常规库存	
	tagShareStock				ActivityStock[ACTIVITY_STOCK_COUNT];//活动库存	
};

//合并记录
struct tagStockMergeRecord
{
	//类型信息
	WORD						wKindCount;							//类型数量
	WORD						wKindIDList[MAX_STOCK_GAME];		//类型列表

	//下级库存
	WORD						wStockCount;						//库存数量
	WORD						wSublayerStockID[MAX_STOCK_GAME];	//库存标识

	//统计信息
	SCORE						wCollectStockScore;					//汇总库存
	SCORE						wCollectJackpotScore[MAX_JACKPOT_COUNT];//汇总彩金	
};

//库存信息
struct tagStockManagerItem
{
	tagStockItem				StockItem;							//库存子项
	tagStockMergeRecord			MergeRecord;						//合并记录
	LONGLONG					lStockControlID;					//控制标识
};

//////////////////////////////////////////////////////////////////////////////////

//互斥对象
class CShareMutex
{
	//存储变量
protected:
	HANDLE						m_hMutex;						//对象句柄
	DWORD						m_dwMutexNo;					//互斥编号
	INT							m_nLockCount;					//锁定计数

	//同步变量
protected:
	CCriticalSection			m_CriticalSection;				//临界对象
	ULONGLONG					m_ulLockTickCount;				//锁定时间戳	

	//函数定义
public:
	//构造函数
	CShareMutex();
	//析构函数
	virtual ~CShareMutex();

	//函数定义
protected:
	//赋值构造
	CShareMutex(const CShareMutex& ShareMutex) = delete;
	//赋值重载
	CShareMutex& operator=(const CShareMutex& other) = delete;

	//状态函数
public:
	//校验句柄
	bool IsMutexValid();
	//互斥编号
	DWORD GetMutexNo() { return m_dwMutexNo; }

	//功能函数
public:	
	//关闭句柄
	VOID CloseMutex();
	//初始化
	VOID InitMutex(LPCTSTR pszMutexPrefix, DWORD dwMutexNo);

	//信息函数
public:
	//获取时间戳
	ULONGLONG GetLockTickCount();
	//设置时间戳
	VOID SetLockTickCount(ULONGLONG ulTickCount);

	//同步函数
public:
	//解锁对象
	VOID Unlock(WORD wStockKind=0,bool bForce=false);
	//锁定对象
	DWORD Lock(DWORD dwMilliseconds=INFINITE);
};

//////////////////////////////////////////////////////////////////////////////////

//共享库存子项
class MONITOR_SERVICE_CLASS CShareStockItem : public CHandleBase
{
	//存储变量
protected:
	DWORD						m_dwUserLabel;						//用户标签
	CShareMutex *				m_pShareMutex;						//互斥对象
	tagShareStock*				m_pShareStock;						//库存信息

	//函数定义
public:
	//构造函数
	CShareStockItem() = delete;
	//构造函数
	CShareStockItem(DWORD dwUserLabel,CShareMutex* pShareMutex, tagShareStock* pShareStock);
	//析构函数
	virtual ~CShareStockItem();

	//函数定义
public:
	//赋值构造
	CShareStockItem(const CShareStockItem& ShareStockItem) = delete;
	//赋值重载
	CShareStockItem& operator=(const CShareStockItem& other) = delete;

	//库存管理
public:	
	//库存抽水
	bool StockShrinkAndUpdate(SCORE lBetScore,WORD wJackpotCount);

	//库存属性
public:
	//获取状态
	WORD GetStockID();
	//库存类型
	WORD GetStockKind();
	//当前库存
	SCORE GetStockScore();
	//抽水比例
	WORD GetShrinkRatio();
	//库存信息
	tagStockItem* GetStockItem();	
	//获取彩池
	tagJackpotItem* GetJackpotItem(WORD wLevelID);	
};

//////////////////////////////////////////////////////////////////////////////////

//互斥列表
struct tagMutexList
{
	CShareMutex					ListMutex;							//互斥对象
	CShareMutex					RelationMutex;						//互斥对象
	CShareMutex					StockMutext[MAX_STOCK_ITEM];		//互斥数组		
};

//互斥数据
struct tagMutexData
{
	tagMutexList				RoutineMutex[MAX_STOCK_GAME];		//互斥对象
	CShareMutex					ActivityMutex[ACTIVITY_STOCK_COUNT];//互斥对象
};

//类型定义
typedef CWHAutoPtr<CShareStockItem>	 CShareStockItemPtr;

//////////////////////////////////////////////////////////////////////////////////

//库存访问
class MONITOR_SERVICE_CLASS CShareStockVisitor : public CServiceThread
{
	//配置变量
protected:
	WORD								m_wKindID;							//类型标识
	tagStockList*						m_pStockList;						//库存列表
	
	//互斥对象
protected:
	CShareMutex							m_InitMutex;						//对象句柄
	tagMutexList						m_MutexList;						//互斥列表	

	//函数定义
public:
	//构造函数
	CShareStockVisitor();
	//析构函数
	virtual ~CShareStockVisitor();

	//函数定义
public:
	//重置对象
	VOID ResetStockVisitor();
	//初始化
	bool InitStockVisitor(WORD wKindID);	

	//查找函数
public:
	//查找库存
	virtual CShareStockItemPtr GetStockItem(WORD wStockKind, SCORE lBetAmount, DWORD dwUserLabel);	

	//事件函数
protected:
	//运行事件
	virtual bool OnEventThreadRun();

	//辅助函数
protected:
	//获取标志
	bool GetStockInit();
	//子项索引
	int GetStockIndex(WORD wStockKind, LONG lBetAmount);	
};

//////////////////////////////////////////////////////////////////////////////////
//类型定义
typedef CWHStockData<WORD, tagStockManagerItem>  CStockStockItem;

//接口定义
interface IStockManagerSink
{
	//预警接口
public:
	//库存预警
	virtual VOID OnEventStockScoreAlarm(tagStockAlarmItem StockAlarmList[], WORD wItemCount) = NULL;
	//彩池预警
	virtual VOID OnEventJackpotScoreAlarm(tagJackpotAlarmItem JackpotAlarmList[], WORD wItemCount) = NULL;

	//更新接口
public:
	//库存更新
	virtual VOID OnEventStockScoreUpdate(tagStockVariationItem StockVariationList[], WORD wItemCount)=NULL;
	//状态更新
	virtual VOID OnEventStockStateUpdate(tagStateVariationItem StateVariationList[], WORD wItemCount)=NULL;
	//彩池更新
	virtual VOID OnEventJackpotScoreUpdate(tagJackpotVariationItem JackpotVariationList[], WORD wItemCount)=NULL;
	
};

//////////////////////////////////////////////////////////////////////////////////
// 
//库存管理
class MONITOR_SERVICE_CLASS CShareStockManager
{
	//接口变量
protected:
	IStockManagerSink*					m_pStockManagerSink;				//回调接口	

	//管理变量
protected:
	bool								m_bManageState;						//管理状态
	LONGLONG							m_lStockControlID;					//控制标识
	CStockStockItem						m_StockStockItem;					//库存仓库	
	tagSuperPlayerControl				m_SuperPlayerControl;				//大R控制	

	//互斥对象
protected:
	CShareMutex							m_InitMutex;						 //对象句柄
	tagMutexData						m_MutexData;						 //互斥对象

	//函数定义
public:
	//构造函数
	CShareStockManager();
	//析构函数
	virtual ~CShareStockManager();

	//配置函数
public:
	//停止管理
	bool CloseStockManager();
	//启动管理
	VOID StartStockManager(IStockManagerSink* pIStockManagerSink);

	//事件函数
public:
	//时间脉冲
	VOID OnEventTimerPulse();

	//库存管理
public:
	//删除库存
	VOID DeleteStockItem(WORD wStockID);
	//修改库存
	bool ModifyStockItem(tagStockItem * pStockItem);	
	//添加库存
	bool AppendStockItem(tagStockItem StockItem[], WORD wStockCount);
	//调整库存值
	bool AdjustStockScore(WORD wStockID,SCORE lChangeScore);
	//调整彩金
	bool AdjustJackpotScore(WORD wStockID, WORD wLevelID, SCORE lChangeScore);	
	//统计彩金
	bool CollectJackpotScore(WORD wStockKind,WORD wKindID, SCORE &lJackpotScore);
	//应用大R控制
	VOID ApplySuperPlayerControl(tagSuperPlayerControl* pSuperPlayerControl);

	//库存查找
protected:
	//激活共享库存
	int ActiveActivityStockItem();
	//查找共享库存
	int SearchActivityStockItem(WORD wStockID);
	//激活库存列表
	int ActiveRoutineStockList(WORD wStockKind);
	//查找库存列表
	int SearchRoutineStockList(WORD wStockKind,WORD wKindID);
	//激活共享库存
	int ActiveRoutineStockItem(int nListIndex, WORD wStockKind);
	//查找共享库存
	int SearchRoutineStockItem(int nListIndex, WORD wStockID);
	//查找共享库存
	int SearchRoutineStockItem(int nListIndex, WORD wStockKind, LONG lBetAmount);	

	//辅助函数
protected:
	//检测变化
	VOID DetectStockChange();
	//检测状态
	VOID DetectStockState();	
	//合并库存
	bool MergeRoutineStock(int nStockIndex);
	//分裂库存
	bool FissionActivityStock(int nStockIndex);
	//收集类型
	bool CollectRelationKindID(tagStockMergeRecord* pStockMergeRecord, tagStockItem* pStockItem);
};

//////////////////////////////////////////////////////////////////////////////////

#endif