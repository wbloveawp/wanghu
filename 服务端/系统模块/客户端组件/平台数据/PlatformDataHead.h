#ifndef PLATFORM_DATA_HEAD_HEAD_FILE
#define PLATFORM_DATA_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////
//ƽ̨�ļ�

//ƽ̨�ļ�
#include "..\..\ȫ�ֶ���\Platform.h"
#include "..\..\ȫ�ֶ���\BrowserDef.h"

//��ͷ�ļ�
#include "..\..\�ͻ������\ͼ������\ImageAideHead.h"
#include "..\..\�ͻ������\������Դ\SkinResourceHead.h"
#include "..\..\�ͻ������\�������\NetworkServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef PLATFORM_DATA_CLASS
	#ifdef  PLATFORM_DATA_DLL
		#define PLATFORM_DATA_CLASS _declspec(dllexport)
	#else
		#define PLATFORM_DATA_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define PLATFORM_DATA_DLL_NAME	TEXT("PlatformData.dll")			//��� DLL ����
#else
	#define PLATFORM_DATA_DLL_NAME	TEXT("PlatformDataD.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////
//���ӵ�ַ

//��Ϊ��Ա
const TCHAR szApplyMember[]=TEXT("http://www.abcdefgh.com/pay/Member.aspx");

//��������
const TCHAR szFirendCenter[]=TEXT("http://friend.abcdefgh.com");

//�������
const TCHAR szAreaInfoNote[]=TEXT("http://services.abcdefgh.com/GameHelp/AreaInfoNote.aspx");

//////////////////////////////////////////////////////////////////////////
//�����ļ�

#ifndef PLATFORM_DATA_DLL
	#include "GlobalServer.h"
	#include "GlobalStation.h"
	#include "GlobalUserInfo.h"
	#include "OptionParameter.h"
	#include "GlobalService.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif