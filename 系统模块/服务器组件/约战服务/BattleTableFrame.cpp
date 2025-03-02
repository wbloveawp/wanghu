#include "StdAfx.h"
#include "math.h"
#include "DataBasePacket.h"
#include "BattleTableFrame.h"
#include "..\游戏服务\DataBasePacket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
//常量定义
#define MIN_GPS_DISTANCE			100									//GPS距离

#ifdef _DEBUG
#define TIME_BATTLE_TABLE_DISMISS	 600								//解散时间
#define TIME_BATTLE_AGREEMENT_DISMISS 60								//协议解散
#else
#define TIME_BATTLE_TABLE_DISMISS	  10								//解散时间
#define TIME_BATTLE_AGREEMENT_DISMISS 60*2								//协议解散
#endif

//////////////////////////////////////////////////////////////////////////

//定义结构
struct tagUserRateItem
{
	WORD wChairID;
	WORD wAddupWinRate;
	BYTE cbControlStatus;
};

//////////////////////////////////////////////////////////////////////////

//坐标校验
bool IsGPSValid(double dLat, double dLng) 
{
	if (abs(dLat)>90.f) return false;
	if (abs(dLng)>180.f) return false;

	return true;
}

//计算GPS距离
double GetGPSDistance(double dLat1, double dLng1, double dLat2, double dLng2)
{
	//范围校验
	if (IsGPSValid(dLat1,dLng1)==false) return 0;
	if (IsGPSValid(dLat2,dLng2)==false) return 0;

	//变量定义
  double dDistance=0.f;
  double dLonRes=102900.f,dLatRes=110000.f;

	//计算距离
	double dAbsLat=abs(dLat1-dLat2);
	double dAbsLng=abs(dLng1-dLng2);
  dDistance=sqrt(dAbsLat*dLatRes*dAbsLat*dLatRes + dAbsLng*dLonRes*dAbsLng*dLonRes);

  return dDistance;
}

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameBattle::CTableFrameBattle()
{
	//标志变量
	m_bDeductCost = false;
	m_bReturnCost = false;
	m_bDismissNotify = false;
	m_bBattleService = false;
	m_bBattleFinish = false;

	//协议解散
	m_dwLaunchTime = 0;
	m_dwLaunchUserID = 0;
	ZeroMemory(m_cbBallotResult, sizeof(m_cbBallotResult));

	//约战信息
	m_dwConfigID = 0;
	m_dwBattleNum = 0;
	m_dwCreateTime = 0;
	m_dwBattleRule = 0;
	m_bBattleService = true;
	m_lBattleRecordID = 0;
	m_cbBattleState = BATTLE_STATE_FREE;
	ZeroMemory(&m_BattleConfig, sizeof(m_BattleConfig));

	//茶舍信息
	m_dwClubID = 0;
	m_wSwitchOption = 0;
	m_dwClubCreaterID = 0;

	//组件接口
	m_pIEventBattleItem = NULL;
	m_pIEventGameOption = NULL;
	m_pITCPSocketBattle = NULL;
}

//析构函数
CTableFrameBattle::~CTableFrameBattle()
{
}

//接口查询
VOID* CTableFrameBattle::QueryInterface(REFGUID Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IEventBattleSink, Guid, dwQueryVer);
	return __super::QueryInterface(Guid, dwQueryVer);
}

//计算税收
SCORE CTableFrameBattle::AccountRevenue(SCORE lGameScore)
{
	//约战积分
	if (m_BattleConfig.cbScoreKind == SCORE_KIND_BATTLE) return 0L;

	//高级配置
	auto pAdvancedOption = (tagAdvancedOption*)m_BattleConfig.cbAdvancedOption;

	if (lGameScore<= pAdvancedOption->lMinWinScore) return 0L;

	return (pAdvancedOption->lRevenueRate+m_BattleConfig.lGameRevenue) * lGameScore / 1000L;

}

//关联椅子
WORD CTableFrameBattle::GetRelationChairID(DWORD dwUserID)
{
	auto pBattleUserItem = GetBattleUserItem(dwUserID);
	return pBattleUserItem ? pBattleUserItem->wChairID:INVALID_WORD;
}

//关联参数
tagRelationParameter* CTableFrameBattle::GetRelationParameter()
{
	//构造结构
	static tagRelationParameter_Battle RelationParameter = {};

	//设置变量
	RelationParameter.dwClubID = m_dwClubID;
	RelationParameter.cbScoreKind = m_BattleConfig.cbScoreKind;
	RelationParameter.dwRelationNum = m_dwBattleNum;
	RelationParameter.lRelationRecordID = m_lBattleRecordID;	

	return &RelationParameter;
}

//写入积分
bool CTableFrameBattle::WriteUserScore(WORD wChairID, tagScoreInfo& ScoreInfo)
{
	if (!__super::WriteUserScore(wChairID, ScoreInfo))
	{
		return false;
	}

	//统计成绩
	if (m_dwBattleNum != 0)
	{
		//获取信息
		tagBattleScoreItem* pUserScoreItem = NULL;

		//查找子项
		for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
		{
			if (m_BattleUserItemArray[i].wChairID == wChairID)
			{
				pUserScoreItem = &m_BattleUserItemArray[i];
				break;
			}
		}

		//统计成绩
		if (pUserScoreItem != NULL)
		{
			//统计局数
			if (ScoreInfo.lScore < 0) pUserScoreItem->lLostCount++;
			if (ScoreInfo.lScore > 0) pUserScoreItem->lWinCount++;
			if (ScoreInfo.lScore == 0) pUserScoreItem->lDrawCount++;

			//累计分数
			pUserScoreItem->lScore += ScoreInfo.lScore;
		}
	}

	return true;
}

//kk jjia
//qu zhuo zi canshu
CMD_GF_TableParam CTableFrameBattle::GetTableParam(tagScoreInfo ScoreInfoArray[], WORD wScoreCount)
{
	//变量定义
	CMD_GF_TableParam GFTableParam;
	ZeroMemory(&GFTableParam, sizeof(GFTableParam));

	//桌子信息
	GFTableParam.lBaseScore = m_lBaseScore;
	GFTableParam.dwOwnerID = m_dwTableOwnerID;
	GFTableParam.dwBattleNum = m_dwBattleNum;

	//结算信息
	GFTableParam.wFinishCount = m_wFinishCount;
	GFTableParam.wPlayCount = m_BattleConfig.wPlayCount;

	//记录标识
	GFTableParam.lBattleRecordID = m_lBattleRecordID;
	return GFTableParam;

}


//写入积分
bool CTableFrameBattle::WriteTableScore(tagScoreInfo ScoreInfoArray[], WORD wScoreCount)
{
	//CTraceService::TraceString(_T("WriteTableScore_1"), TraceLevel_Exception);
	if (!__super::WriteTableScore(ScoreInfoArray, wScoreCount))//
	{
		return false;
	}
	//CTraceService::TraceString(_T("WriteTableScore_2"), TraceLevel_Exception);
	//统计成绩
	if (m_dwBattleNum != 0)
	{
		//变量定义
		tagScoreInfo* pScoreInfo = NULL;
		tagBattleScoreItem* pUserScoreItem = NULL;

		for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
		{
			//获取信息
			pUserScoreItem = &m_BattleUserItemArray[i];
			pScoreInfo = &ScoreInfoArray[pUserScoreItem->wChairID];				

			//计算税收
			SCORE lClubRevenue = 0;// CalcCustomRevenue(pScoreInfo->lScore + pScoreInfo->lRevenue);//kk guan
						
			//test start
			//高级配置
		//auto pAdvancedOption = (tagAdvancedOption*)m_BattleConfig.cbAdvancedOption;
			//CString s;
			//s.Format(_T("a16  lScore:%I64d,lClubRevenue:%I64d, lRevenue:%I64d, m_BattleConfig.cbScoreKind:%d , SCORE_KIND_BATTLE:%d"), pScoreInfo->lScore, lClubRevenue, pScoreInfo->lRevenue, m_BattleConfig.cbScoreKind, SCORE_KIND_BATTLE);
			//CTraceService::TraceString(s, TraceLevel_Exception); 
			//test 
			// 
			//更新积分
			pUserScoreItem->lScore += pScoreInfo->lScore;//
			pUserScoreItem->lClubRevenue += lClubRevenue;
			pUserScoreItem->lSysRevenue += pScoreInfo->lRevenue-lClubRevenue;			

			//修改胜负
			switch (pScoreInfo->ScoreType)
			{
			case ScoreType_Win:		//胜局
			{
				pUserScoreItem->lWinCount++;
				break;
			}
			case ScoreType_Lose:	//输局
			{
				pUserScoreItem->lLostCount++;
				break;
			}
			case ScoreType_Draw:	//和局
			{
				pUserScoreItem->lDrawCount++;
				break;
			}
			}
		}
	}

	return true;
}

//约战结束
bool CTableFrameBattle::BattleFinish(BYTE cbReason)
{
	//变量定义
	bool bGameStarted = IsGameStarted();	

	//判断原因
	if (m_dwBattleNum != 0 && cbReason != BFR_NORMAL)
	{
		//退费校验
		if (m_BattleConfig.lConsumeCount > 0 && (m_wFinishCount == 0 || (m_wFinishCount == 1 && bGameStarted == true)))
		{
			//退还费用
			if (m_bDeductCost == true)
			{
				PerformReturnCost(m_BattleConfig.wPayType);
			}
			else
			{
				m_bReturnCost = true;
			}
		}

		//构造消息
		WORD wType = 0;
		TCHAR szMessage[128] = L"";

		//超时解散
		if (cbReason == BFR_OFFLINE_EXPIRE)
		{
			wType = SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE;
			StringCchCopy(szMessage, CountArray(szMessage), CWHService::FormatMessage(MIM_SERVER_0065, TEXT("游戏超时时间已到，当前有玩家超时或断线超过规定时间；约战自动结束。")));
		}

		//协议解散
		if (cbReason == BFR_AGREEMENT_DISMISS)
		{
			wType = SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE;
			if (m_wFinishCount == 0) wType |= SMT_CLOSE_GAME;
			StringCchCopy(szMessage, CountArray(szMessage), CWHService::FormatMessage(MIM_SERVER_0066, TEXT("该房间经玩家协商，一致同意解散，请重新约战。")));
		}

		//消息群发
		if (szMessage[0] != 0)
		{
			for (WORD i = 0; i < m_wChairCount; i++)
			{
				//获取用户
				IServerUserItem* pIServerUserItem = GetTableUserItem(i);
				if ((pIServerUserItem == NULL) || (pIServerUserItem->IsClientReady() == false)) continue;

				//发送数据							
				m_pIServiceFunction->SendGameMessage(pIServerUserItem, szMessage, wType);
			}

			//变量定义
			WORD wEnumIndex = 0;
			IServerUserItem* pIServerUserItem = NULL;

			//旁观用户
			do
			{
				//获取用户
				pIServerUserItem = EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem == NULL) break;
				if (pIServerUserItem->IsClientReady() == false) continue;

				//发送数据							
				m_pIServiceFunction->SendGameMessage(pIServerUserItem, szMessage, wType);

			} while (true);
		}
	}

	//完成标志
	m_bBattleFinish = (cbReason == BFR_NORMAL);	

	//解散游戏
	if (bGameStarted == true) DismissGame();	

	//约战结束
	if (m_dwBattleNum != 0) DismissBattleTable(!m_bReturnCost);

	return true;
}

//获取数组
bool CTableFrameBattle::GetOptionValueArray(WORD wOptionIndex, COptionValueArray& OptionValueArray)
{
	return m_pIServiceFunction->GetOptionValueArray(m_BattleConfig.cbEmployScene, wOptionIndex, OptionValueArray);
}

//约战规则
VOID CTableFrameBattle::SetBattleRule(DWORD dwAddRules, DWORD dwRemoveRules)
{
	//添加规则
	m_dwBattleRule |= dwAddRules;

	//移除规则
	m_dwBattleRule &= ~dwRemoveRules;	
}

//游戏事件
bool CTableFrameBattle::OnEventSocketGame(WORD wSubCmdID, VOID* pData, WORD wDataSize, IServerUserItem* pIServerUserItem)
{
	//框架调用
	if (__super::OnEventSocketGame(wSubCmdID, pData, wDataSize, pIServerUserItem))
	{
		return true;
	}

	return true;
}

//框架事件
bool CTableFrameBattle::OnEventSocketFrame(WORD wSubCmdID, VOID* pData, WORD wDataSize, IServerUserItem* pIServerUserItem)
{
	//框架调用
	if (__super::OnEventSocketFrame(wSubCmdID, pData, wDataSize, pIServerUserItem))
	{
		return true;
	}

	switch (wSubCmdID)
	{
	case SUB_GF_MODIFY_CONFIG:		//修改配置
	{
		//提取数据
		auto pModifyConfig = (CMD_GF_ModifyConfig*)pData;

		//校验状态
		if (m_dwBattleNum != pModifyConfig->dwBattleNum) return false;
		if (pIServerUserItem->GetUserID() != m_dwTableOwnerID) return false;

		//语音掩码
		if (pModifyConfig->wOptionMask == OPTION_MASK_VOICE)
		{
			m_BattleConfig.cbAllowVoice = pModifyConfig->cbOptionValue;
		}	

		//变量定义
		CMD_BS_S_ModifyTable ModifyBattleTable;
		ZeroMemory(&ModifyBattleTable, sizeof(ModifyBattleTable));

		//桌子信息
		ModifyBattleTable.dwUserID = m_dwTableOwnerID;
		ModifyBattleTable.dwBattleNum = m_dwBattleNum;
		CopyMemory(&ModifyBattleTable.BattleConfig, &m_BattleConfig, sizeof(ModifyBattleTable.BattleConfig));

		//发送消息
		WORD wPacketSize = sizeof(ModifyBattleTable) - sizeof(ModifyBattleTable.szErrorMessage);
		m_pITCPSocketBattle->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_MODIFY_TABLE, &ModifyBattleTable, wPacketSize, 0);

		//配置通知
		NotifyTableConfig();

		return true;
	}
	case SUB_GF_LAUNCH_DISMISS:		//发起解散
	{
		//判断配置
		if (CServerRule::IsAllowAgreementDismiss(m_pGameServiceOption->dwServerRule) == false)
		{
			return false;
		}

		//判断状态
		if (m_dwLaunchUserID != 0) return true;
		if (m_wFinishCount == 0) return true;

		//获取用户
		tagBattleUserItem* pBattleUserItem = GetBattleUserItem(pIServerUserItem->GetUserID());
		if (pBattleUserItem == NULL || pBattleUserItem->cbUserStatus == US_LOOKON) return true;

		//设置变量
		m_dwLaunchTime = (DWORD)time(NULL);
		m_dwLaunchUserID = pIServerUserItem->GetUserID();
		m_cbBallotResult[pIServerUserItem->GetChairID()] = 1;

		//构造结构
		CMD_GF_DismissNotify DismissNotify;
		ZeroMemory(&DismissNotify, sizeof(DismissNotify));

		//设置变量
		DismissNotify.dwUserID = m_dwLaunchUserID;
		DismissNotify.wResidueTime = TIME_BATTLE_AGREEMENT_DISMISS;

		//获取配置
		if (m_pIEventCustomFrameTime != NULL)
		{
			DismissNotify.wResidueTime = m_pIEventCustomFrameTime->GetAgreementDismissTime();
		}

		//统计用户
		for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
		{
			//获取对象
			pBattleUserItem = &m_BattleUserItemArray[i];
			if (pBattleUserItem->cbUserStatus == US_LOOKON) continue;

			//设置用户
			DismissNotify.wNeedAgreeCount++;
			DismissNotify.dwUserIDList[DismissNotify.wUserCount++] = pBattleUserItem->dwUserID;
		}

		//调整同意人数
		if (m_pIEventBattleItem != NULL)
		{
			DismissNotify.wNeedAgreeCount = m_pIEventBattleItem->GetDismissAgreeCount();
			if (DismissNotify.wNeedAgreeCount > DismissNotify.wUserCount) DismissNotify.wNeedAgreeCount = DismissNotify.wUserCount;
		}

		//游戏用户
		for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
		{
			//获取对象
			pBattleUserItem = &m_BattleUserItemArray[i];
			if (pBattleUserItem->cbUserStatus == US_LOOKON) continue;

			//获取用户
			IServerUserItem* pISendUserItem = GetTableUserItem(pBattleUserItem->wChairID);
			if ((pISendUserItem == NULL) || (pISendUserItem->IsClientReady() == false)) continue;

			//发送数据
			m_pIServiceFunction->SendDataToUser(pISendUserItem, MDM_GF_FRAME, SUB_GF_DISMISS_NOTIFY, &DismissNotify, sizeof(DismissNotify));
		}

		//游戏暂停
		PauseGame();

		//设置定时器
		SetGameTimer(IDI_AGREEMENT_DISMISS, (DismissNotify.wResidueTime + 5) * 1000L, 1, NULL);

		return true;
	}
	case SUB_GF_BALLOT_DISMISS:		//投票解散
	{
		//参数校验
		ASSERT(wDataSize == sizeof(CMD_GF_BallotDismiss));
		if (wDataSize != sizeof(CMD_GF_BallotDismiss)) return false;

		//状态判断
		if (m_dwLaunchUserID == 0) return true;

		//提取数据
		CMD_GF_BallotDismiss* pBallotDismiss = (CMD_GF_BallotDismiss*)pData;

		//执行投票
		return PerformBallotDismiss(pIServerUserItem->GetChairID(), pIServerUserItem->GetUserID(), pBallotDismiss->bAgreeDismiss);
	}
	}

	return true;
}

//尝试开始
bool CTableFrameBattle::TryStartGame()
{
	if (m_BattleConfig.lConsumeCount > 0 && m_bDeductCost == false)
	{
		////房主支付
		//if (m_BattleConfig.wPayType == PAY_TYPE_OWNER)
		//{
		//	//构造结构
		//	DBR_GR_DeductBattleCost DeductBattleCost;
		//	ZeroMemory(&DeductBattleCost, sizeof(DeductBattleCost));

		//	//开桌参数
		//	DeductBattleCost.wTableID = m_wTableID;
		//	DeductBattleCost.dwClubID = m_dwClubID;
		//	DeductBattleCost.dwUserID = m_dwTableOwnerID;
		//	DeductBattleCost.lConsumeCount = m_BattleConfig.lConsumeCount;
		//	DeductBattleCost.cbCurrencyKind = m_BattleConfig.cbCurrencyKind;

		//	//投递数据
		//	m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DEDUCT_BATTLE_COST, m_dwBattleNum, 0L, 0L, &DeductBattleCost, sizeof(DeductBattleCost));

		//	return false;			
		//}

		//AA支付
		if (m_BattleConfig.wPayType == PAY_TYPE_USER)
		{
			//变量定义
			tagBattleUser BattleUser;
			tagBattleScoreItem* pUserScoreItem = NULL;

			//构造结构
			DBR_GR_DeductBattleCostAA DeductBattleCost;
			ZeroMemory(&DeductBattleCost, sizeof(DeductBattleCost));

			//开桌参数
			DeductBattleCost.wTableID = m_wTableID;
			DeductBattleCost.dwClubID = m_dwClubID;
			DeductBattleCost.lConsumeCount = m_BattleConfig.lConsumeCount;
			DeductBattleCost.cbCurrencyKind = m_BattleConfig.cbCurrencyKind;

			//设置用户
			for (WORD i = 0; i < m_wChairCount; i++)
			{
				//获取用户
				IServerUserItem* pIServerUserItem = GetTableUserItem(i);
				if (pIServerUserItem == NULL) continue;
				if (pIServerUserItem->GetUserStatus() != US_READY) continue;

				//设置变量
				BattleUser.dwUserID = pIServerUserItem->GetUserID();

				//拷贝数据
				CopyMemory(&DeductBattleCost.BattleUser[DeductBattleCost.wUserCount++], &BattleUser, sizeof(BattleUser));
			}

			//有人未扣费
			if (DeductBattleCost.wUserCount > 0)
			{
				//投递数据
				WORD wStructSize = DeductBattleCost.wUserCount * sizeof(tagBattleUser);
				WORD wSendDataSize = sizeof(DeductBattleCost) - sizeof(DeductBattleCost.BattleUser) + wStructSize;
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DEDUCT_BATTLE_COSTAA, m_dwBattleNum, 0L, 0L, &DeductBattleCost, wSendDataSize);

				return false;
			}
		}
	}

	return __super::TryStartGame();
}

//开始事件
bool CTableFrameBattle::OnEventGameStart()
{
	//校验编号
	if (m_dwBattleNum == 0) return false;
		
	//绑定信息
	if (++m_wFinishCount == 1)
	{
		//设置变量
		m_dwStartTime = (DWORD)time(NULL);
		m_cbBattleState = BATTLE_STATE_PLAY;
	}

	//锁定用户
	DBR_GR_BattleLock BattleLock;
	ZeroMemory(&BattleLock, sizeof(BattleLock));

	//约战信息
	BattleLock.dwBattleNum = m_dwBattleNum;
	BattleLock.cbScoreKind = m_BattleConfig.cbScoreKind;

	//用户分数
	tagBattleUserItem BattleUserItem;
	ZeroMemory(&BattleUserItem, sizeof(BattleUserItem));

	//设置用户
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//获取用户
		IServerUserItem* pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;
		if (GetBattleUserItem(pIServerUserItem->GetUserID()) != NULL) continue;

		//设置信息
		BattleUserItem.wChairID = i;
		BattleUserItem.dwUserID = pIServerUserItem->GetUserID();
		BattleUserItem.dwGameID = pIServerUserItem->GetGameID();
		BattleUserItem.cbGender = pIServerUserItem->GetUserInfo()->cbGender;
		StringCchCopy(BattleUserItem.szNickName, CountArray(BattleUserItem.szNickName), pIServerUserItem->GetAccounts());

		//添加玩家
		m_BattleUserItemArray.Add(BattleUserItem);

		//添加用户
		BattleLock.dwUserID[BattleLock.wUserCount++] = BattleUserItem.dwUserID;
	}

	//投递数据
	if (BattleLock.wUserCount > 0)
	{
		WORD wStructSize = BattleLock.wUserCount * sizeof(BattleLock.dwUserID[0]);
		WORD wPacketSize = sizeof(BattleLock) - sizeof(BattleLock.dwUserID) + wStructSize;
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_BATTLE_LOCK, m_dwBattleNum, 0L, 0L, &BattleLock, wPacketSize);
	}
	
	//更新属性
	for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
	{
		//获取信息
		tagBattleUserItem* pBattleUserItem = &m_BattleUserItemArray[i];

		//获取用户
		IServerUserItem* pIServerUserItem = GetTableUserItem(pBattleUserItem->wChairID);
		if (pIServerUserItem == NULL) continue;

		//头像信息
		pBattleUserItem->wFaceID = pIServerUserItem->GetUserInfo()->wFaceID;
		pBattleUserItem->dwCustomID = pIServerUserItem->GetUserInfo()->dwCustomID;
	}

	//发送成绩
	NotifyTableGrade();

	//桌子状态		
	NotifyTableParam();

	//关闭定时器
	KillGameTimer(IDI_TABLE_DISMISS);

	return true;
}

//结束时间
bool CTableFrameBattle::OnEventGameConclude()
{
	//校验编号
	if (m_dwBattleNum == 0) return false;
	
	//发送成绩
	NotifyTableGrade();

	//桌子参数
	NotifyTableParam();

	//尝试解散
	if (IsBattleFinished() == true)
	{
		//约战结束
		DismissBattleTable(!m_bReturnCost);
	}	
	else
	{
		//解散时间
		DWORD dwDismissTime = TIME_BATTLE_TABLE_DISMISS;
		if (m_pIEventCustomFrameTime != NULL)
		{
			dwDismissTime = m_pIEventCustomFrameTime->GetBattleDismissTime();
		}

		//设置定时器
		SetGameTimer(IDI_TABLE_DISMISS, dwDismissTime * 1000L, -1, 0);		
	}

	return true;
}

//用户坐下
bool CTableFrameBattle::OnActionUserSitdown(IServerUserItem* pIServerUserItem)
{
	if (m_dwBattleNum != 0)
	{
		//关闭定时器
		//KillGameTimer(IDI_TABLE_DISMISS);

		//坐下通知
		if (m_BattleConfig.wNotifyMask & NOTIFY_MASK_TABLE_USER)
		{
			//构造结构
			CMD_BS_S_UserSitdown UserSitdown;
			ZeroMemory(&UserSitdown, sizeof(UserSitdown));

			//设置变量
			UserSitdown.dwBattleNum = m_dwBattleNum;
			UserSitdown.dwUserID = pIServerUserItem->GetUserID();

			//用户信息
			UserSitdown.TableUserItem.wChairID = pIServerUserItem->GetChairID();
			UserSitdown.TableUserItem.dwGameID = pIServerUserItem->GetGameID();
			UserSitdown.TableUserItem.dwUserID = pIServerUserItem->GetUserID();
			UserSitdown.TableUserItem.wFaceID = pIServerUserItem->GetUserInfo()->wFaceID;
			UserSitdown.TableUserItem.cbGender = pIServerUserItem->GetUserInfo()->cbGender;
			UserSitdown.TableUserItem.dwCustomID = pIServerUserItem->GetUserInfo()->dwCustomID;			
			UserSitdown.TableUserItem.wGameOrder = g_GameLevelManager->CalcGameOrder(pIServerUserItem->GetExperience());
			UserSitdown.TableUserItem.wMemberOrder = g_GameLevelManager->CalcUserOrder(pIServerUserItem->GetMemberPoint());
			StringCchCopy(UserSitdown.TableUserItem.szNickName, CountArray(UserSitdown.TableUserItem.szNickName), pIServerUserItem->GetAccounts());
			StringCchCopy(UserSitdown.TableUserItem.szUserHeadUrl, CountArray(UserSitdown.TableUserItem.szUserHeadUrl), pIServerUserItem->GetUserInfo()->szUserHeadUrl);

			//发送消息		
			m_pITCPSocketBattle->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_USER_SITDOWN, &UserSitdown, sizeof(UserSitdown), 0);
		}
	}

	return true;
}

//用户起立
bool CTableFrameBattle::OnActionUserStandup(IServerUserItem* pIServerUserItem)
{
	//起立通知
	if (m_dwBattleNum != 0)
	{
		//约战用户信息
		tagBattleUserItem* pBattleUserItem = GetBattleUserItem(pIServerUserItem->GetUserID());
		if (m_wFinishCount == 0 || pBattleUserItem == NULL)
		{
			//起立通知
			if (m_BattleConfig.wNotifyMask & NOTIFY_MASK_TABLE_USER)
			{
				//构造结构
				CMD_BS_S_UserStandup UserStandup;
				ZeroMemory(&UserStandup, sizeof(UserStandup));

				//设置变量
				UserStandup.dwBattleNum = m_dwBattleNum;
				UserStandup.wChairID = pIServerUserItem->GetChairID();				
				UserStandup.dwUserID = pIServerUserItem->GetUserID();

				//发送消息		
				m_pITCPSocketBattle->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_USER_STANDUP, &UserStandup, sizeof(UserStandup), 0);
			}

			//解锁用户
			//if (pBattleUserItem && pBattleUserItem->wGameCount == 0)
			//{
			//	//构造用户
			//	DBR_GR_BattleUnlock BattleUnlock;
			//	ZeroMemory(&BattleUnlock, sizeof(BattleUnlock));

			//	//设置房号
			//	BattleUnlock.dwBattleNum = m_dwBattleNum;
			//	BattleUnlock.dwUserID[BattleUnlock.wUserCount++] = pBattleUserItem->dwUserID;					

			//	//投递数据
			//	WORD wStructSize = BattleUnlock.wUserCount * sizeof(BattleUnlock.dwUserID[0]);
			//	WORD wPacketSize = sizeof(BattleUnlock) - sizeof(BattleUnlock.dwUserID) + wStructSize;
			//	m_pIKernelDataBaseEngine->PostDataBaseRequest(m_dwBattleNum, DBR_GR_BATTLE_UNLOCK, 0L, 0L, &BattleUnlock, wPacketSize);					
			//}
		}
	}

	return true;
}

//旁观校验
bool CTableFrameBattle::EfficacyUserLookon(WORD wChairID, IServerUserItem* pIServerUserItem)
{
	//父类调用
	if (!__super::EfficacyUserLookon(wChairID, pIServerUserItem))
	{
		return false;
	}

	//约战状态
	if (m_bBattleService == false)
	{
		LPCTSTR pszDescribe = CWHService::FormatMessage(MIM_SERVER_0089, TEXT("系统维护中，约战房间暂时停止服务。请在维护完成后重试！"));
		SendRequestFailure(pIServerUserItem, pszDescribe, 0);
		return false;
	}

	//禁止旁观
	if (m_BattleConfig.cbLookonMode == LOOKON_MODE_FORBID)
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0091, TEXT("抱歉，当前约战房间不允许旁观！")), 0);
		return false;
	}
	

	return true;
}

//坐下校验
bool CTableFrameBattle::EfficacyUserSitdown(WORD wChairID, IServerUserItem* pIServerUserItem)
{
	if (!__super::EfficacyUserSitdown(wChairID, pIServerUserItem))
	{
		return false;
	}

	//用户信息
	auto pServerUserInfo = pIServerUserItem->GetUserInfo();

	//约战状态
	if (m_bBattleService == false)
	{
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0089, TEXT("系统维护中，约战房间暂时停止服务。请在维护完成后重试！")), 0);
		return false;
	}

	//未创建房间
	if (m_dwBattleNum == 0)
	{
		//发送消息
		SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0104, TEXT("抱歉，本房间尚未创建，您不能加入！")), RFC_NULL);

		return false;
	}

	//赢家支付
	if (m_BattleConfig.wPayType == PAY_TYPE_WINNER)
	{
		//房卡不够
		if (m_BattleConfig.cbCurrencyKind == CURRENCY_KIND_CARD && pServerUserInfo->lUserCard < m_BattleConfig.lConsumeCount)
		{
			//发送消息
			SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0107, L"抱歉，您身上的房卡不够，不能加入！"), RFC_NULL);

			return false;
		}		
	}

	//AA支付
	if (m_BattleConfig.wPayType == PAY_TYPE_USER && m_bDeductCost==false)
	{
		//金币不足
		if (m_BattleConfig.cbCurrencyKind == CURRENCY_KIND_GOLD &&  pServerUserInfo->lGold < m_BattleConfig.lConsumeCount)
		{
			//发送消息
			SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0106, L"抱歉，您身上的金币不够支付入场费，不能加入！"), RFC_NULL);

			return false;
		}

		//房卡不足
		if (m_BattleConfig.cbCurrencyKind == CURRENCY_KIND_CARD && pServerUserInfo->lUserCard < m_BattleConfig.lConsumeCount)
		{
			//发送消息
			SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0107, L"抱歉，您身上的房卡不够，不能加入！"), RFC_NULL);

			return false;
		}
	}	

	//同IP或GPS距离<100m
	if (m_BattleConfig.cbAllowSameIP == FALSE && m_wFinishCount == 0)
	{
		//变量定义
		auto pServerUserItem = (CServerUserItem*)pIServerUserItem;

		//同IP检测
		for (WORD i = 0; i < m_wChairCount; i++)
		{
			//获取用户
			auto pTableUserItem = (CServerUserItem*)GetTableUserItem(i);
			if (pTableUserItem == NULL) continue;

			//地址过滤				
			if (pServerUserItem == pTableUserItem) continue;
			if (pServerUserItem->GetClientIP() != pTableUserItem->GetClientIP()) continue;

			//发送消息
			SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0108, TEXT("由于您的IP跟已在桌的玩家相同，无法参与本次约战。")), RFC_NULL);

			return false;
		}

		//获取坐标
		auto dGPSLng1 = pServerUserItem->GetGlobalPosLng();
		auto dGPSLat1 = pServerUserItem->GetGlobalPosLat();

		//校验坐标
		if (IsGPSValid(dGPSLat1, dGPSLng1) == false)
		{
			//发送消息
			SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0109, TEXT("由于您的地理位置获取异常，无法参与本次约战，请打开GPS定位后再次尝试！")), RFC_NULL);

			return false;
		}

		//位置检测
		for (WORD i = 0; i < m_wChairCount; i++)
		{
			//获取用户
			CServerUserItem* pTableUserItem = (CServerUserItem*)GetTableUserItem(i);
			if (pTableUserItem == NULL) continue;

			//地址过滤				
			if (pServerUserItem == pTableUserItem) continue;

			//获取经纬度
			auto dGPSLng2 = pTableUserItem->GetGlobalPosLng();
			auto dGPSLat2 = pTableUserItem->GetGlobalPosLat();

			//计算距离
			if (GetGPSDistance(dGPSLat1, dGPSLng1, dGPSLat2, dGPSLng2) <= MIN_GPS_DISTANCE)
			{
				//发送消息
				LPCTSTR pszMessage = CWHService::FormatMessageV(MIM_SERVER_0110, TEXT("由于您的地理位置跟已在桌玩家的距离小于%d米，无法参与本次约战。"), MIN_GPS_DISTANCE);
				SendRequestFailure(pIServerUserItem, pszMessage, RFC_NULL);

				return false;
			}
		}
	}

	return true;
}

//起立校验
bool CTableFrameBattle::EfficacyUserStandup(WORD wChairID, IServerUserItem* pIServerUserItem, bool bInitiative)
{
	if (!__super::EfficacyUserStandup(wChairID, pIServerUserItem, bInitiative))
	{
		return false;
	}
	
	//用户属性
	BYTE cbUserStatus = pIServerUserItem->GetUserStatus();

	//约战托管处理
	if (bInitiative == true && GetBattleUserItem(pIServerUserItem->GetUserID())!=NULL)
	{
		//判断状态
		if (cbUserStatus != US_LOOKON && m_dwBattleNum != 0 && m_wFinishCount > 0)
		{
			//if (pIServerUserItem->IsTrusteeUser() == false)
			//{
			//	pIServerUserItem->SetTrusteeUser(true);
			//	pIServerUserItem->SetClientReady(true);

			//	//掉线通知
			//	if (m_pITableUserAction != NULL) m_pITableUserAction->OnActionUserOffLine(pIServerUserItem);
			//}

			if (pIServerUserItem->IsClientReady() == false)
			{
				pIServerUserItem->SetClientReady(true);
			}

			//SendRequestFailure(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0121, TEXT("抱歉，约战尚未完成，您暂时不能退出！")), 0);//kk guan

			//return false;//kk guan
		}
	}

	return true;
}

//时间事件
bool CTableFrameBattle::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	if (__super::OnEventTimer(dwTimerID, dwBindParameter) == false)
	{
		switch (dwTimerID)
		{
		case IDI_TABLE_DISMISS: 	//桌子解散
		{
			//解散游戏
			if (IsGameStarted() == true)
			{
				DismissGame();
			}

			//约战房间
			if (m_dwBattleNum != 0)
			{
				//超时解散
				if (m_wFinishCount==0)
				{
					//茶社的桌子不解散
					if (m_dwClubID != 0) return true;

					//构造消息
					TCHAR szMessage[128] = L"";
					StringCchCopy(szMessage, CountArray(szMessage), CWHService::FormatMessage(MIM_SERVER_0070, TEXT("房间等待时间已到，当前游戏人数不够！房间自动解散！")));

					//消息群发
					for (WORD i = 0; i < m_wChairCount; i++)
					{
						//获取用户
						IServerUserItem* pIServerUserItem = GetTableUserItem(i);
						if ((pIServerUserItem == NULL) || (pIServerUserItem->IsClientReady() == false)) continue;

						//发送数据							
						m_pIServiceFunction->SendGameMessage(pIServerUserItem, szMessage, SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);
					}									
				}	

				//约战完成
				BattleFinish(BFR_WAIT_EXPIRE);
			}

			return true;
		}
		case IDI_AGREEMENT_DISMISS:	//协议解散
		{
			//删除定时器
			KillGameTimer(IDI_AGREEMENT_DISMISS);

			for (WORD i = 0; i < m_wChairCount; i++)
			{
				//变量定义
				WORD wChairID = i;
				DWORD dwUserID = 0;

				//获取用户
				IServerUserItem* pIServerUserItem = GetTableUserItem(wChairID);
				if (pIServerUserItem != NULL) dwUserID = pIServerUserItem->GetUserID();

				//查找用户
				if (dwUserID == 0)
				{
					for (INT_PTR j = 0; j < m_BattleUserItemArray.GetCount(); j++)
					{
						if (wChairID == m_BattleUserItemArray[j].wChairID)
						{
							dwUserID = m_BattleUserItemArray[j].dwUserID;
							break;
						}
					}
				}

				//状态判断
				if (dwUserID != 0 && m_cbBallotResult[i] == 0)
				{
					//执行投票
					PerformBallotDismiss(wChairID, dwUserID, true);
				}
			}

			//设置变量
			m_dwLaunchTime = 0;
			m_dwLaunchUserID = 0;
			ZeroMemory(m_cbBallotResult, sizeof(m_cbBallotResult));

			return true;
		}
		}
	}

	return true;
}

//终端
VOID CTableFrameBattle::OnEventClientReady(IServerUserItem* pIServerUserItem)
{
	//校验编号
	if (m_dwBattleNum != 0)
	{
		//发送成绩
		NotifyTableGrade();

		//配置通知
		NotifyTableConfig(pIServerUserItem->GetChairID());

		//参数通知
		NotifyTableParam(pIServerUserItem->GetChairID());		
	}

	//解散提醒
	if (m_dwLaunchUserID != 0) SendDimissNotify(pIServerUserItem);
}

//桌子初始化
VOID CTableFrameBattle::OnEventTableFrameInit(tagTableFrameParameter& TableFrameParameter)
{
	//获取接口
	m_pITCPSocketBattle = TableFrameParameter.pITCPSocketService;
	m_pIEventGameOption = QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink, IEventGameOption);
	m_pIEventBattleItem = QUERY_OBJECT_PTR_INTERFACE(m_pITableFrameSink, IEventBattleItem);
}

//桌子参数
bool CTableFrameBattle::NotifyTableParam(WORD wChairID)
{
	//变量定义
	CMD_GF_TableParam GFTableParam;
	ZeroMemory(&GFTableParam, sizeof(GFTableParam));

	//桌子信息
	GFTableParam.lBaseScore = m_lBaseScore;
	GFTableParam.dwOwnerID = m_dwTableOwnerID;
	GFTableParam.dwBattleNum = m_dwBattleNum;

	//结算信息
	GFTableParam.wFinishCount = m_wFinishCount;
	GFTableParam.wPlayCount = m_BattleConfig.wPlayCount;

	//记录标识
	GFTableParam.lBattleRecordID = m_lBattleRecordID;

	//游戏用户
	SendTableData(wChairID, SUB_GF_TABLE_PARAM, &GFTableParam, sizeof(GFTableParam), MDM_GF_FRAME);

	//旁观用户
	SendLookonData(wChairID, SUB_GF_TABLE_PARAM, &GFTableParam, sizeof(GFTableParam), MDM_GF_FRAME);

	//状态判断
	if ((m_BattleConfig.wNotifyMask & NOTIFY_MASK_TABLE_PARAM) != 0 && m_wFinishCount > 0 && wChairID==INVALID_CHAIR)
	{
		//构造结构
		CMD_BS_S_TableParam SBTableParam;
		ZeroMemory(&SBTableParam, sizeof(SBTableParam));

		//桌子信息		
		SBTableParam.dwBattleNum = m_dwBattleNum;
		SBTableParam.dwStartTime = m_dwStartTime;
		SBTableParam.wFinishCount = m_wFinishCount;
		SBTableParam.cbGameStatus = IsGameStarted() ? TRUE : FALSE;

		//发送数据
		m_pITCPSocketBattle->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_TABLE_PARAM, &SBTableParam, sizeof(SBTableParam), 0);
	}

	return true;
}

//约战配置
bool CTableFrameBattle::NotifyTableConfig(WORD wChairID)
{
	//变量定义
	CMD_GF_TableConfig TableConfig = {};
	
	//桌子信息
	TableConfig.dwBattleNum = m_dwBattleNum;
	TableConfig.lWinnerScore = m_BattleConfig.lWinnerScore;
	TableConfig.lConsumeCount = m_BattleConfig.lConsumeCount;
	CopyMemory(&TableConfig.BattleConfig, &m_BattleConfig, sizeof(TableConfig.BattleConfig));

	//游戏用户
	SendTableData(wChairID, SUB_GF_TABLE_CONFIG, &TableConfig, sizeof(TableConfig), MDM_GF_FRAME);

	//旁观用户	
	SendLookonData(wChairID, SUB_GF_TABLE_CONFIG, &TableConfig, sizeof(TableConfig), MDM_GF_FRAME);

	return true;
}

//桌子成绩
bool CTableFrameBattle::NotifyTableGrade()
{
	//判断数量
	if (m_BattleUserItemArray.GetCount() == 0)
	{
		return true;
	}

	//禁止发送成绩
	if (CBattleRule::IsForbidSendGrade(m_pGameServiceOption->dwBattleRule))
	{
		return true;
	}

	//网络数据
	WORD wPacketSize = 0;
	BYTE cbDataBuffer[TABLE_USER_COUNT * sizeof(tagUserGradeItem)];

	//发送数据
	for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
	{
		if (wPacketSize + sizeof(tagUserGradeItem) > sizeof(cbDataBuffer))
		{
			//发送数据
			SendTableData(INVALID_CHAIR, SUB_GF_TABLE_GRADE, cbDataBuffer, wPacketSize, MDM_GF_FRAME);

			//发给旁观
			SendLookonData(INVALID_CHAIR, SUB_GF_TABLE_GRADE, cbDataBuffer, wPacketSize, MDM_GF_FRAME);

			wPacketSize = 0;
		}

		//提取数据
		tagUserGradeItem* pUserGradeItem = (tagUserGradeItem*)(cbDataBuffer + wPacketSize);

		//设置成绩
		pUserGradeItem->wChairID = m_BattleUserItemArray[i].wChairID;
		pUserGradeItem->dwUserID = m_BattleUserItemArray[i].dwUserID;
		pUserGradeItem->lAddupScore = m_BattleUserItemArray[i].lScore;
		StringCchCopy(pUserGradeItem->szNickName, CountArray(pUserGradeItem->szNickName), m_BattleUserItemArray[i].szNickName);

		//位置偏移
		wPacketSize += sizeof(tagUserGradeItem);
	}

	//发送数据
	if (wPacketSize > 0)
	{
		//发送数据
		SendTableData(INVALID_CHAIR, SUB_GF_TABLE_GRADE, cbDataBuffer, wPacketSize, MDM_GF_FRAME);

		//发给旁观
		SendLookonData(INVALID_CHAIR, SUB_GF_TABLE_GRADE, cbDataBuffer, wPacketSize, MDM_GF_FRAME);
	}

	return true;
}

//约战战绩
bool CTableFrameBattle::NotifyTableBattle(bool bRecordBattle)
{
	//判断数量
	if (m_BattleUserItemArray.GetCount() == 0)
	{
		return true;
	}

	//构造用户
	DBR_GR_BattleUnlock BattleUnlock;
	ZeroMemory(&BattleUnlock, sizeof(BattleUnlock));

	//设置房号
	BattleUnlock.dwBattleNum = m_dwBattleNum;

	//移除用户
	for (INT_PTR i = m_BattleUserItemArray.GetCount() - 1; i >= 0; i--)
	{
		//获取对象
		tagBattleUserItem* pBattleUserItem = &m_BattleUserItemArray[i];

		//添加用户
		BattleUnlock.dwUserID[BattleUnlock.wUserCount++] = pBattleUserItem->dwUserID;
	}

	//锁定用户
	if (BattleUnlock.wUserCount > 0)
	{
		//投递数据
		WORD wStructSize = BattleUnlock.wUserCount * sizeof(BattleUnlock.dwUserID[0]);
		WORD wPacketSize = sizeof(BattleUnlock) - sizeof(BattleUnlock.dwUserID) + wStructSize;
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_BATTLE_UNLOCK, m_dwBattleNum, 0L, 0L, &BattleUnlock, wPacketSize);
	}

	//kk jia start
		//计算税收
	//战绩记录
	if (bRecordBattle && CBattleRule::IsForbidBattleRecord(m_pGameServiceOption->dwBattleRule) == false)//kk 
	{
		//只扣大赢家
		WORD wUserCount = (WORD)m_BattleUserItemArray.GetCount();
		BYTE bDaWin = 0;
		SCORE lDaWinScore = m_BattleUserItemArray[0].lScore;
		for (BYTE i = 0; i < wUserCount; i++)
		{
			SCORE lScore = m_BattleUserItemArray[i].lScore;
			lDaWinScore = m_BattleUserItemArray[bDaWin].lScore;
			if (lScore > lDaWinScore)
			{
				bDaWin = i;

			}
		}	
		
		SCORE lScore = m_BattleUserItemArray[bDaWin].lScore;
		if (lScore > 0)
		{
			SCORE lClubRevenue = CalcCustomRevenue(lScore);
			m_BattleUserItemArray[bDaWin].lClubRevenue = lClubRevenue;
			m_BattleUserItemArray[bDaWin].lScore -= lClubRevenue;

			//
			IServerUserItem* pIServerUserItem = GetTableUserItem(m_BattleUserItemArray[bDaWin].wChairID);
			if (pIServerUserItem != NULL)
			{
				//CString s;
				//s.Format(_T("a17_1  i:%d,lScore:%I64d,lClubRevenue:%I64d"), bDaWin, pIServerUserItem->GetUserScore(),  lClubRevenue);
				//CTraceService::TraceString(s, TraceLevel_Exception);

				pIServerUserItem->WriteUserScore(-lClubRevenue,0,0,0, ScoreType_Null);

				//s.Format(_T("a17_2  i:%d,lScore:%I64d,lClubRevenue:%I64d"), bDaWin, pIServerUserItem->GetUserScore(), lClubRevenue);
				//CTraceService::TraceString(s, TraceLevel_Exception);
			}

			/*CString s;
			s.Format(_T("a17  i:%d,lScore:%I64d,m_BattleUserItemArray[i].lScore:%I64d,lClubRevenue:%I64d, lRevenue:%I64d,m_BattleUserItemArray[i].lClubRevenue:%I64d"), i, lScore, m_BattleUserItemArray[i].lScore, lClubRevenue, m_BattleUserItemArray[i].lClubRevenue);
			CTraceService::TraceString(s, TraceLevel_Exception);*/
		}

		
	}
	//kk jia end
	//补写分数
	if (CServerRule::IsImmediateWriteScore(m_pGameServiceOption->dwServerRule) == false)
	{
		//变量定义
		DBR_GR_WriteGameScore WriteGameScore;
		ZeroMemory(&WriteGameScore, sizeof(WriteGameScore));

		//锁定信息
		tagLockScoreInfo LockScoreInfo;
		ZeroMemory(&LockScoreInfo, sizeof(LockScoreInfo));
		LockScoreInfo.lMaxLockScore = m_pGameServiceOption->lMaxLockScore;
		LockScoreInfo.lLessLockScore = m_pGameServiceOption->lLessLockScore;
		LockScoreInfo.lRestrictScore = m_pGameServiceOption->lRestrictScore;

		//遍历用户
		for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
		{
			//获取用户
			IServerUserItem* pIServerUserItem = GetTableUserItem(m_BattleUserItemArray[i].wChairID);
			if (pIServerUserItem != NULL)
			{
				//提取积分
				tagVariationScore VariationScore;
				ZeroMemory(&VariationScore, sizeof(VariationScore));
				pIServerUserItem->DistillVariation(VariationScore, LockScoreInfo);

				//用户信息
				WriteGameScore.dwUserID = pIServerUserItem->GetUserID();
				WriteGameScore.dwClientIP = pIServerUserItem->GetClientIP();
				WriteGameScore.dwPlayTimeCount = VariationScore.dwPlayTimeCount;

				//积分信息
				WriteGameScore.lRevenue = VariationScore.lRevenue;//kk
				WriteGameScore.lUnLockScore = VariationScore.lUnLockScore;

				//积分信息
				WriteGameScore.lScore = VariationScore.VariationScore.lScore;
				WriteGameScore.lWinCount = VariationScore.VariationScore.lWinCount;
				WriteGameScore.lLostCount = VariationScore.VariationScore.lLostCount;
				WriteGameScore.lDrawCount = VariationScore.VariationScore.lDrawCount;
				WriteGameScore.lFleeCount = VariationScore.VariationScore.lFleeCount;
				WriteGameScore.lExperience = VariationScore.VariationScore.lExperience;
				WriteGameScore.lWinStreak = VariationScore.VariationScore.lWinStreak;
				WriteGameScore.lMaxWinStreak = VariationScore.VariationScore.lMaxWinStreak;

				//投递请求
				m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_WRITE_GAME_SCORE, WriteGameScore.dwUserID, 0, pIServerUserItem->GetTokenID(), &WriteGameScore, sizeof(WriteGameScore));
			}
		}
	}

	//分数排名(未退费)
	if (m_bReturnCost == false)
	{
		//变量定义	
		CBattleUserItemArray BattleUserItemArray;

		//交换数据
		BattleUserItemArray.Append(m_BattleUserItemArray);
		m_BattleUserItemArray.RemoveAll();

		//成绩排名	
		WORD wRankCount = (WORD)BattleUserItemArray.GetCount();
		for (WORD wRankID = 1; wRankID <= wRankCount; wRankID++)
		{
			//变量定义		
			INT_PTR nIndex = 0;
			LONGLONG lScore = BattleUserItemArray[0].lScore;

			//查找高分
			for (INT_PTR i = 1; i < BattleUserItemArray.GetCount(); i++)
			{
				//分数不等
				if (BattleUserItemArray[i].lScore > lScore)
				{
					//设置变量
					nIndex = i;
					lScore = BattleUserItemArray[i].lScore;
				}

				//分数相等
				if (BattleUserItemArray[i].lScore == lScore)
				{
					//胜局不等
					if (BattleUserItemArray[i].lWinCount > BattleUserItemArray[nIndex].lWinCount)
					{
						//设置变量
						nIndex = i;
						lScore = BattleUserItemArray[i].lScore;
					}
				}
			}

			//设置名次		
			BattleUserItemArray[nIndex].wRankID = wRankID;
			BattleUserItemArray[nIndex].cbUserStatus = US_NULL;

			//获取用户
			IServerUserItem* pIServerUserItem = GetTableUserItem(BattleUserItemArray[nIndex].wChairID);
			if (pIServerUserItem != NULL)
			{
				//设置状态
				BattleUserItemArray[nIndex].cbUserStatus = pIServerUserItem->GetUserStatus();
			}

			//添加用户
			m_BattleUserItemArray.Add(BattleUserItemArray[nIndex]);

			//移除记录
			BattleUserItemArray.RemoveAt(nIndex);
		}
	}

	
	//房卡支付-大赢家支付
	if (m_BattleConfig.cbCurrencyKind == CURRENCY_KIND_CARD && m_BattleConfig.wPayType == PAY_TYPE_WINNER)
	{
		//获取大赢家
		auto pBattleUserItem = &m_BattleUserItemArray[0];
		if (pBattleUserItem != NULL && pBattleUserItem->wRankID == 1)
		{
			//构造结构
			DBR_GR_DeductBattleCost DeductBattleCost;
			ZeroMemory(&DeductBattleCost, sizeof(DeductBattleCost));

			//开桌参数
			DeductBattleCost.wTableID = m_wTableID;
			DeductBattleCost.dwClubID = m_dwClubID;
			DeductBattleCost.wPayType = m_BattleConfig.wPayType;
			DeductBattleCost.dwUserID = pBattleUserItem->dwUserID;
			DeductBattleCost.lConsumeCount = m_BattleConfig.lConsumeCount;
			DeductBattleCost.cbCurrencyKind = m_BattleConfig.cbCurrencyKind;

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DEDUCT_BATTLE_COST, DeductBattleCost.dwUserID, 0L, 0L, &DeductBattleCost, sizeof(DeductBattleCost));
		}
	}		

	//金币支付-赢家奖励
	if (m_BattleConfig.cbCurrencyKind == CURRENCY_KIND_GOLD && m_BattleConfig.wPayType == PAY_TYPE_USER)
	{
		//构造结构
		DBR_GR_BattleReward BattleReward = {};

		//计算税收
		auto pAdvancedOption = (tagAdvancedOption*)m_BattleConfig.cbAdvancedOption;
		auto lClubRevenue = (m_BattleConfig.lConsumeCount * m_BattleConfig.wPlayerCount) * pAdvancedOption->lRevenueRate / 1000;

		CString s;
		s.Format(_T("aaa1 lClubRevenue:%I64d , m_BattleConfig.lConsumeCount:%I64d , m_BattleConfig.wPlayerCount:%d"), lClubRevenue , m_BattleConfig.lConsumeCount , m_BattleConfig.wPlayerCount);
		CTraceService::TraceString(s, TraceLevel_Debug); 

		auto lSysRevenue =  (m_BattleConfig.lConsumeCount * m_BattleConfig.wPlayerCount)- m_BattleConfig.lWinnerScore*m_BattleConfig.wWinnerCount - lClubRevenue;

		for (int i = 0; i < m_BattleUserItemArray.GetCount(); i++)
		{
			auto pBattleUserItem = &m_BattleUserItemArray[i];
			if (pBattleUserItem == NULL || pBattleUserItem->wRankID != i + 1) continue;			

			//设置变量
			BattleReward.dwUserID = pBattleUserItem->dwUserID;
			BattleReward.lRewardGold = m_BattleConfig.lWinnerScore;

			//奖励分数
			pBattleUserItem->lRewardScore = m_BattleConfig.lWinnerScore;

			//计算税收
			pBattleUserItem->lClubRevenue = lClubRevenue/m_BattleConfig.wWinnerCount;
			pBattleUserItem->lSysRevenue = lSysRevenue/m_BattleConfig.wWinnerCount;

			//投递请求
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_BATTLE_REWARD, BattleReward.dwUserID, 0, 0, &BattleReward, sizeof(BattleReward));

			//退出判断
			if (pBattleUserItem->wRankID >= m_BattleConfig.wWinnerCount) break;
		}
	}

	//战绩记录
	if (bRecordBattle && CBattleRule::IsForbidBattleRecord(m_pGameServiceOption->dwBattleRule) == false)//kk 
	{
		//构造结构
		DBR_GR_BattleRecord BattleRecord;
		ZeroMemory(&BattleRecord, sizeof(BattleRecord));
		
		//约战信息
		BattleRecord.dwBattleNum = m_dwBattleNum;
		BattleRecord.dwOwnerID = m_dwTableOwnerID;
		BattleRecord.bBattleFinish = IsBattleFinished();
		BattleRecord.lBattleRecordID = m_lBattleRecordID;

		//群组信息
		BattleRecord.dwClubID = m_dwClubID;
		BattleRecord.dwClubCreaterID = m_dwClubCreaterID;

		//支付信息
		BattleRecord.wPayType = m_BattleConfig.wPayType;  //lClubRevenue  BattleUserItem
		BattleRecord.dwPayUserID = m_BattleConfig.dwPayUserID;

		//创建信息
		BattleRecord.dwConfigID = m_dwConfigID;
		BattleRecord.dwCreateTime = m_dwStartTime;	
		BattleRecord.wFinishCount = m_wFinishCount;
		BattleRecord.wPlayCount = m_BattleConfig.wPlayCount;
		BattleRecord.lBaseScore = m_BattleConfig.lBaseScore;
		BattleRecord.lWinnerScore = m_BattleConfig.lWinnerScore;
		BattleRecord.cbScoreKind = m_BattleConfig.cbScoreKind;
		BattleRecord.cbSettleKind = m_BattleConfig.cbSettleKind;
		BattleRecord.cbAllowPublic = m_BattleConfig.cbAllowPublic;
		BattleRecord.lConsumeCount = m_BattleConfig.lConsumeCount;
		BattleRecord.cbCurrencyKind = m_BattleConfig.cbCurrencyKind;		
		

		//变量定义
		CString strOptionItem;
		CString strGameOption = TEXT("");

		//普通配置
		for (BYTE i = 0; i < m_BattleConfig.cbOptionSize; i++)
		{
			strOptionItem.Format(TEXT("%02X"), m_BattleConfig.cbGameOption[i]);
			/*if (strGameOption.IsEmpty() == false)
			{
				strGameOption.Append(TEXT("|"));
			}*/
			strGameOption.Append(strOptionItem);
		}

		//游戏配置
		StringCchCopy(BattleRecord.szGameOption, CountArray(BattleRecord.szGameOption), strGameOption.GetString());		

		//分数信息
		BattleRecord.wUserCount = (WORD)m_BattleUserItemArray.GetCount();
		CopyMemory(BattleRecord.BattleUserItem, m_BattleUserItemArray.GetData(), BattleRecord.wUserCount * sizeof(tagBattleUserItem));

		//投递请求
		WORD wPacketSize = BattleRecord.wUserCount * sizeof(tagBattleUserItem);
		wPacketSize += sizeof(BattleRecord) - sizeof(BattleRecord.BattleUserItem);
		m_pIAssistDataBaseEngine->PostDataBaseRequest(DBR_GR_BATTLE_RECORD, 0, 0, 0, &BattleRecord, wPacketSize);
	}

	//网络数据
	WORD wPacketSize = 0;
	BYTE cbDataBuffer[sizeof(tagBattleScoreItem) * TABLE_USER_COUNT];

	//发送数据
	for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
	{
		//约战摘要
		if (m_pIEventBattleItem != NULL)
		{
			auto pszBattleAbstract = m_pIEventBattleItem->GetUserBattleAbstract(m_BattleUserItemArray[i].wChairID);
			if (pszBattleAbstract != NULL)
			{
				StringCchCopy(m_BattleUserItemArray[i].szBattleAbstract, CountArray(m_BattleUserItemArray[i].szBattleAbstract), m_pIEventBattleItem->GetUserBattleAbstract(m_BattleUserItemArray[i].wChairID));
			}
		}		

		//提取数据
		auto pUserScoreItem = (tagBattleScoreItem*)(cbDataBuffer + wPacketSize);
		CopyMemory(pUserScoreItem, &m_BattleUserItemArray[i], sizeof(tagBattleScoreItem));

		//位置偏移
		wPacketSize += sizeof(tagBattleScoreItem);
	}

	//发送数据
	if (wPacketSize > 0)
	{
		//发给游戏玩家
		SendTableData(INVALID_CHAIR, SUB_GF_TABLE_BATTLE, cbDataBuffer, wPacketSize, MDM_GF_FRAME);

		//发给旁观玩家
		SendLookonData(INVALID_CHAIR, SUB_GF_TABLE_BATTLE, cbDataBuffer, wPacketSize, MDM_GF_FRAME);
	}

	return true;
}

//用户列表
bool CTableFrameBattle::NotifyTableUserList()
{
	//状态判断
	//if (m_wFinishCount==0) return false;

	//绑定用户
	if (CBattleRule::IsForbidBindUser(m_pGameServiceOption->dwBattleRule)) return false;

	//构造结构
	CMD_BS_S_TableUserList TableUserList;
	ZeroMemory(&TableUserList, sizeof(TableUserList));

	//设置变量
	TableUserList.dwBattleNum = m_dwBattleNum;

	//变量定义
	tagBattleTableUser* pTableUserItem = NULL;

	//设置用户
	for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
	{
		//获取用户
		IServerUserItem* pIServerUserItem = GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL) continue;
		if (wChairID >= CountArray(TableUserList.TableUserItem)) break;

		//获取对象
		TableUserList.wUserCount++;
		pTableUserItem = &TableUserList.TableUserItem[wChairID];

		//设置变量
		pTableUserItem->wChairID = wChairID;
		pTableUserItem->dwGameID = pIServerUserItem->GetGameID();
		pTableUserItem->dwUserID = pIServerUserItem->GetUserID();
		pTableUserItem->cbGender = pIServerUserItem->GetUserInfo()->cbGender;		
		pTableUserItem->wFaceID = pIServerUserItem->GetUserInfo()->wFaceID;
		pTableUserItem->dwCustomID = pIServerUserItem->GetUserInfo()->dwCustomID;
		pTableUserItem->wGameOrder = g_GameLevelManager->CalcGameOrder(pIServerUserItem->GetExperience());
		pTableUserItem->wMemberOrder = g_GameLevelManager->CalcUserOrder(pIServerUserItem->GetMemberPoint());
		StringCchCopy(pTableUserItem->szNickName, CountArray(pTableUserItem->szNickName), pIServerUserItem->GetAccounts());
	}

	//发送数据	
	m_pITCPSocketBattle->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_TABLE_USERLIST, &TableUserList, sizeof(TableUserList), 0);

	return true;
}

//尝试解散
bool CTableFrameBattle::IsBattleFinished()
{
	//局数结算
	if (m_BattleConfig.cbSettleKind == SETTLE_KIND_COUNT)
	{
		if (m_wFinishCount >= m_BattleConfig.wPlayCount)
		{
			return true;
		}
	}

	//自定义结算
	if (m_BattleConfig.cbSettleKind == SETTLE_KIND_CUSTOM)
	{
		return m_bBattleFinish;
	}

	return false;
}

//解散桌子
bool CTableFrameBattle::DismissBattleTable(DWORD dwUserID)
{
	//状态判断
	if (m_dwBattleNum == 0)
	{
		return false;
	}

	//变量定义
	bool bGameStarted = IsGameStarted();

	//退费校验
	if (m_BattleConfig.lConsumeCount > 0 && (m_wFinishCount == 0 || (m_wFinishCount == 1 && bGameStarted == true)))
	{
		//退还费用
		if (m_bDeductCost == true)
		{
			PerformReturnCost(m_BattleConfig.wPayType);
		}
		else
		{
			m_bReturnCost = true;	
		}
	}

	//变量定义
	WORD wEnumIndex = 0;
	TCHAR szMessage[128] = L"";
	_snwprintf_s(szMessage, CountArray(szMessage), CWHService::FormatMessage(MIM_SERVER_0067, TEXT("房主已解散当前房间，请重新约战！")));

	//旁观玩家
	do
	{
		//获取用户
		auto pIServerUserItem = EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem == NULL) break;

		//效验状态
		if (pIServerUserItem->IsClientReady() == false) continue;

		//断开连接
		m_pIServiceFunction->SendGameMessage(pIServerUserItem, szMessage, SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);

	} while (true);

	//在座玩家
	for (WORD i = 0; i < m_wChairCount; i++)
	{
		//获取用户
		IServerUserItem* pIServerUserItem = GetTableUserItem(i);
		if (pIServerUserItem == NULL) continue;
		if (pIServerUserItem->GetUserID() == dwUserID) continue;
		if (pIServerUserItem->IsClientReady() == false) continue;

		//发送数据
		if (m_wFinishCount == 0)
		{
			m_pIServiceFunction->SendGameMessage(pIServerUserItem, szMessage, SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);
		}
		else
		{
			m_pIServiceFunction->SendGameMessage(pIServerUserItem, szMessage, SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE);
		}
	}

	//解散游戏
	if (bGameStarted == true) DismissGame();

	//解散桌子
	if (m_dwBattleNum != 0) DismissBattleTable(!m_bReturnCost);

	return true;
}

//解散桌子
void CTableFrameBattle::DismissBattleTable(bool bRecordBattle)
{

	//CTraceService::TraceString(_T("解散桌子"), TraceLevel_Exception);
	//状态判断
	if (m_dwBattleNum == 0) return;

	//发送战绩
	NotifyTableBattle(bRecordBattle);

	//变量定义
	CMD_BS_S_TableDismiss TableDismiss;
	ZeroMemory(&TableDismiss, sizeof(TableDismiss));

	//桌子信息	
	TableDismiss.wTableID = m_wTableID;
	TableDismiss.dwBattleNum = m_dwBattleNum;
	TableDismiss.wServerID = m_pGameServiceOption->wServerID;

	//发送数据
	m_pITCPSocketBattle->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_TABLE_DISMISS, &TableDismiss, sizeof(TableDismiss), 0);

	//设置变量	
	m_dwPlayCount = 0;
	m_dwStartTime = 0;
	m_wFinishCount = 0;
	m_dwBattleRule = 0;
	m_lBattleRecordID = 0;
	m_bBattleFinish = false;
	m_cbBattleState = BATTLE_STATE_FREE;

	//设置变量
	m_dwLaunchTime = 0;
	m_dwLaunchUserID = 0;
	ZeroMemory(m_cbBallotResult, sizeof(m_cbBallotResult));

	//清理数据
	m_dwConfigID = 0;
	m_dwBattleNum = 0;
	m_dwTableOwnerID = 0;
	m_szTablePassword[0] = 0;	
	ZeroMemory(&m_BattleConfig, sizeof(m_BattleConfig));

	//发送状态
	SendTableStatus();

	//设置底分
	SetCellScore(m_pGameServiceOption->lCellScore);

	//解散通知
	if (m_pIEventBattleItem) m_pIEventBattleItem->OnEventTableDismiss();

	//移除元素
	m_BattleUserItemArray.RemoveAll();

	//关闭定时器
	KillGameTimer(IDI_TABLE_DISMISS);
	KillGameTimer(IDI_KICKOUT_PLAYER);
	KillGameTimer(IDI_KICKOUT_LOOKON);
	KillGameTimer(IDI_AGREEMENT_DISMISS);

	//用户处理
	for (WORD wChairID = 0; wChairID < m_wChairCount; wChairID++)
	{
		//获取用户
		IServerUserItem* pIServerUserItem = GetTableUserItem(wChairID);
		if (pIServerUserItem == NULL) continue;

		//托管用户
		if (pIServerUserItem->IsTrusteeUser() == true)
		{
			//用户起立
			PerformStandUpAction(pIServerUserItem, true);

			//踢出房间
			if (pIServerUserItem->GetUserStatus() != US_NULL)
			{
				pIServerUserItem->SetUserStatus(US_NULL, INVALID_TABLE, INVALID_CHAIR);
			}
		}
		else
		{
			//校验服务状态
			if (m_bBattleService == false)
			{
				//断开连接
				m_pIServiceFunction->SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0078, TEXT("系统即将维护，约战房间暂时停止服务。请在维护完成后重试！")), SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);
			}
		}
	}

	//变量定义
	WORD wEnumIndex = 0;
	IServerUserItem* pIServerUserItem = NULL;

	//旁观用户
	do
	{
		//获取用户
		pIServerUserItem = EnumLookonUserItem(wEnumIndex++);
		if (pIServerUserItem == NULL) break;

		//效验状态
		if (pIServerUserItem->IsClientReady() == false) continue;

		//校验服务状态
		if (m_bBattleService == false)
		{
			//断开连接
			m_pIServiceFunction->SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0078, TEXT("系统即将维护，约战房间暂时停止服务。请在维护完成后重试！")), SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);
		}
		else
		{
			//断开连接
			m_pIServiceFunction->SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0122, TEXT("当前所旁观的房间已解散！")), SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);
		}

	} while (true);
}

//开启桌子
bool CTableFrameBattle::CreateBattleTable(tagBattleCreateParam& CreateParam)
{
	//拷贝配置
	CopyMemory(&m_BattleConfig, &CreateParam.BattleConfig, sizeof(m_BattleConfig));

	//if (m_pIEventBattleItem != NULL)
		//CTraceService::TraceString(_T("m_pIEventBattleItem is not null"), TraceLevel_Exception);

	//校验配置
	if (VerifyBattleConfig(&m_BattleConfig, false) == false)
	{
		CTraceService::TraceString(_T("VerifyBattleConfig is false"), TraceLevel_Exception);
		return false;
	}
	//else
		//CTraceService::TraceString(_T("VerifyBattleConfig is false"), TraceLevel_Exception);

	//设置变量
	m_dwPlayCount = 0;
	m_dwStartTime = 0;
	m_wFinishCount = 0;
	m_dwBattleRule = 0;
	m_bDeductCost = false;
	m_bBattleFinish = false;
	m_bReturnCost = false;
	m_bDismissNotify = false;
	m_cbBattleState = BATTLE_STATE_WAIT;
	m_dwConfigID = CreateParam.dwConfigID;
	m_dwBattleNum = CreateParam.dwBattleNum;
	m_dwCreateTime = CreateParam.dwCreateTime;
	m_dwTableOwnerID = CreateParam.dwOwnerID;
	StringCchCopy(m_szTablePassword, CountArray(m_szTablePassword), CreateParam.szPassword);

	//记录标识
	m_lBattleRecordID = BuildBattlleRecordID();

	//群组信息
	m_dwClubID = CreateParam.dwClubID;
	m_wSwitchOption = CreateParam.wSwitchOption;
	m_wClubServiceID = CreateParam.wClubServiceID;
	m_dwClubCreaterID = CreateParam.dwClubCreaterID;

	//功能标志
	m_bSameIpRemind = !CBattleRule::IsForbidSameIPRemind(m_pGameServiceOption->dwBattleRule);

	//设置变量
	m_dwLaunchTime = 0;
	m_dwLaunchUserID = 0;
	ZeroMemory(m_cbBallotResult, sizeof(m_cbBallotResult));

	//设置人数
	SetMaxPlayerCount(m_BattleConfig.wPlayerCount);

	//设置变量
	m_ChargeUserArray.RemoveAll();
	m_BattleUserItemArray.RemoveAll();

	//发送状态
	SendTableStatus();

	//设置底分
	SetCellScore(m_BattleConfig.lBaseScore);

	//创建通知
	if (m_pIEventBattleItem != NULL)
	{
		//创建通知
		m_pIEventBattleItem->OnEventTableCreated(&CreateParam, sizeof(tagBattleCreateParam));
	}

	//解散时间
	DWORD dwDismissTime = TIME_BATTLE_TABLE_DISMISS;
	if (m_pIEventCustomFrameTime != NULL)
	{
		dwDismissTime = m_pIEventCustomFrameTime->GetBattleDismissTime();
	}

	//时间间隔
	if (dwDismissTime > 0)
	{
		DWORD dwTimeSpace = (DWORD)time(NULL) - m_dwCreateTime;
		if (dwTimeSpace >= dwDismissTime)
		{
			SetGameTimer(IDI_TABLE_DISMISS, dwDismissTime * 1000L, -1, 0);
		}
		else
		{
			SetGameTimer(IDI_TABLE_DISMISS, (dwDismissTime - dwTimeSpace) * 1000L, -1, 0);
		}
	}

	return true;
}

//修改桌子
bool CTableFrameBattle::ModifyBattleTable(tagBattleModifyParam& ModifyParam)
{
	//校验编号
	if (ModifyParam.dwBattleNum != m_dwBattleNum)
	{
		return false;
	}

	//校验桌主
	if (m_dwClubID==0 && ModifyParam.dwUserID != m_dwTableOwnerID)
	{
		return false;
	}

	//校验馆主
	if (m_dwClubID!=0 && ModifyParam.dwUserID != m_dwClubCreaterID)
	{
		return false;
	}

	//校验配置	
	if (VerifyBattleConfig(&ModifyParam.BattleConfig, true) == false)
	{
		return false;
	}

	//更新配置	
	CopyMemory(&m_BattleConfig, &ModifyParam.BattleConfig, sizeof(m_BattleConfig));

	//设置配置
	VerifyBattleConfig(&m_BattleConfig, false);

	//修改通知
	if (m_pIEventBattleItem != NULL)
	{
		//修改通知
		m_pIEventBattleItem->OnEventTableModify(&ModifyParam, sizeof(ModifyParam));
	}

	//配置通知
	NotifyTableConfig();

	return true;
}

//桌子信息
void CTableFrameBattle::GetBattleTableItem(tagBattleTableItemEx& TableItem)
{
	//设置变量
	ZeroMemory(&TableItem, sizeof(TableItem));

	//桌子信息
	TableItem.wTableID = m_wTableID;
	TableItem.dwBattleNum = m_dwBattleNum;
	TableItem.dwOwnerID = m_dwTableOwnerID;
	TableItem.dwClubID = m_dwClubID;
	TableItem.wSwitchOption = m_wSwitchOption;
	TableItem.wClubServiceID = m_wClubServiceID;
	TableItem.dwClubCreaterID = m_dwClubCreaterID;	
	TableItem.wServerID = m_pGameServiceOption->wServerID;
	StringCchCopy(TableItem.szPassword, CountArray(TableItem.szPassword), m_szTablePassword);

	//创建信息  
	TableItem.dwConfigID = m_dwConfigID;
	TableItem.dwCreateTime = m_dwCreateTime;	
	CopyMemory(&TableItem.BattleConfig, &m_BattleConfig, sizeof(TableItem.BattleConfig));
}

//设置状态
void CTableFrameBattle::SetBattleService(bool bBattleService)
{
	//设置状态
	m_bBattleService = bBattleService;

	//停止服务
	if (m_bBattleService == false)
	{
		//变量定义
		WORD wEnumIndex = 0;
		IServerUserItem* pIServerUserItem = NULL;

		//旁观玩家
		do
		{
			//获取用户
			pIServerUserItem = EnumLookonUserItem(wEnumIndex++);
			if (pIServerUserItem == NULL) break;

			//效验状态
			if (pIServerUserItem->IsClientReady() == false) continue;

			//断开连接
			m_pIServiceFunction->SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0078, TEXT("系统即将维护，约战房间暂时停止服务。请在维护完成后重试！")), SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);

		} while (true);

		//在坐玩家
		for (WORD i = 0; i < m_wChairCount; i++)
		{
			//获取用户
			IServerUserItem* pIServerUserItem = GetTableUserItem(i);
			if (pIServerUserItem == NULL) continue;

			if (m_wFinishCount == 0)
			{
				m_pIServiceFunction->SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0078, TEXT("系统即将维护，约战房间暂时停止服务。请在维护完成后重试！")), SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);
			}
			else
			{
				m_pIServiceFunction->SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0079, TEXT("系统即将维护，本局游戏完成后，暂时无法进入房间，请在维护完成后重试！")), SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE);
			}
		}
	}
}

//执行投票
bool CTableFrameBattle::PerformBallotDismiss(WORD wChairID, DWORD dwUserID, bool bAgreeDismiss)
{
	//参数校验
	ASSERT(wChairID < m_wChairCount);
	if (wChairID >= m_wChairCount) return false;

	//投票结果			
	m_cbBallotResult[wChairID] = bAgreeDismiss ? 1 : 2;

	//构造结构
	CMD_GF_DismissBallot DismissBallot;
	ZeroMemory(&DismissBallot, sizeof(DismissBallot));

	//设置变量
	DismissBallot.dwUserID = dwUserID;
	DismissBallot.bAgreeDismiss = bAgreeDismiss;

	//数量定义
	WORD wAgreeCount = 0, wDisAgreeCount = 0;
	WORD wNeedAgreeCount = 0, wBallotUserCount = 0;

	//变量定义
	tagBattleUserItem* pBattleUserItem = NULL;

	//统计用户
	for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
	{
		//获取对象
		pBattleUserItem = &m_BattleUserItemArray[i];

		//设置用户
		wBallotUserCount++;
	}

	//同意人数
	wNeedAgreeCount = wBallotUserCount;
	if (m_pIEventBattleItem != NULL)
	{
		wNeedAgreeCount = m_pIEventBattleItem->GetDismissAgreeCount();
		if (wNeedAgreeCount > wBallotUserCount) wNeedAgreeCount = wBallotUserCount;
	}

	//游戏用户
	for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
	{
		//获取对象
		pBattleUserItem = &m_BattleUserItemArray[i];

		//获取椅子
		WORD wChairID = pBattleUserItem->wChairID;

		//统计结果
		if (m_cbBallotResult[wChairID] == 1) wAgreeCount++;
		if (m_cbBallotResult[wChairID] == 2) wDisAgreeCount++;

		//获取用户
		IServerUserItem* pISendUserItem = GetTableUserItem(wChairID);
		if ((pISendUserItem == NULL) || (pISendUserItem->IsClientReady() == false)) continue;

		//发送数据
		m_pIServiceFunction->SendDataToUser(pISendUserItem, MDM_GF_FRAME, SUB_GF_DISMISS_BALLOT, &DismissBallot, sizeof(DismissBallot));
	}

	//解散失败
	if (wDisAgreeCount > (wBallotUserCount - wNeedAgreeCount))
	{
		//删除定时器
		KillGameTimer(IDI_AGREEMENT_DISMISS);

		//继续游戏
		ContinueGame();

		//设置变量
		m_dwLaunchUserID = 0;
		ZeroMemory(m_cbBallotResult, sizeof(m_cbBallotResult));
	}

	//解散成功
	if (wAgreeCount == wNeedAgreeCount)
	{
		//删除定时器
		KillGameTimer(IDI_AGREEMENT_DISMISS);

		//设置变量
		m_dwLaunchUserID = 0;
		ZeroMemory(m_cbBallotResult, sizeof(m_cbBallotResult));

		//游戏用户
		for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
		{
			//获取对象
			pBattleUserItem = &m_BattleUserItemArray[i];

			//获取用户
			IServerUserItem* pISendUserItem = GetTableUserItem(pBattleUserItem->wChairID);
			if ((pISendUserItem == NULL) || (pISendUserItem->IsClientReady() == false)) continue;

			//发送数据
			m_pIServiceFunction->SendDataToUser(pISendUserItem, MDM_GF_FRAME, SUB_GF_DISMISS_SUCCESS, NULL, 0);
		}

		//约战结束
		BattleFinish(BFR_AGREEMENT_DISMISS);
	}

	return true;
}

//扣费结果
bool CTableFrameBattle::DeductCostResult(bool bDeductCostSuccess, DWORD dwPayUserID, DWORD dwFailUserID)
{
	//扣费失败
	if (!bDeductCostSuccess)
	{
		//AA支付
		if (m_BattleConfig.wPayType == PAY_TYPE_USER)
		{
			//构造消息
			TCHAR szOtherMessage[128] = TEXT("");
			TCHAR szAppointMessage[128] = TEXT("");
			WORD wType = SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_ROOM;

			//金币类型
			if (m_BattleConfig.cbCurrencyKind == CURRENCY_KIND_GOLD)
			{
				//构造提示			
				_snwprintf_s(szOtherMessage, CountArray(szOtherMessage), CWHService::FormatMessage(MIM_SERVER_0082, TEXT("本桌有玩家金币不足，游戏无法开始！")));
				_snwprintf_s(szAppointMessage, CountArray(szAppointMessage), CWHService::FormatMessage(MIM_SERVER_0083, TEXT("由于您身上金币不足，无法参与游戏！")));
			}

			//房卡类型
			if (m_BattleConfig.cbCurrencyKind == CURRENCY_KIND_CARD)
			{
				//构造提示			
				_snwprintf_s(szOtherMessage, CountArray(szOtherMessage), CWHService::FormatMessage(MIM_SERVER_0082, TEXT("本桌有玩家房卡不足，游戏无法开始！")));
				_snwprintf_s(szAppointMessage, CountArray(szAppointMessage), CWHService::FormatMessage(MIM_SERVER_0083, TEXT("由于您身上房卡不足，无法参与游戏！")));
			}

			//变量定义
			WORD wEnumIndex = 0;

			//旁观玩家
			do
			{
				//获取用户
				auto pIServerUserItem = EnumLookonUserItem(wEnumIndex++);
				if (pIServerUserItem == NULL) break;

				//效验状态
				if (pIServerUserItem->IsClientReady() == false) continue;

				//断开连接
				m_pIServiceFunction->SendGameMessage(pIServerUserItem, CWHService::FormatMessage(MIM_SERVER_0078, TEXT("系统即将维护，约战房间暂时停止服务。请在维护完成后重试！")), SMT_CHAT | SMT_EJECT | SMT_SHOW_MOBILE | SMT_CLOSE_GAME);

			} while (true);

			//在坐玩家
			for (WORD i = 0; i < m_wChairCount; i++)
			{
				//获取用户
				IServerUserItem* pIServerUserItem = GetTableUserItem(i);
				if (pIServerUserItem == NULL) continue;
				if (pIServerUserItem->IsClientReady() == false) continue;

				//更新状态
				pIServerUserItem->SetUserStatus(US_SIT, m_wTableID, i);

				//发送消息
				if (dwFailUserID != pIServerUserItem->GetUserID())
				{
					m_pIServiceFunction->SendGameMessage(pIServerUserItem, szOtherMessage, wType);
				}
				else
				{
					m_pIServiceFunction->SendGameMessage(pIServerUserItem, szAppointMessage, wType);
				}
			}			
		}

		return false;					
	}	

	//扣费标识
	m_bDeductCost = true;

	//付费玩家
	m_BattleConfig.dwPayUserID = dwPayUserID;

	//AA支付
	if (m_BattleConfig.wPayType == PAY_TYPE_USER)
	{
		return __super::TryStartGame();
	}

	return true;
}

//记录标识
SCORE CTableFrameBattle::BuildBattlleRecordID()
{
	//静态变量
	static WORD wBattleCount = 0;
	static DWORD dwLastSecond = (DWORD)time(NULL);

	//变量定义
	DWORD dwSecond = (DWORD)time(NULL);

	//更新变量
	if (dwLastSecond != dwSecond)
	{
		wBattleCount = 0;
		dwLastSecond = dwSecond;
	}

	//格式化标识
	TCHAR szRecordID[19];
	_sntprintf_s(szRecordID, CountArray(szRecordID), TEXT("%05d%10d%03d"), m_pGameServiceOption->wServerID, dwSecond, ++wBattleCount);

	//数据转换
	SCORE lRecordID = _tstoi64(szRecordID);

	return lRecordID;
}

//退还费用
bool CTableFrameBattle::PerformReturnCost(WORD wPayType)
{
	//标识判断
	if (m_bReturnCost == true)
	{
		return false;
	}

	//退费标识
	m_bReturnCost = true;

	//馆主支付
	if (wPayType == PAY_TYPE_OWNER)
	{
		//变量定义
		DBR_GR_ReturnBattleCost ReturnBattleCost;
		ZeroMemory(&ReturnBattleCost, sizeof(ReturnBattleCost));

		//构造结构
		ReturnBattleCost.dwClubID = m_dwClubID;
		ReturnBattleCost.dwUserID = m_BattleConfig.dwPayUserID;		
		ReturnBattleCost.lReturnCount = m_BattleConfig.lConsumeCount;
		ReturnBattleCost.cbCurrencyKind = m_BattleConfig.cbCurrencyKind;

		//投递数据
		m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_RETURN_BATTLE_COST, ReturnBattleCost.dwUserID, 0L, 0L, &ReturnBattleCost, sizeof(ReturnBattleCost));
	}

	//AA支付
	if (wPayType == PAY_TYPE_USER)
	{
		//变量定义
		DBR_GR_ReturnBattleCost ReturnBattleCost;
		ZeroMemory(&ReturnBattleCost, sizeof(ReturnBattleCost));

		for (WORD i = 0; i < m_wChairCount; i++)
		{
			//获取用户
			IServerUserItem* pIServerUserItem = GetTableUserItem(i);

			//状态判断
			if (pIServerUserItem == NULL) continue;			

			//构造结构
			ReturnBattleCost.dwClubID = m_dwClubID;			
			ReturnBattleCost.dwUserID = pIServerUserItem->GetUserID();
			ReturnBattleCost.lReturnCount = m_BattleConfig.lConsumeCount;			
			ReturnBattleCost.cbCurrencyKind = m_BattleConfig.cbCurrencyKind;

			//投递数据
			m_pIDataBaseEngine->PostDataBaseRequest(DBR_GR_RETURN_BATTLE_COST, ReturnBattleCost.dwUserID, 0L, 0L, &ReturnBattleCost, sizeof(ReturnBattleCost));
		}
	}

	return true;
}

//计算税收
SCORE CTableFrameBattle::CalcCustomRevenue(SCORE lGameScore)
{
	//CString s;
	//s.Format(_T("bbbbb  lGameScore:%I64d, m_BattleConfig.cbScoreKind:%d , SCORE_KIND_BATTLE:%d,"), lGameScore,  m_BattleConfig.cbScoreKind, SCORE_KIND_BATTLE);
    //CTraceService::TraceString(s, TraceLevel_Exception); 
	//约战积分
	if (m_BattleConfig.cbScoreKind == SCORE_KIND_BATTLE) return 0L;

	//高级配置
	//auto pAdvancedOption = (tagAdvancedOption*)m_BattleConfig.cbAdvancedOption;// kk guan

	//if (lGameScore <= pAdvancedOption->lMinWinScore) return 0L;//kk guan

	//return pAdvancedOption->lRevenueRate * lGameScore / 1000L;//kk guan

	//kk jia start
	if (lGameScore < 0)
	{
		return  0;
	}

	if (lGameScore < m_BattleConfig.lMinWin)
	{
		return  0;
	}
	if ( lGameScore >= m_BattleConfig.lMinWin && lGameScore < m_BattleConfig.lWin2)
	{
		return m_BattleConfig.lRevenueRate1;
	}
	if (lGameScore >= m_BattleConfig.lWin2 && lGameScore < m_BattleConfig.lWin3)
	{
		return m_BattleConfig.lRevenueRate2;
	}
	
	return m_BattleConfig.lRevenueRate3;
	
	//kk jia end	
}

//获取用户
IServerUserItem* CTableFrameBattle::GetFirstSitUserItem()
{
	//变量定义
	DWORD dwSitdownTime = (DWORD)-1;
	IServerUserItem* pIServerUserItem = NULL;

	for (int i = 0; i < m_wChairCount; i++)
	{
		auto pITableUserItem = GetTableUserItem(i);
		if (pITableUserItem == NULL) continue;
		if (pITableUserItem->GetLogonTime() < dwSitdownTime)
		{
			pIServerUserItem = pITableUserItem;
			dwSitdownTime = pITableUserItem->GetLogonTime();
		}
	}

	return pIServerUserItem;
}

//解散提醒
bool CTableFrameBattle::SendDimissNotify(IServerUserItem* pIServerUserItem)
{
	//获取用户
	tagBattleUserItem* pBattleUserItem = GetBattleUserItem(pIServerUserItem->GetUserID());
	if (pBattleUserItem == NULL)
	{
		return true;
	}

	//获取时间
	DWORD dwCurrentTime = (DWORD)time(NULL);
	DWORD dwDismissTime = TIME_BATTLE_AGREEMENT_DISMISS;

	//获取配置
	if (m_pIEventCustomFrameTime != NULL)
	{
		dwDismissTime = m_pIEventCustomFrameTime->GetAgreementDismissTime();
	}

	//构造结构
	CMD_GF_DismissNotify DismissNotify;
	ZeroMemory(&DismissNotify, sizeof(DismissNotify));

	//设置变量
	DismissNotify.dwUserID = m_dwLaunchUserID;
	DismissNotify.wResidueTime = dwDismissTime - __min(dwCurrentTime - m_dwLaunchTime, dwDismissTime);

	//统计用户
	for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
	{
		//获取对象
		pBattleUserItem = &m_BattleUserItemArray[i];

		//设置用户
		DismissNotify.wNeedAgreeCount++;
		DismissNotify.dwUserIDList[DismissNotify.wUserCount++] = pBattleUserItem->dwUserID;
	}

	//调整同意人数
	if (m_pIEventBattleItem != NULL)
	{
		DismissNotify.wNeedAgreeCount = m_pIEventBattleItem->GetDismissAgreeCount();
	}

	//调整人数
	if (DismissNotify.wNeedAgreeCount > DismissNotify.wUserCount)
	{
		DismissNotify.wNeedAgreeCount = DismissNotify.wUserCount;
	}

	//判断时间
	if (DismissNotify.wResidueTime > 0)
	{
		//发送数据
		m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_FRAME, SUB_GF_DISMISS_NOTIFY, &DismissNotify, sizeof(DismissNotify));

		//构造结构
		CMD_GF_DismissBallot DismissBallot;
		ZeroMemory(&DismissBallot, sizeof(DismissBallot));

		//约战用户
		for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
		{
			//获取对象
			pBattleUserItem = &m_BattleUserItemArray[i];

			//获取椅子
			WORD wChairID = pBattleUserItem->wChairID;

			//获取用户
			IServerUserItem* pITableUserItem = GetTableUserItem(wChairID);
			if (pITableUserItem == NULL || m_cbBallotResult[wChairID] == 0) continue;

			//设置变量
			DismissBallot.dwUserID = pITableUserItem->GetUserID();
			DismissBallot.bAgreeDismiss = (m_cbBallotResult[wChairID] == 1);

			//发送数据
			m_pIServiceFunction->SendDataToUser(pIServerUserItem, MDM_GF_FRAME, SUB_GF_DISMISS_BALLOT, &DismissBallot, sizeof(DismissBallot));
		}
	}

	return true;
}

//获取用户
tagBattleUserItem* CTableFrameBattle::GetBattleUserItem(DWORD dwUserID)
{
	//校验用户
	for (INT_PTR i = 0; i < m_BattleUserItemArray.GetCount(); i++)
	{
		//获取对象
		if (m_BattleUserItemArray[i].dwUserID == dwUserID)
		{
			return &m_BattleUserItemArray[i];
		}
	}

	return NULL;
}

//校验配置
bool CTableFrameBattle::VerifyBattleConfig(tagBattleConfig* pBattleConfig, bool bOnlyVerify)
{
	//游戏选项
	if (m_pIEventGameOption != NULL && pBattleConfig->cbOptionSize > 0)
	{
		if (m_pIEventGameOption->SetNormalOption(bOnlyVerify, pBattleConfig->cbGameOption, pBattleConfig->cbOptionSize) == false)
		{
			return false;
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
