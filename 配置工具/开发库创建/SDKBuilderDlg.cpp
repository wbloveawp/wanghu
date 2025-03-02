#include "Stdafx.h"
#include "Direct.h"
#include "SDKBuilder.h"
#include "SDKBuilderDlg.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSDKBuilderDlg, CDialog)
	ON_BN_CLICKED(IDC_BULID, OnBnClickedBulid)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CSDKBuilderDlg::CSDKBuilderDlg() : CDialog(IDD_SDKBUILDER_DIALOG)
{
}

//控件绑定
VOID CSDKBuilderDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SDK_LIST, m_SDKFileList);
	DDX_Control(pDX, IDC_LIB_LIST, m_LIBFileList);
}

//初始化函数
BOOL CSDKBuilderDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//设置图标
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//加载参数
	TCHAR szBuffer[MAX_PATH]=TEXT("");
	TCHAR szWordDir[MAX_PATH]=TEXT("");
	TCHAR szIniFileName[MAX_PATH]=TEXT("");
	GetCurrentDirectory(sizeof(szWordDir),szWordDir);
	_sntprintf_s(szIniFileName,CountArray(szIniFileName),TEXT("%s\\Builder.INI"),szWordDir);

	GetPrivateProfileString(TEXT("FILE_PATH"), TEXT("LibPath"), TEXT(""), szBuffer, sizeof(szBuffer), szIniFileName);
	m_strLibPath = szBuffer;
	GetPrivateProfileString(TEXT("FILE_PATH"),TEXT("SourePath"),TEXT(""),szBuffer,sizeof(szBuffer),szIniFileName);
	m_strSourcePath=szBuffer;
	GetPrivateProfileString(TEXT("FILE_PATH"),TEXT("TargetPath"),TEXT(""),szBuffer,sizeof(szBuffer),szIniFileName);
	m_strTargetPath=szBuffer;
	

	//加载文件
	LoadSDKFileList();
	LoadLIBFileList();

	//初始化控件
	SetDlgItemText(IDC_SOURCE_PATH2, m_strLibPath);
	SetDlgItemText(IDC_SOURCE_PATH,m_strSourcePath);		
	SetDlgItemText(IDC_TARGER_PATH,m_strTargetPath);
	m_SDKFileList.SetBackgroundColor(FALSE,RGB(200,200,200));
	m_LIBFileList.SetBackgroundColor(FALSE,RGB(200,200,200));

	return TRUE;
}

//加载头文件
VOID CSDKBuilderDlg::LoadSDKFileList()
{
	//清理界面
	m_SDKFileList.Clear();

	//打开文件
	CFile SDKFile;
	if (SDKFile.Open(TEXT("SDKList.TXT"),CFile::modeRead,NULL)==FALSE) return;

	//读取文件
	DWORD dwFileSize=(DWORD)SDKFile.GetLength();
	if (dwFileSize>0)
	{
		//读取文件
		LPSTR pszBuffer=new CHAR [dwFileSize+1];
		SDKFile.Read(pszBuffer,dwFileSize);
		pszBuffer[dwFileSize]=0;

		//字符转换
		BYTE * pcbBuffer=(BYTE *)pszBuffer;
		bool bUnicdoe=((dwFileSize>=2)&&(pcbBuffer[0]==0xFF)&&(pcbBuffer[1]==0xFE));
		CString strBuffer=(bUnicdoe==true)?(LPCTSTR)(CW2CT((LPWSTR)(pszBuffer+2))):((LPCTSTR)CA2CT(pszBuffer));

		//插入字符
		m_SDKFileList.InsertString((LPCTSTR)strBuffer,RGB(0,0,0));

		//删除内存
		SafeDelete(pszBuffer);
	}

	return;
}

//加载链接库
VOID CSDKBuilderDlg::LoadLIBFileList()
{
	//清理界面
	m_LIBFileList.Clear();

	//打开文件
	CFile LIBFile;
	if (LIBFile.Open(TEXT("LIBList.TXT"),CFile::modeRead,NULL)==FALSE) return;

	//读取文件
	DWORD dwFileSize=(DWORD)LIBFile.GetLength();
	if (dwFileSize>0)
	{
		//读取文件
		LPSTR pszBuffer=new CHAR [dwFileSize+1];
		LIBFile.Read(pszBuffer,dwFileSize);
		pszBuffer[dwFileSize]=0;

		//字符转换
		BYTE * pcbBuffer=(BYTE *)pszBuffer;
		bool bUnicdoe=((dwFileSize>=2)&&(pcbBuffer[0]==0xFF)&&(pcbBuffer[1]==0xFE));
		CString strBuffer=(bUnicdoe==true)?(LPCTSTR)(CW2CT((LPWSTR)(pszBuffer+2))):((LPCTSTR)CA2CT(pszBuffer));

		//插入字符
		m_LIBFileList.InsertString((LPCTSTR)strBuffer,RGB(0,0,0));

		//删除内存
		SafeDelete(pszBuffer);
	}

	return;
}

//创建目录
VOID CSDKBuilderDlg::CreateDirectory(LPCTSTR pszDirectory)
{
	//变量定义
	INT nExcursion=0;
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	lstrcpyn(szDirectory,pszDirectory,CountArray(szDirectory));

	//创建目录
	do
	{
		if (szDirectory[nExcursion]==0) 
		{
			::CreateDirectory(szDirectory,NULL);
			break;
		}
		if (szDirectory[nExcursion]==TEXT('\\'))
		{
			szDirectory[nExcursion]=0;
			::CreateDirectory(szDirectory,NULL);
			szDirectory[nExcursion]=TEXT('\\');
		}
		nExcursion++;

	} while (true);

	return;
}

//转换文件
bool CSDKBuilderDlg::ConversionSDKFile(LPCTSTR pszSDKFile)
{
	//变量定义
	CString strBuffer;
	CFile SourceFile,TargetFile;

	//打开原文件
	if (SourceFile.Open(pszSDKFile,CFile::modeRead,NULL)==FALSE)
	{
		strBuffer.Format(TEXT("“%s”文件读取失败！"),pszSDKFile);
		AfxMessageBox(strBuffer,MB_ICONINFORMATION);
		return false;
	}

	//打开目标文件
	TCHAR szTargerName[MAX_PATH]=TEXT("");
	_sntprintf_s(szTargerName,CountArray(szTargerName),TEXT("%s\\Include\\%s"),m_strTargetPath.GetString(),SourceFile.GetFileName().GetString());
	if (TargetFile.Open(szTargerName,CFile::modeCreate|CFile::modeWrite,NULL)==FALSE)
	{
		strBuffer.Format(TEXT("“%s”文件写入失败！"),szTargerName);
		AfxMessageBox(strBuffer,MB_ICONINFORMATION);
		return false;
	}

	//读取文件
	CStringA strFileData;
	DWORD dwFileLength=(DWORD)SourceFile.GetLength();
	SourceFile.Read(strFileData.GetBufferSetLength(dwFileLength),dwFileLength);
	strFileData.ReleaseBuffer();

	//变量定义
	DWORD dwResultIndex=0;
	DWORD dwCurrentIndex=0;
	DWORD dwIncludeLength=lstrlen(TEXT("#include \""));

	//缓冲定义
	LPCSTR pszSourceData=(LPCSTR)strFileData;
	LPSTR pszResultData=new CHAR [dwFileLength];

	//转换文件
	do
	{
		if ((pszSourceData[dwCurrentIndex]=='#')&&((dwCurrentIndex+dwIncludeLength)<dwFileLength)
			&&(memcmp(pszSourceData+dwCurrentIndex, "#include \"",sizeof(CHAR)*dwIncludeLength)==0))
		{
			//变量定义
			DWORD dwEndIndex=0;
			DWORD dwStartIndex=dwIncludeLength;

			//查找开始
			for (DWORD i=dwIncludeLength;i<dwFileLength;i++)
			{
				if (pszSourceData[dwCurrentIndex+i]=='\\')
				{
					dwStartIndex=i+1;
					continue;
				}
				if (pszSourceData[dwCurrentIndex+i]=='\"')
				{
					dwEndIndex=i;
					break;
				}
			}

			//结果判断
			if (dwEndIndex!=0)
			{
				//字符串头
				CopyMemory(pszResultData+dwResultIndex,"#include \"",sizeof(CHAR)*dwIncludeLength);
				dwResultIndex+=dwIncludeLength;

				//包含文件
				CopyMemory(pszResultData+dwResultIndex,pszSourceData+dwCurrentIndex+dwStartIndex,sizeof(CHAR)*(dwEndIndex-dwStartIndex));
				dwResultIndex+=(dwEndIndex-dwStartIndex);

				//设置索引
				dwCurrentIndex+=dwEndIndex;
			}
			else
			{
				ASSERT(FALSE);
				pszResultData[dwResultIndex++]=pszSourceData[dwCurrentIndex++];
			}
		}
		else 
		{
			pszResultData[dwResultIndex++]=pszSourceData[dwCurrentIndex++];
		}

		//完成判断
		if (dwCurrentIndex>=dwFileLength)
		{
			ASSERT(dwCurrentIndex==dwFileLength);
			break;
		}

	} while (true);

	//写入文件
	TargetFile.Write(pszResultData,dwResultIndex);

	//关闭文件
	TargetFile.Close();
	SourceFile.Close();

	//清理资源
	SafeDelete(pszResultData);

	return true;
}

//拷贝文件
bool CSDKBuilderDlg::CopyBinaryFile(LPCTSTR pszSourceFile, LPCTSTR pszTargetPath)
{
	//变量定义
	CFile SourceFile,TargetFile;

	//打开文件
	if (SourceFile.Open(pszSourceFile,CFile::modeRead,NULL)==FALSE)
	{
		CString strBuffer;
		strBuffer.Format(TEXT("“%s”文件读取失败！"),pszSourceFile);
		AfxMessageBox(strBuffer,MB_ICONINFORMATION);
		return false;
	}

	//打开文件
	TCHAR szTargerName[MAX_PATH]=TEXT("");
	_sntprintf_s(szTargerName,CountArray(szTargerName),TEXT("%s\\%s"),pszTargetPath,SourceFile.GetFileName().GetString());
	if (TargetFile.Open(szTargerName,CFile::modeCreate|CFile::modeWrite,NULL)==FALSE)
	{
		CString strBuffer;
		strBuffer.Format(TEXT("“%s”文件写入失败！"),szTargerName);
		AfxMessageBox(strBuffer,MB_ICONINFORMATION);
		return false;
	}

	//拷贝文件
	UINT uReadCount=0;
	BYTE szBuffer[10240];
	do
	{
		uReadCount=SourceFile.Read(szBuffer,sizeof(szBuffer));
		if (uReadCount==0) break;
		TargetFile.Write(szBuffer,uReadCount);
	} while (true);

	//关闭文件
	TargetFile.Close();
	SourceFile.Close();

	return true;
}

//生成命令
VOID CSDKBuilderDlg::OnBnClickedBulid()
{
	//变量定义
	TCHAR szFileName[MAX_PATH]=TEXT("");
	CString strPath,strTemp,strSourceFile;

	//创建目录
	strPath.Format(TEXT("%s\\Lib"),m_strTargetPath);
	CreateDirectory(strPath);

	strPath.Format(TEXT("%s\\Include"),m_strTargetPath);
	CreateDirectory(strPath);

	//获取头文件
	for (INT i=0;i<m_SDKFileList.GetLineCount();i++)
	{
		if (m_SDKFileList.GetLine(i,szFileName, CountArray(szFileName))==0) continue;
		if ((szFileName[0]==0)||(szFileName[0]==TEXT('\r'))||(szFileName[0]==TEXT('\n'))) continue;
		for (INT j=0;j<MAX_PATH;j++) 
		{
			if (szFileName[j]==0) break;
			if ((szFileName[j]==TEXT('\r'))||(szFileName[j]==TEXT('\n')))
			{
				szFileName[j]=0;
				break;
			}
		}
		strSourceFile.Format(TEXT("%s\\%s"),m_strSourcePath,szFileName);
		if (ConversionSDKFile(strSourceFile)==false) return;
	}

	//获取文件
	CString strAnsiLibTargetPath,strUniLibTargetPath;

	//6603
	strAnsiLibTargetPath.Format(TEXT("%s\\Lib"),m_strTargetPath);

	//拷贝文件
	for (INT i=0;i<m_LIBFileList.GetLineCount();i++)
	{
		if (m_LIBFileList.GetLine(i,szFileName,CountArray(szFileName))==0) continue;
		if ((szFileName[0]==0)||(szFileName[0]==TEXT('\r'))||(szFileName[0]==TEXT('\n'))) continue;
		for (INT j=0;j<MAX_PATH;j++)
		{
			if (szFileName[j]==0) break;
			if ((szFileName[j]==TEXT('\r'))||(szFileName[j]==TEXT('\n')))
			{
				szFileName[j]=0;
				break;
			}
		}
		strSourceFile.Format(TEXT("%s\\%s"),m_strLibPath,szFileName);
		if (CopyBinaryFile(strSourceFile,strAnsiLibTargetPath)==false) return;
	}

	//转化完成
	AfxMessageBox(TEXT("文件转化完成！"),MB_ICONINFORMATION);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
