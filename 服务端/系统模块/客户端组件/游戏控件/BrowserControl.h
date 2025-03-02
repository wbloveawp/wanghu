#ifndef BROWSER_CONTROL_HEAD_FILE
#define BROWSER_CONTROL_HEAD_FILE

#pragma once

#include "ClientControlsHead.h"

//////////////////////////////////////////////////////////////////////////

//浏览窗口
class CLIENT_CONTROLS_CLASS CWebBrowser : public CWnd
{
	//变量定义
public:
	enum
	{
		CSC_UPDATECOMMANDS=-1,
		CSC_NAVIGATEFORWARD=1,
		CSC_NAVIGATEBACK=2
	}CommandStateChangeConstants;
	enum
	{
		OLECMDID_OPEN=1,
		OLECMDID_NEW=2,
		OLECMDID_SAVE=3,
		OLECMDID_SAVEAS=4,
		OLECMDID_SAVECOPYAS=5,
		OLECMDID_PRINT=6,
		OLECMDID_PRINTPREVIEW=7,
		OLECMDID_PAGESETUP=8,
		OLECMDID_SPELL=9,
		OLECMDID_PROPERTIES=10,
		OLECMDID_CUT=11,
		OLECMDID_COPY=12,
		OLECMDID_PASTE=13,
		OLECMDID_PASTESPECIAL=14,
		OLECMDID_UNDO=15,
		OLECMDID_REDO=16,
		OLECMDID_SELECTALL=17,
		OLECMDID_CLEARSELECTION=18,
		OLECMDID_ZOOM=19,
		OLECMDID_GETZOOMRANGE=20,
		OLECMDID_UPDATECOMMANDS=21,
		OLECMDID_REFRESH=22,
		OLECMDID_STOP=23,
		OLECMDID_HIDETOOLBARS=24,
		OLECMDID_SETPROGRESSMAX=25,
		OLECMDID_SETPROGRESSPOS=26,
		OLECMDID_SETPROGRESSTEXT=27,
		OLECMDID_SETTITLE=28,
		OLECMDID_SETDOWNLOADSTATE=29,
		OLECMDID_STOPDOWNLOAD=30,
		OLECMDID_ONTOOLBARACTIVATED=31,
		OLECMDID_FIND=32,
		OLECMDID_DELETE=33,
		OLECMDID_HTTPEQUIV=34,
		OLECMDID_HTTPEQUIV_DONE=35,
		OLECMDID_ENABLE_INTERACTION=36,
		OLECMDID_ONUNLOAD=37,
		OLECMDID_PROPERTYBAG2=38,
		OLECMDID_PREREFRESH=39,
		OLECMDID_SHOWSCRIPTERROR=40,
		OLECMDID_SHOWMESSAGE=41,
		OLECMDID_SHOWFIND=42,
		OLECMDID_SHOWPAGESETUP=43,
		OLECMDID_SHOWPRINT=44,
		OLECMDID_CLOSE=45,
		OLECMDID_ALLOWUILESSSAVEAS=46,
		OLECMDID_DONTDOWNLOADCSS=47,
		OLECMDID_UPDATEPAGESTATUS=48,
		OLECMDID_PRINT2=49,
		OLECMDID_PRINTPREVIEW2=50,
		OLECMDID_SETPRINTTEMPLATE=51,
		OLECMDID_GETPRINTTEMPLATE=52
	}OLECMDID;
	enum
	{
		OLECMDF_SUPPORTED=1,
		OLECMDF_ENABLED=2,
		OLECMDF_LATCHED=4,
		OLECMDF_NINCHED=8,
		OLECMDF_INVISIBLE=16,
		OLECMDF_DEFHIDEONCTXTMENU=32
	}OLECMDF;
	enum
	{
		OLECMDEXECOPT_DODEFAULT=0,
		OLECMDEXECOPT_PROMPTUSER=1,
		OLECMDEXECOPT_DONTPROMPTUSER=2,
		OLECMDEXECOPT_SHOWHELP=3
	}OLECMDEXECOPT;
	enum
	{
		READYSTATE_UNINITIALIZED=0,
		READYSTATE_LOADING=1,
		READYSTATE_LOADED=2,
		READYSTATE_INTERACTIVE=3,
		READYSTATE_COMPLETE=4
	}tagREADYSTATE;
	enum
	{
		secureLockIconUnsecure=0,
		secureLockIconMixed=1,
		secureLockIconSecureUnknownBits=2,
		secureLockIconSecure40Bit=3,
		secureLockIconSecure56Bit=4,
		secureLockIconSecureFortezza=5,
		secureLockIconSecure128Bit=6
	}SecureLockIconConstants;
	enum
	{
		SWC_EXPLORER=0,
		SWC_BROWSER=1,
		SWC_3RDPARTY=2,
		SWC_CALLBACK=4
	}ShellWindowTypeConstants;
	enum
	{
		SWFO_NEEDDISPATCH=1,
		SWFO_INCLUDEPENDING=2,
		SWFO_COOKIEPASSED=4
	}ShellWindowFindWindowOptions;

	//函数定义
public:
	CLSID const & GetClsid()
	{
		static CLSID const clsid={0x8856F961,0x340A,0x11D0,{0xA9,0x6B,0x0,0xC0,0x4F,0xD7,0x5,0xA2}};
		return clsid;
	}
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd * pParentWnd, UINT nID, 
		CCreateContext * pContext=NULL)
	{ 
		return CreateControl(GetClsid(),lpszWindowName,dwStyle,rect,pParentWnd,nID); 
	}

	BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd * pParentWnd, 
		UINT nID, CFile * pPersist=NULL, BOOL bStorage=FALSE, BSTR bstrLicKey=NULL)
	{ 
		return CreateControl(GetClsid(),lpszWindowName,dwStyle,rect,pParentWnd,nID,pPersist,bStorage,bstrLicKey);
	}

	//函数定义
public:
	VOID GoBack()
	{
		InvokeHelper(0x64,DISPATCH_METHOD,VT_EMPTY,NULL,NULL);
	}
	VOID GoForward()
	{
		InvokeHelper(0x65,DISPATCH_METHOD,VT_EMPTY,NULL,NULL);
	}
	VOID GoHome()
	{
		InvokeHelper(0x66,DISPATCH_METHOD,VT_EMPTY,NULL,NULL);
	}
	VOID GoSearch()
	{
		InvokeHelper(0x67,DISPATCH_METHOD,VT_EMPTY,NULL,NULL);
	}
	VOID Navigate(LPCTSTR URL, VARIANT * Flags, VARIANT * TargetFrameName, VARIANT * PostData, VARIANT * Headers)
	{
		static BYTE parms[]=VTS_BSTR VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT;
		InvokeHelper(0x68,DISPATCH_METHOD,VT_EMPTY,NULL,parms,URL,Flags,TargetFrameName,PostData,Headers);
	}
	VOID Refresh()
	{
		InvokeHelper(DISPID_REFRESH,DISPATCH_METHOD,VT_EMPTY,NULL,NULL);
	}
	VOID Refresh2(VARIANT * Level)
	{
		static BYTE parms[]=VTS_PVARIANT;
		InvokeHelper(0x69,DISPATCH_METHOD,VT_EMPTY,NULL,parms,Level);
	}
	VOID Stop()
	{
		InvokeHelper(0x6a,DISPATCH_METHOD,VT_EMPTY,NULL,NULL);
	}
	LPDISPATCH get_Application()
	{
		LPDISPATCH result;
		InvokeHelper(0xc8,DISPATCH_PROPERTYGET,VT_DISPATCH,(VOID *)&result,NULL);
		return result;
	}
	LPDISPATCH get_Parent()
	{
		LPDISPATCH result;
		InvokeHelper(0xc9,DISPATCH_PROPERTYGET,VT_DISPATCH,(VOID *)&result,NULL);
		return result;
	}
	LPDISPATCH get_Container()
	{
		LPDISPATCH result;
		InvokeHelper(0xca,DISPATCH_PROPERTYGET,VT_DISPATCH,(VOID *)&result,NULL);
		return result;
	}
	LPDISPATCH get_Document()
	{
		LPDISPATCH result;
		InvokeHelper(0xcb,DISPATCH_PROPERTYGET,VT_DISPATCH,(VOID *)&result,NULL);
		return result;
	}
	BOOL get_TopLevelContainer()
	{
		BOOL result;
		InvokeHelper(0xcc,DISPATCH_PROPERTYGET,VT_BOOL,(VOID *)&result,NULL);
		return result;
	}
	CString get_Type()
	{
		CString result;
		InvokeHelper(0xcd,DISPATCH_PROPERTYGET,VT_BSTR,(VOID *)&result,NULL);
		return result;
	}
	LONG get_Left()
	{
		LONG result;
		InvokeHelper(0xce,DISPATCH_PROPERTYGET,VT_I4,(VOID *)&result,NULL);
		return result;
	}
	VOID put_Left(LONG newValue)
	{
		static BYTE parms[]=VTS_I4;
		InvokeHelper(0xce, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
	LONG get_Top()
	{
		LONG result;
		InvokeHelper(0xcf,DISPATCH_PROPERTYGET,VT_I4,(VOID *)&result,NULL);
		return result;
	}
	VOID put_Top(LONG newValue)
	{
		static BYTE parms[]=VTS_I4;
		InvokeHelper(0xcf, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
	LONG get_Width()
	{
		LONG result;
		InvokeHelper(0xd0,DISPATCH_PROPERTYGET,VT_I4,(VOID *)&result,NULL);
		return result;
	}
	VOID put_Width(LONG newValue)
	{
		static BYTE parms[]=VTS_I4;
		InvokeHelper(0xd0, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
	LONG get_Height()
	{
		LONG result;
		InvokeHelper(0xd1,DISPATCH_PROPERTYGET,VT_I4,(VOID *)&result,NULL);
		return result;
	}
	VOID put_Height(LONG newValue)
	{
		static BYTE parms[]=VTS_I4;
		InvokeHelper(0xd1, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
	CString get_LocationName()
	{
		CString result;
		InvokeHelper(0xd2,DISPATCH_PROPERTYGET,VT_BSTR,(VOID *)&result,NULL);
		return result;
	}
	CString get_LocationURL()
	{
		CString result;
		InvokeHelper(0xd3,DISPATCH_PROPERTYGET,VT_BSTR,(VOID *)&result,NULL);
		return result;
	}
	BOOL get_Busy()
	{
		BOOL result;
		InvokeHelper(0xd4,DISPATCH_PROPERTYGET,VT_BOOL,(VOID *)&result,NULL);
		return result;
	}
	VOID Quit()
	{
		InvokeHelper(0x12c,DISPATCH_METHOD,VT_EMPTY,NULL,NULL);
	}
	VOID ClientToWindow(LONG * pcx, LONG * pcy)
	{
		static BYTE parms[]=VTS_PI4 VTS_PI4;
		InvokeHelper(0x12d,DISPATCH_METHOD,VT_EMPTY,NULL,parms,pcx,pcy);
	}
	VOID PutProperty(LPCTSTR Property, VARIANT vtValue)
	{
		static BYTE parms[]=VTS_BSTR VTS_VARIANT;
		InvokeHelper(0x12e,DISPATCH_METHOD,VT_EMPTY,NULL,parms,Property,&vtValue);
	}
	VARIANT GetProperty(LPCTSTR Property)
	{
		VARIANT result;
		static BYTE parms[]=VTS_BSTR;
		InvokeHelper(0x12f,DISPATCH_METHOD,VT_VARIANT,(VOID *)&result,parms,Property);
		return result;
	}
	CString get_Name()
	{
		CString result;
		InvokeHelper(0x0,DISPATCH_PROPERTYGET,VT_BSTR,(VOID *)&result,NULL);
		return result;
	}
	LONG get_HWND()
	{
		LONG result;
		InvokeHelper(DISPID_HWND,DISPATCH_PROPERTYGET,VT_I4,(VOID *)&result,NULL);
		return result;
	}
	CString get_FullName()
	{
		CString result;
		InvokeHelper(0x190,DISPATCH_PROPERTYGET,VT_BSTR,(VOID *)&result,NULL);
		return result;
	}
	CString get_Path()
	{
		CString result;
		InvokeHelper(0x191,DISPATCH_PROPERTYGET,VT_BSTR,(VOID *)&result,NULL);
		return result;
	}
	BOOL get_Visible()
	{
		BOOL result;
		InvokeHelper(0x192,DISPATCH_PROPERTYGET,VT_BOOL,(VOID *)&result,NULL);
		return result;
	}
	VOID put_Visible(BOOL newValue)
	{
		static BYTE parms[]=VTS_BOOL;
		InvokeHelper(0x192, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
	BOOL get_StatusBar()
	{
		BOOL result;
		InvokeHelper(0x193,DISPATCH_PROPERTYGET,VT_BOOL,(VOID *)&result,NULL);
		return result;
	}
	VOID put_StatusBar(BOOL newValue)
	{
		static BYTE parms[]=VTS_BOOL;
		InvokeHelper(0x193, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
	CString get_StatusText()
	{
		CString result;
		InvokeHelper(0x194,DISPATCH_PROPERTYGET,VT_BSTR,(VOID *)&result,NULL);
		return result;
	}
	VOID put_StatusText(LPCTSTR newValue)
	{
		static BYTE parms[]=VTS_BSTR;
		InvokeHelper(0x194, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
	LONG get_ToolBar()
	{
		LONG result;
		InvokeHelper(0x195,DISPATCH_PROPERTYGET,VT_I4,(VOID *)&result,NULL);
		return result;
	}
	VOID put_ToolBar(LONG newValue)
	{
		static BYTE parms[]=VTS_I4;
		InvokeHelper(0x195, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
	BOOL get_MenuBar()
	{
		BOOL result;
		InvokeHelper(0x196,DISPATCH_PROPERTYGET,VT_BOOL,(VOID *)&result,NULL);
		return result;
	}
	VOID put_MenuBar(BOOL newValue)
	{
		static BYTE parms[]=VTS_BOOL;
		InvokeHelper(0x196, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
	BOOL get_FullScreen()
	{
		BOOL result;
		InvokeHelper(0x197,DISPATCH_PROPERTYGET,VT_BOOL,(VOID *)&result,NULL);
		return result;
	}
	VOID put_FullScreen(BOOL newValue)
	{
		static BYTE parms[]=VTS_BOOL;
		InvokeHelper(0x197, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
	VOID Navigate2(VARIANT * URL, VARIANT * Flags, VARIANT * TargetFrameName, VARIANT * PostData, VARIANT * Headers)
	{
		static BYTE parms[]=VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT;
		InvokeHelper(0x1f4,DISPATCH_METHOD,VT_EMPTY,NULL,parms,URL,Flags,TargetFrameName,PostData,Headers);
	}
	LONG QueryStatusWB(LONG cmdID)
	{
		LONG result;
		static BYTE parms[]=VTS_I4;
		InvokeHelper(0x1f5,DISPATCH_METHOD,VT_I4,(VOID *)&result,parms,cmdID);
		return result;
	}
	VOID ExecWB(LONG cmdID, LONG cmdexecopt, VARIANT * pvaIn, VARIANT * pvaOut)
	{
		static BYTE parms[]=VTS_I4 VTS_I4 VTS_PVARIANT VTS_PVARIANT;
		InvokeHelper(0x1f6,DISPATCH_METHOD,VT_EMPTY,NULL,parms,cmdID,cmdexecopt,pvaIn,pvaOut);
	}
	VOID ShowBrowserBar(VARIANT * pvaClsid, VARIANT * pvarShow, VARIANT * pvarSize)
	{
		static BYTE parms[]=VTS_PVARIANT VTS_PVARIANT VTS_PVARIANT;
		InvokeHelper(0x1f7,DISPATCH_METHOD,VT_EMPTY,NULL,parms,pvaClsid,pvarShow,pvarSize);
	}
	LONG get_ReadyState()
	{
		LONG result;
		InvokeHelper(DISPID_READYSTATE,DISPATCH_PROPERTYGET,VT_I4,(VOID *)&result,NULL);
		return result;
	}
	BOOL get_Offline()
	{
		BOOL result;
		InvokeHelper(0x226,DISPATCH_PROPERTYGET,VT_BOOL,(VOID *)&result,NULL);
		return result;
	}
	VOID put_Offline(BOOL newValue)
	{
		static BYTE parms[]=VTS_BOOL;
		InvokeHelper(0x226, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
	BOOL get_Silent()
	{
		BOOL result;
		InvokeHelper(0x227,DISPATCH_PROPERTYGET,VT_BOOL,(VOID *)&result,NULL);
		return result;
	}
	VOID put_Silent(BOOL newValue)
	{
		static BYTE parms[]=VTS_BOOL;
		InvokeHelper(0x227, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
	BOOL get_RegisterAsBrowser()
	{
		BOOL result;
		InvokeHelper(0x228,DISPATCH_PROPERTYGET,VT_BOOL,(VOID *)&result,NULL);
		return result;
	}
	VOID put_RegisterAsBrowser(BOOL newValue)
	{
		static BYTE parms[]=VTS_BOOL;
		InvokeHelper(0x228, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
	BOOL get_RegisterAsDropTarget()
	{
		BOOL result;
		InvokeHelper(0x229,DISPATCH_PROPERTYGET,VT_BOOL,(VOID *)&result,NULL);
		return result;
	}
	VOID put_RegisterAsDropTarget(BOOL newValue)
	{
		static BYTE parms[]=VTS_BOOL;
		InvokeHelper(0x229, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
	BOOL get_TheaterMode()
	{
		BOOL result;
		InvokeHelper(0x22a,DISPATCH_PROPERTYGET,VT_BOOL,(VOID *)&result,NULL);
		return result;
	}
	VOID put_TheaterMode(BOOL newValue)
	{
		static BYTE parms[]=VTS_BOOL;
		InvokeHelper(0x22a, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
	BOOL get_AddressBar()
	{
		BOOL result;
		InvokeHelper(0x22b,DISPATCH_PROPERTYGET,VT_BOOL,(VOID *)&result,NULL);
		return result;
	}
	VOID put_AddressBar(BOOL newValue)
	{
		static BYTE parms[]=VTS_BOOL;
		InvokeHelper(0x22b, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
	BOOL get_Resizable()
	{
		BOOL result;
		InvokeHelper(0x22c,DISPATCH_PROPERTYGET,VT_BOOL,(VOID *)&result,NULL);
		return result;
	}
	VOID put_Resizable(BOOL newValue)
	{
		static BYTE parms[]=VTS_BOOL;
		InvokeHelper(0x22c, DISPATCH_PROPERTYPUT,VT_EMPTY,NULL,parms,newValue);
	}
};

//////////////////////////////////////////////////////////////////////////

//浏览控件
class CLIENT_CONTROLS_CLASS CHtmlBrowser : public CHtmlView
{
	//函数定义
public:
	//构造函数
	CHtmlBrowser();
	//析构函数
	virtual ~CHtmlBrowser();

	//重载函数
protected:
	//绑定函数
	virtual VOID PreSubclassWindow();
	//消耗处理
	virtual VOID PostNcDestroy();

public:
	//创建函数
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);

	//消息映射
protected:
	//绘画背景
	BOOL OnEraseBkgnd(CDC * pDC);
	//创建消息
	INT OnCreate(LPCREATESTRUCT lpCreateStruct);
	//IE关闭事件
	VOID OnWindowClosing(BOOL IsChildWindow, BOOL *bCancel);

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif