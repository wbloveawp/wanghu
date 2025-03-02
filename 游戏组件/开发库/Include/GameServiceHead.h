#ifndef GAME_SERVICE_HEAD_HEAD_FILE
#define GAME_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//ƽ̨����
#include "Platform.h"
#include "BattleRule.h"
#include "RightDefine.h"
#include "BrowserDef.h"
#include "ServerRule.h"

//��Ϣ����
#include "CMD_Commom.h"
#include "CMD_Validate.h"
#include "CMD_CenterServer.h"
#include "CMD_GameServer.h"
#include "CMD_StoreServer.h"
#include "CMD_MessageServer.h"

//ƽ̨�ļ�
#include "CommonServiceHead.h"
#include "ServiceFrameHead.h"
#include "KernelEngineHead.h"
#include "MonitorServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef GAME_SERVICE_CLASS
#ifdef  GAME_SERVICE_DLL
#define GAME_SERVICE_CLASS _declspec(dllexport)
#else
#define GAME_SERVICE_CLASS _declspec(dllimport)
#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define GAME_SERVICE_DLL_NAME	TEXT("GameService.dll")				//��� DLL ����
#else
	#define GAME_SERVICE_DLL_NAME	TEXT("GameServiceD.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////
//����˵��

//�û�����
class CServerUserPack;

//////////////////////////////////////////////////////////////////////////////////
//�û��ӿ�

//�û��ӿ�
interface IServerUserItem;
interface IServerUserManager;
interface IServerUserItemSink;

//�����˽ӿ�
interface IRobotUserItem;
interface IRobotUserItemSink;
interface IRobotUserManager;

//���ӽӿ�
interface ITableFrameItem;
interface ITableFrameSink;
interface ITableUserAction;
interface ITableEventSink;

//¼��ӿ�
interface IGameVideoItem;
interface IGameVideoManager;

//�����ӿ�
interface IMatchEventSink;

//����ӿ�
interface IServiceUnits;
interface IServiceFunction;
interface IGameServiceManager;
interface IGameServiceCustomRule;
interface IGameServiceCustomTime;
interface IGameServiceDynamicRule;

//�����ӿ�
interface IGameBalanceControl;

//////////////////////////////////////////////////////////////////////////
//��������

//��������
#define ROBOT_WAITSTANDUP			0x01								//�ȴ�����
#define ROBOT_WAITLEAVE				0x02								//�ȴ��뿪	

//��Ϸ��Ϊ
#define ROBOT_BEBANKER				0x0100								//������ׯ	
#define ROBOT_OCCUPYSEAT			0x0200								//����ռλ

//����ԭ��
#define GER_NORMAL					0x00								//�������
#define GER_DISMISS					0x01								//��Ϸ��ɢ
#define GER_USER_LEAVE				0x02								//�û��뿪
#define GER_NETWORK_ERROR			0x03								//�������

//����ԭ��
#define BFR_NORMAL					0									//�������
#define BFR_SYSTEM					1									//ϵͳԭ��
#define BFR_OFFLINE_EXPIRE			2									//���߳�ʱ
#define BFR_MANAGER_DISMISS			3									//����Ա��ɢ
#define BFR_AGREEMENT_DISMISS		4									//Э���ɢ
#define BFR_DEDUCT_COST_FAILED		5									//�۷�ʧ��

//ͬ��Ŀ��
#define SYNC_TARGET_CLIENT			0x01								//ͬ���ͻ���
#define SYNC_TARGET_DATABASE		0x02								//ͬ�����ݿ�
#define SYNC_TARGET_GAMESERVER		0x04								//ͬ����Ϸ

//Ⱥ������
#define BG_MOBILE					(BYTE)(0x01)						//�ֻ�Ⱥ��
#define BG_COMPUTER					(BYTE)(0x02)						//����Ⱥ��
#define BG_ALL_CLIENT				(BYTE)(0xFF)						//ȫ��Ⱥ��

//��������
#define INDEX_SOCKET				(WORD)(0x0000)						//��������
#define INDEX_ROBOT					(WORD)(0x2000)						//��������
#define INVALID_WORD_INDEX			(WORD)(0xFFFF)						//��Ч����

#define MAX_MATCH_COUNT				(1024/(sizeof(ULARGE_INTEGER)*2))	//��������

//��������
#define GAME_SERVICE_CREATE_NAME	"CreateGameServiceManager"			//��������

//////////////////////////////////////////////////////////////////////////
//ʱ���ʶ

//����ʱ��
#define IDI_ATTEMPER_SINK_START		1									//��ʼ��ʶ
#define IDI_ATTEMPER_SINK_RANGE		99									//��ʶ��Χ

//���ݿ�����
#define IDI_DATA_BASE_SINK_START	100									//��ʼ��ʶ
#define IDI_DATA_BASE_SINK_RANGE	199									//��ʶ��Χ

//����ʱ��
#define IDI_ROBOT_MANAGER_START		200									//��ʼ��ʶ
#define IDI_ROBOT_MANAGER_RANGE		299									//��ʶ��Χ

//��չ��Χ
#define IDI_EXTEND_MODULE_START		300									//��ʼ��ʶ
#define IDI_EXTEND_MODULE_RANGE		5999								//������ʶ

//����ʱ��
#define IDI_TABLE_FRAME_START		20000								//��ʼ��ʶ
#define IDI_TABLE_FRAME_RANGE		50000								//��ʶ��Χ

//////////////////////////////////////////////////////////////////////////
//ʱ���ʶ

//����ʱ��
#define TIME_TABLE_FRAME_RANGE		100									//��ʶ��Χ

//��Ϸʱ��
#define TIME_TABLE_SINK_RANGE		50									//��ʶ��Χ
#define TIME_TABLE_KERNEL_RANGE		30									//��ʶ��Χ
#define TIME_TABLE_EXTEND_RANGE		20									//��ʶ��Χ

//����ʱ��
#define TIME_MATCH_FRAME_RANGE		160									//��ʶ��Χ
#define TIME_MATCH_GROUP_RANGE		10									//��ʶ��Χ

//����ʱ��
#define IDI_ROBOT_USER_TIEM			1									//��ʼ��ʶ
#define IDI_ROBOT_ITEM_SINK			1									//��ʼ��ʶ

//////////////////////////////////////////////////////////////////////////
//���ݱ�ʶ
#define DB_BASICS_REQUEST_START		0									//��ʼ��ʶ
#define DB_BASICS_RESPOND_START		1000								//��ʼ��ʶ
#define DB_EXTEND_REQUEST_START		2000								//��ʼ��ʶ
#define DB_EXTEND_RESPOND_START		3000								//��ʼ��ʶ

//////////////////////////////////////////////////////////////////////////
//ʱ�䳣��

//ϵͳʱ��
#define TIME_AMORTIZE_DISPOSE		30L									//����ʱ��
#define TIME_CONNECT_STORE			3L									//����ʱ��
#define TIME_CONNECT_SERVICE		3L									//����ʱ��
#define TIME_CONNECT_MONITOR		5L									//����ʱ��			
#define TIME_CONNECT_VALIDATE		5L									//����ʱ��
#define TIME_CONNECT_CENTER			5L									//����ʱ��
#define TIME_REPORT_SERVER_INFO		30L									//�ϱ�ʱ��

//����ʱ��
#ifdef _DEBUG

#define TIME_LOAD_ROBOT_INFO		15L									//�����û�
#define TIME_DISTRIBUTE_ROBOT		15L									//�����û�
#define TIME_LOAD_SYSTEM_MESSAGE	900L								//ϵͳ��Ϣ
#define TIME_SEND_SYSTEM_MESSAGE	480L								//ϵͳ��Ϣ
#define TIME_WAIT_USER_SCORE		30L									//д�ֵȴ�

#else

#define TIME_LOAD_ROBOT_INFO		15L									//�����û�
#define TIME_DISTRIBUTE_ROBOT		15L									//�����û�
#define TIME_LOAD_SYSTEM_MESSAGE	900L								//ϵͳ��Ϣ
#define TIME_SEND_SYSTEM_MESSAGE	480L								//ϵͳ��Ϣ
#define TIME_WAIT_USER_SCORE		30L									//д�ֵȴ�

#endif

//�뿪ʱ��
#define TIME_ROBOT_LEAVE_MIN		2									//�뿪ʱ��
#define TIME_ROBOT_LEAVE_MAX		10									//�뿪ʱ��

//����ʱ��
#define TIME_ROBOT_STANDUP_MIN		2									//����ʱ��
#define TIME_ROBOT_STANDUP_MAX		5									//����ʱ��

//ȡ��ʱ��
#define TIME_ROBOT_INSURE_MIN		10									//ȡ��ʱ��
#define TIME_ROBOT_INSURE_MAX		20									//ȡ��ʱ��

//��ʼ��Ϸ
#define TIME_ROBOT_STARTGAME_MIN	3									//��ʼʱ��
#define TIME_ROBOT_STARTGAME_MAX	6									//��ʼʱ��

//����ģʽ
#define DISTRIBUTE_MODE_IMMEDIATELY 0									//����ģʽ
#define DISTRIBUTE_MODE_DELAYWAIT	1									//��ʱģʽ

//����ѡ��
#define DISTRIBUTE_SAME_TABLE		0x01								//ͬ��ѡ��
#define DISTRIBUTE_SAME_ADDRESS		0x02								//��ַѡ��
#define DISTRIBUTE_SAME_MACHINE		0x04								//����ѡ��
#define DISTRIBUTE_PLAYER_FIRST		0x08								//�������

//��������
#define ROBOT_ACTION_LEAVE			1									//�뿪����
#define ROBOT_ACTION_STANDUP		2									//��������
#define ROBOT_ACTION_INSURE			3									//����ȡ��
#define ROBOT_ACTION_DISMISS		4									//ͶƱ��ɢ
#define ROBOT_ACTION_STARTGAME		5									//��ʼ��Ϸ

//////////////////////////////////////////////////////////////////////////
//ö�ٶ���

//��������
enum enScoreType
{
	ScoreType_Null,					//��Ч
	ScoreType_Win,					//ʤ��
	ScoreType_Lose,					//���
	ScoreType_Draw,					//�;�
	ScoreType_Flee,					//�Ӿ�
	ScoreType_Present,				//����
	ScoreType_Service				//�����
};

//��ʼģʽ
enum enStartMode
{
	StartMode_AllReady,				//����׼��
	StartMode_Symmetry,				//�Գƿ�ʼ
	StartMode_FullReady,			//���˿�ʼ
	StartMode_TimeControl,			//ʱ�����
	StartMode_MasterControl,		//�������
};

//////////////////////////////////////////////////////////////////////////
//�ṹ����

//��������
struct tagGameServiceAttrib
{
	//�ں�����
	WORD							wKindID;							//���ƺ���
	WORD							wChairCount;						//������Ŀ
	WORD							wSupporType;						//֧������
	BYTE							cbDynamicJoin;						//��̬����
	BYTE							cbSegmentScore;						//��λ����
	TCHAR							szGameName[LEN_KIND];				//��Ϸ����

	//��������
	DWORD							dwServerVersion;					//��Ϸ�汾
	DWORD							dwClientVersion;					//��Ϸ�汾
	TCHAR							szDataBaseName[32];					//��Ϸ����
	TCHAR							szServerDLLName[LEN_PROCESS];		//��������
	TCHAR							szClientEXEName[LEN_PROCESS];		//��������
};

//��������
struct tagGameServiceOption
{
	//�б�����
	WORD							wKindID;							//�ҽ�����
	WORD							wNodeID;							//�ҽӽڵ�
	WORD							wSortID;							//���б�ʶ
	WORD							wVisibleMask;						//��������

	//��������
	WORD							wServerID;							//�������
	WORD							wTableCount;						//������Ŀ
	WORD							wServerType;						//��������
	WORD							wServerPort;						//����˿�
	WORD							wServerLevel;						//����ȼ�
	TCHAR							szServerName[LEN_SERVER];			//��������
	BYTE							cbMatchType;						//��������
	WORD							wDBListID;							//���ݿ�����	

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

	//��������
	SCORE							lMaxLockScore;						//�������
	SCORE							lLessLockScore;						//��������

	//��������
	WORD							wMaxPlayer;							//�����Ŀ
	DWORD							dwServerRule;						//�������
	DWORD							dwAttachUserRight;					//����Ȩ��

	//��չ����
	BYTE							cbCustomRule[1024];					//�Զ�����
	WORD							wLessStartPlayer;					//�������� 	

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

//�������
struct tagExtendServiceParameter
{
	//���ò���
	tagGameServiceOption*			pGameServiceOption;					//��������
	tagGameServiceAttrib*			pGameServiceAttrib;					//��������

	//�ں����
	ITimerEngine*					pITimerEngine;						//ʱ������
	IDataBaseEngine*				pIDataBaseEngine;					//��������	
	ITCPNetworkEngine*				pTCPNetworkEngine;					//��������
	ITCPSocketService*				pITCPSocketService;					//��������
	ITCPSocketService*				pITCPSocketCenterServer;			//���ķ���	

	//�������
	IRobotUserManager*				pIRobotUserManager;					//��������	
	IServiceFunction*				pIMainServiceFrame;					//������
	IServerUserManager*				pIServerUserManager;				//�û�����	
};

//���Ӳ���
struct tagTableFrameParameter
{
	//�ں����
	ITimerEngine *					pITimerEngine;						//ʱ������
	ITCPSocketService *				pITCPSocketService;					//�������
	IDataBaseEngine *				pIAssistDataBaseEngine;				//��������	
	IDataBaseEngine*				pIDataBaseEngineEngine;				//��������	

	//�������
	IServiceFunction *				pIServiceFunction;					//������
	IGameVideoManager *				pIGameVideoManager;					//¼�����
	IGameServiceManager *			pIGameServiceManager;				//�������
	IRobotUserManager *				pIRobotUserManager;					//��������	

	//�ص����
	ITableEventSink *				pITableEventSink;					//�����¼�

	//���ñ���
	tagGameServiceAttrib *			pGameServiceAttrib;					//��������
	tagGameServiceOption *			pGameServiceOption;					//���ò���
};

//��չ���Ȳ���
struct tagExtendDBSinkParameter
{
	//��Ϸ���ݿ�
	CDataBaseAide *					pGameDBAide;						//��Ϸ���ݿ�
	IDataBase*						pIGameDBModule;						//��Ϸ���ݿ�

	//�˺����ݿ�
	CDataBaseAide *					pAccountsDBAide;					//�˺����ݿ�
	IDataBase*						pIAccountsDBModule;					//�˺����ݿ�

	//ƽ̨���ݿ�
	CDataBaseAide *					pPlatformDBAide;					//��Ϸ���ݿ�
	IDataBase*						pIPlatformDBModule;					//��Ϸ���ݿ�

	//���ñ���
	tagGameServiceAttrib*			pGameServiceAttrib;					//��������
	tagGameServiceOption*			pGameServiceOption;					//���ò���

	//�������
	ITimerEngine*					pITimerEngine;						//��ʱ����
	IDataBaseEngine*				pIDataBaseEngine;					//��������
	IDataBaseEngineEvent*			pIDataBaseEngineEvent;				//�����¼�
};

//���Ʋ���
struct tagControlParameter
{
	bool							bControlEnabled;					//���ƿ���
	LONG							lBenchmarkValue;					//��׼��ֵ	
	WORD							wWLControlRate;						//����ʤ��
	WORD							wBLControlRate;						//����ʤ��
	WORD							wInWhiteListRate;					//����ʤ��
	WORD							wInBlackListRate;					//����ʤ��	
	WORD							wOutWhiteListRate;					//�˳�ʤ��
	WORD							wOutBlackListRate;					//�˳�ʤ��
	WORD							wStartPlayCount;					//�������
	WORD							wSupportControlMode;				//����ģʽ
};

//��������
struct tagUserControlItem
{
	BYTE							cbControlMode;						//����ģʽ
	BYTE							cbControlKind;						//��������
	BYTE							cbControlStatus;					//����״̬
	WORD							wTargetWinRate;						//Ŀ��ʤ��
	WORD							wControlWinRate;					//����ʤ��	
	LONG							lTotalWinCount;						//�ϼ�Ӯ��
	LONG							lTotalLoseCount;					//�ϼ����
	SCORE							lTotalWinScore;						//�ϼ�Ӯ��
	SCORE							lTotalLoseScore;					//�ϼ����
	WORD							wControlPlayCount;					//��Ϸ����
	DWORD							dwControlDuration;					//����ʱ��	
	DWORD							dwControlStartTime;					//��ʼʱ��	
};

//�Ƹ����
struct tagWealthVariation
{
	WORD							wReason;							//���ԭ��
	WORD							wMoneyID;							//���ұ�ʶ
	SCORE							lBefore;							//�������
	SCORE							lBalance;							//�������
	BYTE							cbSyncTarget;						//ͬ��Ŀ��
	SCORE							lVariationScore;					//�������	
	SYSTEMTIME						VariationTime;						//���ʱ��		
};

//��Ʒ���
struct tagGoodsVariation
{
	WORD							wReason;							//���ԭ��
	WORD							wGoodsID;							//��Ʒ��ʶ
	WORD							wGoodsKind;							//��Ʒ����
	DWORD							dwBeforeCount;						//��Ʒ����	
	LONG							lVariationCount;					//�������
	SYSTEMTIME						VariationTime;						//���ʱ��	
};

//////////////////////////////////////////////////////////////////////////
//�����ṹ

//������Ϣ
struct tagScoreInfo
{
	SCORE							lScore;								//��Ϸ����
	SCORE							lRevenue;							//��Ϸ˰��
	LONG							lExperience;						//��Ϸ����
	enScoreType						ScoreType;							//��������		
	SCORE							lBetAmount;							//��ע����
};

//������Ϣ
struct tagLockScoreInfo
{
	SCORE							lMaxLockScore;						//�������
	SCORE							lLessLockScore;						//��������
	SCORE							lRestrictScore;						//���ƻ���
};

//������Ϣ
struct tagVariationScore
{
	SCORE							lRevenue;							//��Ϸ˰��	
	SCORE							lUnLockScore;						//�ͷŻ���
	DWORD							dwPlayTimeCount;					//��Ϸʱ��
	tagServerUserScore				VariationScore;						//�������
};

//��������
struct tagScoreRecordItem
{	
	DWORD							dwUserID;							//�û���ʶ
	WORD							wChairID;							//�û���λ	
	SCORE							lScore;								//�û�����
	SCORE							lRevenue;							//�û�˰��
	SCORE							lBetAmount;							//��ע���
	SCORE							lBeforScore;						//�û�����
};

//�û�����
struct tagServerUserRule
{
	//�����־
	bool							bLimitSameIP;						//Ч���ַ
	bool							bLimitWinRate;						//����ʤ��
	bool							bLimitFleeRate;						//���ƶ���
	bool							bLimitGameScore;					//���Ʒ���

	//��������
	WORD							wMaxFleeRate;						//�������
	WORD							wLessWinRate;						//���ʤ��
	LONG							lMaxGameScore;						//��߷��� 
	LONG							lLessGameScore;						//��ͷ���
	TCHAR							szPassword[LEN_PASSWORD];			//��������
};

//�û���Ϣ
struct tagUserInfoPlus
{
	//���Ա���
	WORD							wBindIndex;							//������
	DWORD							dwTokenID;							//��������
	DWORD							dwClientIP;							//���ӵ�ַ

	//������Ϣ
	SCORE							lFreeScore;							//���ɻ���
	LONG							lLockServiceScore;					//���������
	SCORE							lFreezeScore;						//�������
	SCORE							lRestrictScore;						//���ƻ���

	//��������
	bool							bRobotUser;							//�����û�
	bool							bMobileUser;						//�ֻ��û�	

	//Ȩ�ޱ���
	DWORD							dwUserRight;						//�û�Ȩ��
	DWORD							dwMasterRight;						//����Ȩ��

	//������
	TCHAR							szMachineID[LEN_MACHINE_ID];		//��������

	//������Ϣ
	WORD							wMacthGameCount;					//��������

	//��������
	DWORD							dwInoutIndex;						//������ʶ

	//λ����Ϣ
	double							dGlobalPosLat;						//��λ����
	double							dGlobalPosLng;						//��λγ��

	//״̬��Ϣ
	BYTE							cbShutUpStatus;						//����״̬
	DWORD							dwShutUpOverTime;					//���ʱ��

	//�ڰ�����
	WORD							wAddupWinRate;						//�ۼ�ʤ��
	SCORE							lAddupWinScore;						//�ۼ�Ӯ��
	SCORE							lAddupLoseScore;					//�ۼ����
	tagUserControlItem				UserControlItem;					//��������
};

//���߻���
struct tagOfflineScore
{
	DWORD							dwUserID;							//���ID
	DWORD							dwUserRight;						//���Ȩ��
	DWORD							dwClientIP;							//���IP
	DWORD							dwPlayTimeCount;					//��Ϸʱ��

	SCORE							lScore;								//��Ϸ����
	SCORE							lWinScore;							//ʤ������
	enScoreType						ScoreType;							//��������
	SCORE							lRevenue;							//��Ϸ˰��
	SCORE							lUnLockScore;						//��������
	SCORE							lBetAmount;							//��ע���
	LONG							lWinCount;							//ʤ����Ŀ
	LONG							lLostCount;							//ʧ����Ŀ
	LONG							lDrawCount;							//ƽ����Ŀ
	LONG							lFleeCount;							//������Ŀ
	LONG							lExperience;						//�û�����
};

//����״��
struct tagTableUserInfo
{
	WORD							wMinUserCount;						//��������	
	WORD							wTableUserCount;					//�û���Ŀ
	WORD							wTableReadyCount;					//׼����Ŀ
	WORD							wClientReadyCount;					//׼����Ŀ
	WORD							wTableRobotCount;					//������Ŀ	
};

//����״��
struct tagRobotUserInfo
{
	WORD							wFreeUserCount;						//�û���Ŀ
	WORD							wPlayUserCount;						//�û���Ŀ
	WORD							wSitdownUserCount;					//�û���Ŀ
	IRobotUserItem *				pIRobotUserFree[MAX_ROBOT];		//�����ӿ�
	IRobotUserItem *				pIRobotUserPlay[MAX_ROBOT];		//�����ӿ�
	IRobotUserItem *				pIRobotUserSitdown[MAX_ROBOT];	//�����ӿ�
};

//������Ϣ
struct tagRobotService
{
	WORD							wInsureDrawCount;					//ȡ�����
	DWORD							dwSwitchTableInnings;				//��Ϸ����
};

//�ʺ���Ϣ
struct tagRobotAccountsInfo
{
	DWORD							dwUserID;							//������ʶ
	TCHAR							szPassword[LEN_PASSWORD];			//��������
};

//��������
struct tagRobotItemConfig
{
	tagRobotAccountsInfo			RobotAccountsInfo;					//�ʺ���Ϣ		
	tagRobotParameter *				pRobotParameter;					//��������
};

//��������
struct tagRobotAction
{
	WORD							wActionCode;						//��������
	DWORD							dwUserID;							//������ʶ
	DWORD							dwDelayTime;						//�ӳ�ʱ��
	DWORD							dwGenerateTime;						//����ʱ��
	BYTE							cbActionData[16];					//��������
};

//������
struct tagCardDispatcher
{
	//��������
	BYTE							cbControlGameCount;					//���ƾ���
	BYTE							cbMagicIndex;						//��������
	BYTE							cbMagicOpenPos;						//����λ��
	BYTE							cbMagicOpenCard;					//������
	BYTE							cbCardCount;						//������Ŀ
	BYTE							cbCardData[MAX_CARD_REPERTORY];		//��������
};

//////////////////////////////////////////////////////////////////////////
//���Ͷ���
typedef CWHArray<WORD, WORD> COptionValueArray;

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameServiceManager INTERFACE_VERSION(1,1)
	static const GUID IID_IGameServiceManager={0xffc1663d,0x26a8,0x488e,0x009b,0x43,0x31,0x33,0xef,0xb4,0x64,0xf2};
#else
	#define VER_IGameServiceManager INTERFACE_VERSION(1,1)
	static const GUID IID_IGameServiceManager={0xbf32a738,0x1380,0x4182,0x008b,0x7f,0xb8,0x97,0x7d,0xf3,0x26,0xd5};
#endif

//��Ϸ�ӿ�
interface IGameServiceManager : public IUnknownEx
{
	//�����ӿ�
public:
	//�������
	virtual bool GetGameServiceAttrib(tagGameServiceAttrib & GameServiceAttrib)=NULL;
	//��������
	virtual bool RectifyOptionParameter(tagGameServiceOption & GameServiceOption)=NULL;

	//�����ӿ�
public:
	//��������
	virtual VOID * CreateTableFrameSink(REFGUID Guid, DWORD dwQueryVer)=NULL;
	//��������
	virtual VOID * CreateRobotUserItemSink(REFGUID Guid, DWORD dwQueryVer)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameServiceCustomRule INTERFACE_VERSION(1,1)
	static const GUID IID_IGameServiceCustomRule={0x76b5f361,0x2989,0x40da,0x00b6,0x34,0x2e,0xca,0x20,0x62,0xa2,0x2a};
#else
	#define VER_IGameServiceCustomRule INTERFACE_VERSION(1,1)
	static const GUID IID_IGameServiceCustomRule={0x639781cc,0x799f,0x4f99,0x008e,0x48,0x46,0xfa,0x58,0xde,0x9d,0xa6};
#endif

//���ýӿ�
interface IGameServiceCustomRule : public IUnknownEx
{
	//���ýӿ�
public:
	//��ȡ����
	virtual bool SaveCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)=NULL;
	//Ĭ������
	virtual bool DefaultCustomRule(LPBYTE pcbCustomRule, WORD wCustonSize)=NULL;
	//��������
	virtual HWND CreateCustomRule(CWnd * pParentWnd, CRect rcCreate, LPBYTE pcbCustomRule, WORD wCustonSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameVideoPrivateSink INTERFACE_VERSION(1,1)
	static const GUID IID_IGameVideoPrivateSink={0x7ca0d424,0x9d7b,0x4b32,0xb4,0x50,0xea,0x69,0xc4,0x5c,0x0,0x1e};
#else
	#define VER_IGameVideoPrivateSink INTERFACE_VERSION(1,1)
	static const GUID IID_IGameVideoPrivateSink={0x1ee7b273,0xcbed,0x40a0,0xa9,0xe8,0x75,0x98,0x16,0xda,0x2,0x29};
#endif

//¼��ӿ�
interface IGameVideoPrivateSink : public IUnknownEx
{
	//���ͽӿ�
public:
	//˽������
	virtual bool SendPrivateData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IServerUserItem INTERFACE_VERSION(1,2)
	static const GUID IID_IServerUserItem={0xd5f0b821,0x8c38,0x478b,0x0095,0x73,0x55,0x89,0x23,0xf1,0xda,0xf2};
#else
	#define VER_IServerUserItem INTERFACE_VERSION(1,2)
	static const GUID IID_IServerUserItem={0x0b8f593a,0x1a9d,0x4085,0x0088,0x93,0x62,0x01,0xd7,0x50,0x09,0xdd};
#endif

//�û��ӿ�
interface IServerUserItem : public IUnknownEx
{
	//������Ϣ
public:	
	//��������
	virtual DWORD GetTokenID()=NULL;	
	//������
	virtual WORD GetBindIndex() = NULL;
	//�û���ַ
	virtual DWORD GetClientIP()=NULL;
	//�û���Ϣ
	virtual tagServerUserInfo * GetUserInfo()=NULL;
	//�û�����
	virtual tagServerUserRule * GetUserRule()=NULL;

	//������Ϣ
public:
	//�û���ʶ
	virtual DWORD GetUserID()=NULL;
	//��Ϸ��ʶ
	virtual DWORD GetGameID()=NULL;
	//�û�����
	virtual LPCTSTR GetAccounts()=NULL;

	//״̬�ӿ�
public:
	//���Ӻ���
	virtual WORD GetTableID()=NULL;
	//���Ӻ���
	virtual WORD GetChairID()=NULL;
	//�û�״̬
	virtual BYTE GetUserStatus()=NULL;

	//Ȩ����Ϣ
public:
	//�û�Ȩ��
	virtual DWORD GetUserRight()=NULL;
	//����Ȩ��
	virtual DWORD GetMasterRight()=NULL;

	//��Ϸ�ȼ�
public:
	//��Ϸ�ȼ�
	virtual BYTE GetGameOrder()=NULL;
	//��Ϸ����
	virtual DWORD GetExperience()=NULL;	

	//��Ա�ȼ�
public:
	//��Ա�ȼ�
	virtual BYTE GetMemberOrder() = NULL;
	//����ȼ�
	virtual BYTE GetMasterOrder() = NULL;
	//��Ա����
	virtual DWORD GetMemberPoint() = NULL;
	//���µ���
	virtual VOID UpdateMemberPoint(DWORD dwIncPoint) = NULL;

	//ʱ����Ϣ
public:
	//��¼ʱ��
	virtual DWORD GetLogonTime()=NULL;
	//��Ϸʱ��
	virtual DWORD GetPlayTimeCount()=NULL;
	//����Ϸʱ��
	virtual DWORD GetTotalPlayTimeCount()=NULL;

	//������Ϣ
public:
	//�û�����
	virtual SCORE GetUserScore()=NULL;	
	//���ɻ���
	virtual SCORE GetUserFreeScore()=NULL;
	//��������
	virtual SCORE GetUserLockScore()=NULL;	

	//������Ϣ
public:
	//�û�ʤ��
	virtual WORD GetUserWinRate()=NULL;
	//�û�����
	virtual WORD GetUserLostRate()=NULL;
	//�û�����
	virtual WORD GetUserDrawRate()=NULL;
	//�û�����
	virtual WORD GetUserFleeRate()=NULL;
	//��Ϸ����
	virtual LONG GetUserPlayCount()=NULL;

	//��λ��Ϣ
public:
	//��������
	virtual LONG GetLoveHeart()=NULL;	

	//Ч��ӿ�
public:
	//�Ա��ʺ�
	virtual bool ContrastAccounts(LPCTSTR pszAccounts)=NULL;
	//�Ա�����
	virtual bool ContrastPassword(LPCTSTR pszPassword)=NULL;
	//�Ա���ʱ����
	virtual bool ContrastTempPassword(LPCTSTR pszPassword)=NULL;
	//�Ա���������
	virtual bool ContrastTablePassword(LPCTSTR pszPassword)=NULL;

	//�й�״̬
public:
	//�ж�״̬
	virtual bool IsTrusteeUser()=NULL;
	//����״̬
	virtual VOID SetTrusteeUser(bool bTrusteeUser)=NULL;

	//��Ϸ״̬
public:
	//����״̬
	virtual bool IsClientReady()=NULL;
	//��������
	virtual VOID SetClientReady(bool bClientReady)=NULL;

	//�����û�
public:
	//����״̬
	virtual bool IsRobotUser()=NULL;
	//���ÿ���
	virtual VOID SetRobotUser(bool bbMachineUser)=NULL;

	//�ֻ��û�
public:
	//����״̬
	virtual bool IsMobileUser()=NULL;
	//���ÿ���
	virtual VOID SetMobileUser(bool bMobileUser)=NULL;

	//�˺�״̬
public:
	//����״̬
	virtual BYTE GetShutUpStatus() = NULL;
	//����״̬
	virtual VOID SetShutUpStatus(BYTE cbShutUpStatus) = NULL;
	//���ʱ��
	virtual DWORD GetShutUpOverTime() = NULL;
	//����ʱ��
	virtual VOID SetShutUpOverTime(DWORD dwShutUpOverTime) = NULL;

	//�ڰ�����
public:
	//��ǰʤ��
	virtual WORD GetAddupWinRate() = NULL;
	//�ۼ�Ӯ��
	virtual SCORE GetAddupWinScore() = NULL;
	//�ۼ����
	virtual SCORE GetAddupLoseScore() = NULL;
	//����״̬
	virtual BYTE GetControlStatus() = NULL;	
	//����״̬
	virtual BYTE GetRegulateStatus(SCORE lScore) = NULL;	
	//Ԥ�ڷ���
	virtual bool IsExpectScoreChange(SCORE lScore) = NULL;	
	//��������
	virtual tagUserControlItem* GetUserControlItem() = NULL;

	//�����ӿ�
public:	
	//������ʶ
	virtual DWORD GetMatchID()=NULL;
	//������ʶ
	virtual VOID SetMatchID(DWORD dwMatchID)=NULL;
	//��������
	virtual SCORE GetMatchNo()=NULL;
	//������ʶ
	virtual VOID SetMatchNo(SCORE lMatchNo)=NULL;	
	//����ʱ��
	virtual DWORD GetSignUpTime() = NULL;
	//����ʱ��
	virtual VOID SetSignUpTime(DWORD dwSignUpTime) = NULL;
	//��������
	virtual WORD GetSignupCount() = NULL;
	//��������
	virtual VOID SetSignupCount(WORD wSignupCount) = NULL;	
	//��ȯ����
	virtual DWORD GetTicketCount() = NULL;
	//��ȯ����
	virtual VOID SetTicketCount(DWORD dwTicketCount) = NULL;
	//����״̬
	virtual BYTE GetUserMatchStatus()=NULL;
	//����״̬
	virtual VOID SetUserMatchStatus(BYTE cbMatchStatus)=NULL;
	//��������
	virtual VOID InitMatchScore(SCORE lMatchScore)=NULL;	

	//�ֻ�����
public:
	//�ֻ�����
	virtual WORD GetMobileUserRule()=NULL;
	//���ö���
	virtual VOID SetMobileUserRule(WORD wMobileUserRule)=NULL;
	//��ǰ��ҳ
	virtual WORD GetMobileUserDeskPos()=NULL;
	//��ǰ��ҳ
	virtual VOID SetMobileUserDeskPos(WORD wMobileUserDeskPos)=NULL;
	//��ҳ����
	virtual WORD GetMobileUserDeskCount()=NULL;
	//��ҳ����
	virtual VOID SetMobileUserDeskCount(WORD wMobileUserDeskCount)=NULL;	

	//����ӿ�
public:
	//����״̬
	virtual bool SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID)=NULL;
	//д��Ƹ�
	virtual bool WriteUserWealth(WORD wMoneyID, SCORE lScore, WORD wReason)=NULL;
	//д�����
	virtual bool WriteUserScore(SCORE lGameScore, SCORE lRevenue, SCORE lBetAmount, LONG lExperience, enScoreType ScoreType)=NULL;
	//������ʱ����
	virtual VOID SetTmpPassword( LPCTSTR lpszPassword )=NULL;
	//�޸�Ȩ��
	virtual VOID ModifyUserRight( DWORD dwAddRight, DWORD dwRemoveRight )=NULL;
	//��ѯ��ʱ����
	virtual bool IsTmpPasswdEmpty()=NULL;

	//�����ӿ�
public:
	//�û�����
	virtual CServerUserPack* GetUserPack() = NULL;	

	//�߼��ӿ�
public:
	//����ж�
	virtual bool IsVariation()=NULL;
	//���ò���
	virtual bool SetUserParameter(DWORD dwClientIP, WORD wIndex, DWORD dwTokenID, bool bRobotUser, bool bClientReady)=NULL;
	//��ȡ���
	virtual bool DistillVariation(tagVariationScore & VariationScore, const tagLockScoreInfo & LockScoreInfo)=NULL;
	//�������
	virtual bool FreezeUserScore( SCORE lScore, bool bNotify )=NULL;

	//�����ӿ�
public:
	//������ʶ
	virtual LPCTSTR GetMachineID()=NULL;
	//��ȡ����
	virtual LPCTSTR GetPassword()=NULL;

	//�ͱ��ӿ�
public:
	//�ͱ�����
	virtual BYTE GetResidualTimes()=NULL;	
	//���ͱ�ʶ
	virtual bool IsLargessSubsidy()=NULL;
	//�ͱ�����
	virtual void SetResidualTimes(BYTE cbTimes)=NULL;	
	//���ͱ�ʶ
	virtual void SetLargessSubsidy(bool bLargessSubsidy)=NULL;	

	//�߼��ӿ�
public:
	//�����
	virtual bool DetachBindStatus()=NULL;
	//��¼����
	virtual DWORD GetInoutIndex()=NULL;
	//��ѯ��¼
	virtual bool QueryRecordInfo(tagVariationScore & UserRecordInfo)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IServerUserItemSink INTERFACE_VERSION(1,1)
	static const GUID IID_IServerUserItemSink={0x07311553,0x207d,0x4c0b,0x0095,0x65,0xf3,0xc2,0xcc,0xbd,0x7e,0x69};
#else
	#define VER_IServerUserItemSink INTERFACE_VERSION(1,1)
	static const GUID IID_IServerUserItemSink={0xb5823d39,0xdf93,0x4106,0x0086,0x3f,0x85,0x4b,0xa3,0x6a,0xfb,0x42};
#endif

//״̬�ӿ�
interface IServerUserItemSink : public IUnknownEx
{
	//���Ʊ��
public:
	//���ƿ�ʼ
	virtual bool OnEventUserControlStart(IServerUserItem * pIServerUserItem)=NULL;
	//���ƽ���
	virtual bool OnEventUserControlFinish(IServerUserItem * pIServerUserItem)=NULL;

	//��Ϣ���
public:
	//�û�����
	virtual bool OnEventUserItemScore(IServerUserItem * pIServerUserItem)=NULL;	
	//�û���λ
	virtual bool OnEventUserItemRanking(IServerUserItem* pIServerUserItem)=NULL;
	//�û�״̬
	virtual bool OnEventUserItemStatus(IServerUserItem * pIServerUserItem,WORD wLastTableID,WORD wLastChairID)=NULL;
	//�û�Ȩ��
	virtual bool OnEventUserItemRight(IServerUserItem *pIServerUserItem, DWORD dwAddRight, DWORD dwRemoveRight)=NULL;
	//�û��Ƹ�
	virtual VOID OnEventUserItemWealth(IServerUserItem* pIServerUserItem, tagWealthVariation& WealthVariation) = NULL;
	//�û��ȼ�
	virtual VOID OnEventUserUserLevel(IServerUserItem* pIServerUserItem, BYTE cbLastLevelID, BYTE cbCurrLevelID) = NULL;
	//��Ϸ�ȼ�
	virtual VOID OnEventUserGameLevel(IServerUserItem* pIServerUserItem, BYTE cbLastLevelID, BYTE cbCurrLevelID) = NULL;
	//�û�����
	virtual VOID OnEventUserItemPack(IServerUserItem* pIServerUserItem, WORD wGoodsIDList[], WORD wGoodsCount) = NULL;
	//�û�����
	virtual bool OnEventUserFreezeScore(IServerUserItem *pIServerUserItem, SCORE lFreezeScore)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IServerUserManager INTERFACE_VERSION(1,1)
	static const GUID IID_IServerUserManager={0x2760f435,0xf2b3,0x432b,0x009d,0x14,0x52,0xa2,0x45,0x01,0xcc,0xf7};
#else
	#define VER_IServerUserManager INTERFACE_VERSION(1,1)
	static const GUID IID_IServerUserManager={0xaf50cbe9,0x8c88,0x4d21,0x0089,0x05,0x0c,0x5c,0x14,0x70,0x7a,0xae};
#endif

//�û�����
interface IServerUserManager : public IUnknownEx
{
	//���ýӿ�
public:
	//���ýӿ�
	virtual bool SetServerUserItemSink(IUnknownEx * pIUnknownEx)=NULL;
	//���Ʋ���
	virtual VOID SetControlParamter(tagControlParameter* pControlParameter)=NULL;
	//�������
	virtual VOID SetServiceParameter(tagGameServiceOption* pGameServiceOption)=NULL;

	//���ҽӿ�
public:
	//ö���û�
	virtual IServerUserItem * EnumUserItem(WORD wEnumIndex)=NULL;
	//�����û�
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID)=NULL;
	//�����û�
	virtual IServerUserItem * SearchUserItem(LPCTSTR pszAccounts)=NULL;

	//ͳ�ƽӿ�
public:
	//��������
	virtual DWORD GetUserItemCount()=NULL;

	//����ӿ�
public:
	//ɾ���û�
	virtual bool DeleteUserItem() = NULL;
	//ɾ���û�
	virtual bool DeleteUserItem(IServerUserItem* pIServerUserItem) = NULL;
	//�����û�
	virtual IServerUserItem * ActiveUserItem(tagServerUserInfo& UserInfo, tagUserInfoPlus & UserInfoPlus, LPCTSTR pszPassword)=NULL;	
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableFrameItem INTERFACE_VERSION(1,2)
	static const GUID IID_ITableFrameItem={0x51a16304,0x0417,0x4ee6,0x00b8,0x31,0x7e,0x56,0x09,0x78,0xa2,0x95};
#else
	#define VER_ITableFrameItem INTERFACE_VERSION(1,2)
	static const GUID IID_ITableFrameItem={0x1655fd14,0x6dc0,0x4198,0x00ae,0x77,0x2c,0x63,0x15,0xb2,0x97,0xc1};
#endif

//���ӽӿ�
interface ITableFrameItem : public IUnknownEx
{
	//���Խӿ�
public:
	//���Ӻ���
	virtual WORD GetTableID()=NULL;
	//��Ϸ����
	virtual WORD GetChairCount()=NULL;
	//��λ����Ŀ
	virtual WORD GetNullChairCount()=NULL;
	//��Ϸʱ��
	virtual DWORD GetPlayTimeCount()=NULL;	

	//���ò���
public:
	//��ȡ����
	virtual tagGameServiceAttrib * GetGameServiceAttrib()=NULL;
	//��ȡ����
	virtual tagGameServiceOption * GetGameServiceOption()=NULL;

	//���ýӿ�
public:
	//��ʼģʽ
	virtual enStartMode GetStartMode()=NULL;
	//��ʼģʽ
	virtual VOID SetStartMode(enStartMode StartMode)=NULL;

	//��Ԫ����
public:
	//��Ԫ����
	virtual LONG GetCellScore()=NULL;
	//��Ԫ����
	virtual VOID SetCellScore(LONG lCellScore)=NULL;
	
	//��ͽ��� KK JIA
	virtual VOID SetMinEnterScore(LONGLONG lMinEnterScore) = NULL;

	//��Ϣ�ӿ�
public:
	//����״̬
	virtual bool IsTableLocked()=NULL;
	//��Ϸ״̬
	virtual bool IsGameStarted()=NULL;
	//��Ϸ״̬
	virtual bool IsTableStarted()=NULL;

	//״̬�ӿ�
public:
	//��ȡ״̬
	virtual BYTE GetGameStatus()=NULL;
	//����״̬
	virtual VOID SetGameStatus(BYTE bGameStatus)=NULL;

	//���ƽӿ�
public:
	//��ʼ��Ϸ
	virtual bool StartGame()=NULL;
	//��ͣ��Ϸ
	virtual bool PauseGame()=NULL;
	//������Ϸ
	virtual bool ContinueGame()=NULL;
	//��ɢ��Ϸ
	virtual bool DismissGame()=NULL;	
	//������Ϸ
	virtual bool ConcludeGame(LPCTSTR pszGameCourse)=NULL;

	//ͳ�ƽӿ�
public:
	//����˰��
	virtual SCORE AccountRevenue(SCORE lGameScore)=NULL;

	//д�ֽӿ�
public:	
	//�۷����
	virtual VOID CostForGameStart() = NULL;
	//д�����
	virtual bool WriteUserScore(WORD wChairID, tagScoreInfo& ScoreInfo)=NULL;
	//д�����
	virtual bool WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount)=NULL;	

	//�û��ӿ�
public:
	//Ѱ���û�
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID)=NULL;
	//��Ϸ�û�
	virtual IServerUserItem * GetTableUserItem(WORD wChairID)=NULL;
	//�Թ��û�
	virtual IServerUserItem * EnumLookonUserItem(WORD wEnumIndex)=NULL;

	//ʱ��ӿ�
public:
	//ɾ��ʱ��
	virtual bool KillGameTimer(DWORD dwTimerID) = NULL;
	//����ʱ��
	virtual bool SetGameTimer(DWORD dwTimerID, DWORD dwElapse, DWORD dwRepeat, WPARAM dwBindParameter)=NULL;	

	//����ӿ�
public:
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID)=NULL;
	//��������
	virtual bool SendTableData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize,WORD wMainCmdID=MDM_GF_GAME)=NULL;
	//��������
	virtual bool SendUserData(IServerUserItem* pIServerUserItem, WORD wSubCmdID) = NULL;
	//��������
	virtual bool SendUserData(IServerUserItem* pIServerUserItem, WORD wSubCmdID, VOID* pData, WORD wDataSize) = NULL;
	//��������
	virtual bool SendDebugData(IServerUserItem* pIServerUserItem, WORD wDebugKind, WORD wSubCmdID) = NULL;
	//��������
	virtual bool SendDebugData(IServerUserItem* pIServerUserItem, WORD wDebugKind, WORD wSubCmdID, VOID* pData, WORD wDataSize) = NULL;
	//��������
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID)=NULL;
	//��������
	virtual bool SendLookonData(WORD wChairID, WORD wSubCmdID, VOID * pData, WORD wDataSize, WORD wMainCmdID=MDM_GF_GAME)=NULL;	

	//���ܽӿ�
public:
	//������Ϣ
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType)=NULL;
	//����ƹ���
	virtual bool SendMarqeeNotice(BYTE cbNoticeKind, VOID* pNoticeContent, WORD wBufferSize) = NULL;
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)=NULL;
	//������Ϣ
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)=NULL;	

	//��������
public:
	//�Թ۶���
	virtual bool PerformLookonAction(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//���¶���
	virtual bool PerformSitDownAction(WORD wChairID, IServerUserItem * pIServerUserItem)=NULL;
	//��������
	virtual bool PerformStandUpAction(IServerUserItem* pIServerUserItem, bool bInitiative = false) = NULL;

	//���ܽӿ�
public:
	//�۳������
	virtual VOID ChargeGameAndReset() = NULL;
	//ģ������
	virtual	bool ImitateGameOption(IServerUserItem* pIServerUserItem)=NULL;
	//���ͳ���
	virtual bool OnEventSendGameScene(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)=NULL;
	//��������
	virtual VOID FreezeGameScore(IServerUserItem *pIServerUserItem)=NULL;	

	//�����ӿ�
public:
	//�������
	virtual VOID OnEventUserScoreChange(IServerUserItem *pIServerUserItem)=NULL;

	//��λ�ӿ�
public:
	//��ȡת������
	virtual WORD GetWinConvertRate() = NULL;
	//��ȡת������
	virtual WORD GetLoseConvertRate() = NULL;

	//����ƽӿ�
public:
	//��ȡӮȡ����
	virtual SCORE GetReachWinScore() = NULL;
	//��ȡ�������
	virtual SCORE GetReachAwardAmount() = NULL;

	//ͳ�ƽӿ�
public:
	//�û���Ŀ
	virtual WORD GetSitUserCount() = NULL;
	//������Ŀ
	virtual WORD GetRealUserCount() = NULL;
	//������Ŀ
	virtual WORD GetRobotUserCount() = NULL;
	//�Թ���Ŀ
	virtual WORD GetLookonUserCount() = NULL;
	//������Ŀ
	virtual WORD GetOffLineUserCount() = NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableFrameSink INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrameSink={0xeb3f34a6,0x0d9e,0x4fb6,0x0084,0xc8,0x8a,0x3e,0xae,0x70,0x10,0x60};
#else
	#define VER_ITableFrameSink INTERFACE_VERSION(1,1)
	static const GUID IID_ITableFrameSink={0xfa7f6f71,0xe2f8,0x43bf,0x008c,0xa5,0xe1,0x30,0x13,0x23,0xf3,0x81};
#endif

//�ص��ӿ�
interface ITableFrameSink : public IUnknownEx
{
	//����ӿ�
public:
	//��λ����
	virtual VOID ResetTableFrameSink() = NULL;
	//��������
	virtual bool InitTableFrameSink(IUnknownEx * pIUnknownEx)=NULL;	

	//��Ϸ�¼�
public:
	//��Ϸ��ʼ
	virtual bool OnEventGameStart()=NULL;
	//��Ϸ��ͣ
	virtual bool OnEventGamePause()=NULL;
	//��Ϸ����
	virtual bool OnEventGameContinue()=NULL;
	//��Ϸ����
	virtual bool OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)=NULL;
	//���ͳ���
	virtual bool OnEventSendGameScene(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)=NULL;

	//�¼��ӿ�
public:
	//��ʱ���¼�
	virtual bool OnTimerMessage(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;
	//��Ϸ��Ϣ
	virtual bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
	//�����Ϣ
	virtual bool OnFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_ITableControlSendCard INTERFACE_VERSION(1,1)
static const GUID IID_ITableControlSendCard = { 0xf1926b0d, 0x3990, 0x4a18, 0x9e, 0xdc, 0x2f, 0x1c, 0x44, 0xb9, 0xd9, 0xa8 };
#else
#define VER_ITableControlSendCard INTERFACE_VERSION(1,1)
static const GUID IID_ITableControlSendCard = { 0x88b657fb, 0x13e8, 0x434f, 0xb8, 0x33, 0x4, 0xf6, 0x6f, 0xba, 0xe4, 0xcd };
#endif

//���Ʒ���
interface ITableControlSendCard : public IUnknownEx
{
	//������
	virtual bool OnEventControlSendCard(IServerUserItem* pIServerUserItem, tagCardDispatcher* SendCardData) = NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_ITableUserAction INTERFACE_VERSION(1,1)
	static const GUID IID_ITableUserAction={0xfaa4ff2e,0x6347,0x4de2,0x009d,0x19,0x3d,0x35,0x1c,0xd7,0xa3,0x28};
#else
	#define VER_ITableUserAction INTERFACE_VERSION(1,1)
	static const GUID IID_ITableUserAction={0x7e42fb7f,0x0ffc,0x46fd,0x0097,0x0b,0x3d,0xe9,0x13,0x39,0x1c,0x06};
#endif

//�����¼�
interface ITableUserAction : public IUnknownEx
{
	//�û�����
	virtual bool OnActionUserOffLine(IServerUserItem * pIServerUserItem)=NULL;
	//�û�����
	virtual bool OnActionUserConnect(IServerUserItem * pIServerUserItem)=NULL;
	//�û�����
	virtual bool OnActionUserSitDown(IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//�û�����
	virtual bool OnActionUserStandUp(IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//�û�ͬ��
	virtual bool OnActionUserReady(IServerUserItem * pIServerUserItem, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_ITableUserRequest INTERFACE_VERSION(1,1)
static const GUID IID_ITableUserRequest={0xffa8f2ee,0x1080,0x4f7c,0x9f,0x38,0xbd,0xdd,0x64,0x87,0x99,0x40};
#else
#define VER_ITableUserRequest INTERFACE_VERSION(1,1)
static const GUID IID_ITableUserRequest={0x4de032b5,0x6482,0x4d49,0xbb,0xa0,0xb1,0xc4,0xd3,0x81,0x81,0x7b};
#endif

//�����¼�
interface ITableUserRequest : public IUnknownEx
{
	//�����¼�
public:
	//��������
	virtual bool OnTableUserReqConnect(IServerUserItem *pIServerUserItem)=NULL;
	//��������
	virtual bool OnTableUserReqSitDown(IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//��������
	virtual bool OnTableUserReqStandUp(IServerUserItem * pIServerUserItem, bool bLookonUser)=NULL;
	//����׼��
	virtual bool OnTableUserReqReady(IServerUserItem* pIServerUserItem, VOID* pData, WORD wDataSize) = NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IEventServiceCost INTERFACE_VERSION(1,1)
static const GUID IID_IEventServiceCost={0xc2cc9870,0xc124,0x4fc5,0x0084,0x4e,0x47,0x2a,0x21,0x42,0x35,0x9b};
#else
#define VER_IEventServiceCost INTERFACE_VERSION(1,1)
static const GUID IID_IEventServiceCost={0x9e989e0e,0xea3c,0x4179,0x009d,0x56,0x84,0xad,0x8c,0x66,0x94,0x60};
#endif

//����¼�
interface IEventServiceCost : public IUnknownEx
{
	//��ѯ���
	virtual bool OnEventQueryCostable(IServerUserItem *pIServerUserItem, bool bLookonUser)=NULL;
	//��ѯ�����
	virtual SCORE OnEventQueryCost(IServerUserItem *pIServerUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IEventUserScore INTERFACE_VERSION(1,1)
static const GUID IID_IEventUserScore={0x3e507657,0xbad1,0x4231,0xbe,0x2,0xb2,0x80,0x1d,0xad,0x23,0x20};
#else
#define VER_IEventUserScore INTERFACE_VERSION(1,1)
static const GUID IID_IEventUserScore={0x94a82fba,0x1703,0x45c4,0xb7,0x61,0xbc,0x57,0xf3,0x21,0x2d,0xc2};
#endif

//����¼�
interface IEventUserScore : public IUnknownEx
{
	//�������
	virtual VOID OnEventScoreVariation(IServerUserItem *pIServerUserItem)=NULL;
};

//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IEventBaseScore INTERFACE_VERSION(1,1)
static const GUID IID_IEventBaseScore={0x7614a289,0x8714,0x43b8,0xa0,0x4e,0xcb,0x8b,0x13,0x95,0xe1,0x1a};
#else
#define VER_IEventBaseScore INTERFACE_VERSION(1,1)
static const GUID IID_IEventBaseScore={0x9be405de,0x8fc3,0x4a74,0xb2,0x8c,0x79,0x67,0x5f,0x2c,0xaa,0xe0};
#endif

//����¼�
interface IEventBaseScore : public IUnknownEx
{
	//�׷ֱ��
	virtual VOID OnEventBaseScoreVariation(LONG lBaseScore)=NULL;
};


//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IEventStartEfficacy INTERFACE_VERSION(1,1)
static const GUID IID_IEventStartEfficacy={0xa3cd8cc1,0x7c3a,0x4052,0x92,0x24,0x44,0x89,0x8c,0x69,0x10,0xde};
#else
#define VER_IEventStartEfficacy INTERFACE_VERSION(1,1)
static const GUID IID_IEventStartEfficacy={0xddb4d2a0,0xe032,0x4d50,0xbb,0x56,0x6,0x38,0x32,0xa4,0xde,0x33};
#endif

//��ʼУ��
interface IEventStartEfficacy : public IUnknownEx
{	
	//�����ʶ
	virtual WORD GetMasterChairID()=NULL;
	//��ʼ����
	virtual WORD GetStartPlayerCount()=NULL;	
	//����׼��
	virtual bool IsAllowPlayerReady(WORD wChairID)=NULL;
};


//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IEventCustomFrameTime INTERFACE_VERSION(1,1)
static const GUID IID_IEventCustomFrameTime={0xfeab5761, 0x6dae, 0x4e61, 0xb9, 0x5a, 0x24, 0x84, 0x13, 0x7b, 0xa8, 0x3f};
#else
#define VER_IEventCustomFrameTime INTERFACE_VERSION(1,1)
static const GUID IID_IEventCustomFrameTime={0x846580ea, 0x704f, 0x4277, 0xb5, 0x8e, 0xfd, 0x75, 0x38, 0x9c, 0xea, 0x1e};
#endif

//�Զ���ʱ��
interface IEventCustomFrameTime : public IUnknownEx
{	
	//��ɢʱ��
	virtual DWORD GetBattleDismissTime()=NULL;
	//����ʱ��
	virtual DWORD GetUnReadyKickOutTime()=NULL;
	//��ɢʱ��
	virtual DWORD GetAgreementDismissTime()=NULL;
};

//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IEventGameOption INTERFACE_VERSION(1,1)
static const GUID IID_IEventGameOption={0xf750da53,0xdf05,0x4f9b,0xa4,0x6c,0x5d,0x1c,0x3c,0xe3,0xc4,0x5a};
#else
#define VER_IEventGameOption INTERFACE_VERSION(1,1)
static const GUID IID_IEventGameOption={0xa0dba87c,0x9d1b,0x4c53,0x9a,0x49,0x25,0xf5,0x5,0xed,0xe7,0x12};
#endif

//��Ϸ����
interface IEventGameOption : public IUnknownEx
{	
	//��������
	virtual VOID FormatOptionDetail(CString & strOptionDetail) = NULL;
	//��Ϸ����
	virtual bool SetNormalOption(bool bOnlyVerify, BYTE cbNormalOption[],WORD wBufferSize)=NULL;
	//��Ϸ����
	virtual bool SetAdvancedOption(bool bOnlyVerify, BYTE cbAdvancedOption[], WORD wBufferSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IEventBattleSink INTERFACE_VERSION(1,1)
static const GUID IID_IEventBattleSink={0xd88282c4,0xf92e,0x4fc2,0x9d,0xe2,0x53,0x21,0xeb,0x6a,0x60,0x11};
#else
#define VER_IEventBattleSink INTERFACE_VERSION(1,1)
static const GUID IID_IEventBattleSink={0x6e73b365,0xc988,0x45dd,0x8b,0x3f,0x69,0x4a,0xe7,0x64,0x8,0xc0};
#endif

//Լս�ӿ�
interface IEventBattleSink : public IUnknownEx
{	
	//Լս����
public:
	//Լս����
	virtual bool BattleFinish(BYTE cbReason)=NULL;
	//��ȡ����
	virtual bool GetOptionValueArray(WORD wOptionIndex, COptionValueArray & OptionValueArray)=NULL;

	//Լս����
public:
	//Լս����
	virtual DWORD GetBattleRule()=NULL;
	//Լս����
	virtual VOID SetBattleRule(DWORD dwAddRules, DWORD dwRemoveRules)=NULL;
};

//////////////////////////////////////////////////////////////////////////
#ifdef _UNICODE
#define VER_IEventBattleItem INTERFACE_VERSION(1,1)
static const GUID IID_IEventBattleItem={0x76618df0,0xdc52,0x40aa,0x95,0x81,0x48,0x7c,0x74,0x48,0x1d,0x91};
#else
#define VER_IEventBattleItem INTERFACE_VERSION(1,1)
static const GUID IID_IEventBattleItem={0x7e4557f1,0x20aa,0x4937,0xb0,0x47,0x89,0xf7,0x1e,0x48,0xc4,0x6d};
#endif

//Լս����
interface IEventBattleItem : public IUnknownEx
{	
	//��Ϣ����
public:
	//Ӯȡ����
	virtual SCORE GetBattleWinScore()=NULL;
	//ͬ������
	virtual WORD GetDismissAgreeCount()=NULL;		
	//ԼսժҪ
	virtual LPCTSTR GetUserBattleAbstract(WORD wChairID)=NULL;

	//�¼�֪ͨ
public:
	//���ӽ�ɢ
	virtual VOID OnEventTableDismiss()=NULL;	
	//�����޸�
	virtual VOID OnEventTableModify(LPVOID pModifyParam, WORD wBufferSize)=NULL;
	//���Ӵ���
	virtual VOID OnEventTableCreated(LPVOID pCreateParam, WORD wBufferSize)=NULL;	
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_ITableEventSink INTERFACE_VERSION(1,1)
static const GUID IID_ITableEventSink={0x1fdc6b5b,0x5f1d,0x4761,0x0089,0x61,0x5c,0x2f,0x1c,0x85,0xf7,0xe};
#else
#define VER_ITableEventSink INTERFACE_VERSION(1,1)
static const GUID IID_ITableEventSink={0xb5bfe81e,0x95f,0x494b,0x00b1,0xb9,0x5a,0xb1,0x23,0x46,0xf9,0x63};
#endif

//�����¼�
interface ITableEventSink : public IUnknownEx
{
	//�����¼�
public:
	//�������¼�
	virtual VOID OnEventTableLocked(ITableFrameItem *pITableFrame, BOOL bTableLocked )=NULL;
	//����״̬�¼�
	virtual VOID OnEventTableStatus(ITableFrameItem *pITableFrame, BOOL bPlayStatus )=NULL;

	//�����¼�
public:
	//����д������
	virtual VOID OnTableReqOfflineScore( tagOfflineScore const &OfflineScore )=NULL;
	//�ⶳ��������
	virtual VOID OnTableReqUnfreezeScore( DWORD dwUserID, SCORE lUnfreezeScore )=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IServiceFunction INTERFACE_VERSION(1,1)
	static const GUID IID_IServiceFunction={0x4888f916,0x12c9,0x4a5c,0x00a9,0x4d,0x6c,0xaf,0x1f,0xd0,0x48,0xd0};
#else
	#define VER_IServiceFunction INTERFACE_VERSION(1,1)
	static const GUID IID_IServiceFunction={0x20bb62da,0x77d8,0x4d54,0x008a,0x6f,0xeb,0x7e,0x8d,0xe8,0x15,0x6c};
#endif

//������
interface IServiceFunction : public IUnknownEx
{
	//��Ϣ�ӿ�
public:
	//��ȡ״̬
	virtual WORD GetServiceStatus()=NULL;

	//����ӿ�
public:
	//��������
	virtual bool SendBatchData(BYTE cbSendMask,WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//Ⱥ������
	virtual bool SendMobileData(WORD wCmdTable, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize) = NULL;	
	//��������
	virtual bool SendDataToGate(DWORD dwContextID, DWORD dwTokenID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//��������
	virtual bool SendDataToUser(IServerUserItem * pIServerUserItem, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;	
	//��������
	virtual bool SendDebugData(IServerUserItem* pIServerUserItem, WORD wTableID, WORD wDebugKind, WORD wSubCmdID, VOID* pData, WORD wDataSize) = NULL;

	//����ӿ�
public:
	//��������
	virtual bool SendDataToCenter(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)=NULL;	

	//��Ϣ�ӿ�
public:
	//������Ϣ
	virtual bool SendRoomMessage(LPCTSTR lpszMessage, WORD wType)=NULL;
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(LPCTSTR lpszMessage, WORD wType)=NULL;
	//����ƹ���
	virtual bool SendMarqeeNotice(BYTE cbNoticeKind, VOID* pNoticeContent, WORD wNoticeSize) = NULL;
	//������Ϣ
	virtual bool SendRoomMessage(DWORD dwSocketID, DWORD dwTokenID, LPCTSTR lpszMessage, WORD wType)=NULL;
	//������Ϣ
	virtual bool SendRoomMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)=NULL;
	//��Ϸ��Ϣ
	virtual bool SendGameMessage(IServerUserItem * pIServerUserItem, LPCTSTR lpszMessage, WORD wType)=NULL;	

	//���ӿ�
public:
	//������
	virtual CShareStockItemPtr GetStockItem(WORD wStockKind, WORD wStockSubtype) = NULL;
	//�޸Ŀ��
	virtual bool ModifyStockScore(WORD wStockKind, WORD wStockSubtype, SCORE lModifyScore) = NULL;

	//֪ͨ�ӿ�
public:	
	//��Ϸ��ʼ
	virtual bool OnEventGameStart(ITableFrameItem* pITableFrame) = NULL;
	//��Ϸ����
	virtual bool OnEventGameConclude(ITableFrameItem * pITableFrame)=NULL;		

	//����ӿ�
public:
	//�������
	virtual bool InsertDistribute(IServerUserItem * pIServerUserItem)=NULL;
	//�Ƴ�����
	virtual bool RemoveDistribute(IServerUserItem * pIServerUserItem)=NULL;		

	//Լս�ӿ�
public:
	//��ֵ����
	virtual bool GetOptionValueArray(WORD wEmployScene, WORD wOptionIndex, COptionValueArray & OptionValueArray)=NULL;	

	//�洢�ӿ�
public:
	//�洢¼��
	virtual bool StoreVideoData(VOID * pData,WORD wDataSize)=NULL;

	//���ܽӿ�
public:
	//���дʹ���
	virtual bool FilterWords(LPCTSTR szFiltered)=NULL;
	//���дʹ���
	virtual void FilterWords(LPWSTR szFiltered, int nMsgLen)=NULL;
	//���ͷ���
	virtual bool SendUserScore(IServerUserItem *pIServerUserItem)=NULL;	
};


//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IRobotService INTERFACE_VERSION(1,1)
static const GUID IID_IRobotService = { 0x5468bfb,0xeb56,0x480c,0xa9,0x82,0x62,0x1a,0xc2,0x20,0xf9,0xa2 };
#else
#define VER_IRobotService INTERFACE_VERSION(1,1)
static const GUID IID_IRobotService = { 0xcf77eef2,0xbe9,0x49d8,0xb4,0x50,0xcb,0x27,0xe5,0x5f,0xc8,0x44 };
#endif

//�����˽ӿ�
interface IRobotService : public IUnknownEx
{
	//���ܽӿ�
public:
	//����������
	virtual VOID* CreateRobotUserItemSink(REFGUID Guid, DWORD dwQueryVer) = NULL;
};

DECLARE_MODULE_DYNAMIC(RobotService, RobotService);

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IRobotUserItem INTERFACE_VERSION(1,1)
static const GUID IID_IRobotUserItem = { 0x014b4ef4,0xa9d5,0x45cb,0x0092,0xa1,0x01,0xcf,0xe9,0xe7,0x0f,0x3c };
#else
#define VER_IRobotUserItem INTERFACE_VERSION(1,1)
static const GUID IID_IRobotUserItem = { 0xebb22c87,0x7612,0x4707,0x00b3,0xf8,0x95,0xdd,0x51,0xad,0x93,0x98 };
#endif

//�����˽ӿ�
interface IRobotUserItem : public IUnknownEx
{
	//״̬����
public:
	//��ȡ״̬
	virtual BYTE GetGameStatus() = NULL;
	//����״̬
	virtual VOID SetGameStatus(BYTE cbGameStatus) = NULL;

	//��Ϣ�ӿ�
public:
	//��ȡ I D
	virtual DWORD GetUserID() = NULL;
	//���Ӻ���
	virtual WORD GetTableID() = NULL;
	//���Ӻ���
	virtual WORD GetChairID() = NULL;
	//�û�״̬
	virtual BYTE GetUserStatus() = NULL;
	//�û�����
	virtual LPCTSTR GetAccounts() = NULL;

	//������Ϣ
public:
	//�û�����
	virtual SCORE GetUserScore() = NULL;

	//����ӿ�
public:
	//���ͺ���
	virtual bool SendSocketData(WORD wSubCmdID) = NULL;
	//���ͺ���
	virtual bool SendSocketData(WORD wSubCmdID, VOID * pData, WORD wDataSize) = NULL;

	//��Ϣ�ӿ�
public:
	//��ȡ�Լ�
	virtual IServerUserItem* GetMeUserItem() = NULL;	

	//ͬ����Ϣ
public:
	//������Ŀ
	virtual WORD GetTableRealUserCount() = NULL;
	//ͬ���û�
	virtual IServerUserItem* GetTableUserItem(WORD wChairID) = NULL;

	//���ܽӿ�
public:
	//ɾ��ʱ��
	virtual bool KillGameTimer(UINT nTimerID) = NULL;
	//����ʱ��
	virtual bool SetGameTimer(UINT nTimerID, UINT nElapse) = NULL;
	//����׼��
	virtual bool SendUserReady(VOID * pData, WORD wDataSize) = NULL;	
	//��������
	virtual bool SendChatMessage(DWORD dwTargetUserID, LPCTSTR pszChatMessage, COLORREF crFontColor) = NULL;

	//���ú���
public:
	//���ð汾
	virtual VOID SetAdroidVersion(DWORD dwVersion) = NULL;
	//���ö���
	virtual void SetRobotAction(WORD wTestAction, bool bEnable) = NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IRobotUserItemSink INTERFACE_VERSION(1,1)
static const GUID IID_IRobotUserItemSink = { 0x366aba89,0xcca2,0x40d6,0x0087,0x95,0xb5,0xb2,0xb6,0x1d,0x29,0x80 };
#else
#define VER_IRobotUserItemSink INTERFACE_VERSION(1,1)
static const GUID IID_IRobotUserItemSink = { 0x4cbe7840,0x5068,0x42fb,0x0083,0x38,0x8a,0xb6,0x58,0x65,0xa5,0xdd };
#endif

//�����˽ӿ�
interface IRobotUserItemSink : public IUnknownEx
{
	//���ƽӿ�
public:
	//���ýӿ�
	virtual bool ResetUserItemSink() = NULL;
	//��ʼ�ӿ�
	virtual bool InitUserItemSink(IUnknownEx * pIUnknownEx) = NULL;	

	//�����ӿ�
public:
	//��Ϊģ��
	virtual bool SetActionTemplate(tagActionTemplateData * pActionTemplateData)=NULL;

	//��Ϸ�¼�
public:
	//ʱ����Ϣ
	virtual bool OnEventTimer(UINT nTimerID) = NULL;
	//��Ϸ��Ϣ
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize) = NULL;
	//��Ϸ��Ϣ
	virtual bool OnEventFrameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize) = NULL;
	//������Ϣ
	virtual bool OnEventSendGameScene(BYTE cbGameStatus, bool bLookonOther, VOID * pData, WORD wDataSize) = NULL;

	//�û��¼�
public:
	//�û�����
	virtual VOID OnEventUserEnter(IRobotUserItem * pIRobotUserItem, bool bLookonUser) = NULL;
	//�û��뿪
	virtual VOID OnEventUserLeave(IRobotUserItem * pIRobotUserItem, bool bLookonUser) = NULL;
	//�û�����
	virtual VOID OnEventUserScore(IRobotUserItem * pIRobotUserItem, bool bLookonUser) = NULL;
	//�û�״̬
	virtual VOID OnEventUserStatus(IRobotUserItem * pIRobotUserItem, bool bLookonUser) = NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IRobotUserManager INTERFACE_VERSION(1,1)
	static const GUID IID_IRobotUserManager={0x847b487d,0x2a16,0x4de9,0x008b,0xba,0x54,0x55,0x8f,0x47,0xd4,0x6c};
#else
	#define VER_IRobotUserManager INTERFACE_VERSION(1,1)
	static const GUID IID_IRobotUserManager={0xded38804,0x8413,0x4f56,0x0089,0x9b,0xcd,0x11,0x7a,0x19,0x37,0x51};
#endif

//�����˽ӿ�
interface IRobotUserManager : public IUnknownEx
{
	//���ƽӿ�
public:
	//��������
	virtual bool StartService()=NULL;
	//ֹͣ����
	virtual bool ConcludeService()=NULL;

	//���ýӿ�
public:	
	//�Ƴ�����
	virtual bool RemoveRobotParameter(DWORD dwBatchID)=NULL;
	//��Ӳ���
	virtual bool AddRobotParameter(tagRobotParameter RobotParameter[], WORD wParemeterCount)=NULL;
	//�������
	virtual bool InsertRobotInfo(tagRobotAccountsInfo RobotAccountsInfo[],WORD wRobotCount,DWORD dwBatchID)=NULL;

	//����ӿ�
public:
	//Ͷ�ݶ���
	virtual bool PostponeRobotAction(DWORD dwUserID,WORD wActionCode)=NULL;
	//ɾ������
	virtual bool DeleteRobotUserItem(DWORD dwRobotID,bool bStockRetrieve)=NULL;
	//���һ���
	virtual IRobotUserItem * SearchRobotUserItem(DWORD dwUserID)=NULL;
	//��������
	virtual IRobotUserItem * CreateRobotUserItem(tagRobotItemConfig & RobotItemConfig)=NULL;	

	//�¼��ӿ�
public:
	//�����¼�
	virtual bool OnEventTimerPulse(DWORD dwTimerID, WPARAM dwBindParameter)=NULL;

	//״̬�ӿ�
public:
	//������Ŀ
	virtual WORD GetRobotCount()=NULL;
	//���ػ���
	virtual bool GetRobotLoadInfo(DWORD & dwBatchID,DWORD & dwLoadCount)=NULL;
	//�û�״��
	virtual WORD GetRobotUserInfo(tagRobotUserInfo & RobotUserInfo, DWORD dwServiceMode)=NULL;	

	//����ӿ�
public:
	//��������
	virtual bool SendDataToClient(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//��������
	virtual bool SendDataToClient(DWORD dwRobotID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
	//��������
	virtual bool SendDataToServer(DWORD dwRobotID, WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize)=NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGameServiceCustomTime INTERFACE_VERSION(1,1)
static const GUID IID_IGameServiceCustomTime={0xf57573b0,0x63c3,0x43a9,0x8c,0xb1,0xe2,0x22,0xd3,0x93,0xe2,0xb};
#else
#define VER_IGameServiceCustomTime INTERFACE_VERSION(1,1)
static const GUID IID_IGameServiceCustomTime={0xf57573b0,0x63c3,0x43a9,0x8c,0xb1,0xe2,0x22,0xd3,0x93,0xe2,0xb};
#endif

//ʱ������
interface IGameServiceCustomTime : public IUnknownEx
{	
	//��������
	virtual DWORD GetRobotTimerPulse()=NULL;
	//ʱ�䷶Χ
	virtual DWORD GetTableSinkTimeRange()=NULL;
	//ʱ�䵥Ԫ
	virtual DWORD GetTimerEngineTimeCell()=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
	#define VER_IGameServiceUnits INTERFACE_VERSION(1,1)
	static const GUID IID_IGameServiceUnits={0x9385d3a0,0x448a,0x4ad8,0xbc,0x6f,0x06,0x8f,0xb0,0xb0,0xe1,0x8f};
#else
	#define VER_IGameServiceUnits INTERFACE_VERSION(1,1)
	static const GUID IID_IGameServiceUnits={0xc5950f08,0x41f2,0x4c18,0xaa,0x2b,0x48,0xd2,0x83,0x28,0x61,0x38};
#endif

//����Ԫ
interface IGameServiceUnits : public IUnknownEx
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
	//��Ϸ����
	virtual bool SetServiceCollocate(LPCTSTR pszGameModule, const tagGameServiceOption & GameServiceOption)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IGameBalanceControl INTERFACE_VERSION(1,1)
static const GUID IID_IGameBalanceControl = { 0xce9d1609, 0x9873, 0x4fa6, 0xba, 0x95, 0x5b, 0x36, 0xc4, 0x1, 0x47, 0xee };
#else
#define VER_IGameBalanceControl INTERFACE_VERSION(1,1)
static const GUID IID_IGameBalanceControl = { 0xdf9be98, 0xd15c, 0x4bc4, 0xa1, 0x75, 0xa7, 0x9f, 0xb1, 0x3d, 0xb1, 0x6c };
#endif

//��Ϸƽ�����
interface IGameBalanceControl : public IUnknownEx
{
	//��ս����
public:
	//�����˿�
	virtual bool IsControlCard()=NULL;
	//�˿˵ȼ�
	virtual BYTE GetUserCardLevel(WORD wChairID)=NULL;	

	//Ⱥ�ؽӿ�
public:
	//������
	virtual CShareStockItemPtr GetStockItem(WORD wStockKind, WORD wStockSubtype)=NULL;
	//�޸Ŀ��
	virtual bool ModifyStockScore(WORD wStockKind, WORD wStockSubtype, SCORE lModifyScore)=NULL;
};

//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IExtendDataBaseSink INTERFACE_VERSION(1, 1)
static const GUID IID_IExtendDataBaseSink = { 0x336e9f83,0x5688,0x4957,0xa7,0x4b,0x77,0xff,0x98,0xb7,0xef,0x51 };
#else
#define VER_IExtendDataBaseSink INTERFACE_VERSION(1, 1)
static const GUID IID_IExtendDataBaseSink = { 0x2ca4a68f,0x3c0a,0x4ee4,0xbb,0x3d,0x98,0xdd,0xf8,0xe7,0xc8,0xb3 };
#endif

//��չ��������
interface IExtendDataBaseSink : public IUnknownEx
{
	//���ýӿ�
public:
	//��ʼ��
	virtual bool InitExtendDataBaseSink(tagExtendDBSinkParameter& ExtendDBSinkParameter) = NULL;

	//�ں��¼�
public:
	//ʱ���¼�
	virtual bool OnDataBaseEngineTimer(DWORD dwTimerID, WPARAM dwBindParameter) = NULL;
	//�����¼�
	virtual bool OnDataBaseEngineControl(WORD wControlID, VOID* pData, WORD wDataSize) = NULL;
	//�����¼�
	virtual bool OnDataBaseEngineRequest(WORD wRequestID, DWORD dwContextID, DWORD dwTokenID, VOID* pData, WORD wDataSize, DWORD* pdwControlCode) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#define VER_IExtendServiceManager INTERFACE_VERSION(1,1)
static const GUID IID_IExtendServiceManager = { 0xd513eace,0xb67d,0x43d9,0x0097,0xfa,0xd8,0xa7,0x9d,0x31,0x39,0x9b };
#else
#define VER_IExtendServiceManager INTERFACE_VERSION(1,1)
static const GUID IID_IExtendServiceManager = { 0x7d381e8a,0x8c10,0x4443,0x00b5,0xad,0x75,0x4b,0xe5,0x50,0xc5,0x53 };
#endif

//��չ�������ӿ�
interface IExtendServiceManager : public IUnknownEx
{
	//���ƽӿ�
public:
	//��������
	virtual bool StartService() = NULL;
	//ֹͣ����
	virtual bool ConcludeService() = NULL;	

	//����ӿ�
public:
	//��������
	virtual VOID * CreateExtendTableFrame() = NULL;
	//��������
	virtual VOID * CreateExtendDataBaseSink(REFGUID Guid, DWORD dwQueryVer) = NULL;
	//��ʼ��
	virtual bool InitServiceManager(tagExtendServiceParameter& ExtendServiceParameter) = NULL;

	//ϵͳ�¼�
public:
	//ʱ���¼�
	virtual bool OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter) = NULL;
	//���ݿ��¼�
	virtual bool OnEventDataBase(WORD wRequestID, VOID* pData, WORD wDataSize, DWORD dwContextID, DWORD dwTokenID) = NULL;

	//�����¼�
public:
	//�����¼�
	virtual bool OnEventSocketRead(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize) = NULL;
	//�����¼�
	virtual bool OnEventNetworkRead(WORD wSubCmdID, VOID* pData, WORD wDataSize, IServerUserItem* pIServerUserItem, DWORD dwSocketID, DWORD dwTokenID) = NULL;

	//�û��ӿ�
public:
	//�û���¼
	virtual bool OnEventUserLogon(IServerUserItem* pIServerUserItem) = NULL;
	//�û��ǳ�
	virtual bool OnEventUserLogout(IServerUserItem* pIServerUserItem) = NULL;
};

//////////////////////////////////////////////////////////////////////////

//��Ϸ����
DECLARE_MODULE_DYNAMIC(GameServiceManager, GameServiceManager)

//����Ԫ
DECLARE_MODULE_HELPER(GameServiceUnits, GameServiceUnits, GAME_SERVICE_DLL_NAME, "CreateServiceUnits")


//////////////////////////////////////////////////////////////////////////
//�����ļ�

#ifndef GAME_SERVICE_DLL
	#include "TableFrame.h"
	#include "ServerUserManager.h"
#endif

//////////////////////////////////////////////////////////////////////////

#endif