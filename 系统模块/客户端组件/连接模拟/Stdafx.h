#pragma once

//////////////////////////////////////////////////////////////////////////////////
#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

//�汾����
#include "TargetVer.h"
//#define _AFXDLL
#define _AFX_ALL_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS
//////////////////////////////////////////////////////////////////////////


//MFC �ļ�
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <StrSafe.h>
#include <AfxDisp.h>

//MFC �ļ�
#include <Nb30.h>
#include <AfxInet.h>

//////////////////////////////////////////////////////////////////////////
//ƽ̨�ļ�


//////////////////////////////////////////////////////////////////////////
//���Ӵ���

#pragma comment (lib,"Winmm.lib")
#pragma comment( lib,"strsafe.lib" )


#ifndef _DEBUG
#pragma comment (lib,"../../���ӿ�/CommonService.lib")
#pragma comment (lib,"../../���ӿ�/NetworkService.lib")
#else
#pragma comment (lib,"../../���ӿ�/CommonServiceD.lib")
#pragma comment (lib,"../../���ӿ�/NetworkServiceD.lib")
#endif


//////////////////////////////////////////////////////////////////////////
