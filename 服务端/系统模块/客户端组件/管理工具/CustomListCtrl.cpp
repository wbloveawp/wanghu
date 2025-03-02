#include "Stdafx.h"
#include "CustomListCtrl.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCustomHeaderCtrl, CHeaderCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CCustomListCtrl, CListCtrl)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	//ON_WM_MEASUREITEM()
	ON_WM_MEASUREITEM_REFLECT()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//默认属性
#define LIST_STYTE						LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE|LVS_EX_FLATSB|LVS_EX_GRIDLINES

//////////////////////////////////////////////////////////////////////////

//构造函数
CListCtrlAttribute::CListCtrlAttribute()
{
	//列表属性
	m_wItemHeight = 18;

	//颜色变量
	m_crTitle=RGB(0,0,0);
	m_crNormalBK=GetSysColor(COLOR_WINDOW);
	m_crNormalTX=GetSysColor(COLOR_WINDOWTEXT);	
	m_crSelectedBK=GetSysColor(COLOR_HIGHLIGHT);
	m_crSelectedTX=GetSysColor(COLOR_HIGHLIGHTTEXT);

	return;
}

//析构函数
CListCtrlAttribute::~CListCtrlAttribute()
{
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CCustomHeaderCtrl::CCustomHeaderCtrl()
{
	//设置变量
	m_nColumn=-1;
	m_SortType=CST_NoSort;

	return;
}

//析构函数
CCustomHeaderCtrl::~CCustomHeaderCtrl()
{
}

//重画函数
VOID CCustomHeaderCtrl::OnPaint() 
{
	CPaintDC dc(this);

	//获取位置
	CRect rcRect;
	GetClientRect(&rcRect);

	//建立缓冲
	CDC BufferDC;
	CBitmap BufferBmp;
	BufferDC.CreateCompatibleDC(&dc);
	BufferBmp.CreateCompatibleBitmap(&dc, rcRect.Width(), rcRect.Height());

	//字体
	CFont TitleFont;
	TitleFont.CreatePointFont(100, TEXT("宋体"));

	//设置 DC
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SelectObject(&BufferBmp);
	BufferDC.SetTextColor(RGB(0, 0, 0));
	BufferDC.SelectObject(TitleFont);

	//获取颜色
	COLORREF crBkColor = GetBkColor(dc);
	COLORREF crTextBkColor = GetBkColor(dc);

	//绘画背景
	BufferDC.FillSolidRect(0, 0, rcRect.Width(), rcRect.Height(), crBkColor);

	//绘画子项
	CRect rcItem;
	HDITEM HDItem;
	TCHAR szBuffer[64];
	for (INT i = 0; i < GetItemCount(); i++)
	{
		//构造变量
		HDItem.mask = HDI_TEXT;
		HDItem.pszText = szBuffer;
		HDItem.cchTextMax = CountArray(szBuffer);

		//获取信息
		GetItem(i, &HDItem);
		GetItemRect(i, &rcItem);
		BufferDC.FillSolidRect(rcItem.left, rcItem.top + 1, 1, rcItem.Height() - 2, RGB(100, 100, 100));

		//绘画标题
		rcItem.DeflateRect(3, 1, 3, 1);
		const UINT uFormat = DT_WORDBREAK | DT_EDITCONTROL | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER | DT_CENTER;
		BufferDC.DrawText(szBuffer, lstrlen(szBuffer), &rcItem, uFormat);
	}

	//绘画界面
	dc.BitBlt(0, 0, rcRect.Width(), rcRect.Height(), &BufferDC, 0, 0, SRCCOPY);

	//清理资源
	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();

	return;
}

//背景函数
BOOL CCustomHeaderCtrl::OnEraseBkgnd(CDC * pDC) 
{
	//刷新界面
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//设置标志
VOID CCustomHeaderCtrl::SetSortArrow(INT nSortColumn, enCustomSortType SortType)
{
	//设置变量
	m_SortType=SortType;
	m_nColumn=nSortColumn;

	//重画界面
	Invalidate(FALSE);

	return;
}

//获取标志
VOID CCustomHeaderCtrl::GetSortArrow(INT & nSortColumn, enCustomSortType& SortType)
{
	//设置变量
	SortType=m_SortType;
	nSortColumn=m_nColumn;

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CCustomListCtrl::CCustomListCtrl()
{
}

//析构函数
CCustomListCtrl::~CCustomListCtrl()
{
}

//控件绑定
VOID CCustomListCtrl::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//设置属性
	SetExtendedStyle(LIST_STYTE);	
	m_CustomHeadCtrl.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());

	//设置颜色
	//SetBkColor(m_ListCtrlAttribute.m_crNormalBK);
	//SetTextColor(m_ListCtrlAttribute.m_crNormalTX);
	//SetTextBkColor(m_ListCtrlAttribute.m_crNormalBK);

	//获取区域
	CRect rcwin;
	GetWindowRect(rcwin);

	//发送消息
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcwin.Width();
	wp.cy = rcwin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);	

	return;
}

//绘画函数
VOID CCustomListCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	//TCHAR lpBuffer[256];

	//LV_ITEM lvi;

	//lvi.mask = LVIF_TEXT | LVIF_PARAM;
	//lvi.iItem = lpDrawItemStruct->itemID;
	//lvi.iSubItem = 0;
	//lvi.pszText = lpBuffer;
	//lvi.cchTextMax = sizeof(lpBuffer);
	//VERIFY(GetItem(&lvi));

	//LV_COLUMN lvc, lvcprev;
	//::ZeroMemory(&lvc, sizeof(lvc));
	//::ZeroMemory(&lvcprev, sizeof(lvcprev));
	//lvc.mask = LVCF_WIDTH | LVCF_FMT;
	//lvcprev.mask = LVCF_WIDTH | LVCF_FMT;

	//for (int nCol = 0; GetColumn(nCol, &lvc); nCol++)
	//{
	//	if (nCol > 0)
	//	{
	//		// Get Previous Column Width in order to move the next display item
	//		GetColumn(nCol - 1, &lvcprev);
	//		lpDrawItemStruct->rcItem.left += lvcprev.cx+1;
	//		lpDrawItemStruct->rcItem.right += lpDrawItemStruct->rcItem.left;
	//	}

	//	// Get the text 
	//	::ZeroMemory(&lvi, sizeof(lvi));
	//	lvi.iItem = lpDrawItemStruct->itemID;
	//	lvi.mask = LVIF_TEXT | LVIF_PARAM;
	//	lvi.iSubItem = nCol;
	//	lvi.pszText = lpBuffer;
	//	lvi.cchTextMax = sizeof(lpBuffer);
	//	VERIFY(GetItem(&lvi));

	//	CDC* pDC;
	//	pDC = CDC::FromHandle(lpDrawItemStruct->hDC);


	//	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	//	{
	//		pDC->FillSolidRect(&lpDrawItemStruct->rcItem, GetSysColor(COLOR_HIGHLIGHT));
	//		pDC->SetTextColor(GetSysColor(COLOR_HIGHLIGHTTEXT));
	//	}
	//	else
	//	{
	//		pDC->FillSolidRect(&lpDrawItemStruct->rcItem, GetSysColor(COLOR_WINDOW));
	//		pDC->SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
	//	}

	//	pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));

	//	UINT   uFormat = DT_LEFT| DT_VCENTER;

	//	::DrawText(lpDrawItemStruct->hDC, lpBuffer, lstrlen(lpBuffer), &lpDrawItemStruct->rcItem, uFormat);

	//	pDC->SelectStockObject(SYSTEM_FONT);
	//}

	//变量定义
	CRect rcItem = lpDrawItemStruct->rcItem;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	//获取属性
	INT nItemID = lpDrawItemStruct->itemID;
	INT nColumnCount = GetHeaderCtrl()->GetItemCount();

	//定制变量
	COLORREF crTextColor = m_ListCtrlAttribute.m_crNormalTX;
	COLORREF crBackColor = m_ListCtrlAttribute.m_crNormalBK;

	//颜色定制
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		crTextColor = m_ListCtrlAttribute.m_crSelectedTX;
		crBackColor = m_ListCtrlAttribute.m_crSelectedBK;
	}

	//绘画区域
	CRect rcClipBox;
	pDC->GetClipBox(&rcClipBox);

	//设置 DC
	pDC->SetBkColor(crBackColor);
	pDC->SetTextColor(crTextColor);

	//绘画选择
	/*if (lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		COLORREF rcFillColor = GetBkColor();
		pDC->Draw3dRect(&lpDrawItemStruct->rcItem, rcFillColor, rcFillColor);
	}*/
	
	//绘画子项
	for (INT i = 0; i < nColumnCount; i++)
	{
		//获取位置
		CRect rcSubItem;
		GetSubItemRect(nItemID, i, LVIR_LABEL, rcSubItem);

		//绘画判断
		if (rcSubItem.left > rcClipBox.right) break;
		if (rcSubItem.right < rcClipBox.left) continue;
		if (rcSubItem.left == rcSubItem.right) continue;

		//填充背景
		pDC->FillSolidRect(&rcSubItem, crBackColor);

		//获取文字
		TCHAR szString[256] = TEXT("");
		GetItemText(nItemID, i, szString, CountArray(szString));

		//绘画文字
		rcSubItem.left += 5;
		pDC->DrawText(szString, lstrlen(szString), &rcSubItem, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
	}		

	//绘画选择
	//if (lpDrawItemStruct->itemState & ODS_FOCUS) pDC->DrawFocusRect(&rcItem);

	return;
}

//建立消息
INT CCustomListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置属性
	SetExtendedStyle(LIST_STYTE);
	m_CustomHeadCtrl.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());

	return 0;
}

//绘画函数
VOID CCustomListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = m_ListCtrlAttribute.m_wItemHeight;
}

//绘画背景
BOOL CCustomListCtrl::OnEraseBkgnd(CDC * pDC)
{
	if (m_CustomHeadCtrl.m_hWnd!=NULL)
	{
		//获取位置
		CRect rcRect;
		CRect rcHeader;
		GetClientRect(&rcRect);
		m_CustomHeadCtrl.GetWindowRect(&rcHeader);

		//绘画背景
		rcRect.top=rcHeader.Height();
		pDC->FillSolidRect(&rcRect,GetBkColor());

		return TRUE;
	}

	return __super::OnEraseBkgnd(pDC);
}

//////////////////////////////////////////////////////////////////////////
