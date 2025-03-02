#pragma once

//////////////////////////////////////////////////////////////////////////////////

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0410
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0400
#endif

#define _ATL_ATTRIBUTES
#define _AFX_ALL_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

//////////////////////////////////////////////////////////////////////////////////

//MFC �ļ�
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>
#include <StrSafe.h>
//////////////////////////////////////////////////////////////////////////////////

//���뻷��
//#include "..\..\..\SDKCondition.h"
#include "..\..\..\..\ϵͳģ��\ȫ�ֶ���\Macro.h"
//////////////////////////////////////////////////////////////////////////////////
#ifdef SDK_CONDITION

//////////////////////////////////////////////////////////////////////////////////
//��������

//ƽ̨ͷ�ļ�
#include "..\..\..\������\Include\GameServiceHead.h"
//
//#ifndef _DEBUG
#pragma comment (lib,"../../../������/lib/CommonService.lib")
//#else
#pragma comment (lib,"../../../������/lib/CommonServiceD.lib")
//#endif

//////////////////////////////////////////////////////////////////////////////////

#else

//////////////////////////////////////////////////////////////////////////////////
//ϵͳ����

//ƽ̨����
#include "..\..\..\..\ϵͳģ��\���������\��Ϸ����\GameServiceHead.h"
#ifndef _DEBUG

#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/CommonService.lib")
#pragma comment (lib,"../../../../��������/sqlite/sqlite3.lib")
#else

#pragma comment (lib,"../../../../ϵͳģ��/���ӿ�/CommonServiceD.lib")
#pragma comment (lib,"../../../../��������/sqlite/sqlite3D.lib")
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif
