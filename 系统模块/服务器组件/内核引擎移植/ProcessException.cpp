// ProcessException.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Resource.h"
#include "ProcessException.h"
#include <Tlhelp32.h>  
#include <dbghelp.h>


//���캯��
CExceptionHandle::CExceptionHandle() : CDialog(IDD_DLG_EXCEPTION)
{
}

//��������
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


// CExceptionHandle ��Ϣ�������


BOOL CExceptionHandle::OnInitDialog()
{
	__super::OnInitDialog();

	AfxSetResourceHandle(GetModuleHandle(NULL));
	//����ͼ��
	HICON hIcon = LoadIcon(GetModuleHandle(KERNEL_ENGINE_DLL_NAME), MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	//�����쳣��Ϣ
	SaveExceptionInfo(m_lpExceptionInfo);

	//��ʾ�쳣��Ϣ
	TCHAR szFileName[_MAX_PATH] = TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(), szFileName, _MAX_PATH);
	InsertString(_T("Ӧ�ó���:"));
	InsertString(szFileName);
	InsertString(TEXT("\r\n"));

	//�쳣��Ϣ
	TCHAR szDest[250] = TEXT("");
	_sntprintf(szDest, CountArray(szDest), TEXT("�쳣��ַ:%ld"), m_lpExceptionInfo->ExceptionRecord->ExceptionAddress);
	InsertString(szDest);

	return TRUE; 
}


BOOL CExceptionHandle::PreTranslateMessage(MSG* pMsg)
{
	//��������
	if ((pMsg->message == WM_KEYDOWN) || (pMsg->wParam == VK_ESCAPE) && pMsg->wParam == VK_RETURN)
	{
		return TRUE;
	}

	return __super::PreTranslateMessage(pMsg);
}


void CExceptionHandle::OnBnClickedSendReport()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//������Խ��쳣�������������� ��ʱδʵ��
	EndDialog(-1);
	
}


void CExceptionHandle::OnBnClickedDebug()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	EndDialog(IDABORT);
}

VOID CExceptionHandle::InsertString(LPCTSTR pszString)
{
	m_RichEditCtrl.SetSel(-1, -1);
	m_RichEditCtrl.ReplaceSel(pszString);
}
//�����ļ�
VOID CExceptionHandle::SaveExceptionInfo(EXCEPTION_POINTERS * lpExceptionInfo)
{
	//��ȡ��ǰ����·��
	TCHAR szPath[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szPath, MAX_PATH);

	//����Ŀ¼
	TCHAR szDirectory[MAX_PATH] = TEXT("");
	_sntprintf(szDirectory, MAX_PATH, TEXT("%s\\BugReport"), &szPath);
	//�����ļ�
	m_strFileName.Format(TEXT("%s\\%ld%ld.DMP"), szDirectory, GetTickCount(), time(NULL));
	//����Ŀ¼
	CreateDirectory(szDirectory, NULL);
	//���쳣д���ļ�
	CFile hFile;
	//���ļ�
	if (hFile.Open(m_strFileName, CFile::modeCreate | CFile::modeWrite, NULL))
	{
		//д���쳣��Ϣ
		_MINIDUMP_EXCEPTION_INFORMATION ExInfo;
		ExInfo.ThreadId = ::GetCurrentThreadId();
		ExInfo.ExceptionPointers = lpExceptionInfo;
		ExInfo.ClientPointers = NULL;
		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, 0, 0);
	}
}


CExceptionManager *CExceptionManager::m_pExceptionManager = new CExceptionManager();

//���캯��
CExceptionManager::CExceptionManager()
{
	if (!CExceptionManager::m_pExceptionManager)
	{
		CExceptionManager::m_pExceptionManager = this;
	}

	::SetUnhandledExceptionFilter(CExceptionManager::ProcessExceptionFilter);
}
//��������
CExceptionManager::~CExceptionManager()
{

}

LONG CExceptionManager::ProcessExceptionFilter(EXCEPTION_POINTERS * lpExceptionInfo)
{
	AfxSetResourceHandle(GetModuleHandle(KERNEL_ENGINE_DLL_NAME));
	//�쳣����Ի���
	CExceptionHandle exceptionHandle;
	//�����쳣��Ϣ
	exceptionHandle.m_lpExceptionInfo = lpExceptionInfo;
	//ģ̬�Ի���
	int nRet = exceptionHandle.DoModal();
	//������԰�ť
	if (nRet == IDABORT)
	{
		return EXCEPTION_CONTINUE_SEARCH;
	}
	else  //���ͱ���
	{
		return EXCEPTION_EXECUTE_HANDLER;
	}
}
