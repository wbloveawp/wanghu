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

#include "targetver.h"

//MFC �ļ�
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxcMn.h>
#include <StrSafe.h>
#include <AfxDisp.h>
#include <AfxDtctl.h>
#include <AfxControlBars.h>

//////////////////////////////////////////////////////////////////////////
//�����ļ�

//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"
#include "..\..\��Ϣ����\CMD_CenterServer.h"

//�������
#include "..\..\�������\��������\CommonServiceHead.h"
#include "..\..\���������\������\ServiceFrameHead.h"
#include "..\..\���������\�ں�����\KernelEngineHead.h"
#include "..\..\���������\ģ�����\ModuleManagerHead.h"

//////////////////////////////////////////////////////////////////////////
//���Ӵ���

#ifndef _DEBUG
	#pragma comment (lib,"../../���ӿ�/KernelEngine.lib")
	#pragma comment (lib,"../../���ӿ�/ServiceFrame.lib")
	#pragma comment (lib,"../../���ӿ�/CommonService.lib")	
	#pragma comment (lib,"../../���ӿ�/ModuleManager.lib")
#else
	#pragma comment (lib,"../../���ӿ�/KernelEngineD.lib")
	#pragma comment (lib,"../../���ӿ�/ServiceFrameD.lib")
	#pragma comment (lib,"../../���ӿ�/CommonServiceD.lib")	
	#pragma comment (lib,"../../���ӿ�/ModuleManagerD.lib")
#endif

//////////////////////////////////////////////////////////////////////////
