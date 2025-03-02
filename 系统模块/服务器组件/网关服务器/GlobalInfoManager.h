#ifndef GLOBAL_INFO_MANAGER_HEAD_FILE
#define GLOBAL_INFO_MANAGER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//类说明
class CGlobalServiceInfo;
class CGlobalServiceKind;
class CGlobalInfoManager;

//类型定义
typedef CWHArray<CGlobalServiceInfo*> CArrayServiceInfo;
typedef CWHArray<CTCPSocketServiceHelper*> CSocketServiceArray;
typedef CMap<DWORD, DWORD, tagRouteItem, tagRouteItem> CMapRouteItem;

//类型定义
typedef CWHStockData<WORD, CGlobalServiceInfo> CStockServiceInfo;
typedef CWHStockData<WORD, CGlobalServiceKind> CStockServiceKind;
typedef CWHStockData<DWORD, CGlobalServiceInfo> CStockServiceRoute;

//////////////////////////////////////////////////////////////////////////
//路由子项

//////////////////////////////////////////////////////////////////////////

//服务子项
class CGlobalServiceInfo : public CGlobalServiceItem
{
	friend class CGlobalServiceKind;
	friend class CGlobalInfoManager;

	//变量定义
public:
	ITCPSocketService *				m_pITCPSocketService;				//网络接口	

	//路由管理
protected:
	CMapRouteItem					m_MapRouteItem;						//路由映射

	//拓展属性
protected:	
	WORD							m_wDataSize;						//数据大小
	BYTE							m_cbExtendData[512];				//扩展数据

	//函数定义
protected:
	//构造函数
	CGlobalServiceInfo();
	//析构函数
	virtual ~CGlobalServiceInfo();

	//信息函数
public:
	//扩展数据
	BYTE* GetExtendData() { return m_cbExtendData; }

	//路由管理
public:
	//移除路由
	VOID RemoveRouteItem();
	//移除路由
	VOID RemoveRouteItem(DWORD dwRouteID);
	//添加路由
	VOID AppendRouteItem(tagRouteItem RouteItem[],WORD wItemCount);	

	//重置数据
public:
	//重置数据
	virtual VOID ResetData();
};


//////////////////////////////////////////////////////////////////////////
//服务类型
class CGlobalServiceKind
{
	friend class CGlobalInfoManager;

	//变量定义
protected:
	WORD							m_wServiceKind;						//服务类型
	WORD							m_wServiceMapKind;					//映射方式
	CWHConHash						m_ServiceHash;						//哈希映射
	CArrayServiceInfo				m_ArrayServiceInfo;					//服务列表	

	//函数定义
protected:
	//构造函数
	CGlobalServiceKind();
	//析构函数
	virtual ~CGlobalServiceKind();

	//信息函数
public:
	//服务类型
	WORD GetServiceKind() { return m_wServiceKind; }
	//服务数量
	int GetServiceCount() { return m_ServiceHash.GetNodeCount(); }

	//服务管理
public:
	//查找服务
	WORD LookupService(DWORD dwObjectID);
	//移除服务
	VOID RemoveService(CGlobalServiceInfo* pGlobalServiceInfo);
	//添加服务
	VOID AppendService(CGlobalServiceInfo* pGlobalServiceInfo);	

	//辅助函数
public:
	//重置数据
	VOID ResetData();
	//查找服务
	INT SearchService(WORD wServiceID);
};

//////////////////////////////////////////////////////////////////////////

//全局信息
class CGlobalInfoManager
{
	//配置函数
protected:
	IUnknownEx*						m_pIAttemperEngine;					//调度接口
	tagServiceAttrib*				m_pServiceAttribute;				//服务属性

	//索引变量
protected:	
	CStockServiceInfo				m_StockServiceInfo[SERVICE_COUNT];	//服务标识	
	CStockServiceKind				m_StockServiceKind[SERVICE_COUNT];	//散列类型	

	//网络组件
protected:
	CSocketServiceArray				m_SocketServiceActive;				//服务存储
	CSocketServiceArray				m_SocketServiceBuffer;				//服务缓冲

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
	//服务数目
	DWORD GetServiceCount(WORD wServiceIndex);

	//配置函数
public:
	//回调接口
	VOID SetIAttemperEngine(IUnknownEx* pIAttemperEngine) { m_pIAttemperEngine = pIAttemperEngine; }	
	//服务属性
	VOID SetServiceAttribute(tagServiceAttrib* pServiceAttribute) { m_pServiceAttribute = pServiceAttribute; }

	//服务管理
public:
	//激活服务
	CGlobalServiceInfo * ActiveServiceInfo(WORD wServiceIndex, DWORD dwServiceID, tagServiceItem & ServiceItem);
	//删除服务
	bool DeleteServiceItem(WORD wServiceIndex, WORD wServiceID);

	//查找函数
public:
	//寻找服务
	CGlobalServiceInfo * SearchServiceInfo(WORD wServiceIndex,WORD wServiceID);
	//服务类型
	CGlobalServiceKind * SearchServiceKind(WORD wServiceIndex, WORD wServiceKind);
	//映射服务
	CGlobalServiceInfo * MapServiceInfo(WORD wServiceIndex, WORD wServiceKind, DWORD dwObjectID);

	//枚举函数
public:
	//枚举服务
	CGlobalServiceInfo * EnumServiceInfo(WORD wServiceIndex, CGlobalServiceInfo* pGlobalServiceItem);

	//网络信息
public:
	//网络信息
	ITCPSocketService * GetSocketService(WORD wServiceIndex,WORD wServiceID);

	//辅助函数
private:
	//激活服务
	ITCPSocketService* ActiveSocketService(DWORD dwServiceID);
	//移除服务
	bool RemoveSocketService(ITCPSocketService* pITCPSocketService);
};

//////////////////////////////////////////////////////////////////////////

#endif