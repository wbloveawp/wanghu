#include "StdAfx.h"
#include "GlobalOptionManager.h"

using namespace WHJson;

////////////////////////////////////////////////////////////////////////////////////////////////
//���캯��
CGlobalOptionItem::CGlobalOptionItem()
{
	m_dwOptionMask = 0;
	m_szOptionDigest[0] = 0;
	m_strOptionValue = TEXT("");
}

//��������
CGlobalOptionItem::~CGlobalOptionItem()
{
}

//��������
bool CGlobalOptionItem::ConstructOptionItem(DWORD dwOptionMask, LPCTSTR pszOptionValue)
{
	//��������
	m_dwOptionMask = dwOptionMask;
	
	//����ѡ��
	m_strOptionValue = pszOptionValue;	

	//��������
	return ParseOptionItem(m_strOptionValue.GetString());
}

//��������ѡ��
bool CGlobalOptionItem::ParseOptionItem(LPCTSTR pszOptionValue)
{
	return true;
}


//////////////////////////////////////////////////////////////////////////////////
// 
//���캯��
CCheckInOption::CCheckInOption()
{
	//���ñ���
	m_nCheckInEnabled = false;
	m_szCheckInRewards[0] = 0;
}

//��������
CCheckInOption::~CCheckInOption()
{
}

//��������ѡ��
bool CCheckInOption::ParseOptionItem(LPCTSTR pszOptionValue)
{
	//��ȡ����
	auto JsonValuePtr = CWHJson::getValue(string(CT2CA(pszOptionValue)));
	if (JsonValuePtr->getType() != eJsonTypeObj)
	{
		throw L"���ʹ���,Json����ʧ�ܣ�";
	}

	//ת������
	auto JsonValueObjPtr = CJsonValueObjPtr::dynamicCast(JsonValuePtr);

	//��ȡ����
	CWHJsonInput::readJson(m_nCheckInEnabled, JsonValueObjPtr->value["CheckInEnabled"], true);

	//��ȡ����
	string sCheckInRewards;
	CWHJsonInput::readJson(sCheckInRewards, JsonValueObjPtr->value["CheckInRewards"], true);

	//��������
	StringCchCopy(m_szCheckInRewards, CountArray(m_szCheckInRewards), CA2CT(sCheckInRewards.c_str()));

	//����ժҪ
	CMD5Encrypt::EncryptData(m_szCheckInRewards, m_szOptionDigest);

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
// 
//���캯��
CTurntableOption::CTurntableOption()
{
	m_TurntableEnabled = FALSE;
	m_szTurntableAwards[0] = 0;
}

//��������
CTurntableOption::~CTurntableOption()
{
}

//��������ѡ��
bool CTurntableOption::ParseOptionItem(LPCTSTR pszOptionValue)
{
	//��ȡ����
	auto JsonValuePtr = CWHJson::getValue(string(CT2CA(pszOptionValue)));
	if (JsonValuePtr->getType() != eJsonTypeObj)
	{
		throw L"���ʹ���,Json����ʧ�ܣ�";
	}

	//ת������
	auto JsonValueObjPtr = CJsonValueObjPtr::dynamicCast(JsonValuePtr);

	//��ȡ����
	CWHJsonInput::readJson(m_TurntableEnabled, JsonValueObjPtr->value["TurntableEnabled"], true);

	//��ȡ����
	string sTurntableAward;
	CWHJsonInput::readJson(sTurntableAward, JsonValueObjPtr->value["TurntableAwards"], true);

	//��������
	StringCchCopy(m_szTurntableAwards,CountArray(m_szTurntableAwards), CA2CT(sTurntableAward.c_str()));

	//����ժҪ
	CMD5Encrypt::EncryptData(m_szTurntableAwards,m_szOptionDigest);

	return true;
}



//////////////////////////////////////////////////////////////////////////////////
//���캯��
CMarqueeNoticeOption::CMarqueeNoticeOption()
{
	//֪ͨ����
	m_wReachRankID=0;
	m_lReachWinScore = 0xFFFFFFFFFFFFFFF;
	m_lReachAwardAmount = 0xFFFFFFFFFFFFFFF;
	m_lReachWithdrawAmount = 0xFFFFFFFFFFFFFFF;

	//��������
	m_dwDurationTime = 0;
	m_NoticeRollOptionArray.RemoveAll();
}

//��������
CMarqueeNoticeOption::~CMarqueeNoticeOption()
{
}

//������
INT CMarqueeNoticeOption::GetListTypeArray(BYTE cbLisType[], WORD wBufferSize)
{
	INT nTypeCount = __min(wBufferSize,m_ListTypeArray.GetCount());
	CopyMemory(cbLisType, m_ListTypeArray.GetData(), nTypeCount * sizeof(cbLisType[0]));

	return nTypeCount;
}

//��������
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

//��������ѡ��
bool CMarqueeNoticeOption::ParseOptionItem(LPCTSTR pszOptionValue)
{
	//��ȡ����
	auto JsonValuePtr = CWHJson::getValue(string(CT2CA(pszOptionValue)));
	if (JsonValuePtr->getType() != eJsonTypeObj)
	{
		throw L"���ʹ���,Json����ʧ�ܣ�";
	}

	//ת������
	auto JsonValueObjPtr = CJsonValueObjPtr::dynamicCast(JsonValuePtr);

	//֪ͨ����
	auto NotifyOptionPtr = JsonValueObjPtr->value["NotifyOption"];
	if (NotifyOptionPtr && NotifyOptionPtr->getType() == eJsonTypeObj)
	{
		//ת������
		auto NotifyOptionObjPtr = CJsonValueObjPtr::dynamicCast(NotifyOptionPtr);

		//��ȡ����
		CWHJsonInput::readJson(m_dwDurationTime, NotifyOptionObjPtr->value["DurationTime"], true);

		//��¼�๫��
		auto LogonNoticePtr = NotifyOptionObjPtr->value["LogonNotice"];
		if (LogonNoticePtr && LogonNoticePtr->getType() == eJsonTypeObj)
		{
			//ת������
			auto LogonNoticeObjPtr = CJsonValueObjPtr::dynamicCast(LogonNoticePtr);

			//������ʶ
			CWHJsonInput::readJson(m_wReachRankID, LogonNoticeObjPtr->value["ReachRankID"], true);

			//ת������
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

		//��Ϸ��ʤ����
		auto GameWinPtr = NotifyOptionObjPtr->value["GameWinNotice"];
		if (GameWinPtr && GameWinPtr->getType() == eJsonTypeObj)
		{
			//ת������
			auto GameWinObjPtr = CJsonValueObjPtr::dynamicCast(GameWinPtr);

			//Ӯȡ����
			CWHJsonInput::readJson(m_lReachWinScore, GameWinObjPtr->value["ReachWinScore"], true);			
		}

		//�����н�����
		auto PrizePoolPtr = NotifyOptionObjPtr->value["PrizePoolNotice"];
		if (PrizePoolPtr && PrizePoolPtr->getType() == eJsonTypeObj)
		{
			//ת������
			auto PrizePoolObjPtr = CJsonValueObjPtr::dynamicCast(PrizePoolPtr);

			//Ӯȡ����
			CWHJsonInput::readJson(m_lReachAwardAmount, PrizePoolObjPtr->value["ReachAwardAmount"], true);
		}

		//�û���������
		auto WithdrawNoticePtr = NotifyOptionObjPtr->value["WithdrawNotice"];
		if (WithdrawNoticePtr && PrizePoolPtr->getType() == eJsonTypeObj)
		{
			//ת������
			auto WithdrawNoticeObjPtr = CJsonValueObjPtr::dynamicCast(WithdrawNoticePtr);

			//Ӯȡ����
			CWHJsonInput::readJson(m_lReachWithdrawAmount, WithdrawNoticeObjPtr->value["ReachWithdrawAmount"], true);
		}
	}
	else
	{
		throw L"֪ͨ���ý���ʧ�ܣ�";
	}

	//��������
	auto RollOptionPtr = JsonValueObjPtr->value["RollOption"];
	if (RollOptionPtr && RollOptionPtr->getType() == eJsonTypeArray)
	{
		//��ȡ����
		auto RollOptionArrayPtr = CJsonValueArrayPtr::dynamicCast(RollOptionPtr);

		//��ȡ����
		for (auto RollOptionPtr : RollOptionArrayPtr->value)
		{
			if (RollOptionPtr->getType() == eJsonTypeObj)
			{
				//ת������
				auto RollOptionObjPtr = CJsonValueObjPtr::dynamicCast(RollOptionPtr);

				//������Ϣ
				tagNoticeRollOption NoticeRollOption;
				ZeroMemory(&NoticeRollOption, sizeof(NoticeRollOption));

				//��ȡ����
				CWHJsonInput::readJson(NoticeRollOption.cbPriority, RollOptionObjPtr->value["Priority"], true);
				CWHJsonInput::readJson(NoticeRollOption.bExclusive, RollOptionObjPtr->value["Exclusive"], true);
				CWHJsonInput::readJson(NoticeRollOption.wRollTimes, RollOptionObjPtr->value["RollTimes"], true);
				CWHJsonInput::readJson(NoticeRollOption.wViewPlace, RollOptionObjPtr->value["ViewPlace"], true);
				CWHJsonInput::readJson(NoticeRollOption.cbNoticeKind, RollOptionObjPtr->value["NoticeKind"], true);

				//�������
				m_NoticeRollOptionArray.Add(NoticeRollOption);
			}
		}
	}
	else
	{
		throw L"�������ý���ʧ�ܣ�";
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
//���캯��
CGlobalOptionManager::CGlobalOptionManager()
{
	//��ʼ��
	m_GlobalOptionItemMap.InitHashTable(11);
	m_GlobalOptionTempMap.InitHashTable(11);
}

//��������
CGlobalOptionManager::~CGlobalOptionManager()
{
	ResetData();
}

//��������
VOID CGlobalOptionManager::ResetData()
{
	m_GlobalOptionItemMap.RemoveAll();
	m_GlobalOptionTempMap.RemoveAll();
}

//�л�����
VOID CGlobalOptionManager::SwitchOptionItem(DWORD dwOptionMask)
{
	//ǩ������
	if (dwOptionMask & OPTION_MASK_CHECKIN)
	{
		CGlobalOptionItemPtr GlobalOptionItemPtr;
		if (m_GlobalOptionTempMap.Lookup(OPTION_MASK_CHECKIN, GlobalOptionItemPtr))
		{
			m_GlobalOptionItemMap[OPTION_MASK_CHECKIN] = GlobalOptionItemPtr;
			m_GlobalOptionTempMap.RemoveKey(OPTION_MASK_CHECKIN);
		}
	}

	//ת������
	if (dwOptionMask&OPTION_MASK_TURNTABLE)		
	{
		CGlobalOptionItemPtr GlobalOptionItemPtr;
		if (m_GlobalOptionTempMap.Lookup(OPTION_MASK_TURNTABLE, GlobalOptionItemPtr))
		{
			m_GlobalOptionItemMap[OPTION_MASK_TURNTABLE] = GlobalOptionItemPtr;
			m_GlobalOptionTempMap.RemoveKey(OPTION_MASK_TURNTABLE);			
		}
	}

	//���������
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

//�Ƴ�����
VOID CGlobalOptionManager::RemoveOptionItem(DWORD dwOptionMask)
{
	m_GlobalOptionTempMap.RemoveKey(dwOptionMask);
}

//�������
bool CGlobalOptionManager::AppendOptionItem(DWORD dwOptionMask, LPCTSTR pszOptionValue)
{
	try
	{
		//��������
		CGlobalOptionItemPtr GlobalOptionItemPtr = CreateGlobalOptionItem(dwOptionMask);
		if (!GlobalOptionItemPtr)
		{
			throw TEXT("���󴴽�ʧ�ܣ�");
		}

		//���ɶ���
		GlobalOptionItemPtr->ConstructOptionItem(dwOptionMask, pszOptionValue);

		//�����ֵ�
		m_GlobalOptionTempMap[dwOptionMask] = GlobalOptionItemPtr;

		return true;
	}
	catch (LPCTSTR pszException)
	{
		CTraceService::TraceString(TEXT("������[0x%08x]����ʧ��:%s"), TraceLevel_Exception, dwOptionMask, pszException);
	}
	catch (...)
	{
		CTraceService::TraceString(TEXT("������[0x%08x]����ʧ��:δ֪�쳣"), TraceLevel_Exception, dwOptionMask);
	}

	return false;
}

//��������
CGlobalOptionItemPtr CGlobalOptionManager::CreateGlobalOptionItem(DWORD dwOptionMask)
{
	try
	{
		//��������
		CGlobalOptionItemPtr GlobalOptionItemPtr(NULL);

		switch (dwOptionMask)
		{
		case OPTION_MASK_CHECKIN:		 //ǩ������
		{
			GlobalOptionItemPtr = new CCheckInOption();
			break;
		}
		case OPTION_MASK_TURNTABLE:		 //ת������
		{
			GlobalOptionItemPtr = new CTurntableOption();
			break;
		}
		case OPTION_MASK_MARQUEE_NOTICE: //���������
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