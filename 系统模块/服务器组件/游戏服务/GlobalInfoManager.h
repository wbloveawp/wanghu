#ifndef GLOBAL_INFO_MANAGER_HEAD_FILE
#define GLOBAL_INFO_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"

/////////////////////////////////////////////////////////////////////////

//接口声明
interface IServerUserItem;

//令牌参数
struct tagTokenParameter
{
	//连接属性
	DWORD							dwTokenID;							//令牌标识
	DWORD							dwClientIP;							//连接地址	
	DWORD							dwActiveTime;						//激活时间

	//版本信息
	BYTE							cbClientKind;						//终端类型
	DWORD							dwPlazaVersion;						//广场版本
	DWORD							dwFrameVersion;						//框架版本
	DWORD							dwProcessVersion;					//游戏版本
	DWORD							dwPlazaStationID;					//广场站点

	//绑定参数
	DWORD							dwUserID;							//用户标识
	IServerUserItem	*				pIServerUserItem;					//用户指针
};

//////////////////////////////////////////////////////////////////////////

//类说明
class CGlobalServiceItem;

//数组定义
typedef CWHArray<DWORD>	CTokenIDArray;						
typedef CWHArray<tagTokenParameter*> CTokenParameterArray;
typedef CWHArray<CGlobalServiceItem *> CGlobalServiceItemArray;

//索引定义
typedef CMap<WORD,WORD,CGlobalServiceItem *,CGlobalServiceItem *> CMapServiceID;
typedef CMap<DWORD, DWORD, IServerUserItem *, IServerUserItem *> CMapServerUserItem;
typedef CMap<DWORD, DWORD, tagTokenParameter *, tagTokenParameter *> CMapTokenParamter;

//////////////////////////////////////////////////////////////////////////

//服务子项
class CGlobalServiceItem
{
	//友元定义
	friend class CGlobalInfoManager;

	//变量定义
public:
	WORD							m_wBindIndex;						//绑定索引
	tagServiceItem					m_ServiceItem;						//网关信息
	CTokenIDArray					m_TokenIDArray;						//令牌数组

	//存储变量
protected:
	CMapTokenParamter				m_MapTokenParamter;					//令牌参数	

	//链表属性
protected:
	CGlobalServiceItem *			m_pNextServiceItemPtr;				//对象指针

	//静态变量
protected:
	static CTokenParameterArray		m_TokenParameterBuffer;				//令牌参数	

	//函数定义
protected:
	//构造函数
	CGlobalServiceItem();
	//析构函数
	virtual ~CGlobalServiceItem();

	//功能函数
public:
	//绑定索引
	WORD GetBindIndex() { return m_wBindIndex; }
	//服务标识
	WORD GetServiceID() { return m_ServiceItem.wServiceID; }	
	//最大连接
	WORD GetMaxConnect() { return m_ServiceItem.wMaxConnect; }
	//令牌数量
	WORD GetTokenCount() { return m_MapTokenParamter.GetCount(); }

	//令牌管理
public:	
	//删除令牌
	VOID DeleteToken(DWORD dwTokenID);
	//查找参数
	tagTokenParameter* SearchToken(DWORD dwTokenID);
	//添加令牌
	tagTokenParameter * AppendToken(DWORD dwTokenID);	
	//枚举令牌
	tagTokenParameter * EnumToken(POSITION & Position);

	//管理函数
protected:
	//创建参数
	tagTokenParameter* ActiveTokenParameter();
	//移除参数
	VOID RemoveTokenParameter(tagTokenParameter* pTokenParameter);

	//辅助函数
protected:
	//重置数据
	VOID ResetData();	
};

//////////////////////////////////////////////////////////////////////////

//全局信息
class CGlobalInfoManager
{
	//索引变量
protected:
	CMapServiceID					m_MapServiceID[2];					//服务标识

	//存储变量
protected:
	CGlobalServiceItem *			m_pGlobalServiceItem;				//网关存储

	//函数定义
public:
	//构造函数
	CGlobalInfoManager();
	//析构函数
	virtual ~CGlobalInfoManager();

	//管理函数
public:
	//重置数据
	VOID ResetData();
	//网关数目
	DWORD GetServiceItemCount(WORD wServiceModule);

	//网关管理
public:
	//删除网关
	bool DeleteServiceItem(WORD wServiceModule,WORD wAServiceID);
	//激活网关
	CGlobalServiceItem * ActiveServiceItem(WORD wBindIndex, tagServiceItem & ServiceItem);	

	//查找函数
public:
	//寻找网关
	CGlobalServiceItem * SearchServiceItem(WORD wServiceModule, WORD wServiceID);

	//枚举函数
public:
	//枚举网关
	CGlobalServiceItem * EnumServiceItem(WORD wServiceModule,POSITION & Position);

	//创建函数
private:
	//创建网关
	CGlobalServiceItem * CreateGlobalServiceItem();

	//释放函数
private:
	//释放网关
	bool FreeGlobalServiceItem(CGlobalServiceItem * pGlobalServiceItem);

	//辅助函数
protected:
	//服务索引
	WORD GetServiceIndex(WORD wServiceModule);
};

//////////////////////////////////////////////////////////////////////////

#endif