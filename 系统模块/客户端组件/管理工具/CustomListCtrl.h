#ifndef CUSTOM_LISTCTRL_HEAD_FILE
#define CUSTOM_LISTCTRL_HEAD_FILE

#pragma once

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
//枚举定义

//排序类型
enum enCustomSortType
{
	CST_NoSort,					//没有排序
	CST_Ascend,					//升序排序
	CST_Descale,				//降序排序
};

//////////////////////////////////////////////////////////////////////////

//列表属性
class CListCtrlAttribute
{
	//友元定义
	friend class CCustomListCtrl;
	friend class CCustomHeaderCtrl;

	//列表属性
public:
	WORD							m_wItemHeight;						//子项高度

	//颜色属性
public:
	COLORREF						m_crTitle;							//字体颜色
	COLORREF						m_crNormalTX;						//列表文字
	COLORREF						m_crNormalBK;						//列表底色
	COLORREF						m_crSelectedTX;						//列表文字
	COLORREF						m_crSelectedBK;						//列表底色

	//函数定义
protected:
	//构造函数
	CListCtrlAttribute();
	//析构函数
	virtual ~CListCtrlAttribute();
};

//////////////////////////////////////////////////////////////////////////

//列头控件
class CCustomHeaderCtrl : public CHeaderCtrl
{
	//排序变量
protected:
	INT								m_nColumn;							//当前排列
	enCustomSortType				m_SortType;							//排序类型

	//函数定义
public:
	//构造函数
	CCustomHeaderCtrl();
	//析构函数
	virtual ~CCustomHeaderCtrl();

	//功能函数
public:
	//设置标志
	VOID SetSortArrow(INT nSortColumn, enCustomSortType SortType);
	//获取标志
	VOID GetSortArrow(INT & nSortColumn, enCustomSortType& SortType);

	//消息函数
protected:
	//重画消息
	VOID OnPaint();
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//列表类
class CCustomListCtrl : public CListCtrl
{
	//变量定义
protected:
	CCustomHeaderCtrl					m_CustomHeadCtrl;					//列头控件
	
	//属性变量
public:
	CListCtrlAttribute					m_ListCtrlAttribute;				//列表属性

	//函数定义
public:
	//构造函数
	CCustomListCtrl();
	//析构函数
	virtual ~CCustomListCtrl();

	//重载函数
public:
	//控件绑定
	virtual VOID PreSubclassWindow();	
	//绘画函数
	virtual VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

	//配置函数
public:
	//列表属性
	CListCtrlAttribute* GetListCtrlAttribute() { return &m_ListCtrlAttribute; }

	//功能函数
public:
	//获取表头
	CCustomHeaderCtrl & GetHeadCtrl() { return m_CustomHeadCtrl; }
	//获取标志
	VOID GetSortArrow(INT & nSortCol, enCustomSortType& SortType) { m_CustomHeadCtrl.GetSortArrow(nSortCol,SortType); };
	//设置标志
	VOID SetSortArrow(INT nSortCol, enCustomSortType SortType) { return m_CustomHeadCtrl.SetSortArrow(nSortCol,SortType); }

	//消息函数
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//建立消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);

	//消息响应
protected:
	//绘画函数
	afx_msg VOID MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);


	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif