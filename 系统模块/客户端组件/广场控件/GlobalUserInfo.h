#ifndef GLOBAL_USER_INFO_HEAD_FILE
#define GLOBAL_USER_INFO_HEAD_FILE

#pragma once

#include "PlazaControlsHead.h"

//////////////////////////////////////////////////////////////////////////
//结构定义

//用户信息
struct tagGlobalUserData
{
	//基本资料
	WORD							wFaceID;							//头像索引
	DWORD							dwUserID;							//用户 I D
	DWORD							dwGameID;							//游戏 I D
	DWORD							dwStationID;						//站点标识
	DWORD							dwExperience;						//用户经验
	DWORD							dwLoveLiness;						//用户魅力
	TCHAR							szAccounts[LEN_ACCOUNTS];			//登录帐号
	TCHAR							szPassword[LEN_PASSWORD];			//登录密码

	//扩展资料
	BYTE							cbAge;								//用户年龄
	BYTE							cbGender;							//用户性别
	TCHAR							szUnderWrite[LEN_UNDER_WRITE];		//个性签名

	//地区资料
	TCHAR							szArea[LEN_AREA];					//地区信息
	TCHAR							szCity[LEN_CITY];					//城市信息
	TCHAR							szProvince[LEN_PROVINCE];			//省份信息

	//社团资料
	DWORD							dwGroupID;							//社团索引
	TCHAR							szGroupName[LEN_GROUP_NAME];		//社团名字

	//会员资料
	BYTE							cbMemberOrder;						//会员等级
	SYSTEMTIME						MemberOverDate;						//到期时间
};

//扩展资料
struct tagIndividualUserData
{
	//用户信息
	DWORD							dwUserID;							//用户 I D
	TCHAR							szUserNote[LEN_USER_NOTE];			//用户说明
	TCHAR							szCompellation[LEN_COMPELLATION];	//真实名字

	//电话号码
	TCHAR							szSeatPhone[LEN_SEAT_PHONE];		//固定电话
	TCHAR							szMobilePhone[LEN_MOBILE_PHONE];	//移动电话

	//联系资料
	TCHAR							szQQ[LEN_QQ];						//Q Q 号码
	TCHAR							szEMail[LEN_EMAIL];					//电子邮件
	TCHAR							szDwellingPlace[LEN_DWELLING_PLACE];//联系地址
};

//////////////////////////////////////////////////////////////////////////

//用户信息
class PLAZA_CONTROL_CLASS CGlobalUserInfo
{
	//用户信息
protected:
	tagGlobalUserData				m_GlobalUserData;						//用户资料
	tagIndividualUserData			m_IndividualUserData;					//扩展资料

	//静态变量
protected:
	static CGlobalUserInfo *		m_pGlobalUserInfo;						//用户信息

	//函数定义
public:
	//构造函数
	CGlobalUserInfo();
	//析构函数
	virtual ~CGlobalUserInfo();

	//功能函数
public:
	//重置资料
	VOID ResetUserInfoData();
	//用户资料
	tagGlobalUserData * GetGlobalUserData() { return &m_GlobalUserData; }
	//扩展资料
	tagIndividualUserData * GetIndividualUserData() { return &m_IndividualUserData; }

	//静态函数
public:
	//获取对象
	static CGlobalUserInfo * GetInstance() { return m_pGlobalUserInfo; }
};

//////////////////////////////////////////////////////////////////////////

#endif