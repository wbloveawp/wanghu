#include "StdAfx.h"
#include "Resource.h"
#include "ProcessException.h"
#include "Assistant.h"
#include <DbgHelp.h>
#include "SendBugReport.h"

#pragma warning (disable : 4996)

//////////////////////////////////////////////////////////////////////////////////

//静态变量
CExceptionManager *	CExceptionManager::m_pExceptionManager=NULL;		//异常对象

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CExceptionHandle, CDialog)
	ON_BN_CLICKED(IDC_DEBUG, OnBnClickedDebug)
	ON_BN_CLICKED(IDC_SEND_REPORT, OnBnClickedSendReport)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CExceptionHandle::CExceptionHandle() : CDialog(IDD_EXCEPTION)
{
	//设置变量
	m_lpExceptionInfo=NULL;

	return;
}

//析构函数
CExceptionHandle::~CExceptionHandle()
{
}

//控件绑定
VOID CExceptionHandle::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT, m_RichEditCtrl);
}

//消息解释
BOOL CExceptionHandle::PreTranslateMessage(MSG * pMsg)
{
	//按键过滤
	if (pMsg->message==WM_KEYDOWN)
	{
		if (pMsg->wParam==VK_ESCAPE) return TRUE;
		if (pMsg->wParam==VK_RETURN) return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//初始化函数
BOOL CExceptionHandle::OnInitDialog()
{
	__super::OnInitDialog();

	//设置句柄
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//设置图标
	HICON hIcon=LoadIcon(GetModuleHandle(COMMON_SERVICE_DLL_NAME),MAKEINTRESOURCE(IDI_ICON));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//保存信息
	SaveExceptionInfo(m_lpExceptionInfo);

	//模块名字
	TCHAR szModuleName[MAX_PATH]=TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(),szModuleName,CountArray(szModuleName)); 

	InsertString(TEXT("模块名字："));
	InsertString(szModuleName);
	InsertString(TEXT("\r\n"));

	//错误信息
	TCHAR szExceptionInfo[512]=TEXT("");
	_sntprintf(szExceptionInfo,CountArray(szExceptionInfo),TEXT("%ld"),(DWORD_PTR)(m_lpExceptionInfo->ExceptionRecord->ExceptionAddress));
	InsertString(szExceptionInfo);

	return TRUE;
}

//调试按钮
VOID CExceptionHandle::OnBnClickedDebug()
{
	//关闭窗口
	EndDialog(IDC_DEBUG);

	return;
}

//发送报告
VOID CExceptionHandle::OnBnClickedSendReport()
{
	CSendBugReport BugReport;
	AfxGetMainWnd()->ShowWindow(SW_HIDE);
	ShowWindow(SW_HIDE);
	BugReport.SendMail(TEXT("smtp.qq.com" ),	
		TEXT("BugReport"),
		TEXT("53910068a"),
		TEXT("BugReport"),
		TEXT("BugReport@qq.com"),
		TEXT("BugReport"),
		TEXT("BugReport@qq.com"),
		TEXT(""), 
		TEXT("BugReport"),
		m_strFileName); 
	OnOK();
	return;
}

//插入字串 
VOID CExceptionHandle::InsertString(LPCTSTR pszString)
{
	//插入消息
	m_RichEditCtrl.SetSel(-1L,-1L);
	m_RichEditCtrl.ReplaceSel((LPCTSTR)(pszString));

	return;
}

//保存文件
VOID CExceptionHandle::SaveExceptionInfo(EXCEPTION_POINTERS * lpExceptionInfo)
{
	//系统目录
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CAssistant::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//构造名字
	TCHAR szReprotDirectory[MAX_PATH]=TEXT("");
	_sntprintf(szReprotDirectory,CountArray(szReprotDirectory),TEXT("%s\\BugReport"),szDirectory);
	m_strFileName.Format(TEXT("%s\\%ld%ld.DMP"),szReprotDirectory,(DWORD)time(NULL),GetTickCount());

	//创建目录
	CreateDirectory(szReprotDirectory,NULL);

	//创建文件
	CFile ExceptionFile;
	if (ExceptionFile.Open(m_strFileName,CFile::modeCreate|CFile::modeWrite)==FALSE)
	{
		ASSERT(FALSE);
		return;
	}

	//变量定义
	MINIDUMP_EXCEPTION_INFORMATION MiniDumpExceptionInfo;
	ZeroMemory(&MiniDumpExceptionInfo,sizeof(MiniDumpExceptionInfo));

	//设置变量
	MiniDumpExceptionInfo.ClientPointers=FALSE;
	MiniDumpExceptionInfo.ThreadId=GetCurrentThreadId();
	MiniDumpExceptionInfo.ExceptionPointers=lpExceptionInfo;

	//写入信息
	if (MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),ExceptionFile,MiniDumpNormal,&MiniDumpExceptionInfo,NULL,NULL)==FALSE)
	{
		ASSERT(FALSE);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CExceptionManager::CExceptionManager()
{
	//设置对象
	ASSERT(m_pExceptionManager==NULL);
	if (m_pExceptionManager==NULL) m_pExceptionManager=this;

	//设置异常
	SetUnhandledExceptionFilter(ProcessExceptionFilter);

	return;
}

//析构函数
CExceptionManager::~CExceptionManager()
{
	//设置对象
	ASSERT(m_pExceptionManager==this);
	if (m_pExceptionManager==this) m_pExceptionManager=NULL;

	return;
}

//异常函数
LONG WINAPI CExceptionManager::ProcessExceptionFilter(EXCEPTION_POINTERS * lpExceptionInfo)
{
	//设置句柄
	AfxSetResourceHandle(GetModuleHandle(COMMON_SERVICE_DLL_NAME));

	//变量定义
	CExceptionHandle ExceptionHandle;
	ExceptionHandle.m_lpExceptionInfo=lpExceptionInfo;

	//显示窗口
	if (ExceptionHandle.DoModal()==IDC_DEBUG)
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

//////////////////////////////////////////////////////////////////////////////////

//异常对象
CExceptionManager g_ExceptionManager;

//////////////////////////////////////////////////////////////////////////////////
