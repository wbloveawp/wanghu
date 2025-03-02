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

//��̬����
CSkinListCtrlAttribute				CSkinListCtrl::m_SkinAttribute;		//�б�����

//Ĭ������
#define LIST_STYTE					LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP|LVS_EX_ONECLICKACTIVATE|LVS_EX_UNDERLINEHOT|LVS_EX_FLATSB

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinListCtrlAttribute::CSkinListCtrlAttribute()
{
	//��ɫ����
	m_crTitle=RGB(0,0,0);
	m_crListTX=RGB(0,0,0);
	m_crListBK=RGB(0,0,0);

	return;
}

//��������
CSkinListCtrlAttribute::~CSkinListCtrlAttribute()
{
}

//������Դ
bool CSkinListCtrlAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
{
	//��ȡ��Ϣ
	tagSkinListCtrlResource SkinListCtrlResource;
	HINSTANCE hResResource=pISkinResource->GetResInstance();

	//��ȡ��Դ
	if (pISkinResource!=NULL)pISkinResource->GetListCtrlResource(SkinListCtrlResource);

	//��������
	m_crTitle=SkinListCtrlResource.crTitle;
	m_crListTX=SkinListCtrlResource.crListTX;
	m_crListBK=SkinListCtrlResource.crListBK;

	//������Ϣ
	m_SkinImageL.SetLoadInfo(SkinListCtrlResource.uImageL,hResResource);
	m_SkinImageM.SetLoadInfo(SkinListCtrlResource.uImageM,hResResource);
	m_SkinImageR.SetLoadInfo(SkinListCtrlResource.uImageR,hResResource);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinHeaderCtrl::CSkinHeaderCtrl()
{
	//���ñ���
	m_nColumn=-1;
	m_SortType=SortType_NoSort;

	return;
}

//��������
CSkinHeaderCtrl::~CSkinHeaderCtrl()
{
}

//�ػ�����
VOID CSkinHeaderCtrl::OnPaint() 
{
	CPaintDC dc(this);

	//��ȡλ��
	CRect rcRect;
	GetClientRect(&rcRect);

	//��������
	CDC BufferDC;
	CBitmap BufferBmp;
	BufferDC.CreateCompatibleDC(&dc);
	BufferBmp.CreateCompatibleBitmap(&dc,rcRect.Width(),rcRect.Height());

	//���� DC
	BufferDC.SetBkMode(TRANSPARENT);
	BufferDC.SelectObject(&BufferBmp);
	BufferDC.SetTextColor(CSkinListCtrl::m_SkinAttribute.m_crTitle);
	BufferDC.SelectObject(CSkinListCtrl::m_SkinAttribute.GetDefaultFont());

	//����λͼ
	CImageHandle ImageHandleL(&CSkinListCtrl::m_SkinAttribute.m_SkinImageL);
	CImageHandle ImageHandleM(&CSkinListCtrl::m_SkinAttribute.m_SkinImageM);
	CImageHandle ImageHandleR(&CSkinListCtrl::m_SkinAttribute.m_SkinImageR);

	//�滭����
	ImageHandleM->StretchBlt(BufferDC,0,0,rcRect.Width(),rcRect.Height());

	//�滭����
	CRect rcItem;
	HDITEM HDItem;
	TCHAR szBuffer[64];
	for (INT i=0;i<GetItemCount();i++)
	{
		//�������
		HDItem.mask=HDI_TEXT;
		HDItem.pszText=szBuffer;
		HDItem.cchTextMax=CountArray(szBuffer);

		//��ȡ��Ϣ
		GetItem(i,&HDItem);
		GetItemRect(i,&rcItem);

		//�滭����
		INT nLeftWidth=ImageHandleL->GetWidth();
		INT nRightWidth=ImageHandleR->GetWidth();
		ImageHandleL->StretchBlt(BufferDC,rcItem.left,rcItem.top,nLeftWidth,rcItem.Height());
		ImageHandleM->StretchBlt(BufferDC,rcItem.left+nLeftWidth,rcItem.top,rcItem.Width()-nLeftWidth-nRightWidth,rcItem.Height());
		ImageHandleR->StretchBlt(BufferDC,rcItem.right-nRightWidth,rcItem.top,nRightWidth,rcItem.Height());

		//�滭����
		rcItem.DeflateRect(3,1,3,1);
		const UINT uFormat=DT_WORDBREAK|DT_EDITCONTROL|DT_END_ELLIPSIS|DT_SINGLELINE|DT_VCENTER|DT_CENTER;
		BufferDC.DrawText(szBuffer,lstrlen(szBuffer),&rcItem,uFormat);
	}

	//�滭����
	dc.BitBlt(0,0,rcRect.Width(),rcRect.Height(),&BufferDC,0,0,SRCCOPY);

	//������Դ
	BufferDC.DeleteDC();
	BufferBmp.DeleteObject();
	
	return;
}

//��������
BOOL CSkinHeaderCtrl::OnEraseBkgnd(CDC * pDC) 
{
	//ˢ�½���
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//���ñ�־
VOID CSkinHeaderCtrl::SetSortArrow(INT nSortColumn, enSortType SortType)
{
	//���ñ���
	m_SortType=SortType;
	m_nColumn=nSortColumn;

	//�ػ�����
	Invalidate(FALSE);

	return;
}

//��ȡ��־
VOID CSkinHeaderCtrl::GetSortArrow(INT & nSortColumn, enSortType & SortType)
{
	//���ñ���
	SortType=m_SortType;
	nSortColumn=m_nColumn;

	return;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinListCtrl::CSkinListCtrl()
{
}

//��������
CSkinListCtrl::~CSkinListCtrl()
{
}

//�ؼ���
VOID CSkinListCtrl::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//���ù���
	m_SkinScrollBar.InitScroolBar(this);

	//��������
	SetExtendedStyle(LIST_STYTE);
	m_SkinHeadCtrl.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());

	//������ɫ
	SetBkColor(m_SkinAttribute.m_crListBK);
	SetTextColor(m_SkinAttribute.m_crListTX);
	SetTextBkColor(m_SkinAttribute.m_crListBK);

	return;
}

//������Ϣ
INT CSkinListCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//���ù���
	m_SkinScrollBar.InitScroolBar(this);
	
	//��������
	SetExtendedStyle(LIST_STYTE);
	m_SkinHeadCtrl.SubclassWindow(GetHeaderCtrl()->GetSafeHwnd());

	return 0;
}

//�滭����
BOOL CSkinListCtrl::OnEraseBkgnd(CDC * pDC)
{
	if (m_SkinHeadCtrl.m_hWnd!=NULL)
	{
		//��ȡλ��
		CRect rcRect;
		CRect rcHeader;
		GetClientRect(&rcRect);
		m_SkinHeadCtrl.GetWindowRect(&rcHeader);

		//�滭����
		rcRect.top=rcHeader.Height();
		pDC->FillSolidRect(&rcRect,GetBkColor());

		return TRUE;
	}

	return __super::OnEraseBkgnd(pDC);
}

//////////////////////////////////////////////////////////////////////////
