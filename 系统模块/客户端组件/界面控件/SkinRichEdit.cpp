#include "Stdafx.h"
#include "SkinMenu.h"
#include "SkinRichEdit.h"

//////////////////////////////////////////////////////////////////////////////////

#pragma warning (disable : 4302)

//名字控件
using namespace ImageOleLib;

//////////////////////////////////////////////////////////////////////////
//宏定义

//菜单命令
#define IDM_CLEAR_ALL				(WM_USER+201)						//删除信息
#define IDM_SELECT_ALL				(WM_USER+202)						//全部选择
#define IDM_COPY_STRING				(WM_USER+203)						//拷贝字符
#define IDM_SAVE_STRING				(WM_USER+205)						//保存信息
#define IDM_DELETE_STRING			(WM_USER+204)						//删除字符

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinRichEdit, CRichEditCtrl)

	//系统消息
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONDOWN()

	//菜单命令
	ON_COMMAND(IDM_CLEAR_ALL, OnClearAll)
	ON_COMMAND(IDM_SELECT_ALL, OnSelectAll)
	ON_COMMAND(IDM_COPY_STRING, OnCopyString)
	ON_COMMAND(IDM_SAVE_STRING, OnSaveString)
	ON_COMMAND(IDM_DELETE_STRING, OnDeleteString)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CImageDataObject::CImageDataObject()
{
	//内核变量
	m_ulRefCnt=0;
	ZeroMemory(&m_StgMedium,sizeof(m_StgMedium));
	ZeroMemory(&m_FormatEtc,sizeof(m_FormatEtc));

	//设置接口
	m_pIStorage=NULL;
	m_pIOleObject=NULL;

	return;
}

//析构函数
CImageDataObject::~CImageDataObject()
{
	//释放对象
	::ReleaseStgMedium(&m_StgMedium);

	//释放对象
	if (m_pIOleObject!=NULL) m_pIOleObject->Release();
	if (m_pIStorage!=NULL) m_pIStorage->Release();

	return;
}

//增加引用
ULONG STDMETHODCALLTYPE CImageDataObject::AddRef()
{
	m_ulRefCnt++;
	return m_ulRefCnt;
}

//释放对象
ULONG STDMETHODCALLTYPE CImageDataObject::Release()
{
	if (--m_ulRefCnt==0) delete this;
	return m_ulRefCnt;
}

//查询接口
HRESULT STDMETHODCALLTYPE CImageDataObject::QueryInterface(REFIID iid, VOID * * ppvObject)
{
	//查询接口
	if (iid==IID_IUnknown||iid==IID_IDataObject)
	{
		*ppvObject=this;
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}

//获取数据
HRESULT STDMETHODCALLTYPE CImageDataObject::GetData(FORMATETC * pFormatEtcIn, STGMEDIUM * pStgMedium) 
{
	//状态判断
	ASSERT(m_StgMedium.hBitmap!=NULL);
	if (m_StgMedium.hBitmap==NULL) return E_HANDLE;

	//获取数据
	HANDLE hHandle=OleDuplicateData(m_StgMedium.hBitmap,CF_BITMAP,0);
	if (hHandle==NULL) return E_HANDLE;

	//设置变量
	pStgMedium->tymed=TYMED_GDI;
	pStgMedium->pUnkForRelease=NULL;
	pStgMedium->hBitmap=(HBITMAP)hHandle;

	return S_OK;
}

//设置数据
HRESULT STDMETHODCALLTYPE CImageDataObject::SetData(FORMATETC * pFormatEtc, STGMEDIUM * pStgMedium, BOOL fRelease) 
{
	//设置变量
	m_FormatEtc=*pFormatEtc;
	m_StgMedium=*pStgMedium;

	return S_OK;
}

//设置图片
bool CImageDataObject::SetImage(HBITMAP hBitmap)
{
	//对象信息
	m_StgMedium.tymed=TYMED_GDI;
	m_StgMedium.hBitmap=hBitmap;
	m_StgMedium.pUnkForRelease=NULL;

	//对象信息
	m_FormatEtc.ptd=NULL;
	m_FormatEtc.lindex=-1;
	m_FormatEtc.tymed=TYMED_GDI;
	m_FormatEtc.cfFormat=CF_BITMAP;
	m_FormatEtc.dwAspect=DVASPECT_CONTENT;

	return true;
}

//加载图片
bool CImageDataObject::LoadImage(LPCTSTR pszFileName)
{
	//加载图片
	CImage Image;
	Image.Load(pszFileName);

	//结果判断
	if (Image.IsNull()==true)
	{
		ASSERT(FALSE);
		return false;
	}

	//设置图片
	SetImage(Image.Detach());

	return true;
}

//加载图片
bool CImageDataObject::LoadImage(HINSTANCE hInstance, LPCTSTR lpBitmapName)
{
	//加载图片
	HBITMAP hBitmap=NULL;
	hBitmap=LoadBitmap(hInstance,lpBitmapName);

	//设置图片
	SetImage(hBitmap);

	return true;
}

//获取信息
bool CImageDataObject::IntercalateReObject(REOBJECT & ReObject, IOleClientSite * pIOleClientSite)
{
	//状态判断
	ASSERT(m_StgMedium.hBitmap!=NULL);
	if (m_StgMedium.hBitmap==NULL) return false;

	//创建对象
	if ((m_pIOleObject==NULL)||(m_pIStorage==NULL))
	{
		try
		{
			//创建存储
			LPLOCKBYTES lpLockBytes=NULL;
			CreateILockBytesOnHGlobal(NULL,TRUE,&lpLockBytes);
			StgCreateDocfileOnILockBytes(lpLockBytes,STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE,0,&m_pIStorage);

			//创建对象
			OleCreateStaticFromData(this,IID_IOleObject,OLERENDER_FORMAT,&m_FormatEtc,NULL,m_pIStorage,(VOID * *)&m_pIOleObject);
			OleSetContainedObject(m_pIOleObject,TRUE);

			//对象效验
			ASSERT((m_pIOleObject!=NULL)&&(m_pIStorage!=NULL));
			if ((m_pIOleObject==NULL)||(m_pIStorage==NULL)) throw 0;
		}
		catch (...)
		{
			//释放对象
			if (m_pIOleObject!=NULL)
			{
				m_pIOleObject->Release();
				m_pIOleObject=NULL;
			}

			//释放对象
			if (m_pIStorage!=NULL)
			{
				m_pIStorage->Release();
				m_pIStorage=NULL;
			}

			return false;
		}
	}

	//设置变量
	ZeroMemory(&ReObject,sizeof(ReObject));

	//属性信息
	ReObject.cbStruct=sizeof(ReObject);
	ReObject.cp=REO_CP_SELECTION;
	ReObject.dvaspect=DVASPECT_CONTENT;
	m_pIOleObject->GetUserClassID(&ReObject.clsid);

	//接口信息
	ReObject.pstg=m_pIStorage;
	ReObject.poleobj=m_pIOleObject;
	ReObject.polesite=pIOleClientSite;

	return true;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CSkinRichEdit::CSkinRichEdit()
{
	//设置变量
	m_pIRichEditOLE=NULL;

	return;
}

//析构函数
CSkinRichEdit::~CSkinRichEdit()
{
	//释放对象
	if (m_pIRichEditOLE!=NULL) 
	{
		m_pIRichEditOLE->Release();
		m_pIRichEditOLE=NULL;
	}

	return;
}

//控件绑定
VOID CSkinRichEdit::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//初始化控件
	SetEventMask(ENM_LINK);
	SetOptions(ECOOP_OR,ECO_NOHIDESEL);

	//获取接口
	m_pIRichEditOLE=GetIRichEditOle();

	return;
}

//创建消息
INT CSkinRichEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//初始化控件
	SetEventMask(ENM_LINK);
	SetOptions(ECOOP_OR,ECO_NOHIDESEL);

	//获取接口
	m_pIRichEditOLE=GetIRichEditOle();

	return 0;
}

//清除屏幕
bool CSkinRichEdit::CleanScreen()
{
	//清除屏幕
	SetSel(0,GetWindowTextLength());
	ReplaceSel(TEXT(""));

	return true;
}

//加载消息
bool CSkinRichEdit::LoadMessage(LPCTSTR pszFileName)
{
	//效验参数
	ASSERT(pszFileName!=NULL);
	if (pszFileName==NULL) return false;

	//打开文件
	CFile File;
	if (File.Open(pszFileName,CFile::modeRead,NULL)==FALSE) return false;

	//构造数据
	EDITSTREAM EditStream;
	EditStream.pfnCallback=LoadCallBack;
	EditStream.dwCookie=(DWORD)(LONGLONG)(&File);

	//读取文件
    StreamIn(SF_RTF,EditStream);

	return true;
}

//保存消息
bool CSkinRichEdit::SaveMessage(LPCTSTR pszFileName)
{
	//变量定义
	CString strFileName=pszFileName;

	//打开文件
	if (strFileName.IsEmpty()==true)
	{
		CFileDialog FileDlg(FALSE,TEXT("RTF"),0,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,TEXT("信息文件(*.RTF)|*.RTF||"),this);
		if (FileDlg.DoModal()==IDOK) strFileName=FileDlg.GetPathName();
		else return true;
	}

	//打开文件
	CFile File;
	if (File.Open(strFileName,CFile::modeWrite|CFile::modeCreate,NULL)==FALSE) return false;

	//构造数据
	EDITSTREAM EditStream;
	EditStream.pfnCallback=SaveCallBack;
	EditStream.dwCookie=(DWORD)(LONGLONG)(&File);

	//写入文件
    StreamOut(SF_RTF,EditStream);

	return true;
}

//插入图片
bool CSkinRichEdit::InsertImage(LPCTSTR pszImage)
{
	//变量定义
	LPOLEOBJECT	lpOleObject=NULL;
	LPOLECLIENTSITE pIOleClientSite=NULL;

	//保存状态
	CHARRANGE CharRange;
	GetSel(CharRange.cpMin,CharRange.cpMax);

	//变量定义
	bool bResumeSelect=(CharRange.cpMax!=CharRange.cpMin);

	try
	{
		//变量定义
		IGifAnimatorPtr lpIGifAnimator;

		//创建对象
		if (FAILED(lpIGifAnimator.CreateInstance(CLSID_GifAnimator)))
		{
			ASSERT(FALSE);
			throw 0L;
		}

		//加载图像
		if (FAILED(lpIGifAnimator->LoadFromFile((LPCWSTR)(CT2CW(pszImage)))))
		{
			ASSERT(FALSE);
			throw 0L;
		}

		//获取对象
		if (FAILED(lpIGifAnimator->QueryInterface(IID_IOleObject,(VOID * *)&lpOleObject)))
		{
			ASSERT(FALSE);
			throw 0L;
		}

		//查询接口
		ASSERT(m_pIRichEditOLE!=NULL);
		m_pIRichEditOLE->GetClientSite(&pIOleClientSite);

		//变量定义
		REOBJECT ReObject;
		ZeroMemory(&ReObject,sizeof(REOBJECT));

		//设置变量
		ReObject.cbStruct=sizeof(ReObject);	
		ReObject.dwUser=0;
		ReObject.cp=REO_CP_SELECTION;
		ReObject.dwFlags=REO_BELOWBASELINE;
		ReObject.dvaspect=DVASPECT_CONTENT;
		ReObject.pstg=NULL;
		ReObject.poleobj=lpOleObject;
		ReObject.polesite=pIOleClientSite;

		//设置选择
		SetSel(-1,-1);

		//插入对象
		m_pIRichEditOLE->InsertObject(&ReObject);

		//释放对象
		SafeRelease(pIOleClientSite);

		//状态设置
		if (bResumeSelect==true)
		{
			SetSel(CharRange);
		}
		else
		{
			SendMessage(WM_VSCROLL,SB_BOTTOM,0);
		}

		//更新窗体
		RedrawWindow(NULL,NULL,RDW_ERASE|RDW_UPDATENOW|RDW_INVALIDATE);

		return true;
	}
	catch (...)
	{
		//释放对象
		SafeRelease(lpOleObject);
		SafeRelease(pIOleClientSite);
	}

	return false;
}

//插入图片
bool CSkinRichEdit::InsertImage(CBitmap * pBitmap)
{
	//构造对象
	CImageDataObject ImageDataObject;
	ImageDataObject.SetImage((HBITMAP)pBitmap->GetSafeHandle());

	//插入图片
	InsertDataObject(&ImageDataObject);

	return false;
}

//插入对象
bool CSkinRichEdit::InsertDataObject(CImageDataObject * pImageDataObject)
{
	//效验参数
	ASSERT((m_pIRichEditOLE!=NULL)&&(pImageDataObject!=NULL));
	if ((m_pIRichEditOLE==NULL)||(pImageDataObject==NULL)) return false;

	//保存信息
	CHARRANGE CharRange;
	GetSel(CharRange);

	//变量定义
	IOleClientSite * pIOleClientSite=NULL;
	bool bResumeSelect=(CharRange.cpMax!=CharRange.cpMin);
	
	//填充信息
	REOBJECT ReObject;
	ZeroMemory(&ReObject,sizeof(ReObject));
	m_pIRichEditOLE->GetClientSite(&pIOleClientSite);

	//插入图片
	if (pImageDataObject->IntercalateReObject(ReObject,pIOleClientSite)==true)
	{
		//插入图片
		SetSel(-1L,-1L);
		m_pIRichEditOLE->InsertObject(&ReObject);

		//恢复信息
		if (bResumeSelect==true) SetSel(CharRange);
		else PostMessage(WM_VSCROLL,SB_BOTTOM,0);
	}

	//释放对象
	if (pIOleClientSite!=NULL) pIOleClientSite->Release();

	return true;
}

//插入字串
bool CSkinRichEdit::InsertString(LPCTSTR pszString, COLORREF crTextColor)
{
	//变量定义
	CHARFORMAT2 CharFormat;
	ZeroMemory(&CharFormat,sizeof(CharFormat));

	//构造数据
	CharFormat.cbSize=sizeof(CharFormat); 
	CharFormat.dwMask=CFM_COLOR|CFM_OFFSET;
	CharFormat.crTextColor=crTextColor;

	//插入字串
	InsertString(pszString,CharFormat);

	return true;
}

//插入字串
bool CSkinRichEdit::InsertString(LPCTSTR pszString, COLORREF crTextColor, COLORREF crBackColor)
{
	//变量定义
	CHARFORMAT2 CharFormat;
	ZeroMemory(&CharFormat,sizeof(CharFormat));

	//构造数据
	CharFormat.cbSize=sizeof(CharFormat); 
	CharFormat.dwMask=CFM_COLOR|CFM_BACKCOLOR|CFM_OFFSET;
	CharFormat.crTextColor=crTextColor;
	CharFormat.crBackColor=crBackColor;

	//插入字串
	InsertString(pszString,CharFormat);

	return true;
}

//插入字串
bool CSkinRichEdit::InsertHyperLink(LPCTSTR pszString)
{
	//变量定义
	CHARFORMAT2 CharFormat;
	ZeroMemory(&CharFormat,sizeof(CharFormat));

	//构造数据
	CharFormat.cbSize=sizeof(CharFormat); 
	CharFormat.dwMask=CFM_LINK|CFM_OFFSET;
	CharFormat.dwEffects=CFE_LINK;

	//插入字串
	InsertString(pszString,CharFormat);

	return true;
}

//插入字串
bool CSkinRichEdit::InsertHyperLink(LPCTSTR pszString, COLORREF crBackColor)
{
	//变量定义
	CHARFORMAT2 CharFormat;
	ZeroMemory(&CharFormat,sizeof(CharFormat));

	//构造数据
	CharFormat.cbSize=sizeof(CharFormat); 
	CharFormat.dwMask=CFM_BACKCOLOR|CFM_LINK;
	CharFormat.dwEffects=CFE_LINK;
	CharFormat.crBackColor=crBackColor;

	//插入字串
	InsertString(pszString,CharFormat);

	return true;
}

//设置光标
BOOL CSkinRichEdit::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage)
{
	//设置光标
	if (nHitTest==HTCAPTION)
	{
		SetCursor(LoadCursor(NULL,MAKEINTRESOURCE(IDC_ARROW)));
		return TRUE;
	}

	return __super::OnSetCursor(pWnd,nHitTest,uMessage);
}

//插入字串
bool CSkinRichEdit::InsertString(LPCTSTR pszString, CHARFORMAT2 & CharFormat)
{
	//变量定义
	bool bResumeSelect;
	CHARRANGE CharRange;

	//保存状态
	GetSel(CharRange.cpMin,CharRange.cpMax);
	bResumeSelect=(CharRange.cpMax!=CharRange.cpMin);

	//搜索变量
	LPCTSTR pszHttp=TEXT("http://");
	const INT nHttpLength=lstrlen(pszHttp);
	const INT nStringLength=lstrlen(pszString);

	//索引定义
	INT nStringStart=0;
	INT nStringPause=0;

	//字符解释
	for (INT i=0;i<nStringLength;i++)
	{
		//变量定义
		INT nUrlPause=i;

		//地址判断
		if (((i+nHttpLength)<nStringLength)&&(memcmp(&pszString[i],pszHttp,nHttpLength*sizeof(TCHAR))==0))
		{
			//设置索引
			nUrlPause=i+nHttpLength;

			//地址搜索
			while (nUrlPause<nStringLength)
			{
				//字符判断
				if (EfficacyUrlChar(pszString[nUrlPause])==true)
				{
					nUrlPause++;
					continue;
				}

				break;
			}
		}

		//终止字符
		if (nUrlPause<=(i+nHttpLength)) nStringPause=(i+1);

		//插入字符
		if ((i==(nStringLength-1))||(nUrlPause>(i+nHttpLength)))
		{
			//普通字符
			if (nStringPause>nStringStart)
			{
				//获取缓冲
				CString strNormalString;
				LPTSTR pszNormalString=strNormalString.GetBuffer(nStringPause-nStringStart+1);

				//拷贝字符
				pszNormalString[nStringPause-nStringStart]=0;
				CopyMemory(pszNormalString,&pszString[nStringStart],(nStringPause-nStringStart)*sizeof(TCHAR));

				//释放缓冲
				strNormalString.ReleaseBuffer();

				//插入消息
				SetSel(-1L,-1L);
				SetWordCharFormat(CharFormat);
				ReplaceSel((LPCTSTR)strNormalString);
			}

			//连接地址
			if (nUrlPause>(i+nHttpLength))
			{
				//获取缓冲
				CString strUrlString;
				LPTSTR pszUrlString=strUrlString.GetBuffer((nUrlPause-i)+1);

				//拷贝字符
				pszUrlString[nUrlPause-i]=0;
				CopyMemory(pszUrlString,&pszString[i],(nUrlPause-i)*sizeof(TCHAR));

				//释放缓冲
				strUrlString.ReleaseBuffer();

				//构造格式
				CHARFORMAT2 CharFormatHyper;
				CharFormatHyper=CharFormat;

				//效验地址
				if (EfficacyUrlString(strUrlString)==true)
				{
					CharFormatHyper.dwMask|=CFM_LINK;
					CharFormatHyper.dwEffects|=CFE_LINK;
				}

				//插入消息
				SetSel(-1L,-1L);
				SetWordCharFormat(CharFormatHyper);
				ReplaceSel((LPCTSTR)strUrlString);
			}

			//设置索引
			nStringStart=__max(i,nUrlPause);
		}

		//设置索引
		i+=(nUrlPause-i);
	}

	//状态设置
	if (bResumeSelect==true) SetSel(CharRange);
	else PostMessage(WM_VSCROLL,SB_BOTTOM,0);

	return true;
}

//加载回调
DWORD CALLBACK CSkinRichEdit::LoadCallBack(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	//读取文件
	*pcb=((CFile *)(LONGLONG)(dwCookie))->Read(pbBuff,cb);

	return 0;
}

//保存回调
DWORD CALLBACK CSkinRichEdit::SaveCallBack(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	//写入文件
	((CFile *)(LONGLONG)(dwCookie))->Write(pbBuff,cb);

	return 0;
}

//字符判断
bool CSkinRichEdit::EfficacyUrlChar(TCHAR chChar)
{
	//特殊字符
	if (chChar==TEXT('.')) return true;
	if (chChar==TEXT('=')) return true;
	if (chChar==TEXT('+')) return true;
	if (chChar==TEXT('?')) return true;
	if (chChar==TEXT('#')) return true;
	if (chChar==TEXT('%')) return true;
	if (chChar==TEXT('/')) return true;
	if (chChar==TEXT(':')) return true;
	if (chChar==TEXT('&')) return true;

	//字符范围
	if ((chChar>=TEXT('a'))&&(chChar<=TEXT('z'))) return true;
	if ((chChar>=TEXT('A'))&&(chChar<=TEXT('Z'))) return true;
	if ((chChar>=TEXT('0'))&&(chChar<=TEXT('9'))) return true;

	return false;
}

//地址判断
bool CSkinRichEdit::EfficacyUrlString(LPCTSTR pszUrl)
{
	return true;
}

//销毁消息
VOID CSkinRichEdit::OnDestroy()
{
	__super::OnDestroy();

	//释放接口
	if (m_pIRichEditOLE!=NULL)
	{
		m_pIRichEditOLE->Release();
		m_pIRichEditOLE=NULL;
	}

	return;
}

//右键消息
VOID CSkinRichEdit::OnRButtonDown(UINT nFlags, CPoint Point)
{
	//设置焦点
	SetFocus();

	//构造菜单
	CSkinMenu FunctionMenu;
	FunctionMenu.CreateMenu();

	//建立菜单
	FunctionMenu.AppendMenu(IDM_COPY_STRING,TEXT("复制(&C)     Ctrl+C"));
	FunctionMenu.AppendMenu(IDM_SELECT_ALL,TEXT("全选(&A)     Ctrl+A"));
	FunctionMenu.AppendMenu(IDM_DELETE_STRING,TEXT("删除(&D)"));
	FunctionMenu.AppendMenu(IDM_CLEAR_ALL,TEXT("清除屏幕"));
	FunctionMenu.AppendSeparator();
	FunctionMenu.AppendMenu(IDM_SAVE_STRING,TEXT("保存信息..."));

	//禁止菜单
	UINT nEnableMask=((GetSelectionType()!=SEL_EMPTY)?0:MF_GRAYED);
	FunctionMenu.EnableMenuItem(IDM_COPY_STRING,MF_BYCOMMAND|nEnableMask);
	FunctionMenu.EnableMenuItem(IDM_DELETE_STRING,MF_BYCOMMAND|nEnableMask);

	//显示菜单
	ClientToScreen(&Point);
	FunctionMenu.TrackPopupMenu(Point.x,Point.y,this);

	return;
}

//删除信息
VOID CSkinRichEdit::OnClearAll()
{
	//删除信息
	SetSel(0L,-1L);
	ReplaceSel(NULL);

	return;
}

//全部选择
VOID CSkinRichEdit::OnSelectAll()
{
	//全部选择
	SetSel(0L,-1L);

	return;
}

//拷贝字符
VOID CSkinRichEdit::OnCopyString()
{
	//拷贝字符
	Copy();

	return;
}

//保存信息
VOID CSkinRichEdit::OnSaveString()
{
	//保存信息
	SaveMessage(NULL);

	return;
}

//删除字符
VOID CSkinRichEdit::OnDeleteString()
{
	//删除字符
	ReplaceSel(TEXT(""));

	return;
}

//////////////////////////////////////////////////////////////////////////

