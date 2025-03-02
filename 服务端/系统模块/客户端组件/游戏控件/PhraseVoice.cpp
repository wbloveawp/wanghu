#include "StdAfx.h"
#include "Resource.h"
#include "PhraseVoice.h"
#include "Information.h"

#define new DEBUG_NEW

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CPhraseVoice, CDialog)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_KILLFOCUS()
	ON_BN_CLICKED(IDC_BT_DOWN,OnBnClickedDown)
	ON_BN_CLICKED(IDC_BT_UP,OnBnClickedUp)
	ON_BN_KILLFOCUS(IDC_BT_DOWN,OnBnKillFocus)
	ON_BN_KILLFOCUS(IDC_BT_UP,OnBnKillFocus)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//���캯��
CPhraseVoice::CPhraseVoice(CWnd * pParentWnd) : CDialog(IDD_PHRASE_VOICE,pParentWnd)
{
	//���Ա���
	m_bHovering = false;
	m_nCurrentPhrase = 0;
	m_nPhraseHover = -1;
	ZeroMemory(m_dwSoundID,sizeof(m_dwSoundID));

	//���ñ���
	m_bConfig = false;
	m_pParentWnd = NULL;
	m_ptBenchmarkPos.SetPoint(0,0);
	ZeroMemory( &m_PhraseVoiceInfo,sizeof(m_PhraseVoiceInfo) );

	//��������
	m_pIPhraseVoiceSink = NULL;

	return;
}

//��������
CPhraseVoice::~CPhraseVoice()
{
	//�ͷŶ���
	for( INT_PTR i = 0; i < m_arrPhraseVoice.GetCount(); i++ )
	{
		SafeDelete(m_arrPhraseVoice[i]);
	}
	m_arrPhraseVoice.RemoveAll();
}

//�ӿڲ�ѯ
VOID * __cdecl CPhraseVoice::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IPhraseVoice,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IPhraseVoice,Guid,dwQueryVer);
	return NULL;
}

//��ʼ����
VOID __cdecl CPhraseVoice::InitPhraseVoice( const tagPhraseVoiceInfo &PhraseVoiceInfo, CWnd *pParentWnd )
{
	//Ч��
	ASSERT( !m_bConfig );
	if( m_bConfig ) return;

	//���ñ���
	m_PhraseVoiceInfo = PhraseVoiceInfo;
	m_pParentWnd = pParentWnd;

	//�������
	//m_DirectSound.SetSoundEventSink(this);

	//��ȡ��ǰ����·��
	TCHAR szExePath[MAX_PATH] = {};
	GetModuleFileName( NULL,szExePath,CountArray(szExePath) );
	_tcsupr_s(szExePath);

	//�����ļ�
	TCHAR szExtension[] = TEXT(".EXE");
	LPCTSTR lpszExtension = _tcsstr( szExePath,szExtension );
	if( lpszExtension != szExtension && lpszExtension != NULL )
	{
		INT nIndex = (INT)(lpszExtension-szExePath);
		szExePath[nIndex] = 0;
	}

	//��ȡ�����ļ�
	TCHAR szConfigFileName[MAX_PATH] = {};
	_sntprintf_s( szConfigFileName,CountArray(szConfigFileName),TEXT("%s\\Phrase\\PhraseVoice.ini"),szExePath );

	//���ض���
	INT nPhraseCount = GetPrivateProfileInt( TEXT("PhraseInfo"),TEXT("Count"),0,szConfigFileName );
	TCHAR szItemName[256] = {};
	TCHAR szBuffer[MAX_PATH] = {};
	TCHAR szFileName[MAX_PATH] = {};
	for( INT i = 0; i < nPhraseCount; i++ )
	{
		tagPhraseVoice *pPhrase = NULL;
		try{ pPhrase = new tagPhraseVoice; }
		catch(...)
		{
			ShowInformation(TEXT("��Ǹ�����������ؼ���ʼ��ʧ�ܣ�"),MB_OK,15);
			return ;
		}
		
		//������
		_sntprintf_s( szItemName,CountArray(szItemName),TEXT("Phrase%ld"),i );

		//�����ִ�
		GetPrivateProfileString( szItemName,TEXT("Phrase"),TEXT(""),pPhrase->szPhrase,CountArray(pPhrase->szPhrase),szConfigFileName );

		//Ů��
		GetPrivateProfileString( szItemName,TEXT("SoundGirl"),TEXT(""),szBuffer,CountArray(szBuffer),szConfigFileName );
		_sntprintf_s( pPhrase->szGirlVoicePath,CountArray(pPhrase->szGirlVoicePath),TEXT("%s\\Phrase\\%s"),szExePath,szBuffer );

		//����
		GetPrivateProfileString( szItemName,TEXT("SoundBoy"),TEXT(""),szBuffer,CountArray(szBuffer),szConfigFileName );
		_sntprintf_s( pPhrase->szBoyVoicePath,CountArray(pPhrase->szBoyVoicePath),TEXT("%s\\Phrase\\%s"),szExePath,szBuffer );

		//��Ӷ���
		m_arrPhraseVoice.Add(pPhrase);
	}

	//m_DirectSound.InitDirectSound(pParentWnd);

	m_bConfig = true;

	return;
}

//��׼λ��
VOID __cdecl CPhraseVoice::SetBenchmarkPos( INT nXPos, INT nYPos )
{
	if( nXPos != m_ptBenchmarkPos.x || nYPos != m_ptBenchmarkPos.y )
	{
		m_ptBenchmarkPos.SetPoint(nXPos,nYPos);

		if( m_hWnd ) RectifyControl();
	}

	return;
}

//��׼λ��
VOID __cdecl CPhraseVoice::SetBenchmarkPos( CPoint ptBenchmarkPos )
{
	if( m_ptBenchmarkPos != ptBenchmarkPos )
	{
		m_ptBenchmarkPos = ptBenchmarkPos;

		if( m_hWnd ) RectifyControl();
	}
}

//���ûص�
VOID __cdecl CPhraseVoice::SetPhraseVoiceSink( IPhraseVoiceSink *pIPhraseVoiceSink )
{
	m_pIPhraseVoiceSink = pIPhraseVoiceSink;
}

//��ʾ����
VOID __cdecl CPhraseVoice::ShowPhraseWindow()
{
	//Ч��
	ASSERT( m_bConfig );
	if( !m_bConfig ) return;

	//��������
	if( m_hWnd == NULL )
	{
		AfxSetResourceHandle(GetModuleHandle(CLIENT_CONTROLS_DLL_NAME));
		Create(IDD_PHRASE_VOICE,m_pParentWnd);
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//���°�ť
	UpdateButtons();

	//��ʾ����
	ShowWindow(SW_SHOW);
	SetFocus();
}

//��ȡ����
LPCTSTR __cdecl CPhraseVoice::GetPhraseString( INT nPhraseIndex, TCHAR szResult[], INT nMaxLength )
{
	//Ч��
	ASSERT( m_bConfig );
	if( !m_bConfig ) return NULL;
	ASSERT( nPhraseIndex < m_arrPhraseVoice.GetCount() );
	if( nPhraseIndex >= m_arrPhraseVoice.GetCount() ) return NULL;

	if( szResult != NULL )
		lstrcpyn( szResult,m_arrPhraseVoice[nPhraseIndex]->szPhrase,nMaxLength );

	return m_arrPhraseVoice[nPhraseIndex]->szPhrase;
}

//���Ŷ���
VOID __cdecl CPhraseVoice::PlayPhraseVoice( WORD wChairID, INT nPhraseIndex, INT nGender )
{
	//Ч��
	ASSERT( m_bConfig );
	if( !m_bConfig ) return;
	ASSERT( wChairID != INVALID_CHAIR );
	if( wChairID == INVALID_CHAIR ) return;
	ASSERT( nPhraseIndex < m_arrPhraseVoice.GetCount() );
	if( nPhraseIndex >= m_arrPhraseVoice.GetCount() ) return;

	//�ж�����Ƿ�������
	if( m_dwSoundID[wChairID] != 0 ) return;

	//�ļ�·��
	LPCTSTR lpszFileName = (nGender==2)?m_arrPhraseVoice[nPhraseIndex]->szBoyVoicePath:m_arrPhraseVoice[nPhraseIndex]->szGirlVoicePath;

	//��������
	//m_dwSoundID[wChairID] = m_DirectSound.CreateFromFile(lpszFileName,DSBCAPS_GLOBALFOCUS);
	//if( m_dwSoundID[wChairID] != 0 )
	//	m_DirectSound.Play(m_dwSoundID[wChairID],TRUE);

	return;
}

//���Ž���
VOID __cdecl CPhraseVoice::OnEventSoundStop( DWORD dwSoundID )
{
	//�������״̬
	for( WORD i = 0; i < CountArray(m_dwSoundID); i++ )
	{
		if( dwSoundID == m_dwSoundID[i] )
		{
			m_dwSoundID[i] = 0;
			return;
		}
	}

	return;
}

//ӳ�亯��
VOID CPhraseVoice::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_BT_UP,m_btUp);
	DDX_Control(pDX,IDC_BT_DOWN,m_btDown);
}

//��ʼ����Ϣ
BOOL CPhraseVoice::OnInitDialog()
{
	__super::OnInitDialog();

	//Ч��
	ASSERT( m_bConfig );
	if( !m_bConfig ) return FALSE;

	//��������
	HINSTANCE hResHinstance = AfxGetInstanceHandle();

#pragma warning( push )
#pragma warning( disable : 4311 )
	//�������
	if( !m_ImagePhraseBack.SetLoadInfo(m_PhraseVoiceInfo.pszImageFrame,hResHinstance) || 
		!m_PngFocusLine.LoadImage(hResHinstance,m_PhraseVoiceInfo.pszPngFocus) ||
		!m_btUp.SetButtonImage((UINT)m_PhraseVoiceInfo.pszImageBtUp,hResHinstance,false,true) || 
		!m_btDown.SetButtonImage((UINT)m_PhraseVoiceInfo.pszImageBtDown,hResHinstance,false,true) )
	{
		ShowInformation(TEXT("��Ǹ�����������ؼ���ʼ��ʧ�ܣ�"),MB_OK,15);
		m_bConfig = false;
		return FALSE;
	}
#pragma warning( pop )

	CImageHandle HandleFrame(&m_ImagePhraseBack);
	//��������
	CRgn RgnControl;
	m_ImagePhraseBack.CreateImageRegion(RgnControl,RGB(255,0,255));
	if (RgnControl.GetSafeHandle()!=NULL)
	{
		//�ƶ�����
		SetWindowRgn(RgnControl,FALSE);
		SetWindowPos(NULL,0,0,m_ImagePhraseBack.GetWidth(),m_ImagePhraseBack.GetHeight(),SWP_NOMOVE);
	}

	//������ť
	CRect rcClient;
	GetClientRect(&rcClient);
	CRect rcBtControl;
	m_btUp.GetWindowRect(&rcBtControl);
	m_btUp.SetWindowPos(NULL,rcClient.Width()/2-rcBtControl.Width()-5,rcClient.Height()-rcBtControl.Height()-5,0,0,SWP_NOSIZE|SWP_NOZORDER);
	m_btDown.SetWindowPos(NULL,rcClient.Width()/2+5,rcClient.Height()-rcBtControl.Height()-5,0,0,SWP_NOSIZE|SWP_NOZORDER);

	INT nTransparent = 255;
	if( m_PhraseVoiceInfo.nTransPercent != 0 )
	{
		nTransparent *= (100-m_PhraseVoiceInfo.nTransPercent);
		nTransparent /= 100;

		//����͸��
		ModifyStyleEx(0,0x00080000);
		HINSTANCE hInst = LoadLibrary(TEXT("User32.DLL")); 
		if(hInst) 
		{ 
			typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD); 
			MYFUNC fun = NULL;
			//ȡ��SetLayeredWindowAttributes��������ָ�� 
			fun=(MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
			if(fun)fun(this->GetSafeHwnd(),0,nTransparent,2); 
			FreeLibrary(hInst); 
		}
	}

	//�����ʾ����
	INT nLineWidth = m_PngFocusLine.GetWidth();
	INT nLineHeight = m_PngFocusLine.GetHeight();
	INT nPageCount = m_PhraseVoiceInfo.nPageCount;
	CRect rcTipText(m_PhraseVoiceInfo.cxBorderWidth+2,m_PhraseVoiceInfo.cyBorderHeight,
		nLineWidth-m_PhraseVoiceInfo.cxBorderWidth-2,m_PhraseVoiceInfo.cyBorderHeight+nLineHeight);

	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	for( INT nIndex = 0; nIndex < nPageCount && nIndex < m_arrPhraseVoice.GetCount(); nIndex++ )
	{
		//�������
		m_ToolTipCtrl.AddTool( this,m_arrPhraseVoice[nIndex]->szPhrase,&rcTipText,nIndex+1 );

		//����ƫ��
		rcTipText.OffsetRect(0,nLineHeight);
	}	
	
	//���°�ť
	UpdateButtons();

	//����λ��
	RectifyControl();

	return TRUE;
}

//��ť���
VOID CPhraseVoice::OnBnClickedDown()
{
	INT nCurrentPhrase = __min( m_nCurrentPhrase+m_PhraseVoiceInfo.nPageCount,(INT)m_arrPhraseVoice.GetCount()-1 );
	if( nCurrentPhrase == m_nCurrentPhrase ) return ;

	m_nCurrentPhrase = nCurrentPhrase;

	//������ʾ
	UpdateButtons();

	//������ʾ
	UpdateToolTip();

	Invalidate(FALSE);

	SetFocus();
}

//��ť���
VOID CPhraseVoice::OnBnClickedUp()
{
	INT nCurrentPhrase = __max( 0,m_nCurrentPhrase-m_PhraseVoiceInfo.nPageCount );
	if( nCurrentPhrase == m_nCurrentPhrase ) return;

	m_nCurrentPhrase = nCurrentPhrase;

	//���°�ť
	UpdateButtons();

	//������ʾ
	UpdateToolTip();

	Invalidate(FALSE);

	SetFocus();
}

//��Ϣ����
BOOL CPhraseVoice::PreTranslateMessage(MSG* pMsg)
{
	//��ʾ��Ϣ
	if (m_ToolTipCtrl.m_hWnd!=NULL) 
	{
		m_ToolTipCtrl.RelayEvent(pMsg);
	}

	return __super::PreTranslateMessage(pMsg);
}

//�����ؼ�
VOID CPhraseVoice::UpdateButtons()
{
	m_btUp.EnableWindow(m_nCurrentPhrase==0?FALSE:TRUE);
	m_btDown.EnableWindow(m_nCurrentPhrase+m_PhraseVoiceInfo.nPageCount>=m_arrPhraseVoice.GetCount()?FALSE:TRUE);
}

//�����ؼ�
VOID CPhraseVoice::RectifyControl()
{
	//Ч��
	ASSERT( m_hWnd );
	if( m_hWnd == NULL ) return;
	
	//��������
	CRect rcClient;
	GetClientRect(&rcClient);

	//����λ��
	SetWindowPos(NULL,m_ptBenchmarkPos.x,m_ptBenchmarkPos.y-rcClient.Height(),rcClient.Width(),rcClient.Height(),SWP_NOZORDER);

	return;
}

//ת������
INT CPhraseVoice::SwitchToPhraseIndex( CPoint point )
{
	//��������
	CRect rcClient;
	GetClientRect(rcClient);

	if( point.x < m_PhraseVoiceInfo.cxBorderWidth || point.x > rcClient.Width()-m_PhraseVoiceInfo.cxBorderWidth || 
		point.y > rcClient.Height()-m_PhraseVoiceInfo.cyBottomHeight || point.y-m_PhraseVoiceInfo.cyBorderHeight < 0 ) 
		return -1;

	//��������
	INT nPhraseIndex = (point.y-m_PhraseVoiceInfo.cyBorderHeight)/m_PngFocusLine.GetHeight();
	if( m_nCurrentPhrase+nPhraseIndex >= m_arrPhraseVoice.GetCount() )
		nPhraseIndex = -1;

	return nPhraseIndex;
}

//�滭��Ϣ
VOID CPhraseVoice::OnPaint()
{
	CPaintDC dc(this);

	//��ȡλ��
	CRect rcClient;
	GetClientRect(&rcClient);

	//��������
	CDC DCBuffer;
	CBitmap ImageBuffer;
	DCBuffer.CreateCompatibleDC(&dc);
	ImageBuffer.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());

	//���� DC
	DCBuffer.SetBkMode(TRANSPARENT);
	DCBuffer.SelectObject(&ImageBuffer);
	DCBuffer.SelectObject(&CSkinResourceManager::GetDefaultFont());
	DCBuffer.SetTextColor(m_PhraseVoiceInfo.crTextColor);

	//�滭����
	CImageHandle ImageHandle(&m_ImagePhraseBack);
	m_ImagePhraseBack.BitBlt(DCBuffer,0,0);

	INT nLineWidth = m_PngFocusLine.GetWidth();
	INT nLineHeight = m_PngFocusLine.GetHeight();
	INT nPageCount = m_PhraseVoiceInfo.nPageCount;
	//�滭����
	if( m_nPhraseHover != -1 )
		m_PngFocusLine.DrawImage( &DCBuffer,0,m_PhraseVoiceInfo.cyBorderHeight+(m_nPhraseHover%nPageCount)*nLineHeight );

	CRect rcDrawText(m_PhraseVoiceInfo.cxBorderWidth+2,m_PhraseVoiceInfo.cyBorderHeight,
		nLineWidth-m_PhraseVoiceInfo.cxBorderWidth-2,m_PhraseVoiceInfo.cyBorderHeight+nLineHeight);
	for( INT nIndex = m_nCurrentPhrase; nIndex < m_arrPhraseVoice.GetCount(); nIndex++ )
	{
		DCBuffer.DrawText( m_arrPhraseVoice[nIndex]->szPhrase,&rcDrawText,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS );

		rcDrawText.OffsetRect(0,nLineHeight);

		//ֻ��nPageCount��
		if( nIndex-m_nCurrentPhrase+1 == nPageCount ) break;
	}

	//�滭����
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&DCBuffer,0,0,SRCCOPY);

	//������Դ
	DCBuffer.DeleteDC();
	ImageBuffer.DeleteObject();

	return;
}

//�滭����
BOOL CPhraseVoice::OnEraseBkgnd(CDC * pDC)
{
	//���´���
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//�����Ϣ
VOID CPhraseVoice::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//������Ϣ
	if( m_nPhraseHover!=-1 )
	{
		INT nPhraseIndex = m_nPhraseHover+m_nCurrentPhrase;

		//֪ͨ�¼�
		if( m_pIPhraseVoiceSink )
			m_pIPhraseVoiceSink->OnEventPhraseSelect(nPhraseIndex);

		//���ش���
		ShowWindow(SW_HIDE);

		return;
	}

	return;
}

//����ƶ�
VOID CPhraseVoice::OnMouseMove(UINT nFlags, CPoint point)
{
	__super::OnMouseMove(nFlags, point);

	if( !m_bHovering )
	{
		////���ñ���
		//m_bHovering=true;

		////��������
		//TRACKMOUSEEVENT TrackMouseEvent;
		//ZeroMemory(&TrackMouseEvent,sizeof(TrackMouseEvent));

		////ע����Ϣ
		//TrackMouseEvent.hwndTrack=m_hWnd;
		//TrackMouseEvent.dwFlags=TME_LEAVE;
		//TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		//TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);

		////ע���¼�
		//::TrackMouseEvent(&TrackMouseEvent);

		//SetFocus();
	}

	//��ȡ����
	INT nPhraseHover = SwitchToPhraseIndex(point);

	if( nPhraseHover != m_nPhraseHover )
	{
		m_nPhraseHover = nPhraseHover;

		CRect rcClient;
		GetClientRect(rcClient);
		rcClient.bottom -= m_PhraseVoiceInfo.cyBottomHeight;
		InvalidateRect(&rcClient,FALSE);
	}
}

//������ʾ
VOID CPhraseVoice::UpdateToolTip()
{
	//Ч��
	ASSERT(m_bConfig);
	if( !m_bConfig ) return;

	if( m_ToolTipCtrl.m_hWnd )
	{
		INT nPageCount = m_PhraseVoiceInfo.nPageCount;
		for( INT nPageIndex = 0; nPageIndex < nPageCount; nPageIndex++ )
		{
			//������ʾ����
			if( m_nCurrentPhrase+nPageIndex < m_arrPhraseVoice.GetCount() )
				m_ToolTipCtrl.UpdateTipText( m_arrPhraseVoice[m_nCurrentPhrase+nPageIndex]->szPhrase,this,nPageIndex+1 );
			else m_ToolTipCtrl.UpdateTipText( TEXT(""),this,nPageIndex+1 );
		}
	}

	return;
}

//������Ϣ
void CPhraseVoice::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus(pNewWnd);

	//���ش���
	if( !IsChild(pNewWnd) )
	{
		ShowWindow(SW_HIDE);
	}
}

//��ť����
VOID CPhraseVoice::OnBnKillFocus()
{
	CWnd *pWnd = GetFocus();
	if( pWnd && pWnd->m_hWnd != m_hWnd && !IsChild(pWnd) )
	{
		ShowWindow(SW_HIDE);
	}
}

//////////////////////////////////////////////////////////////////////////

//�����������
DECLARE_CREATE_MODULE(PhraseVoice);

//////////////////////////////////////////////////////////////////////////
