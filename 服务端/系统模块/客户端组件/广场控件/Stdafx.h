#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持

//////////////////////////////////////////////////////////////////////////

//MFC 文件
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>

//////////////////////////////////////////////////////////////////////////
//链接代码

#ifndef _DEBUG
#ifndef _UNICODE
	#pragma comment (lib,"../../链接库/ImageAide.lib")
	#pragma comment (lib,"../../链接库/CommonService.lib")
	#pragma comment (lib,"../../链接库/SkinControls.lib")
	#pragma comment (lib,"../../链接库/ClientControls.lib")
#else
	#pragma comment (lib,"../../链接库/ImageAide.lib")
	#pragma comment (lib,"../../链接库/CommonService.lib")
	#pragma comment (lib,"../../链接库/SkinControls.lib")
	#pragma comment (lib,"../../链接库/ClientControls.lib")
#endif
#else
#ifndef _UNICODE
	#pragma comment (lib,"../../链接库/ImageAideD.lib")
	#pragma comment (lib,"../../链接库/CommonServiceD.lib")
	#pragma comment (lib,"../../链接库/SkinControlsD.lib")
	#pragma comment (lib,"../../链接库/ClientControlsD.lib")
#else
	#pragma comment (lib,"../../链接库/ImageAideD.lib")
	#pragma comment (lib,"../../链接库/CommonServiceD.lib")
	#pragma comment (lib,"../../链接库/SkinControlsD.lib")
	#pragma comment (lib,"../../链接库/ClientControlsD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////

