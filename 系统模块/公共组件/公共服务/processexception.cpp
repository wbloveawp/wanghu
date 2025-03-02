#include "StdAfx.h"
#include "Resource.h"
#include "ProcessException.h"
#include "Assistant.h"
#include <DbgHelp.h>
#include "SendBugReport.h"

#pragma warning (disable : 4996)

//////////////////////////////////////////////////////////////////////////////////

//��̬����
CExceptionManager *	CExceptionManager::m_pExceptionManager=NULL;		//�쳣����

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CExceptionHandle, CDialog)
	ON_BN_CLICKED(IDC_DEBUG, OnBnClickedDebug)
	ON_BN_CLICKED(IDC_SEND_REPORT, OnBnClickedSendReport)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CExceptionHandle::CExceptionHandle() : CDialog(IDD_EXCEPTION)
{
	//���ñ���
	m_lpExceptionInfo=NULL;

	return;
}

//��������
CExceptionHandle::~CExceptionHandle()
{
}

//�ؼ���
VOID CExceptionHandle::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT, m_RichEditCtrl);
}

//��Ϣ����
BOOL CExceptionHandle::PreTranslateMessage(MSG * pMsg)
{
	//��������
	if (pMsg->message==WM_KEYDOWN)
	{
		if (pMsg->wParam==VK_ESCAPE) return TRUE;
		if (pMsg->wParam==VK_RETURN) return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}

//��ʼ������
BOOL CExceptionHandle::OnInitDialog()
{
	__super::OnInitDialog();

	//���þ��
	AfxSetResourceHandle(GetModuleHandle(NULL));

	//����ͼ��
	HICON hIcon=LoadIcon(GetModuleHandle(COMMON_SERVICE_DLL_NAME),MAKEINTRESOURCE(IDI_ICON));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//������Ϣ
	SaveExceptionInfo(m_lpExceptionInfo);

	//ģ������
	TCHAR szModuleName[MAX_PATH]=TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(),szModuleName,CountArray(szModuleName)); 

	InsertString(TEXT("ģ�����֣�"));
	InsertString(szModuleName);
	InsertString(TEXT("\r\n"));

	//������Ϣ
	TCHAR szExceptionInfo[512]=TEXT("");
	_sntprintf(szExceptionInfo,CountArray(szExceptionInfo),TEXT("%ld"),(DWORD_PTR)(m_lpExceptionInfo->ExceptionRecord->ExceptionAddress));
	InsertString(szExceptionInfo);

	return TRUE;
}

//���԰�ť
VOID CExceptionHandle::OnBnClickedDebug()
{
	//�رմ���
	EndDialog(IDC_DEBUG);

	return;
}

//���ͱ���
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

//�����ִ� 
VOID CExceptionHandle::InsertString(LPCTSTR pszString)
{
	//������Ϣ
	m_RichEditCtrl.SetSel(-1L,-1L);
	m_RichEditCtrl.ReplaceSel((LPCTSTR)(pszString));

	return;
}

//�����ļ�
VOID CExceptionHandle::SaveExceptionInfo(EXCEPTION_POINTERS * lpExceptionInfo)
{
	//ϵͳĿ¼
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CAssistant::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//��������
	TCHAR szReprotDirectory[MAX_PATH]=TEXT("");
	_sntprintf(szReprotDirectory,CountArray(szReprotDirectory),TEXT("%s\\BugReport"),szDirectory);
	m_strFileName.Format(TEXT("%s\\%ld%ld.DMP"),szReprotDirectory,(DWORD)time(NULL),GetTickCount());

	//����Ŀ¼
	CreateDirectory(szReprotDirectory,NULL);

	//�����ļ�
	CFile ExceptionFile;
	if (ExceptionFile.Open(m_strFileName,CFile::modeCreate|CFile::modeWrite)==FALSE)
	{
		ASSERT(FALSE);
		return;
	}

	//��������
	MINIDUMP_EXCEPTION_INFORMATION MiniDumpExceptionInfo;
	ZeroMemory(&MiniDumpExceptionInfo,sizeof(MiniDumpExceptionInfo));

	//���ñ���
	MiniDumpExceptionInfo.ClientPointers=FALSE;
	MiniDumpExceptionInfo.ThreadId=GetCurrentThreadId();
	MiniDumpExceptionInfo.ExceptionPointers=lpExceptionInfo;

	//д����Ϣ
	if (MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),ExceptionFile,MiniDumpNormal,&MiniDumpExceptionInfo,NULL,NULL)==FALSE)
	{
		ASSERT(FALSE);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CExceptionManager::CExceptionManager()
{
	//���ö���
	ASSERT(m_pExceptionManager==NULL);
	if (m_pExceptionManager==NULL) m_pExceptionManager=this;

	//�����쳣
	SetUnhandledExceptionFilter(ProcessExceptionFilter);

	return;
}

//��������
CExceptionManager::~CExceptionManager()
{
	//���ö���
	ASSERT(m_pExceptionManager==this);
	if (m_pExceptionManager==this) m_pExceptionManager=NULL;

	return;
}

//�쳣����
LONG WINAPI CExceptionManager::ProcessExceptionFilter(EXCEPTION_POINTERS * lpExceptionInfo)
{
	//���þ��
	AfxSetResourceHandle(GetModuleHandle(COMMON_SERVICE_DLL_NAME));

	//��������
	CExceptionHandle ExceptionHandle;
	ExceptionHandle.m_lpExceptionInfo=lpExceptionInfo;

	//��ʾ����
	if (ExceptionHandle.DoModal()==IDC_DEBUG)
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}

	return EXCEPTION_EXECUTE_HANDLER;
}

//////////////////////////////////////////////////////////////////////////////////

//�쳣����
CExceptionManager g_ExceptionManager;

//////////////////////////////////////////////////////////////////////////////////
