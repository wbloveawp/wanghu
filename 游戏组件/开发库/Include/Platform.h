#ifndef PLATFORM_HEAD_HEAD_FILE
#define PLATFORM_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//ƽ̨����
const TCHAR	szProduct[]=TEXT("ZZ6801");									//��Ʒ����
const TCHAR szProductKey[]=TEXT("ZZPlatform");							//��Ʒ����
const TCHAR szPlazaClass[]=TEXT("ZZGamePlaza");							//�㳡����
const TCHAR szLogonServer[]=TEXT("127.0.0.1");							//��¼��ַ

//////////////////////////////////////////////////////////////////////////

//����汾
#define VERSION_FRAME				PROCESS_VERSION(8,0,1)				//��ܰ汾

//�����汾
#define VERSION_PLAZA				PROCESS_VERSION(8,0,1)				//�����汾
#define VERSION_MOBILE_IOS			PROCESS_VERSION(8,0,1)				//�ֻ��汾
#define VERSION_MOBILE_ANDROID		PROCESS_VERSION(8,0,1)				//�ֻ��汾

//�����汾
#define VERSION_FRAME_SDK			INTERFACE_VERSION(8,1)				//��ܰ汾

//////////////////////////////////////////////////////////////////////////
//���ݿ���

const TCHAR szMessageDB[] = TEXT("ZZMessageDB");						//��Ϣ���ݿ�
const TCHAR szPlatformDB[] = TEXT("ZZPlatformDB");						//ƽ̨���ݿ�
const TCHAR szAccountsDB[] = TEXT("ZZAccountsDB");						//�û����ݿ�
const TCHAR	szGameGoldDB[] = TEXT("ZZGameGoldDB");						//�Ƹ����ݿ�
const TCHAR	szGameScoreDB[] = TEXT("ZZGameScoreDB");					//�������ݿ�
const TCHAR szGameMatchDB[] = TEXT("ZZGameMatchDB");					//�������ݿ�
const TCHAR szGameBattleDB[] = TEXT("ZZGameBattleDB");					//Լս���ݿ�
const TCHAR szGameRankingDB[] = TEXT("ZZGameRankingDB");				//��λ���ݿ�

//////////////////////////////////////////////////////////////////////////
//���þ���
#pragma warning(disable:4200)

//////////////////////////////////////////////////////////////////////////

//����ͷ�ļ�
#include "Condition.h"				//�����궨��

//ƽ̨ͷ�ļ�
#include "Macro.h"					//�����궨��
#include "Define.h"					//���ݺ궨��
#include "Struct.h"					//�ṹ�嶨��
#include "Packet.h"					//���ݰ�����
#include "Message.h"				//��Ϣ��������

//ģ��ͷ�ļ�
#include "Array.h"					//����ģ����
#include "Module.h"					//���ģ����
#include "Stock.h"					//�ֿ�ģ����
#include "AutoPtr.h"				//����ָ��
#include "Singleton.h"				//����ģ��
#include "PacketAide.h"				//���ݰ�����

//////////////////////////////////////////////////////////////////////////
//����ģ��

#define	FUCTION_MODULE_CHAT			//����ģ��	
#define	FUCTION_MODULE_VIDEO		//¼��ģ��
#define	FUCTION_MODULE_MACTH		//����ģ��	
#define	FUCTION_MODULE_BATTLE		//Լսģ��
#define	FUCTION_MODULE_RANKING		//��λģ��

//////////////////////////////////////////////////////////////////////////

#endif