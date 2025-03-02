#ifndef SKIN_COMBOBOX_HEAD_FILE
#define SKIN_COMBOBOX_HEAD_FILE

#pragma once

#include "SkinControlsHead.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////

//按钮属性
class CSkinComboBoxAttribute : public CSkinResourceManager
{
	friend class CSkinComboBox;

	//设置部分
public:
	COLORREF						m_crBoradHover;						//边框颜色
	COLORREF						m_crBoradNormal;					//边框颜色
	CBitImage						m_ImageButtonThumb;					//按钮位图

	//函数定义
protected:
	//构造函数
	CSkinComboBoxAttribute();
	//析构函数
	virtual ~CSkinComboBoxAttribute();

	//功能函数
public:
	//更新资源
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//编辑控件
class CSkinComboEdit : public CEdit
{
	//友元定义
	friend class CSkinComboBox;

	//状态变量
protected:
	bool							m_bHovering;						//盘旋标志

	//函数定义
public:
	//构造函数
	CSkinComboEdit();
	//析构函数
	virtual ~CSkinComboEdit();

	//消息映射
protected:
	//销毁消息
	VOID OnNcDestroy();
	//鼠标移动
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//鼠标离开
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//选择控件
class SKIN_CONTROL_CLASS CSkinComboBox : public CComboBox
{
	//状态变量
protected:
	bool							m_bHovering;						//盘旋标志

	//控件变量
protected:
	CSkinComboEdit					m_SkinComboEdit;					//编辑控件

	//按钮属性
public:
	static CSkinComboBoxAttribute	m_SkinAttribute;					//按钮属性

	//函数定义
public:
	//构造函数
	CSkinComboBox();
	//析构函数
	virtual ~CSkinComboBox();

	//重载函数
protected:
	//控件绑定
    virtual VOID PreSubclassWindow();

	//消息映射
protected:
	//重画消息
    VOID OnPaint();
	//销毁消息
	VOID OnNcDestroy();
	//创建消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标移动
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//鼠标离开
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif