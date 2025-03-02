#include "stdafx.h"
#include <psapi.h> 
#include "ProcessHelper.h"


//////////////////////////////////////////////////////////////////////////////////

//ö�ٲ���
struct EnumWindowsArg
{
    HWND hwndWindow;            //���ھ��
    DWORD dwProcessID;          //����ID
};

//ö�ٴ��ڻص�����
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
//ʱ��ת��  
static UINT64 Filetime2Utc(const FILETIME* ftime)
{
    LARGE_INTEGER li;

    assert(ftime);
    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}

//���CPU�ĺ���  
static int GetProcessorNumber()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return (int)info.dwNumberOfProcessors;
}

//////////////////////////////////////////////////////////////////////////////////
//���캯��
CProcessHelper::CProcessHelper()
{

}

//��������
CProcessHelper::~CProcessHelper()
{

}

//���̴���
bool CProcessHelper::IsProcessExists(DWORD dwProcessID)
{
    //�򿪽���
    bool bExists = false;
	auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessID);
    if (hProcess!=NULL)
    {
        //�˳�����
        DWORD dwExitCode;
        GetExitCodeProcess(hProcess, &dwExitCode);

        //����״̬
        if (dwExitCode == STILL_ACTIVE) bExists = true;

        //�رվ��
        CloseHandle(hProcess);
    }     

	return bExists;
}

//��ֹ����
void CProcessHelper::TerminateProcess(DWORD dwProcessID)
{
	auto handle = OpenProcess(PROCESS_TERMINATE, FALSE, dwProcessID);
	::TerminateProcess(handle, 0);
}

//��������
DWORD CProcessHelper::LaunchProcess(LPCTSTR pszExecuteName, LPCTSTR pszLaunchParam,int nShow)
{
    //����ṹ
    SHELLEXECUTEINFO shellExInfo;
    ZeroMemory(&shellExInfo,sizeof(shellExInfo));

    //��������  
    TCHAR szDircetory[MAX_PATH];
    StringCchCopy(szDircetory,CountArray(szDircetory), pszExecuteName);

    //��ȡ·��
    for (int i=lstrlen(szDircetory); i >= 0 ; i--)
    {
        if (szDircetory[i] == '/' || szDircetory[i] == '\\')
        {
            szDircetory[i] = 0;
            break;
        }
    }

    //���ñ���
    shellExInfo.cbSize = sizeof(SHELLEXECUTEINFO);
    shellExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    shellExInfo.hwnd = NULL;
    shellExInfo.lpVerb = NULL;
    shellExInfo.lpFile = pszExecuteName;    
    shellExInfo.lpDirectory = szDircetory;
    shellExInfo.nShow = nShow;
    shellExInfo.hInstApp = NULL;
    shellExInfo.lpParameters = pszLaunchParam;

    //��������
    int ret = ShellExecuteEx(&shellExInfo);

    //��ȡ��ʶ
    return ::GetProcessId(shellExInfo.hProcess);
}

//CPUʹ��
int CProcessHelper::GetCPUUsage(DWORD dwProcessID, int64_t &LastTime, int64_t &LastSystemTime)
{
    //cpu����  
    static int PocessorCount = -1;

    //ʱ�䶨��
    FILETIME Now;
    FILETIME ExitTime;
    FILETIME UserTime;
    FILETIME KernelTime;    
    FILETIME CreationTime;
    int64_t SystemTime,Time,SystemTimeDelta,TimeDelta;

    //��������
    if (PocessorCount == -1)
    {
        PocessorCount = GetProcessorNumber();
    }

    //��ȡ���
    auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwProcessID);
    if (hProcess == NULL)
    {
        return -1;
    }

    //����ʱ��
    if (!GetProcessTimes(hProcess, &CreationTime, &ExitTime, &KernelTime, &UserTime))
    {
       return -1;
    }

    //��ȡʱ��
    GetSystemTimeAsFileTime(&Now);

    //����ʱ��
    Time = Filetime2Utc(&Now);
    SystemTime = (Filetime2Utc(&KernelTime) + Filetime2Utc(&UserTime)) / PocessorCount;    

    //У��ʱ��
    if ((LastSystemTime == 0) || (LastTime == 0))
    {
        LastTime = Time;
        LastSystemTime = SystemTime;        
        return -1;
    }

    //������ʱ
    TimeDelta = Time - LastTime;
    SystemTimeDelta = SystemTime - LastSystemTime;    

    //У����ʱ
    if (TimeDelta == 0)
    {
        return -1;
    }

    //����CPU
    auto nCPUUasge = (int)((SystemTimeDelta * 100 + TimeDelta / 2) / TimeDelta);

    //��¼ʱ��
    LastTime = Time;
    LastSystemTime = SystemTime;  

    //�رվ��
    CloseHandle(hProcess);

    return nCPUUasge;
}

//�ڴ�ʹ��
bool CProcessHelper::GetMemoryUsage(DWORD dwProcessID, UINT64* pMemory, UINT64* pVMemory)
{
    PROCESS_MEMORY_COUNTERS pmc;
    auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, PROCESS_VM_READ, dwProcessID);
    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
    {
        if (pMemory) *pMemory = pmc.WorkingSetSize;
        if (pVMemory) *pVMemory = pmc.PagefileUsage;       
    }

    //�رվ��
    CloseHandle(hProcess);

    return true;
}

//IO�ֽ���
bool CProcessHelper::GetIOBytes(DWORD dwProcessID, UINT64* pReadBytes, UINT64* pWriteBytes)
{
    IO_COUNTERS Ioc;
    auto hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, PROCESS_VM_READ, dwProcessID);
    if (GetProcessIoCounters(hProcess, &Ioc))
    {
        if (pReadBytes) *pReadBytes = Ioc.ReadTransferCount;
        if (pWriteBytes) *pWriteBytes = Ioc.WriteTransferCount;        
    }

    //�رվ��
    CloseHandle(hProcess);

    return true;
}

//���ھ��
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

//��ȡ·��
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