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

//////////////////////////////////////////////////////////////////////////

//MFC �ļ�
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>

//////////////////////////////////////////////////////////////////////////
//���Ӵ���

#ifndef _DEBUG
#ifndef _UNICODE
	#pragma comment (lib,"../../���ӿ�/ImageAide.lib")
	#pragma comment (lib,"../../���ӿ�/CommonService.lib")
	#pragma comment (lib,"../../���ӿ�/SkinControls.lib")
	#pragma comment (lib,"../../���ӿ�/ClientControls.lib")
#else
	#pragma comment (lib,"../../���ӿ�/ImageAide.lib")
	#pragma comment (lib,"../../���ӿ�/CommonService.lib")
	#pragma comment (lib,"../../���ӿ�/SkinControls.lib")
	#pragma comment (lib,"../../���ӿ�/ClientControls.lib")
#endif
#else
#ifndef _UNICODE
	#pragma comment (lib,"../../���ӿ�/ImageAideD.lib")
	#pragma comment (lib,"../../���ӿ�/CommonServiceD.lib")
	#pragma comment (lib,"../../���ӿ�/SkinControlsD.lib")
	#pragma comment (lib,"../../���ӿ�/ClientControlsD.lib")
#else
	#pragma comment (lib,"../../���ӿ�/ImageAideD.lib")
	#pragma comment (lib,"../../���ӿ�/CommonServiceD.lib")
	#pragma comment (lib,"../../���ӿ�/SkinControlsD.lib")
	#pragma comment (lib,"../../���ӿ�/ClientControlsD.lib")
#endif
#endif

//////////////////////////////////////////////////////////////////////////

