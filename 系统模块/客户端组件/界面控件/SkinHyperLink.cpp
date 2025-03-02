#include "Stdafx.h"
#include "Resource.h"
#include "SkinHyperLink.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
CSkinHyperLinkAttribute				CSkinHyperLink::m_SkinAttribute;	//控件属性

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinHyperLink, CStatic)
	ON_WM_PAINT()
	ON_WM_CREATE()
    ON_WM_SETCURSOR()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_SETTEXT, OnSetTextMesage)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
	ON_CONTROL_REFLECT(STN_CLICKED, OnStnClicked)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinHyperLinkAttribute::CSkinHyperLinkAttribute()
{
	//设置变量
	m_crBackColor=RGB(0,0,0);
	m_crHoverText=RGB(0,0,0);
	m_crNormalText=RGB(0,0,0);
	m_crVisitedText=RGB(0,0,0);

	return;
}

//析构函数
CSkinHyperLinkAttribute::~CSkinHyperLinkAttribute()
{
}

//加载配置
bool CSkinHyperLinkAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
{
	//获取信息
	tagSkinHyperLinkResource SkinHyperLinkResource;
	if (pISkinResource!=NULL) pISkinResource->GetHyperLinkResource(SkinHyperLinkResource);

	//设置变量
	m_crBackColor=SkinHyperLinkResource.crBackColor;
	m_crHoverText=SkinHyperLinkResource.crHoverText;
	m_crNormalText=SkinHyperLinkResource.crNormalText;
	m_crVisitedText=SkinHyperLinkResource.crVisitedText;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinHyperLink::CSkinHyperLink()
{
	//配置变量
	m_bUnderLine=true;
	m_AutoRectify=true;

	//颜色变量
	m_crBackColor=m_SkinAttribute.m_crBackColor;
	m_crHoverText=m_SkinAttribute.m_crHoverText;
	m_crNormalText=m_SkinAttribute.m_crNormalText;
	m_crVisitedText=m_SkinAttribute.m_crVisitedText;

	//状态变量
	m_bVisited=false;
	m_bHovering=false;

	//组件变量
	m_pISkinMouseEvent=NULL;

	return;
}

//析构函数
CSkinHyperLink::~CSkinHyperLink()
{
}

//鼠标接口
bool CSkinHyperLink::SetSkinMouseEvent(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ISkinMouseEvent)!=NULL);
		m_pISkinMouseEvent=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ISkinMouseEvent);

		//成功判断
		if (m_pISkinMouseEvent==NULL) return false;
	}
	else m_pISkinMouseEvent=NULL;

	return true;
}

//获取接口
VOID * CSkinHyperLink::GetSkinMouseEvent(const IID & Guid, DWORD dwQueryVer) 
{ 
	if (m_pISkinMouseEvent==NULL) return NULL;
	return m_pISkinMouseEvent->QueryInterface(Guid,dwQueryVer);
}

//消息解释
BOOL CSkinHyperLink::PreTranslateMessage(MSG * pMsg)
{
	//提示过虑
	if (m_ToolTipCtrl.m_hWnd!=NULL) m_ToolTipCtrl.RelayEvent(pMsg);

	return __super::PreTranslateMessage(pMsg);
}

//控件子类化
VOID CSkinHyperLink::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//初始化
	InitHyperLink();

	return;
}

//建立消息
INT CSkinHyperLink::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//初始化
	InitHyperLink();

	return 0;
}

//绘画消息
VOID CSkinHyperLink::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//绘画背景
	dc.FillSolidRect(&rcClient,m_crBackColor);

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(m_SkinAttribute.GetDefaultFont());

	//设置颜色
	if (m_bHovering==true) dc.SetTextColor(m_crHoverText);
	else if (m_bVisited==true) dc.SetTextColor(m_crVisitedText);
	else dc.SetTextColor(m_crNormalText);

	//获取格式
	LONG dwStyte=GetWindowLong(m_hWnd,GWL_STYLE);
	UINT uFormat=DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
	if (dwStyte&SS_RIGHT) uFormat|=DT_RIGHT;
	else if (dwStyte&SS_CENTER) uFormat|=DT_CENTER;
	else uFormat|=DT_LEFT;

	//绘画文字
	CString strText;
	GetWindowText(strText);
	dc.DrawText(strText,&rcClient,uFormat);

	return;
}

//光标消息
BOOL CSkinHyperLink::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//设置光标
	::SetCursor(LoadCursor(GetModuleHandle(SKIN_CONTROL_DLL_NAME),MAKEINTRESOURCE(IDC_HAND_CUR)));

	return TRUE;
}

//单击控件
VOID CSkinHyperLink::OnStnClicked()
{
	//判断状态
	if ((m_pISkinMouseEvent!=NULL)||(m_strHyperLink.IsEmpty()))	return;

	//重画控件
	if (m_bVisited==false)
	{
		m_bVisited=true;
		Invalidate(FALSE);
	}

	//打开网页
	ShellExecute(NULL,TEXT("open"),m_strHyperLink,NULL,NULL,SW_SHOW);

	return;
}

//鼠标消息
VOID CSkinHyperLink::OnMouseMove(UINT nFlags, CPoint Point)
{
	//状态判断
	if (m_bHovering==true) return;

	//设置变量
	m_bHovering=true;

	//注册消息
	TRACKMOUSEEVENT TrackMouseEvent;
	TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);
	TrackMouseEvent.dwFlags=TME_LEAVE;
	TrackMouseEvent.hwndTrack=GetSafeHwnd();
	TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
	_TrackMouseEvent(&TrackMouseEvent);

	//重画窗口
	Invalidate(FALSE);

	__super::OnMouseMove(nFlags, Point);
}

//鼠标消息
LRESULT CSkinHyperLink::OnMouseLeave(WPARAM wParam, LPARAM lParam)
{
	//设置变量
	m_bHovering=false;

	//重画控件
	Invalidate(FALSE);

	return 0;
}

//标题消息
LRESULT	CSkinHyperLink::OnSetTextMesage(WPARAM wParam, LPARAM lParam)
{
	//默认调用
	LRESULT lResult=Default();

	//调整控件
	if (m_AutoRectify==true) RectifyControl();

	return lResult;
}

//设置访问
VOID CSkinHyperLink::SetVisited(bool bVisited)
{
	//状态判断
	if (m_bVisited==bVisited) return;

	//设置变量
	m_bVisited=bVisited;

	//重画窗口
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//设置下划线
VOID CSkinHyperLink::SetUnderLine(bool bUnderLine)
{
	//状态判断
	if (m_bUnderLine==bUnderLine) return;

	//设置变量
	m_bUnderLine=bUnderLine;

	//重画窗口
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//设置调整
VOID CSkinHyperLink::SetAutoRectify(bool bAutoRectify)
{
	//状态判断
	if (m_AutoRectify==bAutoRectify) return;

	//设置变量
	m_AutoRectify=bAutoRectify;

	//调整控件
	if (m_AutoRectify==true) RectifyControl();

	return;
}

//背景颜色
VOID CSkinHyperLink::SetBackGroundColor(COLORREF crColor)
{
	//设置变量
	m_crBackColor=crColor;

	//重画界面
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//字体颜色
VOID CSkinHyperLink::SetHyperLinkTextColor(COLORREF crHoverText, COLORREF crNormalText, COLORREF crVisitedText)
{
	//设置变量
	m_crHoverText=crHoverText;
	m_crNormalText=crNormalText;
	m_crVisitedText=crVisitedText;

	//重画界面
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//设置 URL
VOID CSkinHyperLink::SetHyperLinkUrl(LPCTSTR pszHyperLink)
{
	//设置变量
	m_strHyperLink=pszHyperLink;

	//更新提示
	UpdateToolTip();

	return;
}

//初始化
VOID CSkinHyperLink::InitHyperLink()
{
	//设置属性
	m_crBackColor=m_SkinAttribute.m_crBackColor;
	SetWindowLong(m_hWnd,GWL_STYLE,GetStyle()|SS_NOTIFY);

	//调整窗口
	if (m_AutoRectify=true) RectifyControl();

	//更新提示
	UpdateToolTip();

	return;
}

//更新提示
VOID CSkinHyperLink::UpdateToolTip()
{
	return;
	//状态判断
	if (m_hWnd==NULL) return;

	//创建控件
	if (m_ToolTipCtrl.m_hWnd==NULL) m_ToolTipCtrl.Create(this);

	//设置提示
	if (m_strHyperLink.IsEmpty()==false)
	{
		CRect rcClient;
		GetClientRect(&rcClient);
		m_ToolTipCtrl.Activate(TRUE);
		m_ToolTipCtrl.AddTool(this,m_strHyperLink,&rcClient,10);
	}
	else m_ToolTipCtrl.Activate(FALSE);

	return;
}

//调整位置
VOID CSkinHyperLink::RectifyControl()
{
	//获取标题
	CString strText;
	GetWindowText(strText);

	//获取位置
	CRect ControlRect;
	GetWindowRect(&ControlRect);
	GetParent()->ScreenToClient(&ControlRect);

	//计算长度
	CDC * pDC=GetDC();
	pDC->SelectObject(&m_SkinAttribute.m_DefaultFont);
	CSize Size=pDC->GetTextExtent(strText);
	ReleaseDC(pDC);

	//移动位置
	MoveWindow(ControlRect.left,ControlRect.top,Size.cx,Size.cy);

	return;
}

//默认回调函数
LRESULT CSkinHyperLink::DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	//事件处理
	if (m_pISkinMouseEvent!=NULL)
	{
		switch (uMessage)
		{
		case WM_MOUSEMOVE:
		case WM_MOUSELEAVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
			{
				UINT uControlID=GetWindowLong(m_hWnd,GWL_ID);
				m_pISkinMouseEvent->OnEventMouse(uMessage,uControlID,(UINT)wParam,LOWORD(lParam),HIWORD(lParam));
				break;
			}
		}
	}

	return __super::DefWindowProc(uMessage, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////

