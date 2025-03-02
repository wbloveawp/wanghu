#include "StdAfx.h"
#include "Resource.h"
#include "GlobalStation.h"
#include "GlobalUserInfo.h"

//////////////////////////////////////////////////////////////////////////

//静态变量
CGlobalStation * CGlobalStation::m_pGlobalStation=NULL;					//站点信息

//////////////////////////////////////////////////////////////////////////

//构造函数
CGlobalStation::CGlobalStation()
{
	//资源变量
	m_hStationIcon=NULL;

	//平台变量
	m_dwStationID=INVALID_DWORD;
	ZeroMemory(m_szStationName,sizeof(m_szStationName));
	ZeroMemory(m_szStationHome,sizeof(m_szStationHome));
	ZeroMemory(m_szStationPlaza,sizeof(m_szStationPlaza));

	//设置对象
	ASSERT(m_pGlobalStation==NULL);
	if (m_pGlobalStation==NULL) m_pGlobalStation=this;

	return;
}

//析构函数
CGlobalStation::~CGlobalStation()
{
	//释放对象
	ASSERT(m_pGlobalStation==this);
	if (m_pGlobalStation==this) m_pGlobalStation=NULL;

	return;
}

//站点标识
DWORD CGlobalStation::GetStationID()
{
	//读取标识
	if (m_dwStationID==INVALID_DWORD)
	{
		//站点文件
		TCHAR szStationFile[MAX_PATH]=TEXT("");
		GetStationFile(szStationFile,CountArray(szStationFile));

		//读取数据
		m_dwStationID=GetPrivateProfileInt(TEXT("Station"),TEXT("StationID"),0L,szStationFile);
	}

	return m_dwStationID;
}

//站点名字
LPCTSTR CGlobalStation::GetStationName()
{
	//读取地址
	if (m_szStationName[0]==0)
	{
		//站点文件
		TCHAR szStationFile[MAX_PATH]=TEXT("");
		GetStationFile(szStationFile,CountArray(szStationFile));

		//读取数据
		GetPrivateProfileString(TEXT("Station"),TEXT("PlatformName"),szProduct,m_szStationName,CountArray(m_szStationName),szStationFile);
	}

	return m_szStationName;
}

//站点图标
HICON CGlobalStation::GetStationIcon()
{
	//加载图标
	if (m_hStationIcon==NULL)
	{
		//站点文件
		TCHAR szStationFile[MAX_PATH]=TEXT("");
		GetStationFile(szStationFile,CountArray(szStationFile));

		//读取路径
		TCHAR szIconName[32]=TEXT("");
		GetPrivateProfileString(TEXT("Resource"),TEXT("Icon"),TEXT("StationIcon.ico"),szIconName,CountArray(szIconName),szStationFile);

		//构造路径
		TCHAR szIconPath[MAX_PATH]=TEXT("");
		_sntprintf_s(szIconPath,CountArray(szIconPath),TEXT("Collocate\\%s"),szIconName);

		//加载图标
		m_hStationIcon=(HICON)LoadImage(NULL,szIconPath,IMAGE_ICON,0,0,LR_LOADFROMFILE);
	}

	//系统图标
	if (m_hStationIcon==NULL)
	{
		HINSTANCE hInstance=GetModuleHandle(PLATFORM_DATA_DLL_NAME);
		m_hStationIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDC_STATION_ICON));
	}

	return m_hStationIcon;
}

//宣传图片
CImage & CGlobalStation::GetPublicizeImage()
{
	////加载广告
	//if (m_ImagePublicize.IsNull()==true)
	//{
	//	//站点文件
	//	TCHAR szStationFile[MAX_PATH]=TEXT("");
	//	GetStationFile(szStationFile,CountArray(szStationFile));

	//	//读取路径
	//	TCHAR szImageName[32]=TEXT("");
	//	GetPrivateProfileString(TEXT("Resource"),TEXT("Publicize"),TEXT("Publicize.bmp"),szImageName,CountArray(szImageName),szStationFile);

	//	//构造路径
	//	TCHAR szImagePath[MAX_PATH]=TEXT("");
	//	_sntprintf_s(szImagePath,CountArray(szImagePath),TEXT("Collocate\\%s"),szImageName);

	//	//加载广告
	//	m_ImagePublicize.Load(szImagePath);
	//}

	////默认广告
	//if (m_ImagePublicize.IsNull()==true)
	//{
	//	HINSTANCE hInstance=GetModuleHandle(PLATFORM_DATA_DLL_NAME);
	//	m_ImagePublicize.LoadFromResource(hInstance,IDB_PUBLICIZE_LOGO);
	//}

	return m_ImagePublicize;
}

//大厅广告
CImage & CGlobalStation::GetPlazaPublicizeImage()
{
	////加载广告
	//if (m_ImagePlazaPublicize.IsNull()==true)
	//{
	//	//站点文件
	//	TCHAR szStationFile[MAX_PATH]=TEXT("");
	//	GetStationFile(szStationFile,CountArray(szStationFile));

	//	//读取路径
	//	TCHAR szImageName[32]=TEXT("");
	//	GetPrivateProfileString(TEXT("Resource"),TEXT("PlazaPublicize"),TEXT("PlazaPublicize.bmp"),szImageName,CountArray(szImageName),szStationFile);

	//	//构造路径
	//	TCHAR szImagePath[MAX_PATH]=TEXT("");
	//	_sntprintf_s(szImagePath,CountArray(szImagePath),TEXT("Collocate\\%s"),szImageName);

	//	//加载广告
	//	m_ImagePlazaPublicize.Load(szImagePath);
	//}

	////默认广告
	//if (m_ImagePlazaPublicize.IsNull()==true)
	//{
	//	HINSTANCE hInstance=GetModuleHandle(PLATFORM_DATA_DLL_NAME);
	//	m_ImagePlazaPublicize.LoadFromResource(hInstance,IDB_PLAZA_PUBLICIZE);
	//}

	return m_ImagePlazaPublicize;
}

//平台标志
CPngImageWarpper & CGlobalStation::GetStationLogoImage()
{
	////加载标志
	//if (m_ImageStationLogo.IsNull()==true) 
	//{
	//	//站点文件
	//	TCHAR szStationFile[MAX_PATH]=TEXT("");
	//	GetStationFile(szStationFile,CountArray(szStationFile));

	//	//读取路径
	//	TCHAR szImageName[32]=TEXT("");
	//	GetPrivateProfileString(TEXT("Resource"),TEXT("Logo"),TEXT("StationLogo.png"),szImageName,CountArray(szImageName),szStationFile);

	//	//构造路径
	//	TCHAR szImagePath[MAX_PATH]=TEXT("");
	//	_sntprintf_s(szImagePath,CountArray(szImagePath),TEXT("Collocate\\%s"),szImageName);

	//	//加载广告
	//	m_ImageStationLogo.LoadImage(szImagePath);
	//}

	////默认标志
	//if (m_ImageStationLogo.IsNull()==true)
	//{
	//	HINSTANCE hInstance=GetModuleHandle(PLATFORM_DATA_DLL_NAME);
	//	m_ImageStationLogo.LoadImage(hInstance,TEXT("STATION_LOGO"));
	//}

	return m_ImageStationLogo;
}

//站点主页
LPCTSTR CGlobalStation::GetStationHomeLink()
{
	//读取判断
	if (m_szStationHome[0]==0)
	{
		//站点文件
		TCHAR szStationFile[MAX_PATH]=TEXT("");
		GetStationFile(szStationFile,CountArray(szStationFile));

		//站点数据
		GetPrivateProfileString(TEXT("Station"),TEXT("PlatformLink"),TEXT(""), m_szStationHome, CountArray(m_szStationHome), szStationFile);
	}

	return m_szStationHome;
}

//站点连接
LPCTSTR CGlobalStation::GetStationPlazaLink()
{
	//变量定义
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//构造页面
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

	//比较地址
	if( lstrcmp(szBrowseUrl,m_szStationPlaza) != 0 )
	{
		lstrcpyn(m_szStationPlaza,szBrowseUrl,CountArray(m_szStationPlaza));
	}

	return m_szStationPlaza;
}

//成绩页面
LPCTSTR CGlobalStation::GetCollectScore(TCHAR szBrowseUrl[], UINT uCharCount)
{
	//站点文件
	TCHAR szStationFile[MAX_PATH]=TEXT("");
	GetStationFile(szStationFile,CountArray(szStationFile));

	//读取地址
	GetPrivateProfileString(TEXT("PlatformLink"),TEXT("CollectScoreLink"),TEXT(""),szBrowseUrl,uCharCount,szStationFile);

	//默认地址
	if (szBrowseUrl[0]==0)
	{
		//系统文件
		TCHAR szPlatformFile[MAX_PATH]=TEXT("");
		GetPlatformFile(szPlatformFile,CountArray(szPlatformFile));

		//读取数据
		GetPrivateProfileString(TEXT("PlatformLink"),TEXT("CollectScoreLink"),TEXT(""),szBrowseUrl,uCharCount,szPlatformFile);
	}

	//系统地址
	if (szBrowseUrl[0]==0)
	{
		//变量定义
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//构造地址
		_sntprintf_s(szBrowseUrl,uCharCount,uCharCount,TEXT("http://service.abcdefgh.com/CollectScore.aspx?UserID=%ld&PlazaStationID=%ld&UserStationID=%ld"),
			pGlobalUserData->dwUserID,GetStationID(),pGlobalUserData->dwStationID);
	}

	return szBrowseUrl;
}

//本地连接
LPCTSTR CGlobalStation::GetNativeIniLink(WORD wLinkID, TCHAR szBrowseUrl[], UINT uCharCount)
{
	//站点文件
	TCHAR szStationFile[MAX_PATH]=TEXT("");
	GetStationFile(szStationFile,CountArray(szStationFile));

	//构造地址
	switch (wLinkID)
	{
	case LINK_ID_PLAZA_HELP:		//大厅帮助
		{
			GetPrivateProfileString(TEXT("ServiceLink"),TEXT("PlazaHelp"),TEXT(""),szBrowseUrl,uCharCount,szStationFile);
			return szBrowseUrl;
		}
	case LINK_ID_GET_PASSWORD:		//取回密码
		{
			GetPrivateProfileString(TEXT("ServiceLink"),TEXT("GetPassword"),TEXT(""),szBrowseUrl,uCharCount,szStationFile);
			return szBrowseUrl;
		}
	case LINK_ID_SERVICE_ARTICLE:	//服务条款
		{
			GetPrivateProfileString(TEXT("ServiceLink"),TEXT("ServiceArticle"),TEXT(""),szBrowseUrl,uCharCount,szStationFile);
			return szBrowseUrl;
		}
	case LINK_ID_SPREADER_NOTE:		//奖励说明
		{
			GetPrivateProfileString(TEXT("ServiceLink"),TEXT("SpreaderNote"),TEXT(""),szBrowseUrl,uCharCount,szStationFile);
			return szBrowseUrl;
		}
	}

	return NULL;
}

//导航连接
LPCTSTR CGlobalStation::GetNavigationLink(DWORD dwIndexID, TCHAR szBrowseUrl[], UINT uCharCount)
{
	//站点文件
	TCHAR szStationFile[MAX_PATH]=TEXT("");
	GetStationFile(szStationFile,CountArray(szStationFile));

	//构造键名
	TCHAR szItemName[16]=TEXT("");
	_sntprintf_s(szItemName,CountArray(szItemName),TEXT("Navigation%ld"),dwIndexID);

	//读取连接
	GetPrivateProfileString(TEXT("Navigation"),szItemName,TEXT(""),szBrowseUrl,uCharCount,szStationFile);

	//读取默认
	if (szBrowseUrl[0]==0)
	{
		//系统文件
		TCHAR szPlatformFile[MAX_PATH]=TEXT("");
		GetPlatformFile(szPlatformFile,CountArray(szPlatformFile));

		//读取数据
		GetPrivateProfileString(TEXT("Navigation"),szItemName,TEXT(""),szBrowseUrl,uCharCount,szPlatformFile);
	}

	//添加大厅版本信息
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

//规则页面
LPCTSTR CGlobalStation::GetRedirectionRule(WORD wKindID, TCHAR szBrowseUrl[], UINT uCharCount)
{
	//站点文件
	TCHAR szStationFile[MAX_PATH]=TEXT("");
	GetStationFile(szStationFile,CountArray(szStationFile));

	//读取地址
	GetPrivateProfileString(TEXT("PlatformLink"),TEXT("GameRuleLink"),TEXT(""),szBrowseUrl,uCharCount,szStationFile);

	//默认地址
	if (szBrowseUrl[0]==0)
	{
		//系统文件
		TCHAR szPlatformFile[MAX_PATH]=TEXT("");
		GetPlatformFile(szPlatformFile,CountArray(szPlatformFile));

		//读取数据
		GetPrivateProfileString(TEXT("PlatformLink"),TEXT("GameRuleLink"),TEXT(""),szBrowseUrl,uCharCount,szPlatformFile);
	}

	//地址变换
	if (szBrowseUrl[0]!=0)
	{
		//构造标识
		TCHAR szKindID[32]=TEXT("");
		_sntprintf_s(szKindID,CountArray(szKindID),TEXT("KindID=%d"),wKindID);

		//转换地址
		CString strResultLink=szBrowseUrl;
		strResultLink.Replace(TEXT("%KindID%"),szKindID);

		//拷贝地址
		lstrcpyn(szBrowseUrl,strResultLink,uCharCount);

		return szBrowseUrl;
	}

	//系统地址
	if (szBrowseUrl[0]==0)
	{
		//变量定义
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//构造地址
		_sntprintf_s(szBrowseUrl,uCharCount, uCharCount, TEXT("%ld/%ld/%ld"),
			wKindID,GetStationID(),pGlobalUserData->dwStationID);
	}

	return szBrowseUrl;
}

//排行页面
LPCTSTR CGlobalStation::GetRedirectionChart(WORD wKindID, TCHAR szBrowseUrl[], UINT uCharCount)
{
	//站点文件
	TCHAR szStationFile[MAX_PATH]=TEXT("");
	GetStationFile(szStationFile,CountArray(szStationFile));

	//读取地址
	GetPrivateProfileString(TEXT("PlatformLink"),TEXT("GameChartLink"),TEXT(""),szBrowseUrl,uCharCount,szStationFile);

	//默认地址
	if (szBrowseUrl[0]==0)
	{
		//系统文件
		TCHAR szPlatformFile[MAX_PATH]=TEXT("");
		GetPlatformFile(szPlatformFile,CountArray(szPlatformFile));

		//读取数据
		GetPrivateProfileString(TEXT("PlatformLink"),TEXT("GameChartLink"),TEXT(""),szBrowseUrl,uCharCount,szPlatformFile);
	}

	//地址变换
	if (szBrowseUrl[0]!=0)
	{
		//变量定义
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//构造标识
		TCHAR szUserID[32]=TEXT("");
		_sntprintf_s(szUserID,CountArray(szUserID),TEXT("UserID=%d"),pGlobalUserData->dwUserID);

		//构造密码
		TCHAR szPassword[32]=TEXT("");
		_sntprintf_s(szPassword,CountArray(szPassword),TEXT("Password=%s"),pGlobalUserData->szPassword);

		//转换地址
		CString strResultLink=szBrowseUrl;
		strResultLink.Replace(TEXT("%UserID%"),szUserID);
		strResultLink.Replace(TEXT("%Password%"),szPassword);

		//拷贝地址
		lstrcpyn(szBrowseUrl,strResultLink,uCharCount);

		return szBrowseUrl;
	}

	//系统地址
	if (szBrowseUrl[0]==0)
	{
		//变量定义
		CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
		tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

		//构造地址
		_sntprintf_s(szBrowseUrl,uCharCount, uCharCount, TEXT("http://service.abcdefgh.com/GameChart.aspx?UserID=%ld&Password=%s&KindID=%ld&PlazaStationID=%ld&UserStationID=%ld"),
			pGlobalUserData->dwUserID,pGlobalUserData->szPassword,wKindID,GetStationID(),pGlobalUserData->dwStationID);
	}

	return szBrowseUrl;
}

//定制页面
LPCTSTR CGlobalStation::GetRedirectionCustom(WORD wCustomID, TCHAR szBrowseUrl[], UINT uCharCount)
{
	//变量定义
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//构造地址
	_sntprintf_s(szBrowseUrl,uCharCount, uCharCount, TEXT("http://service.abcdefgh.com/GameCustom.aspx?CustomID=%ld&UserID=%ld&PlazaStationID=%ld&UserStationID=%ld"),
		wCustomID,pGlobalUserData->dwUserID,GetStationID(),pGlobalUserData->dwStationID);

	return szBrowseUrl;
}

//跳转页面
LPCTSTR CGlobalStation::GetRedirectionLink(DWORD dwResourceID, TCHAR szBrowseUrl[], UINT uCharCount)
{
	//变量定义
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//构造页面
	_sntprintf_s(szBrowseUrl,uCharCount, uCharCount, TEXT("http://service.abcdefgh.com/GlobalJump.aspx?PlazaStationID=%ld&PlazaVersion=%ld&ResourceID=%ld&UserID=%ld&Password=%s"),
		GetStationID(),VERSION_PLAZA,dwResourceID,pGlobalUserData->dwUserID,pGlobalUserData->szPassword);

	return szBrowseUrl;
}

//跳转页面
LPCTSTR CGlobalStation::GetRedirectionLink(DWORD dwResourceID, WORD wKindID, WORD wServerID, TCHAR szBrowseUrl[], UINT uCharCount)
{
	//变量定义
	CGlobalUserInfo * pGlobalUserInfo=CGlobalUserInfo::GetInstance();
	tagGlobalUserData * pGlobalUserData=pGlobalUserInfo->GetGlobalUserData();

	//构造页面
	_sntprintf_s(szBrowseUrl,uCharCount, uCharCount, TEXT("http://service.abcdefgh.com/GlobalJump.aspx?PlazaStationID=%ld&PlazaVersion=%ld&ResourceID=%ld&KindID=%d&ServerID=%d&UserID=%ld&Password=%s"),
		GetStationID(),VERSION_PLAZA,dwResourceID,wKindID,wServerID,pGlobalUserData->dwUserID,pGlobalUserData->szPassword);

	return szBrowseUrl;
}

//定向页面
LPCTSTR CGlobalStation::GetRedirectionServer( DWORD dwServerType, TCHAR szBrowseUrl[], UINT uCharCount )
{
	//构造页面
	_sntprintf_s(szBrowseUrl,uCharCount, uCharCount, TEXT("http://service.abcdefgh.com/GetLogonServer.aspx?StationID=%ld&ServerType=%ld"),
		GetStationID(),dwServerType);

	return szBrowseUrl;
}

//获取资源
LPCTSTR CGlobalStation::GetCustomString( BOOL bPlatform, LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpRetString, DWORD dwSize )
{
	ASSERT(lpRetString);
	if( !lpRetString ) return lpRetString;

	//读取地址
	if (!bPlatform)
	{
		//站点文件
		TCHAR szStationFile[MAX_PATH]=TEXT("");
		GetStationFile(szStationFile,CountArray(szStationFile));

		//读取数据
		GetPrivateProfileString(lpAppName,lpKeyName,lpDefault,lpRetString,dwSize,szStationFile);
	}
	else
	{
		//站点文件
		TCHAR szPlatformFile[MAX_PATH]=TEXT("");
		GetPlatformFile(szPlatformFile,CountArray(szPlatformFile));

		//读取数据
		GetPrivateProfileString(lpAppName,lpKeyName,lpDefault,lpRetString,dwSize,szPlatformFile);
	}

	return lpRetString;
}

//站点文件
LPCTSTR CGlobalStation::GetStationFile(TCHAR szFileName[], UINT uCharCount)
{
	//获取目录
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//构造目录
	_sntprintf_s(szFileName,uCharCount, uCharCount, TEXT("%s\\Collocate\\Collocate.ini"),szDirectory);

	return szFileName;
}

//平台文件
LPCTSTR CGlobalStation::GetPlatformFile(TCHAR szFileName[], UINT uCharCount)
{
	//获取目录
	TCHAR szDirectory[MAX_PATH]=TEXT("");
	CWHService::GetWorkDirectory(szDirectory,CountArray(szDirectory));

	//构造目录
	_sntprintf_s(szFileName,uCharCount, uCharCount, TEXT("%s\\Platform.ini"),szDirectory);

	return szFileName;
}

//兼容站点
bool CGlobalStation::IsCompStation(DWORD dwStationID)
{
	INT_PTR nStationCount=m_CompStationArr.GetCount();

	if(nStationCount==0)
	{
		m_CompStationArr.Add(GetStationID());
		//站点文件
		TCHAR szStationFile[MAX_PATH]=TEXT("");
		GetStationFile(szStationFile,CountArray(szStationFile));

		//变量定义
		TCHAR szKeyName[32]=TEXT("");

		//兼容区域
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

		//兼容站点
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
