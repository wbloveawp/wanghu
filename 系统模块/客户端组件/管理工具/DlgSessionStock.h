#ifndef DLG_SESSION_STOCK_HEAD_FILE
#define DLG_SESSION_STOCK_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CheckComboBox.h"
#include "DlgSessionItem.h"
#include "CustomListCtrl.h"

//////////////////////////////////////////////////////////////////////////////////
//库存会话
class CDlgSessionStock : public CDlgSessionItem
{
	//变量定义
protected:
	int								m_nSelStockItem;					//选择索引
	int								m_nSelJackpotItem;					//选择索引

	//操作变量
protected:
	BYTE							m_cbOperateMode;					//操作模式
	WORD							m_wJackpotCount;					//彩池数量
	tagJackpotItem					m_JackpotItem[MAX_JACKPOT_COUNT];	//彩池信息

	//控件变量
protected:
	CButton							m_btConfirm;						//确认按钮
	CButton							m_btQueryStock;						//查询库存
	CButton							m_btAppendStock;					//添加库存
	CButton							m_btModifyStock;					//修改库存
	CButton							m_btDeleteStock;					//删除库存
	CButton							m_btAppendJackpot;					//添加彩池
	CButton							m_btModifyJackpot;					//修改彩池
	CButton							m_btDeleteJackpot;					//删除彩池	

	//控件变量
protected:
	CComboBox						m_cbStockLevel;						//库存等级
	CCheckComboBox					m_cbGameList;						//游戏列表
	

	//列表配置
protected:
	CCustomListCtrl					m_StockList;						//库存列表
	CCustomListCtrl					m_JackpotList;						//彩池列表

	//函数定义
public:
	//构造函数
	CDlgSessionStock();
	//析构函数
	virtual ~CDlgSessionStock();

	//重载函数
protected:
	//确定函数
	virtual VOID OnOK();
	//销毁窗口
	virtual VOID OnDestory();
	//配置函数
	virtual BOOL OnInitDialog();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//控件绑定
	virtual VOID DoDataExchange(CDataExchange * pDX);	

	//通知事件
public:
	//列表完成
	virtual VOID OnEventListFinish();

	//回调接口
public:
	//连接事件
	virtual bool OnEventMissionLink(LONG lErrorCode) { return true; }
	//关闭事件
	virtual bool OnEventMissionShut(BYTE cbShutReason) { return true; }
	//读取事件
	virtual bool OnEventMissionRead(CMD_Command Command, VOID* pData, WORD wDataSize);

	//按钮事件
protected:
	//确认操作
	VOID OnBnClickedBtConfirm();
	//查询库存
	VOID OnBnClickedBtQueryStock();
	//添加库存
	VOID OnBnClickedBtAppendStock();
	//编辑库存
	VOID OnBnClickedBtModifyStock();
	//删除库存
	VOID OnBnClickedBtDeleteStock();	
	//添加彩池
	VOID OnBnClickedBtAppendJackpot();
	//编辑彩池
	VOID OnBnClickedBtModifyJackpot();
	//删除彩池
	VOID OnBnClickedBtDeleteJackpot();

	//列表事件
protected:
	//单击子项
	VOID OnHdnListClickStockItem(NMHDR *pNMHDR, LRESULT *pResult);
	//删除子项
	VOID OnLvnListDeleteStockItem(NMHDR *pNMHDR, LRESULT *pResult);
	//键盘按下
	VOID OnLvnListKeydownStockItem(NMHDR* pNMHDR, LRESULT* pResult);
	//单击子项
	VOID OnHdnListClickJackpotItem(NMHDR* pNMHDR, LRESULT* pResult);
	//删除子项
	VOID OnLvnListDeleteJackpotItem(NMHDR* pNMHDR, LRESULT* pResult);
	//键盘按下
	VOID OnLvnListKeydownJackpotItem(NMHDR* pNMHDR, LRESULT* pResult);

	//辅助函数
public:
	//缩小分数
	float ScoreZoomout(SCORE lScore);
	//放大分数
	SCORE ScoreZoomin(LPCSTR pszScore);

	//功能函数
public:
	//初始化控件
	VOID InitControls();
	//更新状态
	VOID UpdateControlState();
	//控件使能
	VOID EnableControls(BOOL bEnabled);
	//选择库存
	VOID SelectStockItem(int nIndex);
	//选择彩池
	VOID SelectJackpotItem(int nIndex);
	//修改信息
	VOID AddModifyJackpotInfo(bool bModify);
	//设置子项
	VOID SetStockItemToList(int nItemIndex, tagStockInfo* pStockInfo);
	//设置子项
	VOID SetJackpotItemToList(int nItemIndex, tagJackpotItem* pJackpotItem);	

	//网络请求
public:
	//删除库存
	bool DeleteStockInfo(WORD wStockID);
	//查询库存
	bool QueryStockInfo(LPCTSTR pszKindIDList);
	//修改库存
	bool ModifyStockInfo(tagStockInfo* pStockInfo);
	//添加库存
	bool AppendStockInfo(tagStockInfo* pStockInfo);
	//调整库存
	bool AdjustStockScore(WORD wStockID, SCORE lChangeScore);
	//调整彩金
	bool AdjustJackpotScore(WORD wStockID, WORD wLevelID, SCORE lChangeScore);

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////////////

#endif