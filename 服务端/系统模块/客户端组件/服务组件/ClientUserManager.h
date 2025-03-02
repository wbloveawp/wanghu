#ifndef CLIENT_USER_MANAGER_HEAD_FILE
#define CLIENT_USER_MANAGER_HEAD_FILE

#pragma once

#include "ClientServiceHead.h"

//////////////////////////////////////////////////////////////////////////

//用户信息
class CClientUserItem : public IClientUserItem
{
	//友元定义
	friend class CGameUserManager;
	friend class CPlazaUserManager;

	//属性变量
protected:
	tagClientUserInfo				m_UserInfo;							//用户信息
	tagCustomFaceInfo				m_CustomFaceInfo;					//自定头像

	//扩展属性
protected:
	BYTE							m_cbCompanion;						//用户关系
	WCHAR							m_szUserNote[LEN_NOTE];				//用户备注

	//函数定义
public:
	//构造函数
	CClientUserItem();
	//析构函数
	virtual ~CClientUserItem();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//属性接口
public:
	//重要等级
	virtual LONG __cdecl GetImportOrder();
	//用户信息
	virtual tagClientUserInfo* __cdecl GetUserInfo() { return &m_UserInfo; }
	//自定头像
	virtual tagCustomFaceInfo * GetCustomFaceInfo() { return &m_CustomFaceInfo; }

	//头像信息
public:
	//头像索引
	virtual WORD __cdecl GetFaceID(){ return m_UserInfo.wFaceID; }
	//自定索引
	virtual DWORD __cdecl GetCustomID() { return m_UserInfo.dwCustomID; }
		
	//属性信息
public:
	//机器标示
	virtual bool __cdecl IsRobot() { return m_UserInfo.bIsRobot; }
	//用户年龄
	virtual BYTE __cdecl GetAge() { return m_UserInfo.cbAge; }
	//用户性别
	virtual BYTE __cdecl GetGender() { return m_UserInfo.cbGender; }
	//用户标识
	virtual DWORD __cdecl GetUserID() { return m_UserInfo.dwUserID; }
	//游戏标识
	virtual DWORD __cdecl GetGameID() { return m_UserInfo.dwGameID; }
	//社团标识
	virtual DWORD __cdecl GetGroupID() { return 0; }
	//用户名字
	virtual LPCWSTR __cdecl GetAccounts() { return m_UserInfo.szAccounts; }
	//个性签名
	virtual LPCWSTR __cdecl GetUnderWrite() { return m_UserInfo.szUnderWrite; }

	//等级信息
public:
	//会员等级
	virtual BYTE __cdecl GetMemberOrder() { return m_UserInfo.dwMemberPoint; }
	//管理等级
	virtual BYTE __cdecl GetMasterOrder() { return m_UserInfo.cbMasterOrder; }

	//地区信息
public:
	//区域信息
	virtual LPCWSTR __cdecl GetUserArea() { return m_UserInfo.szArea; }
	//城市信息
	virtual LPCWSTR __cdecl GetUserCity() { return m_UserInfo.szCity; }
	//省份信息
	virtual LPCWSTR __cdecl GetUserProvince() { return m_UserInfo.szProvince; }

	//用户状态
public:
	//用户桌子
	virtual WORD __cdecl GetTableID() { return m_UserInfo.wTableID; }
	//用户椅子
	virtual WORD __cdecl GetChairID() { return m_UserInfo.wChairID; }
	//用户状态
	virtual BYTE __cdecl GetUserStatus() { return m_UserInfo.cbUserStatus; }

	//经验等级
public:
	//经验数值
	virtual LONG __cdecl GetUserExperience();
	//获取等级
	virtual WORD __cdecl GetExperienceLevel();
	//等级信息
	virtual WORD __cdecl GetExperienceIndex(BYTE cbOrderIndex[], WORD wMaxCount);

	//积分信息
public:
	//积分数值
	virtual SCORE __cdecl GetUserScore() { return m_UserInfo.lScore; }
	//金币信息
	virtual SCORE __cdecl GetUserGold() { return m_UserInfo.lGold; }
	//银行信息
	virtual SCORE __cdecl GetInsureGold() { return m_UserInfo.lInsureGold; }

	//游戏信息
public:
	//胜利盘数
	virtual LONG __cdecl GetUserWinCount() { return m_UserInfo.lWinCount; }
	//失败盘数
	virtual LONG __cdecl GetUserLostCount() { return m_UserInfo.lLostCount; }
	//和局盘数
	virtual LONG __cdecl GetUserDrawCount() { return m_UserInfo.lDrawCount; }
	//逃跑盘数
	virtual LONG __cdecl GetUserFleeCount() { return m_UserInfo.lFleeCount; }
	//游戏局数
	virtual LONG __cdecl GetUserPlayCount() { return m_UserInfo.lWinCount+m_UserInfo.lLostCount+m_UserInfo.lDrawCount+m_UserInfo.lFleeCount; }

	//比率信息
public:
	//用户胜率
	virtual float __cdecl GetUserWinRate();
	//用户输率
	virtual float __cdecl GetUserLostRate();
	//用户和率
	virtual float __cdecl GetUserDrawRate();
	//用户逃率
	virtual float __cdecl GetUserFleeRate();

	//用户关系
public:
	//获取关系
	virtual BYTE __cdecl GetUserCompanion() { return m_cbCompanion; }
	//设置关系
	virtual VOID __cdecl SetUserCompanion(BYTE cbCompanion) { m_cbCompanion=cbCompanion; }

	//用户备注
public:
	//获取备注
	virtual LPCWSTR __cdecl GetUserNoteInfo() { return m_szUserNote; }
	//设置备注
	virtual VOID __cdecl SetUserNoteInfo(LPCWSTR pszUserNote);
};

//////////////////////////////////////////////////////////////////////////

//用户数组
typedef CWHArray<CClientUserItem *> CClientUserItemArray;

//////////////////////////////////////////////////////////////////////////

//用户管理
class CPlazaUserManager : public IPlazaUserManager
{
	//变量定义
protected:
	CClientUserItemArray			m_UserItemActive;					//活动数组
	static CClientUserItemArray		m_UserItemStorage;					//存储数组

	//组件接口
protected:
	IUserManagerSink *				m_pIUserManagerSink;				//通知接口
	IUserInformation *				m_pIUserInformation;				//用户信息

	//函数定义
public:
	//构造函数
	CPlazaUserManager();
	//析构函数
	virtual ~CPlazaUserManager();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//配置接口
public:
	//设置接口
	virtual bool __cdecl SetUserInformation(IUnknownEx * pIUnknownEx);
	//设置接口
	virtual bool __cdecl SetUserManagerSink(IUnknownEx * pIUnknownEx);

	//管理接口
public:
	//增加用户
	virtual IClientUserItem * __cdecl ActiveUserItem(const tagClientUserInfo& UserInfo, tagCustomFaceInfo & CustomFaceInfo);
	//删除用户
	virtual bool __cdecl DeleteUserItem(IClientUserItem * pIClientUserItem);

	//更新接口
public:
	//更新积分
	virtual bool __cdecl UpdateUserItemScore(IClientUserItem * pIClientUserItem, tagClientUserScore * const pUserScore);
	//更新状态
	virtual bool __cdecl UpdateUserItemStatus(IClientUserItem * pIClientUserItem, tagUserStatus * const pUserStatus);
	//更新段位
	//virtual bool __cdecl UpdateUserItemSegment(IClientUserItem * pIClientUserItem, tagUserSegment * const pUserSegment);
	//更新头像
	virtual bool __cdecl UpdateUserCustomFace(IClientUserItem * pIClientUserItem, DWORD dwCustomID, tagCustomFaceInfo & CustomFaceInfo);

	//查找接口
public:
	//枚举用户
	virtual IClientUserItem * __cdecl EnumUserItem(WORD wEnumIndex);
	//查找用户
	virtual IClientUserItem * __cdecl SearchUserByUserID(DWORD dwUserID);
	//查找用户
	virtual IClientUserItem * __cdecl SearchUserByGameID(DWORD dwGameID);
	//查找用户
	virtual IClientUserItem * __cdecl SearchUserByAccounts(LPCTSTR pszAccounts);
};

//////////////////////////////////////////////////////////////////////////

//用户管理
class CGameUserManager : public IGameUserManager
{
	//用户变量
protected:
	CClientUserItemArray			m_UserItemLookon;					//旁观数组
	CClientUserItemArray			m_UserItemStorage;					//存储数组

	//变量定义
protected:
	CClientUserItem *				m_pTableUserItem[MAX_CHAIR];		//游戏用户

	//组件接口
protected:
	IUserManagerSink *				m_pIUserManagerSink;				//通知接口

	//函数定义
public:
	//构造函数
	CGameUserManager();
	//析构函数
	virtual ~CGameUserManager();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//配置接口
public:
	//设置接口
	virtual bool __cdecl SetUserManagerSink(IUnknownEx * pIUnknownEx);
	//获取接口
	virtual VOID * __cdecl GetUserManagerSink(REFGUID Guid, DWORD dwQueryVer);

	//管理接口
public:
	//增加用户
	virtual IClientUserItem * __cdecl ActiveUserItem(const tagClientUserInfo& UserInfo, tagCustomFaceInfo & CustomFaceInfo, BYTE cbCompanion, LPCWSTR pszUserNote);
	//删除用户
	virtual bool __cdecl DeleteUserItem(IClientUserItem * pIClientUserItem);
	//重置用户
	virtual bool __cdecl DeleteAllUserItem();

	//更新接口
public:
	//更新积分
	virtual bool __cdecl UpdateUserItemScore(IClientUserItem * pIClientUserItem, tagClientUserScore* const pUserScore);
	//更新状态
	virtual bool __cdecl UpdateUserItemStatus(IClientUserItem * pIClientUserItem, tagUserStatus * const pUserStatus);
	//更新段位
	//virtual bool __cdecl UpdateUserItemSegment(IClientUserItem * pIClientUserItem, tagUserSegment * const pUserSegment);
	//更新头像
	virtual bool __cdecl UpdateUserCustomFace(IClientUserItem * pIClientUserItem, DWORD dwCustomID, tagCustomFaceInfo & CustomFaceInfo);

	//查询接口
public:
	//游戏用户
	virtual IClientUserItem * __cdecl GetTableUserItem(WORD wChariID);
	//旁观用户
	virtual IClientUserItem * __cdecl EnumLookonUserItem(WORD wEnumIndex);

	//查找接口
public:
	//查找用户
	virtual IClientUserItem * __cdecl SearchUserByUserID(DWORD dwUserID);
	//查找用户
	virtual IClientUserItem * __cdecl SearchUserByGameID(DWORD dwGameID);
	//查找用户
	virtual IClientUserItem * __cdecl SearchUserByAccounts(LPCTSTR pszAccounts);
};

//////////////////////////////////////////////////////////////////////////

#endif