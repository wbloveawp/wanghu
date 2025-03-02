#ifndef PLATFORM_HEAD_HEAD_FILE
#define PLATFORM_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////

//平台常量
const TCHAR	szProduct[]=TEXT("LH6801");									//产品名字
const TCHAR szProductKey[]=TEXT("WHCDTYLHU3DPlatform");					//产品主键
const TCHAR szPlazaClass[]=TEXT("WHCDTYLHU3DGamePlaza");				//广场类名
const TCHAR szLogonServer[]=TEXT("127.0.0.1");							//登录地址

//////////////////////////////////////////////////////////////////////////

//程序版本
#define VERSION_FRAME				PROCESS_VERSION(8,0,1)				//框架版本

//大厅版本
#define VERSION_PLAZA				PROCESS_VERSION(8,0,1)				//大厅版本
#define VERSION_MOBILE_IOS			PROCESS_VERSION(8,0,1)				//手机版本
#define VERSION_MOBILE_ANDROID		PROCESS_VERSION(8,0,1)				//手机版本

//开发版本
#define VERSION_FRAME_SDK			INTERFACE_VERSION(8,1)				//框架版本

//////////////////////////////////////////////////////////////////////////
//数据库名

const TCHAR szMessageDB[] = TEXT("LHMessageDB");						//消息数据库
const TCHAR szPlatformDB[] = TEXT("LHPlatformDB");						//平台数据库
const TCHAR szAccountsDB[] = TEXT("LHAccountsDB");						//用户数据库
const TCHAR	szGameGoldDB[] = TEXT("LHGameGoldDB");						//财富数据库
const TCHAR	szGameScoreDB[] = TEXT("LHGameScoreDB");					//积分数据库
const TCHAR szGameMatchDB[] = TEXT("LHGameMatchDB");					//比赛数据库
const TCHAR szGameBattleDB[] = TEXT("LHGameBattleDB");					//约战数据库
const TCHAR szGameRankingDB[] = TEXT("LHGameRankingDB");				//排位数据库

//////////////////////////////////////////////////////////////////////////
//警用警告
#pragma warning(disable:4200)

//////////////////////////////////////////////////////////////////////////

//条件头文件
#include "Condition.h"				//条件宏定义

//平台头文件
#include "Macro.h"					//公共宏定义
#include "Define.h"					//数据宏定义
#include "Struct.h"					//结构体定义
#include "Packet.h"					//数据包定义
#include "Message.h"				//消息索引定义

//模板头文件
#include "Array.h"					//数组模板类
#include "Module.h"					//组件模块类
#include "Stock.h"					//仓库模板类
#include "AutoPtr.h"				//智能指针
#include "Singleton.h"				//单例模板
#include "PacketAide.h"				//数据包助手

//////////////////////////////////////////////////////////////////////////

//授权信息
const TCHAR szCompilation[] = TEXT("A89137E61817FC4DFC3CA1B46A13DA35");

//////////////////////////////////////////////////////////////////////////////////

#endif