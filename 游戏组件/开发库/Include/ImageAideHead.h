#ifndef IMAGE_AIDE_HEAD_HEAD_FILE
#define IMAGE_AIDE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//MFC 文件
#include <GdiPlus.h>
#include <AtlImage.h>

//命名空间
using namespace Gdiplus;

//平台头文件
#include "Platform.h"
#include "CommonServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//公共宏定义

//导出定义
#ifndef IMAGE_AIDE_CLASS
	#ifdef  IMAGE_AIDE_DLL
		#define IMAGE_AIDE_CLASS _declspec(dllexport)
	#else
		#define IMAGE_AIDE_CLASS _declspec(dllimport)
	#endif
#endif

//////////////////////////////////////////////////////////////////////////

//位图加载
struct tagImageLoadInfo
{
	//外部信息
	CString							strFileName;						//文件名字

	//资源信息
	LPCTSTR							pszResource;						//资源标识
	HINSTANCE						hResourceDLL;						//资源句柄
};

//////////////////////////////////////////////////////////////////////////
//导出文件

#ifndef IMAGE_AIDE_DLL
	#include "BitImage.h"
	#include "DibImage.h"
	#include "PngImage.h"
	#include "GIFImage.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif