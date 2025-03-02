#include "StdAfx.h"
#include "MissionRobot.h"
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

CMissionRobot * CMissionRobot::m_pMissionRobot=NULL;						//对象指针

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMissionRobot, CWnd)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CMissionRobot::CMissionRobot()
{
	//存储变量
	m_wDataSize=0;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	//设置变量
	m_RobotAction = RAction_NULL;
	m_pIRobotOperateCenter=NULL;	

	//设置对象
	if (m_pMissionRobot==NULL) m_pMissionRobot=this;

	return;
}

//析构函数
CMissionRobot::~CMissionRobot()
{
	//释放对象
	ASSERT(m_pMissionRobot==this);
	if (m_pMissionRobot==this) m_pMissionRobot=NULL;

	return;
}

//连接事件
bool CMissionRobot::OnEventMissionLink(INT nErrorCode)
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
		if(m_pIRobotOperateCenter!=NULL)
		{
			m_pIRobotOperateCenter->OnEventEnableControls(TRUE);
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
bool CMissionRobot::OnEventMissionShut(BYTE cbShutReason)
{
	//设置变量
	m_wDataSize=0;
	m_RobotAction=RAction_NULL;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	//设置控件
	if(m_pIRobotOperateCenter!=NULL)
	{
		m_pIRobotOperateCenter->OnEventEnableControls(TRUE);
	}

	return true;
}

//读取事件
bool CMissionRobot::OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	WORD wCmd = LOCMMD(Command.wMainCmdID);
	if (wCmd ==MDM_GP_ROBOT_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_ROBOT_OPERATE_RESULT:		//操作结果
			{
				//参数校验
				ASSERT(wDataSize<=sizeof(CMD_GP_RobotOperateResult));
				if(wDataSize>sizeof(CMD_GP_RobotOperateResult)) return false;

				//提取变量
				auto pRobotOperateResult = (CMD_GP_RobotOperateResult*)pData;
				
				//接口回调
				if(m_pIRobotOperateCenter!=NULL)
				{
					m_pIRobotOperateCenter->OnEventEnableControls(TRUE);
					m_pIRobotOperateCenter->OnEventRobotOperateResult(pRobotOperateResult->wSubCommdID, pRobotOperateResult->wParameterCount, (pRobotOperateResult->RobotParameter));
				}

				break;
			}
		case SUB_GP_OPERATE_FAILURE:		//操作失败
			{
				//参数校验
				ASSERT(sizeof(CMD_GP_OperateFailure)>=wDataSize);
				if(sizeof(CMD_GP_OperateFailure)<wDataSize) return false;

				//提取数据
				CMD_GP_OperateFailure * pOperateFailure = (CMD_GP_OperateFailure *)pData;

				//控件设置
				if(m_pIRobotOperateCenter!=NULL)
				{
					m_pIRobotOperateCenter->OnEventEnableControls(TRUE);
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
				if (m_RobotAction == RAction_QueryParameter)
				{
					//发送数据
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_ROBOT_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_QUERY_PARAMETER, m_cbDataBuffer, m_wDataSize);
				}

				//添加参数
				if (m_RobotAction == RAction_AppendParameter)
				{
					//发送数据
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_ROBOT_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_APPEND_PARAMETER, m_cbDataBuffer, m_wDataSize);
				}

				//修改参数
				if (m_RobotAction == RAction_ModifyParameter)
				{
					//发送数据
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_ROBOT_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_MODIFY_PARAMETER, m_cbDataBuffer, m_wDataSize);
				}

				//删除参数
				if (m_RobotAction == RAction_DeleteParameter)
				{
					//发送数据
					ASSERT(GetMissionManager() != NULL);
					GetMissionManager()->SendData(MAKECMMD(MDM_GP_ROBOT_SERVICE, SERVICE_MODULE_LOGON), SUB_GP_DELETE_PARAMETER, m_cbDataBuffer, m_wDataSize);
				}

				//设置变量
				m_wDataSize = 0;
				m_RobotAction = RAction_NULL;
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
VOID CMissionRobot::SetRobotOperateCenter(IRobotOperateCenter * pIRobotOperateCenter)
{
	//参数校验
	ASSERT(pIRobotOperateCenter!=NULL);

	//设置接口
	m_pIRobotOperateCenter=pIRobotOperateCenter;
}

//获取参数
VOID CMissionRobot::GetRobotParameter(WORD wServerID)
{
	//获取对象
	auto pGetParameter = (CMD_GP_QueryParameter *)m_cbDataBuffer;

	//设置变量
	pGetParameter->wServerID=wServerID;
	m_RobotAction=RAction_QueryParameter;
	m_wDataSize=sizeof(CMD_GP_QueryParameter);

	//激活任务
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//设置控件
	if(m_pIRobotOperateCenter!=NULL)
	{
		m_pIRobotOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//删除参数
VOID CMissionRobot::DeleteRobotParameter(WORD wServerID, DWORD dwBatchID)
{
	//获取对象
	CMD_GP_DeleteParameter * pDeleteParameter = (CMD_GP_DeleteParameter *)m_cbDataBuffer;

	//设置变量
	pDeleteParameter->wServerID=wServerID;
	pDeleteParameter->dwBatchID = dwBatchID;
	m_RobotAction=RAction_DeleteParameter;
	m_wDataSize=sizeof(CMD_GP_DeleteParameter);

	//激活任务
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//设置控件
	if(m_pIRobotOperateCenter!=NULL)
	{
		m_pIRobotOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//修改参数
VOID CMissionRobot::ModifyRobotParameter(WORD wServerID,tagRobotParameter * pRobotParameter)
{
	//获取对象
	CMD_GP_ModifyParameter * pModifyParameter = (CMD_GP_ModifyParameter *)m_cbDataBuffer;

	//设置变量
	m_RobotAction=RAction_ModifyParameter;
	m_wDataSize=sizeof(CMD_GP_ModifyParameter);
	pModifyParameter->wServerID=wServerID;
	CopyMemory(&pModifyParameter->RobotParameter,pRobotParameter,sizeof(tagRobotParameter));

	//激活任务
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//设置控件
	if(m_pIRobotOperateCenter!=NULL)
	{
		m_pIRobotOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//添加参数
VOID CMissionRobot::AddRobotParameter(WORD wServerID,tagRobotParameter* pRobotParameter)
{
	//获取对象
	auto pAddParameter = (CMD_GP_AppendParameter *)m_cbDataBuffer;

	//设置变量
	pAddParameter->wServerID=wServerID;
	m_RobotAction=RAction_AppendParameter;
	m_wDataSize=sizeof(CMD_GP_AppendParameter);
	CopyMemory(&pAddParameter->RobotParameter,pRobotParameter,sizeof(tagRobotParameter));

	//激活任务
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//设置控件
	if(m_pIRobotOperateCenter!=NULL)
	{
		m_pIRobotOperateCenter->OnEventEnableControls(FALSE);
	}

	return;
}

//////////////////////////////////////////////////////////////////////////////////
