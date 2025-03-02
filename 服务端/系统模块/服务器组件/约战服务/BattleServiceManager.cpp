#include "StdAfx.h"
#include "AfxTempl.h"
#include "BattleTableFrame.h"
#include "BattleServiceManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//静态成员
CBattleServiceManager * CBattleServiceManager::m_pBattleServiceManager=NULL;

////////////////////////////////////////////////////////////////////////

//构造函数
CBattleServiceManager::CBattleServiceManager(void)
{
	//状态变量
	m_bIsService=false;
	ZeroMemory(&m_ExtendServiceParameter, sizeof(m_ExtendServiceParameter));

	//设置变量
	if (m_pBattleServiceManager==NULL) m_pBattleServiceManager=this;
}

//析构函数
CBattleServiceManager::~CBattleServiceManager(void)
{	
	//设置变量
	if (m_pBattleServiceManager==this) m_pBattleServiceManager=NULL;
}

//停止服务
bool CBattleServiceManager::ConcludeService()
{
	//状态判断
	ASSERT(m_bIsService==true);

	//设置状态
	m_bIsService=false;
	ZeroMemory(&m_ExtendServiceParameter, sizeof(m_ExtendServiceParameter));

	return true;
}

//启动服务
bool CBattleServiceManager::StartService()
{
	//状态判断
	ASSERT(m_bIsService==false);
	if(m_bIsService==true) return false;

	//设置状态
	m_bIsService=true;	

	return true;
}
//接口查询
void *  CBattleServiceManager::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(IExtendServiceManager,Guid,dwQueryVer);	
	QUERYINTERFACE_IUNKNOWNEX(IExtendServiceManager,Guid,dwQueryVer);	
	return NULL;
}

//初始化桌子框架
VOID* CBattleServiceManager::CreateExtendTableFrame()
{
	//变量定义
	CTableFrameBattle * pTableFrameItem = NULL;

	try
	{
		//建立对象
		pTableFrameItem = new CTableFrameBattle();
		if (pTableFrameItem == NULL) throw TEXT("创建失败");

		//添加桌子
		m_TableFrameArray.Add(pTableFrameItem);

		return pTableFrameItem;
	}
	catch (...) {}

	//删除对象
	SafeDelete(pTableFrameItem);

	return NULL;	
}

//创建子项
VOID* CBattleServiceManager::CreateExtendDataBaseSink(REFGUID Guid, DWORD dwQueryVer)
{
	//变量定义
	CExtendDataBaseSink* pExtendDataBaseSink = NULL;

	try
	{
		//建立对象
		pExtendDataBaseSink = new CExtendDataBaseSink();
		if (pExtendDataBaseSink == NULL) throw TEXT("创建失败");

		//查询接口
		VOID* pObject = pExtendDataBaseSink->QueryInterface(Guid, dwQueryVer);
		if (pObject == NULL) throw TEXT("接口查询失败");

		return pObject;
	}
	catch (...) {}

	//删除对象
	SafeDelete(pExtendDataBaseSink);

	return NULL;
}

//初始化接口
bool CBattleServiceManager::InitServiceManager(tagExtendServiceParameter& ExtendServiceParameter)
{
	//拷贝参数
	m_ExtendServiceParameter=ExtendServiceParameter;

	return true;
}

//时间事件
bool CBattleServiceManager::OnEventTimer(DWORD dwTimerID, WPARAM dwBindParameter)
{
	//计算索引
	BYTE cbTimerIndex=dwTimerID-IDI_EXTEND_MODULE_START;

	return false;
}

//数据库事件
bool CBattleServiceManager::OnEventDataBase(WORD wRequestID, VOID * pData, WORD wDataSize,DWORD dwContextID, DWORD dwTokenID)
{
	switch (wRequestID)
	{
	case DBO_GR_DEDUCT_BATTLE_COST: //扣费结果
	{
		//效验参数
		ASSERT (wDataSize <= sizeof(DBO_GR_DeductBattleCost));
		if (wDataSize > sizeof(DBO_GR_DeductBattleCost)) return false;

		//提取数据
		auto pDeductBattleCost = (DBO_GR_DeductBattleCost*)pData;

		//判断桌号
		ASSERT (pDeductBattleCost->wTableID < m_TableFrameArray.GetCount());
		if (pDeductBattleCost->wTableID >= m_TableFrameArray.GetCount()) return true;

		//查找桌子
		auto pTableFrame = (CTableFrameBattle*)m_TableFrameArray[pDeductBattleCost->wTableID];
		if (pTableFrame == NULL) return false;

		//变量定义
		tagBattleTableItemEx TableItem;
		pTableFrame->GetBattleTableItem(TableItem);

		//房主支付
		if (TableItem.BattleConfig.wPayType == PAY_TYPE_OWNER)
		{
			//变量定义
			CMD_BS_S_CreateTable CreateTable;
			ZeroMemory(&CreateTable, sizeof(CreateTable));

			//桌子信息	
			CreateTable.wTableID = TableItem.wTableID;
			CreateTable.dwBattleNum = TableItem.dwBattleNum;
			CreateTable.dwCreateTime = TableItem.dwCreateTime;

			//财富信息
			CreateTable.dwPayUserID = pDeductBattleCost->dwPayUserID;
			CreateTable.lUserGold = pDeductBattleCost->lUserGold;
			CreateTable.lUserCard = pDeductBattleCost->lUserCard;

			//结果信息
			CreateTable.lResultCode = pDeductBattleCost->lResultCode;
			lstrcpynW(CreateTable.szErrorMessage, pDeductBattleCost->szErrorMessage, CountArray(CreateTable.szErrorMessage));

			//发送信息
			WORD wStringSize = CountStringBufferW(CreateTable.szErrorMessage);
			WORD wSendSize = sizeof(CreateTable) - sizeof(CreateTable.szErrorMessage) + wStringSize;
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_CREATE_TABLE, &CreateTable, wSendSize, 0);
		}

		//非大赢家支付
		if (TableItem.BattleConfig.wPayType != PAY_TYPE_WINNER)
		{
			//扣费结果
			pTableFrame->DeductCostResult(pDeductBattleCost->lResultCode == 0, pDeductBattleCost->dwPayUserID, pDeductBattleCost->dwFailUserID);

			//解散桌子
			if (pDeductBattleCost->lResultCode != 0) pTableFrame->DismissBattleTable(false);
		}
	}
	case DBO_GR_RETURN_BATTLE_COST: //退还结果
	{
		//效验参数
		ASSERT(wDataSize <= sizeof(DBO_GR_ReturnBattleCost));
		if (wDataSize > sizeof(DBO_GR_ReturnBattleCost)) return false;

		//提取数据
		auto pReturnBattleCost = (DBO_GR_ReturnBattleCost*)pData;	

		return true;
	}	
	}

	return false;
}

//网络事件
bool CBattleServiceManager::OnEventSocketRead(WORD wMainCmdID, WORD wSubCmdID, VOID* pData, WORD wDataSize)
{
	switch (wSubCmdID)
	{
	case SUB_BS_C_CREATE_TABLE:		 //创建桌子
	{
		//参数校验
		ASSERT(sizeof(CMD_BS_C_CreateTable) == wDataSize);
		if (sizeof(CMD_BS_C_CreateTable) != wDataSize) return false;

		//提取数据
		CMD_BS_C_CreateTable* pCreateTable = (CMD_BS_C_CreateTable*)pData;

		//变量定义			
		tagBattleTableItemEx TableItem;
		CTableFrameBattle* pFindTableFrame = NULL;

		//查找空桌	
		for (WORD i = 0; i < m_ExtendServiceParameter.pGameServiceOption->wTableCount; i++)
		{
			//获取桌子
			auto pTableFrame = (CTableFrameBattle*)m_TableFrameArray[i];

			//判断状态
			if (pTableFrame->IsGameStarted()) continue;
			if (pTableFrame->GetSitUserCount() > 0) continue;

			//获取信息				
			pTableFrame->GetBattleTableItem(TableItem);
			if (TableItem.dwBattleNum != 0 || TableItem.dwOwnerID != 0) continue;

			//设置变量
			pFindTableFrame = pTableFrame;
			break;
		}

		//未找到桌子
		if (pFindTableFrame == NULL)
		{
			//变量定义
			CMD_BS_S_CreateTable CreateBattleTable;
			ZeroMemory(&CreateBattleTable, sizeof(CreateBattleTable));

			//构造结构				
			CreateBattleTable.wTableID = INVALID_TABLE;
			CreateBattleTable.lResultCode = BATTLE_CODE_TABLE_UNFOUND;
			CreateBattleTable.dwBattleNum = pCreateTable->dwBattleNum;

			//错误信息
			StringCchCopy(CreateBattleTable.szErrorMessage, CountArray(CreateBattleTable.szErrorMessage), CWHService::FormatMessage(MIM_SERVER_0043, TEXT("抱歉，没找到可用的房间, 请稍后再试！")));

			//发送信息
			WORD wStringSize = CountStringBuffer(CreateBattleTable.szErrorMessage);
			WORD wPacketSize = sizeof(CreateBattleTable) - sizeof(CreateBattleTable.szErrorMessage) + wStringSize;
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_CREATE_TABLE, &CreateBattleTable, wPacketSize, 0);

			return true;
		}

		//变量定义
		tagBattleCreateParam CreateParam;
		ZeroMemory(&CreateParam, sizeof(CreateParam));

		//桌子信息			
		CreateParam.dwOwnerID = pCreateTable->dwUserID;
		CreateParam.dwBattleNum = pCreateTable->dwBattleNum;
		StringCchCopy(CreateParam.szPassword, CountArray(CreateParam.szPassword), pCreateTable->szPassword);

		//群组信息
		CreateParam.dwClubID = pCreateTable->dwClubID;
		CreateParam.wSwitchOption = pCreateTable->wSwitchOption;
		CreateParam.wClubServiceID = pCreateTable->wClubServiceID;
		CreateParam.dwClubCreaterID = pCreateTable->dwClubCreaterID;		

		//创建信息
		CreateParam.dwCreateTime = (DWORD)time(NULL);
		CreateParam.dwConfigID = pCreateTable->dwConfigID;
		CopyMemory(&CreateParam.BattleConfig, &pCreateTable->BattleConfig, sizeof(CreateParam.BattleConfig));

		//创建桌子
		if (pFindTableFrame->CreateBattleTable(CreateParam) == false)
		{
			//变量定义
			CMD_BS_S_CreateTable CreateBattleTable;
			ZeroMemory(&CreateBattleTable, sizeof(CreateBattleTable));

			//构造结构				
			CreateBattleTable.wTableID = INVALID_TABLE;
			CreateBattleTable.lResultCode = BATTLE_CODE_CREATE_FAILED;
			CreateBattleTable.dwBattleNum = pCreateTable->dwBattleNum;

			//错误信息
			StringCchCopy(CreateBattleTable.szErrorMessage, CountArray(CreateBattleTable.szErrorMessage), CWHService::FormatMessage(MIM_SERVER_0044, TEXT("抱歉，房间创建失败, 请稍后再试！")));

			//发送信息
			WORD wStringSize = CountStringBuffer(CreateBattleTable.szErrorMessage);
			WORD wPacketSize = sizeof(CreateBattleTable) - sizeof(CreateBattleTable.szErrorMessage) + wStringSize;
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_CREATE_TABLE, &CreateBattleTable, wPacketSize, 0);

			return true;
		}

		//房主支付
		if (pCreateTable->BattleConfig.wPayType == PAY_TYPE_OWNER)
		{
			//构造结构
			DBR_GR_DeductBattleCost DeductBattleCost;
			ZeroMemory(&DeductBattleCost, sizeof(DeductBattleCost));

			//用户信息	
			DeductBattleCost.dwUserID = pCreateTable->dwUserID;
			DeductBattleCost.dwClubID = pCreateTable->dwClubID;
			DeductBattleCost.wTableID = pFindTableFrame->GetTableID();
			DeductBattleCost.wPayType = pCreateTable->BattleConfig.wPayType;
			DeductBattleCost.lConsumeCount = pCreateTable->BattleConfig.lConsumeCount;
			DeductBattleCost.cbCurrencyKind = pCreateTable->BattleConfig.cbCurrencyKind;
			
			//投递数据
			m_ExtendServiceParameter.pIDataBaseEngine->PostDataBaseRequest(DBR_GR_DEDUCT_BATTLE_COST, DeductBattleCost.dwUserID, 0L, 0L, &DeductBattleCost, sizeof(DeductBattleCost));
		}
		else
		{
			//获取桌子信息			
			pFindTableFrame->GetBattleTableItem(TableItem);

			//变量定义
			CMD_BS_S_CreateTable CreateBattleTable;
			ZeroMemory(&CreateBattleTable, sizeof(CreateBattleTable));

			//桌子信息	
			CreateBattleTable.wTableID = TableItem.wTableID;
			CreateBattleTable.dwBattleNum = TableItem.dwBattleNum;
			CreateBattleTable.dwCreateTime = TableItem.dwCreateTime;

			//发送信息
			WORD wStringSize = CountStringBuffer(CreateBattleTable.szErrorMessage);
			WORD wPacketSize = sizeof(CreateBattleTable) - sizeof(CreateBattleTable.szErrorMessage) + wStringSize;
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_CREATE_TABLE, &CreateBattleTable, wPacketSize, 0);
		}

		return true;
	}
	case SUB_BS_C_MODIFY_TABLE:		 //修改桌子
	{
		//参数校验
		ASSERT(sizeof(CMD_BS_C_ModifyTable) == wDataSize);
		if (sizeof(CMD_BS_C_ModifyTable) != wDataSize) return false;

		//提取数据
		CMD_BS_C_ModifyTable* pModifyTable = (CMD_BS_C_ModifyTable*)pData;

		//校验桌号
		if (pModifyTable->wTableID >= m_TableFrameArray.GetCount())
		{
			//变量定义
			CMD_BS_S_ModifyTable ModifyBattleTable;
			ZeroMemory(&ModifyBattleTable, sizeof(ModifyBattleTable));

			//错误信息
			ModifyBattleTable.cbResultCode = BATTLE_CODE_MODIFY_FAILED;
			CopyMemory(&ModifyBattleTable.SocketInfo, &pModifyTable->SocketInfo, sizeof(ModifyBattleTable.SocketInfo));
			StringCchCopy(ModifyBattleTable.szErrorMessage, CountArray(ModifyBattleTable.szErrorMessage), CWHService::FormatMessage(MIM_SERVER_0045, L"抱歉，系统未查询到房间信息,配置更新失败！"));

			//发送信息
			WORD wStringSize = CountStringBufferW(ModifyBattleTable.szErrorMessage);
			WORD wPacketSize = sizeof(ModifyBattleTable) - sizeof(ModifyBattleTable.szErrorMessage) + wStringSize;
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_MODIFY_TABLE, &ModifyBattleTable, wPacketSize, 0);
		}

		//获取桌子
		auto pTableFrame = (CTableFrameBattle*)m_TableFrameArray[pModifyTable->wTableID];

		//变量定义
		tagBattleModifyParam ModifyParam;
		ZeroMemory(&ModifyParam, sizeof(ModifyParam));

		//桌子信息
		ModifyParam.dwUserID = pModifyTable->dwUserID;
		ModifyParam.dwBattleNum = pModifyTable->dwBattleNum;

		//拷贝配置
		CopyMemory(&ModifyParam.BattleConfig, &pModifyTable->BattleConfig, sizeof(ModifyParam.BattleConfig));

		//修改桌子
		if (pTableFrame->ModifyBattleTable(ModifyParam) == false)
		{
			//变量定义
			CMD_BS_S_ModifyTable ModifyBattleTable;
			ZeroMemory(&ModifyBattleTable, sizeof(ModifyBattleTable));

			//网络信息
			CopyMemory(&ModifyBattleTable.SocketInfo, &pModifyTable->SocketInfo, sizeof(ModifyBattleTable.SocketInfo));

			//错误信息
			ModifyBattleTable.cbResultCode = BATTLE_CODE_MODIFY_FAILED;
			_snwprintf_s(ModifyBattleTable.szErrorMessage, CountArray(ModifyBattleTable.szErrorMessage), CWHService::FormatMessage(MIM_SERVER_0046, L"抱歉，配置更新失败！"));

			//发送信息
			WORD wStringSize = CountStringBufferW(ModifyBattleTable.szErrorMessage);
			WORD wPacketSize = sizeof(ModifyBattleTable) - sizeof(ModifyBattleTable.szErrorMessage) + wStringSize;
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_MODIFY_TABLE, &ModifyBattleTable, wPacketSize, 0);

			return true;
		}

		//桌子信息		
		tagBattleTableItemEx TableItem;
		pTableFrame->GetBattleTableItem(TableItem);

		//变量定义
		CMD_BS_S_ModifyTable ModifyBattleTable;
		ZeroMemory(&ModifyBattleTable, sizeof(ModifyBattleTable));

		//桌子信息
		ModifyBattleTable.dwUserID = pModifyTable->dwUserID;
		ModifyBattleTable.dwBattleNum = pModifyTable->dwBattleNum;
		CopyMemory(&ModifyBattleTable.BattleConfig, &TableItem.BattleConfig, sizeof(ModifyBattleTable.BattleConfig));

		//网络信息
		CopyMemory(&ModifyBattleTable.SocketInfo, &pModifyTable->SocketInfo, sizeof(ModifyBattleTable.SocketInfo));

		//发送消息
		WORD wPacketSize = sizeof(ModifyBattleTable) - sizeof(ModifyBattleTable.szErrorMessage);
		m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_MODIFY_TABLE, &ModifyBattleTable, wPacketSize, 0);

		return true;
	}
	case SUB_BS_C_BATTLE_OPTION:	 //约战选项
	{
		ASSERT(wDataSize == sizeof(CMD_BS_C_BattleOption));
		if (wDataSize != sizeof(CMD_BS_C_BattleOption)) return false;

		//提取数据
		CMD_BS_C_BattleOption* pBattleOption = (CMD_BS_C_BattleOption*)pData;

		//查找桌子	
		for (WORD i = 0; i < m_ExtendServiceParameter.pGameServiceOption->wTableCount; i++)
		{
			//获取桌子
			auto pTableFrame = (CTableFrameBattle*)m_TableFrameArray[i];
			pTableFrame->SetBattleService(pBattleOption->bBattleService);
		}

		return true;
	}
	case SUB_BS_C_SYNCHRO_TABLE:	 //同步桌子
	{
		//参数校验
		ASSERT(wDataSize % sizeof(tagBattleTableItemEx) == 0);
		if (wDataSize % sizeof(tagBattleTableItemEx) != 0) return false;

		//变量定义
		WORD wTableCount = wDataSize / sizeof(tagBattleTableItemEx);

		//变量定义
		tagBattleTableItemEx TableItem;
		CTableFrameBattle* pTableFrame = NULL;

		//发送桌子
		if (wTableCount == 0)
		{
			//变量定义
			WORD wPacketSize = 0;
			BYTE cbBuffer[SOCKET_BUFFER];

			//查找桌子	
			for (WORD i = 0; i < m_ExtendServiceParameter.pGameServiceOption->wTableCount; i++)
			{
				//获取桌子
				pTableFrame = (CTableFrameBattle*)m_TableFrameArray[i];
				if (pTableFrame->GetRelationParameter()->dwRelationNum == 0) continue;

				//发送判断
				if (wPacketSize + sizeof(tagBattleTableItemEx) > sizeof(cbBuffer))
				{
					//发送数据
					m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_TABLE_SYNCHRO, cbBuffer, wPacketSize, 0);
					wPacketSize = 0;
				}

				//桌子信息
				pTableFrame->GetBattleTableItem(TableItem);

				//提取数据
				tagBattleTableItemEx* pTableItem = (tagBattleTableItemEx*)(cbBuffer + wPacketSize);
				CopyMemory(pTableItem, &TableItem, sizeof(tagBattleTableItemEx));

				//位置偏移
				wPacketSize += sizeof(tagBattleTableItemEx);
			}

			//剩余发送
			if (wPacketSize > 0)
			{
				//发送数据
				m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_TABLE_SYNCHRO, cbBuffer, wPacketSize, 0);
			}

			//查找桌子	
			for (WORD i = 0; i < m_ExtendServiceParameter.pGameServiceOption->wTableCount; i++)
			{
				//获取桌子
				pTableFrame = (CTableFrameBattle*)m_TableFrameArray[i];
				if (pTableFrame->GetRelationParameter()->dwRelationNum == 0) continue;

				//发送信息
				pTableFrame->NotifyTableParam();
				pTableFrame->NotifyTableUserList();
			}

			//同步完成
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_SYNCHRO_FINISH, NULL, 0, 0);
		}
		else
		{
			//变量定义
			tagBattleCreateParam CreateParam;
			tagBattleTableItemEx* pTableItem = (tagBattleTableItemEx*)pData;
			
			//添加桌子
			for (WORD i = 0; i < wTableCount; i++)
			{
				//桌号判断
				ASSERT(pTableItem->wTableID < m_TableFrameArray.GetCount());
				if (pTableItem->wTableID >= m_TableFrameArray.GetCount()) continue;

				//获取桌子
				pTableFrame = (CTableFrameBattle*)m_TableFrameArray[pTableItem->wTableID];

				//构造结构					
				ZeroMemory(&CreateParam, sizeof(CreateParam));

				//桌子信息
				CreateParam.dwOwnerID = pTableItem->dwOwnerID;
				CreateParam.dwBattleNum = pTableItem->dwBattleNum;
				StringCchCopy(CreateParam.szPassword, CountArray(CreateParam.szPassword), pTableItem->szPassword);

				//茶社信息
				CreateParam.dwClubID = pTableItem->dwClubID;
				CreateParam.wSwitchOption = pTableItem->wSwitchOption;
				CreateParam.wClubServiceID = pTableItem->wClubServiceID;
				CreateParam.dwClubCreaterID = pTableItem->dwClubCreaterID;

				//创建信息
				CreateParam.dwConfigID = pTableItem->dwConfigID;
				CreateParam.dwCreateTime = pTableItem->dwCreateTime;				
				CopyMemory(&CreateParam.BattleConfig, &pTableItem->BattleConfig, sizeof(CreateParam.BattleConfig));

				//校验状态
				if (m_ExtendServiceParameter.pIMainServiceFrame->GetServiceStatus() != SERVICE_STATUS_SERVICE)
				{
					//创建桌子
					if (pTableFrame->CreateBattleTable(CreateParam) == false)
					{
						//变量定义
						CMD_BS_S_TableDismiss TableDismiss;
						ZeroMemory(&TableDismiss, sizeof(TableDismiss));

						//桌子信息	
						TableDismiss.wTableID = pTableItem->wTableID;
						TableDismiss.dwBattleNum = pTableItem->dwBattleNum;
						TableDismiss.wServerID = m_ExtendServiceParameter.pGameServiceOption->wServerID;

						//发送数据
						m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_TABLE_DISMISS, &TableDismiss, sizeof(TableDismiss), 0);
					}

					//发送信息
					pTableFrame->NotifyTableParam();
					pTableFrame->NotifyTableUserList();

					CTraceService::TraceString(TEXT("BattleNum=%d,Password=%ls,ClubID=%d,ConfigID=%d"), TraceLevel_Normal, pTableItem->dwBattleNum, pTableItem->szPassword, pTableItem->dwClubID,pTableItem->dwConfigID);
				}
				else
				{
					//桌子信息																		
					pTableFrame->GetBattleTableItem(TableItem);

					//桌子已解散
					if (TableItem.dwBattleNum == 0 || TableItem.dwBattleNum != CreateParam.dwBattleNum)
					{
						//变量定义
						CMD_BS_S_TableDismiss TableDismiss;
						ZeroMemory(&TableDismiss, sizeof(TableDismiss));

						//桌子信息	
						TableDismiss.wTableID = TableItem.wTableID;
						TableDismiss.dwBattleNum = CreateParam.dwBattleNum;
						TableDismiss.wServerID = m_ExtendServiceParameter.pGameServiceOption->wServerID;

						//发送数据
						m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_TABLE_DISMISS, &TableDismiss, sizeof(TableDismiss), 0);
					}
					else
					{
						//约战状态
						if (pTableFrame->GetBattleState() == BATTLE_STATE_PLAY)
						{
							//发送信息
							pTableFrame->NotifyTableParam();
							pTableFrame->NotifyTableUserList();
						}
					}
				}

				//指针递增
				++pTableItem;
			}
		}

		return true;
	}
	case SUB_BS_C_DISMISS_TABLE:	 //解散桌子
	{
		//校验参数
		ASSERT(wDataSize == sizeof(CMD_BS_C_DismissTable));
		if (wDataSize != sizeof(CMD_BS_C_DismissTable)) return false;

		//提取数据
		CMD_BS_C_DismissTable* pDismissTable = (CMD_BS_C_DismissTable*)pData;
		if (pDismissTable->wTableID >= m_TableFrameArray.GetCount()) return false;

		//网络信息
		auto pSocketInfo = &pDismissTable->SocketInfo;

		//获取桌子
		auto pTableFrame = (CTableFrameBattle*)m_TableFrameArray[pDismissTable->wTableID];
		if (pTableFrame->GetRelationParameter()->dwRelationNum != pDismissTable->dwBattleNum)
		{
			//构造结构
			CMD_BS_S_DismissTable DismissBattleTable;
			ZeroMemory(&DismissBattleTable, sizeof(DismissBattleTable));

			//设置变量
			DismissBattleTable.cbResultCode = 1;
			DismissBattleTable.dwBattleNum = pDismissTable->dwBattleNum;
			CopyMemory(&DismissBattleTable.SocketInfo, pSocketInfo, sizeof(DismissBattleTable.SocketInfo));
			StringCchCopy(DismissBattleTable.szDescribeString, CountArray(DismissBattleTable.szDescribeString), CWHService::FormatMessage(MIM_SERVER_0047, TEXT("房间已被解散！")));

			//发送数据
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_DISMISS_TABLE, &DismissBattleTable, sizeof(DismissBattleTable), 0);

			return true;
		}		

		//解散桌子
		pTableFrame->DismissBattleTable(pDismissTable->dwUserID);

		//个人解散
		if (pSocketInfo->dwTokenID != 0 && pSocketInfo->wGateID != 0)
		{
			//构造结构
			CMD_BS_S_DismissTable DismissBattleTable;
			ZeroMemory(&DismissBattleTable, sizeof(DismissBattleTable));

			//设置变量			
			DismissBattleTable.dwBattleNum = pDismissTable->dwBattleNum;
			CopyMemory(&DismissBattleTable.SocketInfo, pSocketInfo, sizeof(DismissBattleTable.SocketInfo));			

			//发送数据
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_DISMISS_TABLE, &DismissBattleTable, sizeof(DismissBattleTable), 0);
		}

		return true;
	}
	case SUB_BS_C_MODIFY_CLUB_SCORE:	//查询积分
	{
		//校验参数
		ASSERT (wDataSize==sizeof(CMD_BS_C_ModifyClubScore));
		if (wDataSize != sizeof(CMD_BS_C_ModifyClubScore)) return false;

		//提取数据
		auto pModifyClubScore = (CMD_BS_C_ModifyClubScore*)pData;

		//查找用户
		auto pIServerUserItem = m_ExtendServiceParameter.pIServerUserManager->SearchUserItem(pModifyClubScore->dwUserID);
		if (pIServerUserItem != NULL && pIServerUserItem->GetScoreKind() == SCORE_KIND_CLUB)
		{
			//锁定信息
			tagLockScoreInfo LockScoreInfo;
			ZeroMemory(&LockScoreInfo, sizeof(LockScoreInfo));
			LockScoreInfo.lMaxLockScore = m_ExtendServiceParameter.pGameServiceOption->lMaxLockScore;
			LockScoreInfo.lLessLockScore = m_ExtendServiceParameter.pGameServiceOption->lLessLockScore;
			LockScoreInfo.lRestrictScore = m_ExtendServiceParameter.pGameServiceOption->lRestrictScore;

			//修改积分
			pIServerUserItem->ModifyUserScore(pModifyClubScore->lChangeScore, LockScoreInfo);

			//发送分数
			m_ExtendServiceParameter.pIMainServiceFrame->SendUserScore(pIServerUserItem);

			//更新分数
			OnEventUserItemScore(pIServerUserItem);
		}	

		return true;
	}
	}

	return true;
}

//命令消息
bool CBattleServiceManager::OnEventNetworkRead(WORD wSubCmdID, VOID * pData, WORD wDataSize, IServerUserItem * pIServerUserItem, DWORD dwSocketID,DWORD dwTokenID)
{
	switch (wSubCmdID)
	{
	case SUB_GR_DISMISS_TABLE:			//解散桌子
	{
		//校验参数
		ASSERT(wDataSize == sizeof(CMD_GR_DismissTable));
		if (wDataSize != sizeof(CMD_GR_DismissTable)) return false;

		//提取数据
		CMD_GR_DismissTable * pDismissTable=(CMD_GR_DismissTable *)pData;
		if (pDismissTable->wTableID>=m_TableFrameArray.GetCount()) return false;

		//获取桌子
		auto pTableFrame= (CTableFrameBattle*)m_TableFrameArray[pDismissTable->wTableID];
		if (pTableFrame->GetRelationParameter()->dwRelationNum != pDismissTable->dwBattleNum) return false;

		//桌子信息
		tagBattleTableItemEx TableItem;
		pTableFrame->GetBattleTableItem(TableItem);

		//用户信息
		DWORD dwUserID = pIServerUserItem->GetUserID();

		//变量定义
		CMD_GR_DismissResult DismissBattleTable;
		ZeroMemory(&DismissBattleTable, sizeof(DismissBattleTable));

		//校验身份
		if (dwUserID != TableItem.dwOwnerID)
		{
			//构造结构
			DismissBattleTable.cbResultCode = FALSE;
			StringCchCopy(DismissBattleTable.szDescribeString, CountArray(DismissBattleTable.szDescribeString), CWHService::FormatMessage(MIM_SERVER_0049,TEXT("您没有权限解散房间!")));
		}
		else
		{
			//约战结束
			if (pTableFrame->DismissBattleTable(dwUserID) == true)
			{
				//构造结构
				DismissBattleTable.cbResultCode = TRUE;
				StringCchCopy(DismissBattleTable.szDescribeString, CountArray(DismissBattleTable.szDescribeString), CWHService::FormatMessage(MIM_SERVER_0050, TEXT("房间解散成功!")) );
			}
			else
			{
				//构造结构
				DismissBattleTable.cbResultCode = FALSE;
				StringCchCopy(DismissBattleTable.szDescribeString, CountArray(DismissBattleTable.szDescribeString), CWHService::FormatMessage(MIM_SERVER_0051, TEXT("房间解散失败!")));
			}
		}

		//发送数据	
		WORD wStringSize=CountStringBuffer(DismissBattleTable.szDescribeString);
		WORD wSendSize=sizeof(DismissBattleTable)-sizeof(DismissBattleTable.szDescribeString)+wStringSize;
		m_ExtendServiceParameter.pIMainServiceFrame->SendDataToUser(pIServerUserItem,MDM_GR_BATTLE, SUB_GR_DISMISS_RESULT,&DismissBattleTable,wSendSize);

		return true;
	}
	}
	return false;
}

//用户登录
bool CBattleServiceManager::OnEventUserLogon(IServerUserItem* pIServerUserItem)
{
	return true;
}

//用户登出
bool CBattleServiceManager::OnEventUserLogout(IServerUserItem* pIServerUserItem)
{
	return true;
}

//用户积分
bool CBattleServiceManager::OnEventUserItemScore(IServerUserItem* pIServerUserItem)
{
	//参数校验
	ASSERT (pIServerUserItem!=NULL);
	if (pIServerUserItem == NULL) return false;

	//茶社积分
	if (pIServerUserItem->GetScoreKind() == SCORE_KIND_CLUB)
	{
		WORD wTableID = pIServerUserItem->GetTableID();
		if (wTableID < m_TableFrameArray.GetCount())
		{
			//获取桌子
			auto pTableFrame = (CTableFrameBattle*)m_TableFrameArray[wTableID];

			//约战信息
			tagBattleTableItemEx TableItem;
			pTableFrame->GetBattleTableItem(TableItem);

			//构造结构
			CMD_BS_S_ClubScore ClubScore = {};

			//设置变量
			ClubScore.dwClubID = TableItem.dwClubID;
			ClubScore.dwBattleNum = TableItem.dwBattleNum;
			ClubScore.dwUserID = pIServerUserItem->GetUserID();			
			ClubScore.lClubScore = pIServerUserItem->GetUserScore();

			//发送数据
			m_ExtendServiceParameter.pITCPSocketService->SendData(MDM_BS_BATTLE_SERVICE, SUB_BS_S_CLUB_SCORE, &ClubScore, sizeof(ClubScore), 0);
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////

//组件创建函数
DECLARE_CREATE_MODULE(BattleServiceManager);

////////////////////////////////////////////////////////////////////////
