#pragma once

//////////////////////////////////////////////////////////////////////////
//MFC �ļ�

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN         
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      

#define _AFX_ALL_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

// MFCͷ�ļ�
#include <afxwin.h>
#include <afxext.h>
#include <afxdisp.h>
#include <afxSock.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>          
#endif

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif 

#include <afxcontrolbars.h>

//////////////////////////////////////////////////////////////////////////////////

//ƽ̨�ļ�
#include "..\..\ϵͳģ��\ȫ�ֶ���\Platform.h"
#include "..\..\ϵͳģ��\�ͻ������\����ؼ�\SkinControlsHead.h"

//////////////////////////////////////////////////////////////////////////////////
//���Ӵ���

#ifndef _DEBUG
	#pragma comment (lib,"../../ϵͳģ��/���ӿ�/SkinControls.lib")
#else
	#pragma comment (lib,"../../ϵͳģ��/���ӿ�/SkinControlsD.lib")
#endif

//////////////////////////////////////////////////////////////////////////////////
