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

//MFC �ļ�
#include <Nb30.h>
#include <AfxInet.h>
#include <strsafe.h>

//#pragma warning( disable : 4995 )

//XML
#import "msxml6.dll" named_guids

//////////////////////////////////////////////////////////////////////////
//ƽ̨�ļ�

//ƽ̨�ļ�
#include "..\..\ȫ�ֶ���\Platform.h"
#include "..\..\ȫ�ֶ���\RightDefine.h"
#include "..\..\ȫ�ֶ���\ServerRule.h"
#include "..\..\ȫ�ֶ���\Module.h"

//��������
#include "..\..\��Ϣ����\CMD_Commom.h"
#include "..\..\��Ϣ����\CMD_GameServer.h"
#include "..\..\��Ϣ����\CMD_LogonServer.h"
#include "..\..\��Ϣ����\CMD_GateServer.h"

//��������
//#include "..\..\��Ϣ����\IPC_GameFrame.h"

//��ͷ�ļ�
#include "..\..\�������\��������\CommonServiceHead.h"
#include "..\..\�ͻ������\�������\ClientDebugHead.h"
#include "..\..\�ͻ������\ƽ̨����\PlatformDataHead.h"
#include "..\..\�ͻ������\����ؼ�\SkinControlsHead.h"
#include "..\..\�ͻ������\�㳡�ؼ�\PlazaControlsHead.h"
#include "..\..\�ͻ������\��Ϸ�ؼ�\ClientControlsHead.h"

//����ļ�
#include "..\..\�������\����ͨѶ\ProcessTradeHead.h"
#include "..\..\�ͻ������\������Դ\SkinResourceHead.h"
//#include "..\..\�ͻ������\�������\AvatarControlHead.h"
#include "..\..\�ͻ������\�������\ClientServiceHead.h"
#include "..\..\�ͻ������\���·���\UpdateServiceHead.h"
#include "..\..\�ͻ������\�������\NetworkServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//���Ӵ���

#pragma comment (lib,"Winmm.lib")
#pragma comment( lib,"strsafe.lib" )


#ifndef _DEBUG
#ifndef _UNICODE
#pragma comment (lib,"../../���ӿ�/ImageAide.lib")
#pragma comment (lib,"../../���ӿ�/SkinControls.lib")
#pragma comment (lib,"../../���ӿ�/PlatformData.lib")
#pragma comment (lib,"../../���ӿ�/PlazaControl.lib")
//#pragma comment (lib,"../../���ӿ�/AvatarControl.lib")
#pragma comment (lib,"../../���ӿ�/CommonService.lib")
#pragma comment (lib,"../../���ӿ�/ClientControls.lib")	
#else
	#pragma comment (lib,"../../���ӿ�/ImageAide.lib")
	#pragma comment (lib,"../../���ӿ�/SkinControls.lib")
	#pragma comment (lib,"../../���ӿ�/PlatformData.lib")
	#pragma comment (lib,"../../���ӿ�/PlazaControl.lib")
	//#pragma comment (lib,"../../���ӿ�/AvatarControl.lib")
	#pragma comment (lib,"../../���ӿ�/CommonService.lib")
	#pragma comment (lib,"../../���ӿ�/ClientControls.lib")
#endif
#else
#ifndef _UNICODE
	#pragma comment (lib,"../../���ӿ�/ImageAideD.lib")
	#pragma comment (lib,"../../���ӿ�/SkinControlsD.lib")
	#pragma comment (lib,"../../���ӿ�/PlatformDataD.lib")
	#pragma comment (lib,"../../���ӿ�/PlazaControlD.lib")
	//#pragma comment (lib,"../../���ӿ�/AvatarControlD.lib")
	#pragma comment (lib,"../../���ӿ�/CommonServiceD.lib")
	#pragma comment (lib,"../../���ӿ�/ClientControlsD.lib")
#else
	#pragma comment (lib,"../../���ӿ�/ImageAideD.lib")
	#pragma comment (lib,"../../���ӿ�/SkinControlsD.lib")
	#pragma comment (lib,"../../���ӿ�/PlatformDataD.lib")
	#pragma comment (lib,"../../���ӿ�/PlazaControlD.lib")
	//#pragma comment (lib,"../../���ӿ�/AvatarControlD.lib")
	#pragma comment (lib,"../../���ӿ�/CommonServiceD.lib")
	#pragma comment (lib,"../../���ӿ�/ClientControlsD.lib")
#endif
#endif


//////////////////////////////////////////////////////////////////////////
//#pragma warning( disable : 4996 )
//#pragma warning( disable : 4805 )