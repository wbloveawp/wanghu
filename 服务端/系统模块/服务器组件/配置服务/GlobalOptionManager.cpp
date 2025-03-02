#include "StdAfx.h"
#include "GlobalOptionManager.h"

using namespace WHJson;

////////////////////////////////////////////////////////////////////////////////////////////////
//构造函数
CGlobalOptionItem::CGlobalOptionItem()
{
	m_dwOptionMask = 0;
	m_szOptionDigest[0] = 0;
	m_strOptionValue = TEXT("");
}

//析构函数
CGlobalOptionItem::~CGlobalOptionItem()
{
}

//构造配置
bool CGlobalOptionItem::ConstructOptionItem(DWORD dwOptionMask, LPCTSTR pszOptionValue)
{
	//配置掩码
	m_dwOptionMask = dwOptionMask;
	
	//拷贝选项
	m_strOptionValue = pszOptionValue;	

	//解析配置
	return ParseOptionItem(m_strOptionValue.GetString());
}

//解析配置选项
bool CGlobalOptionItem::ParseOptionItem(LPCTSTR pszOptionValue)
{
	return true;
}


//////////////////////////////////////////////////////////////////////////////////
// 
//构造函数
CCheckInOption::CCheckInOption()
{
	//设置变量
	m_nCheckInEnabled = false;
	m_szCheckInRewards[0] = 0;
}

//析构函数
CCheckInOption::~CCheckInOption()
{
}

//解析配置选项
bool CCheckInOption::ParseOptionItem(LPCTSTR pszOptionValue)
{
	//获取对象
	auto JsonValuePtr = CWHJson::getValue(string(CT2CA(pszOptionValue)));
	if (JsonValuePtr->getType() != eJsonTypeObj)
	{
		throw L"类型错误,Json解析失败！";
	}

	//转换对象
	auto JsonValueObjPtr = CJsonValueObjPtr::dynamicCast(JsonValuePtr);

	//读取开关
	CWHJsonInput::readJson(m_nCheckInEnabled, JsonValueObjPtr->value["CheckInEnabled"], true);

	//读取奖励
	string sCheckInRewards;
	CWHJsonInput::readJson(sCheckInRewards, JsonValueObjPtr->value["CheckInRewards"], true);

	//拷贝数据
	StringCchCopy(m_szCheckInRewards, CountArray(m_szCheckInRewards), CA2CT(sCheckInRewards.c_str()));

	//生成摘要
	CMD5Encrypt::EncryptData(m_szCheckInRewards, m_szOptionDigest);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
// 
//构造函数
CTurntableOption::CTurntableOption()
{
	m_TurntableEnabled = FALSE;
	m_szTurntableAwards[0] = 0;
}

//析构函数
CTurntableOption::~CTurntableOption()
{
}

//解析配置选项
bool CTurntableOption::ParseOptionItem(LPCTSTR pszOptionValue)
{
	//获取对象
	auto JsonValuePtr = CWHJson::getValue(string(CT2CA(pszOptionValue)));
	if (JsonValuePtr->getType() != eJsonTypeObj)
	{
		throw L"类型错误,Json解析失败！";
	}

	//转换对象
	auto JsonValueObjPtr = CJsonValueObjPtr::dynamicCast(JsonValuePtr);

	//读取开关
	CWHJsonInput::readJson(m_TurntableEnabled, JsonValueObjPtr->value["TurntableEnabled"], true);

	//读取奖励
	string sTurntableAward;
	CWHJsonInput::readJson(sTurntableAward, JsonValueObjPtr->value["TurntableAwards"], true);

	//拷贝数据
	StringCchCopy(m_szTurntableAwards,CountArray(m_szTurntableAwards), CA2CT(sTurntableAward.c_str()));

	//生成摘要
	CMD5Encrypt::EncryptData(m_szTurntableAwards,m_szOptionDigest);

	return true;
}



//////////////////////////////////////////////////////////////////////////////////
//构造函数
CMarqueeNoticeOption::CMarqueeNoticeOption()
{
	//通知配置
	m_wReachRankID=0;
	m_lReachWinScore = 0xFFFFFFFFFFFFFFF;
	m_lReachAwardAmount = 0xFFFFFFFFFFFFFFF;
	m_lReachWithdrawAmount = 0xFFFFFFFFFFFFFFF;

	//滚动配置
	m_dwDurationTime = 0;
	m_NoticeRollOptionArray.RemoveAll();
}

//析构函数
CMarqueeNoticeOption::~CMarqueeNoticeOption()
{
}

//榜单类型
INT CMarqueeNoticeOption::GetListTypeArray(BYTE cbLisType[], WORD wBufferSize)
{
	INT nTypeCount = __min(wBufferSize,m_ListTypeArray.GetCount());
	CopyMemory(cbLisType, m_ListTypeArray.GetData(), nTypeCount * sizeof(cbLisType[0]));

	return nTypeCount;
}

//滚动配置
tagNoticeRollOption * CMarqueeNoticeOption::GetNoticeRollOption(BYTE cbNoticeKind)
{
	for (auto i = 0; i < m_NoticeRollOptionArray.GetCount(); i++)
	{
		if (m_NoticeRollOptionArray[i].cbNoticeKind == cbNoticeKind)
		{
			return &m_NoticeRollOptionArray[i];
		}
	}

	return NULL;
}

//解析配置选项
bool CMarqueeNoticeOption::ParseOptionItem(LPCTSTR pszOptionValue)
{
	//获取对象
	auto JsonValuePtr = CWHJson::getValue(string(CT2CA(pszOptionValue)));
	if (JsonValuePtr->getType() != eJsonTypeObj)
	{
		throw L"类型错误,Json解析失败！";
	}

	//转换对象
	auto JsonValueObjPtr = CJsonValueObjPtr::dynamicCast(JsonValuePtr);

	//通知配置
	auto NotifyOptionPtr = JsonValueObjPtr->value["NotifyOption"];
	if (NotifyOptionPtr && NotifyOptionPtr->getType() == eJsonTypeObj)
	{
		//转换对象
		auto NotifyOptionObjPtr = CJsonValueObjPtr::dynamicCast(NotifyOptionPtr);

		//读取配置
		CWHJsonInput::readJson(m_dwDurationTime, NotifyOptionObjPtr->value["DurationTime"], true);

		//登录类公告
		auto LogonNoticePtr = NotifyOptionObjPtr->value["LogonNotice"];
		if (LogonNoticePtr && LogonNoticePtr->getType() == eJsonTypeObj)
		{
			//转换对象
			auto LogonNoticeObjPtr = CJsonValueObjPtr::dynamicCast(LogonNoticePtr);

			//排名标识
			CWHJsonInput::readJson(m_wReachRankID, LogonNoticeObjPtr->value["ReachRankID"], true);

			//转换对象
			auto ListTypeArrayPtr = CJsonValueArrayPtr::dynamicCast(LogonNoticeObjPtr->value["ListType"]);

			for (auto ListTypePtr : ListTypeArrayPtr->value)
			{
				if (ListTypePtr->getType() == eJsonTypeNum)
				{
					auto ListTypeNumPtr = CJsonValueNumPtr::dynamicCast(ListTypePtr);
					m_ListTypeArray.Add((DWORD)ListTypeNumPtr->value);
				}
			}
		}

		//游戏获胜配置
		auto GameWinPtr = NotifyOptionObjPtr->value["GameWinNotice"];
		if (GameWinPtr && GameWinPtr->getType() == eJsonTypeObj)
		{
			//转换对象
			auto GameWinObjPtr = CJsonValueObjPtr::dynamicCast(GameWinPtr);

			//赢取分数
			CWHJsonInput::readJson(m_lReachWinScore, GameWinObjPtr->value["ReachWinScore"], true);			
		}

		//奖池中奖配置
		auto PrizePoolPtr = NotifyOptionObjPtr->value["PrizePoolNotice"];
		if (PrizePoolPtr && PrizePoolPtr->getType() == eJsonTypeObj)
		{
			//转换对象
			auto PrizePoolObjPtr = CJsonValueObjPtr::dynamicCast(PrizePoolPtr);

			//赢取分数
			CWHJsonInput::readJson(m_lReachAwardAmount, PrizePoolObjPtr->value["ReachAwardAmount"], true);
		}

		//用户提现配置
		auto WithdrawNoticePtr = NotifyOptionObjPtr->value["WithdrawNotice"];
		if (WithdrawNoticePtr && PrizePoolPtr->getType() == eJsonTypeObj)
		{
			//转换对象
			auto WithdrawNoticeObjPtr = CJsonValueObjPtr::dynamicCast(WithdrawNoticePtr);

			//赢取分数
			CWHJsonInput::readJson(m_lReachWithdrawAmount, WithdrawNoticeObjPtr->value["ReachWithdrawAmount"], true);
		}
	}
	else
	{
		throw L"通知配置解析失败！";
	}

	//滚动配置
	auto RollOptionPtr = JsonValueObjPtr->value["RollOption"];
	if (RollOptionPtr && RollOptionPtr->getType() == eJsonTypeArray)
	{
		//获取对象
		auto RollOptionArrayPtr = CJsonValueArrayPtr::dynamicCast(RollOptionPtr);

		//读取奖励
		for (auto RollOptionPtr : RollOptionArrayPtr->value)
		{
			if (RollOptionPtr->getType() == eJsonTypeObj)
			{
				//转换对象
				auto RollOptionObjPtr = CJsonValueObjPtr::dynamicCast(RollOptionPtr);

				//奖励信息
				tagNoticeRollOption NoticeRollOption;
				ZeroMemory(&NoticeRollOption, sizeof(NoticeRollOption));

				//读取配置
				CWHJsonInput::readJson(NoticeRollOption.cbPriority, RollOptionObjPtr->value["Priority"], true);
				CWHJsonInput::readJson(NoticeRollOption.bExclusive, RollOptionObjPtr->value["Exclusive"], true);
				CWHJsonInput::readJson(NoticeRollOption.wRollTimes, RollOptionObjPtr->value["RollTimes"], true);
				CWHJsonInput::readJson(NoticeRollOption.wViewPlace, RollOptionObjPtr->value["ViewPlace"], true);
				CWHJsonInput::readJson(NoticeRollOption.cbNoticeKind, RollOptionObjPtr->value["NoticeKind"], true);

				//添加配置
				m_NoticeRollOptionArray.Add(NoticeRollOption);
			}
		}
	}
	else
	{
		throw L"滚动配置解析失败！";
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
//构造函数
CGlobalOptionManager::CGlobalOptionManager()
{
	//初始化
	m_GlobalOptionItemMap.InitHashTable(11);
	m_GlobalOptionTempMap.InitHashTable(11);
}

//析构函数
CGlobalOptionManager::~CGlobalOptionManager()
{
	ResetData();
}

//重置数据
VOID CGlobalOptionManager::ResetData()
{
	m_GlobalOptionItemMap.RemoveAll();
	m_GlobalOptionTempMap.RemoveAll();
}

//切换配置
VOID CGlobalOptionManager::SwitchOptionItem(DWORD dwOptionMask)
{
	//签到配置
	if (dwOptionMask & OPTION_MASK_CHECKIN)
	{
		CGlobalOptionItemPtr GlobalOptionItemPtr;
		if (m_GlobalOptionTempMap.Lookup(OPTION_MASK_CHECKIN, GlobalOptionItemPtr))
		{
			m_GlobalOptionItemMap[OPTION_MASK_CHECKIN] = GlobalOptionItemPtr;
			m_GlobalOptionTempMap.RemoveKey(OPTION_MASK_CHECKIN);
		}
	}

	//转盘配置
	if (dwOptionMask&OPTION_MASK_TURNTABLE)		
	{
		CGlobalOptionItemPtr GlobalOptionItemPtr;
		if (m_GlobalOptionTempMap.Lookup(OPTION_MASK_TURNTABLE, GlobalOptionItemPtr))
		{
			m_GlobalOptionItemMap[OPTION_MASK_TURNTABLE] = GlobalOptionItemPtr;
			m_GlobalOptionTempMap.RemoveKey(OPTION_MASK_TURNTABLE);			
		}
	}

	//跑马灯配置
	if (dwOptionMask & OPTION_MASK_MARQUEE_NOTICE)
	{
		CGlobalOptionItemPtr GlobalOptionItemPtr;
		if (m_GlobalOptionTempMap.Lookup(OPTION_MASK_MARQUEE_NOTICE, GlobalOptionItemPtr))
		{
			m_GlobalOptionItemMap[OPTION_MASK_MARQUEE_NOTICE] = GlobalOptionItemPtr;
			m_GlobalOptionTempMap.RemoveKey(OPTION_MASK_MARQUEE_NOTICE);
		}
	}

	return;
}

//移除配置
VOID CGlobalOptionManager::RemoveOptionItem(DWORD dwOptionMask)
{
	m_GlobalOptionTempMap.RemoveKey(dwOptionMask);
}

//添加配置
bool CGlobalOptionManager::AppendOptionItem(DWORD dwOptionMask, LPCTSTR pszOptionValue)
{
	try
	{
		//创建对象
		CGlobalOptionItemPtr GlobalOptionItemPtr = CreateGlobalOptionItem(dwOptionMask);
		if (!GlobalOptionItemPtr)
		{
			throw TEXT("对象创建失败！");
		}

		//构成对象
		GlobalOptionItemPtr->ConstructOptionItem(dwOptionMask, pszOptionValue);

		//设置字典
		m_GlobalOptionTempMap[dwOptionMask] = GlobalOptionItemPtr;

		return true;
	}
	catch (LPCTSTR pszException)
	{
		CTraceService::TraceString(TEXT("配置项[0x%08x]构造失败:%s"), TraceLevel_Exception, dwOptionMask, pszException);
	}
	catch (...)
	{
		CTraceService::TraceString(TEXT("配置项[0x%08x]构造失败:未知异常"), TraceLevel_Exception, dwOptionMask);
	}

	return false;
}

//创建子项
CGlobalOptionItemPtr CGlobalOptionManager::CreateGlobalOptionItem(DWORD dwOptionMask)
{
	try
	{
		//变量定义
		CGlobalOptionItemPtr GlobalOptionItemPtr(NULL);

		switch (dwOptionMask)
		{
		case OPTION_MASK_CHECKIN:		 //签到配置
		{
			GlobalOptionItemPtr = new CCheckInOption();
			break;
		}
		case OPTION_MASK_TURNTABLE:		 //转盘配置
		{
			GlobalOptionItemPtr = new CTurntableOption();
			break;
		}
		case OPTION_MASK_MARQUEE_NOTICE: //跑马灯配置
		{
			GlobalOptionItemPtr = new CMarqueeNoticeOption();
			break;
		}
		}

		return GlobalOptionItemPtr;
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////