#ifndef LOGGER_SERVICE_HEAD_FILE
#define LOGGER_SERVICE_HEAD_FILE

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
#ifndef LOGGER_SERVICE_CLASS
#ifdef LOGGER_SERVICE_DLL
#define LOGGER_SERVICE_CLASS _declspec(dllexport)
#else
#define LOGGER_SERVICE_CLASS _declspec(dllimport)
#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
#define LOGGER_SERVICE_DLL_NAME	TEXT("LoggerService.dll")		//�������
#else
#define LOGGER_SERVICE_DLL_NAME	TEXT("LoggerServiceD.dll")		//�������
#endif

//////////////////////////////////////////////////////////////////////////////////
//�ӿ�˵��


//////////////////////////////////////////////////////////////////////////////////

//�����ļ�
#ifndef LOGGER_SERVICE_DLL
	#include "Logger.h"
	#include "LoggerFile.h"
	#include "LoggerService.h"
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif