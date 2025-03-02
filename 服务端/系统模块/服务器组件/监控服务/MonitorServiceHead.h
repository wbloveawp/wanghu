#ifndef MONITOR_SERVICE_HEAD_FILE
#define MONITOR_SERVICE_HEAD_FILE

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
#ifndef MONITOR_SERVICE_CLASS
#ifdef MONITOR_SERVICE_DLL
#define MONITOR_SERVICE_CLASS _declspec(dllexport)
#else
#define MONITOR_SERVICE_CLASS _declspec(dllimport)
#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
#define MONITOR_SERVICE_DLL_NAME	TEXT("MonitorService.dll")		//�������
#else
#define MONITOR_SERVICE_DLL_NAME	TEXT("MonitorServiceD.dll")		//�������
#endif

//////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//״̬�������
struct tagStateVariationItem
{
	WORD								wStockID;					//����ʶ
	BYTE								cbStockState;				//���״̬
	LONGLONG							lStockControlID;			//���Ʊ��
};

//���������
struct tagStockVariationItem
{
	WORD								wStockID;					//����ʶ
	SCORE								lVariationScore;			//�������
	SCORE								lVariationProfit;			//�������
};

//��澯������
struct tagStockAlarmItem
{
	WORD								wStockID;					//����ʶ
	WORD								wKindID;					//���ͱ�ʶ
	SCORE								lBetAmount;					//��ע���
	SCORE								lStockScore;				//��ǰ���
};

//�ʳر������
struct tagJackpotVariationItem
{
	WORD								wStockID;					//����ʶ
	WORD								wLevelID;					//�����ʶ
	SCORE								lVariationScore;			//�������	
};

//�ʳؾ�������
struct tagJackpotAlarmItem
{
	WORD								wStockID;					//����ʶ
	WORD								wKindID;					//���ͱ�ʶ
	WORD								wLevelID;					//�����ʶ
	SCORE								lBetAmount;					//��ע���
	SCORE								lStockScore;				//��ǰ���
};

//////////////////////////////////////////////////////////////////////////////////

//�����ļ�
#ifndef MONITOR_SERVICE_DLL
	#include "ShareStockManager.h"
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif