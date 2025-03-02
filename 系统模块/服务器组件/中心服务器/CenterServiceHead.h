#ifndef CENTER_SERVICE_HEAD_HEAD_FILE
#define CENTER_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"
#include "..\..\ȫ�ֶ���\RightDefine.h"

//��Ϣ����
#include "..\..\��Ϣ����\CMD_Commom.h"
#include "..\..\��Ϣ����\CMD_CenterServer.h"

//ƽ̨�ļ�
#include "..\..\�������\��������\CommonServiceHead.h"
#include "..\..\���������\������\ServiceFrameHead.h"
#include "..\..\���������\�ں�����\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////
//��������

//ģ�鶨��
#ifndef _DEBUG
	#define CENTER_SERVICE_DLL_NAME	TEXT("CenterService.dll")	//��� DLL ����
#else
	#define CENTER_SERVICE_DLL_NAME	TEXT("CenterServiceD.dll")	//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ICenterServiceUnits INTERFACE_VERSION(1,1)
	static const GUID IID_ICenterServiceUnits={0x807845d4,0x5a9c,0x4965,0xaf,0x99,0xd9,0x48,0xee,0x5f,0x7b,0x4c};
#else
	#define VER_ICenterServiceUnits INTERFACE_VERSION(1,1)
	static const GUID IID_ICenterServiceUnits={0x9fb08811,0xf9da,0x4f20,0xab,0xb0,0x27,0xfe,0x78,0x7c,0x0e,0xf1};
#endif

//����Ԫ
interface ICenterServiceUnits : public IUnknownEx
{
	//��Ϣ����
public:
	//��ȡ״̬
	virtual WORD GetServiceStatus()=NULL;

	//�������
public:
	//��������
	virtual bool StartService()=NULL;
	//ֹͣ����
	virtual bool ConcludeService()=NULL;
	//���ýӿ�
	virtual bool SetServiceUnitsSink(IServiceUnitsSink * pIServiceUnitsSink)=NULL;
};

//////////////////////////////////////////////////////////////////////////

//����Ԫ
DECLARE_MODULE_HELPER(CenterServiceUnits, CenterServiceUnits, CENTER_SERVICE_DLL_NAME,"CreateServiceUnits")

//////////////////////////////////////////////////////////////////////////

#endif