#ifndef CLUB_USER_MANAGER_HEAD_FILE
#define CLUB_USER_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"

//////////////////////////////////////////////////////////////////////////////////
//类型声明
class CClubUserItem;
class CClubUserManager;

//////////////////////////////////////////////////////////////////////////////////
// 
//类型定义
typedef CWHStockData<DWORD,CClubUserItem>  CStcokClubUser;

//////////////////////////////////////////////////////////////////////////////////
//用户信息
class CClubUserItem
{
	//友元定义
	friend class CClubUserManager;	

	//用户信息
protected:
	tagClubUserInfo					m_UserInfo;							//用户信息

	//网络信息
protected:
	tagSocketInfo					m_SocketInfo;						//网络信息

	//函数定义
protected:
	//构造函数
	CClubUserItem();
	//析构函数
	virtual ~CClubUserItem();

	//用户状态
public:	
	//获取状态
	virtual BYTE GetUserStatus() { return m_UserInfo.cbUserStatus; }
	//修改状态
	virtual VOID SetUserStatus(BYTE cbUserStatus) { m_UserInfo.cbUserStatus=cbUserStatus; }

	//属性信息
public:
	//用户性别
	virtual BYTE GetGender() { return m_UserInfo.cbGender; }
	//用户标识
	virtual DWORD GetUserID() { return m_UserInfo.dwUserID; }
	//游戏标识
	virtual DWORD GetGameID() { return m_UserInfo.dwGameID; }
	//用户昵称
	virtual LPCTSTR GetNickName() { return m_UserInfo.szNickName; }
	//用户信息
	virtual tagClubUserInfo * GetUserInfo() { return &m_UserInfo; }

	//网络信息
public:
	//令牌标识
	virtual DWORD GetTokenID() { return m_SocketInfo.dwTokenID; }
	//网关标识
	virtual WORD GetGateID() { return m_SocketInfo.wGateID; }	
	//获取信息
	virtual tagSocketInfo * GetSocketInfo() { return &m_SocketInfo; }
	//设置信息
	virtual VOID SetSocketInfo(tagSocketInfo & SocketInfo) { m_SocketInfo=SocketInfo; }

	//辅助变量
protected:
	//重置数据
	virtual VOID ResetUserItem();
};

//////////////////////////////////////////////////////////////////////////////////

//用户管理类
class CClubUserManager
{
	//用户变量
protected:
	CStcokClubUser					m_StockClubUser;					//用户仓库

	//函数定义
public:
	//构造函数
	CClubUserManager();
	//析构函数
	virtual ~CClubUserManager();

	//查找函数
public:
	//查找用户
	CClubUserItem * SearchUserItem(DWORD dwUserID);
	//枚举用户
	CClubUserItem* EnumUserItem(CClubUserItem* pClubUserItem);

	//统计函数
public:
	//在线人数
	DWORD GetUserItemCount() { return (DWORD)m_StockClubUser.GetItemCount(); }

	//管理函数
public:
	//删除用户
	VOID DeleteUserItem();
	//删除用户
	bool DeleteUserItem(DWORD dwUserID);
	//插入用户
	CClubUserItem * InsertUserItem(tagClubUserInfo & UserInfo, tagSocketInfo &SocketInfo);	
};

//////////////////////////////////////////////////////////////////////////////////
#endif