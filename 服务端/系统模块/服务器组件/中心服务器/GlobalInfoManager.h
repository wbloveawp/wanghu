#ifndef GLOBAL_USER_MANAGER_HEAD_FILE
#define GLOBAL_USER_MANAGER_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////

//类说明
class CGlobalUserInfo;
class CGlobalLogonInfo;
class CGlobalAgentInfo;
class CGlobalServerInfo;
class CGlobalMonitorInfo;
class CGlobalServiceInfo;
class CGlobalServiceKind;
class CGlobalGameStock;

//类型定义
typedef CWHArray<CGlobalServiceInfo*> CArrayServiceInfo;
typedef CWHStockData<WORD, CGlobalGameStock> CStockGameStock;

//索引定义
typedef CMap<DWORD, DWORD, tagRouteItem, tagRouteItem> CMapRouteItem;
typedef CMap<DWORD, DWORD, CGlobalUserInfo*, CGlobalUserInfo*> CMapUserID;
typedef CMap<WORD, WORD, CGlobalLogonInfo*, CGlobalLogonInfo*> CMapLogonID;
typedef CMap<WORD, WORD, CGlobalServerInfo*, CGlobalServerInfo*> CMapServerID;
typedef CMap<WORD, WORD, CGlobalServiceInfo*, CGlobalServiceInfo*> CMapServiceID;
typedef CMap<WORD, WORD, CGlobalServiceKind*, CGlobalServiceKind* > CMapServiceKind;

//////////////////////////////////////////////////////////////////////////

//服务信息
class CGlobalServiceInfo : public CGlobalServiceItem
{
	friend class CGlobalServiceKind;
	friend class CGlobalInfoManager;

	//链接变量
protected:
	CGlobalServiceInfo *			m_pNextServiceInfoPtr;			//服务信息

	//路由管理
protected:
	CMapRouteItem					m_MapRouteItem;					//路由映射

	//函数定义
public:
	//构造函数
	CGlobalServiceInfo();
	//析构函数
	virtual ~CGlobalServiceInfo();

	//路由管理
public:
	//移除路由
	VOID RemoveRouteItem(DWORD dwRouteID);
	//添加路由
	VOID AppendRouteItem(tagRouteItem RouteItem[], WORD wItemCount);	

	//查找函数
public:
	//查找路由
	tagRouteItem* QueryRouteItem(DWORD dwRouteID);
	//枚举路由
	tagRouteItem* EnumRouteItem(POSITION &Position);

	//辅助函数
public:
	//重置函数
	virtual VOID ResetData();
};


//////////////////////////////////////////////////////////////////////////
//服务类型
class CGlobalServiceKind
{
	//友元定义
	friend class CGlobalInfoManager;

	//变量定义
public:
	WORD							m_wServiceKind;						//服务类型
	WORD							m_wServiceMapKind;					//映射方式
	CWHConHash						m_ServiceHash;						//服务映射
	CArrayServiceInfo				m_ArrayServiceInfo;					//服务数组

	//链表属性
protected:
	CGlobalServiceKind*				m_pNextServiceKindPtr;				//对象指针

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
	//分配服务
	WORD AssignService(DWORD dwObjectID,WORD wServiceID=INVALID_WORD);
	//移除服务
	VOID RemoveService(CGlobalServiceInfo* pGlobalServiceInfo);
	//添加服务
	VOID AppendService(CGlobalServiceInfo* pGlobalServiceInfo);	

	//辅助函数
protected:
	//重置数据
	VOID ResetData();
	//查找服务
	INT SearchService(WORD wServiceID);
};

//////////////////////////////////////////////////////////////////////////

//广场子项
class CGlobalLogonInfo : public CGlobalServiceInfo
{
	friend class CGlobalInfoManager;

	//索引变量
public:
	CMapUserID						m_MapUserID;				//用户仓库

	//函数定义
protected:
	//构造函数
	CGlobalLogonInfo();
	//析构函数
	virtual ~CGlobalLogonInfo();

	//辅助函数
public:
	//重置函数
	virtual VOID ResetData();
};


//////////////////////////////////////////////////////////////////////////

//游戏彩池
class CGlobalGameStock
{
	friend class CGlobalMonitorInfo;

	//变量定义
public:
	WORD						m_wKindID;						//类型标识
	SCORE						m_lJackpotScore;				//彩池彩金

	//函数定义
protected:
	//构造函数
	CGlobalGameStock();
	//析构函数
	virtual ~CGlobalGameStock();

	//辅助函数
public:
	//重置函数
	virtual VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

//监控子项
class CGlobalMonitorInfo : public CGlobalServiceInfo
{
	friend class CGlobalInfoManager;

	//存储变量
public:
	CStockGameStock					m_StockGameStock;				//库存仓库

	//函数定义
protected:
	//构造函数
	CGlobalMonitorInfo();
	//析构函数
	virtual ~CGlobalMonitorInfo();

	//辅助函数
public:
	//重置函数
	virtual VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

//用户信息
class CGlobalUserInfo : public CGlobalUserItem
{
	friend class CGlobalInfoManager;

	//服务信息
public:
	CGlobalLogonInfo *				m_pGlobalLogonInfo;					//登录子项
	CGlobalServerInfo *				m_pGlobalServerInfo;				//房间子项	
	CGlobalServiceInfo *			m_pGlobalMessageInfo;				//消息子项

	//链表属性
protected:
	CGlobalUserInfo*				m_pNextUserInfoPtr;					//对象指针

	//函数定义
protected:
	//构造函数
	CGlobalUserInfo();
	//析构函数
	virtual ~CGlobalUserInfo();

	//信息函数
public:
	//服务数量
	WORD BindServiceCount();

	//服务信息
public:	
	//登录服务
	CGlobalLogonInfo* GetBindLogonInfo() { return m_pGlobalLogonInfo; }
	//房间服务
	CGlobalServerInfo* GetBindServerInfo() { return m_pGlobalServerInfo; }
	//消息服务
	CGlobalServiceInfo* GetBindMessageInfo() { return m_pGlobalMessageInfo; }

	//登录绑定
public:
	//绑定登录
	VOID BindLogonInfo(CGlobalLogonInfo* pGlobalLogonInfo);
	//绑定登录
	VOID UnBindLogonInfo(CGlobalLogonInfo* pGlobalLogonInfo);	

	//房间绑定
public:
	//绑定房间
	VOID BindServerInfo(CGlobalServerInfo* pGlobalServerInfo);
	//解绑房间
	VOID UnBindServerInfo(CGlobalServerInfo* pGlobalServerInfo);

	//消息绑定
public:
	//绑定消息
	VOID BindMessageInfo(CGlobalServiceInfo* pGlobalMessageInfo);
	//解绑消息
	VOID UnBindMessageInfo(CGlobalServiceInfo* pGlobalMessageInfo);

	//辅助函数
public:
	//重置数据
	virtual VOID ResetData();
};
//////////////////////////////////////////////////////////////////////////

//房间子项
class CGlobalServerInfo : public CGlobalServiceInfo
{
	friend class CGlobalInfoManager;

	//变量定义
public:
	tagGameServer					m_GameServer;						//游戏房间

	//索引变量
public:
	CMapUserID						m_MapUserID;						//用户仓库

	//函数定义
protected:
	//构造函数
	CGlobalServerInfo();
	//析构函数
	virtual ~CGlobalServerInfo();

	//功能函数
public:
	//用户数目
	DWORD GetUserItemCount() { return (DWORD)m_MapUserID.GetCount(); }

	//查找函数
public:
	//寻找用户
	CGlobalUserInfo * SearchUserInfo(DWORD dwUserID);

	//辅助函数
protected:
	//重置数据
	virtual VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

//类型定义
typedef CWHArray<tagPortPair> CPortPairArray;

//代理项目
class CGlobalAgentInfo : public CGlobalServiceInfo
{
	friend class CGlobalInfoManager;

	//变量定义
public:
	tagAgentItem					m_AgentItem;						//代理子项
	CPortPairArray					m_PortPairArray;					//端口数组

	//链表属性
protected:
	CGlobalAgentInfo*				m_pNextAgentItemPtr;				//对象指针

	//函数定义
protected:
	//构造函数
	CGlobalAgentInfo();
	//析构函数
	virtual ~CGlobalAgentInfo();

	//功能函数
public:
	//端口数量
	WORD GetServicePortCount() { return (WORD)m_PortPairArray.GetCount(); }

	//功能函数
public:
	//删除端口
	bool RemoveServicePort(WORD wServicePort);
	//插入网关
	bool InsertServicePort(tagPortPair * pPortPair);

	//辅助函数
public:
	//重置数据
	VOID ResetData();
};

//////////////////////////////////////////////////////////////////////////

//全局信息
class CGlobalInfoManager
{
	//索引变量
protected:
	CMapUserID						m_MapUserID;						//用户标识
	CMapServiceID					m_MapServiceID[SERVICE_COUNT];		//服务标识
	CMapServiceKind					m_MapServiceKind[SERVICE_COUNT];	//服务类型

	//存储变量
protected:
	CGlobalUserInfo*				m_pGlobalUserInfo;					//用户存储
	CGlobalServiceKind*				m_pGlobalServiceKind;				//类型存储
	CGlobalServiceInfo*				m_pGlobalServiceInfo[SERVICE_COUNT];//服务存储

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
	DWORD GetServiceItemCount(WORD wServiceModule);	
	//用户数目
	DWORD GetUserItemCount() { return (DWORD)m_MapUserID.GetCount(); }	

	//用户管理
public:
	//激活用户
	CGlobalUserInfo* ActiveUserInfo(tagGlobalUserInfo & GlobalUserInfo);
	//删除用户
	bool DeleteUserInfo(DWORD dwUserID);

	//服务管理
public:
	//激活服务
	bool ActiveServiceInfo(DWORD dwSocketID, tagServiceItem & ServiceItem);
	//删除服务
	bool DeleteServiceInfo(WORD wServiceModule, WORD wServiceID);	

	//代理管理
public:
	//激活代理
	bool ActiveAgentInfo(DWORD dwSocketID, tagServiceItem & ServiceItem, tagAgentItem & AgentItem);
	//删除代理
	bool DeleteAgentInfo(WORD wAgentID);
	
	//房间管理
public:
	//激活房间
	bool ActiveServerInfo(DWORD dwSocketID, tagServiceItem & ServiceItem, tagGameServer & GameServer);
	//删除房间
	bool DeleteServerInfo(WORD wServerID);	

	//登录管理
public:
	//删除登录
	bool DeleteLogonInfo(WORD wServiceID);

	//查找函数
public:
	//寻找用户
	CGlobalUserInfo * SearchUserInfo(DWORD dwUserID);
	//寻找服务
	CGlobalServiceInfo* SearchServiceInfo(WORD wServiceModule, WORD wServiceID);

	//枚举函数
public:
	//枚举用户
	CGlobalUserInfo * EnumUserInfo(POSITION& Postion);
	//枚举服务
	CGlobalServiceInfo* EnumServiceInfo(WORD wServiceModule, POSITION& Position);

	//映射函数
public:
	//查找服务
	CGlobalServiceInfo* LookupService(WORD wServiceModule, WORD wServiceKind, DWORD dwObjectID);
	//分配服务
	CGlobalServiceInfo* AssignService(WORD wServiceModule, WORD wServiceKind, DWORD dwObjectID, WORD wServiceID=INVALID_WORD);

	//创建函数
private:
	//创建用户
	CGlobalUserInfo* CreateGlobalUserInfo();
	//创建类型
	CGlobalServiceKind* CreateGlobalServiceKind();
	//创建服务
	CGlobalServiceInfo* CreateGlobalServiceInfo(WORD wServiceModule);

	//释放函数
private:
	//释放用户
	bool FreeGlobalUserInfo(CGlobalUserInfo* pGlobalUserInfo);
	//释放类型
	bool FreeGlobalServiceKind(CGlobalServiceKind* pGlobalServiceKind);
	//释放服务
	bool FreeGlobalServiceInfo(CGlobalServiceInfo* pGlobalServiceInfo);
};

//////////////////////////////////////////////////////////////////////////

#endif