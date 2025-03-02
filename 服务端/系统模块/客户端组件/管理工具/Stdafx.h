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

//MFC 文件
#include <Nb30.h>
#include <AfxInet.h>
#include <strsafe.h>

//#pragma warning( disable : 4995 )

//XML
#import "msxml6.dll" named_guids

//////////////////////////////////////////////////////////////////////////
//平台文件

//平台文件
#include "..\..\全局定义\Platform.h"
#include "..\..\全局定义\RightDefine.h"
#include "..\..\全局定义\ServerRule.h"
#include "..\..\全局定义\Module.h"

//网络命令
#include "..\..\消息定义\CMD_Commom.h"
#include "..\..\消息定义\CMD_GameServer.h"
#include "..\..\消息定义\CMD_LogonServer.h"
#include "..\..\消息定义\CMD_GateServer.h"

//进程命令
//#include "..\..\消息定义\IPC_GameFrame.h"

//类头文件
#include "..\..\公共组件\公共服务\CommonServiceHead.h"
#include "..\..\客户端组件\调试组件\ClientDebugHead.h"
#include "..\..\客户端组件\平台数据\PlatformDataHead.h"
#include "..\..\客户端组件\界面控件\SkinControlsHead.h"
#include "..\..\客户端组件\广场控件\PlazaControlsHead.h"
#include "..\..\客户端组件\游戏控件\ClientControlsHead.h"

//组件文件
#include "..\..\公共组件\进程通讯\ProcessTradeHead.h"
#include "..\..\客户端组件\界面资源\SkinResourceHead.h"
//#include "..\..\客户端组件\形象组件\AvatarControlHead.h"
#include "..\..\客户端组件\服务组件\ClientServiceHead.h"
#include "..\..\客户端组件\更新服务\UpdateServiceHead.h"
#include "..\..\客户端组件\网络服务\NetworkServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//链接代码

#pragma comment (lib,"Winmm.lib")
#pragma comment( lib,"strsafe.lib" )


#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../链接库/ImageAide.lib")
#pragma comment (lib,"../../链接库/SkinControls.lib")
#pragma comment (lib,"../../链接库/PlatformData.lib")
#pragma comment (lib,"../../链接库/PlazaControl.lib")
//#pragma comment (lib,"../../链接库/AvatarControl.lib")
#pragma comment (lib,"../../链接库/CommonService.lib")
#pragma comment (lib,"../../链接库/ClientControls.lib")	
#else
	#pragma comment (lib,"../../链接库/ImageAide.lib")
	#pragma comment (lib,"../../链接库/SkinControls.lib")
	#pragma comment (lib,"../../链接库/PlatformData.lib")
	#pragma comment (lib,"../../链接库/PlazaControl.lib")
	//#pragma comment (lib,"../../链接库/AvatarControl.lib")
	#pragma comment (lib,"../../链接库/CommonService.lib")
	#pragma comment (lib,"../../链接库/ClientControls.lib")
#endif
#else
#ifndef _UNICODE
	#pragma comment (lib,"../../链接库/ImageAideD.lib")
	#pragma comment (lib,"../../链接库/SkinControlsD.lib")
	#pragma comment (lib,"../../链接库/PlatformDataD.lib")
	#pragma comment (lib,"../../链接库/PlazaControlD.lib")
	//#pragma comment (lib,"../../链接库/AvatarControlD.lib")
	#pragma comment (lib,"../../链接库/CommonServiceD.lib")
	#pragma comment (lib,"../../链接库/ClientControlsD.lib")
#else
	#pragma comment (lib,"../../链接库/ImageAideD.lib")
	#pragma comment (lib,"../../链接库/SkinControlsD.lib")
	#pragma comment (lib,"../../链接库/PlatformDataD.lib")
	#pragma comment (lib,"../../链接库/PlazaControlD.lib")
	//#pragma comment (lib,"../../链接库/AvatarControlD.lib")
	#pragma comment (lib,"../../链接库/CommonServiceD.lib")
	#pragma comment (lib,"../../链接库/ClientControlsD.lib")
#endif
#endif


//////////////////////////////////////////////////////////////////////////
//#pragma warning( disable : 4996 )
//#pragma warning( disable : 4805 )