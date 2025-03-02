#ifndef PLAZA_CONTROL_HEAD_HEAD_FILE
#define PLAZA_CONTROL_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//ƽ̨�ļ�

//ƽ̨�ļ�
#include "..\..\ȫ�ֶ���\Platform.h"

//��ͷ�ļ�
#include "..\..\�ͻ������\����ؼ�\SkinControlsHead.h"
#include "..\..\�ͻ������\ƽ̨����\PlatformDataHead.h"
#include "..\..\�ͻ������\��Ϸ�ؼ�\ClientControlsHead.h"

//����ļ�
#include "..\..\�ͻ������\������Դ\SkinResourceHead.h"
#include "..\..\�ͻ������\�������\ClientServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//�����궨��

//��������
#ifndef PLAZA_CONTROL_CLASS
	#ifdef  PLAZA_CONTROL_DLL
		#define PLAZA_CONTROL_CLASS _declspec(dllexport)
	#else
		#define PLAZA_CONTROL_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define PLAZA_CONTROL_DLL_NAME	TEXT("PlazaControl.dll")			//��� DLL ����
#else
	#define PLAZA_CONTROL_DLL_NAME	TEXT("PlazaControlD.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IProcessManager INTERFACE_VERSION(1,1)
	static const GUID IID_IProcessManager={0x44958a10,0x6e7e,0x44ab,0xaa,0xb1,0x48,0xca,0x8a,0xc2,0xe4,0x04};
#else
	#define VER_IProcessManager INTERFACE_VERSION(1,1)
	static const GUID IID_IProcessManager={0xfbbd9807,0x60c9,0x4665,0xb8,0x8e,0xe1,0x10,0x72,0x0f,0xe0,0x82};
#endif

//���̹���
interface IProcessManager : public IUnknownEx
{
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IProcessManagerSink INTERFACE_VERSION(1,1)
	static const GUID IID_IProcessManagerSink={0xe2460d28,0xdf88,0x4572,0x96,0x13,0x54,0x6d,0x77,0xc6,0x5d,0xb0};
#else
	#define VER_IProcessManagerSink INTERFACE_VERSION(1,1)
	static const GUID IID_IProcessManagerSink={0xc503a1ee,0x2012,0x42f7,0xa1,0x5c,0x44,0xff,0x1c,0xa0,0x3f,0xdd};
#endif

//��Ϸ����
interface IProcessManagerSink : public IUnknownEx
{
	//���̴���
	virtual bool __cdecl OnGameProcessMistake()=NULL;
	//��������
	virtual bool __cdecl OnGameProcessCreate(bool bAlreadyExist)=NULL;
	//���̹ر�
	virtual bool __cdecl OnGameProcessConclude(DWORD dwExitCode)=NULL;
	//��������
	virtual bool __cdecl OnProcessData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//�����ļ�
#ifndef PLAZA_CONTROL_DLL
	#include "ServerListManager.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif