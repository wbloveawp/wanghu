#ifndef SERVICE_FRAME_HEAD_HEAD_FILE
#define SERVICE_FRAME_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"
#include "..\..\ȫ�ֶ���\RightDefine.h"

//��Ϣ����
#include "..\..\��Ϣ����\CMD_Commom.h"
#include "..\..\��Ϣ����\CMD_Validate.h"
#include "..\..\��Ϣ����\CMD_CenterServer.h"
#include "..\..\��Ϣ����\CMD_GameServer.h"
#include "..\..\��Ϣ����\IPC_MonitorServer.h"

//ƽ̨�ļ�
#include "..\..\�������\��������\CommonServiceHead.h"
#include "..\..\�������\����ͨѶ\ProcessTradeHead.h"
#include "..\..\���������\�ں�����\KernelEngineHead.h"

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

//״̬�ӿ�
interface IServiceUnitsSink
{
	//�ӿڶ���
public:
	//����״̬
	virtual VOID OnServiceUnitsStatus(WORD ServiceStatus)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//�����ļ�
#ifndef SERVICE_FRAME_DLL
#include "UIControlSink.h"
#include "MonitorClient.h"
#include "RichEditTrace.h"
#include "ServiceParameter.h"
#include "GlobalInfoItem.h"
#include "GlobalItemManager.h"
#endif
//////////////////////////////////////////////////////////////////////////

#endif