#ifndef PROCESS_HELPER_HEAD_FILE
#define PROCESS_HELPER_HEAD_FILE
#pragma once

//引入文件
#include "Stdafx.h"

/////////////////////////////////////////////////////////////////////////////////////////
//进程助手
class CProcessHelper
{
	//函数定义
public:
	//构造函数
	CProcessHelper();
	//析构函数
	virtual ~CProcessHelper();

	//进程管理
public:
	//查找进程
	static bool IsProcessExists(DWORD dwProcessID);
	//终止进程
	static void TerminateProcess(DWORD dwProcessID);
	//启动进程
	static DWORD LaunchProcess(LPCTSTR pszExecuteName, LPCTSTR pszLaunchParam,int nShow);

	//服务管理
public:
	//开始服务
	static bool StartService(DWORD dwProcessID);
	//关闭服务
	static bool ConcludeService(DWORD dwProcessID);	

	//辅助函数
public:
	//窗口句柄
	static HWND GetWindowHwndByPID(DWORD dwProcessID);
	//获取路径
	static bool GetProcessPathByPId(DWORD dwProcessId, TCHAR szProcessPath[MAX_PATH]);

	//进程信息
public:
	//CPU使用
	static int  GetCPUUsage(DWORD dwProcessID, int64_t& LastTime, int64_t& LastSystemTime);
	//内存使用
	static bool GetMemoryUsage(DWORD dwProcessID, UINT64* pMemory, UINT64* pVMemory);
	//IO字节数
	static bool GetIOBytes(DWORD dwProcessID, UINT64* pReadBytes, UINT64* pWriteBytes);		
};

/////////////////////////////////////////////////////////////////////////////////////////

#endif
