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
#include <StrSafe.h>
#include <AfxDisp.h>
#include <AfxDtctl.h>
#include <AfxControlBars.h>


//////////////////////////////////////////////////////////////////////////

//�����ļ�
#include "..\..\ȫ�ֶ���\Condition.h"
#include "..\..\ȫ�ֶ���\Platform.h"
#include "..\..\��Ϣ����\CMD_Validate.h"
#include "..\..\��Ϣ����\CMD_LogonServer.h"

//����ģ��
#include "..\..\���������\���÷���\ConfigServiceHead.h"
#include "..\..\���������\ģ�����\ModuleManagerHead.h"
#include "..\..\���������\��־����\LoggerServiceHead.h"
#include "..\..\���������\��ط���\MonitorServiceHead.h"

//¼�����
#ifdef VIDEO_SERVICE_MODULE
	#include "..\..\���������\¼�����\VideoServiceHead.h"
#endif

//��������
#ifdef MATCH_SERVICE_MODULE
	#include "..\..\���������\��������\MatchServiceHead.h"
#endif

//Լս����
#ifdef BATTLE_SERVICE_MODULE
	#include "..\..\���������\Լս����\BattleServiceHead.h"
#endif

//��λ����
#ifdef RANKING_SERVICE_MODULE
	#include "..\..\���������\��λ����\RankingServiceHead.h"
#endif

//////////////////////////////////////////////////////////////////////////
//���Ӵ���

#ifndef _DEBUG
	#pragma comment (lib,"../../���ӿ�/KernelEngine.lib")	
	#pragma comment (lib,"../../���ӿ�/CommonService.lib")
	#pragma comment (lib,"../../���ӿ�/ConfigService.lib")
	#pragma comment (lib,"../../���ӿ�/ServiceFrame.lib")
	#pragma comment (lib,"../../���ӿ�/ProcessTrade.lib")
	#pragma comment (lib,"../../���ӿ�/ModuleManager.lib")
	#pragma comment (lib,"../../���ӿ�/MonitorService.lib")
	#pragma comment (lib,"../../���ӿ�/LoggerService.lib")	
#else
	#pragma comment (lib,"../../���ӿ�/KernelEngineD.lib")
	#pragma comment (lib,"../../���ӿ�/ServiceFrameD.lib")
	#pragma comment (lib,"../../���ӿ�/ProcessTradeD.lib")
	#pragma comment (lib,"../../���ӿ�/ConfigServiceD.lib")
	#pragma comment (lib,"../../���ӿ�/CommonServiceD.lib")
	#pragma comment (lib,"../../���ӿ�/ModuleManagerD.lib")
	#pragma comment (lib,"../../���ӿ�/MonitorServiceD.lib")
	#pragma comment (lib,"../../���ӿ�/LoggerServiceD.lib")	
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#if defined _M_IX86
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#elif defined _M_X64
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#else
		#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
	#endif
#endif

//////////////////////////////////////////////////////////////////////////