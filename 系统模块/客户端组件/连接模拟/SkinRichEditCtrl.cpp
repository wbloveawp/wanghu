#include "StdAfx.h"
#include "SkinRichEditCtrl.h"

//////////////////////////////////////////////////////////////////////////////////

//信息颜色
#define COLOR_INFO					RGB(64,0,0)							//信息颜色
#define COLOR_BACK_GROUND			RGB(230,230,230)					//背景颜色

//菜单命令
#define IDM_CLEAR_ALL				(WM_USER+201)						//删除信息
#define IDM_SELECT_ALL				(WM_USER+202)						//全部选择
#define IDM_COPY_STRING				(WM_USER+203)						//拷贝字符
#define IDM_SAVE_STRING				(WM_USER+205)						//保存信息
#define IDM_DELETE_STRING			(WM_USER+204)						//删除字符

//常量定义
#define IDT_TRACE_STRING			1									//追踪事件

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSkinRichEditCtrl, CRichEditCtrl)

	//系统消息
	ON_WM_CREATE()
	ON_WM_RBUTTONDOWN()

	//菜单消息
	ON_COMMAND(IDM_CLEAR_ALL, OnClearAll)
	ON_COMMAND(IDM_SELECT_ALL, OnSelectAll)
	ON_COMMAND(IDM_COPY_STRING, OnCopyString)
	ON_COMMAND(IDM_SAVE_STRING, OnSaveString)
	ON_COMMAND(IDM_DELETE_STRING, OnDeleteString)

END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CSkinRichEditCtrl::CSkinRichEditCtrl()
{
	//设置变量
	m_lMaxLineCount = 1024;
	m_lReserveLineCount = 512;

	//线程信息
	m_dwMainThreadID = 0L;
	m_hMainThreadHandle = NULL;

	return;
}

//析构函数
CSkinRichEditCtrl::~CSkinRichEditCtrl()
{
}

//绑定函数
VOID CSkinRichEditCtrl::PreSubclassWindow()
{
	__super::PreSubclassWindow();

	//配置控件
	InitializeService();

	return;
}

//加载消息
bool CSkinRichEditCtrl::LoadMessage(LPCTSTR pszFileName)
{
	//效验参数
	ASSERT(pszFileName != NULL);
	if (pszFileName == NULL) return false;

	//打开文件
	CFile File;
	if (File.Open(pszFileName, CFile::modeRead, NULL) == FALSE) return false;

	//构造数据
	EDITSTREAM EditStream;
	EditStream.pfnCallback = LoadCallBack;
	EditStream.dwCookie = (DWORD)(LONGLONG)(&File);

	//读取文件
	StreamIn(SF_RTF, EditStream);

	return true;
}

//保存消息
bool CSkinRichEditCtrl::SaveMessage(LPCTSTR pszFileName)
{
	//变量定义
	CString strFileName = pszFileName;

	//打开文件
	if (strFileName.IsEmpty() == true)
	{
		CFileDialog FileDlg(FALSE, TEXT("RTF"), 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, TEXT("信息文件(*.RTF)|*.RTF||"), this);
		if (FileDlg.DoModal() == IDOK) strFileName = FileDlg.GetPathName();
		else return false;
	}

	//打开文件
	CFile File;
	if (File.Open(strFileName, CFile::modeWrite | CFile::modeCreate, NULL) == FALSE) return false;

	//构造数据
	EDITSTREAM EditStream;
	EditStream.pfnCallback = SaveCallBack;
	EditStream.dwCookie = (DWORD)(LONGLONG)(&File);

	//写入文件
	StreamOut(SF_RTF, EditStream);

	return true;
}

//设置参数
bool CSkinRichEditCtrl::SetParameter(LONG lMaxLineCount, LONG lReserveLineCount)
{
	//效验参数
	ASSERT((m_lMaxLineCount == 0) || (m_lMaxLineCount > m_lReserveLineCount));
	if ((m_lMaxLineCount != 0) && (m_lMaxLineCount <= m_lReserveLineCount)) return false;

	//设置变量
	m_lMaxLineCount = lMaxLineCount;
	m_lReserveLineCount = lReserveLineCount;

	return true;
}

//配置服务
VOID CSkinRichEditCtrl::InitializeService()
{
	//清空队列
	m_DataQueue.RemoveData(false);

	//线程句柄
	m_hMainThreadHandle = GetCurrentThread();
	m_dwMainThreadID = GetThreadId(m_hMainThreadHandle);

	//设置控件
	SetBackgroundColor(FALSE, COLOR_BACK_GROUND);

	return;
}

//插入字串
VOID CSkinRichEditCtrl::InsertString(LPCTSTR pszString, CHARFORMAT2 & CharFormat)
{
	//保存信息
	CHARRANGE CharRange;
	GetSel(CharRange);

	//变量定义
	LONG lTextLength = GetTextLengthEx(GTL_NUMCHARS);

	//插入消息
	SetSel(lTextLength, lTextLength);
	SetSelectionCharFormat(CharFormat);
	ReplaceSel(pszString);

	//滚动字符
	SendMessage(WM_VSCROLL, SB_BOTTOM, 0);

	return;
}

//加载回调
DWORD CALLBACK CSkinRichEditCtrl::LoadCallBack(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	//读取文件
	*pcb = ((CFile *)(ULONGLONG)(dwCookie))->Read(pbBuff, cb);

	return 0;
}

//保存回调
DWORD CALLBACK CSkinRichEditCtrl::SaveCallBack(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG * pcb)
{
	//写入文件
	((CFile *)(ULONGLONG)(dwCookie))->Write(pbBuff, cb);

	return 0;
}

//删除信息
VOID CSkinRichEditCtrl::OnClearAll()
{
	//删除信息
	SetSel(0L, -1L);
	ReplaceSel(NULL);

	return;
}

//全部选择
VOID CSkinRichEditCtrl::OnSelectAll()
{
	//全部选择
	SetSel(0L, -1L);

	return;
}

//拷贝字符
VOID CSkinRichEditCtrl::OnCopyString()
{
	//拷贝字符
	Copy();

	return;
}

//保存信息
VOID CSkinRichEditCtrl::OnSaveString()
{
	//保存信息
	SaveMessage(NULL);

	return;
}

//删除字符
VOID CSkinRichEditCtrl::OnDeleteString()
{
	//删除字符
	ReplaceSel(TEXT(""));

	return;
}

//建立消息
INT CSkinRichEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1) return -1;

	//配置控件
	InitializeService();

	return 0;
}

//右键消息
VOID CSkinRichEditCtrl::OnRButtonDown(UINT nFlags, CPoint Point)
{
	//设置焦点
	SetFocus();

	//构造菜单
	CMenu FunctionMenu;
	FunctionMenu.CreatePopupMenu();

	//建立菜单
	FunctionMenu.AppendMenu(MF_STRING, IDM_COPY_STRING, TEXT("复制(&C)     Ctrl+C"));
	FunctionMenu.AppendMenu(MF_STRING, IDM_SELECT_ALL, TEXT("全选(&A)     Ctrl+A"));
	FunctionMenu.AppendMenu(MF_STRING, IDM_DELETE_STRING, TEXT("删除(&D)"));
	FunctionMenu.AppendMenu(MF_STRING, IDM_CLEAR_ALL, TEXT("清除信息"));
	FunctionMenu.AppendMenu(MF_SEPARATOR);
	FunctionMenu.AppendMenu(MF_STRING, IDM_SAVE_STRING, TEXT("保存信息..."));

	//禁止菜单
	UINT nEnableMask = ((GetSelectionType() != SEL_EMPTY) ? 0 : MF_GRAYED);
	FunctionMenu.EnableMenuItem(IDM_COPY_STRING, MF_BYCOMMAND | nEnableMask);
	FunctionMenu.EnableMenuItem(IDM_DELETE_STRING, MF_BYCOMMAND | nEnableMask);

	//显示菜单
	ClientToScreen(&Point);
	FunctionMenu.TrackPopupMenu(TPM_RIGHTBUTTON, Point.x, Point.y, this);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
