#include "Stdafx.h"
#include "SkinListCtrl.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinHeaderCtrl, CHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CSkinListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//静态变量
CSkinListCtrlAttribute				CSkinListCtrl::m_SkinAttribute;		//列表属性

//默认属性
#define LIST_STYTE					LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE|LVS_EX_UNDERLINEHOT|LVS_EX_FLATSB

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinListCtrlAttribute::CSkinListCtrlAttribute()
{
	//颜色变量
	m_crTitle=RGB(0,0,0);
	m_crListTX=RGB(0,0,0);
	m_crListBK=RGB(0,0,0);

	return;
}

//析构函数
CSkinListCtrlAttribute::~CSkinListCtrlAttribute()
{
}

//更新资源
bool CSkinListCtrlAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
{
	//获取信息
	tagSkinListCtrlResource SkinListCtrlResource;
	HINSTANCE hResResource=pISkinResource->GetResInstance();

	//获取资源
	if (pISkinResource!=NULL)pISkinResource->GetListCtrlResource(SkinListCtrlResource);

	//标题属性
	m_crTitle=SkinListCtrlResource.crTitle;
	m_crListTX=SkinListCtrlResource.crListTX;
	m_crListBK=SkinListCtrlResource.crListBK;

	//标题信息
	m_SkinImageL.SetLoadInfo(SkinListCtrlResource.uImageL,hResResource);
	m_SkinImageM.SetLoadInfo(SkinListCtrlResource.uImageM,hResResource);
	m_SkinImageR.SetLoadInfo(SkinListCtrlResource.uImageR,hResResource);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinHeaderCtrl::CSkinHeaderCtrl()
{
	//设置变量
	m_nColumn=-1;
	m_SortType=SortType_NoSort;

	return;
}

//析构函数
CSkinHeaderCtrl::~CSkinHeaderCtrl()
{
}

//重画函数
VOID CSkinHeaderCtrl::OnPaint() 
{
	CPaintDC dc(this);

	//获取位置
	CRect rcRect;
	GetClientRect(&rcRect);

	//建立缓冲
	CDC BufferDC;
	CBitmap BufferBmp;
	BufferDC.CreateCompatibleDC(&dc);
	BufferBmp.CreateCompatibleBitmap(&dc,rcRect.Width(),rcRect.Height());

	//设置 DC
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SelectObject(&BufferBmp);
	BufferDC.SetTextColor(CSkinListCtrl::m_SkinAttribute.m_crTitle);
	BufferDC.SelectObject(CSkinListCtrl::m_SkinAttribute.GetDefaultFont());

	//加载位图
	CImageHandle ImageHandleL(&CSkinListCtrl::m_SkinAttribute.m_SkinImageL);
	CImageHandle ImageHandleM(&CSkinListCtrl::m_SkinAttribute.m_SkinImageM);
	CImageHandle ImageHandleR(&CSkinListCtrl::m_SkinAttribute.m_SkinImageR);

	//绘画背景
	ImageHandleM->StretchBlt(BufferDC,0,0,rcRect.Width(),rcRect.Height());

	//绘画子项
	CRect rcItem;
	HDITEM HDItem;
	TCHAR szBuffer[64];
	for (INT i=0;i<GetItemCount();i++)
	{
		//构造变量
		HDItem.mask=HDI_TEXT;
		HDItem.pszText=szBuffer;
		HDItem.cchTextMax=CountArray(szBuffer);

		//获取信息
		GetItem(i,&HDItem);
		GetItemRect(i,&rcItem);

		//绘画背景
		INT nLeftWidth=ImageHandleL->GetWidth();
		INT nRightWidth=ImageHandleR->GetWidth();
		ImageHandleL->StretchBlt(BufferDC,rcItem.left,rcItem.top,nLeftWidth,rcItem.Height());
		ImageHandleM->StretchBlt(BufferDC,rcItem.left+nLeftWidth,rcItem.top,rcItem.Width()-nLeftWidth-nRightWidth,rcItem.Height());
		ImageHandleR->StretchBlt(BufferDC,rcItem.right-nRightWidth,rcItem.top,nRightWidth,rcItem.Height());

		//绘画标题
		rcItem.DeflateRect(3,1,3,1);
		const UINT uFormat=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_SINGLELINE|DT_VCENTER|DT_CENTER;
		BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcItem,uFormat);
	}

	//绘画界面
	dc.BitBlt(0,0,rcRect.Width(),rcRect.Height(),&BufferDC,0,0,SRCCOPY);

	//清理资源
	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();
	
	return;
}

//背景函数
BOOL CSkinHeaderCtrl::OnEraseBkgnd(CDC * pDC) 
{
	//刷新界面
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//设置标志
VOID CSkinHeaderCtrl::SetSortArrow(INT nSortColumn, enSortType SortType)
{
	//设置变量
	m_SortType=SortType;
	m_nColumn=nSortColumn;

	//重画界面
	Invalidate(FALSE);

	return;
}

//获取标志
VOID CSkinHeaderCtrl::GetSortArrow(INT & nSortColumn, enSortType & SortType)
{
	//设置变量
	SortType=m_SortType;
	nSortColumn=m_nColumn;

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinListCtrl::CSkinListCtrl()
{
}

//析构函数
CSkinListCtrl::~CSkinListCtrl()
{
}

//控件绑定
VOID CSkinListCtrl::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//设置滚动
	m_SkinScrollBar.InitScroolBar(this);

	//设置属性
	SetExtendedStyle(LIST_STYTE);
	m_SkinHeadCtrl.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());

	//设置颜色
	SetBkColor(m_SkinAttribute.m_crListBK);
	SetTextColor(m_SkinAttribute.m_crListTX);
	SetTextBkColor(m_SkinAttribute.m_crListBK);

	return;
}

//建立消息
INT CSkinListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置滚动
	m_SkinScrollBar.InitScroolBar(this);
	
	//设置属性
	SetExtendedStyle(LIST_STYTE);
	m_SkinHeadCtrl.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());

	return 0;
}

//绘画背景
BOOL CSkinListCtrl::OnEraseBkgnd(CDC * pDC)
{
	if (m_SkinHeadCtrl.m_hWnd!=NULL)
	{
		//获取位置
		CRect rcRect;
		CRect rcHeader;
		GetClientRect(&rcRect);
		m_SkinHeadCtrl.GetWindowRect(&rcHeader);

		//绘画背景
		rcRect.top=rcHeader.Height();
		pDC->FillSolidRect(&rcRect,GetBkColor());

		return TRUE;
	}

	return __super::OnEraseBkgnd(pDC);
}

//////////////////////////////////////////////////////////////////////////
