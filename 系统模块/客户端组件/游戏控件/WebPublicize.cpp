#include "StdAfx.h"
#include "WebPublicize.h"

//////////////////////////////////////////////////////////////////////////

//常量定义
#define IDC_WEB_BROWSER				100									//窗口标识

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CWebPublicize, CWnd)
	ON_WM_SIZE()
	ON_WM_PAINT()
END_MESSAGE_MAP()

BEGIN_EVENTSINK_MAP(CWebPublicize, CWnd)
	ON_EVENT(CWebPublicize, IDC_WEB_BROWSER, 259, OnDocumentCompleteWeb, VTS_DISPATCH VTS_PVARIANT)
	ON_EVENT(CWebPublicize, IDC_WEB_BROWSER, 271, OnNavigateErrorWeb, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
	ON_EVENT(CWebPublicize, IDC_WEB_BROWSER, 250, OnBeforeNavigate2Web, VTS_DISPATCH VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PBOOL)
END_EVENTSINK_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CWebPublicize::CWebPublicize()
{
	//设置变量
	m_cbStatus=BROWSER_STATUS_LOAD;

	return;
}

//析构函数
CWebPublicize::~CWebPublicize()
{
}

//浏览函数
VOID CWebPublicize::Navigate(LPCTSTR pszURL)
{
	//创建窗口
	if (m_WebBrowser.m_hWnd==NULL)
	{
		CRect rcCreate(0,0,0,0);
		if( m_WebBrowser.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_WEB_BROWSER) )
			m_WebBrowser.ModifyStyle(WS_BORDER|WS_CAPTION,0);
	}

	//浏览页面
	m_WebBrowser.Navigate(pszURL,NULL,NULL,NULL,NULL);

	return;
}

//浏览函数
VOID CWebPublicize::Navigate(LPCTSTR pszURL, VARIANT * Flags, VARIANT * TargetFrameName, VARIANT * PostData, VARIANT * Headers)
{
	//创建窗口
	if (m_WebBrowser.m_hWnd==NULL)
	{
		CRect rcCreate(0,0,0,0);
		if( m_WebBrowser.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_WEB_BROWSER) )
		{
			m_WebBrowser.ModifyStyle(WS_BORDER,0,0);
			m_WebBrowser.ModifyStyleEx(WS_EX_CLIENTEDGE,0,0);
		}
	}

	//浏览页面
	m_WebBrowser.Navigate(pszURL,Flags,TargetFrameName,PostData,Headers);

	return;
}

//位置函数
VOID CWebPublicize::GetWebBrowserRect(CRect & rcWebBrowser)
{
	//获取位置
	GetClientRect(&rcWebBrowser);

	return;
}

//绘画界面
VOID CWebPublicize::DrawPublicizeView(CDC * pDC, INT nWidth, INT nHeight, BYTE cbStatus)
{
	//绘画背景
	pDC->FillSolidRect(0,0,nWidth,nHeight,RGB(255,255,255));

	return;
}

//重画消息
VOID CWebPublicize::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//绘画缓冲
	CDC * pBufferDC=CDC::FromHandle(ImageBuffer.GetDC());
	DrawPublicizeView(pBufferDC,rcClient.Width(),rcClient.Height(),m_cbStatus);

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);

	//释放资源
	ImageBuffer.ReleaseDC();

	return;
}

//位置消息
VOID CWebPublicize::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//移动窗口
	if (m_WebBrowser.m_hWnd!=NULL)
	{
		if (m_cbStatus==BROWSER_STATUS_FINISH)
		{
			//获取位置
			CRect rcWebBrowser;
			GetWebBrowserRect(rcWebBrowser);

			//移动位置
			m_WebBrowser.SetWindowPos(NULL,0,0,rcWebBrowser.Width(),rcWebBrowser.Height(),SWP_NOZORDER);
		}
		else
		{
			//移动位置
			m_WebBrowser.SetWindowPos(NULL,0,0,0,0,SWP_NOZORDER);
		}
	}

	return;
}

//导航完成
VOID CWebPublicize::OnDocumentCompleteWeb(LPDISPATCH pDisp, VARIANT FAR * URL)
{
	//设置变量
	if (m_cbStatus!=BROWSER_STATUS_ERROR)
	{
		//设置变量
		m_cbStatus=BROWSER_STATUS_FINISH;

		//移动窗口
		if (m_WebBrowser.m_hWnd!=NULL)
		{
			//获取位置
			CRect rcWebBrowser;
			GetWebBrowserRect(rcWebBrowser);

			m_WebBrowser.SetWindowPos(NULL,0,0,rcWebBrowser.Width(),rcWebBrowser.Height(),SWP_NOZORDER);
		}
	}

	return;
}

//导航错误
VOID CWebPublicize::OnNavigateErrorWeb(LPDISPATCH pDisp, VARIANT FAR * URL, VARIANT FAR * Frame, VARIANT FAR * StatusCode, BOOL FAR * Cancel)
{
	//设置变量
	m_cbStatus=BROWSER_STATUS_ERROR;

	//移动窗口
	if (m_WebBrowser.m_hWnd!=NULL) m_WebBrowser.SetWindowPos(NULL,0,0,0,0,SWP_NOZORDER);

	return;
}

//导航开始
VOID CWebPublicize::OnBeforeNavigate2Web(LPDISPATCH pDisp, VARIANT FAR * URL, VARIANT FAR * Flags, VARIANT FAR * TargetFrameName, VARIANT FAR * PostData, VARIANT FAR * Headers, BOOL FAR * Cancel)
{
	//设置变量
	m_cbStatus=BROWSER_STATUS_LOAD;

	//移动窗口
	if (m_WebBrowser.m_hWnd!=NULL) m_WebBrowser.SetWindowPos(NULL,0,0,0,0,SWP_NOZORDER);

	return;
}

//////////////////////////////////////////////////////////////////////////
