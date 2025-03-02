#include "StdAfx.h"
#include <time.h> 
#include "WHService.h"
#include "DataEncrypt.h"

//////////////////////////////////////////////////////////////////////////

//��������
#pragma comment(lib,"Version")
#pragma comment(lib,"Iphlpapi")

#pragma warning (disable : 4996)

//////////////////////////////////////////////////////////////////////////

//״̬��Ϣ
struct tagAstatInfo
{
	ADAPTER_STATUS					AdapterStatus;						//����״̬
	NAME_BUFFER						NameBuff[16];						//���ֻ���
};

//////////////////////////////////////////////////////////////////////////

//У��ռλ��
bool VerifyPlaceHolder(LPCTSTR pszString, int nStartIndex, int nEndIndex)
{
	//����У��
	ASSERT(lstrlen(pszString) > nEndIndex);
	if (lstrlen(pszString) <= nEndIndex) return false;

	//����У��
	ASSERT(pszString != NULL && nEndIndex > nStartIndex);
	if (pszString == NULL || nEndIndex <= nStartIndex) return false;

	//��������
	const TCHAR szFlags[] = { TEXT('-'), TEXT('+'), TEXT(' '), TEXT('#'), TEXT('0') };
	const TCHAR szDecorate[] = { TEXT('h'), TEXT('j'), TEXT('l'), TEXT('L'), TEXT('t'), TEXT('z'), TEXT('F'), TEXT('N'), TEXT('I') };

	//���Ͷ���
	enum emFormatOption
	{
		FORMAT_OP_NONE = 0,
		FORMAT_OP_FLAGS,
		FORMAT_OP_WIDTH,
		FORMAT_OP_PREC,
		FORMAT_OP_DECORATE
	};

	//��������
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

//��ȡռλ��
VOID ExtractPlaceHolder(LPCTSTR pszString, CString& strPlaceHolder)
{
	//ռλ��
	const TCHAR szHolderList[] = { TEXT('a'), TEXT('A'), TEXT('c'), TEXT('d'), TEXT('e'), TEXT('E'), TEXT('f'), TEXT('G'), TEXT('i'), TEXT('o'), TEXT('p'), TEXT('s'), TEXT('u'), TEXT('x'), TEXT('X') };

	//�Ƴ�����
	strPlaceHolder.Empty();

	//��������
	LPCTSTR pszMove = pszString;

	//�����ַ���
	while (*pszMove != TEXT('\0'))
	{
		if (*pszMove == TEXT('%'))
		{
			//ת�崦��
			if (*(pszMove + 1) == TEXT('%'))
			{
				pszMove = pszMove + 2;
				continue;
			}

			//��������
			bool bExit = false;
			auto pszNewMove = (pszMove + 1);

			//��������			
			int nEndIndex = -1;
			int nStartIndex = pszMove - pszString;

			//����ռλ��
			while (*pszNewMove != TEXT('\0'))
			{
				for (auto chr : szHolderList)
				{
					if (*pszNewMove != chr) continue;

					//���ñ���
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

//���캯��
CWHService::CWHService()
{
}

//��ȡ������
bool CWHService::GetMachineID(TCHAR szMachineID[LEN_MACHINE_ID])
{
	//��������
	TCHAR szNetWorkID[LEN_NETWORK_ID]=TEXT("");

	//������ʶ
	GetNetWorkID(szNetWorkID);

	//ת����Ϣ
	TCHAR szMD5Machine[LEN_MD5] = {0};
	CMD5Encrypt::EncryptData(szNetWorkID,szMD5Machine);

	//���ƽ��
	StringCchCopy(szMachineID, LEN_MACHINE_ID, szMD5Machine);

	return true;
}

//������ַ
bool CWHService::GetNetWorkID(TCHAR szNetWorkID[LEN_NETWORK_ID])
{
	CWHBuffer<BYTE,sizeof(IP_ADAPTER_INFO)> AdapterBuffer;
	ULONG uBufferLength=AdapterBuffer.GetBufferSize();

	//�����ڴ�
	if (GetAdaptersInfo(NULL,&uBufferLength)==ERROR_BUFFER_OVERFLOW)
	{
		AdapterBuffer.CreateBuffer(uBufferLength);
	}

	//��ѯ����
	if ((GetAdaptersInfo((IP_ADAPTER_INFO *)AdapterBuffer.GetData(),&uBufferLength))==NO_ERROR)
	{
		//��������
		BYTE cbOutBuffer[16];		
		IP_ADAPTER_INFO * pEthernetAdapter=NULL;
		IP_ADAPTER_INFO * pPhysicalAdapter=NULL;
		IP_ADAPTER_INFO * pWirelessAdapter=NULL;
		IP_ADAPTER_INFO * pAdapterInfo=(IP_ADAPTER_INFO *)AdapterBuffer.GetData();

		//��������
		while (pAdapterInfo!=NULL)
		{
			//��̫����
			if (pAdapterInfo->Type==MIB_IF_TYPE_ETHERNET)
			{		
				//��������
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

			//��������
			if (pAdapterInfo->Type==71)
			{				
				pWirelessAdapter=pAdapterInfo;
			}

			//��������
			pAdapterInfo=pAdapterInfo->Next;
		}

		//�����������ȡ�����������֮
		if(pWirelessAdapter!=NULL) pEthernetAdapter=pWirelessAdapter;
		if(pPhysicalAdapter!=NULL) pEthernetAdapter=pPhysicalAdapter;
		if(pEthernetAdapter!=NULL)
		{
			//������ʶ
			TCHAR szDeviceName[MAX_PATH]=TEXT("");
			_sntprintf(szDeviceName,CountArray(szDeviceName),TEXT("//./%s"),(LPCTSTR)(CA2T(pEthernetAdapter->AdapterName)));

			//���豸
			HANDLE hDevice=CreateFile(szDeviceName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0L,NULL);

			//�����ж�
			if (hDevice==INVALID_HANDLE_VALUE)
			{
				ASSERT(FALSE);			
				return true;
			}

			//��ȡ��ַ
			DWORD dwReturned=0L;
			DWORD dwInputBuffer=OID_802_3_PERMANENT_ADDRESS;
			if (DeviceIoControl(hDevice,IOCTL_NDIS_QUERY_GLOBAL_STATS,&dwInputBuffer,sizeof(dwInputBuffer),cbOutBuffer,sizeof(cbOutBuffer),&dwReturned,NULL)!=FALSE)
			{
				_sntprintf(szNetWorkID,LEN_NETWORK_ID,TEXT("%02X%02X%02X%02X%02X%02X"),cbOutBuffer[0],cbOutBuffer[1],cbOutBuffer[2],cbOutBuffer[3],cbOutBuffer[4],cbOutBuffer[5]);
				return true;
			}
		}
	}

	//�������
	ASSERT(FALSE);

	//����Ĭ��
	ZeroMemory(szNetWorkID,sizeof(TCHAR)*LEN_NETWORK_ID);

	return false;
}

//�����ַ�
bool CWHService::CopyToClipBoard(LPCTSTR pszString)
{
	//��������
	HWND hWnd=AfxGetMainWnd()->GetSafeHwnd();

	//�򿪼��а�
	if (OpenClipboard(hWnd)==FALSE) return false;
	if (EmptyClipboard()==FALSE) { CloseClipboard(); return false; }

	//��������
	HANDLE hData=GlobalAlloc(GMEM_MOVEABLE|GMEM_ZEROINIT,CountStringBuffer(pszString));
	if (hData==NULL) 
	{
		CloseClipboard();
		return false;
	}

	//��������
	CT2CA strString(pszString);
	LPSTR pszMemString=(LPSTR)GlobalLock(hData);
	strcpy(pszMemString,(LPCSTR)strString);
	SetClipboardData(CF_TEXT,hData);

	//��������
	GlobalUnlock(hData);
	CloseClipboard();

	return true;
}

//ע���ȼ�
bool CWHService::RegisterHotKey(HWND hWnd, UINT uKeyID, WORD wHotKey)
{
	//��������
	BYTE cbModifiers=0;
	if (HIBYTE(wHotKey)&HOTKEYF_ALT) cbModifiers|=MOD_ALT;
	if (HIBYTE(wHotKey)&HOTKEYF_SHIFT) cbModifiers|=MOD_SHIFT;
	if (HIBYTE(wHotKey)&HOTKEYF_CONTROL) cbModifiers|=MOD_CONTROL;

	//ע���ȼ�
	BOOL bSuccess=::RegisterHotKey(hWnd,uKeyID,cbModifiers,LOBYTE(wHotKey));

	return (bSuccess==TRUE)?true:false;
}

//��������
bool CWHService::GetProcessName(TCHAR szProcessName[], WORD wBufferCount)
{
	//ģ��·��
	TCHAR szModulePath[MAX_PATH] = TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(), szModulePath, CountArray(szModulePath));

	//�����ļ�
	for (INT i = lstrlen(szModulePath); i >= 0; i--)
	{
		//ȥ����׺
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

//����Ŀ¼
bool CWHService::GetWorkDirectory(TCHAR szWorkDirectory[], WORD wBufferCount)
{
	//ģ��·��
	TCHAR szModulePath[MAX_PATH]=TEXT("");
	GetModuleFileName(AfxGetInstanceHandle(),szModulePath,CountArray(szModulePath));

	//�����ļ�
	for (INT i=lstrlen(szModulePath);i>=0;i--)
	{
		if (szModulePath[i]==TEXT('\\'))
		{
			szModulePath[i]=0;
			break;
		}
	}

	//���ý��
	ASSERT(szModulePath[0]!=0);
	StringCchCopy(szWorkDirectory,wBufferCount, szModulePath);

	return true;
}

//�ļ��汾
bool CWHService::GetModuleVersion(LPCTSTR pszModuleName, DWORD & dwVersionInfo)
{
	//���ý��
	dwVersionInfo=0L;

	//���ջ���
	BYTE cbInfoBuffer[1024];
	ZeroMemory(cbInfoBuffer,sizeof(cbInfoBuffer));

	//ģ����Ϣ
  DWORD dwFileHandle=NULL;
	if (GetFileVersionInfo(pszModuleName,dwFileHandle,sizeof(cbInfoBuffer),cbInfoBuffer)==FALSE) return false;

	//��ȡ��Ϣ
	UINT uQuerySize=0;
	VS_FIXEDFILEINFO * pFixedFileInfo=NULL;
  if (VerQueryValue(cbInfoBuffer,TEXT("\\"),(VOID * *)&pFixedFileInfo,&uQuerySize)==FALSE) return false;

	//���ý��
	if ((pFixedFileInfo!=NULL)&&(uQuerySize==sizeof(VS_FIXEDFILEINFO)))
	{
		//���ð汾
		WORD wVersion1=HIWORD(pFixedFileInfo->dwFileVersionMS);
		WORD wVersion2=LOWORD(pFixedFileInfo->dwFileVersionMS);
		WORD wVersion3=HIWORD(pFixedFileInfo->dwFileVersionLS);
		WORD wVersion4=LOWORD(pFixedFileInfo->dwFileVersionLS);
		dwVersionInfo=MAKELONG(MAKEWORD(wVersion4,wVersion3),MAKEWORD(wVersion2,wVersion1));

		return true;
	}

	return false;
}

//AsciiתUTF8
int CWHService::GB23122Utf8(LPCSTR pszGB2312, std::string& sUtf8)
{
	//���㳤��
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, pszGB2312, -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen];
	ZeroMemory(pwBuf, nwLen * sizeof(wchar_t));

	//���㳤��
	::MultiByteToWideChar(CP_ACP, 0, pszGB2312, lstrlenA(pszGB2312), pwBuf, nwLen);
	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char* pBuf = new char[nLen];
	ZeroMemory(pBuf, nLen);

	//ת������
	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	//��ӽ�����
	pBuf[nLen-1] = 0;

	//������
	sUtf8.assign(pBuf);

	//�ͷ���Դ
	SafeDeleteArray(pwBuf);
	SafeDeleteArray(pBuf);

	return nLen;	
}

//AsciiתUTF8
int CWHService::GB23122Utf8(LPCSTR pszGB2312, LPSTR pszUtf8, int nBuffSize)
{
	//���㳤��
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, pszGB2312, -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen];
	ZeroMemory(pwBuf, nwLen * sizeof(wchar_t));

	//���㳤��
	::MultiByteToWideChar(CP_ACP, 0, pszGB2312, lstrlenA(pszGB2312), pwBuf, nwLen);
	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	if (nLen > nBuffSize)
	{
		return 0;
	}

	//ת������
	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pszUtf8, nLen, NULL, NULL);

	//��ӽ�����
	pszUtf8[nLen - 1] = 0;

	//�ͷ���Դ
	SafeDeleteArray(pwBuf);

	return nLen;	
}


//UTF8תAscii
int CWHService::Utf82GB2312(LPCSTR pszUtf8, std::string& sAscii)
{
	//���㳤��
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, pszUtf8, -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen];
	ZeroMemory(pwBuf, nwLen * sizeof(wchar_t));

	//���㳤��
	MultiByteToWideChar(CP_UTF8, 0, pszUtf8, lstrlenA(pszUtf8), pwBuf, nwLen);
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	char* pBuf = new char[nLen];
	ZeroMemory(pBuf, nLen);

	//����ת��
	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	//��ӽ�����
	pBuf[nLen-1] = 0;

	//������
	sAscii.assign(pBuf, nLen);

	//�ͷ���Դ
	SafeDeleteArray(pwBuf);
	SafeDeleteArray(pBuf);

	return nLen;
}

//UTF8תAscii
int CWHService::Utf82GB2312(LPCSTR pszUtf8, LPSTR pszGB2312, int nBuffSize)
{
	//���㳤��
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, pszUtf8, -1, NULL, 0);
	wchar_t* pwBuf = new wchar_t[nwLen];
	ZeroMemory(pwBuf, nwLen * sizeof(wchar_t));

	//���㳤��
	MultiByteToWideChar(CP_UTF8, 0, pszUtf8, lstrlenA(pszUtf8), pwBuf, nwLen);
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
	if (nLen > nBuffSize)
	{
		return 0;
	}

	//����ת��
	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pszGB2312, nLen, NULL, NULL);

	//��ӽ�����
	pszGB2312[nLen - 1] = 0;

	//�ͷ���Դ
	SafeDeleteArray(pwBuf);

	return nLen;
}


//Unicode2תUTF8
int CWHService::Unicode2Utf8(LPCWSTR pszUnicode, std::string& sUtf8)
{
	//����У��
	if (!pszUnicode || !wcslen(pszUnicode))
	{
		return 0;
	}

	//���㳤��
	int nLen = WideCharToMultiByte(CP_UTF8, 0, pszUnicode, -1, NULL, 0, NULL, NULL);

	//�����ڴ�
	char* pcUtf8 = new char[nLen];

	//ת���ַ�
	WideCharToMultiByte(CP_UTF8, 0, pszUnicode, -1, pcUtf8, nLen, NULL, NULL);

	//���ý�����
	pcUtf8[nLen-1] = 0;

	//��������
	sUtf8.assign(pcUtf8, nLen);
	
	//�ͷ�����
	SafeDeleteArray(pcUtf8);

	return nLen;
}

//Unicode2תUTF8
int CWHService::Unicode2Utf8(LPCWSTR pszUnicode, LPSTR pszUtf8, int nBuffSize)
{
	//����У��
	if (!pszUnicode || !wcslen(pszUnicode))
	{
		return 0;
	}

	//���㳤��
	int nLen = WideCharToMultiByte(CP_UTF8, 0, pszUnicode, -1, NULL, 0, NULL, NULL);
	if (nLen > nBuffSize)
	{
		return 0;
	}

	//ת���ַ�
	WideCharToMultiByte(CP_UTF8, 0, pszUnicode, -1, pszUtf8, nLen, NULL, NULL);

	//���ý�����
	pszUtf8[nLen-1] = 0;

	return nLen;
}

//UTF8תUNICODE
int CWHService::Utf82Unicode(LPCSTR pszUtf8, LPWSTR pszUnicode, int nBuffSize)
{
	//����У��
	if (!pszUtf8 || !strlen(pszUtf8))
	{
		return 0;
	}

	//���㳤��
	int nLen = MultiByteToWideChar(CP_UTF8, 0, pszUtf8, -1, NULL, 0);
	if (nLen > nBuffSize)
	{
		return 0;
	}

	//ת���ַ�
	MultiByteToWideChar(CP_UTF8, 0, pszUtf8, -1, pszUnicode, nLen);

	//���ý�����
	pszUnicode[nLen-1] = 0;

	return nLen;
}

//UTF8תUNICODE
int CWHService::Utf82Unicode(std::string& sUtf8, LPWSTR pszUnicode, int nBuffSize)
{
	//����У��
	if (sUtf8.empty()==false)
	{
		return 0;
	}

	//���㳤��
	int nLen = MultiByteToWideChar(CP_UTF8, 0, sUtf8.data(), -1, NULL, 0);
	if (nLen > nBuffSize)
	{
		return 0;
	}

	//ת���ַ�
	MultiByteToWideChar(CP_UTF8, 0, sUtf8.data(), -1, pszUnicode, nLen);

	//���ý�����
	pszUnicode[nLen-1] = 0;

	return nLen;
}

//��ʽ����Ϣ	
LPCTSTR CWHService::FormatMessage(WORD wMsgIndex, LPCTSTR pszMsg)
{
	//��̬����
	static TCHAR szBuffer[1024]={0};

	//��Ϣ��ʽ
	LPCTSTR pszFormat = TEXT("{\"Idx\":%d,\"Msg\":\"%s\"}");

	//��ʽ��
	_sntprintf_s(szBuffer, CountArray(szBuffer), pszFormat, wMsgIndex, pszMsg);

	return szBuffer;
}

//��ʽ����Ϣ	
LPCTSTR CWHService::FormatMessageV(WORD wMsgIndex, LPCTSTR pszMsgFormat, ...)
{
	//��̬����
	static TCHAR szBuffer[1024] = { 0 };

	//��Ϣ��ʽ
	LPCTSTR pszFormat = TEXT("{\"Idx\":%d,\"Msg\":\"%s\",\"Ops\":[%s]}");

	//��ȡ����	
	va_list arg;

	//��ʽ����Ϣ	
	TCHAR szMsgBuffer[512] = { 0 };
	va_start(arg, pszMsgFormat);
	_vsntprintf_s(szMsgBuffer, CountArray(szMsgBuffer), CountArray(szMsgBuffer), pszMsgFormat, arg);
	va_end(arg);

	//��ȡռλ��
	CString strPlaceHolder;
	ExtractPlaceHolder(pszMsgFormat, strPlaceHolder);

	//��ʽ��ѡ��
	TCHAR szOpsBuffer[512] = { 0 };
	va_start(arg, pszMsgFormat);
	_vsntprintf_s(szOpsBuffer, CountArray(szOpsBuffer), CountArray(szOpsBuffer), strPlaceHolder.GetString(), arg);
	va_end(arg);

	//��ʽ��
	_sntprintf_s(szBuffer, CountArray(szBuffer), CountArray(szBuffer), pszFormat, wMsgIndex, szMsgBuffer, szOpsBuffer);

	return szBuffer;
}


//���ݽ���
WORD CWHService::DecodeString(LPCTSTR pszString, BYTE cbDataBuffer[], WORD wDataCount)
{
	//��������
	WORD wStringCount = lstrlen(pszString);

	//Ч���С
	ASSERT((wStringCount % 2 == 0L) && (wStringCount / 2L <= wDataCount));
	if ((wStringCount % 2 != 0L) || (wStringCount / 2L > wDataCount)) return 0L;

	//ת������
	for (WORD i = 0; i < wStringCount / 2L; i++)
	{
		//�����ж�
		ASSERT(((pszString[i * 2L] >= TEXT('0')) && (pszString[i * 2L] <= TEXT('9'))) || ((pszString[i * 2L] >= TEXT('A')) && (pszString[i * 2L] <= TEXT('F'))));
		ASSERT(((pszString[i * 2L + 1L] >= TEXT('0')) && (pszString[i * 2L + 1L] <= TEXT('9'))) || ((pszString[i * 2L + 1L] >= TEXT('A')) && (pszString[i * 2L + 1L] <= TEXT('F'))));

		//�����ж�
		if (((pszString[i * 2L] < TEXT('0')) || (pszString[i * 2L] > TEXT('9'))) && ((pszString[i * 2L] < TEXT('A')) || (pszString[i * 2L] > TEXT('F')))) return 0L;
		if (((pszString[i * 2L + 1L] < TEXT('0')) || (pszString[i * 2L + 1L] > TEXT('9'))) && ((pszString[i * 2L + 1L] < TEXT('A')) || (pszString[i * 2L + 1L] > TEXT('F')))) return 0L;

		//��������
		if ((pszString[i * 2L] >= TEXT('0')) && (pszString[i * 2L] <= TEXT('9'))) cbDataBuffer[i] = (BYTE)((pszString[i * 2L] - TEXT('0')) << 4L);
		if ((pszString[i * 2L] >= TEXT('A')) && (pszString[i * 2L] <= TEXT('F'))) cbDataBuffer[i] = (BYTE)((pszString[i * 2L] - TEXT('A') + 10L) << 4L);

		//��������
		if ((pszString[i * 2L + 1L] >= TEXT('0')) && (pszString[i * 2L + 1L] <= TEXT('9'))) cbDataBuffer[i] += (BYTE)(pszString[i * 2L + 1L] - TEXT('0'));
		if ((pszString[i * 2L + 1L] >= TEXT('A')) && (pszString[i * 2L + 1L] <= TEXT('F'))) cbDataBuffer[i] += (BYTE)(pszString[i * 2L + 1L] - TEXT('A') + 10L);
	}

	return wStringCount / 2L;
}

//�ַ�����
WORD CWHService::EncodeString(BYTE cbDataBuffer[], WORD wDataCount, TCHAR szString[], WORD wStringCount)
{
	//Ч���С
	ASSERT((wDataCount * 2L + 1L) <= wStringCount);
	if ((wDataCount * 2L + 1L) > wStringCount) return 0L;

	//ת������
	for (WORD i = 0; i < wDataCount; i++)
	{
		_stprintf_s(szString + i * 2L, wStringCount - i * 2L, TEXT("%02X"), cbDataBuffer[i]);
	}

	//������ֹ
	szString[wDataCount * 2L] = 0L;

	return wDataCount * 2L;
}

//////////////////////////////////////////////////////////////////////////
