#include "StdAfx.h"
#include "Resource.h"
#include "RichEditMessage.h"

//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CRichEditMessage, CSkinRichEdit)
	ON_NOTIFY_REFLECT(EN_LINK, OnEventLink)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////

//构造函数
CRichEditMessage::CRichEditMessage()
{
	//资源变量
	m_crFillColor=RGB(0,0,0);

	//设置变量
	m_pExpressionManager=NULL;
	m_pIRichEditMessageSink=NULL;
	m_dwMsgSoundID=0;

	return;
}

//析构函数
CRichEditMessage::~CRichEditMessage()
{
	//m_DirectSound.ResetDirectSound();
}

//接口查询
VOID * __cdecl CRichEditMessage::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IStringMessage,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(IStringMessage,Guid,dwQueryVer);
	return NULL;
}

//进入事件
bool __cdecl CRichEditMessage::InsertUserEnter(LPCTSTR pszUserName)
{
	//插入换行
	LONG lTextLength=GetWindowTextLength();
	if (lTextLength!=0L) InsertString(TEXT("\r\n"),COLOR_EVENT);

	//插入事件
	InsertUserAccounts(pszUserName);
	InsertString(TEXT("进来了"),COLOR_EVENT);

	return true;
}

//离开事件
bool __cdecl CRichEditMessage::InsertUserLeave(LPCTSTR pszUserName)
{
	//插入换行
	LONG lTextLength=GetWindowTextLength();
	if (lTextLength!=0L) InsertString(TEXT("\r\n"),COLOR_EVENT);

	//插入事件
	InsertUserAccounts(pszUserName);
	InsertString(TEXT("离开了"),COLOR_EVENT);

	return true;
}

//断线事件
bool __cdecl CRichEditMessage::InsertUserOffLine(LPCTSTR pszUserName)
{
	//插入换行
	LONG lTextLength=GetWindowTextLength();
	if (lTextLength!=0L) InsertString(TEXT("\r\n"),COLOR_EVENT);

	//插入事件
	InsertUserAccounts(pszUserName);
	InsertString(TEXT("断线了"),COLOR_WARN);

	return true;
}

//普通消息
bool __cdecl CRichEditMessage::InsertNormalString(LPCTSTR pszString)
{
	//插入换行
	LONG lTextLength=GetWindowTextLength();
	if (lTextLength!=0L) InsertString(TEXT("\r\n"),COLOR_EVENT);

	//插入消息
	InsertString(pszString,COLOR_NORMAL);

	return true;
}

//系统消息
bool __cdecl CRichEditMessage::InsertSystemString(LPCTSTR pszString)
{
	//插入换行
	LONG lTextLength=GetWindowTextLength();
	if (lTextLength!=0L) InsertString(TEXT("\r\n"),COLOR_EVENT);

	//加载图片
	if (m_ImageDataSystem.GetBitmap()==NULL)
	{
		HINSTANCE hInstance=GetModuleHandle(CLIENT_CONTROLS_DLL_NAME);
		LoadRichEditImage(hInstance,IDB_IMAGE_SYSTEM,m_ImageDataSystem);
	}

	//插入图片
	InsertDataObject(&m_ImageDataSystem);

	//变量定义
	CHARFORMAT2 CharFormat;
	ZeroMemory(&CharFormat,sizeof(CharFormat));

	//构造数据
	CharFormat.cbSize=sizeof(CharFormat); 
	CharFormat.dwMask=CFM_COLOR|CFM_OFFSET;
	CharFormat.crTextColor=COLOR_SYSTEM_STRING;
	CharFormat.yOffset=STRING_OFFSET;

	//插入消息
	InsertString(pszString,CharFormat);

	return true;
}

//提示消息
bool __cdecl CRichEditMessage::InsertPromptString(LPCTSTR pszString)
{
	//插入换行
	LONG lTextLength=GetWindowTextLength();
	if (lTextLength!=0L) InsertString(TEXT("\r\n"),COLOR_EVENT);

	//加载图片
	if (m_ImageDataPrompt.GetBitmap()==NULL)
	{
		HINSTANCE hInstance=GetModuleHandle(CLIENT_CONTROLS_DLL_NAME);
		LoadRichEditImage(hInstance,IDB_IMAGE_PROMPT,m_ImageDataPrompt);
	}

	//插入图片
	InsertDataObject(&m_ImageDataPrompt);

	//变量定义
	CHARFORMAT2 CharFormat;
	ZeroMemory(&CharFormat,sizeof(CharFormat));

	//构造数据
	CharFormat.cbSize=sizeof(CharFormat); 
	CharFormat.dwMask=CFM_COLOR|CFM_OFFSET;
	CharFormat.crTextColor=COLOR_SYSTEM_STRING;
	CharFormat.yOffset=STRING_OFFSET;

	//插入消息
	InsertString(pszString,CharFormat);

	return true;
}

//公告消息
bool __cdecl CRichEditMessage::InsertAfficheString(LPCTSTR pszString)
{
	//插入换行
	LONG lTextLength=GetWindowTextLength();
	if (lTextLength!=0L) InsertString(TEXT("\r\n"),COLOR_EVENT);

	//加载图片
	if (m_ImageDataAffiche.GetBitmap()==NULL)
	{
		HINSTANCE hInstance=GetModuleHandle(CLIENT_CONTROLS_DLL_NAME);
		LoadRichEditImage(hInstance,IDB_IMAGE_AFFICHE,m_ImageDataAffiche);
	}
	
	//插入图片
	InsertDataObject(&m_ImageDataAffiche);

	//变量定义
	CHARFORMAT2 CharFormat;
	ZeroMemory(&CharFormat,sizeof(CharFormat));

	//构造数据
	CharFormat.cbSize=sizeof(CharFormat); 
	CharFormat.dwMask=CFM_COLOR|CFM_OFFSET;
	CharFormat.crTextColor=COLOR_SYSTEM_STRING;
	CharFormat.yOffset=STRING_OFFSET;

	//插入消息
	InsertString(pszString,CharFormat);

	return true;
}

//自定消息
bool __cdecl CRichEditMessage::InsertCustomString(LPCTSTR pszString, COLORREF crColor)
{
	//插入换行
	LONG lTextLength=GetWindowTextLength();
	if (lTextLength!=0L) InsertString(TEXT("\r\n"),COLOR_EVENT);

	//插入消息
	InsertString(pszString,crColor);

	return true;
}

//定制消息
bool __cdecl CRichEditMessage::InsertCustomString(LPCTSTR pszString, COLORREF crColor, COLORREF crBackColor)
{
	//插入换行
	LONG lTextLength=GetWindowTextLength();
	if (lTextLength!=0L) InsertString(TEXT("\r\n"),COLOR_EVENT);

	//插入消息
	InsertString(pszString,crColor,crBackColor);

	return true;
}

//用户聊天
bool __cdecl CRichEditMessage::InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor)
{
	//插入换行
	LONG lTextLength=GetWindowTextLength();
	if (lTextLength!=0L) InsertString(TEXT("\r\n"),COLOR_EVENT);

	//插入消息
	InsertUserAccounts(pszSendUser);
	InsertString(TEXT("说："),COLOR_EVENT);
	TranslateInsertString(pszString,crColor);

	return true;
}

//用户聊天
bool __cdecl CRichEditMessage::InsertUserChat(LPCTSTR pszSendUser, LPCTSTR pszRecvUser, LPCTSTR pszString, COLORREF crColor,bool bSoundNotify)
{
	//插入换行
	LONG lTextLength=GetWindowTextLength();
	if (lTextLength!=0L) InsertString(TEXT("\r\n"),COLOR_EVENT);

	//插入消息
	InsertUserAccounts(pszSendUser);
	InsertString(TEXT("对"),COLOR_EVENT);
	InsertUserAccounts(pszRecvUser);
	InsertString(TEXT("说："),COLOR_EVENT);
	TranslateInsertString(pszString,crColor);

	//if(bSoundNotify)
	//	m_DirectSound.Play(m_dwMsgSoundID);
	return true;
}

//用户私聊
bool __cdecl CRichEditMessage::InsertUserWhisper(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, bool bMyselfString)
{
	//获取时间
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	//保存选择
	CHARRANGE CharRange;
	GetSel(CharRange);

	//插入前叠
	TCHAR szTimeString[64]=TEXT("");
	_sntprintf_s(szTimeString,CountArray(szTimeString),TEXT("\r\n%s [ %02d:%02d:%02d ]\r\n"),pszSendUser,SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);
	InsertString(szTimeString,(bMyselfString==true)?COLOR_MYSELF:COLOR_USER);

	//格式数据
	PARAFORMAT2 ParaFormat;
	ZeroMemory(&ParaFormat,sizeof(ParaFormat));
	ParaFormat.cbSize=sizeof(ParaFormat);
	ParaFormat.dwMask=PFM_OFFSETINDENT;
	ParaFormat.dxStartIndent=200L;

	//设置缩进
	SetSel(-1L,-1L);
	SetParaFormat(ParaFormat);

	//插入语句
	TranslateInsertString(pszString,crColor);
	InsertString(TEXT("\r\n"),crColor);

	//格式数据
	ParaFormat.dwMask=PFM_OFFSETINDENT;
	ParaFormat.dxStartIndent=-200L;

	//还原格式
	SetSel(-1L,-1L);
	SetParaFormat(ParaFormat);

	//恢复信息
	if (CharRange.cpMax!=CharRange.cpMin) SetSel(CharRange);
	else PostMessage(WM_VSCROLL,SB_BOTTOM,0);

	//if(!bMyselfString)
	//	m_DirectSound.Play(m_dwMsgSoundID);
	return true;
}

//用户私聊
bool __cdecl CRichEditMessage::InsertUserWhisper(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, bool bMyselfString, const SYSTEMTIME &time)
{
	//保存选择
	CHARRANGE CharRange;
	GetSel(CharRange);

	//插入前叠
	TCHAR szTimeString[64]=TEXT("");
	_sntprintf_s(szTimeString,CountArray(szTimeString),TEXT("\r\n%s [ %02d:%02d:%02d ]\r\n"),pszSendUser,time.wHour,time.wMinute,time.wSecond);
	InsertString(szTimeString,(bMyselfString==true)?COLOR_MYSELF:COLOR_USER);

	//格式数据
	PARAFORMAT2 ParaFormat;
	ZeroMemory(&ParaFormat,sizeof(ParaFormat));
	ParaFormat.cbSize=sizeof(ParaFormat);
	ParaFormat.dwMask=PFM_OFFSETINDENT;
	ParaFormat.dxStartIndent=200L;

	//设置缩进
	SetSel(-1L,-1L);
	SetParaFormat(ParaFormat);

	//插入语句
	TranslateInsertString(pszString,crColor);
	InsertString(TEXT("\r\n"),crColor);

	//格式数据
	ParaFormat.dwMask=PFM_OFFSETINDENT;
	ParaFormat.dxStartIndent=-200L;

	//还原格式
	SetSel(-1L,-1L);
	SetParaFormat(ParaFormat);

	//恢复信息
	if (CharRange.cpMax!=CharRange.cpMin) SetSel(CharRange);
	else PostMessage(WM_VSCROLL,SB_BOTTOM,0);

	//if(!bMyselfString)
	//	m_DirectSound.Play(m_dwMsgSoundID);
	return true;
}

//控件绑定
VOID CRichEditMessage::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//设置滚动
	m_SkinScrollBar.InitScroolBar(this);

	//设置控件
	SetEventMask(ENM_LINK);
	SetAutoURLDetect(FALSE);
	SetOptions(ECOOP_OR,ECO_AUTOWORDSELECTION);

	/*if(m_dwMsgSoundID==0)
	{
		m_DirectSound.InitDirectSound(this);
		TCHAR szPath[MAX_PATH]=TEXT("\0");
		GetCurrentDirectory(MAX_PATH, szPath);
		TCHAR szSoundPath[MAX_PATH]=TEXT("\0");
		_sntprintf_s(szSoundPath,MAX_PATH,TEXT("%s\\sound\\msg.wav"),szPath);
		DWORD dwFlags = DSBCAPS_GLOBALFOCUS;
		m_dwMsgSoundID=m_DirectSound.CreateFromFile(szSoundPath,dwFlags);
		ASSERT(m_dwMsgSoundID!=0);
	}*/

	return;
}

//设置接口
bool CRichEditMessage::SetRichEditMessageSink(IUnknownEx * pIUnknownEx)
{
	//设置接口
	if (pIUnknownEx!=NULL)
	{
		//查询接口
		ASSERT(QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IRichEditMessageSink)!=NULL);
		m_pIRichEditMessageSink=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,IRichEditMessageSink);

		//成功判断
		if (m_pIRichEditMessageSink==NULL) return false;
	}
	else m_pIRichEditMessageSink=NULL;

	return true;
}

//设置组件
bool CRichEditMessage::SetExpressionManager(CExpressionManager * pExpressionManager, COLORREF crFillColor)
{
	//设置变量
	m_crFillColor=crFillColor;
	m_pExpressionManager=pExpressionManager;

	return true;
}

//插入时间
bool CRichEditMessage::InsertSystemTime(COLORREF crColor)
{
	//获取时间
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	//构造消息
	TCHAR szTimeBuffer[64]=TEXT("");
	_sntprintf_s(szTimeBuffer,CountArray(szTimeBuffer),TEXT("%02d:%02d:%02d"),SystemTime.wHour,SystemTime.wMinute,SystemTime.wSecond);

	//插入消息
	InsertString(szTimeBuffer,crColor);

	return true;
}

//用户名字
bool CRichEditMessage::InsertUserAccounts(LPCTSTR pszAccounts)
{
	//效验参数
	ASSERT(pszAccounts!=NULL);
	if (pszAccounts==NULL) return false;

	//插入名字
	InsertString(TEXT("［"),COLOR_NORMAL);
	InsertHyperLink(pszAccounts);
	InsertString(TEXT("］"),COLOR_NORMAL);

	return true;
}

//翻译字符
bool CRichEditMessage::TranslateInsertString(LPCTSTR pszString, COLORREF crColor)
{
	//翻译判断
	if (m_pExpressionManager==NULL) return false;

	//变量定义
	tagTranslateResult TranslateResult;
	ZeroMemory(&TranslateResult,sizeof(TranslateResult));

	//字符变量
	INT nTranslateIndex=0;
	INT nStringLength=lstrlen(pszString);

	while ((nStringLength-nTranslateIndex)>0)
	{
		//解释字符
		bool bSuccess=m_pExpressionManager->TranslateString(&pszString[nTranslateIndex],TranslateResult);

		//结果处理
		if (bSuccess==true && TranslateResult.nDescribeLen==nStringLength)
		{
			//插入字符
			if (TranslateResult.nStartPos>0)
			{
				//变量定义
				TCHAR szString[LEN_USER_CHAT]=TEXT("");
				WORD wCopyCount=__min(TranslateResult.nStartPos,CountArray(szString));

				//构造字符
				szString[wCopyCount]=0;
				CopyMemory(szString,&pszString[nTranslateIndex],wCopyCount*sizeof(TCHAR));

				//插入字符
				InsertString(szString,crColor);
			}

			//构造路径
			TCHAR ImagePath[MAX_PATH]=TEXT("");
			TranslateResult.pExpressionItem->GetExpressionPath(ImagePath,CountArray(ImagePath));

			//插入表情
			InsertImage(ImagePath);

			//设置索引
			nTranslateIndex+=TranslateResult.nStartPos+TranslateResult.nDescribeLen;
		}
		else
		{
			//插入字符
			InsertString(&pszString[nTranslateIndex],crColor);
			break;
		}

	}

	return true;
}

//翻译字符
bool CRichEditMessage::TranslateInsertString(LPCTSTR pszString, CHARFORMAT2 &CharFormat)
{
	//翻译判断
	if (m_pExpressionManager==NULL) return false;

	//变量定义
	tagTranslateResult TranslateResult;
	ZeroMemory(&TranslateResult,sizeof(TranslateResult));

	//字符变量
	INT nTranslateIndex=0;
	INT nStringLength=lstrlen(pszString);

	CString strTemp(pszString,nStringLength);
	CString strReplace(strTemp);
	strTemp.MakeLower();
	int nFindIndx=0,nFindCount=0;
	while((nFindIndx=strTemp.Find(TEXT("http://"),nFindIndx+nFindCount*7))!=-1)
	{
		strReplace.Delete(nFindIndx-nFindCount*7,7);
		nFindCount++;
	}
	LPCTSTR pszString1=strReplace;

	while ((nStringLength-nTranslateIndex)>0)
	{
		//解释字符
		bool bSuccess=m_pExpressionManager->TranslateString(&pszString1[nTranslateIndex],TranslateResult);

		//结果处理
		if (bSuccess==true)
		{
			//插入字符
			if (TranslateResult.nStartPos>0)
			{
				//变量定义
				TCHAR szString[LEN_USER_CHAT]=TEXT("");
				WORD wCopyCount=__min(TranslateResult.nStartPos,CountArray(szString));

				//构造字符
				szString[wCopyCount]=0;
				CopyMemory(szString,&pszString1[nTranslateIndex],wCopyCount*sizeof(TCHAR));

				//插入字符
				InsertString(szString,CharFormat);
			}

			//构造路径
			TCHAR ImagePath[MAX_PATH]=TEXT("");
			TranslateResult.pExpressionItem->GetExpressionPath(ImagePath,CountArray(ImagePath));

			//插入表情
			InsertImage(ImagePath);

			//设置索引
			nTranslateIndex+=TranslateResult.nStartPos+TranslateResult.nDescribeLen;
		}
		else
		{
			//插入字符
			InsertString(&pszString1[nTranslateIndex],CharFormat);
			break;
		}

	}

	return true;
}
//处理图片
bool CRichEditMessage::LoadRichEditImage(HINSTANCE hInstance, UINT uResourceID, CImageDataObject & ImageDataObject)
{
	//加载图片
	CBitImage ImageFile;
	ImageFile.LoadFromResource(hInstance,uResourceID);

	//失败判断
	if (ImageFile.IsNull()==true)
	{
		ASSERT(FALSE);
		return false;
	}

	//创建表情
	CImage ImageRichEdit;
	ImageRichEdit.Create(ImageFile.GetWidth(),ImageFile.GetHeight(),32);

	//绘画表情
	CDC * pDC=CDC::FromHandle(ImageRichEdit.GetDC());
	pDC->FillSolidRect(0,0,ImageFile.GetWidth(),ImageFile.GetHeight(),m_crFillColor);
	ImageFile.AlphaDrawImage(pDC,0,0,RGB(255,0,255));

	//设置图片
	ImageRichEdit.ReleaseDC();
	ImageDataObject.SetImage(ImageRichEdit.Detach());

	return true;
}

//按下连接
VOID CRichEditMessage::OnEventLink(NMHDR * pNMHDR, LRESULT * pResult)
{
	//变量定义
	ENLINK * pEnLink=reinterpret_cast<ENLINK *>(pNMHDR);

	//点击消息
	if ((m_pIRichEditMessageSink!=NULL)&&((pEnLink->msg==WM_LBUTTONDOWN)||(pEnLink->msg==WM_LBUTTONDBLCLK)))
	{
		//变量定义
		CString strString;
		TEXTRANGE TextRange;

		//长度计算
		LONG lLastIndex=pEnLink->chrg.cpMin;
		LONG lFirstIndex=pEnLink->chrg.cpMax;

		//构造变量
		TextRange.chrg.cpMin=pEnLink->chrg.cpMin;
		TextRange.chrg.cpMax=pEnLink->chrg.cpMax;
		TextRange.lpstrText=strString.GetBuffer((lFirstIndex-lLastIndex+1)*2L);

		//获取字符
		LONG lLength=(LONG)SendMessage(EM_GETTEXTRANGE,0,(LPARAM)&TextRange);

		//设置长度
		strString.ReleaseBuffer(lLength);

		//事件通知
		CA2CT strTextRange((LPCSTR)(LPCTSTR)strString);
		if(!m_pIRichEditMessageSink->OnEventStringLink((LPCTSTR)strTextRange,pEnLink->msg))
		{
			CString str(strTextRange);
			CString strHttp("http://");
			str.MakeLower();
			
			if(_tcsncmp(str.Left(7),strHttp.Left(7),7)==0)
				ShellExecute(NULL,TEXT("open"),str,NULL,NULL,SW_SHOWDEFAULT);
		}
	}

	//设置变量
	*pResult=0;

	return;
}

//喇叭消息
bool __cdecl CRichEditMessage::InserBugleString(LPCTSTR pszSendUser, LPCTSTR pszString, COLORREF crColor, BYTE cbBugleType)
{
	////插入换行
	//LONG lTextLength=GetWindowTextLength();
	//if (lTextLength!=0L) InsertString(TEXT("\r\n"),COLOR_EVENT);

	////加载图片
	//if(cbBugleType==BUGLE_TYPE_ROOM)
	//{
	//	if (m_ImageSmallBugle.GetBitmap()==NULL)
	//	{
	//		HINSTANCE hInstance=GetModuleHandle(CLIENT_CONTROLS_DLL_NAME);
	//		LoadRichEditImage(hInstance,IDB_IMAGE_SMALL_BUGLE,m_ImageSmallBugle);
	//	}
	//	//插入图片
	//	InsertDataObject(&m_ImageSmallBugle);
	//}
	//else if(cbBugleType==BUGLE_TYPE_GAME)
	//{
	//	if (m_ImageBigBugle.GetBitmap()==NULL)
	//	{
	//		HINSTANCE hInstance=GetModuleHandle(CLIENT_CONTROLS_DLL_NAME);
	//		LoadRichEditImage(hInstance,IDB_IMAGE_BIG_BUGLE,m_ImageBigBugle);
	//	}
	//	//插入图片
	//	InsertDataObject(&m_ImageBigBugle);
	//}

	////变量定义
	//CHARFORMAT2 CharFormat,CharFormat1;
	//ZeroMemory(&CharFormat,sizeof(CharFormat));

	////构造数据
	//CharFormat.cbSize=sizeof(CharFormat); 
	//CharFormat.dwMask=CFM_COLOR|CFM_OFFSET;
	//CharFormat.crTextColor=crColor;
	//CharFormat.yOffset=STRING_OFFSET;

	////插入名字
	//InsertString(TEXT("【"),CharFormat);
	//
	//ZeroMemory(&CharFormat1,sizeof(CharFormat1));

	////构造数据
	//CharFormat1.cbSize=sizeof(CharFormat); 
	//CharFormat1.dwMask=CFM_LINK|CFM_OFFSET;
	//CharFormat1.dwEffects=CFE_LINK;
	//CharFormat1.yOffset=STRING_OFFSET;

	////插入字串
	//InsertString(pszSendUser,CharFormat1);


	//InsertString(TEXT("】"),CharFormat);

	//InsertString(TEXT("说："),CharFormat);
	////插入消息
	//InsertString(pszString,CharFormat);
	return true;
}
//////////////////////////////////////////////////////////////////////////
