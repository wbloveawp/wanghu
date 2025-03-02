// ProcessException.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "ProcessException.h"
#include <Tlhelp32.h>  
#include <dbghelp.h>


//构造函数
CExceptionHandle::CExceptionHandle() : CDialog(IDD_DLG_EXCEPTION)
{
}

//析构函数
CExceptionHandle::~CExceptionHandle()
{
}

void CExceptionHandle::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT21, m_RichEditCtrl);
}


BEGIN_MESSAGE_MAP(CExceptionHandle, CDialog)
	ON_BN_CLICKED(IDC_BUTTON2, &CExceptionHandle::OnBnClickedSendReport)
	ON_BN_CLICKED(IDC_BUTTON1, &CExceptionHandle::OnBnClickedDebug)
END_MESSAGE_MAP()


// CExceptionHandle 消息处理程序


BOOL CExceptionHandle::OnInitDialog()
{
	__super::OnInitDialog();

	AfxSetResourceHandle(GetModuleHandle(NULL));
	//设置图标
	HICON hIcon = LoadIcon(GetModuleHandle(KERNEL_ENGINE_DLL_NAME), MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	//保存异常信息
	SaveExceptionInfo(m_lpExceptionInfo);

	//显示异常信息
	TCHAR szFileName[_MAX_PATH] = TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(), szFileName, _MAX_PATH);
	InsertString(_T("应用程序:"));
	InsertString(szFileName);
	InsertString(TEXT("\r\n"));

	//异常信息
	TCHAR szDest[250] = TEXT("");
	_sntprintf(szDest, CountArray(szDest), TEXT("异常地址:%ld"), m_lpExceptionInfo->ExceptionRecord->ExceptionAddress);
	InsertString(szDest);

	return TRUE; 
}


BOOL CExceptionHandle::PreTranslateMessage(MSG* pMsg)
{
	//按键过滤
	if ((pMsg->message == WM_KEYDOWN) || (pMsg->wParam == VK_ESCAPE) && pMsg->wParam == VK_RETURN)
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}


void CExceptionHandle::OnBnClickedSendReport()
{
	// TODO: 在此添加控件通知处理程序代码
	//这里可以将异常发送至服务器， 暂时未实现
	EndDialog(-1);
	
}


void CExceptionHandle::OnBnClickedDebug()
{
	// TODO: 在此添加控件通知处理程序代码
	EndDialog(IDABORT);
}

VOID CExceptionHandle::InsertString(LPCTSTR pszString)
{
	m_RichEditCtrl.SetSel(-1, -1);
	m_RichEditCtrl.ReplaceSel(pszString);
}
//保存文件
VOID CExceptionHandle::SaveExceptionInfo(EXCEPTION_POINTERS * lpExceptionInfo)
{
	//获取当前工作路径
	TCHAR szPath[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szPath, MAX_PATH);

	//保存目录
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	_sntprintf(szDirectory, MAX_PATH, TEXT("%s\\BugReport"), &szPath);
	//保存文件
	m_strFileName.Format(TEXT("%s\\%ld%ld.DMP"), szDirectory, GetTickCount(), time(NULL));
	//创建目录
	CreateDirectory(szDirectory, NULL);
	//将异常写入文件
	CFile hFile;
	//打开文件
	if (hFile.Open(m_strFileName, CFile::modeCreate | CFile::modeWrite, NULL))
	{
		//写入异常信息
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;
		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = lpExceptionInfo;
		ExInfo.ClientPointers = NULL;
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, 0, 0);
	}
}


CExceptionManager *CExceptionManager::m_pExceptionManager = new CExceptionManager();

//构造函数
CExceptionManager::CExceptionManager()
{
	if (!CExceptionManager::m_pExceptionManager)
	{
		CExceptionManager::m_pExceptionManager = this;
	}

	::SetUnhandledExceptionFilter(CExceptionManager::ProcessExceptionFilter);
}
//析构函数
CExceptionManager::~CExceptionManager()
{

}

LONG CExceptionManager::ProcessExceptionFilter(EXCEPTION_POINTERS * lpExceptionInfo)
{
	AfxSetResourceHandle(GetModuleHandle(KERNEL_ENGINE_DLL_NAME));
	//异常处理对话框
	CExceptionHandle exceptionHandle;
	//设置异常信息
	exceptionHandle.m_lpExceptionInfo = lpExceptionInfo;
	//模态对话框
	int nRet = exceptionHandle.DoModal();
	//点击调试按钮
	if (nRet == IDABORT)
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
	else  //发送报告
	{
		return EXCEPTION_EXECUTE_HANDLER;
	}
}
