#pragma once

#include "Stdafx.h"
#include "Resource.h"
#include "RecordEditCtrl.h"

//////////////////////////////////////////////////////////////////////////
//颜色定义
#define STRING_ERROR				RGB(200,0,0)						//错误颜色
#define STRING_NORMAL				RGB(100,100,100)					//常规颜色
//玩家配置
struct GameInfo
{
	SCORE							wUserId;							//用户id
	string							sGameName;							//游戏名称
	SCORE							lSelectGame;						//局数
	DOUBLE							lFirstScore;						//初始金币
	DOUBLE							lUserScore;							//当前金币
	DOUBLE							lUserBetTotalScore;					//游戏下注
	DOUBLE							lUserTotalGameScore;				//游戏得分
	DOUBLE							lGameScore;							//本局输赢
	SCORE							lVictory;							//胜局
	SCORE							lWinRate;							//胜率
	SCORE							lSlipperRate;						//返奖率
	DOUBLE							lSumWinning;						//总输赢
};

//库存
struct GameDebugInfo
{
	SCORE							lNumbergames;						//局数
	DOUBLE							lCurSysStorage;						//当前系统库存
	DOUBLE							lCurPlayerStorage;					//当前玩家库存
	INT								lCurParameterK;						//当前调节系数
	INT								lCurResetSection;					//强制重置区间
	DOUBLE							lSysDbgSysWin;						//系统输赢总数
	DOUBLE							lSysDbgPlayerWin;					//玩家输赢总数
	SCORE							lResetTimes;						//重置次数
	DOUBLE							lCurStorageSyatemWinlose;			//累积库存统计
};

//游戏胜率
struct GameWinRate
{
	DOUBLE							lSumBetTotal;						//总下注
	DOUBLE							lSumWinPoints;						//总赢分
	DOUBLE							lSumLosePoints;						//总输分
	DOUBLE							lSumGameScore;						//总游戏得分
	INT								lSumPlayed;							//总局数
	INT								lSumSuccess;						//总胜局
};

//主对话框
class CAnalyseToolDlg : public CDialog
{
	//配置信息
protected:
	WORD							m_wUserCount;						//用户数目
	TCHAR							m_szModuleName[128];				//游戏名称
	SCORE							m_lUserID;							//当前用户ID
	SCORE							m_lUserNumber;						//当前用户总局数
	SCORE							m_lGameNumber;						//总局数
	SCORE							m_lUserId[200];						//游戏ID
	SCORE							m_lCurrentNumber;					//当前页数
	SCORE							m_lSumNumber;						//总页数
	TCHAR							m_szGameName[128];					//游戏名称
	//控件变量
public:
	CListCtrl						m_ResultsShow;						//信息窗口
	CListCtrl						m_InventoryShows;					//信息窗口


	//函数定义
public:
	//构造函数
	CAnalyseToolDlg();
	//析构函数
	virtual ~CAnalyseToolDlg();
	//循环消息
	//VOID CycleMessage();
	//接口查询
	virtual VOID* QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//重载函数
protected:
	//创建函数
	virtual BOOL OnInitDialog();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//控件绑定
	virtual void DoDataExchange(CDataExchange* pDX);
	
	//按钮消息
protected:

	//查询战绩
	VOID OnBnQueryResults(GameInfo* pItemData);
	//查询全部战绩
	VOID OnBnQueryAllResults();
	//显示库存
	VOID OnBnShowInventory(GameDebugInfo* pItemData);
	//查询库存
	VOID OnBnQueryInventory();
	//查询全部库存
	VOID OnBnQueryAllInventory();
	//查找用户
	VOID OnBnSearchUser();
	//关闭
	VOID OnBnGameStar();
	//路径
	string OnGetPath();
	//总页数
	VOID OnBnSumNumber();
	//查询库存上一页
	VOID OnBnQueryPreviousPage();
	//查询库存下一页
	VOID OnBnQueryNextPage();
	//查询游戏胜率
	VOID OnBnQueryGameWinRate();
	//系统消息
protected:

	void DDX_DOUBLE(int nIDC, DOUBLE value);
	DECLARE_MESSAGE_MAP()
public:
	//初始化控件
	VOID InitControls();

	//事件函数
protected:
	//选项变更
	VOID OnCbnSelchangeKindList();
	//选项变更
	VOID OnCbnSelchangeTypeList();
	//选项变更
	VOID OnCbnSelchangeUserList();
	//单击子项
	VOID OnListShowsItemclick(NMHDR* pNMHDR, LRESULT* pResult);	
	//选中子项
	VOID SelectItemShows(int nIndex);
	//单击子项
	VOID OnListInventoryItemclick(NMHDR* pNMHDR, LRESULT* pResult);
	//选中子项
	VOID SelectItemInventory(int nIndex);
	//选择变更
	VOID OnLvnItemchangedResultsShow(NMHDR* pNMHDR, LRESULT* pResult);	
	//子项变更
	VOID OnLvnItemchangedInventoryShows(NMHDR* pNMHDR, LRESULT* pResult);
	
};

//////////////////////////////////////////////////////////////////////////
