#include "StdAfx.h"
#include <time.h> 
#include "WHService.h"
#include "DataEncrypt.h"

//////////////////////////////////////////////////////////////////////////

//导入链接
#pragma comment(lib,"Version")
#pragma comment(lib,"Iphlpapi")

#pragma warning (disable : 4996)

//////////////////////////////////////////////////////////////////////////

//状态信息
struct tagAstatInfo
{
	ADAPTER_STATUS					AdapterStatus;						//网卡状态
	NAME_BUFFER						NameBuff[16];						//名字缓冲
};

//////////////////////////////////////////////////////////////////////////

//校验占位符
bool VerifyPlaceHolder(LPCTSTR pszString, int nStartIndex, int nEndIndex)
{
	//长度校验
	ASSERT(lstrlen(pszString) > nEndIndex);
	if (lstrlen(pszString) <= nEndIndex) return false;

	//参数校验
	ASSERT(pszString != NULL && nEndIndex > nStartIndex);
	if (pszString == NULL || nEndIndex <= nStartIndex) return false;

	//常量定义
	const TCHAR szFlags[] = { TEXT('-'), TEXT('+'), TEXT(' '), TEXT('#'), TEXT('0') };
	const TCHAR szDecorate[] = { TEXT('h'), TEXT('j'), TEXT('l'), TEXT('L'), TEXT('t'), TEXT('z'), TEXT('F'), TEXT('N'), TEXT('I') };

	//类型定义
	enum emFormatOption
	{
		FORMAT_OP_NONE = 0,
		FORMAT_OP_FLAGS,
		FORMAT_OP_WIDTH,
		FORMAT_OP_PREC,
		FORMAT_OP_DECORATE
	};

	//变量定义
	emFormatOption FormatOption = FORMAT_OP_NONE;

	for (auto nIndex = nStartIndex + 1; nIndex <= nEndIndex - 1; nIndex++)
	{
		bool bContinue = false;
		auto pszMove = (pszString + nIndex);
		auto chr = *pszMove;

		//flags
		if (FormatOption < FORMAT_OP_FLAGS)
		{
			for (auto flag : szFlags)
			{
				if (chr == flag)
				{
					bContinue = true;
					FormatOption = FORMAT_OP_FLAGS;
					break;
				}
			}
			if (bContinue) continue;
		}

		//width		
		if (FormatOption < FORMAT_OP_WIDTH)
		{
			if ((chr >= TEXT('0') && chr <= TEXT('9')) || chr == TEXT('*'))
			{
				auto count = (chr == TEXT('*')) ? 1 : 0;

				while (++nIndex <= nEndIndex - 1)
				{
					auto ch = *(pszString + nIndex);

					//*
					if (ch == TEXT('*'))
					{
						count++;
						continue;
					}

					//0-9
					if ((ch >= TEXT('0') && ch <= TEXT('9')))
					{
						continue;
					}

					nIndex--;
					break;
				}

				if (count > 1) return false;

				FormatOption = FORMAT_OP_WIDTH;
				continue;
			}
		}

		//.prec
		if (FormatOption < FORMAT_OP_PREC)
		{
			if (chr == TEXT('.'))
			{
				auto count = 0;

				while (++nIndex <= nEndIndex - 1)
				{
					auto ch = *(pszString + nIndex);

					//*
					if (ch == TEXT('*'))
					{
						count++;
						continue;
					}

					//0-9
					if ((ch >= TEXT('0') && ch <= TEXT('9')))
					{
						continue;
					}

					nIndex--;
					break;
				}

				if (count > 1) return false;

				FormatOption = FORMAT_OP_PREC;
				continue;
			}
		}

		//decorate
		if (FormatOption < FORMAT_OP_DECORATE)
		{
			for (auto dec : szDecorate)
			{
				if (chr == dec)
				{
					switch (chr)
					{
					case TEXT('h'):
					{
						if (nIndex <= nEndIndex - 1 && *(pszMove + 1) == TEXT('h'))
						{
							nIndex++;
						}
						break;
					}
					case TEXT('l'):
					{
						if (nIndex <= nEndIndex - 1 && *(pszMove + 1) == TEXT('l'))
						{
							nIndex++;
						}
						break;
					}
					case TEXT('I'):
					{
						if (nIndex <= nEndIndex - 3 && ((*(pszMove + 1) == TEXT('3') && *(pszMove + 2) == TEXT('2')) || (*(pszMove + 1) == TEXT('6') && *(pszMove + 2) == TEXT('4'))))
						{
							nIndex += 2;
						}
						else
						{
							return false;
						}
						break;
					}
					}

					bContinue = true;
					FormatOption = FORMAT_OP_DECORATE;
					break;
				}
			}

			if (bContinue) continue;
		}

		return false;
	}

	return true;
}

//提取占位符
VOID ExtractPlaceHolder(LPCTSTR pszString, CString& strPlaceHolder)
{
	//占位符
	const TCHAR szHolderList[] = { TEXT('a'), TEXT('A'), TEXT('c'), TEXT('d'), TEXT('e'), TEXT('E'), TEXT('f'), TEXT('G'), TEXT('i'), TEXT('o'), TEXT('p'), TEXT('s'), TEXT('u'), TEXT('x'), TEXT('X') };

	//移除数据
	strPlaceHolder.Empty();

	//变量定义
	LPCTSTR pszMove = pszString;

	//遍历字符串
	while (*pszMove != TEXT('\0'))
	{
		if (*pszMove == TEXT('%'))
		{
			//转义处理
			if (*(pszMove + 1) == TEXT('%'))
			{
				pszMove = pszMove + 2;
				continue;
			}

			//变量定义
			bool bExit = false;
			auto pszNewMove = (pszMove + 1);

			//索引定义			
			int nEndIndex = -1;
			int nStartIndex = pszMove - pszString;

			//查找占位符
			while (*pszNewMove != TEXT('\0'))
			{
				for (auto chr : szHolderList)
				{
					if (*pszNewMove != chr) continue;

					//设置变量
					bExit = true;
					nEndIndex = pszNewMove - pszString;

					if (VerifyPlaceHolder(pszString, nStartIndex, nEndIndex) == true)
					{
						if (!strPlaceHolder.IsEmpty())
						{
							strPlaceHolder.AppendChar(',');
						}

						strPlaceHolder.AppendChar('"');
						strPlaceHolder.Append(pszString + nStartIndex, nEndIndex - nStartIndex + 1);
						strPlaceHolder.AppendChar('"');
					}

					break;
				}

				if (bExit)
				{
					pszMove = pszNewMove;
					break;
				}

				pszNewMove++;
			}
		}

		pszMove++;
	}
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CWHService::CWHService()
{
}

//获取机器码
bool CWHService::GetMachineID(TCHAR szMachineID[LEN_MACHINE_ID])
{
	//变量定义
	TCHAR szNetWorkID[LEN_NETWORK_ID]=TEXT("");

	//网卡标识
	GetNetWorkID(szNetWorkID);

	//转换信息
	TCHAR szMD5Machine[LEN_MD5] = {0};
	CMD5Encrypt::EncryptData(szNetWorkID,szMD5Machine);

	//复制结果
	StringCchCopy(szMachineID, LEN_MACHINE_ID, szMD5Machine);

	return true;
}

//网卡地址
bool CWHService::GetNetWorkID(TCHAR szNetWorkID[LEN_NETWORK_ID])
{
	CWHBuffer<BYTE,sizeof(IP_ADAPTER_INFO)> AdapterBuffer;
	ULONG uBufferLength=AdapterBuffer.GetBufferSize();

	//创建内存
	if (GetAdaptersInfo(NULL,&uBufferLength)==ERROR_BUFFER_OVERFLOW)
	{
		AdapterBuffer.CreateBuffer(uBufferLength);
	}

	//查询网卡
	if ((GetAdaptersInfo((IP_ADAPTER_INFO *)AdapterBuffer.GetData(),&uBufferLength))==NO_ERROR)
	{
		//变量定义
		BYTE cbOutBuffer[16];		
		IP_ADAPTER_INFO * pEthernetAdapter=NULL;
		IP_ADAPTER_INFO * pPhysicalAdapter=NULL;
		IP_ADAPTER_INFO * pWirelessAdapter=NULL;
		IP_ADAPTER_INFO * pAdapterInfo=(IP_ADAPTER_INFO *)AdapterBuffer.GetData();

		//变量定义
		while (pAdapterInfo!=NULL)
		{
			//以太网卡
			if (pAdapterInfo->Type==MIB_IF_TYPE_ETHERNET)
			{		
				//物理网卡
				if(strstr(pAdapterInfo->Description,"PCI")>0)
				{
					pPhysicalAdapter=pAdapterInfo;
					break;
				}
				else
				{
					pEthernetAdapter=pAdapterInfo;
				}
			}

			//无线网卡
			if (pAdapterInfo->Type==71)
			{				
				pWirelessAdapter=pAdapterInfo;
			}

			//设置索引
			pAdapterInfo=pAdapterInfo->Next;
		}

		//物理网卡优先、无线网卡次之
		if(pWirelessAdapter!=NULL) pEthernetAdapter=pWirelessAdapter;
		if(pPhysicalAdapter!=NULL) pEthernetAdapter=pPhysicalAdapter;
		if(pEthernetAdapter!=NULL)
		{
			//网卡标识
			TCHAR szDeviceName[MAX_PATH]=TEXT("");
			_sntprintf(szDeviceName,CountArray(szDeviceName),TEXT("//./%s"),(LPCTSTR)(CA2T(pEthernetAdapter->AdapterName)));

			//打开设备
			HANDLE hDevice=CreateFile(szDeviceName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0L,NULL);

			//错误判断
			if (hDevice==INVALID_HANDLE_VALUE)
			{
				ASSERT(FALSE);			
				return true;
			}

			//获取地址
			DWORD dwReturned=0L;
			DWORD dwInputBuffer=OID_802_3_PERMANENT_ADDRESS;
			if (DeviceIoControl(hDevice,IOCTL_NDIS_QUERY_GLOBAL_STATS,&dwInputBuffer,sizeof(dwInputBuffer),cbOutBuffer,sizeof(cbOutBuffer),&dwReturned,NULL)!=FALSE)
			{
				_sntprintf(szNetWorkID,LEN_NETWORK_ID,TEXT("%02X%02X%02X%02X%02X%02X"),cbOutBuffer[0],cbOutBuffer[1],cbOutBuffer[2],cbOutBuffer[3],cbOutBuffer[4],cbOutBuffer[5]);
				return true;
			}
		}
	}

	//错误断言
	ASSERT(FALSE);

	//设置默认
	ZeroMemory(szNetWorkID,sizeof(TCHAR)*LEN_NETWORK_ID);

	return false;
}

//拷贝字符
bool CWHService::CopyToClipBoard(LPCTSTR pszString)
{
	//变量定义
	HWND hWnd=AfxGetMainWnd()->GetSafeHwnd();

	//打开剪切板
	if (OpenClipboard(hWnd)==FALSE) return false;
	if (EmptyClipboard()==FALSE) { CloseClipboard(); return false; }

	//复制数据
	HANDLE hData=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,CountStringBuffer(pszString));
	if (hData==NULL) 
	{
		CloseClipboard();
		return false;
	}

	//复制数据
	CT2CA strString(pszString);
	LPSTR pszMemString=(LPSTR)GlobalLock(hData);
	strcpy(pszMemString,(LPCSTR)strString);
	SetClipboardData(CF_TEXT,hData);

	//环境设置
	GlobalUnlock(hData);
	CloseClipboard();

	return true;
}

//注册热键
bool CWHService::RegisterHotKey(HWND hWnd, UINT uKeyID, WORD wHotKey)
{
	//变量定义
	BYTE cbModifiers=0;
	if (HIBYTE(wHotKey)&HOTKEYF_ALT) cbModifiers|=MOD_ALT;
	if (HIBYTE(wHotKey)&HOTKEYF_SHIFT) cbModifiers|=MOD_SHIFT;
	if (HIBYTE(wHotKey)&HOTKEYF_CONTROL) cbModifiers|=MOD_CONTROL;

	//注册热键
	BOOL bSuccess=::RegisterHotKey(hWnd,uKeyID,cbModifiers,LOBYTE(wHotKey));

	return (bSuccess==TRUE)?true:false;
}

//进程名称
bool CWHService::GetProcessName(TCHAR szProcessName[], WORD wBufferCount)
{
	//模块路径
	TCHAR szModulePath[MAX_PATH] = TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(), szModulePath, CountArray(szModulePath));

	//分析文件
	for (INT i = lstrlen(szModulePath); i >= 0; i--)
	{
		//去掉后缀
		if (szModulePath[i] == TEXT('.'))
		{
			szModulePath[i] = 0;
			continue;
		}

		if (szModulePath[i] == TEXT('\\'))
		{
			StringCchCopy(szProcessName, wBufferCount, (LPCTSTR)&szModulePath[i+1]);
			break;
		}
	}	

	return true;
}

//进程目录
bool CWHService::GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount)
{
	//模块路径
	TCHAR szModulePath[MAX_PATH]=TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(),szModulePath,CountArray(szModulePath));

	//分析文件
	for (INT i=lstrlen(szModulePath);i>=0;i--)
	{
		if (szModulePath[i]==TEXT('\\'))
		{
			szModulePath[i]=0;
			break;
		}
	}

	//设置结果
	ASSERT(szModulePath[0]!=0);
	StringCchCopy(szWorkDirectory,wBufferCount, szModulePath);

	return true;
}

//文件版本
bool CWHService::GetModuleVersion(LPCTSTR pszModuleName, DWORD & dwVersionInfo)
{
	//设置结果
	dwVersionInfo=0L;

	//接收缓冲
	BYTE cbInfoBuffer[1024];
	ZeroMemory(cbInfoBuffer,sizeof(cbInfoBuffer));

	//模块信息
  DWORD dwFileHandle=NULL;
	if (GetFileVersionInfo(pszModuleName,dwFileHandle,sizeof(cbInfoBuffer),cbInfoBuffer)==FALSE) return false;

	//获取信息
	UINT uQuerySize=0;
	VS_FIXEDFILEINFO * pFixedFileInfo=NULL;
  if (VerQueryValue(cbInfoBuffer,TEXT("\\"),(VOID * *)&pFixedFileInfo,&uQuerySize)==FALSE) return false;

	//设置结果
	if ((pFixedFileInfo!=NULL)&&(uQuerySize==sizeof(VS_FIXEDFILEINFO)))
	{
		//设置版本
		WORD wVersion1=HIWORD(pFixedFileInfo->dwFileVersionMS);
		WORD wVersion2=LOWORD(pFixedFileInfo->dwFileVersionMS);
		WORD wVersion3=HIWORD(pFixedFileInfo->dwFileVersionLS);
		WORD wVersion4=LOWORD(pFixedFileInfo->dwFileVersionLS);
		dwVersionInfo=MAKELONG(MAKEWORD(wVersion4,wVersion3),MAKEWORD(wVersion2,wVersion1));

		return true;
	}

	return false;
}

//Ascii转UTF8
int CWHService::GB23122Utf8(LPCSTR pszGB2312, std::string& sUtf8)
{
	//计算长度
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, pszGB2312, -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen];
	ZeroMemory(pwBuf, nwLen * sizeof(wchar_t));

	//计算长度
	::MultiByteToWideChar(CP_ACP, 0, pszGB2312, lstrlenA(pszGB2312), pwBuf, nwLen);
	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char* pBuf = new char[nLen];
	ZeroMemory(pBuf, nLen);

	//转换编码
	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	//添加结束符
	pBuf[nLen-1] = 0;

	//构造结果
	sUtf8.assign(pBuf);

	//释放资源
	SafeDeleteArray(pwBuf);
	SafeDeleteArray(pBuf);

	return nLen;	
}

//Ascii转UTF8
int CWHService::GB23122Utf8(LPCSTR pszGB2312, LPSTR pszUtf8, int nBuffSize)
{
	//计算长度
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, pszGB2312, -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen];
	ZeroMemory(pwBuf, nwLen * sizeof(wchar_t));

	//计算长度
	::MultiByteToWideChar(CP_ACP, 0, pszGB2312, lstrlenA(pszGB2312), pwBuf, nwLen);
	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	if (nLen > nBuffSize)
	{
		return 0;
	}

	//转换编码
	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pszUtf8, nLen, NULL, NULL);

	//添加结束符
	pszUtf8[nLen - 1] = 0;

	//释放资源
	SafeDeleteArray(pwBuf);

	return nLen;	
}


//UTF8转Ascii
int CWHService::Utf82GB2312(LPCSTR pszUtf8, std::string& sAscii)
{
	//计算长度
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, pszUtf8, -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen];
	ZeroMemory(pwBuf, nwLen * sizeof(wchar_t));

	//计算长度
	MultiByteToWideChar(CP_UTF8, 0, pszUtf8, lstrlenA(pszUtf8), pwBuf, nwLen);
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char* pBuf = new char[nLen];
	ZeroMemory(pBuf, nLen);

	//编码转换
	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	//添加结束符
	pBuf[nLen-1] = 0;

	//构造结果
	sAscii.assign(pBuf, nLen);

	//释放资源
	SafeDeleteArray(pwBuf);
	SafeDeleteArray(pBuf);

	return nLen;
}

//UTF8转Ascii
int CWHService::Utf82GB2312(LPCSTR pszUtf8, LPSTR pszGB2312, int nBuffSize)
{
	//计算长度
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, pszUtf8, -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen];
	ZeroMemory(pwBuf, nwLen * sizeof(wchar_t));

	//计算长度
	MultiByteToWideChar(CP_UTF8, 0, pszUtf8, lstrlenA(pszUtf8), pwBuf, nwLen);
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	if (nLen > nBuffSize)
	{
		return 0;
	}

	//编码转换
	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pszGB2312, nLen, NULL, NULL);

	//添加结束符
	pszGB2312[nLen - 1] = 0;

	//释放资源
	SafeDeleteArray(pwBuf);

	return nLen;
}


//Unicode2转UTF8
int CWHService::Unicode2Utf8(LPCWSTR pszUnicode, std::string& sUtf8)
{
	//参数校验
	if (!pszUnicode || !wcslen(pszUnicode))
	{
		return 0;
	}

	//计算长度
	int nLen = WideCharToMultiByte(CP_UTF8, 0, pszUnicode, -1, NULL, 0, NULL, NULL);

	//申请内存
	char* pcUtf8 = new char[nLen];

	//转换字符
	WideCharToMultiByte(CP_UTF8, 0, pszUnicode, -1, pcUtf8, nLen, NULL, NULL);

	//设置结束符
	pcUtf8[nLen-1] = 0;

	//关联对象
	sUtf8.assign(pcUtf8, nLen);
	
	//释放数组
	SafeDeleteArray(pcUtf8);

	return nLen;
}

//Unicode2转UTF8
int CWHService::Unicode2Utf8(LPCWSTR pszUnicode, LPSTR pszUtf8, int nBuffSize)
{
	//参数校验
	if (!pszUnicode || !wcslen(pszUnicode))
	{
		return 0;
	}

	//计算长度
	int nLen = WideCharToMultiByte(CP_UTF8, 0, pszUnicode, -1, NULL, 0, NULL, NULL);
	if (nLen > nBuffSize)
	{
		return 0;
	}

	//转换字符
	WideCharToMultiByte(CP_UTF8, 0, pszUnicode, -1, pszUtf8, nLen, NULL, NULL);

	//设置结束符
	pszUtf8[nLen-1] = 0;

	return nLen;
}

//UTF8转UNICODE
int CWHService::Utf82Unicode(LPCSTR pszUtf8, LPWSTR pszUnicode, int nBuffSize)
{
	//参数校验
	if (!pszUtf8 || !strlen(pszUtf8))
	{
		return 0;
	}

	//计算长度
	int nLen = MultiByteToWideChar(CP_UTF8, 0, pszUtf8, -1, NULL, 0);
	if (nLen > nBuffSize)
	{
		return 0;
	}

	//转换字符
	MultiByteToWideChar(CP_UTF8, 0, pszUtf8, -1, pszUnicode, nLen);

	//设置结束符
	pszUnicode[nLen-1] = 0;

	return nLen;
}

//UTF8转UNICODE
int CWHService::Utf82Unicode(std::string& sUtf8, LPWSTR pszUnicode, int nBuffSize)
{
	//参数校验
	if (sUtf8.empty()==false)
	{
		return 0;
	}

	//计算长度
	int nLen = MultiByteToWideChar(CP_UTF8, 0, sUtf8.data(), -1, NULL, 0);
	if (nLen > nBuffSize)
	{
		return 0;
	}

	//转换字符
	MultiByteToWideChar(CP_UTF8, 0, sUtf8.data(), -1, pszUnicode, nLen);

	//设置结束符
	pszUnicode[nLen-1] = 0;

	return nLen;
}

//格式化消息	
LPCTSTR CWHService::FormatMessage(WORD wMsgIndex, LPCTSTR pszMsg)
{
	//静态变量
	static TCHAR szBuffer[1024]={0};

	//消息格式
	LPCTSTR pszFormat = TEXT("{\"Idx\":%d,\"Msg\":\"%s\"}");

	//格式化
	_sntprintf_s(szBuffer, CountArray(szBuffer), pszFormat, wMsgIndex, pszMsg);

	return szBuffer;
}

//格式化消息	
LPCTSTR CWHService::FormatMessageV(WORD wMsgIndex, LPCTSTR pszMsgFormat, ...)
{
	//静态变量
	static TCHAR szBuffer[1024] = { 0 };

	//消息格式
	LPCTSTR pszFormat = TEXT("{\"Idx\":%d,\"Msg\":\"%s\",\"Ops\":[%s]}");

	//获取参数	
	va_list arg;

	//格式化消息	
	TCHAR szMsgBuffer[512] = { 0 };
	va_start(arg, pszMsgFormat);
	_vsntprintf_s(szMsgBuffer, CountArray(szMsgBuffer), CountArray(szMsgBuffer), pszMsgFormat, arg);
	va_end(arg);

	//提取占位符
	CString strPlaceHolder;
	ExtractPlaceHolder(pszMsgFormat, strPlaceHolder);

	//格式化选项
	TCHAR szOpsBuffer[512] = { 0 };
	va_start(arg, pszMsgFormat);
	_vsntprintf_s(szOpsBuffer, CountArray(szOpsBuffer), CountArray(szOpsBuffer), strPlaceHolder.GetString(), arg);
	va_end(arg);

	//格式化
	_sntprintf_s(szBuffer, CountArray(szBuffer), CountArray(szBuffer), pszFormat, wMsgIndex, szMsgBuffer, szOpsBuffer);

	return szBuffer;
}


//数据解码
WORD CWHService::DecodeString(LPCTSTR pszString, BYTE cbDataBuffer[], WORD wDataCount)
{
	//变量定义
	WORD wStringCount = lstrlen(pszString);

	//效验大小
	ASSERT((wStringCount % 2 == 0L) && (wStringCount / 2L <= wDataCount));
	if ((wStringCount % 2 != 0L) || (wStringCount / 2L > wDataCount)) return 0L;

	//转换数据
	for (WORD i = 0; i < wStringCount / 2L; i++)
	{
		//数据判断
		ASSERT(((pszString[i * 2L] >= TEXT('0')) && (pszString[i * 2L] <= TEXT('9'))) || ((pszString[i * 2L] >= TEXT('A')) && (pszString[i * 2L] <= TEXT('F'))));
		ASSERT(((pszString[i * 2L + 1L] >= TEXT('0')) && (pszString[i * 2L + 1L] <= TEXT('9'))) || ((pszString[i * 2L + 1L] >= TEXT('A')) && (pszString[i * 2L + 1L] <= TEXT('F'))));

		//数据判断
		if (((pszString[i * 2L] < TEXT('0')) || (pszString[i * 2L] > TEXT('9'))) && ((pszString[i * 2L] < TEXT('A')) || (pszString[i * 2L] > TEXT('F')))) return 0L;
		if (((pszString[i * 2L + 1L] < TEXT('0')) || (pszString[i * 2L + 1L] > TEXT('9'))) && ((pszString[i * 2L + 1L] < TEXT('A')) || (pszString[i * 2L + 1L] > TEXT('F')))) return 0L;

		//设置数据
		if ((pszString[i * 2L] >= TEXT('0')) && (pszString[i * 2L] <= TEXT('9'))) cbDataBuffer[i] = (BYTE)((pszString[i * 2L] - TEXT('0')) << 4L);
		if ((pszString[i * 2L] >= TEXT('A')) && (pszString[i * 2L] <= TEXT('F'))) cbDataBuffer[i] = (BYTE)((pszString[i * 2L] - TEXT('A') + 10L) << 4L);

		//设置数据
		if ((pszString[i * 2L + 1L] >= TEXT('0')) && (pszString[i * 2L + 1L] <= TEXT('9'))) cbDataBuffer[i] += (BYTE)(pszString[i * 2L + 1L] - TEXT('0'));
		if ((pszString[i * 2L + 1L] >= TEXT('A')) && (pszString[i * 2L + 1L] <= TEXT('F'))) cbDataBuffer[i] += (BYTE)(pszString[i * 2L + 1L] - TEXT('A') + 10L);
	}

	return wStringCount / 2L;
}

//字符编码
WORD CWHService::EncodeString(BYTE cbDataBuffer[], WORD wDataCount, TCHAR szString[], WORD wStringCount)
{
	//效验大小
	ASSERT((wDataCount * 2L + 1L) <= wStringCount);
	if ((wDataCount * 2L + 1L) > wStringCount) return 0L;

	//转换数据
	for (WORD i = 0; i < wDataCount; i++)
	{
		_stprintf_s(szString + i * 2L, wStringCount - i * 2L, TEXT("%02X"), cbDataBuffer[i]);
	}

	//设置终止
	szString[wDataCount * 2L] = 0L;

	return wDataCount * 2L;
}

//////////////////////////////////////////////////////////////////////////
