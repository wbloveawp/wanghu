#ifndef GLOBAL_OPTION_MANAGER_HEAD_FILE
#define GLOBAL_OPTION_MANAGER_HEAD_FILE

#pragma once

#include "ConfigServiceHead.h"

//////////////////////////////////////////////////////////////////////////////////
//类型声明
class CGlobalOptionItem;
class CCheckInOption;
class CTurntableOption;
class CMarqueeNoticeOption;

//////////////////////////////////////////////////////////////////////////////////
//类型定义

//类型定义
typedef CWHAutoPtr<CCheckInOption> CCheckInOptionPtr;
typedef CWHAutoPtr<CTurntableOption> CTurntableOptionPtr;
typedef CWHAutoPtr<CGlobalOptionItem> CGlobalOptionItemPtr;
typedef CWHAutoPtr<CMarqueeNoticeOption> CMarqueeNoticeOptionPtr;

//类型定义
typedef CWHArray<CGlobalOptionItem>	CGlobalOptionArray;
typedef CWHArray<tagNoticeRollOption> CNoticeRollOptionArray;

//类型定义
typedef CMap<DWORD, DWORD, CGlobalOptionItemPtr, CGlobalOptionItemPtr>	CGlobalOptionItemMap;

//////////////////////////////////////////////////////////////////////////////////
//配置子项
class CONFIG_SERVICE_CLASS CGlobalOptionItem : public CHandleBase
{
	friend class CGlobalOptionManager;

	//炮倍属性
protected:
	DWORD							m_dwOptionMask;						//配置掩码
	CString							m_strOptionValue;					//配置选项	
	TCHAR							m_szOptionDigest[LEN_MD5];			//配置摘要

	//函数定义
public:
	//构造函数
	CGlobalOptionItem();
	//析构函数
	virtual ~CGlobalOptionItem();

	//配置属性
public:
	//配置掩码
	DWORD GetOptionMask() { return m_dwOptionMask; }
	//配置摘要
	LPCTSTR GetOptionDigest() { return m_szOptionDigest; }
	//配置内容
	LPCTSTR GetOptionValue() { return m_strOptionValue.GetString(); }

	//功能函数
public:
	//构造配置
	bool ConstructOptionItem(DWORD dwOptionMask, LPCTSTR pszOptionValue);

	//赋值函数
private:
	//解析配置选项
	virtual bool ParseOptionItem(LPCTSTR pszOptionValue);
};

//////////////////////////////////////////////////////////////////////////////////
//签到选项
class CONFIG_SERVICE_CLASS CCheckInOption : public CGlobalOptionItem
{
	friend class CGlobalOptionManager;

	//配置选项
protected:
	INT								m_nCheckInEnabled;							//签到开关	
	TCHAR							m_szCheckInRewards[2048];					//签到奖励

	//函数定义
public:
	//构造函数
	CCheckInOption();
	//析构函数
	virtual ~CCheckInOption();

	//配置选项
public:
	//签到开关
	bool IsCheckInEnabled() { return m_nCheckInEnabled > 0; }
	//签到奖励
	LPCTSTR GetCheckInRewards() { return m_szCheckInRewards; }

	//赋值函数
private:
	//解析配置选项
	virtual bool ParseOptionItem(LPCTSTR pszOptionValue);
};


//////////////////////////////////////////////////////////////////////////////////
//转盘配置
class CONFIG_SERVICE_CLASS CTurntableOption : public CGlobalOptionItem
{
	friend class CGlobalOptionManager;

	//配置选项
protected:
	INT								m_TurntableEnabled;						//转盘开关	
	TCHAR							m_szTurntableAwards[2048];					//转盘奖励

	//函数定义
public:
	//构造函数
	CTurntableOption();
	//析构函数
	virtual ~CTurntableOption();

	//配置选项
public:	
	//转盘开关
	bool IsTurntableEnabled() { return m_TurntableEnabled; }	
	//转盘奖励
	LPCTSTR GetTurntableAwards() { return m_szTurntableAwards; }

	//赋值函数
private:
	//解析配置选项
	virtual bool ParseOptionItem(LPCTSTR pszOptionValue);
};

//////////////////////////////////////////////////////////////////////////////////

//跑马灯配置
class CONFIG_SERVICE_CLASS CMarqueeNoticeOption : public CGlobalOptionItem
{
	friend class CGlobalOptionManager;

	//登录通知
protected:
	WORD							m_wReachRankID;						//名次标识
	CByteArray						m_ListTypeArray;					//榜单数组

	//游戏获胜通知
protected:
	SCORE							m_lReachWinScore;					//赢取分数

	//奖池中奖通知
protected:
	SCORE							m_lReachAwardAmount;				//奖励金额

	//提现通知
protected:
	SCORE							m_lReachWithdrawAmount;				//提现金额

	//滚动选项
protected:	
	DWORD							m_dwDurationTime;					//持续时长
	CNoticeRollOptionArray			m_NoticeRollOptionArray;			//滚动配置

	//函数定义
public:
	//构造函数
	CMarqueeNoticeOption();
	//析构函数
	virtual ~CMarqueeNoticeOption();

	//通知配置
public:
	//排名标识
	WORD GetReachRankID() { return m_wReachRankID; }
	//赢取分数
	SCORE GetReachWinScore() { return m_lReachWinScore; }
	//奖励金额
	SCORE GetReachAwardAmount() { return m_lReachAwardAmount; }
	//提现金额
	SCORE GetReachWithdrawAmount() { return m_lReachWithdrawAmount;	}
	//榜单类型
	INT GetListTypeArray(BYTE cbLisType[], WORD wBufferSize);

	//滚动配置
public:
	//持续时长
	DWORD GetDurationTime() { return m_dwDurationTime; }	
	//滚动配置
	tagNoticeRollOption * GetNoticeRollOption(BYTE cbNoticeKind);

	//赋值函数
private:
	//解析配置选项
	virtual bool ParseOptionItem(LPCTSTR pszOptionValue);
};

//////////////////////////////////////////////////////////////////////////////////


//配置管理
class CONFIG_SERVICE_CLASS CGlobalOptionManager : public CSingleton<CGlobalOptionManager>
{	
	//存储变量
protected:
	CGlobalOptionItemMap			m_GlobalOptionItemMap;					//配置字典		
	CGlobalOptionItemMap			m_GlobalOptionTempMap;					//临时配置

	//函数定义
public:
	//构造函数
	CGlobalOptionManager();
	//析构函数
	virtual ~CGlobalOptionManager();

	//服务函数
public:
	//重置数据
	VOID ResetData();

	//炮倍管理
public:	
	//切换配置
	VOID SwitchOptionItem(DWORD dwOptionMask);
	//移除配置
	VOID RemoveOptionItem(DWORD dwOptionMask);
	//添加配置
	bool AppendOptionItem(DWORD dwOptionMask, LPCTSTR pszOptionValue);	

	//配置管理
public:
	//获取配置
	template <class TYPE> TYPE GetOptionItem(DWORD dwOptionMask);

	//辅助函数
private:	
	//创建子项
	CGlobalOptionItemPtr CreateGlobalOptionItem(DWORD dwOptionMask);
};

//////////////////////////////////////////////////////////////////////////////////

//查找配置
template <class TYPE> TYPE CGlobalOptionManager::GetOptionItem(DWORD dwOptionMask)
{
	//查找物品
	CGlobalOptionItemPtr GlobalOptionItemPtr;
	if (m_GlobalOptionItemMap.Lookup(dwOptionMask, GlobalOptionItemPtr) == FALSE)
	{
		return NULL;
	}

	return (typename TYPE::element_type*)GlobalOptionItemPtr.get();
}

//////////////////////////////////////////////////////////////////////////////////

#define g_GlobalOptionManager	(CGlobalOptionManager::getInstance())

//////////////////////////////////////////////////////////////////////////////////


#endif