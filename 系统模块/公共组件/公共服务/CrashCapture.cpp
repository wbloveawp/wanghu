#include "stdafx.h"
#include "DbgHelp.h"
#include "WHService.h"
#include "CrashCapture.h"

/////////////////////////////////////////////////////////////////////////////////////////////////
//生成转储文件
int GenerateMiniDump(PEXCEPTION_POINTERS pExceptionPointers)
{
    //定义函数指针
    typedef BOOL(WINAPI* MiniDumpWriteDumpT)(
        HANDLE,
        DWORD,
        HANDLE,
        MINIDUMP_TYPE,
        PMINIDUMP_EXCEPTION_INFORMATION,
        PMINIDUMP_USER_STREAM_INFORMATION,
        PMINIDUMP_CALLBACK_INFORMATION
        );

    //变量定义
    MiniDumpWriteDumpT pfnMiniDumpWriteDump = NULL;

    //加载模块
    HMODULE hDbgHelp = LoadLibrary(TEXT("DbgHelp.dll"));
    if (NULL == hDbgHelp)
    {
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    //获取函数地址
    pfnMiniDumpWriteDump = (MiniDumpWriteDumpT)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");

    //获取失败
    if (NULL == pfnMiniDumpWriteDump)
    {
        FreeLibrary(hDbgHelp);
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    //当前路径
    TCHAR szWorkDirectory[MAX_PATH] = { 0 };
    CWHService::GetWorkDirectory(szWorkDirectory,CountArray(szWorkDirectory));

    //构造路径
    TCHAR szFilePath[MAX_PATH];
    _sntprintf_s(szFilePath, CountArray(szFilePath), TEXT("%s/Dumps"), szWorkDirectory);

    //创建目录
    if (!PathIsDirectory(szFilePath)) CreateDirectory(szFilePath, NULL);

    //进程名称
    TCHAR szProcessName[MAX_PATH] = { 0 };
    CWHService::GetProcessName(szProcessName, CountArray(szProcessName));

    //本地时间
    SYSTEMTIME stLocalTime;
    GetLocalTime(&stLocalTime);

    //文件路径
    TCHAR szFileName[MAX_PATH];
    _sntprintf_s(szFileName, CountArray(szFileName), TEXT("%s/%s(%04d%02d%02d%02d%02d%02d).dmp"), szFilePath, szProcessName, stLocalTime.wYear, stLocalTime.wMonth, stLocalTime.wDay,
        stLocalTime.wHour, stLocalTime.wMinute, stLocalTime.wSecond);

    //创建文件
    HANDLE hDumpFile = CreateFile(szFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
    if (INVALID_HANDLE_VALUE == hDumpFile)
    {
        FreeLibrary(hDbgHelp);
        return EXCEPTION_CONTINUE_EXECUTION;
    }

    //构造参数
    MINIDUMP_EXCEPTION_INFORMATION expParam;
    ZeroMemory(&expParam,sizeof(expParam));

    //设置参数
    expParam.ThreadId = GetCurrentThreadId();
    expParam.ExceptionPointers = pExceptionPointers;
    expParam.ClientPointers = FALSE;

    //写入dmp文件
    pfnMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, MiniDumpWithDataSegs, (pExceptionPointers ? &expParam : NULL), NULL, NULL);

    // 释放文件
    CloseHandle(hDumpFile);
    FreeLibrary(hDbgHelp);

    return EXCEPTION_EXECUTE_HANDLER;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CCrashCapture::CCrashCapture()
{
}

//析构函数
CCrashCapture::~CCrashCapture()
{
}

//异常过滤
LONG CCrashCapture::ExceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo)
{
    if (IsDebuggerPresent())
    {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    return GenerateMiniDump(lpExceptionInfo);
}