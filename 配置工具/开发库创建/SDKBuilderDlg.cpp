#include "Stdafx.h"
#include "Direct.h"
#include "SDKBuilder.h"
#include "SDKBuilderDlg.h"

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CSDKBuilderDlg, CDialog)
	ON_BN_CLICKED(IDC_BULID, OnBnClickedBulid)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//���캯��
CSDKBuilderDlg::CSDKBuilderDlg() : CDialog(IDD_SDKBUILDER_DIALOG)
{
}

//�ؼ���
VOID CSDKBuilderDlg::DoDataExchange(CDataExchange * pDX)
{
	__super::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SDK_LIST, m_SDKFileList);
	DDX_Control(pDX, IDC_LIB_LIST, m_LIBFileList);
}

//��ʼ������
BOOL CSDKBuilderDlg::OnInitDialog()
{
	__super::OnInitDialog();

	//����ͼ��
	HICON hIcon=LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon,TRUE);
	SetIcon(hIcon,FALSE);

	//���ز���
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
	

	//�����ļ�
	LoadSDKFileList();
	LoadLIBFileList();

	//��ʼ���ؼ�
	SetDlgItemText(IDC_SOURCE_PATH2, m_strLibPath);
	SetDlgItemText(IDC_SOURCE_PATH,m_strSourcePath);		
	SetDlgItemText(IDC_TARGER_PATH,m_strTargetPath);
	m_SDKFileList.SetBackgroundColor(FALSE,RGB(200,200,200));
	m_LIBFileList.SetBackgroundColor(FALSE,RGB(200,200,200));

	return TRUE;
}

//����ͷ�ļ�
VOID CSDKBuilderDlg::LoadSDKFileList()
{
	//�������
	m_SDKFileList.Clear();

	//���ļ�
	CFile SDKFile;
	if (SDKFile.Open(TEXT("SDKList.TXT"),CFile::modeRead,NULL)==FALSE) return;

	//��ȡ�ļ�
	DWORD dwFileSize=(DWORD)SDKFile.GetLength();
	if (dwFileSize>0)
	{
		//��ȡ�ļ�
		LPSTR pszBuffer=new CHAR [dwFileSize+1];
		SDKFile.Read(pszBuffer,dwFileSize);
		pszBuffer[dwFileSize]=0;

		//�ַ�ת��
		BYTE * pcbBuffer=(BYTE *)pszBuffer;
		bool bUnicdoe=((dwFileSize>=2)&&(pcbBuffer[0]==0xFF)&&(pcbBuffer[1]==0xFE));
		CString strBuffer=(bUnicdoe==true)?(LPCTSTR)(CW2CT((LPWSTR)(pszBuffer+2))):((LPCTSTR)CA2CT(pszBuffer));

		//�����ַ�
		m_SDKFileList.InsertString((LPCTSTR)strBuffer,RGB(0,0,0));

		//ɾ���ڴ�
		SafeDelete(pszBuffer);
	}

	return;
}

//�������ӿ�
VOID CSDKBuilderDlg::LoadLIBFileList()
{
	//�������
	m_LIBFileList.Clear();

	//���ļ�
	CFile LIBFile;
	if (LIBFile.Open(TEXT("LIBList.TXT"),CFile::modeRead,NULL)==FALSE) return;

	//��ȡ�ļ�
	DWORD dwFileSize=(DWORD)LIBFile.GetLength();
	if (dwFileSize>0)
	{
		//��ȡ�ļ�
		LPSTR pszBuffer=new CHAR [dwFileSize+1];
		LIBFile.Read(pszBuffer,dwFileSize);
		pszBuffer[dwFileSize]=0;

		//�ַ�ת��
		BYTE * pcbBuffer=(BYTE *)pszBuffer;
		bool bUnicdoe=((dwFileSize>=2)&&(pcbBuffer[0]==0xFF)&&(pcbBuffer[1]==0xFE));
		CString strBuffer=(bUnicdoe==true)?(LPCTSTR)(CW2CT((LPWSTR)(pszBuffer+2))):((LPCTSTR)CA2CT(pszBuffer));

		//�����ַ�
		m_LIBFileList.InsertString((LPCTSTR)strBuffer,RGB(0,0,0));

		//ɾ���ڴ�
		SafeDelete(pszBuffer);
	}

	return;
}

//����Ŀ¼
VOID CSDKBuilderDlg::CreateDirectory(LPCTSTR pszDirectory)
{
	//��������
	INT nExcursion=0;
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	lstrcpyn(szDirectory,pszDirectory,CountArray(szDirectory));

	//����Ŀ¼
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

//ת���ļ�
bool CSDKBuilderDlg::ConversionSDKFile(LPCTSTR pszSDKFile)
{
	//��������
	CString strBuffer;
	CFile SourceFile,TargetFile;

	//��ԭ�ļ�
	if (SourceFile.Open(pszSDKFile,CFile::modeRead,NULL)==FALSE)
	{
		strBuffer.Format(TEXT("��%s���ļ���ȡʧ�ܣ�"),pszSDKFile);
		AfxMessageBox(strBuffer,MB_ICONINFORMATION);
		return false;
	}

	//��Ŀ���ļ�
	TCHAR szTargerName[MAX_PATH]=TEXT("");
	_sntprintf_s(szTargerName,CountArray(szTargerName),TEXT("%s\\Include\\%s"),m_strTargetPath.GetString(),SourceFile.GetFileName().GetString());
	if (TargetFile.Open(szTargerName,CFile::modeCreate|CFile::modeWrite,NULL)==FALSE)
	{
		strBuffer.Format(TEXT("��%s���ļ�д��ʧ�ܣ�"),szTargerName);
		AfxMessageBox(strBuffer,MB_ICONINFORMATION);
		return false;
	}

	//��ȡ�ļ�
	CStringA strFileData;
	DWORD dwFileLength=(DWORD)SourceFile.GetLength();
	SourceFile.Read(strFileData.GetBufferSetLength(dwFileLength),dwFileLength);
	strFileData.ReleaseBuffer();

	//��������
	DWORD dwResultIndex=0;
	DWORD dwCurrentIndex=0;
	DWORD dwIncludeLength=lstrlen(TEXT("#include \""));

	//���嶨��
	LPCSTR pszSourceData=(LPCSTR)strFileData;
	LPSTR pszResultData=new CHAR [dwFileLength];

	//ת���ļ�
	do
	{
		if ((pszSourceData[dwCurrentIndex]=='#')&&((dwCurrentIndex+dwIncludeLength)<dwFileLength)
			&&(memcmp(pszSourceData+dwCurrentIndex, "#include \"",sizeof(CHAR)*dwIncludeLength)==0))
		{
			//��������
			DWORD dwEndIndex=0;
			DWORD dwStartIndex=dwIncludeLength;

			//���ҿ�ʼ
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

			//����ж�
			if (dwEndIndex!=0)
			{
				//�ַ���ͷ
				CopyMemory(pszResultData+dwResultIndex,"#include \"",sizeof(CHAR)*dwIncludeLength);
				dwResultIndex+=dwIncludeLength;

				//�����ļ�
				CopyMemory(pszResultData+dwResultIndex,pszSourceData+dwCurrentIndex+dwStartIndex,sizeof(CHAR)*(dwEndIndex-dwStartIndex));
				dwResultIndex+=(dwEndIndex-dwStartIndex);

				//��������
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

		//����ж�
		if (dwCurrentIndex>=dwFileLength)
		{
			ASSERT(dwCurrentIndex==dwFileLength);
			break;
		}

	} while (true);

	//д���ļ�
	TargetFile.Write(pszResultData,dwResultIndex);

	//�ر��ļ�
	TargetFile.Close();
	SourceFile.Close();

	//������Դ
	SafeDelete(pszResultData);

	return true;
}

//�����ļ�
bool CSDKBuilderDlg::CopyBinaryFile(LPCTSTR pszSourceFile, LPCTSTR pszTargetPath)
{
	//��������
	CFile SourceFile,TargetFile;

	//���ļ�
	if (SourceFile.Open(pszSourceFile,CFile::modeRead,NULL)==FALSE)
	{
		CString strBuffer;
		strBuffer.Format(TEXT("��%s���ļ���ȡʧ�ܣ�"),pszSourceFile);
		AfxMessageBox(strBuffer,MB_ICONINFORMATION);
		return false;
	}

	//���ļ�
	TCHAR szTargerName[MAX_PATH]=TEXT("");
	_sntprintf_s(szTargerName,CountArray(szTargerName),TEXT("%s\\%s"),pszTargetPath,SourceFile.GetFileName().GetString());
	if (TargetFile.Open(szTargerName,CFile::modeCreate|CFile::modeWrite,NULL)==FALSE)
	{
		CString strBuffer;
		strBuffer.Format(TEXT("��%s���ļ�д��ʧ�ܣ�"),szTargerName);
		AfxMessageBox(strBuffer,MB_ICONINFORMATION);
		return false;
	}

	//�����ļ�
	UINT uReadCount=0;
	BYTE szBuffer[10240];
	do
	{
		uReadCount=SourceFile.Read(szBuffer,sizeof(szBuffer));
		if (uReadCount==0) break;
		TargetFile.Write(szBuffer,uReadCount);
	} while (true);

	//�ر��ļ�
	TargetFile.Close();
	SourceFile.Close();

	return true;
}

//��������
VOID CSDKBuilderDlg::OnBnClickedBulid()
{
	//��������
	TCHAR szFileName[MAX_PATH]=TEXT("");
	CString strPath,strTemp,strSourceFile;

	//����Ŀ¼
	strPath.Format(TEXT("%s\\Lib"),m_strTargetPath);
	CreateDirectory(strPath);

	strPath.Format(TEXT("%s\\Include"),m_strTargetPath);
	CreateDirectory(strPath);

	//��ȡͷ�ļ�
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

	//��ȡ�ļ�
	CString strAnsiLibTargetPath,strUniLibTargetPath;

	//6603
	strAnsiLibTargetPath.Format(TEXT("%s\\Lib"),m_strTargetPath);

	//�����ļ�
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

	//ת�����
	AfxMessageBox(TEXT("�ļ�ת����ɣ�"),MB_ICONINFORMATION);

	return;
}

//////////////////////////////////////////////////////////////////////////////////
