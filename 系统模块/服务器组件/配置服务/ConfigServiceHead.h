#ifndef CONFIG_SERVICE_HEAD_FILE
#define CONFIG_SERVICE_HEAD_FILE

/////////////////////////////////////////////////////////////////////////////////
//MFC�ļ�
#include <afxtempl.h>

//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"

//ƽ̨�ļ�
#include "..\..\�������\��������\CommonServiceHead.h"
#include "..\..\���������\�ں�����\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef CONFIG_SERVICE_CLASS
#ifdef CONFIG_SERVICE_DLL
#define CONFIG_SERVICE_CLASS _declspec(dllexport)
#else
#define CONFIG_SERVICE_CLASS _declspec(dllimport)
#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
#define CONFIG_SERVICE_DLL_NAME	TEXT("ConfigService.dll")		//�������
#else
#define CONFIG_SERVICE_DLL_NAME	TEXT("ConfigServiceD.dll")		//�������
#endif

//////////////////////////////////////////////////////////////////////////////////
//�ӿ�˵��


//////////////////////////////////////////////////////////////////////////////////

//�����ļ�
#ifndef CONFIG_SERVICE_DLL
	#include "GameGoodsManager.h"
	#include "GameLevelManager.h"
	#include "GlobalOptionManager.h"
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif