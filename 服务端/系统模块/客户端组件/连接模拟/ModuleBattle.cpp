#include "Stdafx.h"
#include "ModuleBattle.h"

//////////////////////////////////////////////////////////////////////////////////////////////
//时钟定义

#define	IDI_TIME_PULSE				1								//时间脉冲

//////////////////////////////////////////////////////////////////////////////////////////////

//构造函数
CModuleBattle::CModuleBattle(IModuleItemSink* pIModuleItemSink):
	m_pIModuleItemSink(pIModuleItemSink)
{
	//设置模块
	SetServiceModule(SERVICE_MODULE_BATTLE);
}

//析构函数
CModuleBattle::~CModuleBattle()
{
}

//关闭事件
bool CModuleBattle::OnEventVirtualShut()
{
	//设置变量
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));

	//投递消息	
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->PostWindowMessage(WM_BATTLE_LINK_SHUT, 0, 0);

	//构造消息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 与约战服务器的虚拟连接断开了"), m_pIModuleItemSink->GetGlobalUserData()->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(255, 0, 0));

	return true;
}

//连接事件
bool CModuleBattle::OnEventVirtualLink(LONG lErrorCode)
{
	//连接失败
	if (lErrorCode != 0)
	{
		//投递消息	
		ASSERT(m_pIModuleItemSink != NULL);
		m_pIModuleItemSink->PostWindowMessage(WM_BATTLE_LINK_FAILURE, 0, 0);

		return true;
	}

	//发送数据
	if (m_BufferPacket.wMainCmdID != 0 && m_BufferPacket.wSubCmdID != 0)
	{
		SendVirtualData(m_BufferPacket.wMainCmdID, m_BufferPacket.wSubCmdID, m_BufferPacket.cbDataBuffer, m_BufferPacket.wDataSize);
	}

	return true;
}

//删除时间
bool CModuleBattle::KillModuleTimer(UINT nTimerID)
{
	//投递消息
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->KillGameTimer(GetTimerStart() + nTimerID);

	return true;
}

//设置时间
bool CModuleBattle::SetModuleTimer(UINT nTimerID, UINT nElapse)
{
	//投递消息
	ASSERT(m_pIModuleItemSink != NULL);
	m_pIModuleItemSink->SetGameTimer(GetTimerStart() + nTimerID, nElapse);

	return true;
}

//时钟事件
bool CModuleBattle::OnEventTimer(UINT uTimerID, WPARAM wBindParam)
{
	//时钟脉冲
	if (uTimerID == IDI_TIME_PULSE)
	{
		//变量定义
		DWORD dwTimeStamp = (DWORD)time(NULL);
		
		if (dwTimeStamp % 5 == 0)
		{
		
		}
	}

	return true;
}

//读取事件
bool CModuleBattle::OnEventVirtualRead(CMD_Command Command, VOID* pData, WORD wDataSize)
{
	switch (Command.wMainCmdID)
	{
	case MDM_GB_BATTLE_SERVICE:			//茶社服务
	{
		return OnSocketMainBattleService(Command.wSubCmdID, pData, wDataSize);
	}		
	}

	return true;
}

//登录处理
bool CModuleBattle::OnSocketMainBattleService(WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_GB_S_TABLE_ITEM:		//茶社成员
	{
		return OnSocketSubTableItem(pData, wDataSize);
	}
	case SUB_GB_S_TABLE_PARAM:		//茶社消息
	{
		return OnSocketSubTableParam(pData, wDataSize);
	}
	case SUB_GB_S_BATTLE_BASE_OPTION:	//进入成功
	{
		return OnSocketSubBattleBaseOption(pData, wDataSize);
	}
	case SUB_GB_S_BATTLE_GAME_OPTION:	//进入失败
	{
		return OnSocketSubBattleGameOption(pData, wDataSize);
	}
	case SUB_GB_S_BATTLE_OPTION_FINISH:		//茶社子项
	{
		return OnSocketSubBattleOptionFinish(pData, wDataSize);
	}
	//case SUB_GB_S_OPERATE_SUCCESS:		//操作成功
	//{
	//	return OnSocketSubOperateSuccess(pData, wDataSize);
	//}
	//case SUB_GB_S_OPERATE_FAILURE:		//操作失败
	//{
	//	return OnSocketSubOperateFailure(pData, wDataSize);
	//}
	}

	return true;
}
//桌子子项
bool CModuleBattle::OnSocketSubTableItem(VOID* pData, WORD wDataSize)
{
	return true;
}

//桌子列表
bool CModuleBattle::OnSocketSubTableList(VOID* pData, WORD wDataSize)
{
	return true;
}

//桌子参数
bool CModuleBattle::OnSocketSubTableParam(VOID* pData, WORD wDataSize)
{
	return true;
}

//桌子解散
bool CModuleBattle::OnSocketSubTableDismiss(VOID* pData, WORD wDataSize)
{
	/*auto pOperateSuccess = (CMD_GB_S_OperateSuccess*)pData;

	CString strMessage;
	strMessage.Format(TEXT("操作成功：%s"), pOperateSuccess->szDescribeString);
	m_pIModuleItemSink->InsertString(strMessage, RGB(128, 128, 128));*/

	return true;
}

//操作成功
bool CModuleBattle::OnSocketSubOperateSuccess(VOID* pData, WORD wDataSize)
{
	/*auto pOperateSuccess = (CMD_GB_S_OperateSuccess*)pData;

	CString strMessage;
	strMessage.Format(TEXT("操作成功：%s"), pOperateSuccess->szDescribeString);
	m_pIModuleItemSink->InsertString(strMessage,RGB(128, 128, 128));*/

	return true;
}

//操作失败
bool CModuleBattle::OnSocketSubOperateFailure(VOID* pData, WORD wDataSize)
{
	/*auto pOperateFailure = (CMD_GB_S_OperateFailure*)pData;

	CString strMessage;
	strMessage.Format(TEXT("操作失败：[%d]%s"), pOperateFailure->lErrorCode,pOperateFailure->szErrorMessage);
	m_pIModuleItemSink->InsertString(strMessage, RGB(128, 128, 128));*/

	return true;

}

//基础配置
bool CModuleBattle::OnSocketSubBattleBaseOption(VOID* pData, WORD wDataSize)
{
	auto pBaseOption = (tagBattleBaseOption*)pData;
	auto nOptionCount = wDataSize / sizeof(tagBattleBaseOption);

	for (int i = 0; i < nOptionCount; i++)
	{
		m_ArrayBaseOption.Add(*pBaseOption++);
	}

	m_pIModuleItemSink->InsertString(TEXT("收到约战基础配置"), RGB(128, 128, 128));

	return true;
}

//游戏配置
bool CModuleBattle::OnSocketSubBattleGameOption(VOID* pData, WORD wDataSize)
{
	m_pIModuleItemSink->InsertString(TEXT("收到约战游戏配置"), RGB(128, 128, 128));

	return true;
}

//配置完成
bool CModuleBattle::OnSocketSubBattleOptionFinish(VOID* pData, WORD wDataSize)
{
	m_pIModuleItemSink->InsertString(TEXT("约战配置接受完成"), RGB(128, 128, 128));

	//创建房间
	if (m_ArrayBaseOption.GetCount() > 0)
	{
		CreateTable(&m_ArrayBaseOption[0]);
	}

	return true;
}

//进入服务
bool CModuleBattle::EnterService()
{
	//用户接口
	ASSERT(m_pIModuleItemSink != NULL);
	auto pGlobalUserData = m_pIModuleItemSink->GetGlobalUserData();

	//变量定义
	CMD_GB_C_QueryOption QueryOption = {};

	//设置变量
	QueryOption.wKindID = INVALID_WORD;
	//QueryOption.cbEmployScene = 0;

	//校验状态
	if (GetLinkStatus() == true)
	{
		SendVirtualData(MDM_GB_BATTLE_SERVICE, SUB_GB_C_QUERY_OPTION, &QueryOption, sizeof(QueryOption));
	}
	else
	{
		//缓冲数据
		m_BufferPacket.wMainCmdID = MDM_GB_BATTLE_SERVICE;
		m_BufferPacket.wSubCmdID = SUB_GB_C_QUERY_OPTION;
		m_BufferPacket.wDataSize = sizeof(QueryOption);
		CopyMemory(m_BufferPacket.cbDataBuffer,&QueryOption,sizeof(QueryOption));

		//创建连接
		CreateVirtualLink(INVALID_WORD, pGlobalUserData->dwUserID, pGlobalUserData->szLogonToken, NULL, 0);
	}

	//打印信息
	CString strMessage;
	strMessage.Format(TEXT("Accounts=%s: 正在查询约战配置..."), pGlobalUserData->szAccounts);
	m_pIModuleItemSink->InsertString(strMessage.GetString(), RGB(128, 128, 128));

	return true;
}

//创建桌子
VOID CModuleBattle::CreateTable(tagBattleBaseOption* pBattleBaseOption)
{
	//创建房间
	CMD_GB_C_CreateTable  CreateTable = {};

	//设置变量
	StringCchCopy(CreateTable.szMachineID, sizeof(CreateTable.szMachineID), TEXT("jfkjslKKLFJLKfjiwejfLKJ1246546"));

	//约战配置-AA
	CreateTable.BattleConfig.wKindID = pBattleBaseOption->wKindID;
	CreateTable.BattleConfig.cbScoreKind = pBattleBaseOption->cbScoreKind;
	CreateTable.BattleConfig.cbCurrencyKind = CURRENCY_KIND_GOLD;
	CreateTable.BattleConfig.cbSettleKind = SETTLE_KIND_COUNT;
	CreateTable.BattleConfig.wPlayerCount = pBattleBaseOption->wPlayerCount;
	CreateTable.BattleConfig.wPayType = PAY_TYPE_USER;	
	CreateTable.BattleConfig.lBaseScore = pBattleBaseOption->lMaxBaseScore;
	CreateTable.BattleConfig.lEntryCost = pBattleBaseOption->lMinEntryCost;
	//CreateTable.BattleConfig.lRevenueRate = 0;
	//CreateTable.BattleConfig.cbAllowLookon = TRUE;
	CreateTable.BattleConfig.cbAllowPublic = TRUE;
	CreateTable.BattleConfig.cbAllowSameIP = TRUE;
	CreateTable.BattleConfig.wPlayCount = pBattleBaseOption->OptionItem[0].wPlayCount;

	//发送消息
	SendVirtualData(MDM_GB_BATTLE_SERVICE, SUB_GB_C_CREATE_TABLE, &CreateTable, sizeof(CreateTable));
}

//重置模块
VOID CModuleBattle::ResetModule()
{
	//设置变量
	ZeroMemory(&m_BufferPacket, sizeof(m_BufferPacket));	
}
//////////////////////////////////////////////////////////////////////////////////////////////
