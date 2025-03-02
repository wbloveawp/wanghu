#ifndef PLATFORM_DATA_HEAD_HEAD_FILE
#define PLATFORM_DATA_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//平台文件

//平台文件
#include "..\..\全局定义\Platform.h"
#include "..\..\全局定义\BrowserDef.h"

//类头文件
#include "..\..\客户端组件\图像助手\ImageAideHead.h"
#include "..\..\客户端组件\界面资源\SkinResourceHead.h"
#include "..\..\客户端组件\网络服务\NetworkServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//公共定义

//导出定义
#ifndef PLATFORM_DATA_CLASS
	#ifdef  PLATFORM_DATA_DLL
		#define PLATFORM_DATA_CLASS _declspec(dllexport)
	#else
		#define PLATFORM_DATA_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define PLATFORM_DATA_DLL_NAME	TEXT("PlatformData.dll")			//组件 DLL 名字
#else
	#define PLATFORM_DATA_DLL_NAME	TEXT("PlatformDataD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////
//连接地址

//成为会员
const TCHAR szApplyMember[]=TEXT("http://www.abcdefgh.com/pay/Member.aspx");

//交友中心
const TCHAR szFirendCenter[]=TEXT("http://friend.abcdefgh.com");

//区域帮助
const TCHAR szAreaInfoNote[]=TEXT("http://services.abcdefgh.com/GameHelp/AreaInfoNote.aspx");

//////////////////////////////////////////////////////////////////////////
//导出文件

#ifndef PLATFORM_DATA_DLL
	#include "GlobalServer.h"
	#include "GlobalStation.h"
	#include "GlobalUserInfo.h"
	#include "OptionParameter.h"
	#include "GlobalService.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif