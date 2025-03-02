#include "StdAfx.h"
#include "MissionMaster.h"
#include "PlatformFrame.h"

//////////////////////////////////////////////////////////////////////////////////

//激活任务
#define IDI_AVTIVE_MISSION				100								//激活任务
#define TIME_AVTIVE_MISSION				600000L							//激活任务

//////////////////////////////////////////////////////////////////////////////////
//静态变量

CMissionMaster * CMissionMaster::m_pMissionMaster=NULL;					//对象指针

//////////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CMissionMaster, CWnd)
END_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////////////

//构造函数
CMissionMaster::CMissionMaster()
{
	//存储变量
	m_wDataSize=0;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	//设置变量
	m_pIManagerOperateCenter=NULL;	

	//设置对象
	ASSERT(m_pMissionMaster==NULL);
	if (m_pMissionMaster==NULL) m_pMissionMaster=this;

	return;
}

//析构函数
CMissionMaster::~CMissionMaster()
{
	//释放对象
	ASSERT(m_pMissionMaster==this);
	if (m_pMissionMaster==this) m_pMissionMaster=NULL;

	return;
}

//连接事件
bool CMissionMaster::OnEventMissionLink(INT nErrorCode)
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
		if(m_pIManagerOperateCenter!=NULL)
		{
			m_pIManagerOperateCenter->OnEventEnableControls(TRUE);
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
bool CMissionMaster::OnEventMissionShut(LONG cbShutReason)
{
	//设置变量
	m_wDataSize=0;
	ZeroMemory(m_cbDataBuffer,sizeof(m_cbDataBuffer));

	//设置控件
	m_pIManagerOperateCenter->OnEventEnableControls(TRUE);	

	return true;
}

//读取事件
bool CMissionMaster::OnEventMissionRead(CMD_Command Command, VOID * pData, WORD wDataSize)
{
	WORD wCmd = LOCMMD(Command.wMainCmdID);
	if (wCmd==MDM_GP_MANAGER_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
		case SUB_GP_MANAGE_SUCCESS:
			{
				//设置控件
				m_pIManagerOperateCenter->OnEventEnableControls(TRUE);	

				//关闭任务
				CMissionManager * pMissionManager=GetMissionManager();
				if (pMissionManager!=NULL) pMissionManager->ConcludeMissionItem(this);

				//消息提示
				CInformation Information;
				Information.ShowMessageBox(TEXT("操作成功！"));
			}
		}
	}
	else if (wCmd == MDM_GG_GATE_SERVICE)
	{
		switch (Command.wSubCmdID)
		{
			case SUB_GG_S_LINK_SUCCESS:		//接入服务器
			{
				//发送数据
				ASSERT(GetMissionManager() != NULL);
				GetMissionManager()->SendData(MAKECMMD(MDM_GP_MANAGER_SERVICE, SERVICE_MODULE_LOGON), 0, m_cbDataBuffer, m_wDataSize);

				//设置变量
				m_wDataSize = 0;
				ZeroMemory(m_cbDataBuffer, sizeof(m_cbDataBuffer));
			}
		}
	}

	return true;
}

//设置接口
VOID CMissionMaster::SetManagerOperateCenter(IManagerOperateCenter * pIManagerOperateCenter)
{
	//参数校验
	ASSERT(pIManagerOperateCenter!=NULL);

	//设置接口
	m_pIManagerOperateCenter=pIManagerOperateCenter;
}

//激活任务
bool CMissionMaster::ActiveMission()
{
	//状态判断
	ASSERT (m_pIManagerOperateCenter!=NULL);
	if (m_pIManagerOperateCenter==NULL) return false;

	//构造数据
	m_wDataSize=m_pIManagerOperateCenter->ConstructDataPacket(m_cbDataBuffer,sizeof(m_cbDataBuffer));
	if (m_wDataSize==0) return false;

	//激活任务
	CMissionManager * pMissionManager=GetMissionManager();
	if (pMissionManager!=NULL) pMissionManager->AvtiveMissionItem(this,false);

	//设置控件
	m_pIManagerOperateCenter->OnEventEnableControls(FALSE);	

	return true;
}
//////////////////////////////////////////////////////////////////////////////////
