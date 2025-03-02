#ifndef CLUB_INFO_MANAGER_HEAD_FILE
#define CLUB_INFO_MANAGER_HEAD_FILE

#pragma once

//引入文件
#include "ClubServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//茶社状态
#define CLUB_STATE_OFFLINE					0							//离线状态
#define CLUB_STATE_LOADING					1							//加载状态
#define CLUB_STATE_ONLINE					2							//在线状态

//////////////////////////////////////////////////////////////////////////////////
//茶社桌子
struct tagClubTableItem
{
	tagBattleTableItem					TableItem;						//桌子子项
	tagBattleTableParamEx				TableParam;						//桌子参数
};

//约战玩法
struct tagClubBattleConfigEx : public tagClubBattleConfig
{
	WORD							wTableCount;						//桌子数量	
};

//////////////////////////////////////////////////////////////////////////////////

//类说明
class CClubInfoItem;
class CClubInfoManager;

//数组定义
typedef CWHArray<tagSocketInfo> CSocketInfoArray;
typedef CWHArray<CClubInfoItem *> CClubInfoItemArray;
typedef CWHArray<tagClubTableItem*> CClubTableArray;
typedef CWHArray<tagClubMemberInfo *> CClubMemberArray;
typedef CWHArray<tagClubKindSetting*> CKindSettingArray;
typedef CWHArray<tagClubApplyMessage *> CApplyMessageArray;
typedef CWHArray<tagClubBattleConfigEx*> CBattleConfigArray;

//仓库定义
typedef CWHStockData<DWORD, CClubInfoItem> CStockClubID;

//字典定义
typedef CMap<DWORD, DWORD, tagClubTableItem*, tagClubTableItem*> CMapTableID;
typedef CMap<DWORD, DWORD, tagClubMemberInfo*, tagClubMemberInfo*> CMapMemberID;
typedef CMap<DWORD, DWORD, tagClubApplyMessage*, tagClubApplyMessage*> CMapMessageID;

//////////////////////////////////////////////////////////////////////////////////
//茶社子项
class CClubInfoItem
{
	//友元定义
	friend class CClubInfoManager;

	//茶社属性
protected:
	tagClubItem							m_ClubItem;							//茶社子项

	//状态变量
protected:
	bool								m_bUnloadFlag;						//卸载标志
	WORD								m_wOnlineCount;						//在线人数
	BYTE								m_cbClubState;						//茶社状态
	DWORD								m_dwActiveTime;						//活跃时间
	
	//对象管理
protected:
	CMapTableID							m_MapTableID;						//桌子字典
	CMapMemberID						m_MapMemberID;						//成员字典
	CMapMessageID						m_MapMessageID;						//消息字典
	CSocketInfoArray					m_SocketInfoArray;					//网络数组	
	CKindSettingArray					m_KindSettingActive;				//类型设置
	CBattleConfigArray					m_BattleConfigActive;				//玩法数组	

	//静态存储
protected:
	static CClubTableArray				m_ClubTableBuffer;					//桌子数组
	static CClubMemberArray				m_ClubMemberBuffer;					//成员数组
	static CKindSettingArray			m_KindSettingBuffer;				//设置数组
	static CBattleConfigArray			m_BattleConfigBuffer;				//玩法数组
	static CApplyMessageArray			m_ApplyMessageBuffer;				//消息数组

	//函数定义
protected:
	//构造函数
	CClubInfoItem();
	//析构函数
	virtual ~CClubInfoItem();

	//茶社信息
public:
	//茶社标识
	DWORD GetClubID() { return m_ClubItem.ClubProperty.dwClubID; }

	//状态信息
public:
	//获取状态
	BYTE GetClubState() { return m_cbClubState; }
	//设置状态
	VOID SetClubState(BYTE cbClubState) { m_cbClubState = cbClubState; }

	//卸载标志
public:
	//获取标志
	bool GetUnloadFlag() { return m_bUnloadFlag; }
	//设置状态
	VOID SetUnloadFlag(bool bUnloadFlag) { m_bUnloadFlag = bUnloadFlag; }

	//活跃时间
public:
	//获取时间
	DWORD GetActiveTime() { return m_dwActiveTime; }
	//设置时间
	VOID SetActiveTime(DWORD dwActiveTime) { m_dwActiveTime = dwActiveTime; }

	//在线人数
public:
	//增加人数
	VOID IncreaseOnline() { m_wOnlineCount++; }
	//减少人数
	VOID DecreaseOnline() { if (m_wOnlineCount > 0) m_wOnlineCount--; }
	//获取人数
	WORD GetOnlineCount() { return m_wOnlineCount; }	
	//设置人数
	VOID SetOnlineCount(WORD wOnlineCount) { m_wOnlineCount = wOnlineCount; }

	//进入请求
public:
	//枚举请求
	tagSocketInfo* EnumEnterRequest(WORD wIndex);
	//缓存请求
	VOID CacheEnterRequest(tagSocketInfo& SocketInfo);
	//移除请求
	VOID RemoveEnterRequest(tagSocketInfo& SocketInfo);	

	//茶社属性
public:
	//茶社子项
	tagClubItem * GetClubItem() { return &m_ClubItem; }	
	//茶社规则
	tagClubRules * GetClubRules() { return &m_ClubItem.ClubRules; }
	//茶社公告
	tagClubNotice * GetClubNotice() { return &m_ClubItem.ClubNotice; }
	//茶社属性
	tagClubProperty * GetClubProperty() { return &m_ClubItem.ClubProperty; }	
	
	//茶社成员
public:	
	//移除成员
	VOID RemoveMember();
	//移除成员
	bool RemoveMember(DWORD dwMemberID);
	//添加成员
	bool AppendMember(tagClubMemberInfo & ClubMember);
	//枚举成员
	tagClubMemberInfo * EnumMember(POSITION & Position);
	//查找成员
	tagClubMemberInfo * SearchMember(DWORD dwMemberID);

	//约战玩法
public:
	//移除玩法
	VOID RemoveConfig();
	//移除玩法
	bool RemoveConfig(DWORD dwConfigID);
	//添加玩法
	bool AppendConfig(tagClubBattleConfig& BattleConfig);
	//枚举成员
	tagClubBattleConfigEx* EnumConfig(INT nIndex);
	//查找成员
	tagClubBattleConfigEx* SearchConfig(DWORD dwConfigID);

	//类型设置
public:
	//移除桌子
	VOID RemoveKindSetting();
	//移除桌子
	VOID RemoveKindSetting(WORD wKindID);
	//添加设置
	bool AppendKindSetting(tagClubKindSetting& KindSetting);
	//枚举设置
	tagClubKindSetting * EnumKindSetting(INT nIndex);
	//查找桌子
	tagClubKindSetting * SearchKindSetting(WORD wKindID);

	//桌子管理
public:
	//移除桌子
	VOID RemoveClubTable();
	//移除桌子
	VOID RemoveClubTable(DWORD dwBattleNum);
	//添加桌子
	bool AppendClubTable(tagClubTableItem& TableItem);	
	//枚举桌子
	tagClubTableItem* EnumClubTable(POSITION& Position);
	//查找桌子
	tagClubTableItem* SearchClubTable(DWORD dwBattleNum);

	//申请消息
public:
	//移除消息
	VOID RemoveMessage();
	//移除消息
	bool RemoveMessage (DWORD dwMessageID);
	//添加消息
	bool AppendMessage(tagClubApplyMessage & ApplyMessage);
	//枚举消息
	tagClubApplyMessage * EnumMessage(POSITION & Position);
	//查找消息
	tagClubApplyMessage * SearchMessage(DWORD dwMessageID);

	//辅助函数
protected:
	//重置对象
	void ResetClubItem();
	//查找请求
	INT SearchEnterRequest(tagSocketInfo& SocketInfo);

	//静态函数
protected:
	//清理数据
	static VOID CleanStaticData();

	//静态函数
private:
	//创建桌子
	static tagClubTableItem * ActiveClubTable();
	//移除桌子
	static VOID RemoveClubTable(tagClubTableItem * pClubTable);
	//创建成员
	static tagClubMemberInfo * ActiveClubMember();
	//移除成员
	static VOID RemoveClubMember(tagClubMemberInfo * pClubMember);
	//创建玩法
	static tagClubBattleConfigEx * ActiveBattleConfig();
	//移除玩法
	static VOID RemoveBattleConfig(tagClubBattleConfigEx* pIMBattleConfig);
	//创建设置
	static tagClubKindSetting * ActiveKindSetting();
	//移除设置
	static VOID RemoveKindSetting (tagClubKindSetting* pClubKindSetting);
	//创建消息
	static tagClubApplyMessage * ActiveApplyMessage();
	//移除消息
	static VOID RemoveApplyMessage(tagClubApplyMessage * pIMApplyMessage);
};

//////////////////////////////////////////////////////////////////////////////////

//茶社管理类
class CClubInfoManager
{
	//用户变量
protected:
	CStockClubID						m_StockClubID;					//茶社仓库

	//函数定义
public:
	//构造函数
	CClubInfoManager();
	//析构函数
	virtual ~CClubInfoManager();

	//查找函数
public:	
	//查找茶社
	virtual CClubInfoItem * SearchClubItem(DWORD dwClubID);
	//枚举茶社
	virtual CClubInfoItem * EnumClubItem(CClubInfoItem* pClubInfoItem);

	//统计函数
public:
	//茶社总数
	virtual DWORD GetClubItemCount() { return (DWORD)m_StockClubID.GetItemCount(); }

	//管理函数
public:
	//移除茶社
	virtual VOID RemoveClubItem();
	//移除茶社
	virtual VOID RemoveClubItem(DWORD dwClubID);
	//插入茶社
	virtual CClubInfoItem* ActiveClubItem(tagClubItem & ClubItem);
};

//////////////////////////////////////////////////////////////////////////////////
#endif