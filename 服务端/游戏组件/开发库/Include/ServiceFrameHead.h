#ifndef SERVICE_FRAME_HEAD_HEAD_FILE
#define SERVICE_FRAME_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//ƽ̨����
#include "Platform.h"
#include "RightDefine.h"

//��Ϣ����
#include "CMD_Commom.h"
#include "CMD_Validate.h"
#include "CMD_Correspond.h"
#include "CMD_GameServer.h"

//ƽ̨�ļ�
#include "CommonServiceHead.h"
#include "KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef SERVICE_FRAME_CLASS
	#ifdef SERVICE_FRAME_DLL
		#define SERVICE_FRAME_CLASS _declspec(dllexport)
	#else
		#define SERVICE_FRAME_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define SERVICE_FRAME_DLL_NAME	TEXT("ServiceFrame.dll")			//��� DLL ����
#else
	#define SERVICE_FRAME_DLL_NAME	TEXT("ServiceFrameD.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//����״̬
enum enServiceStatus
{
	ServiceStatus_Stop,				//ֹͣ״̬
	ServiceStatus_Config,			//����״̬
	ServiceStatus_Service,			//����״̬
	ServiceStatus_WillStop			//�����ر�
};

//////////////////////////////////////////////////////////////////////////

//״̬�ӿ�
interface IServiceUnitsSink
{
	//�ӿڶ���
public:
	//����״̬
	virtual VOID OnServiceUnitsStatus(enServiceStatus ServiceStatus)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//�����ļ�
#include "UIControlSink.h"
#include "RichEditTrace.h"
#include "ValidateService.h"
#include "ServiceParameter.h"

//////////////////////////////////////////////////////////////////////////

#endif