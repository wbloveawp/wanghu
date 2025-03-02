#ifndef SERVER_USER_MANAGER_HEAD_FILE
#define SERVER_USER_MANAGER_HEAD_FILE

#pragma once

#include "AfxTempl.h"
#include "GameServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//类型声明
class CServerUserPack;
class CServerUserItem;

//////////////////////////////////////////////////////////////////////////////////
//类型定义
typedef CWHStockData<WORD, tagGoodsHoldInfo> CStockGoodsHoldInfo;
typedef CWHStockData<DWORD, tagGoodsVariation> CStockGoodsVariation;

//类型定义
typedef CWHStockList<tagWealthVariation> CListWealthVariation;

//////////////////////////////////////////////////////////////////////////////////
//用户背包
class CServerUserPack
{
	friend class CServerUserItem;

	//状态变量
protected:
	DWORD						m_dwRecordID;							//记录标识
	bool						m_bTransMode;							//事务模式

	//更新变量
protected:
	DWORD						m_dwUpdateTime;							//更新时间
	WORD						m_wUpdateMaxCount;						//更新数量
	WORD						m_wUpdateInterval;						//更新间隔

	//存储变量
protected:
	CWordArray					m_GoodsIDList;							//物品标识
	CStockGoodsHoldInfo			m_StockGoodsHoldInfo;					//物品信息	
	CStockGoodsHoldInfo			m_StockGoodsHoldTemp;					//临时物品
	CStockGoodsVariation		m_StockGoodsVariation;					//变更记录	

	//管理变量
protected:
	CServerUserItem*			m_pServerUserItem;						//用户指针	

	//组件接口
protected:
	IServerUserItemSink*		m_pIServerUserItemSink;					//回调接口

	//函数定义
protected:
	//构造函数
	CServerUserPack(CServerUserItem* pServerUserItem);
	//析构函数
	virtual ~CServerUserPack();

	//信息函数
public:
	//物品数量
	WORD GetGoodsCount() { return (WORD)m_StockGoodsHoldInfo.GetItemCount(); }
	//变更数量
	WORD GetVariationCount() { return (WORD)m_StockGoodsVariation.GetItemCount(); }

	//物品管理
public:
	//开始事务
	VOID BeginTrans();
	//提交事务
	VOID SubmitTrans();
	//移除物品
	bool RemoveGoods(WORD wGoodsID, WORD wReason);
	//移除物品
	bool RemoveGoods(WORD wGoodsID, DWORD dwGoodsCount, WORD wReason);
	//添加物品
	WORD AppendGoods(tagGoodsHoldInfo GoodsHoldInfo[], WORD wGoodsCount);
	//更新物品
	bool UpdateGoods(WORD wGoodsID, WORD wGoodsLevel, DWORD dwIndate, WORD wReason);
	//添加物品
	bool AppendGoods(WORD wGoodsID, DWORD dwGoodsCount, WORD wGoodsLevel, DWORD dwIndate, WORD wReason);

	//变更函数
public:
	//清除变更
	VOID CleanVariation();
	//提交变更
	bool IsSubmitVariation();

	//提取函数
public:
	//提取物品
	bool DistillVariationGoods(TCHAR szBuffer[], WORD wBufferCount);
	//提取记录
	bool DistillVariationRecord(TCHAR szBuffer[], WORD wBufferCount);

	//枚举物品
public:
	//获取对象
	tagGoodsHoldInfo* GetHeadGoodsItem();
	//获取对象
	tagGoodsHoldInfo* GetNextGoodsItem(tagGoodsHoldInfo* pGoodsHoldInfo);

	//功能函数
public:
	//查找物品
	tagGoodsHoldInfo* SearchGoods(WORD wGoodsID);

	//辅助函数
public:
	//获取变更
	inline tagGoodsVariation* GetGoodsVariation();
	//持有信息
	inline tagGoodsHoldInfo* GetGoodsHoldInfo(WORD wGoodsID);

	//辅助函数
protected:
	//重置背包
	VOID ResetUserPack();
};

//////////////////////////////////////////////////////////////////////////

//用户信息
class GAME_SERVICE_CLASS CServerUserItem : public IServerUserItem
{
	//友元定义
	friend class CServerUserManager;

	//属性变量
protected:
	tagServerUserRule				m_UserRule;							//用户规则
	tagServerUserInfo				m_UserInfo;							//用户信息	
	tagServerUserScore				m_ScoreFormer;						//先前分数
	tagServerUserScore				m_ScoreVariation;					//变更分数
	tagVariationScore				m_RecordInfo;						//记录信息

	//积分变量
protected:
	SCORE							m_lRevenue;							//游戏税收
	SCORE							m_lFreeScore;						//自由积分
	SCORE							m_lLockScore;						//锁定积分
	SCORE							m_lFreezeScore;						//冻结积分
	SCORE							m_lLockServiceScore;				//锁定服务费

	//状态变量
protected:
	WORD							m_wLastTableID;						//桌子编号
	WORD							m_wLastChairID;						//椅子编号
	DWORD							m_dwLookonTime;						//旁观时间

	//位置信息
protected:
	double							m_dGlobalPosLat;					//定位精度
	double							m_dGlobalPosLng;					//定位纬度
	TCHAR							m_szPlaceName[LEN_PLACE_NAME];		//地址信息

	//权限变量
protected:
	DWORD							m_dwUserRight;						//用户权限
	DWORD							m_dwMasterRight;					//管理权限

	//等级信息
protected:
	BYTE							m_cbGameOrder;						//游戏等级
	BYTE							m_cbMemberOrder;					//会员等级

	//状态信息
protected:
	BYTE							m_cbShutUpStatus;					//禁言状态
	DWORD							m_dwShutUpOverTime;					//解禁时间

	//黑白名单
protected:
	WORD							m_wAddupWinRate;					//累计胜率
	LONGLONG						m_lAddupWinScore;					//累计赢分
	LONGLONG						m_lAddupLoseScore;					//累计输分
	tagUserControlItem				m_UserControlItem;					//控制子项
	tagControlParameter*			m_pControlParameter;				//控制参数

	//状态变量
protected:
	LONG							m_lNearInfo;						//最近信息
	LONG							m_lNearCount;						//最近数目
	DWORD							m_dwLogonTime;						//登录时间
	DWORD							m_dwPlayTimeCount;					//游戏时间
	DWORD							m_dwTotalPlayTimeCount;				//总游戏时间
	DWORD							m_dwStartTimeCount;					//游戏开始时间
	DWORD							m_dwPlayTimeVariation;				//游戏时间变化

	//辅助变量
protected:
	bool							m_bMobileUser;						//手机用户
	bool							m_bTrusteeUser;						//系统托管
	bool							m_bRobotUser;						//机器用户
	bool							m_bClientReady;						//连接标志
	bool							m_bModifyScore;						//修改标志
	TCHAR							m_szPassword[LEN_MD5];				//用户密码
	TCHAR							m_szTmpPassword[LEN_MD5];			//用户密码
	TCHAR							m_szMachineID[LEN_MACHINE_ID];		//机器序列

	//比赛信息
protected:
	DWORD							m_dwMatchID;						//比赛标识
	SCORE							m_lMatchNo;							//比赛场次
	WORD							m_wSignupCount;						//报名次数
	DWORD							m_dwTicketCount;					//赛券数量	
	DWORD							m_dwSignUpTime;						//报名时间
	BYTE							m_cbMatchStatus;					//报名状态	

	//手机定义
protected:
	WORD							m_wDeskPos;				             //当前分页
	WORD							m_wDeskCount;						 //分页桌数
	WORD							m_wMobileUserRule;					 //手机规则

	//连接属性
protected:
	WORD							m_wBindIndex;						//绑定索引
	DWORD							m_dwTokenID;						//网络令牌
	DWORD							m_dwClientIP;						//连接地址
	DWORD							m_dwInoutIndex;						//进出索引

	//低保信息
public:
	tagSubsidyInfo					m_SubsidyInfo;						//低保数据

	//配置信息
protected:
	WORD							m_wServerType;						//房间类型

	//辅助组件
protected:
	CServerUserPack					m_ServerUserPack;					//用户背包	

	//组件接口
protected:
	IServerUserItemSink *			m_pIServerUserItemSink;				//回调接口

	//函数定义
protected:
	//构造函数
	CServerUserItem();
	//析构函数
	virtual ~CServerUserItem();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { delete this; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//属性信息
public:
	//用户索引
	virtual WORD GetBindIndex() { return m_wBindIndex; }
	//网络令牌
	virtual DWORD GetTokenID() { return m_dwTokenID; }
	//用户地址
	virtual DWORD GetClientIP() { return m_dwClientIP; }
	//用户信息
	virtual tagServerUserInfo * GetUserInfo() { return &m_UserInfo; }
	//用户规则
	virtual tagServerUserRule * GetUserRule() { return &m_UserRule; }

	//属性信息
public:
	//用户标识
	virtual DWORD GetUserID() { return m_UserInfo.dwUserID; }
	//游戏标识
	virtual DWORD GetGameID() { return m_UserInfo.dwGameID; }
	//用户昵称
	virtual LPCTSTR GetAccounts() { return m_UserInfo.szAccounts; }

	//状态接口
public:
	//桌子号码
	virtual WORD GetTableID() { return m_UserInfo.wTableID; }
	//椅子号码
	virtual WORD GetChairID() { return m_UserInfo.wChairID; }
	//用户状态
	virtual BYTE GetUserStatus() { return m_UserInfo.cbUserStatus; }

	//权限信息
public:
	//用户权限
	virtual DWORD GetUserRight() { return m_dwUserRight; } 
	//管理权限
	virtual DWORD GetMasterRight() { return m_dwMasterRight; } 

	//游戏等级
public:
	//游戏等级
	virtual BYTE GetGameOrder() { return m_cbGameOrder; }
	//游戏经验
	virtual DWORD GetExperience() { return m_UserInfo.lExperience; }

	//会员等级
public:	
	//更新点数
	virtual VOID UpdateMemberPoint(DWORD dwIncPoint);
	//会员等级
	virtual BYTE GetMemberOrder() { return m_cbMemberOrder; }
	//管理等级
	virtual BYTE GetMasterOrder() { return m_UserInfo.cbMasterOrder; } 
	//会员点数
	virtual DWORD GetMemberPoint() { return m_UserInfo.dwMemberPoint; }	

	//时间信息
public:
	//登录时间
	virtual DWORD GetLogonTime() { return m_dwLogonTime; }
	//游戏时间
	virtual DWORD GetPlayTimeCount() { return m_dwPlayTimeCount; }
	//总游戏时间
	virtual DWORD GetTotalPlayTimeCount() { return m_dwTotalPlayTimeCount; }

	//积分信息
public:
	//积分类型
	virtual BYTE GetScoreKind() { return m_UserInfo.cbScoreKind; }
	//用户积分
	virtual SCORE GetUserScore() { return m_UserInfo.lScore; }
	//自由积分
	virtual SCORE GetUserFreeScore() { return m_lFreeScore-m_lLockServiceScore-m_lFreezeScore; }
	//锁定积分
	virtual SCORE GetUserLockScore() { return m_lLockScore-m_lFreezeScore; }	

	//积分信息
public:
	//用户胜率
	virtual WORD GetUserWinRate();
	//用户输率
	virtual WORD GetUserLostRate();
	//用户和率
	virtual WORD GetUserDrawRate();
	//用户逃率
	virtual WORD GetUserFleeRate();
	//游戏局数
	virtual LONG GetUserPlayCount() { return m_UserInfo.lWinCount+m_UserInfo.lLostCount+m_UserInfo.lDrawCount+m_UserInfo.lFleeCount; }

	//效验接口
public:
	//对比帐号
	virtual bool ContrastAccounts(LPCTSTR pszAccounts);
	//对比密码
	virtual bool ContrastPassword(LPCTSTR pszPassword);
	//对比临时密码
	virtual bool ContrastTempPassword(LPCTSTR pszPassword);
	//对比桌子密码
	virtual bool ContrastTablePassword(LPCTSTR pszPassword);

	//游戏状态
public:
	//设置连接
	virtual VOID SetClientReady(bool bClientReady);
	//连接状态
	virtual bool IsClientReady() { return m_bClientReady; };	

	//托管状态
public:
	//判断状态
	virtual bool IsTrusteeUser() { return m_bTrusteeUser; }
	//设置状态
	virtual VOID SetTrusteeUser(bool bTrusteeUser) { m_bTrusteeUser=bTrusteeUser; }

	//控制用户
public:
	//控制状态
	virtual bool IsRobotUser() { return m_bRobotUser; }
	//设置控制
	virtual VOID SetRobotUser(bool bRobotUser) { m_bRobotUser=bRobotUser; }

	//手机用户
public:
	//控制状态
	virtual bool IsMobileUser() { return m_bMobileUser; }
	//设置控制
	virtual VOID SetMobileUser(bool bMobileUser) { m_bMobileUser=bMobileUser; }

	//账号状态
public:
	//禁言状态
	virtual BYTE GetShutUpStatus() { return m_cbShutUpStatus;  }
	//设置状态
	virtual VOID SetShutUpStatus(BYTE cbShutUpStatus) { m_cbShutUpStatus = cbShutUpStatus; }
	//解禁时间
	virtual DWORD GetShutUpOverTime() { return m_dwShutUpOverTime;  }
	//设置时间
	virtual VOID SetShutUpOverTime(DWORD dwShutUpOverTime) { m_dwShutUpOverTime = dwShutUpOverTime;	}

	//黑白名单
public:
	//调控状态
	virtual BYTE GetRegulateStatus(SCORE lScore);
	//期望变更
	virtual bool IsExpectScoreChange(SCORE lScore);
	//当前胜率
	virtual WORD GetAddupWinRate() { return m_wAddupWinRate; }
	//累计赢分
	virtual SCORE GetAddupWinScore() { return m_lAddupWinScore; }
	//累计输分
	virtual SCORE GetAddupLoseScore() { return m_lAddupLoseScore; }	
	//控制状态
	virtual BYTE GetControlStatus() { return m_UserControlItem.cbControlStatus; }	
	//控制子项
	virtual tagUserControlItem* GetUserControlItem() { return &m_UserControlItem; }

	//比赛接口
public:	
	//比赛标识
	virtual DWORD GetMatchID() { return m_dwMatchID; }
	//比赛标识
	virtual VOID SetMatchID(DWORD dwMatchID) { m_dwMatchID=dwMatchID; }
	//比赛场次
	virtual SCORE GetMatchNo() { return m_lMatchNo; }
	//比赛标识
	virtual VOID SetMatchNo(SCORE lMatchNo) { m_lMatchNo=lMatchNo; }	
	//报名次数
	virtual WORD GetSignupCount() { return m_wSignupCount; }
	//报名次数
	virtual VOID SetSignupCount(WORD wSignupCount) { m_wSignupCount = wSignupCount; }
	//报名时间
	virtual DWORD GetSignUpTime() { return m_dwSignUpTime; }
	//报名时间
	virtual VOID SetSignUpTime(DWORD dwSignUpTime) { m_dwSignUpTime=dwSignUpTime; }	
	//赛券数量
	virtual DWORD GetTicketCount() { return m_dwTicketCount; }
	//赛券数量
	virtual VOID SetTicketCount(DWORD dwTicketCount) { m_dwTicketCount = dwTicketCount; }
	//比赛状态
	virtual BYTE GetUserMatchStatus() { return m_cbMatchStatus; }
	//比赛状态
	virtual VOID SetUserMatchStatus(BYTE cbMatchStatus) { m_cbMatchStatus=cbMatchStatus; }

	//管理接口
public:
	//设置状态
	virtual bool SetUserStatus(BYTE cbUserStatus, WORD wTableID, WORD wChairID);
	//写入财富
	virtual bool WriteUserWealth(WORD wMoneyID, SCORE lScore, WORD wReason);	
	//写入积分
	virtual bool WriteUserScore(SCORE lGameScore, SCORE lRevenue, SCORE lBetAmount, LONG lExperience, enScoreType ScoreType);
	//修改积分
	virtual VOID ModifyUserScore(SCORE lModifyScore, const tagLockScoreInfo& LockScoreInfo);
	//设置临时密码
	virtual VOID SetTmpPassword( LPCTSTR lpszPassword );	
	//修改权限
	virtual VOID ModifyUserRight( DWORD dwAddRight, DWORD dwRemoveRight );
	//查询临时密码
	virtual bool IsTmpPasswdEmpty() { return m_szTmpPassword[0] == 0; }

	//辅助接口
public:
	//用户背包
	virtual CServerUserPack* GetUserPack() { return &m_ServerUserPack; }

	//高级接口
public:
	//变更判断
	virtual bool IsVariation() { return m_bModifyScore; }
	//设置参数
	virtual bool SetUserParameter(DWORD dwClientIP, WORD wBindIndex, DWORD dwTokenID, bool bRobotUser, bool bClientReady);
	//提取变更
	virtual bool DistillVariation(tagVariationScore & VariationScore, const tagLockScoreInfo & LockScoreInfo);
	//冻结积分
	virtual bool FreezeUserScore( SCORE lScore, bool bNotify );

	//新增接口
public:
	//机器标识
	virtual LPCTSTR GetMachineID() { return m_szMachineID; }
	//获取密码
	virtual LPCTSTR GetPassword(){return m_szPassword;}

	//低保接口
public:	
	//低保信息
	virtual tagSubsidyInfo& GetSubsidyInfo() { return m_SubsidyInfo; }
	//剩余次数
	virtual BYTE GetResidualTimes() { return m_SubsidyInfo.cbResidualTimes; }
	//赠送标识
	virtual bool IsLargessSubsidy() { return m_SubsidyInfo.bLargessSubsidy; }
	//剩余次数
	virtual void SetResidualTimes(BYTE cbTimes) { m_SubsidyInfo.cbResidualTimes=cbTimes; }
	//赠送标识
	virtual void SetLargessSubsidy(bool bLargessSubsidy) { m_SubsidyInfo.bLargessSubsidy=bLargessSubsidy; }
	//低保分数
	virtual SCORE GetSubsidyScore() { return m_UserInfo.lScore + m_UserInfo.lInsureGold + m_SubsidyInfo.lConsumeScore; };

	//位置信息
public:
	//定位经度
	virtual double GetGlobalPosLng() { return m_dGlobalPosLng; }
	//定位纬度
	virtual double GetGlobalPosLat() { return m_dGlobalPosLat; }	
	//地址信息
	virtual PTCHAR GetGlobalPlace() { return m_szPlaceName; }

	//高级接口
public:
	//解除绑定
	virtual bool DetachBindStatus();
	//记录索引
	virtual DWORD GetInoutIndex() { return m_dwInoutIndex; }
	//查询记录
	virtual bool QueryRecordInfo(tagVariationScore & UserRecordInfo);
	//设置自由积分
	void SetFreeScore(SCORE lFreeScore){m_lFreeScore=lFreeScore;}
	//设置锁分
	void SetLockScore(SCORE lLockScore){m_lLockScore=lLockScore;}

	//辅助函数
public:
	//获取时间
	DWORD GetLookonTime() { return m_dwLookonTime; }
	//设置时间
	VOID SetLookonTime(DWORD dwLookonTime) { m_dwLookonTime = dwLookonTime;	}	
	//设置接口
	VOID SetServerUserItemSink(IServerUserItemSink* pIServerUserItemSink);

	//辅助函数
private:
	//重置数据
	VOID ResetUserItem();
	//更新状态
	VOID UpdateControlStatus(SCORE lScore,SCORE lBetAmount);

};

//////////////////////////////////////////////////////////////////////////

//用户索引类
typedef CWHArray<CServerUserItem *> CServerUserItemArray;
typedef CMap<DWORD,DWORD,CServerUserItem *,CServerUserItem *> CServerUserItemMap;

//用户管理类
class GAME_SERVICE_CLASS CServerUserManager : public IServerUserManager
{
	//配置信息
protected:
	tagControlParameter*			m_pControlParameter;				//控制参数
	tagGameServiceOption *			m_pGameServiceOption;				//服务选项

	//用户变量
protected:
	CServerUserItemMap				m_UserIDMap;						//I D 索引
	CServerUserItemArray			m_UserItemArray;					//用户数组
	CServerUserItemArray			m_UserItemStore;					//存储用户

	//组件接口
protected:
	IServerUserItemSink *			m_pIServerUserItemSink;				//回调接口

	//函数定义
public:
	//构造函数
	CServerUserManager();
	//析构函数
	virtual ~CServerUserManager();

	//基础接口
public:
	//释放对象
	virtual VOID Release() { return; }
	//接口查询
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//配置函数
public:
	//设置接口
	virtual bool SetServerUserItemSink(IUnknownEx * pIUnknownEx);
	//控制参数
	virtual VOID SetControlParamter(tagControlParameter* pControlParameter);
	//服务参数
	virtual VOID SetServiceParameter(tagGameServiceOption* pGameServiceOption);	

	//查找函数
public:
	//枚举用户
	virtual IServerUserItem * EnumUserItem(WORD wEnumIndex);
	//查找用户
	virtual IServerUserItem * SearchUserItem(DWORD dwUserID);
	//查找用户
	virtual IServerUserItem * SearchUserItem(LPCTSTR pszAccounts);

	//统计函数
public:
	//在线人数
	virtual DWORD GetUserItemCount() { return (DWORD)m_UserItemArray.GetCount(); }	

	//管理函数
public:
	//删除用户
	virtual bool DeleteUserItem();
	//删除用户
	virtual bool DeleteUserItem(IServerUserItem* pIServerUserItem);
	//激活用户
	virtual IServerUserItem * ActiveUserItem(tagServerUserInfo & UserInfo, tagUserInfoPlus & UserInfoPlus, LPCTSTR pszPassword);	
};

//////////////////////////////////////////////////////////////////////////

#endif