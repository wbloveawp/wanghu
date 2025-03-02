#ifndef GLOBAL_USER_MANAGER_HEAD_FILE
#define GLOBAL_USER_MANAGER_HEAD_FILE

#pragma once

#include "ClientServiceHead.h"

//////////////////////////////////////////////////////////////////////////
//结构定义

//附带信息
struct tagExtraItem
{
	tagDataDescribe					DataDescribe;						//信息头
	BYTE							*pBuffer;							//信息体
};

//信息数组
typedef CWHArray<tagExtraItem *>	CExtraItemPtrArray;				//信息数组

//////////////////////////////////////////////////////////////////////////

//用户信息
class CGlobalUserItem : public IGlobalUserItem
{
	//友元定义
	friend class CGlobalUserManager;

	//属性变量
protected:
	tagGlobalUserInfo				m_GlobalUserInfo;					//用户信息

	//附带信息
protected:
	CExtraItemPtrArray				m_ExtraItemPtrArry;					//信息数组
	static CExtraItemPtrArray		m_ExtraItemStorge;					//信息库存

	//函数定义
protected:
	//构造函数
	CGlobalUserItem();
public:
	//析构函数
	virtual ~CGlobalUserItem();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//属性信息
public:
	//用户标识
	virtual DWORD __cdecl GetUserID() { return m_GlobalUserInfo.dwUserID; }
	//游戏标识
	virtual DWORD __cdecl GetGameID() { return m_GlobalUserInfo.dwGameID; }
	//用户名字
	virtual LPCWSTR __cdecl GetAccounts() { return m_GlobalUserInfo.szNickName; }

	//其他信息
public:
	//额外属性
	virtual VOID * __cdecl GetExtraInfo(WORD &wDataSize,WORD wDataType);

	//设置函数
public:
	//添加信息
	virtual bool __cdecl AddExtraInfo(VOID * pData, WORD wDataSize, WORD wDataType);
	//删除信息
	virtual bool __cdecl RemoveExtraInfo(WORD wDataType);
	//重置用户
	virtual bool __cdecl ResetUserItem();
};

//////////////////////////////////////////////////////////////////////////

//用户数组
typedef CWHArray<CGlobalUserItem *> CGlobalUserItemArray;

//回调数据
typedef CWHArray<IGlobalUserManagerSink *> CGlobalUserManagerSinkArray;

//////////////////////////////////////////////////////////////////////////

//用户管理
class CGlobalUserManager : public IGlobalUserManager
{
	//变量定义
protected:
	CGlobalUserItemArray			m_UserItemActive;					//活动数组
	static CGlobalUserItemArray		m_UserItemStorage;					//存储数组

	//组件接口
protected:
	CGlobalUserManagerSinkArray		m_UserManagerSinkArry;				//通知接口

	//函数定义
public:
	//构造函数
	CGlobalUserManager();
	//析构函数
	virtual ~CGlobalUserManager();

	//基础接口
public:
	//释放对象
	virtual VOID __cdecl Release() { delete this; }
	//接口查询
	virtual VOID * __cdecl QueryInterface(REFGUID Guid, DWORD dwQueryVer);
	
	//配置接口
public:
	//设置接口
	virtual bool __cdecl AddUserManagerSink(IUnknownEx * pIUnknownEx);
	//删除接口
	virtual bool __cdecl RemoveUserManagerSink(IUnknownEx *pIUnkownEx);
	//删除接口
	virtual bool __cdecl RemoveAllUserManagerSink();

	//管理接口
public:
	//增加用户
	virtual IGlobalUserItem * __cdecl ActiveUserItem(const tagGlobalUserInfo & GlobalUserInfo);
	//删除用户
	virtual bool __cdecl DeleteUserItem(IGlobalUserItem * pIGlobalUserItem);

	//更新接口
public:
	//更新基本信息
	virtual bool __cdecl UpdateUserItem(IGlobalUserItem * pIGlobalUserItem, tagGlobalUserInfo * const pGlobalUserInfo);
	//添加附加信息
	virtual bool __cdecl AddUserExtraInfo(IGlobalUserItem * pIGlobalUserItem, VOID * pData, WORD wDataSize, WORD wDataType);
	//删除附加信息
	virtual bool __cdecl RemoveUserExtraInfo(IGlobalUserItem * pIGlobalUserItem, WORD wDataType);

	//查找接口
public:
	//枚举用户
	virtual IGlobalUserItem * __cdecl EnumUserItem(WORD wEnumIndex);
	//查找用户
	virtual IGlobalUserItem * __cdecl SearchUserByUserID(DWORD dwUserID);
	//查找用户
	virtual IGlobalUserItem * __cdecl SearchUserByGameID(DWORD dwGameID);
	//查找用户
	virtual IGlobalUserItem * __cdecl SearchUserByAccounts(LPCTSTR pszAccounts);
};

//////////////////////////////////////////////////////////////////////////

#endif