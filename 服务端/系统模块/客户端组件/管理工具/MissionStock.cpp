#include "StdAfx.h"
#include "MissionStock.h"
#include "PlatformFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////////////

//激活任务
#define IDI_AVTIVE_MISSION				100								//激活任务
#define TIME_AVTIVE_MISSION				600000L							//激活任务

//////////////////////////////////////////////////////////////////////////////////
//静态变量

CMissionStock * CMissionStock::m_pMissionStock=NULL;					//对象指针

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMissionStock, CWnd)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CMissionStock::CMissionStock()
{
	//存储变量
	m_wDataSize=0;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	//设置变量
	m_StockAction=SAction_NULL;
	m_pIStockOperateCenter=NULL;	

	//设置对象
	if (m_pMissionStock==NULL) m_pMissionStock=this;

	return;
}

//析构函数
CMissionStock::~CMissionStock()
{
	//释放对象
	ASSERT(m_pMissionStock==this);
	if (m_pMissionStock==this) m_pMissionStock=NULL;

	return;
}

//连接事件
bool CMissionStock::OnEventMissionLink(INT nErrorCode)
{
	//错误判断
	if (nErrorCode!=0L)
	{
		//创建窗口
		if (m_hWnd==NULL)
		{
			CRect rcCreate(0,0,0,0);
			Create(NULL,NULL,WS_CHILD,rcCreate,GetDesktopWindow(),100);
		}

		//设置控件
		if(m_pIStockOperateCenter!=NULL)
		{
			m_pIStockOperateCenter->OnEventEnableControls(TRUE);
		}

		//显示提示
		CInformation Information(CPlatformFrame::GetInstance());
		Information.ShowMessageBox(TEXT("与服务器的连接已经断开,请重新登录！"),MB_ICONERROR);
	}
	else
	{
		return true;
	}

	return true;
}

//关闭事件
bool CMissionStock::OnEventMissionShut(BYTE cbShutReason)
{
	//设置变量
	m_wDataSize=0;
	m_StockAction=SAction_NULL;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	//设置控件
	if(m_pIStockOperateCenter!=NULL)
	{
		m_pIStockOperateCenter->OnEventEnableControls(TRUE);
	}

	return true;
}

//读取事件
bool CMissionStock::OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	WORD wCmd = LOCMMD(Command.wMainCmdID);
	if (wCmd ==MDM_GP_STOCK_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_STOCK_OPERATE_RESULT:		//操作结果
			{
				//参数校验
				ASSERT(wDataSize<=sizeof(CMD_GP_StockOperateResult));
				if(wDataSize>sizeof(CMD_GP_StockOperateResult)) return false;

				//提取变量
				auto pStockOperateResult = (CMD_GP_StockOperateResult*)pData;
				
				//接口回调
				if(m_pIStockOperateCenter!=NULL)
				{
					m_pIStockOperateCenter->OnEventEnableControls(TRUE);
					m_pIStockOperateCenter->OnEventStockOperateResult(pStockOperateResult->wSubCommdID, pStockOperateResult->wStockCount, pStockOperateResult->StockList);
				}

				break;
			}
		case SUB_GP_OPERATE_FAILURE:	//操作失败
			{
				//参数校验
				ASSERT(sizeof(CMD_GP_OperateFailure)>=wDataSize);
				if(sizeof(CMD_GP_OperateFailure)<wDataSize) return false;

				//提取数据
				CMD_GP_OperateFailure * pOperateFailure = (CMD_GP_OperateFailure *)pData;

				//控件设置
				if(m_pIStockOperateCenter!=NULL)
				{
					m_pIStockOperateCenter->OnEventEnableControls(TRUE);
				}

				//弹出提示
				CInformation Information;
				Information.ShowMessageBox(pOperateFailure->szDescribeString,MB_ICONERROR);

				break;
		}
		}
		
		CMissionManager* pMissionManager = GetMissionManager();
		pMissionManager->ConcludeMissionItem(this);
		return true;
	}
	else if (wCmd == MDM_GG_GATE_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
			case SUB_GG_S_LINK_SUCCESS:		//接入服务器
			{
				//获取参数
				if (m_StockAction == SAction_QueryStockInfo)
				{
					//发送数据
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_STOCK_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_QUERY_STOCK_INFO, m_cbDataBuffer, m_wDataSize);
				}

				//添加参数
				if (m_StockAction == SAction_AppendStockInfo)
				{
					//发送数据
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_STOCK_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_APPEND_STOCK_INFO, m_cbDataBuffer, m_wDataSize);
				}

				//修改参数
				if (m_StockAction == SAction_ModifyStockInfo)
				{
					//发送数据
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_STOCK_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_MODIFY_STOCK_INFO, m_cbDataBuffer, m_wDataSize);
				}

				//删除参数
				if (m_StockAction == SAction_DeleteStockInfo)
				{
					//发送数据
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_STOCK_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_DELETE_STOCK_INFO, m_cbDataBuffer, m_wDataSize);
				}

				//调整库存
				if (m_StockAction == SAction_AdjustStockScore)
				{
					//发送数据
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_STOCK_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_ADJUST_STOCK_SCORE, m_cbDataBuffer, m_wDataSize);
				}

				//调整彩金
				if (m_StockAction == SAction_AdjustJackpotScore)
				{
					//发送数据
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_STOCK_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_ADJUST_JACKPOT_SCORE, m_cbDataBuffer, m_wDataSize);
				}

				//设置变量
				m_wDataSize = 0;
				m_StockAction = SAction_NULL;
				ZeroMemory(m_cbDataBuffer, sizeof(m_cbDataBuffer));

				return true;
			}
			case SUB_GG_S_LINK_FAILURE:
			{

			}
			case SUB_GG_S_LINK_SHUTDOWN:
			{

			}
		}
	}

	return true;
}

//设置接口
VOID CMissionStock::SetStockOperateCenter(IStockOperateCenter * pIStockOperateCenter)
{
	//参数校验
	ASSERT(pIStockOperateCenter !=NULL);

	//设置接口
	m_pIStockOperateCenter= pIStockOperateCenter;
}

//获取参数
VOID CMissionStock::QueryStockInfo(LPCTSTR pszKindIDList)
{
	//获取对象
	auto pQueryParameter = (CMD_GP_QueryStockInfo*)m_cbDataBuffer;

	//设置变量
	m_StockAction=SAction_QueryStockInfo;
	m_wDataSize=sizeof(CMD_GP_QueryStockInfo);
	StringCchCopy(pQueryParameter->szKindID, CountArray(pQueryParameter->szKindID) ,pszKindIDList);

	//激活任务
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//设置控件
	if(m_pIStockOperateCenter!=NULL)
	{
		m_pIStockOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//删除参数
VOID CMissionStock::DeleteStockInfo(WORD wStockID)
{
	//获取对象
	auto pDeleteStockInfo = (CMD_GP_DeleteStockInfo*)m_cbDataBuffer;

	//设置变量
	pDeleteStockInfo->wStockID = wStockID;
	m_StockAction=SAction_DeleteStockInfo;
	m_wDataSize=sizeof(CMD_GP_DeleteStockInfo);

	//激活任务
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//设置控件
	if(m_pIStockOperateCenter!=NULL)
	{
		m_pIStockOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//修改参数
VOID CMissionStock::ModifyStockInfo(tagStockInfo* pStockInfo)
{
	//获取对象
	auto pModifyStockInfo = (CMD_GP_ModifyStockInfo*)m_cbDataBuffer;

	//设置变量
	m_StockAction=SAction_ModifyStockInfo;
	m_wDataSize=sizeof(CMD_GP_ModifyStockInfo);
	CopyMemory(&pModifyStockInfo->StockInfo, pStockInfo,sizeof(pModifyStockInfo->StockInfo));

	//激活任务
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//设置控件
	if(m_pIStockOperateCenter!=NULL)
	{
		m_pIStockOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//添加参数
VOID CMissionStock::AppendStockInfo(tagStockInfo* pStockInfo)
{
	//获取对象
	auto pAppendStockInfo = (CMD_GP_AppendStockInfo*)m_cbDataBuffer;

	//设置变量
	m_StockAction=SAction_AppendStockInfo;
	m_wDataSize=sizeof(CMD_GP_AppendStockInfo);
	CopyMemory(&pAppendStockInfo->StockInfo, pStockInfo,sizeof(pAppendStockInfo->StockInfo));

	//激活任务
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//设置控件
	if(m_pIStockOperateCenter!=NULL)
	{
		m_pIStockOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//调整库存
VOID CMissionStock::AdjustStockScore(WORD wStockID, SCORE lChangeScore)
{
	//获取对象
	auto pAdjustStockScore = (CMD_GP_AdjustStockScore*)m_cbDataBuffer;

	//设置变量
	m_StockAction = SAction_AdjustStockScore;
	m_wDataSize = sizeof(CMD_GP_AdjustStockScore);

	//设置变量
	pAdjustStockScore->wStockID = wStockID;
	pAdjustStockScore->lChangeScore = lChangeScore;

	//激活任务
	CMissionManager* pMissionManager = GetMissionManager();
	if (pMissionManager != NULL) pMissionManager->AvtiveMissionItem(this, false);

	//设置控件
	if (m_pIStockOperateCenter != NULL)
	{
		m_pIStockOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//调整彩金
VOID CMissionStock::AdjustJackpotScore(WORD wStockID, WORD wLevelID, SCORE lChangeScore)
{
	//获取对象
	auto pAdjustJackpotScore = (CMD_GP_AdjustJackpotScore*)m_cbDataBuffer;

	//设置变量
	m_StockAction = SAction_AdjustJackpotScore;
	m_wDataSize = sizeof(CMD_GP_AdjustJackpotScore);

	//设置变量
	pAdjustJackpotScore->wStockID = wStockID;
	pAdjustJackpotScore->wLevelID = wLevelID;
	pAdjustJackpotScore->lChangeScore = lChangeScore;

	//激活任务
	CMissionManager* pMissionManager = GetMissionManager();
	if (pMissionManager != NULL) pMissionManager->AvtiveMissionItem(this, false);

	//设置控件
	if (m_pIStockOperateCenter != NULL)
	{
		m_pIStockOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////
