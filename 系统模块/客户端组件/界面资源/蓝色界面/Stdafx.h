#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��

//MFC �ļ�
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDtctl.h>

//////////////////////////////////////////////////////////////////////////
//ƽ̨�ļ�

#include "..\..\..\ȫ�ֶ���\Platform.h"
#include "..\..\..\�ͻ������\ͼ������\ImageAideHead.h"
#include "..\..\..\�ͻ������\������Դ\SkinResourceHead.h"

//////////////////////////////////////////////////////////////////////////

//ģ�鶨��
#ifndef _DEBUG
	#define SKIN_RESOURCE_DLL_NAME	TEXT("BuleResource.dll")			//��� DLL ����
#else
	#define SKIN_RESOURCE_DLL_NAME	TEXT("BuleResourceD.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////
