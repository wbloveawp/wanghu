#ifndef SKIN_EDIT_HEAD_FILE
#define SKIN_EDIT_HEAD_FILE

#pragma once

#include "SkinControlsHead.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////

//编辑框属性
class CSkinEditAttribute : public CSkinResourceManager
{
	friend class CSkinEdit;
	friend class CSkinEditEx;

	//设置部分
public:
	COLORREF						m_crFocusTX;						//字体颜色
	COLORREF						m_crFocusBK;						//背景颜色
	COLORREF						m_crNoFocusTX;						//字体颜色
	COLORREF						m_crNoFocusBK;						//背景颜色
	COLORREF						m_crDisFocusTX;						//字体颜色
	COLORREF						m_crDisFocusBK;						//背景颜色
	COLORREF						m_crBoradNormal;					//边框颜色

	//GDI 对象
public:
	CBrush							m_brFocus;							//画刷资源
	CBrush							m_brNoFocus;						//画刷资源
	CBrush							m_brDisbale;						//画刷资源

	//函数定义
protected:
	//构造函数
	CSkinEditAttribute();
	//析构函数
	virtual ~CSkinEditAttribute();

	//功能函数
public:
	//更新资源
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//编辑框
class SKIN_CONTROL_CLASS CSkinEdit : public CEdit
{
	//状态变量
protected:
	bool							m_bFocus;							//焦点标志

	//静态变量
public:
	static CSkinEditAttribute		m_SkinAttribute;					//界面属性

	//函数定义
public:
	//构造函数
	CSkinEdit();
	//析构函数
	virtual ~CSkinEdit();

	//查询函数
public:
	//获取属性接口
	static CSkinEditAttribute * GetSkinAttribute() { return &m_SkinAttribute; }

	//消息函数 
protected:
	//重画消息
    VOID OnNcPaint();
	//得到焦点
	VOID OnSetFocus(CWnd * pOldWnd);
	//失去焦点
	VOID OnKillFocus(CWnd * pNewWnd);
	//绘画控件
	HBRUSH CtlColor(CDC * pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

//编辑框
class SKIN_CONTROL_CLASS CSkinEditEx : public CSkinEdit
{
	//颜色变量
protected:
	COLORREF						m_crFocusTX;						//字体颜色
	COLORREF						m_crFocusBK;						//背景颜色
	COLORREF						m_crNoFocusTX;						//字体颜色
	COLORREF						m_crNoFocusBK;						//背景颜色
	COLORREF						m_crDisFocusTX;						//字体颜色
	COLORREF						m_crDisFocusBK;						//背景颜色

	//GDI 对象
public:
	CBrush							m_brFocus;							//画刷资源
	CBrush							m_brNoFocus;						//画刷资源
	CBrush							m_brDisbale;						//画刷资源

	//函数定义
public:
	//构造函数
	CSkinEditEx();
	//析构函数
	virtual ~CSkinEditEx();

	//功能函数
public:
	//设置颜色
	VOID SetFocusColor(COLORREF crFocusTX, COLORREF crFocusBK);
	//设置颜色
	VOID SetNoFocusColor(COLORREF crNoFocusTX, COLORREF crNoFocusBK);
	//设置颜色
	VOID SetDisFocusColor(COLORREF crDisFocusTX, COLORREF crDisFocusBK);

	//消息函数 
protected:
	//重画消息
    VOID OnNcPaint();
	//绘画控件
	HBRUSH CtlColor(CDC * pDC, UINT nCtlColor);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif