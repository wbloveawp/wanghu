#ifndef SKIN_TAB_CONTROL_HEAD_FILE
#define SKIN_TAB_CONTROL_HEAD_FILE

#pragma once

#include "SkinControlsHead.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////

//类说明
class CSkinTabControlItem;

//数组定义
typedef CWHArray<CSkinTabControlItem *>  CSkinTabControlItemArray;//子项数组

//////////////////////////////////////////////////////////////////////////

//子项属性
class CSkinTabControlAttribute : public CSkinResourceManager
{
	friend class CSkinTabControl;

	//设置部分
public:
	COLORREF						m_crNormalTX;						//字体颜色
	COLORREF						m_crSelectTX;						//字体颜色
	COLORREF						m_crControlBack;					//背景颜色
	COLORREF						m_crControlBorad;					//边框颜色

	//函数定义
protected:
	//构造函数
	CSkinTabControlAttribute();
	//析构函数
	virtual ~CSkinTabControlAttribute();

	//功能函数
public:
	//更新资源
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//子项控件
class SKIN_CONTROL_CLASS CSkinTabControl : public CTabCtrl
{
	//按钮属性
public:
	static CSkinTabControlAttribute	m_SkinAttribute;					//子项属性

	//函数定义
public:
	//构造函数
	CSkinTabControl();
	//析够函数
	virtual ~CSkinTabControl();

	//重载函数
protected:
	//绑定函数
	virtual VOID PreSubclassWindow();

	//消息函数
protected:
	//重画函数	
	VOID OnPaint();
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//页面子项
class SKIN_CONTROL_CLASS CSkinTabControlItem : public CDialog
{
	//变量定义
public:
	UINT							m_nIDTemplate;						//模板标识
	CSize							m_ControlSize;						//控件大小

	//函数定义
public:
	//构造函数
	CSkinTabControlItem(UINT nIDTemplate);
	//析构函数
	virtual ~CSkinTabControlItem();

	//重载函数
protected:
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//确定消息
	virtual VOID OnOK() { return; }
	//取消消息
	virtual VOID OnCancel();

	//重载函数
public:
	//获取标题
	virtual LPCTSTR GetItemTitle()=NULL;
	//创建控件
	virtual bool CreateControlItem(CWnd * pParentWnd);

	//消息映射
public:
	//绘画消息
	VOID OnPaint();
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//控件颜色
	HBRUSH OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif