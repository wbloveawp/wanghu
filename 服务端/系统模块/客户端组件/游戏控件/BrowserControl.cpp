#include "Stdafx.h"
#include "BrowserControl.h"
#include "exdispid.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CHtmlBrowser, CHtmlView)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CHtmlBrowser, CHtmlView)
	ON_EVENT(CHtmlBrowser, AFX_IDW_PANE_FIRST, DISPID_WINDOWCLOSING, OnWindowClosing,VTS_BOOL VTS_PBOOL)   
END_EVENTSINK_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CHtmlBrowser::CHtmlBrowser()
{
}

//析构函数
CHtmlBrowser::~CHtmlBrowser()
{
}

//绑定函数
VOID CHtmlBrowser::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//设置背景
	SetClassLongPtr(m_hWnd,GCLP_HBRBACKGROUND,NULL);

	return;
}

//消耗处理
VOID CHtmlBrowser::PostNcDestroy()
{
	return;
}

//建立消息
INT CHtmlBrowser::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置背景
	SetClassLongPtr(m_hWnd,GCLP_HBRBACKGROUND,NULL);

	return 0;
}

//绘画背景
BOOL CHtmlBrowser::OnEraseBkgnd(CDC * pDC)
{
	return TRUE;
}

//创建函数
BOOL CHtmlBrowser::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// create the view window itself
	m_pCreateContext = pContext;
	if (!CView::Create(lpszClassName, lpszWindowName,
		dwStyle, rect, pParentWnd,  nID, pContext))
	{
		return FALSE;
	}

	// assure that control containment is on
	//AfxEnableControlContainer();

	RECT rectClient;
	GetClientRect(&rectClient);

	// create the control window
	// AFX_IDW_PANE_FIRST is a safe but arbitrary ID
	if (!m_wndBrowser.CreateControl(CLSID_WebBrowser, lpszWindowName,
		WS_VISIBLE | WS_CHILD, rectClient, this, AFX_IDW_PANE_FIRST))
	{
		DestroyWindow();
		return FALSE;
	}

	// cache the dispinterface
	LPUNKNOWN lpUnk = m_wndBrowser.GetControlUnknown();
	HRESULT hr = lpUnk->QueryInterface(IID_IWebBrowser2, (void**) &m_pBrowserApp);
	if (!SUCCEEDED(hr))
	{
		m_pBrowserApp = NULL;
		m_wndBrowser.DestroyWindow();
		DestroyWindow();
		return FALSE;
	}

	return TRUE;
}

//IE关闭事件
void CHtmlBrowser::OnWindowClosing(BOOL IsChildWindow, BOOL *bCancel)   
{   
	//禁止关闭
	if(!IsChildWindow)
	{
		if(bCancel)
			*bCancel=TRUE;
	}

	return;
}

//////////////////////////////////////////////////////////////////////////
