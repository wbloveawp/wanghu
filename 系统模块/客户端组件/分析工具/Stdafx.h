#pragma once

//////////////////////////////////////////////////////////////////////////////////
//MFC ����


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
//////////////////////////////////////////////////////////////////////////////////

//�汾����
#include "TargetVer.h"
//MFC �ļ�
#include <Nb30.h>
#include <AfxInet.h>

#include <map>
#include <string>
#include <vector>
#include <assert.h>
#include <stdio.h>
#include <unordered_map>


//////////////////////////////////////////////////////////////////////////
//�����ļ�

#include "..\..\ȫ�ֶ���\Macro.h"
#include "..\..\ȫ�ֶ���\Module.h"

//�������

#include "..\..\..\��������\sqlite\sqlite3.h"

//ƽ̨�ļ�
#include "..\..\�������\��������\CommonServiceHead.h"
#include "..\..\�ͻ������\�������\NetworkServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//���Ӵ���

#ifndef _DEBUG
#pragma comment (lib,"../../���ӿ�/CommonService.lib")
#pragma comment (lib,"../../���ӿ�/NetworkService.lib")
#pragma comment (lib,"../../../��������/sqlite/sqlite3.lib")
#else
#pragma comment (lib,"../../���ӿ�/CommonServiceD.lib")
#pragma comment (lib,"../../���ӿ�/NetworkServiceD.lib")
#pragma comment (lib,"../../../��������/sqlite/sqlite3D.lib")
#endif

//////////////////////////////////////////////////////////////////////////////////

#pragma comment (lib,"Winmm.lib")


//////////////////////////////////////////////////////////////////////////
