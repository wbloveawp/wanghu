#pragma once

//////////////////////////////////////////////////////////////////////////////////
//MFC ����

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#define _AFX_ALL_WARNINGS
#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS

//////////////////////////////////////////////////////////////////////////////////

//�汾����
#include "TargetVer.h"

//MFC �ļ�
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxcMn.h>
#include <Strsafe.h>
#include <AfxDisp.h>
#include <AfxDtctl.h>
#include <Winternl.h>
#include <Tlhelp32.h>
#include <Ws2tcpip.h>
#include <AfxControlBars.h>

//////////////////////////////////////////////////////////////////////////////////
//�����ļ�

//ƽ̨����
#include "..\..\ϵͳģ��\ȫ�ֶ���\Platform.h"

//�������
#include "..\..\ϵͳģ��\�������\��������\CommonServiceHead.h"
#include "..\..\ϵͳģ��\���������\������\ServiceFrameHead.h"
#include "..\..\�������\��������\�ں˴���\KernelCodeHead.h"
#include "..\..\�������\��������\�ں�����\KernelEngineHead.h"
#include "..\..\�������\��������\�ں�����\KernelEngineCore.h"

//////////////////////////////////////////////////////////////////////////////////
//���Ӵ���

#ifndef _DEBUG
#pragma comment (lib,"../../ϵͳģ��/���ӿ�/KernelCode.lib")
#pragma comment (lib,"../../ϵͳģ��/���ӿ�/KernelEngine.lib")
#pragma comment (lib,"../../ϵͳģ��/���ӿ�/ServiceFrame.lib")
#pragma comment (lib,"../../ϵͳģ��/���ӿ�/CommonService.lib")	
#else
#pragma comment (lib,"../../ϵͳģ��/���ӿ�/KernelCodeD.lib")
#pragma comment (lib,"../../ϵͳģ��/���ӿ�/KernelEngineD.lib")
#pragma comment (lib,"../../ϵͳģ��/���ӿ�/ServiceFrameD.lib")
#pragma comment (lib,"../../ϵͳģ��/���ӿ�/CommonServiceD.lib")
#endif

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#if defined _M_IX86
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#elif defined _M_X64
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#else
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#endif
#endif

//////////////////////////////////////////////////////////////////////////////////
