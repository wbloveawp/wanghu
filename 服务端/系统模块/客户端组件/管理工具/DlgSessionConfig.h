#ifndef DLG_SESSION_CONFIG_HEAD_FILE
#define DLG_SESSION_CONFIG_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CheckComboBox.h"
#include "DlgSessionItem.h"
#include "CustomListCtrl.h"

//////////////////////////////////////////////////////////////////////////////////
//库存会话
class CDlgSessionConfig : public CDlgSessionItem
{
	//变量定义
protected:
	WORD							m_wKindID;							//类型标识
	CListCtrl*						m_pSelListCtrl;						//选择控件
	int								m_nSelWeightItem;					//选择索引
	
	//控件变量
protected:
	CButton							m_btQuery;							//查询按钮
	CButton							m_btUpdateBasicConfig;				//保存按钮
	CButton							m_btSaveNormalWeight;				//保存按钮
	CButton							m_btSaveWhiteListWeight;			//保存按钮
	CComboBox						m_cbGameList;						//游戏列表
	CComboBox						m_cbWeightType;						//权重类型

	//列表配置
protected:
	CCustomListCtrl					m_ListNormalWeight;					//权重列表
	CCustomListCtrl					m_ListWhiteListWeight;				//权重列表

	//函数定义
public:
	//构造函数
	CDlgSessionConfig();
	//析构函数
	virtual ~CDlgSessionConfig();

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
	//查询库存
	VOID OnBnClickedBtQuery();
	//添加库存
	VOID OnBnClickedBtAppend();
	//编辑库存
	VOID OnBnClickedBtModify();
	//删除库存
	VOID OnBnClickedBtDelete();	
	//保存配置
	VOID OnBnClickedBtUpdateBasicConfig();
	//保存权重
	VOID OnBnClickedBtUpdateNormalWeight();
	//保存权重
	VOID OnBnClickedBtUpdateWhiteListWeight();

	//列表事件
protected:
	//单击子项
	VOID OnHdnListClickWeightItem(NMHDR *pNMHDR, LRESULT *pResult);
	//删除子项
	VOID OnLvnListDeleteWeightItem(NMHDR *pNMHDR, LRESULT *pResult);
	//键盘按下
	VOID OnLvnListKeydownWeightItem(NMHDR* pNMHDR, LRESULT* pResult);	

	//功能函数
public:
	//初始化控件
	VOID InitControls();
	//控件使能
	VOID EnableControls(BOOL bEnabled);

	//辅助函数
public:
	//修改信息
	VOID AddModifyWightItem(bool bModify);	
	//格式化配置
	bool FormatJsonConfig(LPCTSTR pszBasicConfig, TCHAR szFormatConfig[], WORD wBuffSize);
	//压缩配置
	bool CompressJsonConfig(LPCTSTR pszFormatConfig, TCHAR szBasicConfig[], WORD wBuffSize);

	//辅助函数
public:
	//选择库存
	VOID SelectWeightItem(CListCtrl* pListCtrl,int nIndex);	
	//收集权重
	int CollectWeightItem(CListCtrl* pListCtrl, int nBuffCount, tagTimesWeightItem TimesWeightItem[]);
	//设置子项
	VOID SetWeightItemToList(CListCtrl* pListCtrl, int nItemIndex, tagTimesWeightItem* pTimesWeightItem);

	//网络请求
public:
	//查询库存
	bool QueryGameConfig(WORD wKindID);
	//保存基础配置
	bool UpdateBasicConfig(WORD wKindID, LPCTSTR pszBasiceConfig);
	//更新权重表
	bool UpdateWeightTable(WORD wKindID, tagTimesWeightItem TimesWeightItem[], WORD wItemCount);	

	DECLARE_MESSAGE_MAP()

};

//////////////////////////////////////////////////////////////////////////////////

#endif