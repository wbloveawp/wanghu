#ifndef COMMON_SERVICE_HEAD_HEAD_FILE
#define COMMON_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//�����ļ�

//MFC �ļ�
#include <Nb30.h>
#include <Afxmt.h>
#include <AtlBase.h>
#include <wininet.h>

//ƽ̨�ļ�
#include "Platform.h"           

//////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef COMMON_SERVICE_CLASS
	#ifdef COMMON_SERVICE_DLL
		#define COMMON_SERVICE_CLASS _declspec(dllexport)
	#else
		#define COMMON_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define COMMON_SERVICE_DLL_NAME	TEXT("CommonService.dll")			//�������
#else
	#define COMMON_SERVICE_DLL_NAME	TEXT("CommonServiceD.dll")			//�������
#endif

//////////////////////////////////////////////////////////////////////////
//�����ļ�

#ifndef COMMON_SERVICE_DLL
	#include "WHJwt.h"
	#include "WHJson.h"
	#include "WHSha1.h"
	#include "WHBase64.h"
	#include "WHService.h"
	#include "WHConHash.h"
	#include "WHDataQueue.h"
	#include "WordFilter.h"
	#include "RegKeyAide.h"
	#include "ThreadLock.h"
	#include "Snowflake.h"
	#include "DataEncrypt.h"	
	#include "WininetHttp.h"
	#include "CrashCapture.h"
	#include "ServiceThread.h"	
	#include "WHCustomFile.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif