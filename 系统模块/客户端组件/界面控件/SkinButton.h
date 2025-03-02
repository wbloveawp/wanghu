#ifndef SKIN_BUTTON_HEAD_FILE
#define SKIN_BUTTON_HEAD_FILE

#pragma once

#include "SkinControlsHead.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////

//按钮属性
class CSkinButtonAttribute : public CSkinResourceManager
{
	friend class CSkinButton;

	//设置部分
public:
	COLORREF						m_crText;							//字体颜色
	CBitImage						m_ImageBack;						//按钮位图

	//函数定义
protected:
	//构造函数
	CSkinButtonAttribute();
	//析构函数
	virtual ~CSkinButtonAttribute();

	//功能函数
public:
	//更新资源
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//按钮类
class SKIN_CONTROL_CLASS CSkinButton : public CButton
{
	//状态变量
protected:
	bool							m_bExpand;							//是否拉伸
	bool							m_bHovering;						//盘旋标志
	bool							m_bTransparent;						//是否透明
		
	//配置变量
protected:
	COLORREF						m_crText;							//字体颜色
	CBitImage						m_ImageBack;						//按钮位图
	
	//接口变量
protected:
	ISkinMouseEvent *				m_pISkinMouseEvent;					//事件接口
	WORD							m_wImageIndex;						//过度索引

	//按钮属性
public:
	static CSkinButtonAttribute		m_SkinAttribute;					//按钮属性
	
	//函数定义
public:
	//构造函数
	CSkinButton();
	//析构函数
	virtual ~CSkinButton();

	//重载函数
protected:
	//绑定函数
	virtual VOID PreSubclassWindow();
	//绘画函数
	virtual VOID DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//回调函数
	virtual LRESULT DefWindowProc(UINT uMmessage, WPARAM wParam, LPARAM lParam);

	//配置函数
public:
	//设置接口
	bool SetSkinMouseEvent(IUnknownEx * pIUnknownEx);
	//获取接口
	VOID * GetSkinMouseEvent(const IID & Guid, DWORD dwQueryVer);

	//配置函数
public:
	//设置颜色
	bool SetTextColor(COLORREF crText);
	//加载位图
	bool SetButtonImage(LPCTSTR pszFileName, bool bExpandImage, bool bTransparent);
	//加载位图
	bool SetButtonImage(UINT uBitmapID, HINSTANCE hInstance, bool bExpandImage, bool bTransparent);

	//属性查询
public:
	//获取颜色
	COLORREF GetTextColor() { return m_crText; }
	//获取位图
	bool GetButtonImage(tagImageLoadInfo & ImageLoadInfo);

	//辅助函数
private:
	//调整控件
	VOID RectifyControl();

	//消息函数
protected:
	//销毁消息
	VOID OnNcDestroy();
	//背景消息
	BOOL OnEraseBkgnd(CDC * pDC);
	//建立消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标移动
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//鼠标离开
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	//光标消息
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

//////////////////////////////////////////////////////////////////////////

#endif