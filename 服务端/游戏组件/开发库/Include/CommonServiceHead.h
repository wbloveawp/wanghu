#ifndef COMMON_SERVICE_HEAD_HEAD_FILE
#define COMMON_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//包含文件

//MFC 文件
#include <Nb30.h>
#include <Afxmt.h>
#include <AtlBase.h>
#include <wininet.h>

//平台文件
#include "Platform.h"           

//////////////////////////////////////////////////////////////////////////
//公共定义

//导出定义
#ifndef COMMON_SERVICE_CLASS
	#ifdef COMMON_SERVICE_DLL
		#define COMMON_SERVICE_CLASS _declspec(dllexport)
	#else
		#define COMMON_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define COMMON_SERVICE_DLL_NAME	TEXT("CommonService.dll")			//组件名字
#else
	#define COMMON_SERVICE_DLL_NAME	TEXT("CommonServiceD.dll")			//组件名字
#endif

//////////////////////////////////////////////////////////////////////////
//导出文件

#ifndef COMMON_SERVICE_DLL
	#include "WHJwt.h"
	#include "WHJson.h"
	#include "WHSha1.h"
	#include "WHBase64.h"
	#include "WHService.h"
	#include "WHConHash.h"
	#include "WHDataQueue.h"
	#include "WordFilter.h"
	#include "RegKeyAide.h"
	#include "ThreadLock.h"
	#include "Snowflake.h"
	#include "DataEncrypt.h"	
	#include "WininetHttp.h"
	#include "CrashCapture.h"
	#include "ServiceThread.h"	
	#include "WHCustomFile.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif