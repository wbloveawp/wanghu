#include "StdAfx.h"
#include "WebPublicize.h"

//////////////////////////////////////////////////////////////////////////

//��������
#define IDC_WEB_BROWSER				100									//���ڱ�ʶ

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

//���캯��
CWebPublicize::CWebPublicize()
{
	//���ñ���
	m_cbStatus=BROWSER_STATUS_LOAD;

	return;
}

//��������
CWebPublicize::~CWebPublicize()
{
}

//�������
VOID CWebPublicize::Navigate(LPCTSTR pszURL)
{
	//��������
	if (m_WebBrowser.m_hWnd==NULL)
	{
		CRect rcCreate(0,0,0,0);
		if( m_WebBrowser.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_WEB_BROWSER) )
			m_WebBrowser.ModifyStyle(WS_BORDER|WS_CAPTION,0);
	}

	//���ҳ��
	m_WebBrowser.Navigate(pszURL,NULL,NULL,NULL,NULL);

	return;
}

//�������
VOID CWebPublicize::Navigate(LPCTSTR pszURL, VARIANT * Flags, VARIANT * TargetFrameName, VARIANT * PostData, VARIANT * Headers)
{
	//��������
	if (m_WebBrowser.m_hWnd==NULL)
	{
		CRect rcCreate(0,0,0,0);
		if( m_WebBrowser.Create(NULL,WS_CHILD|WS_VISIBLE,rcCreate,this,IDC_WEB_BROWSER) )
		{
			m_WebBrowser.ModifyStyle(WS_BORDER,0,0);
			m_WebBrowser.ModifyStyleEx(WS_EX_CLIENTEDGE,0,0);
		}
	}

	//���ҳ��
	m_WebBrowser.Navigate(pszURL,Flags,TargetFrameName,PostData,Headers);

	return;
}

//λ�ú���
VOID CWebPublicize::GetWebBrowserRect(CRect & rcWebBrowser)
{
	//��ȡλ��
	GetClientRect(&rcWebBrowser);

	return;
}

//�滭����
VOID CWebPublicize::DrawPublicizeView(CDC * pDC, INT nWidth, INT nHeight, BYTE cbStatus)
{
	//�滭����
	pDC->FillSolidRect(0,0,nWidth,nHeight,RGB(255,255,255));

	return;
}

//�ػ���Ϣ
VOID CWebPublicize::OnPaint()
{
	CPaintDC dc(this);

	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	CImage ImageBuffer;
	ImageBuffer.Create(rcClient.Width(),rcClient.Height(),32);

	//�滭����
	CDC * pBufferDC=CDC::FromHandle(ImageBuffer.GetDC());
	DrawPublicizeView(pBufferDC,rcClient.Width(),rcClient.Height(),m_cbStatus);

	//�滭����
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),pBufferDC,0,0,SRCCOPY);

	//�ͷ���Դ
	ImageBuffer.ReleaseDC();

	return;
}

//λ����Ϣ
VOID CWebPublicize::OnSize(UINT nType, INT cx, INT cy)
{
	__super::OnSize(nType, cx, cy);

	//�ƶ�����
	if (m_WebBrowser.m_hWnd!=NULL)
	{
		if (m_cbStatus==BROWSER_STATUS_FINISH)
		{
			//��ȡλ��
			CRect rcWebBrowser;
			GetWebBrowserRect(rcWebBrowser);

			//�ƶ�λ��
			m_WebBrowser.SetWindowPos(NULL,0,0,rcWebBrowser.Width(),rcWebBrowser.Height(),SWP_NOZORDER);
		}
		else
		{
			//�ƶ�λ��
			m_WebBrowser.SetWindowPos(NULL,0,0,0,0,SWP_NOZORDER);
		}
	}

	return;
}

//�������
VOID CWebPublicize::OnDocumentCompleteWeb(LPDISPATCH pDisp, VARIANT FAR * URL)
{
	//���ñ���
	if (m_cbStatus!=BROWSER_STATUS_ERROR)
	{
		//���ñ���
		m_cbStatus=BROWSER_STATUS_FINISH;

		//�ƶ�����
		if (m_WebBrowser.m_hWnd!=NULL)
		{
			//��ȡλ��
			CRect rcWebBrowser;
			GetWebBrowserRect(rcWebBrowser);

			m_WebBrowser.SetWindowPos(NULL,0,0,rcWebBrowser.Width(),rcWebBrowser.Height(),SWP_NOZORDER);
		}
	}

	return;
}

//��������
VOID CWebPublicize::OnNavigateErrorWeb(LPDISPATCH pDisp, VARIANT FAR * URL, VARIANT FAR * Frame, VARIANT FAR * StatusCode, BOOL FAR * Cancel)
{
	//���ñ���
	m_cbStatus=BROWSER_STATUS_ERROR;

	//�ƶ�����
	if (m_WebBrowser.m_hWnd!=NULL) m_WebBrowser.SetWindowPos(NULL,0,0,0,0,SWP_NOZORDER);

	return;
}

//������ʼ
VOID CWebPublicize::OnBeforeNavigate2Web(LPDISPATCH pDisp, VARIANT FAR * URL, VARIANT FAR * Flags, VARIANT FAR * TargetFrameName, VARIANT FAR * PostData, VARIANT FAR * Headers, BOOL FAR * Cancel)
{
	//���ñ���
	m_cbStatus=BROWSER_STATUS_LOAD;

	//�ƶ�����
	if (m_WebBrowser.m_hWnd!=NULL) m_WebBrowser.SetWindowPos(NULL,0,0,0,0,SWP_NOZORDER);

	return;
}

//////////////////////////////////////////////////////////////////////////
