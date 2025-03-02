#ifndef PLATFORM_HEAD_HEAD_FILE
#define PLATFORM_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//ƽ̨����
const TCHAR	szProduct[]=TEXT("LH6801");									//��Ʒ����
const TCHAR szProductKey[]=TEXT("WHCDTYLHU3DPlatform");					//��Ʒ����
const TCHAR szPlazaClass[]=TEXT("WHCDTYLHU3DGamePlaza");				//�㳡����
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

const TCHAR szMessageDB[] = TEXT("LHMessageDB");						//��Ϣ���ݿ�
const TCHAR szPlatformDB[] = TEXT("LHPlatformDB");						//ƽ̨���ݿ�
const TCHAR szAccountsDB[] = TEXT("LHAccountsDB");						//�û����ݿ�
const TCHAR	szGameGoldDB[] = TEXT("LHGameGoldDB");						//�Ƹ����ݿ�
const TCHAR	szGameScoreDB[] = TEXT("LHGameScoreDB");					//�������ݿ�
const TCHAR szGameMatchDB[] = TEXT("LHGameMatchDB");					//�������ݿ�
const TCHAR szGameBattleDB[] = TEXT("LHGameBattleDB");					//Լս���ݿ�
const TCHAR szGameRankingDB[] = TEXT("LHGameRankingDB");				//��λ���ݿ�

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

//��Ȩ��Ϣ
const TCHAR szCompilation[] = TEXT("A89137E61817FC4DFC3CA1B46A13DA35");

//////////////////////////////////////////////////////////////////////////////////

#endif