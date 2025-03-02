#ifndef GLOBAL_STATION_HEAD_FILE
#define GLOBAL_STATION_HEAD_FILE

#pragma once

#include "PlatformDataHead.h"

//////////////////////////////////////////////////////////////////////////

//站点数据
class PLATFORM_DATA_CLASS CGlobalStation
{
	//站点变量
protected:
	DWORD							m_dwStationID;						//站点标识
	TCHAR							m_szStationName[32];				//站点名字
	TCHAR							m_szStationHome[128];				//平台页面
	TCHAR							m_szStationPlaza[256];				//广场页面
	CWHArray<DWORD,DWORD>			m_CompStationArr;					//兼容站点

	//资源变量
protected:
	HICON							m_hStationIcon;						//平台图标
	CImage							m_ImagePublicize;					//宣传图片
	CPngImageWarpper 				m_ImageStationLogo;					//平台标志
	CImage							m_ImagePlazaPublicize;				//大厅宣传图片

	//静态变量
protected:
	static CGlobalStation *			m_pGlobalStation;					//站点信息

	//函数定义
public:
	//构造函数
	CGlobalStation();
	//析构函数
	virtual ~CGlobalStation();

	//平台属性
public:
	//站点标识
	DWORD GetStationID();
	//站点名字
	LPCTSTR GetStationName();
	//兼容站点
	bool IsCompStation(DWORD dwStationID);

	//站点资源
public:
	//站点图标
	HICON GetStationIcon();
	//宣传图片
	CImage & GetPublicizeImage();
	//平台标志
	CPngImageWarpper & GetStationLogoImage();
	//大厅广告
	CImage & GetPlazaPublicizeImage();

	//固有页面
public:
	//站点主页
	LPCTSTR GetStationHomeLink();
	//站点连接
	LPCTSTR GetStationPlazaLink();

	//全局页面
public:
	//成绩页面
	LPCTSTR GetCollectScore(TCHAR szBrowseUrl[], UINT uCharCount);
	//本地连接
	LPCTSTR GetNativeIniLink(WORD wLinkID, TCHAR szBrowseUrl[], UINT uCharCount);
	//导航连接
	LPCTSTR GetNavigationLink(DWORD dwIndexID, TCHAR szBrowseUrl[], UINT uCharCount);

	//游戏页面
public:
	//规则页面
	LPCTSTR GetRedirectionRule(WORD wKindID, TCHAR szBrowseUrl[], UINT uCharCount);
	//排行页面
	LPCTSTR GetRedirectionChart(WORD wKindID, TCHAR szBrowseUrl[], UINT uCharCount);
	//定制页面
	LPCTSTR GetRedirectionCustom(WORD wCustomID, TCHAR szBrowseUrl[], UINT uCharCount);

	//通用页面
public:
	//跳转页面
	LPCTSTR GetRedirectionLink(DWORD dwResourceID, TCHAR szBrowseUrl[], UINT uCharCount);
	//跳转页面
	LPCTSTR GetRedirectionLink(DWORD dwResourceID, WORD wKindID, WORD wServerID, TCHAR szBrowseUrl[], UINT uCharCount);

	//更新页面
public:
	//定向页面
	LPCTSTR GetRedirectionServer( DWORD dwServerType, TCHAR szBrowseUrl[], UINT uCharCount );

	//定制资源
public:
	//获取资源
	LPCTSTR GetCustomString( BOOL bPlatform, LPCTSTR lpAppName, LPCTSTR lpKeyName, LPCTSTR lpDefault, LPTSTR lpRetString, DWORD dwSize );

	//内部函数
private:
	//站点文件
	LPCTSTR GetStationFile(TCHAR szFileName[], UINT uCharCount);
	//平台文件
	LPCTSTR GetPlatformFile(TCHAR szFileName[], UINT uCharCount);
	
	//静态函数
public:
	//获取对象
	static CGlobalStation * GetInstance() { return m_pGlobalStation; }
};

//////////////////////////////////////////////////////////////////////////

#endif