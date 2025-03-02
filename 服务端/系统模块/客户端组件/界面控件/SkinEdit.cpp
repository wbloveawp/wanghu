#include "Stdafx.h"
#include "SkinEdit.h"
#include "SkinDialog.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
CSkinEditAttribute					CSkinEdit::m_SkinAttribute;				//界面属性

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinEdit, CEdit)
    ON_WM_NCPAINT()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinEditEx, CSkinEdit)
    ON_WM_NCPAINT()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinEditAttribute::CSkinEditAttribute()
{
	//设置变量
	m_crFocusTX=RGB(0,0,0);
	m_crFocusBK=RGB(0,0,0);
	m_crNoFocusTX=RGB(0,0,0);
	m_crNoFocusBK=RGB(0,0,0);
	m_crDisFocusTX=RGB(0,0,0);
	m_crDisFocusBK=RGB(0,0,0);

	return;
}

//析构函数
CSkinEditAttribute::~CSkinEditAttribute() 
{
}

//更新资源
bool CSkinEditAttribute::UpdateSkinResource(ISkinResource * pISkinResource)
{
	//获取信息
	tagSkinEditResource SkinEditResource;
	if (pISkinResource!=NULL) pISkinResource->GetEditResource(SkinEditResource);

	//设置变量
	m_crFocusTX=SkinEditResource.crFocusTX;
	m_crFocusBK=SkinEditResource.crFocusBK;
	m_crNoFocusTX=SkinEditResource.crNoFocusTX;
	m_crNoFocusBK=SkinEditResource.crNoFocusBK;
	m_crDisFocusTX=SkinEditResource.crDisFocusTX;
	m_crDisFocusBK=SkinEditResource.crDisFocusBK;
	m_crBoradNormal=SkinEditResource.crBoradNormal;

	//创建画刷
	m_brFocus.DeleteObject();
	m_brNoFocus.DeleteObject();
	m_brDisbale.DeleteObject();
	m_brFocus.CreateSolidBrush(m_crFocusBK);
	m_brNoFocus.CreateSolidBrush(m_crNoFocusBK);
	m_brDisbale.CreateSolidBrush(m_crDisFocusBK);

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinEdit::CSkinEdit()
{
	//设置变量
	m_bFocus=false;

	return;
}

//析构函数
CSkinEdit::~CSkinEdit()
{
}

//绘画控件
HBRUSH CSkinEdit::CtlColor(CDC * pDC, UINT nCtlColor)
{
	//启用状态
	if (IsWindowEnabled())
	{
		if (m_bFocus==true) 
		{
			pDC->SetBkColor(m_SkinAttribute.m_crFocusBK);
			pDC->SetTextColor(m_SkinAttribute.m_crFocusTX);
			return m_SkinAttribute.m_brFocus;
		}
		else
		{
			pDC->SetBkColor(m_SkinAttribute.m_crNoFocusBK);
			pDC->SetTextColor(m_SkinAttribute.m_crNoFocusTX);
			return m_SkinAttribute.m_brNoFocus;
		}
	}

	//禁用状态
	pDC->SetBkColor(m_SkinAttribute.m_crDisFocusBK);
	pDC->SetTextColor(m_SkinAttribute.m_crDisFocusBK);

	return m_SkinAttribute.m_brDisbale;
}

//重画消息
VOID CSkinEdit::OnNcPaint()
{
	__super::OnNcPaint();

	//获取位置
	CRect rcWindow;
	GetWindowRect(&rcWindow);
	ScreenToClient(&rcWindow);

	//获取属性
	DWORD dwStyle=GetStyle();

	//绘画边框
	if ((dwStyle&WS_BORDER|WS_DLGFRAME)!=0)
	{
		//状态变量
		bool bDisable=(IsWindowEnabled()==FALSE);

		//绘画边框
		if (bDisable==false)
		{
			//绘画边框
			CClientDC ClientDC(this);
			ClientDC.Draw3dRect(&rcWindow,m_SkinAttribute.m_crBoradNormal,m_SkinAttribute.m_crBoradNormal);

			//绘画内框
			if (m_bFocus==true)
			{
				//焦点颜色
				COLORREF crBackGroud=m_SkinAttribute.m_crFocusBK;
				ClientDC.Draw3dRect(rcWindow.left+1,rcWindow.top+1,rcWindow.Width()-2,rcWindow.Height()-2,crBackGroud,crBackGroud);
			}
			else
			{
				//普通颜色
				COLORREF crBackGroud=m_SkinAttribute.m_crNoFocusBK;
				ClientDC.Draw3dRect(rcWindow.left+1,rcWindow.top+1,rcWindow.Width()-2,rcWindow.Height()-2,crBackGroud,crBackGroud);
			}
		}
		else
		{
			//绘画边框
			CClientDC ClientDC(this);
			ClientDC.Draw3dRect(&rcWindow,m_SkinAttribute.m_crDisFocusBK,m_SkinAttribute.m_crDisFocusBK);

			//绘画内框
			COLORREF crBackGroud=CSkinDialog::m_SkinAttribute.m_crBackGround;
			ClientDC.Draw3dRect(rcWindow.left+1,rcWindow.top+1,rcWindow.Width()-2,rcWindow.Height()-2,crBackGroud,crBackGroud);
		}
	}

	return;
}

//得到焦点
VOID CSkinEdit::OnSetFocus(CWnd * pOldWnd)
{
	__super::OnSetFocus(pOldWnd);

	//设置变量
	m_bFocus=true;

	//重画界面
	Invalidate(TRUE);

	return;
}

//失去焦点
VOID CSkinEdit::OnKillFocus(CWnd * pNewWnd)
{
	__super::OnKillFocus(pNewWnd);

	//设置变量
	m_bFocus=false;

	//重画界面
	Invalidate(TRUE);

	return;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinEditEx::CSkinEditEx()
{
	//设置变量
	m_crFocusTX=m_SkinAttribute.m_crFocusTX;
	m_crFocusBK=m_SkinAttribute.m_crFocusBK;
	m_crNoFocusTX=m_SkinAttribute.m_crNoFocusTX;
	m_crNoFocusBK=m_SkinAttribute.m_crNoFocusBK;
	m_crDisFocusTX=m_SkinAttribute.m_crDisFocusTX;
	m_crDisFocusBK=m_SkinAttribute.m_crDisFocusBK;

	//创建画刷
	m_brFocus.DeleteObject();
	m_brNoFocus.DeleteObject();
	m_brDisbale.DeleteObject();
	m_brFocus.CreateSolidBrush(m_crFocusBK);
	m_brNoFocus.CreateSolidBrush(m_crNoFocusBK);
	m_brDisbale.CreateSolidBrush(m_crDisFocusBK);

	return;
}

//析构函数
CSkinEditEx::~CSkinEditEx()
{
}

//设置颜色
VOID CSkinEditEx::SetFocusColor(COLORREF crFocusTX, COLORREF crFocusBK)
{
	//设置颜色
	m_crFocusTX=crFocusTX;
	m_crFocusBK=crFocusBK;

	//创建画刷
	m_brFocus.DeleteObject();
	m_brFocus.CreateSolidBrush(m_crFocusBK);

	//绘画界面
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//设置颜色
VOID CSkinEditEx::SetNoFocusColor(COLORREF crNoFocusTX, COLORREF crNoFocusBK)
{
	//设置颜色
	m_crNoFocusTX=crNoFocusTX;
	m_crNoFocusBK=crNoFocusBK;

	//创建画刷
	m_brNoFocus.DeleteObject();
	m_brNoFocus.CreateSolidBrush(m_crNoFocusBK);

	//绘画界面
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//设置颜色
VOID CSkinEditEx::SetDisFocusColor(COLORREF crDisFocusTX, COLORREF crDisFocusBK)
{
	//设置颜色
	m_crDisFocusTX=crDisFocusTX;
	m_crDisFocusBK=crDisFocusBK;

	//创建画刷
	m_brDisbale.DeleteObject();
	m_brDisbale.CreateSolidBrush(m_crDisFocusBK);

	//绘画界面
	if (m_hWnd!=NULL) Invalidate(FALSE);

	return;
}

//重画消息
VOID CSkinEditEx::OnNcPaint()
{
	//绘画控件
	CEdit::OnNcPaint();

	return;
}

//绘画控件
HBRUSH CSkinEditEx::CtlColor(CDC * pDC, UINT nCtlColor)
{
	//启用状态
	if (IsWindowEnabled())
	{
		if (m_bFocus==true) 
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(m_crFocusBK);
			pDC->SetTextColor(m_crFocusTX);
			return m_brFocus;
		}
		else
		{
			pDC->SetBkMode(TRANSPARENT);
			pDC->SetBkColor(m_crNoFocusBK);
			pDC->SetTextColor(m_crNoFocusTX);
			return m_brNoFocus;
		}
	}

	//禁用状态
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(m_crDisFocusBK);
	pDC->SetTextColor(m_crDisFocusBK);
	return m_brDisbale;
}

//////////////////////////////////////////////////////////////////////////
