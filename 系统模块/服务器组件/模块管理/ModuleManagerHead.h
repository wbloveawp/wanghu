#ifndef MODULE_MANAGER_HEAD_HEAD_FILE
#define MODULE_MANAGER_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"
#include "..\..\ȫ�ֶ���\RightDefine.h"
#include "..\..\ȫ�ֶ���\ServerRule.h"

//ƽ̨�ļ�
#include "..\..\�������\��������\CommonServiceHead.h"
#include "..\..\���������\��Ϸ����\GameServiceHead.h"
#include "..\..\���������\�ں�����\KernelEngineHead.h"
#include "..\..\���������\������\ServiceFrameHead.h"
#include "..\..\���������\���÷���\ConfigServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef MODULE_MANAGER_CLASS
	#ifdef MODULE_MANAGER_DLL
		#define MODULE_MANAGER_CLASS _declspec(dllexport)
	#else
		#define MODULE_MANAGER_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define MODULE_MANAGER_DLL_NAME	TEXT("ModuleManager.dll")			//��� DLL ����
#else
	#define MODULE_MANAGER_DLL_NAME	TEXT("ModuleManagerD.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//ģ����Ϣ
struct tagGameModuleInfo
{
	//ģ������
	WORD							wModuleID;							//ģ���ʶ
	WORD							wDBListID;							//���ݿ�����
	WORD							wSupportType;						//֧������
	DWORD							dwModuleVersion;					//ģ��汾
	DWORD							dwNativeVersion;					//���ذ汾
	DWORD							dwClientVersion;					//�ͻ��˰汾

	//��Ϸ����
	TCHAR							szModuleName[LEN_KIND];				//��Ϸ����

	//�ļ�����
	TCHAR							szServerDLLName[LEN_PROCESS];		//��������
	TCHAR							szClientEXEName[LEN_PROCESS];		//��������
};

//////////////////////////////////////////////////////////////////////////

//������Ϣ
struct tagGameServerInfo
{
	//��������
	WORD							wServerID;							//�����ʶ
	WORD							wModuleID;							//ģ���ʶ

	//�ҽ�����
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wNodeID;							//�ڵ��ʶ
	WORD							wSortID;							//�����ʶ
	WORD							wVisibleMask;						//���ӹ���

	//��������
	LONG							lRevenue;							//˰�ձ���
	LONG							lExpGainRate;						//��������
	SCORE							lCellScore;							//��λ����
	SCORE							lLessScore;							//��ͻ���
	SCORE							lRestrictScore;						//���ƻ���
	SCORE							lMinEnterScore;						//��ͽ���
	SCORE							lMaxEnterScore;						//��߽���
	LONG							lMinEnterMember;					//��ͽ����Ա
	LONG							lMaxEnterMember;					//��߽����Ա

	//������Ϣ
	WORD							wMaxPlayer;							//�����Ŀ
	WORD							wTableCount;						//������Ŀ
	WORD							wServerType;						//��������
	WORD							wServerPort;						//����˿�
	WORD							wServerLevel;						//����ȼ�
	DWORD							dwServerRule;						//�������
	DWORD							dwAttachUserRight;					//����Ȩ��
	TCHAR							szServerName[LEN_SERVER];			//��������
	BYTE							cbMatchType;						//��������
	WORD							wDBListID;							//���ݿ�����

	//ģ������
	DWORD							dwModuleVersion;					//ģ��汾
	DWORD							dwNativeVersion;					//���ذ汾
	DWORD							dwClientVersion;					//�ͻ��˰汾
	TCHAR							szModuleName[LEN_KIND];				//��Ϸ����
	TCHAR							szServerDLLName[LEN_PROCESS];		//��������
	TCHAR							szClientEXEName[LEN_PROCESS];		//��������

	//��չ����
	BYTE							cbCustomRule[1024];					//�Զ�����
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//��������	

	//Լս����
	DWORD							dwBattleRule;						//Լս����

	//��������
	BYTE							cbDistributeMode;					//����ģʽ
	BYTE							cbDistributeRule;					//��������
	WORD							wMinPlayerCount;					//�������
	WORD							wMinDistributeUser;					//��������
	WORD							wDistributeTimeSpace;				//������	
	WORD							wMinPartakeGameUser;				//��������
	WORD							wMaxPartakeGameUser;				//�������

	//������Ϣ
	TCHAR							szServiceAddr[LEN_ADDRESS];			//�����ַ
	TCHAR							szServiceDomain[LEN_DOMAIN];		//��������
};

//////////////////////////////////////////////////////////////////////////

//������Ϣ
struct tagCreateServerInfo
{
	//��������
	WORD							wServerID;							//�����ʶ
	WORD							wModuleID;							//ģ���ʶ

	//�ҽ�����
	WORD							wKindID;							//���ͱ�ʶ
	WORD							wNodeID;							//�ڵ��ʶ
	WORD							wSortID;							//�����ʶ
	WORD							wVisibleMask;						//���ӹ���

	//��������
	LONG							lRevenue;							//˰�ձ���
	LONG							lExpGainRate;						//��������
	SCORE							lCellScore;							//��λ����
	SCORE							lLessScore;							//��ͻ���
	SCORE							lRestrictScore;						//���ƻ���
	SCORE							lMinEnterScore;						//��ͽ���
	SCORE							lMaxEnterScore;						//��߽���
	LONG							lMinEnterMember;					//��ͽ����Ա
	LONG							lMaxEnterMember;					//��߽����Ա	

	//������Ϣ
	WORD							wMaxPlayer;							//�����Ŀ
	WORD							wTableCount;						//������Ŀ
	WORD							wServerType;						//��������
	WORD							wServerPort;						//����˿�
	WORD							wServerLevel;						//����ȼ�
	DWORD							dwServerRule;						//�������
	DWORD							dwAttachUserRight;					//����Ȩ��
	TCHAR							szServerName[LEN_SERVER];			//��������
	BYTE							cbMatchType;						//��������
	WORD							wDBListID;							//���ݿ�����

	//��չ����
	BYTE							cbCustomRule[1024];					//�Զ�����
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//��������

	//Լս����
	DWORD							dwBattleRule;						//Լս����

	//��������
	BYTE							cbDistributeMode;					//����ģʽ
	BYTE							cbDistributeRule;					//��������
	WORD							wMinPlayerCount;					//�������
	WORD							wMinDistributeUser;					//��������
	WORD							wDistributeTimeSpace;				//������	
	WORD							wMinPartakeGameUser;				//��������
	WORD							wMaxPartakeGameUser;				//�������

	//������Ϣ
	TCHAR							szServiceAddr[LEN_ADDRESS];			//�����ַ
	TCHAR							szServiceDomain[LEN_DOMAIN];		//��������
};

//������Ϣ
struct tagCreateMatchInfo
{
	//��ʶ��Ϣ
	WORD							wKindID;							//���ͱ�ʶ
	DWORD							dwClubID;							//�����ʶ
	DWORD							dwMatchID;							//������ʶ

	//������Ϣ	
	BYTE							cbShowType;							//չʾ����	
	BYTE							cbMatchType;						//��������
	TCHAR							szMatchName[32];					//��������
	TCHAR							szMatchDate[64];					//����ʱ��
	TCHAR							szModuleName[LEN_KIND];				//ģ������

	//������Ϣ
	SCORE							lFeeAmount;							//���ý��
	WORD							wFeeMoneyID;						//���ұ�ʶ
	WORD							wUseTicketCount;					//��ȯ����
	WORD							wBuyTicketCount;					//��ȯ����
	WORD							wBuyTicketOption;					//��Ʊѡ��
	SCORE							lBuyTicketAmount;					//���ý��
	WORD							wBuyTicketMoneyID;					//���ұ�ʶ
	WORD							wSignupCount;						//��������	
	BYTE							cbSignupMode;						//������ʽ
	BYTE							cbJoinCondition;					//��������
	BYTE							cbMemberOrder;						//��Ա�ȼ�
	LONG							lExperience;						//��Ҿ���

	//��������
	BYTE							cbMatchDistribute[64];				//��������
	
	//��������
	WORD							wRewardCount;						//������Ŀ
	tagMatchRewardItem				MatchRewardItem[16];				//������Ϣ

	//�����ִ�
	WORD							wRoundCount;						//�ִ���Ŀ
	tagMatchRoundItem				MatchRoundItem[5];					//�ִ���Ϣ

	//��չ����
	BYTE							cbMatchRule[256];					//��������	
	TCHAR							szServiceMachine[LEN_MACHINE_ID];	//��������
};


//////////////////////////////////////////////////////////////////////////

//���ò���
struct tagModuleInitParameter
{
	tagGameServiceAttrib			GameServiceAttrib;					//��������
	tagGameServiceOption			GameServiceOption;					//��������
};

//////////////////////////////////////////////////////////////////////////

//���ݿ���Ϣ
struct tagDataBaseList
{
	WORD							wDBListID;							//�����б�����
	WORD							wDBInfoID;							//���ݿ���Ϣ����
	DWORD							dwDBGenre;							//���ݿ�����
	TCHAR							szDBName[32];						//���ݿ�����
	TCHAR							szDBAliasName[32];					//���ݿ����
};

//////////////////////////////////////////////////////////////////////////

//�����ļ�
#ifndef MODULE_MANAGER_DLL 
	#include "ListControl.h"	
	#include "DlgServerWizard.h"
	#include "DlgServerMatch.h"
	#include "DlgMatchWizard.h"
	#include "DlgServiceOption.h"
	#include "ModuleListControl.h"
	#include "ServerListControl.h"
	#include "ServerInfoManager.h"
	#include "MatchInfoManager.h"
	#include "MatchListControl.h"
	#include "ModuleDBParameter.h"
	#include "ModuleInfoManager.h"	
	#include "DataBaseListManager.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif