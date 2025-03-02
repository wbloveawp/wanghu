#include "StdAfx.h"
#include "Resource.h"
#include "GlobalStation.h"
#include "GlobalUserInfo.h"

//////////////////////////////////////////////////////////////////////////

//��̬����
CGlobalStation * CGlobalStation::m_pGlobalStation=NULL;					//վ����Ϣ

//////////////////////////////////////////////////////////////////////////

//���캯��
CGlobalStation::CGlobalStation()
{
	//��Դ����
	m_hStationIcon=NULL;

	//ƽ̨����
	m_dwStationID=INVALID_DWORD;
	ZeroMemory(m_szStationName,sizeof(m_szStationName));
	ZeroMemory(m_szStationHome,sizeof(m_szStationHome));
	ZeroMemory(m_szStationPlaza,sizeof(m_szStationPlaza));

	//���ö���
	ASSERT(m_pGlobalStation==NULL);
	if (m_pGlobalStation==NULL) m_pGlobalStation=this;

	return;
}

//��������
CGlobalStation::~CGlobalStation()
{
	//�ͷŶ���
	ASSERT(m_pGlobalStation==this);
	if (m_pGlobalStation==this) m_pGlobalStation=NULL;

	return;
}

//վ���ʶ
DWORD CGlobalStation::GetStationID()
{
	//��ȡ��ʶ
	if (m_dwStationID==INVALID_DWORD)
	{
		//վ���ļ�
		TCHAR szStationFile[MAX_PATH]=TEXT("");
		GetStationFile(szStationFile,CountArray(szStationFile));

		//��ȡ����
		m_dwStationID=GetPrivateProfileInt(TEXT("Station"),TEXT("StationID"),0L,szStationFile);
	}

	return m_dwStationID;
}

//վ������
LPCTSTR CGlobalStation::GetStationName()
{
	//��ȡ��ַ
	if (m_szStationName[0]==0)
	{
		//վ���ļ�
		TCHAR szStationFile[MAX_PATH]=TEXT("");
		GetStationFile(szStationFile,CountArray(szStationFile));

		//��ȡ����
		GetPrivateProfileString(TEXT("Station"),TEXT("PlatformName"),szProduct,m_szStationName,CountArray(m_szStationName),szStationFile);
	}

	return m_szStationName;
}

//վ��ͼ��
HICON CGlobalStation::GetStationIcon()
{
	//����ͼ��
	if (m_hStationIcon==NULL)
	{
		//վ���ļ�
		TCHAR szStationFile[MAX_PATH]=TEXT("");
		GetStationFile(szStationFile,CountArray(szStationFile));

		//��ȡ·��
		TCHAR szIconName[32]=TEXT("");
		GetPrivateProfileString(TEXT("Resource"),TEXT("Icon"),TEXT("StationIcon.ico"),szIconName,CountArray(szIconName),szStationFile);

		//����·��
		TCHAR szIconPath[MAX_PATH]=TEXT("");
		_sntprintf_s(szIconPath,CountArray(szIconPath),TEXT("Collocate\\%s"),szIconName);

		//����ͼ��
		m_hStationIcon=(HICON)LoadImage(NULL,szIconPath,IMAGE_ICON,0,0,LR_LOADFROMFILE);
	}

	//ϵͳͼ��
	if (m_hStationIcon==NULL)
	{
		HINSTANCE hInstance=GetModuleHandle(PLATFORM_DATA_DLL_NAME);
		m_hStationIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDC_STATION_ICON));
	}

	return m_hStationIcon;
}

//����ͼƬ
CImage & CGlobalStation::GetPublicizeImage()
{
	////���ع��
	//if (m_ImagePublicize.IsNull()==true)
	//{
	//	//վ���ļ�
	//	TCHAR szStationFile[MAX_PATH]=TEXT("");
	//	GetStationFile(szStationFile,CountArray(szStationFile));

	//	//��ȡ·��
	//	TCHAR szImageName[32]=TEXT("");
	//	GetPrivateProfileString(TEXT("Resource"),TEXT("Publicize"),TEXT("Publicize.bmp"),szImageName,CountArray(szImageName),szStationFile);

	//	//����·��
	//	TCHAR szImagePath[MAX_PATH]=TEXT("");
	//	_sntprintf_s(szImagePath,CountArray(szImagePath),TEXT("Collocate\\%s"),szImageName);

	//	//���ع��
	//	m_ImagePublicize.Load(szImagePath);
	//}

	////Ĭ�Ϲ��
	//if (m_ImagePublicize.IsNull()==true)
	//{
	//	HINSTANCE hInstance=GetModuleHandle(PLATFORM_DATA_DLL_NAME);
	//	m_ImagePublicize.LoadFromResource(hInstance,IDB_PUBLICIZE_LOGO);
	//}

	return m_ImagePublicize;
}

//�������
CImage & CGlobalStation::GetPlazaPublicizeImage()
{
	////���ع��
	//if (m_ImagePlazaPublicize.IsNull()==true)
	//{
	//	//վ���ļ�
	//	TCHAR szStationFile[MAX_PATH]=TEXT("");
	//	GetStationFile(szStationFile,CountArray(szStationFile));

	//	//��ȡ·��
	//	TCHAR szImageName[32]=TEXT("");
	//	GetPrivateProfileString(TEXT("Resource"),TEXT("PlazaPublicize"),TEXT("PlazaPublicize.bmp"),szImageName,CountArray(szImageName),szStationFile);

	//	//����·��
	//	TCHAR szImagePath[MAX_PATH]=TEXT("");
	//	_sntprintf_s(szImagePath,CountArray(szImagePath),TEXT("Collocate\\%s"),szImageName);

	//	//���ع��
	//	m_ImagePlazaPublicize.Load(szImagePath);
	//}

	////Ĭ�Ϲ��
	//if (m_ImagePlazaPublicize.IsNull()==true)
	//{
	//	HINSTANCE hInstance=GetModuleHandle(PLATFORM_DATA_DLL_NAME);
	//	m_ImagePlazaPublicize.LoadFromResource(hInstance,IDB_PLAZA_PUBLICIZE);
	//}

	return m_ImagePlazaPublicize;
}

//ƽ̨��־
CPngImageWarpper & CGlobalStation::GetStationLogoImage()
{
	////���ر�־
	//if (m_ImageStationLogo.IsNull()==true) 
	//{
	//	//վ���ļ�
	//	TCHAR szStationFile[MAX_PATH]=TEXT("");
	//	GetStationFile(szStationFile,CountArray(szStationFile));

	//	//��ȡ·��
	//	TCHAR szImageName[32]=TEXT("");
	//	GetPrivateProfileString(TEXT("Resource"),TEXT("Logo"),TEXT("StationLogo.png"),szImageName,CountArray(szImageName),szStationFile);

	//	//����·��
	//	TCHAR szImagePath[MAX_PATH]=TEXT("");
	//	_sntprintf_s(szImagePath,CountArray(szImagePath),TEXT("Collocate\\%s"),szImageName);

	//	//���ع��
	//	m_ImageStationLogo.LoadImage(szImagePath);
	//}

	////Ĭ�ϱ�־
	//if (m_ImageStationLogo.IsNull()==true)
	//{
	//	HINSTANCE hInstance=GetModuleHandle(PLATFORM_DATA_DLL_NAME);
	//	m_ImageStationLogo.LoadImage(hInstance,TEXT("STATION_LOGO"));
	//}

	return m_ImageStationLogo;
}

//վ����ҳ
LPCTSTR CGlobalStation::GetStationHomeLink()
{
	//��ȡ�ж�
	if (m_szStationHome[0]==0)
	{
		//վ���ļ�
		TCHAR szStationFile[MAX_PATH]=TEXT("");
		GetStationFile(szStationFile,CountArray(szStationFile));

		//վ������
		GetPrivateProfileString(TEXT("Station"),TEXT("PlatformLink"),TEXT(""), m_szStationHome, CountArray(m_szStationHome), szStationFile);
	}

	return m_szStationHome;
}

//վ������
LPCTSTR CGlobalStation::GetStationPlazaLink()
{
	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//����ҳ��
	TCHAR szBrowseUrl[256] = TEXT("");
	if( pGlobalUserData->dwUserID != 0 )
	{
		_sntprintf_s(szBrowseUrl,CountArray(szBrowseUrl),TEXT("http://service.abcdefgh.com/GlobalJump.aspx?PlazaStationID=%ld&PlazaVersion=%ld&ResourceID=%ld&UserID=%ld&Password=%s"),
			GetStationID(),VERSION_PLAZA,JUMP_ID_STATION_PLAZA,pGlobalUserData->dwUserID,pGlobalUserData->szPassword);
	}
	else
	{
		_sntprintf_s(szBrowseUrl,CountArray(szBrowseUrl),TEXT("http://service.abcdefgh.com/GlobalJump.aspx?PlazaStationID=%ld&PlazaVersion=%ld&ResourceID=%ld"),
			GetStationID(),VERSION_PLAZA,JUMP_ID_STATION_PLAZA);
	}

	//�Ƚϵ�ַ
	if( lstrcmp(szBrowseUrl,m_szStationPlaza) != 0 )
	{
		lstrcpyn(m_szStationPlaza,szBrowseUrl,CountArray(m_szStationPlaza));
	}

	return m_szStationPlaza;
}

//�ɼ�ҳ��
LPCTSTR CGlobalStation::GetCollectScore(TCHAR szBrowseUrl[], UINT uCharCount)
{
	//վ���ļ�
	TCHAR szStationFile[MAX_PATH]=TEXT("");
	GetStationFile(szStationFile,CountArray(szStationFile));

	//��ȡ��ַ
	GetPrivateProfileString(TEXT("PlatformLink"),TEXT("CollectScoreLink"),TEXT(""),szBrowseUrl,uCharCount,szStationFile);

	//Ĭ�ϵ�ַ
	if (szBrowseUrl[0]==0)
	{
		//ϵͳ�ļ�
		TCHAR szPlatformFile[MAX_PATH]=TEXT("");
		GetPlatformFile(szPlatformFile,CountArray(szPlatformFile));

		//��ȡ����
		GetPrivateProfileString(TEXT("PlatformLink"),TEXT("CollectScoreLink"),TEXT(""),szBrowseUrl,uCharCount,szPlatformFile);
	}

	//ϵͳ��ַ
	if (szBrowseUrl[0]==0)
	{
		//��������
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//�����ַ
		_sntprintf_s(szBrowseUrl,uCharCount,uCharCount,TEXT("http://service.abcdefgh.com/CollectScore.aspx?UserID=%ld&PlazaStationID=%ld&UserStationID=%ld"),
			pGlobalUserData->dwUserID,GetStationID(),pGlobalUserData->dwStationID);
	}

	return szBrowseUrl;
}

//��������
LPCTSTR CGlobalStation::GetNativeIniLink(WORD wLinkID, TCHAR szBrowseUrl[], UINT uCharCount)
{
	//վ���ļ�
	TCHAR szStationFile[MAX_PATH]=TEXT("");
	GetStationFile(szStationFile,CountArray(szStationFile));

	//�����ַ
	switch (wLinkID)
	{
	case LINK_ID_PLAZA_HELP:		//��������
		{
			GetPrivateProfileString(TEXT("ServiceLink"),TEXT("PlazaHelp"),TEXT(""),szBrowseUrl,uCharCount,szStationFile);
			return szBrowseUrl;
		}
	case LINK_ID_GET_PASSWORD:		//ȡ������
		{
			GetPrivateProfileString(TEXT("ServiceLink"),TEXT("GetPassword"),TEXT(""),szBrowseUrl,uCharCount,szStationFile);
			return szBrowseUrl;
		}
	case LINK_ID_SERVICE_ARTICLE:	//��������
		{
			GetPrivateProfileString(TEXT("ServiceLink"),TEXT("ServiceArticle"),TEXT(""),szBrowseUrl,uCharCount,szStationFile);
			return szBrowseUrl;
		}
	case LINK_ID_SPREADER_NOTE:		//����˵��
		{
			GetPrivateProfileString(TEXT("ServiceLink"),TEXT("SpreaderNote"),TEXT(""),szBrowseUrl,uCharCount,szStationFile);
			return szBrowseUrl;
		}
	}

	return NULL;
}

//��������
LPCTSTR CGlobalStation::GetNavigationLink(DWORD dwIndexID, TCHAR szBrowseUrl[], UINT uCharCount)
{
	//վ���ļ�
	TCHAR szStationFile[MAX_PATH]=TEXT("");
	GetStationFile(szStationFile,CountArray(szStationFile));

	//�������
	TCHAR szItemName[16]=TEXT("");
	_sntprintf_s(szItemName,CountArray(szItemName),TEXT("Navigation%ld"),dwIndexID);

	//��ȡ����
	GetPrivateProfileString(TEXT("Navigation"),szItemName,TEXT(""),szBrowseUrl,uCharCount,szStationFile);

	//��ȡĬ��
	if (szBrowseUrl[0]==0)
	{
		//ϵͳ�ļ�
		TCHAR szPlatformFile[MAX_PATH]=TEXT("");
		GetPlatformFile(szPlatformFile,CountArray(szPlatformFile));

		//��ȡ����
		GetPrivateProfileString(TEXT("Navigation"),szItemName,TEXT(""),szBrowseUrl,uCharCount,szPlatformFile);
	}

	//��Ӵ����汾��Ϣ
	if( szBrowseUrl[0] != 0 )
	{
		if( !StrStr(szBrowseUrl,TEXT("PlazaVersion")) )
		{
			TCHAR szTmpUrl[MAX_PATH] = TEXT("");
			_sntprintf_s( szTmpUrl,CountArray(szTmpUrl),TEXT("&PlazaVersion=%ld"),VERSION_PLAZA );
			lstrcat( szBrowseUrl,szTmpUrl );
		}
	}

	return szBrowseUrl;
}

//����ҳ��
LPCTSTR CGlobalStation::GetRedirectionRule(WORD wKindID, TCHAR szBrowseUrl[], UINT uCharCount)
{
	//վ���ļ�
	TCHAR szStationFile[MAX_PATH]=TEXT("");
	GetStationFile(szStationFile,CountArray(szStationFile));

	//��ȡ��ַ
	GetPrivateProfileString(TEXT("PlatformLink"),TEXT("GameRuleLink"),TEXT(""),szBrowseUrl,uCharCount,szStationFile);

	//Ĭ�ϵ�ַ
	if (szBrowseUrl[0]==0)
	{
		//ϵͳ�ļ�
		TCHAR szPlatformFile[MAX_PATH]=TEXT("");
		GetPlatformFile(szPlatformFile,CountArray(szPlatformFile));

		//��ȡ����
		GetPrivateProfileString(TEXT("PlatformLink"),TEXT("GameRuleLink"),TEXT(""),szBrowseUrl,uCharCount,szPlatformFile);
	}

	//��ַ�任
	if (szBrowseUrl[0]!=0)
	{
		//�����ʶ
		TCHAR szKindID[32]=TEXT("");
		_sntprintf_s(szKindID,CountArray(szKindID),TEXT("KindID=%d"),wKindID);

		//ת����ַ
		CString strResultLink=szBrowseUrl;
		strResultLink.Replace(TEXT("%KindID%"),szKindID);

		//������ַ
		lstrcpyn(szBrowseUrl,strResultLink,uCharCount);

		return szBrowseUrl;
	}

	//ϵͳ��ַ
	if (szBrowseUrl[0]==0)
	{
		//��������
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//�����ַ
		_sntprintf_s(szBrowseUrl,uCharCount, uCharCount, TEXT("%ld/%ld/%ld"),
			wKindID,GetStationID(),pGlobalUserData->dwStationID);
	}

	return szBrowseUrl;
}

//����ҳ��
LPCTSTR CGlobalStation::GetRedirectionChart(WORD wKindID, TCHAR szBrowseUrl[], UINT uCharCount)
{
	//վ���ļ�
	TCHAR szStationFile[MAX_PATH]=TEXT("");
	GetStationFile(szStationFile,CountArray(szStationFile));

	//��ȡ��ַ
	GetPrivateProfileString(TEXT("PlatformLink"),TEXT("GameChartLink"),TEXT(""),szBrowseUrl,uCharCount,szStationFile);

	//Ĭ�ϵ�ַ
	if (szBrowseUrl[0]==0)
	{
		//ϵͳ�ļ�
		TCHAR szPlatformFile[MAX_PATH]=TEXT("");
		GetPlatformFile(szPlatformFile,CountArray(szPlatformFile));

		//��ȡ����
		GetPrivateProfileString(TEXT("PlatformLink"),TEXT("GameChartLink"),TEXT(""),szBrowseUrl,uCharCount,szPlatformFile);
	}

	//��ַ�任
	if (szBrowseUrl[0]!=0)
	{
		//��������
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//�����ʶ
		TCHAR szUserID[32]=TEXT("");
		_sntprintf_s(szUserID,CountArray(szUserID),TEXT("UserID=%d"),pGlobalUserData->dwUserID);

		//��������
		TCHAR szPassword[32]=TEXT("");
		_sntprintf_s(szPassword,CountArray(szPassword),TEXT("Password=%s"),pGlobalUserData->szPassword);

		//ת����ַ
		CString strResultLink=szBrowseUrl;
		strResultLink.Replace(TEXT("%UserID%"),szUserID);
		strResultLink.Replace(TEXT("%Password%"),szPassword);

		//������ַ
		lstrcpyn(szBrowseUrl,strResultLink,uCharCount);

		return szBrowseUrl;
	}

	//ϵͳ��ַ
	if (szBrowseUrl[0]==0)
	{
		//��������
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//�����ַ
		_sntprintf_s(szBrowseUrl,uCharCount, uCharCount, TEXT("http://service.abcdefgh.com/GameChart.aspx?UserID=%ld&Password=%s&KindID=%ld&PlazaStationID=%ld&UserStationID=%ld"),
			pGlobalUserData->dwUserID,pGlobalUserData->szPassword,wKindID,GetStationID(),pGlobalUserData->dwStationID);
	}

	return szBrowseUrl;
}

//����ҳ��
LPCTSTR CGlobalStation::GetRedirectionCustom(WORD wCustomID, TCHAR szBrowseUrl[], UINT uCharCount)
{
	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//�����ַ
	_sntprintf_s(szBrowseUrl,uCharCount, uCharCount, TEXT("http://service.abcdefgh.com/GameCustom.aspx?CustomID=%ld&UserID=%ld&PlazaStationID=%ld&UserStationID=%ld"),
		wCustomID,pGlobalUserData->dwUserID,GetStationID(),pGlobalUserData->dwStationID);

	return szBrowseUrl;
}

//��תҳ��
LPCTSTR CGlobalStation::GetRedirectionLink(DWORD dwResourceID, TCHAR szBrowseUrl[], UINT uCharCount)
{
	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//����ҳ��
	_sntprintf_s(szBrowseUrl,uCharCount, uCharCount, TEXT("http://service.abcdefgh.com/GlobalJump.aspx?PlazaStationID=%ld&PlazaVersion=%ld&ResourceID=%ld&UserID=%ld&Password=%s"),
		GetStationID(),VERSION_PLAZA,dwResourceID,pGlobalUserData->dwUserID,pGlobalUserData->szPassword);

	return szBrowseUrl;
}

//��תҳ��
LPCTSTR CGlobalStation::GetRedirectionLink(DWORD dwResourceID, WORD wKindID, WORD wServerID, TCHAR szBrowseUrl[], UINT uCharCount)
{
	//��������
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//����ҳ��
	_sntprintf_s(szBrowseUrl,uCharCount, uCharCount, TEXT("http://service.abcdefgh.com/GlobalJump.aspx?PlazaStationID=%ld&PlazaVersion=%ld&ResourceID=%ld&KindID=%d&ServerID=%d&UserID=%ld&Password=%s"),
		GetStationID(),VERSION_PLAZA,dwResourceID,wKindID,wServerID,pGlobalUserData->dwUserID,pGlobalUserData->szPassword);

	return szBrowseUrl;
}

//����ҳ��
LPCTSTR CGlobalStation::GetRedirectionServer( DWORD dwServerType, TCHAR szBrowseUrl[], UINT uCharCount )
{
	//����ҳ��
	_sntprintf_s(szBrowseUrl,uCharCount, uCharCount, TEXT("http://service.abcdefgh.com/GetLogonServer.aspx?StationID=%ld&ServerType=%ld"),
		GetStationID(),dwServerType);

	return szBrowseUrl;
}

//��ȡ��Դ
LPCTSTR CGlobalStation::GetCustomString( BOOL bPlatform, LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpRetString, DWORD dwSize )
{
	ASSERT(lpRetString);
	if( !lpRetString ) return lpRetString;

	//��ȡ��ַ
	if (!bPlatform)
	{
		//վ���ļ�
		TCHAR szStationFile[MAX_PATH]=TEXT("");
		GetStationFile(szStationFile,CountArray(szStationFile));

		//��ȡ����
		GetPrivateProfileString(lpAppName,lpKeyName,lpDefault,lpRetString,dwSize,szStationFile);
	}
	else
	{
		//վ���ļ�
		TCHAR szPlatformFile[MAX_PATH]=TEXT("");
		GetPlatformFile(szPlatformFile,CountArray(szPlatformFile));

		//��ȡ����
		GetPrivateProfileString(lpAppName,lpKeyName,lpDefault,lpRetString,dwSize,szPlatformFile);
	}

	return lpRetString;
}

//վ���ļ�
LPCTSTR CGlobalStation::GetStationFile(TCHAR szFileName[], UINT uCharCount)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//����Ŀ¼
	_sntprintf_s(szFileName,uCharCount, uCharCount, TEXT("%s\\Collocate\\Collocate.ini"),szDirectory);

	return szFileName;
}

//ƽ̨�ļ�
LPCTSTR CGlobalStation::GetPlatformFile(TCHAR szFileName[], UINT uCharCount)
{
	//��ȡĿ¼
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//����Ŀ¼
	_sntprintf_s(szFileName,uCharCount, uCharCount, TEXT("%s\\Platform.ini"),szDirectory);

	return szFileName;
}

//����վ��
bool CGlobalStation::IsCompStation(DWORD dwStationID)
{
	INT_PTR nStationCount=m_CompStationArr.GetCount();

	if(nStationCount==0)
	{
		m_CompStationArr.Add(GetStationID());
		//վ���ļ�
		TCHAR szStationFile[MAX_PATH]=TEXT("");
		GetStationFile(szStationFile,CountArray(szStationFile));

		//��������
		TCHAR szKeyName[32]=TEXT("");

		//��������
		UINT uAreaCount=GetPrivateProfileInt(TEXT("StationArea"),TEXT("CompAreaCount"),0L,szStationFile);
		for(UINT i=0; i<uAreaCount; i++)
		{
			_sntprintf_s(szKeyName,CountArray(szKeyName),TEXT("CompArea%dMin"),i+1);
			UINT uAreaMin=GetPrivateProfileInt(TEXT("StationArea"),szKeyName,0L,szStationFile);
			_sntprintf_s(szKeyName,CountArray(szKeyName),TEXT("CompArea%dMax"),i+1);
			UINT uAreaMax=GetPrivateProfileInt(TEXT("StationArea"),szKeyName,0L,szStationFile);
			for(UINT j=uAreaMin;j<=uAreaMax;j++)
			{
				m_CompStationArr.Add(j);
			}
		}

		//����վ��
		UINT uCompStationCount=GetPrivateProfileInt(TEXT("StationArea"),TEXT("CompStationCount"),0L,szStationFile);
		for(UINT i=0; i<uCompStationCount; i++)
		{
			_sntprintf_s(szKeyName,CountArray(szKeyName),TEXT("CompStation%d"),i+1);
			UINT uStationID=GetPrivateProfileInt(TEXT("StationArea"),szKeyName,0L,szStationFile);
			if(uStationID==0) continue;
			m_CompStationArr.Add(uStationID);;
		}
	}

	nStationCount=m_CompStationArr.GetCount();
	for(INT_PTR i=0;i<nStationCount;i++)
	{
		if(dwStationID==m_CompStationArr.GetAt(i))
			return true;
	}

	return false;
}
//////////////////////////////////////////////////////////////////////////
