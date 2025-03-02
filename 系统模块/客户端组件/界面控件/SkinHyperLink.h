#ifndef SKIN_HYPER_LINK_HEAD_FILE
#define SKIN_HYPER_LINK_HEAD_FILE

#pragma once

#include "SkinControlsHead.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////

//列表类属性
class CSkinHyperLinkAttribute : public CSkinResourceManager
{
	friend class CSkinHyperLink;

	//设置部分
public:
	COLORREF						m_crBackColor;						//背景颜色
	COLORREF						m_crHoverText;						//字体颜色
	COLORREF						m_crNormalText;						//字体颜色
	COLORREF						m_crVisitedText;					//字体颜色

	//函数定义
protected:
	//构造函数
	CSkinHyperLinkAttribute();
	//析构函数
	virtual ~CSkinHyperLinkAttribute();

	//功能函数
public:
	//更新资源
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//超连接类
class SKIN_CONTROL_CLASS CSkinHyperLink : public CStatic
{
	//配置变量
protected:
    bool							m_bUnderLine;						//是否划线
    bool							m_AutoRectify;						//自动调整

	//颜色定义
protected:
	COLORREF						m_crBackColor;						//背景颜色
	COLORREF						m_crHoverText;						//字体颜色
	COLORREF						m_crNormalText;						//字体颜色
	COLORREF						m_crVisitedText;					//字体颜色

	//状态变量
protected:
	bool							m_bVisited;                     	//是否访问
   	bool							m_bHovering;						//盘旋标志
    CString							m_strHyperLink;						//连接地址

	//组件变量
protected:
    CToolTipCtrl					m_ToolTipCtrl;						//提示控件
	ISkinMouseEvent *				m_pISkinMouseEvent;					//事件接口

	//属性变量
public:
	static CSkinHyperLinkAttribute	m_SkinAttribute;					//超连接属性

	//函数定义
public:
	//构造函数
	CSkinHyperLink();
	//析构函数
	virtual ~CSkinHyperLink();

	//配置函数
public:
	//设置接口
	bool SetSkinMouseEvent(IUnknownEx * pIUnknownEx);
	//获取接口
	VOID * GetSkinMouseEvent(const IID & Guid, DWORD dwQueryVer);

	//查询函数
public:
	//获取访问
	bool IsVisiteded() { return m_bVisited; }
	//获取下划线
	bool IsUnderline() { return m_bUnderLine; }
	//获取调整
	bool IsAutoRectify() { return m_AutoRectify; }
	//获取地址
	LPCTSTR GetHyperLinkUrl() { return m_strHyperLink; }

	//设置函数
public:
	//设置访问
    VOID SetVisited(bool bVisited);
	//设置下划线
    VOID SetUnderLine(bool bUnderLine);
	//设置调整
    VOID SetAutoRectify(bool bAutoRectify);
	//设置 URL
	VOID SetHyperLinkUrl(LPCTSTR pszHyperLink);

	//颜色设置
public:
	//背景颜色
	VOID SetBackGroundColor(COLORREF crColor);
	//字体颜色
	VOID SetHyperLinkTextColor(COLORREF crHoverText, COLORREF crNormalText, COLORREF crVisitedText);

	//重载函数
protected:
	//控件子类化
	virtual VOID PreSubclassWindow();
	//消息解释
	virtual BOOL PreTranslateMessage(MSG * pMsg);
	//默认回调函数
	LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	//辅助函数
protected:
	//初始化
	VOID InitHyperLink();
	//更新提示
	VOID UpdateToolTip();
	//调整位置
	VOID RectifyControl();

	//消息函数
protected:
	//绘画消息
	VOID OnPaint();
	//单击控件
	VOID OnStnClicked();
	//建立消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标移动
	VOID OnMouseMove(UINT nFlags, CPoint Point);
	//光标消息
    BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	//鼠标离开
	LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	//标题消息
	LRESULT	OnSetTextMesage(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif