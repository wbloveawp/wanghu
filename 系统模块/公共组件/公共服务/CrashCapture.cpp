#include "stdafx.h"
#include "DbgHelp.h"
#include "WHService.h"
#include "CrashCapture.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
//����ת���ļ�
int GenerateMiniDump(PEXCEPTION_POINTERS pExceptionPointers)
{
    //���庯��ָ��
    typedef BOOL(WINAPI* MiniDumpWriteDumpT)(
        HANDLE,
        DWORD,
        HANDLE,
        MINIDUMP_TYPE,
        PMINIDUMP_EXCEPTION_INFORMATION,
        PMINIDUMP_USER_STREAM_INFORMATION,
        PMINIDUMP_CALLBACK_INFORMATION
        );

    //��������
    MiniDumpWriteDumpT pfnMiniDumpWriteDump = NULL;

    //����ģ��
    HMODULE hDbgHelp = LoadLibrary(TEXT("DbgHelp.dll"));
    if (NULL == hDbgHelp)
    {
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    //��ȡ������ַ
    pfnMiniDumpWriteDump = (MiniDumpWriteDumpT)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");

    //��ȡʧ��
    if (NULL == pfnMiniDumpWriteDump)
    {
        FreeLibrary(hDbgHelp);
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    //��ǰ·��
    TCHAR szWorkDirectory[MAX_PATH] = { 0 };
    CWHService::GetWorkDirectory(szWorkDirectory,CountArray(szWorkDirectory));

    //����·��
    TCHAR szFilePath[MAX_PATH];
    _sntprintf_s(szFilePath, CountArray(szFilePath), TEXT("%s/Dumps"), szWorkDirectory);

    //����Ŀ¼
    if (!PathIsDirectory(szFilePath)) CreateDirectory(szFilePath, NULL);

    //��������
    TCHAR szProcessName[MAX_PATH] = { 0 };
    CWHService::GetProcessName(szProcessName, CountArray(szProcessName));

    //����ʱ��
    SYSTEMTIME stLocalTime;
    GetLocalTime(&stLocalTime);

    //�ļ�·��
    TCHAR szFileName[MAX_PATH];
    _sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s/%s(%04d%02d%02d%02d%02d%02d).dmp"), szFilePath, szProcessName, stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
        stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);

    //�����ļ�
    HANDLE hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
    if (INVALID_HANDLE_VALUE == hDumpFile)
    {
        FreeLibrary(hDbgHelp);
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    //�������
    MINIDUMP_EXCEPTION_INFORMATION expParam;
    ZeroMemory(&expParam,sizeof(expParam));

    //���ò���
    expParam.ThreadId = GetCurrentThreadId();
    expParam.ExceptionPointers = pExceptionPointers;
    expParam.ClientPointers = FALSE;

    //д��dmp�ļ�
    pfnMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpWithDataSegs, (pExceptionPointers ? &expParam : NULL), NULL, NULL);

    // �ͷ��ļ�
    CloseHandle(hDumpFile);
    FreeLibrary(hDbgHelp);

    return EXCEPTION_EXECUTE_HANDLER;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CCrashCapture::CCrashCapture()
{
}

//��������
CCrashCapture::~CCrashCapture()
{
}

//�쳣����
LONG CCrashCapture::ExceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo)
{
    if (IsDebuggerPresent())
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    return GenerateMiniDump(lpExceptionInfo);
}