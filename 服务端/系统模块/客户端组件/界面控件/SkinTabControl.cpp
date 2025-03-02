#include "StdAfx.h"
#include "SkinDialog.h"
#include "SkinTabControl.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
CSkinTabControlAttribute			CSkinTabControl::m_SkinAttribute;	//��������

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinTabControl, CTabCtrl)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BEGIN_MESSAGE_MAP(CSkinTabControlItem, CDialog)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinTabControlAttribute::CSkinTabControlAttribute()
{
	//���ñ���
	m_crNormalTX=RGB(0,0,0);
	m_crSelectTX=RGB(0,0,0);
	m_crControlBack=RGB(0,0,0);
	m_crControlBorad=RGB(0,0,0);

	return;
}

//��������
CSkinTabControlAttribute::~CSkinTabControlAttribute() 
{
}

//������Դ
bool CSkinTabControlAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
{
	//��ȡ��Ϣ
	tagSkinTabControlResource SkinTabControlResource;
	if (pISkinResource!=NULL) pISkinResource->GetTabControlResource(SkinTabControlResource);

	//���ñ���
	m_crNormalTX=SkinTabControlResource.crNormalTX;
	m_crSelectTX=SkinTabControlResource.crSelectTX;
	m_crControlBack=SkinTabControlResource.crControlBack;
	m_crControlBorad=SkinTabControlResource.crControlBorad;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinTabControl::CSkinTabControl()
{
}

//��������
CSkinTabControl::~CSkinTabControl()
{
}

//�󶨺���
VOID CSkinTabControl::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//���ñ���
	SetClassLongPtr(m_hWnd,GCLP_HBRBACKGROUND,NULL);
	ModifyStyle(0,WS_CLIPCHILDREN|WS_CLIPSIBLINGS);

	//��������
	CFont Font;
	if (Font.CreatePointFont(110,TEXT("����"))==TRUE) SetFont(&Font);

	return;
}

//�ػ���Ϣ
VOID CSkinTabControl::OnPaint()
{
	//��������
	CPaintDC dc(this);

	//��ȡλ��
	CRect rcClip,rcClient;
	dc.GetClipBox(&rcClip);
	GetClientRect(&rcClient);

	//�滭����
	dc.FillSolidRect(&rcClip,m_SkinAttribute.m_crControlBack);

	//���� DC
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());
	
	//����λ��
	CRect rcItem;
	GetItemRect(0,&rcItem);

	//�滭�߿�
	rcClient.top=rcItem.bottom-1;
	dc.Draw3dRect(&rcClient,m_SkinAttribute.m_crControlBorad,m_SkinAttribute.m_crControlBorad);

	//��������
	CPen LinePen(PS_SOLID,1,m_SkinAttribute.m_crControlBorad);
	CPen * pOldPen=dc.SelectObject(&LinePen);

	//�滭��ť
	INT nCursel=GetCurSel();
	for (INT i=0;i<GetItemCount();i++)
	{
		//��������
		TCITEM ItemInfo;
		TCHAR szBuffer[128];
		ZeroMemory(szBuffer,sizeof(szBuffer));
		ZeroMemory(&ItemInfo,sizeof(ItemInfo));

		//���ñ���
		ItemInfo.mask=TCIF_TEXT;
		ItemInfo.pszText=szBuffer;
		ItemInfo.cchTextMax=CountArray(szBuffer);

		//��ȡ��Ϣ
		GetItem(i,&ItemInfo);
		GetItemRect(i,&rcItem);

		//�滭����
		dc.FillSolidRect(&rcItem,m_SkinAttribute.m_crControlBack);

		//�滭�߿�
		dc.MoveTo(rcItem.left,rcItem.bottom-1);
		dc.LineTo(rcItem.left,rcItem.top+2);
		dc.LineTo(rcItem.left+2,rcItem.top);
		dc.LineTo(rcItem.right-2,rcItem.top);
		dc.LineTo(rcItem.right,rcItem.top+2);
		dc.LineTo(rcItem.right,rcItem.bottom-1);
		
		if (nCursel!=i)
		{
			//�滭�ָ�
			dc.MoveTo(rcItem.left,rcItem.bottom-1);
			dc.LineTo(rcItem.right,rcItem.bottom-1);

			//�滭����
			rcItem.top+=2;
			dc.SetTextColor(m_SkinAttribute.m_crNormalTX);
			DrawText(dc,szBuffer,lstrlen(szBuffer),&rcItem,DT_VCENTER|DT_SINGLELINE|DT_CENTER);
		}
		else 
		{
			//�滭����
			rcItem.top+=3;
			dc.SetTextColor(m_SkinAttribute.m_crSelectTX);
			DrawText(dc,szBuffer,lstrlen(szBuffer),&rcItem,DT_VCENTER|DT_SINGLELINE|DT_CENTER);
		}
	}

	//�������
	dc.SelectObject(pOldPen);
	LinePen.DeleteObject();

	return;
}

//�滭����
BOOL CSkinTabControl::OnEraseBkgnd(CDC * pDC)
{
	//ˢ�½���
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

//���캯��
CSkinTabControlItem::CSkinTabControlItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//���ñ���
	m_nIDTemplate=nIDTemplate;
	m_ControlSize.SetSize(0,0);

	return;
}

//��������
CSkinTabControlItem::~CSkinTabControlItem()
{
}

//��Ϣ����
BOOL CSkinTabControlItem::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE)) 
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//ȡ����Ϣ
VOID CSkinTabControlItem::OnCancel()
{
	//������Ϣ
	CWnd * pParentWnd=GetParent();
	pParentWnd=pParentWnd->GetParent();
	pParentWnd->SendMessage(WM_COMMAND,IDCANCEL,(LPARAM)(pParentWnd->GetSafeHwnd()));

	return;
}

//�����ؼ�
bool CSkinTabControlItem::CreateControlItem(CWnd * pParentWnd)
{
	//�����ؼ�
	if (m_hWnd==NULL)
	{
		//��������
		Create(m_nIDTemplate,pParentWnd);

		//����λ��
		CRect rcWindow;
		GetWindowRect(&rcWindow);
		m_ControlSize.SetSize(rcWindow.Width(),rcWindow.Height());
	}

	return true;
}

//�滭��Ϣ
VOID CSkinTabControlItem::OnPaint()
{
	//��������
	CPaintDC dc(this);

	//�滭����
	CRect rcClient;
	GetClientRect(&rcClient);
	dc.FillSolidRect(0,0,rcClient.Width(),rcClient.Height(),CSkinTabControl::m_SkinAttribute.m_crControlBack);

	//�滭�߿�

	return;
}

//�滭����
BOOL CSkinTabControlItem::OnEraseBkgnd(CDC * pDC)
{
	//���´���
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//�ؼ���ɫ
HBRUSH CSkinTabControlItem::OnCtlColor(CDC * pDC, CWnd * pWnd, UINT nCtlColor)
{
	switch (nCtlColor)
	{
	case CTLCOLOR_DLG:
	case CTLCOLOR_STATIC:
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(CSkinDialog::m_SkinAttribute.m_crBackGround);
			pDC->SetTextColor(CSkinDialog::m_SkinAttribute.m_crControlText);
			return CSkinDialog::m_SkinAttribute.m_brBackGround;
		}
	}

	return __super::OnCtlColor(pDC,pWnd,nCtlColor);
}

//////////////////////////////////////////////////////////////////////////
