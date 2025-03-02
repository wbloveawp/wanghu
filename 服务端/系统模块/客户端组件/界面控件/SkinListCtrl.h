#ifndef SKIN_LISTCTRL_HEAD_FILE
#define SKIN_LISTCTRL_HEAD_FILE

#pragma once

#include "SkinScrollBar.h"
#include "SkinControlsHead.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////
//枚举定义

//排序类型
enum enSortType
{
	SortType_NoSort,				//没有排序
	SortType_Ascend,				//升序排序
	SortType_Descale,				//降序排序
};

//////////////////////////////////////////////////////////////////////////

//列表属性
class CSkinListCtrlAttribute : public CSkinResourceManager
{
	//友元定义
	friend class CSkinListCtrl;
	friend class CSkinHeaderCtrl;

	//颜色属性
public:
	COLORREF						m_crTitle;							//字体颜色
	COLORREF						m_crListTX;							//列表文字
	COLORREF						m_crListBK;							//列表底色

	//资源变量
public:
	CBitImage						m_SkinImageL;						//标题位图
	CBitImage						m_SkinImageM;						//标题位图
	CBitImage						m_SkinImageR;						//标题位图

	//函数定义
protected:
	//构造函数
	CSkinListCtrlAttribute();
	//析构函数
	virtual ~CSkinListCtrlAttribute();

	//功能函数
public:
	//更新资源
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//列头控件
class SKIN_CONTROL_CLASS CSkinHeaderCtrl : public CHeaderCtrl
{
	//排序变量
protected:
	INT								m_nColumn;							//当前排列
	enSortType						m_SortType;							//排序类型

	//函数定义
public:
	//构造函数
	CSkinHeaderCtrl();
	//析构函数
	virtual ~CSkinHeaderCtrl();

	//功能函数
public:
	//设置标志
	VOID SetSortArrow(INT nSortColumn, enSortType SortType);
	//获取标志
	VOID GetSortArrow(INT & nSortColumn, enSortType & SortType);

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
class SKIN_CONTROL_CLASS CSkinListCtrl : public CListCtrl
{
	//变量定义
protected:
	CSkinScrollBar					m_SkinScrollBar;					//滚动条类
	CSkinHeaderCtrl					m_SkinHeadCtrl;						//列头控件
	
	//属性变量
public:
	static CSkinListCtrlAttribute	m_SkinAttribute;					//列表属性

	//函数定义
public:
	//构造函数
	CSkinListCtrl();
	//析构函数
	virtual ~CSkinListCtrl();

	//重载函数
public:
	//控件绑定
	virtual VOID PreSubclassWindow();

	//功能函数
public:
	//获取表头
	CSkinHeaderCtrl & GetHeadCtrl() { return m_SkinHeadCtrl; }
	//获取标志
	VOID GetSortArrow(INT & nSortCol, enSortType & SortType) { m_SkinHeadCtrl.GetSortArrow(nSortCol,SortType); };
	//设置标志
	VOID SetSortArrow(INT nSortCol, enSortType SortType) { return m_SkinHeadCtrl.SetSortArrow(nSortCol,SortType); }

	//消息函数
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//建立消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif