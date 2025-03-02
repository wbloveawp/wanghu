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

//Ĭ������
#define LIST_STYTE						LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE|LVS_EX_FLATSB|LVS_EX_GRIDLINES

//////////////////////////////////////////////////////////////////////////

//���캯��
CListCtrlAttribute::CListCtrlAttribute()
{
	//�б�����
	m_wItemHeight = 18;

	//��ɫ����
	m_crTitle=RGB(0,0,0);
	m_crNormalBK=GetSysColor(COLOR_WINDOW);
	m_crNormalTX=GetSysColor(COLOR_WINDOWTEXT);	
	m_crSelectedBK=GetSysColor(COLOR_HIGHLIGHT);
	m_crSelectedTX=GetSysColor(COLOR_HIGHLIGHTTEXT);

	return;
}

//��������
CListCtrlAttribute::~CListCtrlAttribute()
{
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CCustomHeaderCtrl::CCustomHeaderCtrl()
{
	//���ñ���
	m_nColumn=-1;
	m_SortType=CST_NoSort;

	return;
}

//��������
CCustomHeaderCtrl::~CCustomHeaderCtrl()
{
}

//�ػ�����
VOID CCustomHeaderCtrl::OnPaint() 
{
	CPaintDC dc(this);

	//��ȡλ��
	CRect rcRect;
	GetClientRect(&rcRect);

	//��������
	CDC BufferDC;
	CBitmap BufferBmp;
	BufferDC.CreateCompatibleDC(&dc);
	BufferBmp.CreateCompatibleBitmap(&dc, rcRect.Width(), rcRect.Height());

	//����
	CFont TitleFont;
	TitleFont.CreatePointFont(100, TEXT("����"));

	//���� DC
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SelectObject(&BufferBmp);
	BufferDC.SetTextColor(RGB(0, 0, 0));
	BufferDC.SelectObject(TitleFont);

	//��ȡ��ɫ
	COLORREF crBkColor = GetBkColor(dc);
	COLORREF crTextBkColor = GetBkColor(dc);

	//�滭����
	BufferDC.FillSolidRect(0, 0, rcRect.Width(), rcRect.Height(), crBkColor);

	//�滭����
	CRect rcItem;
	HDITEM HDItem;
	TCHAR szBuffer[64];
	for (INT i = 0; i < GetItemCount(); i++)
	{
		//�������
		HDItem.mask = HDI_TEXT;
		HDItem.pszText = szBuffer;
		HDItem.cchTextMax = CountArray(szBuffer);

		//��ȡ��Ϣ
		GetItem(i, &HDItem);
		GetItemRect(i, &rcItem);
		BufferDC.FillSolidRect(rcItem.left, rcItem.top + 1, 1, rcItem.Height() - 2, RGB(100, 100, 100));

		//�滭����
		rcItem.DeflateRect(3, 1, 3, 1);
		const UINT uFormat = DT_WORDBREAK | DT_EDITCONTROL | DT_END_ELLIPSIS | DT_SINGLELINE | DT_VCENTER | DT_CENTER;
		BufferDC.DrawText(szBuffer, lstrlen(szBuffer), &rcItem, uFormat);
	}

	//�滭����
	dc.BitBlt(0, 0, rcRect.Width(), rcRect.Height(), &BufferDC, 0, 0, SRCCOPY);

	//������Դ
	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();

	return;
}

//��������
BOOL CCustomHeaderCtrl::OnEraseBkgnd(CDC * pDC) 
{
	//ˢ�½���
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//���ñ�־
VOID CCustomHeaderCtrl::SetSortArrow(INT nSortColumn, enCustomSortType SortType)
{
	//���ñ���
	m_SortType=SortType;
	m_nColumn=nSortColumn;

	//�ػ�����
	Invalidate(FALSE);

	return;
}

//��ȡ��־
VOID CCustomHeaderCtrl::GetSortArrow(INT & nSortColumn, enCustomSortType& SortType)
{
	//���ñ���
	SortType=m_SortType;
	nSortColumn=m_nColumn;

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CCustomListCtrl::CCustomListCtrl()
{
}

//��������
CCustomListCtrl::~CCustomListCtrl()
{
}

//�ؼ���
VOID CCustomListCtrl::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//��������
	SetExtendedStyle(LIST_STYTE);	
	m_CustomHeadCtrl.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());

	//������ɫ
	//SetBkColor(m_ListCtrlAttribute.m_crNormalBK);
	//SetTextColor(m_ListCtrlAttribute.m_crNormalTX);
	//SetTextBkColor(m_ListCtrlAttribute.m_crNormalBK);

	//��ȡ����
	CRect rcwin;
	GetWindowRect(rcwin);

	//������Ϣ
	WINDOWPOS wp;
	wp.hwnd = m_hWnd;
	wp.cx = rcwin.Width();
	wp.cy = rcwin.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage(WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp);	

	return;
}

//�滭����
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

	//��������
	CRect rcItem = lpDrawItemStruct->rcItem;
	CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);

	//��ȡ����
	INT nItemID = lpDrawItemStruct->itemID;
	INT nColumnCount = GetHeaderCtrl()->GetItemCount();

	//���Ʊ���
	COLORREF crTextColor = m_ListCtrlAttribute.m_crNormalTX;
	COLORREF crBackColor = m_ListCtrlAttribute.m_crNormalBK;

	//��ɫ����
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		crTextColor = m_ListCtrlAttribute.m_crSelectedTX;
		crBackColor = m_ListCtrlAttribute.m_crSelectedBK;
	}

	//�滭����
	CRect rcClipBox;
	pDC->GetClipBox(&rcClipBox);

	//���� DC
	pDC->SetBkColor(crBackColor);
	pDC->SetTextColor(crTextColor);

	//�滭ѡ��
	/*if (lpDrawItemStruct->itemState & ODS_FOCUS)
	{
		COLORREF rcFillColor = GetBkColor();
		pDC->Draw3dRect(&lpDrawItemStruct->rcItem, rcFillColor, rcFillColor);
	}*/
	
	//�滭����
	for (INT i = 0; i < nColumnCount; i++)
	{
		//��ȡλ��
		CRect rcSubItem;
		GetSubItemRect(nItemID, i, LVIR_LABEL, rcSubItem);

		//�滭�ж�
		if (rcSubItem.left > rcClipBox.right) break;
		if (rcSubItem.right < rcClipBox.left) continue;
		if (rcSubItem.left == rcSubItem.right) continue;

		//��䱳��
		pDC->FillSolidRect(&rcSubItem, crBackColor);

		//��ȡ����
		TCHAR szString[256] = TEXT("");
		GetItemText(nItemID, i, szString, CountArray(szString));

		//�滭����
		rcSubItem.left += 5;
		pDC->DrawText(szString, lstrlen(szString), &rcSubItem, DT_VCENTER | DT_SINGLELINE | DT_END_ELLIPSIS);
	}		

	//�滭ѡ��
	//if (lpDrawItemStruct->itemState & ODS_FOCUS) pDC->DrawFocusRect(&rcItem);

	return;
}

//������Ϣ
INT CCustomListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//��������
	SetExtendedStyle(LIST_STYTE);
	m_CustomHeadCtrl.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());

	return 0;
}

//�滭����
VOID CCustomListCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	lpMeasureItemStruct->itemHeight = m_ListCtrlAttribute.m_wItemHeight;
}

//�滭����
BOOL CCustomListCtrl::OnEraseBkgnd(CDC * pDC)
{
	if (m_CustomHeadCtrl.m_hWnd!=NULL)
	{
		//��ȡλ��
		CRect rcRect;
		CRect rcHeader;
		GetClientRect(&rcRect);
		m_CustomHeadCtrl.GetWindowRect(&rcHeader);

		//�滭����
		rcRect.top=rcHeader.Height();
		pDC->FillSolidRect(&rcRect,GetBkColor());

		return TRUE;
	}

	return __super::OnEraseBkgnd(pDC);
}

//////////////////////////////////////////////////////////////////////////
