#ifndef DLG_MATCH_WIZARD_HEAD_FILE
#define DLG_MATCH_WIZARD_HEAD_FILE

#pragma once

//引入文件
#include "CheckComboBox.h"
#include "MatchInfoManager.h"
#include "MatchListControl.h"
#include "ModuleInfoManager.h"

//////////////////////////////////////////////////////////////////////////

//类说明
class CDlgMatchItem;
class CDlgMatchWizard;
class CDlgMatchWizardStep1;
class CDlgMatchWizardStep2;
class CDlgMatchWizardStep3;

//类说明
class CDlgMatchRound;
class CDlgMatchReward;
class CDlgMatchSingup1;
class CDlgMatchSingup2;
class CDlgMatchDistribute;
class CDlgMatchLockTime;
class CDlgMatchImmediate;

//类说明
class CDlgPromoteModeItem;
class CDlgPromoteOutGame;
class CDlgPromoteFixedGame;
class CDlgPromoteFixedTable;

//////////////////////////////////////////////////////////////////////////

//晋级子项
class MODULE_MANAGER_CLASS CDlgPromoteModeItem : public CDialog
{
	friend class CDlgMatchWizardStep2;	

	//变量定义
protected:		
	tagMatchRoundItem *			m_pMatchRoundItem;					//轮次子项

	//函数定义
protected:
	//构造函数
	CDlgPromoteModeItem(UINT nIDTemplate);
	//析构函数
	virtual ~CDlgPromoteModeItem();

	//重载函数
protected:
	//确定函数
	virtual VOID OnOK() { return; }
	//取消消息
	virtual VOID OnCancel() { return; }

	//接口函数
protected:	
	//保存输入
	virtual bool SaveInputInfo()=NULL;
	//刷新控件
	virtual bool RefreshControl()=NULL;
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//配置函数
protected:		
	//设置配置
	VOID SetMatchRoundItem(tagMatchRoundItem * pMatchRoundItem);
	//获取配置
	tagMatchRoundItem * GetMatchRoundItem() { return m_pMatchRoundItem; }

	//功能函数
private:
	//显示子项
	bool ShowPromoteItem(const CRect & rcRect, CWnd * pParentWnd); 
	//创建子项
	VOID CreatePromoteItem(const CRect & rcRect, CWnd * pParentWnd);

	//消息函数
private:
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//打立出局
class MODULE_MANAGER_CLASS CDlgPromoteOutGame : public CDlgPromoteModeItem
{
	//函数定义
public:
	//构造函数
	CDlgPromoteOutGame();
	//析构函数
	virtual ~CDlgPromoteOutGame();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();
	//刷新控件
	virtual bool RefreshControl();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);		
};

//////////////////////////////////////////////////////////////////////////

//定局积分
class MODULE_MANAGER_CLASS CDlgPromoteFixedGame: public CDlgPromoteModeItem
{
	//函数定义
public:
	//构造函数
	CDlgPromoteFixedGame();
	//析构函数
	virtual ~CDlgPromoteFixedGame();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();
	//刷新控件
	virtual bool RefreshControl();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);		
};

//////////////////////////////////////////////////////////////////////////

//按桌晋级
class MODULE_MANAGER_CLASS CDlgPromoteFixedTable: public CDlgPromoteModeItem
{
	//函数定义
public:
	//构造函数
	CDlgPromoteFixedTable();
	//析构函数
	virtual ~CDlgPromoteFixedTable();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();	
	//刷新控件
	virtual bool RefreshControl();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);		
};

//////////////////////////////////////////////////////////////////////////

//比赛子项类
class MODULE_MANAGER_CLASS CDlgMatchItem : public CDialog
{
	friend class CDlgMatchWizardStep1;
	friend class CDlgMatchWizardStep3;

	//变量定义
protected:		
	tagMatchOption *			m_pMatchOption;					//比赛配置
	CMatchInfoBuffer *				m_pMatchInfoBuffer;					//模块信息

	//函数定义
protected:
	//构造函数
	CDlgMatchItem(UINT nIDTemplate);
	//析构函数
	virtual ~CDlgMatchItem();

	//重载函数
protected:
	//确定函数
	virtual VOID OnOK() { return; }
	//取消消息
	virtual VOID OnCancel() { return; }

	//接口函数
protected:	
	//保存输入
	virtual bool SaveInputInfo()=NULL;	
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//配置函数
protected:	
	//设置比赛
	VOID SetMatchInfoBuffer(CMatchInfoBuffer * pMatchInfoBuffer);
	//设置配置
	VOID SetMatchOption(tagMatchOption * pMatchOption);
	//获取配置
	tagMatchOption * GetMatchOption() { return m_pMatchOption; }

	//功能函数
private:
	//显示子项
	bool ShowMatchItem(const CRect & rcRect, CWnd * pParentWnd); 
	//创建子项
	VOID CreateMatchItem(const CRect & rcRect, CWnd * pParentWnd);

	//事件函数
public:
	//类型变更
	virtual VOID OnEventMatchTypeChanged(BYTE cbMatchType);

	//消息函数
private:
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};


//////////////////////////////////////////////////////////////////////////

//报名配置
class MODULE_MANAGER_CLASS CDlgMatchSignup1 : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//函数定义
public:
	//构造函数
	CDlgMatchSignup1();
	//析构函数
	virtual ~CDlgMatchSignup1();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//事件函数
public:
	//类型变更
	virtual VOID OnEventMatchTypeChanged(BYTE cbMatchType);

	//消息函数
protected:
	//选择变更
	void OnCbnSelchangeComboFromMatch();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//报名配置
class MODULE_MANAGER_CLASS CDlgMatchSignup2 : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//函数定义
public:
	//构造函数
	CDlgMatchSignup2();
	//析构函数
	virtual ~CDlgMatchSignup2();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
};

//////////////////////////////////////////////////////////////////////////

//撮合配置
class MODULE_MANAGER_CLASS CDlgMatchDistribute : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//函数定义
public:
	//构造函数
	CDlgMatchDistribute();
	//析构函数
	virtual ~CDlgMatchDistribute();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
};

////////////////////////////////////////////////////////////////////////////
//
////排名方式
//class MODULE_MANAGER_CLASS CDlgMatchRankingMode : public CDlgMatchItem
//{
//	friend class CDlgMatchWizardStep1;
//
//	//函数定义
//public:
//	//构造函数
//	CDlgMatchRankingMode();
//	//析构函数
//	virtual ~CDlgMatchRankingMode();
//
//	//重载函数
//protected:
//	//初始化函数
//	virtual BOOL OnInitDialog();
//
//	//接口函数
//protected:
//	//保存输入
//	virtual bool SaveInputInfo();
//	//调整控件
//	virtual VOID RectifyControl(INT nWidth, INT nHeight);
//
//	//事件函数
//public:
//	//类型变更
//	virtual VOID OnEventMatchTypeChanged(BYTE cbMatchType);
//};


//////////////////////////////////////////////////////////////////////////

//比赛奖励
class MODULE_MANAGER_CLASS CDlgMatchReward : public CDlgMatchItem
{
	friend class CDlgMatchWizardStep1;

	//控件变量
protected:
	CRewardInfoBuffer 			m_RewardInfoBuffer;				//奖励信息	
	CRewardListControl			m_RewardListControl;			//列表控件

	//函数定义
public:
	//构造函数
	CDlgMatchReward();
	//析构函数
	virtual ~CDlgMatchReward();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//辅助函数
protected:
	//清空控件
	VOID CleanControl();
	//设置配置
	VOID SetMatchRewardItem(tagMatchRewardItem * pMatchRewardItem);
	//获取配置
	bool GetMatchRewardItem(tagMatchRewardItem & MatchRewardItem);	

	//消息函数
public:
	//添加奖励
	VOID OnBnClickedAddReward();
	//修改奖励
	VOID OnBnClickedModifyReward();
	//删除奖励
	VOID OnBnClickedDeleteReward();

	//控件消息
protected:
	//子项变更
	VOID OnLvnItemchangedListReward(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()	
};

//////////////////////////////////////////////////////////////////////////

//定时赛类
class MODULE_MANAGER_CLASS CDlgMatchLockTime : public CDlgMatchItem
{
	//控件变量
protected:
	CCheckComboBox						m_ComboWeekDay;					//组合控件
	CCheckComboBox						m_ComboMonthDay;				//组合控件

	//函数定义
public:
	//构造函数
	CDlgMatchLockTime();
	//析构函数
	virtual ~CDlgMatchLockTime();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//交换数据
	virtual VOID DoDataExchange(CDataExchange* pDX);

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//辅助函数
protected:
	//报名时间
	VOID GetMatchTime(SYSTEMTIME &MatchTime,UINT uHourID,UINT uMinuteID,UINT uSecondID);
	//报名时间
	VOID SetMatchTime(const SYSTEMTIME &MatchTime,UINT uHourID,UINT uMinuteID,UINT uSecondID);	

	//辅助函数
protected:
	//设置列表
	VOID SetComboDataList(CCheckComboBox* pCheckComboBox, BYTE cboDataFlagList[], BYTE cbDataCount);
	//获取列表
	VOID GetComboDataList(CCheckComboBox* pCheckComboBox, BYTE cboDataFlagList[], BYTE cbDataCount);
};

//////////////////////////////////////////////////////////////////////////

//即时赛类
class MODULE_MANAGER_CLASS CDlgMatchImmediate : public CDlgMatchItem
{
	//函数定义
public:
	//构造函数
	CDlgMatchImmediate();
	//析构函数
	virtual ~CDlgMatchImmediate();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	DECLARE_MESSAGE_MAP()	
};

//////////////////////////////////////////////////////////////////////////

//锦标赛赛类
class MODULE_MANAGER_CLASS CDlgMatchTournament : public CDlgMatchItem
{
	//控件变量
protected:
	CCheckComboBox						m_ComboWeekDay;					//组合控件
	CCheckComboBox						m_ComboMonthDay;				//组合控件
	CCheckComboBox						m_ComboYearMonth;				//组合控件

	//函数定义
public:
	//构造函数
	CDlgMatchTournament();
	//析构函数
	virtual ~CDlgMatchTournament();

	//重载函数
protected:
	//初始化函数
	virtual BOOL OnInitDialog();
	//交换数据
	virtual VOID DoDataExchange(CDataExchange* pDX);

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//辅助函数
protected:
	//报名时间
	VOID GetMatchTime(SYSTEMTIME& MatchTime, UINT uHourID, UINT uMinuteID, UINT uSecondID);
	//报名时间
	VOID SetMatchTime(const SYSTEMTIME& MatchTime, UINT uHourID, UINT uMinuteID, UINT uSecondID);

	//辅助函数
protected:
	//设置列表
	VOID SetComboDataList(CCheckComboBox* pCheckComboBox, BYTE cboDataFlagList[], BYTE cbDataCount);
	//获取列表
	VOID GetComboDataList(CCheckComboBox* pCheckComboBox, BYTE cboDataFlagList[], BYTE cbDataCount);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//向导基础类
class MODULE_MANAGER_CLASS CDlgMatchWizardItem : public CDialog
{
	//友元定义
	friend class CDlgMatchWizard;

	//状态变量
private:
	bool							m_bSaveData;						//保存标志

	//变量定义
protected:
	CDlgMatchWizard *				m_pDlgMatchWizard;					//向导指针
	tagMatchOption *				m_pMatchOption;						//比赛配置
	CMatchInfoBuffer *				m_pMatchInfoBuffer;					//比赛信息	
	CModuleInfoBuffer *				m_pModuleInfoBuffer;				//模块信息		 

	//函数定义
protected:
	//构造函数
	CDlgMatchWizardItem(UINT nIDTemplate);
	//析构函数
	virtual ~CDlgMatchWizardItem();

	//重载函数
protected:
	//确定函数
	virtual VOID OnOK();
	//取消消息
	virtual VOID OnCancel();

	//接口函数
protected:
	//保存输入
	virtual bool SaveInputInfo()=NULL;
	//激活子项
	virtual VOID OnMatchItemActive()=NULL;
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight)=NULL;

	//功能函数
private:
	//保存数据
	bool SaveItemData();
	//创建向导
	bool ShowWizardItem(const CRect & rcRect, CWnd * pParentWnd);

	//消息函数
private:
	//位置消息
	VOID OnSize(UINT nType, INT cx, INT cy);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//公共向导
class MODULE_MANAGER_CLASS CDlgMatchWizardStep1 : public CDlgMatchWizardItem
{
	//控件变量
protected:
	CComboBox						m_ModuleComboBox;				//模块列表

	//控件变量
protected:
	BYTE							m_cbItemIndex;					//子项索引
	CDlgMatchItem *					m_pDlgMatchItem[4];				//子项数组
	CDlgMatchReward					m_DlgMatchReward;				//奖励配置
	CDlgMatchSignup1				m_DlgMatchSignup1;				//报名配置
	CDlgMatchSignup2				m_DlgMatchSignup2;				//报名配置
	CDlgMatchDistribute				m_DlgMatchDistribute;			//配桌配置	
	//CDlgMatchRankingMode			m_DlgMatchRankingMode;			//排名配置

	//函数定义
public:
	//构造函数
	CDlgMatchWizardStep1();
	//析构函数
	virtual ~CDlgMatchWizardStep1();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//激活子项
	virtual VOID OnMatchItemActive();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);

	//辅助函数
protected:
	//基础配置
	VOID InitBaseOption();
	//保存配置
	bool SaveBaseOption();
	//激活子项
	VOID ActiveOptionItem(BYTE cbItemIndex);

	//消息函数
protected:
	//选择变更
	void OnCbnSelchangeComboMatchType();	
	//选择变更
	void OnTcnSelchangeMatchOption(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()	
};

//////////////////////////////////////////////////////////////////////////

//比赛向导
class MODULE_MANAGER_CLASS CDlgMatchWizardStep2 : public CDlgMatchWizardItem
{
	//控件变量
protected:
	CRoundInfoBuffer 			m_RoundInfoBuffer;				//轮次信息	
	CRoundListControl			m_RoundListControl;				//列表控件

	//控件变量
protected:
	BYTE						m_cbItemIndex;					//子项索引
	tagMatchRoundItem			m_MatchRoundItem;				//轮次子项	
	CDlgPromoteOutGame			m_PromoteOutGame;				//打立出局
	CDlgPromoteFixedGame		m_PromoteFixedGame;				//定局积分
	CDlgPromoteFixedTable		m_PromoteFixedTable;			//按桌晋级
	CDlgPromoteModeItem	*		m_pPromoteModeItem[3];			//控件变量

	//函数定义
public:
	//构造函数
	CDlgMatchWizardStep2();
	//析构函数
	virtual ~CDlgMatchWizardStep2();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();

	//辅助函数
protected:
	//清理控件
	VOID CleanControl();
	//显示子项
	VOID ShowPromoteItem(BYTE cbItemIndex);
	//设置配置
	VOID SetMatchRoundItem(tagMatchRoundItem * pMatchRoundItem);
	//获取配置
	bool GetMatchRoundItem(tagMatchRoundItem & MatchRoundItem);	

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//激活事件
	virtual VOID OnMatchItemActive();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);	

	//消息函数
public:
	//添加轮次
	VOID OnBnClickedAddRound();
	//修改轮次
	VOID OnBnClickedModifyRound();
	//修改轮次
	VOID OnBnClickedDeleteRound();

	//控件消息
protected:
	//选择变更
	void OnCbnSelchangeComboPromoteMode();
	//子项变更
	VOID OnLvnItemchangedListRound(NMHDR *pNMHDR, LRESULT *pResult);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//比赛向导
class MODULE_MANAGER_CLASS CDlgMatchWizardStep3 : public CDlgMatchWizardItem
{
	//变量定义
protected:
	CDlgMatchItem *				m_pDlgMatchItem;				//比赛子项
	CDlgMatchLockTime			m_DlgMatchLockTime;				//定时赛
	CDlgMatchImmediate			m_DlgMatchImmediate;			//即时赛
	CDlgMatchTournament			m_DlgMatchTournament;			//锦标赛

	//函数定义
public:
	//构造函数
	CDlgMatchWizardStep3();
	//析构函数
	virtual ~CDlgMatchWizardStep3();

	//重载函数
protected:
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);
	//初始化函数
	virtual BOOL OnInitDialog();

	//重载函数
public:
	//保存输入
	virtual bool SaveInputInfo();
	//激活事件
	virtual VOID OnMatchItemActive();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);	
};

//////////////////////////////////////////////////////////////////////////

//比赛向导
class MODULE_MANAGER_CLASS CDlgMatchWizard : public CDialog
{
	//友元定义
	friend class CDlgMatchWizardStep1;
	friend class CDlgMatchWizardStep2;
	friend class CDlgMatchWizardStep3;

	//变量定义
protected:
	WORD							m_wActiveIndex;						//活动索引
	CDlgMatchWizardItem *			m_pWizardItem[3];					//向导指针

	//配置变量
protected:	
	tagMatchOption					m_MatchOption;						//比赛配置		
	CMatchInfoBuffer				m_MatchInfoBuffer;					//模块信息
	CModuleInfoBuffer				m_ModuleInfoBuffer;					//模块信息		

	//控件变量
protected:
	CDlgMatchWizardStep1			m_MatchWizardStep1;					//设置步骤
	CDlgMatchWizardStep2			m_MatchWizardStep2;					//设置步骤
	CDlgMatchWizardStep3			m_MatchWizardStep3;					//设置步骤

	//函数定义
public:
	//构造函数
	CDlgMatchWizard();
	//析构函数
	virtual ~CDlgMatchWizard();

	//重载函数
public:
	//初始化函数
	virtual BOOL OnInitDialog();
	//确定函数
	virtual VOID OnOK();

	//配置函数
public:	
	//获取配置
	VOID GetMatchOption(tagMatchOption & MatchOption);	
	//设置配置
	VOID SetMatchOption(tagMatchOption & MatchOption);	

	//内部函数
private:	
	//激活向导
	bool ActiveWizardItem(WORD wIndex);		

	//消息映射
public:
	//上一步
	VOID OnBnClickedLast();
	//下一步
	VOID OnBnClickedNext();
	//完成按钮
	VOID OnBnClickedFinish();

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif