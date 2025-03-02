#ifndef SKIN_SPLITTER_HEAD_FILE
#define SKIN_SPLITTER_HEAD_FILE

#pragma once

#include "SkinControlsHead.h"
#include "SkinResourceManager.h"

//////////////////////////////////////////////////////////////////////////

//类说明
class CSkinSplitter;

//拆分类型
enum enSplitterType
{
	SplitterType_Hor,				//横向拆分
	SplitterType_Vor,				//竖向拆分
};

//拆分条接口
interface ISplitterSink
{
	//拆分消息
	virtual bool __cdecl OnSplitterEvent(CSkinSplitter * pSkinSplitter, UINT uSplitID, INT nXPos, INT nYPos)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//拆分条属性
class CSkinSplitterAttribute : public CSkinResourceManager
{
	friend class CSkinSplitter;

	//位图资源
public:
	COLORREF						m_crBorad;							//边框颜色
	COLORREF						m_crBackGround;						//背景颜色

	//函数定义
protected:
	//构造函数
	CSkinSplitterAttribute();
	//析构函数
	virtual ~CSkinSplitterAttribute();

	//功能函数
public:
	//更新资源
	bool UpdateSkinResource(ISkinResource * pISkinResource);
};

//////////////////////////////////////////////////////////////////////////

//拆分条类
class SKIN_CONTROL_CLASS CSkinSplitter : public CWnd
{
	//状态变量
protected:
	bool							m_bMouseDown;						//是否按下
	CRect							m_rcDrawLine;						//线条位置

	//属性变量
protected:
	INT								m_nMaxPos;							//最大位置
	INT								m_nLessPos;							//最小位置
	enSplitterType					m_SplitterType;						//拆分类型

	//颜色定义
protected:
	COLORREF						m_crBorad;							//边框颜色
	COLORREF						m_crBackGround;						//背景颜色

	//组件变量
protected:
	ISplitterSink *					m_pISplitterSink;					//拆分事件

	//拆分条属性
public:
	static CSkinSplitterAttribute	m_SkinAttribute;					//界面属性

	//函数定义
public:
	//构造函数
	CSkinSplitter();
	//析构函数
	virtual ~CSkinSplitter();

	//重载函数
protected:
	//绑定函数
	virtual VOID PreSubclassWindow();
	//命令消息
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//配置函数
public:
	//设置范围
	VOID SetSplitterRange(INT nLessPos, INT nMaxPos);
	//设置类型
	VOID SetSplitterType(enSplitterType SplitterType);
	//设置接口
	VOID SetSplitterSink(ISplitterSink * pISplitterSink);
	//设置背景
	VOID SetSplitterColor(COLORREF crBorad, COLORREF crBackGround);

	//查询函数
public:
	//获取类型
	enSplitterType GetSplitterType() { return m_SplitterType; }
	//获取接口
	ISplitterSink * GetSplitterSink() { return m_pISplitterSink; };
	//设置范围
	VOID GetSplitterRange(INT & nLessPos, INT & nMaxPos) { nLessPos=m_nLessPos; nMaxPos=m_nMaxPos; }

	//消息函数
protected:
	//重画消息
	VOID OnPaint();
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//失去焦点
	VOID OnKillFocus(CWnd * pNewWnd);
	//鼠标消息
	VOID OnMouseMove(UINT nFlags, CPoint point);
	//鼠标消息
	VOID OnLButtonUp(UINT nFlags, CPoint point);
	//鼠标消息
	VOID OnLButtonDown(UINT nFlags, CPoint point);
	//光标消息
	BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMmessage);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif