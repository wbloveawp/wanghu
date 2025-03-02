#ifndef SKIN_CONTROLS_HEAD_HEAD_FILE
#define SKIN_CONTROLS_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//MFC 文件
#include <RichOle.h>
#include <AtlImage.h>
#include <afxtempl.h>

//平台文件
#include "..\..\全局定义\Platform.h"
#include "..\..\客户端组件\图像助手\ImageAideHead.h"
#include "..\..\客户端组件\界面资源\SkinResourceHead.h"

//////////////////////////////////////////////////////////////////////////
//公共定义

//导出定义
#ifndef SKIN_CONTROL_CLASS
	#ifdef  SKIN_CONTROL_DLL
		#define SKIN_CONTROL_CLASS _declspec(dllexport)
	#else
		#define SKIN_CONTROL_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define SKIN_CONTROL_DLL_NAME	TEXT("SkinControls.dll")			//组件 DLL 名字
#else
	#define SKIN_CONTROL_DLL_NAME	TEXT("SkinControlsD.dll")			//组件 DLL 名字
#endif

//////////////////////////////////////////////////////////////////////////

//常量定义
#define CX_DEF_FONT					6									//字体宽度
#define CY_DEF_FONT					12									//字体高度

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ISkinMouseEvent INTERFACE_VERSION(1,1)
	static const GUID IID_ISkinMouseEvent={0x89535ac0,0x2a14,0x4093,0x00ae,0xe1,0xd9,0x3f,0x22,0x92,0xfa,0x35};
#else
	#define VER_ISkinMouseEvent INTERFACE_VERSION(1,1)
	static const GUID IID_ISkinMouseEvent={0x491fbebf,0x8035,0x460c,0x00b1,0xa9,0x5a,0x60,0x24,0xa8,0xd6,0xf4};
#endif

//按钮事件
interface ISkinMouseEvent : public IUnknownEx
{
	//鼠标事件
	virtual bool __cdecl OnEventMouse(UINT uMessage, UINT uControlID, UINT nFlags, INT nXPos, INT nYPos)=NULL;
};

//////////////////////////////////////////////////////////////////////////
//导出文件

#ifndef SKIN_CONTROL_DLL
	#include "SkinAide.h"
	#include "SkinMenu.h"
	#include "SkinEdit.h"
	#include "SkinButton.h"
	#include "SkinDialog.h"
	#include "SkinListCtrl.h"
	#include "SkinSplitter.h"
	#include "SkinRichEdit.h"
	#include "SkinComboBox.h"
	#include "SkinHyperLink.h"
	#include "SkinTabControl.h"
	#include "SkinScrollBar.h"
	#include "SkinResourceManager.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif