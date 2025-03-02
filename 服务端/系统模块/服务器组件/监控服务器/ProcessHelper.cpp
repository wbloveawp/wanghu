#include "stdafx.h"
#include <psapi.h> 
#include "ProcessHelper.h"


//////////////////////////////////////////////////////////////////////////////////

//枚举参数
struct EnumWindowsArg
{
    HWND hwndWindow;            //窗口句柄
    DWORD dwProcessID;          //进程ID
};

//枚举窗口回调函数
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    EnumWindowsArg* pArg = (EnumWindowsArg*)lParam;
    DWORD dwProcessID = 0;
    
    ::GetWindowThreadProcessId(hwnd, &dwProcessID);
    if (dwProcessID == pArg->dwProcessID)
    {
        pArg->hwndWindow = hwnd;        
        return FALSE;
    }

    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////////
// 
//时间转换  
static UINT64 Filetime2Utc(const FILETIME* ftime)
{
    LARGE_INTEGER li;

    assert(ftime);
    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}

//获得CPU的核数  
static int GetProcessorNumber()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return (int)info.dwNumberOfProcessors;
}

//////////////////////////////////////////////////////////////////////////////////
//构造函数
CProcessHelper::CProcessHelper()
{

}

//析构函数
CProcessHelper::~CProcessHelper()
{

}

//进程存在
bool CProcessHelper::IsProcessExists(DWORD dwProcessID)
{
    //打开进程
    bool bExists = false;
	auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
    if (hProcess!=NULL)
    {
        //退出代码
        DWORD dwExitCode;
        GetExitCodeProcess(hProcess, &dwExitCode);

        //激活状态
        if (dwExitCode == STILL_ACTIVE) bExists = true;

        //关闭句柄
        CloseHandle(hProcess);
    }     

	return bExists;
}

//终止进程
void CProcessHelper::TerminateProcess(DWORD dwProcessID)
{
	auto handle = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
	::TerminateProcess(handle, 0);
}

//启动进程
DWORD CProcessHelper::LaunchProcess(LPCTSTR pszExecuteName, LPCTSTR pszLaunchParam,int nShow)
{
    //构造结构
    SHELLEXECUTEINFO shellExInfo;
    ZeroMemory(&shellExInfo,sizeof(shellExInfo));

    //变量定义  
    TCHAR szDircetory[MAX_PATH];
    StringCchCopy(szDircetory,CountArray(szDircetory), pszExecuteName);

    //截取路径
    for (int i=lstrlen(szDircetory); i >= 0 ; i--)
    {
        if (szDircetory[i] == '/' || szDircetory[i] == '\\')
        {
            szDircetory[i] = 0;
            break;
        }
    }

    //设置变量
    shellExInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    shellExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    shellExInfo.hwnd = NULL;
    shellExInfo.lpVerb = NULL;
    shellExInfo.lpFile = pszExecuteName;    
    shellExInfo.lpDirectory = szDircetory;
    shellExInfo.nShow = nShow;
    shellExInfo.hInstApp = NULL;
    shellExInfo.lpParameters = pszLaunchParam;

    //启动进程
    int ret = ShellExecuteEx(&shellExInfo);

    //获取标识
    return ::GetProcessId(shellExInfo.hProcess);
}

//CPU使用
int CProcessHelper::GetCPUUsage(DWORD dwProcessID, int64_t &LastTime, int64_t &LastSystemTime)
{
    //cpu数量  
    static int PocessorCount = -1;

    //时间定义
    FILETIME Now;
    FILETIME ExitTime;
    FILETIME UserTime;
    FILETIME KernelTime;    
    FILETIME CreationTime;
    int64_t SystemTime,Time,SystemTimeDelta,TimeDelta;

    //核心数量
    if (PocessorCount == -1)
    {
        PocessorCount = GetProcessorNumber();
    }

    //获取句柄
    auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessID);
    if (hProcess == NULL)
    {
        return -1;
    }

    //进程时间
    if (!GetProcessTimes(hProcess, &CreationTime, &ExitTime, &KernelTime, &UserTime))
    {
       return -1;
    }

    //获取时间
    GetSystemTimeAsFileTime(&Now);

    //计算时间
    Time = Filetime2Utc(&Now);
    SystemTime = (Filetime2Utc(&KernelTime) + Filetime2Utc(&UserTime)) / PocessorCount;    

    //校验时间
    if ((LastSystemTime == 0) || (LastTime == 0))
    {
        LastTime = Time;
        LastSystemTime = SystemTime;        
        return -1;
    }

    //计算延时
    TimeDelta = Time - LastTime;
    SystemTimeDelta = SystemTime - LastSystemTime;    

    //校验延时
    if (TimeDelta == 0)
    {
        return -1;
    }

    //计算CPU
    auto nCPUUasge = (int)((SystemTimeDelta * 100 + TimeDelta / 2) / TimeDelta);

    //记录时间
    LastTime = Time;
    LastSystemTime = SystemTime;  

    //关闭句柄
    CloseHandle(hProcess);

    return nCPUUasge;
}

//内存使用
bool CProcessHelper::GetMemoryUsage(DWORD dwProcessID, UINT64* pMemory, UINT64* pVMemory)
{
    PROCESS_MEMORY_COUNTERS pmc;
    auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, PROCESS_VM_READ, dwProcessID);
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
    {
        if (pMemory) *pMemory = pmc.WorkingSetSize;
        if (pVMemory) *pVMemory = pmc.PagefileUsage;       
    }

    //关闭句柄
    CloseHandle(hProcess);

    return true;
}

//IO字节数
bool CProcessHelper::GetIOBytes(DWORD dwProcessID, UINT64* pReadBytes, UINT64* pWriteBytes)
{
    IO_COUNTERS Ioc;
    auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, PROCESS_VM_READ, dwProcessID);
    if (GetProcessIoCounters(hProcess, &Ioc))
    {
        if (pReadBytes) *pReadBytes = Ioc.ReadTransferCount;
        if (pWriteBytes) *pWriteBytes = Ioc.WriteTransferCount;        
    }

    //关闭句柄
    CloseHandle(hProcess);

    return true;
}

//窗口句柄
HWND CProcessHelper::GetWindowHwndByPID(DWORD dwProcessID)
{
    HWND hwndRet = NULL;
    EnumWindowsArg ewa;
    ewa.dwProcessID = dwProcessID;
    ewa.hwndWindow = NULL;
    EnumWindows(EnumWindowsProc, (LPARAM)&ewa);
    if (ewa.hwndWindow)
    {
        hwndRet = ewa.hwndWindow;
    }
    return hwndRet;
}

//获取路径
bool CProcessHelper::GetProcessPathByPId(DWORD dwProcessId, TCHAR szProcessPath[MAX_PATH])
{
    bool bSuccess = false;
    HANDLE hProcess = NULL;    
    hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessId);
    do
    {
        if (NULL == hProcess)
        {
            break;
        }
        
        HMODULE hMod = NULL;
        DWORD cbNeeded = 0;
        TCHAR szPath[MAX_PATH] = { 0 };
        if (FALSE == EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
        {
            break;
        }
        if (0 == GetModuleFileNameEx(hProcess, hMod, szPath, MAX_PATH))
        {
            break;
        }
        wcscpy(szProcessPath, szPath);
        bSuccess = true;

    } while (0);

    if (NULL != hProcess)
    {
        CloseHandle(hProcess);
        hProcess = NULL;
    }

    return bSuccess;
}

//////////////////////////////////////////////////////////////////////////////////