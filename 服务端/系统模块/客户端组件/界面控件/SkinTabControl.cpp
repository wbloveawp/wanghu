#include "StdAfx.h"
#include "SkinDialog.h"
#include "SkinTabControl.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
CSkinTabControlAttribute			CSkinTabControl::m_SkinAttribute;	//子项属性

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

//构造函数
CSkinTabControlAttribute::CSkinTabControlAttribute()
{
	//设置变量
	m_crNormalTX=RGB(0,0,0);
	m_crSelectTX=RGB(0,0,0);
	m_crControlBack=RGB(0,0,0);
	m_crControlBorad=RGB(0,0,0);

	return;
}

//析构函数
CSkinTabControlAttribute::~CSkinTabControlAttribute() 
{
}

//更新资源
bool CSkinTabControlAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
{
	//获取信息
	tagSkinTabControlResource SkinTabControlResource;
	if (pISkinResource!=NULL) pISkinResource->GetTabControlResource(SkinTabControlResource);

	//设置变量
	m_crNormalTX=SkinTabControlResource.crNormalTX;
	m_crSelectTX=SkinTabControlResource.crSelectTX;
	m_crControlBack=SkinTabControlResource.crControlBack;
	m_crControlBorad=SkinTabControlResource.crControlBorad;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinTabControl::CSkinTabControl()
{
}

//析够函数
CSkinTabControl::~CSkinTabControl()
{
}

//绑定函数
VOID CSkinTabControl::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//设置背景
	SetClassLongPtr(m_hWnd,GCLP_HBRBACKGROUND,NULL);
	ModifyStyle(0,WS_CLIPCHILDREN|WS_CLIPSIBLINGS);

	//创建字体
	CFont Font;
	if (Font.CreatePointFont(110,TEXT("宋体"))==TRUE) SetFont(&Font);

	return;
}

//重画消息
VOID CSkinTabControl::OnPaint()
{
	//变量定义
	CPaintDC dc(this);

	//获取位置
	CRect rcClip,rcClient;
	dc.GetClipBox(&rcClip);
	GetClientRect(&rcClient);

	//绘画背景
	dc.FillSolidRect(&rcClip,m_SkinAttribute.m_crControlBack);

	//设置 DC
	dc.SetBkMode(TRANSPARENT);
	dc.SelectObject(CSkinResourceManager::GetDefaultFont());
	
	//子项位置
	CRect rcItem;
	GetItemRect(0,&rcItem);

	//绘画边框
	rcClient.top=rcItem.bottom-1;
	dc.Draw3dRect(&rcClient,m_SkinAttribute.m_crControlBorad,m_SkinAttribute.m_crControlBorad);

	//创建对象
	CPen LinePen(PS_SOLID,1,m_SkinAttribute.m_crControlBorad);
	CPen * pOldPen=dc.SelectObject(&LinePen);

	//绘画按钮
	INT nCursel=GetCurSel();
	for (INT i=0;i<GetItemCount();i++)
	{
		//变量定义
		TCITEM ItemInfo;
		TCHAR szBuffer[128];
		ZeroMemory(szBuffer,sizeof(szBuffer));
		ZeroMemory(&ItemInfo,sizeof(ItemInfo));

		//设置变量
		ItemInfo.mask=TCIF_TEXT;
		ItemInfo.pszText=szBuffer;
		ItemInfo.cchTextMax=CountArray(szBuffer);

		//获取信息
		GetItem(i,&ItemInfo);
		GetItemRect(i,&rcItem);

		//绘画背景
		dc.FillSolidRect(&rcItem,m_SkinAttribute.m_crControlBack);

		//绘画边框
		dc.MoveTo(rcItem.left,rcItem.bottom-1);
		dc.LineTo(rcItem.left,rcItem.top+2);
		dc.LineTo(rcItem.left+2,rcItem.top);
		dc.LineTo(rcItem.right-2,rcItem.top);
		dc.LineTo(rcItem.right,rcItem.top+2);
		dc.LineTo(rcItem.right,rcItem.bottom-1);
		
		if (nCursel!=i)
		{
			//绘画分割
			dc.MoveTo(rcItem.left,rcItem.bottom-1);
			dc.LineTo(rcItem.right,rcItem.bottom-1);

			//绘画文字
			rcItem.top+=2;
			dc.SetTextColor(m_SkinAttribute.m_crNormalTX);
			DrawText(dc,szBuffer,lstrlen(szBuffer),&rcItem,DT_VCENTER|DT_SINGLELINE|DT_CENTER);
		}
		else 
		{
			//绘画文字
			rcItem.top+=3;
			dc.SetTextColor(m_SkinAttribute.m_crSelectTX);
			DrawText(dc,szBuffer,lstrlen(szBuffer),&rcItem,DT_VCENTER|DT_SINGLELINE|DT_CENTER);
		}
	}

	//清理对象
	dc.SelectObject(pOldPen);
	LinePen.DeleteObject();

	return;
}

//绘画背景
BOOL CSkinTabControl::OnEraseBkgnd(CDC * pDC)
{
	//刷新界面
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinTabControlItem::CSkinTabControlItem(UINT nIDTemplate) : CDialog(nIDTemplate)
{
	//设置变量
	m_nIDTemplate=nIDTemplate;
	m_ControlSize.SetSize(0,0);

	return;
}

//析构函数
CSkinTabControlItem::~CSkinTabControlItem()
{
}

//消息解释
BOOL CSkinTabControlItem::PreTranslateMessage(MSG * pMsg)
{
	//按键过虑
	if ((pMsg->message==WM_KEYDOWN)&&(pMsg->wParam==VK_ESCAPE)) 
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//取消消息
VOID CSkinTabControlItem::OnCancel()
{
	//发送消息
	CWnd * pParentWnd=GetParent();
	pParentWnd=pParentWnd->GetParent();
	pParentWnd->SendMessage(WM_COMMAND,IDCANCEL,(LPARAM)(pParentWnd->GetSafeHwnd()));

	return;
}

//创建控件
bool CSkinTabControlItem::CreateControlItem(CWnd * pParentWnd)
{
	//创建控件
	if (m_hWnd==NULL)
	{
		//创建窗口
		Create(m_nIDTemplate,pParentWnd);

		//设置位置
		CRect rcWindow;
		GetWindowRect(&rcWindow);
		m_ControlSize.SetSize(rcWindow.Width(),rcWindow.Height());
	}

	return true;
}

//绘画消息
VOID CSkinTabControlItem::OnPaint()
{
	//变量定义
	CPaintDC dc(this);

	//绘画界面
	CRect rcClient;
	GetClientRect(&rcClient);
	dc.FillSolidRect(0,0,rcClient.Width(),rcClient.Height(),CSkinTabControl::m_SkinAttribute.m_crControlBack);

	//绘画边框

	return;
}

//绘画背景
BOOL CSkinTabControlItem::OnEraseBkgnd(CDC * pDC)
{
	//更新窗口
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//控件颜色
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
