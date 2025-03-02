#ifndef SIMULATE_LINK_HEAD_HEAD_FILE
#define SIMULATE_LINK_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//ƽ̨����
#include "..\..\ȫ�ֶ���\Platform.h"

//��Ϣ����
#include "..\..\��Ϣ����\CMD_Commom.h"
#include "..\..\��Ϣ����\CMD_GateServer.h"
#include "..\..\��Ϣ����\CMD_GameServer.h"
#include "..\..\��Ϣ����\CMD_LogonServer.h"
#include "..\..\��Ϣ����\CMD_BattleServer.h"
#include "..\..\��Ϣ����\CMD_RankingServer.h"
#include "..\..\��Ϣ����\CMD_MatchServer.h"
#include "..\..\��Ϣ����\CMD_ClubServer.h"
#include "..\..\��Ϣ����\CMD_ClubPlaza.h"
#include "..\..\��Ϣ����\CMD_MessageServer.h"

//ƽ̨�ļ�
#include "..\..\���������\��Ϸ����\GameServiceHead.h"
#include "..\..\�������\��������\CommonServiceHead.h"
#include "..\..\�ͻ������\�������\NetworkServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//ʱ���ʶ

//��ʼ��ʶ
#define IDI_MODULE_START			100   								//��ʼ��ʶ

//��¼ģ��
#define IDI_MODULE_LOGON_START		IDI_MODULE_START+1   				//��ʼ��ʶ
#define IDI_MODULE_LOGON_RANGE		IDI_MODULE_START+99					//��ʶ��Χ

//����ģ��
#define IDI_MODULE_SERVER_START		IDI_MODULE_START+100   				//��ʼ��ʶ
#define IDI_MODULE_SERVER_RANGE		IDI_MODULE_START+199				//��ʶ��Χ

//Լսģ��
#define IDI_MODULE_BATTLE_START		IDI_MODULE_START+200   				//��ʼ��ʶ
#define IDI_MODULE_BATTLE_RANGE		IDI_MODULE_START+299				//��ʶ��Χ

//����ģ��
#define IDI_MODULE_MATCH_START		IDI_MODULE_START+300   				//��ʼ��ʶ
#define IDI_MODULE_MATCH_RANGE		IDI_MODULE_START+399				//��ʶ��Χ

//��λģ��
#define IDI_MODULE_RANKING_START	IDI_MODULE_START+400   				//��ʼ��ʶ
#define IDI_MODULE_RANKING_RANGE	IDI_MODULE_START+499				//��ʶ��Χ

//����ģ��
#define IDI_MODULE_CLUB_START		IDI_MODULE_START+500   				//��ʼ��ʶ
#define IDI_MODULE_CLUB_RANGE		IDI_MODULE_START+599				//��ʶ��Χ

//�㳡ģ��
#define IDI_MODULE_CLUBPLAZA_START	IDI_MODULE_START+600   				//��ʼ��ʶ
#define IDI_MODULE_CLUBPLAZA_RANGE	IDI_MODULE_START+699				//��ʶ��Χ

//////////////////////////////////////////////////////////////////////////////////
//�ṹ����

//���ݰ�
struct tagDataPacket
{
	WORD							wMainCmdID;							//��������
	WORD							wSubCmdID;							//��������
	WORD							wDataSize;							//���ݴ�С
	BYTE							cbDataBuffer[SOCKET_BUFFER-12];		//���ݻ���
};

//�û���Ϣ
struct tagGlobalUserData
{
	WORD							wFaceID;							//ͷ���ʶ
	BYTE							cbGender;							//�û��Ա�
	DWORD							dwCustomID;							//�Զ�ͷ��
	DWORD							dwUserID;							//�û� I D
	DWORD							dwGameID;							//��Ϸ I D
	TCHAR							szNickName[LEN_ACCOUNTS];			//�û��ǳ�
	TCHAR							szAccounts[LEN_ACCOUNTS];			//��¼�ʺ�
	TCHAR							szLogonPass[LEN_PASSWORD];			//��¼����
	TCHAR							szInsurePass[LEN_PASSWORD];			//��������

	//����ȼ�
	DWORD							dwExperience;						//������ֵ

	//��Ա����
	WORD							wMasterOrder;						//����ȼ�
	DWORD							dwMemberPoint;						//��Ա����
	DWORD							dwMemberOverTime;					//����ʱ��

	//�Ƹ���Ϣ
	LONGLONG						lGameScore;							//��Ϸ����
	LONGLONG						lUserGold;							//�û��ƽ�
	LONGLONG						lUserCard;							//�û���ʯ	
	LONGLONG						lUserInsure;						//�û�����	

	//��Ϊ����
	DWORD							dwActionMaskEver;					//��Ϊ����
	DWORD							dwActionMaskPerDay;					//��Ϊ����
	DWORD							dwActionMaskPerWeek;				//��Ϊ����

	//�ϼ���Ϣ
	DWORD							dwParentGameID;						//�û� I D
	WORD							wParentFaceID;						//ͷ��ID
	TCHAR							szParentNickName[LEN_ACCOUNTS];		//�û��ǳ�

	//������Ϣ	
	DWORD							dwLikedNumber;						//������Ŀ	

	//������Ϣ
	WORD							wLockServerID;						//��������

	//��չ��Ϣ
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//�ֻ�����
	TCHAR							szLogonToken[LEN_LOGON_TOKEN];		//��¼����

	//״̬��Ϣ
	WORD							wTableID;							//��������
	WORD							wChairID;							//����λ��
	BYTE							cbUserStatus;						//�û�״̬

	WORD GetChairID() { return wChairID; }
	SCORE GetUserScore() { return lGameScore; }
};

//////////////////////////////////////////////////////////////////////////////////
//�ӿڶ���

//ģ������
interface IModuleItem
{
	//��Ϣ����
public:
	//ʱ�ӱ�ʶ
	virtual DWORD GetTimerStart() = NULL;
	//ʱ�ӷ�Χ
	virtual DWORD GetTimerRange() = NULL;

	//���ܺ���
public:
	//�ر�ʱ��
	virtual bool KillModuleTimer(UINT uTimerID)=NULL;
	//����ʱ��
	virtual bool SetModuleTimer(UINT uTimerID, UINT nElapse)=NULL;

	//ʱ���¼�
public:
	//����ʱ��
	virtual bool OnEventTimer(UINT uTimerID, WPARAM wBindParam) = NULL;
};

//////////////////////////////////////////////////////////////////////////////////
//�ӿڶ���

//ģ��ص��ӿ�
interface IModuleItemSink
{
	//ʱ�ӽӿ�
public:
	//����ʱ��
	virtual VOID KillGameTimer(UINT uTimerID)=NULL;
	//����ʱ��
	virtual VOID SetGameTimer(UINT uTimerID, DWORD dwElapse)=NULL;	

	//��Ϣ�ӿ�
public:
	//�û�����
	virtual tagGlobalUserData* GetGlobalUserData() = NULL;

	//ģ��ӿ�
public:
	//����ģ��
	virtual VOID * GetModuleClub() = NULL;
	//����ģ��
	virtual VOID * GetModuleServer() = NULL;
	//Լսģ��
	virtual VOID * GetModuleBattle() = NULL;
	//����ģ��
	virtual VOID * GetModuleClubPlaza() = NULL;

	//���ܽӿ�
public:
	//������Ϣ
	virtual VOID InsertString(LPCTSTR pszString, COLORREF crColor)=NULL;
	//Ͷ����Ϣ
	virtual VOID PostWindowMessage(UINT MsgID, WPARAM wParam, LPARAM lParam) = NULL;	
};

//////////////////////////////////////////////////////////////////////////////////

#endif