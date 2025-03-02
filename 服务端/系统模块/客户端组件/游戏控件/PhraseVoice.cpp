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

//构造函数
CPhraseVoice::CPhraseVoice(CWnd * pParentWnd) : CDialog(IDD_PHRASE_VOICE,pParentWnd)
{
	//属性变量
	m_bHovering = false;
	m_nCurrentPhrase = 0;
	m_nPhraseHover = -1;
	ZeroMemory(m_dwSoundID,sizeof(m_dwSoundID));

	//配置变量
	m_bConfig = false;
	m_pParentWnd = NULL;
	m_ptBenchmarkPos.SetPoint(0,0);
	ZeroMemory( &m_PhraseVoiceInfo,sizeof(m_PhraseVoiceInfo) );

	//辅助变量
	m_pIPhraseVoiceSink = NULL;

	return;
}

//析构函数
CPhraseVoice::~CPhraseVoice()
{
	//释放短语
	for( INT_PTR i = 0; i < m_arrPhraseVoice.GetCount(); i++ )
	{
		SafeDelete(m_arrPhraseVoice[i]);
	}
	m_arrPhraseVoice.RemoveAll();
}

//接口查询
VOID * __cdecl CPhraseVoice::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IPhraseVoice,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IPhraseVoice,Guid,dwQueryVer);
	return NULL;
}

//初始函数
VOID __cdecl CPhraseVoice::InitPhraseVoice( const tagPhraseVoiceInfo &PhraseVoiceInfo, CWnd *pParentWnd )
{
	//效验
	ASSERT( !m_bConfig );
	if( m_bConfig ) return;

	//设置变量
	m_PhraseVoiceInfo = PhraseVoiceInfo;
	m_pParentWnd = pParentWnd;

	//设置组件
	//m_DirectSound.SetSoundEventSink(this);

	//获取当前程序路径
	TCHAR szExePath[MAX_PATH] = {};
	GetModuleFileName( NULL,szExePath,CountArray(szExePath) );
	_tcsupr_s(szExePath);

	//分析文件
	TCHAR szExtension[] = TEXT(".EXE");
	LPCTSTR lpszExtension = _tcsstr( szExePath,szExtension );
	if( lpszExtension != szExtension && lpszExtension != NULL )
	{
		INT nIndex = (INT)(lpszExtension-szExePath);
		szExePath[nIndex] = 0;
	}

	//获取配置文件
	TCHAR szConfigFileName[MAX_PATH] = {};
	_sntprintf_s( szConfigFileName,CountArray(szConfigFileName),TEXT("%s\\Phrase\\PhraseVoice.ini"),szExePath );

	//加载短语
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
			ShowInformation(TEXT("抱歉，短语语音控件初始化失败！"),MB_OK,15);
			return ;
		}
		
		//子项名
		_sntprintf_s( szItemName,CountArray(szItemName),TEXT("Phrase%ld"),i );

		//短语字串
		GetPrivateProfileString( szItemName,TEXT("Phrase"),TEXT(""),pPhrase->szPhrase,CountArray(pPhrase->szPhrase),szConfigFileName );

		//女声
		GetPrivateProfileString( szItemName,TEXT("SoundGirl"),TEXT(""),szBuffer,CountArray(szBuffer),szConfigFileName );
		_sntprintf_s( pPhrase->szGirlVoicePath,CountArray(pPhrase->szGirlVoicePath),TEXT("%s\\Phrase\\%s"),szExePath,szBuffer );

		//男声
		GetPrivateProfileString( szItemName,TEXT("SoundBoy"),TEXT(""),szBuffer,CountArray(szBuffer),szConfigFileName );
		_sntprintf_s( pPhrase->szBoyVoicePath,CountArray(pPhrase->szBoyVoicePath),TEXT("%s\\Phrase\\%s"),szExePath,szBuffer );

		//添加短语
		m_arrPhraseVoice.Add(pPhrase);
	}

	//m_DirectSound.InitDirectSound(pParentWnd);

	m_bConfig = true;

	return;
}

//基准位置
VOID __cdecl CPhraseVoice::SetBenchmarkPos( INT nXPos, INT nYPos )
{
	if( nXPos != m_ptBenchmarkPos.x || nYPos != m_ptBenchmarkPos.y )
	{
		m_ptBenchmarkPos.SetPoint(nXPos,nYPos);

		if( m_hWnd ) RectifyControl();
	}

	return;
}

//基准位置
VOID __cdecl CPhraseVoice::SetBenchmarkPos( CPoint ptBenchmarkPos )
{
	if( m_ptBenchmarkPos != ptBenchmarkPos )
	{
		m_ptBenchmarkPos = ptBenchmarkPos;

		if( m_hWnd ) RectifyControl();
	}
}

//设置回调
VOID __cdecl CPhraseVoice::SetPhraseVoiceSink( IPhraseVoiceSink *pIPhraseVoiceSink )
{
	m_pIPhraseVoiceSink = pIPhraseVoiceSink;
}

//显示短语
VOID __cdecl CPhraseVoice::ShowPhraseWindow()
{
	//效验
	ASSERT( m_bConfig );
	if( !m_bConfig ) return;

	//创建窗口
	if( m_hWnd == NULL )
	{
		AfxSetResourceHandle(GetModuleHandle(CLIENT_CONTROLS_DLL_NAME));
		Create(IDD_PHRASE_VOICE,m_pParentWnd);
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	//更新按钮
	UpdateButtons();

	//显示窗口
	ShowWindow(SW_SHOW);
	SetFocus();
}

//获取短语
LPCTSTR __cdecl CPhraseVoice::GetPhraseString( INT nPhraseIndex, TCHAR szResult[], INT nMaxLength )
{
	//效验
	ASSERT( m_bConfig );
	if( !m_bConfig ) return NULL;
	ASSERT( nPhraseIndex < m_arrPhraseVoice.GetCount() );
	if( nPhraseIndex >= m_arrPhraseVoice.GetCount() ) return NULL;

	if( szResult != NULL )
		lstrcpyn( szResult,m_arrPhraseVoice[nPhraseIndex]->szPhrase,nMaxLength );

	return m_arrPhraseVoice[nPhraseIndex]->szPhrase;
}

//播放短语
VOID __cdecl CPhraseVoice::PlayPhraseVoice( WORD wChairID, INT nPhraseIndex, INT nGender )
{
	//效验
	ASSERT( m_bConfig );
	if( !m_bConfig ) return;
	ASSERT( wChairID != INVALID_CHAIR );
	if( wChairID == INVALID_CHAIR ) return;
	ASSERT( nPhraseIndex < m_arrPhraseVoice.GetCount() );
	if( nPhraseIndex >= m_arrPhraseVoice.GetCount() ) return;

	//判断玩家是否在语音
	if( m_dwSoundID[wChairID] != 0 ) return;

	//文件路径
	LPCTSTR lpszFileName = (nGender==2)?m_arrPhraseVoice[nPhraseIndex]->szBoyVoicePath:m_arrPhraseVoice[nPhraseIndex]->szGirlVoicePath;

	//播放声音
	//m_dwSoundID[wChairID] = m_DirectSound.CreateFromFile(lpszFileName,DSBCAPS_GLOBALFOCUS);
	//if( m_dwSoundID[wChairID] != 0 )
	//	m_DirectSound.Play(m_dwSoundID[wChairID],TRUE);

	return;
}

//播放结束
VOID __cdecl CPhraseVoice::OnEventSoundStop( DWORD dwSoundID )
{
	//设置玩家状态
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

//映射函数
VOID CPhraseVoice::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_BT_UP,m_btUp);
	DDX_Control(pDX,IDC_BT_DOWN,m_btDown);
}

//初始化消息
BOOL CPhraseVoice::OnInitDialog()
{
	__super::OnInitDialog();

	//效验
	ASSERT( m_bConfig );
	if( !m_bConfig ) return FALSE;

	//变量定义
	HINSTANCE hResHinstance = AfxGetInstanceHandle();

#pragma warning( push )
#pragma warning( disable : 4311 )
	//设置组件
	if( !m_ImagePhraseBack.SetLoadInfo(m_PhraseVoiceInfo.pszImageFrame,hResHinstance) || 
		!m_PngFocusLine.LoadImage(hResHinstance,m_PhraseVoiceInfo.pszPngFocus) ||
		!m_btUp.SetButtonImage((UINT)m_PhraseVoiceInfo.pszImageBtUp,hResHinstance,false,true) || 
		!m_btDown.SetButtonImage((UINT)m_PhraseVoiceInfo.pszImageBtDown,hResHinstance,false,true) )
	{
		ShowInformation(TEXT("抱歉，短语语音控件初始化失败！"),MB_OK,15);
		m_bConfig = false;
		return FALSE;
	}
#pragma warning( pop )

	CImageHandle HandleFrame(&m_ImagePhraseBack);
	//创建区域
	CRgn RgnControl;
	m_ImagePhraseBack.CreateImageRegion(RgnControl,RGB(255,0,255));
	if (RgnControl.GetSafeHandle()!=NULL)
	{
		//移动窗口
		SetWindowRgn(RgnControl,FALSE);
		SetWindowPos(NULL,0,0,m_ImagePhraseBack.GetWidth(),m_ImagePhraseBack.GetHeight(),SWP_NOMOVE);
	}

	//调整按钮
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

		//设置透明
		ModifyStyleEx(0,0x00080000);
		HINSTANCE hInst = LoadLibrary(TEXT("User32.DLL")); 
		if(hInst) 
		{ 
			typedef BOOL (WINAPI *MYFUNC)(HWND,COLORREF,BYTE,DWORD); 
			MYFUNC fun = NULL;
			//取得SetLayeredWindowAttributes（）函数指针 
			fun=(MYFUNC)GetProcAddress(hInst, "SetLayeredWindowAttributes");
			if(fun)fun(this->GetSafeHwnd(),0,nTransparent,2); 
			FreeLibrary(hInst); 
		}
	}

	//添加提示区域
	INT nLineWidth = m_PngFocusLine.GetWidth();
	INT nLineHeight = m_PngFocusLine.GetHeight();
	INT nPageCount = m_PhraseVoiceInfo.nPageCount;
	CRect rcTipText(m_PhraseVoiceInfo.cxBorderWidth+2,m_PhraseVoiceInfo.cyBorderHeight,
		nLineWidth-m_PhraseVoiceInfo.cxBorderWidth-2,m_PhraseVoiceInfo.cyBorderHeight+nLineHeight);

	m_ToolTipCtrl.Create(this);
	m_ToolTipCtrl.Activate(TRUE);
	for( INT nIndex = 0; nIndex < nPageCount && nIndex < m_arrPhraseVoice.GetCount(); nIndex++ )
	{
		//添加区域
		m_ToolTipCtrl.AddTool( this,m_arrPhraseVoice[nIndex]->szPhrase,&rcTipText,nIndex+1 );

		//设置偏移
		rcTipText.OffsetRect(0,nLineHeight);
	}	
	
	//更新按钮
	UpdateButtons();

	//调整位置
	RectifyControl();

	return TRUE;
}

//按钮点击
VOID CPhraseVoice::OnBnClickedDown()
{
	INT nCurrentPhrase = __min( m_nCurrentPhrase+m_PhraseVoiceInfo.nPageCount,(INT)m_arrPhraseVoice.GetCount()-1 );
	if( nCurrentPhrase == m_nCurrentPhrase ) return ;

	m_nCurrentPhrase = nCurrentPhrase;

	//更新提示
	UpdateButtons();

	//更新提示
	UpdateToolTip();

	Invalidate(FALSE);

	SetFocus();
}

//按钮点击
VOID CPhraseVoice::OnBnClickedUp()
{
	INT nCurrentPhrase = __max( 0,m_nCurrentPhrase-m_PhraseVoiceInfo.nPageCount );
	if( nCurrentPhrase == m_nCurrentPhrase ) return;

	m_nCurrentPhrase = nCurrentPhrase;

	//更新按钮
	UpdateButtons();

	//更新提示
	UpdateToolTip();

	Invalidate(FALSE);

	SetFocus();
}

//消息解释
BOOL CPhraseVoice::PreTranslateMessage(MSG* pMsg)
{
	//提示消息
	if (m_ToolTipCtrl.m_hWnd!=NULL) 
	{
		m_ToolTipCtrl.RelayEvent(pMsg);
	}

	return __super::PreTranslateMessage(pMsg);
}

//调整控件
VOID CPhraseVoice::UpdateButtons()
{
	m_btUp.EnableWindow(m_nCurrentPhrase==0?FALSE:TRUE);
	m_btDown.EnableWindow(m_nCurrentPhrase+m_PhraseVoiceInfo.nPageCount>=m_arrPhraseVoice.GetCount()?FALSE:TRUE);
}

//调整控件
VOID CPhraseVoice::RectifyControl()
{
	//效验
	ASSERT( m_hWnd );
	if( m_hWnd == NULL ) return;
	
	//变量定义
	CRect rcClient;
	GetClientRect(&rcClient);

	//调整位置
	SetWindowPos(NULL,m_ptBenchmarkPos.x,m_ptBenchmarkPos.y-rcClient.Height(),rcClient.Width(),rcClient.Height(),SWP_NOZORDER);

	return;
}

//转换索引
INT CPhraseVoice::SwitchToPhraseIndex( CPoint point )
{
	//变量定义
	CRect rcClient;
	GetClientRect(rcClient);

	if( point.x < m_PhraseVoiceInfo.cxBorderWidth || point.x > rcClient.Width()-m_PhraseVoiceInfo.cxBorderWidth || 
		point.y > rcClient.Height()-m_PhraseVoiceInfo.cyBottomHeight || point.y-m_PhraseVoiceInfo.cyBorderHeight < 0 ) 
		return -1;

	//计算索引
	INT nPhraseIndex = (point.y-m_PhraseVoiceInfo.cyBorderHeight)/m_PngFocusLine.GetHeight();
	if( m_nCurrentPhrase+nPhraseIndex >= m_arrPhraseVoice.GetCount() )
		nPhraseIndex = -1;

	return nPhraseIndex;
}

//绘画消息
VOID CPhraseVoice::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//创建缓冲
	CDC DCBuffer;
	CBitmap ImageBuffer;
	DCBuffer.CreateCompatibleDC(&dc);
	ImageBuffer.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());

	//设置 DC
	DCBuffer.SetBkMode(TRANSPARENT);
	DCBuffer.SelectObject(&ImageBuffer);
	DCBuffer.SelectObject(&CSkinResourceManager::GetDefaultFont());
	DCBuffer.SetTextColor(m_PhraseVoiceInfo.crTextColor);

	//绘画背景
	CImageHandle ImageHandle(&m_ImagePhraseBack);
	m_ImagePhraseBack.BitBlt(DCBuffer,0,0);

	INT nLineWidth = m_PngFocusLine.GetWidth();
	INT nLineHeight = m_PngFocusLine.GetHeight();
	INT nPageCount = m_PhraseVoiceInfo.nPageCount;
	//绘画焦点
	if( m_nPhraseHover != -1 )
		m_PngFocusLine.DrawImage( &DCBuffer,0,m_PhraseVoiceInfo.cyBorderHeight+(m_nPhraseHover%nPageCount)*nLineHeight );

	CRect rcDrawText(m_PhraseVoiceInfo.cxBorderWidth+2,m_PhraseVoiceInfo.cyBorderHeight,
		nLineWidth-m_PhraseVoiceInfo.cxBorderWidth-2,m_PhraseVoiceInfo.cyBorderHeight+nLineHeight);
	for( INT nIndex = m_nCurrentPhrase; nIndex < m_arrPhraseVoice.GetCount(); nIndex++ )
	{
		DCBuffer.DrawText( m_arrPhraseVoice[nIndex]->szPhrase,&rcDrawText,DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS );

		rcDrawText.OffsetRect(0,nLineHeight);

		//只画nPageCount条
		if( nIndex-m_nCurrentPhrase+1 == nPageCount ) break;
	}

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&DCBuffer,0,0,SRCCOPY);

	//清理资源
	DCBuffer.DeleteDC();
	ImageBuffer.DeleteObject();

	return;
}

//绘画背景
BOOL CPhraseVoice::OnEraseBkgnd(CDC * pDC)
{
	//更新窗口
	Invalidate(FALSE);
	UpdateWindow();

	return TRUE;
}

//鼠标消息
VOID CPhraseVoice::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags,Point);

	//发送消息
	if( m_nPhraseHover!=-1 )
	{
		INT nPhraseIndex = m_nPhraseHover+m_nCurrentPhrase;

		//通知事件
		if( m_pIPhraseVoiceSink )
			m_pIPhraseVoiceSink->OnEventPhraseSelect(nPhraseIndex);

		//隐藏窗口
		ShowWindow(SW_HIDE);

		return;
	}

	return;
}

//鼠标移动
VOID CPhraseVoice::OnMouseMove(UINT nFlags, CPoint point)
{
	__super::OnMouseMove(nFlags, point);

	if( !m_bHovering )
	{
		////设置变量
		//m_bHovering=true;

		////变量定义
		//TRACKMOUSEEVENT TrackMouseEvent;
		//ZeroMemory(&TrackMouseEvent,sizeof(TrackMouseEvent));

		////注册消息
		//TrackMouseEvent.hwndTrack=m_hWnd;
		//TrackMouseEvent.dwFlags=TME_LEAVE;
		//TrackMouseEvent.dwHoverTime=HOVER_DEFAULT;
		//TrackMouseEvent.cbSize=sizeof(TrackMouseEvent);

		////注册事件
		//::TrackMouseEvent(&TrackMouseEvent);

		//SetFocus();
	}

	//获取索引
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

//更新提示
VOID CPhraseVoice::UpdateToolTip()
{
	//效验
	ASSERT(m_bConfig);
	if( !m_bConfig ) return;

	if( m_ToolTipCtrl.m_hWnd )
	{
		INT nPageCount = m_PhraseVoiceInfo.nPageCount;
		for( INT nPageIndex = 0; nPageIndex < nPageCount; nPageIndex++ )
		{
			//更新提示文字
			if( m_nCurrentPhrase+nPageIndex < m_arrPhraseVoice.GetCount() )
				m_ToolTipCtrl.UpdateTipText( m_arrPhraseVoice[m_nCurrentPhrase+nPageIndex]->szPhrase,this,nPageIndex+1 );
			else m_ToolTipCtrl.UpdateTipText( TEXT(""),this,nPageIndex+1 );
		}
	}

	return;
}

//焦点消息
void CPhraseVoice::OnKillFocus(CWnd* pNewWnd)
{
	__super::OnKillFocus(pNewWnd);

	//隐藏窗口
	if( !IsChild(pNewWnd) )
	{
		ShowWindow(SW_HIDE);
	}
}

//按钮焦点
VOID CPhraseVoice::OnBnKillFocus()
{
	CWnd *pWnd = GetFocus();
	if( pWnd && pWnd->m_hWnd != m_hWnd && !IsChild(pWnd) )
	{
		ShowWindow(SW_HIDE);
	}
}

//////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(PhraseVoice);

//////////////////////////////////////////////////////////////////////////
