#include "Stdafx.h"
#include "Resource.h"
#include "SkinButton.h"
#include ".\skinbutton.h"

//////////////////////////////////////////////////////////////////////////

//时间标识
#define IDI_TRANSITION				10									//过渡标识
#define TIME_TRANSITION				30									//过渡时间

//常量定义
#define MIN_TRANSITION_INDEX		0									//过渡索引
#define MAX_TRANSITION_INDEX		8									//过渡索引

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinButton, CButton)
	ON_WM_CREATE()
	ON_WM_MOUSEMOVE()
	ON_WM_NCDESTROY()
	ON_WM_SETCURSOR()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_MOUSELEAVE,OnMouseLeave)
	ON_WM_TIMER()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//静态变量
CSkinButtonAttribute				CSkinButton::m_SkinAttribute;		//按钮属性

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinButtonAttribute::CSkinButtonAttribute()
{
	//设置变量
	m_crText=RGB(0,0,0);

	return;
}

//析构函数
CSkinButtonAttribute::~CSkinButtonAttribute() 
{
}

//更新资源
bool CSkinButtonAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
{
	//获取信息
	tagSkinButtonResource SkinButtonResource;
	HINSTANCE hResResource=pISkinResource->GetResInstance();

	//获取资源
	if (pISkinResource!=NULL) pISkinResource->GetButtonResource(SkinButtonResource);

	//设置变量
	m_crText=SkinButtonResource.crButtonText;

	//加载资源
	m_ImageBack.SetLoadInfo(SkinButtonResource.uButton,hResResource);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinButton::CSkinButton()
{
	//状态变量
	m_bExpand=true;
	m_bHovering=false;
	m_bTransparent=false;

	//配置变量
	m_pISkinMouseEvent=NULL;
	m_crText=m_SkinAttribute.m_crText;

	m_wImageIndex=MIN_TRANSITION_INDEX;

	return;
}

//析构函数
CSkinButton::~CSkinButton()
{
}

//绑定函数
VOID CSkinButton::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//设置按钮
	SetButtonStyle(GetButtonStyle()|BS_OWNERDRAW);
	SendMessage(WM_SETFONT,(WPARAM)(HFONT)(m_SkinAttribute.GetDefaultFont()),TRUE);

	return;
}

//背景函数
BOOL CSkinButton::OnEraseBkgnd(CDC * pDC)
{
	//更新界面
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//建立消息
INT CSkinButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置按钮
	SetButtonStyle(GetButtonStyle()|BS_OWNERDRAW);
	SendMessage(WM_SETFONT,(WPARAM)(HFONT)(m_SkinAttribute.GetDefaultFont()),TRUE);

	return 0;
}

//设置颜色
bool CSkinButton::SetTextColor(COLORREF crText)
{
	//设置变量
	m_crText=crText;

	//更新界面
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return true;
}

//加载位图
bool CSkinButton::SetButtonImage(LPCTSTR pszFileName, bool bExpandImage, bool bTransparent)
{
	//效验参数
	ASSERT(pszFileName);
	if (pszFileName==NULL) return false;

	//设置变量
	m_bExpand=bExpandImage;
	m_bTransparent=bTransparent;
	m_ImageBack.SetLoadInfo(pszFileName);

	//更新界面
	if (m_bExpand==false) RectifyControl();

	//更新窗口
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return true;
}

//加载位图
bool CSkinButton::SetButtonImage(UINT uBitmapID, HINSTANCE hInstance, bool bExpandImage, bool bTransparent)
{
	//设置变量
	m_bExpand=bExpandImage;
	m_bTransparent=bTransparent;
	m_ImageBack.SetLoadInfo(uBitmapID,hInstance);

	//更新界面
	if (m_bExpand==false) RectifyControl();

	//更新窗口
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return true;
}

//获取位图
bool CSkinButton::GetButtonImage(tagImageLoadInfo & ImageLoadInfo)
{
	//变量定义
	bool bSuccess=false;

	//获取属性
	if (m_ImageBack.IsSetLoadInfo()==true)
	{
		bSuccess=m_ImageBack.GetLoadInfo(ImageLoadInfo);
	}
	else 
	{
		bSuccess=CSkinButton::m_SkinAttribute.m_ImageBack.GetLoadInfo(ImageLoadInfo);
	}

	return bSuccess;
}

//调整位置
VOID CSkinButton::RectifyControl()
{
	//调整效验
	ASSERT((m_ImageBack.IsSetLoadInfo()==true)&&(m_hWnd!=NULL));
	if ((m_ImageBack.IsSetLoadInfo()==false)||(m_hWnd==NULL)) return;

	//加载资源
	CImageHandle ImageHandle(&m_ImageBack);

	//调整大小
	SetWindowPos(NULL,0,0,m_ImageBack.GetWidth()/5,m_ImageBack.GetHeight(),SWP_NOMOVE|SWP_NOACTIVATE|SWP_NOZORDER);

	//透明设置
	if (m_bTransparent==true)
	{
		//创建区域
		CRgn RgnControl;
		m_ImageBack.CreateImageRegion(RgnControl,RGB(255,0,255));

		//设置区域
		SetWindowRgn(RgnControl,TRUE);
	}

	return;
}

//鼠标消息
VOID CSkinButton::OnMouseMove(UINT nFlags, CPoint Point)
{
	__super::OnMouseMove(nFlags, Point);

	if (m_bHovering==false)
	{
		//设置变量
		m_bHovering=true;

		//更新界面
		Invalidate(FALSE);

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

		//过渡处理
		if (m_wImageIndex<MAX_TRANSITION_INDEX)
		{
			SetTimer(IDI_TRANSITION,TIME_TRANSITION,NULL);
		}
	}

	return;
}

//鼠标消息
LRESULT CSkinButton::OnMouseLeave(WPARAM wparam, LPARAM lparam)
{
	//设置变量
	m_bHovering=false;

	//过渡处理
	if (m_wImageIndex>MIN_TRANSITION_INDEX)
	{
		SetTimer(IDI_TRANSITION,TIME_TRANSITION,NULL);
	}

	//更新界面
	Invalidate(FALSE);

	return 0;
}

//光标消息
BOOL CSkinButton::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//设置光标
	SetCursor(LoadCursor(GetModuleHandle(SKIN_CONTROL_DLL_NAME),MAKEINTRESOURCE(IDC_HAND_CUR)));

	return TRUE;
}

//销毁消息
VOID CSkinButton::OnNcDestroy()
{
	//状态变量
	m_bExpand=true;
	m_bHovering=false;

	//动画变量
	m_wImageIndex=MIN_TRANSITION_INDEX;

	__super::OnNcDestroy();
}

//绘画函数
VOID CSkinButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//变量定义
	HDC hDC=lpDrawItemStruct->hDC;
	CDC * pDC=CDC::FromHandle(lpDrawItemStruct->hDC);
	bool bDisable=((lpDrawItemStruct->itemState&ODS_DISABLED)!=0);
	bool bButtonDown=((lpDrawItemStruct->itemState&ODS_SELECTED)!=0);

	//获取文字
	CString strText;
	GetWindowText(strText);

	//加载资源
	CImageHandle ImageHandle;
	if (m_ImageBack.IsSetLoadInfo()==true) ImageHandle.AttachResource(&m_ImageBack);
	else ImageHandle.AttachResource(&m_SkinAttribute.m_ImageBack);

	//绘画界面
	if (ImageHandle.IsValid()==true)
	{
		//变量定义
		INT nImagePos=0;
		INT nPartWidth=ImageHandle->GetWidth()/5;

		//计算位置
		if (bDisable==true) nImagePos=nPartWidth*4;
		else if (bButtonDown==true) nImagePos=nPartWidth;
		else if (m_bHovering==true) nImagePos=nPartWidth*3;

		//状态判断
		bool bTransitionDraw=false;
		if ((m_bHovering==true)&&(m_wImageIndex<MAX_TRANSITION_INDEX)) bTransitionDraw=true;
		if ((m_bHovering==false)&&(m_wImageIndex>MIN_TRANSITION_INDEX)) bTransitionDraw=true;

		//绘画背景
		if ((bDisable==false)&&(bButtonDown==false)&&(bTransitionDraw==true))
		{
			//创建缓冲
			CBitImage BufferImage;
			BufferImage.Create(rcClient.Width(),rcClient.Height(),32);

			//获取设备
			HDC hBufferDC=BufferImage.GetDC();
			CDC * pBufferDC=CDC::FromHandle(hBufferDC);
			pBufferDC->SelectObject(pDC->GetCurrentFont());

			//绘画底图
			if (m_wImageIndex<MAX_TRANSITION_INDEX)
			{
				if (m_bExpand==false)
				{
					ImageHandle->BitBlt(hBufferDC,0,0,rcClient.Width(),rcClient.Height(),0,0,SRCCOPY);
				}
				else
				{
					ImageHandle->StretchBlt(hBufferDC,0,0,rcClient.Width(),rcClient.Height(),0,0,nPartWidth,ImageHandle->GetHeight(),SRCCOPY);
				}
			}

			//绘画过渡
			if (m_wImageIndex>MIN_TRANSITION_INDEX)
			{
				if (m_bTransparent==false)
				{
					ImageHandle->AlphaBlend(pBufferDC->GetSafeHdc(),0,0,rcClient.Width(),rcClient.Height(),
						nPartWidth*3,0,nPartWidth,ImageHandle->GetHeight(),m_wImageIndex*255/MAX_TRANSITION_INDEX);
				}
				else
				{
					ImageHandle->BlendDrawImage(pBufferDC,0,0,rcClient.Width(),rcClient.Height(),
						nPartWidth*3,0,RGB(255,0,255),m_wImageIndex*255/MAX_TRANSITION_INDEX);
				}
			}

			//调整位置
			rcClient.top++;
			if (bButtonDown==true) rcClient.top+=2;

			//绘画文字
			pBufferDC->SetBkMode(TRANSPARENT);
			pBufferDC->SetTextColor((bDisable==true)?GetSysColor(COLOR_GRAYTEXT):m_crText);
			pBufferDC->DrawText(strText,strText.GetLength(),rcClient,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

			//绘画界面
			if (m_bTransparent==false)
			{
				BufferImage.BitBlt(hDC,0,0,rcClient.Width(),rcClient.Height(),0,0);
			}
			else
			{
				BufferImage.AlphaDrawImage(pDC,0,0,rcClient.Width(),rcClient.Height(),0,0,RGB(255,0,255));
			}

			//清理资源
			BufferImage.ReleaseDC();

			return;
		}
		else
		{
			if (m_bTransparent==false)
			{
				if (m_bExpand==false) ImageHandle->BitBlt(hDC,0,0,rcClient.Width(),rcClient.Height(),nImagePos,0,SRCCOPY);
				else ImageHandle->StretchBlt(hDC,0,0,rcClient.Width(),rcClient.Height(),nImagePos,0,nPartWidth,ImageHandle->GetHeight(),SRCCOPY);
			}
			else
			{
				CDC * pDestDC=CDC::FromHandle(hDC);
				ImageHandle->AlphaDrawImage(pDestDC,0,0,rcClient.Width(),rcClient.Height(),nImagePos,0,RGB(255,0,255));
			}
		}
	}
	else
	{
		//默认界面
		CDC * pDC=CDC::FromHandle(hDC);
		pDC->FillSolidRect(&rcClient,GetSysColor(COLOR_BTNFACE));
		if (bButtonDown) pDC->Draw3dRect(&rcClient,GetSysColor(COLOR_WINDOWFRAME),GetSysColor(COLOR_3DHILIGHT));
		else pDC->Draw3dRect(&rcClient,GetSysColor(COLOR_3DHILIGHT),GetSysColor(COLOR_WINDOWFRAME));
	}

	//调整位置
	rcClient.top++;
	if (bButtonDown==true) rcClient.top+=2;

	//绘画文字
	::SetBkMode(hDC,TRANSPARENT);
	::SetTextColor(hDC,(bDisable==true)?GetSysColor(COLOR_GRAYTEXT):m_crText);
	DrawText(hDC,strText,strText.GetLength(),rcClient,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);

	return;
}

//回调函数
LRESULT CSkinButton::DefWindowProc(UINT uMessage, WPARAM wParam, LPARAM lParam)
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
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
			{
				UINT uControlID=GetWindowLong(m_hWnd,GWL_ID);
				m_pISkinMouseEvent->OnEventMouse(uMessage,uControlID,(UINT)wParam,LOWORD(lParam),HIWORD(lParam));
				break;
			}
		}
	}

	return __super::DefWindowProc(uMessage, wParam, lParam);
}

//鼠标接口
bool CSkinButton::SetSkinMouseEvent(IUnknownEx * pIUnknownEx)
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
VOID * CSkinButton::GetSkinMouseEvent(const IID & Guid, DWORD dwQueryVer) 
{ 
	if (m_pISkinMouseEvent==NULL) return NULL;
	return m_pISkinMouseEvent->QueryInterface(Guid,dwQueryVer);
}

//时间消息
void CSkinButton::OnTimer(UINT_PTR nIDEvent)
{
	//过渡变量
	if (nIDEvent==IDI_TRANSITION)
	{
		//盘旋过渡
		if ((m_bHovering==true)&&(m_wImageIndex<MAX_TRANSITION_INDEX))
		{
			m_wImageIndex++;
		}

		//常态过渡
		if ((m_bHovering==false)&&(m_wImageIndex>MIN_TRANSITION_INDEX))
		{
			m_wImageIndex--;
		}

		//删除时间
		if ((m_wImageIndex==MIN_TRANSITION_INDEX)||(m_wImageIndex==MAX_TRANSITION_INDEX))
		{
			KillTimer(IDI_TRANSITION);
		}

		//更新界面
		Invalidate(FALSE);

		return;
	}

	__super::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////////

