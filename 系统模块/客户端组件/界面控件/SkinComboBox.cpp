#include "StdAfx.h"
#include "SkinDialog.h"
#include "SkinComboBox.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
CSkinComboBoxAttribute			CSkinComboBox::m_SkinAttribute;			//按钮属性

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinComboEdit, CEdit)
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CSkinComboBox, CComboBox)
    ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_NCDESTROY()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinComboBoxAttribute::CSkinComboBoxAttribute()
{
	//设置变量
	m_crBoradHover=RGB(0,0,0);
	m_crBoradNormal=RGB(0,0,0);

	return;
}

//析构函数
CSkinComboBoxAttribute::~CSkinComboBoxAttribute() 
{
}

//更新资源
bool CSkinComboBoxAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
{
	//获取信息
	tagSkinComboBoxResource SkinComboBoxResource;
	HINSTANCE hResResource=pISkinResource->GetResInstance();

	//获取资源
	if (pISkinResource!=NULL) pISkinResource->GetComboBoxResource(SkinComboBoxResource);

	//设置变量
	m_crBoradHover=SkinComboBoxResource.crBoradHover;
	m_crBoradNormal=SkinComboBoxResource.crBoradNormal;

	//加载资源
	m_ImageButtonThumb.SetLoadInfo(SkinComboBoxResource.uThumb,hResResource);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinComboEdit::CSkinComboEdit()
{
	//设置状态
	m_bHovering=false;

	return;
}

//析构函数
CSkinComboEdit::~CSkinComboEdit()
{
}

//销毁消息
VOID CSkinComboEdit::OnNcDestroy()
{
	//设置状态
	m_bHovering=false;

	__super::OnNcDestroy();
}

//鼠标消息
VOID CSkinComboEdit::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	if (m_bHovering==false)
	{
		//设置变量
		m_bHovering=true;

		//更新界面
		Invalidate();

		//变量定义
		TRACKMOUSEEVENT TrackMouseEvent;
		ZeroMemory(&TrackMouseEvent,sizeof(TrackMouseEvent));

		//注册消息
		TrackMouseEvent.hwndTrack=m_hWnd;
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);

		//注册事件
		::TrackMouseEvent(&TrackMouseEvent);
	}

	return;
}

//鼠标消息
LRESULT CSkinComboEdit::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	//设置变量
	m_bHovering=false;

	//更新界面
	Invalidate();

	return 0;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinComboBox::CSkinComboBox()
{
	//设置状态
	m_bHovering=false;

	return;
}

//析构函数
CSkinComboBox::~CSkinComboBox()
{
}

//控件绑定
VOID CSkinComboBox::PreSubclassWindow ()
{
	//绑定控件
    HWND hEdit=::GetWindow(GetSafeHwnd(),GW_CHILD);
    if (hEdit!=NULL) m_SkinComboEdit.SubclassWindow(hEdit);

	return;
}

//重画消息
VOID CSkinComboBox::OnPaint()
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//按钮区域
    CRect rcThumb;
	rcThumb.SetRect(rcClient.right-GetSystemMetrics(SM_CXHTHUMB)-1,rcClient.top+1,rcClient.right-1, rcClient.bottom-1);

	//默认处理
    ValidateRect(rcThumb);
    Default();

	//资源变量
    CClientDC ClientDC(this);

	//状态变量
	bool bDisable=(IsWindowEnabled()==FALSE);
	bool bDropped=(DefWindowProc(CB_GETDROPPEDSTATE,0L,0L)!=0L);
	bool bHovering=(m_bHovering==true)||(m_SkinComboEdit.m_bHovering==true);

	//绘画背景
	if (bDisable==false)
	{
		COLORREF crColor=GetSysColor(COLOR_WINDOW);
		ClientDC.Draw3dRect(1,1,rcClient.Width()-rcThumb.Width()-2,rcClient.Height()-2,crColor,crColor);
	}
	else
	{
		COLORREF crColor=CSkinDialog::m_SkinAttribute.m_crBackGround;
		ClientDC.Draw3dRect(1,1,rcClient.Width()-rcThumb.Width()-2,rcClient.Height()-2,crColor,crColor);
	}

	//绘画边框
	if ((bDisable==false)&&(bHovering==true))
	{
		ClientDC.Draw3dRect(&rcClient,m_SkinAttribute.m_crBoradHover,m_SkinAttribute.m_crBoradHover);
	}
	else
	{
		ClientDC.Draw3dRect(&rcClient,m_SkinAttribute.m_crBoradNormal,m_SkinAttribute.m_crBoradNormal);
	}

	//计算位置
	INT nImageIndex=0;
	if (bDisable==true) nImageIndex=4;
	else if (bDropped==true) nImageIndex=1;
	else if (bHovering==true) nImageIndex=3;

	//绘画按钮
	CImageHandle HandleThumb(&m_SkinAttribute.m_ImageButtonThumb);
	INT nButtonWidth=m_SkinAttribute.m_ImageButtonThumb.GetWidth()/5L;
	m_SkinAttribute.m_ImageButtonThumb.BitBlt(ClientDC,rcThumb.left,rcThumb.top,nButtonWidth,rcThumb.Height(),nImageIndex*nButtonWidth,0);

	return;
}

//销毁消息
VOID CSkinComboBox::OnNcDestroy()
{
	//设置状态
	m_bHovering=false;

	__super::OnNcDestroy();
}

//创建消息
INT CSkinComboBox::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//绑定控件
    HWND hEdit=::GetWindow(GetSafeHwnd(),GW_CHILD);
    if (hEdit!=NULL) m_SkinComboEdit.SubclassWindow(hEdit);

	return 0;
}

//鼠标消息
VOID CSkinComboBox::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	if (m_bHovering==false)
	{
		//设置变量
		m_bHovering=true;

		//更新界面
		Invalidate();

		//变量定义
		TRACKMOUSEEVENT TrackMouseEvent;
		ZeroMemory(&TrackMouseEvent,sizeof(TrackMouseEvent));

		//注册消息
		TrackMouseEvent.hwndTrack=m_hWnd;
		TrackMouseEvent.dwFlags=TME_LEAVE;
		TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);

		//注册事件
		::TrackMouseEvent(&TrackMouseEvent);
	}

	return;
}

//鼠标消息
LRESULT CSkinComboBox::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	//设置变量
	m_bHovering=false;

	//更新界面
	Invalidate();

	return 0;
}

//////////////////////////////////////////////////////////////////////////
