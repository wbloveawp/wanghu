#ifndef GLOBAL_INFO_ITEM_HEAD_FILE
#define GLOBAL_INFO_ITEM_HEAD_FILE

#pragma once

#include "ServiceFrameHead.h"

//////////////////////////////////////////////////////////////////////////
// 
//类说明
class CGlobalUserItem;
class CGlobalGateItem;
class CGlobalServerItem;
class CGlobalServiceItem;

//////////////////////////////////////////////////////////////////////////

//网络信息
struct tagUserSocketInfo
{
	DWORD							dwUserID;							//用户标识	
	WORD							wGateID;							//网关标识
	DWORD							dwTokenID;							//令牌标识
};


//令牌参数
struct tagTokenParameter
{
	//连接属性
	DWORD							dwTokenID;							//令牌标识
	DWORD							dwClientIP;							//连接地址	
	DWORD							dwActiveTime;						//激活时间	

	//用户信息
	DWORD							dwUserID;							//用户标识	
	LPVOID							pBindUserItem;						//绑定子项

	//扩展数据
	BYTE							cbExtendData[128];					//扩展数据
};

//////////////////////////////////////////////////////////////////////////

//类型定义
typedef CWHStockData<DWORD, tagTokenParameter> CStockTokenID;
typedef CWHStockData<WORD, CGlobalGateItem> CStockGateItem;
typedef CWHStockData<DWORD, CGlobalUserItem> CStockUserItem;
typedef CWHStockData<WORD, CGlobalServerItem> CStockServerItem;
typedef CWHStockData<WORD, CGlobalServiceItem> CStockServiceItem;

//////////////////////////////////////////////////////////////////////////

//用户子项
class SERVICE_FRAME_CLASS CGlobalUserItem
{
	//网络属性
public:	
	WORD							m_wGateID;							//网关标识
	DWORD							m_dwUserID;							//用户标识		
	DWORD							m_dwTokenID;						//令牌标识	

	//用户状态
public:
	BYTE							m_cbIMUserStatus;					//用户状态
	BYTE							m_cbCBUserStatus;					//用户状态

	//用户属性
public:
	BYTE							m_cbGender;							//用户性别
	DWORD							m_dwGameID;							//游戏标识
	TCHAR							m_szNickName[LEN_NICKNAME];			//登录昵称
	TCHAR							m_szUserHeadUrl[256];			//玩家昵称	kk jia

	//头像信息
public:
	WORD							m_wFaceID;							//头像标识
	DWORD							m_dwCustomID;						//头像标识

	//状态信息
public:
	BYTE							m_cbShutUpStatus;					//禁言状态
	DWORD							m_dwShutUpOverTime;					//解禁时间

	//函数定义
protected:
	//构造函数
	CGlobalUserItem();
	//析构函数
	virtual ~CGlobalUserItem();

	//网络信息
public:
	//网关标识
	WORD GetGateID() { return m_wGateID; }
	//令牌标识
	DWORD GetTokenID() { return m_dwTokenID; }	

	//用户状态
public:
	//用户状态
	BYTE GetIMUserStatus() { return m_cbIMUserStatus; }
	//用户状态
	BYTE GetCBUserStatus() { return m_cbCBUserStatus; }

	//用户信息
public:
	//用户标识
	DWORD GetUserID() { return m_dwUserID; }
	//游戏标识
	DWORD GetGameID() { return m_dwGameID; }
	//玩家性别
	BYTE GetGender() { return m_cbGender; }
	//用户帐号
	LPCTSTR GetNickName() { return m_szNickName; }

	//头像信息
public:
	//系统头像
	WORD GetFaceID() { return m_wFaceID; }
	//头像标识
	DWORD GetCustomID() { return m_dwCustomID; }

	//辅助函数
public:
	//重置数据
	virtual VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////
// 
//服务子项
class SERVICE_FRAME_CLASS CGlobalServiceItem
{
	//服务信息
public:
	DWORD							m_dwSocketID;						//网络标识
	tagServiceItem					m_ServiceItem;						//服务子项

	//状态变量
public:
	WORD							m_wOnlineCount;						//在线数量

	//函数定义
protected:
	//构造函数
	CGlobalServiceItem();
	//析构函数
	virtual ~CGlobalServiceItem();

	//功能函数
public:
	//绑定索引
	DWORD GetSocketID() { return m_dwSocketID; }
	//服务标识
	WORD GetServiceID() { return m_ServiceItem.wServiceID; }
	//服务索引
	WORD GetServiceModule() { return m_ServiceItem.wServiceModule; }

	//辅助函数
public:
	//重置数据
	virtual VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

//网关子项
class SERVICE_FRAME_CLASS CGlobalGateItem : public CGlobalServiceItem
{
	//令牌容器
public:
	CDWordArray						m_TokenIDArray;						//令牌数组	
	CStockTokenID					m_StockTokenID;						//令牌参数

	//函数定义
protected:
	//构造函数
	CGlobalGateItem();
	//析构函数
	virtual ~CGlobalGateItem();

	//功能函数
public:
	//令牌数量
	WORD GetTokenCount() { return (WORD)m_StockTokenID.GetItemCount(); }

	//管理函数
public:
	//移除令牌
	VOID RemoveTokenID();
	//移除令牌
	VOID RemoveTokenID(DWORD dwTokenID);
	//激活令牌
	tagTokenParameter* ActiveTokenID(DWORD dwTokenID);

	//查找函数
public:
	//查询令牌
	tagTokenParameter* SearchTokenID(DWORD dwTokenID);

	//枚举函数
public:
	//查询令牌
	tagTokenParameter* EnumTokenID(tagTokenParameter* pTokenParameter);

	//功能函数
public:
	//重置数据
	VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

//房间子项
class SERVICE_FRAME_CLASS CGlobalServerItem
{
	//变量定义
public:
	DWORD							m_dwSocketID;						//网络标识
	tagGameServer					m_GameServer;						//游戏房间

	//函数定义
protected:
	//构造函数
	CGlobalServerItem();
	//析构函数
	virtual ~CGlobalServerItem();

	//信息函数
public:
	//网络标识
	DWORD GetSocketID() { return m_dwSocketID; }
	//房间标识
	WORD GetServerID() { return m_GameServer.wServerID; }	

	//辅助函数
public:
	//重置数据
	virtual VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

#endif