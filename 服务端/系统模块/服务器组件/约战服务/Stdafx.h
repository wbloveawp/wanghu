#pragma once


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
#include <StrSafe.h>

#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>          
#endif

#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>
#endif 

#include <afxcontrolbars.h>

//////////////////////////////////////////////////////////////////////////
//���ͷ�ļ�

#include "..\..\���������\��Ϸ����\GameServiceHead.h"
#include "..\..\���������\���÷���\ConfigServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//���Ӵ���

#ifndef _DEBUG
		#pragma comment (lib,"../../���ӿ�/CommonService.lib")
		#pragma comment (lib,"../../���ӿ�/GameService.lib")
		#pragma comment (lib,"../../���ӿ�/KernelEngine.lib")
		#pragma comment (lib,"../../���ӿ�/ConfigService.lib")
#else
		#pragma comment (lib,"../../���ӿ�/CommonServiceD.lib")
		#pragma comment (lib,"../../���ӿ�/GameServiceD.lib")
		#pragma comment (lib,"../../���ӿ�/KernelEngineD.lib")
		#pragma comment (lib,"../../���ӿ�/ConfigServiceD.lib")
#endif

//////////////////////////////////////////////////////////////////////////



