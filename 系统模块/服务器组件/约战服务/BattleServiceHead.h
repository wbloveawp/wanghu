#ifndef BATTLE_SERVICE_HEAD_HEAD_FILE
#define BATTLE_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//MFC�ļ�
#include <afxtempl.h>

//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"

//��Ϣ����
#include "..\..\��Ϣ����\CMD_Commom.h"
#include "..\..\��Ϣ����\CMD_CenterServer.h"
#include "..\..\��Ϣ����\CMD_BattleServer.h"
#include "..\..\��Ϣ����\CMD_GameServer.h"

//ƽ̨�ļ�
#include "..\..\�������\��������\CommonServiceHead.h"
#include "..\..\���������\��Ϸ����\GameServiceHead.h"
#include "..\..\���������\�ں�����\KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef BATTLE_SERVICE_CLASS
	#ifdef  BATTLE_SERVICE_DLL
		#define BATTLE_SERVICE_CLASS _declspec(dllexport)
	#else
		#define BATTLE_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define BATTLE_SERVICE_DLL_NAME	TEXT("BattleService.dll")			//�������
#else
	#define BATTLE_SERVICE_DLL_NAME	TEXT("BattleServiceD.dll")			//�������
#endif

//////////////////////////////////////////////////////////////////////////////////

#ifndef BATTLE_SERVICE_DLL

	#define BATTLE_MODULE_HELPER
	
	DECLARE_MODULE_HELPER(BattleServiceManager, ExtendServiceManager, BATTLE_SERVICE_DLL_NAME,"CreateBattleServiceManager")

#endif

//////////////////////////////////////////////////////////////////////////////////

#endif